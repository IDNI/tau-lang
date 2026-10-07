# Adding a Boolean algebra to Tau

A Boolean algebra (BA) is a plugin: one directory holding its value type, its
constant parsing, and a **descriptor** — the single compile-time interface core
uses to reach it. Which BAs a build contains is chosen at configure time with
`-DTAU_BAS=`. Core names no BA, so adding one touches no file outside its own
directory (plus, out-of-tree, a single registration call).

Start by copying `src/boolean_algebras/_template/`, a working two-element BA;
its `README.md` is the short version of this page.

## The pack

The BAs of a build are its *pack*, resolved at configure time:

```bash
./dev preset devel                           # the default pack
./dev preset devel -DTAU_BAS=tau,sbf,bv      # a smaller one
./dev preset devel -DTAU_BAS=sbf,tau         # smallest useful pack
```

`cmake/tau_bas.cmake` globs `src/boolean_algebras/*/ba.cmake`, resolves the
listed ids, and generates `tau_pack.h` into the build tree:

| the header provides | what for |
|---|---|
| `tau_pack::node_t` | the node type of the configured pack |
| `TAU_PACK_BASE_BAS` | the base BAs, for templates that build `tau_ba<BAs...>` |
| `TAU_PACK_FULL_BAS` | the full variant list, wrapper included; use for `node<...>` |
| `TAU_PACK_HAS_BA_<ID>` | one define per enabled BA, for tests and the pre-instantiation lists only; core never branches on it |

`tau` is a reserved id: the wrapper BA embedding a whole Tau spec. When listed,
the resolver emits `node<tau_ba<base...>, base...>`. Only `tau_ba` implements
`pack`/`unpack`, the tree/value bridge that lets core pull an embedded spec
out of, and back into, a constant — that pair is not part of the generic
descriptor contract below, since every other BA has nothing to bridge.

At runtime the pack answers for itself: `node::ba::types()` returns the
descriptors' `type_name`s in pack order, `type_names()` the same as a
`constexpr std::array<std::string_view, N>`, and `types_joined()` them
comma-joined as a `string_view` over a `constexpr` buffer. A spec annotating a
type the pack does not own is rejected against that list — including a
parameterized one, matched by family name, so `:bv[8]` needs `bv` in the pack.
`tau --version` prints the same list.

## The targets a BA does not support

A manifest may name the dependency-store targets the algebra cannot build
for, with a reason each:

```cmake
set(TAU_BA_UNSUPPORTED_TARGETS wasm32-emscripten)
set(TAU_BA_UNSUPPORTED_REASON_wasm32-emscripten "nondistributable: cvc5 links GMP")
```

The target names are `linux-x86_64`, `linux-arm64`, `darwin-arm64`,
`darwin-x86_64`, `windows-x86_64-mingw`, `windows-x86_64-msvc` and
`wasm32-emscripten`. When `-DTAU_BAS` is unset, every BA whose
manifest names the current target is dropped from the default pack and its
reason is printed:

```
-- skipping bv on wasm32-emscripten: nondistributable: cvc5 links GMP
```

Naming such a BA in an explicit `-DTAU_BAS=` is a fatal error carrying the same
reason. The manifest stays in place, so a target that supports the BA still
gets it. `bv` and `hsb` name `wasm32-emscripten` because cvc5 links the host
GMP, so a wasm build is not distributable; `nlang` names `wasm32-emscripten`
because curl has no wasm port.

## What a BA must provide

### The value type

Any type with the Boolean operators (`operator~`, `&`, `|`, `^`), plus
three things generic core requires of every alternative in the constants
variant:

- **equality** — core compares constants directly;
- **comparison with `bool`** — `your_ba == true` must compile, since core
  compares constants against plain truth values. It answers true only when
  that is decided: a value you cannot decide equals neither `true` nor
  `false`, so `!=` is true for both. The operator has no way to say why;
  the descriptor's `is_one` and `is_zero` return a `result` and carry the
  report;
- **`operator<<`** — the tree printer streams whichever alternative a constant
  holds;
- **`std::hash`** — constants live in a hashed variant.

All four are checked by the descriptor concept, so a missing one is reported
with the BA named instead of failing deep inside `std::variant`.

The Boolean operators are total. Each operator returns a value of the
algebra type, never a `result`. This keeps every Boolean expression
composable: an algebra that cannot decide an operator falls back to a
defined value instead. An algebra that must report why a decision failed
reports it through a descriptor member instead, such as `normalize` or
`is_zero`.

