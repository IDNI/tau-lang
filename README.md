![The TAU logo](/docs/images/tau_logo.png)

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
	2. [Compiling the source code](#compiling-the-source-code)
3. [Quick start](#quick-start)
4. [The Tau Language](#the-tau-language)
	1. [Tau specifications](#tau-specifications)
	2. [Boolean functions](#boolean-functions)
	3. [Functions and predicates](#functions-and-predicates)
	4. [Constants](#constants)
	5. [Variables](#variables)
	6. [Pointwise revision](#pointwise-revision) 
	7. [Reserved symbols](#reserved-symbols)
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
7. [The Theory behind the Tau Language](#the-theory-behind-the-tau-language)
8. [Known issues](#known-issues)
9. [Future work](#future-work)
10. [Submitting issues](#submitting-issues)
11. [License](#license)
12. [Authors](#authors)


# **Introduction**

The Tau language is an expressive, decidable and executable formal software specification
language. It allows for specifying software using requirements and constraints in a purely
logical and declarative approach.
Tau specifications can be checked for satisfiability, to decide if there exists a program
matching the specification but, most importantly, can also be executed. The Tau framework
is able to synthesize a program adhering to a given specification.

Imagine programming by writing only tests, and getting a working program that
passes all the tests, thus meeting all the specified requirements.

A unique feature of the Tau language is the ability to specify programs capable of mechanized
deductive reasoning over Tau specifications themselves. This allows for a controlled adaption of new
requirements because the requirements for the requirements can be taken into account,
which is useful for collaborative specification, maintenance,
updates, safety and user control.

To go to the quick start [click here](#quick-start).

To go to the installation guide [click here](#installing-the-tau-language-framework).

For viewing known issues, future work and how to submit issues, [click here](#known-issues).

For visiting the theory behind Tau Language [click here](#the-theory-behind-the-tau-language).

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
GCC 13.1.0. You also need at least a cmake version 3.22.1 installed in your system.
The only code dependencies are Boost C++ Libraries (libboost) an Z3 SMT Solver (libz3-dev).

After cloning:

```bash
git clone https://github.com/IDNI/tau-lang.git
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

# **Quick start**

To start using the Tau Language, download the latest release from the
[GitHub page](https://github.com/IDNI/tau-lang/releases/tag/v0.7-alpha). Once
you have downloaded and installed the executable (see the Section
[Installing the Tau Framework](#installing-the-tau-language-framework)), you can run
it from the command line by typing `tau`.

The programming model underlying the Tau Language is fully declarative. You
specify, possibly only very implicitly, how the current and previous inputs and
outputs are related, at each point of time. So what you write in the Tau Language
is not a program, but a specification, or spec, which represents all programs that
meet the specification. Once you run a specification, you actually run one
automatically-chosen representative program from that set.

In the scope of the Tau Language, a specification means that for all inputs, at each
point in time, exist outputs, that clearly do not depend on future inputs
("time-compatible"). Implied from this definition is that all specifications run
indefinitely no matter what the inputs are.

For example, the following specification:

```
o1[t] = 0
```

states that the output `o1` at all time points (`t`) has to be `0`. Similarly, the
following specification:

```
o1[t] = i1[t]
```

states that the output `o1` at time `t` has to be the same as the input
`i1` at time `t`.
In the above examples, `o1` and `i1` are IO variables. They are used to define
the inputs and outputs of the specified specifications and also declare their type.

An example of how to define IO variables is the following:

```
tau i1 = console
tau o1 = console
```

In the above case we specify that `i1` and `o1` are of type `tau`, namely that
they are Tau formulas, and they take values from the console (e.g. stdin/stdout).
Type actually means which Boolean algebra the values come from. It is a unique
property of the Tau langauge that it can refer to its own sentences, abstracted
as Boolean algebra elements.

You can also define files as IO streams:

```
tau i1 = ifile("input.in")
tau o1 = ofile("output.out")
```

Back to the above two examples, the one where the output is always zero and the
one where it equals the input, those Tau specs define only one specification each
(there's a caveat in this statement but we shall treat it later on). An example
of a Tau spec that specifies infinitely many programs would be:

```
o1[t] & i1[t] = 0
```

Here `&` is conjunction in the Boolean algebra from which the inputs and outputs
are taken from. This spec says that the conjunction has to be empty.
Clearly, you can consider more complicated specifications, e.g.:

```
o1[t] & o1[t-1] & i1[t] = 0 || o1[t] = i1[t]
```

which states that at each point in time, either the conjunction of the current
output with the previous output and with the current input, has to be 0, or, the
output has to equal the input. Note the difference between Boolean (algebraic
operations) and Logical operators. The former are &|', and the latter are &&,||,!.

In order to simplify the process of writing and running Tau specifications, we allow
to define functions and predicates, possibly by means of recurrence relations.
The following is a simple predicate defined by a recurrence relation,
which takes as argument a Tau formula:

```
f[0](y) := T
f[n](y) := f[n - 1](y)
```

which you can use in your spec as follows:

```
o1[t] = 0 && f(i1[t])
```

Also, you can use the following recurrence relation definition

```
g[0](y) := 0
g[n](y) := g[n](y)'
```

which defines a function (rather a predicate) and alternates between 0 and 1
depending on the parity of n.

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
We say local specification because a formula `tau` can only talk about a fixed
(though arbitrary) point in time.

In order for a specification to communicate with the outside world, so-called *streams*
are use. Those streams come in two flavors: input and output streams. Input
streams are used in a `local_spec` to receive input from a user, while output streams
are used for presenting output to a user. Each stream is associated with a point in time
in the specification.
For example the output stream variable `o1[t-2]` means
"the value in output stream number 1 two time-steps ago". So `o1[t]` would mean
"the value in output stream number 1 at the current time-step". Likewise, for 
input stream variables like `i1[t]`. It means "the input in the input stream
1 at the current time-step". Input streams can also have an offset in order to
speak about past inputs. For example `i2[t-3]` means "the input in the input
stream 2 three time-steps ago". For further detail about streams, please refer 
to section [Variables](#variables).

In all above cases, `t` is a free variable and refers to the current time at
each point in time. The key point now is that an `always` statement will
quantify all scoped `t` universally, while a `sometimes` statement will quantify
them existentially. For example the specification `always o1[t] = 0` says that
at all time-steps the output stream number 1 will write `0`. Similarly, the
specification `sometimes o1[t] = 0` says that there exists a time-step at which
the output stream 1 will write `0`. When executing a Tau specification, the first
time-step is always 0. 

Formally, the grammar for Tau specifications is
```
spec => local_spec | always local_spec | sometimes local_spec
      | (spec && spec) | (spec || spec) | !spec
```
where `local_spec` is a formula defined by the rules:

```
local_spec => (local_spec "&&" local_spec)
            | "!" local_spec | (local_spec "^" local_spec)
            | (local_spec "||" local_spec) | (local_spec "->" local_spec)
            | (local_spec "<->" local_spec) | (local_spec "?" local_spec ":" local_spec)
            | (term "=" term) | (term "!=" term) | (term "<" term)
            | (term "!<" term) | (term "<=" term) | (term "!<=" term)
            | (term ">" term) | (term "!>" term)
            | "all" variable local_spec | "ex" variable local_spec
            | predicate | T | F
```
The naming conventions for `variable` are discussed in [Variables](#variables).
Furthermore, `term` is discussed in the next section [Boolean functions](#boolean-functions).
The `predicate` non-terminal in the above grammar describes how
to add predicate definitions directly into a formula. See the subsection
[Functions and predicates](#functions-and-predicates) for the
grammar definition of `predicate`.
In the REPL ([The Tau REPL](#the-tau-repl)) they
can be provided as explained in subsection
[Functions, predicates and input/output stream variables](#functions-predicates-and-inputoutput-stream-variables).

The precedence of the logical operators/quantifiers is as follows (from higher
precedence to lower):
`!` > `&&` > `^` > `||` > `<->` > `<-` > `->` > `ex ... ...` > `all ... ...` >
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

## **Boolean functions**

One of the key ingredients of the Tau Language are Boolean functions build from
Boolean combinations of variables, streams and constants over some fixed chosen atomless (or
finite -to be developed-) Boolean algebra. In particular, each Boolean
function has a unique type being this chosen Boolean algebra.
They are given by the following grammar:

```
term => (term "&" term) | term "'"
      | (term "+" term) | (term "|" term)
      | function | constant | uninterpreted_constant
      | variable | stream_variable | "0" | "1"

```

where

* `term` stands for a well-formed subformula representing a Boolean function and the operators `&`, `'`,
`+` (or equivalently `^`) and `|` respectively stand for conjunction, negation, exclusive-or
and disjunction,
* `function` is the non-terminal symbol used to incorporate function definitions (see the Subsection
[Functions and Predicates](#functions-and-predicates)),
* `constant` stands for an element from an available Boolean algebra. The type of the constant
determines the type of the Boolean function (see Subsection [Constants](#constants) for details).
* `uninterpreted_constant` stands for an uninterpreted constant from the fixed Boolean
algebra, which can be thought of as a variable being existentially quantified from the outside of
the formula. Its syntax is as follows:

```
uninterpreted_constant => "<" [name] ":" name ">"
```

* `variable` is a variable over the fixed Boolean algebra (see Subsection
[Variables](#variables) for details), 
* `stream_variable` represents an input or output 
stream. The type of a stream also determines the type of the Boolean function (see also subsection [Variables](#variables)) and
* `0` and `1` stand for the bottom and top element in the fixed Boolean
algebra.

In this case, the order of the operations is the following (from higher precedence
to lower): `'` > `&` > `+` (equivalently > `^`) > `|`.

If no type information is present within a Boolean function, it is assumed to be of the 
general type `countable atomless Boolean algebra`. Since all such Boolean algebras are
isomorphic, no particular Boolean algebra is chosen.

For example, the following is a valid Boolean function:

```
(x & y | (z ^ 0))
```

where `x`, `y` and `z` are variables.

## **Functions and predicates**

Another key concept in the Tau Language are functions and predicates. They are given
by the following grammar where `function_def` defines the syntax for a function
and `predicate_def` defines the syntax for a predicate:

```
function_def      => function ":=" term
function          => name "[" index+  "]" "(" [ variable ("," variable)* ] ")"
predicate_def     => predicate ":=" spec
predícate         => name "[" index+  "]" "(" [ variable ("," variable)* ] ")"
```

where `name` is the name of the function or predicate (it has to be a sequence of
letters and numbers starting by a letter) and `index` is a positive integer or
a variable or a variable minus a positive integer, i.e.:

```
index => number | variable | variable "-" number
```

Examples of functions and predicates are:

```
g[0](Y) := 1.
g[n](Y) := g[n - 1](Y).
```

or also

```
g[0](Y) := T.
g[n](Y) := h[n - 1](Y).
h[0](Y) := F.
h[n](Y) := g[n - 1](Y).
```

## **Constants**

Constants in the Tau Language are elements of the underlying Boolean algebras,
usually other than `0` and `1` that have a dedicated syntax.

In the REPL, we support two Boolean algebras: the Tau Boolean algebra and the
simple Boolean function algebra. The Tau Boolean algebra is an extensional Boolean
algebra that encodes Tau specifications over base algebras (in the REPL case we
only support the simple Boolean functions as base one).

Thus, in general the syntax for constants is the following:

```
constant => "{" (spec | term) "}" [":" base_boolean_algebra_type]
```

where `base_boolean_algebra_type` is given by:

```
base_boolean_algebra_type => "tau" | "sbf"
```

i.e. we could have a Tau formula seen as a Boolean algebra element (you can omit
the type, as `tau` is the default type). For example, the following is a valid
constant in the Tau Boolean algebra:

```
{ ex x ex y ex z (x & y | z) = 0 }:tau
```

or even

```
{ { ex x ex y ex z (x & y | z) = 0 }:tau = 0 }:tau
```

where `x`, `y` and `z` are variables.

Regarding the simple Boolean function algebra, the syntax is the following:

```
constant => "{" term "}" ":" "sbf".
```

A constant in the simple Boolean function algebra is for example:

```
{ (x & y | z) }:sbf
```

where `x`, `y` and `z` are variables.

## **Variables**

Variables range over Boolean algebra elements. In the REPL you can work with
open formulas (i.e. when variables are not quantified), but a specification
makes sense only for closed formulas. Their syntax depends
on whether the `charvar` option is enabled or not. If it is enabled, the syntax is
a single character followed by digits. Otherwise, the syntax is an
arbitrary string of `chars`.

We also have IO variables, which are actually infinite sequence of Boolean
algebra elements, each, indexed by positions in the sequence. They are used to
define the inputs and outputs of the program. The name for an input variable is
`i{num}` (e.g. `i1`) whereas output variables are of the form `o{num}` (in the
near future we will allow arbitrary names for IO variables). They should always
be referred to with reference to time (i.e. position in the sequence), and
the syntax is `i2[t]` or `o1[t]` where `t` always denotes time, but also can be
`i1[t-1]` or `o2[t-3]` (always a constant lookback).

As commented later on, IO variables need to be defined before the spec is run.
For example, the following is a valid definition of IO variables:

```
tau i1 = console.
tau o1 = console.
```

where `tau` points to the type of the variables (in this case, tau formulas) and
`console` stands for the input/output stream of the variable (in this case, the
console).

In the near future we will allow arbitrary names for the IO variables.

Finally, we have the uninterpreted constant context, which are implicitly
existentially quantified variables. The syntax is `<:name>`.

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
Note that in order to interpret the output, `1` of type `tau` is represented as `T` and `O` as `F`.
Furthermore, Tau specifications during execution are always displayed normalized.
The REPL informs the user whenever an update was done successfully by printing the new, updated specification.

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
`sometimes` parts by `st_spec`.
The updated specification is given by <br> 
`update && ( (ex [outputs] update && aw_spec) -> aw_spec)`, where `[outputs]` refers to the list of all output streams present 
in `aw_spec` and `update` combined. The meaning is that whenever possible given the current input at a particular step, 
`update` and `aw_spec` are executed together. The name _pointwise revision_ originates from this behavior.
Note that it is possible to refine the definition of the new specification
in more advanced ways. We will explore this aspect in the future.
Note that if `update` is satisfiable, then the updated specification is satisfiable.
3) As a final step, it is checked if the previous `sometimes` statements `st_spec` are executable along the updated specification.
If this is the case, they are added to the updated specification. Otherwise, 
the updated specification constructed in step 2 is accepted.

Note that in the step after an update was successfully applied, the new specification starts
running as if it was started at time step 0 shifted to the correct time step to match the overall
history. This means, in particular, that streams with lookback `k` only become solvable once the 
specification has continued for at least `k` steps. For example, updating a specification in 
step `s` with `o1[t] = i1[t-1]` means that in the next step `o1[s+1]` is unspecified. 
To see this, assume we start at step 0. Then `o1[t] = i1[t-1]` will leave `o1[0]` unspecified
since `i1[-1]` is not defined. We do not allow to define negative time steps in general. The only
exception is during pointwise revision in order to allow access to previous stream values,
as explained in step 1 above.

## **Reserved symbols**

Tau Language has a set of reserved symbols that cannot be used as identifiers.
In particular, we insist that `T` and `F` are reserved for true and false values
respectively in tau formulas and `0` and `1` stand for the corresponding Boolean
algebra elements.

# **Command line interface**

The general form of tau executable command line is:

```bash
tau [ options ] [ <specification> ]
```

where `[ options ]` are the command line options and `[ <specification> ]` is the Tau
specification you want to run. If you omit the tau specification, the Tau REPL will be
started.

The general options are the following:

| Option             | Description                                           |
|--------------------|-------------------------------------------------------|
| -h, --help         | detailed information about options                    |
| -l, --license      | show the license                                      |
| -v, --version      | show the version of the executable                    |
|--------------------|-------------------------------------------------------|
| -V, --charvar      | charvar (enabled by default)                          |
| -S, --severity     | severity level (trace/debug/info/error)               |
| -I, --indenting    | indenting of the formulas                             |
| -H, --highlighting | syntax highlighting                                   |

whereas the REPL specific options are:

| Options            | Description                                 |
|--------------------|---------------------------------------------|
| -e, --evaluate     | REPL command to be evaluated                |
| -s, --status       | display status                              |
| -c, --color        | use colors                                  |
| -d, --debug        | debug mode                                  |

# **The Tau REPL**

The Tau REPL is a command line interface that allows you to interact with the Tau
Language. It is a simple and easy to use tool that allows you to write and
execute Tau specifications on the go.

## **Basic REPL commands**

The Tau REPL provides a set of basic commands that allow you to obtain help,
version information, exit the REPL, clear the screen and so on. The syntax of
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

* `rec_relation`: defines a function or predicate supporting the usage of
recurrence relations. See the Tau Language Section for more information.

* `<type> i<number> = console | ifile(<filename>)`: defines an input stream variable.
The input variable can read values from the console or from a provided file.
`<type>` can be either `tau` or `sbf` (simple Boolean function) at the moment.

* `<type> o<number> = console | ofile(<filename>)`: defines an output stream variable.
The output variable can write values to the console or into a file.
<type>` can be either `tau` or `sbf` (simple Boolean function) at the moment.

## **Memory related commands**

All the results are stored in the REPL memory. You can also store well-formed Tau
formulas or Boolean functions for later reference. To do so, you can use the following
syntax:

* `tau|term`: store a tau formula or a Boolean function in the REPL memory.

If you want to consult the REPL memory contents, you can use the following commands:

* `history|hist`: show all the previously stored Tau expressions.

* `history|hist <repl_memory>`: show the Tau expression at the specified REPL memory position.

In general, to retrieve a Tau expression from the REPL memory, you can use the following
syntax:

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

* `snf <repl_memory|tau|term>`: computes the strong normal form of the given expression.

* `onf <var> <repl_memory|tau>`: computes the order normal form of the given
expression with respect to the given variable.

## **Specification execution**

Finally, you can run the given specification once you have defined the IO
variables as you need. The syntax of the commands is:

* `run|r <repl_memory|tau>`: runs the given specification.

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

# **Future work**

* Add support for redefinition of recurrence relations.
* Add support for arbitrary names for IO variables.
* Improve the performance of normalization of Boolean functions.

# **Submitting issues**

Submit issues at the following link: [Tau Language issues](https://github.com/IDNI/tau-lang/issues).

# **License**

Tau Language is licensed under the following terms:
[Tau Language License](https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt)

# **Authors**

The Tau Language has been developed by the following authors:

* Ohad Asor
* David Castro Esteban
* Tomáš Klapka
* Lucca Tiemens
