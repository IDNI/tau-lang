// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "tau_tree.h"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// various extractors

template <NodeType node>
rr_sig tree<node>::get_rr_sig(tref n) {
	auto r = tt(n); // traverse to ref if n is bf_ref or wff_ref
	if (auto r_as_child = r | ref; r_as_child) r = r_as_child;
	return { rr_dict(r | tau_parser::sym | tt::string),
		(r | offsets || offset).size(),
		(r | ref_args || ref_arg).size() };
};

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
	return { { rules, geth(main_fm) } };
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
	return get_leaves(n, tau_parser::wff_or);
}

template <NodeType node>
trefs tree<node>::get_dnf_bf_clauses(tref n) {
	return get_leaves(n, tau_parser::bf_or);
}

template <NodeType node>
trefs tree<node>::get_cnf_wff_clauses(tref n) {
	return get_leaves(n, tau_parser::wff_and);
}

template <NodeType node>
trefs tree<node>::get_cnf_bf_clauses(tref n) {
	return get_leaves(n, tau_parser::bf_and);
}

} // namespace idni::tau_lang