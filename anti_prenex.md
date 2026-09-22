# Anti-prenexing — pseudocode baseline

Spec: Chapter 5 of `taba0.5.tex` (= ch. 5, p. 51 of `taba0.5.pdf`). Paper steps
`2a`–`2k` are cited inline; step `(3)`, the universal branch, is handled by
dualisation in `PROCESS_BLOCK`. Steps `2e`–`2j` — one Boole decomposition
on a chosen pivot — are one licensed decomposition (§6) under a
no-duplication test: `TRY_DECOMPOSE`, on the atom whose two signs sit in
sibling conjuncts. When the chosen atom is a PIN — an equation whose zeros
fix a binder variable, so its truth substitutes that variable away (§3,
`TRY_WITNESS`) — its T-arm deletes that binder. All
remaining case analysis is `EXPAND` (§6), distributing disjuncts the
formula already contains. A disjunction of negated equations, a NEGATIVE
TREE (§1), never blocks a variable's settling and is taken whole by the
clause methods when it reaches a leaf.

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
|φ|      cached node count, LAZY: computed on the first query, memoised on
         the node, never recomputed — a tree count over the hash-consed DAG
         (the sort convention of 2d, the case witness, and EXPAND, and the
         metric of §5's size acceptance), O(1) at every query. An atom counts
         1, its terms are not children; a reference and a temporal operator
         count 1 like an atom; |¬ψ| = |Qx.ψ| = 1 + |ψ|; a connective chain is
         counted along its STORED BINARY SPINE, so k members give
         k−1 + Σ |members|
‖·‖      the count IN MEMORY (§10) of a TERM: for a PLAIN term its distinct
         term-structure nodes — operators, functional quantifiers and leaves
         once each, wrappers not counted, a leaf's inside not entered — and
         for a BDD-backed term the BDD's distinct nodes, its terminals
         counting 0 — TRY_WITNESS's tie-break among a variable's
         pins reads it, the cost every later compose or replace of the
         witness pays. No procedure reads a formula's in-memory count (the
         decomposition licence, §6, uses |·|); §10 names it only in the
         cost argument
neg(φ)   the NNF of ¬φ — TO_NNF's NEG (§3), factored — a pure function of
         the node: computed on first demand and cached on it
h(φ)     cached structural hash, set at construction from the node's kind and
         its children's hashes — wide enough that collisions are negligible.
         Every input is canonical (phase-0 binder ids, source names for free
         variables, BDD-canonical terms), so h is a pure function of CONTENT:
         identical across runs, components, and inputs sharing subtrees
FV(φ)    cached free-variable set, LAZY: computed on the first query,
         memoised on the node, never recomputed — the union of the children's
         sets, the leaf occurrences inside backed terms and inside reference
         arguments included, formula-binder and functional-quantifier
         subscripts excluded — every X ∩ FV test in this document is an
         intersection of cached sets, never a walk

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
    PURE (¬)equation: both terms built from variables, constants, the BA
      operations ∪ · ′ + and functional quantifiers alone — no reference, no
      arithmetic, no foreign-typed subterm. The literal shape §7's engines
      decide without a solver (the router's split extends it to emissions
      and pure units); 2a's top-level positives and the consistency check
      (§6) take only these
    NEGATIVE TREE: an ∨-node all of whose leaves are negated equations —
      ∧-nodes allowed inside; a top-level ∧ of such things is not one tree
      but several conjuncts, each a literal or a tree. The leaf methods take
      a tree whole (§7), so it is FINAL for scope narrowing: it never flags a
      variable as unsettled (§6). It stays a disjunctive conjunct for
      EXPAND, which may distribute over it like any other (§6, EXPAND).
      Untouched by the classification: SIMPLIFY's literal rules, |·|, h,
      FV. Post phase 3 no negated order atom exists (NORMALIZE_OPERATORS),
      so "negated equation" names every negative leaf. FLAT: a tree whose
      members are all literals; NESTED otherwise
    PLAIN conjunct: not an ∨-node — a literal, a unit, a reference.
      DISJUNCTIVE conjunct: an ∨-node, a negative tree included

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
    Only the sides of EQUATIONS are backed: an order atom is never cofactored
    (cof_memo keys are TERM_OF of equations) and the solver path consumes it
    as written, so it stays plain whatever it touches. BDD-backed means "has
    a decision variable": a term that stops touching the block — a cofactor,
    a quantification over all of it — is plain again, and a block-free term
    never carries a BDD. One BDD is one term node (interned), so the D2 round
    trip through a component boundary returns the node it started from.
    Consequences:
      - cofactoring on x ∈ X is CHILD SELECTION; nothing else is ever cofactored on
      - ∀_X f = meet of the leaves,  ∃_X f = join of the leaves — one traversal
      - substitution (TRY_WITNESS, at every call site) is a BDD compose
        PLUS rewriting x inside the leaves (r(x) ↦ r(t)) — sound for any term,
        reaches every occurrence, needs no leaf guard. Every reference argument
        it changes is handed ONCE to the caller's re-simplification, so
        arguments stay simplified (inv. 6): phase 1 simplifies them at entry
        (§3), and substitution is the only rewrite that reaches inside a
        reference (§4). Needed because references are
        leaves: a dead occurrence like x·x′ ∪ a inside an argument would keep
        the FV guards (X ∩ FV, `usable`) conservative
      - depth is bounded by |X|, not by the formula's variable count
      - FUNCTIONAL QUANTIFIERS ARE TRANSPARENT — term operators binding their
        subscript: Y is excluded from FV and alpha-renamed with the formula
        binders. Cofactoring on x ∉ Y commutes with them, so PREPARE_TERMS
        backs the BODY over P and slides the quantifier onto the body's
        leaves — ∀_Y(x·b₁ ∪ x′·b₀) = x·∀_Y b₁ ∪ x′·∀_Y b₀, dually for ∃_Y: a
        Y-substitution is a homomorphism fixing x, and meet/join distribute
        over an orthogonal decomposition. Linear in the body's BDD; nesting
        recurses innermost-first. The slide lives in the BDD library's term
        construction and fires at PREPARE_TERMS and at the entry pass alone,
        over PLAIN bodies: a chain is collected whole, its body built under
        the order and the chain wrapped onto every leaf; a chain touching no
        decision variable is one canonical leaf, put through the
        constructor. No later term construction slides: a chain met by
        SIMPLIFY_TERM, TERM_OF or any BDD operation of the push is ONE LEAF,
        as it stands — in particular a keep-mode emission of a settled
        sub-block (§7 DISCHARGE), whose body still carries block variables;
        those are HIDDEN from then on (leaf hazard, below). A stored BDD is
        never spelled out before the component's close, and never
        reordered. One order is alive at a time, the component's; a chain's
        own order exists at entry alone (`RESOLVE_FUNCTIONAL_PLAIN`, §3),
        before any order is live. Under keep_functional, deep block nesting stacks
        quantifier chains at the leaves — linear per level, amortised by
        hash-consing, inherent to symbolic mode.
      - LEAF HAZARD: a block variable may occur inside a leaf (a reference
        argument), where cofactoring does not reach — a leaf is opaque, its
        Shannon expansion may not be assumed — so a leaf-x makes f₀/f₁ not
        cofactors at all. A functional quantifier slid at PREPARE_TERMS
        hides no block variable — a reference or foreign subterm does,
        including one inside a quantifier's body — and so does a keep-mode
        emission of a settled sub-block, a chain over a stored BDD whose
        body still carries block variables (§7 DISCHARGE): the push never
        looks into it again. The test is LEAF_FV(t) ∩ X ≠ ∅ (§3),
        read behind `usable` (COF) and the methods' `opaque?` (§7)
