// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <iostream>
#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>

#include "doctest.h"
#include "runtime.h"
#include "hooks.h"
#include "interpreter.h"

#define base_bas tau_ba<sbf_ba>, sbf_ba

using namespace boost::log;
using namespace idni::tau_lang;

std::string random_file(const std::string& extension = ".out", const std::string prefix = "/tmp/") {
    // define the characters to use in the random string
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	// length of the random string
    const size_t length = 10;
    // random number generator
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);
    std::ostringstream oss;
	// add default location
	oss << prefix;
    // generate random string
    for (size_t i = 0; i < length; ++i) oss << charset[dist(generator)];
    // append the file extension and return the file name
    oss << extension;
    return oss.str();
}

template<typename...BAs>
struct output_console {

	output_console() = default;
	output_console(const std::string& type) : _type(type) {}

	bool write(const assignment<BAs...>& outputs) const {
		// for each stream in out.streams, write the value from the solution
		for (const auto& [var, value]: outputs)
			std::cout << var << " := " << value << "\n";
		return true; // success (always)
	}

	std::optional<type> type_of(const tau<BAs...>&) const {
		return { _type };
	}

	void add_output (const tau<BAs...>& v, const std::string&, const std::string&) {
		if (!streams.contains(v)) streams.emplace(v, v);
	}

	assignment<BAs...> streams;
	std::string _type = "sbf";
};

template<typename...BAs>
struct input_vector {

	input_vector() = default;
	input_vector(std::vector<assignment<BAs...>>& inputs) : inputs(
		std::move(inputs)) {}
	input_vector(const std::string& type) : _type(type) {}
	input_vector(std::vector<assignment<BAs...>>& inputs,
		const std::string& type) : inputs(std::move(inputs)), _type(type) {}

	std::optional<assignment<BAs...>> get() const {
		if (inputs.empty()) return { assignment<BAs...>{} };
		if (current == inputs.size()) return {};
		return inputs[current];
	}

	std::pair<std::optional<assignment<BAs...> >, bool> read(
		const auto& , const size_t ) {
        if (inputs.empty()) return { assignment<BAs...>{}, false };
		if (current >= inputs.size()) return { assignment<BAs...>{}, false };
		return { inputs[current++], false};
	}

	std::optional<type> type_of(const tau<BAs...>&) const {
		return { _type };
	}

	void add_input (const tau<BAs...>& v, const std::string&, const std::string&) {
		if (!streams.contains(v)) streams.emplace(v,v);
	}

	std::vector<assignment<BAs...>> inputs;
	assignment<BAs...> streams;
	size_t current = 0;
	std::string _type = "sbf";
};

template<typename... BAs>
void build_input(const std::string& name, const std::vector<std::string>& values,
		const std::string& type, auto& assgn) {
	size_t t = 0;
	for (const auto& val : values) {
		auto in_var = build_in_variable_at_n<BAs...>(name, t);
		auto v = nso_factory<BAs...>::instance().parse(val, type);
		auto v_const = build_bf_constant(v.value(), type);

		if (assgn.size() <= t) {
			std::map<tau<BAs...>, tau<BAs...>> a;
			a.emplace(in_var, v_const);
			assgn.emplace_back(std::move(a));
		} else assgn[t].emplace(in_var, v_const);
		++t;
	}
}

template<typename... BAs>
void build_output(const std::string& name, const std::vector<std::string>& values,
		const std::string& type, auto& assgn) {
	size_t t = 0;
	for (const auto& val : values) {
		auto out_var = build_out_variable_at_n<BAs...>(name, t);
		if (val.empty()) {
			assgn.emplace(out_var, nullptr);
		} else {
			auto v = nso_factory<BAs...>::instance().parse( val, type);
			auto v_const = build_bf_constant(v.value(), type);
			assgn.emplace(out_var, v_const);
		}
		++t;
	}
}

inline bool matches_output(const auto& assm, const auto& memory) {
	for (const auto& [var, val] : assm) {
		if (val == nullptr) continue;
		if (auto it = memory.find(var); it != memory.end()) {
			if (!are_bf_equal(it->second, val)) {
				std::cout << "(Error) " << it->second << " != " << val << "\n";
#ifdef DEBUG
				std::cout << "first:\n";
				ptree(std::cout, it->second);
				std::cout << "\n";
				std::cout << "second:\n";
				ptree(std::cout, val);
				std::cout << "\n";
#endif // DEBUG
				return false;
			}
		} else {
			std::cout << "(Error) " << var << " not found\n";
			return false;
		}
	}
	return true;
}

