// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file normal_forms.h
 * @brief Normal form transformations for Tau language formulas.
 *
 * This file declares functions and types used to transform well-formed formulas
 * (wff) and Boolean functions (bf) into various normal forms: negation normal
 * form (NNF), disjunctive normal form (DNF), conjunctive normal form (CNF),
 * Boole normal form (BNF), ordered normal form (ONF), and prenex normal form
 * (PNF). It also provides quantifier-handling utilities (push/eliminate), as
 * well as syntactic simplification routines operating on the AST.
 *
 * Template implementations reside in normal_forms.tmpl.h, which is included
 * at the bottom of this file.
 */

#ifndef __IDNI__TAU__NORMAL_FORMS_H__
#define __IDNI__TAU__NORMAL_FORMS_H__

#include <list>
#include <numeric>

#include "execution.h"
#include "tau_bdd.h"


namespace idni::tau_lang {

/**
 * @brief Memory slot identifiers for pre-order traversal caching.
 *
 * These slots are used as template parameters to `pre_order::apply_unique`
 * to select a specific static cache that avoids redundant rewriting on
 * already-visited sub-trees within a single traversal pass.
 */
enum MemorySlotPre {
	normalize_ba_m,          ///< Cache slot for normalize_ba traversals.
	push_negation_in_m,      ///< Cache slot for push_negation_in traversals.
	to_dnf_m,                ///< Cache slot for to_dnf traversals.
	to_cnf_m,                ///< Cache slot for to_cnf traversals.
	eliminate_quantifiers_m, ///< Cache slot for quantifier elimination traversals.
	anti_prenex_step_m,      ///< Cache slot for anti-prenex step traversals.
	synt_path_simp_m         ///< Cache slot for syntactic path simplification traversals.
};

/**
 * @brief Memory slot identifiers for post-order traversal caching.
 *
 * These slots are used as template parameters to `post_order::apply_unique`
 * to select a specific static cache.
 */
enum MemorySlotPost {
	anti_prenex_m ///< Cache slot for anti-prenex post-order traversals.
};

/**
 * @brief Convert `!=` (bf_neq) atoms to negated equalities `!(= 0)`.
 *
 * Rewrites every sub-formula of the form `$X != 0` into `!($X = 0)`.
 * @tparam node Tree node type.
 * @param fm The formula to transform.
 * @return Transformed formula with all `bf_neq` replaced by negated `bf_eq`.
 */
template <NodeType node>
tref unequal_to_not_equal(tref fm);

/**
 * @brief Convert negated equalities `!($X = 0)` back to `$X != 0`.
 *
 * Inverse of `unequal_to_not_equal`: rewrites `!($X = 0)` to `$X != 0`.
 * @tparam node Tree node type.
 * @param fm The formula to transform.
 * @return Transformed formula with negated `bf_eq` replaced by `bf_neq`.
 */
template <NodeType node>
tref not_equal_to_unequal(tref fm);

/**
 * @brief Normalize atomic formula operators to a canonical subset.
 *
 * Rewrites non-canonical comparison operators (`!=`, `!<=`, `!<`, `>=`, `>`,
 * `!>=`, `!>`) into equivalent combinations of `<`, `<=`, `=`, `!=`, and
 * `wff_neg`.
 * @tparam node Tree node type.
 * @param fm The formula to normalize.
 * @return Formula with atomic operators reduced to a canonical set.
 */
template <NodeType node>
tref normalize_atomic_formula_operators(tref fm);

/**
 * @brief Rewrite `>` and `>=` comparisons to `<` and `<=`.
 *
 * Replaces `bf_gt(a, b)` with `bf_lt(b, a)` and `bf_gteq(a, b)` with
 * `bf_lteq(b, a)` (and their negated forms), so only the less-than direction
 * needs to be handled downstream.
 * @tparam node Tree node type.
 * @param fm The formula to transform.
 * @return Formula with all `>` / `>=` replaced by flipped `<` / `<=`.
 */
template <NodeType node>
tref gt_gteq_to_lt_lteq(tref fm);

/**
 * @brief Expand an equality or disequality over a disjunction of BF terms.
 *
 * Applies the rewrites:
 *   - `(A | B) = 0  =>  A = 0 && B = 0`
 *   - `(A | B) != 0 =>  A != 0 || B != 0`
 * @tparam node Tree node type.
 * @param fm The formula to transform.
 * @return Expanded formula.
 */
template <NodeType node>
tref unsqueeze_wff(const tref& fm);

/**
 * @brief Merge adjacent equalities or disequalities over a common BF disjunction.
 *
 * Applies the rewrites:
 *   - `A = 0 && B = 0  =>  (A | B) = 0`
 *   - `A != 0 || B != 0 => (A | B) != 0`
 * Only merges atoms of the same Boolean algebra type.
 * @tparam node Tree node type.
 * @param fm The formula to transform.
 * @return Merged formula.
 */
template <NodeType node>
tref squeeze_wff(const tref& fm);

/**
 * @brief Expand positive equalities `(A | B) = 0` only.
 *
 * A restricted variant of `unsqueeze_wff` that only rewrites
 * `(A | B) = 0  =>  A = 0 && B = 0`.
 * @tparam node Tree node type.
 * @param fm The formula to transform.
 * @return Expanded formula.
 */
template <NodeType node>
tref unsqueeze_wff_pos(tref fm);

/**
 * @brief Merge adjacent positive equalities `A = 0 && B = 0` only.
 *
 * A restricted variant of `squeeze_wff` that only rewrites
 * `A = 0 && B = 0  =>  (A | B) = 0`.
 * @tparam node Tree node type.
 * @param fm The formula to transform.
 * @return Merged formula.
 */
template <NodeType node>
tref squeeze_wff_pos(tref fm);

/**
 * @brief Expand negative disequalities `(A | B) != 0` only.
 *
 * A restricted variant of `unsqueeze_wff` that only rewrites
 * `(A | B) != 0  =>  A != 0 || B != 0`.
 * @tparam node Tree node type.
 * @param fm The formula to transform.
 * @return Expanded formula.
 */
template <NodeType node>
tref unsqueeze_wff_neg(tref fm);

/**
 * @brief Merge adjacent negative disequalities `A != 0 || B != 0` only.
 *
 * A restricted variant of `squeeze_wff` that only rewrites
 * `A != 0 || B != 0  =>  (A | B) != 0`.
 * @tparam node Tree node type.
 * @param fm The formula to transform.
 * @return Merged formula.
 */
template <NodeType node>
tref squeeze_wff_neg(tref fm);

/**
 * @brief Normalize an equality or disequality to the form `expr (!)= 0`.
 *
 * Rewrites `X = Y` to `(X + Y) = 0` and `X != Y` to `(X + Y) != 0`
 * by introducing an XOR term.
 * @tparam node Tree node type.
 * @param eq The equation (wrapped in wff) to normalize.
 * @return Normalized equation with zero right-hand side.
 */
template <NodeType node>
tref norm_equation(tref eq);

/**
 * @brief Normalize an equality or disequality (trimmed) to `expr (!)= 0`.
 *
 * Same semantics as `norm_equation` but operates on the raw `bf_eq`/`bf_neq`
 * node directly (without the enclosing `wff` wrapper), returning a trimmed
 * result.
 * @tparam node Tree node type.
 * @param eq The equality/disequality node to normalize.
 * @return Normalized and trimmed equation.
 */
template <NodeType node>
tref norm_trimmed_equation(tref eq);

/**
 * @brief Reverse normalization: recover `X (!)= Y` from `(X + Y) (!)= 0`.
 *
 * Detects the pattern `(X xor Y) = 0` (or `!= 0`) and rewrites it back to
 * `X = Y` (or `X != Y`).
 * @tparam node Tree node type.
 * @param eq The normalized equation to denormalize.
 * @return Denormalized equation, or the original if the pattern is absent.
 */
template <NodeType node>
tref denorm_equation(tref eq);

/**
 * @brief Normalize all equalities and disequalities in `fm` to `expr (!)= 0`.
 *
 * Applies `norm_equation` to every `bf_eq`/`bf_neq` sub-formula via a
 * pre-order traversal.
 * @tparam node Tree node type.
 * @param fm The formula containing equations to normalize.
 * @return Formula with all equations in normalized form.
 */
template <NodeType node>
tref norm_all_equations(tref fm);

/**
 * @brief Expand a single XOR sub-expression to its OR-AND definition.
 *
 * Rewrites `A xor B` as `(A & !B) | (!A & B)`.
 * @tparam node Tree node type.
 * @param fm The formula whose top-level bf_xor child is to be expanded.
 * @return Formula with the top-level XOR expanded.
 */
template <NodeType node>
tref apply_xor_def(tref fm);

/**
 * @brief Expand all XOR sub-expressions throughout `fm`.
 *
 * Applies `apply_xor_def` recursively to every `bf_xor` node via pre-order.
 * @tparam node Tree node type.
 * @param fm The formula to transform.
 * @return Formula with all XOR nodes expanded.
 */
template <NodeType node>
tref apply_all_xor_def(tref fm);

// -----------------------------------------------------------------------------

/**
 * @brief Normalize all Boolean algebra constants in a BF term.
 *
 * Traverses the Boolean function `fm`, pushes negations into constants, and
 * calls `node::ba::normalize` on every `ba_constant` node to reduce it to
 * its canonical representative.
 * @tparam node Tree node type.
 * @param fm A Boolean function (`bf`) node to normalize.
 * @return The BF with all BA constants in normal form.
 */
template <NodeType node>
tref normalize_ba(tref fm);

/// @cond FORWARD_DECL
template <NodeType node>
struct bf_reduce_canonical;
/// @endcond

/**
 * @brief Locate the innermost quantified sub-formula and its bound variable.
 *
 * Traverses `n` bottom-up to find the deepest quantifier (`wff_ex`, `wff_all`,
 * `wff_sometimes`, `wff_always`). Returns a pair of the traverser pointing to
 * the bound variable and the inner `wff` node. If no quantifier is present the
 * traverser is empty and the inner formula is `n` itself.
 * @tparam node Tree node type.
 * @param n Formula to inspect.
 * @return `{variable_traverser, inner_wff}`.
 */
template <NodeType node>
std::pair<typename tree<node>::traverser, tref> get_inner_quantified_wff(tref n);

/**
 * @brief Functor that converts a formula to Ordered Normal Form (ONF) w.r.t. a variable.
 *
 * Assumes the input formula is already in DNF. For each conjunct it rewrites
 * every equality `f = 0` (resp. `f != 0`) that contains the given variable
 * into an interval constraint `f[0] <= var <= f[1]` (resp. `f[0] > var || var > f[1]`),
 * where `f[0]` and `f[1]` are obtained by substituting 0 and 1 for the variable.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct onf_wff {
	/// @brief Construct with the variable to order with respect to.
	explicit onf_wff(tref _var);
	/// @brief Apply ONF conversion to formula `n`.
	tref operator()(tref n) const;
private:
	tref onf_subformula(tref n) const;
	tref var = nullptr;
};

/// @brief Type alias for `onf_wff<node>`.
template <NodeType node>
using onf_wff_t = onf_wff<node>;

/**
 * @brief Pipe operator: apply `onf_wff` to a tree traverser.
 * @tparam node Tree node type.
 */
template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& t, const onf_wff_t<node>& r);

