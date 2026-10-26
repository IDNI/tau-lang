// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// hoa_label: the one reader of HOA edge labels and the folds over it. Every
// consumer (parse_hoa, the game reader, the ABA oracle, the product game,
// minimisation, code generation) reads a label here, so the folds must agree
// on every label and every reader must refuse the same texts.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "ltl_aba.h"

#include <random>

using namespace idni::tau_lang;
using hoa_label::kind;

namespace {

// The label @p g printed back fully parenthesised, to pin its structure.
std::string shape(const std::string& g) {
	auto e = hoa_label::read(g);
	REQUIRE(e.has_value());
	return hoa_label::fold<std::string>(*e,
		[](const hoa_label::node& n, std::vector<std::string>& v)
			-> std::string
	{
		switch (n.k) {
		case kind::tt:   return "t";
		case kind::ff:   return "f";
		case kind::ap:   return std::to_string(n.ap);
		case kind::neg:  return "!" + v[n.lhs];
		case kind::conj: return "(" + v[n.lhs] + "&" + v[n.rhs] + ")";
		case kind::disj: return "(" + v[n.lhs] + "|" + v[n.rhs] + ")";
		}
		return "?";
	});
}

bool value(const hoa_label::expr& e, size_t bits) {
	return hoa_label::eval(e, [&](size_t ap) { return (bits >> ap) & 1; });
}

bool dnf_value(const hoa_label::dnf& d, size_t bits) {
	for (const auto& c : d) {
		bool all = true;
		for (const auto& l : c)
			if ((((bits >> l.ap) & 1) != 0) != l.pos) { all = false; break; }
		if (all) return true;
	}
	return false;
}

// A random label over @p n APs, @p depth levels deep, with blanks.
std::string random_label(std::mt19937& g, size_t n, int depth) {
	std::uniform_int_distribution<int> pick(0, depth > 0 ? 6 : 2);
	switch (pick(g)) {
	case 0: return std::to_string(g() % n);
	case 1: return "!" + std::to_string(g() % n);
	case 2: return g() % 2 ? "t" : "f";
	case 3: return "!" + random_label(g, n, depth - 1);
	case 4: return random_label(g, n, depth - 1) + " & "
		+ random_label(g, n, depth - 1);
	case 5: return random_label(g, n, depth - 1) + "|"
		+ random_label(g, n, depth - 1);
	default: return "(" + random_label(g, n, depth - 1) + ")";
	}
}

} // namespace

