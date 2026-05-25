// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_integration-interpreter_helper.h"

// ── Group 1: Two inputs, two outputs (tau type, 6-10 steps) ──────────────────

TEST_SUITE("LTL Execution (multi-io):") {

	// Test 1: Dual mirror — both outputs copy their respective inputs exactly.
	TEST_CASE("G(o1=i1 && o2=i2): dual mirror, 8 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i1[t]:tau && o2[t]:tau = i2[t]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0",
			"<:e> = 0", "<:f> = 0", "<:g> = 0", "<:h> = 0"
		};
		strings i2_values = {
			"<:p> = 0", "<:q> = 0", "<:r> = 0", "<:s> = 0",
			"<:t> = 0", "<:u> = 0", "<:v> = 0", "<:w> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 8);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == i1_values);
		CHECK(o2->get_values() == i2_values);
	}

	// Test 2: Swapped mirror — o1 copies i2, o2 copies i1.
	TEST_CASE("G(o1=i2 && o2=i1): swapped mirror, 7 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i2[t]:tau && o2[t]:tau = i1[t]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0",
			"<:e> = 0", "<:f> = 0", "<:g> = 0"
		};
		strings i2_values = {
			"<:p> = 0", "<:q> = 0", "<:r> = 0", "<:s> = 0",
			"<:t> = 0", "<:u> = 0", "<:v> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 7);
		CHECK(maybe_i.has_value());
		// swapped: o1 should receive i2's values, o2 should receive i1's values
		CHECK(o1->get_values() == i2_values);
		CHECK(o2->get_values() == i1_values);
	}

	// Test 3: o1 mirrors i1, o2 is constant 0 (bottom/false).
	TEST_CASE("G(o1=i1 && o2=0): one mirror one constant false, 6 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i1[t]:tau && o2[t]:tau = 0).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0",
			"<:d> = 0", "<:e> = 0", "<:f> = 0"
		};
		strings i2_values = {
			"<:x> = 0", "<:y> = 0", "<:z> = 0",
			"<:w> = 0", "<:v> = 0", "<:u> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == i1_values);
		// o2 is always 0 → tau bottom → "F"
		auto o2_vals = o2->get_values();
		CHECK(o2_vals.size() == 6);
		for (const auto& v : o2_vals) CHECK(v == "F");
	}

	// Test 4: o1 mirrors i1, o2 is constant 1 (top/true).
	TEST_CASE("G(o1=i1 && o2=1): one mirror one constant true, 6 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i1[t]:tau && o2[t]:tau = 1).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0",
			"<:d> = 0", "<:e> = 0", "<:f> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == i1_values);
		// o2 is always 1 → tau top → "T"
		auto o2_vals = o2->get_values();
		CHECK(o2_vals.size() == 6);
		for (const auto& v : o2_vals) CHECK(v == "T");
	}

	// Test 5: Single G with both mirrors — equivalent to G(o1=i1) && G(o2=i2).
	TEST_CASE("G(o1=i1 && o2=i2): two mirrors in one G, 9 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i1[t]:tau && o2[t]:tau = i2[t]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0", "<:e> = 0",
			"<:f> = 0", "<:g> = 0", "<:h> = 0", "<:i> = 0"
		};
		strings i2_values = {
			"<:p> = 0", "<:q> = 0", "<:r> = 0", "<:s> = 0", "<:t> = 0",
			"<:u> = 0", "<:v> = 0", "<:w> = 0", "<:x> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 9);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == i1_values);
		CHECK(o2->get_values() == i2_values);
	}

	// Test 6: Both outputs are constant zero.
	TEST_CASE("G(o1=0 && o2=0): both outputs constant false, 7 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = 0 && o2[t]:tau = 0).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0",
			"<:e> = 0", "<:f> = 0", "<:g> = 0"
		};
		strings i2_values = {
			"<:p> = 0", "<:q> = 0", "<:r> = 0", "<:s> = 0",
			"<:t> = 0", "<:u> = 0", "<:v> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 7);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		auto o2_vals = o2->get_values();
		CHECK(o1_vals.size() == 7);
		CHECK(o2_vals.size() == 7);
		for (const auto& v : o1_vals) CHECK(v == "F");
		for (const auto& v : o2_vals) CHECK(v == "F");
	}

	// Test 7: Both outputs are constant one.
	TEST_CASE("G(o1=1 && o2=1): both outputs constant true, 6 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = 1 && o2[t]:tau = 1).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0",
			"<:d> = 0", "<:e> = 0", "<:f> = 0"
		};
		strings i2_values = {
			"<:x> = 0", "<:y> = 0", "<:z> = 0",
			"<:w> = 0", "<:v> = 0", "<:u> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		auto o2_vals = o2->get_values();
		CHECK(o1_vals.size() == 6);
		CHECK(o2_vals.size() == 6);
		for (const auto& v : o1_vals) CHECK(v == "T");
		for (const auto& v : o2_vals) CHECK(v == "T");
	}

	// Test 8: o1 mirrors i2, o2 is constant false.
	TEST_CASE("G(o1=i2 && o2=0): o1 copies i2, o2 constant false, 8 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i2[t]:tau && o2[t]:tau = 0).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0",
			"<:e> = 0", "<:f> = 0", "<:g> = 0", "<:h> = 0"
		};
		strings i2_values = {
			"<:p> = 0", "<:q> = 0", "<:r> = 0", "<:s> = 0",
			"<:t> = 0", "<:u> = 0", "<:v> = 0", "<:w> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 8);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == i2_values);
		auto o2_vals = o2->get_values();
		CHECK(o2_vals.size() == 8);
		for (const auto& v : o2_vals) CHECK(v == "F");
	}

	// Test 9: o1 copies i2, o2 copies i1 — single G.
	TEST_CASE("G(o1=i2 && o2=i1): swapped mirrors in one G, 10 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i2[t]:tau && o2[t]:tau = i1[t]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0", "<:e> = 0",
			"<:f> = 0", "<:g> = 0", "<:h> = 0", "<:i> = 0", "<:j> = 0"
		};
		strings i2_values = {
			"<:p> = 0", "<:q> = 0", "<:r> = 0", "<:s> = 0", "<:t> = 0",
			"<:u> = 0", "<:v> = 0", "<:w> = 0", "<:x> = 0", "<:y> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 10);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == i2_values);
		CHECK(o2->get_values() == i1_values);
	}

	// Test 10: o1 mirrors i1, o2 constant true — single G.
	TEST_CASE("G(o1=i1 && o2=1): mirror and constant true in one G, 8 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i1[t]:tau && o2[t]:tau = 1).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0",
			"<:e> = 0", "<:f> = 0", "<:g> = 0", "<:h> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 8);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == i1_values);
		auto o2_vals = o2->get_values();
		CHECK(o2_vals.size() == 8);
		for (const auto& v : o2_vals) CHECK(v == "T");
	}
}

