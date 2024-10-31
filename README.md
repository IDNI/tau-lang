![The TAU logo](/docs/tau_logo.png)

TODO (MEDIUM) add executor related info: splitter, !<= for bf, ...

TODO (HIGH) more structured and to the point

** This repo is under active development and you could expect daily new features,
improvements and fixes. **

Tau advances formal methods by removing coding from the process, while expanding
its industrial capability, reliability and ease of maintenance. Tau enables you
to automatically create the most complex, provably correct software, by simply
writing sentences about what you want the software to do.

Tau Language is able to embed and extend the most powerful decidable knowledge
representation languages, logics, and Boolean algebras to describe states in
the specification.

This README.md file only tries to give you a brief operative overview of Tau
Language so you could understand the main points without being too precise
(actually deliberately imprecise just for the sake of ideas exposition). For a
more detailed explanation of Tau Language and all its details, please refer to
the TABA book
([Theories and Applications of Boolean Algebras by Ohad Ashor](./docs/taba.pdf)).

# Compiling the source code

To compile the source code you need a recent C++ compiler, say GCC 13.1.0
compiler suite, as the source code uses recent features introduced in C++23
standard. You also need at least a cmake version 3.22.1 installed in your system.
Only a recent version of boost libraries are required to compile the source code.

## Basic compilation

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

## Additional options

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

# The Tau REPL

The Tau REPL is a command line interface that allows you to interact with Tau
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

## REPLE options

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

## Recurrence relations and input/output variables

As in other programming languages, you could define functions (recurrence
relations in Tau Language) and input/output variables. The syntax of the commands
is the following:

* `definitions|defs`: shows all the definitions of the current program. That
includes the definitions of the recursive relations and the input/output variables.

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

* `wff|bf`: store a well-formed formula or a Boolean function in memory.

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

* `subst|s <memory|wff|bf> [<memory|wff|bf>/<memory|wff|bf>]`: substitutes a
memory, well-formed formula or Boolean function by another one in the given
expression (beeing this one a memory position, well-formed formula or Boolean
Function).

* `instantiate|inst|i <memory|wff> [<var>/<memory|bf>]`: instantiates a variable
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

* `solve <memory|wff>`: solves the given system of equations given by the
well-formed formula. It only computes one solution.

* `normalize|n <memory|rr|ref|wff|bf>`: normalizes the given expression. See
the TABA book for details.

* `qelim <memory|wff>`: performs quantifier elimination on the given expression.

## Normal forms

Also, the Tau REPL includes several transformations procedures to standard forms.
The syntax of the commands as follows:

* `dnf <memory|wff|bf>`: computes the disjunctive normal form of the given
expression.

* `cnf <memory|wff|bf>`: computes the conjunctive normal form of the given
expression.

* `nnf <memory|wff|bf>`: computes the negation normal form of the given expression.

* `mnf <memory|wff|bf>`: computes the minterm normal form of the given expression.

* `snf <memory|wff|bf>`: computes the strong normal form of the given expression.

* `onf <var> <memory|wff>`: computes the order normal form of the given
expression with respect to the given variable.

## Program execution

Finally, you could run the given program once you have defined input/output
variables as you need. The syntax of the commands is the

* `run|r <memory|wff>`: runs the given program.

# The Tau Language

## Boolean functions

In usual programming languages, constants usually came in the form of integers,
strings,... in the case of Tau Language they came in the form of constants in
Boolean algebras.

Almost everyone related to Computer Science is familiar with the classical
Boolean algebra given by'0' and '1'. But Tau Language goes beyond that simple
notion of Boolean Algebra and allows you to consider more elaborated ones as
the Lindenbaum-Tarski algebra of a given logic (formulas of a given logic
considering conjunction, negation, exclusive or,...or for example equality
-which would stand for equivalence in the given logic-), finite Boolean
algebras (which would stand for algebras over finite bit representations),
define your own Boolean Algebras and so on. If your input/output Boolean algebras
are decidable, your are safe, Tau Language extension would also be decidable.

Taking into account the previous considerations, Tau Language allows you to
define Boolean functions in a clear and concise way. A Boolean function is a
Boolean combination of constants and variables.

If you follow the analogy with the usual programming languages, and you consider
integer inputs and outputs, Boolean functions would correspond to evaluate
polynomials over the integers.

In Tau Language we could represent Boolean functions essentially following
the grammar:

```
bf -> elem | "("bf "&" bf")" | bf "'" | "("bf "^" bf")" | "("bf "+" bf")" | "("bf "|" bf")"
	| fall var bf | fex var bf
elem -> var | const | bf_ref
const -> 0 | 1 | { B }
```

where `elem` stands for an element of one of the Boolean algebras, `bf` for a
Boolean functions, `var`stands for a variable, `const` stands for a constant,
`bf_ref` stands for a reference to a Boolean function recursive relation
(see recursive relations Section), `B` stands for a Boolean constant, `fall` is
the universal functional quantifier and `fex` is the existential functional
quantifier. As usual, the operators `&`, `'`, `^` and `|` stands for conjunction,
negation, exclusive-or and disjunction respectively.

For example, the following is a valid expression in terms of Boolean function:

```
(X & Y | (Z ^ 0))
```

where `X`, `Y` and `Z` are variables.

## Well-formed formulas

However, nothing is that simple in usual programming languages, we have
decisions,... In the case of Tau Language, well formed formulas deal with
that. They provide us an extra logical layer on basic
computations (given by Boolean formulas) allowing us to use conditional
and similar constructions.

