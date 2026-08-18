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
      consumed only by ELIMINATE_BITVECTOR_CLAUSE

discharge (both valid in ANY BA — thm:boole-const):
    ∃X. f = 0   ≡   ∀_X f = 0
    ∃X. f ≠ 0   ≡   ∃_X f ≠ 0

    ≠ is NOTATION ONLY from phase 3 on: an emitted `h ≠ 0` denotes the
    formula-level ¬(h = 0) (invariant 4) — no fused ≠ atom is ever built.

term representation, per component (PREPARE_TERMS):
    A term is a BDD whose DECISION VARIABLES are exactly the block's variables
    and whose LEAVES hold everything else: free variables, constants, references,
    foreign-typed sub-terms. This is thm:mnf over X only,
    f(x⃗,y⃗) = ⋃_{a ∈ 2^|X|} f(a,y⃗) · x⃗^a, with arbitrary BA coefficients.
    Consequences:
      - cofactoring on x ∈ X is CHILD SELECTION; nothing else is ever cofactored on
      - ∀_X f = meet of the leaves,  ∃_X f = join of the leaves — one traversal
      - substitution (unique-zero arm, TRY_WITNESS) is a BDD compose PLUS
        rewriting x inside the leaves (r(x) ↦ r(t)) — sound for any term, reaches
        every occurrence, needs no leaf guard
      - depth is bounded by |X|, not by the formula's variable count
      - LEAF HAZARD: a block variable may occur inside a leaf (a reference
        argument), where cofactoring does not reach — a leaf is opaque, its
        Shannon expansion may not be assumed — so a leaf-x makes f₀/f₁ not
        cofactors at all. Guarded by `usable`, COFACTOR_REDUCE's FV check, and
        the absorb arm's equation-only license
```

`ctx` carries:

| field | meaning |
|---|---|
| `type` | the block's BA type `τ`. Single, by invariant 2 |
| `order` | BDD variable order. inner → **lower** rank |
| `prio` | pivot priority. inner → **higher** rank |
| `used` | pivots barred on the current path. Needed by **both** decomposition forms: the non-equality split retains the pivot, the equality form's β re-introduces it negated |
| `subsume_max` | constant threshold on a clause's negative count, above which O4's subsumption scan is skipped |
| `keep_functional` | emit `∀_X`/`∃_X` symbolically instead of discharging them |
| `push_memo` | `(φ, X) → formula`, per component. `used` is not in the key; an in-progress entry is a miss (see `PUSH_BLOCK`) |
| `elim_memo` | `(clause, X) → formula`, per component. `used` plays no role in elimination, so the key is exact |
| `quant_memo` | `(term, ∀ or ∃) → term`, per component |
| `cof_memo` | `(settled term, x) → (f₀, f₁, p, usable)`, per component. Filled by `CLASSIFY`, read by `BOOLE_DECOMPOSE_EQUALITY_PIVOT`; the facts are ψ-independent |
| `solver_memo` | canonical closed query → `sat`/`unsat`/`unknown`. Per RUN — shared across components and blocks, never reset |

The two rank conventions are opposite and cannot be merged: `order` is consumed
innermost-first by BDD quantification; `prio` is read by pivot selection, which
prefers the atom carrying the innermost variable.

---

## 2. Invariants

1. **The push holds no type-specific reasoning and discharges nothing itself.**
   Every identity it uses holds in any BA; the single exception is 2a, which
   *asks* the type table (`EX_DISTRIBUTES_OVER_NEGATIVES`) rather than deciding
   anything. Every elimination goes through §7. Atomlessness is asserted in
   exactly two places — that capability and `ELIMINATE_ATOMLESS_CLAUSE`'s
   negative rule — and they are two uses of **one** theorem
   (`cor:Multivariate-BFs-over`).
2. **Type homogeneity is derived, not required.** A term carries one BA type, so
   no atom mentions variables of two types, so the shared-atom partition in
   `PUSH_EX_BLOCK` cannot merge across types — every component is
   type-homogeneous, which makes `ctx.type` well defined. `COLLECT_RUN` does no
   type test. A mixed-type component arises only through a non-atomic conjunct
   (a reference with arguments of several types, a nested binder), and every
   elimination method freezes those.
3. **A surviving quantifier is sound.** Callers read it as *undecided*, never as
   false. Every graceful exit re-wraps; no method may answer `F` for "cannot
   decide".
4. **Negation is at formula level** from phase 3 onward — `¬(f = 0)`, never
   `f ≠ 0`, and no fused negated order operator. Equations are *not*
   zero-normalised; `ELIMINATE_ATOMLESS_CLAUSE` is the only method that rewrites
   one.
5. **A positive pivot survives only in the plain split** — the last arm of
   `BOOLE_DECOMPOSE_EQUALITY_PIVOT` and all of
   `BOOLE_DECOMPOSE_NON_EQUALITY_PIVOT`. Every other arm absorbs the pivot away
   (2j), substitutes it out (unique zero), or folds it. A retained or negated
   pivot is barred from re-selection by `ctx.used`, which keeps the recursion
   well-founded given that `CHOOSE_PIVOT` descends through `¬`.
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

---

## 3. Top level

```
ANTI_PRENEX(φ, keep_functional = false):
    if φ carries no quantifier: return φ
 0. φ ← CANONICALISE_BINDER_IDS(φ)     // makes every later substitution capture-safe
 1. φ ← TO_NNF(φ) ; φ ← SIMPLIFY(φ)
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
        if n = ∃x.ψ:
            ψ* ← TRY_WITNESS(x, ψ) ; if ψ* ≠ ⊥: n ← ψ*
    return φ

