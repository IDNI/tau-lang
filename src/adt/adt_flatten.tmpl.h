// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <algorithm>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "adt_flatten.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "adt"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// adt_flatten
//
// Shapes relied on below, all pre-infer_ba_types (see parser/tau.tgf and the
// "typed child stripping after inference" tests in test_ba_types_inference.cpp
// for confirmation that these survive intact with infer_ba_types = false):
//
//   variable     -> (uconst|io_var|var_name) [member_path] [typed]
//   member_path  -> member_name+                (dotted path, outer -> inner)
//   typed        -> type [subtype]               (same shape as adt_member's
//                                                  base_type, see adt_types.h)
//   io_var       -> var_name offset               (io_var_name retypes to
//                                                   var_name at parse time)
//
// Every wff-category value is wrapped in a `wff` node and every bf-category
// value in a `bf` node (one wrapper per grammar reference site -- see
// tree<node>::build_wff_and / build_bf_eq / get_0 / get_1 for the same
// convention); a quantifier's bound variable is a plain, unwrapped
// `variable` node. tree<node>::build_* static constructors (declared in
// tau_tree.h, defined in tau_tree_builders.tmpl.h) build exactly this shape,
// so they are used throughout instead of hand-rolling wrapper nodes --
// deferred template instantiation makes the forward use safe: by the time
// any of these are actually instantiated (a test .cpp calling
// adt_flatten<node_t>), tau.h has already pulled in tau_tree_builders.tmpl.h.

// One scope entry: the variable's ADT-type-name sid, plus whether this
// scope's OWN quantifier binds that variable (as opposed to the type having
// been fixed by some other, non-binding annotation found in the same
// scope -- see the "annotation propagates in scope" vs. "conflicting
// annotations" tests for why these are different variables in the same
// local map). Only a quantifier's own bound variable is ever marked
// is_binder; every other entry (including the whole outermost/global scope,
// which has no binder at all) stays false. Used by pass 2 to decide whether
// a rewritten occurrence needs its own `typed` annotation: a bound
// variable's flat members get it once, on the binder, and every other
// (body) occurrence stays bare -- exactly like an ordinary (non-ADT)
// quantified variable; a genuinely free tuple-typed variable has no binder
// to carry the annotation, so every flattened occurrence keeps it.
struct adt_scope_entry { size_t adt_sid; bool is_binder = false; };

// A scope stack of var-sid -> adt_scope_entry maps, one map per open
// quantifier scope plus the outermost/global one (index 0). Not templated on
// node: sids are plain dict ids either way.
using adt_scope_stack = std::vector<std::map<size_t, adt_scope_entry>>;

template <NodeType node>
tref adt_flatten_find_child(tref n, size_t nt) {
	using tau = tree<node>;
	for (tref c : tau::get(n).get_children())
		if (tau::get(c).is(nt)) return c;
	return nullptr;
}

// The identifying dict id of a variable's head (var_name/io_var/uconst),
// ignoring an io_var's offset -- the key scope maps are keyed on, so that
// e.g. `x` and `x.a` (and, once io is handled, `p[t]` and `p[t-1]`) share
// one scope entry.
template <NodeType node>
size_t adt_flatten_var_key(tref head) {
	using tau = tree<node>;
	auto t = tau::get(head);
	if (t.is(tau::io_var)) return tau::get(t.first()).data();
	if (t.is(tau::uconst)) return tau::get(t.only_child()).data();
	return t.data();
}

template <NodeType node>
std::string adt_flatten_head_str(tref head) {
	return dict(adt_flatten_var_key<node>(head));
}

// Dotted description of a `variable` node (head + member_path, ignoring any
// `typed` child) for error messages -- e.g. `x` or `l.p.a`.
template <NodeType node>
std::string adt_flatten_describe_var(tref var_node) {
	using tau = tree<node>;
	std::string s = adt_flatten_head_str<node>(tau::get(var_node).first());
	if (tref mp = adt_flatten_find_child<node>(var_node, tau::member_path); mp)
		for (tref c : tau::get(mp).get_children())
			s += "." + dict(tau::get(c).data());
	return s;
}

// -----------------------------------------------------------------------------
// Shared ADT-type resolution for one `variable` node (bare or with a member
// path), used identically by generic-context rewriting, tuple-equality
// classification, and quantifier-binder expansion.

