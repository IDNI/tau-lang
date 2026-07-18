// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

//#include "bv_predicate_blasting_predicates.tmpl.h"

namespace idni::tau_lang {

/**
 * @brief Extracts the arguments of a bitvector multiplication term.
 *
 * If one child is a bitvector constant, returns (non-constant, constant).
 * Otherwise, returns (nullptr, nullptr).
 *
 * @tparam node Node type
 * @param term The term to analyze
 * @return Pair of (non-constant, constant) or (nullptr, nullptr)
 *
 * @par Example
 * @code{.cpp}
 * // "x * { 6 }:bv[4]" -> (x, { 6 }:bv[4])
 * tref fm = get_nso_rr("x * { 6 }:bv[4] = 0.").value().main->get();
 * tref mul_term = tau::trim(tau::get(fm)[0].first());
 * auto [var, cst] = get_bvmul_arguments<node_t>(mul_term);
 * // tau::get(var).to_str() == "x", tau::get(cst).to_str() == "{ 6 }:bv[4]"
 * @endcode
 */
template <NodeType node>
static std::pair<tref, tref> get_bvmul_arguments(tref term) {
	using tau = tree<node>;

	auto left = tau::get(term).child(0);
	auto right = tau::get(term).child(1);

	auto tl = tau::trim(left), tr = tau::trim(right);
	if (tau::get(tl).is_ba_constant() && is_bv_constant<node>(tl)) {
		return std::make_pair(right, left);
	}
	if (tau::get(tr).is_ba_constant() && is_bv_constant<node>(tr)) {
		return std::make_pair(left, right);
	}

	// None of the factors is constant, we cannot blast this multiplication.
	return std::make_pair(nullptr, nullptr);
}

/**
 * @brief Extracts the arguments of a bitvector division/modulo or shift term.
 *
 * If the right child is a bitvector constant, returns (left, right).
 * Otherwise, returns (nullptr, nullptr).
 *
 * @tparam node Node type
 * @param term The term to analyze
 * @return Pair of (dividend, divisor) or (nullptr, nullptr)
 *
 * @par Example
 * @code{.cpp}
 * // "x / { 3 }:bv[4]" -> (x, { 3 }:bv[4])
 * tref fm = get_nso_rr("x / { 3 }:bv[4] = 0.").value().main->get();
 * tref div_term = tau::trim(tau::get(fm)[0].first());
 * auto [dividend, divisor] = get_arguments<node_t>(div_term);
 * // tau::get(dividend).to_str() == "x", tau::get(divisor).to_str() == "{ 3 }:bv[4]"
 * @endcode
 */
template <NodeType node>
static std::pair<tref, tref> get_arguments(tref term) {
	using tau = tree<node>;

	auto left = tau::get(term).child(0);
	auto right = tau::get(term).child(1);

	auto tr = tau::trim(right);
	if (tau::get(tr).is_ba_constant() && is_bv_constant<node>(tr)) {
		return std::make_pair(left, right);
	}

	// Divisor/modulo/count is not constant, we cannot blast this division/modulo/shift.
	return std::make_pair(nullptr, nullptr);
}

/**
 * @brief Existentially quantifies the auxiliary variables introduced during
 * blasting on top of subformula.
 *
 * Unlike build_wff_ex_many, the quantifier ids are chosen above any numeric
 * variable occurring in subformula -- bound or free. Operands may contain
 * variables bound by quantifiers outside the atomic being blasted (already
 * renamed to their canonical numeric form), and reusing such an id here
 * would capture them.
 *
 * @tparam node Node type
 * @param vars The auxiliary variables to quantify (innermost last)
 * @param subformula The formula to quantify over
 * @return The quantified formula
 *
 * @par Example
 * This is internal plumbing shared by every `*_predicate` function
 * (`eq_predicate`, `lt_predicate`, ...) and by `bvadd`/`bvsub`/`bvmul`/
 * `bvdiv`/`bvmod`/`bved` to close over the fresh carry/borrow/quotient-style
 * variables their constraints introduce. Conceptually: given `vars = [c0,
 * c1]` (two fresh carry variables) and a `subformula` whose highest existing
 * numeric variable id is, say, `5`, this wraps `subformula` in `ex c1 (ex c0
 * (subformula[c0 -> "6", c1 -> "7"]))`, then immediately eliminates both
 * quantifiers via `resolve_quantifiers2` (BDD-based, since blasted
 * arithmetic commonly introduces many disjuncts) rather than leaving them
 * in the result. See @ref bvadd's example: the two carry variables its
 * bit-recurrence introduces are exactly the kind of `vars` this function
 * closes over.
 */
template<NodeType node>
static tref quantify_aux_vars(const trefs& vars, tref subformula) {
	using tau = tree<node>;

	if (vars.empty()) return subformula;
	int_t id = find_biggest_quant_id<node>(subformula);
	auto is_number = [](const std::string& s) {
		if (s.empty()) return false;
		for (const unsigned char c : s)
			if (!std::isdigit(c)) return false;
		return true;
	};
	auto f = [&](tref n) {
		if (is<node, tau::variable>(n)) {
			if (const auto& name = get_var_name<node>(n);
				is_number(name))
				id = std::max(id, (int_t)std::stoi(name));
		}
		return true;
	};
	pre_order<node>(subformula).visit_unique(f);
	subtree_map<node, tref> changes;
	tref res = subformula;
	for (tref v : vars | std::views::reverse) {
		changes.emplace(v, tau::build_variable(std::to_string(++id),
			tau::get(v).get_ba_type()));
		res = tau::build_wff_ex(v, res, false);
	}
	res = rewriter::replace<node>(res, changes);
	// The aux variables just wrapped (carries/borrows/partial products/...)
	// retain a bv BA-type tag but are ordinary Boole-decomposable content --
	// always eliminate/push them here, regardless of nominal bv typing.
	// Scoped to exactly this local wrapping (not the whole formula being
	// blasted), so a still-unresolved, genuinely bv-typed quantifier
	// elsewhere (e.g. a user-level variable `x` still appearing in an
	// unblasted comparison atom like `x = 3`, since it has no arithmetic
	// to blast) is not caught up in it.
	//
	// Uses resolve_quantifiers2's BDD-based elimination directly, not
	// anti_prenex_block's generic Boole decomposition: blasted arithmetic
	// (e.g. XOR-expanded sum/carry constraints) commonly introduces
	// disjunctions and many atoms, and naive atom-by-atom Boole
	// decomposition scales combinatorially with atom count where BDD
	// elimination does not.
	typename term_handle<node>::order ord;
	for (size_t i = 0; i < vars.size(); ++i)
		ord.emplace(changes[vars[vars.size() - 1 - i]],
			static_cast<int_t>(i));
	return resolve_quantifiers2<node>(res, ord, [](tref) { return false; });
}

/**
 * @brief Performs predicate blasting for bitvector terms.
 *
 * Traverses the term, replacing supported bitvector operations with existentially
 * quantified variables and predicates.
 *
 * @tparam node Node type
 * @param term The term to blast
 * @param changes Map of subtree replacements
 * @param vars Vector to collect introduced variables
 * @return The blasted predicate, or nullptr on error
 *
 * @par Example
 * @code{.cpp}
 * // Called by every *_predicate function to resolve embedded arithmetic
 * // before the top-level comparison itself is handled; see @ref lt_predicate
 * // and @ref bvadd for the arithmetic this rewrites in practice.
 * tref fm = get_nso_rr(
 *     "ex x (x = { 3 }:bv[4] && x + { 5 }:bv[4] = { 8 }:bv[4]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
static std::pair<tref /* predicate */, tref /* transformed */> atomic_blasting(tref term, trefs& vars, subtree_map<node, tref>& changes) {
	using tau = tree<node>;

	tref predicate = nullptr;
	bool error = false;
	auto type_id = tau::get(term).get_ba_type();

	// Operands may have been replaced by fresh variables already (post-order
	// traversal blasts inner operations first), so resolve them through the
	// changes map, defaulting to the original subtree.
	auto lookup = [&changes](tref c) -> tref {
		auto it = changes.find(c);
		return it != changes.end() ? it->second : c;
	};

	// Conjoin a new constraint into the accumulated predicate; a nullptr
	// constraint (failed blasting) is an error.
	auto conjoin = [&](tref current) {
		if (!current) { error = true; return; }
		predicate = predicate
			? build_wff_and<node>(predicate, current)
			: current;
	};

	auto f = [&](tref t) {
		auto nt = tau::get(t).get_type();

		switch (nt) {
			case tau::bf_add: case tau::bf_sub: {
				auto result = build_variable<node>(type_id);
				auto bf_result = tau::get(tau::bf, result);
				auto left = lookup(tau::get(t).child(0));
				auto right = lookup(tau::get(t).child(1));
				vars.push_back(result);
				changes[t] = result;
				conjoin((nt == tau::bf_add)
					? bvadd<node>(left, right, bf_result, vars)
					: bvsub<node>(left, right, bf_result, vars));
				break;
			}
			case tau::bf_shl: case tau::bf_shr: {
				auto [shiftand_raw, count] = get_arguments<node>(t);
				if (!count) { error = true; break; }
				auto shiftand = lookup(shiftand_raw);
				auto shifted = tau::build_variable(type_id);
				auto bf_shifted = tau::get(tau::bf, shifted);
				vars.push_back(shifted);
				changes[t] = shifted;
				conjoin((nt == tau::bf_shl)
					? bvshl<node>(shiftand, count, bf_shifted)
					: bvshr<node>(shiftand, count, bf_shifted));
				break;
			}
			case tau::bf_mul: {
				auto [factor_raw, constant] = get_bvmul_arguments<node>(t);
				if (!constant) { error = true; break; }
				auto factor = lookup(factor_raw);
				auto product = tau::build_variable(type_id);
				auto bf_product = tau::get(tau::bf, product);
				vars.push_back(product);
				changes[t] = product;
				conjoin(bvmul<node>(factor, constant, bf_product, vars));
				break;
			}
			case tau::bf_div: case tau::bf_mod: {
				auto [dividend_raw, divisor] = get_arguments<node>(t);
				if (!divisor) { error = true; break; }
				auto dividend = lookup(dividend_raw);
				auto result = tau::build_variable(type_id);
				auto bf_result = tau::get(tau::bf, result);
				vars.push_back(result);
				changes[t] = result;
				conjoin((nt == tau::bf_mod)
					? bvmod<node>(dividend, divisor, bf_result, vars)
					: bvdiv<node>(dividend, divisor, bf_result, vars));
				break;
			}
			case tau::bf_cast: {
				auto child = tau::get(t).child(0);
				auto src = lookup(child);
				auto target_type_id = tau::get(t).get_ba_type();
				auto result = tau::build_variable(target_type_id);
				auto bf_result = tau::get(tau::bf, result);
				auto src_width = get_bv_type_bitwidth<node>(src);
				auto target_width = get_bv_type_bitwidth<node>(result);
				// Same-size cast: just substitute with the source.
				// src is a bf node while t (bf_cast) sits under a bf
				// wrapper, so strip the bf level to avoid bf { bf {..} }.
				if (src_width == target_width) {
					changes[t] = tau::trim(src);
					break;
				}
				// Different sizes: introduce variable and predicate
				vars.push_back(result);
				changes[t] = result;
				conjoin(bvcast<node>(src, bf_result));
				break;
			}
			// Nand, nor and xnor are treated in the hooks so we never get those
			// cases at this point.
			default: {

				break;
			}
		}
		// The visitor contract is: return true to continue the traversal,
		// false to abort it (we only abort on error).
		return !error;
	};

	post_order<node>(term).search_unique(f);
	// If we have an unsupported operation, we return nullptr to indicate failure
	if (error) return { nullptr, nullptr };
	// We reconstruct the original bf and wrap it
	auto modified = rewriter::replace<node>(term, changes);
	return { predicate, modified };
}

/**
 * @internal
 * @brief Blasts an atomic comparison whose top-level relation itself needs
 * no per-bit decomposition (equality/inequality), only its arithmetic
 * sub-terms do.
 *
 * Runs `atomic_blasting` to rewrite any embedded arithmetic (e.g. `x+1`)
 * into a fresh variable plus a conjoined constraint, then returns the
 * (possibly unchanged) comparison conjoined with that constraint,
 * existentially quantifying the fresh auxiliary variables. Shared plumbing
 * for `eq_predicate` and `neq_predicate`.
 * @tparam node Tree node type.
 * @param atomic The atomic `=`/`!=` comparison to blast.
 * @return The blasted predicate, or `nullptr` on error.
 *
 * @par Example
 * @code{.cpp}
 * // "x + { 1 }:bv[4] = { 5 }:bv[4]": the embedded addition is blasted into
 * // a fresh variable constrained by bvadd, conjoined with the equality.
 * tref fm = get_nso_rr(
 *     "x + { 1 }:bv[4] = { 5 }:bv[4].").value().main->get();
 * tref atomic = tau::trim(tau::get(fm)[0].get());
 * tref res = keep_comparison_predicate<node_t>(atomic);
 * CHECK( res != nullptr );
 * @endcode
 * @endinternal
 */
template<NodeType node>
static tref keep_comparison_predicate(tref atomic) {
	using tau = tree<node>;

	subtree_map<node, tref> changes;
	trefs vars;
	auto [predicate, blasted] = atomic_blasting<node>(atomic, vars, changes);
	if (!blasted) return nullptr;

	// If the atomic contains no blastable arithmetic, predicate is null and
	// the (unchanged) atomic itself is the result.
	auto wff_blasted = tau::get(tau::wff, blasted);
	return quantify_aux_vars<node>(vars, predicate
		? tau::build_wff_and(predicate, wff_blasted)
		: wff_blasted);
}

/**
 * @brief Blasts an equality predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic equality predicate
 * @return The resulting predicate term
 *
 * @par Example
 * @code{.cpp}
 * // Dispatched for "=" atoms by wff_predicate_blasting (see bf_eq case);
 * // exercised e.g. by "x + { 5 }:bv[4] = { 8 }:bv[4]" (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:70-72).
 * tref fm = get_nso_rr(
 *     "ex x (x = { 3 }:bv[4] && x + { 5 }:bv[4] = { 8 }:bv[4]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
static tref eq_predicate(tref atomic) {
	return keep_comparison_predicate<node>(atomic);
}

/**
 * @brief Blasts an inequality predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic inequality predicate
 * @return The resulting predicate term, or nullptr on error
 *
 * @par Example
 * @code{.cpp}
 * // Dispatched for "!=" atoms; x != x is never satisfiable (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:546-547).
 * tref fm = get_nso_rr("ex x x:bv[4] != x:bv[4].").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_F() );
 * @endcode
 */
template<NodeType node>
static tref neq_predicate(tref atomic) {
	return keep_comparison_predicate<node>(atomic);
}

/**
 * @brief Blasts a less-than predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic less-than predicate
 * @return The resulting predicate term, or nullptr on error
 *
 * @par Example
 * @code{.cpp}
 * // Dispatched for "<" atoms, delegating to bvlt after atomic_blasting
 * // resolves any embedded arithmetic (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:316-317).
 * tref fm = get_nso_rr(
 *     "ex x (x = { 2 }:bv[2] && x < { 3 }:bv[2]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
static tref lt_predicate(tref atomic) {
	using tau = tree<node>;

	subtree_map<node, tref> changes;
	trefs vars;

	auto [predicate, blasted] = atomic_blasting<node>(atomic, vars, changes);
	if (!blasted) return nullptr;

	auto left = tau::get(blasted).child(0);
	auto right = tau::get(blasted).child(1);
	auto applied = bvlt<node>(left, right);
	if (!applied) return nullptr;

	return quantify_aux_vars<node>(vars, predicate
		? tau::build_wff_and(predicate, applied)
		: applied);
}

/**
 * @brief Blasts a greater-than predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic greater-than predicate
 * @return The resulting predicate term, or nullptr on error
 *
 * @par Example
 * @code{.cpp}
 * // Dispatched for ">" atoms; x > x is never satisfiable (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:362-363).
 * tref fm = get_nso_rr("ex x x:bv[4] > x:bv[4].").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_F() );
 * @endcode
 */
template<NodeType node>
static tref gt_predicate(tref atomic) {
	using tau = tree<node>;

	subtree_map<node, tref> changes;
	trefs vars;

	auto [predicate, blasted] = atomic_blasting<node>(atomic, vars, changes);
	if (!blasted) return nullptr;

	auto left = tau::get(blasted).child(0);
	auto right = tau::get(blasted).child(1);
	auto applied = bvgt<node>(left, right);
	if (!applied) return nullptr;

	return quantify_aux_vars<node>(vars, predicate
		? tau::build_wff_and(predicate, applied)
		: applied);
}

/**
 * @brief Blasts a less-than-or-equal predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic less-than-or-equal predicate
 * @return The resulting predicate term
 *
 * @par Example
 * @code{.cpp}
 * // Dispatched for "<=" atoms: 2 <= 3 for 2-bit bitvectors (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:415-417).
 * tref fm = get_nso_rr(
 *     "ex x (x = { 2 }:bv[2] && x <= { 3 }:bv[2]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
static tref lteq_predicate(tref atomic) {
	using tau = tree<node>;

	auto inner = gt_predicate<node>(atomic);
	return inner ? tau::build_wff_neg(inner) : nullptr;
}

/**
 * @brief Blasts a greater-than-or-equal predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic greater-than-or-equal predicate
 * @return The resulting predicate term
 *
 * @par Example
 * @code{.cpp}
 * // Dispatched for ">=" atoms: all x, x >= x (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:389-391).
 * tref fm = get_nso_rr("all x x:bv[4] >= x:bv[4].").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
static tref gteq_predicate(tref atomic) {
	using tau = tree<node>;

	auto inner = lt_predicate<node>(atomic);
	return inner ? tau::build_wff_neg(inner) : nullptr;
}

/**
 * @brief Blasts a not-less-than predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic not-less-than predicate
 * @return The resulting predicate term
 *
 * @par Example
 * @code{.cpp}
 * // Dispatched for "!<" atoms: 2 !< 2 is T (equal case) (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:850-851).
 * tref fm = get_nso_rr(
 *     "ex x (x = { 2 }:bv[4] && x !< { 2 }:bv[4]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
static tref nlt_predicate(tref atomic) {
	using tau = tree<node>;

	auto inner = lt_predicate<node>(atomic);
	return inner ? tau::build_wff_neg(inner) : nullptr;
}

/**
 * @brief Blasts a not-greater-than predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic not-greater-than predicate
 * @return The resulting predicate term
 *
 * @par Example
 * @code{.cpp}
 * // Dispatched for "!>" atoms: 2 !> 2 is T (equal case) (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp,
 * // TEST_SUITE("bvngt"), "bvngt: 2 !> 2 is T (equal case)").
 * tref fm = get_nso_rr(
 *     "ex x (x = { 2 }:bv[4] && x !> { 2 }:bv[4]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
static tref ngt_predicate(tref atomic) {
	using tau = tree<node>;

	auto inner = gt_predicate<node>(atomic);
	return inner ? tau::build_wff_neg(inner) : nullptr;
}

/**
 * @brief Blasts a not-less-than-or-equal predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic not-less-than-or-equal predicate
 * @return The resulting predicate term
 *
 * @par Example
 * @code{.cpp}
 * // Dispatched for "!<=" atoms: 3 !<= 1 is T (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp:433-434).
 * tref fm = get_nso_rr(
 *     "ex x (x = { 3 }:bv[2] && x !<= { 1 }:bv[2]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
static tref nlteq_predicate(tref atomic) {
	auto applied = gt_predicate<node>(atomic);
	return applied;
}

/**
 * @brief Blasts a not-greater-than-or-equal predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic not-greater-than-or-equal predicate
 * @return The resulting predicate term
 *
 * @par Example
 * @code{.cpp}
 * // Dispatched for "!>=" atoms: 0 !>= 1 is T (see
 * // tests/integration/test_integration-heuristics-bv_predicate_blasting.cpp,
 * // TEST_SUITE("bvngteq"), "bvngteq: 0 !>= 1 is T").
 * tref fm = get_nso_rr(
 *     "ex x (x = { 0 }:bv[4] && x !>= { 1 }:bv[4]).").value().main->get();
 * tref blasted = bv_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
static tref ngteq_predicate(tref atomic) {
	auto applied = lt_predicate<node>(atomic);
	return applied;
}

/**
 * @brief Recursively blasts all bitvector formulas in a formula.
 *
 * Traverses the formula, replacing atomic bitvector predicates with their blasted forms.
 *
 * @tparam node Node type
 * @param term The formula to blast
 * @return The formula with predicates blasted, or nullptr on error
 *
 * @par Example
 * @code{.cpp}
 * // This is what bv_predicate_blasting (the public entry point) calls
 * // internally; see @ref bv_predicate_blasting for a worked example.
 * tref fm = get_nso_rr(
 *     "ex x (x = { 3 }:bv[4] && x + { 5 }:bv[4] = { 8 }:bv[4]).").value().main->get();
 * tref blasted = wff_predicate_blasting<node_t>(fm);
 * CHECK( tau::get(normalizer<node_t>(blasted)).equals_T() );
 * @endcode
 */
template<NodeType node>
static tref wff_predicate_blasting(tref term) {
	using tau = tree<node>;

	subtree_map<node, tref> changes;
	bool error = false;

	auto f = [&](tref t) {
		auto nt = tau::get(t).get_type();

		auto blast = [&](tref atomic, const auto& blaster) -> tref {
				auto blasted = blaster(atomic);
				if (!blasted) return error = true, t;
				// The atomic being replaced sits under a wff node and
				// the blasters return wff-level trees, so strip the
				// wff wrapper to avoid wff { wff { ... } } nestings
				// that defeat hook-based simplification.
				changes[t] = tau::trim(blasted);
				return blasted;
		};

		switch (nt) {
			case tau::bf_eq: { blast(t, eq_predicate<node>); break; }
			case tau::bf_neq: { blast(t, neq_predicate<node>); break; }
			case tau::bf_lt: { blast(t, lt_predicate<node>); break; }
			case tau::bf_gt: { blast(t, gt_predicate<node>); break; }
			case tau::bf_lteq: { blast(t, lteq_predicate<node>); break; }
			case tau::bf_gteq: { blast(t, gteq_predicate<node>); break; }
			case tau::bf_nlt: { blast(t, nlt_predicate<node>); break; }
			case tau::bf_ngt: { blast(t, ngt_predicate<node>); break; }
			case tau::bf_nlteq: { blast(t, nlteq_predicate<node>); break; }
			case tau::bf_ngteq: { blast(t, ngteq_predicate<node>); break; }
			case tau::bf_interval: {
				// a <= b <= c is equivalent to (a <= b) && (b <= c);
				// blast each comparison separately and conjoin the results.
				tref lo  = tau::get(t)[0].get();
				tref mid = tau::get(t)[1].get();
				tref hi  = tau::get(t)[2].get();
				tref lo_le_mid = tau::get(tau::bf_lteq, lo, mid);
				tref mid_le_hi = tau::get(tau::bf_lteq, mid, hi);
				tref left  = lteq_predicate<node>(lo_le_mid);
				tref right = lteq_predicate<node>(mid_le_hi);
				if (!left || !right) { error = true; break; }
				changes[t] = tau::trim(tau::build_wff_and(left, right));
				break;
			}
			default: {
				// just replace the children if needed, the current node will be
				// reconstructed in the post-order traversal
				trefs ch;
				for (tref c : tau::get(t).children()) {
					if (changes.find(c) != changes.end())
						ch.push_back(changes[c]);
					else ch.push_back(c);
				}

				if (auto new_t = tau::get(tau::get(t).value, ch.data(), ch.size()); new_t != t)
					changes[t] = new_t;
				break;
			}
		}
		// The visitor contract is: return true to continue the traversal,
		// false to abort it (we only abort on error).
		return !error;
	};

	post_order<node>(term).search_unique(f);

	if (error) {
		DBG(LOG_DEBUG << "Failed to compute predicate blasting in: " << LOG_FM(term);)
		return nullptr;
	}

	// quantify_aux_vars already anti-prenexes each blasted atomic's own
	// freshly-introduced auxiliary quantifiers (scoped locally, so a
	// still-unresolved genuinely bv-typed quantifier elsewhere in `term`
	// is left untouched); nothing further to do here.
	return changes.find(term) != changes.end() ? changes[term] : term;
}

/**
 * @brief Entry point for predicate blasting on bitvector formulas.
 *
 * Alias for wff_predicate_blasting.
 *
 * @tparam node Node type
 * @param term The formula to blast
 * @return The formula with predicates blasted, or nullptr on error
 */
template<NodeType node>
inline tref bv_predicate_blasting(tref term) {
	auto ret = wff_predicate_blasting<node>(term);
	return ret;
}

} // namespace idni::tau_lang