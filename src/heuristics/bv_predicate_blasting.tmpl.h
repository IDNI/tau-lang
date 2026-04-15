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

	if (is_bv_constant<node>(left)) return std::make_pair(right, left);
	if (is_bv_constant<node>(right)) return std::make_pair(left, right);

	return std::make_pair(nullptr, nullptr);
}

/**
 * @brief Extracts the arguments of a bitvector division/modulo term.
 *
 * If the left child is a bitvector constant, returns (left, right).
 * Otherwise, returns (nullptr, nullptr).
 *
 * @tparam node Node type
 * @param term The term to analyze
 * @return Pair of (dividend, divisor) or (nullptr, nullptr)
 */
template <NodeType node>
static std::pair<tref, tref> get_bved_arguments(tref term) {
	using tau = tree<node>;

	auto left = tau::get(term).child(0);
	auto right = tau::get(term).child(1);

	if (is_bv_constant<node>(left)) return std::make_pair(left, right);

	return std::make_pair(nullptr, nullptr);
}

/**
 * @brief Extracts the arguments of a bitvector shift term.
 *
 * If the left child is a bitvector constant, returns (left, right).
 * Otherwise, returns (nullptr, nullptr).
 *
 * @tparam node Node type
 * @param term The term to analyze
 * @return Pair of (shiftand, count) or (nullptr, nullptr)
 */
