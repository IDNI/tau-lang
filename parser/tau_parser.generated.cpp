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
//G100: __E___E_wff_32_33(107) => 'F'.
	p(NT(107), (T(31)));
//G101: __E_wff_32(106)      => __E___E_wff_32_33(107) _(9) wff(38).
	p(NT(106), (NT(107)+NT(9)+NT(38)));
//G102: wff_sometimes(105)   => __E_wff_32(106).
	p(NT(105), (NT(106)));
//G103: wff(38)              => wff_sometimes(105).
	p(NT(38), (NT(105)));
//G104: __E___E_wff_34_35(110) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(110), (T(24)+T(10)+T(32)+T(24)+T(17)+T(15)));
//G105: __E___E_wff_34_35(110) => '[' ']'.
	p(NT(110), (T(20)+T(21)));
//G106: __E___E_wff_34_35(110) => 'G'.
	p(NT(110), (T(33)));
//G107: __E_wff_34(109)      => __E___E_wff_34_35(110) _(9) wff(38).
	p(NT(109), (NT(110)+NT(9)+NT(38)));
//G108: wff_always(108)      => __E_wff_34(109).
	p(NT(108), (NT(109)));
//G109: wff(38)              => wff_always(108).
	p(NT(38), (NT(108)));
//G110: __E_wff_36(112)      => 'A' _(9) wff(38).
	p(NT(112), (T(34)+NT(9)+NT(38)));
//G111: wff_A(111)           => __E_wff_36(112).
	p(NT(111), (NT(112)));
//G112: wff(38)              => wff_A(111).
	p(NT(38), (NT(111)));
//G113: __E_wff_37(114)      => 'E' _(9) wff(38).
	p(NT(114), (T(35)+NT(9)+NT(38)));
//G114: wff_E(113)           => __E_wff_37(114).
	p(NT(113), (NT(114)));
//G115: wff(38)              => wff_E(113).
	p(NT(38), (NT(113)));
//G116: __E_wff_38(116)      => '-' _(9) wff(38).
	p(NT(116), (T(36)+NT(9)+NT(38)));
//G117: wff_semantic_neg(115) => __E_wff_38(116).
	p(NT(115), (NT(116)));
//G118: wff(38)              => wff_semantic_neg(115).
	p(NT(38), (NT(115)));
//G119: __E_wff_39(118)      => wff(38) _(9) '?' _(9) wff(38) _(9) ':' _(9) wff(38).
	p(NT(118), (NT(38)+NT(9)+T(37)+NT(9)+NT(38)+NT(9)+T(2)+NT(9)+NT(38)));
//G120: wff_conditional(117) => __E_wff_39(118).
	p(NT(117), (NT(118)));
//G121: wff(38)              => wff_conditional(117).
	p(NT(38), (NT(117)));
//G122: __E_wff_40(120)      => 'a' 'l' 'l' __(10) q_vars(121) __(10) wff(38).
	p(NT(120), (T(24)+T(10)+T(10)+NT(10)+NT(121)+NT(10)+NT(38)));
//G123: wff_all(119)         => __E_wff_40(120).
	p(NT(119), (NT(120)));
//G124: wff(38)              => wff_all(119).
	p(NT(38), (NT(119)));
//G125: __E_wff_41(123)      => 'e' 'x' __(10) q_vars(121) __(10) wff(38).
	p(NT(123), (T(11)+T(38)+NT(10)+NT(121)+NT(10)+NT(38)));
//G126: wff_ex(122)          => __E_wff_41(123).
	p(NT(122), (NT(123)));
//G127: wff(38)              => wff_ex(122).
	p(NT(38), (NT(122)));
//G128: wff_ref(124)         => ref(65).
	p(NT(124), (NT(65)));
//G129: wff(38)              => wff_ref(124).
	p(NT(38), (NT(124)));
//G130: wff(38)              => constraint(125).
	p(NT(38), (NT(125)));
//G131: __E_wff_42(127)      => wff(38) _(9) '-' '>' _(9) wff(38).
	p(NT(127), (NT(38)+NT(9)+T(36)+T(30)+NT(9)+NT(38)));
//G132: wff_imply(126)       => __E_wff_42(127).
	p(NT(126), (NT(127)));
//G133: wff(38)              => wff_imply(126).
	p(NT(38), (NT(126)));
//G134: __E_wff_43(129)      => wff(38) _(9) '<' '-' _(9) wff(38).
	p(NT(129), (NT(38)+NT(9)+T(29)+T(36)+NT(9)+NT(38)));
//G135: wff_rimply(128)      => __E_wff_43(129).
	p(NT(128), (NT(129)));
//G136: wff(38)              => wff_rimply(128).
	p(NT(38), (NT(128)));
//G137: __E_wff_44(131)      => wff(38) _(9) '<' '-' '>' _(9) wff(38).
	p(NT(131), (NT(38)+NT(9)+T(29)+T(36)+T(30)+NT(9)+NT(38)));
//G138: wff_equiv(130)       => __E_wff_44(131).
	p(NT(130), (NT(131)));
//G139: wff(38)              => wff_equiv(130).
	p(NT(38), (NT(130)));
//G140: __E_wff_45(133)      => wff(38) _(9) '|' '|' _(9) wff(38).
	p(NT(133), (NT(38)+NT(9)+T(39)+T(39)+NT(9)+NT(38)));
//G141: wff_or(132)          => __E_wff_45(133).
	p(NT(132), (NT(133)));
//G142: wff(38)              => wff_or(132).
	p(NT(38), (NT(132)));
//G143: __E_wff_46(135)      => wff(38) _(9) '^' '^' _(9) wff(38).
	p(NT(135), (NT(38)+NT(9)+T(40)+T(40)+NT(9)+NT(38)));
//G144: wff_xor(134)         => __E_wff_46(135).
	p(NT(134), (NT(135)));
//G145: wff(38)              => wff_xor(134).
	p(NT(38), (NT(134)));
//G146: __E_wff_47(137)      => wff(38) _(9) '&' '&' _(9) wff(38).
	p(NT(137), (NT(38)+NT(9)+T(41)+T(41)+NT(9)+NT(38)));
//G147: wff_and(136)         => __E_wff_47(137).
	p(NT(136), (NT(137)));
//G148: wff(38)              => wff_and(136).
	p(NT(38), (NT(136)));
//G149: __E_wff_48(139)      => '!' _(9) wff(38).
	p(NT(139), (T(42)+NT(9)+NT(38)));
//G150: wff_neg(138)         => __E_wff_48(139).
	p(NT(138), (NT(139)));
//G151: wff(38)              => wff_neg(138).
	p(NT(38), (NT(138)));
//G152: wff_t(140)           => 'T'.
	p(NT(140), (T(43)));
//G153: wff(38)              => wff_t(140).
	p(NT(38), (NT(140)));
//G154: wff_f(141)           => 'F'.
	p(NT(141), (T(31)));
//G155: wff(38)              => wff_f(141).
	p(NT(38), (NT(141)));
//G156: __E_wff_49(143)      => bf(40) _(9) '<' '=' _(9) bf(40) _(9) '<' '=' _(9) bf(40).
	p(NT(143), (NT(40)+NT(9)+T(29)+T(3)+NT(9)+NT(40)+NT(9)+T(29)+T(3)+NT(9)+NT(40)));
//G157: bf_interval(142)     => __E_wff_49(143).
	p(NT(142), (NT(143)));
//G158: wff(38)              => bf_interval(142).
	p(NT(38), (NT(142)));
//G159: __E_wff_50(145)      => bf(40) _(9) '=' _(9) bf(40).
	p(NT(145), (NT(40)+NT(9)+T(3)+NT(9)+NT(40)));
//G160: bf_eq(144)           => __E_wff_50(145).
	p(NT(144), (NT(145)));
//G161: wff(38)              => bf_eq(144).
	p(NT(38), (NT(144)));
//G162: __E_wff_51(147)      => bf(40) _(9) '!' '=' _(9) bf(40).
	p(NT(147), (NT(40)+NT(9)+T(42)+T(3)+NT(9)+NT(40)));
//G163: bf_neq(146)          => __E_wff_51(147).
	p(NT(146), (NT(147)));
//G164: wff(38)              => bf_neq(146).
	p(NT(38), (NT(146)));
//G165: __E_wff_52(149)      => bf(40) _(9) '<' _(9) bf(40).
	p(NT(149), (NT(40)+NT(9)+T(29)+NT(9)+NT(40)));
//G166: bf_lt(148)           => __E_wff_52(149).
	p(NT(148), (NT(149)));
//G167: wff(38)              => bf_lt(148).
	p(NT(38), (NT(148)));
//G168: __E_wff_53(151)      => bf(40) _(9) '!' '<' _(9) bf(40).
	p(NT(151), (NT(40)+NT(9)+T(42)+T(29)+NT(9)+NT(40)));
//G169: bf_nlt(150)          => __E_wff_53(151).
	p(NT(150), (NT(151)));
//G170: wff(38)              => bf_nlt(150).
	p(NT(38), (NT(150)));
//G171: __E_wff_54(153)      => bf(40) _(9) '<' '=' _(9) bf(40).
	p(NT(153), (NT(40)+NT(9)+T(29)+T(3)+NT(9)+NT(40)));
//G172: bf_lteq(152)         => __E_wff_54(153).
	p(NT(152), (NT(153)));
//G173: wff(38)              => bf_lteq(152).
	p(NT(38), (NT(152)));
//G174: __E_wff_55(155)      => bf(40) _(9) '!' '<' '=' _(9) bf(40).
	p(NT(155), (NT(40)+NT(9)+T(42)+T(29)+T(3)+NT(9)+NT(40)));
//G175: bf_nlteq(154)        => __E_wff_55(155).
	p(NT(154), (NT(155)));
//G176: wff(38)              => bf_nlteq(154).
	p(NT(38), (NT(154)));
//G177: __E_wff_56(157)      => bf(40) _(9) '>' _(9) bf(40).
	p(NT(157), (NT(40)+NT(9)+T(30)+NT(9)+NT(40)));
//G178: bf_gt(156)           => __E_wff_56(157).
	p(NT(156), (NT(157)));
//G179: wff(38)              => bf_gt(156).
	p(NT(38), (NT(156)));
//G180: __E_wff_57(159)      => bf(40) _(9) '!' '>' _(9) bf(40).
	p(NT(159), (NT(40)+NT(9)+T(42)+T(30)+NT(9)+NT(40)));
//G181: bf_ngt(158)          => __E_wff_57(159).
	p(NT(158), (NT(159)));
//G182: wff(38)              => bf_ngt(158).
	p(NT(38), (NT(158)));
//G183: __E_wff_58(161)      => bf(40) _(9) '>' '=' _(9) bf(40).
	p(NT(161), (NT(40)+NT(9)+T(30)+T(3)+NT(9)+NT(40)));
//G184: bf_gteq(160)         => __E_wff_58(161).
	p(NT(160), (NT(161)));
//G185: wff(38)              => bf_gteq(160).
	p(NT(38), (NT(160)));
