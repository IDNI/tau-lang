// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The equivalence of two formulas decided by their conjuncts
// (satisfiability.h, `equivalence_by_parts`) against the universally closed
// equivalence put to the normalizer as one question. The two must give one
// answer wherever the one question is decided: on pairs written for a
// reason, and on pairs drawn at random: a formula and the formula altered,
// which keeps or changes its meaning, and a formula and its mirror.

#include "test_integration-interpreter_helper.h"

namespace {

struct answers { int parts, whole; };

answers decide(const std::string& a, const std::string& b) {
	tref x = create_spec((a + ".").c_str());
	tref y = create_spec((b + ".").c_str());
	return { closed_equivalence_by_parts<node_t>(x, y),
		closed_equivalence_whole<node_t>(x, y) };
}

bool both(const std::string& a, const std::string& b, int expected) {
	const answers d = decide(a, b);
	return d.parts == expected && d.whole == expected;
}

// A stream occurrence and a constant of bv[8].
std::string occ(size_t stream, size_t back) {
	return "o" + std::to_string(stream) + (back
		? "[t-" + std::to_string(back) + "]" : std::string("[t]"))
		+ ":bv[8]";
}
std::string num(size_t c) {
	return "{ " + std::to_string(c) + " }:bv[8]";
}

// An atom: a relation between a term and a term or a constant. Mirrored, it
// is written with the operands of what commutes exchanged.
struct atom_t {
	std::string x, op, y, rel, z;
	std::string text(bool mirrored) const {
		const std::string term = op.empty() ? x
			: "(" + (mirrored ? y + " " + op + " " + x
				: x + " " + op + " " + y) + ")";
		return mirrored && (rel == "=" || rel == "!=")
			? z + " " + rel + " " + term : term + " " + rel + " " + z;
	}
};

// A clause: an atom, a disjunction of two, or a disjunction of one with a
// conjunction of two.
struct clause_t {
	std::vector<atom_t> atoms;
	std::string text(bool mirrored) const {
		const auto a = [&](size_t i) { return atoms[i].text(mirrored); };
		if (atoms.size() == 1) return "(" + a(0) + ")";
		if (atoms.size() == 2) return mirrored
			? "(" + a(1) + " || " + a(0) + ")"
			: "(" + a(0) + " || " + a(1) + ")";
		return mirrored
			? "((" + a(2) + " && " + a(1) + ") || " + a(0) + ")"
			: "(" + a(0) + " || (" + a(1) + " && " + a(2) + "))";
	}
};

struct drawing {
	std::mt19937 rnd;
	explicit drawing(unsigned seed) : rnd(seed) {}
	size_t below(size_t n) {
		return std::uniform_int_distribution<size_t>(0, n - 1)(rnd);
	}
	std::string variable() { return occ(1 + below(4), below(2)); }
	atom_t atom() {
		atom_t a;
		a.x = variable();
		if (below(3) == 0) {
			static const char* ops[] = { "&", "|", "^", "+" };
			a.op = ops[below(4)];
			a.y = below(4) == 0 ? a.x : variable();
		}
		switch (below(5)) {
		case 0: a.rel = "="; a.z = variable(); break;
		case 1: a.rel = "!="; a.z = num(below(4)); break;
		case 2: a.rel = "<"; a.z = num(1 + below(4)); break;
		default: a.rel = "="; a.z = num(below(4)); break;
		}
		return a;
	}
	clause_t clause() {
		clause_t c;
		const size_t n = below(4);
		for (size_t i = n == 0 ? 2 : n == 1 ? 3 : 1; i; --i)
			c.atoms.push_back(atom());
		return c;
	}
	std::vector<clause_t> clauses() {
		std::vector<clause_t> cs;
		for (size_t n = 3 + below(4); n; --n) cs.push_back(clause());
		return cs;
	}
};

std::string conjoined(const std::vector<clause_t>& cs, bool mirrored = false) {
	std::string f;
	for (const clause_t& c : cs)
		f += (f.empty() ? "" : " && ") + c.text(mirrored);
	return f;
}

// The clauses altered: their order changed, one of them dropped, one of
// them stated twice, a constant or a relation of one of them changed, or
// one more clause.
std::vector<clause_t> altered(drawing& g, std::vector<clause_t> cs) {
	switch (g.below(6)) {
	case 0: std::shuffle(cs.begin(), cs.end(), g.rnd); break;
	case 1: cs.erase(cs.begin() + g.below(cs.size())); break;
	case 2: cs.push_back(cs[g.below(cs.size())]);
		std::shuffle(cs.begin(), cs.end(), g.rnd); break;
	case 3: {
		atom_t& a = cs[g.below(cs.size())].atoms[0];
		a.z = num(g.below(4));
		break; }
	case 4: {
		atom_t& a = cs[g.below(cs.size())].atoms[0];
		a.rel = a.rel == "=" ? "!=" : "=";
		break; }
	default: cs.push_back(g.clause()); break;
	}
	return cs;
}

// The number of pairs of the random cases and their seed; the environment
// variables TAU_EQUIVALENCE_PAIRS and TAU_EQUIVALENCE_SEED override them.
size_t pairs(size_t by_default) {
	const char* v = std::getenv("TAU_EQUIVALENCE_PAIRS");
	return v && *v ? std::stoul(v) : by_default;
}
unsigned seed(unsigned by_default) {
	const char* v = std::getenv("TAU_EQUIVALENCE_SEED");
	return v && *v ? unsigned(std::stoul(v)) : by_default;
}

const std::string a1 = occ(1, 0), a2 = occ(2, 0), a3 = occ(3, 0),
	b1 = occ(1, 1);

} // namespace

