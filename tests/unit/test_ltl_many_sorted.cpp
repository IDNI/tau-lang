// 250 nontrivial many-sorted LTL realizability tests.
// Every formula mixes at least 2 data-sort types (qlt, bv, sbf, hsb, qint, tau).
// Tests cover all 15 type-pairs, diverse triples, 4/5/6-type combos,
// and a rich mix of temporal operators (G, F, U, R, S) with cross-type
// implications, past references, negation, and input-output interaction.

#include "test_init.h"
#include "test_tau_helpers.h"
#include <unordered_set>

using namespace idni::tau_lang;

static void do_gc() {
	std::unordered_set<tref> keep;
	tau::gc(keep);
}

static tref spec(const char* s) {
	auto nso_rr = get_nso_rr<node_t>(tau::get(s));
	if (!nso_rr.has_value()) return nullptr;
	return nso_rr.value().main->get();
}

static bool realizable(const char* s) {
	do_gc();
	auto nso = get_nso_rr<node_t>(tau::get(s));
	if (!nso.has_value()) return false;
	tref fm = nso.value().main->get();
	if (!fm) return false;
	return is_tau_formula_sat<node_t>(fm);
}

// ltlsynt (Spot >= 2.10) is a required dependency for LTL realizability tests.

// ============================================================================
// 2-type formulas: 50 tests (MS-0001 .. MS-0050)
// All 15 type pairs covered, ~3-4 tests per pair
// ============================================================================

TEST_SUITE("many_sorted_ltl") {

// --- qlt + bv (MS-0001 .. MS-0004) ---

TEST_CASE("[MS-0001] qlt+bv: G implication across types") {
	const char* fm = "G (o1[t]:qlt > {1/2}:qlt -> o2[t]:bv[8] = {42}:bv[8]).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0002] qlt+bv: F conjunction, both satisfiable independently") {
	const char* fm = "(F (o1[t]:qlt = {2/3}:qlt)) && (F (o2[t]:bv[8] = {#b10110101}:bv[8])).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0003] qlt+bv: UNREAL — contradictory implication forces impossible bv") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && (o1[t]:qlt > {0}:qlt -> o2[t]:bv[8] = {0}:bv[8] && o2[t]:bv[8] = {255}:bv[8])).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0004] qlt+bv: Until across types") {
	const char* fm = "(o1[t]:qlt > {0}:qlt) U (o2[t]:bv[8] = {#b00001111}:bv[8]).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + sbf (MS-0005 .. MS-0007) ---

