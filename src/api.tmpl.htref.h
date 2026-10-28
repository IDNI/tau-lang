// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "api.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "api"

namespace idni::tau_lang {

// Helper functions
// ------------------------------------------------------------

// Lifts a tref->tref map (e.g. a solver solution) into owning htref
// handles, so the result survives a tree sweep.
template <NodeType node>
std::map<htref, htref> geth(const subtree_map<node, tref>& m) {
	std::map<htref, htref> hm;
	for (auto [k, v] : m) hm[tree<node>::geth(k)] = tree<node>::geth(v);
	return hm;
}

// Lifts a value of a tref overload into what its htref overload gives: a
// tree into a handle, a solution into handles, any other value unchanged.
template <NodeType node, typename T>
auto lift_htref_value(const T& v) {
	if constexpr (std::is_same_v<T, tref>) return tree<node>::geth(v);
	else if constexpr (std::is_same_v<T, subtree_map<node, tref>>)
		return geth<node>(v);
	else return v;
}

// The body of an htref overload: @p f, the tref overload, on the trees
// @p handles hold, its value lifted into handles; a null handle is an
// invalid_argument error.
template <NodeType node, typename F, typename... H>
auto lift_htref(F&& f, const H&... handles) {
	using value_t = typename std::invoke_result_t<F,
		std::conditional_t<true, tref, H>...>::value_type;
	using lifted_t = decltype(lift_htref_value<node>(
		std::declval<value_t>()));
	return with_budget<node>([&]() -> result<lifted_t> {
		if ((!handles || ...)) {
			result<lifted_t> r;
			return r.with_assert_check_error(code::invalid_argument,
				messages::invalid_arguments);
		}
		return f(handles->get()...).transform([](const value_t& v) {
			return lift_htref_value<node>(v);
		});
	});
}

// ------------------------------------------------------------
// htref API — GC-safe handle wrappers
// ------------------------------------------------------------
// Each method delegates to the corresponding tref overload and wraps
// the result with tau::geth() to produce a GC-safe shared-pointer handle.
// A null handle argument is an invalid_argument error (false for the
// boolean queries).

// Parsing
// ------------------------------------------------------------

template <NodeType node>
result<htref> api<node>::geth_term(const std::string& input, bool simplified) {
	return with_budget<node>([&] {
		return get_term(input, simplified).transform(
			[](tref v) { return tau::geth(v); });
	});
}

template <NodeType node>
result<htref> api<node>::geth_formula(const std::string& input, bool simplified) {
	return with_budget<node>([&] {
		return get_formula(input, simplified).transform(
			[](tref v) { return tau::geth(v); });
	});
}

template <NodeType node>
result<htref> api<node>::geth_function_def(const std::string& function_def, bool simplified) {
	return with_budget<node>([&] {
		return get_function_def(function_def, simplified).transform(
			[](tref v) { return tau::geth(v); });
	});
}

template <NodeType node>
result<htref> api<node>::geth_predicate_def(const std::string& predicate_def, bool simplified) {
	return with_budget<node>([&] {
		return get_predicate_def(predicate_def, simplified).transform(
			[](tref v) { return tau::geth(v); });
	});
}

template <NodeType node>
result<htref> api<node>::geth_stream_def(const std::string& stream_def) {
	return with_budget<node>([&] {
		return get_stream_def(stream_def).transform(
			[](tref v) { return tau::geth(v); });
	});
}

template <NodeType node>
result<htref> api<node>::geth_spec(const std::string& src) {
	return with_budget<node>([&] {
		return get_spec(src).transform(
			[](tref v) { return tau::geth(v); });
	});
}

template <NodeType node>
result<htref> api<node>::geth_definition(const std::string& definition, bool simplified) {
	return with_budget<node>([&] {
		return get_definition(definition, simplified).transform(
			[](tref v) { return tau::geth(v); });
	});
}

template <NodeType node>
result<htref> api<node>::geth_spec_or_term(const std::string& expr, bool simplified) {
	return with_budget<node>([&] {
		return get_spec_or_term(expr, simplified).transform(
			[](tref v) { return tau::geth(v); });
	});
}

template <NodeType node>
result<htref> api<node>::geth_formula_or_term(const std::string& expr, bool simplified) {
	return with_budget<node>([&] {
		return get_formula_or_term(expr, simplified).transform(
			[](tref v) { return tau::geth(v); });
	});
}

// Querying
// ------------------------------------------------------------

template <NodeType node>
bool api<node>::contains(htref expr, typename node::type nt) {
	if (!expr) return false;
	return contains(expr->get(), nt);
}

template <NodeType node>
bool api<node>::is_term(htref term) {
	if (!term) return false;
	return is_term(term->get());
}

template <NodeType node>
bool api<node>::is_formula(htref fm) {
	if (!fm) return false;
	return is_formula(fm->get());
}

// Using definitions
// ------------------------------------------------------------

template <NodeType node>
result<htref> api<node>::apply_def(htref def, htref expression) {
	return lift_htref<node>([](tref def, tref expression) {
		return apply_def(def, expression); }, def, expression);
}

template <NodeType node>
result<htref> api<node>::apply_defs(const std::set<htref>& defs, htref expression) {
	return with_budget<node>([&] {
		if (!expression) {
			result<htref> r;
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		}
		subtree_set<node> tdefs;
		for (htref def : defs) if (def) tdefs.insert(def->get());
		return apply_defs(tdefs, expression->get()).transform(
			[](tref v) { return tau::geth(v); });
	});
}

template <NodeType node>
result<htref> api<node>::apply_all_defs(htref expr) {
	return lift_htref<node>([](tref expr) {
		return apply_all_defs(expr); }, expr);
}


// Printing
// ------------------------------------------------------------

template <NodeType node>
std::ostream& api<node>::print(std::ostream& os, htref expression) {
	if (!expression) return os;
	return print(os, expression->get());
}

template <NodeType node>
std::string api<node>::to_str(htref expression) {
	if (!expression) return {};
	return to_str(expression->get());
}


// Substitution
// ------------------------------------------------------------

template <NodeType node>
result<htref> api<node>::substitute(htref expr, htref that, htref with) {
	return lift_htref<node>([](tref expr, tref that, tref with) {
		return substitute(expr, that, with); }, expr, that, with);
}

template <NodeType node>
result<htref> api<node>::substitute(htref expr, std::map<htref, htref> that_with) {
	return with_budget<node>([&] {
		result<htref> r;
		if (!expr) {
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		}
		// Unwrap htref to tref, then apply all pairs in one simultaneous
		// pass -- matches the tref overload's semantics (see api.tmpl.h).
		std::map<tref, tref> unwrapped;
		for (auto [that, with] : that_with) {
			if (!that || !with) {
				return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
			}
			unwrapped.emplace(that->get(), with->get());
		}
		return substitute(expr->get(), unwrapped).transform(
			[](tref v) { return tau::geth(v); });
	});
}

// Normal forms
// ------------------------------------------------------------

template <NodeType node>
result<htref> api<node>::boole_normal_form(htref expr) {
	return lift_htref<node>([](tref expr) {
		return boole_normal_form(expr); }, expr);
}

template <NodeType node>
result<htref> api<node>::dnf(htref expr) {
	return lift_htref<node>([](tref expr) {
		return dnf(expr); }, expr);
}

template <NodeType node>
result<htref> api<node>::cnf(htref expr) {
	return lift_htref<node>([](tref expr) {
		return cnf(expr); }, expr);
}

template <NodeType node>
result<htref> api<node>::nnf(htref expr) {
	return lift_htref<node>([](tref expr) {
		return nnf(expr); }, expr);
}

template <NodeType node>
result<htref> api<node>::onf(htref expr, htref var) {
	return lift_htref<node>([](tref expr, tref var) {
		return onf(expr, var); }, expr, var);
}

template <NodeType node>
result<htref> api<node>::pnf(htref expr) {
	return lift_htref<node>([](tref expr) {
		return pnf(expr); }, expr);
}

template <NodeType node>
result<htref> api<node>::mnf(htref expr) {
	return lift_htref<node>([](tref expr) {
		return mnf(expr); }, expr);
}

// Procedures
// ------------------------------------------------------------

template <NodeType node>
result<htref> api<node>::syntactic_term_simplification(htref term) {
	return lift_htref<node>([](tref term) {
		return syntactic_term_simplification(term); }, term);
}

template <NodeType node>
result<htref> api<node>::syntactic_formula_simplification(htref fm) {
	return lift_htref<node>([](tref fm) {
		return syntactic_formula_simplification(fm); }, fm);
}

template <NodeType node>
result<htref> api<node>::normalize_formula(htref fm) {
	return lift_htref<node>([](tref fm) {
		return normalize_formula(fm); }, fm);
}

template <NodeType node>
result<htref> api<node>::normalize_term(htref term) {
	return lift_htref<node>([](tref term) {
		return normalize_term(term); }, term);
}

template <NodeType node>
result<htref> api<node>::anti_prenex(htref fm) {
	return lift_htref<node>([](tref fm) {
		return anti_prenex(fm); }, fm);
}

template <NodeType node>
result<htref> api<node>::eliminate_quantifiers(htref fm) {
	return lift_htref<node>([](tref fm) {
		return eliminate_quantifiers(fm); }, fm);
}

template <NodeType node>
result<bool> api<node>::realizable(htref fm) {
	return lift_htref<node>([](tref fm) {
		return realizable(fm); }, fm);
}

template <NodeType node>
result<bool> api<node>::unrealizable(htref fm) {
	return lift_htref<node>([](tref fm) {
		return unrealizable(fm); }, fm);
}

template <NodeType node>
result<bool> api<node>::sat(htref fm) {
	return lift_htref<node>([](tref fm) {
		return sat(fm); }, fm);
}

template <NodeType node>
result<bool> api<node>::unsat(htref fm) {
	return lift_htref<node>([](tref fm) {
		return unsat(fm); }, fm);
}

template <NodeType node>
result<bool> api<node>::valid(htref fm) {
	return lift_htref<node>([](tref fm) {
		return valid(fm); }, fm);
}

template <NodeType node>
result<bool> api<node>::valid_spec(htref fm) {
	return lift_htref<node>([](tref fm) {
		return valid_spec(fm); }, fm);
}

// Solving
// ------------------------------------------------------------

template <NodeType node>
result<std::map<htref, htref>> api<node>::solve(htref fm,
	solver_mode mode)
{
	return with_budget<node>([&] {
		if (!fm) {
			result<std::map<htref, htref>> r;
			return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
		}
		return solve(fm->get(), mode).transform(
			[](const subtree_map<node, tref>& m) { return geth<node>(m); });
	});
}

template <NodeType node>
result<std::map<htref, htref>> api<node>::lgrs(htref equation) {
	return lift_htref<node>([](tref equation) {
		return lgrs(equation); }, equation);
}

template <NodeType node>
result<htref> api<node>::simplify(htref expr, bool use_defaults) {
	return lift_htref<node>([use_defaults](tref expr) {
		return simplify(expr, use_defaults); }, expr);
}

} // namespace idni::tau_lang
