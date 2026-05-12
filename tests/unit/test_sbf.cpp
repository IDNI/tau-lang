// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Comprehensive tests for sbf_ba — simple Boolean function Boolean algebra
// represented by BDDs (hbdd<Bool>).
// 200+ tests covering: construction, is_zero/is_one, BA operations,
// algebraic laws, splitter, normalize_sbf, and parse_sbf.

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "boolean_algebras/sbf_ba.h"

using namespace idni::tau_lang;

// --- helpers ---

static void init_bdd() { bdd_init<Bool>(); }

static sbf_ba sone()  { init_bdd(); return bdd_handle<Bool>::htrue;  }
static sbf_ba szero() { init_bdd(); return bdd_handle<Bool>::hfalse; }

// positive literal for variable v (v > 0)
static sbf_ba svar(int_t v) {
	init_bdd();
	return bdd_handle<Bool>::bit(true, v);
}

// negative literal for variable v
static sbf_ba sneg(int_t v) {
	init_bdd();
	return bdd_handle<Bool>::bit(false, v);
}

// parse an sbf expression string and extract the sbf_ba value
static sbf_ba spar(const char* src) {
	auto r = parse_sbf<tau_ba<bv, sbf_ba>, bv, sbf_ba>(src);
	assert(r.has_value());
	return std::get<sbf_ba>(r.value().first);
}

// ============================================================================
TEST_SUITE("sbf — configuration") {
// ============================================================================

TEST_CASE("bdd_init") {
	bdd_init<Bool>();
}

} // TEST_SUITE configuration

// ============================================================================
TEST_SUITE("sbf — basic construction") {
// ============================================================================

TEST_CASE("one is not zero") {
	CHECK(sone() != szero());
}

TEST_CASE("one equals bool true") {
	CHECK(sone() == true);
}

TEST_CASE("zero equals bool false") {
	CHECK(szero() == false);
}

TEST_CASE("one is not bool false") {
	CHECK_FALSE(sone() == false);
}

TEST_CASE("zero is not bool true") {
	CHECK_FALSE(szero() == true);
}

TEST_CASE("htrue and htrue are equal") {
	CHECK(sone() == sone());
}

TEST_CASE("hfalse and hfalse are equal") {
	CHECK(szero() == szero());
}

TEST_CASE("positive variable is not zero") {
	CHECK(svar(1) != szero());
}

TEST_CASE("positive variable is not one") {
	CHECK(svar(1) != sone());
}

TEST_CASE("two positive variables with different ids are different") {
	CHECK(svar(1) != svar(2));
}

TEST_CASE("same variable id creates identical object") {
	CHECK(svar(3) == svar(3));
}

TEST_CASE("negative literal is not zero") {
	CHECK(sneg(1) != szero());
}

TEST_CASE("negative literal is not one") {
	CHECK(sneg(1) != sone());
}

TEST_CASE("positive and negative literals of same var are different") {
	CHECK(svar(1) != sneg(1));
}

TEST_CASE("variables with ids 1 through 5 are all distinct") {
	for (int_t i = 1; i <= 5; ++i)
		for (int_t j = i + 1; j <= 5; ++j)
			CHECK(svar(i) != svar(j));
}

TEST_CASE("negative literals with ids 1 through 5 are all distinct") {
	for (int_t i = 1; i <= 5; ++i)
		for (int_t j = i + 1; j <= 5; ++j)
			CHECK(sneg(i) != sneg(j));
}

} // TEST_SUITE basic construction

