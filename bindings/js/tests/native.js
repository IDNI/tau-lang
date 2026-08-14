// Shells out to a native `tau` CLI (same source tree, same -DTAU_BAS=sbf,tau
// pack) to get independent ground truth for the wasm parity tests. See
// parity.js for the query set and the normalizations applied to compare
// the two output formats.

const { spawnSync } = require('child_process');
const fs = require('fs');
const path = require('path');

const NATIVE_BIN = process.env.TAU_NATIVE_BIN
	|| path.join(__dirname, '..', '..', '..', 'build', 'native-sbftau', 'tau');

function checkNativeBinary() {
	if (!fs.existsSync(NATIVE_BIN)) {
		throw new Error(
			`native tau binary not found at ${NATIVE_BIN}\n`
			+ 'Build it with the same pack the wasm module uses:\n'
			+ '  cmake -S . -B build/native-sbftau -DTAU_BAS=sbf,tau -DTAU_BUILD_EXECUTABLE=ON\n'
			+ '  cmake --build build/native-sbftau\n'
			+ 'or point TAU_NATIVE_BIN at an existing sbf,tau build of tau.');
	}
	const probe = spawnSync(NATIVE_BIN, ['--version'], { encoding: 'utf8', timeout: 10000 });
	if (probe.status !== 0) {
		throw new Error(`native tau binary at ${NATIVE_BIN} did not run `
			+ `(exit ${probe.status}): ${probe.stderr || probe.stdout}`);
	}
}

function stripAnsi(s) {
	// eslint-disable-next-line no-control-regex
	return s.replace(/\x1b\[[0-9;]*m/g, '');
}

function shellQuote(arg) {
	return `'${arg.replace(/'/g, `'\\''`)}'`;
}

// Runs one REPL command through -e, with optional extra CLI flags (e.g.
// -I for indenting), and returns its raw (ANSI-stripped) output. The
// benchmark/trace lines and the "%N: <result>" history line interleave
// between the CLI's stdout and stderr, so this shells out with `2>&1`
// rather than concatenating two separately-captured buffers (which would
// destroy the chronological order between them).
function runRepl(cmd, extraFlags = []) {
	const args = [...extraFlags, '-e', cmd, '-q'];
	const shellCmd = `${shellQuote(NATIVE_BIN)} ${args.map(shellQuote).join(' ')} 2>&1`;
	const r = spawnSync('/bin/sh', ['-c', shellCmd], {
		encoding: 'utf8', timeout: 20000,
	});
	return stripAnsi(r.stdout || '');
}

// The REPL prints each command's result as a trailing "%N: <text>" history
// entry; a single -e command produces exactly one, but under -I (indenting)
// its text spans multiple lines, so everything after the LAST "%N: " marker
// to the end of output is taken, trailing blank lines trimmed. Returns null
// if the command was rejected (syntax/type error) rather than answered.
function lastResult(output) {
	if (/\(Error\)/.test(output)) return null;
	const markers = [...output.matchAll(/^%\d+: /gm)];
	if (markers.length === 0) return null;
	const last = markers[markers.length - 1];
	return output.slice(last.index + last[0].length).replace(/\s+$/, '');
}

// sat/unsat/valid/realizable/unrealizable print "T" or "F" as their result.
function nativeBool(command, formula) {
	const text = lastResult(runRepl(`${command} ${formula}`));
	if (text === 'T') return true;
	if (text === 'F') return false;
	return null; // rejected input
}

// "whatis" parses an expression exactly as the string API's
// get_spec_or_term/get_formula_or_term do (full spec grammar first,
// falling back to a bare term), including the same IO type resolution --
// unlike bare REPL evaluation, which leaves IO streams untyped. It is the
// native equivalent of both toStr() and getSpec().
function nativeWhatis(expression) {
	return lastResult(runRepl(`whatis ${expression}`));
}

function nativeNormalize(formula) {
	return lastResult(runRepl(`normalize ${formula}`));
}

// The REPL's solution printer (print_solver_cmd_solution,
// repl_evaluator.tmpl.h) wraps the atomic true/false constant as
// `{<literal>}:<type>` for console display -- console decoration only, added
// on top of the literal after api<node>::solve(string) already agrees with
// it (both give tau's "F"/"T", not the generic tree printer's "0"/"1"; see
// api.tmpl.string.h's serialize_solution). The API's map has no type-name
// slot to put that decoration in, so it is stripped here for comparison.
// A compound value is never wrapped this way -- to_str()'s own ba_constant
// case already renders `{ value }:type` as part of the value itself (with
// a space, unlike the tight `{value}:type` above), so it never matches this
// regex and is compared untouched.
function stripAtomicWrap(raw) {
	const m = raw.match(/^\{(\S+)\}:\S+$/);
	return m ? m[1] : raw;
}

// Parses "solution: {\n\tx := VALUE\n...}" into { x: VALUE, ... }, or null
// for "no solution".
function nativeSolve(formula, mode) {
	const flag = mode === 'minimum' ? '--min' : mode === 'maximum' ? '--max' : '';
	const cmd = flag ? `solve ${flag} (${formula})` : `solve ${formula}`;
	const output = runRepl(cmd);
	if (/\(Error\)/.test(output)) return null;
	if (/no solution/.test(output)) return null;
	const block = output.match(/solution: \{([\s\S]*?)\n\}/);
	if (!block) return null;
	const result = {};
	for (const line of block[1].split('\n')) {
		const m = line.match(/(\S+)\s*:=\s*(.+?)\s*$/);
		if (m) result[m[1]] = stripAtomicWrap(m[2]);
	}
	return result;
}

// Drives the interpreter through the REPL's "run N steps" command with an
// explicit input file, the native equivalent of interpreterCreate/Step.
// Returns the collected output-stream values in step order.
function nativeInterpreterRun(spec, inputValues, scratchDir) {
	const inFile = path.join(scratchDir, `native-run-${process.pid}-${Date.now()}.in`);
	fs.writeFileSync(inFile, inputValues.map((v) => `${v}.`).join('\n'));
	try {
		const n = inputValues.length;
		const cmd = `i:tau := in file("${inFile}"). o:tau := out console. `
			+ `run ${n} steps ${spec}`;
		const output = runRepl(cmd);
		const outputs = [];
		const re = /^o\[(\d+)\] := (.*)$/gm;
		let m;
		while ((m = re.exec(output)) !== null) outputs.push(m[2]);
		return outputs;
	} finally {
		fs.unlinkSync(inFile);
	}
}

module.exports = {
	NATIVE_BIN,
	checkNativeBinary,
	runRepl,
	lastResult,
	nativeBool,
	nativeWhatis,
	nativeNormalize,
	nativeSolve,
	nativeInterpreterRun,
};