TRY_WITNESS(x, ψ) → formula | ⊥:
    // ∃x.(x = t ∧ ψ′(x)) ≡ ψ′(t), any type. Returns the witnessed BODY, never a
    // binder — only the caller knows what failure means (∃x stays in place at
    // phase 2; x stays in the block in ELIMINATE_BLOCK step 1). The callers
    // straddle NORMALIZE_OPERATORS, so the match is spelling-agnostic:
    // x = t | t = x | x + t = 0 | …, and g ≠ 0 may still occur at phase 2.
    if some top-level conjunct of ψ is equivalent to x = t, with x ∉ FV(t):
        return ψ′ with that conjunct dropped and x replaced by t
    return ⊥
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
block `X` in scope), `FOLD_DEGENERATE_BINDERS` (drop a binder over a constant
scope or an absent variable), `NORM_EQUATION` (`l = r ↦ l + r = 0`, descending
through one `¬`) and `TERM_OF` (for an atom `l = r`, the term `l + r`) are
primitives. So are the two aggressive normalisers of invariant 6:

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
  it; only the siblings change. Harmless for the squeeze: an X-free positive
  contributes the same term to both cofactors, which can only make `f₀ = f₁`
  more likely.

A heuristic, not a monotone gain: a `t` larger than `y` enlarges terms, and
larger terms compare equal less often.

The BDD is canonical over its decision variables only **up to leaf equality**:
semantically equal but structurally different leaves do not merge. That is the
single source of incompleteness behind every syntactic test in the document —
the equality-pivot arm tests, O1–O4, `COFACTOR_REDUCE`. All are one-way; failure
falls through to a more general path.

`NORM_EQUATION` rewrites an atom and is called in exactly one place:
`ELIMINATE_ATOMLESS_CLAUSE` step 2, whose squeeze needs zero form. Everywhere
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
binder that survives elimination is itself final — re-examining it yields the
same result. One left standing at the top of a processed matrix stays inside
that matrix rather than joining the enclosing run.

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
machinery treats an inner binder as opaque, so inner structure is transported,
not rewritten.

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
        ctx.subsume_max ← K                       // constant, see §7
        ctx.push_memo ← ∅ ; ctx.elim_memo ← ∅
        ctx.quant_memo ← ∅ ; ctx.cof_memo ← ∅
        ctx.solver_memo ← the run's shared table  // the one field not fresh per
                                                  //   component (§7, ASK)
        body ← PREPARE_TERMS(body, P, ctx.order)  // BDD-back both sides of every
                                                  //   atom touching P (§1)
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
    k ← (φ, X)     // the rest of ctx is component-constant; `used` is not in the
                   //   key — a COMPLETED entry answers ∃X.φ whatever `used`
                   //   produced it (no recursion on a hit), and an IN-PROGRESS
                   //   entry is a miss: meeting (φ, X) inside its own computation
                   //   is the recursion `used` exists to stop.
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
    if φ is an atom or a negated atom:
        return ELIMINATE_BLOCK(φ, X, ctx)             // a one-literal clause
    return REWRAP(φ, X)      // reference, nested binder, temporal operator, or ¬
                             //   over one: pushed as far as it goes (inv. 3)
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
        if S = ⊥: return ⊥                // cross-product cap exceeded (§9)
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
    // Ranked by cost — tiers 0–2 fully eliminate without splitting: a clause is
    // ONE ELIMINATE_BLOCK call, 2a is |atoms| calls, 2b multiplies only up to
    // the squeeze cap; tier 3 decomposes.
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
    // 2^(k₁+k₂) into 2^k₁ + 2^k₂. Union-find, near-linear; the narrowed Xᵢ
    // keeps the memo key canonical.
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

