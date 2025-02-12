// This file is generated from a file parser/bitvector.tgf by
//       https://github.com/IDNI/parser/tools/tgf
//
#ifndef __BITVECTOR_PARSER_H__
#define __BITVECTOR_PARSER_H__

#include "parser.h"

namespace bitvector_parser_data {

using char_type     = char;
using terminal_type = char;

inline std::vector<std::string> symbol_names{
	"", "space", "alpha", "digit", "start", "_", "bitvector", "uint", "int", "ulong", 
	"long", "bits", "sign", "minus", "plus", "__E_plus_0", "unsigned", "__E_uint_1", "__E_ulong_2", "bit", 
	"__E_bits_3", "__E_bits_4", "zero", "one", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '-', '+', 'l', 'u', 'b', '0', '1', 
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
		.trim_terminals = false,
		.inline_char_classes = false
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
//G0:   start(4)             => _(5) bitvector(6) _(5).
	p(NT(4), (NT(5)+NT(6)+NT(5)));
//G1:   bitvector(6)         => uint(7).
	p(NT(6), (NT(7)));
//G2:   bitvector(6)         => int(8).
	p(NT(6), (NT(8)));
//G3:   bitvector(6)         => ulong(9).
	p(NT(6), (NT(9)));
//G4:   bitvector(6)         => long(10).
	p(NT(6), (NT(10)));
//G5:   bitvector(6)         => bits(11).
	p(NT(6), (NT(11)));
//G6:   sign(12)             => minus(13).
	p(NT(12), (NT(13)));
//G7:   sign(12)             => plus(14).
	p(NT(12), (NT(14)));
//G8:   minus(13)            => '-'.
	p(NT(13), (T(1)));
//G9:   __E_plus_0(15)       => '+'.
	p(NT(15), (T(2)));
//G10:  __E_plus_0(15)       => null.
	p(NT(15), (nul));
//G11:  plus(14)             => __E_plus_0(15).
	p(NT(14), (NT(15)));
//G12:  int(8)               => sign(12) _(5) unsigned(16).
	p(NT(8), (NT(12)+NT(5)+NT(16)));
//G13:  long(10)             => sign(12) _(5) unsigned(16) _(5) 'l'.
	p(NT(10), (NT(12)+NT(5)+NT(16)+NT(5)+T(3)));
//G14:  __E_uint_1(17)       => plus(14).
	p(NT(17), (NT(14)));
//G15:  __E_uint_1(17)       => null.
	p(NT(17), (nul));
//G16:  uint(7)              => __E_uint_1(17) _(5) unsigned(16) _(5) 'u'.
	p(NT(7), (NT(17)+NT(5)+NT(16)+NT(5)+T(4)));
//G17:  __E_ulong_2(18)      => plus(14).
	p(NT(18), (NT(14)));
//G18:  __E_ulong_2(18)      => null.
	p(NT(18), (nul));
//G19:  ulong(9)             => __E_ulong_2(18) _(5) unsigned(16) _(5) 'u' 'l'.
	p(NT(9), (NT(18)+NT(5)+NT(16)+NT(5)+T(4)+T(3)));
//G20:  __E_bits_3(20)       => _(5) bit(19).
	p(NT(20), (NT(5)+NT(19)));
//G21:  __E_bits_4(21)       => null.
	p(NT(21), (nul));
//G22:  __E_bits_4(21)       => __E_bits_3(20) __E_bits_4(21).
	p(NT(21), (NT(20)+NT(21)));
//G23:  bits(11)             => bit(19) __E_bits_4(21) 'b'.
	p(NT(11), (NT(19)+NT(21)+T(5)));
//G24:  zero(22)             => '0'.
	p(NT(22), (T(6)));
//G25:  bit(19)              => zero(22).
	p(NT(19), (NT(22)));
//G26:  one(23)              => '1'.
	p(NT(23), (T(7)));
//G27:  bit(19)              => one(23).
	p(NT(19), (NT(23)));
	#undef T
	#undef NT
	return loaded = true, p;
}

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace bitvector_parser_data

struct bitvector_parser : public idni::parser<char, char> {
	enum nonterminal {
		nul, space, alpha, digit, start, _, bitvector, uint, int, ulong, 
		long, bits, sign, minus, plus, __E_plus_0, unsigned, __E_uint_1, __E_ulong_2, bit, 
		__E_bits_3, __E_bits_4, zero, one, 
	};
	static bitvector_parser& instance() {
		static bitvector_parser inst;
		return inst;
	}
	bitvector_parser() : idni::parser<char_type, terminal_type>(
		bitvector_parser_data::grammar,
		bitvector_parser_data::parser_options) {}
	size_t id(const std::basic_string<char_type>& name) {
		return bitvector_parser_data::nts.get(name);
	}
	const std::basic_string<char_type>& name(size_t id) {
		return bitvector_parser_data::nts.get(id);
	}
	symbol_type literal(const nonterminal& nt) {
		return symbol_type(nt, &bitvector_parser_data::nts);
	}
};

#endif // __BITVECTOR_PARSER_H__