TEST_SUITE("configuration") {
	TEST_CASE("bdd_init") { bdd_init<Bool>(); }
	TEST_CASE("logging") { logging::trace(); }
}

TEST_SUITE("equivalence by parts: what the writing settles") {

	TEST_CASE("the order of the conjuncts") {
		CHECK(both("(" + a1 + " = " + num(1) + ") && (" + a2 + " < " + num(3) + ")",
			"(" + a2 + " < " + num(3) + ") && (" + a1 + " = " + num(1) + ")", 1));
	}

	TEST_CASE("the sides of an equation and of an inequation") {
		CHECK(both("(" + a1 + " = " + a2 + ") && (" + a3 + " != " + a2 + ")",
			"(" + a2 + " = " + a1 + ") && (" + a2 + " != " + a3 + ")", 1));
	}

	TEST_CASE("the order of the disjuncts, nested") {
		CHECK(both("(" + a1 + " < " + num(2) + " || (" + a2 + " = " + num(1) + " && " + a3 + " = " + num(0) + "))",
			"((" + a3 + " = " + num(0) + " && " + a2 + " = " + num(1) + ") || " + a1 + " < " + num(2) + ")", 1));
	}

	TEST_CASE("an equation of a conjunction applied to another conjunct") {
		CHECK(both("(" + a1 + " = " + b1 + ") && (" + a2 + " < " + a1 + ")",
			"(" + a1 + " = " + b1 + ") && (" + a2 + " < " + b1 + ")", 1));
	}

	TEST_CASE("an equation with a constant applied to another conjunct") {
		CHECK(both("(" + a1 + " = " + num(5) + ") && (" + a2 + " < " + a1 + ")",
			"(" + a1 + " = " + num(5) + ") && (" + a2 + " < " + num(5) + ")", 1));
	}

	TEST_CASE("an equation inside a branch applied inside that branch") {
		CHECK(both("(" + a3 + " = " + num(0) + " || (" + a1 + " = " + a2 + " && " + a1 + " != " + num(3) + "))",
			"(" + a3 + " = " + num(0) + " || (" + a1 + " = " + a2 + " && " + a2 + " != " + num(3) + "))", 1));
	}

	TEST_CASE("an inequation of a disjunction applied to another disjunct") {
		CHECK(both("(" + a1 + " != " + a2 + " || " + a1 + " < " + num(3) + ")",
			"(" + a1 + " != " + a2 + " || " + a2 + " < " + num(3) + ")", 1));
	}

	TEST_CASE("the operands of the term operators that commute") {
		CHECK(both("((" + a1 + " & " + a2 + ") = " + num(1) + ") && ((" + a2 + " | " + a3 + ") != " + num(0) + ") && ((" + a1 + " ^ " + a3 + ") = " + num(2) + ")",
			"((" + a2 + " & " + a1 + ") = " + num(1) + ") && ((" + a3 + " | " + a2 + ") != " + num(0) + ") && ((" + a3 + " ^ " + a1 + ") = " + num(2) + ")", 1));
	}

	TEST_CASE("a term stated twice: once under and and or, twice under xor") {
		CHECK(both("((" + a1 + " & " + a1 + ") = " + num(1) + ")", "(" + a1 + " = " + num(1) + ")", 1));
		CHECK(both("((" + a1 + " ^ " + a1 + " ^ " + a2 + ") = " + num(1) + ")", "(" + a2 + " = " + num(1) + ")", 1));
		CHECK(both("((" + a1 + " ^ " + a1 + " ^ " + a2 + ") = " + num(1) + ")", "((" + a1 + " ^ " + a2 + ") = " + num(1) + ")", 0));
	}

	TEST_CASE("an inequation of a term with itself") {
		CHECK(both("(" + a1 + " != " + a1 + " || " + a2 + " = " + num(1) + ")",
			"(" + a2 + " = " + num(1) + ")", 1));
		CHECK(both("(" + a1 + " != " + a1 + " || " + a2 + " = " + num(1) + ")",
			"(" + a2 + " = " + num(2) + ")", 0));
	}

	TEST_CASE("a conjunct that an equation makes true") {
		CHECK(both("(" + a1 + " = " + num(5) + ") && (" + a1 + " = " + num(5) + " || " + a2 + " = " + num(1) + ")",
			"(" + a1 + " = " + num(5) + ")", 1));
	}
}

