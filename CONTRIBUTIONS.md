# Community contributions

Contributions to [IDNI/tau-lang](https://github.com/IDNI/tau-lang) since the start of
the repository, up to 2026-10-14, grouped by quarter with the most recent first. A
contribution is either a pull request merged into `main`, or an issue that was solved
in `main` and whose author went beyond reporting the problem, with a diagnosis of the
cause or a proposed solution.

`main` was rewritten at some point, so GitHub still lists the original SHAs for the
older pull requests and issues. The commit links below point to the matching commits
in today's `main`, found by author and author date.

# Q4 2026

- **Taumorrow** ([@taumorrow](https://github.com/taumorrow)): 2 contributions (1 pull request, 1 issue)

  - **Temporal reasoning: keeping the look-back of a clause.** A condition whose two
    branches are equal was simplified in a way that lost how far back in time its clause
    looks, so Tau started checking it too early. Taumorrow found the cause and sent the
    fix.

    - 2026-10-02 - [#201](https://github.com/IDNI/tau-lang/issues/201) - A conditional with equal branches loses the lookback its clause is written with
      - Diagnostic and proposed solution: the hook folding equal-branch conditionals drops the literal carrying the lookback, and kept_lookback reads the unhooked tree; read it off the body rebuilt through the hooks (PR #202).
      - commits: [9b97331d](https://github.com/IDNI/tau-lang/commit/9b97331dda0d2e8e4323e595f0a809c15942aaeb) Read the lookback a clause keeps off its body rebuilt through the hooks, [0692cc7b](https://github.com/IDNI/tau-lang/commit/0692cc7b109de236df92be763a802fab787c7769) Test that the warm-up pin follows the body the hooks build
    - 2026-10-02 - [#202](https://github.com/IDNI/tau-lang/pull/202) - Read the lookback a clause keeps off its body rebuilt through the hooks
      - commits: [9b97331d](https://github.com/IDNI/tau-lang/commit/9b97331dda0d2e8e4323e595f0a809c15942aaeb) Read the lookback a clause keeps off its body rebuilt through the hooks, [0692cc7b](https://github.com/IDNI/tau-lang/commit/0692cc7b109de236df92be763a802fab787c7769) Test that the warm-up pin follows the body the hooks build

## Numbers

- Merged pull requests: 1
- Issues counted as contributions: 1
- Distinct contributors: 1
- First-time contributors: 0


# Q3 2026

- **Taumorrow** ([@taumorrow](https://github.com/taumorrow)): 30 contributions (9 pull requests, 21 issues)

  - **Interpreter: long runs that accumulate knowledge.** Programs that keep adding
    rules to a growing store could stall, crash on their first step, or get slower at
    every step. Taumorrow found each bottleneck in turn and submitted fixes for several
    of them. The long runs they reported now finish on default settings, with each step
    costing roughly the same.

    - 2026-07-22 - [#70](https://github.com/IDNI/tau-lang/issues/70) - Interpreter (run) path stalls on specs mixing tau and bv streams since ac4aac43
      - Diagnostic and proposed solution: bisected the stall to ac4aac43, explained the mechanism, and proposed restoring the formula-wide bitvector bail-out on the interpreter path (PR #71, closed; the maintainers fixed it differently).
      - commits: [9d3f01da](https://github.com/IDNI/tau-lang/commit/9d3f01da00fa10b1fa49969ecdf7bde47b2615e6) Fix `run` failing on nested conditionals over mixed :tau / :bv[N] streams
    - 2026-08-06 - [#76](https://github.com/IDNI/tau-lang/issues/76) - Interpreter normalization of mixed :tau/bv specs pays the unbudgeted legacy anti-prenex cost (run-based demos stall on main)
      - Diagnostic and proposed solution: bisected the stall to the legacy `anti_prenex` fallback in `resolve_ex_block`, gave a 7-line reproducer, and proposed limiting the wide bail-out to the interpreter path.
      - commits: [fe7d95be](https://github.com/IDNI/tau-lang/commit/fe7d95be02ae28ae132fa0640ea67dd881f3340d) Hash rr<node> by tree content, not by handle address (fixes #80), [3c6dd441](https://github.com/IDNI/tau-lang/commit/3c6dd441e6b7397b8f3b4b9ba51ab480c2bc9ac9) Pin the bitvector-free mixed :tau stream reproducer from #76
    - 2026-08-27 - [#90](https://github.com/IDNI/tau-lang/issues/90) - Interpreter normalization recurses ~24k frames deep on a run that accumulates clauses into a `:tau` constant (SIGSEGV with the default stack)
      - Diagnostic and proposed solution: traced the crash to the recursive `flatten` in `get_leaves` on a DNF spine of about 23,800 nodes, and proposed an iterative flatten or not building the expanded term at all.
      - commits: [b606617d](https://github.com/IDNI/tau-lang/commit/b606617db7f70e1767e3e0679dc84359888ff368) Flatten and/or spines iteratively and without a memo in get_leaves so clause count bounds neither stack depth nor memory, [697cdbbd](https://github.com/IDNI/tau-lang/commit/697cdbbd4bdcda5150be75722ece316567f31e8f) Add a runtime cqe-max-clauses cap and canonicalize atoms after cqe's DNF split, [72372947](https://github.com/IDNI/tau-lang/commit/72372947edd7b1a9fa663fd042f2e57b5a2d096f) Miniscope complete quantifier elimination so only the variable-dependent residue is distributed, [503926b1](https://github.com/IDNI/tau-lang/commit/503926b133e162363797778dd3a8aee734deb83d) Fold the constant time positions when the continuation becomes the final system, [b7c601d3](https://github.com/IDNI/tau-lang/commit/b7c601d398c1f65fcce97d8e606b5481ab3dfb25) Enable the bitvector case split by default, [d4d5a952](https://github.com/IDNI/tau-lang/commit/d4d5a9521ed41a876bc8a1069be40e06b661c700) Keep the trees of decided tau-algebra rows alive across the step sweep, [43b480d4](https://github.com/IDNI/tau-lang/commit/43b480d4a5e6a553f1fa1b0514e1d27ab3e7da34) Decide the tau-algebra predicate per support component by default
    - 2026-08-28 - [#92](https://github.com/IDNI/tau-lang/issues/92) - Accumulating `run`: per-step cost grows ~k^2.8 with the clause count on the default path
      - Diagnostic and proposed solution: identified `get_free_vars` re-walking unchanged subtrees (fixed by #93) and cache entries lost at the garbage-collection sweep, which made the same decisions run again.
      - commits: [48f68978](https://github.com/IDNI/tau-lang/commit/48f6897858355e9dda9107ac36e33eb47a071562) Cache free-variable sets of connective and binder subtrees in get_free_vars, [d4d5a952](https://github.com/IDNI/tau-lang/commit/d4d5a9521ed41a876bc8a1069be40e06b661c700) Keep the trees of decided tau-algebra rows alive across the step sweep, [43b480d4](https://github.com/IDNI/tau-lang/commit/43b480d4a5e6a553f1fa1b0514e1d27ab3e7da34) Decide the tau-algebra predicate per support component by default
    - 2026-08-29 - [#95](https://github.com/IDNI/tau-lang/issues/95) - Accumulating `run`: two costs in normalizing the accumulated `:tau` constant grow with the clause count
      - Diagnostic and proposed solution: attributed the remaining growth to two places (conjuncts collected again at every nested `wff_and`, and temporal-layer normalisation without a memo) and fixed each one in its own PR (#96, #97).
      - commits: [9abbe75f](https://github.com/IDNI/tau-lang/commit/9abbe75ff412a54b66e0e0a250ab876a1ed43e0e) Memoise the temporal-layer normalisation of tau_ba constants per main tree, [d734b308](https://github.com/IDNI/tau-lang/commit/d734b308aa6e778122beaabccab82b93f141d97b) Mark the flattening spine done in syntactic_path_simplification when a conjunction yields no assignment
    - 2026-08-31 - [#93](https://github.com/IDNI/tau-lang/pull/93) - Cache free-variable sets of connective and binder subtrees in `get_free_vars`
      - commits: [48f68978](https://github.com/IDNI/tau-lang/commit/48f6897858355e9dda9107ac36e33eb47a071562) Cache free-variable sets of connective and binder subtrees in get_free_vars
    - 2026-08-31 - [#96](https://github.com/IDNI/tau-lang/pull/96) - Do not re-collect the conjuncts of a conjunction at every nested `wff_and` in `syntactic_path_simplification`
      - commits: [d734b308](https://github.com/IDNI/tau-lang/commit/d734b308aa6e778122beaabccab82b93f141d97b) Mark the flattening spine done in syntactic_path_simplification when a conjunction yields no assignment
    - 2026-08-31 - [#97](https://github.com/IDNI/tau-lang/pull/97) - Memoise the temporal-layer normalisation of `tau_ba` constants per main tree
      - commits: [9abbe75f](https://github.com/IDNI/tau-lang/commit/9abbe75ff412a54b66e0e0a250ab876a1ed43e0e) Memoise the temporal-layer normalisation of tau_ba constants per main tree
    - 2026-09-17 - [#126](https://github.com/IDNI/tau-lang/issues/126) - Each step of a run enumerates one path per conditional whose guard reads a value the same step computes: 2^k paths, each normalized and solved, before the first satisfiable one
      - Diagnostic and proposed solution: interpreter::step enumerates 2^k expression paths because same-step guards stay unfolded; normalize once, substitute top-level o = c constants until fixpoint, then enumerate (PR #127).
      - commits: [2dbb70ba](https://github.com/IDNI/tau-lang/commit/2dbb70ba7484b29be1bc88aed25ff81f930a2f53) Read off constant equalities in the solver and propagate step definitions

  - **Bitvectors: faster and more accurate reasoning about numbers.** Rules that
    compare fixed-size numbers against constants were slow, and in one case gave a
    wrong answer. Taumorrow explained each problem and contributed the fixes: comparisons
    are now kept when simplifying, cases are split on constant values, and simple number
    questions skip an expensive path in the external solver.

    - 2026-09-05 - [#105](https://github.com/IDNI/tau-lang/issues/105) - Existential over a bitvector pinned by order atoms drops a consequent guarded by the pinned value: `ex x (x > 2 && x < 4 && (x = 3 -> a = 0))` normalizes to T
      - Diagnostic and proposed solution: showed that order atoms were not treated as BDD variables, so `clause_to_vector` dropped them, and made them BDD variables (PR #106).
      - commits: [3aaa68d5](https://github.com/IDNI/tau-lang/commit/3aaa68d5584a9b81f8eec9d8c80ccb24b174a17d) Keep the order atoms in the BDD-based DNF/CNF reduction
    - 2026-09-05 - [#107](https://github.com/IDNI/tau-lang/issues/107) - Specs that test a bitvector command against constants lean entirely on the split budget: the guards are skipped by the Boole decomposition and copied into every branch
      - Diagnostic and proposed solution: explained why these specs depend entirely on the split budget, and proposed a bitvector case-split pre-pass (PR #108, later enabled by default).
      - commits: [a70ad0fa](https://github.com/IDNI/tau-lang/commit/a70ad0fa18e08026cdff452442c381a9f105dc66) Add an opt-in test-point elimination for quantified bitvector variables compared only against constants, [f6ee5bd8](https://github.com/IDNI/tau-lang/commit/f6ee5bd8efe0ef8a4e3d28853ca9057999c9e77d) Add an early exit, a runtime cap and options to the bitvector case split, [b7c601d3](https://github.com/IDNI/tau-lang/commit/b7c601d398c1f65fcce97d8e606b5481ab3dfb25) Enable the bitvector case split by default
    - 2026-09-05 - [#109](https://github.com/IDNI/tau-lang/issues/109) - A closed existential bitvector formula is decided through cvc5's quantifier instantiation; the same matrix in QF_BV decides in milliseconds
      - Diagnostic and proposed solution: identified cvc5's quantified bitvector path as the bottleneck, and proposed a quantifier-free decision pre-pass (PR #110).
      - commits: [29fe136e](https://github.com/IDNI/tau-lang/commit/29fe136e8a634bfb0f8c1e066445a38ed1f9dc3d) Add an opt-in quantifier-free decision for closed bitvector formulas with binders of one kind, [52bc1c2a](https://github.com/IDNI/tau-lang/commit/52bc1c2a8e145aa3a4c8e5de41ed388a4b3eea52) Decline the quantifier-free decision for a universal formula with free variables
    - 2026-09-10 - [#106](https://github.com/IDNI/tau-lang/pull/106) - Keep the order atoms in the BDD-based DNF/CNF reduction
      - commits: [3aaa68d5](https://github.com/IDNI/tau-lang/commit/3aaa68d5584a9b81f8eec9d8c80ccb24b174a17d) Keep the order atoms in the BDD-based DNF/CNF reduction
    - 2026-09-10 - [#108](https://github.com/IDNI/tau-lang/pull/108) - Add an opt-in test-point elimination for quantified bitvector variables compared only against constants
      - commits: [a70ad0fa](https://github.com/IDNI/tau-lang/commit/a70ad0fa18e08026cdff452442c381a9f105dc66) Add an opt-in test-point elimination for quantified bitvector variables compared only against constants
    - 2026-09-10 - [#110](https://github.com/IDNI/tau-lang/pull/110) - Add an opt-in quantifier-free decision for closed bitvector formulas with binders of one kind
      - commits: [29fe136e](https://github.com/IDNI/tau-lang/commit/29fe136e8a634bfb0f8c1e066445a38ed1f9dc3d) Add an opt-in quantifier-free decision for closed bitvector formulas with binders of one kind
    - 2026-09-17 - [#121](https://github.com/IDNI/tau-lang/issues/121) - solve on a conjunction of n pure bitvector equalities is exponential in n: 12 equations take 14 s, the same 12 with an arithmetic term 0.09 s (the pure-equality partition goes to lgrs instead of cvc5)
      - Diagnostic and proposed solution: pure bitvector equalities are routed to the lgrs algebraic path (2^n Boole expansion) instead of cvc5; read off var = constant conjuncts first, or cap the lgrs route by variable count.
      - commits: [2dbb70ba](https://github.com/IDNI/tau-lang/commit/2dbb70ba7484b29be1bc88aed25ff81f930a2f53) Read off constant equalities in the solver and propagate step definitions
    - 2026-09-17 - [#122](https://github.com/IDNI/tau-lang/issues/122) - The bitvector case split copies the whole scope per tested variable when a foreign binder sits in between, and its occurrence scan walks the tree
      - Diagnostic and proposed solution: the case split copies the whole scope at foreign binders and scans occurrences as a tree; descend through same-kind binders and add a visited set (PR #123).
      - commits: [24c59d40](https://github.com/IDNI/tau-lang/commit/24c59d4021e6c1c81a1b20744e0db25e6926f103) Descend through same-kind binders in the bitvector case split
    - 2026-09-17 - [#124](https://github.com/IDNI/tau-lang/issues/124) - A bitvector output defined by arithmetic and read by k guarded comparisons initializes exponentially in k: the definitional existentials reach the case split and the solver instead of being eliminated
      - Diagnostic and proposed solution: ex_subs_based_elimination runs too late and misses conditional definitions; run it before the case split and eliminate total (case-wise) bitvector definitions of existentials.
      - commits: [724b09e6](https://github.com/IDNI/tau-lang/commit/724b09e67a7e50d9c05254f7ebb5ded9b6f2eed6) Eliminate definitional bitvector existentials before the case split

  - **Solver: deciding independent clauses separately.** When a formula is made of many
    clauses that share no variables, Tau used to explore every combination of them, so
    its running time doubled with each clause. Taumorrow measured this, found where it
    happens, proposed checking each independent group on its own, and contributed
    benchmarks and part of the fix.

    - 2026-08-05 - [#72](https://github.com/IDNI/tau-lang/issues/72) - Satisfiable one-off `sat` is exponential in the number of disjoint-support clauses (anti-prenex Boole decomposition)
      - Diagnostic and proposed solution: traced the blow-up to the Boole decomposition splitting on every independent clause, and proposed factoring the formula into connected support components first.
      - commits: [f4954c22](https://github.com/IDNI/tau-lang/commit/f4954c2227f28af676876e799a3748bb9b1050fa) Decide variable-disjoint conjuncts per component in anti_prenex_block (fixes #72), [4ab859b4](https://github.com/IDNI/tau-lang/commit/4ab859b4e3a3bdfb92643be92a7e3d87e786890c) Decide is_nso_impl per connected component (fixes #82)
    - 2026-08-05 - [#73](https://github.com/IDNI/tau-lang/pull/73) - Add satisfiability benchmark fixtures: disjoint vs chained clause supports
      - commits: [a83bd5c0](https://github.com/IDNI/tau-lang/commit/a83bd5c074f8704a508bb5163880f298105571eb) Add satisfiability benchmark fixtures: disjoint vs chained clause supports
    - 2026-08-09 - [#82](https://github.com/IDNI/tau-lang/issues/82) - `find_fixpoint_phi`: one implication check dominates, and it scales super-linearly
      - Diagnostic: showed that one `is_nso_impl` call in `find_fixpoint_phi` takes almost all of the time, because it decomposes all the independent atoms of the formula in one piece.
      - commits: [4ab859b4](https://github.com/IDNI/tau-lang/commit/4ab859b4e3a3bdfb92643be92a7e3d87e786890c) Decide is_nso_impl per connected component (fixes #82), [43b480d4](https://github.com/IDNI/tau-lang/commit/43b480d4a5e6a553f1fa1b0514e1d27ab3e7da34) Decide the tau-algebra predicate per support component by default
    - 2026-08-27 - [#91](https://github.com/IDNI/tau-lang/pull/91) - Opt-in: decide a `:tau` constant's sat/valid per support component, in its zero/one tests and in `simp_tau_unsat_valid`
      - commits: [d80765c5](https://github.com/IDNI/tau-lang/commit/d80765c5f6677327dc3ab723c94f6b913305c97a) Add an opt-in per-support-component sat/valid decision for :tau constants, used by is_zero/is_one, [ed66f48d](https://github.com/IDNI/tau-lang/commit/ed66f48dd423a955d0f9e992bca92bc10cdd0272) Decide simp_tau_unsat_valid's per-path tests unit-wise where exact
    - 2026-09-30 - [#198](https://github.com/IDNI/tau-lang/issues/198) - the zero and one tests of a tau constant by components answer differently from the whole formula where the formula refers to absolute time
      - Diagnostic and proposed solution: factored_tau_sat/valid decide components on their own, ignoring absolute-time references (sometimes, fixed time points, time constraints); decline factoring for such units (PR #200).
      - commits: [10146af8](https://github.com/IDNI/tau-lang/commit/10146af8c9574252fd44769e4014647a50a3cf74) Decide a formula by its components where it refers to no absolute time

  - **Solver: correct answers on large formulas.** Above a certain size, some formulas
    that can never be true were reported as possible. Taumorrow worked out exactly when
    this happened, which led to a fix in how Tau simplifies its constants.

    - 2026-07-07 - [#69](https://github.com/IDNI/tau-lang/issues/69) - `sat`/`normalize` return incorrect results (satisfiable / non-zero) for provably unsatisfiable formulas above a formula-size threshold
      - Diagnostic: a detailed characterisation of the failure (driven by formula size, one-directional, non-monotone) that narrowed down where to look; the suspected cause, a resource limit, turned out to be wrong.
      - commits: [b1013a88](https://github.com/IDNI/tau-lang/commit/b1013a88d71343340aa679fdbebf81f39d8363d2) Fix unsound normalization of Tau-BA constants (issue-69)

  - **Interpreter: correct answers and easier interactive use.** A valid program with a
    starting value was wrongly rejected as impossible, and there was no way to end
    interactive input for some streams. Taumorrow found the cause of the first and
    proposed a simple convention for the second, which was adopted.

    - 2026-09-02 - [#100](https://github.com/IDNI/tau-lang/issues/100) - Satisfiable guarded update + [0]-init is reported unsat; the equation form of the same latch runs
      - Diagnostic and proposed solution: identified a shifted twin clause that constrained the input at time 0, and proposed dropping such twins before instantiating the boundary (PR #103, closed; the maintainers fixed it one stage earlier).
      - commits: [b5796be5](https://github.com/IDNI/tau-lang/commit/b5796be58848a2b48953f833b32d15876e381216) Fold the always clauses of a partition part verbatim instead of re-aligning their lookbacks, [13973c22](https://github.com/IDNI/tau-lang/commit/13973c22ae4217dd0c728eb005993eee5160b947) Add deeper regression cases for guarded updates with initial conditions
    - 2026-09-02 - [#101](https://github.com/IDNI/tau-lang/issues/101) - Tuple console stream: no way to end input interactively (q is read as wire text)
      - Proposed solution: treat a bare `q`/`quit` at a tuple-stream prompt as the end of input (PR #104).
      - commits: [ec7a1b3e](https://github.com/IDNI/tau-lang/commit/ec7a1b3e7b85532fd1779dab61eccf0bc2325601) Accept a bare q/quit at a tuple-stream prompt as end of the run
    - 2026-09-10 - [#104](https://github.com/IDNI/tau-lang/pull/104) - Accept a bare q/quit at a tuple-stream prompt as end of the run
      - commits: [ec7a1b3e](https://github.com/IDNI/tau-lang/commit/ec7a1b3e7b85532fd1779dab61eccf0bc2325601) Accept a bare q/quit at a tuple-stream prompt as end of the run

  - **Reproducible results.** Running the same program twice could take very different
    amounts of work, because some internal data was ordered by where it happened to sit
    in memory. Taumorrow found the cause, and Tau now behaves the same way on every run.

    - 2026-08-07 - [#80](https://github.com/IDNI/tau-lang/issues/80) - `std::hash<rr<node>>` disagrees with `rr::operator==`: it hashes handle addresses
      - Diagnostic and proposed solution: showed that the hash of `rr` hashed `shared_ptr` addresses while its equality compared content, followed the effect up to node ordering, and pointed at the existing content-based `hash_tref` as the fix.
      - commits: [fe7d95be](https://github.com/IDNI/tau-lang/commit/fe7d95be02ae28ae132fa0640ea67dd881f3340d) Hash rr<node> by tree content, not by handle address (fixes #80)

  - **Parser: reading long inputs quickly.** Long constants and comment lines used to
    take minutes to read, because the time grew with the cube of their length.
    Taumorrow found the cause in the parser generator and fixed it upstream, so these
    inputs now load in seconds.

    - 2026-08-29 - [#98](https://github.com/IDNI/tau-lang/issues/98) - Parsing a `{ ... }` constant costs ~n³ in the length of its source
      - Diagnostic and proposed solution: traced the cost to the generator expanding `x*` right-recursively, which the Earley parser completes in about n³, and supplied the fix (left-recursive expansion, all nonterminal ids unchanged) as IDNI/parser#17.
      - commits: [0ac2756f](https://github.com/IDNI/tau-lang/commit/0ac2756fdff71338668bdeccd17b2a53e7be5198) updated parser submodule

  - **Normalizer: the same result in any order.** Simplifying a group of definitions
    gave a different result depending on the order they were written in. Taumorrow found
    that each equation was used only once, before the later ones were known, and
    proposed using them in dependency order.

    - 2026-09-17 - [#120](https://github.com/IDNI/tau-lang/issues/120) - normalize is order-dependent on a conjunction of definitions: the same formula folds to constants in one conjunct order and stays open in the other (equality propagation registers after it rewrites, and never revisits)
      - Diagnostic and proposed solution: equality propagation rewrites each equation before registering it and never revisits; ordering assignments by dependency (topological sort) folds the chain. Also flagged unnormalized constant callbacks in hooks.
      - commits: [e0333148](https://github.com/IDNI/tau-lang/commit/e0333148f79b7a8aeb3ed6358bd108c440dc2a67) Order equality assignments by dependency before propagating them

- **TheDarkLightX** ([@TheDarkLightX](https://github.com/TheDarkLightX)): 30 contributions (30 issues)

  - **Solver: wrong answers.** Several questions got a confident but wrong answer: a
    formula that cannot be true reported as possible, the wrong smallest or largest
    solution, a goal rejected too early, or an answer given when Tau could not actually
    decide. TheDarkLightX found where each one came from and, for most, sent a patch;
    Tau now gives the right answer, or says plainly that it does not know.

    - 2026-09-22 - [#129](https://github.com/IDNI/tau-lang/issues/129) - cnf and dnf can change the meaning of Boolean-function XOR terms
      - Proposed solution: expand Boolean-function XOR before CNF/DNF distribution, then run the reducer, with truth-table regressions for top-level, nested and complemented XOR in SBF, Tau and BV terms.
      - commits: [1a272fbd](https://github.com/IDNI/tau-lang/commit/1a272fbd55cc66ae313b9ad8e02f394a06f82274) Keep cnf and dnf of a Boolean function equivalent when it contains xor
    - 2026-09-23 - [#136](https://github.com/IDNI/tau-lang/issues/136) - Finite run silently consumes steps when bitvector outputs cannot be serialized
      - Diagnostic and proposed solution: traced the missing outputs to a serialization error that the finite-run loop hides, and proposed telling real write errors apart from "continue" and grounding parametric equality models.
      - commits: [be89453b](https://github.com/IDNI/tau-lang/commit/be89453b1e9c90fa4f44c9ee4644076dcf1a8d68) Give pure bitvector equalities ground witnesses and end a finite run on a failed write
    - 2026-09-23 - [#137](https://github.com/IDNI/tau-lang/issues/137) - sat accepts a bare bitvector spec that is unsatisfiable with its implicit always
      - Diagnostic and proposed solution: showed that the one-step arithmetic fast path decides bare stream specifications without their implicit always, and proposed excluding formulas with stream variables from that path.
      - commits: [fee72ba5](https://github.com/IDNI/tau-lang/commit/fee72ba591ee93df5b4f8a9e6c9c1932a2dd5e24) Trust only unsat from the one-step bv check on stream specs
    - 2026-09-23 - [#141](https://github.com/IDNI/tau-lang/issues/141) - Undecided normalization can produce incorrect sat and valid verdicts
      - Diagnostic and proposed solution: showed that check_decided turns an undecided normalization into a definite verdict, and proposed carrying an undecided/UNKNOWN result through normalization and the public API.
      - commits: [908fa7e7](https://github.com/IDNI/tau-lang/commit/908fa7e799a1204a3b243d9be3af137c4754390c) Report an undecided formula as UNKNOWN instead of a sat or valid verdict, [e1f0303d](https://github.com/IDNI/tau-lang/commit/e1f0303da8b1f83010a014c328f5a9318195059e) Eliminate a qlt variable bounded on both sides and honour a pinned disequality, [5397240b](https://github.com/IDNI/tau-lang/commit/5397240b492b714a1756057d4155a21a3f318819) Evaluate fex and fall over a Boolean body
    - 2026-09-23 - [#142](https://github.com/IDNI/tau-lang/issues/142) - Propagate BDD node-table exhaustion as an unknown result
      - Diagnostic and proposed solution: showed that a full BDD node table silently turns new nodes into F, giving wrong verdicts, and supplied a patch with tests that reports an unknown result instead.
      - commits: [eb612869](https://github.com/IDNI/tau-lang/commit/eb612869e63cd99ec899ed5633e1bf555a3862f3) Answer no verdict when a bdd node table fills, and recover for the next command, [07a5c222](https://github.com/IDNI/tau-lang/commit/07a5c222e449bf62b773b57a70beff38622462c5) Print no ltl verdict computed on a full bdd node table
    - 2026-09-24 - [#149](https://github.com/IDNI/tau-lang/issues/149) - Functional-quantifier binder capture gives a false no-solution result
      - Diagnostic and proposed solution: showed that fex/fall binders are not renamed, so substitution captures them, and supplied a patch that renames them and evaluates fex/fall as f[0]|f[1] and f[0]&f[1].
      - commits: [7d061497](https://github.com/IDNI/tau-lang/commit/7d061497ca1c4e7c24714d54c7f110b07aa1c6aa) Evaluate fex and fall at every normalization entry, [3902cf8e](https://github.com/IDNI/tau-lang/commit/3902cf8ef1d40fa338f6690279f28b72da065027) Pin the remaining REPL cases of issues 148, 149 and 150
    - 2026-09-24 - [#150](https://github.com/IDNI/tau-lang/issues/150) - A sometimes goal can be met during warm-up, but sat reports F
      - Diagnostic and proposed solution: showed that the normalizer rejects a sometimes goal that contradicts an always part still in warm-up, and supplied a patch that applies the shortcut only when the lookbacks allow it.
      - commits: [3bd46027](https://github.com/IDNI/tau-lang/commit/3bd4602714a13b0dd2f6b102c63abd88a5a59e50) Keep the warm-up of the always part for a sometimes clause, [3902cf8e](https://github.com/IDNI/tau-lang/commit/3902cf8ef1d40fa338f6690279f28b72da065027) Pin the remaining REPL cases of issues 148, 149 and 150
    - 2026-09-26 - [#181](https://github.com/IDNI/tau-lang/issues/181) - solve --min and --max can return the wrong minimum or maximum
      - Diagnostic: traced the wrong optimum to solve()'s loop over expression paths, which returns the first successful alternative instead of comparing optima across all alternatives.
      - commits: [f960222f](https://github.com/IDNI/tau-lang/commit/f960222fe80e9a2520abc3d4992efd621966cad8) Compare every alternative's solution in solve --min and --max
    - 2026-09-27 - [#183](https://github.com/IDNI/tau-lang/issues/183) - Quantifier elimination returns wrong results for modular bit-vector arithmetic
      - Diagnostic and proposed solution: showed complete_quantifier_elimination applies the Boolean rule f(0)&f(top)=0 to bitvector arithmetic, where it is unsound, and proposed a patch keeping such quantifiers for the solver.
      - commits: [d99d3a19](https://github.com/IDNI/tau-lang/commit/d99d3a196dcb1db5f8d836e53b072945b63ef51b) Keep a binder over bitvector arithmetic out of Boole's squeeze
    - 2026-09-28 - [#195](https://github.com/IDNI/tau-lang/issues/195) - Typed definition arguments produce false type errors
      - Diagnostic and proposed solution: found that collect_immediate_ref_arg_types reads the type of the untyped ref_arg wrapper instead of its typed expression; supplied a patch and API regression test.
      - commits: [d67356b4](https://github.com/IDNI/tau-lang/commit/d67356b4167806f7e4dcb81a8efa6ff1d62a2193) Read a call argument's type from its expression (#195)

  - **Rational values: exact and consistent answers.** Questions over rational numbers
    could round distinct values together, drop a condition, or answer true for a
    statement and for its opposite. TheDarkLightX traced each case to the part of Tau
    that handles these values and proposed the corrections, which made these answers
    exact.

    - 2026-09-23 - [#140](https://github.com/IDNI/tau-lang/issues/140) - solve can return a qlt assignment that violates its input
      - Diagnostic and proposed solution: traced the bad models to the qlt witness builder picking each variable on its own, and proposed building a joint model, checking it against the input and reporting unknown otherwise.
      - commits: [9bc370da](https://github.com/IDNI/tau-lang/commit/9bc370dac9568e21928a1e7d4a203158cfafac8b) Solve qlt ordering systems jointly and verify the model before returning it
    - 2026-09-24 - [#148](https://github.com/IDNI/tau-lang/issues/148) - qlt elimination ignores a disequality when only one value remains
      - Diagnostic and proposed solution: showed that qlt elimination drops a disequality even when the bounds leave a single value, supplied a patch and gave the exact density-based elimination rule for that case.
      - commits: [e47e43c0](https://github.com/IDNI/tau-lang/commit/e47e43c0dfb63a2ee936a2d623f72e60780fe2c4) Eliminate a qlt variable pinned to one value exactly, [9579437b](https://github.com/IDNI/tau-lang/commit/9579437be1721eb55d9ac3535d10dc14ea010bac) Eliminate qlt disequalities beside two bounds by density, [3902cf8e](https://github.com/IDNI/tau-lang/commit/3902cf8ef1d40fa338f6690279f28b72da065027) Pin the remaining REPL cases of issues 148, 149 and 150
    - 2026-09-28 - [#186](https://github.com/IDNI/tau-lang/issues/186) - qint rounds distinct rational endpoints to the same value and returns wrong equality results
      - Diagnostic and proposed solution: traced the error to parse_endpoint converting via std::stod and the map<double,double> representation, and proposed exact rational endpoint parsing and storage, with an explicit error instead of silent rounding.
      - commits: [c7e87400](https://github.com/IDNI/tau-lang/commit/c7e8740064af64f6388faa88e7e9b3f3fd75c7f9) Keep qint endpoints as exact rationals
    - 2026-09-28 - [#187](https://github.com/IDNI/tau-lang/issues/187) - qlt rejects an existential compound-order formula while accepting a concrete instance
      - Diagnostic and proposed solution: showed the qlt interval collector accepts compound terms such as x & {c}, and proposed a case split (x = {c} or not) to remove the meet before order elimination.
      - commits: [f2eafe30](https://github.com/IDNI/tau-lang/commit/f2eafe3039f131037c55888e3c24d9a972f3ef47) Keep qlt quantifier elimination to the atoms it reads
    - 2026-09-28 - [#188](https://github.com/IDNI/tau-lang/issues/188) - qlt endpoint handling makes contradictory quantified formulas normalize to T
      - Diagnostic: showed qlt interval elimination treats 0/1 as infinite bounds while equalities to them go through substitution as ordinary values, so the two paths assume different domains.
      - commits: [f2eafe30](https://github.com/IDNI/tau-lang/commit/f2eafe3039f131037c55888e3c24d9a972f3ef47) Keep qlt quantifier elimination to the atoms it reads
    - 2026-09-28 - [#189](https://github.com/IDNI/tau-lang/issues/189) - qlt returns F for a true quantified formula and T for its negation
      - Diagnostic and proposed solution: located the qlt interval collector's fallback, which silently drops unsupported nested constraints that do not mention the current variable; proposed keeping or evaluating them, or declining the shortcut.
      - commits: [f2eafe30](https://github.com/IDNI/tau-lang/commit/f2eafe3039f131037c55888e3c24d9a972f3ef47) Keep qlt quantifier elimination to the atoms it reads
    - 2026-09-30 - [#197](https://github.com/IDNI/tau-lang/issues/197) - qlt gives wrong quantified answers for intersections with a single point
      - Diagnostic and proposed solution: pointed to generic rules assuming every nonempty value splits into two parts, which a single point cannot; later supplied a patch that eliminates such quantifiers by splitting on whether the value contains the point.
      - commits: [30c40849](https://github.com/IDNI/tau-lang/commit/30c40849393033545d89d38474f9f46f7308baee) Decide qlt quantifiers that meet a single point, [e0585f2a](https://github.com/IDNI/tau-lang/commit/e0585f2a1c1d70c59d590b6b7228242cadacfb66) Eliminate qlt quantifiers over points by cells

  - **Speed.** Some work was done again and again for no reason: simplifying the same
    pieces, trying rules that could not apply, or re-reading the same part of the input.
    TheDarkLightX measured each case and sent patches, several of which made the
    affected steps many times faster.

    - 2026-09-22 - [#130](https://github.com/IDNI/tau-lang/issues/130) - Equality substitution can expand without bound and terminate on allocation failure
      - Diagnostic and proposed solution: traced the blow-up to equality substitution descending into replacements that contain replaceable terms, and proposed a one-line patch that stops the traversal at changed nodes, with timings.
      - commits: [20948938](https://github.com/IDNI/tau-lang/commit/20948938d7b45cf7430fb915b907081426c52a4f) Keep equality substitution from replacing a term by a term containing it
    - 2026-09-23 - [#134](https://github.com/IDNI/tau-lang/issues/134) - Boole decomposition and BDD construction revisit shared subexpressions
      - Diagnostic and proposed solution: showed that Boole decomposition and BDD construction revisit shared subexpressions within a call, and supplied a memoization patch (about 8x faster on a 20-variable XOR).
      - commits: [af98abe5](https://github.com/IDNI/tau-lang/commit/af98abe52b4a3b8ec0b9293663c2b8f91b649aa2) Process shared subterms of a Boole decomposition once
    - 2026-09-23 - [#135](https://github.com/IDNI/tau-lang/issues/135) - Definition expansion scans rules whose heads are absent from the formula
      - Diagnostic and proposed solution: showed that definition expansion tries every rule even when its head cannot occur in the formula, and supplied a patch that skips rules whose head signature is absent (about 7.9x faster on nested definitions).
      - commits: [20ad15fc](https://github.com/IDNI/tau-lang/commit/20ad15fc010337a88a646e0f5359bfcf4bb6e147) Skip definitions whose head does not occur in the formula
    - 2026-09-23 - [#138](https://github.com/IDNI/tau-lang/issues/138) - Limit parser position revisits and retire completed-position bookkeeping
      - Diagnostic and proposed solution: showed that the parser re-processes completed items at each input position and keeps stale bookkeeping, and supplied patches with a position worklist and bookkeeping that is dropped once a position is done, lowering time and memory.
      - commits: [a5cab8c3](https://github.com/IDNI/tau-lang/commit/a5cab8c3a130507f55af62da992ecc9097a176c9) Bump the parser to process only new and pending items per position
    - 2026-09-23 - [#139](https://github.com/IDNI/tau-lang/issues/139) - Avoid redundant substitution while calculating the lookback read set
      - Diagnostic and proposed solution: showed that appear_within_lookback substitutes and simplifies even when no queried variable occurs, and supplied a patch that skips those formulas and stops early (up to 11x faster).
      - commits: [36b701bf](https://github.com/IDNI/tau-lang/commit/36b701bf1f57a172f843859fdbdb0d4e14993ad9) Skip the lookback read-set rewrite for formulas without queried inputs

  - **Crashes on unexpected input.** Two commands crashed when given something other
    than a formula. TheDarkLightX found the missing checks and proposed them, and these
    commands now refuse such input with a clear message.

    - 2026-09-23 - [#131](https://github.com/IDNI/tau-lang/issues/131) - ltl explanation needs a formula input contract before translation
      - Diagnostic and proposed solution: showed that ltl_explain accepts a term where a formula is required, so the crash surfaces in cvc5, and proposed an invalid-argument check at the start of ltl_explain.
      - commits: [48b29d8b](https://github.com/IDNI/tau-lang/commit/48b29d8be85027bd27168f18c84296be07ca22b0) Reject a term given to ltl instead of explaining it as a formula
    - 2026-09-23 - [#132](https://github.com/IDNI/tau-lang/issues/132) - valid lacks a formula-shape check before validity backend dispatch
      - Diagnostic and proposed solution: traced the crash to the validity fast path, which negates a non-formula term, and proposed reusing sat's formula gate in valid and in the fast path.
      - commits: [744a6d2c](https://github.com/IDNI/tau-lang/commit/744a6d2c8cc197ae945398e7a798a0c17ec84f13) Reject a term given to valid before it is negated as a formula

  - **Reading and printing formulas.** The `min` and `max` built-ins lost their meaning
    in some positions, and a printed formula could not always be read back as the same
    formula. TheDarkLightX found both causes and supplied the fixes.

    - 2026-09-28 - [#185](https://github.com/IDNI/tau-lang/issues/185) - min/max lose their built-in meaning under casts and complement; normalized output changes the result
      - Diagnostic and proposed solution: found the cast, complement and juxtaposition grammar rules omit bf_min/bf_max, and supplied a grammar patch with tests, later extended to definition bodies and fixed-point fallbacks.
      - commits: [378ccf21](https://github.com/IDNI/tau-lang/commit/378ccf214f05cdc7b24485d780b23a6ab0d47038) Parse min and max under a cast, a complement and a juxtaposition, [8c78610d](https://github.com/IDNI/tau-lang/commit/8c78610da95f5ea0cca5bf32e3d5c7246c1585a4) Read a bare min or max as the built-in in definitions and fallbacks
    - 2026-09-28 - [#190](https://github.com/IDNI/tau-lang/issues/190) - With charvar=false, printing a conjunction can merge its variable names
      - Proposed solution: a tested patch that connects api::set_charvar to the printer and emits & between conjunction operands when multi-character names are on, so printed formulas parse back unchanged.
      - commits: [48a5e304](https://github.com/IDNI/tau-lang/commit/48a5e304079a7e100ecb62886ce335773a99e065) Print & between conjuncts when charvar is off (#190)

  - **Building, testing, documentation and the editor.** Tau did not build on macOS, one
    test expected more than Tau promises, the help described search limits wrongly, and
    the browser editor froze while it evaluated. TheDarkLightX explained each problem
    and sent the corrections.

    - 2026-09-23 - [#133](https://github.com/IDNI/tau-lang/issues/133) - IDE evaluation blocks unrelated HTTP requests
      - Diagnostic and proposed solution: showed that async IDE handlers call a blocking subprocess.run on the event loop, and supplied patches that move evaluation to a dedicated worker (optionally with bounded admission), with tests and latency measurements.
      - commits: [31592194](https://github.com/IDNI/tau-lang/commit/31592194ca771fe23dbd91bac9fa78edab27f301) Run the IDE's one-shot tau commands without blocking the event loop
    - 2026-09-28 - [#191](https://github.com/IDNI/tau-lang/issues/191) - Search-limit help and API comments still describe an undecided result as unsatisfiable
      - Proposed solution: a diff that fixes the -F help text and the API and satisfiability comments so they say a reached search limit is an error without a verdict, with default 500.
      - commits: [92b76017](https://github.com/IDNI/tau-lang/commit/92b760179ac6412270e54369a2d225dd226fdf18) Describe a search limit that is reached as an error without a verdict
    - 2026-09-28 - [#193](https://github.com/IDNI/tau-lang/issues/193) - macOS build fails because static-path enum names collide with TRUE and FALSE
      - Diagnostic and proposed solution: traced the build failure to the TRUE/FALSE enum members in for_each_static_path colliding with macOS SDK macros, and supplied a rename-only patch.
      - commits: [fc973291](https://github.com/IDNI/tau-lang/commit/fc97329163c0a44587c5f0ddb9025b4601955158) Rename the static-path enum members that macOS defines as macros
    - 2026-09-28 - [#194](https://github.com/IDNI/tau-lang/issues/194) - Existing constant-size-budget test stops before its final output on macOS
      - Diagnostic and proposed solution: traced the expectation to commit b5b05360 and explained why the default size budget need not fit ten steps; proposed a test patch with an explicit 4096 limit.
      - commits: [dd5a841a](https://github.com/IDNI/tau-lang/commit/dd5a841ac1e8ee80aec73939239225a88f53f056) Give the ten-step constant budget test an explicit limit (#194)

- **David Castro** ([@castrod](https://github.com/castrod)): 4 contributions (2 pull requests, 2 issues)

  - **Interpreter: step time with look-back.** A program that looks three steps back
    and has three starting values became dramatically slower from its fifth step.
    The cause was found and fixed, and each step now takes about the same time again.

    - 2026-09-11 - [#115](https://github.com/IDNI/tau-lang/issues/115) - Guarded spec with lookback 3 and three initial conditions: per-step time explodes from step 5
      - Diagnostic and proposed solution: path enumeration ran before memory substitution, so the initial prefix multiplied the paths; proposed substituting memory first and folding constant time positions.
      - commits: [db823a6c](https://github.com/IDNI/tau-lang/commit/db823a6c7025b1e8ae4157ce4c139a9de3e4dbe3) keep step_spec current before the read set is computed, [503926b1](https://github.com/IDNI/tau-lang/commit/503926b133e162363797778dd3a8aee734deb83d) Fold the constant time positions when the continuation becomes the final system

  - **Plugins for new kinds of values: review and fixes.** Before the support for adding
    new kinds of values reached the main branch, David Castro reviewed it, listed the
    build failures and errors it still had, and fixed the blocking ones, including
    building with the declared compilers and refusing a broken selection of kinds when
    the build is set up.

    - 2026-09-09 - [#111](https://github.com/IDNI/tau-lang/issues/111) - feature/plugins: review of the BA-plugin descriptor and capability-fold design (toolchain, errors, tests, docs)
      - Diagnostic and proposed solution: review of feature/plugins locating toolchain failures (gcc 13 ICE, -Wchanges-meaning, clang crash), descriptor/fold errors, test and doc gaps, with fixes in PRs #112-#119.
      - commits: [ec2604cd](https://github.com/IDNI/tau-lang/commit/ec2604cd451f31d1dd6aa65f2365f45763eea3e3) Name the output_always_satisfiable probe instead of nesting it in the fold, [c37fdeec](https://github.com/IDNI/tau-lang/commit/c37fdeec9f81e1c56d9d6f5a598fc31e96cc4abe) Probe print_constant with its real signature in the conformance test, [8f93b8f9](https://github.com/IDNI/tau-lang/commit/8f93b8f9f67698331a8d3c960445e75bcc3bf2c9) Close a report's root scope before printing it, [801da5d5](https://github.com/IDNI/tau-lang/commit/801da5d50c8dde451f509d67364fd13687a3b19b) Classify io variables in make_interpreter, [e73f2f86](https://github.com/IDNI/tau-lang/commit/e73f2f868e9a90524d12508045859cd0a2816ffd) Spell the two game members with elaborated type specifiers, [4c2aef9c](https://github.com/IDNI/tau-lang/commit/4c2aef9ca0584e497cc9e723ecfc7daef26228b9) Let the RelWithDebInfo presets build with gcc, [a0a88b68](https://github.com/IDNI/tau-lang/commit/a0a88b6806c9ee751d35ec05d4e2494c5fb3ac7d) Reject a BA id listed twice in TAU_BAS, [83d6a1e9](https://github.com/IDNI/tau-lang/commit/83d6a1e9f0a98d905c80aefc040a016ba0563a2a) Refuse a pack in which no BA can host a Boolean at configure time, [ab69f9b6](https://github.com/IDNI/tau-lang/commit/ab69f9b604f1181c3b1b066cadc47f7223a3f3de) Run CI on feature and issue branches and on the bv-less pack, [a91393cf](https://github.com/IDNI/tau-lang/commit/a91393cf0f62e648430fabe7bdaa2aeb1935fb8a) Document the compilers the branch builds with, [87141fc1](https://github.com/IDNI/tau-lang/commit/87141fc120a5cfbc59cee770c297ef0d95b04630) Assert every descriptor of the configured pack is complete, one BA at a time, [667e7050](https://github.com/IDNI/tau-lang/commit/667e7050ecbb4be77e226fc138805a6d342fe3cb) Add presets for the bv-only and bv-less packs, [912826db](https://github.com/IDNI/tau-lang/commit/912826db145ad64d9901bf2b595727ced2429527) Remove the solver folds nothing calls, [8dc128e4](https://github.com/IDNI/tau-lang/commit/8dc128e466c06a9591a21091bcf4b082dc9f0fa3) Let tau compile take the compiler for the emitted project, [5bb60a3b](https://github.com/IDNI/tau-lang/commit/5bb60a3bbcbdbdc8c50e8d7f9385132d73719c39) Probe the compile verb's --cxx with a spec every pack can parse, [2f38dc0a](https://github.com/IDNI/tau-lang/commit/2f38dc0a2e0dee78fd7632be7a2dbcb743ad2158) Name the CI matrix entries and give the compile verb its compiler, [ad41b868](https://github.com/IDNI/tau-lang/commit/ad41b8688150d0f87df8ef3ec734174b2ee65fd4) Point the emitted project at the packages the emitting build found, [e756eff5](https://github.com/IDNI/tau-lang/commit/e756eff512ec7a4dac5e7d73f044fd94b0cb6d88) Move the truncated-literal probe into the pack traits, [aa8388d8](https://github.com/IDNI/tau-lang/commit/aa8388d899a43e4a8f4954d5b297dbcf4ba956ae) Name every optional capability with one concept, [9679a7ed](https://github.com/IDNI/tau-lang/commit/9679a7ed84649eba72279a429115e0c2b6e87829) Fold every owner-gated capability through pack_owner_apply, [35ed1160](https://github.com/IDNI/tau-lang/commit/35ed1160b72f271b9250f1b7f1623770ea19e4dc) Check every capability fold against the declaration it answers for, [1b123fec](https://github.com/IDNI/tau-lang/commit/1b123fec4b84e8ddccc775b3700bcf8e3104b76e) Remove the unused constexpr name comparison, [d70318aa](https://github.com/IDNI/tau-lang/commit/d70318aaa5b11d76d5b7e948e0f82da233f3042b) Pin the carrier order in the emitted project, [9466e205](https://github.com/IDNI/tau-lang/commit/9466e20534c57796b751f3b590c970a724aa46a6) Derive the arithmetic-theory trait from arith_ops and solve, [7e809b93](https://github.com/IDNI/tau-lang/commit/7e809b9300044cbdc0ee2c3679e99878eb69d967) Refuse a pack with two solvers until pack_solve routes by owner, [3ef800c9](https://github.com/IDNI/tau-lang/commit/3ef800c941592d0b14647219607bf096de4fbf48) Require value_constant of every Boolean carrier and give Bool one, [3823f906](https://github.com/IDNI/tau-lang/commit/3823f9066274ad1e3a121d692438b74ddf26fa9e) Seed a multi-state Mealy strategy in every pack with a Boolean carrier, [938fe845](https://github.com/IDNI/tau-lang/commit/938fe845542e5819484935e63d7af6cf9a2a4272) Let one BA claim propositional synthesis and check it at compile time, [664f3e04](https://github.com/IDNI/tau-lang/commit/664f3e04c276237a908634f9ec4868fee45488bb) Report a missing wrapper instead of packing a garbage constant, [a3ff1f9d](https://github.com/IDNI/tau-lang/commit/a3ff1f9d9bd4e974724ad84c05176f97a930bfd0) Write a carrier-typed output as its atom's value, not as its prop's truth, [f0275b1b](https://github.com/IDNI/tau-lang/commit/f0275b1bb9dac1694b3ca08860a295875c613293) Make the parameterised-family members optional, [aae23d0a](https://github.com/IDNI/tau-lang/commit/aae23d0a83f8d368046b8a25760501df661c3539) Keep one type-ownership member per descriptor, [7dcf5bc6](https://github.com/IDNI/tau-lang/commit/7dcf5bc64f0f3f623c8a6365e8cc78eceeb8c87c) Ask the parser whether a literal ended early instead of grepping its message, [f7187eb9](https://github.com/IDNI/tau-lang/commit/f7187eb913e87f606344605bf1255fb671ea9520) Pin the REPL's messages for an unknown BA family or option, [e2f9b78e](https://github.com/IDNI/tau-lang/commit/e2f9b78e73a18300380eefabbf56eaf1a172b2ec) Point the emitted artifact at the out-of-tree BA's headers, [de2efe23](https://github.com/IDNI/tau-lang/commit/de2efe23b7769ea960e90ef105f3a94b64c0704e) Run the minimal-pack descriptor checks in every pack, [dcf78577](https://github.com/IDNI/tau-lang/commit/dcf7857783c14f6fc7ab8236ff7bac8f533799ad) Give the out-of-tree BA one capability of each resolution kind, [0f689182](https://github.com/IDNI/tau-lang/commit/0f689182c206e5b9fff05fab962dff36e2b97e74) Give test_ltl_fuzz the long ctest timeout, [0dadffbf](https://github.com/IDNI/tau-lang/commit/0dadffbfbd8f00718176aa21c439ae876aead14e) Assert the fold empty cases in a pack without bv, [8b9ef5db](https://github.com/IDNI/tau-lang/commit/8b9ef5db05f8b1b1f387ef1e4d6c739c6594f710) Bring the BA docs in line with the dispatcher and the presets, [7756bf2e](https://github.com/IDNI/tau-lang/commit/7756bf2e9904ff9b7358254efa7262fb0547d806) Note the test-only pack defines and the splitter contract, [2ec214ce](https://github.com/IDNI/tau-lang/commit/2ec214ce0c9412401982d4a4750146b180e9d411) Describe the capability folds as they stand, [1cac48f1](https://github.com/IDNI/tau-lang/commit/1cac48f15f3040256edff813a49ab5698c9dd9c9) Document every optional capability and how the pack resolves it
    - 2026-09-13 - [#112](https://github.com/IDNI/tau-lang/pull/112) - Fix the gcc-13 ICE, the dead print_constant check and the two devel-only aborts on feature/plugins (#111)
      - commits: [801da5d5](https://github.com/IDNI/tau-lang/commit/801da5d50c8dde451f509d67364fd13687a3b19b) Classify io variables in make_interpreter, [8f93b8f9](https://github.com/IDNI/tau-lang/commit/8f93b8f9f67698331a8d3c960445e75bcc3bf2c9) Close a report's root scope before printing it, [c37fdeec](https://github.com/IDNI/tau-lang/commit/c37fdeec9f81e1c56d9d6f5a598fc31e96cc4abe) Probe print_constant with its real signature in the conformance test, [ec2604cd](https://github.com/IDNI/tau-lang/commit/ec2604cd451f31d1dd6aa65f2365f45763eea3e3) Name the output_always_satisfiable probe instead of nesting it in the fold
    - 2026-09-13 - [#113](https://github.com/IDNI/tau-lang/pull/113) - Build with the pinned compilers, refuse bad packs at configure time, run CI on branches (#111)
      - commits: none

- **andrei-idni** ([@andrei-idni](https://github.com/andrei-idni)): 3 contributions (3 issues)

  - **Bitvectors: safe arithmetic for financial rules.** In Tau, fixed-size number
    arithmetic wraps around or returns the largest value on overflow or division by
    zero, which is dangerous in pricing and fee rules. andrei-idni explained why the
    results came out wrong and wrote specification patterns that avoid the problem.
    These patterns are now in the documentation, together with new `min`/`max`
    built-in functions and an exact-arithmetic mode.

    - 2026-08-26 - [#86](https://github.com/IDNI/tau-lang/issues/86) - `(a*b)/c` truncates the product to the operand width, so a percentage cannot be expressed
      - Diagnostic and proposed solution: showed that the product wraps before the division, confirmed it with divide-first and shift controls, and proposed an exact divmod decomposition (`a*n/d = q*n + (r*n)/d`).
      - commits: [d2ec08cb](https://github.com/IDNI/tau-lang/commit/d2ec08cb4c5de36ac2551b4f9dd27206b38ea10a) Make the (bv[N]) cast usable on variable operands (#86, #87, #88), [2df99cb7](https://github.com/IDNI/tau-lang/commit/2df99cb7b0d92198e5492568ad9b85f483e3a272) Chain same-level bitvector operators left to right and keep (bv[N]) a cast, [376d684d](https://github.com/IDNI/tau-lang/commit/376d684d5ffb6f94d3b6f2e71b2cbbb6d4109e03) Merge feature/bv-widening into main
    - 2026-08-26 - [#87](https://github.com/IDNI/tau-lang/issues/87) - Bitvector overflow and underflow are silent: no error, no flag, no way for a spec to detect it
      - Proposed solution: four verified patterns (saturating add, checked multiply by a constant, checked multiply by a variable, and checked subtract) that detect overflow with the existing operators. They are now in the README, and `min`/`max` builtins were added.
      - commits: [d2ec08cb](https://github.com/IDNI/tau-lang/commit/d2ec08cb4c5de36ac2551b4f9dd27206b38ea10a) Make the (bv[N]) cast usable on variable operands (#86, #87, #88), [4437dad2](https://github.com/IDNI/tau-lang/commit/4437dad2cf9b94b228ebb34e68736899020b562d) Add min/max bitvector builtins with cvc5 and blasting support, [10ae91ea](https://github.com/IDNI/tau-lang/commit/10ae91ea65ceb466978866b0d54f62221a2cd50d) Document overflow guards, saturating idioms and min/max in the README, [02fbae29](https://github.com/IDNI/tau-lang/commit/02fbae296c49b0ded2354d36b989a1bbd6cb4281) Demo the min/max builtins in the solver and interpreter demos
    - 2026-08-26 - [#88](https://github.com/IDNI/tau-lang/issues/88) - Division by zero yields the bitvector maximum; please document it, or offer a way to make it an error
      - Proposed solution: a guard idiom that lets the specification choose its own value for a zero divisor. It is now in the README word for word.
      - commits: [d2ec08cb](https://github.com/IDNI/tau-lang/commit/d2ec08cb4c5de36ac2551b4f9dd27206b38ea10a) Make the (bv[N]) cast usable on variable operands (#86, #87, #88), [d80aa50c](https://github.com/IDNI/tau-lang/commit/d80aa50c286a5fdfa8e93461158876bf76ae0082) Document bitvector division and remainder by zero in the README

- **Jme-Tau** ([@Jme-Tau](https://github.com/Jme-Tau)): 2 contributions (1 pull request, 1 issue)

  - **Building on macOS.** Tau did not build with Apple's compiler on Apple Silicon
    Macs, and several tests crashed on startup or exit. Jme-Tau found the root causes
    and sent the patches. Tau now builds and passes its tests on macOS.

    - 2026-08-06 - [#78](https://github.com/IDNI/tau-lang/issues/78) - macOS arm64: AppleClang build blockers and static-lifetime crashes
      - Diagnostic and proposed solution: identified cvc5 terms that outlived cvc5's node manager at exit, and parser mutexes used before they were initialized. Sent the fixes as IDNI/parser#16 and #79.
      - commits: [3e91a07b](https://github.com/IDNI/tau-lang/commit/3e91a07b8505a299783582f7712480e4aadc0e98) fix: make AppleClang build and teardown safe, [686fcc4a](https://github.com/IDNI/tau-lang/commit/686fcc4a7ba774495a3b3c04fb3604dc1ae0e203) Widen hash seeds to uint64_t so hash_combine binds on macOS, [49ce8190](https://github.com/IDNI/tau-lang/commit/49ce81901e4fadbad4bde966280a0ce7bd9eee4e) Merge PR #79: make the AppleClang build and cvc5 teardown safe
    - 2026-09-11 - [#79](https://github.com/IDNI/tau-lang/pull/79) - Fix AppleClang build errors and cvc5 teardown
      - commits: [3e91a07b](https://github.com/IDNI/tau-lang/commit/3e91a07b8505a299783582f7712480e4aadc0e98) fix: make AppleClang build and teardown safe

- **gmikeyg** ([@gmikeyg](https://github.com/gmikeyg)): 2 contributions (2 issues)

  - **Python bindings: checking specifications from Python.** Python users could run
    specifications but not ask whether they are possible, always true, or impossible
    to implement. gmikeyg proposed the Python functions for this, based on a working
    prototype.

    - 2026-09-23 - [#146](https://github.com/IDNI/tau-lang/issues/146) - Capability request: sat / valid / unrealizable (and an unsat core) in the Python bindings
      - Proposed solution: sketched the Python API (`sat`, `valid`, `unrealizable`, `unsat_core` and a stream-typing reset) from a working deletion-based prototype, and later refined the model by treating proposer-controlled inputs as outputs.
      - commits: [6105d392](https://github.com/IDNI/tau-lang/commit/6105d39257c59963632f040b9cdf20aad15751b9) Expose sat, valid, realizable and a minimal unsat core in the Python binding

  - **Definitions with quantifiers.** Definitions that contain a quantifier gave wrong
    answers once they were used. gmikeyg found that the quantified names of the
    definition and of the question clashed after expansion.

    - 2026-09-21 - [#128](https://github.com/IDNI/tau-lang/issues/128) - Predicate definitions that contain a quantifier give wrong valid/sat/normalize results
      - Diagnostic: bound variables are numbered by depth per formula (find_biggest_quant_id), so a definition's bound variable collides with the query's innermost binder after expansion.
      - commits: [0d488c66](https://github.com/IDNI/tau-lang/commit/0d488c6608447dbea6878bc15ff3746ef25b49ce) Keep definition expansion from capturing a bound variable, [3eccc9ef](https://github.com/IDNI/tau-lang/commit/3eccc9ef685cf9c1d070763abacb9432b5e2dd23) Cover the definition capture fixes on a recurrence relation

## Numbers

- Merged pull requests: 12
- Issues counted as contributions: 59
- Distinct contributors: 6
- First-time contributors: 3 (andrei-idni, Jme-Tau and gmikeyg; Jme-Tau had filed
  issues in Q2 2026, but none of them was solved in `main`)

# Q2 2026

- **LEARNTAU** ([@LEARNTAU](https://github.com/LEARNTAU)): 1 contribution (1 pull request)

  - **Documentation.** Fixed a spelling mistake in the documentation.

    - 2026-04-07 - [#63](https://github.com/IDNI/tau-lang/pull/63) - typo correction arithmatic -> arithmetic
      - commits: [d7656ee9](https://github.com/IDNI/tau-lang/commit/d7656ee93a29dbae664791dc1c844c95600efe2a) typo correction arithmatic -> arithmetic

## Numbers

- Merged pull requests: 1
- Issues counted as contributions: 0
- Distinct contributors: 1
- First-time contributors: 0

# Q1 2026

- **TheDarkLightX** ([@TheDarkLightX](https://github.com/TheDarkLightX)): 2 contributions (2 issues)

  - **Interpreter: declaring input and output streams.** Two bugs in how programs
    declare their inputs and outputs could crash the tool, or silently read from the
    console instead of the file you named. TheDarkLightX found the exact lines at
    fault and proposed the fixes.

    - 2026-01-16 - [#54](https://github.com/IDNI/tau-lang/issues/54) - "Segfault on <var> = out without stream target"
      - Diagnostic and proposed solution: found the missing `break` that let the `input_def`/`output_def` case fall through in type inference, causing an out-of-range child access, and proposed adding it (the grammar side was fixed by pt7k in #57).
      - commits: [fa058eaf](https://github.com/IDNI/tau-lang/commit/fa058eaf42ea53ecd1c31aed6e76a222e90b0d58) Updated tau.tgf to fix issue #54, [d362b077](https://github.com/IDNI/tau-lang/commit/d362b07705bd818a3d26e72c4f13ad819eebc661) generated tau_parser fixing issue #54, [639efb90](https://github.com/IDNI/tau-lang/commit/639efb90ed1c9ea86c2bd1a74bb7960c5799ad5b) skipping input_def and output_def in ba type inference
    - 2026-01-24 - [#58](https://github.com/IDNI/tau-lang/issues/58) - File I/O stream paths not stored correctly, always defaults to console
      - Diagnostic and proposed solution: located the wrong child access in `process_io_def` and proposed going one level deeper, to the `file_name` node.
      - commits: [92d69eff](https://github.com/IDNI/tau-lang/commit/92d69eff10f066efb20a4dace426197275a900ec) properly typing streams in input_def/output_def, [84ecce66](https://github.com/IDNI/tau-lang/commit/84ecce6624fa89966bdba04b2b33e89331b1c94b) getting proper stream data from input_def/output_def, [2a4b9d6f](https://github.com/IDNI/tau-lang/commit/2a4b9d6f7e27a784ec1828dea6aa224db2805e67) fixing passing streams to interpreter

- **pt7k** ([@pt7k](https://github.com/pt7k)): 1 contribution (1 pull request)

  - **Parser: output declarations must name a target.** The language now requires an
    output declaration to say where the output goes, which prevents the crash reported
    in #54.

    - 2026-01-25 - [#57](https://github.com/IDNI/tau-lang/pull/57) - output_def with non-optional stream target
      - commits: [fa058eaf](https://github.com/IDNI/tau-lang/commit/fa058eaf42ea53ecd1c31aed6e76a222e90b0d58) Updated tau.tgf to fix issue #54, [d362b077](https://github.com/IDNI/tau-lang/commit/d362b07705bd818a3d26e72c4f13ad819eebc661) generated tau_parser fixing issue #54

- **LEARNTAU** ([@LEARNTAU](https://github.com/LEARNTAU)): 1 contribution (1 pull request)

  - **Documentation.** Fixed a typo in the documentation.

    - 2026-01-08 - [#53](https://github.com/IDNI/tau-lang/pull/53) - typo correction "
      - commits: [2aa494a8](https://github.com/IDNI/tau-lang/commit/2aa494a82e6b14f6918e9531943982c3b4668e0f) typo correction "

## Numbers

- Merged pull requests: 2
- Issues counted as contributions: 2
- Distinct contributors: 3
- First-time contributors: 1 (TheDarkLightX)

# Q4 2025

- **pt7k** ([@pt7k](https://github.com/pt7k)): 2 contributions (2 issues)

  - **Build: compiling with Ninja.** The project stopped compiling after a merge. pt7k
    showed that the failure only happened with one build tool, Ninja, and gave the
    command-line change that fixed it.

    - 2025-11-02 - [#40](https://github.com/IDNI/tau-lang/issues/40) - Error during compilation (fatal error: tau.h missing) since merge commit 4edc9b2
      - Diagnostic and proposed solution: showed the failure happens only with the Ninja generator, because of a missing header dependency, and proposed replacing `--parallel` with `-j`.
      - commits: [85e518a7](https://github.com/IDNI/tau-lang/commit/85e518a7896c8a307c24caafabf4352a8cce8bc2) fix building with ninja, [3f4fa08c](https://github.com/IDNI/tau-lang/commit/3f4fa08c521fd44a5ec214bc95af2232946c5e1b) cmake: add `tau_header` dependency

  - **Bitvectors: large numbers.** Operations on fixed-size numbers only worked on the
    lowest 63 bits. pt7k proposed a safe way to read large number literals.

    - 2025-12-11 - [#45](https://github.com/IDNI/tau-lang/issues/45) - Bitvector operations now working only within the least significant 63 bits
      - Proposed solution: only convert a literal with `std::stoll` when it has fewer than 19 digits, and use a non-simplifying path for larger constants.
      - commits: [9c0fb6e9](https://github.com/IDNI/tau-lang/commit/9c0fb6e9941fdaed6efae72571adae7ccbc509c9) rolling back simplification as is causing bv failures

- **LEARNTAU** ([@LEARNTAU](https://github.com/LEARNTAU)): 2 contributions (1 pull request, 1 issue)

  - **Normalizer: stopping endless simplification.** Some expressions made the tool
    simplify forever at full CPU. LEARNTAU suggested putting a limit on how long
    simplification may run. That limit now exists as a user setting.

    - 2025-10-25 - [#36](https://github.com/IDNI/tau-lang/issues/36) - REPL hangs and consumes 100% CPU when normalizing undefined terms (int[7], int[10]) — Suggest adding safeguards for undefined and non-grounded expressions
      - Proposed solution: a recursion-depth or rewrite-round limit to stop normalization that never ends, later implemented as `-R`; the root cause itself was found by the maintainers.
      - commits: [d117ed28](https://github.com/IDNI/tau-lang/commit/d117ed281685c0862fea677d066e9f3e7aedcb62) Fix definitions never matching term-position calls, bound expansion (issue-36), [1d30407d](https://github.com/IDNI/tau-lang/commit/1d30407d15353fef1c7b76ab4653d26bd3ba972e) Make normalizer/rewriter/bv-simplify round caps runtime parameters, [13237140](https://github.com/IDNI/tau-lang/commit/132371404bdc772d25a8b41743b753b40f53b2ee) Add CLI options for all runtime limits

  - **Documentation.** Fixed broken formatting in the README.

    - 2025-12-17 - [#48](https://github.com/IDNI/tau-lang/pull/48) - Fix backticks formatting in README
      - commits: [a663b1bf](https://github.com/IDNI/tau-lang/commit/a663b1bf2a7c756cd733cb9f5386b1fc570deba7) fixing a typo in README.md

- **Taumorrow** ([@taumorrow](https://github.com/taumorrow)): 1 contribution (1 issue)

  - **Type inference: hangs on complex formulas.** Some larger formulas made Tau hang
    while it worked out the types of their parts. Taumorrow narrowed the regression
    down to a short range of commits.

    - 2025-12-17 - [#49](https://github.com/IDNI/tau-lang/issues/49) - Bug: Type Inference Regression: Hangs on Complex Formulas
      - Diagnostic: bracketed the regression between commits dcc86f47 and 028649f2 and attributed it to a blow-up in type inference.
      - commits: none

## Numbers

- Merged pull requests: 1
- Issues counted as contributions: 4
- Distinct contributors: 3
- First-time contributors: 2 (Taumorrow, LEARNTAU)

# Q2 2025

- **pt7k** ([@pt7k](https://github.com/pt7k)): 1 contribution (1 pull request)

  - **Documentation.** Improved the README.

    - 2025-04-23 - [#29](https://github.com/IDNI/tau-lang/pull/29) - Update README.md
      - commits: [27b2eb17](https://github.com/IDNI/tau-lang/commit/27b2eb179a31bf4984c16ad6ddae5d4b5fc55f78) Update README.md

## Numbers

- Merged pull requests: 1
- Issues counted as contributions: 0
- Distinct contributors: 1
- First-time contributors: 0

# Q1 2025

- **pt7k** ([@pt7k](https://github.com/pt7k)): 2 contributions (1 pull request, 1 issue)

  - **Interpreter: programs that never finish.** A program defined in a way that can
    never settle on a value made the tool loop forever. pt7k's trace showed the endless
    unfolding, and pt7k suggested rejecting such programs with a clear error message,
    which Tau now does.

    - 2025-01-15 - [#20](https://github.com/IDNI/tau-lang/issues/20) - REPL runner hangs when trying to run a function that assigns another recurrence to the output stream
      - Diagnostic and proposed solution: a debug trace exposing the endless unfolding of the free offset `n`, and the suggestion to reject such programs with an error message, which was adopted.
      - commits: [824062ee](https://github.com/IDNI/tau-lang/commit/824062eebfba1bb9824bde48718eae29fe328a5f) Reject definitions whose head cannot bind a relative offset (issue-20)

  - **Documentation.** Improved the README.

    - 2025-03-22 - [#25](https://github.com/IDNI/tau-lang/pull/25) - Update README.md
      - commits: [4878a790](https://github.com/IDNI/tau-lang/commit/4878a790382af7837475c2c00142c5dcccc5f368) Update README.md

- **Mikolaj Konarski** ([@Mikolaj](https://github.com/Mikolaj)): 1 contribution (1 pull request)

  - **Documentation.** Fixed broken formatting in the README.

    - 2025-03-21 - [#24](https://github.com/IDNI/tau-lang/pull/24) - Fix two cases of broken formatting in README.md
      - commits: [866b2344](https://github.com/IDNI/tau-lang/commit/866b234415ed1e3db013b314ee2a114cb654f58f) Fix two cases of broken formatting in README.md

## Numbers

- Merged pull requests: 2
- Issues counted as contributions: 1
- Distinct contributors: 2
- First-time contributors: 1 (Mikolaj Konarski)

# Q4 2024

- **David Andel** ([@madsurgeon](https://github.com/madsurgeon)): 1 contribution (1 pull request)

  - **Documentation.** Fixed typos and grammar in the README and the developer guide.

    - 2024-11-26 - [#10](https://github.com/IDNI/tau-lang/pull/10) - Improve README and DEVELOPMENT: fixed typos and grammar
      - commits: [3c62d79c](https://github.com/IDNI/tau-lang/commit/3c62d79c1523df57feed0d46a8db195a4ef279a1) Improve README and DEVELOPMENT: fixed typos and grammar

- **pt7k** ([@pt7k](https://github.com/pt7k)): 1 contribution (1 pull request)

  - **Documentation.** Improved the README.

    - 2024-12-09 - [#11](https://github.com/IDNI/tau-lang/pull/11) - Update README.md
      - commits: [dbf17eb2](https://github.com/IDNI/tau-lang/commit/dbf17eb25922a85d75ccded405885af6683267c6) Update README.md

## Numbers

- Merged pull requests: 2
- Issues counted as contributions: 0
- Distinct contributors: 2
- First-time contributors: 2 (David Andel, pt7k)

# Q1 2024

- **m4nki** ([@m4nki](https://github.com/m4nki)): 1 contribution (1 pull request)

  - **Documentation.** Fixed typos in the README.

    - 2024-03-28 - [#2](https://github.com/IDNI/tau-lang/pull/2) - Fixing typos
      - commits: [7bf51acf](https://github.com/IDNI/tau-lang/commit/7bf51acfd24aa295bd93e6d30a23c1df1c2a0851) Update README.md, [5eb01225](https://github.com/IDNI/tau-lang/commit/5eb01225393ca3b7412f36bd717f05fe54d5e98a) Update README.md

## Numbers

- Merged pull requests: 1
- Issues counted as contributions: 0
- Distinct contributors: 1
- First-time contributors: 1 (m4nki)

# Q4 2023

- **Tomáš Klapka** ([@tklip](https://github.com/tklip)): 1 contribution (1 pull request)

  - **Parser and build: syncing with the parser library.** Updated the project to the
    latest version of its parser library and reorganised the build to match.

    - 2023-12-16 - [#1](https://github.com/IDNI/tau-lang/pull/1) - Parser sync
      - commits: [e8ad1818](https://github.com/IDNI/tau-lang/commit/e8ad18185431345f99f823d994f983f8cd6b2074) update parser library, [1746af05](https://github.com/IDNI/tau-lang/commit/1746af05c59ed986d87e68bb28ebf8ec31c0e5ce) fix warnings, [1f059ef7](https://github.com/IDNI/tau-lang/commit/1f059ef70e2c2100f4630ccb57b61430486b447e) CMakeLists: include common fns after submodul's, [a90d8648](https://github.com/IDNI/tau-lang/commit/a90d864875006f912bc9622b69245f17a0050b55) cmake refactoring, [6a6d66e6](https://github.com/IDNI/tau-lang/commit/6a6d66e62843f68ab58888d1d37e9215642b246e) regenerate parsers renamed empty_string to nul, [c09295a6](https://github.com/IDNI/tau-lang/commit/c09295a6608357fda095d7ef519af6938f4ccc7b) link parser lib only to tests which requires it, [ceeaa011](https://github.com/IDNI/tau-lang/commit/ceeaa011c11e7f238eef7b0d817ba714674f9385) -DGENERATE_PARSER=1 to gen parsers + fix build.sh exit status + test scripts accept args, [6c47195d](https://github.com/IDNI/tau-lang/commit/6c47195d2ed15618816e4daa681caf6badb71a04) cmake more refactoring, [dbb4f3fe](https://github.com/IDNI/tau-lang/commit/dbb4f3fe97cdb774abe31227011cfea223f617b7) update parser lib submodule, [328589b3](https://github.com/IDNI/tau-lang/commit/328589b3929607443af7c8cf1771537c77497e5c) cmake fix include paths, [13b61ed4](https://github.com/IDNI/tau-lang/commit/13b61ed4360817d30e630811bc315bf9a411c850) fix inputs => input* EBNF and regen parsers... with up-to-date parser gen, [3a06ae96](https://github.com/IDNI/tau-lang/commit/3a06ae96f73597c96db82db3bbdcbaaee3544659) show parse errors + fix test_bindings parse errors, [abfc037a](https://github.com/IDNI/tau-lang/commit/abfc037af67e200db865f53b2733ff31b6d63576) .gitignore: added .local, [1db58726](https://github.com/IDNI/tau-lang/commit/1db58726088453f6e0b4a5005e91365fce1a8923) Merge branch 'main' into parser_sync, [bb2a21e7](https://github.com/IDNI/tau-lang/commit/bb2a21e714f64b946794554895cc6b5b83c987c7) merge from main, [03fc0d2a](https://github.com/IDNI/tau-lang/commit/03fc0d2aba0ce3911abd8b5917162bb44a945479) update parser lib

## Numbers

- Merged pull requests: 1
- Issues counted as contributions: 0
- Distinct contributors: 1
- First-time contributors: 1 (Tomáš Klapka)

# All time

- Merged pull requests: 24
- Issues counted as contributions: 67
- Distinct contributors: 12

| Quarter | Merged PRs | Issues | Contributors | First-time contributors |
|---|---:|---:|---:|---:|
| Q4 2026 | 1 | 1 | 1 | 0 |
| Q3 2026 | 12 | 59 | 6 | 3 |
| Q2 2026 | 1 | 0 | 1 | 0 |
| Q1 2026 | 2 | 2 | 3 | 1 |
| Q4 2025 | 1 | 4 | 3 | 2 |
| Q2 2025 | 1 | 0 | 1 | 0 |
| Q1 2025 | 2 | 1 | 2 | 1 |
| Q4 2024 | 2 | 0 | 2 | 2 |
| Q1 2024 | 1 | 0 | 1 | 1 |
| Q4 2023 | 1 | 0 | 1 | 1 |
