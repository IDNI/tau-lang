#!/usr/bin/env node

// Parity test for the D5 wasm embind surface: every query runs through
// both the wasm module and a native `tau` CLI built from this same source
// tree with the identical -DTAU_BAS=sbf,tau pack, and the two results are
// diffed. Exits non-zero on any divergence, so it can gate CI.
//
// tau.node.js is the quick load smoke test; this is the deeper one, added
// after a bug (sat() returning false for every well-formed formula, valid()
// able to abort) survived because nothing exercised the string API against
// known-good ground truth.
//
// The native binary path comes from TAU_NATIVE_BIN, defaulting to
// build/native-sbftau/tau (see native.js). It must exist and run --
// silently skipping it would recreate the exact blind spot this test
// exists to close.
//
// Query material is drawn from tests/api/test_api-string_api.cpp and the
// tests/repl/commands/*.cmake suites, restricted to formulas that name no
// BA outside sbf,tau (matching tests/repl/tau_repl_pack.cmake's own gating)
// so results are meaningful against this pack.
//
// Normalizations applied (formatting only -- see each helper in native.js):
//  - ANSI color codes and the debug build's parse-tree/context dump are
//    stripped; only the REPL's final "%N: <result>" history line is read.
//  - sat/unsat/valid/realizable/unrealizable: native prints "T"/"F",
//    the wasm binding returns a JS boolean.
//  - toStr/getSpec: compared against the native "whatis" command, not bare
//    REPL evaluation -- whatis parses through the same
//    get_spec_or_term/get_formula_or_term path (full spec grammar first,
//    IO streams type-resolved) that the string API uses, where bare REPL
//    evaluation does neither. getSpec's trailing '.' (full spec grammar)
//    is stripped for comparison since whatis's history line does not carry
//    one. get_spec_or_term simplifies by default, so only inputs already
//    in their simplified/canonical form are used here: an input like
//    "x = 0 || x = 0" would legitimately print differently (a real
//    simplification, not a formatting quirk) and is exercised instead
//    under normalize, whose own native command applies the same pass.
//  - solve: native's "solution: { x := VALUE }" block is parsed into
//    { x: VALUE }. VALUE matches api<node>::solve(string)'s output verbatim
//    -- both give the declared type's own literal (tau's "F"/"T"), from
//    serialize_solution (api.tmpl.string.h) reusing the same find_ba_type /
//    serialize_constant core helpers as the REPL's solution printer
//    (print_solver_cmd_solution, repl_evaluator.tmpl.h). The one difference
//    left is presentational: the REPL additionally wraps the atomic
//    true/false constant as "{ F }:tau"/"{ T }:tau" for console display, a
//    decoration the API's map has no type-name slot for, so native.js's
//    stripAtomicWrap strips only that wrapper -- it never touches the
//    literal spelling, so a regression back to the generic "0"/"1" would
//    still fail here.
//  - malformed input: only unambiguous syntax errors (e.g. an unbalanced
//    paren) are cross-checked against native. A truncated-but-parseable
//    fragment like "o[t] =" is NOT, because the interactive REPL's
//    multi-line continuation reads more input and can complete it into a
//    valid formula -- a difference in how the two interfaces frame "one
//    query", not in what either computes. The empty string is likewise
//    not comparable via a single -e/echo invocation. Both are still
//    checked against the wasm module's own contract (null/false/true).
//  - interpreter: native ground truth comes from the REPL's
//    "run N steps" command against an explicit input file, reading the
//    resulting "o[K] := VALUE" lines -- a different code path than
//    interpreterCreate/Step, but the same interpreter semantics.

const os = require('os');
const path = require('path');
const native = require('./native');

const WASM_JS = process.env.TAU_WASM_JS
	|| path.join(__dirname, '..', '..', '..', 'build', 'emscripten', 'tau.js');
const SCRATCH_DIR = process.env.TAU_PARITY_SCRATCH_DIR || os.tmpdir();

let failed = false;
let checked = 0;

function check(cond, label) {
	checked++;
	if (cond) { console.log('OK   ' + label); }
	else { console.error('FAIL ' + label); failed = true; }
}

function diverges(label, wasmVal, nativeVal) {
	check(wasmVal === nativeVal, `${label}: wasm=${JSON.stringify(wasmVal)} `
		+ `native=${JSON.stringify(nativeVal)}`);
}

// A fresh split variable's numeric suffix comes from a process-lifetime
// counter (get_new_uninterpreted_constant, splitter.tmpl.h): the wasm
// module is one long-lived instance across this whole run, while each
// native query is its own subprocess, so the two counters never line up.
// Compare structure, not that arbitrary spelling.
function normalizeSplitVars(s) {
	return s === null ? null : s.replace(/\bsplit\d+\b/g, 'split');
}

function diffSolve(label, wasmSol, nativeSol) {
	const same = normalizeSplitVars(JSON.stringify(wasmSol))
		=== normalizeSplitVars(JSON.stringify(nativeSol));
	check(same, `${label}: wasm=${JSON.stringify(wasmSol)} `
		+ `native=${JSON.stringify(nativeSol)}`);
}