TEST_SUITE("equivalence by parts: what the normalizer settles") {

	TEST_CASE("a conjunct implied by another one") {
		CHECK(both("(" + a1 + " < " + num(4) + ") && (" + a1 + " < " + num(8) + ")",
			"(" + a1 + " < " + num(4) + ")", 1));
	}

	TEST_CASE("a conjunct implied through a second variable") {
		CHECK(both("(" + a1 + " < " + a2 + ") && (" + a2 + " < " + num(4) + ") && (" + a1 + " < " + num(4) + ")",
			"(" + a1 + " < " + a2 + ") && (" + a2 + " < " + num(4) + ")", 1));
	}

	TEST_CASE("another constant") {
		CHECK(both("(" + a1 + " = " + num(5) + ")", "(" + a1 + " = " + num(6) + ")", 0));
	}

	TEST_CASE("a conjunct the other formula does not imply") {
		CHECK(both("(" + a1 + " = " + num(1) + ") && (" + a2 + " < " + num(3) + ")",
			"(" + a1 + " = " + num(1) + ")", 0));
	}

	TEST_CASE("an equation applied where the other formula does not state it") {
		CHECK(both("(" + a1 + " = " + b1 + ") && (" + a2 + " < " + a1 + ")",
			"(" + a2 + " < " + b1 + ")", 0));
	}

	TEST_CASE("a disjunction against the conjunction of its operands") {
		CHECK(both("(" + a1 + " = " + num(1) + " || " + a2 + " = " + num(2) + ")",
			"(" + a1 + " = " + num(1) + ") && (" + a2 + " = " + num(2) + ")", 0));
	}

	TEST_CASE("an equation of a branch does not reach the other branch") {
		CHECK(both("((" + a1 + " = " + a2 + " && " + a3 + " = " + num(1) + ") || " + a1 + " < " + num(3) + ")",
			"((" + a1 + " = " + a2 + " && " + a3 + " = " + num(1) + ") || " + a2 + " < " + num(3) + ")", 0));
	}
}

