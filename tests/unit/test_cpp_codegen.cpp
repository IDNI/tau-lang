// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#include "cpp_codegen.h"

#include <sstream>
#include <string>

using namespace idni::tau_lang;

// Helpers.
static hoa_automaton simple_mealy_echo() {
	// 1 state, one self-loop edge per input:
	//   state q0, guard 0 → q0 setting output 1
	//   state q0, guard !0 → q0 setting output !1
	// AP 0 = input "i", AP 1 = output "o".
	hoa_automaton a;
	a.num_states = 1;
	a.initial_state = 0;
	a.aps = {"i", "o"};
	a.edges.resize(1);
	a.edges[0].push_back(hoa_edge{"0&1",  0, false});
	a.edges[0].push_back(hoa_edge{"!0&!1", 0, false});
	a.state_accepting = {false};
	return a;
}

TEST_SUITE("cpp_codegen") {

	TEST_CASE("emits valid C++ for echo spec") {
		auto a = simple_mealy_echo();
		auto d = build_program_desc_prop(a, {"i"}, {"o"}, "echo_ctrl");
		std::ostringstream os;
		emit_program(d, os);
		std::string s = os.str();
		// Basic structural assertions.
		CHECK(s.find("class echo_ctrl {") != std::string::npos);
		CHECK(s.find("struct inputs") != std::string::npos);
		CHECK(s.find("struct outputs") != std::string::npos);
		CHECK(s.find("outputs step(const inputs&") != std::string::npos);
		// Input and output identifiers.
		CHECK(s.find("bool i") != std::string::npos);
		CHECK(s.find("bool o") != std::string::npos);
		// Dispatch logic uses the input.
		CHECK(s.find("in.i") != std::string::npos);
		// The strategy table carries the edges' guard/assignment literals.
		CHECK(s.find("strat_.edges[0].push_back({{1,1}, 0});") != std::string::npos);
		CHECK(s.find("strat_.edges[0].push_back({{-1,-1}, 0});") != std::string::npos);
	}

	TEST_CASE("emits ok=false fallback when no edge matches") {
		auto a = simple_mealy_echo();
		a.edges[0].pop_back();  // remove the !0&!1 edge → incomplete
		auto d = build_program_desc_prop(a, {"i"}, {"o"});
		std::ostringstream os;
		emit_program(d, os);
		std::string s = os.str();
		CHECK(s.find("o.ok = false") != std::string::npos);
	}

	TEST_CASE("multi-state program_desc mentions every state") {
		hoa_automaton a;
		a.num_states = 3;
		a.initial_state = 0;
		a.aps = {"i", "o"};
		a.edges.resize(3);
		a.edges[0].push_back(hoa_edge{"0&1",  1, false});
		a.edges[1].push_back(hoa_edge{"!0&1", 2, false});
		a.edges[2].push_back(hoa_edge{"1",    0, false});
		a.state_accepting = {false, false, false};
		auto d = build_program_desc_prop(a, {"i"}, {"o"});
		CHECK(d.num_states == 3);
		REQUIRE(d.edges.size() == 3);
		CHECK(d.edges[0].size() == 1);
		CHECK(d.edges[0][0].dst == 1);
		CHECK(d.edges[1][0].dst == 2);
		CHECK(d.edges[2][0].dst == 0);
		std::ostringstream os;
		emit_program(d, os);
		std::string s = os.str();
		CHECK(s.find("strat_.num_states = 3") != std::string::npos);
	}
}

// ── LG-4: disjunctive / parenthesised HOA guards ─────────────────────────────
//
// Every emitter tokenised the guard label by top-level '&' and then read each
// conjunct with `for (char c : idx_str) if (isdigit(c)) idx = idx*10+(c-'0')`.
// That loop SKIPS '|', '(' and ')' instead of rejecting them, so:
//   "0|1"   → the digits are concatenated across the '|' → the single literal
//             (1, true): wrong AP, and the other disjunct vanishes;
//   "(0|1)" → the leading '(' fails the isdigit test and the whole conjunct is
//             dropped, silently widening the guard to `true`.
// Spot prints strategy edge labels as sums of products, so both shapes are
// real.  The assertions below are on the EMITTED TEXT, so they survive any
// choice of internal representation.

