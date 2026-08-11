#!/usr/bin/env node

// Smoke test for the tau embind library (D5 v1 surface). Exits non-zero on
// any FAIL so it is usable as a CI gate.
//
// Independent ground truth for the assertions below was taken from the
// native `tau` CLI, built from this same source tree with the same
// -DTAU_BAS=sbf,tau pack (`./build/scratch-sbftau/tau -e '<cmd>' -q`):
//   sat  x = 0                                           -> T
//   sat  x = 0 && x = 1                                  -> F
//   valid x = x                                          -> T
//   normalize x = 0 || x = 0                             -> x = 0
//   solve x = 0                                          -> x := {0}:tau
// The interpreter step sequence mirrors
// tests/api/test_api-string_api.cpp "using get_inputs_for_step".
//
// sat/unsat/valid take a bare formula (no trailing '.'), like
// normalizeFormula/solve/toStr below -- a period is only meaningful to
// getSpec's full spec grammar.

const tauModule = require('./tau.js');

let failed = false;
function check(cond, label) {
	if (cond) { console.log('OK   ' + label); }
	else { console.error('FAIL ' + label); failed = true; }
}

tauModule().then(tau => {
	try {
		const spec = tau.getSpec('o[t] = i[t].');
		check(typeof spec === 'string'
			&& spec.includes('o[t]') && spec.includes('i[t]'),
			`getSpec("o[t] = i[t].") -> ${JSON.stringify(spec)}`);

		check(tau.getSpec('x ) ( invalid !!!') === null,
			'getSpec rejects malformed input');

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

		const norm = tau.normalizeFormula('x = 0 || x = 0');
		check(norm === 'x = 0',
			`normalizeFormula("x = 0 || x = 0") -> ${JSON.stringify(norm)}`);

		const sol = tau.solve('x = 0', 'general');
		check(sol !== null && sol.x === '0',
			`solve("x = 0") -> ${JSON.stringify(sol)}`);

		const toStrOut = tau.toStr('x = 0 || x = 0');
		check(toStrOut === 'x = 0.',
			`toStr("x = 0 || x = 0") -> ${JSON.stringify(toStrOut)}`);

		const handle = tau.interpreterCreate('o[t] = i[t].');
		check(handle > 0, `interpreterCreate("o[t] = i[t].") -> ${handle}`);

		const outputs = [];
		for (let step = 1; step <= 3; step++) {
			const inputVars = tau.interpreterInputVars(handle);
			const inputs = {};
			for (const name of inputVars)
				inputs[name] = (step % 2 === 0) ? 'T.' : 'F.';
			const out = tau.interpreterStep(handle, inputs);
			check(out !== null, `interpreterStep(${step}) produced output`);
			if (out) outputs.push(out.o);
		}
		tau.interpreterFree(handle);
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