//G186: __E_wff_59(163)      => bf(40) _(9) '!' '>' '=' _(9) bf(40).
	p(NT(163), (NT(40)+NT(9)+T(42)+T(30)+T(3)+NT(9)+NT(40)));
//G187: bf_ngteq(162)        => __E_wff_59(163).
	p(NT(162), (NT(163)));
//G188: wff(38)              => bf_ngteq(162).
	p(NT(38), (NT(162)));
//G189: __E_wff_60(165)      => wff(38) _(9) 'U' _(9) wff(38).
	p(NT(165), (NT(38)+NT(9)+T(44)+NT(9)+NT(38)));
//G190: wff_U(164)           => __E_wff_60(165).
	p(NT(164), (NT(165)));
//G191: wff(38)              => wff_U(164).
	p(NT(38), (NT(164)));
//G192: __E_wff_61(167)      => wff(38) _(9) 'R' _(9) wff(38).
	p(NT(167), (NT(38)+NT(9)+T(45)+NT(9)+NT(38)));
//G193: wff_R(166)           => __E_wff_61(167).
	p(NT(166), (NT(167)));
//G194: wff(38)              => wff_R(166).
	p(NT(38), (NT(166)));
//G195: __E_wff_62(169)      => wff(38) _(9) 'W' _(9) wff(38).
	p(NT(169), (NT(38)+NT(9)+T(46)+NT(9)+NT(38)));
//G196: wff_W(168)           => __E_wff_62(169).
	p(NT(168), (NT(169)));
//G197: wff(38)              => wff_W(168).
	p(NT(38), (NT(168)));
//G198: __E_wff_63(171)      => wff(38) _(9) 'S' _(9) wff(38).
	p(NT(171), (NT(38)+NT(9)+T(47)+NT(9)+NT(38)));
//G199: wff_S(170)           => __E_wff_63(171).
	p(NT(170), (NT(171)));
//G200: wff(38)              => wff_S(170).
	p(NT(38), (NT(170)));
//G201: __E_wff_64(173)      => wff(38) _(9) 'T' _(9) wff(38).
	p(NT(173), (NT(38)+NT(9)+T(43)+NT(9)+NT(38)));
//G202: wff_T(172)           => __E_wff_64(173).
	p(NT(172), (NT(173)));
//G203: wff(38)              => wff_T(172).
	p(NT(38), (NT(172)));
//G204: wff(38)              => capture(17).
	p(NT(38), (NT(17)));
//G205: __N_0(471)           => bv_type_like(176).
	p(NT(471), (NT(176)));
//G206: __E___E_bf_65_66(175) => bf(40) & ~( __N_0(471) ).	 # conjunctive
	p(NT(175), (NT(40)) & ~(NT(471)));
//G207: __E_bf_65(174)       => '(' _(9) __E___E_bf_65_66(175) _(9) ')'.
	p(NT(174), (T(12)+NT(9)+NT(175)+NT(9)+T(13)));
//G208: bf_parenthesis(41)   => __E_bf_65(174).
	p(NT(41), (NT(174)));
//G209: bf(40)               => bf_parenthesis(41).
	p(NT(40), (NT(41)));
//G210: bf(40)               => ba_constant(177).
	p(NT(40), (NT(177)));
//G211: bf(40)               => variable(178).
	p(NT(40), (NT(178)));
//G212: __E_bf_67(180)       => 'f' 'a' 'l' 'l' __(10) q_vars(121) __(10) bf(40).
	p(NT(180), (T(9)+T(24)+T(10)+T(10)+NT(10)+NT(121)+NT(10)+NT(40)));
//G213: bf_fall(179)         => __E_bf_67(180).
	p(NT(179), (NT(180)));
//G214: bf(40)               => bf_fall(179).
	p(NT(40), (NT(179)));
//G215: __E_bf_68(182)       => 'f' 'e' 'x' __(10) q_vars(121) __(10) bf(40).
	p(NT(182), (T(9)+T(11)+T(38)+NT(10)+NT(121)+NT(10)+NT(40)));
//G216: bf_fex(181)          => __E_bf_68(182).
	p(NT(181), (NT(182)));
//G217: bf(40)               => bf_fex(181).
	p(NT(40), (NT(181)));
//G218: bf_ref(183)          => ref(65).
	p(NT(183), (NT(65)));
//G219: bf(40)               => bf_ref(183).
	p(NT(40), (NT(183)));
//G220: __E___E_bf_69_70(187) => bf_parenthesis(41).
	p(NT(187), (NT(41)));
//G221: __E___E_bf_69_70(187) => ba_constant(177).
	p(NT(187), (NT(177)));
//G222: __E___E_bf_69_70(187) => variable(178).
	p(NT(187), (NT(178)));
//G223: __E___E_bf_69_70(187) => bf_ref(183).
	p(NT(187), (NT(183)));
//G224: __E___E_bf_69_70(187) => bf_t(188).
	p(NT(187), (NT(188)));
//G225: __E___E_bf_69_70(187) => bf_f(189).
	p(NT(187), (NT(189)));
//G226: __E___E_bf_69_70(187) => bf_neg(190).
	p(NT(187), (NT(190)));
//G227: __E___E_bf_69_70(187) => bf_cast(184).
	p(NT(187), (NT(184)));
//G228: __E___E_bf_69_70(187) => capture(17).
	p(NT(187), (NT(17)));
//G229: __E___E_bf_69_70(187) => bf_fall(179).
	p(NT(187), (NT(179)));
//G230: __E___E_bf_69_70(187) => bf_fex(181).
	p(NT(187), (NT(181)));
//G231: bf_cast_oprnd(43)    => __E___E_bf_69_70(187).
	p(NT(43), (NT(187)));
//G232: __E_bf_69(185)       => '(' _(9) 'b' 'v' _(9) '[' _(9) num(186) _(9) ']' _(9) ')' _(9) bf_cast_oprnd(43).
	p(NT(185), (T(12)+NT(9)+T(25)+T(48)+NT(9)+T(20)+NT(9)+NT(186)+NT(9)+T(21)+NT(9)+T(13)+NT(9)+NT(43)));
//G233: bf_cast(184)         => __E_bf_69(185).
	p(NT(184), (NT(185)));
//G234: bf(40)               => bf_cast(184).
	p(NT(40), (NT(184)));
//G235: __E_bf_71(192)       => func_sym(193) '(' _(9) func_args(194) _(9) ')'.
	p(NT(192), (NT(193)+T(12)+NT(9)+NT(194)+NT(9)+T(13)));
//G236: bf_func_app(191)     => __E_bf_71(192).
	p(NT(191), (NT(192)));
//G237: bf(40)               => bf_func_app(191).
	p(NT(40), (NT(191)));
//G238: __N_1(472)           => bf_shr(195).
	p(NT(472), (NT(195)));
//G239: __N_2(473)           => bf_shl(198).
	p(NT(473), (NT(198)));
//G240: __E___E_bf_72_73(197) => bf(40) & ~( __N_1(472) ) & ~( __N_2(473) ).	 # conjunctive
	p(NT(197), (NT(40)) & ~(NT(472)) & ~(NT(473)));
//G241: __E_bf_72(196)       => bf(40) _(9) '>' '>' _(9) __E___E_bf_72_73(197).
	p(NT(196), (NT(40)+NT(9)+T(30)+T(30)+NT(9)+NT(197)));
//G242: bf_shr(195)          => __E_bf_72(196).
	p(NT(195), (NT(196)));
//G243: bf(40)               => bf_shr(195).
	p(NT(40), (NT(195)));
//G244: __N_3(474)           => bf_shr(195).
	p(NT(474), (NT(195)));
//G245: __N_4(475)           => bf_shl(198).
	p(NT(475), (NT(198)));
//G246: __E___E_bf_74_75(200) => bf(40) & ~( __N_3(474) ) & ~( __N_4(475) ).	 # conjunctive
	p(NT(200), (NT(40)) & ~(NT(474)) & ~(NT(475)));
//G247: __E_bf_74(199)       => bf(40) _(9) '<' '<' _(9) __E___E_bf_74_75(200).
	p(NT(199), (NT(40)+NT(9)+T(29)+T(29)+NT(9)+NT(200)));
//G248: bf_shl(198)          => __E_bf_74(199).
	p(NT(198), (NT(199)));
//G249: bf(40)               => bf_shl(198).
	p(NT(40), (NT(198)));
//G250: __N_5(476)           => bf_add(201).
	p(NT(476), (NT(201)));
//G251: __N_6(477)           => bf_sub(204).
	p(NT(477), (NT(204)));
//G252: __E___E_bf_76_77(203) => bf(40) & ~( __N_5(476) ) & ~( __N_6(477) ).	 # conjunctive
	p(NT(203), (NT(40)) & ~(NT(476)) & ~(NT(477)));
//G253: __E_bf_76(202)       => bf(40) _(9) '+' _(9) __E___E_bf_76_77(203).
	p(NT(202), (NT(40)+NT(9)+T(49)+NT(9)+NT(203)));
//G254: bf_add(201)          => __E_bf_76(202).
	p(NT(201), (NT(202)));
//G255: bf(40)               => bf_add(201).
	p(NT(40), (NT(201)));
//G256: __N_7(478)           => bf_add(201).
	p(NT(478), (NT(201)));
//G257: __N_8(479)           => bf_sub(204).
	p(NT(479), (NT(204)));
//G258: __E___E_bf_78_79(206) => bf(40) & ~( __N_7(478) ) & ~( __N_8(479) ).	 # conjunctive
	p(NT(206), (NT(40)) & ~(NT(478)) & ~(NT(479)));
//G259: __E_bf_78(205)       => bf(40) _(9) '-' _(9) __E___E_bf_78_79(206).
	p(NT(205), (NT(40)+NT(9)+T(36)+NT(9)+NT(206)));
//G260: bf_sub(204)          => __E_bf_78(205).
	p(NT(204), (NT(205)));
//G261: bf(40)               => bf_sub(204).
	p(NT(40), (NT(204)));
//G262: __N_9(480)           => bf_mod(207).
	p(NT(480), (NT(207)));
//G263: __N_10(481)          => bf_mul(210).
	p(NT(481), (NT(210)));
//G264: __N_11(482)          => bf_div(211).
	p(NT(482), (NT(211)));
//G265: __E___E_bf_80_81(209) => bf(40) & ~( __N_9(480) ) & ~( __N_10(481) ) & ~( __N_11(482) ).	 # conjunctive
	p(NT(209), (NT(40)) & ~(NT(480)) & ~(NT(481)) & ~(NT(482)));
//G266: __E_bf_80(208)       => bf(40) _(9) '%' _(9) __E___E_bf_80_81(209).
	p(NT(208), (NT(40)+NT(9)+T(50)+NT(9)+NT(209)));
//G267: bf_mod(207)          => __E_bf_80(208).
	p(NT(207), (NT(208)));
//G268: bf(40)               => bf_mod(207).
	p(NT(40), (NT(207)));
//G269: __N_12(483)          => bf_mod(207).
	p(NT(483), (NT(207)));
//G270: __N_13(484)          => bf_mul(210).
	p(NT(484), (NT(210)));
