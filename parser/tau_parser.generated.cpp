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
//G13:  __E_definitions_4(60) => __E_definitions_2(54) __E_definitions_4(60).
	p(NT(60), (NT(54)+NT(60)));
//G14:  definitions(51)      => __E_definitions_4(60).
	p(NT(51), (NT(60)));
//G15:  __E_spec_multiline_5(62) => _(9) spec_part(31) _(9) '.'.
	p(NT(62), (NT(9)+NT(31)+NT(9)+T(1)));
//G16:  __E_spec_multiline_6(63) => null.
	p(NT(63), (nul));
//G17:  __E_spec_multiline_6(63) => __E_spec_multiline_5(62) __E_spec_multiline_6(63).
	p(NT(63), (NT(62)+NT(63)));
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
//G55:  __E_type_parents_17(82) => __E_type_parents_16(81) __E_type_parents_17(82).
	p(NT(82), (NT(81)+NT(82)));
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
//G65:  __E_tuple_21(88)     => __E_tuple_20(87) __E_tuple_21(88).
	p(NT(88), (NT(87)+NT(88)));
//G66:  tuple(83)            => '{' _(9) member(85) __E_tuple_21(88) _(9) '}'.
	p(NT(83), (T(22)+NT(9)+NT(85)+NT(88)+NT(9)+T(23)));
//G67:  member_name(26)      => chars(27).
	p(NT(26), (NT(27)));
//G68:  __E_member_path_22(89) => '.' member_name(26).
	p(NT(89), (T(1)+NT(26)));
//G69:  __E_member_path_23(90) => null.
	p(NT(90), (nul));
//G70:  __E_member_path_23(90) => __E_member_path_22(89) __E_member_path_23(90).
	p(NT(90), (NT(89)+NT(90)));
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
//G91:  __E___E_ref_args_27_30(103) => __E___E_ref_args_27_29(102) __E___E_ref_args_27_30(103).
	p(NT(103), (NT(102)+NT(103)));
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
//G105: __E___E_wff_34_35(110) => 'G'.
	p(NT(110), (T(32)));
//G106: __E_wff_34(109)      => __E___E_wff_34_35(110) _(9) wff(38).
	p(NT(109), (NT(110)+NT(9)+NT(38)));
//G107: wff_always(108)      => __E_wff_34(109).
	p(NT(108), (NT(109)));
//G108: wff(38)              => wff_always(108).
	p(NT(38), (NT(108)));
//G109: __E_wff_36(112)      => 'F' _(9) wff(38).
	p(NT(112), (T(33)+NT(9)+NT(38)));
//G110: wff_F(111)           => __E_wff_36(112).
	p(NT(111), (NT(112)));
//G111: wff(38)              => wff_F(111).
	p(NT(38), (NT(111)));
//G112: __E_wff_37(114)      => 'A' _(9) wff(38).
	p(NT(114), (T(34)+NT(9)+NT(38)));
//G113: wff_A(113)           => __E_wff_37(114).
	p(NT(113), (NT(114)));
//G114: wff(38)              => wff_A(113).
	p(NT(38), (NT(113)));
//G115: __E_wff_38(116)      => 'E' _(9) wff(38).
	p(NT(116), (T(35)+NT(9)+NT(38)));
//G116: wff_E(115)           => __E_wff_38(116).
	p(NT(115), (NT(116)));
//G117: wff(38)              => wff_E(115).
	p(NT(38), (NT(115)));
//G118: __E_wff_39(118)      => '-' _(9) wff(38).
	p(NT(118), (T(36)+NT(9)+NT(38)));
//G119: wff_semantic_neg(117) => __E_wff_39(118).
	p(NT(117), (NT(118)));
//G120: wff(38)              => wff_semantic_neg(117).
	p(NT(38), (NT(117)));
//G121: __E_wff_40(120)      => wff(38) _(9) '?' _(9) wff(38) _(9) ':' _(9) wff(38).
	p(NT(120), (NT(38)+NT(9)+T(37)+NT(9)+NT(38)+NT(9)+T(2)+NT(9)+NT(38)));
//G122: wff_conditional(119) => __E_wff_40(120).
	p(NT(119), (NT(120)));
//G123: wff(38)              => wff_conditional(119).
	p(NT(38), (NT(119)));
//G124: __E_wff_41(122)      => 'a' 'l' 'l' __(10) q_vars(123) __(10) wff(38).
	p(NT(122), (T(24)+T(10)+T(10)+NT(10)+NT(123)+NT(10)+NT(38)));
//G125: wff_all(121)         => __E_wff_41(122).
	p(NT(121), (NT(122)));
//G126: wff(38)              => wff_all(121).
	p(NT(38), (NT(121)));
//G127: __E_wff_42(125)      => 'e' 'x' __(10) q_vars(123) __(10) wff(38).
	p(NT(125), (T(11)+T(38)+NT(10)+NT(123)+NT(10)+NT(38)));
//G128: wff_ex(124)          => __E_wff_42(125).
	p(NT(124), (NT(125)));
//G129: wff(38)              => wff_ex(124).
	p(NT(38), (NT(124)));
//G130: wff_ref(126)         => ref(65).
	p(NT(126), (NT(65)));
//G131: wff(38)              => wff_ref(126).
	p(NT(38), (NT(126)));
//G132: wff(38)              => constraint(127).
	p(NT(38), (NT(127)));
//G133: __E_wff_43(129)      => wff(38) _(9) '-' '>' _(9) wff(38).
	p(NT(129), (NT(38)+NT(9)+T(36)+T(30)+NT(9)+NT(38)));
//G134: wff_imply(128)       => __E_wff_43(129).
	p(NT(128), (NT(129)));
//G135: wff(38)              => wff_imply(128).
	p(NT(38), (NT(128)));
//G136: __E_wff_44(131)      => wff(38) _(9) '<' '-' _(9) wff(38).
	p(NT(131), (NT(38)+NT(9)+T(29)+T(36)+NT(9)+NT(38)));
//G137: wff_rimply(130)      => __E_wff_44(131).
	p(NT(130), (NT(131)));
//G138: wff(38)              => wff_rimply(130).
	p(NT(38), (NT(130)));
//G139: __E_wff_45(133)      => wff(38) _(9) '<' '-' '>' _(9) wff(38).
	p(NT(133), (NT(38)+NT(9)+T(29)+T(36)+T(30)+NT(9)+NT(38)));
//G140: wff_equiv(132)       => __E_wff_45(133).
	p(NT(132), (NT(133)));
//G141: wff(38)              => wff_equiv(132).
	p(NT(38), (NT(132)));
//G142: __E_wff_46(135)      => wff(38) _(9) '|' '|' _(9) wff(38).
	p(NT(135), (NT(38)+NT(9)+T(39)+T(39)+NT(9)+NT(38)));
//G143: wff_or(134)          => __E_wff_46(135).
	p(NT(134), (NT(135)));
//G144: wff(38)              => wff_or(134).
	p(NT(38), (NT(134)));
//G145: __E_wff_47(137)      => wff(38) _(9) '^' '^' _(9) wff(38).
	p(NT(137), (NT(38)+NT(9)+T(40)+T(40)+NT(9)+NT(38)));
//G146: wff_xor(136)         => __E_wff_47(137).
	p(NT(136), (NT(137)));
//G147: wff(38)              => wff_xor(136).
	p(NT(38), (NT(136)));
//G148: __E_wff_48(139)      => wff(38) _(9) '&' '&' _(9) wff(38).
	p(NT(139), (NT(38)+NT(9)+T(41)+T(41)+NT(9)+NT(38)));
//G149: wff_and(138)         => __E_wff_48(139).
	p(NT(138), (NT(139)));
//G150: wff(38)              => wff_and(138).
	p(NT(38), (NT(138)));
//G151: __E_wff_49(141)      => '!' _(9) wff(38).
	p(NT(141), (T(42)+NT(9)+NT(38)));
//G152: wff_neg(140)         => __E_wff_49(141).
	p(NT(140), (NT(141)));
//G153: wff(38)              => wff_neg(140).
	p(NT(38), (NT(140)));
//G154: wff_t(142)           => 'T'.
	p(NT(142), (T(43)));
//G155: wff(38)              => wff_t(142).
	p(NT(38), (NT(142)));
//G156: wff_f(143)           => 'F'.
	p(NT(143), (T(33)));
//G157: wff(38)              => wff_f(143).
	p(NT(38), (NT(143)));
//G158: __E_wff_50(145)      => bf(40) _(9) '<' '=' _(9) bf(40) _(9) '<' '=' _(9) bf(40).
	p(NT(145), (NT(40)+NT(9)+T(29)+T(3)+NT(9)+NT(40)+NT(9)+T(29)+T(3)+NT(9)+NT(40)));
//G159: bf_interval(144)     => __E_wff_50(145).
	p(NT(144), (NT(145)));
//G160: wff(38)              => bf_interval(144).
	p(NT(38), (NT(144)));
//G161: __E_wff_51(147)      => bf(40) _(9) '=' _(9) bf(40).
	p(NT(147), (NT(40)+NT(9)+T(3)+NT(9)+NT(40)));
//G162: bf_eq(146)           => __E_wff_51(147).
	p(NT(146), (NT(147)));
//G163: wff(38)              => bf_eq(146).
	p(NT(38), (NT(146)));
//G164: __E_wff_52(149)      => bf(40) _(9) '!' '=' _(9) bf(40).
	p(NT(149), (NT(40)+NT(9)+T(42)+T(3)+NT(9)+NT(40)));
//G165: bf_neq(148)          => __E_wff_52(149).
	p(NT(148), (NT(149)));
//G166: wff(38)              => bf_neq(148).
	p(NT(38), (NT(148)));
//G167: __E_wff_53(151)      => bf(40) _(9) '<' _(9) bf(40).
	p(NT(151), (NT(40)+NT(9)+T(29)+NT(9)+NT(40)));
//G168: bf_lt(150)           => __E_wff_53(151).
	p(NT(150), (NT(151)));
//G169: wff(38)              => bf_lt(150).
	p(NT(38), (NT(150)));
//G170: __E_wff_54(153)      => bf(40) _(9) '!' '<' _(9) bf(40).
	p(NT(153), (NT(40)+NT(9)+T(42)+T(29)+NT(9)+NT(40)));
//G171: bf_nlt(152)          => __E_wff_54(153).
	p(NT(152), (NT(153)));
//G172: wff(38)              => bf_nlt(152).
	p(NT(38), (NT(152)));
//G173: __E_wff_55(155)      => bf(40) _(9) '<' '=' _(9) bf(40).
	p(NT(155), (NT(40)+NT(9)+T(29)+T(3)+NT(9)+NT(40)));
//G174: bf_lteq(154)         => __E_wff_55(155).
	p(NT(154), (NT(155)));
//G175: wff(38)              => bf_lteq(154).
	p(NT(38), (NT(154)));
//G176: __E_wff_56(157)      => bf(40) _(9) '!' '<' '=' _(9) bf(40).
	p(NT(157), (NT(40)+NT(9)+T(42)+T(29)+T(3)+NT(9)+NT(40)));
//G177: bf_nlteq(156)        => __E_wff_56(157).
	p(NT(156), (NT(157)));
//G178: wff(38)              => bf_nlteq(156).
	p(NT(38), (NT(156)));
