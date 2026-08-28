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

inline auto make_parser_options() {
	auto o = ::idni::default_parser_options<char_type, terminal_type>();
	return o;
}

inline ::idni::prods<char_type, terminal_type> start_symbol{ nts(6) };

idni::prods<char_type, terminal_type>& productions();

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
		adt_parser_data::make_parser_options()) {}
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
