// Shared plumbing for driving emscripten MODULARIZE=0 doctest binaries
// (<x>.js/<x>.wasm pairs) inside headless Chrome, used by both
// run-in-chrome.js (one test) and run-suite-in-chrome.js (the whole ctest
// suite). Kept in one place so the harness page, the static server and the
// exit-status contract can't drift between the two the way tref and
// has_gpp did (see AGENTS.md's comment-style section and
// .local/build-emscripten.md Phase 4).

const http = require('http');
const fs = require('fs');
const path = require('path');
const { execFileSync } = require('child_process');

const DEFAULT_BUILD_DIR = path.join(__dirname, '..', '..', '..', 'build', 'debug-emscripten-tests');
const DEVRC = path.join(__dirname, '..', '..', '..', 'external', 'parser', 'scripts', 'devrc');
const DEFAULT_TIMEOUT_MS = Number(process.env.TAU_BROWSER_TEST_TIMEOUT_MS) || 30000;

const MIME_TYPES = {
	'.wasm': 'application/wasm',
	'.js': 'text/javascript',
	'.html': 'text/html',
};

// Resolves a bare test name (looked up as <buildDir>/<name>.js) or an
// explicit path (relative/absolute, with or without ".js") to the
// directory/basename of an emscripten output pair, verifying both the
// ".js" and ".wasm" halves exist.
function resolveTarget(arg, buildDir) {
	const looksLikePath = arg.includes('/') || arg.endsWith('.js');
	const jsPath = looksLikePath
		? path.resolve(process.cwd(), arg.endsWith('.js') ? arg : `${arg}.js`)
		: path.join(buildDir, `${arg}.js`);
	const dir = path.dirname(jsPath);
	const name = path.basename(jsPath, '.js');
	const wasmPath = path.join(dir, `${name}.wasm`);
	if (!fs.existsSync(jsPath)) throw new Error(`not found: ${jsPath}`);
	if (!fs.existsSync(wasmPath)) throw new Error(`not found: ${wasmPath}`);
	return { dir, name };
}

// Sets window.Module before the test script loads so its "if
// (Module['print']) out = Module['print']" wiring (same file, every
// emscripten output) picks up ours instead of console.log. EXITSTATUS is a
// bare top-level `var` in the non-MODULARIZE output, so it lands on
// `window` once the script runs -- polled from the Node side rather than
// relying on Module.onExit, which emscripten only calls when the runtime
// is allowed to exit (EXIT_RUNTIME=1, not set on these test targets).
function harnessHtml(scriptName) {
	return `<!doctype html><html><head><meta charset="utf-8"><script>
window.Module = {
	print: (line) => { window.__tauPrint(line); },
	printErr: (line) => { window.__tauPrintErr(line); },
};
</script></head><body><script src="/${scriptName}.js"></script></body></html>`;
}

// One static server per test *directory* -- not per test -- so a batch run
// shares one server across every test that lives in the same build dir
// instead of paying for 61 of them. The harness page is parameterized by
// "?script=" rather than baked in at server-creation time, which is what
// makes that sharing possible.
function createServer(dir) {
	const server = http.createServer((req, res) => {
		const url = new URL(req.url, 'http://localhost');
		if (url.pathname === '/__harness__.html') {
			const scriptName = url.searchParams.get('script');
			if (!scriptName) { res.writeHead(400); res.end('missing ?script='); return; }
			const body = harnessHtml(scriptName);
			res.writeHead(200, { 'Content-Type': 'text/html', 'Content-Length': Buffer.byteLength(body) });
			res.end(body);
			return;
		}
		// Chrome requests this unconditionally on every page load; nothing in
		// a build dir owns it, so answer directly rather than let it fall
		// through to the 404 every other run was logging.
		if (url.pathname === '/favicon.ico') { res.writeHead(204); res.end(); return; }
		const filePath = path.join(dir, decodeURIComponent(url.pathname));
		fs.readFile(filePath, (err, data) => {
			if (err) { res.writeHead(404); res.end(); return; }
			const type = MIME_TYPES[path.extname(filePath)] || 'application/octet-stream';
			res.writeHead(200, { 'Content-Type': type, 'Content-Length': data.length });
			res.end(data);
		});
	});
	return new Promise((resolve) => {
		server.listen(0, '127.0.0.1', () => {
			const port = server.address().port;
			resolve({
				server,
				baseUrl: `http://127.0.0.1:${port}`,
				close: () => new Promise((r) => server.close(r)),
			});
		});
	});
}

function loadPuppeteer() {
	try {
		return require('puppeteer-core');
	} catch {
		throw new Error(
			'puppeteer-core not found. Install the JS test tooling (bindings/js/package.json) once:\n'
			+ '  cd bindings/js && npm install\n');
	}
}

// Same rule cmake/tau-shared-prefix.cmake asks for -- TAU_SHARED_PREFIX
// (default ~/.tau), defined once in devrc's dep_shared_prefix() and asked
// here rather than re-implemented, so this and dep-chrome.sh can't disagree
// about where Chrome landed. Memoized: every caller in one run wants the
// same answer, and it costs a subprocess.
let cachedSharedPrefix = null;
function sharedPrefix() {
	if (cachedSharedPrefix === null) {
		cachedSharedPrefix = execFileSync(
			'bash', ['-c', 'source "$1" && dep_entry && dep_shared_prefix', 'bash', DEVRC],
			{ encoding: 'utf8' },
		).trim();
	}
	return cachedSharedPrefix;
}