//G179: __E_wff_57(159)      => bf(40) _(9) '>' _(9) bf(40).
	p(NT(159), (NT(40)+NT(9)+T(30)+NT(9)+NT(40)));
//G180: bf_gt(158)           => __E_wff_57(159).
	p(NT(158), (NT(159)));
//G181: wff(38)              => bf_gt(158).
	p(NT(38), (NT(158)));
//G182: __E_wff_58(161)      => bf(40) _(9) '!' '>' _(9) bf(40).
	p(NT(161), (NT(40)+NT(9)+T(42)+T(30)+NT(9)+NT(40)));
//G183: bf_ngt(160)          => __E_wff_58(161).
	p(NT(160), (NT(161)));
//G184: wff(38)              => bf_ngt(160).
	p(NT(38), (NT(160)));
//G185: __E_wff_59(163)      => bf(40) _(9) '>' '=' _(9) bf(40).
	p(NT(163), (NT(40)+NT(9)+T(30)+T(3)+NT(9)+NT(40)));
//G186: bf_gteq(162)         => __E_wff_59(163).
	p(NT(162), (NT(163)));
//G187: wff(38)              => bf_gteq(162).
	p(NT(38), (NT(162)));
//G188: __E_wff_60(165)      => bf(40) _(9) '!' '>' '=' _(9) bf(40).
	p(NT(165), (NT(40)+NT(9)+T(42)+T(30)+T(3)+NT(9)+NT(40)));
//G189: bf_ngteq(164)        => __E_wff_60(165).
	p(NT(164), (NT(165)));
//G190: wff(38)              => bf_ngteq(164).
	p(NT(38), (NT(164)));
//G191: __E_wff_61(167)      => wff(38) _(9) 'U' _(9) wff(38).
	p(NT(167), (NT(38)+NT(9)+T(44)+NT(9)+NT(38)));
//G192: wff_U(166)           => __E_wff_61(167).
	p(NT(166), (NT(167)));
//G193: wff(38)              => wff_U(166).
	p(NT(38), (NT(166)));
//G194: __E_wff_62(169)      => wff(38) _(9) 'R' _(9) wff(38).
	p(NT(169), (NT(38)+NT(9)+T(45)+NT(9)+NT(38)));
//G195: wff_R(168)           => __E_wff_62(169).
	p(NT(168), (NT(169)));
//G196: wff(38)              => wff_R(168).
	p(NT(38), (NT(168)));
//G197: __E_wff_63(171)      => wff(38) _(9) 'W' _(9) wff(38).
	p(NT(171), (NT(38)+NT(9)+T(46)+NT(9)+NT(38)));
//G198: wff_W(170)           => __E_wff_63(171).
	p(NT(170), (NT(171)));
//G199: wff(38)              => wff_W(170).
	p(NT(38), (NT(170)));
//G200: __E_wff_64(173)      => wff(38) _(9) 'S' _(9) wff(38).
	p(NT(173), (NT(38)+NT(9)+T(47)+NT(9)+NT(38)));
//G201: wff_S(172)           => __E_wff_64(173).
	p(NT(172), (NT(173)));
//G202: wff(38)              => wff_S(172).
	p(NT(38), (NT(172)));
//G203: __E_wff_65(175)      => wff(38) _(9) 'T' _(9) wff(38).
	p(NT(175), (NT(38)+NT(9)+T(43)+NT(9)+NT(38)));
//G204: wff_T(174)           => __E_wff_65(175).
	p(NT(174), (NT(175)));
//G205: wff(38)              => wff_T(174).
	p(NT(38), (NT(174)));
//G206: wff(38)              => capture(17).
	p(NT(38), (NT(17)));
//G207: __N_0(473)           => bv_type_like(178).
	p(NT(473), (NT(178)));
//G208: __E___E_bf_66_67(177) => bf(40) & ~( __N_0(473) ).	 # conjunctive
	p(NT(177), (NT(40)) & ~(NT(473)));
//G209: __E_bf_66(176)       => '(' _(9) __E___E_bf_66_67(177) _(9) ')'.
	p(NT(176), (T(12)+NT(9)+NT(177)+NT(9)+T(13)));
//G210: bf_parenthesis(41)   => __E_bf_66(176).
	p(NT(41), (NT(176)));
//G211: bf(40)               => bf_parenthesis(41).
	p(NT(40), (NT(41)));
//G212: bf(40)               => ba_constant(179).
	p(NT(40), (NT(179)));
//G213: bf(40)               => variable(180).
	p(NT(40), (NT(180)));
//G214: __E_bf_68(182)       => 'f' 'a' 'l' 'l' __(10) q_vars(123) __(10) bf(40).
	p(NT(182), (T(9)+T(24)+T(10)+T(10)+NT(10)+NT(123)+NT(10)+NT(40)));
//G215: bf_fall(181)         => __E_bf_68(182).
	p(NT(181), (NT(182)));
//G216: bf(40)               => bf_fall(181).
	p(NT(40), (NT(181)));
//G217: __E_bf_69(184)       => 'f' 'e' 'x' __(10) q_vars(123) __(10) bf(40).
	p(NT(184), (T(9)+T(11)+T(38)+NT(10)+NT(123)+NT(10)+NT(40)));
//G218: bf_fex(183)          => __E_bf_69(184).
	p(NT(183), (NT(184)));
//G219: bf(40)               => bf_fex(183).
	p(NT(40), (NT(183)));
//G220: bf_ref(185)          => ref(65).
	p(NT(185), (NT(65)));
//G221: bf(40)               => bf_ref(185).
	p(NT(40), (NT(185)));
//G222: __E___E_bf_70_71(189) => bf_parenthesis(41).
	p(NT(189), (NT(41)));
//G223: __E___E_bf_70_71(189) => ba_constant(179).
	p(NT(189), (NT(179)));
//G224: __E___E_bf_70_71(189) => variable(180).
	p(NT(189), (NT(180)));
//G225: __E___E_bf_70_71(189) => bf_ref(185).
	p(NT(189), (NT(185)));
//G226: __E___E_bf_70_71(189) => bf_t(190).
	p(NT(189), (NT(190)));
//G227: __E___E_bf_70_71(189) => bf_f(191).
	p(NT(189), (NT(191)));
//G228: __E___E_bf_70_71(189) => bf_neg(192).
	p(NT(189), (NT(192)));
//G229: __E___E_bf_70_71(189) => bf_cast(186).
	p(NT(189), (NT(186)));
//G230: __E___E_bf_70_71(189) => capture(17).
	p(NT(189), (NT(17)));
//G231: __E___E_bf_70_71(189) => bf_fall(181).
	p(NT(189), (NT(181)));
//G232: __E___E_bf_70_71(189) => bf_fex(183).
	p(NT(189), (NT(183)));
//G233: bf_cast_oprnd(43)    => __E___E_bf_70_71(189).
	p(NT(43), (NT(189)));
//G234: __E_bf_70(187)       => '(' _(9) 'b' 'v' _(9) '[' _(9) num(188) _(9) ']' _(9) ')' _(9) bf_cast_oprnd(43).
	p(NT(187), (T(12)+NT(9)+T(25)+T(48)+NT(9)+T(20)+NT(9)+NT(188)+NT(9)+T(21)+NT(9)+T(13)+NT(9)+NT(43)));
//G235: bf_cast(186)         => __E_bf_70(187).
	p(NT(186), (NT(187)));
//G236: bf(40)               => bf_cast(186).
	p(NT(40), (NT(186)));
//G237: __E_bf_72(194)       => func_sym(195) '(' _(9) func_args(196) _(9) ')'.
	p(NT(194), (NT(195)+T(12)+NT(9)+NT(196)+NT(9)+T(13)));
//G238: bf_func_app(193)     => __E_bf_72(194).
	p(NT(193), (NT(194)));
//G239: bf(40)               => bf_func_app(193).
	p(NT(40), (NT(193)));
//G240: __N_1(474)           => bf_shr(197).
	p(NT(474), (NT(197)));
//G241: __N_2(475)           => bf_shl(200).
	p(NT(475), (NT(200)));
//G242: __E___E_bf_73_74(199) => bf(40) & ~( __N_1(474) ) & ~( __N_2(475) ).	 # conjunctive
	p(NT(199), (NT(40)) & ~(NT(474)) & ~(NT(475)));
//G243: __E_bf_73(198)       => bf(40) _(9) '>' '>' _(9) __E___E_bf_73_74(199).
	p(NT(198), (NT(40)+NT(9)+T(30)+T(30)+NT(9)+NT(199)));
//G244: bf_shr(197)          => __E_bf_73(198).
	p(NT(197), (NT(198)));
//G245: bf(40)               => bf_shr(197).
	p(NT(40), (NT(197)));
//G246: __N_3(476)           => bf_shr(197).
	p(NT(476), (NT(197)));
//G247: __N_4(477)           => bf_shl(200).
	p(NT(477), (NT(200)));
//G248: __E___E_bf_75_76(202) => bf(40) & ~( __N_3(476) ) & ~( __N_4(477) ).	 # conjunctive
	p(NT(202), (NT(40)) & ~(NT(476)) & ~(NT(477)));
//G249: __E_bf_75(201)       => bf(40) _(9) '<' '<' _(9) __E___E_bf_75_76(202).
	p(NT(201), (NT(40)+NT(9)+T(29)+T(29)+NT(9)+NT(202)));
//G250: bf_shl(200)          => __E_bf_75(201).
	p(NT(200), (NT(201)));
//G251: bf(40)               => bf_shl(200).
	p(NT(40), (NT(200)));
//G252: __N_5(478)           => bf_add(203).
	p(NT(478), (NT(203)));
//G253: __N_6(479)           => bf_sub(206).
	p(NT(479), (NT(206)));
//G254: __E___E_bf_77_78(205) => bf(40) & ~( __N_5(478) ) & ~( __N_6(479) ).	 # conjunctive
	p(NT(205), (NT(40)) & ~(NT(478)) & ~(NT(479)));
//G255: __E_bf_77(204)       => bf(40) _(9) '+' _(9) __E___E_bf_77_78(205).
	p(NT(204), (NT(40)+NT(9)+T(49)+NT(9)+NT(205)));
//G256: bf_add(203)          => __E_bf_77(204).
	p(NT(203), (NT(204)));
//G257: bf(40)               => bf_add(203).
	p(NT(40), (NT(203)));
//G258: __N_7(480)           => bf_add(203).
	p(NT(480), (NT(203)));
//G259: __N_8(481)           => bf_sub(206).
	p(NT(481), (NT(206)));
//G260: __E___E_bf_79_80(208) => bf(40) & ~( __N_7(480) ) & ~( __N_8(481) ).	 # conjunctive
	p(NT(208), (NT(40)) & ~(NT(480)) & ~(NT(481)));
//G261: __E_bf_79(207)       => bf(40) _(9) '-' _(9) __E___E_bf_79_80(208).
	p(NT(207), (NT(40)+NT(9)+T(36)+NT(9)+NT(208)));
//G262: bf_sub(206)          => __E_bf_79(207).
	p(NT(206), (NT(207)));
//G263: bf(40)               => bf_sub(206).
	p(NT(40), (NT(206)));
//G264: __N_9(482)           => bf_mod(209).
	p(NT(482), (NT(209)));