template<typename... BAs>
tau<BAs...> create_spec(const char* spec) {
	auto sample_src = make_tau_source(spec);
	return make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(
		sample_src).value().main;
}

TEST_SUITE("Execution") {
	TEST_CASE("o1[t] = i1[t]") {
		bdd_init<Bool>();
		auto spec = create_spec<base_bas>("o1[t] = i1[t].");
		std::vector<std::string> i1 = {"<:x> = 0", "<:y> = 0", "<:z> = 0"};
		std::vector<std::string> o1 = i1;
		std::vector<std::map<tau<base_bas>, tau<base_bas>>> assgn_in;
		std::map<tau<base_bas>, tau<base_bas>> assgn_out;
		build_input<base_bas>("i1", i1, "tau", assgn_in);
		build_output<base_bas>("o1", o1, "tau", assgn_out);
		auto ins = input_vector<base_bas>(assgn_in, "tau");
		auto outs = output_console<base_bas>("tau");
		auto i = run(spec, ins, outs, 3);
		CHECK( matches_output(assgn_out, i.value().memory) );
	}

	TEST_CASE("u[t] = i1[t]: dec_seq") {
		bdd_init<Bool>();
		auto spec = create_spec<base_bas>("u[t] = i1[t].");
		std::vector<std::string> i1 = {
			"F", "o1[t] = o1[t-1]&i2[t] && o1[0] = 1", "F", "F", "F", "F"
		};
		std::vector<std::string> i2 = {
			"F", "F", "F", "<:x> = 0", "<:y> = 0", "<:z> = 0"
		};
		std::vector<std::string> u = {
			"F", "always o1[t]o1[t-1]' = 0 && i2[t]o1[t]'o1[t-1] = 0 && i2[t]'o1[t] = 0 && o1[0]' = 0",
			"F", "F", "F", "F"
		};
		std::vector<std::string> o1 = {
			"", "", "T", "<:x> = 0", "<:x> = 0 && <:y> = 0",
			"<:x> = 0 && <:y> = 0 && <:z> = 0"
		};
		std::vector<std::map<tau<base_bas>, tau<base_bas>>> assgn_in;
		std::map<tau<base_bas>, tau<base_bas>> assgn_out;
		build_input<base_bas>("i1", i1, "tau", assgn_in);
		build_input<base_bas>("i2", i2, "tau", assgn_in);
		build_output<base_bas>("o1", o1, "tau", assgn_out);
		build_output<base_bas>("u", u, "tau", assgn_out);
		auto ins = input_vector<base_bas>(assgn_in, "tau");
		auto outs = output_console<base_bas>("tau");
		auto i = run(spec, ins, outs, 6);
		CHECK( matches_output(assgn_out, i.value().memory) );
	}

	TEST_CASE("u[t] = i1[t]: negative_rel_pos") {
		bdd_init<Bool>();
		auto spec = create_spec<base_bas>(
			"u[t] = i1[t] && o1[2] = { <:x> = 0 } && o2[1] = { <:y> = 0 }.");
		std::vector<std::string> i1 = {
			"F", "F", "o3[t] = o1[-1] & o2[-2]", "F", "F"
		};
		std::vector<std::string> u = {
			"F", "F", "o3[t] = o1[-1] & o2[-2]", "F", "F"
		};
		std::vector<std::string> o3 = {
			"", "", "", "<:x> = 0 && <:y> = 0", "<:x> = 0 && <:y> = 0"
		};
		std::vector<std::map<tau<base_bas>, tau<base_bas>>> assgn_in;
		std::map<tau<base_bas>, tau<base_bas>> assgn_out;
		build_input<base_bas>("i1", i1, "tau", assgn_in);
		build_output<base_bas>("o3", o3, "tau", assgn_out);
		build_output<base_bas>("u", u, "tau", assgn_out);
		auto ins = input_vector<base_bas>(assgn_in, "tau");
		auto outs = output_console<base_bas>("tau");
		auto i = run(spec, ins, outs, 5);
		CHECK( matches_output(assgn_out, i.value().memory) );
	}

	TEST_CASE("u[t] = i1[t]: 2_clauses") {
		bdd_init<Bool>();
		auto spec = create_spec<base_bas>("u[t] = i1[t] && o2[t] = 0.");
		std::vector<std::string> i1 = {
			"(always o2[-1] = 1) || (always o3[t] = 1)", "F", "F", "F"
		};
		std::vector<std::string> u = {
			"(always o2[-1]' = 0) || (always o3[t]' = 0)", "F", "F", "F"
		};
		std::vector<std::string> o2 = {
			"F", "F", "F", "F",
		};
		std::vector<std::string> o3 = {
			"", "T", "T", "T"
		};
		std::vector<std::map<tau<base_bas>, tau<base_bas>>> assgn_in;
		std::map<tau<base_bas>, tau<base_bas>> assgn_out;
		build_input<base_bas>("i1", i1, "tau", assgn_in);
		build_output<base_bas>("o3", o3, "tau", assgn_out);
		build_output<base_bas>("o2", o2, "tau", assgn_out);
		build_output<base_bas>("u", u, "tau", assgn_out);
		auto ins = input_vector<base_bas>(assgn_in, "tau");
		auto outs = output_console<base_bas>("tau");
		auto i = run(spec, ins, outs, 4);
		CHECK( matches_output(assgn_out, i.value().memory) );
	}

	TEST_CASE("u[t] = i1[t]: history_unsat") {
		bdd_init<Bool>();
		auto spec = create_spec<base_bas>("u[t] = i1[t] && o1[t] = 0.");
		std::vector<std::string> i1 = {
			"F", "o1[-1] = 1", "F", "F"
		};
		std::vector<std::string> u = {
			"F", "o1[-1] = 1", "F", "F"
		};
		std::vector<std::string> o1 = {
			"F", "F", "F", "F",
		};
		std::vector<std::map<tau<base_bas>, tau<base_bas>>> assgn_in;
		std::map<tau<base_bas>, tau<base_bas>> assgn_out;
		build_input<base_bas>("i1", i1, "tau", assgn_in);
		build_output<base_bas>("o1", o1, "tau", assgn_out);
		build_output<base_bas>("u", u, "tau", assgn_out);
		auto ins = input_vector<base_bas>(assgn_in, "tau");
		auto outs = output_console<base_bas>("tau");
		auto i = run(spec, ins, outs, 4);
		CHECK( matches_output(assgn_out, i.value().memory) );
	}

	TEST_CASE("u[t] = i1[t]: spec_replace") {
		bdd_init<Bool>();
		auto spec = create_spec<base_bas>("u[t] = i1[t] && o1[t] = 0.");
		std::vector<std::string> i1 = {
			"F", "o1[t] = 1", "F", "F"
		};
		std::vector<std::string> u = {
			"F", "o1[t] = 1", "", ""
		};
		std::vector<std::string> o1 = {
			"F", "F", "T", "T",
		};
		std::vector<std::map<tau<base_bas>, tau<base_bas>>> assgn_in;
		std::map<tau<base_bas>, tau<base_bas>> assgn_out;
		build_input<base_bas>("i1", i1, "tau", assgn_in);
		build_output<base_bas>("o1", o1, "tau", assgn_out);
		build_output<base_bas>("u", u, "tau", assgn_out);
		auto ins = input_vector<base_bas>(assgn_in, "tau");
		auto outs = output_console<base_bas>("tau");
		auto i = run(spec, ins, outs, 4);
		CHECK( matches_output(assgn_out, i.value().memory) );
	}

	TEST_CASE("this_stream") {
		bdd_init<Bool>();
		auto spec = create_spec<base_bas>("u[t] = i1[t] && this[t] = o1[t].");
		std::vector<std::string> i1 = {
			"o2[t] = 0", "F", "o3[t] = 0", "F"
		};
		std::vector<std::string> u = {
			"o2[t] = 0", "F", "o3[t] = 0", "F"
		};
		std::vector<std::string> o1 = {
			"always o1[t]this[t]' = 0 && o1[t]'this[t] = 0 && i1[t]'o1[t]u[t] = 0 && i1[t]'this[t]'u[t] = 0 && i1[t]u[t]' = 0",
			"always o1[t]this[t]' = 0 && o1[t]'this[t] = 0 && o2[t] = 0 && i1[t]'o1[t]u[t] = 0 && i1[t]'this[t]'u[t] = 0 && i1[t]u[t]' = 0",
			"always o1[t]this[t]' = 0 && o1[t]'this[t] = 0 && o2[t] = 0 && i1[t]'o1[t]u[t] = 0 && i1[t]'this[t]'u[t] = 0 && i1[t]u[t]' = 0",
			"always o1[t]this[t]' = 0 && o1[t]'this[t] = 0 && o2[t] = 0 && o3[t] = 0 && i1[t]'o1[t]u[t] = 0 && i1[t]'this[t]'u[t] = 0 && i1[t]u[t]' = 0"
		};
		std::vector<std::map<tau<base_bas>, tau<base_bas>>> assgn_in;
		std::map<tau<base_bas>, tau<base_bas>> assgn_out;
		build_input<base_bas>("i1", i1, "tau", assgn_in);
		build_output<base_bas>("o1", o1, "tau", assgn_out);
		build_output<base_bas>("u", u, "tau", assgn_out);
		auto ins = input_vector<base_bas>(assgn_in, "tau");
		auto outs = output_console<base_bas>("tau");
		auto i = run(spec, ins, outs, 4);
		CHECK( matches_output(assgn_out, i.value().memory) );
	}
}