//G271: __N_14(485)          => bf_div(211).
	p(NT(485), (NT(211)));
//G272: __E___E_bf_82_83(213) => bf(40) & ~( __N_12(483) ) & ~( __N_13(484) ) & ~( __N_14(485) ).	 # conjunctive
	p(NT(213), (NT(40)) & ~(NT(483)) & ~(NT(484)) & ~(NT(485)));
//G273: __E_bf_82(212)       => bf(40) _(9) '*' _(9) __E___E_bf_82_83(213).
	p(NT(212), (NT(40)+NT(9)+T(51)+NT(9)+NT(213)));
//G274: bf_mul(210)          => __E_bf_82(212).
	p(NT(210), (NT(212)));
//G275: bf(40)               => bf_mul(210).
	p(NT(40), (NT(210)));
//G276: __N_15(486)          => bf_mod(207).
	p(NT(486), (NT(207)));
//G277: __N_16(487)          => bf_mul(210).
	p(NT(487), (NT(210)));
//G278: __N_17(488)          => bf_div(211).
	p(NT(488), (NT(211)));
//G279: __E___E_bf_84_85(215) => bf(40) & ~( __N_15(486) ) & ~( __N_16(487) ) & ~( __N_17(488) ).	 # conjunctive
	p(NT(215), (NT(40)) & ~(NT(486)) & ~(NT(487)) & ~(NT(488)));
//G280: __E_bf_84(214)       => bf(40) _(9) '/' _(9) __E___E_bf_84_85(215).
	p(NT(214), (NT(40)+NT(9)+T(16)+NT(9)+NT(215)));
//G281: bf_div(211)          => __E_bf_84(214).
	p(NT(211), (NT(214)));
//G282: bf(40)               => bf_div(211).
	p(NT(40), (NT(211)));
//G283: __N_18(489)          => bf_nor(216).
	p(NT(489), (NT(216)));
//G284: __E___E_bf_86_87(218) => bf(40) & ~( __N_18(489) ).	 # conjunctive
	p(NT(218), (NT(40)) & ~(NT(489)));
//G285: __E_bf_86(217)       => bf(40) _(9) '!' '|' _(9) __E___E_bf_86_87(218).
	p(NT(217), (NT(40)+NT(9)+T(42)+T(39)+NT(9)+NT(218)));
//G286: bf_nor(216)          => __E_bf_86(217).
	p(NT(216), (NT(217)));
//G287: bf(40)               => bf_nor(216).
	p(NT(40), (NT(216)));
//G288: __N_19(490)          => bf_xnor(219).
	p(NT(490), (NT(219)));
//G289: __E___E_bf_88_89(221) => bf(40) & ~( __N_19(490) ).	 # conjunctive
	p(NT(221), (NT(40)) & ~(NT(490)));
//G290: __E_bf_88(220)       => bf(40) _(9) '!' '^' _(9) __E___E_bf_88_89(221).
	p(NT(220), (NT(40)+NT(9)+T(42)+T(40)+NT(9)+NT(221)));
//G291: bf_xnor(219)         => __E_bf_88(220).
	p(NT(219), (NT(220)));
//G292: bf(40)               => bf_xnor(219).
	p(NT(40), (NT(219)));
//G293: __N_20(491)          => bf_nand(222).
	p(NT(491), (NT(222)));
//G294: __E___E_bf_90_91(224) => bf(40) & ~( __N_20(491) ).	 # conjunctive
	p(NT(224), (NT(40)) & ~(NT(491)));
//G295: __E_bf_90(223)       => bf(40) _(9) '!' '&' _(9) __E___E_bf_90_91(224).
	p(NT(223), (NT(40)+NT(9)+T(42)+T(41)+NT(9)+NT(224)));
//G296: bf_nand(222)         => __E_bf_90(223).
	p(NT(222), (NT(223)));
//G297: bf(40)               => bf_nand(222).
	p(NT(40), (NT(222)));
//G298: __E_bf_92(226)       => bf(40) _(9) '|' _(9) bf(40).
	p(NT(226), (NT(40)+NT(9)+T(39)+NT(9)+NT(40)));
//G299: bf_or(225)           => __E_bf_92(226).
	p(NT(225), (NT(226)));
//G300: bf(40)               => bf_or(225).
	p(NT(40), (NT(225)));
//G301: __E_bf_93(228)       => bf(40) _(9) '^' _(9) bf(40).
	p(NT(228), (NT(40)+NT(9)+T(40)+NT(9)+NT(40)));
//G302: bf_xor(227)          => __E_bf_93(228).
	p(NT(227), (NT(228)));
//G303: bf(40)               => bf_xor(227).
	p(NT(40), (NT(227)));
//G304: __E___E_bf_94_95(230) => typed(70).
	p(NT(230), (NT(70)));
//G305: __E___E_bf_94_95(230) => null.
	p(NT(230), (nul));
//G306: __E_bf_94(229)       => '1' __E___E_bf_94_95(230).
	p(NT(229), (T(52)+NT(230)));
//G307: bf_t(188)            => __E_bf_94(229).
	p(NT(188), (NT(229)));
//G308: bf(40)               => bf_t(188).
	p(NT(40), (NT(188)));
//G309: __E___E_bf_96_97(232) => typed(70).
	p(NT(232), (NT(70)));
//G310: __E___E_bf_96_97(232) => null.
	p(NT(232), (nul));
//G311: __E_bf_96(231)       => '0' __E___E_bf_96_97(232).
	p(NT(231), (T(53)+NT(232)));
//G312: bf_f(189)            => __E_bf_96(231).
	p(NT(189), (NT(231)));
//G313: bf(40)               => bf_f(189).
	p(NT(40), (NT(189)));
//G314: __E___E_bf_98_99(234) => _(9) '&' _(9).
	p(NT(234), (NT(9)+T(41)+NT(9)));
//G315: __E___E_bf_98_99(234) => __(10).
	p(NT(234), (NT(10)));
//G316: __E_bf_98(233)       => bf(40) __E___E_bf_98_99(234) bf(40).
	p(NT(233), (NT(40)+NT(234)+NT(40)));
//G317: bf_and(47)           => __E_bf_98(233).
	p(NT(47), (NT(233)));
//G318: bf(40)               => bf_and(47).
	p(NT(40), (NT(47)));
//G319: __E___E_bf_100_101(236) => bf_parenthesis(41).
	p(NT(236), (NT(41)));
//G320: __E___E_bf_100_101(236) => ba_constant(177).
	p(NT(236), (NT(177)));
//G321: __E___E_bf_100_101(236) => variable(178).
	p(NT(236), (NT(178)));
//G322: __E___E_bf_100_101(236) => bf_ref(183).
	p(NT(236), (NT(183)));
//G323: __E___E_bf_100_101(236) => bf_t(188).
	p(NT(236), (NT(188)));
//G324: __E___E_bf_100_101(236) => bf_f(189).
	p(NT(236), (NT(189)));
//G325: __E___E_bf_100_101(236) => bf_neg(190).
	p(NT(236), (NT(190)));
//G326: __E___E_bf_100_101(236) => capture(17).
	p(NT(236), (NT(17)));
//G327: bf_neg_oprnd(42)     => __E___E_bf_100_101(236).
	p(NT(42), (NT(236)));
//G328: __E_bf_100(235)      => bf_neg_oprnd(42) _(9) '\''.
	p(NT(235), (NT(42)+NT(9)+T(54)));
//G329: bf_neg(190)          => __E_bf_100(235).
	p(NT(190), (NT(235)));
//G330: bf(40)               => bf_neg(190).
	p(NT(40), (NT(190)));
//G331: __E___E_bf_102_103(238) => bf_parenthesis(41).
	p(NT(238), (NT(41)));
//G332: __E___E_bf_102_103(238) => variable(178).
	p(NT(238), (NT(178)));
//G333: __E___E_bf_102_103(238) => bf_ref(183).
	p(NT(238), (NT(183)));
//G334: __E___E_bf_102_103(238) => bf_neg(190).
	p(NT(238), (NT(190)));
//G335: __E___E_bf_102_103(238) => bf_and_nosep(46).
	p(NT(238), (NT(46)));
//G336: __E___E_bf_102_103(238) => capture(17).
	p(NT(238), (NT(17)));
//G337: bf_and_nosep_1st_oprnd(44) => __E___E_bf_102_103(238).
	p(NT(44), (NT(238)));
//G338: __E___E_bf_102_104(239) => bf_parenthesis(41).
	p(NT(239), (NT(41)));
//G339: __E___E_bf_102_104(239) => ba_constant(177).
	p(NT(239), (NT(177)));
//G340: __E___E_bf_102_104(239) => variable(178).
	p(NT(239), (NT(178)));
//G341: __E___E_bf_102_104(239) => bf_ref(183).
	p(NT(239), (NT(183)));
//G342: __E___E_bf_102_104(239) => bf_neg(190).
	p(NT(239), (NT(190)));
//G343: __E___E_bf_102_104(239) => capture(17).
	p(NT(239), (NT(17)));
//G344: bf_and_nosep_2nd_oprnd(45) => __E___E_bf_102_104(239).
	p(NT(45), (NT(239)));
//G345: __E_bf_102(237)      => bf_and_nosep_1st_oprnd(44) bf_and_nosep_2nd_oprnd(45).
	p(NT(237), (NT(44)+NT(45)));
//G346: bf_and_nosep(46)     => __E_bf_102(237).
	p(NT(46), (NT(237)));
//G347: bf(40)               => bf_and_nosep(46).
	p(NT(40), (NT(46)));
//G348: bf(40)               => capture(17).
	p(NT(40), (NT(17)));
//G349: func_sym(193)        => chars(27).
	p(NT(193), (NT(27)));
//G350: __E_func_args_105(240) => _(9) ',' _(9) bf(40).
	p(NT(240), (NT(9)+T(19)+NT(9)+NT(40)));
//G351: __E_func_args_106(241) => null.
	p(NT(241), (nul));
//G352: __E_func_args_106(241) => __E_func_args_105(240) __E_func_args_106(241).
	p(NT(241), (NT(240)+NT(241)));
//G353: func_args(194)       => bf(40) __E_func_args_106(241).
	p(NT(194), (NT(40)+NT(241)));
//G354: func_dynamic(243)    => 'd' 'y' 'n' 'a' 'm' 'i' 'c'.
	p(NT(243), (T(55)+T(17)+T(5)+T(24)+T(28)+T(4)+T(14)));
//G355: func_mode(242)       => func_dynamic(243).
	p(NT(242), (NT(243)));
//G356: func_static(244)     => 's' 't' 'a' 't' 'i' 'c'.
	p(NT(244), (T(15)+T(8)+T(24)+T(8)+T(4)+T(14)));
//G357: func_mode(242)       => func_static(244).
	p(NT(242), (NT(244)));
//G358: __E_func_sort_list_107(246) => _(9) ',' _(9) type(13).
	p(NT(246), (NT(9)+T(19)+NT(9)+NT(13)));
//G359: __E_func_sort_list_108(247) => null.
	p(NT(247), (nul));
//G360: __E_func_sort_list_108(247) => __E_func_sort_list_107(246) __E_func_sort_list_108(247).
	p(NT(247), (NT(246)+NT(247)));
