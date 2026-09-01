// This file is generated from a file parser/adt.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
// productions() lives here so the table is compiled once, not per TU.
//
#include "parser.h"
#include "adt_parser.generated.h"

namespace adt_parser_data {

#ifndef TAU_PARSER_BUILD_HEADER_ONLY
idni::prods<char_type, terminal_type>& productions() {
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
//G3:   __E_tuple_value_1(11) => __E_tuple_value_1(11) __E_tuple_value_0(10).
	p(NT(11), (NT(11)+NT(10)));
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
//G15:  __E_value_chars_4(19) => __E_value_chars_4(19) __E_value_chars_3(18).
	p(NT(19), (NT(19)+NT(18)));
//G16:  value_chars(17)      => __E_value_chars_4(19).
	p(NT(17), (NT(19)));
//G17:  __E_chars_5(20)      => alnum(4).
	p(NT(20), (NT(4)));
//G18:  __E_chars_5(20)      => '_'.
	p(NT(20), (T(6)));
//G19:  __E_chars_6(21)      => null.
	p(NT(21), (nul));
//G20:  __E_chars_6(21)      => __E_chars_6(21) __E_chars_5(20).
	p(NT(21), (NT(21)+NT(20)));
//G21:  chars(16)            => alpha(3) __E_chars_6(21).
	p(NT(16), (NT(3)+NT(21)));
//G22:  __E___7(22)          => null.
	p(NT(22), (nul));
//G23:  __E___7(22)          => __E___7(22) space(2).
	p(NT(22), (NT(22)+NT(2)));
//G24:  _(7)                 => __E___7(22).
	p(NT(7), (NT(22)));
	#undef T
	#undef NT
	return loaded = true, p;
}
#endif

} // namespace adt_parser_data
