// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file atomless_decode.h
 * @brief The per-coordinate decode of a step whose constraints are all
 * disequalities over an atomless type, shared by the interpreter and the
 * table step provider.
 */

#ifndef __IDNI__TAU__ATOMLESS_DECODE_H__
#define __IDNI__TAU__ATOMLESS_DECODE_H__

#include <optional>
#include <vector>

#include "solver.h"

namespace idni::tau_lang {

/**
 * @brief True if `atom` is a bf_neq disequality over an atomless-typed BA.
 *
 * Structural only: the operands are not inspected, since the cofactor
 * elimination of solve_inequality_system_atomless does not need the
 * variable at the top (`o1[t] != o2[t]` folds to `o1[t] ^ o2[t] != 0`).
 * @param atom The atom to test.
 * @return Whether the atom has the shape the atomless decode takes.
 */
template <NodeType node>
bool atomless_disequality_shaped(tref atom) {
	using tau = tree<node>;
	const auto& t = tau::get(atom);
	if (!t.has_child()) return false;
	if (t[0].value.nt != tau::bf_neq) return false;
	return pack_type_is_atomless<node>(find_ba_type<node>(atom));
}

/**
 * @brief Decides the coordinates `coords` of grounded atomless
 * disequalities one at a time, in the order given.
 *
 * At its turn a coordinate gets every atom not yet used that names it
 * alone, after the coordinates decided before it are substituted; an atom
 * naming a second undecided coordinate waits for that coordinate's turn
 * (`o1 != o2` is solved when o2 is decided). A coordinate no atom names
 * alone takes the zero of its type. A free variable that is not in
 * `coords` (a lookback before the start of the run) stays free.
 *
 * The decided values are registered with `ledger` only once every
 * coordinate is decided, so a sibling coordinate of the same step still
 * sees them as external.
 * @param atoms The grounded atoms, each atomless_disequality_shaped.
 * @param coords The coordinates to decide, as bare variables.
 * @param ledger The run's fresh element ledger.
 * @return The values keyed by the bf-wrapped coordinates; nullopt when the
 * atoms are unsatisfiable for this history, or when an atom left over
 * still names a coordinate once every decided value is substituted.
 */
template <NodeType node>
result<std::optional<solution<node>>> atomless_decode_coordinates(
	const trefs& atoms, const trefs& coords, fresh_element_ledger& ledger)
{
	using tau = tree<node>;
	result<std::optional<solution<node>>> r;
	solution<node> sol;
	std::vector<bool> consumed(atoms.size(), false);
	for (tref var : coords) {
		inequality_system<node> sys;
		bool var_constrained = false;
		for (size_t i = 0; i < atoms.size(); ++i) {
			if (consumed[i]) continue;
			tref g = rewriter::replace<node>(atoms[i], sol);
			trefs fv = get_free_vars<node>(g);
			if (fv.empty()
				|| (fv.size() == 1 && tau::subtree_equals(fv[0], var)))
			{
				sys.insert(g);
				consumed[i] = true;
				if (!fv.empty()) var_constrained = true;
			}
		}
		solver_options opts;
		opts.type_id = find_ba_type<node>(var);
		opts.splitter_one = node::ba::splitter_one(
			get_ba_type_tree<node>(opts.type_id));
		opts.ledger = &ledger;
		TAU_TRY(auto val, solve_inequality_system_atomless<node>(sys, opts));
		// unsat for this history: a rejected candidate, not necessarily
		// a failure of the whole step
		if (!val) return r.with_value(std::nullopt);
		// the solver keys its values by bf-wrapped variables
		tref v = nullptr;
		tref sol_key = tau::get(tau::bf, var);
		for (const auto& [k, kv] : *val) {
			tref k_var = tau::get(k).child_is(tau::variable)
				? tau::get(k).first_tree().get() : k;
			if (tau::subtree_equals(k_var, var)) { v = kv; sol_key = k; break; }
		}
		// a constrained coordinate the solver left out would be
		// committed without its own constraint
		if (!v && var_constrained) return r.with_value(std::nullopt);
		if (!v) v = pack_zero_constant<node>(opts.type_id);
		if (!v) v = tau::_0(opts.type_id);
		sol[sol_key] = v;
	}
	// An atom left over may only name variables that are not decided here
	// (a lookback before the start of the run, free in every turn), which
	// leaves it satisfiable; one that still names a coordinate was never
	// checked against the value of that coordinate.
	for (size_t i = 0; i < atoms.size(); ++i) {
		if (consumed[i]) continue;
		tref g = rewriter::replace<node>(atoms[i], sol);
		for (tref v : get_free_vars<node>(g))
			for (tref c : coords)
				if (tau::subtree_equals(c, v))
					return r.with_value(std::nullopt);
	}
	for (const auto& [key, v] : sol)
		TAU_TRY_VOID(ledger_commit_witness<node>(ledger, v,
			tau::get(v).get_ba_type()));
	return r.with_value(std::move(sol));
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__ATOMLESS_DECODE_H__
