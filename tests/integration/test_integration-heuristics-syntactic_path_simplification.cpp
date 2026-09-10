// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "heuristics/syntactic_path_simplification.h"

// ── The eager sweeps as they were before stage 1, kept for the differential test ──

namespace legacy {

using namespace idni::tau_lang;

/**
 * @internal
 * @brief Per-conjunction-path contradiction detection for wff: within each
 * top-level `wff_and`, collects each conjunct as a `true`/`false`
 * assignment (treating a bare atom as `true`, a negated atom as `false`),
 * substitutes them into the rest of the conjunction, and collapses to `F`
 * if any conjunct contradicts another. Does not descend into `wff_or` or
 * push negations.
 * @tparam node Tree node type.
 * @param root Formula to scan.
 * @return @p root with contradictory conjunctions collapsed to `F`.
 *
 * @par Example
 * @code{.cpp}
 * // "x = 0 && !(x = 0)" collapses to F (see
 * // tests/integration/test_integration-heuristics-syntactic_path_simplification.cpp:62-66).
 * tref fm = get_nso_rr("x = 0 && !(x = 0).").value().main->get();
 * tref res = syntactic_path_simplification_simplify_wff<node_t>(fm);
 * CHECK( tau::get(res).equals_F() );
 * @endcode
 * @endinternal
 */
template <NodeType node>
tref syntactic_path_simplification_simplify_wff(tref root) {
	using tau = tree<node>;
	tref skip = nullptr;
	// A conjunction whose conjuncts yield no assignment cannot yield one
	// at any nested wff_and of its own flattening either: their conjuncts
	// are a subset. Mark those nested nodes done once, or the conjuncts
	// of a k-chain get re-collected at each of its k spine nodes.
	//
	// Invariant this relies on: mark_spine visits exactly the wff nodes
	// that get_cnf_wff_clauses (get_leaves along the wff_and spine)
	// descends through, so a marked node's conjuncts are a subset of
	// the marking node's. Both walk the same spine: a wff whose child
	// is wff_and, then that wff_and's children. If one of them changes,
	// the other must follow; the debug check below catches a drift.
	std::unordered_set<tref> done;
	auto mark_spine = [&done](auto&& self, tref m) -> void {
		if (!tau::get(m).is(tau::wff)) return;
		if (!tau::get(m)[0].is(tau::wff_and)) return;
		if (!done.insert(m).second) return;
		for (tref c : tau::get(m)[0].children()) self(self, c);
	};
	auto down = [&skip, &done, &mark_spine](tref n) {
		// Skip intermediate nodes
		if (!tau::get(n).is(tau::wff)) return n;
		if (done.count(n)) {
			// A marked node must yield no assumption: every conjunct
			// of its flattening is a disjunction (the only conjuncts
			// the loop below skips).
			DBG(for (tref l : get_cnf_wff_clauses<node>(n))
				assert(tau::get(l).child_is(tau::wff_or));)
			return n;
		}
		const tau& t_n = tau::get(n)[0];
		// Skip or
		if (t_n.is(tau::wff_or))
			return n;
		// If no conjunction is found, no simplification is possible
		if (!t_n.is(tau::wff_and)) return n;
		subtree_map<node, tref> assignments;
		// Get all conjuncted assumptions
		for (tref l : get_cnf_wff_clauses<node>(n)) {
			if (tau::get(l).child_is(tau::wff_or))
				continue;
			// Treat subtree l as variable
			if (tau::get(l).child_is(tau::wff_neg))
				assignments.emplace(tau::trim2(l), _F<node>());
			else assignments.emplace( l, _T<node>());
		}
		if (assignments.empty()) mark_spine(mark_spine, n);
		tref simp = rewriter::replace(n, assignments);
		// If simp is false, current branch is not sat
		if (tau::get(simp).equals_F()) {
			// Remove branch
			return _F<node>();
		}
		// Rebuild assumptions
		trefs sorted_assms;
		sorted_assms.reserve(assignments.size());
		for (auto [v, k] : assignments)
			if (k == tau::_F())
				sorted_assms.push_back(tau::build_wff_neg(v));
			else sorted_assms.push_back(v);
		// Canonical sorting
		std::ranges::stable_sort(sorted_assms,
			syntactic_path_simplification_wff_comp<node>);
		tref assms = tau::build_wff_and(sorted_assms);
		// Make sure to not revisit built assumptions
		if (tau::get(simp) == get_T<node>())
			skip = tau::get(assms).first();
		else skip = assms;
		// Conjunct assumptions with simplified tree
		return tau::build_wff_and(assms, simp);
	};
	auto visit = [&skip](tref n) {
		if (skip != nullptr && tau::get(n) == tau::get(skip))
			return skip = nullptr, false;
		return is_formula<node>(n);
	};
	return pre_order<node>(root).template
		apply_unique(down, visit);
}

/**
 * @internal
 * @brief The bf-term analog of `syntactic_path_simplification_simplify_wff`:
 * within each top-level `bf_and`, collects factor/negated-factor
 * assignments and collapses to `0` if a contradiction (a factor and its
 * negation both present) is found. Does not descend into `bf_or`.
 * @tparam node Tree node type.
 * @param root Boolean term to scan.
 * @return @p root with contradictory conjunctions collapsed to `0`.
 *
 * @par Example
 * @code{.cpp}
 * // "x & x'" collapses to 0 (see
 * // tests/integration/test_integration-heuristics-syntactic_path_simplification.cpp:111-115).
 * tref fm = get_bf_nso_rr("", "x & x'").value().main->get();
 * tref res = syntactic_path_simplification_simplify_bf<node_t>(fm);
 * CHECK( tau::get(res).equals_0() );
 * @endcode
 * @endinternal
 */
template <NodeType node>
tref syntactic_path_simplification_simplify_bf(tref root) {
	using tau = tree<node>;
	tref skip = nullptr;
	auto down = [&skip](tref n) {
		// Skip intermediate nodes
		if (!tau::get(n).is(tau::bf)) return n;
		const tau& t_n = tau::get(n)[0];
		// Skip or and xor
		if (t_n.is(tau::bf_or) || t_n.is(tau::bf_xor))
			return n;
		// If no conjunction is found, no simplification is possible
		if (!t_n.is(tau::bf_and)) return n;
		subtree_map<node, tref> assignments;
		// Get all conjuncted assumptions
		for (tref l : get_cnf_bf_clauses<node>(n)) {
			if (tau::get(l).child_is(tau::bf_or))
				continue;
			if (tau::get(l).child_is(tau::bf_xor))
				continue;
			// Treat subtree l as variable
			if (tau::get(l).child_is(tau::bf_neg))
				assignments.emplace(tau::trim2(l),
					_0<node>(find_ba_type<node>(l)));
			else assignments.emplace( l,
				_1<node>(find_ba_type<node>(l)));
		}
		tref simp = rewriter::replace_if(n, assignments,
			while_is_boolean_operation<node>);
		// If simp is false, current branch is not sat
		if (tau::get(simp).equals_0()) {
			// Remove branch
			return simp;
		}
		// Rebuild assumptions
		trefs sorted_assms (assignments.size());
		// Canonical sorting
		std::ranges::partial_sort_copy(std::views::keys(assignments),
			sorted_assms, tau::subtree_less);
		tref assms = _1<node>(find_ba_type<node>(n));
		for (tref a : sorted_assms) {
			assms = tau::get(assignments[a]).equals_1()
					? tau::build_bf_and(assms, a)
					: tau::build_bf_and(assms,
						tau::build_bf_neg(a));
		}
		// Make sure to not revisit built assumptions
		if (tau::get(simp).equals_1())
			skip = tau::get(assms).first();
		else skip = assms;
		// Conjunct assumptions with simplified tree
		return tau::build_bf_and(assms, simp);
	};
	auto visit = [&](tref n) {
		if (skip != nullptr && tau::get(n) == tau::get(skip))
			return skip = nullptr, false;
		return is_boolean_operation<node>(n) || is<node, tau::bf>(n);
	};
	return pre_order<node>(root).template
		apply_unique(down, visit);
}


// The main entry before stage 2, over the legacy sweeps above.
template <NodeType node>
tref syntactic_path_simplification(tref fm) {
	using tau = tree<node>;
	auto memo = [](tref r) { return r; };
	tref res = nullptr;
	if (tau::get(fm).is_term()) {
		if (tau::get(fm).equals_0() || tau::get(fm).equals_1())
			return memo(fm);
		// Resolve contradictions
		fm = push_negation_in<node, false>(fm);
		fm = syntactic_path_simplification_simplify_bf<node>(fm);
		// Resolve tautologies
		fm = push_negation_in<node, false>(tau::build_bf_neg(fm));
		fm = syntactic_path_simplification_simplify_bf<node>(fm);
		res = push_negation_in<node, false>(tau::build_bf_neg(fm));
	} else {
		if (tau::get(fm).equals_F() || tau::get(fm).equals_T())
			return memo(fm);
		// Resolve contradictions
		fm = normalize_atomic_formula_operators<node>(to_nnf<node>(fm));
		fm = syntactic_path_simplification_simplify_wff<node>(fm);
		// Resolve tautologies
		fm = normalize_atomic_formula_operators<node>(to_nnf<node>(tau::build_wff_neg(fm)));
		fm = syntactic_path_simplification_simplify_wff<node>(fm);
		res = to_nnf<node>(tau::build_wff_neg(fm));
	}
	return memo(res);
}

} // namespace legacy

