// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_bitblasting"

namespace idni::tau_lang {

using bool_node = idni::tau_lang::node<cvc5::Term, Bool>;
using bool_tree = tree<bool_node>;
static auto bool_id = bool_type_id<bool_node>();

// This is a factory for creating the rules needed for each bitvector operation.
// We also include factory methods for creating the calls to easy the creation
// of the predicates.
template<NodeType node>
struct bv_bitblasting_rules {

	static rewriter::rules bvadd(size_t bitwidth);
	static rewriter::rules bvsub(size_t bitwidth);
	static rewriter::rules bvshl_by_one(size_t bitwidth);
	static rewriter::rules bvrhl_by_one(size_t bitwidth);
	static rewriter::rules bvmul(size_t bitwidth);
	static rewriter::rules bvdiv(size_t bitwidth);
	static rewriter::rules bvmod(size_t bitwidth);
	static rewriter::rules bvshl(size_t bitwidth);
	static rewriter::rules bvrhl(size_t bitwidth);
	static rewriter::rules bit(size_t bitwidth, size_t bit);

	static tref make_bvadd_call(size_t bitwidth);
	static tref make_bvsub_call(size_t bitwidth);
	static tref make_bvshl_by_one_call(size_t bitwidth);
	static tref make_bvrhl_by_one_call(size_t bitwidth);
	static tref make_bvmul_call(size_t bitwidth);
	static tref make_bvdiv_call(size_t bitwidth);
	static tref make_bvmod_call(size_t bitwidth);
	static tref make_bvshl_call(size_t bitwidth);
	static tref make_bvrhl_call(size_t bitwidth);
	static tref make_bit_call(size_t bitwidth, size_t bit);
};

// This is a factory for creating the predicates needed for each bitvector operation.
// We also include factory methods for creating the calls to easy the creation of
// calls. We aplly the above rules to create the final predicates
// for the bitvector operations.
struct bv_bitblasting_predicates {

	static rewriter::rule bvadd_predicate(size_t bitwidth);
	static rewriter::rule bvsub_predicate(size_t bitwidth);
	static rewriter::rule bvshl_by_one_predicate(size_t bitwidth);
	static rewriter::rule bvmul_predicate(size_t bitwidth);
	static rewriter::rule bvdiv_predicate(size_t bitwidth);
	static rewriter::rule bvmod_predicate(size_t bitwidth);
	static rewriter::rule bvshl_predicate(size_t bitwidth);
	static rewriter::rule bvrhl_predicate(size_t bitwidth);

};


template<NodeType node>
tref bvadd_predicate([[maybe_unused]] tref left, [[maybe_unused]] tref right, [[maybe_unused]] tref var) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref bvmul_predicate([[maybe_unused]] tref left, [[maybe_unused]] tref right, [[maybe_unused]] tref var) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref bvsub_predicate([[maybe_unused]] tref left, [[maybe_unused]] tref right, [[maybe_unused]] tref var) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref bvdiv_predicate([[maybe_unused]] tref left, [[maybe_unused]] tref right, [[maybe_unused]] tref var) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref bvrhl_one_predicate([[maybe_unused]] tref left, [[maybe_unused]] tref var) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref bvshl_one_predicate([[maybe_unused]] tref left, [[maybe_unused]] tref var) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
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
					? bvadd_predicate<node>(left, right, var)
					: bvsub_predicate<node>(left, right, var);
				predicate = predicate
					? build_bf_and<node>(predicate, current)
					: current;
				break;
			}
			case tau::bf_shl: case tau::bf_shr: {
				// TODO (HIGH) check for 1 in the by one shift
				// operations and for general case when we support it.
				auto var = build_bf_var<node>(type_id);
				vars.push_back(var);
				auto left = tau::get(t).child(0);
				changes[t] = var;
				auto current = (nt == tau::bf_shl)
					? bvshl_one_predicate<node>(left, var)
					: bvrhl_one_predicate<node>(left, var);
				predicate = predicate
					? build_bf_and<node>(predicate, current)
					: current;
				break;
			}
			case tau::bf_mul: case tau::bf_div: case tau::bf_mod: {
				// Unsupported operation for now
				LOG_ERROR << "Operation "
					<< LOG_NT(nt) << " not supported yet in predicate blasting.";
				return error = true, false;
			}
			case tau::bf_nand: case tau::bf_xnor: case tau::bf_nor:{
				// Unsupported operation for now. They must be replaced by their
				// equivalent in terms of and, or and not for blasting.
				LOG_ERROR << "Operation "
					<< LOG_NT(nt) << " not supported yet in predicate blasting.";
				return error = true, false;
			}
			default: {
				break;
			}
		}
		return true;
	};

	// if we have an unsupported operation, i.e. we have an unsupported operation
	//we return nullptr to indicate failure
	if (error) return nullptr;
	return predicate;
}