TEST_SUITE("equivalence by parts: what the writing leaves as it is written") {

	const std::string x = "x:bv[8]";

	TEST_CASE("a variable bound by a functional quantifier") {
		CHECK(both("(" + x + " = " + num(0) + ") && ((fex x (" + x + " | " + a1 + ")) = " + num(255) + ")",
			"(" + x + " = " + num(0) + ") && ((fex x (" + num(0) + " | " + a1 + ")) = " + num(255) + ")", 0));
	}

	TEST_CASE("a variable bound by a quantifier") {
		CHECK(both("(" + x + " = " + num(0) + ") && (all x ((" + x + " & " + a1 + ") = " + num(0) + "))",
			"(" + x + " = " + num(0) + ") && (all x ((" + num(0) + " & " + a1 + ") = " + num(0) + "))", 0));
	}

	TEST_CASE("the sides of an order") {
		CHECK(both("(" + a1 + " < " + a2 + ")", "(" + a2 + " < " + a1 + ")", 0));
	}

	TEST_CASE("a chain of equations") {
		CHECK(both("(" + a1 + " = " + a2 + ") && (" + a2 + " = " + a3 + ") && (" + a3 + " < " + num(3) + ")",
			"(" + a1 + " = " + a2 + ") && (" + a2 + " = " + a3 + ") && (" + a1 + " < " + num(3) + ")", 1));
	}

	TEST_CASE("two constants in one class") {
		CHECK(both("(" + a1 + " = " + num(1) + ") && (" + a1 + " = " + a2 + ") && (" + a2 + " = " + num(2) + ")",
			"F", 1));
	}

	TEST_CASE("an equation of a disjunct does not reach the other disjunct") {
		CHECK(both("(" + a1 + " = " + a2 + " || " + a1 + " < " + num(3) + ")",
			"(" + a1 + " = " + a2 + " || " + a2 + " < " + num(3) + ")", 0));
	}
}

TEST_SUITE("equivalence by parts: against the one question") {

	TEST_CASE("formulas drawn at random, altered and mirrored") {
		drawing g(seed(20260929));
		const size_t n = pairs(400);
		size_t equivalent = 0, different = 0, undecided = 0;
		for (size_t i = 0; i < n; ++i) {
			const std::vector<clause_t> cs = g.clauses();
			const std::string a = conjoined(cs);
			const std::string b = conjoined(altered(g, cs), g.below(2) == 0);
			const answers d = decide(a, b);
			if (d.whole < 0) { ++undecided; continue; }
			CAPTURE(a);
			CAPTURE(b);
			CHECK(d.parts == d.whole);
			(d.whole == 1 ? equivalent : different)++;
		}
		const std::string counts = "pairs " + std::to_string(n)
			+ ", equivalent " + std::to_string(equivalent)
			+ ", different " + std::to_string(different)
			+ ", undecided " + std::to_string(undecided);
		MESSAGE(counts);
		// both answers occur, and the one question is decided
		CHECK(equivalent * 8 >= n);
		CHECK(different * 8 >= n);
		CHECK(undecided == 0);
	}

	TEST_CASE("a formula and its mirror") {
		drawing g(seed(424242));
		const size_t n = pairs(200);
		for (size_t i = 0; i < n; ++i) {
			const std::vector<clause_t> cs = g.clauses();
			const std::string a = conjoined(cs), b = conjoined(cs, true);
			CAPTURE(a);
			CAPTURE(b);
			const answers d = decide(a, b);
			CHECK(d.whole == 1);
			CHECK(d.parts == 1);
		}
	}

	// `equivalence_by_parts_mode` lets the environment variable
	// TAU_EQUIVALENCE_BY_PARTS override the flag; run with it unset.
	TEST_CASE("the shadow of the switch counts no disagreement") {
		struct shadow_mode {
			int saved = equivalence_by_parts;
			shadow_mode() {
				equivalence_by_parts = 2;
				equivalence_by_parts_mismatches = 0;
			}
			~shadow_mode() { equivalence_by_parts = saved; }
		} shadow;
		REQUIRE(equivalence_by_parts_mode() == 2);
		const size_t calls = closed_equivalence_calls;
		const size_t questions = closed_equivalence_questions;
		drawing g(seed(177));
		for (size_t n = pairs(100); n; --n) {
			const std::vector<clause_t> cs = g.clauses();
			tref x = create_spec((conjoined(cs) + ".").c_str());
			tref y = create_spec((conjoined(altered(g, cs)) + ".").c_str());
			const int whole = closed_equivalence_whole<node_t>(x, y);
			CHECK(closed_equivalence<node_t>(x, y) == whole);
		}
		CHECK(closed_equivalence_calls > calls);
		CHECK(closed_equivalence_questions > questions);
		CHECK(equivalence_by_parts_mismatches == 0);
	}
}