// ── stage 1: the environment sweep against the eager substitution ────────────

namespace {

using idni::tau_lang::normalize_atomic_formula_operators;
using idni::tau_lang::to_nnf;

// The formula-level pipeline of the main entry up to the first sweep.
tref nnf_normalised(tref fm) {
	return normalize_atomic_formula_operators<node_t>(to_nnf<node_t>(fm));
}

// Equality up to the associativity and commutativity of the joins: every
// and/or spine flattened and its members sorted. The eager sweep nests a
// newly exposed literal in its own block while the environment sweep sorts
// it into the enclosing one; both are the same formula.
tref canonical(tref n) {
	// Same spelling on both sides: the entry keeps `!(l = r)` where a parsed
	// expectation reads `l != r`.
	if (tau::get(n).is(tau::wff))
		n = normalize_atomic_formula_operators<node_t>(n);
	auto f = [](tref m) -> tref {
		const auto& t = tau::get(m);
		if (!t.has_child()) return m;
		const auto nt = static_cast<typename node_t::type>(t[0].value.nt);
		const bool wff_join = t.is(tau::wff)
			&& (nt == tau::wff_and || nt == tau::wff_or);
		const bool bf_join = t.is(tau::bf)
			&& (nt == tau::bf_and || nt == tau::bf_or);
		if (!wff_join && !bf_join) return m;
		trefs leaves = get_leaves<node_t>(m, nt);
		std::sort(leaves.begin(), leaves.end(), tau::subtree_less);
		if (wff_join) return nt == tau::wff_and
			? tau::build_wff_and(leaves) : tau::build_wff_or(leaves);
		const size_t ty = find_ba_type<node_t>(m);
		return nt == tau::bf_and
			? tau::build_bf_and(leaves, ty) : tau::build_bf_or(leaves, ty);
	};
	return post_order<node_t>(n).apply_unique(f);
}

const char* const wff_samples[] = {
	"x = 0.",
	"x = 0 || y = 0.",
	"x = 0 && !(x = 0).",
	"x = 0 && y = 0 && x = 0.",
	"x = 0 && y = 0.",
	"x = 0 && (y = 0 || !(x = 0)).",
	"x = 0 && (y = 0 || (z = 0 && !(x = 0))).",
	"(x = 0 || y = 0) && (z = 0 || x = 0) && !(x = 0).",
	"x = 0 && (z != 0 || (y = 0 && (k = 0 || x != 0))) && x = 0 || x = 0 && y = 0 || z = 0 && (z != 0 || k = 0) && z = 0.",
	"x = 0 && (z != 0 || (y = 0 && (k = 0 || x != 0))) && x = 0 || x = 0 && y = 0 || z = 0 && (z != 0 || k = 0) && z = 0 || (j = 0 && l = 0) || k = 0 || !(j = 0 && l = 0).",
	"x = 0 && !(x = 0 || y = 0).",
	"(x = 0 && y = 0) || (x = 0 && !(y = 0)) || (!(x = 0) && z = 0).",
};

const char* const bf_samples[] = {
	"x",
	"x | y",
	"x & x'",
	"x & z & x'",
	"x & (x' | y)",
	"x & (z' | (y & (k | x'))) & x | x & y | z & (z' | k) & z",
	"x & (z' | (y & (k | x'))) & x | x & y | z & (z' | k) & z | (j & l) | k | (j & l)'",
	"(x | y) & (x' | z) & (y' | z')",
};

} // namespace

