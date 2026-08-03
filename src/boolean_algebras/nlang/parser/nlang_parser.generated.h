// This file is generated from a file parser/nlang.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
#ifndef __NLANG_PARSER_H__
#define __NLANG_PARSER_H__

#include "parser.h"

namespace nlang_parser_data {

using char_type     = char;
using terminal_type = char;

inline static constexpr size_t nt_bits = 5;
inline const std::vector<std::string> symbol_names{
	"", "space", "printable", "start", "_", "formula", "nlang_bot", "nlang_top", "nlang_not", "__E_formula_0", 
	"nlang_and", "__E_formula_1", "nlang_or", "__E_formula_2", "nlang_atom", "atom", "atom_char", "__E_atom_3", "__E___4", "__N_0", 
	"__N_1", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', 'n', 'o', 't', 'h', 'i', 'g', 'e', 'v', 
	'r', 'y', '(', ')', 'a', 'd', 
};

inline ::idni::char_class_fns<terminal_type> char_classes =
	::idni::predefined_char_classes<char_type, terminal_type>({
		"space",
		"printable",
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
			15, 16
		},
		.inline_char_classes = true
	}
};

inline ::idni::parser<char_type, terminal_type>::options parser_options{
};

inline ::idni::prods<char_type, terminal_type> start_symbol{ nts(3) };

inline idni::prods<char_type, terminal_type>& productions() {
	static bool loaded = false;
	static idni::prods<char_type, terminal_type>
		p, nul(idni::lit<char_type, terminal_type>{});
	if (loaded) return p;
	#define  T(x) (idni::prods<char_type, terminal_type>{ terminals[x] })
	#define NT(x) (idni::prods<char_type, terminal_type>{ nts(x) })
//G0:   start(3)             => _(4) formula(5) _(4).
	p(NT(3), (NT(4)+NT(5)+NT(4)));
//G1:   nlang_bot(6)         => 'n' 'o' 't' 'h' 'i' 'n' 'g'.
	p(NT(6), (T(1)+T(2)+T(3)+T(4)+T(5)+T(1)+T(6)));
//G2:   formula(5)           => nlang_bot(6).
	p(NT(5), (NT(6)));
//G3:   nlang_top(7)         => 'e' 'v' 'e' 'r' 'y' 't' 'h' 'i' 'n' 'g'.
	p(NT(7), (T(7)+T(8)+T(7)+T(9)+T(10)+T(3)+T(4)+T(5)+T(1)+T(6)));
//G4:   formula(5)           => nlang_top(7).
	p(NT(5), (NT(7)));
//G5:   __E_formula_0(9)     => 'n' 'o' 't' _(4) '(' _(4) formula(5) _(4) ')'.
	p(NT(9), (T(1)+T(2)+T(3)+NT(4)+T(11)+NT(4)+NT(5)+NT(4)+T(12)));
//G6:   nlang_not(8)         => __E_formula_0(9).
	p(NT(8), (NT(9)));
//G7:   formula(5)           => nlang_not(8).
	p(NT(5), (NT(8)));
//G8:   __E_formula_1(11)    => '(' _(4) formula(5) _(4) ')' _(4) 'a' 'n' 'd' _(4) '(' _(4) formula(5) _(4) ')'.
	p(NT(11), (T(11)+NT(4)+NT(5)+NT(4)+T(12)+NT(4)+T(13)+T(1)+T(14)+NT(4)+T(11)+NT(4)+NT(5)+NT(4)+T(12)));
//G9:   nlang_and(10)        => __E_formula_1(11).
	p(NT(10), (NT(11)));
//G10:  formula(5)           => nlang_and(10).
	p(NT(5), (NT(10)));
//G11:  __E_formula_2(13)    => '(' _(4) formula(5) _(4) ')' _(4) 'o' 'r' _(4) '(' _(4) formula(5) _(4) ')'.
	p(NT(13), (T(11)+NT(4)+NT(5)+NT(4)+T(12)+NT(4)+T(2)+T(9)+NT(4)+T(11)+NT(4)+NT(5)+NT(4)+T(12)));
//G12:  nlang_or(12)         => __E_formula_2(13).
	p(NT(12), (NT(13)));
//G13:  formula(5)           => nlang_or(12).
	p(NT(5), (NT(12)));
//G14:  nlang_atom(14)       => atom(15).
	p(NT(14), (NT(15)));
//G15:  formula(5)           => nlang_atom(14).
	p(NT(5), (NT(14)));
//G16:  __E_atom_3(17)       => atom_char(16).
	p(NT(17), (NT(16)));
//G17:  __E_atom_3(17)       => atom_char(16) __E_atom_3(17).
	p(NT(17), (NT(16)+NT(17)));
//G18:  atom(15)             => __E_atom_3(17).
	p(NT(15), (NT(17)));
//G19:  __N_0(19)            => '('.
	p(NT(19), (T(11)));
//G20:  __N_1(20)            => ')'.
	p(NT(20), (T(12)));
//G21:  atom_char(16)        => ~( __N_0(19) ) & ~( __N_1(20) ) & printable(2).	 # conjunctive
	p(NT(16), ~(NT(19)) & ~(NT(20)) & (NT(2)));
//G22:  __E___4(18)          => space(1) _(4).
	p(NT(18), (NT(1)+NT(4)));
//G23:  __E___4(18)          => null.
	p(NT(18), (nul));
//G24:  _(4)                 => __E___4(18).
	p(NT(4), (NT(18)));
	#undef T
	#undef NT
	return loaded = true, p;
}

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace nlang_parser_data

struct nlang_parser_nonterminals {
	enum nonterminal {
		nul, space, printable, start, _, formula, nlang_bot, nlang_top, nlang_not, __E_formula_0, 
		nlang_and, __E_formula_1, nlang_or, __E_formula_2, nlang_atom, atom, atom_char, __E_atom_3, __E___4, __N_0, 
		__N_1, 
	};
};

struct nlang_parser : public idni::parser<char, char>, public nlang_parser_nonterminals {
	static nlang_parser& instance() {
		static nlang_parser inst;
		return inst;
	}
	nlang_parser() : idni::parser<char_type, terminal_type>(
		nlang_parser_data::grammar,
		nlang_parser_data::parser_options) {}
	size_t id(const std::basic_string<char_type>& name) {
		return nlang_parser_data::nts.get(name);
	}
	const std::basic_string<char_type>& name(size_t id) {
		return nlang_parser_data::nts.get(id);
	}
	symbol_type literal(const nonterminal& nt) {
		return symbol_type(nt, &nlang_parser_data::nts);
	}
};

#endif // __NLANG_PARSER_H__
