# Anti-prenexing — pseudocode baseline

Spec: Chapter 5 of `taba0.5.tex` (= ch. 5, p. 51 of `taba0.5.pdf`). Paper steps
`2a`–`2k` are cited inline; step `(3)`, the universal branch, is handled by
dualisation in `PROCESS_BLOCK`. Of steps `2e`–`2j` — one Boole decomposition
on a chosen pivot — the unique split (§6) keeps the one binder-killing pivot
class: the equation whose truth substitutes a binder variable away. Two
licensed decompositions (§6) restore the general step under no-duplication
tests: `TRY_DECOMPOSE` for the atom whose two signs sit in sibling
conjuncts, `TRY_CUT_DECOMPOSE` for the atom cheapest to cut. All remaining
case analysis is `EXPAND` (§6), distributing disjuncts the formula already
contains.

Two phases, sharply separated:

1. **Push** (§4–§6) — an existential block is pushed inward, by identities valid
   in any BA (invariant 1), until every variable scopes a single conjunctive
   clause — the whole block at once when the matrix allows, variable by
   variable (the settle move, §6) when it does not.
2. **Eliminate** (§7) — **type-specific**: the clause is handed to the method
   for the block's Boolean-algebra type.

Two assumptions from the paper (`taba0.5.tex:2881–2887`):

1. The formula is a **DAG sharing common subtrees**, so a term may be used many
   times without blowup. `EXPAND`'s state merging depends on it — equal states
   are equal hash-consed nodes — and it makes the memo tables hit and the
   result joins' membership test O(1).
2. `0x = 0`, `0 ∪ x = x`, `1x = x`, `1 ∪ x = 1` and constant-only atoms fold
   automatically — subsumed by `SIMPLIFY_TERM`/`SIMPLIFY_ATOM` (invariant 6).

Pseudocode only; baseline for further changes.

---

## 1. Notation

```
φ, ψ     formulas               f, g     terms (BFs)
x        one variable           X        block: ordered var list, outermost first
τ        the BA type of a block T, F     the truth constants
f₀, f₁   cofactors f[x←0], f[x←1]
f′       complement             ∪ · +    join, meet, ring sum
|φ|      cached node count, set at construction: 1 + Σ |children| — tree count
         over the hash-consed DAG (the sort convention of 2d, the case
         witness, and EXPAND, and the metric of §5's size acceptance), O(1)
         to maintain
‖φ‖      distinct formula nodes reachable from φ, a unit counted as one leaf
         (§4) — the DAG count against |φ|'s tree count, the currency of the
         cut licence (§6). Computed on first demand by one traversal and
         cached on the node
neg(φ)   the NNF of ¬φ — TO_NNF's NEG (§3), factored — a pure function of
         the node: computed on first demand and cached on it, like ‖φ‖
h(φ)     cached structural hash, set at construction from the node's kind and
         its children's hashes — wide enough that collisions are negligible.
         Every input is canonical (phase-0 binder ids, source names for free
         variables, BDD-canonical terms), so h is a pure function of CONTENT:
         identical across runs, components, and inputs sharing subtrees
FV(φ)    cached free-variable set, set at construction: union of the
         children's sets, leaf occurrences included, quantified subscripts
         excluded — every X ∩ FV test in this document is an intersection of
         cached sets, never a walk

CONTENT ORDER — the canonical total order on nodes: by h, ties broken by
    structural comparison (kind by a fixed rank, children left to right,
    variables by identifier). A pure function of content — the tie-break of
    last resort everywhere (result-join emission, EXPAND's disjunct key), so
    no order anywhere depends on construction history

functional quantifiers (lem:xelim):   ∀ₓf = f₀·f₁      ∃ₓf = f₀ ∪ f₁
  over a block: ∀_X, ∃_X — one BDD quantification, never expanded to 2^|X| terms

atom shapes reaching the core (post phase 3), each optionally under one ¬:
    f = g   |   f ≤ g   |   f < g
    — never f ≠ g, and no mirrored/negated order operator (≰ ≮ ≥ > ≱ ≯)
    — equations stay as written (invariant 4); order atoms are never rewritten
      into equations — never split by the push, they ride whole into clauses —
      consumed only by ELIMINATE_BITVECTOR_CLAUSE's solver path

discharge (both valid in ANY BA — thm:boole-const):
    ∃X. f = 0   ≡   ∀_X f = 0
    ∃X. f ≠ 0   ≡   ∃_X f ≠ 0

    ≠ is NOTATION ONLY from phase 3 on: an emitted `h ≠ 0` denotes the
    formula-level ¬(h = 0) (invariant 4) — no fused ≠ atom is ever built.

term representation, per component (PREPARE_TERMS):
    A term is a BDD whose DECISION VARIABLES are exactly the block's variables
    and whose LEAVES hold everything else: free variables, constants, references,
    foreign-typed sub-terms, and functional quantifiers over block-free bodies.
    This is thm:mnf over X only,
    f(x⃗,y⃗) = ⋃_{a ∈ 2^|X|} f(a,y⃗) · x⃗^a, with arbitrary BA coefficients.
    Consequences:
      - cofactoring on x ∈ X is CHILD SELECTION; nothing else is ever cofactored on
      - ∀_X f = meet of the leaves,  ∃_X f = join of the leaves — one traversal
      - substitution (TRY_WITNESS, at every call site) is a BDD compose
        PLUS rewriting x inside the leaves (r(x) ↦ r(t)) — sound for any term,
        reaches every occurrence, needs no leaf guard. Every reference argument
        it touches is re-emitted through SIMPLIFY_TERM (a formula argument
        through SIMPLIFY): phase 1 simplifies arguments at entry (§3), and
        substitution is the only rewrite that reaches inside a reference (§4),
        so arguments are ALWAYS simplified (inv. 6) — paid only where a
        rewrite happened. This matters: references are leaves, the syntactic
        tests live on leaf merging (§3), and the FV guards (X ∩ FV, `usable`,
        COFACTOR_REDUCE) stay needlessly conservative while a dead occurrence
        like x·x′ ∪ a survives inside an argument. SIMPLIFY_TERM is any-BA —
        the argument's type is immaterial
      - depth is bounded by |X|, not by the formula's variable count
      - FUNCTIONAL QUANTIFIERS ARE TRANSPARENT — term operators binding their
        subscript: Y is excluded from FV and alpha-renamed with the formula
        binders. Cofactoring on x ∉ Y commutes with them, so PREPARE_TERMS
        backs the BODY over P and slides the quantifier onto the body's
        leaves — ∀_Y(x·b₁ ∪ x′·b₀) = x·∀_Y b₁ ∪ x′·∀_Y b₀, dually for ∃_Y: a
        Y-substitution is a homomorphism fixing x, and meet/join distribute
        over an orthogonal decomposition. Linear in the body's BDD; nesting
        recurses innermost-first. Under keep_functional, deep block nesting
        stacks quantifier chains at the leaves — linear per level, amortised
        by hash-consing and quant_memo, inherent to symbolic mode.
      - LEAF HAZARD: a block variable may occur inside a leaf (a reference
        argument), where cofactoring does not reach — a leaf is opaque, its
        Shannon expansion may not be assumed — so a leaf-x makes f₀/f₁ not
        cofactors at all. A functional quantifier never hides a block
        variable — only a reference or foreign subterm does, including one
        inside a quantifier's body. Guarded by `usable` (COF) and
        COFACTOR_REDUCE's FV check
```

`ctx` carries:

