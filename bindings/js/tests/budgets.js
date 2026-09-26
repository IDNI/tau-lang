#!/usr/bin/env node

// Runtime budgets, engine switches and BA-declared options of the wasm
// library: every setter the module binds takes a value and leaves later
// verdicts intact, the budgets with a cheap observable effect show it, and
// the options an algebra declares about itself read back what was set.
// The names are the camelCase form of the Python binding's.
//
// The api has no getter for the core budgets, so a budget is observed
// through what it changes: the tree-node budget refuses a call, and the
// fixpoint-step cap makes a query that needs more steps give up instead of
// answering. The give-up workload is the one of the REPL test
// test_repl-limit_effect-fixpointsteps_giveup.
//
// Needs no native binary.

const path = require('path');

const WASM_JS = process.env.TAU_WASM_JS
	|| path.join(__dirname, '..', '..', '..', 'build', 'emscripten', 'tau.js');

let failed = false;
let checked = 0;

function check(cond, label) {
	checked++;
	if (cond) { console.log('OK   ' + label); }
	else { console.error('FAIL ' + label); failed = true; }
}

// The count setters and the value each is restored to afterwards: the
// shipped default, or 0 (unlimited / off) where that is the default.
const COUNT_SETTERS = {
	setBlockMaxSplits: 0, setBlockMaxRounds: 0, setCqeMaxClauses: 0,
	setLgrsMaxVars: 8, setBlockSqueezeCap: 0, setMaxFixpointSteps: 500,
	setMaxFlagSearchSteps: 500, setMaxDefPasses: 0, setMaxEnumSteps: 0,
	setMaxProbeSteps: 10000, setMaxRewriteRounds: 0,
	setMaxSimplifyRounds: 0, setGcMinSize: 256, setTrefBudget: 0,
	setTrefBudgetSoftPercent: 75, setSpecSizeWarn: 0,
	setMaxRevisionAlts: 0, setMaxConsistencySubsets: 4096,
	setCacheBound: 4096, setMaxCoverProducts: 256, setLtlQeMaxVars: 0,
	setLtlMaxRefinementRounds: 64, setBaDecisionPins: 4096,
};

// The flag setters and their default.
const FLAG_SETTERS = {
	setPreprocessing: true, setBaComponentFactoring: true,
	setPwrSemanticFallback: false, setStepDefinitionalPropagation: true,
};

// The Python binding's setters of the ltlsynt route. This build has no
// process model, so ltlsynt never runs and these are not bound.
const NOT_BOUND = [
	'setLtlTimeoutSec', 'setLtlAlgorithm', 'setLtlHoaMaxStates',
	'setLtlGuardMaxCubes', 'setLtlWindowMaxPaths',
];

function verdictsIntact(tau, label) {
	check(tau.sat('x = 0') === true && tau.sat('x = 0 && x = 1') === false,
		`sat still correct after ${label}`);
}

function runEverySetter(tau) {
	for (const [name, restore] of Object.entries(COUNT_SETTERS)) {
		check(typeof tau[name] === 'function', `${name} is bound`);
		check(tau[name](7) === undefined, `${name}(7) accepted`);
		tau[name](restore);
	}
	verdictsIntact(tau, 'every count setter');
	for (const [name, dflt] of Object.entries(FLAG_SETTERS)) {
		check(typeof tau[name] === 'function', `${name} is bound`);
		tau[name](!dflt);
		check(tau.sat('x = 0') === true, `sat("x = 0") with ${name}(${!dflt})`);
		tau[name](dflt);
	}
	tau.setGcGrowthFactor(2.0);
	tau.setGcGrowthFactor(1.5);
	verdictsIntact(tau, 'every flag setter and setGcGrowthFactor');
	for (const name of NOT_BOUND)
		check(tau[name] === undefined, `${name} is not bound (no ltlsynt)`);
}

