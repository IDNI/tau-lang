// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ext_ba.h
 * @brief A Boolean algebra living outside src/, used to prove out-of-tree
 * registration works. Copied in spirit from src/boolean_algebras/_template.
 *
 * Registered by register.cmake and pulled into a build with
 *   -DTAU_EXTERNAL_BAS=<abs path>/register.cmake -DTAU_BAS=tau,sbf,ext
 */

#ifndef __IDNI__TAU__TESTS__EXTERNAL_BA__EXT_BA_H__
#define __IDNI__TAU__TESTS__EXTERNAL_BA__EXT_BA_H__

#include <array>
#include <functional>
#include <optional>
#include <ostream>
#include <string>

#include "tau_tree.h"
#include "tau_diagnostics.h"
#include "ba_types.h"
#include "boolean_algebras/ba_descriptor.h"

namespace idni::tau_lang {

/** @brief Two-element carrier, enough to exercise the descriptor surface. */
struct ext_ba {
	bool value = false;

	auto operator<=>(const ext_ba&) const = default;
	// declaring operator==(bool) below would otherwise suppress this
	bool operator==(const ext_ba&) const = default;
	/** @brief Core compares constants against plain truth values. */
	bool operator==(bool b) const { return value == b; }

	ext_ba operator~() const { return { !value }; }
	ext_ba operator&(const ext_ba& o) const { return { value && o.value }; }
	ext_ba operator|(const ext_ba& o) const { return { value || o.value }; }
	ext_ba operator^(const ext_ba& o) const { return { value != o.value }; }
	ext_ba operator+(const ext_ba& o) const { return *this ^ o; }
};

/** @brief Every BA in a pack must be streamable; the tree printer needs it. */
inline std::ostream& operator<<(std::ostream& os, const ext_ba& x) {
	return os << (x.value ? "1" : "0");
}

/** @brief Type tree naming this algebra in an annotation (`x:ext`). */
template <NodeType node>
tref ext_ba_type() {
	return ba_descriptor<ext_ba, node>::type_tree();
}

template <typename... BAs>
result<typename node<BAs...>::constant_with_type> parse_ext_ba(
	const std::string& src)
{
	using node_t = node<BAs...>;
	result<typename node_t::constant_with_type> r;
	if (src != "0" && src != "1") {
		r.error(code::parse_error, "Not a valid ext literal: " + src);
		return r;
	}
	return r.with_value(typename node_t::constant_with_type{
		typename node_t::constant{ ext_ba{ src == "1" } },
		ext_ba_type<node_t>() });
}

template <typename... PackBAs>
struct ba_descriptor<ext_ba, node<PackBAs...>> {
	using node_t = node<PackBAs...>;
	using tau = tree<node_t>;

	static constexpr const char* type_name = "ext";
	static constexpr int default_type_priority = 90;
	static constexpr bool atomless = false;
	static constexpr bool non_aba_omcat = false;

	static bool matches_type(tref type_tree) {
		return ba_types_detail::type_tree_name_is<ext_ba, node_t>(
			type_tree, type_name);
	}

	static tref type_tree() {
		return ba_types_detail::make_syntactic_type_tree<node_t>(
			type_name);
	}

	static bool owns_type(size_t ba_type_id) {
		return ba_types_detail::type_tree_name_is<ext_ba, node_t>(
			ba_type_id, type_name);
	}


	static bool is_syntactic_one(const ext_ba& x) { return x.value; }
	static bool is_syntactic_zero(const ext_ba& x) { return !x.value; }
	static result<bool> is_one(const ext_ba& x) { return result<bool>{x.value}; }

	/**
	 * @brief `!x.value`, unless @ref fail_is_zero_ asks it to fail -- a
	 * deterministic hook for pinning that an is_zero failure carries a
	 * report, with no dependence on a real BA's own failure conditions.
	 */
	static result<bool> is_zero(const ext_ba& x) {
		if (fail_is_zero_) {
			result<bool> r;
			r.error(code::internal_error,
				"ext is_zero forced to fail for testing");
			return r;
		}
		return result<bool>{!x.value};
	}
	static inline bool fail_is_zero_ = false;

	static result<bool> is_closed(const ext_ba&) { return result<bool>{true}; }

	static std::string literal_one(tref) { return "1"; }
	static std::string literal_zero(tref) { return "0"; }

	static ext_ba normalize(const ext_ba& x) { return x; }
	static ext_ba splitter(const ext_ba& x, splitter_type) { return x; }
	static tref splitter_one(tref) { return nullptr; }

	static tref simplify_symbol(tref sym) { return sym; }

	/**
	 * @brief Identity, unless @ref fail_simplify_term_ asks it to fail --
	 * the same deterministic hook as @ref fail_is_zero_, for simplify_term.
	 */
	static result<tref> simplify_term(tref term) {
		if (fail_simplify_term_) {
			result<tref> r;
			r.error(code::internal_error,
				"ext simplify_term forced to fail for testing");
			return r;
		}
		return result<tref>{term};
	}
	static inline bool fail_simplify_term_ = false;

	static result<typename node_t::constant_with_type>
	parse(const std::string& src, tref)
	{
		return parse_ext_ba<PackBAs...>(src);
	}

	// Optional capabilities, one of each resolution kind, so the
	// out-of-tree path proves every way core reaches a plugin.

	/** @brief Owner-gated: the zero of this type as a bf constant. */
	static tref zero_constant(size_t) {
		return tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(ext_ba{ false }), type_tree()));
	}

	/**
	 * @brief Accumulated: identity, unless @ref fail_preprocess_ asks it
	 * to fail -- a deterministic hook for pinning that a failure carries
	 * a report, with no dependence on a real BA's own failure conditions.
	 */
	static result<tref> preprocess(tref n) {
		if (fail_preprocess_) {
			result<tref> r;
			r.error(code::internal_error,
				"ext preprocess forced to fail for testing");
			return r;
		}
		return result<tref>{n};
	}
	static void set_preprocessing(bool) {}
	static inline bool fail_preprocess_ = false;

	/** @brief Declared: an option of its own, addressed as `ext-probe`. */
	static bool get_probe() { return probe_; }
	static void set_probe(bool b) { probe_ = b; }
	static std::array<ba_option, 1> options() {
		return {{ { "probe", ba_option_kind::flag, get_probe, set_probe,
			nullptr, nullptr,
			"a switch with no effect, proving options cross the plugin "
			"boundary" } }};
	}
	static inline bool probe_ = false;
};

/** @brief Hooked: declines every comparison, so the atom must survive. */
template <typename... PackBAs>
struct ba_wff_hooks<ext_ba, node<PackBAs...>> {
	static tref wff_lt(const tref*, tref) { return nullptr; }
};

} // namespace idni::tau_lang

// Constants are stored in a hashed variant, so every BA must be hashable.
template<>
struct std::hash<idni::tau_lang::ext_ba> {
	size_t operator()(const idni::tau_lang::ext_ba& x) const noexcept {
		return std::hash<bool>{}(x.value);
	}
};

#endif // __IDNI__TAU__TESTS__EXTERNAL_BA__EXT_BA_H__