// ============================================================================
TEST_SUITE("sbf — is_sbf_zero / is_sbf_one") {
// ============================================================================

TEST_CASE("is_sbf_zero on zero") {
	CHECK(is_sbf_zero(szero()) == true);
}

TEST_CASE("is_sbf_one on one") {
	CHECK(is_sbf_one(sone()) == true);
}

TEST_CASE("is_sbf_zero on one") {
	CHECK(is_sbf_zero(sone()) == false);
}

TEST_CASE("is_sbf_one on zero") {
	CHECK(is_sbf_one(szero()) == false);
}

TEST_CASE("positive variable is not zero") {
	CHECK(is_sbf_zero(svar(1)) == false);
}

TEST_CASE("positive variable is not one") {
	CHECK(is_sbf_one(svar(1)) == false);
}

TEST_CASE("negative literal is not zero") {
	CHECK(is_sbf_zero(sneg(1)) == false);
}

TEST_CASE("negative literal is not one") {
	CHECK(is_sbf_one(sneg(1)) == false);
}

TEST_CASE("var & ~var is zero") {
	auto a = svar(1);
	CHECK(is_sbf_zero(a & ~a) == true);
}

TEST_CASE("var | ~var is one") {
	auto a = svar(1);
	CHECK(is_sbf_one(a | ~a) == true);
}

TEST_CASE("conjunction of two distinct vars is not zero") {
	CHECK(is_sbf_zero(svar(1) & svar(2)) == false);
}

TEST_CASE("conjunction of two distinct vars is not one") {
	CHECK(is_sbf_one(svar(1) & svar(2)) == false);
}

TEST_CASE("disjunction of two distinct vars is not zero") {
	CHECK(is_sbf_zero(svar(1) | svar(2)) == false);
}

TEST_CASE("disjunction of two distinct vars is not one") {
	CHECK(is_sbf_one(svar(1) | svar(2)) == false);
}

TEST_CASE("three-var conjunction is not zero") {
	CHECK(is_sbf_zero(svar(1) & svar(2) & svar(3)) == false);
}

TEST_CASE("var & ~var is zero via is_sbf_zero") {
	auto a = svar(4);
	auto b = sneg(4);
	CHECK(is_sbf_zero(a & b) == true);
}

TEST_CASE("is_sbf_zero on (zero & anything) is true") {
	CHECK(is_sbf_zero(szero() & svar(1)) == true);
}

TEST_CASE("is_sbf_one on (one | anything) is true") {
	CHECK(is_sbf_one(sone() | svar(1)) == true);
}

} // TEST_SUITE is_sbf_zero / is_sbf_one

// ============================================================================
TEST_SUITE("sbf — BA operations on constants") {
// ============================================================================

TEST_CASE("one & one = one") {
	CHECK((sone() & sone()) == true);
}

TEST_CASE("one & zero = zero") {
	CHECK((sone() & szero()) == false);
}

TEST_CASE("zero & one = zero") {
	CHECK((szero() & sone()) == false);
}

TEST_CASE("zero & zero = zero") {
	CHECK((szero() & szero()) == false);
}

TEST_CASE("one | one = one") {
	CHECK((sone() | sone()) == true);
}

TEST_CASE("one | zero = one") {
	CHECK((sone() | szero()) == true);
}

TEST_CASE("zero | one = one") {
	CHECK((szero() | sone()) == true);
}

TEST_CASE("zero | zero = zero") {
	CHECK((szero() | szero()) == false);
}

TEST_CASE("one ^ one = zero") {
	CHECK((sone() ^ sone()) == false);
}

TEST_CASE("one ^ zero = one") {
	CHECK((sone() ^ szero()) == true);
}

TEST_CASE("zero ^ one = one") {
	CHECK((szero() ^ sone()) == true);
}

TEST_CASE("zero ^ zero = zero") {
	CHECK((szero() ^ szero()) == false);
}

TEST_CASE("one + one = zero") {
	CHECK((sone() + sone()) == false);
}

TEST_CASE("one + zero = one") {
	CHECK((sone() + szero()) == true);
}

TEST_CASE("zero + one = one") {
	CHECK((szero() + sone()) == true);
}

TEST_CASE("zero + zero = zero") {
	CHECK((szero() + szero()) == false);
}

TEST_CASE("~one = zero") {
	CHECK((~sone()) == false);
}

TEST_CASE("~zero = one") {
	CHECK((~szero()) == true);
}

} // TEST_SUITE BA operations on constants

