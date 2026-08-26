// This file is generated from a file parser/qlt.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
// productions() lives here so the table is compiled once, not per TU.
//
#include "qlt_parser.generated.h"

namespace qlt_parser_data {

idni::prods<char_type, terminal_type>& productions() {
	static bool loaded = false;
	static idni::prods<char_type, terminal_type>
		p, nul(idni::lit<char_type, terminal_type>{});
	if (loaded) return p;
	#define  T(x) (idni::prods<char_type, terminal_type>{ terminals[x] })
	#define NT(x) (idni::prods<char_type, terminal_type>{ nts(x) })
//G0:   start(8)             => _(4) qlt(9) _(4).
	p(NT(8), (NT(4)+NT(9)+NT(4)));
//G1:   qlt_top(10)          => 't' 'o' 'p'.
	p(NT(10), (T(1)+T(2)+T(3)));
//G2:   qlt(9)               => qlt_top(10).
	p(NT(9), (NT(10)));
//G3:   qlt_bot(11)          => 'b' 'o' 't'.
	p(NT(11), (T(4)+T(2)+T(1)));
//G4:   qlt(9)               => qlt_bot(11).
	p(NT(9), (NT(11)));
//G5:   qlt_bot(11)          => 'b' 'o' 't' 't' 'o' 'm'.
	p(NT(11), (T(4)+T(2)+T(1)+T(1)+T(2)+T(5)));
//G6:   qlt(9)               => qlt_bot(11).
	p(NT(9), (NT(11)));
//G7:   qlt_singleton(12)    => singleton(7).
	p(NT(12), (NT(7)));
//G8:   qlt(9)               => qlt_singleton(12).
	p(NT(9), (NT(12)));
//G9:   qlt_single(13)       => interval(14).
	p(NT(13), (NT(14)));
//G10:  qlt(9)               => qlt_single(13).
	p(NT(9), (NT(13)));
//G11:  __E_qlt_0(16)        => interval(14) _(4) '|' _(4) qlt(9).
	p(NT(16), (NT(14)+NT(4)+T(6)+NT(4)+NT(9)));
//G12:  qlt_union(15)        => __E_qlt_0(16).
	p(NT(15), (NT(16)));
//G13:  qlt(9)               => qlt_union(15).
	p(NT(9), (NT(15)));
//G14:  interval_cc(17)      => ']'.
	p(NT(17), (T(7)));
//G15:  interval(14)         => '[' _(4) endpoint(5) _(4) ',' _(4) endpoint(5) _(4) interval_cc(17).
	p(NT(14), (T(8)+NT(4)+NT(5)+NT(4)+T(9)+NT(4)+NT(5)+NT(4)+NT(17)));
//G16:  interval_co(18)      => ')'.
	p(NT(18), (T(10)));
//G17:  interval(14)         => '[' _(4) endpoint(5) _(4) ',' _(4) endpoint(5) _(4) interval_co(18).
	p(NT(14), (T(8)+NT(4)+NT(5)+NT(4)+T(9)+NT(4)+NT(5)+NT(4)+NT(18)));
//G18:  interval_oc(19)      => ']'.
	p(NT(19), (T(7)));
//G19:  interval(14)         => '(' _(4) endpoint(5) _(4) ',' _(4) endpoint(5) _(4) interval_oc(19).
	p(NT(14), (T(11)+NT(4)+NT(5)+NT(4)+T(9)+NT(4)+NT(5)+NT(4)+NT(19)));
//G20:  interval_oo(20)      => ')'.
	p(NT(20), (T(10)));
//G21:  interval(14)         => '(' _(4) endpoint(5) _(4) ',' _(4) endpoint(5) _(4) interval_oo(20).
	p(NT(14), (T(11)+NT(4)+NT(5)+NT(4)+T(9)+NT(4)+NT(5)+NT(4)+NT(20)));
//G22:  __E_singleton_1(21)  => ep_char(6).
	p(NT(21), (NT(6)));
//G23:  __E_singleton_1(21)  => ep_char(6) __E_singleton_1(21).
	p(NT(21), (NT(6)+NT(21)));
//G24:  singleton(7)         => __E_singleton_1(21).
	p(NT(7), (NT(21)));
//G25:  __E_endpoint_2(22)   => ep_char(6).
	p(NT(22), (NT(6)));
//G26:  __E_endpoint_2(22)   => ep_char(6) __E_endpoint_2(22).
	p(NT(22), (NT(6)+NT(22)));
//G27:  endpoint(5)          => __E_endpoint_2(22).
	p(NT(5), (NT(22)));
//G28:  ep_char(6)           => digit(2).
	p(NT(6), (NT(2)));
//G29:  ep_char(6)           => alpha(3).
	p(NT(6), (NT(3)));
//G30:  ep_char(6)           => '+'.
	p(NT(6), (T(12)));
//G31:  ep_char(6)           => '-'.
	p(NT(6), (T(13)));
//G32:  ep_char(6)           => '.'.
	p(NT(6), (T(14)));
//G33:  ep_char(6)           => '/'.
	p(NT(6), (T(15)));
//G34:  ep_char(6)           => '_'.
	p(NT(6), (T(16)));
//G35:  __E___3(23)          => space(1) _(4).
	p(NT(23), (NT(1)+NT(4)));
//G36:  __E___3(23)          => null.
	p(NT(23), (nul));
//G37:  _(4)                 => __E___3(23).
	p(NT(4), (NT(23)));
	#undef T
	#undef NT
	return loaded = true, p;
}

} // namespace qlt_parser_data