```

`ctx` carries:

| field | meaning |
|---|---|
| `type` | the block's BA type `τ`. Single, by invariant 2 |
| `order` | BDD variable order. inner → **lower** rank |
| `prio` | variable priority. inner → **higher** rank — read by `EXPAND`'s disjunct key, `DECOMPOSE_ARMS`'s choice among a pin atom's pinned variables, and `FOLD_DECIDED`'s probe variable |
| `subsume_max` | threshold on a clause's negative count, above which `SUBSUME_NEGATIVES` (finite method only) skips its O3 scan. `K = 32`. Free to tune: subsumption costs no precision, so neither taint nor flush attaches (cache scope, below) |
| `qbf_node_max` | node budget for `DECIDE_FINITE`'s BDD sweep — peak live nodes of ONE sweep, checked by allocation high-water mark — past which it falls back to `ASK`. `K′ = 2²⁰` |
| `case_max` | threshold on a case pin's branch count, above which the case witness (`TRY_CASE_WITNESS`, `TRY_WITNESS_DEEP`) declines — each branch copies the surrounding spine, repaid only by the deleted binder; past it the conjunct is left to the decomposition and `EXPAND` (§6). `K″ = 16`. Phase 2's `TRY_WITNESS_DEEP` predates any ctx and reads the constant `K″` bare |
| `expand_max` | budget on cases built by `EXPAND` — shared by `TRY_DECOMPOSE`'s arms (§6) — counted per component (`ctx.expand_count`) — past it, the still-unexpanded members re-wrap as ONE pending block and every finished case is kept (inv. 3). `K‴ = 2¹⁴`. A hit taints its computation (cache scope, below) |
| `expand_count` | cases built so far in this component, written by `EXPAND` and `DECOMPOSE_ARMS` and read against `expand_max`. Reset to 0 at component setup (§5) |
| `accept_growth` | growth factor of the per-component SIZE ACCEPTANCE (§5): a component push whose result exceeds `max(γ·\|input\|, accept_floor)` is discarded for the re-wrapped input (inv. 3). `γ = 16`. Neither taint nor flush attaches (cache scope, below) |
| `accept_floor` | absolute `\|·\|` under which acceptance never fires — moderate growth is routine and often repaid downstream; the test exists for detonation (§5). `2²⁰` |
| `propagate_growth` | cap on `SIMPLIFY`'s pin environment (§3): a pin whose admission would push `Σ‖witnesses‖` past this factor times `Σ‖TERM_OF(pinning conjunct)‖` is refused. `4`. Read BARE from the process-wide defaults — `SIMPLIFY` runs in every phase and has no ctx. Precision, never soundness; neither taint nor flush attaches |
| `absorb_occ_max` | occurrence limit of the result joins' absorption pass (§3): a part occurring in more members than this is no candidate key, and a member all of whose parts exceed it stays unabsorbed. `32`. Read BARE — the joins have no ctx. Precision, never soundness |
| `taint_count` | budget hits so far, GLOBAL, never reset: incremented by every source of taint, read by the memo wrappers, which cache only across an unchanged count (cache scope, below) |
| `keep_functional` | decided PER BLOCK by the caller's callback (`ANTI_PRENEX`'s parameter): emit `∀_X`/`∃_X` symbolically instead of discharging them; in the `push_memo`/`elim_memo` keys (cache scope, below). ONE callback on a NODE, a pure function of it: per block it is handed the block's binder node — `REWRAP(matrix, X)`, the run head with its variables outermost first and the matrix below (§5) — and per chain the canonical chain's term node, prefix and body (`RESOLVE_FUNCTIONAL_PLAIN` at entry, `RESOLVE_FUNCTIONAL_BDD` at a component's close, §3). It reads what it needs off the node: a prefix or a variable list alone would not let a policy look at what is quantified |
| `push_memo` | `(REWRAP(φ, X), keep_functional) → formula`, GLOBAL (cache scope, below) — the ordered `X` is carried by the wrap node, so formula and block are one key part. Also the state memo of `EXPAND`: an expansion state IS its formula, and merging is this table firing on canonically assembled children (§6) |
| `elim_memo` | `(REWRAP(clause, X), keep_functional) → formula`, GLOBAL — the wrap node carries the ordered `X`, as in `push_memo`. The key names everything an elimination reads, so it is exact |
| `cof_memo` | `(term, x) → (f₀, f₁, p, usable, pin)`, GLOBAL. Filled and read by `COF`; consumers: the phase-4 pin matches — `TRY_WITNESS` in COF mode, the case pin — and `DECOMPOSE_ARMS`'s pin arm, the same test met at a decomposition's atom: pin iff `usable ∧ f₀ ∪ f₁ = 1`, witness `f₁′`, residual `p = 0`, STRICT when `p` folds to `0` (§3, `TRY_WITNESS`); and `FOLD_DECIDED`. Every consumer forms the key itself, `TERM_OF(·)`, before calling `COF`. Pure functions of the key — a chain the term carries is part of it |
| `atoms_memo` | `formula node → the atoms reachable in it through ∧, ∨ and ¬ alone` — units, references and temporal operators are opaque (§4), so a reference's formula argument contributes none — GLOBAL, purely structural. Read by the occurrence guard of `[atm ↦ T/F]` (§10) |
| `solver_memo` | canonical closed query → `sat`/`unsat`/`unknown`, the query built on plain (converted) terms — a BDD-backed term is a node of one order (§3 `PREPARE_TERMS`). GLOBAL — valid per solver configuration, flushed when it changes (cache scope, below) |
| `qbf_memo` | canonical closed pure-Boolean query → `T`/`F`, written only by `DECIDE_FINITE`'s own sweep. GLOBAL — entries are mathematical truths, never flushed (cache scope, below) |

Every constant above (`K`, `K′`, `K″`, `K‴`, `γ`, the floor, the propagation
factor and the occurrence limit) is provisional pending benchmarks.

COMPONENT STATE — `type`, `order`, `prio`, the six component knobs,
`keep_functional` and `expand_count` — is set at component setup (§5) and
lives for that component alone. GLOBAL — `taint_count` and the seven tables —
outlives the call (cache scope, below). The component knobs are read from the
process-wide defaults at setup, so a changed one applies from the next
component on; `propagate_growth` and `absorb_occ_max` belong to `SIMPLIFY` and
the joins, which run in every phase without a ctx, and are read bare at every
use.

The two rank conventions are opposite and cannot be merged: `order` is consumed
innermost-first by BDD quantification; `prio` is read by `EXPAND`'s disjunct
key, which expands the disjunct whose latest variable is outermost first, by
`DECOMPOSE_ARMS`, which substitutes the innermost pinned variable of a pin
atom, and by `FOLD_DECIDED`, which probes on the innermost variable of an atom.

**Cache scope** follows one rule: a table is GLOBAL — outliving the call,
shared across `ANTI_PRENEX` runs, components, and blocks — exactly when its key
names everything its entries depend on. The variable set is explicit: the
ordered `X` of `push_memo`/`elim_memo` fixes the type, every atom's block
subset (`FV(atm) ∩ P = FV(atm) ∩ X`), and the relative prios; `cof_memo`
carries its `x`; `atoms_memo` is purely structural; `solver_memo` and
`qbf_memo` involve none. `keep_functional` joins `push_memo`'s and
`elim_memo`'s keys alone — their entries embed `DISCHARGE`'s output, which
the flag switches — while `cof_memo`'s key is the term as it arrives, a kept
chain included. Every tie-break is the content order (§1), so no entry embeds
construction history. Five knobs sit in no key — the solver configuration,
`qbf_node_max`, `expand_max`, `accept_growth` and `accept_floor` — and are
handled three ways. TAINT: a computation that hit a budget — an `expand_max`
exhaustion re-wrap, a `qbf_node_max` sweep abandoned to `ASK` that ended
`unknown` — returns its result (sound, inv. 3) but writes no
`push_memo`/`elim_memo` entry, and taint is transitive through every result
assembled from it; so no entry embeds budget or counter state, and raising a
budget needs no flush, only recomputation where the budget bound. Taint is
CARRIED BY A COUNTER, never by the result: results are hash-consed nodes,
and the same formula can be built untainted elsewhere, so no mark may sit
on a node. `ctx.taint_count` counts budget hits; each source increments it,
and each memo wrapper reads it before its computation and writes the entry
only if it is unchanged after — transitivity for free, since a hit inside
the computation is a hit inside every enclosing one. FLUSH:
`solver_memo` is valid per solver configuration and is flushed when it
changes; a `push_memo`/`elim_memo` entry that embedded a genuine solver
`unknown` stays sound (inv. 3) yet blind to a stronger solver, so both are
flushed with it. NEITHER for the acceptance pair: its verdict is re-derived
outside every cache (§5) and acts only by re-wrapping a formula that later
keys contain, so a re-tuned γ or floor re-judges the same pushes. `qbf_memo`
is exempt from every flush: `DECIDE_FINITE` records there only verdicts its
own BDD sweep produced — mathematical truths — while verdicts from its `ASK`
fallback live in `solver_memo` under that table's flush rule.

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
   type test. The partition descends into nested binders, so an atom inside
   a unit connects like any other; a reference connects nothing — it may
   mention variables of several components and types, and every elimination
   method freezes it — and a surviving binder is frozen unless its body is
   translatable for the method's engine, in which case the unit is swallowed
   whole into a query (§7), never opened.
3. **A surviving quantifier is sound.** Callers read it as *undecided*, never as
   false. Every graceful exit re-wraps; no method may answer `F` for "cannot
   decide".
4. **Negation is at formula level** from phase 3 onward — `¬(f = 0)`, never
   `f ≠ 0`, and no fused negated order operator. Equations are *not*
   zero-normalised; `SQUEEZE` — shared by `ELIMINATE_ATOMLESS_CLAUSE` and
   `ELIMINATE_FINITE_CLAUSE` (§7, shared helpers) — is the only place that
   rewrites one.
5. **Case analysis stays inside the formula's own atom vocabulary — no
   manufactured atoms.** Every multiplying step — 2d, the case witness,
   the decomposition, `EXPAND` — distributes members the formula already
   contains or splits on an atom it already contains: the decomposition's
   F-arm conjoins the NEGATION of an atom of ψ while `[a ↦ F]` erases every
   other occurrence, and `EXPAND`'s exclusions are complements of members
   of a disjunct of ψ, so the vocabulary never grows.
   The paper's Boole decomposition is spread over the ladder: a
   pin substitutes out through the witness step (spine level), the case
   witness (all branches of one disjunct), and the decomposition's T-arm
   (a pin atom the census selects, at any depth); the licensed
   decomposition takes the both-signed atom whose worlds span sibling
   conjuncts — both cofactors erase every reachable occurrence, and the
   guard left behind is one-signed, so the census never re-selects its
   atom; a constant-false atom folds through `FOLD_DECIDED`, a finished
   variable leaves through scope narrowing's settle move, and the rest is
   decided at the leaves, where the clause methods are complete.
6. **Everything the algorithm builds is simplified at the point of
   construction** — an atom by `SIMPLIFY_ATOM`, a cofactor by `SIMPLIFY_TERM`, a
   formula built by substituting into one or conjoining onto one by `SIMPLIFY` —
   before it is recursed into. *Selecting* a subtree needs nothing: a disjunct of
   a simplified disjunction is already simplified, and dropping conjuncts only
   makes a conjunction less contradictory. *Assembling results* needs only their
   top-level interaction: `SIMPLIFIED_OR_JOIN`/`SIMPLIFIED_AND_JOIN` (§3). So the
   push adds no re-simplification pass of its own; its deep `SIMPLIFY`s —
   `EXPAND`'s case merge, where E's and d's material first meet, and the
   case witness's and the decomposition's arm edges,
   where a substitution just rewrote the copy — are construction sites, not
   passes.
   Not cosmetic: expansion multiplies the formula, and every step that avoids
   one — the pin matches, the case pins, the decomposition's pin test,
   `FOLD_DECIDED`, O1–O3 — turns on a *syntactic* test over simplified
   values, so those steps fire exactly as often as the normal form is strong.
7. **Guarded disjunctions are assembled guard-first, and `F` propagates eagerly.**
   A guard is a term comparison; a body is a recursion. No body is built until
   its guard has survived, a guard that folds to `T` makes the later branches
   dead, and a conjunct that folds to `F` decides its clause immediately.
8. **Constant-time work before linear before exponential — the cost ladder is
   strict.** Every dispatch tries its options cheapest class first, and a
   cheaper step's success makes the dearer steps unreachable: a memo hit, a
   syntactic test on hash-consed values, a `T`/`F` fold — constant — before
   one traversal (`SIGN_CENSUS`, `TRY_WITNESS`, the incidence pass) before
   anything that multiplies (an expansion, 2b's cross product,
   a solver call). Sound because every step is one-way (§3): it decides or
   falls through, never approximates. It is why the memo wrappers sit
   outside the workers, the dispatcher tries the fast paths before 2d and
   the expansion, `PUSH_OVER_DISJUNCTION` orders its disjuncts
   smallest-first, `PUSH_OVER_CONJUNCTION` narrows scopes and checks the
   positives' consistency before anything multiplies, elimination tries
   witnesses before methods, the push kills binders — pin, case pin — then
   decomposes licensed before expanding, and `EXPAND` queues cheap members
   first. A new branch joins the ladder at its cost class — never ahead of
   a cheaper one.

---

## 3. Top level

```
ANTI_PRENEX(φ, keep_functional = (· ↦ false)):   // a CALLBACK on a NODE, pure:
                                                  //   a block's binder node
                                                  //   (§5) or a canonical
                                                  //   chain's term node (§3)
                                                  //   → bool (§1 ctx table)
    if φ carries no quantifier: return φ
 0. φ ← CANONICALISE_BINDER_IDS(φ)     // formula binders AND functional-
                                       //   quantifier subscripts: makes every
                                       //   later substitution capture-safe and
                                       //   the canonical query keys (ASK,
                                       //   DECIDE_FINITE) collide across
                                       //   alpha-variants
 1. φ ← RESOLVE_FUNCTIONAL_PLAIN(φ, keep_functional)
                                       // every chain PRESENT in the input,
                                       //   the callback asked (§3); from
                                       //   here on a chain is a kept or a
                                       //   stuck one, and it stays
    φ ← TO_NNF(φ) ; φ ← SIMPLIFY(φ, ref_args = true)
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

TRY_WITNESS(x, ψ, ctx = ⊥) → formula | ⊥:
    // ∃x.(x = t ∧ ψ′(x)) ≡ ψ′(t), any type. Returns the witnessed BODY, never a
    // binder — only the caller knows what failure means (x stays in the block
    // in ELIMINATE_BLOCK step 1, its direct caller; phase 2 reaches the same
    // match per spine through TRY_WITNESS_DEEP, with no ctx: nothing is
    // BDD-backed there, so only the spelled match applies). The call sites
    // straddle NORMALIZE_OPERATORS, so the SPELLED match is spelling-agnostic:
    // x = t | t = x | x + t = 0 | …, and g ≠ 0 may still occur at phase 2.
    // On BDD-backed conjuncts — phase 4, ctx present — the match reads COF
    // (§6) on the conjunct's term, TERM_OF(c)
    // — cof_memo's key (§1), which every pin site forms itself: a
    // positive f = 0 PINS x iff usable ∧ f₀ ∪ f₁ = 1. Boole's expansion
    // f = x′f₀ ∪ xf₁ puts the zeros of f at f₀ ≤ x ≤ f₁′, an interval
    // that f₀ ∪ f₁ = 1 (f₁′ ≤ f₀) collapses to a point:
    //     f = 0   ≡   x = f₀  ∧  p = 0,   p = f₀f₁              — any BA
    // — the pin, and a RESIDUAL free of x. The one-point rule then gives
    // ∃x.(f = 0 ∧ ψ′) = p = 0 ∧ ψ′[x ← w] for ANY w agreeing with f₀
    // wherever p = 0 — any w in [f₁′, f₀]. THE WITNESS IS THE LOWER END
    // f₁′ = f₀·p′: it carries the residual into every sibling's terms,
    // where p = 0 then folds syntactically; the upper end f₀ = f₁′ ∪ p
    // would carry p as inert bulk into the same terms. The paper's
    // reproductive solution f₀ ∪ x·f₁′ is x-free exactly under this test:
    // it is the case where the absorb arm deletes a binder. A pin is
    // STRICT when p folds to 0 — f₁′ = f₀ — the spelled match, and
    // every atom with f₁ = f₀′. The squeeze hides pins from the strict
    // test: a settled sub-block emits ∀_X f = 0 over ALL its positives
    // (§6, §7), so a pin y = t among them returns as (y + t) ∪ G = 0,
    // cofactors t ∪ G and t′ ∪ G, complementary only where G = 0; the
    // weak test reads it back — witness tG′, residual G = 0.
    // THE PINNING CONJUNCT STAYS, substituted like every sibling: its image
    // f[x ← f₁′] = f₁f₀ ∪ f₁′f₁ IS p = 0, so a strict pin folds it to T
    // and no site emits a residual on its own. Several pins on x: a strict
    // one first, else the smallest witness — the scan holds every COF
    // entry already. The result is SIMPLIFIED here (inv. 6): a formula
    // built by substitution, handed to every caller ready to recurse into.
    if some top-level conjunct of ψ pins x — the spelled match, or, with ctx,
            COF(TERM_OF(c), x, ctx).pin on an
            equation conjunct c:
        c ← a strict pin among them, else the pin of smallest ‖f₁′‖
        return SIMPLIFY(ψ with x replaced by c's witness in EVERY conjunct,
                        c included)
    return ⊥

