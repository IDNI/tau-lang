![The TAU logo](/docs/images/Tau_Banner.png)

| Build type | Status                                                                                     |
|------------|--------------------------------------------------------------------------------------------|
| Release    | ![Release Build](https://github.com/IDNI/tau-ltl/actions/workflows/test-release.yml/badge.svg) |
| Debug      | ![Debug Build](https://github.com/IDNI/tau-ltl/actions/workflows/test-debug.yml/badge.svg)     |

# Table of contents

1. [Introduction](#introduction)
2. [Installing the Tau Language Framework](#installing-the-tau-language-framework)
	1. [Linux](#linux)
	2. [Windows](#windows)
	3. [MacOS (not available yet)](#macos-not-available-yet)
	2. [Compiling the source code](#compiling-the-source-code)
3. [Quick start](#quick-start)
   1. [Run a spec with the interpreter (`tau`)](#run-a-spec-with-the-interpreter-tau)
   2. [Compile a spec to C++ (`tau_codegen`)](#compile-a-spec-to-c-tau_codegen)
4. [The Tau Language](#the-tau-language)
    1. [Tau specifications](#tau-specifications)
    2. [Full LTL operators](#full-ltl-operators)
    3. [CTL\* fragment and semantic negation](#ctl-fragment-and-semantic-negation)
    4. [Heterogeneous OMCAT function symbols](#heterogeneous-omcat-function-symbols)
    5. [Satisfiability and execution](#satisfiability-and-execution)
    6. [Boolean functions](#boolean-functions)
    7. [Bitvectors](#bitvectors)
    8. [Functions and predicates](#functions-and-predicates)
    9. [Constants](#constants)
    10. [Streams](#streams)
    11. [Variables and uninterpreted constants](#variables-and-uninterpreted-constants)
    12. [Type system](#type-system)
    13. [Pointwise revision](#pointwise-revision)
    14. [Reserved symbols](#reserved-symbols)
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
   1. [Running the IDE](#running-the-ide)
   2. [IDE Features](#ide-features)
   3. [Grammar Integration](#grammar-integration)
   4. [Temporal Formula Visualization](#temporal-formula-visualization)
   5. [AI Assistant (DeepSeek)](#ai-assistant-deepseek)
8. [Documentation (mdBook)](#documentation-mdbook)
9. [The Theory behind the Tau Language](#the-theory-behind-the-tau-language)
10. [Known issues](#known-issues)
11. [Future work](#future-work)
12. [Submitting issues](#submitting-issues)
13. [License](#license)
14. [Authors](#authors)


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

* deb (Debian/Ubuntu): [tau-0.7-Linux.deb](https://github.com/IDNI/tau-ltl/releases/download/v0.7-alpha/tau-0.7-Linux.deb)
* rpm (Fedora): [tau-0.7-Linux.rpm](https://github.com/IDNI/tau-ltl/releases/download/v0.7-alpha/tau-0.7-Linux.rpm)

The executable is installed in `/usr/bin/tau`.

## **Windows**

For windows, we provide a convenient installer that includes the tau executable
and also a zip file:

* Installer: [tau-0.7-win64.exe](https://github.com/IDNI/tau-ltl/releases/download/v0.7-alpha/tau-0.7-win64.exe)
* Zip file: [tau-0.7-win64.zip](https://github.com/IDNI/tau-ltl/releases/download/v0.7-alpha/tau-0.7-win64.zip)

## **MacOS (not available yet)**

A macOS installer will be available in the future.

## **Compiling the source code**

To compile the source code you need a recent C++ compiler supporting C++23, e.g.
GCC 13.1.0. You also need at least cmake version 3.22.1 installed in your system.
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
git clone https://github.com/IDNI/tau-ltl.git
```

you can run either the `release.sh` or `debug.sh` or `relwithdebinfo.sh` scripts
to build the binaries.

To build with doxygen documentation:

```bash
# Compiles the source code in release mode and also the documentation
./release.sh -DBUILD_DOC=ON
# Compiles the source code in debug mode and also the documentation
./debug.sh -DBUILD_DOC=ON
# Compiles the source code in release mode with debug information and also the documentation
./relwithdebinfo.sh -DBUILD_DOC=ON
```

Once you have compiled the source code you can run the `tau` executable to
execute Tau specifications. The `tau` executable is located in either `build-Release`
or `build-Debug` or `build-RelWithDebInfo`.

The same build also produces the `tau_codegen` executable, which compiles a
realizable tau specification to a self-contained C++17 program header
(see [Synthesize a C++ program from a spec](#synthesize-a-c-program-from-a-spec)).


# **Quick start**

Tau-lang offers two ways to execute a specification:

1. **Interpret** the spec directly — solve each time step on the fly.
   Use `tau`.  Best for iteration and REPL use.
2. **Compile** the spec to a C++17 state machine ahead of time.
   Use `tau_codegen`.  Best for high-throughput deployment.

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

## Compile a spec to C++ (`tau_codegen`)

`tau_codegen` compiles a realizable tau specification to a standalone
C++17 header.  The generated code is a pure `switch/case` state machine
with no runtime dependency on libTAU for propositional specs, and compiles
with `g++ -O3 -flto -std=c++17`.

```sh
# 1. Write a spec (mirror-input example).
echo -n 'G(o1[t]:bv = i1[t]:bv)' > spec.tau

# 2. Synthesize + emit a C++ program header in one command.
tau_codegen spec.tau -o program.h

# 3. Compile your application against it.
g++ -O3 -flto -std=c++17 main.cpp -o sim
./sim
```

Full worked example: `examples/reactive_program/`.

**Performance**: generated programs run at billions of steps per second
(see `test_cpp_codegen_bench`).

**Correctness**: synthesis produces a program whose behavior is provably
consistent with the spec on every input trace.  Unrealizable specs are
rejected up front by `tau_codegen`.

`tau_codegen` is a one-shot compiler: it parses the input Tau spec, runs the
same LTL(ABA) synthesis pipeline as the interpreter, and emits a C++ strategy
header.  Pointwise revision (PWR) happens before code generation: a spec
already produced by PWR can be passed to `tau_codegen`, synthesized, emitted,
compiled, and stepped like any other realizable spec.  Runtime PWR/spec
patching remains the interpreter's responsibility, not a feature of generated
C++ headers.

See the [command-line reference](#command-line-interface) for full details
on both `tau` and `tau_codegen` options.

---

To start using the Tau Language, download the latest release from the
[GitHub page](https://github.com/IDNI/tau-ltl/releases/tag/v0.7-alpha). Once
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

In the [demos](https://github.com/IDNI/tau-ltl/tree/main/demos) folder you
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
are used for presenting output to a user. Each stream in the specification
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

Formally, the grammar for Tau specifications is
```
spec => local_spec
      | "G" local_spec          -- globally (always)
      | "F" local_spec          -- eventually (sometimes)
      | spec "U" spec           -- until
      | spec "R" spec           -- release
      | spec "W" spec           -- weak until
      | spec "S" spec           -- since (past LTL)
      | spec "T" spec           -- trigger (past LTL)
      | (spec && spec) | (spec || spec) | !spec
```
where `local_spec` is a formula defined by the rules:

```
local_spec => (local_spec "&&" local_spec)
            | ("!" local_spec)
            | (local_spec "^^" local_spec)
            | (local_spec "||" local_spec)
            | (local_spec "->" local_spec)
            | (local_spec "<->" local_spec)
            | (local_spec "?" local_spec ":" local_spec)
            | (term "=" term) | (term "!=" term)
            | (term "<" term) | (term "!<" term) | (term "<=" term) | (term "!<=" term)
            | (term ">" term) | (term "!>" term) | (term ">=" term)| (term "!>=" term)
            | "all" variable local_spec
            | "ex" variable local_spec
            | predicate
            | T | F
```
The naming conventions for `variable` are discussed in [Variables](#variables).
Furthermore, `term` is discussed in the section [Boolean functions](#boolean-functions)
and `bv_term`, bitvector term, is discussed in the section [Bitvectors](#bitvectors).
The `predicate` non-terminal in the above grammar describes how
to add predicate definitions directly into a formula. See the subsection
[Functions and predicates](#functions-and-predicates) for the
grammar definition of `predicate`.
In the REPL ([The Tau REPL](#the-tau-repl)) they
can be provided as explained in subsection
[Functions, predicates and input/output stream variables](#functions-predicates-and-inputoutput-stream-variables).

The symbols used have the following meaning, where a formula refers to either `local_spec` or `spec`:

| Symbol            | Meaning                                                 |
|-------------------|---------------------------------------------------------|
| `!`               | negation of formula                                     |
| `&&`              | conjunction of formulas                                 |
| `^^`              | xor of formulas                                         |
| `\|\|`            | disjunction of formulas                                 |
| `<->`             | equivalence of formulas                                 |
| `<-`              | left-implication of formulas                            |
| `->`              | right-implication of formulas                           |
| `ex`              | existential quantification of `variable`                |
| `all`             | universal quantification of `variable`                  |
| `... ? ... : ...` | if ... then ... else ...                                |
| `=`               | standard equality relation in BA or bitvectors          |
| `!=`              | standard inequality relation in BA or bitvectors        |
| `<`               | standard less relation in BA or bitvectors              |
| `!<`              | standard not-less relation in BA or bitvectors          |
| `<=`              | standard less-equal relation in BA or bitvectors        |
| `!<=`             | standard not-less-equal relation in BA or bitvectors    |
| `>`               | standard greater relation in BA or bitvectors           |
| `!>`              | standard not-greater relation in BA or bitvectors       |
| `>=`              | standard greater-equal relation in BA or bitvectors     |
| `!>=`             | standard not-greater-equal relation in BA or bitvectors |

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

Standard equivalences hold: `F φ ≡ T U φ`, `G φ ≡ F ¬ φ → ⊥ ≡ φ R ⊥`,
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
| `TAU_LTL_TIMEOUT_SEC` | 60 | Wall-clock limit for each `ltlsynt` call (0 = disable). |
| `TAU_LTL_MAX_DEPTH` | 8 | Max temporal-operator nesting depth before early error (0 = disable). |
| `TAU_LTL_MAX_LOOKBACK` | 5 | Max `[t-k]` lookback before early error (0 = disable). |
| `TAU_LTL_EXPORT_STRATEGY` | _unset_ | `hoa` prints winning-strategy HOA to stderr; `dot` prints Graphviz dot (falls back to HOA if `autfilt` is unavailable). |
| `TAU_LTL_EXPORT_STRATEGY_FILE` | _unset_ | If set to a path, also writes the HOA strategy to that file on success. |
| `TAU_LTL_SIMPLIFICATION` | _ltlsynt default_ | Forwarded to `ltlsynt --simplification=` (`bwoa`\|`sat`\|`bisim-sat`\|`none`). |
| `TAU_LTL_WITNESS` | _unset_ | When set to `1`, prints an environment counter-strategy (HOA) to stderr on UNREALIZABLE — only available when the UNREAL verdict comes from `ltlsynt` (not from earlier tau-internal rejection). |
| `TAU_LTL_CACHE` | _unset_ | When set to `1`, enables a thread-safe whole-formula result cache in `is_tau_formula_sat`. Keyed on `(tref, start_time, output)`. Pure-function, no correctness impact. Useful for fuzz loops / REPL workflows with many identical formulas. |
| `TAU_LTL_ALG` | _unset_ (Algorithm B for input-bearing qlt, Algorithm A for pure-output qlt) | Override synthesis algorithm: `A` = request Algorithm A for pure-output formulas (input-bearing formulas still route to B), `B` = Algorithm B (P_σ binary encoding), `D` = request output-only Algorithm D (input-bearing formulas fall through to B). |

**Execution**: when the interpreter pipeline is given a realizable LTL formula,
`ltl_to_safety_formula` converts the winning Mealy strategy to an executable
`G(φ)` formula.  Single-state strategies (common for F, G(F), R, W) use the
self-loop guard directly.  Multi-state strategies are encoded using one-hot
auxiliary output bitvector variables (`o__ltl_ms0__`, `o__ltl_ms1__`, …) and
work for any number of states.

### Operator precedence

LTL operators bind less tightly than propositional connectives:

```
!  >  &&  >  ^^  >  ||  >  <->  >  <-  >  ->  >  ex  >  all  >  ?:  >  F  >  G  >  U  >  R  >  W  >  S  >  T
```

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
`o1[t-k]`), the interpreter needs `k` warm-up steps before the lookback
values are available.  During these initial steps all output variables are
unconstrained and default to the bottom element (`0`/`"F"` for tau,
`0`/`"F"` for sbf, etc.).  The number of warm-up steps equals the *maximum*
lookback shift across all stream variables in the formula (the global
`max_lookback`).  Every output — even those with a smaller individual shift —
receives `max_lookback` initial default values.

Example: `G(o1[t] = i1[t-1] && o2[t] = i2[t-2])` has `max_lookback = 2`.
Both `o1` and `o2` will output `"F"` for steps 0 and 1, regardless of their
individual shifts.

### Known LTL limitations

- **Spec terminator**: every formula string passed to the parser must end with `.`
  (a period), e.g. `G (o1[t] = 1).`  Omitting the terminator causes a parse error.
- **Semantic Boolean negation is not implemented**: future docs use `-Φ` for
  semantic/program-level complement over causal programs.  Current Tau syntax
  uses `!` for ordinary formula negation; `-` is already used in the grammar for
  subtraction, integer signs, and stream-index shifts, so any future semantic
  negation syntax must be added as a typed spec-level grammar extension.
- **nlang_ba requires DEEPSEEK_API_KEY**: formulas using the `nlang` type require
  the `DEEPSEEK_API_KEY` environment variable to be set; without it the oracle
  calls will fail at runtime.
- **Mixed-type G&&G**: `G(phi_A) && G(phi_B)` where `phi_A` and `phi_B` use
  different BA types raises "nesting of temporal quantifiers".  Use `F` or
  `G+F` combinations for mixed-type liveness, or keep each type in its own
  top-level temporal operator.
- **fall/fex BF quantifiers**: these parse correctly but are not supported in
  LTL synthesis (only in safety/always formulas).
- **Input-only atoms**: formulas consisting entirely of input constraints (no
  output variables) are checked for satisfiability but cannot be synthesized
  (the system has no control over pure-input atoms).
- **Nesting depth**: deeply nested formulas (depth > 6) may exceed stack or memory
  limits.  Use shallower formulas or increase the process stack size when needed.

## **CTL\* fragment and semantic negation**

The default grammar fragment is **LTL** (linear temporal logic).  To use
branching-time operators, switch to the CTL\* fragment:

```
#fragment ctl_star.
```

To switch back:

```
#fragment ltl.
```

### CTL\* operators

The CTL\* fragment extends the LTL grammar with path quantifiers:

| Operator | Syntax | Meaning |
|----------|--------|---------|
| **A** (for-all paths) | `A φ` | φ holds on every path from the current state |
| **E** (exists path) | `E φ` | φ holds on at least one path from the current state |

`A` and `E` quantify over the tree of possible strategy executions, not over
individual traces.  The equivalence `A φ ≡ ¬E¬φ` holds.

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
a single trace.

### Examples

```
-- In the CTL* fragment:
#fragment ctl_star.

-- For all paths, eventually the output is 1
A F(o1[t] = 1).

-- There exists a path where the output is always 0
E G(o1[t] = 0).

-- Semantic negation: no strategy can keep output always equal to input
- G(o1[t] = i1[t]).
```

### Reduction to LTL

CTL\* formulas are reduced to LTL synthesis problems via the
Bloem/Schewe/Khalimov witness-output encoding.  Each existential path choice
is encoded as an additional witness output variable, allowing the existing
`ltlsynt`-based pipeline to handle the branching-time property.

## **Heterogeneous OMCAT function symbols**

Heterogeneous function symbols allow defining typed functions whose arguments
and results may come from different OMCAT sorts.  Function declarations use
the `func` keyword:

```
func f(Bool) -> Bool dynamic.
func g(Bool, Bool) -> Bool static.
```

### Function modes

| Mode | Meaning |
|------|---------|
| `dynamic` | The function may vary per time step (window-level) |
| `static` | The function is fixed for the entire execution (global graph) |

### Function application

Once declared, functions can be applied in formulas:

```
func inc(Bool) -> Bool dynamic.
G(o1[t] = inc(i1[t])).
```

The OCFuncs compilation pipeline handles purification, support bounding,
profile encoding, and LTL skeleton generation automatically.


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
do not make the execution contradictory in a future step.

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
term => (term "&" term) | term "'"
      | (term "^" term) | (term "|" term)
      | function | constant | uninterpreted_constant
      | variable | stream_variable | "0" | "1"

```

where

* `term` stands for a well-formed subformula representing a Boolean function and the operators `&`, `'`,
`^` and `|` respectively stand for conjunction, negation, exclusive-or
and disjunction,
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
[Variables](#variables) for details),
* `stream_variable` represents an input or output
stream. The type of a stream also determines the type of the Boolean function (see also subsection [Variables](#variables)) and
* `0` and `1` stand for the bottom and top element in the fixed Boolean
algebra.

The order of the operations is the following (from higher precedence
to lower): `'` > `&` > `^` > `|`.

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
      | (term _ ">>" _ term)
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


The order of the operations is the following (from higher precedence
to lower): `*` > `/` > `%` > `+` > `-` > `!&` > `!|` > `!^` > `<<` > `>>`.

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
name "(" [ variable ("," variable)* ] ")" [":" type]"
```
hence, omitting the `index`. For example, using the recurrence relation `h` from above,
we can call the fix point by typing into [REPL](#the-tau-repl) `normalize h(x,y,z,w)`.
Not all recurrence relations have a fixpoint. In this case, by default, `0` is returned for functions and
`F` for predicates.

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
base_boolean_algebra_type => "tau" | "sbf" | "bv" [ '[' bit_width ']' ]
                           | "qlt" | "qint" | "nlang" | "hsb"
```

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

`stream_variable => "o(number)[" index "]" | "i(number)[" index "]"`

where `index` is defined in subsection [Functions and predicates](#functions-and-predicates).
Hence, `o1[t]` is a valid output stream, whereas `i1[t]` is a valid input stream.
In the future we will allow arbitrary names for streams, but for now `number`
is used as an identifier, while `o` denotes an output and `i` an input stream.

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

There are currently two ways to assign a type to a stream: either explicitly in
the REPL (see also subsection
[Functions, predicates and input/output stream variables](#functions-predicates-and-inputoutput-stream-variables))
or implicitly by using typed constants. In the later case the type for a stream is inferred.
In a nutshell, the inference process works as follows:
1. Typing a constant in a Boolean function, types the whole Boolean function.
Any stream appearing in this Boolean function will be assigned this type.
2. Streams that have been typed in a single Boolean function, are automatically typed
in all Boolean functions.
3. Streams that appear in a Boolean function, in which a different stream has been typed,
are typed accordingly.
4. If a stream cannot be typed by (possibly repeated application of) any of the above rules, it is assigned the default type `tau`.

Note that this stream type inference only happens if you start executing a Tau specification.
In case a type mismatch is detected, the execution does not start and is terminated.
Furthermore, types are currently not propagated using variables. The process only considers constants and streams.
Variables will be taken into account soon.

When typing a stream explicitly in [REPL](#the-tau-repl), the syntax is
```
stream_definition => stream_variable [":" type] ":=" stream_type stream
```
where `stream_variable` is the name of the stream, `type` is a supported type (`tau`, `sbf`, `bv`, `bv[8]`, `qlt`, `qint`, `nlang`, `hsb`...),
`stream_type` is either input stream (`in`) or output stream (`out`), and `stream` is either `console` (meaning that the
stream reads/outputs values from/to the console) or `file(file_name)` which denotes the file from/into which to read/write
(in quotes if needed). For example,
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

The Tau Language currently supports the following types:

1. `tau`: the type of Tau specifications,
2. `sbf`: the type of simple Boolean functions,
3. `bv`: the type of bitvectors of fixed bit width,
4. `qlt`: the ω-categorical theory of the rationals under `<` (dense linear order, no endpoints) — ω-categorical and decidable, hence supported,
5. `qint`: the Boolean algebra of right-closed, left-open rational intervals `[x, y)`; accepts both rational (`1/4`) and decimal (`0.25`) endpoint constants,
6. `nlang`: the Natural Language Boolean Algebra (requires `DEEPSEEK_API_KEY`), and
7. `hsb`: the Boolean algebra of lex-half-open polyhedra in ℝ^d — generalizes `qint` from 1D to d dimensions using canonical halfspaces (see [hsb](#hsb--lex-half-open-polyhedra)).

You can type the following elements: variables, streams, recurrence relations,
constants and (term) constants. In order to do so, you just add the type
information after a colon `:`. For example,

```
x : sbf
o1[t] : bv
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

Internally, endpoints are stored as exact fixed-point numbers with scale
`2^30` (the real value of a raw integer `r` is `r / 2^30`), so only rationals
whose denominator divides `2^30` are representable as constants; other
rationals cannot be expressed as `qint` literals.  Both rational (`1/4`) and
decimal (`0.25`) syntaxes are accepted for representable endpoints.

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
| `x[0] < 0` | Single open halfspace |
| `x[0] <= 0` | Single closed halfspace |
| `x[0]*0.5 + 0.7 < 0` | Halfspace with coefficient and bias |
| `0.5*x[0] + -0.3*x[1] + 0.7 < 0` | Multivariate halfspace |
| `x[0] < 0 & x[1] < 0` | Conjunction of halfspaces |

Coefficients can appear before (`0.5*x[0]`) or after (`x[0]*0.5`) the variable.
Conjunctions use `&` as separator.  Disjunctions and negations are composed
via the Boolean algebra operations at the formula level.

The test suite (`tests/unit/test_hsb.cpp`) covers Fourier-Motzkin elimination,
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

The DeepSeek API (via the `DEEPSEEK_API_KEY` environment variable) serves as the
semantic oracle for equality, emptiness, and universality tests.  Without a valid
API key the system cannot decide Boolean questions over `nlang` elements.

Elements are written as natural language strings inside `{...}:nlang`:

```
{ mammals }:nlang
{ large birds }:nlang
{ X | (Y & Z) }:sbf         -- not nlang; use plain strings for nlang
```

The content inside `{}` for `nlang` is interpreted as a natural language phrase,
not a Boolean formula.  Compound descriptions should be expressed as English
phrases rather than symbolic connectives.

**Requirement**: `DEEPSEEK_API_KEY` must be set in the environment.

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
Also note that the type `bv` actually represents a family of types, one for each bit width.
For example, `bv[8]` is the type of bitvectors of width 8, while `bv[16]` is the type
of bitvectors of width 16 (the default bit width is 16 if not specified or inferred otherwise).

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

2. `all x:bv x = y`:
      - `x` is typed `bv` (`bv[16] by default),
      - `y` is inferred to be of the same type as `x`, i.e. `bv`,
      - no type mismatch occurs.
3. `all x x = y`:
      - `x` is inferred to be of the default type `tau`, as no type information is present,
      - `y` is inferred to be of the same type as `x`, i.e. `tau`,
      - no type mismatch occurs.
4. `(all x x = y) && x = y:sbf`:
      - `x` is inferred to be of the same type as `y` in the first part, i.e. `tau`, as no type information is present,
      - `y` is inferred to be of type `sbf` in the outer formula which is not compatible with the previous type assigned (`tau`).
      - a type mismatch occurs.
4. `all x (all x x = 1:sbf)`:
      - the inner `x` is inferred to be of type `sbf`,
      - the outer `x` is inferred to be of the default type `tau`,
      - no type mismatch occurs as both `x` are in different scopes.
5. `ex x : bv x = 1 : bv[8]`:
      - `x` is typed as `bv`,
      - the constant `1` is typed as `bv[8]`,
      - `x` is inferred to be of the same type as the constant `1`, i.e. `bv[8]`,
      - no type mismatch occurs.

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
algebra elements.

# **Command line interface**

Tau-lang ships two executables:

- **`tau`** — the interactive / interpreter executable.  Runs a specification
  by solving each time step through the core solver pipeline.  Best for REPL
  use, spec authoring, and executing specifications dynamically.
- **`tau_codegen`** — the ahead-of-time compiler.  Reads a specification,
  invokes the synthesis pipeline, and emits a standalone C++17 header that
  implements a synthesized state-machine program.  Best for deploying
  realized specs at memory-bandwidth speed (billions of steps per second).

Both executables share the same input spec format.  Use whichever matches
your deployment story.

## `tau` — interpreter and REPL

The general form of the tau executable command line is:

```bash
tau [ options ] [ <specification> ]
```

where `[ options ]` are the command line options and `[ <specification> ]` is
the Tau specification file you want to run.  If you omit the tau specification,
the Tau REPL will be started.

The general options are the following:

| Option              | Description                                             |
|---------------------|---------------------------------------------------------|
| -h, --help          | detailed information about options                      |
| -l, --license       | show the license                                        |
| -v, --version       | show the version of the executable                      |
| -V, --charvar       | char-as-variable short form (enabled by default)        |
| -S, --severity      | severity level (trace/debug/info/error); default `info` |
| -I, --indenting     | indent formulas in output                               |
| -H, --highlighting  | syntax highlighting                                     |
| -B, --benchmarks    | print benchmarks (enabled by default)                   |
| -J, --json          | output in JSON format                                   |
| -q, --quit          | quit when no input is available (scripted runs)         |
| -x, --experimental  | enable experimental / transitioning features            |

Whereas the REPL specific options are:

| Option             | Description                                  |
|--------------------|----------------------------------------------|
| -e, --evaluate     | REPL command to evaluate and exit            |
| -s, --status       | display status in the REPL prompt            |
| -c, --color        | use terminal colors                          |
| -d, --debug        | debug mode (present in debug builds)         |

## `tau_codegen` — synthesis-to-C++ compiler

Compiles a tau specification to a standalone C++17 header containing a
synthesized program expressed as a `switch/case` state machine.  The generated
code is a pure switch/case with no runtime dependency on libTAU for
propositional specs, and compiles with `g++ -O3 -flto -std=c++17`.

```bash
tau_codegen [ <spec_file> | - ] [ -o <output.h> ] [ --class <Name> ]
```

| Option              | Description                                                    |
|---------------------|----------------------------------------------------------------|
| -h, --help          | show usage and exit                                            |
| `<spec_file>`       | path to a `.tau` specification; use `-` or omit for stdin      |
| -o `<output.h>`     | emit to this file; omitted means write to stdout               |
| --class `<Name>`    | class identifier for the emitted program; default `TauProgram` |

Exit codes:

| Code | Meaning                                                           |
|------|-------------------------------------------------------------------|
| 0    | success; header written                                           |
| 1    | parse error or I/O error                                          |
| 2    | usage error (unknown flag, missing argument)                      |
| 3    | specification is UNREALIZABLE                                     |

Example session:

```bash
echo -n 'G(o1[t]:bv = i1[t]:bv)' > spec.tau
tau_codegen spec.tau -o program.h --class Echo
g++ -O3 -flto -std=c++17 driver.cpp -o sim
./sim
```

See `examples/reactive_program/` for a fully worked Makefile example.

## When to use which

| Situation                                               | Use         |
|---------------------------------------------------------|-------------|
| Interactive spec authoring / debugging                  | `tau` REPL  |
| One-off running of a spec against inputs                | `tau`       |
| Checking satisfiability / realizability of a spec       | `tau`       |
| Deploying a realized spec at high throughput            | `tau_codegen` |
| Integrating the synthesized behavior into a C++ project | `tau_codegen` |
| Specs with data atoms requiring runtime witness search  | both: `tau_codegen` emits stubs, `tau` / `libTAU` resolves at runtime |

# **The Tau REPL**

The Tau REPL is a command line application that allows you to interact with the Tau
Language. It is a simple and easy to use tool that enables you to write and
execute Tau specifications on the fly.

## **Basic REPL commands**

The Tau REPL provides a set of basic commands that allow you to obtain help,
version information, exit the REPL and clear the screen. The syntax of
the commands is the following:

* `help|h [<command>]`: shows a general help message or the help message of a
specific command.

* `version|v`: shows the version of the Tau REPL. The version of the Tau REPL
corresponds to the repo commit.

* `quit|q` or `exit`: exits the Tau REPL.

* `clear|c`: clears the screen.

## **REPL options**

You have several options at your disposal to configure the Tau REPL. In order
to set or get the value of an option you can use the following commands:

* `get [<option>]`: shows all configurable settings and their values or a single
one if its name is provided.

* `set <option> [=] <value>`: sets a configurable option to a desired value.

* `toggle <option>`: toggle an option between on/off.

The options you have at your disposal are the following:

* `c|color|colors`: Can be on/off. Controls usage of terminal colors in its
output. It's on by default.

* `s|status`: Can be on/off. Controls status visibility in the prompt. It's on
by default.

* `sev|severity`: Possible values are trace/debug/info/error. The value determines
how much information the REPL will provide. This is set to error by default.

* `h|hilight|highlight`: Can be on/off. Controls usage of highlighting in the
output of commands. It's off by default.

* `i|indent|indentation`: Can be on/off. Controls usage of indentation in the
output of commands. It's on by default.

* `charvar|v`: Can be on/off. Controls usage of character variables in the
REPL. It's on by default.

* `d|dbg|debug`: Can be on/off. Controls debug mode. It's off by default.

## **Functions, predicates and input/output stream variables**

As in other programming languages, you can define functions, predicates (both possibly using recurrence
relations) but also input and output stream variables. The syntax of the commands
is the following:

* `definitions|defs`: shows all the definitions of the current session. That
includes the definitions of functions, predicates and the input/output stream variables.

* `definitions|defs <number>`: shows the definition of the given function or predicate.

* `predicate_def`: defines a predicate, supporting the usage of
recurrence relations. See the Tau Language section
[Functions and predicates](#functions-and-predicates) for more information.

* `function_def`: defines a function, supporting the usage of
recurrence relations. See the Tau Language section
[Functions and predicates](#functions-and-predicates) for more information.

* `<type> i<number> := console | ifile(<filename>)`: defines an input stream variable.
The input variable can read values from the console or from a provided file. <br>
`<type>` can be `tau`, `sbf`, `bv`, `qlt`, `qint`, or `nlang`.

* `<type> o<number> := console | ofile(<filename>)`: defines an output stream variable.
The output variable can write values to the console or into a file. <br>
`<type>` can be `tau`, `sbf`, `bv`, `qlt`, `qint`, or `nlang`.

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

* `subst|s <repl_memory|tau|term> [<repl_memory|tau|term>/<repl_memory|tau|term>]`: substitutes a
memory, well-formed formula or Boolean function by another one in the given
expression (this one being a memory position, well-formed formula or Boolean
function).

* `instantiate|inst|i <repl_memory|tau> [<var>/<repl_memory|term>]`: instantiates a variable
by a memory position, well-formed formula or Boolean function in the given
well-formed or Boolean function expression.

* `instantiate|inst|i <repl_memory|term> [<var>/<repl_memory|term>]`: instantiates a variable
by a memory position or Boolean function in the given expression.

## **Logical procedures**

The Tau REPL also provides a set of logical procedures that allow you to check
several aspects of the given specification/well-formed formulas/Boolean functions.
The syntax of the commands is the following:

* `valid <repl_memory|tau>`: checks if the given specification is valid.

* `sat <repl_memory|tau>`: checks if the given specification is satisfiable.

* `unsat <repl_memory|tau>`: checks if the given specification is unsatisfiable.

* `solve <repl_memory|tau>`: solves the given system of equations given by the
well-formed formula. It only computes one solution.

* `normalize|n <repl_memory|rr|ref|tau|term>`: normalizes the given expression. See
the TABA book for details.

* `qelim <repl_memory|tau>`: performs quantifier elimination on the given expression.

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

## **Specification execution**

Finally, you can run a given Tau specification. The syntax for the commands is:

* `run|r <repl_memory|tau>`: runs the given Tau specification.

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
fragment ltl.        # default — standard LTL operators
fragment ctl_star.   # enable CTL* path quantifiers A, E and semantic negation -
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
| `src/algorithm_c_skeleton.h` | Available | Algorithm C: oracle-assisted abstract game. `A_{ρ,J}` encoding — O(\|T_1\|·2^K) inputs, K+\|T_1\| outputs, independent of \|T_3\|. |

## Type enumeration (ω-categorical theories)

| Header | Purpose |
|--------|---------|
| `src/omcat_types.h` | Rational `Rat` type. `QltType1`/`QltType2`/`QltType3` structs for 1-/2-/3-types of (ℚ,<,Σ). `enumerate_qlt_T1` (2k+1 types from k constants), `enumerate_qlt_T2` (T₂ = (pos_m, pos_x, rel_mx) with forced-relation filtering), `enumerate_qlt_T3` (T₃ with transitivity filter). `realize()` rational witnesses. `Pre_over_T1` controllable-predecessor operator; `nu_fixpoint`/`mu_fixpoint` over 2^{T_1}; `reachable_from` BFS. |
| `src/omcat_constants.h` | `parse_rat_literal` for rational/decimal strings; `collect_qlt_constants(fm)` harvesting named constants from a formula. |
| `src/omcat_oracle_cache.h` | Thread-safe runtime cache for atomic oracle answers (tp(m,x) and achievability-set A_{ρ,J}). |

## Supporting infrastructure

| Header | Purpose |
|--------|---------|
| `src/gr1_detect.h` | `is_gr1_fragment(fm, &n_safety, &n_liveness)` classifier for `⋀ G(ψ_safe) ∧ ⋀ GF(ψ_live)`. |
| `src/liveness_decomp.h` | `decompose_liveness(fm)` splits GR(1)-shaped formulas into safety part + GF bodies. |
| `src/spec.h` | `Spec { transient; invariant; reactive }` with `decompose_spec(fm)` classifier. |
| `src/parse_error_hint.h` | `classify_parse_error(formula)` for actionable parse error messages. |
| `src/tau_lang_api.h` | Stable public API: `tau_lang_is_realizable(formula_str)` returning `{REALIZABLE, UNREALIZABLE, ERROR}`. |

## Algorithm A/B soundness rule

Algorithm A has no input propositions, so it is sound only when the formula has
no input variables. If a `qlt` formula contains input variables, the dispatcher
uses Algorithm B even if `TAU_LTL_ALG=A` was requested. Algorithm B adds P-bits
for the current T₂ type, making the synthesized strategy type-aware.

# **Python bindings (ctypes)**

A minimal ctypes-based Python shim is shipped in `bindings/python/` for
scripted LTL(ABA) realizability decisions.  Configure with
`-DTAU_BUILD_BINDING_PYTHON=ON`; the `tau_lang` CMake target produces
`libtau_lang.so` (or `.dylib` / `.dll`).  Usage:

```python
from tau_lang import decide, Verdict, last_error

v = decide("G ((o1[t]:qlt > {1/4}:qlt) && (o1[t]:qlt < {3/4}:qlt)).")
if v == Verdict.REALIZABLE:
    print("realizable")
elif v == Verdict.UNREALIZABLE:
    print("unrealizable")
else:
    print("error:", last_error())
```

Set the `TAU_LTL_LIB` environment variable to the absolute path of the
built shared library if it isn't on the default search path.  The C API
header (`bindings/python/tau_lang_c_api.h`) is directly usable by other
foreign-function-interface callers.

The parse-error hint classifier is also exposed as a pure-header
utility at `src/parse_error_hint.h` — external consumers can call
`idni::tau_lang::classify_parse_error(formula)` after a failed parse to
obtain a short, actionable explanation.

# **Documentation (mdBook)**

A comprehensive user guide is available as an [mdBook](https://rust-lang.github.io/mdBook/) site in the `docs/` folder. It covers the full language reference, all APIs (C++, C, Python, REPL, codegen), tutorials, an error catalog, algorithm selection guide, and performance best practices.

## Building the docs

Install mdBook (requires [Rust](https://rustup.rs/)):

```bash
cargo install mdbook
```

Or download a prebuilt binary from [mdBook releases](https://github.com/rust-lang/mdBook/releases).

Build and serve locally:

```bash
cd docs
mdbook build          # generates docs/book/
mdbook serve          # live-reload at http://localhost:3000
```

The generated site is in `docs/book/` and can be deployed to any static hosting (GitHub Pages, Netlify, etc.).

## Contents

| Section | Pages | Description |
|---------|-------|-------------|
| Getting Started | 3 | Introduction, installation, quick start |
| Language Reference | 11 | Types, temporal operators, Boolean functions, streams, constants, variables, functions, quantifiers, PWR, CTL* |
| API Guide | 5 | C++ API, C API, Python API, REPL, codegen |
| Tutorials | 5 | First spec, multi-output, input-output, runtime revision, Mealy machine extraction |
| Reference | 4 | Operator precedence, grammar, error catalog, algorithm selection |
| Performance | 3 | Best practices, ltlsynt caching, formula complexity |


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
* Minor errors in Windows REPL
* LTL-specific:
  * Always include time indices `[t]` on io_vars: `G(o1[t]:sbf = i1[t]:sbf)`.
    Omitting them (e.g. `G(o1:sbf = i1:sbf)`) is a syntax error.
  * Input variables in the left operand of `U`/`W` cannot be synthesised.
  * `G(phi_A) && G(phi_B)` with **different** BA types raises "nesting of temporal
    quantifiers"; use `F`, or `G+F` combinations with different types instead.
  * `G(phi_A) && G(phi_B)` with the **same** BA type is automatically merged to
    `G(phi_A && phi_B)` — both forms work.
  * Mealy strategies with any number of states are executable.
  * `S` (since) and `T` (trigger) past LTL operators are compiled away to auxiliary
    variables; ltlsynt integration for pure past operators is pending.
  * `nlang` type requires `DEEPSEEK_API_KEY` to be set; without it, any oracle call
    over `nlang` elements will fail at runtime.
  * **Algorithm A** is intentionally restricted to pure-output formulas. If
    input variables are present, the dispatcher uses Algorithm B.


# **Future work**

* Enabling modular arithmetic using fixed width bitvectors in Tau specifications
* Enabling efficient data storage and manipulation in Tau specifications using Boolean functions
* Overcoming performance issues during normalization of formulas and satisfiability checking of Tau specifications
* Add support for redefinitions of functions or predicates.
* Add support for arbitrary stream names.
* Improve the performance of Boolean function normalization.
* **Past LTL S/T operators**: currently compiled away to auxiliary variables; full
  integration with ltlsynt for pure past-LTL fragments is planned.
* **Mealy strategy execution for large strategies**: extend the interpreter to
  support Mealy strategies with more than 8 states.
* **nlang_ba**: support for additional LLM backends (OpenAI, local models) beyond
  the current DeepSeek oracle.
* **qlt/qint synthesis**: further polish of the QE oracle for reactive synthesis
  over `qlt` (DLO) and `qint` (interval BA) types — decimal and rational constants
  are supported, the dedicated DLO QE path handles the `qlt` ω-categorical theory,
  and the data oracle is cross-validated against cvc5 LRA in `test_qlt_oracle`.
* **Algorithm D Phase 2/3**: Algorithm D currently solves the product game for
  safety/reachability objectives.  Extension to the full μ/ν fixpoint parity-game
  formulation (T₃×Q pre-derivation) is planned — this requires derivation of the
  T₃×Q game from the book's §6.7 algorithm and is blocked on interactive design.
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

Please submit issues at the following link: [Tau Language issues](https://github.com/IDNI/tau-ltl/issues).

# **License**

Tau Language is licensed under the following terms:
[Tau Language License](https://github.com/IDNI/tau-ltl/blob/main/LICENSE.md)

# **Authors**

The Tau Language has been developed by the following authors:

* Ohad Asor
* David Castro Esteban
* Tomáš Klapka
* Lucca Tiemens
