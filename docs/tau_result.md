# `result<T>` — the public API's return type

Every `api<node>` operation that can fail returns `result<T>`: the value, or a
structured `report` saying why not. Failure is no longer a null `tref`, an empty
`std::optional`, or a line on stdout. The report also carries timings and
counters, so a run's numbers travel with its errors.

Include `src/tau_diagnostics.h`. It aliases the parser library's types into
`idni::tau_lang` — `result<T>`, `report`, `code`, `attr`, `diag_sink(s)` — and
adds `TAU_TRY`, `TAU_TRY_OR`, `step_awaiting_input()`, `report_has_code()`.
Full reference: `external/parser/docs/diagnostics.md`.

## One name per concept

A function that can build a report returns `result<T>`. It has no twin
that returns the bare value under a suffixed name such as `_total` or
`_name`. Such a twin hides the point where a caller drops the report, and
it goes stale as every caller reaches for the unchecked name instead.

## Consuming

```cpp
auto r = tau_api::get_formula("x = 0");
if (!r) { std::cerr << r.report(); return; }
tref fm = *r;                                  // or r.value()
```

`if (r)` / `has_value()` — succeeded · `*r` / `value()` — the value ·
`report()` — the report, on success too.

A null pointer is a legitimate value for a pointer-typed `result<T>`, for
example `result<tref>`. The container never turns a null pointer into an
error. A producer that treats a null result as failure reports that error
itself, with `error()` on the same result before it returns.

## Propagating

`TAU_TRY` merges the child report into the local `r` and returns early on
failure:

```cpp
result<tref> normalize_input(const std::string& s) {
    result<tref> r;
    TAU_TRY(auto fm, tau_api::get_formula(s));
    return r.with_assert_check_value(normalize<node>(fm));
}
```

`TAU_TRY` merges the report of every child into `r`, on the success path as
well as the failure path. So the function must end with
`return r.with_value(...)` (or another `with_*` form), never with a freshly
constructed `result<T>{...}`. A fresh result discards everything `TAU_TRY`
already merged into `r`.

It is a statement, not an expression — never the unbraced body of an `if`, and
it belongs in the same block as any `scope_guard` it should close.
`TAU_TRY_OR(decl, expr, code, msg)` handles a child returning neither value nor
error.

`TAU_TRY(decl, expr)` takes two macro arguments. A comma inside angle
brackets splits the macro, because the preprocessor does not parse
templates. Wrap such an expression in parentheses:

```cpp
TAU_TRY(auto ok, (bdd<B, o>::get_one_zero(b, m)));
```

## Returning in one statement

`with_value` / `with_error` set and return. The `with_assert_check_*` forms add
a `DBG` well-formedness assert — use them only where that assert already existed
on that path; an error path never needs one, since `error()` enforces the
invariant itself. All four return `result&&` and are terminal.

```cpp
if (!e) return r.with_error(code::parse_error, "Failed to parse term");
return r.with_assert_check_value(e);
```

## Codes

`parse_error` malformed input · `invalid_argument` well-formed but rejected ·
`internal_error` broken engine invariant · `unsat` · `solver_error` ·
`invalid_state` the interpreter needs input before it can step.

Test with `report_has_code(r.report(), code::unsat)`, or
`step_awaiting_input(r.report())` for that last one. Note a verdict can now be
absent: `unsat("not a formula")` has no value where it used to answer `true`.

**An empty result is not always a failure.** `api::step()` returns no value and
no report when the spec asked not to auto-continue — nothing went wrong, there
is simply nothing further to step. So distinguish the two:

```cpp
if (!r) {
    if (r.report().has_error()) { /* real failure */ }
    else                        { /* clean stop, end the run loop */ }
}
```

This is what `main.cpp`'s run loop and the REPL's `continue_running` branch on.

**Never `result<std::optional<T>>`.** A `result<T>` already carries three
states:

- a value and no error means success
- no value and no error means the legitimate decline above
- an error in the report means a genuine failure

Wrapping an optional inside a result duplicates the middle state. Produce
that state instead with a default-constructed `result<T>`.

## Dropping a report

A report is dropped only at a call site whose signature cannot carry one.
Name the blocking contract in a comment there:

```cpp
// Advisory drop: <the contract that blocks the report>.
```

Real contracts in this codebase include:

- a hash function that a `noexcept` constructor calls
- a `std::ostream&` stream operator
- a log macro that expands inside a stream chain
- a mandatory `ba_descriptor` member with a fixed return type
- a `tref f(tref)` rewrite hook
- a traversal callback with a fixed shape

A dropped report must never produce a silently wrong answer. When the only
fallback available would change the meaning of the result, change the
signature instead of dropping the report.

## Measuring and counting

Scopes nest; a `scope_guard` closes on destruction, LIFO.

```cpp
{
    auto g = r.open("normalize");      // info_micros, auto-timed
    r.count("rewrites", n);            // info_count
    r.kb("peak", peak_kb);             // info_kb
}                                      // dtor records elapsed µs
```

| call | records |
|---|---|
| `open(name)` | timed scope, µs on close |
| `open(name, tag, value)` | scope tagged otherwise, value untouched |
| `open_if(enable, …)` | same, or a no-op guard when false |
| `measure(name, f)` | runs `f` in a timed scope, closed before returning |
| `step(enable, name, f)` | `measure`, conditional |
| `count(name, v)` / `kb(name, v)` | a counter / a size in KiB |

`measure` returns what `f` returns, so it wraps an expression in place:

```cpp
auto fm = r.measure("parse", [&]{ return parse(src); });
```

Gate anything whose accounting costs more than it is worth behind
`open_if`/`step` — `rule_counting` does this for per-rule counts, which cost a
map lookup per rewrite.

Read back via `report().nodes()`: each node has `tag`, `key`, `parent`, `value`,
so the scope tree is walkable. `report::print(os)` renders it; the REPL's
`print_benchmarks` does just that.

## Python

```python
r = tau.step(interp, inputs)
if not r:
    print(r.report.errors)             # also .warnings .infos .codes .code_names
    if r.report.awaiting_input: ...
    raise SystemExit(1)
out = r.value                          # or r.unwrap() to raise instead
```

Every fallible call in the Python binding returns one `tau.result`: `get_interpreter`, `step`, `can_extend`, `update`, and `is_realizable`.
Each result carries `.value`, `.report`, `__bool__`, and `unwrap()`.
The binding never raises.
An exception carries one string, so it drops the warnings, the infos, and the timing scopes that a report holds.

`.value` is whatever the call produced, already converted to Python, and `None` when the call produced no value.
It owns its data, so an interpreter taken out of a result stays valid after the caller drops the result.
Nanobind erases the type on purpose.
It binds concrete types, so a per-`T` result class would need one new Python class for each result-returning method.

`update` puts the real verdict of the interpreter in `.value`: True when the interpreter accepts the revision, False when it rejects the revision.
`is_realizable` returns no value when the backend gives no verdict.
It never uses False as a substitute verdict.

`unwrap()` raises a RuntimeError with the report instead of returning a falsy result.
A consumer that wants a bare value writes that wrapper itself.
