# Anti-prenexing — pseudocode baseline

Spec: Chapter 5 of `taba0.5.tex` (= ch. 5, p. 51 of `taba0.5.pdf`). Paper steps
`2a`–`2k` are cited inline; step `(3)`, the universal branch, is handled by
dualisation in `PROCESS_BLOCK`.

Two phases, sharply separated:

1. **Push** (§4–§6) — an existential block is pushed inward, by identities valid
   in any BA (invariant 1), until it scopes a single conjunctive clause.
2. **Eliminate** (§7) — **type-specific**: the clause is handed to the method
   for the block's Boolean-algebra type.

Two assumptions from the paper (`taba0.5.tex:2881–2887`):

1. The formula is a **DAG sharing common subtrees**, so a term may be used many
   times without blowup. `ABSORB_PIVOT` depends on it; it also makes the memo
   tables hit and the result joins' membership test O(1).
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

functional quantifiers (lem:xelim):   ∀ₓf = f₀·f₁      ∃ₓf = f₀ ∪ f₁
  over a block: ∀_X, ∃_X — one BDD quantification, never expanded to 2^|X| terms

atom shapes reaching the core (post phase 3), each optionally under one ¬:
    f = g   |   f ≤ g   |   f < g
    — never f ≠ g, and no mirrored/negated order operator (≰ ≮ ≥ > ≱ ≯)
    — equations stay as written (invariant 4); order atoms are never rewritten
      into equations — decomposed only by BOOLE_DECOMPOSE_NON_EQUALITY_PIVOT,
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
      - substitution (unique-zero arm, TRY_WITNESS, the clamp) is a BDD compose
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
        inside a quantifier's body. Guarded by `usable`, COFACTOR_REDUCE's FV
        check, and ABSORB_PIVOT's per-conjunct cofactor test (a leaf-x
        conjunct is clamped by substitution instead)