// dep-chrome.sh hands the pinned version to `@puppeteer/browsers install`,
// which lays it out as <path>/chrome/linux-<version>/chrome-linux64/chrome;
// globbing for it here (rather than restating CHROME_TAG's default) means
// this file never needs to change when that pin is bumped.
function findInstalledChrome(prefix) {
	const chromeRoot = path.join(prefix, 'chrome', 'chrome');
	if (!fs.existsSync(chromeRoot)) return null;
	const candidates = fs.readdirSync(chromeRoot)
		.filter((entry) => entry.startsWith('linux-'))
		.map((entry) => path.join(chromeRoot, entry, 'chrome-linux64', 'chrome'))
		.filter((p) => fs.existsSync(p));
	if (candidates.length === 0) return null;
	// Normally exactly one -- if a CHROME_TAG bump left an older build behind
	// too, the most recently installed one is the one still meant to run.
	candidates.sort((a, b) => fs.statSync(b).mtimeMs - fs.statSync(a).mtimeMs);
	return candidates[0];
}

function resolveChromePath() {
	if (process.env.CHROME_PATH) {
		if (!fs.existsSync(process.env.CHROME_PATH)) {
			throw new Error(`Chrome not found at $CHROME_PATH (${process.env.CHROME_PATH}).\n`);
		}
		return process.env.CHROME_PATH;
	}
	const installed = findInstalledChrome(sharedPrefix());
	if (installed) return installed;
	throw new Error(
		'Chrome not found. Install the pinned build with:\n'
		+ '  ./dev dep-chrome.sh\n'
		+ 'or point $CHROME_PATH at a Chrome/Chromium binary.\n');
}

// Runs one test in a fresh page against an already-serving directory and
// always closes the page again -- callers share one browser across many
// calls (constraint: one page per test, not one browser per test). Console
// errors, uncaught page exceptions and a crashed renderer are all captured
// into `stderr` alongside doctest's own printErr output, so a crash or a
// hang is reported with something to look at rather than a bare exit code.
//
// Four outcomes are told apart, not folded into one bucket:
//   - assertion failure: exitCode !== 0, the page ran to completion normally
//   - genuine timeout:   `page.waitForFunction` actually waited out timeoutMs
//     -- checked against the real clock (`waitElapsedMs`), not assumed from
//     the fact that the promise rejected
//   - crash:             the page/target went away *before* timeoutMs
//     elapsed (rejection is a puppeteer TargetCloseError/ProtocolError, or
//     the 'error' event fired) -- a dead target thrown quickly must never be
//     reported as "timed out after <timeoutMs>ms", which is what made the
//     original two false timeouts unbelievable (real wall time couldn't fit
//     two genuine 600000ms waits)
//   - harness error:     everything before the wait even started (page
//     failed to load, `exposeFunction`/`goto` itself rejected)
async function runTest(browser, { baseUrl, name, timeoutMs }) {
	const page = await browser.newPage();
	const stdout = [];
	const stderr = [];
	let crashInfo = null;
	page.on('pageerror', (e) => stderr.push(`[page error] ${e}`));
	page.on('console', (msg) => { if (msg.type() === 'error') stderr.push(`[console] ${msg.text()}`); });
	page.on('error', (e) => { crashInfo = crashInfo || `renderer crashed: ${e}`; stderr.push(`[renderer crash] ${e}`); });

	let exitCode = null;
	let timedOut = false;
	let harnessError = null;
	let waitElapsedMs = null;
	try {
		await page.exposeFunction('__tauPrint', (line) => stdout.push(line));
		await page.exposeFunction('__tauPrintErr', (line) => stderr.push(line));
		await page.goto(`${baseUrl}/__harness__.html?script=${encodeURIComponent(name)}`, { waitUntil: 'load' });
		const waitStart = Date.now();
		try {
			await page.waitForFunction('typeof window.EXITSTATUS !== "undefined"', { timeout: timeoutMs });
			exitCode = await page.evaluate(() => window.EXITSTATUS);
		} catch (e) {
			waitElapsedMs = Date.now() - waitStart;
			const puppeteer = loadPuppeteer();
			// A real timeout waited the full budget; anything else (target
			// closed, protocol error, the renderer crashing mid-wait) is the
			// page dying under it, reported at the real elapsed time it
			// actually got instead of the configured budget it never used.
			if (e instanceof puppeteer.TimeoutError && waitElapsedMs >= timeoutMs) {
				timedOut = true;
				stderr.push(`timed out waiting for the test to exit -- waited ${waitElapsedMs}ms (budget ${timeoutMs}ms)`);
			} else {
				crashInfo = crashInfo || `page/target lost after ${waitElapsedMs}ms (budget ${timeoutMs}ms): ${e.message || e}`;
				stderr.push(`[crash] ${crashInfo}`);
			}
		}
	} catch (e) {
		harnessError = e.message || String(e);
		stderr.push(`[harness error] ${harnessError}`);
	} finally {
		try { await page.close(); } catch { /* page may already be gone if the renderer crashed */ }
	}
	return {
		name, exitCode, timedOut, crashed: !!crashInfo, crashInfo, harnessError, waitElapsedMs, stdout, stderr,
	};
}

module.exports = {
	DEFAULT_BUILD_DIR,
	DEFAULT_TIMEOUT_MS,
	MIME_TYPES,
	resolveTarget,
	harnessHtml,
	createServer,
	loadPuppeteer,
	resolveChromePath,
	runTest,
};
