// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// various extractors

template <NodeType node>
size_t get_type_sid(tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;
	static size_t untyped_sid = string_id("untyped");
	if (auto t = tt(n) | tau::type; t) return t | tt::data;
	return untyped_sid;
}

template <NodeType node>
rr_sig get_rr_sig(tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;
	auto r = tt(n); // traverse to ref if n is bf_ref or wff_ref
	if (auto r_as_child = r | tau::ref; r_as_child) r = r_as_child;
	return { rr_dict(r | tau::sym | tt::string),
		(r | tau::offsets || tau::offset).size(),
		(r | tau::ref_args || tau::ref_arg).size() };
}

template <NodeType node>
rewriter::rules get_rec_relations(tref rrs) {
	using tau = tree<node>;
	using tt = tau::traverser;
	rewriter::rules x;
	auto t = tt(rrs);
	if (t.is(tau::rec_relation))
		return x.emplace_back(tau::geth(t.value_tree().first()),
					tau::geth(t.value_tree().second())), x;
	if (t.is(tau::spec)) t = t | tau::rec_relations;
	t = t || tau::rec_relation;
	for (auto& r : t())
		x.emplace_back(r | tt::first | tt::handle,
				r | tt::second | tt::handle);
	return x;
}

template <NodeType node>
std::optional<rr> get_nso_rr(tref r, bool wo_inference) {
	using tau = tree<node>;
	using tt = tau::traverser;
	const auto& t = tau::get(r);
	if (t.is(tau::bf) || t.is(tau::ref)) return { { {}, tau::geth(r) } };
	if (t.is(tau::rec_relation))
		return { { get_rec_relations<node>(r), tau::geth(r) } };
	tref main_fm = (t.is(tau::tau_constant_source) || t.is(tau::spec)
			? tt(r) | tau::main
			: tt(r) | tau::spec | tau::main) | tau::wff | tt::ref;
	rewriter::rules rules = get_rec_relations<node>(r);
	if (wo_inference) return { { rules, tau::geth(main_fm) } };
	return get_nso_rr<node>(rules, main_fm);
}

template <NodeType node>
std::optional<rr> get_nso_rr(const rewriter::rules& rules,
	tref main_fm)
{
	return infer_ref_types<node>({ rules, tree<node>::geth(main_fm) });
}

// -----------------------------------------------------------------------------

template <NodeType node>
void get_leaves(tref n, typename node::type branch, trefs& leaves) {
	auto add_leave = [&branch, &leaves](tref n) {
		const auto& t = tree<node>::get(n);
		if (t.is(branch)) return true;
		if (t.child_is(branch)) return true;
		return leaves.push_back(n), false;
	};
	pre_order<node>(n).visit(add_leave);
}

template <NodeType node>
trefs get_leaves(tref n, typename node::type branch) {
	trefs leaves;
	get_leaves<node>(n, branch, leaves);
	return leaves;
}

template <NodeType node>
trefs get_dnf_wff_clauses(tref n) {
	return get_leaves<node>(n, node::type::wff_or);
}

template <NodeType node>
trefs get_dnf_bf_clauses(tref n) {
	return get_leaves<node>(n, node::type::bf_or);
}

template <NodeType node>
trefs get_cnf_wff_clauses(tref n) {
	return get_leaves<node>(n, node::type::wff_and);
}

template <NodeType node>
trefs get_cnf_bf_clauses(tref n) {
	return get_leaves<node>(n, node::type::bf_and);
}

// -----------------------------------------------------------------------------
// Helpers for variables having io_var as child

template <NodeType node>
bool is_io_initial(tref io_var) {
	return tree<node>::get(io_var)[0][1].is_integer();
}

template <NodeType node>
bool is_io_shift(tref io_var) {
	using tau = tree<node>;
	return tau::get(io_var)[0][1].is(tau::shift);
}

template <NodeType node>
int_t get_io_time_point(tref io_var) {
	return tree<node>::get(io_var)[0][0][1].get_integer();
}

template <NodeType node>
int_t get_io_shift(tref io_var) {
	return tree<node>::get(io_var)[0][1][0][1][0].get_integer();
}

template <NodeType node>
const std::string& get_io_name(tref io_var) {
	return tree<node>::get(io_var)[0].get_string();
}

template <NodeType node>
int_t get_io_var_shift(tref io_var) {
	// If there is a shift
	if (is_io_shift<node>(io_var)) return get_io_shift<node>(io_var);
	return 0;
}

template <NodeType node>
int_t get_max_shift(const trefs& io_vars, bool ignore_temps) {
	int_t max_shift = 0;
	for (tref v : io_vars) {
		if (ignore_temps && tree<node>::get(v).get_string()[0] == '_')
			continue;
		max_shift = std::max(max_shift, get_io_var_shift<node>(v));
	}
	return max_shift;
}

template <NodeType node>
int_t get_max_initial(const trefs& io_vars) {
	int_t max_init = -1;
	for (tref v : io_vars) {
		if (is_io_initial<node>(v)) {
			int_t init = get_io_time_point<node>(v);
			max_init = std::max(max_init, init);
		}
	}
	return max_init;
}

template <NodeType node>
typename tree<node>::subtree_set get_free_vars_from_nso(tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;
	BOOST_LOG_TRIVIAL(trace) << "(I) -- Begin get_free_vars_from_nso of " << n;
	typename tau::subtree_set free_vars;
	BOOST_LOG_TRIVIAL(trace) << "(I) -- End get_free_vars_from_nso";
	auto collector = [&free_vars](tref n) {
		const auto& t = tau::get(n);
		if (t.is(tau::wff_all) || t.is(tau::wff_ex)) {
			tref var = t.find_top(
				(bool(*)(tref)) is_var_or_capture<node>);
			if (var) if (auto it = free_vars.find(var);
				it != free_vars.end())
			{
				BOOST_LOG_TRIVIAL(trace) << "(I) -- removing quantified var: " << tau::get(var);
				free_vars.erase(it);
			}
		} else if (is_var_or_capture<node>(n)) {
			if (auto offset_child = t() | tau::io_var | tau::offset
				| tt::only_child; offset_child)
			{
				if (is_var_or_capture<node>(offset_child.value())) {
					tref var = offset_child | tt::ref;
					if (auto it = free_vars.find(var);
						it != free_vars.end())
					{
						BOOST_LOG_TRIVIAL(trace) << "(I) -- removing var: " << offset_child.value_tree();
						free_vars.erase(it);
					}
				} else if (offset_child.value_tree().is(tau::shift)) {
					tref var = offset_child | tt::first | tt::ref;
					if (auto it = free_vars.find(var);
						it != free_vars.end())
					{
						BOOST_LOG_TRIVIAL(trace) << "(I) -- removing var: " << offset_child.value_tree();
						free_vars.erase(it);
					}
				}
			}
			BOOST_LOG_TRIVIAL(trace) << "(I) -- inserting var: " << tau::get(n);
			free_vars.insert(n);
		}
	};
	post_order<node>(n).search(collector);
	return free_vars;
}

// A formula has a temporal variable if either it contains an io_var with a variable or capture
// or it contains a flag
template <NodeType node>
bool has_temp_var(tref fm) {
	using tau = tree<node>;
	const auto& t = tau::get(fm);
	trefs io_vars = t.select_top(is<node, tau::io_var>);
	if (io_vars.empty())
		return t.find_top(is<node, tau::constraint>) != nullptr;
	// any input/output stream is a temporal variable, also constant positions
	else return true;
}

} // namespace idni::tau_lang