```

`ctx` carries:

| field | meaning |
|---|---|
| `type` | the block's BA type `τ`. Single, by invariant 2 |
| `order` | BDD variable order. inner → **lower** rank |
| `prio` | pivot priority. inner → **higher** rank |
| `used` | pivots barred on the current path. Needed by **both** decomposition forms: the non-equality split retains the pivot, the equality form's β re-introduces it negated |
| `subsume_max` | threshold on a clause's negative count, above which O4's subsumption scan is skipped. `K = 32`, provisional pending benchmarks — free to tune: subsumption costs no precision, so no flush rule attaches (unlike `qbf_node_max`) |
| `qbf_node_max` | node budget for `DECIDE_FINITE`'s BDD sweep — peak live nodes of ONE sweep, checked by allocation high-water mark — past which it falls back to `ASK`. `K′ = 2²⁰`, provisional pending benchmarks |
| `keep_functional` | emit `∀_X`/`∃_X` symbolically instead of discharging them; in the `push_memo`/`elim_memo` keys (cache scope, below) |
| `push_memo` | `(φ, X, keep_functional) → formula`, GLOBAL (cache scope, below). `used` is not in the key; an in-progress entry is a miss (see `PUSH_BLOCK`) |
| `elim_memo` | `(clause, X, keep_functional) → formula`, GLOBAL. `used` plays no role in elimination, so the key is exact |
| `quant_memo` | `(term, ∀ or ∃) → term`, per COMPONENT — the key elides the quantified block (see `DISCHARGE`) |
| `cof_memo` | `(settled term, x) → (f₀, f₁, p, usable)`, GLOBAL. Filled by `CLASSIFY`, read by `BOOLE_DECOMPOSE_EQUALITY_PIVOT`; pure functions of the key — the settled term already reflects `keep_functional` |
| `pool` | `atom → (x(atm), class, admission index)`, per COMPONENT — ψ-independent, but which of an atom's variables are block variables is contextual. Filled lazily by `ADMIT` (§6, 2e) |
| `occ_memo` | `formula node → { atom ↦ (bare, in_or, count) }`, GLOBAL. Filled by `OCC` (§6, 2e); purely structural |
| `solver_memo` | canonical closed query → `sat`/`unsat`/`unknown`. GLOBAL — valid per solver configuration, flushed when it changes (cache scope, below) |
| `qbf_memo` | canonical closed pure-Boolean query → `T`/`F`, written only by `DECIDE_FINITE`'s own sweep. GLOBAL — entries are mathematical truths, never flushed (cache scope, below) |

The two rank conventions are opposite and cannot be merged: `order` is consumed
innermost-first by BDD quantification; `prio` is read by pivot selection, which
prefers the atom carrying the innermost variable.

**Cache scope** follows one rule: a table is GLOBAL — outliving the call,
shared across `ANTI_PRENEX` runs, components, and blocks — exactly when its key
names everything its entries depend on. The variable set is explicit: the
ordered `X` of `push_memo`/`elim_memo` fixes the type, every atom's block
subset (`FV(atm) ∩ P = FV(atm) ∩ X`), and the relative prios; `cof_memo`
carries its `x`; `occ_memo`, `solver_memo`, and `qbf_memo` involve none. The mode:
`keep_functional` joins `push_memo`'s and `elim_memo`'s keys alone — their
entries embed `DISCHARGE`'s output, which the flag switches — while `cof_memo`
needs no flag, its settled-term key being formed AFTER `SETTLE_FUNCTIONAL`
applied the mode, and `occ_memo` is purely structural. `quant_memo` and `pool`
elide their variable set — the block is implied by the component — so they
reset with it. Two caveats: a global `push_memo` entry embeds the criterion-6
tie-breaks (§6, 2e) of the component that FIRST computed it — deterministic,
admission order being a function of the component input alone, but canonical
per table lifetime, not per component. And two configuration knobs sit in no
key: the solver configuration and `qbf_node_max`. `solver_memo` is valid per
solver configuration and is flushed when it changes; a result-cache entry that
embedded an `unknown`- or budget-based re-wrap stays sound (inv. 3) yet blind
to a stronger solver or a raised budget — flush `push_memo`/`elim_memo` on
either change to reclaim that precision. `qbf_memo` is exempt from every flush:
`DECIDE_FINITE` records there only verdicts its own BDD sweep produced —
mathematical truths, valid under any configuration — while verdicts that came
through its `ASK` fallback live in `solver_memo` under that table's flush rule.

---

## 2. Invariants

1. **The push holds no type-specific reasoning and discharges nothing itself.**
   Every identity it uses holds in any BA; the exceptions are 2a and its
   rank-1 mirror in `PUSH_OVER_DISJUNCTION`, which *ask* the type table
   (`EX_DISTRIBUTES_OVER_NEGATIVES`) rather than deciding anything. Every
   elimination goes through §7. Atomlessness is asserted in exactly two
   places — that capability and `ELIMINATE_ATOMLESS_CLAUSE`'s per-negative
   split (one `NEGATIVE_CONDITION` conjoined per negative) — and they are two
   uses of **one** theorem (`cor:Multivariate-BFs-over`).
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
   zero-normalised; `SQUEEZE_AND_SUBSUME` — shared by
   `ELIMINATE_ATOMLESS_CLAUSE` and `ELIMINATE_FINITE_CLAUSE` (§7, shared
   helpers) — is the only place that rewrites one.
5. **A positive pivot survives only in the plain split** — the last arm of
   `BOOLE_DECOMPOSE_EQUALITY_PIVOT` and all of
   `BOOLE_DECOMPOSE_NON_EQUALITY_PIVOT`. Every other arm absorbs the pivot away
   (2j), substitutes it out (unique zero), or folds it. A retained or negated
   pivot is barred from re-selection by `ctx.used`, which keeps the recursion
   well-founded given that `CHOOSE_PIVOT` sees atoms under `¬`.
6. **Everything the algorithm builds is simplified at the point of
   construction** — an atom by `SIMPLIFY_ATOM`, a cofactor by `SIMPLIFY_TERM`, a
   formula built by substituting into one or conjoining onto one by `SIMPLIFY` —
   before it is recursed into. *Selecting* a subtree needs nothing: a disjunct of
   a simplified disjunction is already simplified, and dropping conjuncts only
   makes a conjunction less contradictory. *Assembling results* needs only their
   top-level interaction: `SIMPLIFIED_OR_JOIN`/`SIMPLIFIED_AND_JOIN` (§3). So the
   push adds no `SIMPLIFY` of its own.
   Not cosmetic: Boole decomposition doubles the formula, and every branch that
   avoids one — every arm of `BOOLE_DECOMPOSE_EQUALITY_PIVOT`, O1–O4 — turns on a
   *syntactic* test, so those arms fire exactly as often as the normal form is
   strong.
7. **Guarded disjunctions are assembled guard-first, and `F` propagates eagerly.**
   A guard is a term comparison; a body is a recursion. No body is built until
   its guard has survived, a guard that folds to `T` makes the later branches
   dead, and a conjunct that folds to `F` decides its clause immediately.
8. **Constant-time work before linear before exponential — the cost ladder is
   strict.** Every dispatch tries its options cheapest class first, and a
   cheaper rung's success makes the dearer rungs unreachable: a memo or pool
   hit, a syntactic test on hash-consed values, a `T`/`F` fold — constant —
   before one traversal (`SIGN_CENSUS`, `TRY_WITNESS`, the union-find
   re-partition) before anything that multiplies (a Boole decomposition, 2b's
   cross product, a solver call). Sound because every rung is one-way (§3): it
   decides or falls through, never approximates. It is why the memo wrappers
   sit outside the workers, the dispatcher tries the fast paths before 2d/2e,
   `PUSH_OVER_DISJUNCTION` ranks its disjuncts, `PUSH_OVER_CONJUNCTION`
   re-partitions before its fast paths, elimination tries witnesses before
   methods, and the decided arms precede the live split. A new branch joins
   the ladder at its cost class — never ahead of a cheaper one.

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
    // clause-level pins, but a re-wrapped intermediate block hides a deep pin
    // from phase 4 for good; at ELIMINATE_BLOCK a clause holds no
    // disjunction, and §4 licenses no conjunct deletion inside a unit.
    D ← ∅ ; flipped ← false ; n ← Φ    // D: path-bound vars barred from FV(t)
    loop:
        if n is a spine:
            if some member is a pin with x ∉ FV(t) and FV(t) ∩ D = ∅:
                // (b) holds free of charge: every node the descent visits
                //   contains ALL free occurrences of x
                return Φ with, at n: the pin dropped, x ← t in the members
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
  what stops nested Boole normal forms from compounding: each `ABSORB_PIVOT`
  emits `x′(…) ∪ x(…)`, and a later absorb wraps that shape again.
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
    result: one top-level absorption pass (d ∨ (d ∧ e) = d), then ⋁ members.

SIMPLIFIED_AND_JOIN — the dual: empty join T, T skipped, F decides, top-level ∧
    flattened, duplicates skipped, literal vs complement member → F, absorption
    d ∧ (d ∨ e) = d.
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

A heuristic, not a monotone gain: a `t` larger than `y` enlarges terms, and
larger terms compare equal less often.

The BDD is canonical over its decision variables only **up to leaf equality**:
semantically equal but structurally different leaves do not merge. That is the
single source of incompleteness behind every syntactic test in the document —
the equality-pivot arm tests, O1–O4, `COFACTOR_REDUCE`. All are one-way; failure
falls through to a more general path.

`NORM_EQUATION` rewrites an atom and is called in exactly one place:
`SQUEEZE_AND_SUBSUME` step 1, whose squeeze needs zero form. Everywhere
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
`TRY_WITNESS`, the unique-zero arm, and `ABSORB_PIVOT`'s clamp MUST descend — each deletes the conjunct that
licensed the substitution (the pinning `x = t`, the pivot), so an untouched
inner occurrence would desynchronise from the outer rewrite — and binder-id
canonicalisation (§3, phase 0) makes the descent capture-safe. Equality
propagation MAY skip a unit and stays sound: its pinning conjunct remains in
place (§3). Everything else treats a unit as an opaque leaf — `OCC` and the
census do not descend, no pivot is chosen inside one, and "holds no
disjunction" (§6) is judged with units opaque. The one whole-unit rewrite is
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
may touch a unit).

```
PUSH_EX_BLOCK(body, X, kf):
    // Partition at ENTRY: merge two block variables when some atom of `body`
    // mentions both. Components then share no atom, push sequentially without
    // interference, may reuse ranks 1..|P|, and are TYPE-HOMOGENEOUS (inv. 2),
    // which gives ELIMINATE_BLOCK one type to dispatch on. Connectivity only
    // decays during the push — PUSH_OVER_CONJUNCTION re-partitions on the way.
    for P in CONNECTED_COMPONENTS(X, body):       // each keeps X's order;
                                                  //   0-based: P[0] outermost
        ctx.type  ← the BA type of P
        ctx.order ← { P[i] ↦ |P|-i }              // inner → LOWER,  ranks 1..|P|
        ctx.prio  ← { P[i] ↦ i+1 }                // inner → HIGHER, ranks 1..|P|
        ctx.used  ← ∅
        ctx.keep_functional ← kf
        ctx.subsume_max ← K = 32 ; ctx.qbf_node_max ← K′ = 2²⁰
                                                  // constants (§1, ctx table)
        ctx.quant_memo ← ∅ ; ctx.pool ← ∅         // the two component-scoped
                                                  //   caches (§1, cache scope)
        ctx.push_memo, ctx.elim_memo, ctx.cof_memo, ctx.occ_memo,
            ctx.solver_memo, ctx.qbf_memo ← the global tables
                                                  // cross-run, never reset (§1)
        body ← PREPARE_TERMS(body, P, ctx.order)  // BDD-back both sides of every
                                                  //   atom touching P (§1). A
                                                  //   FORMULA-level binder unit
                                                  //   is transported opaque (§7
                                                  //   translates it at query
                                                  //   time); a TERM-level
                                                  //   functional quantifier
                                                  //   slides onto its body's
                                                  //   leaves (§1 transparency)
        body ← PUSH_BLOCK(body, P, ctx)
    return body
