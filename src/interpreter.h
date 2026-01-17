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

template <NodeType node>
struct api;

template <NodeType node>
struct interpreter {
	using tau = tree<node>;
	using tt = tau::traverser;
	friend struct api<node>;

	interpreter(trefs& ubt_ctn, auto& original_spec, auto& output_partition,
		assignment<node>& memory, const io_context<node>& ctx);

	static std::optional<interpreter> make_interpreter(tref spec,
		const io_context<node>& ctx);

	std::pair<std::optional<assignment<node>>, bool> step();
	std::pair<std::optional<assignment<node>>, bool> step(
						const assignment<node>& values);

	// Update the interpreter with a given update
	void update(tref update);

	trefs ubt_ctn;
	// Partition of spec each with representative for set of output streams
	std::vector<std::pair<tref, tref>> original_spec;
	assignment<node> memory;
	size_t time_point = 0;
	input_streams<node>     inputs;
	output_streams<node>    outputs;
	io_context<node> ctx;

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

	std::pair<std::optional<assignment<node>>, bool> read(
		const trefs& in_vars, size_t time_step);
	bool write(const assignment<node>& outputs);
	bool serialize_constant(std::stringstream& ss, tref constant,
		size_t type) const;
	void rebuild_inputs(const subtree_map<node, size_t>& current_inputs);
	void rebuild_outputs(const subtree_map<node, size_t>& current_outputs);

	bool collect_input_streams(tref dnf, subtree_map<node, size_t>& current_inputs);
	subtree_map<node, size_t> collect_input_streams(tref dnf);
	bool collect_output_streams(tref dnf, subtree_map<node, size_t>& current_outputs);
	subtree_map<node, size_t> collect_output_streams(tref dnf);

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

	std::ostream& dump(std::ostream& os) const;
	std::string dump_to_str() const;

	template <NodeType N>
	friend std::optional<interpreter<N>> run(tref,
		const io_context<N>&, const size_t);
};

template <NodeType node>
tref unpack_tau_constant(tref constant);

template <NodeType node>
std::optional<interpreter<node>> run(tref form,
	const io_context<node>& ctx, const size_t steps = 0);

} // namespace idni::tau_lang

#include "interpreter.impl.h"

# endif //__IDNI__TAU__INTERPRETER_H__