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

## What a BA must provide

### The value type

Any type with the Boolean operators (`operator~`, `&`, `|`, `^`), plus
three things generic core requires of every alternative in the constants
variant:

- **equality** — core compares constants directly;
- **comparison with `bool`** — `your_ba == true` must compile, since core
  compares constants against plain truth values;
- **`operator<<`** — the tree printer streams whichever alternative a constant
  holds;
- **`std::hash`** — constants live in a hashed variant.

All four are checked by the descriptor concept, so a missing one is reported
with the BA named instead of failing deep inside `std::variant`.

### The descriptor

Specialize `ba_descriptor<your_ba, node<PackBAs...>>` in your own header.
`ba_descriptor_complete` in `src/boolean_algebras/ba_descriptor.h` lists the
mandatory members, one `requires` per line, so an omitted member is reported
against the line naming it:

- **identity and classification** — `type_name`, `default_type_priority`,
  `atomless`, `non_aba_omcat`
- **type system** — `matches_type` (by tree), `type_tree`, `owns_type` (by
  id)
- **constants** — `is_one`, `is_zero`, `is_syntactic_one`, `is_syntactic_zero`,
  `is_closed`, `literal_one`, `literal_zero`
- **normalization** — `normalize` (`splitter` and `splitter_one` join it when
  `atomless`; see the optional table)
- **rewriting** — `simplify_symbol`, `simplify_term`
- **parsing** — `parse`

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
| `solve(fm)` | your own decision procedure for a whole formula | the single declarer (two are refused at compile time) |
| `can_solve(fm)`, `sat_status(fm)` | whether you can decide `fm`; a *definite* answer as `optional<bool>`, so "unknown" stays distinct from "unsat" | any declarer / first definite answer |
| `preprocess(fm)`, `set_preprocessing(bool)` | a rewriting pass before solving, and its switch | every declarer, chained in pack order |
| `formula_is_preprocessable(fm)`, `has_preprocessing_residue(fm)` | whether your pass can still make progress / left a shape closing would make expensive | any declarer |
| `term_is_blasteable(term)` | whether a term with an arithmetic operator can be blasted | owner of the term's type |
| `arith_ops` | that the grammar's arithmetic term operators apply to your type | owner |
| `zero_constant(ba_type)`, `value_constant(ba_type, v)` | the type's default zero, when it is not `bf_f`; a constant holding a plain integer | owner |
| `can_host_bool`, `bool_carrier_type()` | that one of your types holds a plain 0/1, and which when that is not your `type_tree()` (bv answers `bv[1]`); a carrier must also declare `value_constant` | ranked by `TAU_BOOL_CARRIERS`, pack order as tie-break |
| `omcat_qe(var, body)` | eliminate a quantifier over your own theory; `nullopt` falls through to the atomless path | owner |
| `omcat_solve_inequality_system(sys, opts)` | solve a pure ordering system over your theory | owner |
| `try_propositional_synthesis(fm, atoms)` | synthesise a propositional strategy for your own atoms | the single declarer |
| `semantic_pwr_optimal(clause, update)` | revise a clause through your winning region | first declarer that answers |
| `codegen_witness(var, conj)`, `codegen_constant_expr(cst)` | C++ spellings of a witness / a constant for generated code | owner |
| `output_always_satisfiable_by_system` | that a system can always meet an output constraint by choosing its output | owner |
| `literal_incomplete(src)` | whether a partly-typed literal is truncated rather than malformed, so the REPL keeps reading | owner, by type tree |
| `print_constant(os, x)`, `hash_constant(x)` | how to render / hash a constant when your own `operator<<` / `std::hash` are not what Tau should use (bv prints SMT-LIB and hashes by creation id) | the constant's own alternative, at the point of use |
| `options()` | your CLI/REPL options, addressed as `<family>-<name>` (see below) | per family |
| `set_charvar(bool)` | keep your grammar in step with core's var/charvar mode | every declarer |
| `set_ba_component_factoring(bool)`, `ba_component_factoring_enabled()` | your own component-factoring switch; today only the wrapper declares one | every declarer / any |
| `type_param(tree)`, `type_id_for(param)`, `type_tree_for(param)` | declare all three iff your family is parameterised (`bv[8]`); `pack_type_tree` then accepts a parameter for your family and refuses one for every other, and inference defaults an under-specified type (a widthless `:bv`) to your own parameterised type | owner |
| `uses_oracle` | deciding a question leaves the process, so comparison-based checks (the conformance laws) skip you; absent means decided here | per BA |
| `splitter(x, kind)`, `splitter_one(tree)` | a proper sub-element of a constant / of the type's one; required only when `atomless`, though a BA that is not may still provide them (qlt does). For a BA without them the dispatcher returns the element itself / `nullptr`, so a caller checks `pack_type_is_atomless` before relying on a proper sub-element | the constant's own alternative / owner |

**Pack order is semantic** wherever the rule above says *first*, *any* or
*chained*: `-DTAU_BAS=a,b` and `-DTAU_BAS=b,a` can differ there. Owner-gated
members never depend on it, and the two single-declarer members refuse a second
claimant at compile time so no build resolves them by order.

Declaring both `arith_ops` and `solve` is what makes core instantiate the
arithmetic pipeline (predicate blasting, the arithmetic skip, the theory
solver) for packs containing you; there is nothing else to switch on.

Every fold's empty case is deliberate. `pack_zero_constant` and
`pack_value_constant` return `nullptr`, and `pack_type_has_arith_ops` returns
`false`, because "no BA owns this type" is an ordinary runtime outcome;
`pack_solve` `static_assert`s, because its call sites are gated and reaching it
means a gate drifted. When writing one, test the capability's concept with
`if constexpr` inside `pack_visit_all` or `pack_owner_apply`: a `?:` in a fold
expression instantiates both arms for every BA, and a `requires`-expression
nested in the fold's lambda crashes gcc 13.

### Options

`options()` returns a range of `ba_option` (`ba_descriptor.h`): a bare `name`,
a `kind` -- `flag`, which the REPL's `set` also accepts as enable/disable/toggle,
or `count`, which takes a number -- a getter and a setter, and a help string.
The REPL and CLI address it as `<family>-<name>` (`bv-blasting`), `<family>`
being your `type_name`, so every width of a parameterised family shares one
option set; `pack_find_ba_option` tells "no such family" from "no such option"
so each gets its own message. The getter and setter are function pointers to
process-wide storage of your own, so every pack in one process shares the
value. A switch that gates a preprocessing pass also needs core's master
`preprocessing` switch on: `bv-blasting` is the example.

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
`wff_nlteq`, `wff_gt`, `wff_ngt`, `wff_gteq`, `wff_ngteq`; `ba_term_hooks` takes
`term_cast`.

Each returns `nullptr` to decline. **Declining is not the same as having no
hook**: core asks `pack_ba_type_has_wff_lt_hook` separately, and when your type
owns the operator but you declined, it preserves the comparison as an atom
rather than falling through to the generic Boolean definition. So return
`nullptr` freely for operands you cannot fold — the atom survives for the solver.

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

`parse` receives the constant's source text and its type tree and returns a
`constant_with_type`. A BA with non-trivial literal syntax gets its own `.tgf`
grammar under `parser/`, compiled ahead of time and regenerated with
`./dev regen` — see `sbf.tgf` or `qint.tgf`.

## Dispatch

Core reaches a BA only through its descriptor: `base_ba_dispatcher` folds over
the pack's descriptors, and constant parsing walks them until one owns the type.
There are no hand-written per-pack dispatchers: the one generic dispatcher
serves every pack, default or reduced.