//G265: __N_10(483)          => bf_mul(212).
	p(NT(483), (NT(212)));
//G266: __N_11(484)          => bf_div(213).
	p(NT(484), (NT(213)));
//G267: __E___E_bf_81_82(211) => bf(40) & ~( __N_9(482) ) & ~( __N_10(483) ) & ~( __N_11(484) ).	 # conjunctive
	p(NT(211), (NT(40)) & ~(NT(482)) & ~(NT(483)) & ~(NT(484)));
//G268: __E_bf_81(210)       => bf(40) _(9) '%' _(9) __E___E_bf_81_82(211).
	p(NT(210), (NT(40)+NT(9)+T(50)+NT(9)+NT(211)));
//G269: bf_mod(209)          => __E_bf_81(210).
	p(NT(209), (NT(210)));
//G270: bf(40)               => bf_mod(209).
	p(NT(40), (NT(209)));
//G271: __N_12(485)          => bf_mod(209).
	p(NT(485), (NT(209)));
//G272: __N_13(486)          => bf_mul(212).
	p(NT(486), (NT(212)));
//G273: __N_14(487)          => bf_div(213).
	p(NT(487), (NT(213)));
//G274: __E___E_bf_83_84(215) => bf(40) & ~( __N_12(485) ) & ~( __N_13(486) ) & ~( __N_14(487) ).	 # conjunctive
	p(NT(215), (NT(40)) & ~(NT(485)) & ~(NT(486)) & ~(NT(487)));
//G275: __E_bf_83(214)       => bf(40) _(9) '*' _(9) __E___E_bf_83_84(215).
	p(NT(214), (NT(40)+NT(9)+T(51)+NT(9)+NT(215)));
//G276: bf_mul(212)          => __E_bf_83(214).
	p(NT(212), (NT(214)));
//G277: bf(40)               => bf_mul(212).
	p(NT(40), (NT(212)));
//G278: __N_15(488)          => bf_mod(209).
	p(NT(488), (NT(209)));
//G279: __N_16(489)          => bf_mul(212).
	p(NT(489), (NT(212)));
//G280: __N_17(490)          => bf_div(213).
	p(NT(490), (NT(213)));
//G281: __E___E_bf_85_86(217) => bf(40) & ~( __N_15(488) ) & ~( __N_16(489) ) & ~( __N_17(490) ).	 # conjunctive
	p(NT(217), (NT(40)) & ~(NT(488)) & ~(NT(489)) & ~(NT(490)));
//G282: __E_bf_85(216)       => bf(40) _(9) '/' _(9) __E___E_bf_85_86(217).
	p(NT(216), (NT(40)+NT(9)+T(16)+NT(9)+NT(217)));
//G283: bf_div(213)          => __E_bf_85(216).
	p(NT(213), (NT(216)));
//G284: bf(40)               => bf_div(213).
	p(NT(40), (NT(213)));
//G285: __N_18(491)          => bf_nor(218).
	p(NT(491), (NT(218)));
//G286: __E___E_bf_87_88(220) => bf(40) & ~( __N_18(491) ).	 # conjunctive
	p(NT(220), (NT(40)) & ~(NT(491)));
//G287: __E_bf_87(219)       => bf(40) _(9) '!' '|' _(9) __E___E_bf_87_88(220).
	p(NT(219), (NT(40)+NT(9)+T(42)+T(39)+NT(9)+NT(220)));
//G288: bf_nor(218)          => __E_bf_87(219).
	p(NT(218), (NT(219)));
//G289: bf(40)               => bf_nor(218).
	p(NT(40), (NT(218)));
//G290: __N_19(492)          => bf_xnor(221).
	p(NT(492), (NT(221)));
//G291: __E___E_bf_89_90(223) => bf(40) & ~( __N_19(492) ).	 # conjunctive
	p(NT(223), (NT(40)) & ~(NT(492)));
//G292: __E_bf_89(222)       => bf(40) _(9) '!' '^' _(9) __E___E_bf_89_90(223).
	p(NT(222), (NT(40)+NT(9)+T(42)+T(40)+NT(9)+NT(223)));
//G293: bf_xnor(221)         => __E_bf_89(222).
	p(NT(221), (NT(222)));
//G294: bf(40)               => bf_xnor(221).
	p(NT(40), (NT(221)));
//G295: __N_20(493)          => bf_nand(224).
	p(NT(493), (NT(224)));
//G296: __E___E_bf_91_92(226) => bf(40) & ~( __N_20(493) ).	 # conjunctive
	p(NT(226), (NT(40)) & ~(NT(493)));
//G297: __E_bf_91(225)       => bf(40) _(9) '!' '&' _(9) __E___E_bf_91_92(226).
	p(NT(225), (NT(40)+NT(9)+T(42)+T(41)+NT(9)+NT(226)));
//G298: bf_nand(224)         => __E_bf_91(225).
	p(NT(224), (NT(225)));
//G299: bf(40)               => bf_nand(224).
	p(NT(40), (NT(224)));
//G300: __E_bf_93(228)       => bf(40) _(9) '|' _(9) bf(40).
	p(NT(228), (NT(40)+NT(9)+T(39)+NT(9)+NT(40)));
//G301: bf_or(227)           => __E_bf_93(228).
	p(NT(227), (NT(228)));
//G302: bf(40)               => bf_or(227).
	p(NT(40), (NT(227)));
//G303: __E_bf_94(230)       => bf(40) _(9) '^' _(9) bf(40).
	p(NT(230), (NT(40)+NT(9)+T(40)+NT(9)+NT(40)));
//G304: bf_xor(229)          => __E_bf_94(230).
	p(NT(229), (NT(230)));
//G305: bf(40)               => bf_xor(229).
	p(NT(40), (NT(229)));
//G306: __E___E_bf_95_96(232) => typed(70).
	p(NT(232), (NT(70)));
//G307: __E___E_bf_95_96(232) => null.
	p(NT(232), (nul));
//G308: __E_bf_95(231)       => '1' __E___E_bf_95_96(232).
	p(NT(231), (T(52)+NT(232)));
//G309: bf_t(190)            => __E_bf_95(231).
	p(NT(190), (NT(231)));
//G310: bf(40)               => bf_t(190).
	p(NT(40), (NT(190)));
//G311: __E___E_bf_97_98(234) => typed(70).
	p(NT(234), (NT(70)));
//G312: __E___E_bf_97_98(234) => null.
	p(NT(234), (nul));
//G313: __E_bf_97(233)       => '0' __E___E_bf_97_98(234).
	p(NT(233), (T(53)+NT(234)));
//G314: bf_f(191)            => __E_bf_97(233).
	p(NT(191), (NT(233)));
//G315: bf(40)               => bf_f(191).
	p(NT(40), (NT(191)));
//G316: __E___E_bf_99_100(236) => _(9) '&' _(9).
	p(NT(236), (NT(9)+T(41)+NT(9)));
//G317: __E___E_bf_99_100(236) => __(10).
	p(NT(236), (NT(10)));
//G318: __E_bf_99(235)       => bf(40) __E___E_bf_99_100(236) bf(40).
	p(NT(235), (NT(40)+NT(236)+NT(40)));
//G319: bf_and(47)           => __E_bf_99(235).
	p(NT(47), (NT(235)));
//G320: bf(40)               => bf_and(47).
	p(NT(40), (NT(47)));
//G321: __E___E_bf_101_102(238) => bf_parenthesis(41).
	p(NT(238), (NT(41)));
//G322: __E___E_bf_101_102(238) => ba_constant(179).
	p(NT(238), (NT(179)));
//G323: __E___E_bf_101_102(238) => variable(180).
	p(NT(238), (NT(180)));
//G324: __E___E_bf_101_102(238) => bf_ref(185).
	p(NT(238), (NT(185)));
//G325: __E___E_bf_101_102(238) => bf_t(190).
	p(NT(238), (NT(190)));
//G326: __E___E_bf_101_102(238) => bf_f(191).
	p(NT(238), (NT(191)));
//G327: __E___E_bf_101_102(238) => bf_neg(192).
	p(NT(238), (NT(192)));
//G328: __E___E_bf_101_102(238) => capture(17).
	p(NT(238), (NT(17)));
//G329: bf_neg_oprnd(42)     => __E___E_bf_101_102(238).
	p(NT(42), (NT(238)));
//G330: __E_bf_101(237)      => bf_neg_oprnd(42) _(9) '\''.
	p(NT(237), (NT(42)+NT(9)+T(54)));
//G331: bf_neg(192)          => __E_bf_101(237).
	p(NT(192), (NT(237)));
//G332: bf(40)               => bf_neg(192).
	p(NT(40), (NT(192)));
//G333: __E___E_bf_103_104(240) => bf_parenthesis(41).
	p(NT(240), (NT(41)));
//G334: __E___E_bf_103_104(240) => variable(180).
	p(NT(240), (NT(180)));
//G335: __E___E_bf_103_104(240) => bf_ref(185).
	p(NT(240), (NT(185)));
//G336: __E___E_bf_103_104(240) => bf_neg(192).
	p(NT(240), (NT(192)));
//G337: __E___E_bf_103_104(240) => bf_and_nosep(46).
	p(NT(240), (NT(46)));
//G338: __E___E_bf_103_104(240) => capture(17).
	p(NT(240), (NT(17)));
//G339: bf_and_nosep_1st_oprnd(44) => __E___E_bf_103_104(240).
	p(NT(44), (NT(240)));
//G340: __E___E_bf_103_105(241) => bf_parenthesis(41).
	p(NT(241), (NT(41)));
//G341: __E___E_bf_103_105(241) => ba_constant(179).
	p(NT(241), (NT(179)));
//G342: __E___E_bf_103_105(241) => variable(180).
	p(NT(241), (NT(180)));
//G343: __E___E_bf_103_105(241) => bf_ref(185).
	p(NT(241), (NT(185)));
//G344: __E___E_bf_103_105(241) => bf_neg(192).
	p(NT(241), (NT(192)));
//G345: __E___E_bf_103_105(241) => capture(17).
	p(NT(241), (NT(17)));
//G346: bf_and_nosep_2nd_oprnd(45) => __E___E_bf_103_105(241).
	p(NT(45), (NT(241)));
//G347: __E_bf_103(239)      => bf_and_nosep_1st_oprnd(44) bf_and_nosep_2nd_oprnd(45).
	p(NT(239), (NT(44)+NT(45)));
//G348: bf_and_nosep(46)     => __E_bf_103(239).
	p(NT(46), (NT(239)));
//G349: bf(40)               => bf_and_nosep(46).
	p(NT(40), (NT(46)));
//G350: bf(40)               => capture(17).
	p(NT(40), (NT(17)));
//G351: func_sym(195)        => chars(27).
	p(NT(195), (NT(27)));
//G352: __E_func_args_106(242) => _(9) ',' _(9) bf(40).
	p(NT(242), (NT(9)+T(19)+NT(9)+NT(40)));
//G353: __E_func_args_107(243) => null.
	p(NT(243), (nul));
//G354: __E_func_args_107(243) => __E_func_args_106(242) __E_func_args_107(243).
	p(NT(243), (NT(242)+NT(243)));
//G355: func_args(196)       => bf(40) __E_func_args_107(243).
	p(NT(196), (NT(40)+NT(243)));