std::optional<assignment<tau_ba<sbf_ba>, sbf_ba>> run_test(const char* sample,
		input_vector<tau_ba<sbf_ba>, sbf_ba>& inputs,
		output_console<tau_ba<sbf_ba>, sbf_ba>& outputs,
		const size_t& times) {
	auto sample_src = make_tau_source(sample);
	auto spec = make_nso_rr_using_factory<
		tau_ba<sbf_ba>, sbf_ba>(sample_src).value().main;

	#ifdef DEBUG
	std::cout << "run_test/------------------------------------------------------\n";
	std::cout << "run_test/sample: " << sample << "\n";
	#endif // DEBUG

	auto intprtr = interpreter<
		input_vector<tau_ba<sbf_ba>, sbf_ba>,
		output_console<tau_ba<sbf_ba>, sbf_ba>,
		tau_ba<sbf_ba>, sbf_ba>::make_interpreter(spec, inputs, outputs);
	if (intprtr) {
		// we read the inputs only once (they are always empty in this test suite)

		for (size_t i = 0; i < times; ++i) {
			// we execute the i-th step
			auto in = inputs.get();

			#ifdef DEBUG
			std::cout << "run_test/input[" << i << "]: ";
			if (in.has_value()) {
				for (const auto& [var, value]: in.value())
					std::cout << var << " <- " << value << " ... ";
				std::cout << "\n";
			} else std::cout << "{}\n"; // no input
			#endif // DEBUG

			auto [out, _ ] = intprtr.value().step();

			// The output can be empty if all variables have been assigned in previous steps
			if (!out.has_value()) {
				intprtr.value().memory.clear();
				#ifdef DEBUG
				std::cout << "run_test/output[" << i << "]: {}\n"; // no output
				#endif // DEBUG
				break;
			}

			#ifdef DEBUG
			std::cout << "run_test/output[" << i << "]: ";
			for (const auto& [var, value]: out.value()) {
				std::cout << var << " <- " << value << " ... ";
				if (auto io_vars = find_top(value, is_non_terminal<tau_parser::io_var, tau_ba<sbf_ba>, sbf_ba>); io_vars) {
					std::cout << "run_test/output[" << i << "]: unexpected io_var " << io_vars.value() << "\n";
					intprtr.value().memory.clear();
					break;
				}
			}
			std::cout << "\n";
			#endif // DEBUG
		}

		return intprtr.value().memory;
	}
	return {};
}

