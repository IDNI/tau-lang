// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_integration-interpreter_helper.h"

TEST_SUITE("Execution") {

	TEST_CASE("o1[t] = i1[t]") {
		bdd_init<Bool>();
		auto spec = create_spec("o1[t] = i1[t].");
		io_context<node_t> ctx;
		std::vector<std::string> i1_values = {"<:x> = 0", "<:y> = 0", "<:z> = 0"};
		ctx.add_input( "i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 3);
		CHECK( maybe_i.has_value() );
		auto o1_values = o1->get_values();
		CHECK( o1_values == i1_values );
	}

	TEST_CASE("u[t] = i1[t]: dec_seq") {
		bdd_init<Bool>();
		auto spec = create_spec("u[t] = i1[t].");
		std::vector<std::string> i1_values = {
			"F", "o1[t] = o1[t-1]&i2[t] && o1[0] = 1", "F", "F", "F", "F"
		};
		std::vector<std::string> i2_values = {
			"<:x> = 0", "<:y> = 0", "<:z> = 0"
		};
		std::vector<std::string> u_expected = {
			"F", "always o1[0]:tau' = 0 && i2[t]:tau o1[t-1]:tau = o1[t]:tau",
			"F", "F", "F", "F"
		};
		std::vector<std::string> o1_expected = {
			"T", "<:x> = 0", "<:x> = 0 && <:y> = 0",
			"<:z> = 0 && <:x> = 0 && <:y> = 0"
		};
		io_context<node_t> ctx;
		auto i1 = std::make_shared<vector_input_stream>(i1_values);
		auto i2 = std::make_shared<vector_input_stream>(i2_values);
		auto o1 = std::make_shared<vector_output_stream>();
		auto u  = std::make_shared<vector_output_stream>();
		ctx.add_input( "i1", tau_type_id<node_t>(), i1);
		ctx.add_input( "i2", tau_type_id<node_t>(), i2);
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("u", tau_type_id<node_t>(), u);
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK( maybe_i.has_value() );
		auto o1_values = o1->get_values();
		CHECK( o1_values == o1_expected );
		auto u_values = u->get_values();
		CHECK( u_values == u_expected);
	}

	TEST_CASE("u[t] = i1[t]: negative_rel_pos") {
		bdd_init<Bool>();
		auto spec = create_spec(
			"u[t] = i1[t] && o1[2] = { <:x> = 0 } && o2[1] = { <:y> = 0 }.");
		std::vector<std::string> i1_values = {
			"F", "F", "o3[t] = o1[-1] & o2[-2]", "F", "F"
		};
		std::vector<std::string> u_expected = {
			"F", "F", "always o3[t]:tau = o2[-2]:tau o1[-1]:tau", "F", "F"
		};
		std::vector<std::string> o3_expected = {
			"<:x> = 0 && <:y> = 0", "<:x> = 0 && <:y> = 0"
		};
		io_context<node_t> ctx;
		auto i1 = std::make_shared<vector_input_stream>(i1_values);
		auto o3 = std::make_shared<vector_output_stream>();
		auto u  = std::make_shared<vector_output_stream>();
		ctx.add_input( "i1", tau_type_id<node_t>(), i1);
		ctx.add_output("o3", tau_type_id<node_t>(), o3);
		ctx.add_output("u",  tau_type_id<node_t>(), u);
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK( maybe_i.has_value() );
		DBG(TAU_LOG_TRACE << "o3 get values";)
		auto o3_values = o3->get_values();
		CHECK( o3_values == o3_expected );
		auto u_values = u->get_values();
		CHECK( u_values == u_expected );
	}

	TEST_CASE("u[t] = i1[t]: 2_clauses") {
		bdd_init<Bool>();
		auto spec = create_spec("u[t] = i1[t] && o2[t] = 0.");
		std::vector<std::string> i1_values = {
			"(always o2[-1] = 1) || (always o3[t] = 1)", "F", "F", "F"
		};
		std::vector<std::string> u_expected = {
			"(always o2[-1]:tau' = 0) || (always o3[t]:tau' = 0)", "F", "F", "F"
		};
		std::vector<std::string> o2_expected = {
			"F", "F", "F", "F",
		};
		std::vector<std::string> o3_expected = {
			"T", "T", "T"
		};
		io_context<node_t> ctx;
		auto i1 = std::make_shared<vector_input_stream>(i1_values);
		auto o2 = std::make_shared<vector_output_stream>();
		auto o3 = std::make_shared<vector_output_stream>();
		auto u  = std::make_shared<vector_output_stream>();
		ctx.add_input( "i1", tau_type_id<node_t>(), i1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		ctx.add_output("o3", tau_type_id<node_t>(), o3);
		ctx.add_output("u",  tau_type_id<node_t>(), u);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK( maybe_i.has_value() );
		auto o2_values = o2->get_values();
		CHECK( o2_values == o2_expected );
		auto o3_values = o3->get_values();
		CHECK( o3_values == o3_expected );
		auto u_values = u->get_values();
		CHECK( u_values == u_expected );
	}

	TEST_CASE("u[t] = i1[t]: history_unsat") {
		bdd_init<Bool>();
		auto spec = create_spec("u[t] = i1[t] && o1[t] = 0.");
		std::vector<std::string> i1_values = {
			"F", "o1[-1] = 1", "F", "F"
		};
		std::vector<std::string> u_expected = {
			"F", "always o1[-1]:tau' = 0", "F", "F"
		};
		std::vector<std::string> o1_expected = {
			"F", "F", "F", "F",
		};
		io_context<node_t> ctx;
		auto i1 = std::make_shared<vector_input_stream>(i1_values);
		auto o1 = std::make_shared<vector_output_stream>();
		auto u  = std::make_shared<vector_output_stream>();
		ctx.add_input( "i1", tau_type_id<node_t>(), i1);
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("u",  tau_type_id<node_t>(), u);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK( maybe_i.has_value() );
		auto o1_values = o1->get_values();
		CHECK( o1_values == o1_expected );
		auto u_values = u->get_values();
		CHECK( u_values == u_expected );
	}

	TEST_CASE("u[t] = i1[t]: spec_replace") {
		bdd_init<Bool>();
		auto spec = create_spec("u[t] = i1[t] && o1[t] = 0.");
		std::vector<std::string> i1_values = {
			"F", "o1[t] = 1", "F", "F"
		};
		std::vector<std::string> u_expected = {
			"F", "always o1[t]:tau' = 0", "F", "F"
		};
		std::vector<std::string> o1_expected = {
			"F", "F", "T", "T",
		};
		io_context<node_t> ctx;
		auto i1 = std::make_shared<vector_input_stream>(i1_values);
		auto o1 = std::make_shared<vector_output_stream>();
		auto u = std::make_shared<vector_output_stream>();
		ctx.add_input( "i1", tau_type_id<node_t>(), i1);
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("u",  tau_type_id<node_t>(), u);
		ctx.add_input("this", tau_type_id<node_t>(), std::make_shared<console_input_stream>());
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK( maybe_i.has_value() );
		auto o1_values = o1->get_values();
		CHECK( o1_values == o1_expected );
		auto u_values = u->get_values();
		CHECK( u_values == u_expected );
	}

	TEST_CASE("this_stream") {
		bdd_init<Bool>();
		auto spec = create_spec("u[t] = i1[t] && this[t] = o1[t].");
		std::vector<std::string> i1_values = {
			"o2[t] = 0", "F", "o3[t] = 0", "F"
		};
		std::vector<std::string> u_expected = {
			"always o2[t]:tau = 0", "F", "always o3[t]:tau = 0", "F"
		};
		std::vector<std::string> o1_expected = {
			"always i1[t]:tau = u[t]:tau && o1[t]:tau = this[t]:tau",
			"always i1[t]:tau = u[t]:tau && o2[t]:tau = 0 && o1[t]:tau = this[t]:tau",
			"always i1[t]:tau = u[t]:tau && o2[t]:tau = 0 && o1[t]:tau = this[t]:tau",
			"always i1[t]:tau = u[t]:tau && o2[t]:tau = 0 && o1[t]:tau = this[t]:tau && o3[t]:tau = 0"
		};
		io_context<node_t> ctx;
		auto i1 = std::make_shared<vector_input_stream>(i1_values);
		auto o1 = std::make_shared<vector_output_stream>();
		auto u  = std::make_shared<vector_output_stream>();
		ctx.add_input( "i1", tau_type_id<node_t>(), i1);
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("u",  tau_type_id<node_t>(), u);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK( maybe_i.has_value() );
		auto o1_values = o1->get_values();
		CHECK( o1_values == o1_expected );
		auto u_values = u->get_values();
		CHECK( u_values == u_expected );
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
		io_context<node_t> ctx;
		auto memory = run_test(sample, ctx, 2);
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
		auto memory = run_test(sample, 8);
		CHECK ( !memory.value().empty() );
	}

	// Fibonacci like sequence with sample Tau programs
	TEST_CASE("o1[0] = {o1[0] = 0.} && o1[1] = {o1[0] = 0.} && o1[t] = o1[t-1] ^ o1[t-2]") {
		const char* sample = "o1[0] =  {o1[0] = 0.} && o1[1] =  {o1[0] = 0.} && o1[t] = o1[t-1] ^ o1[t-2].";
		auto memory = run_test(sample, 8);
		CHECK ( !memory.value().empty() );
	}
}


TEST_SUITE("with inputs and outputs") {

	TEST_CASE("i1[t] = o1[t]") {
		const char* sample = "i1[t] = o1[t].";
		io_context<node_t> ctx;
		std::vector<std::string> i1_values = { "T", "F", "F" };
		ctx.add_input("i1", tau_type_id<node_t>(),
			std::make_shared<vector_input_stream>(i1_values));
		auto memory = run_test(sample, ctx, 3);
		CHECK ( !memory.value().empty() );
	}

	// In this case, we get an error as the output should be equal to the
	// input all the time, but the output is set to 0 at the beginning.
	//
	// Note that this specification is unsat because i1[0] = 0 is deduced
	// which is an assumption on an input stream
	TEST_CASE("i1[t] = o1[t] && o1[0] = 0") {
		const char* sample = "i1[t] = o1[t] && o1[0] = 0.";
		io_context<node_t> ctx;
		std::vector<std::string> i1_values = { "T", "T", "T" };
		ctx.add_input("i1", tau_type_id<node_t>(),
			std::make_shared<vector_input_stream>(i1_values));
		auto memory = run_test(sample, ctx, 3);
		CHECK ( (!memory.has_value() || memory.value().empty()) );
	}

	// In this case we get the sequence 0, 1, 1 as the output is set to 1
	// at the beginning.
	TEST_CASE("i1[t-1] = o1[t] && o1[0] = 0") {
		const char* sample = "i1[t-1] = o1[t] && o1[0] = 0.";
		io_context<node_t> ctx;
		std::vector<std::string> i1_values = { "T", "T", "T" };
		ctx.add_input("i1", tau_type_id<node_t>(),
			std::make_shared<vector_input_stream>(i1_values));
		auto memory = run_test(sample, ctx, 2);
		CHECK ( !memory.value().empty() );
	}

}

// -----------------------------------------------------------------------------
// obsoleted
// TODO: tests for streams - finputs and foutputs were replaced by streams in io_context

// TEST_SUITE("test inputs") {

// 	TEST_CASE("reading from file with sbf inputs") {
// 		bdd_init<Bool>();
// 		// io_context<node_t
// 		typed_io_vars input_map;
// 		tref var = build_var_name_indexed<node_t>(1);
// 		input_map[get_var_name_sid<node_t>(var)] = {
// 			get_typed_stream<node_t>(sbf_type<node_t>(), "../tests/integration/test_files/sbf-alternating_zeros_and_ones-length_10.in") };
// 		finputs<node_t> inputs(input_map);
// 		CHECK ( inputs.type_of(var) > 0 );
// 		for (size_t i = 0; i < 10; ++i) {
// 			auto in = inputs.read();
// 			if (in) {
// 				auto check = (i % 2)
// 					? tau::get(in.value()[var]).equals_1()
// 					: tau::get(in.value()[var]).equals_0();
// 				CHECK ( check );
// 			} else FAIL("no input");
// 		}
// 	}

// 	TEST_CASE("reading from file with tau program inputs") {
// 		bdd_init<Bool>();
// 		typed_io_vars input_map;
// 		tref var = build_var_name_indexed<node_t>(1);
// 		input_map[get_var_name_sid<node_t>(var)] = {
// 			get_typed_stream<node_t>(tau_type<node_t>(), "../tests/integration/test_files/tau-alternating_zeros_and_ones-length_10.in") };
// 		finputs<node_t> inputs(input_map);
// 		CHECK ( inputs.type_of(var) > 0 );
// 		for (size_t i = 0; i < 10; ++i) {
// 			auto in = inputs.read();
// 			if (in) {
// 				auto check = (i % 2)
// 					? tau::get(in.value()[var]).equals_1()
// 					: tau::get(in.value()[var]).equals_0();
// 				CHECK ( check );
// 			} else FAIL("no input");
// 		}
// 	}
// }

// TEST_SUITE("test outputs") {

// 	TEST_CASE("writing to file") {
// 		bdd_init<Bool>();
// 		typed_io_vars output_map;
// 		tref var = build_var_name_indexed<node_t>(1);
// 		size_t var_sid = get_var_name_sid<node_t>(var);
// 		size_t type = get_ba_type_id<node_t>(sbf_type<node_t>());
// 		tref var_0 = build_out_var_at_n_indexed<node_t>(1, 0, type);

// 		output_map[var_sid] = {
// 			get_typed_stream<node_t>(sbf_type<node_t>(), random_file()) };

// #ifdef DEBUG
// 		std::cout << "test_outputs/writing_to_file/output: " << output_map[var_sid].second << "\n";
// #endif // DEBUG

// 		foutputs<node_t> outputs(output_map);
// 		assignment<node_t> output = { { var_0, tau::_1(sbf_type_id<node_t>()) } };

// 		CHECK( outputs.type_of(var) > 0 );
// 		CHECK ( outputs.write(output) );
// 	}

// 	TEST_CASE("writing to files: two outputs") {
// 		bdd_init<Bool>();
// 		typed_io_vars output_map;
// 		size_t type = sbf_type_id<node_t>();
// 		tref var1 = build_var_name_indexed<node_t>(1);
// 		tref var2 = build_var_name_indexed<node_t>(2);
// 		size_t var1_sid = get_var_name_sid<node_t>(var1);
// 		size_t var2_sid = get_var_name_sid<node_t>(var2);
// 		tref var1_0 = build_out_var_at_n<node_t>(var1, 0, type);
// 		tref var2_0 = build_out_var_at_n<node_t>(var2, 0, type);
// 		output_map[var1_sid] = {
// 			get_typed_stream<node_t>(sbf_type<node_t>(), random_file()) };
// 		output_map[var2_sid] = {
// 			get_typed_stream<node_t>(sbf_type<node_t>(), random_file()) };

// #ifdef DEBUG
// 		std::cout << "test_outputs/writing_to_file/output: " << output_map[var1_sid].second << "\n";
// 		std::cout << "test_outputs/writing_to_file/output: " << output_map[var2_sid].second << "\n";
// #endif // DEBUG

// 		foutputs<node_t> outputs(output_map);
// 		assignment<node_t> output = {
// 			{ var1_0, tau::_1(type) },
// 			{ var2_0, tau::_0(type) }
// 		};

// 		CHECK( outputs.type_of(var1) > 0 );
// 		CHECK( outputs.type_of(var2) > 0 );
// 		CHECK ( outputs.write(output) );
// 	}

// 	TEST_CASE("writing to files: completing outputs") {
// 		bdd_init<Bool>();
// 		typed_io_vars output_map;
// 		size_t type = sbf_type_id<node_t>();
// 		auto var1 = build_var_name_indexed<node_t>(1);
// 		auto var2 = build_var_name_indexed<node_t>(2);
// 		size_t var1_sid = get_var_name_sid<node_t>(var1);
// 		size_t var2_sid = get_var_name_sid<node_t>(var2);
// 		auto var1_0 = build_out_var_at_n<node_t>(var1, 0, type);
// 		auto var2_1 = build_out_var_at_n<node_t>(var2, 1, type);
// 		output_map[var1_sid] = {
// 			get_typed_stream<node_t>(sbf_type<node_t>(), random_file()) };
// 		output_map[var2_sid] = {
// 			get_typed_stream<node_t>(sbf_type<node_t>(), random_file()) };

// #ifdef DEBUG
// 		std::cout << "test_outputs/writing_to_file/output: " << output_map[var1_sid].second << "\n";
// 		std::cout << "test_outputs/writing_to_file/output: " << output_map[var2_sid].second << "\n";
// #endif // DEBUG

// 		foutputs<node_t> outputs(output_map);
// 		assignment<node_t> output = {
// 			{ var1_0, tau::_1(type) },
// 			{ var2_1, tau::_1(type) }
// 		};

// 		CHECK( outputs.type_of(var1) > 0 );
// 		CHECK( outputs.type_of(var2) > 0 );
// 		CHECK ( outputs.write(output) );
// 	}
// }
