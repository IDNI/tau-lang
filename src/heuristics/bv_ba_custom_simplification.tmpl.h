// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_ba_simplification"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

template<NodeType node>
typename node::type inverse_of(size_t operation) {
	using tau = tree<node>;

	if (operation == tau::bf_add) return tau::bf_sub;
	if (operation == tau::bf_mul) return tau::bf_div;
	if (operation == tau::bf_sub) return tau::bf_add;
	if (operation == tau::bf_div) return tau::bf_mul;
	return tau::nul; // null is not allowed in a term
}

// Both overloads return/accept *bare* (unwrapped) values: a lone leaf
// operand as-is, or an unwrapped operation(...)-kind node for two or more
// folded operands. tau::bf-wrap is applied only where a value is used as an
// operand of another operation(...) call, never around the final result.
// The final result replaces the block's top node (a bf_add/bf_sub/bf_mul/
// bf_div node), whose own parent already supplies its "bf" wrapper. Wrapping
// the final result again nests the whole term in one more redundant "bf"
// layer -- harmless for a lone leaf (leaves aren't re-wrapped by a later
// round), but for a multi-operand result this re-detects the same
// operation(...) node as a "new" block on every subsequent round and
// re-wraps it again, growing the term forever instead of reaching a
// fixpoint.
/**
 * @internal
 * @brief Splits @p arguments into a folded variable-side term and a folded
 * constant-side term for a single associative @p operation.
 *
 * Constants (`ba_constant` and the `T`/`F` elements) are combined pairwise
 * via @p operation into one bare term; *everything else* is combined into
 * another. Either side may be `nullptr` if no operand of that kind was
 * present. Note the second side is not restricted to `variable` nodes: a
 * block operand may be any term the block traversal treated as opaque (an
 * `&` sub-term, a cast, a `bf_ref` call, ...), and dropping those -- which a
 * `variable`-only classification would silently do -- deletes part of the
 * term (see @ref simplify_blocks).
 * @tparam node Tree node type.
 * @param arguments Operands to fold (variables, constants or opaque terms).
 * @param operation The associative bf operator to fold with (`bf_add` etc.).
 * @param type BA type identifier (unused directly, kept for signature symmetry).
 * @return Pair of (folded variable term or `nullptr`, folded constant term or `nullptr`).
 *
 * @par Example
 * @code{.cpp}
 * // Folding [X, {1}, {2}] with bf_add: variables={X}, constants={1}+{2}.
 * // Operands must be bare (unwrapped) nodes -- trim() the parsed "bf"
 * // wrapper first, or every operand is classified as a non-constant term.
 * auto pbf = parse_bf();
 * tref x  = tau::trim(tau::get("X:bv[8]", pbf));
 * tref c1 = tau::trim(tau::get("{1}:bv[8]", pbf));
 * tref c2 = tau::trim(tau::get("{2}:bv[8]", pbf));
 * auto [vars, ctes] = build_simplification<node_t>({x, c1, c2}, tau::bf_add, bv_type_id<node_t>(8));
 * // tau::get(vars).to_str() == "X"
 * // tau::get(ctes).to_str() == "{ 1 }:bv[8]+{ 2 }:bv[8]" (not yet folded to
 * // { 3 }:bv[8] -- that happens when the caller's own fixpoint loop
 * // re-visits this constant-only subterm as its own block)
 * @endcode
 * @endinternal
 */
template<NodeType node>
std::pair<tref, tref> build_simplification(const trefs& arguments, size_t operation, [[maybe_unused]] size_t type) {
	using tau = tree<node>;

	tref ctes = nullptr;
	tref vars = nullptr;

	for (const tref& op : arguments) {
		auto nt = tau::get(op).get_type();
		const bool constant = nt == tau::ba_constant
			|| nt == tau::bf_t || nt == tau::bf_f;
		tref& side = constant ? ctes : vars;
		side = side
			? tau::get(operation, tau::get(tau::bf, side), tau::get(tau::bf, op))
			: op;
	}
	return {vars, ctes};
}