//G356: func_dynamic(245)    => 'd' 'y' 'n' 'a' 'm' 'i' 'c'.
	p(NT(245), (T(55)+T(17)+T(5)+T(24)+T(28)+T(4)+T(14)));
//G357: func_mode(244)       => func_dynamic(245).
	p(NT(244), (NT(245)));
//G358: func_static(246)     => 's' 't' 'a' 't' 'i' 'c'.
	p(NT(246), (T(15)+T(8)+T(24)+T(8)+T(4)+T(14)));
//G359: func_mode(244)       => func_static(246).
	p(NT(244), (NT(246)));
//G360: __E_func_sort_list_108(248) => _(9) ',' _(9) type(13).
	p(NT(248), (NT(9)+T(19)+NT(9)+NT(13)));
//G361: __E_func_sort_list_109(249) => null.
	p(NT(249), (nul));
//G362: __E_func_sort_list_109(249) => __E_func_sort_list_108(248) __E_func_sort_list_109(249).
	p(NT(249), (NT(248)+NT(249)));
//G363: func_sort_list(247)  => type(13) __E_func_sort_list_109(249).
	p(NT(247), (NT(13)+NT(249)));
//G364: func_decl(250)       => func_mode(244) __(10) func_sym(195) _(9) ':' _(9) func_sort_list(247) _(9) '-' '>' _(9) type(13).
	p(NT(250), (NT(244)+NT(10)+NT(195)+NT(9)+T(2)+NT(9)+NT(247)+NT(9)+T(36)+T(30)+NT(9)+NT(13)));
//G365: __E_constraint_110(252) => '[' ctnvar(22) _(9) '!' '=' _(9) num(188) ']'.
	p(NT(252), (T(20)+NT(22)+NT(9)+T(42)+T(3)+NT(9)+NT(188)+T(21)));
//G366: __E_constraint_110(252) => '[' num(188) _(9) '!' '=' _(9) ctnvar(22) ']'.
	p(NT(252), (T(20)+NT(188)+NT(9)+T(42)+T(3)+NT(9)+NT(22)+T(21)));
//G367: ctn_neq(251)         => __E_constraint_110(252).
	p(NT(251), (NT(252)));
//G368: constraint(127)      => ctn_neq(251).
	p(NT(127), (NT(251)));
//G369: __E_constraint_111(254) => '[' ctnvar(22) _(9) '=' _(9) num(188) ']'.
	p(NT(254), (T(20)+NT(22)+NT(9)+T(3)+NT(9)+NT(188)+T(21)));
//G370: __E_constraint_111(254) => '[' num(188) _(9) '=' _(9) ctnvar(22) ']'.
	p(NT(254), (T(20)+NT(188)+NT(9)+T(3)+NT(9)+NT(22)+T(21)));
//G371: ctn_eq(253)          => __E_constraint_111(254).
	p(NT(253), (NT(254)));
//G372: constraint(127)      => ctn_eq(253).
	p(NT(127), (NT(253)));
//G373: __E_constraint_112(256) => '[' ctnvar(22) _(9) '>' '=' _(9) num(188) ']'.
	p(NT(256), (T(20)+NT(22)+NT(9)+T(30)+T(3)+NT(9)+NT(188)+T(21)));
//G374: __E_constraint_112(256) => '[' num(188) _(9) '>' '=' _(9) ctnvar(22) ']'.
	p(NT(256), (T(20)+NT(188)+NT(9)+T(30)+T(3)+NT(9)+NT(22)+T(21)));
//G375: ctn_gteq(255)        => __E_constraint_112(256).
	p(NT(255), (NT(256)));
//G376: constraint(127)      => ctn_gteq(255).
	p(NT(127), (NT(255)));
//G377: __E_constraint_113(258) => '[' ctnvar(22) _(9) '>' _(9) num(188) ']'.
	p(NT(258), (T(20)+NT(22)+NT(9)+T(30)+NT(9)+NT(188)+T(21)));
//G378: __E_constraint_113(258) => '[' num(188) _(9) '>' _(9) ctnvar(22) ']'.
	p(NT(258), (T(20)+NT(188)+NT(9)+T(30)+NT(9)+NT(22)+T(21)));
//G379: ctn_gt(257)          => __E_constraint_113(258).
	p(NT(257), (NT(258)));
//G380: constraint(127)      => ctn_gt(257).
	p(NT(127), (NT(257)));
//G381: __E_constraint_114(260) => '[' ctnvar(22) _(9) '<' '=' _(9) num(188) ']'.
	p(NT(260), (T(20)+NT(22)+NT(9)+T(29)+T(3)+NT(9)+NT(188)+T(21)));
//G382: __E_constraint_114(260) => '[' num(188) _(9) '<' '=' _(9) ctnvar(22) ']'.
	p(NT(260), (T(20)+NT(188)+NT(9)+T(29)+T(3)+NT(9)+NT(22)+T(21)));
//G383: ctn_lteq(259)        => __E_constraint_114(260).
	p(NT(259), (NT(260)));
//G384: constraint(127)      => ctn_lteq(259).
	p(NT(127), (NT(259)));
//G385: __E_constraint_115(262) => '[' ctnvar(22) _(9) '<' _(9) num(188) ']'.
	p(NT(262), (T(20)+NT(22)+NT(9)+T(29)+NT(9)+NT(188)+T(21)));
//G386: __E_constraint_115(262) => '[' num(188) _(9) '<' _(9) ctnvar(22) ']'.
	p(NT(262), (T(20)+NT(188)+NT(9)+T(29)+NT(9)+NT(22)+T(21)));
//G387: ctn_lt(261)          => __E_constraint_115(262).
	p(NT(261), (NT(262)));
//G388: constraint(127)      => ctn_lt(261).
	p(NT(127), (NT(261)));
//G389: __E_ba_constant_116(263) => capture(17).
	p(NT(263), (NT(17)));
//G390: __E_ba_constant_116(263) => source(16).
	p(NT(263), (NT(16)));
//G391: __E_ba_constant_117(264) => typed(70).
	p(NT(264), (NT(70)));
//G392: __E_ba_constant_117(264) => null.
	p(NT(264), (nul));
//G393: ba_constant(179)     => '{' _(9) __E_ba_constant_116(263) _(9) '}' __E_ba_constant_117(264).
	p(NT(179), (T(22)+NT(9)+NT(263)+NT(9)+T(23)+NT(264)));
//G394: __E___E_source_118_119(266) => src_c(29).
	p(NT(266), (NT(29)));
//G395: __E___E_source_118_119(266) => space(2).
	p(NT(266), (NT(2)));
//G396: __E___E_source_118_120(267) => null.
	p(NT(267), (nul));
//G397: __E___E_source_118_120(267) => __E___E_source_118_119(266) __E___E_source_118_120(267).
	p(NT(267), (NT(266)+NT(267)));
//G398: __E_source_118(265)  => __E___E_source_118_120(267) src_c(29).
	p(NT(265), (NT(267)+NT(29)));
//G399: __E_source_118(265)  => null.
	p(NT(265), (nul));
//G400: source(16)           => src_c(29) __E_source_118(265).
	p(NT(16), (NT(29)+NT(265)));
//G401: src_c(29)            => alnum(6).
	p(NT(29), (NT(6)));
//G402: __N_21(494)          => '{'.
	p(NT(494), (T(22)));
//G403: __N_22(495)          => '}'.
	p(NT(495), (T(23)));
//G404: src_c(29)            => ~( __N_21(494) ) & ~( __N_22(495) ) & punct(7).	 # conjunctive
	p(NT(29), ~(NT(494)) & ~(NT(495)) & (NT(7)));
//G405: __E_src_c_121(268)   => src_c(29).
	p(NT(268), (NT(29)));
//G406: __E_src_c_121(268)   => space(2).
	p(NT(268), (NT(2)));
//G407: __E_src_c_122(269)   => null.
	p(NT(269), (nul));
//G408: __E_src_c_122(269)   => __E_src_c_121(268) __E_src_c_122(269).
	p(NT(269), (NT(268)+NT(269)));
//G409: src_c(29)            => '{' __E_src_c_122(269) '}'.
	p(NT(29), (T(22)+NT(269)+T(23)));
//G410: __E_variable_123(270) => uconst(30).
	p(NT(270), (NT(30)));
//G411: __E_variable_123(270) => io_var(271).
	p(NT(270), (NT(271)));
//G412: __E_variable_123(270) => var_name(18).
	p(NT(270), (NT(18)));
//G413: __E_variable_124(272) => member_path(68).
	p(NT(272), (NT(68)));
//G414: __E_variable_124(272) => null.
	p(NT(272), (nul));
//G415: __E_variable_125(273) => typed(70).
	p(NT(273), (NT(70)));
//G416: __E_variable_125(273) => null.
	p(NT(273), (nul));
//G417: variable(180)        => __E_variable_123(270) __E_variable_124(272) __E_variable_125(273).
	p(NT(180), (NT(270)+NT(272)+NT(273)));
//G418: __N_23(496)          => 'F'.
	p(NT(496), (T(33)));
//G419: __N_24(497)          => 'T'.
	p(NT(497), (T(43)));
//G420: __E_var_name_126(274) => ~( __N_23(496) ) & ~( __N_24(497) ) & alpha(5).	 # conjunctive
	p(NT(274), ~(NT(496)) & ~(NT(497)) & (NT(5)));
//G421: __E_var_name_127(275) => null.
	p(NT(275), (nul));
//G422: __E_var_name_127(275) => digit(3) __E_var_name_127(275).
	p(NT(275), (NT(3)+NT(275)));
//G423: var_name(18)         => __E_var_name_126(274) __E_var_name_127(275).	 # guarded: charvar
	p(NT(18), (NT(274)+NT(275)));
	p.back().guard = "charvar";
//G424: __N_25(498)          => 'F'.
	p(NT(498), (T(33)));
//G425: __N_26(499)          => 'T'.
	p(NT(499), (T(43)));
//G426: __E_var_name_128(276) => ~( __N_25(498) ) & ~( __N_26(499) ) & alpha(5).	 # conjunctive
	p(NT(276), ~(NT(498)) & ~(NT(499)) & (NT(5)));
//G427: __E_var_name_129(277) => alnum(6).
	p(NT(277), (NT(6)));
//G428: __E_var_name_129(277) => '_'.
	p(NT(277), (T(56)));
//G429: __E_var_name_130(278) => null.
	p(NT(278), (nul));
//G430: __E_var_name_130(278) => __E_var_name_129(277) __E_var_name_130(278).
	p(NT(278), (NT(277)+NT(278)));
//G431: var_name(18)         => __E_var_name_128(276) __E_var_name_130(278).	 # guarded: var
	p(NT(18), (NT(276)+NT(278)));
	p.back().guard = "var";
//G432: io_var(271)          => io_var_name(19) '[' offset(279) ']'.
	p(NT(271), (NT(19)+T(20)+NT(279)+T(21)));
//G433: io_var_name(19)      => chars(27).
	p(NT(19), (NT(27)));
//G434: bv_type_like(178)    => 'b' 'v' _(9) '[' _(9) num(188) _(9) ']'.
	p(NT(178), (T(25)+T(48)+NT(9)+T(20)+NT(9)+NT(188)+NT(9)+T(21)));
//G435: __E___E_uconst_131_132(281) => chars(27) _(9).
	p(NT(281), (NT(27)+NT(9)));
