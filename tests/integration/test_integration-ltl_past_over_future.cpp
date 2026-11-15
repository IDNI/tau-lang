// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The identities rewrite_past_over_future applies (src/ltl_aba.h), each
// checked against the testers. With every input read as an output, `valid`
// decides a formula with past operators exactly, so `valid (a) <-> (b)` with
// the rewrite off is the oracle of `a ≡ b`: it reads the past operators of a
// through their testers and b as written. Streams o1, i1 and i2, type sbf.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "ltl_aba.h"
#include <sstream>

using namespace idni::tau_lang;

namespace {

const std::optional<bool> T = true, F = false;

struct rewrite_off {
	rewrite_off() { ltl_past_over_future_rewrite = false; }
	~rewrite_off() { ltl_past_over_future_rewrite = true; }
};

// The main formula of @p s, a formula without its closing dot.
tref fm(const std::string& s) {
	tref f = spec_main(s + ".");
	REQUIRE( f );
	return f;
}

// Whether a past operator over a future operand remains in @p f.
bool has_past_over_future(tref f) {
	return tau::get(f).find_top([](tref n) {
		const auto& t = tau::get(n);
		return (t.child_is(tau::wff_since) || t.child_is(tau::wff_trigger))
			&& past_operands_read_future<node_t>(t[0]); }) != nullptr;
}

// `valid (a) <-> (b)`, the past operators read through their testers.
std::optional<bool> equivalent(tref a, tref b) {
	rewrite_off off;
	auto r = api<node_t>::valid(tau::build_wff_equiv(a, b));
	if (!r.has_value()) {
		std::ostringstream why;
		r.report().print(idni::diagnostics::sinks{
			.error = &why, .warning = &why, .info = {} },
			term::colors(false));
		MESSAGE( why.str() );
		return std::nullopt;
	}
	return r.value();
}

// The rewrite of @p original removes every past operator over a future
// operand, keeps the language of @p original, and is @p expected.
void check_identity(const char* original, const char* expected) {
	tref o = fm(original);
	tref r = rewrite_past_over_future<node_t>(o, true);
	CHECK( r != o );
	CHECK( !has_past_over_future(r) );
	CHECK( equivalent(o, r) == T );
	CHECK( equivalent(r, fm(expected)) == T );
}

// No identity covers @p original: the rewrite keeps a past operator over a
// future operand, for the two bounds.
void check_kept(const char* original) {
	tref o = fm(original);
	tref r = rewrite_past_over_future<node_t>(o, true);
	CHECK( has_past_over_future(r) );
}

} // namespace

#define POF_CASE(name) TEST_CASE(name * doctest::skip(!ltlsynt_available()))

TEST_SUITE("past over future: the oracle") {

	POF_CASE("the oracle tells formulas apart") {
		CHECK( equivalent(fm("O (G (i1[t]:sbf = 1))"),
			fm("F (i1[t]:sbf = 1)")) == F );
		CHECK( equivalent(fm("G ((o1[t]:sbf = 1) -> O (F (i1[t]:sbf = 1)))"),
			fm("G ((o1[t]:sbf = 1) -> F (i1[t]:sbf = 1))")) == F );
	}
}

