![The TAU logo](/docs/tau_logo.svg)

Tau advances formal methods by removing coding from the process, while expanding its industrial capability, reliability and ease of maintenance. Tau enables you to automatically create the most complex, provably correct software, by simply writing sentences about what you want the software to do.

Tau Language is able to embed and extend the most powerful decidable knowledge representation languages, logics, and Boolean algebras to describe states in the specification. Tau Language is able to embed and extend the most powerful decidable knowledge representation languages and logics to describe states in the Tau specification.

This Document deal with general remarks to take into account during the development and testing of the Tau Language and the Tau Language API.

# General remarks

Trought all the code we would use the following tags to denote different types of tasks or notes. We use TODO, DOING, IDEA, FIXME, REVIEW, DOCUMENTATION and MARK. Also, we could give a  priority to each task by adding HIGH, LOW or MEDIUM between parenthesis.

We use the subtags as follows:
	- (IMPORTANT) for main tasks of the project,
	- (HIGH) for tasks that should be done asap as they are important missing functionallity or important improvements,
	- (MEDIUM) for tasks that should be done soon as they are but they are not missing functionallity or important improvements, mostly related to code quality and testing,
	- (LOW) for tasks that should be done at some point but they are not important, mostly related to code quality and testing but not as important as MEDIUM tasks,
	- (VERY LOW) for tasks that should be done at some point but they are not important, mostly elated to code quality and testing but not as important as LOW tasks,

# Development and testing of Tau Language

Check the `README.md`for details about compiling and running the Tau Language (in both Debug and Release mode).

# Standalone executables

We provide a set of standalone executables that are able to parse, normalize using a different sets of BAs. The standalone executables are able to work with BDDs and BA of Tau over BBDs.

## Normalizer (BDDs and BA of Tau over BBDs)

This normalizer is a standalone executable that takes a NSO formula with recurrence relations, over the BDDs and over the TAu BA over BDSs, and normalizes it. The name of the executable is
`runner-normalizer-tau_over_bdd`. It only takes one argument, the path to the file containing the NSO formula with recurrence ralations to normalize, p.e.:

```bash
runner-normalizer-tau_over_bdd /path/to/file
```

## Normalizer (BDDs)

This normalizer is a standalone executable that takes a NSO formula with recurrence relations, over the BDDs, and normalizes it. The name of the executable is `runner-normalizer-bdd`. It only takes one argument, the path to the file containing the NSO formula with recurrence ralations to normalize, p.e.:

```bash
runner-normalizer-bdd /path/to/file
```

# Next steps (Tau Alpha Release)

- [x] (IMPORTANT) Satisfability
	- [x] (IMPORTANT) Basic implementations
	- [x] (IMPORTANT) Testing
- [ ] (IMPORTANT) Execution
	- [ ] (IMPORTANT) Basic implementations
	- [ ] (IMPORTANT) Testing
- [x] (IMPORTANT) CLI interface
	- [x] (IMPORTANT) Basic implementations
	- [x] (IMPORTANT) Pretty printing of the formulas
- [ ] (IMPORTANT) Basic documentation
	- [x] (IMPORTANT) README.md
	- [x] (IMPORTANT) DEVELOPMENT.md
	- [ ] (LOW) Tau Language API documentation
		- [ ] (LOW) Rewriter
		- [ ] (LOW) Nso
		- [ ] (LOW) Tau
		- [ ] (LOW) Normalizer
		- [ ] (LOW) Satisfability
		- [ ] (LOW) Executor
		- [ ] (LOW) CLI
	- [ ] (VERY LOW) CONTRIBUTING.md
	- [ ] (VERY LOW) CODE_OF_CONDUCT.md
	- [ ] (VERY LOW) LICENSE
- [ ] (IMPORTANT) Error and runtime error handling
	- [ ] (IMPORTANT) Parsing errors reporting
	- [ ] (IMPORTANT) Semantic errors reporting
		- [ ] (IMPORTANT) No captures in Tau source
		- [ ] (IMPORTANT) Type checkings
	- [ ] (IMPORTANT) Runtime errors reporting
	- [ ] (IMPORTANT) Internal errors reporting
- [x] (IMPORTANT) Basic logics
	- [x] (IMPORTANT) Propositional Calculus
- [x] (IMPORTANT) Tau BA
	- [x] (IMPORTANT) Definition and operations
	- [x] (IMPORTANT) Basic Testing of operators
- [ ] (IMPORTANT) Semantic checks
	- [ ] (HIGH) No capture allowed in Tau source
	- [ ] (MEDIUM) Type checkings
- [ ] (IMPORTANT) Technical improvements
	- [ ] (MEDIUM) Declare a static const lambda instead of a predicates if possible
	- [ ] (MEDIUM) Make the parsers singleton
	- [ ] (VERY LOW) Split code into h/cpp files
- [ ] (MEDIUM) Grammar improvements
	- [ ] (HIGH) Remove '(' and ')' from grammar
	- [ ] (MEDIUM) Improve typing syntax
	- [ ] (LOW) Collapse the grammar avoiding something -> "whatever"

# Future steps (Tau Blockchain support)

- [ ] (IMPORTANT) Tau Language
	- [ ] (IMPORTANT) Add exists unique for dealing with blockchain state
- [ ] (IMPORTANT) Blockchain support
	- [ ] (IMPORTANT) Finite boolean algebras and arithmetic
- [ ] (IMPORTANT) Performance optimizations
	- [ ] (IMPORTANT) Vector optimizations
	- [ ] (IMPORTANT) Use Boost memory mapped files
	- [ ] (MEDIUM) Use Boost serialization or something else to only
			store serialized object in the blockchain
- [ ] (IMPORTANT) Memoization
	- [ ] (IMPORTANT) Memoization of the normalizer
		- [ ] (IMPORTANT) Normalizer
		- [ ] (IMPORTANT) Equivalence
	- [ ] (IMPORTANT) Memoization of the satisfability
		- [ ] (IMPORTANT) Satisfability
		- [ ] (IMPORTANT) Equivalence
	- [ ] (IMPORTANT) Memoization of the executor?
- [ ] (MEDIUM) Grammar improvements?
