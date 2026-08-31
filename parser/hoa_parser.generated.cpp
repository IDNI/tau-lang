// This file is generated from a file parser/hoa.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
// productions() lives here so the table is compiled once, not per TU.
//
#include "hoa_parser.generated.h"

namespace hoa_parser_data {

idni::prods<char_type, terminal_type>& productions() {
	static bool loaded = false;
	static idni::prods<char_type, terminal_type>
		p, nul(idni::lit<char_type, terminal_type>{});
	if (loaded) return p;
	#define  T(x) (idni::prods<char_type, terminal_type>{ terminals[x] })
	#define NT(x) (idni::prods<char_type, terminal_type>{ nts(x) })
//G0:   __E_start_0(19)      => null.
	p(NT(19), (nul));
//G1:   __E_start_0(19)      => hline(18) __E_start_0(19).
	p(NT(19), (NT(18)+NT(19)));
//G2:   start(17)            => __E_start_0(19) body(20).
	p(NT(17), (NT(19)+NT(20)));
//G3:   hline(18)            => states_line(21).
	p(NT(18), (NT(21)));
//G4:   hline(18)            => start_line(22).
	p(NT(18), (NT(22)));
//G5:   hline(18)            => ap_line(23).
	p(NT(18), (NT(23)));
//G6:   hline(18)            => skip_line(24).
	p(NT(18), (NT(24)));
//G7:   states_line(21)      => 'S' 't' 'a' 't' 'e' 's' ':' b(5) num(7) b(5) '\n'.
	p(NT(21), (T(1)+T(2)+T(3)+T(2)+T(4)+T(5)+T(6)+NT(5)+NT(7)+NT(5)+T(7)));
//G8:   start_line(22)       => 'S' 't' 'a' 'r' 't' ':' b(5) num(7) b(5) '\n'.
	p(NT(22), (T(1)+T(2)+T(3)+T(8)+T(2)+T(6)+NT(5)+NT(7)+NT(5)+T(7)));
//G9:   __E_ap_line_1(25)    => b1(6) ap_name(26).
	p(NT(25), (NT(6)+NT(26)));
//G10:  __E_ap_line_2(27)    => null.
	p(NT(27), (nul));
//G11:  __E_ap_line_2(27)    => __E_ap_line_1(25) __E_ap_line_2(27).
	p(NT(27), (NT(25)+NT(27)));
//G12:  ap_line(23)          => 'A' 'P' ':' b(5) num(7) __E_ap_line_2(27) b(5) '\n'.
	p(NT(23), (T(9)+T(10)+T(6)+NT(5)+NT(7)+NT(27)+NT(5)+T(7)));
//G13:  __E_ap_name_3(29)    => '"' qrun(30) '"'.
	p(NT(29), (T(11)+NT(30)+T(11)));
//G14:  ap_quoted(28)        => __E_ap_name_3(29).
	p(NT(28), (NT(29)));
//G15:  ap_name(26)          => ap_quoted(28).
	p(NT(26), (NT(28)));
//G16:  ap_bare(31)          => nrun1(32).
	p(NT(31), (NT(32)));
//G17:  ap_name(26)          => ap_bare(31).
	p(NT(26), (NT(31)));
//G18:  __E_qrun_4(33)       => qrun(30) qchar(8).
	p(NT(33), (NT(30)+NT(8)));
//G19:  __E_qrun_4(33)       => null.
	p(NT(33), (nul));
//G20:  qrun(30)             => __E_qrun_4(33).
	p(NT(30), (NT(33)));
//G21:  nrun1(32)            => nqchar(9).
	p(NT(32), (NT(9)));
//G22:  nrun1(32)            => nrun1(32) nqchar(9).
	p(NT(32), (NT(32)+NT(9)));
//G23:  __E_body_5(35)       => null.
	p(NT(35), (nul));
//G24:  __E_body_5(35)       => belem(34) __E_body_5(35).
	p(NT(35), (NT(34)+NT(35)));
//G25:  __E_body_6(36)       => end_part(37).
	p(NT(36), (NT(37)));
//G26:  __E_body_6(36)       => null.
	p(NT(36), (nul));
//G27:  body(20)             => '-' '-' 'B' 'O' 'D' 'Y' '-' '-' '\n' __E_body_5(35) __E_body_6(36).
	p(NT(20), (T(12)+T(12)+T(13)+T(14)+T(15)+T(16)+T(12)+T(12)+T(7)+NT(35)+NT(36)));
//G28:  belem(34)            => state_line(38).
	p(NT(34), (NT(38)));
//G29:  belem(34)            => edge_line(39).
	p(NT(34), (NT(39)));
//G30:  belem(34)            => skip_line(24).
	p(NT(34), (NT(24)));
//G31:  __E_state_line_7(40) => b1(6) acc(10).
	p(NT(40), (NT(6)+NT(10)));
//G32:  __E_state_line_7(40) => null.
	p(NT(40), (nul));
//G33:  state_line(38)       => 'S' 't' 'a' 't' 'e' ':' b(5) num(7) __E_state_line_7(40) b(5) '\n'.
	p(NT(38), (T(1)+T(2)+T(3)+T(2)+T(4)+T(6)+NT(5)+NT(7)+NT(40)+NT(5)+T(7)));
//G34:  __E_edge_line_8(42)  => b1(6) acc(10).
	p(NT(42), (NT(6)+NT(10)));
//G35:  __E_edge_line_8(42)  => null.
	p(NT(42), (nul));
//G36:  edge_line(39)        => '[' guard(41) ']' b(5) num(7) __E_edge_line_8(42) b(5) '\n'.
	p(NT(39), (T(17)+NT(41)+T(18)+NT(5)+NT(7)+NT(42)+NT(5)+T(7)));
//G37:  acc(10)              => '{' arun(43) '}'.
	p(NT(10), (T(19)+NT(43)+T(20)));
//G38:  __E_arun_9(44)       => arun(43) achar(11).
	p(NT(44), (NT(43)+NT(11)));
//G39:  __E_arun_9(44)       => null.
	p(NT(44), (nul));
//G40:  arun(43)             => __E_arun_9(44).
	p(NT(43), (NT(44)));
//G41:  __E_guard_10(45)     => gb(16) gdisj(12) gb(16).
	p(NT(45), (NT(16)+NT(12)+NT(16)));
//G42:  __E_guard_10(45)     => null.
	p(NT(45), (nul));
//G43:  guard(41)            => __E_guard_10(45).
	p(NT(41), (NT(45)));
//G44:  gdisj(12)            => gconj(13).
	p(NT(12), (NT(13)));
//G45:  gdisj(12)            => gdisj(12) gb(16) '|' gb(16) gconj(13).
	p(NT(12), (NT(12)+NT(16)+T(21)+NT(16)+NT(13)));
//G46:  gconj(13)            => glit(14).
	p(NT(13), (NT(14)));
//G47:  gconj(13)            => gconj(13) gb(16) '&' gb(16) glit(14).
	p(NT(13), (NT(13)+NT(16)+T(22)+NT(16)+NT(14)));
//G48:  glit(14)             => 't'.
	p(NT(14), (T(2)));
//G49:  glit(14)             => 'f'.
	p(NT(14), (T(23)));
//G50:  __E_glit_11(46)      => '!'.
	p(NT(46), (T(24)));
//G51:  __E_glit_11(46)      => null.
	p(NT(46), (nul));
//G52:  glit(14)             => __E_glit_11(46) gnum(15).
	p(NT(14), (NT(46)+NT(15)));
//G53:  glit(14)             => '(' gb(16) gdisj(12) gb(16) ')'.
	p(NT(14), (T(25)+NT(16)+NT(12)+NT(16)+T(26)));
//G54:  gnum(15)             => digit(3).
	p(NT(15), (NT(3)));
//G55:  gnum(15)             => gnum(15) digit(3).
	p(NT(15), (NT(15)+NT(3)));
//G56:  __E_gb_12(47)        => gb(16) blank(2).
	p(NT(47), (NT(16)+NT(2)));
//G57:  __E_gb_12(47)        => null.
	p(NT(47), (nul));
//G58:  gb(16)               => __E_gb_12(47).
	p(NT(16), (NT(47)));
//G59:  end_part(37)         => '-' '-' 'E' 'N' 'D' '-' '-' '\n' tail(48).
	p(NT(37), (T(12)+T(12)+T(27)+T(28)+T(15)+T(12)+T(12)+T(7)+NT(48)));
//G60:  __E_tail_13(49)      => tail(48) any(50).
	p(NT(49), (NT(48)+NT(50)));
//G61:  __E_tail_13(49)      => null.
	p(NT(49), (nul));
//G62:  tail(48)             => __E_tail_13(49).
	p(NT(48), (NT(49)));
//G63:  __E_num_14(51)       => digit(3).
	p(NT(51), (NT(3)));
//G64:  __E_num_14(51)       => digit(3) __E_num_14(51).
	p(NT(51), (NT(3)+NT(51)));
//G65:  num(7)               => __E_num_14(51).
	p(NT(7), (NT(51)));
//G66:  __N_0(59)            => '"'.
	p(NT(59), (T(11)));
//G67:  qchar(8)             => ~( __N_0(59) ) & printable(4).	 # conjunctive
	p(NT(8), ~(NT(59)) & (NT(4)));
//G68:  __N_1(60)            => ' '.
	p(NT(60), (T(29)));
//G69:  __N_2(61)            => '"'.
	p(NT(61), (T(11)));
//G70:  nqchar(9)            => ~( __N_1(60) ) & ~( __N_2(61) ) & printable(4).	 # conjunctive
	p(NT(9), ~(NT(60)) & ~(NT(61)) & (NT(4)));
//G71:  __N_3(62)            => '}'.
	p(NT(62), (T(20)));
//G72:  __E_achar_15(52)     => ~( __N_3(62) ) & printable(4).	 # conjunctive
	p(NT(52), ~(NT(62)) & (NT(4)));
//G73:  achar(11)            => __E_achar_15(52).
	p(NT(11), (NT(52)));
//G74:  achar(11)            => '\t'.
	p(NT(11), (T(30)));
//G75:  lchar(53)            => printable(4).
	p(NT(53), (NT(4)));
//G76:  lchar(53)            => '\t'.
	p(NT(53), (T(30)));
//G77:  lchar(53)            => '\r'.
	p(NT(53), (T(31)));
//G78:  any(50)              => printable(4).
	p(NT(50), (NT(4)));
//G79:  any(50)              => space(1).
	p(NT(50), (NT(1)));
//G80:  __E_skip_line_16(54) => sfirst(55) lrun(56).
	p(NT(54), (NT(55)+NT(56)));
//G81:  __E_skip_line_16(54) => null.
	p(NT(54), (nul));
//G82:  skip_line(24)        => __E_skip_line_16(54) '\n'.
	p(NT(24), (NT(54)+T(7)));
//G83:  __N_4(63)            => '-'.
	p(NT(63), (T(12)));
//G84:  __N_5(64)            => '['.
	p(NT(64), (T(17)));
//G85:  sfirst(55)           => ~( __N_4(63) ) & ~( __N_5(64) ) & lchar(53).	 # conjunctive
	p(NT(55), ~(NT(63)) & ~(NT(64)) & (NT(53)));
//G86:  __E_lrun_17(57)      => lrun(56) lchar(53).
	p(NT(57), (NT(56)+NT(53)));
//G87:  __E_lrun_17(57)      => null.
	p(NT(57), (nul));
//G88:  lrun(56)             => __E_lrun_17(57).
	p(NT(56), (NT(57)));
//G89:  __E_b_18(58)         => blank(2) b(5).
	p(NT(58), (NT(2)+NT(5)));
//G90:  __E_b_18(58)         => null.
	p(NT(58), (nul));
//G91:  b(5)                 => __E_b_18(58).
	p(NT(5), (NT(58)));
//G92:  b1(6)                => blank(2) b(5).
	p(NT(6), (NT(2)+NT(5)));
	#undef T
	#undef NT
	return loaded = true, p;
}

} // namespace hoa_parser_data
