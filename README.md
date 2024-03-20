![The TAU logo](/docs/tau_logo.png)

TODO (MEDIUM) add executor related info: splitter, !<= for bf, ...

TODO (HIGH) more structured and to the point

** This repo is under active development and you cuold expect daily new features, improvements and fixes. **

Tau advances formal methods by removing coding from the process, while expanding its industrial capability, reliability and ease of maintenance. Tau enables you to automatically create the most complex, provably correct software, by simply writing sentences about what you want the software to do.

Tau Language is able to embed and extend the most powerful decidable knowledge representation languages, logics, and Boolean algebras to describe states in the specification. Tau Language is able to embed and extend the most powerful decidable knowledge representation languages and logics to describe states in the Tau specification.

This README.md file only tries to give you a brief operative overview of the Tau Language so you could understand the main points without being to precise (actually deliberatedly imprecise just for the sake of ideas exposition). For a more detailed explanation of the Tau Language and all its details, please refer to the TABA book ([Theories and Applications of Boolean Algebras by Ohad Ashor](./docs/taba.pdf)).

# Compiling the source code

To compile the source code you need a recent C++ compiler, say GCC 13.1.0 compiler suite, as the source code use recent features introduced in C++23 standard. You also need at least a cmake version 3.22.1 installed in your system. Only a recent version of boost libraries are required to compile the source code.

## Basic compilation

The first step to compile the code is to clone the repository:

```bash
git clone https://github.com/IDNI/tau-lang.git
```

After that, you could run several bash scripts that are provided in the main directory to compile the source code in release or debug mode.

The first script is `release.sh` which compiles the source code in release mode and also all the tests. The second script is `debug.sh` which compiles the source code in debug mode and also all the tests.

```bash
./release.sh # Compiles the source code in release mode
./debug.sh # Compiles the source code in debug mode
```

We also provide scripts to execute the tests suite so you could check that everything is working properly. The first one is `test_release.sh` to run the tests in release mode and the second one is `test_debug.sh` to run the tests in debug mode.

```bash
./test_release.sh # Runs the tests in release mode
./test_debug.sh # Runs the tests in debug mode
```

## Additional options

If you want to produce the documentation of the source code you need to install the `doxygen` tool. After that, you could run the `release.sh` or `debug.sh`script as follows to produce the documentation of the source code:

```bash
./release.sh -DBUILD_DOC=ON # Compiles the source code in release mode and also the documentation
./debug.sh -DBUILD_DOC=ON # Compiles the source code in debug mode and also the documentation
```

# Running Tau programs

Once you have compiled the source code you could run the `tau` executable to execute Tau programs. The `tau` executable is located in the `build-Release` or `build-Debug` directory (depending on your build).

# Tha Tau REPL

The Tau REPL is a command line interface that allows you to interact with the Tau language. It is a simple and easy to use tool that allows you to write and execute Tau programs on the go.

The commands provided by the Tau REPL are:

* `help [<command>]`: shows a general help message or the help message of a specific command. It has an abbreviated form `h`.
* `version`: shows the version of the Tau REPL. It has an abbreviated form `v`. The version of the Tau REPL corresponds to the repo commit.
* `quit` or `exit`: exits the Tau REPL. It has an abbreviated form `q` or `e`.
* `normalize <quoted_wff>`: normalizes a quoted well formed formula, p.e. `normalize '((X && Y) || T)'`. It has an abbreviated form `n`.
* `output [<id>]` or `&[<id>]`: manages outputs of previous commands. It has an abbreviated form `o`.
* `%[<relid>]`: manages outputs of previous commands, counting from the last.
* `get [<option>]`: shows all configurable settings and their values or a single one if its name is provided.
* `set <option> [=] <value>`: sets a configurable option to a desired value.
* `toggle <option>`: toggle an option between on/off.

Tau REPL options:

* `color`: Can be on/off. Controls usage of terminal colors in its output. It's on by default.
* `status`: Can be on/off. Controls status visibility in the prompt. It's on by default.
* `severity`: Possible values are trace/debug/info/error. Value determines how much information will REPL provide. This is set to error by default.

# Understanding Tau language

The Tau Language is a declarative language that allows you to specify the behavior of a program in a clear and concise way.

The syntax is the syntax is an extension of the syntax of the first order theory of boolean algebras. It can extend other languages in a decidable way if your starting point is decidable, otherwise it would inherit its character from the underlying one.

In the following Subsections we will explain the main ingredients of the Tau Language.

## Boolean functions

In usual progremming languages, constants usually came in the form of integers, strings,... in the case of the Tau Laguage they came in the form of constants in Boolean Algebras.

Almost everyone related to Computer Science is familiar with the classical Boolean algebra given by
'0' and '1'. But the Tau Language goes beyond that simple notion of Boolean Algebra and allows you to consider more elaboted ones as the Lindenbaum-Tarski algebra of a given logic (formulas of a given logic considering conjunction, negation, exclusive or,...or for example equality -which would stand for equivalence in the given logic-), finite Boolean algebras (which would stand for algebras over finite bit representations), define your own Boolean Algebras and so on. If your input/output Boolean Algebras are decideble, your are safe, Tau Language extension would also be decidable.

