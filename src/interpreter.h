// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

#include <fstream>

#include "boolean_algebras/tau_ba.h"
#include "boolean_algebras/variant_ba.h"
#include "solver.h"
#include "satisfiability.h"
#include "utility/term.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

namespace idni::tau_lang {

using type = std::string;

using filename = std::string;

template<typename...BAs>
using io_var_name = tau<BAs...>; // out_var_name or in_var_name

template<typename...BAs>
using var_desc = std::pair<io_var_name<BAs...>, type>;

// represents an solution of variables to values.
template<typename...BAs>
using assignment = std::map<tau<BAs...>, tau<BAs...>>;

// A system represent a clause to be solved. It maps the different
// equations of the clause according to its type.
template<typename...BAs>
using system = std::map<type, tau<BAs...>>;

template<typename...BAs>
struct finputs {

	finputs() = delete;

	finputs(std::map<tau<BAs...>, std::pair<type, filename>> inputs) {
		// open the corresponding streams for input and store them in streams
		for (const auto& [var, desc]: inputs) {
			this->types[var] = desc.first;
			this->streams[var] = desc.second.empty()
				? std::optional<std::ifstream>()
				: std::ifstream(desc.second);
			if (this->streams[var]
				&& !this->streams[var].value().is_open())
					BOOST_LOG_TRIVIAL(error)
						<< "(Error) failed to open input file '"
						<< desc.second << "': ";
		}
	}

	finputs (finputs&& other) noexcept {
		types = std::move(other.types);
		streams = std::move(other.streams);
		time_point = other.time_point;
	}

	~finputs() {
		// close the streams
		for (auto& [_, file]: streams)
			if (file) file.value().close();
	}

	void add_input (const tau<BAs...>& var, const type& t, const filename& f) {
		if (!types.contains(var)) {
			types.emplace(var, t);
			streams.emplace(var,
				f.empty()? std::optional<std::ifstream>()
						: std::ifstream(f));
			if (this->streams[var]
				&& !this->streams[var].value().is_open())
					BOOST_LOG_TRIVIAL(error)
						<< "(Error) failed to open input file '"
						<< f << "': ";
		}
	}

	std::optional<assignment<BAs...>> read() {
		// for each stream in in streams, read the value from the file/stdin,
		// parsed it and store it in out.
		if (streams.empty()) return assignment<BAs...>();
		assignment<BAs...> current;
		for (auto& [var, file]: streams) {
			std::string line;
			if (file) {
				std::getline(file.value(), line);
				std::cout << line << "\n";
			} else {
				std::cout << var << "[" << time_point << "] := ";
				term::enable_getline_mode();
				std::getline(std::cin, line);
				term::disable_getline_mode();
				// TODo (MEDIUM) add echo for input from a file instead of console
			}
			if (line.empty()) return {}; // no more inputs
			auto cnst = nso_factory<BAs...>::instance().parse(line, types[var]);
			if (!cnst) {
				BOOST_LOG_TRIVIAL(error)
					<< "(Error) failed to parse input value '"
					<< line << "' for stream '"
					<< var << "'\n";
				return {};
			}
			current[var] = build_bf_constant(cnst.value(), types[var]);
		}
		time_point += 1;
		return current;
	}