```

---

## 6. The push — paper step 2

```
PUSH_BLOCK(φ, X, ctx):                            // memo wrapper
    X ← X ∩ FV(φ), keeping X's order   // canonical memo key + tight downstream
                                       //   quantification. FV includes leaf
                                       //   occurrences (§1)
    if X = ∅: return φ
    k ← (φ, X, ctx.keep_functional)
                   // global: the key names all the entry depends on (§1, cache
                   //   scope), so it suffices across components and runs. `used`
                   //   is not in the key — a COMPLETED entry answers ∃X.φ
                   //   whatever `used` produced it (no recursion on a hit), and
                   //   an IN-PROGRESS entry is a miss: meeting the key inside
                   //   its own computation is the recursion `used` exists to
                   //   stop. Components — and runs — execute sequentially, so a
                   //   foreign entry is always complete.
    if ctx.push_memo[k] is complete:    return ctx.push_memo[k]
    if ctx.push_memo[k] is in progress: return PUSH_BLOCK_UNCACHED(φ, X, ctx)
    mark ctx.push_memo[k] in progress
    r ← PUSH_BLOCK_UNCACHED(φ, X, ctx)
    ctx.push_memo[k] ← r
    return r

PUSH_BLOCK_UNCACHED(φ, X, ctx):                   // dispatcher
    if φ is not a conjunction:        // a conjunction tries the fast paths in
                                      //   PUSH_OVER_CONJUNCTION, dependent part only
        r ← TRY_FAST_PATHS(φ, X, ctx)                 // 2a / 2b
        if r ≠ ⊥: return r
    if φ = ⋁ dᵢ:  return PUSH_OVER_DISJUNCTION(φ, X, ctx)
    if φ = ⋀ cᵢ:  return PUSH_OVER_CONJUNCTION(φ, X, ctx)
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

Both end in one-atom clauses handed to §7; neither eliminates anything itself.
Both classify per X-touching leaf — a subtree no `x ∈ X` touches is a constant
under `∃X` and stays neutral.

