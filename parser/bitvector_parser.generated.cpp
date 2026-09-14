// This file is generated from a file parser/bitvector.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
// productions() lives here so the table is compiled once, not per TU.
//
#include "parser.h"
#include "bitvector_parser.generated.h"

namespace bitvector_parser_data {

#ifndef TAU_PARSER_BUILD_HEADER_ONLY
idni::prods<char_type, terminal_type>& productions() {
	static bool loaded = false;
	static idni::prods<char_type, terminal_type>
		p, nul(idni::lit<char_type, terminal_type>{});
	if (loaded) return p;
	#define  T(x) (idni::prods<char_type, terminal_type>{ terminals[x] })
	#define NT(x) (idni::prods<char_type, terminal_type>{ nts(x) })
//G0:   start(9)             => _(5) bitvector(10) _(5).
	p(NT(9), (NT(5)+NT(10)+NT(5)));
//G1:   bitvector(10)        => '#' 'b' binary(8).
	p(NT(10), (T(1)+T(2)+NT(8)));
//G2:   bitvector(10)        => decimal(6).
	p(NT(10), (NT(6)));
//G3:   bitvector(10)        => '#' 'x' hexadecimal(7).
	p(NT(10), (T(1)+T(3)+NT(7)));
//G4:   __E_binary_0(11)     => '0'.
	p(NT(11), (T(4)));
//G5:   __E_binary_0(11)     => '1'.
	p(NT(11), (T(5)));
//G6:   __E_binary_1(12)     => __E_binary_0(11).
	p(NT(12), (NT(11)));
//G7:   __E_binary_1(12)     => __E_binary_1(12) __E_binary_0(11).
	p(NT(12), (NT(12)+NT(11)));
//G8:   binary(8)            => __E_binary_1(12).
	p(NT(8), (NT(12)));
//G9:   __E_hexadecimal_2(13) => xdigit(4).
	p(NT(13), (NT(4)));
//G10:  __E_hexadecimal_2(13) => __E_hexadecimal_2(13) xdigit(4).
	p(NT(13), (NT(13)+NT(4)));
//G11:  hexadecimal(7)       => __E_hexadecimal_2(13).
	p(NT(7), (NT(13)));
//G12:  __E_decimal_3(14)    => digit(3).
	p(NT(14), (NT(3)));
//G13:  __E_decimal_3(14)    => __E_decimal_3(14) digit(3).
	p(NT(14), (NT(14)+NT(3)));
//G14:  decimal(6)           => __E_decimal_3(14).
	p(NT(6), (NT(14)));
//G15:  __E___4(15)          => space(1) _(5).
	p(NT(15), (NT(1)+NT(5)));
//G16:  __E___4(15)          => null.
	p(NT(15), (nul));
//G17:  _(5)                 => __E___4(15).
	p(NT(5), (NT(15)));
	#undef T
	#undef NT
	return loaded = true, p;
}
#endif

} // namespace bitvector_parser_data
