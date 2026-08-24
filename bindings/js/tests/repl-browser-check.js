#!/usr/bin/env node

// Proves the browser REPL page (js/repl/CMakeLists.txt's output: index.html,
// tau_repl.js/.wasm, vendor/) actually starts and accepts input in headless
// Chrome, end to end -- not just that the wasm module links and runs under
// Node (which .local/build-emscripten.md's D11 already established).
//
// Reuses loadPuppeteer/resolveChromePath from browser-harness.js rather
// than reimplementing Chrome discovery -- the doctest harness and this
// script must agree on where Chrome comes from. createServer from that
// same file is NOT reused: it sets no COOP/COEP headers (the doctest
// targets it serves are pthread-free and don't need them), so this script
// runs its own minimal static server that does, mirroring
// scripts/tau-repl-serve.sh's header/MIME behaviour without shelling out to
// it (a plain `http` server here keeps the check a single Node process,
// matching run-in-chrome.js's shape).
//
// Usage:
//   node repl-browser-check.js [build-dir]
//
// build-dir defaults to build/emscripten-pthread (the emscripten-pthread
// preset's binary dir) and must contain index.html, tau_repl.js/.wasm and
// vendor/ -- point it at a broken copy of that directory to exercise the
// failure path (see .local/build-emscripten.md's Phase 5 checklist item).
//
// Exits 0 only if every step below succeeds, non-zero otherwise:
//   - the page loads and window.crossOriginIsolated is true
//   - the welcome banner appears in the xterm terminal buffer
//   - typing "help" + Enter produces new terminal output

const http = require('http');
const fs = require('fs');
const path = require('path');
const { loadPuppeteer, resolveChromePath } = require('./browser-harness');

const DEFAULT_BUILD_DIR = path.join(__dirname, '..', '..', '..', 'build', 'emscripten-pthread');
const TIMEOUT_MS = Number(process.env.TAU_REPL_CHECK_TIMEOUT_MS) || 30000;

const MIME_TYPES = {
	'.html': 'text/html',
	'.js': 'application/javascript',
	'.mjs': 'application/javascript',
	'.wasm': 'application/wasm',
	'.css': 'text/css',
};

// Same two headers scripts/tau-repl-serve.sh sends -- serving them here
// means the page's own sw.js COOP/COEP fallback is never exercised by this
// check either, which is asserted below rather than assumed.
function createCoiServer(dir) {
	const server = http.createServer((req, res) => {
		const url = new URL(req.url, 'http://localhost');
		const pathname = url.pathname === '/' ? '/index.html' : url.pathname;
		const filePath = path.join(dir, decodeURIComponent(pathname));
		res.setHeader('Cross-Origin-Opener-Policy', 'same-origin');
		res.setHeader('Cross-Origin-Embedder-Policy', 'require-corp');
		fs.readFile(filePath, (err, data) => {
			if (err) { res.writeHead(404); res.end(); return; }
			res.writeHead(200, { 'Content-Type': MIME_TYPES[path.extname(filePath)] || 'application/octet-stream' });
			res.end(data);
		});
	});
	return new Promise((resolve) => {
		server.listen(0, '127.0.0.1', () => {
			const port = server.address().port;
			resolve({ baseUrl: `http://127.0.0.1:${port}`, close: () => new Promise((r) => server.close(r)) });
		});
	});
}

// Reads only the visible rows of the xterm scrollback -- the terminal is an
// internal canvas/DOM grid, not plain text nodes, so buffer.active's own
// line API is the only way to recover what it is showing.
async function terminalText(page) {
	return page.evaluate(() => {
		const buf = window.term.buffer.active;
		const lines = [];
		for (let y = 0; y < window.term.rows; y++) {
			const line = buf.getLine(y);
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
			+ '  ./dev preset emscripten-pthread -DTAU_BUILD_JOBS=4\n');
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
