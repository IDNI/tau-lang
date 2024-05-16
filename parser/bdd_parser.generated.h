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
	"", "space", "alpha", "alnum", "start", "_", "bdd", "__", "__E___0", "disjunction", 
	"bdd2", "conjunction", "bdd3", "exclusive_or", "bdd4", "negation", "bdd5", "lit", "__E_exclusive_or_1", "var", 
	"T", "F", "__E_var_2", "__E_var_3", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '(', ')', '|', '&', '^', '+', '\'', '1', 
	'0', 
};

inline ::idni::char_class_fns<terminal_type> char_classes =
	::idni::predefined_char_classes<char_type, terminal_type>({
		"space",
		"alpha",
		"alnum",
	}, nts);

inline struct ::idni::grammar<char_type, terminal_type>::options
	grammar_options
{
	.transform_negation = false,
	.auto_disambiguate = true,
	.shaping = {
		.trim_terminals = false,
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
//G1:   __(7)                => space(1) _(5).
	p(NT(7), (NT(1)+NT(5)));
//G2:   __E___0(8)           => __(7).
	p(NT(8), (NT(7)));
//G3:   __E___0(8)           => null.
	p(NT(8), (nul));
//G4:   _(5)                 => __E___0(8).
	p(NT(5), (NT(8)));
//G5:   bdd(6)               => disjunction(9).
	p(NT(6), (NT(9)));
//G6:   bdd(6)               => bdd2(10).
	p(NT(6), (NT(10)));
//G7:   bdd2(10)             => conjunction(11).
	p(NT(10), (NT(11)));
//G8:   bdd2(10)             => bdd3(12).
	p(NT(10), (NT(12)));
//G9:   bdd3(12)             => exclusive_or(13).
	p(NT(12), (NT(13)));
//G10:  bdd3(12)             => bdd4(14).
	p(NT(12), (NT(14)));
//G11:  bdd4(14)             => negation(15).
	p(NT(14), (NT(15)));
//G12:  bdd4(14)             => bdd5(16).
	p(NT(14), (NT(16)));
//G13:  bdd5(16)             => lit(17).
	p(NT(16), (NT(17)));
//G14:  bdd5(16)             => '(' _(5) bdd(6) _(5) ')'.
	p(NT(16), (T(1)+NT(5)+NT(6)+NT(5)+T(2)));
//G15:  disjunction(9)       => bdd(6) _(5) '|' _(5) bdd2(10).
	p(NT(9), (NT(6)+NT(5)+T(3)+NT(5)+NT(10)));
//G16:  conjunction(11)      => bdd2(10) _(5) '&' _(5) bdd3(12).
	p(NT(11), (NT(10)+NT(5)+T(4)+NT(5)+NT(12)));
//G17:  conjunction(11)      => bdd2(10) __(7) bdd3(12).
	p(NT(11), (NT(10)+NT(7)+NT(12)));
//G18:  __E_exclusive_or_1(18) => '^'.
	p(NT(18), (T(5)));
//G19:  __E_exclusive_or_1(18) => '+'.
	p(NT(18), (T(6)));
//G20:  exclusive_or(13)     => bdd3(12) _(5) __E_exclusive_or_1(18) _(5) bdd4(14).
	p(NT(13), (NT(12)+NT(5)+NT(18)+NT(5)+NT(14)));
//G21:  negation(15)         => bdd4(14) _(5) '\''.
	p(NT(15), (NT(14)+NT(5)+T(7)));
//G22:  lit(17)              => var(19).
	p(NT(17), (NT(19)));
//G23:  lit(17)              => T(20).
	p(NT(17), (NT(20)));
//G24:  lit(17)              => F(21).
	p(NT(17), (NT(21)));
//G25:  __E_var_2(22)        => alnum(3).
	p(NT(22), (NT(3)));
//G26:  __E_var_3(23)        => null.
	p(NT(23), (nul));
//G27:  __E_var_3(23)        => __E_var_2(22) __E_var_3(23).
	p(NT(23), (NT(22)+NT(23)));
//G28:  var(19)              => alpha(2) __E_var_3(23).
	p(NT(19), (NT(2)+NT(23)));
//G29:  T(20)                => '1'.
	p(NT(20), (T(8)));
//G30:  F(21)                => '0'.
	p(NT(21), (T(9)));
	#undef T
	#undef NT
	return loaded = true, p;
}

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace bdd_parser_data

struct bdd_parser : public idni::parser<char, char> {
	enum nonterminal {
		nul, space, alpha, alnum, start, _, bdd, __, __E___0, disjunction, 
		bdd2, conjunction, bdd3, exclusive_or, bdd4, negation, bdd5, lit, __E_exclusive_or_1, var, 
		T, F, __E_var_2, __E_var_3, 
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