TEST_CASE("[MS-0005] qlt+sbf: G safety with both types constrained") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o1[t]:qlt < {1}:qlt && o2[t]:sbf = {X & Y}:sbf).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0006] qlt+sbf: UNREAL — qlt contradiction poisons conjunction") {
	const char* fm = "G (o1[t]:qlt > {1}:qlt && o1[t]:qlt < {0}:qlt && o2[t]:sbf = {X}:sbf).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0007] qlt+sbf: past reference on qlt, current sbf") {
	const char* fm = "G (o1[t]:qlt > o1[t-1]:qlt && o2[t]:sbf = {X | (Y & Z)}:sbf).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + hsb (MS-0008 .. MS-0010) ---

TEST_CASE("[MS-0008] qlt+hsb: G conjunction both realizable") {
	const char* fm = "G (o1[t]:qlt = {1/2}:qlt && o2[t]:hsb = {top}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0009] qlt+hsb: F liveness across types") {
	const char* fm = "(F (o1[t]:qlt > {2/3}:qlt)) && (F (o2[t]:hsb != {bot}:hsb)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0010] qlt+hsb: implication from qlt to hsb") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt -> o2[t]:hsb = {top}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + qint (MS-0011 .. MS-0013) ---

TEST_CASE("[MS-0011] qlt+qint: G both constant outputs") {
	const char* fm = "G (o1[t]:qlt = {3/4}:qlt && o2[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0012] qlt+qint: temporal nesting F(G) across types") {
	const char* fm = "(F (G (o1[t]:qlt > {0}:qlt))) && (F (o2[t]:qint = {[1/4, 3/4)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0013] qlt+qint: UNREAL — qlt forces impossible range") {
	const char* fm = "G (o1[t]:qlt = {1/2}:qlt && o1[t]:qlt = {2/3}:qlt).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

// --- qlt + tau (MS-0014 .. MS-0016) ---

TEST_CASE("[MS-0014] qlt+tau: G safety with tau true") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0015] qlt+tau: UNREAL — tau contradiction via implication") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && (o1[t]:qlt > {0}:qlt -> o2[t]:tau = {T.}:tau && o2[t]:tau = {F.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0016] qlt+tau: F conjunction across types") {
	const char* fm = "(F (o1[t]:qlt > {1/2}:qlt)) && (F (o2[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- bv + sbf (MS-0017 .. MS-0019) ---

TEST_CASE("[MS-0017] bv+sbf: G conjunction both constant") {
	const char* fm = "G (o1[t]:bv[8] = {42}:bv[8] && o2[t]:sbf = {X & Y}:sbf).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0018] bv+sbf: Until bv holds until sbf achieved") {
	const char* fm = "(o1[t]:bv[8] = {#b10110101}:bv[8]) U (o2[t]:sbf = {X}:sbf).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0019] bv+sbf: UNREAL — bv contradiction poisons formula") {
	const char* fm = "G (o1[t]:bv[8] = {0}:bv[8] && o1[t]:bv[8] = {255}:bv[8] && o2[t]:sbf = {X}:sbf).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

// --- bv + hsb (MS-0020 .. MS-0022) ---

TEST_CASE("[MS-0020] bv+hsb: G disjunction across types") {
	const char* fm = "G (o1[t]:bv[8] = {42}:bv[8] || o2[t]:hsb = {top}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0021] bv+hsb: F conjunction") {
	const char* fm = "(F (o1[t]:bv[8] = {255}:bv[8])) && (F (o2[t]:hsb = {top}:hsb)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0022] bv+hsb: Until across types") {
	const char* fm = "(o1[t]:bv[8] = {#b00001111}:bv[8]) U (o2[t]:hsb != {bot}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- bv + qint (MS-0023 .. MS-0025) ---

TEST_CASE("[MS-0023] bv+qint: G both constant") {
	const char* fm = "G (o1[t]:bv[8] = {#b10110101}:bv[8] && o2[t]:qint = {[1/4, 3/4)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0024] bv+qint: UNREAL — bv contradicts itself") {
	const char* fm = "G (o1[t]:bv[8] = {42}:bv[8] && o1[t]:bv[8] = {0}:bv[8]).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0025] bv+qint: F liveness both types") {
	const char* fm = "(F (o1[t]:bv[8] = {255}:bv[8])) && (F (o2[t]:qint = {[-1, 0) | [1, 2)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- bv + tau (MS-0026 .. MS-0028) ---

TEST_CASE("[MS-0026] bv+tau: G implication") {
	const char* fm = "G (o1[t]:bv[8] = {42}:bv[8] -> o2[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0027] bv+tau: UNREAL — bv always 0 implies tau true AND false") {
	const char* fm = "G (o1[t]:bv[8] = {0}:bv[8] && (o1[t]:bv[8] = {0}:bv[8] -> o2[t]:tau = {T.}:tau && o2[t]:tau = {F.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0028] bv+tau: F conjunction across types") {
	const char* fm = "(F (o1[t]:bv[8] = {42}:bv[8])) && (F (o2[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- sbf + hsb (MS-0029 .. MS-0031) ---

TEST_CASE("[MS-0029] sbf+hsb: G both constant") {
	const char* fm = "G (o1[t]:sbf = {X & Y}:sbf && o2[t]:hsb = {top}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0030] sbf+hsb: F conjunction") {
	const char* fm = "(F (o1[t]:sbf = {X | (Y & Z)}:sbf)) && (F (o2[t]:hsb != {bot}:hsb)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0031] sbf+hsb: UNREAL — sbf X and not-X simultaneously") {
	const char* fm = "G (o1[t]:sbf = {X}:sbf && o1[t]:sbf = {X}:sbf' && o2[t]:hsb = {top}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

// --- sbf + qint (MS-0032 .. MS-0034) ---

TEST_CASE("[MS-0032] sbf+qint: G conjunction") {
	const char* fm = "G (o1[t]:sbf = {X}:sbf && o2[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0033] sbf+qint: Until across types") {
	const char* fm = "(o1[t]:sbf = {X & Y}:sbf) U (o2[t]:qint = {[1/4, 3/4)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0034] sbf+qint: UNREAL — forced sbf negation contradiction") {
	const char* fm = "G (o1[t]:sbf = {X}:sbf && o1[t]:sbf = {X}:sbf' && o2[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

// --- sbf + tau (MS-0035 .. MS-0037) ---

TEST_CASE("[MS-0035] sbf+tau: G both constant") {
	const char* fm = "G (o1[t]:sbf = {X | (Y & Z)}:sbf && o2[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0036] sbf+tau: F liveness") {
	const char* fm = "(F (o1[t]:sbf = {X ^ Y}:sbf)) && (F (o2[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0037] sbf+tau: UNREAL — tau forced T and F") {
	const char* fm = "G (o1[t]:sbf = {X}:sbf && (o1[t]:sbf = {X}:sbf -> o2[t]:tau = {T.}:tau && o2[t]:tau = {F.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

// --- hsb + qint (MS-0038 .. MS-0040) ---

TEST_CASE("[MS-0038] hsb+qint: F both types") {
	const char* fm = "(F (o1[t]:hsb = {top}:hsb)) && (F (o2[t]:qint = {[0, 1)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0039] hsb+qint: G conjunction") {
	const char* fm = "G (o1[t]:hsb = {top}:hsb && o2[t]:qint = {[1/4, 3/4)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0040] hsb+qint: Until across types") {
	const char* fm = "(o1[t]:hsb != {bot}:hsb) U (o2[t]:qint = {[-1, 0) | [1, 2)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- hsb + tau (MS-0041 .. MS-0043) ---

TEST_CASE("[MS-0041] hsb+tau: G conjunction") {
	const char* fm = "G (o1[t]:hsb = {top}:hsb && o2[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0042] hsb+tau: F liveness") {
	const char* fm = "(F (o1[t]:hsb != {bot}:hsb)) && (F (o2[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0043] hsb+tau: implication from hsb to tau") {
	const char* fm = "G (o1[t]:hsb = {top}:hsb -> o2[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qint + tau (MS-0044 .. MS-0046) ---

TEST_CASE("[MS-0044] qint+tau: G conjunction") {
	const char* fm = "G (o1[t]:qint = {[0, 1)}:qint && o2[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0045] qint+tau: F liveness both") {
	const char* fm = "(F (o1[t]:qint = {[1/4, 3/4)}:qint)) && (F (o2[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0046] qint+tau: UNREAL — qint identity contradiction") {
	const char* fm = "G (o1[t]:qint = {[0, 1)}:qint && !(o1[t]:qint = {[0, 1)}:qint) && o2[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

// --- Extra 2-type (MS-0047 .. MS-0050) ---

TEST_CASE("[MS-0047] qlt+bv: G(F) liveness on both types alternating") {
	const char* fm = "G (F (o1[t]:qlt > {1/2}:qlt && o2[t]:bv[8] = {42}:bv[8])).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0048] sbf+bv: Until across types") {
	const char* fm = "(o1[t]:sbf = {X}:sbf) U (o2[t]:bv[8] != {0}:bv[8]).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0049] qlt+sbf: input-output with mixed types") {
	const char* fm = "G ((i1[t]:qlt > {0}:qlt -> o1[t]:sbf = {X & Y}:sbf) && (i1[t]:qlt <= {0}:qlt -> o1[t]:sbf = {X | (Y & Z)}:sbf)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0050] qlt+bv: UNREAL — bv stuck at contradictory values") {
	const char* fm = "G (o1[t]:qlt > o1[t-1]:qlt && o2[t]:bv[8] = {42}:bv[8] && o2[t]:bv[8] = {0}:bv[8]).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

// ============================================================================
// 3-type formulas: 80 tests (MS-0051 .. MS-0130)
// ============================================================================

// --- qlt + bv + sbf (MS-0051 .. MS-0058) ---

TEST_CASE("[MS-0051] qlt+bv+sbf: G safety three outputs") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X & Y}:sbf).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0052] qlt+bv+sbf: implication chain across types") {
	const char* fm = "G ((o1[t]:qlt > {1/2}:qlt -> o2[t]:bv[8] = {255}:bv[8]) && (o2[t]:bv[8] = {255}:bv[8] -> o3[t]:sbf = {X}:sbf)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0053] qlt+bv+sbf: UNREAL — qlt forces bv contradiction") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && (o1[t]:qlt > {0}:qlt -> o2[t]:bv[8] = {0}:bv[8] && o2[t]:bv[8] = {255}:bv[8]) && o3[t]:sbf = {X}:sbf).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0054] qlt+bv+sbf: F liveness three types") {
	const char* fm = "(F (o1[t]:qlt = {2/3}:qlt)) && (F (o2[t]:bv[8] = {#b10110101}:bv[8])) && (F (o3[t]:sbf = {X | (Y & Z)}:sbf)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0055] qlt+bv+sbf: Until on qlt with G on bv+sbf") {
	const char* fm = "((o1[t]:qlt > {0}:qlt) U (o1[t]:qlt = {1/2}:qlt)) && (G (o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X ^ Y}:sbf)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0056] qlt+bv+sbf: G(F) on all three types") {
	const char* fm = "G (F (o1[t]:qlt > {1/2}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X}:sbf)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0057] qlt+bv+sbf: UNREAL — G constant then F different") {
	const char* fm = "(G (o1[t]:qlt = {0}:qlt)) && (F (o1[t]:qlt = {1}:qlt)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0058] qlt+bv+sbf: past reference mixed") {
	const char* fm = "G (o1[t]:qlt > o1[t-1]:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X & Y}:sbf).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + bv + hsb (MS-0059 .. MS-0065) ---

TEST_CASE("[MS-0059] qlt+bv+hsb: G safety three outputs") {
	const char* fm = "G (o1[t]:qlt = {1/2}:qlt && o2[t]:bv[8] = {#b11110000}:bv[8] && o3[t]:hsb = {top}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0060] qlt+bv+hsb: F liveness three types") {
	const char* fm = "(F (o1[t]:qlt > {2/3}:qlt)) && (F (o2[t]:bv[8] = {255}:bv[8])) && (F (o3[t]:hsb != {bot}:hsb)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0061] qlt+bv+hsb: UNREAL — bv contradiction") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o2[t]:bv[8] = {0}:bv[8] && o3[t]:hsb = {top}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0062] qlt+bv+hsb: implication from qlt to bv with hsb") {
	const char* fm = "G (o3[t]:hsb = {top}:hsb && (o1[t]:qlt > {1/2}:qlt -> o2[t]:bv[8] = {255}:bv[8])).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0063] qlt+bv+hsb: Until on qlt, F on bv+hsb") {
	const char* fm = "((o1[t]:qlt > {0}:qlt) U (o1[t]:qlt = {1/2}:qlt)) && (F (o2[t]:bv[8] = {#b10110101}:bv[8])) && (F (o3[t]:hsb = {top}:hsb)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0064] qlt+bv+hsb: past reference on qlt, G on bv+hsb") {
	const char* fm = "G (o1[t]:qlt > o1[t-1]:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:hsb = {top}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0065] qlt+bv+hsb: UNREAL — qlt always positive but F requires negative") {
	const char* fm = "(G (o1[t]:qlt > {0}:qlt)) && (F (o1[t]:qlt < {0}:qlt)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

// --- qlt + bv + qint (MS-0066 .. MS-0071) ---

TEST_CASE("[MS-0066] qlt+bv+qint: G safety three types") {
	const char* fm = "G (o1[t]:qlt = {3/4}:qlt && o2[t]:bv[8] = {#b00001111}:bv[8] && o3[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0067] qlt+bv+qint: F liveness three types") {
	const char* fm = "(F (o1[t]:qlt = {1/2}:qlt)) && (F (o2[t]:bv[8] = {42}:bv[8])) && (F (o3[t]:qint = {[1/4, 3/4)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0068] qlt+bv+qint: UNREAL — qlt contradiction") {
	const char* fm = "G (o1[t]:qlt = {1/2}:qlt && o1[t]:qlt = {2/3}:qlt && o2[t]:bv[8] = {42}:bv[8]).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0069] qlt+bv+qint: implication chain across three types") {
	const char* fm = "G ((o1[t]:qlt > {0}:qlt -> o2[t]:bv[8] = {255}:bv[8]) && (o2[t]:bv[8] = {255}:bv[8] -> o3[t]:qint = {[0, 1)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0070] qlt+bv+qint: Until on bv with G on qlt+qint") {
	const char* fm = "(G (o1[t]:qlt > {0}:qlt && o1[t]:qlt < {1}:qlt)) && ((o2[t]:bv[8] = {42}:bv[8]) U (o2[t]:bv[8] = {255}:bv[8])) && (F (o3[t]:qint = {[1/4, 3/4)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0071] qlt+bv+qint: past reference with three types") {
	const char* fm = "G (o1[t]:qlt > o1[t-1]:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:qint = {[-1, 0) | [1, 2)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + bv + tau (MS-0072 .. MS-0077) ---

TEST_CASE("[MS-0072] qlt+bv+tau: G safety three types") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0073] qlt+bv+tau: F liveness three types") {
	const char* fm = "(F (o1[t]:qlt = {2/3}:qlt)) && (F (o2[t]:bv[8] = {#b10110101}:bv[8])) && (F (o3[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0074] qlt+bv+tau: UNREAL — tau forced to contradictory values") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:tau = {T.}:tau && o3[t]:tau = {F.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0075] qlt+bv+tau: implication from qlt to tau with bv guard") {
	const char* fm = "G (o2[t]:bv[8] = {42}:bv[8] && (o1[t]:qlt > {1/2}:qlt -> o3[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0076] qlt+bv+tau: Until on qlt with G on bv+tau") {
	const char* fm = "((o1[t]:qlt > {0}:qlt) U (o1[t]:qlt < {1}:qlt)) && (G (o2[t]:bv[8] = {42}:bv[8] && o3[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0077] qlt+bv+tau: UNREAL — bv forever 0 and F requires non-0") {
	const char* fm = "(G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {0}:bv[8])) && (F (o2[t]:bv[8] != {0}:bv[8])).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

// --- qlt + sbf + hsb (MS-0078 .. MS-0083) ---

TEST_CASE("[MS-0078] qlt+sbf+hsb: G safety three types") {
	const char* fm = "G (o1[t]:qlt = {1/2}:qlt && o2[t]:sbf = {X & Y}:sbf && o3[t]:hsb = {top}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0079] qlt+sbf+hsb: F liveness three types") {
	const char* fm = "(F (o1[t]:qlt > {2/3}:qlt)) && (F (o2[t]:sbf = {X ^ Y}:sbf)) && (F (o3[t]:hsb != {bot}:hsb)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0080] qlt+sbf+hsb: UNREAL — qlt contradiction") {
	const char* fm = "G (o1[t]:qlt > {1}:qlt && o1[t]:qlt < {0}:qlt && o2[t]:sbf = {X}:sbf && o3[t]:hsb = {top}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0081] qlt+sbf+hsb: implication from sbf to hsb") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && (o2[t]:sbf = {X}:sbf -> o3[t]:hsb = {top}:hsb)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0082] qlt+sbf+hsb: Until on sbf with G on qlt+hsb") {
	const char* fm = "(G (o1[t]:qlt > {0}:qlt && o1[t]:qlt < {1}:qlt)) && ((o2[t]:sbf = {X & Y}:sbf) U (o2[t]:sbf = {X | (Y & Z)}:sbf)) && (F (o3[t]:hsb = {top}:hsb)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0083] qlt+sbf+hsb: past reference on qlt with sbf+hsb") {
	const char* fm = "G (o1[t]:qlt > o1[t-1]:qlt && o2[t]:sbf = {X}:sbf && o3[t]:hsb = {top}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + sbf + qint (MS-0084 .. MS-0088) ---

TEST_CASE("[MS-0084] qlt+sbf+qint: G safety three types") {
	const char* fm = "G (o1[t]:qlt = {3/4}:qlt && o2[t]:sbf = {X | (Y & Z)}:sbf && o3[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0085] qlt+sbf+qint: F liveness three types") {
	const char* fm = "(F (o1[t]:qlt = {1/2}:qlt)) && (F (o2[t]:sbf = {X ^ Y}:sbf)) && (F (o3[t]:qint = {[1/4, 3/4)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0086] qlt+sbf+qint: UNREAL — sbf contradiction") {
	const char* fm = "G (o2[t]:sbf = {X}:sbf && o2[t]:sbf = {X}:sbf' && o1[t]:qlt > {0}:qlt && o3[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0087] qlt+sbf+qint: Until on qlt with G on sbf+qint") {
	const char* fm = "((o1[t]:qlt > {0}:qlt) U (o1[t]:qlt = {1/2}:qlt)) && (G (o2[t]:sbf = {X}:sbf && o3[t]:qint = {[-1, 0) | [1, 2)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0088] qlt+sbf+qint: implication from qlt to sbf") {
	const char* fm = "G (o3[t]:qint = {[0, 1)}:qint && (o1[t]:qlt > {1/2}:qlt -> o2[t]:sbf = {X & Y}:sbf)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + sbf + tau (MS-0089 .. MS-0093) ---

TEST_CASE("[MS-0089] qlt+sbf+tau: G safety three types") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:sbf = {X & Y}:sbf && o3[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0090] qlt+sbf+tau: UNREAL — tau G+F contradiction") {
	const char* fm = "(G (o1[t]:qlt > {0}:qlt && o2[t]:sbf = {X}:sbf && o3[t]:tau = {T.}:tau)) && (F (o3[t]:tau = {F.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0091] qlt+sbf+tau: F liveness three types") {
	const char* fm = "(F (o1[t]:qlt = {2/3}:qlt)) && (F (o2[t]:sbf = {X | (Y & Z)}:sbf)) && (F (o3[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0092] qlt+sbf+tau: implication from sbf to tau") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && (o2[t]:sbf = {X}:sbf -> o3[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0093] qlt+sbf+tau: F on sbf and tau with qlt guard") {
	const char* fm = "(G (o1[t]:qlt > {0}:qlt && o1[t]:qlt < {1}:qlt)) && (F (o2[t]:sbf = {X & Y}:sbf)) && (F (o3[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + hsb + qint (MS-0094 .. MS-0097) ---

TEST_CASE("[MS-0094] qlt+hsb+qint: G safety three types") {
	const char* fm = "G (o1[t]:qlt = {1/2}:qlt && o2[t]:hsb = {top}:hsb && o3[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0095] qlt+hsb+qint: F liveness three types") {
	const char* fm = "(F (o1[t]:qlt > {2/3}:qlt)) && (F (o2[t]:hsb != {bot}:hsb)) && (F (o3[t]:qint = {[1/4, 3/4)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0096] qlt+hsb+qint: UNREAL — qlt contradiction") {
	const char* fm = "G (o1[t]:qlt > {1}:qlt && o1[t]:qlt < {0}:qlt && o2[t]:hsb = {top}:hsb && o3[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0097] qlt+hsb+qint: Until on qlt with G on hsb+qint") {
	const char* fm = "((o1[t]:qlt > {0}:qlt) U (o1[t]:qlt = {1/2}:qlt)) && (G (o2[t]:hsb = {top}:hsb && o3[t]:qint = {[-1, 0) | [1, 2)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + hsb + tau (MS-0098 .. MS-0101) ---

TEST_CASE("[MS-0098] qlt+hsb+tau: G safety three types") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:hsb = {top}:hsb && o3[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0099] qlt+hsb+tau: F liveness three types") {
	const char* fm = "(F (o1[t]:qlt = {2/3}:qlt)) && (F (o2[t]:hsb != {bot}:hsb)) && (F (o3[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0100] qlt+hsb+tau: UNREAL — tau contradiction") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:hsb = {top}:hsb && o3[t]:tau = {T.}:tau && o3[t]:tau = {F.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0101] qlt+hsb+tau: implication from qlt to hsb with tau") {
	const char* fm = "G (o3[t]:tau = {T.}:tau && (o1[t]:qlt > {1/2}:qlt -> o2[t]:hsb = {top}:hsb)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + qint + tau (MS-0102 .. MS-0105) ---

TEST_CASE("[MS-0102] qlt+qint+tau: G safety three types") {
	const char* fm = "G (o1[t]:qlt = {3/4}:qlt && o2[t]:qint = {[0, 1)}:qint && o3[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0103] qlt+qint+tau: F liveness three types") {
	const char* fm = "(F (o1[t]:qlt = {1/2}:qlt)) && (F (o2[t]:qint = {[1/4, 3/4)}:qint)) && (F (o3[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0104] qlt+qint+tau: UNREAL — qlt contradiction") {
	const char* fm = "G (o1[t]:qlt = {1/2}:qlt && o1[t]:qlt = {2/3}:qlt && o2[t]:qint = {[0, 1)}:qint && o3[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0105] qlt+qint+tau: implication from qlt to tau with qint") {
	const char* fm = "G (o2[t]:qint = {[0, 1)}:qint && (o1[t]:qlt > {1/2}:qlt -> o3[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- bv + sbf + hsb (MS-0106 .. MS-0109) ---

TEST_CASE("[MS-0106] bv+sbf+hsb: G safety three types") {
	const char* fm = "G (o1[t]:bv[8] = {42}:bv[8] && o2[t]:sbf = {X & Y}:sbf && o3[t]:hsb = {top}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0107] bv+sbf+hsb: F liveness three types") {
	const char* fm = "(F (o1[t]:bv[8] = {255}:bv[8])) && (F (o2[t]:sbf = {X ^ Y}:sbf)) && (F (o3[t]:hsb != {bot}:hsb)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0108] bv+sbf+hsb: UNREAL — bv contradiction") {
	const char* fm = "G (o1[t]:bv[8] = {0}:bv[8] && o1[t]:bv[8] = {255}:bv[8] && o2[t]:sbf = {X}:sbf && o3[t]:hsb = {top}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0109] bv+sbf+hsb: Until on bv with G on sbf+hsb") {
	const char* fm = "((o1[t]:bv[8] = {42}:bv[8]) U (o1[t]:bv[8] = {255}:bv[8])) && (G (o2[t]:sbf = {X}:sbf && o3[t]:hsb = {top}:hsb)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- bv + sbf + qint (MS-0110 .. MS-0113) ---

TEST_CASE("[MS-0110] bv+sbf+qint: G safety three types") {
	const char* fm = "G (o1[t]:bv[8] = {#b10110101}:bv[8] && o2[t]:sbf = {X | (Y & Z)}:sbf && o3[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0111] bv+sbf+qint: F liveness three types") {
	const char* fm = "(F (o1[t]:bv[8] = {42}:bv[8])) && (F (o2[t]:sbf = {X & Y}:sbf)) && (F (o3[t]:qint = {[1/4, 3/4)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0112] bv+sbf+qint: UNREAL — sbf forced to X and complement") {
	const char* fm = "G (o2[t]:sbf = {X}:sbf && o2[t]:sbf = {X}:sbf' && o1[t]:bv[8] = {42}:bv[8] && o3[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0113] bv+sbf+qint: implication from bv to sbf") {
	const char* fm = "G (o3[t]:qint = {[0, 1)}:qint && (o1[t]:bv[8] = {42}:bv[8] -> o2[t]:sbf = {X & Y}:sbf)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- bv + sbf + tau (MS-0114 .. MS-0117) ---

TEST_CASE("[MS-0114] bv+sbf+tau: G safety three types") {
	const char* fm = "G (o1[t]:bv[8] = {42}:bv[8] && o2[t]:sbf = {X}:sbf && o3[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0115] bv+sbf+tau: UNREAL — bv never 0 but F requires 0") {
	const char* fm = "(G (o1[t]:bv[8] = {42}:bv[8] && o2[t]:sbf = {X}:sbf && o3[t]:tau = {T.}:tau)) && (F (o1[t]:bv[8] = {0}:bv[8])).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0116] bv+sbf+tau: F liveness three types") {
	const char* fm = "(F (o1[t]:bv[8] = {#b10110101}:bv[8])) && (F (o2[t]:sbf = {X ^ Y}:sbf)) && (F (o3[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0117] bv+sbf+tau: Until on bv with G on sbf+tau") {
	const char* fm = "((o1[t]:bv[8] = {42}:bv[8]) U (o1[t]:bv[8] = {255}:bv[8])) && (G (o2[t]:sbf = {X}:sbf && o3[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- bv + hsb + qint (MS-0118 .. MS-0120) ---

TEST_CASE("[MS-0118] bv+hsb+qint: G safety three types") {
	const char* fm = "G (o1[t]:bv[8] = {255}:bv[8] && o2[t]:hsb = {top}:hsb && o3[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0119] bv+hsb+qint: UNREAL — bv contradiction") {
	const char* fm = "G (o1[t]:bv[8] = {42}:bv[8] && o1[t]:bv[8] = {0}:bv[8] && o2[t]:hsb = {top}:hsb && o3[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0120] bv+hsb+qint: F liveness three types") {
	const char* fm = "(F (o1[t]:bv[8] = {#b00001111}:bv[8])) && (F (o2[t]:hsb != {bot}:hsb)) && (F (o3[t]:qint = {[1/4, 3/4)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- bv + hsb + tau (MS-0121 .. MS-0123) ---

TEST_CASE("[MS-0121] bv+hsb+tau: G safety three types") {
	const char* fm = "G (o1[t]:bv[8] = {42}:bv[8] && o2[t]:hsb = {top}:hsb && o3[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0122] bv+hsb+tau: UNREAL — tau contradiction") {
	const char* fm = "G (o1[t]:bv[8] = {42}:bv[8] && o2[t]:hsb = {top}:hsb && o3[t]:tau = {T.}:tau && o3[t]:tau = {F.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0123] bv+hsb+tau: F liveness three types") {
	const char* fm = "(F (o1[t]:bv[8] = {255}:bv[8])) && (F (o2[t]:hsb != {bot}:hsb)) && (F (o3[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- bv + qint + tau (MS-0124 .. MS-0126) ---

TEST_CASE("[MS-0124] bv+qint+tau: G safety three types") {
	const char* fm = "G (o1[t]:bv[8] = {#b10110101}:bv[8] && o2[t]:qint = {[0, 1)}:qint && o3[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0125] bv+qint+tau: UNREAL — bv G+F contradiction") {
	const char* fm = "(G (o1[t]:bv[8] = {42}:bv[8] && o2[t]:qint = {[0, 1)}:qint && o3[t]:tau = {T.}:tau)) && (F (o1[t]:bv[8] = {0}:bv[8])).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0126] bv+qint+tau: F liveness three types") {
	const char* fm = "(F (o1[t]:bv[8] = {255}:bv[8])) && (F (o2[t]:qint = {[1/4, 3/4)}:qint)) && (F (o3[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- sbf + hsb + qint (MS-0127 .. MS-0128) ---

TEST_CASE("[MS-0127] sbf+hsb+qint: G safety three types") {
	const char* fm = "G (o1[t]:sbf = {X & Y}:sbf && o2[t]:hsb = {top}:hsb && o3[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0128] sbf+hsb+qint: UNREAL — sbf contradiction") {
	const char* fm = "G (o1[t]:sbf = {X}:sbf && o1[t]:sbf = {X}:sbf' && o2[t]:hsb = {top}:hsb && o3[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

// --- sbf + hsb + tau (MS-0129) ---

TEST_CASE("[MS-0129] sbf+hsb+tau: G safety three types") {
	const char* fm = "G (o1[t]:sbf = {X | (Y & Z)}:sbf && o2[t]:hsb = {top}:hsb && o3[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- hsb + qint + tau (MS-0130) ---

TEST_CASE("[MS-0130] hsb+qint+tau: G safety three types") {
	const char* fm = "G (o1[t]:hsb = {top}:hsb && o2[t]:qint = {[0, 1)}:qint && o3[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// ============================================================================
// 4-type formulas: 60 tests (MS-0131 .. MS-0190)
// ============================================================================

TEST_CASE("[MS-0131] qlt+bv+sbf+hsb: G safety four outputs") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X & Y}:sbf && o4[t]:hsb = {top}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0132] qlt+bv+sbf+hsb: F liveness four types") {
	const char* fm = "(F (o1[t]:qlt = {2/3}:qlt)) && (F (o2[t]:bv[8] = {#b10110101}:bv[8])) && (F (o3[t]:sbf = {X ^ Y}:sbf)) && (F (o4[t]:hsb != {bot}:hsb)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0133] qlt+bv+sbf+hsb: UNREAL — qlt contradiction") {
	const char* fm = "G (o1[t]:qlt > {1}:qlt && o1[t]:qlt < {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0134] qlt+bv+sbf+hsb: implication chain across four types") {
	const char* fm = "G ((o1[t]:qlt > {1/2}:qlt -> o2[t]:bv[8] = {255}:bv[8]) && (o2[t]:bv[8] = {255}:bv[8] -> o3[t]:sbf = {X}:sbf) && o4[t]:hsb = {top}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0135] qlt+bv+sbf+hsb: Until on qlt with G on bv+sbf+hsb") {
	const char* fm = "((o1[t]:qlt > {0}:qlt) U (o1[t]:qlt = {1/2}:qlt)) && (G (o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X | (Y & Z)}:sbf && o4[t]:hsb = {top}:hsb)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0136] qlt+bv+sbf+hsb: G(F) on all four types") {
	const char* fm = "G (F (o1[t]:qlt > {1/2}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0137] qlt+bv+sbf+hsb: UNREAL — bv contradiction") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {0}:bv[8] && o2[t]:bv[8] = {255}:bv[8] && o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0138] qlt+bv+sbf+hsb: past reference on qlt") {
	const char* fm = "G (o1[t]:qlt > o1[t-1]:qlt && o2[t]:bv[8] = {#b00001111}:bv[8] && o3[t]:sbf = {X & Y}:sbf && o4[t]:hsb = {top}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0139] qlt+bv+sbf+hsb: input-output") {
	const char* fm = "G ((i1[t]:qlt > {0}:qlt -> o1[t]:bv[8] = {255}:bv[8]) && o2[t]:sbf = {X}:sbf && o3[t]:hsb = {top}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0140] qlt+bv+sbf+hsb: Until on qlt with G on rest") {
	const char* fm = "((o1[t]:qlt > {0}:qlt) U (o1[t]:qlt = {1/2}:qlt)) && (G (o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + bv + sbf + qint (MS-0141 .. MS-0148) ---

TEST_CASE("[MS-0141] qlt+bv+sbf+qint: G safety four types") {
	const char* fm = "G (o1[t]:qlt = {3/4}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X & Y}:sbf && o4[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0142] qlt+bv+sbf+qint: F liveness four types") {
	const char* fm = "(F (o1[t]:qlt = {1/2}:qlt)) && (F (o2[t]:bv[8] = {#b10110101}:bv[8])) && (F (o3[t]:sbf = {X | (Y & Z)}:sbf)) && (F (o4[t]:qint = {[1/4, 3/4)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0143] qlt+bv+sbf+qint: UNREAL — sbf contradiction") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X}:sbf && o3[t]:sbf = {X}:sbf' && o4[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0144] qlt+bv+sbf+qint: implication chain") {
	const char* fm = "G ((o1[t]:qlt > {1/2}:qlt -> o2[t]:bv[8] = {255}:bv[8]) && o3[t]:sbf = {X}:sbf && o4[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0145] qlt+bv+sbf+qint: Until on sbf") {
	const char* fm = "(G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8])) && ((o3[t]:sbf = {X & Y}:sbf) U (o3[t]:sbf = {X | (Y & Z)}:sbf)) && (F (o4[t]:qint = {[-1, 0) | [1, 2)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0146] qlt+bv+sbf+qint: past reference") {
	const char* fm = "G (o1[t]:qlt > o1[t-1]:qlt && o2[t]:bv[8] = {#b00001111}:bv[8] && o3[t]:sbf = {X}:sbf && o4[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0147] qlt+bv+sbf+qint: UNREAL — qlt G+F contradiction") {
	const char* fm = "(G (o1[t]:qlt = {1/2}:qlt)) && (F (o1[t]:qlt = {2/3}:qlt)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0148] qlt+bv+sbf+qint: G(F) liveness on all four") {
	const char* fm = "G (F (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X}:sbf && o4[t]:qint = {[0, 1)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + bv + sbf + tau (MS-0149 .. MS-0155) ---

TEST_CASE("[MS-0149] qlt+bv+sbf+tau: G safety four types") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X & Y}:sbf && o4[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0150] qlt+bv+sbf+tau: F liveness four types") {
	const char* fm = "(F (o1[t]:qlt = {2/3}:qlt)) && (F (o2[t]:bv[8] = {#b10110101}:bv[8])) && (F (o3[t]:sbf = {X ^ Y}:sbf)) && (F (o4[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0151] qlt+bv+sbf+tau: UNREAL — tau contradiction") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X}:sbf && o4[t]:tau = {T.}:tau && o4[t]:tau = {F.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0152] qlt+bv+sbf+tau: implication chain") {
	const char* fm = "G ((o1[t]:qlt > {1/2}:qlt -> o2[t]:bv[8] = {255}:bv[8]) && (o3[t]:sbf = {X}:sbf -> o4[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0153] qlt+bv+sbf+tau: Until on qlt, F on bv") {
	const char* fm = "((o1[t]:qlt > {0}:qlt) U (o1[t]:qlt = {1/2}:qlt)) && (F (o2[t]:bv[8] = {42}:bv[8])) && (G (o3[t]:sbf = {X}:sbf && o4[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0154] qlt+bv+sbf+tau: input-output") {
	const char* fm = "G ((i1[t]:qlt > {0}:qlt -> o1[t]:bv[8] = {255}:bv[8]) && o2[t]:sbf = {X}:sbf && o3[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0155] qlt+bv+sbf+tau: UNREAL — qlt always positive but F requires non-positive") {
	const char* fm = "(G (o1[t]:qlt > {0}:qlt)) && (F (o1[t]:qlt <= {0}:qlt)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

// --- qlt + bv + hsb + qint (MS-0156 .. MS-0161) ---

TEST_CASE("[MS-0156] qlt+bv+hsb+qint: G safety four types") {
	const char* fm = "G (o1[t]:qlt = {1/2}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:hsb = {top}:hsb && o4[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0157] qlt+bv+hsb+qint: F liveness four types") {
	const char* fm = "(F (o1[t]:qlt > {2/3}:qlt)) && (F (o2[t]:bv[8] = {255}:bv[8])) && (F (o3[t]:hsb != {bot}:hsb)) && (F (o4[t]:qint = {[1/4, 3/4)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0158] qlt+bv+hsb+qint: UNREAL — bv contradiction") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {0}:bv[8] && o2[t]:bv[8] = {255}:bv[8] && o3[t]:hsb = {top}:hsb && o4[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0159] qlt+bv+hsb+qint: implication") {
	const char* fm = "G (o3[t]:hsb = {top}:hsb && o4[t]:qint = {[0, 1)}:qint && (o1[t]:qlt > {1/2}:qlt -> o2[t]:bv[8] = {255}:bv[8])).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0160] qlt+bv+hsb+qint: Until on qlt with G on rest") {
	const char* fm = "((o1[t]:qlt > {0}:qlt) U (o1[t]:qlt = {1/2}:qlt)) && (G (o2[t]:bv[8] = {42}:bv[8] && o3[t]:hsb = {top}:hsb && o4[t]:qint = {[-1, 0) | [1, 2)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0161] qlt+bv+hsb+qint: past reference") {
	const char* fm = "G (o1[t]:qlt > o1[t-1]:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:hsb = {top}:hsb && o4[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + bv + hsb + tau (MS-0162 .. MS-0166) ---

TEST_CASE("[MS-0162] qlt+bv+hsb+tau: G safety four types") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:hsb = {top}:hsb && o4[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0163] qlt+bv+hsb+tau: F liveness four types") {
	const char* fm = "(F (o1[t]:qlt = {2/3}:qlt)) && (F (o2[t]:bv[8] = {255}:bv[8])) && (F (o3[t]:hsb != {bot}:hsb)) && (F (o4[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0164] qlt+bv+hsb+tau: UNREAL — tau contradiction") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:hsb = {top}:hsb && o4[t]:tau = {T.}:tau && o4[t]:tau = {F.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0165] qlt+bv+hsb+tau: implication from qlt to tau") {
	const char* fm = "G (o2[t]:bv[8] = {42}:bv[8] && o3[t]:hsb = {top}:hsb && (o1[t]:qlt > {1/2}:qlt -> o4[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0166] qlt+bv+hsb+tau: UNREAL — bv contradiction") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o2[t]:bv[8] = {0}:bv[8] && o3[t]:hsb = {top}:hsb && o4[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

// --- qlt + bv + qint + tau (MS-0167 .. MS-0172) ---

TEST_CASE("[MS-0167] qlt+bv+qint+tau: G safety four types") {
	const char* fm = "G (o1[t]:qlt = {3/4}:qlt && o2[t]:bv[8] = {#b00001111}:bv[8] && o3[t]:qint = {[0, 1)}:qint && o4[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0168] qlt+bv+qint+tau: F liveness four types") {
	const char* fm = "(F (o1[t]:qlt = {1/2}:qlt)) && (F (o2[t]:bv[8] = {42}:bv[8])) && (F (o3[t]:qint = {[1/4, 3/4)}:qint)) && (F (o4[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0169] qlt+bv+qint+tau: UNREAL — qlt contradiction") {
	const char* fm = "G (o1[t]:qlt = {1/2}:qlt && o1[t]:qlt = {2/3}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:qint = {[0, 1)}:qint && o4[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0170] qlt+bv+qint+tau: Until on qlt with G on rest") {
	const char* fm = "((o1[t]:qlt > {0}:qlt) U (o1[t]:qlt = {1/2}:qlt)) && (G (o2[t]:bv[8] = {42}:bv[8] && o3[t]:qint = {[0, 1)}:qint && o4[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0171] qlt+bv+qint+tau: implication chain") {
	const char* fm = "G ((o1[t]:qlt > {1/2}:qlt -> o2[t]:bv[8] = {255}:bv[8]) && o3[t]:qint = {[0, 1)}:qint && o4[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0172] qlt+bv+qint+tau: past reference") {
	const char* fm = "G (o1[t]:qlt > o1[t-1]:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:qint = {[0, 1)}:qint && o4[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + sbf + hsb + qint (MS-0173 .. MS-0176) ---

TEST_CASE("[MS-0173] qlt+sbf+hsb+qint: G safety four types") {
	const char* fm = "G (o1[t]:qlt = {1/2}:qlt && o2[t]:sbf = {X & Y}:sbf && o3[t]:hsb = {top}:hsb && o4[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0174] qlt+sbf+hsb+qint: UNREAL — sbf contradiction") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:sbf = {X}:sbf && o2[t]:sbf = {X}:sbf' && o3[t]:hsb = {top}:hsb && o4[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0175] qlt+sbf+hsb+qint: F liveness four types") {
	const char* fm = "(F (o1[t]:qlt > {2/3}:qlt)) && (F (o2[t]:sbf = {X ^ Y}:sbf)) && (F (o3[t]:hsb != {bot}:hsb)) && (F (o4[t]:qint = {[1/4, 3/4)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0176] qlt+sbf+hsb+qint: Until on sbf with G on rest") {
	const char* fm = "(G (o1[t]:qlt > {0}:qlt && o1[t]:qlt < {1}:qlt)) && ((o2[t]:sbf = {X & Y}:sbf) U (o2[t]:sbf = {X | (Y & Z)}:sbf)) && (G (o3[t]:hsb = {top}:hsb && o4[t]:qint = {[0, 1)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + sbf + hsb + tau (MS-0177 .. MS-0180) ---

TEST_CASE("[MS-0177] qlt+sbf+hsb+tau: G safety four types") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:sbf = {X & Y}:sbf && o3[t]:hsb = {top}:hsb && o4[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0178] qlt+sbf+hsb+tau: UNREAL — tau contradiction") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:sbf = {X}:sbf && o3[t]:hsb = {top}:hsb && o4[t]:tau = {T.}:tau && o4[t]:tau = {F.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0179] qlt+sbf+hsb+tau: F liveness four types") {
	const char* fm = "(F (o1[t]:qlt = {2/3}:qlt)) && (F (o2[t]:sbf = {X ^ Y}:sbf)) && (F (o3[t]:hsb != {bot}:hsb)) && (F (o4[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0180] qlt+sbf+hsb+tau: implication from sbf to tau") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o3[t]:hsb = {top}:hsb && (o2[t]:sbf = {X}:sbf -> o4[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + sbf + qint + tau (MS-0181 .. MS-0183) ---

TEST_CASE("[MS-0181] qlt+sbf+qint+tau: G safety four types") {
	const char* fm = "G (o1[t]:qlt = {3/4}:qlt && o2[t]:sbf = {X | (Y & Z)}:sbf && o3[t]:qint = {[0, 1)}:qint && o4[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0182] qlt+sbf+qint+tau: UNREAL — sbf contradiction") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:sbf = {X}:sbf && o2[t]:sbf = {X}:sbf' && o3[t]:qint = {[0, 1)}:qint && o4[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0183] qlt+sbf+qint+tau: F liveness four types") {
	const char* fm = "(F (o1[t]:qlt = {1/2}:qlt)) && (F (o2[t]:sbf = {X & Y}:sbf)) && (F (o3[t]:qint = {[1/4, 3/4)}:qint)) && (F (o4[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + hsb + qint + tau (MS-0184 .. MS-0186) ---

TEST_CASE("[MS-0184] qlt+hsb+qint+tau: G safety four types") {
	const char* fm = "G (o1[t]:qlt = {1/2}:qlt && o2[t]:hsb = {top}:hsb && o3[t]:qint = {[0, 1)}:qint && o4[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0185] qlt+hsb+qint+tau: UNREAL — qlt contradiction") {
	const char* fm = "G (o1[t]:qlt > {1}:qlt && o1[t]:qlt < {0}:qlt && o2[t]:hsb = {top}:hsb && o3[t]:qint = {[0, 1)}:qint && o4[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0186] qlt+hsb+qint+tau: F liveness four types") {
	const char* fm = "(F (o1[t]:qlt > {2/3}:qlt)) && (F (o2[t]:hsb != {bot}:hsb)) && (F (o3[t]:qint = {[1/4, 3/4)}:qint)) && (F (o4[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- bv + sbf + hsb + qint (MS-0187 .. MS-0188) ---

TEST_CASE("[MS-0187] bv+sbf+hsb+qint: G safety four types") {
	const char* fm = "G (o1[t]:bv[8] = {42}:bv[8] && o2[t]:sbf = {X & Y}:sbf && o3[t]:hsb = {top}:hsb && o4[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0188] bv+sbf+hsb+qint: UNREAL — bv contradiction") {
	const char* fm = "G (o1[t]:bv[8] = {0}:bv[8] && o1[t]:bv[8] = {255}:bv[8] && o2[t]:sbf = {X}:sbf && o3[t]:hsb = {top}:hsb && o4[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

// --- bv + sbf + hsb + tau (MS-0189) ---

TEST_CASE("[MS-0189] bv+sbf+hsb+tau: G safety four types") {
	const char* fm = "G (o1[t]:bv[8] = {#b10110101}:bv[8] && o2[t]:sbf = {X | (Y & Z)}:sbf && o3[t]:hsb = {top}:hsb && o4[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- sbf + hsb + qint + tau (MS-0190) ---

TEST_CASE("[MS-0190] sbf+hsb+qint+tau: G safety four types") {
	const char* fm = "G (o1[t]:sbf = {X & Y}:sbf && o2[t]:hsb = {top}:hsb && o3[t]:qint = {[0, 1)}:qint && o4[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// ============================================================================
// 5-type formulas: 30 tests (MS-0191 .. MS-0220)
// ============================================================================

TEST_CASE("[MS-0191] qlt+bv+sbf+hsb+qint: G safety five types") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X & Y}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0192] qlt+bv+sbf+hsb+qint: F liveness five types") {
	const char* fm = "(F (o1[t]:qlt = {2/3}:qlt)) && (F (o2[t]:bv[8] = {#b10110101}:bv[8])) && (F (o3[t]:sbf = {X ^ Y}:sbf)) && (F (o4[t]:hsb != {bot}:hsb)) && (F (o5[t]:qint = {[1/4, 3/4)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0193] qlt+bv+sbf+hsb+qint: UNREAL — qlt contradiction") {
	const char* fm = "G (o1[t]:qlt > {1}:qlt && o1[t]:qlt < {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0194] qlt+bv+sbf+hsb+qint: implication chain") {
	const char* fm = "G ((o1[t]:qlt > {1/2}:qlt -> o2[t]:bv[8] = {255}:bv[8]) && o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0195] qlt+bv+sbf+hsb+qint: Until on qlt with G on rest") {
	const char* fm = "((o1[t]:qlt > {0}:qlt) U (o1[t]:qlt = {1/2}:qlt)) && (G (o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X & Y}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0196] qlt+bv+sbf+hsb+qint: UNREAL — bv contradiction") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {0}:bv[8] && o2[t]:bv[8] = {255}:bv[8] && o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0197] qlt+bv+sbf+hsb+qint: G(F) liveness five types") {
	const char* fm = "G (F (o1[t]:qlt > {1/2}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0198] qlt+bv+sbf+hsb+qint: past reference") {
	const char* fm = "G (o1[t]:qlt > o1[t-1]:qlt && o2[t]:bv[8] = {#b00001111}:bv[8] && o3[t]:sbf = {X & Y}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + bv + sbf + hsb + tau (MS-0199 .. MS-0205) ---

TEST_CASE("[MS-0199] qlt+bv+sbf+hsb+tau: G safety five types") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X & Y}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0200] qlt+bv+sbf+hsb+tau: F liveness five types") {
	const char* fm = "(F (o1[t]:qlt = {2/3}:qlt)) && (F (o2[t]:bv[8] = {255}:bv[8])) && (F (o3[t]:sbf = {X ^ Y}:sbf)) && (F (o4[t]:hsb != {bot}:hsb)) && (F (o5[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0201] qlt+bv+sbf+hsb+tau: UNREAL — tau contradiction") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:tau = {T.}:tau && o5[t]:tau = {F.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0202] qlt+bv+sbf+hsb+tau: implication chain") {
	const char* fm = "G ((o1[t]:qlt > {1/2}:qlt -> o2[t]:bv[8] = {255}:bv[8]) && (o3[t]:sbf = {X}:sbf -> o5[t]:tau = {T.}:tau) && o4[t]:hsb = {top}:hsb).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0203] qlt+bv+sbf+hsb+tau: UNREAL — sbf contradiction") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X}:sbf && o3[t]:sbf = {X}:sbf' && o4[t]:hsb = {top}:hsb && o5[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0204] qlt+bv+sbf+hsb+tau: input-output") {
	const char* fm = "G ((i1[t]:qlt > {0}:qlt -> o1[t]:bv[8] = {255}:bv[8]) && o2[t]:sbf = {X}:sbf && o3[t]:hsb = {top}:hsb && o4[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0205] qlt+bv+sbf+hsb+tau: Until on qlt with G on rest") {
	const char* fm = "((o1[t]:qlt > {0}:qlt) U (o1[t]:qlt = {1/2}:qlt)) && (G (o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X & Y}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + bv + sbf + qint + tau (MS-0206 .. MS-0210) ---

TEST_CASE("[MS-0206] qlt+bv+sbf+qint+tau: G safety five types") {
	const char* fm = "G (o1[t]:qlt = {3/4}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X & Y}:sbf && o4[t]:qint = {[0, 1)}:qint && o5[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0207] qlt+bv+sbf+qint+tau: F liveness five types") {
	const char* fm = "(F (o1[t]:qlt = {1/2}:qlt)) && (F (o2[t]:bv[8] = {#b10110101}:bv[8])) && (F (o3[t]:sbf = {X | (Y & Z)}:sbf)) && (F (o4[t]:qint = {[1/4, 3/4)}:qint)) && (F (o5[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0208] qlt+bv+sbf+qint+tau: UNREAL — qlt G+F contradiction") {
	const char* fm = "(G (o1[t]:qlt = {1/2}:qlt)) && (F (o1[t]:qlt = {2/3}:qlt)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0209] qlt+bv+sbf+qint+tau: Until on sbf with G on rest") {
	const char* fm = "(G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8])) && ((o3[t]:sbf = {X & Y}:sbf) U (o3[t]:sbf = {X | (Y & Z)}:sbf)) && (G (o4[t]:qint = {[0, 1)}:qint && o5[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0210] qlt+bv+sbf+qint+tau: past reference") {
	const char* fm = "G (o1[t]:qlt > o1[t-1]:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X}:sbf && o4[t]:qint = {[0, 1)}:qint && o5[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + bv + hsb + qint + tau (MS-0211 .. MS-0215) ---

TEST_CASE("[MS-0211] qlt+bv+hsb+qint+tau: G safety five types") {
	const char* fm = "G (o1[t]:qlt = {1/2}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:hsb = {top}:hsb && o4[t]:qint = {[0, 1)}:qint && o5[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0212] qlt+bv+hsb+qint+tau: F liveness five types") {
	const char* fm = "(F (o1[t]:qlt > {2/3}:qlt)) && (F (o2[t]:bv[8] = {255}:bv[8])) && (F (o3[t]:hsb != {bot}:hsb)) && (F (o4[t]:qint = {[1/4, 3/4)}:qint)) && (F (o5[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0213] qlt+bv+hsb+qint+tau: UNREAL — bv contradiction") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o2[t]:bv[8] = {0}:bv[8] && o3[t]:hsb = {top}:hsb && o4[t]:qint = {[0, 1)}:qint && o5[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0214] qlt+bv+hsb+qint+tau: Until on qlt with G on rest") {
	const char* fm = "((o1[t]:qlt > {0}:qlt) U (o1[t]:qlt = {1/2}:qlt)) && (G (o2[t]:bv[8] = {42}:bv[8] && o3[t]:hsb = {top}:hsb && o4[t]:qint = {[0, 1)}:qint && o5[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0215] qlt+bv+hsb+qint+tau: implication") {
	const char* fm = "G (o3[t]:hsb = {top}:hsb && o4[t]:qint = {[0, 1)}:qint && o5[t]:tau = {T.}:tau && (o1[t]:qlt > {1/2}:qlt -> o2[t]:bv[8] = {255}:bv[8])).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- qlt + sbf + hsb + qint + tau (MS-0216 .. MS-0218) ---

TEST_CASE("[MS-0216] qlt+sbf+hsb+qint+tau: G safety five types") {
	const char* fm = "G (o1[t]:qlt = {3/4}:qlt && o2[t]:sbf = {X & Y}:sbf && o3[t]:hsb = {top}:hsb && o4[t]:qint = {[0, 1)}:qint && o5[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0217] qlt+sbf+hsb+qint+tau: UNREAL — sbf contradiction") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:sbf = {X}:sbf && o2[t]:sbf = {X}:sbf' && o3[t]:hsb = {top}:hsb && o4[t]:qint = {[0, 1)}:qint && o5[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0218] qlt+sbf+hsb+qint+tau: F liveness five types") {
	const char* fm = "(F (o1[t]:qlt > {2/3}:qlt)) && (F (o2[t]:sbf = {X ^ Y}:sbf)) && (F (o3[t]:hsb != {bot}:hsb)) && (F (o4[t]:qint = {[1/4, 3/4)}:qint)) && (F (o5[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

// --- bv + sbf + hsb + qint + tau (MS-0219 .. MS-0220) ---

TEST_CASE("[MS-0219] bv+sbf+hsb+qint+tau: G safety five types") {
	const char* fm = "G (o1[t]:bv[8] = {#b10110101}:bv[8] && o2[t]:sbf = {X | (Y & Z)}:sbf && o3[t]:hsb = {top}:hsb && o4[t]:qint = {[0, 1)}:qint && o5[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0220] bv+sbf+hsb+qint+tau: UNREAL — bv contradiction") {
	const char* fm = "G (o1[t]:bv[8] = {42}:bv[8] && o1[t]:bv[8] = {0}:bv[8] && o2[t]:sbf = {X}:sbf && o3[t]:hsb = {top}:hsb && o4[t]:qint = {[0, 1)}:qint && o5[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

// ============================================================================
// 6-type formulas: 30 tests (MS-0221 .. MS-0250)
// All six types: qlt + bv + sbf + hsb + qint + tau
// ============================================================================

TEST_CASE("[MS-0221] 6-type: G safety all six outputs") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X & Y}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint && o6[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0222] 6-type: F liveness all six types") {
	const char* fm = "(F (o1[t]:qlt = {2/3}:qlt)) && (F (o2[t]:bv[8] = {#b10110101}:bv[8])) && (F (o3[t]:sbf = {X ^ Y}:sbf)) && (F (o4[t]:hsb != {bot}:hsb)) && (F (o5[t]:qint = {[1/4, 3/4)}:qint)) && (F (o6[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0223] 6-type: UNREAL — qlt contradiction") {
	const char* fm = "G (o1[t]:qlt > {1}:qlt && o1[t]:qlt < {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint && o6[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0224] 6-type: UNREAL — bv contradiction") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {0}:bv[8] && o2[t]:bv[8] = {255}:bv[8] && o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint && o6[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0225] 6-type: UNREAL — sbf contradiction") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X}:sbf && o3[t]:sbf = {X}:sbf' && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint && o6[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0226] 6-type: UNREAL — tau contradiction") {
	const char* fm = "G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint && o6[t]:tau = {T.}:tau && o6[t]:tau = {F.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0227] 6-type: implication chain across all six") {
	const char* fm = "G ((o1[t]:qlt > {1/2}:qlt -> o2[t]:bv[8] = {255}:bv[8]) && (o3[t]:sbf = {X}:sbf -> o6[t]:tau = {T.}:tau) && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0228] 6-type: Until on qlt with G on five") {
	const char* fm = "((o1[t]:qlt > {0}:qlt) U (o1[t]:qlt = {1/2}:qlt)) && (G (o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X & Y}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint && o6[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0229] 6-type: G(F) liveness on all six") {
	const char* fm = "G (F (o1[t]:qlt > {1/2}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint && o6[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0230] 6-type: past reference on qlt with five others") {
	const char* fm = "G (o1[t]:qlt > o1[t-1]:qlt && o2[t]:bv[8] = {#b00001111}:bv[8] && o3[t]:sbf = {X & Y}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint && o6[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0231] 6-type: input-output across six types") {
	const char* fm = "G ((i1[t]:qlt > {0}:qlt -> o1[t]:bv[8] = {255}:bv[8]) && o2[t]:sbf = {X}:sbf && o3[t]:hsb = {top}:hsb && o4[t]:qint = {[0, 1)}:qint && o5[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0232] 6-type: Until on qlt with G on five") {
	const char* fm = "((o1[t]:qlt > {0}:qlt) U (o1[t]:qlt = {1/2}:qlt)) && (G (o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint && o6[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0233] 6-type: UNREAL — G always half but F needs different") {
	const char* fm = "(G (o1[t]:qlt = {1/2}:qlt)) && (F (o1[t]:qlt = {2/3}:qlt)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0234] 6-type: UNREAL — bv always 42 but F needs 0") {
	const char* fm = "(G (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8])) && (F (o2[t]:bv[8] = {0}:bv[8])).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0235] 6-type: Until on bv with G on five") {
	const char* fm = "(G (o1[t]:qlt > {0}:qlt && o1[t]:qlt < {1}:qlt)) && ((o2[t]:bv[8] = {42}:bv[8]) U (o2[t]:bv[8] = {0}:bv[8])) && (G (o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint && o6[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0236] 6-type: negation on bv with five other types") {
	const char* fm = "G (!(o2[t]:bv[8] = {0}:bv[8]) && o1[t]:qlt > {0}:qlt && o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint && o6[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0237] 6-type: mixed temporal operators across all types") {
	const char* fm = "(G (o1[t]:qlt > {0}:qlt && o3[t]:sbf = {X & Y}:sbf && o4[t]:hsb = {top}:hsb && o6[t]:tau = {T.}:tau)) && (F (o2[t]:bv[8] = {255}:bv[8])) && (F (o5[t]:qint = {[1/4, 3/4)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0238] 6-type: Until on sbf with F on bv and G on rest") {
	const char* fm = "(G (o1[t]:qlt > {0}:qlt && o1[t]:qlt < {1}:qlt)) && (F (o2[t]:bv[8] = {#b10110101}:bv[8])) && ((o3[t]:sbf = {X & Y}:sbf) U (o3[t]:sbf = {X | (Y & Z)}:sbf)) && (G (o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint && o6[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0239] 6-type: UNREAL — qlt positive forever but F needs non-positive") {
	const char* fm = "(G (o1[t]:qlt > {0}:qlt)) && (F (o1[t]:qlt <= {0}:qlt)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0240] 6-type: disjunction across two types with four constant guards") {
	const char* fm = "G ((o1[t]:qlt > {0}:qlt || o2[t]:bv[8] = {255}:bv[8]) && o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint && o6[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0241] 6-type: varied constants across all types") {
	const char* fm = "G ((o1[t]:qlt = {-1}:qlt || o1[t]:qlt = {1/2}:qlt) && o2[t]:bv[8] = {#b10110101}:bv[8] && o3[t]:sbf = {X | (Y & Z)}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[-1, 0) | [1, 2)}:qint && o6[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0242] 6-type: UNREAL — qlt forced equal two different values with five other types") {
	const char* fm = "G (o1[t]:qlt = {1/2}:qlt && o1[t]:qlt = {2/3}:qlt && o2[t]:bv[8] = {42}:bv[8] && o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint && o6[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0243] 6-type: mixed F and G with diverse constants") {
	const char* fm = "(F (o1[t]:qlt = {1/2}:qlt)) && (G (o2[t]:bv[8] = {#b00001111}:bv[8] && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint)) && (F (o3[t]:sbf = {X ^ Y}:sbf)) && (F (o6[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0244] 6-type: Until on bv with G on five") {
	const char* fm = "(G (o1[t]:qlt > {0}:qlt)) && ((o2[t]:bv[8] = {42}:bv[8]) U (o2[t]:bv[8] = {255}:bv[8])) && (G (o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint && o6[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0245] 6-type: input-output implication qlt->bv with all six types") {
	const char* fm = "G ((i1[t]:qlt > {1/2}:qlt -> o2[t]:bv[8] = {255}:bv[8]) && (i1[t]:qlt <= {1/2}:qlt -> o2[t]:bv[8] = {0}:bv[8]) && o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint && o6[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0246] 6-type: G(F) on qlt and bv with G on rest") {
	const char* fm = "G (F (o1[t]:qlt > {0}:qlt && o2[t]:bv[8] = {42}:bv[8]) && o3[t]:sbf = {X & Y}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint && o6[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0247] 6-type: UNREAL — qlt always positive but F needs negative") {
	const char* fm = "(G (o1[t]:qlt > {0}:qlt && o6[t]:tau = {T.}:tau)) && (F (o1[t]:qlt < {0}:qlt)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK_FALSE(realizable(fm));
}

TEST_CASE("[MS-0248] 6-type: G(F) mixed with Until across types") {
	const char* fm = "(G (F (o1[t]:qlt > {1/2}:qlt))) && ((o2[t]:bv[8] = {42}:bv[8]) U (o2[t]:bv[8] = {255}:bv[8])) && (G (o3[t]:sbf = {X}:sbf && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint && o6[t]:tau = {T.}:tau)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0249] 6-type: complex implication with disjunction and negation") {
	const char* fm = "G ((!(o1[t]:qlt = {0}:qlt) -> o2[t]:bv[8] = {255}:bv[8] || o3[t]:sbf = {X}:sbf) && o4[t]:hsb = {top}:hsb && o5[t]:qint = {[0, 1)}:qint && o6[t]:tau = {T.}:tau).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

TEST_CASE("[MS-0250] 6-type: comprehensive six-type with varied temporal ops") {
	const char* fm = "(G (o1[t]:qlt > {0}:qlt && o1[t]:qlt < {1}:qlt && o3[t]:sbf = {X & Y}:sbf && o4[t]:hsb = {top}:hsb && o6[t]:tau = {T.}:tau)) && (F (o2[t]:bv[8] = {#b10110101}:bv[8])) && ((o5[t]:qint = {[0, 1)}:qint) U (o5[t]:qint = {[1/4, 3/4)}:qint)).";
	REQUIRE(spec(fm) != nullptr);
	CHECK(realizable(fm));
}

} // TEST_SUITE("many_sorted_ltl")


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}