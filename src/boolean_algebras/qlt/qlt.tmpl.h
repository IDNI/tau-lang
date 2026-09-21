// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_TMPL_H__

#include "boolean_algebras/qlt/qlt.h" // Only for IDE resolution, not really needed.
#include "tau_diagnostics.h"

namespace idni::tau_lang {

template <typename... BAs>
requires BAsPack<BAs...>
result<qlt> parse_qlt_grammar(const std::string& src) {
	result<qlt> r;
	auto parsed = qlt_parser::instance().parse(src.c_str(), src.size());
	if (!parsed.found) {
		r.error(code::parse_error, parsed.parse_error
			.to_str(qlt_parser::error::info_lvl::INFO_BASIC));
		return r;
	}

	auto t = qlt_parser::tree::traverser(parsed.get_shaped_tree2())
		| qlt_parser::qlt;
	if (!t.has_value()) {
		r.error(code::parse_error, "No qlt value in parse tree");
		return r;
	}

	if (auto qval = qlt_eval_parse_tree(t); qval)
		return r.with_value(*qval);
	r.error(code::parse_error, "Not a valid qlt literal");
	return r;
}

template <typename... BAs>
requires BAsPack<BAs...>
result<typename node<BAs...>::constant_with_type> parse_qlt(
	const std::string& src)
{
	result<typename node<BAs...>::constant_with_type> r;
	std::string s = strip_ba_constant_source(src);

	TAU_TRY(auto qval, parse_qlt_grammar<BAs...>(s));

	return r.with_value(typename node<BAs...>::constant_with_type{
		std::variant<BAs...>{ qval },
		qlt_type<node<BAs...>>() });
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_TMPL_H__
