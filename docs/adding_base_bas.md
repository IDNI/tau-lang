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

Anything beyond that surface is an **optional capability**, found by
`requires`-probing rather than by BA name — declare `can_host_bool` to host the
pack's `true`/`false`, or specialize `ba_has_arithmetic_theory<your_ba>` (in
`ba_pack_traits.h`) when the algebra brings arithmetic terms and its own
decision procedure, which is what gates the bitvector machinery in core.

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
