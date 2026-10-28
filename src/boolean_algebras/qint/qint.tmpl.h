// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT__QINT_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT__QINT_TMPL_H__

#include "boolean_algebras/qint/qint.h" // Only for IDE resolution, not really needed.
#include "tau_diagnostics.h"
#include "boolean_algebras/ba_literal_parse.h"
#include "boolean_algebras/exact_rational.h"

namespace idni::tau_lang {

/**
 * @brief Parse a qint literal (without the `{ }` wrapper).
 * @param src Literal text.
 * @return The element; a parse error naming the cause otherwise, with a
 *         dedicated message for a bare number other than 0 / 1.
 */
template <typename... BAs>
requires BAsPack<BAs...>
result<qint> parse_qint_grammar(const std::string& src) {
	result<qint> r;
	// A bare number other than 0 / 1 reads as a point, which qint has not;
	// say so instead of reporting a bare syntax error.
	if (auto t = exact_rational::trim(src); t != "0" && t != "1") {
		if (qint_rational v; qint_detail::parse_endpoint(src, v)
			&& !v.is_inf())
		{
			r.error(code::parse_error, "`" + src + "` is not a qint "
				"element: qint has no points, write an interval "
				"such as [n, n+1), or 0 / 1 for bottom / top");
			return r;
		}
	}
	return parse_with<qint_parser>(src, qint_parser::qint, "qint",
		[](const qint_parser::tree::traverser& t) {
			result<qint> r;
			TAU_TRY_OR(auto qval, qint_eval_parse_tree(t),
				code::parse_error, "Not a valid qint literal");
			return r.with_value(qval);
		});
}

/**
 * @brief Parse a qint constant source and pair it with the qint type.
 * @param src Constant source; strip_ba_constant_source() removes its wrapper.
 * @return The constant and qint_type(); a parse error otherwise.
 */
template <typename... BAs>
requires BAsPack<BAs...>
result<typename node<BAs...>::constant_with_type> parse_qint(
	const std::string& src)
{
	result<typename node<BAs...>::constant_with_type> r;
	TAU_TRY(auto qval, parse_qint_grammar<BAs...>(
		strip_ba_constant_source(src)));
	return r.with_value(typed_pack_constant<qint, BAs...>(std::move(qval)));
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT__QINT_TMPL_H__
