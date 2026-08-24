#!/usr/bin/env node

// Phase 4: the whole ctest suite, in headless Chrome, in one run -- not
// one test as run-in-chrome.js does. Same harness/server/exit-status
// contract (browser-harness.js), just applied to every test ctest knows
// about instead of one passed on the command line.
//
// The test list comes from `ctest --show-only=json-v1`, not a hardcoded
// list, so this tracks whatever the build actually registered (each
// test's command gives its exact <name>.js path, and its own TIMEOUT
// property is honoured rather than one fixed bound -- `ctest --timeout`
// does not override a per-test TIMEOUT, see .local/build-emscripten.md).
//
// One browser for the whole run, one page per test (EXITSTATUS/Module/FS
// state persist on a page, so tests must not share one), with concurrency
// capped (default 4 -- 61 wasm modules, one ~70 MB, contend for memory
// harder in one browser than as 61 node processes). Tests that live in
// different directories (e.g. an ad hoc "--extra" binary outside the
// build dir) each get their own static server; tests sharing a directory
// share one, which is the common case for a real ctest suite.
//
// Usage:
//   node run-suite-in-chrome.js [build-dir] [--concurrency=N]
//       [--extra=<path-to-js-or-name>]...
//
// --extra points at any emscripten MODULARIZE=0 output pair (path with or
// without ".js"), added to the batch alongside the ctest-derived list --
// this is how a scratch/forced-failure binary outside the build dir gets
// exercised through the same batch machinery for a failure-path check.
//
// Requires puppeteer-core and a Chrome binary -- see run-in-chrome.js's
// header for the same requirement.

const { execFileSync } = require('child_process');
const path = require('path');
const {
	DEFAULT_BUILD_DIR, DEFAULT_TIMEOUT_MS, resolveTarget, createServer, loadPuppeteer,
	resolveChromePath, runTest,
} = require('./browser-harness');

// Four concurrent wasm heaps lose renderers on a 20 GB machine, and buy
// nothing: the wall time is set by a few long tests, so 2 measured 311s
// against 4's 322s.
const DEFAULT_CONCURRENCY = 2;

function parseArgs(argv) {
	const opts = { buildDir: null, concurrency: null, extras: [], limit: null };
	const positional = [];
	for (const a of argv) {
		if (a.startsWith('--concurrency=')) opts.concurrency = Number(a.slice('--concurrency='.length));
		else if (a.startsWith('--build-dir=')) opts.buildDir = a.slice('--build-dir='.length);
		else if (a.startsWith('--extra=')) opts.extras.push(a.slice('--extra='.length));
		// Debug aid only, not part of the documented contract: caps the
		// batch to the first N tests so the pool/server plumbing can be
		// checked without waiting on the full suite.
		else if (a.startsWith('--limit=')) opts.limit = Number(a.slice('--limit='.length));
		else if (a === '--help' || a === '-h') { printUsage(); process.exit(0); }
		else positional.push(a);
	}
	if (positional[0]) opts.buildDir = opts.buildDir || positional[0];
	return opts;
}

function printUsage() {
	process.stderr.write(
		'usage: run-suite-in-chrome.js [build-dir] [--concurrency=N] [--extra=<path>]...\n');
}

// Reads ctest's own view of the suite rather than any list this script
// would otherwise have to keep in sync by hand. `command` is
// [node, <path>.js] for every emscripten test (see tests/CMakeLists.txt's
// TEST_RUNNER/TEST_SUFFIX swap) -- the last element is always the script
// path regardless of how the runner argument is spelled.
function loadCtestTests(buildDir) {
	const raw = execFileSync(
		'ctest', ['--test-dir', buildDir, '--show-only=json-v1'],
		{ encoding: 'utf8', maxBuffer: 64 * 1024 * 1024 },
	);
	const data = JSON.parse(raw);
	return data.tests.map((t) => {
		const jsPath = t.command[t.command.length - 1];
		const props = Object.fromEntries((t.properties || []).map((p) => [p.name, p.value]));
		const timeoutMs = typeof props.TIMEOUT === 'number'
			? Math.round(props.TIMEOUT * 1000)
			: DEFAULT_TIMEOUT_MS;
		return { name: t.name, jsPath, timeoutMs };
	// TAU_BUILD_BROWSER_TESTS registers this script itself as the
	// `browser_suite` entry, whose last argument is a build directory rather
	// than a test; without this the batch would try to run itself.
	}).filter((t) => t.jsPath.endsWith('.js'));
}

function loadExtraTests(extraArgs, buildDir) {
	return extraArgs.map((arg) => {
		const { dir, name } = resolveTarget(arg, buildDir);
		return { name, jsPath: path.join(dir, `${name}.js`), timeoutMs: DEFAULT_TIMEOUT_MS };
	});
}

function groupByDir(tests) {
	const map = new Map();
	for (const t of tests) {
		const dir = path.dirname(t.jsPath);
		if (!map.has(dir)) map.set(dir, []);
		map.get(dir).push(t);
	}
	return map;
}

async function runPool(tests, concurrency, worker) {
	const queue = [...tests];
	const results = [];
	async function drain() {
		for (;;) {
			const t = queue.shift();
			if (!t) return;
			results.push(await worker(t));
		}
	}
	await Promise.all(Array.from({ length: Math.min(concurrency, tests.length) }, drain));
	return results;
}

// Tracks whatever the current run has open so a signal handler can tear it
// down -- without this, Ctrl-C (or a harness that times the whole run out
// and sends SIGTERM) leaves Chrome running: `finally` blocks don't get a
// chance to run once the process is killed instead of exiting normally.
const liveResources = { browser: null, servers: null };

