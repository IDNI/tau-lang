![The TAU logo](/docs/images/tau_logo.png)

| Build type | Status                                                                                     |
|------------|--------------------------------------------------------------------------------------------|
| Release    | ![Release Build](https://github.com/IDNI/tau-lang/actions/workflows/release.yml/badge.svg) |
| Debug      | ![Debug Build](https://github.com/IDNI/tau-lang/actions/workflows/debug.yml/badge.svg)     |

# Table of contents

1. [Introduction](#introduction)
2. [Installing the Tau Farmework](#installing-the-tau-framework)
	1. [Linux](#linux)
	2. [Windows](#windows-users)
	3. [MacOS (not available yet)](#macos-users)
	2. [Compiling the source code](#compiling-the-source-code)
3. [Quick start](#quick-start)
4. [The Tau Language](#the-tau-language)
	1. [Constants](#constants)
	2. [Variables](#variables-variables-variables)
	3. [Boolean functions](#boolean-functions)
	4. [Tau formulas](#tau-formulas)
	5. [Functions and predicates](#functions-and-predicates)
	6. [Tau specifications](#tau-specifications)
	7. [Reserved symbols](#reserved-symbols)
5. [Command line options](#command-line-options)
6. [The Tau REPL](#the-tau-repl)
	1. [Basic REPL commands](#basic-repl-commands)
	2. [REPL options](#repl-options)
	3. [Functions, predicates and input/output stream variables](#functions-predicates-and-inputoutput-stream-variables)
	4. [Memory related commands](#memory-related-commands)
	5. [Expression manipulation](#expression-manipulation)
	6. [Logical procedures](#logical-procedures)
	7. [Normal forms](#normal-forms)
	8. [Program execution](#program-execution)
7. [Known issues](#known-issues)
8. [Future work](#future-work)
9. [Submitting issues](#submitting-issues)
10. [License](#license)
11. [Authors](#authors)


# **Introduction**

Tau Language is about enabling you to create software that elegantly adapts to
meet your requirements in a fully formal, correct-by-construction manner.

Tau Language is uniquely vastly expressive while retaining decidability. Further,
it has the unique ability to specify programs that refer to Tau specifications
and to logical relations between them. And, as you'll see, offers
many other advantages when compared to other common formal languages and
programming languages.

Tau Language offers a future where whether you're a developer, end-user, or
stakeholder, Tau-built software will be able to accurately adapt to be what
you, or a group of users, want it to be.

More precisely, the Tau Language is a logical software specification language.
It allows you to write constraints about software, check for satisfiability, and
run a representative program that meets those specifications. Put it in different
words: imagine programming by writing only tests, or assertions. Tau backend
will figure out automatically a program that will make the tests and assertions
pass. Tau is based on the first-order theory of atomless Boolean algebras. All
common logical tasks oevr the language are decidable.

For a more detailed explanation of the theory behind the Tau Language, please
refer to:

* GS Paper [Guarded Successor: A Novel Temporal Logic by Ohad Asor](https://web3.arxiv.org/abs/2407.06214)
* TABA book [Theories and Applications of Boolean Algebras by Ohad Asor](./docs/Theories-and-Applications-of-Boolean-Algebras-0.25.pdf) (In works).
* Youtube lecture series on [Atomless Boolean Algebra by Ohad Asor](https://www.youtube.com/watch?v=lNU1wS5rqtg&list=PLav2klOnTUlOeakJCbLZxoib_x0jYAQ5f&index=2).


This README.md is structured in the following way: first we provide a detailed
explanation of the Tau Language, including the syntax and semantics of the
language. Then we provide a quick start guide to start using the Tau Language.
Finally, we provide a guide to install the Tau Framework in your system, how to
use the command line interface and the Tau REPL (Read-Eval-Print-Loop) that
allows you to interact with the Tau Language.

We also provide a list of known issues, future work and how to submit issues.

To skip straight to our quick start section click [Quick start](#quick-start).

# **Installing the Tau Framework**

## **Linux**

Currently we automatically build the following binaries packages (AMD64 architecture):

* deb (Debian/Ubuntu): [tau-0.7-Linux.deb](https://github.com/IDNI/tau-lang/releases/download/v0.7-alpha/tau-0.7-Linux.deb)
* rpm (Fedora): [tau-0.7-Linux.rpm](https://github.com/IDNI/tau-lang/releases/download/v0.7-alpha/tau-0.7-Linux.rpm)

The executable is installed in `/usr/bin/tau`.

## **Windows**

For windows we provide a convenient installer that includes the tau executable
and also a zip file:

* Installer: [tau-0.7-win64.exe](https://github.com/IDNI/tau-lang/releases/download/v0.7-alpha/tau-0.7-win64.exe)
* Zip file: [tau-0.7-win64.zip](https://github.com/IDNI/tau-lang/releases/download/v0.7-alpha/tau-0.7-win64.zip)

## **MacOS (not available yet)**

A macOS installer will be available in the future.

## **Compiling the source code**

To compile the source code you need a recent C++ compiler supporting C++23, e.g.
GCC 13.1.0. You also need at least a cmake version 3.22.1 installed in your system.
The only code dependency is libboost.

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
execute Tau programs. The `tau` executable is located in either `build-Release`
or `build-Debug` or `build-RelWithDebInfo`.



# **Quick start**

To start using the Tau Language, download the latest release from the
[GitHub page](https://github.com/IDNI/tau-lang/releases/tag/v0.7-alpha). Once
you have downloaded and installed the executable (see the Section
[Installing the Tau Framework](#installing-the-tau-framework)), you can run
it from the command line by typing `tau`.

The programming model underlying the Tau Language is fully declarative. You
specify, possibly only very implicitly, how the current and previous inputs and
outputs are related, at each point of time. So what you write in the Tau Language
is not a program, but a specification, or spec, which represents all programs that
meet the specification. Once you run a specification, you actually run one
automatically-chosen representative program from that set.

In the scope of the Tau Language, a program means that for all inputs, at each
point in time, exist outputs, that clearly do not depend on future inputs
("time-compatible"). Implied from this definition is that all programs run
indefinitely no matter what the inputs are.

For example, the following program:

```
o1[t] = 0
```

states that the output `o1` at all time points (`t`) has to be `0`. Similarly the
following program:

```
o1[t] = i1[t]
```

states that the output `o1` at time `t` has to be the same as the input
`i1` at time `t`.
In the above examples, `o1` and `i1` are IO variables. They are used to define
the inputs and outputs of the specified programs and also declare their type.

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
one where it equals the input, those Tau specs define only one program each
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
output has to equal the input. Note the difference between Boolean and Logical
operators. The former are &|', and the latter are &&,||,!.

In order to simplify the process of writing and running Tau programs, we allow
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

In the Tau Language you define how the current and previous inputs and outputs
are related over time, using the first-order theory of atomless Boolean algebras
extended with a time dimension. For example you can write `o1[t] & o1[t-1] & i1[t] = 0`
which would mean that the current output, and the previous output, and the current input,
have to have an empty intersection. The set-theoretic perspective of Boolean algebra
is justfied by Stone's representation theorem for Boolean algebras, but more concretely,
when a Tau spec is treated as a BA element,
it can be seen as a set of all programs that admit that spec, and the Boolean
operations are simply the set-theoretic union/intersection/complementation.

## **Tau specifications**

At the top level, a Tau specification (we also say `spec`) is a collection of
"always" and "sometimes" statements applied to *local specifications*
(expressed by `tau`, see section [Tau formulas](#tau-formulas)),
combined by the logical
connectives *and*, *or* and *not*, denoted by `&&`, `||` and `!` respectively.
For example a well-formed Tau specification is
```
(always local_spec1) && (sometimes local_spec2)
```
where *local_spec1* and
*local_spec2* are formulas as in section [Tau formulas](#tau-formulas).
We say local specification because a formula `tau` can only talk about a fixed
(though arbitrary) point in time.

Recall from section [Variables](#variables) that there are input and output
stream variables. For example the output stream variable `o1[t-2]` means
"the value in output stream number 1 two time-steps ago". So `o1[t]` would mean
"the value in output stream number 1 at the current time-step". Likewise, there
are input stream variables like `i1[t]`. It means "the input in the input stream
1 at the current time-step". Input streams can also have an offset in order to
speak about past inputs. For example `i2[t-3]` means "the input in the input
stream 2 three time-steps ago". As explained in section [Variables](#variables),
input and output streams currently need to be defined before running a Tau
specification.

In all above cases, `t` is a free variable and refers to the current time at
each point in time. The key point now is that an `always` statement will
quantify all scoped `t` universally, while a `sometimes` statement will quantify
them existentially. For example the specification `always o1[t] = 0` says that
at all time-steps the output stream number 1 will write `0`. Similarly, the
specification `sometimes o1[t] = 0` says that there exists a time-step at which
the output stream 1 will write `0`.

Formally, the grammar for Tau specifications is
```
spec         => tau | always tau | sometimes tau | spec && spec | spec || spec | !spec
rr           => (rec_relation)* spec.
rec_relation => tau_rec_relation | term_rec_relation
```
A Tau specification without a mentioning of "always" or "sometimes" is implicitly
assumed to be an "always" statement. The `rr` in the above grammar describes how
to add function and predicate definitions directly to the formula. In REPL they
can also be provided separately as explained in the Tau REPL subsection
[Functions, predicates and input/output stream variables](#functions-predicates-and-inputoutput-stream-variables).
See the subsection [Functions and predicates](#functions-and-predicates) for the
definitions of `tau_rec_relation` and `term_rec_relation`.

Note that instead of writing `always` and `sometimes` you can also use box `[]`
and diamond `<>`, respectively.

Using this notation, a slightly bigger example of a Tau spec would be
```
    ([] o1[t] i1[t] = 0 && (i1[t] != 1 -> o1[t] != 0)) && (<> o1[t] = i1[t]')
```
which reads: at each point of time, the output should be disjoint from the input.
If the input is not 1, then the output is not zero. And, at least once during
execution, the output equals the complement of the input.

## **Tau formulas**

In traditional programming languages, we have
decisions,... In the case of Tau Language, well formed formulas deal with
that. They provide us an extra logical layer on basic
computations (given by Boolean formulas) allowing us to use conditional
and similar constructions.

Well formed formulas are given in Tau Language by the following grammar:

```
tau => "(" tau "&&" tau ")" | "!" tau | "(" tau "^" tau ")" | "(" tau "||" tau ")"
	| "(" tau "->" tau ")" | "(" tau "<->" tau ")" | "(" tau "?" tau ":" tau ")"
	| "(" term "=" term ")" | "(" term "!=" term ")" | "("term "<" term")"
	| "("term "!<" term")"	| "(" term "<=" term ")" | "(" term "!<=" term ")"
	| "(" term ">" term ")"	| "(" term "!>" term ")" | "all" var tau
	| "ex" var tau | tau_ref | T | F.
```

where

* `tau` stands for a well formed sub-formula and the operators `&`, `!`, `^`,
`|`, `->`, `<->` and `?` stand for conjunction, negation, exclusive-or,
disjunction, implication, equivalence and conditional, in the usual sense,
(respectively).

* the operators `=`, `<`, `<=` and `>` stand for equality, less than, less or
equal than and greater than; the operators `!=`, `!<`, `!<=` and `!>` denote
their negations,

* `all` stands for the universal quantifier and `ex` for the existential one,

* `tau_ref` is  a reference to a predicate (see the Subsection
[Functions and predicates](#functions-and-predicates)), and finally,

* `T` and `F` represent the true and false values.

For example, the following is a valid well formed formula:

```
(x && y || z) = 0 -> (x = 0 ? y = 0 : z = 0)
```

where `x`, `y` and `z` are variables.

## **Boolean functions**

One of the key ingredients of the Tau Language are the Boolean functions
(Boolean combinations of variables, and constants over some chosen atomless (or
finite -to be develop-) Boolean algebra and variables).They are given by the
following grammar:

```
term => "("term "&" term")" | term "'" | "("term "+" term")" | "("term "|" term")"
	 | term_ref | constant | uninterpreted_constant | var | "0" | "1".
```

where

* `term` stands for a well formed sub-formula and the operators `&`, `'`,
`^` and `|` stand for conjunction, negation, exclusive-or and disjunction
(respectively).
* `term_ref` is  a call to the given recurrence relation (see the Subsection
[Functions and Predicates](#functions-and-predicates)),
* `constant` stands for an element of the Boolean algebras (see Subsection
[Constants](#constants) for details),
* `uninterpreted_constant` stands for an uninterpreted constant of the Boolean
algebra, they are assumed to be existentially quantified in the context of the
formula. The syntax is a follows:

```
uninterpreted_constant => "<:" name ">".
```

* `var` is a variable of type a Boolean algebra element (see Subsection
[Variables](#variables-variables-variables) for details), and
* finally, `0` and `1` stands for the given elements in the corresponding Boolean
algebra.

For example, the following is a valid expression in terms of a Boolean function:

```
(x & y | (z ^ 0))
```

where `x`, `y` and `z` are variables.

## **Functions and predicates**

Another key concept in the Tau Language are functions and predicates. They are given
by the following grammar where `term_rec_relation` defines the syntax for a function
and `tau_rec_relation` defines the syntax for a predicate:

```
term_rec_relation => term_ref ":=" term.
term_ref          => sym "[" (offset)+  "]" "(" variable+ ")".
tau_rec_relation  => tau_ref ":=" tau.
tau_ref           => sym "[" (offset)+  "]" "(" variable+ ")".
```

where `sym` is the name of the function or predicate (it has to be a sequence of
letters and numbers starting by a letter) and `offset` is a positive integer or
a variable.

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

The syntax for the first case, the Tau Boolean algebra, is the following:

```
constant => "{" tau "}" [":" "tau"].
```

i.e. we may have a Tau formula seen as a Boolean algebra element (you can omit
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
constant => "{" sbf "}" ":" "sbf".
```

where the grammar for simple Boolean functions is the following:

```
sbf => "("sbf "&" sbf")" | sbf "'" | "("sbf "^" sbf")" | "("sbf "+" sbf")"
	| "("sbf "|" sbf")"	| var | "0" | "1".
```

where `sbf` stands for a simple Boolean function, and the operators `&`, `'`,
(`^`|`+`) and `|` stand for conjunction, negation, exclusive-or and disjunction;
`var` stands for a variable of type Boolean algebra element, and finally, `0` and
`1` stand for the given elements in the simple Boolean algebra.

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
`i{num}` (e.g. `i1`) whereas ouput variables are of the form `o{num}` (in the
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

## **Reserved symbols**

Tau Language has a set of reserved symbols that cannot be used as identifiers.
In particular, we insist that `T` and `F` are reserved for true and false values
respectively in tau formulas and `0` and `1` stand for the corresponding Boolean
algebra elements.

# **Command line interface**

The general form of tau executable command line is:

```bash
tau [ options ] [ <program> ]
```

where `[ options ]` are the command line options and `[ <program> ]` is the Tau
program you want to run. If you omit the tau program, the Tau REPL will be
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
execute Tau programs on the go.

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

* `definitions|defs`: shows all the definitions of the current program. That
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
expression (beeing this one a memory position, well-formed formula or Boolean
function).

* `instantiate|inst|i <repl_memory|tau> [<var>/<repl_memory|term>]`: instantiates a variable
by a memory position, well-formed formula or Boolean function in the given
well-formed or Boolean function expression.

* `instantiate|inst|i <repl_memory|term> [<var>/<repl_memory|term>]`: instantiates a variable
by a memory position or Boolean function in the given expression.

## **Logical procedures**

The Tau REPL also provides a set of logical procedures that allow you to check
several aspects of the given program/well-formed formulas/Boolean functions.
The syntax of the commands is the following:

* `valid <repl_memory|tau>`: checks if the given program is valid.

* `sat <repl_memory|tau>`: checks if the given program is satisfiable.

* `unsat <repl_memory|tau>`: checks if the given program is unsatisfiable.

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

## **Program execution**

Finally, you can run the given program once you have defined the IO
variables as you need. The syntax of the commands is:

* `run|r <repl_memory|tau>`: runs the given program.

# **Known issues**

This is a short list of known issues that will be fixed in a subsequent release:

* Issue in Fixed Point Calculations
* Incorrect type inference of IO variables in certain cases.
* Normalization:
  * Error in DNF/CNF conversions in the normalizer
* “Sometimes” keyword issues:
  * The correctness of the satisfiability algorithm is still not fully verified
  when input variables appear under "sometimes".
  * Allow constants positions under "sometimes".
  * Redundant “Sometimes” statements are not detected.
* Simplification:
  * Simplification of Boolean equations may take longer time in a few cases.
  * Path simplification algorithm does not take equalities between variables
  into account leading to later blow ups.
* Minor errors in windows REPL

# **Future work**

* Add support for redefinition of recurrence relations.

* Add support for arbitrary names for IO variables.

* Improve the performance of normalization of Boolean functions.

# **Submitting issues**

Like any other open-source project on GitHub, you can submit issues using the
following link: [Tau Language issues](https://github.com/IDNI/tau-lang/issues).

# **License**

Tau Language is licensed under the following terms:
[Tau Language License](https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt)

# **Authors**

The Tau Language has been developed by the following authors:

* Ohad Asor
* David Castro Esteban
* Tomáš Klapka
* Lucca Tiemens
