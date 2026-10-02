// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT__QINT_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT__QINT_TMPL_H__

#include "boolean_algebras/qint/qint.h" // Only for IDE resolution, not really needed.
#include "tau_diagnostics.h"

namespace idni::tau_lang {

template <typename... BAs>
requires BAsPack<BAs...>
result<qint> parse_qint_grammar(const std::string& src) {
	result<qint> r;
	auto parsed = qint_parser::instance().parse(src.c_str(), src.size());
	if (!parsed.found) {
		// A bare number other than 0 / 1 reads as a point, which qint has
		// not; say so instead of reporting a bare syntax error.
		if (qint_rational v; qint_detail::parse_endpoint(src, v)
			&& !v.is_inf())
		{
			r.error(code::parse_error, "`" + src + "` is not a qint "
				"element: qint has no points, write an interval "
				"such as [n, n+1), or 0 / 1 for bottom / top");
			return r;
		}
		r.error(code::parse_error, parsed.parse_error
			.to_str(qint_parser::error::info_lvl::INFO_BASIC));
		return r;
	}

	auto t = qint_parser::tree::traverser(parsed.get_shaped_tree2())
		| qint_parser::qint;
	if (!t.has_value()) {
		r.error(code::parse_error, "No qint value in parse tree");
		return r;
	}

	TAU_TRY_OR(auto qval, qint_eval_parse_tree(t),
		code::parse_error, "Not a valid qint literal");
	return r.with_value(qval);
}

template <typename... BAs>
requires BAsPack<BAs...>
result<typename node<BAs...>::constant_with_type> parse_qint(
	const std::string& src)
{
	result<typename node<BAs...>::constant_with_type> r;
	std::string s = strip_ba_constant_source(src);

	TAU_TRY(auto qval, parse_qint_grammar<BAs...>(s));

	return r.with_value(typename node<BAs...>::constant_with_type{
		std::variant<BAs...>{ qval },
		qint_type<node<BAs...>>() });
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT__QINT_TMPL_H__
