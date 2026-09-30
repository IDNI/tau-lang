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
	g_last_error.clear();
	try {
		if (auto e = tau_api::get_spec_or_term(expression); e.has_value())
			return val(tau_api::to_str(e.value()));
		else set_last_error(e);
	} catch (const std::exception&) {}
	return val::null();
}

// Parses a full specification and prints it back, validating the input.
val js_get_spec(const std::string& spec) {
	g_last_error.clear();
	try {
		if (auto s = tau_api::get_spec(spec); s.has_value())
			return val(tau_api::to_str(s.value()));
		else set_last_error(s);
	} catch (const std::exception&) {}
	return val::null();
}

val js_normalize_formula(const std::string& formula) {
	g_last_error.clear();
	try {
		if (auto r = tau_api::normalize_formula(formula); r.has_value())
			return val(*r);
		else set_last_error(r);
	} catch (const std::exception&) {}
	return val::null();
}

bool js_sat(const std::string& formula) {
	g_last_error.clear();
	try {
		auto r = tau_api::sat(formula);
		if (r.has_value()) return r.value();
		set_last_error(r);
		return false;
	} catch (const std::exception&) { return false; }
}

bool js_unsat(const std::string& formula) {
	g_last_error.clear();
	try {
		auto r = tau_api::unsat(formula);
		if (r.has_value()) return r.value();
		set_last_error(r);
		return false;
	} catch (const std::exception&) { return false; }
}

bool js_valid(const std::string& formula) {
	g_last_error.clear();
	try {
		auto r = tau_api::valid(formula);
		if (r.has_value()) return r.value();
		set_last_error(r);
		return false;
	} catch (const std::exception&) { return false; }
}

val js_solve(const std::string& formula, const std::string& mode) {
	g_last_error.clear();
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
	g_last_error.clear();
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
	g_last_error.clear();
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
	g_last_error.clear();
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

// The value a BA-declared option holds after the call, or null with the
// reason in getLastError() when no algebra of the pack declares the name.
val js_ba_option_value(const result<size_t>& r) {
	if (r.has_value()) return val(static_cast<double>(r.value()));
	set_last_error(r);
	return val::null();
}

val js_set_ba_option(const std::string& name, double value) {
	g_last_error.clear();
	if (!(value >= 0)) {
		g_last_error = "set_ba_option: the value must be a "
			"non-negative number";
		return val::null();
	}
	try {
		return js_ba_option_value(tau_api::set_ba_option(name,
			static_cast<size_t>(value)));
	} catch (const std::exception&) { return val::null(); }
}

val js_get_ba_option(const std::string& name) {
	g_last_error.clear();
	try {
		return js_ba_option_value(tau_api::get_ba_option(name));
	} catch (const std::exception&) { return val::null(); }
}

val js_ba_option_names() {
	val out = val::array();
	size_t i = 0;
	for (const std::string& name : tau_api::ba_option_names())
		out.set(i++, name);
	return out;
}

// Runtime budgets and engine switches, under the camelCase form of the
// Python binding's names. Each forwards to the api setter the CLI option
// and the REPL `set` option of the same meaning write.
//
// The api setters of the ltlsynt route are not bound: this build has no
// process model, so ltlsynt never runs and the ltlsynt timeout, the choice
// of ltlsynt encoding, and the caps on what ltlsynt returns (HOA states,
// HOA guard cubes, strategy paths of the window oracle) have nothing to
// act on. The two LTL(ABA) budgets that act before or without ltlsynt
// (setLtlQeMaxVars, setLtlMaxRefinementRounds) are bound.
struct count_setter {
	const char* name;
	void (*set)(size_t);
};
constexpr count_setter count_setters[] = {
	{ "setBlockMaxSplits", &tau_api::set_block_max_splits },
	{ "setBlockMaxRounds", &tau_api::set_block_max_rounds },
	{ "setCqeMaxClauses", &tau_api::set_cqe_max_clauses },
	{ "setLgrsMaxVars", &tau_api::set_lgrs_max_vars },
	{ "setBlockSqueezeCap", &tau_api::set_block_squeeze_cap },
	{ "setMaxFixpointSteps", &tau_api::set_max_fixpoint_steps },
	{ "setMaxFlagSearchSteps", &tau_api::set_max_flag_search_steps },
	{ "setMaxDefPasses", &tau_api::set_max_def_passes },
	{ "setMaxEnumSteps", &tau_api::set_max_enum_steps },
	{ "setMaxProbeSteps", &tau_api::set_max_probe_steps },
	{ "setMaxRewriteRounds", &tau_api::set_max_rewrite_rounds },
	{ "setMaxSimplifyRounds", &tau_api::set_max_simplify_rounds },
	{ "setGcMinSize", &tau_api::set_gc_min_size },
	{ "setTrefBudget", &tau_api::set_tref_budget },
	{ "setTrefBudgetSoftPercent", &tau_api::set_tref_budget_soft_percent },
	{ "setSpecSizeWarn", &tau_api::set_spec_size_warn },
	{ "setMaxRevisionAlts", &tau_api::set_max_revision_alts },
	{ "setMaxConsistencySubsets", &tau_api::set_max_consistency_subsets },
	{ "setCacheBound", &tau_api::set_cache_bound },
	{ "setMaxCoverProducts", &tau_api::set_max_cover_products },
	{ "setMaxConstantSize", &tau_api::set_max_constant_size },
	{ "setLtlQeMaxVars", &tau_api::set_ltl_qe_max_vars },
	{ "setLtlMaxRefinementRounds",
		&tau_api::set_ltl_max_refinement_rounds },
	{ "setBaDecisionPins", &tau_api::set_ba_decision_pins },
};

struct flag_setter {
	const char* name;
	void (*set)(bool);
};
constexpr flag_setter flag_setters[] = {
	{ "setPreprocessing", &tau_api::set_preprocessing },
	{ "setBaComponentFactoring", &tau_api::set_ba_component_factoring },
	{ "setPwrSemanticFallback", &tau_api::set_pwr_semantic_fallback },
	{ "setStepDefinitionalPropagation",
		&tau_api::set_step_definitional_propagation },
};

} // namespace