// φ S ψ ≡ ψ for persistent ψ, φ T ψ ≡ ψ for antipersistent ψ, at every step
TEST_SUITE("past over future: persistent and antipersistent operands") {

	POF_CASE("O (G χ) is G χ") {
		check_identity("O (G (i1[t]:sbf = 1))", "G (i1[t]:sbf = 1)");
	}
	POF_CASE("H (F χ) is F χ") {
		check_identity("H (F (i1[t]:sbf = 1))", "F (i1[t]:sbf = 1)");
	}
	POF_CASE("φ S (G χ) is G χ for any φ") {
		check_identity("(o1[t]:sbf = 1) S (G (i1[t]:sbf = 1))",
			"G (i1[t]:sbf = 1)");
		check_identity("(i2[t]:sbf = 1) S (G (i1[t]:sbf = o1[t]:sbf))",
			"G (i1[t]:sbf = o1[t]:sbf)");
	}
	POF_CASE("φ T (F χ) is F χ for any φ") {
		check_identity("(o1[t]:sbf = 1) T (F (i1[t]:sbf = 1))",
			"F (i1[t]:sbf = 1)");
	}
	POF_CASE("F of a persistent operand is persistent") {
		check_identity("O (F (G (i1[t]:sbf = 1)))", "F (G (i1[t]:sbf = 1))");
	}
	POF_CASE("G of an antipersistent operand is antipersistent") {
		check_identity("H (G (F (i1[t]:sbf = 1)))", "G (F (i1[t]:sbf = 1))");
	}
	POF_CASE("a Boolean combination of persistent operands is persistent") {
		check_identity("O ((G (i1[t]:sbf = 1)) || (G (o1[t]:sbf = 0)))",
			"(G (i1[t]:sbf = 1)) || (G (o1[t]:sbf = 0))");
		check_identity("O ((G (i1[t]:sbf = 1)) && (F (G (i2[t]:sbf = 0))))",
			"(G (i1[t]:sbf = 1)) && (F (G (i2[t]:sbf = 0)))");
	}
	POF_CASE("the identity holds under a future operator") {
		check_identity("G ((o1[t]:sbf = 1) -> O (G (i1[t]:sbf = i2[t]:sbf)))",
			"G ((o1[t]:sbf = 1) -> G (i1[t]:sbf = i2[t]:sbf))");
		check_identity("F (O (G (i1[t]:sbf = 1)))", "F (G (i1[t]:sbf = 1))");
		check_identity("(i2[t]:sbf = 1) W (O (G (i1[t]:sbf = 1)))",
			"(i2[t]:sbf = 1) W (G (i1[t]:sbf = 1))");
		check_identity("G ((i2[t]:sbf = 1) -> H (F (o1[t]:sbf = i1[t]:sbf)))",
			"G ((i2[t]:sbf = 1) -> F (o1[t]:sbf = i1[t]:sbf))");
	}
	POF_CASE("nested past operators are removed inside out") {
		check_identity("O (O (G (i1[t]:sbf = 1)))", "G (i1[t]:sbf = 1)");
		check_identity("H ((o1[t]:sbf = 1) T (F (i1[t]:sbf = 1)))",
			"F (i1[t]:sbf = 1)");
	}
}

// at step 0 a past operator is its operand: there is no step before it
TEST_SUITE("past over future: the root is step 0") {

	POF_CASE("O (F χ) at the root is F χ") {
		check_identity("O (F (i1[t]:sbf = 1))", "F (i1[t]:sbf = 1)");
	}
	POF_CASE("H (G χ) at the root is G χ") {
		check_identity("H (G (i1[t]:sbf = 1))", "G (i1[t]:sbf = 1)");
	}
	POF_CASE("φ S ψ and φ T ψ at the root are ψ") {
		check_identity("(o1[t]:sbf = 1) S (F (i1[t]:sbf = 1))",
			"F (i1[t]:sbf = 1)");
		check_identity("(o1[t]:sbf = 1) T (G (i1[t]:sbf = 1))",
			"G (i1[t]:sbf = 1)");
	}
	POF_CASE("under the root-level connectives") {
		check_identity("(G (o1[t]:sbf = 1)) || (O (F (i1[t]:sbf = 1)))",
			"(G (o1[t]:sbf = 1)) || (F (i1[t]:sbf = 1))");
		check_identity("!(O (F (i1[t]:sbf = 1)))", "!(F (i1[t]:sbf = 1))");
		check_identity("(F (o1[t]:sbf = 1)) && (H (G (i1[t]:sbf = 1)))",
			"(F (o1[t]:sbf = 1)) && (G (i1[t]:sbf = 1))");
	}
}