TRY_WITNESS_DEEP(Q, x, Φ) → formula | ⊥:             // phase 2 only
    // A SPINE is a flattened conjunction (Q = ∃) or disjunction (Q = ∀), and
    // ANY OTHER NODE is a one-member spine, so a lone pin or a bare case pin
    // fires like one among siblings; a
    // PIN is a spine member ≡ x = t (∃) or ≡ x ≠ t (∀) — TRY_WITNESS's
    // SPELLED match (nothing is BDD-backed at phase 2, §3), negated for ∀,
    // strict, so the member is dropped. For a spine S reached from Φ's
    // root through ∧, ∨, and
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
    // at ELIMINATE_BLOCK a clause holds no disjunction but a negative tree
    // (§1), which holds no pin, and §4 licenses no conjunct deletion inside
    // a unit.
    D ← ∅ ; flipped ← false ; n ← Φ    // D: path-bound vars barred from FV(t)
    loop:
        S ← the members of n if n is a spine, else [n]   // any other node is
                                                         //   a one-member spine
        if some member of S is a pin with x ∉ FV(t) and FV(t) ∩ D = ∅:
            // (b) holds free of charge: every node the descent visits
            //   contains ALL free occurrences of x
            return Φ with, at n: the pin dropped, x ← t in the members
                // on a one-member spine the join is empty: T for ∃, F for ∀
        if some member of S is a CASE PIN for x (∨-of-branches for ∃, dualized
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
                //   above; the rewrite is local to S, like the plain one; a
                //   bare case pin — n the member itself — carries nothing
        if n is a spine or the other connective (∨ for ∃, ∧ for ∀):
            if x free in ≥ 2 members: return ⊥      // x-free siblings ride
            n ← the one member holding x            //   along outside Qx
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
every branch of which has a conjunct pinning `x` with witness `tᵢ`
(`TRY_WITNESS`'s match — spelled at phase 2, COF's at phase 4, read on the
settled term and choosing among several pins of a branch as `TRY_WITNESS`
does), `x ∉ FV(tᵢ)`, with at most `K″` branches. The match
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
    //       ∃x.(D ∧ R) = ⋁ᵢ ∃x.(dᵢ ∧ R) = ⋁ᵢ (dᵢ ∧ R)[x ← tᵢ]
    //   — any BA; each branch's pin stays and becomes its residual
    //   (TRY_WITNESS). Distribution is the size trade EXPAND pays member by
    //   member; here every branch deletes the binder, so k substituted
    //   copies of R replace an expansion subtree. All-or-nothing: a branch
    //   without a pin would keep ∃x alive inside its copy — a partial or
    //   nested pin, or a case pin wider than case_max, is left to the
    //   decomposition and EXPAND (§6). tᵢ may mention other block
    //   variables — same-kind binders commute above x. R-side units take
    //   x ← tᵢ by §4's one licensed inside-a-unit rewrite.
    //   This function only MATCHES — no copy is built here: the ladder (§6)
    //   emits the rewrite branch by branch, so a deciding branch is the only
    //   copy ever paid for; phase 2's dualized match rewrites in place
    //   inside TRY_WITNESS_DEEP.
    for each conjunct D = ⋁ᵢ dᵢ of ψ that is a CASE PIN for x (above),
            smallest |D| first:
        return ( [ (dᵢ, tᵢ) : i ], ψ without D )
    return ⊥
    // Cost: one linear scan per x, only on the expansion-bound path.
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
    // as neg(φ) (§1) — written by NEG's entry on first demand; the walk
    // keeps each ¬ψ it rewrites in passing in its own per-node memo, so a
    // later demand for neg(ψ) is one lookup there, never a recomputation.
    ¬ψ ↦ NEG(ψ)      atom ↦ atom      ∧ / ∨ ↦ re-emitted through the joins over
                                       //   the normalised members — phase 1
                                       //   canonicalises the raw input's chains
    Qx.ψ′ / always ψ′ / sometimes ψ′ ↦ rebuilt over the normalised body
    // One walk: a ¬ met on the way down is pushed one level and the walk
    // continues into the result; ¬¬ folds, a binder or a temporal operator
    // under ¬ flips, an atom or a reference under ¬ keeps its ¬ — never a
    // fused ≠, never a negated order operator (inv. 4).

NEG(ψ) → the NNF of ¬ψ:                      // neg(ψ): computed once per node
    T ↦ F ; F ↦ T ; ¬ψ′ ↦ TO_NNF(ψ′)
    an atom, or a reference ↦ its ¬            // as written: no ≠, no negated
                                             //   order operator (inv. 4)
    ∃x.ψ′ ↦ ∀x.NEG(ψ′) ; ∀x.ψ′ ↦ ∃x.NEG(ψ′)  // a unit's quantifiers flip in —
                                             //   and back out on the second
                                             //   negation (§5)
    always ψ′ ↦ sometimes NEG(ψ′) ; sometimes ψ′ ↦ always NEG(ψ′)
                                             // temporal quantifiers dualise
                                             //   like binders; opacity is the
                                             //   PUSH's rule (§4), not TO_NNF's
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

`TO_NNF`, `SIMPLIFY` (TWO PASSES over an NNF formula, in this order: EQUALITY
PROPAGATION — see below — then the PATH SWEEP: per-path contradiction, unit
elimination and the tautology dual, every literal of a junction applied to its
siblings as ONE simultaneous environment (§10), constants folding through the
construction hooks. Neither pass re-spells an atom (inv. 4). Absorption and
canonical assembly are NOT `SIMPLIFY`'s: they belong to the result joins at the
construction sites; `SIMPLIFY` rewrites in place, and a chain keeps its nesting
minus, or with, its rewritten members. One IMPLICIT parameter, the block `X`
in scope — bound through the live order — and one flag, `ref_args = false`,
set by phase 1 alone, which ESTABLISHES invariant 6: with it every atom goes
through `SIMPLIFY_ATOM`, and the traversal also descends into reference
arguments, recursively, running each through `SIMPLIFY_TERM` — a reference's
arguments are always terms, only the reference itself can be a formula.
Elsewhere only the atoms propagation rewrote are re-simplified, and an argument
is touched only by a pin's substitution, which re-simplifies what it changed
(§1)), `FOLD_DEGENERATE_BINDERS` (drop a binder over a
constant scope or an absent variable — formula binders and functional
quantifiers alike; the term constructor applies the same rules when the
symbolic `∀_Y f`/`∃_Y f` term is formed: a subscript not free in `f` is
dropped, a repeated subscript is kept at its innermost occurrence, each
same-kind run sits in content order outermost first — one node per SET per
kind — an adjoining same-kind run of `f` is merged into it, and a chain
binding every free variable of a PLAIN `f` is folded to its constant unless a
subscript hides in a leaf), `NORM_EQUATION(atom, order)`
(`l = r ↦ l + r = 0`, descending through one `¬`), `TERM_OF(atom, order)` (for
an atom `l = r` — or `l ≠ r`, which phase 1 meets before
`NORMALIZE_OPERATORS` — the term `l + r`, read through one `¬`) and
`REWRAP(φ, X, kind = ∃)` (re-attach the block around `φ`, in `X`'s order —
every graceful exit of invariant 3; the kind defaults to `∃` and the dualised
`∀` form is available, `X` is duplicate-free, and the set is attached as given
— `REWRAP` narrows nothing, every caller narrows for itself) are primitives.
So are the two aggressive normalisers of invariant 6:

- `SIMPLIFY_TERM(t, order = ∅)` — constant folding, complement laws, per-path
  contradiction, the cofactor check, and reduction to the canonical form of
  the BDD backing `t`. Per-path contradiction is `prop:xfx` — `x·f(x) =
  x·f(1)`, `x′·f(x) = x′·f(0)` — so a sub-term under a literal is reduced by
  that literal's assignment; absorption is its literal instance (`x ∪ x·c =
  x`), and a compound one (`ab ∪ ab·c`) stays as written. The COFACTOR CHECK
  is one-level Shannon reduction of the WHOLE term over every free variable
  of `t` that is not a variable of `order`: an order variable in a Boolean
  position the BDD has decided already, and one still free in a leaf sits
  where the expansion is not licensed (§1 leaf hazard). For each such `y`,
  `f₀ = t[y←0]` and `f₁ = t[y←1]`, the substitution reaching BOOLEAN
  POSITIONS only — under `∪ · ′ +` and the functional quantifiers, never
  under an arithmetic operator, a reference or a foreign-typed subterm, where
  no Shannon expansion holds — with constants folding through the hooks;
  `f₀ = f₁`, a constant included, makes `t ← f₀`. Sound for a variable that
  also sits inside a reference: the reference is an opaque element the
  expansion is taken around. TWO PHASES, no interplay: the sweep first, then
  the check ONCE over the swept term, one pass over its variables on the
  running term; a cofactor is what the substitution hands back, folded by
  the hooks and simplified no further, and the check's result is emitted as
  it stands. It is what stops nested Boole normal forms from compounding as
  substitutions stack terms inside terms.
- `SIMPLIFY_ATOM(a, order = ∅)` — one recipe in both regimes. `SIMPLIFY_TERM`
  on both sides, the atom rebuilt through the construction hooks, which fold
  a constant-only atom and equal sides. For a (¬)equation the joint ring sum
  `l + r` — `TERM_OF`'s, a BDD operation once a side is BDD-backed — then
  goes through `SIMPLIFY_TERM` as a DECISION alone: `0` decides `=` as `T`, a
  nonzero constant as `F`, dually for `≠` (phase 1 meets it before
  `NORMALIZE_OPERATORS`); anything else leaves the atom as its sides stand.
  Never reshaped, and `NORM_EQUATION` (`SQUEEZE`) stays the one rewrite of
  an equation's shape. An order atom is side-wise alone. NOT idempotent: a
  second call may simplify further, since the check's result is not swept
  again; keys do not drift because an atom is simplified ONCE, at
  construction, and again only when a substitution rewrote it.

The `order` these four carry is the LIVE order of §1's term representation:
the EMPTY order is the plain regime — phases 1, 2 and 5 — and a term touching
no variable of `P` is plain whatever the order says.

The remaining primitives are defined by their contracts alone:

- `CANONICALISE_BINDER_IDS(φ)` — renames every binder, formula binders and
  functional-quantifier subscripts alike, to canonical ids BY DEPTH: a
  binder's id is `1 +` the maximum binder depth below it, in one id space
  shared by formula binders and functional quantifiers. Alpha-variants are
  then one node: phase 0's normaliser and the key of `ASK` and
  `DECIDE_FINITE` (§7). An outer binder's id strictly exceeds every id below
  it — siblings may share one — and that strict decrease is what makes
  `[x ← t]` capture-safe.
- `RESOLVE_FUNCTIONAL_PLAIN(φ, kf)` — resolves every functional-quantifier
  chain PRESENT in `φ` that the callback `kf` does not keep: the input's own
  chains, at entry (phase 1), before any order is live. No term of `φ` is
  BDD-backed.

  The unit of work is the MAXIMAL chain at its top, any mix of `∀`/`∃`,
  found anywhere — under binders, in reference arguments, in chain bodies.
  The chains nested inside its body are resolved first: "innermost first"
  means NESTED, never adjacent binders. Per chain: canonicalise through the
  term constructor (absent subscripts dropped, a repeated one kept
  innermost, same-kind runs in content order, an adjoining run merged, a
  closed plain chain folded), then ask `kf` ONCE, on the canonical chain's
  NODE — prefix outermost first with kinds, the body below it, for the
  callback to read. Kept ⇒ the whole chain stays. Else the body is built as
  a BDD over the CHAIN'S OWN order, innermost subscript lowest, and handed to
  the chain step (below) with that order as the live one; what comes back
  is spelled PLAIN before the pass moves on — a node minted under a chain's
  order belongs to no live order — and a chain nothing of which resolved is
  the node as written. Chain orders re-rank a subscript from chain to chain,
  so the library's tables clear between chains; nothing is cached yet, which
  is why the chain's own order lives here alone.

  After this pass a chain in `φ` is a kept one or a stuck one, and both
  stay: no later step resolves a chain it finds in a leaf.

- `RESOLVE_FUNCTIONAL_BDD(φ, order, kf)` — the component's close (§5):
  resolves every functional-quantifier chain in the TREE of `φ` that `kf`
  does not keep. `order` is the live order; every BDD-backed term of `φ`
  belongs to it, and every chain met sits over a BDD-backed body with its
  subscripts among the decision variables — keep mode's whole-block emission
  `Q_X (bf(BDD_ID))`, `X ⊆ P` (§7 `DISCHARGE`). A chain over a plain body is
  one whose BDD did not branch — its subscripts occur in leaves alone — and
  it stays as it is. Leaves are never entered: a chain in a leaf is an inner
  block's kept emission or a stuck one, both decided.

  Same unit of work, same canonicalise-then-ask as above; kept ⇒ the whole
  chain stays. Else THE CHAIN STEP: ONE quantification of the stored BDD
  under the live order — `bdd_ex`/`bdd_all` for a prefix of one kind,
  `bdd_quant` for a mixed one, nested by the ranks it runs under (an
  emission is one kind; an entry chain's order nests by construction) —
  emitted under the live order, plain when nothing branches.

  LEAF HAZARD: a subscript hiding in a leaf — a reference argument, a
  foreign-typed subterm — is reached by no quantification and STAYS. The
  others are taken from the prefix by the STUCK RULE, innermost first: the
  first hidden subscript opens a stuck run of its kind; an outer subscript
  of that kind is still quantified (it commutes into the run) or joins the
  run when hidden; the first subscript of the OTHER kind blocks everything
  outward. The kept prefix, in its order, is re-attached over the quantified
  body through the constructor. A choice of size, not of soundness —
  crossing an alternation by cofactors is legal, and duplicates the stuck
  chain — and a choice per NEST: a stuck chain nested deeper in the body was
  slid onto the leaves by the build (§1 transparency), where an outer
  subscript's quantification crosses it.

  Neither resolver has a memo of its own: the entry pass runs once, and the
  close's quantifications are covered by the library's memos.
- `φ[x ← t]` and `φ[atm ↦ T/F]` — capture-aware substitution, each returning
  an untouched subtree by one cached test, `FV` and `atoms_memo` (§10);
  `[x ← t]` descends into units (§4) and re-simplifies what it touches
  (§1), `[atm ↦ T/F]` reaches through ∧, ∨ and ¬ alone — units, references and
  temporal operators are opaque, so a formula argument of a reference is not
  in its vocabulary — and erases every occurrence it reaches.

  `[x ← t]` is ONE substitution primitive, shared with the rest of the system.
  It prepares `t` itself: a backed `t` is spelled out once, and when `t`
  carries a binder — formula binder or functional quantifier — the bound
  variables of `t` are RENAMED APART, to fresh ids above the largest id in
  `φ` and in `t`, the ids inside backed terms included, so no binder on any
  path of `φ` equals a subscript inside `t`. Ids are never renumbered there;
  phase 5 restores depth-canonical ids. The substitution is simultaneous and
  the key is matched by content; a node is entered only if `x` is free in it
  (one cached `FV` test), so a rebound `x` is left alone; a backed term is
  rewritten leaves first and then by one compose; a reference argument is
  rewritten and re-simplified once per changed argument; temporal operators
  are entered.
- `COLLECT_RUN(h)` — the maximal same-kind quantifier run from `h`, with its
  matrix (§4).
- `CONNECTED_COMPONENTS(X, body)` — the partition of `X` by atom
  connectivity, descending into nested binders, references connecting
  nothing (§5); each component in `X`'s order.
- `PREPARE_TERMS(body, P, order)` — §1's term representation over `P`:
  both sides of every EQUATION touching `P` backed by a BDD whose decision
  variables are `P` in the given order, everything else in the leaves,
  functional quantifiers slid onto the leaves; order atoms left plain;
  binder units, references and temporal operators transported opaque. The
  caller owns `order` and passes it to every later term operation. A SOLVER
  QUERY is built on converted terms, since a backed term is a node of one
  order and a `solver_memo` key holding one would never hit across
  components.
- `FINISH_TERMS(φ)` — `PREPARE_TERMS`' inverse, run at the component's close
  (§5): every BDD-backed term of `φ` converted back to a plain term wherever
  it sits — under a REWRAPped binder, inside a symbolic functional
  quantifier's body, inside a reference argument, under a temporal operator.
  One memoised walk. Inverse up to term normal form: the round trip returns
  the node it started from, by interning.
- `LEAF_FV(t)` — the free variables the LEAVES of `t` contribute: for a
  BDD-backed term the union over its leaves, for a plain term its whole `FV`.
  The leaf-hazard test (§1) is `LEAF_FV(t) ∩ X ≠ ∅`, read behind `usable`
  (`COF`, §6) and the methods' `opaque?` (§7).
- `SQUEEZE_POSITIVES(φ)` — for an ∧/∨ skeleton whose leaves are positive
  equations, terms `t₁ … t_k` with `φ ≡ t₁ = 0 ∨ … ∨ t_k = 0`, obtained by
  distributing ∧ over ∨ on terms, a conjunction squeezing to the union
  (prop:squeeze-pos; §6, 2b).
- `SPLIT_ARITHMETIC(clause, X)` — `(A, X_A), (B, X_B)`: `A` the impure
  conjuncts closed under the variables reaching them, to a fixpoint, with
  `X_A` those variables; `B` and `X_B` the rest, disjoint (§7, the router).
- `IS_LINEAR_ARITHMETIC(c)`, `LINEAR_ARITHMETIC_TO_BA(c)` — black boxes
  (§7, the router; §9).
- `METHOD(τ)`, `EX_DISTRIBUTES_OVER_NEGATIVES(τ)` — the type table (§7).

**The result joins** assemble already-simplified formulas into a disjunction or
conjunction:

```
SIMPLIFIED_OR_JOIN(r₁, …, rₙ):     // INCREMENTAL: a builder fed one operand at
                                   //   a time, answering decided? in between,
                                   //   the n-ary form a loop over it (inv. 7);
                                   //   an empty join is F
    // A SET of top-level disjuncts, taken LEFT TO RIGHT: a deciding member
    // stops later operands from being evaluated — or BUILT — at all.
    inserting r:
        r = T                   →  the join is T
        r = F                   →  skip
        r = ⋁ rⱼ                →  insert each rⱼ       // flatten one level
        r already a member      →  skip                 // O(1), hash-consed (asm. 1)
        r a literal whose complement is a member  →  the join is T   // unit elim
        anything else           →  add r
    result: one top-level absorption pass (d ∨ (d ∧ e) = d) — indexed the
    way SAT subsumption is. A member's PARTS are its members when it is the
    dual connective (here the conjuncts of an ∧-member), else itself. An
    occurrence list part → members; a member's candidates are the members
    of its RAREST part, the test a marking subset test; a part in more than
    absorb_occ_max members is no candidate key, and a member all of whose
    parts exceed it stays unabsorbed (precision, never soundness — EXPAND's
    cases share nearly every part and are skipped for free) — then ⋁ members
    EMITTED IN THE CONTENT ORDER (§1): insertion order drives evaluation and
    the short-circuits, but the assembled node is a function of the member
    SET — with flattening for associativity, assembly is AC-canonical, so
    converging expansion states build EQUAL memo keys (§6).

SIMPLIFIED_AND_JOIN — the dual: empty join T, T skipped, F decides, top-level ∧
    flattened, duplicates skipped, literal vs complement member → F, absorption
    d ∧ (d ∨ e) = d under the same index and limit, members emitted in the
    content order.
```

Shallow by design: every operand is already simplified (invariant 6), so only
the members' top-level interaction is new — exactly what the insert rules and
the absorption pass cover. A member is never descended into, and
`SIMPLIFIED_AND_JOIN` does **no equality propagation** between members:
conjuncts that must interact at term level — every `SIMPLIFY(atm ∧ …)`
construction of §6 — still go through `SIMPLIFY`.

**Equality propagation**, `SIMPLIFY`'s first pass. A conjunct pinning a FREE
variable licenses substituting the pin's witness for that variable in its
sibling conjuncts: the only step that carries an assumption from one conjunct
into another's *terms* (unit elimination is propositional; per-path
contradiction works inside a single term). It is what lets the syntactic tests
fire in cases like `f = xy ∪ x′a`, `x ∈ X`: `f₀ = a` and `f₁ = y` compare
unequal until `y := a` turns `f` into `a`.

- **The pin is `TRY_WITNESS`'s, on plain terms.** A POSITIVE equation
  conjunct `c` — an equation under `¬` pins nothing — that is `X`-free, hence
  plain (§1), pins a free `y` of `f = TERM_OF(c)` iff `usable ∧ f₀ ∪ f₁ = 1`
  on the cofactors `f₀ = SIMPLIFY_TERM(f[y←0])`, `f₁ = SIMPLIFY_TERM(f[y←1])`;
  the witness is `f₁′`, STRICT when `p = f₀f₁` folds to `0` — the spelled
  shape `y + t = 0`, `y ∉ FV(t)`, is exactly the strict case, `t = f₁′` — else
  WEAK, with the residual `p = 0` kept by the conjunct itself. Several pins in
  one conjunct: a strict one first, else the smallest `‖f₁′‖`. One match, ONE
  implementation, shared with `TRY_WITNESS`'s spelled mode (§3).
- **Orientation.** If either side pins a block variable, `TRY_WITNESS`
  eliminates that variable instead. A free `y` is propagated only when
  `FV(t) ∩ X = ∅` — pushing a block variable into a leaf breaks the
  child-is-cofactor identity (§1) — which an `X`-free conjunct guarantees.
  "The block in scope" is `SIMPLIFY`'s implicit parameter: calls inside
  `PUSH_BLOCK` or an elimination method bind it to the component's `X`;
  phases 1–2 and 5 bind it to `∅` (nothing is BDD-backed there), making the
  guard vacuous.
- **The pinning conjunct stays.** `y` is free, so the conjunct still
  constrains it; only the siblings change. Sound for a weak pin too: the
  conjunct keeps `p = 0` in place, and `f₁′ = f₀·p′` agrees with `f₀` wherever
  `p = 0`. Harmless for the squeeze: an X-free positive contributes the same
  term to both cofactors, which can only make `f₀ = f₁` more likely.
- **Deep, one pass, chained — two stages at the conjunction.** ONE traversal
  of the conjunction. MATCH first: the `X`-free positive equation conjuncts
  in content order, each rewritten by the pins admitted so far and then
  tested — a conjunct that is, or thereby BECOMES, a pin JOINS the
  environment for the conjuncts after it, so chained pins propagate within
  the pass, the environment kept idempotent (a new witness rewritten by the
  pins in force, the ranges in force rewritten by the new pin). Then REWRITE
  every conjunct under the environment the match ended with: the equations
  at the conjunction — a pinning one under the environment RE-DERIVED
  WITHOUT IT, the match rerun over the other candidates with its variable
  barred from being pinned, so it stays and keeps constraining its variable
  while every other pin reaches it (its own entry merely removed from the
  final environment would not do: a later admission can fold that pin into
  another's range, and the conjunct would meet its own equation and fold to
  `T`) — and the rest as the traversal meets them. So the
  result does not depend on the order of the equations, and the example
  above folds whichever conjunct sorts first. Not a fixpoint: nothing is
  re-matched after the rewrite, and a pin that only surfaces once the sweep
  has folded something waits for the next construction site that
  `SIMPLIFY`s it. The substitution descends through a
  sibling's whole ∧/∨ structure — a disjunctive sibling's members included —
  and INTO binder units (§4), a pin suspended under a binder over its variable
  or over a variable of its witness; temporal operators are opaque to both
  passes, as they are to `[atm ↦ T/F]`; a reference never matches — it is
  no equation — but a pin's substitution reaches its arguments like any
  other occurrence and re-simplifies an argument it changed, once (§1);
  under `ref_args` every argument goes through `SIMPLIFY_TERM`, changed or
  not. Each atom the pass rewrites is re-emitted through
  `SIMPLIFY_ATOM`. §8 leans on the depth — a case inherits `D`'s members
  already specialised at ψ's construction — and on the CHAINED pin, which,
  propagated or not, is the one growth channel the `EXPAND` measure cannot
  rule out.
- **The environment is CAPPED.** A witness on its own is smaller than the
  equation it came from; what grows is the CHAINING of ranges. A pin is
  admitted only if, with it in and the ranges rewritten by it,
  `Σ‖witnesses‖` stays within `propagate_growth · Σ‖TERM_OF(pinning
  conjunct)‖` over the environment; a pin that would cross the line is
  refused and the match goes on to the next candidate — precision, never
  soundness (§1 ctx table).

A heuristic, not a monotone gain: a `t` larger than `y` enlarges terms, and
larger terms compare equal less often.

The BDD is canonical over its decision variables only **up to leaf equality**:
semantically equal but structurally different leaves do not merge. That is the
single source of incompleteness behind every syntactic test in the document —
the pin matches, `FOLD_DECIDED`, O1–O3. All are one-way;
failure falls through to a more general path.

`NORM_EQUATION` rewrites an atom and is called in exactly one place:
`SQUEEZE` step 1, whose squeeze needs zero form. Everywhere
else equations stay as written — as `SIMPLIFY_ATOM` left them at construction,
simplified once there and again only when a substitution rewrote them, so no
key drifts — `TERM_OF` reads a term off an atom
without touching it, and every substitution keyed on an atom uses the atom as
it occurs in the formula.

---

## 4. Phase 4 driver — one post-order pass

```
PROCESS_ALL_BLOCKS(φ, kf):                    // kf: the caller's callback
    memo ← ∅                                  // node → formula, whole pass;
                                              //   exact because kf is a pure
                                              //   function of the block
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

**What may touch a unit.** Two classes of REWRITE reach inside a unit's
body: substitution of a free variable, and `SIMPLIFY`'s two passes. (Reading is different: a
query-based method swallows a unit wholesale, and its translation — the
solver's, `BIT_BLAST`'s — consumes the body without rewriting it.)
`TRY_WITNESS` — phase 2's deep pass, the elimination pre-steps, and the push's
witness steps — MUST descend: each use consumes the pinning conjunct that
licensed the substitution — dropped at phase 2, substituted into its `x`-free
residual at phase 4 (§3) — so an untouched inner occurrence would
desynchronise from the outer rewrite — and binder-id canonicalisation (§3,
phase 0) makes the descent capture-safe. `SIMPLIFY`
DESCENDS into a unit's body with both passes: a pin, or a literal key, of an
enclosing conjunction holds throughout that conjunction's scope, so rewriting
or folding its occurrences inside the unit is sound; a pin or key is SUSPENDED
under a binder over its variable, or over a variable of the pin's witness;
the pinning conjunct remains in place (§3). Everything else treats a unit as
an opaque leaf — the census does
not descend, a unit is never a disjunctive conjunct (`EXPAND` reads no members out of one), and settledness
(§6) is judged with units opaque. The one whole-unit rewrite is
§5's dualisation, which flips a unit's quantifiers — temporal ones included
(§3, `NEG`) — in and back out.

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
    // Partition at ENTRY: merge two block variables when some ATOM of `body`
    // mentions both — an atom inside a nested binder included: the walk
    // descends through every formula node, units and all, so the partition
    // is the atom connectivity of the whole body. A REFERENCE merges
    // nothing: a block variable inside one is frozen by every method (§7),
    // never resolved, so a reference mentioning two components straddles
    // them harmlessly. Components then share no atom, push sequentially
    // without interference, may reuse ranks 1..|P|, and are TYPE-HOMOGENEOUS
    // (inv. 2), which gives ELIMINATE_BLOCK one type to dispatch on.
    // Connectivity only decays during the push — PUSH_OVER_CONJUNCTION
    // narrows scopes on the way.
    for P in CONNECTED_COMPONENTS(X, body):       // each keeps X's order;
                                                  //   0-based: P[0] outermost
        ctx.type  ← the BA type of P
        ctx.order ← { P[i] ↦ |P|-i }              // inner → LOWER,  ranks 1..|P|
        ctx.prio  ← { P[i] ↦ i+1 }                // inner → HIGHER, ranks 1..|P|
        ctx.keep_functional ← kf(REWRAP(body, X)) // the callback, per block, on
                                                  //   the block's node (§1)
        ctx.subsume_max ← K = 32 ; ctx.qbf_node_max ← K′ = 2²⁰
        ctx.case_max ← K″ = 16 ; ctx.expand_max ← K‴ = 2¹⁴
        ctx.accept_growth ← γ = 16 ; ctx.accept_floor ← 2²⁰
                                                  // constants (§1, ctx table)
        ctx.expand_count ← 0                      // the one component-scoped
                                                  //   counter (§1, ctx table)
        ctx.push_memo, ctx.elim_memo, ctx.cof_memo, ctx.atoms_memo,
            ctx.solver_memo, ctx.qbf_memo
            ← the global tables                   // cross-run, never reset (§1)
        ctx.taint_count ← the global counter      // likewise (§1, cache scope)
        body ← PREPARE_TERMS(body, P, ctx.order)  // BDD-back both sides of every
                                                  //   EQUATION touching P (§1). A
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
        // The component's close: RESOLVE_FUNCTIONAL_BDD first, while the
        //   terms are still BDDs — a whole-block chain over P's own BDD
        //   resolves by one quantification of the stored BDD, a subscript
        //   hidden in a leaf stays (§3) — then FINISH_TERMS, which spells
        //   every remaining backed term, chains intact
        body ← RESOLVE_FUNCTIONAL_BDD(body, ctx.order, kf)
        body ← FINISH_TERMS(body)                 // PREPARE_TERMS' inverse (§3)
    return body
