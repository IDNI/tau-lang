// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "normalizer.h"
#include "normal_forms.h"
#include "definitions.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "normalizer"

namespace idni::tau_lang {

/// Cap on `expand_defs_until_settled` passes; 0 = unlimited (the default).
/// A real definition set settles in a handful of passes — each pass unfolds
/// every applicable definition at every position at once — and oscillation is
/// caught by a visited set regardless, so only an ever-growing expansion is
/// unbounded when unlimited. Runtime parameter by policy: set via
/// `--max-def-passes`, REPL `defpasses`, or `api::set_max_def_passes`.
inline size_t max_def_passes = 0;

/// Cap on recurrence-relation enumeration steps; 0 = unlimited (the default).
/// A recurrence whose normalized steps are all distinct (e.g. one that keeps
/// growing) iterates forever when unlimited, and each iteration runs a full
/// normalization plus up to `previous.size()` equivalence proofs; real
/// recurrences settle in single-digit steps. Set via `--max-enum-steps`,
/// REPL `enumsteps`, or `api::set_max_enum_steps`.
inline size_t max_enum_steps = 0;

/**
 * @internal
 * @brief Descriptor of a single reference offset.
 *
 * The first element identifies the offset kind (one of `tau_parser::nonterminal`
 * values: `num`, `capture`, `shift`, or `variable`). The second element holds
 * the numeric value for `num`, the `rr_dict` identifier for `capture`, or 0 for
 * `shift` and `variable`.
 * @endinternal
 */
using offset_t = std::pair<tau_parser::nonterminal, size_t>;

/**
 * @internal
 * @brief Lift conjuncts that do not mention a quantified variable out of that
 * variable's scope: `Q x (A && B)` becomes `A && Q x B` whenever `x` is not
 * free in `A`. Sound for both quantifier kinds, and dropping a binder whose
 * scope no longer mentions it is sound under the standing non-empty-domain
 * assumption `process_quantifier_blocks` already relies on.
 *
 * Bit-blasting rewrites bv arithmetic in place, so a mixed-type conjunction
 * such as `ex x ex y (x + y = 0 && s = 0)` (with `s` atomless) keeps the
 * foreign conjunct `s = 0` inside the bv quantifiers' scope. That single
 * conjunct is enough to make `is_bv_solvable_formula` reject the whole scope,
 * so neither the solver nor `resolve_quantifiers2` can decide it, and the
 * blasted bits fall through to generic Boole decomposition instead --
 * hundreds of bv-typed atoms, each split copying the entire formula, with
 * every BDD node operation on a bv leaf allocating solver terms. Scoping the
 * foreign conjuncts out first leaves a closed, purely bitvector scope that
 * the solver settles directly.
 *
 * Applied bottom-up so an inner lift exposes the next one further out.
 * @tparam node Tree node type.
 * @param fm Formula to rewrite.
 * @return Formula with independent conjuncts scoped out of every quantifier.
 * @endinternal
 */
template <NodeType node>
tref scope_out_independent_conjuncts(tref fm) {
	using tau = tree<node>;
	auto lift = [](tref n) -> tref {
		if (!is_child_quantifier<node>(n)) return n;
		tref var = tau::trim2(n);
		tref body = tau::get(n)[0].second();
		// A binder whose scope never mentions it contributes nothing, so
		// drop it outright -- the same non-empty-domain reasoning as the
		// `dep.empty()` case below, just without requiring a conjunction.
		// Worth doing here because a skip-matched block is re-wrapped
		// verbatim by process_quantifier_block's wrap_skipped, so a bv
		// binder left vacuous by earlier simplification would otherwise
		// survive into the output.
		// `var` comes from trim2 and still has its right sibling, while
		// get_free_vars stores trim_right_sibling'ed nodes. Searching one
		// against the other is safe because subtree_less bottoms out in
		// lcrs_tree::operator<, which compares value and left child only
		// and ignores the right sibling.
		if (!hasbc(get_free_vars<node>(body), var, tau::subtree_less))
			return body;
		if (!tau::get(body).child_is(tau::wff_and)) return n;
		trefs dep, indep;
		for (tref c : get_cnf_wff_clauses<node>(body)) {
			if (hasbc(get_free_vars<node>(c), var, tau::subtree_less))
				dep.push_back(c);
			else indep.push_back(c);
		}
		if (indep.empty()) return n;
		tref kept = tau::build_wff_and(indep);
		// Unreachable: a body in which no conjunct mentions `var` was
		// already returned by the vacuous-binder check above.
		DBG(assert(!dep.empty());)
		if (dep.empty()) return kept;
		return tau::build_wff_and(kept, is_child<node>(n, tau::wff_ex)
			? build_wff_ex<node>(var, tau::build_wff_and(dep), false)
			: build_wff_all<node>(var, tau::build_wff_and(dep), false));
	};
	// Do not descend into terms: tau_ba sub-trees carry their own wff_ex/wff_all
	// over I/O variables, which this pass must leave intact -- the same guard
	// select_innermost_blocks and anti_prenex_block use, and for the same
	// reason. Without it `is_child_quantifier` matches those internal binders
	// and both the vacuous-binder drop and the conjunct lift rebuild the
	// enclosing term, which additionally loses inference-assigned bitwidths on
	// bv-containing scopes.
	return post_order<node>(fm).apply_unique(lift, while_is_formula<node>);
}

/**
 * @internal
 * @brief Push/eliminate all quantifiers in `form`, resolving bitvector
 * content along the way.
 *
 * Sequence: resolve any closed bv (sub-)formula via the solver first
 * (pushing a blasted bitvector formula's quantifiers through Boolean
 * normalization before it is closed is exponential, while the solver
 * decides the closed formula directly); push/eliminate the rest via
 * `anti_prenex_block` (which, for a block containing bv-typed variables,
 * attempts predicate blasting on the isolated clause itself); resolve
 * again; then, since blasting can leave genuinely unsupported bv
 * arithmetic (e.g. multiplication/division by a non-constant) behind,
 * collect that residue and run `anti_prenex_block` once more skipping
 * exactly it, so any quantifier not touching surviving arithmetic still
 * gets pushed/resolved; a final resolve pass collapses whatever closed bv
 * (sub-)formula that step produces.
 * @tparam node Tree node type.
 * @param form Formula to process.
 * @return Formula with quantifiers pushed/eliminated as far as possible.
 *
 * @par Example
 * @code{.cpp}
 * // ex x (x|y = 0): x is eliminated entirely, leaving just y = 0
 * tref fm = get_nso_rr("ex x x|y = 0.").value().main->get();
 * tref res = eliminate_bv_and_quantifiers<node_t>(fm);
 * // tau::get(res).to_str() == "y = 0"
 * CHECK( !tau::get(res).find_top(is<node_t, tau::wff_ex>) );
 * @endcode
 * @endinternal
 */
template <NodeType node>
tref eliminate_bv_and_quantifiers(tref form) {
	using tau = tree<node>;

	// Before anything blasts or decomposes: a foreign-typed sibling conjunct
	// inside a bitvector quantifier's scope makes the whole scope fail
	// `is_bv_solvable_formula`, so the solver shortcut below is skipped and
	// the scope gets blasted instead -- and eliminating the auxiliary bit
	// variables blasting introduces is itself exponential in the bitwidth.
	// Scoping those conjuncts out first leaves a closed, purely bitvector
	// scope the solver decides directly, keeping the common mixed-type case
	// off the blasting path entirely.
	form = scope_out_independent_conjuncts<node>(form);
	form = resolve_quantifiers<node>(form);
	// Mark variables used as an argument of an unresolved predicate
	// reference (`wff_ref`), or entangled with one through a shared atom,
	// so they are not Boole-decomposed.
	//
	// NOTE: this is currently a no-op in every case measured. The block
	// machinery already refuses to eliminate across an unresolved
	// reference at *conjunct* granularity -- eliminate_block_over_clause's
	// is_quant_removable_in_clause and treat_ex_quantified_clause's
	// blocks_elimination both test individual conjuncts, independent
	// conjuncts are split out before either applies, and
	// ex_subs_based_elimination still substitutes a unique witness. So
	// `ex y ex z (q(y) && z = 0)` already eliminates z and keeps y
	// without this skip, and all five probe formulas normalize
	// identically with and without it. It is retained as a guard for
	// shapes the conjunct-level checks might miss; if it never earns its
	// keep, drop it and the two traversals it costs per pass.
	//
	// Recomputed before each pass: the set is keyed on tref nodes of the
	// tree being scanned, and `form` is rebuilt in between.
	//
	// The two passes are NOT collapsible into one, measured 2026-08-04. The
	// redesign plan's Task 9 proposed replacing them with a single call, on
	// the reading that the second pass's skip subsumes the first's and that
	// the per-block eliminability analysis has made the staging redundant.
	// Both single-pass variants fail `test_integration-interpreter`'s
	// "nested conditionals over mixed tau/bv streams stay sat" -- the issue
	// #70 regression test -- in both configurations: keeping the second
	// pass's conditional bv skip reports "Tau specification is unsat", and
	// keeping the first pass's unconditional one SIGSEGVs. What the second
	// pass depends on is not the first pass's *verdicts* but its having
	// already run: it works on a formula whose non-bv structure is resolved.
	//
	// The analysis cannot substitute for that today, and the plan's premise
	// that it could does not hold as built: `blasteable` is consumed only
	// inside `eliminate_block_over_clause`, whereas the decisions that
	// matter here -- which quantifiers `collect_quantifier_block` treats as
	// transparent, and what `blast_block` hands to the solver -- still read
	// only the boolean `el.skip(n)`, not the verdict behind it. Collapsing
	// needs those rewired to the verdicts first (the plan's own Task 9
	// step 2), not merely one call deleted.
	{
		analysis_context<node> ctx1;          // bv_is_solver_owned = true
		const eliminability<node> el1 = analyse_formula<node>(form, ctx1);
		form = anti_prenex<node>(form, el1);
	}
	form = resolve_quantifiers<node>(form);
	// Pass 2: bv floor only where the solver could own the content; the
	// arithmetic/blasteable seeds still hold unconditionally.
	//
	// bv-typed content is skipped here as well, not just the arithmetic
	// residue the `arithmetic` verdict marks. Blasting rewrites arithmetic
	// into per-bit equality/comparison atoms that are still bv-typed but no
	// longer arithmetic-tainted, so that verdict stops applying to them and
	// they became eligible for generic Boole decomposition -- hundreds of
	// atoms per blasted operation, each split copying the whole formula, and
	// every BDD node operation on a bv leaf allocating cvc5 terms (bv BDD
	// leaves are solver-term-backed, so this is never the cheap path
	// atomless content enjoys). Skipping them leaves the quantifier in place
	// instead, which is sound; whatever is closeable has already been
	// decided by the solver via scope_out_independent_conjuncts and the
	// resolve passes above, and a genuinely open bv scope (e.g.
	// `ex x (x + y = 0)` with `y` free) could not be reduced by decomposing
	// it anyway.
	//
	// The completeness this gives up is bounded and pinned. "Already decided
	// by the solver" holds only for a scope `is_bv_solvable_formula` accepts;
	// a *closed* scope it rejects (bv arithmetic plus an unresolved wff_ref,
	// say) is neither decided here nor decomposable afterwards, so it comes
	// back with its quantifier intact. That is the intended outcome, not an
	// oversight: it is what `is_non_temp_nso_*`'s check_decided fallback
	// reports rather than asserts, and it is pinned by
	// "undecidable closed bv scope keeps its quantifier"
	// (test_integration-wff_normalization.cpp) together with the
	// UndecidableNormalizationFallback suite (test_normal_forms.cpp).
	//
	// ...but "intended outcome" only holds where the caller can live with an
	// undecided formula. `interpreter::step` cannot: a surviving quantifier
	// leaves its step system unsolvable and the run reports "Tau
	// specification is unexpectedly unsat". So the blanket bv skip is applied
	// only where its own justification above holds -- where the solver could
	// own this bv content. A formula carrying a constant of another Boolean
	// algebra (a `:tau` spec constant, say, as every `run` over mixed `:tau`
	// and `:bv[N]` streams produces) is one cvc5 cannot translate at all, so
	// neither the resolve passes nor blasting will ever decide its bv scopes;
	// skipping them there strands the quantifier for good. Boole decomposition
	// is the only route left, so let it have them -- the `arithmetic` verdict
	// still keeps genuinely unsupported bv arithmetic out of it, and the atom
	// counts in a mixed formula are the spec's own, not blasting's per-bit
	// residue.
	analysis_context<node> ctx2;
	ctx2.bv_is_solver_owned = !has_foreign_ba_constant<node>(form);
	const eliminability<node> el2 = analyse_formula<node>(form, ctx2);
	form = anti_prenex<node>(form, el2);
	form = resolve_quantifiers<node>(form);
	// Option 5a -- the per-formula blasting destination: one attempt on the
	// whole formula, after the last anti-prenex/resolve pass and before the
	// final closed-formula check below. Inert at the shipped default
	// (`blast_placement == per_leaf`).
	//
	// The final check itself is deliberately NOT gated on
	// `solver_placement`: it is the single "final" solver site that both
	// `per_closed_block` and `per_formula` rely on, so it runs under every
	// setting.
	if (bv_blasting && blast_placement == blast_site::per_formula)
		if (tref blasted = bv_predicate_blasting<node>(form);
			blasted && blasted != form)
			form = blast_method == blast_mode::anti_prenex_result
				? anti_prenex<node>(blasted,
					eliminability<node>::bv_only())
				: blasted;
	if (get_free_vars<node>(form).empty() && is_bv_solvable_formula<node>(form)) {
		// Only commit to T/F on a definite answer: cvc5
		// returning unknown, or translation failing, means
		// we cannot decide, not that the formula is false.
		auto status = bv_formula_sat_status<node>(form);
		if (status == bv_sat_status::sat) return tau::_T();
		if (status == bv_sat_status::unsat) return tau::_F();
	}
	return form;
}

// IDEA (HIGH) rewrite steps as a tuple to optimize the execution
/** @internal @copydoc normalize @endinternal */
template <NodeType node>
tref normalize(tref form) {
	using tau = tree<node>;
	// Caching architecture (see private/2026-08-15-normalizer-caching-plan.md,
	// "Explicitly NOT cacheable as-is", for the full rationale):
	// This entry cache (and normalize_non_temp's below) dedupes whole-formula
	// calls -- measured 37/37 distinct on the probe case, i.e. no repetition
	// at this level. The real repetition lives in the leaf passes: to_nnf
	// (16x), normalize_atomic_formula_operators (16x),
	// syntactic_path_simplification (66x) and ex_subs_based_elimination
	// (44,795 calls on the probe case) -- those now carry their own
	// TAU_CACHE-gated caches. anti_prenex_block / anti_prenex(el) /
	// process_quantifier_blocks stay deliberately uncached: their key would
	// have to include per-pass eliminability state and in/out recursion
	// state, which would never hit.
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(form); it != cache.end()) return it->second;
#endif // TAU_CACHE
	// First resolve quantifiers in formulas below temporal quantifiers
	trefs temps = tau::get(form).select_top(is_child_temporal_quantifier<node>);
	// Case that the formula has no temporal quantifier
	if (temps.empty()) {
		form = eliminate_bv_and_quantifiers<node>(form);
	} else {
		subtree_map<node, tref> changes;
		for (tref temp : temps) {
			bool is_aw = is_child<node>(temp, tau::wff_always);
			// Remove temporal quantifier
			tref f = tau::trim2(temp);
			f = eliminate_bv_and_quantifiers<node>(f);
			// Add quantifier again and save as change
			if (is_aw) changes.emplace(temp, tau::build_wff_always(f));
			else changes.emplace(temp, tau::build_wff_sometimes(f));
		}
		form =  rewriter::replace(form, changes);
	}
	// Now normalize the temporal layer and convert the formulas below the temporal
	// quantifiers to Boole normal form
	tref result = normalize_temporal_quantifiers<node>(form);
#ifdef TAU_CACHE
	cache.emplace(form, result);
#endif // TAU_CACHE
	return result;
}

