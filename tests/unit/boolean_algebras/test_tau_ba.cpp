// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Comprehensive tests for tau_ba — the temporal Boolean algebra wrapper.
// Covers: construction, is_zero/is_one, BA operators (~, &, |, ^/+),
// Boolean algebra laws, normalize_tau, syntactic checks, equality,
// splitter, is_tau_closed, parse_tau, operator<<, and LTL integration.

#include <sstream>
#include "test_init.h"
#include "test_tau_helpers.h"
#include "boolean_algebras/tau_ba.h"

using namespace idni::tau_lang;

// inner_bas: the base BAs for tau_ba (without tau_ba itself).
// tau_ba<inner_bas> is the outermost tau_ba type in the test pack.
#define inner_bas qint, qlt, nlang_ba, bv, sbf_ba, hsb
using test_ba = tau_ba<inner_bas>;

// ── helpers ───────────────────────────────────────────────────────────────────

static void do_gc() {
	std::unordered_set<tref> keep;
	tau::gc(keep);
}
struct gc_fixture { gc_fixture() { do_gc(); } };

// Wrap the canonical T tree as a tau_ba (one)
static test_ba tau_one() { return test_ba(tau::geth(tau::_T())); }
// Wrap the canonical F tree as a tau_ba (zero)
static test_ba tau_zero() { return test_ba(tau::geth(tau::_F())); }

// Parse a tau spec string and return the resulting tau_ba.
static test_ba from_spec(const char* s) {
	auto nso = get_nso_rr(s);
	assert(nso.has_value());
	return test_ba(nso.value().rec_relations, nso.value().main);
}

// ============================================================================
TEST_SUITE("tau_ba — construction") {
// ============================================================================

TEST_CASE("default constructor produces a tau_ba") {
	test_ba b;
	(void)b;
	CHECK(true); // if we get here construction succeeded
}

TEST_CASE("construct from htref of T") {
	auto b = test_ba(tau::geth(tau::_T()));
	CHECK(is_tau_syntactic_one(b));
}

TEST_CASE("construct from htref of F") {
	auto b = test_ba(tau::geth(tau::_F()));
	CHECK(is_tau_syntactic_zero(b));
}

TEST_CASE("construct from tref of T") {
	auto b = test_ba(tau::_T());
	CHECK(is_tau_syntactic_one(b));
}

TEST_CASE("construct from tref of F") {
	auto b = test_ba(tau::_F());
	CHECK(is_tau_syntactic_zero(b));
}

TEST_CASE("construct with empty rec_relations and htref of T") {
	rewriter::rules rrs;
	auto b = test_ba(rrs, tau::geth(tau::_T()));
	CHECK(is_tau_syntactic_one(b));
}

TEST_CASE("construct with empty rec_relations and htref of F") {
	rewriter::rules rrs;
	auto b = test_ba(rrs, tau::geth(tau::_F()));
	CHECK(is_tau_syntactic_zero(b));
}

TEST_CASE("construct with empty rec_relations and tref of T") {
	rewriter::rules rrs;
	auto b = test_ba(rrs, tau::_T());
	CHECK(is_tau_syntactic_one(b));
}

TEST_CASE("construct with empty rec_relations and tref of F") {
	rewriter::rules rrs;
	auto b = test_ba(rrs, tau::_F());
	CHECK(is_tau_syntactic_zero(b));
}

TEST_CASE("two tau_one() calls produce equal tau_ba objects") {
	CHECK(tau_one() == tau_one());
}

TEST_CASE("two tau_zero() calls produce equal tau_ba objects") {
	CHECK(tau_zero() == tau_zero());
}

TEST_CASE("tau_one and tau_zero are distinct") {
	CHECK_FALSE(tau_one() == tau_zero());
}

} // TEST_SUITE construction

