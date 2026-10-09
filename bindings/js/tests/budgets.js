#!/usr/bin/env node

// Runtime budgets, engine switches and BA-declared options of the wasm
// library: every setter the module binds takes a value and leaves later
// verdicts intact, the budgets with a cheap observable effect show it, and
// the options an algebra declares about itself read back what was set.
// The names are the names of the api setters.
//
// Every count setter has a getter that reads back the value in force, and a
// budget is also observed through what it changes: the tree-node budget
// refuses a call, and the fixpoint-step cap makes a query that needs more
// steps give up instead of answering. The give-up workload is the one of the
// REPL test test_repl-limit_effect-max_fixpoint_steps_giveup; the constant size
// budget's is the one of
// test_repl-run_cmd-value_past_constant_size_budget.
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
	set_block_max_splits: 0, set_block_max_rounds: 0, set_cqe_max_clauses: 0,
	set_lgrs_max_vars: 8, set_block_squeeze_cap: 0, set_max_fixpoint_steps: 500,
	set_max_flag_search_steps: 500, set_max_def_passes: 0,
	set_max_enum_steps: 0, set_max_probe_steps: 10000,
	set_max_rewrite_rounds: 0,
	set_max_simplify_rounds: 0, set_gc_min_size: 256, set_tref_budget: 0,
	set_tref_budget_soft_percent: 75, set_spec_size_warn: 0,
	set_max_revision_alts: 0, set_max_consistency_subsets: 4096,
	set_cache_bound: 4096, set_max_cover_products: 256,
	set_max_constant_size: 2000,
	set_ltl_qe_max_vars: 0,
	set_ltl_max_refinement_rounds: 64, set_bf_dependence_max_nodes: 65536,
	set_ba_decision_pins: 4096,
};

// What each getter reads after its setter took the restore value: the same
// value, except the QE cap, whose 0 falls back to its default 2.
const READ_BACK = { set_ltl_qe_max_vars: 2 };

// The flag setters and their default.
const FLAG_SETTERS = {
	set_preprocessing: true, set_ba_component_factoring: true,
	set_pwr_semantic_fallback: false, set_step_definitional_propagation: true,
};

// The api setters of the ltlsynt route, and of the data game
// played on ltlsynt's game. This build has no process model, so ltlsynt
// never runs and these are not bound.
const NOT_BOUND = [
	'set_ltl_timeout_sec', 'set_ltl_algorithm', 'set_ltl_hoa_max_states',
	'set_ltl_guard_max_cubes', 'set_ltl_window_max_paths',
	'set_ltl_closed_regions_timeout', 'set_ltl_data_game_max_nodes',
	'set_ltl_data_game_max_memo', 'set_ltl_data_game_max_combinations',
	'set_ltl_max_observations', 'set_ltl_mealy_max_states',
	'set_ltl_mealy_max_edges',
	'set_compile_max_table_edges',
];

function verdictsIntact(tau, label) {
	check(tau.sat('x = 0') === true && tau.sat('x = 0 && x = 1') === false,
		`sat still correct after ${label}`);
}

function runEverySetter(tau) {
	for (const [name, restore] of Object.entries(COUNT_SETTERS)) {
		const getter = 'get' + name.slice(3);
		check(typeof tau[name] === 'function', `${name} is bound`);
		check(typeof tau[getter] === 'function', `${getter} is bound`);
		check(tau[name](7) === undefined, `${name}(7) accepted`);
		check(tau[getter]() === 7, `${getter}() reads 7`);
		tau[name](restore);
		const back = READ_BACK[name] ?? restore;
		check(tau[getter]() === back, `${getter}() reads ${back} again`);
	}
	verdictsIntact(tau, 'every count setter');
	for (const [name, dflt] of Object.entries(FLAG_SETTERS)) {
		check(typeof tau[name] === 'function', `${name} is bound`);
		tau[name](!dflt);
		check(tau.sat('x = 0') === true, `sat("x = 0") with ${name}(${!dflt})`);
		tau[name](dflt);
	}
	tau.set_gc_growth_factor(2.0);
	check(tau.get_gc_growth_factor() === 2.0, 'get_gc_growth_factor() reads 2');
	tau.set_gc_growth_factor(1.5);
	verdictsIntact(tau, 'every flag setter and set_gc_growth_factor');
	for (const name of NOT_BOUND) {
		check(tau[name] === undefined, `${name} is not bound (no ltlsynt)`);
		check(tau['get' + name.slice(3)] === undefined,
			`get${name.slice(3)} is not bound (no ltlsynt)`);
	}
}

function runTrefBudget(tau) {
	const live = tau.tref_count();
	check(typeof live === 'number' && live > 0, `tref_count() -> ${live}`);
	tau.set_tref_budget(1);
	const refused = tau.sat('x = 0');
	const why = tau.get_last_error();
	const noInterpreter = tau.interpreter_create('o[t] = i[t].');
	tau.set_tref_budget(0);
	check(refused === false && why.includes('memory budget exhausted'),
		`set_tref_budget(1) refuses sat: ${JSON.stringify(why)}`);
	check(noInterpreter === 0, 'set_tref_budget(1) refuses interpreter_create');
	check(tau.sat('x = 0') === true, 'set_tref_budget(0) lifts the refusal');
}

