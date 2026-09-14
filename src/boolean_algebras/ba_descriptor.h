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
#include <functional>
#include <optional>
#include <ostream>
#include <string>
#include <tuple>
#include <utility>

#include "splitter_types.h"
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
	{ ba_descriptor<BA, Node>::hash_constant(x) }
		-> std::convertible_to<size_t>;
};

/**
 * @brief Kind of a BA-declared CLI/REPL option.
 *
 * `flag` also accepts enable/disable/toggle; `count` takes a number only.
 */
enum class ba_option_kind { flag, count };

/**
 * @brief One CLI/REPL option a BA declares about itself.
 *
 * `name` is the bare option name; the REPL and CLI address it as
 * `<family>-<name>` (e.g. `bv-blasting`), `<family>` being the owning
 * descriptor's `type_name` (see @ref pack_ba_options in ba_pack_traits.h).
 * The getter and setter are the only access path to the value.
 */
struct ba_option {
	const char* name;
	ba_option_kind kind;
	bool   (*get_flag)()        = nullptr; ///< set iff kind == flag
	void   (*set_flag)(bool)    = nullptr; ///< set iff kind == flag
	size_t (*get_count)()       = nullptr; ///< set iff kind == count
	void   (*set_count)(size_t) = nullptr; ///< set iff kind == count
	const char* help            = "";
};

/**
 * @brief `true` when @p BA's descriptor declares its own CLI/REPL options.
 *
 * Optional capability: a BA declaring no `options()` has none of its own, and
 * @ref pack_ba_options skips it.
 */
template <typename Node, typename BA>
constexpr bool ba_has_options_v = requires {
	{ *(ba_descriptor<BA, Node>::options().begin()) }
		-> std::convertible_to<ba_option>;
	{ ba_descriptor<BA, Node>::options().end() };
};

/**
 * @brief One concept per optional capability, all `<Node, BA>`.
 *
 * A fold, a consumer and the conformance test ask the same name, so a
 * capability's spelling lives in exactly one place. A member-function
 * capability is present when the call is well-formed with the argument
 * types core passes; a flag capability is read through its `_v` variable,
 * so a declared `false` is honoured rather than taken as "present".
 */
template <typename Node, typename BA>
concept ba_has_solve = ba_has_descriptor_v<Node, BA>
	&& requires(tref f) { ba_descriptor<BA, Node>::solve(f); };

template <typename Node, typename BA>
concept ba_has_can_solve = ba_has_descriptor_v<Node, BA>
	&& requires(tref f) {
		{ ba_descriptor<BA, Node>::can_solve(f) }
			-> std::convertible_to<bool>; };

template <typename Node, typename BA>
concept ba_has_sat_status = ba_has_descriptor_v<Node, BA>
	&& requires(tref f) {
		{ ba_descriptor<BA, Node>::sat_status(f) }
			-> std::convertible_to<std::optional<bool>>; };

template <typename Node, typename BA>
concept ba_has_preprocess = ba_has_descriptor_v<Node, BA>
	&& requires(tref f) {
		{ ba_descriptor<BA, Node>::preprocess(f) }
			-> std::convertible_to<tref>; };

template <typename Node, typename BA>
concept ba_has_case_split_quantifiers = ba_has_descriptor_v<Node, BA>
	&& requires(tref f) {
		{ ba_descriptor<BA, Node>::case_split_quantifiers(f) }
			-> std::convertible_to<tref>; };

template <typename Node, typename BA>
concept ba_has_set_preprocessing = ba_has_descriptor_v<Node, BA>
	&& requires(bool b) { ba_descriptor<BA, Node>::set_preprocessing(b); };

template <typename Node, typename BA>
concept ba_has_formula_is_preprocessable = ba_has_descriptor_v<Node, BA>
	&& requires(tref f) {
		{ ba_descriptor<BA, Node>::formula_is_preprocessable(f) }
			-> std::convertible_to<bool>; };