// ============================================================================
TEST_SUITE("tau_ba — is_zero / is_one (semantic)") {
// ============================================================================

TEST_CASE("T is one") {
	CHECK(tau_one().is_one() == true);
}

TEST_CASE("T is not zero") {
	CHECK(tau_one().is_zero() == false);
}

TEST_CASE("F is zero") {
	CHECK(tau_zero().is_zero() == true);
}

TEST_CASE("F is not one") {
	CHECK(tau_zero().is_one() == false);
}

TEST_CASE("T equals true") {
	CHECK(tau_one() == true);
}

TEST_CASE("T does not equal false") {
	CHECK(tau_one() != false);
}

TEST_CASE("F equals false") {
	CHECK(tau_zero() == false);
}

TEST_CASE("F does not equal true") {
	CHECK(tau_zero() != true);
}

TEST_CASE("bool == tau_ba: true == T") {
	CHECK(true == tau_one());
}

TEST_CASE("bool == tau_ba: false == F") {
	CHECK(false == tau_zero());
}

TEST_CASE("bool != tau_ba: false != T") {
	CHECK(false != tau_one());
}

TEST_CASE("bool != tau_ba: true != F") {
	CHECK(true != tau_zero());
}

} // TEST_SUITE is_zero / is_one

// ============================================================================
TEST_SUITE("tau_ba — complement (NOT)") {
// ============================================================================

TEST_CASE("complement of T is zero") {
	auto c = ~tau_one();
	CHECK(c.is_zero() == true);
}

TEST_CASE("complement of F is one") {
	auto c = ~tau_zero();
	CHECK(c.is_one() == true);
}

TEST_CASE("complement of T is syntactically F") {
	auto c = ~tau_one();
	CHECK(is_tau_syntactic_zero(c));
}

TEST_CASE("complement of F is syntactically T") {
	auto c = ~tau_zero();
	CHECK(is_tau_syntactic_one(c));
}

TEST_CASE("double complement of T is T") {
	auto cc = ~~tau_one();
	CHECK(is_tau_syntactic_one(cc));
}

TEST_CASE("double complement of F is F") {
	auto cc = ~~tau_zero();
	CHECK(is_tau_syntactic_zero(cc));
}

TEST_CASE("double complement is one for T") {
	CHECK((~~tau_one()).is_one() == true);
}

TEST_CASE("double complement is zero for F") {
	CHECK((~~tau_zero()).is_zero() == true);
}

TEST_CASE("triple complement of T is syntactically F") {
	auto ccc = ~~~tau_one();
	CHECK(is_tau_syntactic_zero(ccc));
}

TEST_CASE("triple complement of F is syntactically T") {
	auto ccc = ~~~tau_zero();
	CHECK(is_tau_syntactic_one(ccc));
}

} // TEST_SUITE complement

// ============================================================================
TEST_SUITE("tau_ba — AND operator") {
// ============================================================================

TEST_CASE("T & T is one") {
	CHECK((tau_one() & tau_one()).is_one() == true);
}

TEST_CASE("T & F is zero") {
	CHECK((tau_one() & tau_zero()).is_zero() == true);
}

TEST_CASE("F & T is zero") {
	CHECK((tau_zero() & tau_one()).is_zero() == true);
}

TEST_CASE("F & F is zero") {
	CHECK((tau_zero() & tau_zero()).is_zero() == true);
}

TEST_CASE("T & T is syntactically one after normalize_tau") {
	auto r = normalize_tau(tau_one() & tau_one());
	CHECK(is_tau_syntactic_one(r));
}

TEST_CASE("T & F is syntactically zero after normalize_tau") {
	auto r = normalize_tau(tau_one() & tau_zero());
	CHECK(is_tau_syntactic_zero(r));
}

TEST_CASE("F & F is syntactically zero after normalize_tau") {
	auto r = normalize_tau(tau_zero() & tau_zero());
	CHECK(is_tau_syntactic_zero(r));
}

TEST_CASE("A & ~A is zero (complement law)") {
	auto a = tau_one();
	CHECK((a & ~a).is_zero() == true);
}

TEST_CASE("F & ~F is zero") {
	auto a = tau_zero();
	CHECK((a & ~a).is_zero() == true);
}

} // TEST_SUITE AND