/**
 * @brief Convert formula `n` to Ordered Normal Form with respect to `var`.
 *
 * First converts to DNF, then applies `onf_wff` twice (to stabilize).
 * @tparam node Tree node type.
 * @param n Formula to convert.
 * @param var The variable that defines the ordering dimension.
 * @return Formula in ONF with respect to `var`.
 */
template <NodeType node>
tref onf(tref n, tref var);

/**
 * @brief Attempt to merge the current assignment vector with existing DNF paths.
 *
 * If the current assignment `i` (up to position `p`) differs from some existing
 * path in exactly one variable, the two paths are resolved (Quine-McCluskey style)
 * and the resolved path replaces both. Returns `true` if the resulting set
 * covers everything (i.e. the whole formula is tautological up to the remaining
 * variables).
 * @param i Current variable assignment vector (values: -1=false, 1=true, 2=don't-care).
 * @param paths Set of existing DNF path vectors.
 * @param p Number of variables already assigned (prefix length).
 * @param surface When `true` resolve `paths[j]`; when `false` resolve `i` directly.
 * @return `true` if the merged result is a tautology (all paths collapsed).
 */
bool reduce_paths(std::vector<int_t>& i, std::vector<std::vector<int_t>>& paths,
	int_t p, bool surface = true);

/**
 * @brief Simplify a set of DNF path vectors by absorbing and merging redundant paths.
 *
 * For every pair of paths that differ in exactly one position (and are otherwise
 * compatible under don't-care wildcards), merges them into a single path with a
 * don't-care in that position, and removes any path that is a subset of another.
 * @param paths The path vectors to simplify in-place.
 */
