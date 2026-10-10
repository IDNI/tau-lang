// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file bitvector.tmpl.h
 * @brief Template implementations for bitvector.h. bitvector.h says what the
 * router means; the comments here say how it is built.
 *
 * The freeze comes from conditions.h, the split from parts.h, the purity
 * test from terms.h, the finite method and the query closing from finite.h,
 * and the solver's verdict from `ask` (ask.h). What is written here is the
 * router's `opaque?` and the spec's step order with the joint ask.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__ELIMINATE__BITVECTOR_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__ELIMINATE__BITVECTOR_TMPL_H__

#include <algorithm>
#include <optional>

namespace idni::tau_lang::anti_prenexing {

namespace bitvector_detail {

/// Does @p hit hold of some node of @p n? One `find_top` walk, stopping at
/// the first; a BDD-backed term is read leaf by leaf, as `is_pure_term`
/// reads it, since what a stored BDD could hide sits in a leaf.
template <NodeType node, typename Hit>
bool holds_anywhere(tref n, const Hit& hit) {
	using tau = tree<node>;
	using tbdd = tau_term_bdd<node>;
	using th = term_handle<node>;
	auto in_leaf = [&hit](typename tbdd::ref x, bool leaf) {
		return !leaf || tau::get(tbdd::get_var_term(x)).find_top(hit)
			== nullptr;
	};
	auto at = [&hit, &in_leaf](tref m) {
		return hit(m) || (th::is_bdd_backed(m) && !tbdd::visit_nodes(
			th::convert_to_handle(m).get(), in_leaf));
	};
	return tau::get(n).find_top(at) != nullptr;
}

} // namespace bitvector_detail

// --- opaque? ----------------------------------------------------------------------

template <NodeType node>
bool bitvector_opaque(tref conjunct, ba_type_id type) {
	using tau = tree<node>;
	using namespace bitvector_detail;
	auto chain_or_reference = [](tref n) {
		return is_functional_quantifier<node>(n)
			|| tau::get(n).is(tau::ref);
	};
	if (is_child_temporal_quantifier<node>(conjunct)) return true;
	if (is_child_quantifier<node>(conjunct))
		return holds_anywhere<node>(conjunct, [&](tref n) {
			const ba_type_id t = tau::get(n).get_ba_type();
			return chain_or_reference(n) || (t != 0 && t != type);
		});
	// A reference makes a conjunct impure, so one walk finds either, and
	// purity tells a chain the finite side reads from one nobody does.
	return holds_anywhere<node>(conjunct, chain_or_reference)
		&& !is_pure_formula<node>(conjunct, type);
}

// --- ELIMINATE_BITVECTOR_CLAUSE ---------------------------------------------------

template <NodeType node>
tref eliminate_bitvector_clause(const trefs& conjuncts, const block& X,
	ctx<node>& c)
{
	using tau = tree<node>;
	using namespace bitvector_detail;
	// 1. FREEZE, by connected component, what neither engine reads.
	const freeze_result<node> fr = freeze_opaque_components<node>(conjuncts,
		X, [&c](tref m) { return bitvector_opaque<node>(m, c.type); });
	if (fr.X.empty()) return fr.frozen;
	// The slide folds every closed piece of a chain (§1), so a chain left
	// holds an impure leaf or sits in an impure conjunct or a unit: it froze
	// above, and neither the solver side nor the joint ask meets one.
	DBG(assert(std::ranges::none_of(fr.conjuncts, [](tref m) {
		return holds_anywhere<node>(m, is_functional_quantifier<node>);
	}));)
	// 3. §7 SPLIT_ARITHMETIC: the parts holding an impure conjunct are the
	// solver's side `A` over `X_A`, the rest the finite side `B` over `X_B`.
	const parts_partition<node> sp = partition_parts<node>(fr.conjuncts,
		fr.X, [&c](tref m) { return !is_pure_formula<node>(m, c.type); });
	// 4. The pure side first: its `F` decides before any solver query.
	const tref b = sp.rest.empty() ? _T<node>()
		: eliminate_finite_clause<node>(sp.rest, sp.rest_vars, c);
	if (tau::get(b).equals_F()) return _F<node>();
	// 5. §7 SOLVE_ARITHMETIC: `∃X_A.A` as one query, binder units entering
	// it as written.
	auto solver = [](tref q) -> std::optional<bool> {
		const answer r = ask<node>(q);
		if (r == answer::unknown) return std::nullopt;
		return r == answer::sat;
	};
	const tref a = sp.hit.empty() ? _T<node>()
		: query_or_rewrap<node>(sp.hit, sp.hit_vars, solver);
	// 6. THE JOINT ASK over two open sides that share a free variable.
	// `frozen` stays out: a sub-conjunction's unsatisfiability decides.
	if (is_child_quantifier<node>(a) && !tau::get(b).equals_T()
		&& fv_meets<node>(a, get_free_vars<node>(b)))
	{
		const tref ab = simplified_and_join<node>(trefs{ a, b });
		const trefs Z = get_free_vars<node>(ab);
		const tref j = term_handle<node>::convert_to_tau_terms(
			rewrap<node>(ab, Z));
		if (ask<node>(canonicalise_binder_ids<node>(j)) == answer::unsat)
			return _F<node>();
	}
	return simplified_and_join<node>(trefs{ fr.frozen, b, a });
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__ELIMINATE__BITVECTOR_TMPL_H__
