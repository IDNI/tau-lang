// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

// This module binds the public api (src/api.h) and nothing else. Every entry
// point forwards to an api<node> method. If the api does not cover a
// capability, add it to the api first, and then bind it here.

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/shared_ptr.h>

#include <nanobind/stl/bind_vector.h>
#include <nanobind/stl/bind_map.h>

#include <optional>
#include <sstream>
#include <string_view>

#include "tau.h"
#include "tau_pack.h"
#include "preferences.h"

namespace nb = nanobind;
using namespace nb::literals;

using node_t = idni::tau_lang::tau_pack::node_t;

using interpreter_t = idni::tau_lang::interpreter<node_t>;
using tau_api = idni::tau_lang::api<node_t>;

using stream_at = idni::tau_lang::stream_at;
using interpreter_options = idni::tau_lang::interpreter_options;

using serialized_constant_input_stream =
	idni::tau_lang::serialized_constant_input_stream;
using serialized_constant_output_stream =
	idni::tau_lang::serialized_constant_output_stream;
using console_input_stream = idni::tau_lang::console_input_stream;
using console_output_stream = idni::tau_lang::console_output_stream;
using console_prompt_input_stream = idni::tau_lang::console_prompt_input_stream;
using console_prompt_output_stream = idni::tau_lang::console_prompt_output_stream;
using file_input_stream = idni::tau_lang::file_input_stream;
using file_output_stream = idni::tau_lang::file_output_stream;
using vector_input_stream = idni::tau_lang::vector_input_stream;
using vector_output_stream = idni::tau_lang::vector_output_stream;

using input_streams_remap = idni::tau_lang::input_streams_remap;
using output_streams_remap = idni::tau_lang::output_streams_remap;

using report_t = idni::tau_lang::report;
using code_t   = idni::tau_lang::code;

// A report flattened into something nanobind can hand to Python. The three
// display bands are kept apart so a caller can show errors without losing
// the warnings and the timing/count scopes that came with a success.
struct py_report {
	std::vector<std::string> errors;
	std::vector<std::string> warnings;
	std::vector<std::string> infos;
	std::vector<int> codes;          // raw code tag per report node
	std::vector<std::string> code_names;
	bool has_error = false;
	// True when the report carries code::invalid_state, the
	// step-awaiting-input protocol (tau_diagnostics.h).
	bool awaiting_input = false;
	std::string text;                // operator<< rendering
};

static py_report make_py_report(const report_t& rep) {
	py_report out;
	out.has_error = rep.has_error();
	out.awaiting_input = idni::tau_lang::step_awaiting_input(rep);
	for (const auto& n : rep.nodes()) {
		out.codes.push_back(static_cast<int>(n.tag));
		out.code_names.emplace_back(idni::diagnostics::code_name(n.tag));
	}
	// These are data, not display, so render them without colour. `text`
	// keeps it: that field is the human rendering.
	static const idni::term::colors plain(false);
	rep.print(idni::diagnostics::sinks{
		.error   = [&](std::string_view l) { out.errors.emplace_back(l); },
		.warning = [&](std::string_view l) { out.warnings.emplace_back(l); },
		.info    = [&](std::string_view l) { out.infos.emplace_back(l); },
	}, plain);
	std::ostringstream os;
	os << rep;
	out.text = os.str();
	return out;
}

// Value plus report. Deliberately not an Optional and deliberately not a
// raise: raising on error throws away the warnings and the timed scopes that
// a *successful* call accumulates, which is exactly what a consumer wanting
// both the error and the timings needs. `unwrap()` is there for callers that
// do prefer an exception.
//
// The value is type-erased: nanobind binds concrete types, so a py_result<T>
// would need one Python class per T.
struct py_result {
	nb::object value;
	py_report report;
	bool has_value = false;
};

// Converts eagerly, so the value owns its data and outlives this result.
template <typename T>
static py_result to_py_result(idni::tau_lang::result<T>&& r) {
	py_result out;
	out.report = make_py_report(r.report());
	out.has_value = r.has_value();
	out.value = out.has_value
		? nb::cast(std::move(r).value(), nb::rv_policy::move)
		: nb::none();
	return out;
}

NB_MAKE_OPAQUE(input_streams_remap);
NB_MAKE_OPAQUE(output_streams_remap);

