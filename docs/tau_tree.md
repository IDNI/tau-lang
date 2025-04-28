
# Tau tree API

Tau tree API is available in `idni::tau_lang` namespace.

It provides `node<BAs...>` and `tree<node>` types.

It also contains `ba_constants<BAs...>` pool and `ba_constants_binder<BAs...>` named and factory binder



## struct `node<BAs...>`

`node` is templated by a pack of Boolean algebras.

`BAs...` pack is used for two purposes:
1. determine bitsize required for storing BA type ids
2. provide type aliases for packed types so they are accessible everywhere (mostly from templates templated directly with node and not with BAs pack)

Node stores data:
```
	size_t nt     : nt_bits;   - node::type enum id (bitsize is determined from number of nonterminals at constexpr)
	size_t term   : 1;         - flag if is term or formula
	size_t ba_id  : ba_bits;   - BA type id (bitsize is determined from number of BAs at constexpr)
	size_t ext    : 1;         - extension flag (not implemented yet)
	size_t data   : data_bits; - remaining bits for data
	size_t hash;               - hash value
```

Terminals are collapsed into their parent nonterminals, and non digital data is pooled.
In data field is either number, integer, or id of an object in its respective pool.
These are strings (or terminal sequences) and constants.

Strings are:
 -  stored in a pool by `size_t string_id(const std::string&)`
 - and retrieved by `const std::string& string_from_id(size_t)`

Constants, BA types and BA types map are managed by `ba_constants<BAs...> pool`.


### type aliases provided by `node`

`node` struct provides several type aliases.

When we are speaking about `tau_parser::nonterminal` in the parse tree, we provide `node::type` alias to be used in tau tree:
```
	using type                              = tau_parser::nonterminals;
```

`node` also provides several `BAs...` packed aliases. They can be used in constructs where the pack is not available directly, ie. in methods and functions templated by `node` and not by the `BAs...` pack:
```
	using node_t                            = node<BAs...>;
	using bas_variant                       = std::variant<BAs...>;
	using ba_constants_t                    = ba_constants<BAs...>;
	using ba_constants_binder_t             = ba_constants_binder<BAs...>;
	using ba_types_checker_and_propagator_t = ba_types_checker_and_propagator<BAs...>;
	using tau_ba_t                          = tau_ba<BAs...>;
```



## struct `tree<node>`

tree is templated by a `node<BAs...>` type.

It is based on `idni::lcrs_tree<node>` which is then based on `idni::bintree<node>`

It has three base data members:
```
	const node value;     // node 
	const tref l;         // first child
	const tref r;         // right sibling
```


### referencing, passing around, handling (preventing GC), and accessing API

There are four forms of the Tau tree used all around the codebase:

```
tref                          node_ref;            // for passing around and for building
htree::sp                     sp_node_handle;      // to prevent the node from being gc-ed
const tree<node<BAs...>>&     node_value_and_api;  // to access the value and tree API
tree<node<BAs...>>::traverser traverser_wrapper;   // traverser API with operator| and extractors
```


### Helper types and aliases used in implementation:

It is convenient to have type aliases in structs or functions to reduce templating and shorten the code:

When we got `<typename... BAs>`:

	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	using tt = tau::traverser;

When we got `<NodeType node>`:

	using tau = tree<node>;
	using tt = tau::traverser;

These aliases are also used in the following text.


#### node::type enum values on tree

Tau tree inherits the nonterminals enum from a generated tau_parser, so the types are available directly from the tree:

	tau::variable == node::type::variable == tau_parser::variable 



### Typical conversions:

1. I want to acces the node value, children or basically the whole tree API: `const tau&`
```
	- from tref:       tau::get(node_ref);
	- from htree::sp:  tau::get(sp_node_handle);
	- from tt:         traverser_wrapper | tt::Tree;   // using extractor
	                or traverser_wrapper.value_tree();
```
2. I want to prevent the node from being GC-ed: `htree::sp`
```
	- from tref:       tau::geth(node_ref);
	- from const tau&: tau::geth(node_value_and_api.get());
	- from:            traverser_wrapper | tt::handle; // using extractor
```

3. I want to pass node reference around (for API calls and caching): `tref`
```
	- from htree::sp:  sp_node_handle->get();
	- from const tau&: node_value_and_api.get();
	- from tt:         traverser_wrapper | tt::ref;    // using extractor
	                or traverser_wrapper.value();
```

4. I want to use traverser API: `tt`
```
	- from tref:       tt(node_ref);
	- from htree::sp:  tt(sp_node_handle);
	- from const tau&: tt(node_value_and_api);
	                or node_value_and_api();
```


### Tree equality (with regards to LCRS tree)

When comparing `tref == tref` you get structural equality with regards to right siblings because tree is a lcrs tree and comparing `tref` means the right siblings are also compared.

We usually need subtree equality, ie. to ignore right siblings.

For that you can use `bool tau::subtree_equals(tref, tref)`
Or subtree equality is also used when comparing tree nodes (`const tree<node<BAs...>>&`) directly.

```
	tref ref1, ref2;
	if (tau::subtree_equals(ref1, ref2)) ...

	// or
	const auto& t1 = tau::get(ref1), t2 = tau::get(ref2);
	if (t1 == t2) ...
```

Tree also provides `tau::subtree_less(tref, tref)` as subtree equality alternative to `operator<(tref, tref)`