//G436: __E___E_uconst_131_132(281) => null.
	p(NT(281), (nul));
//G437: __E_uconst_131(280)  => __E___E_uconst_131_132(281) ':' _(9) chars(27).
	p(NT(280), (NT(281)+T(2)+NT(9)+NT(27)));
//G438: uconst_name(20)      => __E_uconst_131(280).
	p(NT(20), (NT(280)));
//G439: uconst(30)           => '<' _(9) uconst_name(20) _(9) '>'.
	p(NT(30), (T(29)+NT(9)+NT(20)+NT(9)+T(30)));
//G440: __E_q_vars_133(282)  => _(9) ',' _(9) q_var(28).
	p(NT(282), (NT(9)+T(19)+NT(9)+NT(28)));
//G441: __E_q_vars_134(283)  => null.
	p(NT(283), (nul));
//G442: __E_q_vars_134(283)  => __E_q_vars_133(282) __E_q_vars_134(283).
	p(NT(283), (NT(282)+NT(283)));
//G443: q_vars(123)          => q_var(28) __E_q_vars_134(283).
	p(NT(123), (NT(28)+NT(283)));
//G444: q_var(28)            => capture(17).
	p(NT(28), (NT(17)));
//G445: __N_27(500)          => uconst(30).
	p(NT(500), (NT(30)));
//G446: q_var(28)            => ~( __N_27(500) ) & variable(180).	 # conjunctive
	p(NT(28), ~(NT(500)) & (NT(180)));
//G447: ctnvar(22)           => chars(27).
	p(NT(22), (NT(27)));
//G448: __E_offsets_135(284) => _(9) ',' _(9) offset(279).
	p(NT(284), (NT(9)+T(19)+NT(9)+NT(279)));
//G449: __E_offsets_136(285) => null.
	p(NT(285), (nul));
//G450: __E_offsets_136(285) => __E_offsets_135(284) __E_offsets_136(285).
	p(NT(285), (NT(284)+NT(285)));
//G451: offsets(92)          => '[' _(9) offset(279) __E_offsets_136(285) _(9) ']'.
	p(NT(92), (T(20)+NT(9)+NT(279)+NT(285)+NT(9)+T(21)));
//G452: offset(279)          => integer(15).
	p(NT(279), (NT(15)));
//G453: offset(279)          => capture(17).
	p(NT(279), (NT(17)));
//G454: offset(279)          => shift(286).
	p(NT(279), (NT(286)));
//G455: __N_28(501)          => io_var(271).
	p(NT(501), (NT(271)));
//G456: __E_offset_137(287)  => variable(180) & ~( __N_28(501) ).	 # conjunctive
	p(NT(287), (NT(180)) & ~(NT(501)));
//G457: offset(279)          => __E_offset_137(287).
	p(NT(279), (NT(287)));
//G458: __E_shift_138(288)   => capture(17).
	p(NT(288), (NT(17)));
//G459: __N_29(502)          => io_var(271).
	p(NT(502), (NT(271)));
//G460: __E___E_shift_138_139(289) => variable(180) & ~( __N_29(502) ).	 # conjunctive
	p(NT(289), (NT(180)) & ~(NT(502)));
//G461: __E_shift_138(288)   => __E___E_shift_138_139(289).
	p(NT(288), (NT(289)));
//G462: shift(286)           => __E_shift_138(288) _(9) '-' _(9) num(188).
	p(NT(286), (NT(288)+NT(9)+T(36)+NT(9)+NT(188)));
//G463: __E_chars_140(290)   => alnum(6).
	p(NT(290), (NT(6)));
//G464: __E_chars_140(290)   => '_'.
	p(NT(290), (T(56)));
//G465: __E_chars_141(291)   => null.
	p(NT(291), (nul));
//G466: __E_chars_141(291)   => __E_chars_140(290) __E_chars_141(291).
	p(NT(291), (NT(290)+NT(291)));
//G467: chars(27)            => alpha(5) __E_chars_141(291).
	p(NT(27), (NT(5)+NT(291)));
//G468: __E_digits_142(292)  => digit(3).
	p(NT(292), (NT(3)));
//G469: __E_digits_142(292)  => digit(3) __E_digits_142(292).
	p(NT(292), (NT(3)+NT(292)));
//G470: digits(12)           => __E_digits_142(292).
	p(NT(12), (NT(292)));
//G471: num(188)             => digits(12).
	p(NT(188), (NT(12)));
//G472: __E_integer_143(293) => '-'.
	p(NT(293), (T(36)));
//G473: __E_integer_143(293) => null.
	p(NT(293), (nul));
//G474: integer(15)          => __E_integer_143(293) _(9) digits(12).
	p(NT(15), (NT(293)+NT(9)+NT(12)));
//G475: sym(11)              => chars(27).
	p(NT(11), (NT(27)));
//G476: capture(17)          => '$' chars(27).
	p(NT(17), (T(57)+NT(27)));
//G477: typed(70)            => _(9) ':' _(9) type(13) _(9) '[' _(9) subtype(14) _(9) ']'.
	p(NT(70), (NT(9)+T(2)+NT(9)+NT(13)+NT(9)+T(20)+NT(9)+NT(14)+NT(9)+T(21)));
//G478: __N_30(503)          => 'b' 'v'.
	p(NT(503), (T(25)+T(48)));
//G479: __E_typed_144(294)   => ~( __N_30(503) ) & type(13).	 # conjunctive
	p(NT(294), ~(NT(503)) & (NT(13)));
//G480: typed(70)            => _(9) ':' _(9) __E_typed_144(294).
	p(NT(70), (NT(9)+T(2)+NT(9)+NT(294)));
//G481: type(13)             => chars(27).
	p(NT(13), (NT(27)));
//G482: subtype(14)          => num(188).
	p(NT(14), (NT(188)));
//G483: __E_comment_145(296) => printable(8).
	p(NT(296), (NT(8)));
//G484: __E_comment_145(296) => '\t'.
	p(NT(296), (T(58)));
//G485: __E_comment_146(297) => null.
	p(NT(297), (nul));
//G486: __E_comment_146(297) => __E_comment_145(296) __E_comment_146(297).
	p(NT(297), (NT(296)+NT(297)));
//G487: __E_comment_147(298) => '\n'.
	p(NT(298), (T(59)));
//G488: __E_comment_147(298) => '\r'.
	p(NT(298), (T(60)));
//G489: __E_comment_147(298) => eof(1).
	p(NT(298), (NT(1)));
//G490: comment(295)         => '#' __E_comment_146(297) __E_comment_147(298).
	p(NT(295), (T(61)+NT(297)+NT(298)));
//G491: __(10)               => space(2).
	p(NT(10), (NT(2)));
//G492: __(10)               => comment(295).
	p(NT(10), (NT(295)));
//G493: __(10)               => __(10) space(2).
	p(NT(10), (NT(10)+NT(2)));
//G494: __(10)               => __(10) comment(295).
	p(NT(10), (NT(10)+NT(295)));
//G495: _(9)                 => __(10).
	p(NT(9), (NT(10)));
//G496: _(9)                 => null.
	p(NT(9), (nul));
//G497: cli(299)             => _(9).
	p(NT(299), (NT(9)));
//G498: __E_cli_148(301)     => '.' _(9) cli_command(300) _(9).
	p(NT(301), (T(1)+NT(9)+NT(300)+NT(9)));
//G499: __E_cli_149(302)     => null.
	p(NT(302), (nul));
//G500: __E_cli_149(302)     => __E_cli_148(301) __E_cli_149(302).
	p(NT(302), (NT(301)+NT(302)));
//G501: __E_cli_150(303)     => '.' _(9).
	p(NT(303), (T(1)+NT(9)));
//G502: __E_cli_150(303)     => null.
	p(NT(303), (nul));
//G503: cli(299)             => _(9) cli_command(300) _(9) __E_cli_149(302) __E_cli_150(303).
	p(NT(299), (NT(9)+NT(300)+NT(9)+NT(302)+NT(303)));
//G504: __E_cli_command_151(306) => 'q'.
	p(NT(306), (T(62)));
//G505: __E_cli_command_151(306) => 'q' 'u' 'i' 't'.
	p(NT(306), (T(62)+T(7)+T(4)+T(8)));
//G506: quit_sym(305)        => __E_cli_command_151(306).
	p(NT(305), (NT(306)));
//G507: quit_cmd(304)        => quit_sym(305).
	p(NT(304), (NT(305)));
//G508: cli_command(300)     => quit_cmd(304).
	p(NT(300), (NT(304)));
//G509: __E_cli_command_152(309) => 'v'.
	p(NT(309), (T(48)));
//G510: __E_cli_command_152(309) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(309), (T(48)+T(11)+T(27)+T(15)+T(4)+T(6)+T(5)));
//G511: version_sym(308)     => __E_cli_command_152(309).
	p(NT(308), (NT(309)));
//G512: version_cmd(307)     => version_sym(308).
	p(NT(307), (NT(308)));
//G513: cli_command(300)     => version_cmd(307).
	p(NT(300), (NT(307)));
//G514: __E_cli_command_153(312) => 'c'.
	p(NT(312), (T(14)));
//G515: __E_cli_command_153(312) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(312), (T(14)+T(10)+T(11)+T(24)+T(27)));
//G516: clear_sym(311)       => __E_cli_command_153(312).
	p(NT(311), (NT(312)));
//G517: clear_cmd(310)       => clear_sym(311).
	p(NT(310), (NT(311)));
//G518: cli_command(300)     => clear_cmd(310).
	p(NT(300), (NT(310)));
//G519: __E___E_cli_command_154_155(316) => 'h'.
	p(NT(316), (T(63)));
//G520: __E___E_cli_command_154_155(316) => 'h' 'e' 'l' 'p'.
	p(NT(316), (T(63)+T(11)+T(10)+T(18)));
//G521: help_sym(315)        => __E___E_cli_command_154_155(316).
	p(NT(315), (NT(316)));
//G522: __E___E_cli_command_154_156(317) => __(10) help_arg(318).
	p(NT(317), (NT(10)+NT(318)));
//G523: __E___E_cli_command_154_156(317) => null.
	p(NT(317), (nul));
//G524: __E_cli_command_154(314) => help_sym(315) __E___E_cli_command_154_156(317).
	p(NT(314), (NT(315)+NT(317)));
//G525: help_cmd(313)        => __E_cli_command_154(314).
	p(NT(313), (NT(314)));
//G526: cli_command(300)     => help_cmd(313).
	p(NT(300), (NT(313)));
//G527: file_sym(321)        => 'f' 'i' 'l' 'e'.
	p(NT(321), (T(9)+T(4)+T(10)+T(11)));
//G528: __E_cli_command_157(320) => file_sym(321) __(10) q_string(322).
	p(NT(320), (NT(321)+NT(10)+NT(322)));
//G529: file_cmd(319)        => __E_cli_command_157(320).
	p(NT(319), (NT(320)));
//G530: cli_command(300)     => file_cmd(319).
	p(NT(300), (NT(319)));
//G531: valid_sym(325)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(325), (T(48)+T(24)+T(10)+T(4)+T(55)));
//G532: __E_cli_command_158(324) => valid_sym(325) __(10) spec_arg(37).
	p(NT(324), (NT(325)+NT(10)+NT(37)));