// ── syntactic_path_simplification_wff_comp ───────────────────────────────────

TEST_SUITE("syntactic_path_simplification_wff_comp") {

	TEST_CASE("equality < non-equality") {
		tref eq_atm  = get_nso_rr("x = 0.").value().main->get();
		tref neq_atm = get_nso_rr("y != 0.").value().main->get();
		CHECK(syntactic_path_simplification_wff_comp<node_t>(eq_atm, neq_atm)  == true);
		CHECK(syntactic_path_simplification_wff_comp<node_t>(neq_atm, eq_atm) == false);
	}

	TEST_CASE("equality is irreflexive") {
		tref atm = get_nso_rr("x = 0.").value().main->get();
		CHECK(syntactic_path_simplification_wff_comp<node_t>(atm, atm) == false);
	}

	TEST_CASE("non-equality is irreflexive") {
		tref atm = get_nso_rr("x != 0.").value().main->get();
		CHECK(syntactic_path_simplification_wff_comp<node_t>(atm, atm) == false);
	}

	TEST_CASE("two distinct equalities are strictly ordered") {
		tref atm_x = get_nso_rr("x = 0.").value().main->get();
		tref atm_y = get_nso_rr("y = 0.").value().main->get();
		bool xy = syntactic_path_simplification_wff_comp<node_t>(atm_x, atm_y);
		bool yx = syntactic_path_simplification_wff_comp<node_t>(atm_y, atm_x);
		CHECK(xy != yx);
	}

	TEST_CASE("two distinct non-equalities are strictly ordered") {
		tref atm_x = get_nso_rr("x != 0.").value().main->get();
		tref atm_y = get_nso_rr("y != 0.").value().main->get();
		bool xy = syntactic_path_simplification_wff_comp<node_t>(atm_x, atm_y);
		bool yx = syntactic_path_simplification_wff_comp<node_t>(atm_y, atm_x);
		CHECK(xy != yx);
	}
}

