// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_ba_simplification"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

template<NodeType node>
tref cvc5_tree_to_tau_tree(bv n,
	const std::map<std::string, tref>& var_map) {
#undef rec
#define rec(x) (cvc5_tree_to_tau_tree<node>(x, var_map)) // ease recursive calls
	using tau = tree<node>;

	DBG(LOG_INFO << "cvc5_tree_to_tau_tree/n: " << n.toString() << "\n";)

	auto from_collection = [&var_map](const bv& t, const auto& f) -> tref {
		tref res = rec(t[0]);
		if (res == nullptr) return nullptr; // Unable to transform to tau (returning null)

		for (size_t i = 1; i < t.getNumChildren(); ++i) {
			auto temp = rec(t[i]);
			if (temp == nullptr) return nullptr; // Unable to transform to tau (returning null)
			res = f(res, temp);
		}
		return res;
	};

	auto get_var = [&](const cvc5::Term& v) -> tref {
		std::string cvc5_var = v.toString();
		DBG(LOG_TRACE << "cvc5_tree_to_tau_tree/get_var/cvc5_var: " << cvc5_var << "\n";)
		// remove leading and trailing `|` delimiters added by CVC5
		if (cvc5_var.front() == '|') cvc5_var.erase(0, 1);
		if (cvc5_var.back() == '|') cvc5_var.pop_back();
		size_t bv_size = v.getSort().getBitVectorSize();
		// return the original tau node (bf-wrapped) if it was registered in the var_map
		if (auto it = var_map.find(cvc5_var); it != var_map.end())
			return tau::get_typed(tau::bf, it->second,
				get_ba_type_id<node>(bv_type<node>(bv_size)));
		DBG(LOG_TRACE << "cvc5_tree_to_tau_tree/get_var/bv_size: " << bv_size << "\n";)
		auto result = build_bf_variable<node>(cvc5_var, get_ba_type_id<node>(bv_type<node>(bv_size)));
		DBG(LOG_TRACE << "cvc5_tree_to_tau_tree/get_var/result: " << tau::get(result).tree_to_str() << "\n";)
		return result;
	};

	switch (n.getKind()) {
		case Kind::AND: return from_collection(n, build_wff_and<node>);
		case Kind::OR: return from_collection(n, build_wff_or<node>);
		case Kind::NOT: {
			auto fml = rec(n[0]);
			return fml != nullptr ? build_wff_neg<node>(fml) : nullptr; // Unable to transform to tau (returning null)
		}
		case Kind::EXISTS: {
			auto var_list = rec(n[0]);
			auto body = rec(n[1]);
			return (var_list != nullptr && body != nullptr) ? build_wff_ex<node>(var_list, body) : nullptr; // Unable to transform to tau (returning null)
		}
		case Kind::FORALL: {
			auto var_list = rec(n[0]);
			auto body = rec(n[1]);
			return (var_list != nullptr && body != nullptr) ? build_wff_all<node>(var_list, body) : nullptr; // Unable to transform to tau (returning null)
		}
		case Kind::VARIABLE_LIST: {
			auto var_list = rec(n[0]);
			return var_list != nullptr ? tau::trim(var_list) : nullptr; // Unable to transform to tau (returning null)
		}
		case Kind::VARIABLE:
			return get_var(n);

		case Kind::CONST_BOOLEAN:
			if (n.getBooleanValue()) return tau::_T();
			else return tau::_F();

		case Kind::EQUAL: {
			auto left = rec(n[0]);
			auto right = rec(n[1]);
			return (left != nullptr && right != nullptr) ? build_bf_eq<node>(left, right) : nullptr; // Unable to transform to tau (returning null)
		}
		case Kind::DISTINCT: {
			auto left = rec(n[0]);
			auto right = rec(n[1]);
			return (left != nullptr && right != nullptr) ? build_bf_neq<node>(left, right) : nullptr; // Unable to transform to tau (returning null)
		}
		case Kind::BITVECTOR_ULE: {
			auto left = rec(n[0]);
			auto right = rec(n[1]);
			return (left != nullptr && right != nullptr) ? build_bf_lteq<node>(left, right) : nullptr; // Unable to transform to tau (returning null)
		}
		case Kind::BITVECTOR_UGT: {
			auto left = rec(n[0]);
			auto right = rec(n[1]);
			return (left != nullptr && right != nullptr) ? build_bf_gt<node>(left, right) : nullptr; // Unable to transform to tau (returning null)
		}
		case Kind::BITVECTOR_UGE: {
			auto left = rec(n[0]);
			auto right = rec(n[1]);
			return (left != nullptr && right != nullptr) ? build_bf_gteq<node>(left, right) : nullptr; // Unable to transform to tau (returning null)
		}
		case Kind::BITVECTOR_ULT: {
			auto left = rec(n[0]);
			auto right = rec(n[1]);
			return (left != nullptr && right != nullptr) ? build_bf_lt<node>(left, right) : nullptr; // Unable to transform to tau (returning null)
		}
		case Kind::CONSTANT: return get_var(n);
		case Kind::CONST_BITVECTOR: return build_bf_ba_constant<node>(n,
			get_ba_type_id<node>(bv_type<node>(n.getSort().getBitVectorSize())));

		case Kind::BITVECTOR_NOT: {
			auto operand = rec(n[0]);
			return operand != nullptr ? build_bf_neg<node>(operand) : nullptr; // Unable to transform to tau (returning null)
		}
		// TODO (HIGH) should be two's complement
		case Kind::BITVECTOR_NEG: {
			auto operand = rec(n[0]);
			return operand != nullptr ? build_bf_neg<node>(operand) : nullptr; // Unable to transform to tau (returning null)
		}
		case Kind::BITVECTOR_AND: return from_collection(n, build_bf_and<node>);
		case Kind::BITVECTOR_OR: return from_collection(n, build_bf_or<node>);
		case Kind::BITVECTOR_XOR: return from_collection(n, build_bf_xor<node>);
		case Kind::BITVECTOR_NAND: return from_collection(n, build_bf_nand<node>);
		case Kind::BITVECTOR_NOR: return from_collection(n, build_bf_nor<node>);
		case Kind::BITVECTOR_XNOR: return from_collection(n, build_bf_xnor<node>);
		case Kind::BITVECTOR_ADD: return from_collection(n, build_bf_add<node>);
		case Kind::BITVECTOR_SUB: return from_collection(n, build_bf_sub<node>);
		case Kind::BITVECTOR_MULT: return from_collection(n, build_bf_mul<node>);
		case Kind::BITVECTOR_UDIV: {
			auto left = rec(n[0]);
			auto right = rec(n[1]);
			return (left != nullptr && right != nullptr) ? build_bf_div<node>(left, right) : nullptr; // Unable to transform to tau (returning null)
		}
		case Kind::BITVECTOR_UREM: {
			auto left = rec(n[0]);
			auto right = rec(n[1]);
			return (left != nullptr && right != nullptr) ? build_bf_mod<node>(left, right) : nullptr; // Unable to transform to tau (returning null)
		}
		case Kind::BITVECTOR_SHL: {
			auto left = rec(n[0]);
			auto right = rec(n[1]);
			return (left != nullptr && right != nullptr) ? build_bf_shl<node>(left, right) : nullptr; // Unable to transform to tau (returning null)
		}
		case Kind::BITVECTOR_LSHR: {
			auto left = rec(n[0]);
			auto right = rec(n[1]);
			return (left != nullptr && right != nullptr) ? build_bf_shr<node>(left, right) : nullptr; // Unable to transform to tau (returning null)
		}
		case Kind::BITVECTOR_ZERO_EXTEND: {
			tref operand = rec(n[0]);
			if (!operand) return nullptr;
			uint32_t extra_bits = n.getOp()[0].getUInt32Value();
			size_t src_size = n[0].getSort().getBitVectorSize();
			size_t target_size = src_size + extra_bits;
			return build_bf_cast<node>(operand, get_ba_type_id<node>(bv_type<node>(target_size)));
		}
		case Kind::BITVECTOR_EXTRACT: {
			tref operand = rec(n[0]);
			if (!operand) return nullptr;
			uint32_t hi = n.getOp()[0].getUInt32Value();
			uint32_t lo = n.getOp()[1].getUInt32Value();
			size_t target_size = hi - lo + 1;
			return build_bf_cast<node>(operand, get_ba_type_id<node>(bv_type<node>(target_size)));
		}

		default: {
			DBG(LOG_DEBUG << "Unexpected bitvector kind during tree translation: "
					<< n.getKind() << "\n";)
		}
		return nullptr;
	}
#undef rec
}