//G533: valid_cmd(323)       => __E_cli_command_158(324).
	p(NT(323), (NT(324)));
//G534: cli_command(300)     => valid_cmd(323).
	p(NT(300), (NT(323)));
//G535: sat_sym(328)         => 's' 'a' 't'.
	p(NT(328), (T(15)+T(24)+T(8)));
//G536: __E_cli_command_159(327) => sat_sym(328) __(10) spec_arg(37).
	p(NT(327), (NT(328)+NT(10)+NT(37)));
//G537: sat_cmd(326)         => __E_cli_command_159(327).
	p(NT(326), (NT(327)));
//G538: cli_command(300)     => sat_cmd(326).
	p(NT(300), (NT(326)));
//G539: unsat_sym(331)       => 'u' 'n' 's' 'a' 't'.
	p(NT(331), (T(7)+T(5)+T(15)+T(24)+T(8)));
//G540: __E_cli_command_160(330) => unsat_sym(331) __(10) spec_arg(37).
	p(NT(330), (NT(331)+NT(10)+NT(37)));
//G541: unsat_cmd(329)       => __E_cli_command_160(330).
	p(NT(329), (NT(330)));
//G542: cli_command(300)     => unsat_cmd(329).
	p(NT(300), (NT(329)));
//G543: realizable_sym(334)  => 'r' 'e' 'a' 'l' 'i' 'z' 'a' 'b' 'l' 'e'.
	p(NT(334), (T(27)+T(11)+T(24)+T(10)+T(4)+T(64)+T(24)+T(25)+T(10)+T(11)));
//G544: __E_cli_command_161(333) => realizable_sym(334) __(10) spec_arg(37).
	p(NT(333), (NT(334)+NT(10)+NT(37)));
//G545: realizable_cmd(332)  => __E_cli_command_161(333).
	p(NT(332), (NT(333)));
//G546: cli_command(300)     => realizable_cmd(332).
	p(NT(300), (NT(332)));
//G547: unrealizable_sym(337) => 'u' 'n' 'r' 'e' 'a' 'l' 'i' 'z' 'a' 'b' 'l' 'e'.
	p(NT(337), (T(7)+T(5)+T(27)+T(11)+T(24)+T(10)+T(4)+T(64)+T(24)+T(25)+T(10)+T(11)));
//G548: __E_cli_command_162(336) => unrealizable_sym(337) __(10) spec_arg(37).
	p(NT(336), (NT(337)+NT(10)+NT(37)));
//G549: unrealizable_cmd(335) => __E_cli_command_162(336).
	p(NT(335), (NT(336)));
//G550: cli_command(300)     => unrealizable_cmd(335).
	p(NT(300), (NT(335)));
//G551: solve_sym(340)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(340), (T(15)+T(6)+T(10)+T(48)+T(11)));
//G552: __E___E_cli_command_163_164(341) => solve_options(342).
	p(NT(341), (NT(342)));
//G553: __E___E_cli_command_163_164(341) => null.
	p(NT(341), (nul));
//G554: __E_cli_command_163(339) => solve_sym(340) __E___E_cli_command_163_164(341) __(10) wff_cmd_arg(34).
	p(NT(339), (NT(340)+NT(341)+NT(10)+NT(34)));
//G555: solve_cmd(338)       => __E_cli_command_163(339).
	p(NT(338), (NT(339)));
//G556: cli_command(300)     => solve_cmd(338).
	p(NT(300), (NT(338)));
//G557: lgrs_sym(345)        => 'l' 'g' 'r' 's'.
	p(NT(345), (T(10)+T(65)+T(27)+T(15)));
//G558: __E_cli_command_165(344) => lgrs_sym(345) __(10) wff_cmd_arg(34).
	p(NT(344), (NT(345)+NT(10)+NT(34)));
//G559: lgrs_cmd(343)        => __E_cli_command_165(344).
	p(NT(343), (NT(344)));
//G560: cli_command(300)     => lgrs_cmd(343).
	p(NT(300), (NT(343)));
//G561: __E___E_cli_command_166_167(349) => 'r'.
	p(NT(349), (T(27)));
//G562: __E___E_cli_command_166_167(349) => 'r' 'u' 'n'.
	p(NT(349), (T(27)+T(7)+T(5)));
//G563: run_sym(348)         => __E___E_cli_command_166_167(349).
	p(NT(348), (NT(349)));
//G564: __E___E___E_cli_command_166_168_169(351) => 's' 't' 'e' 'p' 's'.
	p(NT(351), (T(15)+T(8)+T(11)+T(18)+T(15)));
//G565: __E___E___E_cli_command_166_168_169(351) => 's' 't' 'e' 'p'.
	p(NT(351), (T(15)+T(8)+T(11)+T(18)));
//G566: __E___E_cli_command_166_168(350) => __(10) num(188) __(10) __E___E___E_cli_command_166_168_169(351).
	p(NT(350), (NT(10)+NT(188)+NT(10)+NT(351)));
//G567: __E___E_cli_command_166_168(350) => null.
	p(NT(350), (nul));
//G568: __E___E_cli_command_166_170(352) => __(10) spec_arg(37).
	p(NT(352), (NT(10)+NT(37)));
//G569: __E___E_cli_command_166_170(352) => null.
	p(NT(352), (nul));
//G570: __E_cli_command_166(347) => run_sym(348) __E___E_cli_command_166_168(350) __E___E_cli_command_166_170(352).
	p(NT(347), (NT(348)+NT(350)+NT(352)));
//G571: run_cmd(346)         => __E_cli_command_166(347).
	p(NT(346), (NT(347)));
//G572: cli_command(300)     => run_cmd(346).
	p(NT(300), (NT(346)));
//G573: stop_sym(355)        => 's' 't' 'o' 'p'.
	p(NT(355), (T(15)+T(8)+T(6)+T(18)));
//G574: __E_cli_command_171(354) => stop_sym(355).
	p(NT(354), (NT(355)));
//G575: stop_cmd(353)        => __E_cli_command_171(354).
	p(NT(353), (NT(354)));
//G576: cli_command(300)     => stop_cmd(353).
	p(NT(300), (NT(353)));
//G577: memory_sym(358)      => 'm' 'e' 'm' 'o' 'r' 'y'.
	p(NT(358), (T(28)+T(11)+T(28)+T(6)+T(27)+T(17)));
//G578: __E_cli_command_172(357) => memory_sym(358).
	p(NT(357), (NT(358)));
//G579: memory_cmd(356)      => __E_cli_command_172(357).
	p(NT(356), (NT(357)));
//G580: cli_command(300)     => memory_cmd(356).
	p(NT(300), (NT(356)));
//G581: ltl_sym(361)         => 'l' 't' 'l'.
	p(NT(361), (T(10)+T(8)+T(10)));
//G582: __E_cli_command_173(360) => ltl_sym(361) __(10) spec_arg(37).
	p(NT(360), (NT(361)+NT(10)+NT(37)));
//G583: ltl_cmd(359)         => __E_cli_command_173(360).
	p(NT(359), (NT(360)));
//G584: cli_command(300)     => ltl_cmd(359).
	p(NT(300), (NT(359)));
//G585: func_sym_cmd(364)    => 'f' 'u' 'n' 'c'.
	p(NT(364), (T(9)+T(7)+T(5)+T(14)));
//G586: __E_cli_command_174(363) => func_sym_cmd(364) __(10) func_decl(250).
	p(NT(363), (NT(364)+NT(10)+NT(250)));
//G587: func_cmd(362)        => __E_cli_command_174(363).
	p(NT(362), (NT(363)));
//G588: cli_command(300)     => func_cmd(362).
	p(NT(300), (NT(362)));
//G589: __E___E_cli_command_175_176(368) => 'n'.
	p(NT(368), (T(5)));
//G590: __E___E_cli_command_175_176(368) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(368), (T(5)+T(6)+T(27)+T(28)+T(24)+T(10)+T(4)+T(64)+T(11)));
//G591: normalize_sym(367)   => __E___E_cli_command_175_176(368).
	p(NT(367), (NT(368)));
//G592: __E_cli_command_175(366) => normalize_sym(367) __(10) spec_arg(37).
	p(NT(366), (NT(367)+NT(10)+NT(37)));
//G593: normalize_cmd(365)   => __E_cli_command_175(366).
	p(NT(365), (NT(366)));
//G594: cli_command(300)     => normalize_cmd(365).
	p(NT(300), (NT(365)));
//G595: __E___E_cli_command_177_178(372) => 's'.
	p(NT(372), (T(15)));
//G596: __E___E_cli_command_177_178(372) => 's' 'u' 'b' 's' 't'.
	p(NT(372), (T(15)+T(7)+T(25)+T(15)+T(8)));
//G597: __E___E_cli_command_177_178(372) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(372), (T(15)+T(7)+T(25)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G598: subst_sym(371)       => __E___E_cli_command_177_178(372).
	p(NT(371), (NT(372)));
//G599: __E_cli_command_177(370) => subst_sym(371) __(10) nf_cmd_arg(35) _(9) '[' _(9) nf_cmd_arg(35) _(9) '/' _(9) nf_cmd_arg(35) _(9) ']'.
	p(NT(370), (NT(371)+NT(10)+NT(35)+NT(9)+T(20)+NT(9)+NT(35)+NT(9)+T(16)+NT(9)+NT(35)+NT(9)+T(21)));
//G600: subst_cmd(369)       => __E_cli_command_177(370).
	p(NT(369), (NT(370)));
//G601: cli_command(300)     => subst_cmd(369).
	p(NT(300), (NT(369)));
//G602: __E___E_cli_command_179_180(376) => 'i'.
	p(NT(376), (T(4)));
//G603: __E___E_cli_command_179_180(376) => 'i' 'n' 's' 't'.
	p(NT(376), (T(4)+T(5)+T(15)+T(8)));
//G604: __E___E_cli_command_179_180(376) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(376), (T(4)+T(5)+T(15)+T(8)+T(24)+T(5)+T(8)+T(4)+T(24)+T(8)+T(11)));
//G605: inst_sym(375)        => __E___E_cli_command_179_180(376).
	p(NT(375), (NT(376)));
//G606: __E_cli_command_179(374) => inst_sym(375) __(10) inst_args(32).
	p(NT(374), (NT(375)+NT(10)+NT(32)));
//G607: inst_cmd(373)        => __E_cli_command_179(374).
	p(NT(373), (NT(374)));
//G608: cli_command(300)     => inst_cmd(373).
	p(NT(300), (NT(373)));
//G609: dnf_sym(379)         => 'd' 'n' 'f'.
	p(NT(379), (T(55)+T(5)+T(9)));
//G610: __E_cli_command_181(378) => dnf_sym(379) __(10) nf_cmd_arg(35).
	p(NT(378), (NT(379)+NT(10)+NT(35)));
//G611: dnf_cmd(377)         => __E_cli_command_181(378).
	p(NT(377), (NT(378)));
//G612: cli_command(300)     => dnf_cmd(377).
	p(NT(300), (NT(377)));
//G613: cnf_sym(382)         => 'c' 'n' 'f'.
	p(NT(382), (T(14)+T(5)+T(9)));
//G614: __E_cli_command_182(381) => cnf_sym(382) __(10) nf_cmd_arg(35).
	p(NT(381), (NT(382)+NT(10)+NT(35)));
