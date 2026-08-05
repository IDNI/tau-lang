// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "antiprenexing/antiprenexing.h"

namespace idni::tau_lang {

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "anti_prenex"


/**
 * @internal
 * @brief Nesting depth of `blast_block`'s blast-then-re-enter hop.
 *
 * The hop is `anti_prenex_block` (7-arg) -> `blast_block` ->
 * `anti_prenex_block` (pipeline) -> `process_quantifier_block` -> the 7-arg
 * recursion again, and it was bounded only by `blasted != ex_fm`. The
 * split budget does not bound it either: the pipeline entry allocates a fresh
 * `splits_left` per block, so the shared-budget discipline does not survive the
 * hop. If `bv_predicate_blasting` is ever non-idempotent on its own output the
 * recursion is unbounded.
 *
 * The counter lives here rather than being threaded through the five signatures
 * between the two ends because the pipeline entry is forward-declared in
 * `heuristics/bv_predicate_blasting.h`, which cannot carry a default argument
 * for it (a function template's default cannot be added in a later
 * declaration). Named and per-node-type rather than a hidden function-local
 * `static`, and always adjusted through `blast_reentry_guard` so it unwinds on
 * every exit including an exception. Single-threaded, like the rest of this
 * pass -- see `bv_blasting`'s note in `heuristics/bv_predicate_blasting.h`.
 * @endinternal
 */
template <NodeType node>
size_t& blast_reentry_depth() {
	static size_t depth = 0;
	return depth;
}

/// Maximum nesting of `blast_block`'s blast-then-re-enter hop. Real formulas
/// use one level: blast once, then the re-entry finds nothing left to blast.
inline constexpr size_t max_blast_reentry_depth = 8;

/** @internal @brief RAII increment of `blast_reentry_depth`. @endinternal */
template <NodeType node>
struct blast_reentry_guard {
	blast_reentry_guard() { ++blast_reentry_depth<node>(); }
	~blast_reentry_guard() { --blast_reentry_depth<node>(); }
	blast_reentry_guard(const blast_reentry_guard&) = delete;
	blast_reentry_guard& operator=(const blast_reentry_guard&) = delete;
};

/// Maximum number of Boole (Shannon) splits one block elimination may perform
/// in total, across the whole recursion rather than along one path. Chapter 5's
/// steps 2a/2b/2h keep the split count low on the shapes they cover; this bounds
/// the residue.
///
/// It deliberately counts *splits*, not depth: the recursion branches in two, so
/// a depth bound of d still admits 2^d leaves and does not bound the work at all
/// (a 30-conjunct formula reaches minutes of runtime under a depth bound of 24).
///
/// Exhausting it costs precision, not soundness: the caller's
/// resolve_quantifiers2 -> resolve_quantifiers -> anti_prenex chain absorbs
/// whatever is left unresolved.
/// Runtime-tunable via `api::set_block_max_splits`. Resource limits belong in a
/// runtime parameter, never a header constant -- `bv_blasting`
/// (heuristics/bv_predicate_blasting.h) is the precedent. Like it, this is NOT
/// thread-safe: the tau library assumes single-threaded access.
inline size_t block_boole_max_splits = std::numeric_limits<size_t>::max();

/// Maximum rounds `process_quantifier_blocks` may take before giving up.
/// Unconditional: the termination argument at its use site is subtle enough
/// that a regression must fail loudly rather than hang Release forever.
/// Runtime-tunable via `api::set_block_max_rounds`, same caveats as above.
inline size_t block_max_rounds = std::numeric_limits<size_t>::max();

} // namespace idni::tau_lang

/**
 * @internal
 * @brief Core recursive helper for the anti-prenex block algorithm.
 *
 * Pushes a block of existential quantifier variables into a formula by splitting on disjunctions,
 * isolating dependent conjuncts, and performing Boole decomposition for
 * block variables not matched by `skip`. Once a leaf clause depends only on
 * `skip`-matched (e.g. bitvector) block variables, predicate blasting is
 * attempted on it directly (via the `blast_block` helper) instead of Boole
 * decomposition, which requires atomless-typed quantified variables.
 * @tparam node Tree node type.
 * @param formula Formula to push the quantifier block into.
 * @param block Ordered list of existentially quantified variables to push.
 * @param used_atms Set of atomic formulas already consumed in the current recursion; updated in place.
 * @param quant_pattern Map from quantified variables to their priority for BDD variable ordering.
 * @param order Variable ordering relation used by `resolve_quantifiers2`.
 * @param skip Predicate identifying variables/atomic formulas this pass must
 *        not Boole-decompose (defaults to BV-typed nodes; diverted to
 *        predicate blasting instead once nothing else can be pushed).
 * @param splits_left Remaining Boole-split budget, shared across the whole
 *        recursion and decremented by each split. Distribution over
 *        disjunctions and the step 2h gamma folds do not split and so do not
 *        charge it. On exhaustion the recursion takes the graceful re-wrap path
 *        instead of splitting further. See `block_boole_max_splits` for why this
 *        counts splits rather than depth.
 * @param elim Per-block eliminability analysis of the block's *original* body,
 *        carried down to `eliminate_block_over_clause`. Only its
 *        `verdict_of` is meaningful this deep: verdicts are keyed on the block
 *        variables, which this recursion never changes, whereas
 *        `conjuncts_of` is keyed on the original body's conjuncts and goes
 *        stale as soon as a split or a gamma fold rebuilds one. That is why
 *        `eliminate_block_over_clause` re-derives the reserved conjunct set from
 *        the clause actually in front of it instead of trusting
 *        `conjuncts_of` outright. Defaults to the empty analysis (everything
 *        eliminable), which is the pre-analysis behaviour.
 * @return Formula with the quantifier block pushed as far inward as possible.
 * @endinternal
 */
