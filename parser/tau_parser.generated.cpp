// This file is generated from a file parser/tau.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
// productions() lives here so the table is compiled once, not per TU.
//
#include "parser.h"
#include "tau_parser.generated.h"

namespace tau_parser_data {

#ifndef TAU_PARSER_BUILD_HEADER_ONLY
idni::prods<char_type, terminal_type>& productions() {
	static bool loaded = false;
	static idni::prods<char_type, terminal_type>
		p, nul(idni::lit<char_type, terminal_type>{});
	if (loaded) return p;
	#define  T(x) (idni::prods<char_type, terminal_type>{ terminals[x] })
	#define NT(x) (idni::prods<char_type, terminal_type>{ nts(x) })
//G0:   start(48)            => spec(49).
	p(NT(48), (NT(49)));
//G1:   __E_spec_0(50)       => definitions(51).
	p(NT(50), (NT(51)));
//G2:   __E_spec_0(50)       => null.
	p(NT(50), (nul));
//G3:   main(52)             => wff(38).
	p(NT(52), (NT(38)));
//G4:   __E_spec_1(53)       => '.' _(9).
	p(NT(53), (T(1)+NT(9)));
//G5:   __E_spec_1(53)       => null.
	p(NT(53), (nul));
//G6:   spec(49)             => __E_spec_0(50) _(9) main(52) _(9) __E_spec_1(53).
	p(NT(49), (NT(50)+NT(9)+NT(52)+NT(9)+NT(53)));
//G7:   __E___E_definitions_2_3(55) => rec_relation(56).
	p(NT(55), (NT(56)));
//G8:   __E___E_definitions_2_3(55) => input_def(57).
	p(NT(55), (NT(57)));
//G9:   __E___E_definitions_2_3(55) => output_def(58).
	p(NT(55), (NT(58)));
//G10:  __E___E_definitions_2_3(55) => type_def(59).
	p(NT(55), (NT(59)));
//G11:  __E_definitions_2(54) => _(9) __E___E_definitions_2_3(55) _(9) '.'.
	p(NT(54), (NT(9)+NT(55)+NT(9)+T(1)));
//G12:  __E_definitions_4(60) => __E_definitions_2(54).
	p(NT(60), (NT(54)));
//G13:  __E_definitions_4(60) => __E_definitions_4(60) __E_definitions_2(54).
	p(NT(60), (NT(60)+NT(54)));
//G14:  definitions(51)      => __E_definitions_4(60).
	p(NT(51), (NT(60)));
//G15:  __E_spec_multiline_5(62) => _(9) spec_part(31) _(9) '.'.
	p(NT(62), (NT(9)+NT(31)+NT(9)+T(1)));
//G16:  __E_spec_multiline_6(63) => null.
	p(NT(63), (nul));
//G17:  __E_spec_multiline_6(63) => __E_spec_multiline_6(63) __E_spec_multiline_5(62).
	p(NT(63), (NT(63)+NT(62)));
//G18:  __E_spec_multiline_7(64) => _(9) '.'.
	p(NT(64), (NT(9)+T(1)));
//G19:  __E_spec_multiline_7(64) => null.
	p(NT(64), (nul));
//G20:  spec_multiline(61)   => __E_spec_multiline_6(63) _(9) spec_part(31) __E_spec_multiline_7(64) _(9).
	p(NT(61), (NT(63)+NT(9)+NT(31)+NT(64)+NT(9)));
//G21:  spec_multiline(61)   => _(9).
	p(NT(61), (NT(9)));
//G22:  spec_part(31)        => rec_relation(56).
	p(NT(31), (NT(56)));
//G23:  spec_part(31)        => input_def(57).
	p(NT(31), (NT(57)));
//G24:  spec_part(31)        => output_def(58).
	p(NT(31), (NT(58)));
//G25:  spec_part(31)        => type_def(59).
	p(NT(31), (NT(59)));
//G26:  spec_part(31)        => main(52).
	p(NT(31), (NT(52)));
//G27:  __E_rec_relation_8(66) => capture(17).
	p(NT(66), (NT(17)));
//G28:  __E_rec_relation_8(66) => ref(65).
	p(NT(66), (NT(65)));
//G29:  __E_rec_relation_8(66) => wff(38).
	p(NT(66), (NT(38)));
//G30:  __E_rec_relation_8(66) => bf(40).
	p(NT(66), (NT(40)));
//G31:  rec_relation(56)     => ref(65) _(9) ':' '=' _(9) __E_rec_relation_8(66).
	p(NT(56), (NT(65)+NT(9)+T(2)+T(3)+NT(9)+NT(66)));
//G32:  __E_input_def_9(67)  => member_path(68).
	p(NT(67), (NT(68)));
//G33:  __E_input_def_9(67)  => null.
	p(NT(67), (nul));
//G34:  __E_input_def_10(69) => typed(70).
	p(NT(69), (NT(70)));
//G35:  __E_input_def_10(69) => null.
	p(NT(69), (nul));
//G36:  input_def(57)        => io_var_name(19) __E_input_def_9(67) __E_input_def_10(69) _(9) ':' '=' _(9) 'i' 'n' __(10) stream(71).
	p(NT(57), (NT(19)+NT(67)+NT(69)+NT(9)+T(2)+T(3)+NT(9)+T(4)+T(5)+NT(10)+NT(71)));
//G37:  __E_output_def_11(72) => member_path(68).
	p(NT(72), (NT(68)));
//G38:  __E_output_def_11(72) => null.
	p(NT(72), (nul));
//G39:  __E_output_def_12(73) => typed(70).
	p(NT(73), (NT(70)));
//G40:  __E_output_def_12(73) => null.
	p(NT(73), (nul));
//G41:  output_def(58)       => io_var_name(19) __E_output_def_11(72) __E_output_def_12(73) _(9) ':' '=' _(9) 'o' 'u' 't' __(10) stream(71).
	p(NT(58), (NT(19)+NT(72)+NT(73)+NT(9)+T(2)+T(3)+NT(9)+T(6)+T(7)+T(8)+NT(10)+NT(71)));
//G42:  stream(71)           => 'f' 'i' 'l' 'e' _(9) '(' _(9) q_file_name(74) _(9) ')'.
	p(NT(71), (T(9)+T(4)+T(10)+T(11)+NT(9)+T(12)+NT(9)+NT(74)+NT(9)+T(13)));
//G43:  console_sym(75)      => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(75), (T(14)+T(6)+T(5)+T(15)+T(6)+T(10)+T(11)));
//G44:  stream(71)           => console_sym(75).
	p(NT(71), (NT(75)));
//G45:  type_name(25)        => chars(27).
	p(NT(25), (NT(27)));
//G46:  __E___E_type_def_13_14(77) => __(10) 'o' 'f' __(10).
	p(NT(77), (NT(10)+T(6)+T(9)+NT(10)));
//G47:  __E___E_type_def_13_14(77) => _(9) '/' _(9).
	p(NT(77), (NT(9)+T(16)+NT(9)));
//G48:  __E_type_def_13(76)  => __E___E_type_def_13_14(77) type_parents(78).
	p(NT(76), (NT(77)+NT(78)));
//G49:  __E_type_def_13(76)  => null.
	p(NT(76), (nul));
//G50:  __E_type_def_15(79)  => __(10) 'i' 's' __(10).
	p(NT(79), (NT(10)+T(4)+T(15)+NT(10)));
//G51:  __E_type_def_15(79)  => _(9) '=' _(9).
	p(NT(79), (NT(9)+T(3)+NT(9)));
//G52:  type_def(59)         => 't' 'y' 'p' 'e' __(10) type_name(25) __E_type_def_13(76) __E_type_def_15(79) type_body(80).
	p(NT(59), (T(8)+T(17)+T(18)+T(11)+NT(10)+NT(25)+NT(76)+NT(79)+NT(80)));
//G53:  __E_type_parents_16(81) => _(9) ',' _(9) type_name(25).
	p(NT(81), (NT(9)+T(19)+NT(9)+NT(25)));
//G54:  __E_type_parents_17(82) => null.
	p(NT(82), (nul));
//G55:  __E_type_parents_17(82) => __E_type_parents_17(82) __E_type_parents_16(81).
	p(NT(82), (NT(82)+NT(81)));
//G56:  type_parents(78)     => '(' _(9) type_name(25) __E_type_parents_17(82) _(9) ')'.
	p(NT(78), (T(12)+NT(9)+NT(25)+NT(82)+NT(9)+T(13)));
//G57:  type_body(80)        => tuple(83).
	p(NT(80), (NT(83)));
//G58:  __E_type_body_18(84) => _(9) '[' _(9) subtype(14) _(9) ']'.
	p(NT(84), (NT(9)+T(20)+NT(9)+NT(14)+NT(9)+T(21)));
//G59:  __E_type_body_18(84) => null.
	p(NT(84), (nul));
//G60:  type_body(80)        => type(13) __E_type_body_18(84).
	p(NT(80), (NT(13)+NT(84)));
//G61:  __E_tuple_19(86)     => member_name(26) typed(70).
	p(NT(86), (NT(26)+NT(70)));
//G62:  member(85)           => __E_tuple_19(86).
	p(NT(85), (NT(86)));
//G63:  __E_tuple_20(87)     => _(9) ',' _(9) member(85).
	p(NT(87), (NT(9)+T(19)+NT(9)+NT(85)));
//G64:  __E_tuple_21(88)     => null.
	p(NT(88), (nul));
//G65:  __E_tuple_21(88)     => __E_tuple_21(88) __E_tuple_20(87).
	p(NT(88), (NT(88)+NT(87)));
//G66:  tuple(83)            => '{' _(9) member(85) __E_tuple_21(88) _(9) '}'.
	p(NT(83), (T(22)+NT(9)+NT(85)+NT(88)+NT(9)+T(23)));
//G67:  member_name(26)      => chars(27).
	p(NT(26), (NT(27)));
//G68:  __E_member_path_22(89) => '.' member_name(26).
	p(NT(89), (T(1)+NT(26)));
//G69:  __E_member_path_23(90) => null.
	p(NT(90), (nul));
//G70:  __E_member_path_23(90) => __E_member_path_23(90) __E_member_path_22(89).
	p(NT(90), (NT(90)+NT(89)));
//G71:  member_path(68)      => '.' member_name(26) __E_member_path_23(90).
	p(NT(68), (T(1)+NT(26)+NT(90)));
//G72:  __E_ref_24(91)       => offsets(92).
	p(NT(91), (NT(92)));
//G73:  __E_ref_24(91)       => null.
	p(NT(91), (nul));
//G74:  __E_ref_25(94)       => typed(70).
	p(NT(94), (NT(70)));
//G75:  __E_ref_25(94)       => null.
	p(NT(94), (nul));
//G76:  __E_ref_26(95)       => _(9) 'f' 'a' 'l' 'l' 'b' 'a' 'c' 'k' __(10) fp_fallback(96).
	p(NT(95), (NT(9)+T(9)+T(24)+T(10)+T(10)+T(25)+T(24)+T(14)+T(26)+NT(10)+NT(96)));
//G77:  __E_ref_26(95)       => null.
	p(NT(95), (nul));
//G78:  ref(65)              => sym(11) __E_ref_24(91) ref_args(93) __E_ref_25(94) __E_ref_26(95).
	p(NT(65), (NT(11)+NT(91)+NT(93)+NT(94)+NT(95)));
//G79:  first_sym(97)        => 'f' 'i' 'r' 's' 't'.
	p(NT(97), (T(9)+T(4)+T(27)+T(15)+T(8)));
//G80:  fp_fallback(96)      => first_sym(97).
	p(NT(96), (NT(97)));
//G81:  last_sym(98)         => 'l' 'a' 's' 't'.
	p(NT(98), (T(10)+T(24)+T(15)+T(8)));
//G82:  fp_fallback(96)      => last_sym(98).
	p(NT(96), (NT(98)));
//G83:  fp_fallback(96)      => capture(17).
	p(NT(96), (NT(17)));
//G84:  fp_fallback(96)      => ref(65).
	p(NT(96), (NT(65)));
//G85:  fp_fallback(96)      => wff(38).
	p(NT(96), (NT(38)));
//G86:  fp_fallback(96)      => bf(40).
	p(NT(96), (NT(40)));
//G87:  ref_arg(101)         => bf(40).
	p(NT(101), (NT(40)));
//G88:  __E___E_ref_args_27_28(100) => _(9) ref_arg(101).
	p(NT(100), (NT(9)+NT(101)));
//G89:  __E___E_ref_args_27_29(102) => _(9) ',' _(9) ref_arg(101).
	p(NT(102), (NT(9)+T(19)+NT(9)+NT(101)));
//G90:  __E___E_ref_args_27_30(103) => null.
	p(NT(103), (nul));
//G91:  __E___E_ref_args_27_30(103) => __E___E_ref_args_27_30(103) __E___E_ref_args_27_29(102).
	p(NT(103), (NT(103)+NT(102)));
//G92:  __E_ref_args_27(99)  => __E___E_ref_args_27_28(100) __E___E_ref_args_27_30(103).
	p(NT(99), (NT(100)+NT(103)));
//G93:  __E_ref_args_27(99)  => null.
	p(NT(99), (nul));
//G94:  ref_args(93)         => '(' __E_ref_args_27(99) _(9) ')'.
	p(NT(93), (T(12)+NT(99)+NT(9)+T(13)));
//G95:  __E_wff_31(104)      => '(' _(9) wff(38) _(9) ')'.
	p(NT(104), (T(12)+NT(9)+NT(38)+NT(9)+T(13)));
//G96:  wff_parenthesis(39)  => __E_wff_31(104).
	p(NT(39), (NT(104)));
//G97:  wff(38)              => wff_parenthesis(39).
	p(NT(38), (NT(39)));
//G98:  __E___E_wff_32_33(107) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(107), (T(15)+T(6)+T(28)+T(11)+T(8)+T(4)+T(28)+T(11)+T(15)));
//G99:  __E___E_wff_32_33(107) => '<' '>'.
	p(NT(107), (T(29)+T(30)));
//G100: __E_wff_32(106)      => __E___E_wff_32_33(107) _(9) wff(38).
	p(NT(106), (NT(107)+NT(9)+NT(38)));
//G101: wff_sometimes(105)   => __E_wff_32(106).
	p(NT(105), (NT(106)));
//G102: wff(38)              => wff_sometimes(105).
	p(NT(38), (NT(105)));
//G103: __E___E_wff_34_35(110) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(110), (T(24)+T(10)+T(31)+T(24)+T(17)+T(15)));
//G104: __E___E_wff_34_35(110) => '[' ']'.
	p(NT(110), (T(20)+T(21)));
//G105: __E_wff_34(109)      => __E___E_wff_34_35(110) _(9) wff(38).
	p(NT(109), (NT(110)+NT(9)+NT(38)));
//G106: wff_always(108)      => __E_wff_34(109).
	p(NT(108), (NT(109)));
//G107: wff(38)              => wff_always(108).
	p(NT(38), (NT(108)));
//G108: __E_wff_36(112)      => wff(38) _(9) '?' _(9) wff(38) _(9) ':' _(9) wff(38).
	p(NT(112), (NT(38)+NT(9)+T(32)+NT(9)+NT(38)+NT(9)+T(2)+NT(9)+NT(38)));
//G109: wff_conditional(111) => __E_wff_36(112).
	p(NT(111), (NT(112)));
//G110: wff(38)              => wff_conditional(111).
	p(NT(38), (NT(111)));
//G111: __E_wff_37(114)      => 'a' 'l' 'l' __(10) q_vars(115) __(10) wff(38).
	p(NT(114), (T(24)+T(10)+T(10)+NT(10)+NT(115)+NT(10)+NT(38)));
//G112: wff_all(113)         => __E_wff_37(114).
	p(NT(113), (NT(114)));
//G113: wff(38)              => wff_all(113).
	p(NT(38), (NT(113)));
//G114: __E_wff_38(117)      => 'e' 'x' __(10) q_vars(115) __(10) wff(38).
	p(NT(117), (T(11)+T(33)+NT(10)+NT(115)+NT(10)+NT(38)));
//G115: wff_ex(116)          => __E_wff_38(117).
	p(NT(116), (NT(117)));
//G116: wff(38)              => wff_ex(116).
	p(NT(38), (NT(116)));
//G117: __N_0(431)           => bf_min(120).
	p(NT(431), (NT(120)));
//G118: __N_1(432)           => bf_max(121).
	p(NT(432), (NT(121)));
//G119: __E_wff_39(119)      => ref(65) & ~( __N_0(431) ) & ~( __N_1(432) ).	 # conjunctive
	p(NT(119), (NT(65)) & ~(NT(431)) & ~(NT(432)));
//G120: wff_ref(118)         => __E_wff_39(119).
	p(NT(118), (NT(119)));
//G121: wff(38)              => wff_ref(118).
	p(NT(38), (NT(118)));
//G122: wff(38)              => constraint(122).
	p(NT(38), (NT(122)));
//G123: __E_wff_40(124)      => wff(38) _(9) '-' '>' _(9) wff(38).
	p(NT(124), (NT(38)+NT(9)+T(34)+T(30)+NT(9)+NT(38)));
//G124: wff_imply(123)       => __E_wff_40(124).
	p(NT(123), (NT(124)));
//G125: wff(38)              => wff_imply(123).
	p(NT(38), (NT(123)));
//G126: __E_wff_41(126)      => wff(38) _(9) '<' '-' _(9) wff(38).
	p(NT(126), (NT(38)+NT(9)+T(29)+T(34)+NT(9)+NT(38)));
//G127: wff_rimply(125)      => __E_wff_41(126).
	p(NT(125), (NT(126)));
//G128: wff(38)              => wff_rimply(125).
	p(NT(38), (NT(125)));
//G129: __E_wff_42(128)      => wff(38) _(9) '<' '-' '>' _(9) wff(38).
	p(NT(128), (NT(38)+NT(9)+T(29)+T(34)+T(30)+NT(9)+NT(38)));
//G130: wff_equiv(127)       => __E_wff_42(128).
	p(NT(127), (NT(128)));
//G131: wff(38)              => wff_equiv(127).
	p(NT(38), (NT(127)));
//G132: __E_wff_43(130)      => wff(38) _(9) '|' '|' _(9) wff(38).
	p(NT(130), (NT(38)+NT(9)+T(35)+T(35)+NT(9)+NT(38)));
//G133: wff_or(129)          => __E_wff_43(130).
	p(NT(129), (NT(130)));
//G134: wff(38)              => wff_or(129).
	p(NT(38), (NT(129)));
//G135: __E_wff_44(132)      => wff(38) _(9) '^' '^' _(9) wff(38).
	p(NT(132), (NT(38)+NT(9)+T(36)+T(36)+NT(9)+NT(38)));
//G136: wff_xor(131)         => __E_wff_44(132).
	p(NT(131), (NT(132)));
//G137: wff(38)              => wff_xor(131).
	p(NT(38), (NT(131)));
//G138: __E_wff_45(134)      => wff(38) _(9) '&' '&' _(9) wff(38).
	p(NT(134), (NT(38)+NT(9)+T(37)+T(37)+NT(9)+NT(38)));
//G139: wff_and(133)         => __E_wff_45(134).
	p(NT(133), (NT(134)));
//G140: wff(38)              => wff_and(133).
	p(NT(38), (NT(133)));
//G141: __E_wff_46(136)      => '!' _(9) wff(38).
	p(NT(136), (T(38)+NT(9)+NT(38)));
//G142: wff_neg(135)         => __E_wff_46(136).
	p(NT(135), (NT(136)));
//G143: wff(38)              => wff_neg(135).
	p(NT(38), (NT(135)));
//G144: wff_t(137)           => 'T'.
	p(NT(137), (T(39)));
//G145: wff(38)              => wff_t(137).
	p(NT(38), (NT(137)));
//G146: wff_f(138)           => 'F'.
	p(NT(138), (T(40)));
//G147: wff(38)              => wff_f(138).
	p(NT(38), (NT(138)));
//G148: __E_wff_47(140)      => bf(40) _(9) '<' '=' _(9) bf(40) _(9) '<' '=' _(9) bf(40).
	p(NT(140), (NT(40)+NT(9)+T(29)+T(3)+NT(9)+NT(40)+NT(9)+T(29)+T(3)+NT(9)+NT(40)));
//G149: bf_interval(139)     => __E_wff_47(140).
	p(NT(139), (NT(140)));
//G150: wff(38)              => bf_interval(139).
	p(NT(38), (NT(139)));
//G151: __E_wff_48(142)      => bf(40) _(9) '=' _(9) bf(40).
	p(NT(142), (NT(40)+NT(9)+T(3)+NT(9)+NT(40)));
//G152: bf_eq(141)           => __E_wff_48(142).
	p(NT(141), (NT(142)));
//G153: wff(38)              => bf_eq(141).
	p(NT(38), (NT(141)));
//G154: __E_wff_49(144)      => bf(40) _(9) '!' '=' _(9) bf(40).
	p(NT(144), (NT(40)+NT(9)+T(38)+T(3)+NT(9)+NT(40)));
//G155: bf_neq(143)          => __E_wff_49(144).
	p(NT(143), (NT(144)));
//G156: wff(38)              => bf_neq(143).
	p(NT(38), (NT(143)));
//G157: __E_wff_50(146)      => bf(40) _(9) '<' _(9) bf(40).
	p(NT(146), (NT(40)+NT(9)+T(29)+NT(9)+NT(40)));
//G158: bf_lt(145)           => __E_wff_50(146).
	p(NT(145), (NT(146)));
//G159: wff(38)              => bf_lt(145).
	p(NT(38), (NT(145)));
//G160: __E_wff_51(148)      => bf(40) _(9) '!' '<' _(9) bf(40).
	p(NT(148), (NT(40)+NT(9)+T(38)+T(29)+NT(9)+NT(40)));
//G161: bf_nlt(147)          => __E_wff_51(148).
	p(NT(147), (NT(148)));
//G162: wff(38)              => bf_nlt(147).
	p(NT(38), (NT(147)));
//G163: __E_wff_52(150)      => bf(40) _(9) '<' '=' _(9) bf(40).
	p(NT(150), (NT(40)+NT(9)+T(29)+T(3)+NT(9)+NT(40)));
//G164: bf_lteq(149)         => __E_wff_52(150).
	p(NT(149), (NT(150)));
//G165: wff(38)              => bf_lteq(149).
	p(NT(38), (NT(149)));
//G166: __E_wff_53(152)      => bf(40) _(9) '!' '<' '=' _(9) bf(40).
	p(NT(152), (NT(40)+NT(9)+T(38)+T(29)+T(3)+NT(9)+NT(40)));
//G167: bf_nlteq(151)        => __E_wff_53(152).
	p(NT(151), (NT(152)));
//G168: wff(38)              => bf_nlteq(151).
	p(NT(38), (NT(151)));
//G169: __E_wff_54(154)      => bf(40) _(9) '>' _(9) bf(40).
	p(NT(154), (NT(40)+NT(9)+T(30)+NT(9)+NT(40)));
//G170: bf_gt(153)           => __E_wff_54(154).
	p(NT(153), (NT(154)));
//G171: wff(38)              => bf_gt(153).
	p(NT(38), (NT(153)));
//G172: __E_wff_55(156)      => bf(40) _(9) '!' '>' _(9) bf(40).
	p(NT(156), (NT(40)+NT(9)+T(38)+T(30)+NT(9)+NT(40)));
//G173: bf_ngt(155)          => __E_wff_55(156).
	p(NT(155), (NT(156)));
//G174: wff(38)              => bf_ngt(155).
	p(NT(38), (NT(155)));
//G175: __E_wff_56(158)      => bf(40) _(9) '>' '=' _(9) bf(40).
	p(NT(158), (NT(40)+NT(9)+T(30)+T(3)+NT(9)+NT(40)));
//G176: bf_gteq(157)         => __E_wff_56(158).
	p(NT(157), (NT(158)));
//G177: wff(38)              => bf_gteq(157).
	p(NT(38), (NT(157)));
//G178: __E_wff_57(160)      => bf(40) _(9) '!' '>' '=' _(9) bf(40).
	p(NT(160), (NT(40)+NT(9)+T(38)+T(30)+T(3)+NT(9)+NT(40)));
//G179: bf_ngteq(159)        => __E_wff_57(160).
	p(NT(159), (NT(160)));
//G180: wff(38)              => bf_ngteq(159).
	p(NT(38), (NT(159)));
//G181: wff(38)              => capture(17).
	p(NT(38), (NT(17)));
//G182: __N_2(433)           => bv_type_like(163).
	p(NT(433), (NT(163)));
//G183: __E___E_bf_58_59(162) => bf(40) & ~( __N_2(433) ).	 # conjunctive
	p(NT(162), (NT(40)) & ~(NT(433)));
//G184: __E_bf_58(161)       => '(' _(9) __E___E_bf_58_59(162) _(9) ')'.
	p(NT(161), (T(12)+NT(9)+NT(162)+NT(9)+T(13)));
//G185: bf_parenthesis(41)   => __E_bf_58(161).
	p(NT(41), (NT(161)));
//G186: bf(40)               => bf_parenthesis(41).
	p(NT(40), (NT(41)));
//G187: bf(40)               => ba_constant(164).
	p(NT(40), (NT(164)));
//G188: bf(40)               => variable(165).
	p(NT(40), (NT(165)));
//G189: __E_bf_60(167)       => 'f' 'a' 'l' 'l' __(10) q_vars(115) __(10) bf(40).
	p(NT(167), (T(9)+T(24)+T(10)+T(10)+NT(10)+NT(115)+NT(10)+NT(40)));
//G190: bf_fall(166)         => __E_bf_60(167).
	p(NT(166), (NT(167)));
//G191: bf(40)               => bf_fall(166).
	p(NT(40), (NT(166)));
//G192: __E_bf_61(169)       => 'f' 'e' 'x' __(10) q_vars(115) __(10) bf(40).
	p(NT(169), (T(9)+T(11)+T(33)+NT(10)+NT(115)+NT(10)+NT(40)));
//G193: bf_fex(168)          => __E_bf_61(169).
	p(NT(168), (NT(169)));
//G194: bf(40)               => bf_fex(168).
	p(NT(40), (NT(168)));
//G195: __E_bf_62(170)       => 'm' 'i' 'n' _(9) '(' _(9) bf(40) _(9) ',' _(9) bf(40) _(9) ')'.
	p(NT(170), (T(28)+T(4)+T(5)+NT(9)+T(12)+NT(9)+NT(40)+NT(9)+T(19)+NT(9)+NT(40)+NT(9)+T(13)));
//G196: bf_min(120)          => __E_bf_62(170).
	p(NT(120), (NT(170)));
//G197: bf(40)               => bf_min(120).
	p(NT(40), (NT(120)));
//G198: __E_bf_63(171)       => 'm' 'a' 'x' _(9) '(' _(9) bf(40) _(9) ',' _(9) bf(40) _(9) ')'.
	p(NT(171), (T(28)+T(24)+T(33)+NT(9)+T(12)+NT(9)+NT(40)+NT(9)+T(19)+NT(9)+NT(40)+NT(9)+T(13)));
//G199: bf_max(121)          => __E_bf_63(171).
	p(NT(121), (NT(171)));
//G200: bf(40)               => bf_max(121).
	p(NT(40), (NT(121)));
//G201: bf_ref(172)          => ref(65).
	p(NT(172), (NT(65)));
//G202: bf(40)               => bf_ref(172).
	p(NT(40), (NT(172)));
//G203: __E___E_bf_64_65(176) => bf_parenthesis(41).
	p(NT(176), (NT(41)));
//G204: __E___E_bf_64_65(176) => ba_constant(164).
	p(NT(176), (NT(164)));
//G205: __E___E_bf_64_65(176) => variable(165).
	p(NT(176), (NT(165)));
//G206: __E___E_bf_64_65(176) => bf_ref(172).
	p(NT(176), (NT(172)));
//G207: __E___E_bf_64_65(176) => bf_t(177).
	p(NT(176), (NT(177)));
//G208: __E___E_bf_64_65(176) => bf_f(178).
	p(NT(176), (NT(178)));
//G209: __E___E_bf_64_65(176) => bf_neg(179).
	p(NT(176), (NT(179)));
//G210: __E___E_bf_64_65(176) => bf_cast(173).
	p(NT(176), (NT(173)));
//G211: __E___E_bf_64_65(176) => capture(17).
	p(NT(176), (NT(17)));
//G212: __E___E_bf_64_65(176) => bf_fall(166).
	p(NT(176), (NT(166)));
//G213: __E___E_bf_64_65(176) => bf_fex(168).
	p(NT(176), (NT(168)));
//G214: bf_cast_oprnd(43)    => __E___E_bf_64_65(176).
	p(NT(43), (NT(176)));
//G215: __E_bf_64(174)       => '(' _(9) 'b' 'v' _(9) '[' _(9) num(175) _(9) ']' _(9) ')' _(9) bf_cast_oprnd(43).
	p(NT(174), (T(12)+NT(9)+T(25)+T(41)+NT(9)+T(20)+NT(9)+NT(175)+NT(9)+T(21)+NT(9)+T(13)+NT(9)+NT(43)));
//G216: bf_cast(173)         => __E_bf_64(174).
	p(NT(173), (NT(174)));
//G217: bf(40)               => bf_cast(173).
	p(NT(40), (NT(173)));
//G218: __N_3(434)           => bf_shr(180).
	p(NT(434), (NT(180)));
//G219: __N_4(435)           => bf_shl(183).
	p(NT(435), (NT(183)));
//G220: __E___E_bf_66_67(182) => bf(40) & ~( __N_3(434) ) & ~( __N_4(435) ).	 # conjunctive
	p(NT(182), (NT(40)) & ~(NT(434)) & ~(NT(435)));
//G221: __E_bf_66(181)       => bf(40) _(9) '>' '>' _(9) __E___E_bf_66_67(182).
	p(NT(181), (NT(40)+NT(9)+T(30)+T(30)+NT(9)+NT(182)));
//G222: bf_shr(180)          => __E_bf_66(181).
	p(NT(180), (NT(181)));
//G223: bf(40)               => bf_shr(180).
	p(NT(40), (NT(180)));
//G224: __N_5(436)           => bf_shr(180).
	p(NT(436), (NT(180)));
//G225: __N_6(437)           => bf_shl(183).
	p(NT(437), (NT(183)));
//G226: __E___E_bf_68_69(185) => bf(40) & ~( __N_5(436) ) & ~( __N_6(437) ).	 # conjunctive
	p(NT(185), (NT(40)) & ~(NT(436)) & ~(NT(437)));
//G227: __E_bf_68(184)       => bf(40) _(9) '<' '<' _(9) __E___E_bf_68_69(185).
	p(NT(184), (NT(40)+NT(9)+T(29)+T(29)+NT(9)+NT(185)));
//G228: bf_shl(183)          => __E_bf_68(184).
	p(NT(183), (NT(184)));
//G229: bf(40)               => bf_shl(183).
	p(NT(40), (NT(183)));
//G230: __N_7(438)           => bf_add(186).
	p(NT(438), (NT(186)));
//G231: __N_8(439)           => bf_sub(189).
	p(NT(439), (NT(189)));
//G232: __E___E_bf_70_71(188) => bf(40) & ~( __N_7(438) ) & ~( __N_8(439) ).	 # conjunctive
	p(NT(188), (NT(40)) & ~(NT(438)) & ~(NT(439)));
//G233: __E_bf_70(187)       => bf(40) _(9) '+' _(9) __E___E_bf_70_71(188).
	p(NT(187), (NT(40)+NT(9)+T(42)+NT(9)+NT(188)));
//G234: bf_add(186)          => __E_bf_70(187).
	p(NT(186), (NT(187)));
//G235: bf(40)               => bf_add(186).
	p(NT(40), (NT(186)));
//G236: __N_9(440)           => bf_add(186).
	p(NT(440), (NT(186)));
//G237: __N_10(441)          => bf_sub(189).
	p(NT(441), (NT(189)));
//G238: __E___E_bf_72_73(191) => bf(40) & ~( __N_9(440) ) & ~( __N_10(441) ).	 # conjunctive
	p(NT(191), (NT(40)) & ~(NT(440)) & ~(NT(441)));
//G239: __E_bf_72(190)       => bf(40) _(9) '-' _(9) __E___E_bf_72_73(191).
	p(NT(190), (NT(40)+NT(9)+T(34)+NT(9)+NT(191)));
//G240: bf_sub(189)          => __E_bf_72(190).
	p(NT(189), (NT(190)));
//G241: bf(40)               => bf_sub(189).
	p(NT(40), (NT(189)));
//G242: __N_11(442)          => bf_mod(192).
	p(NT(442), (NT(192)));
//G243: __N_12(443)          => bf_mul(195).
	p(NT(443), (NT(195)));
//G244: __N_13(444)          => bf_div(196).
	p(NT(444), (NT(196)));
//G245: __E___E_bf_74_75(194) => bf(40) & ~( __N_11(442) ) & ~( __N_12(443) ) & ~( __N_13(444) ).	 # conjunctive
	p(NT(194), (NT(40)) & ~(NT(442)) & ~(NT(443)) & ~(NT(444)));
//G246: __E_bf_74(193)       => bf(40) _(9) '%' _(9) __E___E_bf_74_75(194).
	p(NT(193), (NT(40)+NT(9)+T(43)+NT(9)+NT(194)));
//G247: bf_mod(192)          => __E_bf_74(193).
	p(NT(192), (NT(193)));
//G248: bf(40)               => bf_mod(192).
	p(NT(40), (NT(192)));
//G249: __N_14(445)          => bf_mod(192).
	p(NT(445), (NT(192)));
//G250: __N_15(446)          => bf_mul(195).
	p(NT(446), (NT(195)));
//G251: __N_16(447)          => bf_div(196).
	p(NT(447), (NT(196)));
//G252: __E___E_bf_76_77(198) => bf(40) & ~( __N_14(445) ) & ~( __N_15(446) ) & ~( __N_16(447) ).	 # conjunctive
	p(NT(198), (NT(40)) & ~(NT(445)) & ~(NT(446)) & ~(NT(447)));
//G253: __E_bf_76(197)       => bf(40) _(9) '*' _(9) __E___E_bf_76_77(198).
	p(NT(197), (NT(40)+NT(9)+T(44)+NT(9)+NT(198)));
//G254: bf_mul(195)          => __E_bf_76(197).
	p(NT(195), (NT(197)));
//G255: bf(40)               => bf_mul(195).
	p(NT(40), (NT(195)));
//G256: __N_17(448)          => bf_mod(192).
	p(NT(448), (NT(192)));
//G257: __N_18(449)          => bf_mul(195).
	p(NT(449), (NT(195)));
//G258: __N_19(450)          => bf_div(196).
	p(NT(450), (NT(196)));
//G259: __E___E_bf_78_79(200) => bf(40) & ~( __N_17(448) ) & ~( __N_18(449) ) & ~( __N_19(450) ).	 # conjunctive
	p(NT(200), (NT(40)) & ~(NT(448)) & ~(NT(449)) & ~(NT(450)));
//G260: __E_bf_78(199)       => bf(40) _(9) '/' _(9) __E___E_bf_78_79(200).
	p(NT(199), (NT(40)+NT(9)+T(16)+NT(9)+NT(200)));
//G261: bf_div(196)          => __E_bf_78(199).
	p(NT(196), (NT(199)));
//G262: bf(40)               => bf_div(196).
	p(NT(40), (NT(196)));
//G263: __N_20(451)          => bf_nor(201).
	p(NT(451), (NT(201)));
//G264: __E___E_bf_80_81(203) => bf(40) & ~( __N_20(451) ).	 # conjunctive
	p(NT(203), (NT(40)) & ~(NT(451)));
//G265: __E_bf_80(202)       => bf(40) _(9) '!' '|' _(9) __E___E_bf_80_81(203).
	p(NT(202), (NT(40)+NT(9)+T(38)+T(35)+NT(9)+NT(203)));
//G266: bf_nor(201)          => __E_bf_80(202).
	p(NT(201), (NT(202)));
//G267: bf(40)               => bf_nor(201).
	p(NT(40), (NT(201)));
//G268: __N_21(452)          => bf_xnor(204).
	p(NT(452), (NT(204)));
//G269: __E___E_bf_82_83(206) => bf(40) & ~( __N_21(452) ).	 # conjunctive
	p(NT(206), (NT(40)) & ~(NT(452)));
//G270: __E_bf_82(205)       => bf(40) _(9) '!' '^' _(9) __E___E_bf_82_83(206).
	p(NT(205), (NT(40)+NT(9)+T(38)+T(36)+NT(9)+NT(206)));
//G271: bf_xnor(204)         => __E_bf_82(205).
	p(NT(204), (NT(205)));
//G272: bf(40)               => bf_xnor(204).
	p(NT(40), (NT(204)));
//G273: __N_22(453)          => bf_nand(207).
	p(NT(453), (NT(207)));
//G274: __E___E_bf_84_85(209) => bf(40) & ~( __N_22(453) ).	 # conjunctive
	p(NT(209), (NT(40)) & ~(NT(453)));
//G275: __E_bf_84(208)       => bf(40) _(9) '!' '&' _(9) __E___E_bf_84_85(209).
	p(NT(208), (NT(40)+NT(9)+T(38)+T(37)+NT(9)+NT(209)));
//G276: bf_nand(207)         => __E_bf_84(208).
	p(NT(207), (NT(208)));
//G277: bf(40)               => bf_nand(207).
	p(NT(40), (NT(207)));
//G278: __E_bf_86(211)       => bf(40) _(9) '|' _(9) bf(40).
	p(NT(211), (NT(40)+NT(9)+T(35)+NT(9)+NT(40)));
//G279: bf_or(210)           => __E_bf_86(211).
	p(NT(210), (NT(211)));
//G280: bf(40)               => bf_or(210).
	p(NT(40), (NT(210)));
//G281: __E_bf_87(213)       => bf(40) _(9) '^' _(9) bf(40).
	p(NT(213), (NT(40)+NT(9)+T(36)+NT(9)+NT(40)));
//G282: bf_xor(212)          => __E_bf_87(213).
	p(NT(212), (NT(213)));
//G283: bf(40)               => bf_xor(212).
	p(NT(40), (NT(212)));
//G284: __E___E_bf_88_89(215) => typed(70).
	p(NT(215), (NT(70)));
//G285: __E___E_bf_88_89(215) => null.
	p(NT(215), (nul));
//G286: __E_bf_88(214)       => '1' __E___E_bf_88_89(215).
	p(NT(214), (T(45)+NT(215)));
//G287: bf_t(177)            => __E_bf_88(214).
	p(NT(177), (NT(214)));
//G288: bf(40)               => bf_t(177).
	p(NT(40), (NT(177)));
//G289: __E___E_bf_90_91(217) => typed(70).
	p(NT(217), (NT(70)));
//G290: __E___E_bf_90_91(217) => null.
	p(NT(217), (nul));
//G291: __E_bf_90(216)       => '0' __E___E_bf_90_91(217).
	p(NT(216), (T(46)+NT(217)));
//G292: bf_f(178)            => __E_bf_90(216).
	p(NT(178), (NT(216)));
//G293: bf(40)               => bf_f(178).
	p(NT(40), (NT(178)));
//G294: __E___E_bf_92_93(219) => _(9) '&' _(9).
	p(NT(219), (NT(9)+T(37)+NT(9)));
//G295: __E___E_bf_92_93(219) => __(10).
	p(NT(219), (NT(10)));
//G296: __E_bf_92(218)       => bf(40) __E___E_bf_92_93(219) bf(40).
	p(NT(218), (NT(40)+NT(219)+NT(40)));
//G297: bf_and(47)           => __E_bf_92(218).
	p(NT(47), (NT(218)));
//G298: bf(40)               => bf_and(47).
	p(NT(40), (NT(47)));
//G299: __E___E_bf_94_95(221) => bf_parenthesis(41).
	p(NT(221), (NT(41)));
//G300: __E___E_bf_94_95(221) => ba_constant(164).
	p(NT(221), (NT(164)));
//G301: __E___E_bf_94_95(221) => variable(165).
	p(NT(221), (NT(165)));
//G302: __E___E_bf_94_95(221) => bf_ref(172).
	p(NT(221), (NT(172)));
//G303: __E___E_bf_94_95(221) => bf_t(177).
	p(NT(221), (NT(177)));
//G304: __E___E_bf_94_95(221) => bf_f(178).
	p(NT(221), (NT(178)));
//G305: __E___E_bf_94_95(221) => bf_neg(179).
	p(NT(221), (NT(179)));
//G306: __E___E_bf_94_95(221) => capture(17).
	p(NT(221), (NT(17)));
//G307: bf_neg_oprnd(42)     => __E___E_bf_94_95(221).
	p(NT(42), (NT(221)));
//G308: __E_bf_94(220)       => bf_neg_oprnd(42) _(9) '\''.
	p(NT(220), (NT(42)+NT(9)+T(47)));
//G309: bf_neg(179)          => __E_bf_94(220).
	p(NT(179), (NT(220)));
//G310: bf(40)               => bf_neg(179).
	p(NT(40), (NT(179)));
//G311: __E___E_bf_96_97(223) => bf_parenthesis(41).
	p(NT(223), (NT(41)));
//G312: __E___E_bf_96_97(223) => variable(165).
	p(NT(223), (NT(165)));
//G313: __E___E_bf_96_97(223) => bf_ref(172).
	p(NT(223), (NT(172)));
//G314: __E___E_bf_96_97(223) => bf_neg(179).
	p(NT(223), (NT(179)));
//G315: __E___E_bf_96_97(223) => bf_and_nosep(46).
	p(NT(223), (NT(46)));
//G316: __E___E_bf_96_97(223) => capture(17).
	p(NT(223), (NT(17)));
//G317: bf_and_nosep_1st_oprnd(44) => __E___E_bf_96_97(223).
	p(NT(44), (NT(223)));
//G318: __E___E_bf_96_98(224) => bf_parenthesis(41).
	p(NT(224), (NT(41)));
//G319: __E___E_bf_96_98(224) => ba_constant(164).
	p(NT(224), (NT(164)));
//G320: __E___E_bf_96_98(224) => variable(165).
	p(NT(224), (NT(165)));
//G321: __E___E_bf_96_98(224) => bf_ref(172).
	p(NT(224), (NT(172)));
//G322: __E___E_bf_96_98(224) => bf_neg(179).
	p(NT(224), (NT(179)));
//G323: __E___E_bf_96_98(224) => capture(17).
	p(NT(224), (NT(17)));
//G324: bf_and_nosep_2nd_oprnd(45) => __E___E_bf_96_98(224).
	p(NT(45), (NT(224)));
//G325: __E_bf_96(222)       => bf_and_nosep_1st_oprnd(44) bf_and_nosep_2nd_oprnd(45).
	p(NT(222), (NT(44)+NT(45)));
//G326: bf_and_nosep(46)     => __E_bf_96(222).
	p(NT(46), (NT(222)));
//G327: bf(40)               => bf_and_nosep(46).
	p(NT(40), (NT(46)));
//G328: bf(40)               => capture(17).
	p(NT(40), (NT(17)));
//G329: __E_constraint_99(226) => '[' ctnvar(22) _(9) '!' '=' _(9) num(175) ']'.
	p(NT(226), (T(20)+NT(22)+NT(9)+T(38)+T(3)+NT(9)+NT(175)+T(21)));
//G330: __E_constraint_99(226) => '[' num(175) _(9) '!' '=' _(9) ctnvar(22) ']'.
	p(NT(226), (T(20)+NT(175)+NT(9)+T(38)+T(3)+NT(9)+NT(22)+T(21)));
//G331: ctn_neq(225)         => __E_constraint_99(226).
	p(NT(225), (NT(226)));
//G332: constraint(122)      => ctn_neq(225).
	p(NT(122), (NT(225)));
//G333: __E_constraint_100(228) => '[' ctnvar(22) _(9) '=' _(9) num(175) ']'.
	p(NT(228), (T(20)+NT(22)+NT(9)+T(3)+NT(9)+NT(175)+T(21)));
//G334: __E_constraint_100(228) => '[' num(175) _(9) '=' _(9) ctnvar(22) ']'.
	p(NT(228), (T(20)+NT(175)+NT(9)+T(3)+NT(9)+NT(22)+T(21)));
//G335: ctn_eq(227)          => __E_constraint_100(228).
	p(NT(227), (NT(228)));
//G336: constraint(122)      => ctn_eq(227).
	p(NT(122), (NT(227)));
//G337: __E_constraint_101(230) => '[' ctnvar(22) _(9) '>' '=' _(9) num(175) ']'.
	p(NT(230), (T(20)+NT(22)+NT(9)+T(30)+T(3)+NT(9)+NT(175)+T(21)));
//G338: __E_constraint_101(230) => '[' num(175) _(9) '>' '=' _(9) ctnvar(22) ']'.
	p(NT(230), (T(20)+NT(175)+NT(9)+T(30)+T(3)+NT(9)+NT(22)+T(21)));
//G339: ctn_gteq(229)        => __E_constraint_101(230).
	p(NT(229), (NT(230)));
//G340: constraint(122)      => ctn_gteq(229).
	p(NT(122), (NT(229)));
//G341: __E_constraint_102(232) => '[' ctnvar(22) _(9) '>' _(9) num(175) ']'.
	p(NT(232), (T(20)+NT(22)+NT(9)+T(30)+NT(9)+NT(175)+T(21)));
//G342: __E_constraint_102(232) => '[' num(175) _(9) '>' _(9) ctnvar(22) ']'.
	p(NT(232), (T(20)+NT(175)+NT(9)+T(30)+NT(9)+NT(22)+T(21)));
//G343: ctn_gt(231)          => __E_constraint_102(232).
	p(NT(231), (NT(232)));
//G344: constraint(122)      => ctn_gt(231).
	p(NT(122), (NT(231)));
//G345: __E_constraint_103(234) => '[' ctnvar(22) _(9) '<' '=' _(9) num(175) ']'.
	p(NT(234), (T(20)+NT(22)+NT(9)+T(29)+T(3)+NT(9)+NT(175)+T(21)));
//G346: __E_constraint_103(234) => '[' num(175) _(9) '<' '=' _(9) ctnvar(22) ']'.
	p(NT(234), (T(20)+NT(175)+NT(9)+T(29)+T(3)+NT(9)+NT(22)+T(21)));
//G347: ctn_lteq(233)        => __E_constraint_103(234).
	p(NT(233), (NT(234)));
//G348: constraint(122)      => ctn_lteq(233).
	p(NT(122), (NT(233)));
//G349: __E_constraint_104(236) => '[' ctnvar(22) _(9) '<' _(9) num(175) ']'.
	p(NT(236), (T(20)+NT(22)+NT(9)+T(29)+NT(9)+NT(175)+T(21)));
//G350: __E_constraint_104(236) => '[' num(175) _(9) '<' _(9) ctnvar(22) ']'.
	p(NT(236), (T(20)+NT(175)+NT(9)+T(29)+NT(9)+NT(22)+T(21)));
//G351: ctn_lt(235)          => __E_constraint_104(236).
	p(NT(235), (NT(236)));
//G352: constraint(122)      => ctn_lt(235).
	p(NT(122), (NT(235)));
//G353: __E_ba_constant_105(237) => null.
	p(NT(237), (nul));
//G354: __E_ba_constant_105(237) => __E_ba_constant_105(237) space(2).
	p(NT(237), (NT(237)+NT(2)));
//G355: __E_ba_constant_106(238) => capture(17).
	p(NT(238), (NT(17)));
//G356: __E_ba_constant_106(238) => source(16).
	p(NT(238), (NT(16)));
//G357: __E_ba_constant_107(239) => null.
	p(NT(239), (nul));
//G358: __E_ba_constant_107(239) => __E_ba_constant_107(239) space(2).
	p(NT(239), (NT(239)+NT(2)));
//G359: __E_ba_constant_108(240) => typed(70).
	p(NT(240), (NT(70)));
//G360: __E_ba_constant_108(240) => null.
	p(NT(240), (nul));
//G361: ba_constant(164)     => '{' __E_ba_constant_105(237) __E_ba_constant_106(238) __E_ba_constant_107(239) '}' __E_ba_constant_108(240).
	p(NT(164), (T(22)+NT(237)+NT(238)+NT(239)+T(23)+NT(240)));
//G362: __E___E_source_109_110(242) => src_c(29).
	p(NT(242), (NT(29)));
//G363: __E___E_source_109_110(242) => space(2).
	p(NT(242), (NT(2)));
//G364: __E___E_source_109_111(243) => null.
	p(NT(243), (nul));
//G365: __E___E_source_109_111(243) => __E___E_source_109_111(243) __E___E_source_109_110(242).
	p(NT(243), (NT(243)+NT(242)));
//G366: __E_source_109(241)  => __E___E_source_109_111(243) src_c(29).
	p(NT(241), (NT(243)+NT(29)));
//G367: __E_source_109(241)  => null.
	p(NT(241), (nul));
//G368: source(16)           => src_c(29) __E_source_109(241).
	p(NT(16), (NT(29)+NT(241)));
//G369: src_c(29)            => alnum(6).
	p(NT(29), (NT(6)));
//G370: __N_23(454)          => '{'.
	p(NT(454), (T(22)));
//G371: __N_24(455)          => '}'.
	p(NT(455), (T(23)));
//G372: src_c(29)            => ~( __N_23(454) ) & ~( __N_24(455) ) & punct(7).	 # conjunctive
	p(NT(29), ~(NT(454)) & ~(NT(455)) & (NT(7)));
//G373: __E_src_c_112(244)   => src_c(29).
	p(NT(244), (NT(29)));
//G374: __E_src_c_112(244)   => space(2).
	p(NT(244), (NT(2)));
//G375: __E_src_c_113(245)   => null.
	p(NT(245), (nul));
//G376: __E_src_c_113(245)   => __E_src_c_113(245) __E_src_c_112(244).
	p(NT(245), (NT(245)+NT(244)));
//G377: src_c(29)            => '{' __E_src_c_113(245) '}'.
	p(NT(29), (T(22)+NT(245)+T(23)));
//G378: __E_variable_114(246) => uconst(30).
	p(NT(246), (NT(30)));
//G379: __E_variable_114(246) => io_var(247).
	p(NT(246), (NT(247)));
//G380: __E_variable_114(246) => var_name(18).
	p(NT(246), (NT(18)));
//G381: __E_variable_115(248) => member_path(68).
	p(NT(248), (NT(68)));
//G382: __E_variable_115(248) => null.
	p(NT(248), (nul));
//G383: __E_variable_116(249) => typed(70).
	p(NT(249), (NT(70)));
//G384: __E_variable_116(249) => null.
	p(NT(249), (nul));
//G385: variable(165)        => __E_variable_114(246) __E_variable_115(248) __E_variable_116(249).
	p(NT(165), (NT(246)+NT(248)+NT(249)));
//G386: __N_25(456)          => 'F'.
	p(NT(456), (T(40)));
//G387: __N_26(457)          => 'T'.
	p(NT(457), (T(39)));
//G388: __E_var_name_117(250) => ~( __N_25(456) ) & ~( __N_26(457) ) & alpha(5).	 # conjunctive
	p(NT(250), ~(NT(456)) & ~(NT(457)) & (NT(5)));
//G389: __E_var_name_118(251) => null.
	p(NT(251), (nul));
//G390: __E_var_name_118(251) => __E_var_name_118(251) digit(3).
	p(NT(251), (NT(251)+NT(3)));
//G391: var_name(18)         => __E_var_name_117(250) __E_var_name_118(251).	 # guarded: charvar
	p(NT(18), (NT(250)+NT(251)));
	p.back().guard = "charvar";
//G392: __N_27(458)          => 'F'.
	p(NT(458), (T(40)));
//G393: __N_28(459)          => 'T'.
	p(NT(459), (T(39)));
//G394: __E_var_name_119(252) => ~( __N_27(458) ) & ~( __N_28(459) ) & alpha(5).	 # conjunctive
	p(NT(252), ~(NT(458)) & ~(NT(459)) & (NT(5)));
//G395: __E_var_name_120(253) => alnum(6).
	p(NT(253), (NT(6)));
//G396: __E_var_name_120(253) => '_'.
	p(NT(253), (T(48)));
//G397: __E_var_name_121(254) => null.
	p(NT(254), (nul));
//G398: __E_var_name_121(254) => __E_var_name_121(254) __E_var_name_120(253).
	p(NT(254), (NT(254)+NT(253)));
//G399: var_name(18)         => __E_var_name_119(252) __E_var_name_121(254).	 # guarded: var
	p(NT(18), (NT(252)+NT(254)));
	p.back().guard = "var";
//G400: io_var(247)          => io_var_name(19) '[' offset(255) ']'.
	p(NT(247), (NT(19)+T(20)+NT(255)+T(21)));
//G401: io_var_name(19)      => chars(27).
	p(NT(19), (NT(27)));
//G402: bv_type_like(163)    => 'b' 'v' _(9) '[' _(9) num(175) _(9) ']'.
	p(NT(163), (T(25)+T(41)+NT(9)+T(20)+NT(9)+NT(175)+NT(9)+T(21)));
//G403: __E___E_uconst_122_123(257) => chars(27) _(9).
	p(NT(257), (NT(27)+NT(9)));
//G404: __E___E_uconst_122_123(257) => null.
	p(NT(257), (nul));
//G405: __E_uconst_122(256)  => __E___E_uconst_122_123(257) ':' _(9) chars(27).
	p(NT(256), (NT(257)+T(2)+NT(9)+NT(27)));
//G406: uconst_name(20)      => __E_uconst_122(256).
	p(NT(20), (NT(256)));
//G407: uconst(30)           => '<' _(9) uconst_name(20) _(9) '>'.
	p(NT(30), (T(29)+NT(9)+NT(20)+NT(9)+T(30)));
//G408: __E_q_vars_124(258)  => _(9) ',' _(9) q_var(28).
	p(NT(258), (NT(9)+T(19)+NT(9)+NT(28)));
//G409: __E_q_vars_125(259)  => null.
	p(NT(259), (nul));
//G410: __E_q_vars_125(259)  => __E_q_vars_125(259) __E_q_vars_124(258).
	p(NT(259), (NT(259)+NT(258)));
//G411: q_vars(115)          => q_var(28) __E_q_vars_125(259).
	p(NT(115), (NT(28)+NT(259)));
//G412: q_var(28)            => capture(17).
	p(NT(28), (NT(17)));
//G413: __N_29(460)          => uconst(30).
	p(NT(460), (NT(30)));
//G414: q_var(28)            => ~( __N_29(460) ) & variable(165).	 # conjunctive
	p(NT(28), ~(NT(460)) & (NT(165)));
//G415: ctnvar(22)           => chars(27).
	p(NT(22), (NT(27)));
//G416: __E_offsets_126(260) => _(9) ',' _(9) offset(255).
	p(NT(260), (NT(9)+T(19)+NT(9)+NT(255)));
//G417: __E_offsets_127(261) => null.
	p(NT(261), (nul));
//G418: __E_offsets_127(261) => __E_offsets_127(261) __E_offsets_126(260).
	p(NT(261), (NT(261)+NT(260)));
//G419: offsets(92)          => '[' _(9) offset(255) __E_offsets_127(261) _(9) ']'.
	p(NT(92), (T(20)+NT(9)+NT(255)+NT(261)+NT(9)+T(21)));
//G420: offset(255)          => integer(15).
	p(NT(255), (NT(15)));
//G421: offset(255)          => capture(17).
	p(NT(255), (NT(17)));
//G422: offset(255)          => shift(262).
	p(NT(255), (NT(262)));
//G423: __N_30(461)          => io_var(247).
	p(NT(461), (NT(247)));
//G424: __E_offset_128(263)  => variable(165) & ~( __N_30(461) ).	 # conjunctive
	p(NT(263), (NT(165)) & ~(NT(461)));
//G425: offset(255)          => __E_offset_128(263).
	p(NT(255), (NT(263)));
//G426: __E_shift_129(264)   => capture(17).
	p(NT(264), (NT(17)));
//G427: __N_31(462)          => io_var(247).
	p(NT(462), (NT(247)));
//G428: __E___E_shift_129_130(265) => variable(165) & ~( __N_31(462) ).	 # conjunctive
	p(NT(265), (NT(165)) & ~(NT(462)));
//G429: __E_shift_129(264)   => __E___E_shift_129_130(265).
	p(NT(264), (NT(265)));
//G430: shift(262)           => __E_shift_129(264) _(9) '-' _(9) num(175).
	p(NT(262), (NT(264)+NT(9)+T(34)+NT(9)+NT(175)));
//G431: __E_chars_131(266)   => alnum(6).
	p(NT(266), (NT(6)));
//G432: __E_chars_131(266)   => '_'.
	p(NT(266), (T(48)));
//G433: __E_chars_132(267)   => null.
	p(NT(267), (nul));
//G434: __E_chars_132(267)   => __E_chars_132(267) __E_chars_131(266).
	p(NT(267), (NT(267)+NT(266)));
//G435: chars(27)            => alpha(5) __E_chars_132(267).
	p(NT(27), (NT(5)+NT(267)));
//G436: __E_digits_133(268)  => digit(3).
	p(NT(268), (NT(3)));
//G437: __E_digits_133(268)  => __E_digits_133(268) digit(3).
	p(NT(268), (NT(268)+NT(3)));
//G438: digits(12)           => __E_digits_133(268).
	p(NT(12), (NT(268)));
//G439: num(175)             => digits(12).
	p(NT(175), (NT(12)));
//G440: __E_integer_134(269) => '-'.
	p(NT(269), (T(34)));
//G441: __E_integer_134(269) => null.
	p(NT(269), (nul));
//G442: integer(15)          => __E_integer_134(269) _(9) digits(12).
	p(NT(15), (NT(269)+NT(9)+NT(12)));
//G443: sym(11)              => chars(27).
	p(NT(11), (NT(27)));
//G444: capture(17)          => '$' chars(27).
	p(NT(17), (T(49)+NT(27)));
//G445: typed(70)            => _(9) ':' _(9) type(13) _(9) '[' _(9) subtype(14) _(9) ']'.
	p(NT(70), (NT(9)+T(2)+NT(9)+NT(13)+NT(9)+T(20)+NT(9)+NT(14)+NT(9)+T(21)));
//G446: __N_32(463)          => 'b' 'v'.
	p(NT(463), (T(25)+T(41)));
//G447: __E_typed_135(270)   => ~( __N_32(463) ) & type(13).	 # conjunctive
	p(NT(270), ~(NT(463)) & (NT(13)));
//G448: typed(70)            => _(9) ':' _(9) __E_typed_135(270).
	p(NT(70), (NT(9)+T(2)+NT(9)+NT(270)));
//G449: type(13)             => chars(27).
	p(NT(13), (NT(27)));
//G450: subtype(14)          => num(175).
	p(NT(14), (NT(175)));
//G451: __E_comment_136(272) => printable(8).
	p(NT(272), (NT(8)));
//G452: __E_comment_136(272) => '\t'.
	p(NT(272), (T(50)));
//G453: __E_comment_137(273) => null.
	p(NT(273), (nul));
//G454: __E_comment_137(273) => __E_comment_137(273) __E_comment_136(272).
	p(NT(273), (NT(273)+NT(272)));
//G455: __E_comment_138(274) => '\n'.
	p(NT(274), (T(51)));
//G456: __E_comment_138(274) => '\r'.
	p(NT(274), (T(52)));
//G457: __E_comment_138(274) => eof(1).
	p(NT(274), (NT(1)));
//G458: comment(271)         => '#' __E_comment_137(273) __E_comment_138(274).
	p(NT(271), (T(53)+NT(273)+NT(274)));
//G459: __(10)               => space(2).
	p(NT(10), (NT(2)));
//G460: __(10)               => comment(271).
	p(NT(10), (NT(271)));
//G461: __(10)               => __(10) space(2).
	p(NT(10), (NT(10)+NT(2)));
//G462: __(10)               => __(10) comment(271).
	p(NT(10), (NT(10)+NT(271)));
//G463: _(9)                 => __(10).
	p(NT(9), (NT(10)));
//G464: _(9)                 => null.
	p(NT(9), (nul));
//G465: cli(275)             => _(9).
	p(NT(275), (NT(9)));
//G466: __E_cli_139(277)     => '.' _(9) cli_command(276) _(9).
	p(NT(277), (T(1)+NT(9)+NT(276)+NT(9)));
//G467: __E_cli_140(278)     => null.
	p(NT(278), (nul));
//G468: __E_cli_140(278)     => __E_cli_140(278) __E_cli_139(277).
	p(NT(278), (NT(278)+NT(277)));
//G469: __E_cli_141(279)     => '.' _(9).
	p(NT(279), (T(1)+NT(9)));
//G470: __E_cli_141(279)     => null.
	p(NT(279), (nul));
//G471: cli(275)             => _(9) cli_command(276) _(9) __E_cli_140(278) __E_cli_141(279).
	p(NT(275), (NT(9)+NT(276)+NT(9)+NT(278)+NT(279)));
//G472: __E_cli_command_142(282) => 'q'.
	p(NT(282), (T(54)));
//G473: __E_cli_command_142(282) => 'q' 'u' 'i' 't'.
	p(NT(282), (T(54)+T(7)+T(4)+T(8)));
//G474: quit_sym(281)        => __E_cli_command_142(282).
	p(NT(281), (NT(282)));
//G475: quit_cmd(280)        => quit_sym(281).
	p(NT(280), (NT(281)));
//G476: cli_command(276)     => quit_cmd(280).
	p(NT(276), (NT(280)));
//G477: __E_cli_command_143(285) => 'v'.
	p(NT(285), (T(41)));
//G478: __E_cli_command_143(285) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(285), (T(41)+T(11)+T(27)+T(15)+T(4)+T(6)+T(5)));
//G479: version_sym(284)     => __E_cli_command_143(285).
	p(NT(284), (NT(285)));
//G480: version_cmd(283)     => version_sym(284).
	p(NT(283), (NT(284)));
//G481: cli_command(276)     => version_cmd(283).
	p(NT(276), (NT(283)));
//G482: __E_cli_command_144(288) => 'c'.
	p(NT(288), (T(14)));
//G483: __E_cli_command_144(288) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(288), (T(14)+T(10)+T(11)+T(24)+T(27)));
//G484: clear_sym(287)       => __E_cli_command_144(288).
	p(NT(287), (NT(288)));
//G485: clear_cmd(286)       => clear_sym(287).
	p(NT(286), (NT(287)));
//G486: cli_command(276)     => clear_cmd(286).
	p(NT(276), (NT(286)));
//G487: __E___E_cli_command_145_146(292) => 'h'.
	p(NT(292), (T(55)));
//G488: __E___E_cli_command_145_146(292) => 'h' 'e' 'l' 'p'.
	p(NT(292), (T(55)+T(11)+T(10)+T(18)));
//G489: help_sym(291)        => __E___E_cli_command_145_146(292).
	p(NT(291), (NT(292)));
//G490: __E___E_cli_command_145_147(293) => __(10) help_arg(294).
	p(NT(293), (NT(10)+NT(294)));
//G491: __E___E_cli_command_145_147(293) => null.
	p(NT(293), (nul));
//G492: __E_cli_command_145(290) => help_sym(291) __E___E_cli_command_145_147(293).
	p(NT(290), (NT(291)+NT(293)));
//G493: help_cmd(289)        => __E_cli_command_145(290).
	p(NT(289), (NT(290)));
//G494: cli_command(276)     => help_cmd(289).
	p(NT(276), (NT(289)));
//G495: file_sym(297)        => 'f' 'i' 'l' 'e'.
	p(NT(297), (T(9)+T(4)+T(10)+T(11)));
//G496: __E_cli_command_148(296) => file_sym(297) __(10) q_string(298).
	p(NT(296), (NT(297)+NT(10)+NT(298)));
//G497: file_cmd(295)        => __E_cli_command_148(296).
	p(NT(295), (NT(296)));
//G498: cli_command(276)     => file_cmd(295).
	p(NT(276), (NT(295)));
//G499: valid_sym(301)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(301), (T(41)+T(24)+T(10)+T(4)+T(56)));
//G500: __E_cli_command_149(300) => valid_sym(301) __(10) spec_arg(37).
	p(NT(300), (NT(301)+NT(10)+NT(37)));
//G501: valid_cmd(299)       => __E_cli_command_149(300).
	p(NT(299), (NT(300)));
//G502: cli_command(276)     => valid_cmd(299).
	p(NT(276), (NT(299)));
//G503: sat_sym(304)         => 's' 'a' 't'.
	p(NT(304), (T(15)+T(24)+T(8)));
//G504: __E_cli_command_150(303) => sat_sym(304) __(10) spec_arg(37).
	p(NT(303), (NT(304)+NT(10)+NT(37)));
//G505: sat_cmd(302)         => __E_cli_command_150(303).
	p(NT(302), (NT(303)));
//G506: cli_command(276)     => sat_cmd(302).
	p(NT(276), (NT(302)));
//G507: unsat_sym(307)       => 'u' 'n' 's' 'a' 't'.
	p(NT(307), (T(7)+T(5)+T(15)+T(24)+T(8)));
//G508: __E_cli_command_151(306) => unsat_sym(307) __(10) spec_arg(37).
	p(NT(306), (NT(307)+NT(10)+NT(37)));
//G509: unsat_cmd(305)       => __E_cli_command_151(306).
	p(NT(305), (NT(306)));
//G510: cli_command(276)     => unsat_cmd(305).
	p(NT(276), (NT(305)));
//G511: solve_sym(310)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(310), (T(15)+T(6)+T(10)+T(41)+T(11)));
//G512: __E___E_cli_command_152_153(311) => solve_options(312).
	p(NT(311), (NT(312)));
//G513: __E___E_cli_command_152_153(311) => null.
	p(NT(311), (nul));
//G514: __E_cli_command_152(309) => solve_sym(310) __E___E_cli_command_152_153(311) __(10) wff_cmd_arg(34).
	p(NT(309), (NT(310)+NT(311)+NT(10)+NT(34)));
//G515: solve_cmd(308)       => __E_cli_command_152(309).
	p(NT(308), (NT(309)));
//G516: cli_command(276)     => solve_cmd(308).
	p(NT(276), (NT(308)));
//G517: lgrs_sym(315)        => 'l' 'g' 'r' 's'.
	p(NT(315), (T(10)+T(57)+T(27)+T(15)));
//G518: __E_cli_command_154(314) => lgrs_sym(315) __(10) wff_cmd_arg(34).
	p(NT(314), (NT(315)+NT(10)+NT(34)));
//G519: lgrs_cmd(313)        => __E_cli_command_154(314).
	p(NT(313), (NT(314)));
//G520: cli_command(276)     => lgrs_cmd(313).
	p(NT(276), (NT(313)));
//G521: __E___E_cli_command_155_156(319) => 'r'.
	p(NT(319), (T(27)));
//G522: __E___E_cli_command_155_156(319) => 'r' 'u' 'n'.
	p(NT(319), (T(27)+T(7)+T(5)));
//G523: run_sym(318)         => __E___E_cli_command_155_156(319).
	p(NT(318), (NT(319)));
//G524: __E_cli_command_155(317) => run_sym(318) __(10) spec_arg(37).
	p(NT(317), (NT(318)+NT(10)+NT(37)));
//G525: run_cmd(316)         => __E_cli_command_155(317).
	p(NT(316), (NT(317)));
//G526: cli_command(276)     => run_cmd(316).
	p(NT(276), (NT(316)));
//G527: __E___E_cli_command_157_158(323) => 'n'.
	p(NT(323), (T(5)));
//G528: __E___E_cli_command_157_158(323) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(323), (T(5)+T(6)+T(27)+T(28)+T(24)+T(10)+T(4)+T(58)+T(11)));
//G529: normalize_sym(322)   => __E___E_cli_command_157_158(323).
	p(NT(322), (NT(323)));
//G530: __E_cli_command_157(321) => normalize_sym(322) __(10) spec_arg(37).
	p(NT(321), (NT(322)+NT(10)+NT(37)));
//G531: normalize_cmd(320)   => __E_cli_command_157(321).
	p(NT(320), (NT(321)));
//G532: cli_command(276)     => normalize_cmd(320).
	p(NT(276), (NT(320)));
//G533: __E___E_cli_command_159_160(327) => 's'.
	p(NT(327), (T(15)));
//G534: __E___E_cli_command_159_160(327) => 's' 'u' 'b' 's' 't'.
	p(NT(327), (T(15)+T(7)+T(25)+T(15)+T(8)));
//G535: __E___E_cli_command_159_160(327) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(327), (T(15)+T(7)+T(25)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G536: subst_sym(326)       => __E___E_cli_command_159_160(327).
	p(NT(326), (NT(327)));
//G537: __E___E_cli_command_159_161(328) => _(9) subst_group(329).
	p(NT(328), (NT(9)+NT(329)));
//G538: __E___E_cli_command_159_162(330) => __E___E_cli_command_159_161(328).
	p(NT(330), (NT(328)));
//G539: __E___E_cli_command_159_162(330) => __E___E_cli_command_159_162(330) __E___E_cli_command_159_161(328).
	p(NT(330), (NT(330)+NT(328)));
//G540: __E_cli_command_159(325) => subst_sym(326) __(10) nf_cmd_arg(35) __E___E_cli_command_159_162(330).
	p(NT(325), (NT(326)+NT(10)+NT(35)+NT(330)));
//G541: subst_cmd(324)       => __E_cli_command_159(325).
	p(NT(324), (NT(325)));
//G542: cli_command(276)     => subst_cmd(324).
	p(NT(276), (NT(324)));
//G543: __E___E_cli_command_163_164(334) => 'i'.
	p(NT(334), (T(4)));
//G544: __E___E_cli_command_163_164(334) => 'i' 'n' 's' 't'.
	p(NT(334), (T(4)+T(5)+T(15)+T(8)));
//G545: __E___E_cli_command_163_164(334) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(334), (T(4)+T(5)+T(15)+T(8)+T(24)+T(5)+T(8)+T(4)+T(24)+T(8)+T(11)));
//G546: inst_sym(333)        => __E___E_cli_command_163_164(334).
	p(NT(333), (NT(334)));
//G547: __E_cli_command_163(332) => inst_sym(333) __(10) inst_args(32).
	p(NT(332), (NT(333)+NT(10)+NT(32)));
//G548: inst_cmd(331)        => __E_cli_command_163(332).
	p(NT(331), (NT(332)));
//G549: cli_command(276)     => inst_cmd(331).
	p(NT(276), (NT(331)));
//G550: dnf_sym(337)         => 'd' 'n' 'f'.
	p(NT(337), (T(56)+T(5)+T(9)));
//G551: __E_cli_command_165(336) => dnf_sym(337) __(10) nf_cmd_arg(35).
	p(NT(336), (NT(337)+NT(10)+NT(35)));
//G552: dnf_cmd(335)         => __E_cli_command_165(336).
	p(NT(335), (NT(336)));
//G553: cli_command(276)     => dnf_cmd(335).
	p(NT(276), (NT(335)));
//G554: cnf_sym(340)         => 'c' 'n' 'f'.
	p(NT(340), (T(14)+T(5)+T(9)));
//G555: __E_cli_command_166(339) => cnf_sym(340) __(10) nf_cmd_arg(35).
	p(NT(339), (NT(340)+NT(10)+NT(35)));
//G556: cnf_cmd(338)         => __E_cli_command_166(339).
	p(NT(338), (NT(339)));
//G557: cli_command(276)     => cnf_cmd(338).
	p(NT(276), (NT(338)));
//G558: nnf_sym(343)         => 'n' 'n' 'f'.
	p(NT(343), (T(5)+T(5)+T(9)));
//G559: __E_cli_command_167(342) => nnf_sym(343) __(10) nf_cmd_arg(35).
	p(NT(342), (NT(343)+NT(10)+NT(35)));
//G560: nnf_cmd(341)         => __E_cli_command_167(342).
	p(NT(341), (NT(342)));
//G561: cli_command(276)     => nnf_cmd(341).
	p(NT(276), (NT(341)));
//G562: mnf_sym(346)         => 'm' 'n' 'f'.
	p(NT(346), (T(28)+T(5)+T(9)));
//G563: __E_cli_command_168(345) => mnf_sym(346) __(10) nf_cmd_arg(35).
	p(NT(345), (NT(346)+NT(10)+NT(35)));
//G564: mnf_cmd(344)         => __E_cli_command_168(345).
	p(NT(344), (NT(345)));
//G565: cli_command(276)     => mnf_cmd(344).
	p(NT(276), (NT(344)));
//G566: onf_sym(349)         => 'o' 'n' 'f'.
	p(NT(349), (T(6)+T(5)+T(9)));
//G567: __E_cli_command_169(348) => onf_sym(349) __(10) variable(165) __(10) onf_cmd_arg(36).
	p(NT(348), (NT(349)+NT(10)+NT(165)+NT(10)+NT(36)));
//G568: onf_cmd(347)         => __E_cli_command_169(348).
	p(NT(347), (NT(348)));
//G569: cli_command(276)     => onf_cmd(347).
	p(NT(276), (NT(347)));
//G570: qelim_sym(352)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(352), (T(54)+T(11)+T(10)+T(4)+T(28)));
//G571: __E_cli_command_170(351) => qelim_sym(352) __(10) wff_cmd_arg(34).
	p(NT(351), (NT(352)+NT(10)+NT(34)));
//G572: qelim_cmd(350)       => __E_cli_command_170(351).
	p(NT(350), (NT(351)));
//G573: cli_command(276)     => qelim_cmd(350).
	p(NT(276), (NT(350)));
//G574: get_sym(355)         => 'g' 'e' 't'.
	p(NT(355), (T(57)+T(11)+T(8)));
//G575: __E___E_cli_command_171_172(356) => __(10) option_name(23).
	p(NT(356), (NT(10)+NT(23)));
//G576: __E___E_cli_command_171_172(356) => null.
	p(NT(356), (nul));
//G577: __E_cli_command_171(354) => get_sym(355) __E___E_cli_command_171_172(356).
	p(NT(354), (NT(355)+NT(356)));
//G578: get_cmd(353)         => __E_cli_command_171(354).
	p(NT(353), (NT(354)));
//G579: cli_command(276)     => get_cmd(353).
	p(NT(276), (NT(353)));
//G580: set_sym(359)         => 's' 'e' 't'.
	p(NT(359), (T(15)+T(11)+T(8)));
//G581: __E___E_cli_command_173_174(360) => __(10).
	p(NT(360), (NT(10)));
//G582: __E___E_cli_command_173_174(360) => _(9) '=' _(9).
	p(NT(360), (NT(9)+T(3)+NT(9)));
//G583: __E_cli_command_173(358) => set_sym(359) __(10) option_name(23) __E___E_cli_command_173_174(360) option_value(24).
	p(NT(358), (NT(359)+NT(10)+NT(23)+NT(360)+NT(24)));
//G584: set_cmd(357)         => __E_cli_command_173(358).
	p(NT(357), (NT(358)));
//G585: cli_command(276)     => set_cmd(357).
	p(NT(276), (NT(357)));
//G586: enable_sym(363)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(363), (T(11)+T(5)+T(24)+T(25)+T(10)+T(11)));
//G587: __E_cli_command_175(362) => enable_sym(363) __(10) option_name(23).
	p(NT(362), (NT(363)+NT(10)+NT(23)));
//G588: enable_cmd(361)      => __E_cli_command_175(362).
	p(NT(361), (NT(362)));
//G589: cli_command(276)     => enable_cmd(361).
	p(NT(276), (NT(361)));
//G590: disable_sym(366)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(366), (T(56)+T(4)+T(15)+T(24)+T(25)+T(10)+T(11)));
//G591: __E_cli_command_176(365) => disable_sym(366) __(10) option_name(23).
	p(NT(365), (NT(366)+NT(10)+NT(23)));
//G592: disable_cmd(364)     => __E_cli_command_176(365).
	p(NT(364), (NT(365)));
//G593: cli_command(276)     => disable_cmd(364).
	p(NT(276), (NT(364)));
//G594: toggle_sym(369)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(369), (T(8)+T(6)+T(57)+T(57)+T(10)+T(11)));
//G595: __E_cli_command_177(368) => toggle_sym(369) __(10) option_name(23).
	p(NT(368), (NT(369)+NT(10)+NT(23)));
//G596: toggle_cmd(367)      => __E_cli_command_177(368).
	p(NT(367), (NT(368)));
//G597: cli_command(276)     => toggle_cmd(367).
	p(NT(276), (NT(367)));
//G598: __E___E_cli_command_178_179(373) => 'd' 'e' 'f' 's'.
	p(NT(373), (T(56)+T(11)+T(9)+T(15)));
//G599: __E___E_cli_command_178_179(373) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(373), (T(56)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G600: def_sym(372)         => __E___E_cli_command_178_179(373).
	p(NT(372), (NT(373)));
//G601: __E_cli_command_178(371) => def_sym(372).
	p(NT(371), (NT(372)));
//G602: def_list_cmd(370)    => __E_cli_command_178(371).
	p(NT(370), (NT(371)));
//G603: cli_command(276)     => def_list_cmd(370).
	p(NT(276), (NT(370)));
//G604: __E_cli_command_180(375) => def_sym(372) __(10) num(175).
	p(NT(375), (NT(372)+NT(10)+NT(175)));
//G605: def_print_cmd(374)   => __E_cli_command_180(375).
	p(NT(374), (NT(375)));
//G606: cli_command(276)     => def_print_cmd(374).
	p(NT(276), (NT(374)));
//G607: def_rr_cmd(376)      => rec_relation(56).
	p(NT(376), (NT(56)));
//G608: cli_command(276)     => def_rr_cmd(376).
	p(NT(276), (NT(376)));
//G609: def_input_cmd(377)   => input_def(57).
	p(NT(377), (NT(57)));
//G610: cli_command(276)     => def_input_cmd(377).
	p(NT(276), (NT(377)));
//G611: def_output_cmd(378)  => output_def(58).
	p(NT(378), (NT(58)));
//G612: cli_command(276)     => def_output_cmd(378).
	p(NT(276), (NT(378)));
//G613: def_type_cmd(379)    => type_def(59).
	p(NT(379), (NT(59)));
//G614: cli_command(276)     => def_type_cmd(379).
	p(NT(276), (NT(379)));
//G615: __E___E_cli_command_181_182(383) => 'h' 'i' 's' 't'.
	p(NT(383), (T(55)+T(4)+T(15)+T(8)));
//G616: __E___E_cli_command_181_182(383) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(383), (T(55)+T(4)+T(15)+T(8)+T(6)+T(27)+T(17)));
//G617: history_sym(382)     => __E___E_cli_command_181_182(383).
	p(NT(382), (NT(383)));
//G618: __E_cli_command_181(381) => history_sym(382).
	p(NT(381), (NT(382)));
//G619: history_list_cmd(380) => __E_cli_command_181(381).
	p(NT(380), (NT(381)));
//G620: cli_command(276)     => history_list_cmd(380).
	p(NT(276), (NT(380)));
//G621: __E_cli_command_183(385) => history_sym(382) __(10) history(386).
	p(NT(385), (NT(382)+NT(10)+NT(386)));
//G622: history_print_cmd(384) => __E_cli_command_183(385).
	p(NT(384), (NT(385)));
//G623: cli_command(276)     => history_print_cmd(384).
	p(NT(276), (NT(384)));
//G624: __E_cli_command_184(388) => ref(65).
	p(NT(388), (NT(65)));
//G625: __E_cli_command_184(388) => wff(38).
	p(NT(388), (NT(38)));
//G626: __E_cli_command_184(388) => bf(40).
	p(NT(388), (NT(40)));
//G627: history_store_cmd(387) => __E_cli_command_184(388).
	p(NT(387), (NT(388)));
//G628: cli_command(276)     => history_store_cmd(387).
	p(NT(276), (NT(387)));
//G629: __E_solve_options_185(389) => __(10) solve_option(390).
	p(NT(389), (NT(10)+NT(390)));
//G630: __E_solve_options_186(391) => null.
	p(NT(391), (nul));
//G631: __E_solve_options_186(391) => __E_solve_options_186(391) __E_solve_options_185(389).
	p(NT(391), (NT(391)+NT(389)));
//G632: solve_options(312)   => __E_solve_options_186(391).
	p(NT(312), (NT(391)));
//G633: __E_solve_option_187(392) => solver_mode(393).
	p(NT(392), (NT(393)));
//G634: __E_solve_option_187(392) => type(13).
	p(NT(392), (NT(13)));
//G635: solve_option(390)    => '-' '-' __E_solve_option_187(392).
	p(NT(390), (T(34)+T(34)+NT(392)));
//G636: __E_solver_mode_188(395) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(395), (T(28)+T(4)+T(5)+T(4)+T(28)+T(7)+T(28)));
//G637: __E_solver_mode_188(395) => 'm' 'i' 'n'.
	p(NT(395), (T(28)+T(4)+T(5)));
//G638: solver_mode_minimum(394) => __E_solver_mode_188(395).
	p(NT(394), (NT(395)));
//G639: solver_mode(393)     => solver_mode_minimum(394).
	p(NT(393), (NT(394)));
//G640: __E_solver_mode_189(397) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(397), (T(28)+T(24)+T(33)+T(4)+T(28)+T(7)+T(28)));
//G641: __E_solver_mode_189(397) => 'm' 'a' 'x'.
	p(NT(397), (T(28)+T(24)+T(33)));
//G642: solver_mode_maximum(396) => __E_solver_mode_189(397).
	p(NT(396), (NT(397)));
//G643: solver_mode(393)     => solver_mode_maximum(396).
	p(NT(393), (NT(396)));
//G644: q_file_name(74)      => '"' file_name(21) '"'.
	p(NT(74), (T(59)+NT(21)+T(59)));
//G645: __E_file_name_190(398) => printable(8).
	p(NT(398), (NT(8)));
//G646: __E_file_name_190(398) => __E_file_name_190(398) printable(8).
	p(NT(398), (NT(398)+NT(8)));
//G647: file_name(21)        => __E_file_name_190(398).
	p(NT(21), (NT(398)));
//G648: __E_option_name_191(399) => alnum(6).
	p(NT(399), (NT(6)));
//G649: __E_option_name_191(399) => __E_option_name_191(399) alnum(6).
	p(NT(399), (NT(399)+NT(6)));
//G650: option_name(23)      => __E_option_name_191(399).
	p(NT(23), (NT(399)));
//G651: __E_option_value_192(400) => alnum(6).
	p(NT(400), (NT(6)));
//G652: __E_option_value_192(400) => '.'.
	p(NT(400), (T(1)));
//G653: __E_option_value_193(401) => __E_option_value_192(400).
	p(NT(401), (NT(400)));
//G654: __E_option_value_193(401) => __E_option_value_193(401) __E_option_value_192(400).
	p(NT(401), (NT(401)+NT(400)));
//G655: option_value(24)     => __E_option_value_193(401).
	p(NT(24), (NT(401)));
//G656: bf_cmd_arg(33)       => history(386).
	p(NT(33), (NT(386)));
//G657: bf_cmd_arg(33)       => bf(40).
	p(NT(33), (NT(40)));
//G658: wff_cmd_arg(34)      => history(386).
	p(NT(34), (NT(386)));
//G659: wff_cmd_arg(34)      => wff(38).
	p(NT(34), (NT(38)));
//G660: nf_cmd_arg(35)       => history(386).
	p(NT(35), (NT(386)));
//G661: __N_33(464)          => bf_min(120).
	p(NT(464), (NT(120)));
//G662: __N_34(465)          => bf_max(121).
	p(NT(465), (NT(121)));
//G663: __E_nf_cmd_arg_194(402) => ref(65) & ~( __N_33(464) ) & ~( __N_34(465) ).	 # conjunctive
	p(NT(402), (NT(65)) & ~(NT(464)) & ~(NT(465)));
//G664: nf_cmd_arg(35)       => __E_nf_cmd_arg_194(402).
	p(NT(35), (NT(402)));
//G665: nf_cmd_arg(35)       => wff(38).
	p(NT(35), (NT(38)));
//G666: nf_cmd_arg(35)       => bf(40).
	p(NT(35), (NT(40)));
//G667: onf_cmd_arg(36)      => history(386).
	p(NT(36), (NT(386)));
//G668: onf_cmd_arg(36)      => wff(38).
	p(NT(36), (NT(38)));
//G669: spec_arg(37)         => history(386).
	p(NT(37), (NT(386)));
//G670: spec_arg(37)         => spec(49).
	p(NT(37), (NT(49)));
//G671: __N_35(466)          => bf_min(120).
	p(NT(466), (NT(120)));
//G672: __N_36(467)          => bf_max(121).
	p(NT(467), (NT(121)));
//G673: __E_spec_arg_195(403) => ref(65) & ~( __N_35(466) ) & ~( __N_36(467) ).	 # conjunctive
	p(NT(403), (NT(65)) & ~(NT(466)) & ~(NT(467)));
//G674: spec_arg(37)         => __E_spec_arg_195(403).
	p(NT(37), (NT(403)));
//G675: spec_arg(37)         => wff(38).
	p(NT(37), (NT(38)));
//G676: spec_arg(37)         => bf(40).
	p(NT(37), (NT(40)));
//G677: __E_subst_group_196(404) => _(9) ',' _(9) nf_cmd_arg(35) _(9) '/' _(9) nf_cmd_arg(35).
	p(NT(404), (NT(9)+T(19)+NT(9)+NT(35)+NT(9)+T(16)+NT(9)+NT(35)));
//G678: __E_subst_group_197(405) => null.
	p(NT(405), (nul));
//G679: __E_subst_group_197(405) => __E_subst_group_197(405) __E_subst_group_196(404).
	p(NT(405), (NT(405)+NT(404)));
//G680: subst_group(329)     => '[' _(9) nf_cmd_arg(35) _(9) '/' _(9) nf_cmd_arg(35) __E_subst_group_197(405) _(9) ']'.
	p(NT(329), (T(20)+NT(9)+NT(35)+NT(9)+T(16)+NT(9)+NT(35)+NT(405)+NT(9)+T(21)));
//G681: __E_inst_args_198(406) => _(9) inst_group(407).
	p(NT(406), (NT(9)+NT(407)));
//G682: __E_inst_args_199(408) => __E_inst_args_198(406).
	p(NT(408), (NT(406)));
//G683: __E_inst_args_199(408) => __E_inst_args_199(408) __E_inst_args_198(406).
	p(NT(408), (NT(408)+NT(406)));
//G684: inst_args(32)        => wff_cmd_arg(34) __E_inst_args_199(408).
	p(NT(32), (NT(34)+NT(408)));
//G685: __E_inst_args_200(409) => _(9) inst_group(407).
	p(NT(409), (NT(9)+NT(407)));
//G686: __E_inst_args_201(410) => __E_inst_args_200(409).
	p(NT(410), (NT(409)));
//G687: __E_inst_args_201(410) => __E_inst_args_201(410) __E_inst_args_200(409).
	p(NT(410), (NT(410)+NT(409)));
//G688: inst_args(32)        => bf_cmd_arg(33) __E_inst_args_201(410).
	p(NT(32), (NT(33)+NT(410)));
//G689: __E_inst_group_202(411) => _(9) ',' _(9) bf(40) _(9) '/' _(9) bf_cmd_arg(33).
	p(NT(411), (NT(9)+T(19)+NT(9)+NT(40)+NT(9)+T(16)+NT(9)+NT(33)));
//G690: __E_inst_group_203(412) => null.
	p(NT(412), (nul));
//G691: __E_inst_group_203(412) => __E_inst_group_203(412) __E_inst_group_202(411).
	p(NT(412), (NT(412)+NT(411)));
//G692: inst_group(407)      => '[' _(9) bf(40) _(9) '/' _(9) bf_cmd_arg(33) __E_inst_group_203(412) _(9) ']'.
	p(NT(407), (T(20)+NT(9)+NT(40)+NT(9)+T(16)+NT(9)+NT(33)+NT(412)+NT(9)+T(21)));
//G693: help_arg(294)        => help_sym(291).
	p(NT(294), (NT(291)));
//G694: help_arg(294)        => version_sym(284).
	p(NT(294), (NT(284)));
//G695: help_arg(294)        => quit_sym(281).
	p(NT(294), (NT(281)));
//G696: help_arg(294)        => clear_sym(287).
	p(NT(294), (NT(287)));
//G697: help_arg(294)        => get_sym(355).
	p(NT(294), (NT(355)));
//G698: help_arg(294)        => set_sym(359).
	p(NT(294), (NT(359)));
//G699: help_arg(294)        => enable_sym(363).
	p(NT(294), (NT(363)));
//G700: help_arg(294)        => disable_sym(366).
	p(NT(294), (NT(366)));
//G701: help_arg(294)        => toggle_sym(369).
	p(NT(294), (NT(369)));
//G702: help_arg(294)        => file_sym(297).
	p(NT(294), (NT(297)));
//G703: help_arg(294)        => history_sym(382).
	p(NT(294), (NT(382)));
//G704: help_arg(294)        => abs_history_sym(413).
	p(NT(294), (NT(413)));
//G705: help_arg(294)        => rel_history_sym(414).
	p(NT(294), (NT(414)));
//G706: help_arg(294)        => selection_sym(415).
	p(NT(294), (NT(415)));
//G707: help_arg(294)        => def_sym(372).
	p(NT(294), (NT(372)));
//G708: help_arg(294)        => inst_sym(333).
	p(NT(294), (NT(333)));
//G709: help_arg(294)        => subst_sym(326).
	p(NT(294), (NT(326)));
//G710: help_arg(294)        => normalize_sym(322).
	p(NT(294), (NT(322)));
//G711: help_arg(294)        => execute_sym(416).
	p(NT(294), (NT(416)));
//G712: help_arg(294)        => solve_sym(310).
	p(NT(294), (NT(310)));
//G713: help_arg(294)        => valid_sym(301).
	p(NT(294), (NT(301)));
//G714: help_arg(294)        => sat_sym(304).
	p(NT(294), (NT(304)));
//G715: help_arg(294)        => unsat_sym(307).
	p(NT(294), (NT(307)));
//G716: help_arg(294)        => run_sym(318).
	p(NT(294), (NT(318)));
//G717: help_arg(294)        => dnf_sym(337).
	p(NT(294), (NT(337)));
//G718: help_arg(294)        => cnf_sym(340).
	p(NT(294), (NT(340)));
//G719: help_arg(294)        => snf_sym(417).
	p(NT(294), (NT(417)));
//G720: help_arg(294)        => nnf_sym(343).
	p(NT(294), (NT(343)));
//G721: help_arg(294)        => mnf_sym(346).
	p(NT(294), (NT(346)));
//G722: help_arg(294)        => onf_sym(349).
	p(NT(294), (NT(349)));
//G723: help_arg(294)        => qelim_sym(352).
	p(NT(294), (NT(352)));
//G724: __E___E_help_arg_204_205(420) => 's'.
	p(NT(420), (T(15)));
//G725: __E___E_help_arg_204_205(420) => null.
	p(NT(420), (nul));
//G726: __E_help_arg_204(419) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_204_205(420).
	p(NT(419), (T(11)+T(33)+T(24)+T(28)+T(18)+T(10)+T(11)+NT(420)));
//G727: examples_sym(418)    => __E_help_arg_204(419).
	p(NT(418), (NT(419)));
//G728: help_arg(294)        => examples_sym(418).
	p(NT(294), (NT(418)));
//G729: __E_history_206(422) => '%'.
	p(NT(422), (T(43)));
//G730: rel_history_sym(414) => '%' '-'.
	p(NT(414), (T(43)+T(34)));
//G731: history_id(424)      => digits(12).
	p(NT(424), (NT(12)));
//G732: __E___E_history_206_207(423) => history_id(424).
	p(NT(423), (NT(424)));
//G733: __E___E_history_206_207(423) => null.
	p(NT(423), (nul));
//G734: __E_history_206(422) => rel_history_sym(414) __E___E_history_206_207(423).
	p(NT(422), (NT(414)+NT(423)));
//G735: rel_history(421)     => __E_history_206(422).
	p(NT(421), (NT(422)));
//G736: history(386)         => rel_history(421).
	p(NT(386), (NT(421)));
//G737: abs_history_sym(413) => '%'.
	p(NT(413), (T(43)));
//G738: __E_history_208(426) => abs_history_sym(413) history_id(424).
	p(NT(426), (NT(413)+NT(424)));
//G739: abs_history(425)     => __E_history_208(426).
	p(NT(425), (NT(426)));
//G740: history(386)         => abs_history(425).
	p(NT(386), (NT(425)));
//G741: unreachable(427)     => BDD_ID(428).
	p(NT(427), (NT(428)));
//G742: stream_def(429)      => input_def(57).
	p(NT(429), (NT(57)));
//G743: stream_def(429)      => output_def(58).
	p(NT(429), (NT(58)));
//G744: fm_or_term(430)      => wff(38).
	p(NT(430), (NT(38)));
//G745: fm_or_term(430)      => bf(40).
	p(NT(430), (NT(40)));
	#undef T
	#undef NT
	return loaded = true, p;
}
#endif

} // namespace tau_parser_data
