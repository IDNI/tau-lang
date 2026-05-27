// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_integration-interpreter_helper.h"

// ─────────────────────────────────────────────────────────────────────────────
// Group 1 – G with constant outputs (0 = "F", 1 = "T")
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (tau): G constant outputs") {

	TEST_CASE("G(o1[t]:tau = 0) – 4 steps produces all F") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = 0).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		strings expected = {"F", "F", "F", "F"};
		CHECK(o1->get_values() == expected);
	}

	TEST_CASE("G(o1[t]:tau = 1) – 4 steps produces all T") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = 1).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		strings expected = {"T", "T", "T", "T"};
		CHECK(o1->get_values() == expected);
	}

	TEST_CASE("G(o1[t]:tau = 0) – 6 steps produces all F") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = 0).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
		strings expected = {"F", "F", "F", "F", "F", "F"};
		CHECK(o1->get_values() == expected);
	}

	TEST_CASE("G(o1[t]:tau = 1) – 8 steps produces all T") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = 1).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 8);
		CHECK(maybe_i.has_value());
		strings expected = {"T", "T", "T", "T", "T", "T", "T", "T"};
		CHECK(o1->get_values() == expected);
	}

	TEST_CASE("G(o1[t]:tau = 0 && o2[t]:tau = 1) – 5 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = 0 && o2[t]:tau = 1).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		strings exp_o1 = {"F", "F", "F", "F", "F"};
		strings exp_o2 = {"T", "T", "T", "T", "T"};
		CHECK(o1->get_values() == exp_o1);
		CHECK(o2->get_values() == exp_o2);
	}

} // TEST_SUITE "LTL Execution (tau): G constant outputs"

// ─────────────────────────────────────────────────────────────────────────────
// Group 2 – G with input mirroring
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (tau): G input mirroring") {

	TEST_CASE("G(o1[t]:tau = i1[t]:tau) – 3 simple values") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = i1[t]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {"<:x> = 0", "<:y> = 0", "<:z> = 0"};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 3);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == i1_values);
	}

	TEST_CASE("G(o1[t]:tau = i1[t]:tau) – compound tau values") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = i1[t]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {"<:w> = 0", "<:x> = 0", "<:v> = 0", "<:y> = 0"};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == i1_values);
	}

	TEST_CASE("G(o1[t]:tau = i1[t]:tau) – 5 steps mixed values") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = i1[t]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {"<:x> = 0", "<:y> = 0", "<:z> = 0", "<:x> = 0", "<:y> = 0"};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == i1_values);
	}

	TEST_CASE("G(o1[t]:tau = i1[t]:tau) – 6 steps, repeating compound") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = i1[t]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:w> = 0",
			"<:x> = 0",
			"<:v> = 0",
			"<:z> = 0",
			"<:y> = 0",
			"<:u> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == i1_values);
	}

	TEST_CASE("G(o1[t]:tau = i1[t]:tau) – 8 steps, bottom/top sentinel values") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = i1[t]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:x> = 0", "<:y> = 0", "<:z> = 0",
			"<:x> = 0", "<:y> = 0", "<:z> = 0",
			"<:x> = 0", "<:y> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 8);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == i1_values);
	}

} // TEST_SUITE "LTL Execution (tau): G input mirroring"

// ─────────────────────────────────────────────────────────────────────────────
// Group 3 – G with two outputs
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (tau): G two outputs") {

	TEST_CASE("G(o1[t]:tau = i1[t]:tau && o2[t]:tau = 0) – 3 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = i1[t]:tau && o2[t]:tau = 0).");
		io_context<node_t> ctx;
		strings i1_values = {"<:x> = 0", "<:y> = 0", "<:z> = 0"};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 3);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == i1_values);
		strings exp_o2 = {"F", "F", "F"};
		CHECK(o2->get_values() == exp_o2);
	}

	TEST_CASE("G(o1[t]:tau = i1[t]:tau && o2[t]:tau = 1) – 4 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = i1[t]:tau && o2[t]:tau = 1).");
		io_context<node_t> ctx;
		strings i1_values = {"<:x> = 0", "<:y> = 0", "<:z> = 0", "<:w> = 0"};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == i1_values);
		strings exp_o2 = {"T", "T", "T", "T"};
		CHECK(o2->get_values() == exp_o2);
	}

	TEST_CASE("G(o1[t]:tau = 0 && o2[t]:tau = i1[t]:tau) – 4 steps mirror to o2") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = 0 && o2[t]:tau = i1[t]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {"<:x> = 0", "<:y> = 0", "<:z> = 0", "<:x> = 0"};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		strings exp_o1 = {"F", "F", "F", "F"};
		CHECK(o1->get_values() == exp_o1);
		CHECK(o2->get_values() == i1_values);
	}

	TEST_CASE("G(o1[t]:tau = i1[t]:tau && o2[t]:tau = i2[t]:tau) – 3 steps two inputs") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = i1[t]:tau && o2[t]:tau = i2[t]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {"<:x> = 0", "<:y> = 0", "<:z> = 0"};
		strings i2_values = {"<:w> = 0", "<:x> = 0", "<:y> = 0"};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 3);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == i1_values);
		CHECK(o2->get_values() == i2_values);
	}

	TEST_CASE("G(o1[t]:tau = 1 && o2[t]:tau = 0) – 5 steps constant pair") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = 1 && o2[t]:tau = 0).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		strings exp_o1 = {"T", "T", "T", "T", "T"};
		strings exp_o2 = {"F", "F", "F", "F", "F"};
		CHECK(o1->get_values() == exp_o1);
		CHECK(o2->get_values() == exp_o2);
	}

} // TEST_SUITE "LTL Execution (tau): G two outputs"

