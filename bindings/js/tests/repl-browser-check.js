#!/usr/bin/env node

// Proves the browser REPL page (js/repl/CMakeLists.txt's output: index.html,
// tau_repl_web.js/.wasm, vendor/) starts and accepts input in headless Chrome
// end to end, not only that the wasm module links and runs under Node.
//
// loadPuppeteer/resolveChromePath/createCoiServer come from browser-harness.js
// so this script, the doctest harness and the REPL suite runner agree on where
// Chrome comes from and how the page is served. createCoiServer routes the
// REPL page's own asset set, which createServer's doctest routing does not.
//
// Usage:
//   node repl-browser-check.js [build-dir]
//
// build-dir defaults to build/release-wasm-repl-browser (the
// release-wasm-repl-browser preset's binary dir) and must contain index.html,
// tau_repl_web.js/.wasm and vendor/; a broken copy exercises the failure path.
//
// Exits 0 only if every step below succeeds, non-zero otherwise:
//   - the page loads and window.crossOriginIsolated is true
//   - the welcome banner appears in the xterm terminal buffer
//   - typing "help" + Enter produces new terminal output

const fs = require('fs');
const path = require('path');
const { createCoiServer, loadPuppeteer, resolveChromePath } = require('./browser-harness');

const DEFAULT_BUILD_DIR = path.join(__dirname, '..', '..', '..', 'build', 'release-wasm-repl-browser');
const TIMEOUT_MS = Number(process.env.TAU_REPL_CHECK_TIMEOUT_MS) || 30000;


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

async function waitForText(page, substring, timeoutMs) {
	const start = Date.now();
	for (;;) {
		const text = await terminalText(page);
		if (text.includes(substring)) return text;
		if (Date.now() - start > timeoutMs) {
			throw new Error(`timed out after ${timeoutMs}ms waiting for ${JSON.stringify(substring)} in terminal buffer, `
				+ `last seen:\n${text}`);
		}
		await new Promise((r) => setTimeout(r, 200));
	}
}

// Waits for the buffer to differ from a baseline snapshot -- used after
// sending "help", whose own name is already substring-present in the
// welcome banner's "type \"help\"..." line, so waitForText(page, "help", …)
// would return immediately on the pre-existing banner text instead of the
// command's actual output.
async function waitForChange(page, baseline, timeoutMs) {
	const start = Date.now();
	for (;;) {
		const text = await terminalText(page);
		if (text !== baseline) return text;
		if (Date.now() - start > timeoutMs) {
			throw new Error(`timed out after ${timeoutMs}ms waiting for the terminal buffer to change, `
				+ `still:\n${text}`);
		}
		await new Promise((r) => setTimeout(r, 200));
	}
}

// Same signal-cleanup shape as run-in-chrome.js: a signal mid-run skips the
// `finally` block below, so without this Ctrl-C (or a harness timeout) would
// leave Chrome and the static server behind.
let liveBrowser = null;
let liveServerClose = null;
for (const sig of ['SIGINT', 'SIGTERM']) {
	process.on(sig, () => {
		Promise.resolve()
			.then(() => (liveBrowser ? liveBrowser.close().catch(() => {}) : null))
			.then(() => (liveServerClose ? liveServerClose().catch(() => {}) : null))
			.finally(() => process.exit(sig === 'SIGINT' ? 130 : 143));
	});
}

async function main() {
	const buildDir = path.resolve(process.argv[2] || DEFAULT_BUILD_DIR);
	if (!fs.existsSync(path.join(buildDir, 'index.html'))) {
		process.stderr.write(`${buildDir}/index.html not found -- build it first:\n`
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

	const { baseUrl, close } = await createCoiServer(buildDir);
	liveServerClose = close;

	let browser;
	let exitCode = 1;
	try {
		browser = await puppeteer.launch({ executablePath: chromePath, headless: true });
		liveBrowser = browser;
		const page = await browser.newPage();
		const pageErrors = [];
		page.on('pageerror', (e) => pageErrors.push(String(e)));
		page.on('console', (msg) => { if (msg.type() === 'error') pageErrors.push(`[console] ${msg.text()}`); });

		await page.goto(baseUrl, { waitUntil: 'load' });

		const crossOriginIsolated = await page.evaluate(() => window.crossOriginIsolated);
		if (crossOriginIsolated !== true) {
			throw new Error(`window.crossOriginIsolated is ${crossOriginIsolated}, expected true `
				+ '(server headers should have made the page isolated on first load, '
				+ 'without needing the sw.js registration-and-reload fallback)');
		}
		process.stdout.write(`crossOriginIsolated: ${crossOriginIsolated} (from server headers)\n`);

		const banner = await waitForText(page, 'Welcome to the', TIMEOUT_MS);
		if (!banner.includes('tau> ')) {
			throw new Error(`banner appeared but "tau> " prompt did not:\n${banner}`);
		}
		process.stdout.write(`banner observed:\n${banner}\n`);

		const before = await terminalText(page);
		// Real keyboard input (focus + type + Enter) rather than
		// window.sendReplInput: it exercises xterm's onData -> stdin_buffer
		// wiring exactly as a user would, in addition to the upload feature's
		// sendReplInput path, which this check does not otherwise touch.
		await page.click('#terminal');
		await page.keyboard.type('help');
		await page.keyboard.press('Enter');
		const after = await waitForChange(page, before, TIMEOUT_MS);
		if (!after.includes('General commands:')) {
			throw new Error(`terminal changed but not with "help"'s own output:\n${after}`);
		}
		process.stdout.write(`terminal after "help":\n${after}\n`);

		if (pageErrors.length) {
			throw new Error(`page reported errors during the run:\n${pageErrors.join('\n')}`);
		}

		exitCode = 0;
	} catch (e) {
		process.stderr.write(`[repl-browser-check] ${e.message || e}\n`);
		exitCode = 1;
	} finally {
		if (browser) await browser.close();
		liveBrowser = null;
		await close();
		liveServerClose = null;
	}
	process.exit(exitCode);
}

main();
