// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// parser/hoa.tgf: the HOA format as a tgf grammar. parse_hoa does not use it
// (see the note above parse_hoa in src/ltl_aba_synthesis.tmpl.h); these cases
// keep the grammar valid against Spot's output and against parse_hoa.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "hoa_parser.generated.h"

using namespace idni::tau_lang;

namespace {

using ptree = hoa_parser::tree;
using nt = hoa_parser_nonterminals;

tref hoa_tree(const std::string& s) {
	auto r = hoa_parser::instance().parse(s.c_str(), s.size());
	return r.found ? r.get_shaped_tree2() : nullptr;
}

bool hoa_parses(const std::string& s) {
	auto r = hoa_parser::instance().parse(s.c_str(), s.size());
	return r.found;
}

void collect(tref n, size_t k, trefs& out) {
	for (tref c : ptree::get(n).get_children()) {
		if (ptree::get(c).is_t()) continue;
		if (ptree::get(c).get_nt() == k) out.push_back(c);
		collect(c, k, out);
	}
}

size_t count(tref n, size_t k) {
	trefs out;
	collect(n, k, out);
	return out.size();
}

// ltlsynt's strategy for `G(a <-> X b)`, `--ins=a --outs=b`
const char* ltlsynt_hoa =
	"HOA: v1\n"
	"States: 2\n"
	"Start: 1\n"
	"AP: 2 \"a\" \"b\"\n"
	"acc-name: all\n"
	"Acceptance: 0 t\n"
	"properties: trans-labels explicit-labels state-acc deterministic weak\n"
	"controllable-AP: 1\n"
	"--BODY--\n"
	"State: 0\n"
	"[!0&!1] 0\n"
	"[0&!1] 1\n"
	"State: 1\n"
	"[!0&1] 0\n"
	"[0&1] 1\n"
	"--END--\n";

// `ltl2tgba -Hl 'a U b'`: the whole automaton on one line
const char* one_line_hoa =
	"HOA: v1 name: \"a U b\" States: 2 Start: 1 AP: 2 \"a\" \"b\" "
	"acc-name: Buchi Acceptance: 1 Inf(0) properties: trans-labels "
	"explicit-labels state-acc deterministic stutter-invariant terminal "
	"very-weak --BODY-- State: 0 {0} [t] 0 State: 1 [1] 0 [0&!1] 1 "
	"--END--\n";

// `ltl2tgba -H 'GFa & FG(b U c)'`: transition marks with several sets
const char* gen_buchi_hoa =
	"HOA: v1\n"
	"name: \"G(Fa & FG(b U c))\"\n"
	"States: 2\n"
	"Start: 0\n"
	"AP: 3 \"a\" \"b\" \"c\"\n"
	"acc-name: generalized-Buchi 2\n"
	"Acceptance: 2 Inf(0)&Inf(1)\n"
	"properties: trans-labels explicit-labels trans-acc stutter-invariant\n"
	"--BODY--\n"
	"State: 0\n"
	"[t] 0\n"
	"[2] 1\n"
	"State: 1\n"
	"[!0&1&!2] 1\n"
	"[!0&2] 1 {0}\n"
	"[0&1&!2] 1 {1}\n"
	"[0&2] 1 {0 1}\n"
	"--END--\n";

// `ltl2tgba -D -G -H 'GFa -> GFb'`: parity acceptance, Fin and |
const char* parity_hoa =
	"HOA: v1\n"
	"name: \"F(GFb | G!a)\"\n"
	"States: 1\n"
	"Start: 0\n"
	"AP: 2 \"b\" \"a\"\n"
	"acc-name: parity min even 2\n"
	"Acceptance: 2 Inf(0) | Fin(1)\n"
	"properties: trans-labels explicit-labels trans-acc complete\n"
	"properties: deterministic\n"
	"--BODY--\n"
	"State: 0\n"
	"[0] 0 {0}\n"
	"[!0&!1] 0\n"
	"[!0&1] 0 {1}\n"
	"--END--\n";

// What Spot does not print but the format allows: comments, an alias, an
// escaped quote, a conjunction of initial states, a state label, a state
// name, an empty mark set, an implicit edge label, a universal edge and an
// unknown header.
const char* full_syntax_hoa =
	"/* lead */ HOA: v1 /* after the version */\n"
	"States: 3\n"
	"Start: 0&1\n"
	"AP: 2 \"a\\\"q\" \"b\"\n"
	"Alias: @x 0 & !(1 | f)\n"
	"Acceptance: 3 (Fin(!0) | Inf(1)) & t & Fin(2)\n"
	"tool: \"spot\" \"2.16\"\n"
	"spot.highlight.edges: 1 2 3 4\n"
	"foo: t 12 \"s\" bar-baz\n"
	"properties:\n"
	"--BODY--\n"
	"State: [t] 0 \"init\" {0 2}\n"
	"1 {}\n"
	"[@x] 2\n"
	"State: 1\n"
	"[0] 1&2\n"
	"State: 2 {1}\n"
	"--END--\n";

// test_parse_hoa.cpp's well-formed strategy
const char* small_hoa =
	"HOA: v1\n"
	"States: 2\n"
	"Start: 0\n"
	"AP: 2 \"p0\" \"p1\"\n"
	"acc-name: all\n"
	"Acceptance: 0 t\n"
	"--BODY--\n"
	"State: 0\n"
	"[0&1] 1\n"
	"[!0] 0\n"
	"State: 1 {0}\n"
	"[t] 1\n"
	"--END--\n";

std::string with(const std::string& text, const std::string& from,
	const std::string& to)
{
	std::string s = text;
	size_t at = s.find(from);
	REQUIRE(at != std::string::npos);
	return s.replace(at, from.size(), to);
}

} // namespace