```

**Size acceptance.** One component's push can return a result whose tree
count sits orders beyond its input while the DAG stays small — DETONATION:
cheap to build, ruinous to carry, as the result is raw material for every
later rewrite of this and every enclosing block, and rewrites specialise
shared subtrees apart — a substitution copies per context — so `|·|` bounds
what later passes can be made to touch, where DAG size does not. A
mid-flight tree count far past any threshold can still collapse before the
component closes, and no pre-push feature of the matrix predicts which, so
the test is an acceptance run once on the completed push: both sizes are
§1-cached, the comparison O(1). Rejection discards the result for the
unmodified input re-wrapped over the component (inv. 3), sound because
nothing of the discarded push survives; the P-free conjuncts stay outside
the wrap, and the unit reaches enclosing blocks as an ordinary opaque leaf
(§4), decidable once a query-based method swallows it (§7). Not a taint
event (§1, cache scope): the verdict is a pure function of the completed
pair, and the entries the discarded push wrote remain correct answers to
their own keys.

---

## 6. The push — paper step 2

```
PUSH_BLOCK(φ, X, ctx):                            // memo wrapper
    // KEY CANONICALISATION, two halves of one idea — nothing enters the key
    // that ∃X does not scope:
    X ← X ∩ FV(φ), keeping X's order   // tight downstream quantification. FV
                                       //   includes leaf occurrences (§1)
    if X = ∅: return φ
    if φ = ⋀cᵢ and some cᵢ has FV(cᵢ) ∩ X = ∅:
        // ∃X(indep ∧ dep) = indep ∧ ∃X.dep. What a formula says about FREE
        //   variables never enters the key — expansion cases that differ only
        //   in emitted guard facts converge to ONE state (§6)
        indep ← ⋀ { cᵢ : FV(cᵢ) ∩ X = ∅ }
        return SIMPLIFIED_AND_JOIN(indep, PUSH_BLOCK(⋀ the rest, X, ctx))
    k ← (φ, X, ctx.keep_functional)
                   // the pair (φ, X) IS the wrap node REWRAP(φ, X), which
                   //   carries the ordered X (§1, ctx table).
                   // global: the key names all the entry depends on (§1, cache
                   //   scope), so it suffices across components and runs. No
                   //   in-progress state: a key met mid-computation is an
                   //   unwritten entry — a plain miss, recomputed — and an
                   //   X-preserving cycle passes through a budgeted step
                   //   (§8), whose shared budget caps the depth
    if ctx.push_memo[k] exists: return ctx.push_memo[k]
    t ← ctx.taint_count
    r ← PUSH_BLOCK_UNCACHED(φ, X, ctx)
    if ctx.taint_count = t: ctx.push_memo[k] ← r   // no budget hit inside:
                                                   //   a tainted result is
                                                   //   returned, never cached
                                                   //   (§1, cache scope)
    return r

