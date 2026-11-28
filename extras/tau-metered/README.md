# Deterministic work budgets for Tau

This opt-in native C++ target addresses request 3 of
[issue #211](https://github.com/IDNI/tau-lang/issues/211). Default Tau builds
and the ordinary `TAU::TAU` API are unchanged. No compiler, solver binary,
or WebAssembly image is bundled with the target.

## Build and use

Configure with `TAU_BUILD_METERED_NATIVE=ON` and the path to a local
Wasmtime C SDK 51.0.0 in `TAU_METERED_WASMTIME_PREFIX`. Build
`tau_metered_native` and install component `MeteredSDK`. Consumers
use `find_package(TauMeteredSource CONFIG REQUIRED)` and link
`TAU::METERED`. The header is `<metered_tau.h>`.

Provision the five local files named in `tools/profile.manifest.json`.
The engine accepts exactly the profile recorded there. Given the four
matching raw build outputs, use the pinned WASI SDK 34 `llvm-objdump`:

```sh
python3 tools/provision-profile.py ./images \
  --objdump /path/to/llvm-objdump --tau /path/to/tau_work_budget.wasm \
  --ltlsynt /path/to/ltlsynt --ltlfilt /path/to/ltlfilt \
  --autfilt /path/to/autfilt
python3 tools/provision-profile.py --check ./images
```

The raw-file digest list is `tools/raw-inputs.json`. See the
[source build recipes](tools/rebuild/README.md) to produce these inputs.
Rebuilding a
different binary requires a new profile and new receipts, not a bypass
of authentication. Source builds use Emscripten SDK 6.0.12 for Tau and
the cvc5 closure, and Spot 2.16 with WASI SDK 34 for the three separate
commands. No existing default WebAssembly distribution is changed.

```cpp
#include <metered_tau.h>

idni::tau_lang::metered::engine tau("images");
auto result = tau.get_interpreter("o1[t]:sbf = 1.", 100'000'000);
```

Inspect the report before using the interpreter. A missing, unreadable, or
unqualified profile is reported by `get_interpreter` as `operational` at
`profile`, with zero logical work and no value. The SDK itself is built
with C++23; the public header is usable from C++20.
`budget` means the call
stopped before publishing a value or state. `used` is a deterministic
work count, not elapsed time. `operational` is local failure, never a
consensus verdict. The successful descriptor identifies the version
through `engine::profile_id()`. Compare receipts only for the same
profile and exact starting state. Pass the same typed input sequence
on each node; a logical map presented in a different input order may
take different host-ordering work.

## Meter and state boundary

Typed input admission and serialization, module initialization and restore,
guest execution, the supported Spot requests, return transfer and
publication share one count. The profile executes the Tau and cvc5 guest
and separately provisioned Spot commands. A successful publication
retains the prepared store instead of making another full memory copy.
On the next call, the host charges for restoring its memory into a
fresh candidate before inspecting those bytes. A refusing call never
mutates the previous store.

Named external files and ambient oracle services are not admitted in
this profile. It does not lift a live account cap, authorize untrusted
interpreter artifacts, or claim to close requests 1, 2 or 4.

## Replay the installed SDK

From this directory, with `/path/to/install` the relocated MeteredSDK
prefix and the matching Wasmtime SDK available:

```sh
cmake -S tools/replay -B /tmp/tau-metered-replay \
  -DCMAKE_PREFIX_PATH=/path/to/install \
  -DTAU_METERED_WASMTIME_PREFIX=/path/to/wasmtime-c-api
cmake --build /tmp/tau-metered-replay
/tmp/tau-metered-replay/tau-metered-replay /path/to/images
```

This checks the Spot child and publication cutoffs, exact stage receipts,
384-bit arithmetic, step rollback, and both an accepted and a declined
explicit update. It fails even in Release builds if an expectation changes.