// wasm getSpec()/toStr() append the full-spec grammar's trailing '.',
// with indenting on, a newline ahead of it; whatis's history line carries
// neither, native.lastResult already having trimmed its trailing
// whitespace.
function stripTrailingPeriod(s) {
	return s === null ? null : s.replace(/\.$/, '').replace(/\s+$/, '');
}

// -- sat / unsat / valid --------------------------------------------------
// [formula, expected sat, expected unsat, expected valid] (null = not
// asserted for that predicate here, still cross-checked where present).

const BOOL_CASES = [
	['T', true, false, true],
	['F', false, true, false],
	['X = 0', true, false, false],
	['X != 0', true, false, false],
	['X = 0 || Y = 0', true, false, null],
	['X = 0 && X != 0', false, true, null],
	['x=0 || !x=0', true, false, true],
	['T || F', null, null, true],
	['(x = 0) -> (y = 0)', null, null, false],
	['!((x = 0) -> (y = 0))', null, null, false],
	['(T -> x = 0) <-> (x = 0)', null, null, true],
	['((x = 0) -> F) <-> (!(x = 0))', null, null, true],
	['(x < y) <-> ((x & y\') = 0 && x != y)', null, null, true],
	['(x <= y) <-> ((x & y\') = 0)', null, null, true],
	['(G (o1[t] != 0)) && (G (o2[t] != 0))', true, false, null],
	['(G (o1[t] = 0)) && (G (o1[t] = 1))', false, true, null],
	['! ((G (o1[t] = 0)) && (G (o1[t] = 1)))', true, false, null],
	['(G (o1[t] = 0)) ^^ (G (o1[t] = 1))', true, false, null],
];

function runBoolCases(tau) {
	for (const [formula, expSat, expUnsat, expValid] of BOOL_CASES) {
		const wasmSat = tau.sat(formula);
		const nativeSat = native.nativeBool('sat', formula);
		diverges(`sat(${JSON.stringify(formula)})`, wasmSat, nativeSat);
		if (expSat !== null) check(wasmSat === expSat,
			`sat(${JSON.stringify(formula)}) === ${expSat} (expected)`);

		const wasmUnsat = tau.unsat(formula);
		const nativeUnsat = native.nativeBool('unsat', formula);
		diverges(`unsat(${JSON.stringify(formula)})`, wasmUnsat, nativeUnsat);
		if (expUnsat !== null) check(wasmUnsat === expUnsat,
			`unsat(${JSON.stringify(formula)}) === ${expUnsat} (expected)`);

		const wasmValid = tau.valid(formula);
		const nativeValid = native.nativeBool('valid', formula);
		diverges(`valid(${JSON.stringify(formula)})`, wasmValid, nativeValid);
		if (expValid !== null) check(wasmValid === expValid,
			`valid(${JSON.stringify(formula)}) === ${expValid} (expected)`);
	}
}

// -- normalizeFormula -------------------------------------------------------
// [formula, expected normalized text]

const NORMALIZE_CASES = [
	["X & X' = 0", 'T'],
	['x = 0 || x = 0', 'x = 0'],
	['F -> x = 0', 'T'],
	['(x = 0) -> T', 'T'],
	['(x = 0) -> (x = 0)', 'T'],
	['(x = 0) <-> !(x = 0)', 'F'],
	['x < x', 'F'],
	['x <= x', 'T'],
	['ex x x=0', 'T'],
	['ex x,y xy=0', 'T'],
	['all x x!=0', 'F'],
	['all x ex y x=y', 'T'],
];

function runNormalizeCases(tau) {
	for (const [formula, expected] of NORMALIZE_CASES) {
		const wasmNorm = tau.normalizeFormula(formula);
		const nativeNorm = native.nativeNormalize(formula);
		diverges(`normalizeFormula(${JSON.stringify(formula)})`,
			wasmNorm, nativeNorm);
		check(wasmNorm === expected,
			`normalizeFormula(${JSON.stringify(formula)}) === `
			+ `${JSON.stringify(expected)} (expected)`);
	}
}

// -- toStr / getSpec (via whatis) -------------------------------------------
// Bare formulas/terms already in canonical form (see file header).

const TOSTR_CASES = [
	'x = 0',
	"x & y' = 1",
	'(G (o1[t] != 0)) && (G (o2[t] != 0))',
];

const GETSPEC_CASES = [
	'o[t] = i[t].',
	'o[t] = i[t] && o[t-1] = i[t-1].',
];

function runToStrCases(tau) {
	for (const expr of TOSTR_CASES) {
		const wasmStr = stripTrailingPeriod(tau.toStr(expr));
		const nativeStr = native.nativeWhatis(expr);
		diverges(`toStr(${JSON.stringify(expr)})`, wasmStr, nativeStr);
	}
}

function runGetSpecCases(tau) {
	for (const spec of GETSPEC_CASES) {
		const wasmSpec = stripTrailingPeriod(tau.getSpec(spec));
		const nativeSpec = native.nativeWhatis(spec);
		diverges(`getSpec(${JSON.stringify(spec)})`, wasmSpec, nativeSpec);
	}
}