PUSH_BLOCK_UNCACHED(φ, X, ctx):                   // dispatcher
    if φ is not a conjunction:        // a conjunction tries the fast paths in
                                      //   PUSH_OVER_CONJUNCTION, after scope
                                      //   narrowing
        r ← TRY_FAST_PATHS(φ, X, ctx)                 // 2a / 2b
        if r ≠ ⊥: return r
    if φ is a negative tree (§1): return ELIMINATE_BLOCK(φ, X, ctx)
                             // one conjunct, taken whole by the leaf (§7);
                             //   where 2a applies it already took the tree
                             //   above. Pushing per member (2d) would hand
                             //   the finite method one unit per non-literal
                             //   member instead of one
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

Both end in small clauses handed to §7 — one atom, or the top-level positives
plus one; neither eliminates anything itself. Both classify per X-touching
leaf — a subtree no `x ∈ X` touches is a constant under `∃X` and stays
neutral.

**2a** distributes `∃X` to each negated atom in one pass, X-free parts riding
along: `∃X.(¬(f=0) ∧ (¬(g=0) ∨ c))` becomes `∃_X f ≠ 0 ∧ (∃_X g ≠ 0 ∨ c)`.
Positive literals at the TOP level ride too: with `p = 0` a conjunct, every
negated leaf takes it along as one clause, `∃X.(p=0 ∧ ¬(g=0))`, under a
single `∀_X p = 0` — so a guard literal left by a decomposition or an
exclusive case does not refute the path.
**2b** collapses `⋀(⋁ positives)` by distributing on *terms*, reaching a
one-atom clause without an expansion.

```
SIGN_CENSUS(φ, X, p) → (pos, neg, other, free):
    // One walk over the ∧/∨ structure, classifying per X-touching leaf; p is
    // the number of top-level positive literals the caller tolerates. A
    // maximal X-free subtree — FV(n) ∩ X = ∅, one cached test (§1) — is
    // NEUTRAL: a constant under ∃X, counted once in free, not descended.
    // An X-touching node:
    //     ∧ / ∨                ↦  sum the children's counts
    //     equation             ↦  pos += 1
    //     ¬ over an equation   ↦  neg += 1
    //     anything else        ↦  other += 1, not descended — an order
    //                             atom, a unit (§4), a reference, a temporal
    //                             operator, or ¬ over one of these
    // EARLY ABORT (inv. 8): the walk stops the moment the counts refute 2a
    // and 2b at once — other > 0, or pos > p together with neg > 0 or
    // free > 0. Per state, uncached (§9).

TRY_FAST_PATHS(φ, X, ctx) → formula | ⊥:
    P ← the top-level conjuncts of φ that are positive PURE equation
        literals (§1)         // ∅ unless φ is a conjunction; X-touching by
                              //   PUSH_OVER_CONJUNCTION's wrapper invariant.
                              //   An impure positive stays an ordinary
                              //   positive leaf and refutes 2a
    c ← SIGN_CENSUS(φ, X, |P|)                          // above
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
        // — one atom per component of P, §7 — is emitted ONCE at the top:
        // an X-free clause needs it, every other clause implies it, and §7
        // discharges it once — the library's memos — however
        // many leaves carry P. WHY: a decomposition's positive arm and
        // EXPAND's exclusive cases leave exactly such a top-level literal,
        // which would otherwise refute 2a and force an expansion. The push
        // routes; §7 reasons (inv. 1).
        if not EX_DISTRIBUTES_OVER_NEGATIVES(ctx.type): return ⊥
        return SIMPLIFIED_AND_JOIN(ELIMINATE_BLOCK(⋀P, X, ctx),
                                   DISTRIBUTE_TO_ATOMS(φ minus P, X, ctx, P))
                              // the wrapper narrows X to the clause (§7), so
                              //   this is the consistency check's key
                              //   (below): one elim_memo entry serves both

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
    // < expansion) queues frequent non-deciders ahead of near-certain ones
    // and pays a census per disjunct for a worse order.
    acc ← an empty SIMPLIFIED_OR_JOIN
    for d in stable_sort(dᵢ by |d|):             // cheap disjuncts first, so a T
        insert PUSH_BLOCK(d, X, ctx) into acc    //   short-circuits before anything
        if acc decided T: return T               //   is expanded. The insert is
    return acc's result                          //   the paper's "unit elimination
                                                 //   after each disjunct"
```

### Conjunction — narrow scopes, then dispatch

```
PUSH_OVER_CONJUNCTION(ψ = ⋀cᵢ, X, ctx):
    // Wrapper invariant: every conjunct touches X (PUSH_BLOCK strips the
    // rest), and X ⊆ FV(ψ). A DISJUNCTIVE conjunct is one whose top node is
    // ∨ — NNF (inv. 4), flat ∧-spines, and unit opacity (§4) make top-level
    // kind equal to reachable case structure, so classifying costs one kind
    // test per conjunct.

    // SCOPE NARROWING — one incidence pass, two moves: the push only ever
    // REMOVES connections and disjunct-occurrences, so both conditions are
    // monotone along the walk — narrow here, not only at block entry.
    // SPLIT where variable sets are disjoint: ∃X(ψ₁ ∧ ψ₂) = ∃X₁ψ₁ ∧ ∃X₂ψ₂,
    // Xᵢ = X ∩ FV(ψᵢ) — before the fast paths, so 2b's cross product is
    // paid per part (inv. 8). SETTLE where a variable's scope is final — x
    // in no disjunctive conjunct other than a negative tree (§1), which the
    // leaf takes whole, so no expansion can add x-material:
    // ∃X(A ∧ B) = ∃(X∖Xs)(∃Xs.A ∧ B), A = the conjuncts touching Xs,
    // Xs ∩ FV(B) = ∅ — the overlapping case A(x, y) ∧ B(y) the split cannot
    // reach. Settling is branch-independent work hoisted above the
    // branching — paid once here, not per case at the leaves — and it is
    // what drains expansion states so they merge (§6). A ψ whose ∨-nodes
    // are all negative trees, or none, is the degenerate case Xs = X, A = ψ:
    // the block is pushed home.
    parts, Xs ← INCIDENCE(the conjuncts, X)
    if |parts| > 1:
        acc ← an empty SIMPLIFIED_AND_JOIN
        for p in parts:
            insert PUSH_BLOCK(⋀p, X ∩ FV(p), ctx) into acc
            if acc decided F: return F        // eager F (inv. 7)
        return acc's result                   // each part settles on re-entry
    if Xs ≠ ∅:
        A ← ⋀ { c ∈ conjuncts of ψ : FV(c) ∩ Xs ≠ ∅ }   // plain conjuncts and
                                                        //   negative trees, by
                                                        //   Xs's definition
        r ← ELIMINATE_BLOCK(A, Xs, ctx)     // a SUB-block: under keep_functional
        if r = F: return F                  //   its emission hides the variables
                                            //   of X ∖ Xs it carries (§7
                                            //   DISCHARGE, §1 leaf hazard)
        if Xs = X: return r                                      // pushed home
        return PUSH_BLOCK(SIMPLIFIED_AND_JOIN(r, the conjuncts of ψ not in A),
                          X ∖ Xs, ctx)

    r ← TRY_FAST_PATHS(ψ, X, ctx)      // 2a / 2b, on the narrowed ψ — the
    if r ≠ ⊥: return r                 //   wrapper stripped X-free conjuncts,
                                       //   so 2b's cross product carries
                                       //   nothing it need not

    // CONSISTENCY — before anything multiplies (inv. 8), ask §7 whether
    // the positive equations alone have a common zero in X: their squeeze,
    // discharged per component as ∀_{X_k} F_k = 0, folds to F exactly when
    // some component's meet of the leaves is a nonzero constant — joint
    // over every positive and every block variable of that component,
    // beyond FOLD_DECIDED's one-atom, one-variable probe.
    // Otherwise such a body is multiplied into cases that each die at
    // their own leaf. Routing, not reasoning (inv. 1): only the verdict F
    // is read — a non-F result is the solvability condition the leaves
    // restate more strongly. PURE positives only — anything else would
    // freeze or reach the solver. Memoized under the clause: paid exactly
    // when the positive set changed.
    P ← the positive PURE equation conjuncts of ψ (§1)
    if P ≠ ∅ and ELIMINATE_BLOCK(⋀P, X, ctx) = F: return F

    // Binder-killing steps, cheapest first (inv. 8); EXPAND, which
    // multiplies, is the floor.
    for x in X:
        ψ* ← TRY_WITNESS(x, ψ, ctx)               // a pin at spine level: one
        if ψ* ≠ ⊥:                                //   scan, no copies. On BDD-
            return PUSH_BLOCK(ψ*, X ∖ {x}, ctx)   //   backed conjuncts the
                                                  //   match reads COF on the
                                                  //   settled term (§3): pin
                                                  //   iff usable ∧ f₀ ∪ f₁ = 1,
                                                  //   witness f₁′, residual
                                                  //   p = 0 (§1, cof_memo)
    for x in X:                                    // a case witness beats the
        (branches, R) ← TRY_CASE_WITNESS(x, ψ, ctx)   // decomposition (inv. 8):
        if ⊥: continue                                //   every branch deletes
                                                      //   the binder
        // LAZY EMISSION — 2d's race over §3's rewrite without materialising
        //   the whole distribution: one branch at a time is substituted,
        //   SIMPLIFIED (inv. 6) and pushed, smallest first, and a branch
        //   that decides T leaves every later copy unbuilt — the same
        //   laziness the decomposition's arm order buys.
        acc ← an empty SIMPLIFIED_OR_JOIN
        for (dᵢ, tᵢ) in branches, smallest |dᵢ| first:
            bᵢ ← SIMPLIFY((dᵢ ∧ R)[x ← tᵢ])          // dᵢ's pin → its residual
            insert PUSH_BLOCK(bᵢ, (X ∖ {x}) ∩ FV(bᵢ), ctx) into acc
            if acc decided T: return T
        return acc's result
    r ← TRY_DECOMPOSE(ψ, X, ctx)       // one LICENSED Boole decomposition on
    if r ≠ ⊥: return r                 //   a census atom (§6): separates the
                                       //   worlds EXPAND would multiply
    return EXPAND(ψ, X, ctx)

PARTS(conjuncts, X) → parts:
    // ONE pass over cached FV sets — an intersection per conjunct, no
    // descent (§1): union-find merging block variables that co-occur in a
    // conjunct; parts = the conjuncts grouped by component. Near-linear.
    // The grouping alone is what §7 shares — the freeze, the squeeze's
    // components, the finite tier-2 split.

INCIDENCE(conjuncts, X) → (parts, Xs):
    // PARTS plus one kind test per conjunct: a variable flagged on touching
    // a disjunctive conjunct that is not a negative tree (§1, one cached
    // leaf test) → Xs, the unflagged remainder.
```

### The licensed decomposition, then expansion — paper steps 2e–2k

Below the case-witness step two mechanisms remain. One licensed
decomposition is the paper's 2e–2j for atoms no step above can take, under
a no-duplication test on its built cofactors; when its atom is a PIN — an
equation whose truth substitutes a binder variable away — the T-arm deletes
that binder (`DECOMPOSE_ARMS`). `TRY_DECOMPOSE` takes the atom that
SEPARATES sibling conjuncts, judged in tree size over the conjuncts it
touches. `EXPAND` is the floor: it distributes `∃X` over ONE
disjunctive conjunct the formula already contains — in EXCLUSIVE cases, a
built case's complement conjoined into the cases after it wherever that
complement holds no disjunction of positives — and hands
every case back to `PUSH_BLOCK`, which runs the full ladder on each and
re-enters here for the next disjunct. A case's state is an ordinary push
key, so SHARING is `push_memo` firing: two cases that converge (same
surviving dependent conjuncts, same pending disjuncts) are one key, solved
once. Everything is arranged for that convergence: children assemble in the
content order (§3, joins), X-free material leaves the key at birth
(`PUSH_BLOCK`) — which also keeps a decomposition's guard residue out of
sub-push keys — the witness steps and the pin arm substitute pins out, and
the settle move drains finished variables. Merging is best-effort — a missed
merge costs time, never soundness. Pending disjuncts are never specialised
with a case's free facts (the fact's conjunct stays in place, as in §4's
unit skip): branch-local rewriting of shared material would fragment the
sharing.

```
COF(f, x, ctx) → (f₀, f₁, p, usable, pin):       // the cof_memo recipe — 2f's
    on a ctx.cof_memo[(f, x)] miss, compute and record:        //   test
        f₀ ← SIMPLIFY_TERM(f[x←0]) ; f₁ ← SIMPLIFY_TERM(f[x←1])
        usable ← (x ∉ FV(f₀) ∪ FV(f₁))           // §1 leaf hazard
        p ← usable ? SIMPLIFY_TERM(f₀·f₁) : ⊥    // f has a zero iff p = 0
        pin ← usable ∧ SIMPLIFY_TERM(f₀ ∪ f₁) = 1   // the zeros of f fix x:
                                                 //   witness f₁′, residual
                                                 //   p = 0 (TRY_WITNESS,
                                                 //   §3); STRICT iff p = 0
    return ctx.cof_memo[(f, x)]

FOLD_DECIDED(C, X, ctx) → conjunct list | F:
    // f = 0 forces f₀f₁ = 0 on EVERY variable, so p a nonzero constant proves
    // the atom F outright — a fold SIMPLIFY_ATOM cannot see (it decides an
    // atom whose ring sum is a constant; here the PRODUCT of two cofactors
    // is). One memoized probe per (¬)equation; a negative
    // tree conjunct (§1) is read literal-wise.
    for each (¬)equation conjunct c of C, and each literal c of a negative
            tree conjunct t of C:
        f ← TERM_OF(c)
        x̂ ← the highest-prio variable of FV(f) ∩ X ; if none: continue
        (f₀, f₁, p, usable, _) ← COF(f, x̂, ctx)
        if usable and p is a nonzero constant:
            if c is positive: return F           // the atom is F; the case dies
            if c is a conjunct: drop c from C    // ¬F = T rides out
            else: t ← t[c ↦ T] through the joins // inside a tree T folds up;
                                                 //   a tree folded to T is
                                                 //   dropped
    return C
```

