// This file is generated from a file parser/qint.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
#ifndef __QINT_PARSER_H__
#define __QINT_PARSER_H__

#include "parser.h"

namespace qint_parser_data {

using char_type     = char;
using terminal_type = char;

inline static constexpr size_t nt_bits = 5;
inline const std::vector<std::string> symbol_names{
	"", "space", "digit", "alpha", "start", "_", "qint", "qint_top", "qint_bot", "qint_integer", 
	"integer", "qint_single", "interval", "qint_union", "__E_qint_0", "endpoint", "ep_char", "__E_endpoint_1", "__E_integer_2", "__E_integer_3", 
	"__E___4", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', 't', 'o', 'p', 'b', 'm', '|', '[', ',', 
	')', '+', '-', '.', '/', 
};

inline ::idni::char_class_fns<terminal_type> char_classes =
	::idni::predefined_char_classes<char_type, terminal_type>({
		"space",
		"digit",
		"alpha",
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
			10, 15, 16
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
//G0:   start(4)             => _(5) qint(6) _(5).
	p(NT(4), (NT(5)+NT(6)+NT(5)));
//G1:   qint_top(7)          => 't' 'o' 'p'.
	p(NT(7), (T(1)+T(2)+T(3)));
//G2:   qint(6)              => qint_top(7).
	p(NT(6), (NT(7)));
//G3:   qint_bot(8)          => 'b' 'o' 't'.
	p(NT(8), (T(4)+T(2)+T(1)));
//G4:   qint(6)              => qint_bot(8).
	p(NT(6), (NT(8)));
//G5:   qint_bot(8)          => 'b' 'o' 't' 't' 'o' 'm'.
	p(NT(8), (T(4)+T(2)+T(1)+T(1)+T(2)+T(5)));
//G6:   qint(6)              => qint_bot(8).
	p(NT(6), (NT(8)));
//G7:   qint_integer(9)      => integer(10).
	p(NT(9), (NT(10)));
//G8:   qint(6)              => qint_integer(9).
	p(NT(6), (NT(9)));
//G9:   qint_single(11)      => interval(12).
	p(NT(11), (NT(12)));
//G10:  qint(6)              => qint_single(11).
	p(NT(6), (NT(11)));
//G11:  __E_qint_0(14)       => interval(12) _(5) '|' _(5) qint(6).
	p(NT(14), (NT(12)+NT(5)+T(6)+NT(5)+NT(6)));
//G12:  qint_union(13)       => __E_qint_0(14).
	p(NT(13), (NT(14)));
//G13:  qint(6)              => qint_union(13).
	p(NT(6), (NT(13)));
//G14:  interval(12)         => '[' _(5) endpoint(15) _(5) ',' _(5) endpoint(15) _(5) ')'.
	p(NT(12), (T(7)+NT(5)+NT(15)+NT(5)+T(8)+NT(5)+NT(15)+NT(5)+T(9)));
//G15:  __E_endpoint_1(17)   => ep_char(16).
	p(NT(17), (NT(16)));
//G16:  __E_endpoint_1(17)   => ep_char(16) __E_endpoint_1(17).
	p(NT(17), (NT(16)+NT(17)));
//G17:  endpoint(15)         => __E_endpoint_1(17).
	p(NT(15), (NT(17)));
//G18:  ep_char(16)          => digit(2).
	p(NT(16), (NT(2)));
//G19:  ep_char(16)          => alpha(3).
	p(NT(16), (NT(3)));
//G20:  ep_char(16)          => '+'.
	p(NT(16), (T(10)));
//G21:  ep_char(16)          => '-'.
	p(NT(16), (T(11)));
//G22:  ep_char(16)          => '.'.
	p(NT(16), (T(12)));
//G23:  ep_char(16)          => '/'.
	p(NT(16), (T(13)));
//G24:  __E_integer_2(18)    => '-'.
	p(NT(18), (T(11)));
//G25:  __E_integer_2(18)    => null.
	p(NT(18), (nul));
//G26:  __E_integer_3(19)    => digit(2).
	p(NT(19), (NT(2)));
//G27:  __E_integer_3(19)    => digit(2) __E_integer_3(19).
	p(NT(19), (NT(2)+NT(19)));
//G28:  integer(10)          => __E_integer_2(18) __E_integer_3(19).
	p(NT(10), (NT(18)+NT(19)));
//G29:  __E___4(20)          => space(1) _(5).
	p(NT(20), (NT(1)+NT(5)));
//G30:  __E___4(20)          => null.
	p(NT(20), (nul));
//G31:  _(5)                 => __E___4(20).
	p(NT(5), (NT(20)));
	#undef T
	#undef NT
	return loaded = true, p;
}

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace qint_parser_data

struct qint_parser_nonterminals {
	enum nonterminal {
		nul, space, digit, alpha, start, _, qint, qint_top, qint_bot, qint_integer, 
		integer, qint_single, interval, qint_union, __E_qint_0, endpoint, ep_char, __E_endpoint_1, __E_integer_2, __E_integer_3, 
		__E___4, 
	};
};

struct qint_parser : public idni::parser<char, char>, public qint_parser_nonterminals {
	static qint_parser& instance() {
		static qint_parser inst;
		return inst;
	}
	qint_parser() : idni::parser<char_type, terminal_type>(
		qint_parser_data::grammar,
		qint_parser_data::parser_options) {}
	size_t id(const std::basic_string<char_type>& name) {
		return qint_parser_data::nts.get(name);
	}
	const std::basic_string<char_type>& name(size_t id) {
		return qint_parser_data::nts.get(id);
	}
	symbol_type literal(const nonterminal& nt) {
		return symbol_type(nt, &qint_parser_data::nts);
	}
};

#endif // __QINT_PARSER_H__