| field | meaning |
|---|---|
| `type` | the block's BA type `τ`. Single, by invariant 2 |
| `order` | BDD variable order. inner → **lower** rank |
| `prio` | variable priority. inner → **higher** rank — read by `EXPAND`'s disjunct key, `CHOOSE_SPLIT`'s candidate order, and `FOLD_DECIDED`'s probe variable |
| `subsume_max` | threshold on a clause's negative count, above which `SUBSUME_NEGATIVES` (finite method only) skips its O4 scan. `K = 32`, provisional pending benchmarks — free to tune: subsumption costs no precision, so neither taint nor flush attaches (unlike `qbf_node_max` — cache scope, below) |
| `qbf_node_max` | node budget for `DECIDE_FINITE`'s BDD sweep — peak live nodes of ONE sweep, checked by allocation high-water mark — past which it falls back to `ASK`. `K′ = 2²⁰`, provisional pending benchmarks |
| `case_max` | threshold on a case pin's branch count, above which the case witness (`TRY_CASE_WITNESS`, `TRY_WITNESS_DEEP`) declines — each branch copies the surrounding spine, repaid only by the deleted binder. Past it phase 4 degrades gracefully: the unique split peels the same conjunct two arms at a time. `K″ = 16`, provisional pending benchmarks — free to tune. Phase 2's `TRY_WITNESS_DEEP` predates any ctx and reads the constant `K″` bare |
| `expand_max` | budget on cases built by `EXPAND` — shared by the split's and both decompositions' arms (§6) — counted per component (`ctx.expand_count`) — past it, the still-unexpanded members re-wrap as ONE pending block and every finished case is kept (inv. 3). `K‴ = 2¹⁴`, provisional pending benchmarks — a hit TAINTS its computation (cache scope, below): the result is returned, never cached, so a raise regains precision with no flush |
| `accept_growth` | growth factor of the per-component SIZE ACCEPTANCE (§5): a component push whose result exceeds `max(γ·\|input\|, accept_floor)` is discarded for the re-wrapped input (inv. 3). `γ = 16`, provisional pending benchmarks — free to tune: the verdict is re-derived outside every cache and acts only through the formula it re-wraps, so neither taint nor flush attaches (cache scope, below) |
| `accept_floor` | absolute `\|·\|` under which acceptance never fires. Moderate growth is routine and often repaid downstream; the test exists for DETONATION — the multiplicative regime where one block's result sits orders beyond repayment. `2²⁰`, provisional pending benchmarks |
| `keep_functional` | emit `∀_X`/`∃_X` symbolically instead of discharging them; in the `push_memo`/`elim_memo` keys (cache scope, below) |
| `push_memo` | `(φ, X, keep_functional) → formula`, GLOBAL (cache scope, below). Also the state memo of `EXPAND`: an expansion state IS its formula, and merging is this table firing on canonically assembled children (§6) |
| `elim_memo` | `(clause, X, keep_functional) → formula`, GLOBAL. The key names everything an elimination reads, so it is exact |
| `quant_memo` | functional-quantifier term → term, GLOBAL — the key IS the query (`ASK`'s convention) and names term, kind, and quantified set in one node; entries are pure functions of it. `SETTLE_FUNCTIONAL`'s discharges may share it (see `DISCHARGE`) |
| `cof_memo` | `(settled term, x) → (f₀, f₁, p, usable)`, GLOBAL. Filled and read by `COF`; consumers: the phase-4 pin matches (pin iff `usable ∧ f₁ = f₀′`, with `t = f₀`), `ADMIT`'s UNIQUE classification (the same test — a pin and a UNIQUE pivot are one property, met at different positions), and `FOLD_DECIDED`; pure functions of the key — the settled term already reflects `keep_functional` |
| `occ_memo` | `formula node → { equation atom ↦ (bare, in_or, count) }`, GLOBAL — filled by `OCC` (§6, the unique split); purely structural, unit-opaque; order atoms never enter (no UNIQUE class exists for them) |
| `atoms_memo` | `formula node → the atoms occurring in it`, units opaque (§4), GLOBAL — purely structural. Read by the occurrence guard of `[atm ↦ T/F]` (§10) and by `CUT_WALK`'s incidence graph (§6): a conjunct's atoms are one lookup, never a walk |
| `pool` | `atom → (UNIQUE verdict at each block variable of the atom in prio order, admission index)`, per COMPONENT — recorded lazily by `ADMIT` (§6, the unique split); prio-dependent, so scoped like `prio` itself. Admission order is a function of the component input alone, making the last-resort tie-break canonical |
| `solver_memo` | canonical closed query → `sat`/`unsat`/`unknown`. GLOBAL — valid per solver configuration, flushed when it changes (cache scope, below) |
| `qbf_memo` | canonical closed pure-Boolean query → `T`/`F`, written only by `DECIDE_FINITE`'s own sweep. GLOBAL — entries are mathematical truths, never flushed (cache scope, below) |

The two rank conventions are opposite and cannot be merged: `order` is consumed
innermost-first by BDD quantification; `prio` is read by `EXPAND`'s disjunct
key, which expands the disjunct whose latest variable is outermost first, by
`CHOOSE_SPLIT`, which prefers the innermost pinned variable, and by
`FOLD_DECIDED`, which probes on the innermost variable of an atom.

**Cache scope** follows one rule: a table is GLOBAL — outliving the call,
shared across `ANTI_PRENEX` runs, components, and blocks — exactly when its key
names everything its entries depend on. The variable set is explicit: the
ordered `X` of `push_memo`/`elim_memo` fixes the type, every atom's block
subset (`FV(atm) ∩ P = FV(atm) ∩ X`), and the relative prios; `cof_memo`
carries its `x`; `quant_memo`'s key is the query itself; `occ_memo` and
`atoms_memo` are purely structural; `solver_memo` and `qbf_memo` involve
none. `pool` is the one prio-dependent table and scopes with the component
accordingly. The mode:
`keep_functional` joins `push_memo`'s and
`elim_memo`'s keys alone — their entries embed `DISCHARGE`'s output, which the
flag switches — while `cof_memo` needs no flag, its settled-term key being
formed AFTER `SETTLE_FUNCTIONAL` applied the mode. Every tie-break anywhere is
the content order (§1) — a pure function of content — so no cached entry
embeds construction history. One deliberate exception: the cut walk a
decomposition arm inherits (§6) sits in no key. It steers only WHICH of
several equivalent results a push builds, so an entry computed under any
walk serves every caller; the price is that a `push_memo` entry is a
function of its key only up to equivalence. One caveat: five configuration
knobs sit in no key — the solver configuration, `qbf_node_max`,
`expand_max`, and the acceptance pair `accept_growth`/`accept_floor`. The
budgets are handled by TAINT, the solver configuration by a flush, the
acceptance pair by NEITHER: its verdict is re-derived outside every cache
(§5, after the completed component push) and acts only by re-wrapping a
formula — later keys contain that formula, so no entry embeds the verdict,
and a re-tuned γ or floor simply re-judges the same pushes. TAINT: a
computation that hit a budget — an `expand_max` exhaustion re-wrap, a
`qbf_node_max` sweep abandoned to `ASK` that ended `unknown` — returns its
result (sound, inv. 3) but writes NO `push_memo`/`elim_memo` entry, and
taint is TRANSITIVE: a result assembled from a tainted subresult is tainted
too. So no entry embeds budget or counter state — in particular not
`ctx.expand_count`'s value at computation time — and raising a budget needs
no flush; the price is recomputation where the budget bound. The FLUSH:
`solver_memo` is valid per solver configuration and is flushed when it
changes; a `push_memo`/`elim_memo` entry that embedded a genuine solver
`unknown` (budget-free — the budget-involved ones were tainted, never
written) stays sound (inv. 3) yet
blind to a stronger solver — flush both on a configuration change to reclaim
that precision. `qbf_memo` is exempt from every flush:
`DECIDE_FINITE` records there only verdicts its own BDD sweep produced —
mathematical truths, valid under any configuration — while verdicts that came
through its `ASK` fallback live in `solver_memo` under that table's flush rule.

---

## 2. Invariants

1. **The push holds no type-specific reasoning and discharges nothing itself.**
   Every identity it uses holds in any BA; the exception is 2a, which *asks*
   the type table (`EX_DISTRIBUTES_OVER_NEGATIVES`) rather than deciding
   anything. Every elimination goes through §7. Atomlessness is asserted in
   exactly two places — that capability and `ELIMINATE_ATOMLESS_CLAUSE`'s
   per-negative split (one `NEGATIVE_CONDITION` conjoined per negative) — and
   they are two uses of **one** theorem (`cor:Multivariate-BFs-over`).
2. **Type homogeneity is derived, not required.** A term carries one BA type, so
   no atom mentions variables of two types, so the shared-atom partition in
   `PUSH_EX_BLOCK` cannot merge across types — every component is
   type-homogeneous, which makes `ctx.type` well defined. `COLLECT_RUN` does no
   type test. A mixed-type component arises only through a non-atomic conjunct
   (a reference with arguments of several types, a nested binder), and every
   elimination method freezes those it cannot read — a reference always, a
   surviving binder unless its body is translatable for the method's engine, in
   which case the unit is swallowed whole into a query (§7), never opened.
3. **A surviving quantifier is sound.** Callers read it as *undecided*, never as
   false. Every graceful exit re-wraps; no method may answer `F` for "cannot
   decide".
4. **Negation is at formula level** from phase 3 onward — `¬(f = 0)`, never
   `f ≠ 0`, and no fused negated order operator. Equations are *not*
   zero-normalised; `SQUEEZE` — shared by `ELIMINATE_ATOMLESS_CLAUSE` and
   `ELIMINATE_FINITE_CLAUSE` (§7, shared helpers) — is the only place that
   rewrites one.
5. **Case analysis stays inside the formula's own atom vocabulary — no
   manufactured atoms.** Every multiplying step — 2d, the case witness, the
   unique split, both decompositions, `EXPAND` — distributes members the
   formula already contains or splits on an atom it already contains: the
   split's F-arm conjoins the NEGATION of an atom of ψ while `[atm ↦ F]`
   erases every other occurrence, and `EXPAND`'s exclusions are complements
   of literal members of a disjunct of ψ, so the vocabulary never grows and
   the atom is RETIRED rather than barred — bare selection (§6) can never
   take it again, and no barred-atom set exists.
   What the rest of Boole decomposition provided is recovered piecewise: a
   pin substitutes out through the witness step (spine level), the case
   witness (all branches of one disjunct), and the unique split (any bare
   occurrence, however deep); the licensed decompositions take the
   both-signed atom whose worlds span sibling conjuncts, then any atom cheap
   to cut — both cofactors erase every reachable occurrence, and the guard
   left behind is one-signed for the first step's census and a lone
   top-level literal for the second's candidate rule, so neither re-selects
   its atom (retired, not barred); a constant-false atom folds through
   `FOLD_DECIDED`, a finished variable leaves through scope narrowing's
   settle move, and the rest is decided at the leaves, where the clause
   methods are complete.
6. **Everything the algorithm builds is simplified at the point of
   construction** — an atom by `SIMPLIFY_ATOM`, a cofactor by `SIMPLIFY_TERM`, a
   formula built by substituting into one or conjoining onto one by `SIMPLIFY` —
   before it is recursed into. *Selecting* a subtree needs nothing: a disjunct of
   a simplified disjunction is already simplified, and dropping conjuncts only
   makes a conjunction less contradictory. *Assembling results* needs only their
   top-level interaction: `SIMPLIFIED_OR_JOIN`/`SIMPLIFIED_AND_JOIN` (§3). So the
   push adds no re-simplification pass of its own; its deep `SIMPLIFY`s —
   `EXPAND`'s case merge, where E's and d's material first meet, and the
   case witness's, the unique split's, and the decompositions' arm edges,
   where a substitution just rewrote the copy — are construction sites, not
   passes.
   Not cosmetic: expansion multiplies the formula, and every step that avoids
   one — the pin matches, the case pins, the UNIQUE classification,
   `FOLD_DECIDED`, O1–O4 — turns on a *syntactic* test over simplified
   values, so those steps fire exactly as often as the normal form is strong.
7. **Guarded disjunctions are assembled guard-first, and `F` propagates eagerly.**
   A guard is a term comparison; a body is a recursion. No body is built until
   its guard has survived, a guard that folds to `T` makes the later branches
   dead, and a conjunct that folds to `F` decides its clause immediately.
8. **Constant-time work before linear before exponential — the cost ladder is
   strict.** Every dispatch tries its options cheapest class first, and a
   cheaper step's success makes the dearer steps unreachable: a memo hit, a
   syntactic test on hash-consed values, a `T`/`F` fold — constant — before
   one traversal (`SIGN_CENSUS`, `TRY_WITNESS`, the incidence pass, the cut
   walk) before anything that multiplies (an expansion, 2b's cross product,
   a solver call). Sound because every step is one-way (§3): it decides or
   falls through, never approximates. It is why the memo wrappers sit
   outside the workers, the dispatcher tries the fast paths before 2d and
   the expansion, `PUSH_OVER_DISJUNCTION` orders its disjuncts
   smallest-first, `PUSH_OVER_CONJUNCTION` narrows scopes — splitting the
   disjoint, settling the final — before its fast paths, elimination tries
   witnesses before methods, the push kills binders — pin, case pin, then
   the unique split — then decomposes licensed — by separation, then by
   cut — before expanding, and `EXPAND` queues cheap members first. A new
   branch joins the ladder at its cost class — never ahead of a cheaper
   one.

---

## 3. Top level

```
ANTI_PRENEX(φ, keep_functional = false):
    if φ carries no quantifier: return φ
 0. φ ← CANONICALISE_BINDER_IDS(φ)     // formula binders AND functional-
                                       //   quantifier subscripts: makes every
                                       //   later substitution capture-safe and
                                       //   the canonical query keys (ASK,
                                       //   DECIDE_FINITE) collide across
                                       //   alpha-variants
 1. φ ← TO_NNF(φ) ; φ ← SIMPLIFY(φ, ref_args = true)
                                       // the ONE ref_args caller: canonical
                                       //   entry state for reference
                                       //   arguments; from here on only
                                       //   substitution dirties one, and it
                                       //   re-simplifies what it touched (§1)
 2. φ ← ELIMINATE_BY_SUBSTITUTION(φ) ; φ ← SIMPLIFY(φ)
 3. φ ← NORMALIZE_OPERATORS(φ)
 4. φ ← PROCESS_ALL_BLOCKS(φ, keep_functional)
 5. φ ← SIMPLIFY(φ) ; φ ← FOLD_DEGENERATE_BINDERS(φ)
    return CANONICALISE_BINDER_IDS(φ)
```

Phase order is fixed as given: phase 2 runs **before** phase 3.

```
ELIMINATE_BY_SUBSTITUTION(φ):                        // one global pass
    for every node n of φ, pre-order:
        if n = ∃x.ψ or n = ∀x.ψ:
            ψ* ← TRY_WITNESS_DEEP(n's binder kind, x, ψ)
            if ψ* ≠ ⊥: n ← ψ*
    return φ

TRY_WITNESS(x, ψ) → formula | ⊥:
    // ∃x.(x = t ∧ ψ′(x)) ≡ ψ′(t), any type. Returns the witnessed BODY, never a
    // binder — only the caller knows what failure means (x stays in the block
    // in ELIMINATE_BLOCK step 1, its direct caller; phase 2 reaches the same
    // match per spine through TRY_WITNESS_DEEP). The call sites straddle
    // NORMALIZE_OPERATORS, so the match is spelling-agnostic:
    // x = t | t = x | x + t = 0 | …, and g ≠ 0 may still occur at phase 2.
    if some top-level conjunct of ψ is equivalent to x = t, with x ∉ FV(t):
        return ψ′ with that conjunct dropped and x replaced by t
    return ⊥

TRY_WITNESS_DEEP(Q, x, Φ) → formula | ⊥:             // phase 2 only
    // A SPINE is a flattened conjunction (Q = ∃) or disjunction (Q = ∀); a
    // PIN is a spine member ≡ x = t (∃) or ≡ x ≠ t (∀) — TRY_WITNESS's match,
    // negated for ∀. For a spine S reached from Φ's root through ∧, ∨, and
    // binders only:
    //     Qx.Φ  ≡  Φ[S ← (S minus the pin)[x ← t]]
    // under (a) x ∉ FV(t); (b) CONFINEMENT — every free x of Φ lies inside S;
    // (c) FV(t) meets no variable bound on the path, EXCEPT Q-kind variables
    // bound before the first kind flip, whose binders commute above Qx. The ∀
    // form is the ∃ form under ¬∃x¬, so one argument covers both. The rewrite
    // is LOCAL to S — t never crosses a binder, phase-0 ids keep it
    // capture-safe; only the DELETION of Qx acts at a distance. Rewritten ⇒
    // original is the witness x := t, evaluable above Qx exactly by (c) —
    // counterexamples (∃ form): ∃x∀y.(x = y ∧ …) and ∃x∀y∃z.(x = z ∧ z = y),
    // both F while their rewrites need not be. Original ⇒ rewritten is
    // pointwise: post-TO_NNF, S sits POSITIVE and the pin licenses the
    // substitution inside it. Deep search lives here alone: the push recovers
    // spine-level pins (the witness step, ELIMINATE_BLOCK's pre-steps), but a
    // re-wrapped intermediate block hides a deep pin from phase 4 for good;
    // at ELIMINATE_BLOCK a clause holds no disjunction, and §4 licenses no
    // conjunct deletion inside a unit.
    D ← ∅ ; flipped ← false ; n ← Φ    // D: path-bound vars barred from FV(t)
    loop:
        if n is a spine:
            if some member is a pin with x ∉ FV(t) and FV(t) ∩ D = ∅:
                // (b) holds free of charge: every node the descent visits
                //   contains ALL free occurrences of x
                return Φ with, at n: the pin dropped, x ← t in the members
            if some member is a CASE PIN for x (∨-of-branches for ∃, dualized
                    for ∀; every branch pins x, x ∉ FV(tᵢ); ≤ K″ branches —
                    the case_max constant, read bare: phase 2 predates any
                    ctx) and FV(that member) ∩ D = ∅:
                // the WHOLE member must avoid D, not only the tᵢ: past a kind
                //   flip the branch CHOICE may not depend on the inner
                //   variable — ∃x∀y.((y = 0 ∧ x = 0) ∨ (y ≠ 0 ∧ x = 1)) is F,
                //   its rewrite ∀y.(y = 0 ∨ y ≠ 0) is T, tᵢ constant. Before
                //   the flip the binders commute above Qx, and the member may
                //   use them, like t in the plain pin
                return Φ with, at n: the spine replaced by
                    ⋁ᵢ ((dᵢ minus its pin) ∧ (spine minus the member))[x ← tᵢ]
                    // connectives and pin sense in the ∀ form under ¬∃x¬, as
                    //   above; the rewrite is local to S, like the plain one
            if x free in ≥ 2 members: return ⊥
            n ← the one member holding x
        else if n is the other connective (∨ for ∃, ∧ for ∀):
            if x free in ≥ 2 children: return ⊥     // x-free siblings ride
            n ← the one child holding x             //   along outside Qx
        else if n = Q′v.χ:
            if Q′ ≠ Q: flipped ← true
            if flipped: D ← D ∪ {v}    // Q-kind vars before the flip commute
            n ← χ                      //   above Qx and stay usable in t
        else: return ⊥                 // atom without a pin, reference,
                                       //   temporal operator: not descended
    // Cost: the descent walks only toward x's occurrences — linear in that
    // cone, one attempt per binder node, keeping the pass one-pass.
```

A CASE PIN for `x` (both phases use the notion): a disjunction `D = ⋁ᵢ dᵢ`
every branch of which has a conjunct `≡ x = tᵢ` (`TRY_WITNESS`'s
spelling-agnostic match), `x ∉ FV(tᵢ)`, with at most `K″` branches. The match
stays at `D`'s TOP branches deliberately: expanding nested ∧/∨ structure into
deeper cases lets guard disjunctions that merely CONTAIN solving atoms
qualify, and their distribution duplicates without telescoping. A member that
is a unit stays opaque (§4): it is not a disjunction, so it is never a case
pin. Phase 2 matches it dualized inside `TRY_WITNESS_DEEP` above; phase 4
consumes it in the case-witness step (§6's ladder):

```
TRY_CASE_WITNESS(x, ψ, ctx) → (branches, R) | ⊥:      // ψ a conjunction; ∃ form —
    // Guarded assignments —                          //   phase 4 pushes ∃ only
    //   (c ∧ x = t₁) ∨ (¬c ∧ x = t₂) — are the shape a program rule takes
    //   after TO_NNF; the pins sit under the disjunction where neither
    //   TRY_WITNESS nor a clause can see them. The rewrite distributes and
    //   witnesses:
    //       ∃x.(D ∧ R) = ⋁ᵢ ∃x.(dᵢ ∧ R) = ⋁ᵢ ((dᵢ − pinᵢ) ∧ R)[x ← tᵢ]
    //   — any BA, no type consulted. Distribution is a size trade the push
    //   otherwise pays only inside EXPAND, member by member; it becomes a
    //   guaranteed win exactly here, where every branch deletes the binder:
    //   k substituted copies of R replace an expansion subtree, each case
    //   DROPS the other members, and no atom is minted. All-or-nothing: a
    //   branch without a pin would keep ∃x alive inside its copy, and the
    //   win is gone — a partial-pin or nested-pin conjunct is the unique
    //   split's case (§6), which reaches any bare occurrence and peels a
    //   case pin wider than case_max two arms at a time. tᵢ may mention
    //   other block variables — same-kind binders commute above x. R-side
    //   units take x ← tᵢ by §4's one licensed inside-a-unit rewrite
    //   (substitution of a free variable).
    //   This function only MATCHES: it returns the case pin's branches with
    //   their pins and witnesses, plus the rest R — no copy is built here.
    //   The ladder (§6) emits the rewrite BRANCH BY BRANCH, so a deciding
    //   branch is the only copy ever paid for; phase 2's dualized match
    //   materialises inline in TRY_WITNESS_DEEP, which rewrites in place.
    for each conjunct D = ⋁ᵢ dᵢ of ψ that is a CASE PIN for x (above),
            smallest |D| first:
        return ( [ (dᵢ, its pin, tᵢ) : i ], ψ without D )
    return ⊥
    // Cost: one linear scan per x, only on the expansion-bound path — the
    //   fast paths have already failed, and what follows dwarfs the scan.
```

```
NORMALIZE_OPERATORS(φ):
    // Formula-level negation, every type:
    f ≠ 0     ↦  ¬(f = 0)

    // Fused negated/mirrored comparisons: single atoms with their own operator,
    // so TO_NNF does not reach them. Applied ONLY where the operator denotes
    // ARITHMETIC comparison (today bv[n]): the four negated rewrites are
    // total-order laws and fail for a lattice order — over the powerset of
    // {1,2}, a = {1}, b = {2}: ¬(a ≤ b) holds, b < a does not. The mirror
    // rewrites are plain swaps, valid in any order, but guarded with the rest.
    a ≰ b     ↦  b < a                              // bf_nlteq
    a ≮ b     ↦  b ≤ a                              // bf_nlt
    a ≥ b     ↦  b ≤ a                              // bf_gteq
    a > b     ↦  b < a                              // bf_gt
    a ≱ b     ↦  a < b                              // bf_ngteq
    a ≯ b     ↦  a ≤ b                              // bf_ngt
    // Postcondition for arithmetic-typed content: only un-negated ≤ and < remain.
```

```
TO_NNF(φ):
    // ¬ down to the atoms, at most one per atom (inv. 4). Phase 1 rebuilds
    // the raw input bottom-up in one memoized pass; from phase 3 on every
    // node is in NNF already and the only work is NEG, cached on the node
    // as neg(φ) (§1).
    ¬ψ ↦ NEG(ψ)      atom ↦ atom      ∧ / ∨ / Qx ↦ rebuilt over the children

NEG(ψ) → the NNF of ¬ψ:                      // neg(ψ): computed once per node
    T ↦ F ; F ↦ T ; atom ↦ ¬atom ; ¬ψ′ ↦ TO_NNF(ψ′)
    ∃x.ψ′ ↦ ∀x.NEG(ψ′) ; ∀x.ψ′ ↦ ∃x.NEG(ψ′)  // a unit's quantifiers flip in —
                                             //   and back out on the second
                                             //   negation (§5)
    ⋀ⱼ mⱼ ↦ SIMPLIFIED_OR_JOIN(NEG(mⱼ) : j)                     // De Morgan
    ⋁ⱼ mⱼ ↦ FACTORED NEGATION, below

    // FACTORED NEGATION. A push result's members share material by
    // construction — EXPAND's cases, the decomposition arms, the case
    // witness's branches differ in a guard or one member and share the
    // rest. Plain De Morgan on ⋁ⱼ(C ∧ restⱼ) gives ⋀ⱼ(¬C ∨ ¬restⱼ): one
    // disjunction per member, each carrying every ¬c — a shape the
    // ENCLOSING block can only expand, member-fold. Pulling the shared
    // conjuncts out first,
    //     ¬⋁ⱼ(C ∧ restⱼ)  =  ⋁_{c∈C} ¬c  ∨  ¬⋁ⱼ restⱼ              — any BA
    // leaves a flat disjunction — one literal per shared conjunct, one
    // negated residue — which the enclosing ∃ distributes over for free
    // (2d's race); the hard shape survives only inside the residue. Always,
    // on the full intersection; the residue is negated by this same
    // recursion (§8). A member equal to C leaves the residue T, and ¬T = F
    // drops out of the join. Only this polarity factors: the dual,
    // ¬⋀ⱼ(D ∨ restⱼ) = ⋀¬d ∧ ¬⋀ restⱼ, would turn De Morgan's disjunction
    // — the shape ∃ distributes over — into a conjunction, the hard shape.
    C ← ⋂ⱼ conjuncts(mⱼ)        // a non-conjunction: its own one-element set
    if C = ∅: return SIMPLIFIED_AND_JOIN(NEG(mⱼ) : j)             // De Morgan
    rests ← SIMPLIFIED_OR_JOIN(SIMPLIFIED_AND_JOIN(conjuncts(mⱼ) ∖ C) : j)
    return SIMPLIFIED_OR_JOIN(NEG(c) : c ∈ C, NEG(rests))
```

`TO_NNF`, `SIMPLIFY` (constant folding, absorption, per-path contradiction, unit
elimination, and equality propagation — see below; one IMPLICIT parameter, the
block `X` in scope, and one flag, `ref_args = false`: when set, the traversal
also descends into reference arguments, recursively, running each through
`SIMPLIFY_TERM` — formula arguments through `SIMPLIFY`. Only phase 1 sets it:
elsewhere arguments are already simplified (§1), and descending would
re-traverse them for nothing), `FOLD_DEGENERATE_BINDERS` (drop a binder over a constant
scope or an absent variable), `NORM_EQUATION` (`l = r ↦ l + r = 0`, descending
through one `¬`), `TERM_OF` (for an atom `l = r`, the term `l + r`) and
`REWRAP(φ, X)` (re-attach `∃X` around `φ`, in `X`'s order — every graceful
exit of invariant 3) are primitives. So are the two aggressive normalisers of
invariant 6:

- `SIMPLIFY_TERM(t)` — constant folding, absorption and complement laws, per-path
  contradiction, and reduction to the canonical form of the BDD backing `t`.
  Per-path contradiction is `prop:xfx` — `x·f(x) = x·f(1)`, `x′·f(x) = x′·f(0)` —
  so a sub-term under a literal is reduced by that literal's assignment. It is
  what stops nested Boole normal forms from compounding as substitutions stack
  terms inside terms.
- `SIMPLIFY_ATOM(a)` — `SIMPLIFY_TERM` on both sides, then fold a constant-only
  atom to `T`/`F`.

**The result joins** assemble already-simplified formulas into a disjunction or
conjunction:

```
SIMPLIFIED_OR_JOIN(r₁, …, rₙ):     // n-ary or incremental; an empty join is F
    // A SET of top-level disjuncts, taken LEFT TO RIGHT: a deciding member
    // stops later operands from being evaluated at all.
    inserting r:
        r = T                   →  the join is T
        r = F                   →  skip
        r = ⋁ rⱼ                →  insert each rⱼ       // flatten one level
        r already a member      →  skip                 // O(1), hash-consed (asm. 1)
        r a literal whose complement is a member  →  the join is T   // unit elim
        anything else           →  add r
    result: one top-level absorption pass (d ∨ (d ∧ e) = d), then ⋁ members
    EMITTED IN THE CONTENT ORDER (§1): insertion order drives evaluation and
    the short-circuits, but the assembled node is a function of the member
    SET — with flattening for associativity, assembly is AC-canonical, so
    converging expansion states build EQUAL memo keys (§6, Expansion).

SIMPLIFIED_AND_JOIN — the dual: empty join T, T skipped, F decides, top-level ∧
    flattened, duplicates skipped, literal vs complement member → F, absorption
    d ∧ (d ∨ e) = d, members emitted in the content order.
```

Shallow by design: every operand is already simplified (invariant 6), so only
the members' top-level interaction is new — exactly what the insert rules and
the absorption pass cover. A member is never descended into, and
`SIMPLIFIED_AND_JOIN` does **no equality propagation** between members:
conjuncts that must interact at term level — every `SIMPLIFY(atm ∧ …)`
construction of §6 — still go through `SIMPLIFY`.

**Equality propagation**, `SIMPLIFY`'s last component. A conjunct pinning a
variable — `y + t = 0`, `y ∉ FV(t)` — licenses substituting `y := t` in its
sibling conjuncts: the only step that carries an assumption from one conjunct
into another's *terms* (unit elimination is propositional; per-path
contradiction works inside a single term). It is what lets the syntactic tests
fire in cases like `f = xy ∪ x′a`, `x ∈ X`: `f₀ = a` and `f₁ = y` compare
unequal until `y := a` turns `f` into `a`.

- **Orientation.** If either side pins a block variable, `TRY_WITNESS`
  eliminates that variable instead. A free `y` is propagated only when
  `FV(t) ∩ X = ∅` — pushing a block variable into a leaf breaks the
  child-is-cofactor identity (§1). "The block in scope" is `SIMPLIFY`'s implicit
  parameter: calls inside `PUSH_BLOCK` or an elimination method bind it to the
  component's `X`; phases 1–2 and 5 bind it to `∅` (nothing is BDD-backed
  there), making the guard vacuous.
