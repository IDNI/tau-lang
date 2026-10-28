// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS___TEMPLATE__MY_BA_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS___TEMPLATE__MY_BA_TMPL_H__

#include "ba_types.h"
#include "boolean_algebras/ba_literal_parse.h"
#include "tau_diagnostics.h"

namespace idni::tau_lang {

/** @brief Type tree naming this algebra in a type annotation (`x:my_ba`). */
// Derived from the descriptor, which builds it from its own type_name. Keep it
// if your own code wants the short spelling; core never calls it.
template <NodeType node>
tref my_ba_type() {
	return ba_type_tree<my_ba, node>();
}

// A refusal reports why through the returned result instead of an empty
// optional -- see docs/adding_base_bas.md.
template <typename... BAs>
result<typename node<BAs...>::constant_with_type> parse_my_ba(
	const std::string& src)
{
	result<typename node<BAs...>::constant_with_type> r;
	if (src != "0" && src != "1") {
		r.error(code::parse_error, "not a valid my_ba literal",
			{{label::value, src}});
		return r;
	}
	// A BA with a grammar of its own reads src with parse_with instead.
	return r.with_value(typed_pack_constant<my_ba, BAs...>(
		my_ba{ src == "1" }));
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS___TEMPLATE__MY_BA_TMPL_H__
