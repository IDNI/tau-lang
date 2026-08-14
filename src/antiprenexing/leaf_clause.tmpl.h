// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "leaf_clause"

// ---------------------------------------------------------------------------
// THE TWO OPPOSITE PRECONDITIONS -- read this before "simplifying" either
// normalisation call below into a single shared one. They genuinely conflict.
//
// `squeeze_positives` (normal_forms_transformations.tmpl.h:382-385) DBG-asserts
// its input holds no `wff_neg(bf_eq)`, and it selects with
// `select_top(is<bf_eq>)`. `select_top` descends THROUGH a `wff_neg`, so the
// equation inside a `!(g = 0)` is folded into the *positive* squeeze, while the
// disequation scan that follows matches only `bf_neq` and never re-adds the
// negation. On `ex x (x a = 0 && !(x b = 0))` that gives f = x(a|b) and
// f_0*f_1 = 0 => T: the disequation inverted and silently dropped. So the
// single-variable squeeze path needs negatives spelled `bf_neq`, which `to_nnf`
// establishes.
//
// The block squeeze path needs the EXACT OPPOSITE: a `bf_neq`-free clause,
// because it classifies conjuncts by `bf_eq` / `wff_neg(bf_eq)` and a surviving
// `bf_neq` matches neither. `normalize_atomic_formula_operators` establishes
// that spelling.
//
// Neither spelling can be made global. `squeeze_positives` has a second caller
// outside anti-prenexing at `interpreter.tmpl.h:1156`, so its precondition is
// not ours to change; and the rest of this pipeline canonicalises on the
// `!(= 0)` spelling. Each precondition is therefore established locally, at its
// own point of use. Both calls are no-ops on input already in the right form.
// `tests/unit/test_leaf_clause.cpp` pins both paths against the collapse,
// through both public entry points.
// ---------------------------------------------------------------------------

