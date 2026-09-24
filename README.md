![The TAU logo](/docs/images/Tau_Banner.png)

| Build type | Status                                                                                     |
|------------|--------------------------------------------------------------------------------------------|
| Release    | ![Release Build](https://github.com/IDNI/tau-lang/actions/workflows/test-release.yml/badge.svg) |
| Debug      | ![Debug Build](https://github.com/IDNI/tau-lang/actions/workflows/test-debug.yml/badge.svg)     |

# Table of contents

1. [Introduction](#introduction)
2. [Installing the Tau Language Framework](#installing-the-tau-language-framework)
	1. [Linux](#linux)
	2. [Windows](#windows)
	3. [MacOS (not available yet)](#macos-not-available-yet)
	4. [Compiling the source code](#compiling-the-source-code)
3. [Quick start](#quick-start)
   1. [Run a spec with the interpreter (`tau`)](#run-a-spec-with-the-interpreter-tau)
   2. [Compile a spec to an executable (`tau compile`)](#compile-a-spec-to-an-executable-tau-compile)
4. [The Tau Language](#the-tau-language)
    1. [Tau specifications](#tau-specifications)
    2. [Full LTL operators](#full-ltl-operators)
    3. [CTL\* fragment and semantic negation](#ctl-fragment-and-semantic-negation)
    4. [Satisfiability and execution](#satisfiability-and-execution)
    5. [Boolean functions](#boolean-functions)
    6. [Bitvectors](#bitvectors)
    7. [Functions and predicates](#functions-and-predicates)
    8. [Constants](#constants)
    9. [Streams](#streams)
    10. [Variables and uninterpreted constants](#variables-and-uninterpreted-constants)
    11. [Type system](#type-system)
    12. [Abstract data types](#abstract-data-types)
    13. [Constant time constraints](#constant-time-constraints)
    14. [Pointwise revision](#pointwise-revision)
    15. [Reserved symbols](#reserved-symbols)
5. [Command line interface](#command-line-interface)
6. [The Tau REPL](#the-tau-repl)
	1. [Basic REPL commands](#basic-repl-commands)
	2. [REPL options](#repl-options)
	3. [Functions, predicates and input/output stream variables](#functions-predicates-and-inputoutput-stream-variables)
	4. [Memory related commands](#memory-related-commands)
	5. [Expression manipulation](#expression-manipulation)
	6. [Logical procedures](#logical-procedures)
	7. [Normal forms](#normal-forms)
	8. [Specification execution](#specification-execution)
7. [Web IDE](#web-ide)
8. [Developer infrastructure for ω-categorical synthesis](#developer-infrastructure-for-ω-categorical-synthesis)
9. [The C++ API and language bindings](#the-c-api-and-language-bindings)
10. [The Theory behind the Tau Language](#the-theory-behind-the-tau-language)
11. [Known issues](#known-issues)
12. [Future work](#future-work)
13. [Submitting issues](#submitting-issues)
14. [License](#license)
15. [Authors](#authors)


# **Introduction**

The Tau language is an expressive, decidable and executable formal software specification
language. It allows for specifying software using requirements and constraints in a purely
logical and declarative approach.
Tau specifications can be checked for satisfiability, effectively deciding if there exists a program
matching the specification. Furthermore, the Tau framework
is able to synthesize a program adhering to a satisfiable specification.

Imagine programming by writing the tests only, while getting a working program automatically which
is guaranteed to pass all the tests, thus meeting all the specified requirements.

A unique feature of the Tau language is the ability to specify programs capable of mechanized
deductive reasoning over Tau specifications themselves. This allows for a controlled adaption of new
requirements because the requirements for the requirements can be taken into account,
which is useful for collaborative specification, maintenance,
updates, safety and user control.

To go to the quick start [click here](#quick-start).

To go to the installation guide [click here](#installing-the-tau-language-framework).

For viewing known issues, future work and how to submit issues, [click here](#known-issues).

For visiting the theory behind the Tau Language [click here](#the-theory-behind-the-tau-language).

Please note that the Tau language is work in progress. Releases are currently in an alpha state
and can contain bugs. See the [submitting issues](#submitting-issues) section if you want to submit a report.
All features discussed below in this readme are available but can have performance issues.


# **Installing the Tau Language Framework**

## **Linux**

Currently, we automatically build the following binaries packages (AMD64 architecture):

* deb (Debian/Ubuntu): [tau-0.7-Linux.deb](https://github.com/IDNI/tau-lang/releases/download/v0.7-alpha/tau-0.7-Linux.deb)
* rpm (Fedora): [tau-0.7-Linux.rpm](https://github.com/IDNI/tau-lang/releases/download/v0.7-alpha/tau-0.7-Linux.rpm)

The executable is installed in `/usr/bin/tau`.

## **Windows**

For windows, we provide a convenient installer that includes the tau executable
and also a zip file:

* Installer: [tau-0.7-win64.exe](https://github.com/IDNI/tau-lang/releases/download/v0.7-alpha/tau-0.7-win64.exe)
* Zip file: [tau-0.7-win64.zip](https://github.com/IDNI/tau-lang/releases/download/v0.7-alpha/tau-0.7-win64.zip)

## **MacOS (not available yet)**

A macOS installer will be available in the future.

## **Compiling the source code**

To compile the source code you need a C++ compiler supporting C++23: GCC 13.3
or newer, or Clang 19 or newer (Clang 18 crashes while instantiating the tree
pack in `src/instantiate_pack.cpp`). You also need at least cmake version 3.22.1
installed in your system. `tau compile` builds the emitted project with the
compiler given by `--cxx` or `TAU_CXX`, else with `clang++` when it is on
PATH, else with cmake's default.
The code dependencies are the Boost C++ Libraries (including Boost.Log), CVC5,
libcurl, and Spot (`ltlsynt`/`ltl2tgba`) for LTL synthesis.
CVC5 is used only in order to support the theory of bitvectors within the language.
The core language and its algorithms are independent of CVC5.

On Ubuntu 22.04, the CI dependency set can be installed with:

```bash
./scripts/setup-ubuntu-ci-deps.sh
```

The CI build intentionally uses fixed tools and pinned transient dependencies:
GCC/G++ 13, Ninja, Spot from the `home:adl` package repository, cvc5 1.3.1, and
doctest `v2.4.11` verified by SHA-256 during CMake configure.  This avoids
depending on moving upstream defaults when reproducing CI locally.

After cloning:

```bash
git clone https://github.com/IDNI/tau-lang.git
```

all build and test operations go through the `./dev` helper, which dispatches to
the scripts in [`scripts/`](scripts). Run `./dev` with no arguments to list them,
and see [`scripts/README.md`](scripts/README.md) for the details of option
handling, parallel jobs and build directories.

The parser is a git submodule at `external/parser/`; `./dev` initializes it
automatically on first use.

CVC5 is built into `~/.tau/cvc5` by the CMake configure step if it is not there
yet. Boost is taken from the system by default, and only built into `~/.tau/boost`
when that is required (cross-compiling for Windows, or a position-independent
build). You can also build either dependency explicitly:

```bash
./dev dep-boost
./dev dep-cvc5
```

To build the binaries:

```bash
./dev release          # Release build          -> build-Release/
./dev debug            # Debug build            -> build-Debug/
./dev relwithdebinfo   # Release + debug info   -> build-RelWithDebInfo/
```

Alternatively, use the CMake presets declared in
[`CMakePresets.json`](CMakePresets.json), which build into `build/<build type>` (e.g. `build/release`):

```bash
./dev preset release-tau run -- --help
./dev preset release-tests run
```

To build with doxygen documentation, pass `-DTAU_BUILD_DOC=ON` to any of the
build scripts:

```bash
./dev release -DTAU_BUILD_DOC=ON
```

To build and run the test suites (please run both, as Debug and Release builds
enable different assertions):

```bash
./dev test-release
./dev test-debug
```

To build the Python bindings (see
[The C++ API and language bindings](#the-c-api-and-language-bindings)):

```bash
./dev binding python
```

Once you have compiled the source code you can run the `tau` executable to
execute Tau specifications. The `tau` executable is located in either `build-Release`
or `build-Debug` or `build-RelWithDebInfo` (or in `build/<build type>` when building
with presets).

The same binary also carries the `compile` verb, which turns a realizable
specification into a standalone executable (see
[Compile a spec to an executable](#compile-a-spec-to-an-executable-tau-compile)).

## **Selecting Boolean algebras (`-DTAU_BAS=`)**

Each Boolean algebra (BA) is a plugin, and a build contains the set — the
*pack* — chosen at configure time:

```bash
./dev preset release                              # the default pack
./dev preset release -DTAU_BAS=tau,sbf,bv         # drop qlt, qint, nlang, hsb
./dev preset release -DTAU_BAS=sbf,tau            # smallest useful pack
```

`tau` is the wrapper algebra that embeds a whole Tau spec as a value; the other
ids are base algebras. A BA's dependencies are declared in its own manifest, so
a pack without `bv` and `hsb` does not need CVC5, and one without `nlang` does
not need libcurl. Referencing the type of an algebra the build does not contain
is reported as an unknown type annotation, listing the ones it does contain.

Some internal machinery — the state bits of an LTL synthesis, a CTL* witness
output — needs a type holding a plain 0 or 1. Which algebra provides it is the
*Boolean carrier*, chosen from an ordered preference:

```bash
./dev preset release -DTAU_BOOL_CARRIERS=bv,sbf,bool   # the default order
./dev preset release -DTAU_BOOL_CARRIERS=sbf           # prefer sbf even when bv is present
```

The first listed algebra the pack actually contains wins, so one order serves
every pack: with the default, a pack with `bv` carries bits in `bv[1]` and a
pack without it falls to `sbf`.

To add an algebra — in-tree or from outside the source tree — copy
`src/boolean_algebras/_template/` and follow
[docs/adding_base_bas.md](docs/adding_base_bas.md).


# **Quick start**

Tau-lang offers two ways to execute a specification:

1. **Interpret** the spec directly — solve each time step on the fly.
   Use `tau`.  Best for iteration and REPL use.
2. **Compile** the spec to a standalone executable ahead of time.
   Use `tau compile`.  Best for deployment: the synthesis happens once, at
   compile time, and the program only steps the strategy.

Both use the same spec language.  You can start with the interpreter while
authoring a spec and switch to the compiler for production.

## Run a spec with the interpreter (`tau`)

```sh
# Start the REPL.
tau

# Or run a spec file directly.
tau spec.tau

# Or evaluate a REPL command inline.
tau -e "sat G(o1[t]:bv = i1[t]:bv)"
```

Full [command-line reference](#command-line-interface).  The interpreter is
the right choice when you want to iterate on a spec, inspect intermediate
results, or run short jobs from the shell.

## Compile a spec to an executable (`tau compile`)

`tau compile` parses a specification, runs the same LTL(ABA) synthesis
pipeline as the interpreter, and turns the synthesized strategy into a
standalone executable: it emits a small CMake project next to the spec
(`<spec>.build/`, one `main.cpp` that drives the strategy through the same run
loop `tau <spec>` uses, linked against the emitting build's libTAU), builds it,
and copies the program to the requested path.

```sh
# 1. Write a spec (mirror-input example).
echo 'G(o1[t]:bv = i1[t]:bv).' > spec.tau

# 2. Synthesize, emit and build in one command.
tau compile spec.tau -o sim

# 3. Run it: it reads inputs and prints outputs like the interpreter does
#    and exits when its input closes.
./sim
```

| Option | Description |
|--------|-------------|
| `-o, --output <path>` | executable path (default: the spec file path without extension) |
| `-c, --cxx <compiler>` | C++ compiler for the emitted project (default: `TAU_CXX`, else `clang++` when on PATH, else cmake's default) |

The exit code is `0` when the program was built and `1` on any failure; the
reason (parse error, UNREALIZABLE, no verdict from the synthesis backend, a
`cmake` configure or build failure) is in the `compile failed: …` message,
not in a dedicated code.  A spec whose strategy cannot be executed (an
Algorithm-B verdict over the `qlt` type, see the synthesis algorithms below)
is refused the same way.

Full worked example: `examples/reactive_program/` (the `Makefile` runs
`tau compile`; its `main.cpp` documents the shape of the emitted
`tau_program` class, which the library API `build_program_desc` +
`emit_program` in `src/cpp_codegen.h` produces).

**Correctness**: synthesis produces a program whose behaviour is consistent
with the spec on every input trace.  Unrealizable specs are rejected up front.

Pointwise revision (PWR) happens before code generation: a spec already
produced by PWR can be compiled and stepped like any other realizable spec.
Runtime PWR/spec patching remains the interpreter's responsibility, not a
feature of compiled programs.

See the [command-line reference](#command-line-interface) for the `tau`
options.

---

To start using the Tau Language, download the latest release from the
[GitHub page](https://github.com/IDNI/tau-lang/releases/tag/v0.7-alpha). Once
you have downloaded and installed the executable (see the Section
[Installing the Tau Framework](#installing-the-tau-language-framework)), you can run
it from the command line by typing `tau`.

The programming model underlying the Tau Language is fully declarative. You
specify, possibly only very implicitly, how the current and previous inputs and
outputs are related, at each point of time. As a result, you do not write a program
in the Tau Language, but a specification which effectively represents all programs that
meet this specification. Once you run a specification, you actually run one
automatically-chosen representative program from that set.

In the scope of the Tau Language, a specification is [satisfiable](#satisfiability-and-execution),
loosely speaking, if for all inputs, at each point in time, there exist outputs,
that do not depend on future inputs while matching the specification.
Implied from this definition is that all specifications run
indefinitely no matter what the inputs are.

For example, the following specification:

```
o1[t] = 0
```

states that the output `o1` at all time points `t` has to be `0`. Similarly, the
following specification:

```
o1[t] = i1[t]
```

states that the output `o1` has to be the same as the input
`i1` at all time points `t`.
In the above examples, `o1` and `i1` are [streams](#streams). They are used to define
the inputs and outputs of the specification.

An example of how to define input and output streams is the following:

```
i1 : tau := in console
o1 : tau := out console
```

In the above case we specify that `i1` and `o1` are of type `tau`, namely that
they are Tau specifications, and they take values from the console (e.g. stdin/stdout).
The type defines from which Boolean algebra the values come from. The `tau` type is a unique
property of the Tau langauge because it enables Tau specifications to reason over
Tau specifications themselves abstracted as Boolean algebra elements.

You can also define files as input or output streams:

```
i1 : tau := in file("input.in")
o1 : tau := out file("output.out")
```

The above two examples, `o1[t] = 0` and `o1[t] = i1[t]`, define one program each
(there's a caveat in this statement, but we shall ignore it here). This is not
always the case. An example
of a Tau specification that specifies infinitely many programs is

```
o1[t] & i1[t] = 0
```
because many different assignments to `o1[t]` can be chosen as valid output. Each choice
represents one program.
Here `&` denotes conjunction in the Boolean algebra from which the inputs and outputs
are taken from. This specification says that the conjunction has to be empty.

Clearly, you can consider more complicated specifications, e.g.:

```
o1[t] & o1[t-1] & i1[t] = 0 || o1[t] = i1[t]
```

which states that at each point in time, either the conjunction of the current
output with the previous output and with the current input, has to be 0, or, the
output has to equal the input. Note the difference between [Boolean (algebraic
operations)](#boolean-functions) and [Logical operators](#tau-specifications) .
The former are `&, |, '`, and the latter are `&&, ||, !`.

In order to simplify the process of writing and running Tau specifications, we allow
[function and predicate definitions](#functions-and-predicates),
possibly by means of recurrence relations.
The following is a simple predicate defined by a recurrence relation,
which takes as argument a Tau term:

```
f[0](y) := T
f[n](y) := f[n - 1](y)
```

which you can use in your specification as follows:

```
o1[t] = 0 && f(i1[t])
```

Also, you can use the following function definition by a recurrence relation:

```
g[0](y):sbf := 0
g[n](y):sbf := g[n - 1](y)'
```

which defines a function (rather than a predicate) and alternates between 0 and 1
depending on the parity of n in the `sbf` Boolean algebra (see below in [Type System](#type-system)).

In the [demos](https://github.com/IDNI/tau-lang/tree/main/demos) folder you
can find lots of examples regarding how to use the Tau Language, its semantics
and workings.

# **The Tau Language**

The core idea of the Tau Language is to specify how current and previous inputs and outputs of a program
are related over time, using the first-order theory of finite and infinite atomless Boolean algebras
extended with a time dimension, represented by the natural numbers starting at 0. It was carefully crafted around being purely logical,
allowing efficient and decidable satisfiability checking, while being executable,
yielding a framework to enable program synthesis.

For example, you can write `o1[t] & o1[t-1] & i1[t] = 0`
which means that the current output `o1[t]`, the previous output `o1[t-1]`, and the current input `i1[t]`,
have to have an empty intersection. The set-theoretic perspective of Boolean algebra
is given by Stone's representation theorem for Boolean algebras. More concretely,
when a Tau specification is treated as a Boolean algebra element,
it can be seen as a set of all programs that admit that specification, and the Boolean
operations are the set-theoretic union/intersection and complementation.

A key feature is the ability of checking satisfiability of Tau specifications within Tau specifications
using the theory of Boolean equations and an abstraction of specifications to Boolean algebra elements.
This enables, in particular, a novel approach to software updates. A specification which is currently
executed, can take as an input an arbitrary new Tau specification, seen as an update, check if the proposed new specification is
satisfiable and incorporate the update into the existing specification using an operation we call pointwise revision.
Section [Pointwise revision](#pointwise-revision) provides a more detailed introduction to this feature and how to use it.

The following is a general introduction to using the Tau language.

## **Tau specifications**

At the top level, a Tau specification (we also say `spec`) is a collection of
`G` ("globally") and `F` ("finally/eventually") statements applied to *local specifications*
(expressed by `local_spec`, see below),
combined by the logical
connectives *and*, *or* and *not*, denoted by `&&`, `||` and `!` respectively.
For example a well-formed Tau specification is
```
(G local_spec1) && (F local_spec2)
```
where *local_spec1* and
*local_spec2* are formulas as described below.
We say local specification because such a formula can only talk about a fixed
(though arbitrary) point in time.

In order for a specification to communicate with the outside world, so-called *streams*
are use. Those streams come in two flavors: input and output streams. Input
streams are used in a `local_spec` to receive input from a user, while output streams
are used for presenting output to a user. Streams can be given arbitrary names;
the names `i1`, `o1`, ... used throughout this document are just a convention
(see section [Streams](#streams)). Each stream in the specification
is associated with a relative or constant point in time.
For example the output stream variable `o1[t-2]` means
"the value in output stream number 1 two time-steps ago". So `o1[t]` would mean
"the value in output stream number 1 at the current time-step". Likewise, for
input stream variables like `i1[t]`. It means "the input in the input stream
1 at the current time-step". Input streams can also have an offset in order to
speak about past inputs. For example `i2[t-3]` means "the input in the input
stream 2 three time-steps ago". For further detail about streams, please refer
to section [Streams](#streams).

In all above cases, `t` is a free variable and refers to the current time at
each point in time. The key point now is that a `G` (globally) statement will
quantify all scoped `t` universally, while an `F` (eventually) statement will
quantify them existentially. For example the specification `G o1[t] = 0` says
that at all time-steps the output stream number 1 will write `0`. Similarly,
the specification `F o1[t] = 0` says that there exists a time-step at which
the output stream 1 will write `0`. When executing a Tau specification, the
first time-step is always 0.

A Tau specification without any temporal wrapper is implicitly assumed to be
a `G` ("globally") statement.

Formally, a specification is a (possibly empty) list of definitions followed by
a single formula, its *main* formula:
```
spec        => [ definitions ] local_spec [ "." ]
             | "G" local_spec          -- globally (always)
             | "F" local_spec          -- eventually (sometimes)
             | spec "U" spec           -- until
             | spec "R" spec           -- release
             | spec "W" spec           -- weak until
             | spec "S" spec           -- since (past LTL)
             | spec "T" spec           -- trigger (past LTL)
             | (spec && spec) | (spec || spec) | !spec
definitions => ( (function_def | predicate_def | stream_def | type_def) "." )+
```
where `type_def` declares an abstract data type (see
[Abstract data types](#abstract-data-types))
where `local_spec` is a formula defined by the rules:

```
local_spec => ("(" local_spec ")")
            | (("sometimes" | "<>") local_spec)
            | (("always"    | "[]") local_spec)
            | (local_spec "?" local_spec ":" local_spec)
            | ("all" variable ("," variable)* local_spec)
            | ("ex"  variable ("," variable)* local_spec)
            | (local_spec "->"  local_spec)
            | (local_spec "<-"  local_spec)
            | (local_spec "<->" local_spec)
            | (local_spec "||"  local_spec)
            | (local_spec "^^"  local_spec)
            | (local_spec "&&"  local_spec)
            | ("!" local_spec)
            | (term "=" term) | (term "!=" term)
            | (term "<" term) | (term "!<" term) | (term "<=" term) | (term "!<=" term)
            | (term ">" term) | (term "!>" term) | (term ">=" term)| (term "!>=" term)
            | (term "<=" term "<=" term)
            | time_constraint
            | predicate
            | T | F
```
The naming conventions for `variable` are discussed in
[Variables and uninterpreted constants](#variables-and-uninterpreted-constants).
Furthermore, `term` is discussed in the sections
[Boolean functions](#boolean-functions) and [Bitvectors](#bitvectors).
Note that `always` and `sometimes` are ordinary formula-level operators, and a
specification with no `always` or `sometimes` at all is implicitly an `always` statement.
The `predicate` non-terminal in the above grammar describes how
to add predicate definitions directly into a formula. See the subsection
[Functions and predicates](#functions-and-predicates) for the
grammar definition of `predicate`, and
[Constant time constraints](#constant-time-constraints) for `time_constraint`.
In the REPL ([The Tau REPL](#the-tau-repl)) definitions
can be provided as explained in subsection
[Functions, predicates and input/output stream variables](#functions-predicates-and-inputoutput-stream-variables).

The symbols used have the following meaning, where a formula refers to either `local_spec` or `spec`:

| Symbol              | Meaning                                                 |
|---------------------|---------------------------------------------------------|
| `!`                 | negation of formula                                     |
| `&&`                | conjunction of formulas                                 |
| `^^`                | xor of formulas                                         |
| `\|\|`              | disjunction of formulas                                 |
| `<->`               | equivalence of formulas                                 |
| `<-`                | left-implication of formulas                            |
| `->`                | right-implication of formulas                           |
| `ex`                | existential quantification of one or more `variable`s   |
| `all`               | universal quantification of one or more `variable`s     |
| `... ? ... : ...`   | if ... then ... else ...                                |
| `=`                 | standard equality relation in BA or bitvectors          |
| `!=`                | standard inequality relation in BA or bitvectors        |
| `<`                 | standard less relation in BA or bitvectors              |
| `!<`                | standard not-less relation in BA or bitvectors          |
| `<=`                | standard less-equal relation in BA or bitvectors        |
| `!<=`               | standard not-less-equal relation in BA or bitvectors    |
| `>`                 | standard greater relation in BA or bitvectors           |
| `!>`                | standard not-greater relation in BA or bitvectors       |
| `>=`                | standard greater-equal relation in BA or bitvectors     |
| `!>=`               | standard not-greater-equal relation in BA or bitvectors |
| `... <= ... <= ...` | interval: the middle term lies between the outer two    |

A quantifier can bind several variables at once, so `all x, y, z ...` is
shorthand for `all x all y all z ...`, and likewise for `ex`.

The precedence of the logical operators/quantifiers is as follows (from higher
precedence to lower):
`!` > `&&` > `^^` > `||` > `<->` > `<-` > `->` > `ex ... ...` > `all ... ...` >
`... ? ... : ...` > `F ...` > `G ...` > `... U ...` > `... R ...` > `... W ...` >
`... S ...` > `... T ...`.

Using this notation, a slightly bigger example of a Tau spec would be

```
    (G o1[t] i1[t] = 0 && (i1[t] != 1 -> o1[t] != 0)) && (F o1[t] = i1[t]')
```

which reads: at each point of time, the output should be disjoint from the input.
If the input is not 1, then the output is not zero. And, at least once during
execution, the output equals the complement of the input.


## **Full LTL operators**

Tau supports the standard LTL (Linear Temporal Logic) operators for full
reactive synthesis, extending the `G`/`F` safety fragment with
liveness and until-style operators.

### Grammar

```
spec => ...
      | "F" spec               -- eventually / finally
      | "G" spec               -- globally
      | spec "U" spec          -- until
      | spec "R" spec          -- release
      | spec "W" spec          -- weak until
      | spec "S" spec          -- since (past LTL)
      | spec "T" spec          -- trigger (past LTL)
```

`G` (globally) and `F` (eventually) are the primary temporal operators.  `U`, `R`,
`W` extend the language beyond the safety fragment.  `S` (since) and `T` (trigger)
are the past-LTL duals of `U` and `R` respectively; they are compiled away to
auxiliary variables before being passed to the synthesis engine.

The operators `X` (neXt) and `Y` (Yesterday) are *not* provided as explicit
keywords: they are subsumed by time-indexed stream variables.  For example,
`o1[t+1]` refers to the next output and `i1[t-1]` refers to the previous input,
so time shifts serve the role of X/Y directly.

Past LTL via lookback indices on io_vars (`i1[t-k]`, `o1[t-k]`) is the
preferred way to express history constraints.  The `S` and `T` operators provide
an alternative surface syntax that is automatically reduced to such indices.

### Semantics

| Operator | Meaning |
|----------|---------|
| `F φ`   | φ holds at some future time step (eventually) |
| `G φ`   | φ holds at all future time steps (globally / always) |
| `φ U ψ` | φ holds continuously until ψ first holds (until) |
| `φ R ψ` | ψ holds until and including the first time φ holds, or forever if φ never holds (release) |
| `φ W ψ` | like `U` but ψ need never hold (weak until) |
| `φ S ψ` | φ has held since (some past time at which) ψ held (since — past dual of U) |
| `φ T ψ` | ψ has held since (some past time at which) φ held, or ψ has always held (trigger — past dual of R) |

Standard equivalences hold: `F φ ≡ T U φ`, `G φ ≡ ¬ F ¬ φ ≡ ⊥ R φ`,
`φ W ψ ≡ G φ ∨ (φ U ψ)`.  Past duals: `φ S ψ` is the since operator and
`φ T ψ` is its release-like dual.

### Examples

```
-- Output o1 must eventually be 0
F (o1[t] = 0)

-- Output o1 must stay 0 until input i1 is 1
(o1[t] = 0) U (i1[t] = 1)

-- Infinitely often the output must echo the input
G (F (o1[t] = i1[t]))

-- Release: i1[t] != 0 releases o1[t] = 0
(i1[t] != 0) R (o1[t] = 0)
```

### Synthesis game semantics

Tau realizability is defined as a **two-player reactive synthesis game**:

- The **system** (synthesized program) controls all *output* streams (`o1`, `o2`, …).
- The **environment** (adversarial input) controls all *input* streams (`i1`, `i2`, …).
- At each time step, the environment provides inputs first; the system then produces outputs.
  The system may use any past inputs and outputs with lookback index `[t-k]`.

A formula `φ` is:
- **REALIZABLE** if `∃strategy. ∀env. φ` — the system has a winning strategy that satisfies `φ` for every possible input sequence.
- **UNREALIZABLE** if `∀strategy. ∃env. ¬φ` — no matter what the system does, the environment can always violate `φ`.

```
-- REALIZABLE: system can always echo the input
G (o1[t] = i1[t]).

-- REALIZABLE: system can always output 0 (vacuously satisfies the antecedent)
G (i1[t] = 1 -> o1[t] = 1).

-- UNREALIZABLE: no output can satisfy both simultaneously
G (o1[t] = 0) && G (o1[t] = 1).

-- UNREALIZABLE: environment can always choose i1=1 to block the condition
G (o1[t] = 1 -> i1[t] = 0).
```

Naming convention: `oN` (N = 1, 2, …) are output stream names; `iN` are input stream names.  These names are required — the synthesis engine uses the `o`/`i` prefix to classify variables as system-controlled vs. environment-controlled.

### Realizability algorithm

LTL(ABA) realizability uses an oracle-assisted synthesis algorithm:

1. **Data atoms** — maximal temporal-operator-free subformulas involving
   stream variables — are extracted from the formula.
2. Each data atom is replaced by a propositional variable `d_i`; the result is a
   pure-propositional LTL skeleton φ*(d₀, …, d_{K-1}).
3. A type enumeration over the named constants in the formula (T₁ for output
   memory, T₂ for (memory, input) pairs) builds a propositional formula that
   encodes which (memory-type, data-pattern) triples are feasible.  The formula
   also adds input propositions (P-bits) that expose the current input's T₁ type
   to the synthesizer, enabling type-aware strategies.
4. Spot's `ltlsynt` decides realizability of the propositional formula and
   extracts a winning Mealy strategy automaton (HOA format).
5. The **ABA oracle** (tau-lang's own quantifier-elimination engine) verifies
   that every strategy transition is consistent with the underlying Boolean
   algebra: for every input assignment, the system can find output values
   satisfying the data guard (`∀i. ∃o. guard`).

A formula is **realizable** iff both (4) and (5) succeed.  The external tool
`ltlsynt` (part of Spot ≥ 2.10) must be on the `PATH` for LTL formulas.

#### Synthesis algorithms

The LTL(ABA) implementation currently exposes the following synthesis paths:

| Algorithm | `TAU_LTL_ALG` | Description |
|-----------|--------------|-------------|
| **Algorithm B** (default for input-bearing `qlt`) | unset or `B` | Adds `⌈log₂\|T₂\|⌉` *input* propositions (P-bits) binary-encoding the T₂ type σ = (pos_m, pos_x, rel_mx). The strategy observes the current input's T₁ type, making it sound for formulas with input variables (e.g. `G(o1 > i1)`). Formula structure: `(Φ_I ∧ Ψ_I) → (Φ_O^R ∧ Φ_δ ∧ φ*)` where Φ_I/Ψ_I are env assumptions about P-bits. |
| **Algorithm A** | automatic for pure-output formulas, or `A` for pure-output formulas | Uses only R-bits for memory type with no input propositions. Fast for pure-output formulas. When input variables are present, Algorithm A is intentionally bypassed because it cannot observe the environment's T₁ type. |
| **Algorithm D** | `D` | Output-only direct parity-game construction. Builds the propositional synthesis game for φ*(D_i) via `ltlsynt --print-game-hoa`, then solves the data product game (synthesis game × T₁) using Zielonka's recursive attractor solver. D-patterns are decoded by AP name (`d_i`), not by HOA AP order. Input-variable formulas fall through to Algorithm B. |

Algorithm B is the default for formulas over the `qlt` type that contain input
variables. Pure-output `qlt` formulas route to Algorithm A because no P-bits are
needed. Algorithm D is accessible via `TAU_LTL_ALG=D`, but remains output-only.
The algorithm picker follows the same soundness rule: A and D are excluded when
input variables are present.

**Synthesis timeout**: by default, `ltlsynt` is given 60 seconds to solve
the propositional skeleton.  Set the environment variable `TAU_LTL_TIMEOUT_SEC`
to a positive integer to change the limit, or to `0` to disable it entirely.

```bash
TAU_LTL_TIMEOUT_SEC=120 tau "G (F (o1[t] = i1[t]))."
```

**LTL synthesis environment variables** (all optional):

| Variable | Default | Description |
|----------|---------|-------------|
| `TAU_LTL_TIMEOUT_SEC` | 60 | Wall-clock limit for each `ltlsynt` call (0 = disable). Environment fallback of `--ltl-timeout` / REPL `set ltltimeout`; the option wins when given. |
| `TAU_LTL_EXPORT_STRATEGY` | _unset_ | `hoa` prints winning-strategy HOA to stderr; `dot` prints Graphviz dot (falls back to HOA if `autfilt` is unavailable). |
| `TAU_LTL_EXPORT_STRATEGY_FILE` | _unset_ | If set to a path, also writes the HOA strategy to that file on success. |
| `TAU_LTL_SIMPLIFICATION` | _ltlsynt default_ | Forwarded to `ltlsynt --simplification=` (`bwoa`\|`sat`\|`bisim-sat`\|`none`). |
| `TAU_LTL_WITNESS` | _unset_ | When set to `1`, prints an environment counter-strategy (HOA) to stderr on UNREALIZABLE — only available when the UNREAL verdict comes from `ltlsynt` (not from earlier tau-internal rejection). |
| `TAU_LTL_OMCAT_QE_MAX_VARS` | 2 | Free-variable cap for the omcat (`qlt`) existential quantifier-elimination fast path. Values above 2 re-enable a fast path that is not sound; leave it at the default. Environment fallback of `--ltl-qe-max-vars` / REPL `set ltlqemaxvars`. |
| `TAU_LTL_ALG` | _unset_ (Algorithm B for input-bearing qlt, Algorithm A for pure-output qlt) | Override synthesis algorithm: `A` = request Algorithm A for pure-output formulas (input-bearing formulas still route to B), `B` = Algorithm B (P_σ binary encoding), `D` = request output-only Algorithm D (input-bearing formulas fall through to B). Environment fallback of `--ltl-alg` / REPL `set ltlalg`; anything other than `A`, `B`, `D` or `auto` is reported once and read as `auto`. |
| `TAU_LTL_HOA_MAX_STATES` | 4194304 (2^22) | Largest state count accepted from an `ltlsynt` HOA strategy (0 = unlimited); a larger count is read as a garbled header. Environment fallback of `--ltl-hoa-max-states` / REPL `set ltlhoamaxstates`. |
| `TAU_LTL_GUARD_MAX_CUBES` | 512 | DNF cubes a HOA guard label may expand into in the Algorithm D product game (0 = unlimited); a guard beyond it is refused. Environment fallback of `--ltl-guard-max-cubes` / REPL `set ltlguardmaxcubes`. |
| `TAU_LTL_REFINEMENT_ROUNDS` | 64 | ABA-oracle refinement rounds of one realizability check (0 = unlimited); on the cap the verdict is UNKNOWN. Environment fallback of `--ltl-refinement-rounds` / REPL `set ltlrefinementrounds`. |
| `TAU_LTL_WINDOW_MAX_PATHS` | 4096 | Strategy paths the multi-step window oracle examines per check (0 = unlimited); a hit cap yields UNKNOWN. Environment fallback of `--ltl-window-max-paths` / REPL `set ltlwindowmaxpaths`. |

Every limit above is a runtime parameter carried by all three surfaces --
a CLI flag, a REPL option and an `api::set_*` setter (see the CLI and REPL
option tables) -- with the environment variable as the last fallback. Each
one resolves **option > environment > default**, so a flag or a `set`
command always wins over a variable a script exported, and each variable is
validated: a negative, out-of-range or non-numeric value keeps the default
and says so once. Zero is a value rather than an absence -- it means
unlimited for every cap here, and no watchdog for the timeout -- except for
`--ltl-qe-max-vars`, whose own "not set" sentinel is 0 because a cap of 0
would mean nothing there.

The caps an algebra declares about itself follow the same three surfaces,
addressed `--<ba>-<option>` on the command line and `<ba>-<option>` in the
REPL, and are present when that algebra is in the pack: `qlt` declares
`--qlt-t3-cap` (data atoms its T3 encodings accept, default 20, at most 30,
`TAU_QLT_T3_CAP`) and `--qlt-const-output-max` (constant-output assignments
the fast path in front of Algorithm B enumerates, default 100,
`TAU_QLT_CONST_OUTPUT_MAX`); `nlang` declares `--nlang-http-timeout`
(seconds per LLM request, default 15, `TAU_NLANG_HTTP_TIMEOUT`).

**Other environment variables.** Three Boolean switches keep an environment
fallback beside their option: `TAU_BA_COMPONENT_FACTORING` (a non-empty
value other than `0` enables, `0` disables; read once, then it overrides
`--ba-component-factoring` / `set factoring`), `TAU_BV_CASE_SPLIT` (`0`
disables, any other value enables; overrides `bv-case-split` in both
directions) and `TAU_BV_QF_DECISION` (a value other than `0` enables the
quantifier-free bitvector decision; `bv-quantifier-free-decision` enables it
too). The nlang oracle reads `TAU_LLM_API_KEY` (or `OPENAI_API_KEY`),
`TAU_LLM_ENDPOINT` and `TAU_LLM_MODEL`. Two diagnostic gates change logging
only, never a verdict: `TAU_LEAN_DECIDE_CROSSCHECK` re-decides the lean
constant tests through the full path and reports disagreements, and
`TAU_PHI_DELTA_CROSSCHECK=1` shadows the ABA oracle with the closed-form
Φ_Δ of the atomless algebra on matching shapes. `TAU_CODEGEN_RUN_SDK_LINK_TEST`
opts the codegen test suite into a minutes-long real `cmake` build.

**Normal-form reuse switches.** Four three-way switches (`0` off, `1` on, the default, `2` shadow: both paths run, every disagreement is counted and reported at exit when the variable is set) govern the reuse of the normal forms of tau constants: `TAU_BA_NORMALIZED_MEMO` (a normal form `normalize_tau` returned is decided and renormalized without a further normalization pass and, as an operand of the constant operators, taken as it is), `TAU_BA_NORMALIZED_CONJUNCTION` (the normal form of a conjunction of normal forms is built from their bodies, or assembled from their shape), `TAU_API_SAT_FACTORED` (`api::sat` decides an always-conjunction over streams by its stream-disjoint components) and `TAU_BA_NORMALIZED_WITHOUT` (the normal form of an always-conjunction without one of its conjuncts is assembled from its shape).



**Type scope seeding.** `TAU_TYPE_SCOPE_SEED` governs how the type inference of a REPL line or of a step's input is seeded from the streams the session has seen: `0` walks the whole scope on every call, `1` (the default) seeds the streams the formula mentions only, `2` is the shadow mode, which seeds by mention, infers once more from the whole scope, and counts every disagreement, reported at exit.

**Continuation fixpoint from the functional shape.** `TAU_FUNCTIONAL_CONTINUATION` governs how the fixpoint of the unbounded continuation is settled: `0` decides the implication between two consecutive iterates only, `1` (the default) settles the fixpoint, the run check and the constant closure from the functional shape of the specification where it has one (every output of a time point defined by one equation or one conditional tree over the inputs, the earlier outputs and the other outputs of the time point, without a cycle) and decides the implication elsewhere, `2` is the shadow mode, which lets the implication decide every check and counts the checks the shape would have settled, those among them whose implication does not hold or could not be decided, and the closures it would have settled, reported at exit together with the number of checks.

**Factorized continuation.** `TAU_FACTORIZED_CONTINUATION` governs the form of the unbounded continuation of a specification of functional shape (see `TAU_FUNCTIONAL_CONTINUATION`) and its warm-up: `0` normalizes the continuation as one formula, whose disjunctive normal form multiplies the cases of the definitions along the chains of definitions reading each other, and warms every part up by quantifying the coordinates not yet reached and eliminating them, `1` (the default) normalizes the continuation conjunct by conjunct, keeps the conjunction, which is the form the interpreter grounds and solves at every step, and warms a part of the shape up by keeping the conjuncts of the reached coordinates alone, which is what the elimination returns for such a part (before the last initial condition of the specification, and after an update, every part is eliminated), `2` is the shadow mode, which computes both forms and both warm-ups, lets the one formula and the elimination decide, and counts every disagreement the normalizer finds and every equivalence it cannot decide, reported at exit together with the number of continuations and warm-ups kept when the variable is set. The line printed when the fixpoint is reached shows the form that is kept.

**Execution**: when the interpreter pipeline is given a realizable LTL formula,
`ltl_to_safety_formula` converts the winning Mealy strategy to an executable
`G(φ)` formula.  Single-state strategies (common for F, G(F), R, W) use the
self-loop guard directly.  Multi-state strategies are encoded using one-hot
auxiliary output bitvector variables (`o__ltl_ms0__`, `o__ltl_ms1__`, …) and
work for any number of states.

### Operator precedence

The binary temporal operators bind tighter than every propositional
connective, `!` included; the prefix operators `F` and `G` bind loosest:

```
U, R, W, S, T  >  !  >  &&  >  ^^  >  ||  >  <->  >  <-  >  ->  >  ex  >  all  >  ?:  >  F, G
```

So `!a U b` is `!(a U b)`, `a && b U c` is `a && (b U c)`, and `G a && b` is
`G (a && b)`.  Mixing binary temporal operators without parentheses is best
avoided: `a U b R c` groups as `a U (b R c)` but `a R b U c` as `(a R b) U c`.
The printer always parenthesises such operands.

In the CTL\* fragment the prefix operators `A`, `E` and `-` sit at the level
of `F` and `G`.

### Multi-type LTL formulas

Different LTL sub-formulas may use different BA types (tau, sbf, bv), but each
*atomic formula* must use exactly one type.  Variables of different types must
be given different stream names (e.g. `o1:tau` and `o2:sbf`).

Verified working patterns:

| Pattern | Result |
|---------|--------|
| `F(tau) && F(sbf)` | REALIZABLE |
| `F(sbf) && F(bv)` | REALIZABLE |
| `G(sbf) && F(bv)` | REALIZABLE |
| `G(tau) && F(bv)` | REALIZABLE |
| `G(tau) && G(tau)` | Merged to `G(tau && tau)` — works |
| `G(sbf) && G(sbf)` | Merged to `G(sbf && sbf)` — works |
| `G(tau) && G(sbf)` | REALIZABLE ✓ (split into independent checks) |
| `G(tau) && G(bv)` | REALIZABLE ✓ (split into independent checks) |

When combining two `G` sub-formulas of the **same** type, write either
`G(phi_A && phi_B)` or `G(phi_A) && G(phi_B)` — the two are semantically
equivalent and tau-lang automatically merges the latter form into a single
`G` before normalization.

### Lookback initialization

When a specification uses lookback stream variables (e.g. `i1[t-k]` or
`o1[t-k]`), the values they read do not exist for the first `k` steps.  The
interpreter handles this per *clause*: the `always` part of a specification
(every `always` statement merged into one) and each `sometimes` statement, or
each top-level conjunct of a full-LTL formula, is enforced from the deepest
lookback that clause reads, and asks nothing before that.  During a clause's
warm-up its outputs are unconstrained by it, lookback-free ones included; the
interpreter picks the bottom element (`0`/`"F"` for tau, `0`/`"F"` for sbf,
etc.) unless a later step of the run needs another value, so
`G(o1[t-1] = 1)` starts with `o1[0] = 1`.

A clause that reads no past starts at step 0 even beside one that does.
`(always o1[t] = 1) && (sometimes o1[t-2] = 0)` is unsatisfiable: the
`always` holds from step 0, so `o1` never was `0` two steps earlier.  Within
one clause the deepest lookback counts for every literal:
`G(o1[t] = 1 && o2[t] = o2[t-1])` leaves `o1[0]` unconstrained, and
`G(o1[t] = i1[t-1] && o2[t] = i2[t-2])` leaves both outputs unconstrained
for steps 0 and 1, whatever their individual shifts.  The LTL synthesis
pipeline follows the same rule, so `G(p U q)` with `q` reading the past
agrees with `G q` when `p` is contradictory.

### Known LTL limitations

- **Spec terminator**: in specification files and in programs passed to the
  parser every statement ends with `.` (a period), e.g. `G (o1[t] = 1).`; the
  argument of a single REPL command (`sat G (o1[t] = 1)`) may omit it.
- **Semantic negation (`-`)**: `- φ` under a temporal operator reads φ as
  started fresh at that point (see [CTL\* fragment and semantic negation](#ctl-fragment-and-semantic-negation)).
- **nlang needs an LLM API key**: the oracle reads `TAU_LLM_API_KEY` (falling
  back to `OPENAI_API_KEY`), with `TAU_LLM_ENDPOINT` (default
  `https://api.openai.com/v1`) and `TAU_LLM_MODEL` optional and each HTTP
  request capped by the `nlang-http-timeout` option (15 s, or
  `TAU_NLANG_HTTP_TIMEOUT`). Without a key
  every emptiness, universality and equivalence question over `nlang`
  elements is answered `false` (not cached), a warning is printed once, and
  verdicts over `nlang` are not reliable.
- **fall/fex BF quantifiers**: these parse correctly but are not supported in
  LTL synthesis (only in safety/always formulas).
- **Input-only atoms**: formulas consisting entirely of input constraints (no
  output variables) are checked for satisfiability but cannot be synthesized
  (the system has no control over pure-input atoms).
- **Nesting depth**: deeply nested formulas may exceed stack or memory limits;
  there is no built-in depth cap.  Use shallower formulas or increase the process
  stack size when needed.

## **CTL\* fragment and semantic negation**

The default grammar fragment is **LTL** (linear temporal logic).  To use
branching-time operators, switch to the CTL\* fragment:

```
fragment ctl_star
```

To switch back:

```
fragment ltl
```

### CTL\* operators

The CTL\* fragment extends the LTL grammar with path quantifiers:

| Operator | Syntax | Meaning |
|----------|--------|---------|
| **A** (for-all paths) | `A φ` | φ holds on every path from the current state |
| **E** (exists path) | `E φ` | φ holds on at least one path from the current state |

`A` and `E` quantify over the tree of possible strategy executions, one
branch per input sequence, not over individual traces.  Not every placement
is supported by the encoding:

- `E χ` in positive polarity is encoded through a fresh witness output.  When
  the specification has inputs, the witness path is pinned by one *direction*
  output per input stream, which names the value that path takes next, and the
  constraint is read one step later, where "the path follows the directions"
  is a plain `always`.  The encoding is then exact.  A past operator (`S`,
  `T`) inside χ has no such form: that witness keeps the all-paths encoding,
  which is stricter than `E`, so an unrealizable result is reported as UNKNOWN.
- `A χ` in positive polarity inside a universal context (under `&&`, `G` or
  another `A`) reduces to `χ` itself.
- `A` or `E` in negative polarity (under `!`, on the left of `->`) is first
  rewritten through its dual (`!A χ = E !χ`, `!E χ = A !χ`).
- Anything that still has no sound encoding -- `A` under an existential or
  eventual context (`||`, `F`, `U`, ...), `A`/`E` on either side of
  `<->`/`^^` or in the guard of `?:` -- is refused with an error.

`sat` of a CTL\* formula is `sat` of its encoding, so `sat A φ` and `sat φ`
agree.  `valid` does not decide formulas
with `A`, `E` or `-` and reports UNKNOWN.

### Semantic negation (`-`)

The CTL\* fragment also introduces **semantic negation** (`-`), which is
distinct from syntactic negation (`!`):

| Negation | Syntax | Meaning |
|----------|--------|---------|
| Syntactic (`!`) | `! φ` | Boolean complement of formula φ |
| Semantic (`-`) | `- φ` | There is **no** winning strategy for the system satisfying φ |

`- φ` is a strategy-level statement: it asserts that the specification φ is
**unrealizable** — no matter what the system does, the environment can always
violate φ.  This differs from `! φ`, which simply flips the truth value of φ on
a single trace.  `- φ` is a closed statement about φ's own game and is decided
by the realizability verdict of φ: it is true exactly when `realizable φ` is
false, and undecided when that is.  Under a temporal operator or a path
quantifier it means "φ is unrealizable from this point on", read as φ started
fresh there — as a specification installed by a revision reads its own
lookback, what precedes its start is warm-up.  That game is the same at every
point, so `- φ` folds to the same constant wherever it sits.

### Examples

```
-- In the CTL* fragment (REPL command, no terminator):
fragment ctl_star

-- For all paths, eventually the output is 1
A F(o1[t] = 1).

-- There exists a path where the output is always 0
E G(o1[t] = 0).

-- The environment cannot be forced to raise i1: true
-(F i1[t] = 1).

-- The same game at every step: true
G (-(F i1[t] = 1)).

-- There exists a branch where the environment raises i1
E (F i1[t] = 1).
```

### Reduction to LTL

CTL\* formulas are reduced to LTL synthesis problems with the
Bloem/Schewe/Khalimov witness-output encoding (arXiv:1711.10636): each
existential path choice becomes a witness output, with one direction output
per input stream pinning the path it witnesses, so the existing
`ltlsynt`-based pipeline handles the branching-time property.  The witness
constraint is stated one step after the witness, where "the path follows the
directions" needs no next-step operator: the path formula is rewritten by its
one-step expansion laws (`G φ = φ ∧ X G φ`, `φ U ψ = ψ ∨ (φ ∧ X(φ U ψ))`, …)
and its present part is read back through lookback.  The witness and direction
streams are internal: they use the reserved `w_` prefix and are never printed.
The `fragment ctl_star` switch is a REPL setting: specification files and the
API accept `A`, `E` and `-` without it.

## **Satisfiability and execution**

### Satisfiability

The notion of satisfiability in the Tau language is non-standard and conceptually defined in such a way
that a satisfiable Tau specification can be executed indefinitely regardless of the
inputs given to the specification. In particular, a specification is unsatisfiable
if a particular sequence of inputs would result in a contradictory situation.

This notion of satisfiability is achieved by a certain quantification pattern of
streams, which we call *time-compatability*. More precisely, all input streams are quantified
universally and all output streams are quantified existentially in such a way that
1. the ordering of
   quantifiers is increasing with respect to the time step to which the quantified variable refers and
2. the outputs always depend on the inputs in a given step.

More informally, we can think of the phrase that _for all input,
there exists output at each point in time such that the Tau specification
can be satisfied_ (in the standard sense).

The following example shows the explained quantification pattern for the Tau specification `o1[t] = i1[t] && ( i1[t-2] = 1 -> o2[t-1] = 1 )`:
```
all i1[t-2] ex o2[t-1] all i1[t] ex o1[t] o1[t] = i1[t] && ( i1[t-2] = 1 -> o2[t-1] = 1 )
```

This explanation of satisfiability neglects the fact that a contradiction can, in fact, occur only
after a specification is executed for a certain number of steps. The entire procedure is, hence, (much) more involved.
Further resources concerning the details can be found in the [theory section](#the-theory-behind-the-tau-language).

A specification without a temporal operator is read as if it were wrapped in
`always`, and it gets the same answers as its `always` spelling: a stream
constraint is not decided as a single step, so `o1[t]:bv[2] > o1[t-1]:bv[2]`
(an output that must grow forever within a finite range) is unsatisfiable.

For full LTL (`U`, `R`, `W`, `S`, `T`, nested temporal operators) the same
notion is realizability: `sat` and `realizable` decide it through the LTL(ABA)
synthesis pipeline and agree in both directions.

Validity is not quantified over an environment but over traces: `valid φ`
holds when no trace violates φ. It is `unsat ! φ` with every input stream read
as an output, for `always`/`sometimes` specifications as well as for full LTL,
so that `valid φ` implies `sat φ`. For example `valid sometimes i1[t] = 1` is
F, since the input can stay 0 at every step, and `G (F i1[t] = 1)` is neither
valid nor satisfiable: the inputs can stay 0, and the system cannot make them 1.

A verdict that cannot be decided is reported as UNKNOWN (an error, never a T
or F answer). This covers a backend failure, a resource cap that gave up, an
`E` over inputs in the CTL\* fragment, and a closed formula that normalization
leaves undecided, such as a functional quantifier over arithmetic (see
[Boolean functions](#boolean-functions)). When one disjunct of a formula is
satisfiable, the answer is still decided, even if another disjunct is
undecided. A problem whose decision diagrams outgrow the node table ends with
the error `bdd node table exhausted: a node did not fit, so the result is
unknown and no answer is given`. The next command runs normally.

### Execution

We have talked about the execution of a Tau specification. Here we want to
explain in more detail what that means. In general, the execution of Tau specification
is about receiving values for the input streams from a source and to produce values
for the output streams in accordance with the specification.
In this fashion, a Tau specification is started at time step 0.
In each consecutive step the time step is incremented by 1. As a result, we get a
continues timeline starting at 0 and ending at however far the specification has
been executed.

Take for a minimal example the specification `G o1[t] = i1[t]`,
saying that at each time step `t`, the input `i1` is equal to the output `o1`.
Executing this specification means to start at `t := 0`. Hence, in the first step
during execution we have `o1[0] = i1[0]`. The value for `i1[0]` is then requested,
let's call it `v`.
After the value is received, `i1[0]` is replaced with `v`, yielding `o1[0] = v`.
At this point the specification has as only stream the output `o1[0]` remaining and,
hence, can be passed to an internal solver
to get a value for `o1[0]`, matching the specification. In this case this is easy, since
it is immediate that `o1[0]` has to be `v` as well.
Outputting the result for `o1[0]` then concludes the first step of execution. We then move
to the next step being `t := 1`, which yields `o1[1] = i1[1]` for the specification. The steps
above are now repeated such that at the end of the step, the value for `o1[1]` is outputted.
In the same fashion execution can now be continued as long as desired.

For more complicated specifications the main steps stay the same:
1. Execution is started at `t := 0`
2. The inputs are requested (if present)
3. The outputs are produced according to the specification after the received inputs
have been plugged in
4. The time step `t` is incremented by 1 and execution continues from point 2. above

Note that before a Tau specification is executed, it is checked for satisfiability in the sense
that it can be executed indefinitely as described above. During this process the specification
is also converted to what we call _unbounded continuation_ which essentially adds all implicit
assumptions from a specification ensuring that the solutions for output stream values
do not make the execution contradictory in a future step. The continuation is built
time point by time point until a fixpoint is reached. A specification of functional
shape, whose outputs are functions of its inputs and of the earlier outputs, is a
fixpoint as soon as the lookback is covered: the block a time point adds holds for every
value of what it reads from outside, so adding it changes nothing. The shape is read off
the specification as written, before it is normalized, and settles the fixpoint, the run
check and the constant closure without deciding the implication between the two iterates
(`TAU_FUNCTIONAL_CONTINUATION`, see the environment variables below). Such a
continuation is kept as the conjunction of its conjuncts' normal forms, and its
warm-up keeps the conjuncts of the reached coordinates (`TAU_FACTORIZED_CONTINUATION`).

It is not always the case that the values which can be assigned to outputs are unique.
For this reason, a single specification can give rise to a multitude of different programs
differing in the choices made for assignments. As a result, during execution only
one of possibly many program is executed. The execution process in Tau, however,
is fully deterministic, so that the same program is always chosen for the same specification.


## **Boolean functions**

One of the key ingredients of the Tau Language are Boolean functions build from
Boolean combinations of variables, streams and constants over some fixed atomless (or
finite -to be developed-) Boolean algebra. In particular, each Boolean
function has a unique type being this chosen Boolean algebra.
They are given by the following grammar:

```
term => ("(" term ")") | (term "&" term) | term "'"
      | (term "^" term) | (term "|" term)
      | ("fall" variable ("," variable)* term)
      | ("fex"  variable ("," variable)* term)
      | function | constant | uninterpreted_constant
      | variable | stream_variable | "0" | "1"

```

where

* `term` stands for a well-formed subformula representing a Boolean function and the operators `&`, `'`,
`^` and `|` respectively stand for conjunction, negation, exclusive-or
and disjunction,
* `fall` and `fex` are the *functional* (term-level) universal and existential
quantifiers. Unlike `all` and `ex`, which build a formula, these build a Boolean
function: `fall x f` denotes the meet and `fex x f` the join of `f` over all
values of `x`. Over a Boolean body, normalization evaluates them through Boole's
expansion: `fex x f` becomes `f[x:=0] | f[x:=1]` and `fall x f` becomes
`f[x:=0] & f[x:=1]`, innermost first. A body with arithmetic, a cast,
`min`/`max`, a function reference or a quantifier that remains keeps the functional
quantifier as an atomic term, and a closed formula over it is reported as
UNKNOWN,
* the conjunction operator `&` may be omitted between two operands, so `xy` is
the same as `x & y`,
* `function` is the non-terminal symbol used to incorporate function definitions (see the subsection
[Functions and Predicates](#functions-and-predicates)),
* `constant` stands for an element from an available Boolean algebra. The type of the constant
determines the type of the Boolean function (see subsection [Constants](#constants) for details).
* `uninterpreted_constant` stands for an uninterpreted constant from the fixed Boolean
algebra, which can be thought of as a variable being existentially quantified from the outside of
the formula. Its syntax is as follows:

```
uninterpreted_constant => "<" [name] ":" name ">"
```

* `variable` is a variable over the fixed Boolean algebra (see subsection
[Variables and uninterpreted constants](#variables-and-uninterpreted-constants)
for details),
* `stream_variable` represents an input or output
stream. The type of a stream also determines the type of the Boolean function
(see also subsection [Streams](#streams)) and
* `0` and `1` stand for the bottom and top element in the fixed Boolean
algebra. For a bitvector type they are the all-zeros and the **all-ones**
vector, so `1:bv[8]` is `255`, not the number one (see
[`0` and `1` in bitvectors](#0-and-1-in-bitvectors)).

The order of the operations is the following (from higher precedence
to lower): `'` > `&` > `^` > `|` > `fex ... ...` > `fall ... ...`.
The full ordering including the bitvector operators is given in the section
[Bitvectors](#bitvectors) below.

If no type information is present within a Boolean function, it is assumed to be of the
general type `countable atomless Boolean algebra`. Since all such Boolean algebras are
isomorphic, no particular model is chosen.

For example, the following is a valid Boolean function of general type:

```
(x & y | (z ^ 0))
```

where `x`, `y` and `z` are variables.

## **Bitvectors**

Another key ingredient of the Tau Language is bitvectors build from
usual Boolean algebra operations and variables, streams and bitvector constants.
They add the following to the above grammar for terms:

```
term => (term _ '+' _ term) | (term _ '-' _ term) | (term _ '*' _ term)
      | (term _ '/' _ term) | (term _ '%' _ term) | (term _ "!&" _ term)
      | (term _ "!|" _ term) | (term _ "!^" _ term) | (term _ "<<" _ term)
      | (term _ ">>" _ term) | ("(" "bv" "[" bit_width "]" ")" _ term)
      | ("min" _ "(" _ term _ "," _ term _ ")")
      | ("max" _ "(" _ term _ "," _ term _ ")")
```

where `term` is as above are as above and
the new operators meaning is given in the following table:

| Symbol            | Meaning                                                |
|-------------------|--------------------------------------------------------|
| `*`               | modular multiplication of bitvectors                   |
| `/`               | modular division of bitvectors                         |
| `%`               | modular remainder of bitvectors                        |
| `+`               | modular addition of bitvectors                         |
| `-`               | modular subtraction of bitvectors                      |
| `!&`              | bitwise nand of bitvectors                             |
| `!\|`             | bitwise nor of bitvectors                              |
| `!^`              | bitwise xnor of bitvectors                             |
| `<<`              | left shift of bitvector by a number of bits            |
| `>>`              | right shift of bitvector by a number of bits           |
| `(bv[n])`         | cast of a term to a bitvector of width `n`             |
| `min(x, y)`       | unsigned minimum of two bitvectors                     |
| `max(x, y)`       | unsigned maximum of two bitvectors                     |

### `0` and `1` in bitvectors

As in every Boolean algebra, `0` and `1` denote the bottom and the top
element. For `bv[n]` the bottom is the all-zeros vector and the top is the
**all-ones** vector `2^n - 1`, so `1:bv[8]` is `255`. The number one is a
constant, `{1}:bv[8]`. This holds everywhere a bare `1` meets a bitvector,
in input and in output:

| written                    | meaning for `bv[8]`                                   |
|----------------------------|-------------------------------------------------------|
| `1:bv[8]`, `x + 1`         | `255`; `x + 255`, i.e. `x - 1` modulo 256             |
| `{1}:bv[8]`, `x + {1}:bv[8]` | the number one; the successor of `x`                |
| `x <= 1`                   | always true                                           |
| `1 <= x`                   | `x = 255`, i.e. `x' = 0`                              |
| `x'`                       | `255 - x`, the bitwise complement                     |
| `(bv[8]) 1:bv[4]`          | `{15}:bv[8]`: the cast widens `1111` with zeros       |
| `n {255}:bv[8]`            | prints `1`: an all-ones constant is shown as the top  |

Prefer braced constants (`{ #x01 }`, `{1}:bv[8]`) whenever a number is
intended, and read a bare `1` in bitvector output as "all ones". The
saturating idioms below rely on this: `i1'` is `2^w - 1 - i1` because the
complement is taken against the all-ones top.

`min` and `max` are call-style builtins, defined only for bitvectors: using
them on operands of any other Boolean algebra is a type-resolution error. Like
the comparison operators they compare unsigned, so e.g.
`max({200}:bv[8], {100}:bv[8])` is `{200}:bv[8]` even though 200 is negative
as a signed 8-bit value. For satisfiability they are handed to CVC5 as
`ite(bvule(x, y), ...)`; under predicate blasting (`-B`) they blast through a
fresh result variable `r` constrained by
`((x < y) -> r = x) && ((x >= y) -> r = y)` (the dual for `max`), with no
constant-operand precondition. Being self-delimiting they take no part in
operator precedence, and the names `min` and `max` are reserved: a user
function of that name can no longer be called with exactly two arguments.

The `(bv[n])` entry is a cast. It converts its operand to the bitvector type
of the given width, for example:

```
(bv[8]) x = { #x1f } : bv[8]
```

The cast operand must be a parenthesized term, a constant, a variable, a
function call, `0`, `1`, a negation, a functional quantifier or another cast;
wrap anything else in parentheses.

A cast converts between bitvector widths only. Its operand must itself be a
bitvector term, and its result is a bitvector of the target width that takes
part in the surrounding formula like any other term: casting an `sbf` or
`tau` term, or mixing a cast with a non-bitvector sibling as in
`((bv[8]) x:bv[4]) & y:sbf = 0`, is a type error reported during type
inference.

### Division and remainder by zero

Division and remainder are total functions with the SMT-LIB semantics of
`bvudiv` and `bvurem` (as implemented by CVC5). Neither raises an error on a
zero divisor; instead, for a bitvector `x` of width `w`:

```
x / 0  =  2^w - 1     (all-ones, the maximum value of bv[w])
x % 0  =  x
```

In particular `x / 0` yields the *maximum* of the type — the least safe
default for anything metering, pricing or otherwise accumulating — so never
rely on it implicitly. The same rule reaches `x / x` and `0 / x`: both are
`{1}` and `0` only for `x != 0`, and the maximum at `x = 0`, so the
normalizer leaves them unfolded while `x` is symbolic; `1 / 0` and `1 % 0`
are the all-ones top element again. When a divisor can be zero, guard it in the
specification and pick the zero-case value explicitly:

```
always ( ( i2[t]:bv[24] != { #x000000 }:bv[24] && o1[t]:bv[24] = ( i1[t]:bv[24] / i2[t]:bv[24] ) )
      || ( i2[t]:bv[24] =  { #x000000 }:bv[24] && o1[t]:bv[24] = { #x000000 }:bv[24] ) ).
```

Here `1200 / 3` gives `400` as usual, while a zero divisor gives `0` — the
spec author's own choice — instead of the inherited `16777215`.

### Overflow: checked and saturating arithmetic

`+`, `-`, `*` and `<<` are modular, with the SMT-LIB semantics of `bvadd`,
`bvsub`, `bvmul` and `bvshl`: results wrap around at the bit width silently —
there is no error, no flag and no carry bit. A wrapped value looks like any
other value, so a specification that must not wrap has to say so itself. Since
Tau is a constraint language, the overflow condition is expressible with the
ordinary operators, and a specification can branch on it. The patterns below
are the recommended forms; all examples use `bv[24]`, whose maximum is
`{ #xffffff }` (16777215), but nothing in them is width-specific.

**Saturating addition.** Unsigned addition overflowed iff the sum is smaller
than an operand:

```
always ( ( ( i1[t]:bv[24] + i2[t]:bv[24] ) >= i1[t]:bv[24] && o1[t]:bv[24] = ( i1[t]:bv[24] + i2[t]:bv[24] ) )
      || ( ( i1[t]:bv[24] + i2[t]:bv[24] ) <  i1[t]:bv[24] && o1[t]:bv[24] = { #xffffff }:bv[24] ) ).

  16777115 + 100  ->  16777215   (exact sum, no clamp needed)
  16777215 + 100  ->  16777215   (clamped instead of wrapping to 99)
```

**Checked multiplication by a constant.** The largest safe operand
`floor((2^w - 1) / n)` is itself a constant, so the guard is a plain
comparison — no arithmetic at all. For `n = 5` at width 24 the threshold is
`{ #x333333 }`:

```
always ( ( i1[t]:bv[24] <= { #x333333 }:bv[24] && o1[t]:bv[24] = ( i1[t]:bv[24] * { #x000005 }:bv[24] ) )
      || ( i1[t]:bv[24] >  { #x333333 }:bv[24] && o1[t]:bv[24] = { #xffffff }:bv[24] ) ).

  3355443  ->  16777215   (largest exact product)
  3355444  ->  16777215   (clamped instead of wrapping to 4)
```

**Checked multiplication by a variable.** When the multiplier is itself a
stream there is no precomputable threshold; the round-trip test
`(a * b) / b = a` detects the wrap (guard the divisor per
[Division and remainder by zero](#division-and-remainder-by-zero)):

```
always ( ( i2[t]:bv[24] != { #x000000 }:bv[24] && ( ( i1[t]:bv[24] * i2[t]:bv[24] ) / i2[t]:bv[24] ) =  i1[t]:bv[24]
           && o1[t]:bv[24] = ( i1[t]:bv[24] * i2[t]:bv[24] ) )
      || ( i2[t]:bv[24] != { #x000000 }:bv[24] && ( ( i1[t]:bv[24] * i2[t]:bv[24] ) / i2[t]:bv[24] ) != i1[t]:bv[24]
           && o1[t]:bv[24] = { #xffffff }:bv[24] )
      || ( i2[t]:bv[24] =  { #x000000 }:bv[24] && o1[t]:bv[24] = { #x000000 }:bv[24] ) ).

  4000000 * 5  ->  16777215   (detected; the raw product wraps)
```

**Saturating (monus) subtraction.** Underflow is an ordering comparison:

```
always ( ( i1[t]:bv[24] >= i2[t]:bv[24] && o1[t]:bv[24] = ( i1[t]:bv[24] - i2[t]:bv[24] ) )
      || ( i1[t]:bv[24] <  i2[t]:bv[24] && o1[t]:bv[24] = { #x000000 }:bv[24] ) ).

  100 - 1000  ->  0   (clamped instead of 16776316)
```

Getting the addition predicate subtly wrong is easy, so prefer copying these
forms over re-deriving them.

With the [`min`/`max` builtins](#bitvectors), the two saturating forms above
collapse to single expressions — no branch at all. `i1'` is the bitwise
complement, i.e. `2^w - 1 - i1`, so the addend below can never push the sum
past the maximum:

```
always o1[t]:bv[24] = ( i1[t]:bv[24] + min(i2[t]:bv[24], i1[t]:bv[24]') ).   # saturating +
always o1[t]:bv[24] = ( i1[t]:bv[24] - min(i1[t]:bv[24], i2[t]:bv[24]) ).    # saturating (monus) -
```

The checked-multiplication patterns keep their branches: the product itself
wraps, so no post-hoc `min` can recover it. The
[exact (widened) arithmetic mode](#exact-widened-arithmetic-mode) below lifts
exactly this restriction, for callers willing to opt in.

### Exact (widened) arithmetic mode

Everything above is *modular*: intermediate results wrap silently at the
operands' declared width, so a computation that must not overflow has to be
reasoned about with the guard patterns above, or, for addition and
subtraction only, with the `min`/`max` idiom that closes this section. Tau
also offers an opt-in **exact (widened) arithmetic mode** that instead
computes each atomic formula's arithmetic exactly, as over the naturals, at
whatever width is needed to avoid any overflow, and truncates only once, at
the point where a result is actually stored. Under this mode the
checked-multiplication gap admitted just above disappears: `min(i1*i2, K)`
becomes a correct, guard-free saturating multiplication.

The mode is off by default and changes nothing when disabled. Turn it on
with:

* the command line options `--bv-widening` (enable the mode; disabled
  by default) and `--bv-max-width <n>` (cap the width the mode is
  allowed to compute at; `0` leaves the current cap unchanged, 1024 unless
  already set). Both apply whether Tau is run as a REPL or given a
  specification file directly.
* the matching REPL options `bv-widening` (on/off, `set bv-widening on`) and
  `bv-max-width` (numeric, `set bv-max-width <n>`; `0` leaves the current
  cap unchanged) — like `bv-case-split`, a BA-declared option with no
  dedicated `api<node>` setter of its own.

**Semantics.** For every bitvector atomic formula (an equality, a
comparison, an interval), the mode computes, bottom-up, the minimum width
`W` at which none of the atom's arithmetic can overflow:

| Node                                     | Needed width                                               |
|------------------------------------------|------------------------------------------------------------|
| variable, io_var, constant               | the atom's own declared or inferred width `w` (a leaf's own type is not consulted separately) |
| `a + b`                                  | `max(width(a), width(b)) + 1`                               |
| `a - b`                                  | `max(width(a), width(b))`                                   |
| `a * b`                                  | `width(a) + width(b)`                                       |
| `a / b`, `a % b`                         | `width(a)` (a quotient or remainder never exceeds the dividend) |
| `a << k` (`k` a constant)                | `width(a) + k`                                              |
| `a >> k`, `a >> v` (either kind of shift right) | `width(a)`                                          |
| `a << v` (`v` a variable)                | `width(a)` — see caveat below                              |
| `min(a, b)`, `max(a, b)`                 | `max(width(a), width(b))`                                   |
| `&`, `\|`, `^`, `'`, `!&`, `!\|`, `!^`   | `max` of the operand widths — run at `W` like everything else, see caveat below |

`W` is the largest of these over the whole atom. Every leaf is upcast once
to `bv[W]` by zero-extension and every operator is retyped `bv[W]`; there is
no interior truncation anywhere. The only place a result is ever cut back
down is an equality with exactly one bare variable/stream side (an
"assignment"): there, the *other* side is computed at `W` and then cast
down, truncating, to the variable's own declared width — so
`o1[t] = i1[t] + i2[t]` still stores the same wrapped sum as in the default
mode, while `o1[t] = min(i1[t] + i2[t], K)` lets `min` see the exact,
unwrapped sum before it clamps. A comparison, an interval, or an equality
between two compound expressions is instead extended on both sides and
compared or equated exactly, with no truncation at all — this is where the
mode is most visible: `i1*i2 <= c` stops wrapping.

**The mode changes what formulas mean.** Widening is not a faster or more
precise way of answering the same question: it rewrites every atom into a
different formula, one over exact arithmetic, *before* any decision
procedure sees it, so satisfiability, validity, normalization, `solve`, and
the realizability of a specification are all decided for that rewritten
formula. A formula whose truth depends on wrap-around can therefore flip
its answer when the mode is switched on:

```
sat ex x:bv[4] (x:bv[4] << {4}:bv[4] != {0}:bv[4]).

  modular                        ->  F   (a 4-bit value shifted left by 4 is always 0)
  widened (--bv-widening)        ->  T   (the shift runs at W = 8: 1 << 4 = 16 != 0, so x = 1 works)

valid all x:bv[8] (x:bv[8] + x:bv[8] >= x:bv[8]).

  modular                        ->  F   (128 + 128 wraps to 0 < 128)
  widened (--bv-widening)        ->  T   (the sum runs at W = 9 and never wraps)

n x:bv[4] << {4}:bv[4] = {0}:bv[4]

  modular                        ->  T
  widened (--bv-widening)        ->  (bv[8]) x<<{ 4 }:bv[8] = 0
```

Neither answer is wrong; they answer different questions. The modular
answer is the one for the formula as written over `bv[4]`/`bv[8]`, and the
widened answer is the one for its exact-arithmetic reading. So the mode
must be chosen for a specification as a whole, not toggled around
individual queries, and a wrap-around that a specification *relies on*
(a mask computed by shifting, a counter meant to roll over) has to be
pinned with an explicit cast, which the mode leaves untouched:
`sat ex x:bv[4] ((bv[4]) (x:bv[4] << {4}:bv[4]) != {0}:bv[4]).` answers
`F` in both modes. The only construct that keeps its modular meaning on
its own is the assignment truncation described above:
`always o1[t]:bv[4] = i1[t]:bv[4] << {4}:bv[4] && o1[t] != {0}:bv[4]` stays
unsatisfiable with the mode on, because the shifted value is cut back to
4 bits when it is stored in `o1[t]`.

**Which commands see the mode.** The widening pass runs at the entry of the
normalizer, after definitions have been expanded, so a command decides the
widened formula exactly when it normalizes its argument. Everything else
handles the formula as written, in modular arithmetic, even with the mode
on:

| Command                                                          | Widened | Notes                                                                 |
|------------------------------------------------------------------|---------|-----------------------------------------------------------------------|
| `normalize` / `n` on a formula                                    | yes     | prints the elaborated atoms with their casts; the result is a fixed point (`n %1` returns it unchanged) |
| `sat`, `unsat`, `valid`                                           | yes     | on formulas and on specifications alike                               |
| `solve`                                                          | yes     | the assignment is searched for in the widened atoms                   |
| `run` / `r`, a specification file, runtime updates                | yes     | every execution step normalizes the current specification, see below |
| `dnf`, `cnf`, `nnf`, `mnf`, `qelim`, `subst`, `inst`              | no      | these transform the tree without normalizing it                       |
| a bare term, a history entry                                      | no      | there is no atom to widen; the tree is stored as parsed               |

```
qelim ex x (x:bv[8] * { 16 }:bv[8] = { 0 }:bv[8] && x != { 0 }:bv[8]).

  modular                        ->  T   (x = 16: the product wraps to 0)
  widened (--bv-widening)        ->  T   (qelim does not normalize, so the same modular answer)

sat ex x:bv[8] (x * { 16 }:bv[8] = { 0 }:bv[8] && x != { 0 }:bv[8]).

  modular                        ->  T
  widened (--bv-widening)        ->  F   (the product runs at W = 16 and is never 0 for x != 0)

solve --bv x:bv[8] * { 16 }:bv[8] = { 0 }:bv[8] && x != { 0 }:bv[8].

  modular                        ->  x := { 240 }:bv[8]
  widened (--bv-widening)        ->  no solution
```

The split matters when commands are chained through the history: whatever a
non-normalizing command has already decided stays decided in modular
arithmetic (`qelim` above returns `T`, and `sat %1` can only confirm it),
while whatever it leaves in place as a formula is widened by the next
normalizing command that receives it.

**Normalization.** `n` is the command that shows what the pass did: each
leaf appears under its zero-extending cast, every operator is retyped, and
an assignment keeps its truncating outer cast. Two details of the printed
form are worth knowing:

* A constant product that would wrap is not folded while the mode is on, so
  `n { 16 }:bv[8] * { 16 }:bv[8]` prints the product itself instead of `0`.
  Inside an atom the same product is computed exactly: `n { 16 }:bv[8] *
  { 16 }:bv[8] <= { 200 }:bv[8]` is `F` with the mode on and `T` without it,
  while `n x:bv[8] = { 16 }:bv[8] * { 16 }:bv[8]` is `x = 0` in both modes,
  because an assignment truncates the exact 256 back to 8 bits.
* Definitions are expanded first and widened afterwards, so a call whose
  body carries the arithmetic is widened like inline arithmetic: with
  `fn(a:bv[8]) := a * a.`, `n fn(x) <= { 200 }:bv[8]` prints
  `{ 200 }:bv[16] !< (bv[16]) x*(bv[16]) x`. A call that is *not* expanded
  is an opaque subterm and leaves its atom modular, see below.

**Execution.** The interpreter normalizes the current specification at
every step — the initial specification, its expanded definitions, and any
update merged at runtime — so with the mode on each step is solved over the
widened atoms. Inputs are read at their declared width and outputs are
stored at theirs: an output equation is an assignment, so the value that
reaches the stream is always truncated to the stream's width, exactly as in
the default mode. What changes is everything that sits *between* the
arithmetic and the store — a comparison guard, a `min`/`max`, a division —
which now sees the exact intermediate value. Two runs on `bv[8]` inputs
`16, 200, 3`:

```
always o1[t]:bv[8] = i1[t]:bv[8] * { 16 }:bv[8].

  modular                        ->  0, 128, 48    (256 and 3200 wrap on the way into o1)
  widened (--bv-widening)        ->  0, 128, 48    (same: the assignment truncates the exact product)

always (   i1[t]:bv[8] * { 16 }:bv[8] <= { 200 }:bv[8] && o1[t]:bv[8] = i1[t]:bv[8] * { 16 }:bv[8] )
    || (   i1[t]:bv[8] * { 16 }:bv[8] >  { 200 }:bv[8] && o1[t]:bv[8] = { 200 }:bv[8] ).

  modular                        ->  0, 128, 48    (the guard compares the wrapped product, so it never fires)
  widened (--bv-widening)        ->  200, 200, 48  (the guard sees 256 and 3200 and clamps)
```

A runtime update goes through the same normalization when it is accepted,
so its widened form is what gets merged and printed. With `u[t] = i1[t]`
running and `o1[t]:bv[8] = min({ 16 }:bv[8] * { 16 }:bv[8], { 200 }:bv[8])`
proposed on `i1`:

```
  modular                        ->  u[0] := always o1[t]:bv[8] = 0              then o1[1] := 0
  widened (--bv-widening)        ->  u[0] := always o1[t]:bv[8] = { 200 }:bv[8]  then o1[1] := 200
```

A proportional value is the everyday case of this: `( i1[t] * 5 ) / 100`
wraps at the operand width in the default mode as soon as `i1 * 5` exceeds
the maximum, while with the mode on the product is formed at twice the
width, the quotient is exact and — a quotient never needing more bits than
its dividend — the final truncation into the output is lossless. At
`bv[24]` with `i1 = 3355444` the default mode stores `0` and the widened
mode stores the exact `167772`.

Realizability follows the same reading: whether a specification is accepted
for execution or rejected (`Tau specification is unsat`) is decided for its
widened formula. Inputs are universally quantified, so a constraint on an
input that only holds without wrap-around is rejected in the default mode
and accepted with the mode on:

```
always o1[t]:bv[8] = { 1 }:bv[8] && i1[t]:bv[8] * { 2 }:bv[8] >= i1[t]:bv[8].

  modular                        ->  Tau specification is unsat   (i1 = 128 doubles to 0 < 128)
  widened (--bv-widening)        ->  o1[0] := 1, o1[1] := 1, ...  (the product runs at W = 16, so it always holds)
```

Conversely a specification that is only satisfiable thanks to a wrap-around
is rejected with the mode on:

```
always o1[t]:bv[8] * { 16 }:bv[8] = { 0 }:bv[8] && o1[t]:bv[8] != { 0 }:bv[8].

  modular                        ->  o1[0] := 240                (240 * 16 = 3840 wraps to 0)
  widened (--bv-widening)        ->  Tau specification is unsat  (no nonzero 8-bit value has a zero exact product)
```

The setting is global to the process (`--bv-widening`/`bv-widening`) and is
read at each normalization, so choose it before a specification is built
and keep it for the whole run rather than switching it between steps.

**Caveats.**

* Subtraction still wraps on underflow: `-`'s needed width never grows on
  its own (`max(width(a), width(b))`), so an underflowing subtraction still
  wraps, just at `W` rather than at the base width when it sits above an
  already-widened operand. Guard it exactly as in the default mode, with
  `a >= b`.
* A variable-amount left shift is the one operation the mode cannot bound:
  accounting for every possible shift amount could need up to
  `w + 2^{w_v} - 1` bits, so `a << v` for a variable `v` still executes at
  `W` and can still wrap there. A constant-amount shift, `a << k`, is fully
  accounted for and exact.
* Complement and the other negating bitwise operators (`'`, `!&`, `!|`,
  `!^`) act at `W`, not at the operand's own declared width, because every
  operator in the atom runs at the atom's computed width. So `x'` means
  `2^W - 1 - x`, not the base-width complement zero-extended — meaning the
  saturating idiom `min(i2, i1')` from the previous section no longer means
  "clamp against `i1`'s own maximum" once `i1'` is computed at some wider
  `W`:

  ```
  sat ex x ((x:bv[8] * { 255 }:bv[8])' <= { 200 }:bv[8]).

    modular                        ->  T   (x = 1 gives complement 0 <= 200)
    widened (--bv-widening)        ->  F   (the complement is computed at W = 16, always >= 510)
  ```

  If a specific width is intended, pin it explicitly with a cast, exactly
  as in the default mode:

  ```
  (bv[8]) i1[t]:bv[8]'          # the base-width (8-bit) complement, regardless of the surrounding computation's width
  ```

  A cast is always a boundary for this pass: whatever it wraps is left
  untouched — it computes exactly as it does in the default mode, at its
  own widths — and its result enters the surrounding widened computation at
  the cast's declared width, recovering today's exact meaning even with the
  mode switched on — e.g. adding the cast to the query above,
  `sat ex x ((bv[8]) (x:bv[8] * { 255 }:bv[8])' <= { 200 }:bv[8]).`, answers
  `T` again with `--bv-widening` still on.

**Payoff.** With the mode on, the two saturating patterns that need a
branch in the default mode — and the checked multiply that needs one no
matter what, per the admission above — both collapse to a single `min`:

```
always o1[t]:bv[8] = min( i1[t]:bv[8] + i2[t]:bv[8], { 200 }:bv[8] ).   # guard-free saturating +

  200 + 100  ->  200   (exact sum 300 does not fit under {200}, so min clamps to it)
  200 + 100  ->  44    (default mode: the sum itself already wrapped to 44 before min ever ran)
```

```
always o1[t]:bv[8] = min( i1[t]:bv[8] * i2[t]:bv[8], { 200 }:bv[8] ).   # guard-free checked *

  16 * 16  ->  200   (exact product 256 does not fit under {200}, so min clamps to it)
  16 * 16  ->  0     (default mode: the product itself already wrapped to 0; no post-hoc min can recover it)
```

The second pattern is the one this section opened by ruling out: with the
mode on, the multiplication itself never wraps, so `min` sees the real
product and the previously-impossible checked multiply becomes an ordinary
guard-free expression.

**Opaque subterms.** An atom containing a subterm the width computation
does not understand — a function or predicate call, a capture — is left
exactly as written, in modular semantics, with no diagnostic: the mode
widens only atoms whose arithmetic it can see through completely. Expand
such calls (or rewrite the atom without them) if it must be exact.

**Cap.** `W` is bounded by `--bv-max-width`/`bv-max-width` (default 1024); a
formula that would need a wider computation fails cleanly instead of
growing without bound. Exceeding it logs two error lines and the query
answers conservatively rather than crashing or hanging, e.g.:

```
(Error) bv-widening: required width 16 exceeds bv-max-width 8
(Error) is_tau_formula_sat: normalization failed (bv-widening cap exceeded); answering unsat. This is a conservative fallback, not a proof.
```

A satisfiability check answers unsat (`F`), a validity check answers "not
valid", and constructing or updating a specification that would exceed the
cap is rejected the same way — in every case this is a conservative
fallback the cap forces, not a proof that the formula is actually
unsatisfiable/invalid/unacceptable.

**Cost.** The mode is inert — identical performance — when off. When on,
a ground or lightly-quantified spec runs about as fast as the default mode;
widening one atom's arithmetic by a few dozen extra bits is cheap for CVC5.
Quantified nonlinear arithmetic is a different story: doubling the width of
a product inside a quantifier can turn an instant query into an
intractable one, e.g.
`valid all x all y ((x:bv[64]*y:bv[64])/y = x || y = {0}:bv[64])` answers in
well under a second in the default mode but does not finish in five minutes
once widened to bv[128]. The mode is at its best for executable specs and
ground or lightly-quantified reasoning; deep quantified nonlinear
arithmetic may need the cap, or restructuring the specification, to stay
tractable.

The guard idioms from the previous section remain the answer in the
default mode, and stay available — and correct — with the exact mode
switched on too. [`demos/demo_2.4-exact_bitvector_arithmetic.tau`](demos/demo_2.4-exact_bitvector_arithmetic.tau)
walks through the mode interactively, including the cap and a saturating
stream accumulator.

### Precedence of term operations

The order of *all* term operations, bitvector and Boolean alike, is the
following (from higher precedence to lower):

```
(bv[n]) > ' > & > ^ > | > !& > !^ > !| > { %, *, / } > { -, + } > { <<, >> } > fex > fall
```

The three groups in braces are shared precedence levels: `%`, `*` and `/` sit
together at one level, `-` and `+` at the next, and `<<` and `>>` at the last.
Operators of one level chain left to right, so `a + b - c` parses as
`(a + b) - c` and `a * b / c` as `(a * b) / c`. Parenthesize whenever the
intent is not obvious.

## **Functions and predicates**

Another key concept in the Tau Language are functions and predicates. They are given
by the following grammar where `function_def` defines the syntax for a function
and `predicate_def` defines the syntax for a predicate:

```
function_def      => function ":=" term
function          => name "[" index+  "]" "(" [ variable ("," variable)* ] ") [":" type]"
predicate_def     => predicate ":=" spec
predícate         => name "[" index+  "]" "(" [ variable [":" type] ("," variable [":" type])* ] ")"
```

where `name` is the name of the function or predicate (it has to be a sequence of
letters and numbers starting by a letter), `type`is the type of the arguments or
the function itself (as considered in [Type System](#type-system)) and `index`
is a positive integer or a variable or a variable minus a positive integer, i.e.:

```
index => number | variable | variable "-" number
```

Simple examples of function definitions are
```
union(x, y, z) : sbf := x | y | z
intersection(x, y, z) :sbf := x & y & z
bf(a, b, x) : tau := ax | bx'
```
while sample predicate definitions are
```
bottom(x) := x = 0
not_atom(x : tau) := ex y y < x && y != 0
chain(x,y,z) := x < y && y < z
```

Furthermore, it is supported to define a function or a predicate by means of a
recurrence relations. Let's see a small example for such a function definition:
```
rotate[0](x,y,z) : sbf := x & y | z
rotate[n](x,y,z) : sbf := rotate[n-1](y,z,x)
```
As can be seen `rotate[n](x,y,z)` is defined in terms of `rotate[n-1](y,z,x)`. Together
with the initial condition `rotate[0](x,y,z)`, `rotate[k](x,y,z)` can be calculated for any positive
number `k`, and essentially rotates the arguments counterclockwise until reaching `rotate[0](...)`.

A similar but more complex example for predicates is
```
f[0](w,x,y,z) := x & { G o1[t] = 0 } != 0
f[n](w,x,y,z) := f[n-1](x,y,z,w)

h[0](w,x,y,z) := f[0](w,x,y,z)
h[n](w,x,y,z) := h[n-1](w,x,y,z) && f[n](w,x,y,z)
```
As a result, calling `h[3](w,x,y,z)` checks that none of the arguments `w, x, y, z`
assumes `F o1[t] != 0`.

As can be seen, recurrence relations can also refer to other recurrence relations.
The rule one must follow is that the `index` at the right-hand side of `:=` is not bigger
than any `index` on the left. In addition, there must be no circular dependency among the
recurrence relation definitions in use.

Furthermore, we support the calculation of a fixpoint of a defined recurrence relation during normalization.
The syntax is to call a defined recurrence relation as
```
name "(" [ variable ("," variable)* ] ")" [":" type] [ "fallback" fp_fallback ]
```
hence, omitting the `index`. For example, using the recurrence relation `h` from above,
we can call the fix point by typing into [REPL](#the-tau-repl) `normalize h(x,y,z,w)`.

Not all recurrence relations have a fixpoint. The enumeration of the successive
steps may instead enter a loop, and the optional `fallback` clause says what to
return in that case:

```
fp_fallback => "first" | "last" | term | local_spec
```

* `first` returns the step at which the loop was detected,
* `last` returns the step just before it, and
* any term or formula returns that value verbatim (it must have the same type as
  the recurrence relation).

If no `fallback` clause is given, the default is `0` for functions and `F` for
predicates. For example:

```
normalize h(x,y,z,w) fallback last
```

The enumeration of successive steps is unbounded by default. It can be capped
with the `-E, --max-enum-steps` command line option or the `enumsteps` REPL
option (see [REPL options](#repl-options)); if neither a fixpoint nor a loop is
found within the cap, the search gives up with an error. Such a cap is a bound
on the search, not a proof that no fixpoint exists.

It should be noted that recurrence relations in the Tau language are a conservative extension,
meaning that they do not add to the general expressiveness.

## **Constants**

Constants in the Tau Language are elements of some available Boolean algebra,
usually different from just `0` and `1`. Constants in a particular Boolean algebra come
with their own syntax.

In the Tau language, we currently support the following base types.  Most are
Boolean algebras; `qlt` is ω-categorical and decidable, and is supported for
that reason:
1. the Boolean algebra of Tau specifications (also referred to as Tau Boolean algebra)
2. the Boolean algebra of simple Boolean functions (`sbf`)
3. the Boolean algebra of bitvectors of fixed bit width (`bv`)
4. the ω-categorical theory of dense linear order without endpoints (`qlt`) — rationals under `<`; ω-categorical and decidable, hence supported
5. the Boolean algebra of rational intervals `[x, y)` (`qint`) — right-closed, left-open; accepts both rational (`1/4`) and decimal (`0.25`) constants
6. the Natural Language Boolean Algebra (`nlang`)
7. the Boolean algebra of lex-half-open polyhedra in ℝ^d (`hsb`) — generalizes `qint` from 1D to d dimensions using canonical halfspaces

The Boolean algebra of Tau specifications is an extensional Boolean
algebra that encodes Tau specifications over arbitrary available other Boolean algebras.
As a result, it is possible to have a Tau specification that itself has a Tau specification
as a constant, allowing controlled reasoning of Tau specifications over Tau specifications.

Given the available algebras, the general syntax for constants is the following:

```
constant => "{" (spec | term) "}" [":" base_boolean_algebra_type]
```

where `base_boolean_algebra_type` is given by:

```
base_boolean_algebra_type => "tau" | "sbf" | "bv" '[' bit_width ']'
                           | "qlt" | "qint" | "nlang" | "hsb"
```

Note that the bit width of a bitvector type is mandatory: `bv[8]` is a type, while
a bare `bv` is not. Beware that omitting the width is not reported as an error —
`bv` is simply not recognized as a type name, and the expression is reinterpreted
as something else (`x:bv = 0` parses as `xv = 0`, a conjunction of two variables).

As mentioned, we can have a Tau specification seen as a Boolean algebra element (you can omit
the type, since `tau` is the default type). For example, the following is a valid
constant in the Tau Boolean algebra:

```
{ ex x ex y ex z (x & y | z) = 0 } : tau
```

or even a deeper nesting resulting in

```
{ { ex x ex y ex z (x & y | z) = 0 } : tau = 0 } : tau
```

where `x`, `y` and `z` are variables.

A constant in the simple Boolean function algebra is for example:

```
{ (x & y | z) } : sbf
```

where `x`, `y` and `z` are variables.

Finally, a constant in the bitvector Boolean algebra follows the syntax:

```
constant => '{' "#b" [0-1]+ | "#x" [0-9a-fA-F]+ | [0-9]+ `}`
```

For example, the following are valid bitvector constants of width 8:

```
{ #x1f } : bv[8]
```
or

```
{ #b00011111 } : bv[8]
```

## **Streams**

Streams represent the input and the output of Tau specifications. They are the
communication with the outside world, so to speak.
We currently have two kinds of them: input streams and output streams.
The syntax is given by

`stream_variable => name "[" index "]"`

where `name` is a sequence of letters, numbers and `_` starting with a letter, and
`index` is defined in subsection
[Functions and predicates](#functions-and-predicates).
Hence, `o1[t]`, `i1[t]`, `sensor[t]` and `log_out[t-1]` are all valid stream
variables.

Whether a stream is an input or an output is determined in one of two ways:

1. by an explicit stream definition (`:= in ...` / `:= out ...`, see below), or
2. otherwise by its name: a name starting with `i` (or the special name `this`)
   is an input, and a name starting with `o` (or the special name `u`) is an
   output.

A stream that is neither declared nor matches one of those naming conventions
cannot be resolved as an input or an output. This is why the conventional names
`i1`, `i2`, ... and `o1`, `o2`, ... used throughout this document work without any
declaration.

Both kinds of streams are indexed by time starting at the time step 0. A stream
associates to each time step a Boolean algebra element matching the type of the stream.

Which Boolean algebra element is associated, is decided by the Tau specification
in which the stream is used. For example, the specification `G o1[t] = 0`
says that `o1` associates `0` to each time step. One can alternatively think about this
as the value `0` being written into the stream `o1` at each time step.

There are several ways how a stream can refer to a time point. The following list uses an output stream
but input streams can be used in these same ways:
1. `o1[t]` refers to the current time point while executing a specification
2. `o1[t-k]` refers to the time point k steps ago while executing a specification
3. `o1[k]` refers to the fixed time point k while executing a specification

There are currently two ways to assign a type to a stream: either explicitly in a
stream definition (see below) or implicitly, by type inference from the context in
which the stream is used. The inference is performed by the general type system
described in section [Type system](#type-system), so constants, variables and
other streams occurring in the same atomic formula all contribute to the inferred
type. A stream that remains untyped after inference is assigned the default type
`tau`.

In case a type mismatch is detected, the specification is rejected and execution
does not start.

The syntax of a stream definition is
```
stream_definition => stream_name [member_path] [":" type] ":=" ("in" | "out") stream
```
where `stream_name` is the name of the stream, `type` is a supported type (`tau`,
`sbf`, `bv[8]`, `qlt`, `qint`, `nlang`, `hsb`, ..., or a user-defined type — see
[Abstract data types](#abstract-data-types)), `member_path` is an optional
tuple-member path (also discussed there), `in` marks an input stream and `out`
an output stream, and `stream` is either `console` (meaning that the stream
reads/outputs values from/to the console) or `file(file_name)` which denotes the
file from/into which to read/write (in quotes if needed). For example,
```
i1 : tau := in console
```
or
```
o1 : tau := out console
```
or
```
o2 : tau := out file("log.tau")
```
or, using an arbitrary stream name,
```
sensor : bv[8] := in file("samples.in")
```

Stream definitions can be given either as part of a specification's definitions
(each terminated by `.`) or entered directly in the REPL — see subsection
[Functions, predicates and input/output stream variables](#functions-predicates-and-inputoutput-stream-variables).

### Special streams

There are two predefined special `tau` streams called `u` and `this` which are discussed in the beginning of section [pointwise revision](#pointwise-revision)
and subsection [the this input stream](#the-this-input-stream).

## **Variables and uninterpreted constants**

Variables range over Boolean algebra elements. As mentioned in subsection
[Boolean functions](#boolean-functions), each
variable can only appear as part of a `term` and has, thus, a unique type.

The syntax for the non-terminal `variable` depends on whether the `charvar` option
is enabled or not. See [REPL options](#repl-options) for further details about the option. The option is enabled by default.

If `charvar` is enabled, a variable is a single letter followed by an arbitrary sequence
of numbers.

If `charvar` is disabled, a variable can be any sequence of letters, numbers and `_`,
must however be started by a letter.

A variable (or stream variable) of a tuple type may additionally be followed
by a member path, as in `l.tag` or `s.line.p.a` — see
[Abstract data types](#abstract-data-types).

In the Tau language a variable can appear free or quantified universally or existentially. Since
a Tau specification has to be a closed formula, in order to be executable, variables appearing
in this context must appear under the scope of a quantifier.
In the Tau REPL, however, you can also work with
open formulas (i.e. when variables are not quantified).

### Uninterpreted constants

Finally, we also have uninterpreted constants. They can be thought of as a variable being
implicitly existentially quantified from the outside of the Tau specification.
For this reason, from a semantic point of view, they behave
more like variables than constants.
The syntax is mentioned above in subsection [Boolean functions](#boolean-functions).
The appearance of an uninterpreted constant within a Tau specification keeps the specification
closed. Note that when executing a specification containing uninterpreted constants
they are currently assigned a default value in a suitable way and the specification
is then executed with those assignments.

## **Type system**

### Available types

The Tau Language currently supports the following base types:

1. `tau`: the type of Tau specifications,
2. `sbf`: the type of simple Boolean functions,
3. `bv[n]`: the type of bitvectors of bit width `n`,
4. `qlt`: the ω-categorical theory of the rationals under `<` (dense linear order, no endpoints) — ω-categorical and decidable, hence supported,
5. `qint`: the Boolean algebra of right-closed, left-open rational intervals `[x, y)`; accepts both rational (`1/4`) and decimal (`0.25`) endpoint constants,
6. `nlang`: the Natural Language Boolean Algebra (its oracle needs `TAU_LLM_API_KEY` or `OPENAI_API_KEY`, see [Known LTL limitations](#known-ltl-limitations)), and
7. `hsb`: the Boolean algebra of lex-half-open polyhedra in ℝ^d — generalizes `qint` from 1D to d dimensions using canonical halfspaces (see [hsb](#hsb--lex-half-open-polyhedra)).

In addition, user-defined type names — aliases of base types and tuple
types — can be introduced with a `type` definition; see section
[Abstract data types](#abstract-data-types).

You can type the following elements: variables, streams, recurrence relations,
constants and (term) constants. In order to do so, you just add the type
information after a colon `:`. For example,

```
x : sbf
o1[t] : bv[8]
{ #b00001111 } : bv[16]
{ ex x ex y (x & y) = 0 } : tau
1: tau
```

are all valid typed elements.

#### `qlt` — ω-categorical dense linear order

`qlt` represents the first-order theory of the rationals under `<`: a dense
linear order without endpoints.  This theory is ω-categorical, decidable, and
admits quantifier elimination, which is why tau-lang supports it.  Every
definable set is a finite Boolean combination of open/half-open/closed
intervals with rational endpoints.

tau-lang treats `qlt` specially: it is routed through a dedicated DLO
quantifier-elimination path (`qlt_dlo_qe`), not through the Boolean-algebra
pipeline (the comparison hooks `<`, `>`, `≤`, `≥` have their own semantics,
and `bf_*` Boolean-function rewrites do not apply).

Elements are written as interval expressions inside `{...}:qlt`:

```
{ (0,1) }:qlt          -- open interval between 0 and 1
{ [0.5,2) }:qlt        -- half-open interval; 0.5 parses as 1/2
{ (-inf,0) }:qlt       -- everything less than 0
{ (0,1) | [3,5) }:qlt  -- union of two intervals
{ 0.45 }:qlt           -- singleton constant (decimal accepted)
{ 1/3 }:qlt            -- singleton constant (rational form)
```

Endpoints are exact rationals.  The special symbols `-inf` and `+inf`
represent the extended line endpoints.  Parentheses `(`, `)` exclude the
endpoint; brackets `[`, `]` include it.  Both rational (`p/q`) and decimal
(`0.d…`) literal syntaxes are accepted.

#### `qint` — atomless Boolean algebra of rational intervals

`qint` represents the atomless Boolean algebra of right-closed, left-open
intervals `[a, b)` over the rationals on the extended real line.  Elements
are finite unions of such intervals.  The sentinels `-inf` and `+inf` are
supported as endpoints.

Internally an element is a sorted map from left to right endpoint stored as
IEEE doubles, so endpoints are exact for dyadic rationals and rounded otherwise.
Rational (`1/4`), decimal (`0.25`) and integer endpoint syntaxes are accepted,
plus `-inf`/`+inf`.  Bare integers inside `{...}:qint` have a special meaning:
`{0}` and `{1}` are the algebraic bottom and top, and any other integer `n`
denotes the interval `[n, n+1)`; write `[0,1)` and `[1,2)` explicitly.

Elements are written as interval expressions inside `{...}:qint`:

```
{ [0,1) }:qint           -- unit interval [0, 1)
{ [1/2,+inf) }:qint      -- from 1/2 to +infinity (rational syntax)
{ [0.5,+inf) }:qint      -- identical to above (decimal syntax)
{ [-inf,0) }:qint        -- everything strictly negative
{ [0,1/2) | [1,2) }:qint -- union of two intervals
```

Only right-closed, left-open intervals are representable as single atoms;
arbitrary Boolean combinations produce finite unions of such intervals.

#### `hsb` — lex-half-open polyhedra

`hsb` represents the atomless Boolean algebra of *lex-half-open polyhedra* in
ℝ^d (LP_d^Q).  It generalizes `qint` from 1D to d dimensions using canonical
halfspaces.

**Mathematical construction.** For a normal vector w ∈ ℝ^d \ {0}, define:
- L(w) = min{i : w_i ≠ 0} — the lex-leading index
- s(w) = sign(w_{L(w)}) ∈ {+1, −1} — the lex-leading sign

The canonical halfspace H_{w,b} is:
- `{ x : ⟨w,x⟩ + b ≤ 0 }` if s(w) = −1 (closed — boundary included)
- `{ x : ⟨w,x⟩ + b < 0 }` if s(w) = +1 (open — boundary excluded)

**Key properties:**
1. **Complement**: ℝ^d \ H_{w,b} = H_{−w,−b}.  Negating the normal flips the
   lex-leading sign, hence flips open↔closed.
2. **No equalities**: for any v ≠ 0, exactly one of {⟨v,x⟩ ≤ c} and
   {⟨v,x⟩ ≥ c} is canonical, so {⟨v,x⟩ = c} is never expressible.
3. **Atomless**: every non-empty element has non-empty interior (because no
   equalities → full-dimensional cells).  This is the defining property of an
   atomless Boolean algebra.
4. **1D reduction**: when d = 1, H_{w,b} gives exactly the half-open intervals
   `[a, b)` of `qint`.

Elements are formula trees of canonical halfspaces combined with Boolean
operations (&, |, ~).  Emptiness checking uses Fourier-Motzkin elimination to
test LP feasibility of each conjunctive cell in the DNF expansion.

`hsb` is registered in the dispatcher pack and `main.cpp` (available as a
data-sort alongside `sbf`, `bv`, `qint`, `qlt`, and `nlang`).

**Parsed constant syntax.** The `parse_hsb` function accepts:

| Input | Meaning |
|-------|---------|
| `top`, `{ top }` | Universe (ℝ^d) |
| `bot`, `bottom`, `{ bot }` | Empty set |
| `x[0] < 0` | Single open halfspace (x[0] < 0) |
| `0 <= x[0]` | Single closed halfspace (x[0] >= 0): read as `-x[0] <= 0` |
| `x[0] < 1` | Constant bound: read as `x[0] - 1 < 0` |
| `x[1] <= x[0]` | Variable bound (x[0] >= x[1]): read as `-x[0] + x[1] <= 0` |
| `x[0]*0.5 + 0.7 < 0` | Halfspace with coefficient and bias |
| `0.5*x[0] + -0.3*x[1] + 0.7 < 0` | Multivariate halfspace |
| `2*x[0] < x[1] - 1` | Linear expressions on both sides: read as `2*x[0] - x[1] + 1 < 0` |
| `(x[0] < 0 & x[1] < 0)` | Conjunction of halfspaces (parentheses required) |
| `((x[0] < 0 & x[1] < 0) \| ~(x[2] < 0))` | Disjunction and negation |

Coefficients can appear before (`0.5*x[0]`) or after (`x[0]*0.5`) the variable.
Both sides of a half-space are linear expressions (`linexpr < linexpr` or
`linexpr <= linexpr`); the parser moves the right-hand side to the left, so
`x[0] < 1` is `x[0] - 1 < 0`, `x[1] <= x[0]` is `-x[0] + x[1] <= 0`, and
`x[0] + x[1] < x[1] + 3` is `x[0] - 3 < 0`. A comparison whose variables all
cancel (`x[0] < x[0]`, `1 < 2`) is rejected. Coefficients and biases are
unsigned numbers written as integers, decimals (`0.5`) or fractions (`1/2`);
a leading `-` negates a term, and a fraction with a zero denominator is
rejected.

**Strictness is canonical, not chosen by the operator.** The algebra is the
Boolean algebra of *lex-half-open* polyhedra: for a normal vector `w` whose
first non-zero coefficient is positive the algebra contains only the open
half-space `⟨w,x⟩ + b < 0`, and for a negative leading coefficient only the
closed one `⟨w,x⟩ + b <= 0`; the complement of an element flips both the
sign and the strictness, which is what keeps the family closed under the
Boolean operations. So `x[0] < 1` (open, x[0] < 1) and `1 <= x[0]` (closed,
x[0] >= 1) are elements, while the closed set x[0] <= 1 and the open set
x[0] > 1 are not. A literal whose operator names one of the two missing
sets (`x[0] <= 1`, `1 < x[0]`) is rejected with an error that shows the
element the algebra does have (`x[0] - 1 < 0`, `-x[0] + 1 <= 0`); put the
leading variable on the other side of the comparison to name the other
bound. The printed form of an element always shows its canonical operator.
Inside `{...}:hsb`, `&`, `|` and `~` combine constraints; every `&`/`|`
combination must be parenthesised.  The same combinations are also available
through the Boolean algebra operations at the formula level.

The test suite (`src/boolean_algebras/hsb/tests/test_hsb.cpp`) covers Fourier-Motzkin elimination,
complement closure, splitter, Boolean combinations, parser, dispatcher
integration, and LTL(hsb) realizability.

#### `nlang` — Natural Language Boolean Algebra (experimental)

> **Note:** `nlang` is experimental.  Its semantics depend on an external LLM
> oracle (DeepSeek) and results may be non-deterministic or inconsistent across
> API calls.  Use for exploration and prototyping only.

`nlang` represents a Boolean algebra whose elements are natural language concept
descriptions (strings).  Boolean operations compose descriptions using English
connectives:

- `{ A }:nlang | { B }:nlang` produces `"(A) or (B)"`
- `{ A }:nlang & { B }:nlang` produces `"(A) and (B)"`
- `~{ A }:nlang` produces `"not (A)"`

An OpenAI-compatible chat completion endpoint serves as the semantic oracle
for equality, emptiness, and universality tests: the key is read from
`TAU_LLM_API_KEY` (or `OPENAI_API_KEY`), the base URL from `TAU_LLM_ENDPOINT`
(default `https://api.openai.com/v1`) and the model from `TAU_LLM_MODEL`
(unset: the endpoint's default); each request is capped by the
`nlang-http-timeout` option (15 s, or `TAU_NLANG_HTTP_TIMEOUT`).  Without a
key every emptiness,
universality and equivalence question is answered `false` (a warning is
printed once; the default is not cached), so verdicts over `nlang` elements
are not reliable without the key.

Elements are written as natural language strings inside `{...}:nlang`:

```
{ mammals }:nlang
{ large birds }:nlang
{ X | (Y & Z) }:sbf         -- not nlang; use plain strings for nlang
```

The content inside `{}` for `nlang` is a natural language phrase, with a small
structural layer: `nothing` and `everything` denote bottom and top, and
`not (φ)`, `(φ) and (ψ)`, `(φ) or (ψ)` compose phrases (this is the canonical
form the Boolean operations print).  Any other text is a single atom.

**Requirement**: `TAU_LLM_API_KEY` (or `OPENAI_API_KEY`) must be set in the
environment for the oracle to answer.

#### `bv` — bit width and unification

The bit width of a bitvector type is mandatory: `x : bv[8]` is well-formed, while
`x : bv` is not a bitvector annotation at all (see [Constants](#constants)). Each
width is a distinct type, so `bv[8]` and `bv[16]` never unify — unifying them is a
type error, not an implicit widening (use an explicit cast, see
[Bitvectors](#bitvectors)).

In the case of functional recurrence relations (as all arguments must have the
same type) the syntax is as follows:

```
f(x) : tau := ...
g[n](x, y) : sbf := ...
```

In the case of predicate recurrence relations, the syntax is different as each argument
can have its own type. The syntax is as follows:

```
p(x : tau) := ...
q[n](x : sbf, y : tau) := ...
```

In general, if no type information is present, the default type `tau` is assumed.
Also note that `bv` actually denotes a family of types, one for each bit width.
For example, `bv[8]` is the type of bitvectors of width 8, while `bv[16]` is the type
of bitvectors of width 16.

### Type inference

The Tau Language has a type inference system which infers types of all elements
that are not explicitly typed and detect type mismatches.

The inference system is base on a union-find data structure
and works by unifying types of elements appearing together in the same context.

Moreover, the inference system is scoped, i.e. types inferred within a certain scope
do not leak outside of that scope (unless the element is scoped in an outer scope).
For example, argument types inferred within a function definition do not leak outside of
that function definition.

Among others, we have the following scopes:

1. global scope: the scoped elements are untyped variables and streams of the formula,
2. recurrence relations definitions: the scoped elements are the arguments of the
recurrence relation (everything is resolved taking into account those type arguments):
variables, streams and constants,
3. existential and universal quantifications: the scoped elements are the
quantified variables, and
4. atomic formulas: the scoped elements are constants appearing in the atomic formula.

Depending on the scope, we also merge the types of the different elements to be typed:

1. untyped variables and streams in the global scope are assigned the default type `tau`,
2. in a recurrence relation definition:
      - in the functional case: everything is typed acoordingly to the type of
      the recurrence relation: argument variables, constants, streams,...
      - in the predicate case: everything is typed accordingly to the involved
      argument types: variables, constants, streams,...
3. in an existential or universal quantification:
      - the quantified variable is typed accordingly to the type inferred in the
      quantified formula,
4. in an atomic formula:
      - everything is merged together and typed accordingly: variables, streams,
      constants,...

If we have no type information, the default type `tau` is assumed.

### Examples

Here are some small examples to illustrate the type inference system:

1. `x = y && y = z:sbf`:
      - ``x` is inferred to be of the same type as `y`,
      - `z` is typed as `sbf`,
      - `y` is inferred to be of the same type as `z`, i.e. `sbf`,
      - no type mismatch occurs.

2. `all x:bv[16] x = y`:
      - `x` is typed `bv[16]`,
      - `y` is inferred to be of the same type as `x`, i.e. `bv[16]`,
      - no type mismatch occurs.
3. `all x x = y`:
      - `x` is inferred to be of the default type `tau`, as no type information is present,
      - `y` is inferred to be of the same type as `x`, i.e. `tau`,
      - no type mismatch occurs.
4. `(all x x = y) && x = y:sbf`:
      - `x` is inferred to be of the same type as `y` in the first part, i.e. `tau`, as no type information is present,
      - `y` is inferred to be of type `sbf` in the outer formula which is not compatible with the previous type assigned (`tau`).
      - a type mismatch occurs.
5. `all x (all x x = 1:sbf)`:
      - the inner `x` is inferred to be of type `sbf`,
      - the outer `x` is inferred to be of the default type `tau`,
      - no type mismatch occurs as both `x` are in different scopes.
6. `ex x x = 1 : bv[8]`:
      - the constant `1` is typed as `bv[8]` (it is the all-ones vector `{255}:bv[8]`, see [`0` and `1` in bitvectors](#0-and-1-in-bitvectors)),
      - `x` is inferred to be of the same type as the constant `1`, i.e. `bv[8]`,
      - no type mismatch occurs.
7. `x:bv[8] = {1}:bv[16]`:
      - `x` is typed as `bv[8]` and the constant as `bv[16]`,
      - the two bitvector widths are distinct types and do not unify,
      - a type mismatch occurs.

## **Abstract data types**

Besides the base types described in [Type system](#type-system), the Tau
Language supports user-defined types, declared with the keyword `type`. A type
definition is a definition like any other (see
[Tau specifications](#tau-specifications)): as part of a specification it is
terminated by `.`, and it can equally be entered on its own in the REPL, where
it persists for the session and is listed by `definitions|defs`.

The syntax is:

```
type_def     => "type" name [ ("of" | "/") type_parents ] ("is" | "=") type_body
type_parents => "(" name ("," name)* ")"
type_body    => tuple | type
tuple        => "{" member ("," member)* "}"
member       => name ":" type
```

The spellings `of (...)` and `/ (...)` are interchangeable, and so are `is` and
`=`. Type definitions may reference types declared later in the same
specification — resolution is order-independent.

### Aliases

The simplest form introduces a new name (an *alias*) for an existing type:

```
type byte = bv[8].
```

Anywhere a type annotation is accepted, `byte` now means `bv[8]`, so
`x:byte = x:bv[8]` holds. Commands that report values, such as `solve`, resolve
the alias to its underlying type in their output.

### Tuples

A *tuple* type collects several named members, each with its own type:

```
type Point = {a: sbf, b: sbf}.
```

An equality between two values of a tuple type expands member-wise, and the
expansion happens at parse time, before any normalization: `x:Point = y:Point`
is read as `x.a = y.a && x.b = y.b`.

An individual member is reached with the `.` operator: `p.a` is member `a` of
`p`. Member types may themselves be tuple types, nesting to arbitrary depth,
and a member path then chains accesses: `s.line.p.a`. Member-wise expansion
works at any depth, not only at the leaves, so an equality between two
tuple-typed *members* again expands to all their flattened members. When a
member is accessed on a quantified variable, annotate the variable at the
binder and parenthesize the body, so that the member access sees the
annotation:

```
ex l:Line (l.tag = 0 && l.p.a = 0)
```

`solve` reports one value per flattened member, not a single tuple value.

### Inheritance

A tuple type may inherit the members of one or more other tuple types:

```
type Tagged = {tag: byte}.
type Line of (Tagged) is {p: Point, q: Point}.
```

The parents' members come first, in declaration order, followed by the type's
own members, so `Line` flattens to `tag, p.a, p.b, q.a, q.b`.

### Errors and re-declaration

A type definition is rejected with an error when it declares a duplicate type
name within one specification, a duplicate member (including one introduced
via inheritance), a cycle (through members, aliases or parents), or when it
inherits from something that is not a tuple type or is not declared. An
undeclared name in a *member* position is not an error — it is treated as a
base (non-ADT) type and passed through unchanged.

Re-declaring a type name in a later REPL command replaces the earlier
definition: the last definition wins. The same holds for repeated stream
definitions (tuple-typed ones included) — note, however, that a stream name
keeps the type it was first declared with for the rest of a REPL session.

### Tuple-typed streams

A stream may be declared with a tuple type; it then reads and writes all its
members together, as one value, in a JSON-like wire format that nests exactly
like the type declaration (not like the dotted member paths):

```
type Point = {a: sbf, b: sbf}.
i : Point := in console.
o : Point := out console.
o[t] = i[t]
```

with console input (and output) of the form:

```
{ a: "1", b: "0" }
```

Unlike a `tau`-valued stream, the wire literal is complete on its own line —
no terminating period is needed. File streams work the same way, one wire
literal per line.

A specification may also copy just *part* of a tuple between streams, e.g.
`(o[0].a = i[0].a)`. A member the specification does not copy at some time
point is not left dangling: the emitted tuple initializes it to the default
value of that member's own Boolean algebra, which is that algebra's `0`.
Beware, however, that a member the specification never mentions at *any* time
point has no output stream at all, and the tuple, missing one member, is then
never written.

Many worked examples can be found in
[`demos/demo_4.1-abstract_data_types.tau`](demos/demo_4.1-abstract_data_types.tau).

## **Constant time constraints**

Besides the relations between terms, a formula can also constrain the *time point*
at which it applies. Such a constraint is written in square brackets and compares
a time variable against a constant:

```
time_constraint => "[" ctnvar rel num "]" | "[" num rel ctnvar "]"
rel             => "=" | "!=" | "<" | "<=" | ">" | ">="
```

where `ctnvar` is a time variable (typically `t`) and `num` a non-negative
integer. The constraint evaluates to `T` at the time points that satisfy it and
to `F` elsewhere, so it acts as a guard on the rest of the formula. For example:

```
[t <= 3] && o1[t] = 0
```

only holds during the first four time steps, whereas

```
always ([t < 3] -> o1[t] = 0) && ([t >= 3] -> o1[t] = 1)
```

writes `0` into `o1` for the first three time steps and `1` afterwards.

Note that the operand order matters and is *not* normalized: `[t >= 3]` means
"the current time point is at least 3", whereas `[3 >= t]` means "3 is at least
the current time point", i.e. at most 3.

## **Pointwise revision**

As mentioned in the beginning, pointwise revision refers to the feature to incorporate
updates into a currently running specification. To this end, the special output stream `u`
is introduced. A Tau specification written into this stream, will be interpreted as a potential
update. If the proposed update is not satisfiable as a stand-alone specification, no update is performed. Otherwise,
the pointwise revision procedure is called, updating the currently running specification with the specification
written into `u`.

### Minimal example

Let us work with a concrete, minimal example to make the idea more clear. Afterwords,
the algorithm for pointwise revision is explained in more detail.

Suppose we execute the specification `u[t] = i1[t]`. By default, both streams are of type `tau`.
This can be done in the REPL by just entering
```
run u[t] = i1[t]
```
`u[t]` is the special output stream, while `i1[t]` is an ordinary input stream.
Hence, the specification says that the input into `i1` is written into `u` in each step of execution.
We now input `o1[t] = 1` for `i1[0]`. This will yield the following output:
```
u[0] := o1[t] = 1
```
Since `o1[t] = 1` is a satisfiable specification, the update is accepted and in
the following step 1, the running Tau specification is
```
u[t] = i1[t] && o1[t] = 1
```
If we now input `o2[t] = 0 && o2[t] = 1` for `i1[1]` in the next step,
no update is performed, because `o2[t]` cannot be 0 and 1 at the same time. So this
is an example of a very simple unsatisfiable specification.

Finally, in the next step we input `o1[t] = 0` for `i1[2]` as a proposed update. This is a satisfiable specification but contradicts
the currently running specification (we required `o1[t] = 1`). The pointwise revision algorithm will replace the
previous specification with the update, yielding just `o1[t] = 0`,
in order to ensure that the new specification is satisfiable.

Running this example in REPL (see [The Tau REPL](#the-tau-repl) below) yields:
```
tau> run u[t] = i1[t]

Temporal normalization of G specification reached fixpoint after 0 steps, yielding the result:
i1[t]u[t]' = 0 && i1[t]'u[t] = 0

-----------------------------------------------------------------------------------------------------------
Please provide requested input, or press ENTER to terminate                                               |
If no input is requested, press ENTER to continue to the next execution step, or type q(uit) to terminate |
-----------------------------------------------------------------------------------------------------------

Execution step: 0
i1[0] := o1[t] = 1
u[0] := G o1[t]' = 0

Updated specification:
G o1[t]' = 0 && i1[t]u[t]' = 0 && i1[t]'u[t] = 0

Execution step: 1
i1[1] := o2[t] = 0 && o2[t] = 1
o1[1] := T
u[1] := F

Execution step: 2
i1[2] := o1[t] = 0
o1[2] := T
u[2] := G o1[t] = 0

Updated specification:
G o1[t] = 0

Execution step: 3
o1[3] := F
u[3] := F

Execution step: 4
o1[4] := F
u[4] := F
...
```
Note, in order to interpret the output, that `1` of type `tau` is represented as `T` and `O` as `F`.
Furthermore, Tau specifications are always displayed normalized during execution.
The REPL informs the user whenever an update was done successfully by printing the new, updated specification.

### The *this* input stream

Aside from `u` there is another special stream which can be utilized in specifications incorporating updates.
The special stream is called `this` and behaves in the same way as an input stream. However, the input is
not asked from the user. Instead, the currently running specification is written into the `this` stream at each
point in time. For example the following specification always prints the currently running specification
using the output stream `o1`: `u[t] = i1[t] && o1[t] = this[t]`.

### Pointwise revision details

The following is a detailed explanation of the pointwise revision algorithm. Pointwise revision is performed
at the end of an execution step in which a Tau specification, let's call it `update`, is written
into the output stream `u`. First, it is checked if `update` is satisfiable. This means that
for any given input at any given step during execution, there has to exist output satisfying the specification,
where in this later case the standard definition of satisfiability for a logical formula is meant.
If `update` is not satisfiable, it normalizes to `F` and no update is applied.

In case `update` is satisfiable, the following steps are performed:

1) `update` can refer to previous memory positions by using negative numbers in a stream index,
for example `o1[-k]`. Let `t` be the current time point of execution. Then `o1[-k]` is replaced with
the value at `o1[t-k]`. If no such memory position is present, no update is performed. Furthermore,
`t-k` must not be below 0. After replacing all such streams with
the respective value from the memory, it is checked again if `update` is satisfiable given these memory references.
If it is unsatisfiable, no update is performed. Otherwise, we move to the next step:
2) Let us refer to the currently running specification as `spec`. `spec` is composed of a single `G` statement
and possibly several `F` statements. We denote the `G` part by `aw_spec` and the collection of
`F` parts by `st_spec`. In the same way `aw_update` denotes the `G` part of `update`.
The next candidate for the updated specification, let's call it `U`, is given by <br>
`U := update && ( (ex [outputs] aw_update && aw_spec) -> aw_spec)`, where `[outputs]` refers to the list of all output streams present
in `aw_spec` and `aw_update` combined. The meaning is that, whenever possible given the current input at a particular step,
`update` and `aw_spec` are executed together. The name _pointwise revision_ originates from this behavior.
Note that it is possible to refine the definition of the new specification
in more advanced ways. We will explore this aspect in the future. <br>
If `update` is satisfiable, then `U` is satisfiable,
unless the `F` part of `update`
prevents it. If it is prevented, `update` becomes the final updated specification.
Otherwise, `U` becomes the updated specification and the next step is performed.
3) As a final step, it is checked if the previous `F` statements `st_spec` are executable along the updated specification `U`.
If this is the case, they are added to the updated specification. Otherwise,
`U` is accepted as the final update.

Note that in the step after an update was successfully applied, the new specification starts
running as if it was started at time step 0 shifted to the correct time step to match the overall
history. This means, in particular, that streams with lookback `k` only become solvable once the
specification has continued for at least `k` steps. For example, updating a specification in
step `s` with `o1[t] = i1[t-1]` means that in the next step `o1[s+1]` is unspecified.
To see this, assume we start at step 0. Then `o1[t] = i1[t-1]` will leave `o1[0]` unspecified
since `i1[-1]` is not defined, since we do not allow defining negative time steps in general. The only
exception is during pointwise revision in order to allow access to previous stream values,
as explained in step 1 above.

## **Reserved symbols**

Tau Language has a set of reserved symbols that cannot be used as identifiers.
In particular, we require that `T` and `F` are reserved for truth values in Tau specifications
and `0` and `1` stand for the corresponding Boolean
algebra elements. The names `min` and `max` are reserved for the builtin
bitvector operations (see [Bitvectors](#bitvectors)): a two-argument call
`min(x, y)` or `max(x, y)` always denotes the builtin, never a user-defined
function of the same name.

# **Command line interface**

Tau-lang ships one executable, `tau`, with two roles:

- **interpreter / REPL** — runs a specification by solving each time step
  through the core solver pipeline.  Best for REPL use, spec authoring, and
  executing specifications dynamically.
- **`tau compile`** — the ahead-of-time compiler.  Reads a specification
  file, invokes the synthesis pipeline once, and builds a standalone
  executable that steps the synthesized strategy (see
  [`tau compile`](#tau-compile--synthesis-to-executable-compiler)).

Both roles share the same spec format.  Use whichever matches your deployment
story.

## `tau` — interpreter and REPL

The general form of the tau executable command line is:

```bash
tau [ options ] [ <specification file> ]
```

where `[ options ]` are the command line options and `[ <specification file> ]` is
the path to a file containing the Tau specification you want to run. Use `-` to
read the specification from standard input. If you omit the file, the Tau REPL
will be started.

The general options are the following:

| Option             | Description                                             |
|--------------------|---------------------------------------------------------|
| -h, --help         | detailed information about options                      |
| -l, --license      | show the license                                        |
| -v, --version      | show the version of the executable                      |
| -V, --charvar      | char-as-variable short form (enabled by default)        |
| -B, --preprocessing | master switch for BA-specific preprocessing passes, e.g. bv predicate blasting (enabled by default) |
| -K, --ba-component-factoring | decide tau-algebra constants per support component (enabled by default) |
| -S, --severity     | severity level (trace/debug/info/error); default `info` |
| -I, --indenting    | indent formulas in output                               |
| -H, --highlighting | syntax highlighting                                     |
| -b, --benchmarks   | print benchmarks (enabled by default)                   |
| -J, --json         | output in JSON format                                   |
| -q, --quit         | quit when no input is available (scripted runs)         |

Whereas the REPL specific options are:

| Options            | Description                                            |
|--------------------|--------------------------------------------------------|
| -e, --evaluate     | REPL command to be evaluated                           |
| -s, --status       | display status (enabled by default)                    |
| -c, --color        | use colors (enabled by default)                        |
| -X, --legacy-repl  | use the legacy terminal REPL instead of the FTXUI one  |
| -x, --experimental | enable transitioning features                          |
| -d, --debug        | debug mode (Debug builds only)                         |

and the limit options, which bound the engine's iterative searches. Every
cap defaults to unlimited (`0`) except the two temporal-normalization caps
(`--max-fixpoint-steps`, `--max-flag-search-steps`), which ship at `500`
because those searches have no termination guarantee — unlimited turns a
non-converging spec from a loud give-up into a hang (pass `0` to opt in);
`--ba-decision-pins` (4096, 0 = none) and `--max-probe-steps` (10000);
`--spec-size-warn`'s `0` means off; and the two gc knobs keep their tuned
defaults. Each has a matching REPL option (see [REPL options](#repl-options)):

| Option                        | Description                                                                            |
|-------------------------------|----------------------------------------------------------------------------------------|
| -w, --spec-size-warn          | warn when an updated specification exceeds this many characters (0 = off)              |
| -a, --max-revision-alts       | cap the revision alternatives kept per specification part, dropping middle preference tiers (0 = unlimited) |
| -Z, --pwr-semantic            | enable the semantic (winning-region) fallback of the temporal pointwise revision (off by default) |
| -t, --step-definitional-propagation | propagate the constants a step formula determines before its paths are enumerated, one path instead of 2^k for k guards reading them (on by default) |
| -p, --block-max-splits        | cap per-block Boole-decomposition splits in anti-prenexing (0 = unlimited)             |
| -r, --block-max-rounds        | cap anti-prenexing quantifier-block driver rounds (0 = unlimited)                      |
| -N, --ba-decision-pins        | decided tau-algebra rows whose key tree is kept alive across the step sweep (default 4096, 0 = none) |
| -Q, --cqe-max-clauses         | cap the DNF clauses complete quantifier elimination may distribute one scope into (0 = unlimited) |
| -g, --lgrs-max-vars           | hand a pure-equality bitvector system with more distinct variables than this to the solver instead of squeezing it per width and computing a ground solution algebraically, whose Boole expansion is exponential in them (default 8, 0 = unlimited) |
| -f, --max-fixpoint-steps      | cap temporal-normalization fixpoint steps; a give-up reports an error, not a verdict (default 500; 0 = unlimited) |
| -F, --max-flag-search-steps   | cap the eventual-flag search past the flag boundary; a give-up reports an error, not a verdict (default 500; 0 = unlimited) |
| -z, --block-squeeze-cap       | skip block squeezing above this operand-set size (0 = unlimited)                       |
| -m, --max-simplify-rounds     | cap bitvector simplification rewrite rounds (0 = unlimited)                            |
| -P, --max-def-passes          | cap definition-expansion passes (0 = unlimited)                                        |
| -E, --max-enum-steps          | cap recurrence-relation enumeration steps (0 = unlimited)                              |
| -M, --max-probe-steps         | cap the untyped saturation probe over a residual recurrence reference (default 10000, 0 = unlimited) |
| -R, --max-rewrite-rounds      | cap rewrite-to-fixpoint rounds (0 = unlimited)                                         |
| -G, --gc-min-size             | tree-node count floor before gc may trigger (default 256)                              |
| -W, --gc-growth-factor        | gc triggers when node count grows by this factor since last sweep (default 1.5; <= 0 disables gc) |
| -y, --tref-budget             | cap the live interned tree nodes; an api call that starts with the store at or above the cap fails instead of running (default `TAU_TREF_BUDGET` or 0; 0 = unlimited) |
| -C, --tref-budget-soft        | percentage of `--tref-budget` at which a sweep is forced regardless of the gc growth trigger (default `TAU_TREF_BUDGET_SOFT` or 75) |
| -j, --max-consistency-subsets | cap k-ary consistency subset checks per atom group in LTL(ABA) synthesis (default 4096; 0 = unlimited) |
| -n, --max-cover-products      | cap the ABA oracle's mixed-type coverage expansion (default 256; 0 = unlimited)        |
| -A, --cache-bound             | bound the string-keyed synthesis caches, FIFO eviction (default 4096; 0 = unbounded)   |
| -T, --ltl-timeout             | wall-clock cap in seconds on each `ltlsynt` call (0 = no watchdog; default `TAU_LTL_TIMEOUT_SEC` or 60) |
| -L, --ltl-alg                 | omcat synthesis algorithm: `A`, `B`, `D` or `auto` (default `TAU_LTL_ALG` or `auto`)     |
| -k, --ltl-qe-max-vars         | free-variable cap of the omcat QE fast path; above 2 is not sound (0 = `TAU_LTL_OMCAT_QE_MAX_VARS` or 2) |
| -Y, --ltl-hoa-max-states      | largest state count accepted from an `ltlsynt` HOA strategy (default `TAU_LTL_HOA_MAX_STATES` or 4194304; 0 = unlimited) |
| -U, --ltl-guard-max-cubes     | cap the DNF cubes a HOA guard may expand into in the Algorithm D game (default `TAU_LTL_GUARD_MAX_CUBES` or 512; 0 = unlimited) |
| -D, --ltl-refinement-rounds   | cap the ABA-oracle refinement rounds of a realizability check; the cap answers UNKNOWN (default `TAU_LTL_REFINEMENT_ROUNDS` or 64; 0 = unlimited) |
| -O, --ltl-window-max-paths    | cap the strategy paths the multi-step window oracle examines per check (default `TAU_LTL_WINDOW_MAX_PATHS` or 4096; 0 = unlimited) |

Beyond these, each Boolean algebra in the configured pack (`-DTAU_BAS=`, see
"Selecting Boolean algebras" above) may declare CLI options of its own,
addressed `--<ba>-<option>`, and present when that BA is part of the build. bv, for instance, declares `--bv-blasting` (bv's own
predicate-blasting switch, enabled by default), `--bv-blastdepth` (cap
blast-block re-entry nesting in anti-prenexing, 0 = unlimited),
`--bv-case-split` (bitvector case split of quantified variables tested
against constants, enabled by default), `--bv-case-split-max-tests` (cap
the constants a quantified bitvector variable may be tested against for the
case split, 0 = unlimited), `--bv-definitional-elimination` (eliminate
existentially quantified bitvector variables that a total definition in
their scope determines, before the case split; on by default) with its
caps `--bv-defelim-max-clauses`, `--bv-defelim-max-atoms`,
`--bv-defelim-max-subset` and `--bv-defelim-max-rounds` (flattened clauses
per conjunct, guard atoms per propositional check, clause-subset size and
rounds per block; defaults 16, 18, 4 and 256, `0` = unlimited except for
the atoms, which stop at 30), `--bv-quantifier-free-decision` (decide a
closed bitvector formula whose binders are all of one kind quantifier-free,
off by default), `--bv-widening` (exact, widened bitvector arithmetic
instead of modular wraparound, off by default) and `--bv-max-width` (cap
the width widening may compute at; `0` leaves the current cap unchanged,
1024 unless already set); bv blasts only when both `--preprocessing`/`-B`
and `--bv-blasting` are on. In a build without bv, `--bv-blasting`,
`--bv-blastdepth`, `--bv-case-split`, `--bv-case-split-max-tests`,
`--bv-definitional-elimination`, the four `--bv-defelim-max-*` caps,
`--bv-quantifier-free-decision`, `--bv-widening` and `--bv-max-width` are
not recognized options at all.

## `tau compile` — synthesis-to-executable compiler

```bash
tau compile <spec.tau> [ -o <exe> ] [ -c <c++ compiler> ]
```

Parses the spec file (a file argument, not stdin), synthesizes it, emits
`<spec.tau>.build/` (a `main.cpp` driving the strategy plus a `CMakeLists.txt`
that links the emitting build's prebuilt libTAU and its algebra libraries
with the same compile definitions), runs `cmake` configure and build, and
places the program at `-o <exe>` (default: the spec path without its
extension).  The program behaves like `tau <spec.tau>`: it reads inputs,
prints outputs, and exits when its input closes.  Exit code `0` on success,
`1` on every failure, with the reason in the `compile failed:` message (see
[Compile a spec to an executable](#compile-a-spec-to-an-executable-tau-compile)).

Emitting a C++ *header* with the synthesized class (`tau_program`, with the
`declare_open` oracle-callback surface shown in `examples/declare_open_codegen/`)
is a library operation: `build_program_desc` + `emit_program` in
`src/cpp_codegen.h`; there is no CLI flag for it.

## When to use which

| Situation                                               | Use         |
|---------------------------------------------------------|-------------|
| Interactive spec authoring / debugging                  | `tau` REPL  |
| One-off running of a spec against inputs                | `tau`       |
| Checking satisfiability / realizability of a spec       | `tau`       |
| Deploying a realized spec as one binary                 | `tau compile` |
| Integrating the synthesized behaviour into a C++ project | the `cpp_codegen.h` library API (`emit_program`) |
| Specs with data atoms requiring runtime witness search  | both: the compiled program solves witnesses per step through libTAU, as the interpreter does |

# **The Tau REPL**

The Tau REPL is a command line application that allows you to interact with the Tau
Language. It is a simple and easy to use tool that enables you to write and
execute Tau specifications on the fly.

By default the REPL uses a full-screen terminal interface based on
[FTXUI](https://github.com/ArthurSonzogni/FTXUI), with command history persisted
in `.tau_history`. Pass `-X` (`--legacy-repl`) for the plain line-oriented REPL,
which is also what you get if the project was configured with
`-DTAU_DONT_USE_FTXUI=ON`.

## **Basic REPL commands**

The Tau REPL provides a set of basic commands that allow you to obtain help,
version information, exit the REPL and clear the screen. The syntax of
the commands is the following:

* `help|h [<command>]`: shows a general help message or the help message of a
specific command.

* `help|h example[s]`: shows examples of the Tau language syntax.

* `version|v`: shows the version of the Tau REPL. The version of the Tau REPL
corresponds to the repo commit.

* `quit|q`: exits the Tau REPL.

* `clear|c`: clears the screen.

* `whatis <tau|term|repl_history>`: shows the inferred type of an expression.

* `reset`: returns the session to the state it started in. It stops a run in
  progress, clears the history, the definitions, the input/output streams and
  the caches, and frees the memory nothing uses any more. Options keep their
  values.

* `fragment ltl|ctl_star`: selects the grammar fragment; `ctl_star` adds the
`A`, `E` and `-` operators (see [CTL\* fragment](#ctl-fragment-and-semantic-negation)).

## **REPL options**

You have several options at your disposal to configure the Tau REPL. In order
to set or get the value of an option you can use the following commands:

* `get [<option>]`: shows all configurable settings and their values or a single
one if its name is provided.

* `set <option> [=] <value>`: sets a configurable option to a desired value.

* `enable <option>`: sets a boolean option to on.

* `disable <option>`: sets a boolean option to off.

* `toggle <option>`: toggle an option between on/off.

The options you have at your disposal are the following:

* `c|color|colors`: Can be on/off. Controls usage of terminal colors in its
output. It's on by default.

* `s|status`: Can be on/off. Controls status visibility in the prompt. It's on
by default.

* `S|sev|severity`: Possible values are trace/debug/info/error. The value determines
how much information the REPL will provide. It's `info` by default in Release
builds and `debug` in Debug builds.

* `H|highlight|highlighting`: Can be on/off. Controls usage of
highlighting in the output of commands. It's off by default.

* `I|indent|indenting`: Can be on/off. Controls usage of indentation in the
output of commands. It's off by default.

* `V|charvar`: Can be on/off. Controls usage of character variables in the
REPL. It's on by default.

* `B|preprocessing`: Can be on/off. Master switch for every BA-specific
preprocessing pass, e.g. bv's own predicate blasting (see below) — off
disables all of them regardless of their own setting. It's on by default.

* `factoring|bacomponentfactoring`: Can be on/off. Controls support-component factoring of the
tau-algebra constant tests: a constant whose clauses share no variables is
decided per component, each decision remembered across steps, instead of as a
whole. It's on by default (the REPL starts with the value of the
`-K, --ba-component-factoring` command line option).

* `Z|pwrsemantic`: Can be on/off. Enables the semantic (winning-region)
fallback of the temporal pointwise revision, the mode that re-solves the
revised specification as an Algorithm D game over the `qlt` type
(`-Z, --pwr-semantic`). It's off by default.

* `stepprop|stepdefinitionalpropagation`: Can be on/off. Before a step's
paths are enumerated, normalizes the step formula once, substitutes every
top-level `o = c` with `c` a constant and repeats until no new constant
appears, carrying the values into the solution. A guard reading a value the
same step computes then folds instead of forking the enumeration, so a step
with `k` such guards solves one path instead of up to `2^k`. An identity on
the solution set (`-t, --step-definitional-propagation`). It's on by default.

* `b|benchmarks|benchmarking`: Can be on/off. Controls printing of timing
benchmarks after each command. It's on by default.

* `d|dbg|debug`: Can be on/off. Controls debug mode. Only available in Debug
builds, where it's on by default.

Besides the boolean options above, the REPL exposes every limit option as a
numeric option. These take a count via `set <option> <n>` (so `enable`,
`disable` and `toggle` do not apply); `0` means unlimited, except for the two
gc knobs, which keep their tuned defaults (`gcgrowth` at or below `0` disables
gc). Each mirrors the command line option shown alongside:

* `maxsplits|blockmaxsplits`: per-block Boole-decomposition split budget in
anti-prenexing (`--block-max-splits`). Unlimited by default.

* `maxrounds|blockmaxrounds`: anti-prenexing quantifier-block driver round cap
(`--block-max-rounds`). Unlimited by default.

* `decisionpins|badecisionpins`: how many decided tau-algebra rows keep their key tree alive
across the interpreter's step sweep, oldest released first
(`--ba-decision-pins`). 4096 by default; `0` disables the pinning (a raw
count, not "unlimited").

* `maxclauses|cqemaxclauses`: cap on the DNF clauses complete quantifier
elimination may distribute one scope into (`--cqe-max-clauses`). Unlimited by
default.

* `lgrsmaxvars`: above this many distinct variables, a partition of pure
bitvector equalities is handed to the solver instead of being squeezed per
width and given a ground solution algebraically (`find_solution`, or
`find_minimal_solution` in minimum mode), whose Boole expansion is
exponential in the variables;
`var = constant` conjuncts are read off before the count (`--lgrs-max-vars`).
8 by default.

* `fixpointsteps|maxfixpointsteps`: temporal-normalization fixpoint step cap
(`--max-fixpoint-steps`). Default 500 — the search has no convergence
guarantee, so unlimited (`0`) hangs on a non-converging spec instead of giving
up loudly. A give-up is reported as an error ("gave up before reaching a
result"), never as a `T`/`F` verdict.

* `flagsteps|maxflagsearchsteps`: cap on the eventual-flag search past the
flag boundary (`--max-flag-search-steps`). Default 500, for the same reason
as `fixpointsteps`; a give-up is likewise an error, not an unsatisfiable
verdict.

* `squeezecap|blocksqueezecap`: operand-set size above which block squeezing
declines (`--block-squeeze-cap`). Unlimited by default.

* `simplifyrounds|maxsimplifyrounds`: bitvector simplification rewrite round
cap (`--max-simplify-rounds`). Unlimited by default.

* `defpasses|maxdefpasses`: definition-expansion pass cap
(`--max-def-passes`). Unlimited by default.

* `enumsteps|maxenumsteps`: recurrence-relation enumeration step cap
(`--max-enum-steps`). Unlimited by default.

* `probesteps|maxprobesteps`: cap on the untyped saturation probe that
`calculate_fixed_point` runs over a residual recurrence reference to tell a
type-blocked rule from a legitimately uninterpreted one (`--max-probe-steps`).
10000 by default, since a diverging probe never stabilizes; a finite
`enumsteps` tightens it further; 0 = unlimited.

* `rewriterounds|maxrewriterounds`: rewrite-to-fixpoint round cap
(`--max-rewrite-rounds`). Unlimited by default.

* `gcminsize`: tree-node count floor before the interpreter's gc may trigger
(`--gc-min-size`). 256 by default.

* `gcgrowth|gcgrowthfactor`: gc growth-factor trigger; accepts decimals such
as `1.5` (`--gc-growth-factor`). 1.5 by default; a value at or below 0
disables gc.

* `trefbudget`: cap on the live interned tree nodes (`--tref-budget`). A
command that starts with the store at or above the cap fails without running;
one that was allowed to start finishes even if it ends above it. Unlimited by
default, or `TAU_TREF_BUDGET` when that is set.

* `trefbudgetsoft`: percentage of `trefbudget` at which the interpreter sweeps
regardless of its gc growth trigger (`--tref-budget-soft`). 75 by default, or
`TAU_TREF_BUDGET_SOFT` when that is set.

* `specsizewarn`: warn when an updated specification exceeds this many printed
characters (`--spec-size-warn`). 0 (off) by default.

* `revisionalts|maxrevisionalts`: cap on revision alternatives kept per
specification part (`--max-revision-alts`). Unlimited by default.

* `maxsubsets`: cap on the k-ary consistency subset checks per atom group in
LTL(ABA) synthesis (`--max-consistency-subsets`). 4096 by default; a fired cap
is sound but may answer unrealizable.

* `maxcoverproducts`: cap on the ABA oracle's mixed-type coverage expansion
(`--max-cover-products`). 256 by default.

* `cachebound`: bound on the string-keyed synthesis caches, with FIFO eviction
(`--cache-bound`). 4096 by default; 0 = unbounded.

* `ltltimeout`: wall-clock cap in seconds on each `ltlsynt` call
(`--ltl-timeout`). 60 by default, or `TAU_LTL_TIMEOUT_SEC` when that is set;
0 disables the watchdog. `get ltltimeout` shows the effective value.

* `ltlalg`: the omcat synthesis algorithm, `A`, `B`, `D` or `auto`
(`--ltl-alg`). `auto` by default, or `TAU_LTL_ALG` when that is set.

* `ltlqemaxvars`: free-variable cap of the omcat quantifier-elimination fast
path (`--ltl-qe-max-vars`). 2 by default, or `TAU_LTL_OMCAT_QE_MAX_VARS` when
that is set; values above 2 re-enable a fast path that is not sound.

* `ltlhoamaxstates`: largest state count accepted from an `ltlsynt` HOA
strategy (`--ltl-hoa-max-states`). 4194304 by default, or
`TAU_LTL_HOA_MAX_STATES` when that is set; 0 = unlimited. `get` shows the
effective value, as it does for every limit below.

* `ltlguardmaxcubes`: cap on the DNF cubes a HOA guard may expand into in the
Algorithm D product game (`--ltl-guard-max-cubes`). 512 by default, or
`TAU_LTL_GUARD_MAX_CUBES` when that is set; 0 = unlimited.

* `ltlrefinementrounds`: cap on the ABA-oracle refinement rounds of one
realizability check, each round blocking an infeasible strategy edge and
re-running `ltlsynt` (`--ltl-refinement-rounds`). 64 by default, or
`TAU_LTL_REFINEMENT_ROUNDS` when that is set; 0 = unlimited. On the cap the
verdict is an error (UNKNOWN), never a false answer.

* `ltlwindowmaxpaths`: cap on the strategy paths the multi-step window oracle
examines per check (`--ltl-window-max-paths`). 4096 by default, or
`TAU_LTL_WINDOW_MAX_PATHS` when that is set; 0 = unlimited; a hit cap
likewise answers UNKNOWN.

Changing any of these, the two temporal-normalization caps, `preprocessing`
or an option an algebra declares (below) between two queries drops the
verdict memos, so the next `sat`/`realizable` is decided
under the new budgets rather than answered from the old ones.

Beyond the options above, each Boolean algebra in the configured pack may
expose options of its own, addressed `<ba>-<option>` and reachable when that
BA is part of the build; `set`/`get`/`enable`/`disable`/`toggle` route such a
name to its owning BA the same way they route a bare name to a core option. bv, for
instance, declares `bv-blasting` (its own predicate-blasting switch,
mirroring `--bv-blasting`; on by default, but effective only while the
master `preprocessing` above is also on), `bv-blastdepth` (blast-block
re-entry nesting cap in anti-prenexing, mirroring `--bv-blastdepth`;
unlimited by default), `bv-case-split` (the bitvector case split: a
quantified bitvector variable that occurs only in comparisons against
constants of its type is eliminated by one witness per cell those constants
cut the domain into, before any quantifier block forms; mirroring
`--bv-case-split`, on by default), `bv-case-split-max-tests` (cap on the
constants a quantified bitvector variable may be tested against for the case
split to apply, mirroring `--bv-case-split-max-tests`; unlimited by default),
`bv-definitional-elimination` (substitute an existentially quantified
bitvector variable that a total definition in its scope determines -- a bare
equation, or clauses `D_i || x = c_i` whose guards cover every case -- where
it is read, and drop its binder, before the case split; mirroring
`--bv-definitional-elimination`, on by default) with its caps
`bv-defelim-max-clauses`, `bv-defelim-max-atoms`, `bv-defelim-max-subset` and
`bv-defelim-max-rounds` (mirroring the command line options of the same
names), `bv-quantifier-free-decision` (decide a closed bitvector formula whose
binders are all of one kind quantifier-free, mirroring
`--bv-quantifier-free-decision`; off by default), `bv-widening` (the
[exact, widened bitvector arithmetic mode](#exact-widened-arithmetic-mode),
mirroring `--bv-widening`; off by default) and `bv-max-width` (cap on the
width widening may compute at, mirroring `--bv-max-width`; 1024 by default,
and unlike the budgets above it is a hard ceiling that is never unlimited,
so `set bv-max-width 0` leaves the current value unchanged).
In a session built without bv, `set bv-blasting off` reports `No BA named
'bv' in this pack (...)` instead of changing anything.

## **Functions, predicates and input/output stream variables**

As in other programming languages, you can define functions, predicates (both possibly using recurrence
relations), types, but also input and output stream variables. The syntax of the commands
is the following:

* `definitions|defs`: shows all the definitions of the current session. That
includes the definitions of functions, predicates, types and the input/output stream variables.

* `definitions|defs <number>`: shows the definition of the given function or predicate.

* `predicate_def`: defines a predicate, supporting the usage of
recurrence relations. See the Tau Language section
[Functions and predicates](#functions-and-predicates) for more information.

* `function_def`: defines a function, supporting the usage of
recurrence relations. See the Tau Language section
[Functions and predicates](#functions-and-predicates) for more information.

* `type_def`: defines an abstract data type (an alias or a tuple type). See
the Tau Language section [Abstract data types](#abstract-data-types) for more
information.

* `<name> [: <type>] := in console | in file(<filename>)`: defines an input stream
variable. The input variable can read values from the console or from a provided
file.

* `<name> [: <type>] := out console | out file(<filename>)`: defines an output
stream variable. The output variable can write values to the console or into a
file.

In both cases `<name>` is any stream name and `<type>` is `tau`, `sbf` or `bv[n]`
(see [Streams](#streams) and [Type system](#type-system)). If the type is omitted
the stream is left untyped and its type is inferred on use. For example:

```
i1 : tau := in console
o1 : tau := out console
sensor : bv[8] := in file("samples.in")
```

## **Memory related commands**

All the results are stored in the REPL memory. You can also store well-formed Tau
formulas or Boolean functions for later reference. To do so, you can use the following
syntax:

* `tau|term`: store a tau formula or a Boolean function in the REPL memory.

If you want to consult the REPL memory contents, you can use the following commands:

* `history|hist`: show all the previously stored Tau expressions.

* `history|hist <repl_memory>`: show the Tau expression at the specified REPL memory position.

In general, to retrieve a Tau expression from the REPL memory, you can use the following
syntax for `<repl_memory>`:

* `%`: to retrieve the Tau expression stored at the latest position
* `%<number>`: to retrieve the Tau expression stored at position `<number>`
* `%-<number>`: to retrieve the Tau expression stored at the latest position minus `<number>`

## **Expression manipulation**

You can substitute expressions into other expressions or instantiate variables
in expressions. The syntax of the commands is the following:

* `substitute|subst|s <repl_memory|tau|term> [<repl_memory|tau|term>/<repl_memory|tau|term>, ...]`: substitutes a
memory, well-formed formula or Boolean function by another one in the given
expression (this one being a memory position, well-formed formula or Boolean
function). Several comma separated `match/replace` pairs may be given in one
command; all pairs are applied simultaneously in a single pass over the input,
so every match is found against the original expression and no pair's
replacement is ever re-matched by another pair (`s x & y [x / y, y / x]` swaps
`x` and `y`). Repeating the same match pattern in two pairs is an error.
The result must remain well-typed: a replacement whose type conflicts with the
matched context (e.g. `s x:sbf & y:sbf = 0 [x:sbf / z:bv[16]]`, or mismatched
bitvector widths) is rejected at substitution time instead of storing an
ill-typed expression. Untyped expressions carry the default type (`tau`) and
an unannotated replacement adopts the matched context's type
(`s x:sbf & y:sbf = 0 [x:sbf / z]` yields `zy = 0` with `z` typed `sbf`).
A pair whose match pattern does not occur in the input is reported with a
warning instead of silently leaving the input unchanged. An input that cannot
be type-inferred at all (e.g. bitvector arithmetic without width annotations)
is matched as parsed, so `s a + b = c [a / d]` substitutes `a` even though
the expression carries no type information.
Several bracket groups may follow the input: each group is applied to the
result of the previous one, while the pairs inside a group stay simultaneous.
So `s a | c [a / b] [b / d]` chains — the `b` introduced by the first group
is rewritten to `d` by the second — whereas `s a | c [a / b, b / d]` yields
`b | c`. The same match pattern may appear in different groups (that is what
chaining is for); repeating it inside one group is still an error.

* `instantiate|inst|i <repl_memory|tau> [<var>/<repl_memory|term>]`: instantiates a variable
by a memory position, well-formed formula or Boolean function in the given
well-formed or Boolean function expression.

* `instantiate|inst|i <repl_memory|term> [<var>/<repl_memory|term>]`: instantiates a variable
by a memory position or Boolean function in the given expression.

`instantiate` accepts the same multiple forms as `substitute`: several comma
separated `var/value` pairs in one bracket are applied simultaneously
(`i x & y [x / y, y / x]` swaps `x` and `y`), and several bracket groups
compose sequentially. The match side of every pair must be a variable, and
the same type safety and no-match reporting apply.

## **Logical procedures**

The Tau REPL also provides a set of logical procedures that allow you to check
several aspects of the given specification/well-formed formulas/Boolean functions.
The syntax of the commands is the following:

* `valid <repl_memory|tau>`: checks if the given specification is valid, that
is, whether no trace violates it, with every input stream read as an output
(see [Satisfiability](#satisfiability)).

* `sat <repl_memory|tau>`: checks if the given specification is satisfiable.

* `unsat <repl_memory|tau>`: checks if the given specification is unsatisfiable.

`sat`, `unsat` and `valid` print `T` or `F`, or an UNKNOWN error when the
formula cannot be decided.

* `solve [<options>] <repl_memory|tau>`: solves the given system of equations given
by the well-formed formula, computing a single satisfying assignment for its free
variables. The available options are:
	* `--min|--minimum`: computes a minimum solution of the system,
	* `--max|--maximum`: computes a maximum solution of the system,
	* `--<type>`: uses the given type (`sbf`, `tau`, ...) for the solution.

  Every value in the assignment is a constant: `solve x:bv[2] = y:bv[2]`
  answers `x := { 3 }:bv[2]` and `y := { 3 }:bv[2]`, where `lgrs` gives the
  reproductive solution `x := x|y`, `y := x|y`. An
  ordering system over `qlt` is solved as a whole, so related variables get
  distinct values (`solve x:qlt < y:qlt` gives `x` a smaller value than `y`),
  and its model is checked against every atom before it is printed.

* `lgrs [--<type>] <repl_memory|tau>`: computes a least general reproductive
solution (LGRS) for the given equation.

* `normalize|n <repl_memory|rr|ref|tau|term>`: normalizes the given expression. See
the TABA book for details.

* `qelim <repl_memory|tau>`: eliminates the non-temporal quantifiers in the given
expression.

* `realizable <repl_memory|tau|rr>`: checks whether a winning system strategy
exists for every input sequence and prints `T` or `F`.

* `unrealizable <repl_memory|tau|rr>`: the complement of `realizable`.

## **Normal forms**

Also, the Tau REPL includes several transformation procedures to standard forms.
The syntax of the commands is as follows:

* `dnf <repl_memory|tau|term>`: computes the disjunctive normal form of the given
expression.

* `cnf <repl_memory|tau|term>`: computes the conjunctive normal form of the given
expression.

* `nnf <repl_memory|tau|term>`: computes the negation normal form of the given expression.

* `mnf <repl_memory|tau|term>`: computes the minterm normal form of the given expression.

* `onf <var> <repl_memory|tau>`: computes the order normal form of the given
expression with respect to the given variable.

* `without <repl_memory|tau> <repl_memory|tau>`: computes the normal form of
the given always-conjunction without the conjunct equal to the given single
clause; the normalized formula itself when it is no always-conjunction or no
conjunct equals the clause.

## **Specification execution**

Finally, you can run a given Tau specification. The syntax for the commands is:

* `run|r <repl_memory|tau>`: runs the given Tau specification.

* `run N steps <repl_memory|tau>`: runs the specification for exactly `N` steps
and keeps the session; `run N steps` continues the stored session for `N` more
steps and a bare `run` continues it until it ends or needs input. A step that
fails, for instance on an output file that cannot be written, ends the run and
prints the error.

* `stop`: discards the stored run session.

* `memory`: prints the stored run session's current variable-to-value map.

* `ltl <repl_memory|tau>`: prints the full LTL(ABA) translation pipeline for a
  formula containing LTL operators (`F`, `G`, `U`, `R`, `W`).  Shows the data
  atom extraction, propositional skeleton, ltlsynt result, HOA strategy
  automaton, ABA oracle feasibility checks, and the synthesized safety formula.
  Useful for understanding how a full-LTL formula is handled step by step.

# **Web IDE**

A full-featured browser-based IDE for composing, navigating, debugging, and
running Tau programs.  It is a Python local webserver that wraps the `tau`
binary and exposes every REPL command through a modern editor UI.

![IDE Editor](https://app.devin.ai/attachments/8db0bb57-41bb-467d-8081-a30bb8c67229/ide-editor.png)

![IDE Visualization](https://app.devin.ai/attachments/8f3de213-52a9-4e3b-9674-bdfb2dd5aac7/ide-visualization.png)

## Running the IDE

### Quick start (one command)

The launcher script handles **everything** — builds the tau binary (if needed),
creates a Python virtual environment, installs dependencies, and starts the IDE:

```bash
# Linux / macOS
./run-ide.sh
```

```powershell
# Windows (PowerShell)
.\run-ide.ps1
```

Then open **http://localhost:8080** in your browser.

**Options:**

| Flag | Effect |
|------|--------|
| `--port 9090` | Use a different port |
| `--jobs 4` | Parallel build jobs (default: half your cores) |
| `--skip-build` | Skip C++ build (IDE still works for editing, help, grammar, visualization, AI) |
| `--help` | Show all options |

> The IDE works for editing, syntax highlighting, help, grammar browsing,
> visualization, and AI assistance **without** the tau binary.  Execution
> features (Run, Eval, REPL) activate once the binary is built.

### What the launcher does

1. **Locates Python 3.10+** — tries `python3`, then `python`
2. **Builds the tau binary** (unless `--skip-build` or already built):
   - Initializes git submodules
   - Installs build dependencies on Ubuntu (`scripts/setup-ubuntu-ci-deps.sh`)
   - Vendors the `ankerl/unordered_dense` header (avoids network issues)
   - Runs `release.sh` → produces `build-Release/tau`
3. **Creates a Python venv** at `.venv/` (skipped if it already exists)
4. **Installs Python deps** from `ide/requirements.txt`
5. **Starts the IDE server** on the specified port

### Build variants

If you prefer to build manually:

| Script | Binary location | Use case |
|--------|----------------|----------|
| `./release.sh` | `build-Release/tau` | Normal use (fastest) |
| `./debug.sh` | `build-Debug/tau` | Debugging with symbols |
| `./relwithdebinfo.sh` | `build-RelWithDebInfo/tau` | Profiling |

The IDE server automatically searches all of these directories for the binary.

### Manual setup (if you prefer not to use the launcher)

```bash
cd tau-ltl
python3 -m venv .venv
source .venv/bin/activate      # ⬅ MUST activate before pip install
pip install -r ide/requirements.txt
python -m uvicorn ide.server:app --host 0.0.0.0 --port 8080
```

### Troubleshooting

| Problem | Fix |
|---------|-----|
| `unordered_dense` clone fails during build | The launcher vendors this automatically; if building manually, run: `mkdir -p external/parser/external/ankerl && curl -sL https://raw.githubusercontent.com/martinus/unordered_dense/v4.4.0/include/ankerl/unordered_dense.h -o external/parser/external/ankerl/unordered_dense.h` |
| `externally-managed-environment` error | You forgot to activate the venv: run `source .venv/bin/activate` first |
| `pip install` fails with permission error | Use the launcher script or create a venv manually (see above) |
| `python3: command not found` | Try `python` instead of `python3`; ensure Python 3.10+ is installed |
| `No module named 'uvicorn'` | Make sure you activated the venv (`source .venv/bin/activate`) before running |
| Port 8080 already in use | `./run-ide.sh --port 9090` |

### Custom binary location

```bash
python -m ide.server --port 8080 --tau-binary /path/to/tau
```

## IDE Features

| Area       | What you get |
|------------|-------------|
| **Compose** | Syntax highlighting (grammar-driven), autocomplete from grammar tokens, 8 starter templates, file tabs |
| **Navigate** | Sidebar explorer with test/example files, symbol search, command palette (`Ctrl+Shift+P`) |
| **Debug**   | AST viewer panel, `whatis` type inspection, variable hover tooltips |
| **Run**     | Execute specs (`Shift+Enter`), evaluate formulas (`Ctrl+Enter`), quick actions (SAT / valid / normalize / DNF / CNF / …) |
| **Help**    | 10 searchable help topics, grammar reference derived from `parser/tau.tgf`, keyboard shortcut reference |
| **Visualize** | Timeline diagrams for temporal operators, AST tree view, color-coded operator legend |
| **AI** | DeepSeek-powered NL ↔ Tau translation, context-aware code assistance, code explanations |

### Keyboard shortcuts

| Shortcut          | Action                 |
|-------------------|------------------------|
| `Ctrl+Enter`      | Evaluate selection/line |
| `Shift+Enter`     | Run specification      |
| `Ctrl+Space`      | Autocomplete           |
| `Ctrl+/`          | Toggle comment         |
| `Ctrl+Shift+P`    | Command palette        |
| `Ctrl+Shift+V`    | Visualize formula      |
| `Ctrl+N`          | New file               |
| `Ctrl+O`          | Open file              |
| `Ctrl+S`          | Save file              |
| `Ctrl+G`          | Go to line             |
| `F1`              | Toggle help panel      |
| `Escape`          | Close palette/panels   |

### Fragment switching

Use the dropdown in the top bar or the REPL command to switch between grammar
fragments:

```
fragment ltl         # default — standard LTL operators
fragment ctl_star    # enable CTL* path quantifiers A, E and semantic negation -
```

## Grammar Integration

The IDE parses `parser/tau.tgf` at server startup and derives:

- **Token categories** for syntax highlighting (keywords, temporal ops, commands,
  types, operators — all from the grammar)
- **Autocomplete entries** with documentation for every command, keyword, and
  operator
- **Grammar reference panel** in the help sidebar showing every `wff`, `bf`, and
  CLI production rule with syntax, label, and description
- **Hover tooltips** — hover a highlighted token in the editor to see its grammar
  rule and description

If the grammar file changes, restart the server to pick up the new definitions.
The highlighter and autocomplete update automatically.

## Temporal Formula Visualization

Click the **Viz** button or press `Ctrl+Shift+V` to visualize the current
formula (or selected text).  The visualization panel shows:

- **Timeline diagram** — SVG rendering showing how temporal operators behave over
  time steps.  `G` spans all steps, `F` highlights eventual satisfaction, `U`
  shows the handoff point, etc.
- **AST tree** — interactive parse tree of the formula with color-coded nodes for
  each operator type
- **Operator legend** — all temporal operators with their semantics and colors

This is a presentation/teaching aid — useful for understanding what a formula
means over time.

## AI Assistant (DeepSeek)

The IDE includes a DeepSeek-powered AI assistant for working with Tau code.
Click the **AI** button in the top bar or use the AI tab in the bottom panel.

**Setup:** Click the gear icon to enter your DeepSeek API key.  The key is
stored in browser `localStorage` only — it is never written to disk or sent
anywhere except directly to the DeepSeek API.

**Three modes:**

1. **Ask (context-aware)** — "How do I add a liveness property to this?" The
   assistant sees your entire editor content and provides code snippets that
   integrate with your existing program.

2. **Generate from NL** — Describe what you want in plain English and the
   assistant generates complete Tau code.  The generation prompt includes the
   full grammar, language semantics, 15 worked examples, and common-mistake
   avoidance rules.

3. **Explain code** — Select code (or use the full editor content) and get a
   structured natural-language explanation: one-sentence summary, operator-by-
   operator breakdown, I/O behavior, required fragment.

**Models:** Choose between DeepSeek R1 (reasoning, slower, more accurate) and
DeepSeek V3 (fast chat).

Generated code can be inserted at cursor or replace the editor content with one
click.


# **Developer infrastructure for ω-categorical synthesis**

The following headers (all standalone, header-only, test-covered) implement
tau-lang's LTL(ABA) synthesis pipeline over ω-categorical theories.

## Core synthesis algorithms

| Header | Status | Purpose |
|--------|--------|---------|
| `src/algorithm_b_skeleton.h` | **Default path** | Algorithm B: `build_algorithm_b_skeleton(T1_size, T2_size, K, feasible_set_b, t2_pos_m, phi_star)`. Adds ⌈log₂\|T₂\|⌉ input P-bits encoding T₂ = (pos_m, pos_x, rel_mx). Assembles `(Φ_I ∧ Ψ_I) → (Φ_O^R ∧ Φ_δ ∧ φ*)`. Sound for input-variable formulas. Activated by default or `TAU_LTL_ALG=B`. |
| `src/algorithm_a_skeleton.h` | Available for pure-output formulas | Algorithm A: `build_algorithm_a_skeleton(T1_size, K, feasible_set, phi_star)`. Uses ⌈log₂\|T₁\|⌉ output R-bits for the memory type, no input propositions. Faster for pure-output formulas; bypassed when input variables appear. |
| `src/algorithm_d_game.h` | Available for output-only formulas (`TAU_LTL_ALG=D`) | Algorithm D: direct parity-game construction. HOA guard evaluator (`eval_guard`); synthesis game parser (`parse_synth_game_hoa`); `build_product_game` (synthesis game × T₁, with T₃ feasibility pruning); `zielonka_win_player1` (recursive attractor + subgame solver, odd priority = system wins). |
| Algorithm C (deleted) | Not implemented | Oracle-assisted abstract game with `A_{ρ,J}` oracle propositions. Deleted 2026-08-25 (D2): Algorithm D provides the `T1`-product game without the formula blow-up, and C's oracle propositions had no executable strategy; the design write-up is kept outside this repository. |

## Type enumeration (ω-categorical theories)

| Header | Purpose |
|--------|---------|
| `src/omcat_types.h` | `rational` type (128-bit cross-multiplied comparison). `qlt_type1`/`qlt_type2`/`qlt_type3` structs for 1-/2-/3-types of (ℚ,<,Σ). `enumerate_qlt_T1` (2k+1 types from k constants), `enumerate_qlt_T2` (T₂ = (pos_m, pos_x, rel_mx) with forced-relation filtering), `enumerate_qlt_T3` (T₃ with transitivity filter). `realize()` rational witnesses. `Pre_over_T1`, `nu_fixpoint`/`mu_fixpoint` over 2^{T_1} and `reachable_from` are staged helpers that Algorithm D does not use yet. |
| `src/boolean_algebras/qlt/omcat_constants.h` | `parse_rat_literal` for rational/decimal strings (at most 18 fractional digits); `collect_qlt_constants(fm)` harvesting named constants from a formula. |
| `src/omcat_oracle_cache.h` | Thread-safe runtime cache for atomic oracle answers (tp(m,x) and achievability-set A_{ρ,J}). |

## Supporting infrastructure

| Header | Purpose |
|--------|---------|
| `src/gr1_detect.h` | `is_gr1_fragment(fm, &n_safety, &n_liveness)` classifier for `⋀ G(ψ_safe) ∧ ⋀ GF(ψ_live)`. Staged: not wired into the synthesis dispatch, exercised by unit tests only. |
| `src/liveness_decomp.h` | `decompose_liveness(fm)` splits GR(1)-shaped formulas into safety part + GF bodies. Staged, unit tests only. |
| `src/mealy_extract.h` | Mealy-machine extraction helpers. Staged, unit tests only. |
| `src/decomposed_spec.h` | `decomposed_spec { transient; invariant; reactive }` with `decompose_spec(fm)` classifier. |
| `src/parse_error_hint.h` | `classify_parse_error(formula)` for actionable parse error messages. |
| `src/tau_lang_api.h` | Documentation header for the library entry points (`is_tau_formula_sat`, `get_nso_rr`, `run`); there is no `tau_lang_is_realizable` symbol. |

## Algorithm A/B soundness rule

Algorithm A has no input propositions, so it is sound only when the formula has
no input variables. If a `qlt` formula contains input variables, the dispatcher
uses Algorithm B even if `TAU_LTL_ALG=A` was requested. Algorithm B adds P-bits
for the current T₂ type, making the synthesized strategy type-aware.

# **The C++ API and language bindings**

Besides the executable and the REPL, the framework is usable as a library.

The public C++ API is [`src/api.h`](src/api.h). All operations are exposed as
static methods on `api<node>`, and cover parsing (`get_spec`, `get_formula`,
`get_term`, `get_definition`, ...), printing, substitution and instantiation,
the logical procedures, the normal forms and the execution of specifications
(`get_interpreter`, `get_inputs_for_step`, `step`). `unsat_core` returns a
subset-minimal set of a specification's top-level conjuncts that is already
unrealizable (or unsatisfiable), and `reset` returns the process to a fresh
state, freeing every tree node no `htref` holds. Global switches such as
`set_charvar`, `set_preprocessing`, `set_ba_component_factoring`,
`set_indenting`, `set_highlighting`, `set_json` and
`set_severity` mirror the command line options, and every runtime limit has a
setter of the same name as its option (`set_block_max_splits`,
`set_max_fixpoint_steps`, `set_ba_decision_pins`, ...). An option a Boolean
algebra declares about itself is set by the name it has on the command line,
without the dashes in front: `set_ba_option("bv-widening", 1)` or
`set_ba_option("bv-defelim-max-atoms", 5)` (a flag takes 0 or 1) returns the
value now in force, `get_ba_option(name)` reads it back, both answer an error
when no algebra of the build declares the name, and `ba_option_names()` lists
the names the build has.

The underlying tree representation is documented in
[`docs/tau_tree.md`](docs/tau_tree.md), and
[`docs/adding_base_bas.md`](docs/adding_base_bas.md) describes how to add a new
base Boolean algebra. For development conventions, standalone runners and testing
notes see [`DEVELOPMENT.md`](DEVELOPMENT.md).

Python bindings are provided via
[nanobind](https://github.com/wjakob/nanobind) in
[`bindings/python`](bindings/python) and built with `./dev binding python`. They
expose the interpreter part of the API together with a set of stream
implementations (console, file, and in-memory vector streams) so that inputs and
outputs can be driven from Python. The decision procedures `sat`, `unsat`,
`valid`, `realizable` (alias `is_realizable`) and `unrealizable` take a full
specification and return a `tau.result` whose value is the verdict, or `None`
when the spec does not parse or gets no verdict. `unsat_core(spec,
realizability=True)` returns a subset-minimal list of the top-level conjuncts
(`always (A && B)` counts as `always A` and `always B`) that is already
unrealizable, or unsatisfiable with `realizability=False`. `reset()` returns
the engine to a fresh state, as the REPL `reset` command does. See
[`docs/tau_result.md`](docs/tau_result.md) for the result type.

```python
import tau   # the built module lives in <build dir>/bindings/python/nanobind

i_stream = tau.vector_input_stream(["T", "F", "T"])
o_stream = tau.vector_output_stream()

opts = tau.interpreter_options()
opts.input_remaps["i"] = i_stream
opts.output_remaps["o"] = o_stream

interpreter = tau.get_interpreter("o[t] = i[t].", opts)
for _ in range(3):
    tau.step(interpreter)

print(o_stream.get_values())   # ['T', 'F', 'T']
```

The module also carries the api's runtime budgets and engine switches under
the same names (`tau.set_max_fixpoint_steps(1000)`, `tau.set_tref_budget(n)`,
`tau.set_ltl_timeout_sec(120)`, `tau.set_preprocessing(False)`, ...) and the
options the algebras declare (`tau.ba_option_names()`,
`tau.set_ba_option("bv-widening", 1)`, `tau.get_ba_option(name)`, both
returning a `tau.result`).

Further examples are in [`tests/bindings/python`](tests/bindings/python).

# **The Theory behind the Tau Language**

* GS Paper [Guarded Successor: A Novel Temporal Logic by Ohad Asor](https://web3.arxiv.org/abs/2407.06214)
* TABA book [Theories and Applications of Boolean Algebras by Ohad Asor](./docs/Theories-and-Applications-of-Boolean-Algebras-0.25.pdf) (In works).
* YouTube lecture series on [Atomless Boolean Algebra by Ohad Asor](https://www.youtube.com/watch?v=lNU1wS5rqtg&list=PLav2klOnTUlOeakJCbLZxoib_x0jYAQ5f&index=2).


# **Known issues**

This is a short list of known issues that will be fixed in a subsequent release:

* Simplification:
  * Simplification of Boolean equations may take longer time in a few cases.
  * Path simplification algorithm does not take equalities between variables
  into account leading to later blow ups.
* Several procedures are bounded rather than complete, and report an error
  instead of an answer when the bound is reached. This is the case for the
  fixpoint enumeration of recurrence relations and for the satisfiability
  fixpoint search. Such a bound is a limit on the search, not a proof about the
  input.
* Fixpoint calculation is not supported for recurrence relations with more than
  one offset index.
* The `anf` (algebraic normal form) and `pnf` (prenex normal form) commands are
  not implemented and are currently not reachable from the REPL grammar.
* Minor errors in Windows REPL
* LTL-specific:
  * Always include time indices `[t]` on io_vars: `G(o1[t]:sbf = i1[t]:sbf)`.
    Omitting them (e.g. `G(o1:sbf = i1:sbf)`) is a syntax error.
  * Input variables in the left operand of `U`/`W` cannot be synthesised.
  * `G(phi_A) && G(phi_B)` with the **same** BA type is automatically merged to
    `G(phi_A && phi_B)`; with **different** BA types each `G` part is normalized
    independently — both forms work.
  * Mealy strategies with any number of states are executable.
  * `S` (since) and `T` (trigger) past LTL operators are decided through the
    ppLTLTT temporal-tester encoding on the synthesis path and compiled away
    to auxiliary output variables for pure-past execution; the tester
    integration is complete, the compile-away pass's `aux_pairs` output is
    unused.
  * `nlang` type requires `TAU_LLM_API_KEY` (or `OPENAI_API_KEY`) to be set;
    without it every oracle question is answered `false` (not cached), so
    verdicts are not reliable.
  * **Algorithm A** is intentionally restricted to pure-output formulas. If
    input variables are present, the dispatcher uses Algorithm B.


# **Future work**

* Enabling efficient data storage and manipulation in Tau specifications using Boolean functions
* Overcoming performance issues during normalization of formulas and satisfiability checking of Tau specifications
* Add support for redefinitions of functions or predicates.
* Support the Boolean algebra of Boolean (not just simple) functions in general.
* Improve the performance of Boolean function normalization.
* **Past LTL S/T operators**: currently compiled away to auxiliary variables; full
  integration with ltlsynt for pure past-LTL fragments is planned.
* **nlang_ba**: support for additional LLM backends (OpenAI, local models) beyond
  the current DeepSeek oracle.
* **qlt/qint synthesis**: further polish of the QE oracle for reactive synthesis
  over `qlt` (DLO) and `qint` (interval BA) types — decimal and rational constants
  are supported, the dedicated DLO QE path handles the `qlt` ω-categorical theory,
  and the data oracle is cross-validated against cvc5 LRA in `test_qlt_oracle`.
* **Algorithm D Phase 2/3**: Algorithm D solves the product game as a parity
  game (Zielonka's recursive algorithm over priorities derived from the Büchi,
  co-Büchi or parity acceptance `ltlsynt` prints) for output-only `qlt`
  formulas.  The separate μ/ν fixpoint formulation over 2^{T₁} in
  `src/omcat_types.h` (`Pre_over_T1`, `nu_fixpoint`, `mu_fixpoint`) is staged
  and not used by the solver; extending D to input-bearing formulas (the T₂
  dimension in the environment states) is the open design item.
* **BA type encoding for Algorithm B**: currently only `qlt` (DLO) types use the
  T₁/T₂ type-enumeration path.  Extension to other BA types (sbf, bv, tau) requires
  BDD-based type encoding: the type of a BA element relative to the formula's
  named constants is determined by which atom of the subalgebra generated by those
  constants the element falls in — equivalently, the element's BDD restricted to
  the constant variables.
* **Term algebras**: support for term algebras (theory of trees) as a concrete domain
  — decidable ω-categorical theories admitting quantifier elimination.
* **Explicit template instantiations**: reduce compile times by moving hot function
  templates into separate `.cpp` files with explicit instantiations.

# **Submitting issues**

Please submit issues at the following link: [Tau Language issues](https://github.com/IDNI/tau-lang/issues).

# **License**

Tau Language is licensed under the following terms:
[Tau Language License](https://github.com/IDNI/tau-lang/blob/main/LICENSE.md)

# **Authors**

The Tau Language has been developed by the following authors:

* Ohad Asor
* David Castro Esteban
* Tomáš Klapka
* Lucca Tiemens
