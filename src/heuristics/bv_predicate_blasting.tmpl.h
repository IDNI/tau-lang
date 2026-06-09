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
 * If the left child is a bitvector constant, returns (left, right).
 * Otherwise, returns (nullptr, nullptr).
 *
 * @tparam node Node type
 * @param term The term to analyze
 * @return Pair of (dividend, divisor) or (nullptr, nullptr)
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
 */
template<NodeType node>
static std::pair<tref /* predicate */, tref /* transformed */> atomic_blasting(tref term, trefs& vars, subtree_map<node, tref>& changes) {
	using tau = tree<node>;

	tref predicate = nullptr;
	bool error = false;
	auto type_id = tau::get(term).get_ba_type();

	auto f = [&](tref t) {
		auto nt = tau::get(t).get_type();

		switch (nt) {
			case tau::bf_add: case tau::bf_sub: {
				auto result = build_variable<node>(type_id);
				auto bf_result = tau::get(tau::bf, result);
				vars.push_back(result);
				changes[t] = result;
				auto left = changes[tau::get(t).child(0)];
				auto right = changes[tau::get(t).child(1)];
				auto current = (nt == tau::bf_add)
					? bvadd<node>(left, right, bf_result)
					: bvsub<node>(left, right, bf_result);
				predicate = predicate
					? build_wff_and<node>(predicate, current)
					: current;
				break;
			}
			case tau::bf_shl: case tau::bf_shr: {
				auto [shiftand, count] = get_arguments<node>(t);
				if (!count) { error = true; break; }
				auto shifted = tau::build_variable(type_id);
				auto bf_shifted = tau::get(tau::bf, shifted);
				vars.push_back(shifted);
				changes[t] = shifted;
				auto current = (nt == tau::bf_shl)
					? bvshl<node>(shiftand, count, bf_shifted)
					: bvrhl<node>(shiftand, count, bf_shifted);
				predicate = predicate
					? build_wff_and<node>(predicate, current)
					: current;
				break;
			}
			case tau::bf_mul: {
				auto [factor, constant] = get_bvmul_arguments<node>(t);
				if (!constant) { error = true; break; }
				auto product = tau::build_variable(type_id);
				auto bf_product = tau::get(tau::bf, product);
				vars.push_back(product);
				changes[t] = product;
				predicate = predicate
					? build_wff_and<node>(predicate, bvmul<node>(factor, constant, bf_product))
					: bvmul<node>(factor, constant, bf_product);
				break;
			}
			case tau::bf_div: case tau::bf_mod: {
				auto [dividend, divisor] = get_arguments<node>(t);
				if (!divisor) { error = true; break; }
				auto result = tau::build_variable(type_id);
				auto bf_result = tau::get(tau::bf, result);
				vars.push_back(result);
				changes[t] = result;
				auto current = (nt == tau::bf_mod)
					? bvmod<node>(dividend, divisor, bf_result)
					: bvdiv<node>(dividend, divisor, bf_result);
				predicate = predicate
					? build_wff_and<node>(predicate, current)
					: current;
				break;
			}
			case tau::bf_cast: {
				auto child = tau::get(t).child(0);
				auto src = (changes.find(child) != changes.end()) ? changes[child] : child;
				auto target_type_id = tau::get(t).get_ba_type();
				auto result = tau::build_variable(target_type_id);
				auto bf_result = tau::get(tau::bf, result);
				auto src_width = get_bv_type_bitwidth<node>(src);
				auto target_width = get_bv_type_bitwidth<node>(result);
				// Same-size cast: just substitute with source
				if (src_width == target_width) {
					changes[t] = src;
					break;
				}
				// Different sizes: introduce variable and predicate
				vars.push_back(result);
				changes[t] = result;
				auto current = bvcast<node>(src, bf_result);
				predicate = predicate
					? build_wff_and<node>(predicate, current)
					: current;
				break;
			}
			// Nand, nor and xnor are treated in the hooks so we never get those
			// cases at this point.
			default: {

				break;
			}
		}
		return error;
	};

	// If we have an unsupported operation, we return nullptr to indicate failure
	if (error) return { nullptr, nullptr };
	post_order<node>(term).search_unique(f);
	// We reconstruct the original bf and wrap it
	auto modified = rewriter::replace<node>(term, changes);
	return { predicate, modified };
}