template <NodeType node>
struct adt_resolution {
	enum kind_t { k_not_adt, k_alias, k_full_leaf, k_partial } kind = k_not_adt;
	tref alias_target = nullptr;   ///< valid iff kind == k_alias
	bool locally_typed = false;    ///< this occurrence's own `typed` gave the type
	bool is_bound = false;         ///< type came from a scope entry whose quantifier
	                                ///< binds this variable (k_full_leaf/k_partial only)
	std::string flat_name;         ///< valid iff kind == k_full_leaf
	tref base_type = nullptr;      ///< valid iff kind == k_full_leaf
	std::vector<std::pair<std::string, tref>> members; ///< valid iff kind == k_partial
};

/**
 * Resolve @p var_node's ADT type (from its own `typed` child, or from the
 * scope stack) and, if it names a tuple type, resolve its member path (empty
 * path = the whole tuple) against the registry. Returns `std::nullopt` after
 * `LOG_ERROR` on: member access with no known ADT type in scope, member
 * access through a non-tuple (alias or past a leaf), or an unknown member.
 */
template <NodeType node>
std::optional<adt_resolution<node>> adt_resolve_var(tref var_node,
		const adt_registry<node>& reg, const adt_scope_stack& scopes)
{
	using tau = tree<node>;
	using tt = typename tau::traverser;

	adt_resolution<node> r;
	tref head = tau::get(var_node).first();
	tref member_path_node = adt_flatten_find_child<node>(var_node, tau::member_path);
	tref typed_node = adt_flatten_find_child<node>(var_node, tau::typed);

	std::vector<size_t> path_sids;
	if (member_path_node)
		for (tref c : tau::get(member_path_node).get_children())
			path_sids.push_back(tau::get(c).data());

	size_t adt_sid = 0; bool have = false;
	if (typed_node) {
		size_t tname = tt(typed_node) | tau::type | tt::data;
		if (reg.defines(tname)) { adt_sid = tname; have = true; r.locally_typed = true; }
	}
	bool is_bound = false;
	if (!have) {
		size_t key = adt_flatten_var_key<node>(head);
		for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
			if (auto f = it->find(key); f != it->end())
				{ adt_sid = f->second.adt_sid; is_bound = f->second.is_binder;
					have = true; break; }
	}
	if (!have) {
		if (!path_sids.empty()) {
			LOG_ERROR << "(Error) ADT: member access on '"
				<< adt_flatten_head_str<node>(head)
				<< "' with no known ADT type in scope\n";
			return std::nullopt;
		}
		r.kind = adt_resolution<node>::k_not_adt;
		return r;
	}
	if (reg.is_alias(adt_sid)) {
		if (!path_sids.empty()) {
			LOG_ERROR << "(Error) ADT: member access on '"
				<< adt_flatten_head_str<node>(head)
				<< "' through a non-tuple type\n";
			return std::nullopt;
		}
		r.kind = adt_resolution<node>::k_alias;
		r.alias_target = reg.alias_target(adt_sid);
		return r;
	}

	// tuple type: resolve path_sids as a prefix against the flat member list
	const auto& mems = reg.members(adt_sid);
	std::vector<const adt_member<node>*> cands;
	for (const auto& m : mems)
		if (m.path.size() >= path_sids.size()
			&& std::equal(path_sids.begin(), path_sids.end(), m.path.begin()))
			cands.push_back(&m);
	if (cands.empty()) {
		bool overflow = false;
		for (const auto& m : mems)
			if (m.path.size() < path_sids.size()
				&& std::equal(m.path.begin(), m.path.end(), path_sids.begin()))
				{ overflow = true; break; }
		LOG_ERROR << "(Error) ADT: "
			<< (overflow ? "member access on a non-tuple member of '"
				: "unknown member in '")
			<< adt_flatten_head_str<node>(head) << "'\n";
		return std::nullopt;
	}

	r.is_bound = is_bound;
	std::string hs = adt_flatten_head_str<node>(head);
	const adt_member<node>* full_m = nullptr;
	for (auto* m : cands) if (m->path.size() == path_sids.size()) { full_m = m; break; }
	if (full_m) {
		r.kind = adt_resolution<node>::k_full_leaf;
		r.flat_name = hs;
		for (size_t sid : full_m->path) r.flat_name += "." + dict(sid);
		r.base_type = full_m->base_type;
		return r;
	}

	r.kind = adt_resolution<node>::k_partial;
	for (auto* m : cands) {
		std::string name = hs;
		for (size_t sid : m->path) name += "." + dict(sid);
		r.members.emplace_back(std::move(name), m->base_type);
	}
	return r;
}