/**
 * @internal
 * @brief Combines a "direct" side and an "inverse" side into a single bare
 * term: `args_side operation^-1 invs_side` (e.g. `args_side - invs_side`
 * when @p operation is `bf_add`).
 *
 * Never introduces a spurious identity operand: either side may be
 * `nullptr` (e.g. `1 - X` has a constant only on the args side and a
 * variable only on the invs side), and unconditionally wrapping a null
 * `tref` would crash tree construction.
 * @tparam node Tree node type.
 * @param operation The associative operator whose inverse combines the two sides.
 * @param type BA type identifier, used to build a `0` identity when @p args_side is absent.
 * @param args_side The direct-side term, or `nullptr`.
 * @param invs_side The inverse-side term, or `nullptr`.
 * @return The combined bare term, or `nullptr` if both sides are absent.
 *
 * @par Example
 * @code{.cpp}
 * // Only an inverse side present, operation=bf_add: builds bf_sub(0, X)
 * tref x = tau::trim(tau::get("X:bv[8]", parse_bf()));
 * tref res = combine_diff<node_t>(tau::bf_add, bv_type_id<node_t>(8), nullptr, x);
 * CHECK( tau::get(res).is(tau::bf_sub) );
 * @endcode
 * @endinternal
 */
template<NodeType node>
tref combine_diff(size_t operation, size_t type, tref args_side, tref invs_side) {
	using tau = tree<node>;

	if (!args_side && !invs_side) return nullptr;
	if (!invs_side) return args_side;
	if (!args_side) return tau::get(inverse_of<node>(operation), _0<node>(type), tau::get(tau::bf, invs_side));
	return tau::get(inverse_of<node>(operation), tau::get(tau::bf, args_side), tau::get(tau::bf, invs_side));
}

/**
 * @internal
 * @brief Folds one associative block, given its direct operands @p arguments
 * and its inverse-side operands @p inverses.
 *
 * Only the additive family is a group here. `+`/`-` are associative and
 * commutative in Z/2^n and mutually inverse, so operands may be reordered and
 * a term occurring on both sides cancels. `*`/`/` share none of that:
 * `bvudiv` is not the inverse of `bvmul` (`X*Y/Y != X` as soon as `X*Y` wraps
 * -- bv[4], X=5, Y=4 gives 1 -- and `X/X` is all ones at X=0), division does
 * not reassociate (`a/(b/c) != (a/b)/c`, `(a*b)/c != a*(b/c)` once either
 * product wraps), and neither BA element (`0` = all zeros, `1` = all ones) is
 * the multiplicative identity, so there is no identity to seed an absent side
 * with. A multiplicative block is therefore folded only when it holds no
 * division at all, in which case it is a pure product and regrouping is
 * sound; `simplify_blocks` recovers the pure-product sub-blocks of a block it
 * declined here by descending into the block top's operands.
 * @tparam node Tree node type.
 * @param arguments Direct-side operands.
 * @param inverses Inverse-side operands.
 * @param operation The block's canonical operator (`bf_add` or `bf_mul`).
 * @param type BA type identifier used to build the `0` identity.
 * @return The folded bare term, or `nullptr` when no sound fold exists.
 * @endinternal
 */