	// Read input from command line and return mapping from in_vars to this input
	std::pair<std::optional<assignment<BAs...> >, bool> read(
		std::vector<tau<BAs...> >& in_vars, size_t time_step) {
		std::ranges::sort(in_vars, constant_io_comp);
		assignment<BAs...> value;
		for (const auto& var : in_vars) {
			// Skip output stream variables
			if (var | tau_parser::io_var | tau_parser::out)
				continue;
			// Skip input stream variables with time point greater time_step
			if (get_io_time_point(var) > (int_t)time_step)
				continue;
			assert(is_non_terminal(tau_parser::variable, var));
			std::string line;
			auto var_name = trim(trim2(var));
			if (auto it = streams.find(var_name); it != streams.end() && it->second.has_value()) {
				std::getline(it->second.value(), line);
				std::cout << line << "\n";
			} else if (it == streams.end()) {
				BOOST_LOG_TRIVIAL(error)
					<< "(Error) failed to find input stream for stream '" << var_name << "'\n";
				return {};
			} else {
				std::cout << var << " := ";
				term::enable_getline_mode();
				std::getline(std::cin, line);
				term::disable_getline_mode();
			}
			if (line.empty()) return {value, true}; // no more inputs
			const auto it = types.find(var_name);
			if (it == types.end()) {
				BOOST_LOG_TRIVIAL(error)
					<< "(Error) failed to find type for " << var_name;
				return {};
			}
			auto cnst = nso_factory<BAs...>::instance().parse(line, it->second);
			if (!cnst) {
				BOOST_LOG_TRIVIAL(error)
					<< "(Error) failed to parse input value '"
					<< line << "' for stream '"
					<< var << "'\n";
				return {};
			}
//			print_tau_tree(std::cout, cnst.value());
			if (is_non_terminal<tau_parser::bitvector, BAs...>(cnst.value())) {
				// If the constant is a bitvector, we need to wrap it in a bv
				// with the type of the variable.
				value[wrap(tau_parser::bv, var)] = wrap(tau_parser::bv, cnst.value());
				continue;
			}
			const auto wrapped_const = build_bf_constant(cnst.value(), it->second);
			// Check that the input is a closed formula
			if (has_open_tau_fm_in_constant(wrapped_const))
				return {};
			value[wrap(tau_parser::bf, var)] = wrapped_const;
		}
		return {value, false};
	}

	std::optional<type> type_of(const tau<BAs...>& var) const {
		if (auto type = types.find(var); type != types.end())
			return type->second;

		auto name = tau_to_str(var);
		if (name.size() > 1 && name[0] == '_' && name[1] == 'e') {
			return { "sbf" };
		}

		// BOOST_LOG_TRIVIAL(error)
		// 	<< "(Error) failed to find type for stream: "
		// 	<< var << "\n";
		return {};
	}

	std::map<tau<BAs...>, type> types;
	std::map<tau<BAs...>, std::optional<std::ifstream>> streams;
	size_t time_point = 0;
};

template<typename...BAs>
struct foutputs {

	foutputs() = delete;

	foutputs(std::map<tau<BAs...>, std::pair<type, filename>> outputs) {
		// open the corresponding streams for input and store them in streams
		for (const auto& [var, desc]: outputs) {
			this->types[var] = desc.first;
			this->streams[var] = desc.second.empty()
				? std::optional<std::ofstream>()
				: std::ofstream(desc.second);
		}
	}

	foutputs (foutputs&& other) noexcept {
		types = std::move(other.types);
		streams = std::move(other.streams);
	}

	~foutputs() {
		// close the streams
		for (auto& [_, file]: streams)
			if (file) file.value().close();
	}

	void add_output (const tau<BAs...>& var, const type& t, const filename& f) {
		if (!types.contains(var)) {
			types.emplace(var, t);
			streams.emplace(var,
				f.empty()? std::optional<std::ofstream>()
						: std::ofstream(f));
		}
	}

	bool write(const assignment<BAs...>& outputs) {
		// Sort variables in output by time
		std::vector<tau<BAs...>> io_vars;
		for (const auto& [var, _ ] : outputs) {
			assert(is_child_non_terminal(tau_parser::io_var, trim(var)));
			io_vars.push_back(var);
		}
		std::ranges::sort(io_vars, constant_io_comp);

		// for each stream in out.streams, write the value from the solution
		for (const auto& io_var : io_vars) {
			// get the BA element associated with io_var_name
			auto value = outputs.find(io_var)->second
				| tau_parser::bf_constant
				| tau_parser::constant
				| only_child_extractor<BAs...>
				| ba_extractor<BAs...>;
				auto io_var_name = trim2(trim2(io_var));
			std::stringstream ss;
			if (!value && !is_non_terminal(tau_parser::bv, io_var)) {
				// is a bv
				if (auto check = outputs.find(io_var)->second
						| tau_parser::bf_t; check) {
					auto type = types.find(io_var_name)->second;
					ss << nso_factory<BAs...>::instance().one(type);
				// is bf_f
				} else if (auto check = outputs.find(io_var)->second
						| tau_parser::bf_f; check) {
					auto type = types.find(io_var_name)->second;
					ss << nso_factory<BAs...>::instance().zero(type);
				// is something else but not a BA element
				} else {
					BOOST_LOG_TRIVIAL(error)
						<< "(Error): no Boolean algebra element assigned to output '"
						<< io_var << "'\n";
					return false;
				}
			} else if (is_non_terminal(tau_parser::bv, io_var)) {
				ss << outputs.find(io_var)->second;
			} else {
				ss << value.value();
			}
			// get the out_var_name tag
			if (auto stream = streams.find(io_var_name); stream != streams.end())
				if (stream->second) stream->second.value() << ss.str() << "\n";
				else std::cout << io_var << " := " << ss.str() << "\n";
			else {
				std::stringstream ss; ss << io_var;
				if (auto name = ss.str(); !name.empty() && name.front() == '_') continue;

				BOOST_LOG_TRIVIAL(error)
					<< "(Error) failed to find output stream for stream '"
					<< io_var << "'\n";
				return false;
			}
		}
		return true; // success
	}