// ============================================================================
TEST_SUITE("tau_ba — OR operator") {
// ============================================================================

TEST_CASE("T | T is one") {
	CHECK((tau_one() | tau_one()).is_one() == true);
}

TEST_CASE("T | F is one") {
	CHECK((tau_one() | tau_zero()).is_one() == true);
}

TEST_CASE("F | T is one") {
	CHECK((tau_zero() | tau_one()).is_one() == true);
}

TEST_CASE("F | F is zero") {
	CHECK((tau_zero() | tau_zero()).is_zero() == true);
}

TEST_CASE("T | T is syntactically one after normalize_tau") {
	auto r = normalize_tau(tau_one() | tau_one());
	CHECK(is_tau_syntactic_one(r));
}

TEST_CASE("F | F is syntactically zero after normalize_tau") {
	auto r = normalize_tau(tau_zero() | tau_zero());
	CHECK(is_tau_syntactic_zero(r));
}

TEST_CASE("A | ~A is one (complement law)") {
	auto a = tau_one();
	CHECK((a | ~a).is_one() == true);
}

TEST_CASE("F | ~F is one") {
	auto a = tau_zero();
	CHECK((a | ~a).is_one() == true);
}

} // TEST_SUITE OR

// ============================================================================
TEST_SUITE("tau_ba — XOR / addition operators") {
// ============================================================================

TEST_CASE("T ^ T is zero") {
	CHECK((tau_one() ^ tau_one()).is_zero() == true);
}

TEST_CASE("T ^ F is one") {
	CHECK((tau_one() ^ tau_zero()).is_one() == true);
}

TEST_CASE("F ^ T is one") {
	CHECK((tau_zero() ^ tau_one()).is_one() == true);
}

TEST_CASE("F ^ F is zero") {
	CHECK((tau_zero() ^ tau_zero()).is_zero() == true);
}

TEST_CASE("+ operator is the same as ^ operator") {
	auto a = tau_one();
	auto b = tau_zero();
	CHECK((a + b).is_one() == (a ^ b).is_one());
	CHECK((a + b).is_zero() == (a ^ b).is_zero());
}

TEST_CASE("T + T is zero") {
	CHECK((tau_one() + tau_one()).is_zero() == true);
}

TEST_CASE("T + F is one") {
	CHECK((tau_one() + tau_zero()).is_one() == true);
}

TEST_CASE("A ^ A is zero (XOR self-cancellation)") {
	auto a = tau_one();
	CHECK((a ^ a).is_zero() == true);
}

TEST_CASE("F ^ F is syntactically zero after normalize_tau") {
	auto r = normalize_tau(tau_zero() ^ tau_zero());
	CHECK(is_tau_syntactic_zero(r));
}

} // TEST_SUITE XOR / addition

