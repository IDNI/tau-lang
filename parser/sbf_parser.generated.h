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

inline idni::prods<char_type, terminal_type>& productions() {
	static bool loaded = false;
	static idni::prods<char_type, terminal_type>
		p, nul(idni::lit<char_type, terminal_type>{});
	if (loaded) return p;
	#define  T(x) (idni::prods<char_type, terminal_type>{ terminals[x] })
	#define NT(x) (idni::prods<char_type, terminal_type>{ nts(x) })
//G0:   start(12)            => _(4) sbf(6) _(4).
	p(NT(12), (NT(4)+NT(6)+NT(4)));
//G1:   __E_sbf_0(13)        => '(' _(4) sbf(6) _(4) ')'.
	p(NT(13), (T(1)+NT(4)+NT(6)+NT(4)+T(2)));
//G2:   group(7)             => __E_sbf_0(13).
	p(NT(7), (NT(13)));
//G3:   sbf(6)               => group(7).
	p(NT(6), (NT(7)));
//G4:   sbf(6)               => variable(5).
	p(NT(6), (NT(5)));
//G5:   __E_sbf_1(15)        => sbf(6) _(4) '|' _(4) sbf(6).
	p(NT(15), (NT(6)+NT(4)+T(3)+NT(4)+NT(6)));
//G6:   disjunction(14)      => __E_sbf_1(15).
	p(NT(14), (NT(15)));
//G7:   sbf(6)               => disjunction(14).
	p(NT(6), (NT(14)));
//G8:   __E___E_sbf_2_3(18)  => '^'.
	p(NT(18), (T(4)));
//G9:   __E___E_sbf_2_3(18)  => '+'.
	p(NT(18), (T(5)));
//G10:  __E_sbf_2(17)        => sbf(6) _(4) __E___E_sbf_2_3(18) _(4) sbf(6).
	p(NT(17), (NT(6)+NT(4)+NT(18)+NT(4)+NT(6)));
//G11:  exclusive_disjunction(16) => __E_sbf_2(17).
	p(NT(16), (NT(17)));
//G12:  sbf(6)               => exclusive_disjunction(16).
	p(NT(6), (NT(16)));
//G13:  __E___E_sbf_4_5(20)  => _(4) '&' _(4).
	p(NT(20), (NT(4)+T(6)+NT(4)));
//G14:  __E___E_sbf_4_5(20)  => space(1) _(4).
	p(NT(20), (NT(1)+NT(4)));
//G15:  __E_sbf_4(19)        => sbf(6) __E___E_sbf_4_5(20) sbf(6).
	p(NT(19), (NT(6)+NT(20)+NT(6)));
//G16:  conjunction(11)      => __E_sbf_4(19).
	p(NT(11), (NT(19)));
//G17:  sbf(6)               => conjunction(11).
	p(NT(6), (NT(11)));
//G18:  __E___E_sbf_6_7(23)  => group(7).
	p(NT(23), (NT(7)));
//G19:  __E___E_sbf_6_7(23)  => variable(5).
	p(NT(23), (NT(5)));
//G20:  __E___E_sbf_6_7(23)  => negation(21).
	p(NT(23), (NT(21)));
//G21:  __E___E_sbf_6_7(23)  => one(24).
	p(NT(23), (NT(24)));
//G22:  __E___E_sbf_6_7(23)  => zero(25).
	p(NT(23), (NT(25)));
//G23:  negation_oprnd(8)    => __E___E_sbf_6_7(23).
	p(NT(8), (NT(23)));
//G24:  __E_sbf_6(22)        => negation_oprnd(8) _(4) '\''.
	p(NT(22), (NT(8)+NT(4)+T(7)));
//G25:  negation(21)         => __E_sbf_6(22).
	p(NT(21), (NT(22)));
//G26:  sbf(6)               => negation(21).
	p(NT(6), (NT(21)));
//G27:  __E___E_sbf_8_9(27)  => group(7).
	p(NT(27), (NT(7)));
//G28:  __E___E_sbf_8_9(27)  => variable(5).
	p(NT(27), (NT(5)));
//G29:  __E___E_sbf_8_9(27)  => disjunction(14).
	p(NT(27), (NT(14)));
//G30:  __E___E_sbf_8_9(27)  => exclusive_disjunction(16).
	p(NT(27), (NT(16)));
//G31:  __E___E_sbf_8_9(27)  => negation(21).
	p(NT(27), (NT(21)));
//G32:  conjunction_nosep_1st_oprnd(9) => __E___E_sbf_8_9(27).
	p(NT(9), (NT(27)));
//G33:  __E_sbf_8(26)        => conjunction_nosep_1st_oprnd(9) sbf(6).
	p(NT(26), (NT(9)+NT(6)));
//G34:  conjunction_nosep(10) => __E_sbf_8(26).
	p(NT(10), (NT(26)));
//G35:  sbf(6)               => conjunction_nosep(10).
	p(NT(6), (NT(10)));
//G36:  one(24)              => '1'.
	p(NT(24), (T(8)));
//G37:  sbf(6)               => one(24).
	p(NT(6), (NT(24)));
//G38:  zero(25)             => '0'.
	p(NT(25), (T(9)));
//G39:  sbf(6)               => zero(25).
	p(NT(6), (NT(25)));
//G40:  __E___10(28)         => space(1) _(4).
	p(NT(28), (NT(1)+NT(4)));
//G41:  __E___10(28)         => null.
	p(NT(28), (nul));
//G42:  _(4)                 => __E___10(28).
	p(NT(4), (NT(28)));
//G43:  __E_variable_11(29)  => null.
	p(NT(29), (nul));
//G44:  __E_variable_11(29)  => digit(3) __E_variable_11(29).
	p(NT(29), (NT(3)+NT(29)));
//G45:  variable(5)          => alpha(2) __E_variable_11(29).	 # guarded: charvar
	p(NT(5), (NT(2)+NT(29)));
	p.back().guard = "charvar";
//G46:  __E_variable_12(30)  => alnum(31).
	p(NT(30), (NT(31)));
//G47:  __E_variable_12(30)  => '_'.
	p(NT(30), (T(10)));
//G48:  __E_variable_13(32)  => null.
	p(NT(32), (nul));
//G49:  __E_variable_13(32)  => __E_variable_12(30) __E_variable_13(32).
	p(NT(32), (NT(30)+NT(32)));
//G50:  variable(5)          => alpha(2) __E_variable_13(32).	 # guarded: var
	p(NT(5), (NT(2)+NT(32)));
	p.back().guard = "var";
	#undef T
	#undef NT
	return loaded = true, p;
}

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