Well formed formulas are given in Tau Language by the following grammar:

```
wff -> "("wff "&&" wff")" | "!" wff | "("wff "^" wff")" | "("wff "||" wff")" | "("wff "->" wff")"
	| "("wff "<->" wff")" | "("wff "?" wff ":" wff")" | all var wff | ex var wff
	| ball bool_var wff | bex bool_var wff | wff_ref
	| "("bf "=" bf")" | "("bf "!=" bf")" | "("bf "<" bf")" | "("bf "<=" bf")" | "("bf ">" bf")" | T | F.
```

where `wff` stands for a sub-well formed formula, `wff_ref` stands for a
reference to a well formed recursive relation (see recursive relations Section),
`T` stands for the true constant and `F` stands for the false constant. As usual,
the operators `&`, `!`, `^`,  `|`, `->`, `<->` and `?` stands for conjunction,
negation, exclusive-or, disjunction, implication, equivalence and conditional
(in the usual sense) respectively.

Also, `all` stands for the universal quantifier and `ex` for the existential one
 whereas `ball` stands for Boolean universal quantifier and `bex` stands for
 Boolean existential quantifier. The difference between the Boolean and the
 regular quantifiers is that the Boolean ones depend on a Boolean variable,
 i.e. a variable whose values are exactly `T` of `F`. The use of such quantifiers
 is to simplify the writing of complex formulas.

Finally, the operators `=`, `!=`, `<`, `<=` and `>` stands for equality,
inequality, less than, less or equal than and greater than respectively.

Finally, `=` stands for equality, `!=` for inequality, `<` for less than,
`<=` for less or equal than, `>` for greater than (all of them in one of the
underlying Boolean algebras) and `T` for true and `F` for false.

For example, the following is a valid well formed formula:

```
(X && Y || ((Z ^ 0) = 0)) -> (X ? Y : Z)
```

where `X`, `Y` and `Z` are variables.

As in the case of `bf`, we have two types of quantifiers and the variables could
be of the above types. As announced, we would clarify both of them in a
forthcoming Section.

## Variables, variables, variables,...

Tau specifications, as deal with program specifications, could freely include
input and output variables. The distinction between input and output variables
is made by the use of the `i_` and `o_` prefix. For instance, in a Tau
specification we could have variables named `ì_keyboard` or `o_terminal`.
Moreover, the variables must have a timestamp, i.e. you could have
`i_keyboard[t]` that denotes the value of the variable `i_keyboard` at time `t`.
Also you could have `i_keyboard[t-1]` that denotes the value of the variable
`i_keyboard` at time `t-1` or `ì_keyboard[3]` that denotes the value of variable
at time `3` of the specification.

Free variables used in recursive relations must be preceded by a `$` symbol,
for example `$X` or `$Y`. The use of `$` symbol is to avoid name clashes with
the variables of the specification. They could be used freely inside the body
of the definitions.

## Tau formulas

Tau formulas are given by the following grammar:

```
tau => "{" wff "}" | tau_ref | "("tau "&&&" tau")" | "("tau "|||" tau")" | "!!!" tau.
```

where `tau` stands for a tau formula, `tau_wff` stands for a well formed formula,
`tau_ref` stands for a reference to a well formed formula, `tau_and` stands for
a conjunction of two tau formulas, `tau_or` stands for a disjunction of two tau
 formulas, `tau_neg` stands for a negation of a tau formula and `capture` stands
  for a capture/variable.

## Recursive relations

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
wff_rec_relation -> wff_ref "::=" wff.
wff_ref -> sym "[" (offset)+  "]" "(" variable+ ")".
```

anf finally, in the case of tau formulas, the grammar is:

```
tau_rec_relation -> tau_ref ":::=" tau.
tau_ref -> sym "[" (offset)+  "]" "(" variable+ ")".
```

where `bf_rec_relation` stands for a Boolean function recursive relation, `bf_ref`
stands for a reference to a Boolean function (see Boolean functions Section),
`wff_rec_relation` stands for a well formed formula recursive relation, `wff_ref`
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

## Tau programs

TODO (HIGH) explain what is a specification

Taking into account all the previous definitions and considerations, Tau programs
are given by the following grammar:

```
rr => (rec_relation)* wff.
```

where `tau_rec_relation` stands for a tau recursive relations, `wff_rec_relation`
for a wff recursive relation, `bf_rec_relation` for a bf recursive relation and
`tau` stands for a tau formula.

Thus, they are a collection of Tau recursive relations and a main formula, p.e.:

```
g[0](Y) :::= {T}.
g[n](Y) :::= g[n - 1](Y).
g[1](Y);
```

TODO (HIGH) add tau recursive relations

## Type resolution

Type resolution is used to ensure that all the Boolean Algebra operations are
consistent. Types could be specified at two levels:

* At the Boolean algebra expression level: in this case, the type of the
expression is the type of one of the constants
of the Boolean expression, there is no need to provide the type in each of them.

* At the quantifier expression level: in this case, the type of a variable is
specified and all the underlying Boolean algebra expressions are typed accordingly.

In both cases, if there is a mismatch between the types, an error is raised
during the type resolution phase, i.e. before executing the program.

## Reserved symbols

Tau Language has a set of reserved symbols that cannot be used as identifiers.
In particular, `T` and `F` are reserved for true and false values respectively
in well formed formulas and `0` and `1` stand for the corresponding Boolean
algebra in Boolean function formulas.
