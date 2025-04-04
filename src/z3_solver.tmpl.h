// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "z3_context.h"

namespace idni::tau_lang {

using namespace z3;

// evaluates a parsed bdd terminal node recursively
template <typename...BAs>
z3::expr eval_z3(const tau<BAs...>& form, z3::expr_vector& vars) {
	// at this level we traverse the wff nodes
	if (is_non_terminal_node(form)) {
		auto nt = std::get<tau_source_sym>(form->value).n();
		switch (nt) {
			case tau_parser::wff:
			case tau_parser::z3:
				return eval_z3(form->child[0], vars);
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
				auto vn = make_string(tau_node_terminal_extractor<BAs...>, v);
				auto f = eval_z3(form->child[1], vars);
				auto x = z3_context.bv_const(vn.c_str(), 32);
				vars.push_back(x);
				return forall(x, f);
			}
			case tau_parser::wff_ex: {
				auto v = form->child[0];
				auto vn = make_string(tau_node_terminal_extractor<BAs...>, v);
				auto f = eval_z3(form->child[1], vars);
				auto x = z3_context.bv_const(vn.c_str(), 32);
				vars.push_back(x);
				return exists(x, f);
			}
/*			case tau_parser::wff_t:
			case tau_parser::wff_f:
				#ifdef DEBUG
				BOOST_LOG_TRIVIAL(error)
					<< " solver.tmpl.h:" << __LINE__ << " eval_z3/constant: wff_t/wff_f";
				#endif // DEBUG
				return {};*/
			case tau_parser::variable: {
				// check if the variable is already in the map
				/*auto it = vars.find(form);
				if (it != vars.end()) {
					return it->second;
				} else {*/
					auto v = make_string(tau_node_terminal_extractor<BAs...>, form);
					// create a new constant according to the type and added to the map
					auto x = z3_context.bv_const(v.c_str(), 32);
					vars.push_back(x);
					return x;
				//}
			}
			case tau_parser::z3_eq: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[0], vars);
				return l == r;
			};
			case tau_parser::z3_neq: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[0], vars);
				return l != r;
			}
			case tau_parser::z3_less_equal: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[0], vars);
				return l <= r;
			}
			case tau_parser::z3_nleq: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[0], vars);
				return l > r;
			}
			case tau_parser::z3_greater: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[0], vars);
				return l > r;
			}
			case tau_parser::z3_ngreater: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[0], vars);
				return l <= r;
			}
			case tau_parser::z3_greater_equal: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[0], vars);
				return l >= r;
			}
			case tau_parser::z3_ngeq: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[0], vars);
				return l < r;
			}
			case tau_parser::z3_less: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[0], vars);
				return l < r;
			}
			case tau_parser::z3_nless: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[0], vars);
				return l >= r;
			}
			case tau_parser::bitvector: {
				return std::get<z3::expr>(form->child[0]->value);
			}
		}
	}
}

template<typename...BAs>
std::optional<z3_solution<BAs...>> solve_z3(const tau<BAs...>& form) {
	// transform form into a z3::expr
	z3::solver solver(z3_context);
	z3::expr_vector vars(z3_context);
	auto expr = eval_z3(form, vars);
	solver.add(expr);
	auto result = solver.check();
	// extract the model and return the solution
	if (result == z3::sat) {
		auto model = solver.get_model();
		z3_solution<BAs...> solution(model, vars);
		//for (const auto& v: vars) {
			//std::cout << model << "\n";
			//auto expr = z3_context.bv_const(k.c_str(), 32);
			//auto val = model.eval(expr);
			//solution.emplace(k, make_node<tau_sym<BAs...>>(make_shared<z3::expr>(val), {}));
		//}
		return solution;
	}
	return {};
}

} // namespace idni::tau_lang