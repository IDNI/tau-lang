// This file is generated from a file parser/bdd.tgf by
//       https://github.com/IDNI/parser/tools/tgf
//
#ifndef __BDD_PARSER_H__
#define __BDD_PARSER_H__

#include "parser.h"

namespace bdd_parser_data {

using char_type     = char;
using terminal_type = char;

inline std::vector<std::string> symbol_names{
	"", "space", "alpha", "digit", "start", "_", "bdd", "group", "__E_bdd_0", "variable", 
	"__E_bdd_1", "__E___E_bdd_1_2", "negation", "__E_bdd_3", "disjunction", "__E_bdd_4", "conjunction", "__E_bdd_5", "__E___E_bdd_5_6", "exclusive_disjunction", 
	"__E_bdd_7", "__E___E_bdd_7_8", "one", "zero", "__E___9", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '(', ')', '\'', '|', '&', '^', '+', '1', 
	'0', 
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
		.to_trim_children_terminals = {
			12, 14, 16, 19, 22, 23
		},
		.trim_terminals = false,
		.to_inline = {
			{ 6, 7, 6 }
		},
		.inline_char_classes = true
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
//G0:   start(4)             => _(5) bdd(6) _(5).
	p(NT(4), (NT(5)+NT(6)+NT(5)));
//G1:   __E_bdd_0(8)         => '(' _(5) bdd(6) _(5) ')'.
	p(NT(8), (T(1)+NT(5)+NT(6)+NT(5)+T(2)));
//G2:   group(7)             => __E_bdd_0(8).
	p(NT(7), (NT(8)));
//G3:   bdd(6)               => group(7).
	p(NT(6), (NT(7)));
//G4:   __E___E_bdd_1_2(11)  => null.
	p(NT(11), (nul));
//G5:   __E___E_bdd_1_2(11)  => digit(3) __E___E_bdd_1_2(11).
	p(NT(11), (NT(3)+NT(11)));
//G6:   __E_bdd_1(10)        => alpha(2) __E___E_bdd_1_2(11).
	p(NT(10), (NT(2)+NT(11)));
//G7:   variable(9)          => __E_bdd_1(10).
	p(NT(9), (NT(10)));
//G8:   bdd(6)               => variable(9).
	p(NT(6), (NT(9)));
//G9:   __E_bdd_3(13)        => bdd(6) _(5) '\''.
	p(NT(13), (NT(6)+NT(5)+T(3)));
//G10:  negation(12)         => __E_bdd_3(13).
	p(NT(12), (NT(13)));
//G11:  bdd(6)               => negation(12).
	p(NT(6), (NT(12)));
//G12:  __E_bdd_4(15)        => bdd(6) _(5) '|' _(5) bdd(6).
	p(NT(15), (NT(6)+NT(5)+T(4)+NT(5)+NT(6)));
//G13:  disjunction(14)      => __E_bdd_4(15).
	p(NT(14), (NT(15)));
//G14:  bdd(6)               => disjunction(14).
	p(NT(6), (NT(14)));
//G15:  __E___E_bdd_5_6(18)  => _(5).
	p(NT(18), (NT(5)));
//G16:  __E___E_bdd_5_6(18)  => _(5) '&' _(5).
	p(NT(18), (NT(5)+T(5)+NT(5)));
//G17:  __E_bdd_5(17)        => bdd(6) __E___E_bdd_5_6(18) bdd(6).
	p(NT(17), (NT(6)+NT(18)+NT(6)));
//G18:  conjunction(16)      => __E_bdd_5(17).
	p(NT(16), (NT(17)));
//G19:  bdd(6)               => conjunction(16).
	p(NT(6), (NT(16)));
//G20:  __E___E_bdd_7_8(21)  => '^'.
	p(NT(21), (T(6)));
//G21:  __E___E_bdd_7_8(21)  => '+'.
	p(NT(21), (T(7)));
//G22:  __E_bdd_7(20)        => bdd(6) _(5) __E___E_bdd_7_8(21) _(5) bdd(6).
	p(NT(20), (NT(6)+NT(5)+NT(21)+NT(5)+NT(6)));
//G23:  exclusive_disjunction(19) => __E_bdd_7(20).
	p(NT(19), (NT(20)));
//G24:  bdd(6)               => exclusive_disjunction(19).
	p(NT(6), (NT(19)));
//G25:  one(22)              => '1'.
	p(NT(22), (T(8)));
//G26:  bdd(6)               => one(22).
	p(NT(6), (NT(22)));
//G27:  zero(23)             => '0'.
	p(NT(23), (T(9)));
//G28:  bdd(6)               => zero(23).
	p(NT(6), (NT(23)));
//G29:  __E___9(24)          => space(1) _(5).
	p(NT(24), (NT(1)+NT(5)));
//G30:  __E___9(24)          => null.
	p(NT(24), (nul));
//G31:  _(5)                 => __E___9(24).
	p(NT(5), (NT(24)));
	#undef T
	#undef NT
	return loaded = true, p;
}

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace bdd_parser_data

struct bdd_parser : public idni::parser<char, char> {
	enum nonterminal {
		nul, space, alpha, digit, start, _, bdd, group, __E_bdd_0, variable, 
		__E_bdd_1, __E___E_bdd_1_2, negation, __E_bdd_3, disjunction, __E_bdd_4, conjunction, __E_bdd_5, __E___E_bdd_5_6, exclusive_disjunction, 
		__E_bdd_7, __E___E_bdd_7_8, one, zero, __E___9, 
	};
	static bdd_parser& instance() {
		static bdd_parser inst;
		return inst;
	}
	bdd_parser() : idni::parser<char_type, terminal_type>(
		bdd_parser_data::grammar,
		bdd_parser_data::parser_options) {}
	size_t id(const std::basic_string<char_type>& name) {
		return bdd_parser_data::nts.get(name);
	}
	const std::basic_string<char_type>& name(size_t id) {
		return bdd_parser_data::nts.get(id);
	}
	symbol_type literal(const nonterminal& nt) {
		return symbol_type(nt, &bdd_parser_data::nts);
	}
};

#endif // __BDD_PARSER_H__