template <typename Node, typename BA>
concept ba_has_preprocessing_residue = ba_has_descriptor_v<Node, BA>
	&& requires(tref f) {
		{ ba_descriptor<BA, Node>::has_preprocessing_residue(f) }
			-> std::convertible_to<bool>; };

template <typename Node, typename BA>
concept ba_has_term_is_blasteable = ba_has_descriptor_v<Node, BA>
	&& requires(tref t) {
		{ ba_descriptor<BA, Node>::term_is_blasteable(t) }
			-> std::convertible_to<bool>; };

template <typename Node, typename BA>
concept ba_has_set_charvar = ba_has_descriptor_v<Node, BA>
	&& requires(bool b) { ba_descriptor<BA, Node>::set_charvar(b); };

template <typename Node, typename BA>
concept ba_has_component_factoring = ba_has_descriptor_v<Node, BA>
	&& requires(bool b) {
		ba_descriptor<BA, Node>::set_ba_component_factoring(b);
		{ ba_descriptor<BA, Node>::ba_component_factoring_enabled() }
			-> std::convertible_to<bool>; };

template <typename Node, typename BA>
concept ba_has_decision_pins = ba_has_descriptor_v<Node, BA>
	&& requires(size_t n) {
		ba_descriptor<BA, Node>::set_ba_decision_pins(n); };

template <typename Node, typename BA>
concept ba_has_zero_constant = ba_has_descriptor_v<Node, BA>
	&& requires(size_t t) {
		{ ba_descriptor<BA, Node>::zero_constant(t) }
			-> std::convertible_to<tref>; };

template <typename Node, typename BA>
concept ba_has_value_constant = ba_has_descriptor_v<Node, BA>
	&& requires(size_t t, size_t v) {
		{ ba_descriptor<BA, Node>::value_constant(t, v) }
			-> std::convertible_to<tref>; };

template <typename Node, typename BA>
concept ba_has_bool_carrier_type = ba_has_descriptor_v<Node, BA>
	&& requires {
		{ ba_descriptor<BA, Node>::bool_carrier_type() }
			-> std::convertible_to<tref>; };

template <typename Node, typename BA>
concept ba_has_omcat_qe = ba_has_descriptor_v<Node, BA>
	&& requires(tref v, tref b) {
		{ ba_descriptor<BA, Node>::omcat_qe(v, b) }
			-> std::convertible_to<std::optional<bool>>; };

template <typename Node, typename BA>
concept ba_has_semantic_pwr = ba_has_descriptor_v<Node, BA>
	&& requires(tref c, tref u) {
		{ ba_descriptor<BA, Node>::semantic_pwr_optimal(c, u) }
			-> std::convertible_to<tref>; };

template <typename Node, typename BA>
concept ba_has_codegen_witness = ba_has_descriptor_v<Node, BA>
	&& requires(tref v, tref c) {
		{ ba_descriptor<BA, Node>::codegen_witness(v, c) }
			-> std::convertible_to<std::optional<std::string>>; };

template <typename Node, typename BA>
concept ba_has_codegen_constant_expr = ba_has_descriptor_v<Node, BA>
	&& requires(tref c) {
		{ ba_descriptor<BA, Node>::codegen_constant_expr(c) }
			-> std::convertible_to<std::optional<std::string>>; };

template <typename Node, typename BA>
concept ba_has_literal_incomplete = ba_has_descriptor_v<Node, BA>
	&& requires(const std::string& s) {
		{ ba_descriptor<BA, Node>::literal_incomplete(s) }
			-> std::convertible_to<bool>; };

template <typename Node, typename BA>
concept ba_has_print_constant = ba_has_descriptor_v<Node, BA>
	&& requires(std::ostream& os, const BA& x) {
		{ ba_descriptor<BA, Node>::print_constant(os, x) }
			-> std::same_as<std::ostream&>; };