TEST_SUITE("cpp_codegen guard parsing (LG-4)") {

	// aps: 0 = input i0, 1 = input i1, 2 = output o.
	static hoa_automaton two_input_one_output(const char* guard) {
		hoa_automaton a;
		a.num_states = 1;
		a.initial_state = 0;
		a.aps = {"i0", "i1", "o"};
		a.edges.resize(1);
		a.edges[0].push_back(hoa_edge{guard, 0, false});
		a.state_accepting = {false};
		return a;
	}

	// "0|1" fires on EITHER input.  The digit-concatenating scan reads it as
	// AP 1 alone, so the emitted dispatch never mentions i0.
	TEST_CASE("[LG4-01] disjunctive guard keeps both disjuncts") {
		auto a = two_input_one_output("0|1");
		std::ostringstream os;
		auto d = build_program_desc_prop(a, {"i0", "i1"}, {"o"}, "Disj");
		emit_program(d, os);
		std::string s = os.str();
		CHECK(s.find("in.i0") != std::string::npos);
		CHECK(s.find("in.i1") != std::string::npos);
	}

	// "(0|1)&2" assigns the output and gates on either input.  The '(' makes
	// the first conjunct disappear, so the emitted guard becomes `true` — the
	// generated program reacts to inputs it was never supposed to react to.
	TEST_CASE("[LG4-02] parenthesised conjunct is not dropped") {
		auto a = two_input_one_output("(0|1)&2");
		std::ostringstream os;
		auto d = build_program_desc_prop(a, {"i0", "i1"}, {"o"}, "ParenDisj");
		emit_program(d, os);
		std::string s = os.str();
		CHECK(s.find("o.o = true") != std::string::npos);
		CHECK(s.find("in.i0") != std::string::npos);
		CHECK(s.find("in.i1") != std::string::npos);
	}

	// Regression guard: flat conjunctions must keep emitting exactly what they
	// did before the consolidation.
	TEST_CASE("[LG4-03] flat conjunctive guards unchanged") {
		auto a = two_input_one_output("0&!1&2");
		std::ostringstream os;
		auto d = build_program_desc_prop(a, {"i0", "i1"}, {"o"}, "Flat");
		emit_program(d, os);
		std::string s = os.str();
		// Flag-only prop emission bakes guard polarity into the strategy
		// table's ints, not a textual "!in.iN" conditional.
		CHECK(s.find("strat_.edges[0].push_back({{1,-1,1}, 0});") != std::string::npos);
		CHECK(s.find("o.o = true") != std::string::npos);
	}

} // TEST_SUITE("cpp_codegen guard parsing (LG-4)")


// ── LG-5: trivially-realizable solutions (num_states == 0) ───────────────────
//
// `solve_ltl_aba` returns `num_states = 0` on the constant-output fast path
// and `is_ltl_aba_realizable` calls that REALIZABLE, so tau_codegen hands the
// empty automaton straight to the emitters.  The State enum body then comes
// out empty while `State state_ = State::q0;` still names q0 — the generated
// header does not compile, and the CLI exits 0 without a word.

TEST_SUITE("cpp_codegen trivial solution (LG-5)") {

	static hoa_automaton empty_automaton() {
		hoa_automaton a;
		a.num_states = 0;
		a.initial_state = 0;
		a.aps = {"i", "o"};
		a.state_accepting = {};
		return a;
	}

	TEST_CASE("[LG5-01] prop emitter does not reference q0 with an empty enum") {
		auto a = empty_automaton();
		std::ostringstream os;
		auto d = build_program_desc_prop(a, {"i"}, {"o"}, "Trivial");
		emit_program(d, os);
		std::string s = os.str();
		CHECK(s.find("State::q0") == std::string::npos);
		CHECK(s.find("class Trivial {") != std::string::npos);
		CHECK(s.find("outputs step(const inputs&") != std::string::npos);
	}

} // TEST_SUITE("cpp_codegen trivial solution (LG-5)")