```
CHOOSE_PIVOT(ψ, X, ctx) → (atom, variable) | ⊥:
    // ALL atoms are candidates, including those under ¬ — the INNER atom is
    // collected: splitting on f = 0 inside ¬(f = 0) is the same case analysis,
    // and ψ[atm ↦ T] turns ¬atm into ¬T, folded to F. Non-negated is preference
    // (1), not a filter.
    cands ← atoms of ψ, top-down, descending through ¬ but not into quantifiers
              or temporal operators; atoms in ctx.used are skipped
    keep only atoms in which some x ∈ X occurs free        // "x explicit in f"
    for atm in cands: x(atm) ← the X-variable in atm with the highest prio
    order candidates by, in decreasing priority:
      0. CLASSIFY(atm, x(atm), ctx):  DECIDED > XFREE > UNIQUE > ZERO > PLAIN
      1. has a BARE (non-negated) occurrence in ψ     — the paper's preference
      2. occurs inside a DISJUNCTION of ψ             — see below
      3. is an equation
      4. max{ ctx.prio[v] : v ∈ FV(atm) ∩ X }         — innermost variable
      5. occurrence count of atm in ψ         — more collapse under [atm ↦ T/F]
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
    // ψ-independent facts, computed once per (f, x) per component; the entry is
    // the handover BOOLE_DECOMPOSE_EQUALITY_PIVOT reads instead of recomputing.
    if not usable:                 return PLAIN
    if p is a nonzero constant
       or (f₀ = f₁, f₀ constant):  return DECIDED   // one call, no new atom
    if f₀ = f₁:                    return XFREE     // x-free guard replaces pivot
    if f₀ = f₁′:                   return UNIQUE    // x substituted out of T-part
    if p = 0:                      return ZERO      // absorb — OPTIMISTIC: its
                                                    //   ψ-dependent guard is
                                                    //   re-checked in the arm
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
        else if usable and p = 0               // f has a zero, PROVEN
                and every atom of φ₁ carrying x is a (¬)equation with x only a
                decision variable, never in a leaf:
            // The absorb rewrites each x-atom via ITS cofactors, so every
            // x-atom must have them; an unrewritten conjunct would escape the
            // pivot's solution interval once the pivot is deleted.
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
    // Sound only under the caller's TWO licenses; nothing here re-checks.
    // (1) f₀·f₁ = 0 PROVEN — else unsound: f = a, 0 < a < 1, φ₁ = (g ≠ 0),
    // g = 1 gives truth F but absorbed form a′ ≠ 0. (2) Every x-atom of φ₁ a
    // (¬)equation with x only a decision variable, so the loop reaches every
    // x-carrying conjunct and each g₀/g₁ is a genuine cofactor.
    // Terms grow here — hence SIMPLIFY_ATOM on emission (inv. 6), which is what
    // the caller's syntactic tests compare against.
    for every atom a of φ₁ in which x occurs explicitly:
        g  ← TERM_OF(a)
        g₀ ← SIMPLIFY_TERM(g[x←0]) ; g₁ ← SIMPLIFY_TERM(g[x←1])
        a positive  ↦  SIMPLIFY_ATOM( x′(f₀ ∪ g₀) ∪ x(f₁ ∪ g₁) = 0 )   // squeeze f in
        a negative  ↦  the atom UNDER the kept ¬ becomes
                       SIMPLIFY_ATOM( x′f₀′g₀ ∪ x f₁′g₁ = 0 )          // absorb f′ in
                       // a ≠ atom under the ¬ would double-negate
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
    k ← (clause, X)   // exact key (ctx table); nothing below recurses back
                      //   here, so no in-progress state
    if ctx.elim_memo[k] exists: return ctx.elim_memo[k]
    r ← ELIMINATE_BLOCK_UNCACHED(clause, X, ctx)
    ctx.elim_memo[k] ← r
    return r

ELIMINATE_BLOCK_UNCACHED(clause, X, ctx):
    // Type-agnostic pre-steps — pure identities, run once here, not per method.
 1. for x in X:                                // a witness beats any method:
        if clause ∈ {T, F}: break              //   cheaper and exact
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
| bitvector | `ELIMINATE_BITVECTOR_CLAUSE` | **false** — `bv[1]`: `∃x(x≠0 ∧ x′≠0)` is F |
| anything else | `⊥` → re-wrap | **false** — fail closed |

**Method contract.**

- **Given:** a single conjunctive clause; `X` non-empty; every `x ∈ X` occurs in
  the clause; every conjunct mentions some `x ∈ X`; `X` homogeneous of type `τ`.
- **Return:** a formula equivalent to `∃X.clause`. Discharge what the method
  can; **re-wrap** what it cannot. Never answer `F` for "cannot decide"
  (invariant 3).
- **Freezing is the method's own job** — which conjunct shapes are unusable is
  method-specific: an order atom blocks the atomless squeeze yet is native to
  the solver. Freeze per COMPONENT, never per clause: close
  `{opaque conjuncts} × {variables reaching them}` to a fixpoint, re-wrap `∃`
  around that part only, and continue with the rest.

### Atomless

```
ELIMINATE_ATOMLESS_CLAUSE(clause, X, ctx):
    //  ∃X.( f=0 ∧ ⋀ⱼ gⱼ≠0 )  ≡  ∀_X f=0 ∧ ⋀ⱼ ∃_X f′·gⱼ ≠ 0     (cor:qelim-main)

 1. // Opaque = not a (¬)equation (a reference, a binder, an order atom) — and
    // ALSO a (¬)equation hiding a block variable in a leaf: quantification is
    // cofactoring (§1 leaf hazard), so DISCHARGE would emit that x free.
    frozen, clause, X ← FREEZE_OPAQUE_COMPONENTS(clause, X)
    if X = ∅: return frozen
 2. clause ← NORM_EQUATION applied to every conjunct
    //   zero form for the squeeze; "positives"/"negatives" mean f = 0 / ¬(f = 0)
 3. f ← ⋃ { positives of clause }                        // squeeze; ⋃{} = 0
    // O4, term-order subsumption: gᵢ ≤ gⱼ makes ¬(gⱼ = 0) redundant. Only the
    //   BA order relates the two atoms — propositional simplification cannot
    //   see it. Tested modulo the positive part as f′gᵢgⱼ′ = 0: under f = 0,
    //   f′ = 1 and the test collapses to gᵢ ≤ gⱼ. Dropping now
    //   saves the DISCHARGE and costs no precision (∃_X f′gᵢ ≤ ∃_X f′gⱼ).
    //   Gated: k(k−1)/2 tests save at most k−1 DISCHARGEs, and BDD size is not
    //   monotone under ≤ — no cheap prescreen.
    if |negatives of clause| ≤ ctx.subsume_max:
        keep ← []
        for each negative ¬(gⱼ = 0) of clause, in order:
            // survivors only: mutually subsuming negatives are equal — testing
            //   against all would drop both
            if no gᵢ in keep has SIMPLIFY_TERM(f′·gᵢ·gⱼ′) = 0: keep += gⱼ
        clause ← ⋀ { positives } ∧ ⋀ { ¬(g = 0) : g ∈ keep }
    drop from X every variable not occurring in clause
    //   subsumption can remove a variable's last occurrence; such a variable
    //   sat only in dropped negatives, never in f
    if X = ∅: return SIMPLIFIED_AND_JOIN(frozen, clause)
    // pos holds even where it folds to T (no positives ⇒ f = 0 vacuously);
    //   O1–O3 reason under it as a sibling assumption.
    pos ← DISCHARGE(∃X. f = 0, ctx)                               // ⇒ ∀_X f = 0
    if pos = F: return F                  // f has no common zero: ∃X.clause is F
    f_indep ← (COFACTOR_REDUCE(f, X) ≠ ⊥)     // loop-invariant, computed once
    // Per-negative ∃_X conjuncts are the OTHER use of atomlessness (inv. 1).
    // The shortcuts rest on:  ∃_X f′ = (∀_X f)′,  and  ∃_X(f′·g) = g · ∃_X f′
    // for g independent of X.
    neg ← an empty SIMPLIFIED_AND_JOIN
    for each negative ¬(g = 0) of clause:
        if g ≡ 1: continue           // O1: ∃_X f′g = (∀_X f)′ = 0′ ≠ 0, by pos
        if f_indep:                  // O2: f indep of X + pos ⇒ f′ = 1
            insert DISCHARGE(∃X. g ≠ 0, ctx) into neg             // ⇒ ∃_X g ≠ 0
        else if g* ← COFACTOR_REDUCE(g, X) ; g* ≠ ⊥:
            // O3: ∃_X f′g = g · (∀_X f)′ = g. Emit the REDUCED term: g may
            //   still spell an X-variable, and that variable has no binder here.
            insert SIMPLIFY_ATOM(g* ≠ 0) into neg
        else:
            insert DISCHARGE(∃X. f′·g ≠ 0, ctx) into neg          // ⇒ ∃_X f′g ≠ 0
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
    if ctx.keep_functional: emit the functional-quantifier term as written
    else: read ctx.quant_memo[(term, ∀ or ∃)]; on a miss, one BDD quantification
          over ctx.order, recorded. Emit SIMPLIFY_ATOM of the result. For
          |X| = 1: two substitutions and a meet/join (lem:xelim), no traversal.