//G361: func_sort_list(245)  => type(13) __E_func_sort_list_108(247).
	p(NT(245), (NT(13)+NT(247)));
//G362: func_decl(248)       => func_mode(242) __(10) func_sym(193) _(9) ':' _(9) func_sort_list(245) _(9) '-' '>' _(9) type(13).
	p(NT(248), (NT(242)+NT(10)+NT(193)+NT(9)+T(2)+NT(9)+NT(245)+NT(9)+T(36)+T(30)+NT(9)+NT(13)));
//G363: __E_constraint_109(250) => '[' ctnvar(22) _(9) '!' '=' _(9) num(186) ']'.
	p(NT(250), (T(20)+NT(22)+NT(9)+T(42)+T(3)+NT(9)+NT(186)+T(21)));
//G364: __E_constraint_109(250) => '[' num(186) _(9) '!' '=' _(9) ctnvar(22) ']'.
	p(NT(250), (T(20)+NT(186)+NT(9)+T(42)+T(3)+NT(9)+NT(22)+T(21)));
//G365: ctn_neq(249)         => __E_constraint_109(250).
	p(NT(249), (NT(250)));
//G366: constraint(125)      => ctn_neq(249).
	p(NT(125), (NT(249)));
//G367: __E_constraint_110(252) => '[' ctnvar(22) _(9) '=' _(9) num(186) ']'.
	p(NT(252), (T(20)+NT(22)+NT(9)+T(3)+NT(9)+NT(186)+T(21)));
//G368: __E_constraint_110(252) => '[' num(186) _(9) '=' _(9) ctnvar(22) ']'.
	p(NT(252), (T(20)+NT(186)+NT(9)+T(3)+NT(9)+NT(22)+T(21)));
//G369: ctn_eq(251)          => __E_constraint_110(252).
	p(NT(251), (NT(252)));
//G370: constraint(125)      => ctn_eq(251).
	p(NT(125), (NT(251)));
//G371: __E_constraint_111(254) => '[' ctnvar(22) _(9) '>' '=' _(9) num(186) ']'.
	p(NT(254), (T(20)+NT(22)+NT(9)+T(30)+T(3)+NT(9)+NT(186)+T(21)));
//G372: __E_constraint_111(254) => '[' num(186) _(9) '>' '=' _(9) ctnvar(22) ']'.
	p(NT(254), (T(20)+NT(186)+NT(9)+T(30)+T(3)+NT(9)+NT(22)+T(21)));
//G373: ctn_gteq(253)        => __E_constraint_111(254).
	p(NT(253), (NT(254)));
//G374: constraint(125)      => ctn_gteq(253).
	p(NT(125), (NT(253)));
//G375: __E_constraint_112(256) => '[' ctnvar(22) _(9) '>' _(9) num(186) ']'.
	p(NT(256), (T(20)+NT(22)+NT(9)+T(30)+NT(9)+NT(186)+T(21)));
//G376: __E_constraint_112(256) => '[' num(186) _(9) '>' _(9) ctnvar(22) ']'.
	p(NT(256), (T(20)+NT(186)+NT(9)+T(30)+NT(9)+NT(22)+T(21)));
//G377: ctn_gt(255)          => __E_constraint_112(256).
	p(NT(255), (NT(256)));
//G378: constraint(125)      => ctn_gt(255).
	p(NT(125), (NT(255)));
//G379: __E_constraint_113(258) => '[' ctnvar(22) _(9) '<' '=' _(9) num(186) ']'.
	p(NT(258), (T(20)+NT(22)+NT(9)+T(29)+T(3)+NT(9)+NT(186)+T(21)));
//G380: __E_constraint_113(258) => '[' num(186) _(9) '<' '=' _(9) ctnvar(22) ']'.
	p(NT(258), (T(20)+NT(186)+NT(9)+T(29)+T(3)+NT(9)+NT(22)+T(21)));
//G381: ctn_lteq(257)        => __E_constraint_113(258).
	p(NT(257), (NT(258)));
//G382: constraint(125)      => ctn_lteq(257).
	p(NT(125), (NT(257)));
//G383: __E_constraint_114(260) => '[' ctnvar(22) _(9) '<' _(9) num(186) ']'.
	p(NT(260), (T(20)+NT(22)+NT(9)+T(29)+NT(9)+NT(186)+T(21)));
//G384: __E_constraint_114(260) => '[' num(186) _(9) '<' _(9) ctnvar(22) ']'.
	p(NT(260), (T(20)+NT(186)+NT(9)+T(29)+NT(9)+NT(22)+T(21)));
//G385: ctn_lt(259)          => __E_constraint_114(260).
	p(NT(259), (NT(260)));
//G386: constraint(125)      => ctn_lt(259).
	p(NT(125), (NT(259)));
//G387: __E_ba_constant_115(261) => capture(17).
	p(NT(261), (NT(17)));
//G388: __E_ba_constant_115(261) => source(16).
	p(NT(261), (NT(16)));
//G389: __E_ba_constant_116(262) => typed(70).
	p(NT(262), (NT(70)));
//G390: __E_ba_constant_116(262) => null.
	p(NT(262), (nul));
//G391: ba_constant(177)     => '{' _(9) __E_ba_constant_115(261) _(9) '}' __E_ba_constant_116(262).
	p(NT(177), (T(22)+NT(9)+NT(261)+NT(9)+T(23)+NT(262)));
//G392: __E___E_source_117_118(264) => src_c(29).
	p(NT(264), (NT(29)));
//G393: __E___E_source_117_118(264) => space(2).
	p(NT(264), (NT(2)));
//G394: __E___E_source_117_119(265) => null.
	p(NT(265), (nul));
//G395: __E___E_source_117_119(265) => __E___E_source_117_118(264) __E___E_source_117_119(265).
	p(NT(265), (NT(264)+NT(265)));
//G396: __E_source_117(263)  => __E___E_source_117_119(265) src_c(29).
	p(NT(263), (NT(265)+NT(29)));
//G397: __E_source_117(263)  => null.
	p(NT(263), (nul));
//G398: source(16)           => src_c(29) __E_source_117(263).
	p(NT(16), (NT(29)+NT(263)));
//G399: src_c(29)            => alnum(6).
	p(NT(29), (NT(6)));
//G400: __N_21(492)          => '{'.
	p(NT(492), (T(22)));
//G401: __N_22(493)          => '}'.
	p(NT(493), (T(23)));
//G402: src_c(29)            => ~( __N_21(492) ) & ~( __N_22(493) ) & punct(7).	 # conjunctive
	p(NT(29), ~(NT(492)) & ~(NT(493)) & (NT(7)));
//G403: __E_src_c_120(266)   => src_c(29).
	p(NT(266), (NT(29)));
//G404: __E_src_c_120(266)   => space(2).
	p(NT(266), (NT(2)));
//G405: __E_src_c_121(267)   => null.
	p(NT(267), (nul));
//G406: __E_src_c_121(267)   => __E_src_c_120(266) __E_src_c_121(267).
	p(NT(267), (NT(266)+NT(267)));
//G407: src_c(29)            => '{' __E_src_c_121(267) '}'.
	p(NT(29), (T(22)+NT(267)+T(23)));
//G408: __E_variable_122(268) => uconst(30).
	p(NT(268), (NT(30)));
//G409: __E_variable_122(268) => io_var(269).
	p(NT(268), (NT(269)));
//G410: __E_variable_122(268) => var_name(18).
	p(NT(268), (NT(18)));
//G411: __E_variable_123(270) => member_path(68).
	p(NT(270), (NT(68)));
//G412: __E_variable_123(270) => null.
	p(NT(270), (nul));
//G413: __E_variable_124(271) => typed(70).
	p(NT(271), (NT(70)));
//G414: __E_variable_124(271) => null.
	p(NT(271), (nul));
//G415: variable(178)        => __E_variable_122(268) __E_variable_123(270) __E_variable_124(271).
	p(NT(178), (NT(268)+NT(270)+NT(271)));
//G416: __N_23(494)          => 'F'.
	p(NT(494), (T(31)));
//G417: __N_24(495)          => 'T'.
	p(NT(495), (T(43)));
//G418: __E_var_name_125(272) => ~( __N_23(494) ) & ~( __N_24(495) ) & alpha(5).	 # conjunctive
	p(NT(272), ~(NT(494)) & ~(NT(495)) & (NT(5)));
//G419: __E_var_name_126(273) => null.
	p(NT(273), (nul));
//G420: __E_var_name_126(273) => digit(3) __E_var_name_126(273).
	p(NT(273), (NT(3)+NT(273)));
//G421: var_name(18)         => __E_var_name_125(272) __E_var_name_126(273).	 # guarded: charvar
	p(NT(18), (NT(272)+NT(273)));
	p.back().guard = "charvar";
//G422: __N_25(496)          => 'F'.
	p(NT(496), (T(31)));
//G423: __N_26(497)          => 'T'.
	p(NT(497), (T(43)));
//G424: __E_var_name_127(274) => ~( __N_25(496) ) & ~( __N_26(497) ) & alpha(5).	 # conjunctive
	p(NT(274), ~(NT(496)) & ~(NT(497)) & (NT(5)));
//G425: __E_var_name_128(275) => alnum(6).
	p(NT(275), (NT(6)));
//G426: __E_var_name_128(275) => '_'.
	p(NT(275), (T(56)));
//G427: __E_var_name_129(276) => null.
	p(NT(276), (nul));
//G428: __E_var_name_129(276) => __E_var_name_128(275) __E_var_name_129(276).
	p(NT(276), (NT(275)+NT(276)));
//G429: var_name(18)         => __E_var_name_127(274) __E_var_name_129(276).	 # guarded: var
	p(NT(18), (NT(274)+NT(276)));
	p.back().guard = "var";
//G430: io_var(269)          => io_var_name(19) '[' offset(277) ']'.
	p(NT(269), (NT(19)+T(20)+NT(277)+T(21)));
//G431: io_var_name(19)      => chars(27).
	p(NT(19), (NT(27)));
//G432: bv_type_like(176)    => 'b' 'v' _(9) '[' _(9) num(186) _(9) ']'.
	p(NT(176), (T(25)+T(48)+NT(9)+T(20)+NT(9)+NT(186)+NT(9)+T(21)));
//G433: __E___E_uconst_130_131(279) => chars(27) _(9).
	p(NT(279), (NT(27)+NT(9)));
//G434: __E___E_uconst_130_131(279) => null.
	p(NT(279), (nul));
//G435: __E_uconst_130(278)  => __E___E_uconst_130_131(279) ':' _(9) chars(27).
	p(NT(278), (NT(279)+T(2)+NT(9)+NT(27)));
//G436: uconst_name(20)      => __E_uconst_130(278).
	p(NT(20), (NT(278)));
//G437: uconst(30)           => '<' _(9) uconst_name(20) _(9) '>'.
	p(NT(30), (T(29)+NT(9)+NT(20)+NT(9)+T(30)));
//G438: __E_q_vars_132(280)  => _(9) ',' _(9) q_var(28).
	p(NT(280), (NT(9)+T(19)+NT(9)+NT(28)));
