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
	"", "space", "printable", "_", "atom", "atom_char", "start", "formula", "nlang_bot", "nlang_top", 
	"nlang_not", "__E_formula_0", "nlang_and", "__E_formula_1", "nlang_or", "__E_formula_2", "nlang_atom", "__E_atom_3", "__E___4", "__N_0", 
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
			3
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			4, 5
		},
		.inline_char_classes = true
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

} // namespace nlang_parser_data

struct nlang_parser_nonterminals {
	enum nonterminal {
		nul, space, printable, _, atom, atom_char, start, formula, nlang_bot, nlang_top, 
		nlang_not, __E_formula_0, nlang_and, __E_formula_1, nlang_or, __E_formula_2, nlang_atom, __E_atom_3, __E___4, __N_0, 
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
		nlang_parser_data::make_parser_options()) {}
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