// ── Group 2: Lookback t-1, t-2, t-3 (tau type, 6-8 steps) ───────────────────

TEST_SUITE("LTL Execution (lookback):") {

	// Test 11: One-step delay — o1[t] = i1[t-1].
	// At t=0: i1[-1] is undefined → output is "T" (tau top / undefined past).
	// At t=k (k>=1): o1[k] = i1[k-1].
	TEST_CASE("G(o1[t]=i1[t-1]): 1-step delay, 7 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i1[t-1]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0",
			"<:e> = 0", "<:f> = 0", "<:g> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 7);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		CHECK(o1_vals.size() == 7);
		// step 0: undefined past → "F" (tau bottom)
		std::vector<strings> expected = {
			{ "F" },
			{ "<:a> = 0" },
			{ "<:b> = 0" },
			{ "<:c> = 0" },
			{ "<:d> = 0" },
			{ "<:e> = 0" },
			{ "<:f> = 0" }
		};
		CHECK(values_matches_any_of(o1_vals, expected));
	}

	// Test 12: Two-step delay — o1[t] = i1[t-2].
	// At t=0,1: undefined past → "T".
	// At t=k (k>=2): o1[k] = i1[k-2].
	TEST_CASE("G(o1[t]=i1[t-2]): 2-step delay, 8 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i1[t-2]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0",
			"<:e> = 0", "<:f> = 0", "<:g> = 0", "<:h> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 8);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		CHECK(o1_vals.size() == 8);
		// steps 0-1: undefined → "F"; steps 2..7: shifted i1 values
		std::vector<strings> expected = {
			{ "F" },
			{ "F" },
			{ "<:a> = 0" },
			{ "<:b> = 0" },
			{ "<:c> = 0" },
			{ "<:d> = 0" },
			{ "<:e> = 0" },
			{ "<:f> = 0" }
		};
		CHECK(values_matches_any_of(o1_vals, expected));
	}

	// Test 13: Three-step delay — o1[t] = i1[t-3].
	// At t=0,1,2: undefined → "T". At t>=3: shifted.
	TEST_CASE("G(o1[t]=i1[t-3]): 3-step delay, 8 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i1[t-3]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0",
			"<:e> = 0", "<:f> = 0", "<:g> = 0", "<:h> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 8);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		CHECK(o1_vals.size() == 8);
		// steps 0-2: "F"; steps 3..7: i1[0..4]
		std::vector<strings> expected = {
			{ "F" },
			{ "F" },
			{ "F" },
			{ "<:a> = 0" },
			{ "<:b> = 0" },
			{ "<:c> = 0" },
			{ "<:d> = 0" },
			{ "<:e> = 0" }
		};
		CHECK(values_matches_any_of(o1_vals, expected));
	}

	// Test 14: Crossed lookback — o1 gets delayed i2, o2 gets delayed i1.
	TEST_CASE("G(o1[t]=i2[t-1] && o2[t]=i1[t-1]): crossed 1-step delay, 7 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i2[t-1]:tau && o2[t]:tau = i1[t-1]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0",
			"<:e> = 0", "<:f> = 0", "<:g> = 0"
		};
		strings i2_values = {
			"<:p> = 0", "<:q> = 0", "<:r> = 0", "<:s> = 0",
			"<:t> = 0", "<:u> = 0", "<:v> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 7);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		auto o2_vals = o2->get_values();
		CHECK(o1_vals.size() == 7);
		CHECK(o2_vals.size() == 7);
		// o1 = delayed i2: step 0 → "F", then i2[0..5]
		std::vector<strings> o1_expected = {
			{ "F" },
			{ "<:p> = 0" }, { "<:q> = 0" }, { "<:r> = 0" },
			{ "<:s> = 0" }, { "<:t> = 0" }, { "<:u> = 0" }
		};
		// o2 = delayed i1: step 0 → "F", then i1[0..5]
		std::vector<strings> o2_expected = {
			{ "F" },
			{ "<:a> = 0" }, { "<:b> = 0" }, { "<:c> = 0" },
			{ "<:d> = 0" }, { "<:e> = 0" }, { "<:f> = 0" }
		};
		CHECK(values_matches_any_of(o1_vals, o1_expected));
		CHECK(values_matches_any_of(o2_vals, o2_expected));
	}

	// Test 15: o1 = i1[t-1], o2 = i2[t-2] — different delays per output.
	TEST_CASE("G(o1[t]=i1[t-1] && o2[t]=i2[t-2]): mixed delays, 8 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i1[t-1]:tau && o2[t]:tau = i2[t-2]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0",
			"<:e> = 0", "<:f> = 0", "<:g> = 0", "<:h> = 0"
		};
		strings i2_values = {
			"<:p> = 0", "<:q> = 0", "<:r> = 0", "<:s> = 0",
			"<:t> = 0", "<:u> = 0", "<:v> = 0", "<:w> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 8);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		auto o2_vals = o2->get_values();
		CHECK(o1_vals.size() == 8);
		CHECK(o2_vals.size() == 8);
		// o1 = i1[t-1] with max_lookback=2: steps 0-1 → "F", then i1[0..5]
		std::vector<strings> o1_expected = {
			{ "F" }, { "F" },
			{ "<:a> = 0" }, { "<:b> = 0" }, { "<:c> = 0" }, { "<:d> = 0" },
			{ "<:e> = 0" }, { "<:f> = 0" }
		};
		// o2 = i2[t-2]: steps 0-1 → "F", then i2[0..5]
		std::vector<strings> o2_expected = {
			{ "F" }, { "F" },
			{ "<:p> = 0" }, { "<:q> = 0" }, { "<:r> = 0" }, { "<:s> = 0" },
			{ "<:t> = 0" }, { "<:u> = 0" }
		};
		CHECK(values_matches_any_of(o1_vals, o1_expected));
		CHECK(values_matches_any_of(o2_vals, o2_expected));
	}

	// Test 16: o1 = i1[t-2], o2 = i2[t-1] — mixed delays reversed.
	TEST_CASE("G(o1[t]=i1[t-2] && o2[t]=i2[t-1]): reversed mixed delays, 7 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i1[t-2]:tau && o2[t]:tau = i2[t-1]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0",
			"<:e> = 0", "<:f> = 0", "<:g> = 0"
		};
		strings i2_values = {
			"<:p> = 0", "<:q> = 0", "<:r> = 0", "<:s> = 0",
			"<:t> = 0", "<:u> = 0", "<:v> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 7);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		auto o2_vals = o2->get_values();
		CHECK(o1_vals.size() == 7);
		CHECK(o2_vals.size() == 7);
		// o1 = i1[t-2] with max_lookback=2: steps 0-1 → "F", then i1[0..4]
		std::vector<strings> o1_expected = {
			{ "F" }, { "F" },
			{ "<:a> = 0" }, { "<:b> = 0" }, { "<:c> = 0" },
			{ "<:d> = 0" }, { "<:e> = 0" }
		};
		// o2 = i2[t-1] with max_lookback=2: steps 0-1 → "F", then i2[0..4]
		std::vector<strings> o2_expected = {
			{ "F" }, { "F" },
			{ "<:p> = 0" }, { "<:q> = 0" }, { "<:r> = 0" },
			{ "<:s> = 0" }, { "<:t> = 0" }
		};
		CHECK(values_matches_any_of(o1_vals, o1_expected));
		CHECK(values_matches_any_of(o2_vals, o2_expected));
	}

	// Test 17: Single output with 3-step delay, smaller step count.
	TEST_CASE("G(o1[t]=i1[t-3]): 3-step delay, 6 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i1[t-3]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0",
			"<:d> = 0", "<:e> = 0", "<:f> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		CHECK(o1_vals.size() == 6);
		// steps 0-2: "F"; steps 3..5: i1[0..2]
		std::vector<strings> expected = {
			{ "F" }, { "F" }, { "F" },
			{ "<:a> = 0" }, { "<:b> = 0" }, { "<:c> = 0" }
		};
		CHECK(values_matches_any_of(o1_vals, expected));
	}

	// Test 18: Crossed 2-step delay — o1 gets i2 delayed 2, o2 gets i1 delayed 2.
	TEST_CASE("G(o1[t]=i2[t-2] && o2[t]=i1[t-2]): crossed 2-step delay, 8 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i2[t-2]:tau && o2[t]:tau = i1[t-2]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0",
			"<:e> = 0", "<:f> = 0", "<:g> = 0", "<:h> = 0"
		};
		strings i2_values = {
			"<:p> = 0", "<:q> = 0", "<:r> = 0", "<:s> = 0",
			"<:t> = 0", "<:u> = 0", "<:v> = 0", "<:w> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 8);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		auto o2_vals = o2->get_values();
		CHECK(o1_vals.size() == 8);
		CHECK(o2_vals.size() == 8);
		// o1 = i2[t-2]: steps 0-1 → "F", then i2[0..5]
		std::vector<strings> o1_expected = {
			{ "F" }, { "F" },
			{ "<:p> = 0" }, { "<:q> = 0" }, { "<:r> = 0" },
			{ "<:s> = 0" }, { "<:t> = 0" }, { "<:u> = 0" }
		};
		// o2 = i1[t-2]: steps 0-1 → "F", then i1[0..5]
		std::vector<strings> o2_expected = {
			{ "F" }, { "F" },
			{ "<:a> = 0" }, { "<:b> = 0" }, { "<:c> = 0" },
			{ "<:d> = 0" }, { "<:e> = 0" }, { "<:f> = 0" }
		};
		CHECK(values_matches_any_of(o1_vals, o1_expected));
		CHECK(values_matches_any_of(o2_vals, o2_expected));
	}

	// Test 19: o1 = i1[t-1] via separate G conjunct, 6 steps.
	TEST_CASE("G(o1[t]=i1[t-1]) via G conjunct: 1-step delay, 6 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i1[t-1]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:x> = 0", "<:y> = 0", "<:z> = 0",
			"<:w> = 0", "<:v> = 0", "<:u> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		CHECK(o1_vals.size() == 6);
		// step 0: "F"; steps 1-5: i1[0..4]
		std::vector<strings> expected = {
			{ "F" },
			{ "<:x> = 0" }, { "<:y> = 0" }, { "<:z> = 0" },
			{ "<:w> = 0" }, { "<:v> = 0" }
		};
		CHECK(values_matches_any_of(o1_vals, expected));
	}

	// Test 20: o1 = i2[t-3], 8 steps — cross-variable 3-step delay.
	TEST_CASE("G(o1[t]=i2[t-3]): cross-variable 3-step delay, 8 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i2[t-3]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0",
			"<:e> = 0", "<:f> = 0", "<:g> = 0", "<:h> = 0"
		};
		strings i2_values = {
			"<:p> = 0", "<:q> = 0", "<:r> = 0", "<:s> = 0",
			"<:t> = 0", "<:u> = 0", "<:v> = 0", "<:w> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 8);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		CHECK(o1_vals.size() == 8);
		// steps 0-2: "F"; steps 3..7: i2[0..4]
		std::vector<strings> expected = {
			{ "F" }, { "F" }, { "F" },
			{ "<:p> = 0" }, { "<:q> = 0" }, { "<:r> = 0" },
			{ "<:s> = 0" }, { "<:t> = 0" }
		};
		CHECK(values_matches_any_of(o1_vals, expected));
	}
}

