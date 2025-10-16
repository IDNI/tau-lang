// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_integration-interpreter_helper.h"

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
		spec_context<node_t> ctx;
		auto i = run<node_t>(spec, ins, outs, ctx, 3);
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
		spec_context<node_t> ctx;
		auto i = run<node_t>(spec, ins, outs, ctx, 6);
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
		spec_context<node_t> ctx;
		auto i = run<node_t>(spec, ins, outs, ctx, 5);
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
		spec_context<node_t> ctx;
		auto i = run<node_t>(spec, ins, outs, ctx, 4);
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
		spec_context<node_t> ctx;
		auto i = run<node_t>(spec, ins, outs, ctx, 4);
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
		spec_context<node_t> ctx;
		auto i = run<node_t>(spec, ins, outs, ctx, 4);
		CHECK( matches_output(assgn_out, i.value().memory) );
	}

	TEST_CASE("this_stream") {
		bdd_init<Bool>();
		auto spec = create_spec("u[t] = i1[t] && this[t] = o1[t].");
		std::vector<std::string> i1 = {
			"o2[t] = 0", "F", "o3[t] = 0", "F"
		};
		std::vector<std::string> u = {
			"o2[t] = 0", "F", "o3[t] = 0", "F"
		};
		std::vector<std::string> o1 = {
			"always i1[t]o1[t]u[t]' = 0 && o1[t]this[t]' = 0 && o1[t]'this[t] = 0 && i1[t]this[t]'u[t]' = 0 && i1[t]'o1[t]u[t] = 0 && i1[t]'this[t]'u[t] = 0",
			"always i1[t]o1[t]u[t]' = 0 && o1[t]this[t]' = 0 && o1[t]'this[t] = 0 && i1[t]this[t]'u[t]' = 0 && o2[t] = 0 && i1[t]'o1[t]u[t] = 0 && i1[t]'this[t]'u[t] = 0",
			"always i1[t]o1[t]u[t]' = 0 && o1[t]this[t]' = 0 && o1[t]'this[t] = 0 && i1[t]this[t]'u[t]' = 0 && o2[t] = 0 && i1[t]'o1[t]u[t] = 0 && i1[t]'this[t]'u[t] = 0",
			"always i1[t]o1[t]u[t]' = 0 && o1[t]this[t]' = 0 && o1[t]'this[t] = 0 && i1[t]this[t]'u[t]' = 0 && o2[t] = 0 && o3[t] = 0 && i1[t]'o1[t]u[t] = 0 && i1[t]'this[t]'u[t] = 0"
		};
		std::vector<assignment<node_t>> assgn_in;
		assignment<node_t> assgn_out;
		build_input("i1", i1, "tau", assgn_in);
		build_output("o1", o1, "tau", assgn_out);
		build_output("u", u, "tau", assgn_out);
		auto ins = input_vector(assgn_in, "tau");
		ins.add_input(build_var_name<node_t>("this"),0,0);
		auto outs = output_console("tau");
		spec_context<node_t> ctx;
		auto i = run<node_t>(spec, ins, outs, ctx, 4);
		CHECK( matches_output(assgn_out, i.value().memory) );
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

	TEST_CASE("o1[t] ^ o1[t-1] = 1") {
		const char* sample = "o1[t] ^ o1[t-1] = 1.";
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

	TEST_CASE("<:a> o1[t] ^ <:b> o1[t]' = 0") {
		const char* sample = "<:a> o1[t] ^ <:b> o1[t]' = 0.";
		auto memory = run_test(sample, 8);
		CHECK ( !memory.value().empty() );
	}

	TEST_CASE("o1[0] = <:c> && o1[t] = <:a> o1[t-1] ^ <:b> o1[t-1]'") {
		const char* sample = "o1[0] = <:c> && o1[t] = <:a> o1[t-1] ^ <:b> o1[t-1]'.";
		auto memory = run_test(sample, 8);
		CHECK ( !memory.value().empty() );
	}

	// f(f(f(x))) = f(x) using uninterpreted constants
	TEST_CASE("o1[t] = <:a> o1[t-1] ^ <:b> o1[t-1]'") {
		const char* sample = "o1[t] = <:a> o1[t-1] ^ <:b> o1[t-1]'.";
		auto memory = run_test(sample, 8);
		CHECK ( !memory.value().empty() );
	}

	// f(f(f(x))) = f(x) using constants
	TEST_CASE("o1[t] = {a}:sbf o1[t-1] ^ {b}:sbf o1[t-1]'") {
		// TODO (CHECK this one too
		// const char* sample = "o1[t] = {a}:sbf o1[t-1] ^ {b}:sbf o1[t-1]'.";
		const char* sample = "o1[t] = {a}:sbf o1[t-1] ^ {b}:sbf o1[t-1]'.";
		auto memory = run_test(sample, 8);
		CHECK ( !memory.value().empty() );
	}

	// Fibonacci like sequence with BA constants (0 or 1)
	TEST_CASE("o1[0] = 1 && o1[1] = 1 && o1[t] = o1[t-1] ^ o1[t-2]") {
		const char* sample = "o1[0] = 1 && o1[1] = 1 && o1[t] = o1[t-1] ^ o1[t-2].";
		auto memory = run_test(sample, 8);
		CHECK ( !memory.value().empty() );
	}

	// Fibonacci like sequence with SBFs
	TEST_CASE("o1[0] = {a}:sbf && o1[1] = {a}:sbf && o1[t] = o1[t-1] ^ o1[t-2]") {
		const char* sample = "o1[0] =  {a}:sbf && o1[1] =  {a}:sbf && o1[t] = o1[t-1] ^ o1[t-2].";
		auto memory = run_test(sample, 8);
		CHECK ( !memory.value().empty() );
	}

	// Fibonacci like sequence with sample Tau syntax
	TEST_CASE("o1[0] = {<:x> = 0.} && o1[1] = {<:x> = 0.} && o1[t] = o1[t-1] ^ o1[t-2]") {
		const char* sample = "o1[0] =  {<:x> = 0.} && o1[1] =  {<:x> = 0.} && o1[t] = o1[t-1] ^ o1[t-2].";
		auto memory = run_test(sample, 8, "tau");
		CHECK ( !memory.value().empty() );
	}

	// Fibonacci like sequence with sample Tau programs
	TEST_CASE("o1[0] = {o1[0] = 0.} && o1[1] = {o1[0] = 0.} && o1[t] = o1[t-1] ^ o1[t-2]") {
		const char* sample = "o1[0] =  {o1[0] = 0.} && o1[1] =  {o1[0] = 0.} && o1[t] = o1[t-1] ^ o1[t-2].";
		auto memory = run_test(sample, 8, "tau");
		CHECK ( !memory.value().empty() );
	}
}


TEST_SUITE("with inputs and outputs") {

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
		bdd_init<Bool>();
		typed_io_vars input_map;
		tref var = build_var_name_indexed<node_t>(1);
		input_map[get_var_name_sid<node_t>(var)] = {
			get_typed_stream<node_t>("sbf", "../tests/integration/test_files/sbf-alternating_zeros_and_ones-length_10.in") };
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
		bdd_init<Bool>();
		typed_io_vars input_map;
		tref var = build_var_name_indexed<node_t>(1);
		input_map[get_var_name_sid<node_t>(var)] = {
			get_typed_stream<node_t>("tau", "../tests/integration/test_files/tau-alternating_zeros_and_ones-length_10.in") };
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
		bdd_init<Bool>();
		typed_io_vars output_map;
		tref var = build_var_name_indexed<node_t>(1);
		size_t var_sid = get_var_name_sid<node_t>(var);
		size_t type = get_ba_type_id<node_t>("sbf");
		tref var_0 = build_out_var_at_n_indexed<node_t>(1, 0, type);

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
		bdd_init<Bool>();
		typed_io_vars output_map;
		size_t type = get_ba_type_id<node_t>("sbf");
		tref var1 = build_var_name_indexed<node_t>(1);
		tref var2 = build_var_name_indexed<node_t>(2);
		size_t var1_sid = get_var_name_sid<node_t>(var1);
		size_t var2_sid = get_var_name_sid<node_t>(var2);
		tref var1_0 = build_out_var_at_n<node_t>(var1, 0, type);
		tref var2_0 = build_out_var_at_n<node_t>(var2, 0, type);
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
		bdd_init<Bool>();
		typed_io_vars output_map;
		size_t type = get_ba_type_id<node_t>("sbf");
		auto var1 = build_var_name_indexed<node_t>(1);
		auto var2 = build_var_name_indexed<node_t>(2);
		size_t var1_sid = get_var_name_sid<node_t>(var1);
		size_t var2_sid = get_var_name_sid<node_t>(var2);
		auto var1_0 = build_out_var_at_n<node_t>(var1, 0, type);
		auto var2_1 = build_out_var_at_n<node_t>(var2, 1, type);
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