// set_colors decides whether a diagnostic carries ANSI escapes.
function runColors(tau) {
	const refusal = (colors) => {
		tau.set_colors(colors);
		tau.set_tref_budget(1);
		tau.sat('x = 0');
		const why = tau.get_last_error();
		tau.set_tref_budget(0);
		return why;
	};
	const plain = refusal(false);
	const colored = refusal(true);
	check(plain.includes('memory budget exhausted') && !plain.includes('\u001b'),
		'set_colors(false) leaves the diagnostic plain: '
			+ JSON.stringify(plain));
	check(colored.includes('\u001b'),
		'set_colors(true) colors the diagnostic');
}

function runFixpointSteps(tau) {
	const spec = 'always o1[t] = o1[t-2]';
	tau.set_max_fixpoint_steps(1);
	const capped = tau.sat(spec);
	const why = tau.get_last_error();
	tau.set_max_fixpoint_steps(500);
	check(capped === false && why.includes('gave up before reaching a result'),
		`set_max_fixpoint_steps(1) gives up on ${JSON.stringify(spec)}`);
	// The give-up must not be remembered as the verdict.
	check(tau.sat(spec) === true,
		`set_max_fixpoint_steps(500) decides ${JSON.stringify(spec)}`);
}

// Steps the run whose values grow with every step until a step fails;
// returns that step's diagnostic, or null when every step ran.
function growingRun(tau, steps) {
	const h = tau.interpreter_create('always (o1[t] != o1[t-1] && '
		+ 'o1[t] != 0 && o1[t] != 1 && o1[t] != i1[t]).');
	if (h === 0) return 'interpreter_create: ' + tau.get_last_error();
	try {
		for (let k = 0; k < steps; k++)
			if (tau.interpreter_step(h, { i1: '<:a> = 0' }) === null)
				return tau.get_last_error();
		return null;
	} finally { tau.interpreter_free(h); }
}

function runMaxConstantSize(tau) {
	check(tau.get_max_constant_size() === 2000,
		`get_max_constant_size() -> ${tau.get_max_constant_size()} `
			+ '(default 2000)');
	tau.set_max_constant_size(300);
	check(tau.get_max_constant_size() === 300,
		'get_max_constant_size() reads 300 after set_max_constant_size(300)');
	tau.set_max_constant_size(0);
	check(tau.get_max_constant_size() === 0,
		'get_max_constant_size() reads 0 (unlimited) after '
			+ 'set_max_constant_size(0)');
	tau.set_max_constant_size(1);
	const capped = growingRun(tau, 3);
	tau.set_max_constant_size(2000);
	check(capped !== null && capped.includes('constant size budget'),
		'set_max_constant_size(1) stops the growing run at the budget');
	check(growingRun(tau, 1) === null,
		'set_max_constant_size(2000) runs its first step');
	check(tau.get_max_constant_size() === 2000,
		'get_max_constant_size() restored');
}

function runOptions(tau) {
	const names = tau.option_names();
	check(Array.isArray(names) && names.includes('max-fixpoint-steps'),
		`option_names() -> ${JSON.stringify(names)}`);
	for (const name of names) {
		const t = tau.get_option(name);
		check(typeof t === 'string', `get_option(${name}) -> ${t}`);
		// the key reads back as set or unset, not as itself
		if (name === 'nlang-api-key') continue;
		check(tau.set_option(name, t) === t,
			`set_option(${name}, ${t}) keeps it`);
	}
	check(tau.set_option('nope-nothing', 'x') === null
		&& tau.get_last_error().length > 0,
		'set_option refuses an undeclared name');
	check(tau.get_option('nope-nothing') === null
		&& tau.get_last_error().length > 0,
		'get_option refuses an undeclared name');
	check(tau.set_option('max-fixpoint-steps', 'x') === null
		&& tau.get_last_error().length > 0,
		'set_option refuses a text that is no count');

	// 0 is a valid value of the cell budgets: it lifts the bound.
	for (const [count, v] of [['qlt-t3-cap', '5'], ['qlt-cells-budget', '0'],
		['qlt-cells-max-params', '3']]) {
		check(names.includes(count),
			`${count} is declared (qlt is in the pack)`);
		if (!names.includes(count)) continue;
		const saved = tau.get_option(count);
		check(tau.set_option(count, v) === v,
			`set_option(${count}, ${v}) -> ${v}`);
		check(tau.get_option(count) === v, `get_option(${count}) reads ${v}`);
		check(tau.set_option(count, saved) === saved,
			`set_option(${count}, ${saved}) restores it`);
		check(tau.get_option(count) === saved,
			`get_option(${count}) reads ${saved} again`);
	}
}

const tauModule = require(WASM_JS);
tauModule().then((tau) => {
	try {
		runEverySetter(tau);
		runTrefBudget(tau);
		runColors(tau);
		runFixpointSteps(tau);
		runMaxConstantSize(tau);
		runOptions(tau);
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