	std::optional<type> type_of(const tau<BAs...>& var) const {
		if (auto type = types.find(var); type != types.end())
			return type->second;

		auto name = tau_to_str(var);
		if (name.size() > 1 && name[0] == '_' && name[1] == 'e') {
			return { "sbf" };
		}

		// BOOST_LOG_TRIVIAL(error)
		// 	<< "(Error) failed to find type for stream '"
		// 	<< var << "'\n";
		return {};
	}

	std::map<tau<BAs...>, type> types;
	std::map<tau<BAs...>, std::optional<std::ofstream>> streams;
};

template<typename input_t, typename output_t, typename...BAs>
struct interpreter {
	interpreter(const tau<BAs...>& ubt_ctn, const tau<BAs...>& original_spec,
		assignment<BAs...>& memory, input_t& input, output_t& output) :
						ubt_ctn(ubt_ctn),
						original_spec(original_spec),
						memory(memory),
						inputs(std::move(input)),
						outputs(std::move(output)) {
		compute_lookback_and_initial();
		// Put all streams in ubt_ctn into outputs/inputs
		compute_systems(this->ubt_ctn, inputs, outputs);
	}

	static std::optional<interpreter> make_interpreter(
		const tau<BAs...>& spec, auto& inputs, auto& outputs);

	std::pair<std::optional<assignment<BAs...>>, bool> step();

	// Update the interpreter with a given update
	void update(const tau<BAs...>& update);

	tau<BAs...> ubt_ctn;
	tau<BAs...> original_spec;
	assignment<BAs...> memory;
	size_t time_point = 0;
	input_t inputs;
	output_t outputs;

private:
	// store all the possible systems to be solved, each system corresponds to a
	// different clause.
	std::vector<system<BAs...>> systems;
	bool final_system = false;
	size_t formula_time_point = 0;
	int_t highest_initial_pos = 0;
	int_t lookback = 0;

	tau<BAs...> get_ubt_ctn_at (int_t t);

	bool calculate_initial_systems ();

	std::vector<tau<BAs...>> build_inputs_for_step (const size_t t);

	// Return typed systems of equations for the solver corresponding to each clause
	// in the unbound continuation
	static std::vector<system<BAs...>> compute_systems(const tau<BAs...>& ubd_ctn,
		auto& inputs, auto& outputs);

	// Get the type for a clause of a local specification
	static std::optional<system<BAs...>> compute_atomic_fm_types(const tau<BAs...>& clause,
		auto& inputs, auto& outputs);

	// Record type of io_vars in inputs and outputs
	static void type_io_vars(const auto& io_vars, const std::string& type, auto& inputs, auto& outputs);

	// Compute the type of the equation f = 0 or f != 0 stored in fm for the solver
	static std::optional<type> get_type_atomic_fm(const tau<BAs...>& fm,
		auto& inputs, auto& outputs);

	tau<BAs...> update_to_time_point(const tau<BAs...>& f, const int_t t);

	bool is_memory_access_valid(const auto& io_vars);