Taking into account the previous considerations, the Tau Language allows you to define Boolean functions in a clear and concise way. A Boolean function is a Boolean combination of constants and variables.

If you follow the analogy with the usual programming languages, and you consider integer inputs and outputs, Boolean functions would correspond to evaluate polynomials over the integers.

In the Tau language we could represent boolean functions essentially following the grammar:

```
bf -> elem | "("bf "&" bf")" | bf "'" | "("bf "^" bf")" | "("bf "+" bf")" | "("bf "|" bf")"
	| fall var bf | fex var bf
elem -> var | const | bf_ref
const -> 0 | 1 | { B }
```

where `elem` stands for an element of one of the boolean algebras, `bf` for a boolean functions, `var`stands for a variable, `const` stands for a constant, `bf_ref` stands for a reference to a boolean function recursive relation (see recursive relations Section), `B` stands for a boolean constant, `fall` is the universal functional quantifier and `fex` is the existential functional quantifier. As usual, the operators `&`, `'`, `^` and `|` stands for conjunction, negation, exclusive-or and disjunction respectively.

For example, the following is a valid expression in terms of Boolean function:

```
(X & Y | (Z ^ 0))
```

where `X`, `Y` and `Z` are variables.

## Functional quantifiers

Assuming that you are familiar with the concept of quantifiers in predicate logic, the concept of functional quantifiers is not so different. In the case of functional quantifiers we are quantifying over the given Boolean Algebra, i.e. we are considering the set of all the possible values of the given Boolean Algebra. So, existential functional quantifiers would stand for the existentce of an input in the Boolean algebra such that the Boolean function satifies something. Whereas in the case of a universal functional quantifier we are considering that for all possible inputs in the Boolean algebra something follows.

In general, eliminating such a quantifiers could be a hards task, and most of the times impossible. However, in the case of Boolean algebras, the elimination of such quantifiers is possible and it could be done in a decidable way if the underlying Boolean algebra is decidable.

The formula could be whatever we derive from the previious grammar, whereas the varibles could be of the following types:

- regular variables, they are just a sequence of `chars` (no especial symbols). They stand for regular variables in a formula, like the ones in predicate logic, and
- captures, they are a sequence of `chars`preceed by a `$`. They behave in the same way as PROLOG variables.

In order to eliminate the quantifiers, we could use the following rules:

* functional existential quantifiers get transformed as follows:

```
fex x f(x, X) := f(0,X) | f(1, X).
```

whereas universal functional quantifiers get transformed in the following way:
```
fall x f(x, X) := f(0,X) & f(1, X).
```

We would go further about the meaning of captures in subsequent sections, for the time being, just keep in mind that functional quatifiers depend on variables and they essentially quantify over the given Boolean algebra regarding a given Boolean function.

## Well-formed formulas

However, nothing is that simple in usual programming languages, we have decissions,... In the case of the Tau Language, well formed formulas deal with that. They provide us an extra logical layer on basic
computations (given by Boolean formulas) allowing us to use conditional and similar constructioons.

Well formed formulas are given in the Tau language by the following grammar:

```
wff -> "("wff "&&" wff")" | "!" wff | "("wff "^" wff")" | "("wff "||" wff")" | "("wff "->" wff")"
	| "("wff "<->" wff")" | "("wff "?" wff ":" wff")" | all var wff | ex var wff
	| ball bool_var wff | bex bool_var wff | wff_ref
	| "("bf "=" bf")" | "("bf "!=" bf")" | "("bf "<" bf")" | "("bf "<=" bf")" | "("bf ">" bf")" | T | F.
```

where `wff` stands for a sub-well formed formula, `wff_ref` stands for a reference to a well formed recursive relation (see recursive relations Section), `T` stands for the true constant and `F` stands for the false constant. As usual, the operators `&`, `!`, `^`,  `|`, `->`,
`<->` and `?` stands for conjunction, negation, exclusive-or, disjunction, implication, equivalence and conditional (in the usual sense) respectively.

Also, `all` stands for the universal quantifier and `ex` for the existential one, whereas `ball` stands for boolean universal quantifier and `bex` stands for boolean existential quantifier. The difference between the boolean and the regular quantifiers is that the boolean ones depend on a boolean variable, i.e. a variable whose values are exactly `T` of `F`. The use of such quantifiers is to simplify the writing of complex formulas.

Finally, the operators `=`, `!=`, `<`, `<=` and `>` stands for equality, inequality, less than, less or equal than and greater than respectively.

Finally, `=` stands for equality, `!=` for inequality, `<` for less than, `<=` for less or equal than, `>` for greater than (all of them in one of the underlying boolean algebras) and `T` for true and `F` for false.

For example, the following is a valid well formed formula:

```
(X && Y || ((Z ^ 0) = 0)) -> (X ? Y : Z)
```

where `X`, `Y` and `Z` are variables.

