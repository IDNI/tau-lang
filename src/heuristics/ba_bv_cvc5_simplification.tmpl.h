// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_ba-simplification"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

template<NodeType node>
tref cvc5_tree_to_tau_tree(bv n) {
#define rec(x) (cvc5_tree_to_tau_tree<node>(x)) // ease recursive calls
	using tau = tree<node>;

	DBG(LOG_INFO << "cvc5_tree_to_tau_tree/n: " << n.toString() << "\n";)

	auto from_collection = [](const bv& t, const auto& f) {
		tref res = rec(t[0]);
		for (size_t i = 1; i < t.getNumChildren(); ++i) {
			res = f(res, rec(t[i]));
		}
		return res;
	};

	auto get_var = [&](const cvc5::Term& v) -> tref {
		std::string cvc5_var = v.toString();
		DBG(LOG_INFO << "get_var/cvc5_var: " << cvc5_var << "\n";)
		// remove leading and trailing spaces `|`
		if (cvc5_var.front() == '|') cvc5_var.erase(0, 1);
		if (cvc5_var.back() == '|') cvc5_var.pop_back();
		// setting parsing options to parse variable as bf
		typename tau::get_options opts = {
			.parse = { .start = tau::bf }
		};
		// returning parsed variable tree
		auto result = tau::get(cvc5_var, opts);
		DBG(LOG_INFO << "cvc5_tree_to_tau_tree/result: " << tau::get(result).tree_to_str() << "\n";)
		return result;
	};

	switch (n.getKind()) {
		case Kind::AND: return from_collection(n, build_wff_and<node>);
		case Kind::OR: return from_collection(n, build_wff_or<node>);
		case Kind::NOT: return build_wff_neg<node>(rec(n[0]));
		case Kind::EXISTS: return build_wff_ex<node>(rec(n[0]), rec(n[1]));
		case Kind::FORALL: return build_wff_all<node>(rec(n[0]), rec(n[1]));
		case Kind::VARIABLE_LIST: return tau::trim(rec(n[0]));
		case Kind::VARIABLE:
			return get_var(n);

		case Kind::CONST_BOOLEAN:
			if (n.getBooleanValue()) return tau::_T();
			else return tau::_F();

		case Kind::EQUAL: return build_bf_eq<node>(rec(n[0]), rec(n[1]));
		case Kind::DISTINCT: return build_bf_neq<node>(rec(n[0]), rec(n[1]));
		case Kind::BITVECTOR_ULE: return build_bf_lteq<node>(rec(n[0]), rec(n[1]));
		case Kind::BITVECTOR_UGT: return build_bf_gt<node>(rec(n[0]), rec(n[1]));
		case Kind::BITVECTOR_UGE: return build_bf_gteq<node>(rec(n[0]), rec(n[1]));
		case Kind::BITVECTOR_ULT: return build_bf_lt<node>(rec(n[0]), rec(n[1]));

		case Kind::CONSTANT: return get_var(n);
		case Kind::CONST_BITVECTOR: return build_bf_ba_constant<node>(n,
			get_ba_type_id<node>(bv_type<node>(n.getSort().getBitVectorSize())));

		case Kind::BITVECTOR_NOT: return build_bf_neg<node>(rec(n[0]));
		// TODO (HIGH) should be two's complement
		case Kind::BITVECTOR_NEG: return build_bf_neg<node>(rec(n[0]));
		case Kind::BITVECTOR_AND: return from_collection(n, build_bf_and<node>);
		case Kind::BITVECTOR_OR: return from_collection(n, build_bf_or<node>);
		case Kind::BITVECTOR_XOR: return from_collection(n, build_bf_xor<node>);
		case Kind::BITVECTOR_NAND: return from_collection(n, build_bf_nand<node>);
		case Kind::BITVECTOR_NOR: return from_collection(n, build_bf_nor<node>);
		case Kind::BITVECTOR_XNOR: return from_collection(n, build_bf_xnor<node>);
		case Kind::BITVECTOR_ADD: return from_collection(n, build_bf_add<node>);
		case Kind::BITVECTOR_SUB: return from_collection(n, build_bf_sub<node>);
		case Kind::BITVECTOR_MULT: return from_collection(n, build_bf_mul<node>);
		case Kind::BITVECTOR_UDIV: return build_bf_div<node>(rec(n[0]), rec(n[1]));
		case Kind::BITVECTOR_SMOD: return build_bf_mod<node>(rec(n[0]), rec(n[1]));
		case Kind::BITVECTOR_SHL: return build_bf_shl<node>(rec(n[0]), rec(n[1]));
		case Kind::BITVECTOR_LSHR: return build_bf_shr<node>(rec(n[0]), rec(n[1]));

		default:
			LOG_ERROR << "Unexpected bitvector kind during tree translation: "
					<< n.getKind() << "\n";
			DBG(assert(false);)
			return nullptr;
	}
#undef rec
}

template<NodeType node>
tref ba_bv_cvc5_simplification(tref term) {
	using tau = tree<node>;
	using tt = tau::traverser;

	subtree_map<node, bv> vars, free_vars;
	auto bv_term = bv_eval_node<node>(tt(term), vars, free_vars);
	if (!bv_term) return term; // Unable to transform to bv
	auto simplified_bv = normalize(bv_term.value());
	return cvc5_tree_to_tau_tree<node>(simplified_bv);
}

} // namespace idni::tau_lang