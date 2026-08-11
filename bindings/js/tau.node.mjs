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