TEST_SUITE("hoa grammar") {

	TEST_CASE("[HOA-G-01] Spot's outputs parse") {
		for (const char* s : { ltlsynt_hoa, one_line_hoa, gen_buchi_hoa,
			parity_hoa, small_hoa })
		{
			const std::string text = s;
			CAPTURE(text);
			CHECK(hoa_parses(text));
		}
	}

	TEST_CASE("[HOA-G-02] every construct of the format parses") {
		tref t = hoa_tree(full_syntax_hoa);
		REQUIRE(t != nullptr);
		CHECK(count(t, nt::state) == 3);
		CHECK(count(t, nt::edge) == 3);
		CHECK(count(t, nt::alias) == 1);
		CHECK(count(t, nt::l_alias) == 1);
		CHECK(count(t, nt::acc_fin) == 2);
		CHECK(count(t, nt::acc_set_neg) == 1);
		CHECK(count(t, nt::other_header) == 2);
		CHECK(count(t, nt::comment) == 0); // trimmed with _
	}

	TEST_CASE("[HOA-G-03] the tree counts agree with parse_hoa") {
		// parse_hoa reads one item per line, as Spot prints by default
		CHECK_FALSE(parse_hoa(one_line_hoa).has_value());
		for (const char* s : { ltlsynt_hoa, gen_buchi_hoa,
			parity_hoa, small_hoa })
		{
			const std::string text = s;
			CAPTURE(text);
			tref t = hoa_tree(text);
			REQUIRE(t != nullptr);
			auto r = parse_hoa(s);
			REQUIRE(r.has_value());
			const hoa_automaton& aut = r.value();
			size_t edges = 0;
			for (const auto& out : aut.edges) edges += out.size();
			CHECK(count(t, nt::state) == aut.num_states);
			CHECK(count(t, nt::edge) == edges);
			trefs aps;
			collect(t, nt::aps, aps);
			REQUIRE(aps.size() == 1);
			CHECK(count(aps[0], nt::string) == aut.aps.size());
		}
	}

	TEST_CASE("[HOA-G-04] labels keep the precedence ! & |") {
		tref t = hoa_tree(with(small_hoa, "[0&1] 1", "[!0 | 0&1] 1"));
		REQUIRE(t != nullptr);
		trefs ors;
		collect(t, nt::l_or, ors);
		REQUIRE(ors.size() == 1);
		CHECK(count(ors[0], nt::l_and) == 1);
		CHECK(count(ors[0], nt::l_not) == 1);
	}

	TEST_CASE("[HOA-G-05] text that is not an automaton is refused") {
		const std::string g = small_hoa;
		for (const std::string& s : {
			std::string(""),
			with(g, "--BODY--\n", ""),               // no body marker
			with(g, "--END--\n", ""),                // truncated
			with(g, "HOA: v1\n", ""),                // no version
			with(g, "[0&1] 1", "[0&] 1"),            // broken label
			with(g, "[0&1] 1", "[0&1 1"),            // unclosed label
			with(g, "[0&1] 1", "[x] 1"),             // label not over AP indices
			with(g, "[0&1] 1", "[0&1] 01"),          // leading zero
			with(g, "[0&1] 1", "[0&1] -1"),          // negative target
			with(g, "State: 1 {0}", "State: 1 {0"),  // unclosed marks
			with(g, "\"p0\"", "\"p0"),               // unclosed string
			with(g, "acc-name: all", "acc-name all"),// header without ':'
			with(g, "Acceptance: 0 t", "Acceptance: 0 Inf 0"),
			g + "trailing\n",                        // text after --END--
			with(g, "State: 0\n", "/* /* nested */ */ State: 0\n"),
		}) {
			CAPTURE(s);
			CHECK_FALSE(hoa_parses(s));
		}
	}

	TEST_CASE("[HOA-G-06] adjacent items need a separator") {
		// `1 2` is two implicit edges, `12` one target; `0 1` cannot
		// shrink to `01`
		const std::string g = small_hoa;
		tref two = hoa_tree(with(g, "[t] 1\n", "1 1\n"));
		REQUIRE(two != nullptr);
		CHECK(count(two, nt::edge) == 4);
		tref one = hoa_tree(with(g, "[t] 1\n", "11\n"));
		REQUIRE(one != nullptr);
		CHECK(count(one, nt::edge) == 3);
		CHECK_FALSE(hoa_parses(with(g, "\"p0\" \"p1\"", "\"p0\"\"p1\"")));
		CHECK(hoa_parses(with(g, "\"p0\" \"p1\"", "\"p0\"/**/\"p1\"")));
	}

	TEST_CASE("[HOA-G-07] t and f are constants, not identifiers") {
		const std::string g = small_hoa;
		CHECK(hoa_parses(with(g, "acc-name: all", "acc-name: all t f")));
		CHECK_FALSE(hoa_parses(with(g, "HOA: v1", "HOA: t")));
		CHECK(hoa_parses(with(g, "HOA: v1", "HOA: tf")));
	}
}