// ─────────────────────────────────────────────────────────────────────────────
// Group 4 – G with lookback inputs (t-1, t-2, t-3)
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (tau): G lookback inputs") {

	TEST_CASE("G(o1[t]:tau = i1[t-1]:tau) – 5 steps, o1[k] = i1[k-1]") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = i1[t-1]:tau).");
		io_context<node_t> ctx;
		// 5 input values; step 0 has no i1[-1] so output may be T or F
		strings i1_values = {"<:x> = 0", "<:y> = 0", "<:z> = 0", "<:x> = 0", "<:w> = 0"};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		auto o1_values = o1->get_values();
		CHECK(o1_values.size() == 5);
		// Steps 1..4: o1[k] must equal i1[k-1]
		for (size_t k = 1; k < 5; ++k) {
			CHECK(o1_values[k] == i1_values[k - 1]);
		}
	}

	TEST_CASE("G(o1[t]:tau = i1[t-2]:tau) – 6 steps, o1[k] = i1[k-2]") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = i1[t-2]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {"<:x> = 0", "<:y> = 0", "<:z> = 0", "<:x> = 0", "<:y> = 0", "<:w> = 0"};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
		auto o1_values = o1->get_values();
		CHECK(o1_values.size() == 6);
		// Steps 2..5: o1[k] must equal i1[k-2]
		for (size_t k = 2; k < 6; ++k) {
			CHECK(o1_values[k] == i1_values[k - 2]);
		}
	}

	TEST_CASE("G(o1[t]:tau = i1[t-3]:tau) – 7 steps, o1[k] = i1[k-3]") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = i1[t-3]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:x> = 0", "<:y> = 0", "<:z> = 0",
			"<:w> = 0", "<:x> = 0", "<:y> = 0", "<:z> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 7);
		CHECK(maybe_i.has_value());
		auto o1_values = o1->get_values();
		CHECK(o1_values.size() == 7);
		// Steps 3..6: o1[k] must equal i1[k-3]
		for (size_t k = 3; k < 7; ++k) {
			CHECK(o1_values[k] == i1_values[k - 3]);
		}
	}

	TEST_CASE("G(o1[t]:tau = i1[t-1]:tau) – 4 steps compound inputs") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = i1[t-1]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:w> = 0",
			"<:x> = 0",
			"<:v> = 0",
			"<:z> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		auto o1_values = o1->get_values();
		CHECK(o1_values.size() == 4);
		for (size_t k = 1; k < 4; ++k) {
			CHECK(o1_values[k] == i1_values[k - 1]);
		}
	}

	TEST_CASE("G(o1[t]:tau = i1[t-2]:tau && o2[t]:tau = i1[t-1]:tau) – 5 steps two lookbacks") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = i1[t-2]:tau && o2[t]:tau = i1[t-1]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:x> = 0", "<:y> = 0", "<:z> = 0", "<:w> = 0", "<:x> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		auto o1_values = o1->get_values();
		auto o2_values = o2->get_values();
		CHECK(o1_values.size() == 5);
		CHECK(o2_values.size() == 5);
		// o1[k] = i1[k-2] for k >= 2
		for (size_t k = 2; k < 5; ++k) {
			CHECK(o1_values[k] == i1_values[k - 2]);
		}
		// max_lookback=2: both outputs get 2 initial "F" steps regardless of individual shift.
		// o2[0..1] = "F", o2[k] = i1[k-1] for k >= 2
		CHECK(o2_values[0] == "F");
		CHECK(o2_values[1] == "F");
		for (size_t k = 2; k < 5; ++k) {
			CHECK(o2_values[k] == i1_values[k - 1]);
		}
	}

} // TEST_SUITE "LTL Execution (tau): G lookback inputs"

