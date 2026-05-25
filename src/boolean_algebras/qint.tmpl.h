// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT_TMPL_H__

#include "boolean_algebras/qint.h" // Only for IDE resolution, not really needed.

namespace idni::tau_lang {

template <typename... BAs>
requires BAsPack<BAs...>
std::optional<qint> parse_qint_grammar(const std::string& src) {
	auto result = qint_parser::instance().parse(src.c_str(), src.size());
	if (!result.found) return std::nullopt;

	auto t = qint_parser::tree::traverser(result.get_shaped_tree2())
		| qint_parser::qint;
	if (!t.has_value()) return std::nullopt;

	return qint_eval_parse_tree(t);
}

template <typename... BAs>
requires BAsPack<BAs...>
std::optional<typename node<BAs...>::constant_with_type> parse_qint(
	const std::string& src)
{
	std::string s = src;
	s.erase(0, s.find_first_not_of(" \t\n\r"));
	auto last = s.find_last_not_of(" \t\n\r");
	if (last != std::string::npos) s = s.substr(0, last + 1);

	if (!s.empty() && s.front() == '{' && s.back() == '}')
		s = s.substr(1, s.size() - 2);
	s.erase(0, s.find_first_not_of(" \t\n\r"));
	last = s.find_last_not_of(" \t\n\r");
	if (last != std::string::npos) s = s.substr(0, last + 1);

	auto qval = parse_qint_grammar<BAs...>(s);
	if (!qval) return std::nullopt;

	return typename node<BAs...>::constant_with_type{
		std::variant<BAs...>{ *qval },
		qint_type<node<BAs...>>() };
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QINT_TMPL_H__
