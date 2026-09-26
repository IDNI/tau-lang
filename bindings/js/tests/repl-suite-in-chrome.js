#!/usr/bin/env node

// Replays the node build's test_repl-* ctest cases inside the browser REPL page
// (js/repl's tau_repl_web.js) in headless Chrome, applying each case's own
// PASS/FAIL regexes.
//
// Usage: node repl-suite-in-chrome.js <page-build-dir> [tests-build-dir]
// [--concurrency=N] [--limit=N]. tests-build-dir defaults to build/release-wasm.
//
// The page keeps its own build directory because its link settings conflict
// with the node targets, so the case list and the page are two directories.
//
// A case the page cannot replay is skipped, never failed: no `-e`, echo or
// printf input to extract, an extra CLI flag, or the TAU_BROWSER_SKIP property
// tests/repl/add_repl_test.cmake sets for hostfs and ltlsynt cases.
//
// The regexes match the rendered xterm buffer, which the runner widens to 1000
// columns first; a marker that wraps mid-pattern cannot be matched.
//
// Requires puppeteer-core and a Chrome binary, as run-in-chrome.js does.

const { execFileSync } = require('child_process');
const fs = require('fs');
const path = require('path');
const {
	DEFAULT_TIMEOUT_MS, loadPuppeteer, resolveChromePath, createCoiServer,
} = require('./browser-harness');

// Chrome refuses to start as root without --no-sandbox, which is how the
// container stages run it. The sandbox stays on everywhere else.
const CHROME_ARGS = (process.env.TAU_CHROME_ARGS || '').split(' ').filter(Boolean);

// Two pages is two wasm heaps plus two shared-memory segments; the REPL page
// is ~20 MB before its heap, so it contends harder than the doctest batch.
const DEFAULT_CONCURRENCY = 2;
const QUIESCE_MS = 1000;
const POLL_MS = 200;
const CTEST_DISCOVERY_TIMEOUT_MS = Number(process.env.TAU_BROWSER_DISCOVERY_TIMEOUT_MS) || 120000;
// One CDP command may not outlive this, so a page whose wasm main thread is
// busy rejects the poll instead of blocking the worker indefinitely.
const PROTOCOL_TIMEOUT_MS = Number(process.env.TAU_BROWSER_PROTOCOL_TIMEOUT_MS) || 300000;
// Below ctest's 7200 s TIMEOUT, so the run reports before ctest kills it.
const SUITE_DEADLINE_MS = Number(process.env.TAU_BROWSER_SUITE_DEADLINE_MS) || 6600000;

function parseArgs(argv) {
	const opts = { pageDir: null, testsDir: null, concurrency: null, limit: null };
	const positional = [];
	for (const a of argv) {
		if (a.startsWith('--concurrency=')) opts.concurrency = Number(a.slice('--concurrency='.length));
		else if (a.startsWith('--tests-dir=')) opts.testsDir = a.slice('--tests-dir='.length);
		else if (a.startsWith('--limit=')) opts.limit = Number(a.slice('--limit='.length));
		else if (a === '--help' || a === '-h') { printUsage(); process.exit(0); }
		else positional.push(a);
	}
	if (positional[0]) opts.pageDir = opts.pageDir || positional[0];
	if (positional[1]) opts.testsDir = opts.testsDir || positional[1];
	return opts;
}

function printUsage() {
	process.stderr.write(
		'usage: repl-suite-in-chrome.js <page-build-dir> [tests-build-dir] '
		+ '[--concurrency=N] [--limit=N]\n');
}

// Unescapes a printf(1) format string as the shell left it in the ctest
// command: the suite writes "\\n" so printf turns it into a newline, and the
// command string this reads still holds the two characters.
function unescapePrintf(s) {
	return s.replace(/\\n/g, '\n').replace(/\\t/g, '\t').replace(/\\\\/g, '\\');
}

