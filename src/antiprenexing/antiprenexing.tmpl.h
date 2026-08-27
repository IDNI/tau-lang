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

/// Maximum nesting of `blast_block`'s blast-then-re-enter hop; 0 = unlimited
/// (the default). Real formulas use one level: blast once, then the re-entry
/// finds nothing left to blast — bound it (`--max-blast-reentry-depth`, REPL
/// `blastdepth`, `api::set_max_blast_reentry_depth`) if a blasting regression
/// ever loops. Runtime-tunable per the runtime-parameter policy; like the
/// other knobs here it is NOT thread-safe.
inline size_t max_blast_reentry_depth = 0;

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

/// Maximum number of DNF clauses complete_quantifier_elimination may
/// distribute one scope into (estimated as the product of the per-factor
/// disjunct counts of the scope's CNF factors). Above it cqe declines and
/// re-wraps the quantifier verbatim -- the same escape hatch it takes for
/// temporal and wff_ref scopes -- and logs a warning. The nomic ratchet
/// (GitHub #90) handed cqe an 81-factor CNF whose naive product was 2e75
/// clauses. Runtime-tunable via `api::set_cqe_max_clauses`; same caveats
/// as block_max_rounds above.
inline size_t cqe_max_clauses = std::numeric_limits<size_t>::max();

