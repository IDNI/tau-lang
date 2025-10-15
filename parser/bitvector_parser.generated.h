// This file is generated from a file ../parser/bitvector.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
#ifndef __BITVECTOR_PARSER_H__
#define __BITVECTOR_PARSER_H__

#include "parser.h"

namespace bitvector_parser_data {

using char_type     = char;
using terminal_type = char;

inline static constexpr size_t nt_bits = 4;
inline const std::vector<std::string> symbol_names{
	"", "space", "alpha", "digit", "xdigit", "start", "_", "bitvector", "binary", "decimal", 
	"hexadecimal", "__E_binary_0", "__E_binary_1", "__E_hexadecimal_2", "__E_decimal_3", "__E___4", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '#', 'b', 'x', '0', '1', 
};

inline ::idni::char_class_fns<terminal_type> char_classes =
	::idni::predefined_char_classes<char_type, terminal_type>({
		"space",
		"alpha",
		"digit",
		"xdigit",
	}, nts);

inline struct ::idni::grammar<char_type, terminal_type>::options
	grammar_options
{
	.transform_negation = false,
	.auto_disambiguate = true,
	.shaping = {
		.to_trim = {
			6
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			8, 9, 10
		},
		.inline_char_classes = false
	},
	.enabled_guards = {
		"charvar"
	}
};

inline ::idni::parser<char_type, terminal_type>::options parser_options{
};

inline ::idni::prods<char_type, terminal_type> start_symbol{ nts(5) };

inline idni::prods<char_type, terminal_type>& productions() {
	static bool loaded = false;
	static idni::prods<char_type, terminal_type>
		p, nul(idni::lit<char_type, terminal_type>{});
	if (loaded) return p;
	#define  T(x) (idni::prods<char_type, terminal_type>{ terminals[x] })
	#define NT(x) (idni::prods<char_type, terminal_type>{ nts(x) })
//G0:   start(5)             => _(6) bitvector(7) _(6).
	p(NT(5), (NT(6)+NT(7)+NT(6)));
//G1:   bitvector(7)         => '#' 'b' binary(8).
	p(NT(7), (T(1)+T(2)+NT(8)));
//G2:   bitvector(7)         => decimal(9).
	p(NT(7), (NT(9)));
//G3:   bitvector(7)         => '#' 'x' hexadecimal(10).
	p(NT(7), (T(1)+T(3)+NT(10)));
//G4:   __E_binary_0(11)     => '0'.
	p(NT(11), (T(4)));
//G5:   __E_binary_0(11)     => '1'.
	p(NT(11), (T(5)));
//G6:   __E_binary_1(12)     => __E_binary_0(11).
	p(NT(12), (NT(11)));
//G7:   __E_binary_1(12)     => __E_binary_0(11) __E_binary_1(12).
	p(NT(12), (NT(11)+NT(12)));
//G8:   binary(8)            => __E_binary_1(12).
	p(NT(8), (NT(12)));
//G9:   __E_hexadecimal_2(13) => xdigit(4).
	p(NT(13), (NT(4)));
//G10:  __E_hexadecimal_2(13) => xdigit(4) __E_hexadecimal_2(13).
	p(NT(13), (NT(4)+NT(13)));
//G11:  hexadecimal(10)      => __E_hexadecimal_2(13).
	p(NT(10), (NT(13)));
//G12:  __E_decimal_3(14)    => digit(3).
	p(NT(14), (NT(3)));
//G13:  __E_decimal_3(14)    => digit(3) __E_decimal_3(14).
	p(NT(14), (NT(3)+NT(14)));
//G14:  decimal(9)           => __E_decimal_3(14).
	p(NT(9), (NT(14)));
//G15:  __E___4(15)          => space(1) _(6).
	p(NT(15), (NT(1)+NT(6)));
//G16:  __E___4(15)          => null.
	p(NT(15), (nul));
//G17:  _(6)                 => __E___4(15).
	p(NT(6), (NT(15)));
	#undef T
	#undef NT
	return loaded = true, p;
}

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace bitvector_parser_data

struct bitvector_parser_nonterminals {
	enum nonterminal {
		nul, space, alpha, digit, xdigit, start, _, bitvector, binary, decimal, 
		hexadecimal, __E_binary_0, __E_binary_1, __E_hexadecimal_2, __E_decimal_3, __E___4, 
	};
};

struct bitvector_parser : public idni::parser<char, char>, public bitvector_parser_nonterminals {
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