// ── syntactic_path_simplification_simplify_wff ───────────────────────────────

TEST_SUITE("syntactic_path_simplification_simplify_wff") {

	TEST_CASE("pure atom returned unchanged") {
		tref fm  = get_nso_rr("x = 0.").value().main->get();
		tref res = syntactic_path_simplification_simplify_wff<node_t>(fm);
		CHECK(tau::get(res) == tau::get(fm));
	}

	TEST_CASE("OR formula returned unchanged at top level") {
		tref fm  = get_nso_rr("x = 0 || y = 0.").value().main->get();
		tref res = syntactic_path_simplification_simplify_wff<node_t>(fm);
		CHECK(tau::get(res) == tau::get(fm));
	}

	TEST_CASE("x = 0 && !(x = 0) collapses to F") {
		tref fm  = get_nso_rr("x = 0 && !(x = 0).").value().main->get();
		tref res = syntactic_path_simplification_simplify_wff<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}

	TEST_CASE("negated bf contradiction under wff context collapses to F") {
		tref fm  = get_nso_rr("y = 0 && !(x & x' = 0).").value().main->get();
		tref res = syntactic_path_simplification_simplify_wff<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}

	TEST_CASE("duplicate conjunct is simplified") {
		tref fm  = get_nso_rr("x = 0 && y = 0 && x = 0.").value().main->get();
		tref res = syntactic_path_simplification_simplify_wff<node_t>(fm);
		// The duplicate x = 0 is removed; result should differ from input
		CHECK(tau::get(res) != tau::get(fm));
	}

	TEST_CASE("no negation: simple conjunction unchanged structure") {
		tref fm  = get_nso_rr("x = 0 && y = 0.").value().main->get();
		tref res = syntactic_path_simplification_simplify_wff<node_t>(fm);
		// No contradiction; formula may be restructured but is not F
		CHECK(!tau::get(res).equals_F());
	}
}

// ── syntactic_path_simplification_simplify_bf ────────────────────────────────

TEST_SUITE("syntactic_path_simplification_simplify_bf") {

	TEST_CASE("0 returned unchanged") {
		tref fm  = get_bf_nso_rr("", "0").value().main->get();
		tref res = syntactic_path_simplification_simplify_bf<node_t>(fm);
		CHECK(tau::get(res) == tau::get(fm));
	}

	TEST_CASE("1 returned unchanged") {
		tref fm  = get_bf_nso_rr("", "1").value().main->get();
		tref res = syntactic_path_simplification_simplify_bf<node_t>(fm);
		CHECK(tau::get(res) == tau::get(fm));
	}

	TEST_CASE("OR term returned unchanged at top level") {
		tref fm  = get_bf_nso_rr("", "x | y").value().main->get();
		tref res = syntactic_path_simplification_simplify_bf<node_t>(fm);
		CHECK(tau::get(res) == tau::get(fm));
	}

	TEST_CASE("x & x' collapses to 0") {
		tref fm  = get_bf_nso_rr("", "x & x'").value().main->get();
		tref res = syntactic_path_simplification_simplify_bf<node_t>(fm);
		CHECK(tau::get(res).equals_0());
	}

	TEST_CASE("x & z & x' collapses to 0") {
		tref fm  = get_bf_nso_rr("", "x & z & x'").value().main->get();
		tref res = syntactic_path_simplification_simplify_bf<node_t>(fm);
		CHECK(tau::get(res).equals_0());
	}
}

// ── syntactic_path_simplification_unsat_on_unchanged_negations ───────────────

