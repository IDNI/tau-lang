// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

#include <fstream>

#include "boolean_algebras/tau_ba.h"
#include "boolean_algebras/variant_ba.h"
#include "solver.h"
#include "satisfiability.h"
#include "term.h"

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

	~finputs() {
		// close the streams
		for (auto& [_, file]: streams)
			if (file) file.value().close();
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
					<< line << "' for variable '"
					<< var << "'\n";
				return {};
			}
			current[var] = build_bf_constant(cnst.value());
		}
		time_point += 1;
		return current;
	}

	// Read input from command line and return mapping from in_vars to this input
	std::pair<std::optional<assignment<BAs...> >, bool> read(
		std::vector<tau<BAs...> >& in_vars, size_t time_step) const {
		std::ranges::sort(in_vars, constant_io_comp);
		assignment<BAs...> value;
		for (const auto& var : in_vars) {
			// Skip output stream variables
			if (var | tau_parser::io_var | tau_parser::out)
				continue;
			// Skip input stream variables with time point greater time_step
			if (get_io_time_point(var) > time_step)
				continue;
			assert(is_non_terminal(tau_parser::variable, var));
			std::string line;
			auto var_name = trim(trim2(var));
			if (auto it = streams.find(var_name); it != streams.end() && it->second.has_value()) {
				std::getline(it->second.value(), line);
				std::cout << line << "\n";
			} else if (it == streams.end()) {
				BOOST_LOG_TRIVIAL(error)
					<< "(Error) failed to find input stream for variable '" << var_name << "'\n";
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
					<< line << "' for variable '"
					<< var << "'\n";
				return {};
			}
			value[wrap(tau_parser::bf, var)] = build_bf_constant(cnst.value());
		}
		return {value, false};
	}

	std::optional<type> type_of(const tau<BAs...>& var) const {
		if (auto type = types.find(var); type != types.end())
			return type->second;

		std::stringstream ss; ss << var;
		if (auto name = ss.str(); !name.empty() && name.front() == '_') {
			return { "sbf" };
		}

		BOOST_LOG_TRIVIAL(error)
			<< "(Error) failed to find type for variable: "
			<< var << "\n";
		return {};
	}

	std::map<tau<BAs...>, type> types;
	mutable std::map<tau<BAs...>, std::optional<std::ifstream>> streams;
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

	~foutputs() {
		// close the streams
		for (auto& [_, file]: streams)
			if (file) file.value().close();
	}

	bool write(const assignment<BAs...>& outputs) {
		// Sort variables in output by time
		std::vector<tau<BAs...>> io_vars;
		for (const auto& [var, ass] : outputs) {
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
			if (!value) {
				// is bf_t
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
						<< "(Error): no Boolean algebra element assigned to variable '"
						<< io_var << "'\n";
					return false;
				}
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
					<< "(Error) failed to find output stream for variable '"
					<< io_var << "'\n";
				return false;
			}
		}
		return true; // success
	}

	std::optional<type> type_of(const tau<BAs...>& var) const {
		if (auto type = types.find(var); type != types.end())
			return type->second;

		std::stringstream ss; ss << var;
		if (auto name = ss.str(); !name.empty() && name.front() == '_') {
			return { "sbf" };
		}

		BOOST_LOG_TRIVIAL(error)
			<< "(Error) failed to find type for variable '"
			<< var << "'\n";
		return {};
	}

	std::map<tau<BAs...>, type> types;
	std::map<tau<BAs...>, std::optional<std::ofstream>> streams;

private:

	std::vector<assignment<BAs...>> sort(const assignment<BAs...>& sol) {
		auto compute_range = [](const assignment<BAs...>& sol) {
			size_t lower = std::numeric_limits<size_t>::max(), upper = 0;
			for (const auto& [var, value]: sol) {
				if (auto num = var
						| tau_parser::variable
						| tau_parser::io_var
						| tau_parser::out
						| tau_parser::offset
						| tau_parser::num
						| only_child_extractor<BAs...>
						| offset_extractor<BAs...>; num) {
					lower = std::min(lower, num | optional_value_extractor<size_t>);
					upper = std::max(upper, num | optional_value_extractor<size_t>);
				}
			}
			return std::make_pair(lower, upper);
		};

		auto [lower, upper] = compute_range(sol);

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(trace)
			<< "sort/lower: " << lower << "\n"
			<< "sort/upper: " << upper << "\n";
		#endif // DEBUG

		std::vector<assignment<BAs...>> result(upper - lower + 1);

		for (auto& [var, value]: sol) {
			if (auto num = var
					| tau_parser::variable
					| tau_parser::io_var
					| tau_parser::out
					| tau_parser::offset
					| tau_parser::num
					| only_child_extractor<BAs...>
					| offset_extractor<BAs...>; num) {
				auto io_var = var
					| tau_parser::variable
					| tau_parser::io_var
					| tau_parser::out
					| tau_parser::out_var_name
					| optional_value_extractor<tau<BAs...>>;
				result[num.value() - lower][io_var] = value;
			}
		}

		#ifdef DEBUG
		for (size_t i = 0; i < result.size(); ++i) {
			BOOST_LOG_TRIVIAL(trace)
				<< "sort/result/[" << i <<"]: ";
			for (const auto& [k, v]: result[i])
				BOOST_LOG_TRIVIAL(trace)
					<< "\t" << k << " := " << v << " ";
			BOOST_LOG_TRIVIAL(trace) << "\n";
		}
		#endif // DEBUG

		return result;
	}

	std::vector<assignment<BAs...>> complete(const std::vector<assignment<BAs...>>& sols) {
		std::vector<assignment<BAs...>> result(sols.size());
		for (size_t i = 0; i < sols.size(); ++i) {
			assignment<BAs...> nsol = sols[i];
			for (const auto& [var, _]: types)
				if ( !nsol.contains(var) ) nsol[var] = _0<BAs...>;
			result[i] = nsol;
		}

		#ifdef DEBUG
		for (size_t i = 0; i < result.size(); ++i) {
			BOOST_LOG_TRIVIAL(trace)
				<< "complete/result/[" << i <<"]: ";
			for (const auto& [k, v]: result[i])
				BOOST_LOG_TRIVIAL(trace)
					<< "\t" << k << " := " << v << " ";
			BOOST_LOG_TRIVIAL(trace) << "\n";
		}
		#endif // DEBUG

		return result;
	}
};