```

### Bitvector

```
ELIMINATE_BITVECTOR_CLAUSE(clause, X, ctx):
 1. // Opaque = a conjunct with a reference or surviving binder in its terms —
    // untranslatable. Order and equality atoms are both native to the solver.
    frozen, clause, X ← FREEZE_OPAQUE_COMPONENTS(clause, X)
    if X = ∅: return frozen
 2. q ← ∃X.clause
    if q is closed:
        ASK(q, ctx):  sat ⇒ return frozen,  unsat ⇒ return F,  unknown ⇒ step 3
    else:               // close the free variables Z both ways
        if ASK(∀Z.q, ctx) = sat:    return frozen  // q valid
        if ASK(∃Z.q, ctx) = unsat:  return F       // q unsatisfiable
 3. return SIMPLIFIED_AND_JOIN(frozen, REWRAP(clause, X))    // undecided (inv. 3)
```

```
ASK(q, ctx) → sat | unsat | unknown:
    // Key = CANONICALISE_BINDER_IDS(q): alpha-variants collide across
    // components, blocks, and the two closings. `unknown` is cached too — the
    // solver configuration is fixed for the run. elim_memo cannot subsume this:
    // it is per component, and different (clause, X) can normalise to the same
    // query after freezing.
    k ← CANONICALISE_BINDER_IDS(q)
    if ctx.solver_memo[k] exists: return ctx.solver_memo[k]
    r ← the solver's verdict on k
    ctx.solver_memo[k] ← r ; return r
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
| `PROCESS_ALL_BLOCKS` | one post-order pass over a finite tree; each run is eliminated once |