void join_paths(std::vector<std::vector<int_t>>& paths);

/**
 * @brief Lexicographic comparator for Tau formula variables.
 *
 * Compares two variable nodes by the string representation of their sub-trees.
 * Used to define a canonical variable order for BDD construction and DNF
 * reduction.
 * @tparam node Tree node type.
 */
template <NodeType node>
auto lex_var_comp = [](tref x, tref y) {
#ifdef TAU_CACHE
	using cache_t = std::map<std::pair<tref, tref>, bool,
				subtree_pair_less<node, tref>>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find({x,y}); it != cache.end())
		return it->second;
#endif // TAU_CACHE
	// TODO (QUESTION) strings have unique id, use .data() instead?
	auto xx = tree<node>::get(x).to_str();
	auto yy = tree<node>::get(y).to_str();
#ifdef TAU_CACHE
	return cache.emplace(std::make_pair(x, y), xx < yy).first->second;
#endif // TAU_CACHE
	return xx < yy;
};

/**
 * @brief Predicate that classifies a wff node as a BDD variable.
 *
 * In BDD-based DNF/CNF reductions of well-formed formulas the following node
 * types are treated as atomic BDD variables: `bf_eq`, `wff_ref`, `wff_ex`,
 * `wff_sometimes`, `wff_always`, `wff_all`, and `constraint`.
 * @tparam node Tree node type.
 * @todo Extend for the full grammar.
 */
template <NodeType node>
inline auto is_wff_bdd_var = [](tref n) {
	using tau = tree<node>;
	const auto& t = tau::get(n);
	DBG(assert(!t.is(tau::bf_neq));)
	return t.child_is(tau::bf_eq)
		|| t.child_is(tau::wff_ref)
		|| t.child_is(tau::wff_ex)
		|| t.child_is(tau::wff_sometimes)
		|| t.child_is(tau::wff_always)
		|| t.child_is(tau::wff_all)
		|| t.child_is(tau::constraint);
};

/**
 * @brief Predicate that classifies a bf node as a BDD variable.
 *
 * In BDD-based reductions of Boolean functions the following node types are
 * treated as atomic BDD variables: `variable`, `capture`, `bf_ref`,
 * `ba_constant`, `bf_fall`, and `bf_fex`.
 * @tparam node Tree node type.
 */
template <NodeType node>
inline auto is_bf_bdd_var = [](tref n) {
	using tau = tree<node>;
	const auto& t = tau::get(n);
	return t.child_is(tau::variable)
		|| t.child_is(tau::capture)
		|| t.child_is(tau::bf_ref)
		|| t.child_is(tau::ba_constant)
		|| t.child_is(tau::bf_fall)
		|| t.child_is(tau::bf_fex);
};

/**
 * @brief Mark variables in `i` as irrelevant (don't-care) if absent from `fm`.
 *
 * Starting at position `p+1` in `vars`, sets `i[v] = 2` for every variable
 * that does not appear in `fm`, allowing subsequent path merging to ignore it.
 * @tparam node Tree node type.
 * @param fm Formula whose variables are inspected.
 * @param vars Ordered list of all variables.
 * @param i Assignment vector to update in-place.
 * @param p Current prefix length (variables 0..p already assigned).
 * @param is_var Predicate identifying variable nodes.
 */
template <NodeType node>
void elim_vars_in_assignment(tref fm, const auto& vars, auto& i,
	const int_t p, const auto& is_var);

/**
 * @brief Reduce a DNF or CNF formula by removing redundant clauses.
 *
 * Converts the formula to a path representation and eliminates dominated or
 * tautological clauses via `dnf_cnf_to_reduced`. Returns the simplified formula.
 * @tparam node Tree node type.
 * @tparam is_cnf When `true`, treat `fm` as CNF instead of DNF.
 * @param fm Formula in DNF (or CNF when `is_cnf` is `true`).
 * @return Reduced formula.
 */
template <NodeType node, bool is_cnf = false>
tref reduce(tref fm);

/**
 * @brief Recursively enumerate variable assignments and collect coefficients.
 *
 * For each Boolean variable in `vars` starting at position `p`, substitutes 0
 * and 1 (or `F`/`T` for wff mode), propagates the simplification, and stores
 * the resulting coefficient together with its assignment path in `dnf`.
 * Calls `reduce_paths` after each leaf to merge compatible paths.
 * Returns `true` if the formula evaluates to a tautology.
 * @tparam node Tree node type.
 * @param fm Current (partially simplified) formula.
 * @param vars Ordered variable list.
 * @param i Current assignment vector.
 * @param dnf Map from coefficient to list of associated assignment paths.
 * @param is_var Predicate identifying variable nodes.
 * @param p Current variable index (default: 0).
 * @param is_wff `true` when operating on wff, `false` for bf (default: false).
 * @return `true` if the result is a tautology.
 */
template <NodeType node>
bool assign_and_reduce(tref fm, const trefs& vars, std::vector<int_t>& i,
	auto& dnf, const auto& is_var, int_t p = 0, bool is_wff = false);

