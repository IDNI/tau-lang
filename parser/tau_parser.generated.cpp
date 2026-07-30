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
//G72:  __E___E_wff_21_22(89) => 'G'.
	p(NT(89), (T(28)));
//G73:  __E_wff_21(88)       => __E___E_wff_21_22(89) _(9) wff(36).
	p(NT(88), (NT(89)+NT(9)+NT(36)));
//G74:  wff_always(87)       => __E_wff_21(88).
	p(NT(87), (NT(88)));
//G75:  wff(36)              => wff_always(87).
	p(NT(36), (NT(87)));
//G76:  __E_wff_23(91)       => 'F' _(9) wff(36).
	p(NT(91), (T(29)+NT(9)+NT(36)));
//G77:  wff_F(90)            => __E_wff_23(91).
	p(NT(90), (NT(91)));
//G78:  wff(36)              => wff_F(90).
	p(NT(36), (NT(90)));
//G79:  __E_wff_24(93)       => 'A' _(9) wff(36).
	p(NT(93), (T(30)+NT(9)+NT(36)));
//G80:  wff_A(92)            => __E_wff_24(93).
	p(NT(92), (NT(93)));
//G81:  wff(36)              => wff_A(92).
	p(NT(36), (NT(92)));
//G82:  __E_wff_25(95)       => 'E' _(9) wff(36).
	p(NT(95), (T(31)+NT(9)+NT(36)));
//G83:  wff_E(94)            => __E_wff_25(95).
	p(NT(94), (NT(95)));
//G84:  wff(36)              => wff_E(94).
	p(NT(36), (NT(94)));
//G85:  __E_wff_26(97)       => '-' _(9) wff(36).
	p(NT(97), (T(32)+NT(9)+NT(36)));
//G86:  wff_semantic_neg(96) => __E_wff_26(97).
	p(NT(96), (NT(97)));
//G87:  wff(36)              => wff_semantic_neg(96).
	p(NT(36), (NT(96)));
//G88:  __E_wff_27(99)       => wff(36) _(9) '?' _(9) wff(36) _(9) ':' _(9) wff(36).
	p(NT(99), (NT(36)+NT(9)+T(33)+NT(9)+NT(36)+NT(9)+T(2)+NT(9)+NT(36)));
//G89:  wff_conditional(98)  => __E_wff_27(99).
	p(NT(98), (NT(99)));
//G90:  wff(36)              => wff_conditional(98).
	p(NT(36), (NT(98)));
//G91:  __E_wff_28(101)      => 'a' 'l' 'l' __(10) q_vars(102) __(10) wff(36).
	p(NT(101), (T(16)+T(10)+T(10)+NT(10)+NT(102)+NT(10)+NT(36)));
//G92:  wff_all(100)         => __E_wff_28(101).
	p(NT(100), (NT(101)));
//G93:  wff(36)              => wff_all(100).
	p(NT(36), (NT(100)));
//G94:  __E_wff_29(104)      => 'e' 'x' __(10) q_vars(102) __(10) wff(36).
	p(NT(104), (T(11)+T(34)+NT(10)+NT(102)+NT(10)+NT(36)));
//G95:  wff_ex(103)          => __E_wff_29(104).
	p(NT(103), (NT(104)));
//G96:  wff(36)              => wff_ex(103).
	p(NT(36), (NT(103)));
//G97:  wff_ref(105)         => ref(62).
	p(NT(105), (NT(62)));
//G98:  wff(36)              => wff_ref(105).
	p(NT(36), (NT(105)));
//G99:  wff(36)              => constraint(106).
	p(NT(36), (NT(106)));
//G100: __E_wff_30(108)      => wff(36) _(9) '-' '>' _(9) wff(36).
	p(NT(108), (NT(36)+NT(9)+T(32)+T(23)+NT(9)+NT(36)));
//G101: wff_imply(107)       => __E_wff_30(108).
	p(NT(107), (NT(108)));
//G102: wff(36)              => wff_imply(107).
	p(NT(36), (NT(107)));
//G103: __E_wff_31(110)      => wff(36) _(9) '<' '-' _(9) wff(36).
	p(NT(110), (NT(36)+NT(9)+T(22)+T(32)+NT(9)+NT(36)));
//G104: wff_rimply(109)      => __E_wff_31(110).
	p(NT(109), (NT(110)));
//G105: wff(36)              => wff_rimply(109).
	p(NT(36), (NT(109)));
//G106: __E_wff_32(112)      => wff(36) _(9) '<' '-' '>' _(9) wff(36).
	p(NT(112), (NT(36)+NT(9)+T(22)+T(32)+T(23)+NT(9)+NT(36)));
//G107: wff_equiv(111)       => __E_wff_32(112).
	p(NT(111), (NT(112)));
//G108: wff(36)              => wff_equiv(111).
	p(NT(36), (NT(111)));
//G109: __E_wff_33(114)      => wff(36) _(9) '|' '|' _(9) wff(36).
	p(NT(114), (NT(36)+NT(9)+T(35)+T(35)+NT(9)+NT(36)));
//G110: wff_or(113)          => __E_wff_33(114).
	p(NT(113), (NT(114)));
//G111: wff(36)              => wff_or(113).
	p(NT(36), (NT(113)));
//G112: __E_wff_34(116)      => wff(36) _(9) '^' '^' _(9) wff(36).
	p(NT(116), (NT(36)+NT(9)+T(36)+T(36)+NT(9)+NT(36)));
//G113: wff_xor(115)         => __E_wff_34(116).
	p(NT(115), (NT(116)));
//G114: wff(36)              => wff_xor(115).
	p(NT(36), (NT(115)));
//G115: __E_wff_35(118)      => wff(36) _(9) '&' '&' _(9) wff(36).
	p(NT(118), (NT(36)+NT(9)+T(37)+T(37)+NT(9)+NT(36)));
//G116: wff_and(117)         => __E_wff_35(118).
	p(NT(117), (NT(118)));
//G117: wff(36)              => wff_and(117).
	p(NT(36), (NT(117)));
//G118: __E_wff_36(120)      => '!' _(9) wff(36).
	p(NT(120), (T(38)+NT(9)+NT(36)));
//G119: wff_neg(119)         => __E_wff_36(120).
	p(NT(119), (NT(120)));
//G120: wff(36)              => wff_neg(119).
	p(NT(36), (NT(119)));
//G121: wff_t(121)           => 'T'.
	p(NT(121), (T(39)));
//G122: wff(36)              => wff_t(121).
	p(NT(36), (NT(121)));
//G123: wff_f(122)           => 'F'.
	p(NT(122), (T(29)));
//G124: wff(36)              => wff_f(122).
	p(NT(36), (NT(122)));
//G125: __E_wff_37(124)      => bf(38) _(9) '<' '=' _(9) bf(38) _(9) '<' '=' _(9) bf(38).
	p(NT(124), (NT(38)+NT(9)+T(22)+T(3)+NT(9)+NT(38)+NT(9)+T(22)+T(3)+NT(9)+NT(38)));
//G126: bf_interval(123)     => __E_wff_37(124).
	p(NT(123), (NT(124)));
//G127: wff(36)              => bf_interval(123).
	p(NT(36), (NT(123)));
//G128: __E_wff_38(126)      => bf(38) _(9) '=' _(9) bf(38).
	p(NT(126), (NT(38)+NT(9)+T(3)+NT(9)+NT(38)));
//G129: bf_eq(125)           => __E_wff_38(126).
	p(NT(125), (NT(126)));
//G130: wff(36)              => bf_eq(125).
	p(NT(36), (NT(125)));
//G131: __E_wff_39(128)      => bf(38) _(9) '!' '=' _(9) bf(38).
	p(NT(128), (NT(38)+NT(9)+T(38)+T(3)+NT(9)+NT(38)));
//G132: bf_neq(127)          => __E_wff_39(128).
	p(NT(127), (NT(128)));
//G133: wff(36)              => bf_neq(127).
	p(NT(36), (NT(127)));
//G134: __E_wff_40(130)      => bf(38) _(9) '<' _(9) bf(38).
	p(NT(130), (NT(38)+NT(9)+T(22)+NT(9)+NT(38)));
//G135: bf_lt(129)           => __E_wff_40(130).
	p(NT(129), (NT(130)));
//G136: wff(36)              => bf_lt(129).
	p(NT(36), (NT(129)));
//G137: __E_wff_41(132)      => bf(38) _(9) '!' '<' _(9) bf(38).
	p(NT(132), (NT(38)+NT(9)+T(38)+T(22)+NT(9)+NT(38)));
//G138: bf_nlt(131)          => __E_wff_41(132).
	p(NT(131), (NT(132)));
//G139: wff(36)              => bf_nlt(131).
	p(NT(36), (NT(131)));
//G140: __E_wff_42(134)      => bf(38) _(9) '<' '=' _(9) bf(38).
	p(NT(134), (NT(38)+NT(9)+T(22)+T(3)+NT(9)+NT(38)));
//G141: bf_lteq(133)         => __E_wff_42(134).
	p(NT(133), (NT(134)));
//G142: wff(36)              => bf_lteq(133).
	p(NT(36), (NT(133)));
//G143: __E_wff_43(136)      => bf(38) _(9) '!' '<' '=' _(9) bf(38).
	p(NT(136), (NT(38)+NT(9)+T(38)+T(22)+T(3)+NT(9)+NT(38)));
//G144: bf_nlteq(135)        => __E_wff_43(136).
	p(NT(135), (NT(136)));
//G145: wff(36)              => bf_nlteq(135).
	p(NT(36), (NT(135)));
//G146: __E_wff_44(138)      => bf(38) _(9) '>' _(9) bf(38).
	p(NT(138), (NT(38)+NT(9)+T(23)+NT(9)+NT(38)));
//G147: bf_gt(137)           => __E_wff_44(138).
	p(NT(137), (NT(138)));
//G148: wff(36)              => bf_gt(137).
	p(NT(36), (NT(137)));
//G149: __E_wff_45(140)      => bf(38) _(9) '!' '>' _(9) bf(38).
	p(NT(140), (NT(38)+NT(9)+T(38)+T(23)+NT(9)+NT(38)));
//G150: bf_ngt(139)          => __E_wff_45(140).
	p(NT(139), (NT(140)));
//G151: wff(36)              => bf_ngt(139).
	p(NT(36), (NT(139)));
//G152: __E_wff_46(142)      => bf(38) _(9) '>' '=' _(9) bf(38).
	p(NT(142), (NT(38)+NT(9)+T(23)+T(3)+NT(9)+NT(38)));
