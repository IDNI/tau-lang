// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "bv_simplify_options.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_ba_simplification"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

// The inverse operator used to cancel @p operation when moving an operand
// across a block: bf_add <-> bf_sub and bf_mul <-> bf_div. Only these four
// are valid inputs; anything else returns tau::nul after logging (and
// asserts in debug builds) — see the caller-bug note in the body.
template<NodeType node>
typename node::type inverse_of(size_t operation) {
	using tau = tree<node>;

	if (operation == tau::bf_add) return tau::bf_sub;
	if (operation == tau::bf_mul) return tau::bf_div;
	if (operation == tau::bf_sub) return tau::bf_add;
	if (operation == tau::bf_div) return tau::bf_mul;
	// Every caller in this file only ever passes one of the four operators
	// above; reaching here means a caller bug, not a malformed term.
	DBG(assert(false && "inverse_of: operation is not one of bf_add/bf_sub/bf_mul/bf_div");)
	LOG_ERROR << "inverse_of: unsupported operation " << LOG_NT(operation);
	return tau::nul; // null is not allowed in a term
}

// The identity element for an associative @p operation: 0 for bf_add, 1 for
// bf_mul. Only these two are ever passed as the direct (non-inverse)
// operator of a block (see the DBG asserts in combine_diff/build_simplification).
template<NodeType node>
tref identity_of(size_t operation, size_t type) {
	using tau = tree<node>;

	if (operation == tau::bf_add) return _0<node>(type);
	if (operation == tau::bf_mul) return _1<node>(type);
	DBG(assert(false && "identity_of: operation must be bf_add or bf_mul");)
	LOG_ERROR << "identity_of: unsupported operation " << LOG_NT(operation);
	return _0<node>(type);
}

