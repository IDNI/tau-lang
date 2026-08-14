#!/usr/bin/env node

// Phase 4 feasibility spike: drives one wasm/doctest binary (Node runs
// them today, see tests/native.js's sibling on the Node side and
// tests/CMakeLists.txt's TEST_RUNNER swap) inside headless Chrome instead,
// and reports the same two things a shell cares about -- the captured
// doctest output and the process exit status.
//
// The harness page, static server and exit-status contract live in
// browser-harness.js, shared with run-suite-in-chrome.js (the whole ctest
// suite) so the two can't drift apart.
//
// Usage:
//   node run-in-chrome.js <test-name-or-path> [build-dir]
//
// <test-name-or-path> is either a bare name looked up as
// <build-dir>/<name>.js (build-dir defaults to build/debug-emscripten-tests)
// or a path (relative/absolute, with or without ".js") to any emscripten
// MODULARIZE=0 output pair <x>.js/<x>.wasm -- used here to point at a
// scratch binary outside the tree for the forced-failure check.
//
// Requires puppeteer-core, installed as a devDependency of
// bindings/js/package.json ("cd bindings/js && npm install"), and a Chrome
// binary, found via $CHROME_PATH or the pinned install "./dev dep-chrome.sh"
// puts under the shared prefix.

const {
	DEFAULT_BUILD_DIR, resolveTarget, createServer, loadPuppeteer, resolveChromePath, runTest,
	DEFAULT_TIMEOUT_MS,
} = require('./browser-harness');
const path = require('path');

// Same rationale as run-suite-in-chrome.js's `liveResources`: a signal
// arriving mid-run skips the `finally` block below, so Ctrl-C would
// otherwise leave the launched Chrome process behind.
let liveBrowser = null;
for (const sig of ['SIGINT', 'SIGTERM']) {
	process.on(sig, () => {
		(liveBrowser ? liveBrowser.close().catch(() => {}) : Promise.resolve())
			.finally(() => process.exit(sig === 'SIGINT' ? 130 : 143));
	});
}

async function main() {
	const [, , targetArg, buildDirArg] = process.argv;
	if (!targetArg) {
		process.stderr.write('usage: run-in-chrome.js <test-name-or-path> [build-dir]\n');
		process.exit(2);
	}
	const buildDir = buildDirArg ? path.resolve(buildDirArg) : DEFAULT_BUILD_DIR;
	const { dir, name } = resolveTarget(targetArg, buildDir);

	let puppeteer;
	let chromePath;
	try {
		puppeteer = loadPuppeteer();
		chromePath = resolveChromePath();
	} catch (e) {
		process.stderr.write(e.message);
		process.exit(2);
	}

	const { close, baseUrl } = await createServer(dir);
	let browser;
	let exitCode = 1;
	try {
		browser = await puppeteer.launch({ executablePath: chromePath, headless: true });
		liveBrowser = browser;
		const result = await runTest(browser, { baseUrl, name, timeoutMs: DEFAULT_TIMEOUT_MS });
		for (const line of result.stdout) process.stdout.write(`${line}\n`);
		for (const line of result.stderr) process.stderr.write(`${line}\n`);
		if (result.harnessError) {
			process.stderr.write(`[harness error] ${result.harnessError}\n`);
			exitCode = 1;
		} else if (result.crashed) {
			process.stderr.write(`[crash] ${result.crashInfo}\n`);
			exitCode = 1;
		} else if (result.timedOut) {
			process.stderr.write(`[timeout] waited ${result.waitElapsedMs}ms (budget ${DEFAULT_TIMEOUT_MS}ms)\n`);
			exitCode = 1;
		} else {
			exitCode = result.exitCode;
		}
	} finally {
		if (browser) await browser.close();
		liveBrowser = null;
		await close();
	}
	process.exit(exitCode);
}

main();