/**
 * @internal
 * @brief Core recursive helper for the anti-prenex block algorithm.
 *
 * Pushes a block of existential quantifier variables into a formula by splitting on disjunctions,
 * isolating dependent conjuncts, and performing Boole decomposition for
 * block variables `el` reports eliminable. Once a leaf clause depends only on
 * non-eliminable (e.g. bitvector) block variables, predicate blasting is
 * attempted on it directly (via the `blast_block` helper) instead of Boole
 * decomposition, which requires atomless-typed quantified variables.
 * @tparam node Tree node type.
 * @param formula Formula to push the quantifier block into.
 * @param block Ordered list of existentially quantified variables to push.
 * @param used_atms Set of atomic formulas already consumed in the current recursion; updated in place.
 * @param quant_pattern Map from quantified variables to their priority for BDD variable ordering.
 * @param order Variable ordering relation used by `resolve_quantifiers2`.
 * @param el Eliminability analysis identifying variables/atomic formulas this
 *        pass must not Boole-decompose (`el.skip(n)`; BV-typed nodes by
 *        default, diverted to predicate blasting instead once nothing else can
 *        be pushed).
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
	const eliminability<node>& el,
	size_t& splits_left,
	const block_eliminability<node>& elim = block_eliminability<node>{}) {
	using tau = tree<node>;
	// Once no eliminable block variable occurs free anymore, the
	// remaining leaf is entirely skipped (e.g. bitvector) content:
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
		// `solver_placement` first: off `eager` this whole attempt is
		// declined and the code below takes its existing fall-through
		// (blast, or return the re-wrapped `ex_fm` untouched).
		if (solver_placement == solver_site::eager
				&& get_free_vars<node>(ex_fm).empty()
				&& is_bv_solvable_formula<node>(ex_fm)) {
			// Spelled out: bv_formula_sat_status returns an optional, and a
			// nullopt (cvc5 answered unknown, or the translation failed)
			// means undecided, not false. `auto` hid that.
			std::optional<bv_sat_status> status = bv_formula_sat_status<node>(ex_fm);
			if (status == bv_sat_status::sat) return tau::_T();
			if (status == bv_sat_status::unsat) return tau::_F();
			DBG(if (!status) LOG_ERROR << "solver undecided on " << LOG_FM(ex_fm);)
		}
		if (bv_blasting && blast_placement == blast_site::per_leaf)
			if (auto blasted = bv_predicate_blasting<node>(ex_fm);
					blasted && blasted != ex_fm) {
				// blast_mode::defer: hand back the rewritten formula
				// and leave the quantifiers blasting introduced to
				// the next resolve pass. Same shape as the
				// depth-exceeded return just below, and sound for
				// the same reason.
				if (blast_method == blast_mode::defer)
					return blasted;
				// Bound the hop back into the pipeline; see
				// blast_reentry_depth. Returning `blasted` rather
				// than ex_fm keeps the blasting work done so far --
				// its quantifiers simply survive, which is sound.
				if (max_blast_reentry_depth
					&& blast_reentry_depth<node>()
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
				// Re-enter skipping bv-typed content rather than
				// skipping nothing at all:
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
					eliminability<node>::bv_only());
			}
		return ex_fm;
	};

	// Frozen content must never reach blast_block: a reference or a kept
	// binder makes it untouchable by any destination, blasting and the
	// solver included -- the same reasoning `process_quantifier_block`'s
	// own early-out applies before this recursion even starts. Every
	// blast_block call site below checks `el.has_frozen` first and, on a
	// hit, re-wraps the block around the argument instead: the same shape
	// the budget-exhausted path further down already uses.
	auto rewrap_block = [&](tref f) -> tref {
		for (auto v = block.rbegin(); v != block.rend(); ++v)
			f = build_wff_ex<node>(*v, f, false);
		return f;
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
	// called at several sites below (including once per disjunct), and it is
	// a full subtree walk. The memo is not shared across recursion levels --
	// each recursive call builds its own lambda -- which would need it
	// threaded as a parameter.
	//
	// The walk itself belongs to the analysis (`eliminability::has_skip_content`),
	// not to a `find_top(el.skip)` here: `skip` floors on a node's BA TYPE, so
	// on the bv-typed terms inside an atom the analysis explicitly classified
	// eliminable it answers yes, and every pure-BA bv clause would divert to
	// blast_block however the atom itself was classified.
	auto skip_content_memo = std::make_shared<subtree_unordered_map<node, bool>>();
	auto has_skip_content = [&el, skip_content_memo](tref f) -> bool {
		auto& memo = *skip_content_memo;
		if (auto it = memo.find(f); it != memo.end()) return it->second;
		return memo.emplace(f, el.has_skip_content(f)).first->second;
	};

	auto has_active_var = [&](tref f) {
		const trefs& vars = get_free_vars<node>(f);
		for (tref v : block)
			if (!el.skip(v) && hasbc(vars, v, tau::subtree_less))
				return true;
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
		const block_atom_profile<node> prof = profile_block_atoms<node>(f, el, true);
		if (prof.all_negated())
			return resolve_quantifiers2<node>(
				distribute_block_over_atoms<node>(f, block),
				order, el);
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
			// Pure structural scan (get_ba_type only), so a shared subtree
			// contributes the same note_type/homogeneous result every time
			// it's reached -- visit_unique is sound and avoids re-walking
			// it once per path on a blasted DAG.
			pre_order<node>(f).visit_unique(type_scan);
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
				tau::build_wff_or(disjs), order, el);
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
			const block_atom_profile<node> p = profile_block_atoms<node>(d, el, true);
			if (p.all_negated()) return 0;              // step 2a
			if (p.all_positive()) return 1;             // step 2b
			if (!tau::get(d).find_top(is<node, tau::wff_or>))
				return 2;                           // step 2c
			return 3;                                   // step 2e..
		};
		// Ranked once per disjunct, not once per comparison. Each rank runs a
		// full profile_block_atoms -- a skip find_top over the disjunct plus a
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
				quant_pattern, order, el, splits_left, elim);
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
		// Var-free conjuncts; they must be re-attached to every result
		// built from the dependent part below
		const tref indep = tau::build_wff_and(conjs);
		// GitHub #72: try the dependent part per connected component
		// under the block's variables -- ex X (A(X1) && B(X2)) with X1,
		// X2 disjoint is ex X1 A && ex X2 B -- and adopt that only when
		// every component DECIDES (T or F). Fed to the Boole
		// decomposition as one formula, N conjuncts with pairwise
		// disjoint support are a 2^N Shannon expansion: every branch
		// carries the other N-1 conjuncts unchanged, nothing ever
		// simplifies a sibling, and when the component in front of the
		// split is unsatisfiable (the shape a closed implication check
		// produces) not even the T shortcut prunes it. Decided on its
		// own, an F component ends the whole conjunction at once and a
		// T component simply drops out.
		//
		// Undecided components are NOT adopted: on a residual (a
		// component that keeps quantifiers or leaves a formula), the
		// joint processing below simplifies across components (measured
		// 2026-08-26 on the nested-block chain a tau constant's
		// complement produces: per-component residuals grew a 23 KB
		// body to 43 KB where the joint path shrank it to 0.9 KB, and
		// that compounded to 20 s over the remaining levels versus 0.3
		// s). So the per-component pass is a decision pre-check, and on
		// anything undecided the formula falls through untouched.
		// Frozen content and references are left out entirely: their
		// block is re-wrapped verbatim by design (see
		// process_quantifier_block's all-frozen early-out), and `el`
		// only knows about references once an analysis registered
		// them, so those are checked structurally as well.
		auto has_ref = [](tref f) {
			return tau::get(f).find_top(is<node, tau::wff_ref>)
				|| tau::get(f).find_top(is<node, tau::bf_ref>);
		};
		if (dep.size() > 1 && !el.has_frozen(formula)
			&& !has_ref(formula)) {
			std::vector<trefs> comps =
				group_by_shared_vars<node>(dep, block);
			if (comps.size() > 1) {
				bool all_decided = true;
				for (const trefs& comp : comps) {
					const tref cf = tau::build_wff_and(comp);
					const trefs& cvars = get_free_vars<node>(cf);
					trefs sub_block;
					for (tref v : block)
						if (hasbc(cvars, v, tau::subtree_less))
							sub_block.push_back(v);
					tref r = anti_prenex_block<node>(cf, sub_block,
						used_atms, quant_pattern, order, el,
						splits_left, elim);
					if (tau::get(r).equals_F()) return _F<node>();
					if (!tau::get(r).equals_T()) {
						all_decided = false;
						break;
					}
				}
				if (all_decided) return indep;
			}
		}
		formula = tau::build_wff_and(dep);
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
					used_atms, quant_pattern, order, el,
					splits_left, elim));
		// Check if dependent formula is clause -> push block into clause.
		// eliminate_block_over_clause assumes the whole clause is
		// homogeneously atomless-typed, so any skip-matched content in it
		// (not just among `block`'s own variables) must divert to
		// blast_block instead.
		if (!tau::get(formula).find_top(is<node, tau::wff_or>)) {
			if (!has_active_var(formula) || has_skip_content(formula)) {
				// Defense-in-depth: unreachable as of 2026-08-15 through
				// the 2-arg path -- formula-level frozen verdicts are
				// always variable-disjoint from active variables (see
				// task-6 report trace), so the dep/indep separation
				// detaches frozen content first. Kept because
				// reachability depends on upstream routing that the
				// category-ordered wrapping and per-block blasting work
				// (Tasks 7-8) changes, and because analyse_block-style
				// seeding (kept binders, unrecognized shapes) would break
				// the disjointness argument if it ever feeds el.
				if (el.has_frozen(formula))
					return tau::build_wff_and(indep,
						rewrap_block(formula));
				return tau::build_wff_and(indep, blast_block(formula));
			}
			return tau::build_wff_and(
				indep,
				resolve_quantifiers2<node>(
				eliminate_block_over_clause<node>(formula, block, elim, order), order, el));
		}
		// Using the available atomic formulas, do Boole decomposition on best fit
		auto is_atomic = [&used_atms, &el](tref n) {
			if (!tau::get(n).is(tau::wff)) return false;
			// Exclude used atomic fms
			if (used_atms.contains(n)) return false;
			// Exclude atomic formulas the caller wants skipped (e.g. BV-typed)
			if (el.skip(n)) return false;
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
			if (!has_active_var(formula)) {
				// Defense-in-depth: unreachable as of 2026-08-15 through
				// the 2-arg path -- formula-level frozen verdicts are
				// always variable-disjoint from active variables (see
				// task-6 report trace), so the dep/indep separation
				// detaches frozen content first. Kept because
				// reachability depends on upstream routing that the
				// category-ordered wrapping and per-block blasting work
				// (Tasks 7-8) changes, and because analyse_block-style
				// seeding (kept binders, unrecognized shapes) would break
				// the disjointness argument if it ever feeds el.
				if (el.has_frozen(formula))
					return tau::build_wff_and(indep,
						rewrap_block(formula));
				return tau::build_wff_and(indep, blast_block(formula));
			}
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
		if (!el.skip(atm)) {
			int_t best = -1;
			const trefs& avars = get_free_vars<node>(atm);
			for (tref v : block) {
				if (el.skip(v)) continue;
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
						order, el, splits_left, elim));
			// gamma3: f is identically 1, so the atom holds for
			// no value of the variable -- fold it to F.
			if (an.kind == boole_atom_case::identically_one)
				return tau::build_wff_and(indep,
					anti_prenex_block<node>(
						rewriter::replace<node>(
							formula, atm, tau::_F()),
						block, used_atms, quant_pattern,
						order, el, splits_left, elim));
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
					used_atms, quant_pattern, order, el,
					splits_left, elim);
				tref pr2 = anti_prenex_block<node>(gr, block,
					used_atms, quant_pattern, order, el,
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
			for (tref v : block) if (!el.skip(v)) ++active_vars;
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
					used_atms, quant_pattern, order, el,
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
				l, block, used_atms, quant_pattern, order, el,
				splits_left, elim);
			used_atms.erase(atm);
			return tau::build_wff_and(indep, res);
		}
		// One split, charged once against the shared budget.
		--splits_left;
		tref nl = anti_prenex_block(
			tau::build_wff_and(atm, l), block, used_atms,
			quant_pattern, order, el, splits_left, elim);
		if (tau::get(nl).equals_T()) {
			used_atms.erase(atm);
			// dep part is T, only the var-free part remains
			return indep;
		}
		tref nr = anti_prenex_block(
			tau::build_wff_and(tau::build_wff_neg(atm), r),
				block, used_atms, quant_pattern, order, el,
				splits_left, elim);
		used_atms.erase(atm);
		return tau::build_wff_and(indep, tau::build_wff_or(nl, nr));
	}

	// Connective is not wff_and or wff_or (e.g. a single atom or a wff_ref)
	// -> try blasting if the block needs it, else quantifiers stay with formula
	if (!has_active_var(formula)) {
		// Defense-in-depth: unreachable as of 2026-08-15 through the 2-arg
		// path -- formula-level frozen verdicts are always
		// variable-disjoint from active variables (see task-6 report
		// trace), so the dep/indep separation detaches frozen content
		// first. Kept because reachability depends on upstream routing
		// that the category-ordered wrapping and per-block blasting work
		// (Tasks 7-8) changes, and because analyse_block-style seeding
		// (kept binders, unrecognized shapes) would break the
		// disjointness argument if it ever feeds el.
		if (el.has_frozen(formula)) return rewrap_block(formula);
		return blast_block(formula);
	}
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
	const eliminability<node>& el)
{
	size_t splits_left = block_boole_max_splits;
	return anti_prenex_block<node>(formula, block, used_atms, quant_pattern,
		order, el, splits_left);
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
	// Binders displaced from the run, outermost first, each with its own
	// kind (true = ∃) and its category (the verdict `el.verdict_of(var)`
	// returned at the moment collection displaced it -- skip() and
	// verdict_of() are consistent by construction, so a displaced var's
	// category is never `eliminable`; eliminable vars join `vars` above
	// instead). NOT all same-kind: collection absorbs skip-matched
	// binders of either kind while the block's own kind is still
	// unfixed (see collect_quantifier_block), so a run of displaced
	// binders can contain a mix of ∃ and ∀. Re-wrapped by
	// process_quantifier_block's wrap_skipped, which reorders by
	// category (frozen outermost, then arithmetic, then blasteable)
	// ONLY WITHIN each maximal same-kind segment of this vector --
	// same-kind quantifiers commute, so that much is sound; segment
	// order itself (i.e. order across a kind change) is never touched,
	// because opposite-kind quantifiers do not commute. The category is
	// an ordering hint only, not a semantic invariant: it is read once
	// at displacement time and never re-checked, which is fine because
	// same-kind commutation makes the within-segment emission order
	// sound regardless of whether the category could in principle have
	// drifted.
	std::vector<std::tuple<tref, bool, elim_verdict>> displaced;
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
 * join it. They are recorded in `displaced`, each with the verdict
 * `el.verdict_of(var)` returned at the moment of displacement, and
 * re-wrapped later by category, so that active quantifiers sitting below
 * them still join *this* block instead of being stranded in a separate one
 * of their own.
 * @tparam node Tree node type.
 * @param n Formula node whose direct child is a quantifier.
 * @param el Eliminability analysis marking variables this pass must not
 *        eliminate (`el.skip(v)`).
 * @return The collected block, including the matrix it scopes over.
 * @endinternal
 */