template<typename input_t, typename output_t, typename...BAs>
struct interpreter {
	interpreter(const tau<BAs...>& ubt_ctn, assignment<BAs...>& memory,
		const auto& input, const auto& output) :
						ubt_ctn(ubt_ctn),
						memory(memory),
						inputs(input),
						outputs(output) {
		compute_lookback_and_initial(ubt_ctn);
		// TODO: Re-enable after inputs and outputs member have been removed
		// collect non-temporary output stream variables
		// for (const auto& o: select_top(ubt_ctn,
		// 	is_non_terminal<tau_parser::out_var_name, BAs...>)) {
		// 	std::stringstream ss; ss << o;
		// 	// Exclude temporary vars
		// 	if (ss.str()[0] != '_') outputs.insert(o);
		// }
	}

	static std::optional<interpreter> make_interpreter(
		tau<BAs...> spec, const auto& inputs, const auto& outputs);

	std::pair<std::optional<assignment<BAs...>>, bool> step();

	// store all the possible systems to be solved, each system corresponds to a
	// different clause.

	tau<BAs...> ubt_ctn;
	assignment<BAs...> memory;
	size_t time_point = 0;
	// TODO: Remove inputs and outputs, once type inference for variables is ready
	// and solver can accept clauses with several types
	const input_t& inputs;
	const output_t& outputs;

private:

	std::vector<system<BAs...>> systems;
	size_t formula_time_point = 0;
	int_t highest_initial_pos = 0;
	int_t lookback = 0;

	bool calculate_initial_systems ();

	// Return typed systems of equations for the solver corresponding to each clause
	// in the unbound continuation
	static std::vector<system<BAs...>> compute_systems(const tau<BAs...>& ubd_ctn,
		const auto& inputs, const auto& outputs);

	// Get the type for a clause of a local specification
	static std::optional<system<BAs...>> compute_atomic_fm_types(const tau<BAs...>& clause,
		const auto& inputs, const auto& outputs);

	// Compute the type of the equation f = 0 or f != 0 stored in fm for the solver
	static std::optional<std::pair<type, tau<BAs...>>> get_type_fm(const tau<BAs...>& fm,
		const auto& inputs, const auto& outputs);

	tau<BAs...> update_to_time_point(const tau<BAs...>& f);

	// If a variable is assigned a variable V in a solution from the solver,
	// try to find a non-variable value by checking the solution for V
	void resolve_solution_dependencies(solution<BAs...>& s);

	// Return the lookback and highest initial position of the given unbound continuation
	void compute_lookback_and_initial (const tau<BAs...>& ubd_ctn);

	// Find an executable specification from DNF
	static tau<BAs...> get_executable_spec(const tau<BAs...>& fm);
};

template<typename input_t, typename output_t, typename...BAs>
void run(const tau<BAs...>& form, input_t& inputs, output_t& outputs) {
	auto spec = normalizer(form);
	auto intrprtr = interpreter<input_t, output_t, BAs...>
		::make_interpreter(spec, inputs, outputs);
	if (!intrprtr) return;

	BOOST_LOG_TRIVIAL(info) << "-----------------------------------------------------------------------------------------------------------";
	BOOST_LOG_TRIVIAL(info) << "Please provide requested input, or press ENTER to terminate                                               |";
	BOOST_LOG_TRIVIAL(info) << "If no input is requested, press ENTER to continue to the next execution step, or type q(uit) to terminate |";
	BOOST_LOG_TRIVIAL(info) << "-----------------------------------------------------------------------------------------------------------\n\n";

	// Continuously perform execution step until user quits
	while (true) {
		auto [output, auto_continue] = intrprtr.value().step();
		// If the user provided empty input for an input stream, quit
		if (!output.has_value()) return;
		if (!outputs.write(output.value())) return;
		// If there is no input, ask the user if execution should continue
		if (!auto_continue) {
			std::string line;
			term::enable_getline_mode();
			std::getline(std::cin, line);
			term::disable_getline_mode();
			if (line == "q" || line == "quit")
				return;
		} else std::cout << "\n";
	}
}

} // namespace idni::tau_lang

#include "interpreter.impl.h"

# endif //__INTERPRETER_H__