/**
 * @brief Compute the reduced DNF of a Boolean function.
 *
 * Enumerates all variable assignments via `assign_and_reduce` using the
 * lexicographic variable order `lex_var_comp`, then reconstructs the minimal
 * DNF from the resulting coefficient-path map.
 * @tparam node Tree node type.
 * @param fm Boolean function to reduce.
 * @param make_paths_disjoint When `true`, skip the `join_paths` merging step
 *        so that every surviving path is explicitly disjoint.
 * @return Reduced DNF of `fm`.
 */
template <NodeType node>
tref bf_reduced_dnf(tref fm, bool make_paths_disjoint = false);

/**
 * @brief Functor adapter that applies `bf_reduced_dnf` to every `bf` sub-tree.
 *
 * Allows `bf_reduce_canonical` to be used in the tree traversal pipeline via
 * `operator|`.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct bf_reduce_canonical {
	/// @brief Apply canonical DNF reduction to `fm`.
	tref operator()(tref fm) const;
};

/**
 * @brief Pipe operator: apply `bf_reduce_canonical` to a tree traverser.
 * @tparam node Tree node type.
 */
template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& t,
	const bf_reduce_canonical<node>& r);

/**
 * @brief Check whether assignment vector `i` is contained in (subsumed by) some path.
 *
 * Returns `true` if there exists a path in `paths` that subsumes `i`
 * (i.e. every specific bit of the path also matches in `i`). As a side effect,
 * any path in `paths` that is strictly subsumed by `i` is erased.
 * @param i Assignment vector to test.
 * @param paths Existing set of path vectors.
 * @return `true` if `i` is subsumed by an existing path.
 */
inline bool is_contained_in(const std::vector<int_t>& i, auto& paths);

/**
 * @brief Convert a single DNF/CNF clause to an assignment vector.
 *
 * Traverses `clause` and records, for each variable known to `var_pos`, whether
 * it appears positively (1), negated (-1), or is absent (2 = don't-care).
 * Also detects clauses that are trivially decided (always true or always false).
 * @tparam node Tree node type.
 * @param clause A single clause (conjunction of literals for DNF, disjunction for CNF).
 * @param var_pos Map from variable node to its index in the path vector.
 * @param wff `true` if operating on wff atoms, `false` for bf atoms.
 * @param is_cnf `true` if the clause is from a CNF, `false` for DNF.
 * @return `{assignment_vector, clause_is_decided}`.
 */
template <NodeType node>
std::pair<std::vector<int_t>, bool> clause_to_vector(tref clause,
	const auto& var_pos, const bool wff, const bool is_cnf);

/**
 * @brief Collect and reduce path vectors from all clauses of a DNF/CNF formula.
 *
 * Iterates over the leaves (clauses) of `new_fm`, converts each to a path
 * vector via `clause_to_vector`, and (when `all_reductions` is `true`) merges
 * compatible paths via `reduce_paths`.
 * @tparam node Tree node type.
 * @param new_fm Formula in DNF or CNF.
 * @param wff `true` for wff, `false` for bf.
 * @param vars Ordered variable list defining the path vector dimensions.
 * @param[out] decided Set to `false` if at least one satisfiable clause is found.
 * @param is_cnf `true` to treat `new_fm` as CNF.
 * @param all_reductions `true` to apply `reduce_paths` after each clause (default).
 * @return Vector of reduced path vectors.
 */
template <NodeType node>
std::vector<std::vector<int_t>> collect_paths(tref new_fm, bool wff,
	const auto& vars, bool& decided, bool is_cnf, bool all_reductions = true);

/**
 * @brief Reconstruct a reduced formula from a set of path vectors.
 *
 * Given the path vectors returned by `dnf_cnf_to_reduced`, builds the
 * corresponding DNF (or CNF when `is_cnf` is `true`) formula over `vars`.
 * Returns `F`/`0` (empty DNF) or `T`/`1` (empty CNF) for trivial cases.
 * @tparam node Tree node type.
 * @param paths Reduced path vectors.
 * @param vars Ordered variable list.
 * @param is_cnf `true` to build CNF.
 * @param wff `true` for wff, `false` for bf.
 * @param type_id BA type identifier for constant nodes.
 * @return Reconstructed reduced formula.
 */
template <NodeType node>
tref build_reduced_formula(const auto& paths, const auto& vars, bool is_cnf,
	bool wff, size_t type_id);

/**
 * @brief Reduce a DNF or CNF formula to a minimal equivalent form.
 *
 * Converts `fm` to NNF, extracts BDD variables, converts clauses to path
 * vectors, applies `join_paths` for absorption, and reconstructs the minimal
 * formula.
 * @tparam node Tree node type.
 * @param fm Formula in DNF (or CNF when `is_cnf` is `true`).
 * @param is_cnf `true` to treat `fm` as CNF (default: `false`).
 * @return `{reduced_paths, variable_list}`.
 */
template <NodeType node>
std::pair<std::vector<std::vector<int_t>>, trefs> dnf_cnf_to_reduced(
	tref fm, bool is_cnf = false);

/**
 * @brief Factor a DNF expression by grouping clauses with maximal common atoms.
 *
 * Attempts to reduce the size of the DNF by extracting common conjuncts from
 * pairs of disjuncts. Only pairs with the highest overlap count are merged.
 * @tparam node Tree node type.
 * @param fm Formula in DNF (wff or bf).
 * @return Formula in a factored, equivalent representation.
 */
template <NodeType node>
tref group_dnf_expression(tref fm);

/**
 * @brief Check whether the ordered sequence `v1` is a subsequence of `v2`.
 * @tparam node Tree node type.
 * @param v1 Candidate subsequence.
 * @param v2 Sequence to check in.
 * @return `true` if every element of `v1` appears in `v2` in the same order.
 */
template <NodeType node>
bool is_ordered_subset(const auto& v1, const auto& v2);

/**
 * @brief Check whether the ordered overlap between `v1` and `v2` is at least `i`.
 * @tparam node Tree node type.
 * @param i Minimum required overlap count.
 * @param v1 First sorted sequence.
 * @param v2 Second sorted sequence.
 * @return `true` if the two sequences share at least `i` common elements.
 */
template <NodeType node>
bool is_ordered_overlap_at_least(size_t i, const trefs& v1, const trefs& v2);

