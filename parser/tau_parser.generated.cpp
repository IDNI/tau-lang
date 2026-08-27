// This file is generated from a file parser/tau.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
// productions() lives here so the table is compiled once, not per TU.
//
#include "tau_parser.generated.h"

namespace tau_parser_data {

idni::prods<char_type, terminal_type>& productions() {
	static bool loaded = false;
	static idni::prods<char_type, terminal_type>
		p, nul(idni::lit<char_type, terminal_type>{});
	if (loaded) return p;
	#define  T(x) (idni::prods<char_type, terminal_type>{ terminals[x] })
	#define NT(x) (idni::prods<char_type, terminal_type>{ nts(x) })
//G0:   start(46)            => spec(47).
	p(NT(46), (NT(47)));
//G1:   __E_spec_0(48)       => definitions(49).
	p(NT(48), (NT(49)));
//G2:   __E_spec_0(48)       => null.
	p(NT(48), (nul));
//G3:   main(50)             => wff(36).
	p(NT(50), (NT(36)));
//G4:   __E_spec_1(51)       => '.' _(9).
	p(NT(51), (T(1)+NT(9)));
//G5:   __E_spec_1(51)       => null.
	p(NT(51), (nul));
//G6:   spec(47)             => __E_spec_0(48) _(9) main(50) _(9) __E_spec_1(51).
	p(NT(47), (NT(48)+NT(9)+NT(50)+NT(9)+NT(51)));
//G7:   __E___E_definitions_2_3(53) => rec_relation(54).
	p(NT(53), (NT(54)));
//G8:   __E___E_definitions_2_3(53) => input_def(55).
	p(NT(53), (NT(55)));
//G9:   __E___E_definitions_2_3(53) => output_def(56).
	p(NT(53), (NT(56)));
//G10:  __E_definitions_2(52) => _(9) __E___E_definitions_2_3(53) _(9) '.'.
	p(NT(52), (NT(9)+NT(53)+NT(9)+T(1)));
//G11:  __E_definitions_4(57) => __E_definitions_2(52).
	p(NT(57), (NT(52)));
//G12:  __E_definitions_4(57) => __E_definitions_2(52) __E_definitions_4(57).
	p(NT(57), (NT(52)+NT(57)));
//G13:  definitions(49)      => __E_definitions_4(57).
	p(NT(49), (NT(57)));
//G14:  __E_spec_multiline_5(59) => _(9) spec_part(29) _(9) '.'.
	p(NT(59), (NT(9)+NT(29)+NT(9)+T(1)));
//G15:  __E_spec_multiline_6(60) => null.
	p(NT(60), (nul));
//G16:  __E_spec_multiline_6(60) => __E_spec_multiline_5(59) __E_spec_multiline_6(60).
	p(NT(60), (NT(59)+NT(60)));
//G17:  __E_spec_multiline_7(61) => _(9) '.'.
	p(NT(61), (NT(9)+T(1)));
//G18:  __E_spec_multiline_7(61) => null.
	p(NT(61), (nul));
//G19:  spec_multiline(58)   => __E_spec_multiline_6(60) _(9) spec_part(29) __E_spec_multiline_7(61) _(9).
	p(NT(58), (NT(60)+NT(9)+NT(29)+NT(61)+NT(9)));
//G20:  spec_multiline(58)   => _(9).
	p(NT(58), (NT(9)));
//G21:  spec_part(29)        => rec_relation(54).
	p(NT(29), (NT(54)));
//G22:  spec_part(29)        => input_def(55).
	p(NT(29), (NT(55)));
//G23:  spec_part(29)        => output_def(56).
	p(NT(29), (NT(56)));
//G24:  spec_part(29)        => main(50).
	p(NT(29), (NT(50)));
//G25:  __E_rec_relation_8(63) => capture(17).
	p(NT(63), (NT(17)));
//G26:  __E_rec_relation_8(63) => ref(62).
	p(NT(63), (NT(62)));
//G27:  __E_rec_relation_8(63) => wff(36).
	p(NT(63), (NT(36)));
//G28:  __E_rec_relation_8(63) => bf(38).
	p(NT(63), (NT(38)));
//G29:  rec_relation(54)     => ref(62) _(9) ':' '=' _(9) __E_rec_relation_8(63).
	p(NT(54), (NT(62)+NT(9)+T(2)+T(3)+NT(9)+NT(63)));
//G30:  __E_input_def_9(64)  => typed(65).
	p(NT(64), (NT(65)));
//G31:  __E_input_def_9(64)  => null.
	p(NT(64), (nul));
//G32:  input_def(55)        => io_var_name(19) __E_input_def_9(64) _(9) ':' '=' _(9) 'i' 'n' __(10) stream(66).
	p(NT(55), (NT(19)+NT(64)+NT(9)+T(2)+T(3)+NT(9)+T(4)+T(5)+NT(10)+NT(66)));
//G33:  __E_output_def_10(67) => typed(65).
	p(NT(67), (NT(65)));
//G34:  __E_output_def_10(67) => null.
	p(NT(67), (nul));
//G35:  output_def(56)       => io_var_name(19) __E_output_def_10(67) _(9) ':' '=' _(9) 'o' 'u' 't' __(10) stream(66).
	p(NT(56), (NT(19)+NT(67)+NT(9)+T(2)+T(3)+NT(9)+T(6)+T(7)+T(8)+NT(10)+NT(66)));
//G36:  stream(66)           => 'f' 'i' 'l' 'e' _(9) '(' _(9) q_file_name(68) _(9) ')'.
	p(NT(66), (T(9)+T(4)+T(10)+T(11)+NT(9)+T(12)+NT(9)+NT(68)+NT(9)+T(13)));
//G37:  console_sym(69)      => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(69), (T(14)+T(6)+T(5)+T(15)+T(6)+T(10)+T(11)));
//G38:  stream(66)           => console_sym(69).
	p(NT(66), (NT(69)));
//G39:  __E_ref_11(70)       => offsets(71).
	p(NT(70), (NT(71)));
//G40:  __E_ref_11(70)       => null.
	p(NT(70), (nul));
//G41:  __E_ref_12(73)       => typed(65).
	p(NT(73), (NT(65)));
//G42:  __E_ref_12(73)       => null.
	p(NT(73), (nul));
//G43:  __E_ref_13(74)       => _(9) 'f' 'a' 'l' 'l' 'b' 'a' 'c' 'k' __(10) fp_fallback(75).
	p(NT(74), (NT(9)+T(9)+T(16)+T(10)+T(10)+T(17)+T(16)+T(14)+T(18)+NT(10)+NT(75)));
//G44:  __E_ref_13(74)       => null.
	p(NT(74), (nul));
//G45:  ref(62)              => sym(11) __E_ref_11(70) ref_args(72) __E_ref_12(73) __E_ref_13(74).
	p(NT(62), (NT(11)+NT(70)+NT(72)+NT(73)+NT(74)));
//G46:  first_sym(76)        => 'f' 'i' 'r' 's' 't'.
	p(NT(76), (T(9)+T(4)+T(19)+T(15)+T(8)));
//G47:  fp_fallback(75)      => first_sym(76).
	p(NT(75), (NT(76)));
//G48:  last_sym(77)         => 'l' 'a' 's' 't'.
	p(NT(77), (T(10)+T(16)+T(15)+T(8)));
//G49:  fp_fallback(75)      => last_sym(77).
	p(NT(75), (NT(77)));
//G50:  fp_fallback(75)      => capture(17).
	p(NT(75), (NT(17)));
//G51:  fp_fallback(75)      => ref(62).
	p(NT(75), (NT(62)));
//G52:  fp_fallback(75)      => wff(36).
	p(NT(75), (NT(36)));
//G53:  fp_fallback(75)      => bf(38).
	p(NT(75), (NT(38)));
//G54:  ref_arg(80)          => bf(38).
	p(NT(80), (NT(38)));
//G55:  __E___E_ref_args_14_15(79) => _(9) ref_arg(80).
	p(NT(79), (NT(9)+NT(80)));
//G56:  __E___E_ref_args_14_16(81) => _(9) ',' _(9) ref_arg(80).
	p(NT(81), (NT(9)+T(20)+NT(9)+NT(80)));
//G57:  __E___E_ref_args_14_17(82) => null.
	p(NT(82), (nul));
//G58:  __E___E_ref_args_14_17(82) => __E___E_ref_args_14_16(81) __E___E_ref_args_14_17(82).
	p(NT(82), (NT(81)+NT(82)));
//G59:  __E_ref_args_14(78)  => __E___E_ref_args_14_15(79) __E___E_ref_args_14_17(82).
	p(NT(78), (NT(79)+NT(82)));
//G60:  __E_ref_args_14(78)  => null.
	p(NT(78), (nul));
//G61:  ref_args(72)         => '(' __E_ref_args_14(78) _(9) ')'.
	p(NT(72), (T(12)+NT(78)+NT(9)+T(13)));
//G62:  __E_wff_18(83)       => '(' _(9) wff(36) _(9) ')'.
	p(NT(83), (T(12)+NT(9)+NT(36)+NT(9)+T(13)));
//G63:  wff_parenthesis(37)  => __E_wff_18(83).
	p(NT(37), (NT(83)));
//G64:  wff(36)              => wff_parenthesis(37).
	p(NT(36), (NT(37)));
//G65:  __E___E_wff_19_20(86) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(86), (T(15)+T(6)+T(21)+T(11)+T(8)+T(4)+T(21)+T(11)+T(15)));
//G66:  __E___E_wff_19_20(86) => '<' '>'.
	p(NT(86), (T(22)+T(23)));
//G67:  __E_wff_19(85)       => __E___E_wff_19_20(86) _(9) wff(36).
	p(NT(85), (NT(86)+NT(9)+NT(36)));
//G68:  wff_sometimes(84)    => __E_wff_19(85).
	p(NT(84), (NT(85)));
//G69:  wff(36)              => wff_sometimes(84).
	p(NT(36), (NT(84)));
//G70:  __E___E_wff_21_22(89) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(89), (T(16)+T(10)+T(24)+T(16)+T(25)+T(15)));
//G71:  __E___E_wff_21_22(89) => '[' ']'.
	p(NT(89), (T(26)+T(27)));
//G72:  __E_wff_21(88)       => __E___E_wff_21_22(89) _(9) wff(36).
	p(NT(88), (NT(89)+NT(9)+NT(36)));
//G73:  wff_always(87)       => __E_wff_21(88).
	p(NT(87), (NT(88)));
//G74:  wff(36)              => wff_always(87).
	p(NT(36), (NT(87)));
//G75:  __E_wff_23(91)       => wff(36) _(9) '?' _(9) wff(36) _(9) ':' _(9) wff(36).
	p(NT(91), (NT(36)+NT(9)+T(28)+NT(9)+NT(36)+NT(9)+T(2)+NT(9)+NT(36)));
//G76:  wff_conditional(90)  => __E_wff_23(91).
	p(NT(90), (NT(91)));
//G77:  wff(36)              => wff_conditional(90).
	p(NT(36), (NT(90)));
//G78:  __E_wff_24(93)       => 'a' 'l' 'l' __(10) q_vars(94) __(10) wff(36).
	p(NT(93), (T(16)+T(10)+T(10)+NT(10)+NT(94)+NT(10)+NT(36)));
//G79:  wff_all(92)          => __E_wff_24(93).
	p(NT(92), (NT(93)));
//G80:  wff(36)              => wff_all(92).
	p(NT(36), (NT(92)));
//G81:  __E_wff_25(96)       => 'e' 'x' __(10) q_vars(94) __(10) wff(36).
	p(NT(96), (T(11)+T(29)+NT(10)+NT(94)+NT(10)+NT(36)));
//G82:  wff_ex(95)           => __E_wff_25(96).
	p(NT(95), (NT(96)));
//G83:  wff(36)              => wff_ex(95).
	p(NT(36), (NT(95)));
//G84:  wff_ref(97)          => ref(62).
	p(NT(97), (NT(62)));
//G85:  wff(36)              => wff_ref(97).
	p(NT(36), (NT(97)));
//G86:  wff(36)              => constraint(98).
	p(NT(36), (NT(98)));
//G87:  __E_wff_26(100)      => wff(36) _(9) '-' '>' _(9) wff(36).
	p(NT(100), (NT(36)+NT(9)+T(30)+T(23)+NT(9)+NT(36)));
//G88:  wff_imply(99)        => __E_wff_26(100).
	p(NT(99), (NT(100)));
//G89:  wff(36)              => wff_imply(99).
	p(NT(36), (NT(99)));
//G90:  __E_wff_27(102)      => wff(36) _(9) '<' '-' _(9) wff(36).
	p(NT(102), (NT(36)+NT(9)+T(22)+T(30)+NT(9)+NT(36)));
//G91:  wff_rimply(101)      => __E_wff_27(102).
	p(NT(101), (NT(102)));
//G92:  wff(36)              => wff_rimply(101).
	p(NT(36), (NT(101)));
//G93:  __E_wff_28(104)      => wff(36) _(9) '<' '-' '>' _(9) wff(36).
	p(NT(104), (NT(36)+NT(9)+T(22)+T(30)+T(23)+NT(9)+NT(36)));
//G94:  wff_equiv(103)       => __E_wff_28(104).
	p(NT(103), (NT(104)));
//G95:  wff(36)              => wff_equiv(103).
	p(NT(36), (NT(103)));
//G96:  __E_wff_29(106)      => wff(36) _(9) '|' '|' _(9) wff(36).
	p(NT(106), (NT(36)+NT(9)+T(31)+T(31)+NT(9)+NT(36)));
//G97:  wff_or(105)          => __E_wff_29(106).
	p(NT(105), (NT(106)));
//G98:  wff(36)              => wff_or(105).
	p(NT(36), (NT(105)));
//G99:  __E_wff_30(108)      => wff(36) _(9) '^' '^' _(9) wff(36).
	p(NT(108), (NT(36)+NT(9)+T(32)+T(32)+NT(9)+NT(36)));
//G100: wff_xor(107)         => __E_wff_30(108).
	p(NT(107), (NT(108)));
//G101: wff(36)              => wff_xor(107).
	p(NT(36), (NT(107)));
//G102: __E_wff_31(110)      => wff(36) _(9) '&' '&' _(9) wff(36).
	p(NT(110), (NT(36)+NT(9)+T(33)+T(33)+NT(9)+NT(36)));
//G103: wff_and(109)         => __E_wff_31(110).
	p(NT(109), (NT(110)));
//G104: wff(36)              => wff_and(109).
	p(NT(36), (NT(109)));
//G105: __E_wff_32(112)      => '!' _(9) wff(36).
	p(NT(112), (T(34)+NT(9)+NT(36)));
//G106: wff_neg(111)         => __E_wff_32(112).
	p(NT(111), (NT(112)));
//G107: wff(36)              => wff_neg(111).
	p(NT(36), (NT(111)));
//G108: wff_t(113)           => 'T'.
	p(NT(113), (T(35)));
//G109: wff(36)              => wff_t(113).
	p(NT(36), (NT(113)));
//G110: wff_f(114)           => 'F'.
	p(NT(114), (T(36)));
//G111: wff(36)              => wff_f(114).
	p(NT(36), (NT(114)));
//G112: __E_wff_33(116)      => bf(38) _(9) '<' '=' _(9) bf(38) _(9) '<' '=' _(9) bf(38).
	p(NT(116), (NT(38)+NT(9)+T(22)+T(3)+NT(9)+NT(38)+NT(9)+T(22)+T(3)+NT(9)+NT(38)));
//G113: bf_interval(115)     => __E_wff_33(116).
	p(NT(115), (NT(116)));
//G114: wff(36)              => bf_interval(115).
	p(NT(36), (NT(115)));
//G115: __E_wff_34(118)      => bf(38) _(9) '=' _(9) bf(38).
	p(NT(118), (NT(38)+NT(9)+T(3)+NT(9)+NT(38)));
//G116: bf_eq(117)           => __E_wff_34(118).
	p(NT(117), (NT(118)));
//G117: wff(36)              => bf_eq(117).
	p(NT(36), (NT(117)));
//G118: __E_wff_35(120)      => bf(38) _(9) '!' '=' _(9) bf(38).
	p(NT(120), (NT(38)+NT(9)+T(34)+T(3)+NT(9)+NT(38)));
//G119: bf_neq(119)          => __E_wff_35(120).
	p(NT(119), (NT(120)));
//G120: wff(36)              => bf_neq(119).
	p(NT(36), (NT(119)));
//G121: __E_wff_36(122)      => bf(38) _(9) '<' _(9) bf(38).
	p(NT(122), (NT(38)+NT(9)+T(22)+NT(9)+NT(38)));
//G122: bf_lt(121)           => __E_wff_36(122).
	p(NT(121), (NT(122)));
//G123: wff(36)              => bf_lt(121).
	p(NT(36), (NT(121)));
//G124: __E_wff_37(124)      => bf(38) _(9) '!' '<' _(9) bf(38).
	p(NT(124), (NT(38)+NT(9)+T(34)+T(22)+NT(9)+NT(38)));
//G125: bf_nlt(123)          => __E_wff_37(124).
	p(NT(123), (NT(124)));
//G126: wff(36)              => bf_nlt(123).
	p(NT(36), (NT(123)));
//G127: __E_wff_38(126)      => bf(38) _(9) '<' '=' _(9) bf(38).
	p(NT(126), (NT(38)+NT(9)+T(22)+T(3)+NT(9)+NT(38)));
//G128: bf_lteq(125)         => __E_wff_38(126).
	p(NT(125), (NT(126)));
//G129: wff(36)              => bf_lteq(125).
	p(NT(36), (NT(125)));
//G130: __E_wff_39(128)      => bf(38) _(9) '!' '<' '=' _(9) bf(38).
	p(NT(128), (NT(38)+NT(9)+T(34)+T(22)+T(3)+NT(9)+NT(38)));
//G131: bf_nlteq(127)        => __E_wff_39(128).
	p(NT(127), (NT(128)));
//G132: wff(36)              => bf_nlteq(127).
	p(NT(36), (NT(127)));
//G133: __E_wff_40(130)      => bf(38) _(9) '>' _(9) bf(38).
	p(NT(130), (NT(38)+NT(9)+T(23)+NT(9)+NT(38)));
//G134: bf_gt(129)           => __E_wff_40(130).
	p(NT(129), (NT(130)));
//G135: wff(36)              => bf_gt(129).
	p(NT(36), (NT(129)));
//G136: __E_wff_41(132)      => bf(38) _(9) '!' '>' _(9) bf(38).
	p(NT(132), (NT(38)+NT(9)+T(34)+T(23)+NT(9)+NT(38)));
//G137: bf_ngt(131)          => __E_wff_41(132).
	p(NT(131), (NT(132)));
//G138: wff(36)              => bf_ngt(131).
	p(NT(36), (NT(131)));
//G139: __E_wff_42(134)      => bf(38) _(9) '>' '=' _(9) bf(38).
	p(NT(134), (NT(38)+NT(9)+T(23)+T(3)+NT(9)+NT(38)));
//G140: bf_gteq(133)         => __E_wff_42(134).
	p(NT(133), (NT(134)));
//G141: wff(36)              => bf_gteq(133).
	p(NT(36), (NT(133)));
//G142: __E_wff_43(136)      => bf(38) _(9) '!' '>' '=' _(9) bf(38).
	p(NT(136), (NT(38)+NT(9)+T(34)+T(23)+T(3)+NT(9)+NT(38)));
//G143: bf_ngteq(135)        => __E_wff_43(136).
	p(NT(135), (NT(136)));
//G144: wff(36)              => bf_ngteq(135).
	p(NT(36), (NT(135)));
//G145: wff(36)              => capture(17).
	p(NT(36), (NT(17)));
//G146: __N_0(387)           => bv_type_like(139).
	p(NT(387), (NT(139)));
//G147: __E___E_bf_44_45(138) => bf(38) & ~( __N_0(387) ).	 # conjunctive
	p(NT(138), (NT(38)) & ~(NT(387)));
//G148: __E_bf_44(137)       => '(' _(9) __E___E_bf_44_45(138) _(9) ')'.
	p(NT(137), (T(12)+NT(9)+NT(138)+NT(9)+T(13)));
//G149: bf_parenthesis(39)   => __E_bf_44(137).
	p(NT(39), (NT(137)));
//G150: bf(38)               => bf_parenthesis(39).
	p(NT(38), (NT(39)));
//G151: bf(38)               => ba_constant(140).
	p(NT(38), (NT(140)));
//G152: bf(38)               => variable(141).
	p(NT(38), (NT(141)));
//G153: __E_bf_46(143)       => 'f' 'a' 'l' 'l' __(10) q_vars(94) __(10) bf(38).
	p(NT(143), (T(9)+T(16)+T(10)+T(10)+NT(10)+NT(94)+NT(10)+NT(38)));
//G154: bf_fall(142)         => __E_bf_46(143).
	p(NT(142), (NT(143)));
//G155: bf(38)               => bf_fall(142).
	p(NT(38), (NT(142)));
//G156: __E_bf_47(145)       => 'f' 'e' 'x' __(10) q_vars(94) __(10) bf(38).
	p(NT(145), (T(9)+T(11)+T(29)+NT(10)+NT(94)+NT(10)+NT(38)));
//G157: bf_fex(144)          => __E_bf_47(145).
	p(NT(144), (NT(145)));
//G158: bf(38)               => bf_fex(144).
	p(NT(38), (NT(144)));
//G159: bf_ref(146)          => ref(62).
	p(NT(146), (NT(62)));
//G160: bf(38)               => bf_ref(146).
	p(NT(38), (NT(146)));
//G161: __E___E_bf_48_49(150) => bf_parenthesis(39).
	p(NT(150), (NT(39)));
//G162: __E___E_bf_48_49(150) => ba_constant(140).
	p(NT(150), (NT(140)));
//G163: __E___E_bf_48_49(150) => variable(141).
	p(NT(150), (NT(141)));
//G164: __E___E_bf_48_49(150) => bf_ref(146).
	p(NT(150), (NT(146)));
//G165: __E___E_bf_48_49(150) => bf_t(151).
	p(NT(150), (NT(151)));
//G166: __E___E_bf_48_49(150) => bf_f(152).
	p(NT(150), (NT(152)));
//G167: __E___E_bf_48_49(150) => bf_neg(153).
	p(NT(150), (NT(153)));
//G168: __E___E_bf_48_49(150) => bf_cast(147).
	p(NT(150), (NT(147)));
//G169: __E___E_bf_48_49(150) => capture(17).
	p(NT(150), (NT(17)));
//G170: __E___E_bf_48_49(150) => bf_fall(142).
	p(NT(150), (NT(142)));
//G171: __E___E_bf_48_49(150) => bf_fex(144).
	p(NT(150), (NT(144)));
//G172: bf_cast_oprnd(41)    => __E___E_bf_48_49(150).
	p(NT(41), (NT(150)));
//G173: __E_bf_48(148)       => '(' _(9) 'b' 'v' _(9) '[' _(9) num(149) _(9) ']' _(9) ')' _(9) bf_cast_oprnd(41).
	p(NT(148), (T(12)+NT(9)+T(17)+T(37)+NT(9)+T(26)+NT(9)+NT(149)+NT(9)+T(27)+NT(9)+T(13)+NT(9)+NT(41)));
//G174: bf_cast(147)         => __E_bf_48(148).
	p(NT(147), (NT(148)));
//G175: bf(38)               => bf_cast(147).
	p(NT(38), (NT(147)));
//G176: __N_1(388)           => bf_shr(154).
	p(NT(388), (NT(154)));
//G177: __N_2(389)           => bf_shl(157).
	p(NT(389), (NT(157)));
//G178: __E___E_bf_50_51(156) => bf(38) & ~( __N_1(388) ) & ~( __N_2(389) ).	 # conjunctive
	p(NT(156), (NT(38)) & ~(NT(388)) & ~(NT(389)));
//G179: __E_bf_50(155)       => bf(38) _(9) '>' '>' _(9) __E___E_bf_50_51(156).
	p(NT(155), (NT(38)+NT(9)+T(23)+T(23)+NT(9)+NT(156)));
//G180: bf_shr(154)          => __E_bf_50(155).
	p(NT(154), (NT(155)));
//G181: bf(38)               => bf_shr(154).
	p(NT(38), (NT(154)));
//G182: __N_3(390)           => bf_shr(154).
	p(NT(390), (NT(154)));
//G183: __N_4(391)           => bf_shl(157).
	p(NT(391), (NT(157)));
//G184: __E___E_bf_52_53(159) => bf(38) & ~( __N_3(390) ) & ~( __N_4(391) ).	 # conjunctive
	p(NT(159), (NT(38)) & ~(NT(390)) & ~(NT(391)));
//G185: __E_bf_52(158)       => bf(38) _(9) '<' '<' _(9) __E___E_bf_52_53(159).
	p(NT(158), (NT(38)+NT(9)+T(22)+T(22)+NT(9)+NT(159)));
//G186: bf_shl(157)          => __E_bf_52(158).
	p(NT(157), (NT(158)));
//G187: bf(38)               => bf_shl(157).
	p(NT(38), (NT(157)));
//G188: __N_5(392)           => bf_add(160).
	p(NT(392), (NT(160)));
//G189: __N_6(393)           => bf_sub(163).
	p(NT(393), (NT(163)));
//G190: __E___E_bf_54_55(162) => bf(38) & ~( __N_5(392) ) & ~( __N_6(393) ).	 # conjunctive
	p(NT(162), (NT(38)) & ~(NT(392)) & ~(NT(393)));
//G191: __E_bf_54(161)       => bf(38) _(9) '+' _(9) __E___E_bf_54_55(162).
	p(NT(161), (NT(38)+NT(9)+T(38)+NT(9)+NT(162)));
//G192: bf_add(160)          => __E_bf_54(161).
	p(NT(160), (NT(161)));
//G193: bf(38)               => bf_add(160).
	p(NT(38), (NT(160)));
//G194: __N_7(394)           => bf_add(160).
	p(NT(394), (NT(160)));
//G195: __N_8(395)           => bf_sub(163).
	p(NT(395), (NT(163)));
//G196: __E___E_bf_56_57(165) => bf(38) & ~( __N_7(394) ) & ~( __N_8(395) ).	 # conjunctive
	p(NT(165), (NT(38)) & ~(NT(394)) & ~(NT(395)));
//G197: __E_bf_56(164)       => bf(38) _(9) '-' _(9) __E___E_bf_56_57(165).
	p(NT(164), (NT(38)+NT(9)+T(30)+NT(9)+NT(165)));
//G198: bf_sub(163)          => __E_bf_56(164).
	p(NT(163), (NT(164)));
//G199: bf(38)               => bf_sub(163).
	p(NT(38), (NT(163)));
//G200: __N_9(396)           => bf_mod(166).
	p(NT(396), (NT(166)));
//G201: __N_10(397)          => bf_mul(169).
	p(NT(397), (NT(169)));
//G202: __N_11(398)          => bf_div(170).
	p(NT(398), (NT(170)));
//G203: __E___E_bf_58_59(168) => bf(38) & ~( __N_9(396) ) & ~( __N_10(397) ) & ~( __N_11(398) ).	 # conjunctive
	p(NT(168), (NT(38)) & ~(NT(396)) & ~(NT(397)) & ~(NT(398)));
//G204: __E_bf_58(167)       => bf(38) _(9) '%' _(9) __E___E_bf_58_59(168).
	p(NT(167), (NT(38)+NT(9)+T(39)+NT(9)+NT(168)));
//G205: bf_mod(166)          => __E_bf_58(167).
	p(NT(166), (NT(167)));
//G206: bf(38)               => bf_mod(166).
	p(NT(38), (NT(166)));
//G207: __N_12(399)          => bf_mod(166).
	p(NT(399), (NT(166)));
//G208: __N_13(400)          => bf_mul(169).
	p(NT(400), (NT(169)));
//G209: __N_14(401)          => bf_div(170).
	p(NT(401), (NT(170)));
//G210: __E___E_bf_60_61(172) => bf(38) & ~( __N_12(399) ) & ~( __N_13(400) ) & ~( __N_14(401) ).	 # conjunctive
	p(NT(172), (NT(38)) & ~(NT(399)) & ~(NT(400)) & ~(NT(401)));
//G211: __E_bf_60(171)       => bf(38) _(9) '*' _(9) __E___E_bf_60_61(172).
	p(NT(171), (NT(38)+NT(9)+T(40)+NT(9)+NT(172)));
//G212: bf_mul(169)          => __E_bf_60(171).
	p(NT(169), (NT(171)));
//G213: bf(38)               => bf_mul(169).
	p(NT(38), (NT(169)));
//G214: __N_15(402)          => bf_mod(166).
	p(NT(402), (NT(166)));
//G215: __N_16(403)          => bf_mul(169).
	p(NT(403), (NT(169)));
//G216: __N_17(404)          => bf_div(170).
	p(NT(404), (NT(170)));
//G217: __E___E_bf_62_63(174) => bf(38) & ~( __N_15(402) ) & ~( __N_16(403) ) & ~( __N_17(404) ).	 # conjunctive
	p(NT(174), (NT(38)) & ~(NT(402)) & ~(NT(403)) & ~(NT(404)));
//G218: __E_bf_62(173)       => bf(38) _(9) '/' _(9) __E___E_bf_62_63(174).
	p(NT(173), (NT(38)+NT(9)+T(41)+NT(9)+NT(174)));
//G219: bf_div(170)          => __E_bf_62(173).
	p(NT(170), (NT(173)));
//G220: bf(38)               => bf_div(170).
	p(NT(38), (NT(170)));
//G221: __N_18(405)          => bf_nor(175).
	p(NT(405), (NT(175)));
//G222: __E___E_bf_64_65(177) => bf(38) & ~( __N_18(405) ).	 # conjunctive
	p(NT(177), (NT(38)) & ~(NT(405)));
//G223: __E_bf_64(176)       => bf(38) _(9) '!' '|' _(9) __E___E_bf_64_65(177).
	p(NT(176), (NT(38)+NT(9)+T(34)+T(31)+NT(9)+NT(177)));
//G224: bf_nor(175)          => __E_bf_64(176).
	p(NT(175), (NT(176)));
//G225: bf(38)               => bf_nor(175).
	p(NT(38), (NT(175)));
//G226: __N_19(406)          => bf_xnor(178).
	p(NT(406), (NT(178)));
//G227: __E___E_bf_66_67(180) => bf(38) & ~( __N_19(406) ).	 # conjunctive
	p(NT(180), (NT(38)) & ~(NT(406)));
//G228: __E_bf_66(179)       => bf(38) _(9) '!' '^' _(9) __E___E_bf_66_67(180).
	p(NT(179), (NT(38)+NT(9)+T(34)+T(32)+NT(9)+NT(180)));
//G229: bf_xnor(178)         => __E_bf_66(179).
	p(NT(178), (NT(179)));
//G230: bf(38)               => bf_xnor(178).
	p(NT(38), (NT(178)));
//G231: __N_20(407)          => bf_nand(181).
	p(NT(407), (NT(181)));
//G232: __E___E_bf_68_69(183) => bf(38) & ~( __N_20(407) ).	 # conjunctive
	p(NT(183), (NT(38)) & ~(NT(407)));
//G233: __E_bf_68(182)       => bf(38) _(9) '!' '&' _(9) __E___E_bf_68_69(183).
	p(NT(182), (NT(38)+NT(9)+T(34)+T(33)+NT(9)+NT(183)));
//G234: bf_nand(181)         => __E_bf_68(182).
	p(NT(181), (NT(182)));
//G235: bf(38)               => bf_nand(181).
	p(NT(38), (NT(181)));
//G236: __E_bf_70(185)       => bf(38) _(9) '|' _(9) bf(38).
	p(NT(185), (NT(38)+NT(9)+T(31)+NT(9)+NT(38)));
//G237: bf_or(184)           => __E_bf_70(185).
	p(NT(184), (NT(185)));
//G238: bf(38)               => bf_or(184).
	p(NT(38), (NT(184)));
//G239: __E_bf_71(187)       => bf(38) _(9) '^' _(9) bf(38).
	p(NT(187), (NT(38)+NT(9)+T(32)+NT(9)+NT(38)));
//G240: bf_xor(186)          => __E_bf_71(187).
	p(NT(186), (NT(187)));
//G241: bf(38)               => bf_xor(186).
	p(NT(38), (NT(186)));
//G242: __E___E_bf_72_73(189) => typed(65).
	p(NT(189), (NT(65)));
//G243: __E___E_bf_72_73(189) => null.
	p(NT(189), (nul));
//G244: __E_bf_72(188)       => '1' __E___E_bf_72_73(189).
	p(NT(188), (T(42)+NT(189)));
//G245: bf_t(151)            => __E_bf_72(188).
	p(NT(151), (NT(188)));
//G246: bf(38)               => bf_t(151).
	p(NT(38), (NT(151)));
//G247: __E___E_bf_74_75(191) => typed(65).
	p(NT(191), (NT(65)));
//G248: __E___E_bf_74_75(191) => null.
	p(NT(191), (nul));
//G249: __E_bf_74(190)       => '0' __E___E_bf_74_75(191).
	p(NT(190), (T(43)+NT(191)));
//G250: bf_f(152)            => __E_bf_74(190).
	p(NT(152), (NT(190)));
//G251: bf(38)               => bf_f(152).
	p(NT(38), (NT(152)));
//G252: __E___E_bf_76_77(193) => _(9) '&' _(9).
	p(NT(193), (NT(9)+T(33)+NT(9)));
//G253: __E___E_bf_76_77(193) => __(10).
	p(NT(193), (NT(10)));
//G254: __E_bf_76(192)       => bf(38) __E___E_bf_76_77(193) bf(38).
	p(NT(192), (NT(38)+NT(193)+NT(38)));
//G255: bf_and(45)           => __E_bf_76(192).
	p(NT(45), (NT(192)));
//G256: bf(38)               => bf_and(45).
	p(NT(38), (NT(45)));
//G257: __E___E_bf_78_79(195) => bf_parenthesis(39).
	p(NT(195), (NT(39)));
//G258: __E___E_bf_78_79(195) => ba_constant(140).
	p(NT(195), (NT(140)));
//G259: __E___E_bf_78_79(195) => variable(141).
	p(NT(195), (NT(141)));
//G260: __E___E_bf_78_79(195) => bf_ref(146).
	p(NT(195), (NT(146)));
//G261: __E___E_bf_78_79(195) => bf_t(151).
	p(NT(195), (NT(151)));
//G262: __E___E_bf_78_79(195) => bf_f(152).
	p(NT(195), (NT(152)));
//G263: __E___E_bf_78_79(195) => bf_neg(153).
	p(NT(195), (NT(153)));
//G264: __E___E_bf_78_79(195) => capture(17).
	p(NT(195), (NT(17)));
//G265: bf_neg_oprnd(40)     => __E___E_bf_78_79(195).
	p(NT(40), (NT(195)));
//G266: __E_bf_78(194)       => bf_neg_oprnd(40) _(9) '\''.
	p(NT(194), (NT(40)+NT(9)+T(44)));
//G267: bf_neg(153)          => __E_bf_78(194).
	p(NT(153), (NT(194)));
//G268: bf(38)               => bf_neg(153).
	p(NT(38), (NT(153)));
//G269: __E___E_bf_80_81(197) => bf_parenthesis(39).
	p(NT(197), (NT(39)));
//G270: __E___E_bf_80_81(197) => variable(141).
	p(NT(197), (NT(141)));
//G271: __E___E_bf_80_81(197) => bf_ref(146).
	p(NT(197), (NT(146)));
//G272: __E___E_bf_80_81(197) => bf_neg(153).
	p(NT(197), (NT(153)));
//G273: __E___E_bf_80_81(197) => bf_and_nosep(44).
	p(NT(197), (NT(44)));
//G274: __E___E_bf_80_81(197) => capture(17).
	p(NT(197), (NT(17)));
//G275: bf_and_nosep_1st_oprnd(42) => __E___E_bf_80_81(197).
	p(NT(42), (NT(197)));
//G276: __E___E_bf_80_82(198) => bf_parenthesis(39).
	p(NT(198), (NT(39)));
//G277: __E___E_bf_80_82(198) => ba_constant(140).
	p(NT(198), (NT(140)));
//G278: __E___E_bf_80_82(198) => variable(141).
	p(NT(198), (NT(141)));
//G279: __E___E_bf_80_82(198) => bf_ref(146).
	p(NT(198), (NT(146)));
//G280: __E___E_bf_80_82(198) => bf_neg(153).
	p(NT(198), (NT(153)));
//G281: __E___E_bf_80_82(198) => capture(17).
	p(NT(198), (NT(17)));
//G282: bf_and_nosep_2nd_oprnd(43) => __E___E_bf_80_82(198).
	p(NT(43), (NT(198)));
//G283: __E_bf_80(196)       => bf_and_nosep_1st_oprnd(42) bf_and_nosep_2nd_oprnd(43).
	p(NT(196), (NT(42)+NT(43)));
//G284: bf_and_nosep(44)     => __E_bf_80(196).
	p(NT(44), (NT(196)));
//G285: bf(38)               => bf_and_nosep(44).
	p(NT(38), (NT(44)));
//G286: bf(38)               => capture(17).
	p(NT(38), (NT(17)));
//G287: __E_constraint_83(200) => '[' ctnvar(22) _(9) '!' '=' _(9) num(149) ']'.
	p(NT(200), (T(26)+NT(22)+NT(9)+T(34)+T(3)+NT(9)+NT(149)+T(27)));
//G288: __E_constraint_83(200) => '[' num(149) _(9) '!' '=' _(9) ctnvar(22) ']'.
	p(NT(200), (T(26)+NT(149)+NT(9)+T(34)+T(3)+NT(9)+NT(22)+T(27)));
//G289: ctn_neq(199)         => __E_constraint_83(200).
	p(NT(199), (NT(200)));
//G290: constraint(98)       => ctn_neq(199).
	p(NT(98), (NT(199)));
//G291: __E_constraint_84(202) => '[' ctnvar(22) _(9) '=' _(9) num(149) ']'.
	p(NT(202), (T(26)+NT(22)+NT(9)+T(3)+NT(9)+NT(149)+T(27)));
//G292: __E_constraint_84(202) => '[' num(149) _(9) '=' _(9) ctnvar(22) ']'.
	p(NT(202), (T(26)+NT(149)+NT(9)+T(3)+NT(9)+NT(22)+T(27)));
//G293: ctn_eq(201)          => __E_constraint_84(202).
	p(NT(201), (NT(202)));
//G294: constraint(98)       => ctn_eq(201).
	p(NT(98), (NT(201)));
//G295: __E_constraint_85(204) => '[' ctnvar(22) _(9) '>' '=' _(9) num(149) ']'.
	p(NT(204), (T(26)+NT(22)+NT(9)+T(23)+T(3)+NT(9)+NT(149)+T(27)));
//G296: __E_constraint_85(204) => '[' num(149) _(9) '>' '=' _(9) ctnvar(22) ']'.
	p(NT(204), (T(26)+NT(149)+NT(9)+T(23)+T(3)+NT(9)+NT(22)+T(27)));
//G297: ctn_gteq(203)        => __E_constraint_85(204).
	p(NT(203), (NT(204)));
//G298: constraint(98)       => ctn_gteq(203).
	p(NT(98), (NT(203)));
//G299: __E_constraint_86(206) => '[' ctnvar(22) _(9) '>' _(9) num(149) ']'.
	p(NT(206), (T(26)+NT(22)+NT(9)+T(23)+NT(9)+NT(149)+T(27)));
//G300: __E_constraint_86(206) => '[' num(149) _(9) '>' _(9) ctnvar(22) ']'.
	p(NT(206), (T(26)+NT(149)+NT(9)+T(23)+NT(9)+NT(22)+T(27)));
//G301: ctn_gt(205)          => __E_constraint_86(206).
	p(NT(205), (NT(206)));
//G302: constraint(98)       => ctn_gt(205).
	p(NT(98), (NT(205)));
//G303: __E_constraint_87(208) => '[' ctnvar(22) _(9) '<' '=' _(9) num(149) ']'.
	p(NT(208), (T(26)+NT(22)+NT(9)+T(22)+T(3)+NT(9)+NT(149)+T(27)));
//G304: __E_constraint_87(208) => '[' num(149) _(9) '<' '=' _(9) ctnvar(22) ']'.
	p(NT(208), (T(26)+NT(149)+NT(9)+T(22)+T(3)+NT(9)+NT(22)+T(27)));
//G305: ctn_lteq(207)        => __E_constraint_87(208).
	p(NT(207), (NT(208)));
//G306: constraint(98)       => ctn_lteq(207).
	p(NT(98), (NT(207)));
//G307: __E_constraint_88(210) => '[' ctnvar(22) _(9) '<' _(9) num(149) ']'.
	p(NT(210), (T(26)+NT(22)+NT(9)+T(22)+NT(9)+NT(149)+T(27)));
//G308: __E_constraint_88(210) => '[' num(149) _(9) '<' _(9) ctnvar(22) ']'.
	p(NT(210), (T(26)+NT(149)+NT(9)+T(22)+NT(9)+NT(22)+T(27)));
//G309: ctn_lt(209)          => __E_constraint_88(210).
	p(NT(209), (NT(210)));
//G310: constraint(98)       => ctn_lt(209).
	p(NT(98), (NT(209)));
//G311: __E_ba_constant_89(211) => capture(17).
	p(NT(211), (NT(17)));
//G312: __E_ba_constant_89(211) => source(16).
	p(NT(211), (NT(16)));
//G313: __E_ba_constant_90(212) => typed(65).
	p(NT(212), (NT(65)));
//G314: __E_ba_constant_90(212) => null.
	p(NT(212), (nul));
//G315: ba_constant(140)     => '{' _(9) __E_ba_constant_89(211) _(9) '}' __E_ba_constant_90(212).
	p(NT(140), (T(45)+NT(9)+NT(211)+NT(9)+T(46)+NT(212)));
//G316: __E___E_source_91_92(214) => src_c(27).
	p(NT(214), (NT(27)));
//G317: __E___E_source_91_92(214) => space(2).
	p(NT(214), (NT(2)));
//G318: __E___E_source_91_93(215) => null.
	p(NT(215), (nul));
//G319: __E___E_source_91_93(215) => __E___E_source_91_92(214) __E___E_source_91_93(215).
	p(NT(215), (NT(214)+NT(215)));
//G320: __E_source_91(213)   => __E___E_source_91_93(215) src_c(27).
	p(NT(213), (NT(215)+NT(27)));
//G321: __E_source_91(213)   => null.
	p(NT(213), (nul));
//G322: source(16)           => src_c(27) __E_source_91(213).
	p(NT(16), (NT(27)+NT(213)));
//G323: src_c(27)            => alnum(6).
	p(NT(27), (NT(6)));
//G324: __N_21(408)          => '{'.
	p(NT(408), (T(45)));
//G325: __N_22(409)          => '}'.
	p(NT(409), (T(46)));
//G326: src_c(27)            => ~( __N_21(408) ) & ~( __N_22(409) ) & punct(7).	 # conjunctive
	p(NT(27), ~(NT(408)) & ~(NT(409)) & (NT(7)));
//G327: __E_src_c_94(216)    => src_c(27).
	p(NT(216), (NT(27)));
//G328: __E_src_c_94(216)    => space(2).
	p(NT(216), (NT(2)));
//G329: __E_src_c_95(217)    => null.
	p(NT(217), (nul));
//G330: __E_src_c_95(217)    => __E_src_c_94(216) __E_src_c_95(217).
	p(NT(217), (NT(216)+NT(217)));
//G331: src_c(27)            => '{' __E_src_c_95(217) '}'.
	p(NT(27), (T(45)+NT(217)+T(46)));
//G332: __E_variable_96(218) => uconst(28).
	p(NT(218), (NT(28)));
//G333: __E_variable_96(218) => io_var(219).
	p(NT(218), (NT(219)));
//G334: __E_variable_96(218) => var_name(18).
	p(NT(218), (NT(18)));
//G335: __E_variable_97(220) => typed(65).
	p(NT(220), (NT(65)));
//G336: __E_variable_97(220) => null.
	p(NT(220), (nul));
//G337: variable(141)        => __E_variable_96(218) __E_variable_97(220).
	p(NT(141), (NT(218)+NT(220)));
//G338: __N_23(410)          => 'F'.
	p(NT(410), (T(36)));
//G339: __N_24(411)          => 'T'.
	p(NT(411), (T(35)));
//G340: __E_var_name_98(221) => ~( __N_23(410) ) & ~( __N_24(411) ) & alpha(5).	 # conjunctive
	p(NT(221), ~(NT(410)) & ~(NT(411)) & (NT(5)));
//G341: __E_var_name_99(222) => null.
	p(NT(222), (nul));
//G342: __E_var_name_99(222) => digit(3) __E_var_name_99(222).
	p(NT(222), (NT(3)+NT(222)));
//G343: var_name(18)         => __E_var_name_98(221) __E_var_name_99(222).	 # guarded: charvar
	p(NT(18), (NT(221)+NT(222)));
	p.back().guard = "charvar";
//G344: __N_25(412)          => 'F'.
	p(NT(412), (T(36)));
//G345: __N_26(413)          => 'T'.
	p(NT(413), (T(35)));
//G346: __E_var_name_100(223) => ~( __N_25(412) ) & ~( __N_26(413) ) & alpha(5).	 # conjunctive
	p(NT(223), ~(NT(412)) & ~(NT(413)) & (NT(5)));
//G347: __E_var_name_101(224) => alnum(6).
	p(NT(224), (NT(6)));
//G348: __E_var_name_101(224) => '_'.
	p(NT(224), (T(47)));
//G349: __E_var_name_102(225) => null.
	p(NT(225), (nul));
//G350: __E_var_name_102(225) => __E_var_name_101(224) __E_var_name_102(225).
	p(NT(225), (NT(224)+NT(225)));
//G351: var_name(18)         => __E_var_name_100(223) __E_var_name_102(225).	 # guarded: var
	p(NT(18), (NT(223)+NT(225)));
	p.back().guard = "var";
//G352: io_var(219)          => io_var_name(19) '[' offset(226) ']'.
	p(NT(219), (NT(19)+T(26)+NT(226)+T(27)));
//G353: io_var_name(19)      => chars(25).
	p(NT(19), (NT(25)));
//G354: bv_type_like(139)    => 'b' 'v' _(9) '[' _(9) num(149) _(9) ']'.
	p(NT(139), (T(17)+T(37)+NT(9)+T(26)+NT(9)+NT(149)+NT(9)+T(27)));
//G355: __E___E_uconst_103_104(228) => chars(25) _(9).
	p(NT(228), (NT(25)+NT(9)));
//G356: __E___E_uconst_103_104(228) => null.
	p(NT(228), (nul));
//G357: __E_uconst_103(227)  => __E___E_uconst_103_104(228) ':' _(9) chars(25).
	p(NT(227), (NT(228)+T(2)+NT(9)+NT(25)));
//G358: uconst_name(20)      => __E_uconst_103(227).
	p(NT(20), (NT(227)));
//G359: uconst(28)           => '<' _(9) uconst_name(20) _(9) '>'.
	p(NT(28), (T(22)+NT(9)+NT(20)+NT(9)+T(23)));
//G360: __E_q_vars_105(229)  => _(9) ',' _(9) q_var(26).
	p(NT(229), (NT(9)+T(20)+NT(9)+NT(26)));
//G361: __E_q_vars_106(230)  => null.
	p(NT(230), (nul));
//G362: __E_q_vars_106(230)  => __E_q_vars_105(229) __E_q_vars_106(230).
	p(NT(230), (NT(229)+NT(230)));
//G363: q_vars(94)           => q_var(26) __E_q_vars_106(230).
	p(NT(94), (NT(26)+NT(230)));
//G364: q_var(26)            => capture(17).
	p(NT(26), (NT(17)));
//G365: __N_27(414)          => uconst(28).
	p(NT(414), (NT(28)));
//G366: q_var(26)            => ~( __N_27(414) ) & variable(141).	 # conjunctive
	p(NT(26), ~(NT(414)) & (NT(141)));
//G367: ctnvar(22)           => chars(25).
	p(NT(22), (NT(25)));
//G368: __E_offsets_107(231) => _(9) ',' _(9) offset(226).
	p(NT(231), (NT(9)+T(20)+NT(9)+NT(226)));
//G369: __E_offsets_108(232) => null.
	p(NT(232), (nul));
//G370: __E_offsets_108(232) => __E_offsets_107(231) __E_offsets_108(232).
	p(NT(232), (NT(231)+NT(232)));
//G371: offsets(71)          => '[' _(9) offset(226) __E_offsets_108(232) _(9) ']'.
	p(NT(71), (T(26)+NT(9)+NT(226)+NT(232)+NT(9)+T(27)));
//G372: offset(226)          => integer(15).
	p(NT(226), (NT(15)));
//G373: offset(226)          => capture(17).
	p(NT(226), (NT(17)));
//G374: offset(226)          => shift(233).
	p(NT(226), (NT(233)));
//G375: __N_28(415)          => io_var(219).
	p(NT(415), (NT(219)));
//G376: __E_offset_109(234)  => variable(141) & ~( __N_28(415) ).	 # conjunctive
	p(NT(234), (NT(141)) & ~(NT(415)));
//G377: offset(226)          => __E_offset_109(234).
	p(NT(226), (NT(234)));
//G378: __E_shift_110(235)   => capture(17).
	p(NT(235), (NT(17)));
//G379: __N_29(416)          => io_var(219).
	p(NT(416), (NT(219)));
//G380: __E___E_shift_110_111(236) => variable(141) & ~( __N_29(416) ).	 # conjunctive
	p(NT(236), (NT(141)) & ~(NT(416)));
//G381: __E_shift_110(235)   => __E___E_shift_110_111(236).
	p(NT(235), (NT(236)));
//G382: shift(233)           => __E_shift_110(235) _(9) '-' _(9) num(149).
	p(NT(233), (NT(235)+NT(9)+T(30)+NT(9)+NT(149)));
//G383: __E_chars_112(237)   => alnum(6).
	p(NT(237), (NT(6)));
//G384: __E_chars_112(237)   => '_'.
	p(NT(237), (T(47)));
//G385: __E_chars_113(238)   => null.
	p(NT(238), (nul));
//G386: __E_chars_113(238)   => __E_chars_112(237) __E_chars_113(238).
	p(NT(238), (NT(237)+NT(238)));
//G387: chars(25)            => alpha(5) __E_chars_113(238).
	p(NT(25), (NT(5)+NT(238)));
//G388: __E_digits_114(239)  => digit(3).
	p(NT(239), (NT(3)));
//G389: __E_digits_114(239)  => digit(3) __E_digits_114(239).
	p(NT(239), (NT(3)+NT(239)));
//G390: digits(12)           => __E_digits_114(239).
	p(NT(12), (NT(239)));
//G391: num(149)             => digits(12).
	p(NT(149), (NT(12)));
//G392: __E_integer_115(240) => '-'.
	p(NT(240), (T(30)));
//G393: __E_integer_115(240) => null.
	p(NT(240), (nul));
//G394: integer(15)          => __E_integer_115(240) _(9) digits(12).
	p(NT(15), (NT(240)+NT(9)+NT(12)));
//G395: sym(11)              => chars(25).
	p(NT(11), (NT(25)));
//G396: capture(17)          => '$' chars(25).
	p(NT(17), (T(48)+NT(25)));
//G397: typed(65)            => _(9) ':' _(9) type(13) _(9) '[' _(9) subtype(14) _(9) ']'.
	p(NT(65), (NT(9)+T(2)+NT(9)+NT(13)+NT(9)+T(26)+NT(9)+NT(14)+NT(9)+T(27)));
//G398: __N_30(417)          => 'b' 'v'.
	p(NT(417), (T(17)+T(37)));
//G399: __E_typed_116(241)   => ~( __N_30(417) ) & type(13).	 # conjunctive
	p(NT(241), ~(NT(417)) & (NT(13)));
//G400: typed(65)            => _(9) ':' _(9) __E_typed_116(241).
	p(NT(65), (NT(9)+T(2)+NT(9)+NT(241)));
//G401: type(13)             => chars(25).
	p(NT(13), (NT(25)));
//G402: subtype(14)          => num(149).
	p(NT(14), (NT(149)));
//G403: __E_comment_117(243) => printable(8).
	p(NT(243), (NT(8)));
//G404: __E_comment_117(243) => '\t'.
	p(NT(243), (T(49)));
//G405: __E_comment_118(244) => null.
	p(NT(244), (nul));
//G406: __E_comment_118(244) => __E_comment_117(243) __E_comment_118(244).
	p(NT(244), (NT(243)+NT(244)));
//G407: __E_comment_119(245) => '\n'.
	p(NT(245), (T(50)));
//G408: __E_comment_119(245) => '\r'.
	p(NT(245), (T(51)));
//G409: __E_comment_119(245) => eof(1).
	p(NT(245), (NT(1)));
//G410: comment(242)         => '#' __E_comment_118(244) __E_comment_119(245).
	p(NT(242), (T(52)+NT(244)+NT(245)));
//G411: __(10)               => space(2).
	p(NT(10), (NT(2)));
//G412: __(10)               => comment(242).
	p(NT(10), (NT(242)));
//G413: __(10)               => __(10) space(2).
	p(NT(10), (NT(10)+NT(2)));
//G414: __(10)               => __(10) comment(242).
	p(NT(10), (NT(10)+NT(242)));
//G415: _(9)                 => __(10).
	p(NT(9), (NT(10)));
//G416: _(9)                 => null.
	p(NT(9), (nul));
//G417: cli(246)             => _(9).
	p(NT(246), (NT(9)));
//G418: __E_cli_120(248)     => '.' _(9) cli_command(247) _(9).
	p(NT(248), (T(1)+NT(9)+NT(247)+NT(9)));
//G419: __E_cli_121(249)     => null.
	p(NT(249), (nul));
//G420: __E_cli_121(249)     => __E_cli_120(248) __E_cli_121(249).
	p(NT(249), (NT(248)+NT(249)));
//G421: __E_cli_122(250)     => '.' _(9).
	p(NT(250), (T(1)+NT(9)));
//G422: __E_cli_122(250)     => null.
	p(NT(250), (nul));
//G423: cli(246)             => _(9) cli_command(247) _(9) __E_cli_121(249) __E_cli_122(250).
	p(NT(246), (NT(9)+NT(247)+NT(9)+NT(249)+NT(250)));
//G424: __E_cli_command_123(253) => 'q'.
	p(NT(253), (T(53)));
//G425: __E_cli_command_123(253) => 'q' 'u' 'i' 't'.
	p(NT(253), (T(53)+T(7)+T(4)+T(8)));
//G426: quit_sym(252)        => __E_cli_command_123(253).
	p(NT(252), (NT(253)));
//G427: quit_cmd(251)        => quit_sym(252).
	p(NT(251), (NT(252)));
//G428: cli_command(247)     => quit_cmd(251).
	p(NT(247), (NT(251)));
//G429: __E_cli_command_124(256) => 'v'.
	p(NT(256), (T(37)));
//G430: __E_cli_command_124(256) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(256), (T(37)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G431: version_sym(255)     => __E_cli_command_124(256).
	p(NT(255), (NT(256)));
//G432: version_cmd(254)     => version_sym(255).
	p(NT(254), (NT(255)));
//G433: cli_command(247)     => version_cmd(254).
	p(NT(247), (NT(254)));
//G434: __E_cli_command_125(259) => 'c'.
	p(NT(259), (T(14)));
//G435: __E_cli_command_125(259) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(259), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G436: clear_sym(258)       => __E_cli_command_125(259).
	p(NT(258), (NT(259)));
//G437: clear_cmd(257)       => clear_sym(258).
	p(NT(257), (NT(258)));
//G438: cli_command(247)     => clear_cmd(257).
	p(NT(247), (NT(257)));
//G439: __E___E_cli_command_126_127(263) => 'h'.
	p(NT(263), (T(54)));
//G440: __E___E_cli_command_126_127(263) => 'h' 'e' 'l' 'p'.
	p(NT(263), (T(54)+T(11)+T(10)+T(55)));
//G441: help_sym(262)        => __E___E_cli_command_126_127(263).
	p(NT(262), (NT(263)));
//G442: __E___E_cli_command_126_128(264) => __(10) help_arg(265).
	p(NT(264), (NT(10)+NT(265)));
//G443: __E___E_cli_command_126_128(264) => null.
	p(NT(264), (nul));
//G444: __E_cli_command_126(261) => help_sym(262) __E___E_cli_command_126_128(264).
	p(NT(261), (NT(262)+NT(264)));
//G445: help_cmd(260)        => __E_cli_command_126(261).
	p(NT(260), (NT(261)));
//G446: cli_command(247)     => help_cmd(260).
	p(NT(247), (NT(260)));
//G447: file_sym(268)        => 'f' 'i' 'l' 'e'.
	p(NT(268), (T(9)+T(4)+T(10)+T(11)));
//G448: __E_cli_command_129(267) => file_sym(268) __(10) q_string(269).
	p(NT(267), (NT(268)+NT(10)+NT(269)));
//G449: file_cmd(266)        => __E_cli_command_129(267).
	p(NT(266), (NT(267)));
//G450: cli_command(247)     => file_cmd(266).
	p(NT(247), (NT(266)));
//G451: valid_sym(272)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(272), (T(37)+T(16)+T(10)+T(4)+T(56)));
//G452: __E_cli_command_130(271) => valid_sym(272) __(10) spec_arg(35).
	p(NT(271), (NT(272)+NT(10)+NT(35)));
//G453: valid_cmd(270)       => __E_cli_command_130(271).
	p(NT(270), (NT(271)));
//G454: cli_command(247)     => valid_cmd(270).
	p(NT(247), (NT(270)));
//G455: sat_sym(275)         => 's' 'a' 't'.
	p(NT(275), (T(15)+T(16)+T(8)));
//G456: __E_cli_command_131(274) => sat_sym(275) __(10) spec_arg(35).
	p(NT(274), (NT(275)+NT(10)+NT(35)));
//G457: sat_cmd(273)         => __E_cli_command_131(274).
	p(NT(273), (NT(274)));
//G458: cli_command(247)     => sat_cmd(273).
	p(NT(247), (NT(273)));
//G459: unsat_sym(278)       => 'u' 'n' 's' 'a' 't'.
	p(NT(278), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G460: __E_cli_command_132(277) => unsat_sym(278) __(10) spec_arg(35).
	p(NT(277), (NT(278)+NT(10)+NT(35)));
//G461: unsat_cmd(276)       => __E_cli_command_132(277).
	p(NT(276), (NT(277)));
//G462: cli_command(247)     => unsat_cmd(276).
	p(NT(247), (NT(276)));
//G463: solve_sym(281)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(281), (T(15)+T(6)+T(10)+T(37)+T(11)));
//G464: __E___E_cli_command_133_134(282) => solve_options(283).
	p(NT(282), (NT(283)));
//G465: __E___E_cli_command_133_134(282) => null.
	p(NT(282), (nul));
//G466: __E_cli_command_133(280) => solve_sym(281) __E___E_cli_command_133_134(282) __(10) wff_cmd_arg(32).
	p(NT(280), (NT(281)+NT(282)+NT(10)+NT(32)));
//G467: solve_cmd(279)       => __E_cli_command_133(280).
	p(NT(279), (NT(280)));
//G468: cli_command(247)     => solve_cmd(279).
	p(NT(247), (NT(279)));
//G469: lgrs_sym(286)        => 'l' 'g' 'r' 's'.
	p(NT(286), (T(10)+T(57)+T(19)+T(15)));
//G470: __E_cli_command_135(285) => lgrs_sym(286) __(10) wff_cmd_arg(32).
	p(NT(285), (NT(286)+NT(10)+NT(32)));
//G471: lgrs_cmd(284)        => __E_cli_command_135(285).
	p(NT(284), (NT(285)));
//G472: cli_command(247)     => lgrs_cmd(284).
	p(NT(247), (NT(284)));
//G473: __E___E_cli_command_136_137(290) => 'r'.
	p(NT(290), (T(19)));
//G474: __E___E_cli_command_136_137(290) => 'r' 'u' 'n'.
	p(NT(290), (T(19)+T(7)+T(5)));
//G475: run_sym(289)         => __E___E_cli_command_136_137(290).
	p(NT(289), (NT(290)));
//G476: __E_cli_command_136(288) => run_sym(289) __(10) spec_arg(35).
	p(NT(288), (NT(289)+NT(10)+NT(35)));
//G477: run_cmd(287)         => __E_cli_command_136(288).
	p(NT(287), (NT(288)));
//G478: cli_command(247)     => run_cmd(287).
	p(NT(247), (NT(287)));
//G479: __E___E_cli_command_138_139(294) => 'n'.
	p(NT(294), (T(5)));
//G480: __E___E_cli_command_138_139(294) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(294), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(58)+T(11)));
//G481: normalize_sym(293)   => __E___E_cli_command_138_139(294).
	p(NT(293), (NT(294)));
//G482: __E_cli_command_138(292) => normalize_sym(293) __(10) spec_arg(35).
	p(NT(292), (NT(293)+NT(10)+NT(35)));
//G483: normalize_cmd(291)   => __E_cli_command_138(292).
	p(NT(291), (NT(292)));
//G484: cli_command(247)     => normalize_cmd(291).
	p(NT(247), (NT(291)));
//G485: __E___E_cli_command_140_141(298) => 's'.
	p(NT(298), (T(15)));
//G486: __E___E_cli_command_140_141(298) => 's' 'u' 'b' 's' 't'.
	p(NT(298), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G487: __E___E_cli_command_140_141(298) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(298), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G488: subst_sym(297)       => __E___E_cli_command_140_141(298).
	p(NT(297), (NT(298)));
//G489: __E_cli_command_140(296) => subst_sym(297) __(10) nf_cmd_arg(33) _(9) '[' _(9) nf_cmd_arg(33) _(9) '/' _(9) nf_cmd_arg(33) _(9) ']'.
	p(NT(296), (NT(297)+NT(10)+NT(33)+NT(9)+T(26)+NT(9)+NT(33)+NT(9)+T(41)+NT(9)+NT(33)+NT(9)+T(27)));
//G490: subst_cmd(295)       => __E_cli_command_140(296).
	p(NT(295), (NT(296)));
//G491: cli_command(247)     => subst_cmd(295).
	p(NT(247), (NT(295)));
//G492: __E___E_cli_command_142_143(302) => 'i'.
	p(NT(302), (T(4)));
//G493: __E___E_cli_command_142_143(302) => 'i' 'n' 's' 't'.
	p(NT(302), (T(4)+T(5)+T(15)+T(8)));
//G494: __E___E_cli_command_142_143(302) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(302), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G495: inst_sym(301)        => __E___E_cli_command_142_143(302).
	p(NT(301), (NT(302)));
//G496: __E_cli_command_142(300) => inst_sym(301) __(10) inst_args(30).
	p(NT(300), (NT(301)+NT(10)+NT(30)));
//G497: inst_cmd(299)        => __E_cli_command_142(300).
	p(NT(299), (NT(300)));
//G498: cli_command(247)     => inst_cmd(299).
	p(NT(247), (NT(299)));
//G499: dnf_sym(305)         => 'd' 'n' 'f'.
	p(NT(305), (T(56)+T(5)+T(9)));
//G500: __E_cli_command_144(304) => dnf_sym(305) __(10) nf_cmd_arg(33).
	p(NT(304), (NT(305)+NT(10)+NT(33)));
//G501: dnf_cmd(303)         => __E_cli_command_144(304).
	p(NT(303), (NT(304)));
//G502: cli_command(247)     => dnf_cmd(303).
	p(NT(247), (NT(303)));
//G503: cnf_sym(308)         => 'c' 'n' 'f'.
	p(NT(308), (T(14)+T(5)+T(9)));
//G504: __E_cli_command_145(307) => cnf_sym(308) __(10) nf_cmd_arg(33).
	p(NT(307), (NT(308)+NT(10)+NT(33)));
//G505: cnf_cmd(306)         => __E_cli_command_145(307).
	p(NT(306), (NT(307)));
//G506: cli_command(247)     => cnf_cmd(306).
	p(NT(247), (NT(306)));
//G507: nnf_sym(311)         => 'n' 'n' 'f'.
	p(NT(311), (T(5)+T(5)+T(9)));
//G508: __E_cli_command_146(310) => nnf_sym(311) __(10) nf_cmd_arg(33).
	p(NT(310), (NT(311)+NT(10)+NT(33)));
//G509: nnf_cmd(309)         => __E_cli_command_146(310).
	p(NT(309), (NT(310)));
//G510: cli_command(247)     => nnf_cmd(309).
	p(NT(247), (NT(309)));
//G511: mnf_sym(314)         => 'm' 'n' 'f'.
	p(NT(314), (T(21)+T(5)+T(9)));
//G512: __E_cli_command_147(313) => mnf_sym(314) __(10) nf_cmd_arg(33).
	p(NT(313), (NT(314)+NT(10)+NT(33)));
//G513: mnf_cmd(312)         => __E_cli_command_147(313).
	p(NT(312), (NT(313)));
//G514: cli_command(247)     => mnf_cmd(312).
	p(NT(247), (NT(312)));
//G515: onf_sym(317)         => 'o' 'n' 'f'.
	p(NT(317), (T(6)+T(5)+T(9)));
//G516: __E_cli_command_148(316) => onf_sym(317) __(10) variable(141) __(10) onf_cmd_arg(34).
	p(NT(316), (NT(317)+NT(10)+NT(141)+NT(10)+NT(34)));
//G517: onf_cmd(315)         => __E_cli_command_148(316).
	p(NT(315), (NT(316)));
//G518: cli_command(247)     => onf_cmd(315).
	p(NT(247), (NT(315)));
//G519: qelim_sym(320)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(320), (T(53)+T(11)+T(10)+T(4)+T(21)));
//G520: __E_cli_command_149(319) => qelim_sym(320) __(10) wff_cmd_arg(32).
	p(NT(319), (NT(320)+NT(10)+NT(32)));
//G521: qelim_cmd(318)       => __E_cli_command_149(319).
	p(NT(318), (NT(319)));
//G522: cli_command(247)     => qelim_cmd(318).
	p(NT(247), (NT(318)));
//G523: get_sym(323)         => 'g' 'e' 't'.
	p(NT(323), (T(57)+T(11)+T(8)));
//G524: __E___E_cli_command_150_151(324) => __(10) option_name(23).
	p(NT(324), (NT(10)+NT(23)));
//G525: __E___E_cli_command_150_151(324) => null.
	p(NT(324), (nul));
//G526: __E_cli_command_150(322) => get_sym(323) __E___E_cli_command_150_151(324).
	p(NT(322), (NT(323)+NT(324)));
//G527: get_cmd(321)         => __E_cli_command_150(322).
	p(NT(321), (NT(322)));
//G528: cli_command(247)     => get_cmd(321).
	p(NT(247), (NT(321)));
//G529: set_sym(327)         => 's' 'e' 't'.
	p(NT(327), (T(15)+T(11)+T(8)));
//G530: __E___E_cli_command_152_153(328) => __(10).
	p(NT(328), (NT(10)));
//G531: __E___E_cli_command_152_153(328) => _(9) '=' _(9).
	p(NT(328), (NT(9)+T(3)+NT(9)));
//G532: __E_cli_command_152(326) => set_sym(327) __(10) option_name(23) __E___E_cli_command_152_153(328) option_value(24).
	p(NT(326), (NT(327)+NT(10)+NT(23)+NT(328)+NT(24)));
//G533: set_cmd(325)         => __E_cli_command_152(326).
	p(NT(325), (NT(326)));
//G534: cli_command(247)     => set_cmd(325).
	p(NT(247), (NT(325)));
//G535: enable_sym(331)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(331), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G536: __E_cli_command_154(330) => enable_sym(331) __(10) option_name(23).
	p(NT(330), (NT(331)+NT(10)+NT(23)));
//G537: enable_cmd(329)      => __E_cli_command_154(330).
	p(NT(329), (NT(330)));
//G538: cli_command(247)     => enable_cmd(329).
	p(NT(247), (NT(329)));
//G539: disable_sym(334)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(334), (T(56)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G540: __E_cli_command_155(333) => disable_sym(334) __(10) option_name(23).
	p(NT(333), (NT(334)+NT(10)+NT(23)));
//G541: disable_cmd(332)     => __E_cli_command_155(333).
	p(NT(332), (NT(333)));
//G542: cli_command(247)     => disable_cmd(332).
	p(NT(247), (NT(332)));
//G543: toggle_sym(337)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(337), (T(8)+T(6)+T(57)+T(57)+T(10)+T(11)));
//G544: __E_cli_command_156(336) => toggle_sym(337) __(10) option_name(23).
	p(NT(336), (NT(337)+NT(10)+NT(23)));
//G545: toggle_cmd(335)      => __E_cli_command_156(336).
	p(NT(335), (NT(336)));
//G546: cli_command(247)     => toggle_cmd(335).
	p(NT(247), (NT(335)));
//G547: __E___E_cli_command_157_158(341) => 'd' 'e' 'f' 's'.
	p(NT(341), (T(56)+T(11)+T(9)+T(15)));
//G548: __E___E_cli_command_157_158(341) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(341), (T(56)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G549: def_sym(340)         => __E___E_cli_command_157_158(341).
	p(NT(340), (NT(341)));
//G550: __E_cli_command_157(339) => def_sym(340).
	p(NT(339), (NT(340)));
//G551: def_list_cmd(338)    => __E_cli_command_157(339).
	p(NT(338), (NT(339)));
//G552: cli_command(247)     => def_list_cmd(338).
	p(NT(247), (NT(338)));
//G553: __E_cli_command_159(343) => def_sym(340) __(10) num(149).
	p(NT(343), (NT(340)+NT(10)+NT(149)));
//G554: def_print_cmd(342)   => __E_cli_command_159(343).
	p(NT(342), (NT(343)));
//G555: cli_command(247)     => def_print_cmd(342).
	p(NT(247), (NT(342)));
//G556: def_rr_cmd(344)      => rec_relation(54).
	p(NT(344), (NT(54)));
//G557: cli_command(247)     => def_rr_cmd(344).
	p(NT(247), (NT(344)));
//G558: def_input_cmd(345)   => input_def(55).
	p(NT(345), (NT(55)));
//G559: cli_command(247)     => def_input_cmd(345).
	p(NT(247), (NT(345)));
//G560: def_output_cmd(346)  => output_def(56).
	p(NT(346), (NT(56)));
//G561: cli_command(247)     => def_output_cmd(346).
	p(NT(247), (NT(346)));
//G562: __E___E_cli_command_160_161(350) => 'h' 'i' 's' 't'.
	p(NT(350), (T(54)+T(4)+T(15)+T(8)));
//G563: __E___E_cli_command_160_161(350) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(350), (T(54)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G564: history_sym(349)     => __E___E_cli_command_160_161(350).
	p(NT(349), (NT(350)));
//G565: __E_cli_command_160(348) => history_sym(349).
	p(NT(348), (NT(349)));
//G566: history_list_cmd(347) => __E_cli_command_160(348).
	p(NT(347), (NT(348)));
//G567: cli_command(247)     => history_list_cmd(347).
	p(NT(247), (NT(347)));
//G568: __E_cli_command_162(352) => history_sym(349) __(10) history(353).
	p(NT(352), (NT(349)+NT(10)+NT(353)));
//G569: history_print_cmd(351) => __E_cli_command_162(352).
	p(NT(351), (NT(352)));
//G570: cli_command(247)     => history_print_cmd(351).
	p(NT(247), (NT(351)));
//G571: __E_cli_command_163(355) => ref(62).
	p(NT(355), (NT(62)));
//G572: __E_cli_command_163(355) => wff(36).
	p(NT(355), (NT(36)));
//G573: __E_cli_command_163(355) => bf(38).
	p(NT(355), (NT(38)));
//G574: history_store_cmd(354) => __E_cli_command_163(355).
	p(NT(354), (NT(355)));
//G575: cli_command(247)     => history_store_cmd(354).
	p(NT(247), (NT(354)));
//G576: __E_solve_options_164(356) => __(10) solve_option(357).
	p(NT(356), (NT(10)+NT(357)));
//G577: __E_solve_options_165(358) => null.
	p(NT(358), (nul));
//G578: __E_solve_options_165(358) => __E_solve_options_164(356) __E_solve_options_165(358).
	p(NT(358), (NT(356)+NT(358)));
//G579: solve_options(283)   => __E_solve_options_165(358).
	p(NT(283), (NT(358)));
//G580: __E_solve_option_166(359) => solver_mode(360).
	p(NT(359), (NT(360)));
//G581: __E_solve_option_166(359) => type(13).
	p(NT(359), (NT(13)));
//G582: solve_option(357)    => '-' '-' __E_solve_option_166(359).
	p(NT(357), (T(30)+T(30)+NT(359)));
//G583: __E_solver_mode_167(362) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(362), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G584: __E_solver_mode_167(362) => 'm' 'i' 'n'.
	p(NT(362), (T(21)+T(4)+T(5)));
//G585: solver_mode_minimum(361) => __E_solver_mode_167(362).
	p(NT(361), (NT(362)));
//G586: solver_mode(360)     => solver_mode_minimum(361).
	p(NT(360), (NT(361)));
//G587: __E_solver_mode_168(364) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(364), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G588: __E_solver_mode_168(364) => 'm' 'a' 'x'.
	p(NT(364), (T(21)+T(16)+T(29)));
//G589: solver_mode_maximum(363) => __E_solver_mode_168(364).
	p(NT(363), (NT(364)));
//G590: solver_mode(360)     => solver_mode_maximum(363).
	p(NT(360), (NT(363)));
//G591: q_file_name(68)      => '"' file_name(21) '"'.
	p(NT(68), (T(59)+NT(21)+T(59)));
//G592: __E_file_name_169(365) => printable(8).
	p(NT(365), (NT(8)));
//G593: __E_file_name_169(365) => printable(8) __E_file_name_169(365).
	p(NT(365), (NT(8)+NT(365)));
//G594: file_name(21)        => __E_file_name_169(365).
	p(NT(21), (NT(365)));
//G595: __E_option_name_170(366) => alnum(6).
	p(NT(366), (NT(6)));
//G596: __E_option_name_170(366) => alnum(6) __E_option_name_170(366).
	p(NT(366), (NT(6)+NT(366)));
//G597: option_name(23)      => __E_option_name_170(366).
	p(NT(23), (NT(366)));
//G598: __E_option_value_171(367) => alnum(6).
	p(NT(367), (NT(6)));
//G599: __E_option_value_171(367) => '.'.
	p(NT(367), (T(1)));
//G600: __E_option_value_172(368) => __E_option_value_171(367).
	p(NT(368), (NT(367)));
//G601: __E_option_value_172(368) => __E_option_value_171(367) __E_option_value_172(368).
	p(NT(368), (NT(367)+NT(368)));
//G602: option_value(24)     => __E_option_value_172(368).
	p(NT(24), (NT(368)));
//G603: bf_cmd_arg(31)       => history(353).
	p(NT(31), (NT(353)));
//G604: bf_cmd_arg(31)       => bf(38).
	p(NT(31), (NT(38)));
//G605: wff_cmd_arg(32)      => history(353).
	p(NT(32), (NT(353)));
//G606: wff_cmd_arg(32)      => wff(36).
	p(NT(32), (NT(36)));
//G607: nf_cmd_arg(33)       => history(353).
	p(NT(33), (NT(353)));
//G608: nf_cmd_arg(33)       => ref(62).
	p(NT(33), (NT(62)));
//G609: nf_cmd_arg(33)       => wff(36).
	p(NT(33), (NT(36)));
//G610: nf_cmd_arg(33)       => bf(38).
	p(NT(33), (NT(38)));
//G611: onf_cmd_arg(34)      => history(353).
	p(NT(34), (NT(353)));
//G612: onf_cmd_arg(34)      => wff(36).
	p(NT(34), (NT(36)));
//G613: spec_arg(35)         => history(353).
	p(NT(35), (NT(353)));
//G614: spec_arg(35)         => spec(47).
	p(NT(35), (NT(47)));
//G615: spec_arg(35)         => ref(62).
	p(NT(35), (NT(62)));
//G616: spec_arg(35)         => wff(36).
	p(NT(35), (NT(36)));
//G617: spec_arg(35)         => bf(38).
	p(NT(35), (NT(38)));
//G618: inst_args(30)        => wff_cmd_arg(32) _(9) '[' _(9) bf(38) _(9) '/' _(9) bf_cmd_arg(31) _(9) ']'.
	p(NT(30), (NT(32)+NT(9)+T(26)+NT(9)+NT(38)+NT(9)+T(41)+NT(9)+NT(31)+NT(9)+T(27)));
//G619: inst_args(30)        => bf_cmd_arg(31) _(9) '[' _(9) bf(38) _(9) '/' _(9) bf_cmd_arg(31) _(9) ']'.
	p(NT(30), (NT(31)+NT(9)+T(26)+NT(9)+NT(38)+NT(9)+T(41)+NT(9)+NT(31)+NT(9)+T(27)));
//G620: help_arg(265)        => help_sym(262).
	p(NT(265), (NT(262)));
//G621: help_arg(265)        => version_sym(255).
	p(NT(265), (NT(255)));
//G622: help_arg(265)        => quit_sym(252).
	p(NT(265), (NT(252)));
//G623: help_arg(265)        => clear_sym(258).
	p(NT(265), (NT(258)));
//G624: help_arg(265)        => get_sym(323).
	p(NT(265), (NT(323)));
//G625: help_arg(265)        => set_sym(327).
	p(NT(265), (NT(327)));
//G626: help_arg(265)        => enable_sym(331).
	p(NT(265), (NT(331)));
//G627: help_arg(265)        => disable_sym(334).
	p(NT(265), (NT(334)));
//G628: help_arg(265)        => toggle_sym(337).
	p(NT(265), (NT(337)));
//G629: help_arg(265)        => file_sym(268).
	p(NT(265), (NT(268)));
//G630: help_arg(265)        => history_sym(349).
	p(NT(265), (NT(349)));
//G631: help_arg(265)        => abs_history_sym(369).
	p(NT(265), (NT(369)));
//G632: help_arg(265)        => rel_history_sym(370).
	p(NT(265), (NT(370)));
//G633: help_arg(265)        => selection_sym(371).
	p(NT(265), (NT(371)));
//G634: help_arg(265)        => def_sym(340).
	p(NT(265), (NT(340)));
//G635: help_arg(265)        => inst_sym(301).
	p(NT(265), (NT(301)));
//G636: help_arg(265)        => subst_sym(297).
	p(NT(265), (NT(297)));
//G637: help_arg(265)        => normalize_sym(293).
	p(NT(265), (NT(293)));
//G638: help_arg(265)        => execute_sym(372).
	p(NT(265), (NT(372)));
//G639: help_arg(265)        => solve_sym(281).
	p(NT(265), (NT(281)));
//G640: help_arg(265)        => valid_sym(272).
	p(NT(265), (NT(272)));
//G641: help_arg(265)        => sat_sym(275).
	p(NT(265), (NT(275)));
//G642: help_arg(265)        => unsat_sym(278).
	p(NT(265), (NT(278)));
//G643: help_arg(265)        => run_sym(289).
	p(NT(265), (NT(289)));
//G644: help_arg(265)        => dnf_sym(305).
	p(NT(265), (NT(305)));
//G645: help_arg(265)        => cnf_sym(308).
	p(NT(265), (NT(308)));
//G646: help_arg(265)        => snf_sym(373).
	p(NT(265), (NT(373)));
//G647: help_arg(265)        => nnf_sym(311).
	p(NT(265), (NT(311)));
//G648: help_arg(265)        => mnf_sym(314).
	p(NT(265), (NT(314)));
//G649: help_arg(265)        => onf_sym(317).
	p(NT(265), (NT(317)));
//G650: help_arg(265)        => qelim_sym(320).
	p(NT(265), (NT(320)));
//G651: __E___E_help_arg_173_174(376) => 's'.
	p(NT(376), (T(15)));
//G652: __E___E_help_arg_173_174(376) => null.
	p(NT(376), (nul));
//G653: __E_help_arg_173(375) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_173_174(376).
	p(NT(375), (T(11)+T(29)+T(16)+T(21)+T(55)+T(10)+T(11)+NT(376)));
//G654: examples_sym(374)    => __E_help_arg_173(375).
	p(NT(374), (NT(375)));
//G655: help_arg(265)        => examples_sym(374).
	p(NT(265), (NT(374)));
//G656: __E_history_175(378) => '%'.
	p(NT(378), (T(39)));
//G657: rel_history_sym(370) => '%' '-'.
	p(NT(370), (T(39)+T(30)));
//G658: history_id(380)      => digits(12).
	p(NT(380), (NT(12)));
//G659: __E___E_history_175_176(379) => history_id(380).
	p(NT(379), (NT(380)));
//G660: __E___E_history_175_176(379) => null.
	p(NT(379), (nul));
//G661: __E_history_175(378) => rel_history_sym(370) __E___E_history_175_176(379).
	p(NT(378), (NT(370)+NT(379)));
//G662: rel_history(377)     => __E_history_175(378).
	p(NT(377), (NT(378)));
//G663: history(353)         => rel_history(377).
	p(NT(353), (NT(377)));
//G664: abs_history_sym(369) => '%'.
	p(NT(369), (T(39)));
//G665: __E_history_177(382) => abs_history_sym(369) history_id(380).
	p(NT(382), (NT(369)+NT(380)));
//G666: abs_history(381)     => __E_history_177(382).
	p(NT(381), (NT(382)));
//G667: history(353)         => abs_history(381).
	p(NT(353), (NT(381)));
//G668: unreachable(383)     => BDD_ID(384).
	p(NT(383), (NT(384)));
//G669: stream_def(385)      => input_def(55).
	p(NT(385), (NT(55)));
//G670: stream_def(385)      => output_def(56).
	p(NT(385), (NT(56)));
//G671: fm_or_term(386)      => wff(36).
	p(NT(386), (NT(36)));
//G672: fm_or_term(386)      => bf(38).
	p(NT(386), (NT(38)));
	#undef T
	#undef NT
	return loaded = true, p;
}

} // namespace tau_parser_data