//G153: bf_gteq(141)         => __E_wff_46(142).
	p(NT(141), (NT(142)));
//G154: wff(36)              => bf_gteq(141).
	p(NT(36), (NT(141)));
//G155: __E_wff_47(144)      => bf(38) _(9) '!' '>' '=' _(9) bf(38).
	p(NT(144), (NT(38)+NT(9)+T(38)+T(23)+T(3)+NT(9)+NT(38)));
//G156: bf_ngteq(143)        => __E_wff_47(144).
	p(NT(143), (NT(144)));
//G157: wff(36)              => bf_ngteq(143).
	p(NT(36), (NT(143)));
//G158: __E_wff_48(146)      => wff(36) _(9) 'U' _(9) wff(36).
	p(NT(146), (NT(36)+NT(9)+T(40)+NT(9)+NT(36)));
//G159: wff_U(145)           => __E_wff_48(146).
	p(NT(145), (NT(146)));
//G160: wff(36)              => wff_U(145).
	p(NT(36), (NT(145)));
//G161: __E_wff_49(148)      => wff(36) _(9) 'R' _(9) wff(36).
	p(NT(148), (NT(36)+NT(9)+T(41)+NT(9)+NT(36)));
//G162: wff_R(147)           => __E_wff_49(148).
	p(NT(147), (NT(148)));
//G163: wff(36)              => wff_R(147).
	p(NT(36), (NT(147)));
//G164: __E_wff_50(150)      => wff(36) _(9) 'W' _(9) wff(36).
	p(NT(150), (NT(36)+NT(9)+T(42)+NT(9)+NT(36)));
//G165: wff_W(149)           => __E_wff_50(150).
	p(NT(149), (NT(150)));
//G166: wff(36)              => wff_W(149).
	p(NT(36), (NT(149)));
//G167: __E_wff_51(152)      => wff(36) _(9) 'S' _(9) wff(36).
	p(NT(152), (NT(36)+NT(9)+T(43)+NT(9)+NT(36)));
//G168: wff_S(151)           => __E_wff_51(152).
	p(NT(151), (NT(152)));
//G169: wff(36)              => wff_S(151).
	p(NT(36), (NT(151)));
//G170: __E_wff_52(154)      => wff(36) _(9) 'T' _(9) wff(36).
	p(NT(154), (NT(36)+NT(9)+T(39)+NT(9)+NT(36)));
//G171: wff_T(153)           => __E_wff_52(154).
	p(NT(153), (NT(154)));
//G172: wff(36)              => wff_T(153).
	p(NT(36), (NT(153)));
//G173: wff(36)              => capture(17).
	p(NT(36), (NT(17)));
//G174: __E_bf_53(155)       => '(' _(9) bf(38) _(9) ')'.
	p(NT(155), (T(12)+NT(9)+NT(38)+NT(9)+T(13)));
//G175: bf_parenthesis(39)   => __E_bf_53(155).
	p(NT(39), (NT(155)));
//G176: bf(38)               => bf_parenthesis(39).
	p(NT(38), (NT(39)));
//G177: bf(38)               => ba_constant(156).
	p(NT(38), (NT(156)));
//G178: bf(38)               => variable(157).
	p(NT(38), (NT(157)));
//G179: __E_bf_54(159)       => 'f' 'a' 'l' 'l' __(10) q_vars(102) __(10) bf(38).
	p(NT(159), (T(9)+T(16)+T(10)+T(10)+NT(10)+NT(102)+NT(10)+NT(38)));
//G180: bf_fall(158)         => __E_bf_54(159).
	p(NT(158), (NT(159)));
//G181: bf(38)               => bf_fall(158).
	p(NT(38), (NT(158)));
//G182: __E_bf_55(161)       => 'f' 'e' 'x' __(10) q_vars(102) __(10) bf(38).
	p(NT(161), (T(9)+T(11)+T(34)+NT(10)+NT(102)+NT(10)+NT(38)));
//G183: bf_fex(160)          => __E_bf_55(161).
	p(NT(160), (NT(161)));
//G184: bf(38)               => bf_fex(160).
	p(NT(38), (NT(160)));
//G185: bf_ref(162)          => ref(62).
	p(NT(162), (NT(62)));
//G186: bf(38)               => bf_ref(162).
	p(NT(38), (NT(162)));
//G187: __E___E_bf_56_57(165) => '[' _(9) subtype(14) _(9) ']'.
	p(NT(165), (T(26)+NT(9)+NT(14)+NT(9)+T(27)));
//G188: __E___E_bf_56_57(165) => null.
	p(NT(165), (nul));
//G189: __E___E_bf_56_58(166) => bf_parenthesis(39).
	p(NT(166), (NT(39)));
//G190: __E___E_bf_56_58(166) => ba_constant(156).
	p(NT(166), (NT(156)));
//G191: __E___E_bf_56_58(166) => variable(157).
	p(NT(166), (NT(157)));
//G192: __E___E_bf_56_58(166) => bf_ref(162).
	p(NT(166), (NT(162)));
//G193: __E___E_bf_56_58(166) => bf_t(167).
	p(NT(166), (NT(167)));
//G194: __E___E_bf_56_58(166) => bf_f(168).
	p(NT(166), (NT(168)));
//G195: __E___E_bf_56_58(166) => bf_neg(169).
	p(NT(166), (NT(169)));
//G196: __E___E_bf_56_58(166) => bf_cast(163).
	p(NT(166), (NT(163)));
//G197: __E___E_bf_56_58(166) => capture(17).
	p(NT(166), (NT(17)));
//G198: bf_cast_oprnd(41)    => __E___E_bf_56_58(166).
	p(NT(41), (NT(166)));
//G199: __E_bf_56(164)       => '(' _(9) type(13) _(9) __E___E_bf_56_57(165) _(9) ')' _(9) bf_cast_oprnd(41).
	p(NT(164), (T(12)+NT(9)+NT(13)+NT(9)+NT(165)+NT(9)+T(13)+NT(9)+NT(41)));
//G200: bf_cast(163)         => __E_bf_56(164).
	p(NT(163), (NT(164)));
//G201: bf(38)               => bf_cast(163).
	p(NT(38), (NT(163)));
//G202: __E_bf_59(171)       => func_sym(172) '(' _(9) func_args(173) _(9) ')'.
	p(NT(171), (NT(172)+T(12)+NT(9)+NT(173)+NT(9)+T(13)));
//G203: bf_func_app(170)     => __E_bf_59(171).
	p(NT(170), (NT(171)));
//G204: bf(38)               => bf_func_app(170).
	p(NT(38), (NT(170)));
//G205: __N_0(446)           => bf_shr(174).
	p(NT(446), (NT(174)));
//G206: __E___E_bf_60_61(176) => bf(38) & ~( __N_0(446) ).	 # conjunctive
	p(NT(176), (NT(38)) & ~(NT(446)));
//G207: __E_bf_60(175)       => bf(38) _(9) '>' '>' _(9) __E___E_bf_60_61(176).
	p(NT(175), (NT(38)+NT(9)+T(23)+T(23)+NT(9)+NT(176)));
//G208: bf_shr(174)          => __E_bf_60(175).
	p(NT(174), (NT(175)));
//G209: bf(38)               => bf_shr(174).
	p(NT(38), (NT(174)));
//G210: __N_1(447)           => bf_shl(177).
	p(NT(447), (NT(177)));
//G211: __E___E_bf_62_63(179) => bf(38) & ~( __N_1(447) ).	 # conjunctive
	p(NT(179), (NT(38)) & ~(NT(447)));
//G212: __E_bf_62(178)       => bf(38) _(9) '<' '<' _(9) __E___E_bf_62_63(179).
	p(NT(178), (NT(38)+NT(9)+T(22)+T(22)+NT(9)+NT(179)));
//G213: bf_shl(177)          => __E_bf_62(178).
	p(NT(177), (NT(178)));
//G214: bf(38)               => bf_shl(177).
	p(NT(38), (NT(177)));
//G215: __E_bf_64(181)       => bf(38) _(9) '+' _(9) bf(38).
	p(NT(181), (NT(38)+NT(9)+T(44)+NT(9)+NT(38)));
//G216: bf_add(180)          => __E_bf_64(181).
	p(NT(180), (NT(181)));
//G217: bf(38)               => bf_add(180).
	p(NT(38), (NT(180)));
//G218: __N_2(448)           => bf_sub(182).
	p(NT(448), (NT(182)));
//G219: __E___E_bf_65_66(184) => bf(38) & ~( __N_2(448) ).	 # conjunctive
	p(NT(184), (NT(38)) & ~(NT(448)));
//G220: __E_bf_65(183)       => bf(38) _(9) '-' _(9) __E___E_bf_65_66(184).
	p(NT(183), (NT(38)+NT(9)+T(32)+NT(9)+NT(184)));
//G221: bf_sub(182)          => __E_bf_65(183).
	p(NT(182), (NT(183)));
//G222: bf(38)               => bf_sub(182).
	p(NT(38), (NT(182)));
//G223: __N_3(449)           => bf_mod(185).
	p(NT(449), (NT(185)));
//G224: __E___E_bf_67_68(187) => bf(38) & ~( __N_3(449) ).	 # conjunctive
	p(NT(187), (NT(38)) & ~(NT(449)));
//G225: __E_bf_67(186)       => bf(38) _(9) '%' _(9) __E___E_bf_67_68(187).
	p(NT(186), (NT(38)+NT(9)+T(45)+NT(9)+NT(187)));
//G226: bf_mod(185)          => __E_bf_67(186).
	p(NT(185), (NT(186)));
//G227: bf(38)               => bf_mod(185).
	p(NT(38), (NT(185)));
//G228: __E_bf_69(189)       => bf(38) _(9) '*' _(9) bf(38).
	p(NT(189), (NT(38)+NT(9)+T(46)+NT(9)+NT(38)));
//G229: bf_mul(188)          => __E_bf_69(189).
	p(NT(188), (NT(189)));
//G230: bf(38)               => bf_mul(188).
	p(NT(38), (NT(188)));
//G231: __N_4(450)           => bf_div(190).
	p(NT(450), (NT(190)));
//G232: __E___E_bf_70_71(192) => bf(38) & ~( __N_4(450) ).	 # conjunctive
	p(NT(192), (NT(38)) & ~(NT(450)));
//G233: __E_bf_70(191)       => bf(38) _(9) '/' _(9) __E___E_bf_70_71(192).
	p(NT(191), (NT(38)+NT(9)+T(47)+NT(9)+NT(192)));
