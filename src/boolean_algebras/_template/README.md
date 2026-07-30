# Skeleton Boolean algebra

Copy this directory to start a new BA, in-tree or out-of-tree. `my_ba` is the
two-element Boolean algebra — the smallest thing that satisfies the descriptor
surface, so what remains after you strip it is exactly what a BA must provide.

## Files

| file | what it holds |
|---|---|
| `my_ba.h` | the value type, the parse declaration, and the includes that pull the rest in |
| `my_ba.tmpl.h` | the type tree naming the algebra, and constant parsing |
| `my_ba_descriptor.tmpl.h` | the descriptor — the only interface core uses |
| `my_ba_ba_hooks_ext.tmpl.h` | optional rewrite hooks; delete it if the algebra has none |

There is deliberately **no `ba.cmake` here**: `cmake/tau_bas.cmake` globs
`src/boolean_algebras/*/ba.cmake`, so a manifest in this directory would
register a phantom BA in every build. Write one when you copy the directory.

## In-tree

1. `cp -r src/boolean_algebras/_template src/boolean_algebras/<id>` and rename
   the files, the include guards, and `my_ba` to your value type.
2. Add `src/boolean_algebras/<id>/ba.cmake`:

   ```cmake
   set(TAU_BA_ID <id>)
   set(TAU_BA_TYPE <value type>)
   set(TAU_BA_HEADER boolean_algebras/<id>/<id>.h)
   # optional, when the algebra needs a library:
   # set(TAU_BA_LINK_LIBS <target>)
   # set(TAU_BA_SOURCES boolean_algebras/<id>/<id>.cpp)
   ```

3. Configure with the id in the pack: `./dev preset debug -DTAU_BAS=tau,sbf,<id>`.

Nothing else in the tree needs editing — the manifest is discovered by glob and
the descriptor is found by specialization.

## Out-of-tree

Keep the directory anywhere and register it before the pack is resolved:

```cmake
tau_register_ba(<id>
    PATH   /abs/path/to/<id>
    HEADER <id>.h
    TYPE   <value type>
    LINK_LIBS <targets…>)
```

Pass the file holding that call as `-DTAU_EXTERNAL_BAS=/abs/path/register.cmake`;
`tests/external_ba/` is a working example, run by `scripts/test-external-ba.sh`.

## The descriptor

`ba_descriptor_complete` in `boolean_algebras/ba_descriptor.h` lists the
mandatory members, one `requires` per line, so a missing one is reported
against the line that names it. Assert it for your pack with:

```cpp
static_assert(assert_pack_descriptors_complete<my_node_t>());
```

Capabilities beyond that surface are optional and probed with `requires`, never
by BA name — declare `can_host_bool` if one of your types can hold a plain 0 or
1 (add `bool_carrier_type()` when that is not your `type_tree()`), or
`ba_has_arithmetic_theory<my_ba>` (in `ba_pack_traits.h`) if the algebra brings
arithmetic terms and its own decision procedure.