// ============================================================================
TEST_SUITE("sbf — BA operations on variables") {
// ============================================================================

TEST_CASE("var & zero = zero") {
	CHECK((svar(1) & szero()) == false);
}

TEST_CASE("var & one = var") {
	auto a = svar(1);
	CHECK((a & sone()) == a);
}

TEST_CASE("zero & var = zero") {
	CHECK((szero() & svar(1)) == false);
}

TEST_CASE("one & var = var") {
	auto a = svar(1);
	CHECK((sone() & a) == a);
}

TEST_CASE("var | zero = var") {
	auto a = svar(1);
	CHECK((a | szero()) == a);
}

TEST_CASE("var | one = one") {
	CHECK((svar(1) | sone()) == true);
}

TEST_CASE("zero | var = var") {
	auto a = svar(1);
	CHECK((szero() | a) == a);
}

TEST_CASE("one | var = one") {
	CHECK((sone() | svar(1)) == true);
}

TEST_CASE("var ^ zero = var") {
	auto a = svar(1);
	CHECK((a ^ szero()) == a);
}

TEST_CASE("var ^ one = ~var") {
	auto a = svar(1);
	CHECK((a ^ sone()) == ~a);
}

TEST_CASE("var ^ var = zero") {
	auto a = svar(1);
	CHECK((a ^ a) == false);
}

TEST_CASE("var & var = var (idempotency)") {
	auto a = svar(2);
	CHECK((a & a) == a);
}

TEST_CASE("var | var = var (idempotency)") {
	auto a = svar(2);
	CHECK((a | a) == a);
}

TEST_CASE("var & ~var = zero (complement law)") {
	auto a = svar(3);
	CHECK((a & ~a) == false);
}

TEST_CASE("var | ~var = one (complement law)") {
	auto a = svar(3);
	CHECK((a | ~a) == true);
}

TEST_CASE("double complement of variable") {
	auto a = svar(1);
	CHECK(~~a == a);
}

TEST_CASE("double complement of conjunction") {
	auto a = svar(1) & svar(2);
	CHECK(~~a == a);
}

TEST_CASE("triple complement equals single complement") {
	auto a = svar(1);
	CHECK(~~~a == ~a);
}

TEST_CASE("complement of negative literal") {
	auto neg = sneg(1);
	CHECK(~neg == svar(1));
}

TEST_CASE("var & ~var on second variable") {
	auto b = svar(2);
	CHECK((b & ~b) == false);
}

TEST_CASE("xor of distinct vars is not zero") {
	CHECK_FALSE(is_sbf_zero(svar(1) ^ svar(2)));
}

TEST_CASE("xor of distinct vars is not one") {
	CHECK_FALSE(is_sbf_one(svar(1) ^ svar(2)));
}

} // TEST_SUITE BA operations on variables