//G234: bf_div(190)          => __E_bf_70(191).
	p(NT(190), (NT(191)));
//G235: bf(38)               => bf_div(190).
	p(NT(38), (NT(190)));
//G236: __N_5(451)           => bf_nor(193).
	p(NT(451), (NT(193)));
//G237: __E___E_bf_72_73(195) => bf(38) & ~( __N_5(451) ).	 # conjunctive
	p(NT(195), (NT(38)) & ~(NT(451)));
//G238: __E_bf_72(194)       => bf(38) _(9) '!' '|' _(9) __E___E_bf_72_73(195).
	p(NT(194), (NT(38)+NT(9)+T(38)+T(35)+NT(9)+NT(195)));
//G239: bf_nor(193)          => __E_bf_72(194).
	p(NT(193), (NT(194)));
//G240: bf(38)               => bf_nor(193).
	p(NT(38), (NT(193)));
//G241: __N_6(452)           => bf_xnor(196).
	p(NT(452), (NT(196)));
//G242: __E___E_bf_74_75(198) => bf(38) & ~( __N_6(452) ).	 # conjunctive
	p(NT(198), (NT(38)) & ~(NT(452)));
//G243: __E_bf_74(197)       => bf(38) _(9) '!' '^' _(9) __E___E_bf_74_75(198).
	p(NT(197), (NT(38)+NT(9)+T(38)+T(36)+NT(9)+NT(198)));
//G244: bf_xnor(196)         => __E_bf_74(197).
	p(NT(196), (NT(197)));
//G245: bf(38)               => bf_xnor(196).
	p(NT(38), (NT(196)));
//G246: __N_7(453)           => bf_nand(199).
	p(NT(453), (NT(199)));
//G247: __E___E_bf_76_77(201) => bf(38) & ~( __N_7(453) ).	 # conjunctive
	p(NT(201), (NT(38)) & ~(NT(453)));
//G248: __E_bf_76(200)       => bf(38) _(9) '!' '&' _(9) __E___E_bf_76_77(201).
	p(NT(200), (NT(38)+NT(9)+T(38)+T(37)+NT(9)+NT(201)));
//G249: bf_nand(199)         => __E_bf_76(200).
	p(NT(199), (NT(200)));
//G250: bf(38)               => bf_nand(199).
	p(NT(38), (NT(199)));
//G251: __E_bf_78(203)       => bf(38) _(9) '|' _(9) bf(38).
	p(NT(203), (NT(38)+NT(9)+T(35)+NT(9)+NT(38)));
//G252: bf_or(202)           => __E_bf_78(203).
	p(NT(202), (NT(203)));
//G253: bf(38)               => bf_or(202).
	p(NT(38), (NT(202)));
//G254: __E_bf_79(205)       => bf(38) _(9) '^' _(9) bf(38).
	p(NT(205), (NT(38)+NT(9)+T(36)+NT(9)+NT(38)));
//G255: bf_xor(204)          => __E_bf_79(205).
	p(NT(204), (NT(205)));
//G256: bf(38)               => bf_xor(204).
	p(NT(38), (NT(204)));
//G257: __E___E_bf_80_81(207) => typed(65).
	p(NT(207), (NT(65)));
//G258: __E___E_bf_80_81(207) => null.
	p(NT(207), (nul));
//G259: __E_bf_80(206)       => '1' __E___E_bf_80_81(207).
	p(NT(206), (T(48)+NT(207)));
//G260: bf_t(167)            => __E_bf_80(206).
	p(NT(167), (NT(206)));
//G261: bf(38)               => bf_t(167).
	p(NT(38), (NT(167)));
//G262: __E___E_bf_82_83(209) => typed(65).
	p(NT(209), (NT(65)));
//G263: __E___E_bf_82_83(209) => null.
	p(NT(209), (nul));
//G264: __E_bf_82(208)       => '0' __E___E_bf_82_83(209).
	p(NT(208), (T(49)+NT(209)));
//G265: bf_f(168)            => __E_bf_82(208).
	p(NT(168), (NT(208)));
//G266: bf(38)               => bf_f(168).
	p(NT(38), (NT(168)));
//G267: __E___E_bf_84_85(211) => _(9) '&' _(9).
	p(NT(211), (NT(9)+T(37)+NT(9)));
//G268: __E___E_bf_84_85(211) => __(10).
	p(NT(211), (NT(10)));
//G269: __E_bf_84(210)       => bf(38) __E___E_bf_84_85(211) bf(38).
	p(NT(210), (NT(38)+NT(211)+NT(38)));
//G270: bf_and(45)           => __E_bf_84(210).
	p(NT(45), (NT(210)));
//G271: bf(38)               => bf_and(45).
	p(NT(38), (NT(45)));
//G272: __E___E_bf_86_87(213) => bf_parenthesis(39).
	p(NT(213), (NT(39)));
//G273: __E___E_bf_86_87(213) => ba_constant(156).
	p(NT(213), (NT(156)));
//G274: __E___E_bf_86_87(213) => variable(157).
	p(NT(213), (NT(157)));
//G275: __E___E_bf_86_87(213) => bf_ref(162).
	p(NT(213), (NT(162)));
//G276: __E___E_bf_86_87(213) => bf_t(167).
	p(NT(213), (NT(167)));
//G277: __E___E_bf_86_87(213) => bf_f(168).
	p(NT(213), (NT(168)));
//G278: __E___E_bf_86_87(213) => bf_neg(169).
	p(NT(213), (NT(169)));
//G279: __E___E_bf_86_87(213) => capture(17).
	p(NT(213), (NT(17)));
//G280: bf_neg_oprnd(40)     => __E___E_bf_86_87(213).
	p(NT(40), (NT(213)));
//G281: __E_bf_86(212)       => bf_neg_oprnd(40) _(9) '\''.
	p(NT(212), (NT(40)+NT(9)+T(50)));
//G282: bf_neg(169)          => __E_bf_86(212).
	p(NT(169), (NT(212)));
//G283: bf(38)               => bf_neg(169).
	p(NT(38), (NT(169)));
//G284: __E___E_bf_88_89(215) => bf_parenthesis(39).
	p(NT(215), (NT(39)));
//G285: __E___E_bf_88_89(215) => variable(157).
	p(NT(215), (NT(157)));
//G286: __E___E_bf_88_89(215) => bf_ref(162).
	p(NT(215), (NT(162)));
//G287: __E___E_bf_88_89(215) => bf_neg(169).
	p(NT(215), (NT(169)));
//G288: __E___E_bf_88_89(215) => bf_and_nosep(44).
	p(NT(215), (NT(44)));
//G289: __E___E_bf_88_89(215) => capture(17).
	p(NT(215), (NT(17)));
//G290: bf_and_nosep_1st_oprnd(42) => __E___E_bf_88_89(215).
	p(NT(42), (NT(215)));
//G291: __E___E_bf_88_90(216) => bf_parenthesis(39).
	p(NT(216), (NT(39)));
//G292: __E___E_bf_88_90(216) => ba_constant(156).
	p(NT(216), (NT(156)));
//G293: __E___E_bf_88_90(216) => variable(157).
	p(NT(216), (NT(157)));
//G294: __E___E_bf_88_90(216) => bf_ref(162).
	p(NT(216), (NT(162)));
//G295: __E___E_bf_88_90(216) => bf_neg(169).
	p(NT(216), (NT(169)));
//G296: __E___E_bf_88_90(216) => capture(17).
	p(NT(216), (NT(17)));
//G297: bf_and_nosep_2nd_oprnd(43) => __E___E_bf_88_90(216).
	p(NT(43), (NT(216)));
//G298: __E_bf_88(214)       => bf_and_nosep_1st_oprnd(42) bf_and_nosep_2nd_oprnd(43).
	p(NT(214), (NT(42)+NT(43)));
//G299: bf_and_nosep(44)     => __E_bf_88(214).
	p(NT(44), (NT(214)));
//G300: bf(38)               => bf_and_nosep(44).
	p(NT(38), (NT(44)));
//G301: bf(38)               => capture(17).
	p(NT(38), (NT(17)));
//G302: func_sym(172)        => chars(25).
	p(NT(172), (NT(25)));
//G303: __E_func_args_91(217) => _(9) ',' _(9) bf(38).
	p(NT(217), (NT(9)+T(20)+NT(9)+NT(38)));
//G304: __E_func_args_92(218) => null.
	p(NT(218), (nul));
//G305: __E_func_args_92(218) => __E_func_args_91(217) __E_func_args_92(218).
	p(NT(218), (NT(217)+NT(218)));
//G306: func_args(173)       => bf(38) __E_func_args_92(218).
	p(NT(173), (NT(38)+NT(218)));
//G307: func_dynamic(220)    => 'd' 'y' 'n' 'a' 'm' 'i' 'c'.
	p(NT(220), (T(51)+T(25)+T(5)+T(16)+T(21)+T(4)+T(14)));
//G308: func_mode(219)       => func_dynamic(220).
	p(NT(219), (NT(220)));
//G309: func_static(221)     => 's' 't' 'a' 't' 'i' 'c'.
	p(NT(221), (T(15)+T(8)+T(16)+T(8)+T(4)+T(14)));
//G310: func_mode(219)       => func_static(221).
	p(NT(219), (NT(221)));
//G311: __E_func_sort_list_93(223) => _(9) ',' _(9) type(13).
	p(NT(223), (NT(9)+T(20)+NT(9)+NT(13)));
//G312: __E_func_sort_list_94(224) => null.
	p(NT(224), (nul));
//G313: __E_func_sort_list_94(224) => __E_func_sort_list_93(223) __E_func_sort_list_94(224).
	p(NT(224), (NT(223)+NT(224)));
//G314: func_sort_list(222)  => type(13) __E_func_sort_list_94(224).
	p(NT(222), (NT(13)+NT(224)));
//G315: func_decl(225)       => func_mode(219) __(10) func_sym(172) _(9) ':' _(9) func_sort_list(222) _(9) '-' '>' _(9) type(13).
	p(NT(225), (NT(219)+NT(10)+NT(172)+NT(9)+T(2)+NT(9)+NT(222)+NT(9)+T(32)+T(23)+NT(9)+NT(13)));
//G316: __E_constraint_95(227) => '[' ctnvar(22) _(9) '!' '=' _(9) num(228) ']'.
	p(NT(227), (T(26)+NT(22)+NT(9)+T(38)+T(3)+NT(9)+NT(228)+T(27)));
