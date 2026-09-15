// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

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

namespace nb = nanobind;
using namespace nb::literals;

using bv = idni::tau_lang::bv;
using sbf_ba = idni::tau_lang::sbf_ba;
template <typename... BAs>
using tau_ba = idni::tau_lang::tau_ba<BAs...>;

#define bas_pack tau_ba<bv, sbf_ba>, bv, sbf_ba
using node_t = idni::tau_lang::node<bas_pack>;

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
	rep.print(idni::diagnostics::sinks{
		.error   = [&](std::string_view l) { out.errors.emplace_back(l); },
		.warning = [&](std::string_view l) { out.warnings.emplace_back(l); },
		.info    = [&](std::string_view l) { out.infos.emplace_back(l); },
	});
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
	nb::set_leak_warnings(leak_warnings());

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

	// Interpreter
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
		"carries the incoming revision rather than the merged result.");

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
		"Create an interpreter from a specification string with options.");

	m.def("get_inputs_for_step",
		[](interpreter_t& i) {
			return tau_api::get_inputs_for_step(i);
		}, "interpreter"_a,
		"Get the inputs needed for the next step.");

	m.def("step",
		[](interpreter_t& i,
			const std::map<stream_at, std::string>& inputs)
		{
			return to_py_result(tau_api::step(i, inputs));
		}, "interpreter"_a, "inputs"_a,
		"Step the interpreter with given inputs.");

	m.def("step",
		[](interpreter_t& i) {
			return to_py_result(tau_api::step(i));
		}, "interpreter"_a,
		"Step the interpreter without inputs (uses remapped streams).");
}