// ============================================================================
TEST_SUITE("sbf — algebraic laws") {
// ============================================================================

TEST_CASE("commutativity of and") {
	auto a = svar(1), b = svar(2);
	CHECK(is_sbf_zero((a & b) ^ (b & a)));
}

TEST_CASE("commutativity of or") {
	auto a = svar(1), b = svar(2);
	CHECK(is_sbf_zero((a | b) ^ (b | a)));
}

TEST_CASE("commutativity of xor") {
	auto a = svar(1), b = svar(2);
	CHECK(is_sbf_zero((a ^ b) ^ (b ^ a)));
}

TEST_CASE("associativity of and") {
	auto a = svar(1), b = svar(2), c = svar(3);
	CHECK(is_sbf_zero(((a & b) & c) ^ (a & (b & c))));
}

TEST_CASE("associativity of or") {
	auto a = svar(1), b = svar(2), c = svar(3);
	CHECK(is_sbf_zero(((a | b) | c) ^ (a | (b | c))));
}

TEST_CASE("De Morgan: ~(a & b) = ~a | ~b") {
	auto a = svar(1), b = svar(2);
	CHECK(is_sbf_zero(~(a & b) ^ (~a | ~b)));
}

TEST_CASE("De Morgan: ~(a | b) = ~a & ~b") {
	auto a = svar(1), b = svar(2);
	CHECK(is_sbf_zero(~(a | b) ^ (~a & ~b)));
}

TEST_CASE("absorption: a & (a | b) = a") {
	auto a = svar(1), b = svar(2);
	CHECK(is_sbf_zero((a & (a | b)) ^ a));
}

TEST_CASE("absorption: a | (a & b) = a") {
	auto a = svar(1), b = svar(2);
	CHECK(is_sbf_zero((a | (a & b)) ^ a));
}

TEST_CASE("distributivity: a & (b | c) = (a & b) | (a & c)") {
	auto a = svar(1), b = svar(2), c = svar(3);
	CHECK(is_sbf_zero((a & (b | c)) ^ ((a & b) | (a & c))));
}

TEST_CASE("distributivity: a | (b & c) = (a | b) & (a | c)") {
	auto a = svar(1), b = svar(2), c = svar(3);
	CHECK(is_sbf_zero((a | (b & c)) ^ ((a | b) & (a | c))));
}

TEST_CASE("De Morgan three vars: ~(a & b & c) = ~a | ~b | ~c") {
	auto a = svar(1), b = svar(2), c = svar(3);
	CHECK(is_sbf_zero(~(a & b & c) ^ (~a | ~b | ~c)));
}

TEST_CASE("De Morgan three vars: ~(a | b | c) = ~a & ~b & ~c") {
	auto a = svar(1), b = svar(2), c = svar(3);
	CHECK(is_sbf_zero(~(a | b | c) ^ (~a & ~b & ~c)));
}

TEST_CASE("zero is identity for or") {
	auto a = svar(1) & svar(2);
	CHECK(is_sbf_zero((a | szero()) ^ a));
}

TEST_CASE("one is identity for and") {
	auto a = svar(1) & svar(2);
	CHECK(is_sbf_zero((a & sone()) ^ a));
}

TEST_CASE("zero absorbs in and") {
	auto a = svar(1) | svar(2);
	CHECK((a & szero()) == false);
}

TEST_CASE("one absorbs in or") {
	auto a = svar(1) & svar(2);
	CHECK((a | sone()) == true);
}

TEST_CASE("a ^ a = zero") {
	auto a = svar(1) | (svar(2) & svar(3));
	CHECK((a ^ a) == false);
}

TEST_CASE("a ^ zero = a") {
	auto a = svar(1) | svar(2);
	CHECK(is_sbf_zero((a ^ szero()) ^ a));
}

TEST_CASE("a ^ one = ~a") {
	auto a = svar(1) & svar(2);
	CHECK(is_sbf_zero((a ^ sone()) ^ ~a));
}

TEST_CASE("a ^ ~a = one") {
	auto a = svar(1) | svar(2);
	CHECK((a ^ ~a) == true);
}

TEST_CASE("complement of zero is one") {
	CHECK(~szero() == sone());
}

TEST_CASE("complement of one is zero") {
	CHECK(~sone() == szero());
}

TEST_CASE("boolean identity: a & (b | ~b) = a") {
	auto a = svar(1), b = svar(2);
	CHECK(is_sbf_zero((a & (b | ~b)) ^ a));
}

TEST_CASE("boolean identity: a | (b & ~b) = a") {
	auto a = svar(1), b = svar(2);
	CHECK(is_sbf_zero((a | (b & ~b)) ^ a));
}

TEST_CASE("four-variable absorption chain") {
	auto a = svar(1), b = svar(2), c = svar(3), d = svar(4);
	auto lhs = a & b & (a | c) & (b | d);
	auto rhs = a & b;
	CHECK(is_sbf_zero(lhs ^ rhs));
}

TEST_CASE("idempotency on complex expression") {
	auto x = (svar(1) | svar(2)) & (svar(2) | svar(3));
	CHECK(is_sbf_zero((x & x) ^ x));
	CHECK(is_sbf_zero((x | x) ^ x));
}

} // TEST_SUITE algebraic laws

