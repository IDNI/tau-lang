// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

//#include <cvc5/cvc5.h>

#include <functional>

#include "tau_tree.h"
#include "definitions.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "extractors"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// various extractors

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

/**
 * @brief Classify every io_var in @p fm as input or output by stamping
 * the io_var node's data field: 1 = input, 2 = output, 0 stays as
 * unresolved. Streams registered in @p ctx win; unregistered ones fall
 * back to the name heuristic ('i...'/"this" -> in, 'o...'/"u" -> out).
 * @return @p fm with the stamped io_var nodes substituted in.
 */
template <NodeType node>
tref resolve_io_vars(io_context<node>& ctx, tref fm) {
	LOG_TRACE << "resolve_io_vars - fm: " << LOG_FM_DUMP(fm);
	using tau = tree<node>;
	auto resolve = [&ctx](tref n) {
		const auto& t = tau::get(n);
		if (t.is(tau::io_var)) {
			// EX-1: this used to be canonize<node>(n), but canonize
			// expects the enclosing `variable` node -- it selects an
			// io_var CHILD (tt(x) | tau::io_var | tau::var_name).
			// Handed the io_var itself it matched nothing and returned
			// its argument unchanged, offset subtree and all, so the
			// key could never equal what add_input_console/
			// add_output_console register
			// (build_canonized_io_var == variable(io_var(var_name))).
			// Both context lookups below were therefore dead, and
			// classification always fell through to the name heuristic.
			// Build the key the registrars' way instead.
			tref var_name = get_var_name_node<node>(n);
			tref var = var_name
				? tau::get(tau::variable,
					tau::get(tau::io_var, { var_name }))
				: canonize<node>(n);
			if (auto it = ctx.inputs.find(var); it != ctx.inputs.end())
				return t.replace_value(
					t.value.replace_data(1));
			if (auto it = ctx.outputs.find(var); it != ctx.outputs.end())
				return t.replace_value(
					t.value.replace_data(2));

			static const auto io_prefixed_io_var =
				[](size_t var_sid) -> size_t
			{
				return (dict(var_sid)[0] == 'i' || dict(var_sid) == "this")
						? 1
						: ((dict(var_sid)[0] == 'o' ||
						dict(var_sid) == "u")
							? 2
							: 0);
			};
			size_t var_sid = get_var_name_sid<node>(var);
			size_t direction = io_prefixed_io_var(var_sid);
			DBG(LOG_TRACE << "io_prefixed_io_var: " << dict(var_sid)
				<< " " << (direction == 1 ? "IN"
					: (direction == 2 ? "OUT"
					: "UNRESOLVED I/O"));)
			if (direction) return t.replace_value(
					t.value.replace_data(direction));
		}
		return n;
	};
	tref resolved = pre_order<node>(fm).apply_unique(resolve);
	LOG_TRACE << "resolve_io_vars - resolved: " << LOG_FM_DUMP(resolved);
	return resolved;
}

template <NodeType node>
rewriter::rules get_rec_relations(io_context<node>& ctx, tref rrs) {
	using tau = tree<node>;
	using tt = tau::traverser;
	rewriter::rules x;
	if (!rrs) return x;
	DBG(LOG_TRACE << "get_rec_relations: " << LOG_FM_DUMP(rrs);)
	auto t = tt(rrs);

	auto resolve_in_formula = [&x, &ctx](tt& t) {
		x.emplace_back(t | tt::first  | tt::handle,
			tau::geth(resolve_io_vars<node>(ctx,
						t | tt::second | tt::ref)));
	};

	if (t.is(tau::rec_relation)) return resolve_in_formula(t), x;
	if (t.is(tau::start)) t = t | tau::spec | tau::definitions;
	else if (t.is(tau::spec))  t = t | tau::definitions;
	t = t || tau::rec_relation;
	for (auto& r : t()) resolve_in_formula(r);

	DBG(LOG_TRACE << "get_rec_relations number of defs: " << x.size();)
	return x;
}

template <NodeType node>
rewriter::rules get_rec_relations(tref rrs) {
	return get_rec_relations<node>(
		*definitions<node>::instance().get_io_context(), rrs);
}

// TI-3: every case of one recurrence family (same symbol name, offset
// arity, AND ref-arg arity -- the full rr_sig, matching how
// is_functional_ref/the fixpoint-call machinery itself identifies a
// family: get_rr_sig's own three fields, not just a name+arg_arity
// subset of it. An indexed family `f[n]/f[0]` and an unrelated plain
// function `f(x)` sharing the name and argument count are DIFFERENT
// families -- name+arg_arity alone would conflate them, wrongly rejecting
// the unrelated plain function as "a case of recurrence f" the moment an
// indexed family of the same name/arity also exists.) must agree on the
// effective BA types of its head arguments. Cases are entered as separate
// statements and inferred independently, so a half-annotated family
// type-checks per case but can never match one set of call arguments: the
// indexed call silently fails to expand and the fixpoint enumeration
// never reaches its base case (2026-09-01). Reject the family at assembly
// time with a message that names both offending cases.
//
// Per-position state accumulated across every case of one family: the
// effective BA type id pinned so far (0 = still a wildcard -- no case has
// pinned this position, e.g. because every case's argument there is a
// non-variable match pattern, such as a nested ref, with no variable type
// to read), and the head that pinned it, for the error message.
// Effective BA type id of each of @p r's OWN immediate arguments (ref >
// ref_args > ref_arg, one level each way): a recursive descendant search
// would also pick up ref_args belonging to a nested ref used AS one of the
// arguments (e.g. `add(int[0](1), x)`'s `int[0](1)` pattern argument has
// ref_args of its own), inflating the argument count/shape. A ref_arg's
// type lives on its argument variable (ref_arg > bf > variable), not on
// the ref_arg node itself -- see transform_ref_args_to_captures's
// def_transformer, which reads the same t[0][0] for the same reason. A
// non-variable argument (a nested ref, a constant) reads whatever
// effective type its own subtree carries.
template <NodeType node>
std::vector<size_t> collect_immediate_ref_arg_types(tref r) {
	using tau = tree<node>;
	using tt = tau::traverser;
	std::vector<size_t> types;
	for (tref a : (tt(r) | tau::ref_args || tau::ref_arg).values()) {
		const auto& at = tau::get(a);
		tref var = (at.children_size() > 0
				&& at[0].children_size() > 0
				&& at[0][0].is(tau::variable))
			? at[0][0].get() : a;
		types.push_back(get_effective_ba_type<node>(var));
	}
	return types;
}