// guard_to_cpp had no caller anywhere in the suite (coverage 2026-09-16):
// pin the token mapping it performs on HOA guard labels.
TEST_SUITE("cpp_codegen guard_to_cpp") {
	TEST_CASE("maps HOA guard syntax to a C++ expression over ap[]") {
		CHECK(codegen_detail::guard_to_cpp("0") == "ap[0]");
		CHECK(codegen_detail::guard_to_cpp("!0") == "!ap[0]");
		CHECK(codegen_detail::guard_to_cpp("0 & !1") == "ap[0]&&!ap[1]");
		CHECK(codegen_detail::guard_to_cpp("(0 | 1) & 12") == "(ap[0]||ap[1])&&ap[12]");
		CHECK(codegen_detail::guard_to_cpp("t") == "true");
		CHECK(codegen_detail::guard_to_cpp("f") == "false");
	}

	TEST_CASE("an empty guard is true and an unknown token passes through") {
		CHECK(codegen_detail::guard_to_cpp("") == "true");
		CHECK(codegen_detail::guard_to_cpp("  ") == "true");
		CHECK(codegen_detail::guard_to_cpp("0^1") == "ap[0]^ap[1]");
	}
}

TEST_SUITE("cpp_codegen sanitize") {
	TEST_CASE("maps a name to a C++ identifier") {
		CHECK(codegen_detail::sanitize("o_1") == "o_1");
		CHECK(codegen_detail::sanitize("a-b.c") == "a_b_c");
		CHECK(codegen_detail::sanitize("9x") == "_9x");
		CHECK(codegen_detail::sanitize("") == "_");
	}
}

static bool has(const std::string& s, const std::string& pat) {
	return s.find(pat) != std::string::npos;
}

// A hand-built program_desc reaches the emitter's shapes no small spec
// synthesizes: step guards next to inputs, a witness output beside a flag.
TEST_SUITE("cpp_codegen emit_program from a program_desc") {

	TEST_CASE("a flag-only program linking tau matches its step guards") {
		program_desc d;
		d.class_name = "Stepped";
		d.num_states = 1;
		d.inputs = { { "i", "i", field_kind::flag } };
		d.outputs = { { "o", "o", field_kind::flag } };
		d.step_guard_ks = { 3 };
		d.needs_tau_link = true;
		d.edges.resize(1);
		edge_desc e;
		e.guard = { 1, -1, 1 };
		d.edges[0].push_back(e);
		std::ostringstream os;
		auto r = emit_program(d, os);
		REQUIRE(r.has_value());
		const std::string s = os.str();
		CHECK(has(s, "#include \"codegen_strategy.h\""));
		CHECK(has(s, "namespace tau_codegen_detail = ::idni::tau_lang::codegen;"));
		CHECK(!has(s, "namespace tau_codegen_detail {"));
		CHECK(has(s, "\"__step_ge3\","));
		CHECK(has(s, "ap[1] = step_ >= 3;"));
		CHECK(has(s, "++step_;"));
		CHECK(has(s, "std::size_t step_ = 0;"));
		CHECK(has(s, "strat_.edges[0].push_back({{1,-1,1}, 0});"));
	}

	TEST_CASE("a witness-bearing program unrolls each edge's guard") {
		program_desc d;
		d.class_name = "Unrolled";
		d.num_states = 1;
		d.inputs = { { "i", "i", field_kind::flag } };
		d.outputs = { { "f", "f", field_kind::flag },
			{ "w", "w", field_kind::witness } };
		d.step_guard_ks = { 2 };
		d.needs_tau_link = true;
		d.edges.resize(1);
		edge_desc e0;
		e0.guard = { 1, -1, 1 };
		e0.witness_ctors = { { "w", "make_w()" } };
		edge_desc e1;
		e1.guard = { -1, 1, -1 };
		edge_desc e2;
		e2.guard = { 0, 0, 0 };
		d.edges[0] = { e0, e1, e2 };
		std::ostringstream os;
		auto r = emit_program(d, os);
		REQUIRE(r.has_value());
		const std::string s = os.str();
		CHECK(has(s, "if (in.i && !(step_ >= 2)) {"));
		CHECK(has(s, "o.f = true;"));
		CHECK(has(s, "static const tref w_s0_e0_w = make_w();"));
		CHECK(has(s, "o.w = w_s0_e0_w;"));
		CHECK(has(s, "else if (!in.i && (step_ >= 2)) {"));
		CHECK(has(s, "o.f = false;"));
		CHECK(has(s, "else if (true) {"));
		CHECK(has(s, "++step_;"));
		CHECK(has(s, "std::size_t step_ = 0;"));
		// a witness-bearing step() builds no strategy table
		CHECK(!has(s, "strat_"));
	}
}