// -----------------------------------------------------------------------------
// Pass 1: scope-aware ADT typing.
//
// Collects the LOCAL registry-type annotations of one scope (the region
// spanned by a quantifier's bound variable and body, or the whole spec for
// the outermost/global scope), without crossing into a nested quantifier's
// own scope, NOR into a rec_relation's own scope (a definition's formal
// parameters and body are that definition's own scope, isolated from
// whatever contains the definitions list -- see
// adt_flatten_rewrite_rec_relation; without this, a tuple-typed formal's
// annotation would leak into the enclosing, often global, scope and could
// spuriously "conflict" with an unrelated definition's own same-named
// formal). Called once per scope, at scope-open time, so that "an
// annotation anywhere in the scope fixes it" holds regardless of where in
// the scope's text the annotation and the member access appear relative to
// each other.

template <NodeType node>
bool adt_flatten_collect_local(tref n, const adt_registry<node>& reg,
		std::map<size_t, adt_scope_entry>& local)
{
	using tau = tree<node>;
	using tt = typename tau::traverser;
	if (!n) return true;
	auto t = tau::get(n);
	if (is_logical_or_functional_quant<node>(n)) return true; // nested scope
	if (t.is(tau::rec_relation)) return true; // rec_relation's own scope
	if (t.is(tau::variable)) {
		if (tref typed_node = adt_flatten_find_child<node>(n, tau::typed); typed_node) {
			size_t tname = tt(typed_node) | tau::type | tt::data;
			if (reg.defines(tname)) {
				size_t key = adt_flatten_var_key<node>(t.first());
				auto [it, inserted] = local.try_emplace(key, adt_scope_entry{ tname, false });
				if (!inserted && it->second.adt_sid != tname) {
					LOG_ERROR << "(Error) ADT: conflicting type annotations "
						"for variable '" << adt_flatten_head_str<node>(t.first())
						<< "'\n";
					return false;
				}
			}
		}
		return true;
	}
	for (tref c : t.get_children())
		if (!adt_flatten_collect_local<node>(c, reg, local)) return false;
	return true;
}

// -----------------------------------------------------------------------------
// Pass 2: bottom-up rewrite.

template <NodeType node>
std::optional<tref> adt_flatten_rewrite(tref n, const adt_registry<node>& reg,
		adt_scope_stack scopes);

// Rule 1 (generic context) / rule 5 (errors): a `variable` node reached
// anywhere other than as a quantifier's bound variable or a direct bf_eq/
// bf_neq operand -- i.e. any "other bf context".
template <NodeType node>
std::optional<tref> adt_flatten_rewrite_variable(tref n,
		const adt_registry<node>& reg, const adt_scope_stack& scopes)
{
	using tau = tree<node>;
	auto res = adt_resolve_var<node>(n, reg, scopes);
	if (!res) return std::nullopt;
	switch (res->kind) {
	case adt_resolution<node>::k_not_adt:
		return n;
	case adt_resolution<node>::k_alias:
		if (res->locally_typed)
			return tau::get(tau::variable, tau::get(n).first(), res->alias_target);
		return n;
	case adt_resolution<node>::k_full_leaf:
		// A bound variable's flat member is annotated once, on its binder
		// (see adt_flatten_rewrite_quantifier); every other occurrence of it
		// stays bare, exactly like an ordinary quantified variable. A free
		// (unbound) tuple-typed variable has no binder to carry the
		// annotation, so every flattened occurrence keeps it.
		return res->is_bound
			? tau::get(tau::variable, tau::build_var_name(res->flat_name))
			: tau::get(tau::variable, tau::build_var_name(res->flat_name),
				res->base_type);
	case adt_resolution<node>::k_partial:
		LOG_ERROR << "(Error) ADT: tuple-typed term '"
			<< adt_flatten_describe_var<node>(n) << "' used outside an "
			"equality or quantifier context\n";
		return std::nullopt;
	}
	return n; // unreachable
}

