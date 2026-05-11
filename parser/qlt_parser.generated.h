// This file is generated from a file parser/qlt.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
#ifndef __QLT_PARSER_H__
#define __QLT_PARSER_H__

#include "parser.h"

namespace qlt_parser_data {

using char_type     = char;
using terminal_type = char;

inline static constexpr size_t nt_bits = 5;
inline const std::vector<std::string> symbol_names{
	"", "space", "digit", "alpha", "start", "_", "qlt", "qlt_top", "qlt_bot", "qlt_singleton", 
	"singleton", "qlt_single", "interval", "qlt_union", "__E_qlt_0", "endpoint", "interval_cc", "interval_co", "interval_oc", "interval_oo", 
	"ep_char", "__E_singleton_1", "__E_endpoint_2", "__E___3", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', 't', 'o', 'p', 'b', 'm', '|', ']', '[', 
	',', ')', '(', '+', '-', '.', '/', '_', 
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
			10, 15, 20
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
//G0:   start(4)             => _(5) qlt(6) _(5).
	p(NT(4), (NT(5)+NT(6)+NT(5)));
//G1:   qlt_top(7)           => 't' 'o' 'p'.
	p(NT(7), (T(1)+T(2)+T(3)));
//G2:   qlt(6)               => qlt_top(7).
	p(NT(6), (NT(7)));
//G3:   qlt_bot(8)           => 'b' 'o' 't'.
	p(NT(8), (T(4)+T(2)+T(1)));
//G4:   qlt(6)               => qlt_bot(8).
	p(NT(6), (NT(8)));
//G5:   qlt_bot(8)           => 'b' 'o' 't' 't' 'o' 'm'.
	p(NT(8), (T(4)+T(2)+T(1)+T(1)+T(2)+T(5)));
//G6:   qlt(6)               => qlt_bot(8).
	p(NT(6), (NT(8)));
//G7:   qlt_singleton(9)     => singleton(10).
	p(NT(9), (NT(10)));
//G8:   qlt(6)               => qlt_singleton(9).
	p(NT(6), (NT(9)));
//G9:   qlt_single(11)       => interval(12).
	p(NT(11), (NT(12)));
//G10:  qlt(6)               => qlt_single(11).
	p(NT(6), (NT(11)));
//G11:  __E_qlt_0(14)        => interval(12) _(5) '|' _(5) qlt(6).
	p(NT(14), (NT(12)+NT(5)+T(6)+NT(5)+NT(6)));
//G12:  qlt_union(13)        => __E_qlt_0(14).
	p(NT(13), (NT(14)));
//G13:  qlt(6)               => qlt_union(13).
	p(NT(6), (NT(13)));
//G14:  interval_cc(16)      => ']'.
	p(NT(16), (T(7)));
//G15:  interval(12)         => '[' _(5) endpoint(15) _(5) ',' _(5) endpoint(15) _(5) interval_cc(16).
	p(NT(12), (T(8)+NT(5)+NT(15)+NT(5)+T(9)+NT(5)+NT(15)+NT(5)+NT(16)));
//G16:  interval_co(17)      => ')'.
	p(NT(17), (T(10)));
//G17:  interval(12)         => '[' _(5) endpoint(15) _(5) ',' _(5) endpoint(15) _(5) interval_co(17).
	p(NT(12), (T(8)+NT(5)+NT(15)+NT(5)+T(9)+NT(5)+NT(15)+NT(5)+NT(17)));
//G18:  interval_oc(18)      => ']'.
	p(NT(18), (T(7)));
//G19:  interval(12)         => '(' _(5) endpoint(15) _(5) ',' _(5) endpoint(15) _(5) interval_oc(18).
	p(NT(12), (T(11)+NT(5)+NT(15)+NT(5)+T(9)+NT(5)+NT(15)+NT(5)+NT(18)));
//G20:  interval_oo(19)      => ')'.
	p(NT(19), (T(10)));
//G21:  interval(12)         => '(' _(5) endpoint(15) _(5) ',' _(5) endpoint(15) _(5) interval_oo(19).
	p(NT(12), (T(11)+NT(5)+NT(15)+NT(5)+T(9)+NT(5)+NT(15)+NT(5)+NT(19)));
//G22:  __E_singleton_1(21)  => ep_char(20).
	p(NT(21), (NT(20)));
//G23:  __E_singleton_1(21)  => ep_char(20) __E_singleton_1(21).
	p(NT(21), (NT(20)+NT(21)));
//G24:  singleton(10)        => __E_singleton_1(21).
	p(NT(10), (NT(21)));
//G25:  __E_endpoint_2(22)   => ep_char(20).
	p(NT(22), (NT(20)));
//G26:  __E_endpoint_2(22)   => ep_char(20) __E_endpoint_2(22).
	p(NT(22), (NT(20)+NT(22)));
//G27:  endpoint(15)         => __E_endpoint_2(22).
	p(NT(15), (NT(22)));
//G28:  ep_char(20)          => digit(2).
	p(NT(20), (NT(2)));
//G29:  ep_char(20)          => alpha(3).
	p(NT(20), (NT(3)));
//G30:  ep_char(20)          => '+'.
	p(NT(20), (T(12)));
//G31:  ep_char(20)          => '-'.
	p(NT(20), (T(13)));
//G32:  ep_char(20)          => '.'.
	p(NT(20), (T(14)));
//G33:  ep_char(20)          => '/'.
	p(NT(20), (T(15)));
//G34:  ep_char(20)          => '_'.
	p(NT(20), (T(16)));
//G35:  __E___3(23)          => space(1) _(5).
	p(NT(23), (NT(1)+NT(5)));
//G36:  __E___3(23)          => null.
	p(NT(23), (nul));
//G37:  _(5)                 => __E___3(23).
	p(NT(5), (NT(23)));
	#undef T
	#undef NT
	return loaded = true, p;
}

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace qlt_parser_data

struct qlt_parser_nonterminals {
	enum nonterminal {
		nul, space, digit, alpha, start, _, qlt, qlt_top, qlt_bot, qlt_singleton, 
		singleton, qlt_single, interval, qlt_union, __E_qlt_0, endpoint, interval_cc, interval_co, interval_oc, interval_oo, 
		ep_char, __E_singleton_1, __E_endpoint_2, __E___3, 
	};
};

struct qlt_parser : public idni::parser<char, char>, public qlt_parser_nonterminals {
	static qlt_parser& instance() {
		static qlt_parser inst;
		return inst;
	}
	qlt_parser() : idni::parser<char_type, terminal_type>(
		qlt_parser_data::grammar,
		qlt_parser_data::parser_options) {}
	size_t id(const std::basic_string<char_type>& name) {
		return qlt_parser_data::nts.get(name);
	}
	const std::basic_string<char_type>& name(size_t id) {
		return qlt_parser_data::nts.get(id);
	}
	symbol_type literal(const nonterminal& nt) {
		return symbol_type(nt, &qlt_parser_data::nts);
	}
};

#endif // __QLT_PARSER_H__