function runTrefBudget(tau) {
	const live = tau.trefCount();
	check(typeof live === 'number' && live > 0, `trefCount() -> ${live}`);
	tau.setTrefBudget(1);
	const refused = tau.sat('x = 0');
	const why = tau.getLastError();
	const noInterpreter = tau.interpreterCreate('o[t] = i[t].');
	tau.setTrefBudget(0);
	check(refused === false && why.includes('memory budget exhausted'),
		`setTrefBudget(1) refuses sat: ${JSON.stringify(why)}`);
	check(noInterpreter === 0, 'setTrefBudget(1) refuses interpreterCreate');
	check(tau.sat('x = 0') === true, 'setTrefBudget(0) lifts the refusal');
}

// setColors decides whether a diagnostic carries ANSI escapes.
function runColors(tau) {
	const refusal = (colors) => {
		tau.setColors(colors);
		tau.setTrefBudget(1);
		tau.sat('x = 0');
		const why = tau.getLastError();
		tau.setTrefBudget(0);
		return why;
	};
	const plain = refusal(false);
	const colored = refusal(true);
	check(plain.includes('memory budget exhausted') && !plain.includes('\u001b'),
		`setColors(false) leaves the diagnostic plain: ${JSON.stringify(plain)}`);
	check(colored.includes('\u001b'),
		'setColors(true) colors the diagnostic');
}

function runFixpointSteps(tau) {
	const spec = 'always o1[t] = o1[t-2]';
	tau.setMaxFixpointSteps(1);
	const capped = tau.sat(spec);
	const why = tau.getLastError();
	tau.setMaxFixpointSteps(500);
	check(capped === false && why.includes('gave up before reaching a result'),
		`setMaxFixpointSteps(1) gives up on ${JSON.stringify(spec)}`);
	// The give-up must not be remembered as the verdict.
	check(tau.sat(spec) === true,
		`setMaxFixpointSteps(500) decides ${JSON.stringify(spec)}`);
}

function runBaOptions(tau) {
	const names = tau.baOptionNames();
	check(Array.isArray(names) && names.length > 0,
		`baOptionNames() -> ${JSON.stringify(names)}`);
	check(names.every((n) => n.includes('-')),
		'every BA option is named <family>-<option>');
	for (const name of names) {
		const v = tau.getBaOption(name);
		check(typeof v === 'number', `getBaOption(${name}) -> ${v}`);
		check(tau.setBaOption(name, v) === v,
			`setBaOption(${name}, ${v}) keeps it`);
	}
	check(tau.setBaOption('nope-nothing', 1) === null
		&& tau.getLastError().length > 0,
		'setBaOption refuses an undeclared name');
	check(tau.getBaOption('nope-nothing') === null
		&& tau.getLastError().length > 0,
		'getBaOption refuses an undeclared name');
	check(tau.setBaOption('nope-nothing', -1) === null,
		'setBaOption refuses a negative value');

	const count = 'qlt-t3-cap';
	check(names.includes(count), `${count} is declared (qlt is in the pack)`);
	if (!names.includes(count)) return;
	const saved = tau.getBaOption(count);
	check(tau.setBaOption(count, 5) === 5, `setBaOption(${count}, 5) -> 5`);
	check(tau.getBaOption(count) === 5, `getBaOption(${count}) reads 5`);
	check(tau.setBaOption(count, saved) === saved,
		`setBaOption(${count}, ${saved}) restores it`);
	check(tau.getBaOption(count) === saved,
		`getBaOption(${count}) reads ${saved} again`);
}

const tauModule = require(WASM_JS);
tauModule().then((tau) => {
	try {
		runEverySetter(tau);
		runTrefBudget(tau);
		runColors(tau);
		runFixpointSteps(tau);
		runBaOptions(tau);
	} catch (e) {
		console.error('EXCEPTION: ' + e.stack);
		failed = true;
	}

	console.log(`${checked} checks run.`);
	if (failed) {
		console.error('BUDGETS TEST FAILED');
		process.exit(1);
	}
	console.log('BUDGETS TEST PASSED');
}).catch((e) => {
	console.error('MODULE LOAD FAILED: ' + e.stack);
	process.exit(1);
});