As in the caes of `bf`, we have two types of qantifiers and the varibles could be of the above types. As announced, we would clarify both of them in a forthcoming Section.

## Variables, variables, variables,...

Tau specifications, as deal with program specifications, could freely include input and output variables. The disctintion between input and output variables is made by the use of the `i_` and `o_` prefix. For instance, in a Tau specification we could have variables named `ì_keyboard` or `o_terminal`. Moreover, the variables must have a timestamp, i.e. you could have
`i_keyboard[t]` that denotes the value of the variable `i_keyboard` at time `t`. Also you could have `i_keyboard[t-1]` that denotes the value of the variable `i_keyboard` at time `t-1` or `ì_keyboard[3]` that denotes the value of variable at
time `3` of the specification.

Free variables used in recursive relations must be preceed by a `$` symbol, for example `$X` or `$Y`. The use of `$` symbol is to avoid name clashes with the variables of the specification. They could be used freely inside the body of the definitions.

## Tau formulas

Tau formulas are given by the following grammar:

```
tau => "{" wff "}" | tau_ref | "("tau "&&&" tau")" | "("tau "|||" tau")" | "!!!" tau.
```

where `tau` stands for a tau formula, `tau_wff` stands for a well formed formula, `tau_ref` stands for a reference to a well formed formula, `tau_and` stands for a conjunction of two tau formulas, `tau_or` stands for a disjunction of two tau formulas, `tau_neg` stands for a negation of a tau formula and `capture` stands for a capture/variable.

## Recursive relations

Last, in usual programming languages you have loops, recursion and so on. In the case of the Tau Language, recursive relations are used to this end. Obviously, they could not go as far as the usual programming languages (we will fall into the undecideble side), but they are enough to express the most complex specifications.

They are given by the following grammar:

```
bf_rec_relation -> bf_ref ":=" bf.
bf_ref -> sym "[" (offset)+  "]" "(" capture+ ")".
```

in the case of boolean functions and in the case of well-formed general formulas, the grammar is:

```
wff_rec_relation -> wff_ref "::=" wff.
wff_ref -> sym "[" (offset)+  "]" "(" capture+ ")".
```

anf finally, in the case of tau formulas, the grammar is:

```
tau_rec_relation -> tau_ref ":::=" tau.
tau_ref -> sym "[" (offset)+  "]" "(" capture+ ")".
```

where `bf_rec_relation` stands for a boolean function recursive relation, `bf_ref` stands for a reference to a boolean function (see boolean functions Section), `wff_rec_relation` stands for a well formed formula recursive relation, `wff_ref` stands for a reference to a well formed formula (see well formed formulas Section), `sym` stands for a symbol, `offset` stands for an offset and `capture` stands for a capture/variable.

Examples of recursive relations are:

```
g[0]($Y) := 1.
g[$n]($Y) := g[$n - 1]($Y).
```

for the case of boolean functions,

```
g[0]($Y) ::= T.
g[$n]($Y) ::= h[$n - 1]($Y).
h[0]($Y) ::= F.
h[$n]($Y) ::= g[$n - 1]($Y).
```

and finally, for the case of well formed formulas:

```
g[0]($Y) :::= {T}.
g[$n]($Y) :::= g[$n - 1]($Y).
```

## Tau programs

TODO (HIGH) explain what is a specification

Taking into account all the previous definitions and considerations, Tau programs are given by the following grammar:

```
tau_program => (tau_rec_relation)* tau.
```

where `tau_rec_relation` stands for a tau recursive relations and `tau` stands for a tau formula.

Thus, they are a collection of Tau recursive relations and a main formula, p.e.:

```
g[0]($Y) :::= {T}.
g[$n]($Y) :::= g[$n - 1]($Y).
g[1](Y);
```

TODO (HIGH) add tau recursive relations

## Type resolution

Type resolution is used to ensure that all the Boolean Algebra operations are consistente. Types could be specified at two levels:

* At the boolean algebra expression level: in this case, the type of the expression is the type of one of the constants
of the boolean expression, there is no need to provide the type in each of them.

* At the quantifier expression level: in this case, the type of a variable is specified and all the
underlying boolean algebra expressions are typed accordingly.

In both cases, if there is a missmatch between the types, an error is raised during the type resolution phase, i.e. before executing the program.

## Reserved symbols

Tau language has a set of reserved symbols that cannot be used as identifiers. In particular, `T` and `F` are reserved for true and false values respectively in well formed formulas and `0` and `1` stand for the corresponding Boolean Algebra in boolean function formulas.

# Understanding Tau language execution

"More to be added"

## Time-compatible formulas

"More to be added"

## Satisfability of Tau programs

"More to be added"

## Execution of Tau programs

"More to be added"

# Extending the Tau language

"More to be added"

## Basic extensions

"More to be added"

### Implementing custom boolean algebras

"More to be added"

### Named bindings

"More to be added"

## Advanced extensions

"More to be added"

### Source code bindings

"More to be added"

### Boolean algebra element factories

"More to be added"

### Using custom grammars

"More to be added"

# Examples

"More to be added"

# Contributing

"More to be added"