// ============================================================================
TEST_SUITE("tau_ba — Boolean algebra laws") {
// ============================================================================

TEST_CASE("idempotency of AND: T & T = T") {
	auto a = tau_one();
	CHECK((a & a).is_one() == true);
}

TEST_CASE("idempotency of AND: F & F = F") {
	auto a = tau_zero();
	CHECK((a & a).is_zero() == true);
}

TEST_CASE("idempotency of OR: T | T = T") {
	auto a = tau_one();
	CHECK((a | a).is_one() == true);
}

TEST_CASE("idempotency of OR: F | F = F") {
	auto a = tau_zero();
	CHECK((a | a).is_zero() == true);
}

TEST_CASE("De Morgan: ~(T & T) = ~T | ~T") {
	auto a = tau_one();
	auto b = tau_one();
	CHECK((~(a & b)).is_zero() == (~a | ~b).is_zero());
}

TEST_CASE("De Morgan: ~(T | F) = ~T & ~F") {
	auto a = tau_one();
	auto b = tau_zero();
	CHECK((~(a | b)).is_zero() == (~a & ~b).is_zero());
}

TEST_CASE("complement law: A & ~A = F") {
	auto a = tau_one();
	auto ca = ~a;
	CHECK((a & ca).is_zero() == true);
}

TEST_CASE("complement law: A | ~A = T") {
	auto a = tau_one();
	auto ca = ~a;
	CHECK((a | ca).is_one() == true);
}

TEST_CASE("complement law for F: F & ~F = F") {
	auto a = tau_zero();
	auto ca = ~a;
	CHECK((a & ca).is_zero() == true);
}

TEST_CASE("complement law for F: F | ~F = T") {
	auto a = tau_zero();
	auto ca = ~a;
	CHECK((a | ca).is_one() == true);
}

TEST_CASE("absorption: T & (T | F) = T") {
	auto a = tau_one();
	auto b = tau_zero();
	CHECK((a & (a | b)).is_one() == true);
}

TEST_CASE("absorption: F | (F & T) = F") {
	auto a = tau_zero();
	auto b = tau_one();
	CHECK((a | (a & b)).is_zero() == true);
}

TEST_CASE("T & F = F (annihilator)") {
	CHECK((tau_one() & tau_zero()).is_zero() == true);
}

TEST_CASE("T | F = T (identity of OR)") {
	CHECK((tau_one() | tau_zero()).is_one() == true);
}

TEST_CASE("F & T = F (commutativity of AND with annihilator)") {
	CHECK((tau_zero() & tau_one()).is_zero() == true);
}

} // TEST_SUITE Boolean algebra laws

// ============================================================================
TEST_SUITE("tau_ba — normalize_tau") {
// ============================================================================

TEST_CASE("normalize_tau of T is syntactically T") {
	auto r = normalize_tau(tau_one());
	CHECK(is_tau_syntactic_one(r));
}

TEST_CASE("normalize_tau of F is syntactically F") {
	auto r = normalize_tau(tau_zero());
	CHECK(is_tau_syntactic_zero(r));
}

TEST_CASE("normalize_tau of ~T is syntactically F") {
	auto r = normalize_tau(~tau_one());
	CHECK(is_tau_syntactic_zero(r));
}

TEST_CASE("normalize_tau of ~F is syntactically T") {
	auto r = normalize_tau(~tau_zero());
	CHECK(is_tau_syntactic_one(r));
}

TEST_CASE("normalize_tau of T & T is syntactically T") {
	auto r = normalize_tau(tau_one() & tau_one());
	CHECK(is_tau_syntactic_one(r));
}

TEST_CASE("normalize_tau of T & F is syntactically F") {
	auto r = normalize_tau(tau_one() & tau_zero());
	CHECK(is_tau_syntactic_zero(r));
}

TEST_CASE("normalize_tau of F | F is syntactically F") {
	auto r = normalize_tau(tau_zero() | tau_zero());
	CHECK(is_tau_syntactic_zero(r));
}

TEST_CASE("normalize_tau of T | F is syntactically T") {
	auto r = normalize_tau(tau_one() | tau_zero());
	CHECK(is_tau_syntactic_one(r));
}

} // TEST_SUITE normalize_tau