```
TRY_DECOMPOSE(ψ, X, ctx) → formula | ⊥:
    // Reached only past the case-witness step: what remains is case structure
    // SPANNING conjuncts — an atom's two signs at member tops of two
    // DIFFERENT disjunctive conjuncts — a shape no step above takes flat.
    // One Boole decomposition on such an atom separates the worlds:
    //     ∃X ψ  =  ∃X (a ∧ ψ[a ↦ T])  ∨  ∃X (¬a ∧ ψ[a ↦ F])      — any BA
    // Exact for ANY atom of ψ (inv. 5); arms by DECOMPOSE_ARMS below, a pin
    // atom's T-arm substituting its binder away. Both cofactors erase every
    // occurrence and an arm keeps at most one ONE-SIGNED guard literal, so
    // the census below (both signs required) never re-selects the atom
    // (inv. 5).
    if ctx.expand_count ≥ ctx.expand_max: return ⊥    // spent: EXPAND re-wraps
                                                      //   and taints

    // CENSUS, rank order COVER: one pass over the member tops of ψ's
    // disjunctive conjuncts (equation and order atoms; a negative tree's
    // members count like any other), counting signed
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
    // PROBE FIRST: a candidate FOLD_DECIDED proves F is folded, not
    // decomposed — every occurrence erased, no guard, no arm, no budget —
    // and the smaller body re-enters the ladder (inv. 8). One memoized COF
    // lookup per candidate.
    for a in cands:
        if FOLD_DECIDED([a], X, ctx) = F:
            b ← SIMPLIFY(ψ[a ↦ F]) ; return PUSH_BLOCK(b, X ∩ FV(b), ctx)
    a ← first of cands passing ; if none: return ⊥
    return DECOMPOSE_ARMS(a, ψ[a ↦ T], ψ[a ↦ F], X, ctx)
                                  // the touched conjuncts' cofactors are the
                                  //   licence's, already built

DECOMPOSE_ARMS(a, C_T, C_F, X, ctx):
    // The two arms of ∃X ψ = ∃X (a ∧ C_T) ∨ ∃X (¬a ∧ C_F).
    // PIN ARM: when a is an equation whose settled term f PINS a block
    // variable — COF at x gives pin, the witness step's own test on the
    // same key (§3) — then under a,
    // x = f₁′ uniquely, and the T-arm is ∃(X ∖ {x}) (a ∧ C_T)[x ← f₁′]:
    // the same worlds with one binder fewer, the guard kept and substituted
    // into its residual p = 0 (TRY_WITNESS, §3), T for a strict pin. This
    // is the paper's 2g at whatever depth the census found the atom — a
    // spine-level pin was the witness step's, a disjunct pinning on every
    // branch the case witness's; a nested or partial pin, or a case pin
    // wider than case_max, arrives here. Several pinned block variables:
    // the innermost (ctx.prio). The substitution is unlicensed, like every
    // binder-killing substitution: the arms were licensed in guard form,
    // and the deleted binder repays the copy. Occurrences inside units stay
    // sound unreplaced: a has one truth value per world, and x ← f₁′
    // resolves a unit-internal copy by itself; a rebound x is left alone
    // (capture-aware substitution, §10).
    // An X-free guard hoists through PUSH_BLOCK's strip; an X-containing
    // one rides as a dependent conjunct. Arms share EXPAND's budget (§1,
    // cache scope); the caller exits on a spent budget, so the first arm
    // always fits and exhaustion can only re-wrap the second (inv. 3).
    // Smaller arm first — ∃'s T short-circuit never builds the second; an
    // arm that folds to a constant is no case and charges nothing.
    f ← a is an equation ? TERM_OF(a) : ⊥
                                                  // cof_memo's key (§1), formed
                                                  //   by the caller at every
                                                  //   pin site
    x ← f ≠ ⊥ ? the innermost x ∈ X ∩ FV(f) with COF(f, x, ctx).pin : ⊥
                                                  // the pin, if any
    arm_T ← x = ⊥ ? (a ∧ C_T, X) : ((a ∧ C_T)[x ← f₁′], X ∖ {x})
                                                  // binder deleted; a → p = 0
    arm_F ← (¬a ∧ C_F, X)
    acc ← an empty SIMPLIFIED_OR_JOIN
    for (φ, Y) in ⟨arm_T, arm_F⟩, smaller |φ| first:
        b ← SIMPLIFY(φ)                                            // inv. 6
        if b ∈ {T, F}: insert b into acc
        else if ctx.expand_count ≥ ctx.expand_max:
            ctx.taint_count ← ctx.taint_count + 1          // §1, cache scope
            insert REWRAP(b, Y ∩ FV(b)) into acc ; break
        else:
            ctx.expand_count ← ctx.expand_count + 1
            insert PUSH_BLOCK(b, Y ∩ FV(b), ctx) into acc
        if acc decided T: break
    return acc's result
```

```
EXPAND(ψ, X, ctx):
    // Reached only when every step above failed: ψ connected, mixed census,
    // no spine pin, no case pin, no atom licensed to separate, no settled
    // variable.
    //     ∃X(E ∧ (⋁ⱼ dⱼ) ∧ S′)  =  ⋁ⱼ ∃X(E ∧ exclⱼ ∧ dⱼ ∧ S′)      — any BA
    // with exclⱼ implied complements of the BUILT cases before dⱼ (a dead
    // case's complement is implied too).
    // EXCLUSIVE CASES: members are not mutually exclusive, so distributing
    // over them as they stand builds CONSENSUS cases — on (¬g ∨ ¬P) ∧
    // (g ∨ ¬Q) the cases ¬g ∧ ¬Q and ¬P ∧ g, plus the redundant ¬P ∧ ¬Q —
    // and each level's negation hands the next level a wider image to
    // expand. Shannon on a literal member makes the cases disjoint,
    //     ∃X(E ∧ (ℓ ∨ R) ∧ S′)  =  ∃X(E ∧ ℓ ∧ S′)  ∨  ∃X(E ∧ ¬ℓ ∧ R ∧ S′),
    // and the conjoined complement folds the sibling disjunct by unit
    // elimination. A complement is CONJOINABLE when it holds no disjunction
    // with a positive member — every ∨-node in it is a negative tree (§1):
    // a literal's complement (a literal), a conjunction of positives'
    // (a flat negative tree), a negative tree's (a conjunction of
    // positives), and their conjunctions. A complement holding a
    // disjunction of positives would ADD positive case structure and is
    // skipped — its case is distributed without an exclusion of its own,
    // still exclusive from every case whose complement was conjoined, so
    // the identity stays exact.
    // Any subset of the implied complements is sound. A positive
    // complement is a top-level positive the fast path 2a tolerates; a
    // negative-tree complement is a case fact the cases below may split on.
    // A NEGATIVE TREE (§1) is distributed over like any disjunct: the
    // cases after a negative member carry its complement, a POSITIVE, and
    // positives are what the cheap steps consume — a pin for the witness
    // step, an atom shared with sibling disjuncts for unit elimination and
    // absorption. Taking the tree whole would withhold them (§9).
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
            ctx.taint_count ← ctx.taint_count + 1
            φp ← SIMPLIFIED_AND_JOIN(⋀E, excl, ⋁ of the remaining members,
                                     ⋀(S ∖ {D}))
            insert REWRAP(φp, X ∩ FV(φp)) into acc
            break
        ctx.expand_count ← ctx.expand_count + 1
        dc ← FOLD_DECIDED(the conjuncts of d, X, ctx)   // a nested tree among
                                                       //   them: literal-wise
        if dc = F: continue                  // dead case: no copy built, and
                                             //   no exclusion — it is implied
        if dc = []:                          // a member proven T makes D T:
            return PUSH_BLOCK(SIMPLIFIED_AND_JOIN(⋀E, ⋀(S ∖ {D})), X, ctx)
                                             //   ψ is E ∧ S′ — one push, no
                                             //   cases, the built ones subsumed
        c ← SIMPLIFY(⋀E ∧ excl ∧ ⋀dc)   // DEEP, but over E ∪ excl ∪ d alone —
                                  //   the pending disjuncts S∖{D} are NEVER
                                  //   re-simplified: normalization is paid
                                  //   where material meets, not per case
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
        e ← NORMALIZE_OPERATORS(NEG(d))      // the cases after d exclude its
        if e is conjoinable: insert e into excl  // worlds — an equation's ¬,
                                             //   an order atom's mirror (§3)
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
    X ← X ∩ FV(clause), keeping X's order  // PUSH_BLOCK's key canonicalisation:
                                           //   nothing enters the key that ∃X
                                           //   does not scope — 2a's leaf
                                           //   clauses and 2b's terms arrive
                                           //   under the whole block
    if X = ∅: return clause
    k ← (clause, X, ctx.keep_functional)   // exact key (ctx table): the pair
                      //   is the wrap node REWRAP(clause, X), which carries
                      //   the ordered X; nothing below recurses back into the
                      //   push
    if ctx.elim_memo[k] exists: return ctx.elim_memo[k]
    t ← ctx.taint_count
    r ← ELIMINATE_BLOCK_UNCACHED(clause, X, ctx)
    if ctx.taint_count = t: ctx.elim_memo[k] ← r   // no budget hit inside:
                                                   //   a tainted result is
                                                   //   returned, never cached
                                                   //   (§1, cache scope)
    return r

ELIMINATE_BLOCK_UNCACHED(clause, X, ctx):
    // Type-agnostic pre-steps — pure identities, run once here, not per method.
 1. for x in X:                                // a witness beats any method:
        if clause ∈ {T, F}: break              //   cheaper and exact (inv. 8)
        ψ* ← TRY_WITNESS(x, clause, ctx)
        if ψ* ≠ ⊥: clause ← ψ* ; X ← X ∖ {x}   // ⊥ leaves x and clause untouched
    if clause ∈ {T, F}: return clause
 2. // The witness pass substitutes into every conjunct, the pin included,
    // so a conjunct can lose its last block variable — the pin's residual
    // p = 0, or a sibling. STRIP those, PUSH_BLOCK's own move,
    // ∃X(indep ∧ dep) = indep ∧ ∃X.dep, one cached FV test per conjunct:
    // the method contract (every conjunct mentions some x ∈ X) then holds.
    indep ← ⋀ { c ∈ conjuncts of clause : FV(c) ∩ X = ∅ }
    clause ← ⋀ the rest
    drop from X every variable not occurring in clause
    if X = ∅: return SIMPLIFIED_AND_JOIN(indep, clause)
 3. M ← METHOD(ctx.type)
    if M = ⊥: return SIMPLIFIED_AND_JOIN(indep, REWRAP(clause, X))
                                               // no method for τ: undecided (inv. 3)
    return SIMPLIFIED_AND_JOIN(indep, M(clause, X, ctx))
```

The type table — adding a type is one row plus one method obeying the contract:

| `τ` | `METHOD(τ)` | `EX_DISTRIBUTES_OVER_NEGATIVES(τ)` |
|---|---|---|
| atomless | `ELIMINATE_ATOMLESS_CLAUSE` | **true** — `cor:Multivariate-BFs-over` |
| bitvector | `ELIMINATE_BITVECTOR_CLAUSE` — a router: pure Boolean — linear arithmetic converts in — ↦ `ELIMINATE_FINITE_CLAUSE`, residual arithmetic ↦ solver | **false** — `bv[1]`: `∃x(x≠0 ∧ x′≠0)` is F |
| anything else | `⊥` → re-wrap | **false** — fail closed |

**Method contract.**

