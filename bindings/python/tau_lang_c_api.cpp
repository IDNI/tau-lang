// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// C API implementation for Python/ctypes bindings.
// Provides realizability checking and Mealy machine synthesis/execution.

#define bas_pack tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb>, qint, qlt, nlang_ba, bv, sbf_ba, hsb

#include "boolean_algebras/qint.h"
#include "boolean_algebras/qlt.h"
#include "boolean_algebras/nlang_ba.h"
#include "boolean_algebras/hsb.h"
#include "tau.h"
#include "parse_error_hint.h"
#include "api.h"

#include "tau_lang_c_api.h"

#include <boost/log/core.hpp>
#include <mutex>
#include <string>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <sstream>
#include <vector>

namespace {
using namespace idni::tau_lang;
using node_t = node<bas_pack>;
using tau    = tree<node_t>;
using tau_api = api<node_t>;

std::mutex g_mtx;
std::string g_last_error;
std::string g_step_result;
std::string g_output_vars_result;
std::string g_input_vars_result;

bool g_logging_disabled = false;
void disable_logging_once() {
	if (g_logging_disabled) return;
	boost::log::core::get()->set_logging_enabled(false);
	g_logging_disabled = true;
}

// Handle counter for synthesized interpreters
std::atomic<int64_t> g_next_handle{1};
std::unordered_map<int64_t, std::unique_ptr<interpreter<node_t>>> g_interpreters;

// ---------------------------------------------------------------------------
// Minimal JSON helpers (no external library dependency)
// ---------------------------------------------------------------------------

void skip_ws(const char*& p) {
	while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') ++p;
}

// Parse a JSON-quoted string. p must point at the opening '"'.
// Advances p past the closing '"'. Returns the unescaped content.
bool parse_json_string(const char*& p, std::string& out) {
	if (*p != '"') return false;
	++p;
	out.clear();
	while (*p && *p != '"') {
		if (*p == '\\') {
			++p;
			if (!*p) return false;
			switch (*p) {
				case '"':  out += '"';  break;
				case '\\': out += '\\'; break;
				case '/':  out += '/';  break;
				case 'n':  out += '\n'; break;
				case 't':  out += '\t'; break;
				case 'r':  out += '\r'; break;
				default:   out += *p;   break;
			}
		} else {
			out += *p;
		}
		++p;
	}
	if (*p != '"') return false;
	++p;
	return true;
}

// Parse a JSON object {"key": "value", ...} into a map.
bool parse_json_object(const char* json,
	std::map<std::string, std::string>& out)
{
	out.clear();
	if (!json) return true; // NULL treated as empty
	const char* p = json;
	skip_ws(p);
	if (*p != '{') return false;
	++p;
	skip_ws(p);
	if (*p == '}') return true; // empty object
	while (true) {
		skip_ws(p);
		std::string key, value;
		if (!parse_json_string(p, key)) return false;
		skip_ws(p);
		if (*p != ':') return false;
		++p;
		skip_ws(p);
		if (!parse_json_string(p, value)) return false;
		out[key] = value;
		skip_ws(p);
		if (*p == '}') break;
		if (*p != ',') return false;
		++p;
	}
	return true;
}

// Escape a string for JSON output.
std::string json_escape(const std::string& s) {
	std::string out;
	out.reserve(s.size() + 4);
	for (char c : s) {
		switch (c) {
			case '"':  out += "\\\""; break;
			case '\\': out += "\\\\"; break;
			case '\n': out += "\\n";  break;
			case '\r': out += "\\r";  break;
			case '\t': out += "\\t";  break;
			default:   out += c;      break;
		}
	}
	return out;
}

// Build a JSON object string from a stream_at→string map + state.
std::string build_step_json(
	const std::map<stream_at, std::string>& outputs, int64_t state)
{
	std::ostringstream ss;
	ss << "{";
	bool first = true;
	for (const auto& [sa, val] : outputs) {
		if (!first) ss << ", ";
		ss << "\"" << json_escape(sa.name) << "\": \""
		   << json_escape(val) << "\"";
		first = false;
	}
	if (!first) ss << ", ";
	ss << "\"state\": " << state;
	ss << "}";
	return ss.str();
}

// Build a JSON array of strings.
std::string build_json_array(const std::vector<std::string>& items) {
	std::ostringstream ss;
	ss << "[";
	for (size_t i = 0; i < items.size(); ++i) {
		if (i > 0) ss << ", ";
		ss << "\"" << json_escape(items[i]) << "\"";
	}
	ss << "]";
	return ss.str();
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// Realizability (existing)
// ---------------------------------------------------------------------------

extern "C" int tau_lang_decide(const char* formula) {
	if (!formula) {
		std::lock_guard<std::mutex> lg(g_mtx);
		g_last_error = "null formula pointer";
		return TAU_LTL_INTERNAL_ERR;
	}
	std::lock_guard<std::mutex> lg(g_mtx);
	disable_logging_once();
	g_last_error.clear();

	try {
		auto nso = get_nso_rr<node_t>(tau::get(formula));
		if (!nso.has_value()) {
			g_last_error = "parse error";
			auto hint = classify_parse_error(formula);
			if (!hint.empty()) g_last_error += ": " + hint;
			return TAU_LTL_PARSE_ERROR;
		}
		tref fm = nso.value().main->get();
		if (!fm) {
			g_last_error = "empty formula";
			return TAU_LTL_PARSE_ERROR;
		}
		bool r = is_tau_formula_sat<node_t>(fm);
		return r ? TAU_LTL_REALIZABLE : TAU_LTL_UNREALIZABLE;
	} catch (const std::exception& e) {
		g_last_error = std::string("internal: ") + e.what();
		return TAU_LTL_INTERNAL_ERR;
	} catch (...) {
		g_last_error = "internal: unknown exception";
		return TAU_LTL_INTERNAL_ERR;
	}
}

extern "C" const char* tau_lang_last_error(void) {
	std::lock_guard<std::mutex> lg(g_mtx);
	return g_last_error.c_str();
}

// ---------------------------------------------------------------------------
// Mealy machine synthesis and execution
// ---------------------------------------------------------------------------

extern "C" int64_t tau_lang_synthesize(const char* formula) {
	if (!formula) {
		std::lock_guard<std::mutex> lg(g_mtx);
		g_last_error = "null formula pointer";
		return TAU_LTL_SYNTH_INTERNAL_ERR;
	}
	std::lock_guard<std::mutex> lg(g_mtx);
	disable_logging_once();
	g_last_error.clear();

	try {
		// 1. Parse and check realizability
		auto nso = get_nso_rr<node_t>(tau::get(formula));
		if (!nso.has_value()) {
			g_last_error = "parse error";
			auto hint = classify_parse_error(formula);
			if (!hint.empty()) g_last_error += ": " + hint;
			return TAU_LTL_SYNTH_PARSE_ERROR;
		}
		tref fm = nso.value().main->get();
		if (!fm) {
			g_last_error = "empty formula";
			return TAU_LTL_SYNTH_PARSE_ERROR;
		}
		bool realizable = is_tau_formula_sat<node_t>(fm);
		if (!realizable) {
			g_last_error = "formula is unrealizable";
			return TAU_LTL_SYNTH_UNREALIZABLE;
		}

		// 2. Create an interpreter (Mealy machine) via the high-level API
		auto interp_opt = tau_api::get_interpreter(std::string(formula));
		if (!interp_opt.has_value()) {
			g_last_error = "failed to create interpreter";
			return TAU_LTL_SYNTH_INTERNAL_ERR;
		}

		// 3. Store in the global map
		int64_t handle = g_next_handle.fetch_add(1);
		g_interpreters[handle] = std::make_unique<interpreter<node_t>>(
			std::move(interp_opt.value()));
		return handle;
	} catch (const std::exception& e) {
		g_last_error = std::string("internal: ") + e.what();
		return TAU_LTL_SYNTH_INTERNAL_ERR;
	} catch (...) {
		g_last_error = "internal: unknown exception";
		return TAU_LTL_SYNTH_INTERNAL_ERR;
	}
}

extern "C" const char* tau_lang_mealy_step(int64_t handle,
	const char* input_json)
{
	std::lock_guard<std::mutex> lg(g_mtx);
	disable_logging_once();
	g_last_error.clear();
	g_step_result.clear();

	try {
		auto it = g_interpreters.find(handle);
		if (it == g_interpreters.end()) {
			g_last_error = "invalid handle";
			return nullptr;
		}
		auto& interp = *it->second;

		// Parse input JSON into stream_at map
		std::map<std::string, std::string> input_map;
		if (input_json && *input_json) {
			if (!parse_json_object(input_json, input_map)) {
				g_last_error = "malformed input JSON";
				return nullptr;
			}
		}

		// Build the inputs map expected by api::step
		std::map<stream_at, std::string> inputs;
		for (const auto& [name, value] : input_map) {
			inputs[{name, interp.time_point}] = value;
		}

		// Execute one step — always use with-inputs overload
		// (the no-args overload reads from stdin via console streams).
		// interactive=false: skip auto_continue check which is only
		// relevant for CLI pause/prompt — C API callers manage their
		// own stepping loop.
		auto result = tau_api::step(interp, std::move(inputs),
			/*interactive=*/false);
		if (!result.has_value()) {
			g_last_error = "step failed or no output";
			return nullptr;
		}

		// Build JSON result including state
		g_step_result = build_step_json(
			result.value(), static_cast<int64_t>(interp.time_point));
		return g_step_result.c_str();
	} catch (const std::exception& e) {
		g_last_error = std::string("internal: ") + e.what();
		return nullptr;
	} catch (...) {
		g_last_error = "internal: unknown exception";
		return nullptr;
	}
}

extern "C" const char* tau_lang_mealy_output_vars(int64_t handle) {
	std::lock_guard<std::mutex> lg(g_mtx);
	disable_logging_once();
	g_last_error.clear();
	g_output_vars_result.clear();

	try {
		auto it = g_interpreters.find(handle);
		if (it == g_interpreters.end()) {
			g_last_error = "invalid handle";
			return nullptr;
		}
		auto& interp = *it->second;

		std::vector<std::string> output_names;
		for (const auto& [var, _] : interp.ctx.outputs) {
			std::string name = get_var_name<node_t>(var);
			if (!name.empty() && name.front() != '_')
				output_names.push_back(name);
		}

		g_output_vars_result = build_json_array(output_names);
		return g_output_vars_result.c_str();
	} catch (const std::exception& e) {
		g_last_error = std::string("internal: ") + e.what();
		return nullptr;
	} catch (...) {
		g_last_error = "internal: unknown exception";
		return nullptr;
	}
}

extern "C" const char* tau_lang_mealy_input_vars(int64_t handle) {
	std::lock_guard<std::mutex> lg(g_mtx);
	disable_logging_once();
	g_last_error.clear();
	g_input_vars_result.clear();

	try {
		auto it = g_interpreters.find(handle);
		if (it == g_interpreters.end()) {
			g_last_error = "invalid handle";
			return nullptr;
		}
		auto& interp = *it->second;

		auto needed = tau_api::get_inputs_for_step(interp);
		std::vector<std::string> input_names;
		for (const auto& sa : needed) {
			input_names.push_back(sa.name);
		}

		g_input_vars_result = build_json_array(input_names);
		return g_input_vars_result.c_str();
	} catch (const std::exception& e) {
		g_last_error = std::string("internal: ") + e.what();
		return nullptr;
	} catch (...) {
		g_last_error = "internal: unknown exception";
		return nullptr;
	}
}

extern "C" int64_t tau_lang_mealy_state(int64_t handle) {
	std::lock_guard<std::mutex> lg(g_mtx);
	g_last_error.clear();

	auto it = g_interpreters.find(handle);
	if (it == g_interpreters.end()) {
		g_last_error = "invalid handle";
		return -1;
	}
	return static_cast<int64_t>(it->second->time_point);
}

extern "C" void tau_lang_mealy_free(int64_t handle) {
	std::lock_guard<std::mutex> lg(g_mtx);
	g_interpreters.erase(handle);
}