// Both overloads return/accept *bare* (unwrapped) values: a lone leaf
// operand as-is, or an unwrapped operation(...)-kind node for two or more
// folded operands. tau::bf-wrap is applied only where a value is used as an
// operand of another operation(...) call, never around the final result.
// The final result replaces either last_operation (a bf_add/bf_sub/bf_mul
// node -- bf_div is never a block root, see simplify_block_root) or is used
// as an operand by the caller's own last_operation substitution, and
// last_operation's own parent already supplies its "bf"
// wrapper. Wrapping the final result again nests the whole term in one more
// redundant "bf" layer -- harmless for a lone leaf (leaves aren't
// re-wrapped by a later round), but for a multi-operand result this
// re-detects the same operation(...) node as a "new" block on every
// subsequent round and re-wraps it again, growing the term forever instead
// of reaching a fixpoint.
/**
 * @internal
 * @brief Splits @p arguments into a folded variable-side term and a folded
 * constant-side term for a single associative @p operation.
 *
 * Variables -- and any other non-constant operand, e.g. an opaque
 * subtree contributed by a nested block (see @ref collect_block_operand)
 * -- are combined pairwise via @p operation into one bare term; constants
 * (and `T`/`F`) are combined into another. Either side may be `nullptr` if
 * no operand of that kind was present. No operand kind is ever dropped.
 * @tparam node Tree node type.
 * @param arguments Operands to fold (variables and/or constants).
 * @param operation The associative bf operator to fold with (`bf_add` etc.).
 * @param type BA type identifier (unused directly, kept for signature symmetry).
 * @return Pair of (folded variable term or `nullptr`, folded constant term or `nullptr`).
 *
 * @par Example
 * @code{.cpp}
 * // Folding [X, {1}, {2}] with bf_add: variables={X}, constants={1}+{2}.
 * // Operands must be bare (unwrapped) nodes -- trim() the parsed "bf"
 * // wrapper first, or every operand falls through the switch untouched.
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
		switch (nt) {
			case tau::ba_constant: case tau::bf_t: case tau::bf_f:
				ctes = ctes ? tau::get(operation, tau::get(tau::bf, ctes), tau::get(tau::bf, op))
					: op;
				break;
			// tau::variable, and anything else (an opaque non-constant
			// subtree contributed by collect_block_operand): fold
			// identically -- both are "unknown" values for this operation.
			default:
				vars = vars ? tau::get(operation, tau::get(tau::bf, vars), tau::get(tau::bf, op))
					: op;
				break;
		}
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
 * @param type BA type identifier, used to build the identity (0 for bf_add,
 * 1 for bf_mul) when @p args_side is absent.
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

	DBG(assert((operation == tau::bf_add || operation == tau::bf_mul)
		&& "combine_diff: operation must be the direct (non-inverse) associative operator");)

	if (!args_side && !invs_side) return nullptr;
	if (!invs_side) return args_side;
	if (!args_side) return tau::get(inverse_of<node>(operation), identity_of<node>(operation, type), tau::get(tau::bf, invs_side));
	return tau::get(inverse_of<node>(operation), tau::get(tau::bf, args_side), tau::get(tau::bf, invs_side));
}

template<NodeType node>
tref build_simplification(const trefs& arguments, const trefs& inverses, size_t operation, size_t type) {
	using tau = tree<node>;

	DBG(assert((operation == tau::bf_add || operation == tau::bf_mul)
		&& "build_simplification: operation must be the direct (non-inverse) associative operator");)

	// Cancelling a common operand (a + b - b = a) is only sound for
	// bf_add: bitvector division truncates, so a * b / b == a does not
	// hold in general. invs is structurally always empty for bf_mul
	// (division never folds into a block), so this is a no-op there anyway.
	trefs args = arguments;
	trefs invs = inverses;
	if (operation == tau::bf_add) {
		for (const tref& arg : arguments) {
			auto it = std::find(invs.begin(), invs.end(), arg);
			if (it != invs.end()) {
				invs.erase(it);
				args.erase(std::find(args.begin(), args.end(), arg));
			}
		}
	}

	if (invs.empty()) {
		auto [vars, ctes] = build_simplification<node>(args, operation, type);
		if (!vars && !ctes) return identity_of<node>(operation, type);
		if (!vars) return ctes;
		if (!ctes) return vars;
		return tau::get(operation, tau::get(tau::bf, vars), tau::get(tau::bf, ctes));
	}

	if (args.empty()) {
		auto [vars, ctes] = build_simplification<node>(invs, operation, type);
		if (!vars && !ctes) return identity_of<node>(operation, type);
		if (!vars) return tau::get(inverse_of<node>(operation), identity_of<node>(operation, type), tau::get(tau::bf, ctes));
		if (!ctes) return tau::get(inverse_of<node>(operation), identity_of<node>(operation, type), tau::get(tau::bf, vars));
		return tau::get(inverse_of<node>(operation),
			identity_of<node>(operation, type),
			tau::get(tau::bf,
				tau::get(operation, tau::get(tau::bf, vars), tau::get(tau::bf, ctes))));
	}

	auto [invs_vars, invs_ctes] = build_simplification<node>(invs, operation, type);
	auto [args_vars, args_ctes] = build_simplification<node>(args, operation, type);

	tref vars = combine_diff<node>(operation, type, args_vars, invs_vars);
	tref ctes = combine_diff<node>(operation, type, args_ctes, invs_ctes);
	if (!vars && !ctes) return identity_of<node>(operation, type);
	if (!vars) return ctes;
	if (!ctes) return vars;
	return tau::get(operation, tau::get(tau::bf, vars), tau::get(tau::bf, ctes));
}

// Forward declaration: simplify_block_root and collect_block_operand are
// mutually recursive (a block operand that isn't part of the current
// associative run is simplified as its own independent subtree, which may
// itself be -- or contain -- a block).
template<NodeType node>
std::pair<trefs, trefs> collect_block_operand(tref n, size_t operation, subtree_map<node, tref>& changes);

/**
 * @internal
 * @brief Recursive core of `simplify_blocks`: if @p n is a `+`/`-`/`*`
 * block root, folds its associative run via `collect_block_operand` and
 * `build_simplification`, recording the replacement in @p changes;
 * otherwise it recurses into each child as an independent block.
 *
 * `/` is deliberately opaque, like `&`/`<<`/etc: bitvector division
 * truncates, so it is neither invertible into a `*` run (`X / (A/B)` is
 * not `X * B / A`) nor safely reorderable with `*`. Folding stops at a
 * `/` node and resumes independently inside its operands.
 *
 * Each call owns its own local state; nothing is shared across sibling
 * subtrees.
 * @tparam node Tree node type.
 * @param n Bare (already `trim`med) subtree to process.
 * @param changes Rewrite map from block-top node to folded replacement,
 * accumulated across the whole recursion.
 *
 * @par Example
 * @code{.cpp}
 * // {1}:bv[8] + X:bv[8] + {2}:bv[8]: one additive block, folds to X + {3}
 * // (see tests/integration/test_integration-heuristics-bv_ba_custom_simplification.cpp:30-43).
 * auto pbf = parse_bf();
 * tref src = tau::get("{1}:bv[8] + X:bv[8] + {2}:bv[8]", pbf);
 * auto changes = simplify_blocks<node_t>(src);
 * tref simplified = rewriter::replace<node_t>(src, changes);
 * // tau::get(simplified).to_str() matches one of several equivalent
 * // stringifications of "X + { 3 }:bv[8]" (see matches_to_str_to_any_of
 * // in the cited test)
 * @endcode
 * @endinternal
 */
