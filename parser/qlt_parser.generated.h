// This file is generated from a file parser/qlt.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
#ifndef __QLT_PARSER_H__
#define __QLT_PARSER_H__

#include "parser.h"

namespace qlt_parser_data {

using char_type     = char;
using terminal_type = char;

inline static constexpr size_t nt_bits = 5;
inline const std::vector<std::string> symbol_names{
	"", "space", "digit", "alpha", "_", "endpoint", "ep_char", "singleton", "start", "qlt", 
	"qlt_top", "qlt_bot", "qlt_singleton", "qlt_single", "interval", "qlt_union", "__E_qlt_0", "interval_cc", "interval_co", "interval_oc", 
	"interval_oo", "__E_singleton_1", "__E_endpoint_2", "__E___3", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', 't', 'o', 'p', 'b', 'm', '|', ']', '[', 
	',', ')', '(', '+', '-', '.', '/', '_', 
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

} // namespace qlt_parser_data

struct qlt_parser_nonterminals {
	enum nonterminal {
		nul, space, digit, alpha, _, endpoint, ep_char, singleton, start, qlt, 
		qlt_top, qlt_bot, qlt_singleton, qlt_single, interval, qlt_union, __E_qlt_0, interval_cc, interval_co, interval_oc, 
		interval_oo, __E_singleton_1, __E_endpoint_2, __E___3, 
	};
};

struct qlt_parser : public idni::parser<char, char>, public qlt_parser_nonterminals {
	static qlt_parser& instance() {
		static qlt_parser inst;
		return inst;
	}
	qlt_parser() : idni::parser<char_type, terminal_type>(
		qlt_parser_data::grammar,
		qlt_parser_data::make_parser_options()) {}
	size_t id(const std::basic_string<char_type>& name) {
		return qlt_parser_data::nts.get(name);
	}
	const std::basic_string<char_type>& name(size_t id) {
		return qlt_parser_data::nts.get(id);
	}
	symbol_type literal(const nonterminal& nt) {
		return symbol_type(nt, &qlt_parser_data::nts);
	}
};

#endif // __QLT_PARSER_H__
