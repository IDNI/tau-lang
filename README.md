![The TAU logo](/docs/tau_logo.png)

# Table of contents

1. [Introduction](#introduction)
2. [Quick start](#quick-start)
	1. [Installing the Tau REPL](#installing-the-tau-repl)
		1. [Linux users](#linux-users)
		2. [Windows users](#windows-users)
	2. [Compiling the source code](#compiling-the-source-code)
	3. [Running Tau REPL](#running-tau-repl)
		1. [Command line options](#command-line-options)
5. [The Tau Language](#the-tau-language)
	1. [Boolean functions](#boolean-functions)
	2. [Constants](#constants)
	3. [Variables](#variables-variables-variables)
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

# Quick start

## Installing the Tau REPL

### Linux users (NOT YET AVAILABLE)

We provide convenient packages for Linux users to install the Tau REPL. Currently,
we automatically build packages (AMD64 architecture) for the following package managers:

* deb (Debian/Ubuntu): [tau_0.1.0_amd64.deb](http://someurl.com)
* rpm (Fedora): [tau-0.1.0-1.x86_64.rpm](http://someurl.com)

Those packages include the tau executable.

### Windows users (NOT YET AVAILABLE)

For windows we provide a convenient installer that includes the tau executable
and also a zip file:

* Installer: [tau-0.1.0-amd64.exe](http://someurl.com)
* Zip file: [tau-0.1.0-amd64.zip](http://someurl.com)

## Compiling the source code

To compile the source code you need a recent C++ compiler, say GCC 13.1.0
compiler suite, as the source code uses recent features introduced in C++23
standard. You also need at least a cmake version 3.22.1 installed in your system.
Only a recent version of boost libraries are required to compile the source code.

The first step to compile the code is to clone the repository:

```bash
git clone https://github.com/IDNI/tau-lang.git
```

After that, you could run several bash scripts that are provided in the main
directory to compile the source code in release or debug mode (also in release
with debug information -used for benchmarking-).

The first script is `release.sh` which compiles the source code in release mode
and also all the tests. The second script is `debug.sh` which compiles the
source code in debug mode and also all the tests. The third one is
`relwithdebinfo.sh` which compiles the source code in release mode with debug
information, also all the tests and few benchmarks.

```bash
./release.sh # Compiles the source code in release mode
./debug.sh # Compiles the source code in debug mode
./relwithdebinfo.sh # Compiles the source code in release mode with debug information
```

We also provide scripts to execute the tests suite so you could check that
everything is working properly. The first one is `test_release.sh` to run the
tests in release mode and the second one is `test_debug.sh` to run the test
in debug mode (also `test_relwithdebinfo.sh`).

```bash
./test_release.sh # Runs the tests in release mode
./test_debug.sh # Runs the tests in debug mode
./test_relwithdebinfo.sh # Runs the tests in release mode with debug information
```

If you want to produce the documentation of the source code you need to install
the `doxygen` tool. After that, you could run the `release.sh` or `debug.sh`
(also the `relwithdebinfo.sh`) scripts as follows to produce the documentation
of the source code:

```bash
 # Compiles the source code in release mode and also the documentation
./release.sh -DBUILD_DOC=ON
# Compiles the source code in debug mode and also the documentation
./debug.sh -DBUILD_DOC=ON
# Compiles the source code in release mode with debug information and also the documentation
./relwithdebinfo.sh -DBUILD_DOC=ON
```

Please take a look to the main
([CMakeLists.txt](https://github.com/IDNI/tau-lang/blob/main/CMakeLists.txt))
for additional options.

## Running Tau REPL

Once you have compiled the source code you could run the `tau` executable to
execute Tau programs. The `tau` executable is located in the `build-Release`
or `build-Debug` directory (alternatively in `build-RelWithDebInfo`).

### Command line options

The following command line interface options are available:

| Option         | Description                                               |
|----------------|-----------------------------------------------------------|
| -h, --help     | detailed information about options                        |
| -p, --program  | program to run                                            |
| -e, --evaluate | program to be evaluated (alternative to -p)               |
| -l, --license  | show the license (NOT YET AVAILABLE)                      |
| -i, --input    | program's input                                           |
| -o, --output   | program's output                                          |
| -v, --charvar  | charvar (enabled by default)                              |

Among the possible inputs and outputs, the following are predefined:

| Input/Output | Description        |
|--------------|--------------------|
| @stdin, -    | standard input     |
| @stdout      | standard output    |
| @null        | null output        |

### Simple examples of usage

TODO (HIGH) add several simple examples of usage

# The Tau Language

## Boolean functions

One of the key ingredients of the Tau Language are the Boolean functions (Boolean
combinations of constants -over a Boolean algebra- and variables). They
are given by the following grammar:

```
bf -> "("bf "&" bf")" | bf "'" | "("bf "^" bf")" | "("bf "+" bf")" | "("bf "|" bf")"
	| bf_ref | constant | uninterpreted_constant | var | "0" | "1".
```

where

* `bf` stands for a well formed sub-formula and the operators `&`, `'`,
`^` and `|` stand for conjunction, negation, exclusive-or and disjunction
(respectively).
* `bf_ref` is  a reference to a recurrence relation (see the Subsection
[Recurrence relations](#recurrence-relations)),
* `constant` stands for an element of the Boolean algebras (see Subsction
[Constants](#constants) for details),
* `uninterpreted_constant` stands for an uninterpreted constant of the Boolean
algebra, they are assume to be existentialy quantified in the context of the
formula. The syntax is a follows:

```
uninterpreted_constant -> "<:" name ">".
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
constant -> "{" tau "}" [":" "tau"].
```

i.e. we could have a tau formula as a constant inside a tau formula as a constant.
For example, the following is a valid constant in the Tau Boolean algebra:

```
{ (x & y | z) = 0 }:tau
```

or even

```
{ { (x & y | z) = 0 }:tau = 0 }:tau
```

where `x`, `y` and `z` are variables.


Regarding the simple Boolean function algebra, the syntax is the following:

```
constant -> "{" sbf "}" ":" "sbf".
```

where the grammar for simple Boolean functions is the following:

```
sbf -> "("sbf "&" sbf")" | sbf "'" | "("sbf "^" sbf")" | "("sbf "+" sbf")"
	| "("sbf "|" sbf")"	| var | "0" | "1".
```

where `sbf` stands for a simple Boolean function, and the operators `&`, `'`,
(`^`|`+ `) and `|` stand for conjunction, negation, exclusive-or and disjunction;
`var` stands for a variable on the simple Boolean algebra, and finally, `0` and
`1` stands for the given elements in the simple Boolean algebra.

## Variables


TODO (HIGH) add a detailed explanation of variables, charvar options, etc.

## Tau formulas

However, nothing is that simple in usual programming languages, we have
decisions,... In the case of Tau Language, well formed formulas deal with
that. They provide us an extra logical layer on basic
computations (given by Boolean formulas) allowing us to use conditional
and similar constructions.

Well formed formulas are given in Tau Language by the following grammar:

```
tau -> "(" tau "&&" tau ")" | "!" tau | "(" tau "^" tau ")" | "(" tau "||" tau ")"
	| "(" tau "->" tau ")" | "(" tau "<->" tau ")" | "(" tau "?" tau ":" tau ")"
	| "(" bf "=" bf ")" | "(" bf "!=" bf ")" | "("bf "<" bf")" | "("bf "!<" bf")"
	| "(" bf "<=" bf ")" | "(" bf "!<=" bf ")" | "(" bf ">" bf ")"
	| "(" bf "!>" bf ")" | "all" var tau | "ex" var tau | tau_ref | T | F.
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

TODO (HIGH) review this section

Last, in usual programming languages you have loops, recursion and so on. In the
case of Tau Language, recursive relations are used to this end. Obviously, they
could not go as far as the usual programming languages (we will fall into the
undecidable side), but they are enough to express the most complex specifications.

They are given by the following grammar:

```
bf_rec_relation -> bf_ref ":=" bf.
bf_ref -> sym "[" (offset)+  "]" "(" variable+ ")".
```

in the case of Boolean functions and in the case of well-formed general formulas,
the grammar is:

```
tau_rec_relation -> tau_ref "::=" tau.
tau_ref -> sym "[" (offset)+  "]" "(" variable+ ")".
```

anf finally, in the case of tau formulas, the grammar is:

```
tau_rec_relation -> tau_ref ":::=" tau.
tau_ref -> sym "[" (offset)+  "]" "(" variable+ ")".
```

where `bf_rec_relation` stands for a Boolean function recursive relation, `bf_ref`
stands for a reference to a Boolean function (see Boolean functions Section),
`tau_rec_relation` stands for a well formed formula recursive relation, `tau_ref`
stands for a reference to a well formed formula (see well formed formulas
Section), `sym` stands for a symbol, `offset` stands for an offset and `capture`
stands for a capture/variable.

Examples of recursive relations are:

```
g[0](Y) := 1.
g[n](Y) := g[n - 1](Y).
```

for the case of Boolean functions,

```
g[0](Y) ::= T.
g[n](Y) ::= h[n - 1](Y).
h[0](Y) ::= F.
h[n](Y) ::= g[n - 1](Y).
```

and finally, for the case of well formed formulas:

```
g[0](Y) :::= {T}.
g[n](Y) :::= g[n - 1](Y).
```

## Tau specifications

TODO (HIGH) review this section

Taking into account all the previous definitions and considerations, Tau programs
are given by the following grammar:

```
rr => (rec_relation)* tau.
```

where `tau_rec_relation` stands for a tau recursive relations, `tau_rec_relation`
for a tau recursive relation, `bf_rec_relation` for a bf recursive relation and
`tau` stands for a tau formula.

Thus, they are a collection of Tau recursive relations and a main formula, p.e.:

```
g[0](Y) :::= {T}.
g[n](Y) :::= g[n - 1](Y).
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
includes the definitions of the recursive relations and the IO variables.

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

* `tau|bf`: store a tau formula or a Boolean function in memory.

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

* `subst|s <memory|tau|bf> [<memory|tau|bf>/<memory|tau|bf>]`: substitutes a
memory, well-formed formula or Boolean function by another one in the given
expression (beeing this one a memory position, well-formed formula or Boolean
Function).

* `instantiate|inst|i <memory|tau> [<var>/<memory|bf>]`: instantiates a variable
by a memory position, well-formed formula or Boolean function in the given
well-formed or Boolean function expression.

* `instantiate|inst|i <memory|bf> [<var>/<memory|bf>]`: instantiates a variable
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

* `normalize|n <memory|rr|ref|tau|bf>`: normalizes the given expression. See
the TABA book for details.

* `qelim <memory|tau>`: performs quantifier elimination on the given expression.

## Normal forms

Also, the Tau REPL includes several transformations procedures to standard forms.
The syntax of the commands as follows:

* `dnf <memory|tau|bf>`: computes the disjunctive normal form of the given
expression.

* `cnf <memory|tau|bf>`: computes the conjunctive normal form of the given
expression.

* `nnf <memory|tau|bf>`: computes the negation normal form of the given expression.

* `mnf <memory|tau|bf>`: computes the minterm normal form of the given expression.

* `snf <memory|tau|bf>`: computes the strong normal form of the given expression.

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
