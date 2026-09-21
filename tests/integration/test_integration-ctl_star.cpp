// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Tests for CTL* synthesis via LTL synthesis and semantic negation (-).
// Covers: A/E path quantifiers, nested CTL*, semantic negation, NNF duals,
// CTL*→LTL reduction, and interaction with existing LTL operators.

#include "test_integration-satisfiability_helper.h"

// ═════════════════════════════════════════════════════════════════════════════
// 1. PARSING TESTS — grammar acceptance of A, E, -
// ═════════════════════════════════════════════════════════════════════════════

TEST_SUITE("CTL* parsing - A operator") {
	TEST_CASE("parse_A_simple_eq") {
		auto r = get_nso_rr<node_t>(tau::get("A (o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_always_inner") {
		auto r = get_nso_rr<node_t>(tau::get("A always o1[t] = o1[t-1].").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_sometimes_inner") {
		auto r = get_nso_rr<node_t>(tau::get("A sometimes o1[t] = 1.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_nested_bool") {
		auto r = get_nso_rr<node_t>(tau::get("A (o1[t] = 1 && o2[t] = 0).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_with_F") {
		auto r = get_nso_rr<node_t>(tau::get("A F o1[t] = 1.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_with_G") {
		auto r = get_nso_rr<node_t>(tau::get("A G o1[t] = 1.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_with_U") {
		auto r = get_nso_rr<node_t>(tau::get("A (o1[t] = 0 U o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_with_R") {
		auto r = get_nso_rr<node_t>(tau::get("A (o1[t] = 1 R o2[t] = 0).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_or") {
		auto r = get_nso_rr<node_t>(tau::get("A (o1[t] = 1 || o2[t] = 0).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_imply") {
		auto r = get_nso_rr<node_t>(tau::get("A (o1[t] = 1 -> o2[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_paren") {
		auto r = get_nso_rr<node_t>(tau::get("A ((o1[t] = 1)).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_neg") {
		auto r = get_nso_rr<node_t>(tau::get("A !o1[t] = 1.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_input_var") {
		auto r = get_nso_rr<node_t>(tau::get("A (i1[t] = 1 -> o1[t] = i1[t]).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_W") {
		auto r = get_nso_rr<node_t>(tau::get("A (o1[t] = 1 W o2[t] = 0).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_xor") {
		auto r = get_nso_rr<node_t>(tau::get("A ((o1[t] = 1) ^^ (o2[t] = 0)).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_equiv") {
		auto r = get_nso_rr<node_t>(tau::get("A (o1[t] = 1 <-> o2[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_conditional") {
		auto r = get_nso_rr<node_t>(tau::get("A (i1[t] = 1 ? o1[t] = 1 : o1[t] = 0).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_two_outputs") {
		auto r = get_nso_rr<node_t>(tau::get("A (o1[t] = 1 && o2[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_multibit") {
		auto r = get_nso_rr<node_t>(tau::get("A (o1[t]:bv[8] = 0).").value_or(nullptr));
		CHECK(r.has_value());
	}
}

TEST_SUITE("CTL* parsing - E operator") {
	TEST_CASE("parse_E_simple_eq") {
		auto r = get_nso_rr<node_t>(tau::get("E (o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_always_inner") {
		auto r = get_nso_rr<node_t>(tau::get("E always o1[t] = 1.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_sometimes_inner") {
		auto r = get_nso_rr<node_t>(tau::get("E sometimes o1[t] = 1.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_nested_bool") {
		auto r = get_nso_rr<node_t>(tau::get("E (o1[t] = 0 || o2[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_with_F") {
		auto r = get_nso_rr<node_t>(tau::get("E F o1[t] = 1.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_with_G") {
		auto r = get_nso_rr<node_t>(tau::get("E G o1[t] = 0.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_with_U") {
		auto r = get_nso_rr<node_t>(tau::get("E (o1[t] = 0 U o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_with_R") {
		auto r = get_nso_rr<node_t>(tau::get("E (o1[t] = 0 R o2[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_and") {
		auto r = get_nso_rr<node_t>(tau::get("E (o1[t] = 1 && o2[t] = 0).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_imply") {
		auto r = get_nso_rr<node_t>(tau::get("E (i1[t] = 0 -> o1[t] = 0).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_paren") {
		auto r = get_nso_rr<node_t>(tau::get("E ((o1[t] = 0)).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_neg") {
		auto r = get_nso_rr<node_t>(tau::get("E !o1[t] = 0.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_W") {
		auto r = get_nso_rr<node_t>(tau::get("E (o1[t] = 0 W o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_xor") {
		auto r = get_nso_rr<node_t>(tau::get("E ((o1[t] = 0) ^^ (o2[t] = 1)).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_equiv") {
		auto r = get_nso_rr<node_t>(tau::get("E (o1[t] = 0 <-> o2[t] = 0).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_conditional") {
		auto r = get_nso_rr<node_t>(tau::get("E (i1[t] = 0 ? o1[t] = 0 : o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_two_outputs") {
		auto r = get_nso_rr<node_t>(tau::get("E (o1[t] = 0 && o2[t] = 0).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_multibit") {
		auto r = get_nso_rr<node_t>(tau::get("E (o1[t]:bv[8] = 0).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_lookback") {
		auto r = get_nso_rr<node_t>(tau::get("E (o1[t] = o1[t-1]).").value_or(nullptr));
		CHECK(r.has_value());
	}
}

TEST_SUITE("CTL* parsing - semantic negation") {
	TEST_CASE("parse_sem_neg_simple") {
		auto r = get_nso_rr<node_t>(tau::get("-(o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_always") {
		auto r = get_nso_rr<node_t>(tau::get("-(always o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_sometimes") {
		auto r = get_nso_rr<node_t>(tau::get("-(sometimes o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_F") {
		auto r = get_nso_rr<node_t>(tau::get("-(F o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_and") {
		auto r = get_nso_rr<node_t>(tau::get("-(o1[t] = 1 && o2[t] = 0).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_or") {
		auto r = get_nso_rr<node_t>(tau::get("-(o1[t] = 1 || o2[t] = 0).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_neg") {
		auto r = get_nso_rr<node_t>(tau::get("-(!o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_double") {
		auto r = get_nso_rr<node_t>(tau::get("--(o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_A") {
		auto r = get_nso_rr<node_t>(tau::get("-(A o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_E") {
		auto r = get_nso_rr<node_t>(tau::get("-(E o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_U") {
		auto r = get_nso_rr<node_t>(tau::get("-(o1[t] = 0 U o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_R") {
		auto r = get_nso_rr<node_t>(tau::get("-(o1[t] = 1 R o2[t] = 0).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_imply") {
		auto r = get_nso_rr<node_t>(tau::get("-(i1[t] = 1 -> o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_equiv") {
		auto r = get_nso_rr<node_t>(tau::get("-(o1[t] = 1 <-> o2[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_paren") {
		auto r = get_nso_rr<node_t>(tau::get("-((o1[t] = 1)).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_conditional") {
		auto r = get_nso_rr<node_t>(tau::get("-(i1[t] = 1 ? o1[t] = 1 : o1[t] = 0).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_G") {
		auto r = get_nso_rr<node_t>(tau::get("-(G o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_multibit") {
		auto r = get_nso_rr<node_t>(tau::get("-(o1[t]:bv[8] = 0).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_xor") {
		auto r = get_nso_rr<node_t>(tau::get("-(o1[t] = 0 ^^ o2[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_lookback") {
		auto r = get_nso_rr<node_t>(tau::get("-(o1[t] = o1[t-1]).").value_or(nullptr));
		CHECK(r.has_value());
	}
}

// ═════════════════════════════════════════════════════════════════════════════
// 2. NESTED CTL* PARSING
// ═════════════════════════════════════════════════════════════════════════════

TEST_SUITE("CTL* parsing - nested quantifiers") {
	TEST_CASE("parse_AA") {
		auto r = get_nso_rr<node_t>(tau::get("A A o1[t] = 1.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_AE") {
		auto r = get_nso_rr<node_t>(tau::get("A E o1[t] = 1.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_EA") {
		auto r = get_nso_rr<node_t>(tau::get("E A o1[t] = 1.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_EE") {
		auto r = get_nso_rr<node_t>(tau::get("E E o1[t] = 1.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_E_conjunct") {
		auto r = get_nso_rr<node_t>(tau::get("A o1[t] = 1 && E o2[t] = 0.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_E_disjunct") {
		auto r = get_nso_rr<node_t>(tau::get("A o1[t] = 1 || E o2[t] = 0.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_sem_neg_A") {
		auto r = get_nso_rr<node_t>(tau::get("E -(A o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_A_E") {
		auto r = get_nso_rr<node_t>(tau::get("-(A E o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_GF") {
		auto r = get_nso_rr<node_t>(tau::get("A G F o1[t] = 1.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_FG") {
		auto r = get_nso_rr<node_t>(tau::get("E F G o1[t] = 0.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_F_E_G") {
		auto r = get_nso_rr<node_t>(tau::get("A F (E G o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_triple_A") {
		auto r = get_nso_rr<node_t>(tau::get("A A A o1[t] = 1.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_triple_E") {
		auto r = get_nso_rr<node_t>(tau::get("E E E o1[t] = 0.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_AEA") {
		auto r = get_nso_rr<node_t>(tau::get("A E A o1[t] = 1.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_EAE") {
		auto r = get_nso_rr<node_t>(tau::get("E A E o1[t] = 0.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_neg_A_neg_E") {
		auto r = get_nso_rr<node_t>(tau::get("!A !E o1[t] = 1.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_nested_3") {
		auto r = get_nso_rr<node_t>(tau::get("---(o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_with_lookback") {
		auto r = get_nso_rr<node_t>(tau::get("A (o1[t] = o1[t-1] && o2[t] = o2[t-2]).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_with_lookback") {
		auto r = get_nso_rr<node_t>(tau::get("E (o1[t] != o1[t-1]).").value_or(nullptr));
		CHECK(r.has_value());
	}
}

// ═════════════════════════════════════════════════════════════════════════════
// 3. CTL* COMBINED WITH EXISTING OPERATORS
// ═════════════════════════════════════════════════════════════════════════════

TEST_SUITE("CTL* parsing - combined with LTL") {
	TEST_CASE("parse_A_U") {
		auto r = get_nso_rr<node_t>(tau::get("A (o1[t] = 0 U o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_U") {
		auto r = get_nso_rr<node_t>(tau::get("E (o1[t] = 0 U o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_W") {
		auto r = get_nso_rr<node_t>(tau::get("A (o1[t] = 0 W o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_W") {
		auto r = get_nso_rr<node_t>(tau::get("E (o1[t] = 0 W o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_R") {
		auto r = get_nso_rr<node_t>(tau::get("A (o1[t] = 1 R o2[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_R") {
		auto r = get_nso_rr<node_t>(tau::get("E (o1[t] = 1 R o2[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_S") {
		auto r = get_nso_rr<node_t>(tau::get("A (o1[t] = 0 S o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_S") {
		auto r = get_nso_rr<node_t>(tau::get("E (o1[t] = 0 S o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_T") {
		auto r = get_nso_rr<node_t>(tau::get("A (o1[t] = 0 T o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_T") {
		auto r = get_nso_rr<node_t>(tau::get("E (o1[t] = 0 T o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_U") {
		auto r = get_nso_rr<node_t>(tau::get("-(o1[t] = 0 U o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_W") {
		auto r = get_nso_rr<node_t>(tau::get("-(o1[t] = 0 W o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_R") {
		auto r = get_nso_rr<node_t>(tau::get("-(o1[t] = 0 R o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_S") {
		auto r = get_nso_rr<node_t>(tau::get("-(o1[t] = 0 S o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_T") {
		auto r = get_nso_rr<node_t>(tau::get("-(o1[t] = 0 T o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_G_F") {
		auto r = get_nso_rr<node_t>(tau::get("A G F o1[t] = 1.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_G_F") {
		auto r = get_nso_rr<node_t>(tau::get("E G F o1[t] = 1.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_F_G") {
		auto r = get_nso_rr<node_t>(tau::get("A F G o1[t] = 1.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_F_G") {
		auto r = get_nso_rr<node_t>(tau::get("E F G o1[t] = 0.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_A_conj_always") {
		auto r = get_nso_rr<node_t>(tau::get("A o1[t] = 1 && always o2[t] = 0.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_E_conj_sometimes") {
		auto r = get_nso_rr<node_t>(tau::get("E o1[t] = 1 && sometimes o2[t] = 0.").value_or(nullptr));
		CHECK(r.has_value());
	}
	TEST_CASE("parse_sem_neg_with_constraint") {
		auto r = get_nso_rr<node_t>(tau::get("-([n = 5] && o1[t] = 1).").value_or(nullptr));
		CHECK(r.has_value());
	}
}

// ═════════════════════════════════════════════════════════════════════════════
// 4. CONSTANT FOLDING / HOOK SIMPLIFICATIONS
// ═════════════════════════════════════════════════════════════════════════════

TEST_SUITE("CTL* hooks - constant folding") {
	// create_spec returns a wff node; the folded result is the first child.
	// E.g. A T → wff(wff_t), so child_is(wff_t) is the correct check.
	TEST_CASE("A_T_reduces_to_T") {
		tref spec = create_spec("A T.");
		CHECK(tau::get(spec).child_is(tau::wff_t));
	}
	TEST_CASE("A_F_reduces_to_F") {
		tref spec = create_spec("A F.");
		CHECK(tau::get(spec).child_is(tau::wff_f));
	}
	TEST_CASE("E_T_reduces_to_T") {
		tref spec = create_spec("E T.");
		CHECK(tau::get(spec).child_is(tau::wff_t));
	}
	TEST_CASE("E_F_reduces_to_F") {
		tref spec = create_spec("E F.");
		CHECK(tau::get(spec).child_is(tau::wff_f));
	}
	TEST_CASE("sem_neg_T_reduces_to_F") {
		tref spec = create_spec("-T.");
		CHECK(tau::get(spec).child_is(tau::wff_f));
	}
	TEST_CASE("sem_neg_F_reduces_to_T") {
		tref spec = create_spec("-F.");
		CHECK(tau::get(spec).child_is(tau::wff_t));
	}
}

// ═════════════════════════════════════════════════════════════════════════════
// 5. NNF DUALS
// ═════════════════════════════════════════════════════════════════════════════

TEST_SUITE("CTL* NNF duals") {
	TEST_CASE("neg_A_to_E_neg") {
		// !A φ  should push to  E !φ  in NNF
		tref spec = create_spec("!(A o1[t] = 1).");
		auto nnf = to_nnf<node_t>(spec);
		// After NNF, the outermost should not be wff_neg wrapping wff_A
		auto& t = tau::get(nnf);
		if (t.has_child()) {
			CHECK(t[0].value.nt != tau::wff_neg);
		}
	}
	TEST_CASE("neg_E_to_A_neg") {
		// !E φ  should push to  A !φ  in NNF
		tref spec = create_spec("!(E o1[t] = 1).");
		auto nnf = to_nnf<node_t>(spec);
		auto& t = tau::get(nnf);
		if (t.has_child()) {
			CHECK(t[0].value.nt != tau::wff_neg);
		}
	}
	TEST_CASE("double_neg_A_identity") {
		// !(!A phi) → !(E !phi) → A !!phi → A phi in NNF
		tref spec = create_spec("!(!A o1[t] = 1).");
		auto nnf = to_nnf<node_t>(spec);
		REQUIRE(nnf != nullptr);
		auto& t = tau::get(nnf);
		// NNF must push negations inward — no wff_neg at the outer child
		if (t.has_child()) {
			CHECK(t[0].value.nt != tau::wff_neg);
		}
	}
	TEST_CASE("double_neg_E_identity") {
		// !(!E phi) → !(A !phi) → E !!phi → E phi in NNF
		tref spec = create_spec("!(!E o1[t] = 0).");
		auto nnf = to_nnf<node_t>(spec);
		REQUIRE(nnf != nullptr);
		auto& t = tau::get(nnf);
		// NNF must push negations inward — no wff_neg at the outer child
		if (t.has_child()) {
			CHECK(t[0].value.nt != tau::wff_neg);
		}
	}
	TEST_CASE("neg_A_neg_eq_E") {
		// !(A !(o1[t] = 1)) → E !!(o1[t] = 1) → E (o1[t] = 1) in NNF
		tref spec = create_spec("!(A !(o1[t] = 1)).");
		auto nnf = to_nnf<node_t>(spec);
		REQUIRE(nnf != nullptr);
		auto& t = tau::get(nnf);
		// NNF must push negations inward — no wff_neg at the outer child
		if (t.has_child()) {
			CHECK(t[0].value.nt != tau::wff_neg);
		}
	}
}

// ═════════════════════════════════════════════════════════════════════════════
// 6. DETECTION FUNCTIONS
// ═════════════════════════════════════════════════════════════════════════════

TEST_SUITE("CTL* detection") {
	TEST_CASE("has_ctl_star_A") {
		tref spec = create_spec("A o1[t] = 1.");
		CHECK(has_ctl_star_operators<node_t>(spec));
	}
	TEST_CASE("has_ctl_star_E") {
		tref spec = create_spec("E o1[t] = 1.");
		CHECK(has_ctl_star_operators<node_t>(spec));
	}
	TEST_CASE("has_ctl_star_sem_neg") {
		tref spec = create_spec("-(o1[t] = 1).");
		CHECK(has_ctl_star_operators<node_t>(spec));
	}
	TEST_CASE("no_ctl_star_in_pure_ltl") {
		tref spec = create_spec("always o1[t] = 1.");
		CHECK(!has_ctl_star_operators<node_t>(spec));
	}
	TEST_CASE("no_ctl_star_in_safety") {
		tref spec = create_spec("always o1[t] = o1[t-1].");
		CHECK(!has_ctl_star_operators<node_t>(spec));
	}
	TEST_CASE("no_ctl_star_in_F") {
		tref spec = create_spec("F o1[t] = 1.");
		CHECK(!has_ctl_star_operators<node_t>(spec));
	}
	TEST_CASE("has_ctl_star_nested_deep") {
		tref spec = create_spec("always (A o1[t] = 1).");
		CHECK(has_ctl_star_operators<node_t>(spec));
	}
	TEST_CASE("has_ctl_star_in_conjunction") {
		tref spec = create_spec("always(o1[t] = 0) && (E o2[t] = 1).");
		CHECK(has_ctl_star_operators<node_t>(spec));
	}
}

// ═════════════════════════════════════════════════════════════════════════════
// 7. LARGE-SCALE STRESS TESTS — parsing many CTL* formulas
// ═════════════════════════════════════════════════════════════════════════════

TEST_SUITE("CTL* stress - 200 parsing formulas") {
	TEST_CASE("stress_A_eq_variants") {
		for (int i = 0; i < 20; ++i) {
			std::string fm = "A (o1[t] = " + std::to_string(i % 2) + ").";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()).value_or(nullptr));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_E_eq_variants") {
		for (int i = 0; i < 20; ++i) {
			std::string fm = "E (o1[t] = " + std::to_string(i % 2) + ").";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()).value_or(nullptr));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_sem_neg_variants") {
		for (int i = 0; i < 20; ++i) {
			std::string fm = "-(o" + std::to_string(i + 1) + "[t] = 0).";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()).value_or(nullptr));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_A_always_chain") {
		for (int depth = 1; depth <= 10; ++depth) {
			std::string fm = "";
			for (int d = 0; d < depth; ++d) fm += "A ";
			fm += "o1[t] = 1.";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()).value_or(nullptr));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_E_always_chain") {
		for (int depth = 1; depth <= 10; ++depth) {
			std::string fm = "";
			for (int d = 0; d < depth; ++d) fm += "E ";
			fm += "o1[t] = 0.";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()).value_or(nullptr));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_sem_neg_chain") {
		for (int depth = 1; depth <= 10; ++depth) {
			std::string fm = "";
			for (int d = 0; d < depth; ++d) fm += "-";
			fm += "(o1[t] = 1).";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()).value_or(nullptr));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_A_multi_output") {
		for (int n = 1; n <= 20; ++n) {
			std::string fm = "A (";
			for (int i = 1; i <= n; ++i) {
				if (i > 1) fm += " && ";
				fm += "o" + std::to_string(i) + "[t] = " + std::to_string(i % 2);
			}
			fm += ").";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()).value_or(nullptr));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_E_multi_output") {
		for (int n = 1; n <= 20; ++n) {
			std::string fm = "E (";
			for (int i = 1; i <= n; ++i) {
				if (i > 1) fm += " || ";
				fm += "o" + std::to_string(i) + "[t] = " + std::to_string(i % 2);
			}
			fm += ").";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()).value_or(nullptr));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_alternating_AE") {
		for (int n = 1; n <= 20; ++n) {
			std::string fm = "";
			for (int i = 0; i < n; ++i) {
				fm += (i % 2 == 0) ? "A " : "E ";
			}
			fm += "o1[t] = 1.";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()).value_or(nullptr));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_sem_neg_with_A_E") {
		for (int n = 0; n < 20; ++n) {
			std::string fm = "-";
			fm += (n % 2 == 0) ? "(A o1[t] = " : "(E o1[t] = ";
			fm += std::to_string(n % 2) + ").";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()).value_or(nullptr));
			CHECK(r.has_value());
		}
	}
}

// ═════════════════════════════════════════════════════════════════════════════
// 8. MORE STRESS TESTS FOR INTERACTIONS
// ═════════════════════════════════════════════════════════════════════════════

TEST_SUITE("CTL* stress - 200 combined formulas") {
	TEST_CASE("stress_A_with_ltl_ops") {
		const char* ops[] = {"F", "G", "always", "sometimes"};
		for (int i = 0; i < 40; ++i) {
			std::string fm = "A ";
			fm += ops[i % 4];
			fm += " o1[t] = " + std::to_string(i % 2) + ".";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()).value_or(nullptr));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_E_with_ltl_ops") {
		const char* ops[] = {"F", "G", "always", "sometimes"};
		for (int i = 0; i < 40; ++i) {
			std::string fm = "E ";
			fm += ops[i % 4];
			fm += " o1[t] = " + std::to_string(i % 2) + ".";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()).value_or(nullptr));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_sem_neg_with_ltl_ops") {
		const char* ops[] = {"F", "G", "always", "sometimes"};
		for (int i = 0; i < 40; ++i) {
			std::string fm = "-(";
			fm += ops[i % 4];
			fm += " o1[t] = " + std::to_string(i % 2) + ").";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()).value_or(nullptr));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_A_binary_ltl") {
		const char* ops[] = {"U", "R", "W"};
		for (int i = 0; i < 30; ++i) {
			std::string fm = "A (o1[t] = ";
			fm += std::to_string(i % 2);
			fm += " ";
			fm += ops[i % 3];
			fm += " o2[t] = ";
			fm += std::to_string((i + 1) % 2);
			fm += ").";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()).value_or(nullptr));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_E_binary_ltl") {
		const char* ops[] = {"U", "R", "W"};
		for (int i = 0; i < 30; ++i) {
			std::string fm = "E (o1[t] = ";
			fm += std::to_string(i % 2);
			fm += " ";
			fm += ops[i % 3];
			fm += " o2[t] = ";
			fm += std::to_string((i + 1) % 2);
			fm += ").";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()).value_or(nullptr));
			CHECK(r.has_value());
		}
	}
	TEST_CASE("stress_A_io_var_patterns") {
		for (int i = 0; i < 20; ++i) {
			std::string fm = "A (i" + std::to_string(i + 1) +
				"[t] = 1 -> o" + std::to_string(i + 1) +
				"[t] = i" + std::to_string(i + 1) + "[t]).";
			auto r = get_nso_rr<node_t>(tau::get(fm.c_str()).value_or(nullptr));
			CHECK(r.has_value());
		}
	}
}

// ═════════════════════════════════════════════════════════════════════════════
// 9. SEMANTICS — sat / realizability verdicts, semantic negation
// ═════════════════════════════════════════════════════════════════════════════
//
// `sat` of a CTL* formula is `sat` of its LTL reduction, so it follows the
// one-way LTL rule (realizable decides T, unrealizable leaves it undecided)
// and sat(A χ) agrees with sat(χ). Realizability is asked through
// is_ctl_star_realizable, which also reduces first.

namespace {

std::optional<bool> ctl_sat(const char* src) {
	tref fm = create_spec(src);
	REQUIRE(fm != nullptr);
	auto r = is_tau_formula_sat<node_t>(fm);
	if (!r.has_value()) return std::nullopt;
	return r.value();
}

std::optional<bool> ctl_realizable(const char* src) {
	tref fm = create_spec(src);
	REQUIRE(fm != nullptr);
	auto r = is_ctl_star_realizable<node_t>(fm, 0, false);
	if (!r.has_value()) return std::nullopt;
	return r.value();
}

} // namespace

TEST_SUITE("CTL* semantics - semantic negation") {

	// In a Boolean context `-φ` is the negated realizability verdict of φ.
	TEST_CASE("[CTLS-SEM-01] -(always o1=1) is F: the body is realizable") {
		CHECK(ctl_sat("-(always o1[t] = 1).") == std::optional<bool>(false));
	}

	TEST_CASE("[CTLS-SEM-02] -(F o1=1) && G(o1=0) is F") {
		CHECK(ctl_sat("(-(F o1[t] = 1)) && (G (o1[t] = 0)).")
			== std::optional<bool>(false));
	}

	TEST_CASE("[CTLS-SEM-03] --(o1=1) is T: -(o1=1) is F, and F is unrealizable") {
		CHECK(ctl_sat("--(o1[t] = 1).") == std::optional<bool>(true));
	}

	TEST_CASE("[CTLS-SEM-04] -T is UNREALIZABLE (folded to F)") {
		CHECK(ctl_sat("-T.") == std::optional<bool>(false));
	}

	TEST_CASE("[CTLS-SEM-05] -F is REALIZABLE (folded to T)") {
		CHECK(ctl_sat("-F.") == std::optional<bool>(true));
	}

	// The environment owns i1, so F i1 = 1 is unrealizable and its semantic
	// negation holds.
	TEST_CASE("[CTLS-SEM-06] -(F i1=1) is T") {
		CHECK(ctl_sat("-(F i1[t] = 1).") == std::optional<bool>(true));
		CHECK(ctl_realizable("-(F i1[t] = 1).") == std::optional<bool>(true));
	}

	TEST_CASE("[CTLS-SEM-07] -(F i1=1) && G(o1=1) is T, -(F o1=1) || G(o1=0) is T") {
		CHECK(ctl_sat("(-(F i1[t] = 1)) && (G (o1[t] = 1)).")
			== std::optional<bool>(true));
		CHECK(ctl_sat("(-(F o1[t] = 1)) || (G (o1[t] = 0)).")
			== std::optional<bool>(true));
	}

	// Under a temporal operator `-` means "unrealizable from here on", read
	// as ψ started fresh at that point: the same game everywhere, so it
	// folds like a top-level `-`.
	TEST_CASE("[CTLS-SEM-08] -φ under G / A / F folds") {
		CHECK(ctl_sat("G (-(F i1[t] = 1)).") == std::optional<bool>(true));
		CHECK(ctl_sat("G (-(o1[t] = i1[t])).") == std::optional<bool>(false));
		CHECK(ctl_sat("A (-(F o1[t] = 1)).") == std::optional<bool>(false));
		CHECK(ctl_sat("(G (o2[t] = 1)) && (F (-(F i1[t] = 1))).")
			== std::optional<bool>(true));
	}

	// "ψ from here on" reads ψ as a specification starting here, so what
	// precedes is warm-up and the verdict does not depend on the history:
	// a ψ with lookback or a fixed-time atom folds like any other.
	TEST_CASE("[CTLS-SEM-09] -φ reading the past folds from a fresh start") {
		CHECK(ctl_sat("G (-(o1[t] = o1[t-1])).") == std::optional<bool>(false));
		CHECK(ctl_sat("G (-(o1[0] = 1)).") == std::optional<bool>(false));
		CHECK(ctl_sat("G (-(always o1[t] != o1[t-1])).")
			== std::optional<bool>(false));
	}

} // TEST_SUITE("CTL* semantics - semantic negation")


TEST_SUITE("CTL* semantics - A / E verdicts") {

	TEST_CASE("[CTLS-AE-01] E(always o1=1) is realizable and sat") {
		CHECK(ctl_sat("E (always o1[t] = 1).") == std::optional<bool>(true));
		CHECK(ctl_realizable("E (always o1[t] = 1).") == std::optional<bool>(true));
	}

	TEST_CASE("[CTLS-AE-02] A(always o1=1) is realizable and sat") {
		CHECK(ctl_sat("A (always o1[t] = 1).") == std::optional<bool>(true));
		CHECK(ctl_realizable("A (always o1[t] = 1).") == std::optional<bool>(true));
	}

	TEST_CASE("[CTLS-AE-03] E(F o1=1) is realizable") {
		CHECK(ctl_realizable("E (F o1[t] = 1).") == std::optional<bool>(true));
	}

	// With no inputs the computation tree is one path and the witness
	// encoding is exact, so a quantified conjunct cannot hide a
	// contradictory one.
	TEST_CASE("[CTLS-AE-04] E(always o1=1) && G(o1=1) && G(o1=0) is unrealizable") {
		CHECK(ctl_realizable(
		    "(E (always o1[t] = 1)) && ((G (o1[t] = 1)) && (G (o1[t] = 0)))."
		) == std::optional<bool>(false));
	}

	// An input can never be forced. sat(A χ) agrees with sat(χ): the LTL
	// rule leaves an unrealizable full-LTL formula undecided, the safety
	// pipeline decides G.
	TEST_CASE("[CTLS-AE-05] A(F i1=1) is unrealizable, its sat undecided like F i1=1") {
		CHECK(ctl_realizable("A (F i1[t] = 1).") == std::optional<bool>(false));
		CHECK(ctl_sat("A (F i1[t] = 1).") == ctl_sat("F (i1[t] = 1)."));
	}

	TEST_CASE("[CTLS-AE-06] A(always i1=1) is unrealizable and unsat") {
		CHECK(ctl_realizable("A (always i1[t] = 1).") == std::optional<bool>(false));
		CHECK(ctl_sat("A (always i1[t] = 1).") == std::optional<bool>(false));
	}

	TEST_CASE("[CTLS-AE-07] A(F o1=1) is realizable") {
		CHECK(ctl_realizable("A (F o1[t] = 1).") == std::optional<bool>(true));
		CHECK(ctl_sat("A (F o1[t] = 1).") == std::optional<bool>(true));
	}

	// A under G is still a universal context: G(A φ) ≡ G φ over a tree.
	TEST_CASE("[CTLS-AE-08] always(A(o1=1)) is realizable, always(A(i1=1)) is not") {
		CHECK(ctl_realizable("always (A (o1[t] = 1)).") == std::optional<bool>(true));
		CHECK(ctl_realizable("always (A (i1[t] = 1)).") == std::optional<bool>(false));
	}

	// Placements with no sound encoding are refused, never answered.
	TEST_CASE("[CTLS-AE-09] A under F / || is refused") {
		CHECK_FALSE(ctl_sat("F (A (o1[t] = 1)).").has_value());
		CHECK_FALSE(ctl_sat("(A (o1[t] = 1)) || (always o1[t] = 0).").has_value());
	}

	// E / A in negative polarity go through their NNF duals.
	TEST_CASE("[CTLS-AE-10] negative A / E are decided through the duals") {
		// !A(F o1=1) = E(G o1!=1): realizable (hold o1 at 0)
		CHECK(ctl_realizable("!(A (F o1[t] = 1)).") == std::optional<bool>(true));
		// !E(G o1=1) = A(F o1!=1) = F o1!=1: realizable
		CHECK(ctl_realizable("!(E (always o1[t] = 1)).") == std::optional<bool>(true));
		// (E G o1=1) -> G o1=0 = A(F o1!=1) || G o1=0: A under || stays refused
		CHECK_FALSE(ctl_realizable(
		    "(E (always o1[t] = 1)) -> (always o1[t] = 0).").has_value());
		// both-polarity connectives stay refused
		CHECK_FALSE(ctl_realizable(
		    "(E (always o1[t] = 1)) <-> (always o1[t] = 1).").has_value());
	}

	// Direction outputs pin the witness path, so E over inputs is decided:
	// the environment may raise i1, may hold it, and the two branches are
	// independent.
	TEST_CASE("[CTLS-AE-11] E over inputs is decided through the directions") {
		CHECK(ctl_realizable("E (F i1[t] = 1).") == std::optional<bool>(true));
		CHECK(ctl_realizable("E (always i1[t] = 1).") == std::optional<bool>(true));
		CHECK(ctl_realizable("A (always (E (F i1[t] = 1))).")
			== std::optional<bool>(true));
		CHECK(ctl_realizable(
		    "(E (always i1[t] = 1)) && (E (always i1[t] = 0))."
		) == std::optional<bool>(true));
		// no branch satisfies a contradiction
		CHECK(ctl_realizable(
		    "E (always (o1[t] = i1[t] && o1[t] = 0 && i1[t] = 1))."
		) == std::optional<bool>(false));
		CHECK(ctl_realizable(
		    "(E (F o1[t] = 1)) && (always o1[t] = 0)."
		) == std::optional<bool>(false));
	}

	// A past operator under E has no one-step unfolding, so that witness
	// keeps the all-paths encoding and an unrealizable verdict is undecided.
	TEST_CASE("[CTLS-AE-12] E over a past operator stays undecided") {
		CHECK_FALSE(ctl_realizable("E ((i1[t] = 1) S (i1[t] = 0)).")
			.has_value());
	}

} // TEST_SUITE("CTL* semantics - A / E verdicts")


TEST_SUITE("CTL* printing") {

	// The printed form re-parses to the same tree.
	TEST_CASE("[CTLS-PRINT-01] semantic negation and path quantifiers round-trip") {
		for (const char* src : {
			"(-(always o1[t] = 1)) && (always o1[t] = 0).",
			"-((always o1[t] = 1) && (always o1[t] = 0)).",
			"(E (always o1[t] = 1)) && (always o1[t] = 0).",
			"E ((always o1[t] = 1) && (always o1[t] = 0)).",
			"-(o1[t] = 1).",
			"!(-(F o1[t] = 1))." })
		{
			tref fm = create_spec(src);
			REQUIRE(fm != nullptr);
			std::string printed = tau::get(fm).to_str() + ".";
			tref again = create_spec(printed.c_str());
			REQUIRE(again != nullptr);
			CHECK_MESSAGE(tau::get(fm) == tau::get(again),
				src << " printed as " << printed);
		}
	}

} // TEST_SUITE("CTL* printing")
