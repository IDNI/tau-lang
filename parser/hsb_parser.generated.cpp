// This file is generated from a file parser/hsb.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
// productions() lives here so the table is compiled once, not per TU.
//
#include "hsb_parser.generated.h"

namespace hsb_parser_data {

idni::prods<char_type, terminal_type>& productions() {
	static bool loaded = false;
	static idni::prods<char_type, terminal_type>
		p, nul(idni::lit<char_type, terminal_type>{});
	if (loaded) return p;
	#define  T(x) (idni::prods<char_type, terminal_type>{ terminals[x] })
	#define NT(x) (idni::prods<char_type, terminal_type>{ nts(x) })
//G0:   start(6)             => _(3) hsb(7) _(3).
	p(NT(6), (NT(3)+NT(7)+NT(3)));
//G1:   hsb_top(8)           => 't' 'o' 'p'.
	p(NT(8), (T(1)+T(2)+T(3)));
//G2:   hsb(7)               => hsb_top(8).
	p(NT(7), (NT(8)));
//G3:   hsb_bot(9)           => 'b' 'o' 't'.
	p(NT(9), (T(4)+T(2)+T(1)));
//G4:   hsb(7)               => hsb_bot(9).
	p(NT(7), (NT(9)));
//G5:   hsb_bot(9)           => 'b' 'o' 't' 't' 'o' 'm'.
	p(NT(9), (T(4)+T(2)+T(1)+T(1)+T(2)+T(5)));
//G6:   hsb(7)               => hsb_bot(9).
	p(NT(7), (NT(9)));
//G7:   __E_hsb_0(11)        => '~' _(3) hsb(7).
	p(NT(11), (T(6)+NT(3)+NT(7)));
//G8:   hsb_not(10)          => __E_hsb_0(11).
	p(NT(10), (NT(11)));
//G9:   hsb(7)               => hsb_not(10).
	p(NT(7), (NT(10)));
//G10:  __E_hsb_1(13)        => '(' _(3) hsb(7) _(3) ')'.
	p(NT(13), (T(7)+NT(3)+NT(7)+NT(3)+T(8)));
//G11:  hsb_paren(12)        => __E_hsb_1(13).
	p(NT(12), (NT(13)));
//G12:  hsb(7)               => hsb_paren(12).
	p(NT(7), (NT(12)));
//G13:  __E_hsb_2(15)        => '(' _(3) hsb(7) _(3) '&' _(3) hsb(7) _(3) ')'.
	p(NT(15), (T(7)+NT(3)+NT(7)+NT(3)+T(9)+NT(3)+NT(7)+NT(3)+T(8)));
//G14:  hsb_and(14)          => __E_hsb_2(15).
	p(NT(14), (NT(15)));
//G15:  hsb(7)               => hsb_and(14).
	p(NT(7), (NT(14)));
//G16:  __E_hsb_3(17)        => '(' _(3) hsb(7) _(3) '|' _(3) hsb(7) _(3) ')'.
	p(NT(17), (T(7)+NT(3)+NT(7)+NT(3)+T(10)+NT(3)+NT(7)+NT(3)+T(8)));
//G17:  hsb_or(16)           => __E_hsb_3(17).
	p(NT(16), (NT(17)));
//G18:  hsb(7)               => hsb_or(16).
	p(NT(7), (NT(16)));
//G19:  hsb_hs(18)           => halfspace(19).
	p(NT(18), (NT(19)));
//G20:  hsb(7)               => hsb_hs(18).
	p(NT(7), (NT(18)));
//G21:  __E_halfspace_4(21)  => linexpr(22) _(3) '<' '=' _(3) '0'.
	p(NT(21), (NT(22)+NT(3)+T(11)+T(12)+NT(3)+T(13)));
//G22:  hs_leq(20)           => __E_halfspace_4(21).
	p(NT(20), (NT(21)));
//G23:  halfspace(19)        => hs_leq(20).
	p(NT(19), (NT(20)));
//G24:  __E_halfspace_5(24)  => linexpr(22) _(3) '<' _(3) '0'.
	p(NT(24), (NT(22)+NT(3)+T(11)+NT(3)+T(13)));
//G25:  hs_lt(23)            => __E_halfspace_5(24).
	p(NT(23), (NT(24)));
//G26:  halfspace(19)        => hs_lt(23).
	p(NT(19), (NT(23)));
//G27:  __E_linexpr_6(26)    => linexpr(22) _(3) '+' _(3) lterm(27).
	p(NT(26), (NT(22)+NT(3)+T(14)+NT(3)+NT(27)));
//G28:  le_add(25)           => __E_linexpr_6(26).
	p(NT(25), (NT(26)));
//G29:  linexpr(22)          => le_add(25).
	p(NT(22), (NT(25)));
//G30:  __E_linexpr_7(29)    => linexpr(22) _(3) '-' _(3) lterm(27).
	p(NT(29), (NT(22)+NT(3)+T(15)+NT(3)+NT(27)));
//G31:  le_sub(28)           => __E_linexpr_7(29).
	p(NT(28), (NT(29)));
//G32:  linexpr(22)          => le_sub(28).
	p(NT(22), (NT(28)));
//G33:  le_lterm(30)         => lterm(27).
	p(NT(30), (NT(27)));
//G34:  linexpr(22)          => le_lterm(30).
	p(NT(22), (NT(30)));
//G35:  __E_lterm_8(32)      => unum(5) _(3) '*' _(3) var(33).
	p(NT(32), (NT(5)+NT(3)+T(16)+NT(3)+NT(33)));
//G36:  lt_coeff_var(31)     => __E_lterm_8(32).
	p(NT(31), (NT(32)));
//G37:  lterm(27)            => lt_coeff_var(31).
	p(NT(27), (NT(31)));
//G38:  __E_lterm_9(35)      => var(33) _(3) '*' _(3) unum(5).
	p(NT(35), (NT(33)+NT(3)+T(16)+NT(3)+NT(5)));
//G39:  lt_var_coeff(34)     => __E_lterm_9(35).
	p(NT(34), (NT(35)));
//G40:  lterm(27)            => lt_var_coeff(34).
	p(NT(27), (NT(34)));
//G41:  __E_lterm_10(37)     => '-' _(3) unum(5) _(3) '*' _(3) var(33).
	p(NT(37), (T(15)+NT(3)+NT(5)+NT(3)+T(16)+NT(3)+NT(33)));
//G42:  lt_neg_coeff_var(36) => __E_lterm_10(37).
	p(NT(36), (NT(37)));
//G43:  lterm(27)            => lt_neg_coeff_var(36).
	p(NT(27), (NT(36)));
//G44:  __E_lterm_11(39)     => '-' _(3) var(33) _(3) '*' _(3) unum(5).
	p(NT(39), (T(15)+NT(3)+NT(33)+NT(3)+T(16)+NT(3)+NT(5)));
//G45:  lt_neg_var_coeff(38) => __E_lterm_11(39).
	p(NT(38), (NT(39)));
//G46:  lterm(27)            => lt_neg_var_coeff(38).
	p(NT(27), (NT(38)));
//G47:  __E_lterm_12(41)     => '-' _(3) var(33).
	p(NT(41), (T(15)+NT(3)+NT(33)));
//G48:  lt_neg_var(40)       => __E_lterm_12(41).
	p(NT(40), (NT(41)));
//G49:  lterm(27)            => lt_neg_var(40).
	p(NT(27), (NT(40)));
//G50:  __E_lterm_13(43)     => '-' _(3) unum(5).
	p(NT(43), (T(15)+NT(3)+NT(5)));
//G51:  lt_neg_const(42)     => __E_lterm_13(43).
	p(NT(42), (NT(43)));
//G52:  lterm(27)            => lt_neg_const(42).
	p(NT(27), (NT(42)));
//G53:  lt_var(44)           => var(33).
	p(NT(44), (NT(33)));
//G54:  lterm(27)            => lt_var(44).
	p(NT(27), (NT(44)));
//G55:  lt_const(45)         => unum(5).
	p(NT(45), (NT(5)));
//G56:  lterm(27)            => lt_const(45).
	p(NT(27), (NT(45)));
//G57:  var(33)              => 'x' '[' nat(4) ']'.
	p(NT(33), (T(17)+T(18)+NT(4)+T(19)));
//G58:  __E_nat_14(46)       => digit(2).
	p(NT(46), (NT(2)));
//G59:  __E_nat_14(46)       => digit(2) __E_nat_14(46).
	p(NT(46), (NT(2)+NT(46)));
//G60:  nat(4)               => __E_nat_14(46).
	p(NT(4), (NT(46)));
//G61:  unum(5)              => nat(4) '.' nat(4).
	p(NT(5), (NT(4)+T(20)+NT(4)));
//G62:  unum(5)              => nat(4) '/' nat(4).
	p(NT(5), (NT(4)+T(21)+NT(4)));
//G63:  unum(5)              => nat(4).
	p(NT(5), (NT(4)));
//G64:  __E___15(47)         => space(1) _(3).
	p(NT(47), (NT(1)+NT(3)));
//G65:  __E___15(47)         => null.
	p(NT(47), (nul));
//G66:  _(3)                 => __E___15(47).
	p(NT(3), (NT(47)));
	#undef T
	#undef NT
	return loaded = true, p;
}

} // namespace hsb_parser_data