//G439: __E_q_vars_133(281)  => null.
	p(NT(281), (nul));
//G440: __E_q_vars_133(281)  => __E_q_vars_132(280) __E_q_vars_133(281).
	p(NT(281), (NT(280)+NT(281)));
//G441: q_vars(121)          => q_var(28) __E_q_vars_133(281).
	p(NT(121), (NT(28)+NT(281)));
//G442: q_var(28)            => capture(17).
	p(NT(28), (NT(17)));
//G443: __N_27(498)          => uconst(30).
	p(NT(498), (NT(30)));
//G444: q_var(28)            => ~( __N_27(498) ) & variable(178).	 # conjunctive
	p(NT(28), ~(NT(498)) & (NT(178)));
//G445: ctnvar(22)           => chars(27).
	p(NT(22), (NT(27)));
//G446: __E_offsets_134(282) => _(9) ',' _(9) offset(277).
	p(NT(282), (NT(9)+T(19)+NT(9)+NT(277)));
//G447: __E_offsets_135(283) => null.
	p(NT(283), (nul));
//G448: __E_offsets_135(283) => __E_offsets_134(282) __E_offsets_135(283).
	p(NT(283), (NT(282)+NT(283)));
//G449: offsets(92)          => '[' _(9) offset(277) __E_offsets_135(283) _(9) ']'.
	p(NT(92), (T(20)+NT(9)+NT(277)+NT(283)+NT(9)+T(21)));
//G450: offset(277)          => integer(15).
	p(NT(277), (NT(15)));
//G451: offset(277)          => capture(17).
	p(NT(277), (NT(17)));
//G452: offset(277)          => shift(284).
	p(NT(277), (NT(284)));
//G453: __N_28(499)          => io_var(269).
	p(NT(499), (NT(269)));
//G454: __E_offset_136(285)  => variable(178) & ~( __N_28(499) ).	 # conjunctive
	p(NT(285), (NT(178)) & ~(NT(499)));
//G455: offset(277)          => __E_offset_136(285).
	p(NT(277), (NT(285)));
//G456: __E_shift_137(286)   => capture(17).
	p(NT(286), (NT(17)));
//G457: __N_29(500)          => io_var(269).
	p(NT(500), (NT(269)));
//G458: __E___E_shift_137_138(287) => variable(178) & ~( __N_29(500) ).	 # conjunctive
	p(NT(287), (NT(178)) & ~(NT(500)));
//G459: __E_shift_137(286)   => __E___E_shift_137_138(287).
	p(NT(286), (NT(287)));
//G460: shift(284)           => __E_shift_137(286) _(9) '-' _(9) num(186).
	p(NT(284), (NT(286)+NT(9)+T(36)+NT(9)+NT(186)));
//G461: __E_chars_139(288)   => alnum(6).
	p(NT(288), (NT(6)));
//G462: __E_chars_139(288)   => '_'.
	p(NT(288), (T(56)));
//G463: __E_chars_140(289)   => null.
	p(NT(289), (nul));
//G464: __E_chars_140(289)   => __E_chars_139(288) __E_chars_140(289).
	p(NT(289), (NT(288)+NT(289)));
//G465: chars(27)            => alpha(5) __E_chars_140(289).
	p(NT(27), (NT(5)+NT(289)));
//G466: __E_digits_141(290)  => digit(3).
	p(NT(290), (NT(3)));
//G467: __E_digits_141(290)  => digit(3) __E_digits_141(290).
	p(NT(290), (NT(3)+NT(290)));
//G468: digits(12)           => __E_digits_141(290).
	p(NT(12), (NT(290)));
//G469: num(186)             => digits(12).
	p(NT(186), (NT(12)));
//G470: __E_integer_142(291) => '-'.
	p(NT(291), (T(36)));
//G471: __E_integer_142(291) => null.
	p(NT(291), (nul));
//G472: integer(15)          => __E_integer_142(291) _(9) digits(12).
	p(NT(15), (NT(291)+NT(9)+NT(12)));
//G473: sym(11)              => chars(27).
	p(NT(11), (NT(27)));
//G474: capture(17)          => '$' chars(27).
	p(NT(17), (T(57)+NT(27)));
//G475: typed(70)            => _(9) ':' _(9) type(13) _(9) '[' _(9) subtype(14) _(9) ']'.
	p(NT(70), (NT(9)+T(2)+NT(9)+NT(13)+NT(9)+T(20)+NT(9)+NT(14)+NT(9)+T(21)));
//G476: __N_30(501)          => 'b' 'v'.
	p(NT(501), (T(25)+T(48)));
//G477: __E_typed_143(292)   => ~( __N_30(501) ) & type(13).	 # conjunctive
	p(NT(292), ~(NT(501)) & (NT(13)));
//G478: typed(70)            => _(9) ':' _(9) __E_typed_143(292).
	p(NT(70), (NT(9)+T(2)+NT(9)+NT(292)));
//G479: type(13)             => chars(27).
	p(NT(13), (NT(27)));
//G480: subtype(14)          => num(186).
	p(NT(14), (NT(186)));
//G481: __E_comment_144(294) => printable(8).
	p(NT(294), (NT(8)));
//G482: __E_comment_144(294) => '\t'.
	p(NT(294), (T(58)));
//G483: __E_comment_145(295) => null.
	p(NT(295), (nul));
//G484: __E_comment_145(295) => __E_comment_144(294) __E_comment_145(295).
	p(NT(295), (NT(294)+NT(295)));
//G485: __E_comment_146(296) => '\n'.
	p(NT(296), (T(59)));
//G486: __E_comment_146(296) => '\r'.
	p(NT(296), (T(60)));
//G487: __E_comment_146(296) => eof(1).
	p(NT(296), (NT(1)));
//G488: comment(293)         => '#' __E_comment_145(295) __E_comment_146(296).
	p(NT(293), (T(61)+NT(295)+NT(296)));
//G489: __(10)               => space(2).
	p(NT(10), (NT(2)));
//G490: __(10)               => comment(293).
	p(NT(10), (NT(293)));
//G491: __(10)               => __(10) space(2).
	p(NT(10), (NT(10)+NT(2)));
//G492: __(10)               => __(10) comment(293).
	p(NT(10), (NT(10)+NT(293)));
//G493: _(9)                 => __(10).
	p(NT(9), (NT(10)));
//G494: _(9)                 => null.
	p(NT(9), (nul));
//G495: cli(297)             => _(9).
	p(NT(297), (NT(9)));
//G496: __E_cli_147(299)     => '.' _(9) cli_command(298) _(9).
	p(NT(299), (T(1)+NT(9)+NT(298)+NT(9)));
//G497: __E_cli_148(300)     => null.
	p(NT(300), (nul));
//G498: __E_cli_148(300)     => __E_cli_147(299) __E_cli_148(300).
	p(NT(300), (NT(299)+NT(300)));
//G499: __E_cli_149(301)     => '.' _(9).
	p(NT(301), (T(1)+NT(9)));
//G500: __E_cli_149(301)     => null.
	p(NT(301), (nul));
//G501: cli(297)             => _(9) cli_command(298) _(9) __E_cli_148(300) __E_cli_149(301).
	p(NT(297), (NT(9)+NT(298)+NT(9)+NT(300)+NT(301)));
//G502: __E_cli_command_150(304) => 'q'.
	p(NT(304), (T(62)));
//G503: __E_cli_command_150(304) => 'q' 'u' 'i' 't'.
	p(NT(304), (T(62)+T(7)+T(4)+T(8)));
//G504: quit_sym(303)        => __E_cli_command_150(304).
	p(NT(303), (NT(304)));
//G505: quit_cmd(302)        => quit_sym(303).
	p(NT(302), (NT(303)));
//G506: cli_command(298)     => quit_cmd(302).
	p(NT(298), (NT(302)));
//G507: __E_cli_command_151(307) => 'v'.
	p(NT(307), (T(48)));
//G508: __E_cli_command_151(307) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(307), (T(48)+T(11)+T(27)+T(15)+T(4)+T(6)+T(5)));
//G509: version_sym(306)     => __E_cli_command_151(307).
	p(NT(306), (NT(307)));
//G510: version_cmd(305)     => version_sym(306).
	p(NT(305), (NT(306)));
//G511: cli_command(298)     => version_cmd(305).
	p(NT(298), (NT(305)));
//G512: __E_cli_command_152(310) => 'c'.
	p(NT(310), (T(14)));
//G513: __E_cli_command_152(310) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(310), (T(14)+T(10)+T(11)+T(24)+T(27)));
//G514: clear_sym(309)       => __E_cli_command_152(310).
	p(NT(309), (NT(310)));
//G515: clear_cmd(308)       => clear_sym(309).
	p(NT(308), (NT(309)));
//G516: cli_command(298)     => clear_cmd(308).
	p(NT(298), (NT(308)));
//G517: __E___E_cli_command_153_154(314) => 'h'.
	p(NT(314), (T(63)));
//G518: __E___E_cli_command_153_154(314) => 'h' 'e' 'l' 'p'.
	p(NT(314), (T(63)+T(11)+T(10)+T(18)));
//G519: help_sym(313)        => __E___E_cli_command_153_154(314).
	p(NT(313), (NT(314)));
//G520: __E___E_cli_command_153_155(315) => __(10) help_arg(316).
	p(NT(315), (NT(10)+NT(316)));
//G521: __E___E_cli_command_153_155(315) => null.
	p(NT(315), (nul));
//G522: __E_cli_command_153(312) => help_sym(313) __E___E_cli_command_153_155(315).
	p(NT(312), (NT(313)+NT(315)));
//G523: help_cmd(311)        => __E_cli_command_153(312).
	p(NT(311), (NT(312)));
//G524: cli_command(298)     => help_cmd(311).
	p(NT(298), (NT(311)));
//G525: file_sym(319)        => 'f' 'i' 'l' 'e'.
	p(NT(319), (T(9)+T(4)+T(10)+T(11)));
//G526: __E_cli_command_156(318) => file_sym(319) __(10) q_string(320).
	p(NT(318), (NT(319)+NT(10)+NT(320)));
//G527: file_cmd(317)        => __E_cli_command_156(318).
	p(NT(317), (NT(318)));
//G528: cli_command(298)     => file_cmd(317).
	p(NT(298), (NT(317)));
//G529: valid_sym(323)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(323), (T(48)+T(24)+T(10)+T(4)+T(55)));
//G530: __E_cli_command_157(322) => valid_sym(323) __(10) spec_arg(37).
	p(NT(322), (NT(323)+NT(10)+NT(37)));
//G531: valid_cmd(321)       => __E_cli_command_157(322).
	p(NT(321), (NT(322)));
//G532: cli_command(298)     => valid_cmd(321).
	p(NT(298), (NT(321)));
//G533: sat_sym(326)         => 's' 'a' 't'.
	p(NT(326), (T(15)+T(24)+T(8)));
//G534: __E_cli_command_158(325) => sat_sym(326) __(10) spec_arg(37).
	p(NT(325), (NT(326)+NT(10)+NT(37)));