// Assumes that the formula passed does not have temporal quantifiers
// This normalization will non perform the temporal normalization
/** @internal @copydoc normalize_non_temp @endinternal */
template <NodeType node>
tref normalize_non_temp(tref fm) {
	//	using tt = tau::traverser;
	// See normalize's cache comment above for the caching architecture
	// (entry vs. leaf-pass caches, and why anti_prenex_block/anti_prenex(el)
	// stay uncached).
	#ifdef TAU_CACHE
	using tau = tree<node>;
	using cache_t = subtree_unordered_map<node, tref>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(fm); it != cache.end()) return it->second;
#endif // TAU_CACHE
	tref result = eliminate_bv_and_quantifiers<node>(fm);
	result = term_boole_normal_form<node>(result);
	// NOTE: Do NOT add `tau::reget` here. It strips the explicit bitwidth
	// subtype from BV-typed nodes (io_vars and BV constants), causing
	// get_bv_size assertions downstream. `fold_trivial_quantifiers` is a
	// different matter and IS wanted: it rebuilds only the ancestors of the
	// nodes it folds, not every node, so it does not have that effect.
	//
	// It has to run here, not only in normalize_with_temp_simp. Every
	// `is_non_temp_nso_*` / `are_nso_equivalent` predicate reads THIS
	// function's result as T, F or "undecided" via check_decided, and that
	// happens well before normalize_with_temp_simp's fold. A residual
	// `all b2, b1 T` -- which the resolve passes above can leave behind when
	// a scope folds to a constant after its quantifier prefix was already
	// re-attached -- was therefore being reported as a formula normalization
	// could not decide, and `are_nso_equivalent` answered negatively on a
	// formula that is plainly T. Pinned by
	// "a term containing a bf_ref still normalizes"
	// (test_integration-normalizer_helpers.cpp).
	result = fold_trivial_quantifiers<node>(result);
#ifdef TAU_CACHE
	cache.emplace(fm, result);
#endif // TAU_CACHE
	return result;
}

/**
 * @internal
 * @brief Collects all variable and capture nodes from tree node @p n using `select_top`.
 * @tparam node Tree node type.
 * @param n The root node to search within.
 * @return A flat list of all variable and capture sub-nodes found.
 * @endinternal
 */
template <NodeType node>
trefs get_vars_from_nso(tref n) {
	return tree<node>::get(n).select_top(is_var_or_capture<node>());
}

// Given a tref produce a number i such that the uninterpreted constant const_i is
// not present
/** @internal @copydoc get_new_uninterpreted_constant @endinternal */
template <NodeType node>
tref get_new_uninterpreted_constant(tref fm, const std::string& name, size_t type) {
	using tau = tree<node>;
	trefs uninter_consts
		= tau::get(fm).select_top(is<node, tau::uconst_name>);
	auto is_number = [](const std::string& s) static {
		if (s.empty()) return false;
		for (const unsigned char c : s) if (!std::isdigit(c)) return false;
		return true;
	};
	// build_bf_uconst("", name + id, type) stores the name as ":" + name + id
	// (see build_bf_uconst); only uconsts matching that exact prefix followed
	// by a numeric suffix belong to this family and should be numbered.
	const std::string prefix = ":" + name;
	std::set ids{ 0 };
	for (tref uninter_const : uninter_consts) {
		const auto& tmp = tau::get(uninter_const).get_string();
		if (tmp.size() <= prefix.size()
			|| tmp.compare(0, prefix.size(), prefix) != 0) continue;
		std::string id = tmp.substr(prefix.size());
		if (is_number(id)) ids.insert(std::stoi(id));
	}
	std::string id = std::to_string(*ids.rbegin() + 1);
	tref uninter_const = tau::build_bf_uconst("", name + id, type);
	return uninter_const;
}

/**
 * @internal
 * @brief Extracts reference metadata from a `ref` node.
 *
 *  Returns a pair of `rr_sig` (name id and arity) and a vector of `offset_t`
 *  values. Currently only the first offset is considered.
 * @tparam node Tree node type.
 * @param ref The `ref` tree node to extract information from.
 * @return A pair of the recurrence relation signature and its offset list.
 *
 * @par Example
 * @code{.cpp}
 * // "h(Y)" has no bracketed offset and one argument
 * auto nso_rr = get_bf_nso_rr("h(X):tau := 1.", "h(Y)").value();
 * tref ref = get_ref<node_t>(nso_rr.main->get());
 * auto [sig, offsets] = get_ref_info<node_t>(ref);
 * // offsets.size() == 0, sig.arg_arity == 1
 * @endcode
 * @endinternal
 */
// extracts ref info. returns pair of rr_sig (name id and arity)
// and vector of its offsets (offset_t)
template <NodeType node>
std::pair<rr_sig, std::vector<offset_t>> get_ref_info(tref ref) {
	using tau = tree<node>;
	using tt = tau::traverser;
	LOG_TRACE << "get_ref_info: " << LOG_FM_DUMP(ref);
	std::pair<rr_sig, std::vector<offset_t>>
					ret{ get_rr_sig<node>(ref), {} };
	auto offsets = tt(ref) | tau::offsets || tau::offset;
	LOG_TRACE << "get ref " << LOG_RR_SIG(ret.first) << " offsets.size: " << offsets.size();
	for (auto offset : offsets()) {
		const auto& t = offset[0];
		int_t d = 0;
		if (t.is_integer()) d = t.get_integer();
		else if (t.get_type() == tau::capture) d = t.data();
		ret.second.emplace_back(t.get_type(), d);
		break; // consider only first offset for now
		// TODO (LOW) support multiindex offsets
		// need to find a canonical way of enumeration first
	}
	return ret;
};

/**
 * @internal
 * @brief Traverses @p n downward through single-child nodes to find a `ref` node.
 * @tparam node Tree node type.
 * @param n The node to start the search from.
 * @return The first `ref` node found, or `nullptr` if none exists.
 *
 * @par Example
 * @code{.cpp}
 * auto nso_rr = get_bf_nso_rr("h(X):tau := 1.", "h(Y)").value();
 * tref ref = get_ref<node_t>(nso_rr.main->get());
 * // tau::get(ref).to_str() == "h(Y)"
 *
 * tref fm_no_ref = get_nso_rr("x = 0.").value().main->get();
 * CHECK( get_ref<node_t>(fm_no_ref) == nullptr );
 * @endcode
 * @endinternal
 */
template <NodeType node>
tref get_ref(tref n) {
	using tau = tree<node>;
	using tt = tau::traverser;
	auto ref = tt(n);
	while (ref && !ref.is(tau::ref)) ref = ref | tt::only_child;
	if (!ref || !ref.is(tau::ref)) return nullptr;
	return ref.value();
}