template<NodeType node>
tref build_simplification(const trefs& arguments, const trefs& inverses, size_t operation, size_t type) {
	using tau = tree<node>;

	const bool additive = operation == tau::bf_add;
	if (!additive && !inverses.empty()) return nullptr;

	// removing commons elements from arguments and inverses; sound only for
	// the additive family (see the note above)
	trefs args = arguments;
	trefs invs = inverses;
	if (additive) for (const tref& arg : arguments) {
		auto it = std::find(invs.begin(), invs.end(), arg);
		if (it != invs.end()) {
			invs.erase(it);
			args.erase(std::find(args.begin(), args.end(), arg));
		}
	}

	if (invs.empty()) {
		auto [vars, ctes] = build_simplification<node>(args, operation, type);
		// bare, like every other return here: `_0` is bf-wrapped, and
		// returning it would nest the replacement in a second bf layer
		if (!vars && !ctes) return _0_trimmed<node>(type);
		if (!vars) return ctes;
		if (!ctes) return vars;
		return tau::get(operation, tau::get(tau::bf, vars), tau::get(tau::bf, ctes));
	}

	// Everything below is additive-only: a multiplicative block with a
	// non-empty inverse side already returned nullptr above, so `_0(type)`
	// is the additive identity here and never a bogus multiplicative one.
	if (args.empty()) {
		auto [vars, ctes] = build_simplification<node>(invs, operation, type);
		// bare, like every other return here: `_0` is bf-wrapped, and
		// returning it would nest the replacement in a second bf layer
		if (!vars && !ctes) return _0_trimmed<node>(type);
		if (!vars) return tau::get(inverse_of<node>(operation), _0<node>(type), tau::get(tau::bf, ctes));
		if (!ctes) return tau::get(inverse_of<node>(operation), _0<node>(type), tau::get(tau::bf, vars));
		return tau::get(inverse_of<node>(operation),
			_0<node>(type),
			tau::get(tau::bf,
				tau::get(operation, tau::get(tau::bf, vars), tau::get(tau::bf, ctes))));
	}

	auto [invs_vars, invs_ctes] = build_simplification<node>(invs, operation, type);
	auto [args_vars, args_ctes] = build_simplification<node>(args, operation, type);

	tref vars = combine_diff<node>(operation, type, args_vars, invs_vars);
	tref ctes = combine_diff<node>(operation, type, args_ctes, invs_ctes);
	if (!vars && !ctes) return _0_trimmed<node>(type);
	if (!vars) return ctes;
	if (!ctes) return vars;
	return tau::get(operation, tau::get(tau::bf, vars), tau::get(tau::bf, ctes));
}

/**
 * @internal
 * @brief Single pass over @p n: finds every maximal associative `+`/`-` or
 * `*`/`/` block and records the folded replacement for its top node.
 *
 * A block is discovered top-down. `collect` descends from a block top only
 * through nodes of the *same* family (`+`/`-`, or `*`/`/`), flipping the
 * args/inverses side each time it crosses an inverse operator, and stops at
 * everything else. Every other node kind -- `&`, `|`, `^`, shifts, `%`, a
 * cast, a `bf_ref` call, a parenthesis, a quantified term -- is therefore an
 * opaque block operand, kept whole on the variable side by
 * `build_simplification`. The driver then re-visits each collected operand,
 * so the operands of a non-block operator really are scanned as fresh,
 * independent blocks (and a block under a cast takes its BA type from its own
 * top node, not from the root).
 *
 * This replaces a post-order traversal that maintained a stack of operand
 * frames. That stack could not hold its invariant: a node hitting the
 * traversal's `default` arm pushed no frame, yet `variable`/`ba_constant`
 * leaves *inside* it did, so the frames of a non-block operator's operands
 * survived and were folded into the next block -- `(x & y) + z` folded to
 * `y + z`, `f(x) + y` to `x + y`, and `((a + b) & c) + d` popped a one-frame
 * stack and read past the end of an empty vector.
 * @tparam node Tree node type.
 * @param n Term to scan for foldable blocks.
 * @return Rewrite map from original block-top node to its folded replacement.
 * Only blocks whose fold actually changes the term are recorded.
 *
 * @par Example
 * @code{.cpp}
 * // {1}:bv[8] + X:bv[8] + {2}:bv[8]: one additive block, folds to X + {3}
 * // (see tests/integration/test_integration-heuristics-bv_ba_custom_simplification.cpp).
 * auto pbf = parse_bf();
 * tref src = tau::get("{1}:bv[8] + X:bv[8] + {2}:bv[8]", pbf);
 * auto changes = simplify_blocks<node_t>(src);
 * tref simplified = rewriter::replace<node_t>(src, changes);
 * // tau::get(simplified) == tau::get(tau::get("X:bv[8] + {3}:bv[8]", pbf))
 * @endcode
 * @endinternal
 */
