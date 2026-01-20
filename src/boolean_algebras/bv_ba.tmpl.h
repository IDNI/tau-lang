// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

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
			return (l && r) ? std::optional<bv>(make_bitvector_shl(l.value(), r.value())) : std::nullopt;
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

bool operator==(const Term& lhs, const bool& rhs) {
	if (rhs) {
		// Check if lhs equals 1
		return normalize(lhs) == make_bitvector_top_elem(lhs.getSort().getBitVectorSize());
	} else {
		// Check if rhs equals 0
		return normalize(lhs) == make_bitvector_bottom_elem(lhs.getSort().getBitVectorSize());
	}
}

bool operator==(const bool& lhs, const Term& rhs) { return rhs == lhs; }

bool operator!=(const Term& lhs, const bool& rhs) { return !(lhs == rhs); }

bool operator!=(const bool& lhs, const Term& rhs) { return !(rhs == lhs); }

template <typename ...BAs> requires BAsPack<BAs...>
bool is_associative_and_commutative(size_t symbol) {
	using tau = tree<node<BAs...>>;
	switch (symbol) {
		case tau::bf_add: return true;
		case tau::bf_sub: return false;
		case tau::bf_mul: return true;
		case tau::bf_div: return false;
		case tau::bf_mod: return false;
		case tau::bf_shr: return false;
		case tau::bf_shl: return false;
		case tau::bf_nor: return false;
		case tau::bf_xnor: return false;
		case tau::bf_nand: return false;
		default: return false;
	}
}

template<typename ... BAs> requires BAsPack<BAs...>
size_t get_inv_sym(size_t symbol) {
	using tau = tree<node<BAs...>>;
	switch (symbol) {
		case tau::bf_add: return tau::bf_sub;
		case tau::bf_mul: return tau::bf_div;
		default: return symbol;
	}
}

template<NodeType node>
tref term_add(tref symbol) {
	using tau = tree<node>;
	auto add_consts = [](const bv& c1, const bv& c2, size_t type_id) {
		bv res = make_bitvector_add(c1, c2);
		typename node::constant v = {res};
		return tau::build_bf_ba_constant(v, type_id);
	};
	// bf > term symbol > (bf > term symbol) (bf > term_symbol)
	const tau& c1 = tau::get(symbol)[0][0][0];
	const tau& c2 = tau::get(symbol)[0][1][0];
	switch (c1.value.nt) {
		// 1 + X
		case tau::bf_t: {
			if (size_t t = c2.get_ba_type();
				c2.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width =  get_bv_width<node>(get_ba_type_tree<node>(t));
				return add_consts(
					make_bitvector_top_elem(width),
					std::get<bv>(c2.get_ba_constant()),
					c2.get_ba_type());
			}
			// 1 + 1
			if (c2.is(tau::bf_t)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return add_consts(
					make_bitvector_top_elem(width),
					make_bitvector_top_elem(width),
					c2.get_ba_type());
			}
			break;
		}
		// 0 + X
		case tau::bf_f: return tau::get(symbol)[0].second();
		default: break;
	}
	switch (c2.value.nt) {
		// X + 1
		case tau::bf_t: {
			if (size_t t = c1.get_ba_type();
				c1.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return add_consts(
					std::get<bv>(c1.get_ba_constant()),
					make_bitvector_top_elem(width),
					c1.get_ba_type());
				}
			break;
		}
		// x + 0
		case tau::bf_f: return tau::get(symbol)[0].first();
		default: break;
	}
	// { ... } + { ... }
	if (c1.is_ba_constant() && c2.is_ba_constant()
		&& c1.get_ba_type() > 0 && c2.get_ba_type() == c1.get_ba_type()) {
		DBG(assert(is_bv_type_family<node>(c1.get_ba_type()));)
		return add_consts(std::get<bv>(c1.get_ba_constant()),
					std::get<bv>(c2.get_ba_constant()),
					c1.get_ba_type());
	}
	return symbol;
}