template <NodeType node>
bool validate_rr_case_types(const rr<node>& defs) {
	struct family_state {
		std::vector<size_t> types; // 0 = unpinned/wildcard so far
		std::vector<tref> heads;   // case head that pinned types[i]
	};
	std::map<rr_sig, family_state> families;
	for (const auto& r : defs.rec_relations) {
		tref head = unwrap_to_ref<node>(r.first->get());
		if (!head) continue;
		rr_sig fam = get_rr_sig<node>(head);
		std::vector<size_t> types =
			collect_immediate_ref_arg_types<node>(head);
		DBG(LOG_TRACE << "validate_rr_case_types: " << LOG_FM(head)
			<< " collected " << types.size() << " arg type(s)";
			for (size_t ti : types) LOG_TRACE << "  type id: " << ti;)
		auto [it, inserted] = families.try_emplace(fam);
		family_state& fs = it->second;
		if (inserted) {
			fs.types.assign(types.size(), 0);
			fs.heads.assign(types.size(), nullptr);
		}
		// arg_arity is part of the family key, so every case should
		// collect exactly that many immediate ref_args; a mismatch would
		// mean this collection and get_rr_sig's own count disagree, which
		// should not happen -- skip rather than index out of bounds or
		// misreport an arity slip as a type disagreement.
		if (types.size() != fs.types.size()) {
			DBG(LOG_TRACE << "validate_rr_case_types: arity mismatch "
				"collecting immediate ref_args for `" << LOG_FM(head)
				<< "` (" << types.size() << " vs " << fs.types.size()
				<< "); skipping";)
			continue;
		}
		for (size_t i = 0; i < types.size(); ++i) {
			if (!types[i]) continue; // wildcard here: pins nothing
			if (!fs.types[i]) { // first case to pin this position
				fs.types[i] = types[i];
				fs.heads[i] = head;
				continue;
			}
			if (fs.types[i] != types[i]) {
				LOG_ERROR << "the cases of recurrence `"
					<< LOG_FM(head) << "` disagree on their argument"
					" types (`" << LOG_FM(fs.heads[i])
					<< "` vs `" << LOG_FM(head) << "`); annotate the"
					" argument the same way in every case";
				return false;
			}
		}
	}
	return true;
}

// TI-4: a call whose argument types can never match its definition's
// parameter types is a silent no-op at rule-application time -- the rule
// simply never fires, so `pr2(u) := (u:sbf = 0)` followed by `pr2(z:tau)`
// echoed the call back unexpanded, and the same shape under solve
// surfaced as "Internal error in solver" (2026-09-02). Functions already
// error on this (their calls unify against the recorded signature type in
// infer_ba_types); predicates have no recorded signature, so their calls
// are checked here instead, where the definitions and every call site are
// both in hand. Rule matching treats an untyped node and :tau
// interchangeably (an untyped parameter materializes as :tau, and an
// untyped argument matches one -- verified against nso_rr_apply), so both
// normalize to tau before comparing; every other pairing must be exact. A
// reference matching no definition family is uninterpreted and stays
// legal, as always.
template <NodeType node>
bool validate_rr_call_types(const rr<node>& defs) {
	using tau = tree<node>;
	struct family_state {
		std::vector<size_t> types; // 0 = unpinned by any case so far
		tref head = nullptr;       // one case head, for the message
	};
	std::map<rr_sig, family_state> families;
	for (const auto& r : defs.rec_relations) {
		tref head = unwrap_to_ref<node>(r.first->get());
		if (!head) continue;
		auto types = collect_immediate_ref_arg_types<node>(head);
		auto [it, inserted] = families.try_emplace(
			get_rr_sig<node>(head), family_state{ types, head });
		if (!inserted && it->second.types.size() == types.size())
			// a later case may pin a position an earlier one left open
			for (size_t i = 0; i < types.size(); ++i)
				if (!it->second.types[i])
					it->second.types[i] = types[i];
	}
	if (families.empty()) return true;
	auto norm = [](size_t t) {
		return !t || t == untyped_type_id<node>()
			? tau_type_id<node>() : t;
	};
	auto calls_match = [&](tref root) -> bool {
		if (!root) return true;
		for (tref call : tau::get(root).select_all(is<node, tau::ref>)) {
			rr_sig sig = get_rr_sig<node>(call);
			auto it = families.find(sig);
			// An offset-free call to an indexed family is that
			// family's fixpoint-call syntax (same lookup rule as
			// is_functional_ref/find_fpcalls): check it against the
			// family's argument types too.
			if (it == families.end() && sig.offset_arity == 0)
				for (auto jt = families.begin();
						jt != families.end(); ++jt)
					if (jt->first.name == sig.name
						&& jt->first.arg_arity == sig.arg_arity
						&& jt->first.offset_arity > 0) {
						it = jt;
						break;
					}
			if (it == families.end()) continue; // uninterpreted
			auto args = collect_immediate_ref_arg_types<node>(call);
			const auto& fs = it->second;
			if (args.size() != fs.types.size()) continue;
			for (size_t i = 0; i < args.size(); ++i)
				if (norm(args[i]) != norm(fs.types[i])) {
					LOG_ERROR << "the call `" << LOG_FM(call)
						<< "` disagrees with the argument types"
						" of its definition `" << LOG_FM(fs.head)
						<< "` and can never match; type the"
						" arguments and the definition's"
						" parameters the same way";
					return false;
				}
		}
		return true;
	};
	if (defs.main && !calls_match(defs.main->get())) return false;
	for (const auto& r : defs.rec_relations)
		if (!calls_match(r.second->get())) return false;
	return true;
}

