// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

namespace idni::tau_lang {

template <NodeType node>
std::pair<tref, tref> get_bvmul_arguments(tref term) {
	using tau = tree<node>;

	auto left = tau::get(term).child(0);
	auto right = tau::get(term).child(1);

	if (is_bv_constant<node>(left)) return std::make_pair(right, left);
	if (is_bv_constant<node>(right)) return std::make_pair(left, right);

	return std::make_pair(nullptr, nullptr);
}

template <NodeType node>
std::pair<tref, tref> get_bved_arguments(tref term) {
	using tau = tree<node>;

	auto left = tau::get(term).child(0);
	auto right = tau::get(term).child(1);

	if (is_bv_constant<node>(left)) return std::make_pair(left, right);

	return std::make_pair(nullptr, nullptr);
}

template <NodeType node>
std::pair<tref, tref> get_bvsh_arguments(tref term) {
	using tau = tree<node>;

	auto left = tau::get(term).child(0);
	auto right = tau::get(term).child(1);

	if (is_bv_constant<node>(left)) return std::make_pair(left, right);

	return std::make_pair(nullptr, nullptr);
}

template<NodeType node>
tref bf_predicate_blasting(tref term, subtree_map<node, tref>& changes, trefs& vars) {
	using tau = tree<node>;

	tref predicate;
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


template<NodeType node>
tref atomic_predicate_blasting(tref atomic) {
	using tau = tree<node>;

	// We only blast atomic predicates over bitvector terms. If the atomic formula
	// is not over bitvectors, we return it unchanged.
	auto type = tau::get(atomic).get_ba_type();
	if(!is_bv_type_family<node>(type)) return atomic;

	auto nt = tau::get(atomic).get_type();
	subtree_map<node, tref> changes;
	trefs vars;
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
			DBG( LOG_DEBUG << "Unknown atomic predicate in blasting: " << LOG_NT(nt) << ". It will be left unchanged."; )
			return nullptr;
		}
	}

	if (!predicate) {
		DBG( LOG_DEBUG << "Failed to compute predicate in blasting: " << LOG_FM(atomic) << ". It will be left unchanged."; )
		return nullptr;
	}

	return predicate;
}

template<NodeType node>
tref wff_predicate_blasting(tref term) {
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
			default: {
				trefs ch;
				for (tref c : tau::get(t).children()) {
					if (changes.find(c) != changes.end())
						ch.push_back(changes[c]);
					else ch.push_back(c);
				}

				if (auto new_n = tau::get(tau::get(t).value, ch.data(), ch.size()); new_n != t)
					changes.insert_or_assign(t, new_n);
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

template<NodeType node>
tref bv_predicate_blasting(tref term) {
	return wff_predicate_blasting<node>(term);
}

} // namespace idni::tau_lang