//G615: cnf_cmd(380)         => __E_cli_command_182(381).
	p(NT(380), (NT(381)));
//G616: cli_command(300)     => cnf_cmd(380).
	p(NT(300), (NT(380)));
//G617: nnf_sym(385)         => 'n' 'n' 'f'.
	p(NT(385), (T(5)+T(5)+T(9)));
//G618: __E_cli_command_183(384) => nnf_sym(385) __(10) nf_cmd_arg(35).
	p(NT(384), (NT(385)+NT(10)+NT(35)));
//G619: nnf_cmd(383)         => __E_cli_command_183(384).
	p(NT(383), (NT(384)));
//G620: cli_command(300)     => nnf_cmd(383).
	p(NT(300), (NT(383)));
//G621: mnf_sym(388)         => 'm' 'n' 'f'.
	p(NT(388), (T(28)+T(5)+T(9)));
//G622: __E_cli_command_184(387) => mnf_sym(388) __(10) nf_cmd_arg(35).
	p(NT(387), (NT(388)+NT(10)+NT(35)));
//G623: mnf_cmd(386)         => __E_cli_command_184(387).
	p(NT(386), (NT(387)));
//G624: cli_command(300)     => mnf_cmd(386).
	p(NT(300), (NT(386)));
//G625: onf_sym(391)         => 'o' 'n' 'f'.
	p(NT(391), (T(6)+T(5)+T(9)));
//G626: __E_cli_command_185(390) => onf_sym(391) __(10) variable(180) __(10) onf_cmd_arg(36).
	p(NT(390), (NT(391)+NT(10)+NT(180)+NT(10)+NT(36)));
//G627: onf_cmd(389)         => __E_cli_command_185(390).
	p(NT(389), (NT(390)));
//G628: cli_command(300)     => onf_cmd(389).
	p(NT(300), (NT(389)));
//G629: qelim_sym(394)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(394), (T(62)+T(11)+T(10)+T(4)+T(28)));
//G630: __E_cli_command_186(393) => qelim_sym(394) __(10) wff_cmd_arg(34).
	p(NT(393), (NT(394)+NT(10)+NT(34)));
//G631: qelim_cmd(392)       => __E_cli_command_186(393).
	p(NT(392), (NT(393)));
//G632: cli_command(300)     => qelim_cmd(392).
	p(NT(300), (NT(392)));
//G633: whatis_sym(397)      => 'w' 'h' 'a' 't' 'i' 's'.
	p(NT(397), (T(31)+T(63)+T(24)+T(8)+T(4)+T(15)));
//G634: __E_cli_command_187(396) => whatis_sym(397) __(10) spec_arg(37).
	p(NT(396), (NT(397)+NT(10)+NT(37)));
//G635: whatis_cmd(395)      => __E_cli_command_187(396).
	p(NT(395), (NT(396)));
//G636: cli_command(300)     => whatis_cmd(395).
	p(NT(300), (NT(395)));
//G637: reset_sym(400)       => 'r' 'e' 's' 'e' 't'.
	p(NT(400), (T(27)+T(11)+T(15)+T(11)+T(8)));
//G638: __E_cli_command_188(399) => reset_sym(400).
	p(NT(399), (NT(400)));
//G639: reset_cmd(398)       => __E_cli_command_188(399).
	p(NT(398), (NT(399)));
//G640: cli_command(300)     => reset_cmd(398).
	p(NT(300), (NT(398)));
//G641: fragment_sym(403)    => 'f' 'r' 'a' 'g' 'm' 'e' 'n' 't'.
	p(NT(403), (T(9)+T(27)+T(24)+T(65)+T(28)+T(11)+T(5)+T(8)));
//G642: __E_cli_command_189(402) => fragment_sym(403) __(10) fragment_name(404).
	p(NT(402), (NT(403)+NT(10)+NT(404)));
//G643: fragment_cmd(401)    => __E_cli_command_189(402).
	p(NT(401), (NT(402)));
//G644: cli_command(300)     => fragment_cmd(401).
	p(NT(300), (NT(401)));
//G645: get_sym(407)         => 'g' 'e' 't'.
	p(NT(407), (T(65)+T(11)+T(8)));
//G646: __E___E_cli_command_190_191(408) => __(10) option_name(23).
	p(NT(408), (NT(10)+NT(23)));
//G647: __E___E_cli_command_190_191(408) => null.
	p(NT(408), (nul));
//G648: __E_cli_command_190(406) => get_sym(407) __E___E_cli_command_190_191(408).
	p(NT(406), (NT(407)+NT(408)));
//G649: get_cmd(405)         => __E_cli_command_190(406).
	p(NT(405), (NT(406)));
//G650: cli_command(300)     => get_cmd(405).
	p(NT(300), (NT(405)));
//G651: set_sym(411)         => 's' 'e' 't'.
	p(NT(411), (T(15)+T(11)+T(8)));
//G652: __E___E_cli_command_192_193(412) => __(10).
	p(NT(412), (NT(10)));
//G653: __E___E_cli_command_192_193(412) => _(9) '=' _(9).
	p(NT(412), (NT(9)+T(3)+NT(9)));
//G654: __E_cli_command_192(410) => set_sym(411) __(10) option_name(23) __E___E_cli_command_192_193(412) option_value(24).
	p(NT(410), (NT(411)+NT(10)+NT(23)+NT(412)+NT(24)));
//G655: set_cmd(409)         => __E_cli_command_192(410).
	p(NT(409), (NT(410)));
//G656: cli_command(300)     => set_cmd(409).
	p(NT(300), (NT(409)));
//G657: enable_sym(415)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(415), (T(11)+T(5)+T(24)+T(25)+T(10)+T(11)));
//G658: __E_cli_command_194(414) => enable_sym(415) __(10) option_name(23).
	p(NT(414), (NT(415)+NT(10)+NT(23)));
//G659: enable_cmd(413)      => __E_cli_command_194(414).
	p(NT(413), (NT(414)));
//G660: cli_command(300)     => enable_cmd(413).
	p(NT(300), (NT(413)));
//G661: disable_sym(418)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(418), (T(55)+T(4)+T(15)+T(24)+T(25)+T(10)+T(11)));
//G662: __E_cli_command_195(417) => disable_sym(418) __(10) option_name(23).
	p(NT(417), (NT(418)+NT(10)+NT(23)));
//G663: disable_cmd(416)     => __E_cli_command_195(417).
	p(NT(416), (NT(417)));
//G664: cli_command(300)     => disable_cmd(416).
	p(NT(300), (NT(416)));
//G665: toggle_sym(421)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(421), (T(8)+T(6)+T(65)+T(65)+T(10)+T(11)));
//G666: __E_cli_command_196(420) => toggle_sym(421) __(10) option_name(23).
	p(NT(420), (NT(421)+NT(10)+NT(23)));
//G667: toggle_cmd(419)      => __E_cli_command_196(420).
	p(NT(419), (NT(420)));
//G668: cli_command(300)     => toggle_cmd(419).
	p(NT(300), (NT(419)));
//G669: __E___E_cli_command_197_198(425) => 'd' 'e' 'f' 's'.
	p(NT(425), (T(55)+T(11)+T(9)+T(15)));
//G670: __E___E_cli_command_197_198(425) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(425), (T(55)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G671: def_sym(424)         => __E___E_cli_command_197_198(425).
	p(NT(424), (NT(425)));
//G672: __E_cli_command_197(423) => def_sym(424).
	p(NT(423), (NT(424)));
//G673: def_list_cmd(422)    => __E_cli_command_197(423).
	p(NT(422), (NT(423)));
//G674: cli_command(300)     => def_list_cmd(422).
	p(NT(300), (NT(422)));
//G675: __E_cli_command_199(427) => def_sym(424) __(10) num(188).
	p(NT(427), (NT(424)+NT(10)+NT(188)));
//G676: def_print_cmd(426)   => __E_cli_command_199(427).
	p(NT(426), (NT(427)));
//G677: cli_command(300)     => def_print_cmd(426).
	p(NT(300), (NT(426)));
//G678: def_rr_cmd(428)      => rec_relation(56).
	p(NT(428), (NT(56)));
//G679: cli_command(300)     => def_rr_cmd(428).
	p(NT(300), (NT(428)));
//G680: def_input_cmd(429)   => input_def(57).
	p(NT(429), (NT(57)));
//G681: cli_command(300)     => def_input_cmd(429).
	p(NT(300), (NT(429)));
//G682: def_output_cmd(430)  => output_def(58).
	p(NT(430), (NT(58)));
//G683: cli_command(300)     => def_output_cmd(430).
	p(NT(300), (NT(430)));
//G684: def_type_cmd(431)    => type_def(59).
	p(NT(431), (NT(59)));
//G685: cli_command(300)     => def_type_cmd(431).
	p(NT(300), (NT(431)));
//G686: __E___E_cli_command_200_201(435) => 'h' 'i' 's' 't'.
	p(NT(435), (T(63)+T(4)+T(15)+T(8)));
//G687: __E___E_cli_command_200_201(435) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(435), (T(63)+T(4)+T(15)+T(8)+T(6)+T(27)+T(17)));
//G688: history_sym(434)     => __E___E_cli_command_200_201(435).
	p(NT(434), (NT(435)));
//G689: __E_cli_command_200(433) => history_sym(434).
	p(NT(433), (NT(434)));
//G690: history_list_cmd(432) => __E_cli_command_200(433).
	p(NT(432), (NT(433)));
//G691: cli_command(300)     => history_list_cmd(432).
	p(NT(300), (NT(432)));
//G692: __E_cli_command_202(437) => history_sym(434) __(10) history(438).
	p(NT(437), (NT(434)+NT(10)+NT(438)));
//G693: history_print_cmd(436) => __E_cli_command_202(437).
	p(NT(436), (NT(437)));
//G694: cli_command(300)     => history_print_cmd(436).
	p(NT(300), (NT(436)));
//G695: __E_cli_command_203(440) => ref(65).
	p(NT(440), (NT(65)));
//G696: __E_cli_command_203(440) => wff(38).
	p(NT(440), (NT(38)));
//G697: __E_cli_command_203(440) => bf(40).
	p(NT(440), (NT(40)));
//G698: history_store_cmd(439) => __E_cli_command_203(440).
	p(NT(439), (NT(440)));
//G699: cli_command(300)     => history_store_cmd(439).
	p(NT(300), (NT(439)));
//G700: solve_options(342)   => __(10) solve_option(441) solve_options(342).
	p(NT(342), (NT(10)+NT(441)+NT(342)));
//G701: solve_options(342)   => null.
	p(NT(342), (nul));
//G702: __E_solve_option_204(442) => solver_mode(443).
	p(NT(442), (NT(443)));
//G703: __E_solve_option_204(442) => type(13).
	p(NT(442), (NT(13)));
//G704: solve_option(441)    => '-' '-' __E_solve_option_204(442).
	p(NT(441), (T(36)+T(36)+NT(442)));
//G705: __E_solver_mode_205(445) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(445), (T(28)+T(4)+T(5)+T(4)+T(28)+T(7)+T(28)));
//G706: __E_solver_mode_205(445) => 'm' 'i' 'n'.
	p(NT(445), (T(28)+T(4)+T(5)));