template<NodeType node>
tref anti_prenex_block(tref formula, const trefs& block,
	subtree_unordered_set<node>& used_atms,
	const auto& quant_pattern,
	const typename term_handle<node>::order& order,
	const std::function<bool(tref)>& skip,
	size_t& splits_left,
	const block_eliminability<node>& elim = block_eliminability<node>{}) {
	using tau = tree<node>;
	// Once no non-skip-matched block variable occurs free anymore, the
	// remaining leaf is entirely skip-matched (e.g. bitvector) content:
	// wrap the whole block around it and try predicate blasting instead of
	// the BDD-based elimination below, which requires atomless-typed
	// quantified variables. bv_predicate_blasting already anti-prenexes
	// each blasted atomic's own freshly-introduced auxiliary quantifiers
	// (scoped locally); the anti_prenex_block call below is a separate
	// concern: it attempts to push/resolve the block's own quantifiers
	// (still bv-typed and left untouched by blasting itself) now that the
	// scope's arithmetic has been simplified to boolean form, so nothing
	// needs to be skipped anymore.
	auto blast_block = [&](tref dep_formula) -> tref {
		tref ex_fm = dep_formula;
		for (auto v = block.rbegin(); v != block.rend(); ++v)
			ex_fm = build_wff_ex<node>(*v, ex_fm, false);
		// A closed, purely bitvector formula is decided directly by the
		// solver, same as in resolve_quantifiers/treat_ex_quantified_clause:
		// deciding the blasted form (with its many auxiliary quantifiers) is
		// much harder for it than solving the unblasted arithmetic natively.
		// Wrapping in `block`'s own quantifiers closes `ex_fm` when the only
		// surviving free variables were exactly the block's. That is not
		// guaranteed: the has_skip_content call site below reaches
		// blast_block with active variables still free, and a free variable
		// that is not a block variable at all is never bound by this wrap.
		// The explicit get_free_vars emptiness check on the next line is
		// therefore the actual guard, not an optimization.
		if (get_free_vars<node>(ex_fm).empty()
				&& is_bv_solvable_formula<node>(ex_fm)) {
			// Spelled out: bv_formula_sat_status returns an optional, and a
			// nullopt (cvc5 answered unknown, or the translation failed)
			// means undecided, not false. `auto` hid that.
			std::optional<bv_sat_status> status = bv_formula_sat_status<node>(ex_fm);
			if (status == bv_sat_status::sat) return tau::_T();
			if (status == bv_sat_status::unsat) return tau::_F();
			DBG(if (!status) LOG_ERROR << "solver undecided on " << LOG_FM(ex_fm);)
		}
		if (bv_blasting)
			if (auto blasted = bv_predicate_blasting<node>(ex_fm);
					blasted && blasted != ex_fm) {
				// Bound the hop back into the pipeline; see
				// blast_reentry_depth. Returning `blasted` rather
				// than ex_fm keeps the blasting work done so far --
				// its quantifiers simply survive, which is sound.
				if (blast_reentry_depth<node>()
					>= max_blast_reentry_depth)
				{
					LOG_ERROR << "blast_block: blast/re-enter"
						" depth " << max_blast_reentry_depth
						<< " exceeded on " << LOG_FM(ex_fm)
						<< "; returning the blasted formula"
						" without re-entering.";
					return blasted;
				}
				blast_reentry_guard<node> depth_guard;
				// Re-enter skipping bv-typed content rather than no_skip:
				// blasting rewrites arithmetic into equality/comparison
				// atoms that are still bv-typed (e.g. bit-mask equalities),
				// not atomless-typed booleans. Treating them as ordinary
				// decomposable atoms hands their elimination to the general
				// Boole-decomposition/BDD machinery, whose BDD leaves for
				// bv content are backed by actual solver terms -- orders of
				// magnitude more expensive per node than atomless (sbf)
				// BDD leaves. Keeping them skip-matched routes them back
				// through this same solver-first/blast_block path (a no-op
				// once nothing is left to blast) instead.
				return anti_prenex<node>(blasted,
					is_tref_bv_type_family<node>);
			}
		return ex_fm;
	};

	// A block variable this pass could still Boole-decompose (i.e. not
	// skip-matched). Since a skip-matched variable's quantifier commutes
	// with the rest and is left untouched either way, it must be ignored
	// here: only once none of the *active* block variables occur free
	// anymore is the leaf truly exhausted for this pass, at which point
	// the whole block (skip-matched variables included) is handed to
	// blast_block.
	// Whether `f` still contains any skip-matched (e.g. bitvector) typed
	// content at all, regardless of whether it is one of `block`'s own
	// variables. eliminate_block_over_clause assumes its whole input clause
	// is homogeneously atomless-typed (matching `block`'s type); since
	// `block` may no longer carry every skip-matched variable in scope
	// (callers may filter those out before pushing the block in here),
	// this check -- not block membership -- is what decides whether that
	// homogeneity assumption actually holds.
	// Memoized for the duration of this invocation: has_skip_content is
	// called at several sites below (including once per disjunct), and
	// find_top(skip) is a full subtree walk through an erased predicate. The
	// memo is not shared across recursion levels -- each recursive call builds
	// its own lambda -- which would need it threaded as a parameter.
	auto skip_content_memo = std::make_shared<subtree_unordered_map<node, bool>>();
	auto has_skip_content = [&skip, skip_content_memo](tref f) -> bool {
		auto& memo = *skip_content_memo;
		if (auto it = memo.find(f); it != memo.end()) return it->second;
		const bool r = tree<node>::get(f).find_top(skip) != nullptr;
		return memo.emplace(f, r).first->second;
	};

	auto has_active_var = [&](tref f) {
		const trefs& vars = get_free_vars<node>(f);
		for (tref v : block)
			if (!skip(v) && hasbc(vars, v, tau::subtree_less)) return true;
		return false;
	};

	// Does any block variable -- skip-matched or not -- occur free in `f`?
	// Skip-matched variables count here: dropping one would let its
	// conjunct be classified as independent below and moved outside the
	// block's quantifier scope, leaking the variable free.
	auto has_block_var = [&](tref f) {
		const trefs& vars = get_free_vars<node>(f);
		for (tref v : block)
			if (hasbc(vars, v, tau::subtree_less)) return true;
		return false;
	};

	// Does any block variable *other than* `except` occur free in `f`?
	// gamma4 needs this before lifting an atom out of the block's scope.
	auto has_block_var_other_than = [&](tref f, tref except) {
		const trefs& vars = get_free_vars<node>(f);
		for (tref v : block) {
			if (tau::get(v) == tau::get(except)) continue;
			if (hasbc(vars, v, tau::subtree_less)) return true;
		}
		return false;
	};

	// Goal: push the quantifier block as far into clause as possible.
	if (!has_block_var(formula)) return formula;

	// Chapter 5 steps 2a and 2b: the two whole-formula fast paths, tried
	// before any Boole decomposition (the paper's a, b, c, then d order).
	//
	//  2a -- every atom negated: distribute the block over all connectives
	//        at once (Corollary 5.1 with J1 empty) and let
	//        resolve_quantifiers2 turn each `ex X g != 0` into the
	//        functional `ex_X g != 0`.
	//  2b -- no atom negated: squeeze the positives until every
	//        conjunction is gone (distributing `&&` over `||` on terms via
	//        f1 = 0 && f2 = 0 == f1|f2 = 0), then distribute the block
	//        over the resulting disjunction.
	//
	// Both are guarded on skip content by block_atom_profile: a
	// skip-matched atom must reach blast_block / the solver, and squeezing
	// bitvector terms into a bf_or is exactly the work predicate blasting
	// must do instead.
	//
	// Returns nullptr when neither applies. Must be handed a purely
	// *dependent* formula: squeezing an independent conjunct into every
	// disjunct is sound but couples it needlessly and undoes the
	// separation the conjunction case performs.
	auto try_fast_paths = [&](tref f) -> tref {
		if (!has_active_var(f)) return nullptr;
		// guards_only: stop the census as soon as neither fast path can
		// fire. Exact for all_negated()/all_positive(), which is all this
		// caller reads.
		const block_atom_profile<node> prof = profile_block_atoms<node>(f, skip, true);
		if (prof.all_negated())
			return resolve_quantifiers2<node>(
				distribute_block_over_atoms<node>(f, block),
				order, skip);
		if (prof.all_positive()) {
			// The first *typed* block variable, then the first typed
			// node of the matrix as a fallback: collect_quantifier_block
			// lets get_ba_type() == 0 variables join a typed run, so
			// block.front() is not guaranteed to carry the run's type,
			// and squeeze_positive_disjuncts seeds its accumulator from
			// _0<node>(ba_type) without asserting anything about it.
			// This is the same scan eliminate_block_over_clause performs,
			// which is the other consumer of that assumption.
			size_t ba_type = 0;
			bool homogeneous = true;
			auto note_type = [&](size_t t) {
				if (t == 0) return;
				if (ba_type == 0) ba_type = t;
				else if (ba_type != t) homogeneous = false;
			};
			for (tref v : block) note_type(tau::get(v).get_ba_type());
			auto type_scan = [&](tref n) {
				const size_t t = tau::get(n).get_ba_type();
				if (t == 0) return true;
				note_type(t);
				return false;
			};
			pre_order<node>(f).visit(type_scan);
			// Decline rather than build a mixed-type bf_or: a matrix
			// mixing e.g. sbf and tau atoms would otherwise be combined
			// by build_bf_or under whichever type happened to come first.
			// The general algorithm below handles it correctly.
			if (!homogeneous) {
				DBG(LOG_TRACE << "try_fast_paths/2b declined: "
					"matrix is not BA-type homogeneous: "
					<< LOG_FM(f) << "\n";)
				return nullptr;
			}
			auto sq = squeeze_positive_disjuncts<node>(f, ba_type);
			// Cap exceeded: fall back to the general algorithm.
			if (!sq) return nullptr;
			// The empty disjunction is F.
			if (sq->empty()) return _F<node>();
			trefs disjs;
			disjs.reserve(sq->size());
			for (tref term : *sq) {
				tref eq = build_bf_eq_0<node>(term);
				if (has_block_var(eq))
					for (auto v = block.rbegin();
						v != block.rend(); ++v)
						eq = build_wff_ex<node>(
							*v, eq, false);
				disjs.push_back(eq);
			}
			return resolve_quantifiers2<node>(
				tau::build_wff_or(disjs), order, skip);
		}
		return nullptr;
	};

	const tau& ft = tau::get(formula);
	// For a conjunction the fast paths run further down, after the
	// dependent/independent separation has isolated the dependent part.
	if (!ft.child_is(tau::wff_and))
		if (tref fp = try_fast_paths(formula)) return fp;

	// Case disjunction
	if (ft.child_is(tau::wff_or)) {
		// Chapter 5 step 2d: distribute over the disjunction, but
		// process the disjuncts that fall into steps a, b, c *first*, so
		// a disjunct that resolves to T short-circuits the rest before
		// any of them is decomposed. The paper also asks for "unit
		// elimination after each disjunct", which is what the
		// simplification of the running accumulator below does: it is
		// how a T disjunct becomes visible as T at all.
		trefs disjs = get_dnf_wff_clauses<node>(formula);
		auto rank = [&](tref d) -> int {
			const block_atom_profile<node> p = profile_block_atoms<node>(d, skip, true);
			if (p.all_negated()) return 0;              // step 2a
			if (p.all_positive()) return 1;             // step 2b
			if (!tau::get(d).find_top(is<node, tau::wff_or>))
				return 2;                           // step 2c
			return 3;                                   // step 2e..
		};
		// Ranked once per disjunct, not once per comparison. Each rank runs a
		// full profile_block_atoms -- a find_top(skip) over the disjunct plus a
		// recursive census -- so calling it from inside the comparator cost
		// ~2*k*log k traversals for k disjuncts instead of k.
		std::vector<std::pair<int, tref>> ranked;
		ranked.reserve(disjs.size());
		for (tref d : disjs) ranked.emplace_back(rank(d), d);
		std::stable_sort(ranked.begin(), ranked.end(),
			[](const auto& a, const auto& b) { return a.first < b.first; });
		tref acc = _F<node>();
		for (const auto& [_, d] : ranked) {
			tref rd = anti_prenex_block<node>(d, block, used_atms,
				quant_pattern, order, skip, splits_left, elim);
			if (tau::get(rd).equals_T()) return _T<node>();
			acc = syntactic_path_simplification<node>(
				tau::build_wff_or(acc, rd));
			if (tau::get(acc).equals_T()) return _T<node>();
		}
		return acc;
	}

	// Case conjunction
	if (ft.child_is(tau::wff_and)) {
		trefs conjs = get_cnf_wff_clauses<node>(formula);
		trefs dep;
		for (tref &conj : conjs) {
			// Conjunct contains a quantified var
			if (has_block_var(conj)) {
				dep.push_back(conj);
				conj = _T<node>();
			}
		}
		if (dep.empty()) return formula;
		formula = tau::build_wff_and(dep);
		// Var-free conjuncts; they must be re-attached to every result
		// built from the dependent part below
		const tref indep = tau::build_wff_and(conjs);
		// Chapter 5 steps 2a/2b, now that the dependent part is
		// isolated. Handed `formula` rather than the original
		// conjunction so an independent conjunct is never squeezed into
		// every disjunct.
		if (tref fp = try_fast_paths(formula))
			return tau::build_wff_and(indep, fp);
		// Chapter 5 step 2d: distribute over disjunctions as much as
		// possible before decomposing anything. Once the independent
		// conjuncts are scoped out the dependent part can be a bare
		// disjunction; re-dispatch so the wff_or case above handles it
		// instead of falling through to Boole decomposition. The
		// recursion terminates: each disjunct is a strict subtree.
		if (tau::get(formula).child_is(tau::wff_or))
			return tau::build_wff_and(indep,
				anti_prenex_block<node>(formula, block,
					used_atms, quant_pattern, order, skip,
					splits_left, elim));
		// Check if dependent formula is clause -> push block into clause.
		// eliminate_block_over_clause assumes the whole clause is
		// homogeneously atomless-typed, so any skip-matched content in it
		// (not just among `block`'s own variables) must divert to
		// blast_block instead.
		if (!tau::get(formula).find_top(is<node, tau::wff_or>)) {
			if (!has_active_var(formula) || has_skip_content(formula))
				return tau::build_wff_and(indep, blast_block(formula));
			return tau::build_wff_and(
				indep,
				resolve_quantifiers2<node>(
				eliminate_block_over_clause<node>(formula, block, elim, order), order, skip));
		}
		// Using the available atomic formulas, do Boole decomposition on best fit
		auto is_atomic = [&used_atms, &skip](tref n) {
			if (!tau::get(n).is(tau::wff)) return false;
			// Exclude used atomic fms
			if (used_atms.contains(n)) return false;
			// Exclude atomic formulas the caller wants skipped (e.g. BV-typed)
			if (skip(n)) return false;
			const tau& c = tau::get(n)[0];
			switch (c.value.nt) {
				case tau::bf_eq:
				case tau::bf_lt:
				case tau::bf_lteq: return true;
				default: return false;
			}
		};
		// Collect current atms, applying chapter 5 step 2e's two
		// constraints on the pivot: it must be a *non-negated* atomic
		// formula, and the quantified variable must occur explicitly in
		// it.
		//
		// "Non-negated" is enforced by refusing to descend into
		// negations at all: select_top_until's `until` predicate
		// suppresses both selection of a node and descent below it, so
		// the equation inside a `!(f = 0)` is never reachable.
		//
		// "x occurs in f" is enforced by dropping candidates with no
		// active block variable. The ranking already sorts those last,
		// but it would still pick one when nothing better exists, and
		// such a split cannot make progress toward eliminating the
		// block.
		//
		// Equations are preferred over order atoms: only a bf_eq can
		// ever let eliminate_block_over_clause remove the block (a bf_lt
		// hits its unrecognized-conjunct path and forces a re-wrap), so
		// splitting on an order atom doubles the formula without ever
		// paying for itself. Order atoms remain available as a last
		// resort rather than being dropped outright.
		// Temporal quantifiers stop the scan as well. wff_always/wff_sometimes
		// are not terms, so select_top_until would otherwise descend into them
		// and could return an atom from inside a temporal scope; the split
		// below then replaces that atom everywhere via rewriter::replace and
		// guards each branch with a time-*independent* copy of it, which is
		// invalid when the atom's truth varies over time. The fast paths
		// already decline on such a shape (profile_block_atoms_rec classifies
		// wff_always as `others`), so only this decomposition path was exposed.
		// Whether a temporal operator can sit below a first-order quantifier
		// block at this point is not established -- normalize strips them
		// first for well-formed input -- so this is a free guard, not a
		// reproduced bug fix.
		auto stop_at = [](tref n) {
			return is_quantifier<node>(n)
				|| is_temporal_quantifier<node>(n)
				|| is<node, tau::wff_neg>(n);
		};
		const trefs candidates = rewriter::select_top_until<node>(
			formula, is_atomic, stop_at);
		trefs eq_atms, ord_atms;
		for (tref a : candidates) {
			if (!has_active_var(a)) continue;
			if (tau::get(a)[0].is(tau::bf_eq)) eq_atms.push_back(a);
			else ord_atms.push_back(a);
		}
		const trefs& atms = eq_atms.empty() ? ord_atms : eq_atms;
		// No decomposable atomic formula available (e.g. only !=
		// equations, quantified subformulas, or exclusively skip-matched
		// atoms remain): try blasting if the block needs it, else keep
		// the block on the dependent part instead of dereferencing end()
		if (atms.empty() || splits_left == 0) {
			DBG(if (splits_left == 0)
				LOG_TRACE << "anti_prenex_block: Boole split"
					" budget exhausted, re-wrapping block\n";)
			if (!has_active_var(formula))
				return tau::build_wff_and(indep, blast_block(formula));
			for (auto v = block.rbegin(); v != block.rend(); ++v)
				formula = build_wff_ex<node>(*v, formula, false);
			return tau::build_wff_and(indep, formula);
		}
		// Sort the atomic formulas and get minimum
		tref atm = *std::ranges::min_element(atms,
			atm_formula_order_for_quant_elim<node>(quant_pattern));

		// Chapter 5 step 2h: the reduced Boole form. Four of the five
		// gamma cases are settled by cofactoring the pivot on the
		// highest-priority active block variable occurring in it, and
		// none of them needs the general recursion of gamma5.
		//
		// Only an *active* (non-skip) variable and a non-skip atom are
		// cofactored: this is Boolean reasoning, exactly what `skip`
		// reserves for predicate blasting and the solver.
		tref pivot_var = nullptr;
		if (!skip(atm)) {
			int_t best = -1;
			const trefs& avars = get_free_vars<node>(atm);
			for (tref v : block) {
				if (skip(v)) continue;
				if (!hasbc(avars, v, tau::subtree_less))
					continue;
				auto it = quant_pattern.find(v);
				const int_t pr = it == quant_pattern.end()
					? 0 : it->second;
				if (pr > best) best = pr, pivot_var = v;
			}
		}
		if (pivot_var) {
			const boole_atom_analysis<node> an =
				analyze_boole_atom<node>(atm, pivot_var);
			// gamma2: f is identically 0, so the atom holds for
			// every value of the variable -- fold it to T.
			if (an.kind == boole_atom_case::identically_zero)
				return tau::build_wff_and(indep,
					anti_prenex_block<node>(
						rewriter::replace<node>(
							formula, atm, tau::_T()),
						block, used_atms, quant_pattern,
						order, skip, splits_left, elim));
			// gamma3: f is identically 1, so the atom holds for
			// no value of the variable -- fold it to F.
			if (an.kind == boole_atom_case::identically_one)
				return tau::build_wff_and(indep,
					anti_prenex_block<node>(
						rewriter::replace<node>(
							formula, atm, tau::_F()),
						block, used_atms, quant_pattern,
						order, skip, splits_left, elim));
			// gamma4: f does not depend on the variable, so the
			// atom does not constrain it. Lift the atom out of the
			// block's scope instead of carrying it into both
			// branches of a split.
			if (an.kind == boole_atom_case::independent) {
				tref out_atm = rewriter::replace<node>(atm,
					tau::get(tau::bf,
						tau::trim_right_sibling(
							pivot_var)),
					tau::_0(find_ba_type<node>(pivot_var)));
				// `out_atm` is conjoined at *this* level while the
				// block's binders are only re-attached inside the
				// recursive results below, so any other block
				// variable still present in it would end up free.
				// analyze_boole_atom only establishes independence
				// from `pivot_var`, not from the rest of the
				// block, so check the rest explicitly and fall
				// through to the general split when the lift is
				// not safe. gamma1 guards against the same hazard
				// more coarsely, with `active_vars == 1` (see its
				// TODO below).
				if (!has_block_var_other_than(out_atm, pivot_var)) {
				tref gl = syntactic_path_simplification_unsat_on_unchanged_negations
					<node>(rewriter::replace<node>(
						formula, atm, tau::_T()));
				tref gr = syntactic_path_simplification_unsat_on_unchanged_negations
					<node>(rewriter::replace<node>(
						formula, atm, tau::_F()));
				used_atms.insert(atm);
				tref pl = anti_prenex_block<node>(gl, block,
					used_atms, quant_pattern, order, skip,
					splits_left, elim);
				tref pr2 = anti_prenex_block<node>(gr, block,
					used_atms, quant_pattern, order, skip,
					splits_left, elim);
				used_atms.erase(atm);
				return tau::build_wff_and(indep,
					tau::build_wff_or(
						tau::build_wff_and(out_atm, pl),
						tau::build_wff_and(
							tau::build_wff_neg(
								out_atm),
							pr2)));
				}
				// Otherwise out_atm still mentions another block
				// variable: lifting it would leak that variable
				// free, so leave this atom to the general split.
			}
			// gamma1: f has a unique zero, so the T-branch has
			// exactly one witness, pivot_var := f[x<-0].
			// Substituting it removes the variable outright instead
			// of leaving a quantifier for the BDD stage.
			//
			// TODO (MEDIUM): restricted to blocks whose only active
			// variable is the pivot. Applying it inside a larger
			// block needs `block`, `order` and `quant_pattern`
			// rebuilt without pivot_var before recursing, otherwise
			// the remaining variables are resolved against an order
			// that still mentions the substituted one.
			size_t active_vars = 0;
			for (tref v : block) if (!skip(v)) ++active_vars;
			if (an.kind == boole_atom_case::unique_zero
				&& active_vars == 1)
			{
				const tref bf_pivot = tau::get(tau::bf,
					tau::trim_right_sibling(pivot_var));
				tref wl = rewriter::replace<node>(
					formula, atm, tau::_T());
				wl = rewriter::replace<node>(
					wl, bf_pivot, an.cofactor_0);
				wl = syntactic_path_simplification_unsat_on_unchanged_negations
					<node>(wl);
				// The F-branch needs ¬atm CONJOINED, exactly as the
				// general split below does (`¬atm && r`): replacing the
				// atom's occurrences by F does not make the atom false,
				// and a constraint that lived only in those occurrences
				// vanishes with them. Found by ground evaluation on
				// `ex q (!(q = 0) && (a q = 0 || q = 0))`: γ1 fires on
				// the pivot `q = 0`, the F-branch became `a q = 0`
				// (the `q != 0` constraint gone), and the elimination
				// answered T where the truth is `a' != 0`. This was the
				// wrong-F the legacy fallback had been repairing on the
				// issue #70 interpreter spec. Legacy's own γ1 analogue
				// conjoins `¬boole_atm`, so this now matches it.
				tref wr = syntactic_path_simplification_unsat_on_unchanged_negations
					<node>(tau::build_wff_and(
						tau::build_wff_neg(atm),
						rewriter::replace<node>(
							formula, atm, tau::_F())));
				used_atms.insert(atm);
				tref rr = anti_prenex_block<node>(wr, block,
					used_atms, quant_pattern, order, skip,
					splits_left, elim);
				used_atms.erase(atm);
				// The T-branch is quantifier-free by
				// construction: pivot_var was substituted out
				// and it was the block's only active variable.
				return tau::build_wff_and(indep,
					tau::build_wff_or(wl, rr));
			}
		}

		// Remove/add available atomic formulas in stack-like fashion
		used_atms.insert(atm);
		// Do Boole decomposition.
		// Paper assumption 2: the branch formulas are simplified before
		// recursing. The node hooks only fold constants, so a
		// contradiction between two *other* atoms on the same path
		// survives into the recursion and keeps atoms alive that the
		// decomposition then splits on for nothing. This is the same
		// pass legacy ex_quantified_boole_decomposition applies.
		tref l = syntactic_path_simplification_unsat_on_unchanged_negations
			<node>(rewriter::replace<node>(formula, atm, tau::_T()));
		tref r = syntactic_path_simplification_unsat_on_unchanged_negations
			<node>(rewriter::replace<node>(formula, atm, tau::_F()));
		if (tau::get(l) == tau::get(r)) {
			tref res = anti_prenex_block(
				l, block, used_atms, quant_pattern, order, skip,
				splits_left, elim);
			used_atms.erase(atm);
			return tau::build_wff_and(indep, res);
		}
		// One split, charged once against the shared budget.
		--splits_left;
		tref nl = anti_prenex_block(
			tau::build_wff_and(atm, l), block, used_atms,
			quant_pattern, order, skip, splits_left, elim);
		if (tau::get(nl).equals_T()) {
			used_atms.erase(atm);
			// dep part is T, only the var-free part remains
			return indep;
		}
		tref nr = anti_prenex_block(
			tau::build_wff_and(tau::build_wff_neg(atm), r),
				block, used_atms, quant_pattern, order, skip,
				splits_left, elim);
		used_atms.erase(atm);
		return tau::build_wff_and(indep, tau::build_wff_or(nl, nr));
	}

	// Connective is not wff_and or wff_or (e.g. a single atom or a wff_ref)
	// -> try blasting if the block needs it, else quantifiers stay with formula
	if (!has_active_var(formula))
		return blast_block(formula);
	for (auto v = block.rbegin(); v != block.rend(); ++v)
		formula = build_wff_ex<node>(*v, formula, false);
	return formula;
}

