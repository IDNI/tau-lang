// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Embind surface for the WebAssembly build (D5 v1): string in, string out,
// plus an opaque handle for a steppable interpreter. No tref/htref crosses
// the JS boundary.

#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <boost/log/core.hpp>

#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

#include "tau.h"

using namespace idni;
using namespace idni::tau_lang;
using emscripten::val;
using emscripten::optional_override;

using node_t = tau_pack::node_t;
using tau_api = api<node_t>;

namespace {

std::unordered_map<int, std::unique_ptr<interpreter<node_t>>> g_interpreters;
int g_next_handle = 1;

// Set from a failed api result's report so a null/false return still
// tells a JS caller why, without the library printing anything itself.
std::string g_last_error;

// Runs once at module init: JS callers read errors from return values,
// not stderr.
void disable_logging() {
	boost::log::core::get()->set_logging_enabled(false);
}

template <typename Result>
void set_last_error(const Result& r) {
	std::ostringstream oss;
	oss << r.report();
	g_last_error = oss.str();
}

// The report text of a failed tau_init() or environment load, returned by
// every engine call so the module still loads and tells a JS caller why it
// cannot work.
std::string g_init_error;

// Clears the last error. False, with the init error as the last error, when
// the init failed.
bool begin_engine_call() {
	g_last_error.clear();
	if (g_init_error.empty()) return true;
	g_last_error = g_init_error;
	return false;
}

std::string js_get_last_error() {
	return g_last_error;
}

solver_mode parse_solver_mode(const std::string& mode) {
	if (mode == "maximum") return solver_mode::maximum;
	if (mode == "minimum") return solver_mode::minimum;
	return solver_mode::general;
}

boost::log::trivial::severity_level parse_severity(const std::string& lvl) {
	if (lvl == "trace") return boost::log::trivial::trace;
	if (lvl == "debug") return boost::log::trivial::debug;
	if (lvl == "error") return boost::log::trivial::error;
	return boost::log::trivial::info;
}

// Parses spec/formula/term and prints it back through the current
// pretty-printer settings.
val js_to_str(const std::string& expression) {
	if (!begin_engine_call()) return val::null();
	try {
		if (auto e = tau_api::get_spec_or_term(expression); e.has_value())
			return val(tau_api::to_str(e.value()));
		else set_last_error(e);
	} catch (const std::exception&) {}
	return val::null();
}

// Parses a full specification and prints it back, validating the input.
val js_get_spec(const std::string& spec) {
	if (!begin_engine_call()) return val::null();
	try {
		if (auto s = tau_api::get_spec(spec); s.has_value())
			return val(tau_api::to_str(s.value()));
		else set_last_error(s);
	} catch (const std::exception&) {}
	return val::null();
}

val js_normalize_formula(const std::string& formula) {
	if (!begin_engine_call()) return val::null();
	try {
		if (auto r = tau_api::normalize_formula(formula); r.has_value())
			return val(*r);
		else set_last_error(r);
	} catch (const std::exception&) {}
	return val::null();
}

bool js_sat(const std::string& formula) {
	if (!begin_engine_call()) return false;
	try {
		auto r = tau_api::sat(formula);
		if (r.has_value()) return r.value();
		set_last_error(r);
		return false;
	} catch (const std::exception&) { return false; }
}

bool js_unsat(const std::string& formula) {
	if (!begin_engine_call()) return false;
	try {
		auto r = tau_api::unsat(formula);
		if (r.has_value()) return r.value();
		set_last_error(r);
		return false;
	} catch (const std::exception&) { return false; }
}

bool js_valid(const std::string& formula) {
	if (!begin_engine_call()) return false;
	try {
		auto r = tau_api::valid(formula);
		if (r.has_value()) return r.value();
		set_last_error(r);
		return false;
	} catch (const std::exception&) { return false; }
}

val js_solve(const std::string& formula, const std::string& mode) {
	if (!begin_engine_call()) return val::null();
	try {
		auto r = tau_api::solve(formula, parse_solver_mode(mode));
		if (!r.has_value()) { set_last_error(r); return val::null(); }
		val out = val::object();
		for (auto& [var, value] : *r) out.set(var, value);
		return out;
	} catch (const std::exception&) {}
	return val::null();
}

int js_interpreter_create(const std::string& spec) {
	if (!begin_engine_call()) return 0;
	try {
		auto interp = tau_api::get_interpreter(spec);
		if (!interp.has_value()) { set_last_error(interp); return 0; }
		int handle = g_next_handle++;
		g_interpreters[handle] = std::make_unique<interpreter<node_t>>(
			std::move(*interp));
		return handle;
	} catch (const std::exception&) { return 0; }
}

// inputs: a plain JS object mapping input stream name to its value string
// at the interpreter's current time point.
val js_interpreter_step(int handle, val inputs) {
	if (!begin_engine_call()) return val::null();
	auto it = g_interpreters.find(handle);
	if (it == g_interpreters.end()) return val::null();
	try {
		auto& interp = *it->second;
		std::map<stream_at, std::string> step_inputs;
		val keys = val::global("Object").call<val>("keys", inputs);
		unsigned n = keys["length"].as<unsigned>();
		for (unsigned k = 0; k < n; ++k) {
			std::string name = keys[k].as<std::string>();
			step_inputs[{name, stream_pos(interp.time_point)}] =
				inputs[name].as<std::string>();
		}
		auto r = tau_api::step(interp, std::move(step_inputs),
			/*interactive=*/false);
		if (!r.has_value()) { set_last_error(r); return val::null(); }
		val out = val::object();
		for (auto& [sa, value] : *r) out.set(sa.name, value);
		out.set("state", static_cast<double>(interp.time_point));
		return out;
	} catch (const std::exception&) { return val::null(); }
}

val js_interpreter_input_vars(int handle) {
	if (!begin_engine_call()) return val::null();
	auto it = g_interpreters.find(handle);
	if (it == g_interpreters.end()) return val::null();
	try {
		auto r = tau_api::get_inputs_for_step(*it->second);
		if (!r.has_value()) { set_last_error(r); return val::null(); }
		val out = val::array();
		size_t i = 0;
		for (auto& sa : *r) out.set(i++, sa.name);
		return out;
	} catch (const std::exception&) { return val::null(); }
}

void js_interpreter_free(int handle) {
	g_interpreters.erase(handle);
}

// The text of option @p name, or null with the reason in get_last_error()
// when the repository declares no such option.
val js_get_option(const std::string& name) {
	if (!begin_engine_call()) return val::null();
	auto r = idni::options().get_text(name);
	if (!r.has_value()) { set_last_error(r); return val::null(); }
	return val(r.value());
}

// Writes option @p name from @p text and gives the text it reads after the
// write, or null with the reason in get_last_error().
val js_set_option(const std::string& name, const std::string& text) {
	if (!begin_engine_call()) return val::null();
	auto r = idni::options().set_text(name, text);
	if (!r.has_value()) { set_last_error(r); return val::null(); }
	return js_get_option(name);
}

val js_option_names() {
	val out = val::array();
	size_t i = 0;
	for (const std::string& name : idni::options().names())
		out.set(i++, name);
	return out;
}

// Runtime budgets and engine switches, each under the name of its api setter.
// Each forwards to the api setter the CLI option
// and the REPL `set` option of the same meaning write, and each count has
// a getter of the same name with `get` for `set`, which reads back the value
// in force.
//
// The api setters of the ltlsynt route are not bound: this build has no
// process model, so ltlsynt never runs and the ltlsynt timeout, the choice
// of ltlsynt encoding, the caps on what ltlsynt returns (HOA states, HOA
// guard cubes, strategy paths of the window oracle), the observation props
// assumed into the skeleton it is given, and every bound of the data game
// played on its game (closed regions, BDD nodes and memo, tabulated values,
// the Mealy view and the table `tau compile` carries of it) have nothing to
// act on. The two LTL(ABA) budgets that act before or without ltlsynt
// (set_ltl_qe_max_vars, set_ltl_max_refinement_rounds) are bound.
struct count_setter {
	const char* name;
	void (*set)(size_t);
	size_t (*get)();
};
constexpr count_setter count_setters[] = {
	{ "set_block_max_splits", &tau_api::set_block_max_splits,
		&tau_api::get_block_max_splits },
	{ "set_block_max_rounds", &tau_api::set_block_max_rounds,
		&tau_api::get_block_max_rounds },
	{ "set_cqe_max_clauses", &tau_api::set_cqe_max_clauses,
		&tau_api::get_cqe_max_clauses },
	{ "set_lgrs_max_vars", &tau_api::set_lgrs_max_vars,
		&tau_api::get_lgrs_max_vars },
	{ "set_block_squeeze_cap", &tau_api::set_block_squeeze_cap,
		&tau_api::get_block_squeeze_cap },
	{ "set_max_fixpoint_steps", &tau_api::set_max_fixpoint_steps,
		&tau_api::get_max_fixpoint_steps },
	{ "set_max_flag_search_steps", &tau_api::set_max_flag_search_steps,
		&tau_api::get_max_flag_search_steps },
	{ "set_max_def_passes", &tau_api::set_max_def_passes,
		&tau_api::get_max_def_passes },
	{ "set_max_enum_steps", &tau_api::set_max_enum_steps,
		&tau_api::get_max_enum_steps },
	{ "set_max_probe_steps", &tau_api::set_max_probe_steps,
		&tau_api::get_max_probe_steps },
	{ "set_max_rewrite_rounds", &tau_api::set_max_rewrite_rounds,
		&tau_api::get_max_rewrite_rounds },
	{ "set_max_simplify_rounds", &tau_api::set_max_simplify_rounds,
		&tau_api::get_max_simplify_rounds },
	{ "set_gc_min_size", &tau_api::set_gc_min_size, &tau_api::get_gc_min_size },
	{ "set_tref_budget", &tau_api::set_tref_budget, &tau_api::get_tref_budget },
	{ "set_tref_budget_soft_percent", &tau_api::set_tref_budget_soft_percent,
		&tau_api::get_tref_budget_soft_percent },
	{ "set_spec_size_warn", &tau_api::set_spec_size_warn,
		&tau_api::get_spec_size_warn },
	{ "set_max_revision_alts", &tau_api::set_max_revision_alts,
		&tau_api::get_max_revision_alts },
	{ "set_max_consistency_subsets", &tau_api::set_max_consistency_subsets,
		&tau_api::get_max_consistency_subsets },
	{ "set_cache_bound", &tau_api::set_cache_bound, &tau_api::get_cache_bound },
	{ "set_max_cover_products", &tau_api::set_max_cover_products,
		&tau_api::get_max_cover_products },
	{ "set_max_constant_size", &tau_api::set_max_constant_size,
		&tau_api::get_max_constant_size },
	{ "set_ltl_qe_max_vars", &tau_api::set_ltl_qe_max_vars,
		&tau_api::get_ltl_qe_max_vars },
	{ "set_ltl_max_refinement_rounds", &tau_api::set_ltl_max_refinement_rounds,
		&tau_api::get_ltl_max_refinement_rounds },
	{ "set_bf_dependence_max_nodes", &tau_api::set_bf_dependence_max_nodes,
		&tau_api::get_bf_dependence_max_nodes },
	{ "set_ba_decision_pins", &tau_api::set_ba_decision_pins,
		&tau_api::get_ba_decision_pins },
};

struct flag_setter {
	const char* name;
	void (*set)(bool);
};
constexpr flag_setter flag_setters[] = {
	{ "set_preprocessing", &tau_api::set_preprocessing },
	{ "set_ba_component_factoring", &tau_api::set_ba_component_factoring },
	{ "set_pwr_semantic_fallback", &tau_api::set_pwr_semantic_fallback },
	{ "set_step_definitional_propagation",
		&tau_api::set_step_definitional_propagation },
};

} // namespace

