// This file is generated from a file parser/sbf.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
#ifndef __SBF_PARSER_H__
#define __SBF_PARSER_H__

#include "parser.h"

namespace sbf_parser_data {

using char_type     = char;
using terminal_type = char;

inline static constexpr size_t nt_bits = 6;
inline const std::vector<std::string> symbol_names{
	"", "space", "alpha", "digit", "_", "variable", "sbf", "group", "negation_oprnd", "conjunction_nosep_1st_oprnd", 
	"conjunction_nosep", "conjunction", "start", "__E_sbf_0", "disjunction", "__E_sbf_1", "exclusive_disjunction", "__E_sbf_2", "__E___E_sbf_2_3", "__E_sbf_4", 
	"__E___E_sbf_4_5", "negation", "__E_sbf_6", "__E___E_sbf_6_7", "one", "zero", "__E_sbf_8", "__E___E_sbf_8_9", "__E___10", "__E_variable_11", 
	"__E_variable_12", "alnum", "__E_variable_13", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '(', ')', '|', '^', '+', '&', '\'', '1', 
	'0', '_', 
};

inline ::idni::char_class_fns<terminal_type> char_classes =
	::idni::predefined_char_classes<char_type, terminal_type>({
		"space",
		"alpha",
		"digit",
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
			5
		},
		.to_inline = {
			{ 6, 7, 6 },
			{ 8, 7, 6 },
			{ 9, 7, 6 },
			{ 10, 7, 11 }
		},
		.inline_char_classes = true
	},
	.enabled_guards = {
		"charvar"
	}
};

inline auto make_parser_options() {
	auto o = ::idni::default_parser_options<char_type, terminal_type>();
	return o;
}

inline ::idni::prods<char_type, terminal_type> start_symbol{ nts(12) };

idni::prods<char_type, terminal_type>& productions();

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace sbf_parser_data

struct sbf_parser_nonterminals {
	enum nonterminal {
		nul, space, alpha, digit, _, variable, sbf, group, negation_oprnd, conjunction_nosep_1st_oprnd, 
		conjunction_nosep, conjunction, start, __E_sbf_0, disjunction, __E_sbf_1, exclusive_disjunction, __E_sbf_2, __E___E_sbf_2_3, __E_sbf_4, 
		__E___E_sbf_4_5, negation, __E_sbf_6, __E___E_sbf_6_7, one, zero, __E_sbf_8, __E___E_sbf_8_9, __E___10, __E_variable_11, 
		__E_variable_12, alnum, __E_variable_13, 
	};
};

struct sbf_parser : public idni::parser<char, char>, public sbf_parser_nonterminals {
	static sbf_parser& instance() {
		static sbf_parser inst;
		return inst;
	}
	sbf_parser() : idni::parser<char_type, terminal_type>(
		sbf_parser_data::grammar,
		sbf_parser_data::make_parser_options()) {}
	size_t id(const std::basic_string<char_type>& name) {
		return sbf_parser_data::nts.get(name);
	}
	const std::basic_string<char_type>& name(size_t id) {
		return sbf_parser_data::nts.get(id);
	}
	symbol_type literal(const nonterminal& nt) {
		return symbol_type(nt, &sbf_parser_data::nts);
	}
};

#endif // __SBF_PARSER_H__
