// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file my_ba.h
 * @brief Skeleton Boolean algebra; copy this directory to start a new one.
 *
 * `my_ba` is a two-element Boolean algebra. It is the smallest thing that
 * satisfies the descriptor surface, so it shows the required shape without
 * any theory of its own. See docs/adding_base_bas.md.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS___TEMPLATE__MY_BA_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS___TEMPLATE__MY_BA_H__

#include <functional>
#include <optional>
#include <ostream>
#include <string>

#include "tau_tree.h"

namespace idni::tau_lang {

/** @brief Two-element carrier; replace with the algebra's own value type. */
struct my_ba {
	bool value = false;

	auto operator<=>(const my_ba&) const = default;
	// declaring operator==(bool) below would otherwise suppress this
	bool operator==(const my_ba&) const = default;
	/** @brief Core compares constants against plain truth values. */
	bool operator==(bool b) const { return value == b; }

	my_ba operator~() const { return { !value }; }
	my_ba operator&(const my_ba& o) const { return { value && o.value }; }
	my_ba operator|(const my_ba& o) const { return { value || o.value }; }
	my_ba operator^(const my_ba& o) const { return { value != o.value }; }
	my_ba operator+(const my_ba& o) const { return *this ^ o; }
};

/** @brief Every BA in a pack must be streamable; the tree printer needs it. */
inline std::ostream& operator<<(std::ostream& os, const my_ba& x) {
	return os << (x.value ? "1" : "0");
}

/** @brief Parse a `my_ba` constant; `{0}` and `{1}` are the only literals. */
template <typename... BAs>
std::optional<typename node<BAs...>::constant_with_type> parse_my_ba(
	const std::string& src);

} // namespace idni::tau_lang

#include "boolean_algebras/_template/my_ba.tmpl.h"
#include "boolean_algebras/_template/my_ba_descriptor.tmpl.h"

// Constants are stored in a hashed variant, so every BA must be hashable.
template<>
struct std::hash<idni::tau_lang::my_ba> {
	size_t operator()(const idni::tau_lang::my_ba& x) const noexcept {
		return std::hash<bool>{}(x.value);
	}
};

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS___TEMPLATE__MY_BA_H__