template <typename Node, typename BA>
concept ba_has_type_tree_for = ba_has_descriptor_v<Node, BA>
	&& requires(unsigned short p, tref t) {
		{ ba_descriptor<BA, Node>::type_tree_for(p) }
			-> std::convertible_to<tref>;
		{ ba_descriptor<BA, Node>::type_id_for(p) }
			-> std::convertible_to<size_t>;
		{ ba_descriptor<BA, Node>::type_param(t) }
			-> std::convertible_to<std::optional<unsigned short>>; };

/**
 * @brief The flag capabilities, read as values: `false` when absent.
 *
 * Variables rather than `requires` written inline at the point of use:
 * gcc 13.3 ICEs (cp/pt.cc:1747) on a requires-expression nested in a fold's
 * per-element lambda, and a name is what the conformance test enumerates.
 */
template <typename Node, typename BA>
constexpr bool ba_arith_ops_v = [] {
	if constexpr (ba_has_descriptor_v<Node, BA> && requires {
		{ ba_descriptor<BA, Node>::arith_ops } -> std::convertible_to<bool>; })
		return static_cast<bool>(ba_descriptor<BA, Node>::arith_ops);
	else return false;
}();

template <typename Node, typename BA>
constexpr bool ba_can_host_bool_v = [] {
	if constexpr (ba_has_descriptor_v<Node, BA> && requires {
		{ ba_descriptor<BA, Node>::can_host_bool }
			-> std::convertible_to<bool>; })
		return static_cast<bool>(ba_descriptor<BA, Node>::can_host_bool);
	else return false;
}();

template <typename Node, typename BA>
constexpr bool ba_uses_oracle_v = [] {
	if constexpr (ba_has_descriptor_v<Node, BA> && requires {
		{ ba_descriptor<BA, Node>::uses_oracle }
			-> std::convertible_to<bool>; })
		return static_cast<bool>(ba_descriptor<BA, Node>::uses_oracle);
	else return false;
}();

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
	// core compares constants against plain truth values
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
	// constants and closedness
 && requires(const BA& x) {
        { ba_descriptor<BA, Node>::is_syntactic_one(x) }
            -> std::convertible_to<bool>;                            }
 && requires(const BA& x) {
        { ba_descriptor<BA, Node>::is_syntactic_zero(x) }
            -> std::convertible_to<bool>;                            }
 && requires(const BA& x) {
        { ba_descriptor<BA, Node>::is_one(x) }
            -> std::convertible_to<bool>;                            }
 && requires(const BA& x) {
        { ba_descriptor<BA, Node>::is_zero(x) }
            -> std::convertible_to<bool>;                            }
 && requires(const BA& x) {
        { ba_descriptor<BA, Node>::is_closed(x) }
            -> std::convertible_to<bool>;                            }
	// literals
 && requires(tref t) {
        { ba_descriptor<BA, Node>::literal_one(t) }
            -> std::convertible_to<std::string>;                     }
 && requires(tref t) {
        { ba_descriptor<BA, Node>::literal_zero(t) }
            -> std::convertible_to<std::string>;                     }
	// normalization and splitting
 && requires(const BA& x) {
        ba_descriptor<BA, Node>::normalize(x);                       }
 && (!ba_descriptor<BA, Node>::atomless || requires(const BA& x, splitter_type st) {
        ba_descriptor<BA, Node>::splitter(x, st);                    })
 && (!ba_descriptor<BA, Node>::atomless || requires(tref t) {
        ba_descriptor<BA, Node>::splitter_one(t);                    })
	// symbol and term simplification
 && requires(tref t) {
        ba_descriptor<BA, Node>::simplify_symbol(t);                 }
 && requires(tref t) {
        ba_descriptor<BA, Node>::simplify_term(t);                   }
	// parsing
 && requires(const std::string& src, tref t) {
        ba_descriptor<BA, Node>::parse(src, t);                      };

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