// Check that the Tau formula does not use Boolean combinations of models
/** @internal @copydoc has_no_boolean_combs_of_models @endinternal */
template <NodeType node>
bool has_no_boolean_combs_of_models(tref n) {
	using tau = tree<node>;
	const auto& fm = tau::get(n);
	if (is<node>(fm.first(), tau::wff_always)) {
		// check that there is no wff_always or wff_sometimes in the subtree
		if (fm[0][0].find_top(is<node, tau::wff_always>))
			return false;
		if (fm[0][0].find_top(is<node, tau::wff_sometimes>))
			return false;
	} else {
		if (fm.find_top(is<node, tau::wff_always>))
			return false;
		if (fm.find_top(is<node, tau::wff_sometimes>))
			return false;
	}
	return true;
}

/**
 * @internal
 * @brief Report a formula that `normalize_non_temp` could not decide.
 *
 * The predicates below all normalize a closed formula and read the result as
 * `T` or `F`. That is not guaranteed: a closed bitvector scope the solver cannot
 * settle -- cvc5 answering `unknown`, or a translation failure such as an
 * unresolved `wff_ref` inside bv arithmetic -- comes back with its quantifier
 * intact, and `is_bv_solvable_formula` does not reject it because it inspects
 * only `variable` nodes. Asserting decidability here aborted Debug builds on a
 * user-reachable input; the predicates now fall back to their negative answer,
 * which is the conservative direction for every current caller
 * (`api::is_valid` reports "not valid", `simplify_temporal_clause` declines to
 * eliminate a part, `find_fixpoint_phi`/`chi` keep unrolling until their step
 * cap) -- and say so loudly instead of silently.
 *
 * The proper fix is a tri-state (`true`/`false`/`undecided`) contract threaded
 * through these predicates and their callers; until then this at least makes the
 * case diagnosable.
 *
 * One shape among the undecided ones is not a gap to chase: a temporal
 * operator (`always`, `sometimes`, ...) directly inside a quantifier scope
 * (e.g. `all b (always b != c)`, NZ-1) is undecidable by this project's
 * quantifier-elimination machinery on principle, not by omission --
 * substituting a time-invariant constant for `b` says nothing about a scope
 * whose truth varies over time, so no case-split on `b` alone can ever
 * settle it (see `complete_quantifier_elimination`'s doc comment, and
 * the pinned `UndecidableNormalizationFallback` / `wff_normalization`
 * tests). That shape is reported at `LOG_WARNING`, not `LOG_ERROR`, so it
 * does not read as a regression; every other undecided shape keeps
 * `LOG_ERROR` so a real one still surfaces loudly.
 * @tparam node Tree node type.
 * @param who Name of the calling predicate, for the log line.
 * @param normalized The normalized formula to check.
 * @return `true` if the formula was decided (`T`, `F`, or a constraint).
 * @endinternal
 */
template <NodeType node>
bool check_decided(const char* who, tref normalized) {
	using tau = tree<node>;
	const auto& t = tau::get(normalized);
	if (t.equals_T() || t.equals_F()
		|| t.find_top(is<node, tau::constraint>)) return true;
	// NZ-1: a quantifier whose scope still holds a temporal operator.
	auto is_temporal_under_quantifier = [](tref m) {
		return is_child_quantifier<node>(m)
			&& tree<node>::get(tree<node>::get(m)[0].second())
				.find_top(is_temporal_quantifier<node>);
	};
	if (t.find_top(is_temporal_under_quantifier)) {
		LOG_WARNING << who << ": normalization could not decide "
			<< LOG_FM(normalized) << "; answering negatively. This is a "
			"conservative fallback, not a proof -- a temporal operator "
			"directly inside a quantifier scope (NZ-1) is undecidable by "
			"this pipeline.";
		return false;
	}
	LOG_ERROR << who << ": normalization could not decide "
		<< LOG_FM(normalized) << "; answering negatively. This is a "
		"conservative fallback, not a proof.";
	return false;
}

/** @internal @copydoc is_non_temp_nso_satisfiable @endinternal */
template <NodeType node>
bool is_non_temp_nso_satisfiable(tref n) {
	using tau = tree<node>;

	DBG(assert(n != nullptr));

	const auto& fm = tau::get(n);
	DBG(assert(!fm.find_top(is<node, tau::wff_always>));)
	DBG(assert(!fm.find_top(is<node, tau::wff_sometimes>));)
	tref nn = n;
	const trefs& vars = fm.get_free_vars();
	nn = tau::build_wff_ex_many(vars, nn);
	tref normalized = normalize_non_temp<node>(nn);
	const auto& t = tau::get(normalized);

	DBG(LOG_TRACE << "is_non_temp_nso_satisfiable/normalized: "
		  << LOG_FM(normalized);)

	check_decided<node>("is_non_temp_nso_satisfiable", normalized);

	return t.equals_T();
}

/**
 * @internal
 * @brief Checks whether a non-temporal NSO formula is unsatisfiable.
 *
 *  Wraps free variables with existential quantifiers, normalizes via
 *  `normalize_non_temp`, and returns `true` if the result is `F`.
 * @tparam node Tree node type.
 * @param n The non-temporal formula to test.
 * @return `true` if the formula is unsatisfiable, `false` otherwise.
 *
 * @par Example
 * @code{.cpp}
 * tref fm = get_nso_rr("x = 0 && x != 0.").value().main->get();
 * CHECK( is_non_temp_nso_unsat<node_t>(fm) );
 * @endcode
 * @endinternal
 */
template <NodeType node>
bool is_non_temp_nso_unsat(tref n) {
	using tau = tree<node>;
	DBG(assert(n != nullptr));
	DBG(assert(!tau::get(n).find_top(is<node, tau::wff_always>));)
	DBG(assert(!tau::get(n).find_top(is<node, tau::wff_sometimes>));)

	tref nn = n;
	const trefs& vars = get_free_vars<node>(nn);
	nn = tau::build_wff_ex_many(vars, nn);
	tref normalized = normalize_non_temp<node>(nn);
	const auto& t = tau::get(normalized);
	check_decided<node>("is_non_temp_nso_unsat", normalized);
	return t.equals_F();
}

/** @internal @copydoc are_nso_equivalent @endinternal */
template <NodeType node>
bool are_nso_equivalent(tref n1, tref n2) {
	using tau = tree<node>;
	LOG_DEBUG << "Begin are_nso_equivalent";
	LOG_TRACE << "-- n1: " << LOG_FM(n1);
	LOG_TRACE << "-- n2: " << LOG_FM(n2);

	// If this method is called on a formula that has Boolean combinations of models, it is used incorrectly
	DBG(assert((has_no_boolean_combs_of_models<node>(n1)
		&& has_no_boolean_combs_of_models<node>(n2)));)

	const auto& t1 = tau::get(n1);
	const auto& t2 = tau::get(n2);
	if (t1[0].is(tau::wff_always)) n1 = t1[0].first();
	if (t2[0].is(tau::wff_always)) n2 = t2[0].first();

	if (tau::get(n1) == tau::get(n2)) {
		LOG_DEBUG << "-- End are_nso_equivalent: true (equiv nodes)";
		return true;
	}

	// equivalence of references
	tref r1opt = get_ref<node>(n1), r2opt = get_ref<node>(n2);
	if (r1opt != nullptr && r2opt != nullptr) { // both are refs
		bool equiv = get_ref_info<node>(r1opt)
						== get_ref_info<node>(r2opt);
		LOG_DEBUG << "End are_nso_equivalent: " << equiv
							<< " (equiv refs)";
		return equiv;
	}
	else if (r1opt != nullptr || r2opt != nullptr) { // one is a ref
		LOG_DEBUG << "End are_nso_equivalent: false (ref and not ref)";
		return false;
	}

	tref imp1 = tau::build_wff_imply(n1, n2);
	tref imp2 = tau::build_wff_imply(n2, n1);
	const trefs& vars = get_free_vars<node>(tau::build_wff_and(n1, n2));
	DBG(for (tref v : vars) LOG_DEBUG << "var: " << LOG_FM(v);)
	imp1 = tau::build_wff_all_many(vars, imp1);
	imp2 = tau::build_wff_all_many(vars, imp2);

	LOG_DEBUG << "wff: " << LOG_FM(tau::build_wff_and(imp1, imp2));

	tref ndir1 = normalize_non_temp<node>(imp1);
	const tau& tdir1 = tau::get(ndir1);
	check_decided<node>("are_nso_equivalent", ndir1);
	if (tdir1.equals_F()) {
		LOG_DEBUG << "End are_nso_equivalent: " << LOG_FM(tdir1.get());
		return false;
	}
	tref ndir2 = normalize_non_temp<node>(imp2);
	const tau& tdir2 = tau::get(ndir2);
	check_decided<node>("are_nso_equivalent", ndir2);
	const bool res = (tdir1.equals_T() && tdir2.equals_T());
	LOG_DEBUG << "End are_nso_equivalent: " << res;
	return res;
}

/**
 * @internal
 * @brief Returns `true` if @p n is logically equivalent to any formula in @p previous.
 *
 *  Equivalence is checked via `are_nso_equivalent`.
 * @tparam node Tree node type.
 * @param n The formula to test.
 * @param previous The list of candidate formulas to compare against.
 * @return `true` if @p n is equivalent to at least one formula in @p previous.
 *
 * @par Example
 * @code{.cpp}
 * tref n = get_nso_rr("x = 0.").value().main->get();
 * trefs previous {
 *     get_nso_rr("y = 0.").value().main->get(),
 *     get_nso_rr("!(x != 0).").value().main->get()
 * };
 * CHECK( is_nso_equivalent_to_any_of<node_t>(n, previous) ); // matches !(x!=0)
 * @endcode
 * @endinternal
 */
template <NodeType node>
bool is_nso_equivalent_to_any_of(tref n, trefs& previous) {
	return std::any_of(previous.begin(), previous.end(), [n] (tref& p) {
			return are_nso_equivalent<node>(n, p);
		});
}