//G535: sat_cmd(324)         => __E_cli_command_158(325).
	p(NT(324), (NT(325)));
//G536: cli_command(298)     => sat_cmd(324).
	p(NT(298), (NT(324)));
//G537: unsat_sym(329)       => 'u' 'n' 's' 'a' 't'.
	p(NT(329), (T(7)+T(5)+T(15)+T(24)+T(8)));
//G538: __E_cli_command_159(328) => unsat_sym(329) __(10) spec_arg(37).
	p(NT(328), (NT(329)+NT(10)+NT(37)));
//G539: unsat_cmd(327)       => __E_cli_command_159(328).
	p(NT(327), (NT(328)));
//G540: cli_command(298)     => unsat_cmd(327).
	p(NT(298), (NT(327)));
//G541: realizable_sym(332)  => 'r' 'e' 'a' 'l' 'i' 'z' 'a' 'b' 'l' 'e'.
	p(NT(332), (T(27)+T(11)+T(24)+T(10)+T(4)+T(64)+T(24)+T(25)+T(10)+T(11)));
//G542: __E_cli_command_160(331) => realizable_sym(332) __(10) spec_arg(37).
	p(NT(331), (NT(332)+NT(10)+NT(37)));
//G543: realizable_cmd(330)  => __E_cli_command_160(331).
	p(NT(330), (NT(331)));
//G544: cli_command(298)     => realizable_cmd(330).
	p(NT(298), (NT(330)));
//G545: unrealizable_sym(335) => 'u' 'n' 'r' 'e' 'a' 'l' 'i' 'z' 'a' 'b' 'l' 'e'.
	p(NT(335), (T(7)+T(5)+T(27)+T(11)+T(24)+T(10)+T(4)+T(64)+T(24)+T(25)+T(10)+T(11)));
//G546: __E_cli_command_161(334) => unrealizable_sym(335) __(10) spec_arg(37).
	p(NT(334), (NT(335)+NT(10)+NT(37)));
//G547: unrealizable_cmd(333) => __E_cli_command_161(334).
	p(NT(333), (NT(334)));
//G548: cli_command(298)     => unrealizable_cmd(333).
	p(NT(298), (NT(333)));
//G549: solve_sym(338)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(338), (T(15)+T(6)+T(10)+T(48)+T(11)));
//G550: __E___E_cli_command_162_163(339) => solve_options(340).
	p(NT(339), (NT(340)));
//G551: __E___E_cli_command_162_163(339) => null.
	p(NT(339), (nul));
//G552: __E_cli_command_162(337) => solve_sym(338) __E___E_cli_command_162_163(339) __(10) wff_cmd_arg(34).
	p(NT(337), (NT(338)+NT(339)+NT(10)+NT(34)));
//G553: solve_cmd(336)       => __E_cli_command_162(337).
	p(NT(336), (NT(337)));
//G554: cli_command(298)     => solve_cmd(336).
	p(NT(298), (NT(336)));
//G555: lgrs_sym(343)        => 'l' 'g' 'r' 's'.
	p(NT(343), (T(10)+T(65)+T(27)+T(15)));
//G556: __E_cli_command_164(342) => lgrs_sym(343) __(10) wff_cmd_arg(34).
	p(NT(342), (NT(343)+NT(10)+NT(34)));
//G557: lgrs_cmd(341)        => __E_cli_command_164(342).
	p(NT(341), (NT(342)));
//G558: cli_command(298)     => lgrs_cmd(341).
	p(NT(298), (NT(341)));
//G559: __E___E_cli_command_165_166(347) => 'r'.
	p(NT(347), (T(27)));
//G560: __E___E_cli_command_165_166(347) => 'r' 'u' 'n'.
	p(NT(347), (T(27)+T(7)+T(5)));
//G561: run_sym(346)         => __E___E_cli_command_165_166(347).
	p(NT(346), (NT(347)));
//G562: __E___E___E_cli_command_165_167_168(349) => 's' 't' 'e' 'p' 's'.
	p(NT(349), (T(15)+T(8)+T(11)+T(18)+T(15)));
//G563: __E___E___E_cli_command_165_167_168(349) => 's' 't' 'e' 'p'.
	p(NT(349), (T(15)+T(8)+T(11)+T(18)));
//G564: __E___E_cli_command_165_167(348) => __(10) num(186) __(10) __E___E___E_cli_command_165_167_168(349).
	p(NT(348), (NT(10)+NT(186)+NT(10)+NT(349)));
//G565: __E___E_cli_command_165_167(348) => null.
	p(NT(348), (nul));
//G566: __E___E_cli_command_165_169(350) => __(10) spec_arg(37).
	p(NT(350), (NT(10)+NT(37)));
//G567: __E___E_cli_command_165_169(350) => null.
	p(NT(350), (nul));
//G568: __E_cli_command_165(345) => run_sym(346) __E___E_cli_command_165_167(348) __E___E_cli_command_165_169(350).
	p(NT(345), (NT(346)+NT(348)+NT(350)));
//G569: run_cmd(344)         => __E_cli_command_165(345).
	p(NT(344), (NT(345)));
//G570: cli_command(298)     => run_cmd(344).
	p(NT(298), (NT(344)));
//G571: stop_sym(353)        => 's' 't' 'o' 'p'.
	p(NT(353), (T(15)+T(8)+T(6)+T(18)));
//G572: __E_cli_command_170(352) => stop_sym(353).
	p(NT(352), (NT(353)));
//G573: stop_cmd(351)        => __E_cli_command_170(352).
	p(NT(351), (NT(352)));
//G574: cli_command(298)     => stop_cmd(351).
	p(NT(298), (NT(351)));
//G575: memory_sym(356)      => 'm' 'e' 'm' 'o' 'r' 'y'.
	p(NT(356), (T(28)+T(11)+T(28)+T(6)+T(27)+T(17)));
//G576: __E_cli_command_171(355) => memory_sym(356).
	p(NT(355), (NT(356)));
//G577: memory_cmd(354)      => __E_cli_command_171(355).
	p(NT(354), (NT(355)));
//G578: cli_command(298)     => memory_cmd(354).
	p(NT(298), (NT(354)));
//G579: ltl_sym(359)         => 'l' 't' 'l'.
	p(NT(359), (T(10)+T(8)+T(10)));
//G580: __E_cli_command_172(358) => ltl_sym(359) __(10) spec_arg(37).
	p(NT(358), (NT(359)+NT(10)+NT(37)));
//G581: ltl_cmd(357)         => __E_cli_command_172(358).
	p(NT(357), (NT(358)));
//G582: cli_command(298)     => ltl_cmd(357).
	p(NT(298), (NT(357)));
//G583: func_sym_cmd(362)    => 'f' 'u' 'n' 'c'.
	p(NT(362), (T(9)+T(7)+T(5)+T(14)));
//G584: __E_cli_command_173(361) => func_sym_cmd(362) __(10) func_decl(248).
	p(NT(361), (NT(362)+NT(10)+NT(248)));
//G585: func_cmd(360)        => __E_cli_command_173(361).
	p(NT(360), (NT(361)));
//G586: cli_command(298)     => func_cmd(360).
	p(NT(298), (NT(360)));
//G587: __E___E_cli_command_174_175(366) => 'n'.
	p(NT(366), (T(5)));
//G588: __E___E_cli_command_174_175(366) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(366), (T(5)+T(6)+T(27)+T(28)+T(24)+T(10)+T(4)+T(64)+T(11)));
//G589: normalize_sym(365)   => __E___E_cli_command_174_175(366).
	p(NT(365), (NT(366)));
//G590: __E_cli_command_174(364) => normalize_sym(365) __(10) spec_arg(37).
	p(NT(364), (NT(365)+NT(10)+NT(37)));
//G591: normalize_cmd(363)   => __E_cli_command_174(364).
	p(NT(363), (NT(364)));
//G592: cli_command(298)     => normalize_cmd(363).
	p(NT(298), (NT(363)));
//G593: __E___E_cli_command_176_177(370) => 's'.
	p(NT(370), (T(15)));
//G594: __E___E_cli_command_176_177(370) => 's' 'u' 'b' 's' 't'.
	p(NT(370), (T(15)+T(7)+T(25)+T(15)+T(8)));
//G595: __E___E_cli_command_176_177(370) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(370), (T(15)+T(7)+T(25)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G596: subst_sym(369)       => __E___E_cli_command_176_177(370).
	p(NT(369), (NT(370)));
//G597: __E_cli_command_176(368) => subst_sym(369) __(10) nf_cmd_arg(35) _(9) '[' _(9) nf_cmd_arg(35) _(9) '/' _(9) nf_cmd_arg(35) _(9) ']'.
	p(NT(368), (NT(369)+NT(10)+NT(35)+NT(9)+T(20)+NT(9)+NT(35)+NT(9)+T(16)+NT(9)+NT(35)+NT(9)+T(21)));
//G598: subst_cmd(367)       => __E_cli_command_176(368).
	p(NT(367), (NT(368)));
//G599: cli_command(298)     => subst_cmd(367).
	p(NT(298), (NT(367)));
//G600: __E___E_cli_command_178_179(374) => 'i'.
	p(NT(374), (T(4)));
//G601: __E___E_cli_command_178_179(374) => 'i' 'n' 's' 't'.
	p(NT(374), (T(4)+T(5)+T(15)+T(8)));
//G602: __E___E_cli_command_178_179(374) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(374), (T(4)+T(5)+T(15)+T(8)+T(24)+T(5)+T(8)+T(4)+T(24)+T(8)+T(11)));
//G603: inst_sym(373)        => __E___E_cli_command_178_179(374).
	p(NT(373), (NT(374)));
//G604: __E_cli_command_178(372) => inst_sym(373) __(10) inst_args(32).
	p(NT(372), (NT(373)+NT(10)+NT(32)));
//G605: inst_cmd(371)        => __E_cli_command_178(372).
	p(NT(371), (NT(372)));
//G606: cli_command(298)     => inst_cmd(371).
	p(NT(298), (NT(371)));
//G607: dnf_sym(377)         => 'd' 'n' 'f'.
	p(NT(377), (T(55)+T(5)+T(9)));
//G608: __E_cli_command_180(376) => dnf_sym(377) __(10) nf_cmd_arg(35).
	p(NT(376), (NT(377)+NT(10)+NT(35)));
//G609: dnf_cmd(375)         => __E_cli_command_180(376).
	p(NT(375), (NT(376)));
//G610: cli_command(298)     => dnf_cmd(375).
	p(NT(298), (NT(375)));
//G611: cnf_sym(380)         => 'c' 'n' 'f'.
	p(NT(380), (T(14)+T(5)+T(9)));
//G612: __E_cli_command_181(379) => cnf_sym(380) __(10) nf_cmd_arg(35).
	p(NT(379), (NT(380)+NT(10)+NT(35)));
//G613: cnf_cmd(378)         => __E_cli_command_181(379).
	p(NT(378), (NT(379)));
//G614: cli_command(298)     => cnf_cmd(378).
	p(NT(298), (NT(378)));
//G615: nnf_sym(383)         => 'n' 'n' 'f'.
	p(NT(383), (T(5)+T(5)+T(9)));
