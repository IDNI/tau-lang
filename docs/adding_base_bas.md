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
./dev preset debug                           # the default pack
./dev preset debug -DTAU_BAS=tau,sbf,bv      # a smaller one
./dev preset debug -DTAU_BAS=sbf,tau         # smallest useful pack
```

`cmake/tau_bas.cmake` globs `src/boolean_algebras/*/ba.cmake`, resolves the
listed ids, and generates `tau_pack.h` into the build tree:

| the header provides | what for |
|---|---|
| `tau_pack::node_t` | the node type of the configured pack |
| `TAU_PACK_BASE_BAS` | the base BAs, for templates that build `tau_ba<BAs...>` |
| `TAU_PACK_FULL_BAS` | the full variant list, wrapper included; use for `node<...>` |
| `tau_pack::type_names` | the pack's type names, in pack order |
| `TAU_PACK_HAS_BA_<ID>` | one define per enabled BA |

`tau` is a reserved id: the wrapper BA embedding a whole Tau spec. When listed,
the resolver emits `node<tau_ba<base...>, base...>`.

## What a BA must provide

### The value type

Any type with the Boolean operators (`operator~`, `&`, `|`, `^`, `+`), plus
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
- **type system** — `matches_type`, `type_tree`, `owns_type` (by tree and by
  id), `type_param`, `type_id_for`, `type_tree_for`
- **constants** — `is_one`, `is_zero`, `is_syntactic_one`, `is_syntactic_zero`,
  `is_closed`, `literal_one`, `literal_zero`
- **normalization** — `normalize`, `splitter`, `splitter_one`
- **tree bridge** — `pack`, `unpack` (only the wrapper BA does anything here)
- **rewriting** — `simplify_symbol`, `simplify_term`
- **parsing** — `parse`

Assert a whole pack at its first instantiation site:

```cpp
static_assert(assert_pack_descriptors_complete<my_node_t>());
```

A BA whose value type is an alias for a type in **another namespace** must put
its free operators there too, not in `idni::tau_lang`. `ba_descriptor_complete`
checks `x == b` from a definition context that precedes your header, so the
operator is reachable only by ADL — and ADL follows the value type's own
namespace. `bv` (an alias for `cvc5::Term`) declares its bool comparisons in
namespace `cvc5` for exactly this reason. A BA defining its own struct, or one
whose alias names a template with an `idni::tau_lang` argument, is unaffected.

### Optional capabilities

Anything beyond the mandatory surface is an **optional capability**: core probes
for it with `requires` and never by BA name, so declaring one is how you opt in.
Omit any that does not apply. The folds live in `ba_pack_traits.h` as `pack_*`.

| member | what core asks it for |
|---|---|
| `solve`, `is_sat` | your own decision procedure for a whole formula |
| `can_solve` | whether a formula is one you can decide at all |
| `sat_status` | a *definite* answer — `optional<bool>`, so "unknown" stays distinct from "unsat" |
| `preprocess`, `set_preprocessing` | a rewriting pass to run before solving |
| `zero_constant(ba_type)` | the type's default zero, when it is not `bf_f` |
| `value_constant(ba_type, value)` | a constant of that type holding a plain integer value |
| `literal_incomplete(src)` | whether a partly-typed literal is truncated rather than malformed, so the REPL keeps reading |
| `arith_ops` | that the grammar's arithmetic term operators apply to your type |
| `can_host_bool` | that one of your types can hold a plain 0 or 1, making you a candidate Boolean carrier |
| `bool_carrier_type()` | *which* of your types that is, when it is not your `type_tree()` — bv answers `bv[1]`, not its default `bv[16]` |
| `print_constant(os, x)` | how to render a constant, when your own `operator<<` formats it in a way Tau should not show |

Also specialize `ba_has_arithmetic_theory<your_ba>` (in `ba_pack_traits.h`) when
the algebra brings arithmetic terms *and* its own decision procedure — that is
what makes core instantiate the arithmetic pipeline for packs containing you.

Every fold's empty case is deliberate. `pack_zero_constant` and
`pack_value_constant` return `nullptr`, and `pack_type_has_arith_ops` returns
`false`, because "no BA owns this type" is an ordinary runtime outcome;
`pack_solve` `static_assert`s, because its call sites are gated and reaching it
means a gate drifted. When writing one, put `if constexpr` inside a per-element
lambda: a `?:` in a fold expression instantiates both arms for every BA.

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

`src/boolean_algebras/<id>/ba.cmake`, three lines and up:

```cmake
set(TAU_BA_ID <id>)
set(TAU_BA_TYPE <value type>)
set(TAU_BA_HEADER boolean_algebras/<id>/<id>.h)
# set(TAU_BA_SOURCES boolean_algebras/<id>/<id>.cpp)   # if it has any
# set(TAU_BA_LINK_LIBS <target>)                       # if it needs a library
```

Dependencies declared here are linked only when the BA is in the pack.

## Out-of-tree

Keep the directory anywhere and register it before the pack resolves:

```cmake
tau_register_ba(<id>
    PATH   /abs/path/to/<id>
    HEADER <id>.h
    TYPE   <value type>)
```

Pass that file to the configure as `-DTAU_EXTERNAL_BAS=/abs/path/register.cmake`.
`tests/external_ba/` is a complete working example, built and run in one command
by `scripts/test-external-ba.sh`.

## Constants and grammar

`parse` receives the constant's source text and its type tree and returns a
`constant_with_type`. A BA with non-trivial literal syntax gets its own `.tgf`
grammar under `parser/`, compiled ahead of time and regenerated with
`./dev regen` — see `sbf.tgf` or `qint.tgf`.

## Dispatch

Core reaches a BA only through its descriptor: `base_ba_dispatcher` folds over
the pack's descriptors, and constant parsing walks them until one owns the type.
Packs whose BAs are not all described yet keep hand-written specializations in
`base_ba_dispatcher_*.cpp`; those disappear as each BA gains a descriptor.
