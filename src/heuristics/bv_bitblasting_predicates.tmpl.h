// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_bitblasting"

namespace idni::tau_lang {

using bool_node = idni::tau_lang::node<cvc5::Term, Bool>;
using bool_tree = tree<bool_node>;
static auto bool_id = bool_type_id<bool_node>();


/*using bool_node = idni::tau_lang::node<cvc5::Term, Bool>;
using bool_tree = tree<bool_node>;
using bitblast_solution = solution<bool_node>;
static auto bool_id = bool_type_id<bool_node>();

template<NodeType node>
tref basic_bitblast(const typename tree<node>::traverser& term) {
	using tau = tree<node>;
	using tt = tau::traverser;

	auto t = tau::get(tt(term) | tt::ref);
	bool_node new_node{t.value.nt, t.value.data, t.value.term, t.value.ba_type, t.value.ext};
	return bool_tree::get(new_node, t.get_children());
}

// bf_bitblast returns a vector of bits representing the bitblasted term, where
// the i-th element of the vector corresponds to the i-th bit of the original
// term (0-indexed from the most significant bit).
template<NodeType node>
std::optional<trefs> bf_bitblast(const typename tree<node>::traverser& term) {
	using tau = tree<node>;
	using tt = tau::traverser;

	static auto one = bool_tree::get_typed(bool_tree::bf_t, bool_id);
	static auto zero = bool_tree::get_typed(bool_tree::bf_f, bool_id);

	auto nt = term | tt::nt;
	auto ref = term | tt::ref;
	auto n = tau::get(ref);
	auto type = n.get_ba_type();
	auto bv_width = get_bv_width<node>(type);

	switch (nt) {
		case tau::bf_add: case tau::bf_sub: case tau::bf_mul:
		case tau::bf_div: case tau::bf_mod: case tau::bf_shl:
		case tau::bf_shr: case tau::bf_nand: case tau::bf_xnor:
		case tau::bf_nor:{
			// Unsupported operations for now
			LOG_ERROR << "Arithmetic operations are not supported in bitblasting yet.";
			return std::nullopt;
		}
		case tau::bf: {
			auto bits = bf_bitblast<node>(term | tt::first);
			if (!bits) return std::nullopt;
			trefs result;
			for (size_t i = 0; i < bv_width; ++i)
				result.push_back(bool_tree::get(bool_tree::bf, bits.value()[i]));
			return result;
		}
		case tau::bf_neg: {
			auto bits = bf_bitblast<node>(term | tt::first);
			if (!bits) return std::nullopt;
			trefs result;
			for (size_t i = 0; i < bv_width; ++i) {
				result.push_back(bool_tree::get(bool_tree::bf_neg, bits.value()[i]));
			}
			return result;
		}
		case tau::bf_and: case tau::bf_or: case tau::bf_xor: {
			auto left_bits = bf_bitblast<node>(term | tt::first);
			auto right_bits = bf_bitblast<node>(term | tt::second);
			if (!left_bits || !right_bits) return std::nullopt;
			trefs result;
			for (size_t i = 0; i < bv_width; ++i)
				result.push_back(bool_tree::get(nt, left_bits.value()[i], right_bits.value()[i]));
			return result;
		}
		case tau::bf_t:	case tau::bf_f: {
			trefs result;
			for (size_t i = 0; i < bv_width; ++i)
				result.push_back(nt == bool_tree::bf_t ? one : zero);
			return result;
			}
		case tau::ba_constant: {
			auto cte = term | tt::ba_constant;
			DBG(assert(std::holds_alternative<bv>(cte));)
			auto str = std::get<bv>(cte).getBitVectorValue(2);
			trefs result;
			for (size_t i = 0; i < bv_width; ++i)
				result.push_back(str[i] == '1' ? one : zero);
			return result;
		}
		case tau::variable: case tau::ctnvar: {
			auto booled = basic_bitblast<node>(term);
			auto booled_node = bool_tree::get(booled);
			auto bit = bool_tree::get(booled_node.value.ba_retype(bool_id), booled_node.get_children());
			trefs result;
			for (size_t i = 0; i < bv_width; ++i) {
				result.push_back(bool_tree::add_child(bit,
					bool_tree::get(bool_tree::offset, bool_tree::get_num(i))));
			}
			return result;
		}
		default:
			// Unsupported operations for now
			LOG_ERROR << "Unsupported operation in bitblasting: " << LOG_FM(tt(term)|| tt::ref);
			return std::nullopt;
	}
}

template<NodeType node>
tref wff_bitblast(const typename tree<node>::traverser& term) {
	using tau = tree<node>;
	using tt = tau::traverser;

	static auto one = bool_tree::get_typed(bool_tree::bf_t, bool_id);

	auto nt = term | tt::nt;
	auto ref = term | tt::ref;
	auto n = tau::get(ref);

	switch (nt) {
		case tau::wff:
		return bool_tree::get(bool_tree::wff, wff_bitblast<node>(term | tt::first));
		case tau::wff_sometimes: case tau::wff_always:
		case tau::wff_all: case tau::wff_ex:
		case tau::wff_neg: case tau::wff_or: case tau::wff_conditional:
		case tau::wff_imply: case tau::wff_rimply: case tau::wff_equiv:
		case tau::wff_xor: case tau::wff_and: case tau::bf_interval:
		case tau::bf_lt: case tau::bf_gt: case tau::bf_lteq: case tau::bf_gteq:
		case tau::bf_nlt: case tau::bf_ngt: case tau::bf_nlteq: case tau::bf_ngteq: {
			// Unsupported operations for now
			LOG_ERROR << "Unsupported operation in wff bitblasting: " << LOG_NT(nt) << " " << LOG_FM(tt(term)|| tt::ref);
			return nullptr;
		}
		case tau::bf_eq: case tau::bf_neq: {
			auto type = n.get_ba_type();
			auto bv_width = get_bv_width<node>(type);
			auto left = bf_bitblast<node>(term | tt::first);
			auto right = bf_bitblast<node>(term | tt::second);
			if (!left || !right) return nullptr;
			tref result = bool_tree::_0(bool_id);
			for (size_t i = 0; i < bv_width; ++i) {
				result = build_bf_or<bool_node>(
					result,
					build_bf_xor<bool_node>(left.value()[i], right.value()[i])
				);
			}
			return (nt == tau::bf_eq)
				? build_bf_eq_0<bool_node>(result)
				: build_bf_eq_0<bool_node>(build_bf_xor<bool_node>(result, one));
		}
		case tau::wff_t: case tau::wff_f: {
			return ref;
		}
		default: {
			// Unsupported operations for now
			LOG_ERROR << "Unsupported operation in wff bitblasting: " << LOG_NT(nt) << " " << LOG_FM(tt(term)|| tt::ref);
			return nullptr;
		}
	}
}

template<NodeType node>
std::optional<tref> bitblast(tref term) {
	using tau = tree<node>;
	using tt = tau::traverser;

	return wff_bitblast<node>(tt(term));
}

template<NodeType node>
std::optional<solution<node>> unbitblast([[maybe_unused]] bitblast_solution& solution) {
	return std::nullopt;
}

template<NodeType node>
std::optional<solution<node>> bv_bitblasting_solve(tref term) {
	auto bitblasted = bitblast<node>(term);
	if (!bitblasted) {
		LOG_ERROR << "Bitblasting failed for term: " << LOG_FM(term);
		return std::nullopt;
	}

	DBG(LOG_TRACE << "bv_bitblasting_solve/bitblasted: " << LOG_FM(bitblasted.value());)

	auto bool_solution = lgrs<bool_node>(bitblasted.value());
	if (!bool_solution) {
		LOG_ERROR << "Solving bitblasted term failed for: " << LOG_FM(term);
		return std::nullopt;
	}

	DBG(LOG_TRACE << "bv_bitblasting_solve/bool_solution: " << dump_to_str(bool_solution.value());)

	auto solution = unbitblast<node>(bool_solution.value());
	if (!solution) {
		LOG_ERROR << "Unbitblasting failed for term: " << LOG_FM(term);
		return std::nullopt;
	}

	DBG(LOG_TRACE << "bv_bitblasting_solve/solution: " << dump_to_str(solution.value());)

	return solution;
}

template<NodeType node>
bool bv_bitblasting_sat(tref term) {
	return bitblast<node>(term);
}

template<NodeType node>
bool bv_bitblasting_unsat(tref term) {
	return bitblast<node>(term);
}

template<NodeType node>
bool bv_bitblasting_valid(tref term) {
	return bitblast<node>(term);
}*/

template<NodeType node>
tref bvadd_predicate([[maybe_unused]] tref result, [[maybe_unused]] tref var) {
	// Unsupported operations for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref bvadd_predicate([[maybe_unused]] tref left, [[maybe_unused]] tref right, [[maybe_unused]]	 tref var) {
	// Unsupported operations for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref bvsub_predicate([[maybe_unused]] tref result, [[maybe_unused]] tref var) {
	// Unsupported operations for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref bvsub_predicate([[maybe_unused]] tref left, [[maybe_unused]] tref right, [[maybe_unused]] tref var) {
	// Unsupported operations for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref bvrhl_one_predicate([[maybe_unused]] tref left, [[maybe_unused]] tref var) {
	// Unsupported operations for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref bvshl_one_predicate([[maybe_unused]] tref left, [[maybe_unused]] tref var) {
	// Unsupported operations for now
	LOG_ERROR << "Not yet implemented.";
	return nullptr;
}

template<NodeType node>
tref atomic_predicate_blasting(tref term) {
	using tau = tree<node>;
	//using tt = tau::traverser;

	subtree_map<node, tref> changes;
	tref predicate = nullptr;

	auto f = [&](tref t) {
		auto nt = tau::get(t).get_type();
		auto type_id = tau::get(t).get_ba_type();

		switch (nt) {
			case tau::bf_add: case tau::bf_sub: {
				auto var = build_variable<node>(type_id);
				auto left = tau::get(t).child(0);
				auto right = tau::get(t).child(1);
				changes[t] = var;
				auto current_predicate = (nt == tau::bf_add)
					? bvadd_predicate<node>(left, right, var)
					: bvsub_predicate<node>(left, right, var);
				auto subformula = predicate
					? build_bf_and<node>(predicate, current_predicate)
					: current_predicate;
				predicate = build_wff_ex<node>(var, subformula);
				break;
			}
			case tau::bf_shr: case tau::bf_shl: {
				auto var = build_bf_var<node>(type_id);
				auto left = tau::get(t).child(0);
				changes[t] = var;
				auto current_predicate = (nt == tau::bf_shl)
					? bvshl_one_predicate<node>(left, var)
					: bvrhl_one_predicate<node>(left, var);
				auto subformula = predicate
					? build_bf_and<node>(predicate, current_predicate)
					: current_predicate;
				predicate = build_wff_ex<node>(var, subformula);
				break;
			}
			case tau::bf_mul: case tau::bf_div: case tau::bf_mod:
			case tau::bf_nand: case tau::bf_xnor: case tau::bf_nor:{
				// Unsupported operations for now
				LOG_ERROR << "Arithmetic operations are not supported yet predicate blasting.";
				return false;
			}
			default: {
				auto n_t = rewriter::replace<node>(t, changes);
				if (n_t != t) changes[t] = n_t;
				break;
			}
		}
		return true;
	};

	post_order<node>(term).search_unique(f);

	if (changes.find(term) != changes.end()) {
		DBG(LOG_TRACE << "bv_predicate_blasting/term -> changes[term]: "
			<< LOG_FM(term) << " -> " << LOG_FM(changes[term]);)
		return changes[term];
	}
	return term;
}

template<NodeType node>
tref wff_predicate_blasting(tref term) {
	using tau = tree<node>;
	using tt = tau::traverser;

	subtree_map<node, tref> changes;
	auto one = bool_tree::_1(bool_id);

	auto f = [&](tref t) {
		auto nt = tau::get(t).get_type();
		auto type_id = tau::get(t).get_ba_type();

		switch (nt) {
			case tau::bf_interval:
			case tau::bf_lt: case tau::bf_gt: case tau::bf_lteq: case tau::bf_gteq:
			case tau::bf_nlt: case tau::bf_ngt: case tau::bf_nlteq: case tau::bf_ngteq: {
				// Unsupported operations for now
				LOG_ERROR << "Unsupported comparison in wff predicate blasting: "
					<< LOG_NT(nt) << " " << LOG_FM(tt(term)|| tt::ref);
				return false;
			}
			case tau::bf_eq: case tau::bf_neq: {
				auto bv_width = get_bv_width<node>(type_id);
				auto left = atomic_predicate_blasting<node>(tau::get(term)[0]);
				auto right = atomic_predicate_blasting<node>(tau::get(term)[1]);
				if (!left || !right) return nullptr;
				tref result = bool_tree::_0(bool_id);
				for (size_t i = 0; i < bv_width; ++i) {
					result = build_bf_or<bool_node>(
						result,
						build_bf_xor<bool_node>(left.value()[i], right.value()[i])
					);
				}
				return (nt == tau::bf_eq)
					? build_bf_eq_0<bool_node>(result)
					: build_bf_eq_0<bool_node>(build_bf_xor<bool_node>(result, one));
			}
			case tau::wff_t: case tau::wff_f: {
				return ref;
			}
			default: {
				// Unsupported operations for now
				LOG_ERROR << "Unsupported operation in wff bitblasting: " << LOG_NT(nt) << " " << LOG_FM(tt(term)|| tt::ref);
				return false;
			}
		}
		return true;
	};

	post_order<node>(term).search_unique(f);

	if (changes.find(term) != changes.end()) {
		DBG(LOG_TRACE << "bv_predicate_blasting/term -> changes[term]: "
			<< LOG_FM(term) << " -> " << LOG_FM(changes[term]);)
		return changes[term];
	}
	return term;
}



} // namespace idni::tau_lang