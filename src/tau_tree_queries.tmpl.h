// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "tau_tree.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "queries"

namespace idni::tau_lang {

/** @internal @copydoc is(tref, size_t) @endinternal */
template <NodeType node>
bool is(tref n, size_t nt) {
	return tree<node>::get(n).is(nt);
}

/** @internal @copydoc is(tref) @endinternal */
template <NodeType node, typename node::type nt>
bool is(tref n) { return is<node>(n, nt); }

/** @internal @copydoc is(size_t) @endinternal */
template <NodeType node>
inline std::function<bool(tref)> is(size_t nt) {
	return [nt](tref n) { return is<node>(n, nt); };
}

/** @internal @copydoc is(tref, std::initializer_list<size_t>) @endinternal */
template <NodeType node>
bool is(tref n, std::initializer_list<size_t> nts) {
	for (auto nt : nts) {
		if (tree<node>::get(n).is(nt)) return true;
	}
	return false;
}

/** @internal @copydoc is(std::initializer_list<size_t>) @endinternal */
// The list is copied into a vector: an initializer_list only views a temporary array that dies at the end of
// the full expression, so a stored predicate would read freed memory.
template <NodeType node>
inline std::function<bool(tref)> is(std::initializer_list<size_t> nts) {
	return [nts = std::vector<size_t>(nts)](tref n) {
		for (auto nt : nts) if (tree<node>::get(n).is(nt)) return true;
		return false;
	};
}

/** @internal @copydoc is_child(tref, size_t) @endinternal */
template <NodeType node>
bool is_child(tref n, size_t nt) {
	return tree<node>::get(n).child_is(nt);
}

/** @internal @copydoc is_child(tref) @endinternal */
template <NodeType node, size_t nt>
bool is_child(tref n) { return is_child<node>(n, nt); }

/** @internal @copydoc is_child(size_t) @endinternal */
template <NodeType node>
inline std::function<bool(tref)> is_child(size_t nt) {
	return [nt](tref n) { return is_child<node>(n, nt); };
}

/** @internal @copydoc is_child_quantifier @endinternal */
template <NodeType node>
bool is_child_quantifier(tref n) {
	return tree<node>::get(n).child_is(node::type::wff_all)
		|| tree<node>::get(n).child_is(node::type::wff_ex);
}

/** @internal @copydoc is_temporal_quantifier @endinternal */
template <NodeType node>
bool is_temporal_quantifier(tref n) {
	return tree<node>::get(n).is(node::type::wff_always)
		|| tree<node>::get(n).is(node::type::wff_sometimes)
		|| tree<node>::get(n).is(node::type::wff_until)
		|| tree<node>::get(n).is(node::type::wff_release)
		|| tree<node>::get(n).is(node::type::wff_weak_until)
		|| tree<node>::get(n).is(node::type::wff_since)
		|| tree<node>::get(n).is(node::type::wff_trigger)
		|| tree<node>::get(n).is(node::type::wff_A)
		|| tree<node>::get(n).is(node::type::wff_E)
		|| tree<node>::get(n).is(node::type::wff_semantic_neg);
}

/** @internal @copydoc is_child_temporal_quantifier @endinternal */
template <NodeType node>
bool is_child_temporal_quantifier(tref n) {
	// Delegates to is_temporal_quantifier, so the LTL and CTL* operators
	// count too. Consumers (e.g. normalize's temporal-block detection) rely
	// on this to avoid applying quantifier elimination across a temporal
	// operator.
	const auto& t = tree<node>::get(n);
	if (!t.has_child()) return false;
	return is_temporal_quantifier<node>(t.first());
}

/** @internal @copydoc is_ba_element @endinternal */
template <NodeType node>
bool is_ba_element(tref n) {
	return tree<node>::get(n).is(node::type::ba_constant)
		|| tree<node>::get(n).is(node::type::variable)
		|| tree<node>::get(n).is(node::type::bf_t)
		|| tree<node>::get(n).is(node::type::bf_f);
}

/** @internal @copydoc is_uconst @endinternal */
template <NodeType node>
bool is_uconst(tref n) {
	return tree<node>::get(n).is(node::type::uconst_name);
}

/** @internal @copydoc is_io_var @endinternal */
template <NodeType node>
bool is_io_var(tref n) {
	return tree<node>::get(n).is(node::type::io_var)
		|| (tree<node>::get(n).is(node::type::variable)
			&& tree<node>::get(n).child_is(node::type::io_var));
}

/** @internal @copydoc is_input_var @endinternal */
template <NodeType node>
bool is_input_var(tref n) {
	return tree<node>::get(n).is_input_variable();
}

/** @internal @copydoc is_output_var @endinternal */
template <NodeType node>
bool is_output_var(tref n) {
	return tree<node>::get(n).is_output_variable();
}

/** @internal @copydoc io_var_direction @endinternal */
// 0 (not io_var/unresolved), 1 (input) or 2 (output): the tag if resolved,
// else the name: a leading `i` or the name `this` is an input, a leading
// `o` or the name `u` an output.
template <NodeType node>
size_t io_var_direction(tref n) {
	const auto& t = tree<node>::get(n);
	if (!t.is(node::type::io_var)) return 0;
	// data() is a 64-bit payload; the direction tag is a size_t.
	size_t dir = static_cast<size_t>(t.data());
	if (dir == 1 || dir == 2) return dir;
	const std::string& nm = get_var_name<node>(n);
	if (nm.empty()) return 0;
	if (nm[0] == 'i' || nm == "this") return 1;
	if (nm[0] == 'o' || nm == "u")    return 2;
	return 0;
}

/** @internal @copydoc is_var_or_capture(tref) @endinternal */
template <NodeType node>
bool is_var_or_capture(tref n) {
	return tree<node>::get(n).is(node::type::variable)
		|| tree<node>::get(n).is(node::type::capture);
}

/** @internal @copydoc is_var_or_capture() @endinternal */
template <NodeType node>
inline std::function<bool(tref)> is_var_or_capture() {
	return [](tref n) { return is_var_or_capture<node>(n); };
}

/** @internal @copydoc is_quantifier @endinternal */
template <NodeType node>
bool is_quantifier(tref n) {
	return tree<node>::get(n).is(node::type::wff_all)
		|| tree<node>::get(n).is(node::type::wff_ex);
}

/** @internal @copydoc is_functional_quantifier @endinternal */
template<NodeType node>
bool is_functional_quantifier(tref n) {
	using tau = tree<node>;
	return tau::get(n).is(tau::bf_fall) || tau::get(n).is(tau::bf_fex);
}

/** @internal @copydoc is_logical_or_functional_quant @endinternal */
template <NodeType node>
bool is_logical_or_functional_quant(tref n) {
	return is_quantifier<node>(n) || is_functional_quantifier<node>(n);
}

/** @internal @copydoc contains(tref, tref) @endinternal */
template <NodeType node>
bool contains(tref fm, tref sub_fm) {
#ifdef TAU_CACHE
	using tau = tree<node>;
	using cache_t = std::unordered_map<std::pair<tref, tref>, bool>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(std::make_pair(tau::trim_right_sibling(fm),
		tau::trim_right_sibling(sub_fm))); it != cache.end())
		return it->second;
#endif // TAU_CACHE
	bool is_contained = false;
	auto has_sub_fm = [&sub_fm, &is_contained](tref n) {
		if (tree<node>::subtree_equals(n, sub_fm))
			return is_contained = true, false;
		return true;
	};
	pre_order<node>(fm).search_unique(has_sub_fm);
#ifdef TAU_CACHE
	return cache.emplace(std::make_pair(tau::trim_right_sibling(fm),
		tau::trim_right_sibling(sub_fm)), is_contained).first->second;
#endif // TAU_CACHE
	return is_contained;
}

