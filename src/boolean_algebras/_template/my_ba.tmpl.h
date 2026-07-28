// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS___TEMPLATE__MY_BA_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS___TEMPLATE__MY_BA_TMPL_H__

#include "ba_types.h"

namespace idni::tau_lang {

/** @brief Type tree naming this algebra in a type annotation (`x:my_ba`). */
template <NodeType node>
tref my_ba_type() {
	using tau = tree<node>;
	return tau::get(tau::typed, tau::get(tau::type, "my_ba"));
}

template <typename... BAs>
std::optional<typename node<BAs...>::constant_with_type> parse_my_ba(
	const std::string& src)
{
	using node_t = node<BAs...>;
	if (src != "0" && src != "1") return std::nullopt;
	return typename node_t::constant_with_type{
		typename node_t::constant{ my_ba{ src == "1" } },
		my_ba_type<node_t>() };
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS___TEMPLATE__MY_BA_TMPL_H__