### The descriptor

Specialize `ba_descriptor<your_ba, node<PackBAs...>>` in your own header.
`ba_descriptor_complete` in `src/boolean_algebras/ba_descriptor.h` lists the
mandatory members, one `requires` per line, so an omitted member is reported
against the line naming it:

- **identity and classification** — `type_name`, `default_type_priority`,
  `atomless`, `non_aba_omcat`
- **type system** — `matches_type` (by tree), `type_tree`, `owns_type` (by
  id)
- **constants** — `is_one`, `is_zero`, `is_closed` (each returns a
  `result<bool>`, described below), plus `is_syntactic_one`,
  `is_syntactic_zero`, `literal_one`, `literal_zero` (plain `bool` or
  `string`, because these never run a decision procedure)
- **normalization** — `normalize` and `splitter` (the latter joins when
  `atomless`; see the optional table), each returning a `result` as described
  below
- **rewriting** — `simplify_symbol` (plain `tref`), `simplify_term` (returns
  a `result<tref>`, described below)
- **parsing** — `parse`

Declare `atomless = false` whenever your algebra has an atom, even if it is
infinite. Core then never distributes a quantifier over several disequations
(`ex x (A && B) == ex x A && ex x B` for negated atoms), a law that holds only
without atoms (`pack_type_is_atomic`). It keeps that law's `F`, which holds in
any Boolean algebra, asks your `omcat_qe` otherwise, and keeps the binder
when neither decides.

Declare `non_aba_omcat = true` only for a theory whose variables denote
points of an ω-categorical structure rather than elements of a Boolean
algebra, its constants being sets of such points (qlt). Core then hands every
binder of your type to `omcat_qe` and `omcat_qe_residual`, and keeps the
binder when both decline: no Boolean-algebra law reaches it, since
`ex x (x = c)`, for one, holds only when `c` is a point. `solve` asks your
`omcat_solve_inequality_system` for every system of your type, in every mode,
and both `solve` and the substitution heuristics put a variable of your type
equal to a term only when the term is a point: another variable, or a
constant your `dense_order_compare` orders against itself.
A comparison with a constant that is not such a point is not read with the
laws of a total order (`order_is_total`): `a >= b` stays `b <= a` rather than
`!(a < b)`, and `a <= a` is left to your hooks.