template<NodeType node>
tref eq_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref neq_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref lt_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref gt_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref lteq_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref gteq_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref nlt_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref ngt_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref nlteq_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref ngteq_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref interval_predicate([[maybe_unused]] tref n, [[maybe_unused]] trefs& vars) {
	// Unsupported operation for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref atomic_predicate_blasting(tref atomic) {

	auto type_id = tau::get(atomic).get_ba_type();
	if(!is_bv_type_family<node>(type_id))	return atomic;

	auto atomic_nt = tau::get(atomic)[0].get_type();
	tref predicate;
	trefs vars;

	tref current;
	switch (atomic_nt) {
		case tau::bf_eq: { current = eq_predicate<node>(atomic, vars); break; }
		case tau::bf_neq: { current = neq_predicate<node>(atomic, vars); break; }
		case tau::bf_lt: { current = lt_predicate<node>(atomic, vars); break; }
		case tau::bf_gt: { current = gt_predicate<node>(atomic, vars); break; }
		case tau::bf_lteq: { current = lteq_predicate<node>(atomic, vars); break; }
		case tau::bf_gteq: { current = gteq_predicate<node>(atomic, vars); break; }
		case tau::bf_nlt: { current = nlt_predicate<node>(atomic, vars); break; }
		case tau::bf_ngt: { current = ngt_predicate<node>(atomic, vars); break; }
		case tau::bf_nlteq: { current = nlteq_predicate<node>(atomic, vars); break; }
		case tau::bf_ngteq: { current = ngteq_predicate<node>(atomic, vars); break; }
		case tau::bf_interval: { current = interval_predicate<node>(atomic, vars); break; }
		default: {
			// error, unknown atomic predicate, we return nullptr to indicate failure
			LOG_ERROR << "Unknown atomic predicate: " << LOG_NT(atomic_nt);
			return nullptr;
		}
	}

	predicate = predicate
		? build_bf_and<node>(predicate, current)
		: current;

	// we add the existentials for the variables we introduced
	for (auto it = vars.rbegin(); it != vars.rend(); ++it) {
		predicate = build_wff_ex<node>(*it, predicate);
	}

	return predicate;
}

template<NodeType node>
tref wff_predicate_blasting(tref term) {
	using tau = tree<node>;

	subtree_map<node, tref> changes;

	auto f = [&](tref t) {
		auto nt = tau::get(t).get_type();

		switch (nt) {
			case tau::bf_interval:
			case tau::bf_lt: case tau::bf_gt: case tau::bf_lteq: case tau::bf_gteq:
			case tau::bf_nlt: case tau::bf_ngt: case tau::bf_nlteq: case tau::bf_ngteq:
			case tau::bf_eq: case tau::bf_neq: {
				changes[t] = atomic_predicate_blasting<node>(t);
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

	auto n_term = changes.find(term) != changes.end() ? changes[term] : term;
	DBG(LOG_TRACE << "bv_predicate_blasting/wff/n_term: " << LOG_FM(n_term);)

	return n_term;
}



} // namespace idni::tau_lang