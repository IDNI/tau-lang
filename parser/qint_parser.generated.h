// This file is generated from a file parser/qint.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
#ifndef __QINT_PARSER_H__
#define __QINT_PARSER_H__

#include "parser.h"

namespace qint_parser_data {

using char_type     = char;
using terminal_type = char;

inline static constexpr size_t nt_bits = 5;
inline const std::vector<std::string> symbol_names{
	"", "space", "digit", "alpha", "_", "endpoint", "ep_char", "integer", "start", "qint", 
	"qint_top", "qint_bot", "qint_integer", "qint_single", "interval", "qint_union", "__E_qint_0", "__E_endpoint_1", "__E_integer_2", "__E_integer_3", 
	"__E___4", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', 't', 'o', 'p', 'b', 'm', '|', '[', ',', 
	')', '+', '-', '.', '/', 
};

inline ::idni::char_class_fns<terminal_type> char_classes =
	::idni::predefined_char_classes<char_type, terminal_type>({
		"space",
		"digit",
		"alpha",
	}, nts);

inline struct ::idni::grammar<char_type, terminal_type>::options
	grammar_options
{
	.transform_negation = false,
	.auto_disambiguate = true,
	.shaping = {
		.to_trim = {
			4
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			5, 6, 7
		},
		.inline_char_classes = true
	}
};

inline auto make_parser_options() {
	auto o = ::idni::default_parser_options<char_type, terminal_type>();
	return o;
}

inline ::idni::prods<char_type, terminal_type> start_symbol{ nts(8) };

idni::prods<char_type, terminal_type>& productions();

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace qint_parser_data

struct qint_parser_nonterminals {
	enum nonterminal {
		nul, space, digit, alpha, _, endpoint, ep_char, integer, start, qint, 
		qint_top, qint_bot, qint_integer, qint_single, interval, qint_union, __E_qint_0, __E_endpoint_1, __E_integer_2, __E_integer_3, 
		__E___4, 
	};
};

struct qint_parser : public idni::parser<char, char>, public qint_parser_nonterminals {
	static qint_parser& instance() {
		static qint_parser inst;
		return inst;
	}
	qint_parser() : idni::parser<char_type, terminal_type>(
		qint_parser_data::grammar,
		qint_parser_data::make_parser_options()) {}
	size_t id(const std::basic_string<char_type>& name) {
		return qint_parser_data::nts.get(name);
	}
	const std::basic_string<char_type>& name(size_t id) {
		return qint_parser_data::nts.get(id);
	}
	symbol_type literal(const nonterminal& nt) {
		return symbol_type(nt, &qint_parser_data::nts);
	}
};

#endif // __QINT_PARSER_H__
