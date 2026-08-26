// This file is generated from a file parser/hsb.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
#ifndef __HSB_PARSER_H__
#define __HSB_PARSER_H__

#include "parser.h"

namespace hsb_parser_data {

using char_type     = char;
using terminal_type = char;

inline static constexpr size_t nt_bits = 6;
inline const std::vector<std::string> symbol_names{
	"", "space", "digit", "_", "nat", "unum", "start", "hsb", "hsb_top", "hsb_bot", 
	"hsb_not", "__E_hsb_0", "hsb_paren", "__E_hsb_1", "hsb_and", "__E_hsb_2", "hsb_or", "__E_hsb_3", "hsb_hs", "halfspace", 
	"hs_leq", "__E_halfspace_4", "linexpr", "hs_lt", "__E_halfspace_5", "le_add", "__E_linexpr_6", "lterm", "le_sub", "__E_linexpr_7", 
	"le_lterm", "lt_coeff_var", "__E_lterm_8", "var", "lt_var_coeff", "__E_lterm_9", "lt_neg_coeff_var", "__E_lterm_10", "lt_neg_var_coeff", "__E_lterm_11", 
	"lt_neg_var", "__E_lterm_12", "lt_neg_const", "__E_lterm_13", "lt_var", "lt_const", "__E_nat_14", "__E___15", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', 't', 'o', 'p', 'b', 'm', '~', '(', ')', 
	'&', '|', '<', '=', '0', '+', '-', '*', 'x', '[', 
	']', '.', '/', 
};

inline ::idni::char_class_fns<terminal_type> char_classes =
	::idni::predefined_char_classes<char_type, terminal_type>({
		"space",
		"digit",
	}, nts);

inline struct ::idni::grammar<char_type, terminal_type>::options
	grammar_options
{
	.transform_negation = false,
	.auto_disambiguate = true,
	.shaping = {
		.to_trim = {
			3
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			4, 5
		},
		.inline_char_classes = true
	}
};

inline auto make_parser_options() {
	auto o = ::idni::default_parser_options<char_type, terminal_type>();
	return o;
}

inline ::idni::prods<char_type, terminal_type> start_symbol{ nts(6) };

idni::prods<char_type, terminal_type>& productions();

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace hsb_parser_data

struct hsb_parser_nonterminals {
	enum nonterminal {
		nul, space, digit, _, nat, unum, start, hsb, hsb_top, hsb_bot, 
		hsb_not, __E_hsb_0, hsb_paren, __E_hsb_1, hsb_and, __E_hsb_2, hsb_or, __E_hsb_3, hsb_hs, halfspace, 
		hs_leq, __E_halfspace_4, linexpr, hs_lt, __E_halfspace_5, le_add, __E_linexpr_6, lterm, le_sub, __E_linexpr_7, 
		le_lterm, lt_coeff_var, __E_lterm_8, var, lt_var_coeff, __E_lterm_9, lt_neg_coeff_var, __E_lterm_10, lt_neg_var_coeff, __E_lterm_11, 
		lt_neg_var, __E_lterm_12, lt_neg_const, __E_lterm_13, lt_var, lt_const, __E_nat_14, __E___15, 
	};
};

struct hsb_parser : public idni::parser<char, char>, public hsb_parser_nonterminals {
	static hsb_parser& instance() {
		static hsb_parser inst;
		return inst;
	}
	hsb_parser() : idni::parser<char_type, terminal_type>(
		hsb_parser_data::grammar,
		hsb_parser_data::make_parser_options()) {}
	size_t id(const std::basic_string<char_type>& name) {
		return hsb_parser_data::nts.get(name);
	}
	const std::basic_string<char_type>& name(size_t id) {
		return hsb_parser_data::nts.get(id);
	}
	symbol_type literal(const nonterminal& nt) {
		return symbol_type(nt, &hsb_parser_data::nts);
	}
};

#endif // __HSB_PARSER_H__