- **The pinning conjunct stays.** `y` is free, so `y + t = 0` still constrains
  it; only the siblings change — which is also why a binder-unit sibling may be
  skipped (§4, what may touch a unit). Harmless for the squeeze: an X-free
  positive contributes the same term to both cofactors, which can only make
  `f₀ = f₁` more likely.
- **Deep, one pass.** Substitution into a sibling descends through its whole
  ∧/∨ structure — a disjunctive sibling's members included — stopping only at
  unit boundaries (the skip above). But propagation runs ONCE, not to a
  fixpoint: a conjunct that only BECOMES a pin under this pass's substitutions
  does not propagate in turn — it fires at the next construction site that
  `SIMPLIFY`s it. §8 leans on both halves: depth is why a case inherits `D`'s
  members already specialised at ψ's construction, and the unpropagated
  CHAINED pin is the one growth channel the `EXPAND` measure cannot rule out.

A heuristic, not a monotone gain: a `t` larger than `y` enlarges terms, and
larger terms compare equal less often.

The BDD is canonical over its decision variables only **up to leaf equality**:
semantically equal but structurally different leaves do not merge. That is the
single source of incompleteness behind every syntactic test in the document —
the pin matches, `FOLD_DECIDED`, O1–O4, `COFACTOR_REDUCE`. All are one-way;
failure falls through to a more general path.

`NORM_EQUATION` rewrites an atom and is called in exactly one place:
`SQUEEZE` step 1, whose squeeze needs zero form. Everywhere
else equations stay as written — `TERM_OF` reads a term off an atom without
touching it, and every substitution keyed on an atom uses the atom as it occurs
in the formula.

---

## 4. Phase 4 driver — one post-order pass

```
PROCESS_ALL_BLOCKS(φ, kf):
    memo ← ∅                                  // node → formula, whole pass
    return PROCESS_NODE(φ, kf, memo)

PROCESS_NODE(n, kf, memo):                    // 2k: every quantifier, exactly once
    if memo[n] exists: return memo[n]         // asm. 1: a shared subtree is
                                              //   processed once, not once per site
    if n is a quantifier:
        blk ← COLLECT_RUN(n)
        blk.matrix ← PROCESS_NODE(blk.matrix, kf, memo)  // post-order: the matrix
        result ← PROCESS_BLOCK(blk, kf)                  //   is final first
    else if n is a formula node:
        result ← n rebuilt with PROCESS_NODE(·, kf, memo) on each child
    else:
        result ← n                            // a term: never descended into
    memo[n] ← result ; return result
```

```
COLLECT_RUN(h) → { kind, vars, matrix }:
    // Walk down from h while the next node is a quantifier; the first non-binder
    // is the matrix, so an intermediate binder is never a head of its own and
    // the run stays maximal. Only same-kind quantifiers commute: a kind change
    // ends the run and starts a new, inner one. No type test — a run may mix BA
    // types; PUSH_EX_BLOCK's partition splits it into homogeneous components
    // (invariant 2).
```

No fixpoint is needed: a run is always eliminated over a final matrix, and a
binder that survives elimination is final AS A UNIT — its inner structure is
never re-opened, and one left standing at the top of a processed matrix stays
inside that matrix rather than joining the enclosing run. Final is not
untouchable: an outer block pushed down to such a unit hands it whole to §7,
where a method may swallow it into a solver or `DECIDE_FINITE` query and decide
it — so a clause that re-wrapped open is decided the moment enough enclosing
blocks have closed it. Re-running elimination on the unit alone would yield the
same re-wrap; deciding the LARGER query is new information, not re-examination,
and each enclosing block makes exactly one such attempt (§8) — still no
fixpoint.

**What may touch a unit.** Exactly one class of REWRITE reaches inside a
unit's body: substitution of a free variable. (Reading is different: a
query-based method swallows a unit wholesale, and its translation — the
solver's, `BIT_BLAST`'s — consumes the body without rewriting it.)
`TRY_WITNESS` — phase 2's deep pass, the elimination pre-steps, and the push's
witness steps — MUST descend: each use deletes the pinning `x = t` that
licensed the substitution, so an untouched inner occurrence would
desynchronise from the outer rewrite — and binder-id canonicalisation (§3,
phase 0) makes the descent capture-safe. Equality
propagation MAY skip a unit and stays sound: its pinning conjunct remains in
place (§3). Everything else treats a unit as an opaque leaf — the census does
not descend, `OCC` collects nothing from inside one, a unit is never a
disjunctive conjunct (`EXPAND` reads no members out of one), and settledness
(§6) is judged with units opaque. The one whole-unit rewrite is
§5's dualisation, which flips a unit's quantifiers in and back out.

---

## 5. One block

```
PROCESS_BLOCK(blk, kf):
    if blk.kind = ∀:                                  // paper step (3), by dualisation
        body ← NORMALIZE_OPERATORS(TO_NNF(¬blk.matrix))
        r    ← PUSH_EX_BLOCK(body, blk.vars, kf)
        return NORMALIZE_OPERATORS(TO_NNF(¬r))
    return PUSH_EX_BLOCK(blk.matrix, blk.vars, kf)
```

The two negations flip already-pushed inner quantifiers in and back out; the ∃
machinery treats an inner binder as opaque and transports it — free-variable
substitution is the one operation that reaches inside a unit's body (§4, what
may touch a unit). The outbound negation is where `TO_NNF`'s factoring (§3)
pays: `r` is an OR-join of arms and cases that share material by
construction, and factored negation hands the enclosing block a flat
disjunction instead of a conjunction of one disjunction per member.

```
PUSH_EX_BLOCK(body, X, kf):
    // Partition at ENTRY: merge two block variables when some atom of `body`
    // mentions both. Components then share no atom, push sequentially without
    // interference, may reuse ranks 1..|P|, and are TYPE-HOMOGENEOUS (inv. 2),
    // which gives ELIMINATE_BLOCK one type to dispatch on. Connectivity only
    // decays during the push — PUSH_OVER_CONJUNCTION narrows scopes on the way.
    for P in CONNECTED_COMPONENTS(X, body):       // each keeps X's order;
                                                  //   0-based: P[0] outermost
        ctx.type  ← the BA type of P
        ctx.order ← { P[i] ↦ |P|-i }              // inner → LOWER,  ranks 1..|P|
        ctx.prio  ← { P[i] ↦ i+1 }                // inner → HIGHER, ranks 1..|P|
        ctx.keep_functional ← kf
        ctx.subsume_max ← K = 32 ; ctx.qbf_node_max ← K′ = 2²⁰
        ctx.case_max ← K″ = 16 ; ctx.expand_max ← K‴ = 2¹⁴
        ctx.accept_growth ← γ = 16 ; ctx.accept_floor ← 2²⁰
                                                  // constants (§1, ctx table)
        ctx.expand_count ← 0                      // the one component-scoped
                                                  //   counter (§1, ctx table)
        ctx.pool ← ∅                              // per component, like prio (§1)
        ctx.push_memo, ctx.elim_memo, ctx.cof_memo, ctx.occ_memo,
            ctx.atoms_memo, ctx.quant_memo, ctx.solver_memo, ctx.qbf_memo
            ← the global tables                   // cross-run, never reset (§1)
        body ← PREPARE_TERMS(body, P, ctx.order)  // BDD-back both sides of every
                                                  //   atom touching P (§1). A
                                                  //   FORMULA-level binder unit
                                                  //   is transported opaque (§7
                                                  //   translates it at query
                                                  //   time); a TERM-level
                                                  //   functional quantifier
                                                  //   slides onto its body's
                                                  //   leaves (§1 transparency)
        prev ← body
        body ← PUSH_BLOCK(body, P, ctx)
        if |body| > max(ctx.accept_growth · |prev|, ctx.accept_floor):
            // SIZE ACCEPTANCE (below; inv. 3). The re-wrap takes only the
            //   P-touching conjuncts — ∃P(dep ∧ indep) = ∃P.dep ∧ indep,
            //   PUSH_BLOCK's own strip — so a reject withholds nothing a
            //   later component could still push
            dep  ← ⋀ { c ∈ conjuncts of prev : FV(c) ∩ P ≠ ∅ }
            body ← SIMPLIFIED_AND_JOIN(REWRAP(dep, P ∩ FV(dep)),
                                       the conjuncts of prev not in dep)
    return body
```