// ============================================================================
TEST_SUITE("tau_ba — syntactic checks") {
// ============================================================================

TEST_CASE("is_tau_syntactic_one: T is syntactically one") {
	CHECK(is_tau_syntactic_one(tau_one()));
}

TEST_CASE("is_tau_syntactic_one: F is not syntactically one") {
	CHECK_FALSE(is_tau_syntactic_one(tau_zero()));
}

TEST_CASE("is_tau_syntactic_zero: F is syntactically zero") {
	CHECK(is_tau_syntactic_zero(tau_zero()));
}

TEST_CASE("is_tau_syntactic_zero: T is not syntactically zero") {
	CHECK_FALSE(is_tau_syntactic_zero(tau_one()));
}

TEST_CASE("~T is syntactically zero") {
	CHECK(is_tau_syntactic_zero(~tau_one()));
}

TEST_CASE("~F is syntactically one") {
	CHECK(is_tau_syntactic_one(~tau_zero()));
}

TEST_CASE("T & F is syntactically zero (operator& short-circuits)") {
	// build_wff_and short-circuits: T & F → F directly, no normalize_tau needed
	CHECK(is_tau_syntactic_zero(tau_one() & tau_zero()));
}

TEST_CASE("after normalize_tau: T & F is syntactically zero") {
	auto r = normalize_tau(tau_one() & tau_zero());
	CHECK(is_tau_syntactic_zero(r));
}

TEST_CASE("after normalize_tau: T | T is syntactically one") {
	auto r = normalize_tau(tau_one() | tau_one());
	CHECK(is_tau_syntactic_one(r));
}

} // TEST_SUITE syntactic checks

// ============================================================================
TEST_SUITE("tau_ba — equality / inequality operators") {
// ============================================================================

TEST_CASE("T == T") {
	CHECK(tau_one() == tau_one());
}

TEST_CASE("F == F") {
	CHECK(tau_zero() == tau_zero());
}

TEST_CASE("T != F") {
	CHECK(tau_one() != tau_zero());
}

TEST_CASE("F != T") {
	CHECK(tau_zero() != tau_one());
}

TEST_CASE("T == true") {
	CHECK(tau_one() == true);
}

TEST_CASE("F == false") {
	CHECK(tau_zero() == false);
}

TEST_CASE("T != false") {
	CHECK(tau_one() != false);
}

TEST_CASE("F != true") {
	CHECK(tau_zero() != true);
}

TEST_CASE("true == T (symmetric)") {
	CHECK(true == tau_one());
}

TEST_CASE("false == F (symmetric)") {
	CHECK(false == tau_zero());
}

TEST_CASE("false != T (symmetric)") {
	CHECK(false != tau_one());
}

TEST_CASE("true != F (symmetric)") {
	CHECK(true != tau_zero());
}

TEST_CASE("~T is zero (syntactic)") {
	CHECK(is_tau_syntactic_zero(~tau_one()));
}

TEST_CASE("~F is one (syntactic)") {
	CHECK(is_tau_syntactic_one(~tau_zero()));
}

} // TEST_SUITE equality / inequality

// ============================================================================
TEST_SUITE("tau_ba — splitter") {
// ============================================================================

TEST_CASE("splitter of F is zero") {
	auto s = splitter(tau_zero(), splitter_type::lower);
	CHECK(s.is_zero() == true);
}

TEST_CASE("splitter of T is non-zero") {
	auto s = splitter(tau_one(), splitter_type::lower);
	CHECK(s.is_zero() == false);
}

TEST_CASE("splitter of T is non-one") {
	auto s = splitter(tau_one(), splitter_type::lower);
	CHECK(s.is_one() == false);
}

TEST_CASE("splitter of T with upper type is non-zero") {
	auto s = splitter(tau_one(), splitter_type::upper);
	CHECK(s.is_zero() == false);
}

TEST_CASE("splitter of T with upper type is non-one") {
	auto s = splitter(tau_one(), splitter_type::upper);
	CHECK(s.is_one() == false);
}

TEST_CASE("tau_splitter_one is non-zero") {
	auto s = tau_splitter_one<inner_bas>();
	CHECK(s.is_zero() == false);
}

TEST_CASE("tau_splitter_one is non-one") {
	auto s = tau_splitter_one<inner_bas>();
	CHECK(s.is_one() == false);
}

TEST_CASE("tau_splitter_one complement is non-zero") {
	auto s = tau_splitter_one<inner_bas>();
	auto other = tau_one() & ~s;
	CHECK(other.is_zero() == false);
}

TEST_CASE("splitter result is sub-element of T") {
	auto s = splitter(tau_one(), splitter_type::lower);
	// s <= T, so s & ~T = s & F = F
	CHECK((s & ~tau_one()).is_zero() == true);
}

} // TEST_SUITE splitter