// ── Group 3: Combined lookback + constants (tau type, 6-10 steps) ─────────────

TEST_SUITE("LTL Execution (combined):") {

	// Test 21: o1 = i1[t-1], o2 = constant 0.
	TEST_CASE("G(o1[t]=i1[t-1] && o2[t]=0): delay + constant false, 7 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i1[t-1]:tau && o2[t]:tau = 0).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0",
			"<:e> = 0", "<:f> = 0", "<:g> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 7);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		auto o2_vals = o2->get_values();
		CHECK(o1_vals.size() == 7);
		CHECK(o2_vals.size() == 7);
		// o1: step 0 → "F"; steps 1..6 → i1[0..5]
		std::vector<strings> o1_expected = {
			{ "F" },
			{ "<:a> = 0" }, { "<:b> = 0" }, { "<:c> = 0" },
			{ "<:d> = 0" }, { "<:e> = 0" }, { "<:f> = 0" }
		};
		CHECK(values_matches_any_of(o1_vals, o1_expected));
		for (const auto& v : o2_vals) CHECK(v == "F");
	}

	// Test 22: o1 = i1[t-1], o2 = constant 1.
	TEST_CASE("G(o1[t]=i1[t-1] && o2[t]=1): delay + constant true, 7 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i1[t-1]:tau && o2[t]:tau = 1).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0",
			"<:e> = 0", "<:f> = 0", "<:g> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 7);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		auto o2_vals = o2->get_values();
		CHECK(o1_vals.size() == 7);
		CHECK(o2_vals.size() == 7);
		// o1: step 0 → "F"; steps 1..6 → i1[0..5]
		std::vector<strings> o1_expected = {
			{ "F" },
			{ "<:a> = 0" }, { "<:b> = 0" }, { "<:c> = 0" },
			{ "<:d> = 0" }, { "<:e> = 0" }, { "<:f> = 0" }
		};
		CHECK(values_matches_any_of(o1_vals, o1_expected));
		// o2 = constant 1, but step 0 is unconstrained → "F"; rest "T"
		CHECK(o2_vals.size() == 7);
		CHECK(o2_vals[0] == "F");
		for (size_t i = 1; i < o2_vals.size(); ++i) CHECK(o2_vals[i] == "T");
	}

	// Test 23: o1 = i1[t] (current), o2 = i2[t-2] (2-step delay).
	TEST_CASE("G(o1[t]=i1[t] && o2[t]=i2[t-2]): current + 2-step delay, 8 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i1[t]:tau && o2[t]:tau = i2[t-2]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0",
			"<:e> = 0", "<:f> = 0", "<:g> = 0", "<:h> = 0"
		};
		strings i2_values = {
			"<:p> = 0", "<:q> = 0", "<:r> = 0", "<:s> = 0",
			"<:t> = 0", "<:u> = 0", "<:v> = 0", "<:w> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 8);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		auto o2_vals = o2->get_values();
		CHECK(o1_vals.size() == 8);
		CHECK(o2_vals.size() == 8);
		// o1 = i1[t] with global lookback=2: F at steps 0-1, then i1[0..5]
		std::vector<strings> o1_expected = {
			{ "F" }, { "F" },
			{ "<:a> = 0" }, { "<:b> = 0" }, { "<:c> = 0" }, { "<:d> = 0" },
			{ "<:e> = 0" }, { "<:f> = 0" }
		};
		CHECK(values_matches_any_of(o1_vals, o1_expected));
		// o2 = i2[t-2]: steps 0-1 → "F", then i2[0..5]
		std::vector<strings> o2_expected = {
			{ "F" }, { "F" },
			{ "<:p> = 0" }, { "<:q> = 0" }, { "<:r> = 0" },
			{ "<:s> = 0" }, { "<:t> = 0" }, { "<:u> = 0" }
		};
		CHECK(values_matches_any_of(o2_vals, o2_expected));
	}

	// Test 24: Both outputs constant — o1=0, o2=1.
	TEST_CASE("G(o1[t]=0 && o2[t]=1): both constant (false/true), 10 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = 0 && o2[t]:tau = 1).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0", "<:e> = 0",
			"<:f> = 0", "<:g> = 0", "<:h> = 0", "<:i> = 0", "<:j> = 0"
		};
		strings i2_values = {
			"<:p> = 0", "<:q> = 0", "<:r> = 0", "<:s> = 0", "<:t> = 0",
			"<:u> = 0", "<:v> = 0", "<:w> = 0", "<:x> = 0", "<:y> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 10);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		auto o2_vals = o2->get_values();
		CHECK(o1_vals.size() == 10);
		CHECK(o2_vals.size() == 10);
		for (const auto& v : o1_vals) CHECK(v == "F");
		for (const auto& v : o2_vals) CHECK(v == "T");
	}

	// Test 25: o1 = i1[t] (current), o2 = i1[t-1] (one step behind — same input).
	TEST_CASE("G(o1[t]=i1[t] && o2[t]=i1[t-1]): current and 1-step delayed same input, 8 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i1[t]:tau && o2[t]:tau = i1[t-1]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0",
			"<:e> = 0", "<:f> = 0", "<:g> = 0", "<:h> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 8);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		auto o2_vals = o2->get_values();
		CHECK(o1_vals.size() == 8);
		CHECK(o2_vals.size() == 8);
		// o1 = i1[t] with global lookback=1: F at step 0, then i1[1..7]
		std::vector<strings> o1_expected = {
			{ "F" },
			{ "<:b> = 0" }, { "<:c> = 0" }, { "<:d> = 0" }, { "<:e> = 0" },
			{ "<:f> = 0" }, { "<:g> = 0" }, { "<:h> = 0" }
		};
		CHECK(values_matches_any_of(o1_vals, o1_expected));
		// o2 = i1[t-1]: step 0 → "F"; steps 1..7 → i1[0..6]
		std::vector<strings> o2_expected = {
			{ "F" },
			{ "<:a> = 0" }, { "<:b> = 0" }, { "<:c> = 0" }, { "<:d> = 0" },
			{ "<:e> = 0" }, { "<:f> = 0" }, { "<:g> = 0" }
		};
		CHECK(values_matches_any_of(o2_vals, o2_expected));
	}

	// Test 26: o1 = i1[t-2], o2 = constant 1.
	TEST_CASE("G(o1[t]=i1[t-2] && o2[t]=1): 2-step delay + constant true, 8 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i1[t-2]:tau && o2[t]:tau = 1).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0",
			"<:e> = 0", "<:f> = 0", "<:g> = 0", "<:h> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 8);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		auto o2_vals = o2->get_values();
		CHECK(o1_vals.size() == 8);
		CHECK(o2_vals.size() == 8);
		// o1 = i1[t-2]: steps 0-1 → "F"; steps 2..7 → i1[0..5]
		std::vector<strings> o1_expected = {
			{ "F" }, { "F" },
			{ "<:a> = 0" }, { "<:b> = 0" }, { "<:c> = 0" },
			{ "<:d> = 0" }, { "<:e> = 0" }, { "<:f> = 0" }
		};
		CHECK(values_matches_any_of(o1_vals, o1_expected));
		// o2 = constant 1, but steps 0-1 unconstrained → "F"; rest "T"
		CHECK(o2_vals.size() == 8);
		for (size_t i = 0; i < 2; ++i) CHECK(o2_vals[i] == "F");
		for (size_t i = 2; i < o2_vals.size(); ++i) CHECK(o2_vals[i] == "T");
	}

	// Test 27: o1 = i2[t-1], o2 = i1[t] — mix of delay and current across inputs.
	TEST_CASE("G(o1[t]=i2[t-1] && o2[t]=i1[t]): cross-delayed + current, 7 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i2[t-1]:tau && o2[t]:tau = i1[t]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0",
			"<:e> = 0", "<:f> = 0", "<:g> = 0"
		};
		strings i2_values = {
			"<:p> = 0", "<:q> = 0", "<:r> = 0", "<:s> = 0",
			"<:t> = 0", "<:u> = 0", "<:v> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 7);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		auto o2_vals = o2->get_values();
		CHECK(o1_vals.size() == 7);
		CHECK(o2_vals.size() == 7);
		// o1 = i2[t-1]: step 0 → "F"; steps 1..6 → i2[0..5]
		std::vector<strings> o1_expected = {
			{ "F" },
			{ "<:p> = 0" }, { "<:q> = 0" }, { "<:r> = 0" },
			{ "<:s> = 0" }, { "<:t> = 0" }, { "<:u> = 0" }
		};
		// o2 = i1[t] with global lookback=1: F at step 0, then i1[0..5]
		std::vector<strings> o2_expected = {
			{ "F" },
			{ "<:a> = 0" }, { "<:b> = 0" }, { "<:c> = 0" },
			{ "<:d> = 0" }, { "<:e> = 0" }, { "<:f> = 0" }
		};
		CHECK(values_matches_any_of(o1_vals, o1_expected));
		CHECK(values_matches_any_of(o2_vals, o2_expected));
	}

	// Test 28: o1 = i1[t-3], o2 = i2[t] — 3-step delay on one, current on other.
	TEST_CASE("G(o1[t]=i1[t-3] && o2[t]=i2[t]): 3-step delay + current, 9 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i1[t-3]:tau && o2[t]:tau = i2[t]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0", "<:e> = 0",
			"<:f> = 0", "<:g> = 0", "<:h> = 0", "<:i> = 0"
		};
		strings i2_values = {
			"<:p> = 0", "<:q> = 0", "<:r> = 0", "<:s> = 0", "<:t> = 0",
			"<:u> = 0", "<:v> = 0", "<:w> = 0", "<:x> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 9);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		auto o2_vals = o2->get_values();
		CHECK(o1_vals.size() == 9);
		CHECK(o2_vals.size() == 9);
		// o1 = i1[t-3]: steps 0-2 → "F"; steps 3..8 → i1[0..5]
		std::vector<strings> o1_expected = {
			{ "F" }, { "F" }, { "F" },
			{ "<:a> = 0" }, { "<:b> = 0" }, { "<:c> = 0" },
			{ "<:d> = 0" }, { "<:e> = 0" }, { "<:f> = 0" }
		};
		CHECK(values_matches_any_of(o1_vals, o1_expected));
		// o2 = i2[t] with global lookback=3: F at steps 0-2, then i2[0..5]
		std::vector<strings> o2_expected = {
			{ "F" }, { "F" }, { "F" },
			{ "<:p> = 0" }, { "<:q> = 0" }, { "<:r> = 0" },
			{ "<:s> = 0" }, { "<:t> = 0" }, { "<:u> = 0" }
		};
		CHECK(values_matches_any_of(o2_vals, o2_expected));
	}

	// Test 29: o1 = i1[t-1], o2 = i2[t-2] — both delayed, different amounts.
	TEST_CASE("G(o1[t]=i1[t-1] && o2[t]=i2[t-2]): both delayed, 9 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i1[t-1]:tau && o2[t]:tau = i2[t-2]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0", "<:e> = 0",
			"<:f> = 0", "<:g> = 0", "<:h> = 0", "<:i> = 0"
		};
		strings i2_values = {
			"<:p> = 0", "<:q> = 0", "<:r> = 0", "<:s> = 0", "<:t> = 0",
			"<:u> = 0", "<:v> = 0", "<:w> = 0", "<:x> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 9);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		auto o2_vals = o2->get_values();
		CHECK(o1_vals.size() == 9);
		CHECK(o2_vals.size() == 9);
		// o1 = i1[t-1] with max_lookback=2: steps 0-1 → "F"; steps 2..8 → i1[0..6]
		std::vector<strings> o1_expected = {
			{ "F" }, { "F" },
			{ "<:a> = 0" }, { "<:b> = 0" }, { "<:c> = 0" }, { "<:d> = 0" },
			{ "<:e> = 0" }, { "<:f> = 0" }, { "<:g> = 0" }
		};
		// o2 = i2[t-2]: steps 0-1 → "F"; steps 2..8 → i2[0..6]
		std::vector<strings> o2_expected = {
			{ "F" }, { "F" },
			{ "<:p> = 0" }, { "<:q> = 0" }, { "<:r> = 0" }, { "<:s> = 0" },
			{ "<:t> = 0" }, { "<:u> = 0" }, { "<:v> = 0" }
		};
		CHECK(values_matches_any_of(o1_vals, o1_expected));
		CHECK(values_matches_any_of(o2_vals, o2_expected));
	}

	// Test 30: o1 = i2[t-2], o2 = 0 — cross-delayed, second constant false.
	TEST_CASE("G(o1[t]=i2[t-2] && o2[t]=0): cross 2-step delay + constant false, 8 steps") {
		bdd_init<Bool>();
		auto spec = create_spec("G (o1[t]:tau = i2[t-2]:tau && o2[t]:tau = 0).");
		io_context<node_t> ctx;
		strings i1_values = {
			"<:a> = 0", "<:b> = 0", "<:c> = 0", "<:d> = 0",
			"<:e> = 0", "<:f> = 0", "<:g> = 0", "<:h> = 0"
		};
		strings i2_values = {
			"<:p> = 0", "<:q> = 0", "<:r> = 0", "<:s> = 0",
			"<:t> = 0", "<:u> = 0", "<:v> = 0", "<:w> = 0"
		};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 8);
		CHECK(maybe_i.has_value());
		auto o1_vals = o1->get_values();
		auto o2_vals = o2->get_values();
		CHECK(o1_vals.size() == 8);
		CHECK(o2_vals.size() == 8);
		// o1 = i2[t-2]: steps 0-1 → "F"; steps 2..7 → i2[0..5]
		std::vector<strings> o1_expected = {
			{ "F" }, { "F" },
			{ "<:p> = 0" }, { "<:q> = 0" }, { "<:r> = 0" },
			{ "<:s> = 0" }, { "<:t> = 0" }, { "<:u> = 0" }
		};
		CHECK(values_matches_any_of(o1_vals, o1_expected));
		for (const auto& v : o2_vals) CHECK(v == "F");
	}
}


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}