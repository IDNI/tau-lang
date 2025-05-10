// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__INTERPRETER_H__
#define __IDNI__TAU__INTERPRETER_H__

#include "solver.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "interpreter"

namespace idni::tau_lang {

// represents a solution  of variables to values.
template <NodeType node>
using assignment = typename tree<node>::subtree_map;

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
	using bac = node::ba_constants;

	finputs() = delete;
	finputs(typed_io_vars inputs);
	finputs(finputs&& other) noexcept;
	~finputs();

	void add_input(tref var, size_t type_sid, size_t filename_sid);
	std::optional<assignment<node>> read();
	// Read input from command line and return mapping from in_vars to this input
	std::pair<std::optional<assignment<node>>, bool> read(
					trefs& in_vars, size_t time_step);

	size_t type_of(tref var) const;

	// map of var name node to a type id
	std::map<tref, size_t> types;
	// map of var name node to a stream
	std::map<tref, std::optional<std::ifstream>,
		typename tau::subtree_equality> streams;

	size_t time_point = 0;
};

template <NodeType node>
struct foutputs {
	using tau = tree<node>;
	using tt = tau::traverser;
	using bac = node::ba_constants;

	foutputs() = delete;
	foutputs(typed_io_vars outputs);
	foutputs(foutputs&& other) noexcept;
	~foutputs();

	void add_output(tref var, size_t type_sid, size_t filename_sid);
	bool write(const assignment<node>& outputs);

	size_t type_of(tref var) const;

	// map of var name node to a type id
	std::map<tref, size_t> types;
	// map of var name node to a stream
	std::map<tref, std::optional<std::ofstream>,
		typename tau::subtree_equality> streams;
};

template <NodeType node, typename in_t, typename out_t>
struct interpreter {
	using tau = tree<node>;
	using tt = tau::traverser;

	interpreter(tref ubt_ctn, tref original_spec,
		assignment<node>& memory, in_t& input, out_t& output);

	static std::optional<interpreter> make_interpreter(tref spec,
						auto& inputs, auto& outputs);

	std::pair<std::optional<assignment<node>>, bool> step();

	// Update the interpreter with a given update
	void update(tref update);

	tref ubt_ctn;
	tref original_spec;
	assignment<node> memory;
	size_t time_point = 0;
	in_t inputs;
	out_t outputs;

private:
	// store all the possible systems to be solved, each system corresponds to a
	// different clause.
	std::vector<system> systems;
	bool final_system = false;
	size_t formula_time_point = 0;
	int_t highest_initial_pos = 0;
	int_t lookback = 0;

	tref get_ubt_ctn_at(int_t t);

	bool calculate_initial_systems();

	trefs build_inputs_for_step(const size_t t);

	// Return typed systems of equations for the solver corresponding to each clause
	// in the unbound continuation
	static std::vector<system> compute_systems(tref ubd_ctn,
		auto& inputs, auto& outputs);

	// Get the type for a clause of a local specification
	static std::optional<system> compute_atomic_fm_types(tref clause,
		auto& inputs, auto& outputs);

	// Record type of io_vars in inputs and outputs
	static void type_io_vars(trefs io_vars, size_t type,
					auto& inputs, auto& outputs);

	// Compute the type of the equation f = 0 or f != 0 stored in fm for the solver
	static size_t get_type_atomic_fm(tref fm, auto& inputs, auto& outputs);

	tref update_to_time_point(tref f, const int_t t);

	bool is_memory_access_valid(const auto& io_vars);

	// If a variable is assigned a variable V in a solution from the solver,
	// try to find a non-variable value by checking the solution for V
	void resolve_solution_dependencies(solution<node>& s);

	// Return the lookback and highest initial position of the given unbound continuation
	void compute_lookback_and_initial();

	// Find an executable specification from DNF
	static std::pair<tref, tref> get_executable_spec(tref fm,
						const size_t start_time = 0);

	// Pointwise revision algorithm for producing updated specification
	// Both spec and update need to be normalized
	tref pointwise_revision(tref spec, tref update, const int_t start_time);

	// The update stream u contained in this solution for spec is guaranteed maximal
	std::optional<assignment<node>> solution_with_max_update(tref spec);

	// Returns if the variable is excluded from output
	static bool is_excluded_output(tref var);

	// Return those variables that appear within the lookback
	trefs appear_within_lookback(const trefs& vars);

};

template <NodeType node>
std::optional<tref> unpack_tau_constant(tref constant);

template <NodeType node, typename in_t, typename out_t>
std::optional<interpreter<node, in_t, out_t>> run(tref form,
		in_t& inputs, out_t& outputs, const size_t steps = 0);

} // namespace idni::tau_lang

#include "interpreter.impl.h"

# endif //__IDNI__TAU__INTERPRETER_H__