**2a** distributes `∃X` to each negated atom in one pass, X-free parts riding
along: `∃X.(¬(f=0) ∧ (¬(g=0) ∨ c))` becomes `∃_X f ≠ 0 ∧ (∃_X g ≠ 0 ∨ c)`.
**2b** collapses `⋀(⋁ positives)` by distributing on *terms*, reaching a
one-atom clause without a Boole decomposition.

```
TRY_FAST_PATHS(φ, X, ctx) → formula | ⊥:
    c ← SIGN_CENSUS(φ, X)     // descends ∧/∨ only. A maximal X-free subtree is
                              //   NEUTRAL: counted once in c.free, not descended.
                              //   X-touching leaves: pos = equation, neg =
                              //   negated equation, anything else = other
                              //   (not descended)
    if c.other > 0: return ⊥

    if c.pos = 0 and c.neg > 0:                                        // 2a
        if not EX_DISTRIBUTES_OVER_NEGATIVES(ctx.type): return ⊥
        // ∃X(A ∧ B) ≡ ∃X A ∧ ∃X B for negated atoms — Cor. 5.1, J₁ = ∅; the
        // capability IS atomlessness (inv. 1). c.free is unconstrained: per DNF
        // clause ∃X(⋀ᵢNᵢ ∧ C) = C ∧ ⋀ᵢ∃X Nᵢ, and the leaf-wise transform
        // refactors back through the ∧/∨ skeleton.
        return DISTRIBUTE_TO_ATOMS(φ, X, ctx)

    if c.free = 0 and c.neg = 0 and c.pos > 0:                         // 2b
        // f₁=0 ∧ f₂=0 ≡ f₁∪f₂=0 (prop:squeeze-pos) + distributing ∧ over ∨, on
        // TERMS, any BA. c.free = 0 is required: the term-level squeeze cannot
        // carry a formula-level X-free part through the cross product.
        S ← SQUEEZE_POSITIVES(φ)          // [t₁..t_k] ≜ t₁=0 ∨ … ∨ t_k=0
        return SIMPLIFIED_OR_JOIN(ELIMINATE_BLOCK(t = 0, X, ctx) : t ∈ S)
    return ⊥

DISTRIBUTE_TO_ATOMS(φ, X, ctx):
    // Rebuild φ's ∧/∨ skeleton, replacing each X-touching LITERAL l — atom plus
    // its enclosing ¬, if any — by ELIMINATE_BLOCK(l, X, ctx). The ¬ goes INSIDE
    // the elimination: replacing the bare atom under a kept ¬ would compute
    // ¬(∀_X f = 0), not ∃_X f ≠ 0. X-free subtrees are left verbatim, with no
    // vacuous binder. Nodes are reassembled with the result joins, so a decided
    // literal folds through the skeleton.
```

### 2d — distribute over disjunction

```
PUSH_OVER_DISJUNCTION(⋁dᵢ, X, ctx):
    // Ranked by cost (inv. 8) — tiers 0–2 fully eliminate without splitting: a
    // clause is ONE ELIMINATE_BLOCK call, 2a is |atoms| calls, 2b pays its
    // term cross product; tier 3 decomposes.
    rank(d) = 0 if d holds no disjunction                       // already a clause
              1 if EX_DISTRIBUTES_OVER_NEGATIVES(ctx.type)
                   and every X-touching leaf of d is a negated equation       // 2a
              2 if every leaf of d is an X-touching positive equation         // 2b
              3 otherwise                                       // 2e…
    acc ← an empty SIMPLIFIED_OR_JOIN
    for d in stable_sort(dᵢ by rank):            // cheap disjuncts first, so a T
        insert PUSH_BLOCK(d, X, ctx) into acc    //   short-circuits before anything
        if acc decided T: return T               //   is decomposed. The insert is
    return acc's result                          //   the paper's "unit elimination
                                                 //   after each disjunct"
```

### Conjunction — separate, then dispatch

```
PUSH_OVER_CONJUNCTION(⋀cᵢ, X, ctx):
    dep   ← { cᵢ : some x ∈ X occurs free in cᵢ }   // nonempty: X ⊆ FV(φ) (PUSH_BLOCK)
    indep ← ⋀ { cᵢ ∉ dep }                          // the block does not scope these

    // Re-partition HERE, not only at block entry: the push only ever REMOVES
    // connections. ∃ distributes over variable-disjoint parts, each keeping its
    // own variables: ∃X(ψ₁ ∧ ψ₂) = ∃X₁ψ₁ ∧ ∃X₂ψ₂, Xᵢ = X ∩ FV(ψᵢ). Splitting
    // BEFORE the fast paths keeps 2b's cross product per part and turns
    // 2^(k₁+k₂) into 2^k₁ + 2^k₂ (inv. 8). Union-find, near-linear; the
    // narrowed Xᵢ keeps the memo key canonical.
    parts ← conjuncts of dep grouped by shared block variables
    if |parts| > 1:
        acc ← a SIMPLIFIED_AND_JOIN holding indep
        for p in parts:
            insert PUSH_BLOCK(⋀p, X ∩ FV(p), ctx) into acc
            if acc decided F: return F              // eager F (inv. 7)
        return acc's result
    ψ ← ⋀ dep

    r ← TRY_FAST_PATHS(ψ, X, ctx)              // DEPENDENT part only: squeezing an
    if r ≠ ⊥:                                  //   independent conjunct into every
        return SIMPLIFIED_AND_JOIN(indep, r)   //   disjunct couples it for nothing
    if ψ is a disjunction:                                                 // 2d
        return SIMPLIFIED_AND_JOIN(indep, PUSH_BLOCK(ψ, X, ctx))
    if ψ holds no disjunction:                                     // pushed home
        return SIMPLIFIED_AND_JOIN(indep, ELIMINATE_BLOCK(ψ, X, ctx))
    return SIMPLIFIED_AND_JOIN(indep, DECOMPOSE(ψ, X, ctx))                // 2e–2k
```