// ─────────────────────────────────────────────────────────────────────────────
// Group 5 – F formulas (eventually): just check has_value + size
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (tau): F formulas") {

	TEST_CASE("F(o1[t]:tau = 0) – 4 steps realizable") {
		bdd_init<Bool>();
		auto spec = create_spec("F(o1[t]:tau = 0).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 4);
	}

	TEST_CASE("F(o1[t]:tau = 1) – 4 steps realizable") {
		bdd_init<Bool>();
		auto spec = create_spec("F(o1[t]:tau = 1).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 4);
	}

	TEST_CASE("F(o1[t]:tau = i1[t]:tau) – 5 steps realizable") {
		bdd_init<Bool>();
		auto spec = create_spec("F(o1[t]:tau = i1[t]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {"<:x> = 0", "<:y> = 0", "<:z> = 0", "<:x> = 0", "<:y> = 0"};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 5);
	}

	TEST_CASE("F(o1[t]:tau = 0 && o2[t]:tau = 1) – 4 steps realizable") {
		bdd_init<Bool>();
		auto spec = create_spec("F(o1[t]:tau = 0 && o2[t]:tau = 1).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 4);
		CHECK(o2->get_values().size() == 4);
	}

	TEST_CASE("G(o1[t]:tau = i1[t]:tau) && F(o2[t]:tau = 0) – 5 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G(o1[t]:tau = i1[t]:tau) && F(o2[t]:tau = 0).");
		io_context<node_t> ctx;
		strings i1_values = {"<:x> = 0", "<:y> = 0", "<:z> = 0", "<:x> = 0", "<:y> = 0"};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		// o1 must mirror i1 exactly (G constraint is strict)
		CHECK(o1->get_values() == i1_values);
		// o2 eventually 0 — just verify size
		CHECK(o2->get_values().size() == 5);
	}

} // TEST_SUITE "LTL Execution (tau): F formulas"

// ─────────────────────────────────────────────────────────────────────────────
// Group 6 – U / R / W formulas: just check has_value + size
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (tau): U R W formulas") {

	TEST_CASE("(o1[t]:tau = 1) U (o1[t]:tau = 0) – 4 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("(o1[t]:tau = 1) U (o1[t]:tau = 0).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 4);
	}

	TEST_CASE("(o1[t]:tau = 1) U (o1[t]:tau = 0) – 5 steps (no input vars)") {
		bdd_init<Bool>();
		auto spec = create_spec("(o1[t]:tau = 1) U (o1[t]:tau = 0).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 5);
	}

	TEST_CASE("(o1[t]:tau = 0) R (o1[t]:tau = 1) – 4 steps (release)") {
		bdd_init<Bool>();
		auto spec = create_spec("(o1[t]:tau = 0) R (o1[t]:tau = 1).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 4);
	}

	TEST_CASE("(o1[t]:tau = 1) W (o1[t]:tau = 0) – 5 steps (weak until)") {
		bdd_init<Bool>();
		auto spec = create_spec("(o1[t]:tau = 1) W (o1[t]:tau = 0).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 5);
	}

	TEST_CASE("(o1[t]:tau = 0) W (o1[t]:tau = 1) – 5 steps weak until (no input vars)") {
		bdd_init<Bool>();
		auto spec = create_spec("(o1[t]:tau = 0) W (o1[t]:tau = 1).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 5);
	}

} // TEST_SUITE "LTL Execution (tau): U R W formulas"

// ─────────────────────────────────────────────────────────────────────────────
// Group 7 – sbf: G with constant outputs
// Uses nontrivial {}-expressions in constants.
// bdd_init<Bool>() is mandatory first in each test case for sbf.
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (sbf): G constant outputs") {

	// G(o1:sbf = {X & Y}:sbf) — output is always the compound conjunction X∧Y.
	TEST_CASE("G(o1:sbf = {X & Y}) constant output, 4 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = {X & Y}:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 4);
	}

	// G(o1:sbf = {X | Z}:sbf) — output is always the disjunction X∨Z.
	TEST_CASE("G(o1:sbf = {X | Z}) constant output, 5 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = {X | Z}:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 5);
	}

	// G(o1:sbf = {X | (Y & Z)}:sbf) — nontrivial three-variable compound.
	TEST_CASE("G(o1:sbf = {X | (Y & Z)}) constant output, 6 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = {X | (Y & Z)}:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 6);
	}

	// G(o1:sbf = {X & Y}:sbf) with 7-step run.
	TEST_CASE("G(o1:sbf = {X & Y}) constant output, 7 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = {X & Y}:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 7);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 7);
	}

	// G(o1:sbf = {X | (Y & Z)}:sbf) with 4-step minimal run.
	TEST_CASE("G(o1:sbf = {X | (Y & Z)}) constant output, 4 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = {X | (Y & Z)}:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 4);
	}

} // TEST_SUITE "LTL Execution (sbf): G constant outputs"

// ─────────────────────────────────────────────────────────────────────────────
// Group 8 – sbf: G with input mirroring
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (sbf): G input mirroring") {

	// G(o1:sbf = i1:sbf) — real input mirroring, 3 steps.
	TEST_CASE("G(o1:sbf = i1:sbf) mirroring, 3 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		strings i1_values = {"X & Y", "X | Z", "X | (Y & Z)"};
		ctx.add_input("i1", sbf_type_id<node_t>(),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = i1[t]:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 3);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 3);
	}

	// G(o1:sbf = i1:sbf) — real input mirroring, 4 steps with nontrivial expressions.
	TEST_CASE("G(o1:sbf = i1:sbf) mirroring, 4 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		strings i1_values = {"X & Y", "X | Z", "X | (Y & Z)", "X & Y"};
		ctx.add_input("i1", sbf_type_id<node_t>(),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = i1[t]:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 4);
	}

	// G(o1:sbf = i1:sbf) — real input mirroring, 5 steps with varied sbf expressions.
	TEST_CASE("G(o1:sbf = i1:sbf) mirroring, 5 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		strings i1_values = {"X & Y", "X | Z", "X | (Y & Z)", "X & Y", "X | Z"};
		ctx.add_input("i1", sbf_type_id<node_t>(),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = i1[t]:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 5);
	}

	// G(o1:sbf = i1:sbf') — complement mirroring: output is complement of input.
	TEST_CASE("G(o1:sbf = i1:sbf') complement mirroring, 4 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		strings i1_values = {"X & Y", "X | Z", "X | (Y & Z)", "X & Y"};
		ctx.add_input("i1", sbf_type_id<node_t>(),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = i1[t]:sbf').");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 4);
	}

	// G(o1:sbf | i1:sbf = 1) — union equals top: o1 covers the complement of i1.
	TEST_CASE("G(o1:sbf | i1:sbf = 1) union-top, 5 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		strings i1_values = {"X & Y", "X | Z", "X | (Y & Z)", "X & Y", "X | Z"};
		ctx.add_input("i1", sbf_type_id<node_t>(),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf | i1[t]:sbf = 1).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(!o1->get_values().empty());
	}

} // TEST_SUITE "LTL Execution (sbf): G input mirroring"

// ─────────────────────────────────────────────────────────────────────────────
// Group 9 – sbf: G with lookback
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (sbf): G with lookback") {

	// G(o1:sbf = i1[t-1]:sbf) — output mirrors input with 1-step delay, 4 steps.
	TEST_CASE("G(o1:sbf = i1[t-1]:sbf) lookback-1, 4 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		strings i1_values = {"X & Y", "X | Z", "X | (Y & Z)", "X & Y"};
		ctx.add_input("i1", sbf_type_id<node_t>(),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = i1[t-1]:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
	}

	// G(o1:sbf = i1[t-1]:sbf) — output mirrors input with 1-step delay, 5 steps.
	TEST_CASE("G(o1:sbf = i1[t-1]:sbf) lookback-1, 5 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		strings i1_values = {"X & Y", "X | Z", "X | (Y & Z)", "X & Y", "X | Z"};
		ctx.add_input("i1", sbf_type_id<node_t>(),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = i1[t-1]:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
	}

	// G(o1:sbf = i1[t-2]:sbf) — output mirrors input with 2-step delay, 5 steps.
	TEST_CASE("G(o1:sbf = i1[t-2]:sbf) lookback-2, 5 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		strings i1_values = {"X & Y", "X | Z", "X | (Y & Z)", "X & Y", "X | Z"};
		ctx.add_input("i1", sbf_type_id<node_t>(),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = i1[t-2]:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
	}

	// G(o1:sbf = i1[t-3]:sbf) — output mirrors input with 3-step delay, 6 steps.
	TEST_CASE("G(o1:sbf = i1[t-3]:sbf) lookback-3, 6 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		strings i1_values = {"X & Y", "X | Z", "X | (Y & Z)", "X & Y", "X | Z", "X | (Y & Z)"};
		ctx.add_input("i1", sbf_type_id<node_t>(),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = i1[t-3]:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
	}

	// G(o1:sbf = o1[t-1]:sbf) — output echoes its own previous value (no input needed).
	TEST_CASE("G(o1:sbf = o1[t-1]:sbf) self-lookback, 5 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = o1[t-1]:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
	}

} // TEST_SUITE "LTL Execution (sbf): G with lookback"

// ─────────────────────────────────────────────────────────────────────────────
// Group 10 – sbf: F and U/W operators
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (sbf): F and U W operators") {

	// F(o1:sbf = {X & Y}:sbf) — output equals compound constant eventually.
	TEST_CASE("F(o1:sbf = {X & Y}) eventually, 4 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "F (o1[t]:sbf = {X & Y}:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(!o1->get_values().empty());
	}

	// F(o1:sbf = {X | (Y & Z)}:sbf) — nontrivial three-variable constant, eventually.
	TEST_CASE("F(o1:sbf = {X | (Y & Z)}) eventually, 5 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "F (o1[t]:sbf = {X | (Y & Z)}:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(!o1->get_values().empty());
	}

	// (o1:sbf = {X & Y}) U (o1:sbf = {X | Z}) — until with sbf constants.
	TEST_CASE("({X & Y}:sbf) U ({X | Z}:sbf) until, 4 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx,
			"(o1[t]:sbf = {X & Y}:sbf) U (o1[t]:sbf = {X | Z}:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(!o1->get_values().empty());
	}

	// (o1:sbf = {X | (Y & Z)}) W (o1:sbf = {X & Y}) — weak until.
	TEST_CASE("({X|(Y&Z)}:sbf) W ({X&Y}:sbf) weak-until, 5 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx,
			"(o1[t]:sbf = {X | (Y & Z)}:sbf) W (o1[t]:sbf = {X & Y}:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(!o1->get_values().empty());
	}

	// G(F(o1:sbf = {X & Y}:sbf)) — nontrivial sbf constant, infinitely often.
	TEST_CASE("G(F(o1:sbf = {X & Y})) inf-often, 6 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (F (o1[t]:sbf = {X & Y}:sbf)).");
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
		CHECK(!o1->get_values().empty());
	}

} // TEST_SUITE "LTL Execution (sbf): F and U W operators"

// ─────────────────────────────────────────────────────────────────────────────
// Group 11 – bv: G with constant outputs
// Uses nontrivial {#b...} and decimal bitvector constants.
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (bv): G constant outputs") {

	// G(o1:bv[8] = {#b10110101}:bv[8]) — nontrivial 8-bit pattern, 4 steps.
	TEST_CASE("G(o1:bv[8] = {#b10110101}) constant output, 4 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "G (o1[t]:bv[8] = {#b10110101}:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 4);
	}

	// G(o1:bv[8] = {#b00001111}:bv[8]) — lower nibble, 5 steps.
	TEST_CASE("G(o1:bv[8] = {#b00001111}) constant output, 5 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "G (o1[t]:bv[8] = {#b00001111}:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 5);
	}

	// G(o1:bv[8] = {#b11110000}:bv[8]) — upper nibble, 6 steps.
	TEST_CASE("G(o1:bv[8] = {#b11110000}) constant output, 6 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "G (o1[t]:bv[8] = {#b11110000}:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 6);
	}

	// G(o1:bv[8] = {5}:bv[8]) — decimal constant 5, 5 steps.
	TEST_CASE("G(o1:bv[8] = {5}) decimal constant output, 5 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "G (o1[t]:bv[8] = {5}:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 5);
	}

	// G(o1:bv[8] = {255}:bv[8]) — all-ones byte, 7 steps.
	TEST_CASE("G(o1:bv[8] = {255}) all-ones byte constant output, 7 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "G (o1[t]:bv[8] = {255}:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 7);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 7);
	}

} // TEST_SUITE "LTL Execution (bv): G constant outputs"

// ─────────────────────────────────────────────────────────────────────────────
// Group 12 – bv: G with input mirroring, F, U, W
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (bv): G input mirroring and LTL operators") {

	// G(o1:bv[8] = i1:bv[8]) — real bv input mirroring, 3 steps.
	TEST_CASE("G(o1:bv[8] = i1:bv[8]) mirroring, 3 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		strings i1_values = {"5", "#b10110101", "255"};
		ctx.add_input("i1", bv_type_id<node_t>(8),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "G (o1[t]:bv[8] = i1[t]:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 3);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 3);
	}

	// G(o1:bv[8] = i1:bv[8]) — real bv input mirroring, 4 steps with nontrivial patterns.
	TEST_CASE("G(o1:bv[8] = i1:bv[8]) mirroring, 4 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		strings i1_values = {"#b00001111", "#b11110000", "#b10110101", "255"};
		ctx.add_input("i1", bv_type_id<node_t>(8),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "G (o1[t]:bv[8] = i1[t]:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 4);
	}

	// G(o1:bv[8] = i1:bv[8]) — real bv input mirroring, 5 steps.
	TEST_CASE("G(o1:bv[8] = i1:bv[8]) mirroring, 5 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		strings i1_values = {"5", "#b10110101", "255", "#b00001111", "#b11110000"};
		ctx.add_input("i1", bv_type_id<node_t>(8),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "G (o1[t]:bv[8] = i1[t]:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 5);
	}

	// G(o1:bv[8] = i1[t-1]:bv[8]) — output mirrors bv input with 1-step delay, 5 steps.
	TEST_CASE("G(o1:bv[8] = i1[t-1]:bv[8]) lookback-1 mirroring, 5 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		strings i1_values = {"5", "#b10110101", "255", "#b00001111", "#b11110000"};
		ctx.add_input("i1", bv_type_id<node_t>(8),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "G (o1[t]:bv[8] = i1[t-1]:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
	}

	// F(o1:bv[8] = {#b10110101}:bv[8]) — eventually outputs nontrivial bit pattern.
	TEST_CASE("F(o1:bv[8] = {#b10110101}) eventually, 4 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "F (o1[t]:bv[8] = {#b10110101}:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(!o1->get_values().empty());
	}

	// (o1:bv[8] = {#b00001111}) U (o1:bv[8] = {#b11110000}) — complementary nibbles until.
	TEST_CASE("({#b00001111}:bv[8]) U ({#b11110000}:bv[8]) until, 5 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx,
			"(o1[t]:bv[8] = {#b00001111}:bv[8]) U (o1[t]:bv[8] = {#b11110000}:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(!o1->get_values().empty());
	}

	// G(F(o1:bv[8] = {255}:bv[8])) — output equals 255 infinitely often, 6 steps.
	TEST_CASE("G(F(o1:bv[8] = {255})) inf-often constant output, 6 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "G (F (o1[t]:bv[8] = {255}:bv[8])).");
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
		CHECK(!o1->get_values().empty());
	}

	// (o1:bv[8] = {5}) W (o1:bv[8] = {#b10110101}) — decimal then binary weak-until, 4 steps.
	TEST_CASE("({5}:bv[8]) W ({#b10110101}:bv[8]) weak-until, 4 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx,
			"(o1[t]:bv[8] = {5}:bv[8]) W (o1[t]:bv[8] = {#b10110101}:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(!o1->get_values().empty());
	}

	// (o1:bv[8] = {#b00001111}) R (o1:bv[8] = {#b11110000}) — release, 5 steps.
	// p R q: q holds until (and including) when p holds; or q holds forever.
	// System can always output q (upper nibble), satisfying R vacuously.
	TEST_CASE("({#b00001111}:bv[8]) R ({#b11110000}:bv[8]) release, 5 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx,
			"(o1[t]:bv[8] = {#b00001111}:bv[8]) R (o1[t]:bv[8] = {#b11110000}:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(!o1->get_values().empty());
	}

	// G(F(o1:bv[8] = {#b10110101})) && G(F(o1:bv[8] != {#b10110101})) — alternating.
	// Requires a 2-state Mealy machine — multi-state encoding.
	TEST_CASE("G(F(o1:bv[8]={#b10110101})) && G(F(o1:bv[8]!={#b10110101})) alternating, 6 steps") {
		bdd_init<Bool>();
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx,
			"G (F (o1[t]:bv[8] = {#b10110101}:bv[8])) && "
			"G (F (!(o1[t]:bv[8] = {#b10110101}:bv[8]))).");
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
		CHECK(!o1->get_values().empty());
	}

} // TEST_SUITE "LTL Execution (bv): G input mirroring and LTL operators"



TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}