template<NodeType node>
tref term_sub(tref symbol) {
	using tau = tree<node>;
	auto sub_consts = [](const bv& c1, const bv& c2, size_t type_id) {
		bv res = make_bitvector_sub(c1, c2);
		typename node::constant v = {res};
		return tau::build_bf_ba_constant(v, type_id);
	};
	// bf > term symbol > (bf > term symbol) (bf > term_symbol)
	const tau& c1 = tau::get(symbol)[0][0][0];
	const tau& c2 = tau::get(symbol)[0][1][0];
	switch (c1.value.nt) {
		// 1 - X
		case tau::bf_t: {
			if (size_t t = c2.get_ba_type();
				c2.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width =  get_bv_width<node>(get_ba_type_tree<node>(t));
				return sub_consts(
					make_bitvector_top_elem(width),
					std::get<bv>(c2.get_ba_constant()),
					c2.get_ba_type());
			}
			// 1 - 1
			if (c2.is(tau::bf_t)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				return tau::_0(c2.get_ba_type());
			}
			break;
		}
		// 0 - X cannot be simplified
		case tau::bf_f: {
			// 0 - 1
			if (c2.is(tau::bf_t)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width =  get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return sub_consts(
					make_bitvector_bottom_elem(width),
					make_bitvector_top_elem(width),
					c2.get_ba_type());
			}
		}
		default: break;
	}
	switch (c2.value.nt) {
		// X - 1
		case tau::bf_t: {
			if (size_t t = c1.get_ba_type();
				c1.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return sub_consts(
					std::get<bv>(c1.get_ba_constant()),
					make_bitvector_top_elem(width),
					c1.get_ba_type());
				}
			break;
		}
		// x - 0
		case tau::bf_f: return tau::get(symbol)[0].first();
		default: break;
	}
	// X - X
	if (c1 == c2) return tau::_0(c2.get_ba_type());
	// { ... } - { ... }
	if (c1.is_ba_constant() && c2.is_ba_constant()
		&& c1.get_ba_type() > 0 && c2.get_ba_type() == c1.get_ba_type()) {
		DBG(assert(is_bv_type_family<node>(c1.get_ba_type()));)
		return sub_consts(std::get<bv>(c1.get_ba_constant()),
					std::get<bv>(c2.get_ba_constant()),
					c1.get_ba_type());
		}
	return symbol;
}

template<NodeType node>
tref term_mul(tref symbol) {
	using tau = tree<node>;
	auto mul_consts = [](const bv& c1, const bv& c2, size_t type_id) {
		bv res = make_bitvector_mul(c1, c2);
		typename node::constant v = {res};
		return tau::build_bf_ba_constant(v, type_id);
	};
	// bf > term symbol > (bf > term symbol) (bf > term_symbol)
	const tau& c1 = tau::get(symbol)[0][0][0];
	const tau& c2 = tau::get(symbol)[0][1][0];
	switch (c1.value.nt) {
		// 1 * X
		case tau::bf_t: {
			if (size_t t = c2.get_ba_type();
				c2.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return mul_consts(
					make_bitvector_top_elem(width),
					std::get<bv>(c2.get_ba_constant()),
					c2.get_ba_type());
			}
			// 1 * 1
			if (c2.is(tau::bf_t)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return mul_consts(
					make_bitvector_top_elem(width),
					make_bitvector_top_elem(width),
					c2.get_ba_type());
			}
			break;
		}
		// 0 * X
		case tau::bf_f: return tau::_0(c2.get_ba_type());
		default: break;
	}
	switch (c2.value.nt) {
		// X * 1
		case tau::bf_t: {
			if (size_t t = c1.get_ba_type();
				c1.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return mul_consts(
					std::get<bv>(c1.get_ba_constant()),
					make_bitvector_top_elem(width),
					c1.get_ba_type());
				}
			break;
		}
		// X * 0
		case tau::bf_f: return tau::_0(c1.get_ba_type());
		default: break;
	}
	// {1} * X
	if (c1.is_ba_constant() && c1.get_ba_type() > 0) {
		DBG(assert(is_bv_type_family<node>(c1.get_ba_type()));)
		if (const bv cc = std::get<bv>(c1.get_ba_constant());
			cc.isBitVectorValue() && cc.getBitVectorValue(10) == "1") {
			return tau::get(symbol)[0].second();
		}
	}
	// X * {1}
	if (c2.is_ba_constant() && c2.get_ba_type() > 0) {
		DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
		if (const bv cc = std::get<bv>(c2.get_ba_constant());
			cc.isBitVectorValue() && cc.getBitVectorValue(10) == "1") {
			return tau::get(symbol)[0].first();
		}
	}
	// { ... } * { ... }
	if (c1.is_ba_constant() && c2.is_ba_constant()
		&& c1.get_ba_type() > 0 && c2.get_ba_type() == c1.get_ba_type()) {
		DBG(assert(is_bv_type_family<node>(c1.get_ba_type()));)
		return mul_consts(std::get<bv>(c1.get_ba_constant()),
					std::get<bv>(c2.get_ba_constant()),
					c1.get_ba_type());
		}
	return symbol;
}