**Size acceptance.** One component's push can return a result whose tree
count sits orders beyond its input while the DAG stays small — cheap to
BUILD (sharing is heavy exactly then), ruinous to CARRY: an accepted result
is raw material for every later rewrite of this and every enclosing block,
and rewrites specialise shared subtrees apart — a substitution mints a fresh
copy per context — so `|·|` bounds what later passes can be made to touch,
where DAG size does not. Prediction fails in both directions: a mid-flight
tree count far past any threshold can still collapse before the component
closes, and no pre-push feature of the matrix separates the two. So the test
is an ACCEPTANCE, run once on the completed push — both sizes are §1-cached,
the comparison O(1) even against an astronomical result. Rejection discards
the result for the UNMODIFIED input re-wrapped over the component —
invariant 3's currency, the same shape as `EXPAND`'s graceful exhaustion,
sound by construction because nothing of the discarded push survives. The
P-free conjuncts stay outside the wrap (`PUSH_BLOCK`'s own strip), so a
later component loses only material genuinely entangled with the unit; the
unit itself reaches enclosing blocks as an ordinary opaque leaf (§4),
decidable the moment a query-based method swallows it (§7). Deliberately NOT a taint event (§1, cache scope): the
verdict is a pure function of the completed pair, no cache entry embeds it,
and the entries the discarded push wrote remain correct answers to their own
keys — re-derivable capital, exactly what a raised γ reclaims.

---

## 6. The push — paper step 2

```
PUSH_BLOCK(φ, X, ctx, walk = ⊥):                  // memo wrapper. walk: a cut
                                                  //   arm's remaining walk (§6)
    // KEY CANONICALISATION, two halves of one idea — nothing enters the key
    // that ∃X does not scope:
    X ← X ∩ FV(φ), keeping X's order   // tight downstream quantification. FV
                                       //   includes leaf occurrences (§1)
    if X = ∅: return φ
    if φ = ⋀cᵢ and some cᵢ has FV(cᵢ) ∩ X = ∅:
        // ∃X(indep ∧ dep) = indep ∧ ∃X.dep. What a formula says about FREE
        //   variables never enters the key — expansion cases that differ only
        //   in emitted guard facts converge to ONE state (§6, Expansion)
        indep ← ⋀ { cᵢ : FV(cᵢ) ∩ X = ∅ }
        return SIMPLIFIED_AND_JOIN(indep, PUSH_BLOCK(⋀ the rest, X, ctx, walk))
    k ← (φ, X, ctx.keep_functional)
                   // global: the key names all the entry depends on (§1, cache
                   //   scope), so it suffices across components and runs. No
                   //   in-progress state is needed — not because a key cannot
                   //   meet itself (§8's measure has one unproven edge, the
                   //   chained pin), but because a self-meet is HARMLESS: the
                   //   entry is unwritten, so it is a plain miss and
                   //   recomputes — the old in-progress branch minus the
                   //   bookkeeping — and an X-preserving cycle must pass
                   //   through a budgeted step (§8), whose shared budget caps
                   //   the depth. walk stays OUT of the key: every walk yields
                   //   an equivalent result, so one entry serves them all
    if ctx.push_memo[k] exists: return ctx.push_memo[k]
    r ← PUSH_BLOCK_UNCACHED(φ, X, ctx, walk)
    if r is untainted: ctx.push_memo[k] ← r    // a budget-hit result is
                                               //   returned, never cached
                                               //   (§1, cache scope)
    return r

PUSH_BLOCK_UNCACHED(φ, X, ctx, walk):             // dispatcher
    if φ is not a conjunction:        // a conjunction tries the fast paths in
                                      //   PUSH_OVER_CONJUNCTION, after scope
                                      //   narrowing
        r ← TRY_FAST_PATHS(φ, X, ctx)                 // 2a / 2b
        if r ≠ ⊥: return r
    if φ = ⋁ dᵢ:  return PUSH_OVER_DISJUNCTION(φ, X, ctx)
    if φ = ⋀ cᵢ:  return PUSH_OVER_CONJUNCTION(φ, X, ctx, walk)
    if φ is an atom, a negated atom, or a nested binder:
        return ELIMINATE_BLOCK(φ, X, ctx)
                             // a one-literal (or one-unit) clause. A nested
                             //   binder reaches §7 so a method may swallow the
                             //   unit into a query — once enough blocks close
                             //   it, it must be DECIDED, not wrapped deeper;
                             //   if nothing fires, the method's re-wrap is
                             //   exactly the REWRAP below
    return REWRAP(φ, X)      // reference, temporal operator, or ¬ over one:
                             //   pushed as far as it goes (inv. 3)
```

### 2a / 2b — the two whole-formula fast paths

Both end in small clauses handed to §7 — one atom, or the top-level positives
plus one; neither eliminates anything itself. Both classify per X-touching
leaf — a subtree no `x ∈ X` touches is a constant under `∃X` and stays
neutral.

**2a** distributes `∃X` to each negated atom in one pass, X-free parts riding
along: `∃X.(¬(f=0) ∧ (¬(g=0) ∨ c))` becomes `∃_X f ≠ 0 ∧ (∃_X g ≠ 0 ∨ c)`.
Positive literals at the TOP level ride too: with `p = 0` a conjunct, every
negated leaf takes it along as one two-literal clause, `∃X.(p=0 ∧ ¬(g=0))`,
under a single `∀_X p = 0` — so a guard literal left by a decomposition or
an exclusive case no longer refutes the path.
**2b** collapses `⋀(⋁ positives)` by distributing on *terms*, reaching a
one-atom clause without an expansion.

```
TRY_FAST_PATHS(φ, X, ctx) → formula | ⊥:
    P ← the top-level conjuncts of φ that are positive equation literals
                              // ∅ unless φ is a conjunction; X-touching by
                              //   PUSH_OVER_CONJUNCTION's wrapper invariant
    c ← SIGN_CENSUS(φ, X)     // descends ∧/∨ only. A maximal X-free subtree is
                              //   NEUTRAL: counted once in c.free, not descended.
                              //   X-touching leaves: pos = equation, neg =
                              //   negated equation, anything else = other
                              //   (not descended)
    if c.other > 0: return ⊥

    if c.pos = |P| and c.neg > 0:                                      // 2a
        // Every positive leaf is one of the top-level literals — a conjunct
        // of EVERY DNF clause. ∃X(A ∧ B) ≡ ∃X A ∧ ∃X B for negated atoms —
        // Cor. 5.1, J₁ = ∅; the capability IS atomlessness (inv. 1). With
        // P, per DNF clause and f the squeeze of P:
        //     ∃X(P ∧ ⋀ᵢNᵢ ∧ C)  =  C ∧ ∀_X f=0 ∧ ⋀ᵢ ∃_X f′gᵢ≠0
        //                       =  C ∧ ⋀ᵢ ∃X(P ∧ Nᵢ)
        // — the clause identity §7 applies (cor:qelim-main), then the tier-1
        // identity per leaf — and the leaf-wise transform refactors back
        // through the ∧/∨ skeleton, c.free unconstrained. The ∀_X f=0 half
        // is emitted ONCE at the top: an X-free clause needs it, every other
        // clause implies it, and §7 discharges it once (quant_memo) however
        // many leaves carry P. WHY: a decomposition's positive arm and
        // EXPAND's exclusive cases leave exactly such a top-level literal,
        // which used to refute 2a and force an expansion. The push routes;
        // §7 reasons (inv. 1).
        if not EX_DISTRIBUTES_OVER_NEGATIVES(ctx.type): return ⊥
        return SIMPLIFIED_AND_JOIN(ELIMINATE_BLOCK(⋀P, X, ctx),
                                   DISTRIBUTE_TO_ATOMS(φ minus P, X, ctx, P))

    if c.free = 0 and c.neg = 0 and c.pos > 0:                         // 2b
        // f₁=0 ∧ f₂=0 ≡ f₁∪f₂=0 (prop:squeeze-pos) + distributing ∧ over ∨, on
        // TERMS, any BA. c.free = 0 is required: the term-level squeeze cannot
        // carry a formula-level X-free part through the cross product.
        S ← SQUEEZE_POSITIVES(φ)          // [t₁..t_k] ≜ t₁=0 ∨ … ∨ t_k=0
        return SIMPLIFIED_OR_JOIN(ELIMINATE_BLOCK(t = 0, X, ctx) : t ∈ S)
    return ⊥

DISTRIBUTE_TO_ATOMS(φ, X, ctx, P):
    // Rebuild φ's ∧/∨ skeleton, replacing each X-touching LITERAL l — atom plus
    // its enclosing ¬, if any — by ELIMINATE_BLOCK(⋀P ∧ l, X, ctx): the
    // top-level positives ride into every leaf's clause (2a, above). The ¬
    // goes INSIDE the elimination: replacing the bare atom under a kept ¬
    // would compute ¬(∀_X f = 0), not ∃_X f ≠ 0. X-free subtrees are left
    // verbatim, with no vacuous binder. Nodes are reassembled with the result
    // joins, so a decided literal folds through the skeleton.
```

### 2d — distribute over disjunction

```
PUSH_OVER_DISJUNCTION(⋁dᵢ, X, ctx):
    // A short-circuit race: the disjuncts are independent pushes and the
    // join's members are order-invariant — order changes only the work spent
    // before some disjunct decides T. Smallest first (inv. 8): |d| predicts
    // a disjunct's push cost, and the race is won by reaching a cheap
    // DECIDER — an elimination-cost ranking (clause < 2a-shaped < 2b-shaped
    // < split) queues frequent non-deciders ahead of near-certain ones and
    // pays a census per disjunct for a worse order.
    acc ← an empty SIMPLIFIED_OR_JOIN
    for d in stable_sort(dᵢ by |d|):             // cheap disjuncts first, so a T
        insert PUSH_BLOCK(d, X, ctx) into acc    //   short-circuits before anything
        if acc decided T: return T               //   is expanded. The insert is
    return acc's result                          //   the paper's "unit elimination
                                                 //   after each disjunct"
```

### Conjunction — narrow scopes, then dispatch

```
PUSH_OVER_CONJUNCTION(ψ = ⋀cᵢ, X, ctx, walk):
    // Wrapper invariant: every conjunct touches X (PUSH_BLOCK strips the
    // rest), and X ⊆ FV(ψ). A DISJUNCTIVE conjunct is one whose top node is
    // ∨ — NNF (inv. 4), flat ∧-spines, and unit opacity (§4) make top-level
    // kind equal to reachable case structure, so classifying costs one kind
    // test per conjunct.

    // SCOPE NARROWING — one incidence pass, two moves, one licence: the push
    // only ever REMOVES connections and disjunct-occurrences, so both
    // conditions are monotone along the walk — narrow HERE, not only at
    // block entry. SPLIT where variable sets are disjoint: ∃X(ψ₁ ∧ ψ₂) =
    // ∃X₁ψ₁ ∧ ∃X₂ψ₂, Xᵢ = X ∩ FV(ψᵢ) — before the fast paths, keeping 2b's
    // cross product per part: 2^(k₁+k₂) becomes 2^k₁ + 2^k₂ (inv. 8). SETTLE
    // where a variable's scope is final — x in no disjunctive conjunct, so
    // no expansion can add x-material: ∃X(A ∧ B) = ∃(X∖Xs)(∃Xs.A ∧ B) with
    // A = the conjuncts touching Xs, Xs ∩ FV(B) = ∅. The split needs
    // DISJOINT variable sets; the settle move covers the overlapping case
    // A(x, y) ∧ B(y) the split can never reach. Settling is
    // branch-independent, inevitable work hoisted above the branching — paid
    // once here, not once per case at the leaves — and it is what drains
    // expansion states so they merge (§6, Expansion). A ψ holding no
    // disjunction is the degenerate case Xs = X, A = ψ: the block is pushed
    // home.
    parts, Xs ← INCIDENCE(the conjuncts, X)
    if |parts| > 1:
        acc ← an empty SIMPLIFIED_AND_JOIN
        for p in parts:
            insert PUSH_BLOCK(⋀p, X ∩ FV(p), ctx) into acc
            if acc decided F: return F        // eager F (inv. 7)
        return acc's result                   // each part settles on re-entry
    if Xs ≠ ∅:
        A ← ⋀ { c ∈ conjuncts of ψ : FV(c) ∩ Xs ≠ ∅ }   // plain conjuncts only,
                                                        //   by Xs ∩ FV(S) = ∅
        r ← ELIMINATE_BLOCK(A, Xs, ctx)     // a SUB-block: quant_memo's key is
        if r = F: return F                  //   the query itself (§1, ctx table)
        if Xs = X: return r                                      // pushed home
        return PUSH_BLOCK(SIMPLIFIED_AND_JOIN(r, the conjuncts of ψ not in A),
                          X ∖ Xs, ctx)

    r ← TRY_FAST_PATHS(ψ, X, ctx)      // 2a / 2b, on the narrowed ψ — the
    if r ≠ ⊥: return r                 //   wrapper stripped X-free conjuncts,
                                       //   so 2b's cross product carries
                                       //   nothing it need not

    // Binder-killing steps, cheapest first (inv. 8); EXPAND, which
    // multiplies, is the floor.
    for x in X:
        ψ* ← TRY_WITNESS(x, ψ)                    // a pin at spine level: one
        if ψ* ≠ ⊥:                                //   scan, no copies. On BDD-
            return PUSH_BLOCK(ψ*, X ∖ {x}, ctx)   //   backed conjuncts the
                                                  //   match reads COF: pin iff
                                                  //   usable ∧ f₁ = f₀′, with
                                                  //   t = f₀
    for x in X:                                    // a case witness beats the
        (branches, R) ← TRY_CASE_WITNESS(x, ψ, ctx)   // split (inv. 8): every
        if ⊥: continue                                //   branch deletes the
                                                      //   binder
        // LAZY EMISSION — this step runs 2d's race over §3's rewrite without
        //   ever materialising the whole distribution: one branch at a time
        //   is substituted, SIMPLIFIED (inv. 6 — the copy was substituted),
        //   and pushed, smallest first (2d's convention), and a branch that
        //   decides T ends the loop with every later copy UNBUILT. k
        //   substituted copies of R are the rewrite's whole price; on
        //   satisfiable-leaning input one copy is all that is ever paid —
        //   the same laziness the split's arm order buys (§6, below).
        acc ← an empty SIMPLIFIED_OR_JOIN
        for (dᵢ, pinᵢ, tᵢ) in branches, smallest |dᵢ| first:
            bᵢ ← SIMPLIFY(((dᵢ minus pinᵢ) ∧ R)[x ← tᵢ])
            insert PUSH_BLOCK(bᵢ, (X ∖ {x}) ∩ FV(bᵢ), ctx) into acc
            if acc decided T: return T
        return acc's result
    r ← TRY_UNIQUE_SPLIT(ψ, X, ctx)    // one Boole decomposition on a UNIQUE
    if r ≠ ⊥: return r                 //   atom of ψ (§6, the unique split);
                                       //   beats expansion (inv. 8)
    r ← TRY_DECOMPOSE(ψ, X, ctx)       // one LICENSED Boole decomposition on
    if r ≠ ⊥: return r                 //   a census atom (§6): separates the
                                       //   worlds EXPAND would multiply
    r ← TRY_CUT_DECOMPOSE(ψ, X, ctx, walk)   // one LICENSED Boole decomposition
    if r ≠ ⊥: return r                       //   on the atom cheapest to CUT
                                             //   (§6): adds little, hands two
                                             //   smaller problems back
    return EXPAND(ψ, X, ctx)

INCIDENCE(conjuncts, X) → (parts, Xs):
    // ONE pass over cached FV sets — a kind test and an intersection per
    // conjunct, no descent (§1): union-find merging block variables that
    // co-occur in a conjunct → parts, the conjuncts grouped by component; a
    // variable flagged on touching a disjunctive conjunct → Xs, the unflagged
    // remainder. Near-linear.
```

### The unique split, the licensed decompositions, then expansion — paper steps 2e–2k, redone

Four mechanisms replace the pivot ladder below the case-witness step. The
UNIQUE SPLIT keeps exactly one pivot class from the paper's 2e–2j — the
equation whose truth substitutes a binder variable away — selected through
the candidate index below and spent before anything multiplies blindly.
Between it and the floor, two licensed decompositions restore the general
step for atoms no step above can take, each under a no-duplication test on
its built cofactors: `TRY_DECOMPOSE` takes the atom that SEPARATES sibling
conjuncts, judged in tree size over the conjuncts it touches;
`TRY_CUT_DECOMPOSE` takes the atom cheapest to CUT, judged in shared size
over the whole body. `EXPAND` is the floor: it distributes `∃X` over ONE
disjunctive conjunct the formula already contains — in EXCLUSIVE cases, each
literal member's complement conjoined into the cases after it — and hands
every case back to `PUSH_BLOCK`, which runs the full ladder on each and
re-enters here for the next disjunct. All feed the same state sharing: a
case's state is an ordinary push key, so SHARING is `push_memo` firing —
two cases that converge (same surviving dependent conjuncts, same pending
disjuncts) are one key, solved once. Everything is arranged for that
convergence: children assemble in the
content order (§3, joins), X-free material leaves the key at birth
(`PUSH_BLOCK`) — which is also what keeps the split's guard residue out of
sub-push keys — the witness steps and the split substitute pins out, and the
settle move drains finished variables. Merging is best-effort — a missed
merge costs time, never soundness. Pending disjuncts are deliberately NEVER
specialised with a case's free facts (the fact's conjunct stays in place, as
in §4's unit skip): branch-local rewriting of shared material is what would
fragment the sharing.

```
COF(f, x, ctx) → (f₀, f₁, p, usable):            // the cof_memo recipe — 2f's
    on a ctx.cof_memo[(f, x)] miss, compute and record:        //   test, kept
        f₀ ← SIMPLIFY_TERM(f[x←0]) ; f₁ ← SIMPLIFY_TERM(f[x←1])
        usable ← (x ∉ FV(f₀) ∪ FV(f₁))           // §1 leaf hazard
        p ← usable ? SIMPLIFY_TERM(f₀·f₁) : ⊥    // f has a zero iff p = 0
    return ctx.cof_memo[(f, x)]

FOLD_DECIDED(C, X, ctx) → conjunct list | F:
    // f = 0 forces f₀f₁ = 0 on EVERY variable, so p a nonzero constant proves
    // the atom F outright — a fold SIMPLIFY_ATOM cannot see (it folds
    // constant-only atoms). One memoized probe per (¬)equation.
    for each (¬)equation conjunct c of C:
        f ← SETTLE_FUNCTIONAL(TERM_OF(c), ctx)
        x̂ ← the highest-prio variable of FV(f) ∩ X ; if none: continue
        (f₀, f₁, p, usable) ← COF(f, x̂, ctx)
        if usable and p is a nonzero constant:
            if c is positive: return F           // the atom is F; the case dies
            drop c from C                        // ¬F = T rides out
    return C

SETTLE_FUNCTIONAL(f, ctx):                       // 2f, kept
    if ctx.keep_functional: return f             // ch. 6 needs ∀ₓ/∃ₓ symbolic
    if f carries a functional quantifier: discharge by BDD quantification —
        through quant_memo (§1: the key is the quantifier term) — ; SIMPLIFY
    return f
```

**The candidate index.** `CHOOSE_SPLIT` walks nothing: candidates come from
two caches keyed on immutable values, so neither is ever invalidated — a
formula an arm rebuilds is a NEW key that pays only for its fresh spine, an
unchanged subtree hits. A UNIQUE atom is `TRY_WITNESS`'s own pin read off
the same `cof_memo` entry — one property, met at a different position: a pin
the witness step sees stands at spine level; one whose disjunct pins on
every branch is the case witness's (§3, the ladder above); and a nested ite
buries its pins below the top branches, past any all-or-nothing match — that
remainder, plus the case pin wider than `case_max`, is the split's: it
reaches any bare occurrence, however deep.

```
OCC(n, ctx) → { equation atom ↦ (bare, in_or, count) }:  // memoized in
    equation atom a          ↦ { a ↦ (true, false, 1) }  //   ctx.occ_memo (§1)
    ¬ over an equation atom  ↦ { a ↦ (false, false, 1) }     // the INNER atom
    ∧/∨ node                 ↦ merge the children's maps LEFT TO RIGHT, a key
                               keeping its first position, so keys sit in the
                               content order — bare/in_or OR-ed, counts summed
                               (tree count: a shared subtree contributes
                               through each parent); a ∨ node then sets in_or
                               on every atom of its map
    anything else            ↦ ∅   // unit, order atom, reference, temporal
                                   //   operator: not descended — and only an
                                   //   equation can classify UNIQUE

ADMIT(atm, ctx):                  // one-time pool record; no-op when present
    // LAZY — paid on the atom's first CHOOSE_SPLIT touch, so a formula the
    // fast paths fully handle classifies nothing. ψ-independent, computed
    // once: X only sheds variables that left FV(ψ), and atm ⊆ ψ keeps
    // FV(atm) ∩ P ⊆ X, so the verdict vector never varies. Classified at
    // EVERY block variable of the atom — an atom pinning two block
    // variables, o₂ = o₁·t, is UNIQUE at o₂ and PLAIN at o₁, and judging
    // only one would lose the candidate.
    ctx.pool[atm] ← ( [ (x, usable ∧ f₁ = f₀′ read off COF(f, x))
                          : x ∈ FV(atm) ∩ P, prio-descending ],
                      the next admission index )

CHOOSE_SPLIT(ψ, X, ctx) → (atom, variable, f₀) | ⊥:
    // BARE IS A FILTER, not the old preference: the F-arm erases every
    // occurrence of atm and conjoins only its negation, so under bare-only
    // selection the atom retires ITSELF — no barred set travels down the
    // path, and push_memo keys stay pure (φ, X). The price is the
    // negated-only pin, whose T-arm would still delete a binder; that shape
    // falls through to EXPAND.
    cands ← keys of OCC(ψ, ctx) with bare and FV(atm) ∩ X ≠ ∅, in map order
    ADMIT each candidate, in cands order
    keep those with a UNIQUE verdict at some x ∈ X; per candidate,
        x(atm) ← the highest-prio such x         // pool order: prio-descending
    if none: return ⊥
    order candidates by, in decreasing priority:
      1. in_or — resolving a disjunction is the point; a bare spine pin was
         already TRY_WITNESS's upstream
      2. ctx.prio[x(atm)] — innermost variable, EXPAND's convention
      3. count — more collapse under [atm ↦ T/F]
      4. admission index — the canonical tie-break of last resort
    return (best, x(best), f₀ of COF at it)
```

```
TRY_UNIQUE_SPLIT(ψ, X, ctx) → formula | ⊥:
    // Reached only past the case-witness step, so no conjunct of ψ is a
    // case pin for any x ∈ X: the pins the candidates name sit below a
    // disjunct's top branches (a nested ite), share a conjunct with
    // pin-free members, or live in a conjunct wider than case_max — the
    // shapes distribution cannot take flat. NEVER emit the distribution on
    // the chosen atom's conjunct here: asserting atm does not drop the
    // other members (they survive as x ← f₀ residue), and only the case
    // pin's all-branch structure licenses dropping them.

    (atm, x, f₀) ← CHOOSE_SPLIT(ψ, X, ctx)
    if ⊥: return ⊥

    // One Boole decomposition — 2g's unique arm, sole survivor of the pivot
    // ladder. Case analysis on atm's truth: under atm, x = f₀ uniquely, so
    // the T-arm substitutes the binder away; the F-arm CONJOINS ¬atm — not
    // substituted: ψ[atm ↦ F] does not assert the atom false — and the one
    // negative occurrence it creates is invisible to the bare filter, so
    // the atom is retired (inv. 5). Occurrences OCC cannot see (inside
    // units) stay sound unreplaced: pointwise, atm has one truth value per
    // world, and the T-arm's substitution resolves a unit-internal copy by
    // itself — f(f₀) = f₀f₁ = f₀f₀′ = 0. Arm order is forced by inv. 7's
    // spirit: the T-arm is pushed FIRST and a T short-circuit never builds
    // the F-arm, so on satisfiable-leaning input one arm's copy is all that
    // is ever paid. Arms share ctx.expand_count/expand_max — one budget,
    // one taint story with EXPAND; a trip re-wraps the unbuilt remainder as
    // ONE pending block (inv. 3) and TAINTS (§1, cache scope).
    if ctx.expand_count ≥ ctx.expand_max:
        taint ; return REWRAP(ψ, X ∩ FV(ψ))
    ctx.expand_count ← ctx.expand_count + 1
    τ ← PUSH_BLOCK(SIMPLIFY(ψ[atm ↦ T][x ← f₀]), X ∖ {x}, ctx)     // inv. 6
    if τ = T: return T                        // the F-arm is never built
    φ₂ ← ¬atm ∧ ψ[atm ↦ F]
    if ctx.expand_count ≥ ctx.expand_max:
        taint ; return SIMPLIFIED_OR_JOIN(τ, REWRAP(φ₂, X ∩ FV(φ₂)))
    ctx.expand_count ← ctx.expand_count + 1
    return SIMPLIFIED_OR_JOIN(τ, PUSH_BLOCK(SIMPLIFY(φ₂), X, ctx))  // inv. 6
```

```
TRY_DECOMPOSE(ψ, X, ctx) → formula | ⊥:
    // Reached only past the unique split: what remains is case structure
    // SPANNING conjuncts — an atom's two signs at member tops of two
    // DIFFERENT disjunctive conjuncts — a shape no step above takes flat.
    // One Boole decomposition on such an atom separates the worlds:
    //     ∃X ψ  =  ∃X (a ∧ ψ[a ↦ T])  ∨  ∃X (¬a ∧ ψ[a ↦ F])      — any BA
    // Exact for ANY atom of ψ (inv. 5); arms by DECOMPOSE_ARMS below. Both
    // cofactors erase every occurrence and each arm keeps one ONE-SIGNED
    // guard literal, so the census below (both signs required) never
    // re-selects the atom, nor does the cut step's candidate rule — retired,
    // not barred (inv. 5).
    if ctx.expand_count ≥ ctx.expand_max: return ⊥    // spent: EXPAND re-wraps
                                                      //   and taints

    // CENSUS, rank order COVER: one pass over the member tops of ψ's
    // disjunctive conjuncts (equation and order atoms), counting signed
    // coverage; a candidate must occur in BOTH signs; rank by total
    // coverage, ties by the weaker side, then content order. The rank
    // only orders the tries — the licence decides. Ranking by cofactor
    // size instead selects atoms that partition the LEAST structure.
    cands ← the top 3 both-signed atoms by coverage ; if ∅: return ⊥

    // THE LICENCE, on BUILT cofactors of the TOUCHED region — the
    // conjuncts carrying the atom; the rest is one shared subtree in
    // both arms, its sub-pushes folding through push_memo:
    //     Σc∋a |c[a ↦ T]| + |c[a ↦ F]|   ≤   Σc∋a |c| + (Σc∋a |c|)/4
    // The decomposition must SEPARATE material, not copy it: a passing
    // atom partitions its region between the arms; a failing one would
    // specialise shared bulk per path — the multiplication the ladder
    // exists to avoid. First licensed candidate wins.
    a ← first of cands passing ; if none: return ⊥
    return DECOMPOSE_ARMS(ψ, a, ψ[a ↦ T], ψ[a ↦ F], X, ctx)
                                  // the touched conjuncts' cofactors are the
                                  //   licence's, already built

DECOMPOSE_ARMS(ψ, a, C_T, C_F, X, ctx, walk = ⊥):
    // The two arms of ∃X ψ = ∃X (a ∧ C_T) ∨ ∃X (¬a ∧ C_F) — both
    // decompositions' emission. An X-free guard hoists through PUSH_BLOCK's
    // strip; an X-containing one rides as a dependent conjunct. Arms share
    // EXPAND's budget — one taint story (§1, cache scope); exhaustion
    // re-wraps (inv. 3). Smaller arm first; ∃'s T short-circuit never
    // builds the second (inv. 7's spirit); an arm that folds to a constant
    // is no case and charges nothing. walk: the cut step's remaining walk,
    // handed down with the push (TRY_CUT_DECOMPOSE, below).
    if ctx.expand_count ≥ ctx.expand_max: taint ; return REWRAP(ψ, X ∩ FV(ψ))
    acc ← an empty SIMPLIFIED_OR_JOIN
    for (g, C) in ⟨(a, C_T), (¬a, C_F)⟩, smaller |C| first:
        b ← SIMPLIFY(g ∧ C)                                        // inv. 6
        if b ∈ {T, F}: insert b into acc
        else if ctx.expand_count ≥ ctx.expand_max:
            taint ; insert REWRAP(b, X ∩ FV(b)) into acc ; break
        else:
            ctx.expand_count ← ctx.expand_count + 1
            insert PUSH_BLOCK(b, X ∩ FV(b), ctx, walk) into acc
        if acc decided T: break
    return acc's result
```

```
TRY_CUT_DECOMPOSE(ψ, X, ctx, walk) → formula | ⊥:
    // Reached only past TRY_DECOMPOSE: no atom separates ψ cheaply. The
    // opposite question — which atom is cheap to CUT: its two cofactors
    // share everything it does not touch, so the same identity
    //     ∃X ψ  =  ∃X (a ∧ ψ[a ↦ T])  ∨  ∃X (¬a ∧ ψ[a ↦ F])      — any BA
    // adds little material and hands two smaller problems back to the
    // ladder, whose recursion builds the decomposition DIAGRAM one level
    // per invocation: arms reaching the same sub-problem meet in push_memo.
    // Exact for any atom (inv. 5): substitution reaches every occurrence
    // outside units (§4), and a unit-internal copy stays sound unreplaced
    // (the split's argument).
    if ctx.expand_count ≥ ctx.expand_max: return ⊥    // spent: EXPAND re-wraps
                                                      //   and taints

    // CANDIDATES: every atom with a reachable occurrence in a conjunct
    // other than its own top-level literal — deep or at a member top,
    // equation or order atom, X-free or not, either sign. A LONE top-level
    // literal is excluded because pinning it reproduces ψ — and that is
    // what retires an atom in its own arm, where the guard is exactly such
    // a literal, for this step and TRY_DECOMPOSE alike (inv. 5, §8). A
    // literal with occurrences still standing qualifies, once — its cut is
    // the propagation SIMPLIFY's sweep did not make: a literal the sweep
    // itself minted (the chained channel, §8), or a case fact EXPAND's
    // assembly leaves unpropagated in the pending disjuncts (above).
    cands ← the candidate atoms of ψ ; if ∅: return ⊥

    // THE WALK: the try order is a Cuthill–McKee walk over the incidence
    // graph of candidates and conjuncts (CUT_WALK, below). Consecutive
    // atoms are neighbours, so successive cuts erode ψ from one end and
    // the conjuncts straddling pinned and unpinned atoms stay one band
    // wide. A cut arm inherits the walk past its atom (DECOMPOSE_ARMS):
    // the arm's body differs from ψ only where that atom stood, so the
    // walk still orders it — atoms gone are dropped, atoms it lacks are
    // appended in content order — and both arms continue the SAME walk,
    // which is what makes their sub-problems coincide. Any other entry
    // walks afresh.
    if walk = ⊥: walk ← CUT_WALK(ψ, cands)
    else: walk ← (walk ∩ cands, in walk order)
                 ++ (cands ∖ walk, in content order)

    // THE LICENCE, in shared size: the two cofactors together may MINT —
    // create nodes the hash-cons store did not hold — at most a quarter of
    // ψ's distinct nodes:
    //     mints(ψ[a ↦ T]) + mints(ψ[a ↦ F])   ≤   ‖ψ‖ / 4
    // Untouched conjuncts are the same nodes in both cofactors and cost
    // nothing; material the pin rewrites differently per arm is minted
    // twice and counts in full. No touched-region carve-out
    // (TRY_DECOMPOSE's): sharing is counted directly, across the arms too.
    // Measured at no extra cost — the store's allocations over the rewrite
    // (§10) — and the rewrite is abandoned the moment the count passes the
    // bound: not a cut, next atom.
    for a in walk:
        C_T, C_F ← ψ[a ↦ T], ψ[a ↦ F], abandoned past ‖ψ‖/4 mints in total
        if abandoned: continue
        return DECOMPOSE_ARMS(ψ, a, C_T, C_F, X, ctx, the walk past a)
    return ⊥

CUT_WALK(ψ, cands) → atom list:            // Cuthill–McKee on the incidence
    // graph: an edge joins a candidate to each conjunct of ψ holding a
    // reachable occurrence of it — atoms_memo (§1), one lookup per
    // conjunct. Degree = incident conjuncts. Linear in the occurrences.
    walk ← [] ; queue ← []                 // pushing marks visited
    while some candidate is unvisited:
        push the unvisited candidate of least degree (content-order ties)
        while queue is nonempty:
            a ← pop front ; append a to walk
            for c in the conjuncts incident to a, in content order:
                push the unvisited candidates of c, least degree first
                    (content-order ties)
    return walk
```

```
EXPAND(ψ, X, ctx):
    // Reached only when every step above failed: ψ connected, mixed census,
    // no spine pin, no case pin, no UNIQUE candidate, no atom licensed to
    // separate or to cut, no settled variable.
    //     ∃X(E ∧ (⋁ⱼ dⱼ) ∧ S′)  =  ⋁ⱼ ∃X(E ∧ exclⱼ ∧ dⱼ ∧ S′)      — any BA
    // with exclⱼ the complements of the LITERAL members before dⱼ.
    // EXCLUSIVE CASES: members are not mutually exclusive, so distributing
    // over them as they stand builds CONSENSUS cases — on (¬g ∨ ¬P) ∧
    // (g ∨ ¬Q) the cases ¬g ∧ ¬Q and ¬P ∧ g, plus the redundant ¬P ∧ ¬Q —
    // and each level's negation hands the next level a wider image to
    // expand. Shannon on a literal member makes the cases disjoint,
    //     ∃X(E ∧ (ℓ ∨ R) ∧ S′)  =  ∃X(E ∧ ℓ ∧ S′)  ∨  ∃X(E ∧ ¬ℓ ∧ R ∧ S′),
    // and the conjoined complement folds the sibling disjunct by unit
    // elimination. Only literal members qualify — a non-literal's
    // complement is a disjunction and would ADD case structure; the
    // non-literal members keep plain distribution among themselves, still
    // exclusive from every literal case, so the identity stays exact. The
    // size sort below already puts the literals first. A complement is a
    // top-level positive the fast path 2a tolerates (§6, 2a).
    E ← FOLD_DECIDED(the plain conjuncts of ψ, X, ctx)
    if E = F: return F
    S ← the disjunctive conjuncts of ψ
    D ← the member of S minimising
          ( max{ ctx.prio[x] : x ∈ FV(D) ∩ X },  member count,  content order )
        // Fixed by content, not by path: the expansion order is the analog of
        //   a BDD's variable order and must be canonical for keys to collide.
        //   Expanding the disjunct whose latest variable is outermost retires
        //   variables in quantifier order — once every disjunct touching x is
        //   expanded, x settles (scope narrowing) and drains.
    acc ← an empty SIMPLIFIED_OR_JOIN ; excl ← an empty SIMPLIFIED_AND_JOIN
    for d in stable_sort(members of D, by |d|):       // 2d's convention: cheap
                                                      //   deciders first — the
                                                      //   literals, then the
                                                      //   rest
        if ctx.expand_count ≥ ctx.expand_max:
            // Graceful exhaustion: every finished case is kept; the remainder
            //   is ONE sound pending block (inv. 3), decided later like any
            //   re-wrap once enclosing blocks close it (§4). It carries the
            //   exclusions so far — under them the remaining members'
            //   disjunction is its own exclusive decomposition. The
            //   exhaustion TAINTS every enclosing computation (§1, cache
            //   scope): results embedding it are returned, never cached
            φp ← SIMPLIFIED_AND_JOIN(⋀E, excl, ⋁ of the remaining members,
                                     ⋀(S ∖ {D}))
            insert REWRAP(φp, X ∩ FV(φp)) into acc
            break
        ctx.expand_count ← ctx.expand_count + 1
        dc ← FOLD_DECIDED(the conjuncts of d, X, ctx)
        if dc = F: continue                  // dead case: no copy built, and
                                             //   no exclusion — it is implied
        c ← SIMPLIFY(⋀E ∧ excl ∧ ⋀dc)   // DEEP, but over E ∪ excl ∪ d alone —
                                  //   the pending disjuncts S∖{D} are NEVER
                                  //   re-simplified: normalization is paid
                                  //   where material meets, not per split
                                  //   over the whole ψ; the exclusions reach
                                  //   them at their own expansion
        if c = F: continue                           // dead too: no exclusion
        insert PUSH_BLOCK(SIMPLIFIED_AND_JOIN(c, ⋀(S ∖ {D})), X, ctx) into acc
                                  // the wrapper narrows X and strips what the
                                  //   case decided about free variables; the
                                  //   ladder re-runs in full — witnessing the
                                  //   pins d exposed, settling what d
                                  //   completed, splitting what d disconnected
        if acc decided T: return T                       // ∃'s short-circuit
        if d is a literal: insert NORMALIZE_OPERATORS(NEG(d)) into excl
                                  // the cases after d exclude its worlds — an
                                  //   equation's ¬, an arithmetic order atom's
                                  //   mirror (§3)
    return acc's result
```

---

## 7. Elimination — the type dispatch

```
ELIMINATE_BLOCK(clause, X, ctx):                      // memo wrapper
    // Reached when a block — or a settled sub-block (§6, scope narrowing) —
    // scopes a single conjunctive clause, every conjunct of which mentions
    // some x ∈ X. Other block variables may occur in the clause: the settle
    // move's Xs excludes them, and the methods read them as free.
    if clause ∈ {T, F}: return clause
    k ← (clause, X, ctx.keep_functional)   // exact key (ctx table); nothing
                      //   below recurses back into the push
    if ctx.elim_memo[k] exists: return ctx.elim_memo[k]
    r ← ELIMINATE_BLOCK_UNCACHED(clause, X, ctx)
    if r is untainted: ctx.elim_memo[k] ← r    // §1, cache scope: a
                                               //   budget-hit result is
                                               //   returned, never cached
    return r

ELIMINATE_BLOCK_UNCACHED(clause, X, ctx):
    // Type-agnostic pre-steps — pure identities, run once here, not per method.
 1. for x in X:                                // a witness beats any method:
        if clause ∈ {T, F}: break              //   cheaper and exact (inv. 8)
        ψ* ← TRY_WITNESS(x, clause)
        if ψ* ≠ ⊥: clause ← ψ* ; X ← X ∖ {x}   // ⊥ leaves x and clause untouched
    if clause ∈ {T, F}: return clause
 2. drop from X every variable not occurring in clause
    if X = ∅: return clause
 3. M ← METHOD(ctx.type)
    if M = ⊥: return REWRAP(clause, X)         // no method for τ: undecided (inv. 3)
    return M(clause, X, ctx)
```

The type table — adding a type is one row plus one method obeying the contract:

| `τ` | `METHOD(τ)` | `EX_DISTRIBUTES_OVER_NEGATIVES(τ)` |
|---|---|---|
| atomless | `ELIMINATE_ATOMLESS_CLAUSE` | **true** — `cor:Multivariate-BFs-over` |
| bitvector | `ELIMINATE_BITVECTOR_CLAUSE` — a router: pure Boolean — linear arithmetic converts in — ↦ `ELIMINATE_FINITE_CLAUSE`, residual arithmetic ↦ solver | **false** — `bv[1]`: `∃x(x≠0 ∧ x′≠0)` is F |
| anything else | `⊥` → re-wrap | **false** — fail closed |

**Method contract.**

- **Given:** a single conjunctive clause; `X` non-empty; every `x ∈ X` occurs in
  the clause; every conjunct mentions some `x ∈ X`; `X` homogeneous of type `τ`.
  Other variables of the enclosing component may occur in the clause and are
  read as FREE — the settle move's sub-blocks (§6) are how they arrive.
- **Return:** a formula equivalent to `∃X.clause`. Discharge what the method
  can; **re-wrap** what it cannot. Never answer `F` for "cannot decide"
  (invariant 3).
- **Freezing is the method's own job** — which conjunct shapes are unusable is
  method-specific: an order atom blocks the atomless squeeze yet is native to
  the solver, and a surviving binder is opaque to the atomless engine yet
  swallowed whole — never opened — by a query-based method whose engine can
  translate its body (§4 finality). Freeze per COMPONENT, never per clause —
  the closure is `FREEZE_OPAQUE_COMPONENTS`, shared, each method passing its
  own `opaque?`:

```
FREEZE_OPAQUE_COMPONENTS(clause, X, opaque?) → (frozen, clause, X):
    // opaque? is the METHOD's predicate — each §7 step 1 names its own.
    // WHY components: ∃ does not distribute over ∧ across shared variables.
    // A transparent conjunct sharing an x with an opaque one cannot keep its
    // own ∃x — re-wrapping around the opaque conjunct alone would leave that
    // occurrence outside the wrap — and its OTHER variables then bind further
    // conjuncts the same way. The closure the bullet names is exactly:
    // freeze every connected component that contains an opaque conjunct.
    parts ← the conjuncts of clause, grouped by INCIDENCE's union-find (§6)
            // every conjunct touches some x ∈ X, so the grouping is total
    O  ← ⋃ { p ∈ parts : some conjunct of p is opaque? }
    if O = ∅: return (T, clause, X)
    Xf ← X ∩ FV(⋀O), keeping X's order
    return (REWRAP(⋀O, Xf),                       // inv. 3
            clause without O's conjuncts, X ∖ Xf)
```

### Shared helpers

Shared by the atomless and finite methods — both engines' mathematics is
any-BA up to this point. `SQUEEZE` normalises the clause and is the only
place an equation is rewritten (invariant 4). `SUBSUME_NEGATIVES` prunes its
negatives and is the finite method's step alone — its comment carries the
why. `NEGATIVE_CONDITION` emits the exact condition for ONE negative;
conjoining one per negative is the atomless method's licence alone (inv. 1).

```
SQUEEZE(clause) → (f, negatives, clause):
 1. clause ← NORM_EQUATION applied to every (¬)equation conjunct
    //   zero form for the squeeze; "positives"/"negatives" mean f = 0 / ¬(f = 0).
    //   A swallowed binder unit or a conversion emission (finite method) is
    //   not an atom — it rides along untouched
 2. f ← ⋃ { positives of clause }                        // squeeze; ⋃{} = 0
    return (f, the negatives of clause, clause)

SUBSUME_NEGATIVES(f, clause, X, ctx) → (negatives, clause, X):  // finite only
    // O4, term-order subsumption: gᵢ ≤ gⱼ makes ¬(gⱼ = 0) redundant. Only the
    //   BA order relates the two atoms — propositional simplification cannot
    //   see it. Tested modulo the positive part as f′gᵢgⱼ′ = 0: under f = 0,
    //   f′ = 1 and the test collapses to gᵢ ≤ gⱼ. Dropping costs no precision
    //   (∃_X f′gᵢ ≤ ∃_X f′gⱼ). Finite-only: the k(k−1)/2 meets are repaid
    //   only where a surviving negative fattens a QUERY — the tier-2 blast, a
    //   re-wrap in converted form. The atomless per-negative saving is one
    //   memoized DISCHARGE, which does not repay them, so that method
    //   squeezes without subsuming, at a small result-size price; if size
    //   ever binds, the scan can return there behind a low threshold. BDD
    //   size is not monotone under ≤ — no cheap prescreen; ctx.subsume_max
    //   gates the quadratic scan.
    if |negatives of clause| ≤ ctx.subsume_max:
        keep ← []
        for each negative ¬(gⱼ = 0) of clause, in order:
            // survivors only: mutually subsuming negatives are equal — testing
            //   against all would drop both
            if no gᵢ in keep has SIMPLIFY_TERM(f′·gᵢ·gⱼ′) = 0: keep += gⱼ
        clause ← clause with every negative not in keep dropped
    drop from X every variable not occurring in clause
    //   subsumption can remove a variable's last occurrence; such a variable
    //   sat only in dropped negatives, never in f
    return (the negatives of clause, clause, X)
```

```
NEGATIVE_CONDITION(g, f, f_indep, X, ctx) → formula:
    // The condition for ONE negative ¬(g = 0), modulo the sibling pos =
    // (∀_X f = 0): exact in ANY BA — the tier-1 lemma (Finite section).
    // The shortcuts rest on:  ∃_X f′ = (∀_X f)′,  and  ∃_X(f′·g) = g · ∃_X f′
    // for g independent of X.
    if g ≡ 1: return T               // O1: ∃_X f′g = (∀_X f)′ = 0′ ≠ 0, by pos
    if f_indep:                      // O2: f indep of X + pos ⇒ f′ = 1
        return DISCHARGE(∃X. g ≠ 0, ctx)                          // ⇒ ∃_X g ≠ 0
    if g* ← COFACTOR_REDUCE(g, X) ; g* ≠ ⊥:
        // O3: ∃_X f′g = g · (∀_X f)′ = g. Emit the REDUCED term: g may still
        //   spell an X-variable, and that variable has no binder here.
        return SIMPLIFY_ATOM(g* ≠ 0)
    return DISCHARGE(∃X. f′·g ≠ 0, ctx)                           // ⇒ ∃_X f′g ≠ 0
```

### Atomless

```
ELIMINATE_ATOMLESS_CLAUSE(clause, X, ctx):
    //  ∃X.( f=0 ∧ ⋀ⱼ gⱼ≠0 )  ≡  ∀_X f=0 ∧ ⋀ⱼ ∃_X f′·gⱼ ≠ 0     (cor:qelim-main)

 1. // opaque? = not a (¬)equation: a reference, an order atom, and EVERY binder
    // unit — this engine answers no queries, so it can swallow none. ALSO
    // opaque: a (¬)equation hiding a block variable in a leaf — quantification
    // is cofactoring (§1 leaf hazard), so DISCHARGE would emit that x free.
    frozen, clause, X ← FREEZE_OPAQUE_COMPONENTS(clause, X, opaque?)
    if X = ∅: return frozen
 2. f, negatives, clause ← SQUEEZE(clause)
    // pos holds even where it folds to T (no positives ⇒ f = 0 vacuously);
    //   O1–O3 reason under it as a sibling assumption.
    pos ← DISCHARGE(∃X. f = 0, ctx)                               // ⇒ ∀_X f = 0
    if pos = F: return F                  // f has no common zero: ∃X.clause is F
    f_indep ← (COFACTOR_REDUCE(f, X) ≠ ⊥)     // loop-invariant, computed once
    // ONE condition PER negative is the OTHER use of atomlessness (inv. 1);
    // each condition alone is any-BA-exact (NEGATIVE_CONDITION).
    neg ← an empty SIMPLIFIED_AND_JOIN
    for g in negatives:
        insert NEGATIVE_CONDITION(g, f, f_indep, X, ctx) into neg
        if neg decided F: return F   // one unsatisfiable negative decides
    return SIMPLIFIED_AND_JOIN(frozen, pos, neg's result)

    // No positives ⇒ f = 0 ⇒ O2 fires for every negative: the result
    // ⋀ⱼ ∃_X gⱼ ≠ 0 is the clause-level instance of 2a.
```

```
COFACTOR_REDUCE(t, X) → term | ⊥:
    // "t does not depend on X", decided syntactically, one variable at a time —
    // linear in |X|: each success removes its variable from the running term.
    for x in X:
        t₀ ← SIMPLIFY_TERM(t[x←0]) ; t₁ ← SIMPLIFY_TERM(t[x←1])
        if t₀ ≠ t₁: return ⊥
        t ← t₀
    if FV(t) ∩ X ≠ ∅: return ⊥     // a leaf-x survives child selection (§1)
    return t
```

```
DISCHARGE(∃X.atom, ctx):
    // ∃X. f = 0  ⇒  ∀_X f = 0            ∃X. h ≠ 0  ⇒  ∃_X h ≠ 0
    // The memo key is the functional-quantifier term ITSELF — ASK's
    // convention: one hash-consed node names term, kind, and quantified set,
    // so the settle move's sub-block queries (§6) and SETTLE_FUNCTIONAL's
    // discharges share the table without collision, and the table is GLOBAL
    // (§1, cache scope): entries are pure functions of the key.
    if ctx.keep_functional: emit the functional-quantifier term as written
    else: read ctx.quant_memo[the ∀_X/∃_X term]; on a miss, one BDD
          quantification over ctx.order, recorded. Emit SIMPLIFY_ATOM of the
          result. For |X| = 1: two substitutions and a meet/join (lem:xelim),
          no traversal.
```

### Bitvector — the router

Two engines serve `bv[n]`, split by translatability. A block variable under an
arithmetic operator admits no Shannon expansion — over `bv[2]`, `f(x) = x + 1`
has `x·f(11) ∪ x′·f(00) = x′·01 ≠ f(x)` — so an arithmetic subterm containing a
block variable is the §1 leaf hazard: no BDD-engine move applies, only the
solver sees inside it. Order atoms are arithmetic — every `≤`/`<` denotes
arithmetic comparison today (`NORMALIZE_OPERATORS`, §3). The pure remainder —
(¬)equations over `∪ · ′ +` and constants — is exactly what
`ELIMINATE_FINITE_CLAUSE` decides without the solver.

Linear arithmetic moves sides. An arithmetic atom whose content is linear —
sums, constant multiples, comparisons — is expressible in the pure fragment
through the type's atoms as constants: bit i of `u` is the pure atom
`aᵢ·u ≠ 0`, and carry/borrow predicates chain them. Two BLACK-BOX methods
carry the move (internals deferred, §9): `IS_LINEAR_ARITHMETIC(c)`, a
syntactic guard, and `LINEAR_ARITHMETIC_TO_BA(c)`, defined on guard-positive
conjuncts, emitting an equivalent ¬/∧/∨ combination of pure (¬)equations over
the SAME free variables. An emission is the one non-literal, non-unit conjunct
shape a clause can hold — the push never builds one — so downstream recognises
it by shape alone: PURE for the split, riding through the squeeze untouched,
forcing tier 2 — carries
couple bits, so an emission IS the k ≥ 2 competition. The price: an undecided
clause re-wraps in converted form — still pure, decidable by sweep once
enclosing blocks close it (§4), but the word-level original is gone (§9).

Functional quantifiers cut the other way: over same-type pure content they are
BA polynomials (lem:xelim; §1 transparency), hence PURE — the finite side
reads them natively — while the solver has no term-level binder, so an IMPURE
conjunct carrying one is unreadable for both engines and freezes (step 1). For
`|Y| = 1` the two-cofactor expansion `∀_y f = f₀·f₁` is a cheap refinement
that returns such a conjunct to the solver.

```
ELIMINATE_BITVECTOR_CLAUSE(clause, X, ctx):
 1. // opaque? = unreadable for BOTH engines: a conjunct with a reference in its
    // terms, a binder unit whose body is not bv-translatable (a reference or
    // foreign type inside), or an IMPURE conjunct carrying a functional
    // quantifier (no term-level binder in the solver's language — see above).
    // A binder unit with a translatable body is swallowed whole into a query
    // below — how a clause that re-wrapped open gets decided once enclosing
    // blocks close it (§4).
    frozen, clause, X ← FREEZE_OPAQUE_COMPONENTS(clause, X, opaque?)
    if X = ∅: return frozen
 2. // Linear-arithmetic migration (intro above): AFTER the freeze — a variable
    // reaching a frozen conjunct is gone either way — and BEFORE the split, so
    // a converted conjunct and the pure conjuncts sharing its variables land
    // on the finite side together. Constant guard before a linear rewrite
    // (inv. 8); atom conjuncts only — unit bodies are not rewritten (§4; §9).
    for each conjunct c of clause:
        if IS_LINEAR_ARITHMETIC(c): replace c by LINEAR_ARITHMETIC_TO_BA(c)
 3. // The split. PURE = a (¬)equation over BA operations only — functional-
    // quantifier terms count (see above) — a conversion emission (step 2), or
    // a binder unit whose body is recursively pure; everything else — an
    // order atom, residual arithmetic, a binder unit with arithmetic inside —
    // is the solver's. Same closure discipline as freezing, aimed at the
    // solver instead of a re-wrap: {impure conjuncts} × {variables reaching
    // them} to a fixpoint. The fixpoint makes X_A and X_B disjoint, so
    // ∃X(A ∧ B) ≡ ∃X_A.A ∧ ∃X_B.B.
    (A, X_A), (B, X_B) ← SPLIT_ARITHMETIC(clause, X)
 4. b ← T if B = ∅ else ELIMINATE_FINITE_CLAUSE(B, X_B, ctx)
    if b = F: return F                                // one side false decides
 5. a ← T if A = ∅ else SOLVE_ARITHMETIC(A, X_A, ctx)
 6. // ∀Z distributes over ∧, so the per-side closings lose no VALIDITY; joint
    // UNSATISFIABILITY does not distribute — the sides may share free
    // variables satisfiable only under incompatible assignments, and one
    // joint ask restores the pre-split single-query precision. frozen is
    // excluded: unsat of a sub-conjunction already decides. Skipped when b
    // carries a functional quantifier: the solver has no term-level binder,
    // and this is the one site that would hand it one.
    if a is a surviving binder and b ∉ {T, F} and FV(a) ∩ FV(b) ≠ ∅
       and b carries no functional quantifier:
        if ASK(∃Z.(a ∧ b), ctx) = unsat: return F
 7. return SIMPLIFIED_AND_JOIN(frozen, b, a)
```

```
SOLVE_ARITHMETIC(clause, X, ctx):                     // the solver path
 1. q ← ∃X.clause     // binder units enter q as written — the solver accepts
                      //   quantified bv formulas; alternation depth = how many
                      //   enclosing blocks re-wrapped (§4)
 2. if q is closed:
        ASK(q, ctx):  sat ⇒ return T,  unsat ⇒ return F,  unknown ⇒ step 3
    else:               // close the free variables Z both ways
        if ASK(∀Z.q, ctx) = sat:    return T          // q valid
        if ASK(∃Z.q, ctx) = unsat:  return F          // q unsatisfiable
 3. return REWRAP(clause, X)                          // undecided (inv. 3)
```

```
ASK(q, ctx) → sat | unsat | unknown:
    // Key = CANONICALISE_BINDER_IDS(q): alpha-variants collide across
    // components, blocks, runs, and the two closings. `unknown` is cached too —
    // the table is valid per solver configuration, flushed when it changes
    // (§1, cache scope). elim_memo cannot subsume this: different (clause, X)
    // can normalise to the same query after freezing.
    k ← CANONICALISE_BINDER_IDS(q)
    if ctx.solver_memo[k] exists: return ctx.solver_memo[k]
    r ← the solver's verdict on k
    ctx.solver_memo[k] ← r ; return r
```

### Finite (pure Boolean)

The pure side of the bitvector router — and `METHOD(τ)` for any finite BA of
known width whose term language is pure BA (none routed directly yet).
Everything through one negative is the atomless mathematics — exact in EVERY
BA — and finiteness is consulted only at tier 2, exactly where atomlessness
would have been: invariant 1's two sites stay two.

Tier-1 exactness (one negative, ANY BA): `∃X.(f=0 ∧ g≠0) ≡ ∀_X f=0 ∧
∃_X f′g ≠ 0`. One variable, under `f₀f₁ = 0`: the witness `x = f₀ ∪ f₁′g₁` has
`f(x) = 0` and `g(x) ≥ f₁′g₁`, and `x = f₀` has `g(x) ≥ f₀′g₀` — one of the two
is nonzero whenever `∃ₓ(f′g) = f₀′g₀ ∪ f₁′g₁ ≠ 0`. A block by induction, the
intermediate factor collapsing under the absorption `∃_y(f′g) ≤ ∃_y f′ =
(∀_y f)′`. Competition for atoms needs a SECOND negative — the type-table row:
`bv[1]`, `∃x(x≠0 ∧ x′≠0)` is F while both single-negative conditions hold.

```
ELIMINATE_FINITE_CLAUSE(clause, X, ctx):
 1. // opaque? = the atomless step-1 condition plus anything arithmetic — MINUS
    // binder units with recursively pure bodies and MINUS conversion
    // emissions, both swallowed into the tier-2 query (§4). A no-op when
    // called from the router, whose split already guarantees purity.
    frozen, clause, X ← FREEZE_OPAQUE_COMPONENTS(clause, X, opaque?)
    if X = ∅: return frozen
 2. f, negatives, clause ← SQUEEZE(clause)
    negatives, clause, X ← SUBSUME_NEGATIVES(f, clause, X, ctx)
    if X = ∅: return SIMPLIFIED_AND_JOIN(frozen, clause)
 3. pos ← DISCHARGE(∃X. f = 0, ctx)       // exact in ANY BA (thm:boole-const);
    if pos = F: return F                  //   implied by the clause, so F here
                                          //   decides even before tier 2
 4. if clause holds no binder unit, no conversion emission, and |negatives| ≤ 1:
        // Tiers 0–1 — the shared pos/neg emission, exact here by the tier-1
        // lemma. A binder unit or a conversion emission forces tier 2: the
        // clause does not factor around it.
        if negatives = ∅: return SIMPLIFIED_AND_JOIN(frozen, pos)
        f_indep ← (COFACTOR_REDUCE(f, X) ≠ ⊥)
        neg ← NEGATIVE_CONDITION(the single negative g, f, f_indep, X, ctx)
        if neg = F: return F
        return SIMPLIFIED_AND_JOIN(frozen, pos, neg)
 5. // Tier 2 — k ≥ 2 negatives competing for atoms, a swallowed binder unit,
    // or a conversion emission. NP-complete even closed, so the ladder inside
    // the tier is by representation cost (inv. 8): one shared BDD sweep, then
    // the solver (inside DECIDE_FINITE), then surrender — never the 2^k
    // formula expansion.
    q ← ∃X.clause
    if q is closed:
        r ← DECIDE_FINITE(q, ctx)
        if r ≠ unknown: return SIMPLIFIED_AND_JOIN(frozen, r)
        return SIMPLIFIED_AND_JOIN(frozen, REWRAP(clause, X))   // inv. 3
 6. // Open: close the free variables Z both ways — detection only, the solver
    // path's shape. Sound and still pure: every variable in a pure clause's
    // terms has the block's finite type (inv. 2).
    if DECIDE_FINITE(∀Z.q, ctx) = T: return frozen              // q valid
    if DECIDE_FINITE(∃Z.q, ctx) = F: return F                   // q unsat
 7. return SIMPLIFIED_AND_JOIN(frozen, REWRAP(clause, X))
    // Open and undecided: re-wrap and wait for an enclosing block to close —
    // and then decide — the unit (§4). Exact open elimination (Hall's theorem
    // on the negatives' coefficient minterms; ch. 2 §Distinct Representatives,
    // thm:main-ineq) is deliberately not used: 2^k-governed, and its
    // cardinality conditions leave the pure fragment (popcount arithmetic).
```

**The blast.** `bv[n]` is the powerset algebra of its `n` atoms `a₁ … aₙ`, and
`u ↦ (a₁ ≤ u, …, aₙ ≤ u)` is a BA isomorphism `bv[n] ≅ bool^n`: every PURE
operation acts per bit, and no operation couples two bit positions — the same
coordinatewise-ness whose failure under arithmetic (a carry couples bits)
drives the router's split. `BIT_BLAST` rides the isomorphism compositionally —
terms, atoms, connectives, quantifiers — so `q` holds in `bv[n]` iff its blast
holds as a QBF over `bool`: soundness is inherited from the isomorphism, not
argued per case.

```
BIT_BLAST(φ) → QBF over two-valued variables:
    // ⟦t⟧ᵢ ≜ "bit i of t", i = 1..n; per bv variable x, n fresh two-valued
    // variables x⁽ⁱ⁾ ≜ (aᵢ ≤ x). Bits of variables and constants are read
    // against ONE atom enumeration per type — mixing two enumerations within
    // a blast misaligns coordinates.

    // Terms — coordinatewise, which is the whole translation:
    ⟦x⟧ᵢ = x⁽ⁱ⁾                      ⟦c⟧ᵢ = bit i of the constant c
    ⟦s · t⟧ᵢ = ⟦s⟧ᵢ ∧ ⟦t⟧ᵢ           ⟦s ∪ t⟧ᵢ = ⟦s⟧ᵢ ∨ ⟦t⟧ᵢ
    ⟦t′⟧ᵢ    = ¬⟦t⟧ᵢ                 ⟦s + t⟧ᵢ = ⟦s⟧ᵢ ⊕ ⟦t⟧ᵢ
    ⟦∀ₓt⟧ᵢ = ⟦t⟧ᵢ[x⁽ⁱ⁾←F] ∧ ⟦t⟧ᵢ[x⁽ⁱ⁾←T]     // functional-quantifier terms are
    ⟦∃ₓt⟧ᵢ = ⟦t⟧ᵢ[x⁽ⁱ⁾←F] ∨ ⟦t⟧ᵢ[x⁽ⁱ⁾←T]     //   cofactor meets/joins — pure,
                                     //   and slice i sees only x's bit i; a
                                     //   block ∀_X/∃_X iterates per variable
                                     //   of X

    // Atoms — the GENERAL form: a swallowed unit's body is not zero-normalised
    // (SQUEEZE rewrites only the clause it was handed), so l = r
    // may not be assumed to be zero form; f = 0 is the special case ⋀ᵢ ¬⟦f⟧ᵢ.
    l = r   ↦   ⋀_{i=1..n} ( ⟦l⟧ᵢ ↔ ⟦r⟧ᵢ )

    // Connectives ¬/∧/∨ map unchanged, atoms rewritten in place. There is NO
    // ≠ case: ¬(g = 0) is a formula-level ¬ (inv. 4), and De Morgan alone
    // gives ¬⋀ᵢ¬⟦g⟧ᵢ = ⋁ᵢ⟦g⟧ᵢ — the "linearly many equations" of the n-fold
    // expansion appear as n branches of ONE shared BDD, never as n formula
    // copies. Equalities decompose per bit; the negatives' ⋁ᵢ all read the
    // SAME existential bits — that shared coupling IS the k ≥ 2 competition.
    ∃x.ψ   ↦   ∃x⁽¹⁾ … ∃x⁽ⁿ⁾. BIT_BLAST(ψ)     // ∀x dually; nesting kept.
                                     //   Exact with no guard formulas: every
                                     //   bit assignment IS an element of bv[n]

    anything else ↦ FAIL             // an order atom, arithmetic, a reference
                                     //   here is a routing bug — the
                                     //   coordinatewise rules would be
                                     //   SILENTLY WRONG, so never approximate.
                                     //   FAIL aborts; it is NOT `unknown` —
                                     //   mapping it there would mask the bug
                                     //   as a sound re-wrap
```

```
DECIDE_FINITE(q, ctx) → T | F | unknown:
    // q: CLOSED, pure Boolean, every variable of one finite type of width n.
    // BIT_BLAST(q) is a QBF; one BDD sweep decides it: abstract each block's
    // bits (∃ = ∨ of cofactors, ∀ = ∧) innermost-out, as soon as its sub-BDD
    // is built, so intermediates stay small — alternation depth (§4) is just
    // repetition. COMPLETE on this fragment; `unknown` arises only from the
    // node budget. The BDD is propositional, NOT §1's term BDD: the blast is
    // syntactic from q. Indices never mix (BIT_BLAST), so the matrix is a
    // Boolean combination of n single-index slices — slice-major variable
    // order keeps the equalities' ⋀ᵢ linear; the negatives' ⋁ᵢ is
    // the honest exponential (inv. 8) behind ctx.qbf_node_max. Past the
    // budget, ASK: sweep and solver blow up on different instances — a
    // complementary fallback.
    k ← CANONICALISE_BINDER_IDS(q)
    if ctx.qbf_memo[k] exists: return ctx.qbf_memo[k]
    if ctx.solver_memo[k] = sat:   return T   // constant before exponential
    if ctx.solver_memo[k] = unsat: return F   //   (inv. 8): a verdict from an
                                              //   earlier budget fallback
                                              //   decides without re-running
                                              //   the sweep; a cached
                                              //   `unknown` stays a miss —
                                              //   the sweep may still succeed
    r ← the BDD sweep over BIT_BLAST(q), budgeted by ctx.qbf_node_max
    if r ∈ {T, F}: ctx.qbf_memo[k] ← r ; return r    // mathematical truth —
                                                     //   never flushed (§1)
    ASK(q, ctx): sat ⇒ return T, unsat ⇒ return F, unknown ⇒ return unknown
                                                     // cached in solver_memo
                                                     //   under ITS flush rule
```

---

## 8. Termination

| edge | measure |
|---|---|
| `PUSH_BLOCK` key canonicalisation | the stripped remainder is a proper sub-conjunction, and the strip fires at most once per chain — the remainder holds no X-free conjunct |
| `PUSH_OVER_DISJUNCTION`, `PUSH_OVER_CONJUNCTION` scope narrowing | strict subformula or a strictly smaller block: a disjunct or one part of the split is a proper sub-conjunction; the settle move strictly shrinks `X`, and its replacement conjuncts hold no disjunction |
| `TRY_FAST_PATHS` (2a, 2b) | leaves: both go straight to `ELIMINATE_BLOCK` — 2a's as the two-literal clauses `⋀P ∧ l` plus the one clause `⋀P` — never back into `PUSH_BLOCK` |
| the witness and case-witness steps (§6) | `X` strictly shrinks — the pin or case pin deletes its binder before re-entry |
| `TRY_UNIQUE_SPLIT`, binary arms (§6) | T-arm: `|X|` strictly shrinks. F-arm: `|X|` constant while the OCC index's bare-candidate set loses `atm` for good — `[atm ↦ F]` erases every occurrence and mints no term, and the conjoined `¬atm` is not bare (the filter, §6) — modulo the same chained channel as `EXPAND`'s caveat below: a term the arm-edge `SIMPLIFY` mints can be a fresh bare candidate. The SHARED `ctx.expand_max` bounds both arms' count outright |
| `TRY_DECOMPOSE`, binary arms (§6) | `|X|` constant while BOTH arms lose every occurrence of the atom — the substitution erases and mints no term, and the surviving guard literal is one-signed and alone, invisible to the both-signs census and to the cut step's candidate rule — modulo the same chained channel as the split's caveat (an arm-edge `SIMPLIFY` can mint); the SHARED `ctx.expand_max` bounds the arm count outright |
| `TRY_CUT_DECOMPOSE`, binary arms (§6) | `|X|` constant while both arms lose every reachable occurrence of the atom and keep only its guard — a lone top-level literal, which the candidate rule excludes (§6) — so neither decomposition takes the atom again; modulo the same chained channel (an arm-edge `SIMPLIFY` can mint); the SHARED `ctx.expand_max` bounds the arm count outright |
| `EXPAND` | lexicographic (`|X|`, multiset of top-level disjunctive-conjunct sizes), modulo ONE caveat: each case drops `D` and gains only disjunctions lying properly inside ONE member — smaller than `|D|` at selection time — plus exclusion LITERALS, never disjunctions, whose unit elimination only shrinks; the case-edge `SIMPLIFY` builds no disjunction node, but it can GROW an inherited one: a CHAINED pin — formed inside `d` by a construction-time substitution, unpropagated because propagation runs once (§3) — fires here and can push a member past `|D|`. Well-foundedness therefore rests on `ctx.expand_max`, which bounds the case count outright |
| `ELIMINATE_BLOCK` pre-steps | `X` shrinks or the clause is decided |
| `ELIMINATE_BITVECTOR_CLAUSE` (router) | one guarded conversion per conjunct — a rewrite, no recursion — then two variable-disjoint sub-clauses, each handed to its engine exactly once; no re-entry into the push |
| swallowed binder units (`ELIMINATE_FINITE_CLAUSE`, `SOLVE_ARITHMETIC`) | a unit is decided wholesale or re-wrapped, never opened; each enclosing block makes exactly one attempt on it (one post-order pass, §4) — no fixpoint across blocks |
| `DECIDE_FINITE` | one syntactic `BIT_BLAST` pass, then one BDD sweep over a finite bit set, budgeted by `ctx.qbf_node_max` |
| `PROCESS_ALL_BLOCKS` | one post-order pass over a finite tree; each run is eliminated once |
| §5 size acceptance | no recursion edge: the test runs on a COMPLETED component push and only replaces its result by a re-wrap (inv. 3) |
| `TO_NNF`, factored negation (§3) | `NEG(rests)` recurses on a NEW node, not a subformula — but every residue member lost the shared conjuncts, so `\|rests\|` is strictly below the negated disjunction's |

No elimination method re-enters `ANTI_PRENEX`, so the phases of §3 run once per
call. Every recursion edge above strictly decreases the lexicographic measure
(`|X|`, then the candidate/disjunctive-conjunct material) except the one
chained channel — `EXPAND`'s chained pin and the atoms an arm-edge
`SIMPLIFY` mints for the split and the decompositions — which the shared
`ctx.expand_max` bounds. `push_memo` needs no in-progress state either way:
a key met mid-computation is an unwritten entry — a plain miss, recomputed —
and any X-preserving cycle passes through `EXPAND`, a split F-arm, or a
decomposition arm (the strip fires once per chain; 2d and the scope split
strictly shrink the formula; the witness step, the case witness, and the
settle move strictly shrink `X`), so the one shared budget caps its depth
(§6).

---

## 9. Open items

- **`IS_LINEAR_ARITHMETIC` / `LINEAR_ARITHMETIC_TO_BA` are black boxes.** The
  router leans only on their contract (router intro). Deferred: the carry
  encoding (fresh ∃-bound definition variables would put a binder unit into
  the emission); the operator list defining "linear" and the mod-2ⁿ wrap
  convention, which must match the solver's; the bit-significance order,
  which must read against `BIT_BLAST`'s atom enumeration; whether re-wraps
  and `ASK` fallbacks should use the word-level original instead of the
  converted form; and whether conversion may reach inside a translatable
  unit's body (§4).
- **`SIGN_CENSUS` is the one deep per-state walk** on the expansion path
  above the cut step (whose `‖ψ‖` traversal and walk are its own, §6). Its
  result is a pure function of `(node, X ∩ FV)` and could be cached per
  node the way `OCC`'s map is (§6), if profiling ever shows it hot; the
  census aborts on the first `other` leaf — and the moment the counts refute
  both 2a and 2b at once: a positive beyond the top-level literals with
  `neg > 0` or `free > 0` — so this is deliberately not done yet.
- **`ZERO`/absorb pivots stay retired.** The unique split restores only the
  class whose T-arm deletes a binder by substitution. The paper's absorb arm
  (2i/2j — `p = 0` proven, the reproductive solution `r(x) = f₀ ∪ x·f₁′`)
  stays out: classification at every block variable of an atom covers much
  of what it used to reach, and its arm neither deletes a binder nor retires
  an atom, so it earns no step on the ladder here. Reopen only if a family
  walls with many `p = 0` candidates and no UNIQUE ones.
- **The acceptance dial, and a reject-retry variant.**
  `accept_growth`/`accept_floor` trade output boundedness against decision
  fidelity: tight settings catch every detonation but also discard pushes
  whose mid-flight size would have collapsed before the component closed —
  a formula the looser setting still decides. Deferred: retrying a rejected
  component under a sharply smaller `expand_max`, trading the whole-block
  re-wrap for partial progress with bounded growth; and the one family the
  dial does not clear — matrices dominated by deep definitional binder
  units, opaque to the census, `OCC`, and every ladder step — which needs
  a mechanism of its own, not a setting.
- **`TRY_CUT_DECOMPOSE`'s walk is the cheapest one.** Its seed is the
  candidate of fewest conjuncts; the classic pseudo-peripheral seed costs a
  few more passes and may cut better. The licence probes by rewriting;
  `atoms_memo` could carry each occurrence's depth and skip an atom whose
  rebuilt paths alone exceed the bound, without a rewrite. Both wait for
  profiling.
- **One step at a time cannot see the diagram's width.** The cut step
  builds the decomposition diagram one level per invocation and judges
  only that level's cost; how many DISTINCT sub-problems a level leaves
  after merging — the width a levelled construction could bound directly —
  reaches it only through the shared budget, late. A cheap cut can also
  change little: the memo, scope narrowing, and the candidate rule answer
  that in mechanism, unmeasured. If a family walls on cascades the budget
  catches late, an explicit levelled diagram with a width bound is the
  recorded alternative.

---

## 10. Implementation notes — porting hazards

Constraints on any implementation of this document, each independent of the
§5/§6 policy — miss one and walls reappear that no budget or acceptance
setting can explain, because the executor's cost stops tracking the
algorithm's:

- **Every pass over formulas must cache its result per node.** The formula
  is stored with identical subformulas shared (assumption 1), so it has two
  sizes: `|·|`, the count as written out in full, where a shared part counts
  once per place it appears, and `‖·‖`, the count in memory, where it counts
  once. The algorithm's cost tracks the second only if every pass —
  dualisation (`TO_NNF`/`¬` in `PROCESS_BLOCK` — the `neg(φ)` property,
  §1), constructor flattening, the result joins' member expansion, the
  census and incidence passes, phase 2's traversal, substitution — processes
  a subformula once, however many places it appears in. A pass without that
  cache costs the written-out size, which the push drives far beyond the
  in-memory size (§5). With the cut licence (§6) this stops being a matter
  of speed: that licence is stated in `‖·‖`, so it approves as small a
  formula that an uncached pass then pays for at written-out scale, and the
  budget no longer describes the cost. Per-node caching is an assumption the
  licence depends on, not an optimisation.
- **Substitution needs occurrence guards.** `[x ← t]` must return an
  untouched subtree by one cached-FV test; `[atm ↦ T/F]` needs the
  analogous guard — `atoms_memo` (§1), a per-node atom vocabulary with
  exactly the substitution's own reach (unit-opaque, §4). One memo per
  rewrite, shared across every conjunct of the site: the copies overlap.
  The cut licence (§6) reads its measure off this rewrite: the nodes it
  MINTS are the store's allocations during the rewrite, and the rewrite
  stops at the bound — a failed probe costs no more than the bound allows.
- **Unit elimination sweeps once.** Inside `SIMPLIFY`, the literal
  conjuncts' truth assignments bind DISTINCT atoms — a complement pair has
  already decided the junction — so they commute: apply them as one
  simultaneous environment in one sweep over the non-literal siblings, not
  one literal at a time with a restart per change.
- **`|φ|` and `FV(φ)` are set at construction (§1), never recomputed** —
  the acceptance comparison, the sort keys, and every `X ∩ FV` test in this
  document are O(1) only because of it.