/**
 * @internal
 * @brief Convenience overload of the core block recursion that owns a fresh
 * Boole-split budget.
 *
 * Equivalent to the 7-arg form with `splits_left = block_boole_max_splits`. Use
 * it when there is no enclosing elimination whose budget should be shared. It
 * carries no eliminability analysis either, so `eliminate_block_over_clause` sees
 * the empty one (everything eliminable) -- the pre-analysis behaviour.
 * @tparam node Tree node type.
 * @endinternal
 */
template<NodeType node>
tref anti_prenex_block(tref formula, const trefs& block,
	subtree_unordered_set<node>& used_atms,
	const auto& quant_pattern,
	const typename term_handle<node>::order& order,
	const std::function<bool(tref)>& skip)
{
	size_t splits_left = block_boole_max_splits;
	return anti_prenex_block<node>(formula, block, used_atms, quant_pattern,
		order, skip, splits_left);
}

/**
 * @internal
 * @brief One maximal quantifier block: the run of consecutive quantifiers
 * that `process_quantifier_block` eliminates jointly, plus the matrix it
 * scopes over.
 *
 * "Maximal" is bounded by the three things the block elimination requires to
 * be uniform across the block:
 *  - the quantifier kind (∃ vs ∀): only same-kind quantifiers commute, so a
 *    kind change starts a new, inner block;
 *  - the Boolean-algebra type of the bound variables: the block is resolved
 *    through a single BDD variable order and a single
 *    `eliminate_block_over_clause` call, both of which assume one BA type (the
 *    latter asserts it), so a type change likewise starts a new block;
 *  - `skip`-matched (e.g. bitvector) variables, which this pass never
 *    eliminates and which therefore never enter the active block.
 * @tparam node Tree node type.
 * @endinternal
 */
