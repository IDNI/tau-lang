// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/bv/bv_ba.h" // Only for IDE resolution, not really needed.
#include "bv_widening.h"               // Only for IDE resolution, not really needed.

#include <algorithm>
#include <stdexcept>
#include <string>

namespace idni::tau_lang {

/**
 * @internal
 * @brief Read the literal shift amount of a `bf_shl` operator node, or `0`
 * when it is not a usable literal.
 *
 * A variable shift amount executes at the left operand's width and may
 * wrap (see the `bf_shl` rule in bv_widening.h), so it reads as `0`
 * growth. A shift amount too large to fit `unsigned long
 * long` (an exceedingly wide bitvector literal) is treated the same way:
 * `std::stoull` throwing is caught and folded into the same "no known
 * growth" fallback as a non-constant amount, rather than propagating an
 * uncaught exception out of needed_width.
 *
 * A constant amount does not always survive as a `ba_constant`: the
 * all-ones value of a bv type is canonicalized to the *top element*
 * `bf_t` at construction time (test_bv_ba_hooks.cpp, "an all-ones literal
 * is the top element": `{255}:bv[8]` and `1:bv[8]` are the same node), and
 * the zero to `bf_f`. A `bf_t` amount is just as literal as a
 * `ba_constant` -- `2^w - 1` for its own declared width `w` -- and is read
 * as such here, otherwise `x << { 255 }:bv[8]` would silently fall into
 * the "no known growth" branch and be widened to 8 instead of the exact
 * `l + 255` the design's `bf_shl` rule requires. An *untyped* `bf_t` (the
 * plain Boolean top, `ba_type == 0`) carries no width, so its value is
 * unknown here and it is treated as non-constant. A `bf_f` amount needs no
 * case of its own: it is a literal zero, whose growth is the `0` the
 * fallthrough already returns (and `term_shl` folds `X << 0` to `X` at
 * construction anyway, bv_ba_hooks.tmpl.h:661-663).
 *
 * `op` is the operator node directly under a `bf` wrapper (i.e. `n[0]`
 * for the `bf` node whose operator is `bf_shl`). Its second child (`op[1]`)
 * is itself a `bf` wrapper around the shift-amount operand, and `op[1][0]`
 * drills one level further to the actual leaf/operator -- the same
 * two-level access idiom `term_shl` uses to read its operands
 * (bv_ba_hooks.tmpl.h:596-597: `tau::get(symbol)[0][1][0]`).
 *
 * @tparam node Tree node type.
 * @param op The `bf_shl` operator node (the child of its `bf` wrapper).
 * @return The literal amount, or `0` for a variable amount, an untyped
 * constant, or an amount too large to be represented (`w >= 64` top
 * element, or a literal `std::stoull` cannot parse).
 * @endinternal
 */
template <NodeType node>
size_t bf_shl_shift_amount(const tree<node>& op) {
	using tau = tree<node>;

	const tau& amount = op[1][0];
	const size_t amount_type = amount.get_ba_type();
	const bool typed_bv = amount_type > 0
		&& is_bv_type_family<node>(amount_type);

	// The top element is the literal 2^w - 1 for its own declared width w.
	if (amount.is(tau::bf_t)) {
		if (!typed_bv) return 0; // untyped: no width, not a known constant
		const size_t w = get_bv_width<node>(amount_type);
		// 2^w - 1 is not representable in size_t for w >= 64 (and the
		// resulting width would be astronomically past any usable
		// bv_max_width anyway): fall into the same "no known growth"
		// fallback the unparseably-large `std::stoull` case uses, rather
		// than wrapping around to a small -- silently under-widening --
		// amount.
		if (w >= sizeof(size_t) * 8) return 0;
		return (static_cast<size_t>(1) << w) - 1;
	}

	if (amount.is_ba_constant() && typed_bv) {
		const auto c = amount.get_ba_constant();
		try {
			return static_cast<size_t>(
				std::stoull(std::get<bv>(c).getBitVectorValue(10)));
		} catch (const std::exception&) {
			return 0; // too large (or malformed) to parse: no known growth
		}
	}
	return 0;
}

template <NodeType node>
size_t needed_width(tref bf_node, size_t base_w, size_t& maxW) {
	using tau = tree<node>;

	const tau& n = tau::get(bf_node);
	DBG(assert(n.is(tau::bf));)
	// bf -> op (bf_add/bf_mul/.../variable/ba_constant/...); each operand of
	// a binary/unary op is itself a `bf` wrapper, so recursion is over
	// `op.child(i)` (see term_add's `tau::get(symbol)[0][0]`,
	// bv_ba_hooks.tmpl.h:29-30, for the same one-level-under-`bf` access).
	const tau& op = n[0];

	auto rec = [&](size_t i) -> size_t {
		return needed_width<node>(op.child(i), base_w, maxW);
	};

	size_t w = 0;
	switch (op.value.nt) {
	// Leaves: contribute the caller-supplied base width. Deliberately does
	// not consult get_ba_type() here -- callers of needed_width may parse
	// without type inference and pass base_w explicitly (see bv_widening.h).
	case tau::variable:
	case tau::ba_constant:
	case tau::bf_t:
	case tau::bf_f:
		w = base_w;
		break;
	// Transparent wrapper: "(" bf ")" contributes its inner width unchanged.
	case tau::bf_parenthesis: {
		size_t inner = rec(0);
		if (inner == 0) return 0;
		w = inner;
		break;
	}
	// Boundary: the cast's own target width, regardless of its operand.
	// The target type id is attached directly to the bf_cast node at
	// parse-to-tree time (tau_tree_from_parser.tmpl.h, case bf_cast), so no
	// type inference is required to read it here. A cast to a non-bv-family
	// (or untyped) target has no width to contribute and would DBG-assert
	// inside get_bv_width, so it is treated as opaque -- the same explicit
	// guard is_side_saturated_at's operator branch applies before its own
	// get_bv_width call.
	case tau::bf_cast: {
		const size_t cast_type = op.get_ba_type();
		if (cast_type == 0 || !is_bv_type_family<node>(cast_type)) return 0;
		w = get_bv_width<node>(cast_type);
		break;
	}
	case tau::bf_add: {
		size_t l = rec(0), r = rec(1);
		if (l == 0 || r == 0) return 0;
		w = std::max(l, r) + 1;
		break;
	}
	case tau::bf_sub: {
		size_t l = rec(0), r = rec(1);
		if (l == 0 || r == 0) return 0;
		w = std::max(l, r);
		break;
	}
	case tau::bf_mul: {
		size_t l = rec(0), r = rec(1);
		if (l == 0 || r == 0) return 0;
		w = l + r;
		break;
	}
	case tau::bf_div:
	case tau::bf_mod: {
		size_t l = rec(0), r = rec(1);
		if (l == 0 || r == 0) return 0;
		w = l; // keep the dividend's width; result magnitude is bounded by it
		break;
	}
	case tau::bf_min:
	case tau::bf_max:
	case tau::bf_and:
	case tau::bf_or:
	case tau::bf_xor:
	case tau::bf_nand:
	case tau::bf_nor:
	case tau::bf_xnor: {
		size_t l = rec(0), r = rec(1);
		if (l == 0 || r == 0) return 0;
		w = std::max(l, r);
		break;
	}
	case tau::bf_neg: {
		size_t l = rec(0);
		if (l == 0) return 0;
		w = l; // complement runs at the operand's own width
		break;
	}
	case tau::bf_shr: {
		size_t l = rec(0), r = rec(1);
		if (l == 0 || r == 0) return 0;
		w = l; // a right shift never grows the needed width
		break;
	}
	case tau::bf_shl: {
		size_t l = rec(0), r = rec(1);
		if (l == 0 || r == 0) return 0;
		w = l + bf_shl_shift_amount<node>(op); // 0 when the amount is not a literal
		break;
	}
	default:
		return 0; // opaque subterm (bf_ref, capture, ...): caller skips the atom
	}
	maxW = std::max(maxW, w);
	return w;
}

/**
 * @internal
 * @brief Rebuild one operator node with (already widened) children, typed
 * at the wide width, under a fresh `bf` wrapper.
 *
 * Handles a `bf_parenthesis`/`bf_neg`-style single-child wrapper when
 * `r == nullptr`. The result has exactly the shape `build_bf_min` (etc.)
 * produce, except typed at `wide_tid` via `tree<node>::get_typed`. Wrapping in `bf`
 * triggers the ordinary construction-time hooks (constant folding etc.,
 * see the module doc comment's Trap 1) exactly as any other `bf` node
 * construction would.
 *
 * @tparam node Tree node type.
 * @param nt The operator's nonterminal (`bf_add`, `bf_neg`, ...).
 * @param l The widened first operand (a `bf` node).
 * @param r The widened second operand, or `nullptr` for a single-child operator.
 * @param wide_tid The `bv[W]` type id to attach to the rebuilt operator.
 * @return The new `bf` node.
 * @endinternal
 */
template <NodeType node>
tref rebuild_bf_op(typename node::type nt, tref l, tref r, size_t wide_tid) {
	using tau = tree<node>;

	if (r == nullptr)
		return tau::get(tau::bf, tau::get_typed(nt, l, wide_tid));
	return tau::get(tau::bf, tau::get_typed(nt, l, r, wide_tid));
}

template <NodeType node>
tref widen_term(tref bf_node, size_t base_w, size_t W) {
	using tau = tree<node>;

	if (W == base_w) return bf_node;
	const tau& op = tau::get(bf_node)[0];
	const size_t wide_tid = bv_type_id<node>(W);
	switch (op.value.nt) {
	// Leaves and user-cast boundaries: wrap the *whole* incoming bf_node
	// (cast included, for bf_cast -- its own operand is left untouched and
	// keeps computing exactly as in the default mode, at its own widths)
	// in one more, outer cast.
	case tau::variable:
	case tau::ba_constant:
	case tau::bf_t:
	case tau::bf_f:
	case tau::bf_cast:
		return build_bf_cast<node>(bf_node, wide_tid);
	// Every other operator (bf_add/sub/mul/div/mod/min/max/and/or/xor/
	// nand/nor/xnor/neg/shl/shr, and the purely transparent
	// bf_parenthesis wrapper, which needs no special case since it has
	// one child just like bf_neg) is rebuilt with widened children and
	// retyped bv[W]. No interior truncations anywhere (amended D2/D3):
	// bitwise ops and both shift operands run at W like everything else.
	default: {
		tref l = widen_term<node>(op.child(0), base_w, W);
		tref r = (op.children_size() > 1)
			? widen_term<node>(op.child(1), base_w, W) : nullptr;
		// op.value.nt is the raw size_t bitfield (tau_tree.h: `const T nt :
		// nt_bits`); get_type() is the proper accessor that static_casts it
		// to node::type (tau_tree.tmpl.h) -- required here since
		// rebuild_bf_op's first parameter is strongly typed, and an
		// unscoped enum only converts implicitly *to* an integral type,
		// never the other way around.
		return rebuild_bf_op<node>(op.get_type(), l, r, wide_tid);
	}
	}
}

/**
 * @internal
 * @brief Tell whether an atom side is bare storage (a `variable`, possibly
 * parenthesised).
 *
 * A `variable` covers io_vars and uninterpreted constants alike (see
 * parser/tau.tgf:149, `variable => (uconst | io_var | var_name) [
 * member_path ] [ typed ]` -- so no separate uconst check is needed),
 * possibly under one or more transparent `bf_parenthesis` wrappers. A
 * user-cast side (`bf_cast`) is deliberately NOT bare storage: per the
 * design, a cast is an independent sub-computation boundary, not the
 * declared storage target itself.
 *
 * @tparam node Tree node type.
 * @param side A `bf` side of a `bf_eq`/`bf_neq` atom.
 * @return `true` iff the side is a `variable` under zero or more `bf_parenthesis` wrappers.
 * @endinternal
 */
template <NodeType node>
bool is_bare_storage_side(tref side) {
	using tau = tree<node>;

	tref cur = side;
	for (;;) {
		const tau& op = tau::get(cur)[0];
		if (op.value.nt != tau::bf_parenthesis)
			return op.value.nt == tau::variable;
		cur = op.child(0);
	}
}

/**
 * @internal
 * @brief Width of a `bf_cast` node's operand, or `0` when it cannot be
 * determined.
 *
 * The width is unknown for an untyped operand (e.g. a tree parsed with
 * inference off) or a non-bv-family one. Reads the operand's own `bf` wrapper first
 * and falls back to the node underneath it, the same source-width lookup
 * bv_term_cast itself performs (bv_ba_hooks.tmpl.h:872-873).
 *
 * @tparam node Tree node type.
 * @param cast_op The `bf_cast` operator node.
 * @return The operand's bv width, or `0` for an untyped or non-bv operand.
 * @endinternal
 */
template <NodeType node>
size_t bf_cast_operand_width(const tree<node>& cast_op) {
	using tau = tree<node>;

	tref operand = cast_op.child(0);
	size_t t = tau::get(operand).get_ba_type();
	if (t == 0) t = tau::get(operand)[0].get_ba_type();
	if (t == 0 || !is_bv_type_family<node>(t)) return 0;
	return get_bv_width<node>(t);
}

/**
 * @internal
 * @brief Tell whether an atom side is already uniformly expressed at width
 * `W` (the idempotency guard's per-side check).
 *
 * "Uniformly expressed at `W`" means: every
 * operator node on the way down is typed `bv[W]`, and every leaf either is
 * wrapped in a `(bv[W])` cast or is itself a constant already typed
 * `bv[W]` -- i.e. exactly the shape `widen_term(..., W)` produces for the
 * "extend-all-sides" atom rules (comparisons, `bf_interval`, both-compound
 * equality). `saw_widening_cast` is an in/out accumulator set to `true` as
 * soon as a `(bv[W])` cast over a *strictly narrower* operand is seen
 * anywhere; the caller requires it (see widen_atom) before accepting
 * saturation.
 *
 * This is the idempotency guard for the extend-all-sides atom shapes (see
 * widen_atom's doc comment): it is checked BEFORE needed_width ever runs
 * again, specifically because needed_width's bf_cast-boundary rule (Task
 * 3, locked behavior) cannot distinguish "a genuinely wide value" from "a
 * zero-extended narrower one" -- re-running it on an already-widened,
 * non-truncated comparison would blindly re-sum already-wide cast-boundary
 * widths (e.g. a bf_mul of two already-(bv[W])-cast operands recomputes to
 * W+W, not W) and inflate without bound under repeated application. The
 * truncating-assignment shape never satisfies this check (its untouched
 * bare side is a `variable`, which is never saturated), which is correct:
 * that shape's idempotency is already handled correctly by needed_width's
 * own recomputation (the outer truncating cast is itself a boundary
 * matching base_w, so W == base_w naturally on a second call) -- see the
 * "idempotent (assignment shape)" test.
 *
 * Per node kind:
 *
 *  - `variable` -> never saturated. widen_term always wraps a variable
 *    leaf in a `(bv[W])` cast, and (unlike a constant, below) that cast
 *    survives construction, so a bare variable under an operator can only
 *    mean this side was never elaborated.
 *  - `ba_constant` / `bf_t` / `bf_f` -> saturated iff the leaf's OWN type
 *    is bv-family of width exactly `W`. A constant leaf's `(bv[W])` cast
 *    does NOT survive: `bv_term_cast` (bv_ba_hooks.tmpl.h:862-941) folds
 *    any cast of a constant / top / bottom element into a bare, retyped
 *    constant at construction time. Returning `false` here unconditionally
 *    (as this guard originally did) therefore made every widened atom with
 *    a folded constant leaf -- `i1*i2 <= {200}`, `x << {3}`, ... -- fail
 *    the guard on re-entry and re-widen without bound, up to the D4 cap
 *    and its answer-flipping conservative fallback. Re-entry is routine
 *    (normalize_non_temp runs inside is_tau_formula_sat, solve() and the
 *    fixpoint loops), so this is the load-bearing half of the fix.
 *  - `bf_cast` -> saturated iff its own declared target is exactly `W`.
 *    Its operand is not inspected for saturation, mirroring widen_term's
 *    own treatment of casts as opaque boundaries; its *width* is read only
 *    to decide whether this is a widening cast (see below).
 *  - any other operator -> saturated iff it is itself typed `bv[W]` and
 *    every child is (recursively) saturated at `W`.
 *
 * Why the extra `saw_widening_cast` requirement -- the false-positive
 * direction. Constant leaves alone are far too weak a signal: a fresh,
 * never-widened `{16}:bv[8] * {16}:bv[8] <= {10}:bv[8]` is uniformly typed
 * bv[8] with three constant leaves, so leaf-saturation alone would skip it
 * and lose the mode entirely. Requiring at least one `(bv[W])` cast over a
 * strictly narrower operand pins the pass's own signature, and it is
 * *always* present whenever skipping would actually matter:
 *
 *  (1) Re-widening only ever escalates when the tree still contains a
 *      GROWING operator (`bf_add`, `bf_mul`, a constant-amount `bf_shl`);
 *      every other operator's rule returns at most its widest child, so
 *      needed_width recomputes exactly `W` and widen_atom's own
 *      `W == base_w` check already returns the atom unchanged.
 *  (2) A growing operator whose operands are all constants folds away at
 *      construction (`term_add`/`term_mul`/`term_shl` all fold constant
 *      pairs, and at `W` the exact result fits by construction, so the
 *      fit-gate of Task 2 never keeps it symbolic) -- an all-constant atom
 *      collapses to T/F during elaboration and never survives to re-entry.
 *      So a surviving growing operator has a non-constant leaf under it.
 *  (3) That leaf is a `(bv[W])` cast over a variable of the atom's own
 *      pre-widening base width, or over a user cast of declared width
 *      `wu`. In both cases the operand's width is strictly below `W`:
 *      widen_term ran with `W > base_w`, and for any growing operator
 *      `needed(op) > needed(child)` (add: `max+1`; mul: `l+r` with both
 *      >= 1; shl: `l+k` with `k >= 1`), while `W` is the maximum over all
 *      nodes -- so every leaf under a growing operator contributed
 *      strictly less than `W`.
 *
 * The residual false positives are therefore exactly the atoms a user
 * wrote already in the pass's canonical widened form -- every operator
 * typed `bv[W]`, every leaf either a `bv[W]` constant or explicitly cast
 * from something narrower to `bv[W]`, e.g.
 * `(bv[16]) x:bv[8] * (bv[16]) y:bv[8] <= {200}:bv[16]`. Those are
 * *indistinguishable*, node for node, from this pass's own output (nothing
 * in the tree records who built a cast), so treating them as a fixed point
 * is forced by the idempotency requirement itself: the pass's output must
 * be its own fixed point, hence any input that coincides with one is left
 * alone. The cost is bounded and self-inflicted -- the user pinned every
 * operand's width with explicit casts, which is precisely how the default
 * mode is asked for locally (README, "casts pin widths exactly as today")
 * -- and it does NOT extend to same-width user casts:
 * `(bv[8]) x:bv[8] * (bv[8]) y:bv[8] <= {200}:bv[8]` has no cast over a
 * narrower operand, so it is not accepted as saturated and still widens to
 * W = 16 as the mode requires. Nor does it affect the common
 * width-pinning idiom `(bv[8]) (x:bv[8] * {255}:bv[8])' <= {200}:bv[8]`:
 * there `W == base_w`, so the guard firing or not makes no difference --
 * needed_width's own `W == base_w` path returns the atom unchanged either
 * way.
 *
 * @tparam node Tree node type.
 * @param side A `bf` side of the atom under test.
 * @param W The atom's own current width.
 * @param saw_widening_cast [in,out] Set to `true` once a `(bv[W])` cast over a
 * strictly narrower operand is seen; the caller requires it before accepting
 * saturation.
 * @return `true` iff every operator on the side is typed `bv[W]` and every
 * leaf is either a `(bv[W])` cast or a constant already typed `bv[W]`.
 * @endinternal
 */
template <NodeType node>
bool is_side_saturated_at(tref side, size_t W, bool& saw_widening_cast) {
	using tau = tree<node>;

	const tau& op = tau::get(side)[0];
	switch (op.value.nt) {
	case tau::bf_parenthesis:
		return is_side_saturated_at<node>(op.child(0), W,
			saw_widening_cast);
	case tau::bf_cast: {
		const size_t cast_type = op.get_ba_type();
		if (cast_type == 0 || !is_bv_type_family<node>(cast_type))
			return false;
		if (get_bv_width<node>(cast_type) != W) return false;
		// An operand of unknown width (0, from a tree never run through
		// type inference) counts as narrower: erring towards "already
		// widened" keeps the pass terminating, which is the stronger
		// safety property here -- escalation ends at the D4 cap, whose
		// conservative fallback flips answers.
		if (bf_cast_operand_width<node>(op) < W) saw_widening_cast = true;
		return true;
	}
	case tau::variable:
		return false; // a bare, uncast variable is never "already widened"
	case tau::ba_constant:
	case tau::bf_t:
	case tau::bf_f: {
		// A constant leaf keeps no cast (bv_term_cast folds it away), so
		// its own declared width is the only evidence available.
		const size_t leaf_type = op.get_ba_type();
		return leaf_type != 0 && is_bv_type_family<node>(leaf_type)
			&& get_bv_width<node>(leaf_type) == W;
	}
	default: {
		// An opaque subterm (bf_ref, capture, ...) has ba_type 0 (or, in
		// principle, some non-bv type) -- get_bv_width DBG-asserts on
		// anything that isn't bv-family, so it must never be called on
		// one. Guard explicitly rather than relying on op.get_ba_type()
		// happening to equal W (it can't, since W is always a bv-family
		// width here, but 0 would still reach get_bv_width unguarded).
		const size_t op_type = op.get_ba_type();
		if (op_type == 0 || !is_bv_type_family<node>(op_type)) return false;
		if (get_bv_width<node>(op_type) != W) return false;
		if (!is_side_saturated_at<node>(op.child(0), W, saw_widening_cast))
			return false;
		if (op.children_size() > 1
				&& !is_side_saturated_at<node>(op.child(1), W,
					saw_widening_cast)) return false;
		return true;
	}
	}
}

template <NodeType node>
tref widen_atom(tref atom) {
	using tau = tree<node>;

	const tau& n = tau::get(atom);
	const size_t atom_type = n.get_ba_type();
	if (!is_bv_type_family<node>(atom_type)) return atom; // not bv: no-op

	const size_t base_w = get_bv_width<node>(atom_type);
	const bool is_interval = n.value.nt == tau::bf_interval;
	const size_t nsides = is_interval ? 3 : 2;

	// Step 1: idempotency guard (see is_side_saturated_at's doc comment): if every
	// side is ALREADY uniformly expressed at the atom's own current width
	// AND at least one leaf is pinned there by a genuine widening cast
	// (a `(bv[W])` cast over a strictly narrower operand -- this pass's own
	// signature, and the thing that tells an elaborated atom apart from a
	// fresh uniformly-typed one like `{16}*{16} <= {10}`), this atom is
	// already in the pass's canonical "extend-all" widened form -- return
	// it unchanged rather than letting needed_width recompute (and inflate)
	// it below.
	{
		bool all_saturated = true, saw_widening_cast = false;
		for (size_t i = 0; i < nsides && all_saturated; ++i)
			if (!is_side_saturated_at<node>(n.child(i), base_w,
					saw_widening_cast))
				all_saturated = false;
		if (all_saturated && saw_widening_cast) return atom;
	}

	// Step 2: needed_width on every side; any opaque side (returns 0,
	// e.g. bf_ref/capture) -- skip the atom entirely, unchanged.
	tref sides[3];
	size_t maxW = 0;
	for (size_t i = 0; i < nsides; ++i) {
		sides[i] = n.child(i);
		if (needed_width<node>(sides[i], base_w, maxW) == 0) return atom;
	}

	// Step 3: W == base_w -> nothing to elaborate; W > bv_max_width ->
	// loud, logged cap error (D4).
	const size_t W = maxW;
	if (W == base_w) return atom;
	if (W > bv_max_width) {
		LOG_ERROR << "bv-widening: required width " << W
			<< " exceeds bv-max-width " << bv_max_width;
		return nullptr;
	}

	// Step 4: bf_eq/bf_neq with exactly one bare-storage side --
	// truncating "assignment" semantics: the bare side is untouched, the
	// other side is elaborated at W and truncated back to base_w.
	if (!is_interval
			&& (n.value.nt == tau::bf_eq || n.value.nt == tau::bf_neq)) {
		const bool bare0 = is_bare_storage_side<node>(sides[0]);
		const bool bare1 = is_bare_storage_side<node>(sides[1]);
		if (bare0 != bare1) {
			const size_t other_i = bare0 ? 1 : 0;
			tref wide_other = widen_term<node>(sides[other_i], base_w, W);
			tref truncated = build_bf_cast<node>(wide_other,
				bv_type_id<node>(base_w));
			tref l = bare0 ? sides[0] : truncated;
			tref r = bare0 ? truncated : sides[1];
			return tau::get(n.get_type(), l, r);
		}
	}

	// Step 5: comparisons, bf_interval, and both-compound equality --
	// extend every side exactly, no truncation.
	trefs new_sides;
	new_sides.reserve(nsides);
	for (size_t i = 0; i < nsides; ++i)
		new_sides.push_back(widen_term<node>(sides[i], base_w, W));
	if (is_interval) return tau::get(n.get_type(), new_sides);
	return tau::get(n.get_type(), new_sides[0], new_sides[1]);
}

template <NodeType node>
tref widen_bv_arithmetic(tref fm) {
	using tau = tree<node>;

	if (!bv_widening) return fm; // pass fully inert when the flag is off

	// The full bv-family atom nt set widen_atom knows how to elaborate --
	// parser/tau.tgf:64-74. `is<node>({...})` (tau_tree_queries.tmpl.h) is
	// the factory overload that returns a std::function<bool(tref)>,
	// suitable directly as select_top's predicate.
	//
	// select_top descends through the whole tree (quantifiers included --
	// a quantifier node's own nt never matches the atom predicate, so
	// recursion continues underneath it), collecting every bv-widening-
	// eligible atom node, wherever it is nested.
	trefs atoms = tau::get(fm).select_top(is<node>({
		tau::bf_eq, tau::bf_neq, tau::bf_lt, tau::bf_nlt,
		tau::bf_lteq, tau::bf_nlteq, tau::bf_gt, tau::bf_ngt,
		tau::bf_gteq, tau::bf_ngteq, tau::bf_interval
	}));
	if (atoms.empty()) return fm; // no bv atom at all: nothing to do

	// Build the replacement map for the CHANGED atoms only: widen_atom is
	// itself a no-op (returns the same tref) for a non-bv-family atom or
	// an already-saturated bv atom, so those are simply left out of
	// `changes` rather than mapped to themselves. The map type MUST be the
	// project's subtree_map (a std::map ordered by subtree_less, tree.h)
	// -- NOT std::map<tref, tref> keyed on raw pointer identity -- since
	// that is the type rewriter::replace's subtree_map overload expects
	// (the idiom used throughout, e.g.
	// normal_forms_transformations.tmpl.h's shift_io_vars_in_fm).
	subtree_map<node, tref> changes;
	for (tref atom : atoms) {
		tref widened = widen_atom<node>(atom);
		if (widened == nullptr) return nullptr; // D4 cap: propagate the failure
		if (widened != atom) changes[atom] = widened;
	}
	if (changes.empty()) return fm; // every atom was already a no-op

	return rewriter::replace<node>(fm, changes);
}

} // namespace idni::tau_lang