/**
 * @brief Count the number of elements shared by two ordered sequences.
 * @tparam node Tree node type.
 * @param v1 First sorted sequence.
 * @param v2 Second sorted sequence.
 * @return Number of common elements.
 */
template <NodeType node>
int_t get_ordered_overlap(const trefs& v1, const trefs& v2);

/**
 * @brief Functor that reduces a wff to DNF.
 *
 * Wraps `reduce<node, false>` so it can be used in the traversal pipeline.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct wff_reduce_dnf {
	/// @brief Reduce `fm` to DNF.
	tref operator() (tref fm) const;
};

/**
 * @brief Functor that reduces a wff to CNF.
 *
 * Wraps `reduce<node, true>` so it can be used in the traversal pipeline.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct wff_reduce_cnf {
	/// @brief Reduce `fm` to CNF.
	tref operator() (tref fm) const;
};

/**
 * @brief Pipe operator: apply `wff_reduce_dnf` to a tree traverser.
 * @tparam node Tree node type.
 */
template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& fm,
	const wff_reduce_dnf<node>& r);

/**
 * @brief Pipe operator: apply `wff_reduce_cnf` to a tree traverser.
 * @tparam node Tree node type.
 */
template <NodeType node>
typename tree<node>::traverser operator|(
	const typename tree<node>::traverser& fm,
	const wff_reduce_cnf<node>& r);

/**
 * @brief Build the DNF of the conjunction of two formulas already in DNF.
 *
 * Distributes conjunctions over disjunctions by taking the Cartesian product of
 * the clauses of `d1` and `d2` and building their pairwise conjunctions.
 * @tparam node Tree node type.
 * @param d1 First DNF formula (wff or bf).
 * @param d2 Second DNF formula (same type as `d1`).
 * @return DNF of `d1 && d2`.
 */
template <NodeType node>
tref conjunct_dnfs_to_dnf(tref d1, tref d2);

/**
 * @brief Build the CNF of the disjunction of two formulas already in CNF.
 *
 * Distributes disjunctions over conjunctions by taking the Cartesian product of
 * the clauses of `c1` and `c2` and building their pairwise disjunctions.
 * @tparam node Tree node type.
 * @param c1 First CNF formula (wff or bf).
 * @param c2 Second CNF formula (same type as `c1`).
 * @return CNF of `c1 || c2`.
 */
template <NodeType node>
tref disjunct_cnfs_to_cnf(tref c1, tref c2);

/**
 * @brief Apply one step of De Morgan / dual-negation rewriting.
 *
 * Pushes a single negation one level deeper into the formula using De Morgan's
 * laws and quantifier duality:
 *   - `!(A && B) => !A || !B`
 *   - `!(A || B) => !A && !B`
 *   - `!(ex x. P) => all x. !P`
 *   - `!(all x. P) => ex x. !P`
 *   - `!always P  => sometimes !P`
 *   - `!sometimes P => always !P`
 *   - `!(A = B)   => A != B` and dual
 *   - `!(A < B)   => A !< B` and dual (wff mode)
 *   - `!(A & B)   => !A | !B` (bf mode)
 *   - `!(A xor B) => !(A) xor B` (bf mode)
 *
 * When `is_wff` is `true` the wff rule set is used; otherwise the bf rule set.
 * @tparam node Tree node type.
 * @tparam is_wff `true` to apply wff rewriting, `false` for bf (default: `true`).
 * @param fm Formula with a single outermost negation.
 * @return Formula with the negation pushed one level in, or `fm` unchanged if
 *         no rule applies.
 */
template <NodeType node, bool is_wff = true>
tref push_negation_one_in(tref fm);

/**
 * @brief Push all negations to the literal level (NNF pre-step for DNF).
 *
 * Applies `push_negation_one_in` via `pre_order` until no negation remains
 * above a non-literal node. Operates on wff or bf depending on `is_wff`.
 * @tparam node Tree node type.
 * @tparam is_wff `true` for wff, `false` for bf (default: `true`).
 * @param fm Formula to transform.
 * @return Formula with all negations pushed to literals.
 */
template <NodeType node, bool is_wff = true>
tref push_negation_in(tref fm);

/**
 * @brief Convert a formula to Disjunctive Normal Form (DNF).
 *
 * Pushes negations in, then distributes conjunctions over disjunctions while
 * interleaving reductions via `wff_reduce_dnf` (or `reduce` for bf).
 * @tparam node Tree node type.
 * @tparam is_wff `true` for wff, `false` for bf (default: `true`).
 * @param fm Formula to convert.
 * @return Equivalent formula in DNF.
 */
template <NodeType node, bool is_wff = true>
tref to_dnf(tref fm);

/**
 * @brief Convert a formula to Conjunctive Normal Form (CNF).
 *
 * Pushes negations in, then distributes disjunctions over conjunctions while
 * interleaving reductions via `wff_reduce_cnf` (or `reduce<true>` for bf).
 * @tparam node Tree node type.
 * @tparam is_wff `true` for wff, `false` for bf (default: `true`).
 * @param fm Formula to convert.
 * @return Equivalent formula in CNF.
 */
template <NodeType node, bool is_wff = true>
tref to_cnf(tref fm);

/**
 * @brief Convert a formula to Negation Normal Form (NNF).
 *
 * Convenience wrapper around `push_negation_in<node, true>`.
 * @tparam node Tree node type.
 * @param fm Formula to convert.
 * @return Equivalent formula in NNF.
 */
template <NodeType node>
tref to_nnf(tref fm);

/**
 * @brief Shift all relative (non-initial) IO variable offsets in `fm` by `shift`.
 *
 * Increases the lookback of every `io_var` in `io_vars` that is not an initial
 * condition by adding `shift` to its time offset. Does nothing if `shift <= 0`.
 * @tparam node Tree node type.
 * @param fm Formula whose IO variables are to be shifted.
 * @param io_vars Collection of IO variable nodes to consider.
 * @param shift Number of time steps to add to the lookback.
 * @return Formula with adjusted IO variable offsets.
 */