// Unescapes \" and \\ inside a double-quoted shell word (the `-e "<cmd>"`
// form). A REPL command's own quotes reach CMake already escaped.
function unescapeQuoted(s) {
	return s.replace(/\\"/g, '"').replace(/\\\\/g, '\\');
}

// The invocation must be only <emulator> tau_repl.js plus exactly <extra>: a
// shell prefix (an env assignment, a redirect) or an extra flag is a CLI the
// page cannot reproduce.
function plainReplTail(tail, extra) {
	const m = tail.match(/^(?:\S+\s+)?\S*tau_repl\.js(?:\s(.*))?$/s);
	return m !== null && (m[1] || '').trim() === extra;
}

// Extracts the REPL input lines a ctest command replays, or null when the
// case needs something a browser page cannot give it (a CLI flag, an env
// variable, a host process). Three forms, matching tests/repl/add_repl_test.cmake:
//
//   echo "<cmd>. q" | <emu><tau>                  -> one line, "<cmd>. q"
//   printf '<line>\n...' | <emu><tau> -X          -> one line per printf line
//   <emu><tau> -e "<cmd>" [-S trace] [2>&1]       -> one line, "<cmd>"
//
// `trace` reproduces -S trace: the page starts at the default (info)
// severity, so the runner sends "set severity trace" first.
function parseReplCase(cmd) {
	let m = cmd.match(/^echo\s+"((?:[^"\\]|\\.)*)"\s*\|\s*(.*)$/s);
	if (m) return plainReplTail(m[2], '')
		? { lines: [unescapeQuoted(m[1])], trace: false } : null;

	m = cmd.match(/^printf\s+'((?:[^'\\]|\\.)*)'\s*\|\s*(.*)$/s);
	if (m) {
		if (!plainReplTail(m[2], '-X')) return null;
		const lines = unescapePrintf(m[1]).split('\n');
		if (lines.length && lines[lines.length - 1] === '') lines.pop();
		return { lines, trace: false };
	}

	m = cmd.match(/^(?:\S+\s+)?\S*tau_repl\.js\s+(.*)$/s);
	if (m) {
		const args = m[1];
		const e = args.match(/-e\s+"((?:[^"\\]|\\.)*)"/s);
		if (!e) return null;
		// Anything left after removing -e/-S trace/2>&1 is a CLI flag with no
		// browser equivalent (--max-fixpoint-steps, --version, a spec file).
		const leftover = args
			.replace(/-e\s+"(?:[^"\\]|\\.)*"/s, '')
			.replace(/-S\s+trace/, '')
			.replace(/\s*2>&1\s*$/, '')
			.trim();
		if (leftover !== '') return null;
		return { lines: [unescapeQuoted(e[1])], trace: /-S\s+trace/.test(args) };
	}
	return null;
}

function asArray(v) {
	if (v === undefined || v === null) return null;
	return Array.isArray(v) ? v : [v];
}

// ctest --show-only=json-v1 is the same discovery run-suite-in-chrome.js uses;
// only `bash -c` commands can carry REPL input, so everything else is left to
// parseReplCase to reject.
function loadReplCases(testsDir) {
	const raw = execFileSync(
		'ctest', ['--test-dir', testsDir, '--show-only=json-v1'],
		{ encoding: 'utf8', maxBuffer: 64 * 1024 * 1024, timeout: CTEST_DISCOVERY_TIMEOUT_MS },
	);
	const data = JSON.parse(raw);
	return data.tests
		.filter((t) => t.name.startsWith('test_repl-'))
		.map((t) => {
			const props = Object.fromEntries((t.properties || []).map((p) => [p.name, p.value]));
			const command = Array.isArray(t.command) ? t.command : [];
			const script = command[0] === 'bash' && command[1] === '-c' ? command[2] : null;
			const timeoutMs = typeof props.TIMEOUT === 'number'
				? Math.round(props.TIMEOUT * 1000)
				: DEFAULT_TIMEOUT_MS;
			const parsed = script === null ? null : parseReplCase(script);
			return {
				name: t.name,
				script,
				parsed,
				pass: asArray(props.PASS_REGULAR_EXPRESSION),
				fail: asArray(props.FAIL_REGULAR_EXPRESSION),
				timeoutMs,
				// Set by tests/repl/add_repl_test.cmake for a case whose
				// REQUIREs the page cannot satisfy (hostfs, ltlsynt).
				browserSkip: props.TAU_BROWSER_SKIP !== undefined,
			};
		});
}

function compileRegexes(patterns) {
	if (!patterns) return null;
	const out = [];
	for (const p of patterns) {
		try { out.push(new RegExp(p)); }
		catch (e) { throw new Error(`invalid ctest regex ${JSON.stringify(p)}: ${e.message}`); }
	}
	return out;
}

// Reads the visible rows. The viewport starts at buffer.viewportY (line 0 is
// the top of the scrollback), so a marker that scrolled off the top is still
// matched.
async function terminalText(page) {
	return page.evaluate(() => {
		const buf = window.term.buffer.active;
		const top = buf.viewportY;
		const lines = [];
		for (let y = 0; y < window.term.rows; y++) {
			const line = buf.getLine(top + y);
			if (line) lines.push(line.translateToString(true));
		}
		return lines.join('\n');
	});
}

function sleep(ms) { return new Promise((r) => setTimeout(r, ms)); }