//G317: __E_constraint_95(227) => '[' num(228) _(9) '!' '=' _(9) ctnvar(22) ']'.
	p(NT(227), (T(26)+NT(228)+NT(9)+T(38)+T(3)+NT(9)+NT(22)+T(27)));
//G318: ctn_neq(226)         => __E_constraint_95(227).
	p(NT(226), (NT(227)));
//G319: constraint(106)      => ctn_neq(226).
	p(NT(106), (NT(226)));
//G320: __E_constraint_96(230) => '[' ctnvar(22) _(9) '=' _(9) num(228) ']'.
	p(NT(230), (T(26)+NT(22)+NT(9)+T(3)+NT(9)+NT(228)+T(27)));
//G321: __E_constraint_96(230) => '[' num(228) _(9) '=' _(9) ctnvar(22) ']'.
	p(NT(230), (T(26)+NT(228)+NT(9)+T(3)+NT(9)+NT(22)+T(27)));
//G322: ctn_eq(229)          => __E_constraint_96(230).
	p(NT(229), (NT(230)));
//G323: constraint(106)      => ctn_eq(229).
	p(NT(106), (NT(229)));
//G324: __E_constraint_97(232) => '[' ctnvar(22) _(9) '>' '=' _(9) num(228) ']'.
	p(NT(232), (T(26)+NT(22)+NT(9)+T(23)+T(3)+NT(9)+NT(228)+T(27)));
//G325: __E_constraint_97(232) => '[' num(228) _(9) '>' '=' _(9) ctnvar(22) ']'.
	p(NT(232), (T(26)+NT(228)+NT(9)+T(23)+T(3)+NT(9)+NT(22)+T(27)));
//G326: ctn_gteq(231)        => __E_constraint_97(232).
	p(NT(231), (NT(232)));
//G327: constraint(106)      => ctn_gteq(231).
	p(NT(106), (NT(231)));
//G328: __E_constraint_98(234) => '[' ctnvar(22) _(9) '>' _(9) num(228) ']'.
	p(NT(234), (T(26)+NT(22)+NT(9)+T(23)+NT(9)+NT(228)+T(27)));
//G329: __E_constraint_98(234) => '[' num(228) _(9) '>' _(9) ctnvar(22) ']'.
	p(NT(234), (T(26)+NT(228)+NT(9)+T(23)+NT(9)+NT(22)+T(27)));
//G330: ctn_gt(233)          => __E_constraint_98(234).
	p(NT(233), (NT(234)));
//G331: constraint(106)      => ctn_gt(233).
	p(NT(106), (NT(233)));
//G332: __E_constraint_99(236) => '[' ctnvar(22) _(9) '<' '=' _(9) num(228) ']'.
	p(NT(236), (T(26)+NT(22)+NT(9)+T(22)+T(3)+NT(9)+NT(228)+T(27)));
//G333: __E_constraint_99(236) => '[' num(228) _(9) '<' '=' _(9) ctnvar(22) ']'.
	p(NT(236), (T(26)+NT(228)+NT(9)+T(22)+T(3)+NT(9)+NT(22)+T(27)));
//G334: ctn_lteq(235)        => __E_constraint_99(236).
	p(NT(235), (NT(236)));
//G335: constraint(106)      => ctn_lteq(235).
	p(NT(106), (NT(235)));
//G336: __E_constraint_100(238) => '[' ctnvar(22) _(9) '<' _(9) num(228) ']'.
	p(NT(238), (T(26)+NT(22)+NT(9)+T(22)+NT(9)+NT(228)+T(27)));
//G337: __E_constraint_100(238) => '[' num(228) _(9) '<' _(9) ctnvar(22) ']'.
	p(NT(238), (T(26)+NT(228)+NT(9)+T(22)+NT(9)+NT(22)+T(27)));
//G338: ctn_lt(237)          => __E_constraint_100(238).
	p(NT(237), (NT(238)));
//G339: constraint(106)      => ctn_lt(237).
	p(NT(106), (NT(237)));
//G340: __E_ba_constant_101(239) => capture(17).
	p(NT(239), (NT(17)));
//G341: __E_ba_constant_101(239) => source(16).
	p(NT(239), (NT(16)));
//G342: __E_ba_constant_102(240) => typed(65).
	p(NT(240), (NT(65)));
//G343: __E_ba_constant_102(240) => null.
	p(NT(240), (nul));
//G344: ba_constant(156)     => '{' _(9) __E_ba_constant_101(239) _(9) '}' __E_ba_constant_102(240).
	p(NT(156), (T(52)+NT(9)+NT(239)+NT(9)+T(53)+NT(240)));
//G345: __E___E_source_103_104(242) => src_c(27).
	p(NT(242), (NT(27)));
//G346: __E___E_source_103_104(242) => space(2).
	p(NT(242), (NT(2)));
//G347: __E___E_source_103_105(243) => null.
	p(NT(243), (nul));
//G348: __E___E_source_103_105(243) => __E___E_source_103_104(242) __E___E_source_103_105(243).
	p(NT(243), (NT(242)+NT(243)));
//G349: __E_source_103(241)  => __E___E_source_103_105(243) src_c(27).
	p(NT(241), (NT(243)+NT(27)));
//G350: __E_source_103(241)  => null.
	p(NT(241), (nul));
//G351: source(16)           => src_c(27) __E_source_103(241).
	p(NT(16), (NT(27)+NT(241)));
//G352: src_c(27)            => alnum(6).
	p(NT(27), (NT(6)));
//G353: __N_8(454)           => '{'.
	p(NT(454), (T(52)));
//G354: __N_9(455)           => '}'.
	p(NT(455), (T(53)));
//G355: src_c(27)            => ~( __N_8(454) ) & ~( __N_9(455) ) & punct(7).	 # conjunctive
	p(NT(27), ~(NT(454)) & ~(NT(455)) & (NT(7)));
//G356: __E_src_c_106(244)   => src_c(27).
	p(NT(244), (NT(27)));
//G357: __E_src_c_106(244)   => space(2).
	p(NT(244), (NT(2)));
//G358: __E_src_c_107(245)   => null.
	p(NT(245), (nul));
//G359: __E_src_c_107(245)   => __E_src_c_106(244) __E_src_c_107(245).
	p(NT(245), (NT(244)+NT(245)));
//G360: src_c(27)            => '{' __E_src_c_107(245) '}'.
	p(NT(27), (T(52)+NT(245)+T(53)));
//G361: __E_variable_108(246) => uconst(28).
	p(NT(246), (NT(28)));
//G362: __E_variable_108(246) => io_var(247).
	p(NT(246), (NT(247)));
//G363: __E_variable_108(246) => var_name(18).
	p(NT(246), (NT(18)));
//G364: __E_variable_109(248) => typed(65).
	p(NT(248), (NT(65)));
//G365: __E_variable_109(248) => null.
	p(NT(248), (nul));
//G366: variable(157)        => __E_variable_108(246) __E_variable_109(248).
	p(NT(157), (NT(246)+NT(248)));
//G367: __N_10(456)          => 'F'.
	p(NT(456), (T(29)));
//G368: __N_11(457)          => 'T'.
	p(NT(457), (T(39)));
//G369: __E_var_name_110(249) => ~( __N_10(456) ) & ~( __N_11(457) ) & alpha(5).	 # conjunctive
	p(NT(249), ~(NT(456)) & ~(NT(457)) & (NT(5)));
//G370: __E_var_name_111(250) => null.
	p(NT(250), (nul));
//G371: __E_var_name_111(250) => digit(3) __E_var_name_111(250).
	p(NT(250), (NT(3)+NT(250)));
//G372: var_name(18)         => __E_var_name_110(249) __E_var_name_111(250).	 # guarded: charvar
	p(NT(18), (NT(249)+NT(250)));
	p.back().guard = "charvar";
//G373: __N_12(458)          => 'F'.
	p(NT(458), (T(29)));
//G374: __N_13(459)          => 'T'.
	p(NT(459), (T(39)));
//G375: __E_var_name_112(251) => ~( __N_12(458) ) & ~( __N_13(459) ) & alpha(5).	 # conjunctive
	p(NT(251), ~(NT(458)) & ~(NT(459)) & (NT(5)));
//G376: __E_var_name_113(252) => alnum(6).
	p(NT(252), (NT(6)));
//G377: __E_var_name_113(252) => '_'.
	p(NT(252), (T(54)));
//G378: __E_var_name_114(253) => null.
	p(NT(253), (nul));
//G379: __E_var_name_114(253) => __E_var_name_113(252) __E_var_name_114(253).
	p(NT(253), (NT(252)+NT(253)));
//G380: var_name(18)         => __E_var_name_112(251) __E_var_name_114(253).	 # guarded: var
	p(NT(18), (NT(251)+NT(253)));
	p.back().guard = "var";
//G381: io_var(247)          => io_var_name(19) '[' offset(254) ']'.
	p(NT(247), (NT(19)+T(26)+NT(254)+T(27)));
//G382: io_var_name(19)      => chars(25).
	p(NT(19), (NT(25)));
//G383: __E___E_uconst_115_116(256) => chars(25) _(9).
	p(NT(256), (NT(25)+NT(9)));
//G384: __E___E_uconst_115_116(256) => null.
	p(NT(256), (nul));
//G385: __E_uconst_115(255)  => __E___E_uconst_115_116(256) ':' _(9) chars(25).
	p(NT(255), (NT(256)+T(2)+NT(9)+NT(25)));
//G386: uconst_name(20)      => __E_uconst_115(255).
	p(NT(20), (NT(255)));
//G387: uconst(28)           => '<' _(9) uconst_name(20) _(9) '>'.
	p(NT(28), (T(22)+NT(9)+NT(20)+NT(9)+T(23)));
//G388: __E_q_vars_117(257)  => _(9) ',' _(9) q_var(26).
	p(NT(257), (NT(9)+T(20)+NT(9)+NT(26)));
//G389: __E_q_vars_118(258)  => null.
	p(NT(258), (nul));
//G390: __E_q_vars_118(258)  => __E_q_vars_117(257) __E_q_vars_118(258).
	p(NT(258), (NT(257)+NT(258)));
//G391: q_vars(102)          => q_var(26) __E_q_vars_118(258).
	p(NT(102), (NT(26)+NT(258)));
//G392: q_var(26)            => capture(17).
	p(NT(26), (NT(17)));
//G393: __N_14(460)          => uconst(28).
	p(NT(460), (NT(28)));
//G394: q_var(26)            => ~( __N_14(460) ) & variable(157).	 # conjunctive
	p(NT(26), ~(NT(460)) & (NT(157)));
