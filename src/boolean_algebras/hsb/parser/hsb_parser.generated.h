// This file is generated from a file parser/hsb.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
#ifndef __HSB_PARSER_H__
#define __HSB_PARSER_H__

#include "parser.h"

namespace hsb_parser_data {

using char_type     = char;
using terminal_type = char;

inline static constexpr size_t nt_bits = 6;
inline const std::vector<std::string> symbol_names{
	"", "space", "digit", "start", "_", "hsb", "hsb_top", "hsb_bot", "hsb_not", "__E_hsb_0", 
	"hsb_paren", "__E_hsb_1", "hsb_and", "__E_hsb_2", "hsb_or", "__E_hsb_3", "hsb_hs", "halfspace", "hs_leq", "__E_halfspace_4", 
	"linexpr", "hs_lt", "__E_halfspace_5", "le_add", "__E_linexpr_6", "lterm", "le_sub", "__E_linexpr_7", "le_lterm", "lt_coeff_var", 
	"__E_lterm_8", "unum", "var", "lt_var_coeff", "__E_lterm_9", "lt_neg_coeff_var", "__E_lterm_10", "lt_neg_var_coeff", "__E_lterm_11", "lt_neg_var", 
	"__E_lterm_12", "lt_neg_const", "__E_lterm_13", "lt_var", "lt_const", "nat", "__E_nat_14", "__E___15", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', 't', 'o', 'p', 'b', 'm', '~', '(', ')', 
	'&', '|', '<', '=', '0', '+', '-', '*', 'x', '[', 
	']', '.', '/', 
};

inline ::idni::char_class_fns<terminal_type> char_classes =
	::idni::predefined_char_classes<char_type, terminal_type>({
		"space",
		"digit",
	}, nts);

inline struct ::idni::grammar<char_type, terminal_type>::options
	grammar_options
{
	.transform_negation = false,
	.auto_disambiguate = true,
	.shaping = {
		.to_trim = {
			4
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			31, 45
		},
		.inline_char_classes = true
	}
};

inline ::idni::parser<char_type, terminal_type>::options parser_options{
};

inline ::idni::prods<char_type, terminal_type> start_symbol{ nts(3) };

inline idni::prods<char_type, terminal_type>& productions() {
	static bool loaded = false;
	static idni::prods<char_type, terminal_type>
		p, nul(idni::lit<char_type, terminal_type>{});
	if (loaded) return p;
	#define  T(x) (idni::prods<char_type, terminal_type>{ terminals[x] })
	#define NT(x) (idni::prods<char_type, terminal_type>{ nts(x) })
//G0:   start(3)             => _(4) hsb(5) _(4).
	p(NT(3), (NT(4)+NT(5)+NT(4)));
//G1:   hsb_top(6)           => 't' 'o' 'p'.
	p(NT(6), (T(1)+T(2)+T(3)));
//G2:   hsb(5)               => hsb_top(6).
	p(NT(5), (NT(6)));
//G3:   hsb_bot(7)           => 'b' 'o' 't'.
	p(NT(7), (T(4)+T(2)+T(1)));
//G4:   hsb(5)               => hsb_bot(7).
	p(NT(5), (NT(7)));
//G5:   hsb_bot(7)           => 'b' 'o' 't' 't' 'o' 'm'.
	p(NT(7), (T(4)+T(2)+T(1)+T(1)+T(2)+T(5)));
//G6:   hsb(5)               => hsb_bot(7).
	p(NT(5), (NT(7)));
//G7:   __E_hsb_0(9)         => '~' _(4) hsb(5).
	p(NT(9), (T(6)+NT(4)+NT(5)));
//G8:   hsb_not(8)           => __E_hsb_0(9).
	p(NT(8), (NT(9)));
//G9:   hsb(5)               => hsb_not(8).
	p(NT(5), (NT(8)));
//G10:  __E_hsb_1(11)        => '(' _(4) hsb(5) _(4) ')'.
	p(NT(11), (T(7)+NT(4)+NT(5)+NT(4)+T(8)));
//G11:  hsb_paren(10)        => __E_hsb_1(11).
	p(NT(10), (NT(11)));
//G12:  hsb(5)               => hsb_paren(10).
	p(NT(5), (NT(10)));
//G13:  __E_hsb_2(13)        => '(' _(4) hsb(5) _(4) '&' _(4) hsb(5) _(4) ')'.
	p(NT(13), (T(7)+NT(4)+NT(5)+NT(4)+T(9)+NT(4)+NT(5)+NT(4)+T(8)));
//G14:  hsb_and(12)          => __E_hsb_2(13).
	p(NT(12), (NT(13)));
//G15:  hsb(5)               => hsb_and(12).
	p(NT(5), (NT(12)));
//G16:  __E_hsb_3(15)        => '(' _(4) hsb(5) _(4) '|' _(4) hsb(5) _(4) ')'.
	p(NT(15), (T(7)+NT(4)+NT(5)+NT(4)+T(10)+NT(4)+NT(5)+NT(4)+T(8)));
//G17:  hsb_or(14)           => __E_hsb_3(15).
	p(NT(14), (NT(15)));
//G18:  hsb(5)               => hsb_or(14).
	p(NT(5), (NT(14)));
//G19:  hsb_hs(16)           => halfspace(17).
	p(NT(16), (NT(17)));
//G20:  hsb(5)               => hsb_hs(16).
	p(NT(5), (NT(16)));
//G21:  __E_halfspace_4(19)  => linexpr(20) _(4) '<' '=' _(4) '0'.
	p(NT(19), (NT(20)+NT(4)+T(11)+T(12)+NT(4)+T(13)));
//G22:  hs_leq(18)           => __E_halfspace_4(19).
	p(NT(18), (NT(19)));
//G23:  halfspace(17)        => hs_leq(18).
	p(NT(17), (NT(18)));
//G24:  __E_halfspace_5(22)  => linexpr(20) _(4) '<' _(4) '0'.
	p(NT(22), (NT(20)+NT(4)+T(11)+NT(4)+T(13)));
//G25:  hs_lt(21)            => __E_halfspace_5(22).
	p(NT(21), (NT(22)));
//G26:  halfspace(17)        => hs_lt(21).
	p(NT(17), (NT(21)));
//G27:  __E_linexpr_6(24)    => linexpr(20) _(4) '+' _(4) lterm(25).
	p(NT(24), (NT(20)+NT(4)+T(14)+NT(4)+NT(25)));
//G28:  le_add(23)           => __E_linexpr_6(24).
	p(NT(23), (NT(24)));
//G29:  linexpr(20)          => le_add(23).
	p(NT(20), (NT(23)));
//G30:  __E_linexpr_7(27)    => linexpr(20) _(4) '-' _(4) lterm(25).
	p(NT(27), (NT(20)+NT(4)+T(15)+NT(4)+NT(25)));
//G31:  le_sub(26)           => __E_linexpr_7(27).
	p(NT(26), (NT(27)));
//G32:  linexpr(20)          => le_sub(26).
	p(NT(20), (NT(26)));
//G33:  le_lterm(28)         => lterm(25).
	p(NT(28), (NT(25)));
//G34:  linexpr(20)          => le_lterm(28).
	p(NT(20), (NT(28)));
//G35:  __E_lterm_8(30)      => unum(31) _(4) '*' _(4) var(32).
	p(NT(30), (NT(31)+NT(4)+T(16)+NT(4)+NT(32)));
//G36:  lt_coeff_var(29)     => __E_lterm_8(30).
	p(NT(29), (NT(30)));
//G37:  lterm(25)            => lt_coeff_var(29).
	p(NT(25), (NT(29)));
//G38:  __E_lterm_9(34)      => var(32) _(4) '*' _(4) unum(31).
	p(NT(34), (NT(32)+NT(4)+T(16)+NT(4)+NT(31)));
//G39:  lt_var_coeff(33)     => __E_lterm_9(34).
	p(NT(33), (NT(34)));
//G40:  lterm(25)            => lt_var_coeff(33).
	p(NT(25), (NT(33)));
//G41:  __E_lterm_10(36)     => '-' _(4) unum(31) _(4) '*' _(4) var(32).
	p(NT(36), (T(15)+NT(4)+NT(31)+NT(4)+T(16)+NT(4)+NT(32)));
//G42:  lt_neg_coeff_var(35) => __E_lterm_10(36).
	p(NT(35), (NT(36)));
//G43:  lterm(25)            => lt_neg_coeff_var(35).
	p(NT(25), (NT(35)));
//G44:  __E_lterm_11(38)     => '-' _(4) var(32) _(4) '*' _(4) unum(31).
	p(NT(38), (T(15)+NT(4)+NT(32)+NT(4)+T(16)+NT(4)+NT(31)));
//G45:  lt_neg_var_coeff(37) => __E_lterm_11(38).
	p(NT(37), (NT(38)));
//G46:  lterm(25)            => lt_neg_var_coeff(37).
	p(NT(25), (NT(37)));
//G47:  __E_lterm_12(40)     => '-' _(4) var(32).
	p(NT(40), (T(15)+NT(4)+NT(32)));
//G48:  lt_neg_var(39)       => __E_lterm_12(40).
	p(NT(39), (NT(40)));
//G49:  lterm(25)            => lt_neg_var(39).
	p(NT(25), (NT(39)));
//G50:  __E_lterm_13(42)     => '-' _(4) unum(31).
	p(NT(42), (T(15)+NT(4)+NT(31)));
//G51:  lt_neg_const(41)     => __E_lterm_13(42).
	p(NT(41), (NT(42)));
//G52:  lterm(25)            => lt_neg_const(41).
	p(NT(25), (NT(41)));
//G53:  lt_var(43)           => var(32).
	p(NT(43), (NT(32)));
//G54:  lterm(25)            => lt_var(43).
	p(NT(25), (NT(43)));
//G55:  lt_const(44)         => unum(31).
	p(NT(44), (NT(31)));
//G56:  lterm(25)            => lt_const(44).
	p(NT(25), (NT(44)));
//G57:  var(32)              => 'x' '[' nat(45) ']'.
	p(NT(32), (T(17)+T(18)+NT(45)+T(19)));
//G58:  __E_nat_14(46)       => digit(2).
	p(NT(46), (NT(2)));
//G59:  __E_nat_14(46)       => digit(2) __E_nat_14(46).
	p(NT(46), (NT(2)+NT(46)));
//G60:  nat(45)              => __E_nat_14(46).
	p(NT(45), (NT(46)));
//G61:  unum(31)             => nat(45) '.' nat(45).
	p(NT(31), (NT(45)+T(20)+NT(45)));
//G62:  unum(31)             => nat(45) '/' nat(45).
	p(NT(31), (NT(45)+T(21)+NT(45)));
//G63:  unum(31)             => nat(45).
	p(NT(31), (NT(45)));
//G64:  __E___15(47)         => space(1) _(4).
	p(NT(47), (NT(1)+NT(4)));
//G65:  __E___15(47)         => null.
	p(NT(47), (nul));
//G66:  _(4)                 => __E___15(47).
	p(NT(4), (NT(47)));
	#undef T
	#undef NT
	return loaded = true, p;
}

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace hsb_parser_data

struct hsb_parser_nonterminals {
	enum nonterminal {
		nul, space, digit, start, _, hsb, hsb_top, hsb_bot, hsb_not, __E_hsb_0, 
		hsb_paren, __E_hsb_1, hsb_and, __E_hsb_2, hsb_or, __E_hsb_3, hsb_hs, halfspace, hs_leq, __E_halfspace_4, 
		linexpr, hs_lt, __E_halfspace_5, le_add, __E_linexpr_6, lterm, le_sub, __E_linexpr_7, le_lterm, lt_coeff_var, 
		__E_lterm_8, unum, var, lt_var_coeff, __E_lterm_9, lt_neg_coeff_var, __E_lterm_10, lt_neg_var_coeff, __E_lterm_11, lt_neg_var, 
		__E_lterm_12, lt_neg_const, __E_lterm_13, lt_var, lt_const, nat, __E_nat_14, __E___15, 
	};
};

struct hsb_parser : public idni::parser<char, char>, public hsb_parser_nonterminals {
	static hsb_parser& instance() {
		static hsb_parser inst;
		return inst;
	}
	hsb_parser() : idni::parser<char_type, terminal_type>(
		hsb_parser_data::grammar,
		hsb_parser_data::parser_options) {}
	size_t id(const std::basic_string<char_type>& name) {
		return hsb_parser_data::nts.get(name);
	}
	const std::basic_string<char_type>& name(size_t id) {
		return hsb_parser_data::nts.get(id);
	}
	symbol_type literal(const nonterminal& nt) {
		return symbol_type(nt, &hsb_parser_data::nts);
	}
};

#endif // __HSB_PARSER_H__
