// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "tau_tree.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "queries"

namespace idni::tau_lang {

template <NodeType node>
bool is(tref n, size_t nt) {
	return tree<node>::get(n).is(nt);
}

template <NodeType node, typename node::type nt>
bool is(tref n) { return is<node>(n, nt); }

// factory method for is predicate
template <NodeType node>
inline std::function<bool(tref)> is(size_t nt) {
	return [nt](tref n) { return is<node>(n, nt); };
}

template <NodeType node>
bool is(tref n, std::initializer_list<size_t> nts) {
	for (auto nt : nts) {
		if (tree<node>::get(n).is(nt)) return true;
	}
	return false;
}

// factory method for is predicate
template <NodeType node>
inline std::function<bool(tref)> is(std::initializer_list<size_t> nts) {
	return [nts](tref n) { return is<node>(n, nts); };
}



template <NodeType node>
bool is_child(tref n, size_t nt) {
	return tree<node>::get(n).child_is(nt);
}

template <NodeType node, size_t nt>
bool is_child(tref n) { return is_child<node>(n, nt); }

// factory method for is predicate
template <NodeType node>
inline std::function<bool(tref)> is_child(size_t nt) {
	return [nt](tref n) { return is_child<node>(n, nt); };
}

template <NodeType node>
bool is_child_quantifier(tref n) {
	return tree<node>::get(n).child_is(node::type::wff_all)
		|| tree<node>::get(n).child_is(node::type::wff_ex);
}

template <NodeType node>
bool is_temporal_quantifier(tref n) {
	return tree<node>::get(n).is(node::type::wff_always)
		|| tree<node>::get(n).is(node::type::wff_sometimes);

}

template <NodeType node>
bool is_ba_element(tref n) {
	return tree<node>::get(n).is(node::type::ba_constant)
		|| tree<node>::get(n).is(node::type::variable)
		|| tree<node>::get(n).is(node::type::bf_t)
		|| tree<node>::get(n).is(node::type::bf_f);
}

template <NodeType node>
bool is_uconst(tref n) {
	return tree<node>::get(n).is(node::type::uconst_name);
}

template <NodeType node>
bool is_io_var(tref n) {
	return tree<node>::get(n).is(node::type::io_var)
		|| (tree<node>::get(n).is(node::type::variable)
			&& tree<node>::get(n).child_is(node::type::io_var));
}

template <NodeType node>
bool is_input_var(tref n) {
	return tree<node>::get(n).is_input_variable();
}

template <NodeType node>
bool is_output_var(tref n) {
	return tree<node>::get(n).is_output_variable();
}

template <NodeType node>
bool is_var_or_capture(tref n) {
	return tree<node>::get(n).is(node::type::variable)
		|| tree<node>::get(n).is(node::type::capture);
}

template <NodeType node>
inline std::function<bool(tref)> is_var_or_capture() {
	return [](tref n) { return is_var_or_capture<node>(n); };
}

template <NodeType node>
bool is_quantifier(tref n) {
	return tree<node>::get(n).is(node::type::wff_all)
		|| tree<node>::get(n).is(node::type::wff_ex);
}

template<NodeType node>
bool is_functional_quantifier(tref n) {
	using tau = tree<node>;
	return tau::get(n).is(tau::bf_fall) || tau::get(n).is(tau::bf_fex);
}

template <NodeType node>
bool is_logical_or_functional_quant(tref n) {
	return is_quantifier<node>(n) || is_functional_quantifier<node>(n);
}

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

template <NodeType node>
bool is_atomic_fm(tref n) {
	using tau = tree<node>;
	auto fm = tau::get(n);
	if (!fm.is(tau::wff)) return false;
	const tau& child = fm[0];
	return fm.is(tau::wff)
	       && (child.is(tau::bf_eq)
		   || child.is(tau::bf_neq)
		   || child.is(tau::bf_lteq)
		   || child.is(tau::bf_nlteq)
		   || child.is(tau::bf_gt)
		   || child.is(tau::bf_ngt)
		   || child.is(tau::bf_gteq)
		   || child.is(tau::bf_ngteq)
		   || child.is(tau::bf_lt)
		   || child.is(tau::bf_nlt));
};

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
		tau::anf_cmd,
		tau::nnf_cmd,
		tau::pnf_cmd,
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
		tau::def_list_cmd,
		tau::def_input_cmd,
		tau::def_output_cmd,
		tau::history_print_cmd,
		tau::history_list_cmd,
		tau::history_store_cmd
	});
}


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

template <NodeType node>
auto visit_wff = [](tref n) static {
	return !tree<node>::get(n).is_term();
};

template <NodeType node>
auto is_boolean_operation = [](tref n) static {
	using tau = tree<node>;
	const tau& t = tau::get(n);
	if (t.is(tau::bf) || t.is(tau::bf_and) || t.is(tau::bf_or)
		|| t.is(tau::bf_xor) || t.is(tau::bf_neg)
		|| t.is(tau::bf_fex) || t.is(tau::bf_fall)) return true;
	return false;
};

template <NodeType node>
auto is_non_boolean_term = [](tref n) static {
	using tau = tree<node>;
	const tau& t = tau::get(n);
	if (t.is(tau::bf_add) || t.is(tau::bf_sub) || t.is(tau::bf_mul)
		|| t.is(tau::bf_div) || t.is(tau::bf_mod) || t.is(tau::bf_shr)
			|| t.is(tau::bf_shl) || t.is(tau::bf_nand)
			|| t.is(tau::bf_nor) || t.is(tau::bf_xnor))
		return true;
	return false;
};

template <NodeType node>
bool has_fallback (tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;

	auto f = tt(n) | tau::fp_fallback | tt::first | tt::ref;
	return f != nullptr && !tau::get(f).is(tau::first_sym) && !tau::get(f).is(tau::last_sym);
}

template<NodeType node>
bool is_equational_assignment(tref eq) {
	using tau = tree<node>;
	DBG(assert(tau::get(eq).child_is(tau::bf_eq)));
	const tau& t = tau::get(eq)[0];
	if (t[0].child_is(tau::variable)) return true;
	if (t[1].child_is(tau::variable)) return true;
	return false;
}
} // namespace idni::tau_lang