template<NodeType node>
void simplify_block_root(tref n, subtree_map<node, tref>& changes) {
	using tau = tree<node>;

	if (!n) {
		DBG(assert(false && "simplify_block_root: null subtree");)
		LOG_ERROR << "simplify_block_root: null subtree, skipping";
		return;
	}

	auto nt = tau::get(n).get_type();
	if (nt == tau::bf_add || nt == tau::bf_sub || nt == tau::bf_mul) {
		auto inverse = (nt == tau::bf_sub);
		auto operation = inverse ? inverse_of<node>(nt) : nt;
		auto type = tau::get(n).get_ba_type();
		auto [args, invs] = collect_block_operand<node>(n, operation, changes);

		// A binary block operator always contributes at least two leaves
		// to its own run, so this is structurally unreachable; kept as a
		// documented, checked invariant rather than trusted silently.
		DBG(assert((!args.empty() || !invs.empty())
			&& "simplify_block_root: block collected no operands");)
		if (args.empty() && invs.empty()) {
			LOG_ERROR << "simplify_block_root: block rooted at "
				<< LOG_NT(nt) << " collected no operands, leaving it unsimplified";
			return;
		}

		changes[n] = build_simplification<node>(args, invs, operation, type);

		DBG(LOG_TRACE << "simplify_block_root/operation: " << LOG_NT(operation) << "\n";
			LOG_TRACE << "simplify_block_root/simplification: " << tau::get(changes[n]).tree_to_str() << "\n";)
		return;
	}

	for (tref c : tau::get(n).children()) simplify_block_root<node>(c, changes);
}

/**
 * @internal
 * @brief Contributes @p n to the associative run being collected for
 * @p operation. If @p n is a same-family `+`/`-` node, splits it into its
 * two operands and recurses, applying @p operation's inverse swap to the
 * second operand's (args, invs) when @p n itself is the inverse form (e.g.
 * `-` inside a `+` run). Anything else -- a leaf, a different-family block
 * (e.g. `*` nested in a `+` run), `/` (always opaque, never part of a run --
 * see `simplify_block_root`), or a non-block operator -- does not belong to
 * this run: it is simplified independently via `simplify_block_root` and
 * contributed as exactly one opaque operand, the same as a leaf.
 * @tparam node Tree node type.
 * @param n Bare subtree to fold into the run.
 * @param operation The run's associative operator (`bf_add` or `bf_mul`).
 * @param changes Rewrite map, forwarded to any nested block this discovers.
 * @return `(args, invs)`: @p n's operands on the "direct" and "inverse" side.
 *
 * @par Example
 * `a - (b - c)`: recursing into `(b - c)` gives `(args=[b], invs=[c])`;
 * the outer `-` swaps that to `(args=[c], invs=[b])` before merging with
 * `a`, giving `(args=[a, c], invs=[b])`, i.e. `(a + c) - b`.
 * @endinternal
 */
