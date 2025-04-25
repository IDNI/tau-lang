// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// various extractors

template <NodeType node>
std::string tree<node>::get_type_name(tref n) {
	auto t = tt(n) | tau::type;
	if (t) return t | tt::string;
	return "untyped";
}

template <NodeType node>
rr_sig tree<node>::get_rr_sig(tref n) {
	auto r = tt(n); // traverse to ref if n is bf_ref or wff_ref
	if (auto r_as_child = r | ref; r_as_child) r = r_as_child;
	return { rr_dict(r | node::type::sym | tt::string),
		(r | offsets || offset).size(),
		(r | ref_args || ref_arg).size() };
}

template <NodeType node>
rewriter::rules tree<node>::get_rec_relations(tref rrs) {
	rewriter::rules x;
	auto t = tt(rrs);
	if (t.is(rec_relation))
		return x.emplace_back(geth(t.value_tree().first()),
					geth(t.value_tree().second())), x;
	if (t.is(spec)) t = t | rec_relations;
	t = t || rec_relation;
	for (auto& r : t())
		x.emplace_back(r | tt::first | tt::handle,
				r | tt::second | tt::handle);
	return x;
}

template <NodeType node>
std::optional<rr> tree<node>::get_nso_rr(tref r) {
	using tt = tree<node>::traverser;
	const auto& t = get(r);
	if (t.is(bf) || t.is(ref)) return { { {}, geth(r) } };
	if (t.is(rec_relation))
		return { { tree<node>::get_rec_relations(r), geth(r) } };
	tref main_fm = (t.is(tau_constant_source) || t.is(spec)
					? tt(r) | main
					: tt(r) | spec | main) | wff | tt::ref;
	rewriter::rules rules = get_rec_relations(r);
	return get_nso_rr(rules, main_fm);
}

template <NodeType node>
std::optional<rr> tree<node>::get_nso_rr(const rewriter::rules& rules,
	tref main_fm)
{
	return infer_ref_types({ rules, geth(main_fm) });
}

template <NodeType node>
rewriter::builder tree<node>::get_builder(tref ref) {
	DBG(assert(ref != nullptr);)
	tt b(ref);
	auto body = b | builder_body | tt::only_child;
	DBG(assert((body | tt::nt) == bf_builder_body
		|| (body | tt::nt) == wff_builder_body);)
	return { geth(b | builder_head | tt::ref),
		geth(body | tt::first | tt::ref) };
}

template <NodeType node>
rewriter::rules tree<node>::get_rules(tref r) {
	auto rs = tt(r) | rules || rule || tt::first;
	rewriter::rules x;
	for (auto r : rs.traversers()) {
		// tree::get(r.value()).print(std::cout << "rule: ");
		x.emplace_back(tree::geth(r | tt::first | tt::ref),
				tree::geth(r | tt::second | tt::ref));
	}
	return x;
}

// -----------------------------------------------------------------------------

template <NodeType node>
void tree<node>::get_leaves(tref n, node::type branch, trefs& leaves) {
	auto add_leave = [&branch, &leaves](tref n) {
		const auto& t = tree<node>::get(n);
		if (t.is(branch)) return true;
		if (t.child_is(branch)) return true;
		return leaves.push_back(n), false;
	};
	pre_order<node>(n).visit(add_leave);
}

template <NodeType node>
trefs tree<node>::get_leaves(tref n, node::type branch) {
	trefs leaves;
	get_leaves(n, branch, leaves);
	return leaves;
}

template <NodeType node>
trefs tree<node>::get_dnf_wff_clauses(tref n) {
	return get_leaves(n, node::type::wff_or);
}

template <NodeType node>
trefs tree<node>::get_dnf_bf_clauses(tref n) {
	return get_leaves(n, node::type::bf_or);
}

template <NodeType node>
trefs tree<node>::get_cnf_wff_clauses(tref n) {
	return get_leaves(n, node::type::wff_and);
}

template <NodeType node>
trefs tree<node>::get_cnf_bf_clauses(tref n) {
	return get_leaves(n, node::type::bf_and);
}


// -----------------------------------------------------------------------------
// Helpers for variables having io_var as child