template <NodeType node>
static std::pair<tref, tref> get_bvsh_arguments(tref term) {
	using tau = tree<node>;

	auto left = tau::get(term).child(0);
	auto right = tau::get(term).child(1);

	if (is_bv_constant<node>(left)) return std::make_pair(left, right);

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
static tref bf_predicate_blasting(tref term, subtree_map<node, tref>& changes) {
	using tau = tree<node>;

	tref predicate;
	trefs vars;
	bool error = false;
	auto type_id = tau::get(term).get_ba_type();

	auto f = [&](tref t) {
		auto nt = tau::get(t).get_type();

		switch (nt) {
			case tau::bf_add: case tau::bf_sub: {
				auto var = build_variable<node>(type_id);
				vars.push_back(var);
				changes[t] = var;
				auto left = changes[tau::get(t).child(0)];
				auto right = changes[tau::get(t).child(1)];
				auto current = (nt == tau::bf_add)
					? bvadd<node>(left, right, var)
					: bvsub<node>(left, right, var);
				predicate = predicate
					? build_bf_and<node>(predicate, current)
					: current;
				break;
			}
			case tau::bf_shl: case tau::bf_shr: {
				auto [shiftand, count] = get_bvsh_arguments<node>(t);
				if (!count) break;
				auto result = tau::build_bf_variable(type_id);
				vars.push_back(result);
				changes[t] = result;
				auto current = (nt == tau::bf_shl)
					? bvshl<node>(shiftand, count, result)
					: bvrhl<node>(shiftand, count, result);
				predicate = predicate
					? build_bf_and<node>(predicate, current)
					: current;
				break;
			}
			case tau::bf_mul: {
				auto [factor, constant] = get_bvmul_arguments<node>(t);
				if (!constant) break;
				auto product = tau::build_bf_variable(type_id);
				vars.push_back(product);
				changes[t] = product;
				predicate = predicate
					? build_bf_and<node>(predicate, bvmul<node>(factor, constant, product))
					: bvmul<node>(factor, constant, product);
				break;
			}
			case tau::bf_div: case tau::bf_mod: {
				auto [dividend, divisor] = get_bved_arguments<node>(t);
				if (!divisor) break;
				auto result = tau::build_bf_variable(type_id);
				vars.push_back(result);
				changes[t] = result;
				auto current = (nt == tau::bf_mod)
					? bvmod<node>(dividend, divisor, result)
					: bvdiv<node>(dividend, divisor, result);
				predicate = predicate
					? build_bf_and<node>(predicate, current)
					: current;
				break;
			}
			// Nand, nor and xnor are treated in the hooks so we never get those
			// cases at this point.
			default: {
				trefs ch;
				for (tref c : tau::get(t).children()) {
					if (changes.find(c) != changes.end())
						ch.push_back(changes[c]);
					else ch.push_back(c);
				}

				if (auto new_n = tau::get(tau::get(t).value, ch.data(), ch.size()); new_n != t)
					changes[t] = new_n;
				break;
			}
		}
		return true;
	};

	// If we have an unsupported operation, we return nullptr to indicate failure
	if (error) return nullptr;
	post_order<node>(term).search_unique(f);
	auto modified = rewriter::replace<node>(term, changes);
	// We add the existentials for the variables we introduced
	for (auto it = vars.rbegin(); it != vars.rend(); ++it) {
		predicate = build_wff_ex<node>(*it, predicate);
	}

	return (predicate)
		? tau::build_wff_and(predicate, modified)
		: term;
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
	// TODO (HIGH) add simplifications to avoid the top level variable if possible
	subtree_map<node, tref> changes;
	return bf_predicate_blasting<node>(atomic, changes);
}

/**
 * @brief Blasts an inequality predicate over bitvectors.
 *
 * @tparam node Node type
 * @param n The atomic inequality predicate
 * @return The resulting predicate term
 */
template<NodeType node>
static tref neq_predicate(tref n) {
	using tau = tree<node>;

	return tau::build_wff_neg(eq_predicate<node>(n));
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

	auto bitwidth = get_bv_type_bitwidth<node>(atomic);
	auto predicate = bvlt_rule<node>(bitwidth);
	auto blasted = bf_predicate_blasting<node>(atomic, changes);
	if (!blasted) return nullptr;
	auto left = tau::get(blasted).child(0);
	auto right = tau::get(blasted).child(1);
	auto call = make_bvlt_call_from_index<node>(left, right, bitwidth);
	return nso_rr_apply<node>(predicate, call);
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

	auto bitwidth = get_bv_type_bitwidth<node>(atomic);
	auto predicate = bvgt_rule<node>(bitwidth);
	auto blasted = bf_predicate_blasting<node>(atomic, changes);
	if (!blasted) return nullptr;
	auto left = tau::get(blasted).child(0);
	auto right = tau::get(blasted).child(1);
	auto call = make_bvgt_call_from_index<node>(left, right, bitwidth);
	return nso_rr_apply<node>(predicate, call);
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

	return tau::build_wff_neg(gt_predicate<node>(atomic));
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

	return tau::build_wff_neg(lt_predicate<node>(atomic));
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
	return gteq_predicate<node>(atomic);
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
	return lteq_predicate<node>(atomic);
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
	return gt_predicate<node>(atomic);
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
	return lt_predicate<node>(atomic);
}

/**
 * @brief Blasts atomic bitvector predicates into equivalent formulas.
 *
 * Only operates on atomic predicates over bitvector terms. Returns nullptr for
 * unsupported cases.
 *
 * @tparam node Node type
 * @param atomic The atomic predicate to blast
 * @return The blasted predicate, or nullptr on error
 */
template<NodeType node>
static tref atomic_predicate_blasting(tref atomic) {
	using tau = tree<node>;

	// We only blast atomic predicates over bitvector terms. If the atomic formula
	// is not over bitvectors, we return it unchanged.
	auto type = tau::get(atomic).get_ba_type();
	if(!is_bv_type_family<node>(type)) return atomic;

	auto nt = tau::get(atomic).get_type();
	subtree_map<node, tref> changes;
	tref predicate;

	switch (nt) {
		case tau::bf_eq: { predicate = eq_predicate<node>(atomic); break; }
		case tau::bf_neq: { predicate = neq_predicate<node>(atomic); break; }
		case tau::bf_lt: { predicate = lt_predicate<node>(atomic); break; }
		case tau::bf_gt: { predicate = gt_predicate<node>(atomic); break; }
		case tau::bf_lteq: { predicate = lteq_predicate<node>(atomic); break; }
		case tau::bf_gteq: { predicate = gteq_predicate<node>(atomic); break; }
		case tau::bf_nlt: { predicate = nlt_predicate<node>(atomic); break; }
		case tau::bf_ngt: { predicate = ngt_predicate<node>(atomic); break; }
		case tau::bf_nlteq: { predicate = nlteq_predicate<node>(atomic); break; }
		case tau::bf_ngteq: { predicate = ngteq_predicate<node>(atomic); break; }
		default: {
			// error, unknown atomic predicate, we return nullptr to indicate failure
			LOG_ERROR << "Unknown atomic predicate in blasting: " << LOG_NT(nt) << "\n";
			return nullptr;
		}
	}

	if (!predicate) {
#ifdef DEBUG
		LOG_DEBUG << "Failed to compute predicate in blasting: "
			<< LOG_FM(atomic) << ". It will be left unchanged.";
#endif // DEBUG
		return nullptr;
	}

	return predicate;
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

		switch (nt) {
			case tau::bf_lt: case tau::bf_gt: case tau::bf_lteq: case tau::bf_gteq:
			case tau::bf_nlt: case tau::bf_ngt: case tau::bf_nlteq: case tau::bf_ngteq:
			case tau::bf_eq: case tau::bf_neq: {
				auto blasted = atomic_predicate_blasting<node>(t);
				if(!blasted) return error = true, false;
				changes[t] = blasted;
				break;
			}
			case tau::bf_interval: {
				// TODO (MEDIUM) convert into two predicates and a conjunction,
				// but for now we just return an error.
				DBG( LOG_DEBUG << "Interval predicates are currently not supported in blasting."; )
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
		return true;
	};

	post_order<node>(term).search_unique(f);

	if (error) return nullptr;

	auto n_term = changes.find(term) != changes.end() ? changes[term] : term;
	DBG(LOG_TRACE << "bv_predicate_blasting/wff/n_term: " << LOG_FM(n_term);)

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
	return wff_predicate_blasting<node>(term);
}

} // namespace idni::tau_lang