![The TAU logo](/docs/tau_logo.svg)

TODO (HIGH) add executor related info: splitter, bf_not_less_equal, ...

Tau advances formal methods by removing coding from the process, while expanding its industrial capability, reliability and ease of maintenance. Tau enables you to automatically create the most complex, provably correct software, by simply writing sentences about what you want the software to do.

Tau Language is able to embed and extend the most powerful decidable knowledge representation languages, logics, and Boolean algebras to describe states in the specification. Tau Language is able to embed and extend the most powerful decidable knowledge representation languages and logics to describe states in the Tau specification.

# Compiling the source code

To compile the source code you need a recent C++ compiler, say GCC 13.1.0 compiler suite, as the source code use recent features introduced in C++23 standard. You also need at least a cmake version 3.22.1 installed in your system. No extra libraries are required to compile the source code.

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

Once you have compiled the source code you could run the `tau` executable to execute a Tau programs. The `tau` executable is located in the `build-Release` or `build-Debug` directory (depending in your build). You could run the `tau` executable as follows:

```bash
./tau tau_example.tau
```

where `tau_example.tau` is a Tau program.

There are a lot examples of Tau programs in the `examples/tau` directory.

# Understanding Tau language

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

## Boolean functions

In the Tau language we could represent boolean functions essentially following the grammar:

```
bf -> elem | "("bf "&" bf")" | "~" bf | "("bf "^" bf")" | "("bf "+" bf")" | "("bf "|" bf")"
	| fall var bf | fex var bf
elem -> var | const | bf_ref
const -> 0 | 1 | { B }
```

where `elem` stands for an element of one of the boolean algebras, `bf` for a boolean functions, `var`stands for a variable, `const` stands for a constant, `bf_ref` stands for a reference to a boolean function recursive relation (see recursive relations Section), `B` stands for a boolean constant, `fall` is the universal functional quantifier and `fex` is the existential functional quantifier. As usual, the operators `&`, `~`, `^` and `|` stands for conjunction, negation, exclusive-or and disjunction respectively.

For example, the following is a valid boolean function:

```
(X & Y | (Z ^ 0))
```

where `X`, `Y` and `Z` are variables.

## Functional quantifiers

As show in the `bf` grammar, we consider two types of `bf` qantifiers: the existential one `fex`and the universal one `fall`. Both depend in a variable and a `bf` formula. The formula could be whatever we derive from the previious grammar, whereas the varibles could be of the following types:

- regular variables, they are just a sequence of `chars` (no especial symbols). They stand for regular variables in a formula, like the ones in predicate logic, and
- captures, they are a sequence of `chars`preceed by a `$`. They behave in the same way as PROLOG variables.

Functional existential quantifiers get converted as follows:

```
fex x f(x, X) := f(0,X) | f(1, X).
```

whereas universal functional quantifiers get converted in the following way:
```
fall x f(x, X) := f(0,X) & f(1, X).
```

We would go further about the meaning of captures in subsequent sections, for the time being, just keep in mind that functional quatifiers depend on variables and they essentially quantify over the given boolean algebra.

## Well-formed formulas

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

Recursive ralations are introduced to simplify the writing of well formed formulas. They are given by the following grammar:

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

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

## Time-compatible formulas

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

## Satisfability of Tau programs

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

## Execution of Tau programs

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

# Extending the Tau language

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

## Basic extensions

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

### Implementing custom boolean algebras

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

### Named bindings

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

## Advanced extensions

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

### Source code bindings

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

### Boolean algebra element factories

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

### Using custom grammars

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

# Examples

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

# Contributing

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."