// Rule 3: a quantifier over a tuple-typed variable expands into one binder
// per flat member, in flat member order; an ordinary (non-tuple) binder is
// rewritten in place (alias -> base type) if needed.
template <NodeType node>
std::optional<tref> adt_flatten_rewrite_quantifier(tref n, size_t nt,
		const adt_registry<node>& reg, adt_scope_stack scopes)
{
	using tau = tree<node>;
	tref bound_var = tau::get(n).first();
	tref body = tau::get(n).second();

	std::map<size_t, adt_scope_entry> local;
	if (!adt_flatten_collect_local<node>(bound_var, reg, local)) return std::nullopt;
	if (!adt_flatten_collect_local<node>(body, reg, local)) return std::nullopt;
	// Mark this scope's own bound variable (not any other annotation found
	// while scanning the body) as the one whose flattened occurrences may
	// stay bare -- see adt_scope_entry's comment.
	if (auto it = local.find(adt_flatten_var_key<node>(tau::get(bound_var).first()));
		it != local.end())
		it->second.is_binder = true;
	scopes.push_back(std::move(local));

	auto res = adt_resolve_var<node>(bound_var, reg, scopes);
	if (!res) return std::nullopt;

	std::vector<tref> member_vars;
	switch (res->kind) {
	case adt_resolution<node>::k_not_adt:
		member_vars.push_back(bound_var);
		break;
	case adt_resolution<node>::k_alias:
		member_vars.push_back(res->locally_typed
			? tau::get(tau::variable, tau::get(bound_var).first(), res->alias_target)
			: bound_var);
		break;
	case adt_resolution<node>::k_full_leaf:
		member_vars.push_back(tau::get(tau::variable,
			tau::build_var_name(res->flat_name), res->base_type));
		break;
	case adt_resolution<node>::k_partial:
		for (auto& [name, bt] : res->members)
			member_vars.push_back(tau::get(tau::variable, tau::build_var_name(name), bt));
		break;
	}

	auto new_body = adt_flatten_rewrite<node>(body, reg, scopes);
	if (!new_body) return std::nullopt;

	tref subformula = *new_body; // already wrapped (wff for wff_all/ex, bf for bf_fall/fex)
	for (auto it = member_vars.rbegin(); it != member_vars.rend(); ++it) {
		switch (nt) {
		case tau::wff_all: subformula = tau::build_wff_all(*it, subformula, false); break;
		case tau::wff_ex:  subformula = tau::build_wff_ex(*it, subformula, false); break;
		case tau::bf_fall: subformula = tau::build_bf_fall(*it, subformula); break;
		default:           subformula = tau::build_bf_fex(*it, subformula); break;
		}
	}
	return tau::get(subformula).first(); // unwrap: caller rewraps
}

// Rule 4 (ref-argument expansion): shared arg-list rewriter used both for a
// rec_relation's own HEAD (formal declarations: `f(x:Point, v)`, dispatched
// from adt_flatten_rewrite_rec_relation below) and for an ordinary CALL SITE
// ref's actual arguments (`f(y, w)`, dispatched from this file's generic
// switch via `case tau::ref:`, for wff_ref/bf_ref call sites and for a
// rec_relation body that is itself a bare ref). A ref_arg whose bf is a
// plain variable or a full member path reaching a tuple type -- the only two
// shapes design section 3 rule 4 allows to fill a tuple-typed slot -- splices
// into one ref_arg per flat member (both shapes already resolve to
// adt_resolve_var's k_partial: an empty member_path lists every flat member,
// a path landing on a nested tuple boundary lists that subtree's). Anything
// else is delegated to the ordinary generic recursive rewrite: a tuple-typed
// variable nested inside some other bf operation (`y'`, `y | z`, ...) already
// errors there via rule 5's "used outside an equality or quantifier context"
// (adt_flatten_rewrite_variable's k_partial branch) -- exactly the "any other
// bf ... = error" design rule, with no extra code needed here. (NOTE: `y | y`
// -- same operand on both sides -- is not a usable example here: it folds to
// bare `y` during parsing itself, before adt_flatten ever runs, per Boolean
// idempotence; the "non-variable tuple arg" test below uses `y'` instead.)
//
// @p head_style selects how a flat member is annotated: true (a
// rec_relation's own formal declarations) always types every flat member,
// mirroring how a quantifier types its own binder (adt_flatten_rewrite_quantifier);
// false (an actual call-site argument) types a flat member only if the
// source variable is free in the current scope (!adt_resolution::is_bound),
// mirroring every other occurrence rewrite in this file
// (adt_flatten_rewrite_variable's k_full_leaf case, mk_var_bf in
// adt_flatten_rewrite_equality) -- a bound tuple variable's members are typed
// once, on their binder, and stay bare everywhere else, including as call
// arguments.
template <NodeType node>
std::optional<std::pair<trefs, bool>> adt_flatten_rewrite_ref_args(
		tref ref_args_node, const adt_registry<node>& reg,
		const adt_scope_stack& scopes, bool head_style)
{
	using tau = tree<node>;
	trefs new_args;
	bool changed = false;
	for (tref ra : tau::get(ref_args_node).get_children()) {
		tref arg_bf = tau::get(ra).first();
		tref content = tau::get(arg_bf).first();
		if (tau::get(content).is(tau::variable)) {
			auto res = adt_resolve_var<node>(content, reg, scopes);
			if (!res) return std::nullopt;
			if (res->kind == adt_resolution<node>::k_partial) {
				changed = true;
				bool bare = !head_style && res->is_bound;
				for (auto& [name, bt] : res->members) {
					tref v = bare
						? tau::get(tau::variable, tau::build_var_name(name))
						: tau::get(tau::variable, tau::build_var_name(name), bt);
					new_args.push_back(tau::get(tau::ref_arg, tau::get(tau::bf, v)));
				}
				continue;
			}
		}
		auto rc = adt_flatten_rewrite<node>(ra, reg, scopes);
		if (!rc) return std::nullopt;
		changed = changed || (*rc != ra);
		new_args.push_back(*rc);
	}
	return std::make_pair(std::move(new_args), changed);
}

