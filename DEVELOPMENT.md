![The TAU logo](/docs/tau_logo.svg)

Tau advances formal methods by removing coding from the process, while expanding its industrial capability, reliability and ease of maintenance. Tau enables you to automatically create the most complex, provably correct software, by simply writing sentences about what you want the software to do.

Tau Language is able to embed and extend the most powerful decidable knowledge representation languages, logics, and Boolean algebras to describe states in the specification.
Tau Language is able to embed and extend the most powerful decidable knowledge representation languages and logics to describe states in the Tau specification.

This Document deals with general remarks to take into account during the development and testing of the Tau Language and the Tau Language API.

# General remarks

Throughout all the code we would use the following tags to denote different types of tasks or notes. We use TODO, DOING, IDEA, FIXME, REVIEW, DOCUMENTATION and MARK. Also, we could give a priority to each task by adding HIGH, LOW or MEDIUM between parenthesis.

We use the subtags as follows:
	- (IMPORTANT) for main tasks of the project,
	- (HIGH) for tasks that should be done asap as they are important missing functionality or important improvements,
	- (MEDIUM) for tasks that should be done soon as they are but they are not missing functionality or important improvements, mostly related to code quality and testing,
	- (LOW) for tasks that should be done at some point but they are not important, mostly related to code quality and testing but not as important as MEDIUM tasks,
	- (VERY LOW) for tasks that should be done at some point but they are not important, mostly related to code quality and testing but not as important as LOW tasks.

# Development and testing of Tau Language

Check the `README.md`for details about compiling and running the Tau Language (in both Debug and Release mode).

# Standalone executables

We provide a set of standalone executables that are able to parse, normalize using different sets of BAs. The standalone executables are able to work with BDDs and BA of Tau over BBDs.

## Normalizer (BDDs and BA of Tau over BBDs)

This normalizer is a standalone executable that takes a NSO formula with recurrence relations, over the BDDs and over the Tau BA over BBDs, and normalizes it. The name of the executable is
`runner-normalizer-tau_over_bdd`. It only takes one argument, the path to the file containing the NSO formula with recurrence relations to normalize, p.e.:

```bash
runner-normalizer-tau_over_bdd /path/to/file
```

## Normalizer (BDDs)

This normalizer is a standalone executable that takes a NSO formula with recurrence relations, over the BDDs, and normalizes it. The name of the executable is `runner-normalizer-bdd`. It only takes one argument, the path to the file containing the NSO formula with recurrence relations to normalize, p.e.:

```bash
runner-normalizer-bdd /path/to/file
```