### 2e — pivot

**The candidate index.** `CHOOSE_PIVOT` walks nothing: candidates come from two
caches keyed on immutable values, so neither is ever invalidated — a formula an
arm rebuilds is a NEW key that pays only for its fresh spine, an unchanged
subtree hits (asm. 1). Keeping candidates current costs the size of the change.

```
OCC(n, ctx) → { atom ↦ (bare, in_or, count) }:     // memoized in ctx.occ_memo
    atom a             ↦  { a ↦ (true, false, 1) }
    ¬ over an atom a   ↦  { a ↦ (false, false, 1) }        // the INNER atom
    ∧/∨ node           ↦  merge the children's maps LEFT TO RIGHT, a key
                          keeping its first position, so keys sit in document
                          order — bare/in_or OR-ed, counts summed (tree count:
                          a shared subtree contributes through each parent);
                          a ∨ node then sets in_or on every atom of the map
    anything else      ↦  ∅      // quantifier, temporal operator, reference:
                                 //   not descended

ADMIT(atm, ctx):                  // one-time pool record; no-op when present
    // LAZY — paid on the atom's first CHOOSE_PIVOT touch, so a formula the
    // fast paths fully handle classifies nothing. ψ-independent, computed
    // once: at every call X = P ∩ FV(φ) ⊇ FV(atm) ∩ P, so x(atm), and with
    // it class, never varies. Admission order — query order across calls,
    // document order within one (OCC's merge) — is a function of the
    // component input alone, making the criterion-6 tie-break canonical.
    x ← the block variable in atm with the highest prio
    ctx.pool[atm] ← (x, CLASSIFY(atm, x, ctx), the next admission index)
```

```
CHOOSE_PIVOT(ψ, X, ctx) → (atom, variable) | ⊥:
    // ALL atoms are candidates, including those under ¬ — OCC keeps the INNER
    // atom: splitting on f = 0 inside ¬(f = 0) is the same case analysis, and
    // ψ[atm ↦ T] turns ¬atm into ¬T, folded to F. Non-negated is preference
    // (1), not a filter.
    cands ← keys of OCC(ψ, ctx), in map order, skipping ctx.used and every
              atom in which no x ∈ X occurs free       // "x explicit in f"
    ADMIT each candidate, in cands order
    order candidates by, in decreasing priority:       // 1/2/5 read OCC(ψ);
                                                       //   0/6 read the pool
      0. class:  DECIDED > XFREE > UNIQUE > ZERO > PLAIN
      1. bare  — a non-negated occurrence in ψ     — the paper's preference
      2. in_or — occurs inside a DISJUNCTION of ψ  — see below
      3. is an equation
      4. max{ ctx.prio[v] : v ∈ FV(atm) ∩ X }      — innermost variable
      5. count — more collapse under [atm ↦ T/F]
      6. admission index — the stable tie-break of last resort
    return (best candidate, its x(atm))

    // Why (2): a negated-only atom occurring solely as a top-level conjunct
    // makes no progress — φ₁ collapses to F and β = ¬atm ∧ ψ[atm↦F] is ψ again;
    // inside a disjunction the split resolves that disjunction. Lives WITHIN
    // class PLAIN: the same atom classified DECIDED folds ψ in one call.
```

```
CLASSIFY(atm, x, ctx) → DECIDED | XFREE | UNIQUE | ZERO | PLAIN:
    // Ranking heuristic only — the arms re-verify nothing and stay sound for
    // ANY pivot; an all-PLAIN clause selects as if criterion 0 did not exist.
    if atm is not an equation: return PLAIN
    f ← SETTLE_FUNCTIONAL(TERM_OF(atm), ctx)                    // 2f
    (f₀, f₁, p, usable) ← ctx.cof_memo[(f, x)]; on a miss compute and record:
        f₀ ← SIMPLIFY_TERM(f[x←0]) ; f₁ ← SIMPLIFY_TERM(f[x←1])
        usable ← (x ∉ FV(f₀) ∪ FV(f₁))          // §1 leaf hazard
        p ← usable ? SIMPLIFY_TERM(f₀·f₁) : ⊥   // f has a zero iff p = 0
    // ψ-independent facts, computed once per (f, x) across runs; the entry is
    // the handover BOOLE_DECOMPOSE_EQUALITY_PIVOT reads instead of recomputing.
    if not usable:                 return PLAIN
    if p is a nonzero constant
       or (f₀ = f₁, f₀ constant):  return DECIDED   // one call, no new atom
    if f₀ = f₁:                    return XFREE     // x-free guard replaces pivot
    if f₀ = f₁′:                   return UNIQUE    // x substituted out of T-part
    if p = 0:                      return ZERO      // absorb/clamp — firm: the
                                                    //   arm's guard is exactly
                                                    //   this entry, ψ-free
    return PLAIN
```