template<NodeType node>
tref term_div(tref symbol) {
	using tau = tree<node>;
	auto div_consts = [](const bv& c1, const bv& c2, size_t type_id) {
		bv res = make_bitvector_div(c1, c2);
		typename node::constant v = {res};
		return tau::build_bf_ba_constant(v, type_id);
	};
	// bf > term symbol > (bf > term symbol) (bf > term_symbol)
	const tau& c1 = tau::get(symbol)[0][0][0];
	const tau& c2 = tau::get(symbol)[0][1][0];
	switch (c1.value.nt) {
		// 1 / X
		case tau::bf_t: {
			if (size_t t = c2.get_ba_type();
				c2.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return div_consts(
					make_bitvector_top_elem(width),
					std::get<bv>(c2.get_ba_constant()),
					c2.get_ba_type());
			}
			// 1 / 1
			if (c2.is(tau::bf_t)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return div_consts(
					make_bitvector_top_elem(width),
					make_bitvector_top_elem(width),
					c2.get_ba_type());
			}
			// 1 / 0
			if (c2.is(tau::bf_f)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return div_consts(
					make_bitvector_top_elem(width),
					make_bitvector_bottom_elem(width),
					c2.get_ba_type());
			}
			break;
		}
		// 0 / X
		case tau::bf_f: return tau::_0(c2.get_ba_type());
		default: break;
	}
	switch (c2.value.nt) {
		// X / 1
		case tau::bf_t: {
			if (size_t t = c1.get_ba_type();
				c1.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return div_consts(
					std::get<bv>(c1.get_ba_constant()),
					make_bitvector_top_elem(width),
					c1.get_ba_type());
				}
			break;
		}
		// X / 0
		case tau::bf_f: {
			if (size_t t = c1.get_ba_type();
				c1.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return div_consts(
					std::get<bv>(c1.get_ba_constant()),
					make_bitvector_bottom_elem(width),
					c1.get_ba_type());
				}
			break;
		}
		default: break;
	}
	// X / X
	if (c1 == c2) {
		const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
		return tau::build_bf_ba_constant(make_bitvector_value(1, width), c2.get_ba_type());
	}
	// { ... } / { ... }
	if (c1.is_ba_constant() && c2.is_ba_constant()
		&& c1.get_ba_type() > 0 && c2.get_ba_type() == c1.get_ba_type()) {
		DBG(assert(is_bv_type_family<node>(c1.get_ba_type()));)
		return div_consts(std::get<bv>(c1.get_ba_constant()),
					std::get<bv>(c2.get_ba_constant()),
					c1.get_ba_type());
		}
	return symbol;
}

template<NodeType node>
tref term_mod(tref symbol) {
	using tau = tree<node>;
	auto mod_consts = [](const bv& c1, const bv& c2, size_t type_id) {
		bv res = make_bitvector_mod(c1, c2);
		typename node::constant v = {res};
		return tau::build_bf_ba_constant(v, type_id);
	};
	// bf > term symbol > (bf > term symbol) (bf > term_symbol)
	const tau& c1 = tau::get(symbol)[0][0][0];
	const tau& c2 = tau::get(symbol)[0][1][0];
	switch (c1.value.nt) {
		// 1 % X
		case tau::bf_t: {
			if (size_t t = c2.get_ba_type();
				c2.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return mod_consts(
					make_bitvector_top_elem(width),
					std::get<bv>(c2.get_ba_constant()),
					c2.get_ba_type());
			}
			// 1 % 1
			if (c2.is(tau::bf_t)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return mod_consts(
					make_bitvector_top_elem(width),
					make_bitvector_top_elem(width),
					c2.get_ba_type());
			}
			// 1 % 0
			if (c2.is(tau::bf_f)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return mod_consts(
					make_bitvector_top_elem(width),
					make_bitvector_bottom_elem(width),
					c2.get_ba_type());
			}
			break;
		}
		// 0 % X
		case tau::bf_f: return tau::_0(c2.get_ba_type());
		default: break;
	}
	switch (c2.value.nt) {
		// X % 1
		case tau::bf_t: {
			if (size_t t = c1.get_ba_type();
				c1.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return mod_consts(
					std::get<bv>(c1.get_ba_constant()),
					make_bitvector_top_elem(width),
					c1.get_ba_type());
				}
			break;
		}
		// X % 0
		case tau::bf_f: {
			if (size_t t = c1.get_ba_type();
				c1.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return mod_consts(
					std::get<bv>(c1.get_ba_constant()),
					make_bitvector_bottom_elem(width),
					c1.get_ba_type());
				}
			break;
		}
		default: break;
	}
	// { ... } % { ... }
	if (c1.is_ba_constant() && c2.is_ba_constant()
		&& c1.get_ba_type() > 0 && c2.get_ba_type() == c1.get_ba_type()) {
		DBG(assert(is_bv_type_family<node>(c1.get_ba_type()));)
		return mod_consts(std::get<bv>(c1.get_ba_constant()),
					std::get<bv>(c2.get_ba_constant()),
					c1.get_ba_type());
		}
	return symbol;
}

