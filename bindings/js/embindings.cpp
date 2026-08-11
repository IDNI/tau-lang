// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Embind surface for the WebAssembly build (D5 v1): string in, string out,
// plus an opaque handle for a steppable interpreter. No tref/htref crosses
// the JS boundary.

#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <boost/log/core.hpp>

#include <memory>
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

// Runs once at module init: JS callers read errors from return values,
// not stderr.
void disable_logging() {
	boost::log::core::get()->set_logging_enabled(false);
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
	try {
		if (tref e = tau_api::get_spec_or_term(expression); e)
			return val(tau_api::to_str(e));
	} catch (const std::exception&) {}
	return val::null();
}

// Parses a full specification and prints it back, validating the input.
val js_get_spec(const std::string& spec) {
	try {
		if (tref s = tau_api::get_spec(spec); s)
			return val(tau_api::to_str(s));
	} catch (const std::exception&) {}
	return val::null();
}

val js_normalize_formula(const std::string& formula) {
	try {
		if (auto r = tau_api::normalize_formula(formula); r)
			return val(*r);
	} catch (const std::exception&) {}
	return val::null();
}

bool js_sat(const std::string& formula) {
	try { return tau_api::sat(formula); }
	catch (const std::exception&) { return false; }
}

bool js_unsat(const std::string& formula) {
	try { return tau_api::unsat(formula); }
	catch (const std::exception&) { return false; }
}

bool js_valid(const std::string& formula) {
	try { return tau_api::valid(formula); }
	catch (const std::exception&) { return false; }
}

val js_solve(const std::string& formula, const std::string& mode) {
	try {
		auto r = tau_api::solve(formula, parse_solver_mode(mode));
		if (!r) return val::null();
		val out = val::object();
		for (auto& [var, value] : *r) out.set(var, value);
		return out;
	} catch (const std::exception&) {}
	return val::null();
}

int js_interpreter_create(const std::string& spec) {
	try {
		auto interp = tau_api::get_interpreter(spec);
		if (!interp) return 0;
		int handle = g_next_handle++;
		g_interpreters[handle] = std::make_unique<interpreter<node_t>>(
			std::move(*interp));
		return handle;
	} catch (const std::exception&) { return 0; }
}

// inputs: a plain JS object mapping input stream name to its value string
// at the interpreter's current time point.
val js_interpreter_step(int handle, val inputs) {
	auto it = g_interpreters.find(handle);
	if (it == g_interpreters.end()) return val::null();
	try {
		auto& interp = *it->second;
		std::map<stream_at, std::string> step_inputs;
		val keys = val::global("Object").call<val>("keys", inputs);
		unsigned n = keys["length"].as<unsigned>();
		for (unsigned k = 0; k < n; ++k) {
			std::string name = keys[k].as<std::string>();
			step_inputs[{name, interp.time_point}] =
				inputs[name].as<std::string>();
		}
		auto r = tau_api::step(interp, std::move(step_inputs),
			/*interactive=*/false);
		if (!r) return val::null();
		val out = val::object();
		for (auto& [sa, value] : *r) out.set(sa.name, value);
		out.set("state", static_cast<double>(interp.time_point));
		return out;
	} catch (const std::exception&) { return val::null(); }
}

val js_interpreter_input_vars(int handle) {
	auto it = g_interpreters.find(handle);
	if (it == g_interpreters.end()) return val::null();
	try {
		val out = val::array();
		size_t i = 0;
		for (auto& sa : tau_api::get_inputs_for_step(*it->second))
			out.set(i++, sa.name);
		return out;
	} catch (const std::exception&) { return val::null(); }
}

void js_interpreter_free(int handle) {
	g_interpreters.erase(handle);
}

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

	emscripten::function("setCharvar", &tau_api::set_charvar);
	emscripten::function("setBlasting", &tau_api::set_blasting);
	emscripten::function("setIndenting", &tau_api::set_indenting);
	emscripten::function("setHighlighting", &tau_api::set_highlighting);
	emscripten::function("setJson", &tau_api::set_json);
	emscripten::function("setSeverity", optional_override(
		[](const std::string& lvl) {
			tau_api::set_severity(parse_severity(lvl));
		}));

	emscripten::function("interpreterCreate", &js_interpreter_create);
	emscripten::function("interpreterStep", &js_interpreter_step);
	emscripten::function("interpreterInputVars", &js_interpreter_input_vars);
	emscripten::function("interpreterFree", &js_interpreter_free);
};