```
DECOMPOSE(ψ, X, ctx):
    atm, x ← CHOOSE_PIVOT(ψ, X, ctx)
    if atm = ⊥: return REWRAP(ψ, X)
    // One Boole decomposition on atm — case analysis on an atom's truth value,
    // any BA. An equality pivot has cofactors, so a test may decide the case or
    // license absorbing the pivot; a non-equality pivot has none, so its split
    // is always plain and the pivot always retained.
    if atm is an equation:
        return BOOLE_DECOMPOSE_EQUALITY_PIVOT(ψ, atm, x, X, ctx)   // 2g–2j
    return BOOLE_DECOMPOSE_NON_EQUALITY_PIVOT(ψ, atm, X, ctx)
```

### 2f–2j — one Boole decomposition on an equality pivot

The paper's γ₁–γ₅ (2h) appear as **tests, not emitted guards**: a decided case
costs no formula, and only the x-free arm emits a new atom.

```
BOOLE_DECOMPOSE_EQUALITY_PIVOT(ψ, atm, x, X, ctx):
    // atm may be l = r with r ≠ 0: TERM_OF reads a term off it without
    // rewriting; the substitutions below key on atm as it occurs in ψ.
    f ← SETTLE_FUNCTIONAL(TERM_OF(atm), ctx)       // 2f; atm holds iff f = 0
    (f₀, f₁, p, usable) ← ctx.cof_memo[(f, x)]     // prepaid by CLASSIFY
    //   φ₁ ≜ SIMPLIFY(ψ[atm ↦ T])   φ₂ ≜ SIMPLIFY(ψ[atm ↦ F])   // 2g; atm in
    //   neither; built on first use — a decided arm needs only one
    // The block is NEVER split: every recursion is on the whole X; atm stays a
    // conjunct in scope, and lifting happens only in PUSH_OVER_CONJUNCTION.
    // `usable` gates every cofactor test (§1 leaf hazard); the last arm reads
    // no cofactor, so the unusable case falls through to it.

    // ---- atm DECIDED --------------------------------------------------------
    // p a nonzero constant: f has no zero, atm is F for every x, ψ ≡ φ₂.
    if usable and p is a nonzero constant:
        return PUSH_BLOCK(φ₂, X, ctx)

    // ---- atm ≡ (f₀ = 0), x-free: f does not depend on x ---------------------
    // Guard-first (inv. 7). Subsumes f ≡ 0 (guard T) and f ≡ 1 (guard F). No
    // used-bar: atm occurs in neither part and g is a new, x-free atom.
    if usable and f₀ = f₁:
        g ← SIMPLIFY_ATOM(f₀ = 0)
        if g = T: return PUSH_BLOCK(φ₁, X, ctx)
        if g = F: return PUSH_BLOCK(φ₂, X, ctx)
        τ ← PUSH_BLOCK(SIMPLIFY(g ∧ φ₁), X, ctx)
        if τ = T: return T                     // the ¬g part is never built
        return SIMPLIFIED_OR_JOIN(τ, PUSH_BLOCK(SIMPLIFY(¬g ∧ φ₂), X, ctx))

    // ---- atm LIVE: the split. The F-part is the same in all three cases;
    //      only the T-part differs — take the best available. Arm order is
    //      forced: f₀ = f₁′ implies p = 0, so testing has-a-zero first would
    //      never reach the substitution.
    with atm added to ctx.used for the two calls below:    // path-scoped,
                                                           //   restored on every exit
        if usable and f₀ = f₁′:                // unique zero, at x := f₀
            τ ← PUSH_BLOCK(SIMPLIFY(φ₁[x ← f₀]), X, ctx)   // x substituted out
        else if usable and p = 0:              // f has a zero, PROVEN — and the
            // arm is TOTAL over conjunct shapes: a (¬)equation with genuine
            // cofactors is absorbed, every other x-conjunct is clamped by
            // substituting the reproductive solution (ABSORB_PIVOT).
            τ ← PUSH_BLOCK(SIMPLIFY(ABSORB_PIVOT(φ₁, f₀, f₁, x)), X, ctx)  // 2i, 2j
        else:                                  // nothing proven → pivot retained
            τ ← PUSH_BLOCK(SIMPLIFY(atm ∧ φ₁), X, ctx)
        if τ = T: return T                     // the F-part is never built
        // ¬atm is CONJOINED, not substituted: ψ[atm ↦ F] does not assert the
        // atom false, and prop:squeeze-pos has no negative counterpart to
        // absorb it.
        β ← PUSH_BLOCK(SIMPLIFY(¬atm ∧ φ₂), X, ctx)
    return SIMPLIFIED_OR_JOIN(τ, β)
```