template<NodeType node>
tref term_shr(tref symbol) {
	using tau = tree<node>;
	auto shr_consts = [](const bv& c1, const bv& c2, size_t type_id) {
		bv res = make_bitvector_shr(c1, c2);
		typename node::constant v = {res};
		return tau::build_bf_ba_constant(v, type_id);
	};
	// bf > term symbol > (bf > term symbol) (bf > term_symbol)
	const tau& c1 = tau::get(symbol)[0][0][0];
	const tau& c2 = tau::get(symbol)[0][1][0];
	switch (c1.value.nt) {
		// 1 >> X
		case tau::bf_t: {
			// 1 >> {...}
			if (size_t t = c2.get_ba_type();
				c2.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return shr_consts(
					make_bitvector_top_elem(width),
					std::get<bv>(c2.get_ba_constant()),
					c2.get_ba_type());
			}
			// 1 >> 1
			if (c2.is(tau::bf_t)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return shr_consts(
					make_bitvector_top_elem(width),
					make_bitvector_top_elem(width),
					c2.get_ba_type());
			}
			break;
		}
		// 0 >> X
		case tau::bf_f: {
			// 0 >> {...}
			if (size_t t = c2.get_ba_type();
				c2.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return shr_consts(
					make_bitvector_bottom_elem(width),
					std::get<bv>(c2.get_ba_constant()),
					c2.get_ba_type());
			}
			// 0 >> 1
			if (c2.is(tau::bf_t)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return shr_consts(
					make_bitvector_bottom_elem(width),
					make_bitvector_top_elem(width),
					c2.get_ba_type());
			}
			break;
		}
		default: break;
	}
	switch (c2.value.nt) {
		// X >> 1
		case tau::bf_t: {
			if (size_t t = c1.get_ba_type();
				c1.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return shr_consts(
					std::get<bv>(c1.get_ba_constant()),
					make_bitvector_top_elem(width),
					c1.get_ba_type());
				}
			break;
		}
		// X >> 0
		case tau::bf_f: return tau::get(symbol)[0].first();
		default: break;
	}
	// { ... } >> { ... }
	if (c1.is_ba_constant() && c2.is_ba_constant()
		&& c1.get_ba_type() > 0 && c2.get_ba_type() == c1.get_ba_type()) {
		DBG(assert(is_bv_type_family<node>(c1.get_ba_type()));)
		return shr_consts(std::get<bv>(c1.get_ba_constant()),
					std::get<bv>(c2.get_ba_constant()),
					c1.get_ba_type());
	}
	return symbol;
}