	// If a variable is assigned a variable V in a solution from the solver,
	// try to find a non-variable value by checking the solution for V
	void resolve_solution_dependencies(solution<BAs...>& s);

	// Return the lookback and highest initial position of the given unbound continuation
	void compute_lookback_and_initial ();

	// Find an executable specification from DNF
	static std::pair<tau<BAs...>, tau<BAs...>>
	get_executable_spec(const tau<BAs...>& fm, const size_t start_time = 0);

	// Pointwise revision algorithm for producing updated specification
	// Both spec and update need to be normalized
	tau<BAs...> pointwise_revision(tau<BAs...> spec, tau<BAs...> update,
					const int_t start_time);

	// The update stream u contained in this solution for spec is guaranteed maximal
	std::optional<assignment<BAs...>>
	solution_with_max_update(const tau<BAs...>& spec);

	// Returns if the variable is excluded from output
	static bool is_excluded_output(const tau<BAs...>& var);

	// Return those variables that appear within the lookback
	std::vector<tau<BAs...> > appear_within_lookback(
		const std::vector<tau<BAs...> >& vars);

};

template<typename... BAs>
std::optional<tau<BAs...>> unpack_tau_constant(const tau<BAs...>& constant) {
	using p = tau_parser;
	auto c_variant = constant
		| p::bf_constant
		| p::constant
		| only_child_extractor<BAs...>
		| ba_extractor<BAs...>;
	if (!c_variant)	return {};
	return nso_factory<BAs...>::instance().unpack_tau_ba(c_variant.value());
}

template<typename input_t, typename output_t, typename...BAs>
std::optional<interpreter<input_t, output_t, BAs...>>
run(const tau<BAs...>& form, input_t& inputs, output_t& outputs, const size_t steps = 0) {

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace) << "interpreter.h:" << __LINE__ << " run/form: " << form;
	#endif // DEBUG

	auto spec = normalizer(form);

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(trace) << "interpreter.h:" << __LINE__ << " run/spec: " << spec;
	#endif // DEBUG

	auto intrprtr_o = interpreter<input_t, output_t, BAs...>
		::make_interpreter(spec, inputs, outputs);
	if (!intrprtr_o) return {};
	auto& intrprtr = intrprtr_o.value();

	BOOST_LOG_TRIVIAL(info) << "-----------------------------------------------------------------------------------------------------------";
	BOOST_LOG_TRIVIAL(info) << "Please provide requested input, or press ENTER to terminate                                               |";
	BOOST_LOG_TRIVIAL(info) << "If no input is requested, press ENTER to continue to the next execution step, or type q(uit) to terminate |";
	BOOST_LOG_TRIVIAL(info) << "-----------------------------------------------------------------------------------------------------------\n\n";

	// Continuously perform execution step until user quits
	while (true) {
		auto [output, auto_continue] = intrprtr.step();
		// If the user provided empty input for an input stream, quit
		if (!output.has_value()) break;
		if (!intrprtr.outputs.write(output.value())) break;
		// If there is no input, ask the user if execution should continue
		if (!auto_continue && steps == 0) {
			std::string line;
			term::enable_getline_mode();
			std::getline(std::cin, line);
			term::disable_getline_mode();
			if (line == "q" || line == "quit")
				break;
		} else std::cout << "\n";

		// Update interpreter in case the output stream u is present and unequal to 0
		auto update_stream = build_out_variable_at_n<BAs...>(
			"u", intrprtr.time_point - 1);
		// Update only if u is of type tau
		if (auto t = intrprtr.outputs.type_of(get_tau_io_name(trim(update_stream)));
				t.has_value() && t.value() == "tau") {
			auto it = output.value().find(update_stream);
			if (it != output.value().end() && it->second != _0<BAs...>) {
				auto update = unpack_tau_constant(it->second);
				if (update) {
					BOOST_LOG_TRIVIAL(trace) << "update: " << update.value() << "\n";
					intrprtr.update(update.value());
				}
			}
		}
		if (steps != 0 && intrprtr.time_point == steps) break;
	}
	return intrprtr_o;
}

} // namespace idni::tau_lang

#include "interpreter.impl.h"

# endif //__INTERPRETER_H__