template<NodeType node>
struct quantifier_block {
	// Node the run hangs off: its direct child is the outermost quantifier
	// of the block. This is the node the eliminated result replaces in the
	// enclosing tree.
	tref head = nullptr;
	// Active (non-skip-matched) bound variables, outermost first. All of
	// one quantifier kind and one Boolean-algebra type.
	trefs vars;
	// skip-matched quantifiers displaced from within the run, outermost
	// first, each paired with its own kind (true = ∃). Re-wrapped around
	// the result by process_quantifier_block.
	std::vector<std::pair<tref, bool>> skipped;
	// Kind of the block itself (true = ∃). Taken from the first active
	// quantifier of the run, not from the first quantifier: a leading
	// skip-matched quantifier is transparent and must not decide the kind
	// of the block that follows it.
	bool is_ex = false;
	// Matrix the whole run scopes over, as found in the tree.
	tref body = nullptr;
};

/**
 * @internal
 * @brief Collects the maximal quantifier block whose head is `n`.
 *
 * Walks the quantifier chain down from `n` (whose direct child must be a
 * quantifier), accumulating bound variables until the chain ends or reaches
 * a quantifier that cannot join the block (see `quantifier_block`).
 * `skip`-matched quantifiers are transparent: they neither end the block nor
 * join it. They are recorded in `skipped` and re-wrapped later, so that
 * active quantifiers sitting below them still join *this* block instead of
 * being stranded in a separate one of their own.
 * @tparam node Tree node type.
 * @param n Formula node whose direct child is a quantifier.
 * @param skip Predicate marking variables this pass must not eliminate.
 * @return The collected block, including the matrix it scopes over.
 * @endinternal
 */