template<NodeType node>
tref term_shl(tref symbol) {
	using tau = tree<node>;
	auto shl_consts = [](const bv& c1, const bv& c2, size_t type_id) {
		bv res = make_bitvector_shl(c1, c2);
		typename node::constant v = {res};
		return tau::build_bf_ba_constant(v, type_id);
	};
	// bf > term symbol > (bf > term symbol) (bf > term_symbol)
	const tau& c1 = tau::get(symbol)[0][0][0];
	const tau& c2 = tau::get(symbol)[0][1][0];
	switch (c1.value.nt) {
		// 1 << X
		case tau::bf_t: {
			// 1 << {...}
			if (size_t t = c2.get_ba_type();
				c2.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return shl_consts(
					make_bitvector_top_elem(width),
					std::get<bv>(c2.get_ba_constant()),
					c2.get_ba_type());
			}
			// 1 << 1
			if (c2.is(tau::bf_t)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return shl_consts(
					make_bitvector_top_elem(width),
					make_bitvector_top_elem(width),
					c2.get_ba_type());
			}
			break;
		}
		// 0 << X
		case tau::bf_f: {
			// 0 << {...}
			if (size_t t = c2.get_ba_type();
				c2.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return shl_consts(
					make_bitvector_bottom_elem(width),
					std::get<bv>(c2.get_ba_constant()),
					c2.get_ba_type());
				}
			// 0 << 1
			if (c2.is(tau::bf_t)) {
				DBG(assert(is_bv_type_family<node>(c2.get_ba_type()));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(c2.get_ba_type()));
				return shl_consts(
					make_bitvector_bottom_elem(width),
					make_bitvector_top_elem(width),
					c2.get_ba_type());
			}
			break;
		}
		default: break;
	}
	switch (c2.value.nt) {
		// X << 1
		case tau::bf_t: {
			if (size_t t = c1.get_ba_type();
				c1.is_ba_constant() && t > 0) {
				DBG(assert(is_bv_type_family<node>(t));)
				const size_t width = get_bv_width<node>(get_ba_type_tree<node>(t));
				return shl_consts(
					std::get<bv>(c1.get_ba_constant()),
					make_bitvector_top_elem(width),
					c1.get_ba_type());
				}
			break;
		}
		// X << 0
		case tau::bf_f: return tau::get(symbol)[0].first();
		default: break;
	}
	// { ... } << { ... }
	if (c1.is_ba_constant() && c2.is_ba_constant()
		&& c1.get_ba_type() > 0 && c2.get_ba_type() == c1.get_ba_type()) {
		DBG(assert(is_bv_type_family<node>(c1.get_ba_type()));)
		return shl_consts(std::get<bv>(c1.get_ba_constant()),
					std::get<bv>(c2.get_ba_constant()),
					c1.get_ba_type());
	}
	return symbol;
}

template<NodeType node>
tref term_nor(tref symbol) {
	using tau = tree<node>;
	tref c1 = tau::get(symbol)[0].first();
	tref c2 = tau::get(symbol)[0].second();
	return tau::build_bf_neg(tau::build_bf_or(c1, c2));
}

template<NodeType node>
tref term_xnor(tref symbol) {
	using tau = tree<node>;
	tref c1 = tau::get(symbol)[0].first();
	tref c2 = tau::get(symbol)[0].second();
	return tau::build_bf_neg(tau::build_bf_xor(c1, c2));
}

template<NodeType node>
tref term_nand(tref symbol) {
	using tau = tree<node>;
	tref c1 = tau::get(symbol)[0].first();
	tref c2 = tau::get(symbol)[0].second();
	return tau::build_bf_neg(tau::build_bf_and(c1, c2));
}

// TODO: make test cases
// 0+1 = x1 :bv && 1+1 = x2 :bv && 1+0 = x3 :bv &&  1+{8} = x4 :bv &&  {8}+1 = x5 :bv &&  {8} + {8} = x6:bv
// 0-1 = x1 :bv && 1-1 = x2 :bv && 1-0 = x3 :bv &&  1-{8} = x4 :bv &&  {8}-1 = x5 :bv &&  {8} - {8} = x6:bv
// 0*1 = x1 :bv && 1*1 = x2 :bv && 1*0 = x3 :bv &&  1*{8} = x4 :bv &&  {8}*1 = x5 :bv &&  {8} * {8} = x6:bv
// 0/1 = x1 :bv && 1/1 = x2 :bv && 1/0 = x3 :bv &&  1/{8} = x4 :bv &&  {8}/1 = x5 :bv &&  {8} / {8} = x6:bv
// 0%1 = x1 :bv && 1%1 = x2 :bv && 1%0 = x3 :bv &&  1%{8} = x4 :bv &&  {8}%1 = x5 :bv &&  {8} % {8} = x6:bv
// 0>>1 = x1 :bv && 1>>1 = x2 :bv && 1>>0 = x3 :bv &&  1>>{8} = x4 :bv &&  {8}>>1 = x5 :bv &&  {8} >> {8} = x6:bv
// 0<<1 = x1 :bv && 1<<1 = x2 :bv && 1<<0 = x3 :bv &&  1<<{8} = x4 :bv &&  {8}<<1 = x5 :bv &&  {8} << {8} = x6:bv