std::optional<assignment<tau_ba<sbf_ba>, sbf_ba>> run_test(const char* sample,
		const size_t& times, const std::string& type = "sbf") {
	input_vector<tau_ba<sbf_ba>, sbf_ba> inputs(type);
	output_console<tau_ba<sbf_ba>, sbf_ba> outputs(type);
	return run_test(sample, inputs, outputs, times);
}

std::optional<assignment<tau_ba<sbf_ba>, sbf_ba>> run_test(const char* sample,
		input_vector<tau_ba<sbf_ba>, sbf_ba>& inputs,
		const size_t& times) {
	output_console<tau_ba<sbf_ba>, sbf_ba> outputs;
	return run_test(sample, inputs, outputs, times);
}

std::optional<assignment<tau_ba<sbf_ba>, sbf_ba>> run_test(const char* sample,
		output_console<tau_ba<sbf_ba>, sbf_ba>& outputs,
		const size_t& times) {
	input_vector<tau_ba<sbf_ba>, sbf_ba> inputs;
	return run_test(sample, inputs, outputs, times);
}

TEST_SUITE("configuration") {

	TEST_CASE("logging") {
		core::get()->set_filter(trivial::severity >= trivial::error);
		add_console_log(std::cout, keywords::format =
			expressions::stream << expressions::smessage);
	}

	TEST_CASE("bdd initialization") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("only outputs") {

	// In this test suite we test the interpreter with only outputs in the Tau
	// specifications, no inputs are provided at all. The outputs are written
	// to the console.

	// coonstant test cases

	TEST_CASE("o1[t] = 0") {
		const char* sample = "o1[t] = 0.";
		auto memory = run_test(sample, 2);
		CHECK ( !memory.value().empty() );
	}

	TEST_CASE("o1[t] = {a}:sbf") {
		const char* sample = "o1[t] = {a}:sbf.";
		auto memory = run_test(sample, 2);
		CHECK ( !memory.value().empty() );
	}

	TEST_CASE("o1[0] = 1") {
		const char* sample = "o1[0] = 1.";
		auto memory = run_test(sample, 3);
		CHECK ( !memory.value().empty() );
	}

	TEST_CASE("o1[1] = 1") {
		const char* sample = "o1[1] = 1.";
		auto memory = run_test(sample, 3);
		CHECK ( !memory.value().empty() );
	}

	TEST_CASE("o1[0] = {a}:sbf") {
		const char* sample = "o1[0] = {a}:sbf.";
		auto memory = run_test(sample, 3);
		CHECK ( !memory.value().empty() );
	}

	TEST_CASE("o1[1] = {a}:sbf") {
		const char* sample = "o1[1] = {a}:sbf.";
		auto memory = run_test(sample, 3);
		CHECK ( !memory.value().empty() );
	}

	// TODO (HIGH) check with Ohad the proper output in this case.
	// The spec is T, i.e. all outputs are valid. However, due to our inner
	// implementation, we are not defining outputs as we have T in the spec.
	// and hence, we are not able to provide outputs.
	// One possibility is return an optional instead of an assignment and
	// return an optional with an empty assigment in this case.
	TEST_CASE("o1[t] & o1[t]'= 0") {
		const char* sample = "o1[t] & o1[t]' = 0.";
		auto memory = run_test(sample, 2);
		CHECK ( (!memory.has_value() || memory.value().empty()) );
	}

	TEST_CASE("o1[0] = 1 && o1[t] = o1[t-1]") {
		const char* sample = "o1[0] = 1 && o1[t] = o1[t-1].";
		auto memory = run_test(sample, 2);
		CHECK ( !memory.value().empty() );
	}

	TEST_CASE("o1[0] = {a}:sbf && o1[t] = o1[t-1]") {
		const char* sample = "o1[0] = {a}:sbf && o1[t] = o1[t-1].";
		auto memory = run_test(sample, 3);
		CHECK ( !memory.value().empty() );
	}

	TEST_CASE("o1[0] = {a}:sbf && o1[t] != o1[t-1]") {
		const char* sample = "o1[0] = {a}:sbf && o1[t] = o1[t-1].";
		auto memory = run_test(sample, 3);
		CHECK ( !memory.value().empty() );
	}

	TEST_CASE("o1[t] | o2[t]= 0") {
		const char* sample = "o1[t] | o2[t]= 0.";
		auto memory = run_test(sample, 2);
		CHECK ( !memory.value().empty() );
	}

	TEST_CASE("o1[t] & o1[t-1] = 1") {
		const char* sample = "o1[t] & o1[t-1] = 1.";
		auto memory = run_test(sample, 2);
		CHECK ( !memory.value().empty() );
	}

	TEST_CASE("o1[t] + o1[t-1] = 1") {
		const char* sample = "o1[t] + o1[t-1] = 1.";
		auto memory = run_test(sample, 2);
		CHECK ( !memory.value().empty() );
	}

	TEST_CASE("o1[0] = {a}:sbf && o1[t] < o1[t-1] && o1[t] != 0") {
		const char* sample = "o1[0] = {a}:sbf && o1[t] < o1[t-1] && o1[t] != 0.";
		auto memory = run_test(sample, 4);
		CHECK ( !memory.value().empty() );
	}

	// increasing monotonicity (2)
	TEST_CASE("o1[0] = {a}:sbf && o1[t] > o1[t-1] && o1[t] != 1") {
		const char* sample = "o1[0] = {a}:sbf && o1[t] > o1[t-1] && o1[t] != 1.";
		auto memory = run_test(sample, 4);
		CHECK ( !memory.value().empty() );
	}

	// increasing monotonicity (3)
	TEST_CASE("o1[0] = 0 && o1[t] > o1[t-1] && o1[t] != 1") {
		const char* sample = "o1[0] = {a}:sbf && o1[t] > o1[t-1] && o1[t] != 1.";
		auto memory = run_test(sample, 4);
		CHECK ( !memory.value().empty() );
	}

	// increasing monotonicity (4)
	TEST_CASE("o1[t] > o1[t-1] && o1[t] != 1") {
		const char* sample = "o1[0] = {a}:sbf && o1[t] > o1[t-1] && o1[t] != 1.";
		auto memory = run_test(sample, 4);
		CHECK ( !memory.value().empty() );
	}

	// increasing monotonicity (5)
	TEST_CASE("o1[t] > o1[t-1]") {
		const char* sample = "o1[0] = {a}:sbf && o1[t] > o1[t-1] && o1[t] != 1.";
		auto memory = run_test(sample, 4);
		CHECK ( !memory.value().empty() );
	}

	TEST_CASE("<:a> o1[t] + <:b> o1[t]' = 0") {
		const char* sample = "<:a> o1[t] + <:b> o1[t]' = 0.";
		auto memory = run_test(sample, 8);
		CHECK ( !memory.value().empty() );
	}

	TEST_CASE("o1[0] = <:c> && o1[t] = <:a> o1[t-1] + <:b> o1[t-1]'") {
		const char* sample = "o1[0] = <:c> && o1[t] = <:a> o1[t-1] + <:b> o1[t-1]'.";
		auto memory = run_test(sample, 8);
		CHECK ( !memory.value().empty() );
	}

	// f(f(f(x))) = f(x) using uninterpreted constants
	TEST_CASE("o1[t] = <:a> o1[t-1] + <:b> o1[t-1]'") {
		const char* sample = "o1[t] = <:a> o1[t-1] + <:b> o1[t-1]'.";
		auto memory = run_test(sample, 8);
		CHECK ( !memory.value().empty() );
	}

	// f(f(f(x))) = f(x) using constants
	TEST_CASE("o1[t] = {a}:sbf o1[t-1] + {b}:sbf o1[t-1]'") {
		// TODO (CHECK this one too
		// const char* sample = "o1[t] = {a}:sbf o1[t-1] + {b}:sbf o1[t-1]'.";
		const char* sample = "o1[t] = {a}:sbf o1[t-1] + {b}:sbf o1[t-1]'.";
		auto memory = run_test(sample, 8);
		CHECK ( !memory.value().empty() );
	}

	// Fibonacci like sequence with BA constants (0 or 1)
	TEST_CASE("o1[0] = 1 && o1[1] = 1 && o1[t] = o1[t-1] + o1[t-2]") {
		const char* sample = "o1[0] = 1 && o1[1] = 1 && o1[t] = o1[t-1] + o1[t-2].";
		auto memory = run_test(sample, 8);
		CHECK ( !memory.value().empty() );
	}

	// Fibonacci like sequence with SBFs
	TEST_CASE("o1[0] = {a}:sbf && o1[1] = {a}:sbf && o1[t] = o1[t-1] + o1[t-2]") {
		const char* sample = "o1[0] =  {a}:sbf && o1[1] =  {a}:sbf && o1[t] = o1[t-1] + o1[t-2].";
		auto memory = run_test(sample, 8);
		CHECK ( !memory.value().empty() );
	}

	// Fibonacci like sequence with sample Tau syntax
	TEST_CASE("o1[0] = {<:x> = 0.} && o1[1] = {<:x> = 0.} && o1[t] = o1[t-1] + o1[t-2]") {
		const char* sample = "o1[0] =  {<:x> = 0.} && o1[1] =  {<:x> = 0.} && o1[t] = o1[t-1] + o1[t-2].";
		auto memory = run_test(sample, 8, "tau");
		CHECK ( !memory.value().empty() );
	}

	// Fibonacci like sequence with sample Tau programs
	TEST_CASE("o1[0] = {o1[0] = 0.} && o1[1] = {o1[0] = 0.} && o1[t] = o1[t-1] + o1[t-2]") {
		const char* sample = "o1[0] =  {o1[0] = 0.} && o1[1] =  {o1[0] = 0.} && o1[t] = o1[t-1] + o1[t-2].";
		auto memory = run_test(sample, 8, "tau");
		CHECK ( !memory.value().empty() );
	}
}


TEST_SUITE("with inputs and outputs") {

	input_vector<tau_ba<sbf_ba>, sbf_ba> build_i1_inputs(
			std::vector<tau<tau_ba<sbf_ba>, sbf_ba>> values) {
		std::vector<assignment<tau_ba<sbf_ba>, sbf_ba>> assignments;
		size_t t = 0;
		for (const auto& value: values) {
			assignment<tau_ba<sbf_ba>, sbf_ba> assignment;
			assignment[build_in_variable_at_n<tau_ba<sbf_ba>, sbf_ba>(1, t)] = value;
			assignments.push_back(assignment);
			++t;
		}
		input_vector<tau_ba<sbf_ba>, sbf_ba> ins(assignments);
		return ins;
	}

	TEST_CASE("i1[t] = o1[t]") {
		const char* sample = "i1[t] = o1[t].";
		auto ins = build_i1_inputs({
			_1<tau_ba<sbf_ba>, sbf_ba>,
			_0<tau_ba<sbf_ba>, sbf_ba>,
			_0<tau_ba<sbf_ba>, sbf_ba> });
		auto memory = run_test(sample, ins, 3);
		CHECK ( !memory.value().empty() );
	}

	// In this case, we get an error as the output should be equal to the
	// input all the time, but the output is set to 0 at the beginning.
	//
	// Note that this specification is unsat because i1[0] = 0 is deduced
	// which is an assumption on an input stream
	TEST_CASE("i1[t] = o1[t] && o1[0] = 0") {
		const char* sample = "i1[t] = o1[t] && o1[0] = 0.";
		auto ins = build_i1_inputs({
			_1<tau_ba<sbf_ba>, sbf_ba>,
			_1<tau_ba<sbf_ba>, sbf_ba>,
			_1<tau_ba<sbf_ba>, sbf_ba> });
		auto memory = run_test(sample, ins, 3);
		CHECK ( (!memory.has_value() || memory.value().empty()) );
	}

	// In this case we get the sequence 0, 1, 1 as the output is set to 1
	// at the beginning.
	TEST_CASE("i1[t-1] = o1[t] && o1[0] = 0") {
		const char* sample = "i1[t-1] = o1[t] && o1[0] = 0.";
		auto ins = build_i1_inputs({
			_1<tau_ba<sbf_ba>, sbf_ba>,
			_1<tau_ba<sbf_ba>, sbf_ba>,
			_1<tau_ba<sbf_ba>, sbf_ba> });
		auto memory = run_test(sample, ins, 2);
		CHECK ( !memory.value().empty() );
	}

}

TEST_SUITE("test inputs") {

	TEST_CASE("reading from file with sbf inputs") {
		std::map<tau<tau_ba<sbf_ba>, sbf_ba>, std::pair<type, std::string>> input_map;
		auto var = build_in_var_name<tau_ba<sbf_ba>, sbf_ba>(1);
		input_map[var] = { "sbf", "integration/test_files/sbf-alternating_zeros_and_ones-length_10.in"};
		finputs<tau_ba<sbf_ba>, sbf_ba> inputs(input_map);
		CHECK ( inputs.type_of(var).has_value() );
		for (size_t i = 0; i < 10; ++i) {
			auto in = inputs.read();
			if (in) {
				auto check = (i % 2)
					? in.value()[var] == _1<tau_ba<sbf_ba>, sbf_ba>
					: in.value()[var] == _0<tau_ba<sbf_ba>, sbf_ba>;
				CHECK ( check );
			} else FAIL("no input");
		}
	}

	TEST_CASE("reading from file with tau program inputs") {
		std::map<tau<tau_ba<sbf_ba>, sbf_ba>, std::pair<type, std::string>> input_map;
		auto var = build_in_var_name<tau_ba<sbf_ba>, sbf_ba>(1);
		input_map[var] = { "tau", "integration/test_files/tau-alternating_zeros_and_ones-length_10.in"};
		finputs<tau_ba<sbf_ba>, sbf_ba> inputs(input_map);
		CHECK ( inputs.type_of(var).has_value() );
		for (size_t i = 0; i < 10; ++i) {
			auto in = inputs.read();
			if (in) {
				auto check = (i % 2)
					? in.value()[var] == _1<tau_ba<sbf_ba>, sbf_ba>
					: in.value()[var] == _0<tau_ba<sbf_ba>, sbf_ba>;
				CHECK ( check );
			} else FAIL("no input");
		}
	}
}

TEST_SUITE("test outputs") {

	TEST_CASE("writing to file") {
		std::map<tau<tau_ba<sbf_ba>, sbf_ba>, std::pair<type, std::string>> output_map;
		auto var = build_out_var_name<tau_ba<sbf_ba>, sbf_ba>(1);
		auto var_0 = build_out_variable_at_n<tau_ba<sbf_ba>, sbf_ba>(1, 0);

		output_map[var] = { "sbf", random_file() };

		#ifdef DEBUG
		std::cout << "test_outputs/writing_to_file/output: " << output_map[var].second << "\n";
		#endif // DEBUG

		foutputs<tau_ba<sbf_ba>, sbf_ba> outputs(output_map);
		assignment<tau_ba<sbf_ba>, sbf_ba> output = {
			{ var_0, _1<tau_ba<sbf_ba>, sbf_ba> }
		};

		CHECK( outputs.type_of(var).has_value() );
		CHECK ( outputs.write(output) );
	}

	TEST_CASE("writing to files: two outputs") {
		std::map<tau<tau_ba<sbf_ba>, sbf_ba>, std::pair<type, std::string>> output_map;
		auto var1 = build_out_var_name<tau_ba<sbf_ba>, sbf_ba>(1);
		auto var2 = build_out_var_name<tau_ba<sbf_ba>, sbf_ba>(2);
		auto var1_0 = build_out_variable_at_n<tau_ba<sbf_ba>, sbf_ba>(1, 0);
		auto var2_0 = build_out_variable_at_n<tau_ba<sbf_ba>, sbf_ba>(2, 0);
		output_map[var1] = {"sbf", random_file()};
		output_map[var2] = {"sbf", random_file()};

		#ifdef DEBUG
		std::cout << "test_outputs/writing_to_file/output: " << output_map[var1].second << "\n";
		std::cout << "test_outputs/writing_to_file/output: " << output_map[var2].second << "\n";
		#endif // DEBUG

		foutputs<tau_ba<sbf_ba>, sbf_ba> outputs(output_map);
		assignment<tau_ba<sbf_ba>, sbf_ba> output = {
			{ var1_0, _1<tau_ba<sbf_ba>, sbf_ba> },
			{ var2_0, _0<tau_ba<sbf_ba>, sbf_ba> }
		};

		CHECK( outputs.type_of(var1).has_value() );
		CHECK( outputs.type_of(var2).has_value() );
		CHECK ( outputs.write(output) );
	}

	TEST_CASE("writing to files: completing outputs") {
		std::map<tau<tau_ba<sbf_ba>, sbf_ba>, std::pair<type, std::string>> output_map;
		auto var1 = build_out_var_name<tau_ba<sbf_ba>, sbf_ba>(1);
		auto var2 = build_out_var_name<tau_ba<sbf_ba>, sbf_ba>(2);
		auto var1_0 = build_out_variable_at_n<tau_ba<sbf_ba>, sbf_ba>(1, 0);
		auto var2_1 = build_out_variable_at_n<tau_ba<sbf_ba>, sbf_ba>(2, 1);
		output_map[var1] = {"sbf", random_file()};
		output_map[var2] = {"sbf", random_file()};

		#ifdef DEBUG
		std::cout << "test_outputs/writing_to_file/output: " << output_map[var1].second << "\n";
		std::cout << "test_outputs/writing_to_file/output: " << output_map[var2].second << "\n";
		#endif // DEBUG

		foutputs<tau_ba<sbf_ba>, sbf_ba> outputs(output_map);
		assignment<tau_ba<sbf_ba>, sbf_ba> output = {
			{ var1_0, _1<tau_ba<sbf_ba>, sbf_ba> },
			{ var2_1, _1<tau_ba<sbf_ba>, sbf_ba> }
		};

		CHECK( outputs.type_of(var1).has_value() );
		CHECK( outputs.type_of(var2).has_value() );
		CHECK ( outputs.write(output) );
	}
}
