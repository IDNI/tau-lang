// This file is generated from a file parser/z3.tgf by
//       https://github.com/IDNI/parser/tools/tgf
//
#ifndef __Z3_PARSER_H__
#define __Z3_PARSER_H__

#include "parser.h"

namespace z3_parser_data {

using char_type     = char;
using terminal_type = char;

inline std::vector<std::string> symbol_names{
	"", "space", "alpha", "digit", "start", "_", "bitvector", "_uint", "_int", "_ulong", 
	"_long", "_bits", "sign", "minus", "plus", "__E_plus_0", "_unsigned", "__E__uint_1", "__E__ulong_2", "__E__unsigned_3", 
	"_digit", "__E__unsigned_4", "_bit", "__E__bits_5", "__E__bits_6", "zero", "one", 
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
//G1:   bitvector(6)         => _uint(7).
	p(NT(6), (NT(7)));
//G2:   bitvector(6)         => _int(8).
	p(NT(6), (NT(8)));
//G3:   bitvector(6)         => _ulong(9).
	p(NT(6), (NT(9)));
//G4:   bitvector(6)         => _long(10).
	p(NT(6), (NT(10)));
//G5:   bitvector(6)         => _bits(11).
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
//G12:  _int(8)              => sign(12) _(5) _unsigned(16).
	p(NT(8), (NT(12)+NT(5)+NT(16)));
//G13:  _long(10)            => sign(12) _(5) _unsigned(16) _(5) 'l'.
	p(NT(10), (NT(12)+NT(5)+NT(16)+NT(5)+T(3)));
//G14:  __E__uint_1(17)      => plus(14).
	p(NT(17), (NT(14)));
//G15:  __E__uint_1(17)      => null.
	p(NT(17), (nul));
//G16:  _uint(7)             => __E__uint_1(17) _(5) _unsigned(16) _(5) 'u'.
	p(NT(7), (NT(17)+NT(5)+NT(16)+NT(5)+T(4)));
//G17:  __E__ulong_2(18)     => plus(14).
	p(NT(18), (NT(14)));
//G18:  __E__ulong_2(18)     => null.
	p(NT(18), (nul));
//G19:  _ulong(9)            => __E__ulong_2(18) _(5) _unsigned(16) _(5) 'u' 'l'.
	p(NT(9), (NT(18)+NT(5)+NT(16)+NT(5)+T(4)+T(3)));
//G20:  __E__unsigned_3(19)  => _digit(20).
	p(NT(19), (NT(20)));
//G21:  __E__unsigned_4(21)  => null.
	p(NT(21), (nul));
//G22:  __E__unsigned_4(21)  => __E__unsigned_3(19) __E__unsigned_4(21).
	p(NT(21), (NT(19)+NT(21)));
//G23:  _unsigned(16)        => digit(3) __E__unsigned_4(21).
	p(NT(16), (NT(3)+NT(21)));
//G24:  __E__bits_5(23)      => _(5) _bit(22).
	p(NT(23), (NT(5)+NT(22)));
//G25:  __E__bits_6(24)      => null.
	p(NT(24), (nul));
//G26:  __E__bits_6(24)      => __E__bits_5(23) __E__bits_6(24).
	p(NT(24), (NT(23)+NT(24)));
//G27:  _bits(11)            => _bit(22) __E__bits_6(24) 'b'.
	p(NT(11), (NT(22)+NT(24)+T(5)));
//G28:  zero(25)             => '0'.
	p(NT(25), (T(6)));
//G29:  _bit(22)             => zero(25).
	p(NT(22), (NT(25)));
//G30:  one(26)              => '1'.
	p(NT(26), (T(7)));
//G31:  _bit(22)             => one(26).
	p(NT(22), (NT(26)));
	#undef T
	#undef NT
	return loaded = true, p;
}

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace z3_parser_data

struct z3_parser : public idni::parser<char, char> {
	enum nonterminal {
		nul, space, alpha, digit, start, _, bitvector, _uint, _int, _ulong, 
		_long, _bits, sign, minus, plus, __E_plus_0, _unsigned, __E__uint_1, __E__ulong_2, __E__unsigned_3, 
		_digit, __E__unsigned_4, _bit, __E__bits_5, __E__bits_6, zero, one, 
	};
	static z3_parser& instance() {
		static z3_parser inst;
		return inst;
	}
	z3_parser() : idni::parser<char_type, terminal_type>(
		z3_parser_data::grammar,
		z3_parser_data::parser_options) {}
	size_t id(const std::basic_string<char_type>& name) {
		return z3_parser_data::nts.get(name);
	}
	const std::basic_string<char_type>& name(size_t id) {
		return z3_parser_data::nts.get(id);
	}
	symbol_type literal(const nonterminal& nt) {
		return symbol_type(nt, &z3_parser_data::nts);
	}
};

#endif // __Z3_PARSER_H__