/** @internal @copydoc is_nso_impl @endinternal */
template <NodeType node>
bool is_nso_impl(tref n1, tref n2) {
	using tau = tree<node>;

	LOG_DEBUG << "Begin is_nso_impl";
	LOG_TRACE << "n1 " << LOG_FM(n1);
	LOG_TRACE << "n2 " << LOG_FM(n2);
	// If this method is called on a formula that has Boolean combinations of models, it is used incorrectly
	DBG(assert((has_no_boolean_combs_of_models<node>(n1)
		 && has_no_boolean_combs_of_models<node>(n2)));)

	const auto& t1 = tau::get(n1);
	const auto& t2 = tau::get(n2);
	if (t1[0].is(tau::wff_always)) n1 = t1[0].first();
	if (t2[0].is(tau::wff_always)) n2 = t2[0].first();

	if (tau::get(n1) == tau::get(n2)) {
		LOG_DEBUG << "End is_nso_impl: true (n1 implies n2)";
		return true;
	}

	// Decides `all vars (f -> g)` by closing over f's and g's free
	// variables and normalizing.
	auto closed_impl_holds = [](tref f, tref g) {
		tref imp = tau::build_wff_imply(f, g);
		const trefs& vars = get_free_vars<node>(imp);
		imp = tau::build_wff_all_many(vars, imp);
		LOG_DEBUG << "wff: " << LOG_FM(imp);
		tref nres = normalize_non_temp<node>(imp);
		check_decided<node>("is_nso_impl", nres);
		return tau::get(nres).equals_T();
	};

	// GitHub #82: decide the implication per connected component rather
	// than closing the whole `n1 -> n2` over every free variable in one
	// piece. `find_fixpoint_phi`'s iterates are top-level conjunctions
	// whose atoms fall into many variable-disjoint components (one per
	// accumulated clause), and one monolithic normalization of them is
	// exponential in the component count. Group the conjuncts of BOTH
	// sides together by shared variables; with P = /\ P_c and Q = /\ Q_c
	// over the resulting components c (variable-disjoint by
	// construction), exactly:
	//
	//   all V (P -> Q)  iff  for every c: P && !Q_c is unsat,
	//   P && !Q_c unsat  iff  (P_c && !Q_c) unsat  or  R_c unsat,
	//
	// where R_c = /\ of the other components' antecedent parts. The first
	// disjunct is `all (P_c -> Q_c)` on a component-sized formula; the
	// second only matters when the first fails, and R_c is unsatisfiable
	// iff one of those other components is, so they are checked one at a
	// time and only on demand. When everything is connected this is
	// exactly the single check it replaces (one component), so it costs
	// nothing on specs whose state is all one component -- grouping per
	// consequent conjunct instead would re-decide that one big antecedent
	// once per conjunct. Consequent conjuncts that are literally
	// antecedent conjuncts are implied syntactically and dropped first.
	const trefs ante = get_cnf_wff_clauses<node>(n1);
	trefs cons;
	{
		subtree_unordered_set<node> ante_set(ante.begin(), ante.end());
		for (tref q : get_cnf_wff_clauses<node>(n2))
			if (!tau::get(q).equals_T() && !ante_set.contains(q))
				cons.push_back(q);
	}
	if (cons.empty()) {
		LOG_DEBUG << "End is_nso_impl: true (consequent is syntactically"
			" contained in the antecedent)";
		return true;
	}
	if (ante.size() + cons.size() <= 2) {
		const bool holds = closed_impl_holds(n1, n2);
		LOG_DEBUG << "End is_nso_impl: " << holds;
		return holds;
	}
	// Antecedent conjuncts first, so each group's members split into a
	// leading antecedent part and a trailing consequent part.
	trefs both(ante);
	both.insert(both.end(), cons.begin(), cons.end());
	subtree_unordered_set<node> cons_set(cons.begin(), cons.end());
	const trefs& all_vars =
		get_free_vars<node>(tau::build_wff_imply(n1, n2));
	const std::vector<trefs> groups =
		group_by_shared_vars<node>(both, all_vars);
	std::vector<trefs> group_ante(groups.size()), group_cons(groups.size());
	for (size_t gi = 0; gi < groups.size(); ++gi)
		for (tref c : groups[gi])
			(cons_set.contains(c) ? group_cons : group_ante)[gi]
				.push_back(c);
	auto ante_of = [&](size_t gi) -> tref {
		return group_ante[gi].empty() ? tau::_T()
			: tau::build_wff_and(group_ante[gi]);
	};
	// Lazily decided unsatisfiability of each group's antecedent part.
	std::vector<std::optional<bool>> group_unsat(groups.size());
	auto is_group_unsat = [&](size_t gi) -> bool {
		if (group_ante[gi].empty()) return false;
		if (!group_unsat[gi]) group_unsat[gi] =
			closed_impl_holds(ante_of(gi), tau::_F());
		return *group_unsat[gi];
	};
	for (size_t gi = 0; gi < groups.size(); ++gi) {
		if (group_cons[gi].empty()) continue;
		if (closed_impl_holds(ante_of(gi),
			tau::build_wff_and(group_cons[gi]))) continue;
		// Not implied within its component: the implication still
		// holds if some other component's antecedent is unsatisfiable.
		for (size_t gj = 0; gj < groups.size(); ++gj)
			if (gj != gi && is_group_unsat(gj)) {
				LOG_DEBUG << "End is_nso_impl: true (antecedent"
					" unsatisfiable)";
				return true;
			}
		LOG_DEBUG << "End is_nso_impl: false (component not implied: "
			<< LOG_FM(tau::build_wff_and(group_cons[gi])) << ")";
		return false;
	}
	LOG_DEBUG << "End is_nso_impl: true";
	return true;
}

/**
 * @internal
 * @brief Checks whether two Boolean functions @p n1 and @p n2 are equal.
 *
 *  Builds `(n1 XOR n2) = 0`, universally quantifies all free variables,
 *  normalizes, and returns `true` if the result is `T`.
 * @tparam node Tree node type.
 * @param n1 The first Boolean function.
 * @param n2 The second Boolean function.
 * @return `true` if @p n1 and @p n2 are semantically equal.
 *
 * @par Example
 * @code{.cpp}
 * auto pbf = parse_bf();
 * tref n1 = tau::get("X", pbf);
 * tref n2 = tau::get("X & X", pbf);  // idempotent AND: X&X == X
 * CHECK( are_bf_equal<node_t>(n1, n2) );
 * @endcode
 * @endinternal
 */
template <NodeType node>
bool are_bf_equal(tref n1, tref n2) {
	using tau = tree<node>;
	using tt = tau::traverser;

	LOG_DEBUG << "Begin are_bf_equal";
	LOG_TRACE << "n1 " << LOG_FM(n1);
	LOG_TRACE << "n2 " << LOG_FM(n2);
	const auto& t1 = tau::get(n1);
	const auto& t2 = tau::get(n2);
	DBG(assert(t1.is(tau::bf)));
	DBG(assert(t2.is(tau::bf)));

	if (t1 == t2) {
		LOG_DEBUG << "End are_bf_equal: true (equal bf)";
		return true;
	}

	tref bf_equal_fm = tau::build_bf_eq_0(tau::build_bf_xor(n1, n2));
	const trefs& vars = get_free_vars<node>(bf_equal_fm);
	bf_equal_fm = tau::build_wff_all_many(vars, bf_equal_fm);
	LOG_TRACE << "wff: " << LOG_FM(bf_equal_fm);

	tref normalized = normalize_non_temp<node>(bf_equal_fm);
	LOG_TRACE << "Normalized: " << LOG_FM(normalized);

	auto check = tt(normalized) | tau::wff_t;
	LOG_DEBUG << "End are_bf_equal: " << check.has_value();
	return check.has_value();
}

/**
 * @internal
 * @brief Returns `true` if Boolean function @p n is equal to any formula in @p previous.
 *
 *  Equality is checked via `are_bf_equal`.
 * @tparam node Tree node type.
 * @param n The Boolean function to test.
 * @param previous The list of candidate Boolean functions to compare against.
 * @return `true` if @p n equals at least one formula in @p previous.
 *
 * @par Example
 * @code{.cpp}
 * auto pbf = parse_bf();
 * tref n1 = tau::get("X", pbf);
 * tref n2 = tau::get("X & X", pbf);
 * tref n3 = tau::get("Y", pbf);
 * trefs previous { n3, n2 };
 * CHECK( is_bf_same_to_any_of<node_t>(n1, previous) ); // matches n2 (X&X == X)
 * @endcode
 * @endinternal
 */
template <NodeType node>
bool is_bf_same_to_any_of(tref n, trefs& previous) {
	return std::any_of(previous.begin(), previous.end(), [n](tref p) {
		return are_bf_equal<node>(n, p);
	});
}

/**
 * @internal
 * @brief Applies registered function and predicate definitions to @p spec.
 *
 *  Builds an `rr` with the global symbol definitions appended and unfolds it
 *  via `nso_rr_apply`. Returns @p spec unchanged if no `ref` nodes are present.
 * @tparam node Tree node type.
 * @param spec The specification formula to expand.
 * @return The formula with all applicable definitions unfolded.
 *
 * @par Example
 * @code{.cpp}
 * definitions<node_t>::instance().clear();
 * api<node_t>::get_function_def("f(x) := x + 1");
 * tref expr = api<node_t>::get_term("f(t)", false);
 * tref res = apply_defs_to_spec<node_t>(expr);
 * // tau::get(res).to_str() == "t+1"
 * CHECK( !tau::get(res).find_top(is<node_t, tau::ref>) );
 * @endcode
 * @endinternal
 */
template <NodeType node>
tref apply_defs_to_spec (tref spec) {
	using tau = tree<node>;
	rr<node> spec_with_defs {tau::geth(spec)};
	if (tau::get(spec).find_top(is<node, tau::ref>)) {
		const auto& defs = definitions<node>::instance().get_sym_defs();
		spec_with_defs.rec_relations.insert(spec_with_defs.rec_relations.end(),
		       defs.begin(), defs.end());
		return nso_rr_apply(spec_with_defs);
	}
	return spec;
}

/**
 * @internal
 * @brief Unfolds the registered definitions in @p fm until none applies.
 *
 *  Alternates `apply_defs_to_spec` with a simplification pass, since a
 *  definition can become applicable only after the previous unfolding has been
 *  simplified. @p pre runs before each unfolding, @p post on the result of an
 *  unfolding that changed the formula.
 *
 *  User-supplied definitions need not terminate, and a single pass cannot tell
 *  that they do not: `f(x) := f(x)'` unfolds one level per pass and the
 *  simplifier folds the new double negation straight back, so the formula
 *  oscillates forever between two states, while a growing definition never
 *  revisits a state at all. The first needs a revisit check, the second a pass
 *  cap. Both mean the formula has no normal form, so neither may return the
 *  formula reached so far -- a half-expanded term is indistinguishable from a
 *  real result to every caller.
 * @tparam node Tree node type.
 * @param fm Formula whose references are to be expanded.
 * @param pre Simplification applied before each unfolding.
 * @param post Simplification applied after an unfolding that changed @p fm.
 * @return The expanded formula, or `nullptr` if the expansion never settles.
 * @endinternal
 */
template <NodeType node>
tref expand_defs_until_settled(tref fm, auto&& pre, auto&& post) {
	using tau = tree<node>;
	// Pass cap: the global max_def_passes, 0 = unlimited (see its doc).
	std::unordered_set<tref> visited;
	for (size_t pass = 0; !max_def_passes || pass != max_def_passes;
		++pass) {
		// Unresolved symbol is still present
		if (!tau::get(fm).find_top(is<node, tau::ref>)) return fm;
		fm = pre(fm);
		if (!fm) return nullptr;
		tref expanded = apply_defs_to_spec<node>(fm);
		if (!expanded) return nullptr;
		// Structural comparison: unfolding may rebuild equal nodes.
		if (tau::get(expanded) == tau::get(fm)) return fm;
		fm = post(expanded);
		if (!fm) return nullptr;
		if (!visited.insert(fm).second) {
			LOG_ERROR << "Definition expansion oscillates without "
				"reaching a normal form; the definitions in use "
				"are most likely non-terminating for this "
				"argument";
			return nullptr;
		}
	}
	LOG_ERROR << "Definition expansion did not settle after "
		<< max_def_passes << " passes (max-def-passes); the "
		"definitions in use are most likely non-terminating for this "
		"argument";
	return nullptr;
}