TEST_SUITE("syntactic_path_simplification_unsat_on_unchanged_negations") {

	TEST_CASE("wff: x = 0 && !(x = 0) collapses to F") {
		tref fm  = get_nso_rr("x = 0 && !(x = 0).").value().main->get();
		tref res = syntactic_path_simplification_unsat_on_unchanged_negations<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}

	TEST_CASE("wff: NNF contradiction x = 0 && x != 0 collapses to F") {
		// `x != 0` is filed under the key of `x = 0` with the opposite
		// polarity (stage 3), so the contradiction is seen without any
		// negation being rewritten.
		tref fm  = get_nso_rr("x = 0 && x != 0.").value().main->get();
		tref res = syntactic_path_simplification_unsat_on_unchanged_negations<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}

	TEST_CASE("bf: x & x' collapses to 0") {
		tref fm  = get_bf_nso_rr("", "x & x'").value().main->get();
		tref res = syntactic_path_simplification_unsat_on_unchanged_negations<node_t>(fm);
		CHECK(tau::get(res).equals_0());
	}

	TEST_CASE("bf: non-tautological OR is not turned into 1") {
		// unsat_on_unchanged_negations has no tautology pass.
		// A non-tautological OR (x | y) must not become 1.
		tref fm  = get_bf_nso_rr("", "x | y").value().main->get();
		tref res = syntactic_path_simplification_unsat_on_unchanged_negations<node_t>(fm);
		CHECK(!tau::get(res).equals_1());
	}
}

// ── syntactic_path_simplification (main entry point) ─────────────────────────

TEST_SUITE("syntactic_path_simplification") {
	TEST_CASE("1") {
		const char* sample = "x & (z' | (y & (k | x'))) & x | x & y | z & (z' | k) & z | (j & l) | k | (j & l)'";
		tref fm = get_bf_nso_rr("", sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		CHECK((tau::get(res).to_str() == "1"));
	}
	TEST_CASE("2") {
		const char* sample = "x = 0 && (z != 0 || (y = 0 && (k = 0 || x != 0))) && x = 0 || x = 0 && y = 0 || z = 0 && (z != 0 || k = 0) && z = 0.";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		// Debug pins expected[0] as the canonical shape (see test_helpers.h);
		// Release accepts any listed variant.
		// First entry: the environment sweep (stage 1) sorts a literal that a
		// disjunct folds into (here k = 0) into the enclosing block.
		// Earlier entries: parser regens 8f1a74c1 and 2026-08-27 flipped
		// the atom order.
		CHECK( matches_to_str_to_any_of(res, {
			"x = 0 && (z != 0 || k = 0 && y = 0) || y = 0 && x = 0 || k = 0 && z = 0",
			"x = 0 && (z != 0 || k = 0 && y = 0) || y = 0 && x = 0 || z = 0 && k = 0",
			"x = 0 && (z != 0 || y = 0 && k = 0) || x = 0 && y = 0 || z = 0 && k = 0",
			"x = 0 && (z != 0 || y = 0 && k = 0) || y = 0 && x = 0 || z = 0 && k = 0",
			"x = 0 && (z != 0 || k = 0 && y = 0) || x = 0 && y = 0 || z = 0 && k = 0",
		}) );
	}
	TEST_CASE("3") {
		const char* sample = "x & (z' | (y & (k | x'))) & x | x & y | z & (z' | k) & z";
		tref fm = get_bf_nso_rr("", sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		// Order flipped by the 8f1a74c1 parser regen (Debug's
		// matches_to_any_of only checks expected[0] -- see test_helpers.h).
		CHECK( matches_to_str_to_any_of(res, {
			"x&(z'|yk)|yx|zk",
			"x&(z'|ky)|yx|zk",
			"x&(z'|ky)|xy|zk",
			"x&(z'|yk)|xy|zk",
		}) );
	}
	TEST_CASE("4") {
		const char* sample = "x = 0 && (z != 0 || (y = 0 && (k = 0 || x != 0))) && x = 0 || x = 0 && y = 0 || z = 0 && (z != 0 || k = 0) && z = 0 || (j = 0 && l = 0) || k = 0 || !(j = 0 && l = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		CHECK((tau::get(res).to_str() == "T"));
	}
	TEST_CASE("4_5") {
		const char* sample = "(ex x x = 0) && (ex x x != 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		CHECK( matches_to_str_to_any_of(res, {
			"(ex b1 b1 != 0) && (ex b1 b1 = 0)",
			"(ex b1 b1 = 0) && (ex b1 b1 != 0)",
		}) );
	}
	// syntactic_path_simplification_simplify_wff (the internal helper used
	// above) assumes NNF-normalized input: it detects contradictions only
	// between exact-duplicate top-level conjuncts, so "x = 0" and the
	// non-NNF "!(x = 0 || y = 0)" are not recognized as contradictory
	// (the negated disjunction is a different subtree, not "x = 0"
	// itself). The public entry point normalizes to NNF first, so it
	// must still collapse this to F.
	TEST_CASE("non-NNF input with negated disjunction collapses to F") {
		const char* sample = "x = 0 && !(x = 0 || y = 0).";
		tref fm = get_nso_rr(sample).value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}
}

