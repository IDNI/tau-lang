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
		.def_ro("time_point", &interpreter_t::time_point);

	// API functions
	m.def("get_interpreter",
		[](const std::string& spec) {
			return tau_api::get_interpreter(spec);
		}, "specification"_a,
		"Create an interpreter from a specification string.");

	m.def("get_interpreter",
		[](const std::string& spec, interpreter_options& opts) {
			return tau_api::get_interpreter(spec, opts);
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
			return tau_api::step(i, inputs);
		}, "interpreter"_a, "inputs"_a,
		"Step the interpreter with given inputs.");

	m.def("step",
		[](interpreter_t& i) {
			return tau_api::step(i);
		}, "interpreter"_a,
		"Step the interpreter without inputs (uses remapped streams).");
}
