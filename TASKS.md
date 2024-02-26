
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

# General improvements

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
	- [ ] (MEDIUM) Remove captures from the user input

# CLI improvements

- [ ] Subformula selection for wff
	- [ ] Convert selection to dnf
	- [ ] Convert selection to cnf
	- [ ] Convert selection to nnf
	- [ ] Convert selection to bdd
	- [ ] Convert selection to anf
	- [ ] Convert selection to minterm
- [ ] Eliminate selected quantifier(s) in standard way
- [ ] Eliminate selected quantifier(s) in differential way
- [ ] Eliminate selected quantifier(s) in minterm way
- [ ] Eliminate selected quantifier(s) in order way
- [ ] Replace all occurrences of a variable in the selection with proper formula
- [ ] Convert to order normal form wrt selected (single) var
- [ ] Subformula selection for bf
	- [ ] Convert selection to dnf
	- [ ] Convert selection to cnf
	- [ ] Convert selection to nnf
	- [ ] Convert selection to bdd
	- [ ] Convert selection to anf
	- [ ] Convert selection to minterm
- [ ] Subst for a var in the selection
- [ ] Subst for a func in the selection
- [ ] Auto-find expressions that make something zero
	- [ ] By x+f(x) (for univar)
	- [ ] By the LGRS (for multivar)
- [ ] Find an expression expressing a single solution
- [ ] Expand quantification over bf/sbf into first order quantification wrt a selected set of vars
- [ ] Strong normalization of selected subformulas

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