template<NodeType node>
quantifier_block<node> collect_quantifier_block(tref n,
	const std::function<bool(tref)>& skip)
{
	using tau = tree<node>;
	DBG(assert(is_child_quantifier<node>(n));)
	quantifier_block<node> blk;
	blk.head = n;
	size_t ba_type = 0;
	bool ba_type_set = false;
	bool kind_fixed = false;
	tref curr = n;
	while (is_child_quantifier<node>(curr)) {
		const bool curr_is_ex = is_child<node>(curr, tau::wff_ex);
		tref var = tau::trim2(curr);
		if (skip(var)) {
			// Transparent only while it genuinely commutes with the
			// block. Same-kind quantifiers do; opposite-kind ones do
			// not -- `ex a all x phi` is not `all x ex a phi`, and
			// wrap_skipped re-emits everything in `skipped`
			// outermost, so absorbing an opposite-kind quantifier
			// here would hoist it out past the block. Disjoint BA
			// types do not rescue it: with `a` atomless and `x` a
			// bitvector, phi = (a = 0 <-> x = 0) makes
			// `all x ex a phi` true and `ex a all x phi` false.
			//
			// A skipped quantifier seen *before* any active one is
			// always safe: it stays outermost either way.
			//
			// Breaking is self-healing: blk.body then starts with
			// that quantifier and select_innermost_blocks picks it
			// up as an inner block on a later round.
			if (kind_fixed && curr_is_ex != blk.is_ex) break;
			blk.skipped.emplace_back(var, curr_is_ex);
			// Provisional only: a run of nothing but skip-matched
			// quantifiers has no active kind of its own, and
			// process_quantifier_block still needs one to pick a
			// branch. An active quantifier below overrides this.
			if (!kind_fixed) blk.is_ex = curr_is_ex;
		} else {
			// The first *active* quantifier fixes the block's kind,
			// so a leading skip-matched quantifier of the opposite
			// kind cannot split the run that follows it.
			if (!kind_fixed) blk.is_ex = curr_is_ex, kind_fixed = true;
			else if (curr_is_ex != blk.is_ex) break;
			// get_ba_type() == 0 means *untyped*, not "a distinct
			// type": eliminate_block_over_clause's own homogeneity
			// check ignores type-0 nodes for exactly that reason.
			// Treating 0 as a type of its own would split a run
			// whenever one variable happens to be untyped,
			// degrading block elimination back into the
			// single-quantifier elimination it exists to avoid.
			if (const size_t vt = tau::get(var).get_ba_type();
				vt > 0)
			{
				if (ba_type_set && vt != ba_type) break;
				ba_type = vt, ba_type_set = true;
			}
			blk.vars.push_back(var);
		}
		curr = tau::get(curr)[0].second();
	}
	blk.body = curr;
	return blk;
}

/**
 * @internal
 * @brief Eliminates one maximal quantifier block over an already-processed matrix.
 *
 * Delegates the whole block (whether atomless, bitvector-typed, or mixed)
 * to the 6-arg `anti_prenex_block`, which splits disjunctions/conjuncts,
 * Boole-decomposes atomless content, and attempts predicate blasting on
 * whatever leaf clause still needs `skip`-matched (e.g. bitvector) content
 * resolved. There is no further fallback: the legacy step-based algorithm
 * that used to catch residual quantifiers here was deleted 2026-08-04 (see
 * the note in `resolve_ex_block`). ∃-blocks are pushed in directly;
 * ∀-blocks are dualized to ∃-blocks.
 *
 * `blk.body` is required to already be free of unresolved quantifier scope:
 * the pipeline below (`eliminate_block_over_clause` in particular) assumes it
 * is handed fully resolved, homogeneously typed content. `process_quantifier_blocks`
 * guarantees this by only ever eliminating innermost blocks.
 * @tparam node Tree node type.
 * @param blk The maximal block to eliminate, as returned by `collect_quantifier_block`.
 * @param skip Predicate marking variables this pass must not eliminate.
 * @param ctx_bv_is_solver_owned Formula-wide input of the eliminability
 *        analysis: `false` when the formula holds a constant of a Boolean
 *        algebra cvc5 cannot translate, in which case no bitvector scope
 *        anywhere in it will ever be decided by the solver. A plain `bool`
 *        rather than the `analysis_context` it ends up in, deliberately:
 *        short-lived churn that Task 9 replaces with the full context once the
 *        normalizer stops computing this separately.
 * @return Formula with the quantifier block eliminated or pushed inward.
 * @endinternal
 */
// Processes one quantifier block:
// - ∃-blocks: push into body with the 6-arg anti_prenex_block (which
//   handles skip-matched/bitvector content itself via blasting), resolve
//   remaining quantifiers, then fall back to anti_prenex for whatever
//   structural (non-bitvector) cases still need it. ∀-blocks are dualized:
//   ∀x φ ≡ ¬∃x ¬φ.
template<NodeType node>
tref process_quantifier_block(const quantifier_block<node>& blk,
	const std::function<bool(tref)>& skip = is_tref_bv_type_family<node>,
	bool ctx_bv_is_solver_owned = true)
{
	using tau = tree<node>;
	trefs block_vars = blk.vars;
	tref body = blk.body;
	const bool is_ex = blk.is_ex;

	// Re-wrap quantifiers displaced by the collection above around a
	// result, innermost displaced quantifier first, so the
	// outermost-encountered one ends up outermost again -- valid
	// regardless of order since they commute with the rest.
	auto wrap_skipped = [&](tref r) -> tref {
		for (auto it = blk.skipped.rbegin(); it != blk.skipped.rend(); ++it)
			r = it->second ? build_wff_ex<node>(it->first, r, false)
				: build_wff_all<node>(it->first, r, false);
		return r;
	};

	// Trivial Skolemization: if every variable in this block is trivially
	// eliminable (see heuristics/trivial_skolem.h), skip BDD ordering,
	// Boole decomposition, and predicate blasting below entirely. Only
	// valid for ∃-blocks. normalize_atomic_formula_operators first so
	// bf_neq atoms (e.g. from an already-resolved inner block) are
	// canonical bf_eq before trivial_skolem_ex's bf_eq-only matcher looks
	// at them.
	//
	// Only adopt the result when ALL block variables were eliminated
	// (trivial_skolem_ex then returns the bare matrix, with no
	// re-quantification at all).
	//
	// The redesign plan's Task 7 proposed adopting a *partial* result too,
	// on the reading that the discard existed solely to dodge
	// build_wff_ex_many's capture-unsafe rename of survivors, and that
	// canonical ids (step 0 of the pipeline) had made that rename moot.
	// Measured 2026-08-04, both halves of that turned out wrong:
	//
	//  - `if (simplified != body) return wrap_skipped(simplified);`, the
	//    plan's exact replacement, fails four tests. There is a SECOND
	//    reason for the discard the plan did not account for: returning a
	//    partial result short-circuits the rest of this function, which is
	//    what would have eliminated the survivors. Adopting it trades a
	//    fully eliminated block for a partly eliminated one.
	//  - Continuing from the simplified body instead of returning it does
	//    keep the suite green -- and never once fires. Instrumented across
	//    test_antiprenexing, test_leaf_clause, satisfiability1,
	//    wff_normalization and splitter, trivial_skolem_ex returned a
	//    partial result exactly zero times. There is nothing here to adopt.
	//
	// So the discard stays. Do not "fix" it without an input that actually
	// produces a partial trivial-Skolem result.
	if (is_ex) {
		tref simplified = trivial_skolem_ex<node>(block_vars,
			normalize_atomic_formula_operators<node>(body));
		if (!is_child<node>(simplified, tau::wff_ex))
			return wrap_skipped(simplified);
	}

	// Build BDD variable order (innermost = lowest index = highest priority).
	typename term_handle<node>::order ord;
	for (size_t i = 0; i < block_vars.size(); ++i)
		ord.emplace(block_vars[block_vars.size() - 1 - i],
			static_cast<int_t>(i));

	// Build quant_pattern (outermost = highest priority for Boole decomposition).
	subtree_unordered_map<node, int_t> qp;
	for (size_t i = 0; i < block_vars.size(); ++i)
		qp.emplace(block_vars[i],
			static_cast<int_t>(block_vars.size() - i));

	// Run the core ∃-block elimination pipeline on a pre-normalized body b.
	// normalize_atomic_formula_operators is called here so the block core's
	// own conjunct classification (try_fast_paths, profile_block_atoms) sees
	// canonical atoms. eliminate_block_over_clause no longer depends on it:
	// it establishes whichever spelling each of its two squeeze paths needs,
	// locally -- and they need opposite ones.
	auto resolve_ex_block = [&](tref b) -> tref {
		b = normalize_atomic_formula_operators<node>(b);
		// Per-block eliminability analysis, computed over the body this
		// elimination is actually about to run on (and, for a ∀-block,
		// over the *negated* body -- that is the formula the ∃ pipeline
		// below sees). Only a bound variable's own scope can constrain
		// it, so the block body is as informative as the whole formula
		// and strictly fresher.
		analysis_context<node> actx;
		actx.bv_is_solver_owned = ctx_bv_is_solver_owned;
		const block_eliminability<node> elim = analyse_block<node>(
			block_vars, get_cnf_wff_clauses<node>(b), actx);
		subtree_unordered_set<node> used_atms;
		// One budget per block elimination, shared by its whole
		// recursion.
		size_t splits_left = block_boole_max_splits;
		tref r = anti_prenex_block<node>(b, block_vars, used_atms, qp,
			ord, skip, splits_left, elim);
		r = resolve_quantifiers2<node>(r, ord, skip);
		// Fallback for closed bv (sub-)formulas resolve_quantifiers2 leaves
		// untouched, and a final safety net for blasting failures: try
		// CVC5/blasting once more via the whole-tree resolve_quantifiers.
		// find_top_until, not find_top: tau_ba sub-trees carry their own
		// wff_ex/wff_all over I/O variables (see select_innermost_blocks),
		// so a plain find_top is permanently true for any formula holding
		// such a constant -- and then resolve_quantifiers plus a full
		// anti_prenex run on every block, even one that resolved cleanly.
		// anti_prenex_block's own short-circuit already uses this form.
		auto has_live_quantifier = [](tref m) {
			return tree<node>::get(m).find_top_until(
				is_quantifier<node>,
				[](tref k) { return !while_is_formula<node>(k); })
					!= nullptr;
		};
		if (has_live_quantifier(r))
			r = resolve_quantifiers<node>(r);
		// No further fallback. The legacy step-based `anti_prenex` used
		// to run here on whatever still carried a quantifier, and was
		// deleted 2026-08-04 once both full suites passed without it
		// (319/319, Debug and Release). The experiment that proved it and
		// the three capabilities that had to be built first -- the
		// eliminability partition, the leaf_clause merge, and the γ1
		// `¬atm` fix above (the wrong answer the fallback had been
		// silently repairing, pinned by Gamma1NegatedBranch) -- are in
		// this file's history at the deletion commit. Known cost, measured
		// then: test_integration-satisfiability2 ~17-20x slower than with
		// the fallback (47 s vs 2.7 s Release); everything else at
		// baseline. A quantifier this path cannot discharge now simply
		// survives -- callers treat that as undecided, never as false.
		return r;
	};

	tref result;
	if (is_ex) {
		result = resolve_ex_block(body);
	} else {
		// ∀-block: dualize to ∃-block on negated body, then negate result.
		tref pushed = resolve_ex_block(
			to_nnf<node>(tau::build_wff_neg(body)));
		result = normalize_atomic_formula_operators<node>(
			to_nnf<node>(tau::build_wff_neg(pushed)));
	}
	return wrap_skipped(result);
}