// Folds constants out of quantifiers, negations and the binary connectives:
// ex x T = all x T = T, ex x F = all x F = F, !T = F, !F = T, plus the usual
// T/F identities for && and ||. Such residues can be left behind by
// substitution based eliminations, which rebuild nodes without running the
// construction hooks.
/** @internal @copydoc fold_trivial_quantifiers @endinternal */
template <NodeType node>
tref fold_trivial_quantifiers(tref fm) {
	using tau = tree<node>;
	auto f = [](tref n) -> tref {
		const auto& t = tau::get(n);
		if (!t.is(tau::wff)) return n;
		const auto& c = t[0];
		// ex x T/F → T/F, all x T/F → T/F
		if (c.is(tau::wff_ex) || c.is(tau::wff_all)) {
			tref body = c.second();
			if (tau::get(body).equals_T() || tau::get(body).equals_F())
				return body;
		}
		// !T → F, !F → T
		if (c.is(tau::wff_neg)) {
			tref b = c.first();
			if (tau::get(b).equals_T()) return tau::_F();
			if (tau::get(b).equals_F()) return tau::_T();
		}
		// Boolean identities: T/F with && and ||
		if (c.is(tau::wff_and)) {
			tref l = c.first(), r = c.second();
			if (tau::get(l).equals_T()) return r;
			if (tau::get(r).equals_T()) return l;
			if (tau::get(l).equals_F() || tau::get(r).equals_F())
				return tau::_F();
		}
		if (c.is(tau::wff_or)) {
			tref l = c.first(), r = c.second();
			if (tau::get(l).equals_T() || tau::get(r).equals_T())
				return tau::_T();
			if (tau::get(l).equals_F()) return r;
			if (tau::get(r).equals_F()) return l;
		}
		return n;
	};
	return post_order<node>(fm).apply_unique(f);
}

/**
 * @internal
 * @brief Simplifies a single clause from the outer DNF of a normalized temporal formula.
 *
 *  Removes `always`/`sometimes` sub-formulas that are implied by others in the
 *  same clause, and checks for unsatisfiable `always ∧ sometimes` pairs.
 * @tparam node Tree node type.
 * @param clause A conjunctive clause from the temporal DNF to simplify.
 * @return The simplified clause, or `std::nullopt` if the clause is unsatisfiable.
 *
 * @par Example
 * @code{.cpp}
 * // (always x=0 && y=0) && (always x=0): the second always is implied by the
 * // first (x=0 && y=0 => x=0), so it is dropped.
 * tref p = get_nso_rr("x = 0 && y = 0.").value().main->get();
 * tref q = get_nso_rr("x = 0.").value().main->get();
 * tref clause = tau::build_wff_and(tau::build_wff_always(p), tau::build_wff_always(q));
 * auto res = simplify_temporal_clause<node_t>(clause);
 * // res.has_value() == true
 * // tau::get(*res).to_str() == "always x = 0 && y = 0"
 * @endcode
 * @endinternal
 */
// Simplifies one temporal clause from the outer DNF of a normalized formula.
// Returns the simplified clause, or nullopt if the clause is unsatisfiable.
template<NodeType node>
std::optional<tref> simplify_temporal_clause(tref clause) {
	using tau = tree<node>;

	const auto& t = tau::get(clause);
	trefs aw_parts = t.select_top(is_child<node, tau::wff_always>);
	trefs st_parts = t.select_top(is_child<node, tau::wff_sometimes>);
	if ((aw_parts.size() == 1 && st_parts.empty()) ||
		(aw_parts.empty() && st_parts.size() == 1))
		return clause;

	// Replace all temporal parts with T to isolate the non-temporal skeleton.
	subtree_map<node, tref> changes;
	for (tref aw : aw_parts) changes.emplace(aw, tau::_T());
	for (tref st : st_parts) changes.emplace(st, tau::_T());
	tref new_clause = rewriter::replace<node>(clause, changes);
	DBG(LOG_TRACE << "    new clause: " << LOG_FM(new_clause);)

	// Eliminate parts in a group that are implied by another part in the same group.
	// repr(parts[i]) returns the formula to use for implication checking.
	auto eliminate_implied = [](trefs& parts, auto&& repr) {
		for (size_t i = 0; i < parts.size(); ++i) {
			// Skip parts already replaced by T: is_nso_impl(x, T) is
			// trivially true and would only re-assign T, so the pair
			// cannot eliminate anything and the two full
			// normalizations it costs are wasted. Deliberately no
			// `break` when parts[i] itself becomes T: the remaining
			// is_nso_impl(T, parts[j]) checks still ask whether
			// parts[j] is valid, and dropping that would change which
			// parts survive, not just how long it takes.
			if (tau::get(parts[i]).equals_T()) continue;
			for (size_t j = i + 1; j < parts.size(); ++j) {
				if (tau::get(parts[j]).equals_T()) continue;
				if (is_nso_impl<node>(repr(parts[i]), repr(parts[j])))
					parts[j] = tau::_T();
				else if (is_nso_impl<node>(repr(parts[j]), repr(parts[i])))
					parts[i] = tau::_T();
			}
		}
	};
	// Eliminate always parts implied by other always parts.
	eliminate_implied(aw_parts, [](tref x) { return x; });

	// Clause is unsatisfiable if any always ∧ sometimes pair is unsat.
	for (tref aw : aw_parts) for (tref st : st_parts) {
		tref f = tau::build_wff_and(
			get_temporally_quantified_formula<node>(aw),
			get_temporally_quantified_formula<node>(st));
		if (is_non_temp_nso_unsat<node>(f)) return std::nullopt;
	}

	// Eliminate sometimes parts implied by any always part.
	for (tref aw : aw_parts) for (tref& st : st_parts)
		if (is_nso_impl<node>(aw, get_temporally_quantified_formula<node>(st))) st = tau::_T();

	// Eliminate sometimes parts implied by other sometimes parts.
	eliminate_implied(st_parts, get_temporally_quantified_formula<node>);

	new_clause = tau::build_wff_and(new_clause, tau::build_wff_and(
				tau::build_wff_and(aw_parts),
				tau::build_wff_and(st_parts)));
	return new_clause;
}

/** @internal @copydoc normalize_with_temp_simp @endinternal */
template <NodeType node>
tref normalize_with_temp_simp(tref fm) {
	using tau = tree<node>;
	fm = normalize<node>(fm);
	// Substitution based eliminations rebuild nodes without running the
	// construction hooks, so trivially foldable residues (constant
	// equations, quantifiers over T/F...) may survive; rebuild with hooks
	// and fold the remaining trivial quantifiers.
	fm = fold_trivial_quantifiers<node>(tau::reget(fm));
	// Residual quantified bv subformulas can survive the substitution
	// based eliminations (the closed formula check in resolve_quantifiers
	// runs before they are created); resolve them late and fold again.
	if (tau::get(fm).find_top(is_quantifier<node>)) {
		if (auto resolved = resolve_quantifiers<node>(fm);
			resolved && resolved != fm)
			fm = fold_trivial_quantifiers<node>(
				tau::reget(resolved));
		// A block that is eliminable in isolation can still be
		// carrying its binder here: analyse_formula computes verdicts
		// whole-formula with a flat cross-scope join, so an arithmetic
		// atom in a SIBLING clause sharing free variables with the
		// block's atoms lifts those variables above `eliminable` and
		// the binder is kept (review-pointwise-revision §3, R6).
		// Re-eliminate each surviving maximal block with block-local
		// analysis, which sees no sibling content. Do not descend into
		// terms: tau_ba constants carry their own wff_ex/wff_all over
		// I/O variables, which must be left intact (the AP-7 guard).
		if (trefs blocks = rewriter::select_top_until<node>(fm,
			is_child_quantifier<node>, [](tref n) {
				return tree<node>::get(n).is_term(); });
			!blocks.empty())
		{
			subtree_map<node, tref> changes;
			for (tref b : blocks) {
				tref r = eliminate_bv_and_quantifiers<node>(b);
				if (!r || r == b || tau::get(r).find_top(
					is_quantifier<node>))
				{
					// Dual attempt: the substitution-based
					// one-point pass inside anti_prenex only
					// matches EXISTENTIAL scopes, so a
					// universal block whose vars carry the
					// `arithmetic` verdict (skipped by the
					// block core, and blasting is off by
					// default) never reaches it. Its negation
					// is the ∃ form; eliminate that and negate
					// back.
					tref nb = eliminate_bv_and_quantifiers<
						node>(tau::build_wff_neg(b));
					if (nb && !tau::get(nb).find_top(
						is_quantifier<node>))
						r = normalize_non_temp<node>(
							tau::build_wff_neg(nb));
				}
				// Adopt only a fully quantifier-free result: a
				// partially processed block (anti-prenex
				// distribution with the binders still in
				// place) is typically LARGER than the
				// original, and adopting it fattens the
				// stored spec and multiplies its DNF paths on
				// every later normalization.
				if (r && r != b && !tau::get(r).find_top(
					is_quantifier<node>))
					changes.emplace(b, r);
			}
			if (!changes.empty())
				fm = fold_trivial_quantifiers<node>(tau::reget(
					rewriter::replace<node>(fm, changes)));
		}
	}
	// Apply present function/predicate definitions
	fm = expand_defs_until_settled<node>(fm, [](tref n) { return n; },
		[](tref n) { return normalize<node>(n); });
	if (!fm) return nullptr;

	DBG(LOG_TRACE << "fm: " << LOG_FM(fm) << "\n";)
	if (tau::get(fm).equals_T() || tau::get(fm).equals_F())
		return fm;
	// If after normalization no temporal quantifier is present, the formula
	// is non-temporal
	if (!tau::get(fm).find_top(is_temporal_quantifier<node>))
		return fm;
	tref nn = tau::_F();
	// The temporal layer of a formula is in DNF
	for (tref clause : expression_paths<node>(fm)) {
		DBG(LOG_TRACE << "    clause: " << LOG_FM(clause);)
		if (auto simplified = simplify_temporal_clause<node>(clause)) {
			nn = tau::build_wff_or(nn, *simplified);
			DBG(LOG_TRACE << "    nn: " << LOG_FM(nn);)
		} else {
			DBG(LOG_TRACE << "    clause false";)
		}
	}
	DBG(assert(nn != nullptr);)
	DBG(LOG_TRACE << "normalize_with_temp_simp result: " << LOG_FM(nn);)
	return nn;
}

/**
 * @internal
 * @brief Returns the maximum integer lookback offset found in `offsets` nodes of @p form.
 * @tparam node Tree node type.
 * @param form The formula to scan for integer offset values.
 * @return The largest integer offset value found, or 0 if none exist.
 *
 * @par Example
 * @code{.cpp}
 * // Rule heads f[0], f[2], f[4], f[8] have integer lookbacks 0/2/4/8;
 * // f[n] has a symbolic (capture) offset, so its lookback is reported as 0.
 * auto nso_rr = get_bf_nso_rr(
 *     "f[0](x):tau := 0."
 *     "f[2](x):tau := 0."
 *     "f[4](x):tau := 0."
 *     "f[8](x):tau := 0."
 *     "f[n](x):tau := 1.",
 *     "f(x):tau").value();
 * // get_max_lookback_in_rr<node_t>(nso_rr.rec_relations[3].first->get()) == 8
 * // get_max_lookback_in_rr<node_t>(nso_rr.rec_relations[4].first->get()) == 0
 * @endcode
 * @endinternal
 */
template <NodeType node>
size_t get_max_lookback_in_rr(tref form) {
	using tau = tree<node>;
	using tt = tau::traverser;
	size_t max = 0;
	for (tref offsets : tau::get(form).select_top(is<node, tau::offsets>)) {
		for (auto offset : (tt(offsets) || tau::offset)())
			if (auto i = offset | tau::integer; i) {
				int_t c = i | tt::integer;
				max = std::max(max, static_cast<size_t>(c));
			}
	}
	return max;
}