template <typename ...BAs> requires BAsPack<BAs...>
tref base_ba_symbol_simplification(tref symbol, const bv&) {
	using node_t = node<BAs...>;
	using tau = tree<node_t>;
	switch (tau::get(symbol)[0].get_type()) {
		case tau::bf_add: return term_add<node_t>(symbol);
		case tau::bf_sub: return term_sub<node_t>(symbol);
		case tau::bf_mul: return term_mul<node_t>(symbol);
		case tau::bf_div: return term_div<node_t>(symbol);
		case tau::bf_mod: return term_mod<node_t>(symbol);
		case tau::bf_shr: return term_shr<node_t>(symbol);
		case tau::bf_shl: return term_shl<node_t>(symbol);
		case tau::bf_nor: return term_nor<node_t>(symbol);
		case tau::bf_xnor: return term_xnor<node_t>(symbol);
		case tau::bf_nand: return term_nand<node_t>(symbol);
		default: return symbol;
	}
}

template <typename ...BAs> requires BAsPack<BAs...>
tref canonize_associative_commutative_symbol(tref term_tree, auto& excluded) {
	using node_t = node<BAs...>;
	using tau = tree<node_t>;
	using tt = tau::traverser;
	// std::cout << "term_tree: " << tau::get(term_tree) << "\n";
	// We use tau::ctnvar to tag the inverse, since this non-terminal
	// cannot naturally appear there
	auto build_add_inv = [](tref n) {
		return tau::get(tau::bf, tau::get(tau::ctnvar, tau::get(tau::bf_sub), n));
	};
	auto is_add_inv = [](tref n) {
		return (tt(n) | tau::ctnvar | tau::bf_sub).has_value();
	};
	auto build_mult_inv = [](tref n) {
		return tau::get(tau::bf, tau::get(tau::ctnvar, tau::get(tau::bf_div), n));
	};
	auto is_inv = [](tref n) {
		return (tt(n) | tau::ctnvar).has_value();
	};
	auto get_inv_arg = [](tref n) {
		return tau::get(n)[0].second();
	};
	auto is_const = [](tref n) -> bool {
		const tau& c = tau::get(n)[0];
		return c.is(tau::bf_t) || c.is(tau::bf_f) || c.is_ba_constant();
	};
	auto is_inv_const = [&](tref n) -> bool {
		return tau::get(n).child_is(tau::ctnvar) && is_const(get_inv_arg(n));
	};
	auto term_comp = [&](tref l, tref r) -> bool {
		const tau& tl = tau::get(l);
		const tau& tr = tau::get(r);
		// Constants first
		if (tl.equals_0()) return !tau::get(r).equals_0();
		if (tr.equals_0()) return false;
		if (tl.equals_1()) return !tau::get(r).equals_1();
		if (tr.equals_1()) return false;
		if (tl.child_is(tau::ba_constant))
			return !tr.child_is(tau::ba_constant);
		if (tr.child_is(tau::ba_constant)) return false;
		// Inverted constants after constants
		if (is_inv_const(l)) return !is_inv_const(r);
		if (is_inv_const(r)) return false;
		// All non-constant ignore inversion
		if (is_inv(l)) {
			if (is_inv(r)) return tau::get(get_inv_arg(l)) < tau::get(get_inv_arg(r));
			else return tau::get(get_inv_arg(l)) < tau::get(r);
		}
		if (is_inv(r)) return tau::get(l) < tau::get(get_inv_arg(r));
		return tau::get(l) < tau::get(r);
	};
	auto sym = tau::get(term_tree)[0].get_type();
	auto inv_sym = get_inv_sym<BAs...>(sym);
	DBG(assert(is_associative_and_commutative<BAs...>(sym)));
	// Get terms connected by same symbol
	auto clauses = get_leaves<node_t>(term_tree, sym);
	// Use associativity and commutativity to sort
	std::ranges::sort(clauses, term_comp);
	// Build resulting constants
	for (size_t i = 1; i < clauses.size(); ++i) {
		tref& c1 = clauses[i-1];
		tref& c2 = clauses[i];
		if (is_const(c1) && is_const(c2)) {
			c1 = tau::get(tau::bf, tau::get(sym, c1, c2));
			excluded.insert(c1);
			clauses.erase(clauses.begin()+i);
			--i;
		} else if (is_inv_const(c1) && is_const(c2)) {
			tref s = get_inv_arg(c1);
			c1 = tau::get(tau::bf, tau::get(inv_sym, c2, s));
			excluded.insert(c1);
			clauses.erase(clauses.begin()+i);
			--i;
		} else if (is_const(c1) && is_inv_const(c2)) {
			tref s = get_inv_arg(c2);
			c1 = tau::get(tau::bf, tau::get(inv_sym, c1, s));
			excluded.insert(c1);
			clauses.erase(clauses.begin()+i);
			--i;
		} else if (is_inv_const(c1) && is_inv_const(c2)) {
			tref newc = tau::get(tau::bf,
				tau::get(sym, get_inv_arg(c1), get_inv_arg(c2)));
			if (is_add_inv(c1)) {
				c1 = build_add_inv(newc);
			} else c1 = build_mult_inv(newc);
			excluded.insert(c1);
			clauses.erase(clauses.begin()+i);
			--i;
		} else if (is_inv(c1) && tau::get(get_inv_arg(c1)) == tau::get(c2)) {
			tref s = get_inv_arg(c1);
			c1 = tau::get(tau::bf, tau::get(inv_sym, c2, s));
			excluded.insert(c1);
			clauses.erase(clauses.begin()+i);
			--i;
		} else if (is_inv(c2) && tau::get(get_inv_arg(c2)) == tau::get(c1)) {
			tref s = get_inv_arg(c2);
			c1 = tau::get(tau::bf, tau::get(inv_sym, c1, s));
			excluded.insert(c1);
			clauses.erase(clauses.begin()+i);
			--i;
		}
	}
	// We now sort all inversions to the back in order to optimally resolve the temporary
	// inverse construction later
	auto inv_comp = [&](tref l, tref r) -> bool {
		const tau& tl = tau::get(l);
		const tau& tr = tau::get(r);
		// All inversions to back
		if (is_inv(r) || tr.child_is(inv_sym)) {
			return !(is_inv(l) || tl.child_is(inv_sym));
		}
		return false;
	};
	std::ranges::sort(clauses, inv_comp);
	// Avoid parenthesis in output
	for (size_t i = 1; i < clauses.size(); ++i) {
		tref& c1 = clauses[i-1];
		tref& c2 = clauses[i];
		if (!is_inv(c1) && is_inv(c2)) {
			c1 = tau::get(tau::bf, tau::get(sym, c1, c2));
			excluded.insert(c1);
			clauses.erase(clauses.begin()+i);
			--i;
		}
	}
	// Rebuild term_tree
	tref r = clauses[0];
	for (size_t i = 1; i < clauses.size(); ++i) {
		r = tau::get(tau::bf, tau::get(sym, r, clauses[i]));
		// Do not process this node symbol again
		excluded.insert(r);
	}
	return r;
}