/**
 * @internal
 * @brief Collects the innermost maximal quantifier blocks of `fm`.
 *
 * A *block head* is a node whose direct child is a quantifier; the block it
 * heads is the whole run `collect_quantifier_block` walks down from it. A
 * head is *innermost* when the matrix that run scopes over holds no
 * quantifier scope of its own -- that is, when its block can be eliminated
 * over an already-resolved body right now.
 *
 * Heads listed in `done` are neither emitted nor descended into: they have
 * already been handed to `process_quantifier_block`, so re-processing them
 * would repeat work that has, by definition, converged. Skipping them
 * instead promotes their *enclosing* head to innermost, which is precisely
 * what lets an outer run absorb quantifiers an inner block left behind.
 * @tparam node Tree node type.
 * @param fm Formula to scan.
 * @param skip Predicate marking variables this pass must not eliminate.
 * @param done Heads already processed in this pass.
 * @param out Collected blocks, appended to.
 * @endinternal
 */
template<NodeType node>
void select_innermost_blocks(tref fm, const std::function<bool(tref)>& skip,
	const subtree_unordered_set<node>& done,
	std::vector<quantifier_block<node>>& out)
{
	// Do not descend into terms: tau_ba sub-trees carry their own internal
	// wff_ex/wff_all over I/O variables, which this pass must leave intact
	// (the same guard anti_prenex_block's short-circuit uses).
	const trefs heads = rewriter::select_top_until<node>(fm,
		is_child_quantifier<node>,
		[](tref n) { return !while_is_formula<node>(n); });
	for (tref head : heads) {
		quantifier_block<node> blk =
			collect_quantifier_block<node>(head, skip);
		if (done.contains(head)) {
			// Retired, but chapter 5 step 2k asks for step 2 to be
			// re-entered wrt *every* quantifier in the resulting
			// expression: a block nested inside this one's result
			// has never been seen. Descend without re-emitting the
			// head itself.
			select_innermost_blocks<node>(blk.body, skip, done, out);
			continue;
		}
		const size_t before = out.size();
		select_innermost_blocks<node>(blk.body, skip, done, out);
		// Nothing nested under the run: this block is innermost.
		if (out.size() == before) out.push_back(std::move(blk));
	}
}

/**
 * @internal
 * @brief Eliminates every maximal quantifier block in `fm`, innermost first.
 *
 * Block elimination is only worth its name when a whole maximal run of
 * quantifiers reaches `process_quantifier_block` at once: the run is
 * resolved through one joint BDD variable order and one Boole-decomposition
 * pivot ranking over all of its variables. Splitting a run across several
 * calls degrades this to repeated single-quantifier elimination -- the very
 * thing the block algorithm exists to avoid.
 *
 * Two requirements pull in opposite directions:
 *  - a block must be collected *maximally*, so no quantifier that could join
 *    the run is eliminated on its own beforehand;
 *  - a block must be eliminated over a *resolved* matrix, since a clause
 *    holding an unresolved, foreign-typed quantifier scope has no top-level
 *    conjunct structure for `eliminate_block_over_clause` to squeeze.
 *
 * A plain `post_order` traversal satisfies neither: it rewrites a node only
 * after its children, so by the time the outermost quantifier of
 * `ex x ex y ex z φ` is reached, `y` and `z` have already been peeled off and
 * eliminated one at a time -- the maximal block is destroyed before it can
 * ever be seen. Collecting the run top-down and recursing into its matrix
 * fixes that but only half-way: the run is then fixed *before* the matrix is
 * resolved, so quantifiers that resolving the matrix leaves exposed directly
 * beneath the run (residuals of the `anti_prenex` fallback, or quantifiers
 * uncovered when an intervening scope vanishes) never join it, and get
 * eliminated separately -- single-quantifier elimination again, just one
 * level up.
 *
 * This driver therefore works strictly bottom-up and *re-collects* after
 * every splice. Each round takes the currently innermost blocks
 * (`select_innermost_blocks`), eliminates each over its already-resolved
 * matrix, and splices the results back through one `changes` map -- the
 * usual cached-rewrite pattern, leaving all node rebuilding to the rewriter.
 * The next round then re-collects blocks from the *updated* tree, so any run
 * that inner elimination made contiguous is now seen, and eliminated, as the
 * single maximal block it has become. Rounds continue upward until no
 * unprocessed block remains.
 *
 * Termination: `done` retires both the head handed to
 * `process_quantifier_block` and the node that replaces it, so every tree
 * position is processed exactly once however the splice renames it. A round
 * that finds only retired heads collects nothing and returns; a round that
 * finds new ones retires them too, and the heads reachable from a finite tree
 * are finite. Retiring the result is what makes this hold: without it, a run
 * of nothing but skip-matched quantifiers -- which has no active variable to
 * eliminate, and which `wrap_skipped` therefore reproduces around a possibly
 * perturbed body -- would head the same block again under a new tref every
 * round, and the loop would not terminate.
 * @tparam node Tree node type.
 * @param fm Formula to process.
 * @param skip Predicate marking variables this pass must not eliminate.
 * @param ctx_bv_is_solver_owned Formula-wide input of the eliminability
 *        analysis; see `process_quantifier_block`. Passed straight through.
 * @return Formula with every maximal quantifier block eliminated or pushed inward.
 * @endinternal
 */