//G616: __E_cli_command_182(382) => nnf_sym(383) __(10) nf_cmd_arg(35).
	p(NT(382), (NT(383)+NT(10)+NT(35)));
//G617: nnf_cmd(381)         => __E_cli_command_182(382).
	p(NT(381), (NT(382)));
//G618: cli_command(298)     => nnf_cmd(381).
	p(NT(298), (NT(381)));
//G619: mnf_sym(386)         => 'm' 'n' 'f'.
	p(NT(386), (T(28)+T(5)+T(9)));
//G620: __E_cli_command_183(385) => mnf_sym(386) __(10) nf_cmd_arg(35).
	p(NT(385), (NT(386)+NT(10)+NT(35)));
//G621: mnf_cmd(384)         => __E_cli_command_183(385).
	p(NT(384), (NT(385)));
//G622: cli_command(298)     => mnf_cmd(384).
	p(NT(298), (NT(384)));
//G623: onf_sym(389)         => 'o' 'n' 'f'.
	p(NT(389), (T(6)+T(5)+T(9)));
//G624: __E_cli_command_184(388) => onf_sym(389) __(10) variable(178) __(10) onf_cmd_arg(36).
	p(NT(388), (NT(389)+NT(10)+NT(178)+NT(10)+NT(36)));
//G625: onf_cmd(387)         => __E_cli_command_184(388).
	p(NT(387), (NT(388)));
//G626: cli_command(298)     => onf_cmd(387).
	p(NT(298), (NT(387)));
//G627: qelim_sym(392)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(392), (T(62)+T(11)+T(10)+T(4)+T(28)));
//G628: __E_cli_command_185(391) => qelim_sym(392) __(10) wff_cmd_arg(34).
	p(NT(391), (NT(392)+NT(10)+NT(34)));
//G629: qelim_cmd(390)       => __E_cli_command_185(391).
	p(NT(390), (NT(391)));
//G630: cli_command(298)     => qelim_cmd(390).
	p(NT(298), (NT(390)));
//G631: whatis_sym(395)      => 'w' 'h' 'a' 't' 'i' 's'.
	p(NT(395), (T(32)+T(63)+T(24)+T(8)+T(4)+T(15)));
//G632: __E_cli_command_186(394) => whatis_sym(395) __(10) spec_arg(37).
	p(NT(394), (NT(395)+NT(10)+NT(37)));
//G633: whatis_cmd(393)      => __E_cli_command_186(394).
	p(NT(393), (NT(394)));
//G634: cli_command(298)     => whatis_cmd(393).
	p(NT(298), (NT(393)));
//G635: reset_sym(398)       => 'r' 'e' 's' 'e' 't'.
	p(NT(398), (T(27)+T(11)+T(15)+T(11)+T(8)));
//G636: __E_cli_command_187(397) => reset_sym(398).
	p(NT(397), (NT(398)));
//G637: reset_cmd(396)       => __E_cli_command_187(397).
	p(NT(396), (NT(397)));
//G638: cli_command(298)     => reset_cmd(396).
	p(NT(298), (NT(396)));
//G639: fragment_sym(401)    => 'f' 'r' 'a' 'g' 'm' 'e' 'n' 't'.
	p(NT(401), (T(9)+T(27)+T(24)+T(65)+T(28)+T(11)+T(5)+T(8)));
//G640: __E_cli_command_188(400) => fragment_sym(401) __(10) fragment_name(402).
	p(NT(400), (NT(401)+NT(10)+NT(402)));
//G641: fragment_cmd(399)    => __E_cli_command_188(400).
	p(NT(399), (NT(400)));
//G642: cli_command(298)     => fragment_cmd(399).
	p(NT(298), (NT(399)));
//G643: get_sym(405)         => 'g' 'e' 't'.
	p(NT(405), (T(65)+T(11)+T(8)));
//G644: __E___E_cli_command_189_190(406) => __(10) option_name(23).
	p(NT(406), (NT(10)+NT(23)));
//G645: __E___E_cli_command_189_190(406) => null.
	p(NT(406), (nul));
//G646: __E_cli_command_189(404) => get_sym(405) __E___E_cli_command_189_190(406).
	p(NT(404), (NT(405)+NT(406)));
//G647: get_cmd(403)         => __E_cli_command_189(404).
	p(NT(403), (NT(404)));
//G648: cli_command(298)     => get_cmd(403).
	p(NT(298), (NT(403)));
//G649: set_sym(409)         => 's' 'e' 't'.
	p(NT(409), (T(15)+T(11)+T(8)));
//G650: __E___E_cli_command_191_192(410) => __(10).
	p(NT(410), (NT(10)));
//G651: __E___E_cli_command_191_192(410) => _(9) '=' _(9).
	p(NT(410), (NT(9)+T(3)+NT(9)));
//G652: __E_cli_command_191(408) => set_sym(409) __(10) option_name(23) __E___E_cli_command_191_192(410) option_value(24).
	p(NT(408), (NT(409)+NT(10)+NT(23)+NT(410)+NT(24)));
//G653: set_cmd(407)         => __E_cli_command_191(408).
	p(NT(407), (NT(408)));
//G654: cli_command(298)     => set_cmd(407).
	p(NT(298), (NT(407)));
//G655: enable_sym(413)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(413), (T(11)+T(5)+T(24)+T(25)+T(10)+T(11)));
//G656: __E_cli_command_193(412) => enable_sym(413) __(10) option_name(23).
	p(NT(412), (NT(413)+NT(10)+NT(23)));
//G657: enable_cmd(411)      => __E_cli_command_193(412).
	p(NT(411), (NT(412)));
//G658: cli_command(298)     => enable_cmd(411).
	p(NT(298), (NT(411)));
//G659: disable_sym(416)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(416), (T(55)+T(4)+T(15)+T(24)+T(25)+T(10)+T(11)));
//G660: __E_cli_command_194(415) => disable_sym(416) __(10) option_name(23).
	p(NT(415), (NT(416)+NT(10)+NT(23)));
//G661: disable_cmd(414)     => __E_cli_command_194(415).
	p(NT(414), (NT(415)));
//G662: cli_command(298)     => disable_cmd(414).
	p(NT(298), (NT(414)));
//G663: toggle_sym(419)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(419), (T(8)+T(6)+T(65)+T(65)+T(10)+T(11)));
//G664: __E_cli_command_195(418) => toggle_sym(419) __(10) option_name(23).
	p(NT(418), (NT(419)+NT(10)+NT(23)));
//G665: toggle_cmd(417)      => __E_cli_command_195(418).
	p(NT(417), (NT(418)));
//G666: cli_command(298)     => toggle_cmd(417).
	p(NT(298), (NT(417)));
//G667: __E___E_cli_command_196_197(423) => 'd' 'e' 'f' 's'.
	p(NT(423), (T(55)+T(11)+T(9)+T(15)));
//G668: __E___E_cli_command_196_197(423) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(423), (T(55)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G669: def_sym(422)         => __E___E_cli_command_196_197(423).
	p(NT(422), (NT(423)));
//G670: __E_cli_command_196(421) => def_sym(422).
	p(NT(421), (NT(422)));
//G671: def_list_cmd(420)    => __E_cli_command_196(421).
	p(NT(420), (NT(421)));
//G672: cli_command(298)     => def_list_cmd(420).
	p(NT(298), (NT(420)));
//G673: __E_cli_command_198(425) => def_sym(422) __(10) num(186).
	p(NT(425), (NT(422)+NT(10)+NT(186)));
//G674: def_print_cmd(424)   => __E_cli_command_198(425).
	p(NT(424), (NT(425)));
//G675: cli_command(298)     => def_print_cmd(424).
	p(NT(298), (NT(424)));
//G676: def_rr_cmd(426)      => rec_relation(56).
	p(NT(426), (NT(56)));
//G677: cli_command(298)     => def_rr_cmd(426).
	p(NT(298), (NT(426)));
//G678: def_input_cmd(427)   => input_def(57).
	p(NT(427), (NT(57)));
//G679: cli_command(298)     => def_input_cmd(427).
	p(NT(298), (NT(427)));
//G680: def_output_cmd(428)  => output_def(58).
	p(NT(428), (NT(58)));
//G681: cli_command(298)     => def_output_cmd(428).
	p(NT(298), (NT(428)));
//G682: def_type_cmd(429)    => type_def(59).
	p(NT(429), (NT(59)));
//G683: cli_command(298)     => def_type_cmd(429).
	p(NT(298), (NT(429)));
//G684: __E___E_cli_command_199_200(433) => 'h' 'i' 's' 't'.
	p(NT(433), (T(63)+T(4)+T(15)+T(8)));
//G685: __E___E_cli_command_199_200(433) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(433), (T(63)+T(4)+T(15)+T(8)+T(6)+T(27)+T(17)));
//G686: history_sym(432)     => __E___E_cli_command_199_200(433).
	p(NT(432), (NT(433)));
//G687: __E_cli_command_199(431) => history_sym(432).
	p(NT(431), (NT(432)));
//G688: history_list_cmd(430) => __E_cli_command_199(431).
	p(NT(430), (NT(431)));
//G689: cli_command(298)     => history_list_cmd(430).
	p(NT(298), (NT(430)));
//G690: __E_cli_command_201(435) => history_sym(432) __(10) history(436).
	p(NT(435), (NT(432)+NT(10)+NT(436)));
//G691: history_print_cmd(434) => __E_cli_command_201(435).
	p(NT(434), (NT(435)));
//G692: cli_command(298)     => history_print_cmd(434).
	p(NT(298), (NT(434)));
//G693: __E_cli_command_202(438) => ref(65).
	p(NT(438), (NT(65)));
//G694: __E_cli_command_202(438) => wff(38).
	p(NT(438), (NT(38)));
//G695: __E_cli_command_202(438) => bf(40).
	p(NT(438), (NT(40)));
//G696: history_store_cmd(437) => __E_cli_command_202(438).
	p(NT(437), (NT(438)));
//G697: cli_command(298)     => history_store_cmd(437).
	p(NT(298), (NT(437)));
//G698: solve_options(340)   => __(10) solve_option(439) solve_options(340).
	p(NT(340), (NT(10)+NT(439)+NT(340)));
//G699: solve_options(340)   => null.
	p(NT(340), (nul));
//G700: __E_solve_option_203(440) => solver_mode(441).
	p(NT(440), (NT(441)));
//G701: __E_solve_option_203(440) => type(13).
	p(NT(440), (NT(13)));
//G702: solve_option(439)    => '-' '-' __E_solve_option_203(440).
	p(NT(439), (T(36)+T(36)+NT(440)));
//G703: __E_solver_mode_204(443) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(443), (T(28)+T(4)+T(5)+T(4)+T(28)+T(7)+T(28)));
//G704: __E_solver_mode_204(443) => 'm' 'i' 'n'.
	p(NT(443), (T(28)+T(4)+T(5)));
//G705: solver_mode_minimum(442) => __E_solver_mode_204(443).
	p(NT(442), (NT(443)));
//G706: solver_mode(441)     => solver_mode_minimum(442).
	p(NT(441), (NT(442)));