template <NodeType node>
std::optional<rr<node>> get_nso_rr(io_context<node>& ctx, tref r) {
	using tau = tree<node>;
	using tt = tau::traverser;
	if (!r) return {};
	DBG(LOG_TRACE << "get_nso_rr: " << LOG_FM(r);)
	const auto& t = tau::get(r).is(tau::start) ? tau::get(r)[0]
						   : tau::get(r);
	r = t.get();
	if (t.is(tau::bf) || t.is(tau::ref)) return { { {}, tau::geth(r) } };
	if (t.is(tau::rec_relation)) {
		auto rec_only = rr<node>(get_rec_relations<node>(ctx, r),
			(htref) nullptr);
		if (!validate_rr_case_types<node>(rec_only)) return {};
		if (!validate_rr_call_types<node>(rec_only)) return {};
		return { rec_only };
	}
	LOG_TRACE << "get_nso_rr - r: " << LOG_FM_DUMP(r);

	tref expression = tt(r) | tau::main | tau::wff | tt::ref;
	if (!expression) expression = tt(r) | tau::main | tau::bf | tt::ref;
	tref main_fm = resolve_io_vars<node>(ctx, expression);
	if (!main_fm) return {};

	rewriter::rules rules = get_rec_relations<node>(ctx, r);
	DBG(LOG_TRACE << "rules: " << rules.size();)
	auto nso_rr = rr<node>(rules, tau::geth(main_fm));
	auto check_resolved_io_vars = [](htref form) {
		for (tref io_var : tau::get(form).select_all(is<node, tau::io_var>)) {
			// LOG_TRACE << "io_var: " << LOG_FM_DUMP(io_var);
			if (tau::get(io_var).data() == 0) {
				LOG_ERROR << "I/O variable is not defined "
					<< TAU_TO_STR(io_var);
				return false;
			}
		}
		return true;
	};
	if (!check_resolved_io_vars(nso_rr.main)) return {};
	for (const auto& rec_relation : nso_rr.rec_relations)
		if (!check_resolved_io_vars(rec_relation.second))
			return {};
	if (!validate_rr_case_types<node>(nso_rr)) return {};
	if (!validate_rr_call_types<node>(nso_rr)) return {};
	DBG(LOG_TRACE << "get_nso_rr result: "<< LOG_RR(nso_rr);)
	return nso_rr;
}

template <NodeType node>
std::optional<rr<node>> get_nso_rr(tref r) {
	return get_nso_rr<node>(
		*definitions<node>::instance().get_io_context(), r);
}

// -----------------------------------------------------------------------------

template <NodeType node>
void get_leaves(tref n, typename node::type branch, trefs& leaves,
	trefs* inner)
{
	using tau = tree<node>;
	if (!n) return;

	// Explicit-stack pre-order walk along the and/or spine. The previous
	// recursive flatten needed one frame per nesting level and, since a
	// DNF's or-spine is a left-deep binary chain, its depth equalled the
	// clause count and overflowed the 8 MB stack near ~24k clauses
	// (GitHub #90). It also memoised the full leaf list at every spine
	// node, which is quadratic in the clause count (30k clauses -> 4 GB).
	// Leaves are appended left to right with multiplicity preserved (a
	// shared or duplicated subtree is spliced in once per occurrence), so
	// no memo is needed: the work is linear in the output size.
	std::vector<tref> stack{n};
	while (!stack.empty()) {
		const tref m = stack.back();
		stack.pop_back();
		const auto& t = tau::get(m);
		if (t.is(branch) || t.child_is(branch)) {
			// The inner spine nodes, for a caller that must recognise
			// them later (the path sweep marks them handled).
			if (inner && m != n) inner->push_back(m);
			const auto children = t.get_children();
			for (auto it = children.rbegin(); it != children.rend(); ++it)
				stack.push_back(*it);
		} else {
			LOG_TRACE << "adding leaf: " << LOG_FM(m);
			leaves.push_back(m);
		}
	}
}

template <NodeType node>
trefs get_leaves(tref n, typename node::type branch) {
	trefs leaves;
	get_leaves<node>(n, branch, leaves);
	LOG_TRACE << "got leaves: " << leaves.size();
	for (tref l : leaves) LOG_TRACE << "leaf: " << LOG_FM(l);
	return leaves;
}

template <NodeType node>
trefs get_dnf_wff_clauses(tref n) {
	using tau = tree<node>;
	LOG_TRACE << "getting dnf wff clauses for " << LOG_FM(n);
	return get_leaves<node>(n, tau::wff_or);
}

template <NodeType node>
trefs get_dnf_bf_clauses(tref n) {
	using tau = tree<node>;
	LOG_TRACE << "getting dnf bf clauses for " << LOG_FM(n);
	return get_leaves<node>(n, tau::bf_or);
}

template <NodeType node>
trefs get_cnf_wff_clauses(tref n) {
	using tau = tree<node>;
	LOG_TRACE << "getting cnf wff clauses for " << LOG_FM(n);
	return get_leaves<node>(n, tau::wff_and);
}

template <NodeType node>
trefs get_cnf_bf_clauses(tref n) {
	using tau = tree<node>;
	LOG_TRACE << "getting cnf bf clauses for " << LOG_FM(n);
	return get_leaves<node>(n, tau::bf_and);
}

template<NodeType node>
tref expression_paths<node>::iterator::operator*() {
	if (!_expr) return nullptr;
	size_t idx = 0;
	const bool term = tau::get(_expr).is_term();
	auto f = [&idx, &term, this](tref n) {
		bool check = true;
		// In order to catch chained bf_or we need to loop
		while (check) {
			if (term) {
				if (!tau::get(n).is(tau::bf)) return n;
				const tau& t = tau::get(n)[0];
				check = t.is(tau::bf_or) || t.is(tau::bf_xor);
			} else {
				if (!tau::get(n).is(tau::wff)) return n;
				check = tau::get(n)[0].is(tau::wff_or);
			}
			if (check) {
				if (idx == decisions.size()) {
					// Encounter fork for the first time
					// Go left with save
					decisions.push_back(true);
					++idx;
					n = tau::get(n)[0].first();
				} else if (decisions[idx++]) {
					// Go left
					n = tau::get(n)[0].first();
				} else {
					// Go right
					n = tau::get(n)[0].second();
				}
			}
		}
		return n;
	};
	auto visit = [](tref n) {
		if (is_temporal_quantifier<node>(n) || is_quantifier<node>(n))
			return false;
		else return is_formula<node>(n);
	};
	tref res = nullptr;
	if (term) res = pre_order<node>(_expr).apply(f);
	else res = pre_order<node>(_expr).apply(f, visit, identity);
	// Remove dangling decisions
	while (idx < decisions.size()) decisions.pop_back();
	return res;
}

