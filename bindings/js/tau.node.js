#!/usr/bin/env node

// Smoke test for the tau embind library (D5 v1 surface). Exits non-zero on
// any FAIL so it is usable as a CI gate.
//
// Independent ground truth for the assertions below was taken from the
// native `tau` CLI, built from this same source tree with the default pack.
// The wasm default pack skips bv and hsb (cvc5 links GMP) and nlang (curl
// has no wasm port), so the queries use only what both packs hold. The
// commands (`tau -e '<cmd>' -q`):
//   sat  x = 0                                           -> T
//   sat  x = 0 && x = 1                                  -> F
//   valid x = x                                          -> T
//   normalize x = 0 || x = 0                             -> x = 0
//   solve x = 0                                          -> x := { F }:tau
// The interpreter step sequence mirrors
// tests/api/test_api-string_api.cpp "using get_inputs_for_step".
//
// sat/unsat/valid take a bare formula (no trailing '.'), like
// normalize_formula/solve/to_str below -- a period is only meaningful to
// get_spec's full spec grammar.

const tauModule = require('./tau.js');

let failed = false;
function check(cond, label) {
	if (cond) { console.log('OK   ' + label); }
	else { console.error('FAIL ' + label); failed = true; }
}

tauModule().then(tau => {
	try {
		const spec = tau.get_spec('o[t] = i[t].');
		check(typeof spec === 'string'
			&& spec.includes('o[t]') && spec.includes('i[t]'),
			`get_spec("o[t] = i[t].") -> ${JSON.stringify(spec)}`);

		check(tau.get_spec('x ) ( invalid !!!') === null,
			'get_spec rejects malformed input');

		check(tau.unsat('x = 0 && x = 1') === true,
			'unsat(contradiction) === true (verified against native CLI)');
		check(tau.sat('x = 0 && x = 1') === false,
			'sat(contradiction) === false (verified against native CLI)');
		check(tau.valid('x = x') === true,
			'valid("x = x") === true (verified against native CLI)');

		check(tau.sat('x = 0') === true,
			'sat("x = 0") === true (native CLI says T; solve() agrees)');
		check(tau.unsat('x = 0') === false,
			'unsat("x = 0") === false (x = 0 is satisfiable)');

		const norm = tau.normalize_formula('x = 0 || x = 0');
		check(norm === 'x = 0',
			`normalize_formula("x = 0 || x = 0") -> ${JSON.stringify(norm)}`);

		const sol = tau.solve('x = 0', 'general');
		check(sol !== null && sol.x === 'F',
			`solve("x = 0") -> ${JSON.stringify(sol)}`);

		const toStrOut = tau.to_str('x = 0 || x = 0');
		check(toStrOut === 'x = 0.',
			`to_str("x = 0 || x = 0") -> ${JSON.stringify(toStrOut)}`);

		const handle = tau.interpreter_create('o[t] = i[t].');
		check(handle > 0, `interpreter_create("o[t] = i[t].") -> ${handle}`);

		const outputs = [];
		for (let step = 1; step <= 3; step++) {
			const inputVars = tau.interpreter_input_vars(handle);
			const inputs = {};
			for (const name of inputVars)
				inputs[name] = (step % 2 === 0) ? 'T.' : 'F.';
			const out = tau.interpreter_step(handle, inputs);
			check(out !== null, `interpreter_step(${step}) produced output`);
			if (out) outputs.push(out.o);
		}
		tau.interpreter_free(handle);
		check(JSON.stringify(outputs) === JSON.stringify(['F', 'T', 'F']),
			'interpreter output sequence matches '
			+ 'tests/api/test_api-string_api.cpp: '
			+ `${JSON.stringify(outputs)}`);
	} catch (e) {
		console.error('EXCEPTION: ' + e.stack);
		failed = true;
	}

	if (failed) {
		console.error('SMOKE TEST FAILED');
		process.exit(1);
	}
	console.log('SMOKE TEST PASSED');
}).catch(e => {
	console.error('MODULE LOAD FAILED: ' + e.stack);
	process.exit(1);
});