// ============================================================================
TEST_SUITE("tau_ba — is_tau_closed") {
// ============================================================================

TEST_CASE("T is closed") {
	CHECK(is_tau_closed(tau_one()));
}

TEST_CASE("F is closed") {
	CHECK(is_tau_closed(tau_zero()));
}

TEST_CASE("~T is closed") {
	CHECK(is_tau_closed(~tau_one()));
}

TEST_CASE("~F is closed") {
	CHECK(is_tau_closed(~tau_zero()));
}

TEST_CASE("T & F is closed") {
	CHECK(is_tau_closed(tau_one() & tau_zero()));
}

TEST_CASE("T | F is closed") {
	CHECK(is_tau_closed(tau_one() | tau_zero()));
}

TEST_CASE("T ^ F is closed") {
	CHECK(is_tau_closed(tau_one() ^ tau_zero()));
}

TEST_CASE("formula from spec with output var is closed") {
	auto b = from_spec("G (o1[t]:tau = {T.}:tau).");
	CHECK(is_tau_closed(b));
}

} // TEST_SUITE is_tau_closed

// ============================================================================
TEST_SUITE("tau_ba — parse_tau") {
// ============================================================================

TEST_CASE("parse T. succeeds") {
	auto r = parse_tau<inner_bas>("T.");
	CHECK(r.has_value());
}

TEST_CASE("parse T. is syntactically one") {
	auto r = parse_tau<inner_bas>("T.");
	REQUIRE(r.has_value());
	auto b = std::get<test_ba>(r->first);
	CHECK(is_tau_syntactic_one(b));
}

TEST_CASE("parse F. succeeds") {
	auto r = parse_tau<inner_bas>("F.");
	CHECK(r.has_value());
}

TEST_CASE("parse F. is syntactically zero") {
	auto r = parse_tau<inner_bas>("F.");
	REQUIRE(r.has_value());
	auto b = std::get<test_ba>(r->first);
	CHECK(is_tau_syntactic_zero(b));
}

TEST_CASE("parse empty string returns nullopt") {
	auto r = parse_tau<inner_bas>("");
	CHECK(!r.has_value());
}

TEST_CASE("parse tau spec with output var succeeds") {	auto r = parse_tau<inner_bas>("G (o1[t]:tau = {T.}:tau).");
	CHECK(r.has_value());
}

TEST_CASE("parse tau spec produces correct type tag") {	auto r = parse_tau<inner_bas>("T.");
	REQUIRE(r.has_value());
	CHECK(r->second != nullptr);
}

TEST_CASE("parse T. is_one()") {
	auto r = parse_tau<inner_bas>("T.");
	REQUIRE(r.has_value());
	auto b = std::get<test_ba>(r->first);
	CHECK(b.is_one() == true);
}

TEST_CASE("parse F. is_zero()") {
	auto r = parse_tau<inner_bas>("F.");
	REQUIRE(r.has_value());
	auto b = std::get<test_ba>(r->first);
	CHECK(b.is_zero() == true);
}

TEST_CASE("parse realizable tau spec is not zero") {	auto r = parse_tau<inner_bas>("G (o1[t]:tau = {T.}:tau).");
	REQUIRE(r.has_value());
	auto b = std::get<test_ba>(r->first);
	CHECK(b.is_zero() == false);
}

} // TEST_SUITE parse_tau