template<NodeType node>
expression_paths<node>::iterator& expression_paths<node>::iterator::operator++() {
	if (keep_path) {
		keep_path = false;
		return *this;
	}
	while (!decisions.empty() && !decisions.back())
		decisions.pop_back();
	if (!decisions.empty())
		decisions.back() = false;
	else _expr = nullptr; // We have reached the end
	return *this;
}

template<NodeType node>
tref expression_paths<node>::iterator::apply(const auto& f) {
	if (!_expr) return nullptr;
	tref path = operator*();
	tref res = f(path);
	// std::cout << "Apply f on " << tau::get(path) << " yields " << tau::get(res) << "\n";
	// If no change occurs
	if (tau::subtree_equals(res, path)) return nullptr;
	const bool term = tau::get(_expr).is_term();
	// Now decisions holds the number of current wff_or occurrences on path
	if (decisions.empty()) {
		// Empty decisions means there is just a single path
		_prev_expr = _expr;
		_expr = (term ? tau::_0(find_ba_type<node>(_expr)) : tau::_F());
		return res;
	}
	size_t idx = 0;
	bool removed = false;
	subtree_unordered_set<node> excluded;
	auto remove = [&](tref n) {
		if (removed) return n;
		bool check;
		if (term) {
			if (!tau::get(n).is(tau::bf)) return n;
			const tau& t = tau::get(n)[0];
			check = t.is(tau::bf_or) || t.is(tau::bf_xor);
		} else {
			if (!tau::get(n).is(tau::wff)) return n;
			check = tau::get(n)[0].is(tau::wff_or);
		}
		if (check) {
			DBG(assert(idx < decisions.size());)
			if (idx == decisions.size() - 1) {
				// In order to delete the current path
				// we simply exclude it
				removed = true;
				if (decisions.back()) {
					keep_path = true;
					return tau::get(n)[0].second();
				}
				else return tau::get(n)[0].first();
			}
			else if (decisions[idx++]) {
				// Go left
				excluded.insert(tau::get(n)[0].second());
			} else {
				// Go right
				excluded.insert(tau::get(n)[0].first());
			}
		}
		return n;
	};
	auto visit = [&excluded, &term](tref n) {
		if (!term && is_temporal_quantifier<node>(n)) return false;
		if (!term && is_quantifier<node>(n)) return false;
		if (excluded.contains(n)) return false;
		else if (!term) return is_formula<node>(n);
		else return true;
	};
	// Remove path from _expr and return res
	_prev_expr = _expr;
	_expr = pre_order<node>(_expr).apply(remove, visit, identity);
	return res;
}

// Roll back the last apply(): restore the expression saved before the
// path was erased and clear keep_path, so operator++ advances past the
// reinstated path. Only one apply() can be undone -- _prev_expr is a
// single slot, not a history.
template<NodeType node>
void expression_paths<node>::iterator::undo_apply() {
	keep_path = false;
	_expr = _prev_expr;
}

// Iterators are equal iff their expressions are structurally equal and
// their decision vectors agree, with the shorter vector's missing tail
// read as all "left" (true): a fork not yet visited defaults to its
// left branch, so the padded states denote the same path.
template<NodeType node>
bool expression_paths<node>::iterator::operator==(const iterator& other) const {
	if (tau::subtree_equals(_expr, other._expr)) {
		if (decisions.size() <= other.decisions.size()) {
			for (size_t i = 0; i < decisions.size(); ++i)
				if (decisions[i] != other.decisions[i])
					return false;
			for (size_t i = decisions.size();
			     i < other.decisions.size(); ++i)
				if (!other.decisions[i]) return false;
			return true;
		} else {
			for (size_t i = 0; i < other.decisions.size(); ++i)
				if (decisions[i] != other.decisions[i])
					return false;
			for (size_t i = other.decisions.size();
			     i < decisions.size(); ++i)
				if (!decisions[i]) return false;
			return true;
		}
	} else return false;
}

template<NodeType node>
bool expression_paths<node>::iterator::operator!=(const iterator& other) const {
	return !(*this == other);
}

template<NodeType node>
expression_paths<node>::iterator expression_paths<node>::begin() const {
	return iterator(_expr);
}

template<NodeType node>
expression_paths<node>::iterator expression_paths<node>::end() const {
	return iterator(nullptr);
}

template<NodeType node>
tref expression_paths<node>::apply(const auto& path_transform) {
	iterator it = iterator(_expr);
	trefs changes;
	tref res = nullptr;
	while (it != end()) {
		if (tref c = it.apply(path_transform)) {
			changes.push_back(c);
		}
		res = it.get_expr();
		++it;
	}
	if (tau::get(_expr).is_term()) {
		return tau::build_bf_or(res, tau::build_bf_or(changes,
			find_ba_type<node>(res)));
	} else return tau::build_wff_or(res, tau::build_wff_or(changes));
}

template<NodeType node>
tref expression_paths<node>::apply(const auto& path_transform, const auto& callback) {
	auto build_or = [*this](tref l, tref r) {
		if (tau::get(_expr).is_term())
			return tau::build_bf_or(l, r);
		else return tau::build_wff_or(l, r);
	};
	iterator it = iterator(_expr);
	tref change = tau::get(_expr).is_term() ? tau::_0(
		find_ba_type<node>(_expr)) : tau::_F();
	tref res = nullptr;
	while (callback(res) && it != end()) {
		if (tref c = it.apply(path_transform))
			change = build_or(change, c);
		res = build_or(it.get_expr(), change);
		++it;

	}
	return res;
}