/** @internal @copydoc is_point_term @endinternal */
template <NodeType node>
bool is_point_term(size_t ba_type, tref term) {
	using tau = tree<node>;
	if (tau::get(term).child_is(tau::variable)) return true;
	return pack_dense_order_compare<node>(ba_type, term, term) == 0;
}

/** @internal @copydoc order_is_total @endinternal */
template <NodeType node>
bool order_is_total(tref a, tref b) {
	using tau = tree<node>;
	auto set_constant = [](tref t, size_t type) {
		const auto& x = tau::get(t);
		return x.has_child() && x[0].is_ba_constant()
			&& !is_point_term<node>(type, t);
	};
	for (tref t : { a, b }) {
		const size_t type = tau::get(t).get_ba_type();
		if (type && pack_type_is_non_aba_omcat<node>(type)
			&& (set_constant(a, type) || set_constant(b, type)))
			return false;
	}
	return true;
}

/** @internal @copydoc is_atomic_fm @endinternal */
// True if n is a wff whose child is a comparison (=, !=, <, <=, >, >= or a
// negated order). bf_interval and wff_ref are deliberately excluded as they
// are not treated as atomic predicates for normalization purposes.
template <NodeType node>
bool is_atomic_fm(tref n) {
	using tau = tree<node>;
	const auto& fm = tau::get(n);
	if (!fm.is(tau::wff)) return false;
	const tau& child = fm[0];
	return child.is(tau::bf_eq)
	       || child.is(tau::bf_neq)
	       || child.is(tau::bf_lteq)
	       || child.is(tau::bf_nlteq)
	       || child.is(tau::bf_gt)
	       || child.is(tau::bf_ngt)
	       || child.is(tau::bf_gteq)
	       || child.is(tau::bf_ngteq)
	       || child.is(tau::bf_lt)
	       || child.is(tau::bf_nlt);
}

