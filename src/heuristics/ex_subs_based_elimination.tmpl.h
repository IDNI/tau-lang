// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "ex_subs_based_elimination.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "heuristics"

namespace idni::tau_lang {

template <NodeType node>
tref merge_and(const std::map<tref, tref>& subs, tref left, tref right) {
	// At least one side must have a substitution
	if (!subs.contains(left) && !subs.contains(right)) return nullptr;
	// We return one of the substitution at hand. This could be refined
	// in the future to allow more general cases. For example, we could
	// check for containment between both sides or a deeper equality test.
	// For example, if we are dealing with the case:
	//
	// o = i & j && o = i
	//
	// when eliminating 'o'.
	return subs.contains(left) ? subs.at(left) : subs.at(right);
}

template <NodeType node>
tref merge_or(const std::map<tref, tref>& subs, tref left, tref right) {
	// Both sides must have a substitution
	if (!subs.contains(left) || !subs.contains(right))
		return nullptr;
	// We check that both sides must have the same substitution, this could
	// be refined in the future to allow more general cases. For example,
	// we could check for containment between both sides or a deeper equality
	// test, for example if we are dealing with the case:
	//
	// o = i * { 2 }:bv || o = i << { 1 }:bv
	//
	// when eliminating 'o'.
	if (subs.at(left) != subs.at(right)) return nullptr;
	return subs.at(left);
}

template <NodeType node>
tref postorder(tref ex_clause, tref var) {
	using tau = tree<node>;

	std::map<tref, tref> subs;
	bool error = false;

	// We visit the formula until reaching atomic formulas (eq)
	auto visit_subtree = [&](tref n) -> bool {
		return !is<node>(n, tau::bf_interval)
			&& !is<node>(n, tau::bf_neq)
			&& !is<node>(n, tau::bf_lteq)
			&& !is<node>(n, tau::bf_nlteq)
			&& !is<node>(n, tau::bf_gt)
			&& !is<node>(n, tau::bf_ngt)
			&& !is<node>(n, tau::bf_gteq)
			&& !is<node>(n, tau::bf_ngteq)
			&& !is<node>(n, tau::bf_lt)
			&& !is<node>(n, tau::bf_nlt);
	};
	auto visit = [&](tref n) -> bool {
		using tau = tree<node>;

		// Get the node type
		auto t = tau::get(n);
		size_t nt = t.get_type();

		switch (nt) {
			// It's an atomic formula
			case tau::bf_eq: case tau::bf_neq: case tau::bf_lteq:
			case tau::bf_nlteq:	case tau::bf_gt: case tau::bf_ngt:
			case tau::bf_gteq: case tau::bf_ngteq: case tau::bf_lt:
			case tau::bf_nlt: {
				// This is a really simple case. In the future we could
				// extend this to support more complex substitutions calling
				// invert procedures (depending on the type).
				auto left = t[0][0].get();
				auto right = t[1][0].get();
				if (left == var) subs[n] = right;
				else if (right == var) subs[n] = left;
				break;
			}
			// It's a formula conjunction
			case tau::wff_and: {
				auto left = t[0][0].get();
				auto right = t[1][0].get();
				// If we have no substitutions we just continue, maybe later
				// we found a substitution.
				if (auto n_subs = merge_and<node>(subs, left, right); n_subs)
					subs[n] = n_subs;
				break;
			}
			// It's a formula disjunction
			case tau::wff_or: {
				auto left = t[0][0].get();
				auto right = t[1][0].get();
				// If we have no compatible we just continue, maybe later
				// we found a substitution.
				if (auto n_subs = merge_or<node>(subs, left, right); n_subs)
					subs[n] = n_subs;
				break;
			}
			default:
				// Do nothing, just continue.
				break;
		}
		return true;
	};

	post_order<node>(ex_clause).search_unique(visit, visit_subtree);

	// If we didn't find a suitable substitution we return the original clause.
	if (error) return ex_clause;
	// Otherwise, we apply the substitution into the clause and return it if
	// available.
	return subs.contains(ex_clause)	? subs[ex_clause] : nullptr;
}


template <NodeType node>
tref preorder(tref ex_clause, tref var) {
	using tau = tree<node>;

	tref found = nullptr;

	// We visit the formula until reaching atomic formulas (eq)
	auto visit_subtree = [&](tref n) -> bool {
		return !is<node>(n, tau::wff_or)
			&& !is<node>(n, tau::bf_interval)
			&& !is<node>(n, tau::bf_neq)
			&& !is<node>(n, tau::bf_lteq)
			&& !is<node>(n, tau::bf_nlteq)
			&& !is<node>(n, tau::bf_gt)
			&& !is<node>(n, tau::bf_ngt)
			&& !is<node>(n, tau::bf_gteq)
			&& !is<node>(n, tau::bf_ngteq)
			&& !is<node>(n, tau::bf_lt)
			&& !is<node>(n, tau::bf_nlt);
	};

	auto visit = [&](tref n) -> bool {
		using tau = tree<node>;

		// Get the node type
		auto t = tau::get(n);
		size_t nt = t.get_type();

		switch (nt) {
			// It's an atomic formula
			case tau::bf_eq: {
				// This is a really simple case. In the future we could
				// extend this to support more complex substitutions calling
				// invert procedures (depending on the type).
				auto left = t[0][0].get();
				auto right = t[1][0].get();
				if (left == var) found = right;
				else if (right == var) found = left;
				return false;
			}
			default:
				// Do nothing, just continue.
				break;
		}
		return true;
	};

	auto up = [&](tref) -> void {	return; };

	pre_order<node>(ex_clause).search_unique(visit, visit_subtree, up);
	return found;
}

template <NodeType node>
tref ex_subs_based_elimination(tref ex_clause, tref var)
{
	if (auto res = preorder<node>(ex_clause, var); res)
		return rewriter::replace<node>(ex_clause, var, res);
	else return ex_clause;
	//if (auto res = postorder<node>(ex_clause, var); res)
	//	return rewriter::replace<node>(ex_clause, var, res);
	//return ex_clause;
}

} // namespace idni::tau_lang