template<NodeType node>
tref expression_paths<node>::apply_only_if(const auto& path_transform,
	const auto& callback) {
	auto build_or = [*this](tref l, tref r) {
		if (tau::get(_expr).is_term())
			return tau::build_bf_or(l, r);
		else return tau::build_wff_or(l, r);
	};
	iterator it = iterator(_expr);
	tref res = nullptr;
	while (it != end()) {
		if (tref c = it.apply(path_transform)) {
			res = build_or(it.get_expr(), c);
			if (callback(res)) break;
			else it.undo_apply();
			res = it.get_expr();
		}
		++it;

	}
	DBG(LOG_TRACE << "apply_only_if res: " << tau::get(res) << "\n";)
	return res;
}

// -----------------------------------------------------------------------------

template <NodeType node>
size_t get_ba_type(tref n) {
	return tree<node>::get(n).get_ba_type();
}

template <NodeType node>
tref get_var_name_node(tref var) {
	using tau = tree<node>;
	using tt = tau::traverser;
	auto v = tt(var);
	if (v.is(tau::ba_constant) || v.is(tau::var_name)) return var;
	
	// Unwrap layers (bf -> variable -> io_var/uconst_name -> var_name)
	// trying each combination until we find var_name or uconst_name
	constexpr std::array wrappers = {tau::bf, tau::variable, tau::io_var};
	constexpr std::array terminals = {tau::var_name, tau::uconst_name};
	
	// Try direct terminal access
	for (auto term : terminals) {
		if (auto vn = v | term; vn) return vn.value();
	}
	
	// Try unwrapping one layer at a time, checking terminals at each level
	auto current = v;
	for (auto wrapper : wrappers) {
		if (auto next = current | wrapper; next) {
			current = tt(next.value());
			for (auto term : terminals) {
				if (auto vn = current | term; vn) return vn.value();
			}
		}
	}
	
	return nullptr;
}

template <NodeType node>
const std::string& get_var_name(tref var) {
	tref vn = get_var_name_node<node>(var);
	if (!vn) { static const std::string empty; return empty; }
	return tree<node>::get(vn).get_string();
}

template <NodeType node>
size_t get_var_name_sid(tref var) {
	tref vn = get_var_name_node<node>(var);
	if (!vn) return 0;
	return tree<node>::get(vn).data();
}

// -----------------------------------------------------------------------------
// Helpers for variables having io_var as child

template <NodeType node>
bool is_io_initial(tref io_var) {
	return tree<node>::get(io_var)[0][1][0].is_integer();
}

template <NodeType node>
bool is_io_shift(tref io_var) {
	using tau = tree<node>;
	return tau::get(io_var)[0][1][0].is(tau::shift);
}

template <NodeType node>
int_t get_io_time_point(tref io_var) {
	using tau = tree<node>;
	return tau::get(io_var)[0][1][0].get_integer();
}