template<NodeType node>
std::pair<trefs, trefs> collect_block_operand(tref n, size_t operation, subtree_map<node, tref>& changes) {
	using tau = tree<node>;

	if (!n) {
		DBG(assert(false && "collect_block_operand: null subtree");)
		LOG_ERROR << "collect_block_operand: null subtree, contributing nothing";
		return {};
	}

	auto nt = tau::get(n).get_type();
	if (nt == tau::bf_add || nt == tau::bf_sub || nt == tau::bf_mul) {
		auto inverse = (nt == tau::bf_sub);
		auto nt_op = inverse ? inverse_of<node>(nt) : nt;
		if (nt_op == operation) {
			DBG(assert(tau::get(n).children_size() == 2
				&& "collect_block_operand: block operator is not binary");)
			auto left  = tau::trim(tau::get(n).child(0));
			auto right = tau::trim(tau::get(n).child(1));
			auto [args, invs] = collect_block_operand<node>(left, operation, changes);
			auto [rargs, rinvs] = collect_block_operand<node>(right, operation, changes);
			if (inverse) std::swap(rargs, rinvs);
			args.insert(args.end(), rargs.begin(), rargs.end());
			invs.insert(invs.end(), rinvs.begin(), rinvs.end());
			return { std::move(args), std::move(invs) };
		}
	}

	// Not part of this run: simplify independently (may register nested
	// block replacements in `changes`) and contribute the original
	// subtree as one opaque operand. The actual substitution is applied
	// by the caller's rewriter::replace, possibly needing another
	// fixpoint round to fully flatten, like constant folding.
	simplify_block_root<node>(n, changes);
	return { trefs{n}, trefs{} };
}

/**
 * @internal
 * @brief Scans @p n for maximal `+`/`-`/`*` associative blocks and
 * folds each one's variable and constant operands together.
 *
 * Any operator that is not `bf_add`/`bf_sub`/`bf_mul` -- `/` (bitvector
 * division truncates, so it is neither invertible into a `*` run nor
 * safely reorderable with `*`; see `simplify_block_root`), `&`, `|`, `^`,
 * `<<`, `>>`, `mod`, `cast`, `interval`, ... -- terminates a block above
 * it and is recursed into as fresh, independent blocks.
 * Repeatedly re-run by `bv_ba_custom_simplification` until a fixpoint.
 * @tparam node Tree node type.
 * @param n Term to scan for foldable blocks.
 * @return Rewrite map from original block-top node to its folded replacement.
 * @endinternal
 */
template<NodeType node>
subtree_map<node, tref> simplify_blocks(const tref& n) {
	subtree_map<node, tref> changes;
	simplify_block_root<node>(n, changes);
	return changes;
}

template<NodeType node>
tref bv_ba_custom_simplification(const tref term) {
	// Loop simplify_blocks to a fixpoint. Previously current was inserted
	// into visited every iteration right before the loop condition
	// checked for it, so the condition was always false and the loop ran
	// exactly once, discarding any further simplification. Guard against
	// a longer oscillating cycle (as done in repeat_all, see RR-2) with a
	// visited set, and against an ever-growing rewrite with the global
	// max_simplify_rounds (0 = unlimited).
	tref current = term;
	std::unordered_set<tref> visited{current};
	size_t round = 0;
	for (; !max_simplify_rounds || round < max_simplify_rounds; ++round) {
		auto changes = simplify_blocks<node>(current);
		tref next = rewriter::replace<node>(current, changes);
		if (next == current) break;
		if (visited.contains(next)) { current = next; break; }
		visited.insert(next);
		current = next;
	}
	if (max_simplify_rounds && round == max_simplify_rounds)
		LOG_ERROR << "bv_ba_custom_simplification: exceeded "
			<< max_simplify_rounds << " rounds (max-simplify-rounds)"
			" without reaching a fixpoint or cycle, giving up";

#ifdef DEBUG
	LOG_TRACE << "bv_ba_custom_simplification/term: " << LOG_FM(term) << "\n";
	LOG_TRACE << "bv_ba_custom_simplification/final: " << LOG_FM(current) << "\n";
#endif // DEBUG

	return current;
}

} // namespace idni::tau_lang