// Rewrites one `ref` node (`sym [offsets] ref_args [typed] [fallback]`) in
// full: its ref_args (adt_flatten_rewrite_ref_args above); its own result
// `typed` -- a tuple-typed result (`f(v):Point`) is always an error per
// design section 5 (this used to silently pass through and crash downstream
// inference instead of failing cleanly here); an alias-typed result is
// rewritten to the alias target, same as any other alias annotation; a
// non-registry result type passes through untouched, same as everywhere
// else; its `fallback` and any `offsets`, both recursed into generically (a
// tuple-typed term surfacing there other than through a ref_arg slot already
// errors via the same rule-5 path as everything else -- see
// "fp_fallback with tuple-typed content fails" test). Used for both a
// rec_relation's own head (@p head_style true, called directly from
// adt_flatten_rewrite_rec_relation -- a rec_relation head is never reached
// through the generic dispatch below) and an ordinary call site (false,
// dispatched from `case tau::ref:`).
template <NodeType node>
std::optional<tref> adt_flatten_rewrite_ref(tref n, const adt_registry<node>& reg,
		const adt_scope_stack& scopes, bool head_style)
{
	using tau = tree<node>;
	using tt = typename tau::traverser;

	tref sym = tau::get(n).first();
	tref ref_args_node = adt_flatten_find_child<node>(n, tau::ref_args);
	tref typed_node = adt_flatten_find_child<node>(n, tau::typed);
	tref fallback_node = adt_flatten_find_child<node>(n, tau::fp_fallback);
	tref offsets_node = adt_flatten_find_child<node>(n, tau::offsets);

	auto args_res = adt_flatten_rewrite_ref_args<node>(ref_args_node, reg, scopes, head_style);
	if (!args_res) return std::nullopt;
	auto& [new_args, changed] = *args_res;

	tref new_typed = typed_node;
	if (typed_node) {
		size_t tname = tt(typed_node) | tau::type | tt::data;
		if (reg.defines(tname)) {
			if (!reg.is_alias(tname)) {
				LOG_ERROR << "(Error) ADT: tuple-typed ref result on '"
					<< adt_flatten_head_str<node>(sym) << "' is not allowed\n";
				return std::nullopt;
			}
			new_typed = reg.alias_target(tname);
			changed = true;
		}
	}

	tref new_fallback = fallback_node;
	if (fallback_node) {
		auto rc = adt_flatten_rewrite<node>(fallback_node, reg, scopes);
		if (!rc) return std::nullopt;
		new_fallback = *rc;
		changed = changed || (new_fallback != fallback_node);
	}

	tref new_offsets = offsets_node;
	if (offsets_node) {
		auto rc = adt_flatten_rewrite<node>(offsets_node, reg, scopes);
		if (!rc) return std::nullopt;
		new_offsets = *rc;
		changed = changed || (new_offsets != offsets_node);
	}

	if (!changed) return n;
	trefs children{ sym };
	if (new_offsets) children.push_back(new_offsets);
	children.push_back(tau::get(tau::ref_args, new_args));
	if (new_typed) children.push_back(new_typed);
	if (new_fallback) children.push_back(new_fallback);
	return tau::get(tau::ref, children);
}