async function waitForText(page, substring, timeoutMs) {
	const start = Date.now();
	for (;;) {
		const text = await terminalText(page);
		if (text.includes(substring)) return text;
		if (Date.now() - start > timeoutMs) {
			throw new Error(`timed out waiting for ${JSON.stringify(substring)} in the terminal buffer, `
				+ `last seen:\n${text}`);
		}
		await sleep(POLL_MS);
	}
}

async function openPage(browser, baseUrl, timeoutMs) {
	const page = await browser.newPage();
	const errors = [];
	page.on('pageerror', (e) => errors.push(`[page error] ${e}`));
	page.on('console', (msg) => { if (msg.type() === 'error') errors.push(`[console] ${msg.text()}`); });
	await page.goto(baseUrl, { waitUntil: 'load', timeout: timeoutMs });
	await waitForText(page, 'Welcome to the', timeoutMs);
	// The buffer is what the regexes are matched against, so widen the
	// terminal before any output: a 140-column grid wraps long formula output
	// mid-marker. _ftxui_on_resize keeps the program's own layout in step.
	await page.evaluate(() => {
		try { window.term.resize(1000, 200); } catch { /* keep the default grid */ }
		if (window.Module && typeof window.Module._ftxui_on_resize === 'function') {
			try { window.Module._ftxui_on_resize(1000, 200); } catch { /* optional fast path */ }
		}
	});
	return { page, errors };
}

async function sendLine(page, line) {
	// sendReplInput is terminal.js's own stdin writer (the upload path uses
	// it too); typing NUL-free text is a superset of what xterm's onData
	// sends, and it needs no focus.
	await page.evaluate((l) => window.sendReplInput(l + '\r'), line);
}

// Sends every line, then waits for the pass regex -- the same signal ctest
// waits for -- or, for a case with only a FAIL regex, for the buffer to stop
// changing.
async function waitForOutcome(page, c, passRes, timeoutMs) {
	const start = Date.now();
	let text = await terminalText(page);
	if (passRes) {
		while (Date.now() - start < timeoutMs) {
			text = await terminalText(page);
			if (passRes.some((re) => re.test(text))) return { text, matched: true };
			await sleep(POLL_MS);
		}
		return { text, matched: false };
	}
	let prev = text;
	let stableSince = Date.now();
	while (Date.now() - start < timeoutMs) {
		text = await terminalText(page);
		if (text !== prev) { prev = text; stableSince = Date.now(); }
		else if (Date.now() - stableSince > QUIESCE_MS) break;
		await sleep(POLL_MS);
	}
	return { text, matched: true };
}

async function runCase(browser, baseUrl, c, timeoutMs) {
	let page = null;
	let errors = [];
	try {
		const passRes = compileRegexes(c.pass);
		const failRes = compileRegexes(c.fail);
		({ page, errors } = await openPage(browser, baseUrl, DEFAULT_TIMEOUT_MS));
		if (c.parsed.trace) await sendLine(page, 'set severity trace');
		for (const line of c.parsed.lines) await sendLine(page, line);
		const { text, matched } = await waitForOutcome(page, c, passRes, timeoutMs);
		const passOk = passRes ? matched : true;
		const failHit = failRes ? failRes.some((re) => re.test(text)) : false;
		return {
			name: c.name,
			ok: passOk && !failHit,
			reason: !passOk ? `no PASS_REGULAR_EXPRESSION match within ${timeoutMs}ms`
				: failHit ? 'FAIL_REGULAR_EXPRESSION matched'
					: null,
			errors,
			text,
		};
	} catch (e) {
		return { name: c.name, ok: false, reason: e.message || String(e), errors, text: '' };
	} finally {
		if (page) { try { await page.close(); } catch { /* page may already be gone */ } }
	}
}

async function runPool(cases, concurrency, worker) {
	const queue = [...cases];
	const results = [];
	async function drain() {
		for (;;) {
			const c = queue.shift();
			if (!c) return;
			results.push(await worker(c));
		}
	}
	await Promise.all(Array.from({ length: Math.min(concurrency, cases.length) }, drain));
	return results;
}

// Tracks the browser and server so a signal tears them down: a `finally`
// block does not run once the process is killed rather than exiting.
const liveResources = { browser: null, server: null };

async function teardownLiveResources() {
	const { browser, server } = liveResources;
	if (browser) {
		try { await browser.close(); } catch { /* already gone */ }
		const proc = typeof browser.process === 'function' ? browser.process() : null;
		if (proc && proc.exitCode === null) { try { proc.kill('SIGKILL'); } catch { /* already gone */ } }
	}
	if (server) { try { await server.close(); } catch { /* already gone */ } }
}