template<typename ... BAs> requires BAsPack<BAs...>
tref base_ba_term_simplification(tref term, const bv&) {
	using tau = tree<node<BAs...>>;
	using tt = tau::traverser;
	// TODO: introduce proper symbol for inverse
	// We use tau::ctnvar to tag the inverse, since this non-terminal
	// cannot naturally appear there
	auto build_add_inv = [](tref n) {
		return tau::get(tau::bf, tau::get(tau::ctnvar, tau::get(tau::bf_sub), n));
	};
	auto is_add_inv = [](tref n) {
		return tt(n) | tau::ctnvar | tau::bf_sub;
	};
	auto build_mult_inv = [](tref n) {
		return tau::get(tau::bf, tau::get(tau::ctnvar, tau::get(tau::bf_div), n));
	};
	auto is_mult_inv = [](tref n) {
		return tt(n) | tau::ctnvar | tau::bf_div;
	};
	auto get_inv_arg = [](tref n) {
		return tau::get(n)[0].second();
	};
	// Rename x - y to x + (-y) and x / y to x * (/y)
	auto inv_rename = [&](tref n) {
		if (!tau::get(n).is(tau::bf)) return n;
		const tau& c = tau::get(n)[0];
		if (c.is(tau::bf_sub)) 
			return tau::build_bf_add(c.first(), build_add_inv(c.second()));
		if (c.is(tau::bf_div))
			return tau::build_bf_mul(c.first(), build_mult_inv(c.second())); 
		return n;
	};
	term = pre_order<node<BAs...>>(term).apply_unique(inv_rename);
	// Push inverses in with rules
	// -(X + Y) == -X + -Y and --X == X and
	// (X * Y)^(-1) == X^(-1) * Y^(-1) and X^(-1)^(-1) = X
	auto push_inv_in = [&](tref n) {
		if (!tau::get(n).is(tau::bf)) return n;
		// Treat additive inverse
		if (is_add_inv(n)) {
			const tau& inv_arg = tau::get(get_inv_arg(n));
			if (inv_arg.child_is(tau::bf_add)) {
				tref c1 = inv_arg[0].first();
				tref c2 = inv_arg[0].second();

				if (is_add_inv(c1)) c1 = get_inv_arg(c1);
				else c1 = build_add_inv(c1);
				if (is_add_inv(c2)) c2 = get_inv_arg(c2);
				else c2 = build_add_inv(c2);

				return tau::build_bf_add(c1, c2);
			} else if (is_add_inv(inv_arg.get())) {
				return get_inv_arg(inv_arg.get());
			}
		}
		// Treat multiplicative inverse
		if (is_mult_inv(n)) {
			const tau& inv_arg = tau::get(get_inv_arg(n));
			if (inv_arg.child_is(tau::bf_mul)) {
				tref c1 = inv_arg[0].first();
				tref c2 = inv_arg[0].second();

				if (is_mult_inv(c1)) c1 = get_inv_arg(c1);
				else c1 = build_mult_inv(c1);
				if (is_mult_inv(c2)) c2 = get_inv_arg(c2);
				else c2 = build_mult_inv(c2);

				return tau::build_bf_mul(c1, c2);
			} else if (is_add_inv(inv_arg.get())) {
				return get_inv_arg(inv_arg.get());
			}
		}
		return n;
	};
	term = pre_order<node<BAs...>>(term).apply_unique(push_inv_in);

	// Simplify symbols
	subtree_unordered_set<node<BAs...>> excluded;
	auto simplify = [&excluded](tref n) {
		if (!tau::get(n).is(tau::bf)) return n;
		if (excluded.contains(n)) return n;
		// Get current symbol
		const tau& c = tau::get(n)[0];
		// Simplify associative and commutative symbol
		if (is_associative_and_commutative<BAs...>(c.get_type())) {
			return canonize_associative_commutative_symbol<BAs...>(n, excluded);
		}
		return n;
	};
	term = pre_order<node<BAs...>>(term).apply_unique(simplify);
	// Rename inverses back to available symbols
	auto rename_back = [&](tref n) {
		if (!tau::get(n).is(tau::bf)) return n;
		const tau& c = tau::get(n)[0];
		if (c.is(tau::bf_add)) {
			// Check children for inverses
			if (is_add_inv(c.second())) {
				if (is_add_inv(c.first())) {
					return tau::build_bf_sub(
						tau::build_bf_sub(
							tau::_0(c.get_ba_type()),
							get_inv_arg(c.first())),
							get_inv_arg(c.second()));
				}
				return tau::build_bf_sub(c.first(), get_inv_arg(c.second()));
			}
			if (is_add_inv(c.first())) {
				// Here second argument is not inverse, hence, swap
				return tau::build_bf_sub(c.second(), get_inv_arg(c.first()));
			}
			return n;
		} else if (c.is(tau::bf_mul)) {
			// Check children for inverses
			if (is_mult_inv(c.second())) {
				if (is_mult_inv(c.first())) {
					const size_t width = get_bv_width<node<BAs...>>(c.get_ba_type_tree());
					auto const_1 = tau::build_bf_ba_constant(
						make_bitvector_value(1, width), c.get_ba_type());
					return tau::build_bf_div(
						tau::build_bf_div(
							const_1,
							get_inv_arg(c.first())),
							get_inv_arg(c.second()));
				}
				return tau::build_bf_div(c.first(), get_inv_arg(c.second()));
			}
			if (is_mult_inv(c.first())) {
				// Here second argument is not inverse, hence, swap
				return tau::build_bf_div(c.second(), get_inv_arg(c.first()));
			}
			return n;
		}
		return n;
	};
	term = pre_order<node<BAs...>>(term).apply_unique(rename_back);
	return term;
}

} // namespace idni::tau_lang