TEST_SUITE("hoa_label") {

	TEST_CASE("[HL-01] ! binds before &, & before |, and both group left") {
		CHECK(shape("0") == "0");
		CHECK(shape("!0&1") == "(!0&1)");
		CHECK(shape("0&1|2") == "((0&1)|2)");
		CHECK(shape("0|1&2") == "(0|(1&2))");
		CHECK(shape("0&1&2") == "((0&1)&2)");
		CHECK(shape("0|1|2") == "((0|1)|2)");
		CHECK(shape("!(0|1)&2") == "(!(0|1)&2)");
		CHECK(shape("!!0") == "!!0");
		CHECK(shape("!t | f") == "(!t|f)");
		CHECK(shape("(((0)))") == "0");
	}

	TEST_CASE("[HL-02] blanks are free and an empty label is t") {
		CHECK(shape(" \t0 &\t!1 | ( 2 ) ") == "((0&!1)|2)");
		CHECK(shape("") == "t");
		CHECK(shape("  \t ") == "t");
	}

	TEST_CASE("[HL-03] text that is not a label is refused") {
		for (const char* g : { "0&", "&0", "0|", "|0", "!", "0!", "0 1",
			"(0", "0)", "()", "(0|1))", "!(0", "x", "0&x", "T", "-1",
			"0 & & 1", "[0]", "1234567890", "0 (1)", ")(" })
		{
			CAPTURE(g);
			CHECK_FALSE(hoa_label::read(g).has_value());
		}
		CHECK(hoa_label::read("123456789").has_value());
	}

	TEST_CASE("[HL-04] a deep label reads without recursion") {
		const size_t depth = 200000;
		std::string nested = std::string(depth, '(') + "0"
			+ std::string(depth, ')');
		auto e = hoa_label::read(nested);
		REQUIRE(e.has_value());
		CHECK(value(*e, 1));
		std::string negs = std::string(depth, '!') + "0";
		auto n = hoa_label::read(negs);
		REQUIRE(n.has_value());
		CHECK(value(*n, 1)); // an even number of negations
		CHECK(hoa_label::to_dnf(*n).has_value());
	}

	TEST_CASE("[HL-05] ap_bound is one past the largest index") {
		CHECK(hoa_label::ap_bound(*hoa_label::read("t")) == 0);
		CHECK(hoa_label::ap_bound(*hoa_label::read("0")) == 1);
		CHECK(hoa_label::ap_bound(*hoa_label::read("3 & !7 | 1")) == 8);
	}

	TEST_CASE("[HL-06] eval, DNF and truth table agree on every label") {
		std::mt19937 g(20261026);
		const size_t n = 5;
		for (int k = 0; k < 400; ++k) {
			std::string text = random_label(g, n, 4);
			CAPTURE(text);
			auto e = hoa_label::read(text);
			REQUIRE(e.has_value());
			auto d = hoa_label::to_dnf(*e, 0);
			REQUIRE(d.has_value());
			auto t = strategy_minimize_detail::guard_table(text, n);
			REQUIRE(t.has_value());
			for (size_t bits = 0; bits < (size_t{ 1 } << n); ++bits) {
				CAPTURE(bits);
				bool v = value(*e, bits);
				CHECK(dnf_value(*d, bits) == v);
				CHECK((((*t)[bits / 64] >> (bits % 64)) & 1) == v);
			}
		}
	}

	TEST_CASE("[HL-07] the readers refuse the same texts") {
		for (const char* g : { "0&", "(0", "x", "0 1", "!" }) {
			CAPTURE(g);
			CHECK_FALSE(hoa_label::to_dnf(g).has_value());
			CHECK_FALSE(strategy_minimize_detail::guard_table(g, 2)
				.has_value());
		}
	}

	TEST_CASE("[HL-08] sum_of_products keeps the products as written") {
		auto sop = [](const std::string& g) {
			auto e = hoa_label::read(g);
			REQUIRE(e.has_value());
			return hoa_label::sum_of_products(*e);
		};
		auto p = sop("0&!1 | !t & 2 | f");
		REQUIRE(p.has_value());
		REQUIRE(p->size() == 3);
		std::vector<std::string> texts;
		for (const auto& product : *p) {
			std::string s;
			for (const auto& l : product)
				s += (s.empty() ? "" : "&") + l.str();
			texts.push_back(s);
		}
		CHECK(texts == std::vector<std::string>{ "0&!1", "!t&2", "f" });
		CHECK(sop("t").has_value());
		CHECK_FALSE(sop("!(0|1)").has_value());
		CHECK_FALSE(sop("(0|1)&2").has_value());
		CHECK_FALSE(sop("!!0").has_value());
	}
}

TEST_SUITE("hoa_label in parse_hoa") {

	const std::string head =
		"HOA: v1\nStates: 1\nStart: 0\nAP: 2 \"p0\" \"p1\"\n"
		"acc-name: all\nAcceptance: 0 t\n--BODY--\nState: 0\n";

	TEST_CASE("[HL-10] every label shape reaches the automaton") {
		for (const char* g : { "t", "!t", "!(0 | 1)", "(0|1) & !1",
			"0&!1 | !0&1", "" })
		{
			CAPTURE(g);
			auto r = parse_hoa(head + "[" + g + "] 0\n--END--\n");
			REQUIRE(r.has_value());
			REQUIRE(r.value().edges[0].size() == 1);
			CHECK(r.value().edges[0][0].guard_label == g);
		}
	}

	TEST_CASE("[HL-11] a label that is not one, or names an AP outside "
	          "the table, refuses the automaton") {
		for (const char* g : { "0&", "(0", "x", "2", "0 & 5",
			"99999999999" })
		{
			CAPTURE(g);
			CHECK(parse_hoa(head + "[" + g + "] 0\n--END--\n")
				.has_error());
		}
	}
}