No elimination method re-enters `ANTI_PRENEX`, so the phases of §3 run once per
call. `ABSORB_PIVOT` rewrites the surviving atoms without removing `x` from
them: the decrease comes from `atm`'s own incidences vanishing, not from `x`
leaving the rest.

---

## 9. Open items

- **No method for a finite/atomic BA type.** `bool` — a 2-element, hence atomic,
  BA — hits `METHOD(τ) = ⊥` and re-wraps. The method that belongs there is the
  paper's atomic case: positives by squeeze + Boole consistency (any BA),
  negatives by Hall's theorem on minterms (ch. 2 §Distinct Representatives,
  `thm:main-ineq` and the corollary after it).
- `TODO (HIGH)` at `normal_forms.tmpl.h:114` — guard the four negated-order
  rewrites to operators bound to arithmetic comparison (today: `bv[n]`), as
  `NORMALIZE_OPERATORS` specifies.
- **Standing assumption: no lattice-ordered comparison operator.** Every `≤`/`<`
  denotes arithmetic comparison (today `bv[n]`), so the guarded rewrites in
  `NORMALIZE_OPERATORS` cover every order atom and `≰ ≮ ≱ ≯` never survive the
  pass. A lattice-order spelling would need an untyped `a ≰ b ↦ ¬(a ≤ b)` rule
  beside `f ≠ 0 ↦ ¬(f = 0)` — sound in any order, no operand swap — and would
  want `ELIMINATE_ATOMLESS_CLAUSE` to consume `f ≤ g` via `f g′ = 0` rather
  than freeze it.
- `SQUEEZE_POSITIVES`' cross-product cap: value, and whether declining to the
  general path is the right response to exceeding it.
