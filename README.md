![The TAU logo](/docs/images/tau_logo.png)

| Build type | Status                                                                                     |
|------------|--------------------------------------------------------------------------------------------|
| Release    | ![Release Build](https://github.com/IDNI/tau-lang/actions/workflows/release.yml/badge.svg) |
| Debug      | ![Debug Build](https://github.com/IDNI/tau-lang/actions/workflows/debug.yml/badge.svg)     |

# Table of contents

1. [Introduction](#introduction)
2. [Quick start](#quick-start)
3. [Installing the Tau Farmework](#installing-the-tau-framework)
	1. [Linux (not available yet)](#linux)
	2. [Windows (not available yet)](#windows-users)
	3. [MacOS (not available yet)](#macos-users)
	2. [Compiling the source code](#compiling-the-source-code)
4. [Command line options](#command-line-options)
5. [The Tau Language](#the-tau-language)
	1. [Constants](#constants)
	2. [Variables](#variables-variables-variables)
	3. [Boolean functions](#boolean-functions)
	4. [Tau formulas](#tau-formulas)
	5. [Recurrence relations](#recurrence-relations)
	6. [Tau specifications](#tau-specifications)
	7. [Reserved symbols](#reserved-symbols)
6. [The Tau REPL](#the-tau-repl)
	1. [Basic REPL commands](#basic-repl-commands)
	2. [REPL options](#repl-options)
	3. [Recurrence relations and IO variables](#recurrence-relations-and-inputoutput-variables)
	4. [Memory related commands](#memory-related-commands)
	5. [Expression manipulation](#expression-manipulation)
	6. [Logical procedures](#logical-procedures)
	7. [Normal forms](#normal-forms)
	8. [Program execution](#program-execution)
7. [Known issues](#known-issues)
8. [Future work](#future-work)

# Introduction

The Tau language is a logical software specification language. It allows you to
write constraints about software, check for satisfiability, and run a candidate
program that meet those specifications. All related logical tasks are decidable.

The language is based on the first-order theory of atomless Boolean algebras.

For a more detailed explanation of the theory behind the Tau Language, please
refer to the TABA book ongoing draft
([Theories and Applications of Boolean Algebras by Ohad Asor](./docs/taba.pdf)).

This README.md in structured in the following way: first, we provide a quick
start guide to start using the Tau Language. Then, we provide a guide to install
the Tau Framework in your system. After that, we provide a detailed explanation
of the Tau Language, including the syntax and semantics of the language. Finally,
we provide a guide to use the Tau REPL, a command line interface that allows you
to interact with the Tau Language.

We also provide a list of known issues, future work and how to submit issues.

# Quick start

To start using the Tau Language you can download the latest release from the
[releases page](http://someurl.com). Once you have downloaded the executable
and installed accordingly to you system (see the Section
[Installing the Tau Framework](#installing-the-tau-framework)), you could run
it from the command line in your system.

The programming model underlying the Tau language is declarative, you specify how
the current and previous inputs and outputs are related over time. Of course, you
are given and specification and multiple programs could satisfy it. The Tau
framework will find one of them for you.

For example, you could write the following program:

```
o1[t] = 0
```

which states that the output `o1` at time `t` has to be `0`. Or also you could
write the following program:

```
o1[t] = i1[t]
```

which states that the output `o1` at time `t` has to be the same as the input
`i1` at time `t`.

In the above examples, `o1` and `i1` are IO variables. They are used to define
the inputs and outputs of the specification and also declare its type.

An example of how to define IO variables is the following:

```
tau i1 = console.
tau o1 = console.
```

In the above case we specify that `i1` and `o1` are of type `tau` and they take
values from the console (let say stdin for the input and stdout for the output).
You couls define as IO streams also files:

```
tau i1 = ifile("input.in").
tau o1 = ofile("output.out").
```

Of course you could consider more complicated specifications as:

```
o1[t] & o1[t-1] & i1[t] = 0
```

which states that the current output, and the previous output, and the current
input, have to be 0.

In order to simplify the process of writing and running Tau programs, we provide
also recursive relations. The following is a simple recurrence relation
definition that takes as values expressions in the Tau Language formulas:

```
f[0](y) := T.
f[n](y) := f[n - 1](y).
```

which you could use in your program as follows:

```
o1[t] = f[5](i1[t]).
```

Or also, you can use the following recurrence relation definition

```
g[0](y) := 0.
g[n](y) := g[n](y)'.
```

which takes values in expressions of the Tau Language Boolean functions and
alternates between 0 and 1 depending on the parity of n.

To get all the details about the Tau Language, please refer to the Section
[The Tau Language](#the-tau-language). There you could find all the details
about the syntax and semantics of the language.

# Installing the Tau Framework

## Linux (not available yet)

Currently we automatically build the following binaries packages (AMD64 architecture):

* deb (Debian/Ubuntu): [tau_0.1.0_amd64.deb](http://someurl.com)
* rpm (Fedora): [tau-0.1.0-1.x86_64.rpm](http://someurl.com)

The executable is installed in `/usr/bin/tau`.

## Windows (not available yet)

For windows we provide a convenient installer that includes the tau executable
and also a zip file:

* Installer: [tau-0.1.0-amd64.exe](http://someurl.com)
* Zip file: [tau-0.1.0-amd64.zip](http://someurl.com)

## MacOS (not available yet)

## Compiling the source code

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
execute Tau programs. The `tau` executable is located in the either `build-Release`
or `build-Debug` or `build-RelWithDebInfo`.

### Command line interface

The general form of tau executable command line is:

```bash
tau [ options ] [ <command> [ <command options> ] ]
```

The general options are the following:

| Option         | Description                                               |
|----------------|-----------------------------------------------------------|
| -h, --help     | detailed information about options                        |
| -l, --license  | show the license (not available yet)                      |
| -v, --version  | show the version of the executable (not available yet)    |

Regarding the commands and its options we have the following:

| Command        | Options         | Description                                 |
|----------------|-----------------|---------------------------------------------|
| run            |                 | run a Tau program (not available yet)       |
|                | -p, --program   | program to run                              |
|                | -e, --evaluate  | program to be evaluated (alternative to -p) |
|                | -i, --input     | program's input                             |
|                | -o, --output    | program's output                            |
|                | -v, --charvar   | charvar (enabled by default)                |
|                | -h, --help	   | detailed information about options          |
| repl           |                 | run the Tau REPL (default)                  |
|                | -v, --charvar   | charvar (enabled by default)                |
|                | -h, --help	   | detailed information about options          |
|                | -e, --evaluate  | repl command to be evaluated                |
| help		     |                 | show help                                   |

Among the possible inputs and outputs, the following are predefined:


TODO (HIGH) exemplify, to demonstrate what this means.


| Input/Output | Description        |
|--------------|--------------------|
| @stdin, -    | standard input     |
| @stdout      | standard output    |
| @null        | null output        |

### Simple examples of usage

TODO (HIGH) add several simple examples of usage

# The Tau Language

In the Tau language you define how the current and previous inputs and outputs
are related over time, using the first-order theory of atomless Boolean algebras
extended with a time dimension. For example you can write `o1[t] & o1[t-1] & i1[t] = 0`
which would mean that the current output, and the previous output, and the current input,
have to have an empty intersection. The set-theoretic perspective of Boolean algebra
is justfied by Stone's representation theorem for Boolean algebras, but more concretely,
when a Tau spec is treated as a BA element,
it can be seen as a set of all programs that admit that spec, and the Boolean
operations are simply the set-theoretic union/intersection/complementation.

## Constants

Constants in the Tau Language are elements of the underlying Boolean algebras,
usually other than `0` and `1` that have a dedicated syntax (but could also
represented as such).

In the REPL, we support two Boolean algebras: the Tau Boolean algebra and the
simple Boolean function algebra. The Tau Boolean algebra is an extensional Boolean
algebra that encodes Tau specifications over base algebras (in the REPL case we
only support the simple Boolean functions as base one).

The syntax for the first case, the Tau Boolean algebra, is the following:

```
constant => "{" tau "}" [":" "tau"].
```

i.e. we could have a tau formula as aelement of a base Boolean algebra and so on.
For example, the following is a valid constant in the Tau Boolean algebra:

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
(`^`|`+ `) and `|` stand for conjunction, negation, exclusive-or and disjunction;
`var` stands for a variable on the simple Boolean algebra, and finally, `0` and
`1` stands for the given elements in the simple Boolean algebra.

A simple example of a constant in the simple Boolean function algebra is the for
example:

```
{ (x & y | z) }:sbf
```

where `x`, `y` and `z` are variables.

## Variables

Regarding variables, we could distinguish between variables in Boolean functions,
IO variables and (somehow streching the concept) unconstrained context (which
are implicitly existentially quantified variables).

Variables in Boolean functions are just regular variables that could be used.
They could be quantified or not, depending on the context. Their syntax depends
on whether `charvar` option is enabled or not. If it is enabled, the syntax is
just a single character followed by digits. Otherwise, the syntax is just an
arbitrary string of `chars`.

Apart from that, we also have IO variables. They are used to define the inputs
and outputs of the program. The name for an input variable is `i{num}` whereas
ouput variables are of the form `o{num}` (in the near future we will allow
arbitrary names for IO variables). One particularity of IO variables is that
they take different values at different times. Thus, the are always refered to
with a time offset. The syntax is `i{num}[t]` or `o{num}[t]` where `t` is the
time offset, but also could be `i{num}[t-1]` or `o{num}[t-3]` (always a bounded
positive loopback).

As commented later on, IO variables need to be defined before the program is run.
For example, the following are a valid definition of IO variables:

```
tau i1 = console.
tau o1 = console.
```

where `tau` points to the type of the variables (in this case, tau formulas) and
`console` stands for the input/output stream of the variable (in this case, the
console).

In the near future we will allow arbitrary names for the IO variables.

Finally, we have the unconstrained constant context, which are implicitly
existentially quantified variables. The syntax is `<:name>`.

## Boolean functions

One of the key ingredients of the Tau Language are the Boolean functions (Boolean
combinations of variables, and constants over some chosen atomless (or finite, TBD)
Boolean algebra and variables).They
are given by the following grammar:

```
term => "("term "&" term")" | term "'" | "("term "+" term")" | "("term "|" term")"
	 | term_ref | constant | uninterpreted_constant | var | "0" | "1".
```

where

* `term` stands for a well formed sub-formula and the operators `&`, `'`,
`^` and `|` stand for conjunction, negation, exclusive-or and disjunction
(respectively).
* `term_ref` is  a reference to a recurrence relation (see the Subsection
[Recurrence relations](#recurrence-relations)),
* `constant` stands for an element of the Boolean algebras (see Subsction
[Constants](#constants) for details),
* `uninterpreted_constant` stands for an uninterpreted constant of the Boolean
algebra, they are assume to be existentialy quantified in the context of the
formula. The syntax is a follows:

```
uninterpreted_constant => "<:" name ">".
```

* `var` stands for a variable of the Boolean algebra (see Subsection
[Variables](#variables-variables-variables) for details), and
* finally, `0` and `1` stands for the given elements in the corresponding Boolean
algebra

For example, the following is a valid expression in terms of Boolean function:

```
(x & y | (z ^ 0))
```

where `x`, `y` and `z` are variables.

## Tau formulas

However, nothing is that simple in usual programming languages, we have
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

* the operators `=`, `<`, `<=` and `>` stands for equality, less than, less or
equal than and greater than; the operators `!=`, `!<`, `!<=` and `!>` denote
their negations,

* `all` stands for the universal quantifier and `ex` for the existential one,

* `tau_ref` is  a reference to a recurrence relation (see the Subsection
[Recurrence relations](#recurrence-relations)), and finally,

* `T` and `F` represent the true and false values.

For example, the following is a valid well formed formula:

```
(x && y || z) = 0 -> (x = 0 ? y = 0 : z = 0)
```

where `x`, `y` and `z` are variables.

## Recurrence relations

Another key concept in the Tau Language is  recurrence relations. They are given
by the following grammar:

```
term_rec_relation => term_ref ":=" term.
term_ref          => sym "[" (offset)+  "]" "(" variable+ ")".
tau_rec_relation  => tau_ref ":=" tau.
tau_ref           => sym "[" (offset)+  "]" "(" variable+ ")".
```

where `sym` is the name of the recurrence relation (it has to be a sequence of
letters and numbers starting by a letter) and `offset` is a positive integer or
a variable.

Examples of recurrence relations are:

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

## Tau specifications

Taking into account all the previous definitions and considerations, Tau programs
are given by the following grammar:

```
rr           => (rec_relation)* tau.
rec_relation => tau_rec_relation | term_rec_relation
```

Thus, they are a collection of recurrence relations and a main formula, p.e.:

```
g[0](Y) := {T}.
g[n](Y) := g[n - 1](Y).
g[1](Y);
```

## Reserved symbols

Tau Language has a set of reserved symbols that cannot be used as identifiers.
In particular, we insists that `T` and `F` are reserved for true and false values
respectively in tau formulas and `0` and `1` stand for the corresponding Boolean
algebra in Boolean function formulas.

# The Tau REPL

The Tau REPL is a command line interface that allows you to interact with the Tau
Language. It is a simple and easy to use tool that allows you to write and
execute Tau programs on the go.

## Basic REPL commands

The Tau REPL provides a set of basic commands that allow you to obtain help,
version information, exit the REPL, clear the screen and so on. The syntax of
the commands is the following:

* `help|h [<command>]`: shows a general help message or the help message of a
specific command.

* `version|v`: shows the version of the Tau REPL. The version of the Tau REPL
corresponds to the repo commit.

* `quit|q` or `exit`: exits the Tau REPL.

* `clear|c`: clears the screen.

## REPL options

You have several options at your disposal to configure the Tau REPL. In order
to set or get the value of an option you could use the following commands:

* `get [<option>]`: shows all configurable settings and their values or a single
one if its name is provided.

* `set <option> [=] <value>`: sets a configurable option to a desired value.

* `toggle <option>`: toggle an option between on/off.

The options you have at your disposal are the following:

* `c|color|colors`: Can be on/off. Controls usage of terminal colors in its
output. It's on by default.

* `s|status`: Can be on/off. Controls status visibility in the prompt. It's on
by default.

* `v|charvar`: Can be on/off. Controls usage of character variables in the REPL.

* `sev|severity`: Possible values are trace/debug/info/error. Value determines
how much information will REPL provide. This is set to error by default.

* `h|hilight|highlight`: Can be on/off. Controls usage of highlighting in the
output of commands. It's off by default.

* `i|indent|indentation`: Can be on/off. Controls usage of indentation in the
output of commands. It's on by default.

* `charvar|v`: Can be on/off. Controls usage of character variables in the
REPL. It's on by default.

* `d|dbg|debug`: Can be on/off. Controls debug mode. It's off by default.

## Recurrence relations and IO variables

As in other programming languages, you could define functions (recurrence
relations in Tau Language) and IO variables. The syntax of the commands
is the following:

* `definitions|defs`: shows all the definitions of the current program. That
includes the definitions of the recurrence relations and the IO variables.

* `definitions|defs <number>`: shows the definition of the given recurrence
relation.

* `rec_relation`: defines a recurrence relation. See the Tau Language Section
for more information.

* `<type> i<number> = console | ifile(<filename>)`: defines an input variable.
The input variable could take values from the console or from a file.

* `<type> o<number> = console | ofile(<filename>)`: defines an output variable.
The output variable could take values from the console or from a file.

## Memory related commands

All the results are stored in memory. Also you could stored well-formed formulas
or Boolean function for later reference. To do so, you could use the following
syntax:

* `tau|term`: store a tau formula or a Boolean function in memory.

If you want to consult the memory contents, you could use the following commands:

* `history|hist`: show all the previous commands ouputs.

* `history|hist <memory>`: show the output of the given memory position.

In general, to retrive a previous memory position you could use the following
syntax:

* `%<id>`: stands for the absolute memory position with the given id.

* `%-<relid>`: stands for the relative memory position with the given relative id.

## Expression manipulation

You could substitute expressions into other expressions or instantiate variables
in expressions. The syntax of the commands is the following:

* `subst|s <memory|tau|term> [<memory|tau|term>/<memory|tau|term>]`: substitutes a
memory, well-formed formula or Boolean function by another one in the given
expression (beeing this one a memory position, well-formed formula or Boolean
Function).

* `instantiate|inst|i <memory|tau> [<var>/<memory|term>]`: instantiates a variable
by a memory position, well-formed formula or Boolean function in the given
well-formed or Boolean function expression.

* `instantiate|inst|i <memory|term> [<var>/<memory|term>]`: instantiates a variable
by a memory position or Boolean function in the given expression.

## Logical procedures

The Tau REPL also provides a set of logical procedures that allow you to check
several aspects of the given program/well-formed formulas/Boolean functions.
The syntax of the commands is the following:

* `valid <rr>`: checks if the given program is valid.

* `sat <rr>`: checks if the given program is satisfiable.

* `unsat <rr>`: checks if the given program is unsatisfiable.

* `solve <memory|tau>`: solves the given system of equations given by the
well-formed formula. It only computes one solution.

* `normalize|n <memory|rr|ref|tau|term>`: normalizes the given expression. See
the TABA book for details.

* `qelim <memory|tau>`: performs quantifier elimination on the given expression.

## Normal forms

Also, the Tau REPL includes several transformations procedures to standard forms.
The syntax of the commands as follows:

* `dnf <memory|tau|term>`: computes the disjunctive normal form of the given
expression.

* `cnf <memory|tau|term>`: computes the conjunctive normal form of the given
expression.

* `nnf <memory|tau|term>`: computes the negation normal form of the given expression.

* `mnf <memory|tau|term>`: computes the minterm normal form of the given expression.

* `snf <memory|tau|term>`: computes the strong normal form of the given expression.

* `onf <var> <memory|tau>`: computes the order normal form of the given
expression with respect to the given variable.

## Program execution

Finally, you could run the given program once you have defined IO
variables as you need. The syntax of the commands is the

* `run|r <memory|tau>`: runs the given program.

# Known issues

TODO (HIGH) add a list of known issues

# Future work

TODO (HIGH) add a list of future work

# Submitting issues

As any other opensource project in GitHUb, you could submit issues in the
following link: [Tau Language issues](https://github.com/IDNI/tau-lang/issues).