//G395: ctnvar(22)           => chars(25).
	p(NT(22), (NT(25)));
//G396: __E_offsets_119(259) => _(9) ',' _(9) offset(254).
	p(NT(259), (NT(9)+T(20)+NT(9)+NT(254)));
//G397: __E_offsets_120(260) => null.
	p(NT(260), (nul));
//G398: __E_offsets_120(260) => __E_offsets_119(259) __E_offsets_120(260).
	p(NT(260), (NT(259)+NT(260)));
//G399: offsets(71)          => '[' _(9) offset(254) __E_offsets_120(260) _(9) ']'.
	p(NT(71), (T(26)+NT(9)+NT(254)+NT(260)+NT(9)+T(27)));
//G400: offset(254)          => integer(15).
	p(NT(254), (NT(15)));
//G401: offset(254)          => capture(17).
	p(NT(254), (NT(17)));
//G402: offset(254)          => shift(261).
	p(NT(254), (NT(261)));
//G403: __N_15(461)          => io_var(247).
	p(NT(461), (NT(247)));
//G404: __E_offset_121(262)  => variable(157) & ~( __N_15(461) ).	 # conjunctive
	p(NT(262), (NT(157)) & ~(NT(461)));
//G405: offset(254)          => __E_offset_121(262).
	p(NT(254), (NT(262)));
//G406: __E_shift_122(263)   => capture(17).
	p(NT(263), (NT(17)));
//G407: __N_16(462)          => io_var(247).
	p(NT(462), (NT(247)));
//G408: __E___E_shift_122_123(264) => variable(157) & ~( __N_16(462) ).	 # conjunctive
	p(NT(264), (NT(157)) & ~(NT(462)));
//G409: __E_shift_122(263)   => __E___E_shift_122_123(264).
	p(NT(263), (NT(264)));
//G410: shift(261)           => __E_shift_122(263) _(9) '-' _(9) num(228).
	p(NT(261), (NT(263)+NT(9)+T(32)+NT(9)+NT(228)));
//G411: __E_chars_124(265)   => alnum(6).
	p(NT(265), (NT(6)));
//G412: __E_chars_124(265)   => '_'.
	p(NT(265), (T(54)));
//G413: __E_chars_125(266)   => null.
	p(NT(266), (nul));
//G414: __E_chars_125(266)   => __E_chars_124(265) __E_chars_125(266).
	p(NT(266), (NT(265)+NT(266)));
//G415: chars(25)            => alpha(5) __E_chars_125(266).
	p(NT(25), (NT(5)+NT(266)));
//G416: __E_digits_126(267)  => digit(3).
	p(NT(267), (NT(3)));
//G417: __E_digits_126(267)  => digit(3) __E_digits_126(267).
	p(NT(267), (NT(3)+NT(267)));
//G418: digits(12)           => __E_digits_126(267).
	p(NT(12), (NT(267)));
//G419: num(228)             => digits(12).
	p(NT(228), (NT(12)));
//G420: __E_integer_127(268) => '-'.
	p(NT(268), (T(32)));
//G421: __E_integer_127(268) => null.
	p(NT(268), (nul));
//G422: integer(15)          => __E_integer_127(268) _(9) digits(12).
	p(NT(15), (NT(268)+NT(9)+NT(12)));
//G423: sym(11)              => chars(25).
	p(NT(11), (NT(25)));
//G424: capture(17)          => '$' chars(25).
	p(NT(17), (T(55)+NT(25)));
//G425: __E_typed_128(269)   => '[' _(9) subtype(14) _(9) ']'.
	p(NT(269), (T(26)+NT(9)+NT(14)+NT(9)+T(27)));
//G426: __E_typed_128(269)   => null.
	p(NT(269), (nul));
//G427: typed(65)            => _(9) ':' _(9) type(13) _(9) __E_typed_128(269).
	p(NT(65), (NT(9)+T(2)+NT(9)+NT(13)+NT(9)+NT(269)));
//G428: type(13)             => chars(25).
	p(NT(13), (NT(25)));
//G429: subtype(14)          => num(228).
	p(NT(14), (NT(228)));
//G430: __E_comment_129(271) => printable(8).
	p(NT(271), (NT(8)));
//G431: __E_comment_129(271) => '\t'.
	p(NT(271), (T(56)));
//G432: __E_comment_130(272) => null.
	p(NT(272), (nul));
//G433: __E_comment_130(272) => __E_comment_129(271) __E_comment_130(272).
	p(NT(272), (NT(271)+NT(272)));
//G434: __E_comment_131(273) => '\n'.
	p(NT(273), (T(57)));
//G435: __E_comment_131(273) => '\r'.
	p(NT(273), (T(58)));
//G436: __E_comment_131(273) => eof(1).
	p(NT(273), (NT(1)));
//G437: comment(270)         => '#' __E_comment_130(272) __E_comment_131(273).
	p(NT(270), (T(59)+NT(272)+NT(273)));
//G438: __(10)               => space(2).
	p(NT(10), (NT(2)));
//G439: __(10)               => comment(270).
	p(NT(10), (NT(270)));
//G440: __(10)               => __(10) space(2).
	p(NT(10), (NT(10)+NT(2)));
//G441: __(10)               => __(10) comment(270).
	p(NT(10), (NT(10)+NT(270)));
//G442: _(9)                 => __(10).
	p(NT(9), (NT(10)));
//G443: _(9)                 => null.
	p(NT(9), (nul));
//G444: cli(274)             => _(9).
	p(NT(274), (NT(9)));
//G445: __E_cli_132(276)     => '.' _(9) cli_command(275) _(9).
	p(NT(276), (T(1)+NT(9)+NT(275)+NT(9)));
//G446: __E_cli_133(277)     => null.
	p(NT(277), (nul));
//G447: __E_cli_133(277)     => __E_cli_132(276) __E_cli_133(277).
	p(NT(277), (NT(276)+NT(277)));
//G448: __E_cli_134(278)     => '.' _(9).
	p(NT(278), (T(1)+NT(9)));
//G449: __E_cli_134(278)     => null.
	p(NT(278), (nul));
//G450: cli(274)             => _(9) cli_command(275) _(9) __E_cli_133(277) __E_cli_134(278).
	p(NT(274), (NT(9)+NT(275)+NT(9)+NT(277)+NT(278)));
//G451: __E_cli_command_135(281) => 'q'.
	p(NT(281), (T(60)));
//G452: __E_cli_command_135(281) => 'q' 'u' 'i' 't'.
	p(NT(281), (T(60)+T(7)+T(4)+T(8)));
//G453: quit_sym(280)        => __E_cli_command_135(281).
	p(NT(280), (NT(281)));
//G454: quit_cmd(279)        => quit_sym(280).
	p(NT(279), (NT(280)));
//G455: cli_command(275)     => quit_cmd(279).
	p(NT(275), (NT(279)));
//G456: __E_cli_command_136(284) => 'v'.
	p(NT(284), (T(61)));
//G457: __E_cli_command_136(284) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(284), (T(61)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G458: version_sym(283)     => __E_cli_command_136(284).
	p(NT(283), (NT(284)));
//G459: version_cmd(282)     => version_sym(283).
	p(NT(282), (NT(283)));
//G460: cli_command(275)     => version_cmd(282).
	p(NT(275), (NT(282)));
//G461: __E_cli_command_137(287) => 'c'.
	p(NT(287), (T(14)));
//G462: __E_cli_command_137(287) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(287), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G463: clear_sym(286)       => __E_cli_command_137(287).
	p(NT(286), (NT(287)));
//G464: clear_cmd(285)       => clear_sym(286).
	p(NT(285), (NT(286)));
//G465: cli_command(275)     => clear_cmd(285).
	p(NT(275), (NT(285)));
//G466: __E___E_cli_command_138_139(291) => 'h'.
	p(NT(291), (T(62)));
//G467: __E___E_cli_command_138_139(291) => 'h' 'e' 'l' 'p'.
	p(NT(291), (T(62)+T(11)+T(10)+T(63)));
//G468: help_sym(290)        => __E___E_cli_command_138_139(291).
	p(NT(290), (NT(291)));
//G469: __E___E_cli_command_138_140(292) => __(10) help_arg(293).
	p(NT(292), (NT(10)+NT(293)));
//G470: __E___E_cli_command_138_140(292) => null.
	p(NT(292), (nul));
//G471: __E_cli_command_138(289) => help_sym(290) __E___E_cli_command_138_140(292).
	p(NT(289), (NT(290)+NT(292)));
//G472: help_cmd(288)        => __E_cli_command_138(289).
	p(NT(288), (NT(289)));
//G473: cli_command(275)     => help_cmd(288).
	p(NT(275), (NT(288)));
//G474: file_sym(296)        => 'f' 'i' 'l' 'e'.
	p(NT(296), (T(9)+T(4)+T(10)+T(11)));
//G475: __E_cli_command_141(295) => file_sym(296) __(10) q_string(297).
	p(NT(295), (NT(296)+NT(10)+NT(297)));
//G476: file_cmd(294)        => __E_cli_command_141(295).
	p(NT(294), (NT(295)));
//G477: cli_command(275)     => file_cmd(294).
	p(NT(275), (NT(294)));
//G478: valid_sym(300)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(300), (T(61)+T(16)+T(10)+T(4)+T(51)));
//G479: __E_cli_command_142(299) => valid_sym(300) __(10) spec_arg(35).
	p(NT(299), (NT(300)+NT(10)+NT(35)));
//G480: valid_cmd(298)       => __E_cli_command_142(299).
	p(NT(298), (NT(299)));
//G481: cli_command(275)     => valid_cmd(298).
	p(NT(275), (NT(298)));
//G482: sat_sym(303)         => 's' 'a' 't'.
	p(NT(303), (T(15)+T(16)+T(8)));
//G483: __E_cli_command_143(302) => sat_sym(303) __(10) spec_arg(35).
	p(NT(302), (NT(303)+NT(10)+NT(35)));
//G484: sat_cmd(301)         => __E_cli_command_143(302).
	p(NT(301), (NT(302)));
//G485: cli_command(275)     => sat_cmd(301).
	p(NT(275), (NT(301)));
