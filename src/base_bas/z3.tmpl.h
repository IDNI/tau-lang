// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

namespace idni::tau_lang {

using namespace z3;

static auto BV = z3_context.bv_sort(64);
static auto BOOL = z3_context.bool_sort();

// deactivated for the moment
//static bool _signed = false;

// evaluates a parsed bdd terminal node recursively
template <typename...BAs>
z3::expr eval_z3(const tau<BAs...>& form, std::map<tau<BAs...>, z3::expr>& vars, bool checked) {
	auto nt = std::get<tau_source_sym>(form->value).n();
	// control overflow/underflow if requested
	// deactivated for the moment
	if (checked) {
		BOOST_LOG_TRIVIAL(warning)
			<< "(Warning) overflow/underflow checking is deactivated";
		/*switch (nt) {
			case tau_parser::z3_add: {
				auto l = eval_z3(form->child[0], vars, checked);
				auto r = eval_z3(form->child[1], vars, checked);
				return z3::bvadd_no_overflow(l, r, _signed);
			}
			case tau_parser::z3_sub: {
				auto l = eval_z3(form->child[0], vars, checked);
				auto r = eval_z3(form->child[1], vars, checked);
				return z3::bvsub_no_underflow(l, r, _signed);
			}
			case tau_parser::z3_mul: {
				auto l = eval_z3(form->child[0], vars, checked);
				auto r = eval_z3(form->child[1], vars, checked);
				return z3::bvmul_no_overflow(l, r, _signed);
			}
		}*/
	}
	// otherwise, use modular arithmetic
	switch (nt) {
		// due to hooks we should consider wff_t or bf_t
		case tau_parser::wff:
		case tau_parser::bv: {
			auto expr = eval_z3(form->child[0], vars, checked);
			return expr;
		}
		case tau_parser::wff_neg: {
			return !eval_z3(form->child[0], vars, checked);
		}
		case tau_parser::wff_and: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return l && r;
		}
		case tau_parser::wff_or: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return l || r;
		}
		case tau_parser::wff_all: {
			auto v = form->child[0];
			if (auto it = vars.find(form); it != vars.end()) return it-> second;
			auto vn = make_string(tau_node_terminal_extractor<BAs...>, v);
			auto f = eval_z3(form->child[1], vars, checked);
			auto x = z3_context.bv_const(vn.c_str(), sizeof(size_t) * 8);
			vars.emplace(v, x);
			return forall(x, f);
		}
		case tau_parser::wff_ex: {
			auto v = form->child[0];
			if (auto it = vars.find(form); it != vars.end()) return it-> second;
			auto vn = make_string(tau_node_terminal_extractor<BAs...>, v);
			auto f = eval_z3(form->child[1], vars, checked);
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
		case tau_parser::bv_checked: {
			return eval_z3(form->child[0], vars, true);
		}
		case tau_parser::bv_eq: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return l == r;
		};
		case tau_parser::bv_neq: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return l != r;
		}
		case tau_parser::bv_less_equal: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return l <= r;
		}
		case tau_parser::bv_nleq: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return !(l <= r);
		}
		case tau_parser::bv_greater: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return l > r;
		}
		case tau_parser::bv_ngreater: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return !(l > r);
		}
		case tau_parser::bv_greater_equal: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return l >= r;
		}
		case tau_parser::bv_ngeq: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return !(l >= r);
		}
		case tau_parser::bv_less: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return l < r;
		}
		case tau_parser::bv_nless: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return !(l < r);
		}
		case tau_parser::bv_neg: {
			auto l = eval_z3(form->child[0], vars, checked);
			return -l;
		}
		case tau_parser::bv_add: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return l + r;
		}
		case tau_parser::bv_sub: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return l - r;
		}
		case tau_parser::bv_mul: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return l * r;
		}
		case tau_parser::bv_div: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return l / r;
		}
		case tau_parser::bv_mod: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return l % r;
		}
		case tau_parser::bv_and: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return l & r;
		}
		case tau_parser::bv_nand: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return nand(l, r);
		}
		case tau_parser::bv_or: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return l | r;
		}
		case tau_parser::bv_nor: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return nor(l, r);
		}
		case tau_parser::bv_xor: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return l ^ r;
		}
		case tau_parser::bv_xnor: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return xnor(l, r);
		}
		case tau_parser::bv_min: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return min(l, r);
		}
		case tau_parser::bv_max: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto r = eval_z3(form->child[1], vars, checked);
			return max(l, r);
		}
		case tau_parser::bv_rotate_left: {
			auto l = eval_z3(form->child[0], vars, checked);
			auto value = make_string(tau_node_terminal_extractor<BAs...>, form->child[1]);
			// this is the type of rotate_left
			unsigned int ul = std::stoul(value);
			return l.rotate_left(ul);
		}
		case tau_parser::bv_rotate_right: {
			auto l = eval_z3(form->child[0], vars, checked);
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
			#ifdef DEBUG
			BOOST_LOG_TRIVIAL(error) << "(Error) unknow z3 non-terminal: " << nt;
			#endif // DEBUG
		}
	}
	assert(false);
}


template <typename...BAs>
bool is_z3_formula_sat(const tau<BAs...>& form) {
	z3::solver solver(z3_context);
	std::map<tau<BAs...>, z3::expr> vars;
	auto expr = eval_z3(form, vars);
	// solve the equations
	solver.add(expr);

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(info)
		<< "(Info) z3 solver\n" << solver;
	#endif // DEBUG

	auto result = solver.check() == z3::sat;

	#ifdef DEBUG
	if (result) {
		BOOST_LOG_TRIVIAL(info)
			<< "(Info) z3 system is sat\n"
			<< "(Info) z3 model\n" << solver.get_model();
	} else {
		BOOST_LOG_TRIVIAL(info)
			<< "(Info) z3 system is unsat\n";
	}
	#endif // DEBUG

	return result;
}


template<typename...BAs>
std::optional<solution<BAs...>> solve_z3(const tau<BAs...>& form, z3::solver& solver) {
	std::map<tau<BAs...>, z3::expr> vars;
	auto expr = eval_z3(form, vars);
	// solve the equations
	solver.add(expr);

	#ifdef DEBUG
	BOOST_LOG_TRIVIAL(info)
	<< "(Info) z3 solver\n" << solver;
	#endif // DEBUG

	auto result = solver.check();
	// extract the model and return the solution if sat
	if (result == z3::sat) {

		#ifdef DEBUG
		BOOST_LOG_TRIVIAL(info)
			<< "(Info) z3 system is sat\n"
			<< "(Info) z3 model\n" << solver.get_model();
		#endif // DEBUG

		solution<BAs...> s;
		for (const auto& v: vars) {
			auto bvn = make_node<tau_sym<BAs...>>(solver.get_model().eval(v.second), {});
			std::vector<tau<BAs...>> bvv{bvn};
			s.emplace(v.first,
				std::make_shared<rewriter::node<tau_sym<BAs...>>>(
					tau_parser::instance().literal(tau_parser::bitvector), bvv));
				}
				return s;
			} else {

				#ifdef DEBUG
				BOOST_LOG_TRIVIAL(info) << "(Info) z3 system is unsat";
				#endif // DEBUG

			}
			return {};
		}

template<typename...BAs>
std::optional<solution<BAs...>> solve_z3(const tau<BAs...>& form) {
	z3::solver solver(z3_context);
	return solve_z3(form, solver);
}

} // namespace idni::tau_lang