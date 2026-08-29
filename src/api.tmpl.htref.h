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
	result<htref> r;
	auto inner = get_term(input, simplified);
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<htref> api<node>::geth_formula(const std::string& input, bool simplified) {
	result<htref> r;
	auto inner = get_formula(input, simplified);
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<htref> api<node>::geth_function_def(const std::string& function_def, bool simplified) {
	result<htref> r;
	auto inner = get_function_def(function_def, simplified);
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<htref> api<node>::geth_predicate_def(const std::string& predicate_def, bool simplified) {
	result<htref> r;
	auto inner = get_predicate_def(predicate_def, simplified);
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<htref> api<node>::geth_stream_def(const std::string& stream_def) {
	result<htref> r;
	auto inner = get_stream_def(stream_def);
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<htref> api<node>::geth_spec(const std::string& src) {
	result<htref> r;
	auto inner = get_spec(src);
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<htref> api<node>::geth_definition(const std::string& definition, bool simplified) {
	result<htref> r;
	auto inner = get_definition(definition, simplified);
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<htref> api<node>::geth_spec_or_term(const std::string& expr, bool simplified) {
	result<htref> r;
	auto inner = get_spec_or_term(expr, simplified);
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<htref> api<node>::geth_formula_or_term(const std::string& expr, bool simplified) {
	result<htref> r;
	auto inner = get_formula_or_term(expr, simplified);
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
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
	result<htref> r;
	if (!def || !expression) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = apply_def(def->get(), expression->get());
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<htref> api<node>::apply_defs(const std::set<htref>& defs, htref expression) {
	result<htref> r;
	if (!expression) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	subtree_set<node> tdefs;
	for (htref def : defs) if (def) tdefs.insert(def->get());
	auto inner = apply_defs(tdefs, expression->get());
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<htref> api<node>::apply_all_defs(htref expr) {
	result<htref> r;
	if (!expr) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = apply_all_defs(expr->get());
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
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
	result<htref> r;
	if (!expr || !that || !with) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = substitute(expr->get(), that->get(), with->get());
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
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
		auto sub = substitute(e, that->get(), with->get());
		if (!sub.has_value()) {
			r.merge(std::move(sub));
			DBG(assert(r.is_well_formed());)
			return r;
		}
		e = sub.value();
	}
	r = tau::geth(e);
	DBG(assert(r.is_well_formed());)
	return r;
}

// Normal forms
// ------------------------------------------------------------

template <NodeType node>
result<htref> api<node>::boole_normal_form(htref expr) {
	result<htref> r;
	if (!expr) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = boole_normal_form(expr->get());
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<htref> api<node>::dnf(htref expr) {
	result<htref> r;
	if (!expr) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = dnf(expr->get());
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<htref> api<node>::cnf(htref expr) {
	result<htref> r;
	if (!expr) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = cnf(expr->get());
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<htref> api<node>::nnf(htref expr) {
	result<htref> r;
	if (!expr) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = nnf(expr->get());
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

// Procedures
// ------------------------------------------------------------

template <NodeType node>
result<htref> api<node>::syntactic_term_simplification(htref term) {
	result<htref> r;
	if (!term) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = syntactic_term_simplification(term->get());
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<htref> api<node>::syntactic_formula_simplification(htref fm) {
	result<htref> r;
	if (!fm) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = syntactic_formula_simplification(fm->get());
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<htref> api<node>::normalize_formula(htref fm) {
	result<htref> r;
	if (!fm) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = normalize_formula(fm->get());
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<htref> api<node>::normalize_term(htref term) {
	result<htref> r;
	if (!term) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = normalize_term(term->get());
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<htref> api<node>::anti_prenex(htref fm) {
	result<htref> r;
	if (!fm) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = anti_prenex(fm->get());
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<htref> api<node>::eliminate_quantifiers(htref fm) {
	result<htref> r;
	if (!fm) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	// Apply defs at the tref level before eliminating quantifiers
	// (the tref overload also calls apply_all_defs internally)
	auto applied = apply_all_defs(fm->get());
	if (!applied.has_value()) {
		r.merge(std::move(applied));
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto elim = eliminate_quantifiers(applied.value());
	if (!elim.has_value()) r.merge(std::move(elim));
	else                   r = tau::geth(elim.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<bool> api<node>::realizable(htref fm) {
	result<bool> r;
	if (!fm) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = realizable(fm->get());
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = inner.value();
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<bool> api<node>::unrealizable(htref fm) {
	result<bool> r;
	if (!fm) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = unrealizable(fm->get());
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = inner.value();
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<bool> api<node>::sat(htref fm) {
	result<bool> r;
	if (!fm) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = sat(fm->get());
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = inner.value();
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<bool> api<node>::unsat(htref fm) {
	result<bool> r;
	if (!fm) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = unsat(fm->get());
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = inner.value();
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<bool> api<node>::valid(htref fm) {
	result<bool> r;
	if (!fm) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = valid(fm->get());
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = inner.value();
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<bool> api<node>::valid_spec(htref fm) {
	result<bool> r;
	if (!fm) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = valid_spec(fm->get());
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = inner.value();
	DBG(assert(r.is_well_formed());)
	return r;
}

// Solving
// ------------------------------------------------------------

template <NodeType node>
result<std::map<htref, htref>> api<node>::solve(htref fm,
	solver_mode mode)
{
	result<std::map<htref, htref>> r;
	if (!fm) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = solve(fm->get(), mode);
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = geth<node>(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<std::map<htref, htref>> api<node>::lgrs(htref equation) {
	result<std::map<htref, htref>> r;
	if (!equation) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = lgrs(equation->get());
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = geth<node>(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

template <NodeType node>
result<htref> api<node>::simplify(htref expr, bool use_defaults) {
	result<htref> r;
	if (!expr) {
		r.error(code::invalid_argument, "Invalid argument(s)");
		DBG(assert(r.is_well_formed());)
		return r;
	}
	auto inner = simplify(expr->get(), use_defaults);
	if (!inner.has_value()) r.merge(std::move(inner));
	else                    r = tau::geth(inner.value());
	DBG(assert(r.is_well_formed());)
	return r;
}

} // namespace idni::tau_lang
