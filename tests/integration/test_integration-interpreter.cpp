// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <random>

#include "test_init.h"
#include "test_tau_helpers.h"
#ifdef DEBUG // in release it is included with tau.h
#	include "interpreter.h"
#endif

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

struct output_console {
	output_console() = default;
	output_console(const std::string& type)
		: _type(get_ba_type_id<node_t>(type)) {}

	bool write(const assignment<node_t>& outputs) const {
		// for each stream in out.streams, write the value from the solution
		for (const auto& [var, value]: outputs)
			std::cout << var << " := " << value << "\n";
		return true; // success (always)
	}

	size_t type_of(tref) const { return _type; }

	void add_output(tref v, size_t, size_t) {
		if (!streams.contains(v)) streams.emplace(v, v);
	}

	assignment<node_t> streams;
	size_t _type = get_ba_type_id<node_t>("sbf");
};

struct input_vector {

	input_vector() = default;
	input_vector(std::vector<assignment<node_t>>& inputs) : inputs(
		std::move(inputs)) {}
	input_vector(const std::string& type)
		: _type(get_ba_type_id<node_t>(type)) {}
	input_vector(std::vector<assignment<node_t>>& inputs,
		const std::string& type) : inputs(std::move(inputs)),
			_type(get_ba_type_id<node_t>(type)) {}

	std::optional<assignment<node_t>> get() const {
		if (inputs.empty()) return { assignment<node_t>{} };
		if (current == inputs.size()) return {};
		return inputs[current];
	}

	std::pair<std::optional<assignment<node_t>>, bool> read(
		const auto&, const size_t)
	{
		if (inputs.empty()) return { assignment<node_t>{}, false };
		if (current >= inputs.size()) return { assignment<node_t>{}, false };
		return { inputs[current++], false };
	}

	size_t type_of(tref) const { return _type; }

	void add_input(tref v, size_t, size_t) {
		if (!streams.contains(v)) streams.emplace(v, v);
	}

	std::vector<assignment<node_t>> inputs;
	assignment<node_t> streams;
	size_t current = 0;
	size_t _type = get_ba_type_id<node_t>("sbf");
};

void build_input(const std::string& name,
	const std::vector<std::string>& values, const std::string& type,
	auto& assgn)
{
	size_t t = 0;
	for (const auto& val : values) {
		auto in_var = build_in_var_at_n<node_t>(name, t);
		auto v = node_t::nso_factory::instance().parse(val, type);
		auto v_const = tau::get_ba_constant(v.value().first,
			get_ba_type_id<node_t>(type));

		if (assgn.size() <= t) {
			subtree_map<node_t, tref> a;
			a.emplace(in_var, v_const);
			assgn.emplace_back(std::move(a));
		} else assgn[t].emplace(in_var, v_const);
		++t;
	}
}

void build_output(const std::string& name, const std::vector<std::string>& values,
		const std::string& type, auto& assgn) {
	size_t t = 0;
	for (const auto& val : values) {
		auto out_var = build_out_var_at_n<node_t>(name, t);
		if (val.empty()) {
			assgn.emplace(out_var, nullptr);
		} else {
			auto v = node_t::nso_factory::instance().parse(val, type);
			auto v_const = tau::get_ba_constant(v.value().first,
				get_ba_type_id<node_t>(type));
			assgn.emplace(out_var, v_const);
		}
		++t;
	}
}

inline bool matches_output(const auto& assm, const auto& memory) {
	using node = node_t;
	for (const auto& [var, val] : assm) {
		if (val == nullptr) continue;
		if (auto it = memory.find(var); it != memory.end()) {
			if (!are_bf_equal<node_t>(it->second, val)) {
				TAU_LOG_ERROR << TAU_LOG_FM(it->second)
						<< " != " << TAU_LOG_FM(val);
#ifdef DEBUG
				TAU_LOG_INFO << "first:\n"
						<< TAU_LOG_FM_DUMP(it->second);
				TAU_LOG_INFO << "second:\n" << TAU_LOG_FM_DUMP(val);
#endif // DEBUG
				return false;
			}
		} else {
			TAU_LOG_ERROR << TAU_TO_STR(var) << " not found";
			return false;
		}
	}
	return true;
}

tref create_spec(const char* spec) {
	return get_nso_rr<node_t>(tau::get(spec)).value().main->get();
}