namespace idni::tau_lang {

/**
 * @internal
 * @brief `true` if this conjunct is a shape the squeeze below can consume.
 *
 * The two recognised spellings are `f = 0` and `!(f = 0)`. Anything else --
 * `bf_lt`, a `wff_ref`, a surviving nested binder -- is treated as frozen by
 * the caller rather than clearing an all-or-nothing flag, which is what lets a
 * single unrecognised conjunct cost only its own component instead of the whole
 * clause.
 * @endinternal
 */
template <NodeType node>
bool is_squeezable_conjunct(tref conj) {
	using tau = tree<node>;
	const tau& c = tau::get(conj);
	return c.child_is(tau::bf_eq)
		|| (c.child_is(tau::wff_neg) && c[0][0].child_is(tau::bf_eq));
}

/** @internal @copydoc eliminate_block_over_clause @endinternal */
template <NodeType node>
tref eliminate_block_over_clause(tref clause, const trefs& block,
	const block_eliminability<node>& elim,
	const typename term_handle<node>::order& order)
{
	using tau = tree<node>;
	(void) order; // reserved: the squeezes below build their own orders

	if (tau::get(clause).equals_T() || tau::get(clause).equals_F())
		return clause;
	// If there are no quantifiers to remove, the clause can be returned
	if (block.empty()) return clause;

	// ---- Lift the conjuncts no block variable touches --------------------
	//
	// Absorbed from `treat_ex_quantified_clause`. A conjunct free of every
	// block variable is not in the block's scope in any meaningful sense, and
	// carrying it into the squeeze would distribute the block over a term it
	// does not occur in. `push_ex_block_into_clause` used to only DBG-assert
	// this could not happen; doing the lift makes the assumption true instead
	// of assumed.
	tref indep = _T<node>();
	trefs conjs;
	for (tref c : get_cnf_wff_clauses<node>(clause)) {
		bool touches = false;
		for (tref v : block)
			if (contains<node>(c, v)) { touches = true; break; }
		if (touches) conjs.push_back(c);
		else indep = tau::build_wff_and(indep, c);
	}
	// Nothing in the clause depends on the block at all: under the standing
	// non-empty domain assumption every binder drops.
	if (conjs.empty()) return indep;

	// ---- BA-type homogeneity, over the DEPENDENT conjuncts only ----------
	//
	// The squeeze needs one BA type: `build_bf_or` seeds its accumulator
	// from `_0<node>(clause_type)`, so a heterogeneous input silently
	// produces a wrongly-typed term.
	//
	// Scanned AFTER the lift, and over `conjs` rather than the whole clause,
	// which is what makes this usable at all. A mixed `:tau` / `:bv[N]` spec
	// -- every `run` over mixed streams -- reaches here with a clause whose
	// conjuncts are individually homogeneous but collectively are not, and
	// the block's own variables only ever occur in conjuncts of the block's
	// type. Scanning the whole clause declined all of those: measured
	// 2026-08-04, that is what left the `all i2[1]:bv[8] (...)` block
	// standing in `test_integration-interpreter`'s "nested conditionals over
	// mixed tau/bv streams stay sat" (issue #70) once the legacy fallback
	// stopped rescuing it, making the step system unsolvable and the run
	// report "Tau specification is unexpectedly unsat".
	//
	// A runtime guard, deliberately with no DBG assert beside it. It used to
	// carry one, on the reading that a heterogeneous clause meant the
	// quantifiers had been pushed in wrongly upstream. The same experiment
	// disproved that: `test_integration-splitter` reaches this line
	// legitimately and Release, which only had the guard, passed 26/26.
	// Mixing BA types is a shape this algorithm meets; declining is the
	// answer, not dying.
	size_t clause_type = 0;
	bool types_homogeneous = true;
	auto note_type = [&](size_t t) {
		if (t == 0) return;
		if (clause_type == 0) clause_type = t;
		else if (clause_type != t) types_homogeneous = false;
	};
	// The first *typed* block variable: collect_quantifier_block lets
	// untyped (get_ba_type() == 0) variables join a typed run, so
	// block.front() is not guaranteed to carry the run's type.
	for (tref v : block) note_type(tau::get(v).get_ba_type());
	auto type_scan = [&](tref n) {
		const size_t t = tau::get(n).get_ba_type();
		if (t == 0) return true;
		note_type(t);
		return false;
	};
	for (tref c : conjs) pre_order<node>(c).visit(type_scan);
	if (!types_homogeneous) {
		LOG_ERROR << "eliminate_block_over_clause: dependent conjuncts "
			"mix BA types, keeping the quantifier block: "
			<< LOG_FM(clause);
		tref kept = tau::build_wff_and(conjs);
		for (auto v = block.rbegin(); v != block.rend(); ++v)
			kept = build_wff_ex<node>(*v, kept, false);
		return tau::build_wff_and(indep, kept);
	}

	// ---- Partition the block by eliminability component ------------------
	//
	// A variable whose component reaches an unresolved reference, a kept
	// binder or a shape the squeeze cannot consume is re-wrapped around only
	// *its own* conjuncts; the rest of the clause is eliminated as normal.
	//
	// `elim` is a SEED, not the last word. `verdict_of` is keyed on the
	// block variables, which the caller's recursion never rewrites, so a
	// verdict stays meaningful however deep this call sits. `conjuncts_of`
	// is keyed on the conjuncts of the block's *original* body, and the
	// recursion above rebuilds conjuncts (Boole splits, gamma folds, the
	// `atm && l` re-conjunction), so those trefs can be stale here. Trusting
	// them outright would let a rewritten conjunct still holding a kept
	// variable land in the eliminated part and leak that variable free.
	// So the reserved set is closed to a fixpoint against the conjuncts
	// actually in front of us, which yields the two invariants the split
	// needs -- no kept variable occurs in a free conjunct, and no live
	// variable occurs in a kept conjunct -- by construction rather than by
	// assumption.
	subtree_unordered_set<node> kept_set, reserved;
	for (tref v : block)
		if (elim.verdict_of(v) != elim_verdict::eliminable)
			kept_set.insert(v);
	// Marking by identity rather than by index keeps this correct when two
	// kept variables share a conjunct. Entries that are stale (not among
	// `conjs`) simply never match and are inert.
	for (tref v : block)
		if (kept_set.contains(v))
			for (tref c : elim.conjuncts_of(v)) reserved.insert(c);
	// An unrecognised conjunct shape freezes its own component rather than
	// the whole clause. This replaces `push_ex_block_into_clause`'s
	// `is_quant_removable_in_clause` flag: that flag re-wrapped the entire
	// block around the entire clause on the first `bf_lt` it met, which is
	// the all-or-nothing behaviour this module exists to remove.
	for (tref c : conjs)
		if (!is_squeezable_conjunct<node>(c)) reserved.insert(c);
	// `contains` (syntactic subtree occurrence), not `get_free_vars`: it is
	// a superset of free occurrence -- it also counts an occurrence under a
	// nested binder -- and over-reserving is sound where under-reserving is
	// not.
	for (bool changed = true; changed; ) {
		changed = false;
		for (tref c : conjs) {
			bool touches_kept = reserved.contains(c);
			if (!touches_kept)
				for (tref v : block)
					if (kept_set.contains(v)
						&& contains<node>(c, v))
					{
						touches_kept = true;
						break;
					}
			if (!touches_kept) continue;
			if (reserved.insert(c).second) changed = true;
			for (tref v : block)
				if (!kept_set.contains(v)
					&& contains<node>(c, v))
				{
					kept_set.insert(v);
					changed = true;
				}
		}
	}
	trefs live, kept_vars;
	for (tref v : block)
		(kept_set.contains(v) ? kept_vars : live).push_back(v);
	trefs kept_conjs, free_conjs;
	for (tref c : conjs)
		(reserved.contains(c) ? kept_conjs : free_conjs).push_back(c);

	// The frozen part: its conjuncts carry no live variable and its binders
	// bind no variable occurring outside it, so it detaches from the squeeze
	// as an ordinary conjunct. A kept variable with no conjunct of its own
	// occurs nowhere in the clause, so dropping its binder is sound under the
	// standing non-empty domain assumption.
	auto with_kept = [&](tref rest) {
		tref kept_part = _T<node>();
		if (!kept_conjs.empty()) {
			kept_part = tau::build_wff_and(kept_conjs);
			for (auto v = kept_vars.rbegin(); v != kept_vars.rend();
				++v)
				kept_part = build_wff_ex<node>(*v, kept_part,
					false);
		}
		return tau::build_wff_and(indep,
			tau::build_wff_and(kept_part, rest));
	};

	if (live.empty()) {
		// Nothing left to eliminate. Re-wrap the dependent part whole,
		// which is what the frozen path above already expresses.
		return with_kept(_T<node>());
	}

	tref scoped = tau::build_wff_and(free_conjs);

	// Drop live binders that bind nothing. A variable occurring nowhere in
	// the part about to be squeezed is unconstrained, so under the standing
	// non-empty domain assumption its binder simply goes -- the same identity
	// the `live.empty()` and independent-conjunct paths above already use.
	//
	// This is routing, not just tidying. The squeeze below has two paths, and
	// only the single-variable one substitutes constants INTO reference
	// arguments (`g(0)`, `g(1)`), which is the only way to settle a variable
	// under an unresolved `bf_ref`. A vacuous second binder is enough to send
	// a clause that the single-variable path would resolve to the block path,
	// which cannot: `ex b1, b2 (g(b1) != 0)` is exactly that shape.
	auto occurring_in = [&](const trefs& vs, tref in) {
		trefs r;
		for (tref v : vs) if (contains<node>(in, v)) r.push_back(v);
		return r;
	};
	live = occurring_in(live, scoped);
	if (live.empty()) return with_kept(scoped);

	// ---- A substitution witness, per live variable -----------------------
	//
	// Absorbed from `treat_ex_quantified_clause`: `ex x (x = t && phi(x))`
	// becomes `phi(t)`. Cheaper and more precise than the squeeze, so it is
	// tried first; a variable it settles leaves `live` and never reaches the
	// BDD. Re-checked against the *current* `scoped` each time, since an
	// earlier substitution can create the witness for a later variable.
	//
	// History, because this loop was once removed as "unsound": with the
	// legacy fallback off it made the issue #70 step formula normalise to F.
	// The fault was NOT here -- it was the γ1 cofactor branch of
	// `anti_prenex_block` dropping `¬atm` from its F-branch (see the fix
	// comment there), and this loop merely fed the core the reduced shapes
	// that reached γ1. With that fixed, the loop is back, and with it the
	// completeness it provides: without it, blocks it would discharge
	// survive and multiply across driver rounds
	// (`test_integration-interpreter` diverges, satisfiability2 is ~13x
	// slower).
	trefs still_live;
	for (size_t i = 0; i < live.size(); ++i) {
		if (tau::get(scoped).equals_T()
			|| tau::get(scoped).equals_F())
		{
			// Decided: every variable not yet visited is settled
			// too, since neither T nor F constrains any of them.
			still_live.clear();
			break;
		}
		if (tref e = ex_subs_based_elimination<node>(live[i], scoped);
			e != scoped)
		{
			scoped = e;
			continue;
		}
		still_live.push_back(live[i]);
	}

	if (tau::get(scoped).equals_T()) return with_kept(_T<node>());
	// An existential over F is F, independently of the variables.
	if (tau::get(scoped).equals_F()) return _F<node>();
	// Again after substitution: eliminating one variable can remove the last
	// occurrence of another.
	still_live = occurring_in(still_live, scoped);
	if (still_live.empty()) return with_kept(scoped);

	// ---- Bitvector content: the solver, then blasting --------------------
	//
	// Absorbed from `treat_ex_quantified_clause`, and scoped by the analysis
	// rather than by a bare type test: `blasteable` is exactly "bv-typed
	// and cvc5 can be expected to translate this formula", which is the
	// question this branch needs answered. Gating on the verdict rather than
	// on `is_bv_type_family` alone is what keeps a formula carrying a foreign
	// BA constant out of a solver that cannot represent it.
	//
	// Currently unreachable through the block driver, which diverts a
	// skip-matched block to `blast_block` before a clause ever gets here; it
	// becomes live when the legacy path is rewired through this module.
	for (tref v : still_live) {
		if (elim.verdict_of(v) != elim_verdict::blasteable) continue;
		if (!is_bv_type_family<node>(tau::get(v).get_ba_type())) continue;
		const trefs& free_vars = get_free_vars<node>(scoped);
		if ((free_vars.empty() || (free_vars.size() == 1
			&& tau::get(free_vars[0]) == tau::get(v)))
			&& is_bv_solvable_formula<node>(scoped))
		{
			// Closed bv formula: simplify to T/F, but only on a
			// definite answer. cvc5 returning unknown, or translation
			// failing, means we cannot decide -- not that it is false.
			std::optional<bv_sat_status> status
				= bv_formula_sat_status<node>(
					tau::build_wff_ex(v, scoped, false));
			if (status == bv_sat_status::sat)
				return with_kept(_T<node>());
			if (status == bv_sat_status::unsat) return _F<node>();
			DBG(if (!status) LOG_TRACE << "solver undecided";)
		}
		// Non-closed, or closed-but-undecided: blast the bv existential
		// into Boolean bit quantifiers and re-enter. The re-entry keeps
		// the bv skip: the blasted atoms stay bv-typed, so handing them
		// to generic Boole decomposition would build BDD leaves backed by
		// solver terms -- orders of magnitude costlier per node than
		// atomless ones.
		if (bv_blasting) {
			tref ex_fm = tau::build_wff_ex(v, scoped, false);
			if (auto blasted = bv_predicate_blasting<node>(ex_fm);
				blasted && blasted != ex_fm)
				return with_kept(anti_prenex<node>(blasted,
					eliminability<node>::bv_only()));
		}
		// Not resolvable: keep this binder around the scoped part.
		return with_kept(tau::build_wff_ex(v, scoped, false));
	}

	// ---- The squeeze -----------------------------------------------------
	if (still_live.size() == 1) {
		// Single variable: the `f_0 * f_1 = 0` construction, which also
		// substitutes constants INTO reference arguments (`g(0)`, `g(1)`)
		// and so can settle a variable the block squeeze cannot.
		// `to_nnf` establishes this path's precondition -- see the file
		// comment.
		tref var = still_live.front();
		scoped = to_nnf<node>(scoped);
		const size_t type_v = find_ba_type<node>(var);
		trefs neqs = tau::get(scoped).select_top(is<node, tau::bf_neq>);
		// ---- Atomless-only: the mixed `= 0` / `!= 0` construction ----
		//
		// The `neqs` block below is Corollary 5.1 with J1 = {f = 0}: it
		// answers `ex x (f = 0 && g != 0)` with the CONJUNCTION
		// `f_0 f_1 = 0 && (f_1' g_1 | f_0' g_0) != 0`, which needs a witness
		// strictly between the two cofactors and so holds only in an ATOMLESS
		// Boolean algebra. bv is atomic: at bv[1], `ex x (x != 0 && x' != 0)`
		// is F, while this construction (no positives, f_0 = f_1 = 0) builds
		// `(g_0 | g_1) != 0` per disequation and answers T for both.
		//
		// The guard names both atomic (finite) BA families -- bv and bool --
		// spelled exactly as step 2a's own atomlessness guard spells it
		// (`profile_block_atoms`, block_atom_profile.tmpl.h). `bool` is the
		// two-element BA, so it is maximally atomic and breaks this law even
		// harder than bv[1] does; nothing floors it, so a bool-typed block
		// reaches this squeeze. Where the guard declines, the existing
		// decline path applies: re-wrap the binder, exactly as the
		// heterogeneous-type and unrecognised-shape branches do. The
		// positive-only construction underneath (`f_0 f_1 = 0`) is Boole's
		// consistency condition and is valid in any Boolean algebra, so only
		// the `neqs` case is guarded.
		//
		// normalize_atomic_formula_operators on the way out for the same
		// reason the success path takes it: `to_nnf` above spelled the
		// negatives `bf_neq`, and no caller may be able to tell which path ran.
		if (!neqs.empty() && type_v > 0
			&& (is_bv_type_family<node>(type_v)
				|| is_bool_type<node>(type_v))) {
			DBG(LOG_TRACE << "eliminate_block_over_clause: atomic BA "
				"with disequations, keeping the binder: "
				<< LOG_FM(scoped) << "\n";)
			return normalize_atomic_formula_operators<node>(
				with_kept(tau::build_wff_ex(var, scoped, false)));
		}
		tref f = squeeze_positives<node>(scoped, type_v);
		tref f_0 = f ? rewriter::replace<node>(f, var,
			tau::_0_trimmed(type_v)) : tau::_0(type_v);
		tref f_1 = f ? rewriter::replace<node>(f, var,
			tau::_1_trimmed(type_v)) : tau::_0(type_v);
		tref out = _T<node>();
		if (neqs.size()) {
			tref nneqs = tau::_T();
			for (tref neq : neqs) {
				neq = norm_trimmed_equation<node>(neq);
				tref g = tau::trim(neq);
				tref g_0 = rewriter::replace<node>(g, var,
					tau::_0_trimmed(type_v));
				tref g_1 = rewriter::replace<node>(g, var,
					tau::_1_trimmed(type_v));
				// Both 1 => the disequation is implied by
				// f_0 f_1 = 0.
				if (tau::get(g_0).equals_1()
					&& tau::get(g_1).equals_1()) continue;
				if (tau::get(f_0) == tau::get(f_1))
					nneqs = tau::build_wff_and(nneqs,
						tau::build_bf_neq_0(
							tau::build_bf_or(g_0, g_1)));
				else if (tau::get(g_0) == tau::get(g_1))
					nneqs = tau::build_wff_and(nneqs,
						tau::build_bf_neq_0(g_0));
				else nneqs = tau::build_wff_and(nneqs,
					tau::build_bf_neq_0(tau::build_bf_or(
						tau::build_bf_and(
							tau::build_bf_neg(f_1), g_1),
						tau::build_bf_and(
							tau::build_bf_neg(f_0), g_0))));
			}
			out = tau::build_wff_and(
				tau::build_bf_eq_0(tau::build_bf_and(f_0, f_1)),
				nneqs);
		} else if (f) {
			out = tau::build_bf_eq_0(tau::build_bf_and(f_0, f_1));
		}
		// The normal form is taken over the squeeze result alone, not over
		// what `with_kept` wraps around it: the frozen part still carries
		// binders, and term_boole_normal_form is a decomposition over
		// reference terms, not something to point at a quantified scope.
		//
		// normalize_atomic_formula_operators on the way OUT, undoing the
		// `to_nnf` above. The two squeeze paths need opposite spellings
		// internally, but the module must not leak that choice: the rest
		// of the pipeline canonicalises negatives as `!(= 0)`, and a
		// caller should not be able to tell which path ran. Without this,
		// the same clause comes back spelled `bf_neq` from the
		// single-variable path and `!(= 0)` from the block path.
		return normalize_atomic_formula_operators<node>(
			with_kept(term_boole_normal_form<node>(out)));
	}

	// Several variables: squeeze the positives into one term and adjust the
	// negatives, one BDD quantifier call for the whole block.
	// `normalize_atomic_formula_operators` establishes this path's
	// precondition -- the opposite one. See the file comment.
	scoped = normalize_atomic_formula_operators<node>(scoped);
	trefs pos, neg;
	for (tref conj : get_cnf_wff_clauses<node>(scoped)) {
		const tau& c = tau::get(conj);
		if (c.child_is(tau::bf_eq))
			pos.push_back(tau::trim2(norm_equation<node>(conj)));
		else if (c.child_is(tau::wff_neg) && c[0][0].child_is(tau::bf_eq))
			neg.push_back(tau::trim2(
				norm_equation<node>(c.trim2())));
		else
			// The partition above reserved every unrecognised shape,
			// and the substitution loop only ever removes conjuncts.
			// A shape reaching here means one was rebuilt into an
			// unrecognised form in between, so decline: re-wrap the
			// live block around the scoped part rather than dropping
			// the conjunct.
			{
				tref keep = scoped;
				for (auto v = still_live.rbegin();
					v != still_live.rend(); ++v)
					keep = build_wff_ex<node>(*v, keep,
						false);
				return with_kept(keep);
			}
	}
	// ---- Atomless-only, the block-squeeze half of the same law -------------
	//
	// The negative adjustment below is Corollary 5.1 again, with
	// J1 = {f = 0}: it distributes the block over the disequations, keeping
	// `ex X (f = 0)` and one `ex X (f' g != 0)` per negative. Same atomless
	// precondition as the single-variable `neqs` construction above, same
	// atomic (finite) BA families -- bv and bool, the same pair and the same
	// spelling step 2a's guard uses -- same bv[1] counterexample, same decline:
	// re-wrap the live block around the scoped part, the shape the
	// unrecognised-conjunct branch above already uses. `scoped` is already in
	// the `!(= 0)` spelling here, so unlike the single-variable path this needs
	// no normalisation on the way out.
	//
	// The positive half needs no guard: `f1 = 0 && f2 = 0 <=> f1|f2 = 0` and
	// the single-atom quantifier resolution that follows it hold in any
	// Boolean algebra.
	if (!neg.empty() && clause_type > 0
		&& (is_bv_type_family<node>(clause_type)
			|| is_bool_type<node>(clause_type))) {
		DBG(LOG_TRACE << "eliminate_block_over_clause: atomic BA with "
			"negated conjuncts, keeping the block: "
			<< LOG_FM(scoped) << "\n";)
		tref keep = scoped;
		for (auto v = still_live.rbegin(); v != still_live.rend(); ++v)
			keep = build_wff_ex<node>(*v, keep, false);
		return with_kept(keep);
	}
	// Squeeze positive atomic formulas together -> result is a lazy BDD.
	// By assumption they all have the same type.
	tref f = build_bf_or<node>(pos, clause_type);
	tref res = build_bf_eq_0<node>(f);
	for (auto v = still_live.rbegin(); v != still_live.rend(); ++v)
		res = build_wff_ex<node>(*v, res, false);
	// Update all negative atomic formulas -> result is a lazy BDD
	for (tref& g : neg) {
		g = tau::build_bf_and(tau::build_bf_neg(f), g);
		g = tau::build_wff_neg(tau::build_bf_eq_0(g));
		for (auto v = still_live.rbegin(); v != still_live.rend(); ++v)
			g = build_wff_ex<node>(*v, g, false);
	}
	return with_kept(tau::build_wff_and(res, tau::build_wff_and(neg)));
}

} // namespace idni::tau_lang
