// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "boolean_algebras/bv_ba.h" // Only for IDE resolution, not really needed.
#include "../parser/bitvector_parser.generated.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_ba"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

template<NodeType node>
size_t get_bv_size(const tref t) {
	using tau = tree<node>;
	using tt = tau::traverser;
	if (auto subtype = tt(t) | tau::subtype | tt::ref; subtype) {
		return tau::get(subtype)[0].get_num();
	}
	return default_bv_size;
}

template <NodeType node>
std::optional<bv> bv_eval_node(const typename tree<node>::traverser& form, subtree_map<node, bv> vars,
			       subtree_map<node, bv>& free_vars) {
	using tau = tree<node>;
	using tt = tree<node>::traverser;

	auto nt = form | tt::nt;

	switch (nt) {
		case node::type::wff_always:
		case node::type::wff_sometimes: {
			return bv_eval_node<node>(form | tt::first, vars, free_vars);
		}
		// due to hooks we should consider wff_t or bf_t
		case node::type::wff: case node::type::bf:
		// TODO (HIGH) deal with those eq appropiately
		/*case node::type::bv:*/ {
			return bv_eval_node<node>(form | tt::first, vars, free_vars);
		}
		case node::type::wff_t: return make_bitvector_true();
		case node::type::wff_f: return make_bitvector_false();
		case node::type::wff_neg: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			return l ? std::optional<bv>(make_term_not(l.value())) : std::nullopt;
		}
		case node::type::wff_and: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_and(l.value(), r.value())) : std::nullopt;
		}
		case node::type::wff_or: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_or(l.value(), r.value())) : std::nullopt;
		}
		case node::type::wff_all: {
			tref v = (form | tt::first | tt::ref);
			DBG(assert(is<node>(v, tau::variable));)
			size_t bv_size = get_bv_size<node>(tau::get(v).get_ba_type_tree());
			bv x = cvc5_term_manager.mkVar(cvc5_term_manager.mkBitVectorSort(bv_size), tau::get(v).to_str());
			vars.emplace(v, x);

			auto f = bv_eval_node<node>(form | tt::second, vars, free_vars);
			if (!f) return std::nullopt;
			auto res = std::optional<bv>(make_term_forall({x}, f.value()));
			vars.erase(v);
			return res;
		}
		case node::type::wff_ex: {
			tref v = (form | tt::first | tt::ref);
			DBG(assert(is<node>(v, tau::variable));)
			size_t bv_size = get_bv_size<node>(tau::get(v).get_ba_type_tree());
			bv x = cvc5_term_manager.mkVar(cvc5_term_manager.mkBitVectorSort(bv_size), tau::get(v).to_str());
			vars.emplace(v, x);

			auto f = bv_eval_node<node>(form | tt::second, vars, free_vars);
			if (!f) return std::nullopt;
			auto res = std::optional<bv>(make_term_exists({x}, f.value()));
			vars.erase(v);
			return res;
		}
		case node::type::variable: {
			// check if the variable is alr
			tref v = form | tt::ref;
			if (auto it = vars.find(v); it != vars.end()) return it->second;
			if (auto it = free_vars.find(form | tt::ref); it != free_vars.end()) return it->second;
			auto vn = (form | tt::Tree).to_str();
			// create a new constant according to the type and added to the map
			size_t bv_size = get_bv_size<node>(tau::get(v).get_ba_type_tree());
			auto x = cvc5_term_manager.mkConst(cvc5_term_manager.mkBitVectorSort(bv_size), vn.c_str());
			free_vars.emplace(form | tt::ref, x);
			return std::optional<bv>(x);
		}
		case node::type::bf_eq: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_equal(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_neq: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_distinct(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_lteq: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_less_equal(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_nlteq: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_not(make_term_less_equal(l.value(), r.value()))) : std::nullopt;
		}
		case node::type::bf_gt: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_greater(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_ngt: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_not(make_term_greater(l.value(), r.value()))) : std::nullopt;
		}
		case node::type::bf_gteq: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_greater_equal(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_ngteq: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_not(make_term_greater_equal(l.value(), r.value()))) : std::nullopt;
		}
		case node::type::bf_lt: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_less(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_nlt: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_term_not(make_term_less(l.value(), r.value()))) : std::nullopt;
		}
		case node::type::bf_neg: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			return (l) ? std::optional<bv>(make_bitvector_not(l.value())) : std::nullopt;
		}
		case node::type::bf_add: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_add(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_sub: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_sub(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_mul: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_mul(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_div: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_div(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_mod: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_mod(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_and: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_and(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_nand: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_nand(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_or: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_or(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_nor: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_nor(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_xor: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_xor(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_xnor: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_xnor(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_shl: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_shr(l.value(), r.value())) : std::nullopt;
		}
		case node::type::bf_shr: {
			auto l = bv_eval_node<node>(form | tt::first, vars, free_vars);
			auto r = bv_eval_node<node>(form | tt::second, vars, free_vars);
			return (l && r) ? std::optional<bv>(make_bitvector_shr(l.value(), r.value())) : std::nullopt;
		}
		case node::type::ba_constant: {
			auto cte = form | tt::ba_constant;
			DBG(assert(std::holds_alternative<bv>(cte));)
			return std::optional<bv>(std::get<bv>(cte));
		}
		case node::type::bf_t: {
			tref c = form | tt::ref;
			auto bv_size = get_bv_size<node>(tau::get(c).get_ba_type_tree());
			return make_bitvector_top_elem(bv_size);
		}
		case node::type::bf_f: {
			tref c = form | tt::ref;
			auto bv_size = get_bv_size<node>(tau::get(c).get_ba_type_tree());
			return make_bitvector_bottom_elem(bv_size);
		}
		default:
			return std::nullopt;
	}
}

template<NodeType node>
std::optional<bv> bv_eval_node(tref form, subtree_map<node, bv> vars,
	subtree_map<node, bv>& free_vars) {
	using tau = tree<node>;
	using tt = tau::traverser;
	auto t = tt(form);
	return bv_eval_node(t, vars, free_vars);
}

template<NodeType node>
tref cvc5_tree_to_tau_tree(bv n) {
#define rec(x) (cvc5_tree_to_tau_tree<node>(x)) // ease recursive calls

	auto from_collection = [](const bv& t, const auto& f) {
		tref res = rec(t[0]);
		for (size_t i = 1; i < t.getNumChildren(); ++i) {
			res = f(res, rec(t[i]));
		}
		return res;
	};

	using tau = tree<node>;
	switch (n.getKind()) {
		case Kind::AND: return from_collection(n, build_wff_and<node>);
		case Kind::OR: return from_collection(n, build_wff_or<node>);
		case Kind::NOT: return build_wff_neg<node>(rec(n[0]));
		case Kind::EXISTS: return build_wff_ex<node>(rec(n[0]), rec(n[1]));
		case Kind::FORALL: return build_wff_all<node>(rec(n[0]), rec(n[1]));
		case Kind::VARIABLE_LIST: return tau::trim(rec(n[0]));
		case Kind::VARIABLE: return build_bf_variable<node>(n.toString(),
			get_ba_type_id<node>(bv_type<node>(n.getSort().getBitVectorSize())));

		case Kind::CONST_BOOLEAN:
			if (n.getBooleanValue()) return tau::_T();
			else return tau::_F();

		case Kind::EQUAL: return build_bf_eq<node>(rec(n[0]), rec(n[1]));
		case Kind::DISTINCT: return build_bf_neq<node>(rec(n[0]), rec(n[1]));
		case Kind::BITVECTOR_ULE: return build_bf_lteq<node>(rec(n[0]), rec(n[1]));
		case Kind::BITVECTOR_UGT: return build_bf_gt<node>(rec(n[0]), rec(n[1]));
		case Kind::BITVECTOR_UGE: return build_bf_gteq<node>(rec(n[0]), rec(n[1]));
		case Kind::BITVECTOR_ULT: return build_bf_lt<node>(rec(n[0]), rec(n[1]));

		case Kind::CONSTANT: return build_bf_variable<node>(n.toString(),
			get_ba_type_id<node>(bv_type<node>(n.getSort().getBitVectorSize())));
		case Kind::CONST_BITVECTOR: return build_bf_ba_constant<node>(n,
			get_ba_type_id<node>(bv_type<node>(n.getSort().getBitVectorSize())));

		case Kind::BITVECTOR_NOT: return build_bf_neg<node>(rec(n[0]));
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

template <NodeType node>
bool is_bv_formula_sat(tref form) {
	using tt = tree<node>::traverser;

	subtree_map<node, bv> vars, free_vars;
	cvc5::Solver solver(cvc5_term_manager);
	config_cvc5_solver(solver);

	auto expr = bv_eval_node<node>(tt(form), vars, free_vars);
	// TODO (MEDIUM) handle this case at an upper level (maybe return an optional)
	if (!expr) {
		LOG_ERROR << "Failed to translate the formula to cvc5: " << LOG_FM(form);
		return false;
	}
	DBG( LOG_TRACE << "CVC5 translated formula: " << expr.value(); )
	solver.assertFormula(expr.value());
	return solver.checkSat().isSat();
}

template <NodeType node>
bool is_bv_formula_unsat(tref form) {
	return !is_bv_formula_sat<node>(form);
}

template <NodeType node>
bool is_bv_formula_valid(tref form) {
	using tau = tree<node>;
	return is_bv_formula_unsat<node>(tau::build_wff_neg(form));
}

template <NodeType node>
std::optional<solution<node>> solve_bv(const tref form) {
	using tau = tree<node>;
	using tt = tau::traverser;

	subtree_map<node, bv> vars, free_vars;
	cvc5::Solver solver(cvc5_term_manager);
	config_cvc5_solver(solver);

	auto expr = bv_eval_node<node>(tt(form), vars, free_vars);
	if (!expr) {
		LOG_ERROR << "Failed to translate the formula to cvc5: " << LOG_FM(form);
		return std::nullopt;
	}
	DBG( LOG_TRACE << "CVC5 translated formula: " << expr.value(); )

	// solve the equations
	solver.assertFormula(expr.value());
	LOG_DEBUG << "Solving bitvector formula: " << expr.value();
	auto result = solver.checkSat();
	// extract the model and return the solution if sat
	if (result.isSat()) {
		LOG_DEBUG << "Bitvector system is sat.";
		solution<node> s;
		for (const auto& [tau_var, bv_var] : free_vars) {
			bv cte = solver.getValue(bv_var);
			s.emplace(tau::get(tau::bf, tau_var),
				tau::get(tau::bf, tau::get_ba_constant(cte,
					bv_type<node>(cte.getSort().getBitVectorSize()))));
		}
		return s;
	} else
		LOG_DEBUG << "Bitvector system is unsat.";
	return {};
}

template<NodeType node>
std::optional<solution<node>> solve_bv(const trefs& lits) {
	using tau = tree<node>;

	return solve_bv<node>(tau::build_wff_and(lits));
}

template<typename ... BAs> requires BAsPack<BAs...>
std::optional<bv> bv_constant_from_parse_tree(tref parse_tree, tref type_tree) {
	using tt = bitvector_parser::tree::traverser;
	using bv_parser = bitvector_parser::nonterminal;
	if (!parse_tree) return std::nullopt;
	auto t = bitvector_parser::tree::traverser(parse_tree)
					| bitvector_parser::bitvector;

	auto type = t | tt::only_child | tt::nonterminal;
	size_t base;
	switch (type) {
		case bv_parser::decimal: { base = 10; break; }
		case bv_parser::binary: { base = 2; break; }
		case bv_parser::hexadecimal: { base = 16; break; }
		default: {
			DBG(assert(false);)
			return std::nullopt;
		}
	}
	DBG(assert(base > 0 );)
	size_t bv_size = get_bv_size<node<BAs...>>(type_tree);
	auto str = t | tt::terminals;
	try {
		return make_bitvector_cte(bv_size, str, base);
	} catch (const cvc5::CVC5ApiException& e) {
		LOG_ERROR << "Error creating bitvector constant from string '"
			<< str << "': " << e.what() << "\n";
		return std::nullopt;
	}
}

template<typename...BAs>
requires BAsPack<BAs...>
std::optional<typename node<BAs...>::constant_with_type> parse_bv(const std::string& src,
		tref type_tree) {

	auto result = bitvector_parser::instance().parse(src.c_str(), src.size());
	if (!result.found) {
		auto msg = result.parse_error
			.to_str(bitvector_parser::error::info_lvl::INFO_BASIC);
		LOG_ERROR << "[bv] " << msg << "\n";
		return {}; // Syntax error
	}
	auto cte = bv_constant_from_parse_tree<BAs...>(result.get_shaped_tree2(), type_tree);
	if (!cte) {
		LOG_ERROR << "Failed to parse bitvector constant: " << src;
		return {};
	}
	return typename node<BAs...>::constant_with_type{ cte.value(), type_tree };
}

} // namespace idni::tau_lang