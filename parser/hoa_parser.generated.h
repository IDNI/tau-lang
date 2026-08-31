// This file is generated from a file parser/hoa.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
#ifndef __HOA_PARSER_H__
#define __HOA_PARSER_H__

#include "parser.h"

namespace hoa_parser_data {

using char_type     = char;
using terminal_type = char;

inline static constexpr size_t nt_bits = 7;
inline const std::vector<std::string> symbol_names{
	"", "space", "blank", "digit", "printable", "b", "b1", "num", "qchar", "nqchar", 
	"acc", "achar", "gdisj", "gconj", "glit", "gnum", "gb", "start", "hline", "__E_start_0", 
	"body", "states_line", "start_line", "ap_line", "skip_line", "__E_ap_line_1", "ap_name", "__E_ap_line_2", "ap_quoted", "__E_ap_name_3", 
	"qrun", "ap_bare", "nrun1", "__E_qrun_4", "belem", "__E_body_5", "__E_body_6", "end_part", "state_line", "edge_line", 
	"__E_state_line_7", "guard", "__E_edge_line_8", "arun", "__E_arun_9", "__E_guard_10", "__E_glit_11", "__E_gb_12", "tail", "__E_tail_13", 
	"any", "__E_num_14", "__E_achar_15", "lchar", "__E_skip_line_16", "sfirst", "lrun", "__E_lrun_17", "__E_b_18", "__N_0", 
	"__N_1", "__N_2", "__N_3", "__N_4", "__N_5", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', 'S', 't', 'a', 'e', 's', ':', '\n', 'r', 
	'A', 'P', '"', '-', 'B', 'O', 'D', 'Y', '[', ']', 
	'{', '}', '|', '&', 'f', '!', '(', ')', 'E', 'N', 
	' ', '\t', '\r', 
};

inline ::idni::char_class_fns<terminal_type> char_classes =
	::idni::predefined_char_classes<char_type, terminal_type>({
		"space",
		"blank",
		"digit",
		"printable",
	}, nts);

inline struct ::idni::grammar<char_type, terminal_type>::options
	grammar_options
{
	.transform_negation = false,
	.auto_disambiguate = true,
	.shaping = {
		.to_trim = {
			5, 6
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			7, 8, 9, 10, 11, 12, 13, 14, 15, 16
		},
		.inline_char_classes = true
	}
};

inline auto make_parser_options() {
	auto o = ::idni::default_parser_options<char_type, terminal_type>();
	return o;
}

inline ::idni::prods<char_type, terminal_type> start_symbol{ nts(17) };

idni::prods<char_type, terminal_type>& productions();

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace hoa_parser_data

struct hoa_parser_nonterminals {
	enum nonterminal {
		nul, space, blank, digit, printable, b, b1, num, qchar, nqchar, 
		acc, achar, gdisj, gconj, glit, gnum, gb, start, hline, __E_start_0, 
		body, states_line, start_line, ap_line, skip_line, __E_ap_line_1, ap_name, __E_ap_line_2, ap_quoted, __E_ap_name_3, 
		qrun, ap_bare, nrun1, __E_qrun_4, belem, __E_body_5, __E_body_6, end_part, state_line, edge_line, 
		__E_state_line_7, guard, __E_edge_line_8, arun, __E_arun_9, __E_guard_10, __E_glit_11, __E_gb_12, tail, __E_tail_13, 
		any, __E_num_14, __E_achar_15, lchar, __E_skip_line_16, sfirst, lrun, __E_lrun_17, __E_b_18, __N_0, 
		__N_1, __N_2, __N_3, __N_4, __N_5, 
	};
};

struct hoa_parser : public idni::parser<char, char>, public hoa_parser_nonterminals {
	static hoa_parser& instance() {
		static hoa_parser inst;
		return inst;
	}
	hoa_parser() : idni::parser<char_type, terminal_type>(
		hoa_parser_data::grammar,
		hoa_parser_data::make_parser_options()) {}
	size_t id(const std::basic_string<char_type>& name) {
		return hoa_parser_data::nts.get(name);
	}
	const std::basic_string<char_type>& name(size_t id) {
		return hoa_parser_data::nts.get(id);
	}
	symbol_type literal(const nonterminal& nt) {
		return symbol_type(nt, &hoa_parser_data::nts);
	}
};

#endif // __HOA_PARSER_H__