/**
 * @internal
 * @brief Builds a new shift node from an existing `shift` reference adjusted to `step - offset`.
 *
 *  Returns the capture node directly when `step == offset`.
 * @tparam node Tree node type.
 * @param shift The existing shift node whose numeric offset will be adjusted.
 * @param step The target step value used to compute the new offset.
 * @return A new shift node with the adjusted offset, or the capture node if `step == offset`.
 *
 * @par Example
 * @code{.cpp}
 * // Extract the "n - 1" shift node from the body of "h[n](X):tau := h[n-1](X)'."
 * auto nso_rr = get_bf_nso_rr(
 *     "h[n](X):tau := h[n - 1](X)'."
 *     "h[0](X):tau := X.", "h(Y)").value();
 * auto rr_captures = transform_ref_args_to_captures<node_t>(nso_rr);
 * tref body = rr_captures.rec_relations[0].second->get(); // h[n-1](X)'
 * tref ref = get_ref<node_t>(body);
 * tref shift = (tt(ref) | tau::offsets | tau::offset | tau::shift).value();
 * // tau::get(shift).to_str() == "n-1"
 *
 * tref res1 = build_shift_from_shift<node_t>(shift, 1); // step == offset (1)
 * // tau::get(res1).to_str() == "n"     (bare capture)
 * tref res3 = build_shift_from_shift<node_t>(shift, 3);
 * // tau::get(res3).to_str() == "n-2"   (step - offset = 3 - 1)
 * @endcode
 * @endinternal
 */
template <NodeType node>
tref build_shift_from_shift(tref shift, size_t step) {
	using tau = tree<node>;
	using tt = tau::traverser;
	auto num = tt(shift) | tau::num;
	auto offset = num | tt::num;
	if (step == offset) return tt(shift) | tau::capture | tt::ref;
	subtree_map<node, tref> changes{
		{ num.value(), tau::get_num(step - offset) }};
	return rewriter::replace<node>(shift, changes);
}

/**
 * @internal
 * @brief Replaces all `shift` sub-nodes in @p form with adjusted shifts for the given @p step.
 *
 *  Each shift is rewritten by `build_shift_from_shift` to reflect the current step.
 * @tparam node Tree node type.
 * @param form The recurrence main formula containing shift nodes.
 * @param step The current enumeration step used to adjust each shift offset.
 * @return The formula with all shift nodes replaced by their step-adjusted equivalents.
 *
 * @par Example
 * This drives `build_enumerated_main_step`'s use of `build_shift_from_shift`
 * across every `shift` sub-node of @p form in one pass: for each top-level
 * `offsets` node found, its `shift` child (if any) is looked up and replaced
 * by `build_shift_from_shift<node>(shift, step)`. See @ref build_shift_from_shift
 * for the underlying single-shift adjustment (e.g. the `"n - 1"` shift of an
 * `h[n](X):tau := h[n-1](X)'.` rule body becomes `"n"` at `step == 1` and
 * `"n-2"` at `step == 3`).
 * @endinternal
 */
template <NodeType node>
tref build_main_step(tref form, size_t step) {
	using tau = tree<node>;
	using tt = tau::traverser;
	subtree_map<node, tref> changes;
	for (tref offsets : tau::get(form).select_top(is<node, tau::offsets>)) {
		auto shift = tt(offsets) | tau::shift;
		if (!shift.has_value()) continue;
		auto nshift = build_shift_from_shift<node>(shift.value(), step);
		changes[shift.value()] = nshift;
	}
	return rewriter::replace<node>(form, changes);
}

/**
 * @internal
 * @brief Creates an enumerated (concrete) step of the recurrence main formula.
 *
 *  Fixes the first offset to @p i and all remaining offsets to 0, then
 *  delegates to `build_main_step` for shift adjustment.
 * @tparam node Tree node type.
 * @param form The recurrence main formula to instantiate.
 * @param i The concrete index to assign to the first offset.
 * @param offset_arity The total number of offsets to generate.
 * @return The main formula with the first offset set to @p i and the rest to 0.
 *
 * @par Example
 * @code{.cpp}
 * // "h(Y)" (a fixed-point call, no offset) instantiated at concrete steps
 * auto nso_rr = get_bf_nso_rr(
 *     "h[n](X):tau := h[n - 1](X)'."
 *     "h[0](X):tau := X.", "h(Y)").value();
 * auto rr_captures = transform_ref_args_to_captures<node_t>(nso_rr);
 * tref main = rr_captures.main->get(); // "h(Y)"
 * tref step0 = build_enumerated_main_step<node_t>(main, 0, 1);
 * // tau::get(step0).to_str() == "h[0](Y)"
 * tref step3 = build_enumerated_main_step<node_t>(main, 3, 1);
 * // tau::get(step3).to_str() == "h[3](Y)"
 * @endcode
 * @endinternal
 */
// enumerates index in main with step i - used for finding a fixed point
template <NodeType node>
tref build_enumerated_main_step(tref form, size_t i, size_t offset_arity) {
	using tau = tree<node>;
	LOG_TRACE << "Begin build_enumerated_main_step: " << LOG_FM_DUMP(form)
		<< " step: " << i << " offset arity: " << offset_arity;
	subtree_map<node, tref> changes;
	trefs ofs; // create offsets node
	ofs.push_back(tau::get(tau::offset, tau::get_integer(i)));
	for (size_t o = 1; o < offset_arity; ++o)
		ofs.push_back(tau::get(tau::offset, tau::get_integer(0)));

	// create enumerated replacement
	const auto& t = tau::get(form)[0][0];
	LOG_TRACE << "t: " << LOG_FM_DUMP(t.get());
	LOG_TRACE << "t.value: " << t.value;
	LOG_TRACE << "t[0]: " << LOG_FM(t.first());
	LOG_TRACE << "t[1]: " << LOG_FM(t.second());
	changes[t.get()] = tau::get(t.value,
		{ t.first(), tau::get(tau::offsets, ofs), t.second() },
		t.right_sibling());
	form = rewriter::replace<node>(form, changes);
	LOG_DEBUG << "(F*) " << LOG_FM(form);
	return build_main_step<node>(form, i);
}

/** @internal @copydoc get_unbindable_relative_offset @endinternal */
template <NodeType node>
tref get_unbindable_relative_offset(tref head, tref body) {
	using tau = tree<node>;
	// Every variable an offset in the head mentions is bound for the body.
	// That is not only the head's own offset -- `r[n](x)` -- but also one
	// nested in an argument: `pred(int[t](1)) := int[t-1](1)` binds `t`
	// just as well, and rejecting it would break ordinary definitions.
	// Captures and variables share a name id (transform_ref_args_to_captures
	// builds a capture from its var_name's data), so one set covers this
	// running either before or after that transformation.
	auto offset_vars = [](tref n, std::set<size_t>& out) {
		for (tref offsets : tau::get(n).select_all(is<node, tau::offsets>))
			for (tref v : tau::get(offsets).select_all(
				is<node>({ tau::var_name, tau::capture })))
					out.insert(tau::get(v).value.data);
	};
	std::set<size_t> bound;
	offset_vars(head, bound);
	for (tref ref : tau::get(body).select_all(is<node, tau::ref>)) {
		std::set<size_t> used;
		offset_vars(ref, used);
		for (size_t v : used) if (!bound.contains(v)) return ref;
	}
	return nullptr;
}

/**
 * @internal
 * @brief Validates a recurrence relation.
 *
 *  Checks that the main formula has no relative offsets, that no rule's head
 *  contains a shift offset, and that integer-indexed rules do not depend on
 *  future states.
 * @tparam node Tree node type.
 * @param nso_rr The recurrence relation to validate.
 * @return `true` if all validity conditions are satisfied, `false` otherwise.
 *
 * @par Example
 * @code{.cpp}
 * // As in nso_rr_apply, offsets must first be transformed to captures so
 * // the relative-offset variable ("n") is recognized as such.
 * auto nso_rr = get_bf_nso_rr(
 *     "h[n](X):tau := h[n - 1](X)'."
 *     "h[0](X):tau := X.", "h[8](Y)").value();
 * auto rr_captures = transform_ref_args_to_captures<node_t>(nso_rr);
 * CHECK( is_valid<node_t>(rr_captures) );
 * @endcode
 * @endinternal
 */
template <NodeType node>
bool is_valid(const rr<node>& nso_rr) {
	using tau = tree<node>;
	LOG_TRACE << "-- is_valid: " << LOG_RR(nso_rr);
	for (const auto& r : nso_rr.rec_relations)
		if (tref ref = get_unbindable_relative_offset<node>(
			r.first->get(), r.second->get()); ref)
	{
		LOG_ERROR << "Recurrence relation "
			<< TAU_TO_STR(r.first->get()) << " cannot use the "
			"relative offset of " << TAU_TO_STR(ref)
			<< ": its head declares no offset to bind it";
		return false;
	}
	for (tref main_offsets : tau::get(nso_rr.main)
		.select_all(is<node,tau::offsets>)) if (tau::get(main_offsets)
			.find_top(is<node, tau::capture>))
	{
		LOG_ERROR << "Main " << TAU_TO_STR(nso_rr.main->get())
			<< " cannot contain a relative offset "
			<< TAU_TO_STR(main_offsets);
		return false; // capture in main's offset
	}
	for (size_t ri = 0; ri != nso_rr.rec_relations.size(); ++ri) {
		const auto& r = nso_rr.rec_relations[ri];
		auto left = get_ref_info<node>(get_ref<node>(r.first->get()));
		for (const auto& [ot, _] : left.second)
			if (ot == tau::shift) {
				LOG_ERROR << "Recurrence relation "
					<< r.first->get() << " cannot contain "
					<< "an offset shift";
				return false; // head ref cannot have shift
			}
		if (left.second.size() == 0) continue; // no offsets
		// take only first offset for consideration
		offset_t ho = left.second.front();
		LOG_TRACE << "head offset " << LOG_NT(ho.first) << " / " << ho.second;
		for (tref ref : tau::get(r.second)
			.select_all(is<node, tau::ref>))
		{
			auto right = get_ref_info<node>(ref);
			if (right.second.size() == 0) continue; // no offsets
			auto& bo = right.second.front();
			LOG_TRACE << "body offset " << LOG_NT(bo.first) << " / " << bo.second;
			if (ho.first == tau::integer) {
				if (bo.first == tau::capture) {
					LOG_ERROR << "Recurrence relation "
						<< r.first << " (having a fixed"
						" offset) cannot depend on a "
						"relative offset " << r.second;
					return false; // left num right capture
				}
				if (bo.first == tau::integer
					&& ho.second < bo.second)
				{
					LOG_ERROR << "Recurrence relation "
						<< r.first << " cannot depend "
						<< "on a future state "
						<< r.second;
					return false; // l num < r num
				}
			}
		}
	}
	LOG_TRACE << "-- Recurrence relation is valid";
	return true;
}

/**
 * @internal
 * @brief Checks that a recurrence relation is well-founded.
 *
 *  Requires at least one relative (capture-offset) rule and verifies that the
 *  dependency graph among rule signatures is acyclic.
 * @tparam node Tree node type.
 * @param nso_rr The recurrence relation to check.
 * @return `true` if the relation is well-founded, `false` otherwise.
 *
 * @par Example
 * @code{.cpp}
 * // Must run after transform_ref_args_to_captures (as nso_rr_apply does)
 * // so the relative-offset variable "n" is recognized as a capture offset;
 * // without it, has_relative_rule stays false and this returns false.
 * auto nso_rr = get_bf_nso_rr(
 *     "h[n](X):tau := h[n - 1](X)'."
 *     "h[0](X):tau := X.", "h[8](Y)").value();
 * auto rr_captures = transform_ref_args_to_captures<node_t>(nso_rr);
 * CHECK( is_well_founded<node_t>(rr_captures) );
 * @endcode
 * @endinternal
 */