template<NodeType node>
quantifier_block<node> collect_quantifier_block(tref n,
	const eliminability<node>& el)
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
		if (el.skip(var)) {
			// Transparent only while it genuinely commutes with the
			// block. Same-kind quantifiers do; opposite-kind ones do
			// not -- `ex a all x phi` is not `all x ex a phi`.
			// wrap_skipped only reorders WITHIN a maximal same-kind
			// segment of `displaced` (frozen outermost, arithmetic,
			// then blasteable innermost, all within that segment);
			// it never moves a binder past a kind change. Absorbing
			// an opposite-kind quantifier here once the block's own
			// kind is fixed would still leave it stranded inside a
			// block whose active vars (`blk.vars`) are the OTHER
			// kind, so it must end the run regardless -- that part
			// is unrelated to wrap_skipped's re-emission order.
			// Disjoint BA types do not rescue it: with `a` atomless
			// and `x` a bitvector, phi = (a = 0 <-> x = 0) makes
			// `all x ex a phi` true and `ex a all x phi` false.
			//
			// A skipped quantifier seen *before* any active one is
			// always safe: it stays outermost either way -- but a
			// RUN of several such pre-kind-fixed skipped quantifiers
			// of MIXED kinds is not thereby exempt from the
			// same-kind-commutes rule; wrap_skipped's segmenting is
			// what keeps that run's own internal order sound (see
			// `quantifier_block::displaced`).
			//
			// Breaking is self-healing: blk.body then starts with
			// that quantifier and select_innermost_blocks picks it
			// up as an inner block on a later round.
			if (kind_fixed && curr_is_ex != blk.is_ex) break;
			blk.displaced.emplace_back(var, curr_is_ex,
				el.verdict_of(var));
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

/** @copydoc complete_quantifier_elimination */
template<NodeType node>
tref complete_quantifier_elimination(tref formula) {
	using tau = tree<node>;
	// Innermost quantifier first (post_order): a nested quantifier's own
	// scope is already resolved by the time it is substituted into its
	// parent's, so the parent's substitution never has to look through a
	// binder of its own.
	auto step = [](tref n) -> tref {
		if (!is_child_quantifier<node>(n)) return n;
		const tau& t = tau::get(n);
		const tref var = t[0].first();
		const tref scoped = t[0].second();
		// The NZ-1 shape (a temporal operator directly inside a
		// quantifier scope, e.g. `all b (always b != c)`): eliminating `b`
		// says nothing about a scope whose truth varies over time, so this
		// is genuinely undecidable by any case-split on `b` alone -- see
		// check_decided's doc comment, and the pinned
		// UndecidableNormalizationFallback / wff_normalization tests for
		// this exact shape.
		if (tau::get(scoped).find_top(is_temporal_quantifier<node>))
			return n;
		// An uninterpreted predicate reference (`wff_ref`, e.g. `f(x)`)
		// anywhere in the scope: no Boolean-algebra elimination technique
		// can settle what an opaque predicate holds for, so `v` genuinely
		// cannot be removed. eliminate_block_over_clause would decline
		// each such conjunct on its own (correctly, via its unrecognized-
		// conjunct reservation) and still re-wrap `v` -- but distributing
		// first, the way the DNF split below does, restructures the
		// formula into separate quantifiers over independent references
		// even though nothing was actually eliminated. Declining up front
		// instead leaves a frozen scope exactly as found, matching
		// process_quantifier_block's own all-frozen early-out (pinned by
		// "an all-frozen block is re-wrapped verbatim").
		if (tau::get(scoped).find_top(is<node, tau::wff_ref>))
			return n;
		const bool is_ex = t.child_is(tau::wff_ex);
		// Dualize ∀ to ∃ over the negated scope -- the same identity
		// resolve_ex_block uses for a whole block, so only the ∃ case
		// needs an eliminator below.
		const tref body = is_ex ? scoped
			: to_nnf<node>(tau::build_wff_neg(scoped));
		// `ex v (A|B) = (ex v A)|(ex v B)`: distribute over the scope's
		// disjunction, then eliminate `v` from each OR-free clause with
		// the squeeze `eliminate_block_over_clause` already uses for a
		// whole block -- handed a singleton one here, which is what makes
		// this sound for ANY Boolean algebra (atomless or atomic, not just
		// `bool`): that squeeze's own type checks decide, per clause,
		// whether the elimination applies, and re-wrap `v` around a clause
		// it must decline.
		//
		// This is what the pipeline above gave up on: anti_prenex_block's
		// own pivot selection (`is_atomic`/`stop_at`) only ever splits on a
		// NON-negated atom, so a variable occurring solely in `!=` atoms
		// starves it of a pivot and it re-wraps the whole block instead of
		// reaching this squeeze at all.
		const block_eliminability<node> trivial_elim{};
		const typename term_handle<node>::order empty_order{};
		auto mentions = [&](tref f) { return contains<node>(f, var); };
		// Residue eliminator: the DNF split + per-clause squeeze, run only
		// on the var-dependent part `elim` below hands it. Returns nullopt
		// past the clause cap: the DNF multiplies the residue's CNF
		// factors' disjunct counts, so that product is estimated first (in
		// the overflow-safe division form) and cqe declines instead of
		// building an astronomically large DNF (the nomic ratchet, GitHub
		// #90, handed it an 81-factor CNF with a naive product of 2e75).
		auto distribute = [&](tref b) -> std::optional<tref> {
			size_t est = 1;
			for (tref f : get_cnf_wff_clauses<node>(b)) {
				const size_t k = get_dnf_wff_clauses<node>(f).size();
				if (k == 0 || est > cqe_max_clauses / k) {
					est = std::numeric_limits<size_t>::max();
					break;
				}
				est *= k;
			}
			if (est > cqe_max_clauses) return std::nullopt;
			tref res = _F<node>();
			// normalize_atomic_formula_operators after distributing: the
			// reducing to_dnf rebuilds negated atoms as `bf_neq`, while
			// the squeeze recognizes only the canonical `f = 0` /
			// `!(f = 0)` spellings anti_prenex's step 3 established --
			// without this every clause carrying a negated atom was
			// declined and re-wrapped whenever the scope actually needed
			// distributing.
			for (tref clause : get_dnf_wff_clauses<node>(
					normalize_atomic_formula_operators<node>(
						to_dnf<node, true>(b))))
				res = tau::build_wff_or(res,
					eliminate_block_over_clause<node>(clause, {var},
						trivial_elim, empty_order));
			return res;
		};
		// Miniscoping: ex v (A || B) = ex v A || ex v B, and
		// ex v (A && B) = A && ex v B when v does not occur in A. Exact
		// for any Boolean algebra; it only shrinks what `distribute` has
		// to multiply out, which is what keeps a scope whose v-free
		// factors dominate (the ratchet's 81-factor CNF had 21 distinct
		// v atoms) from being distributed wholesale.
		auto elim = [&](auto& self, tref b) -> std::optional<tref> {
			if (!mentions(b)) return b;
			const tau& tb = tau::get(b);
			if (tb.child_is(tau::wff_or)) {
				auto l = self(self, tb[0].first());
				if (!l) return std::nullopt;
				auto r = self(self, tb[0].second());
				if (!r) return std::nullopt;
				return tau::build_wff_or(*l, *r);
			}
			if (tb.child_is(tau::wff_and)) {
				tref free = nullptr, dep = nullptr;
				for (tref c : get_cnf_wff_clauses<node>(b)) {
					tref& slot = mentions(c) ? dep : free;
					slot = slot ? tau::build_wff_and(slot, c) : c;
				}
				// dep is set: b mentions var, so some factor does. With
				// nothing to lift, fall through to distribute -- every
				// factor depends on var and the split is unavoidable.
				if (free) {
					auto d = self(self, dep);
					if (!d) return std::nullopt;
					return tau::build_wff_and(free, *d);
				}
			}
			return distribute(b);
		};
		auto res = elim(elim, body);
		if (!res) {
			LOG_WARNING << "complete_quantifier_elimination: scope of "
				<< LOG_FM(var) << " would distribute into more than "
				<< cqe_max_clauses << " clauses; quantifier kept";
			return n;
		}
		return is_ex ? *res : to_nnf<node>(tau::build_wff_neg(*res));
	};
	// Do not descend into terms: a tau_ba constant carries its own internal
	// wff_ex/wff_all over I/O variables (the same guard
	// select_innermost_blocks and anti_prenex_block's short-circuit use).
	auto visit = [](tref t) { return while_is_formula<node>(t); };
	return normalize_atomic_formula_operators<node>(
		post_order<node>(formula).apply_unique(step, visit));
}

/**
 * @internal
 * @brief Eliminates one maximal quantifier block over an already-processed matrix.
 *
 * Delegates the whole block (whether atomless, bitvector-typed, or mixed)
 * to the 6-arg `anti_prenex_block`, which splits disjunctions/conjuncts,
 * Boole-decomposes atomless content, and attempts predicate blasting on
 * whatever leaf clause still needs `skip`-matched (e.g. bitvector) content
 * resolved. A quantifier still standing after all of that falls to
 * `complete_quantifier_elimination` (see `resolve_ex_block`'s note) --
 * temporal-free only, everything else genuinely survives. ∃-blocks are
 * pushed in directly; ∀-blocks are dualized to ∃-blocks.
 *
 * `blk.body` is required to already be free of unresolved quantifier scope:
 * the pipeline below (`eliminate_block_over_clause` in particular) assumes it
 * is handed fully resolved, homogeneously typed content. `process_quantifier_blocks`
 * guarantees this by only ever eliminating innermost blocks.
 * @tparam node Tree node type.
 * @param blk The maximal block to eliminate, as returned by `collect_quantifier_block`.
 * @param el Eliminability analysis marking variables this pass must not
 *        eliminate (`el.skip(v)`).
 * @param ctx_bv_is_solver_owned Formula-wide input of the eliminability
 *        analysis: `false` when the formula holds a constant of a Boolean
 *        algebra cvc5 cannot translate, in which case no bitvector scope
 *        anywhere in it will ever be decided by the solver. It scopes the
 *        `blasteable` verdicts of the synthesized block view below, exactly as
 *        `analyse_block`'s `analysis_context` scoped its own seeds. Not
 *        redundant with `el`: the three `eliminability<node>::bv_only()` entry
 *        points (the 1-argument `anti_prenex`, `blast_block`'s re-entry,
 *        `eliminate_block_over_clause`'s re-entry) hand down a floor built
 *        from no context at all, so without this the bv content of a formula
 *        carrying a foreign BA constant would be marked `blasteable` and
 *        stranded -- the issue #70 shape. `anti_prenex` computes the flag once
 *        (`!has_foreign_ba_constant`) and passes it through
 *        `process_quantifier_blocks` to here. Task 9 replaces it with the full
 *        `analysis_context`.
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
	const eliminability<node>& el = eliminability<node>::bv_only(),
	bool ctx_bv_is_solver_owned = true)
{
	using tau = tree<node>;
	trefs block_vars = blk.vars;
	tref body = blk.body;
	const bool is_ex = blk.is_ex;

	// Re-wrap quantifiers displaced by the collection above around a
	// result. `displaced` is not all one kind (see its doc comment):
	// collection absorbs skip-matched binders of either kind while the
	// block's own kind is still unfixed. So reordering is done ONLY
	// WITHIN each maximal same-kind segment of `displaced` -- binders in
	// one such segment commute freely with each other, which is what
	// makes category order sound there; segment order itself (i.e.
	// order across a kind change) is left exactly as encountered,
	// because opposite-kind quantifiers never commute. Determinism: for
	// an unchanged block this always re-wraps to a structurally
	// identical tref (same input `blk.displaced` -> same segmentation ->
	// same stable emission order), which is what lets
	// process_quantifier_blocks' `done`-set termination retire it.
	//
	// `skip_blasteable` is the bookkeeping the `per_block` hook at the end
	// of this function needs: that hook wraps the blasteable sub-run around
	// `result` itself, but ONLY from the TRAILING same-kind segment of
	// `displaced` (the one adjacent to `result` -- see the hook's own
	// comment), never reaching past a kind boundary. So `skip_blasteable`
	// must likewise suppress blasteable emission ONLY in the first segment
	// processed below (which is that same trailing segment, by
	// construction): a blasteable binder sitting in an OUTER segment was
	// never extracted by the hook and must still be emitted here, or it
	// would be silently dropped rather than merely mis-ordered. Passing a
	// flag rather than handing this lambda a filtered copy of
	// `blk.displaced` keeps the emission loop and its determinism argument
	// untouched, and leaves all the other call sites unchanged.
	auto wrap_skipped = [&](tref r, bool skip_blasteable = false) -> tref {
		// Walk `displaced` from its end, peeling off one maximal
		// same-kind segment at a time (encounter order, so a segment
		// closer to the front is closer to the OUTSIDE of the run).
		// Processing segments innermost-first -- i.e. the one nearest
		// `body`/`r` first -- means each subsequent (more-outer)
		// segment's wraps land OUTSIDE what came before, exactly
		// reproducing the original nesting across every kind boundary.
		size_t seg_end = blk.displaced.size();
		bool first_segment = true;
		while (seg_end > 0) {
			const bool seg_kind = std::get<1>(blk.displaced[seg_end - 1]);
			size_t seg_begin = seg_end;
			while (seg_begin > 0 && std::get<1>(
				blk.displaced[seg_begin - 1]) == seg_kind)
					--seg_begin;
			// Within this one same-kind segment: emit by category,
			// innermost group first (blasteable, then arithmetic,
			// then frozen -- so frozen ends outermost WITHIN the
			// segment). Stable within a category (original relative
			// order preserved).
			for (elim_verdict want : { elim_verdict::blasteable,
				elim_verdict::arithmetic, elim_verdict::frozen }) {
				if (first_segment && skip_blasteable
					&& want == elim_verdict::blasteable)
						continue;
				for (size_t i = seg_end; i-- > seg_begin; ) {
					const auto& [var, is_ex_q, cat] =
						blk.displaced[i];
					if (cat != want) continue;
					r = is_ex_q ? build_wff_ex<node>(var, r, false)
						: build_wff_all<node>(var, r, false);
				}
			}
			seg_end = seg_begin;
			first_segment = false;
		}
		return r;
	};

	// A frozen variable's scope must not be normalized: re-wrap the whole
	// block verbatim. Cheap and exact -- frozen means "no destination can
	// take it", so every path below would only end in the same re-wrap.
	if (!block_vars.empty()) {
		bool all_frozen = true;
		for (tref v : block_vars)
			if (el.verdict_of(v) != elim_verdict::frozen)
				{ all_frozen = false; break; }
		if (all_frozen) {
			tref kept = body;
			for (auto v = block_vars.rbegin();
				v != block_vars.rend(); ++v)
				kept = is_ex ? build_wff_ex<node>(*v, kept, false)
					: build_wff_all<node>(*v, kept, false);
			return wrap_skipped(kept);
		}
	}

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
		// Per-block eliminability view, handed to
		// eliminate_block_over_clause. Synthesized from the formula-level
		// analysis rather than recomputed: `el` is already keyed on these
		// very block variables, so reading its verdicts costs no traversal
		// at all. `components` is deliberately left empty --
		// eliminate_block_over_clause re-derives the reserved conjunct set
		// from the clause actually in front of it (see its own comment),
		// which is what keeps this correct across the splits and gamma
		// folds that rebuild those conjuncts.
		//
		// Measured 2026-08-14 against (a) bypassing the analysis entirely
		// (empty `elim`) and (b) running `analyse_block` over the body (a
		// fresh union-find pass per block): test_integration-satisfiability2
		// Release 26.7 / 27.2 / 26.7 s and Debug 187.7 / 183.0 / 178.7 s
		// for (a) / (b) / (c); all three 100% correct in both configs.
		// This one -- (c) -- was taken.
		block_eliminability<node> elim{};
		for (tref v : block_vars) {
			elim_verdict vd = el.verdict_of(v);
			// `blasteable` means "bv-typed, and cvc5 can be
			// expected to translate this formula". The second half
			// is a property of the whole formula, and `el` does not
			// always carry it: the three
			// `eliminability<node>::bv_only()` entry points hand
			// down an unconditional bv floor built from no context
			// at all. Applying it here is what keeps a formula
			// holding a foreign BA constant out of a solver that
			// cannot represent it -- Boole decomposition is then the
			// only route left for that content, and marking it
			// `blasteable` would strand the quantifier for good
			// (the issue #70 shape). `arithmetic` and `frozen` are
			// untouched: neither is about solver ownership.
			if (!ctx_bv_is_solver_owned
				&& vd == elim_verdict::blasteable)
					vd = elim_verdict::eliminable;
			elim.verdicts.emplace(v, vd);
			elim.members[vd].insert(v);
		}
		subtree_unordered_set<node> used_atms;
		// One budget per block elimination, shared by its whole
		// recursion.
		size_t splits_left = block_boole_max_splits;
		tref r = anti_prenex_block<node>(b, block_vars, used_atms, qp,
			ord, el, splits_left, elim);
		r = resolve_quantifiers2<node>(r, ord, el);
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
		// Last resort: Boole/Shannon-eliminate whatever is still quantified,
		// one variable at a time (complete_quantifier_elimination). The
		// legacy step-based `anti_prenex` used to run here unconditionally
		// on whatever still carried a quantifier, and was deleted
		// 2026-08-04 once both full suites passed without it (319/319,
		// Debug and Release) -- the experiment that proved it and the three
		// capabilities that had to be built first (the eliminability
		// partition, the leaf_clause merge, the γ1 `¬atm` fix above, pinned
		// by Gamma1NegatedBranch) are in this file's history at the
		// deletion commit. That measurement predates this pipeline meeting
		// a variable that occurs only in `!=` atoms: anti_prenex_block's own
		// pivot selection never splits on a negated atom (see its
		// `is_atomic`/`stop_at`), so such a variable starves it of a pivot
		// and it gives up with the block still quantified -- a real
		// completeness regression against the deleted algorithm, not the
		// performance trade the deletion accepted.
		// complete_quantifier_elimination reaches this shape by routing
		// straight to the same per-clause squeeze eliminate_block_over_
		// clause already uses (sound for any Boolean algebra, not just
		// `bool`) instead of anti_prenex_block's atom-pivot splitting; it
		// skips a scope still holding a temporal operator (the NZ-1 shape,
		// genuinely undecidable by either algorithm).
		if (has_live_quantifier(r))
			r = complete_quantifier_elimination<node>(r);
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

	// Options 5c / 7b -- the per-block destination. The block is fully
	// processed here: every eliminable variable is gone, so this is the
	// first point at which the blasteable binders this block displaced can
	// be handed to a destination as one sub-block instead of one leaf
	// clause at a time. Task 7 made that sub-run contiguous and innermost
	// WITHIN its own same-kind segment of `displaced` (see wrap_skipped's
	// comment: category reordering, and this extraction, are sound only
	// within one same-kind segment, never across a kind change), so
	// wrapping it here and telling wrap_skipped to skip the blasteable
	// group reproduces the same nesting order it would have produced
	// itself. This hook therefore only ever looks at the TRAILING maximal
	// same-kind segment -- the one adjacent to `result` -- never reaching
	// past a kind boundary into an outer segment.
	//
	// Inert at the shipped defaults (`per_leaf` / `eager`): the guard below
	// is false and `result` reaches wrap_skipped untouched.
	//
	// Not every block reaches this point: the all-frozen early-out and a
	// full trivial-Skolem elimination both return through wrap_skipped
	// above, before this hook ever runs -- so "per_block" means "per block
	// that reaches the main pipeline", not literally every block collected.
	bool blasteable_consumed = false;
	if (solver_placement == solver_site::per_closed_block
		|| blast_placement == blast_site::per_block)
	{
		tref sub = result;
		size_t n_blasteable = 0;
		// The trailing same-kind segment of `displaced` -- see above.
		size_t trailing_seg_begin = blk.displaced.size();
		if (trailing_seg_begin > 0) {
			const bool seg_kind = std::get<1>(
				blk.displaced[trailing_seg_begin - 1]);
			while (trailing_seg_begin > 0 && std::get<1>(
				blk.displaced[trailing_seg_begin - 1]) == seg_kind)
					--trailing_seg_begin;
		}
		for (size_t i = blk.displaced.size();
			i-- > trailing_seg_begin; ) {
			const auto& [var, is_ex_q, cat] = blk.displaced[i];
			if (cat != elim_verdict::blasteable) continue;
			// Same demotion resolve_ex_block applies to block_vars
			// (see its comment, and the issue #70 rationale it
			// cites): when the formula holds a foreign BA constant,
			// no bitvector content in it is solver-owned, so a
			// displaced `blasteable` binder must not be handed to
			// either the solver arm or the blasting arm below --
			// the solver arm is already protected by its own
			// is_bv_solvable_formula check, but the blasting arm
			// is not. Leaving it out of `sub`/`n_blasteable`
			// entirely keeps it out of both, and it falls through
			// to wrap_skipped's normal blasteable-group emission
			// instead, exactly as it would without this hook.
			if (!ctx_bv_is_solver_owned) continue;
			sub = is_ex_q ? build_wff_ex<node>(var, sub, false)
				: build_wff_all<node>(var, sub, false);
			++n_blasteable;
		}
		if (n_blasteable) {
			bool handled = false;
			if (solver_placement == solver_site::per_closed_block
				&& get_free_vars<node>(sub).empty()
				&& is_bv_solvable_formula<node>(sub))
			{
				// Only commit to T/F on a definite answer --
				// nullopt means undecided, not false. Same
				// reasoning as in resolve_quantifiers.
				std::optional<bv_sat_status> st =
					bv_formula_sat_status<node>(sub);
				if (st == bv_sat_status::sat)
					{ result = tau::_T(); handled = true; }
				else if (st == bv_sat_status::unsat)
					{ result = tau::_F(); handled = true; }
			}
			if (!handled && bv_blasting
				&& blast_placement == blast_site::per_block)
				if (tref bl = bv_predicate_blasting<node>(sub);
					bl && bl != sub)
				{
					// Same hop bound blast_block applies, for
					// the same reason (see
					// blast_reentry_depth); `defer` and an
					// exhausted budget both keep the blasted
					// formula with its quantifiers alive,
					// which is sound. `defer` is a deliberate
					// choice and stays silent; an exhausted
					// budget is logged the same way blast_block
					// logs its own exhaustion, so the two causes
					// remain distinguishable in the log.
					if (blast_method
						== blast_mode::anti_prenex_result)
					{
						const bool may_reenter =
							!max_blast_reentry_depth
							|| blast_reentry_depth<node>()
								< max_blast_reentry_depth;
						if (may_reenter)
						{
							blast_reentry_guard<node> guard;
							result = anti_prenex<node>(bl,
								eliminability<node>::bv_only());
						} else {
							LOG_ERROR << "per_block hook:"
								" blast/re-enter depth "
								<< max_blast_reentry_depth
								<< " exceeded on "
								<< LOG_FM(sub)
								<< "; returning the"
								" blasted formula without"
								" re-entering.";
							result = bl;
						}
					} else result = bl;
					handled = true;
				}
			// Either way the blasteable binders are now around
			// `result` already (or gone with the constant that
			// replaced it), so wrap_skipped must not emit them a
			// second time. `handled == false` keeps `sub`, which
			// already carries them.
			if (!handled) result = sub;
			blasteable_consumed = true;
		}
	}
	return wrap_skipped(result, blasteable_consumed);
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
 * @param el Eliminability analysis marking variables this pass must not
 *        eliminate (`el.skip(v)`).
 * @param done Heads already processed in this pass.
 * @param out Collected blocks, appended to.
 * @endinternal
 */
template<NodeType node>
void select_innermost_blocks(tref fm, const eliminability<node>& el,
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
			collect_quantifier_block<node>(head, el);
		if (done.contains(head)) {
			// Retired, but chapter 5 step 2k asks for step 2 to be
			// re-entered wrt *every* quantifier in the resulting
			// expression: a block nested inside this one's result
			// has never been seen. Descend without re-emitting the
			// head itself.
			select_innermost_blocks<node>(blk.body, el, done, out);
			continue;
		}
		const size_t before = out.size();
		select_innermost_blocks<node>(blk.body, el, done, out);
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
 * @param el Eliminability analysis marking variables this pass must not
 *        eliminate (`el.skip(v)`).
 * @param ctx_bv_is_solver_owned Formula-wide input of the eliminability
 *        analysis; see `process_quantifier_block`. Passed straight through.
 * @return Formula with every maximal quantifier block eliminated or pushed inward.
 * @endinternal
 */
template<NodeType node>
tref process_quantifier_blocks(tref fm, const eliminability<node>& el,
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
	// The cap takes the documented graceful give-up (log + return the
	// formula unprocessed) in every build. A DBG assert used to guard this
	// spot, but once block_max_rounds became a runtime parameter whose
	// default is unlimited it could never fire at the default (rounds
	// never reaches SIZE_MAX) — it only aborted Debug runs of users who
	// set a FINITE cap, contradicting api::set_block_max_rounds's
	// contract. Removed 2026-08-19; the LOG_ERROR below is the loud
	// failure channel in all configurations.
	// Read once into a local: a caller retuning it mid-pass would otherwise
	// move the goalposts between iterations.
	const size_t max_rounds = block_max_rounds;
	size_t rounds = 0;
	for (;;) {
		if (++rounds > max_rounds) {
			LOG_ERROR << "process_quantifier_blocks: no convergence "
				"after " << max_rounds << " rounds, returning the "
				"formula unprocessed: " << LOG_FM(fm);
			return fm;
		}
		std::vector<quantifier_block<node>> blocks;
		select_innermost_blocks<node>(fm, el, done, blocks);
		DBG(LOG_TRACE << "process_quantifier_blocks round " << rounds
			<< ": " << blocks.size() << " block(s)\n";)
		if (blocks.empty())
			return post_order<node>(fm).apply_unique(
				drop_const_quant);
		subtree_map<node, tref> changes;
		for (const quantifier_block<node>& blk : blocks) {
			done.insert(blk.head);
			tref res = process_quantifier_block<node>(blk, el,
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
 * @brief Convenience overload: the pipeline with the default analysis
 * (`eliminability<node>::bv_only()`). See the 2-argument overload below for
 * what it actually does.
 * @tparam node Tree node type.
 * @param formula Formula to process.
 * @return Formula with quantifiers pushed inward as far as possible using the
 * block-based algorithm.
 * @endinternal
 */
template<NodeType node>
tref anti_prenex(tref formula) {
	return anti_prenex<node>(formula, eliminability<node>::bv_only());
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
 * @param el Eliminability analysis marking content this pass must not
 *        Boole-decompose (`el.skip(n)`).
 * @return Formula with quantifiers pushed inward as far as possible.
 * @endinternal
 */
template<NodeType node>
tref anti_prenex(tref formula, const eliminability<node>& el) {
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
	// this formula will ever be decided by the solver, and `blasteable`
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
	formula = process_quantifier_blocks<node>(formula, el,
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
	return resolve_quantifiers2<node>(formula, order,
		eliminability<node>::bv_only());
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
 * @param el Eliminability analysis identifying variables this pass must not
 *        eliminate (BV-typed nodes by default, which need the
 *        solver/blasting instead).
 * @return Formula with quantifiers resolved.
 * @endinternal
 */
template<NodeType node>
tref resolve_quantifiers2(tref formula, const typename term_handle<node>::order& order,
		const eliminability<node>& el) {
	using tau = tree<node>;
	subtree_set<node> excluded;
	auto down_resolver = [&](tref n) {
		if (is_child_quantifier<node>(n)) {
			// Check if the formula is closed and proceed to eliminate
			// the quantifier
			if (el.skip(tau::trim2(n))) {
				// Gated on `eager`: off it, the attempt is simply
				// declined and the scope is excluded from the
				// traversal -- which is exactly what both the
				// not-solvable and the undecided paths below already
				// do, so the two arms collapse into the single
				// excluded.insert() after them.
				if (solver_placement == solver_site::eager) {
					if (const trefs& free_vars = get_free_vars<node>(n);
						free_vars.empty() && is_bv_solvable_formula<node>(n))
					{
						// Closed bv formula with explicit bitwidth: simplify
						// to T/F, but only on a definite answer -- cvc5
						// returning unknown, or translation failing, means we
						// cannot decide, not that the formula is false.
						std::optional<bv_sat_status> status = bv_formula_sat_status<node>(n);
						if (status == bv_sat_status::sat) return tau::_T();
						if (status == bv_sat_status::unsat) return tau::_F();
						DBG(if (!status) LOG_TRACE << "solver undecided on " << LOG_FM(n);)
					}
				}
				excluded.insert(n);
			}
			// No atomlessness restriction here, and none needed: the two
			// laws this branch applies -- `ex x f = 0 <=> f_0 f_1 = 0` and
			// `ex x f != 0 <=> f_0 | f_1 != 0` (Boole's consistency
			// condition and its dual, one atom at a time) -- hold in ANY
			// Boolean algebra. What does need atomlessness is the
			// distribution of a block over SEVERAL atoms, and both places
			// that do it now carry their own guard: step 2a via
			// `block_atom_profile::all_negated`'s `finite_ba_content`, and
			// `eliminate_block_over_clause`'s two negative-atom
			// constructions via `is_bv_type_family` on the clause type.
			// (The former TODO (HIGH) here named exactly those two.)
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
					if (el.skip(var)
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
				//
				// "Before blasting" holds within one pass only.
				// eliminate_bv_and_quantifiers calls this function
				// three times and is itself re-entered from the
				// interpreter's fixpoint loops, so a later pass meets
				// scopes an earlier one already blasted -- measured on
				// the spec below, 15623 scopes reached this point and
				// only the first 6 predated any blasting, with 1530
				// blasting rewrites in between. That is why the open
				// branch below screens for blasting residue; see there.
				//
				// The whole solver branch below -- the closed check
				// and the open-scope closing trick alike -- is gated
				// on `solver_placement == eager`. Off it, the code
				// falls through to the blasting attempt and the
				// excluded.insert() below, which is the path an
				// unsolvable scope already takes.
				// Initialized: under non-eager placement the assigning
				// call below is short-circuited away, yet the blasting
				// gate reads bv_reason unconditionally.
				bv_unsolvable_reason bv_reason = bv_unsolvable_reason::ok;
				if (solver_placement == solver_site::eager
					&& is_bv_solvable_formula<node>(n, bv_reason)) {
					// Only commit to T/F on a definite answer: cvc5
					// returning unknown, or translation failing, means
					// we cannot decide, not that the formula is false.
					const trefs& fv = get_free_vars<node>(n);
					if (fv.empty()) {
						// Same residue screen as the open branch below: `n`
						// may already carry a blasting-introduced quantifier
						// alternation from a prior pass, even though this
						// call adds no new binder itself -- screening avoids
						// handing cvc5 a closed query it cannot terminate on.
						if (!has_blasting_residue<node>(n)) {
							std::optional<bv_sat_status> status =
								bv_formula_sat_status<node>(n);
							if (status == bv_sat_status::sat) return tau::_T();
							if (status == bv_sat_status::unsat) return tau::_F();
							DBG(if (!status) LOG_TRACE << "solver undecided on " << LOG_FM(n);)
						}
					} else if (!has_blasting_residue<node>(n)) {
						// The residue screen is what keeps this branch
						// from hanging the process. Closing the free
						// variables of an already-blasted scope wraps the
						// auxiliary quantifiers blasting introduced in a
						// universal block; that alternation turns on
						// cegqi-innermost=false (see
						// config_cvc5_solver_alternating_quantifiers) and
						// cvc5's counterexample-guided instantiation then
						// does not terminate -- stack samples show >1300
						// nested CegInstantiator::constructInstantiation
						// frames inside one checkSat, memory climbing, no
						// result. checkSat carries no time or resource
						// bound and none can be added (a truncated
						// instantiation search reports a definite wrong
						// answer: see the warning on
						// config_cvc5_solver_alternating_quantifiers), so
						// the whole run hangs with no output. On
						//
						//   r always u[t] = i0[t]
						//   always ( (((i1[t]:bv[4] + i2[t]:bv[4])
						//     + (i5[t]:bv[4] !^ i6[t]:bv[4])) >= {1}:bv[4])
						//     || (((i1[t]:bv[4] + i2[t]:bv[4])
						//     + (i5[t]:bv[4] !^ i6[t]:bv[4])) < {1}:bv[4]) ).
						//
						// this took cvc5 from 15623 queries, the last of
						// which never returned, to 51 that all do.
						//
						// Deliberately only this branch: the closed query
						// above synthesises no binder, so it cannot build
						// that alternation, and screening it too costs
						// real time (~78s -> ~112s on the 2-clause
						// variant of the spec above) by pushing scopes
						// cvc5 decides cheaply into Boole decomposition.
						//
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
				// Blasting can only make progress on a scope
				// is_bv_solvable_formula rejected for a reason it can
				// actually resolve (embedded arithmetic/comparisons it
				// rewrites). A variable typed outside the bv family has
				// no arithmetic on it to rewrite, so blasting cannot
				// close that gap; leave it unresolved for the outer
				// machinery instead, like every other give-up path here.
				if (bv_blasting
					&& blast_placement == blast_site::per_leaf
					&& bv_reason != bv_unsolvable_reason::non_bv_variable)
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
