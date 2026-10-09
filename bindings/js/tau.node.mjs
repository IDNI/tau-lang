#!/usr/bin/env node

// ES module smoke test for the tau embind library (D5 v1 surface). Same
// assertions as tau.node.js (the CommonJS smoke test) -- see that file for
// the native-CLI ground truth these were checked against. Kept separate
// rather than shared because the two module formats need distinct load
// syntax (import vs require) and this proves import actually works, not
// just require().

import tauModule from './tau.esm.mjs';

let failed = false;
function check(cond, label) {
	if (cond) { console.log('OK   ' + label); }
	else { console.error('FAIL ' + label); failed = true; }
}

try {
	const tau = await tauModule();

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
