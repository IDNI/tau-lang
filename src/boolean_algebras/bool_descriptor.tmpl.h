// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file bool_descriptor.tmpl.h
 * @brief Descriptor through which core reaches the classical Boolean algebra.
 *
 * `Bool` is not listed in `TAU_BAS` — it is not a configurable plugin — but it
 * still needs a descriptor, because packs such as `node<bv, Bool>` reach the
 * generic dispatcher like any other.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_DESCRIPTOR_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_DESCRIPTOR_TMPL_H__

#include "boolean_algebras/ba_descriptor_named.h"
#include "ba_types.h"

namespace idni::tau_lang {

/**
 * @brief Descriptor of the two-element Boolean algebra `Bool`.
 * @tparam PackBAs the base BAs of the node pack this descriptor serves.
 */
template <typename... PackBAs>
struct ba_descriptor<Bool, node<PackBAs...>>
	: ba_descriptor_named<ba_descriptor<Bool, node<PackBAs...>>,
		Bool, node<PackBAs...>>
{
	using node_t = node<PackBAs...>;
	using tau = tree<node_t>;
	/// The members derived from type_name (ba_descriptor_named).
	using named = ba_descriptor_named<ba_descriptor, Bool, node_t>;
	using named::bf_constant;

	/// @brief Spelling of the type in annotations (`:bool`).
	static constexpr const char* type_name = "bool";

	/** @brief Wins the default type over any base BA that scores higher. */
	static constexpr int default_type_priority = 1;

	/** @brief Two-element: the one algebra that is definitely not atomless. */
	static constexpr bool atomless = false;
	static constexpr bool non_aba_omcat = false;

	/** @brief Bool is a bit already. */
	static constexpr bool can_host_bool = true;

	/// @brief `true` when @p x is 1; exact, since Bool is two-element.
	static bool is_syntactic_one(const Bool& x) { return x.is_one(); }

	/// @brief `true` when @p x is 0; exact, since Bool is two-element.
	static bool is_syntactic_zero(const Bool& x) { return x.is_zero(); }

	/// @brief `true` when @p x is 1; never fails.
	static result<bool> is_one(const Bool& x) { return result<bool>{x.is_one()}; }

	/// @brief `true` when @p x is 0; never fails.
	static result<bool> is_zero(const Bool& x) { return result<bool>{x.is_zero()}; }

	/// @brief The literal spelling of 1 (`"1"`), whatever the type.
	static std::string literal_one(tref) { return "1"; }

	/// @brief The literal spelling of 0 (`"0"`), whatever the type.
	static std::string literal_zero(tref) { return "0"; }

	/**
	 * @brief The constant of this type holding @p value (0 or 1), as a bf.
	 * @return nullptr for any other value; the first argument is ignored.
	 */
	static tref value_constant(size_t, size_t value) {
		if (value > 1) return nullptr;
		return bf_constant(Bool(value != 0));
	}

	/// @brief @p x in normal form, through normalize_bool; never fails.
	static result<Bool> normalize(const Bool& x) {
		return result<Bool>{normalize_bool(x)};
	}

	/// @brief @p sym unchanged: Bool has no symbol simplification.
	static tref simplify_symbol(tref sym) { return sym; }

	/// @brief @p term unchanged: Bool has no term simplification.
	static result<tref> simplify_term(tref term) { return result<tref>{term}; }

	/**
	 * @brief Parses a Bool literal.
	 * @param src the literal text: `0`, `false`, `F`, `1`, `true` or `T`;
	 * the type tree argument is ignored.
	 * @return the constant with its type, or an error when @p src is not a
	 * Bool literal.
	 */
	static result<typename node_t::constant_with_type>
	parse(const std::string& src, tref)
	{
		return parse_bool<PackBAs...>(src);
	}
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_DESCRIPTOR_TMPL_H__
