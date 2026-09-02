# Tau demos

Annotated REPL scripts that walk through the Tau Language, from basic syntax to
executing specifications. Each file is a valid Tau REPL script: comments start
with `#` and are passed through to the REPL, which ignores them.

## Running a demo

Build the Release binary and step through a script with `run.sh`:

```sh
./dev release              # from the repository root, produces build-Release/tau
cd demos
./run.sh demo_1.1-basic_syntax_and_history.tau
```

`run.sh` feeds the script to the REPL one line at a time and waits for a
keypress between lines, so you can read each result before the next command
runs. Press `Ctrl-C` to stop early.

To run a script start to finish with no pauses, pipe it in directly:

```sh
../build-Release/tau -X < demo_1.1-basic_syntax_and_history.tau
```

`-X` selects the line-oriented REPL, which is the one that behaves correctly
when standard input is a pipe rather than a terminal.

The demos use `../tests/integration/test_files/` for sample stream inputs and
`/tmp` for stream outputs, so they assume a \*nix filesystem layout. On Windows,
adjust those paths. `demo_3.4-interpreter_bitvectors.tau` and
`demo_4.1-abstract_data_types.tau` instead use their own `fixture/`
subdirectory (relative to `demos/`) for their file stream examples.

## The demos

| Demo | Topic |
|------|-------|
| `demo_1.1-basic_syntax_and_history.tau` | Terms, formulas, quantifiers, constants, stream variables, the history store |
| `demo_1.2-commands_and_history.tau` | Normal forms (`dnf`, `cnf`, `nnf`, `mnf`, `onf`), `subst`, `inst`, and history references as arguments |
| `demo_1.3-recurrence_relations.tau` | Defining functions and predicates by recurrence relations, fixpoint calls and `fallback` |
| `demo_1.4-normalization.tau` | `normalize` and `qelim`: deciding validity, minimizing, and canonical output |
| `demo_1.5-temporal_reasoning.tau` | Deciding temporal specifications: `sat`, `unsat`, `valid` and `normalize` over `always`/`sometimes` and time constraints |
| `demo_2.1-solver.tau` | `solve` and `lgrs`: satisfying assignments, splitters, Tau constants |
| `demo_2.2-solver-min_max.tau` | `solve --min` / `--max`: minimum and maximum solutions |
| `demo_2.3-solver-bitvectors.tau` | Bitvector equations, solved through cvc5 |
| `demo_3.1-interpreter_sbf.tau` | Executing specifications over `sbf` streams: console and file I/O |
| `demo_3.2-interpreter_tau.tau` | Executing specifications whose streams carry Tau specifications |
| `demo_3.3-interpreter_fpbf.tau` | Draft sketch of a programmable-Boolean-function machine (not yet runnable) |
| `demo_3.4-interpreter_bitvectors.tau` | Executing specifications over bitvector streams: casts across widths, modular accumulation, widen-multiply-divide-narrow |
| `demo_3.5-updatable_specifications.tau` | Self-amending specifications: pointwise revision through the update stream `u`, and the `this` stream |
| `demo_4.1-abstract_data_types.tau` | Abstract data types: aliases, tuples, inheritance, member access, and tuple-typed streams |
| `demo_4.2-adt_functions_and_predicates.tau` | Functions and predicates over tuple members: typed heads, body annotations, mixed-algebra tuples, deep member paths |
| `demo_4.3-adt_recurrence_relations.tau` | Recurrence relations over tuple members: indexed and fixpoint calls, loops and `fallback`, bitvector members |
| `demo_4.4-adts_as_arguments.tau` | Whole tuple values as definition arguments: one parameter per flattened member, sub-tuples, mixed algebras, recurrences |
| `demo_5.1-options_and_budgets.tau` | REPL options: `get`/`set`/`toggle`, `charvar`, severity, and the resource budgets with their give-ups |
| `sample_demo.tau` | Minimal template for writing your own demo |

Demos are numbered by section, so read them in order within a section.

## Writing a demo

Keep each line a single REPL command, end the script with `quit`, and prefer
`#` comments over long prose blocks so the script stays readable as it scrolls
past. `run.sh` filters lines containing `##`, so use `##` for notes that should
not reach the REPL at all.