// G(β || O ψ) ≡ β W ψ and G(β || !O ψ) ≡ G(ψ -> G β), from step 0
TEST_SUITE("past over future: separation under a root-level always") {

	POF_CASE("G(α -> O ψ) is (!α) W ψ") {
		check_identity("G ((o1[t]:sbf = 1) -> O (F (i1[t]:sbf = 1)))",
			"(!(o1[t]:sbf = 1)) W (F (i1[t]:sbf = 1))");
		check_identity("G ((o1[t]:sbf = 1) -> O (F (i1[t]:sbf = 1)))",
			"(F (o1[t]:sbf = 1)) -> (F (i1[t]:sbf = 1))");
	}
	POF_CASE("G(α -> H ψ) is G((F α) -> ψ)") {
		check_identity("G ((i2[t]:sbf = 1) -> H (G (i1[t]:sbf = 1)))",
			"G ((F (i2[t]:sbf = 1)) -> G (i1[t]:sbf = 1))");
	}
	POF_CASE("G(O ψ -> β) is G(ψ -> G β)") {
		check_identity("G ((O (F (i1[t]:sbf = 1))) -> (o1[t]:sbf = 1))",
			"G ((F (i1[t]:sbf = 1)) -> G (o1[t]:sbf = 1))");
	}
	POF_CASE("G(H ψ -> β) is β W !ψ") {
		check_identity("G ((H (G (i1[t]:sbf = 1))) -> (o1[t]:sbf = 1))",
			"(o1[t]:sbf = 1) W (!(G (i1[t]:sbf = 1)))");
	}
	POF_CASE("β is the disjunction of the other disjuncts") {
		check_identity(
			"G ((o1[t]:sbf = 1) -> ((i2[t]:sbf = 1) || O (F (i1[t]:sbf = 1))))",
			"((!(o1[t]:sbf = 1)) || (i2[t]:sbf = 1)) W (F (i1[t]:sbf = 1))");
		check_identity(
			"G (((o1[t]:sbf = 1) || (i2[t]:sbf = 1)) -> O (F (i1[t]:sbf = 1)))",
			"(!((o1[t]:sbf = 1) || (i2[t]:sbf = 1))) W (F (i1[t]:sbf = 1))");
	}
	POF_CASE("with no other disjunct") {
		check_identity("G (O (F (i1[t]:sbf = 1)))", "F (i1[t]:sbf = 1)");
		check_identity("G (!(O (F (i1[t]:sbf = 1))))", "G (!(F (i1[t]:sbf = 1)))");
	}
	POF_CASE("a conjunction under the always is split") {
		check_identity(
			"G (((o1[t]:sbf = 1) -> O (F (i1[t]:sbf = 1))) && (o1[t]:sbf = i2[t]:sbf))",
			"((!(o1[t]:sbf = 1)) W (F (i1[t]:sbf = 1))) && (G (o1[t]:sbf = i2[t]:sbf))");
	}
}

TEST_SUITE("past over future: shapes no identity covers") {

	POF_CASE("O (F χ) under a weak until is kept") {
		check_kept("(o1[t]:sbf = 1) W (O (F (i1[t]:sbf = 1)))");
	}
	POF_CASE("a since with a condition under an always is kept") {
		check_kept("G ((o1[t]:sbf = 1) -> ((i2[t]:sbf = 1) S (F (i1[t]:sbf = 1))))");
	}
	POF_CASE("two past operators under one always are kept") {
		check_kept("G ((O (F (i1[t]:sbf = 1))) || (O (F (i2[t]:sbf = 1))))");
	}
	POF_CASE("a past operator over a future-free operand is not touched") {
		tref o = fm("G ((o1[t]:sbf = 1) -> O (i1[t]:sbf = 1))");
		CHECK( rewrite_past_over_future<node_t>(o, true) == o );
	}
}