// A rec_relation (`f(args) := body`) is its own scope: its formal
// parameters' annotations (a valid ADT-typing source per design section 3,
// pass 1: "a ref formal") are visible to member-path resolution in its own
// body only, isolated from whatever contains the definitions list (sibling
// definitions, `main`) -- see adt_flatten_collect_local's matching stop
// condition. Each formal found in the collected local scope is additionally
// marked as this scope's own binder -- exactly like a quantifier's bound
// variable (adt_flatten_rewrite_quantifier) -- so that, like a bound
// variable's flat members, the HEAD's own flattened formal always carries
// its type (adt_flatten_rewrite_ref_args's head_style=true) while every BODY
// occurrence of it resolves is_bound and stays bare, matching a quantified
// variable's binder-vs-body split exactly (Task 6's ref-arg rule: see
// adt_flatten_rewrite_ref/adt_flatten_rewrite_ref_args above).
template <NodeType node>
std::optional<tref> adt_flatten_rewrite_rec_relation(tref n,
		const adt_registry<node>& reg, adt_scope_stack scopes)
{
	using tau = tree<node>;
	tref head = tau::get(n).first();
	tref body = tau::get(n).second();

	std::map<size_t, adt_scope_entry> local;
	if (!adt_flatten_collect_local<node>(head, reg, local)) return std::nullopt;
	if (!adt_flatten_collect_local<node>(body, reg, local)) return std::nullopt;
	if (tref ref_args_node = adt_flatten_find_child<node>(head, tau::ref_args); ref_args_node)
		for (tref ra : tau::get(ref_args_node).get_children()) {
			tref content = tau::get(tau::get(ra).first()).first();
			if (!tau::get(content).is(tau::variable)) continue;
			size_t key = adt_flatten_var_key<node>(tau::get(content).first());
			if (auto it = local.find(key); it != local.end()) it->second.is_binder = true;
		}
	scopes.push_back(std::move(local));

	auto new_head = adt_flatten_rewrite_ref<node>(head, reg, scopes, true);
	if (!new_head) return std::nullopt;
	auto new_body = adt_flatten_rewrite<node>(body, reg, scopes);
	if (!new_body) return std::nullopt;
	if (*new_head == head && *new_body == body) return n;
	return tau::get(tau::rec_relation, *new_head, *new_body);
}

