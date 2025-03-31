// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "z3_context.h"

namespace idni::tau_lang {

// evaluates a parsed bdd terminal node recursively
template <typename...BAs>
std::optional<z3::expr> eval_z3(const tau<BAs...>& form, std::map<tau<BAs...>, z3::expr>& vars) {
	// at this level we traverse the wff nodes
	if (is_non_terminal_node(form)) {
		auto nt = std::get<tau_source_sym>(form->value).n();
		switch (nt) {
			case tau_parser::wff: return eval_z3(form->child[0], vars);
			case tau_parser::wff_neg: {
				auto c = eval_z3(form->child[0], vars);
				if (!c.has_value()) return {};
				return !c.value();
			}
			case tau_parser::wff_and: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				if (!l.has_value() || !r.has_value()) return {};
				return l.value() && r.value();
			}
			case tau_parser::wff_or: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[1], vars);
				if (!l.has_value() || !r.has_value()) return {};
				return l.value() || r.value();
			}
			case tau_parser::wff_all: {
				auto v = form->child[0];
				auto vn = make_string(tau_node_terminal_extractor<BAs...>, v);
				auto f = eval_z3(form->child[1], vars);
				if (!f.has_value()) return {};
				// create a new constant according to the type and added to the map
				vars.emplace(v, z3_context.bool_const(vn.c_str()));
				return forall(vars.at(v), f.value());
			}
			case tau_parser::wff_ex: {
				auto v = form->child[0];
				auto vn = make_string(tau_node_terminal_extractor<BAs...>, v);
				auto f = eval_z3(form->child[1], vars);
				if (!f.has_value()) return {};
				// create a new constant according to the type and added to the map
				vars.emplace(v, z3_context.bool_const(vn.c_str()));
				return exists(vars.at(v), f.value());
			}
			case tau_parser::wff_t:
			case tau_parser::wff_f:
				#ifdef DEBUG
				BOOST_LOG_TRIVIAL(error)
					<< " solver.tmpl.h:" << __LINE__ << " eval_z3/constant: wff_t/wff_f";
				#endif // DEBUG
				return {};
			case tau_parser::z3_eq: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[0], vars);
				if (!l.has_value() || !r.has_value()) return {};
				return l.value() == r.value();
			};
			case tau_parser::z3_neq: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[0], vars);
				if (!l.has_value() || !r.has_value()) return {};
				return l.value() != r.value();
			}
			case tau_parser::z3_less_equal: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[0], vars);
				if (!l.has_value() || !r.has_value()) return {};
				return l.value() <= r.value();
			}
			case tau_parser::z3_nleq: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[0], vars);
				if (!l.has_value() || !r.has_value()) return {};
				return l.value() > r.value();
			}
			case tau_parser::z3_greater: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[0], vars);
				if (!l.has_value() || !r.has_value()) return {};
				return l.value() > r.value();
			}
			case tau_parser::z3_ngreater: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[0], vars);
				if (!l.has_value() || !r.has_value()) return {};
				return l.value() <= r.value();
			}
			case tau_parser::z3_greater_equal: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[0], vars);
				if (!l.has_value() || !r.has_value()) return {};
				return l.value() >= r.value();
			}
			case tau_parser::z3_ngeq: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[0], vars);
				if (!l.has_value() || !r.has_value()) return {};
				return l.value() < r.value();
			}
			case tau_parser::z3_less: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[0], vars);
				if (!l.has_value() || !r.has_value()) return {};
				return l.value() < r.value();
			}
			case tau_parser::z3_nless: {
				auto l = eval_z3(form->child[0], vars);
				auto r = eval_z3(form->child[0], vars);
				if (!l.has_value() || !r.has_value()) return {};
				return l.value() >= r.value();
			}
			case tau_parser::z3_constant: {
				return std::get<z3::expr>(form->child[0]->value);
			}
			default: return {};
		}
	}
}

template<typename...BAs>
std::optional<solution<BAs...>> solve_z3(const tau<BAs...>& form) {
	// transform form into a z3::expr
	z3::solver solver(z3_context);
	std::map<tau<BAs...>, z3::expr> vars;
	auto expr = eval_z3(form, vars);
	if (!expr.has_value()) return {};
	solver.add(expr.value());
	auto result = solver.check();
	// extract the model and return the solution
	if (result == z3::sat) {
		auto model = solver.get_model();
		solution<BAs...> solution;
		for (const auto& [k, v]: vars) {
			auto val = model.eval(v);
			solution.emplace(k, make_node<tau_sym<BAs...>>(val, {}));
		}
		return solution;
	}
	return {};
}

} // namespace idni::tau_lang