async function teardownLiveResources() {
	const { browser, servers } = liveResources;
	if (browser) {
		try { await browser.close(); } catch { /* already gone */ }
		// browser.close() asks Chrome to quit over the CDP connection; if the
		// renderer is wedged (the same failure mode this runner exists to
		// catch) that request can go unanswered, so fall back to killing the
		// OS process directly rather than leaving it behind.
		const proc = typeof browser.process === 'function' ? browser.process() : null;
		if (proc && proc.exitCode === null) { try { proc.kill('SIGKILL'); } catch { /* already gone */ } }
	}
	if (servers) { for (const { close } of servers.values()) { try { await close(); } catch { /* already gone */ } } }
}

for (const sig of ['SIGINT', 'SIGTERM']) {
	process.on(sig, () => {
		teardownLiveResources().finally(() => process.exit(sig === 'SIGINT' ? 130 : 143));
	});
}

async function main() {
	const opts = parseArgs(process.argv.slice(2));
	const buildDir = path.resolve(opts.buildDir || DEFAULT_BUILD_DIR);
	const concurrency = opts.concurrency || Number(process.env.TAU_BROWSER_TEST_CONCURRENCY) || DEFAULT_CONCURRENCY;

	let puppeteer;
	let chromePath;
	try {
		puppeteer = loadPuppeteer();
		chromePath = resolveChromePath();
	} catch (e) {
		process.stderr.write(e.message);
		process.exit(2);
	}

	let ctestTests = loadCtestTests(buildDir);
	if (opts.limit) ctestTests = ctestTests.slice(0, opts.limit);
	const extraTests = loadExtraTests(opts.extras, buildDir);
	const tests = [...ctestTests, ...extraTests];
	if (tests.length === 0) {
		process.stderr.write(`no tests found in ${buildDir} (ctest --show-only reported none)\n`);
		process.exit(2);
	}

	process.stdout.write(
		`${tests.length} test(s) from ${buildDir}${extraTests.length ? ` (+${extraTests.length} extra)` : ''}, `
		+ `concurrency=${concurrency}\n`);

	const dirGroups = groupByDir(tests);
	const servers = new Map();
	for (const dir of dirGroups.keys()) servers.set(dir, await createServer(dir));
	liveResources.servers = servers;

	let browser;
	let browserCrashed = false;
	const start = Date.now();
	let results;
	try {
		browser = await puppeteer.launch({ executablePath: chromePath, headless: true });
		liveResources.browser = browser;
		browser.on('disconnected', () => { browserCrashed = true; });

		let completed = 0;
		results = await runPool(tests, concurrency, async (test) => {
			if (browserCrashed) {
				return {
					...test, exitCode: null, timedOut: false, crashed: true,
					crashInfo: 'browser disconnected before this test could run', stdout: [], stderr: [],
				};
			}
			const { baseUrl } = servers.get(path.dirname(test.jsPath));
			const r = await runTest(browser, { baseUrl, name: test.name, timeoutMs: test.timeoutMs });
			completed += 1;
			const status = r.harnessError ? 'ERROR'
				: r.crashed ? 'CRASH'
				: r.timedOut ? 'TIMEOUT'
				: r.exitCode === 0 ? 'PASS' : 'FAIL';
			process.stdout.write(`[${completed}/${tests.length}] ${status.padEnd(7)} ${test.name}\n`);
			if (status !== 'PASS') {
				for (const line of r.stdout) process.stdout.write(`    out| ${line}\n`);
				for (const line of r.stderr) process.stdout.write(`    err| ${line}\n`);
			}
			return { ...test, ...r };
		});
	} finally {
		await teardownLiveResources();
		liveResources.browser = null;
		liveResources.servers = null;
	}
	const wallSec = (Date.now() - start) / 1000;

	const harnessErrors = results.filter((r) => r.harnessError);
	const crashed = results.filter((r) => !r.harnessError && r.crashed);
	const timedOut = results.filter((r) => !r.harnessError && !r.crashed && r.timedOut);
	const failed = results.filter((r) => !r.harnessError && !r.crashed && !r.timedOut && r.exitCode !== 0);
	const passed = results.filter((r) => !r.harnessError && !r.crashed && !r.timedOut && r.exitCode === 0);

	process.stdout.write('\n==== summary ====\n');
	process.stdout.write(
		`total: ${results.length}  passed: ${passed.length}  failed: ${failed.length}  `
		+ `timed out: ${timedOut.length}  crashed: ${crashed.length}  harness errors: ${harnessErrors.length}\n`);
	process.stdout.write(`wall time: ${wallSec.toFixed(1)}s  concurrency: ${concurrency}\n`);
	if (failed.length) {
		process.stdout.write(`FAILED:\n${failed.map((r) => `  - ${r.name} (exit ${r.exitCode})`).join('\n')}\n`);
	}
	if (timedOut.length) {
		process.stdout.write(
			`TIMED OUT:\n${timedOut.map((r) => `  - ${r.name} (waited ${r.waitElapsedMs}ms, budget ${r.timeoutMs}ms)`).join('\n')}\n`);
	}
	if (crashed.length) {
		process.stdout.write(`CRASHED:\n${crashed.map((r) => `  - ${r.name}: ${r.crashInfo}`).join('\n')}\n`);
	}
	if (harnessErrors.length) {
		process.stdout.write(
			`HARNESS ERRORS:\n${harnessErrors.map((r) => `  - ${r.name}: ${r.harnessError}`).join('\n')}\n`);
	}

	process.exit((failed.length + timedOut.length + crashed.length + harnessErrors.length) > 0 ? 1 : 0);
}

main().catch((e) => {
	console.error(e);
	process.exit(2);
});