template <NodeType node>
bool is_well_founded(const rr<node>& nso_rr) {
	using tau = tree<node>;
	LOG_TRACE << "-- is_well_founded: " << LOG_RR(nso_rr);
	std::unordered_map<rr_sig, std::set<rr_sig>> graph;
	std::unordered_map<rr_sig, bool> visited, visiting;
	std::function<bool(rr_sig)> is_cyclic = [&](const rr_sig& sig) {
		if (visiting[sig]) return true;
		if (visited[sig]) return false;
		visiting[sig] = true;
		for (const auto& neighbor : graph[sig])
			if (is_cyclic(neighbor)) return true;
		visiting[sig] = false;
		visited[sig]  = true;
		return false;
	};
	bool has_relative_rule = false;
	for (size_t ri = 0; ri != nso_rr.rec_relations.size(); ++ri) {
		LOG_TRACE <<"rec relation "<<LOG_RULE(nso_rr.rec_relations[ri]);
		const auto& r = nso_rr.rec_relations[ri];
		auto left = get_ref_info<node>(get_ref<node>(r.first->get()));
		for (const auto& [ot, _] : left.second)
			if (ot == tau::capture)
				has_relative_rule = true;
		if (left.second.size() == 0) continue; // no offsets
		// take only first offset for consideration
		offset_t ho = left.second.front();
		LOG_TRACE << "head offset " << LOG_NT(ho.first) << " / " << ho.second;
		for (const auto& ref : tau::get(r.second).select_all(
			is<node, tau::ref>))
		{
			auto right = get_ref_info<node>(ref);
			if (right.second.size() == 0) continue; // no offsets
			auto& bo = right.second.front();
			LOG_TRACE << "body offset " << LOG_NT(bo.first) << " / " << bo.second;
			if (ho == bo) graph[left.first].insert(right.first);
		}
		visited[left.first]  = false;
		visiting[left.first] = false;
	}
	if (!has_relative_rule) {
		LOG_ERROR << "Recurrence relation has no rules"
			  << " other than initial conditions";
		return false;
	}
	for (const auto& [left, _] : graph)
		if (!visited[left] && is_cyclic(left)) {
			LOG_ERROR << "Recurrence relation is cyclic";
			return false;
		}
	LOG_DEBUG << "Recurrence relation is well founded";
	return true;
}

/**
 * @internal
 * @brief Iterates the recurrence relation to find a fixed point starting from `max_lookback`.
 *
 *  Applies all rules at each step until the result stabilizes (fixed point) or
 *  a loop is detected. Returns the fixed-point formula, or a fallback value when
 *  a loop is found.
 * @tparam node Tree node type.
 * @param nso_rr The recurrence relation driving the iteration.
 * @param form The main formula template to enumerate.
 * @param nt The non-terminal type (e.g. `wff` or `bf`) determining normalization and equivalence checks.
 * @param offset_arity The number of offsets in the main formula's reference.
 * @param fallback The formula to return when a loop (no fixed point) is detected.
 * @return The fixed-point formula, or @p fallback if the iteration loops without converging.
 *
 * @par Example
 * @code{.cpp}
 * // h[n](X) := h[n-1](X)' with h[0](X) := X alternates between X and X'
 * // forever as n grows, so no single value is ever reached: the iteration
 * // loops and the supplied fallback (0) is returned instead.
 * auto nso_rr = get_bf_nso_rr(
 *     "h[n](X):tau := h[n - 1](X)'."
 *     "h[0](X):tau := X.", "h(Y)").value();
 * auto rr_captures = transform_ref_args_to_captures<node_t>(nso_rr);
 * tref main = rr_captures.main->get(); // "h(Y)", the fixed-point call
 * tref fp = calculate_fixed_point<node_t>(rr_captures, main, tau::bf, 1,
 *     tau::_0(tau_type_id<node_t>()));
 * // tau::get(fp).to_str() == "0"   (the fallback, since no fixed point exists)
 * @endcode
 * @endinternal
 */
template <NodeType node>
tref calculate_fixed_point(const rr<node>& nso_rr,
	tref form, typename node::type nt, size_t offset_arity,
	tref fallback)
{
	using tau = tree<node>;
	LOG_DEBUG << "Calculating fixed point: " << LOG_FM(form);
	LOG_DEBUG << "Spec: " << LOG_RR(nso_rr);
	//ptree<BAs...>(std::cout << "form: ", form) << "\n";

	auto ft = tau::get(fallback).get_type();
	bool first = ft == tau::first_sym, last = ft == tau::last_sym;
	if (!first && !last && ft != nt) {
		LOG_ERROR << "Fallback type mismatch";
		return nullptr;
	}

	if (!is_well_founded<node>(nso_rr)) return nullptr;

	trefs previous;
	// Identity index over `previous`, so a repeated value is recognised in O(1)
	// instead of one full normalize_non_temp-based equivalence proof per stored
	// step. It only *pre-empts* the semantic scan below -- normalization is not
	// canonical, so two equivalent steps need not be structurally identical and
	// dropping the semantic check would weaken loop detection.
	subtree_unordered_set<node> seen;
	tref current;

	// Termination cap: the global max_enum_steps, 0 = unlimited (see its
	// doc for why an unlimited run on a growing recurrence never ends).
	size_t steps = 0;

	size_t max_lookback = 0;
	std::vector<size_t> lookbacks;
	for (const auto& r : nso_rr.rec_relations) {
		size_t lookback = std::max(
			get_max_lookback_in_rr<node>(r.first->get()),
			get_max_lookback_in_rr<node>(r.second->get()));
		lookbacks.push_back(lookback);
		max_lookback = std::max(max_lookback, lookback);
	}
	LOG_DEBUG << "max lookback " << max_lookback;

	// Families defined by the driving recurrence: the full rr_sig (name,
	// offset arity, AND ref-arg arity), matching is_functional_ref's
	// fixpoint-call family match. Name+arg_arity alone would conflate an
	// indexed family like `f[n]/f[0]` with an unrelated plain function
	// `f(x)` of the same name/arity (see validate_rr_case_types's family
	// key in tau_tree_extractors.tmpl.h for the same fix and the fuller
	// rationale) -- here that conflation would make this guard treat a
	// residual belonging to the unrelated plain function as if it were
	// part of the recurrence this call actually drives.
	std::set<rr_sig> def_families;
	for (const auto& r : nso_rr.rec_relations)
		if (tref h = unwrap_to_ref<node>(r.first->get()); h)
			def_families.insert(get_rr_sig<node>(h));

	// Support for the partial-match guard below (search "Partial-match
	// guard" for the rationale). Hoisted out of the `for (i)` loop since
	// both are loop-invariant: `untyped_rules` depends only on @p nso_rr,
	// not on the current step, and `legit_uninterpreted` accumulates
	// verdicts *across* steps -- trees are hash-consed, so a residual
	// with the same shape recurring at a later step (as it typically
	// does for a legitimately-uninterpreted base) is literally the same
	// tref, and the untyped re-saturation probe below need only ever be
	// paid once for it, not once per step.
	// apply_unique takes its callable by non-const lvalue reference (see
	// pre_order<node>::apply_unique's signature in
	// external/parser/src/utility/tree.h/tree_traversals_pre_order.tmpl.h)
	// -- it must be a named variable, not a temporary lambda, or overload
	// resolution fails to bind. Mirrors resolve_io_vars's `resolve`
	// lambda, passed the same way.
	auto do_untype = [](tref m) { return untype<node>(m); };
	auto strip_types = [&do_untype](tref n) {
		return pre_order<node>(n).apply_unique(do_untype);
	};
	rewriter::rules untyped_rules;
	if (!def_families.empty())
		for (const auto& ur : nso_rr.rec_relations)
			untyped_rules.emplace_back(
				tau::geth(strip_types(ur.first->get())),
				tau::geth(strip_types(ur.second->get())));
	// The untyped probe's own saturation cap. Stripping types only
	// ENABLES matches (never blocks one the typed loop above already
	// found), so an untyped re-saturation can diverge on a shape the
	// typed loop never reached -- e.g. cross-family type-blocked mutual
	// recursion (`a[n](x:sbf) := b[n](x)` with `b[n](x:tau) := a[n](x)`:
	// each family is internally consistent on its own, so
	// validate_rr_case_types passes both, the typed loop leaves a
	// residual, and the untyped probe would rewrite a->b->a->b... with
	// no fixed point, forever). Reuse max_enum_steps when the caller set
	// a finite bound (0 means unlimited); otherwise fall back to a
	// generous but finite constant so the guard that exists to turn a
	// hang into a fast error cannot itself hang.
	static constexpr size_t probe_saturation_fallback_cap = 10000;
	const size_t probe_cap = max_enum_steps
		? max_enum_steps : probe_saturation_fallback_cap;
	subtree_unordered_set<node> legit_uninterpreted;

	// Whether any rule application has ever rewritten an enumerated step.
	// A rule with a capture offset matches every index from its lookback
	// on, and a fixed-offset rule only indices up to max_lookback, so if
	// nothing fired at the first two steps nothing ever will: the call
	// does not reach its definitions at all (typically a kind or type
	// mismatch between the call site and the stored rules), and silently
	// enumerating bare `name[i](args)` refs forever used to hang the REPL.
	bool ever_changed = false;

	for (size_t i = max_lookback; ; i++) {
		++steps;
		if (max_enum_steps && steps > max_enum_steps) {
			LOG_ERROR << "calculate_fixed_point: no fixed point and no "
				"loop after " << max_enum_steps
				<< " enumeration steps (max-enum-steps) for "
				<< LOG_FM(form)
				<< "; giving up. This is a bound on the search, "
				"not a proof that no fixed point exists.";
			return nullptr;
		}
		current = build_enumerated_main_step<node>(
							form, i, offset_arity);
		bool changed;
		do { // apply rec relation rules and check for cycle dependency
			changed = false;
			for (size_t ri = 0;
				ri != nso_rr.rec_relations.size(); ++ri)
			{
				const auto& r = nso_rr.rec_relations[ri];
				if (lookbacks[ri] > i) {
					// LOG_DEBUG << "(I) -- current step " << i << " < " << lookbacks[ri] << " lookback, skipping " << r;
					continue; // skip steps depending on future fixed offsets
				}
				auto prev = current;
				current = nso_rr_apply<node>(r, prev);
				if (tau::get(current) != tau::get(prev)) changed = true,
					ever_changed = true;
			}
		} while (changed);

		if (!ever_changed && i > max_lookback) {
			LOG_ERROR << "calculate_fixed_point: no recurrence rule "
				"applies to " << LOG_FM(current) << "; the call "
				"does not match its definitions (kind or type "
				"mismatch between the call site and the rules); "
				"giving up.";
			return nullptr;
		}

		// Partial-match guard: rules were applied to saturation, yet the
		// step still holds a reference into a family this recurrence
		// defines. This is only a defect when some rule's PATTERN
		// actually covers this position and a type mismatch is what
		// blocked it (typically the family's cases disagree on argument
		// types -- normally caught earlier by validate_rr_case_types;
		// this is the fallback for any future partial-match cause). It
		// is NOT a defect when no rule covers this position at all
		// regardless of type -- e.g. a step-only recurrence (no base
		// case) enumerated down to an offset no rule's pattern reaches
		// leaves an uninterpreted call that are_nso_equivalent treats as
		// an opaque atom, and a fixpoint parametric in it can still be
		// found (see "no initial condition",
		// test_integration-nso_rr_fixed_point.cpp).
		//
		// Tell the two apart by re-attempting saturation on an UNTYPED
		// clone of the whole step (using the hoisted untyped rules),
		// via the exact same per-rule application the loop above just
		// used, and see which residual(s) that unblocks: a residual
		// whose own untyped shape is no longer present anywhere in the
		// probe result was rewritten by some rule once its type stopped
		// blocking -- that rule's shape does cover the position and
		// this IS the defect. A residual whose untyped shape is still
		// present unchanged was never going to match here regardless of
		// type -- legitimately left alone (and memoized: trees are
		// hash-consed, so the same shape recurring at a later step is
		// literally the same tref and is skipped without re-probing).
		// Checking every not-yet-cleared residual against the SAME
		// single probe run (rather than probing per residual) is what
		// lets one whole-step probe correctly attribute the error to
		// the specific residual a rule actually unblocks, instead of
		// always blaming whichever residual happens to appear first in
		// document order.
		trefs residuals;
		for (tref rr_ref : tau::get(current).select_all(is<node, tau::ref>)) {
			if (def_families.contains(get_rr_sig<node>(rr_ref))
				&& !legit_uninterpreted.contains(rr_ref))
				residuals.push_back(rr_ref);
		}
		if (!residuals.empty()) {
			tref untyped_current = strip_types(current);
			tref probe = untyped_current;
			bool probe_changed;
			bool probe_exhausted = false;
			size_t probe_steps = 0;
			do {
				probe_changed = false;
				// Deliberately does not re-check lookbacks[ri] > i (the
				// main loop's skip for a fixed-offset rule whose literal
				// offset the enumeration hasn't reached yet): by this
				// point in the outer loop i >= max_lookback always
				// holds, so every rule was already eligible above and
				// stays eligible here.
				for (const auto& ur : untyped_rules) {
					tref pprev = probe;
					probe = nso_rr_apply<node>(ur, probe);
					if (tau::get(probe) != tau::get(pprev))
						probe_changed = true;
				}
				if (probe_changed && ++probe_steps >= probe_cap) {
					// Divergence, not silence: the probe kept finding
					// something to rewrite, so some rule's shape DOES
					// cover this residual -- it was genuinely type-
					// blocked in the typed loop above. Exhausting the
					// cap is therefore itself the "blocked" verdict,
					// same as if the probe had stabilized on a changed
					// result; fall through to attribution below with
					// whatever the probe last produced.
					probe_exhausted = true;
					break;
				}
			} while (probe_changed);
			subtree_unordered_set<node> probe_refs;
			for (tref pr : tau::get(probe).select_all(is<node, tau::ref>))
				probe_refs.insert(pr);
			tref blocked = nullptr;
			for (tref rr_ref : residuals) {
				if (!probe_refs.contains(strip_types(rr_ref))) {
					blocked = rr_ref;
					break;
				}
			}
			// A stabilized probe with every residual's untyped shape
			// still present means none of them was ever going to match
			// (the normal, legitimate case, handled below); an
			// exhausted (diverging) probe never gets to claim that --
			// name whichever residual triggered this probe run.
			if (!blocked && probe_exhausted) blocked = residuals.front();
			if (blocked) {
				LOG_ERROR << "calculate_fixed_point: `"
					<< LOG_FM(blocked) << "` remains after every rule"
					" was applied to saturation; one of its cases"
					" never matches the call (kind or type mismatch);"
					" giving up.";
				return nullptr;
			}
			for (tref rr_ref : residuals) legit_uninterpreted.insert(rr_ref);
			DBG(LOG_TRACE << "calculate_fixed_point: " << residuals.size()
				<< " residual(s) remain, but no rule matches any of"
				" them even untyped; legitimately uninterpreted,"
				" continuing";)
		}

		LOG_DEBUG << "Begin enumeration step";
		LOG_DEBUG << "current: " << LOG_FM(current);

		LOG_DEBUG << "Normalize step";
		current = nt == tau::wff ? normalize<node>(current)
					 : bf_reduced_dnf<node>(current);
		LOG_DEBUG << "Normalized step";
		LOG_DEBUG << "current: " << LOG_FM(current);

		if (previous.size() && (nt == tau::wff
			? are_nso_equivalent<node>(current, previous.back())
			: are_bf_equal<node>(current, previous.back())))
		{
			LOG_DEBUG << "End enumeration step: fixed point"
						<< " found at step: " << i;
			LOG_DEBUG << "previous.back(): " << LOG_FM(previous.back());
			return previous.back();
		}
		else if (previous.size() > 1 && (seen.contains(current) || (nt == tau::wff
			? is_nso_equivalent_to_any_of<node>(current, previous)
			: is_bf_same_to_any_of<node>(current, previous))))
		{
			LOG_DEBUG << "End enumeration step - loop "
				<< "(no fixed point) detected at step: "
				<< i << " returning fallback "
				<< (first ? "first" : last ? "last" : "");

			if (last) return previous.back();
			if (first) return current;
			LOG_DEBUG << "End enumeration step - fallback: "
							<< LOG_FM(fallback);
			return fallback;
		}
		LOG_DEBUG << "End enumeration step - no fixed point resolution "
			<< "at step: " << i << " incrementing";
		LOG_DEBUG << "current: " << LOG_FM(current);
		previous.push_back(current);
		seen.insert(current);
	}
	// Unreachable: every exit from the loop above is a return, and the step cap
	// guarantees one is taken.
	return nullptr;
}

