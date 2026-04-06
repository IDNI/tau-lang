# Adding new base Boolean Algebras to Tau

This document decsribar how to add new base Boolean algebras (BAs) to Tau.


This is done by implementing the following elements:

* a template specialization of `base_ba_dispatcher` including your BA types,

* the usual logical operators for your new BA type, such as `operator&`,
`operator|`, `operator~` and so on,

* the usual comparison operators for your new BA type, such as
`operator==` and `operator!=` (or `operator<=>` if suits), and also the ones
related to equality and inequality comparisons with `bool` values, such as
`operator==(const YourBA& ba, bool b)` and `operator!=(const YourBA& ba, bool b)`,

* a template specialization of the method
`get(>::get(const std::string& constant_source, tref type_tree, [[maybe_unused]] const std::string options)`
of `ba_constants` in order to parse the constants of your BA from their string
representation in the source code, and finally

* a template specialization of the ` std::hash<YourBA>` struct to allow your
BA type to be used as a key in hash-based containers, such as
`std::unordered_map` and `std::unordered_set`.

## Specializing `base_ba_dispatcher`

To add a new base Boolean algebra (BA) to Tau, you need to create a template
specialization of the `base_ba_dispatcher` struct for your BA type.
This specialization should provide implementations for the static methods
defined in the primary template, tailored to your BA's behavior.

You could take a look at the existing specializations of `base_ba_dispatcher`
present in file `src/base_ba_dispatcher.tmpl.h` and the tests in
`tests/test_Bool_helpers.h` as references.

## Defining operators for your BAs

To ensure that your BA can be used effectively within Tau, you need to define the
usual Boolean operators for your new BA type. More precisely,
you have to implement `operator&`, `operator|`, `operator~` and `operator^` for your
new BA type (the default BA types operators are already defined).

An example of how to implement these operators can be found in the file
`src/boolean_algebras/tau_ba.h` (and the corresponding
`src/boolean_algebras/tau_ba.tmpl.h`). Also check `src/boolean_algebras/bdd_handle.h`.

## Defining comparison operators for your new BA

In addition to the logical operators, you also need to define the equality and
inequality operators for your new BA type. This includes `operator==` and
`operator!=` for comparing two instances of your BA, as well as the operators for
comparing your BA with `bool` values, such as `operator==(const YourBA& ba, bool b)`
and `operator!=(const YourBA& ba, bool b)`.

An example of how to implement these operators can be found in the file
`src/boolean_algebras/tau_ba.h` (and the corresponding
`src/boolean_algebras/tau_ba.tmpl.h`). Also check `src/boolean_algebras/bdd_handle.h`.

## Parsing constants of your BAs

To allow Tau to recognize and parse constants of your new BA type from their
string representation in the source code, you need to specialize the `get`
method of the `ba_constants` struct for your BA type. This method should take a
string representation of a constant, along with its type information, and return
an instance of `constant_with_type`.

An example of how to implement this specialization can be found in the file
`src/base_ba_dispatcher.tmpl.h`. In this case, we have defined specialized
grammars for each of the BA types considered and call to the proper generated
parser to parse accordingly to the BA type. The grammars are defined in
`parser/` (also the generated parsers).

## Hashing your BA type

To enable your new BA type to be used as a key in hash-based containers, such as
`std::unordered_map` and `std::unordered_set`, you need to specialize the
`std::hash` struct for your BA type. This involves providing an implementation
of the `operator()` method that computes a hash value for an instance of your
new BA type.

An example of how to implement this specialization can be found in the file
`src/boolean_algebras/tau_ba.h`.