// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Pointwise revision for nested-temporal LTL(ABA).
//
// Implements the hybrid semantic-syntactic algorithm from pwr-ltl.tex:
//   - Semantic per-step formula ((∃o. α∧β) → α) ∧ β at atomic leaves
//   - REAL checks at temporal operator nodes for commitment-side selection
//   - And-distribution into invariant slots for clause-level granularity
//   - Semantic optimal mode fallback (pwr-ltl.tex §11): winning-region
//     revision via Algorithm D when fast mode drops a clause
//
// On safety inputs G(α) * G(β), this produces exactly the old per-step
// formula G(((∃o.α∧β)→α) ∧ β), ensuring backward compatibility.

#ifndef __IDNI__TAU__POINTWISE_REVISION_H__
#define __IDNI__TAU__POINTWISE_REVISION_H__

#include "satisfiability.h"
#include "semantic_pwr.h"

namespace idni::tau_lang {

// ---------------------------------------------------------------------------
// AST classification helpers
// ---------------------------------------------------------------------------

// Identify the temporal operator kind of a wff node.
enum class temporal_op { NONE, ALWAYS, SOMETIMES, F, U, R, W, S, T };

template <NodeType node>
temporal_op get_temporal_op(tref fm) {
	using tau = tree<node>;
	if (!fm) return temporal_op::NONE;
	const auto& t = tau::get(fm);
	if (!t.has_child()) return temporal_op::NONE;
	auto nt = t[0].value.nt;
	if (nt == tau::wff_always)    return temporal_op::ALWAYS;
	if (nt == tau::wff_sometimes) return temporal_op::SOMETIMES;
	if (nt == tau::wff_F)         return temporal_op::F;
	if (nt == tau::wff_U)         return temporal_op::U;
	if (nt == tau::wff_R)         return temporal_op::R;
	if (nt == tau::wff_W)         return temporal_op::W;
	if (nt == tau::wff_S)         return temporal_op::S;
	if (nt == tau::wff_T)         return temporal_op::T;
	return temporal_op::NONE;
}

template <NodeType node>
bool is_temporal(tref fm) {
	return get_temporal_op<node>(fm) != temporal_op::NONE;
}

// Check if the operator is binary (U/R/W/S/T) vs unary (G/F/sometimes).
inline bool is_binary_temporal(temporal_op op) {
	return op == temporal_op::U || op == temporal_op::R
	    || op == temporal_op::W || op == temporal_op::S
	    || op == temporal_op::T;
}

// Check whether a node is a boolean combination (and/or/neg) at the wff level.
template <NodeType node>
bool is_wff_boolean(tref fm) {
	using tau = tree<node>;
	if (!fm) return false;
	const auto& t = tau::get(fm);
	if (!t.has_child()) return false;
	auto nt = t[0].value.nt;
	return nt == tau::wff_and || nt == tau::wff_or || nt == tau::wff_neg
	    || nt == tau::wff_imply || nt == tau::wff_equiv
	    || nt == tau::wff_xor;
}

// Check whether a node is an atom (non-temporal wff leaf).
template <NodeType node>
bool is_atom_leaf(tref fm) {
	return !is_temporal<node>(fm) && !is_wff_boolean<node>(fm);
}

// ---------------------------------------------------------------------------
// Role decomposition: invariant side vs commitment side
// ---------------------------------------------------------------------------

// For a binary temporal operator, extract the invariant and commitment children.
//   U: inv=LHS, commit=RHS    (φ₁ U φ₂)
//   W: inv=LHS, commit=RHS    (φ₁ W φ₂) — weak until
//   R: inv=RHS, commit=LHS    (φ₁ R φ₂) — dual of U
//   S: inv=LHS, commit=RHS    (φ₁ S φ₂)
//   T: inv=RHS, commit=LHS    (φ₁ T φ₂) — dual of S

template <NodeType node>
struct role_pair { tref invariant; tref commitment; };

template <NodeType node>
role_pair<node> decompose_roles(tref fm) {
	using tau = tree<node>;
	auto op = get_temporal_op<node>(fm);
	// Only valid for binary temporal operators (U/R/W/S/T).
	// G, F, sometimes are unary — do not call this on them.
	if (op == temporal_op::ALWAYS || op == temporal_op::SOMETIMES
	    || op == temporal_op::F || op == temporal_op::NONE)
		return { nullptr, nullptr };
	tref lhs = tau::get(fm)[0].first();
	tref rhs = tau::get(fm)[0].child(1);
	switch (op) {
		case temporal_op::U:
		case temporal_op::W:
		case temporal_op::S:
			return { lhs, rhs };
		case temporal_op::R:
		case temporal_op::T:
			return { rhs, lhs };
		default:
			return { nullptr, nullptr };
	}
}

// Rebuild a binary temporal operator from invariant and commitment sides.
template <NodeType node>
tref rebuild_from_roles(temporal_op op, tref inv, tref commit) {
	using tau = tree<node>;
	switch (op) {
		case temporal_op::U: return tau::build_wff_U(inv, commit);
		case temporal_op::W: return tau::build_wff_W(inv, commit);
		case temporal_op::R: return tau::build_wff_R(commit, inv);
		case temporal_op::S: return tau::build_wff_S(inv, commit);
		case temporal_op::T: return tau::build_wff_T(commit, inv);
		default: return nullptr;
	}
}

// ---------------------------------------------------------------------------
// Gather top-level conjuncts from a conjunction chain
// ---------------------------------------------------------------------------

template <NodeType node>
void gather_top_conjuncts(tref fm, std::vector<tref>& out) {
	using tau = tree<node>;
	if (!fm) return;
	const auto& t = tau::get(fm);
	if (t.has_child() && t[0].value.nt == tau::wff_and) {
		const auto& op = t[0];
		for (size_t i = 0; i < op.children_size(); ++i)
			gather_top_conjuncts<node>(op.child(i), out);
		return;
	}
	out.push_back(fm);
}

// ---------------------------------------------------------------------------
// Select current-time system output variables from a formula.
//
// For the per-step semantic formula, we quantify only current-step
// controllable outputs (o_i[t]), NOT lookback outputs (o_i[t-k])
// and NOT input variables (i_j[t]).  In tau-ltl's AST, atoms at
// temporal-tree leaves reference the current time step after
// normalization, so all output io_vars found here are current-time.
// ---------------------------------------------------------------------------

template <NodeType node>
trefs select_output_vars(tref fm) {
	using tau = tree<node>;
	auto pred = [](tref n) {
		if (!is_child<node, tau::io_var>(n)) return false;
		if (!tau::get(n).is_output_variable()) return false;
		// Exclude lookback variables (o[t-k], k>0) — only current-time.
		if (get_io_var_shift<node>(n) != 0) return false;
		return true;
	};
	return tau::get(fm).select_top(pred);
}

// ---------------------------------------------------------------------------
// Semantic per-step formula: ((∃o. α∧β) → α) ∧ β
// ---------------------------------------------------------------------------

template <NodeType node>
tref semantic_revise_atoms(tref alpha, tref beta) {
	using tau = tree<node>;
	// Build α ∧ β
	tref alpha_and_beta = build_wff_and<node>(alpha, beta);
	// Collect output variables from α ∧ β
	trefs out_vars = select_output_vars<node>(alpha_and_beta);
	// ∃o. (α ∧ β)
	tref compat = tau::build_wff_ex_many(out_vars, alpha_and_beta);
	// (∃o. α∧β) → α    ≡    ¬(∃o. α∧β) ∨ α
	tref impl = build_wff_or<node>(
		build_wff_neg<node>(compat), alpha);
	// ((∃o. α∧β) → α) ∧ β
	return build_wff_and<node>(impl, beta);
}

// ---------------------------------------------------------------------------
// Core recursive revision: revise(φ, ψ, ψ_f)
//
// φ   = spec subtree
// ψ   = aligned update subtree
// ψ_f = full update formula (for REAL checks)
// ---------------------------------------------------------------------------

template <NodeType node>
tref revise(tref phi, tref psi, tref psi_f, const int_t start_time) {
	using tau = tree<node>;

	// Early exit: if REAL(φ ∧ ψ_f), keep spec unchanged
	tref conj = build_wff_and<node>(phi, psi_f);
	if (is_tau_formula_sat<node>(conj, start_time))
		return phi;

	// Case 1: Both atoms → semantic per-step formula
	if (is_atom_leaf<node>(phi) && is_atom_leaf<node>(psi))
		return semantic_revise_atoms<node>(phi, psi);

	temporal_op op_phi = get_temporal_op<node>(phi);
	temporal_op op_psi = get_temporal_op<node>(psi);

	// Case 2: Same binary temporal operator (U/R/W/S/T)
	if (is_binary_temporal(op_phi) && op_phi == op_psi) {
		auto [inv_phi, commit_phi] = decompose_roles<node>(phi);
		auto [inv_psi, commit_psi] = decompose_roles<node>(psi);

		// Recurse on invariant sides
		tref r_inv = revise<node>(inv_phi, inv_psi, psi_f, start_time);

		// Try keeping spec's commitment side
		tref candidate = rebuild_from_roles<node>(op_phi, r_inv, commit_phi);
		tref check = build_wff_and<node>(candidate, psi_f);
		if (is_tau_formula_sat<node>(check, start_time))
			return candidate;

		// Fall back to update's commitment side
		return rebuild_from_roles<node>(op_phi, r_inv, commit_psi);
	}

	// Case 2b: Same unary temporal operator (G/F/sometimes)
	if (op_phi == op_psi && !is_binary_temporal(op_phi)
	    && op_phi != temporal_op::NONE) {
		tref inner_phi = tau::get(phi)[0].first();
		tref inner_psi = tau::get(psi)[0].first();
		tref r_inner = revise<node>(inner_phi, inner_psi, psi_f, start_time);
		if (op_phi == temporal_op::ALWAYS)
			return tau::build_wff_always(r_inner);
		if (op_phi == temporal_op::SOMETIMES)
			return build_wff_sometimes<node>(r_inner);
		if (op_phi == temporal_op::F)
			return build_wff_F<node>(r_inner);
		return psi;
	}

	// Case 3: Atom vs binary temporal — lift atom
	// α ≡ op(α, α) under non-strict semantics
	if (is_atom_leaf<node>(phi) && is_binary_temporal(op_psi)) {
		tref lifted = rebuild_from_roles<node>(op_psi, phi, phi);
		return revise<node>(lifted, psi, psi_f, start_time);
	}
	if (is_binary_temporal(op_phi) && is_atom_leaf<node>(psi)) {
		tref lifted = rebuild_from_roles<node>(op_phi, psi, psi);
		return revise<node>(phi, lifted, psi_f, start_time);
	}

	// Case 4: G vs binary temporal — unwrap G(α) as invariant
	// G(α) = ⊥ R α.  If ψ is R-based, decompose and recurse.
	if (op_phi == temporal_op::ALWAYS && (op_psi == temporal_op::R
	    || op_psi == temporal_op::T)) {
		tref inner_phi = tau::get(phi)[0].first();
		auto [inv_psi, commit_psi] = decompose_roles<node>(psi);
		tref r_inv = revise<node>(inner_phi, inv_psi, psi_f, start_time);
		return rebuild_from_roles<node>(op_psi, r_inv, commit_psi);
	}
	if ((op_phi == temporal_op::R || op_phi == temporal_op::T)
	    && op_psi == temporal_op::ALWAYS) {
		auto [inv_phi, commit_phi] = decompose_roles<node>(phi);
		tref inner_psi = tau::get(psi)[0].first();
		tref r_inv = revise<node>(inv_phi, inner_psi, psi_f, start_time);
		tref candidate = rebuild_from_roles<node>(op_phi, r_inv, commit_phi);
		tref check = build_wff_and<node>(candidate, psi_f);
		if (is_tau_formula_sat<node>(check, start_time))
			return candidate;
		return tau::build_wff_always(r_inv);
	}

	// Case 5: F vs U/W — unwrap F(α) as commitment
	// F(α) = ⊤ U α.  If ψ is U-based, decompose and recurse.
	if (op_phi == temporal_op::F && (op_psi == temporal_op::U
	    || op_psi == temporal_op::W)) {
		tref inner_phi = tau::get(phi)[0].first();
		auto [inv_psi, commit_psi] = decompose_roles<node>(psi);
		tref r_commit = revise<node>(inner_phi, commit_psi, psi_f,
			start_time);
		return rebuild_from_roles<node>(op_psi, inv_psi, r_commit);
	}

	// Case 6: Operator mismatch — update wins
	return psi;
}

// ---------------------------------------------------------------------------
// And-distribution: distribute ∧ into invariant slots
//   (a∧b) U c → (a U c) ∧ (b U c)
//   a R (b∧c) → (a R b) ∧ (a R c)
// ---------------------------------------------------------------------------

template <NodeType node>
tref and_distribute(tref fm) {
	using tau = tree<node>;
	auto op = get_temporal_op<node>(fm);
	if (op == temporal_op::NONE) return fm;

	// Only distribute for binary operators with a conjunction in the
	// invariant slot.
	if (op == temporal_op::U || op == temporal_op::W
	    || op == temporal_op::S) {
		// Invariant is LHS.  Check if LHS is a conjunction.
		tref lhs = tau::get(fm)[0].first();
		tref rhs = tau::get(fm)[0].child(1);
		const auto& lhs_t = tau::get(lhs);
		if (lhs_t.has_child() && lhs_t[0].value.nt == tau::wff_and) {
			std::vector<tref> conjs;
			gather_top_conjuncts<node>(lhs, conjs);
			if (conjs.size() > 1) {
				tref result = nullptr;
				for (tref c : conjs) {
					tref distributed = rebuild_from_roles<node>(
						op, c, rhs);
					distributed = and_distribute<node>(distributed);
					result = result
						? build_wff_and<node>(result, distributed)
						: distributed;
				}
				return result;
			}
		}
	}
	if (op == temporal_op::R || op == temporal_op::T) {
		// Invariant is RHS.  Check if RHS is a conjunction.
		tref lhs = tau::get(fm)[0].first();
		tref rhs = tau::get(fm)[0].child(1);
		const auto& rhs_t = tau::get(rhs);
		if (rhs_t.has_child() && rhs_t[0].value.nt == tau::wff_and) {
			std::vector<tref> conjs;
			gather_top_conjuncts<node>(rhs, conjs);
			if (conjs.size() > 1) {
				tref result = nullptr;
				for (tref c : conjs) {
					tref distributed = rebuild_from_roles<node>(
						op, c, lhs);
					distributed = and_distribute<node>(distributed);
					result = result
						? build_wff_and<node>(result, distributed)
						: distributed;
				}
				return result;
			}
		}
	}

	// G(a∧b) → G(a) ∧ G(b)
	if (op == temporal_op::ALWAYS) {
		tref inner = tau::get(fm)[0].first();
		std::vector<tref> conjs;
		gather_top_conjuncts<node>(inner, conjs);
		if (conjs.size() > 1) {
			tref result = nullptr;
			for (tref c : conjs) {
				tref g = tau::build_wff_always(c);
				result = result
					? build_wff_and<node>(result, g)
					: g;
			}
			return result;
		}
	}

	return fm;
}

// ---------------------------------------------------------------------------
// Top-level pointwise revision: spec * update
//
// Steps 0-5 from pwr-ltl.tex § The pointwise revision algorithm.
// ---------------------------------------------------------------------------

template <NodeType node>
tref pointwise_revision_temporal(
	tref spec, tref update, const int_t start_time)
{
	using tau = tree<node>;

	// NOTE: We do NOT call normalizer<node>() here.
	// The interpreter (make_interpreter / step) has already normalized
	// the spec and the update before calling pointwise_revision.
	// Calling normalizer on an already-normalized safety formula crashes
	// in always_conjunction → build_wff_and (pre-existing codebase bug).

	// Trivial cases
	if (tau::get(update).equals_T()) return spec;
	if (tau::get(spec).equals_T())   return update;

	// Step 0: And-distribute into invariant slots, then split into clauses
	tref spec_d   = and_distribute<node>(spec);
	tref update_d = and_distribute<node>(update);

	std::vector<tref> spec_clauses;
	gather_top_conjuncts<node>(spec_d, spec_clauses);
	std::vector<tref> update_clauses;
	gather_top_conjuncts<node>(update_d, update_clauses);

	// Step 1: Global vacuity
	tref global_conj = build_wff_and<node>(spec, update);
	if (is_tau_formula_sat<node>(global_conj, start_time))
		return global_conj;

	// Step 2-3: Per-clause revision
	std::vector<tref> revised;
	revised.reserve(spec_clauses.size());

	for (tref sc : spec_clauses) {
		// Step 2: Per-clause vacuity
		tref clause_conj = build_wff_and<node>(sc, update);
		if (is_tau_formula_sat<node>(clause_conj, start_time)) {
			revised.push_back(sc);
			continue;
		}

		// Step 3: Recursive revision against best-matching update clause
		tref best = nullptr;
		for (tref uc : update_clauses) {
				// Step 4: Clause selection — try same-structure match first
			temporal_op op_sc = get_temporal_op<node>(sc);
			temporal_op op_uc = get_temporal_op<node>(uc);
			if (op_sc == op_uc && (is_binary_temporal(op_sc)
			    || op_sc != temporal_op::NONE)) {
				best = uc;
				break;
			}
		}
		if (!best && !update_clauses.empty())
			best = update_clauses[0];

		if (best) {
			tref r = revise<node>(sc, best, update, start_time);

			// Optimal mode fallback (pwr-ltl.tex §11): if fast mode
			// returned the update clause unchanged (dropped the spec
			// clause), try semantic winning-region revision.
			if (r == best || r == update) {
				tref opt = semantic_pwr_optimal<node>(
					sc, update, start_time);
				if (opt) {
					revised.push_back(opt);
					continue;
				}
			}

			revised.push_back(r);
		}
	}

	// Step 5: Assemble and verify
	// assembly = (∧ revised_clauses) ∧ update
	tref assembly = update;
	for (tref r : revised)
		assembly = build_wff_and<node>(assembly, r);

	if (is_tau_formula_sat<node>(assembly, start_time))
		return assembly;

	// Fallback: return update
	return update;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__POINTWISE_REVISION_H__