template <NodeType node>
tref shift_io_vars_in_fm(tref fm, const auto& io_vars, const int_t shift);

/**
 * @brief Shift initial-condition IO variables in `fm` by `shift`.
 *
 * Adds `shift` to the absolute time point of every `io_var` that *is* an
 * initial condition. Returns `F` if any resulting time point would be negative.
 * Does nothing if `shift <= 0`.
 * @tparam node Tree node type.
 * @param fm Formula whose initial IO variables are to be shifted.
 * @param io_vars Collection of IO variable nodes to consider.
 * @param shift Time offset to add.
 * @return Formula with adjusted initial IO variable time points, or `F`.
 */
template <NodeType node>
tref shift_const_io_vars_in_fm(tref fm, const auto& io_vars, const int_t shift);

/**
 * @brief Conjoin two `always`-quantified formulas, aligning their lookbacks.
 *
 * Strips the `always` wrapper from `fm1_aw` and `fm2_aw`, determines their
 * maximum shift, and adjusts the formula with the shorter lookback by adding
 * the difference, then returns their conjunction.
 * @tparam node Tree node type.
 * @param fm1_aw First (possibly `always`-wrapped) formula.
 * @param fm2_aw Second (possibly `always`-wrapped) formula.
 * @return Conjunction of the two formulas with aligned lookbacks.
 */
template <NodeType node>
tref always_conjunction (tref fm1_aw, tref fm2_aw);

/**
 * @brief Push an existential quantifier one step deeper into `fm`.
 *
 * Applies the following rewrites to the immediately scoped formula:
 *   - `ex x. (A || B)  =>  (ex x. A) || (ex x. B)` (push into disjunction)
 *   - `ex x. (A && B)  =>  (ex x. A_x) && A_no_x`  (remove quantifier from clauses not containing `x`)
 *   - `ex x. ex y. P   =>  ex y. ex x. P`            (commute quantifiers)
 *   - `ex x. P` (no `x` in P) `=> P`                (drop vacuous quantifier)
 * Nodes that cannot be simplified further are added to `excluded`.
 * @tparam node Tree node type.
 * @param fm Existentially quantified formula.
 * @param excluded Optional set to track nodes that should not be revisited.
 * @return Simplified formula.
 */
template <NodeType node>
tref push_existential_quantifier_one(tref fm, subtree_set<node>* excluded = nullptr);

/**
 * @brief Push a universal quantifier one step deeper into `fm`.
 *
 * Applies the dual rewrites to `push_existential_quantifier_one`:
 *   - `all x. (A && B) =>  (all x. A) && (all x. B)` (push into conjunction)
 *   - `all x. (A || B) =>  (all x. A_x) || A_no_x`   (remove from disjuncts not containing `x`)
 *   - `all x. all y. P =>  all y. all x. P`            (commute quantifiers)
 *   - `all x. P` (no `x` in P) `=> P`                 (drop vacuous quantifier)
 * @tparam node Tree node type.
 * @param fm Universally quantified formula.
 * @return Simplified formula.
 */
template <NodeType node>
tref push_universal_quantifier_one(tref fm);

/**
 * @brief Collect all positive equalities in `n` of the given BA type and merge them.
 *
 * Finds all `bf_eq` atoms whose BA type matches `type_id`, normalizes each
 * to `f = 0` form, and combines their left-hand sides into a single disjunction
 * `f1 | f2 | ...`. Returns the resulting BF, or `nullptr` if no match is found.
 * @tparam node Tree node type.
 * @param n Formula to search for equalities.
 * @param type_id BA type identifier selecting which equalities to merge.
 * @return Disjunction of all matching left-hand sides, or `nullptr`.
 */
template <NodeType node>
tref squeeze_positives(tref n, size_t type_id);

/**
 * @brief Replace all free variables in `fm` by the constant `val`.
 *
 * Collects the free variables of `fm` and substitutes each with `val`.
 * @tparam node Tree node type.
 * @param fm Formula whose free variables are to be replaced.
 * @param val The constant value to substitute.
 * @return `fm` with all free variables replaced by `val`.
 */
template <NodeType node>
tref replace_free_vars_by(tref fm, tref val);

/**
 * @brief Equality-aware simplifier using a union-find data structure.
 *
 * Traverses a formula in pre-order and, at each `bf_eq` node, records the
 * equality in a union-find structure (stacked per disjunction branch). Later
 * occurrences of terms known to be equal are replaced by their canonical
 * representative, potentially simplifying the whole formula to `T` or `F`.
 * @tparam node Tree node type.
 */
template <NodeType node>
struct simplify_using_equality;

/**
 * @brief Path-based syntactic simplifier for wff and bf.
 *
 * Detects syntactically contradictory or tautological sub-formulas along every
 * conjunction/disjunction path and replaces them by `F`/`T` (or `0`/`1` for bf).
 * Works purely syntactically (no semantic normalization) and runs in near-linear
 * time in the formula size.
 * @tparam node Tree node type.
 */
template <NodeType node>
class syntactic_path_simplification;

/**
 * @brief Try to simplify an atomic formula with respect to a single variable.
 *
 * Substitutes 0 and 1 for `var` in `atomic_fm` and checks syntactically whether
 * the result is always 0, always 1, or independent of `var`. In those cases the
 * atom is simplified accordingly.
 * @tparam node Tree node type.
 * @param atomic_fm The atomic formula (equation) to simplify.
 * @param var The variable to test.
 * @return Simplified atomic formula.
 */
template <NodeType node>
tref syntactic_variable_simplification(tref atomic_fm, tref var);

/**
 * @brief Apply full syntactic simplifications to a formula.
 *
 * Combines `simplify_using_equality` (equality propagation) and
 * `syntactic_path_simplification` (path contradiction/tautology removal) in
 * sequence.
 * @tparam node Tree node type.
 * @param formula Formula to simplify.
 * @return Simplified formula.
 */
template <NodeType node>
tref syntactic_formula_simplification(tref formula);