`is_one`, `is_zero`, `is_closed`, `normalize`, `splitter`, and
`simplify_term` can each run a full decision or rewrite procedure. Each one
can fail. Give each the same
result-carrying shape that `preprocess` uses (see
[Preprocessing](#preprocessing)). On success, the result holds a value. On
failure, the result holds no value, only a report that names the reason. A
BA whose own check cannot fail, such as a plain field comparison, still
returns a result. Wrap the value with `result<bool>{...}`,
`result<your_ba>{...}` or `result<tref>{...}` and add no report.

Every other mandatory member keeps a fixed, non-`result` signature, such
as `type_name`. A BA that meets a failure there cannot
return a report, because the signature has no channel for one. Name the
operation that failed and the blocking contract instead:

```cpp
// TODO (HIGH) dropped error: <what fails> -- <the contract that blocks the report>.
```

Assert a whole pack at its first instantiation site:

```cpp
static_assert(assert_pack_descriptors_complete<my_node_t>());
```

That the members exist is checked when they compile; that they *behave* is
checked by `tests/unit/test_ba_conformance.cpp`, which folds over the configured
pack and runs the same battery against every algebra in it — yours included, and
an out-of-tree one too. It exercises the type-system round trips, the literals
and what they parse back to, the predicate implications, and the Boolean laws
over normalized values. Nothing there names an algebra, so joining a pack is all
it takes to be covered.

A BA whose value type is an alias for a type in **another namespace** must put
its free operators there too, not in `idni::tau_lang`. `ba_descriptor_complete`
checks `x == b` from a definition context that precedes your header, so the
operator is reachable only by ADL — and ADL follows the value type's own
namespace. `bv` (an alias for `cvc5::Term`) declares its bool comparisons in
namespace `cvc5` for exactly this reason. A BA defining its own struct, or one
whose alias names a template with an `idni::tau_lang` argument, is unaffected.

### Optional capabilities

Anything beyond the mandatory surface is an **optional capability**: core probes
for it with a named concept (`ba_has_<capability>` in `ba_descriptor.h`) and
never by BA name, so declaring one is how you opt in. Omit any that does not
apply. The folds live in `ba_pack_traits.h` as `pack_*`, except the three that
need solver or LTL types, which sit beside their single consumer:
`omcat_solve_inequality_system` (`solver.tmpl.h`), `try_propositional_synthesis`
(`ltl_aba_builders.tmpl.h`) and the comparison hooks (`hooks_wff.tmpl.h`).

| member | what core asks it for | resolution |
|---|---|---|
| `solve(fm)` | your own decision procedure for a whole formula of your types. Returns a `result` whose value converts to the caller's `optional<solution>`: nullopt when you find no solution, an error when you could not try (an error is never read as "no solution") | owner of the formula's type |
| `can_solve(fm)`, `sat_status(fm)` | whether you can decide `fm`; a *definite* answer as `optional<bool>`, so "unknown" stays distinct from "unsat" | any declarer / first definite answer |
| `preprocess(fm)`, `set_preprocessing(bool)` | a rewriting pass before solving, and its switch. A failure reports the reason (see [Preprocessing](#preprocessing)) | every declarer, chained in pack order, stopping at the first failure |
| `case_split_quantifiers(fm)` | eliminate your quantified variables tested only against constants by a finite case split, before any quantifier block forms | every declarer, chained in pack order |
| `eliminate_definitional_existentials(fm)` | substitute your existentially quantified variables that a total definition in their scope determines and drop the binder, before the case split | every declarer, chained in pack order |
| `widen_arithmetic(fm)` | elaborate your arithmetic atoms to an overflow-free width before solving. Returns a `result<tref>`. A failure reports the reason, the same shape as `preprocess` | every declarer, chained in pack order, stopping at the first failure |
| `widening_state()` | whether your widening is currently on, for a caller that must key a cache on it (your own construction-time hooks read it too, not just `widen_arithmetic`) | any declarer active |
| `formula_is_preprocessable(fm)`, `has_preprocessing_residue(fm)` | whether your pass can still make progress / left a shape closing would make expensive | any declarer |
| `term_is_blasteable(term)` | whether a term with an arithmetic operator can be blasted | owner of the term's type |
| `arith_ops` | that the grammar's arithmetic term operators apply to your type | owner |
| `zero_constant(ba_type)`, `value_constant(ba_type, v)` | the type's default zero, when it is not `bf_f`; a constant holding a plain integer | owner |
| `modular_width(ba_type)`, `modular_value(ba_type, c)` | that the type's values are the integers below 2^n with unsigned modular semantics (bitwise Boolean operators, `+ - *` modulo 2^n, unsigned `/ %` and comparisons, logical shifts), and the integer a constant holds; 0 / `nullopt` when not. The data game then plays such a stream on its n bits, and normalization folds a disjunction pinning one variable to every value of the type to `T` (dually, a conjunction excluding every value to `F`) (bv declares it, answering 0 while widening is on) | owner |
| `decide_closed(form)` | the truth of a closed formula over the type whatever its quantifier prefix, with no quantifier eliminated first; `nullopt` when undecided. The data game then keeps, as the regions of a game over streams of that one type, the formulas whose quantifiers the normalizer leaves standing, and decides them whole (bv declares it) | owner |
| `decide_ground(form)` | the truth of a formula without variables, streams or temporal operators whose constants are all of the type, when normalization leaves it standing because its comparisons cannot be decided one at a time; `nullopt` when undecided. Normalization then replaces it by `T` or `F` (qlt declares it, for named endpoints) | owner |
| `dense_order_compare(ba_type, a, b)` | that the type's values form a dense linear order without endpoints, read by `=` and the order comparisons, and the order (-1, 0, 1) of two constants, `nullopt` for one that is no point of the order. The data game then codes such streams by the order type of their window (qlt declares it) | owner |
| `can_host_bool`, `bool_carrier_type()` | that one of your types holds a plain 0/1, and which when that is not your `type_tree()` (bv answers `bv[1]`); a carrier must also declare `value_constant` | ranked by `TAU_BOOL_CARRIERS`, pack order as tie-break |
| `exact_constant(x)` | false when a constant over-approximates its value (one depending on an unknown part, such as qlt's named endpoints); a fold of two constants into such a value keeps its term instead, and its comparison with 0 or 1 is left to your `=`/`!=` hooks, kept when they decline (qlt declares it) | the constant's own alternative |
| `omcat_qe(var, body)` | decide a quantifier over your own theory, for every value of the other free variables; `nullopt` asks `omcat_qe_residual` (a `non_aba_omcat` type) or falls through to the atomless path | owner |
| `omcat_qe_residual(var, body)` | a quantifier-free formula equivalent to `ex var. body` when its truth depends on the other variables, which `omcat_qe` can only answer as undetermined (qlt turns `ex x (a < x && x < b)` into `a < b`, and `ex x (x > y && {[0,1]}:qlt & x != 0)` into `{ (-inf, 1) }:qlt & y != 0`); `nullptr` keeps the binder | owner |
| `omcat_solve_inequality_system(sys, opts)` | a model of a system over your theory, every one of a `non_aba_omcat` type (qlt answers with one point per variable), as a `result<std::optional<solution>>`: `nullopt` when no model exists, an error when the system cannot be decided | owner |
| `try_propositional_synthesis(fm, atoms)` | synthesise a propositional strategy for your own atoms | the single declarer |
| `semantic_pwr_optimal(clause, update)` | revise a clause through your winning region | first declarer that answers |
| `codegen_witness(var, conj)`, `codegen_constant_expr(cst)` | C++ spellings of a witness / a constant for generated code; `codegen_constant_expr` returns a `result<std::optional<std::string>>`: `nullopt` when you have no spelling for the constant, an error when building one failed | owner |
| `output_always_satisfiable_by_system` | that a system can always meet an output constraint by choosing its output | owner |
| `literal_incomplete(src)` | whether a partly-typed literal is truncated rather than malformed, so the REPL keeps reading | owner, by type tree |
| `print_constant(os, x)`, `hash_constant(x)` | how to render / hash a constant when your own `operator<<` / `std::hash` are not what Tau should use (bv prints SMT-LIB and hashes by creation id). `hash_constant` must return `std::uint64_t`, the same value on every platform, because `std::hash` returns `size_t`. Another return type fails `ba_descriptor_complete` and a `static_assert` in `node::hashit`. On x86_64 Linux `size_t` is `std::uint64_t`, so there a `size_t` result still passes | the constant's own alternative, at the point of use |
| `constant_size(x)` | how many tree nodes a constant carries when operations on constants build ever larger ones (the wrapper embeds a whole spec); `max_constant_size` bounds the values the solver builds by it | the constant's own alternative, at the point of use |
| `options()` | your CLI/REPL options, addressed as `<family>-<name>` (see below) | per family |
| `set_charvar(bool)` | keep your grammar in step with core's var/charvar mode | every declarer |
| `set_ba_component_factoring(bool)`, `ba_component_factoring_enabled()` | your own component-factoring switch; today only the wrapper declares one | every declarer / any |
| `set_ba_decision_pins(size_t)`, `ba_decision_pins()` | your own cap on the decided rows kept alive across a sweep; today only the wrapper declares one | every declarer / the declarer's, 0 when none |
| `type_param(tree)`, `type_id_for(param)`, `type_tree_for(param)` | declare all three iff your family is parameterised (`bv[8]`); `pack_type_tree` then accepts a parameter for your family and refuses one for every other, and inference defaults an under-specified type (a widthless `:bv`) to your own parameterised type | owner |
| `uses_oracle` | deciding a question leaves the process, so comparison-based checks (the conformance laws) skip you; absent means decided here | per BA |
| `splitter(x, kind)`, `splitter_one(tree)` | a proper sub-element of a constant / of the type's one, returned as a `result`; required only when `atomless`, though a BA that is not may still provide them (qlt does). For a BA without them the dispatcher returns the element itself / `nullptr`, so a caller checks `pack_type_is_atomless` before relying on a proper sub-element | the constant's own alternative / owner |

**Pack order is semantic** wherever the rule above says *first*, *any* or
*chained*: `-DTAU_BAS=a,b` and `-DTAU_BAS=b,a` can differ there. Owner-gated
members never depend on it, and the single-declarer member refuses a second
claimant at compile time so no build resolves it by order.

Declaring both `arith_ops` and `solve` is what makes core instantiate the
arithmetic pipeline (predicate blasting, the arithmetic skip, the theory
solver) for packs containing you; there is nothing else to switch on. The
theory solver hands your `solve` the atoms of a clause whose types you own, all
of them in one formula (a cast lets one variable span two of your types), so
several solving algebras can share a pack.

Every fold's empty case is deliberate. `pack_zero_constant` and
`pack_value_constant` return `nullptr`, `pack_type_has_arith_ops` returns
`false`, and `pack_solve` returns the value `nullopt` (no owner, or an owner
without `solve`), because "no BA owns this type" is an ordinary runtime outcome;
`pack_bool_carrier_type` `static_assert`s, because a pack with nothing to carry
a bit cannot build core at all. When writing one, test the capability's concept with
`if constexpr` inside `pack_visit_all` or `pack_owner_apply`: a `?:` in a fold
expression instantiates both arms for every BA, and a `requires`-expression
nested in the fold's lambda crashes gcc 13.

### Options

`options()` returns a range of `ba_option` (`ba_descriptor.h`): a bare `name`,
a `kind` -- `flag`, which the REPL's `set` also accepts as enable/disable/toggle,
`count`, which takes a number, or `text`, which takes a word -- a getter and a
setter, and a help string.
The REPL and CLI address it as `<family>-<name>` (`bv-blasting`), `<family>`
being your `type_name`, so every width of a parameterised family shares one
option set; `pack_find_ba_option` tells "no such family" from "no such option"
so each gets its own message. The getter and setter are function pointers to
process-wide storage of your own, so every pack in one process shares the
value. A switch that gates a preprocessing pass also needs core's master
`preprocessing` switch on: `bv-blasting` is the example.

A `text` option fills `get_text` and `set_text`, which follow `help` in the
struct, and leaves the four numeric accessors null. Its setter returns `false`
for a word it does not take, and the REPL, the CLI and the API then report an
invalid value; let an empty text clear the option. A getter returns what a
reader may see: an option that holds a secret answers `set` or `unset`
(`nlang-api-key`). The REPL grammar gives a value letters, digits and
`. - _ : /`, optionally in double quotes (a host name needs them, since a
`.` also separates commands). The API reaches a text option through `set_ba_text_option` and
`get_ba_text_option`, and the numeric pair refuses it. The value joins
`pack_ba_options_fingerprint` like a flag or a count does.

A `flag` is written back from the command line only when the given value
differs from the one in force, and a `count` or `text` option only when its flag is actually given on the
command line, so a getter is free to resolve an environment fallback of its
own and the CLI will not shadow it with the option's default. Every count
option has one, named `TAU_<FAMILY>_<NAME>` with dashes as underscores
(`bv-defelim-max-atoms` reads `TAU_BV_DEFELIM_MAX_ATOMS`). The shortest way is
to store the option in an `env_limit<size_t>` (`env_limits.h`): the setter
assigns it, the getter reads it, and it resolves option > environment >
default like core's own limits do (`bv_defelim_max_atoms` is the example).
A getter that must re-read the variable on every call reads it with
`env_limit_count` instead and keeps the setter writing a parameter it prefers
when set (`qlt-t3-cap`, `TAU_QLT_T3_CAP`). Name the variable and the default
in the help string: the CLI registers the option with an empty default, so
`--help` shows the help string alone.

### Rewrite hooks

Capabilities answer questions; **hooks rewrite trees**, and they are a separate
mechanism. `ba_descriptor.h` declares

```cpp
template <typename BA, typename Node> struct ba_term_hooks {};
template <typename BA, typename Node> struct ba_wff_hooks {};
```

*defined and empty*, unlike `ba_descriptor` — so specialize neither, one, or
both, in your own `<id>_ba_hooks_ext.tmpl.h` (see `_template/`), included from
your descriptor header. `ba_wff_hooks` takes `wff_lt`, `wff_nlt`, `wff_lteq`,
`wff_nlteq`, `wff_gt`, `wff_ngt`, `wff_gteq`, `wff_ngteq`, `wff_eq`, `wff_neq`;
`ba_term_hooks` takes `term_cast`.

Each returns `nullptr` to decline. **Declining is not the same as having no
hook**: for an ordering operator core asks `pack_ba_type_has_wff_lt_hook`
separately, and when your type owns the operator but you declined, it preserves
the comparison as an atom rather than falling through to the generic Boolean
definition. So return `nullptr` freely for operands you cannot fold — the atom
survives for the solver. `wff_eq` and `wff_neq` differ: every algebra has the
Boolean equation, so when you decline core's own equality rules go on (qlt
uses them to decide `v = 1` for a point variable, since its typed 0 and 1 are
the order's ends rather than points).

A hook keeps a fixed signature too, so it cannot carry a report. A hook
that meets a failure there declines with `nullptr` and names the blocking
contract the same way.

### The manifest

`src/boolean_algebras/<id>/ba.cmake`, three lines and up. It is where the plugin
declares everything it owns — sources, grammar, suites — so no list elsewhere
names your algebra:

```cmake
set(TAU_BA_ID <id>)
set(TAU_BA_TYPE <value type>)
set(TAU_BA_HEADER boolean_algebras/<id>/<id>.h)
# set(TAU_BA_SOURCES boolean_algebras/<id>/<id>.cpp)   # if it has any
# set(TAU_BA_GRAMMAR parser/<id>.tgf)                  # if it parses constants
# set(TAU_BA_LINK_LIBS <target>)                       # if it needs a library
# set(TAU_BA_REQUIRES_PACKAGES <package>)              # found only for packs
                                                       # holding this BA
# set(TAU_BA_TESTS tests/test_<id>.cpp …)              # registered with the BA
```

Paths are relative to the manifest, so a plugin is one directory:
`<id>/parser/<id>.tgf` beside `<id>/tests/`. Dependencies are found and linked
only when the BA is in the pack, its grammar is generated into the build tree
only then, and its suites are registered only then.

A suite of yours that also needs *another* algebra says so, one line per suite:

```cmake
set(TAU_BA_TEST_REQUIRES_test_<id>_mixed bv qlt)
```

Without every named algebra in the pack that suite is skipped with a message,
while your other suites still run. Nothing infers this from the source, so a
requirement hiding in a fixture header or an `#include <cvc5/…>` must be
declared — the exception being external packages, which are read from the
includes.

## Out-of-tree

Keep the directory anywhere and register it before the pack resolves:

```cmake
tau_register_ba(<id>
    PATH    /abs/path/to/<id>
    HEADER  <id>.h
    TYPE    <value type>
    GRAMMAR parser/<id>.tgf
    TESTS   tests/test_<id>.cpp)
```

Pass that file to the configure as `-DTAU_EXTERNAL_BAS=/abs/path/register.cmake`.
`tests/external_ba/` is a complete working example, built and run in one command
by `scripts/test-external-ba.sh`, which configures from scratch, builds every
suite the pack can run, and checks the algebra against the descriptor contract
before smoke-running the CLI.

## Constants and grammar

`parse` receives the source text of the constant and its type tree. It
returns a `result<constant_with_type>`.

On success, the result holds the parsed constant. On refusal, the result
holds no value, only a report that names the reason.

Add the reason with `r.error(code::parse_error, "the reason")`, then return
`r`. Do not log the same reason too. The BA reports the reason. Core prints
the report.

```cpp
static result<typename node_t::constant_with_type>
parse(const std::string& src, tref)
{
    result<typename node_t::constant_with_type> r;
    if (src != "0" && src != "1") {
        r.error(code::parse_error, "Not a valid my_ba literal: " + src);
        return r;
    }
    return r.with_value(typename node_t::constant_with_type{
        typename node_t::constant{ my_ba{ src == "1" } },
        my_ba_type<node_t>() });
}
```

A BA with non-trivial literal syntax gets its own `.tgf` grammar under
`parser/`. Build tools compile the grammar ahead of time. `./dev regen`
regenerates it. See `sbf.tgf` or `qint.tgf` for an example.

## Preprocessing

`preprocess` is an optional capability. Declare it only when your BA
rewrites a formula before solving, as bv does for predicate blasting.
Omit it otherwise.

`preprocess` receives the whole formula. It returns a `result<tref>`.

On success, the result holds the formula, rewritten or unchanged. On
failure, the result holds no value, only a report that names the reason.

Add the reason with `r.error(code::internal_error, "the reason")`. Return
`r`. Do not log the same reason too. The BA reports the reason. Core
prints the report.

`pack_preprocess` chains every declaring BA's `preprocess`, in pack order.
It stops at the first failure and carries that report forward.

## Dispatch

Core reaches a BA only through its descriptor: `base_ba_dispatcher` folds over
the pack's descriptors, and constant parsing walks them until one owns the type.
There are no hand-written per-pack dispatchers: the one generic dispatcher
serves every pack, default or reduced.