TEST_SUITE("syntactic_path_simplification_stage1") {

	// The sweep alone, on the input shape the main entry hands it: exact
	// equality with the eager substitution on quantifier-free input.
	TEST_CASE("wff sweep matches the legacy sweep on quantifier-free input") {
		for (const char* sample : wff_samples) {
			tref fm = nnf_normalised(get_nso_rr(sample).value().main->get());
			tref legacy_res = legacy::syntactic_path_simplification_simplify_wff<node_t>(fm);
			tref res = syntactic_path_simplification_simplify_wff<node_t>(fm);
			CAPTURE(sample);
			CHECK(tau::get(canonical(res)) == tau::get(canonical(legacy_res)));
		}
	}

	TEST_CASE("bf sweep matches the legacy sweep on quantifier-free input") {
		for (const char* sample : bf_samples) {
			tref fm = push_negation_in<node_t, false>(
				get_bf_nso_rr("", sample).value().main->get());
			tref legacy_res = legacy::syntactic_path_simplification_simplify_bf<node_t>(fm);
			tref res = syntactic_path_simplification_simplify_bf<node_t>(fm);
			CAPTURE(sample);
			CHECK(tau::get(canonical(res)) == tau::get(canonical(legacy_res)));
		}
	}

	// The sweep reaches a fixpoint in one call: a second call over its
	// output changes nothing. (The two-pass main entry is not idempotent
	// on its own: its last sweep runs over the negation and sorts the
	// negated literals; the single pass of stage 2 makes it so.)
	TEST_CASE("sweep is idempotent") {
		for (const char* sample : wff_samples) {
			tref fm = nnf_normalised(get_nso_rr(sample).value().main->get());
			tref once = syntactic_path_simplification_simplify_wff<node_t>(fm);
			tref twice = syntactic_path_simplification_simplify_wff<node_t>(once);
			CAPTURE(sample);
			CHECK(tau::get(once) == tau::get(twice));
		}
		for (const char* sample : bf_samples) {
			tref fm = push_negation_in<node_t, false>(
				get_bf_nso_rr("", sample).value().main->get());
			tref once = syntactic_path_simplification_simplify_bf<node_t>(fm);
			tref twice = syntactic_path_simplification_simplify_bf<node_t>(once);
			CAPTURE(sample);
			CHECK(tau::get(once) == tau::get(twice));
		}
	}

	// Ruling 1: a quantified conjunct's body is entered, so a contradiction
	// inside it is found from the conjunction position as well.
	TEST_CASE("contradiction inside a quantified conjunct collapses to F") {
		tref fm = get_nso_rr("x = 0 && (ex y (y = 0 && z = 0 && !(y = 0))).")
			.value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}

	// Ruling 1: sibling keys are in force inside a quantified conjunct.
	TEST_CASE("sibling key reaches into a quantified conjunct") {
		tref fm = get_nso_rr("x = 0 && (ex y (x = 0 || y = 0)).")
			.value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		// The quantified body folds to T under x = 0, the binder over T
		// folds to T, and the conjunct vanishes.
		tref expected = get_nso_rr("x = 0.").value().main->get();
		CHECK(tau::get(res) == tau::get(expected));
	}

	// The capture guard: a binder over a variable that a key in force
	// mentions suspends that key inside the binder. The binder is built
	// with the clashing name on purpose (parse-time canonicalisation would
	// rename it).
	TEST_CASE("capture guard: wff binder shadows an outer key") {
		tref a = get_nso_rr("x = 0.").value().main->get();
		tref b = get_nso_rr("x != 0.").value().main->get();
		tref x = tau::get(a).find_top(is<node_t, tau::variable>);
		REQUIRE(x != nullptr);
		tref fm = tau::build_wff_and(a, tau::build_wff_ex(x, b, false));
		tref res = syntactic_path_simplification<node_t>(fm);
		// x = 0 && ex x (x != 0) is satisfiable: the inner x is bound.
		CHECK(!tau::get(res).equals_F());
	}

	TEST_CASE("capture guard: bf functional binder shadows an outer key") {
		tref x  = get_bf_nso_rr("", "x").value().main->get();
		tref body = get_bf_nso_rr("", "x' & y").value().main->get();
		tref z  = get_bf_nso_rr("", "z").value().main->get();
		tref xv = tau::get(x).find_top(is<node_t, tau::variable>);
		REQUIRE(xv != nullptr);
		// x & (fex x (x' & y) | z): the inner x is bound, so the term is
		// x & (y | z), never x & z.
		tref fm = tau::build_bf_and(x, tau::build_bf_or(
			tau::build_bf_fex(xv, body, false), z));
		tref res = syntactic_path_simplification<node_t>(fm);
		tref wrong = tau::build_bf_and(x, z);
		CHECK(tau::get(res) != tau::get(wrong));
	}

	// Ruling 2: a temporal conjunct is a key as a whole but never entered.
	TEST_CASE("temporal conjunct body is not entered") {
		// Built by hand: the parser rejects a temporal operator next to an
		// unscoped atom at the top level.
		tref a = get_nso_rr("x = 0.").value().main->get();
		tref t = get_nso_rr("always (x = 0 || y = 0).").value().main->get();
		REQUIRE(tau::get(t).child_is(tau::wff_always));
		tref fm = tau::build_wff_and(a, t);
		tref res = syntactic_path_simplification_unsat_on_unchanged_negations<node_t>(fm);
		CHECK(tau::get(canonical(res)) == tau::get(canonical(fm)));
	}

	TEST_CASE("units_opaque leaves a quantified conjunct untouched") {
		tref fm = get_nso_rr("x = 0 && (ex y (x = 0 || y = 0)).")
			.value().main->get();
		path_sweep_options opts; opts.units_opaque = true;
		tref res = syntactic_path_simplification_simplify_wff<node_t>(fm, opts);
		CHECK(tau::get(res) == tau::get(fm));
	}
}