template<NodeType node>
subtree_map<node, tref> simplify_blocks(const tref& n) {
	using tau = tree<node>;

	subtree_map<node, tref> changes;

	auto is_block_operator = [](size_t nt) {
		return nt == tau::bf_add || nt == tau::bf_sub
			|| nt == tau::bf_mul || nt == tau::bf_div;
	};
	// bf_add and bf_mul are the canonical (non-inverse) family operators.
	auto family_of = [](size_t nt) {
		return (nt == tau::bf_sub) ? size_t(tau::bf_add)
			: (nt == tau::bf_div) ? size_t(tau::bf_mul) : nt;
	};

	// Collects the operands of the maximal block of family `family` rooted at
	// `top`, in left-to-right order, splitting them between the direct side
	// (`args`) and the inverse side (`invs`). `operands` receives them all,
	// in the same order, for the driver to re-visit.
	auto collect = [&](tref top, size_t family,
			trefs& args, trefs& invs, trefs& operands)
	{
		const size_t inverse_op = inverse_of<node>(family);
		std::vector<std::pair<tref, bool>> pending{ { top, false } };
		while (!pending.empty()) {
			auto [t, inverted] = pending.back(); pending.pop_back();
			const size_t nt = tau::get(t).get_type();
			if (nt == family || nt == inverse_op) {
				const bool inverse = nt == inverse_op;
				// right first, so the left subtree is drained first
				// and the operand order matches the source order
				pending.emplace_back(tau::trim(tau::get(t).child(1)),
					inverted != inverse);
				pending.emplace_back(tau::trim(tau::get(t).child(0)),
					inverted);
				continue;
			}
			(inverted ? invs : args).push_back(t);
			operands.push_back(t);
		}
	};

	// The block's BA type: its own top node when typed, else the first typed
	// operand, else the root's. Taking it from the root alone gives sub-blocks
	// under a width-changing cast the wrong width.
	auto block_type = [&](tref top, const trefs& operands) {
		if (size_t t = tau::get(top).get_ba_type(); t) return t;
		for (tref operand : operands)
			if (size_t t = tau::get(operand).get_ba_type(); t) return t;
		return tau::get(n).get_ba_type();
	};

	std::vector<tref> to_visit{ n };
	while (!to_visit.empty()) {
		tref t = to_visit.back(); to_visit.pop_back();
		const size_t nt = tau::get(t).get_type();
		if (is_block_operator(nt)) {
			const size_t family = family_of(nt);
			trefs args, invs, operands;
			collect(t, family, args, invs, operands);
			tref folded = build_simplification<node>(args, invs,
				family, block_type(t, operands));
			if (folded) {
				if (folded != t) changes[t] = folded;
				// the block absorbed these operands; each is a
				// fresh, independent block of its own
				for (tref operand : operands)
					to_visit.push_back(operand);
			} else {
				// no sound fold for this block (a multiplicative
				// block holding a division): fall back to scanning
				// the top node's two operands separately, so a pure
				// product below it is still folded
				to_visit.push_back(tau::trim(tau::get(t).child(0)));
				to_visit.push_back(tau::trim(tau::get(t).child(1)));
			}
			continue;
		}
		for (tref c : tau::get(t).get_children())
			to_visit.push_back(c);
	}

	return changes;
}

template<NodeType node>
tref bv_ba_custom_simplification(const tref term) {
	// Loop simplify_blocks to a fixpoint. Previously current was inserted
	// into visited every iteration right before the loop condition
	// checked for it, so the condition was always false and the loop ran
	// exactly once, discarding any further simplification. Guard against
	// a longer oscillating cycle (as done in repeat_all, see RR-2) with a
	// visited set, and against an ever-growing rewrite with max_rounds.
	tref current = term;
	std::unordered_set<tref> visited{current};
	constexpr size_t max_rounds = 1'000'000;
	size_t round = 0;
	for (; round < max_rounds; ++round) {
		auto changes = simplify_blocks<node>(current);
		tref next = rewriter::replace<node>(current, changes);
		if (next == current) break;
		if (visited.contains(next)) { current = next; break; }
		visited.insert(next);
		current = next;
	}
	if (round == max_rounds)
		LOG_ERROR << "bv_ba_custom_simplification: exceeded " << max_rounds
			<< " rounds without reaching a fixpoint or cycle, giving up";

#ifdef DEBUG
	LOG_TRACE << "bv_ba_custom_simplification/term: " << LOG_FM(term) << "\n";
	LOG_TRACE << "bv_ba_custom_simplification/final: " << LOG_FM(current) << "\n";
#endif // DEBUG

	return current;
}

} // namespace idni::tau_lang