#ifdef TAU_PACK_HAS_BA_BV
static tref parse_wff(const char* s) {
	tau::get_options opts;
	opts.parse.start = tau::wff;
	return tau::get(s, opts).value_or(nullptr);
}

TEST_SUITE("cpp_codegen atom ground expressions") {

	TEST_CASE("each order comparison is rebuilt with its own builder") {
		for (auto [src, fn] : std::initializer_list<
			std::pair<const char*, const char*>>{
			{ "o1[t]:bv[8] > i1[t]:bv[8]",   "build_bf_gt<" },
			{ "o1[t]:bv[8] !> i1[t]:bv[8]",  "build_bf_ngt<" },
			{ "o1[t]:bv[8] >= i1[t]:bv[8]",  "build_bf_gteq<" },
			{ "o1[t]:bv[8] !>= i1[t]:bv[8]", "build_bf_ngteq<" } })
		{
			INFO(std::string(src));
			tref atom = parse_wff(src);
			REQUIRE(atom != nullptr);
			auto e = codegen_detail::build_atom_ground_expr<node_t>(atom);
			REQUIRE(e.has_value());
			INFO(e.value());
			CHECK(has(e.value(), fn));
			CHECK(has(e.value(), "\"o1\""));
			CHECK(has(e.value(), "\"i1\""));
		}
	}

	TEST_CASE("each binary operator of an operand is rebuilt with its own builder") {
		for (auto [src, fn] : std::initializer_list<
			std::pair<const char*, const char*>>{
			{ "o1[t]:bv[8] ^ i1[t]:bv[8] = o2[t]:bv[8]",  "build_bf_xor<" },
			{ "o1[t]:bv[8] << i1[t]:bv[8] = o2[t]:bv[8]", "build_bf_shl<" },
			{ "o1[t]:bv[8] >> i1[t]:bv[8] = o2[t]:bv[8]", "build_bf_shr<" },
			{ "o1[t]:bv[8] - i1[t]:bv[8] = o2[t]:bv[8]",  "build_bf_sub<" },
			{ "o1[t]:bv[8] * i1[t]:bv[8] = o2[t]:bv[8]",  "build_bf_mul<" },
			{ "o1[t]:bv[8] / i1[t]:bv[8] = o2[t]:bv[8]",  "build_bf_div<" },
			{ "o1[t]:bv[8] % i1[t]:bv[8] = o2[t]:bv[8]",  "build_bf_mod<" } })
		{
			INFO(std::string(src));
			tref atom = parse_wff(src);
			REQUIRE(atom != nullptr);
			auto e = codegen_detail::build_atom_ground_expr<node_t>(atom);
			REQUIRE(e.has_value());
			INFO(e.value());
			CHECK(has(e.value(), fn));
			CHECK(has(e.value(), "build_bf_eq<"));
			CHECK(has(e.value(), "\"o2\""));
		}
	}

	TEST_CASE("an operand over a variable that is no stream is refused") {
		tref atom = parse_wff("x:bv[8] > o1[t]:bv[8]");
		REQUIRE(atom != nullptr);
		auto e = codegen_detail::build_atom_ground_expr<node_t>(atom);
		CHECK(!e.has_value());
		CHECK(report_has_code(e.report(), code::unsupported_operation));
	}

	TEST_CASE("a formula that is no comparison is refused") {
		tref atom = parse_wff("T");
		REQUIRE(atom != nullptr);
		auto e = codegen_detail::build_atom_ground_expr<node_t>(atom);
		CHECK(!e.has_value());
		CHECK(report_has_code(e.report(), code::unsupported_operation));
	}
}
#endif // TAU_PACK_HAS_BA_BV