// ── stage 2: one unified pass ────────────────────────────────────────────────

TEST_SUITE("syntactic_path_simplification_stage2") {

	// The unified pass is equivalent to the former two-pass entry on every
	// sample, and at least as simplified (it may fold more, so this is an
	// equivalence, not a syntactic comparison).
	TEST_CASE("main entry is equivalent to the legacy entry on formulas") {
		for (const char* sample : wff_samples) {
			tref fm = get_nso_rr(sample).value().main->get();
			tref legacy_res = legacy::syntactic_path_simplification<node_t>(fm);
			tref res = syntactic_path_simplification<node_t>(fm);
			CAPTURE(sample);
			CHECK(are_nso_equivalent<node_t>(res, legacy_res));
		}
	}

	TEST_CASE("main entry is equivalent to the legacy entry on terms") {
		for (const char* sample : bf_samples) {
			tref fm = get_bf_nso_rr("", sample).value().main->get();
			tref legacy_res = legacy::syntactic_path_simplification<node_t>(fm);
			tref res = syntactic_path_simplification<node_t>(fm);
			CAPTURE(sample);
			// Two terms are one function exactly when `t1 = t2` is valid.
			CHECK(are_nso_equivalent<node_t>(
				tau::build_bf_eq(res, legacy_res), _T<node_t>()));
		}
	}

	// With one pass there is no negated second sweep to reorder literals:
	// the entry itself is a fixpoint.
	TEST_CASE("main entry is idempotent") {
		for (const char* sample : wff_samples) {
			tref fm = get_nso_rr(sample).value().main->get();
			tref once = syntactic_path_simplification<node_t>(fm);
			tref twice = syntactic_path_simplification<node_t>(once);
			CAPTURE(sample);
			CHECK(tau::get(once) == tau::get(twice));
		}
		for (const char* sample : bf_samples) {
			tref fm = get_bf_nso_rr("", sample).value().main->get();
			tref once = syntactic_path_simplification<node_t>(fm);
			tref twice = syntactic_path_simplification<node_t>(once);
			CAPTURE(sample);
			CHECK(tau::get(once) == tau::get(twice));
		}
	}

	TEST_CASE("a disjunct is assumed false in its siblings") {
		tref fm = get_nso_rr("x = 0 || (y = 0 && x != 0).").value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		tref expected = get_nso_rr("x = 0 || y = 0.").value().main->get();
		CHECK(tau::get(canonical(res)) == tau::get(canonical(expected)));
	}

	TEST_CASE("both kinds of assumption are in force along a path") {
		// Under x = 0 (conjunct) and y != 0 assumed false (disjunct), the
		// inner conjunction folds to z = 0.
		tref fm = get_nso_rr("x = 0 && (y != 0 || (z = 0 && x = 0 && y = 0)).")
			.value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		tref expected = get_nso_rr("x = 0 && (y != 0 || z = 0).").value().main->get();
		CHECK(tau::get(canonical(res)) == tau::get(canonical(expected)));
	}

	TEST_CASE("unchanged-negations entry still has no tautology pass") {
		tref fm = get_bf_nso_rr("", "x | x'").value().main->get();
		tref res = syntactic_path_simplification_unsat_on_unchanged_negations<node_t>(fm);
		// Folded by the construction hooks at parse time, or left alone:
		// never turned into 1 by this entry.
		CHECK((tau::get(res) == tau::get(fm)));
	}
}