//G707: solver_mode_minimum(444) => __E_solver_mode_205(445).
	p(NT(444), (NT(445)));
//G708: solver_mode(443)     => solver_mode_minimum(444).
	p(NT(443), (NT(444)));
//G709: __E_solver_mode_206(447) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(447), (T(28)+T(24)+T(38)+T(4)+T(28)+T(7)+T(28)));
//G710: __E_solver_mode_206(447) => 'm' 'a' 'x'.
	p(NT(447), (T(28)+T(24)+T(38)));
//G711: solver_mode_maximum(446) => __E_solver_mode_206(447).
	p(NT(446), (NT(447)));
//G712: solver_mode(443)     => solver_mode_maximum(446).
	p(NT(443), (NT(446)));
//G713: q_file_name(74)      => '"' file_name(21) '"'.
	p(NT(74), (T(66)+NT(21)+T(66)));
//G714: __E_file_name_207(448) => printable(8).
	p(NT(448), (NT(8)));
//G715: __E_file_name_207(448) => printable(8) __E_file_name_207(448).
	p(NT(448), (NT(8)+NT(448)));
//G716: file_name(21)        => __E_file_name_207(448).
	p(NT(21), (NT(448)));
//G717: __E_option_name_208(449) => alnum(6).
	p(NT(449), (NT(6)));
//G718: __E_option_name_208(449) => alnum(6) __E_option_name_208(449).
	p(NT(449), (NT(6)+NT(449)));
//G719: option_name(23)      => __E_option_name_208(449).
	p(NT(23), (NT(449)));
//G720: __E_option_value_209(450) => alnum(6).
	p(NT(450), (NT(6)));
//G721: __E_option_value_209(450) => '.'.
	p(NT(450), (T(1)));
//G722: __E_option_value_210(451) => __E_option_value_209(450).
	p(NT(451), (NT(450)));
//G723: __E_option_value_210(451) => __E_option_value_209(450) __E_option_value_210(451).
	p(NT(451), (NT(450)+NT(451)));
//G724: option_value(24)     => __E_option_value_210(451).
	p(NT(24), (NT(451)));
//G725: fragment_ltl(453)    => 'l' 't' 'l'.
	p(NT(453), (T(10)+T(8)+T(10)));
//G726: __E_fragment_name_211(452) => fragment_ltl(453).
	p(NT(452), (NT(453)));
//G727: fragment_ctl_star(454) => 'c' 't' 'l' '_' 's' 't' 'a' 'r'.
	p(NT(454), (T(14)+T(8)+T(10)+T(56)+T(15)+T(8)+T(24)+T(27)));
//G728: __E_fragment_name_211(452) => fragment_ctl_star(454).
	p(NT(452), (NT(454)));
//G729: fragment_ctl_star(454) => 'c' 't' 'l' '*'.
	p(NT(454), (T(14)+T(8)+T(10)+T(51)));
//G730: __E_fragment_name_211(452) => fragment_ctl_star(454).
	p(NT(452), (NT(454)));
//G731: fragment_name(404)   => __E_fragment_name_211(452).
	p(NT(404), (NT(452)));
//G732: bf_cmd_arg(33)       => history(438).
	p(NT(33), (NT(438)));
//G733: bf_cmd_arg(33)       => bf(40).
	p(NT(33), (NT(40)));
//G734: wff_cmd_arg(34)      => history(438).
	p(NT(34), (NT(438)));
//G735: wff_cmd_arg(34)      => wff(38).
	p(NT(34), (NT(38)));
//G736: nf_cmd_arg(35)       => history(438).
	p(NT(35), (NT(438)));
//G737: nf_cmd_arg(35)       => ref(65).
	p(NT(35), (NT(65)));
//G738: nf_cmd_arg(35)       => wff(38).
	p(NT(35), (NT(38)));
//G739: nf_cmd_arg(35)       => bf(40).
	p(NT(35), (NT(40)));
//G740: onf_cmd_arg(36)      => history(438).
	p(NT(36), (NT(438)));
//G741: onf_cmd_arg(36)      => wff(38).
	p(NT(36), (NT(38)));
//G742: spec_arg(37)         => history(438).
	p(NT(37), (NT(438)));
//G743: spec_arg(37)         => spec(49).
	p(NT(37), (NT(49)));
//G744: spec_arg(37)         => ref(65).
	p(NT(37), (NT(65)));
//G745: spec_arg(37)         => wff(38).
	p(NT(37), (NT(38)));
//G746: spec_arg(37)         => bf(40).
	p(NT(37), (NT(40)));
//G747: inst_args(32)        => wff_cmd_arg(34) _(9) '[' _(9) bf(40) _(9) '/' _(9) bf_cmd_arg(33) _(9) ']'.
	p(NT(32), (NT(34)+NT(9)+T(20)+NT(9)+NT(40)+NT(9)+T(16)+NT(9)+NT(33)+NT(9)+T(21)));
//G748: inst_args(32)        => bf_cmd_arg(33) _(9) '[' _(9) bf(40) _(9) '/' _(9) bf_cmd_arg(33) _(9) ']'.
	p(NT(32), (NT(33)+NT(9)+T(20)+NT(9)+NT(40)+NT(9)+T(16)+NT(9)+NT(33)+NT(9)+T(21)));
//G749: help_arg(318)        => help_sym(315).
	p(NT(318), (NT(315)));
//G750: help_arg(318)        => version_sym(308).
	p(NT(318), (NT(308)));
//G751: help_arg(318)        => quit_sym(305).
	p(NT(318), (NT(305)));
//G752: help_arg(318)        => clear_sym(311).
	p(NT(318), (NT(311)));
//G753: help_arg(318)        => get_sym(407).
	p(NT(318), (NT(407)));
//G754: help_arg(318)        => set_sym(411).
	p(NT(318), (NT(411)));
//G755: help_arg(318)        => enable_sym(415).
	p(NT(318), (NT(415)));
//G756: help_arg(318)        => disable_sym(418).
	p(NT(318), (NT(418)));
//G757: help_arg(318)        => toggle_sym(421).
	p(NT(318), (NT(421)));
//G758: help_arg(318)        => file_sym(321).
	p(NT(318), (NT(321)));
//G759: help_arg(318)        => history_sym(434).
	p(NT(318), (NT(434)));
//G760: help_arg(318)        => abs_history_sym(455).
	p(NT(318), (NT(455)));
//G761: help_arg(318)        => rel_history_sym(456).
	p(NT(318), (NT(456)));
//G762: help_arg(318)        => selection_sym(457).
	p(NT(318), (NT(457)));
//G763: help_arg(318)        => def_sym(424).
	p(NT(318), (NT(424)));
//G764: help_arg(318)        => inst_sym(375).
	p(NT(318), (NT(375)));
//G765: help_arg(318)        => subst_sym(371).
	p(NT(318), (NT(371)));
//G766: help_arg(318)        => normalize_sym(367).
	p(NT(318), (NT(367)));
//G767: help_arg(318)        => execute_sym(458).
	p(NT(318), (NT(458)));
//G768: help_arg(318)        => solve_sym(340).
	p(NT(318), (NT(340)));
//G769: help_arg(318)        => lgrs_sym(345).
	p(NT(318), (NT(345)));
//G770: help_arg(318)        => valid_sym(325).
	p(NT(318), (NT(325)));
//G771: help_arg(318)        => sat_sym(328).
	p(NT(318), (NT(328)));
//G772: help_arg(318)        => unsat_sym(331).
	p(NT(318), (NT(331)));
//G773: help_arg(318)        => realizable_sym(334).
	p(NT(318), (NT(334)));
//G774: help_arg(318)        => unrealizable_sym(337).
	p(NT(318), (NT(337)));
//G775: help_arg(318)        => run_sym(348).
	p(NT(318), (NT(348)));
//G776: help_arg(318)        => ltl_sym(361).
	p(NT(318), (NT(361)));
//G777: help_arg(318)        => stop_sym(355).
	p(NT(318), (NT(355)));
//G778: help_arg(318)        => memory_sym(358).
	p(NT(318), (NT(358)));
//G779: help_arg(318)        => dnf_sym(379).
	p(NT(318), (NT(379)));
//G780: help_arg(318)        => cnf_sym(382).
	p(NT(318), (NT(382)));
//G781: help_arg(318)        => snf_sym(459).
	p(NT(318), (NT(459)));
//G782: help_arg(318)        => nnf_sym(385).
	p(NT(318), (NT(385)));
//G783: help_arg(318)        => mnf_sym(388).
	p(NT(318), (NT(388)));
//G784: help_arg(318)        => onf_sym(391).
	p(NT(318), (NT(391)));
//G785: help_arg(318)        => qelim_sym(394).
	p(NT(318), (NT(394)));
//G786: help_arg(318)        => whatis_sym(397).
	p(NT(318), (NT(397)));
//G787: help_arg(318)        => reset_sym(400).
	p(NT(318), (NT(400)));
//G788: help_arg(318)        => fragment_sym(403).
	p(NT(318), (NT(403)));
//G789: __E___E_help_arg_212_213(462) => 's'.
	p(NT(462), (T(15)));
//G790: __E___E_help_arg_212_213(462) => null.
	p(NT(462), (nul));
//G791: __E_help_arg_212(461) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_212_213(462).
	p(NT(461), (T(11)+T(38)+T(24)+T(28)+T(18)+T(10)+T(11)+NT(462)));
//G792: examples_sym(460)    => __E_help_arg_212(461).
	p(NT(460), (NT(461)));
//G793: help_arg(318)        => examples_sym(460).
	p(NT(318), (NT(460)));
//G794: __E_history_214(464) => '%'.
	p(NT(464), (T(50)));
//G795: rel_history_sym(456) => '%' '-'.
	p(NT(456), (T(50)+T(36)));
//G796: history_id(466)      => digits(12).
	p(NT(466), (NT(12)));
//G797: __E___E_history_214_215(465) => history_id(466).
	p(NT(465), (NT(466)));
//G798: __E___E_history_214_215(465) => null.
	p(NT(465), (nul));
//G799: __E_history_214(464) => rel_history_sym(456) __E___E_history_214_215(465).
	p(NT(464), (NT(456)+NT(465)));
//G800: rel_history(463)     => __E_history_214(464).
	p(NT(463), (NT(464)));
//G801: history(438)         => rel_history(463).
	p(NT(438), (NT(463)));
//G802: abs_history_sym(455) => '%'.
	p(NT(455), (T(50)));
//G803: __E_history_216(468) => abs_history_sym(455) history_id(466).
	p(NT(468), (NT(455)+NT(466)));
//G804: abs_history(467)     => __E_history_216(468).
	p(NT(467), (NT(468)));
//G805: history(438)         => abs_history(467).
	p(NT(438), (NT(467)));
//G806: unreachable(469)     => BDD_ID(470).
	p(NT(469), (NT(470)));
//G807: stream_def(471)      => input_def(57).
	p(NT(471), (NT(57)));
//G808: stream_def(471)      => output_def(58).
	p(NT(471), (NT(58)));
//G809: fm_or_term(472)      => wff(38).
	p(NT(472), (NT(38)));
//G810: fm_or_term(472)      => bf(40).
	p(NT(472), (NT(40)));
	#undef T
	#undef NT
	return loaded = true, p;
}

} // namespace tau_parser_data