template <NodeType node>
bool tree<node>::is_io_initial(tref io_var) {
	return get(io_var)[0][0][1]() | integer;;
}

template <NodeType node>
bool tree<node>::is_io_shift(tref io_var) {
	return get(io_var)[0][0][1]() | shift;;
}

template <NodeType node>
int_t tree<node>::get_io_time_point(tref io_var) {
	return get(io_var)[0][0][0][1].get_integer();
}

template <NodeType node>
int_t tree<node>::get_io_shift(tref io_var) {
	return get(io_var)[0][0][1][0][1][0].get_integer();
}

template <NodeType node>
std::string tree<node>::get_io_name(tref io_var) {
	return get(io_var)[0][0][0].get_string();
}

template <NodeType node>
tref tree<node>::get_tau_io_name(tref io_var) {
	return get(io_var)[0][0].first();
}

template <NodeType node>
int_t tree<node>::get_io_var_shift(tref io_var) {
	// If there is a shift
	if (is_io_shift(io_var)) return get_io_shift(io_var);
	return 0;
}

template <NodeType node>
int_t tree<node>::get_max_shift(const trefs& io_vars, bool ignore_temps) {
	int_t max_shift = 0;
	for (tref v : io_vars) {
		if (ignore_temps && get_io_name(v)[0] == '_')
			continue;
		max_shift = std::max(max_shift, get_io_var_shift(v));
	}
	return max_shift;
}

template <NodeType node>
int_t tree<node>::get_max_initial(const trefs& io_vars) {
	int_t max_init = -1;
	for (tref v : io_vars) {
		if (is_io_initial(v)) {
			int_t init = get_io_time_point(v);
			max_init = std::max(max_init, init);
		}
	}
	return max_init;
}

template <NodeType node>
typename tree<node>::subtree_set tree<node>::get_free_vars_from_nso(tref n) {
	BOOST_LOG_TRIVIAL(trace) << "(I) -- Begin get_free_vars_from_nso of " << n;
	typename tree<node>::subtree_set free_vars;
	BOOST_LOG_TRIVIAL(trace) << "(I) -- End get_free_vars_from_nso";
	auto collector = [&free_vars](tref n) {
		const auto& t = get(n);
		if (t.is(wff_all) || t.is(wff_ex)) {
			tref var = t.find_top(
				(bool(*)(tref)) is_var_or_capture<node>);
			if (var) if (auto it = free_vars.find(var);
				it != free_vars.end())
			{
				BOOST_LOG_TRIVIAL(trace) << "(I) -- removing quantified var: " << get(var);
				free_vars.erase(it);
			}
		} else if (is_var_or_capture<node>(n)) {
			if (auto offset_child = t() | io_var
				| tt::only_child | offset | tt::only_child;
					offset_child)
			{
				if (is_var_or_capture<node>(offset_child.value())) {
					tref var = offset_child | tt::ref;
					if (auto it = free_vars.find(var);
						it != free_vars.end())
					{
						BOOST_LOG_TRIVIAL(trace) << "(I) -- removing var: " << offset_child;
						free_vars.erase(it);
					}
				} else if (offset_child.value_tree().is(shift)) {
					tref var = offset_child | tt::first | tt::ref;
					if (auto it = free_vars.find(var);
						it != free_vars.end())
					{
						BOOST_LOG_TRIVIAL(trace) << "(I) -- removing var: " << offset_child;
						free_vars.erase(it);
					}
				}
			}
			BOOST_LOG_TRIVIAL(trace) << "(I) -- inserting var: " << get(n);
			free_vars.insert(n);
		}
	};
	post_order<node>(n).search(collector);
	return free_vars;
}

// A formula has a temporal variable if either it contains an io_var with a variable or capture
// or it contains a flag
template <NodeType node>
bool tree<node>::has_temp_var(tref fm) {
	const auto& t = get(fm);
	trefs io_vars = t.select_top(tau_lang::is<node, node::type::io_var>);
	if (io_vars.empty())
		return t.find_top(tau_lang::is<node, node::type::constraint>) != nullptr;
	// any input/output stream is a temporal variable, also constant positions
	else return true;
}

} // namespace idni::tau_lang