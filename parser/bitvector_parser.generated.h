// This file is generated from a file src/../parser/bitvector.tgf by
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
	"", "space", "alpha", "digit", "xdigit", "_", "decimal", "hexadecimal", "binary", "start", 
	"bitvector", "__E_binary_0", "__E_binary_1", "__E_hexadecimal_2", "__E_decimal_3", "__E___4", 
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
			5
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			6, 7, 8
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

inline ::idni::prods<char_type, terminal_type> start_symbol{ nts(9) };

idni::prods<char_type, terminal_type>& productions();

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace bitvector_parser_data

struct bitvector_parser_nonterminals {
	enum nonterminal {
		nul, space, alpha, digit, xdigit, _, decimal, hexadecimal, binary, start, 
		bitvector, __E_binary_0, __E_binary_1, __E_hexadecimal_2, __E_decimal_3, __E___4, 
	};
};

struct bitvector_parser : public idni::parser<char, char>, public bitvector_parser_nonterminals {
	static bitvector_parser& instance() {
		static bitvector_parser inst;
		return inst;
	}
	bitvector_parser() : idni::parser<char_type, terminal_type>(
		bitvector_parser_data::grammar,
		bitvector_parser_data::make_parser_options()) {}
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