// Rule 2: `=`/`!=` with a tuple-typed side expands into a conjunction (for
// `=`) / disjunction (for `!=`) of member-wise atoms. The other side must be
// a same-shaped tuple term or the constant 0/1 (broadcast to every member);
// anything else is a shape mismatch.
template <NodeType node>
std::optional<tref> adt_flatten_rewrite_equality(tref n, size_t nt,
		const adt_registry<node>& reg, const adt_scope_stack& scopes)
{
	using tau = tree<node>;

	tref l_bf = tau::get(n).first(), r_bf = tau::get(n).second();
	tref l_content = tau::get(l_bf).first(), r_content = tau::get(r_bf).first();

	struct side { bool is_partial = false; bool is_bound = false;
		std::vector<std::pair<std::string, tref>> members; };
	auto classify = [&](tref content) -> std::optional<side> {
		if (!tau::get(content).is(tau::variable)) return side{};
		auto res = adt_resolve_var<node>(content, reg, scopes);
		if (!res) return std::nullopt;
		if (res->kind == adt_resolution<node>::k_partial)
			return side{ true, res->is_bound, std::move(res->members) };
		return side{};
	};
	auto lc = classify(l_content); if (!lc) return std::nullopt;
	auto rc = classify(r_content); if (!rc) return std::nullopt;
	bool l_const = tau::get(l_content).is(tau::bf_t) || tau::get(l_content).is(tau::bf_f);
	bool r_const = tau::get(r_content).is(tau::bf_t) || tau::get(r_content).is(tau::bf_f);
	const char* op = nt == tau::bf_eq ? "equality" : "inequality";
	auto describe_side = [&](tref content) -> std::string {
		if (tau::get(content).is(tau::variable))
			return adt_flatten_describe_var<node>(content);
		if (tau::get(content).is(tau::bf_t)) return "1";
		if (tau::get(content).is(tau::bf_f)) return "0";
		return "<term>";
	};

	auto make_atom = [&](tref lo, tref ro) -> tref {
		return nt == tau::bf_eq ? tau::build_bf_eq(lo, ro) : tau::build_bf_neq(lo, ro);
	};
	auto combine = [&](tref acc, tref atom) -> tref {
		return nt == tau::bf_eq ? tau::build_wff_and(acc, atom) : tau::build_wff_or(acc, atom);
	};
	// Same bare-if-bound rule as adt_flatten_rewrite_variable's k_full_leaf
	// case: an expanded member atom of a bound tuple variable stays bare
	// (the binder already carries the type), a free one keeps it.
	auto mk_var_bf = [&](const std::string& name, tref base_type, bool is_bound) -> tref {
		tref v = is_bound ? tau::get(tau::variable, tau::build_var_name(name))
			: tau::get(tau::variable, tau::build_var_name(name), base_type);
		return tau::get(tau::bf, v);
	};

	if (!lc->is_partial && !rc->is_partial) {
		auto nl = adt_flatten_rewrite<node>(l_bf, reg, scopes);
		if (!nl) return std::nullopt;
		auto nr = adt_flatten_rewrite<node>(r_bf, reg, scopes);
		if (!nr) return std::nullopt;
		return tau::get(make_atom(*nl, *nr)).first();
	}

	std::vector<tref> atoms;
	if (lc->is_partial && rc->is_partial) {
		if (lc->members.size() != rc->members.size()) {
			LOG_ERROR << "(Error) ADT: shape mismatch in tuple " << op << ": '"
				<< describe_side(l_content) << "' has " << lc->members.size()
				<< " member(s), '" << describe_side(r_content) << "' has "
				<< rc->members.size() << "\n";
			return std::nullopt;
		}
		for (size_t i = 0; i < lc->members.size(); ++i) {
			if (!is_same_ba_type<node>(lc->members[i].second, rc->members[i].second)) {
				LOG_ERROR << "(Error) ADT: shape mismatch in tuple " << op
					<< " between '" << describe_side(l_content) << "' and '"
					<< describe_side(r_content) << "': member types differ ('"
					<< lc->members[i].first << "' vs '" << rc->members[i].first
					<< "')\n";
				return std::nullopt;
			}
			atoms.push_back(make_atom(
				mk_var_bf(lc->members[i].first, lc->members[i].second, lc->is_bound),
				mk_var_bf(rc->members[i].first, rc->members[i].second, rc->is_bound)));
		}
	} else if (lc->is_partial && r_const) {
		for (auto& [name, bt] : lc->members)
			atoms.push_back(make_atom(mk_var_bf(name, bt, lc->is_bound), r_bf));
	} else if (rc->is_partial && l_const) {
		for (auto& [name, bt] : rc->members)
			atoms.push_back(make_atom(l_bf, mk_var_bf(name, bt, rc->is_bound)));
	} else {
		bool l_is_tuple = lc->is_partial;
		LOG_ERROR << "(Error) ADT: shape mismatch: '"
			<< describe_side(l_is_tuple ? l_content : r_content)
			<< "' is tuple-typed but '"
			<< describe_side(l_is_tuple ? r_content : l_content)
			<< "' is not a matching tuple term or the constant 0/1\n";
		return std::nullopt;
	}

	tref combined = atoms[0];
	for (size_t i = 1; i < atoms.size(); ++i) combined = combine(combined, atoms[i]);
	return tau::get(combined).first();
}

