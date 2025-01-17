// This file is generated from a file parser/sbf.tgf by
//       https://github.com/IDNI/parser/tools/tgf
//
#ifndef __SBF_PARSER_H__
#define __SBF_PARSER_H__

#include "parser.h"

namespace sbf_parser_data {

using char_type     = char;
using terminal_type = char;

inline std::vector<std::string> symbol_names{
	"", "space", "alpha", "digit", "start", "_", "sbf", "group", "__E_sbf_0", "variable", 
	"disjunction", "__E_sbf_1", "exclusive_disjunction", "__E_sbf_2", "__E___E_sbf_2_3", "conjunction", "__E_sbf_4", "__E___E_sbf_4_5", "negation", "__E_sbf_6", 
	"one", "zero", "__E___7", "__E_variable_8", "__E_variable_9", "alnum", "__E_variable_10", 
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
			5
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			9
		},
		.to_inline = {
			{ 6, 7, 6 }
		},
		.inline_char_classes = true
	},
	.enabled_guards = {
		"charvar"
	}
};

inline ::idni::parser<char_type, terminal_type>::options parser_options{
};

inline ::idni::prods<char_type, terminal_type> start_symbol{ nts(4) };

inline idni::prods<char_type, terminal_type>& productions() {
	static bool loaded = false;
	static idni::prods<char_type, terminal_type>
		p, nul(idni::lit<char_type, terminal_type>{});
	if (loaded) return p;
	#define  T(x) (idni::prods<char_type, terminal_type>{ terminals[x] })
	#define NT(x) (idni::prods<char_type, terminal_type>{ nts(x) })
//G0:   start(4)             => _(5) sbf(6) _(5).
	p(NT(4), (NT(5)+NT(6)+NT(5)));
//G1:   __E_sbf_0(8)         => '(' _(5) sbf(6) _(5) ')'.
	p(NT(8), (T(1)+NT(5)+NT(6)+NT(5)+T(2)));
//G2:   group(7)             => __E_sbf_0(8).
	p(NT(7), (NT(8)));
//G3:   sbf(6)               => group(7).
	p(NT(6), (NT(7)));
//G4:   sbf(6)               => variable(9).
	p(NT(6), (NT(9)));
//G5:   __E_sbf_1(11)        => sbf(6) _(5) '|' _(5) sbf(6).
	p(NT(11), (NT(6)+NT(5)+T(3)+NT(5)+NT(6)));
//G6:   disjunction(10)      => __E_sbf_1(11).
	p(NT(10), (NT(11)));
//G7:   sbf(6)               => disjunction(10).
	p(NT(6), (NT(10)));
//G8:   __E___E_sbf_2_3(14)  => '^'.
	p(NT(14), (T(4)));
//G9:   __E___E_sbf_2_3(14)  => '+'.
	p(NT(14), (T(5)));
//G10:  __E_sbf_2(13)        => sbf(6) _(5) __E___E_sbf_2_3(14) _(5) sbf(6).
	p(NT(13), (NT(6)+NT(5)+NT(14)+NT(5)+NT(6)));
//G11:  exclusive_disjunction(12) => __E_sbf_2(13).
	p(NT(12), (NT(13)));
//G12:  sbf(6)               => exclusive_disjunction(12).
	p(NT(6), (NT(12)));
//G13:  __E___E_sbf_4_5(17)  => _(5).
	p(NT(17), (NT(5)));
//G14:  __E___E_sbf_4_5(17)  => _(5) '&' _(5).
	p(NT(17), (NT(5)+T(6)+NT(5)));
//G15:  __E_sbf_4(16)        => sbf(6) __E___E_sbf_4_5(17) sbf(6).
	p(NT(16), (NT(6)+NT(17)+NT(6)));
//G16:  conjunction(15)      => __E_sbf_4(16).
	p(NT(15), (NT(16)));
//G17:  sbf(6)               => conjunction(15).
	p(NT(6), (NT(15)));
//G18:  __E_sbf_6(19)        => sbf(6) _(5) '\''.
	p(NT(19), (NT(6)+NT(5)+T(7)));
//G19:  negation(18)         => __E_sbf_6(19).
	p(NT(18), (NT(19)));
//G20:  sbf(6)               => negation(18).
	p(NT(6), (NT(18)));
//G21:  one(20)              => '1'.
	p(NT(20), (T(8)));
//G22:  sbf(6)               => one(20).
	p(NT(6), (NT(20)));
//G23:  zero(21)             => '0'.
	p(NT(21), (T(9)));
//G24:  sbf(6)               => zero(21).
	p(NT(6), (NT(21)));
//G25:  __E___7(22)          => space(1) _(5).
	p(NT(22), (NT(1)+NT(5)));
//G26:  __E___7(22)          => null.
	p(NT(22), (nul));
//G27:  _(5)                 => __E___7(22).
	p(NT(5), (NT(22)));
//G28:  __E_variable_8(23)   => null.
	p(NT(23), (nul));
//G29:  __E_variable_8(23)   => digit(3) __E_variable_8(23).
	p(NT(23), (NT(3)+NT(23)));
//G30:  variable(9)          => alpha(2) __E_variable_8(23).	 # guarded: charvar
	p(NT(9), (NT(2)+NT(23)));
	p.back().guard = "charvar";
//G31:  __E_variable_9(24)   => alnum(25).
	p(NT(24), (NT(25)));
//G32:  __E_variable_9(24)   => '_'.
	p(NT(24), (T(10)));
//G33:  __E_variable_10(26)  => null.
	p(NT(26), (nul));
//G34:  __E_variable_10(26)  => __E_variable_9(24) __E_variable_10(26).
	p(NT(26), (NT(24)+NT(26)));
//G35:  variable(9)          => alpha(2) __E_variable_10(26).	 # guarded: var
	p(NT(9), (NT(2)+NT(26)));
	p.back().guard = "var";
	#undef T
	#undef NT
	return loaded = true, p;
}

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace sbf_parser_data

struct sbf_parser : public idni::parser<char, char> {
	enum nonterminal {
		nul, space, alpha, digit, start, _, sbf, group, __E_sbf_0, variable, 
		disjunction, __E_sbf_1, exclusive_disjunction, __E_sbf_2, __E___E_sbf_2_3, conjunction, __E_sbf_4, __E___E_sbf_4_5, negation, __E_sbf_6, 
		one, zero, __E___7, __E_variable_8, __E_variable_9, alnum, __E_variable_10, 
	};
	static sbf_parser& instance() {
		static sbf_parser inst;
		return inst;
	}
	sbf_parser() : idni::parser<char_type, terminal_type>(
		sbf_parser_data::grammar,
		sbf_parser_data::parser_options) {}
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