/** @internal @copydoc is_cli_cmd @endinternal */
template <NodeType node>
bool is_cli_cmd(tref n) {
	using tau = tree<node>;

	return is<node>(n, {
		tau::quit_cmd,
		tau::version_cmd,
		tau::clear_cmd,
		tau::help_cmd,
		tau::file_cmd,
		tau::sat_cmd,
		tau::unsat_cmd,
		tau::solve_cmd,
		tau::run_cmd,
		tau::normalize_cmd,
		tau::subst_cmd,
		tau::inst_cmd,
		tau::dnf_cmd,
		tau::cnf_cmd,
	// Commented out because they are not implemented yet
	//	tau::anf_cmd,
	//	tau::pnf_cmd,
		tau::nnf_cmd,
		tau::mnf_cmd,
		tau::onf_cmd,
		tau::qelim_cmd,
		tau::get_cmd,
		tau::set_cmd,
		tau::enable_cmd,
		tau::disable_cmd,
		tau::toggle_cmd,
		tau::def_list_cmd,
		tau::def_print_cmd,
		tau::def_rr_cmd,
		tau::def_input_cmd,
		tau::def_output_cmd,
		tau::history_print_cmd,
		tau::history_list_cmd,
		tau::history_store_cmd
	});
}


/// Number of nodes in the subtree rooted at fm (right siblings of fm
/// itself excluded), counted by full pre-order visit; memoized per
/// subtree under TAU_CACHE.
template <NodeType node>
int_t node_count (tref fm) {
#ifdef TAU_CACHE
	using tau = tree<node>;
	using cache_t = std::unordered_map<tref, int_t>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(tau::trim_right_sibling(fm)); it != cache.end())
		return it->second;
#endif // TAU_CACHE
	int_t c = 0;
	auto count = [&c](tref) {
		return ++c, true;
	};
	pre_order<node>(fm).visit(count);
#ifdef TAU_CACHE
	return cache.emplace(tau::trim_right_sibling(fm), c).first->second;
#endif // TAU_CACHE
	return c;
}


/// Traversal continuation predicate: descend into a node unless it is a term,
/// so a rewrite visits the formula structure but not inside its atoms.
template <NodeType node>
auto visit_wff = [](tref n) {
	return !tree<node>::get(n).is_term();
};

/** @internal @copydoc is_non_boolean_term @endinternal */
// Arithmetic, shifts, nand/nor/xnor, min/max and width casts are
// non-Boolean. A width cast (`(bv[N]) x`) counts too: its operand and result
// live in different algebras, so Boole-decomposing across it produces a
// mixed-width term no back-end can read.
template <NodeType node>
bool is_non_boolean_term(tref n) {
	using tau = tree<node>;
	const tau& t = tau::get(n);
	if (t.is(tau::bf_add) || t.is(tau::bf_sub) || t.is(tau::bf_mul)
		|| t.is(tau::bf_div) || t.is(tau::bf_mod) || t.is(tau::bf_shr)
			|| t.is(tau::bf_shl) || t.is(tau::bf_nand)
			|| t.is(tau::bf_nor) || t.is(tau::bf_xnor)
			|| t.is(tau::bf_min) || t.is(tau::bf_max)
			|| t.is(tau::bf_cast))
		return true;
	return false;
}

/** @internal @copydoc has_fallback @endinternal */
template <NodeType node>
bool has_fallback (tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;

	auto f = tt(n) | tau::fp_fallback | tt::first | tt::ref;
	return f != nullptr && !tau::get(f).is(tau::first_sym) && !tau::get(f).is(tau::last_sym);
}

/** @internal @copydoc is_equational_assignment @endinternal */
template<NodeType node>
bool is_equational_assignment(tref eq) {
	using tau = tree<node>;
	if (!tau::get(eq).child_is(tau::bf_eq)) return false;
	const tau& t = tau::get(eq)[0];
	if (t[0].child_is(tau::variable)) return true;
	if (t[1].child_is(tau::variable)) return true;
	return false;
}

/** @internal @copydoc is_boolean_operation @endinternal */
template <NodeType node>
bool is_boolean_operation(tref op) {
	using tau = tree<node>;

	const tau& t = tau::get(op);
	if (t.is(tau::bf_and) || t.is(tau::bf_or)
		|| t.is(tau::bf_xor) || t.is(tau::bf_neg)
		|| t.is(tau::bf_fex) || t.is(tau::bf_fall)) return true;
	return false;
}

/** @internal @copydoc is_formula @endinternal */
// An alias of while_is_formula, named for use as a predicate.
template <NodeType node>
inline bool is_formula(tref n) {
	return while_is_formula<node>(n);
}

/** @internal @copydoc while_is_formula @endinternal */
// Visiting continuation predicate (for use with `visit`, `find`,...): true
// for every node that is not a term.
template <NodeType node>
bool while_is_formula(tref n) {
	using tau = tree<node>;

	return !tau::get(n).is_term();
}

/** @internal @copydoc while_is_boolean_operation @endinternal */
template <NodeType node>
bool while_is_boolean_operation(tref n) {
	using tau = tree<node>;

	const tau& t = tau::get(n);
	if (t.is(tau::bf) || t.is(tau::bf_and) || t.is(tau::bf_or)
		|| t.is(tau::bf_xor) || t.is(tau::bf_neg)
		|| t.is(tau::bf_fex) || t.is(tau::bf_fall)) return true;
	return false;
}

} // namespace idni::tau_lang