template <NodeType node>
int_t get_io_shift(tref io_var) {
	return tree<node>::get(io_var)[0][1][0][1].get_num();
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
		if (ignore_temps && get_var_name<node>(v)[0] == '_')
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
const trefs& get_free_vars(tref n) {
	using tau = tree<node>;

	static const trefs no_free_vars{};

	if (!n) return no_free_vars;
	if (typename node::type nt = tau::get(n).get_type();
		nt != tau::bf && nt != tau::wff) return no_free_vars;

	using cache_t = subtree_unordered_map<node, trefs>;
	static cache_t& free_vars_map = tau::template create_cache<cache_t>();
	// A `bf`/`wff` wrapper has exactly the free variables of what it wraps,
	// so the two are one question and are asked under one key. Keying on
	// the inner node stores one entry where the wrapper's and the
	// connective's own used to be two, and makes a query about a formula
	// meet the answer the walk published while visiting it from above.
	const tau& root = tau::get(n);
	const tref key = root.has_child()
		&& !tau::get(root.first()).has_right_sibling() ? root.first() : n;
	if (auto it = free_vars_map.find(key); it != free_vars_map.end())
		return it->second;

	DBG(LOG_TRACE << "Begin get_free_vars of " << LOG_FM(n);)
	// DAG-aware, scope-correct free-variable collection, cached per node: a
	// binder subtracts its own bound variable before anything it is nested
	// in sees the result, so a subtree's free-var set is intrinsic to it and
	// safe to cache -- unlike a naive node-identity dedup (e.g.
	// search_unique), which would still need to merge a memoized subtree's
	// contribution into every scope reaching it.
	//
	// Offset variables inside io_vars (x[t], x[t-1]) are not free
	// occurrences; excluded by not descending into variable nodes at all.
	//
	// Every set here -- a cached one, one being built, the answer -- carries
	// the shape the result is contracted to deliver: right-sibling-trimmed,
	// sorted by subtree_less, deduplicated. Holding that shape throughout is
	// what lets a cached entry be handed straight back and two sets be
	// combined by a linear merge. Trimming happens where a variable enters,
	// so it is paid once per occurrence rather than once per ancestor.

	// What a subtree contributes to the node being computed, collected in
	// two shared stacks: single variables, and whole sets that are already
	// in the delivered shape. A node records where its own entries begin and
	// drops back to that mark when it is done, so nothing allocates a
	// container of its own. Keeping loose variables loose is what makes an
	// atom free: only a node that owns a cache entry ever builds a vector.
	trefs loose;
	std::vector<const trefs*> parts;
	// Chain links already taken apart here (see `collect`). Keyed by
	// identity rather than by structure: a link is the sole child of its
	// wrapper, so it carries no right sibling and interning makes the two
	// the same question, at the price of a pointer hash instead of a tree
	// one. Flat, because a long chain inserts once per link and a
	// node-based set would allocate once per link with it.
	ankerl::unordered_dense::set<tref> opened;
	// Handed to the algorithms below as lambdas rather than as
	// `tau::subtree_less` itself, which would reach their inner loops as a
	// function pointer and so never inline.
	auto less = [](tref a, tref b) { return tau::subtree_less(a, b); };
	auto equal = [](tref a, tref b) { return tau::subtree_equals(a, b); };
	// The nodes the cache keys on: the connectives, where the walk fans out
	// and a chain forms, and the binders, where a scope closes. Term
	// connectives count as much as formula ones -- caching them is what
	// keeps a term grown one factor at a time from being taken apart again
	// at every step, and only the head of a chain is ever computed, the
	// links inside one being taken apart rather than given entries.
	auto is_cacheable = [](const tau& t) {
		return t.is(tau::wff_and) || t.is(tau::wff_or)
			|| t.is(tau::bf_and) || t.is(tau::bf_or)
			|| is_logical_or_functional_quant<node>(t.get());
	};
	// Not a node type, so no chain is open.
	static constexpr size_t no_chain = static_cast<size_t>(-1);
	// Everything collected since the two marks, as one set in the delivered
	// shape. The largest set is merged in rather than re-sorted: a
	// conjunction grown one clause at a time, where that set is the whole
	// previous answer and the rest is one clause, then costs a single pass
	// over it, while a chain taken apart into many small pieces costs one
	// sort over the variable occurrences rather than a pass over the running
	// union per piece.
	// The union of two sets that already carry the delivered shape.
	auto merged = [less](const trefs& a, const trefs& b) {
		trefs out;
		out.reserve(a.size() + b.size());
		std::set_union(a.begin(), a.end(), b.begin(), b.end(),
			std::back_inserter(out), less);
		return out;
	};
	auto combine = [&](size_t lmark, size_t mark) -> trefs {
		// Two sets and nothing loose: a connective one level above the
		// atoms, which is most of them. Merge the two and skip the rest,
		// which is there for the flattened chains.
		if (lmark == loose.size() && mark + 2 == parts.size())
			return merged(*parts[mark], *parts[mark + 1]);
		// Sets that are the same set -- one shared subformula reached
		// through several branches of the DAG is one cached vector -- are
		// merged once. Sharing makes that common, and the pass is over
		// pointers, not variables.
		std::sort(parts.begin() + mark, parts.end());
		parts.erase(std::unique(parts.begin() + mark, parts.end()),
			parts.end());
		// Everything but the largest set is sorted together with the loose
		// variables; the largest is merged into that afterwards.
		size_t big = mark;
		for (size_t i = mark + 1; i < parts.size(); ++i)
			if (parts[i]->size() > parts[big]->size()) big = i;
		trefs rest(loose.begin() + lmark, loose.end());
		for (size_t i = mark; i < parts.size(); ++i)
			if (i != big) rest.insert(rest.end(), parts[i]->begin(),
				parts[i]->end());
		std::sort(rest.begin(), rest.end(), less);
		rest.erase(std::unique(rest.begin(), rest.end(), equal), rest.end());
		if (mark == parts.size()) return rest;
		if (rest.empty()) return *parts[big];
		return merged(rest, *parts[big]);
	};
	// Appends everything @p m contributes to the node being computed.
	// @p chain is the node type of the connective chain being taken apart,
	// or `no_chain`.
	auto collect = [&](this auto&& self, tref m, size_t chain) -> void {
		const tau& t = tau::get(m);
		if (is_var_or_capture<node>(m)) {
			DBG(LOG_TRACE << "inserting var: " << LOG_FM(m);)
			// Deliberately not descending into m's children.
			loose.push_back(tau::trim_right_sibling(m));
			return;
		}
		if (t.is(tau::BDD_ID)) {
			// A BDD-backed term keeps its variables in the BDD rather than
			// in the tree. `U` is keyed by the `BDD_ID` node itself
			// (tau_bdd.h), and get_free_tau_vars already delivers the
			// shape this collects, being a union of answers from here.
			const auto& bdd_u = tau_term_bdd_handle<node>::U;
			if (auto it = bdd_u.find(tau_term_bdd_handle<node>::key_of(m));
				it != bdd_u.end())
			{
				const trefs& fv = tau_term_bdd_handle<node>
					::get_free_tau_vars(it->second.get().b);
				if (!fv.empty()) parts.push_back(&fv);
			}
			return;
		}
		if (is_cacheable(t)) {
			if (auto it = free_vars_map.find(m); it != free_vars_map.end()) {
				if (!it->second.empty()) parts.push_back(&it->second);
				return;
			}
			// A link of the chain being taken apart contributes its own
			// operands rather than a set: `a && (b && (c && ...))`, and a
			// balanced tree of it just as much, is taken apart in one go
			// and merged once. Merging link by link instead passes over
			// the whole suffix at every link, which is what made a long
			// conjunction cost its clause count squared. A link several
			// branches reach is taken apart once and then computed as a
			// node of its own, so every occurrence after that is a lookup.
			if (t.is(chain) && opened.insert(m).second) {
				for (tref c : t.children()) self(c, chain);
				return;
			}
			const size_t lmark = loose.size(), mark = parts.size();
			trefs result;
			if (is_logical_or_functional_quant<node>(m)) {
				// Fresh scope: only this binder's own subtree feeds it.
				// Its children are the bound variable and the body, in
				// that order (build_binder, tau_tree_builders.tmpl.h), so
				// the variable is skipped on the way down rather than
				// collected and taken out again afterwards.
				const tref bound = t.first();
				DBG(assert(is_var_or_capture<node>(bound));)
				for (tref c : t.children())
					if (c != bound) self(c, no_chain);
				result = combine(lmark, mark);
				if (auto it = std::lower_bound(result.begin(),
						result.end(), bound, less);
					it != result.end() && tau::subtree_equals(*it, bound))
				{
					DBG(LOG_TRACE << "removing quantified var: "
									<< LOG_FM(bound);)
					result.erase(it);
				}
			} else {
				for (tref c : t.children()) self(c, t.get_type());
				result = combine(lmark, mark);
			}
			loose.resize(lmark), parts.resize(mark);
			const trefs& published = free_vars_map.emplace(
				m, std::move(result)).first->second;
			if (!published.empty()) parts.push_back(&published);
			return;
		}
		// Anything else contributes exactly what its children do. A node
		// with a single child -- every `bf`/`wff` wrapper between two
		// connectives is one -- passes the open chain through to it;
		// anything else closes the chain.
		const size_t inner = t.has_child()
			&& !tau::get(t.first()).has_right_sibling() ? chain : no_chain;
		for (tref c : t.children()) self(c, inner);
	};
	collect(key, no_chain);
	// A cacheable key published itself on the way. A key that collected one
	// whole set and no loose variable -- an equation over a single term is
	// one -- already has its answer stored under that set's own node, and
	// is handed it rather than a copy of it under a second key. Otherwise
	// the answer is the union of what was collected, and is stored.
	const trefs& fv = [&]() -> const trefs& {
		if (auto it = free_vars_map.find(key); it != free_vars_map.end())
			return it->second;
		if (loose.empty()) {
			if (parts.empty()) return no_free_vars;
			if (parts.size() == 1) return *parts.front();
		}
		return free_vars_map.emplace(key, combine(0, 0)).first->second;
	}();
#ifdef DEBUG
	LOG_TRACE << "End get_free_vars " << LOG_FM(n);
	for (tref v : fv) LOG_TRACE << "\tfree var: " << LOG_FM(v);
	assert(std::is_sorted(fv.begin(), fv.end(), tau::subtree_less));
#endif
	return fv;
}

/**
 * @internal
 * @brief Partition formulas into connected components under shared
 * variables: two formulas land in the same group iff they are linked by a
 * chain of formulas each pair of which has a variable of @p vars in common.
 *
 * This is the factorization behind GitHub #72/#82: for a conjunction whose
 * conjuncts have pairwise disjoint variable support, `ex X (A(X1) && B(X2))`
 * with `X1`, `X2` disjoint is `ex X1 A && ex X2 B`, so each group can be
 * decided on its own instead of feeding the whole conjunction to one Boole
 * decomposition (a 2^N Shannon expansion when nothing is shared).
 * @param fms Formulas to group (order preserved within and across groups:
 *        groups are emitted in order of their first member).
 * @param vars Variables that count as links, in any order; a formula
 *        containing none of them forms a singleton group.
 * @return The groups, each a non-empty subsequence of @p fms.
 * @endinternal
 */
template <NodeType node>
std::vector<trefs> group_by_shared_vars(const trefs& fms, const trefs& vars) {
	const size_t n = fms.size();
	// Union-find over formula indices, keyed through the variables.
	std::vector<size_t> parent(n);
	for (size_t i = 0; i < n; ++i) parent[i] = i;
	auto find = [&](size_t i) {
		while (parent[i] != i) i = parent[i] = parent[parent[i]];
		return i;
	};
	auto unite = [&](size_t a, size_t b) {
		a = find(a), b = find(b);
		if (a != b) parent[std::max(a, b)] = std::min(a, b);
	};
	// Membership by content, not a binary search: callers hand in e.g. a
	// quantifier block in binder order, which is not sorted.
	const subtree_unordered_set<node> links(vars.begin(), vars.end());
	subtree_unordered_map<node, size_t> first_owner;
	for (size_t i = 0; i < n; ++i)
		for (tref v : get_free_vars<node>(fms[i])) {
			if (!links.contains(v)) continue;
			auto [it, fresh] = first_owner.emplace(v, i);
			if (!fresh) unite(it->second, i);
		}
	std::vector<trefs> groups;
	std::vector<size_t> group_of(n, std::numeric_limits<size_t>::max());
	for (size_t i = 0; i < n; ++i) {
		const size_t r = find(i);
		if (group_of[r] == std::numeric_limits<size_t>::max()) {
			group_of[r] = groups.size();
			groups.emplace_back();
		}
		groups[group_of[r]].push_back(fms[i]);
	}
	return groups;
}

/**
 * @brief Collect the free `variable` nodes of @p expression in order of
 * first appearance (pre-order), without duplicates and, unlike
 * get_free_vars, unsorted and without captures. Occurrences bound by a
 * logical or functional quantifier in scope are skipped, and `bf_ref`
 * subtrees are not entered.
 */
template <NodeType node>
trefs get_free_vars_appearance_order(tref expression) {
	using tau = tree<node>;
	std::multiset<tref, subtree_less<node>> scoped;
	trefs free_vars;
	auto f = [&scoped, &free_vars](tref n) {
		// If encounter of non-scoped and new variable, add to free_vars
		if (tau::get(n).is(tau::variable)) {
			if (!scoped.contains(n) && !subtree_vec_contains<node>(free_vars, n))
				free_vars.push_back(n);
			return false;
		} else if (tau::get(n).is(tau::bf_ref)) return false;
		// If encounter of quantifier, add quantified variable to scoped
		if (is_quantifier<node>(n) || is_functional_quantifier<node>(n)) {
			scoped.insert(tau::trim(n));
		}
		return true;
	};
	auto up = [&scoped](tref n) {
		// If quantifier is encountered, remove quantified variable from scoped
		if (is_quantifier<node>(n) || is_functional_quantifier<node>(n)) {
			scoped.extract(tau::trim(n));
		}
	};
	pre_order<node>(expression).visit_unique(f, all, up);
	return free_vars;
}

// Collects all free appearances of variables that are in bound representation
// This can happen for example in subformulas
template <NodeType node>
trefs get_free_bound_vars(tref expression) {
	using tau = tree<node>;
	auto is_number = [](const std::string& s) {
		if (s.empty()) return false;
		for (const unsigned char c : s) if (!std::isdigit(c)) return false;
		return true;
	};
	const trefs& free_vars = get_free_vars<node>(expression);
	trefs bound_vars;
	for (tref fv : free_vars) {
		const tau& fv_t = tau::get(fv);
		if (fv_t[0].is(tau::var_name) && is_number(fv_t[0].get_string()))
			bound_vars.push_back(fv);
	}
	return bound_vars;
}

// A formula has a temporal variable if either it contains an io_var with a variable or capture
// or it contains a flag
template <NodeType node>
bool has_temp_var(tref fm) {
	using tau = tree<node>;
	const auto& t = tau::get(fm);
	tref io_vars = t.find_top(is<node, tau::io_var>);
	if (io_vars == nullptr)
		return t.find_top(is<node, tau::constraint>) != nullptr;
	// any input/output stream is a temporal variable, also constant positions
	else return true;
}

template <NodeType node>
bool has_open_tau_fm_in_constant(tref fm) {
	using tau = tree<node>;
	using tt = tau::traverser;
	trefs consts = tau::get(fm).select_top(is_child<node, tau::ba_constant>);
	for (tref c : consts) {
		tref ba_const = tt(c) | tau::ba_constant | tt::ref;
		// Special case if the ba_constant is not converted to constant yet
		if (tau::get(ba_const).get_ba_constant_id() == 0) return false;
		if (!node::ba::is_closed(tt(ba_const) | tt::ba_constant)) {
			LOG_ERROR << "A Tau formula constant must be closed: "
							<< TAU_TO_STR(ba_const);
			return true;
		}
	}
	return false;
}

template<NodeType node>
bool invalid_nesting_of_temp_quants(tref fm) {
	using tau = tree<node>;
	auto temp_statements = rewriter::select_top<node>(fm,
		is_temporal_quantifier<node>);
	// Check that in no temp_statement another temporal statement is found
	for (const auto& temp_st : temp_statements) {
		if(auto n = rewriter::find_top<node>(tau::trim(temp_st),
			is_temporal_quantifier<node>); n) {
			LOG_ERROR << "Nesting of temporal quantifiers is not allowed: "
			<< "Found \"" << tau::get(n) << "\" in \"" << tau::get(temp_st) << "\"\n";
			return true;
		}
	}
	return false;
}

// If a temporal quantifier is found, all other parts of the formula
// also have to be in the scope of a temporal quantifier
template<NodeType node>
bool missing_temp_quants(tref fm) {
	using tau = tree<node>;
	if (!tau::get(fm).find_top(is_temporal_quantifier<node>))
		return false;
	// All parts of the formula have to be under a temporal quantifier
	trefs fms = tau::get(fm).select_top(is<node, tau::wff>);
	if (fms.empty()) return false;
	auto atom = [](tref n) {
		const tau& n_t = tau::get(n);
		if (n_t.is(tau::wff) || n_t.is(tau::wff_or) ||
			n_t.is(tau::wff_and) || n_t.is(tau::wff_neg))
			return false;
		return true;
	};
	for (tref f : fms) {
		if (tref a = tau::get(f).find_top_until(atom,
			is_temporal_quantifier<node>); a) {
			LOG_ERROR << "The formula \"" << tau::get(a) <<
				"\" must be scoped by a temporal quantifier" << "\n";
			return true;
		}
	}
	return false;
}

template<NodeType node>
bool invalid_nesting_of_quants(tref fm) {
	using tau = tree<node>;
	auto non_temp_quants = rewriter::select_all<node>(fm, is_quantifier<node>);
	for (tref ntq : non_temp_quants) {
		auto temp_quants = rewriter::select_all<node>(ntq,
			is_temporal_quantifier<node>);
		tref var = tau::trim(ntq);
		for (tref tq : temp_quants) {
			// Check that the non-temp quantified variable doesn't appear free
			if (subtree_vec_contains<node>(get_free_vars<node>(tq), var)) {
				LOG_ERROR << "Variable \"" << tau::get(var) << "\" is captured outside of the temporal quantifier in \"" << tau::get(tq) << "\"\n";
				return true;
			}
		}
	}
	return false;
}

template<NodeType node>
bool has_negative_offset(tref fm) {
	using tau = tree<node>;
	using tt = tau::traverser;
	auto refs = [](tref n) {
		return is<node>(n, tau::ref) ||
		       is<node>(n, tau::bf_ref) ||
			      is<node>(n, tau::wff_ref);
	};
	for (tref ref : tau::get(fm).select_top(refs)) {
		auto offsets = tt(ref) | tau::offsets;
		if (!offsets) continue;
		for (tref i : tau::get(offsets | tt::ref).select_top(is<node, tau::integer>)) {
			// Check that each integer is positive
			if (tau::get(i).get_integer() < 0) {
				LOG_ERROR << "Index in recurrence relation is negative: " << tau::get(ref);
				return true;
			}
		}
	}
	return false;
}

/**
 * @brief Return true if some `rec_relation` in @p fm has a body that is
 * itself a reference carrying a fixpoint `fallback` clause; a fallback
 * picks an iterate where a fixpoint is evaluated, so it has no meaning
 * on the defining side of a definition.
 */
template<NodeType node>
bool has_missplaced_fallback(tref fm) {
	using tau = tree<node>;
	using tt = tau::traverser;

	auto missplaced_fallback = [](tref n) {
		// Maybe reject only the same rr
		if (is<node>(n, tau::rec_relation)) {
			return (tt(n) // rec_relation
				| tt::second // body bf/wff
				| tt::first // bf_ref/wff_ref
				| tau::ref
				| tau::fp_fallback | tt::ref) != nullptr;
		};
		return false;
	};

	return tau::get(fm).find_top(missplaced_fallback) != nullptr;
}

template<NodeType node>
bool has_semantic_error(tref fm) {
	return invalid_nesting_of_quants<node>(fm)
		     || has_open_tau_fm_in_constant<node>(fm)
		     || invalid_nesting_of_temp_quants<node>(fm)
		     || missing_temp_quants<node>(fm)
		     || has_negative_offset<node>(fm)
			 || has_missplaced_fallback<node>(fm);
}

// Fast extractors (not depending in extractors, just direct access to the tree structure)

template <NodeType node>
tref get_temporally_quantified_formula(tref n) {
	using tau = tree<node>;

	if (is_child_temporal_quantifier<node>(n)) return tau::trim2(n);
	else if (is_temporal_quantifier<node>(n)) return tau::trim(n);
	return n;
}

} // namespace idni::tau_lang