template<NodeType node>
tref process_quantifier_blocks(tref fm, const std::function<bool(tref)>& skip,
	bool ctx_bv_is_solver_owned = true)
{
	using tau = tree<node>;
	subtree_unordered_set<node> done;
	// A quantifier over a constant scope can appear after its own round: an
	// outer elimination can reduce a kept inner quantifier's scope to T/F,
	// and syntactic_formula_simplification does not fold binders. Sound
	// under the standing non-empty domain assumption. Deliberately NOT
	// fold_trivial_quantifiers -- normalizer.tmpl.h:128 records that it
	// breaks bitwidth preservation.
	auto drop_const_quant = [](tref m) -> tref {
		if (!is_child_quantifier<node>(m)) return m;
		tref scoped = tau::get(m)[0].second();
		if (tau::get(scoped).equals_T() || tau::get(scoped).equals_F())
			return scoped;
		return m;
	};
	// The cap is unconditional: the termination argument documented below is
	// subtle enough that a regression in it must fail loudly rather than hang
	// Release forever. The DBG assert stays so a Debug run stops at the
	// offending formula instead of silently returning it unprocessed.
	// Read once into a local: a caller retuning it mid-pass would otherwise
	// move the goalposts between the assert and the comparison.
	const size_t max_rounds = block_max_rounds;
	size_t rounds = 0;
	for (;;) {
		DBG(assert(rounds < max_rounds
			&& "process_quantifier_blocks did not converge");)
		if (++rounds > max_rounds) {
			LOG_ERROR << "process_quantifier_blocks: no convergence "
				"after " << max_rounds << " rounds, returning the "
				"formula unprocessed: " << LOG_FM(fm);
			return fm;
		}
		std::vector<quantifier_block<node>> blocks;
		select_innermost_blocks<node>(fm, skip, done, blocks);
		DBG(LOG_TRACE << "process_quantifier_blocks round " << rounds
			<< ": " << blocks.size() << " block(s)\n";)
		if (blocks.empty())
			return post_order<node>(fm).apply_unique(
				drop_const_quant);
		subtree_map<node, tref> changes;
		for (const quantifier_block<node>& blk : blocks) {
			done.insert(blk.head);
			tref res = process_quantifier_block<node>(blk, skip,
				ctx_bv_is_solver_owned);
			// Retire the result as well, not just the head it
			// replaces. A block whose run is entirely skip-matched
			// has no active variable to eliminate, so
			// process_quantifier_block re-wraps those quantifiers
			// around a body the pipeline may have perturbed: the
			// spliced-in node then heads the very same block again
			// under a fresh tref, which `done` would not recognise,
			// and the round would repeat forever. Retiring `res`
			// makes "processed once" hold for the position, not
			// merely for one node identity. Merging upward is
			// unaffected -- it happens at the enclosing head, a
			// different node, whose own collection walks straight
			// through whatever quantifiers `res` still carries.
			//
			// `done` is sibling-insensitive, so this retires every
			// structurally identical occurrence of `res`, not just
			// this position. That costs precision -- another block
			// headed by the same subtree elsewhere is only descended
			// into, never emitted, so its quantifiers survive -- and
			// is accepted deliberately: keying on (position, node)
			// would break the termination argument above.
			done.insert(res);
			if (res != blk.head) changes.emplace(blk.head, res);
		}
		// Every innermost block converged, so the tree is unchanged --
		// but the blocks *enclosing* them have not been processed yet.
		// Retiring this round's heads is what promotes those to
		// innermost, so keep going instead of returning.
		if (changes.empty()) continue;
		fm = rewriter::replace_if<node>(fm, changes,
			while_is_formula<node>);
	}
}

/**
 * @internal
 * @brief Convenience overload: the pipeline with the default `skip`
 * (`is_tref_bv_type_family`). See the 2-argument overload below for what it
 * actually does.
 * @tparam node Tree node type.
 * @param formula Formula to process.
 * @return Formula with quantifiers pushed inward as far as possible using the
 * block-based algorithm.
 * @endinternal
 */
template<NodeType node>
tref anti_prenex(tref formula) {
	return anti_prenex<node>(formula, is_tref_bv_type_family<node>);
}

/**
 * @internal
 * @brief Drives the full anti-prenex-block pipeline: NNF + syntactic
 * simplification, substitution-based elimination, canonical operator
 * normalization, then maximal-block application of `process_quantifier_block`.
 *
 * @warning `skip` is honoured by the block machinery itself
 * (`collect_quantifier_block`, `try_fast_paths`, `profile_block_atoms`,
 * `blast_block`) but **not** by every step of this pipeline:
 * `syntactic_formula_simplification` takes no skip predicate at all, and its
 * `simplify_using_equality` step ends in `to_nnf`, which does rewrite
 * bitvector comparison atoms. Nor do the two fallbacks `resolve_ex_block`
 * reaches for -- `resolve_quantifiers` and `anti_prenex` -- accept one.
 * @tparam node Tree node type.
 * @param formula Formula to process.
 * @param skip Predicate marking content this pass must not Boole-decompose.
 * @return Formula with quantifiers pushed inward as far as possible.
 * @endinternal
 */
template<NodeType node>
tref anti_prenex(tref formula, const std::function<bool(tref)>& skip) {
	using tau = tree<node>;

	// Short-circuit: quantifier-free formulas need no processing here.
	// to_nnf and syntactic_formula_simplification can alter tau_ba
	// sub-formulas that the caller expects to be left intact.
	// Use find_top_until to avoid descending into tau_ba sub-trees, which
	// may contain wff_all/wff_ex nodes over I/O variables internally.
	if (!tau::get(formula).find_top_until(is_quantifier<node>,
		[](tref n) { return !while_is_formula<node>(n); }))
		return formula;

	// Formula-wide input of the per-block eliminability analysis, computed
	// once here at pipeline entry and carried down to every block. A
	// constant of another Boolean algebra (a `:tau` spec constant, say) is
	// one cvc5 cannot translate at all, so *no* bitvector scope anywhere in
	// this formula will ever be decided by the solver, and `solver_owned`
	// must not be seeded. Read off the incoming formula: none of the steps
	// below can introduce a foreign BA constant that is not already there.
	const bool ctx_bv_is_solver_owned = !has_foreign_ba_constant<node>(formula);

	// Step 0: canonicalise quantifier ids once, here, so nothing below has
	// to rename. Every binder this pass builds therefore passes
	// `calculate_quant_id = false`, which also stops `find_biggest_quant_id`
	// -- a full pre_order walk -- from running once per binder built, which
	// is quadratic in nesting depth in the split-heavy core.
	//
	// `canonize_quantifier_ids` maintains a *stack* per original variable,
	// so shadowing is handled correctly; `build_wff_ex_many`'s flat rename
	// map is not, which is what made a partial trivial-Skolem result unsafe
	// to adopt before this. Ids are NOT used as a memo key anywhere -- two
	// formulas differing only in binder names are genuinely the same
	// formula here, but nothing relies on that.
	formula = canonize_quantifier_ids<node>(formula);

	// Step 1: NNF + syntactic simplification
	formula = to_nnf<node>(formula);
	formula = syntactic_formula_simplification<node>(formula);

	// Step 2: Substitution-based elimination (innermost first via post_order).
	// Attempts ex x (x = t && phi(x)) => phi(t) for each existential
	// conjunctive scope.
	formula = ex_subs_based_elimination<node>(formula);
	formula = syntactic_formula_simplification<node>(formula);

	// Step 3: Normalize non-canonical operators (bf_neq → ¬(bf_eq),
	// bf_nlteq → bf_lt, etc.) so the block core's conjunct classification
	// sees only canonical atoms.
	formula = normalize_atomic_formula_operators<node>(formula);

	// Step 4: Eliminate each maximal quantifier block (innermost blocks
	// first). For each maximal run of consecutive same-kind, same-BA-type
	// quantifiers, push the whole run into the body using the 6-arg
	// anti_prenex_block, then eliminate the remaining quantifiers over
	// atomic formulas via resolve_quantifiers2. wff_all blocks are handled
	// by negation (dualization): ∀x φ ≡ ¬∃x ¬φ.
	formula = process_quantifier_blocks<node>(formula, skip,
		ctx_bv_is_solver_owned);
	// Step 5: canonicalise again on the way out. The pass builds binders
	// with calculate_quant_id = false throughout, so ids inside the result
	// are whatever entry assigned them plus whatever survived the rewriting;
	// re-canonicalising restores the invariant callers outside this file
	// (api.tmpl.h, tau_tree_from_parser.tmpl.h) already maintain.
	// Step 6: drop binders that bind nothing and scopes that are constant.
	// Neither is a legitimate output of quantifier elimination, and both are
	// reachable: the re-wrap paths above put the WHOLE block back around a
	// part, which can leave a variable bound that no longer occurs in it, and
	// a scope can fold to T/F after the wrap.
	//
	// This is not cosmetic. `check_decided` (normalizer.tmpl.h) reads the
	// normalized formula as T, F or "undecided", and it runs on
	// `normalize_non_temp`'s output -- before `normalize_with_temp_simp`'s
	// `fold_trivial_quantifiers`. So a residual `all b2, b1 T` is reported as
	// a formula normalization *could not decide*, and `are_nso_equivalent`
	// answers negatively on a formula that is plainly T. Folding here, where
	// the residue is created, is what stops that.
	auto fold_vacuous_quant = [](tref n) -> tref {
		if (!is_child_quantifier<node>(n)) return n;
		tref scoped = tau::get(n)[0].second();
		// ex x T/F = all x T/F = T/F.
		if (tau::get(scoped).equals_T() || tau::get(scoped).equals_F())
			return scoped;
		// A binder over a variable absent from its scope is vacuous
		// under the standing non-empty domain assumption.
		if (!contains<node>(scoped, tau::get(n)[0].first()))
			return scoped;
		return n;
	};
	formula = syntactic_formula_simplification<node>(formula);
	formula = post_order<node>(formula).apply_unique(fold_vacuous_quant);
	return canonize_quantifier_ids<node>(formula);
}



/**
 * @internal
 * @brief Resolve quantifiers with a custom variable ordering (default skip).
 * @tparam node Tree node type.
 * @param formula Formula containing quantifiers.
 * @param order Comparison relation for variable ordering.
 * @return Formula with quantifiers resolved.
 * @endinternal
 */
template<NodeType node>
tref resolve_quantifiers2(tref formula, const typename term_handle<node>::order& order) {
	return resolve_quantifiers2<node>(formula, order, is_tref_bv_type_family<node>);
}