```
ABSORB_PIVOT(φ₁, f₀, f₁, x):                                   // 2i, then 2j
    // Sound only under the caller's ONE license — f₀·f₁ = 0 PROVEN, else
    // unsound: f = a, 0 < a < 1, φ₁ = (g ≠ 0), g = 1 gives truth F but
    // absorbed form a′ ≠ 0. Under it the pivot's solutions are the interval
    // W = [f₀, f₁′], and r(x) = f₀ ∪ x·f₁′ is its REPRODUCTIVE solution:
    // f(r(x)) = 0 everywhere, r maps into W, r fixes W pointwise. The arm is
    // TOTAL: a (¬)equation with genuine cofactors is absorbed via them; any
    // other x-conjunct — a reference, a binder unit, a leaf-x atom — is
    // CLAMPED by substituting r, which reaches leaves and unit bodies (§1,
    // §4 what may touch a unit). One witness map proves the mix exact in ANY
    // BA: a rewritten-clause witness x* transfers to x° = r(x*) ∈ W — a
    // clamped conjunct holds at x° by construction, an absorbed positive
    // forces x* ∈ W so x° = x*, and an absorbed negative transfers by
    // g(r(x)) ⊇ x·f₁′g₁ ∪ x′·f₀′g₀ = (f′g)(x).
    // Terms grow here — hence SIMPLIFY_ATOM on emission (inv. 6), which is
    // what the caller's syntactic tests compare against; a clamped reference
    // argument is re-simplified by the substitution itself (§1).
    for every conjunct c of φ₁ in which x occurs free:
        if c is a (¬)equation whose g₀/g₁ below are x-free:  // genuine cofactors
            g  ← TERM_OF(c)
            g₀ ← SIMPLIFY_TERM(g[x←0]) ; g₁ ← SIMPLIFY_TERM(g[x←1])
            c positive  ↦  SIMPLIFY_ATOM( x′(f₀ ∪ g₀) ∪ x(f₁ ∪ g₁) = 0 )  // squeeze f in
            c negative  ↦  the atom UNDER the kept ¬ becomes
                           SIMPLIFY_ATOM( x′f₀′g₀ ∪ x f₁′g₁ = 0 )         // absorb f′ in
                           // a ≠ atom under the ¬ would double-negate
        else:
            c ← SIMPLIFY(c[x ← f₀ ∪ x·f₁′])                  // clamp into W
    return φ₁                    // the pivot is NOT re-attached (2j)

SETTLE_FUNCTIONAL(f, ctx):                                     // 2f
    if ctx.keep_functional: return f      // ch. 6 needs ∀ₓ/∃ₓ symbolic
    if f carries a functional quantifier: discharge by BDD quantification; SIMPLIFY
    return f
```

### One Boole decomposition, plain form — pivot retained

```
BOOLE_DECOMPOSE_NON_EQUALITY_PIVOT(ψ, atm, X, ctx):
    // No cofactors, so nothing absorbs the pivot: folding it to T/F is what
    // removes a non-equality atom from the clause.
    with atm added to ctx.used for the calls below:   // path-scoped, restored on
                                                      //   every exit
        // SIMPLIFY on both inputs (inv. 6): the added conjunct can contradict a
        // sibling or feed equality propagation; without it a contradiction
        // between two non-pivot atoms survives to the next split.
        l ← PUSH_BLOCK(SIMPLIFY(atm ∧ ψ[atm↦T]), X, ctx)
        if l = T: return T                            // r is never built
        r ← PUSH_BLOCK(SIMPLIFY(¬atm ∧ ψ[atm↦F]), X, ctx)
    return SIMPLIFIED_OR_JOIN(l, r)
```

---

## 7. Elimination — the type dispatch

