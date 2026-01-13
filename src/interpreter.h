// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__INTERPRETER_H__
#define __IDNI__TAU__INTERPRETER_H__

#include "solver.h"

namespace idni::tau_lang {

// represents a solution  of variables to values.
template <NodeType node>
using assignment = subtree_map<node, tref>;

// A system represent a clause to be solved. It maps the different
// equations of the clause according to its type.
using system = std::map<size_t, tref>;

// pair of BA type id and filename string id
using typed_stream = std::pair<size_t, size_t>;

// map of variable string id to a typed_stream
using typed_io_vars = std::map<size_t, typed_stream>;

template <NodeType node>
typed_stream get_typed_stream(const std::string& type,
				const std::string& filename);

template <NodeType node>
struct finputs {
	using tau = tree<node>;
	using tt = tau::traverser;
	using bac = ba_constants<node>;

	finputs() = delete;
	finputs(const typed_io_vars& inputs);
	finputs(finputs&& other) noexcept;
	~finputs();

	void add_input(tref var, size_t type_sid, size_t filename_sid);
	std::optional<assignment<node>> read();
	// Read input from command line and return mapping from in_vars to this input
	std::pair<std::optional<assignment<node>>, bool> read(
					trefs& in_vars, size_t time_step);

	size_t type_of(tref var) const;

	void rebuild (const typed_io_vars& inputs);

	// map of var name node to a type id
	subtree_map<node, size_t> types;
	// map of var name node to a stream
	subtree_map<node, std::optional<std::ifstream>> streams;

	size_t time_point = 0;
};

template <NodeType node>
struct foutputs {
	using tau = tree<node>;
	using tt = tau::traverser;
	using bac = ba_constants<node>;

	foutputs() = delete;
	foutputs(const typed_io_vars& outputs);
	foutputs(foutputs&& other) noexcept;
	~foutputs();

	void add_output(tref var, size_t type_sid, size_t filename_sid);
	bool write(const assignment<node>& outputs);

	size_t type_of(tref var) const;

	void rebuild (const typed_io_vars& outputs);

	// map of var name node to a type id
	subtree_map<node, size_t> types;
	// map of var name node to a stream
	subtree_map<node, std::optional<std::ofstream>> streams;
};

template <NodeType node, typename in_t, typename out_t>
struct interpreter {
	using tau = tree<node>;
	using tt = tau::traverser;

	interpreter(trefs& ubt_ctn, auto& original_spec, auto& output_partition,
		assignment<node>& memory, in_t& input, out_t& output,
		const spec_context<node>& ctx);

	static std::optional<interpreter> make_interpreter(tref spec,
						auto& inputs, auto& outputs,
						const auto& ctx);

	std::pair<std::optional<assignment<node>>, bool> step();

	// Update the interpreter with a given update
	void update(tref update);

	trefs ubt_ctn;
	// Partition of spec each with representative for set of output streams
	std::vector<std::pair<tref, tref>> original_spec;
	assignment<node> memory;
	size_t time_point = 0;
	in_t inputs;
	out_t outputs;
	const spec_context<node>& ctx = {};

private:
	static bool stream_comp(tref s1, tref s2) {
		return tau::subtree_less(s1, s2);
	};
	union_find_with_sets<decltype(stream_comp), node> output_partition;
	trefs step_spec;
	bool final_system = false;
	size_t formula_time_point = 0;
	int_t highest_initial_pos = 0;
	int_t lookback = 0;

	static std::vector<std::pair<tref, tref>>
	create_spec_partition(tref spec, auto& output_partition);

	trefs get_ubt_ctn_at(int_t t);

	bool calculate_initial_spec();

	std::pair<trefs, bool> build_inputs_for_step(const size_t t);

	tref update_to_time_point(tref f, const int_t t);

	bool is_memory_access_valid(const auto& io_vars);

	// Return the lookback and highest initial position of the given unbound continuation
	void compute_lookback_and_initial();

	// Find an executable specification from DNF
	static tref get_executable_spec(tref& clause, const size_t start_time = 0);

	// Pointwise revision algorithm for producing updated specification
	// Both spec and update need to be normalized
	tref pointwise_revision(tref spec, tref update, const int_t start_time);

	// The update stream u contained in this solution for spec is guaranteed maximal
	std::optional<assignment<node>> solution_with_max_update(tref spec);

	// Returns if the variable is excluded from output
	static bool is_excluded_output(tref var);

	// Return those variables that appear within the lookback
	trefs appear_within_lookback(const trefs& vars);

	// Utility to unsqueeze always statements without timepoint adjustment
	static tref unsqueeze_always(tref cnf_expression);
};

template <NodeType node>
tref unpack_tau_constant(tref constant);

template <NodeType node, typename in_t, typename out_t>
std::optional<interpreter<node, in_t, out_t>> run(tref form,
		in_t& inputs, out_t& outputs, const auto& ctx,
		const size_t steps = 0);

template <NodeType node>
bool collect_input_streams(tref dnf, typed_io_vars& current_inputs,
	const spec_context<node>& ctx);

template <NodeType node>
typed_io_vars collect_input_streams(tref dnf, const spec_context<node>& ctx);

template <NodeType node>
bool collect_output_streams(tref dnf, typed_io_vars& current_inputs,
	const spec_context<node>& ctx);

template <NodeType node>
typed_io_vars collect_output_streams(tref dnf, const spec_context<node>& ctx);

} // namespace idni::tau_lang

#include "interpreter.impl.h"

# endif //__IDNI__TAU__INTERPRETER_H__