// This file is generated from a file parser/qint.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
// productions() lives here so the table is compiled once, not per TU.
//
#include "qint_parser.generated.h"

namespace qint_parser_data {

idni::prods<char_type, terminal_type>& productions() {
	static bool loaded = false;
	static idni::prods<char_type, terminal_type>
		p, nul(idni::lit<char_type, terminal_type>{});
	if (loaded) return p;
	#define  T(x) (idni::prods<char_type, terminal_type>{ terminals[x] })
	#define NT(x) (idni::prods<char_type, terminal_type>{ nts(x) })
//G0:   start(8)             => _(4) qint(9) _(4).
	p(NT(8), (NT(4)+NT(9)+NT(4)));
//G1:   qint_top(10)         => 't' 'o' 'p'.
	p(NT(10), (T(1)+T(2)+T(3)));
//G2:   qint(9)              => qint_top(10).
	p(NT(9), (NT(10)));
//G3:   qint_bot(11)         => 'b' 'o' 't'.
	p(NT(11), (T(4)+T(2)+T(1)));
//G4:   qint(9)              => qint_bot(11).
	p(NT(9), (NT(11)));
//G5:   qint_bot(11)         => 'b' 'o' 't' 't' 'o' 'm'.
	p(NT(11), (T(4)+T(2)+T(1)+T(1)+T(2)+T(5)));
//G6:   qint(9)              => qint_bot(11).
	p(NT(9), (NT(11)));
//G7:   qint_integer(12)     => integer(7).
	p(NT(12), (NT(7)));
//G8:   qint(9)              => qint_integer(12).
	p(NT(9), (NT(12)));
//G9:   qint_single(13)      => interval(14).
	p(NT(13), (NT(14)));
//G10:  qint(9)              => qint_single(13).
	p(NT(9), (NT(13)));
//G11:  __E_qint_0(16)       => interval(14) _(4) '|' _(4) qint(9).
	p(NT(16), (NT(14)+NT(4)+T(6)+NT(4)+NT(9)));
//G12:  qint_union(15)       => __E_qint_0(16).
	p(NT(15), (NT(16)));
//G13:  qint(9)              => qint_union(15).
	p(NT(9), (NT(15)));
//G14:  interval(14)         => '[' _(4) endpoint(5) _(4) ',' _(4) endpoint(5) _(4) ')'.
	p(NT(14), (T(7)+NT(4)+NT(5)+NT(4)+T(8)+NT(4)+NT(5)+NT(4)+T(9)));
//G15:  __E_endpoint_1(17)   => ep_char(6).
	p(NT(17), (NT(6)));
//G16:  __E_endpoint_1(17)   => ep_char(6) __E_endpoint_1(17).
	p(NT(17), (NT(6)+NT(17)));
//G17:  endpoint(5)          => __E_endpoint_1(17).
	p(NT(5), (NT(17)));
//G18:  ep_char(6)           => digit(2).
	p(NT(6), (NT(2)));
//G19:  ep_char(6)           => alpha(3).
	p(NT(6), (NT(3)));
//G20:  ep_char(6)           => '+'.
	p(NT(6), (T(10)));
//G21:  ep_char(6)           => '-'.
	p(NT(6), (T(11)));
//G22:  ep_char(6)           => '.'.
	p(NT(6), (T(12)));
//G23:  ep_char(6)           => '/'.
	p(NT(6), (T(13)));
//G24:  __E_integer_2(18)    => '-'.
	p(NT(18), (T(11)));
//G25:  __E_integer_2(18)    => null.
	p(NT(18), (nul));
//G26:  __E_integer_3(19)    => digit(2).
	p(NT(19), (NT(2)));
//G27:  __E_integer_3(19)    => digit(2) __E_integer_3(19).
	p(NT(19), (NT(2)+NT(19)));
//G28:  integer(7)           => __E_integer_2(18) __E_integer_3(19).
	p(NT(7), (NT(18)+NT(19)));
//G29:  __E___4(20)          => space(1) _(4).
	p(NT(20), (NT(1)+NT(4)));
//G30:  __E___4(20)          => null.
	p(NT(20), (nul));
//G31:  _(4)                 => __E___4(20).
	p(NT(4), (NT(20)));
	#undef T
	#undef NT
	return loaded = true, p;
}

} // namespace qint_parser_data