EMSCRIPTEN_BINDINGS(tau) {
	disable_logging();

	emscripten::function("getSpec", &js_get_spec);
	emscripten::function("normalizeFormula", &js_normalize_formula);
	emscripten::function("sat", &js_sat);
	emscripten::function("unsat", &js_unsat);
	emscripten::function("valid", &js_valid);
	emscripten::function("solve", &js_solve);
	emscripten::function("toStr", &js_to_str);
	emscripten::function("getLastError", &js_get_last_error);

	emscripten::function("setCharvar", &tau_api::set_charvar);
	emscripten::function("resetDefinitions", &tau_api::reset_definitions);
	emscripten::function("reset", optional_override(
		[]() { return static_cast<double>(tau_api::reset()); }));
	emscripten::function("setBlasting", &tau_api::set_preprocessing);
	emscripten::function("setIndenting", &tau_api::set_indenting);
	emscripten::function("setHighlighting", &tau_api::set_highlighting);
	emscripten::function("setColors", &tau_api::set_colors);
	emscripten::function("setJson", &tau_api::set_json);
	emscripten::function("setSeverity", optional_override(
		[](const std::string& lvl) {
			tau_api::set_severity(parse_severity(lvl));
		}));

	for (const count_setter& s : count_setters)
		emscripten::function(s.name, s.set);
	for (const flag_setter& s : flag_setters)
		emscripten::function(s.name, s.set);
	emscripten::function("setGcGrowthFactor",
		&tau_api::set_gc_growth_factor);
	// The one budget a caller reads back: the api setter writes this
	// variable directly, and 0 means unlimited.
	emscripten::function("getMaxConstantSize", optional_override(
		[]() { return static_cast<double>(max_constant_size); }));
	emscripten::function("trefCount", optional_override(
		[]() { return static_cast<double>(tau_api::tref_count()); }));
	emscripten::function("baOptionNames", &js_ba_option_names);
	emscripten::function("setBaOption", &js_set_ba_option);
	emscripten::function("getBaOption", &js_get_ba_option);

	emscripten::function("interpreterCreate", &js_interpreter_create);
	emscripten::function("interpreterStep", &js_interpreter_step);
	emscripten::function("interpreterInputVars", &js_interpreter_input_vars);
	emscripten::function("interpreterFree", &js_interpreter_free);
};