for (const sig of ['SIGINT', 'SIGTERM']) {
	process.on(sig, () => {
		teardownLiveResources().finally(() => process.exit(sig === 'SIGINT' ? 130 : 143));
	});
}

async function main() {
	const opts = parseArgs(process.argv.slice(2));
	if (!opts.pageDir) { printUsage(); process.exit(2); }
	const pageDir = path.resolve(opts.pageDir);
	const testsDir = path.resolve(opts.testsDir
		|| path.join(__dirname, '..', '..', '..', 'build', 'release-wasm'));
	const concurrency = opts.concurrency
		|| Number(process.env.TAU_BROWSER_TEST_CONCURRENCY) || DEFAULT_CONCURRENCY;

	if (!fs.existsSync(path.join(pageDir, 'index.html'))) {
		process.stderr.write(`${pageDir}/index.html not found -- build it first:\n`
			+ '  ./dev preset release-wasm-repl-browser -DTAU_BUILD_JOBS=4\n');
		process.exit(2);
	}

	let puppeteer;
	let chromePath;
	try {
		puppeteer = loadPuppeteer();
		chromePath = resolveChromePath();
	} catch (e) {
		process.stderr.write(e.message);
		process.exit(2);
	}

	const all = loadReplCases(testsDir);
	const runnable = all.filter((c) => c.parsed !== null && !c.browserSkip);
	const skipped = all.filter((c) => c.parsed === null || c.browserSkip);
	const cases = opts.limit ? runnable.slice(0, opts.limit) : runnable;
	if (all.length === 0) {
		process.stderr.write(`no test_repl-* cases in ${testsDir} `
			+ '(configure the node build first: ./dev preset release-wasm-repl-tests)\n');
		process.exit(2);
	}

	process.stdout.write(
		`${cases.length} case(s) from ${testsDir} replayed in ${pageDir}, `
		+ `concurrency=${concurrency}`
		+ `${opts.limit ? ` (--limit=${opts.limit} of ${runnable.length})` : ''}\n`);

	const { baseUrl, close } = await createCoiServer(pageDir);
	liveResources.server = { close };

	const deadline = Date.now() + SUITE_DEADLINE_MS;
	let browser;
	const start = Date.now();
	let results = [];
	try {
		browser = await puppeteer.launch({
			executablePath: chromePath, headless: true, args: CHROME_ARGS,
			protocolTimeout: PROTOCOL_TIMEOUT_MS,
		});
		liveResources.browser = browser;
		let completed = 0;
		results = await runPool(cases, concurrency, async (c) => {
			const left = deadline - Date.now();
			const r = left <= 0
				? { name: c.name, ok: false, reason: 'suite deadline reached', errors: [], text: '' }
				: await runCase(browser, baseUrl, c, Math.min(c.timeoutMs, left));
			completed += 1;
			process.stdout.write(`[${completed}/${cases.length}] ${r.ok ? 'PASS' : 'FAIL'} ${c.name}\n`);
			if (!r.ok) {
				if (r.reason) process.stdout.write(`    ${r.reason}\n`);
				for (const line of (r.errors || [])) process.stdout.write(`    err| ${line}\n`);
				process.stdout.write(`    buffer| ${(r.text || '').split('\n').join('\n    buffer| ')}\n`);
			}
			return r;
		});
	} finally {
		await teardownLiveResources();
		liveResources.browser = null;
		liveResources.server = null;
	}

	const wallSec = (Date.now() - start) / 1000;
	const failed = results.filter((r) => !r.ok);
	process.stdout.write('\n==== summary ====\n');
	process.stdout.write(
		`total: ${results.length}  passed: ${results.length - failed.length}  `
		+ `failed: ${failed.length}  skipped: ${skipped.length}\n`);
	process.stdout.write(`wall time: ${wallSec.toFixed(1)}s  concurrency: ${concurrency}\n`);
	if (failed.length) {
		process.stdout.write(`FAILED:\n${failed.map((r) => `  - ${r.name}: ${r.reason}`).join('\n')}\n`);
	}
	if (skipped.length) {
		// Not failures: a CLI flag, an env var, a host process or a hostfs
		// mount has no browser REPL equivalent.
		process.stdout.write(`SKIPPED (not replayable in the browser): ${skipped.length}\n`);
		for (const c of skipped.slice(0, 20)) process.stdout.write(`  - ${c.name}\n`);
		if (skipped.length > 20) process.stdout.write(`  ... and ${skipped.length - 20} more\n`);
	}
	process.exit(failed.length > 0 ? 1 : 0);
}

main().catch((e) => {
	console.error(e);
	process.exit(2);
});