bool leak_warnings() {
#ifdef DEBUG
	return true;
#endif
	return false;
}

NB_MODULE(tau, m) {
	m.doc() = "Python bindings for the Tau public api (src/api.h). "
		"The module binds api methods only. Add a missing "
		"capability to the api first, and then bind it here.";

	nb::set_leak_warnings(leak_warnings());

	// The CLI's --color, for embedders. report fields are always plain
	// (see make_py_report); this governs what the engine writes to stdout.
	m.def("set_colors", [](bool state) { tau_api::set_colors(state); },
		"state"_a, "Enable or disable ANSI colour in engine output.");

	m.def("reset_definitions", []() { tau_api::reset_definitions(); },
		"Drop all registered definitions, stream declarations and stream "
		"types, so the next spec may type a stream differently. "
		"Interpreters already built keep their own streams.");

	m.def("reset", []() { return tau_api::reset(); },
		"Return the engine to a fresh state: drop the definitions, empty "
		"the caches and free every tree node nothing holds. Options keep "
		"their values. Release every interpreter first. Returns the "
		"number of tree nodes freed.");

	// Runtime budgets and engine switches. Each forwards to the api
	// setter of the same name, the one the CLI option and the REPL `set`
	// option of the same meaning write.
	struct count_setter {
		const char* name;
		void (*set)(size_t);
		const char* doc;
	};
	static constexpr count_setter count_setters[] = {
		{ "set_block_max_splits", &tau_api::set_block_max_splits,
			"Per-block Boole-decomposition split budget in "
			"anti-prenexing (--block-max-splits); 0 = unlimited." },
		{ "set_block_max_rounds", &tau_api::set_block_max_rounds,
			"Anti-prenexing quantifier-block driver rounds "
			"(--block-max-rounds); 0 = unlimited." },
		{ "set_cqe_max_clauses", &tau_api::set_cqe_max_clauses,
			"DNF clauses complete quantifier elimination may "
			"distribute one scope into (--cqe-max-clauses); "
			"0 = unlimited." },
		{ "set_lgrs_max_vars", &tau_api::set_lgrs_max_vars,
			"Distinct variables above which a pure-equality "
			"bitvector system goes to the solver instead of being "
			"solved algebraically per width (--lgrs-max-vars); "
			"default 8, 0 = unlimited." },
		{ "set_block_squeeze_cap", &tau_api::set_block_squeeze_cap,
			"Operand-set size above which block squeezing declines "
			"(--block-squeeze-cap); 0 = unlimited." },
		{ "set_max_fixpoint_steps", &tau_api::set_max_fixpoint_steps,
			"Temporal-normalization fixpoint steps "
			"(--max-fixpoint-steps); default 500, 0 = unlimited." },
		{ "set_max_flag_search_steps",
			&tau_api::set_max_flag_search_steps,
			"Eventual-flag search steps past the flag boundary "
			"(--max-flag-search-steps); default 500, "
			"0 = unlimited." },
		{ "set_max_def_passes", &tau_api::set_max_def_passes,
			"Definition-expansion passes (--max-def-passes); "
			"0 = unlimited." },
		{ "set_max_enum_steps", &tau_api::set_max_enum_steps,
			"Recurrence-relation enumeration steps "
			"(--max-enum-steps); 0 = unlimited." },
		{ "set_max_probe_steps", &tau_api::set_max_probe_steps,
			"Untyped saturation probe steps over a residual "
			"recurrence reference (--max-probe-steps); default "
			"10000, 0 = unlimited." },
		{ "set_max_rewrite_rounds", &tau_api::set_max_rewrite_rounds,
			"Rewrite-to-fixpoint rounds (--max-rewrite-rounds); "
			"0 = unlimited." },
		{ "set_max_simplify_rounds",
			&tau_api::set_max_simplify_rounds,
			"Bitvector simplification rewrite rounds "
			"(--max-simplify-rounds); 0 = unlimited." },
		{ "set_gc_min_size", &tau_api::set_gc_min_size,
			"Tree-node count floor before the interpreter's gc may "
			"trigger (--gc-min-size); default 256." },
		{ "set_tref_budget", &tau_api::set_tref_budget,
			"Cap on live interned tree nodes (--tref-budget); a call "
			"that starts with the store at or above it fails "
			"without running. 0 = unlimited." },
		{ "set_tref_budget_soft_percent",
			&tau_api::set_tref_budget_soft_percent,
			"Percentage of the tref budget at which a sweep is "
			"forced (--tref-budget-soft); default 75." },
		{ "set_spec_size_warn", &tau_api::set_spec_size_warn,
			"Warn when an updated specification exceeds this many "
			"characters (--spec-size-warn); 0 = off." },
		{ "set_max_revision_alts", &tau_api::set_max_revision_alts,
			"Revision alternatives kept per specification part "
			"(--max-revision-alts); 0 = unlimited." },
		{ "set_max_consistency_subsets",
			&tau_api::set_max_consistency_subsets,
			"k-ary consistency subset checks per atom group in "
			"LTL(ABA) synthesis (--max-consistency-subsets); "
			"default 4096, 0 = unlimited." },
		{ "set_cache_bound", &tau_api::set_cache_bound,
			"Bound of the string-keyed synthesis caches "
			"(--cache-bound); default 4096, 0 = unbounded." },
		{ "set_max_cover_products", &tau_api::set_max_cover_products,
			"The ABA oracle's mixed-type coverage expansion "
			"(--max-cover-products); default 256, 0 = unlimited." },
		{ "set_ltl_qe_max_vars", &tau_api::set_ltl_qe_max_vars,
			"Free-variable cap of the omcat QE fast path "
			"(--ltl-qe-max-vars); above 2 is not sound, 0 falls "
			"back to TAU_LTL_OMCAT_QE_MAX_VARS or 2." },
		{ "set_ltl_hoa_max_states", &tau_api::set_ltl_hoa_max_states,
			"Largest state count accepted from an ltlsynt HOA "
			"strategy (--ltl-hoa-max-states); 0 = unlimited." },
		{ "set_ltl_guard_max_cubes",
			&tau_api::set_ltl_guard_max_cubes,
			"DNF cubes a HOA guard may expand into in the Algorithm "
			"D game (--ltl-guard-max-cubes); 0 = unlimited." },
		{ "set_ltl_max_refinement_rounds",
			&tau_api::set_ltl_max_refinement_rounds,
			"ABA-oracle refinement rounds of a realizability check "
			"(--ltl-refinement-rounds); the cap answers UNKNOWN. "
			"0 = unlimited." },
		{ "set_ltl_window_max_paths",
			&tau_api::set_ltl_window_max_paths,
			"Strategy paths the multi-step window oracle examines "
			"per check (--ltl-window-max-paths); 0 = unlimited." },
		{ "set_ba_decision_pins", &tau_api::set_ba_decision_pins,
			"Decided tau-algebra rows whose key tree is kept alive "
			"across the step sweep (--ba-decision-pins); default "
			"4096, 0 = none." },
	};
	for (const count_setter& s : count_setters)
		m.def(s.name, s.set, "n"_a, s.doc);

	struct flag_setter {
		const char* name;
		void (*set)(bool);
		const char* doc;
	};
	static constexpr flag_setter flag_setters[] = {
		{ "set_preprocessing", &tau_api::set_preprocessing,
			"Master BA preprocessing switch (--preprocessing); a "
			"BA's own pass needs its own option on too, e.g. "
			"bv-blasting." },
		{ "set_ba_component_factoring",
			&tau_api::set_ba_component_factoring,
			"Decide tau-algebra constants per support component "
			"(--ba-component-factoring)." },
		{ "set_pwr_semantic_fallback",
			&tau_api::set_pwr_semantic_fallback,
			"Semantic fallback of the temporal pointwise revision "
			"(--pwr-semantic); off by default." },
		{ "set_step_definitional_propagation",
			&tau_api::set_step_definitional_propagation,
			"Propagate the constants a step formula determines "
			"before its paths are enumerated "
			"(--step-definitional-propagation); on by default." },
	};
	for (const flag_setter& s : flag_setters)
		m.def(s.name, s.set, "state"_a, s.doc);

	m.def("set_gc_growth_factor", &tau_api::set_gc_growth_factor,
		"factor"_a, "Growth factor of the interpreter's gc trigger "
		"(--gc-growth-factor); default 1.5, <= 0 disables gc.");
	m.def("set_ltl_timeout_sec", &tau_api::set_ltl_timeout_sec,
		"seconds"_a, "Wall-clock cap on each ltlsynt call "
		"(--ltl-timeout); 0 disables the watchdog, a negative value "
		"falls back to TAU_LTL_TIMEOUT_SEC or 60.");
	m.def("set_ltl_algorithm", &tau_api::set_ltl_algorithm,
		"algorithm"_a, "Omcat synthesis algorithm: A, B, D or auto "
		"(--ltl-alg); the empty string falls back to TAU_LTL_ALG.");
	m.def("tref_count", &tau_api::tref_count,
		"Live interned tree node count, what set_tref_budget caps.");

	// Options an algebra of the pack declares about itself, named
	// <family>-<option> as on the command line (bv-widening, qlt-t3-cap).
	m.def("ba_option_names", &tau_api::ba_option_names,
		"Names of the options the algebras of this build declare.");
	m.def("set_ba_option",
		[](const std::string& name, size_t value) {
			return to_py_result(tau_api::set_ba_option(name, value));
		}, "name"_a, "value"_a,
		"Set a BA-declared option: a flag takes 0 or 1, a count its "
		"number. Returns a result carrying the value now in force; no "
		"value, and the reason in the report, when no algebra of this "
		"build declares the name.");
	m.def("get_ba_option",
		[](const std::string& name) {
			return to_py_result(tau_api::get_ba_option(name));
		}, "name"_a,
		"The value of a BA-declared option (a flag reads 0 or 1), or no "
		"value when no algebra of this build declares the name.");

	// Stream at
	nb::class_<stream_at>(m, "stream_at")
		.def(nb::init<const std::string&, size_t>())
		.def_rw("name", &stream_at::name)
		.def_rw("time_point", &stream_at::time_point);

	// Serialized constant input stream
	nb::class_<serialized_constant_input_stream>(m,
		"serialized_constant_input_stream");

	// Serialized constant output stream
	nb::class_<serialized_constant_output_stream>(m,
		"serialized_constant_output_stream");

	// Console input stream
	nb::class_<console_input_stream, serialized_constant_input_stream>(
						m, "console_input_stream")
		.def(nb::init<>());

	// Console output stream
	nb::class_<console_output_stream,
		serialized_constant_output_stream>(m, "console_output_stream")
		.def(nb::init<>());

	// Console prompt input stream
	nb::class_<console_prompt_input_stream,
		console_input_stream>(m, "console_prompt_input_stream")
		.def(nb::init<const std::string&>());

	// Console prompt output stream
	nb::class_<console_prompt_output_stream,
		console_output_stream>(m, "console_prompt_output_stream")
		.def(nb::init<const std::string&>());

	// File input stream
	nb::class_<file_input_stream,
		serialized_constant_input_stream>(m, "file_input_stream")
		.def(nb::init<const std::string&>());

	// File output stream
	nb::class_<file_output_stream,
		serialized_constant_output_stream>(m, "file_output_stream")
		.def(nb::init<const std::string&>());

	// Vector input stream
	nb::class_<vector_input_stream,
		serialized_constant_input_stream>(m, "vector_input_stream")
		.def(nb::init<>())
		.def(nb::init<const std::vector<std::string>&>())
		.def("put", &vector_input_stream::put);

	// Vector output stream
	nb::class_<vector_output_stream,
		serialized_constant_output_stream>(m, "vector_output_stream")
		.def(nb::init<>())
		.def("get_values", &vector_output_stream::get_values)
		.def("get", &vector_output_stream::get)
		.def("clear", &vector_output_stream::clear);

	// Input streams remap
	nb::bind_map<input_streams_remap>(m, "input_streams_remap");

	// Output streams remap
	nb::bind_map<output_streams_remap>(m, "output_streams_remap");

	// Interpreter options
	auto opts_class = nb::class_<interpreter_options>(
						m, "interpreter_options");
	opts_class.def(nb::init<>());
	opts_class.def_prop_ro("input_remaps",
		[](interpreter_options& self) -> auto& {
			return self.input_remaps;
		}, nb::rv_policy::reference_internal);
	opts_class.def_prop_ro("output_remaps",
		[](interpreter_options& self) -> auto& {
			return self.output_remaps;
		}, nb::rv_policy::reference_internal);

	// HOA edge / automaton — exposed for visualise_mealy_dot, determinise,
	// boundary_traces consumers on the Python side.
	using HoaEdge = idni::tau_lang::hoa_edge;
	using HoaAutomaton = idni::tau_lang::hoa_automaton;
	nb::class_<HoaEdge>(m, "HoaEdge")
		.def_ro("guard_label", &HoaEdge::guard_label)
		.def_ro("dst", &HoaEdge::dst)
		.def_ro("accepting", &HoaEdge::accepting);
	nb::class_<HoaAutomaton>(m, "HoaAutomaton")
		.def_ro("num_states", &HoaAutomaton::num_states)
		.def_ro("initial_state", &HoaAutomaton::initial_state)
		.def_ro("aps", &HoaAutomaton::aps)
		.def_ro("edges", &HoaAutomaton::edges)
		.def_ro("state_accepting", &HoaAutomaton::state_accepting);

	// Operator-preference types.
	using PreferenceEntry = idni::tau_lang::preference_entry;
	using PreferenceOrder = idni::tau_lang::preference_order;
	nb::class_<PreferenceEntry>(m, "PreferenceEntry")
		.def(nb::init<>())
		.def_rw("var_name", &PreferenceEntry::var_name)
		.def_rw("preferred_value", &PreferenceEntry::preferred_value);
	nb::class_<PreferenceOrder>(m, "PreferenceOrder")
		.def(nb::init<>())
		.def_rw("entries", &PreferenceOrder::entries);

	// Helper: serialise an assignment<node> to a Python-friendly
	// {var_name[t]: value_string} dict.
	auto assignment_to_dict =
		[](interpreter_t& i,
		   const idni::tau_lang::assignment<node_t>& a)
		-> std::map<std::string, std::string>
	{
		(void)i;  // not currently used; reserved for ctype-aware paths
		using tau_t = idni::tau_lang::tree<node_t>;
		std::map<std::string, std::string> out;
		for (const auto& [var, val] : a) {
			idni::tref trimmed = tau_t::trim(var);
			std::string name = idni::tau_lang::get_var_name<node_t>(
				trimmed);
			idni::int_t t = idni::tau_lang::get_io_time_point<node_t>(
				trimmed);
			std::string key = name + "[" + std::to_string(t) + "]";
			// TAU_TO_STR macro uses unqualified `tree<node>` — only
			// works inside idni::tau_lang::. From this lambda
			// (file-scope), spell out the fully-qualified call.
			out[key] = tau_t::get(val).to_str();
		}
		return out;
	};

	// Interpreter bindings.
	nb::class_<interpreter_t>(m, "interpreter")
		.def(nb::init<interpreter_t&&>())
		.def_ro("time_point", &interpreter_t::time_point)
		.def_prop_ro("spec_revision", [](const interpreter_t& self) {
			return tau_api::spec_revision(self);
		})
		.def("current_spec", [](const interpreter_t& self) {
			return tau_api::current_spec(self);
		}, "The interpreter's current specification as a string. "
		"Follows every applied update; not the `u` stream, which "
		"carries the incoming revision rather than the merged result.")

		// ── Inspection ──────────────────────────────────────────────
		.def("reset", &interpreter_t::reset,
			"Reset the interpreter to time t=0 (preserving spec / streams / cached_solution).")
		.def("current_state",
			[](const interpreter_t& i) {
				return to_py_result(i.current_state());
			},
			"Opaque Mealy-state index (0 if no Mealy strategy). "
			"Returns a result carrying the value and the report.")
		.def("accumulator_state",
			[](const interpreter_t& i, const std::string& name) {
				return to_py_result(i.accumulator_state(name));
			},
			"name"_a,
			"Read-only observability of a Def 5.7 accumulator by name.")
		.def("committed_approval_hash",
			[](const interpreter_t& i) {
				return tau_api::approval_hash(i);
			},
			"Operator-approval hash recorded by commit_realiser (empty if no commit).")

		// ── Mealy-strategy introspection ─────────────────────────────
		.def("visualise_mealy_dot",
			&interpreter_t::visualise_mealy_dot,
			"Emit the cached Mealy strategy as a Graphviz DOT graph.")
		.def("determinise", &interpreter_t::determinise,
			"Return the cached deterministic Mealy strategy automaton.")
		.def("boundary_traces", &interpreter_t::boundary_traces,
			"n"_a, "max_length"_a = 100,
			"Extract up to n simple paths from the initial Mealy state, longest first.")
		.def("commit_realiser", &interpreter_t::commit_realiser,
			"approval_hash"_a,
			"Record an operator-approval hash on the interpreter (Approach A3).")

		// ── PWR / runtime methods ────────────────────────────────────
		.def("can_extend",
			[](interpreter_t& i, const std::string& psi_str) {
				return to_py_result(tau_api::can_extend(i, psi_str));
			}, "psi"_a,
			"Per-revision realisability pre-check via syntactic PWR. "
			"Returns a result carrying the verdict and the report.")
		.def("admissible_outputs",
			[assignment_to_dict](interpreter_t& i,
			                     size_t max_results)
			{
				auto r = tau_api::admissible_outputs(i, max_results)
					.transform([&assignment_to_dict, &i](
						std::vector<idni::tau_lang::assignment<node_t>>&& outputs)
					{
						std::vector<std::map<std::string, std::string>> out;
						out.reserve(outputs.size());
						for (const auto& asgn : outputs)
							out.push_back(assignment_to_dict(i, asgn));
						return out;
					});
				return to_py_result(std::move(r));
			}, "max_results"_a = 1024,
			"Enumerate output assignments admissible at the current step (non-advancing).")

		// PWR update: pass the running-spec revision formula `psi`. F6:
		// fires between turns (via the user's call cadence), never mid-token.
		.def("update",
			[](interpreter_t& i, const std::string& psi_str) {
				// The interpreter's own verdict travels through: a
				// rejected (unsat or ill-typed) revision reads False.
				return to_py_result(tau_api::update(i, psi_str));
			}, "psi"_a,
			"Apply pointwise revision: merge the running spec with `psi`. "
			"Returns a result carrying the interpreter's verdict and "
			"the report; the value is False when the revision was "
			"rejected and the running spec is left unchanged.");

	// Decision procedures over a full spec, parsed as get_interpreter
	// parses it. A backend failure (ltlsynt missing or timed out, a
	// budget, an undecidable shape) is not a verdict: the result stays
	// valueless instead of degrading to False, and its report says why
	// (code_names: parse_error / invalid_argument for bad input,
	// solver_error for UNKNOWN).
	auto decide = [](const std::string& spec_str, auto&& procedure) {
		idni::tau_lang::result<bool> r;
		if (auto fm = r.merge_take(tau_api::get_spec(spec_str)))
			if (auto v = r.merge_take(procedure(*fm))) r = *v;
		return to_py_result(std::move(r));
	};
#define TAU_VERDICT_DOC " Returns a result whose value is the verdict, " \
	"or None (with the reason in the report) when the spec does not " \
	"parse or gets no verdict."

	m.def("sat", [decide](const std::string& spec) {
			return decide(spec, [](idni::tref fm) {
				return tau_api::sat(fm); });
		}, "spec"_a, "Tau satisfiability: for every input sequence "
		"there are outputs, step by step and not depending on future "
		"inputs, that satisfy the spec. A spec that some input makes "
		"contradictory is unsat. unsat implies unrealizable; realizable "
		"also decides the full-LTL operators as a game." TAU_VERDICT_DOC);
	m.def("unsat", [decide](const std::string& spec) {
			return decide(spec, [](idni::tref fm) {
				return tau_api::unsat(fm); });
		}, "spec"_a, "Negation of sat." TAU_VERDICT_DOC);
	m.def("valid", [decide](const std::string& spec) {
			return decide(spec, [](idni::tref fm) {
				return tau_api::valid(fm); });
		}, "spec"_a, "Does every trace satisfy the spec?" TAU_VERDICT_DOC);
	m.def("realizable", [decide](const std::string& spec) {
			return decide(spec, [](idni::tref fm) {
				return tau_api::realizable(fm); });
		}, "spec"_a, "Can the outputs be chosen, step by "
		"step, so that the spec holds for every input sequence? "
		"get_interpreter rejects an unrealizable spec." TAU_VERDICT_DOC);
	m.def("unrealizable", [decide](const std::string& spec) {
			return decide(spec, [](idni::tref fm) {
				return tau_api::unrealizable(fm); });
		}, "spec"_a, "Negation of realizable." TAU_VERDICT_DOC);
	// The historical name of realizable.
	m.def("is_realizable", [decide](const std::string& spec) {
			return decide(spec, [](idni::tref fm) {
				return tau_api::realizable(fm); });
		}, "spec"_a, "Alias of realizable." TAU_VERDICT_DOC);

	m.def("unsat_core",
		[](const std::string& spec, bool realizability) {
			return to_py_result(tau_api::unsat_core(spec, realizability));
		}, "spec"_a, "realizability"_a = true,
		"A subset-minimal list of the spec's top-level conjuncts "
		"(always (A && B) split into always A, always B) that is "
		"already unrealizable (realizability=True) or unsatisfiable "
		"(False). An empty list when the spec has no conflict; None "
		"when the spec gets no verdict. A warning in the report means "
		"a sub-check got no verdict and the core may not be minimal.");
#undef TAU_VERDICT_DOC

	// Free function: apply_preferences.
	m.def("apply_preferences",
		[](const std::string& spec_str, const PreferenceOrder& po) {
			return to_py_result(tau_api::apply_preferences(spec_str, po));
		}, "spec"_a, "po"_a,
		"Strengthen a spec with operator preferences (lex-priority). "
		"Returns a result carrying the strengthened spec and the report.");

	// Diagnostics
	nb::class_<py_report>(m, "report",
		"Structured diagnostics for one api call: the three display "
		"bands kept apart, plus the raw code tags.")
		.def_ro("errors", &py_report::errors)
		.def_ro("warnings", &py_report::warnings)
		.def_ro("infos", &py_report::infos)
		.def_ro("codes", &py_report::codes)
		.def_ro("code_names", &py_report::code_names)
		.def_ro("has_error", &py_report::has_error)
		.def_ro("awaiting_input", &py_report::awaiting_input,
			"True when the call stopped because a step needs an "
			"input value (code::invalid_state).")
		.def("__str__", [](const py_report& r) { return r.text; })
		.def("__repr__", [](const py_report& r) {
			return "<tau.report errors=" +
				std::to_string(r.errors.size()) + " warnings=" +
				std::to_string(r.warnings.size()) + ">";
		});

	nb::class_<py_result>(m, "result",
		"What an api call produced plus its diagnostics report. "
		"Falsy when there is no value; `value` is None then.")
		.def_ro("value", &py_result::value)
		.def_ro("report", &py_result::report)
		.def("__bool__", [](const py_result& r) { return r.has_value; })
		.def("unwrap", [](const py_result& r) {
			if (!r.has_value)
				throw std::runtime_error(r.report.text);
			return r.value;
		}, "The value, or raise RuntimeError with the report.")
		.def("__repr__", [](const py_result& r) {
			return std::string("<tau.result ") + (r.has_value
				? "ok" : "empty") + " errors=" +
				std::to_string(r.report.errors.size()) + ">";
		});

	// API functions
	m.def("get_interpreter",
		[](const std::string& spec) {
			return to_py_result(tau_api::get_interpreter(spec));
		}, "specification"_a,
		"Create an interpreter from a specification string. "
		"Returns a result carrying the value and report.");

	m.def("get_interpreter",
		[](const std::string& spec, interpreter_options& opts) {
			return to_py_result(tau_api::get_interpreter(spec, opts));
		}, "specification"_a, "options"_a,
		"Create an interpreter from a specification string with "
		"options. Returns a result carrying the value and report.");

	m.def("get_inputs_for_step",
		[](interpreter_t& i) {
			return to_py_result(tau_api::get_inputs_for_step(i));
		}, "interpreter"_a,
		"Get the inputs needed for the next step. "
		"Returns a result carrying the value and report.");

	m.def("step",
		[](interpreter_t& i,
			const std::map<stream_at, std::string>& inputs)
		{
			return to_py_result(tau_api::step(i, inputs));
		}, "interpreter"_a, "inputs"_a,
		"Step the interpreter with given inputs. "
		"Returns a result carrying the value and report.");

	m.def("step",
		[](interpreter_t& i) {
			return to_py_result(tau_api::step(i));
		}, "interpreter"_a,
		"Step the interpreter without inputs (uses remapped streams). "
		"Returns a result carrying the value and report.");

}
