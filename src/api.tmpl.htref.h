// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "api.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "api"

namespace idni::tau_lang {

// Helper functions
// ------------------------------------------------------------

/// Convert a subtree_map<node,tref> to a map<htref,htref> by wrapping
/// every key and value with tree<node>::geth() for GC safety.
template <NodeType node>
std::map<htref, htref> geth(const subtree_map<node, tref>& m) {
	std::map<htref, htref> hm;
	for (auto [k, v] : m) hm[tree<node>::geth(k)] = tree<node>::geth(v);
	return hm;
}

// ------------------------------------------------------------
// htref API — GC-safe handle wrappers
// ------------------------------------------------------------
// Each method delegates to the corresponding tref overload and wraps
// the result with tau::geth() to produce a GC-safe shared-pointer handle.

// Parsing
// ------------------------------------------------------------

template <NodeType node>
result<htref> api<node>::geth_term(const std::string& input, bool simplified) {
	return get_term(input, simplified).transform(
		[](tref v) { return tau::geth(v); });
}

template <NodeType node>
result<htref> api<node>::geth_formula(const std::string& input, bool simplified) {
	return get_formula(input, simplified).transform(
		[](tref v) { return tau::geth(v); });
}

template <NodeType node>
result<htref> api<node>::geth_function_def(const std::string& function_def, bool simplified) {
	return get_function_def(function_def, simplified).transform(
		[](tref v) { return tau::geth(v); });
}

template <NodeType node>
result<htref> api<node>::geth_predicate_def(const std::string& predicate_def, bool simplified) {
	return get_predicate_def(predicate_def, simplified).transform(
		[](tref v) { return tau::geth(v); });
}

template <NodeType node>
result<htref> api<node>::geth_stream_def(const std::string& stream_def) {
	return get_stream_def(stream_def).transform(
		[](tref v) { return tau::geth(v); });
}

template <NodeType node>
result<htref> api<node>::geth_spec(const std::string& src) {
	return get_spec(src).transform(
		[](tref v) { return tau::geth(v); });
}

template <NodeType node>
result<htref> api<node>::geth_definition(const std::string& definition, bool simplified) {
	return get_definition(definition, simplified).transform(
		[](tref v) { return tau::geth(v); });
}

template <NodeType node>
result<htref> api<node>::geth_spec_or_term(const std::string& expr, bool simplified) {
	return get_spec_or_term(expr, simplified).transform(
		[](tref v) { return tau::geth(v); });
}

template <NodeType node>
result<htref> api<node>::geth_formula_or_term(const std::string& expr, bool simplified) {
	return get_formula_or_term(expr, simplified).transform(
		[](tref v) { return tau::geth(v); });
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
	if (!def || !expression) {
		result<htref> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return apply_def(def->get(), expression->get()).transform(
		[](tref v) { return tau::geth(v); });
}

template <NodeType node>
result<htref> api<node>::apply_defs(const std::set<htref>& defs, htref expression) {
	if (!expression) {
		result<htref> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	subtree_set<node> tdefs;
	for (htref def : defs) if (def) tdefs.insert(def->get());
	return apply_defs(tdefs, expression->get()).transform(
		[](tref v) { return tau::geth(v); });
}

template <NodeType node>
result<htref> api<node>::apply_all_defs(htref expr) {
	if (!expr) {
		result<htref> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return apply_all_defs(expr->get()).transform(
		[](tref v) { return tau::geth(v); });
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
	if (!expr || !that || !with) {
		result<htref> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return substitute(expr->get(), that->get(), with->get()).transform(
		[](tref v) { return tau::geth(v); });
}

template <NodeType node>
result<htref> api<node>::substitute(htref expr, std::map<htref, htref> that_with) {
	result<htref> r;
	if (!expr) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	// Apply substitutions sequentially; each step unwraps htref to tref
	tref e = expr->get();
	for (auto [that, with] : that_with) {
		if (!that || !with) continue;
		auto sub = r.merge_take(substitute(e, that->get(), with->get()));
		if (!sub) {
			DBG(assert(r.is_well_formed());)
			return r;
		}
		e = *sub;
	}
	r = tau::geth(e);
	DBG(assert(r.is_well_formed());)
	return r;
}

// Normal forms
// ------------------------------------------------------------

template <NodeType node>
result<htref> api<node>::boole_normal_form(htref expr) {
	if (!expr) {
		result<htref> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return boole_normal_form(expr->get()).transform(
		[](tref v) { return tau::geth(v); });
}

template <NodeType node>
result<htref> api<node>::dnf(htref expr) {
	if (!expr) {
		result<htref> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return dnf(expr->get()).transform(
		[](tref v) { return tau::geth(v); });
}

template <NodeType node>
result<htref> api<node>::cnf(htref expr) {
	if (!expr) {
		result<htref> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return cnf(expr->get()).transform(
		[](tref v) { return tau::geth(v); });
}

template <NodeType node>
result<htref> api<node>::nnf(htref expr) {
	if (!expr) {
		result<htref> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return nnf(expr->get()).transform(
		[](tref v) { return tau::geth(v); });
}

// Procedures
// ------------------------------------------------------------

template <NodeType node>
result<htref> api<node>::syntactic_term_simplification(htref term) {
	if (!term) {
		result<htref> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return syntactic_term_simplification(term->get()).transform(
		[](tref v) { return tau::geth(v); });
}

template <NodeType node>
result<htref> api<node>::syntactic_formula_simplification(htref fm) {
	if (!fm) {
		result<htref> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return syntactic_formula_simplification(fm->get()).transform(
		[](tref v) { return tau::geth(v); });
}

template <NodeType node>
result<htref> api<node>::normalize_formula(htref fm) {
	if (!fm) {
		result<htref> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return normalize_formula(fm->get()).transform(
		[](tref v) { return tau::geth(v); });
}

template <NodeType node>
result<htref> api<node>::normalize_term(htref term) {
	if (!term) {
		result<htref> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return normalize_term(term->get()).transform(
		[](tref v) { return tau::geth(v); });
}

template <NodeType node>
result<htref> api<node>::anti_prenex(htref fm) {
	if (!fm) {
		result<htref> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return anti_prenex(fm->get()).transform(
		[](tref v) { return tau::geth(v); });
}

template <NodeType node>
result<htref> api<node>::eliminate_quantifiers(htref fm) {
	if (!fm) {
		result<htref> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	// Apply defs at the tref level before eliminating quantifiers
	// (the tref overload also calls apply_all_defs internally)
	return apply_all_defs(fm->get()).and_then(
		[](tref applied) { return eliminate_quantifiers(applied); }
	).transform([](tref v) { return tau::geth(v); });
}

template <NodeType node>
result<bool> api<node>::realizable(htref fm) {
	if (!fm) {
		result<bool> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return realizable(fm->get());
}

template <NodeType node>
result<bool> api<node>::unrealizable(htref fm) {
	if (!fm) {
		result<bool> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return unrealizable(fm->get());
}

template <NodeType node>
result<bool> api<node>::sat(htref fm) {
	if (!fm) {
		result<bool> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return sat(fm->get());
}

template <NodeType node>
result<bool> api<node>::unsat(htref fm) {
	if (!fm) {
		result<bool> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return unsat(fm->get());
}

template <NodeType node>
result<bool> api<node>::valid(htref fm) {
	if (!fm) {
		result<bool> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return valid(fm->get());
}

template <NodeType node>
result<bool> api<node>::valid_spec(htref fm) {
	if (!fm) {
		result<bool> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return valid_spec(fm->get());
}

// Solving
// ------------------------------------------------------------

template <NodeType node>
result<std::map<htref, htref>> api<node>::solve(htref fm,
	solver_mode mode)
{
	if (!fm) {
		result<std::map<htref, htref>> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return solve(fm->get(), mode).transform(
		[](const subtree_map<node, tref>& m) { return geth<node>(m); });
}

template <NodeType node>
result<std::map<htref, htref>> api<node>::lgrs(htref equation) {
	if (!equation) {
		result<std::map<htref, htref>> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return lgrs(equation->get()).transform(
		[](const subtree_map<node, tref>& m) { return geth<node>(m); });
}

template <NodeType node>
result<htref> api<node>::simplify(htref expr, bool use_defaults) {
	if (!expr) {
		result<htref> r;
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	return simplify(expr->get(), use_defaults).transform(
		[](tref v) { return tau::geth(v); });
}

} // namespace idni::tau_lang
