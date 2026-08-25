// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT_TMPL_H__

#include "boolean_algebras/qlt.h" // Only for IDE resolution, not really needed.

namespace idni::tau_lang {

template <typename... BAs>
requires BAsPack<BAs...>
std::optional<qlt> parse_qlt_grammar(const std::string& src) {
	auto result = qlt_parser::instance().parse(src.c_str(), src.size());
	if (!result.found) return std::nullopt;

	auto t = qlt_parser::tree::traverser(result.get_shaped_tree2())
		| qlt_parser::qlt;
	if (!t.has_value()) return std::nullopt;

	return qlt_eval_parse_tree(t);
}

template <typename... BAs>
requires BAsPack<BAs...>
std::optional<typename node<BAs...>::constant_with_type> parse_qlt(
	const std::string& src)
{
	std::string s = strip_ba_constant_source(src);

	auto qval = parse_qlt_grammar<BAs...>(s);
	if (!qval) return std::nullopt;

	return typename node<BAs...>::constant_with_type{
		std::variant<BAs...>{ *qval },
		qlt_type<node<BAs...>>() };
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT_TMPL_H__
