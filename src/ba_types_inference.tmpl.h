// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <map>
#include <memory>
#include <vector>
#include <ranges>
#include <limits.h>

#include "tau_tree.h"
#include "union_find.h"
#include "boolean_algebras/bv_ba.h"
#include "ba_types.h"
#include "type_scoped_resolver.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "inference"

namespace idni::tau_lang {

// A ba_constant whose source text could not be parsed under the type id
// that inference assigned to it.
struct parse_error {
	tref element;
	size_t type_id;
};

template<NodeType node>
using typeables_type_id_map = std::map<size_t, subtree_map<node, size_t>>;

// True for the node kinds that can carry a BA type and take part in
// inference: variables, BA constants, bf_t/bf_f and references. Default
// query of get_typeable_type_ids_by_type.
template<NodeType node>
bool is_typeable(tref t) {
	using tau = tree<node>;

	return is<node, tau::variable>(t)
		|| is<node, tau::ba_constant>(t)
		|| is<node, tau::bf_t>(t)
		|| is<node, tau::bf_f>(t)
		|| is<node, tau::ref>(t);
}

// Logs, at debug level, that @p var received the default type inside the
// environment @p env; @p bv selects the "Default bv width" wording. The
// type name is omitted for io variables and ba_constants.
template <NodeType node>
void default_typing_message(tref var, tref env, const bool bv = false) {
	using tau = tree<node>;

	const std::string type_info = (is_io_var<node>(var)|| tau::get(var).is(tau::ba_constant))
		? "" : get_ba_type_name<node>(tau::get(var).get_ba_type());
	const std::string message = bv ? "(Default bv width) " : "(Default typing) ";
	LOG_DEBUG << message << tau::get(var) << type_info << " in " << tau::get(env) << "\n";
}

// BA2-20: canonize is the type-inference KEY builder -- it untypes,
// unwraps a `bf` wrapper, and rewrites any io_var to
// variable(io_var(var_name)) DROPPING the offset/shift. That last part is
// load-bearing: it is what makes typed history entries of one stream match
// across time points, so scope maps stay per-stream, not per-instant.
template<NodeType node>
tref canonize(tref t) {
	using tau = tree<node>;
	using tt = tau::traverser;

	tref new_t = untype<node>(t);
	if (tau::get(new_t).is(tau::bf)) new_t = tau::trim(new_t);
	if (auto var_name = tt(new_t) | tau::io_var | tau::var_name | tt::ref; var_name)
		new_t = tau::get(tau::variable, tau::get(tau::io_var, { var_name }));
	// DBG(LOG_TRACE << "canonize/t -> new_t:\n"
	// 	<< LOG_FM_TREE(t) << " -> " << LOG_FM_TREE(new_t);)
	return new_t;
}

// Element-wise canonize.
template<NodeType node>
trefs canonize(trefs ts) {
	trefs new_ts;
	for (auto t : ts)
		new_ts.push_back(canonize<node>(t));
	return new_ts;
}

// Returns true if n has already been typed (ba_type != 0).
template<NodeType node>
bool is_processed(tref n) {
	return tree<node>::get(n).get_ba_type() != 0;
}

// Descends through the single-child wrappers the grammar puts around a
// reference (`bf` > `bf_ref` > `ref`, `wff` > `wff_ref` > `ref`, and the
// `wff` > `wff_ref` > `bf` > `bf_ref` > `ref` mix a reclassified head can
// carry) and returns the `ref` inside, or nullptr if @p n does not wrap one.
template<NodeType node>
tref unwrap_to_ref(tref n) {
	using tau = tree<node>;
	while (n && !tau::get(n).is(tau::ref)) {
		const auto& t = tau::get(n);
		if (t.children_size() != 1) return nullptr;
		n = t.first();
	}
	return n;
}

// Returns the signature <symbol id, offset arity, argument arity>
// identifying a reference. @p func must be a `ref` or wrap one (see
// unwrap_to_ref); anything else is a debug-assert violation.
template <NodeType node>
std::tuple<size_t, int_t, int_t> get_function_signature(tref func) {
	using tau = tree<node>;
	using tt = tau::traverser;

	// We accept any of the wrappers a reference can come in, not just
	// bf > bf_ref > ref: a definition head reclassified as functional is
	// still wrapped in the wff_ref the parser gave it.
	if (tref ref = unwrap_to_ref<node>(func); ref) func = ref;
	DBG(assert(tau::get(func).is(tau::ref)));

	const tau& ref_head = tau::get(func);
	size_t sym = ref_head[0].value.data;
	int_t offset_args = 0;
	if (auto o = tt(ref_head) | tau::offsets; o)
		offset_args = tau::get(o | tt::ref).children_size();
	int_t params = tau::get(tt(ref_head) | tau::ref_args | tt::ref).children_size();

	return std::make_tuple(sym, offset_args, params);
}

// True iff a bf under @p parent starts its own type context: the parent
// is null or is none of the bf/bv operators, bf quantifiers and atomic
// formulas, which type their operands through their own scope instead.
template<NodeType node>
bool is_top_level_bf(tref parent) {
	using tau = tree<node>;
	if (parent == nullptr) return true;
	auto nt = tau::get(parent).get_type();
	switch (nt) {
		// bf logical connectives
		case tau::bf_and: case tau::bf_or: case tau::bf_neg:
		case tau::bf_xor:
		// bv ops
		case tau::bf_nor: case tau::bf_nand: case tau::bf_xnor:
		case tau::bf_add: case tau::bf_sub: case tau::bf_mul:
		case tau::bf_div: case tau::bf_mod: case tau::bf_shr:
		case tau::bf_shl:
		// bf quantifiers
		case tau::bf_fall: case tau::bf_fex:
		// bf atomic formulas
		case tau::bf_eq: case tau::bf_neq: case tau::bf_lteq:
		case tau::bf_nlteq: case tau::bf_gt: case tau::bf_ngt:
		case tau::bf_gteq: case tau::bf_ngteq: case tau::bf_lt:
		case tau::bf_nlt: case tau::bf_interval:
			return false;
		default:
			return true;
	}
}

// Rebuilds @p n with its value retyped to @p new_type; the children are
// kept as they are.
template<NodeType node>
tref retype(tref n, const size_t new_type) {
	using tau = tree<node>;
	auto t = tau::get(n);
	return (tau::get(n).has_child())
		? tau::get(t.value.ba_retype(new_type), t.get_children())
		: tau::get(t.value.ba_retype(new_type));
};

// Collects the nodes under @p n matching @p query, without descending
// into nodes matching @p stop (offsets by default), and groups them by
// node kind into canonized-node -> effective-type maps. Repeated
// occurrences of one canonized element are unified; returns an
// inference_error on the first incompatible pair.
template <NodeType node>
std::variant<typeables_type_id_map<node>, inference_error> get_typeable_type_ids_by_type(
		tref n,	const std::function<bool(tref)>& query = is_typeable<node>,
		const std::function<bool(tref)>& stop = is<node, tree<node>::offset>) {
	using tau = tree<node>;

	auto typeables = tau::get(n).select_all_until(query, stop);
	typeables_type_id_map<node> typeable_type_ids_by_type;
	for (tref typeable : typeables) {
		auto canonized = canonize<node>(typeable);
		auto nt = tau::get(typeable).get_type();
		if (typeable_type_ids_by_type.find(nt) == typeable_type_ids_by_type.end())
			typeable_type_ids_by_type[nt] = subtree_map<node, size_t>();
		if (auto it = typeable_type_ids_by_type[nt].find(canonized); it !=
				typeable_type_ids_by_type[nt].end()) {
			if (auto type_id = unify<node>(it->second, get_effective_ba_type<node>(typeable)); type_id) {
				typeable_type_ids_by_type[nt][canonized] = type_id.value();
				continue;
			}
			return inference_error{typeable, it->second, get_effective_ba_type<node>(typeable)}; // incompatible types
		}
		typeable_type_ids_by_type[nt][canonized] = get_effective_ba_type<node>(typeable);
	}
#ifdef DEBUG
	LOG_TRACE << "get_typeable_type_ids_by_type/typeable_type_ids_by_type:\n";
	for (auto [type, typeables] : typeable_type_ids_by_type) {
		LOG_TRACE << "\ttype: " << LOG_NT(type) << "\n";
		for (auto [t, tid] : typeables) {
			LOG_TRACE << "\t\t" << LOG_FM(t) << " : "
				<< ba_types<node>::name(tid) << "\n";
		}
	}
#endif // DEBUG
	return typeable_type_ids_by_type;
}

// Convenience overload: collect exactly the node kinds listed in
// @p types.
template <NodeType node>
std::variant<typeables_type_id_map<node>, inference_error> get_typeable_type_ids_by_type(
		tref n, const std::initializer_list<size_t>& types) {
	return get_typeable_type_ids_by_type<node>(n, is<node>(types));
}

// A bf_cast is a type boundary: its operand is typed from its own
// annotations (type_annotated_operands), never from the enclosing atomic
// formula, whose scope merges every member into one type -- the whole point
// of `(bv[16]) x:bv[8] = c` is that x and c differ in width. A quantifier
// binder, however, must still learn the width of the variable it binds
// when that variable only occurs inside a cast: `ex x ((bv[16]) x:bv[8] =
// c)` otherwise binds an untyped x nobody uses, the binder is dropped, and
// the annotated x is left free. This lifts, for the variables @p bound
// already declares, the type each cast operand below @p body annotates
// them with.
template <NodeType node>
std::optional<inference_error> unify_bound_vars_with_cast_operands(tref body,
		typeables_type_id_map<node>& bound) {
	using tau = tree<node>;
	auto casts = tau::get(body).select_all_until(
		is<node, tau::bf_cast>, is<node, tau::offset>);
	if (casts.empty()) return std::nullopt;
	auto vars_it = bound.find(tau::variable);
	if (vars_it == bound.end()) return std::nullopt;
	auto& vars = vars_it->second;
	for (tref c : casts) {
		auto inner = get_typeable_type_ids_by_type<node>(c,
			is<node>({tau::variable}), is<node, tau::offset>);
		if (std::holds_alternative<inference_error>(inner))
			return std::get<inference_error>(inner);
		for (const auto& [canonized, type_id]
				: std::get<typeables_type_id_map<node>>(inner)[tau::variable]) {
			auto it = vars.find(canonized);
			if (it == vars.end()) continue; // free here: typed by its annotation
			if (auto unified = unify<node>(it->second, type_id); unified)
				it->second = unified.value();
			else return inference_error{canonized, it->second, type_id};
		}
	}
	return std::nullopt;
}

// Classifies a rec_relation as a function definition (true) or a
// predicate definition (false). Functional when either side carries a
// type or is a term, or when the body is a bare reference whose head
// symbol is recorded in @p function_symbols as called from a term
// position (see the note in the body). Non-rec_relation nodes return
// false.
template<NodeType node>
bool is_functional_relation(tref n, const auto& function_symbols) {
	using tau = tree<node>;

	auto t = tau::get(n);
	if (!is<node, tau::rec_relation>(n)) return false;
	// If the head is typed we have a functional relation.
	if (!is_untyped_tref<node>(t[0].get()) || t[0].is_term()) {
		return true;
	}
	if (!is_untyped_tref<node>(t[1].get()) || t[1].is_term()) {
		return true;
	}
	// A relation whose body is nothing but a reference says nothing about
	// being a formula; it was classified as a predicate only because
	// neither side carried a type. If its head symbol is also called from a
	// term position, it is a function. Without this,
	// `pred(int[t](1)) := int[t-1](1)` got a `wff_ref` head while every
	// call `add(pred(x), ...)` is a `bf_ref`, so the definition silently
	// never matched at the call site (issue 36). A body that is a genuine
	// formula (`p(x) := x' = 0`) stays a predicate: calling it from a term
	// position is a type error, not a reason to reinterpret it.
	if (unwrap_to_ref<node>(t[1].get()))
		if (tref head = unwrap_to_ref<node>(t[0].get()); head
			&& function_symbols.contains(
				get_function_signature<node>(head)))
		{
			return true;
		}
	// Otherwise, we have a predicate relation.
	return false;
}

// True iff the fixpoint reference @p n is to be typed as a function:
// the reference itself, or its fp_fallback expression, carries a type
// or is a term. Call only on refs that carry a fallback; non-ref nodes
// return false.
template<NodeType node>
bool is_functional_fallback(tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;

	if (!is<node, tau::ref>(n)) return false;
	// If the head is typed we have a functional relation.
	if (!is_untyped_tref<node>(n) || tau::get(n).is_term()) {
		return true;
	}
	auto fallback = tt(n) | tau::fp_fallback | tt::first | tt::ref;
	if (!is_untyped_tref<node>(fallback) || tau::get(fallback).is_term()) {
		return true;
	}
	// Otherwise, we have a predicate callback.
	return false;
}

// True iff the reference @p n stands in a term position: it carries a
// type itself, or its signature was previously recorded as a function
// in @p function_symbols. An offset-free reference also counts when an
// INDEXED function signature with the same name and argument arity is
// recorded: that is how a fixpoint of a function recurrence is called
// (`g(y)` for `g[n](x) := ...`, resolved the same way by find_fpcalls
// in nso_rr.tmpl.h), so the exact-signature lookup alone can never see
// it. Non-ref nodes return false.
template<NodeType node>
bool is_functional_ref(tref n, const auto& function_symbols) {
	using tau = tree<node>;

	if (!is<node, tau::ref>(n)) return false;
	// If the head is typed we have a functional relation.
	if (!is_untyped_tref<node>(n)) {
		return true;
	}
	// If the head was previously defined as a function we also have a functional ref
	// Otherwise, we have a predicate callback.
	auto sig = get_function_signature<node>(n);
	if (function_symbols.contains(sig)) return true;
	// Offset-free fixpoint call to an indexed function recurrence.
	if (std::get<1>(sig) == 0)
		for (const auto& [fsig, _] : function_symbols)
			if (std::get<0>(fsig) == std::get<0>(sig)
				&& std::get<2>(fsig) == std::get<2>(sig)
				&& std::get<1>(fsig) > 0) return true;
	return false;
}

// Retypes an interior term node from its children: rebuilds @p n typed
// with the first non-zero type found among them (0 when none carries
// one). The children themselves are kept as they are.
template<NodeType node>
tref update_ba_symbol(tref n) {
	using tau = tree<node>;

	// We have one child at least and we know that the types of the
	// children have already been updated and they are consistent.
	auto t = tau::get(n);
	auto chs = t.get_children();
	size_t n_type = 0;
	for (tref c : chs) {
		const size_t ct = tau::get(c).get_ba_type();
		if (ct != 0) {
			n_type = ct;
			break;
		}
	}
	auto new_n = tau::get_raw(t.value.ba_retype(n_type), chs.data(), chs.size());
	return new_n;
}

// Retypes a bv-only operator (add/sub/..., shifts, nand/nor/xnor) from
// its children like update_ba_symbol, but only when the first child's
// type is in the bv type family. Otherwise: returns @p n unchanged when
// defaults are off (it may still be typed later), an inference_error
// when they are on.
template<NodeType node>
std::variant<tref, inference_error, parse_error> update_bv_symbol(tref n,
		const type_inference_options& options) {
	using tau = tree<node>;

	// We have one child at least and we know that the types of the
	// children have already been updated and they are consistent.
	// We only need to check that the type is bv type family.
	auto t = tau::get(n)[0].get_ba_type();
	if (is_bv_type_family<node>(t))
		return update_ba_symbol<node>(n);
	else if (!options.use_defaults) return n;
	// BA2-16: expected = a bv-family type (report as untyped-expected slot
	// per the error's rendering order: found first), found = t.
	return inference_error{n, untyped_type_id<node>(), t};
}

// Hoists the reference's type onto its bf_ref wrapper: takes the first
// BA type found anywhere in the subtree (reference head or arguments),
// strips the type annotations from the inner ref, and rebuilds the
// bf_ref carrying that type.
template<NodeType node>
tref update_bf_ref(tref n) {
	using tau = tree<node>;

	auto type = find_ba_type<node>(n);
	auto new_n = untype<node>(tau::get(n).child(0)); //ref
	return tau::get_typed(tau::bf_ref, new_n, type);
}

// Returns the type to assign to @p n: if @p n is already typed, the
// unification of its effective type with the scope's entry (nullopt on
// conflict); otherwise the scope's entry itself, or the default tau
// type when that entry is still untyped and defaults are enabled.
// Precondition: @p types contains @p canonized.
template<NodeType node>
std::optional<size_t> get_inferred_type(tref n,	tref canonized,
		const subtree_map<node, size_t>& types,
		const type_inference_options& options) {
	// TODO (LOW) I think this could be simplified
	if (has_ba_type<node>(n)) {
		// We check that the type is compatible
		auto current_type = get_effective_ba_type<node>(n);
		auto inferred_type = types.at(canonized);
		return unify<node>(current_type, inferred_type);
	}
	return (types.at(canonized) == untyped_type_id<node>()) && options.use_defaults
		? tau_type_id<node>()
		: types.at(canonized);
}

// Rebuilds @p n retyped to @p type, dropping any `typed` annotation
// children.
template<NodeType node>
tref update_tref(tref n, size_t type) {
	using tau = tree<node>;

	auto t = tau::get(n);
	auto retyped_val = t.value.ba_retype(type);
	trefs ch;
	for (auto c : t.get_children())
		if (!tau::get(c).is(tau::typed)) ch.push_back(c);
	return ch.empty()
		? tau::get(retyped_val)
		: tau::get(retyped_val, ch);
}

// Retypes @p n from the scope snapshot @p types: returns @p n untouched
// when its canonized form has no entry; otherwise resolves the final
// type (get_inferred_type), records it in @p resolver and returns the
// retyped node. inference_error when the node's own type, or the
// resolver's, conflicts with the inferred one.
template<NodeType node>
std::variant<tref, inference_error, parse_error> update_tref(
		type_scoped_resolver<node>& resolver, tref n,
		const subtree_map<node, size_t>& types,
		const type_inference_options& options) {
	using tau = tree<node>;

	// If we have no type information for the element we do nothing
	tref canonized = canonize<node>(n);
	if (!types.contains(canonized)) return n;
	// If the tref is typed
	if (auto type = get_inferred_type<node>(n, canonized, types, options); type) {
		if (auto assigned = resolver.assign(canonized, type.value());
				std::holds_alternative<inference_error>(assigned))
			return std::get<inference_error>(assigned);
		return update_tref<node>(n, type.value());
	}
	return inference_error{n, types.at(canonized), tau::get(n).get_ba_type()};
}

// bf_t/bf_f constants: plain update_tref retyping.
template<NodeType node>
inline std::variant<tref, inference_error, parse_error> update_bf_constant(
		type_scoped_resolver<node>& resolver, tref n,
		const subtree_map<node, size_t>& types,
		const type_inference_options& options) {
	return update_tref<node>(resolver, n, types, options);
}

// Variables: plain update_tref retyping.
template<NodeType node>
inline std::variant<tref, inference_error, parse_error> update_variable(
		type_scoped_resolver<node>& resolver, tref n,
		const subtree_map<node, size_t>& types,
		const type_inference_options& options) {
	return update_tref<node>(resolver, n, types, options);
}

// Retypes a ba_constant from the scope snapshot and, if the constant is
// still unevaluated (data() == 0), parses its source text under the
// inferred type. Failure modes: nullptr when @p types has no entry for
// the canonized constant; parse_error when the source does not parse
// under that type; inference_error on a type or resolver conflict. An
// entry resolving to no concrete type leaves @p n unchanged.
template<NodeType node>
std::variant<tref, inference_error, parse_error> update_ba_constant(
		type_scoped_resolver<node>& resolver, tref n,
		const subtree_map<node, size_t>& types,
		const type_inference_options& options) {
	using tau = tree<node>;

	// If we have no type information for the element we do nothing.
	// BA2-1: return n, not nullptr -- the caller stores the returned tref
	// verbatim when it differs from n, and a nullptr child in the rebuilt
	// tree is dereferenced by any later traversal (update_tref returns n
	// for exactly this case).
	tref canonized = canonize<node>(n);
	if (!types.contains(canonized)) return n;
	// If the tref is typed
	if (auto type = get_inferred_type<node>(n, canonized, types, options); type && type.value()) {
		if (auto assigned = resolver.assign(canonized, type.value());
				std::holds_alternative<inference_error>(assigned))
			return std::get<inference_error>(assigned);
		// Check that the constant was not parsed yet
		DBG(LOG_TRACE << "inferred type of a constant: " << LOG_BA_TYPE(type.value());)
		if (tau::get(n).data() == 0) {
			auto saved_hooks = tau::use_hooks;
			tau::use_hooks = true;
			n = tau::get_ba_constant_from_source(tau::get(n).child_data(), type.value());
			tau::use_hooks = saved_hooks;
			if (n == nullptr) return parse_error{canonized, type.value()};
		}
		return update_tref<node>(n, type.value());
	}
	return n;
}

// True iff the scope snapshot knows @p n but its entry is still the
// untyped id -- i.e. the node is about to be typed by default.
template<NodeType node>
bool using_default_type(tref n, const subtree_map<node, size_t>& types) {
	tref canonized = canonize<node>(n);
	if (!types.contains(canonized)) return false;
	return types.at(canonized) == untyped_type_id<node>();
}

// (BA2-4: update_ref deleted -- zero callers, and it returned an
// inference_error from a tref function, a hard compile error on first
// instantiation. Recover from git and fix the return type if needed.)

// Rewraps a fixpoint reference whose fallback makes it a function:
// retypes its constants, bf_t/bf_f and variables via update(), checks a
// plain-term fallback's annotated type against the reference's own type
// (TI-2 note below), wraps a ref-shaped fallback in the reference's
// typed bf > bf_ref, and rebuilds the ref as {sym, ref_args, fallback}.
// Errors from update() and fallback/reference type conflicts propagate.
template<NodeType node>
std::variant<tref, inference_error, parse_error> update_functional_fallback(
		type_scoped_resolver<node>& resolver, tref n,
		const type_inference_options& options) {
	using tau = tree<node>;
	using tt = tau::traverser;

	// First we update the ba_constant, the variables and bf_t/bf_f in the rr and
	// close the body scope
	auto updated = update<node>(resolver, n, { tau::ba_constant, tau::bf_t, tau::bf_f, tau::variable }, options);
	if (std::holds_alternative<inference_error>(updated))
		return std::get<inference_error>(updated);
	if (std::holds_alternative<parse_error>(updated))
		return std::get<parse_error>(updated);
	auto sym = tt(std::get<tref>(updated)) | tau::sym | tt::ref;
	auto ref_args = tt(std::get<tref>(updated)) | tau::ref_args | tt::ref;
	auto fallback = tt(std::get<tref>(updated)) | tau::fp_fallback | tt::first | tt::ref;
	auto type = find_ba_type<node>(std::get<tref>(updated));
	// Untyped is allowed: an offset-free fixpoint call classified as
	// functional through the recorded function signatures carries no
	// annotation of its own (see is_functional_ref). (This replaces the
	// BA2-22 early return -- the checks below are untyped-guarded, and
	// the fixpoint-call fix needs untyped refs to flow through.)
	// TI-2: a ref-shaped fallback gets wrapped in the reference's type
	// below, but a plain term fallback was never checked against it, so
	// `g(x) fallback x:sbf` with a bv[8] `g` sailed through inference --
	// even though the same annotation one position to the left, in the
	// definition body, is rejected. Unify the two and report a conflict.
	if (!is<node, tau::ref>(fallback)) {
		size_t fallback_type = get_effective_ba_type<node>(fallback);
		if (fallback_type && !is_untyped<node>(fallback_type)
			&& !is_untyped<node>(type)
			&& !unify<node>(fallback_type, type))
				return inference_error{ fallback, type,
					fallback_type };
	}
	if (is<node, tau::ref>(fallback))
		fallback = tau::get_typed(tau::bf,
			tau::get_typed(tau::bf_ref, fallback, type), type);
	fallback = tau::get(tau::fp_fallback, fallback);
	// Wrap like update_functional_ref does: the call stands in a term
	// position, so it must come back as a bf > bf_ref chain for the
	// recurrence's bf rules (and the fixpoint calculation) to see it in
	// their own world -- a bare ref would keep its parser-given wff
	// wrapping and match nothing.
	return tau::get_typed(tau::bf, tau::get_typed(tau::bf_ref,
		tau::get(tau::ref, { sym, ref_args, fallback }), type), type);
}

// Predicate counterpart of update_functional_fallback: retypes the
// leaves via update(), wraps a ref-shaped fallback in an untyped
// wff > wff_ref, and rebuilds the ref as {sym, ref_args, fallback}.
template<NodeType node>
std::variant<tref, inference_error, parse_error> update_predicate_fallback(
		type_scoped_resolver<node>& resolver, tref n,
		const type_inference_options& options) {
	using tau = tree<node>;
	using tt = tau::traverser;

	// First we update the ba_constant, the variables and bf_t/bf_f in the rr and
	// close the body scope
	auto updated = update<node>(resolver, n, { tau::ba_constant, tau::bf_t, tau::bf_f, tau::variable }, options);
	if (std::holds_alternative<inference_error>(updated))
		return std::get<inference_error>(updated);
	if (std::holds_alternative<parse_error>(updated))
		return std::get<parse_error>(updated);
	auto sym = tt(std::get<tref>(updated)) | tau::sym | tt::ref;
	auto ref_args = tt(std::get<tref>(updated)) | tau::ref_args | tt::ref;
	auto fallback = tt(std::get<tref>(updated)) | tau::fp_fallback | tt::first | tt::ref;
	if (is<node, tau::ref>(fallback))
		fallback = tau::get(tau::wff, tau::get(tau::wff_ref, fallback));
	fallback = tau::get(tau::fp_fallback, fallback);
	return tau::get(tau::ref, { sym, ref_args, fallback });
}

// Rewraps a rec_relation classified as functional: retypes its
// constants, bf_t/bf_f and variables via update(), strips head/body
// annotations and any parser-given wff_ref wrapper (see below), wraps
// ref-shaped sides in the relation's typed bf > bf_ref, records the
// head signature with that type in @p function_symbols, and rebuilds
// the rec_relation. Returns nullptr when the body turns out to be a
// formula (a wff cannot define a function); when the relation's type is
// still untyped, returns the leaf-updated relation without rewrapping
// or recording it.
template<NodeType node>
std::variant<tref, inference_error, parse_error> update_functional_rr(
		type_scoped_resolver<node>& resolver, tref n, auto& function_symbols,
		const type_inference_options& options) {
	using tau = tree<node>;

	// First we update the ba_constant, the variables and bf_t/bf_f in the rr and
	// close the body scope
	auto updated = update<node>(resolver, n, { tau::ba_constant, tau::bf_t, tau::bf_f, tau::variable }, options);
	if (std::holds_alternative<inference_error>(updated))
		return std::get<inference_error>(updated);
	if (std::holds_alternative<parse_error>(updated))
		return std::get<parse_error>(updated);
	// Finally, we get the new body and reference and create a new rr
	// assuming the type of the head
	tref head = untype<node>(tau::get(std::get<tref>(updated)).child(0));
	tref body = untype<node>(tau::get(std::get<tref>(updated)).child(1));
	// A relation reclassified as functional because its symbol is called
	// from a term position still carries the wff_ref the parser gave it,
	// and rewrapping that as-is yields a wff_ref holding a bf, which
	// matches nothing. Strip that wrapper so the rewrap below sees the bare
	// reference. A bf wrapper is left untouched: it is already the right
	// shape and carries the types just established.
	auto unwrap_wff_ref = [](tref n) {
		if (!tau::get(n).is(tau::wff)) return n;
		tref ref = unwrap_to_ref<node>(n);
		return ref ? ref : n;
	};
	head = unwrap_wff_ref(head);
	body = unwrap_wff_ref(body);
	// If the body is a formula and not a term, reject
	if (tau::get(body).is(tau::wff)) return nullptr;
	size_t type = find_ba_type<node>(std::get<tref>(updated));
	// DBG(assert(!is_untyped<node>(type)));
	if (is_untyped<node>(type)) return updated;
	tref new_head = is<node, tau::ref>(head)
				? tau::get_typed(tau::bf,
					tau::get_typed(tau::bf_ref, head, type),
					type)
				: head;
	tref new_body = is<node, tau::ref>(body)
				? tau::get_typed( tau::bf,
					tau::get_typed(tau::bf_ref, body, type),
					type)
				: body;

	// Add new function definition to available definitions
	auto sig = get_function_signature<node>(tau::trim2(new_head));
	function_symbols.insert_or_assign(sig, type);
	return tau::get(tau::rec_relation, { new_head, new_body });
}

// Rewraps a rec_relation classified as a predicate: retypes its
// variables via update(), then wraps ref-shaped head and body in an
// untyped wff > wff_ref and rebuilds the rec_relation.
template<NodeType node>
std::variant<tref, inference_error, parse_error> update_predicate_rr(
		type_scoped_resolver<node>& resolver, tref n, const type_inference_options& options) {
	using tau = tree<node>;

	// First we update the variables in the rr head and body and close
	// the rr scope
	auto updated = update<node>(resolver, n, { tau::variable }, options);
	if (std::holds_alternative<inference_error>(updated))
		return std::get<inference_error>(updated);
	// Finally, we get the new body and reference and create a new rr
	// assuming boolean type
	auto head = tau::get(std::get<tref>(updated)).child(0);
	auto body = tau::get(std::get<tref>(updated)).child(1);
	auto new_head = is<node, tau::ref>(head)
				? tau::get(tau::wff, tau::get(tau::wff_ref, head))
				: head;
	auto new_body = is<node, tau::ref>(body)
				? tau::get(tau::wff, tau::get(tau::wff_ref, body))
				: body;
	return tau::get(tau::rec_relation, { new_head, new_body });
}

// Rewraps a standalone reference in term position: retypes its
// constants, bf_t/bf_f and variables via update(), then wraps the
// untyped ref in bf > bf_ref typed with whatever type the updated
// subtree carries (possibly none -- untyped is allowed here).
template<NodeType node>
std::variant<tref, inference_error, parse_error> update_functional_ref(
		type_scoped_resolver<node>& resolver, tref n,
		const type_inference_options& options) {
	using tau = tree<node>;

	// First we update the ba_constant, the variables and bf_t/bf_f in the ref
	auto updated = update<node>(resolver, n, { tau::ba_constant, tau::bf_t, tau::bf_f, tau::variable }, options);
	if (std::holds_alternative<inference_error>(updated))
		return std::get<inference_error>(updated);
	if (std::holds_alternative<parse_error>(updated))
		return std::get<parse_error>(updated);
	// Finally, we wrap the new ref accordingly
	auto type = find_ba_type<node>(std::get<tref>(updated));
	// Now we allow untyped
	//DBG(assert(!is_untyped<node>(type)));
	auto new_n = untype<node>(std::get<tref>(updated));
	return tau::get_typed(tau::bf, tau::get_typed(tau::bf_ref, new_n, type), type);
}

// Rewraps a standalone predicate reference: retypes its variables via
// update(), then wraps it in wff > wff_ref unless @p parent is already
// a wff_ref providing that wrapper.
template<NodeType node>
std::variant<tref, inference_error, parse_error> update_predicate_ref(
		type_scoped_resolver<node>& resolver, tref n, tref parent,
		const type_inference_options& options) {
	using tau = tree<node>;

	// First we update the variables in the ref
	auto updated = update<node>(resolver, n, { tau::variable }, options);
	if (std::holds_alternative<inference_error>(updated))
		return std::get<inference_error>(updated);
	// Finally, we wrap the new ref accordingly
	return is<node, tau::wff_ref>(parent)
		? std::get<tref>(updated)
		: tau::get(tau::wff, tau::get(tau::wff_ref, std::get<tref>(updated)));
}

// Generic bottom-up rebuild step: replaces each child of @p n with its
// recorded replacement from @p changes, records the rebuilt node under
// @p n when anything changed, and returns @p n's current replacement
// (or @p n itself).
template<NodeType node>
tref update_default(tref n, subtree_map<node, tref>& changes) {
	using tau = tree<node>;

	trefs ch;
	for (tref c : tau::get(n).children()) {
		if (changes.find(c) != changes.end())
			ch.push_back(changes[c]);
		else ch.push_back(c);
	}

	if (auto new_n = tau::get_raw(tau::get(n).value, ch.data(), ch.size()); new_n != n)
		changes.insert_or_assign(n, new_n);
	return (changes.find(n) != changes.end()) ? changes[n] : n;
};

// Type variables, constants, and bf nodes in a bf_cast operand subtree
// purely from their explicit type annotations, without consulting the resolver.
// This avoids conflicts when the same numeric constant appears with different
// types inside and outside a bf_cast boundary.
template<NodeType node>
tref type_annotated_operands(tref n) {
	using tau = tree<node>;
	subtree_map<node, tref> changes;

	auto f = [&](tref x) -> bool {
		size_t nt = tau::get(x).get_type();
		tref new_x = update_default<node>(x, changes);
		if (new_x != x) changes.insert_or_assign(x, new_x);
		x = new_x;

		if (is_processed<node>(x)) return true;
		switch (nt) {
			case tau::variable:
			case tau::bf_t: case tau::bf_f: {
				if (size_t type = get_effective_ba_type<node>(x); type) {
					if (auto typed = update_tref<node>(x, type); typed != x)
						changes.insert_or_assign(x, typed);
				}
				break;
			}
			case tau::ba_constant: {
				if (size_t type = get_effective_ba_type<node>(x); type) {
					tref typed = x;
					if (tau::get(typed).data() == 0)
						typed = tau::get_ba_constant_from_source(
							tau::get(typed).child_data(), type);
					if (typed)
						if (auto retyped = update_tref<node>(typed, type); retyped != x)
							changes.insert_or_assign(x, retyped);
				}
				break;
			}
			default:
				if (tau::is_term_nt(nt))
					if (auto upd = update_ba_symbol<node>(x); upd != x)
						changes.insert_or_assign(x, upd);
				break;
		}
		return true;
	};

	post_order<node>(n).search(f);
	return changes.contains(n) ? changes[n] : n;
}

// The retyping pass infer_ba_types runs when a scope's type information
// is complete (from on_leave, just before the scope closes). Post-order
// walk of @p r that rewrites the tree bottom up against a snapshot of
// resolver.current_types() taken once at entry:
// - leaves whose node kind is listed in @p types_to_update (variable,
//   ba_constant, bf_t/bf_f) are retyped from the snapshot, and every
//   assignment is echoed back into @p resolver so the still-open scopes
//   stay in sync with the rewritten tree;
// - bf operators always absorb their children's replacements and, when
//   selected (their own kind, or tau::typeable_symbol as a wildcard, is
//   listed), take their type from a child (update_ba_symbol; the
//   bv-only operators insist on a bv-family type via update_bv_symbol);
// - bf_ref wrappers, when listed, hoist the subtree type onto
//   themselves (update_bf_ref);
// - every other node just absorbs child replacements (update_default);
//   kinds without a case of their own are ignored even if listed.
// Leaves already typed by a previous infer_ba_types run are skipped.
// A separate type_environment stack tracks the nearest enclosing atomic
// formula / rec_relation / ref only to keep the "default typing" log
// message small. Returns the rewritten root (or @p r unchanged), or the
// first inference_error / parse_error met.
template<NodeType node>
std::variant<tref, inference_error, parse_error> update(
		type_scoped_resolver<node>& resolver, tref r,
		std::initializer_list<size_t> types_to_update,
		const type_inference_options& options) {
	using tau = tree<node>;

	subtree_map<node, tref> changes;

	std::optional<std::variant<inference_error, parse_error>> error = std::nullopt;
	auto types = resolver.current_types();
	auto to_be_updated = std::set<size_t>(types_to_update.begin(), types_to_update.end());
	std::vector type_environment = {r};

	auto f = [&](tref n) -> bool {
		auto t = tau::get(n);

		if (is_atomic_fm<node>(n)) type_environment.pop_back();
		else if (t.is(tau::rec_relation)) type_environment.pop_back();
		else if (t.is(tau::ref)) type_environment.pop_back();
		// Do not pop the bf environment

		size_t nt = t.get_type();

		// return error if present
		if (error.has_value()) return false;

		switch (nt) {
			case tau::variable: {
				// Skip variables already typed by a previous infer_ba_types call.
				if (is_processed<node>(n)) break;
				if (!to_be_updated.contains(nt)) break;
				auto updated = update_variable<node>(resolver, n, types, options);
				if (std::holds_alternative<inference_error>(updated)) {
					error = std::get<inference_error>(updated);
				} else {
					if (std::get<tref>(updated) != n)
						changes.insert_or_assign(n, std::get<tref>(updated));
					if (options.use_defaults && using_default_type<node>(n, types)) {
						default_typing_message<node>(std::get<tref>(updated), type_environment.back());
					}
				}
				break;
			}
			case tau::ba_constant: {
				if (is_processed<node>(n)) break;
				if (!to_be_updated.contains(nt)) break;
				auto updated = update_ba_constant<node>(resolver, n, types, options);
				if (std::holds_alternative<inference_error>(updated)) {
					error = std::get<inference_error>(updated);
				} else if (std::holds_alternative<parse_error>(updated)) {
					error = std::get<parse_error>(updated);
				} else {
					if (std::get<tref>(updated) != n)
						changes.insert_or_assign(n, std::get<tref>(updated));
					if (options.use_defaults && using_default_type<node>(n, types)) {
						default_typing_message<node>(std::get<tref>(updated), type_environment.back());
					}
				}
				break;
			}
			case tau::bf_t: case tau::bf_f: {
				if (is_processed<node>(n)) break;
				if (!to_be_updated.contains(nt)) break;
				auto updated = update_bf_constant<node>(resolver, n, types, options);
				if (std::holds_alternative<inference_error>(updated)) {
					error = std::get<inference_error>(updated);
				} else {
					if (std::get<tref>(updated) != n)
						changes.insert_or_assign(n, std::get<tref>(updated));
					if (options.use_defaults && using_default_type<node>(n, types)) {
						default_typing_message<node>(std::get<tref>(updated), type_environment.back());
					}
				}
				break;
			}
			case tau::bf:
			case tau::bf_interval: case tau::bf_eq: case tau::bf_neq:
			case tau::bf_lteq: case tau::bf_nlteq: case tau::bf_gt:
			case tau::bf_ngt: case tau::bf_gteq: case tau::bf_ngteq:
			case tau::bf_lt: case tau::bf_nlt: case tau::bf_or:
			case tau::bf_xor: case tau::bf_and: case tau::bf_neg: {
				// all types allowed; no is_processed check — always propagate child changes
				auto nn = update_default<node>(n, changes);
				if(!to_be_updated.contains(nt) && !to_be_updated.contains(tau::typeable_symbol)) {
					if (nn != n) changes.insert_or_assign(n, nn);
				} else if (auto updated = update_ba_symbol<node>(nn); updated != n)
					changes.insert_or_assign(n, updated);
				break;
			}
			case tau::bf_add: case tau::bf_sub: case tau::bf_mul:
			case tau::bf_div: case tau::bf_mod: case tau::bf_shr:
			case tau::bf_shl: case tau::bf_xnor: case tau::bf_nand:
			case tau::bf_nor: {
				// only bv types allowed
				auto nn = update_default<node>(n, changes);
				if(!to_be_updated.contains(nt) && !to_be_updated.contains(tau::typeable_symbol)) {
					if (nn != n) changes.insert_or_assign(n, nn);
				} else {
					auto updated = update_bv_symbol<node>(nn, options);
					if (std::holds_alternative<inference_error>(updated)) {
						error = std::get<inference_error>(updated);
						break;
					}
					if (n != std::get<tref>(updated))
						changes.insert_or_assign(n, std::get<tref>(updated));
				}

				break;
			}
			case tau::bf_ref: {
				auto nn = update_default<node>(n, changes);
				if(!to_be_updated.contains(nt)) break;
				// TODO (HIGH) check if we need to pass options
				if (auto updated = update_bf_ref<node>(nn); updated != n)
					changes.insert_or_assign(n, updated);
				break;
			}
			default: {
				if (auto updated = update_default<node>(n, changes); updated != n) {
					changes.insert_or_assign(n, updated);
				}
			}
		}

#ifdef DEBUG
		if (!error)
			LOG_TRACE << "infer_ba_types/update/" << LOG_NT(nt) << "/n -> new_n:\n"
				<< LOG_FM_TREE(n) << " -> "
				<< LOG_FM_TREE(changes.contains(n) ? changes[n] : n);
		LOG_TRACE << "infer_ba_types/update/" << LOG_NT(nt) << "/resolver:\n"
			<< resolver.dump_to_str();
#endif // DEBUG
		return !error;
	};

	// Solution to not printing the entire tau command tree for default typing
	auto update_type_env = [&type_environment](tref n) {
		const tau& n_t = tau::get(n);
		if (is_atomic_fm<node>(n)) type_environment.push_back(n);
		else if (n_t.is(tau::rec_relation)) type_environment.push_back(n);
		else if (n_t.is(tau::ref)) type_environment.push_back(n);
		// Case of single bf
		// (BA2-17: an `else if (type_environment.empty() && ...)` push
		// was deleted here -- the environment is seeded with {r} and
		// pushes/pops are balanced, so it can never be empty.)
		return true;
	};
	post_order<node>(r).search(f, update_type_env);
	if (error) {
		auto error_value = error.value();
		if (std::holds_alternative<inference_error>(error_value))
			return std::get<inference_error>(error_value);
		if (std::holds_alternative<parse_error>(error_value))
			return std::get<parse_error>(error_value);
	}
	bool root_changed = changes.find(r) != changes.end();
	return root_changed ? changes[r] : r;
}

// Reconciles a scope's merged type with previously recorded function
// definitions. @p type_map maps each canonized reference in the scope
// to its type id (the tau::ref slice of a typeables map). If @p type is
// still untyped, the first reference whose signature has a genuinely
// typed entry in @p available_function_symbols gets that type assigned
// through @p resolver, and the assignment's result is returned
// (signature-only entries recorded merely to classify a symbol as
// functional are skipped). If @p type is concrete, every referenced
// definition's recorded type must instead unify with it (TI-1 note
// below); on success the untyped id is returned -- callers only test
// for inference_error.
template <NodeType node>
std::variant<size_t, inference_error> type_by_function_symbol(
		type_scoped_resolver<node>& resolver,
		std::map<std::tuple<size_t, int_t, int_t>, size_t>& available_function_symbols,
		size_t type, const auto& type_map) {
	// If the merged type is untyped, we try to update the type
	// information by considering previously defined function definitions
	// contained in available_function_symbols
	if (type == untyped_type_id<node>()) {
		for (auto [func, _] : type_map) {
			if (auto it = available_function_symbols.find(get_function_signature<node>(func));
					it != available_function_symbols.end()) {
				// A signature recorded only to mark the symbol as a
				// function carries no type; assigning it would
				// pin the scope to untyped.
				if (is_untyped<node>(it->second)) continue;
				// Found previous function definition and return as soon as
				// possible
				return resolver.assign(func, it->second);
			}
		}
		return untyped_type_id<node>();
	}
	// TI-1: when the scope already has a type, this used to return
	// immediately and never look at what the callee was declared as, so
	// `f(x:bv[8]) := x. g(y:sbf) := f(y).` passed -- a reference handing a
	// bv[8] function an sbf. A definition carries ONE type across its head,
	// body and parameters (update_functional_rr unifies its arguments with
	// the header type), so a call site whose own type disagrees with the
	// callee's recorded type is inconsistent with that same model.
	for (auto [func, _] : type_map) {
		auto it = available_function_symbols.find(
			get_function_signature<node>(func));
		if (it == available_function_symbols.end()
			|| is_untyped<node>(it->second)
			|| unify<node>(type, it->second)) continue;
		return inference_error{ func, it->second, type };
	}
	return untyped_type_id<node>();
}

// Logs @p error at ERROR level: parse_error as an unparsable constant,
// scope_error as an improperly closed scope, inference_error as an
// expected/found type mismatch.
template <NodeType node>
void inference_error_message(
		const std::variant<inference_error, parse_error,
		typename type_scoped_resolver<node>::scope_error>& error) {
	using tau = tree<node>;
	using scope_error = typename type_scoped_resolver<node>::scope_error;

	if (std::holds_alternative<parse_error>(error)) {
		auto parse_err = std::get<parse_error>(error);
		LOG_ERROR << "Unable to parse  " << tau::get(parse_err.element) << " with type "
			<< ba_types<node>::name(parse_err.type_id) << "\n";
	} else if (std::holds_alternative<scope_error>(error)) {
		auto scope_err = std::get<scope_error>(error);
		LOG_ERROR << "Improper closed scope in "
			<< tau::get(scope_err.element) << ".\n";
	} else {
		DBG(assert(std::holds_alternative<inference_error>(error));)
		auto inference_err = std::get<inference_error>(error);
		LOG_ERROR << "Incompatible type information in "
			<< tau::get(inference_err.element)
			<< ", expected " << ba_types<node>::name(inference_err.expected)
			<< ", found " << ba_types<node>::name(inference_err.found) << "\n";
	}
}

// Infers the types of variables and constants in the tree n. It assumes that
// the types of the scoped variables are known when closing the scope.
// If a variable or constant remains unassigned, it is assigned to tau.
// We assume that the types of the constants could also be propagated across
// scopes (in the future we will restrict it to equations)
// If conflicting type information is found, the function returns nullptr.

// | Element  | Found in          | On enter     | On leave     | Case
// |----------|-------------------|--------------|--------------|-----------------------------------------------
// | bf       | nf_cmd_arg        | bf           | bf           | top level
// |----------|-------------------|--------------|--------------|-----------------------------------------------
// | bf       | bf_cmd_arg        | bf           | bf           | top level
// |----------|-------------------|--------------|--------------|-----------------------------------------------
// | bf       | rec_relation      | bf           | bf           | top level
// |----------|-------------------|--------------|--------------|-----------------------------------------------
// | bf       | ref_arg           | ref_arg      | ref_arg      | top level
// |----------|-------------------|--------------|--------------|-----------------------------------------------
// | bf       | bf_eq             | bf_eq        | bf_eq        | skip (treated at the bf_eq level)
// |----------|-------------------|--------------|--------------|-----------------------------------------------
// | wff      | nf_cmd_arg        | default      | default      |
// |----------|-------------------|--------------|--------------|-----------------------------------------------
// | wff      | wff_cmd_arg       | default      | default      |
// |----------|-------------------|--------------|--------------|-----------------------------------------------
// | wff      | rec_relation      | default      | default      |
// |----------|-------------------|--------------|--------------|-----------------------------------------------
// | ref	  | nf_cmd_arg        | ref	         | ref          | (fallback or continue)
// |----------|-------------------|--------------|--------------|-----------------------------------------------
// | ref      | rec_relation      | rec_relation | rec_relation |
// |----------|-------------------|--------------|--------------|-----------------------------------------------
// | ref      | bf_ref 			  | bf_eq        | bf_eq        | (treated at the bf_eq level))
// |----------|-------------------|--------------|--------------|-----------------------------------------------
// | ref      | wff_ref			  | ref	         | ref          | ref_args also deal with this case for args
// |----------|-------------------|--------------|--------------|-----------------------------------------------
// | wff      | wff_all           | wff_all      | wff_all      | resolve the quantified variables
// |----------|-------------------|--------------|--------------|-----------------------------------------------
// | wff      | wff_ex            | wff_ex       | wff_ex       | resolve the quantified variables
// |----------|-------------------|--------------|--------------|-----------------------------------------------

template <NodeType node>
std::pair<tref, subtree_map<node, size_t>> infer_ba_types(tref n,
 		const subtree_map<node, size_t>* global_scope,
		const std::vector<htref>* definition_heads,
		const type_inference_options& options) {
	using tau = tree<node>;

	type_scoped_resolver<node> resolver;

	// Adding global_scope info to resolver
	if (global_scope)
		for (auto [var, type] : *global_scope) {
			// We only insert io streams into the global scope
			if (!is_io_var<node>(var)) continue;
			auto untyped = canonize<node>(var);
			resolver.insert(untyped);
			resolver.assign(untyped, type);
		}

	// In order to infer types of function symbols depending on predefined
	// definitions, we keep a map of those present symbol definitions
	// The map sends <symbol_name, offset_arity, argument_arity> to a type
	std::map<std::tuple<size_t, int_t, int_t>, size_t>
	available_function_symbols;
	if (definition_heads) {
		for (htref func_def : *definition_heads) {
			const size_t type_id = tau::get(func_def).get_ba_type();
			// Exclude definitions that are not functions
			if (type_id == 0) continue;
			auto sig = get_function_signature<node>(func_def->get());
			available_function_symbols.insert_or_assign(sig, type_id);
		}
	}

	return infer_ba_types<node>(n, available_function_symbols, resolver, options);
}

// This function version is introduced for debugging purposes as it allows
// to inspect the resolver state after the type inference.
template <NodeType node>
std::pair<tref, subtree_map<node, size_t>> infer_ba_types(tref n,
	std::map<std::tuple<size_t, int_t, int_t>, size_t>& available_function_symbols,
	type_scoped_resolver<node>& resolver,
	const type_inference_options& user_options)
{
	using tau = tree<node>;
	using tt = tau::traverser;
	using scope_error = typename type_scoped_resolver<node>::scope_error;

#ifdef DEBUG
	LOG_TRACE << "================================================";
	LOG_TRACE << LOG_WARNING_COLOR << "infer_ba_types" << TC.CLEAR()
		<< " for: " << LOG_FM_DUMP(n);
#endif // DEBUG

	// We restore the original value of use_hooks at the end of the function
	auto using_hooks = tau::use_hooks;
	tau::use_hooks = false;

	type_inference_options options = user_options;

	// A reference the grammar put under a `bf_ref` sits in a term position,
	// so its definition is a function definition -- whatever that
	// definition's own head and body look like in isolation. Recording
	// those signatures before the traversal starts means the classification
	// does not depend on whether the definition or its first use is seen
	// first. The type is left untyped: this only settles predicate versus
	// function, the type comes from the definition itself.
	for (tref bf_ref : tau::get(n).select_all(is<node, tau::bf_ref>))
		if (tref ref = unwrap_to_ref<node>(bf_ref); ref)
			available_function_symbols.try_emplace(
				get_function_signature<node>(ref),
				untyped_type_id<node>());

	subtree_map<node, tref> transformed;
	std::optional<std::variant<inference_error, parse_error, scope_error>> error = std::nullopt;

	// We gather info about types and scopes while entering nodes
	auto on_enter = [&](tref n, tref parent) {
		DBG(assert(n != nullptr);)

		// Stop traversal on error
		if (error) return false;

		// if set to true, we skip traversing the children
		bool skip = false;

		// Get the node type
		auto t = tau::get(n);
		size_t nt = t.get_type();
		// Depoending on the node type...
		switch (nt) {
			case tau::input_def: case tau::output_def: {
				// Samples:
				// i1 : tau = in console
				// o1 : bv[8] = out console
				// ...
				// Extract typed annotation if present and seed resolver with io var type
				{
					auto& io_def = tau::get(n);
					tref io_var_name_child = io_def.first();
					auto io_var = tau::get(tau::variable,
						tau::get(tau::io_var, io_var_name_child));
					auto canonized_io_var = canonize<node>(io_var);
					for (auto c : io_def.get_children()) {
						if (tau::get(c).is(tau::typed)) {
							auto type_id = get_ba_type_id<node>(c);
							resolver.insert(canonized_io_var);
							if (auto assigned = resolver.assign(canonized_io_var, type_id);
									std::holds_alternative<inference_error>(assigned)) {
								error = std::get<inference_error>(assigned);
							}
							break;
						}
					}
				}
				skip = true; break;
			}
			case tau::rec_relation: {
				// Samples:
				// p(x):sbf := x'    (F) head is typed, body is bf
				// p(x):sbf := q(x)  (F) head is typed, body is ref
				// p(x) := x' ^1:sbf (E) head is untyped, body is bf
				// p(x:sbf) := q(x)  (P) head untyped, body is a ref
				// p(x:sbf) := x = 0 (P) head untyped, body is a wff
				// p(x) := x = 0     (E) x is untyped
				// p(x) := x = 1:sbf (P) x is sbf typed in body
				// ...
				// p(x) := q(x):sbf  (F) x is sbf typed in body
				// p(x, y) := x + y  (F) x and y are typed in body

				// We open a new scope with all the vars in the header.
				auto header_type = has_ba_type<node>(t[0].get())
					? get_effective_ba_type<node>(t[0].get())
					: get_effective_ba_type<node>(t[1].get());
				auto arguments = get_typeable_type_ids_by_type<node>(t[0].get(), { tau::variable });
				if (std::holds_alternative<inference_error>(arguments)) {
					error = std::get<inference_error>(arguments);
					break;
				} // Incompatible types
				auto arguments_map = get<typeables_type_id_map<node>>(arguments);
				if (is_functional_relation<node>(n, available_function_symbols)) {
					auto unified = unify<node>(arguments_map, header_type);
					if (std::holds_alternative<inference_error>(unified)) {
						error = std::get<inference_error>(unified);
						break;
					} // Incompatible types
					auto inferred_type = std::get<size_t>(unified);
					// We gather all the data about the body typeables
					auto rec_type_ids = get_typeable_type_ids_by_type<node>(n, {
						tau::ref, tau::variable, tau::ba_constant, tau::bf_t, tau::bf_f });
					if (std::holds_alternative<inference_error>(rec_type_ids)) {
						error = std::get<inference_error>(rec_type_ids);
						break;
					} // Incompatible types
					auto rec_type_ids_map = get<typeables_type_id_map<node>>(rec_type_ids);
					// We create a new scope with all the inferable typeables
					// taking into account that they should have the same type.
					if (auto opened = open_same_type<node>(resolver, rec_type_ids_map, inferred_type);
							std::holds_alternative<inference_error>(opened)) {
						error = std::get<inference_error>(opened);
						break;
					}
					// We merge all the header and the body typeables together.
					auto merged_type = merge<node>(resolver, rec_type_ids_map);
					if (std::holds_alternative<inference_error>(merged_type)) {
						error = std::get<inference_error>(merged_type);
						break;
					}
					// Take type definition due to function symbols into account
					// First remove current ref definition
					auto cur_sig = get_function_signature<node>(t.first());
					auto current_ref = [&](const auto& el) {
						return get_function_signature<node>(el.first) == cur_sig;
					};
					std::erase_if(rec_type_ids_map[tau::ref], current_ref);
					if (auto typed = type_by_function_symbol(resolver, available_function_symbols,
							std::get<size_t>(merged_type), rec_type_ids_map[tau::ref]);
							std::holds_alternative<inference_error>(typed)) {
						error = std::get<inference_error>(typed);
					}
					break;
				}
				// Otherwise, we have a predicate relation. We create a new scope
				// with all the variables in the header.
				open<node>(resolver, arguments_map);
				DBG(LOG_TRACE << "infer_ba_types/on_enter/" << LOG_NT(nt) <<": scope opened\n";)
				break;
			}
			case tau::wff_all: case tau::wff_ex: {
				// Samples:
				// all x:sbf x = 0
				// ex x x = 0:sbf
				//...

				// We open a new scope, we get all the quantified variables,
				// add them to the scope (with the given type if any).
				auto quantified_vars = get_typeable_type_ids_by_type<node>(t.child(0), { tau::variable });
				if (std::holds_alternative<inference_error>(quantified_vars)) {
					error = std::get<inference_error>(quantified_vars);
					break;
				} // Incompatible types
				auto quantified_vars_map = std::get<typeables_type_id_map<node>>(quantified_vars);
				if (auto lifted = unify_bound_vars_with_cast_operands<node>(
						t.child(1), quantified_vars_map); lifted) {
					error = lifted.value();
					break;
				}
				open<node>(resolver, quantified_vars_map);
				DBG(LOG_TRACE << "infer_ba_types/on_enter/" << LOG_NT(nt) <<": scope opened\n";)
				break;
			}
			case tau::bf: {
				// Be aware that if we have a command with unconnected bf's
				// this would fail.
				//
				// If bf is not a top level one, it must have been treated
				// somewhere else.  We still let traversal descend when the bf
				// contains a bf_cast anywhere below it, so the cast boundary
				// is processed: the cast may sit under an operator
				// (`((bv[16]) x:bv[8]) * y`), not only directly under this bf,
				// and skipping here would leave its operand untyped for the
				// enclosing equation's update to trip over.
				if (!is_top_level_bf<node>(parent)) {
					bool has_cast = tau::get(n).find_top(
						[](tref c){ return tau::get(c).is(tau::bf_cast); })
						!= nullptr;
					if (!has_cast) { skip = true; break; }
					break; // has cast below: descend but open no resolver scope
				}
				// bf_cast is a type boundary: its operand is typed directly
				// from annotations in on_leave, not via the resolver.
				if (parent && tau::get(parent).is(tau::bf_cast)) break;
				// Otherwise we have to treat it as a global scope
				auto typeables = get_typeable_type_ids_by_type<node>(n,
					is<node>({tau::ref, tau::variable, tau::ba_constant, tau::bf_t, tau::bf_f}),
					is<node>({tau::offset, tau::bf_cast}));
				if (std::holds_alternative<inference_error>(typeables)) {
					error = std::get<inference_error>(typeables);
					break;
				} // Incompatible types
				auto typeables_map = std::get<typeables_type_id_map<node>>(typeables);
				if (auto inserted = insert<node>(resolver, {
						typeables_map[tau::ref],
						typeables_map[tau::variable],
						typeables_map[tau::ba_constant],
						typeables_map[tau::bf_t],
						typeables_map[tau::bf_f] }); inserted) {
					error = inserted.value(); break;
				}
				auto merged_type = merge<node>(resolver, typeables_map);
				if(std::holds_alternative<inference_error>(merged_type)) {
					error = std::get<inference_error>(merged_type); break;
				}
				// Take type definition due to function symbols into account
				if (auto typed = type_by_function_symbol(resolver, available_function_symbols,
						std::get<size_t>(merged_type), typeables_map[tau::ref]);
						std::holds_alternative<inference_error>(typed)) {
					error = std::get<inference_error>(typed);
				}
				break;
			}
			case tau::ref: {
				// We skip the traversal if the parent is not a wff_ref or
				// is a functional ref as are treated elsewhere.
				if (parent && is_functional_relation<node>(parent, available_function_symbols)) {
					skip = true; break;
				}
				if (has_fallback<node>(n)) {
					// we must deal with it as a rec relation
					auto fallback = tt(n) | tau::fp_fallback | tt::first | tt::ref;
					auto header_type = has_ba_type<node>(n)
						? get_effective_ba_type<node>(n)
						: get_effective_ba_type<node>(fallback);
					auto arguments = get_typeable_type_ids_by_type<node>(n, { tau::variable });
					if (std::holds_alternative<inference_error>(arguments)) {
						error = std::get<inference_error>(arguments);
						break;
					} // Incompatible types
					auto arguments_map = std::get<typeables_type_id_map<node>>(arguments);
					// TI-2: a fallback is written at the call site, inside
					// whatever scope encloses the reference, so its
					// annotations have to agree with that scope. Both
					// branches below open a fresh scope -- right for the
					// reference's own arguments, which are treated as a rec
					// relation -- but that let a fallback's `x:sbf` shadow an
					// enclosing `x:bv[8]` instead of conflicting with it, so
					// `all x:bv[8] g(x) fallback x:sbf = 0.` was accepted
					// while the same annotation in a definition body is
					// rejected. Check the fallback against the enclosing
					// scope before the new one hides it.
					if (tref fb = tt(n) | tau::fp_fallback | tt::first | tt::ref; fb) {
						auto fb_types = get_typeable_type_ids_by_type<node>(
							fb, { tau::variable });
						if (std::holds_alternative<inference_error>(fb_types)) {
							error = std::get<inference_error>(fb_types);
							break;
						}
						bool conflict = false;
						for (const auto& [_, by_node] : std::get<
							typeables_type_id_map<node>>(fb_types))
						{
							for (const auto& [canonized, tid] : by_node) {
								size_t outer = resolver.type_id_of(canonized);
								if (!outer || !tid
									|| is_untyped<node>(outer)
									|| is_untyped<node>(tid)
									|| unify<node>(outer, tid))
										continue;
								error = inference_error{ canonized,
									outer, tid };
								conflict = true;
								break;
							}
							if (conflict) break;
						}
						if (conflict) break;
					}
					// A fallback call is also functional when it is an
					// offset-free call to an indexed function recurrence
					// (is_functional_ref's fixpoint-call matching); its own
					// annotations alone cannot tell (is_functional_fallback).
					if (is_functional_fallback<node>(n)
						|| is_functional_ref<node>(n,
							available_function_symbols)) {
						auto unified = unify<node>(arguments_map, header_type);
						if (std::holds_alternative<inference_error>(unified)) {
							error = std::get<inference_error>(unified);
							break;
						} // Incompatible types
						auto inferred_type = std::get<size_t>(unified);
						// We gather all the data about the body typeables
						auto rec_type_ids = get_typeable_type_ids_by_type<node>(n, {
							tau::ref, tau::variable, tau::ba_constant, tau::bf_t, tau::bf_f });
						if (std::holds_alternative<inference_error>(rec_type_ids)) {
							error = std::get<inference_error>(rec_type_ids);
							break;
						} // Incompatible types
						auto rec_type_ids_map = std::get<typeables_type_id_map<node>>(rec_type_ids);
						// We create a new scope with all the inferable typeables
						// taking into account that they should have the same type.
						if (auto opened = open_same_type<node>(resolver, rec_type_ids_map, inferred_type);
								std::holds_alternative<inference_error>(opened)) {
							error = std::get<inference_error>(opened);
							break;
						} // Incompatible types
						// We merge all the header and the body typeables together.
						const auto merged_type = merge<node>(resolver, rec_type_ids_map);
						if(std::holds_alternative<inference_error>(merged_type)) {
							error = std::get<inference_error>(merged_type);
							break;
						} // Incompatible types
						// Take type definition due to function symbols into account
						if (auto typed = type_by_function_symbol(resolver, available_function_symbols,
								std::get<size_t>(merged_type), rec_type_ids_map[tau::ref]);
								std::holds_alternative<inference_error>(typed)) {
							error = std::get<inference_error>(typed); // Incompatible types
						}
						break;
					}
					// Otherwise, we have a predicate relation. We create a new scope
					// with all the variables in the header.
					open<node>(resolver, arguments_map);
					DBG(LOG_TRACE << "infer_ba_types/on_enter/" << LOG_NT(nt) <<": scope opened\n";)
					break;
				}
				if (is_functional_ref<node>(n, available_function_symbols)) {
					auto arguments = get_typeable_type_ids_by_type<node>(n);
					if (std::holds_alternative<inference_error>(arguments)) {
						error = std::get<inference_error>(arguments);
						break;
					} // Incompatible types
					auto arguments_map = std::get<typeables_type_id_map<node>>(arguments);
					if (auto inserted = insert<node>(resolver, arguments_map); inserted) {
						error = inserted.value();
						break;
					}
					const auto merged_type = merge<node>(resolver, arguments_map);
					if(std::holds_alternative<inference_error>(merged_type)) {
						error = std::get<inference_error>(merged_type);
						break;
					}
					// Take type definition due to function symbols into account
					if (auto typed = type_by_function_symbol(resolver, available_function_symbols,
							std::get<size_t>(merged_type), arguments_map[tau::ref]);
							std::holds_alternative<inference_error>(typed)) {
						error = std::get<inference_error>(typed);
					}
					break;
				}
				// Anyway, we continue the traversal so that we can treat
				// the ref_args as above.
				break;
			}
			case tau::ref_arg:
				// This case only happens when we are processing a wff_ref in
				// a formula.
			case tau::bf_eq: case tau::bf_neq: case tau::bf_lteq: case tau::bf_nlteq:
			case tau::bf_gt: case tau::bf_ngt: case tau::bf_gteq: case tau::bf_ngteq:
			case tau::bf_lt: case tau::bf_nlt:
			case tau::bf_interval: {
				auto typeables = get_typeable_type_ids_by_type<node>(n,
					is<node>({tau::ref, tau::variable, tau::ba_constant, tau::bf_t, tau::bf_f}),
					is<node>({tau::offset, tau::bf_cast}));
				if (std::holds_alternative<inference_error>(typeables)) {
					error = std::get<inference_error>(typeables);
					break;
				} // Incompatible types
				auto typeables_map = std::get<typeables_type_id_map<node>>(typeables);
				open<node>(resolver, {
						typeables_map[tau::ref],
						typeables_map[tau::ba_constant],
						typeables_map[tau::bf_t],
						typeables_map[tau::bf_f] });
				DBG(LOG_TRACE << "infer_ba_types/on_enter/" << LOG_NT(nt) <<": scope opened\n";)
				if (auto inserted = insert<node>(resolver, {
						typeables_map[tau::variable]
						}); inserted) { error = inserted.value(); break; }
				auto merged_type = merge<node>(resolver, typeables_map);
				if(std::holds_alternative<inference_error>(merged_type)) {
					error = std::get<inference_error>(merged_type); break;
				}
				// Take type definition due to function symbols into account
				if (auto typed = type_by_function_symbol(resolver, available_function_symbols,
						std::get<size_t>(merged_type), typeables_map[tau::ref]);
						std::holds_alternative<inference_error>(typed)) {
					error = std::get<inference_error>(typed);
				}
				break;
			}
			default:
			// Otherwise, we continue the traversal of children
			// without doing anything special in this node.
			break;
		}

		if (error) inference_error_message<node>(error.value());

		DBG(LOG_TRACE << "infer_ba_types/on_enter/" << LOG_NT(nt) << "/resolver:\n"
			<< resolver.dump_to_str();)

		// Stop traversal on error
		return !error && !skip;
	};

	// Do nothing between nodes
	auto on_between = [&] (tref, tref) {
		// Stop traversal on error
		return !error;
	};

	// We update types (changing nodes while leaving them) while closing scopes
	auto on_leave = [&] (tref n, tref parent) {
		// We use transformed map to update children if they were any changes
		// and add the current node if resulted changed.
		DBG(assert(n != nullptr);)
		// Helper lambdas

		// Stop traversal on error
		if (error) return;
		// Get the node type
		const auto& t = tau::get(n);
		size_t nt = t.get_type();
		// Depoending on the node type...
		switch (nt) {
			case tau::input_def: case tau::output_def: {
				// Strip the typed structural child (type info already seeded in on_enter)
				auto t2 = tau::get(n);
				trefs filtered_ch;
				for (auto c : t2.get_children())
					if (!tau::get(c).is(tau::typed)) filtered_ch.push_back(c);
				if (filtered_ch.size() != t2.get_children().size()) {
					tref new_n = filtered_ch.empty()
						? tau::get(t2.value)
						: tau::get_raw(t2.value, filtered_ch.data(), filtered_ch.size());
					transformed.insert_or_assign(n, new_n);
				}
				break;
			}
			case tau::rec_relation: {
				// We need to adjust the wrapping around refs in the body and
				// the header accordingly.
				auto new_n = update_default<node>(n, transformed);
				auto updated = is_functional_relation<node>(new_n, available_function_symbols)
					? update_functional_rr<node>(resolver, new_n, available_function_symbols, options)
					: update_predicate_rr<node>(resolver, new_n, options);
				if (std::holds_alternative<parse_error>(updated)) {
					error = std::get<parse_error>(updated);
					break;
				}
				if (std::holds_alternative<inference_error>(updated)) {
					error = std::get<inference_error>(updated);
					break;
				}
				// update_functional_rr rejects a function
				// definition whose body is a formula (e.g.
				// `p(x):sbf := x = 0.`) by returning a null tref
				// rather than an inference_error. Storing that
				// null gives the parent rec_relations node a null
				// child, which the final update pass then
				// dereferences -- report the rejection as an
				// inference failure instead. The type ids are
				// left at 0: the rejection is about the body's
				// wff/bf shape, not about two conflicting types.
				if (std::get<tref>(updated) == nullptr) {
					error = inference_error{ new_n, 0, 0 };
					break;
				}
				if (std::get<tref>(updated) != new_n) transformed.insert_or_assign(n, std::get<tref>(updated));
				if (resolver.close()) {
					DBG(LOG_TRACE << "infer_ba_types/on_leave/" << LOG_NT(nt) <<": scope closed\n";)
					error = scope_error{n};
					break;
				}
				break;
			}
			case tau::wff_all: case tau::wff_ex: {
				tref new_n = update_default<node>(n, transformed);
				auto updated = update<node>(resolver, new_n, { tau::variable}, options);
				if (std::holds_alternative<parse_error>(updated)) {
					error = std::get<parse_error>(updated);
					break;
				}
				if (std::holds_alternative<inference_error>(updated)) {
					error = std::get<inference_error>(updated);
					break;
				}
				if (std::get<tref>(updated) != new_n) transformed.insert_or_assign(n, std::get<tref>(updated));
				if(resolver.close()) {
					DBG(LOG_TRACE << "infer_ba_types/on_leave/" << LOG_NT(nt) <<": scope closed\n";)
					error = scope_error{n};
					break;
				}
				break;
			}
			case tau::ref: {
				if (parent && (is_cli_cmd<node>(parent) || is<node, tau::wff_ref>(parent))) {
					if (has_fallback<node>(n)) {
						// we must deal with it as a rec relation
						// We need to adjust the wrapping around refs in the body and
						// the header accordingly.
						auto new_n = update_default<node>(n, transformed);
						// Same functional test as on_enter: an offset-free
						// fixpoint call to an indexed function recurrence
						// carries no annotation of its own, so it is
						// recognized through the recorded function
						// signatures (is_functional_ref).
						auto updated = is_functional_fallback<node>(new_n)
							|| is_functional_ref<node>(new_n,
								available_function_symbols)
							? update_functional_fallback<node>(resolver, new_n, options)
							: update_predicate_fallback<node>(resolver, new_n, options);
						if (std::holds_alternative<parse_error>(updated)) {
							error = std::get<parse_error>(updated);
							break;
						}
						if (std::holds_alternative<inference_error>(updated)) {
							error = std::get<inference_error>(updated);
							break;
						}
						if (std::get<tref>(updated) != new_n) transformed.insert_or_assign(n, std::get<tref>(updated));
						if (resolver.close()) {
							DBG(LOG_TRACE << "infer_ba_types/on_leave/" << LOG_NT(nt) <<": scope closed\n";)
							error = scope_error{n};
							break;
						}
						break;
					}
					auto new_n = update_default<node>(n, transformed);
					auto updated = is_functional_ref<node>(new_n, available_function_symbols)
						? update_functional_ref<node>(resolver, new_n, options)
						: update_predicate_ref<node>(resolver, new_n, parent, options);
					if (std::holds_alternative<parse_error>(updated)) {
						error = std::get<parse_error>(updated);
						break;
					}
					if (std::holds_alternative<inference_error>(updated)) {
						error = std::get<inference_error>(updated);
						break;
					}
					if (std::get<tref>(updated) != new_n) transformed.insert_or_assign(n, std::get<tref>(updated));
					break;
				}
				tref new_n = update_default<node>(n, transformed);
				auto updated = update<node>(resolver, new_n, { tau::variable }, options);
				if (std::holds_alternative<parse_error>(updated)) {
					error = std::get<parse_error>(updated);
					break;
				}
				if (std::holds_alternative<inference_error>(updated)) {
					error = std::get<inference_error>(updated);
					break;
				}
				if (std::get<tref>(updated) != new_n) transformed.insert_or_assign(n, std::get<tref>(updated));
				break;
			}
			case tau::ref_arg:
				// This case only happens when we are processing a wff_ref in
				// a formula.
			case tau::bf_eq: case tau::bf_neq: case tau::bf_lteq: case tau::bf_nlteq:
			case tau::bf_gt: case tau::bf_ngt: case tau::bf_gteq: case tau::bf_ngteq:
			case tau::bf_lt: case tau::bf_nlt:
			case tau::bf_interval: {
				// For children whose subtree contains a bf_cast, pick up
				// the pre-typed version from transformed (produced by
				// type_annotated_operands in bf on_leave and propagated up
				// through the intermediate operators).  Leave all other
				// children untouched so update<node> types them from the
				// resolver, avoiding stale entries from earlier scopes.
				//
				// The cast may sit anywhere below the child, not only as
				// its direct wrapper: `((bv[16]) x:bv[8]) * y = z` used to
				// re-walk the untyped operand from this equation's scope,
				// record a null replacement for it (update_ba_constant on a
				// constant the scope never saw) and abort in update_default.
				tref new_n = n;
				{
					const auto& t = tau::get(n);
					auto ch = t.get_children();
					bool changed = false;
					for (auto& c : ch) {
						if (auto it = transformed.find(c); it != transformed.end()) {
							bool child_has_cast = tau::get(c).find_top(
								[](tref x) { return tau::get(x).is(tau::bf_cast); })
								!= nullptr;
							if (child_has_cast) {
								c = it->second;
								changed = true;
							}
						}
					}
					if (changed)
						new_n = tau::get_raw(t.value, ch.data(), ch.size());
				}
				auto updated = update<node>(resolver, new_n, { tau::ref, tau::ba_constant, tau::bf_t, tau::bf_f }, options);
				if (std::holds_alternative<parse_error>(updated)) {
					error = std::get<parse_error>(updated);
					 break;
				} else if (std::holds_alternative<inference_error>(updated)) {
					error = std::get<inference_error>(updated);
					break;
				}
				if (std::get<tref>(updated) != n)
					transformed.insert_or_assign(n, std::get<tref>(updated));
				if (resolver.close()) {
					DBG(LOG_TRACE << "infer_ba_types/on_leave/" << LOG_NT(nt) <<": scope closed\n";)
					error = scope_error{n};
				}
				break;
			}
			case tau::bf: {
				tref new_n = update_default<node>(n, transformed);
				if (is_top_level_bf<node>(parent)) {
					if (parent && tau::get(parent).is(tau::bf_cast)) {
						// bf_cast operand: type directly from annotations,
						// not via the resolver, to avoid conflicts with the
						// surrounding context's type scope.
						tref updated = type_annotated_operands<node>(new_n);
						// A variable the annotations left untyped may still
						// be declared by an enclosing binder
						// (`ex x:bv[8] ((bv[16]) x = c)`): take its type from
						// the open scopes. Whatever leaf is still untyped
						// after that is an error here, not an untyped node
						// for the solver to abort on (`(bv[8]) y = c`).
						// Annotations elsewhere in the same operand count
						// too (`fall x:bv[4] x`), then the enclosing scopes.
						subtree_map<node, size_t> known;
						for (tref v : tau::get(updated).select_all_until(
								is<node, tau::variable>, is<node, tau::offset>))
							if (size_t t = tau::get(v).get_ba_type(); t)
								known.insert_or_assign(canonize<node>(v), t);
						for (const auto& [v, t] : resolver.all_types())
							if (t && t != untyped_type_id<node>())
								known.try_emplace(v, t);
						subtree_map<node, tref> retyped;
						tref untyped_leaf = nullptr;
						// offsets (`i1[t]`) hold untyped index variables
						// that are not terms: never descend into them
						auto untyped_leaves = tau::get(updated).select_all_until(
							[](tref x) {
								const auto& xt = tau::get(x);
								return xt.get_ba_type() == 0
									&& (xt.is(tau::variable) || xt.is(tau::ba_constant)
										|| xt.is(tau::bf_t) || xt.is(tau::bf_f));
							}, is<node, tau::offset>);
						for (tref x : untyped_leaves) {
							if (tau::get(x).is(tau::variable)) {
								if (auto it = known.find(canonize<node>(x));
									it != known.end())
								{
									retyped.insert_or_assign(x,
										update_tref<node>(x, it->second));
									continue;
								}
							}
							untyped_leaf = x; break;
						}
						if (untyped_leaf) {
							error = inference_error{untyped_leaf,
								tau::get(parent).get_ba_type(),
								untyped_type_id<node>()};
							break;
						}
						if (!retyped.empty())
							updated = type_annotated_operands<node>(
								rewriter::replace<node>(updated, retyped));
						if (updated != new_n) transformed.insert_or_assign(n, updated);
					} else {
						auto updated = update<node>(resolver, new_n,
							{ tau::variable, tau::ba_constant, tau::bf_t, tau::bf_f }, options);
						if (std::holds_alternative<parse_error>(updated)) {
							error = std::get<parse_error>(updated);
							break;
						} else if (std::holds_alternative<inference_error>(updated)) {
							error = std::get<inference_error>(updated);
							break;
						}
						if (auto updated_ref = std::get<tref>(updated); updated_ref != new_n)
							transformed.insert_or_assign(n, updated_ref);
					}
				} else {
					if (new_n != n) transformed.insert_or_assign(n, new_n);
				}
				break;
			}
			default: {
				tref new_n = update_default<node>(n, transformed);
				// For the root node, we type untyped variables with tau.
				if (!parent) {
					auto updated = update<node>(resolver, new_n, { tau::variable }, options);
					if (std::holds_alternative<parse_error>(updated)) {
						error = std::get<parse_error>(updated);
							break;
					} else if (std::holds_alternative<inference_error>(updated)) {
						error = std::get<inference_error>(updated);
						break;
					}
					new_n = std::get<tref>(updated);
				}
				if (new_n != n) transformed.insert_or_assign(n, new_n);
				break;
			}
		}

		if (error) inference_error_message<node>(error.value());

#ifdef DEBUG
		if (!error)
			LOG_TRACE << "infer_ba_types/on_leave/" << LOG_NT(nt) << "/n -> new_n:\n"
				<< LOG_FM_TREE(n) << " -> "
				<< LOG_FM_TREE(transformed.contains(n) ? transformed[n] : n);
		LOG_TRACE << "infer_ba_types/on_leave/" << LOG_NT(nt) << "/resolver:\n"
			<< resolver.dump_to_str();
#endif // DEBUG

		return;
	};

	// We visit the tree and return the transformed root if no error happened.
	// If an error happened we return nullptr.
	pre_order<node>(n).visit(on_enter, idni::all, on_leave, on_between);
	if (error) return tau::use_hooks = using_hooks,
		std::pair<tref, subtree_map<node, size_t>>{ nullptr, subtree_map<node, size_t>{} };
	// We add to the transformed map the untyping of the bf_t's and the bf_f's.
	// ...some code here...
	tref new_n = transformed.contains(n) ? transformed[n] : n;

	DBG(LOG_TRACE << LOG_WARNING_COLOR << "infer_ba_types (after inference): " << TC.CLEAR()
		<< LOG_FM_DUMP(new_n);)

	if (new_n == nullptr) return tau::use_hooks = using_hooks,
		std::pair<tref, subtree_map<node, size_t>>{ nullptr, subtree_map<node, size_t>{} };
	auto updated = update<node>(resolver, new_n, { tau::typeable_symbol, tau::bf_ref }, options);
	if (std::holds_alternative<inference_error>(updated)) {
		// TODO (HIGH) print error info
		return tau::use_hooks = using_hooks,
			std::pair<tref, subtree_map<node, size_t>>{ nullptr, subtree_map<node, size_t>{} };
	}

	DBG(LOG_TRACE << LOG_WARNING_COLOR << "infer_ba_types (after update): " << TC.CLEAR()
		<< LOG_FM_DUMP(new_n);)

	new_n = std::get<tref>(updated);

	auto n_global_scope = resolver.current_types();

#ifdef DEBUG
	LOG_TRACE << LOG_WARNING_COLOR << "infer_ba_types" << TC.CLEAR()
		<< " of: " << LOG_FM(n) << " resulted into: " << LOG_FM_DUMP(new_n);
	LOG_TRACE << "================================================";
#endif // DEBUG

	tau::use_hooks = using_hooks;
	return std::pair<tref, subtree_map<node, size_t>>{ new_n, std::move(n_global_scope) };
}


} // namespace idni::tau_lang