TEST_SUITE("Execution") {
	TEST_CASE("o1[t] = i1[t]") {
		bdd_init<Bool>();
		auto spec = create_spec("o1[t] = i1[t].");
		std::vector<std::string> i1 = {"<:x> = 0", "<:y> = 0", "<:z> = 0"};
		std::vector<std::string> o1 = i1;
		std::vector<assignment<node_t>> assgn_in;
		assignment<node_t> assgn_out;
		build_input("i1", i1, "tau", assgn_in);
		build_output("o1", o1, "tau", assgn_out);
		auto ins = input_vector(assgn_in, "tau");
		auto outs = output_console("tau");
		auto i = run<node_t>(spec, ins, outs, 3);
		CHECK( matches_output(assgn_out, i.value().memory) );
	}

	TEST_CASE("u[t] = i1[t]: dec_seq") {
		bdd_init<Bool>();
		auto spec = create_spec("u[t] = i1[t].");
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
		std::vector<assignment<node_t>> assgn_in;
		assignment<node_t> assgn_out;
		build_input("i1", i1, "tau", assgn_in);
		build_input("i2", i2, "tau", assgn_in);
		build_output("o1", o1, "tau", assgn_out);
		build_output("u", u, "tau", assgn_out);
		auto ins = input_vector(assgn_in, "tau");
		auto outs = output_console("tau");
		auto i = run<node_t>(spec, ins, outs, 6);
		CHECK( matches_output(assgn_out, i.value().memory) );
	}

	TEST_CASE("u[t] = i1[t]: negative_rel_pos") {
		bdd_init<Bool>();
		auto spec = create_spec(
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
		std::vector<assignment<node_t>> assgn_in;
		assignment<node_t> assgn_out;
		build_input("i1", i1, "tau", assgn_in);
		build_output("o3", o3, "tau", assgn_out);
		build_output("u", u, "tau", assgn_out);
		auto ins = input_vector(assgn_in, "tau");
		auto outs = output_console("tau");
		auto i = run<node_t>(spec, ins, outs, 5);
		CHECK( matches_output(assgn_out, i.value().memory) );
	}

	TEST_CASE("u[t] = i1[t]: 2_clauses") {
		bdd_init<Bool>();
		auto spec = create_spec("u[t] = i1[t] && o2[t] = 0.");
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
		std::vector<assignment<node_t>> assgn_in;
		assignment<node_t> assgn_out;
		build_input("i1", i1, "tau", assgn_in);
		build_output("o3", o3, "tau", assgn_out);
		build_output("o2", o2, "tau", assgn_out);
		build_output("u", u, "tau", assgn_out);
		auto ins = input_vector(assgn_in, "tau");
		auto outs = output_console("tau");
		auto i = run<node_t>(spec, ins, outs, 4);
		CHECK( matches_output(assgn_out, i.value().memory) );
	}

	TEST_CASE("u[t] = i1[t]: history_unsat") {
		bdd_init<Bool>();
		auto spec = create_spec("u[t] = i1[t] && o1[t] = 0.");
		std::vector<std::string> i1 = {
			"F", "o1[-1] = 1", "F", "F"
		};
		std::vector<std::string> u = {
			"F", "o1[-1] = 1", "F", "F"
		};
		std::vector<std::string> o1 = {
			"F", "F", "F", "F",
		};
		std::vector<assignment<node_t>> assgn_in;
		assignment<node_t> assgn_out;
		build_input("i1", i1, "tau", assgn_in);
		build_output("o1", o1, "tau", assgn_out);
		build_output("u", u, "tau", assgn_out);
		auto ins = input_vector(assgn_in, "tau");
		auto outs = output_console("tau");
		auto i = run<node_t>(spec, ins, outs, 4);
		CHECK( matches_output(assgn_out, i.value().memory) );
	}

	TEST_CASE("u[t] = i1[t]: spec_replace") {
		bdd_init<Bool>();
		auto spec = create_spec("u[t] = i1[t] && o1[t] = 0.");
		std::vector<std::string> i1 = {
			"F", "o1[t] = 1", "F", "F"
		};
		std::vector<std::string> u = {
			"F", "o1[t] = 1", "", ""
		};
		std::vector<std::string> o1 = {
			"F", "F", "T", "T",
		};
		std::vector<assignment<node_t>> assgn_in;
		assignment<node_t> assgn_out;
		build_input("i1", i1, "tau", assgn_in);
		build_output("o1", o1, "tau", assgn_out);
		build_output("u", u, "tau", assgn_out);
		auto ins = input_vector(assgn_in, "tau");
		auto outs = output_console("tau");
		auto i = run<node_t>(spec, ins, outs, 4);
		CHECK( matches_output(assgn_out, i.value().memory) );
	}
}


std::optional<assignment<node_t>> run_test(const char* sample,
	input_vector& inputs, output_console& outputs, const size_t& times)
{
	tref spec = create_spec(sample);

#ifdef DEBUG
	std::cout << "run_test/------------------------------------------------------\n";
	std::cout << "run_test/sample: " << sample << "\n";
#endif // DEBUG

	auto intprtr = interpreter<node_t, input_vector, output_console>
				::make_interpreter(spec, inputs, outputs);
	if (intprtr) {
		// we read the inputs only once (they are always empty in this test suite)

		for (size_t i = 0; i < times; ++i) {
			// we execute the i-th step
			auto in = inputs.get();

#ifdef DEBUG
			using node = node_t;
			std::cout << "run_test/input[" << i << "]: ";
			if (in.has_value()) {
				for (const auto& [var, value] : in.value())
					std::cout << TAU_TO_STR(var) << " <- " << TAU_TO_STR(value) << " ... ";
				std::cout << "\n";
			} else std::cout << "{}\n"; // no input
#endif // DEBUG

			auto [out, _ ] = intprtr.value().step ();

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
				std::cout << TAU_TO_STR(var) << " <- " << TAU_TO_STR(value) << " ... ";
				if (tref io_var = tau::get(value).find_top(is<node_t, tau::io_var>); io_var) {
					std::cout << "run_test/output[" << i << "]: unexpected io_var " << TAU_TO_STR(io_var) << "\n";
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

std::optional<assignment<node_t>> run_test(const char* sample,
		const size_t& times, const std::string& type = "sbf")
{
	input_vector inputs(type);
	output_console outputs(type);
	return run_test(sample, inputs, outputs, times);
}

std::optional<assignment<node_t>> run_test(const char* sample,
	input_vector& inputs, const size_t& times)
{
	output_console outputs;
	return run_test(sample, inputs, outputs, times);
}

std::optional<assignment<node_t>> run_test(const char* sample,
	output_console& outputs, const size_t& times)
{
	input_vector inputs;
	return run_test(sample, inputs, outputs, times);
}

TEST_SUITE("configuration") {

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

	input_vector build_i1_inputs(trefs values) {
		std::vector<assignment<node_t>> assignments;
		size_t t = 0;
		for (tref value : values) {
			assignment<node_t> assignment;
			assignment[build_in_var_at_n_indexed<node_t>(1, t)] = value;
			assignments.push_back(assignment);
			++t;
		}
		input_vector ins(assignments);
		return ins;
	}

	TEST_CASE("i1[t] = o1[t]") {
		const char* sample = "i1[t] = o1[t].";
		auto ins = build_i1_inputs({ tau::_1(), tau::_0(), tau::_0() });
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
		auto ins = build_i1_inputs({ tau::_1(), tau::_1(), tau::_1() });
		auto memory = run_test(sample, ins, 3);
		CHECK ( (!memory.has_value() || memory.value().empty()) );
	}

	// In this case we get the sequence 0, 1, 1 as the output is set to 1
	// at the beginning.
	TEST_CASE("i1[t-1] = o1[t] && o1[0] = 0") {
		const char* sample = "i1[t-1] = o1[t] && o1[0] = 0.";
		auto ins = build_i1_inputs({ tau::_1(), tau::_1(), tau::_1() });
		auto memory = run_test(sample, ins, 2);
		CHECK ( !memory.value().empty() );
	}

}

TEST_SUITE("test inputs") {

	TEST_CASE("reading from file with sbf inputs") {
		typed_io_vars input_map;
		tref var = build_var_name_indexed<node_t>(1);
		input_map[get_var_name_sid<node_t>(var)] = {
			get_typed_stream<node_t>("sbf", "integration/test_files/sbf-alternating_zeros_and_ones-length_10.in") };
		finputs<node_t> inputs(input_map);
		CHECK ( inputs.type_of(var) > 0 );
		for (size_t i = 0; i < 10; ++i) {
			auto in = inputs.read();
			if (in) {
				auto check = (i % 2)
					? tau::get(in.value()[var]).equals_1()
					: tau::get(in.value()[var]).equals_0();
				CHECK ( check );
			} else FAIL("no input");
		}
	}

	TEST_CASE("reading from file with tau program inputs") {
		typed_io_vars input_map;
		tref var = build_var_name_indexed<node_t>(1);
		input_map[get_var_name_sid<node_t>(var)] = {
			get_typed_stream<node_t>("tau", "integration/test_files/tau-alternating_zeros_and_ones-length_10.in") };
		finputs<node_t> inputs(input_map);
		CHECK ( inputs.type_of(var) > 0 );
		for (size_t i = 0; i < 10; ++i) {
			auto in = inputs.read();
			if (in) {
				auto check = (i % 2)
					? tau::get(in.value()[var]).equals_1()
					: tau::get(in.value()[var]).equals_0();
				CHECK ( check );
			} else FAIL("no input");
		}
	}
}

TEST_SUITE("test outputs") {

	TEST_CASE("writing to file") {
		typed_io_vars output_map;
		tref var = build_var_name_indexed<node_t>(1);
		size_t var_sid = get_var_name_sid<node_t>(var);
		tref var_0 = build_out_var_at_n_indexed<node_t>(1, 0);

		output_map[var_sid] = {
			get_typed_stream<node_t>("sbf", random_file()) };

#ifdef DEBUG
		std::cout << "test_outputs/writing_to_file/output: " << output_map[var_sid].second << "\n";
#endif // DEBUG

		foutputs<node_t> outputs(output_map);
		assignment<node_t> output = { { var_0, tau::_1() } };

		CHECK( outputs.type_of(var) > 0 );
		CHECK ( outputs.write(output) );
	}

	TEST_CASE("writing to files: two outputs") {
		typed_io_vars output_map;
		tref var1 = build_var_name_indexed<node_t>(1);
		tref var2 = build_var_name_indexed<node_t>(2);
		size_t var1_sid = get_var_name_sid<node_t>(var1);
		size_t var2_sid = get_var_name_sid<node_t>(var2);
		tref var1_0 = build_out_var_at_n<node_t>(var1, 0);
		tref var2_0 = build_out_var_at_n<node_t>(var2, 0);
		output_map[var1_sid] = {
			get_typed_stream<node_t>("sbf", random_file()) };
		output_map[var2_sid] = {
			get_typed_stream<node_t>("sbf", random_file()) };

#ifdef DEBUG
		std::cout << "test_outputs/writing_to_file/output: " << output_map[var1_sid].second << "\n";
		std::cout << "test_outputs/writing_to_file/output: " << output_map[var2_sid].second << "\n";
#endif // DEBUG

		foutputs<node_t> outputs(output_map);
		assignment<node_t> output = {
			{ var1_0, tau::_1() },
			{ var2_0, tau::_0() }
		};

		CHECK( outputs.type_of(var1) > 0 );
		CHECK( outputs.type_of(var2) > 0 );
		CHECK ( outputs.write(output) );
	}

	TEST_CASE("writing to files: completing outputs") {
		typed_io_vars output_map;
		auto var1 = build_var_name_indexed<node_t>(1);
		auto var2 = build_var_name_indexed<node_t>(2);
		size_t var1_sid = get_var_name_sid<node_t>(var1);
		size_t var2_sid = get_var_name_sid<node_t>(var2);
		auto var1_0 = build_out_var_at_n<node_t>(var1, 0);
		auto var2_1 = build_out_var_at_n<node_t>(var2, 1);
		output_map[var1_sid] = {
			get_typed_stream<node_t>("sbf", random_file()) };
		output_map[var2_sid] = {
			get_typed_stream<node_t>("sbf", random_file()) };

#ifdef DEBUG
		std::cout << "test_outputs/writing_to_file/output: " << output_map[var1_sid].second << "\n";
		std::cout << "test_outputs/writing_to_file/output: " << output_map[var2_sid].second << "\n";
#endif // DEBUG

		foutputs<node_t> outputs(output_map);
		assignment<node_t> output = {
			{ var1_0, tau::_1() },
			{ var2_1, tau::_1() }
		};

		CHECK( outputs.type_of(var1) > 0 );
		CHECK( outputs.type_of(var2) > 0 );
		CHECK ( outputs.write(output) );
	}
}
