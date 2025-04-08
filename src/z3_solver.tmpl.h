// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "z3_context.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

namespace idni::tau_lang {

using namespace z3;

// evaluates a parsed bdd terminal node recursively
template <typename...BAs>
z3::expr eval_z3(const tau<BAs...>& form, std::map<tau<BAs...>, z3::expr>& vars) {
	// at this level we traverse the wff nodes
	if (is_non_terminal_node(form)) {
		auto nt = std::get<tau_source_sym>(form->value).n();
		switch (nt) {
			// due to hooks we should consider wff_t or bf_t
			case tau_parser::wff:
			case tau_parser::z3: {
				auto expr = eval_z3(form->child[0], vars);
				return expr;
			}
			case tau_parser::wff_neg: {
				return !eval_z3(form->child[0], vars);
			}
			case tau_parser::wff_and: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return l && r;
			}
			case tau_parser::wff_or: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return l || r;
			}
			case tau_parser::wff_all: {
				auto v = form->child[0];
				if (auto it = vars.find(form); it != vars.end()) return it-> second;
				auto vn = make_string(tau_node_terminal_extractor<BAs...>, v);
				auto f = eval_z3(form->child[1], vars);
				auto x = z3_context.bv_const(vn.c_str(), sizeof(size_t) * 8);
				vars.emplace(v, x);
				return forall(x, f);
			}
			case tau_parser::wff_ex: {
				auto v = form->child[0];
				if (auto it = vars.find(form); it != vars.end()) return it-> second;
				auto vn = make_string(tau_node_terminal_extractor<BAs...>, v);
				auto f = eval_z3(form->child[1], vars);
				auto x = z3_context.bv_const(vn.c_str(), sizeof(size_t) * 8);
				vars.emplace(v, x);
				return exists(x, f);
			}
			case tau_parser::variable: {
				// check if the variable is alr
				if (auto it = vars.find(form); it != vars.end()) return it->second;
				auto v = make_string(tau_node_terminal_extractor<BAs...>, form);
				// create a new constant according to the type and added to the map
				auto x = z3_context.bv_const(v.c_str(), sizeof(size_t) * 8);
				vars.emplace(form, x);
				return x;
			}
			case tau_parser::z3_eq: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return l == r;
			};
			case tau_parser::z3_neq: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return l != r;
			}
			case tau_parser::z3_less_equal: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return l <= r;
			}
			case tau_parser::z3_nleq: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return !(l <= r);
			}
			case tau_parser::z3_greater: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return l > r;
			}
			case tau_parser::z3_ngreater: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return !(l > r);
			}
			case tau_parser::z3_greater_equal: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return l >= r;
			}
			case tau_parser::z3_ngeq: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return !(l >= r);
			}
			case tau_parser::z3_less: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return l < r;
			}
			case tau_parser::z3_nless: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return !(l < r);
			}
			case tau_parser::z3_neg: {
				auto l = eval_z3(form->child[0], vars);
				return -l;
			}
			case tau_parser::z3_add: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return l + r;
			}
			case tau_parser::z3_sub: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return l - r;
			}
			case tau_parser::z3_mul: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return l * r;
			}
			case tau_parser::z3_div: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return l / r;
			}
			case tau_parser::z3_mod: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return l % r;
			}
			case tau_parser::z3_and: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return l & r;
			}
			case tau_parser::z3_nand: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return nand(l, r);
			}
			case tau_parser::z3_or: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return l | r;
			}
			case tau_parser::z3_nor: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return nor(l, r);
			}
			case tau_parser::z3_xor: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return l ^ r;
			}
			case tau_parser::z3_xnor: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return xnor(l, r);
			}
			case tau_parser::z3_min: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return min(l, r);
			}
			case tau_parser::z3_max: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				return max(l, r);
			}
			case tau_parser::z3_rotate_left: {
				auto l = eval_z3(form->child[0], vars);
				auto value = make_string(tau_node_terminal_extractor<BAs...>, form->child[1]);
				// this is the type of rotate_left
				unsigned int ul = std::stoul(value);
				return l.rotate_left(ul);
			}
			case tau_parser::z3_rotate_right: {
				auto l = eval_z3(form->child[0], vars);
				auto value = make_string(tau_node_terminal_extractor<BAs...>, form->child[1]);
				// this is the type of rotate_left
				unsigned int ul = std::stoul(value);
				return l.rotate_right(ul);
			}
			case tau_parser::bitvector: {
				auto expr = std::get<z3::expr>(form->child[0]->value);
				return expr;
			}
			default: {
				std::cout << "Unknown node type: " << nt << "\n";
			}
		}
	}

	std::cout << form;
}

template<typename...BAs>
std::optional<solution<BAs...>> solve_z3(const tau<BAs...>& form) {
	// transform form into a z3::expr
	z3::solver solver(z3_context);
	std::map<tau<BAs...>, z3::expr> vars;
	auto expr = eval_z3(form, vars);
	// solve the equations
	solver.add(expr);
	auto result = solver.check();
	// extract the model and return the solution if sat
	if (result == z3::sat) {
		solution<BAs...> solution;
		for (const auto& v: vars) {
			auto bvn = make_node<tau_sym<BAs...>>(solver.get_model().eval(v.second), {});
			std::vector<tau<BAs...>> bvv{bvn};
			solution.emplace(v.first,
				std::make_shared<rewriter::node<tau_sym<BAs...>>>(
					tau_parser::instance().literal(tau_parser::bitvector), bvv));
		}
		return solution;
	}
	return {};
}

} // namespace idni::tau_lang