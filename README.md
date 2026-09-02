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
4. [The Tau Language](#the-tau-language)
    1. [Tau specifications](#tau-specifications)
    2. [Satisfiability and execution](#satisfiability-and-execution)
    3. [Boolean functions](#boolean-functions)
    4. [Bitvectors](#bitvectors)
    5. [Functions and predicates](#functions-and-predicates)
    6. [Constants](#constants)
    7. [Streams](#streams)
    8. [Variables and uninterpreted constants](#variables-and-uninterpreted-constants)
    9. [Type system](#type-system)
    10. [Abstract data types](#abstract-data-types)
    11. [Constant time constraints](#constant-time-constraints)
    12. [Pointwise revision](#pointwise-revision)
    13. [Reserved symbols](#reserved-symbols)
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
7. [The C++ API and language bindings](#the-c-api-and-language-bindings)
8. [The Theory behind the Tau Language](#the-theory-behind-the-tau-language)
9. [Known issues](#known-issues)
10. [Future work](#future-work)
11. [Submitting issues](#submitting-issues)
12. [License](#license)
13. [Authors](#authors)


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

To compile the source code you need a recent C++ compiler supporting C++23, e.g.
GCC 13.1.0. You also need at least cmake version 3.22.1 installed in your system.
The only external code dependencies are the Boost C++ Libraries (the `log` component)
and the CVC5 SMT Solver.
CVC5 is used only in order to support the theory of bitvectors within the language.
The core language and its algorithms are independent of CVC5.

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


# **Quick start**

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
"always" and "sometimes" statements applied to *local specifications*
(expressed by `local_spec`, see below),
combined by the logical
connectives *and*, *or* and *not*, denoted by `&&`, `||` and `!` respectively.
For example a well-formed Tau specification is
```
(always local_spec1) && (sometimes local_spec2)
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
each point in time. The key point now is that an `always` statement will
quantify all scoped `t` universally, while a `sometimes` statement will quantify
them existentially. For example the specification `always o1[t] = 0` says that
at all time-steps the output stream number 1 will write `0`. Similarly, the
specification `sometimes o1[t] = 0` says that there exists a time-step at which
the output stream 1 will write `0`. When executing a Tau specification, the first
time-step is always 0.

Formally, a specification is a (possibly empty) list of definitions followed by
a single formula, its *main* formula:
```
spec        => [ definitions ] local_spec [ "." ]
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
`... ? ... : ...` > `always ...`> `sometimes ...`.

A Tau specification without a mentioning of "always" or "sometimes" is implicitly
assumed to be an "always" statement.

Note that instead of writing `always` and `sometimes` you can also use box `[]`
and diamond `<>`, respectively.

Using this notation, a slightly bigger example of a Tau spec would be

```
    ([] o1[t] i1[t] = 0 && (i1[t] != 1 -> o1[t] != 0)) && (<> o1[t] = i1[t]')
```

which reads: at each point of time, the output should be disjoint from the input.
If the input is not 1, then the output is not zero. And, at least once during
execution, the output equals the complement of the input.


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

Take for a minimal example the specification `always o1[t] = i1[t]`,
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
values of `x`. They are currently parsed and preserved through normalization as
atomic terms, but not yet evaluated,
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
algebra.

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

The last entry is a cast. It converts its operand to the bitvector type of the
given width, for example:

```
(bv[8]) x = { #x1f } : bv[8]
```

The cast operand must be a parenthesized term, a constant, a variable, a
function call, `0`, `1`, a negation, a functional quantifier or another cast;
wrap anything else in parentheses.

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
rely on it implicitly. When a divisor can be zero, guard it in the
specification and pick the zero-case value explicitly:

```
always ( ( i2[t]:bv[24] != { #x000000 }:bv[24] && o1[t]:bv[24] = ( i1[t]:bv[24] / i2[t]:bv[24] ) )
      || ( i2[t]:bv[24] =  { #x000000 }:bv[24] && o1[t]:bv[24] = { #x000000 }:bv[24] ) ).
```

Here `1200 / 3` gives `400` as usual, while a zero divisor gives `0` — the
spec author's own choice — instead of the inherited `16777215`.

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
f[0](w,x,y,z) := x & { always o1[t] = 0 } != 0
f[n](w,x,y,z) := f[n-1](x,y,z,w)

h[0](w,x,y,z) := f[0](w,x,y,z)
h[n](w,x,y,z) := h[n-1](w,x,y,z) && f[n](w,x,y,z)
```
As a result, calling `h[3](w,x,y,z)` checks that none of the arguments `w, x, y, z`
assumes `sometimes o1[t] != 0`.

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

In the Tau language, we currently support three non-atomless Boolean algebras,
which we also call the base Boolean algebras:
1. the Boolean algebra of Tau specifications (also referred to as Tau Boolean algebra)
2. the Boolean algebra of simple Boolean functions
3. the Boolean algebra of bitvectors of fixed bit width

Several others are in development, like the Boolean algebra of Boolean (not just
simple) functions in general.

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
in which the stream is used. For example, the specification `always o1[t] = 0`
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
`sbf`, `bv[8]`, ..., or a user-defined type — see
[Abstract data types](#abstract-data-types)), `member_path` is an optional
tuple-member path (also discussed there), `in` marks an input stream and `out`
an output stream, and
`stream` is either `console` (meaning that the stream reads/outputs values
from/to the console) or `file(file_name)` which denotes the file from/into which
to read/write (in quotes if needed). For example,
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
2. `sbf`: the type of simple Boolean functions, and
3. `bv[n]`: the type of bitvectors of bit width `n`.

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
      - the constant `1` is typed as `bv[8]`,
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

Temporal normalization of always specification reached fixpoint after 0 steps, yielding the result:
i1[t]u[t]' = 0 && i1[t]'u[t] = 0

-----------------------------------------------------------------------------------------------------------
Please provide requested input, or press ENTER to terminate                                               |
If no input is requested, press ENTER to continue to the next execution step, or type q(uit) to terminate |
-----------------------------------------------------------------------------------------------------------

Execution step: 0
i1[0] := o1[t] = 1
u[0] := always o1[t]' = 0

Updated specification:
always o1[t]' = 0 && i1[t]u[t]' = 0 && i1[t]'u[t] = 0

Execution step: 1
i1[1] := o2[t] = 0 && o2[t] = 1
o1[1] := T
u[1] := F

Execution step: 2
i1[2] := o1[t] = 0
o1[2] := T
u[2] := always o1[t] = 0

Updated specification:
always o1[t] = 0

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
2) Let us refer to the currently running specification as `spec`. `spec` is composed of a single `always` statement
and possibly several `sometimes` statements. We denote the `always` part by `aw_spec` and the collection of
`sometimes` parts by `st_spec`. In the same way `aw_update` denotes the `always` part of `update`.
The next candidate for the updated specification, let's call it `U`, is given by <br>
`U := update && ( (ex [outputs] aw_update && aw_spec) -> aw_spec)`, where `[outputs]` refers to the list of all output streams present
in `aw_spec` and `aw_update` combined. The meaning is that, whenever possible given the current input at a particular step,
`update` and `aw_spec` are executed together. The name _pointwise revision_ originates from this behavior.
Note that it is possible to refine the definition of the new specification
in more advanced ways. We will explore this aspect in the future. <br>
If `update` is satisfiable, then `U` is satisfiable,
unless the `sometimes` part of `update`
prevents it. If it is prevented, `update` becomes the final updated specification.
Otherwise, `U` becomes the updated specification and the next step is performed.
3) As a final step, it is checked if the previous `sometimes` statements `st_spec` are executable along the updated specification `U`.
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

The general form of tau executable command line is:

```bash
tau [ options ] [ <specification file> ]
```

where `[ options ]` are the command line options and `[ <specification file> ]` is
the path to a file containing the Tau specification you want to run. Use `-` to
read the specification from standard input. If you omit the file, the Tau REPL
will be started.

The general options are the following:

| Option             | Description                                           |
|--------------------|-------------------------------------------------------|
| -h, --help         | detailed information about options                    |
| -l, --license      | show the license                                      |
| -v, --version      | show the version of the executable                    |
|--------------------|-------------------------------------------------------|
| -V, --charvar      | charvar (enabled by default)                          |
| -B, --blasting     | bitvector predicate blasting (disabled by default)    |
| -S, --severity     | severity level (trace/debug/info/error)               |
| -I, --indenting    | indenting of the formulas                             |
| -H, --highlighting | syntax highlighting                                   |
| -b, --benchmarks   | print benchmarks (enabled by default)                 |
| -J, --json         | output in JSON format                                 |
| -q, --quit         | quit when no input is available                       |

whereas the REPL specific options are:

| Options            | Description                                            |
|--------------------|--------------------------------------------------------|
| -e, --evaluate     | REPL command to be evaluated                           |
| -s, --status       | display status (enabled by default)                    |
| -c, --color        | use colors (enabled by default)                        |
| -X, --legacy-repl  | use the legacy terminal REPL instead of the FTXUI one  |
| -x, --experimental | enable transitioning features                          |
| -d, --debug        | debug mode (Debug builds only)                         |

and the limit options, which bound the engine's iterative searches. Every
cap defaults to unlimited (`0`); the two gc knobs keep their tuned defaults.
Each has a matching REPL option (see [REPL options](#repl-options)):

| Option                        | Description                                                                            |
|-------------------------------|----------------------------------------------------------------------------------------|
| -w, --spec-size-warn          | warn when an updated specification exceeds this many characters (0 = off)              |
| -a, --max-revision-alts       | cap the revision alternatives kept per specification part (0 = unlimited)              |
| -p, --block-max-splits        | cap per-block Boole-decomposition splits in anti-prenexing (0 = unlimited)             |
| -r, --block-max-rounds        | cap anti-prenexing quantifier-block driver rounds (0 = unlimited)                      |
| -Q, --cqe-max-clauses         | cap the DNF clauses complete quantifier elimination may distribute one scope into (0 = unlimited) |
| -f, --max-fixpoint-steps      | cap temporal-normalization fixpoint steps (0 = unlimited)                              |
| -F, --max-flag-search-steps   | cap the eventual-flag search past the flag boundary; give-up reports unsat (0 = unlimited) |
| -D, --max-blast-reentry-depth | cap blast-block re-entry nesting in anti-prenexing (0 = unlimited)                     |
| -z, --block-squeeze-cap       | skip block squeezing above this operand-set size (0 = unlimited)                       |
| -m, --max-simplify-rounds     | cap bitvector simplification rewrite rounds (0 = unlimited)                            |
| -P, --max-def-passes          | cap definition-expansion passes (0 = unlimited)                                        |
| -E, --max-enum-steps          | cap recurrence-relation enumeration steps (0 = unlimited)                              |
| -R, --max-rewrite-rounds      | cap rewrite-to-fixpoint rounds (0 = unlimited)                                         |
| -G, --gc-min-size             | tree-node count floor before gc may trigger (default 256)                              |
| -W, --gc-growth-factor        | gc triggers when node count grows by this factor since last sweep (default 1.5; <= 0 disables gc) |

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

* `B|blasting`: Can be on/off. Controls bitvector predicate blasting, i.e.
whether bitvector predicates are expanded into their bit-level encoding. It's
off by default (the REPL starts with the value of the `-B, --blasting` command
line option, which defaults to off).

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

* `maxclauses|cqemaxclauses`: cap on the DNF clauses complete quantifier
elimination may distribute one scope into (`--cqe-max-clauses`). Unlimited by
default.

* `fixpointsteps|maxfixpointsteps`: temporal-normalization fixpoint step cap
(`--max-fixpoint-steps`). Unlimited by default.

* `flagsteps|maxflagsearchsteps`: cap on the eventual-flag search past the
flag boundary; a bounded give-up reports unsatisfiable
(`--max-flag-search-steps`). Unlimited by default.

* `blastdepth|maxblastreentrydepth`: blast-block re-entry nesting cap in
anti-prenexing (`--max-blast-reentry-depth`). Unlimited by default.

* `squeezecap|blocksqueezecap`: operand-set size above which block squeezing
declines (`--block-squeeze-cap`). Unlimited by default.

* `simplifyrounds|maxsimplifyrounds`: bitvector simplification rewrite round
cap (`--max-simplify-rounds`). Unlimited by default.

* `defpasses|maxdefpasses`: definition-expansion pass cap
(`--max-def-passes`). Unlimited by default.

* `enumsteps|maxenumsteps`: recurrence-relation enumeration step cap
(`--max-enum-steps`). Unlimited by default.

* `rewriterounds|maxrewriterounds`: rewrite-to-fixpoint round cap
(`--max-rewrite-rounds`). Unlimited by default.

* `gcminsize`: tree-node count floor before the interpreter's gc may trigger
(`--gc-min-size`). 256 by default.

* `gcgrowth|gcgrowthfactor`: gc growth-factor trigger; accepts decimals such
as `1.5` (`--gc-growth-factor`). 1.5 by default; a value at or below 0
disables gc.

* `specsizewarn`: warn when an updated specification exceeds this many printed
characters (`--spec-size-warn`). 0 (off) by default.

* `revisionalts|maxrevisionalts`: cap on revision alternatives kept per
specification part (`--max-revision-alts`). Unlimited by default.

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

* `substitute|subst|s <repl_memory|tau|term> [<repl_memory|tau|term>/<repl_memory|tau|term>]`: substitutes a
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

* `solve [<options>] <repl_memory|tau>`: solves the given system of equations given
by the well-formed formula, computing a single satisfying assignment for its free
variables. The available options are:
	* `--min|--minimum`: computes a minimum solution of the system,
	* `--max|--maximum`: computes a maximum solution of the system,
	* `--<type>`: uses the given type (`sbf`, `tau`, ...) for the solution.

* `lgrs [--<type>] <repl_memory|tau>`: computes a least general reproductive
solution (LGRS) for the given equation.

* `normalize|n <repl_memory|rr|ref|tau|term>`: normalizes the given expression. See
the TABA book for details.

* `qelim <repl_memory|tau>`: eliminates the non-temporal quantifiers in the given
expression.

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

# **The C++ API and language bindings**

Besides the executable and the REPL, the framework is usable as a library.

The public C++ API is [`src/api.h`](src/api.h). All operations are exposed as
static methods on `api<node>`, and cover parsing (`get_spec`, `get_formula`,
`get_term`, `get_definition`, ...), printing, substitution and instantiation,
the logical procedures, the normal forms and the execution of specifications
(`get_interpreter`, `get_inputs_for_step`, `step`). Global switches such as
`set_charvar`, `set_blasting`, `set_indenting`, `set_highlighting`, `set_json` and
`set_severity` mirror the command line options.

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
outputs can be driven from Python:

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


# **Future work**

* Enabling efficient data storage and manipulation in Tau specifications using Boolean functions
* Overcoming performance issues during normalization of formulas and satisfiability checking of Tau specifications
* Add support for redefinitions of functions or predicates.
* Support the Boolean algebra of Boolean (not just simple) functions in general.
* Improve the performance of Boolean function normalization.

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
