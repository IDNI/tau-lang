// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Runs of program-set expressions (src/program_set.h): the compiled
// specification is played, its hidden prediction and selector outputs
// solved and never printed. One input i1, one visible output o1, type sbf.

#include "test_integration-interpreter_helper.h"

namespace {

// The term of @p src, parsed with the hooks off (on, two tau constants fold
// into one; the run reads either).
tref term_or_fail(const char* src) {
	use_hooks_guard<node_t> hooks_off(false);
	tref t = tau::get(src, parse_bf()).value_or(nullptr);
	REQUIRE( t );
	return t;
}

// The sbf constant a printed value denotes, so that values compare by value
// and not by the spelling the printer chose.
std::string sbf_value(const std::string& v) {
	tref bf = tau::get("{" + v + "}:sbf", parse_bf()).value_or(nullptr);
	REQUIRE( bf );
	return tau::get(bf).to_str();
}

strings sbf_values(const strings& values) {
	strings out;
	for (const auto& v : values) out.push_back(sbf_value(v));
	return out;
}

const size_t sbf_tid = get_ba_type_id<node_t>(sbf_type<node_t>());

// Runs @p term for @p steps on the inputs @p i1_values; the values o1 wrote.
std::optional<strings> run_term(const char* term, const strings& i1_values,
	size_t steps)
{
	io_context<node_t> ctx;
	if (!i1_values.empty())
		ctx.add_input("i1", sbf_tid,
			std::make_shared<vector_input_stream>(i1_values));
	auto o1 = std::make_shared<vector_output_stream>();
	ctx.add_output("o1", sbf_tid, o1);
	auto r = run<node_t>(term_or_fail(term), ctx, steps);
	if (!r.has_value()) return std::nullopt;
	return sbf_values(o1->get_values());
}

// The value of the stream @p name at step @p at in @p values, or nullptr.
tref value_of(const assignment<node_t>& values, const std::string& name,
	int_t at)
{
	for (const auto& [var, value] : values) {
		tref v = tau::trim(var);
		if (!is_io_var<node_t>(v)) continue;
		if (get_var_name<node_t>(v) != name) continue;
		if (!is_io_initial<node_t>(v) || get_io_time_point<node_t>(v) != at)
			continue;
		return value;
	}
	return nullptr;
}

} // namespace

#define PS_CASE(name) TEST_CASE(name * doctest::skip(!ltlsynt_available()))