/**
 * @brief Apply syntactic simplifications to a single atomic formula.
 *
 * Normalizes the equation, applies `syntactic_path_simplification` to each
 * term, and then calls `syntactic_variable_simplification` for every free
 * variable.
 * @tparam node Tree node type.
 * @param atomic_formula Atomic formula to simplify.
 * @return Simplified atomic formula.
 */
template <NodeType node>
tref syntactic_atomic_formula_simplification (tref atomic_formula);

/**
 * @brief Squeeze and absorb equations across the Boolean structure of a formula.
 *
 * Collects `= 0` and `!= 0` equations within conjunctions/disjunctions and:
 *   - Squeezes those sharing variables: `f = 0 && g = 0  =>  (f|g) = 0`
 *   - Absorbs into other equations sharing variables, reducing their complexity.
 *
 * This is a lightweight pre-processing step before Boole normal form.
 * @tparam node Tree node type.
 * @param formula The formula to process.
 * @return Formula with equations squeezed and absorbed.
 */
template <NodeType node>
tref squeeze_absorb (tref formula);

/**
 * @brief Squeeze and absorb equations sharing a specific variable.
 *
 * Variant of `squeeze_absorb` that only merges equations containing `var`.
 * @tparam node Tree node type.
 * @param formula The formula to process.
 * @param var The shared variable that triggers merging.
 * @return Formula with equations squeezed and absorbed with respect to `var`.
 */
template <NodeType node>
tref squeeze_absorb (tref formula, tref var);

/**
 * @brief Perform one Boole decomposition step on a term with respect to `var`.
 *
 * Computes `term[var:=1]` and `term[var:=0]`, applies path simplification, and
 * returns `var & term_1 | !var & term_0` (with shortcut optimizations for the
 * cases where `term_1 == term_0` or one equals 0/1).
 * @tparam node Tree node type.
 * @param term Boolean function term to decompose.
 * @param var The variable on which to decompose.
 * @return Boole decomposition of `term` with respect to `var`.
 */
template <NodeType node>
tref term_boole_decomposition(tref term, tref var);

/**
 * @brief Recursively perform Boole decomposition on a term for multiple variables.
 *
 * Starting from `vars[idx]`, substitutes 1 and 0 for each variable and
 * combines via `var & rec(term_1) | !var & rec(term_0)`, propagating early
 * terminations (0 or 1 results).
 * @tparam node Tree node type.
 * @param term Boolean function term to decompose.
 * @param vars Ordered list of variables to use for decomposition.
 * @param idx Starting index in `vars`.
 * @param free_funcs When `true`, treat `bf_ref` nodes as additional variables.
 * @return Fully decomposed term.
 */
template <NodeType node>
tref rec_term_boole_decomposition(tref term, const trefs& vars, const int_t idx,
	const bool free_funcs = false);

/**
 * @brief Convert a term to Boole normal form.
 *
 * Determines the variable order (with `variable_order_for_simplification`),
 * then applies `rec_term_boole_decomposition`. Also handles non-Boolean terms
 * via `node::ba::simplify_term` and `normalize_ba`.
 * @tparam node Tree node type.
 * @param term The term to normalize.
 * @return Term in Boole normal form.
 */
template <NodeType node>
tref term_boole_decomposition(tref term);

/**
 * @brief Recursively perform Boole decomposition on a wff using atomic formulas as BDD vars.
 *
 * Starting from `vars[idx]` (which are atomic `bf_eq`/`bf_lt`/`bf_lteq` formulas),
 * substitutes `T` and `F` for each and combines via standard Boole expansion,
 * using syntactic path simplification after each substitution.
 * @tparam node Tree node type.
 * @param formula Well-formed formula to decompose.
 * @param vars Ordered list of atomic formulas used as BDD variables.
 * @param idx Starting index in `vars`.
 * @return Decomposed formula.
 */
template <NodeType node>
tref rec_boole_decomposition(tref formula, const trefs& vars, const int_t idx);

/**
 * @brief Convert a formula to Boole Normal Form (full procedure).
 *
 * The full four-step procedure:
 *   1. Syntactic simplification.
 *   2. Squeeze-absorb pre-processing.
 *   3. Term Boole decomposition for all equations.
 *   4. Formula-level Boole decomposition (`rec_boole_decomposition`).
 * Finally applies `to_nnf` and `simplify_using_equality`.
 * @tparam node Tree node type.
 * @param formula Formula to normalize.
 * @return Formula in Boole normal form.
 */
template <NodeType node>
tref boole_normal_form(tref formula);

/**
 * @brief Convert a formula to term-level Boole Normal Form.
 *
 * A lighter variant of `boole_normal_form` that only applies syntactic
 * simplification and term-level Boole decomposition, skipping the
 * formula-level BDD step.
 * @tparam node Tree node type.
 * @param formula Formula to normalize.
 * @return Formula with all terms in Boole normal form.
 */
template <NodeType node>
tref term_boole_normal_form(tref formula);

/**
 * @brief Push an existential quantifier further into a formula using Boole decomposition.
 *
 * Selects the best atomic formula (BDD variable) from the pool, attempts
 * syntactic simplification w.r.t. the quantified variable, and if that fails,
 * constructs the full Boole decomposition step. Updates the pool for subsequent
 * recursive calls.
 * @tparam node Tree node type.
 * @param ex_quant_fm Existentially quantified formula.
 * @param pool Map from formula node to its current pool of atomic sub-formulas.
 * @param quant_pattern Map from quantified variable to its priority index.
 * @param excluded Optional set of nodes to skip in subsequent traversals.
 * @param[out] no_atms Set to `true` if no atomic formula is available for decomposition.
 * @return Formula with the quantifier pushed further in.
 */
template <NodeType node>
tref ex_quantified_boole_decomposition(tref ex_quant_fm, auto& pool,
	auto& quant_pattern, subtree_set<node>* excluded, bool& no_atms);

/**
 * @brief Process a single existentially quantified clause in the anti-prenex algorithm.
 *
 * Applies `ex_quantified_boole_decomposition` repeatedly until the quantifier is
 * eliminated or no further simplification is possible.
 * @tparam node Tree node type.
 * @param ex_clause An existentially quantified formula (a single clause).
 * @param[out] quant_eliminated Set to `true` if the quantifier was removed.
 * @return Simplified (possibly quantifier-free) formula.
 */