template<NodeType node>
static tref keep_comparison_predicate(tref atomic) {
	using tau = tree<node>;

	subtree_map<node, tref> changes;
	trefs vars;
	auto [predicate, blasted] = atomic_blasting<node>(atomic, vars, changes);
	if (!blasted) return nullptr;

	predicate = tau::build_wff_ex_many(vars, tau::build_wff_and(predicate, tau::get(tau::wff, blasted)));

	return predicate;
}


/**
 * @brief Blasts an equality predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic equality predicate
 * @return The resulting predicate term
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
 */
template<NodeType node>
static tref lt_predicate(tref atomic) {
	using tau = tree<node>;

	subtree_map<node, tref> changes;
	trefs vars;

	auto bitwidth = get_bv_type_bitwidth<node>(atomic);
	auto rule = bvlt_rule<node>(bitwidth);
	auto [predicate, blasted] = atomic_blasting<node>(atomic, vars, changes);
	if (!blasted) return nullptr;

	auto left = tau::get(blasted).child(0);
	auto right = tau::get(blasted).child(1);
	auto call = make_bvlt_call_from_index<node>(left, right, bitwidth);
	auto applied = nso_rr_apply<node>(rule, call);
	predicate = tau::build_wff_ex_many(vars, tau::build_wff_and(predicate, applied));

	return predicate;
}

/**
 * @brief Blasts a greater-than predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic greater-than predicate
 * @return The resulting predicate term, or nullptr on error
 */
template<NodeType node>
static tref gt_predicate(tref atomic) {
	using tau = tree<node>;

	subtree_map<node, tref> changes;
	trefs vars;

	auto bitwidth = get_bv_type_bitwidth<node>(atomic);
	auto rule = bvgt_rule<node>(bitwidth);
	auto [predicate, blasted] = atomic_blasting<node>(atomic, vars, changes);
	if (!blasted) return nullptr;

	auto left = tau::get(blasted).child(0);
	auto right = tau::get(blasted).child(1);
	auto call = make_bvgt_call_from_index<node>(left, right, bitwidth);
	auto applied = nso_rr_apply<node>(rule, call);
	predicate = tau::build_wff_ex_many(vars, tau::build_wff_and(predicate, applied));

	return predicate;
}

/**
 * @brief Blasts a less-than-or-equal predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic less-than-or-equal predicate
 * @return The resulting predicate term
 */
template<NodeType node>
static tref lteq_predicate(tref atomic) {
	using tau = tree<node>;

	auto applied = tau::build_wff_neg(gt_predicate<node>(atomic));
	return applied;
}

/**
 * @brief Blasts a greater-than-or-equal predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic greater-than-or-equal predicate
 * @return The resulting predicate term
 */
template<NodeType node>
static tref gteq_predicate(tref atomic) {
	using tau = tree<node>;

	auto applied = tau::build_wff_neg(lt_predicate<node>(atomic));
	return applied;
}

/**
 * @brief Blasts a not-less-than predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic not-less-than predicate
 * @return The resulting predicate term
 */
template<NodeType node>
static tref nlt_predicate(tref atomic) {
	using tau = tree<node>;

	auto applied = tau::build_wff_neg(lt_predicate<node>(atomic));
	return applied;
}

/**
 * @brief Blasts a not-greater-than predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic not-greater-than predicate
 * @return The resulting predicate term
 */
template<NodeType node>
static tref ngt_predicate(tref atomic) {
	using tau = tree<node>;

	auto applied = tau::build_wff_neg(gt_predicate<node>(atomic));
	return applied;
}

/**
 * @brief Blasts a not-less-than-or-equal predicate over bitvectors.
 *
 * @tparam node Node type
 * @param atomic The atomic not-less-than-or-equal predicate
 * @return The resulting predicate term
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
				changes[t] = blasted;
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
				// TODO (MEDIUM) convert into two predicates and a conjunction,
				// but for now we just return an error.
				DBG(LOG_DEBUG << "Interval predicates are currently not supported in blasting.";)
				return error = true, false;
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
		return error;
	};

	post_order<node>(term).search_unique(f);

	if (error) {
		DBG(LOG_DEBUG << "Failed to compute predicate blasting in: " << LOG_FM(term);)
		return nullptr;
	}

	auto n_term = changes.find(term) != changes.end() ? changes[term] : term;
	return n_term;
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