// ============================================================================
TEST_SUITE("sbf — splitter") {
// ============================================================================

TEST_CASE("sbf_splitter_one is not zero") {
	init_bdd();
	CHECK(is_sbf_zero(sbf_splitter_one()) == false);
}

TEST_CASE("sbf_splitter on one with upper type is not zero") {
	init_bdd();
	auto s = sbf_splitter(sone(), splitter_type::upper);
	CHECK(is_sbf_zero(s) == false);
}

TEST_CASE("sbf_splitter on one with lower type is not zero") {
	init_bdd();
	auto s = sbf_splitter(sone(), splitter_type::lower);
	CHECK(is_sbf_zero(s) == false);
}

TEST_CASE("sbf_splitter on one with middle type is not zero") {
	init_bdd();
	auto s = sbf_splitter(sone(), splitter_type::middle);
	CHECK(is_sbf_zero(s) == false);
}

TEST_CASE("splitter upper: 3-clause disjunction removes last clause") {
	init_bdd();
	auto a1 = bdd_handle<Bool>::bit(true, 1);
	auto a2 = bdd_handle<Bool>::bit(true, 2);
	auto a3 = bdd_handle<Bool>::bit(true, 3);
	auto d1 = a1 | a2 | a3;
	CHECK(sbf_splitter(d1, splitter_type::upper) == (a1 | a2));
}

TEST_CASE("splitter upper: negative 3-clause disjunction removes last clause") {
	init_bdd();
	auto a1 = bdd_handle<Bool>::bit(false, 1);
	auto a2 = bdd_handle<Bool>::bit(false, 2);
	auto a3 = bdd_handle<Bool>::bit(false, 3);
	auto d1 = a1 | a2 | a3;
	CHECK(sbf_splitter(d1, splitter_type::upper) == (a1 | a2));
}

TEST_CASE("splitter upper: single DNF clause gets refined") {
	init_bdd();
	auto a1 = bdd_handle<Bool>::bit(true, 1);
	auto a2 = bdd_handle<Bool>::bit(true, 2);
	auto a3 = bdd_handle<Bool>::bit(true, 3);
	auto a4 = bdd_handle<Bool>::bit(true, 4);
	auto d1 = a1 & a2 & a3;
	CHECK(sbf_splitter(d1, splitter_type::upper) == (d1 & a4));
}

TEST_CASE("splitter result is not equal to original for multi-clause expression") {
	init_bdd();
	auto a1 = bdd_handle<Bool>::bit(true, 1);
	auto a2 = bdd_handle<Bool>::bit(true, 2);
	auto a3 = bdd_handle<Bool>::bit(true, 3);
	auto d1 = a1 | a2 | a3;
	auto s = sbf_splitter(d1, splitter_type::upper);
	CHECK(s != d1);
}

TEST_CASE("splitter result is contained in original (splitter <= original)") {
	init_bdd();
	auto a1 = bdd_handle<Bool>::bit(true, 1);
	auto a2 = bdd_handle<Bool>::bit(true, 2);
	auto a3 = bdd_handle<Bool>::bit(true, 3);
	auto d1 = a1 | a2 | a3;
	auto s = sbf_splitter(d1, splitter_type::upper);
	// s <= d1 iff s & ~d1 == 0
	CHECK(is_sbf_zero(s & ~d1));
}

TEST_CASE("sbf_splitter_one equals htrue splitter with bad type") {
	init_bdd();
	auto expected = bdd_handle<Bool>::htrue->splitter(splitter_type::bad);
	CHECK(sbf_splitter_one() == expected);
}

TEST_CASE("two-clause disjunction: splitter is a single clause") {
	init_bdd();
	auto a1 = bdd_handle<Bool>::bit(true, 1);
	auto a2 = bdd_handle<Bool>::bit(true, 2);
	auto d = a1 | a2;
	auto s = sbf_splitter(d, splitter_type::upper);
	CHECK(is_sbf_zero(s) == false);
	CHECK(is_sbf_zero(s & ~d) == true);
}

} // TEST_SUITE splitter

// ============================================================================
TEST_SUITE("sbf — normalize_sbf") {
// ============================================================================

TEST_CASE("normalize_sbf of zero is zero") {
	CHECK(normalize_sbf(szero()) == szero());
}

TEST_CASE("normalize_sbf of one is one") {
	CHECK(normalize_sbf(sone()) == sone());
}

TEST_CASE("normalize_sbf of variable is identity") {
	auto a = svar(1);
	CHECK(normalize_sbf(a) == a);
}

TEST_CASE("normalize_sbf of conjunction is identity") {
	auto a = svar(1) & svar(2);
	CHECK(normalize_sbf(a) == a);
}

TEST_CASE("normalize_sbf of disjunction is identity") {
	auto a = svar(1) | svar(2);
	CHECK(normalize_sbf(a) == a);
}

TEST_CASE("normalize_sbf of complement is identity") {
	auto a = ~svar(1);
	CHECK(normalize_sbf(a) == a);
}

TEST_CASE("normalize_sbf of complex expression is identity") {
	auto a = (svar(1) | svar(2)) & (~svar(3) | svar(4));
	CHECK(normalize_sbf(a) == a);
}

TEST_CASE("normalize_sbf of negated literal is identity") {
	auto a = sneg(2);
	CHECK(normalize_sbf(a) == a);
}

} // TEST_SUITE normalize_sbf