EMSCRIPTEN_BINDINGS(tau) {
	disable_logging();
	// get_last_error() after the load shows the warnings of a successful init
	auto init = tau_init<node_t>();
	if (init.has_value()) init.merge(idni::options().load_env("TAU_"));
	set_last_error(init);
	if (!init.has_value()) g_init_error = g_last_error;

	emscripten::function("get_spec", &js_get_spec);
	emscripten::function("normalize_formula", &js_normalize_formula);
	emscripten::function("sat", &js_sat);
	emscripten::function("unsat", &js_unsat);
	emscripten::function("valid", &js_valid);
	emscripten::function("solve", &js_solve);
	emscripten::function("to_str", &js_to_str);
	emscripten::function("get_last_error", &js_get_last_error);

	emscripten::function("set_charvar", &tau_api::set_charvar);
	emscripten::function("reset_definitions", &tau_api::reset_definitions);
	emscripten::function("reset", optional_override(
		[]() { return static_cast<double>(tau_api::reset()); }));
	emscripten::function("set_indenting", &tau_api::set_indenting);
	emscripten::function("set_highlighting", &tau_api::set_highlighting);
	emscripten::function("set_colors", &tau_api::set_colors);
	emscripten::function("set_json", &tau_api::set_json);
	emscripten::function("set_severity", optional_override(
		[](const std::string& lvl) {
			tau_api::set_severity(parse_severity(lvl));
		}));

	for (const count_setter& s : count_setters) {
		emscripten::function(s.name, s.set);
		const std::string get = std::string("get") + (s.name + 3);
		emscripten::function(get.c_str(), s.get);
	}
	for (const flag_setter& s : flag_setters)
		emscripten::function(s.name, s.set);
	emscripten::function("set_gc_growth_factor",
		&tau_api::set_gc_growth_factor);
	emscripten::function("get_gc_growth_factor",
		&tau_api::get_gc_growth_factor);
	emscripten::function("tref_count", optional_override(
		[]() { return static_cast<double>(tau_api::tref_count()); }));
	emscripten::function("option_names", &js_option_names);
	emscripten::function("set_option", &js_set_option);
	emscripten::function("get_option", &js_get_option);

	emscripten::function("interpreter_create", &js_interpreter_create);
	emscripten::function("interpreter_step", &js_interpreter_step);
	emscripten::function("interpreter_input_vars", &js_interpreter_input_vars);
	emscripten::function("interpreter_free", &js_interpreter_free);
};