//G486: unsat_sym(306)       => 'u' 'n' 's' 'a' 't'.
	p(NT(306), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G487: __E_cli_command_144(305) => unsat_sym(306) __(10) spec_arg(35).
	p(NT(305), (NT(306)+NT(10)+NT(35)));
//G488: unsat_cmd(304)       => __E_cli_command_144(305).
	p(NT(304), (NT(305)));
//G489: cli_command(275)     => unsat_cmd(304).
	p(NT(275), (NT(304)));
//G490: realizable_sym(309)  => 'r' 'e' 'a' 'l' 'i' 'z' 'a' 'b' 'l' 'e'.
	p(NT(309), (T(19)+T(11)+T(16)+T(10)+T(4)+T(64)+T(16)+T(17)+T(10)+T(11)));
//G491: __E_cli_command_145(308) => realizable_sym(309) __(10) spec_arg(35).
	p(NT(308), (NT(309)+NT(10)+NT(35)));
//G492: realizable_cmd(307)  => __E_cli_command_145(308).
	p(NT(307), (NT(308)));
//G493: cli_command(275)     => realizable_cmd(307).
	p(NT(275), (NT(307)));
//G494: unrealizable_sym(312) => 'u' 'n' 'r' 'e' 'a' 'l' 'i' 'z' 'a' 'b' 'l' 'e'.
	p(NT(312), (T(7)+T(5)+T(19)+T(11)+T(16)+T(10)+T(4)+T(64)+T(16)+T(17)+T(10)+T(11)));
//G495: __E_cli_command_146(311) => unrealizable_sym(312) __(10) spec_arg(35).
	p(NT(311), (NT(312)+NT(10)+NT(35)));
//G496: unrealizable_cmd(310) => __E_cli_command_146(311).
	p(NT(310), (NT(311)));
//G497: cli_command(275)     => unrealizable_cmd(310).
	p(NT(275), (NT(310)));
//G498: solve_sym(315)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(315), (T(15)+T(6)+T(10)+T(61)+T(11)));
//G499: __E___E_cli_command_147_148(316) => solve_options(317).
	p(NT(316), (NT(317)));
//G500: __E___E_cli_command_147_148(316) => null.
	p(NT(316), (nul));
//G501: __E_cli_command_147(314) => solve_sym(315) __E___E_cli_command_147_148(316) __(10) wff_cmd_arg(32).
	p(NT(314), (NT(315)+NT(316)+NT(10)+NT(32)));
//G502: solve_cmd(313)       => __E_cli_command_147(314).
	p(NT(313), (NT(314)));
//G503: cli_command(275)     => solve_cmd(313).
	p(NT(275), (NT(313)));
//G504: lgrs_sym(320)        => 'l' 'g' 'r' 's'.
	p(NT(320), (T(10)+T(65)+T(19)+T(15)));
//G505: __E_cli_command_149(319) => lgrs_sym(320) __(10) wff_cmd_arg(32).
	p(NT(319), (NT(320)+NT(10)+NT(32)));
//G506: lgrs_cmd(318)        => __E_cli_command_149(319).
	p(NT(318), (NT(319)));
//G507: cli_command(275)     => lgrs_cmd(318).
	p(NT(275), (NT(318)));
//G508: __E___E_cli_command_150_151(324) => 'r'.
	p(NT(324), (T(19)));
//G509: __E___E_cli_command_150_151(324) => 'r' 'u' 'n'.
	p(NT(324), (T(19)+T(7)+T(5)));
//G510: run_sym(323)         => __E___E_cli_command_150_151(324).
	p(NT(323), (NT(324)));
//G511: __E___E___E_cli_command_150_152_153(326) => 's' 't' 'e' 'p' 's'.
	p(NT(326), (T(15)+T(8)+T(11)+T(63)+T(15)));
//G512: __E___E___E_cli_command_150_152_153(326) => 's' 't' 'e' 'p'.
	p(NT(326), (T(15)+T(8)+T(11)+T(63)));
//G513: __E___E_cli_command_150_152(325) => __(10) num(228) __(10) __E___E___E_cli_command_150_152_153(326).
	p(NT(325), (NT(10)+NT(228)+NT(10)+NT(326)));
//G514: __E___E_cli_command_150_152(325) => null.
	p(NT(325), (nul));
//G515: __E___E_cli_command_150_154(327) => __(10) spec_arg(35).
	p(NT(327), (NT(10)+NT(35)));
//G516: __E___E_cli_command_150_154(327) => null.
	p(NT(327), (nul));
//G517: __E_cli_command_150(322) => run_sym(323) __E___E_cli_command_150_152(325) __E___E_cli_command_150_154(327).
	p(NT(322), (NT(323)+NT(325)+NT(327)));
//G518: run_cmd(321)         => __E_cli_command_150(322).
	p(NT(321), (NT(322)));
//G519: cli_command(275)     => run_cmd(321).
	p(NT(275), (NT(321)));
//G520: stop_sym(330)        => 's' 't' 'o' 'p'.
	p(NT(330), (T(15)+T(8)+T(6)+T(63)));
//G521: __E_cli_command_155(329) => stop_sym(330).
	p(NT(329), (NT(330)));
//G522: stop_cmd(328)        => __E_cli_command_155(329).
	p(NT(328), (NT(329)));
//G523: cli_command(275)     => stop_cmd(328).
	p(NT(275), (NT(328)));
//G524: memory_sym(333)      => 'm' 'e' 'm' 'o' 'r' 'y'.
	p(NT(333), (T(21)+T(11)+T(21)+T(6)+T(19)+T(25)));
//G525: __E_cli_command_156(332) => memory_sym(333).
	p(NT(332), (NT(333)));
//G526: memory_cmd(331)      => __E_cli_command_156(332).
	p(NT(331), (NT(332)));
//G527: cli_command(275)     => memory_cmd(331).
	p(NT(275), (NT(331)));
//G528: ltl_sym(336)         => 'l' 't' 'l'.
	p(NT(336), (T(10)+T(8)+T(10)));
//G529: __E_cli_command_157(335) => ltl_sym(336) __(10) spec_arg(35).
	p(NT(335), (NT(336)+NT(10)+NT(35)));
//G530: ltl_cmd(334)         => __E_cli_command_157(335).
	p(NT(334), (NT(335)));
//G531: cli_command(275)     => ltl_cmd(334).
	p(NT(275), (NT(334)));
//G532: func_sym_cmd(339)    => 'f' 'u' 'n' 'c'.
	p(NT(339), (T(9)+T(7)+T(5)+T(14)));
//G533: __E_cli_command_158(338) => func_sym_cmd(339) __(10) func_decl(225).
	p(NT(338), (NT(339)+NT(10)+NT(225)));
//G534: func_cmd(337)        => __E_cli_command_158(338).
	p(NT(337), (NT(338)));
//G535: cli_command(275)     => func_cmd(337).
	p(NT(275), (NT(337)));
//G536: __E___E_cli_command_159_160(343) => 'n'.
	p(NT(343), (T(5)));
//G537: __E___E_cli_command_159_160(343) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(343), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(64)+T(11)));
//G538: normalize_sym(342)   => __E___E_cli_command_159_160(343).
	p(NT(342), (NT(343)));
//G539: __E_cli_command_159(341) => normalize_sym(342) __(10) spec_arg(35).
	p(NT(341), (NT(342)+NT(10)+NT(35)));
//G540: normalize_cmd(340)   => __E_cli_command_159(341).
	p(NT(340), (NT(341)));
//G541: cli_command(275)     => normalize_cmd(340).
	p(NT(275), (NT(340)));
//G542: __E___E_cli_command_161_162(347) => 's'.
	p(NT(347), (T(15)));
//G543: __E___E_cli_command_161_162(347) => 's' 'u' 'b' 's' 't'.
	p(NT(347), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G544: __E___E_cli_command_161_162(347) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(347), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G545: subst_sym(346)       => __E___E_cli_command_161_162(347).
	p(NT(346), (NT(347)));
//G546: __E_cli_command_161(345) => subst_sym(346) __(10) nf_cmd_arg(33) _(9) '[' _(9) nf_cmd_arg(33) _(9) '/' _(9) nf_cmd_arg(33) _(9) ']'.
	p(NT(345), (NT(346)+NT(10)+NT(33)+NT(9)+T(26)+NT(9)+NT(33)+NT(9)+T(47)+NT(9)+NT(33)+NT(9)+T(27)));
//G547: subst_cmd(344)       => __E_cli_command_161(345).
	p(NT(344), (NT(345)));
//G548: cli_command(275)     => subst_cmd(344).
	p(NT(275), (NT(344)));
//G549: __E___E_cli_command_163_164(351) => 'i'.
	p(NT(351), (T(4)));
//G550: __E___E_cli_command_163_164(351) => 'i' 'n' 's' 't'.
	p(NT(351), (T(4)+T(5)+T(15)+T(8)));
//G551: __E___E_cli_command_163_164(351) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(351), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G552: inst_sym(350)        => __E___E_cli_command_163_164(351).
	p(NT(350), (NT(351)));
//G553: __E_cli_command_163(349) => inst_sym(350) __(10) inst_args(30).
	p(NT(349), (NT(350)+NT(10)+NT(30)));
//G554: inst_cmd(348)        => __E_cli_command_163(349).
	p(NT(348), (NT(349)));
//G555: cli_command(275)     => inst_cmd(348).
	p(NT(275), (NT(348)));
//G556: dnf_sym(354)         => 'd' 'n' 'f'.
	p(NT(354), (T(51)+T(5)+T(9)));
//G557: __E_cli_command_165(353) => dnf_sym(354) __(10) nf_cmd_arg(33).
	p(NT(353), (NT(354)+NT(10)+NT(33)));
//G558: dnf_cmd(352)         => __E_cli_command_165(353).
	p(NT(352), (NT(353)));
//G559: cli_command(275)     => dnf_cmd(352).
	p(NT(275), (NT(352)));
//G560: cnf_sym(357)         => 'c' 'n' 'f'.
	p(NT(357), (T(14)+T(5)+T(9)));
//G561: __E_cli_command_166(356) => cnf_sym(357) __(10) nf_cmd_arg(33).
	p(NT(356), (NT(357)+NT(10)+NT(33)));
//G562: cnf_cmd(355)         => __E_cli_command_166(356).
	p(NT(355), (NT(356)));
//G563: cli_command(275)     => cnf_cmd(355).
	p(NT(275), (NT(355)));
//G564: nnf_sym(360)         => 'n' 'n' 'f'.
	p(NT(360), (T(5)+T(5)+T(9)));
//G565: __E_cli_command_167(359) => nnf_sym(360) __(10) nf_cmd_arg(33).
	p(NT(359), (NT(360)+NT(10)+NT(33)));
//G566: nnf_cmd(358)         => __E_cli_command_167(359).
	p(NT(358), (NT(359)));
//G567: cli_command(275)     => nnf_cmd(358).
	p(NT(275), (NT(358)));
//G568: mnf_sym(363)         => 'm' 'n' 'f'.
	p(NT(363), (T(21)+T(5)+T(9)));
//G569: __E_cli_command_168(362) => mnf_sym(363) __(10) nf_cmd_arg(33).
	p(NT(362), (NT(363)+NT(10)+NT(33)));
//G570: mnf_cmd(361)         => __E_cli_command_168(362).
	p(NT(361), (NT(362)));
//G571: cli_command(275)     => mnf_cmd(361).
	p(NT(275), (NT(361)));
//G572: onf_sym(366)         => 'o' 'n' 'f'.
	p(NT(366), (T(6)+T(5)+T(9)));
//G573: __E_cli_command_169(365) => onf_sym(366) __(10) variable(157) __(10) onf_cmd_arg(34).
	p(NT(365), (NT(366)+NT(10)+NT(157)+NT(10)+NT(34)));
//G574: onf_cmd(364)         => __E_cli_command_169(365).
	p(NT(364), (NT(365)));
//G575: cli_command(275)     => onf_cmd(364).
	p(NT(275), (NT(364)));
//G576: qelim_sym(369)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(369), (T(60)+T(11)+T(10)+T(4)+T(21)));
//G577: __E_cli_command_170(368) => qelim_sym(369) __(10) wff_cmd_arg(32).
	p(NT(368), (NT(369)+NT(10)+NT(32)));
//G578: qelim_cmd(367)       => __E_cli_command_170(368).
	p(NT(367), (NT(368)));
//G579: cli_command(275)     => qelim_cmd(367).
	p(NT(275), (NT(367)));
//G580: whatis_sym(372)      => 'w' 'h' 'a' 't' 'i' 's'.
	p(NT(372), (T(24)+T(62)+T(16)+T(8)+T(4)+T(15)));
//G581: __E_cli_command_171(371) => whatis_sym(372) __(10) spec_arg(35).
	p(NT(371), (NT(372)+NT(10)+NT(35)));
//G582: whatis_cmd(370)      => __E_cli_command_171(371).
	p(NT(370), (NT(371)));
//G583: cli_command(275)     => whatis_cmd(370).
	p(NT(275), (NT(370)));
//G584: reset_sym(375)       => 'r' 'e' 's' 'e' 't'.
	p(NT(375), (T(19)+T(11)+T(15)+T(11)+T(8)));
//G585: __E_cli_command_172(374) => reset_sym(375).
	p(NT(374), (NT(375)));
//G586: reset_cmd(373)       => __E_cli_command_172(374).
	p(NT(373), (NT(374)));
//G587: cli_command(275)     => reset_cmd(373).
	p(NT(275), (NT(373)));
//G588: fragment_sym(378)    => 'f' 'r' 'a' 'g' 'm' 'e' 'n' 't'.
	p(NT(378), (T(9)+T(19)+T(16)+T(65)+T(21)+T(11)+T(5)+T(8)));
//G589: __E_cli_command_173(377) => fragment_sym(378) __(10) fragment_name(379).
	p(NT(377), (NT(378)+NT(10)+NT(379)));
//G590: fragment_cmd(376)    => __E_cli_command_173(377).
	p(NT(376), (NT(377)));
//G591: cli_command(275)     => fragment_cmd(376).
	p(NT(275), (NT(376)));
//G592: get_sym(382)         => 'g' 'e' 't'.
	p(NT(382), (T(65)+T(11)+T(8)));
//G593: __E___E_cli_command_174_175(383) => __(10) option_name(23).
	p(NT(383), (NT(10)+NT(23)));
//G594: __E___E_cli_command_174_175(383) => null.
	p(NT(383), (nul));
//G595: __E_cli_command_174(381) => get_sym(382) __E___E_cli_command_174_175(383).
	p(NT(381), (NT(382)+NT(383)));
//G596: get_cmd(380)         => __E_cli_command_174(381).
	p(NT(380), (NT(381)));
//G597: cli_command(275)     => get_cmd(380).
	p(NT(275), (NT(380)));
//G598: set_sym(386)         => 's' 'e' 't'.
	p(NT(386), (T(15)+T(11)+T(8)));
//G599: __E___E_cli_command_176_177(387) => __(10).
	p(NT(387), (NT(10)));
//G600: __E___E_cli_command_176_177(387) => _(9) '=' _(9).
	p(NT(387), (NT(9)+T(3)+NT(9)));
//G601: __E_cli_command_176(385) => set_sym(386) __(10) option_name(23) __E___E_cli_command_176_177(387) option_value(24).
	p(NT(385), (NT(386)+NT(10)+NT(23)+NT(387)+NT(24)));
//G602: set_cmd(384)         => __E_cli_command_176(385).
	p(NT(384), (NT(385)));
//G603: cli_command(275)     => set_cmd(384).
	p(NT(275), (NT(384)));
//G604: enable_sym(390)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(390), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G605: __E_cli_command_178(389) => enable_sym(390) __(10) option_name(23).
	p(NT(389), (NT(390)+NT(10)+NT(23)));
//G606: enable_cmd(388)      => __E_cli_command_178(389).
	p(NT(388), (NT(389)));
//G607: cli_command(275)     => enable_cmd(388).
	p(NT(275), (NT(388)));
//G608: disable_sym(393)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(393), (T(51)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G609: __E_cli_command_179(392) => disable_sym(393) __(10) option_name(23).
	p(NT(392), (NT(393)+NT(10)+NT(23)));
//G610: disable_cmd(391)     => __E_cli_command_179(392).
	p(NT(391), (NT(392)));
//G611: cli_command(275)     => disable_cmd(391).
	p(NT(275), (NT(391)));
//G612: toggle_sym(396)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(396), (T(8)+T(6)+T(65)+T(65)+T(10)+T(11)));
//G613: __E_cli_command_180(395) => toggle_sym(396) __(10) option_name(23).
	p(NT(395), (NT(396)+NT(10)+NT(23)));
//G614: toggle_cmd(394)      => __E_cli_command_180(395).
	p(NT(394), (NT(395)));
//G615: cli_command(275)     => toggle_cmd(394).
	p(NT(275), (NT(394)));
//G616: __E___E_cli_command_181_182(400) => 'd' 'e' 'f' 's'.
	p(NT(400), (T(51)+T(11)+T(9)+T(15)));
//G617: __E___E_cli_command_181_182(400) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(400), (T(51)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G618: def_sym(399)         => __E___E_cli_command_181_182(400).
	p(NT(399), (NT(400)));
//G619: __E_cli_command_181(398) => def_sym(399).
	p(NT(398), (NT(399)));
//G620: def_list_cmd(397)    => __E_cli_command_181(398).
	p(NT(397), (NT(398)));
//G621: cli_command(275)     => def_list_cmd(397).
	p(NT(275), (NT(397)));
//G622: __E_cli_command_183(402) => def_sym(399) __(10) num(228).
	p(NT(402), (NT(399)+NT(10)+NT(228)));
//G623: def_print_cmd(401)   => __E_cli_command_183(402).
	p(NT(401), (NT(402)));
//G624: cli_command(275)     => def_print_cmd(401).
	p(NT(275), (NT(401)));
//G625: def_rr_cmd(403)      => rec_relation(54).
	p(NT(403), (NT(54)));
//G626: cli_command(275)     => def_rr_cmd(403).
	p(NT(275), (NT(403)));
//G627: def_input_cmd(404)   => input_def(55).
	p(NT(404), (NT(55)));
//G628: cli_command(275)     => def_input_cmd(404).
	p(NT(275), (NT(404)));
//G629: def_output_cmd(405)  => output_def(56).
	p(NT(405), (NT(56)));
//G630: cli_command(275)     => def_output_cmd(405).
	p(NT(275), (NT(405)));
//G631: __E___E_cli_command_184_185(409) => 'h' 'i' 's' 't'.
	p(NT(409), (T(62)+T(4)+T(15)+T(8)));
//G632: __E___E_cli_command_184_185(409) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(409), (T(62)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G633: history_sym(408)     => __E___E_cli_command_184_185(409).
	p(NT(408), (NT(409)));
//G634: __E_cli_command_184(407) => history_sym(408).
	p(NT(407), (NT(408)));
//G635: history_list_cmd(406) => __E_cli_command_184(407).
	p(NT(406), (NT(407)));
//G636: cli_command(275)     => history_list_cmd(406).
	p(NT(275), (NT(406)));
//G637: __E_cli_command_186(411) => history_sym(408) __(10) history(412).
	p(NT(411), (NT(408)+NT(10)+NT(412)));
//G638: history_print_cmd(410) => __E_cli_command_186(411).
	p(NT(410), (NT(411)));
//G639: cli_command(275)     => history_print_cmd(410).
	p(NT(275), (NT(410)));
//G640: __E_cli_command_187(414) => ref(62).
	p(NT(414), (NT(62)));
//G641: __E_cli_command_187(414) => wff(36).
	p(NT(414), (NT(36)));
//G642: __E_cli_command_187(414) => bf(38).
	p(NT(414), (NT(38)));
//G643: history_store_cmd(413) => __E_cli_command_187(414).
	p(NT(413), (NT(414)));
//G644: cli_command(275)     => history_store_cmd(413).
	p(NT(275), (NT(413)));
//G645: solve_options(317)   => __(10) solve_option(415) solve_options(317).
	p(NT(317), (NT(10)+NT(415)+NT(317)));
//G646: solve_options(317)   => null.
	p(NT(317), (nul));
//G647: __E_solve_option_188(416) => solver_mode(417).
	p(NT(416), (NT(417)));
//G648: __E_solve_option_188(416) => type(13).
	p(NT(416), (NT(13)));
//G649: solve_option(415)    => '-' '-' __E_solve_option_188(416).
	p(NT(415), (T(32)+T(32)+NT(416)));
//G650: __E_solver_mode_189(419) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(419), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G651: __E_solver_mode_189(419) => 'm' 'i' 'n'.
	p(NT(419), (T(21)+T(4)+T(5)));
//G652: solver_mode_minimum(418) => __E_solver_mode_189(419).
	p(NT(418), (NT(419)));
//G653: solver_mode(417)     => solver_mode_minimum(418).
	p(NT(417), (NT(418)));
//G654: __E_solver_mode_190(421) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(421), (T(21)+T(16)+T(34)+T(4)+T(21)+T(7)+T(21)));
//G655: __E_solver_mode_190(421) => 'm' 'a' 'x'.
	p(NT(421), (T(21)+T(16)+T(34)));
//G656: solver_mode_maximum(420) => __E_solver_mode_190(421).
	p(NT(420), (NT(421)));
//G657: solver_mode(417)     => solver_mode_maximum(420).
	p(NT(417), (NT(420)));
//G658: q_file_name(68)      => '"' file_name(21) '"'.
	p(NT(68), (T(66)+NT(21)+T(66)));
//G659: __E_file_name_191(422) => printable(8).
	p(NT(422), (NT(8)));
//G660: __E_file_name_191(422) => printable(8) __E_file_name_191(422).
	p(NT(422), (NT(8)+NT(422)));
//G661: file_name(21)        => __E_file_name_191(422).
	p(NT(21), (NT(422)));
//G662: __E_option_name_192(423) => alnum(6).
	p(NT(423), (NT(6)));
//G663: __E_option_name_192(423) => alnum(6) __E_option_name_192(423).
	p(NT(423), (NT(6)+NT(423)));
//G664: option_name(23)      => __E_option_name_192(423).
	p(NT(23), (NT(423)));
//G665: __E_option_value_193(424) => alnum(6).
	p(NT(424), (NT(6)));
//G666: __E_option_value_193(424) => alnum(6) __E_option_value_193(424).
	p(NT(424), (NT(6)+NT(424)));
//G667: option_value(24)     => __E_option_value_193(424).
	p(NT(24), (NT(424)));
//G668: fragment_ltl(426)    => 'l' 't' 'l'.
	p(NT(426), (T(10)+T(8)+T(10)));
//G669: __E_fragment_name_194(425) => fragment_ltl(426).
	p(NT(425), (NT(426)));
//G670: fragment_ctl_star(427) => 'c' 't' 'l' '_' 's' 't' 'a' 'r'.
	p(NT(427), (T(14)+T(8)+T(10)+T(54)+T(15)+T(8)+T(16)+T(19)));
//G671: __E_fragment_name_194(425) => fragment_ctl_star(427).
	p(NT(425), (NT(427)));
//G672: fragment_ctl_star(427) => 'c' 't' 'l' '*'.
	p(NT(427), (T(14)+T(8)+T(10)+T(46)));
//G673: __E_fragment_name_194(425) => fragment_ctl_star(427).
	p(NT(425), (NT(427)));
//G674: fragment_name(379)   => __E_fragment_name_194(425).
	p(NT(379), (NT(425)));
//G675: bf_cmd_arg(31)       => history(412).
	p(NT(31), (NT(412)));
//G676: bf_cmd_arg(31)       => bf(38).
	p(NT(31), (NT(38)));
//G677: wff_cmd_arg(32)      => history(412).
	p(NT(32), (NT(412)));
//G678: wff_cmd_arg(32)      => wff(36).
	p(NT(32), (NT(36)));
//G679: nf_cmd_arg(33)       => history(412).
	p(NT(33), (NT(412)));
//G680: nf_cmd_arg(33)       => ref(62).
	p(NT(33), (NT(62)));
//G681: nf_cmd_arg(33)       => wff(36).
	p(NT(33), (NT(36)));
//G682: nf_cmd_arg(33)       => bf(38).
	p(NT(33), (NT(38)));
//G683: onf_cmd_arg(34)      => history(412).
	p(NT(34), (NT(412)));
//G684: onf_cmd_arg(34)      => wff(36).
	p(NT(34), (NT(36)));
//G685: spec_arg(35)         => history(412).
	p(NT(35), (NT(412)));
//G686: spec_arg(35)         => spec(47).
	p(NT(35), (NT(47)));
//G687: spec_arg(35)         => ref(62).
	p(NT(35), (NT(62)));
//G688: spec_arg(35)         => wff(36).
	p(NT(35), (NT(36)));
//G689: spec_arg(35)         => bf(38).
	p(NT(35), (NT(38)));
//G690: inst_args(30)        => wff_cmd_arg(32) _(9) '[' _(9) bf(38) _(9) '/' _(9) bf_cmd_arg(31) _(9) ']'.
	p(NT(30), (NT(32)+NT(9)+T(26)+NT(9)+NT(38)+NT(9)+T(47)+NT(9)+NT(31)+NT(9)+T(27)));
//G691: inst_args(30)        => bf_cmd_arg(31) _(9) '[' _(9) bf(38) _(9) '/' _(9) bf_cmd_arg(31) _(9) ']'.
	p(NT(30), (NT(31)+NT(9)+T(26)+NT(9)+NT(38)+NT(9)+T(47)+NT(9)+NT(31)+NT(9)+T(27)));
//G692: help_arg(293)        => help_sym(290).
	p(NT(293), (NT(290)));
//G693: help_arg(293)        => version_sym(283).
	p(NT(293), (NT(283)));
//G694: help_arg(293)        => quit_sym(280).
	p(NT(293), (NT(280)));
//G695: help_arg(293)        => clear_sym(286).
	p(NT(293), (NT(286)));
//G696: help_arg(293)        => get_sym(382).
	p(NT(293), (NT(382)));
//G697: help_arg(293)        => set_sym(386).
	p(NT(293), (NT(386)));
//G698: help_arg(293)        => enable_sym(390).
	p(NT(293), (NT(390)));
//G699: help_arg(293)        => disable_sym(393).
	p(NT(293), (NT(393)));
//G700: help_arg(293)        => toggle_sym(396).
	p(NT(293), (NT(396)));
//G701: help_arg(293)        => file_sym(296).
	p(NT(293), (NT(296)));
//G702: help_arg(293)        => history_sym(408).
	p(NT(293), (NT(408)));
//G703: help_arg(293)        => abs_history_sym(428).
	p(NT(293), (NT(428)));
//G704: help_arg(293)        => rel_history_sym(429).
	p(NT(293), (NT(429)));
//G705: help_arg(293)        => selection_sym(430).
	p(NT(293), (NT(430)));
//G706: help_arg(293)        => def_sym(399).
	p(NT(293), (NT(399)));
//G707: help_arg(293)        => inst_sym(350).
	p(NT(293), (NT(350)));
//G708: help_arg(293)        => subst_sym(346).
	p(NT(293), (NT(346)));
//G709: help_arg(293)        => normalize_sym(342).
	p(NT(293), (NT(342)));
//G710: help_arg(293)        => execute_sym(431).
	p(NT(293), (NT(431)));
//G711: help_arg(293)        => solve_sym(315).
	p(NT(293), (NT(315)));
//G712: help_arg(293)        => lgrs_sym(320).
	p(NT(293), (NT(320)));
//G713: help_arg(293)        => valid_sym(300).
	p(NT(293), (NT(300)));
//G714: help_arg(293)        => sat_sym(303).
	p(NT(293), (NT(303)));
//G715: help_arg(293)        => unsat_sym(306).
	p(NT(293), (NT(306)));
//G716: help_arg(293)        => realizable_sym(309).
	p(NT(293), (NT(309)));
//G717: help_arg(293)        => unrealizable_sym(312).
	p(NT(293), (NT(312)));
//G718: help_arg(293)        => run_sym(323).
	p(NT(293), (NT(323)));
//G719: help_arg(293)        => ltl_sym(336).
	p(NT(293), (NT(336)));
//G720: help_arg(293)        => stop_sym(330).
	p(NT(293), (NT(330)));
//G721: help_arg(293)        => memory_sym(333).
	p(NT(293), (NT(333)));
//G722: help_arg(293)        => dnf_sym(354).
	p(NT(293), (NT(354)));
//G723: help_arg(293)        => cnf_sym(357).
	p(NT(293), (NT(357)));
//G724: help_arg(293)        => snf_sym(432).
	p(NT(293), (NT(432)));
//G725: help_arg(293)        => nnf_sym(360).
	p(NT(293), (NT(360)));
//G726: help_arg(293)        => mnf_sym(363).
	p(NT(293), (NT(363)));
//G727: help_arg(293)        => onf_sym(366).
	p(NT(293), (NT(366)));
//G728: help_arg(293)        => qelim_sym(369).
	p(NT(293), (NT(369)));
//G729: help_arg(293)        => whatis_sym(372).
	p(NT(293), (NT(372)));
//G730: help_arg(293)        => reset_sym(375).
	p(NT(293), (NT(375)));
//G731: help_arg(293)        => fragment_sym(378).
	p(NT(293), (NT(378)));
//G732: __E___E_help_arg_195_196(435) => 's'.
	p(NT(435), (T(15)));
//G733: __E___E_help_arg_195_196(435) => null.
	p(NT(435), (nul));
//G734: __E_help_arg_195(434) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_195_196(435).
	p(NT(434), (T(11)+T(34)+T(16)+T(21)+T(63)+T(10)+T(11)+NT(435)));
//G735: examples_sym(433)    => __E_help_arg_195(434).
	p(NT(433), (NT(434)));
//G736: help_arg(293)        => examples_sym(433).
	p(NT(293), (NT(433)));
//G737: __E_history_197(437) => '%'.
	p(NT(437), (T(45)));
//G738: rel_history_sym(429) => '%' '-'.
	p(NT(429), (T(45)+T(32)));
//G739: history_id(439)      => digits(12).
	p(NT(439), (NT(12)));
//G740: __E___E_history_197_198(438) => history_id(439).
	p(NT(438), (NT(439)));
//G741: __E___E_history_197_198(438) => null.
	p(NT(438), (nul));
//G742: __E_history_197(437) => rel_history_sym(429) __E___E_history_197_198(438).
	p(NT(437), (NT(429)+NT(438)));
//G743: rel_history(436)     => __E_history_197(437).
	p(NT(436), (NT(437)));
//G744: history(412)         => rel_history(436).
	p(NT(412), (NT(436)));
//G745: abs_history_sym(428) => '%'.
	p(NT(428), (T(45)));
//G746: __E_history_199(441) => abs_history_sym(428) history_id(439).
	p(NT(441), (NT(428)+NT(439)));
//G747: abs_history(440)     => __E_history_199(441).
	p(NT(440), (NT(441)));
//G748: history(412)         => abs_history(440).
	p(NT(412), (NT(440)));
//G749: unreachable(442)     => BDD_ID(443).
	p(NT(442), (NT(443)));
//G750: stream_def(444)      => input_def(55).
	p(NT(444), (NT(55)));
//G751: stream_def(444)      => output_def(56).
	p(NT(444), (NT(56)));
//G752: fm_or_term(445)      => wff(36).
	p(NT(445), (NT(36)));
//G753: fm_or_term(445)      => bf(38).
	p(NT(445), (NT(38)));
	#undef T
	#undef NT
	return loaded = true, p;
}

} // namespace tau_parser_data
