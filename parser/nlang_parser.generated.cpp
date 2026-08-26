// This file is generated from a file parser/nlang.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
// productions() lives here so the table is compiled once, not per TU.
//
#include "nlang_parser.generated.h"

namespace nlang_parser_data {

idni::prods<char_type, terminal_type>& productions() {
	static bool loaded = false;
	static idni::prods<char_type, terminal_type>
		p, nul(idni::lit<char_type, terminal_type>{});
	if (loaded) return p;
	#define  T(x) (idni::prods<char_type, terminal_type>{ terminals[x] })
	#define NT(x) (idni::prods<char_type, terminal_type>{ nts(x) })
//G0:   start(6)             => _(3) formula(7) _(3).
	p(NT(6), (NT(3)+NT(7)+NT(3)));
//G1:   nlang_bot(8)         => 'n' 'o' 't' 'h' 'i' 'n' 'g'.
	p(NT(8), (T(1)+T(2)+T(3)+T(4)+T(5)+T(1)+T(6)));
//G2:   formula(7)           => nlang_bot(8).
	p(NT(7), (NT(8)));
//G3:   nlang_top(9)         => 'e' 'v' 'e' 'r' 'y' 't' 'h' 'i' 'n' 'g'.
	p(NT(9), (T(7)+T(8)+T(7)+T(9)+T(10)+T(3)+T(4)+T(5)+T(1)+T(6)));
//G4:   formula(7)           => nlang_top(9).
	p(NT(7), (NT(9)));
//G5:   __E_formula_0(11)    => 'n' 'o' 't' _(3) '(' _(3) formula(7) _(3) ')'.
	p(NT(11), (T(1)+T(2)+T(3)+NT(3)+T(11)+NT(3)+NT(7)+NT(3)+T(12)));
//G6:   nlang_not(10)        => __E_formula_0(11).
	p(NT(10), (NT(11)));
//G7:   formula(7)           => nlang_not(10).
	p(NT(7), (NT(10)));
//G8:   __E_formula_1(13)    => '(' _(3) formula(7) _(3) ')' _(3) 'a' 'n' 'd' _(3) '(' _(3) formula(7) _(3) ')'.
	p(NT(13), (T(11)+NT(3)+NT(7)+NT(3)+T(12)+NT(3)+T(13)+T(1)+T(14)+NT(3)+T(11)+NT(3)+NT(7)+NT(3)+T(12)));
//G9:   nlang_and(12)        => __E_formula_1(13).
	p(NT(12), (NT(13)));
//G10:  formula(7)           => nlang_and(12).
	p(NT(7), (NT(12)));
//G11:  __E_formula_2(15)    => '(' _(3) formula(7) _(3) ')' _(3) 'o' 'r' _(3) '(' _(3) formula(7) _(3) ')'.
	p(NT(15), (T(11)+NT(3)+NT(7)+NT(3)+T(12)+NT(3)+T(2)+T(9)+NT(3)+T(11)+NT(3)+NT(7)+NT(3)+T(12)));
//G12:  nlang_or(14)         => __E_formula_2(15).
	p(NT(14), (NT(15)));
//G13:  formula(7)           => nlang_or(14).
	p(NT(7), (NT(14)));
//G14:  nlang_atom(16)       => atom(4).
	p(NT(16), (NT(4)));
//G15:  formula(7)           => nlang_atom(16).
	p(NT(7), (NT(16)));
//G16:  __E_atom_3(17)       => atom_char(5).
	p(NT(17), (NT(5)));
//G17:  __E_atom_3(17)       => atom_char(5) __E_atom_3(17).
	p(NT(17), (NT(5)+NT(17)));
//G18:  atom(4)              => __E_atom_3(17).
	p(NT(4), (NT(17)));
//G19:  __N_0(19)            => '('.
	p(NT(19), (T(11)));
//G20:  __N_1(20)            => ')'.
	p(NT(20), (T(12)));
//G21:  atom_char(5)         => ~( __N_0(19) ) & ~( __N_1(20) ) & printable(2).	 # conjunctive
	p(NT(5), ~(NT(19)) & ~(NT(20)) & (NT(2)));
//G22:  __E___4(18)          => space(1) _(3).
	p(NT(18), (NT(1)+NT(3)));
//G23:  __E___4(18)          => null.
	p(NT(18), (nul));
//G24:  _(3)                 => __E___4(18).
	p(NT(3), (NT(18)));
	#undef T
	#undef NT
	return loaded = true, p;
}

} // namespace nlang_parser_data