```
ELIMINATE_BLOCK(clause, X, ctx):                      // memo wrapper
    // Reached only when the block is pushed home: a single conjunctive clause,
    // every conjunct of which mentions some x ∈ X.
    if clause ∈ {T, F}: return clause
    k ← (clause, X, ctx.keep_functional)   // exact key (ctx table); nothing
                      //   below recurses back here, so no in-progress state
    if ctx.elim_memo[k] exists: return ctx.elim_memo[k]
    r ← ELIMINATE_BLOCK_UNCACHED(clause, X, ctx)
    ctx.elim_memo[k] ← r
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
- **Return:** a formula equivalent to `∃X.clause`. Discharge what the method
  can; **re-wrap** what it cannot. Never answer `F` for "cannot decide"
  (invariant 3).
- **Freezing is the method's own job** — which conjunct shapes are unusable is
  method-specific: an order atom blocks the atomless squeeze yet is native to
  the solver, and a surviving binder is opaque to the atomless engine yet
  swallowed whole — never opened — by a query-based method whose engine can
  translate its body (§4 finality). Freeze per COMPONENT, never per clause:
  close `{opaque conjuncts} × {variables reaching them}` to a fixpoint, re-wrap
  `∃` around that part only, and continue with the rest.

### Shared helpers

Shared by the atomless and finite methods — both engines' mathematics is
any-BA up to this point. `SQUEEZE_AND_SUBSUME` normalises and prunes the
clause and is the only place an equation is rewritten (invariant 4).
`NEGATIVE_CONDITION` emits the exact condition for ONE negative; conjoining
one per negative is the atomless method's licence alone (inv. 1).

```
SQUEEZE_AND_SUBSUME(clause, X, ctx) → (f, negatives, clause, X):
 1. clause ← NORM_EQUATION applied to every (¬)equation conjunct
    //   zero form for the squeeze; "positives"/"negatives" mean f = 0 / ¬(f = 0).
    //   A swallowed binder unit or a conversion emission (finite method) is
    //   not an atom — it rides along untouched
 2. f ← ⋃ { positives of clause }                        // squeeze; ⋃{} = 0
    // O4, term-order subsumption: gᵢ ≤ gⱼ makes ¬(gⱼ = 0) redundant. Only the
    //   BA order relates the two atoms — propositional simplification cannot
    //   see it. Tested modulo the positive part as f′gᵢgⱼ′ = 0: under f = 0,
    //   f′ = 1 and the test collapses to gᵢ ≤ gⱼ. Dropping now saves the
    //   per-negative work downstream — a DISCHARGE or a fatter query — and
    //   costs no precision (∃_X f′gᵢ ≤ ∃_X f′gⱼ). Gated: k(k−1)/2 tests save
    //   at most k−1 DISCHARGEs, and BDD size is not monotone under ≤ — no
    //   cheap prescreen.
    if |negatives of clause| ≤ ctx.subsume_max:
        keep ← []
        for each negative ¬(gⱼ = 0) of clause, in order:
            // survivors only: mutually subsuming negatives are equal — testing
            //   against all would drop both
            if no gᵢ in keep has SIMPLIFY_TERM(f′·gᵢ·gⱼ′) = 0: keep += gⱼ
        clause ← clause with every negative not in keep dropped
 3. drop from X every variable not occurring in clause
    //   subsumption can remove a variable's last occurrence; such a variable
    //   sat only in dropped negatives, never in f
    return (f, the negatives of clause, clause, X)
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

 1. // Opaque = not a (¬)equation: a reference, an order atom, and EVERY binder
    // unit — this engine answers no queries, so it can swallow none. ALSO
    // opaque: a (¬)equation hiding a block variable in a leaf — quantification
    // is cofactoring (§1 leaf hazard), so DISCHARGE would emit that x free.
    frozen, clause, X ← FREEZE_OPAQUE_COMPONENTS(clause, X)
    if X = ∅: return frozen
 2. f, negatives, clause, X ← SQUEEZE_AND_SUBSUME(clause, X, ctx)
    if X = ∅: return SIMPLIFIED_AND_JOIN(frozen, clause)
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
    // Sole reader of quant_memo. Its X-free key means "quantify the whole
    // block" — sound here alone: a discharged term's block variables lie in X,
    // and P is component-fixed. SETTLE_FUNCTIONAL's single-variable discharges
    // must not share the table: (t, ∀) would collide between ∀ₓt and ∀_X t.
    if ctx.keep_functional: emit the functional-quantifier term as written
    else: read ctx.quant_memo[(term, ∀ or ∃)]; on a miss, one BDD quantification
          over ctx.order, recorded. Emit SIMPLIFY_ATOM of the result. For
          |X| = 1: two substitutions and a meet/join (lem:xelim), no traversal.
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
 1. // Opaque = unreadable for BOTH engines: a conjunct with a reference in its
    // terms, a binder unit whose body is not bv-translatable (a reference or
    // foreign type inside), or an IMPURE conjunct carrying a functional
    // quantifier (no term-level binder in the solver's language — see above).
    // A binder unit with a translatable body is swallowed whole into a query
    // below — how a clause that re-wrapped open gets decided once enclosing
    // blocks close it (§4).
    frozen, clause, X ← FREEZE_OPAQUE_COMPONENTS(clause, X)
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
 1. // Opaque = the atomless step-1 condition plus anything arithmetic — MINUS
    // binder units with recursively pure bodies and MINUS conversion
    // emissions, both swallowed into the tier-2 query (§4). A no-op when
    // called from the router, whose split already guarantees purity.
    frozen, clause, X ← FREEZE_OPAQUE_COMPONENTS(clause, X)
    if X = ∅: return frozen
 2. f, negatives, clause, X ← SQUEEZE_AND_SUBSUME(clause, X, ctx)
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
    // (SQUEEZE_AND_SUBSUME rewrites only the clause it was handed), so l = r
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
| `PUSH_OVER_DISJUNCTION`, `PUSH_OVER_CONJUNCTION` | strict subformula: a disjunct, the dependent part, or one part of the re-partition — `|parts| > 1`, so each part is a proper sub-conjunction |
| `TRY_FAST_PATHS` (2a, 2b) | leaves: both go straight to `ELIMINATE_BLOCK`, never back into `PUSH_BLOCK` |
| `BOOLE_DECOMPOSE_EQUALITY_PIVOT` | the block never shrinks here, so the measure is on the formula: every recursive argument has strictly fewer (atom, block-variable) incidences than `ψ` — `atm` is folded away, replaced by the `x`-free `g`, absorbed, or substituted out. The two parts that keep it — the last arm's `atm ∧ φ₁` and every F-part's `¬atm` — are covered by `ctx.used` instead. Each part is pushed by its own call, so no call receives two copies of `ψ` |
| `BOOLE_DECOMPOSE_NON_EQUALITY_PIVOT` | `ctx.used`: each split bars its pivot and `CHOOSE_PIVOT` skips barred atoms, so a splitting path is bounded by the clause's non-equality-atom count `k`, and the tree by `2^k` — finite per clause, but not by a constant |
| `ELIMINATE_BLOCK` pre-steps | `X` shrinks or the clause is decided |
| `ELIMINATE_BITVECTOR_CLAUSE` (router) | one guarded conversion per conjunct — a rewrite, no recursion — then two variable-disjoint sub-clauses, each handed to its engine exactly once; no re-entry into the push |
| swallowed binder units (`ELIMINATE_FINITE_CLAUSE`, `SOLVE_ARITHMETIC`) | a unit is decided wholesale or re-wrapped, never opened; each enclosing block makes exactly one attempt on it (one post-order pass, §4) — no fixpoint across blocks |
| `DECIDE_FINITE` | one syntactic `BIT_BLAST` pass, then one BDD sweep over a finite bit set, budgeted by `ctx.qbf_node_max` |
| `PROCESS_ALL_BLOCKS` | one post-order pass over a finite tree; each run is eliminated once |

No elimination method re-enters `ANTI_PRENEX`, so the phases of §3 run once per
call. `ABSORB_PIVOT` rewrites the surviving conjuncts — absorbed or clamped —
without removing `x` from them: the decrease comes from `atm`'s own incidences
vanishing, not from `x` leaving the rest.

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