/**
 * @internal
 * @brief Resolve quantifiers with a custom variable ordering.
 *
 * Variant of `resolve_quantifiers` that uses the provided `order` relation to
 * sort variables before the elimination step.
 * @tparam node Tree node type.
 * @param formula Formula containing quantifiers.
 * @param order Comparison relation for variable ordering.
 * @param skip Predicate identifying variables this pass must not eliminate
 *        (defaults to BV-typed nodes, which need the solver/blasting instead).
 * @return Formula with quantifiers resolved.
 * @endinternal
 */
template<NodeType node>
tref resolve_quantifiers2(tref formula, const typename term_handle<node>::order& order,
		const std::function<bool(tref)>& skip) {
	using tau = tree<node>;
	subtree_set<node> excluded;
	auto down_resolver = [&](tref n) {
		if (is_child_quantifier<node>(n)) {
			// Check if the formula is closed and proceed to eliminate
			// the quantifier
			if (skip(tau::trim2(n))) {
				if (const trefs& free_vars = get_free_vars<node>(n);
					free_vars.empty() && is_bv_solvable_formula<node>(n)) {
					// Closed bv formula with explicit bitwidth: simplify to
					// T/F, but only on a definite answer -- cvc5 returning
					// unknown, or translation failing, means we cannot
					// decide, not that the formula is false.
					std::optional<bv_sat_status> status = bv_formula_sat_status<node>(n);
					if (status == bv_sat_status::sat) return tau::_T();
					if (status == bv_sat_status::unsat) return tau::_F();
					DBG(if (!status) LOG_TRACE << "solver undecided on " << LOG_FM(n);)
					excluded.insert(n);
				} else excluded.insert(n);
			}
			// TODO (HIGH): restrict to atomless types --
			// distribute_block_over_atoms and
			// eliminate_block_over_clause's negative-atom handling are
			// only valid in an atomless BA, and nothing checks that.
			else if (!tau::get(n).find_top(is<node, tau::ref>)) {
				using bdd = term_handle<node>::tbdd;
				// Record quantifier block in quants
				typename term_handle<node>::quants quants;
				const tref original = n; // save before stripping quantifiers
				while (is_child_quantifier<node>(n)) {
					tref var = tau::trim2(n);
					// Only collect binders this call can
					// actually eliminate. A skip-matched
					// variable belongs to the solver /
					// blasting path, and a variable absent
					// from `order` cannot be located by
					// bdd_quant -- stripping either deletes
					// the binder and leaves its variable
					// free in the term, and also makes
					// less_then inconsistent (it returns
					// false in both directions for an
					// unknown variable), which trips
					// bdd_quant's ordering assertion in
					// DEBUG and reads out of bounds in
					// Release. Leave them for the caller's
					// resolve_quantifiers / anti_prenex
					// fallback, the same treatment the
					// non-atomic body case below gets.
					if (skip(var)
						|| order.find(var) == order.end())
						break;
					if (is_child<node>(n, tau::wff_ex)) {
						quants.emplace_back(var, bdd::ex);
					} else {
						quants.emplace_back(var, bdd::all);
					}
					n = tau::get(n)[0].second();
				}
				// Nothing eliminable here: leave the node as it
				// was found.
				if (quants.empty()) return original;
				// All quantifiers have been trimmed from tree here
				const tau& nn = tau::get(n);
				if (nn.child_is(tau::bf_eq)) {
					// Normalize A = B to (A XOR B) = 0 if not already in
					// canonical form (can occur in tau_ba Boole expansions).
					if (!tau::get(n)[0][1].equals_0())
						n = norm_equation<node>(n);
					// Flip quantifiers due to equality
					for (auto& [_,q] : quants) {
						q == bdd::ex ? q = bdd::all : q = bdd::ex;
					}
					// Get the term of the equality
					n = tau::trim2(n);
					size_t term_type = tau::get(n).get_ba_type();
					// TODO (LOW): Syntactically simplify result?
					n = term_handle<node>::build(n, order).
						bdd_quant(quants, order).
							to_tau_term(term_type);
					return tau::build_bf_eq_0(n);
				} else if (nn.child_is(tau::wff_neg) && nn[0][0].child_is(tau::bf_eq)) {
					// Trim negation
					n = tau::trim2(n);
					// Normalize A = B to (A XOR B) = 0 if needed.
					if (!tau::get(n)[0][1].equals_0())
						n = norm_equation<node>(n);
					// Get the term of the equality
					n = tau::trim2(n);
					size_t term_type = tau::get(n).get_ba_type();
					// TODO (LOW): Syntactically simplify result?
					n = term_handle<node>::build(n, order).
						bdd_quant(quants, order).
							to_tau_term(term_type);
					return tau::build_wff_neg(tau::build_bf_eq_0(n));
				} else {
					// Formula under the block is not a single atomic
					// (e.g. a conjunction of bf_lt / bf_gt atoms);
					// return original with quantifier prefix intact so
					// the caller's resolve_quantifiers fallback can handle it.
					return original;
				}
			}
		}
		return n;
	};
	auto visit = [&](tref n) {
		if (excluded.contains(n)) return false;
		return is_formula<node>(n);
	};
	return pre_order<node>(formula).apply_unique(down_resolver, visit);
}

/** @internal @copydoc no_skip @endinternal */
template <NodeType node>
bool no_skip(tref) {
	return false;
}

/** @internal @copydoc resolve_quantifiers @endinternal */
template<NodeType node>
tref resolve_quantifiers(tref formula) {
using tau = tree<node>;
	subtree_set<node> excluded;
	auto down_resolver = [&](tref n) {
		if (is_child_quantifier<node>(n)) {
			// Check if the formula is closed and proceed to eliminate
			// the quantifier
			tref var = tau::trim2(n);
			if (is_bv_type_family<node>(tau::get(var).get_ba_type())) {
				// A purely bitvector formula is decided directly by
				// the solver, closed or not (see the two branches
				// below). This is checked before blasting: the
				// solver handles the bitvector arithmetic natively,
				// while deciding the blasted form (with its many
				// auxiliary quantifiers) is much harder for it --
				// and blasting neither closes a formula nor makes
				// this check succeed later.
				if (is_bv_solvable_formula<node>(n)) {
					// Only commit to T/F on a definite answer: cvc5
					// returning unknown, or translation failing, means
					// we cannot decide, not that the formula is false.
					const trefs& fv = get_free_vars<node>(n);
					if (fv.empty()) {
						std::optional<bv_sat_status> status =
							bv_formula_sat_status<node>(n);
						if (status == bv_sat_status::sat) return tau::_T();
						if (status == bv_sat_status::unsat) return tau::_F();
						DBG(if (!status) LOG_TRACE << "solver undecided on " << LOG_FM(n);)
					} else {
						// Open bv scope: `n` is equivalent to T exactly
						// when it is valid, and to F exactly when it is
						// unsatisfiable. Closing its free variables the
						// two opposite ways turns both into closed
						// queries the solver decides directly -- so an
						// open scope (e.g. `ex x (x + y = 0)` with `y`
						// free, which is valid) need not be blasted
						// either. Worth the two extra queries: blasting
						// such a scope instead means eliminating one
						// auxiliary bit variable per bit, which is
						// exponential in the bitwidth.
						// Only close *plain* variables.
						// get_free_vars collects every
						// is_var_or_capture node, i.e. `variable`
						// *and* bare `capture` nodes, while
						// build_wff_all only DBG-asserts
						// is(tau::variable) -- so in Release a free
						// capture yielded a malformed binder handed
						// straight to cvc5.
						//
						// A bv-typed io_var is deliberately still
						// allowed through: `variable => (uconst |
						// io_var | var_name) [typed]`, so an io_var
						// arrives wrapped in a variable node, gets a
						// well-formed binder, and is translated by
						// name -- which is how every bv io_var
						// scope is decided at all. Only a bare
						// capture is rejected here.
						bool all_plain = true;
						for (tref v : fv)
							if (!tau::get(v).is(tau::variable))
								all_plain = false;
						if (!all_plain) {
							DBG(LOG_TRACE << "resolve_quantifiers:"
								" open bv scope has a non-plain"
								" free variable, not closing: "
								<< LOG_FM(n) << "\n";)
							excluded.insert(n);
							return n;
						}
						tref univ = n, exis = n;
						for (auto it = fv.rbegin(); it != fv.rend(); ++it) {
							univ = tau::build_wff_all(*it, univ, false);
							exis = tau::build_wff_ex(*it, exis, false);
						}
						// `all Y n` sat (it is closed) <=> n valid.
						if (bv_formula_sat_status<node>(univ)
							== bv_sat_status::sat) return tau::_T();
						// `ex Y n` unsat <=> no assignment satisfies n.
						if (bv_formula_sat_status<node>(exis)
							== bv_sat_status::unsat) return tau::_F();
					}
				}
				if (bv_blasting)
					if (auto blasted = bv_predicate_blasting<node>(n);
						blasted && blasted != n)
						return blasted;
				excluded.insert(n);
			}
		}
		return n;
	};
	auto visit = [&](tref n) {
		if (excluded.contains(n)) return false;
		return is_formula<node>(n);
	};
	return pre_order<node>(formula).apply_unique(down_resolver, visit);
}

} // namespace idni::tau_lang
