TAU Implementation introito

* Everything is a part of a (static) DAG:
	- The parse tree obtained from Earley parser is converted into a suitable 
	  DAG representation.
	- The state of the computation is represented by a set of nodes in the DAG.
	- The changes on the state of the computation are directed by rules (also 
		encoded as nodes in the DAG) wich specifies how new nodes are produced 
		(or reused).
	  from older ones.

* The technical ingredients are:
	- The rewriting engine:
		- take care of traversals of the DAG to apply the rules: postorder.
		- provides basic for the creation (or reuse) of nodes.
		- provides basic query operations on the DAG from a given node: select,
		find,...
	- The traversal API:
		- provides a DSL for traversing the DAG: as the underlying DAG is 
			constructed from the results of the parser, the API provides an easy 
			way	to traverse the DAG in a way that is coherent with the 
			nonterminal symbols.
		- also provides extractors to get the element of interest: non-terminal,
			terminal, ba element,...
	- The bindings API:
		- provides a way to bind constants from the underlying BA into tau.
		- several options are at hand: using a map from names to constants of the
			given algebras, using the type system,...
	- The type system:
		- provides a way to bind constants to the program, in this case providing
			an string to be parser by a factory that produce the acutal values 
			for the constants.
	- The callbacks:
		- provide the way to call the operations of the underlying BA from the 
			tau program via rules.

* To ease the development and future changes, there is an extensive collection
of tests that check the correctness of the implementation.