template <NodeType node>
tref treat_ex_quantified_clause(tref ex_clause, bool& quant_eliminated);

/**
 * @brief Resolve/eliminate quantifiers in a formula.
 *
 * Pushes quantifiers inward (via `push_quantifiers_in`), then applies Boole
 * decomposition to eliminate remaining existential quantifiers. Handles
 * bitvector formulas by delegating to the CVC5 solver when the formula is closed.
 * @tparam node Tree node type.
 * @param formula Formula containing quantifiers to eliminate.
 * @return Quantifier-free formula, or a formula with as few quantifiers as possible.
 */
template<NodeType node>
tref resolve_quantifiers(tref formula);

/**
 * @brief Resolve quantifiers with a custom variable ordering.
 *
 * Variant of `resolve_quantifiers` that uses the provided `order` relation to
 * sort variables before the elimination step.
 * @tparam node Tree node type.
 * @param formula Formula containing quantifiers.
 * @param order Comparison relation for variable ordering.
 * @return Formula with quantifiers resolved.
 */
template <NodeType node>
tref resolve_quantifiers2(tref formula, const typename term_handle<node>::order& order);

/**
 * @brief Push a block of existential quantifiers into a single clause.
 *
 * Given a clause and a block of existentially quantified variables, applies
 * `treat_ex_quantified_clause` for each variable in the block (ordered by
 * `order`), distributing quantifiers into the clause structure.
 * @tparam node Tree node type.
 * @param clause A single conjunct/disjunct clause.
 * @param block Sequence of existentially quantified variables to push in.
 * @param order Variable ordering relation used to prioritize elimination.
 * @return Clause with the quantifier block resolved.
 */
template<NodeType node>
tref push_ex_block_into_clause(tref clause, const trefs& block,
	const typename term_handle<node>::order& order);

/**
 * @brief Apply the anti-prenex transformation to a formula.
 *
 * Drives the full anti-prenex procedure: converts to NNF, identifies
 * quantifier blocks, and calls `anti_prenex_block` to push quantifiers into
 * the formula structure as deeply as possible.
 * @tparam node Tree node type.
 * @param formula Formula to anti-prenex.
 * @return Formula with quantifiers pushed in as far as possible.
 */
template <NodeType node>
tref anti_prenex(tref formula);

/**
 * @brief Process a quantifier block in the anti-prenex algorithm.
 *
 * Given a formula, a block of existentially quantified variables, the set of
 * atomic formulas already consumed, a `quant_pattern` priority map, and a
 * variable ordering, distributes the block variables into the formula using
 * Boole decomposition.
 * @tparam node Tree node type.
 * @param formula Formula to transform.
 * @param block Variables forming the quantifier block.
 * @param used_atms Set of atomic formulas already used in prior blocks.
 * @param quant_pattern Priority map from variable to quantifier position index.
 * @param order Variable ordering relation.
 * @return Formula with the block processed.
 */
template<NodeType node>
tref anti_prenex_block(tref formula, const trefs& block,
	subtree_unordered_set<node>& used_atms,
	const auto& quant_pattern,
	const typename term_handle<node>::order& order);

/**
 * @brief Entry point for anti-prenex processing of a single quantifier block.
 *
 * Convenience overload that sets up `used_atms`, `quant_pattern`, and `order`
 * internally before calling the full `anti_prenex_block`.
 * @tparam node Tree node type.
 * @param formula Formula with quantifier block to process.
 * @return Formula with the block processed.
 */
template<NodeType node>
tref anti_prenex_block(tref formula);

/**
 * @brief Normalize temporal quantifiers (`always`/`sometimes`) in a formula.
 *
 * Converts the temporal layer of the formula to DNF, then (when
 * `normalize_scopes` is `true`) normalizes the inner formulas below temporal
 * quantifiers and simplifies temporal implications using `is_nso_impl` and
 * `is_non_temp_nso_unsat`.
 * @tparam node Tree node type.
 * @tparam normalize_scopes When `true` (default) also normalize the inner formulas.
 * @param fm Formula to normalize.
 * @return Formula with normalized temporal quantifiers.
 */
template <NodeType node, bool normalize_scopes = true>
tref normalize_temporal_quantifiers(tref fm);

/**
 * @brief Build a split wff node combining `a` and `b` using the given operator type.
 *
 * Utility to construct e.g. `wff_and(a, b)` or `wff_or(a, b)` from a
 * runtime-determined node type.
 * @tparam node Tree node type.
 * @param type The node type representing the binary wff connective.
 * @param a Left sub-formula.
 * @param b Right sub-formula.
 * @return Combined formula node.
 */
template <NodeType node>
tref build_split_wff_using(typename node::type type, tref a, tref b);

/**
 * @brief Convert a formula to Algebraic Normal Form (ANF) for a given type.
 *
 * The ANF is the XOR-AND normal form where each variable appears at most once
 * per monomial. This is used for specific BA types identified by `type`.
 * @tparam node Tree node type.
 * @tparam type BA type identifier.
 * @param n Formula to convert.
 * @return Formula in ANF.
 */
template <NodeType node, size_t type>
tref anf(tref n);

/**
 * @brief Convert a formula to Prenex Normal Form (PNF).
 *
 * Moves all quantifiers to the outermost position, producing a formula of the
 * form `Q1 x1. Q2 x2. ... Qn xn. matrix` where the matrix is quantifier-free.
 * @tparam node Tree node type.
 * @param n Formula to convert.
 * @return Formula in PNF.
 */
template <NodeType node>
tref pnf(tref n);

} // namespace idni::tau_lang

// This is the proper way to include heuristics as the headre mustbe independent
// of the heuristics themselves and also they could need definitions from the
// header (as is the case in 'heuristicsbv_ba_simplification.h'. Also, they
// need to be included before the definitions as they can be used in there.
#include "heuristics/ex_subs_based_elimination.h"
#include "normal_forms.tmpl.h"

#endif // __IDNI__TAU__NORMAL_FORMS_H__