TEST_SUITE("program_set execution") {

	// T3: the copier, outside "eventually 1" because of the zero input.
	PS_CASE("S2 the copier copies on a run") {
		strings in = { "0", "1", "1", "0", "1", "1", "0", "0" };
		auto out = run_term("{ always o1[t]:sbf = i1[t]:sbf }:tau"
			" & { sometimes o1[t]:sbf = 1 }:tau'", in, 8);
		REQUIRE( out.has_value() );
		CHECK( *out == sbf_values(in) );
	}

	// H1: the second operand is empty, so the first one is played; no
	// violating path is demanded of the unselected tautology.
	PS_CASE("H1 a union with an empty operand plays the other") {
		auto out = run_term("{ always o1[t]:sbf = 1 }:tau"
			" | { always o1[t]:sbf = o1[t]:sbf }:tau'", {}, 4);
		REQUIRE( out.has_value() );
		CHECK( *out == sbf_values({ "1", "1", "1", "1" }) );
	}

	PS_CASE("P1 a recurring goal recurs within the run") {
		auto out = run_term("{ always sometimes o1[t]:sbf = 1 }:tau", {}, 8);
		REQUIRE( out.has_value() );
		CHECK( std::ranges::count(*out, sbf_value("1")) >= 1 );
	}

	// S3: every realizable case runs; the active obligations hold on a
	// fixed input.
	PS_CASE("S3 the realizable cases run") {
		const char* cases[] = {
			"{ always o1[t]:sbf = i1[t]:sbf }:tau'",
			"{ always (i1[t]:sbf = 1 || [t > 0]) }:tau'",
			"{ always ((i1[t]:sbf != 0 && i1[t]:sbf != 1) -> o1[t]:sbf != i1[t]:sbf) }:tau'"
			" & { always ((i1[t]:sbf != 0 && i1[t]:sbf != 1) -> o1[t]:sbf != i1[t]:sbf') }:tau'",
			"{ sometimes o1[t]:sbf = 1 }:tau & { always sometimes o1[t]:sbf = 1 }:tau'",
			"{ always (i1[t]:sbf != 0 -> sometimes o1[t]:sbf = 1) }:tau"
			" & { sometimes o1[t]:sbf = 1 }:tau'",
			"{ always o1[t]:sbf = o1[t-1]:sbf }:tau",
			"{ always (o1[t]:sbf = 1 || [t > 0]) }:tau"
			" & { always o1[t]:sbf = o1[t-1]:sbf }:tau'",
			"{ always o1[t]:sbf = i1[t-2]:sbf }:tau",
			"{ (always o1[t]:sbf = o1[t-1]:sbf)"
			" && (always (i1[t]:sbf = 1 -> o1[t]:sbf = 1)) }:tau",
		};
		strings in = { "1", "0", "1", "0", "0", "1" };
		for (const char* c : cases) {
			CAPTURE( c );
			auto out = run_term(c, in, 4);
			CHECK( out.has_value() );
			// a term that names no o1 writes nothing to it
			if (out && std::string(c).find("o1") != std::string::npos)
				CHECK( out->size() == 4 );
		}
	}

	// S4: the input fed back the prediction of the complement, from the
	// value chosen before step 0 on; on that path the copier property
	// fails, which is what puts the program outside the copier.
	PS_CASE("S4 the predicted path breaks the complemented literal") {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_tid, o1);
		ctx.add_input("i1", sbf_tid, std::make_shared<vector_input_stream>());
		auto i = interpreter<node_t>::make_interpreter(
			term_or_fail("{ always o1[t]:sbf = i1[t]:sbf }:tau'"), ctx);
		REQUIRE( i.has_value() );
		auto& ip = i.value();
		// the value chosen before step 0 is not kept, so the first input
		// is 0; the predictions steer the inputs from step 1 on
		tref predicted = nullptr;
		strings fed, got;
		bool broken = false;
		for (int_t k = 0; k < 6; ++k) {
			tref in = predicted ? predicted
				: tau::get("{0}:sbf", parse_bf()).value_or(nullptr);
			REQUIRE( in );
			assignment<node_t> values;
			values[build_in_var_at_n<node_t>("i1", k, sbf_tid)] = in;
			auto s = ip.step(values);
			REQUIRE( s.has_value() );
			REQUIRE( s.value().first.has_value() );
			const auto& outs = *s.value().first;
			tref o = value_of(outs, "o1", k);
			REQUIRE( o );
			fed.push_back(tau::get(in).to_str());
			got.push_back(tau::get(o).to_str());
			if (tau::get(o).to_str() != tau::get(in).to_str()) broken = true;
			// the hidden outputs are kept out of the step's map
			predicted = value_of(ip.memory, "w_ps0_d_i1", k);
			REQUIRE( predicted );
		}
		CAPTURE( fed ); CAPTURE( got );
		CHECK( broken );
	}

	// P2: T3 on the predicted path: the copier copies, and the predicted
	// path keeps "eventually 1" from holding, so it never asks a 1.
	PS_CASE("P2 the copier follows the recorded progress on the predicted path") {
		io_context<node_t> ctx;
		auto o1 = std::make_shared<vector_output_stream>();
		ctx.add_output("o1", sbf_tid, o1);
		ctx.add_input("i1", sbf_tid, std::make_shared<vector_input_stream>());
		auto i = interpreter<node_t>::make_interpreter(
			term_or_fail("{ always o1[t]:sbf = i1[t]:sbf }:tau"
				" & { sometimes o1[t]:sbf = 1 }:tau'"), ctx);
		REQUIRE( i.has_value() );
		auto& ip = i.value();
		tref predicted = nullptr;
		for (int_t k = 0; k < 6; ++k) {
			tref in = predicted ? predicted
				: tau::get("{0}:sbf", parse_bf()).value_or(nullptr);
			REQUIRE( in );
			assignment<node_t> values;
			values[build_in_var_at_n<node_t>("i1", k, sbf_tid)] = in;
			auto s = ip.step(values);
			REQUIRE( s.has_value() );
			REQUIRE( s.value().first.has_value() );
			const auto& outs = *s.value().first;
			tref o = value_of(outs, "o1", k);
			REQUIRE( o );
			CHECK( sbf_value(tau::get(o).to_str())
				== sbf_value(tau::get(in).to_str()) );
			CHECK( sbf_value(tau::get(o).to_str()) != sbf_value("1") );
			// the hidden outputs are kept out of the step's map
			predicted = value_of(ip.memory, "w_ps0_d_i1", k);
			REQUIRE( predicted );
		}
	}
}