// Normalizes a Boolean function having no recurrence relation
/** @internal @copydoc bf_normalizer_without_rec_relation @endinternal */
template <NodeType node>
tref bf_normalizer_without_rec_relation(tref bf) {
	LOG_DEBUG << "Begin Boolean function normalizer";

	bf = syntactic_path_simplification<node>(bf);
	tref result = bf_reduced_dnf<node>(bf);
	// Apply present function/predicate definitions
	result = expand_defs_until_settled<node>(result,
		[](tref n) { return syntactic_path_simplification<node>(n); },
		[](tref n) { return bf_reduced_dnf<node>(n); });

	LOG_DEBUG << "End Boolean function normalizer";

	return result;
}

// Normalizes a Boolean function in which recurrence relations are present
/** @internal @copydoc bf_normalizer_with_rec_relation @endinternal */
template <NodeType node>
tref bf_normalizer_with_rec_relation(const rr<node> &bf) {
	tref bf_unfolded = nso_rr_apply<node>(bf);

	if (!bf_unfolded) return nullptr;

	LOG_DEBUG << "Begin Boolean function normalizer";
	auto result = bf_normalizer_without_rec_relation<node>(bf_unfolded);
	LOG_DEBUG << "End Boolean function normalizer";

	return result;
}

// REVIEW (HIGH) review overall execution
/** @internal @copydoc normalizer(const rr<node>&) @endinternal */
template <NodeType node>
tref normalizer(const rr<node>& nso_rr) {
	// IDEA extract this to an operator| overload

	LOG_DEBUG << "Begin normalizer";
	LOG_DEBUG << "Spec: " << LOG_RR(nso_rr);

	tref fm = nso_rr_apply<node>(nso_rr);
	if (!fm) return nullptr;
	tref res = normalize_with_temp_simp<node>(fm);

	LOG_DEBUG << "End normalizer";
	LOG_DEBUG << "Result: " << LOG_FM(res);
	return res;
}

/** @internal @copydoc normalizer(tref) @endinternal */
template <NodeType node>
tref normalizer(tref fm) {
	return normalize_with_temp_simp<node>(fm);
}

/**
 * @internal
 * @brief Converts the temporal layer of a formula to reduced DNF, squeezes the always
 * statements and ensures that formulas containing temporal variables are
 * explicitly quantified while non-temporal formulas are not quantified temporally.
 * @tparam node Tree node type
 * @tparam normalize_scopes If true, temporally quantified formulas are converted to Boole normal form
 * @param fm The formula that is to be temporally normalized
 * @return The resulting formula after normalizing the temporal quantifiers
 * @endinternal
 */
template <NodeType node, bool normalize_scopes>
tref normalize_temporal_quantifiers(tref fm) {
	using tau = tree<node>;
	auto norm = [](tref arg) {
		return normalize_scopes
					? term_boole_normal_form<node>(arg)
					: arg;
	};
	if (has_temp_var<node>(fm)) {
		if (tau::get(fm).find_top(is_temporal_quantifier<node>)) {
			// By assumption, all temporal variables are explicitly
			// quantified by temporal quantifier without nesting.
			// DNF conversion is only done on temporal level
			fm = temporal_layer_to_dnf<node>(fm);
			// Simplify temporal layer
			fm = reduce<node>(fm);
			trefs clauses = get_dnf_wff_clauses<node>(fm);
			tref non_temp_clauses = tau::_F();
			tref res = tau::_F();
			for (tref clause : clauses) {
				if (!has_temp_var<node>(clause)) {
					// Remove all temporal quantifiers
					clause = pre_order<node>(clause).
							apply_unique(get_temporally_quantified_formula<node>);
					non_temp_clauses = tau::build_wff_or(
						non_temp_clauses, clause);
					continue;
				}
				tref always_part = tau::_T();
				tref staying = tau::_T();
				// In each clause squeeze all always statements
				for (tref conj : get_cnf_wff_clauses<node>(clause)) {
					// All parts are temporally quantified
					DBG(assert(is_child_temporal_quantifier<node>(conj) || !has_temp_var<node>(conj));)
					if (!has_temp_var<node>(conj))
						always_part = tau::build_wff_and(
							always_part, get_temporally_quantified_formula<node>(conj));
					// The assert above used to be the only thing standing
					// between a malformed temporal layer and a wrong
					// answer. In Release a conjunct that has a temporal
					// variable but is headed by neither wff_always nor
					// wff_sometimes (a negated temporal operator, a
					// disjunction the DNF pass failed to lift) fell into
					// the always branch below and was silently
					// re-quantified universally -- and, if it carries a
					// temporal quantifier of its own, nested inside the
					// always wrapper added at the end of the clause,
					// against this function's stated no-nesting
					// assumption. Decline loudly instead: `fm` here is the
					// DNF-reduced input, so returning it leaves the
					// temporal layer unnormalized rather than wrong.
					else if (!is_child_temporal_quantifier<node>(conj)) {
						LOG_ERROR << "normalize_temporal_quantifiers: conjunct "
							<< LOG_FM(conj) << " has a temporal variable but no"
							" temporal quantifier of its own; leaving the"
							" temporal layer unnormalized. This is a"
							" conservative fallback, not a normal form.";
						return fm;
					}
					// TODO: always conjunction is inefficient
					else if (!is_child<node>(conj, tau::wff_sometimes))
						always_part = always_conjunction<node>(always_part, conj);
					else staying = tau::build_wff_and(
						staying,
						tau::build_wff_sometimes(norm(tau::trim2(conj))));
				}
				always_part = tau::build_wff_always(norm(always_part));
				clause = tau::build_wff_and(always_part, staying);
				res = tau::build_wff_or(res, clause);
			}
			if (!tau::get(non_temp_clauses).equals_F()) {
				non_temp_clauses = tau::build_wff_always(
					norm(non_temp_clauses));
				res = tau::build_wff_or(res, non_temp_clauses);
			}
			return res;
		} else {
			// Temporal variable without temporal quantifier
			// By assumption we quantify fm universally
			return build_wff_always<node>(norm(fm));
		}
	} else {
		// No temporal variable, so no temporal quantifier needed
		fm = pre_order<node>(fm).apply_unique(get_temporally_quantified_formula<node>);
		return norm(fm);
	}
}

} // namespace idni::tau_lang