- **Given:** a single conjunctive clause — a negative tree (§1) is one
  conjunct of it; `X` non-empty; every `x ∈ X` occurs in the clause; every
  conjunct mentions some `x ∈ X` (`ELIMINATE_BLOCK`'s narrowing and strip
  guarantee both); `X` homogeneous of type `τ`.
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
    parts ← PARTS(the conjuncts of clause, X)                          // §6
            // every conjunct touches some x ∈ X, so the grouping is total
    O  ← ⋃ { p ∈ parts : some conjunct of p is opaque? }
    if O = ∅: return (T, clause, X)
    Xf ← X ∩ FV(⋀O), keeping X's order
    return (REWRAP(⋀O, Xf),                       // inv. 3
            clause without O's conjuncts, X ∖ Xf)
```

### Shared helpers

Shared by the atomless and finite methods — both engines' mathematics is
any-BA up to this point. `SQUEEZE` normalises the clause, is the only
place an equation is rewritten (invariant 4), and groups the positives into
COMPONENTS by block-variable incidence among themselves — negatives ignored
— the unit every condition below is computed over. `SUBSUME_NEGATIVES`
prunes the negatives and is the finite method's step alone — its comment
carries the why. `POSITIVE_CONDITION` emits `pos`, one atom per component;
`NEGATIVE_CONDITION` emits the exact condition for ONE negative literal
against the components it touches, and `TREE_CONDITION` assembles those in
a tree's own ∧/∨ structure; conjoining one per negative conjunct is the
atomless method's licence alone (inv. 1).

```
SQUEEZE(clause, X) → (f, comps, negatives, clause):
 1. clause ← NORM_EQUATION applied to every (¬)equation, the literals of a
    negative tree included
    //   zero form for the squeeze; "positives"/"negatives" mean f = 0 / ¬(f = 0).
    //   A swallowed binder unit or a conversion emission (finite method) is
    //   not an atom — it rides along untouched
 2. f ← ⋃ { positives of clause }                        // squeeze; ⋃{} = 0
    comps ← [(F_k, X_k)]: PARTS (§6) over the POSITIVES alone, negatives
            ignored — F_k the union of part k's terms, X_k = X ∩ FV(F_k),
            so f = ⋃_k F_k over pairwise disjoint X_k. A positive whose zero
            form lost its last block variable (x + b = x + c gives b + c = 0)
            is a part of its own over NO variable: PARTS tolerates it, and
            DISCHARGE over the empty block emits the atom as it is
    //   The push's own incidence pass counts negatives as connections, so a
    //   clause whose positives are linked only through negative material
    //   — the shape a negative tree (§1) brings to the leaf whole — arrives
    //   as one component; this grouping is finer, and it is the unit the
    //   conditions below are exact over: quantifiers over disjoint
    //   variable sets distribute over ∪ and ·, so nothing of one component
    //   ever enters another's term.
    return (f, comps, the NEGATIVE CONJUNCTS of clause — literals and trees,
            clause)

SUBSUME_NEGATIVES(f, clause, X, ctx) → (negatives, clause, X):  // finite only
    // O3, term-order subsumption. Only the BA order relates two atoms —
    //   propositional simplification cannot see it. Tested modulo the
    //   positive part as f′gᵢgⱼ′ = 0, written gᵢ ⊑ gⱼ: under f = 0, f′ = 1
    //   and the test collapses to gᵢ ≤ gⱼ. Dropping costs no precision
    //   (∃_X f′gᵢ ≤ ∃_X f′gⱼ). Finite-only: the quadratic meets are repaid
    //   only where a surviving negative fattens a QUERY — the tier-2 blast,
    //   a re-wrap in converted form. The atomless per-negative saving is
    //   one memoized DISCHARGE, which does not repay them, so that method
    //   squeezes without subsuming, at a small result-size price; if size
    //   ever binds, the scan can return there behind a low threshold. BDD
    //   size is not monotone under ≤ — no cheap prescreen; ctx.subsume_max
    //   gates the scan, counted over the literals it reads.
    // Three rules, one primitive. In the CONJUNCTION of single negatives,
    //   gᵢ ⊑ gⱼ makes ¬(gⱼ = 0) redundant — the larger goes. Inside a FLAT
    //   tree (§1) — a disjunction — the direction flips: gᵢ ⊑ gⱼ makes
    //   ¬(gᵢ = 0) redundant, the smaller goes. Across: a single below some
    //   member of a tree implies the tree, which goes; a single above every
    //   member of a tree is implied by it and goes; a tree G₂ goes when
    //   every member of another tree G₁ is below some member of G₂. A
    //   NESTED tree takes part in none of this.
    if the literal count over the singles and the flat trees ≤ ctx.subsume_max:
        for each flat tree G:
            drop each member gᵢ with gᵢ ⊑ gⱼ for another member gⱼ of G
                // survivors only, in order: of an equivalent pair the first
                //   is kept
        keep ← []
        for each single negative ¬(gⱼ = 0) of clause, in order:
            // survivors only: mutually subsuming negatives are equal — testing
            //   against all would drop both
            if no gᵢ in keep has gᵢ ⊑ gⱼ: keep += gⱼ
        drop each flat tree with a member above some g ∈ keep
        drop each g ∈ keep above every member of some flat tree
        drop each flat tree G₂ with every member of another flat tree G₁
            below some member of G₂         // survivors only, in order
        clause ← clause with the dropped negatives removed
    drop from X every variable not occurring in clause
    //   subsumption can remove a variable's last occurrence; such a variable
    //   sat only in dropped negatives, never in f
    return (the negative conjuncts of clause, clause, X)
```

```
POSITIVE_CONDITION(comps, ctx) → formula:
    // pos, per component. The meet distributes over a union of terms with
    // disjoint decision supports:
    //     ∀_X f = ⋃_k ∀_{X_k} F_k       — any BA
    // so ∀_X f = 0 is one atom per component, each discharged over its own
    // variables on its own term and memoized under its own key (the
    // library's memos, §1): a clause that changed one component's positives
    // keeps every other component's entry, and the atoms emitted are the
    // smallest the identity allows — the shape unit elimination and the
    // pin tests consume. Exact by thm:boole-const per component. pos holds
    // even where it folds to T (no positives ⇒ f = 0 vacuously).
    acc ← an empty SIMPLIFIED_AND_JOIN
    for (F_k, X_k) in comps:
        insert DISCHARGE(∃X_k. F_k = 0, ctx) into acc           // ⇒ ∀_{X_k} F_k = 0
        if acc decided F: break
    return acc's result

NEGATIVE_CONDITION(g, comps, X, ctx) → formula:
    // The condition for ONE negative ¬(g = 0), modulo the sibling pos:
    // exact in ANY BA — the tier-1 lemma (Finite section). It absorbs
    // against the components g TOUCHES only: with f = ⋃_k F_k over
    // disjoint X_k, K the components sharing a block variable with g,
    // F_K = ⋃_{k∈K} F_k and X_g = X ∩ FV(g),
    //     ∃_X f′·g  =  ∃_{X_K ∪ X_g}(F_K′·g) · ∏_{k∉K} (∀_{X_k} F_k)′
    // and under pos every factor of the product is 1. The rest of the
    // clause never enters the absorption term — what kept it growing.
    // The shortcuts rest on:  ∃_X f′ = (∀_X f)′,  and  ∃_X(f′·g) = g · ∃_X f′
    // for g independent of X.
    if g ≡ 1: return T               // O1: ∃_X f′g = (∀_X f)′ = 0′ ≠ 0, by pos
    K ← { k : X_k ∩ FV(g) ≠ ∅ } ; X_g ← X ∩ FV(g)
    if K = ∅:                        // O2: no component touches g — every
        return DISCHARGE(∃X_g. g ≠ 0, ctx)   //   factor is 1 under pos. An
                                     //   X-free g lands here with X_g = ∅ and
                                     //   emits g ≠ 0 itself: under §1's term
                                     //   representation a BDD-backed g is
                                     //   canonical over P, so "g independent
                                     //   of X" IS FV(g) ∩ X = ∅, and no
                                     //   cofactor test is needed for it
    return DISCHARGE(∃(X_K ∪ X_g). F_K′·g ≠ 0, ctx)      // ⇒ ∃_{X_K ∪ X_g} F_K′g ≠ 0

TREE_CONDITION(t, comps, X, ctx) → formula:
    // The condition for a negative tree conjunct t (§1), modulo pos: t with
    // every literal ¬(g = 0) replaced by NEGATIVE_CONDITION(g, …), its ∨
    // nodes assembled by SIMPLIFIED_OR_JOIN and its ∧ nodes by
    // SIMPLIFIED_AND_JOIN, bottom-up, memoized per node within this call —
    // comps and X are fixed here, so a memo local to the call is exact, and
    // a tree's repeated subtrees and literals pay once. Exact wherever
    // one condition per DNF clause of t is (the atomless method): distribute
    // ∃X over t's disjunctions, apply the clause identity to each DNF
    // clause — the same positive part throughout — and refactor, sound
    // because each condition depends on its own literal and the positives
    // alone, and pos is the one conjunct every DNF clause shares, so a
    // condition exact modulo pos stays exact inside the joins. The
    // tree is never flattened: the DNF is the proof, not the computation.
    // The result is a negative tree over the free variables — conditions
    // are negated equations — plain for the enclosing block.
    return t[¬(g = 0) ↦ NEGATIVE_CONDITION(g, comps, X, ctx)]
```

### Atomless

```
ELIMINATE_ATOMLESS_CLAUSE(clause, X, ctx):
    //  ∃X.( f=0 ∧ ⋀ⱼ gⱼ≠0 )  ≡  ∀_X f=0 ∧ ⋀ⱼ ∃_X f′·gⱼ ≠ 0     (cor:qelim-main)
    //  — and a negative tree conjunct becomes its TREE_CONDITION.

 1. // opaque? = not a (¬)equation, a negative tree read literal-wise (opaque
    // iff a literal is): a reference, an order atom, and EVERY binder
    // unit — this engine answers no queries, so it can swallow none. ALSO
    // opaque: a (¬)equation whose term hides a block variable in a leaf,
    // LEAF_FV(t) ∩ X ≠ ∅ (§3) — quantification is cofactoring (§1 leaf
    // hazard), so DISCHARGE would emit that x free.
    frozen, clause, X ← FREEZE_OPAQUE_COMPONENTS(clause, X, opaque?)
    if X = ∅: return frozen
 2. f, comps, negatives, clause ← SQUEEZE(clause, X)
    // O1 and O2 reason under pos as a sibling assumption.
    pos ← POSITIVE_CONDITION(comps, ctx)               // ⇒ ⋀_k ∀_{X_k} F_k = 0
    if pos = F: return F        // some component has no common zero: ∃X.clause is F
    // ONE condition PER negative is the OTHER use of atomlessness (inv. 1);
    // each condition alone is any-BA-exact (NEGATIVE_CONDITION).
    neg ← an empty SIMPLIFIED_AND_JOIN
    for n in negatives:                                // literals and trees
        insert (n a tree ? TREE_CONDITION(n, comps, X, ctx)
                : NEGATIVE_CONDITION(TERM_OF(n), comps, X, ctx)) into neg
        if neg decided F: return F   // one unsatisfiable negative decides
    return SIMPLIFIED_AND_JOIN(frozen, pos, neg's result)

    // No positives ⇒ comps = ∅ ⇒ O2 fires for every negative: the result
    // ⋀ⱼ ∃_{X_gⱼ} gⱼ ≠ 0 is the clause-level instance of 2a.
```

```
DISCHARGE(∃X.atom, ctx):
    // ∃X. f = 0  ⇒  ∀_X f = 0            ∃X. h ≠ 0  ⇒  ∃_X h ≠ 0
    // No memo of its own (§1, ctx table): keep mode emits the chain, non-keep
    // mode is one BDD quantification covered by the library's memos.
    if ctx.keep_functional:
        emit SIMPLIFY_ATOM of the functional-quantifier atom — the chain
        built by the term constructor (§1: absent subscripts dropped,
        same-kind segments merged, a chain with no free variable but its
        own folded to its constant). A WHOLE-BLOCK emission is X-free and
        keeps its BDD body until the component's close, where
        RESOLVE_FUNCTIONAL_BDD is one quantification of the stored BDD and
        the finish spells what is kept; a settled SUB-block's emission (§6)
        keeps its BDD body the same way, and the block variables that body
        still carries are HIDDEN from the rest of the push (§1 leaf hazard):
        the atom is opaque to the methods and unusable to COF, and re-wraps
        with its component. No slide, no reorder: the push-in stops at that
        atom, by choice.
    else: one BDD quantification over ctx.order; emit SIMPLIFY_ATOM of the
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
the SAME free variables. An emission is a non-literal, non-unit conjunct the
push never builds (the negative tree, §1, is the other such shape, read by
§7's tree rules), so downstream recognises it by shape alone: PURE for the
split, riding through the squeeze untouched, forcing tier 2 — carries couple
bits, so an emission IS the k ≥ 2 competition. The price: an undecided
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
    // terms, a binder unit whose body is not bv-translatable (a reference, a
    // foreign type or a functional quantifier inside), or an IMPURE conjunct
    // carrying a functional quantifier — a chain reaching this point is a
    // kept or a stuck one (§3), and the solver has no term-level binder.
    // No query below resolves anything: a PURE conjunct carrying a chain is
    // the finite side's (step 3), and the solver path never sees one.
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
 3. // The split. PURE = a pure (¬)equation (§1 — functional-quantifier
    // terms count, see above), a negative tree of pure literals (§1), a
    // conversion emission (step 2), or a binder unit whose body is
    // recursively pure; everything else — an
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
    // carries a functional quantifier — a kept or a stuck chain (§3), and
    // the solver has no term-level binder.
    if a is a surviving binder and b ∉ {T, F} and FV(a) ∩ FV(b) ≠ ∅
       and b carries no functional quantifier:
        if ASK(∃Z.(a ∧ b), ctx) = unsat: return F
 7. return SIMPLIFIED_AND_JOIN(frozen, b, a)
```

```
SOLVE_ARITHMETIC(clause, X, ctx):                     // the solver path
 1. q ← ∃X.clause     // no functional quantifier is present: step 1 froze
                      //   every impure conjunct carrying one and step 3 sent
                      //   every pure one to the finite side; one here is a
                      //   caller mistake, asserted.
                      // binder units enter q as written — the solver accepts
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
A FLAT negative tree (§1) under the squeeze is tier-1 material too: `∃X`
distributes over its ∨, every disjunct is a one-negative clause, and `pos`
factors out — `pos ∧ ⋁ᵢ cond(gᵢ)`. A NESTED tree's tier-1 material is
exactly its root's literal members: a DNF clause picks one member at every
∨ and all members at every ∧, so it holds one literal iff it is a literal
member at the root. Every other DNF clause holds two or more negatives, and
so does every DNF clause of two or more negative conjuncts — flattening
could never turn tier-2 material into tier-1 material, which is why a tree
is never flattened.

```
ELIMINATE_FINITE_CLAUSE(clause, X, ctx):
 1. // opaque? = the atomless step-1 condition plus anything arithmetic — MINUS
    // binder units with recursively pure bodies and MINUS conversion
    // emissions, both swallowed into the tier-2 query (§4). A no-op when
    // called from the router, whose split already guarantees purity.
    frozen, clause, X ← FREEZE_OPAQUE_COMPONENTS(clause, X, opaque?)
    if X = ∅: return frozen
 2. f, comps, negatives, clause ← SQUEEZE(clause, X)
    negatives, clause, X ← SUBSUME_NEGATIVES(f, clause, X, ctx)
    //   modulo the whole f: the strongest test; a variable subsumption drops
    //   from X sat in no positive, so comps stand
    if X = ∅: return SIMPLIFIED_AND_JOIN(frozen, clause)
 3. pos ← POSITIVE_CONDITION(comps, ctx)  // exact in ANY BA (thm:boole-const
    if pos = F: return F                  //   per component); implied by the
                                          //   clause, so F here decides even
                                          //   before tier 2
 4. if clause holds no binder unit, no conversion emission, and at most one
        negative conjunct, that one a literal or a FLAT tree:
        // Tiers 0–1 — the shared pos/neg emission, exact here by the tier-1
        // lemma (intro above). A binder unit or a conversion emission forces
        // tier 2: the clause does not factor around it.
        if negatives = ∅: return SIMPLIFIED_AND_JOIN(frozen, pos)
        neg ← SIMPLIFIED_OR_JOIN(NEGATIVE_CONDITION(g, comps, X, ctx)
                                 : ¬(g = 0) a literal of the one negative)
        if neg = F: return F
        return SIMPLIFIED_AND_JOIN(frozen, pos, neg)
 4′. if clause holds no binder unit, no conversion emission, and exactly one
        negative conjunct, a NESTED tree t with literal members L ≠ ∅ and
        non-literal members T′ at its root:
        // The middle tier: the root's literal members are the tree's whole
        // tier-1 material (intro above) and are decided exactly; the rest
        // stays ONE query and, undecided, ONE unit. Exact:
        //     ∃X(f=0 ∧ (⋁L ∨ ⋁T′)) = ⋁_L ∃X(f=0 ∧ ℓ) ∨ ∃X(f=0 ∧ ⋁T′),
        // and the second disjunct implies pos.
        r ← an empty SIMPLIFIED_OR_JOIN
        for ¬(g = 0) in L:
            insert NEGATIVE_CONDITION(g, comps, X, ctx) into r
        if r not decided T:
            insert FINITE_TIER2(clause with ⋁T′ in t's place, X, ctx) into r
        return SIMPLIFIED_AND_JOIN(frozen, pos, r's result)
 5. return SIMPLIFIED_AND_JOIN(frozen, pos, FINITE_TIER2(clause, X, ctx))
                                          // pos: the queries below drop the
                                          //   parts it already decides

FINITE_TIER2(clause, X, ctx) → formula:
    // Tier 2 — two or more negatives in every DNF clause (several negative
    // conjuncts, or a nested tree's non-literal members), a swallowed
    // binder unit, or a conversion emission. NP-complete even closed, so
    // the ladder inside the tier is by representation cost (inv. 8): one
    // shared BDD sweep, then the solver (inside DECIDE_FINITE), then
    // surrender — never the 2^k formula expansion. A negative tree blasts
    // as written: BIT_BLAST is compositional.
    // ONE QUERY PER PART. A query needs only what its non-positive
    // material touches: split the clause by PARTS (§6) over ALL its
    // conjuncts into variable-disjoint parts, then
    //     ∃X ⋀_parts = ⋀_parts ∃X_part. part                   — any BA
    // and a part holding positives alone is ∃X_part(⋀F = 0) = its own pos
    // atoms (thm:boole-const), emitted by the caller already — so it is
    // dropped; every other part is one query over its own variables, and
    // an undecided one re-wraps as its own, smaller unit. This is
    // NEGATIVE_CONDITION's absorption rule applied to a query instead of
    // a literal: a nested tree's non-literal rest, or several negative
    // conjuncts, touch only some components, and the untouched ones never
    // enter the blast.
    acc ← an empty SIMPLIFIED_AND_JOIN ; open ← []
    for part in PARTS(the conjuncts of clause, X) holding a
            non-positive conjunct, smallest first:
        r ← FINITE_QUERY(⋀part, X ∩ FV(⋀part), ctx)
        if r is a re-wrap: open += r
        insert r into acc
        if acc decided F: break                          // eager F (inv. 7)
    // JOINT UNSATISFIABILITY — the router's step 6 in kind. ∀Z distributes
    // over ∧, so the per-part closings lose no validity; unsatisfiability
    // does not distribute: two open parts may share free variables that
    // are satisfiable only under incompatible assignments, which the one
    // query over the whole clause caught. One closing over the survivors
    // together restores that precision; a part decided T or F needs none,
    // and survivors sharing no free variable are independent.
    if |open| ≥ 2 and some free variable occurs in two members of open:
        if DECIDE_FINITE(∃Z. ⋀open, ctx) = F: return F       // Z = FV(⋀open)
    return acc's result

FINITE_QUERY(clause, X, ctx) → formula:
    q ← ∃X.clause
    if q is closed:
        r ← DECIDE_FINITE(q, ctx)
        if r ≠ unknown: return r
        return REWRAP(clause, X)                                   // inv. 3
    // Open: close the free variables Z both ways — detection only, the solver
    // path's shape. Sound and still pure: every variable in a pure clause's
    // terms has the block's finite type (inv. 2).
    if DECIDE_FINITE(∀Z.q, ctx) = T: return T                   // q valid
    if DECIDE_FINITE(∃Z.q, ctx) = F: return F                   // q unsat
    return REWRAP(clause, X)
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
    ASK(q, ctx): sat ⇒ return T, unsat ⇒ return F,   // cached in solver_memo
                 unknown ⇒ ctx.taint_count ← ctx.taint_count + 1 ; return unknown
                                                     //   under ITS flush rule;
                                                     //   the budget bound and
                                                     //   nothing decided: a
                                                     //   taint source (§1,
                                                     //   cache scope)
```

---

## 8. Termination

| edge | measure |
|---|---|
| `PUSH_BLOCK` key canonicalisation | the stripped remainder is a proper sub-conjunction, and the strip fires at most once per chain — the remainder holds no X-free conjunct |
| `PUSH_OVER_DISJUNCTION`, `PUSH_OVER_CONJUNCTION` scope narrowing | strict subformula or a strictly smaller block: a disjunct or one part of the split is a proper sub-conjunction; the settle move strictly shrinks `X`; its replacement may hold one positive per component and a negative tree over the remaining variables (§7), conjuncts like any other to the smaller block |
| `TRY_FAST_PATHS` (2a, 2b) | leaves: both go straight to `ELIMINATE_BLOCK` — 2a's as one clause `⋀P ∧ l` per negative leaf plus the one clause `⋀P` — never back into `PUSH_BLOCK` |
| the witness and case-witness steps (§6) | `X` strictly shrinks — the pin or case pin deletes its binder before re-entry; the residual it leaves is free of that binder |
| `TRY_DECOMPOSE`, binary arms (§6) | `|X|` constant — strictly smaller in a pin atom's T-arm, whose binder is substituted away — while both arms lose every occurrence of the atom, and a surviving guard literal is one-signed, invisible to the both-signs census; modulo the same chained channel as `EXPAND`'s caveat below (an arm-edge `SIMPLIFY` can create a pin), the shared `ctx.expand_max` bounds the arm count outright |
| `TRY_DECOMPOSE`'s fold (§6) | a candidate `FOLD_DECIDED` proves `F` is folded, not decomposed: `|X|` constant, every occurrence of the atom erased — strictly fewer atoms, no arm, no budget |
| `EXPAND` | lexicographic (`|X|`, multiset of top-level disjunctive-conjunct sizes), modulo ONE caveat: each case drops `D` and gains only disjunctions lying properly inside ONE member — smaller than `|D|` at selection time — plus CONJOINABLE exclusions — literals, whose unit elimination only shrinks, and negative trees, each the size of the member it complements and so smaller than `\|D\|`; the case-edge `SIMPLIFY` builds no disjunction node, but it can GROW an inherited one: a CHAINED pin — formed inside `d` by a construction-time substitution, whether or not that site's single pass propagated it (§3) — fires here and can push a member past `|D|`; `propagate_growth` caps the growth per `SIMPLIFY` call, and well-foundedness rests on `ctx.expand_max`, which bounds the case count outright |
| `ELIMINATE_BLOCK` pre-steps | `X` shrinks, an X-free conjunct is hoisted, or the clause is decided |
| `ELIMINATE_BITVECTOR_CLAUSE` (router) | one guarded conversion per conjunct — a rewrite, no recursion — then two variable-disjoint sub-clauses, each handed to its engine exactly once; no re-entry into the push |
| swallowed binder units (`ELIMINATE_FINITE_CLAUSE`, `SOLVE_ARITHMETIC`) | a unit is decided wholesale or re-wrapped, never opened; each enclosing block makes exactly one attempt on it (one post-order pass, §4) — no fixpoint across blocks |
| `FINITE_TIER2` | one query per variable-disjoint part of the clause, then at most one joint closing over the surviving re-wraps — rewrites, no recursion; each part decided or re-wrapped exactly once |
| `DECIDE_FINITE` | one syntactic `BIT_BLAST` pass, then one BDD sweep over a finite bit set, budgeted by `ctx.qbf_node_max` |
| `PROCESS_ALL_BLOCKS` | one post-order pass over a finite tree; each run is eliminated once |
| §5 size acceptance | no recursion edge: the test runs on a COMPLETED component push and only replaces its result by a re-wrap (inv. 3) |
| the consistency check (§6) | no recursion edge: one §7 call on the positives, which never re-enters the push; it returns `F` or the push proceeds on the unchanged body |
| negative trees (§1) | no edge: under every rewrite of the push — substitution, propagation, unit elimination, cofactoring — a tree stays a tree or shrinks; only dualisation flips it, at a block boundary |
| `TO_NNF`, factored negation (§3) | `NEG(rests)` recurses on a NEW node, not a subformula — but every residue member lost the shared conjuncts, so `\|rests\|` is strictly below the negated disjunction's |

No elimination method re-enters `ANTI_PRENEX`, so the phases of §3 run once per
call. Every recursion edge above strictly decreases the lexicographic measure
(`|X|`, then the disjunctive-conjunct material) except the one chained
channel — a pin a case-edge or arm-edge `SIMPLIFY` creates — which the
shared `ctx.expand_max` bounds. `push_memo` needs no in-progress state:
a key met mid-computation is an unwritten entry — a plain miss, recomputed —
and any X-preserving cycle passes through `EXPAND` or a decomposition arm
(the strip fires once per chain; 2d and the scope split
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
- **`SIGN_CENSUS` is not cached.** Its result is a pure function of
  `(node, X ∩ FV)` and could be cached per node the way `atoms_memo` is
  (§1) if profiling shows it hot; it aborts the moment 2a and 2b are both
  refuted, so this is not done.
- **The paper's absorb arm (2i/2j) is not implemented.** With `p = 0`
  proven it substitutes the reproductive solution `r(x) = f₀ ∪ x·f₁′`. The
  pin test at every block variable of an atom covers much of its reach, and
  its arm neither deletes a binder nor erases an atom, so it earns no step
  on the ladder. Reopen only if a family walls with many `p = 0` candidates
  and no pin ones.
- **The acceptance dial, and a reject-retry variant.**
  `accept_growth`/`accept_floor` trade output boundedness against decision
  fidelity: tight settings catch every detonation but also discard pushes
  whose mid-flight size would have collapsed before the component closed —
  a formula the looser setting still decides. Deferred: retrying a rejected
  component under a sharply smaller `expand_max`, trading the whole-block
  re-wrap for partial progress with bounded growth; and the one family the
  dial does not clear — matrices dominated by deep definitional binder
  units, opaque to the census and every ladder step — which needs
  a mechanism of its own, not a setting.
- **No second decomposition on the atom cheapest to cut.** Judging an
  atom's two cofactors together in in-memory size (§10), sharing everything
  the atom does not touch, licenses one step, not the chain: each cut is
  cheap and admits the next, so the shared budget ends up paying for the
  chain, and its arms feed the pin arm atoms the census would not have
  chosen. Reopen only with a licence on the chain — a width bound on the
  diagram the steps build.
- **A negative tree is not withheld from `EXPAND`, and a disjunct's
  negative members are not merged into one residual case.** Both were
  tried, on the argument that `∃X` distributes over a disjunction of
  negatives exactly, so it need never be a case. Measured harmful, for one
  reason: the exclusive cases after a negative member carry its complement,
  a positive, and positives are what the cheap steps consume — a pin for
  the witness step, an atom shared with sibling disjuncts for unit
  elimination and absorption. A tree taken whole, or a residual case
  holding the negatives together, withholds every one of them, and the
  cases below it stay alive where the exclusive ones die. The tree keeps
  what costs nothing: finality for scope narrowing, and the leaf.
- **The consistency check runs every time.** Its memo makes an unchanged
  positive set free, so it is paid once per change of the positives —
  linear, alongside the witness scans. Recorded fallback if that overhead
  ever shows: re-check only when the positive count has doubled since the
  last check on the path (a path-scoped counter, saved and restored around
  `PUSH_BLOCK`) — logarithmically many checks per path, at the price of a
  detection delay of up to the count at the last check, during which the
  cases below an inconsistency keep multiplying.

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
  in-memory size (§5).
- **Substitution needs occurrence guards.** `[x ← t]` must return an
  untouched subtree by one cached-FV test; `[atm ↦ T/F]` needs the
  analogous guard — `atoms_memo` (§1), a per-node atom vocabulary with
  exactly the substitution's own reach: ∧, ∨ and ¬ alone, units, references
  and temporal operators opaque (§4). One memo per rewrite, shared across
  every conjunct of the site: the copies overlap.
- **Unit elimination sweeps once.** Inside `SIMPLIFY`, the literal
  conjuncts' truth assignments bind DISTINCT atoms — a complement pair has
  already decided the junction — so they commute: apply them as one
  simultaneous environment in one sweep over the non-literal siblings, not
  one literal at a time with a restart per change.
- **Both passes of `SIMPLIFY` are context-sensitive.** A node's result
  depends on the keys, or pins, in force on its path, so their memos are
  keyed by (node, environment version), and a shared subtree met under two
  environments is two computations: the first bullet's per-node caching
  holds per environment, never across. The sweep is the EXISTING path
  simplifier, spelling-preserving; propagation is its own pass before it.
- **`|φ|` and `FV(φ)` are computed once per node and memoised on it (§1),
  never recomputed** — LAZY: the first query on a node pays for it, every
  later one reads the memo. That is what makes the acceptance comparison, the
  sort keys, and every `X ∩ FV` test in this document O(1).
- **The result tables are caches, never premises.** `push_memo`, `elim_memo`,
  `cof_memo`, `solver_memo` and `qbf_memo` (§1) may be absent
  altogether — a build without caching, the Debug build — so no result may
  depend on a hit: a miss costs time and nothing else. The structural facets
  are not caches and always exist: `|·|`, `neg`, the negative-tree flag, and
  `atoms_memo`.
