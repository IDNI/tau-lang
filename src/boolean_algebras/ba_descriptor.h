// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ba_descriptor.h
 * @brief Compile-time interface through which core reaches a Boolean algebra.
 *
 * Each BA specializes `ba_descriptor<BA, Node>` with the mandatory surface
 * listed by `ba_descriptor_complete`. Generic code asks a descriptor what a BA
 * can do; it never tests for a BA by name. Capabilities beyond the mandatory
 * surface are optional and probed with `requires`.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BA_DESCRIPTOR_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BA_DESCRIPTOR_H__

#include <concepts>
#include <cstdint>
#include <functional>
#include <optional>
#include <ostream>
#include <string>
#include <tuple>
#include <utility>

#include "splitter_types.h"
#include "tau_diagnostics.h"
#include "utility/options.h"
#include "utility/tree_types.h"

namespace idni::tau_lang {

/**
 * @brief Descriptor of Boolean algebra @p BA as seen from node type @p Node.
 *
 * Specialize in the BA's own `<ba>_descriptor.tmpl.h`. The primary template is
 * left undefined so an unspecialized BA fails where it is used.
 */
template <typename BA, typename Node>
struct ba_descriptor;

/**
 * @brief Per-BA hook extensions; specialize in `<ba>_ba_hooks_ext.tmpl.h`.
 *
 * Unlike ba_descriptor these primaries are defined and empty, so a BA that
 * defines no hook of a given kind needs no specialization at all: the folds
 * probe for each member with `requires` and skip a BA that lacks it.
 */
template <typename BA, typename Node>
struct ba_term_hooks {};

/// @brief Per-BA formula hook extensions; see ba_term_hooks.
template <typename BA, typename Node>
struct ba_wff_hooks {};

/** @brief `true` when @p BA has a descriptor for @p Node. */
template <typename Node, typename BA>
constexpr bool ba_has_descriptor_v = requires {
	{ ba_descriptor<BA, Node>::type_name }
		-> std::convertible_to<const char*>;
};

/**
 * @brief `true` when @p BA's descriptor content-hashes its own constants.
 *
 * Optional capability: `node::hashit()` (tau_tree_node.tmpl.h) visits a
 * ba_constant's value and, for whichever alternative it holds, prefers
 * `hash_constant` over `std::hash<BA>` when the owning BA declares it. Most
 * BAs need nothing here because `std::hash<BA>` is already content-derived;
 * bv declares it because the default `std::hash<cvc5::Term>` is the term's
 * creation id, not its content (GitHub #89 -- see hash_bv_constant in
 * backends/cvc5/cvc5.h). Like `print_constant`, this is probed at the point
 * of use rather than folded in ba_pack_traits.h, since a variant visit
 * already names the one BA to ask.
 */
template <typename Node, typename BA>
constexpr bool ba_has_hash_constant_v = requires(const BA& x) {
	ba_descriptor<BA, Node>::hash_constant(x);
};

/**
 * @brief `true` unless @p BA declares a `hash_constant` whose result is not
 * `std::uint64_t`. A `size_t` hash keeps 32 bits on wasm32 and would give
 * another constant order there.
 */
template <typename Node, typename BA>
constexpr bool ba_hash_constant_well_typed_v = [] {
	if constexpr (ba_has_hash_constant_v<Node, BA>)
		return std::is_same_v<decltype(ba_descriptor<BA, Node>::hash_constant(
			std::declval<const BA&>())), std::uint64_t>;
	else return true;
}();

/*
 * One concept per optional capability, all `<Node, BA>`.
 *
 * A fold, a consumer and the conformance test ask the same name, so a
 * capability's spelling lives in exactly one place. A member-function
 * capability is present when the call is well-formed with the argument
 * types core passes; a flag capability is read through its `_v` variable,
 * so a declared `false` is honoured rather than taken as "present".
 * docs/adding_base_bas.md lists each member with its resolution rule.
 */
/// @brief @p BA decides a whole formula of its types with its own `solve(f)`,
/// a `result` whose value is the solution or nullopt when there is none, and
/// whose error is why it could not try.
template <typename Node, typename BA>
concept ba_has_solve = ba_has_descriptor_v<Node, BA>
	&& requires(tref f) { ba_descriptor<BA, Node>::solve(f); };

/// @brief @p BA tells whether it can decide a formula: `can_solve(f)`.
template <typename Node, typename BA>
concept ba_has_can_solve = ba_has_descriptor_v<Node, BA>
	&& requires(tref f) {
		{ ba_descriptor<BA, Node>::can_solve(f) }
			-> std::convertible_to<bool>; };

/// @brief @p BA gives a definite satisfiability answer, nullopt when unknown:
/// `sat_status(f)`.
template <typename Node, typename BA>
concept ba_has_sat_status = ba_has_descriptor_v<Node, BA>
	&& requires(tref f) {
		{ ba_descriptor<BA, Node>::sat_status(f) }
			-> std::convertible_to<std::optional<bool>>; };

/// @brief @p BA rewrites a formula before solving: `preprocess(f)`, an error
/// naming why it failed.
template <typename Node, typename BA>
concept ba_has_preprocess = ba_has_descriptor_v<Node, BA>
	&& requires(tref f) {
		{ ba_descriptor<BA, Node>::preprocess(f) }
			-> std::same_as<result<tref>>; };

/// @brief @p BA eliminates its quantified variables tested only against
/// constants by a finite case split: `case_split_quantifiers(f)`.
template <typename Node, typename BA>
concept ba_has_case_split_quantifiers = ba_has_descriptor_v<Node, BA>
	&& requires(tref f) {
		{ ba_descriptor<BA, Node>::case_split_quantifiers(f) }
			-> std::convertible_to<tref>; };

/// @brief @p BA substitutes its existential variables a total definition
/// determines: `eliminate_definitional_existentials(f)`.
template <typename Node, typename BA>
concept ba_has_eliminate_definitional_existentials = ba_has_descriptor_v<Node, BA>
	&& requires(tref f) {
		{ ba_descriptor<BA, Node>::eliminate_definitional_existentials(f) }
			-> std::convertible_to<tref>; };

/// @brief @p BA elaborates its arithmetic atoms to an overflow-free width:
/// `widen_arithmetic(f)`, an error naming why it failed.
template <typename Node, typename BA>
concept ba_has_widen_arithmetic = ba_has_descriptor_v<Node, BA>
	&& requires(tref f) {
		{ ba_descriptor<BA, Node>::widen_arithmetic(f) }
			-> std::same_as<result<tref>>; };

/// @brief @p BA reports whether its widening is on: `widening_state()`.
template <typename Node, typename BA>
concept ba_has_widening_state = ba_has_descriptor_v<Node, BA>
	&& requires() {
		{ ba_descriptor<BA, Node>::widening_state() }
			-> std::convertible_to<bool>; };

/// @brief @p BA has a switch for its preprocessing pass: `set_preprocessing(b)`.
template <typename Node, typename BA>
concept ba_has_set_preprocessing = ba_has_descriptor_v<Node, BA>
	&& requires(bool b) { ba_descriptor<BA, Node>::set_preprocessing(b); };

/// @brief @p BA tells whether its pass can still make progress on a formula:
/// `formula_is_preprocessable(f)`.
template <typename Node, typename BA>
concept ba_has_formula_is_preprocessable = ba_has_descriptor_v<Node, BA>
	&& requires(tref f) {
		{ ba_descriptor<BA, Node>::formula_is_preprocessable(f) }
			-> std::convertible_to<bool>; };

/// @brief @p BA tells whether its pass left a shape that closing would make
/// expensive: `has_preprocessing_residue(f)`.
template <typename Node, typename BA>
concept ba_has_preprocessing_residue = ba_has_descriptor_v<Node, BA>
	&& requires(tref f) {
		{ ba_descriptor<BA, Node>::has_preprocessing_residue(f) }
			-> std::convertible_to<bool>; };

/// @brief @p BA tells whether a term with an arithmetic operator can be
/// blasted: `term_is_blasteable(t)`.
template <typename Node, typename BA>
concept ba_has_term_is_blasteable = ba_has_descriptor_v<Node, BA>
	&& requires(tref t) {
		{ ba_descriptor<BA, Node>::term_is_blasteable(t) }
			-> std::convertible_to<bool>; };

/// @brief @p BA keeps its grammar in step with core's var/charvar mode:
/// `set_charvar(b)`.
template <typename Node, typename BA>
concept ba_has_set_charvar = ba_has_descriptor_v<Node, BA>
	&& requires(bool b) { ba_descriptor<BA, Node>::set_charvar(b); };

/// @brief @p BA has its own component-factoring switch:
/// `set_ba_component_factoring(b)` and `ba_component_factoring_enabled()`.
template <typename Node, typename BA>
concept ba_has_component_factoring = ba_has_descriptor_v<Node, BA>
	&& requires(bool b) {
		ba_descriptor<BA, Node>::set_ba_component_factoring(b);
		{ ba_descriptor<BA, Node>::ba_component_factoring_enabled() }
			-> std::convertible_to<bool>; };

/// @brief @p BA caps the decided rows it keeps pinned:
/// `set_ba_decision_pins(n)` sets the cap and `ba_decision_pins()` reads
/// its effective value.
template <typename Node, typename BA>
concept ba_has_decision_pins = ba_has_descriptor_v<Node, BA>
	&& requires(size_t n) {
		ba_descriptor<BA, Node>::set_ba_decision_pins(n);
		{ ba_descriptor<BA, Node>::ba_decision_pins() }
			-> std::convertible_to<size_t>; };

/// @brief @p BA has options. `declared_options()` gives the set, each name
/// `<prefix>-<name>` (see @ref ba_option_prefix). `bind_options(repo)` binds
/// each option to its field after the pack declares the set.
template <typename Node, typename BA>
concept ba_has_options = ba_has_descriptor_v<Node, BA>
	&& requires(options_repository& repo) {
		{ ba_descriptor<BA, Node>::declared_options() }
			-> std::same_as<const option_set&>;
		{ ba_descriptor<BA, Node>::bind_options(repo) }
			-> std::same_as<result<void>>; };

/// @brief @p BA gives the default zero of its type @p t when it is not `bf_f`:
/// `zero_constant(t)`.
template <typename Node, typename BA>
concept ba_has_zero_constant = ba_has_descriptor_v<Node, BA>
	&& requires(size_t t) {
		{ ba_descriptor<BA, Node>::zero_constant(t) }
			-> std::convertible_to<tref>; };

/// @brief @p BA builds a constant of type @p t holding the integer @p v:
/// `value_constant(t, v)`, nullptr when @p v is not a value of @p t.
template <typename Node, typename BA>
concept ba_has_value_constant = ba_has_descriptor_v<Node, BA>
	&& requires(size_t t, size_t v) {
		{ ba_descriptor<BA, Node>::value_constant(t, v) }
			-> std::convertible_to<tref>; };

/// The type's values are the integers 0 .. 2^n - 1 (n = modular_width, 0 for
/// a type that is not), read with unsigned modular semantics: the Boolean
/// operators bitwise, + - * modulo 2^n, / % unsigned, shifts logical, the
/// comparisons unsigned. modular_value reads the integer a constant holds.
template <typename Node, typename BA>
concept ba_has_modular_bits = ba_has_descriptor_v<Node, BA>
	&& requires(size_t t, tref c) {
		{ ba_descriptor<BA, Node>::modular_width(t) }
			-> std::convertible_to<size_t>;
		{ ba_descriptor<BA, Node>::modular_value(t, c) }
			-> std::convertible_to<std::optional<uint64_t>>; };

/// decide_closed decides a closed formula over the type whatever its
/// quantifier prefix, with no quantifier eliminated first: true when it
/// holds, false when not, nullopt when undecided (the formula untranslatable,
/// the decision out of budget).
template <typename Node, typename BA>
concept ba_has_closed_decision = ba_has_descriptor_v<Node, BA>
	&& requires(tref f) {
		{ ba_descriptor<BA, Node>::decide_closed(f) }
			-> std::convertible_to<std::optional<bool>>; };

/// decide_ground decides a formula without variables, streams or temporal
/// operators whose constants are all of the type, which normalization leaves
/// standing when the type cannot decide its comparisons one at a time: true
/// when it holds, false when not, nullopt when undecided.
template <typename Node, typename BA>
concept ba_has_ground_decision = ba_has_descriptor_v<Node, BA>
	&& requires(tref f) {
		{ ba_descriptor<BA, Node>::decide_ground(f) }
			-> std::convertible_to<std::optional<bool>>; };

/// The type's values, read by = and the order comparisons, form a dense
/// linear order without endpoints; dense_order_compare orders two constants
/// (-1, 0, 1), nullopt when either is not a point of the order, and
/// dense_order_between gives a point above `lo` and below `hi` (either
/// nullptr for no bound), nullptr when a bound is not a point.
template <typename Node, typename BA>
concept ba_has_dense_order = ba_has_descriptor_v<Node, BA>
	&& requires(size_t t, tref a, tref b) {
		{ ba_descriptor<BA, Node>::dense_order_compare(t, a, b) }
			-> std::convertible_to<std::optional<int>>;
		{ ba_descriptor<BA, Node>::dense_order_between(t, a, b) }
			-> std::convertible_to<tref>; };

/// @brief @p BA names which of its types carries a plain 0/1 when that is
/// not its `type_tree()`: `bool_carrier_type()`.
template <typename Node, typename BA>
concept ba_has_bool_carrier_type = ba_has_descriptor_v<Node, BA>
	&& requires {
		{ ba_descriptor<BA, Node>::bool_carrier_type() }
			-> std::convertible_to<tref>; };

/// @brief @p BA decides a quantifier over its own theory for every value of
/// the other variables, nullopt when that depends on them:
/// `omcat_qe(var, body)`.
template <typename Node, typename BA>
concept ba_has_omcat_qe = ba_has_descriptor_v<Node, BA>
	&& requires(tref v, tref b) {
		{ ba_descriptor<BA, Node>::omcat_qe(v, b) }
			-> std::convertible_to<std::optional<bool>>; };

/// A quantifier-free formula equivalent to `ex var. body` over the other
/// variables, or nullptr when the theory cannot eliminate var that way.
template <typename Node, typename BA>
concept ba_has_omcat_qe_residual = ba_has_descriptor_v<Node, BA>
	&& requires(tref v, tref b) {
		{ ba_descriptor<BA, Node>::omcat_qe_residual(v, b) }
			-> std::convertible_to<tref>; };

/// @brief @p BA revises a clause through its winning region:
/// `semantic_pwr_optimal(clause, update)`.
template <typename Node, typename BA>
concept ba_has_semantic_pwr = ba_has_descriptor_v<Node, BA>
	&& requires(tref c, tref u) {
		{ ba_descriptor<BA, Node>::semantic_pwr_optimal(c, u) }
			-> std::same_as<result<tref>>; };

/// @brief @p BA spells a witness of a variable in generated C++, nullopt when
/// it cannot: `codegen_witness(var, conj)`.
template <typename Node, typename BA>
concept ba_has_codegen_witness = ba_has_descriptor_v<Node, BA>
	&& requires(tref v, tref c) {
		{ ba_descriptor<BA, Node>::codegen_witness(v, c) }
			-> std::convertible_to<std::optional<std::string>>; };

/// @brief @p BA spells a constant in generated C++, nullopt when it has no
/// spelling for it and an error when building one failed:
/// `codegen_constant_expr(c)`.
template <typename Node, typename BA>
concept ba_has_codegen_constant_expr = ba_has_descriptor_v<Node, BA>
	&& requires(tref c) {
		{ ba_descriptor<BA, Node>::codegen_constant_expr(c) }
			-> std::convertible_to<
				result<std::optional<std::string>>>; };

/// @brief @p BA tells whether a partly typed literal is truncated rather
/// than malformed, so the REPL keeps reading: `literal_incomplete(src)`.
template <typename Node, typename BA>
concept ba_has_literal_incomplete = ba_has_descriptor_v<Node, BA>
	&& requires(const std::string& s) {
		{ ba_descriptor<BA, Node>::literal_incomplete(s) }
			-> std::convertible_to<bool>; };

/**
 * @brief `true` when @p BA can tell whether a constant is exact.
 *
 * Optional capability: a constant whose value depends on an unknown part (qlt's
 * named endpoints) may be an over-approximation, and folding two constants
 * into such a value would decide comparisons the operands do not decide. Core
 * keeps the operation as a term when `exact_constant` answers false, and leaves
 * the comparison of such a constant with 0 or 1 to the owner's `=`/`!=` hooks,
 * keeping it when they decline. Probed at
 * the point of use, like `print_constant`; absent means every constant is
 * exact.
 */
template <typename Node, typename BA>
concept ba_has_exact_constant = ba_has_descriptor_v<Node, BA>
	&& requires(const BA& x) {
		{ ba_descriptor<BA, Node>::exact_constant(x) }
			-> std::convertible_to<bool>; };

/// @brief @p BA renders a constant for Tau when its own `operator<<` does not:
/// `print_constant(os, x)`; probed at the point of use.
template <typename Node, typename BA>
concept ba_has_print_constant = ba_has_descriptor_v<Node, BA>
	&& requires(std::ostream& os, const BA& x) {
		{ ba_descriptor<BA, Node>::print_constant(os, x) }
			-> std::same_as<std::ostream&>; };

/**
 * @brief `true` when @p BA's constants carry a tree that grows with the
 * operations applied to them, and the descriptor measures it.
 *
 * Optional capability: the wrapper's constant embeds a whole spec, and each
 * Boolean operation on two constants builds a larger one, so a leaf of one
 * node can hold an arbitrarily large formula. `generated_constant_size`
 * (solver.tmpl.h) adds `constant_size` to a tree's own node count, which is
 * what `max_constant_size` bounds. Probed at the point of use, like
 * `print_constant`; absent means a constant counts as its leaf alone.
 */
template <typename Node, typename BA>
concept ba_has_constant_size = ba_has_descriptor_v<Node, BA>
	&& requires(const BA& x) {
		{ ba_descriptor<BA, Node>::constant_size(x) }
			-> std::convertible_to<size_t>; };

/// @brief @p BA's family is parameterised (`bv[8]`): `type_param(t)`,
/// `type_id_for(p)` and `type_tree_for(p)`, all three or none.
template <typename Node, typename BA>
concept ba_has_type_tree_for = ba_has_descriptor_v<Node, BA>
	&& requires(unsigned short p, tref t) {
		{ ba_descriptor<BA, Node>::type_tree_for(p) }
			-> std::convertible_to<tref>;
		{ ba_descriptor<BA, Node>::type_id_for(p) }
			-> std::convertible_to<size_t>;
		{ ba_descriptor<BA, Node>::type_param(t) }
			-> std::convertible_to<std::optional<unsigned short>>; };

/*
 * The flag capabilities, read as values: `false` when absent.
 *
 * Variables rather than `requires` written inline at the point of use:
 * gcc 13.3 ICEs (cp/pt.cc:1747) on a requires-expression nested in a fold's
 * per-element lambda, and a name is what the conformance test enumerates.
 */
/// @brief The grammar's arithmetic term operators apply to @p BA's type.
template <typename Node, typename BA>
constexpr bool ba_arith_ops_v = [] {
	if constexpr (ba_has_descriptor_v<Node, BA> && requires {
		{ ba_descriptor<BA, Node>::arith_ops } -> std::convertible_to<bool>; })
		return static_cast<bool>(ba_descriptor<BA, Node>::arith_ops);
	else return false;
}();

/// @brief One of @p BA's types can hold a plain 0/1 (a Boolean carrier).
template <typename Node, typename BA>
constexpr bool ba_can_host_bool_v = [] {
	if constexpr (ba_has_descriptor_v<Node, BA> && requires {
		{ ba_descriptor<BA, Node>::can_host_bool }
			-> std::convertible_to<bool>; })
		return static_cast<bool>(ba_descriptor<BA, Node>::can_host_bool);
	else return false;
}();

/// @brief Deciding a question over @p BA leaves the process, so
/// comparison-based conformance checks skip it.
template <typename Node, typename BA>
constexpr bool ba_uses_oracle_v = [] {
	if constexpr (ba_has_descriptor_v<Node, BA> && requires {
		{ ba_descriptor<BA, Node>::uses_oracle }
			-> std::convertible_to<bool>; })
		return static_cast<bool>(ba_descriptor<BA, Node>::uses_oracle);
	else return false;
}();

/// @brief A system can always meet an output constraint of @p BA's type by
/// choosing its output (`output_always_satisfiable_by_system`).
template <typename Node, typename BA>
constexpr bool ba_output_always_satisfiable_v = [] {
	if constexpr (ba_has_descriptor_v<Node, BA> && requires {
		{ ba_descriptor<BA, Node>::output_always_satisfiable_by_system }
			-> std::convertible_to<bool>; })
		return static_cast<bool>(
			ba_descriptor<BA, Node>::output_always_satisfiable_by_system);
	else return false;
}();

/**
 * @brief The surface every descriptor must provide.
 *
 * Each requirement sits on its own line so an omitted member is reported
 * against the line naming it. Optional capabilities (own solver, arithmetic,
 * quantifier elimination, hosting the Boolean carrier, ...) are deliberately
 * absent here and are probed where they are used.
 */
template <typename BA, typename Node>
concept ba_descriptor_complete =
	// node<BAs...> keeps constants in a std::variant<BAs...> that generic
	// code compares directly, so every BA value type must compare equal.
	std::equality_comparable<BA>
	// the tree printer streams whichever alternative a constant holds
 && requires(std::ostream& os, const BA& x) {
        { os << x } -> std::same_as<std::ostream&>;                  }
	// constants live in a hashed variant, so the value type must hash
 && requires(const BA& x) {
        { std::hash<BA>{}(x) } -> std::convertible_to<size_t>;       }
	// an optional hash_constant is the 64-bit, platform-independent hash
 && ba_hash_constant_well_typed_v<Node, BA>
	// core compares constants against plain truth values; `x == b` is true
	// only when it is decided, and is_one / is_zero carry the report
 && requires(const BA& x, bool b) {
        { x == b } -> std::convertible_to<bool>;                     }
	// binary/unary operators core dispatches directly on a BA's value type
 && requires(const BA& x, const BA& y) {
        { x & y } -> std::convertible_to<BA>;                        }
 && requires(const BA& x, const BA& y) {
        { x | y } -> std::convertible_to<BA>;                        }
 && requires(const BA& x, const BA& y) {
        { x ^ y } -> std::convertible_to<BA>;                        }
 && requires(const BA& x) {
        { ~x } -> std::convertible_to<BA>;                           }
	// identity and classification
 && requires { { ba_descriptor<BA, Node>::type_name }
                   -> std::convertible_to<const char*>;              }
 && requires { { ba_descriptor<BA, Node>::default_type_priority }
                   -> std::convertible_to<int>;                      }
 && requires { { ba_descriptor<BA, Node>::atomless }
                   -> std::convertible_to<bool>;                     }
 && requires { { ba_descriptor<BA, Node>::non_aba_omcat }
                   -> std::convertible_to<bool>;                     }
	// type system
 && requires(tref t) {
        { ba_descriptor<BA, Node>::matches_type(t) }
            -> std::convertible_to<bool>;                            }
 && requires { ba_descriptor<BA, Node>::type_tree();                 }
 && requires(size_t n) {
        { ba_descriptor<BA, Node>::owns_type(n) }
            -> std::convertible_to<bool>;                            }
	// constants and closedness -- is_syntactic_one/zero stay plain bool
	// (purely syntactic, cannot fail); is_one/is_zero/is_closed can run a
	// full decision procedure, so each returns a result carrying why a
	// failed decision could not be made
 && requires(const BA& x) {
        { ba_descriptor<BA, Node>::is_syntactic_one(x) }
            -> std::convertible_to<bool>;                            }
 && requires(const BA& x) {
        { ba_descriptor<BA, Node>::is_syntactic_zero(x) }
            -> std::convertible_to<bool>;                            }
 && requires(const BA& x) {
        { ba_descriptor<BA, Node>::is_one(x) }
            -> std::same_as<result<bool>>;                           }
 && requires(const BA& x) {
        { ba_descriptor<BA, Node>::is_zero(x) }
            -> std::same_as<result<bool>>;                           }
 && requires(const BA& x) {
        { ba_descriptor<BA, Node>::is_closed(x) }
            -> std::same_as<result<bool>>;                           }
	// literals
 && requires(tref t) {
        { ba_descriptor<BA, Node>::literal_one(t) }
            -> std::convertible_to<std::string>;                     }
 && requires(tref t) {
        { ba_descriptor<BA, Node>::literal_zero(t) }
            -> std::convertible_to<std::string>;                     }
	// normalization and splitting; both can run a full normalization, so
	// each returns a result carrying why a failed one could not be made
 && requires(const BA& x) {
        { ba_descriptor<BA, Node>::normalize(x) }
            -> std::same_as<result<BA>>;                             }
 && (!ba_descriptor<BA, Node>::atomless || requires(const BA& x, splitter_type st) {
        { ba_descriptor<BA, Node>::splitter(x, st) }
            -> std::same_as<result<BA>>;                             })
 && (!ba_descriptor<BA, Node>::atomless || requires(tref t) {
        ba_descriptor<BA, Node>::splitter_one(t);                    })
	// symbol and term simplification -- simplify_term can run a bounded
	// rewrite loop that gives up, so its result carries why
 && requires(tref t) {
        ba_descriptor<BA, Node>::simplify_symbol(t);                 }
 && requires(tref t) {
        { ba_descriptor<BA, Node>::simplify_term(t) }
            -> std::same_as<result<tref>>;                           }
	// parsing: a `result` carries the parsed constant, or, on refusal, a
	// report explaining why -- never a bare empty optional
 && requires(const std::string& src, tref t) {
        { ba_descriptor<BA, Node>::parse(src, t) }
            -> std::same_as<result<typename Node::constant_with_type>>;  };

/** @internal @brief Fold of `ba_descriptor_complete` over the pack. */
template <typename Node, std::size_t... Is>
constexpr bool assert_pack_complete_impl(std::index_sequence<Is...>) {
	using pack = typename Node::bas_tuple;
	return (ba_descriptor_complete<std::tuple_element_t<Is, pack>, Node>
		&& ...);
}

/**
 * @brief `true` when every BA in @p Node's pack has a complete descriptor.
 *
 * Static-assert this at a pack's first instantiation site; a BA missing a
 * member fails the concept with that member named.
 */
template <typename Node>
constexpr bool assert_pack_descriptors_complete() {
	return assert_pack_complete_impl<Node>(
		std::make_index_sequence<
			std::tuple_size_v<typename Node::bas_tuple>>{});
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BA_DESCRIPTOR_H__