// ── stage 3: complement-aware keys ───────────────────────────────────────────

TEST_SUITE("syntactic_path_simplification_stage3") {

	// A negated equality is the complement of its equality whatever its
	// spelling, so the entry that never rewrites negations sees it too.
	TEST_CASE("unchanged-negations entry: x = 0 && x != 0 collapses to F") {
		tref fm = get_nso_rr("x = 0 && x != 0.").value().main->get();
		tref res = syntactic_path_simplification_unsat_on_unchanged_negations<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}

	TEST_CASE("unchanged-negations entry: x != 0 && (y = 0 || x = 0) drops the dead disjunct") {
		tref fm = get_nso_rr("x != 0 && (y = 0 || x = 0).").value().main->get();
		tref res = syntactic_path_simplification_unsat_on_unchanged_negations<node_t>(fm);
		tref expected = get_nso_rr("x != 0 && y = 0.").value().main->get();
		CHECK(tau::get(canonical(res)) == tau::get(canonical(expected)));
	}

	// Ordering atoms are total-order comparisons on bitvectors; `a <= b` is
	// the complement of `b < a`, which the atom normalisation leaves as two
	// distinct positive atoms.
	TEST_CASE("ordering contradiction collapses to F") {
		tref fm = get_nso_rr("x < { 3 }:bv[4] && { 3 }:bv[4] <= x.").value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		CHECK(tau::get(res).equals_F());
	}

	TEST_CASE("ordering tautology collapses to T") {
		tref fm = get_nso_rr("x <= { 3 }:bv[4] || { 3 }:bv[4] < x.").value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		CHECK(tau::get(res).equals_T());
	}

	TEST_CASE("ordering key reaches a sibling in another spelling") {
		// Under x < 3 the disjunct 3 <= x is dead.
		tref fm = get_nso_rr("x < { 3 }:bv[4] && (y = { 1 }:bv[4] || { 3 }:bv[4] <= x).")
			.value().main->get();
		tref res = syntactic_path_simplification<node_t>(fm);
		tref expected = get_nso_rr("x < { 3 }:bv[4] && y = { 1 }:bv[4].").value().main->get();
		CHECK(tau::get(canonical(res)) == tau::get(canonical(expected)));
	}
}

// ── stage 4: atom fingerprints and the relevant-key memo ─────────────────────

TEST_SUITE("syntactic_path_simplification_stage4") {

	TEST_CASE("a subtree's fingerprint is covered by its parent's") {
		tref fm = get_nso_rr("x = 0 && (y != 0 || (z = 0 && ex w (w = 0 || x = 0))).")
			.value().main->get();
		const path_bits& whole = path_bits_of<node_t, true>(fm);
		bool ok = true;
		auto check = [&](tref n) {
			if (tau::get(n).is(tau::wff))
				ok = ok && whole.covers(path_bits_of<node_t, true>(n));
		};
		pre_order<node_t>(fm).visit(check);
		CHECK(ok);
		CHECK(whole.any());
	}

	TEST_CASE("both spellings of an equality share one bit") {
		tref eq  = get_nso_rr("x = 0.").value().main->get();
		tref neq = get_nso_rr("x != 0.").value().main->get();
		tref neg = get_nso_rr("!(x = 0).").value().main->get();
		const path_bits& a = path_bits_of<node_t, true>(eq);
		CHECK(a.covers(path_bits_of<node_t, true>(neq)));
		CHECK(path_bits_of<node_t, true>(neq).covers(a));
		CHECK(a.covers(path_bits_of<node_t, true>(neg)));
		CHECK(path_bits_of<node_t, true>(neg).covers(a));
	}

	TEST_CASE("a constant has no atoms") {
		CHECK(!path_bits_of<node_t, true>(_T<node_t>()).any());
		CHECK(!path_bits_of<node_t, true>(_F<node_t>()).any());
	}

	// A key that cannot occur in a subtree leaves that subtree's result a
	// property of the subtree alone: the same simplified subtree comes back
	// whatever the enclosing conjunction asserts about other atoms.
	TEST_CASE("an unreachable key does not change a subtree's result") {
		tref inner = get_nso_rr("y = 0 || (z = 0 && !(z = 0)).").value().main->get();
		tref alone = syntactic_path_simplification<node_t>(inner);
		tref under = syntactic_path_simplification<node_t>(
			tau::build_wff_and(get_nso_rr("x = 0.").value().main->get(), inner));
		// under x = 0 the inner disjunction is the same simplified node
		CHECK(tau::get(under).find_top([&](tref n) {
			return tau::subtree_equals(n, alone); }) != nullptr);
	}
}