When storing tref in containers (ie. for caching) we can use `tau::subtree_equality`.
Tree API provides set and map with subtree equality:
```
	using subtree_set = std::set<tref, subtree_equality>;
	using subtree_map = std::map<tref, tref, subtree_equality>;
```

Declaring:
```
	typename tau::subtree_map cache;
	typename tau::subtree_set visited;
```


### Tree updates (with regards to LCRS tree)

It is necessary to remember that when replacing a node manually we often need to keep the right sibling value. For that some tau::get() methods accepts `tref right_sibling` as the last argument.

post_order and pre_order non-const traversers keep the right sibling values, so you don't have to care about it when using them.

Example replacing just children of a node:
```
	tref n;                      // imagine we have a node with a right sibling
	tref child1, child2;         // two new children we want instead of the original children
	const auto& t = tau::get(n); // get access to the api

	// create a new node with children while passing the right sibling
	tau::get(t.value, { child1, child2 }, t.right_sibling());
	// or just use the base data member r
	tau::get(t.value, { child1, child2 }, t.r);
```


### Accessing the node value

Node value is stored in `tree<node>::value`.

And there are also helpers to access the value members directly:
```
	size_t data() const;
	size_t child_data() const;

	bool is(size_t nt) const;
	bool is_string() const;
	bool is_integer() const;
	bool is_num() const;
	bool is_ba_constant() const;
	bool is_term() const;

	bool child_is(size_t nt) const;

	node::type get_type() const;
	const std::string& get_type_name() const;
	const std::string& get_string() const;
	int_t get_integer() const;
	size_t get_num() const;
	size_t get_ba_constant_id() const;
	bas_variant get_ba_constant() const;
	size_t get_ba_type() const;
```

### creating nodes and trees with `tree<node>::get`

TODO

### getting tree from parser

 - `get(const tau_parser::result& result)`, etc.
 - `get<decltype(binder)>(binder, const tau_parser::result& result)`, etc.

### builders, queries, and other helpers

TODO

### struct `tree<node>::traverser` API

traverser is used as an alternative way to manually traverse a tree and extract values from it or transform it.

In implementation it is usually aliased with `using tt = tree<node<BAs...>::traverser;` to reduce template usage.

Traverser acts as `std::optional` able to wrap single or multiple `tref` nodes.

Construction:
 - `traverser();` - w/o value
 - `traverser(tref r);` - wrapping a single node
 - `traverser(const htree::sp& r);` - wrapping a single node from sp_handle
 - `traverser(const trefs& n);` - wrapping multiple nodes


From `std::optional` it follows:
  - `bool has_value()`
  - `explicit operator bool()`
  - `tref value()`

It adds methods for multiple values:
  - `const trefs& values()`
  - `bool empty()`
  - `bool size()`

It adds fast access to tree API of wrapped nodes
  - `const tree& value_tree()` - access tree API of the wrapped node (first one if multiple)
  - `const tree& operator[](size_t nth_child)` - access tree API of the nth wrapped node

And it can return vector of traversers wrapping each of the contained nodes
  - `std::vector<traverser> traversers()`
  - or `std::vector<traverser> operator()()`

Traverser is to be used with `operator|` or `operator||` which accepts `node::type` (`size_t`) or one of prepared tree traverser extractors `tree<node<BAs...>>::extractor<result_type>`

Available extractors:
```
	// handles
	extractor<tref>                   ref;                  // value()
	extractor<const trefs&>           refs;                 // values()
	extractor<htree::sp>              handle;               // geth(value())
	extractor<const tree<node>&>      Tree;                 // value_tree()

	// print/dump - returns itself
	extractor<traverser>              dump;                 // value_tree().dump(cout)
	extractor<traverser>              print_tree;           // value_tree().print_tree(cout)

	// node data
	extractor<type>                   nt;                   // value_tree().get_type()
	extractor<std::string>            string;               // value_tree().get_string()
	extractor<int_t>                  integer;              // value_tree().get_integer()
	extractor<size_t>                 num;                  // value_tree().get_num()
	extractor<size_t>                 data;                 // value_tree().data()
	extractor<size_t>                 ba_constant_id;       // value_tree().get_constant_id()
	extractor<size_t>                 ba_type;              // value_tree().get_ba_type()
	extractor<bas_variant>            ba_constant;          // value_tree().get_ba_constant()

	// children
	extractor<traverser>              only_child;           // value_tree().only_child()
	extractor<traverser>              first;                // value_tree().first()
	extractor<traverser>              second;
	extractor<traverser>              third;
	extractor<traverser>              children;             // value_tree().get_children()
	extractor<tref_range<node>>       children_range;       // value_tree().children()
	extractor<tree_range<tree<node>>> children_trees_range; // value_tree().children_trees()

	// (tref) -> tref function wrapping extractor
	extractor<traverser>              f(const auto& fn);
```

```
	using tau = tree<node<BAs...>>;
	using tt = tau::traverser;
	tref start_node; // having a tree reference

	// traverse from start_node tref to wff tref
	tref wff_node = tt(start_node) | tau::spec | tau::main | tau::wff | tt::ref;
	// extract rec relations
	trefs rec_rels = tt(start_node) | tau::spec || tau::rec_relations | tt::refs;
```


### pre_order and post_order, const and non-const traverser

TODO


### rewriter API

TODO


### terminals

 - numbers and integers

   TODO

 - strings

   TODO

 - constants

   TODO

## `ba_constants<BAs...>` pool and binding

TODO

### `ba_constants<BAs...>`

TODO

### `ba_constants_binder<BAs...>`

TODO