// Generic recursive dispatch: quantifiers and `=`/`!=` are special-cased,
// `variable` nodes are resolved/rewritten in place, rec_relation opens its
// own scope (formals untouched, see adt_flatten_rewrite_rec_relation),
// `definitions`/`spec_multiline` have their `type_def` children erased,
// `wff`/`bf` wrappers and every other node kind (`spec`/`start`, `main`,
// wff_and, bf_or, ref, ref_args, offsets, ...) are rebuilt bottom-up with
// recursively rewritten children, reusing the original node (no
// reconstruction) when nothing under it changed. This is deliberately
// agnostic about which nonterminal is the tree's actual root (`spec` vs.
// whatever the grammar's start symbol really materializes as, or
// `spec_multiline` when a caller parses with that start symbol instead --
// e.g. REPL scripting): `definitions`/`spec_multiline` are found and erased
// wherever normal recursive descent from the root reaches them, and if
// erasing every type_def empties a `definitions` (which, unlike
// `spec_multiline`, cannot itself be empty per the grammar's `+` repetition
// -- `spec_multiline`'s own `_` alternative already allows zero items, so an
// emptied one is a legitimately printable node as-is), a child that comes
// back as a null tref (adt_flatten_rewrite's own "drop me" signal, distinct
// from std::nullopt's "error") is simply omitted from its parent's rebuilt
// children list -- so an emptied `definitions` node disappears the same way
// it would if the source had no definitions at all, however many/few levels
// above the root that turns out to be.
template <NodeType node>
std::optional<tref> adt_flatten_rewrite(tref n, const adt_registry<node>& reg,
		adt_scope_stack scopes)
{
	using tau = tree<node>;
	auto t = tau::get(n);
	auto nt = t.get_type(); // node::type; kept as such (not size_t) so it can
	                         // be passed straight to tau::get_typed() below
	switch (nt) {
	case tau::wff_all: case tau::wff_ex:
	case tau::bf_fall:  case tau::bf_fex:
		return adt_flatten_rewrite_quantifier<node>(n, nt, reg, std::move(scopes));
	case tau::bf_eq: case tau::bf_neq:
		return adt_flatten_rewrite_equality<node>(n, nt, reg, scopes);
	case tau::variable:
		return adt_flatten_rewrite_variable<node>(n, reg, scopes);
	case tau::ref:
		// A CALL SITE ref (inside wff_ref/bf_ref, or a rec_relation body
		// that is itself a bare ref) -- a rec_relation's own HEAD ref is
		// never reached here, see adt_flatten_rewrite_rec_relation.
		return adt_flatten_rewrite_ref<node>(n, reg, scopes, false);
	case tau::rec_relation:
		return adt_flatten_rewrite_rec_relation<node>(n, reg, std::move(scopes));
	case tau::definitions: {
		trefs kept;
		for (tref g : t.get_children()) {
			if (tau::get(g).is(tau::type_def)) continue; // erased
			auto rc = adt_flatten_rewrite<node>(g, reg, scopes);
			if (!rc) return std::nullopt;
			kept.push_back(*rc);
		}
		if (kept.empty()) return tref{ nullptr }; // signal: drop this node
		return tau::get(tau::definitions, kept);
	}
	case tau::spec_multiline: {
		trefs kept;
		for (tref g : t.get_children()) {
			if (tau::get(g).is(tau::type_def)) continue; // erased
			auto rc = adt_flatten_rewrite<node>(g, reg, scopes);
			if (!rc) return std::nullopt;
			kept.push_back(*rc);
		}
		return tau::get(tau::spec_multiline, kept); // may legitimately be empty
	}
	default: break;
	}

	trefs kids = t.get_children();
	if (kids.empty()) return n; // leaf: nothing to rewrite
	trefs new_kids;
	new_kids.reserve(kids.size());
	bool changed = false;
	for (tref c : kids) {
		auto rc = adt_flatten_rewrite<node>(c, reg, scopes);
		if (!rc) return std::nullopt;
		if (*rc == nullptr) { changed = true; continue; } // dropped (emptied definitions)
		changed = changed || (*rc != c);
		new_kids.push_back(*rc);
	}
	if (!changed) return n;
	return tau::get_typed(nt, new_kids, t.get_ba_type());
}

// -----------------------------------------------------------------------------
// Top-level entry point.

template <NodeType node>
tref adt_flatten(tref spec, [[maybe_unused]] io_context<node>* ctx) {
	auto reg_opt = adt_registry<node>::build(spec);
	if (!reg_opt) return nullptr; // adt_registry::build already LOG_ERROR'd

	// Fast path. Idempotence relies on this being solely "registry empty",
	// not also "no member_path nodes": the first flatten erases every
	// type_def, so re-flattening its printed-and-reparsed output (which
	// re-parses "x.a" as a bare variable "x" plus a member_path ".a", per
	// the design's print/parse-idempotence argument) runs against an EMPTY
	// registry. With an empty registry there is no ADT type any variable
	// could carry, so every member_path left in the tree is unreachable via
	// pass 1 (nothing can ever resolve to a registry type) and is therefore
	// not an ADT construct at all -- returning the tree unchanged is exactly
	// the fixed point flatten(flatten(x)) == flatten(x) requires.
	if (reg_opt->empty()) return spec;
	const adt_registry<node>& reg = *reg_opt;

	adt_scope_stack scopes;
	std::map<size_t, adt_scope_entry> global;
	if (!adt_flatten_collect_local<node>(spec, reg, global)) return nullptr;
	scopes.push_back(std::move(global));

	// `definitions` (and, within it, type_def erasure) is handled generically
	// by adt_flatten_rewrite wherever normal recursive descent from @p spec
	// reaches it -- see that function's header comment. No special-casing of
	// @p spec's own shape is needed here.
	auto rc = adt_flatten_rewrite<node>(spec, reg, scopes);
	return (rc && *rc) ? *rc : nullptr;
}

} // namespace idni::tau_lang
