// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ba_literal_parse.h
 * @brief The steps a BA with a grammar of its own takes to read a constant:
 * run its generated parser, find the node of the literal, evaluate it, and
 * type the value as a constant of the pack; and the test that tells a
 * truncated literal from a bad one.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BA_LITERAL_PARSE_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BA_LITERAL_PARSE_H__

#include <string>
#include <string_view>
#include <utility>

#include "tau_diagnostics.h"
#include "tau_tree.h"
#include "ba_types.h"

namespace idni::tau_lang {

/**
 * @brief Parse @p src with @p Parser and evaluate the node @p root of the
 * parse tree with @p eval.
 * @param src The literal text.
 * @param root The nonterminal of the literal.
 * @param type_name The type name the errors give.
 * @param eval Called with a traverser at @p root; returns a `result`.
 * @return What @p eval returns; a parse error with the message of the parser
 * when the grammar rejects @p src, or naming @p type_name when the tree has
 * no @p root node.
 */
template <typename Parser, typename Eval>
auto parse_with(const std::string& src, typename Parser::nonterminal root,
	std::string_view type_name, Eval&& eval)
	-> decltype(eval(std::declval<const typename Parser::tree::traverser&>()))
{
	decltype(eval(std::declval<const typename Parser::tree::traverser&>())) r;
	auto parsed = Parser::instance().parse(src.c_str(), src.size());
	if (!parsed.found) {
		r.error(code::parse_error, parsed.parse_error
			.to_str(Parser::error::info_lvl::INFO_BASIC));
		return r;
	}
	auto t = typename Parser::tree::traverser(parsed.get_shaped_tree2())
		| root;
	if (!t.has_value()) {
		r.error(code::parse_error,
			"No " + std::string(type_name) + " value in parse tree");
		return r;
	}
	return eval(t);
}

/**
 * @brief `true` when @p src is a truncated literal of @p Parser's grammar,
 * not a bad one: the parse fails at the end of the text.
 *
 * Distinct from the parse failing, which cannot tell the two apart; the REPL
 * keeps reading on truncation and stops on a genuine syntax error.
 */
template <typename Parser>
bool literal_incomplete_with(const std::string& src) {
	auto parsed = Parser::instance().parse(src.c_str(), src.size());
	return !parsed.found && parsed.parse_error.at_eof();
}

/// @p value as a constant of the pack @p BAs, typed by @p BA's descriptor.
template <typename BA, typename... BAs>
typename node<BAs...>::constant_with_type typed_pack_constant(BA value) {
	return typename node<BAs...>::constant_with_type{
		std::variant<BAs...>{ std::move(value) },
		ba_type_tree<BA, node<BAs...>>() };
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BA_LITERAL_PARSE_H__
