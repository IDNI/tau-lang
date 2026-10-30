// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_integration-interpreter_helper.h"

// Whether some step of a run outputs v.
static bool eventually(const strings& values, const std::string& v) {
	return std::find(values.begin(), values.end(), v) != values.end();
}

// Whether a finite run satisfies `hold` until `goal`: every step before the
// first `goal` outputs `hold`, and a strong until also reaches `goal`.
static bool until_holds(const strings& values, const std::string& hold,
	const std::string& goal, bool strong)
{
	for (const auto& v : values) {
		if (v == goal) return true;
		if (v != hold) return false;
	}
	return !strong;
}

// The sbf constant a printed output denotes, so that sbf outputs compare by
// value and not by the spelling the printer chose.
static tref sbf_value(const std::string& v) {
	tref bf = tau::get("{" + v + "}:sbf", parse_bf()).value_or(nullptr);
	REQUIRE( bf );
	return bf;
}

static strings sbf_values(const strings& values) {
	strings out;
	for (const auto& v : values)
		out.push_back(tau::get(sbf_value(v)).to_str());
	return out;
}

// Whether two runs output the same sbf values step by step.
static bool same_sbf_values(const strings& actual, const strings& expected) {
	return sbf_values(actual) == sbf_values(expected);
}

// ─────────────────────────────────────────────────────────────────────────────
// Group 1 – G with constant outputs (0 = "F", 1 = "T")
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (tau): G constant outputs") {

	TEST_CASE("G(o1[t]:tau = 0) – 4 steps produces all F") {
		auto spec = spec_or_fail("G(o1[t]:tau = 0).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		strings expected = {"F", "F", "F", "F"};
		CHECK(o1->get_values() == expected);
	}

	TEST_CASE("G(o1[t]:tau = 1) – 4 steps produces all T") {
		auto spec = spec_or_fail("G(o1[t]:tau = 1).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		strings expected = {"T", "T", "T", "T"};
		CHECK(o1->get_values() == expected);
	}

	TEST_CASE("G(o1[t]:tau = 0) – 6 steps produces all F") {
		auto spec = spec_or_fail("G(o1[t]:tau = 0).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
		strings expected = {"F", "F", "F", "F", "F", "F"};
		CHECK(o1->get_values() == expected);
	}

	TEST_CASE("G(o1[t]:tau = 1) – 8 steps produces all T") {
		auto spec = spec_or_fail("G(o1[t]:tau = 1).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 8);
		CHECK(maybe_i.has_value());
		strings expected = {"T", "T", "T", "T", "T", "T", "T", "T"};
		CHECK(o1->get_values() == expected);
	}

	TEST_CASE("G(o1[t]:tau = 0 && o2[t]:tau = 1) – 5 steps") {
		auto spec = spec_or_fail("G(o1[t]:tau = 0 && o2[t]:tau = 1).");
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
		auto spec = spec_or_fail("G(o1[t]:tau = i1[t]:tau).");
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
		auto spec = spec_or_fail("G(o1[t]:tau = i1[t]:tau).");
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
		auto spec = spec_or_fail("G(o1[t]:tau = i1[t]:tau).");
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
		auto spec = spec_or_fail("G(o1[t]:tau = i1[t]:tau).");
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
		auto spec = spec_or_fail("G(o1[t]:tau = i1[t]:tau).");
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
		auto spec = spec_or_fail("G(o1[t]:tau = i1[t]:tau && o2[t]:tau = 0).");
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
		auto spec = spec_or_fail("G(o1[t]:tau = i1[t]:tau && o2[t]:tau = 1).");
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
		auto spec = spec_or_fail("G(o1[t]:tau = 0 && o2[t]:tau = i1[t]:tau).");
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
		auto spec = spec_or_fail("G(o1[t]:tau = i1[t]:tau && o2[t]:tau = i2[t]:tau).");
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
		auto spec = spec_or_fail("G(o1[t]:tau = 1 && o2[t]:tau = 0).");
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
		auto spec = spec_or_fail("G(o1[t]:tau = i1[t-1]:tau).");
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
		auto spec = spec_or_fail("G(o1[t]:tau = i1[t-2]:tau).");
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
		auto spec = spec_or_fail("G(o1[t]:tau = i1[t-3]:tau).");
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
		auto spec = spec_or_fail("G(o1[t]:tau = i1[t-1]:tau).");
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
		auto spec = spec_or_fail("G(o1[t]:tau = i1[t-2]:tau && o2[t]:tau = i1[t-1]:tau).");
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
// Group 5 – F formulas (eventually): the run reaches the eventuality
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (tau): F formulas") {

	TEST_CASE("F(o1[t]:tau = 0) – 4 steps realizable" * doctest::skip(!ltlsynt_available())) {
		auto spec = spec_or_fail("F(o1[t]:tau = 0).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 4);
		CHECK(eventually(o1->get_values(), "F"));
	}

	TEST_CASE("F(o1[t]:tau = 1) – 4 steps realizable" * doctest::skip(!ltlsynt_available())) {
		auto spec = spec_or_fail("F(o1[t]:tau = 1).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 4);
		CHECK(eventually(o1->get_values(), "T"));
	}

	TEST_CASE("F(o1[t]:tau = i1[t]:tau) – 5 steps realizable" * doctest::skip(!ltlsynt_available())) {
		auto spec = spec_or_fail("F(o1[t]:tau = i1[t]:tau).");
		io_context<node_t> ctx;
		strings i1_values = {"<:x> = 0", "<:y> = 0", "<:z> = 0", "<:x> = 0", "<:y> = 0"};
		ctx.add_input("i1", tau_type_id<node_t>(), std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 5);
		bool met = false;
		for (size_t k = 0; k < o1->get_values().size(); ++k)
			met = met || o1->get_values()[k] == i1_values[k];
		CHECK(met);
	}

	TEST_CASE("F(o1[t]:tau = 0 && o2[t]:tau = 1) – 4 steps realizable" * doctest::skip(!ltlsynt_available())) {
		auto spec = spec_or_fail("F(o1[t]:tau = 0 && o2[t]:tau = 1).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 4);
		CHECK(o2->get_values().size() == 4);
		bool met = false;
		for (size_t k = 0; k < 4 && k < o1->get_values().size()
			&& k < o2->get_values().size(); ++k)
			met = met || (o1->get_values()[k] == "F"
				&& o2->get_values()[k] == "T");
		CHECK(met);
	}

	TEST_CASE("G(o1[t]:tau = i1[t]:tau) && F(o2[t]:tau = 0) – 5 steps" * doctest::skip(!ltlsynt_available())) {
		auto spec = spec_or_fail("G(o1[t]:tau = i1[t]:tau) && F(o2[t]:tau = 0).");
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
		CHECK(o2->get_values().size() == 5);
		CHECK(eventually(o2->get_values(), "F"));
	}

} // TEST_SUITE "LTL Execution (tau): F formulas"

// ─────────────────────────────────────────────────────────────────────────────
// Group 6 – U / R / W formulas: the run keeps the until, release or weak until
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (tau): U R W formulas") {

	TEST_CASE("(o1[t]:tau = 1) until (o1[t]:tau = 0) – 5 steps (no input vars)" * doctest::skip(!ltlsynt_available())) {
		auto spec = spec_or_fail("(o1[t]:tau = 1) until (o1[t]:tau = 0).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 5);
		CHECK(until_holds(o1->get_values(), "T", "F", true));
	}

	TEST_CASE("(o1[t]:tau = 0) release (o1[t]:tau = 1) – 4 steps (release)" * doctest::skip(!ltlsynt_available())) {
		auto spec = spec_or_fail("(o1[t]:tau = 0) release (o1[t]:tau = 1).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		// o1 = 0 and o1 = 1 never hold together, so o1 = 1 holds forever
		CHECK(o1->get_values() == strings(4, "T"));
	}

	TEST_CASE("(o1[t]:tau = 1) weak_until (o1[t]:tau = 0) – 5 steps (weak until)" * doctest::skip(!ltlsynt_available())) {
		auto spec = spec_or_fail("(o1[t]:tau = 1) weak_until (o1[t]:tau = 0).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 5);
		CHECK(until_holds(o1->get_values(), "T", "F", false));
	}

	TEST_CASE("(o1[t]:tau = 0) weak_until (o1[t]:tau = 1) – 5 steps weak until (no input vars)" * doctest::skip(!ltlsynt_available())) {
		auto spec = spec_or_fail("(o1[t]:tau = 0) weak_until (o1[t]:tau = 1).");
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values().size() == 5);
		CHECK(until_holds(o1->get_values(), "F", "T", false));
	}

} // TEST_SUITE "LTL Execution (tau): U R W formulas"

// ─────────────────────────────────────────────────────────────────────────────
// Group 7 – sbf: G with constant outputs
// Uses nontrivial {}-expressions in constants.
// bdd_init<Bool>() is mandatory first in each test case for sbf.
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (sbf): G constant outputs") {

	TEST_CASE("sbf outputs compare by value, not by spelling") {
		CHECK(same_sbf_values({ "X Y", "X | X' Z" }, { "X & Y", "X | Z" }));
		CHECK(!same_sbf_values({ "X Y" }, { "X | Z" }));
	}

	// G(o1:sbf = {X | Z}:sbf) — output is always the disjunction X∨Z.
	TEST_CASE("G(o1:sbf = {X | Z}) constant output, 5 steps") {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = {X | Z}:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(same_sbf_values(o1->get_values(), strings(5, "X | Z")));
	}

	// G(o1:sbf = {X | (Y & Z)}:sbf) — nontrivial three-variable compound.
	TEST_CASE("G(o1:sbf = {X | (Y & Z)}) constant output, 6 steps") {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = {X | (Y & Z)}:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
		CHECK(same_sbf_values(o1->get_values(), strings(6, "X | (Y & Z)")));
	}

	// G(o1:sbf = {X & Y}:sbf) with 7-step run.
	TEST_CASE("G(o1:sbf = {X & Y}) constant output, 7 steps") {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = {X & Y}:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 7);
		CHECK(maybe_i.has_value());
		CHECK(same_sbf_values(o1->get_values(), strings(7, "X & Y")));
	}

} // TEST_SUITE "LTL Execution (sbf): G constant outputs"

// ─────────────────────────────────────────────────────────────────────────────
// Group 8 – sbf: G with input mirroring
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (sbf): G input mirroring") {

	// G(o1:sbf = i1:sbf) — real input mirroring, 3 steps.
	TEST_CASE("G(o1:sbf = i1:sbf) mirroring, 3 steps") {
		io_context<node_t> ctx;
		strings i1_values = {"X & Y", "X | Z", "X | (Y & Z)"};
		ctx.add_input("i1", sbf_type_id<node_t>(),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = i1[t]:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 3);
		CHECK(maybe_i.has_value());
		CHECK(same_sbf_values(o1->get_values(), i1_values));
	}

	// G(o1:sbf = i1:sbf) — real input mirroring, 4 steps with nontrivial expressions.
	TEST_CASE("G(o1:sbf = i1:sbf) mirroring, 4 steps") {
		io_context<node_t> ctx;
		strings i1_values = {"X & Y", "X | Z", "X | (Y & Z)", "X & Y"};
		ctx.add_input("i1", sbf_type_id<node_t>(),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = i1[t]:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(same_sbf_values(o1->get_values(), i1_values));
	}

	// G(o1:sbf = i1:sbf) — real input mirroring, 5 steps with varied sbf expressions.
	TEST_CASE("G(o1:sbf = i1:sbf) mirroring, 5 steps") {
		io_context<node_t> ctx;
		strings i1_values = {"X & Y", "X | Z", "X | (Y & Z)", "X & Y", "X | Z"};
		ctx.add_input("i1", sbf_type_id<node_t>(),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = i1[t]:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(same_sbf_values(o1->get_values(), i1_values));
	}

	// G(o1:sbf = i1:sbf') — complement mirroring: output is complement of input.
	TEST_CASE("G(o1:sbf = i1:sbf') complement mirroring, 4 steps") {
		io_context<node_t> ctx;
		strings i1_values = {"X & Y", "X | Z", "X | (Y & Z)", "X & Y"};
		ctx.add_input("i1", sbf_type_id<node_t>(),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = i1[t]:sbf').");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		strings complement;
		for (const auto& v : i1_values) complement.push_back("(" + v + ")'");
		CHECK(same_sbf_values(o1->get_values(), complement));
	}

	// G(o1:sbf | i1:sbf = 1) — union equals top: o1 covers the complement of i1.
	TEST_CASE("G(o1:sbf | i1:sbf = 1) union-top, 5 steps") {
		io_context<node_t> ctx;
		strings i1_values = {"X & Y", "X | Z", "X | (Y & Z)", "X & Y", "X | Z"};
		ctx.add_input("i1", sbf_type_id<node_t>(),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf | i1[t]:sbf = 1).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		REQUIRE(o1->get_values().size() == 5);
		for (size_t k = 0; k < 5; ++k)
			CHECK(same_sbf_values({ o1->get_values()[k] + " | " + i1_values[k] },
				{ "1" }));
	}

} // TEST_SUITE "LTL Execution (sbf): G input mirroring"

// ─────────────────────────────────────────────────────────────────────────────
// Group 9 – sbf: G with lookback
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (sbf): G with lookback") {

	// G(o1:sbf = i1[t-1]:sbf) — output mirrors input with 1-step delay, 4 steps.
	TEST_CASE("G(o1:sbf = i1[t-1]:sbf) lookback-1, 4 steps") {
		io_context<node_t> ctx;
		strings i1_values = {"X & Y", "X | Z", "X | (Y & Z)", "X & Y"};
		ctx.add_input("i1", sbf_type_id<node_t>(),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = i1[t-1]:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		auto values = o1->get_values();
		REQUIRE(values.size() == 4);
		// o1[t] = i1[t-1] from step 1 on
		CHECK(same_sbf_values(strings(values.begin() + 1, values.end()),
			strings(i1_values.begin(), i1_values.end() - 1)));
	}

	// G(o1:sbf = i1[t-1]:sbf) — output mirrors input with 1-step delay, 5 steps.
	TEST_CASE("G(o1:sbf = i1[t-1]:sbf) lookback-1, 5 steps") {
		io_context<node_t> ctx;
		strings i1_values = {"X & Y", "X | Z", "X | (Y & Z)", "X & Y", "X | Z"};
		ctx.add_input("i1", sbf_type_id<node_t>(),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = i1[t-1]:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		auto values = o1->get_values();
		REQUIRE(values.size() == 5);
		// o1[t] = i1[t-1] from step 1 on
		CHECK(same_sbf_values(strings(values.begin() + 1, values.end()),
			strings(i1_values.begin(), i1_values.end() - 1)));
	}

	// G(o1:sbf = i1[t-2]:sbf) — output mirrors input with 2-step delay, 5 steps.
	TEST_CASE("G(o1:sbf = i1[t-2]:sbf) lookback-2, 5 steps") {
		io_context<node_t> ctx;
		strings i1_values = {"X & Y", "X | Z", "X | (Y & Z)", "X & Y", "X | Z"};
		ctx.add_input("i1", sbf_type_id<node_t>(),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = i1[t-2]:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		auto values = o1->get_values();
		REQUIRE(values.size() == 5);
		// o1[t] = i1[t-2] from step 2 on
		CHECK(same_sbf_values(strings(values.begin() + 2, values.end()),
			strings(i1_values.begin(), i1_values.end() - 2)));
	}

	// G(o1:sbf = i1[t-3]:sbf) — output mirrors input with 3-step delay, 6 steps.
	TEST_CASE("G(o1:sbf = i1[t-3]:sbf) lookback-3, 6 steps") {
		io_context<node_t> ctx;
		strings i1_values = {"X & Y", "X | Z", "X | (Y & Z)", "X & Y", "X | Z", "X | (Y & Z)"};
		ctx.add_input("i1", sbf_type_id<node_t>(),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = i1[t-3]:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
		auto values = o1->get_values();
		REQUIRE(values.size() == 6);
		// o1[t] = i1[t-3] from step 3 on
		CHECK(same_sbf_values(strings(values.begin() + 3, values.end()),
			strings(i1_values.begin(), i1_values.end() - 3)));
	}

	// G(o1:sbf = o1[t-1]:sbf) — output echoes its own previous value (no input needed).
	TEST_CASE("G(o1:sbf = o1[t-1]:sbf) self-lookback, 5 steps") {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (o1[t]:sbf = o1[t-1]:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		auto values = o1->get_values();
		REQUIRE(values.size() == 5);
		CHECK(same_sbf_values(values, strings(5, values[0])));
	}

} // TEST_SUITE "LTL Execution (sbf): G with lookback"

// ─────────────────────────────────────────────────────────────────────────────
// Group 10 – sbf: F and U/W operators
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (sbf): F and U W operators") {

	// F(o1:sbf = {X & Y}:sbf) — output equals compound constant eventually.
	TEST_CASE("F(o1:sbf = {X & Y}) eventually, 4 steps" * doctest::skip(!ltlsynt_available())) {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "F (o1[t]:sbf = {X & Y}:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(eventually(sbf_values(o1->get_values()),
			tau::get(sbf_value("X & Y")).to_str()));
	}

	// F(o1:sbf = {X | (Y & Z)}:sbf) — nontrivial three-variable constant, eventually.
	TEST_CASE("F(o1:sbf = {X | (Y & Z)}) eventually, 5 steps" * doctest::skip(!ltlsynt_available())) {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "F (o1[t]:sbf = {X | (Y & Z)}:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(eventually(sbf_values(o1->get_values()),
			tau::get(sbf_value("X | (Y & Z)")).to_str()));
	}

	// (o1:sbf = {X & Y}) U (o1:sbf = {X | Z}) — until with sbf constants.
	TEST_CASE("({X & Y}:sbf) U ({X | Z}:sbf) until, 4 steps" * doctest::skip(!ltlsynt_available())) {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx,
			"(o1[t]:sbf = {X & Y}:sbf) until (o1[t]:sbf = {X | Z}:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(until_holds(sbf_values(o1->get_values()),
			tau::get(sbf_value("X & Y")).to_str(),
			tau::get(sbf_value("X | Z")).to_str(), true));
	}

	// (o1:sbf = {X | (Y & Z)}) W (o1:sbf = {X & Y}) — weak until.
	TEST_CASE("({X|(Y&Z)}:sbf) W ({X&Y}:sbf) weak-until, 5 steps" * doctest::skip(!ltlsynt_available())) {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx,
			"(o1[t]:sbf = {X | (Y & Z)}:sbf) weak_until (o1[t]:sbf = {X & Y}:sbf).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(until_holds(sbf_values(o1->get_values()),
			tau::get(sbf_value("X | (Y & Z)")).to_str(),
			tau::get(sbf_value("X & Y")).to_str(), false));
	}

	// G(F(o1:sbf = {X & Y}:sbf)) — nontrivial sbf constant, infinitely often.
	TEST_CASE("G(F(o1:sbf = {X & Y})) inf-often, 6 steps" * doctest::skip(!ltlsynt_available())) {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_type_id<node_t>(), o1);
		auto spec = create_spec(ctx, "G (F (o1[t]:sbf = {X & Y}:sbf)).");
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
		CHECK(eventually(sbf_values(o1->get_values()),
			tau::get(sbf_value("X & Y")).to_str()));
	}

} // TEST_SUITE "LTL Execution (sbf): F and U W operators"

// ─────────────────────────────────────────────────────────────────────────────
// Group 11 – bv: G with constant outputs
// Uses nontrivial {#b...} and decimal bitvector constants.
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (bv): G constant outputs") {

	// G(o1:bv[8] = {#b10110101}:bv[8]) — nontrivial 8-bit pattern, 4 steps.
	TEST_CASE("G(o1:bv[8] = {#b10110101}) constant output, 4 steps") {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "G (o1[t]:bv[8] = {#b10110101}:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == strings(4, "181"));
	}

	// G(o1:bv[8] = {#b00001111}:bv[8]) — lower nibble, 5 steps.
	TEST_CASE("G(o1:bv[8] = {#b00001111}) constant output, 5 steps") {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "G (o1[t]:bv[8] = {#b00001111}:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == strings(5, "15"));
	}

	// G(o1:bv[8] = {#b11110000}:bv[8]) — upper nibble, 6 steps.
	TEST_CASE("G(o1:bv[8] = {#b11110000}) constant output, 6 steps") {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "G (o1[t]:bv[8] = {#b11110000}:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == strings(6, "240"));
	}

	// G(o1:bv[8] = {5}:bv[8]) — decimal constant 5, 5 steps.
	TEST_CASE("G(o1:bv[8] = {5}) decimal constant output, 5 steps") {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "G (o1[t]:bv[8] = {5}:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == strings(5, "5"));
	}

	// G(o1:bv[8] = {255}:bv[8]) — all-ones byte, 7 steps.
	TEST_CASE("G(o1:bv[8] = {255}) all-ones byte constant output, 7 steps") {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "G (o1[t]:bv[8] = {255}:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 7);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == strings(7, "255"));
	}

} // TEST_SUITE "LTL Execution (bv): G constant outputs"

// ─────────────────────────────────────────────────────────────────────────────
// Group 12 – bv: G with input mirroring, F, U, W
// ─────────────────────────────────────────────────────────────────────────────

TEST_SUITE("LTL Execution (bv): G input mirroring and LTL operators") {

	// G(o1:bv[8] = i1:bv[8]) — real bv input mirroring, 3 steps.
	TEST_CASE("G(o1:bv[8] = i1:bv[8]) mirroring, 3 steps") {
		io_context<node_t> ctx;
		strings i1_values = {"5", "#b10110101", "255"};
		ctx.add_input("i1", bv_type_id<node_t>(8),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "G (o1[t]:bv[8] = i1[t]:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 3);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == strings{ "5", "181", "255" });
	}

	// G(o1:bv[8] = i1:bv[8]) — real bv input mirroring, 4 steps with nontrivial patterns.
	TEST_CASE("G(o1:bv[8] = i1:bv[8]) mirroring, 4 steps") {
		io_context<node_t> ctx;
		strings i1_values = {"#b00001111", "#b11110000", "#b10110101", "255"};
		ctx.add_input("i1", bv_type_id<node_t>(8),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "G (o1[t]:bv[8] = i1[t]:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == strings{ "15", "240", "181", "255" });
	}

	// G(o1:bv[8] = i1:bv[8]) — real bv input mirroring, 5 steps.
	TEST_CASE("G(o1:bv[8] = i1:bv[8]) mirroring, 5 steps") {
		io_context<node_t> ctx;
		strings i1_values = {"5", "#b10110101", "255", "#b00001111", "#b11110000"};
		ctx.add_input("i1", bv_type_id<node_t>(8),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "G (o1[t]:bv[8] = i1[t]:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(o1->get_values() == strings{ "5", "181", "255", "15", "240" });
	}

	// G(o1:bv[8] = i1[t-1]:bv[8]) — output mirrors bv input with 1-step delay, 5 steps.
	TEST_CASE("G(o1:bv[8] = i1[t-1]:bv[8]) lookback-1 mirroring, 5 steps") {
		io_context<node_t> ctx;
		strings i1_values = {"5", "#b10110101", "255", "#b00001111", "#b11110000"};
		ctx.add_input("i1", bv_type_id<node_t>(8),
		              std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "G (o1[t]:bv[8] = i1[t-1]:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		auto values = o1->get_values();
		REQUIRE(values.size() == 5);
		// o1[t] = i1[t-1] from step 1 on
		CHECK(strings(values.begin() + 1, values.end())
			== strings{ "5", "181", "255", "15" });
	}

	// F(o1:bv[8] = {#b10110101}:bv[8]) — eventually outputs nontrivial bit pattern.
	TEST_CASE("F(o1:bv[8] = {#b10110101}) eventually, 4 steps" * doctest::skip(!ltlsynt_available())) {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "F (o1[t]:bv[8] = {#b10110101}:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(eventually(o1->get_values(), "181"));
	}

	// (o1:bv[8] = {#b00001111}) U (o1:bv[8] = {#b11110000}) — complementary nibbles until.
	TEST_CASE("({#b00001111}:bv[8]) U ({#b11110000}:bv[8]) until, 5 steps" * doctest::skip(!ltlsynt_available())) {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx,
			"(o1[t]:bv[8] = {#b00001111}:bv[8]) until (o1[t]:bv[8] = {#b11110000}:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		CHECK(until_holds(o1->get_values(), "15", "240", true));
	}

	// G(F(o1:bv[8] = {255}:bv[8])) — output equals 255 infinitely often, 6 steps.
	TEST_CASE("G(F(o1:bv[8] = {255})) inf-often constant output, 6 steps" * doctest::skip(!ltlsynt_available())) {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx, "G (F (o1[t]:bv[8] = {255}:bv[8])).");
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
		CHECK(eventually(o1->get_values(), "255"));
	}

	// (o1:bv[8] = {5}) W (o1:bv[8] = {#b10110101}) — decimal then binary weak-until, 4 steps.
	TEST_CASE("({5}:bv[8]) W ({#b10110101}:bv[8]) weak-until, 4 steps" * doctest::skip(!ltlsynt_available())) {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx,
			"(o1[t]:bv[8] = {5}:bv[8]) weak_until (o1[t]:bv[8] = {#b10110101}:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		CHECK(until_holds(o1->get_values(), "5", "181", false));
	}

	// (o1:bv[8] = {#b00001111}) R (o1:bv[8] = {#b11110000}) — release, 5 steps.
	// p R q: q holds until (and including) when p holds; or q holds forever.
	// System can always output q (upper nibble), satisfying R vacuously.
	TEST_CASE("({#b00001111}:bv[8]) R ({#b11110000}:bv[8]) release, 5 steps" * doctest::skip(!ltlsynt_available())) {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx,
			"(o1[t]:bv[8] = {#b00001111}:bv[8]) release (o1[t]:bv[8] = {#b11110000}:bv[8]).");
		auto maybe_i = run<node_t>(spec, ctx, 5);
		CHECK(maybe_i.has_value());
		// the two constants never hold together, so o1 = 240 holds forever
		CHECK(o1->get_values() == strings(5, "240"));
	}

	// G(F(o1:bv[8] = {#b10110101})) && G(F(o1:bv[8] != {#b10110101})) — alternating.
	// Requires a 2-state Mealy machine — multi-state encoding.
	TEST_CASE("G(F(o1:bv[8]={#b10110101})) && G(F(o1:bv[8]!={#b10110101})) alternating, 6 steps" * doctest::skip(!ltlsynt_available())) {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", bv_type_id<node_t>(8), o1);
		auto spec = create_spec(ctx,
			"G (F (o1[t]:bv[8] = {#b10110101}:bv[8])) && "
			"G (F (!(o1[t]:bv[8] = {#b10110101}:bv[8]))).");
		auto maybe_i = run<node_t>(spec, ctx, 6);
		CHECK(maybe_i.has_value());
		auto values = o1->get_values();
		CHECK(eventually(values, "181"));
		CHECK(std::any_of(values.begin(), values.end(),
			[](const std::string& v) { return v != "181"; }));
	}

} // TEST_SUITE "LTL Execution (bv): G input mirroring and LTL operators"



// ─────────────────────────────────────────────────────────────────────────────
// Data game strategies played without a Mealy view: every step decodes the
// outputs from the codes of the game.
// ─────────────────────────────────────────────────────────────────────────────

namespace {

// Sets the Mealy view bound of the data game strategies for one scope.
struct no_mealy_view {
	size_t saved = data_game_mealy_max_states;
	no_mealy_view() { data_game_mealy_max_states = 0; }
	~no_mealy_view() { data_game_mealy_max_states = saved; }
};

} // namespace

TEST_SUITE("LTL Execution: data game strategy without a Mealy view") {

#ifdef TAU_PACK_HAS_BA_QLT
	TEST_CASE("qlt order types: o1 meets 1/2 two steps after a value at most 0"
		* doctest::skip(!ltlsynt_available()))
	{
		no_mealy_view guard;
		io_context<node_t> ctx;
		strings i1_values = {"{1}", "{2}", "{3}", "{4}"};
		ctx.add_input("i1", qlt_type_id<node_t>(),
			std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", qlt_type_id<node_t>(), o1);
		ctx.add_output("o2", qlt_type_id<node_t>(), o2);
		auto spec = create_spec(ctx,
			"((o1[t]:qlt != i1[t-1]:qlt || {1}:qlt <= o2[t-1]:qlt)) U "
			"((o1[t-2]:qlt <= {0}:qlt && o1[t]:qlt = {1/2}:qlt)).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		auto v1 = o1->get_values(), v2 = o2->get_values();
		INFO("o1: " << [&] { std::string s; for (auto& x : v1) s += x + " "; return s; }());
		REQUIRE(v1.size() >= 3);
		bool met = false;
		for (size_t k = 2; k < v1.size(); ++k) met = met || v1[k] == "1/2";
		CHECK(met);
		// a qlt stream holds one point of the order at each step
		for (auto& x : v1) CHECK((x != "bot" && x != "top"));
		for (auto& x : v2) CHECK((x != "bot" && x != "top"));
	}
#endif // TAU_PACK_HAS_BA_QLT

	TEST_CASE("tau: the goal takes a value no stream holds"
		* doctest::skip(!ltlsynt_available()))
	{
		no_mealy_view guard;
		io_context<node_t> ctx;
		strings i1_values = {"F", "T", "F"};
		ctx.add_input("i1", tau_type_id<node_t>(),
			std::make_shared<vector_input_stream>(i1_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto spec = create_spec(ctx,
			"(always o2[t]:tau = o1[t-1]:tau) && (sometimes (o2[t]:tau != 0 "
			"&& o2[t]:tau != 1 && o2[t]:tau != i1[t]:tau)).");
		auto maybe_i = run<node_t>(spec, ctx, 3);
		CHECK(maybe_i.has_value());
		auto v1 = o1->get_values(), v2 = o2->get_values();
		REQUIRE(v2.size() == 3);
		REQUIRE(v1.size() == 3);
		INFO("o2: " << v2[0] << " " << v2[1] << " " << v2[2]);
		bool met = false;
		for (size_t k = 0; k < v2.size(); ++k)
			met = met || (v2[k] != "F" && v2[k] != "T" && v2[k] != i1_values[k]);
		CHECK(met);
		for (size_t k = 1; k < v2.size(); ++k) CHECK(v2[k] == v1[k - 1]);
	}

	TEST_CASE("tau: complement codes, o1 is the complement of i1 two steps "
		"before" * doctest::skip(!ltlsynt_available()))
	{
		no_mealy_view guard;
		io_context<node_t> ctx;
		strings i1_values = {"F", "T", "F", "T"};
		strings i2_values = {"T", "F", "F", "T"};
		ctx.add_input("i1", tau_type_id<node_t>(),
			std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(),
			std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto spec = create_spec(ctx,
			"(always (o2[0] = 0 && (o1[t-1] = 1 -> o2[t] = 1) && "
			"i1[t-2] = o1[t]')) && (sometimes (o1[t] = 1 && "
			"o2[t] = o2[t-1]')) && (sometimes (o1[t-1] = i2[t-1])).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		auto v1 = o1->get_values();
		REQUIRE(v1.size() == 4);
		// o1[t] = i1[t-2]'
		CHECK(v1[2] == "T");
		CHECK(v1[3] == "F");
	}

	TEST_CASE("tau: complement codes over values other than 0 and 1"
		* doctest::skip(!ltlsynt_available()))
	{
		no_mealy_view guard;
		io_context<node_t> ctx;
		strings i1_values = {"<:x> = 0", "<:y> = 0", "<:x> != 0", "<:y> = 0"};
		strings i2_values = {"<:x> = 0", "<:x> = 0", "<:y> = 0", "<:x> = 0"};
		ctx.add_input("i1", tau_type_id<node_t>(),
			std::make_shared<vector_input_stream>(i1_values));
		ctx.add_input("i2", tau_type_id<node_t>(),
			std::make_shared<vector_input_stream>(i2_values));
		auto o1 = std::make_shared<vector_output_stream>();
		auto o2 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", tau_type_id<node_t>(), o1);
		ctx.add_output("o2", tau_type_id<node_t>(), o2);
		auto spec = create_spec(ctx,
			"(always (o2[0] = 0 && (o1[t-1] = 1 -> o2[t] = 1) && "
			"i1[t-2] = o1[t]')) && (sometimes (o1[t] = 1 && "
			"o2[t] = o2[t-1]')) && (sometimes (o1[t-1] = i2[t-1])).");
		auto maybe_i = run<node_t>(spec, ctx, 4);
		CHECK(maybe_i.has_value());
		auto v1 = o1->get_values(), v2 = o2->get_values();
		REQUIRE(v1.size() == 4);
		REQUIRE(v2.size() == 4);
		// o1[t] = i1[t-2]'
		CHECK(v1[2] == "<:x> != 0");
		CHECK(v1[3] == "<:y> != 0");
		// o2[0] = 0, and o1[1] = 1 forces o2[2] = 1
		CHECK(v2[0] == "F");
		if (v1[1] == "T") CHECK(v2[2] == "T");
	}
}
