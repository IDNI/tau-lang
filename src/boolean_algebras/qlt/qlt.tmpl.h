// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_TMPL_H__

#include "boolean_algebras/qlt/qlt.h" // Only for IDE resolution, not really needed.
#include "tau_diagnostics.h"
#include "boolean_algebras/ba_literal_parse.h"

namespace idni::tau_lang {

/**
 * @brief Parse @p src with the qlt grammar and evaluate it.
 * @return The value, or a `code::parse_error` when the grammar rejects
 * @p src or the parse does not evaluate to a qlt literal.
 */
template <typename... BAs>
requires BAsPack<BAs...>
result<qlt> parse_qlt_grammar(const std::string& src) {
	return parse_with<qlt_parser>(src, qlt_parser::qlt, "qlt",
		[](const qlt_parser::tree::traverser& t) {
			result<qlt> r;
			TAU_TRY_OR(auto qval, qlt_eval_parse_tree(t),
				code::parse_error, "Not a valid qlt literal");
			return r.with_value(qval);
		});
}

/**
 * @brief Parse a qlt constant source (braces stripped by
 * `strip_ba_constant_source`) into a pack constant typed `qlt`.
 * @return The constant and the qlt type tree, or the error of
 * `parse_qlt_grammar`.
 */
template <typename... BAs>
requires BAsPack<BAs...>
result<typename node<BAs...>::constant_with_type> parse_qlt(
	const std::string& src)
{
	result<typename node<BAs...>::constant_with_type> r;
	TAU_TRY(auto qval, parse_qlt_grammar<BAs...>(
		strip_ba_constant_source(src)));
	return r.with_value(typed_pack_constant<qlt, BAs...>(std::move(qval)));
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_TMPL_H__