// ============================================================================
TEST_SUITE("tau_ba — operator<<") {
// ============================================================================

TEST_CASE("printing T produces non-empty string") {
	std::ostringstream ss;
	ss << tau_one();
	CHECK(!ss.str().empty());
}

TEST_CASE("printing F produces non-empty string") {
	std::ostringstream ss;
	ss << tau_zero();
	CHECK(!ss.str().empty());
}

TEST_CASE("printing T contains T marker") {
	std::ostringstream ss;
	ss << tau_one();
	CHECK(ss.str().find('T') != std::string::npos);
}

TEST_CASE("printing F contains F marker") {
	std::ostringstream ss;
	ss << tau_zero();
	CHECK(ss.str().find('F') != std::string::npos);
}

TEST_CASE("T and F print differently") {
	std::ostringstream ss1, ss2;
	ss1 << tau_one();
	ss2 << tau_zero();
	CHECK(ss1.str() != ss2.str());
}

} // TEST_SUITE operator<<

// ============================================================================
TEST_SUITE("tau_ba — LTL integration") {
// ============================================================================

TEST_CASE("parse and check: T spec is one") {	auto b = from_spec("T.");
	CHECK(b.is_one() == true);
}

TEST_CASE("parse and check: F spec is zero") {	auto b = from_spec("F.");
	CHECK(b.is_zero() == true);
}

TEST_CASE("G(o1=T) is not zero (realizable)") {	auto b = from_spec("G (o1[t]:tau = {T.}:tau).");
	CHECK(b.is_zero() == false);
}

TEST_CASE("G(o1=F) is not zero (realizable)") {	auto b = from_spec("G (o1[t]:tau = {F.}:tau).");
	CHECK(b.is_zero() == false);
}

TEST_CASE("F(o1=T) is not zero (realizable)") {	auto b = from_spec("F (o1[t]:tau = {T.}:tau).");
	CHECK(b.is_zero() == false);
}

TEST_CASE("G(o1=T) is not one (not universally valid)") {	auto b = from_spec("G (o1[t]:tau = {T.}:tau).");
	CHECK(b.is_one() == false);
}

TEST_CASE("G(o1=T) & G(o1!=T) is zero (contradiction)") {	auto sat = from_spec("G (o1[t]:tau = {T.}:tau).");
	auto unsat = from_spec("G (o1[t]:tau != {T.}:tau).");
	CHECK((sat & unsat).is_zero() == true);
}

TEST_CASE("G(o1=T) | G(o1=F) is not zero") {	auto a = from_spec("G (o1[t]:tau = {T.}:tau).");
	auto b = from_spec("G (o1[t]:tau = {F.}:tau).");
	CHECK((a | b).is_zero() == false);
}

TEST_CASE("tau_ba conjunction with T is identity") {	auto a = from_spec("G (o1[t]:tau = {T.}:tau).");
	auto and_T = a & tau_one();
	CHECK(and_T.is_zero() == a.is_zero());
}

TEST_CASE("tau_ba disjunction with F is identity") {	auto a = from_spec("G (o1[t]:tau = {T.}:tau).");
	auto or_F = a | tau_zero();
	CHECK(or_F.is_zero() == a.is_zero());
}

TEST_CASE("tau_ba and T does not change one-ness") {	auto a = from_spec("G (o1[t]:tau = {T.}:tau).");
	auto and_T = a & tau_one();
	CHECK(and_T.is_one() == a.is_one());
}

TEST_CASE("G(o1=T) | ~G(o1=T) is one") {	auto a = from_spec("G (o1[t]:tau = {T.}:tau).");
	CHECK((a | ~a).is_one() == true);
}

TEST_CASE("G(o1=T) & ~G(o1=T) is zero") {	auto a = from_spec("G (o1[t]:tau = {T.}:tau).");
	CHECK((a & ~a).is_zero() == true);
}

TEST_CASE("G(o1=T) ^ G(o1=T) is zero") {	auto a = from_spec("G (o1[t]:tau = {T.}:tau).");
	CHECK((a ^ a).is_zero() == true);
}

TEST_CASE("is_tau_closed on G(o1=T) spec") {	auto b = from_spec("G (o1[t]:tau = {T.}:tau).");
	CHECK(is_tau_closed(b));
}

} // TEST_SUITE LTL integration
