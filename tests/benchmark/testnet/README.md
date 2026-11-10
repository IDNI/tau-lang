# tau-overload benchmarks

Benchmarks that load tau the way tau-testnet does, with rules that grow until
the engine strains. The Benchmarks workflow (`.github/workflows/benchmarks.yml`)
runs them on every push to `devel` and `main` and stores the record in the
`tau-overload` series of the dashboard (<https://idni.github.io/tau-lang/benchmarks/>).

| script | group | measures |
|---|---|---|
| `tau_dispatch_blowup.py` | `dispatch-blowup` | the load time of one keyed rule (a `?:` chain or a flat `\|\|`) through the CLI, as a boot spec or as a revision |
| `bench_clause_compile.py` | `clause-compile` | the build of an interpreter for a guarded-comparison clause through the Python binding, and its first step |
| `load_test_bitvectors.py` | `load-test-bv` | a running CLI fed one seeded bitvector rule per step |
| `tau_account_rules_repro.py` | `account-rules-repro` | one rule per account through the Python binding (`sweep --quick`): N rules as the parts of one specification (open, step, and the update that adds part N), one rule per interpreter (build, and the memory of 200 live ones), and 1000 interpreters built, stepped and dropped in one process (the build and step of the first and the last 100) |

`tau_account_rules_repro.py` is the script of issue 211 of IDNI/tau-lang,
kept as it was posted so its numbers compare with the issue's tables.

`run_overload.py` runs the fixed set of points the series records and writes
one row per point (time, peak memory, status) to `<out>/overload.json`;
`scripts/ci/benchmark-collect.py --kind overload` turns that folder into a
record. A point killed at its cap gets the status `cap`. Its time is a lower
bound and is still compared; only a wrong answer or a crash is `fail`.

Run them on a local build:

```bash
./dev preset release-all --target tau,tau_nanobind
python3 tests/benchmark/testnet/run_overload.py --tau build/release/tau \
    --module-dir build/release/bindings/python/nanobind \
    --python "$TAU_PYTHON" --out build/overload-results
```

`--python` names the interpreter the module was built for. Run in Docker, on
any commit whose Dockerfile has the `build-compile` stage, from the root of
that checkout:

```bash
<this folder>/overload-docker.sh build/overload-results
```
