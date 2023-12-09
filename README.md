"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

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

# Running TAU programs

Once you have compiled the source code you could run the `tau` executable to execute a TAU programs. The `tau` executable is located in the `build-Release` or `build-Debug` directory (depending in your build). You could run the `tau` executable as follows:

```bash
./tau tau_example.tau
```

where `tau_example.tau` is a TAU program.

There are a lot examples of TAU programs in the `examples/tau` directory.

# Understanding TAU language syntax

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

## Boolean functions

In the TAU language we could represent boolean functions essentially following the grammar:

```
bf -> elem | (bf "&" bf) | "~" bf | (bf "^" bf) | (bf "+" bf) | (bf "|" bf) | fall var bf | fex var bf
elem -> var | const | bf_ref
const -> 0 | 1 | { B }
```

where `elem` stands for an element of one of the boolean algebras, `bf` for a sub-boolean functions, `var`stands for a variable, `const` stands for a constant, `bf_ref` stands for a reference to a boolean function (see recursive relations Section), `B` stands for a boolean constant, `fall` is the universal functional quantifier and `fex` is the existential functional quantifier. As usual, the operators `&`, `~`, `^` and `|` stands for conjunction, negation, exclusive-or and disjunction respectively.

### Functional quantifiers

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

### Variables

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

## Well-formed formulas

Well formed formulas are given in the TAU language by the following grammar:

```
wff -> (wff "&" wff) | "!" wff | (wff "^" wff) | (wff "|" wff) | (wff "->" wff)
		| (wff "<->" wff) | (wff "?" wff ":" wff) | all var wff | ex var wff | wff_ref
		| (bf "=" bf) | (bf "!=" bf) | (bf "<" bf) | (bf "<=" bf) | (bf ">" bf) | T | F.
```

where `wff` stands for a sub-well formed formula, `wff_ref` stands for a reference to a well formed formula (see recursive relations Section), `T` stands for the true constant and `F` stands for the false constant. As usual, the operators `&`, `!`, `^`,  `|`, `->`,
`<->` and `?` stands for conjunction, negation, exclusive-or, disjunction, implication, equivalence and conditional (in the usual sense) respectively.

Also, `all` stands for the universal quantifier and `ex` for the existential one, whereas `=` stands for equality, `!=` for disequality, `<` for less than, `<=` for less or equal than, `>` for greater than (all of them in one of the underlying boolean algebras) and `T` for true and `F` for false.

## Recursive relations

Recursive ralations are introduced to simplify the writing of boolean functions and well formed formulas. They are given by the following grammar:

```
bf_rec_relation -> bf_ref ":=" bf.
bf_ref -> sym "[" (offset)+  "]" "(" capture+ ")".
```

and in the case of well-formed formulas, the grammar is:

```
wff_rec_relation -> wff_ref ":=" wff.
wff_ref -> sym "[" (offset)+  "]" "(" capture+ ")".
```

where `bf_rec_relation` stands for a boolean function recursive relation, `bf_ref` stands for a reference to a boolean function (see boolean functions Section), `wff_rec_relation` stands for a well formed formula recursive relation, `wff_ref` stands for a reference to a well formed formula (see well formed formulas Section), `sym` stands for a symbol, `offset` stands for an offset and `capture` stands for a capture/variable.


## TAU programs

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

# Understanding TAU language execution

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

## Time-compatible formulas

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

## Satisfability of TAU programs

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

## Execution of TAU programs

"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

# Extending the TAU language

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
