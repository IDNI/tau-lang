# @idni/tau-lang

Node.js and WebAssembly bindings for the Tau Language Framework. The package is
the embind library (`tau.js` / `tau.wasm` plus the ES module `tau.esm.mjs`); the
browser REPL is a separate artifact and is not part of it. Its threading follows
the preset: pthreads by default, or the `release-wasm-nothreads` library.

It is assembled by the Emscripten preset (configure writes `package.json` into
the build directory and `tau_js_package_assets` copies the `files` it lists) and
published to the local dependency store by `scripts/dep/wasm32-emscripten/tau-js.sh`. It
is not published to a public npm registry.

## Usage

The module factory resolves to an object whose functions are the embind
bindings; every function loads the wasm module once and then runs in it.

```js
const tauModule = require('@idni/tau-lang');

tauModule().then((tau) => {
	console.log(tau.sat('x = 0 && x = 1'));        // false
	console.log(tau.normalize_formula('x = 0 || x = 0')); // "x = 0"
});
```

ES modules use the default export as the same factory:

```js
import init from '@idni/tau-lang/tau.esm.mjs';

const tau = await init();
console.log(tau.valid('x = x')); // true
```

The full surface is `get_spec`, `normalize_formula`, `sat`, `unsat`, `valid`,
`solve`, `reset`, `to_str`, `get_last_error`, the `set_*` budgets with their
`get_*` getters, the options by name, and the `interpreter_*` step API.

Every option of the options repository has the name of its `tau` command line
option. `option_names()` lists the names. `set_option(name, text)` sets an
option from its text and gives the text it reads after the write.
`get_option(name)` gives the text of an option. Both give `null` for an
unknown name or a bad text, with the reason in `get_last_error()`.

```js
tau.set_option('max-fixpoint-steps', '1000');
console.log(tau.get_option('max-fixpoint-steps')); // '1000'
```

Under Node.js the module reads the `TAU_` environment variables when it loads.
The variable of an option is `TAU_` plus the option name in upper case, with
`_` for `-` (`max-fixpoint-steps` reads `TAU_MAX_FIXPOINT_STEPS`). A bad value
does not stop the load. The module keeps the reason in `get_last_error()`, and
then every engine call fails with that reason. So check `get_last_error()`
once, right after the load. The text is empty after a clean load, and it can
also hold the warnings of a load that succeeds:

```js
tauModule().then((tau) => {
	const why = tau.get_last_error();
	if (why !== '') throw new Error(why);
	// ...
});
```

A browser has no such variables, so a page uses `set_option` instead.

`tau.node.js` and `tau.node.mjs` are the CommonJS/ESM smoke-test
wrappers; a consumer needs only the entry points named in `package.json`.

## Build from source

```bash
./dev dep-emsdk.sh
./dev preset release-wasm                                    # build tau.js/tau.wasm/tau.esm.mjs
./dev preset release-wasm --target tau_js_publish            # publish to the local store
```
