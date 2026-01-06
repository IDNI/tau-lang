// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__API_H__
#define __IDNI__TAU__API_H__

#include "tau.h"

namespace idni::tau_lang {

using stream_at = std::pair<std::string, size_t>;

struct interpreter_options {
	input_streams_remap  input_remaps;
	output_streams_remap output_remaps;
};

template <NodeType node>
struct api {
	static std::optional<interpreter<node>> get_interpreter(
		const std::string& specification);
	static std::optional<interpreter<node>> get_interpreter(
		const std::string& specification,
		interpreter_options& options);
	// In order to get the inputs needed for the next step
	static std::vector<stream_at> get_inputs_for_step(interpreter<node>& i);
	// Returned map is mapping of output streams to assigned values
	static std::optional<std::map<stream_at, std::string>> step(interpreter<node>& i, std::map<stream_at, std::string> inputs);
	// Returned map is mapping of output streams to assigned values
	static std::optional<std::map<stream_at, std::string>> step(interpreter<node>& i);
	// // Given the assigned values to each input stream, it needs to be checked that the values are valid
	// static bool validate_inputs(interpreter<node>& i, std::map<std::string, std::string> inputs);

	// parsing
	static htref parse_specification(const std::string& specification,
		io_context<node>& ctx = {});
	// static htref parse_term(const std::string& term);
	// static htref parse_formula(const std::string& predicate);
	// static htref parse_function_def(const std::string& function_def);
	// static htref parse_predicate_def(const std::string& predicate_def);
	// static htref parse_stream_def(const std::string& stream_def);
};


} // namespace idni::tau_lang

#include "api.tmpl.h"

#endif // __IDNI__TAU__API_H__