// ============================================================================
TEST_SUITE("sbf — parse_sbf") {
// ============================================================================

TEST_CASE("parse true constant") {
	auto r = spar("1");
	CHECK(is_sbf_one(r));
}

TEST_CASE("parse false constant") {
	auto r = spar("0");
	CHECK(is_sbf_zero(r));
}

TEST_CASE("parse negation of true") {
	CHECK(is_sbf_zero(spar("1'")));
}

TEST_CASE("parse negation of false") {
	CHECK(is_sbf_one(spar("0'")));
}

TEST_CASE("parse double negation of true") {
	CHECK(is_sbf_one(spar("1''")));
}

TEST_CASE("parse double negation of false") {
	CHECK(is_sbf_zero(spar("0''")));
}

TEST_CASE("parse triple negation of true") {
	CHECK(is_sbf_zero(spar("1'''")));
}

TEST_CASE("parse and of true and false") {
	CHECK(is_sbf_zero(spar("1 0")));
}

TEST_CASE("parse and of true and true") {
	CHECK(is_sbf_one(spar("1 1")));
}

TEST_CASE("parse or of false and false") {
	CHECK(is_sbf_zero(spar("0|0")));
}

TEST_CASE("parse or of false and true") {
	CHECK(is_sbf_one(spar("0|1")));
}

TEST_CASE("parse xor of true and true") {
	CHECK(is_sbf_zero(spar("1^1")));
}

TEST_CASE("parse xor of true and false") {
	CHECK(is_sbf_one(spar("1^0")));
}

TEST_CASE("parse xor via + syntax") {
	CHECK(is_sbf_zero(spar("1+1")));
	CHECK(is_sbf_one(spar("1+0")));
}

TEST_CASE("parse variable") {
	auto r = spar("p");
	CHECK(is_sbf_zero(r) == false);
	CHECK(is_sbf_one(r) == false);
}

TEST_CASE("parse negated variable") {
	auto p = spar("p");
	auto p_neg = spar("p'");
	CHECK(is_sbf_zero(p & p_neg));
}

TEST_CASE("parse variable and true") {
	auto p = spar("p");
	CHECK(spar("p 1") == p);
}

TEST_CASE("parse variable and false") {
	CHECK(is_sbf_zero(spar("p 0")));
}

TEST_CASE("parse variable or true") {
	CHECK(is_sbf_one(spar("p|1")));
}

TEST_CASE("parse variable or false") {
	auto p = spar("p");
	CHECK(spar("p|0") == p);
}

TEST_CASE("parse two distinct variables and") {
	auto pq = spar("p q");
	CHECK(is_sbf_zero(pq) == false);
	CHECK(is_sbf_one(pq) == false);
}

TEST_CASE("parse two distinct variables or") {
	auto por = spar("p|q");
	CHECK(is_sbf_zero(por) == false);
}

TEST_CASE("parse complement law through parser") {
	CHECK(is_sbf_one(spar("p|p'")));
}

TEST_CASE("parse and complement law through parser") {
	CHECK(is_sbf_zero(spar("p p'")));
}

TEST_CASE("parse variable xor with self") {
	CHECK(is_sbf_zero(spar("p^p")));
}

TEST_CASE("parse variable xor negation equals one") {
	CHECK(is_sbf_one(spar("p^p'")));
}

TEST_CASE("parse precedence: and over or") {
	// 1|0&0 should parse as 1|(0&0) = 1|0 = 1
	CHECK(is_sbf_one(spar("1|0&0")));
	CHECK(is_sbf_one(spar("1|(0&0)")));
	CHECK(is_sbf_zero(spar("(1|0)&0")));
}

TEST_CASE("parse De Morgan via parser: ~(p&q) == p'|q'") {
	auto lhs = spar("(p q)'");
	auto rhs = spar("p'|q'");
	CHECK(is_sbf_zero(lhs ^ rhs));
}

TEST_CASE("parse complex expression produces non-trivial result") {
	auto r = spar("z' | x b (1'^(a b) | 0+c | a) ^ d | d^e&1");
	CHECK(is_sbf_zero(r) == false);
	CHECK(is_sbf_one(r) == false);
}

TEST_CASE("parsed same-string results are equal") {
	CHECK(spar("p q | r'") == spar("p q | r'"));
}

TEST_CASE("parsed and constructed one are equal") {
	CHECK(spar("1") == sone());
}

TEST_CASE("parsed and constructed zero are equal") {
	CHECK(spar("0") == szero());
}

} // TEST_SUITE parse_sbf
