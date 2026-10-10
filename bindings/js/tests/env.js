#!/usr/bin/env node

// The TAU_ variables of node reach the wasm module. The test runs itself as a
// child with TAU_MAX_FIXPOINT_STEPS set, because the module reads the
// variables once, when it loads.
//
// Needs no native binary.

const path = require('path');
const { spawnSync } = require('child_process');

const WASM_JS = process.env.TAU_WASM_JS
	|| path.join(__dirname, '..', 'tau.js');

// The child prints what the module reads after the load.
if (process.argv[2] === 'child') {
	require(WASM_JS)().then((tau) => {
		console.log(JSON.stringify({
			option: tau.get_option('max-fixpoint-steps'),
			error: tau.get_last_error(),
		}));
	}).catch((e) => {
		console.error('MODULE LOAD FAILED: ' + e.stack);
		process.exit(1);
	});
	return;
}

let failed = false;

function check(cond, label) {
	if (cond) { console.log('OK   ' + label); }
	else { console.error('FAIL ' + label); failed = true; }
}

function load_with(value) {
	const env = Object.assign({}, process.env, { TAU_WASM_JS: WASM_JS });
	if (value === undefined) delete env.TAU_MAX_FIXPOINT_STEPS;
	else env.TAU_MAX_FIXPOINT_STEPS = value;
	const r = spawnSync(process.execPath, [__filename, 'child'],
		{ env, encoding: 'utf8' });
	if (r.status !== 0) {
		console.error(r.stdout + r.stderr);
		return null;
	}
	return JSON.parse(r.stdout.trim().split('\n').pop());
}

const unset = load_with(undefined);
check(unset !== null && unset.option === '500' && unset.error === '',
	'without TAU_MAX_FIXPOINT_STEPS the option keeps its default: '
		+ JSON.stringify(unset));

const seven = load_with('7');
check(seven !== null && seven.option === '7' && seven.error === '',
	'TAU_MAX_FIXPOINT_STEPS=7 sets the option: ' + JSON.stringify(seven));

if (failed) {
	console.error('ENV TEST FAILED');
	process.exit(1);
}
console.log('ENV TEST PASSED');
