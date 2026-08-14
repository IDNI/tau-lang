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

/** @brief constexpr C-string equality, for comparing descriptor type names. */
constexpr bool ba_name_eq(const char* a, const char* b) {
	for (; *a || *b; ++a, ++b) if (*a != *b) return false;
	return true;
}

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
 && requires(tref t) {
        { ba_descriptor<BA, Node>::owns_type(t) }
            -> std::convertible_to<bool>;                            }
 && requires(size_t n) {
        { ba_descriptor<BA, Node>::owns_type(n) }
            -> std::convertible_to<bool>;                            }
 && requires(tref t) {
        ba_descriptor<BA, Node>::type_param(t);                      }
 && requires(unsigned short s) {
        { ba_descriptor<BA, Node>::type_id_for(s) }
            -> std::convertible_to<size_t>;                          }
 && requires(unsigned short s) {
        ba_descriptor<BA, Node>::type_tree_for(s);                   }
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