//G707: __E_solver_mode_205(445) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(445), (T(28)+T(24)+T(38)+T(4)+T(28)+T(7)+T(28)));
//G708: __E_solver_mode_205(445) => 'm' 'a' 'x'.
	p(NT(445), (T(28)+T(24)+T(38)));
//G709: solver_mode_maximum(444) => __E_solver_mode_205(445).
	p(NT(444), (NT(445)));
//G710: solver_mode(441)     => solver_mode_maximum(444).
	p(NT(441), (NT(444)));
//G711: q_file_name(74)      => '"' file_name(21) '"'.
	p(NT(74), (T(66)+NT(21)+T(66)));
//G712: __E_file_name_206(446) => printable(8).
	p(NT(446), (NT(8)));
//G713: __E_file_name_206(446) => printable(8) __E_file_name_206(446).
	p(NT(446), (NT(8)+NT(446)));
//G714: file_name(21)        => __E_file_name_206(446).
	p(NT(21), (NT(446)));
//G715: __E_option_name_207(447) => alnum(6).
	p(NT(447), (NT(6)));
//G716: __E_option_name_207(447) => alnum(6) __E_option_name_207(447).
	p(NT(447), (NT(6)+NT(447)));
//G717: option_name(23)      => __E_option_name_207(447).
	p(NT(23), (NT(447)));
//G718: __E_option_value_208(448) => alnum(6).
	p(NT(448), (NT(6)));
//G719: __E_option_value_208(448) => '.'.
	p(NT(448), (T(1)));
//G720: __E_option_value_209(449) => __E_option_value_208(448).
	p(NT(449), (NT(448)));
//G721: __E_option_value_209(449) => __E_option_value_208(448) __E_option_value_209(449).
	p(NT(449), (NT(448)+NT(449)));
//G722: option_value(24)     => __E_option_value_209(449).
	p(NT(24), (NT(449)));
//G723: fragment_ltl(451)    => 'l' 't' 'l'.
	p(NT(451), (T(10)+T(8)+T(10)));
//G724: __E_fragment_name_210(450) => fragment_ltl(451).
	p(NT(450), (NT(451)));
//G725: fragment_ctl_star(452) => 'c' 't' 'l' '_' 's' 't' 'a' 'r'.
	p(NT(452), (T(14)+T(8)+T(10)+T(56)+T(15)+T(8)+T(24)+T(27)));
//G726: __E_fragment_name_210(450) => fragment_ctl_star(452).
	p(NT(450), (NT(452)));
//G727: fragment_ctl_star(452) => 'c' 't' 'l' '*'.
	p(NT(452), (T(14)+T(8)+T(10)+T(51)));
//G728: __E_fragment_name_210(450) => fragment_ctl_star(452).
	p(NT(450), (NT(452)));
//G729: fragment_name(402)   => __E_fragment_name_210(450).
	p(NT(402), (NT(450)));
//G730: bf_cmd_arg(33)       => history(436).
	p(NT(33), (NT(436)));
//G731: bf_cmd_arg(33)       => bf(40).
	p(NT(33), (NT(40)));
//G732: wff_cmd_arg(34)      => history(436).
	p(NT(34), (NT(436)));
//G733: wff_cmd_arg(34)      => wff(38).
	p(NT(34), (NT(38)));
//G734: nf_cmd_arg(35)       => history(436).
	p(NT(35), (NT(436)));
//G735: nf_cmd_arg(35)       => ref(65).
	p(NT(35), (NT(65)));
//G736: nf_cmd_arg(35)       => wff(38).
	p(NT(35), (NT(38)));
//G737: nf_cmd_arg(35)       => bf(40).
	p(NT(35), (NT(40)));
//G738: onf_cmd_arg(36)      => history(436).
	p(NT(36), (NT(436)));
//G739: onf_cmd_arg(36)      => wff(38).
	p(NT(36), (NT(38)));
//G740: spec_arg(37)         => history(436).
	p(NT(37), (NT(436)));
//G741: spec_arg(37)         => spec(49).
	p(NT(37), (NT(49)));
//G742: spec_arg(37)         => ref(65).
	p(NT(37), (NT(65)));
//G743: spec_arg(37)         => wff(38).
	p(NT(37), (NT(38)));
//G744: spec_arg(37)         => bf(40).
	p(NT(37), (NT(40)));
//G745: inst_args(32)        => wff_cmd_arg(34) _(9) '[' _(9) bf(40) _(9) '/' _(9) bf_cmd_arg(33) _(9) ']'.
	p(NT(32), (NT(34)+NT(9)+T(20)+NT(9)+NT(40)+NT(9)+T(16)+NT(9)+NT(33)+NT(9)+T(21)));
//G746: inst_args(32)        => bf_cmd_arg(33) _(9) '[' _(9) bf(40) _(9) '/' _(9) bf_cmd_arg(33) _(9) ']'.
	p(NT(32), (NT(33)+NT(9)+T(20)+NT(9)+NT(40)+NT(9)+T(16)+NT(9)+NT(33)+NT(9)+T(21)));
//G747: help_arg(316)        => help_sym(313).
	p(NT(316), (NT(313)));
//G748: help_arg(316)        => version_sym(306).
	p(NT(316), (NT(306)));
//G749: help_arg(316)        => quit_sym(303).
	p(NT(316), (NT(303)));
//G750: help_arg(316)        => clear_sym(309).
	p(NT(316), (NT(309)));
//G751: help_arg(316)        => get_sym(405).
	p(NT(316), (NT(405)));
//G752: help_arg(316)        => set_sym(409).
	p(NT(316), (NT(409)));
//G753: help_arg(316)        => enable_sym(413).
	p(NT(316), (NT(413)));
//G754: help_arg(316)        => disable_sym(416).
	p(NT(316), (NT(416)));
//G755: help_arg(316)        => toggle_sym(419).
	p(NT(316), (NT(419)));
//G756: help_arg(316)        => file_sym(319).
	p(NT(316), (NT(319)));
//G757: help_arg(316)        => history_sym(432).
	p(NT(316), (NT(432)));
//G758: help_arg(316)        => abs_history_sym(453).
	p(NT(316), (NT(453)));
//G759: help_arg(316)        => rel_history_sym(454).
	p(NT(316), (NT(454)));
//G760: help_arg(316)        => selection_sym(455).
	p(NT(316), (NT(455)));
//G761: help_arg(316)        => def_sym(422).
	p(NT(316), (NT(422)));
//G762: help_arg(316)        => inst_sym(373).
	p(NT(316), (NT(373)));
//G763: help_arg(316)        => subst_sym(369).
	p(NT(316), (NT(369)));
//G764: help_arg(316)        => normalize_sym(365).
	p(NT(316), (NT(365)));
//G765: help_arg(316)        => execute_sym(456).
	p(NT(316), (NT(456)));
//G766: help_arg(316)        => solve_sym(338).
	p(NT(316), (NT(338)));
//G767: help_arg(316)        => lgrs_sym(343).
	p(NT(316), (NT(343)));
//G768: help_arg(316)        => valid_sym(323).
	p(NT(316), (NT(323)));
//G769: help_arg(316)        => sat_sym(326).
	p(NT(316), (NT(326)));
//G770: help_arg(316)        => unsat_sym(329).
	p(NT(316), (NT(329)));
//G771: help_arg(316)        => realizable_sym(332).
	p(NT(316), (NT(332)));
//G772: help_arg(316)        => unrealizable_sym(335).
	p(NT(316), (NT(335)));
//G773: help_arg(316)        => run_sym(346).
	p(NT(316), (NT(346)));
//G774: help_arg(316)        => ltl_sym(359).
	p(NT(316), (NT(359)));
//G775: help_arg(316)        => stop_sym(353).
	p(NT(316), (NT(353)));
//G776: help_arg(316)        => memory_sym(356).
	p(NT(316), (NT(356)));
//G777: help_arg(316)        => dnf_sym(377).
	p(NT(316), (NT(377)));
//G778: help_arg(316)        => cnf_sym(380).
	p(NT(316), (NT(380)));
//G779: help_arg(316)        => snf_sym(457).
	p(NT(316), (NT(457)));
//G780: help_arg(316)        => nnf_sym(383).
	p(NT(316), (NT(383)));
//G781: help_arg(316)        => mnf_sym(386).
	p(NT(316), (NT(386)));
//G782: help_arg(316)        => onf_sym(389).
	p(NT(316), (NT(389)));
//G783: help_arg(316)        => qelim_sym(392).
	p(NT(316), (NT(392)));
//G784: help_arg(316)        => whatis_sym(395).
	p(NT(316), (NT(395)));
//G785: help_arg(316)        => reset_sym(398).
	p(NT(316), (NT(398)));
//G786: help_arg(316)        => fragment_sym(401).
	p(NT(316), (NT(401)));
//G787: __E___E_help_arg_211_212(460) => 's'.
	p(NT(460), (T(15)));
//G788: __E___E_help_arg_211_212(460) => null.
	p(NT(460), (nul));
//G789: __E_help_arg_211(459) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_211_212(460).
	p(NT(459), (T(11)+T(38)+T(24)+T(28)+T(18)+T(10)+T(11)+NT(460)));
//G790: examples_sym(458)    => __E_help_arg_211(459).
	p(NT(458), (NT(459)));
//G791: help_arg(316)        => examples_sym(458).
	p(NT(316), (NT(458)));
//G792: __E_history_213(462) => '%'.
	p(NT(462), (T(50)));
//G793: rel_history_sym(454) => '%' '-'.
	p(NT(454), (T(50)+T(36)));
//G794: history_id(464)      => digits(12).
	p(NT(464), (NT(12)));
//G795: __E___E_history_213_214(463) => history_id(464).
	p(NT(463), (NT(464)));
//G796: __E___E_history_213_214(463) => null.
	p(NT(463), (nul));
//G797: __E_history_213(462) => rel_history_sym(454) __E___E_history_213_214(463).
	p(NT(462), (NT(454)+NT(463)));
//G798: rel_history(461)     => __E_history_213(462).
	p(NT(461), (NT(462)));
//G799: history(436)         => rel_history(461).
	p(NT(436), (NT(461)));
//G800: abs_history_sym(453) => '%'.
	p(NT(453), (T(50)));
//G801: __E_history_215(466) => abs_history_sym(453) history_id(464).
	p(NT(466), (NT(453)+NT(464)));
//G802: abs_history(465)     => __E_history_215(466).
	p(NT(465), (NT(466)));
//G803: history(436)         => abs_history(465).
	p(NT(436), (NT(465)));
//G804: unreachable(467)     => BDD_ID(468).
	p(NT(467), (NT(468)));
//G805: stream_def(469)      => input_def(57).
	p(NT(469), (NT(57)));
//G806: stream_def(469)      => output_def(58).
	p(NT(469), (NT(58)));
//G807: fm_or_term(470)      => wff(38).
	p(NT(470), (NT(38)));
//G808: fm_or_term(470)      => bf(40).
	p(NT(470), (NT(40)));
	#undef T
	#undef NT
	return loaded = true, p;
}

} // namespace tau_parser_data
