#!/usr/bin/env node
// The JavaScript binding: decisions, an interpreter stepped by hand, options.
//
// Build the library first, from the repository root (it needs emsdk and a
// node that the emscripten output accepts):
//   ./dev preset release-wasm
//   node demos/demo_9.2-javascript.js
//
// Every call below comes from bindings/js/tau.node.js and bindings/js/README.md.
// This file has not been run as a demo.

const tauModule = require('../build/release-wasm/tau.js');

tauModule().then((tau) => {
	// A bad TAU_ variable does not stop the load; its reason is here.
	console.log('load error:', JSON.stringify(tau.get_last_error()));

	// sat, unsat and valid give true or false, and null when there is no verdict.
	console.log(tau.sat('x = 0 && x = 1'));
	console.log(tau.unsat('x = 0 && x = 1'));
	console.log(tau.valid('x = x'));
	console.log(tau.normalize_formula('x = 0 || x = 0'));
	console.log(tau.solve('x = 0', 'general'));

	// An interpreter is a handle. interpreter_input_vars names the inputs of the
	// next step and interpreter_step takes their values.
	const handle = tau.interpreter_create('o[t] = i[t].');
	for (let step = 1; step <= 3; step++) {
		const inputs = {};
		for (const name of tau.interpreter_input_vars(handle))
			inputs[name] = (step % 2 === 0) ? 'T.' : 'F.';
		console.log(tau.interpreter_step(handle, inputs).outputs.o);
	}
	tau.interpreter_free(handle);

	// The options have the names of the command line.
	console.log(tau.set_option('max-fixpoint-steps', '1000'));
	console.log(tau.get_option('max-fixpoint-steps'));
}).catch((e) => { console.error(e.stack); process.exit(1); });