// -- solve --------------------------------------------------------------

const SOLVE_CASES = [
	['x = 0', 'general'],
	['x != 0', 'maximum'],
	['x != 0', 'minimum'],
	['x != 0 && x != 1', 'general'],
	['x = 0 && x != 0', 'general'],
];

function runSolveCases(tau) {
	for (const [formula, mode] of SOLVE_CASES) {
		const wasmSol = tau.solve(formula, mode);
		const nativeSol = native.nativeSolve(formula, mode);
		diffSolve(`solve(${JSON.stringify(formula)}, ${mode})`,
			wasmSol, nativeSol);
	}
}

// -- settings: setIndenting is the one setting with an observable, directly
// comparable textual effect (native's -I flag); the rest are exercised for
// crash-safety and to confirm they don't corrupt later results.

function runSettingsCases(tau) {
	const expr = '(G (o1[t] != 0)) && (G (o2[t] != 0))';
	tau.setIndenting(true);
	const wasmIndented = stripTrailingPeriod(tau.toStr(expr));
	tau.setIndenting(false);
	const nativeText = native.lastResult(native.runRepl(`whatis ${expr}`, ['-I']));
	diverges('setIndenting(true) -> toStr', wasmIndented, nativeText);

	tau.setCharvar(true);
	tau.setBlasting(true);
	tau.setHighlighting(false);
	tau.setJson(false);
	tau.setSeverity('error');
	check(tau.sat('x = 0') === true,
		'sat still correct after touching every setting');
}

// -- malformed input ------------------------------------------------------

function runMalformedCases(tau) {
	const badSyntax = 'x ) ( invalid !!!';
	check(tau.getSpec(badSyntax) === null, 'getSpec rejects bad syntax');
	check(tau.toStr(badSyntax) === null, 'toStr rejects bad syntax');
	check(tau.normalizeFormula(badSyntax) === null,
		'normalizeFormula rejects bad syntax');
	check(tau.sat(badSyntax) === false, 'sat rejects bad syntax');
	// Unparseable input yields no result, so unsat returns false, not
	// true (api.tmpl.string.h's unsat(const std::string&)).
	check(tau.unsat(badSyntax) === false, 'unsat rejects bad syntax');
	check(tau.valid(badSyntax) === false, 'valid rejects bad syntax');
	check(tau.solve(badSyntax, 'general') === null,
		'solve rejects bad syntax');
	const nativeRejected = native.nativeBool('sat', badSyntax);
	check(nativeRejected === null, 'native also rejects bad syntax '
		+ `(sat returned ${JSON.stringify(nativeRejected)})`);

	// Not cross-checked against native -- see file header.
	check(tau.getSpec('') === null, 'getSpec rejects empty input');
	check(tau.sat('') === false, 'sat rejects empty input');
}

// -- interpreter ------------------------------------------------------------

function runInterpreterCases(tau) {
	const spec = 'o[t] = i[t].';
	const inputValues = ['F', 'T', 'F'];

	const handle = tau.interpreterCreate(spec);
	check(handle > 0, `interpreterCreate(${JSON.stringify(spec)})`);
	const wasmOutputs = [];
	for (const v of inputValues) {
		const inputVars = tau.interpreterInputVars(handle);
		const inputs = {};
		for (const name of inputVars) inputs[name] = `${v}.`;
		const out = tau.interpreterStep(handle, inputs);
		check(out !== null, `interpreterStep produced output for input ${v}`);
		wasmOutputs.push(out.o);
	}
	tau.interpreterFree(handle);

	const nativeOutputs = native.nativeInterpreterRun(spec, inputValues, SCRATCH_DIR);
	check(JSON.stringify(wasmOutputs) === JSON.stringify(nativeOutputs),
		`interpreter step sequence: wasm=${JSON.stringify(wasmOutputs)} `
		+ `native=${JSON.stringify(nativeOutputs)}`);
	check(JSON.stringify(wasmOutputs) === JSON.stringify(['F', 'T', 'F']),
		'interpreter output sequence matches '
		+ `tests/api/test_api-string_api.cpp: ${JSON.stringify(wasmOutputs)}`);
}

// ---------------------------------------------------------------------------

native.checkNativeBinary();
console.log(`Using native binary: ${native.NATIVE_BIN}`);

const tauModule = require(WASM_JS);
tauModule().then((tau) => {
	try {
		runBoolCases(tau);
		runNormalizeCases(tau);
		runToStrCases(tau);
		runGetSpecCases(tau);
		runSolveCases(tau);
		runSettingsCases(tau);
		runMalformedCases(tau);
		runInterpreterCases(tau);
	} catch (e) {
		console.error('EXCEPTION: ' + e.stack);
		failed = true;
	}

	console.log(`${checked} checks run.`);
	if (failed) {
		console.error('PARITY TEST FAILED');
		process.exit(1);
	}
	console.log('PARITY TEST PASSED');
}).catch((e) => {
	console.error('MODULE LOAD FAILED: ' + e.stack);
	process.exit(1);
});