template<NodeType node>
tref bv_ba_cvc5_simplification(tref term) {
	using tau = tree<node>;
	using tt = tau::traverser;

	subtree_map<node, bv> vars, free_vars;
	auto bv_term = bv_eval_node<node>(tt(term), vars, free_vars);
	if (!bv_term) return nullptr; // Unable to transform to bv (returning null)
	DBG(LOG_TRACE << "bv_ba_cvc5_simplification/bv_term: " << bv_term.value().toString() << "\n";)
	auto simplified_bv = normalize_bv(bv_term.value());
	DBG(LOG_TRACE << "bv_ba_cvc5_simplification/simplified_bv: " << simplified_bv.toString() << "\n";)
	// build reverse lookup so cvc5_tree_to_tau_tree can recover original tau nodes
	std::map<std::string, tref> var_map;
	for (const auto& [orig, cvc5_var] : free_vars) {
		std::string name = cvc5_var.toString();
		if (!name.empty() && name.front() == '|') name.erase(0, 1);
		if (!name.empty() && name.back() == '|') name.pop_back();
		var_map.emplace(name, orig);
	}
	auto simplified_term = cvc5_tree_to_tau_tree<node>(simplified_bv, var_map);
#ifdef DEBUG
	if (simplified_term)
		LOG_TRACE << "bv_ba_cvc5_simplification/simplified_term: " << tau::get(simplified_term) << "\n"
			<< "bv_ba_cvc5_simplification/simplified_term: " << tau::get(simplified_term).tree_to_str() << "\n";
#endif // DEBUG

	return simplified_term != nullptr ? simplified_term : term;
}

} // namespace idni::tau_lang