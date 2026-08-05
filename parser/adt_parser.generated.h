// This file is generated from a file parser/adt.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
#ifndef __ADT_PARSER_H__
#define __ADT_PARSER_H__

#include "parser.h"

namespace adt_parser_data {

using char_type     = char;
using terminal_type = char;

inline static constexpr size_t nt_bits = 5;
inline const std::vector<std::string> symbol_names{
	"", "eof", "space", "alpha", "alnum", "printable", "start", "_", "tuple_value", "member_value", 
	"__E_tuple_value_0", "__E_tuple_value_1", "member_key", "__E_member_value_2", "leaf_value", "member_name", "chars", "value_chars", "__E_value_chars_3", "__E_value_chars_4", 
	"__E_chars_5", "__E_chars_6", "__E___7", "__N_0", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', ',', '{', '}', ':', '"', '_', 
};

inline ::idni::char_class_fns<terminal_type> char_classes =
	::idni::predefined_char_classes<char_type, terminal_type>({
		"eof",
		"space",
		"alpha",
		"alnum",
		"printable",
	}, nts);

inline struct ::idni::grammar<char_type, terminal_type>::options
	grammar_options
{
	.transform_negation = false,
	.auto_disambiguate = true,
	.shaping = {
		.trim_terminals = false,
		.inline_char_classes = false
	}
};

inline ::idni::parser<char_type, terminal_type>::options parser_options{
};

inline ::idni::prods<char_type, terminal_type> start_symbol{ nts(6) };

inline idni::prods<char_type, terminal_type>& productions() {
	static bool loaded = false;
	static idni::prods<char_type, terminal_type>
		p, nul(idni::lit<char_type, terminal_type>{});
	if (loaded) return p;
	#define  T(x) (idni::prods<char_type, terminal_type>{ terminals[x] })
	#define NT(x) (idni::prods<char_type, terminal_type>{ nts(x) })
//G0:   start(6)             => _(7) tuple_value(8) _(7).
	p(NT(6), (NT(7)+NT(8)+NT(7)));
//G1:   __E_tuple_value_0(10) => _(7) ',' _(7) member_value(9).
	p(NT(10), (NT(7)+T(1)+NT(7)+NT(9)));
//G2:   __E_tuple_value_1(11) => null.
	p(NT(11), (nul));
//G3:   __E_tuple_value_1(11) => __E_tuple_value_0(10) __E_tuple_value_1(11).
	p(NT(11), (NT(10)+NT(11)));
//G4:   tuple_value(8)       => '{' _(7) member_value(9) __E_tuple_value_1(11) _(7) '}'.
	p(NT(8), (T(2)+NT(7)+NT(9)+NT(11)+NT(7)+T(3)));
//G5:   __E_member_value_2(13) => leaf_value(14).
	p(NT(13), (NT(14)));
//G6:   __E_member_value_2(13) => tuple_value(8).
	p(NT(13), (NT(8)));
//G7:   member_value(9)      => member_key(12) _(7) ':' _(7) __E_member_value_2(13).
	p(NT(9), (NT(12)+NT(7)+T(4)+NT(7)+NT(13)));
//G8:   member_key(12)       => member_name(15).
	p(NT(12), (NT(15)));
//G9:   member_key(12)       => '"' member_name(15) '"'.
	p(NT(12), (T(5)+NT(15)+T(5)));
//G10:  member_name(15)      => chars(16).
	p(NT(15), (NT(16)));
//G11:  leaf_value(14)       => '"' value_chars(17) '"'.
	p(NT(14), (T(5)+NT(17)+T(5)));
//G12:  __N_0(23)            => '"'.
	p(NT(23), (T(5)));
//G13:  __E_value_chars_3(18) => ~( __N_0(23) ) & printable(5).	 # conjunctive
	p(NT(18), ~(NT(23)) & (NT(5)));
//G14:  __E_value_chars_4(19) => null.
	p(NT(19), (nul));
//G15:  __E_value_chars_4(19) => __E_value_chars_3(18) __E_value_chars_4(19).
	p(NT(19), (NT(18)+NT(19)));
//G16:  value_chars(17)      => __E_value_chars_4(19).
	p(NT(17), (NT(19)));
//G17:  __E_chars_5(20)      => alnum(4).
	p(NT(20), (NT(4)));
//G18:  __E_chars_5(20)      => '_'.
	p(NT(20), (T(6)));
//G19:  __E_chars_6(21)      => null.
	p(NT(21), (nul));
//G20:  __E_chars_6(21)      => __E_chars_5(20) __E_chars_6(21).
	p(NT(21), (NT(20)+NT(21)));
//G21:  chars(16)            => alpha(3) __E_chars_6(21).
	p(NT(16), (NT(3)+NT(21)));
//G22:  __E___7(22)          => null.
	p(NT(22), (nul));
//G23:  __E___7(22)          => space(2) __E___7(22).
	p(NT(22), (NT(2)+NT(22)));
//G24:  _(7)                 => __E___7(22).
	p(NT(7), (NT(22)));
	#undef T
	#undef NT
	return loaded = true, p;
}

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace adt_parser_data

struct adt_parser_nonterminals {
	enum nonterminal {
		nul, eof, space, alpha, alnum, printable, start, _, tuple_value, member_value, 
		__E_tuple_value_0, __E_tuple_value_1, member_key, __E_member_value_2, leaf_value, member_name, chars, value_chars, __E_value_chars_3, __E_value_chars_4, 
		__E_chars_5, __E_chars_6, __E___7, __N_0, 
	};
};

struct adt_parser : public idni::parser<char, char>, public adt_parser_nonterminals {
	static adt_parser& instance() {
		static adt_parser inst;
		return inst;
	}
	adt_parser() : idni::parser<char_type, terminal_type>(
		adt_parser_data::grammar,
		adt_parser_data::parser_options) {}
	size_t id(const std::basic_string<char_type>& name) {
		return adt_parser_data::nts.get(name);
	}
	const std::basic_string<char_type>& name(size_t id) {
		return adt_parser_data::nts.get(id);
	}
	symbol_type literal(const nonterminal& nt) {
		return symbol_type(nt, &adt_parser_data::nts);
	}
};

#endif // __ADT_PARSER_H__
