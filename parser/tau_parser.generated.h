// This file is generated from a file parser/tau.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
#ifndef __TAU_PARSER_H__
#define __TAU_PARSER_H__

#include "parser.h"

namespace tau_parser_data {

using char_type     = char;
using terminal_type = char;

inline static constexpr size_t nt_bits = 9;
inline const std::vector<std::string> symbol_names{
	"", "eof", "space", "digit", "xdigit", "alpha", "alnum", "punct", "printable", "_", 
	"__", "sym", "digits", "type", "subtype", "integer", "source", "capture", "var_name", "io_var_name", 
	"uconst_name", "file_name", "ctnvar", "option_name", "option_value", "chars", "q_var", "src_c", "uconst", "spec_part", 
	"inst_args", "bf_cmd_arg", "wff_cmd_arg", "nf_cmd_arg", "onf_cmd_arg", "spec_arg", "wff", "wff_parenthesis", "bf", "bf_parenthesis", 
	"bf_neg_oprnd", "bf_cast_oprnd", "bf_and_nosep_1st_oprnd", "bf_and_nosep_2nd_oprnd", "bf_and_nosep", "bf_and", "start", "spec", "__E_spec_0", "definitions", 
	"main", "__E_spec_1", "__E_definitions_2", "__E___E_definitions_2_3", "rec_relation", "input_def", "output_def", "__E_definitions_4", "spec_multiline", "__E_spec_multiline_5", 
	"__E_spec_multiline_6", "__E_spec_multiline_7", "ref", "__E_rec_relation_8", "__E_input_def_9", "typed", "stream", "__E_output_def_10", "q_file_name", "console_sym", 
	"__E_ref_11", "offsets", "ref_args", "__E_ref_12", "__E_ref_13", "fp_fallback", "first_sym", "last_sym", "__E_ref_args_14", "__E___E_ref_args_14_15", 
	"ref_arg", "__E___E_ref_args_14_16", "__E___E_ref_args_14_17", "__E_wff_18", "wff_sometimes", "__E_wff_19", "__E___E_wff_19_20", "wff_always", "__E_wff_21", "__E___E_wff_21_22", 
	"wff_conditional", "__E_wff_23", "wff_all", "__E_wff_24", "q_vars", "wff_ex", "__E_wff_25", "wff_ref", "constraint", "wff_imply", 
	"__E_wff_26", "wff_rimply", "__E_wff_27", "wff_equiv", "__E_wff_28", "wff_or", "__E_wff_29", "wff_xor", "__E_wff_30", "wff_and", 
	"__E_wff_31", "wff_neg", "__E_wff_32", "wff_t", "wff_f", "bf_interval", "__E_wff_33", "bf_eq", "__E_wff_34", "bf_neq", 
	"__E_wff_35", "bf_lt", "__E_wff_36", "bf_nlt", "__E_wff_37", "bf_lteq", "__E_wff_38", "bf_nlteq", "__E_wff_39", "bf_gt", 
	"__E_wff_40", "bf_ngt", "__E_wff_41", "bf_gteq", "__E_wff_42", "bf_ngteq", "__E_wff_43", "__E_bf_44", "ba_constant", "variable", 
	"bf_fall", "__E_bf_45", "bf_fex", "__E_bf_46", "bf_ref", "bf_cast", "__E_bf_47", "num", "__E___E_bf_47_48", "bf_t", 
	"bf_f", "bf_neg", "bf_shr", "__E_bf_49", "__E___E_bf_49_50", "bf_shl", "__E_bf_51", "__E___E_bf_51_52", "bf_add", "__E_bf_53", 
	"bf_sub", "__E_bf_54", "__E___E_bf_54_55", "bf_mod", "__E_bf_56", "__E___E_bf_56_57", "bf_mul", "__E_bf_58", "bf_div", "__E_bf_59", 
	"__E___E_bf_59_60", "bf_nor", "__E_bf_61", "__E___E_bf_61_62", "bf_xnor", "__E_bf_63", "__E___E_bf_63_64", "bf_nand", "__E_bf_65", "__E___E_bf_65_66", 
	"bf_or", "__E_bf_67", "bf_xor", "__E_bf_68", "__E_bf_69", "__E___E_bf_69_70", "__E_bf_71", "__E___E_bf_71_72", "__E_bf_73", "__E___E_bf_73_74", 
	"__E_bf_75", "__E___E_bf_75_76", "__E_bf_77", "__E___E_bf_77_78", "__E___E_bf_77_79", "ctn_neq", "__E_constraint_80", "ctn_eq", "__E_constraint_81", "ctn_gteq", 
	"__E_constraint_82", "ctn_gt", "__E_constraint_83", "ctn_lteq", "__E_constraint_84", "ctn_lt", "__E_constraint_85", "__E_ba_constant_86", "__E_ba_constant_87", "__E_source_88", 
	"__E___E_source_88_89", "__E___E_source_88_90", "__E_src_c_91", "__E_src_c_92", "__E_variable_93", "io_var", "__E_variable_94", "__E_var_name_95", "__E_var_name_96", "__E_var_name_97", 
	"__E_var_name_98", "__E_var_name_99", "offset", "__E_uconst_100", "__E___E_uconst_100_101", "__E_q_vars_102", "__E_q_vars_103", "__E_offsets_104", "__E_offsets_105", "shift", 
	"__E_offset_106", "__E_shift_107", "__E___E_shift_107_108", "__E_chars_109", "__E_chars_110", "__E_digits_111", "__E_integer_112", "__E_typed_113", "comment", "__E_comment_114", 
	"__E_comment_115", "__E_comment_116", "cli", "cli_command", "__E_cli_117", "__E_cli_118", "__E_cli_119", "quit_cmd", "quit_sym", "__E_cli_command_120", 
	"version_cmd", "version_sym", "__E_cli_command_121", "clear_cmd", "clear_sym", "__E_cli_command_122", "help_cmd", "__E_cli_command_123", "help_sym", "__E___E_cli_command_123_124", 
	"__E___E_cli_command_123_125", "help_arg", "file_cmd", "__E_cli_command_126", "file_sym", "q_string", "valid_cmd", "__E_cli_command_127", "valid_sym", "sat_cmd", 
	"__E_cli_command_128", "sat_sym", "unsat_cmd", "__E_cli_command_129", "unsat_sym", "solve_cmd", "__E_cli_command_130", "solve_sym", "__E___E_cli_command_130_131", "solve_options", 
	"lgrs_cmd", "__E_cli_command_132", "lgrs_sym", "run_cmd", "__E_cli_command_133", "run_sym", "__E___E_cli_command_133_134", "normalize_cmd", "__E_cli_command_135", "normalize_sym", 
	"__E___E_cli_command_135_136", "subst_cmd", "__E_cli_command_137", "subst_sym", "__E___E_cli_command_137_138", "inst_cmd", "__E_cli_command_139", "inst_sym", "__E___E_cli_command_139_140", "dnf_cmd", 
	"__E_cli_command_141", "dnf_sym", "cnf_cmd", "__E_cli_command_142", "cnf_sym", "nnf_cmd", "__E_cli_command_143", "nnf_sym", "mnf_cmd", "__E_cli_command_144", 
	"mnf_sym", "onf_cmd", "__E_cli_command_145", "onf_sym", "qelim_cmd", "__E_cli_command_146", "qelim_sym", "get_cmd", "__E_cli_command_147", "get_sym", 
	"__E___E_cli_command_147_148", "set_cmd", "__E_cli_command_149", "set_sym", "__E___E_cli_command_149_150", "enable_cmd", "__E_cli_command_151", "enable_sym", "disable_cmd", "__E_cli_command_152", 
	"disable_sym", "toggle_cmd", "__E_cli_command_153", "toggle_sym", "def_list_cmd", "__E_cli_command_154", "def_sym", "__E___E_cli_command_154_155", "def_print_cmd", "__E_cli_command_156", 
	"def_rr_cmd", "def_input_cmd", "def_output_cmd", "history_list_cmd", "__E_cli_command_157", "history_sym", "__E___E_cli_command_157_158", "history_print_cmd", "__E_cli_command_159", "history", 
	"history_store_cmd", "__E_cli_command_160", "__E_solve_options_161", "solve_option", "__E_solve_options_162", "__E_solve_option_163", "solver_mode", "solver_mode_minimum", "__E_solver_mode_164", "solver_mode_maximum", 
	"__E_solver_mode_165", "__E_file_name_166", "__E_option_name_167", "__E_option_value_168", "__E_option_value_169", "abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", "snf_sym", 
	"examples_sym", "__E_help_arg_170", "__E___E_help_arg_170_171", "rel_history", "__E_history_172", "__E___E_history_172_173", "history_id", "abs_history", "__E_history_174", "unreachable", 
	"BDD_ID", "stream_def", "fm_or_term", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", 
	"__N_7", "__N_8", "__N_9", "__N_10", "__N_11", "__N_12", "__N_13", "__N_14", "__N_15", "__N_16", 
	"__N_17", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', 'a', 'b', 'k', 
	'r', ',', 'm', '<', '>', 'w', 'y', '[', ']', '?', 
	'x', '-', '|', '^', '&', '!', 'T', 'F', 'v', '+', 
	'%', '*', '/', '1', '0', '\'', '{', '}', '_', '$', 
	'\t', '\n', '\r', '#', 'q', 'h', 'p', 'd', 'g', 'z', 
	'"', 
};

inline ::idni::char_class_fns<terminal_type> char_classes =
	::idni::predefined_char_classes<char_type, terminal_type>({
		"eof",
		"space",
		"digit",
		"xdigit",
		"alpha",
		"alnum",
		"punct",
		"printable",
	}, nts);

inline struct ::idni::grammar<char_type, terminal_type>::options
	grammar_options
{
	.transform_negation = false,
	.auto_disambiguate = true,
	.shaping = {
		.to_trim = {
			9, 10
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
			21, 22, 23, 24
		},
		.to_inline = {
			{ 25 },
			{ 26 },
			{ 27 },
			{ 28 },
			{ 28, 20 },
			{ 29 },
			{ 30 },
			{ 31 },
			{ 32 },
			{ 33 },
			{ 34 },
			{ 35 },
			{ 36, 37, 36 },
			{ 38, 39, 38 },
			{ 40, 39, 38 },
			{ 41, 39, 38 },
			{ 42, 39, 38 },
			{ 43, 39, 38 },
			{ 44, 39, 45 }
		},
		.inline_char_classes = true
	},
	.enabled_guards = {
		"charvar"
	}
};

inline auto make_parser_options() {
	auto o = ::idni::default_parser_options<char_type, terminal_type>();
	return o;
}

inline ::idni::prods<char_type, terminal_type> start_symbol{ nts(46) };

inline idni::prods<char_type, terminal_type>& productions() {
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
//G146: __E_bf_44(137)       => '(' _(9) bf(38) _(9) ')'.
	p(NT(137), (T(12)+NT(9)+NT(38)+NT(9)+T(13)));
//G147: bf_parenthesis(39)   => __E_bf_44(137).
	p(NT(39), (NT(137)));
//G148: bf(38)               => bf_parenthesis(39).
	p(NT(38), (NT(39)));
//G149: bf(38)               => ba_constant(138).
	p(NT(38), (NT(138)));
//G150: bf(38)               => variable(139).
	p(NT(38), (NT(139)));
//G151: __E_bf_45(141)       => 'f' 'a' 'l' 'l' __(10) q_vars(94) __(10) bf(38).
	p(NT(141), (T(9)+T(16)+T(10)+T(10)+NT(10)+NT(94)+NT(10)+NT(38)));
//G152: bf_fall(140)         => __E_bf_45(141).
	p(NT(140), (NT(141)));
//G153: bf(38)               => bf_fall(140).
	p(NT(38), (NT(140)));
//G154: __E_bf_46(143)       => 'f' 'e' 'x' __(10) q_vars(94) __(10) bf(38).
	p(NT(143), (T(9)+T(11)+T(29)+NT(10)+NT(94)+NT(10)+NT(38)));
//G155: bf_fex(142)          => __E_bf_46(143).
	p(NT(142), (NT(143)));
//G156: bf(38)               => bf_fex(142).
	p(NT(38), (NT(142)));
//G157: bf_ref(144)          => ref(62).
	p(NT(144), (NT(62)));
//G158: bf(38)               => bf_ref(144).
	p(NT(38), (NT(144)));
//G159: __E___E_bf_47_48(148) => bf_parenthesis(39).
	p(NT(148), (NT(39)));
//G160: __E___E_bf_47_48(148) => ba_constant(138).
	p(NT(148), (NT(138)));
//G161: __E___E_bf_47_48(148) => variable(139).
	p(NT(148), (NT(139)));
//G162: __E___E_bf_47_48(148) => bf_ref(144).
	p(NT(148), (NT(144)));
//G163: __E___E_bf_47_48(148) => bf_t(149).
	p(NT(148), (NT(149)));
//G164: __E___E_bf_47_48(148) => bf_f(150).
	p(NT(148), (NT(150)));
//G165: __E___E_bf_47_48(148) => bf_neg(151).
	p(NT(148), (NT(151)));
//G166: __E___E_bf_47_48(148) => bf_cast(145).
	p(NT(148), (NT(145)));
//G167: __E___E_bf_47_48(148) => capture(17).
	p(NT(148), (NT(17)));
//G168: bf_cast_oprnd(41)    => __E___E_bf_47_48(148).
	p(NT(41), (NT(148)));
//G169: __E_bf_47(146)       => '(' _(9) 'b' 'v' _(9) '[' _(9) num(147) _(9) ']' _(9) ')' _(9) bf_cast_oprnd(41).
	p(NT(146), (T(12)+NT(9)+T(17)+T(37)+NT(9)+T(26)+NT(9)+NT(147)+NT(9)+T(27)+NT(9)+T(13)+NT(9)+NT(41)));
//G170: bf_cast(145)         => __E_bf_47(146).
	p(NT(145), (NT(146)));
//G171: bf(38)               => bf_cast(145).
	p(NT(38), (NT(145)));
//G172: __N_0(383)           => bf_shr(152).
	p(NT(383), (NT(152)));
//G173: __E___E_bf_49_50(154) => bf(38) & ~( __N_0(383) ).	 # conjunctive
	p(NT(154), (NT(38)) & ~(NT(383)));
//G174: __E_bf_49(153)       => bf(38) _(9) '>' '>' _(9) __E___E_bf_49_50(154).
	p(NT(153), (NT(38)+NT(9)+T(23)+T(23)+NT(9)+NT(154)));
//G175: bf_shr(152)          => __E_bf_49(153).
	p(NT(152), (NT(153)));
//G176: bf(38)               => bf_shr(152).
	p(NT(38), (NT(152)));
//G177: __N_1(384)           => bf_shl(155).
	p(NT(384), (NT(155)));
//G178: __E___E_bf_51_52(157) => bf(38) & ~( __N_1(384) ).	 # conjunctive
	p(NT(157), (NT(38)) & ~(NT(384)));
//G179: __E_bf_51(156)       => bf(38) _(9) '<' '<' _(9) __E___E_bf_51_52(157).
	p(NT(156), (NT(38)+NT(9)+T(22)+T(22)+NT(9)+NT(157)));
//G180: bf_shl(155)          => __E_bf_51(156).
	p(NT(155), (NT(156)));
//G181: bf(38)               => bf_shl(155).
	p(NT(38), (NT(155)));
//G182: __E_bf_53(159)       => bf(38) _(9) '+' _(9) bf(38).
	p(NT(159), (NT(38)+NT(9)+T(38)+NT(9)+NT(38)));
//G183: bf_add(158)          => __E_bf_53(159).
	p(NT(158), (NT(159)));
//G184: bf(38)               => bf_add(158).
	p(NT(38), (NT(158)));
//G185: __N_2(385)           => bf_sub(160).
	p(NT(385), (NT(160)));
//G186: __E___E_bf_54_55(162) => bf(38) & ~( __N_2(385) ).	 # conjunctive
	p(NT(162), (NT(38)) & ~(NT(385)));
//G187: __E_bf_54(161)       => bf(38) _(9) '-' _(9) __E___E_bf_54_55(162).
	p(NT(161), (NT(38)+NT(9)+T(30)+NT(9)+NT(162)));
//G188: bf_sub(160)          => __E_bf_54(161).
	p(NT(160), (NT(161)));
//G189: bf(38)               => bf_sub(160).
	p(NT(38), (NT(160)));
//G190: __N_3(386)           => bf_mod(163).
	p(NT(386), (NT(163)));
//G191: __E___E_bf_56_57(165) => bf(38) & ~( __N_3(386) ).	 # conjunctive
	p(NT(165), (NT(38)) & ~(NT(386)));
//G192: __E_bf_56(164)       => bf(38) _(9) '%' _(9) __E___E_bf_56_57(165).
	p(NT(164), (NT(38)+NT(9)+T(39)+NT(9)+NT(165)));
//G193: bf_mod(163)          => __E_bf_56(164).
	p(NT(163), (NT(164)));
//G194: bf(38)               => bf_mod(163).
	p(NT(38), (NT(163)));
//G195: __E_bf_58(167)       => bf(38) _(9) '*' _(9) bf(38).
	p(NT(167), (NT(38)+NT(9)+T(40)+NT(9)+NT(38)));
//G196: bf_mul(166)          => __E_bf_58(167).
	p(NT(166), (NT(167)));
//G197: bf(38)               => bf_mul(166).
	p(NT(38), (NT(166)));
//G198: __N_4(387)           => bf_div(168).
	p(NT(387), (NT(168)));
//G199: __E___E_bf_59_60(170) => bf(38) & ~( __N_4(387) ).	 # conjunctive
	p(NT(170), (NT(38)) & ~(NT(387)));
//G200: __E_bf_59(169)       => bf(38) _(9) '/' _(9) __E___E_bf_59_60(170).
	p(NT(169), (NT(38)+NT(9)+T(41)+NT(9)+NT(170)));
//G201: bf_div(168)          => __E_bf_59(169).
	p(NT(168), (NT(169)));
//G202: bf(38)               => bf_div(168).
	p(NT(38), (NT(168)));
//G203: __N_5(388)           => bf_nor(171).
	p(NT(388), (NT(171)));
//G204: __E___E_bf_61_62(173) => bf(38) & ~( __N_5(388) ).	 # conjunctive
	p(NT(173), (NT(38)) & ~(NT(388)));
//G205: __E_bf_61(172)       => bf(38) _(9) '!' '|' _(9) __E___E_bf_61_62(173).
	p(NT(172), (NT(38)+NT(9)+T(34)+T(31)+NT(9)+NT(173)));
//G206: bf_nor(171)          => __E_bf_61(172).
	p(NT(171), (NT(172)));
//G207: bf(38)               => bf_nor(171).
	p(NT(38), (NT(171)));
//G208: __N_6(389)           => bf_xnor(174).
	p(NT(389), (NT(174)));
//G209: __E___E_bf_63_64(176) => bf(38) & ~( __N_6(389) ).	 # conjunctive
	p(NT(176), (NT(38)) & ~(NT(389)));
//G210: __E_bf_63(175)       => bf(38) _(9) '!' '^' _(9) __E___E_bf_63_64(176).
	p(NT(175), (NT(38)+NT(9)+T(34)+T(32)+NT(9)+NT(176)));
//G211: bf_xnor(174)         => __E_bf_63(175).
	p(NT(174), (NT(175)));
//G212: bf(38)               => bf_xnor(174).
	p(NT(38), (NT(174)));
//G213: __N_7(390)           => bf_nand(177).
	p(NT(390), (NT(177)));
//G214: __E___E_bf_65_66(179) => bf(38) & ~( __N_7(390) ).	 # conjunctive
	p(NT(179), (NT(38)) & ~(NT(390)));
//G215: __E_bf_65(178)       => bf(38) _(9) '!' '&' _(9) __E___E_bf_65_66(179).
	p(NT(178), (NT(38)+NT(9)+T(34)+T(33)+NT(9)+NT(179)));
//G216: bf_nand(177)         => __E_bf_65(178).
	p(NT(177), (NT(178)));
//G217: bf(38)               => bf_nand(177).
	p(NT(38), (NT(177)));
//G218: __E_bf_67(181)       => bf(38) _(9) '|' _(9) bf(38).
	p(NT(181), (NT(38)+NT(9)+T(31)+NT(9)+NT(38)));
//G219: bf_or(180)           => __E_bf_67(181).
	p(NT(180), (NT(181)));
//G220: bf(38)               => bf_or(180).
	p(NT(38), (NT(180)));
//G221: __E_bf_68(183)       => bf(38) _(9) '^' _(9) bf(38).
	p(NT(183), (NT(38)+NT(9)+T(32)+NT(9)+NT(38)));
//G222: bf_xor(182)          => __E_bf_68(183).
	p(NT(182), (NT(183)));
//G223: bf(38)               => bf_xor(182).
	p(NT(38), (NT(182)));
//G224: __E___E_bf_69_70(185) => typed(65).
	p(NT(185), (NT(65)));
//G225: __E___E_bf_69_70(185) => null.
	p(NT(185), (nul));
//G226: __E_bf_69(184)       => '1' __E___E_bf_69_70(185).
	p(NT(184), (T(42)+NT(185)));
//G227: bf_t(149)            => __E_bf_69(184).
	p(NT(149), (NT(184)));
//G228: bf(38)               => bf_t(149).
	p(NT(38), (NT(149)));
//G229: __E___E_bf_71_72(187) => typed(65).
	p(NT(187), (NT(65)));
//G230: __E___E_bf_71_72(187) => null.
	p(NT(187), (nul));
//G231: __E_bf_71(186)       => '0' __E___E_bf_71_72(187).
	p(NT(186), (T(43)+NT(187)));
//G232: bf_f(150)            => __E_bf_71(186).
	p(NT(150), (NT(186)));
//G233: bf(38)               => bf_f(150).
	p(NT(38), (NT(150)));
//G234: __E___E_bf_73_74(189) => _(9) '&' _(9).
	p(NT(189), (NT(9)+T(33)+NT(9)));
//G235: __E___E_bf_73_74(189) => __(10).
	p(NT(189), (NT(10)));
//G236: __E_bf_73(188)       => bf(38) __E___E_bf_73_74(189) bf(38).
	p(NT(188), (NT(38)+NT(189)+NT(38)));
//G237: bf_and(45)           => __E_bf_73(188).
	p(NT(45), (NT(188)));
//G238: bf(38)               => bf_and(45).
	p(NT(38), (NT(45)));
//G239: __E___E_bf_75_76(191) => bf_parenthesis(39).
	p(NT(191), (NT(39)));
//G240: __E___E_bf_75_76(191) => ba_constant(138).
	p(NT(191), (NT(138)));
//G241: __E___E_bf_75_76(191) => variable(139).
	p(NT(191), (NT(139)));
//G242: __E___E_bf_75_76(191) => bf_ref(144).
	p(NT(191), (NT(144)));
//G243: __E___E_bf_75_76(191) => bf_t(149).
	p(NT(191), (NT(149)));
//G244: __E___E_bf_75_76(191) => bf_f(150).
	p(NT(191), (NT(150)));
//G245: __E___E_bf_75_76(191) => bf_neg(151).
	p(NT(191), (NT(151)));
//G246: __E___E_bf_75_76(191) => capture(17).
	p(NT(191), (NT(17)));
//G247: bf_neg_oprnd(40)     => __E___E_bf_75_76(191).
	p(NT(40), (NT(191)));
//G248: __E_bf_75(190)       => bf_neg_oprnd(40) _(9) '\''.
	p(NT(190), (NT(40)+NT(9)+T(44)));
//G249: bf_neg(151)          => __E_bf_75(190).
	p(NT(151), (NT(190)));
//G250: bf(38)               => bf_neg(151).
	p(NT(38), (NT(151)));
//G251: __E___E_bf_77_78(193) => bf_parenthesis(39).
	p(NT(193), (NT(39)));
//G252: __E___E_bf_77_78(193) => variable(139).
	p(NT(193), (NT(139)));
//G253: __E___E_bf_77_78(193) => bf_ref(144).
	p(NT(193), (NT(144)));
//G254: __E___E_bf_77_78(193) => bf_neg(151).
	p(NT(193), (NT(151)));
//G255: __E___E_bf_77_78(193) => bf_and_nosep(44).
	p(NT(193), (NT(44)));
//G256: __E___E_bf_77_78(193) => capture(17).
	p(NT(193), (NT(17)));
//G257: bf_and_nosep_1st_oprnd(42) => __E___E_bf_77_78(193).
	p(NT(42), (NT(193)));
//G258: __E___E_bf_77_79(194) => bf_parenthesis(39).
	p(NT(194), (NT(39)));
//G259: __E___E_bf_77_79(194) => ba_constant(138).
	p(NT(194), (NT(138)));
//G260: __E___E_bf_77_79(194) => variable(139).
	p(NT(194), (NT(139)));
//G261: __E___E_bf_77_79(194) => bf_ref(144).
	p(NT(194), (NT(144)));
//G262: __E___E_bf_77_79(194) => bf_neg(151).
	p(NT(194), (NT(151)));
//G263: __E___E_bf_77_79(194) => capture(17).
	p(NT(194), (NT(17)));
//G264: bf_and_nosep_2nd_oprnd(43) => __E___E_bf_77_79(194).
	p(NT(43), (NT(194)));
//G265: __E_bf_77(192)       => bf_and_nosep_1st_oprnd(42) bf_and_nosep_2nd_oprnd(43).
	p(NT(192), (NT(42)+NT(43)));
//G266: bf_and_nosep(44)     => __E_bf_77(192).
	p(NT(44), (NT(192)));
//G267: bf(38)               => bf_and_nosep(44).
	p(NT(38), (NT(44)));
//G268: bf(38)               => capture(17).
	p(NT(38), (NT(17)));
//G269: __E_constraint_80(196) => '[' ctnvar(22) _(9) '!' '=' _(9) num(147) ']'.
	p(NT(196), (T(26)+NT(22)+NT(9)+T(34)+T(3)+NT(9)+NT(147)+T(27)));
//G270: __E_constraint_80(196) => '[' num(147) _(9) '!' '=' _(9) ctnvar(22) ']'.
	p(NT(196), (T(26)+NT(147)+NT(9)+T(34)+T(3)+NT(9)+NT(22)+T(27)));
//G271: ctn_neq(195)         => __E_constraint_80(196).
	p(NT(195), (NT(196)));
//G272: constraint(98)       => ctn_neq(195).
	p(NT(98), (NT(195)));
//G273: __E_constraint_81(198) => '[' ctnvar(22) _(9) '=' _(9) num(147) ']'.
	p(NT(198), (T(26)+NT(22)+NT(9)+T(3)+NT(9)+NT(147)+T(27)));
//G274: __E_constraint_81(198) => '[' num(147) _(9) '=' _(9) ctnvar(22) ']'.
	p(NT(198), (T(26)+NT(147)+NT(9)+T(3)+NT(9)+NT(22)+T(27)));
//G275: ctn_eq(197)          => __E_constraint_81(198).
	p(NT(197), (NT(198)));
//G276: constraint(98)       => ctn_eq(197).
	p(NT(98), (NT(197)));
//G277: __E_constraint_82(200) => '[' ctnvar(22) _(9) '>' '=' _(9) num(147) ']'.
	p(NT(200), (T(26)+NT(22)+NT(9)+T(23)+T(3)+NT(9)+NT(147)+T(27)));
//G278: __E_constraint_82(200) => '[' num(147) _(9) '>' '=' _(9) ctnvar(22) ']'.
	p(NT(200), (T(26)+NT(147)+NT(9)+T(23)+T(3)+NT(9)+NT(22)+T(27)));
//G279: ctn_gteq(199)        => __E_constraint_82(200).
	p(NT(199), (NT(200)));
//G280: constraint(98)       => ctn_gteq(199).
	p(NT(98), (NT(199)));
//G281: __E_constraint_83(202) => '[' ctnvar(22) _(9) '>' _(9) num(147) ']'.
	p(NT(202), (T(26)+NT(22)+NT(9)+T(23)+NT(9)+NT(147)+T(27)));
//G282: __E_constraint_83(202) => '[' num(147) _(9) '>' _(9) ctnvar(22) ']'.
	p(NT(202), (T(26)+NT(147)+NT(9)+T(23)+NT(9)+NT(22)+T(27)));
//G283: ctn_gt(201)          => __E_constraint_83(202).
	p(NT(201), (NT(202)));
//G284: constraint(98)       => ctn_gt(201).
	p(NT(98), (NT(201)));
//G285: __E_constraint_84(204) => '[' ctnvar(22) _(9) '<' '=' _(9) num(147) ']'.
	p(NT(204), (T(26)+NT(22)+NT(9)+T(22)+T(3)+NT(9)+NT(147)+T(27)));
//G286: __E_constraint_84(204) => '[' num(147) _(9) '<' '=' _(9) ctnvar(22) ']'.
	p(NT(204), (T(26)+NT(147)+NT(9)+T(22)+T(3)+NT(9)+NT(22)+T(27)));
//G287: ctn_lteq(203)        => __E_constraint_84(204).
	p(NT(203), (NT(204)));
//G288: constraint(98)       => ctn_lteq(203).
	p(NT(98), (NT(203)));
//G289: __E_constraint_85(206) => '[' ctnvar(22) _(9) '<' _(9) num(147) ']'.
	p(NT(206), (T(26)+NT(22)+NT(9)+T(22)+NT(9)+NT(147)+T(27)));
//G290: __E_constraint_85(206) => '[' num(147) _(9) '<' _(9) ctnvar(22) ']'.
	p(NT(206), (T(26)+NT(147)+NT(9)+T(22)+NT(9)+NT(22)+T(27)));
//G291: ctn_lt(205)          => __E_constraint_85(206).
	p(NT(205), (NT(206)));
//G292: constraint(98)       => ctn_lt(205).
	p(NT(98), (NT(205)));
//G293: __E_ba_constant_86(207) => capture(17).
	p(NT(207), (NT(17)));
//G294: __E_ba_constant_86(207) => source(16).
	p(NT(207), (NT(16)));
//G295: __E_ba_constant_87(208) => typed(65).
	p(NT(208), (NT(65)));
//G296: __E_ba_constant_87(208) => null.
	p(NT(208), (nul));
//G297: ba_constant(138)     => '{' _(9) __E_ba_constant_86(207) _(9) '}' __E_ba_constant_87(208).
	p(NT(138), (T(45)+NT(9)+NT(207)+NT(9)+T(46)+NT(208)));
//G298: __E___E_source_88_89(210) => src_c(27).
	p(NT(210), (NT(27)));
//G299: __E___E_source_88_89(210) => space(2).
	p(NT(210), (NT(2)));
//G300: __E___E_source_88_90(211) => null.
	p(NT(211), (nul));
//G301: __E___E_source_88_90(211) => __E___E_source_88_89(210) __E___E_source_88_90(211).
	p(NT(211), (NT(210)+NT(211)));
//G302: __E_source_88(209)   => __E___E_source_88_90(211) src_c(27).
	p(NT(209), (NT(211)+NT(27)));
//G303: __E_source_88(209)   => null.
	p(NT(209), (nul));
//G304: source(16)           => src_c(27) __E_source_88(209).
	p(NT(16), (NT(27)+NT(209)));
//G305: src_c(27)            => alnum(6).
	p(NT(27), (NT(6)));
//G306: __N_8(391)           => '{'.
	p(NT(391), (T(45)));
//G307: __N_9(392)           => '}'.
	p(NT(392), (T(46)));
//G308: src_c(27)            => ~( __N_8(391) ) & ~( __N_9(392) ) & punct(7).	 # conjunctive
	p(NT(27), ~(NT(391)) & ~(NT(392)) & (NT(7)));
//G309: __E_src_c_91(212)    => src_c(27).
	p(NT(212), (NT(27)));
//G310: __E_src_c_91(212)    => space(2).
	p(NT(212), (NT(2)));
//G311: __E_src_c_92(213)    => null.
	p(NT(213), (nul));
//G312: __E_src_c_92(213)    => __E_src_c_91(212) __E_src_c_92(213).
	p(NT(213), (NT(212)+NT(213)));
//G313: src_c(27)            => '{' __E_src_c_92(213) '}'.
	p(NT(27), (T(45)+NT(213)+T(46)));
//G314: __E_variable_93(214) => uconst(28).
	p(NT(214), (NT(28)));
//G315: __E_variable_93(214) => io_var(215).
	p(NT(214), (NT(215)));
//G316: __E_variable_93(214) => var_name(18).
	p(NT(214), (NT(18)));
//G317: __E_variable_94(216) => typed(65).
	p(NT(216), (NT(65)));
//G318: __E_variable_94(216) => null.
	p(NT(216), (nul));
//G319: variable(139)        => __E_variable_93(214) __E_variable_94(216).
	p(NT(139), (NT(214)+NT(216)));
//G320: __N_10(393)          => 'F'.
	p(NT(393), (T(36)));
//G321: __N_11(394)          => 'T'.
	p(NT(394), (T(35)));
//G322: __E_var_name_95(217) => ~( __N_10(393) ) & ~( __N_11(394) ) & alpha(5).	 # conjunctive
	p(NT(217), ~(NT(393)) & ~(NT(394)) & (NT(5)));
//G323: __E_var_name_96(218) => null.
	p(NT(218), (nul));
//G324: __E_var_name_96(218) => digit(3) __E_var_name_96(218).
	p(NT(218), (NT(3)+NT(218)));
//G325: var_name(18)         => __E_var_name_95(217) __E_var_name_96(218).	 # guarded: charvar
	p(NT(18), (NT(217)+NT(218)));
	p.back().guard = "charvar";
//G326: __N_12(395)          => 'F'.
	p(NT(395), (T(36)));
//G327: __N_13(396)          => 'T'.
	p(NT(396), (T(35)));
//G328: __E_var_name_97(219) => ~( __N_12(395) ) & ~( __N_13(396) ) & alpha(5).	 # conjunctive
	p(NT(219), ~(NT(395)) & ~(NT(396)) & (NT(5)));
//G329: __E_var_name_98(220) => alnum(6).
	p(NT(220), (NT(6)));
//G330: __E_var_name_98(220) => '_'.
	p(NT(220), (T(47)));
//G331: __E_var_name_99(221) => null.
	p(NT(221), (nul));
//G332: __E_var_name_99(221) => __E_var_name_98(220) __E_var_name_99(221).
	p(NT(221), (NT(220)+NT(221)));
//G333: var_name(18)         => __E_var_name_97(219) __E_var_name_99(221).	 # guarded: var
	p(NT(18), (NT(219)+NT(221)));
	p.back().guard = "var";
//G334: io_var(215)          => io_var_name(19) '[' offset(222) ']'.
	p(NT(215), (NT(19)+T(26)+NT(222)+T(27)));
//G335: io_var_name(19)      => chars(25).
	p(NT(19), (NT(25)));
//G336: __E___E_uconst_100_101(224) => chars(25) _(9).
	p(NT(224), (NT(25)+NT(9)));
//G337: __E___E_uconst_100_101(224) => null.
	p(NT(224), (nul));
//G338: __E_uconst_100(223)  => __E___E_uconst_100_101(224) ':' _(9) chars(25).
	p(NT(223), (NT(224)+T(2)+NT(9)+NT(25)));
//G339: uconst_name(20)      => __E_uconst_100(223).
	p(NT(20), (NT(223)));
//G340: uconst(28)           => '<' _(9) uconst_name(20) _(9) '>'.
	p(NT(28), (T(22)+NT(9)+NT(20)+NT(9)+T(23)));
//G341: __E_q_vars_102(225)  => _(9) ',' _(9) q_var(26).
	p(NT(225), (NT(9)+T(20)+NT(9)+NT(26)));
//G342: __E_q_vars_103(226)  => null.
	p(NT(226), (nul));
//G343: __E_q_vars_103(226)  => __E_q_vars_102(225) __E_q_vars_103(226).
	p(NT(226), (NT(225)+NT(226)));
//G344: q_vars(94)           => q_var(26) __E_q_vars_103(226).
	p(NT(94), (NT(26)+NT(226)));
//G345: q_var(26)            => capture(17).
	p(NT(26), (NT(17)));
//G346: __N_14(397)          => uconst(28).
	p(NT(397), (NT(28)));
//G347: q_var(26)            => ~( __N_14(397) ) & variable(139).	 # conjunctive
	p(NT(26), ~(NT(397)) & (NT(139)));
//G348: ctnvar(22)           => chars(25).
	p(NT(22), (NT(25)));
//G349: __E_offsets_104(227) => _(9) ',' _(9) offset(222).
	p(NT(227), (NT(9)+T(20)+NT(9)+NT(222)));
//G350: __E_offsets_105(228) => null.
	p(NT(228), (nul));
//G351: __E_offsets_105(228) => __E_offsets_104(227) __E_offsets_105(228).
	p(NT(228), (NT(227)+NT(228)));
//G352: offsets(71)          => '[' _(9) offset(222) __E_offsets_105(228) _(9) ']'.
	p(NT(71), (T(26)+NT(9)+NT(222)+NT(228)+NT(9)+T(27)));
//G353: offset(222)          => integer(15).
	p(NT(222), (NT(15)));
//G354: offset(222)          => capture(17).
	p(NT(222), (NT(17)));
//G355: offset(222)          => shift(229).
	p(NT(222), (NT(229)));
//G356: __N_15(398)          => io_var(215).
	p(NT(398), (NT(215)));
//G357: __E_offset_106(230)  => variable(139) & ~( __N_15(398) ).	 # conjunctive
	p(NT(230), (NT(139)) & ~(NT(398)));
//G358: offset(222)          => __E_offset_106(230).
	p(NT(222), (NT(230)));
//G359: __E_shift_107(231)   => capture(17).
	p(NT(231), (NT(17)));
//G360: __N_16(399)          => io_var(215).
	p(NT(399), (NT(215)));
//G361: __E___E_shift_107_108(232) => variable(139) & ~( __N_16(399) ).	 # conjunctive
	p(NT(232), (NT(139)) & ~(NT(399)));
//G362: __E_shift_107(231)   => __E___E_shift_107_108(232).
	p(NT(231), (NT(232)));
//G363: shift(229)           => __E_shift_107(231) _(9) '-' _(9) num(147).
	p(NT(229), (NT(231)+NT(9)+T(30)+NT(9)+NT(147)));
//G364: __E_chars_109(233)   => alnum(6).
	p(NT(233), (NT(6)));
//G365: __E_chars_109(233)   => '_'.
	p(NT(233), (T(47)));
//G366: __E_chars_110(234)   => null.
	p(NT(234), (nul));
//G367: __E_chars_110(234)   => __E_chars_109(233) __E_chars_110(234).
	p(NT(234), (NT(233)+NT(234)));
//G368: chars(25)            => alpha(5) __E_chars_110(234).
	p(NT(25), (NT(5)+NT(234)));
//G369: __E_digits_111(235)  => digit(3).
	p(NT(235), (NT(3)));
//G370: __E_digits_111(235)  => digit(3) __E_digits_111(235).
	p(NT(235), (NT(3)+NT(235)));
//G371: digits(12)           => __E_digits_111(235).
	p(NT(12), (NT(235)));
//G372: num(147)             => digits(12).
	p(NT(147), (NT(12)));
//G373: __E_integer_112(236) => '-'.
	p(NT(236), (T(30)));
//G374: __E_integer_112(236) => null.
	p(NT(236), (nul));
//G375: integer(15)          => __E_integer_112(236) _(9) digits(12).
	p(NT(15), (NT(236)+NT(9)+NT(12)));
//G376: sym(11)              => chars(25).
	p(NT(11), (NT(25)));
//G377: capture(17)          => '$' chars(25).
	p(NT(17), (T(48)+NT(25)));
//G378: typed(65)            => _(9) ':' _(9) type(13) _(9) '[' _(9) subtype(14) _(9) ']'.
	p(NT(65), (NT(9)+T(2)+NT(9)+NT(13)+NT(9)+T(26)+NT(9)+NT(14)+NT(9)+T(27)));
//G379: __N_17(400)          => 'b' 'v'.
	p(NT(400), (T(17)+T(37)));
//G380: __E_typed_113(237)   => ~( __N_17(400) ) & type(13).	 # conjunctive
	p(NT(237), ~(NT(400)) & (NT(13)));
//G381: typed(65)            => _(9) ':' _(9) __E_typed_113(237).
	p(NT(65), (NT(9)+T(2)+NT(9)+NT(237)));
//G382: type(13)             => chars(25).
	p(NT(13), (NT(25)));
//G383: subtype(14)          => num(147).
	p(NT(14), (NT(147)));
//G384: __E_comment_114(239) => printable(8).
	p(NT(239), (NT(8)));
//G385: __E_comment_114(239) => '\t'.
	p(NT(239), (T(49)));
//G386: __E_comment_115(240) => null.
	p(NT(240), (nul));
//G387: __E_comment_115(240) => __E_comment_114(239) __E_comment_115(240).
	p(NT(240), (NT(239)+NT(240)));
//G388: __E_comment_116(241) => '\n'.
	p(NT(241), (T(50)));
//G389: __E_comment_116(241) => '\r'.
	p(NT(241), (T(51)));
//G390: __E_comment_116(241) => eof(1).
	p(NT(241), (NT(1)));
//G391: comment(238)         => '#' __E_comment_115(240) __E_comment_116(241).
	p(NT(238), (T(52)+NT(240)+NT(241)));
//G392: __(10)               => space(2).
	p(NT(10), (NT(2)));
//G393: __(10)               => comment(238).
	p(NT(10), (NT(238)));
//G394: __(10)               => __(10) space(2).
	p(NT(10), (NT(10)+NT(2)));
//G395: __(10)               => __(10) comment(238).
	p(NT(10), (NT(10)+NT(238)));
//G396: _(9)                 => __(10).
	p(NT(9), (NT(10)));
//G397: _(9)                 => null.
	p(NT(9), (nul));
//G398: cli(242)             => _(9).
	p(NT(242), (NT(9)));
//G399: __E_cli_117(244)     => '.' _(9) cli_command(243) _(9).
	p(NT(244), (T(1)+NT(9)+NT(243)+NT(9)));
//G400: __E_cli_118(245)     => null.
	p(NT(245), (nul));
//G401: __E_cli_118(245)     => __E_cli_117(244) __E_cli_118(245).
	p(NT(245), (NT(244)+NT(245)));
//G402: __E_cli_119(246)     => '.' _(9).
	p(NT(246), (T(1)+NT(9)));
//G403: __E_cli_119(246)     => null.
	p(NT(246), (nul));
//G404: cli(242)             => _(9) cli_command(243) _(9) __E_cli_118(245) __E_cli_119(246).
	p(NT(242), (NT(9)+NT(243)+NT(9)+NT(245)+NT(246)));
//G405: __E_cli_command_120(249) => 'q'.
	p(NT(249), (T(53)));
//G406: __E_cli_command_120(249) => 'q' 'u' 'i' 't'.
	p(NT(249), (T(53)+T(7)+T(4)+T(8)));
//G407: quit_sym(248)        => __E_cli_command_120(249).
	p(NT(248), (NT(249)));
//G408: quit_cmd(247)        => quit_sym(248).
	p(NT(247), (NT(248)));
//G409: cli_command(243)     => quit_cmd(247).
	p(NT(243), (NT(247)));
//G410: __E_cli_command_121(252) => 'v'.
	p(NT(252), (T(37)));
//G411: __E_cli_command_121(252) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(252), (T(37)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G412: version_sym(251)     => __E_cli_command_121(252).
	p(NT(251), (NT(252)));
//G413: version_cmd(250)     => version_sym(251).
	p(NT(250), (NT(251)));
//G414: cli_command(243)     => version_cmd(250).
	p(NT(243), (NT(250)));
//G415: __E_cli_command_122(255) => 'c'.
	p(NT(255), (T(14)));
//G416: __E_cli_command_122(255) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(255), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G417: clear_sym(254)       => __E_cli_command_122(255).
	p(NT(254), (NT(255)));
//G418: clear_cmd(253)       => clear_sym(254).
	p(NT(253), (NT(254)));
//G419: cli_command(243)     => clear_cmd(253).
	p(NT(243), (NT(253)));
//G420: __E___E_cli_command_123_124(259) => 'h'.
	p(NT(259), (T(54)));
//G421: __E___E_cli_command_123_124(259) => 'h' 'e' 'l' 'p'.
	p(NT(259), (T(54)+T(11)+T(10)+T(55)));
//G422: help_sym(258)        => __E___E_cli_command_123_124(259).
	p(NT(258), (NT(259)));
//G423: __E___E_cli_command_123_125(260) => __(10) help_arg(261).
	p(NT(260), (NT(10)+NT(261)));
//G424: __E___E_cli_command_123_125(260) => null.
	p(NT(260), (nul));
//G425: __E_cli_command_123(257) => help_sym(258) __E___E_cli_command_123_125(260).
	p(NT(257), (NT(258)+NT(260)));
//G426: help_cmd(256)        => __E_cli_command_123(257).
	p(NT(256), (NT(257)));
//G427: cli_command(243)     => help_cmd(256).
	p(NT(243), (NT(256)));
//G428: file_sym(264)        => 'f' 'i' 'l' 'e'.
	p(NT(264), (T(9)+T(4)+T(10)+T(11)));
//G429: __E_cli_command_126(263) => file_sym(264) __(10) q_string(265).
	p(NT(263), (NT(264)+NT(10)+NT(265)));
//G430: file_cmd(262)        => __E_cli_command_126(263).
	p(NT(262), (NT(263)));
//G431: cli_command(243)     => file_cmd(262).
	p(NT(243), (NT(262)));
//G432: valid_sym(268)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(268), (T(37)+T(16)+T(10)+T(4)+T(56)));
//G433: __E_cli_command_127(267) => valid_sym(268) __(10) spec_arg(35).
	p(NT(267), (NT(268)+NT(10)+NT(35)));
//G434: valid_cmd(266)       => __E_cli_command_127(267).
	p(NT(266), (NT(267)));
//G435: cli_command(243)     => valid_cmd(266).
	p(NT(243), (NT(266)));
//G436: sat_sym(271)         => 's' 'a' 't'.
	p(NT(271), (T(15)+T(16)+T(8)));
//G437: __E_cli_command_128(270) => sat_sym(271) __(10) spec_arg(35).
	p(NT(270), (NT(271)+NT(10)+NT(35)));
//G438: sat_cmd(269)         => __E_cli_command_128(270).
	p(NT(269), (NT(270)));
//G439: cli_command(243)     => sat_cmd(269).
	p(NT(243), (NT(269)));
//G440: unsat_sym(274)       => 'u' 'n' 's' 'a' 't'.
	p(NT(274), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G441: __E_cli_command_129(273) => unsat_sym(274) __(10) spec_arg(35).
	p(NT(273), (NT(274)+NT(10)+NT(35)));
//G442: unsat_cmd(272)       => __E_cli_command_129(273).
	p(NT(272), (NT(273)));
//G443: cli_command(243)     => unsat_cmd(272).
	p(NT(243), (NT(272)));
//G444: solve_sym(277)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(277), (T(15)+T(6)+T(10)+T(37)+T(11)));
//G445: __E___E_cli_command_130_131(278) => solve_options(279).
	p(NT(278), (NT(279)));
//G446: __E___E_cli_command_130_131(278) => null.
	p(NT(278), (nul));
//G447: __E_cli_command_130(276) => solve_sym(277) __E___E_cli_command_130_131(278) __(10) wff_cmd_arg(32).
	p(NT(276), (NT(277)+NT(278)+NT(10)+NT(32)));
//G448: solve_cmd(275)       => __E_cli_command_130(276).
	p(NT(275), (NT(276)));
//G449: cli_command(243)     => solve_cmd(275).
	p(NT(243), (NT(275)));
//G450: lgrs_sym(282)        => 'l' 'g' 'r' 's'.
	p(NT(282), (T(10)+T(57)+T(19)+T(15)));
//G451: __E_cli_command_132(281) => lgrs_sym(282) __(10) wff_cmd_arg(32).
	p(NT(281), (NT(282)+NT(10)+NT(32)));
//G452: lgrs_cmd(280)        => __E_cli_command_132(281).
	p(NT(280), (NT(281)));
//G453: cli_command(243)     => lgrs_cmd(280).
	p(NT(243), (NT(280)));
//G454: __E___E_cli_command_133_134(286) => 'r'.
	p(NT(286), (T(19)));
//G455: __E___E_cli_command_133_134(286) => 'r' 'u' 'n'.
	p(NT(286), (T(19)+T(7)+T(5)));
//G456: run_sym(285)         => __E___E_cli_command_133_134(286).
	p(NT(285), (NT(286)));
//G457: __E_cli_command_133(284) => run_sym(285) __(10) spec_arg(35).
	p(NT(284), (NT(285)+NT(10)+NT(35)));
//G458: run_cmd(283)         => __E_cli_command_133(284).
	p(NT(283), (NT(284)));
//G459: cli_command(243)     => run_cmd(283).
	p(NT(243), (NT(283)));
//G460: __E___E_cli_command_135_136(290) => 'n'.
	p(NT(290), (T(5)));
//G461: __E___E_cli_command_135_136(290) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(290), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(58)+T(11)));
//G462: normalize_sym(289)   => __E___E_cli_command_135_136(290).
	p(NT(289), (NT(290)));
//G463: __E_cli_command_135(288) => normalize_sym(289) __(10) spec_arg(35).
	p(NT(288), (NT(289)+NT(10)+NT(35)));
//G464: normalize_cmd(287)   => __E_cli_command_135(288).
	p(NT(287), (NT(288)));
//G465: cli_command(243)     => normalize_cmd(287).
	p(NT(243), (NT(287)));
//G466: __E___E_cli_command_137_138(294) => 's'.
	p(NT(294), (T(15)));
//G467: __E___E_cli_command_137_138(294) => 's' 'u' 'b' 's' 't'.
	p(NT(294), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G468: __E___E_cli_command_137_138(294) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(294), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G469: subst_sym(293)       => __E___E_cli_command_137_138(294).
	p(NT(293), (NT(294)));
//G470: __E_cli_command_137(292) => subst_sym(293) __(10) nf_cmd_arg(33) _(9) '[' _(9) nf_cmd_arg(33) _(9) '/' _(9) nf_cmd_arg(33) _(9) ']'.
	p(NT(292), (NT(293)+NT(10)+NT(33)+NT(9)+T(26)+NT(9)+NT(33)+NT(9)+T(41)+NT(9)+NT(33)+NT(9)+T(27)));
//G471: subst_cmd(291)       => __E_cli_command_137(292).
	p(NT(291), (NT(292)));
//G472: cli_command(243)     => subst_cmd(291).
	p(NT(243), (NT(291)));
//G473: __E___E_cli_command_139_140(298) => 'i'.
	p(NT(298), (T(4)));
//G474: __E___E_cli_command_139_140(298) => 'i' 'n' 's' 't'.
	p(NT(298), (T(4)+T(5)+T(15)+T(8)));
//G475: __E___E_cli_command_139_140(298) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(298), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G476: inst_sym(297)        => __E___E_cli_command_139_140(298).
	p(NT(297), (NT(298)));
//G477: __E_cli_command_139(296) => inst_sym(297) __(10) inst_args(30).
	p(NT(296), (NT(297)+NT(10)+NT(30)));
//G478: inst_cmd(295)        => __E_cli_command_139(296).
	p(NT(295), (NT(296)));
//G479: cli_command(243)     => inst_cmd(295).
	p(NT(243), (NT(295)));
//G480: dnf_sym(301)         => 'd' 'n' 'f'.
	p(NT(301), (T(56)+T(5)+T(9)));
//G481: __E_cli_command_141(300) => dnf_sym(301) __(10) nf_cmd_arg(33).
	p(NT(300), (NT(301)+NT(10)+NT(33)));
//G482: dnf_cmd(299)         => __E_cli_command_141(300).
	p(NT(299), (NT(300)));
//G483: cli_command(243)     => dnf_cmd(299).
	p(NT(243), (NT(299)));
//G484: cnf_sym(304)         => 'c' 'n' 'f'.
	p(NT(304), (T(14)+T(5)+T(9)));
//G485: __E_cli_command_142(303) => cnf_sym(304) __(10) nf_cmd_arg(33).
	p(NT(303), (NT(304)+NT(10)+NT(33)));
//G486: cnf_cmd(302)         => __E_cli_command_142(303).
	p(NT(302), (NT(303)));
//G487: cli_command(243)     => cnf_cmd(302).
	p(NT(243), (NT(302)));
//G488: nnf_sym(307)         => 'n' 'n' 'f'.
	p(NT(307), (T(5)+T(5)+T(9)));
//G489: __E_cli_command_143(306) => nnf_sym(307) __(10) nf_cmd_arg(33).
	p(NT(306), (NT(307)+NT(10)+NT(33)));
//G490: nnf_cmd(305)         => __E_cli_command_143(306).
	p(NT(305), (NT(306)));
//G491: cli_command(243)     => nnf_cmd(305).
	p(NT(243), (NT(305)));
//G492: mnf_sym(310)         => 'm' 'n' 'f'.
	p(NT(310), (T(21)+T(5)+T(9)));
//G493: __E_cli_command_144(309) => mnf_sym(310) __(10) nf_cmd_arg(33).
	p(NT(309), (NT(310)+NT(10)+NT(33)));
//G494: mnf_cmd(308)         => __E_cli_command_144(309).
	p(NT(308), (NT(309)));
//G495: cli_command(243)     => mnf_cmd(308).
	p(NT(243), (NT(308)));
//G496: onf_sym(313)         => 'o' 'n' 'f'.
	p(NT(313), (T(6)+T(5)+T(9)));
//G497: __E_cli_command_145(312) => onf_sym(313) __(10) variable(139) __(10) onf_cmd_arg(34).
	p(NT(312), (NT(313)+NT(10)+NT(139)+NT(10)+NT(34)));
//G498: onf_cmd(311)         => __E_cli_command_145(312).
	p(NT(311), (NT(312)));
//G499: cli_command(243)     => onf_cmd(311).
	p(NT(243), (NT(311)));
//G500: qelim_sym(316)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(316), (T(53)+T(11)+T(10)+T(4)+T(21)));
//G501: __E_cli_command_146(315) => qelim_sym(316) __(10) wff_cmd_arg(32).
	p(NT(315), (NT(316)+NT(10)+NT(32)));
//G502: qelim_cmd(314)       => __E_cli_command_146(315).
	p(NT(314), (NT(315)));
//G503: cli_command(243)     => qelim_cmd(314).
	p(NT(243), (NT(314)));
//G504: get_sym(319)         => 'g' 'e' 't'.
	p(NT(319), (T(57)+T(11)+T(8)));
//G505: __E___E_cli_command_147_148(320) => __(10) option_name(23).
	p(NT(320), (NT(10)+NT(23)));
//G506: __E___E_cli_command_147_148(320) => null.
	p(NT(320), (nul));
//G507: __E_cli_command_147(318) => get_sym(319) __E___E_cli_command_147_148(320).
	p(NT(318), (NT(319)+NT(320)));
//G508: get_cmd(317)         => __E_cli_command_147(318).
	p(NT(317), (NT(318)));
//G509: cli_command(243)     => get_cmd(317).
	p(NT(243), (NT(317)));
//G510: set_sym(323)         => 's' 'e' 't'.
	p(NT(323), (T(15)+T(11)+T(8)));
//G511: __E___E_cli_command_149_150(324) => __(10).
	p(NT(324), (NT(10)));
//G512: __E___E_cli_command_149_150(324) => _(9) '=' _(9).
	p(NT(324), (NT(9)+T(3)+NT(9)));
//G513: __E_cli_command_149(322) => set_sym(323) __(10) option_name(23) __E___E_cli_command_149_150(324) option_value(24).
	p(NT(322), (NT(323)+NT(10)+NT(23)+NT(324)+NT(24)));
//G514: set_cmd(321)         => __E_cli_command_149(322).
	p(NT(321), (NT(322)));
//G515: cli_command(243)     => set_cmd(321).
	p(NT(243), (NT(321)));
//G516: enable_sym(327)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(327), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G517: __E_cli_command_151(326) => enable_sym(327) __(10) option_name(23).
	p(NT(326), (NT(327)+NT(10)+NT(23)));
//G518: enable_cmd(325)      => __E_cli_command_151(326).
	p(NT(325), (NT(326)));
//G519: cli_command(243)     => enable_cmd(325).
	p(NT(243), (NT(325)));
//G520: disable_sym(330)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(330), (T(56)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G521: __E_cli_command_152(329) => disable_sym(330) __(10) option_name(23).
	p(NT(329), (NT(330)+NT(10)+NT(23)));
//G522: disable_cmd(328)     => __E_cli_command_152(329).
	p(NT(328), (NT(329)));
//G523: cli_command(243)     => disable_cmd(328).
	p(NT(243), (NT(328)));
//G524: toggle_sym(333)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(333), (T(8)+T(6)+T(57)+T(57)+T(10)+T(11)));
//G525: __E_cli_command_153(332) => toggle_sym(333) __(10) option_name(23).
	p(NT(332), (NT(333)+NT(10)+NT(23)));
//G526: toggle_cmd(331)      => __E_cli_command_153(332).
	p(NT(331), (NT(332)));
//G527: cli_command(243)     => toggle_cmd(331).
	p(NT(243), (NT(331)));
//G528: __E___E_cli_command_154_155(337) => 'd' 'e' 'f' 's'.
	p(NT(337), (T(56)+T(11)+T(9)+T(15)));
//G529: __E___E_cli_command_154_155(337) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(337), (T(56)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G530: def_sym(336)         => __E___E_cli_command_154_155(337).
	p(NT(336), (NT(337)));
//G531: __E_cli_command_154(335) => def_sym(336).
	p(NT(335), (NT(336)));
//G532: def_list_cmd(334)    => __E_cli_command_154(335).
	p(NT(334), (NT(335)));
//G533: cli_command(243)     => def_list_cmd(334).
	p(NT(243), (NT(334)));
//G534: __E_cli_command_156(339) => def_sym(336) __(10) num(147).
	p(NT(339), (NT(336)+NT(10)+NT(147)));
//G535: def_print_cmd(338)   => __E_cli_command_156(339).
	p(NT(338), (NT(339)));
//G536: cli_command(243)     => def_print_cmd(338).
	p(NT(243), (NT(338)));
//G537: def_rr_cmd(340)      => rec_relation(54).
	p(NT(340), (NT(54)));
//G538: cli_command(243)     => def_rr_cmd(340).
	p(NT(243), (NT(340)));
//G539: def_input_cmd(341)   => input_def(55).
	p(NT(341), (NT(55)));
//G540: cli_command(243)     => def_input_cmd(341).
	p(NT(243), (NT(341)));
//G541: def_output_cmd(342)  => output_def(56).
	p(NT(342), (NT(56)));
//G542: cli_command(243)     => def_output_cmd(342).
	p(NT(243), (NT(342)));
//G543: __E___E_cli_command_157_158(346) => 'h' 'i' 's' 't'.
	p(NT(346), (T(54)+T(4)+T(15)+T(8)));
//G544: __E___E_cli_command_157_158(346) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(346), (T(54)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G545: history_sym(345)     => __E___E_cli_command_157_158(346).
	p(NT(345), (NT(346)));
//G546: __E_cli_command_157(344) => history_sym(345).
	p(NT(344), (NT(345)));
//G547: history_list_cmd(343) => __E_cli_command_157(344).
	p(NT(343), (NT(344)));
//G548: cli_command(243)     => history_list_cmd(343).
	p(NT(243), (NT(343)));
//G549: __E_cli_command_159(348) => history_sym(345) __(10) history(349).
	p(NT(348), (NT(345)+NT(10)+NT(349)));
//G550: history_print_cmd(347) => __E_cli_command_159(348).
	p(NT(347), (NT(348)));
//G551: cli_command(243)     => history_print_cmd(347).
	p(NT(243), (NT(347)));
//G552: __E_cli_command_160(351) => ref(62).
	p(NT(351), (NT(62)));
//G553: __E_cli_command_160(351) => wff(36).
	p(NT(351), (NT(36)));
//G554: __E_cli_command_160(351) => bf(38).
	p(NT(351), (NT(38)));
//G555: history_store_cmd(350) => __E_cli_command_160(351).
	p(NT(350), (NT(351)));
//G556: cli_command(243)     => history_store_cmd(350).
	p(NT(243), (NT(350)));
//G557: __E_solve_options_161(352) => __(10) solve_option(353).
	p(NT(352), (NT(10)+NT(353)));
//G558: __E_solve_options_162(354) => null.
	p(NT(354), (nul));
//G559: __E_solve_options_162(354) => __E_solve_options_161(352) __E_solve_options_162(354).
	p(NT(354), (NT(352)+NT(354)));
//G560: solve_options(279)   => __E_solve_options_162(354).
	p(NT(279), (NT(354)));
//G561: __E_solve_option_163(355) => solver_mode(356).
	p(NT(355), (NT(356)));
//G562: __E_solve_option_163(355) => type(13).
	p(NT(355), (NT(13)));
//G563: solve_option(353)    => '-' '-' __E_solve_option_163(355).
	p(NT(353), (T(30)+T(30)+NT(355)));
//G564: __E_solver_mode_164(358) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(358), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G565: __E_solver_mode_164(358) => 'm' 'i' 'n'.
	p(NT(358), (T(21)+T(4)+T(5)));
//G566: solver_mode_minimum(357) => __E_solver_mode_164(358).
	p(NT(357), (NT(358)));
//G567: solver_mode(356)     => solver_mode_minimum(357).
	p(NT(356), (NT(357)));
//G568: __E_solver_mode_165(360) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(360), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G569: __E_solver_mode_165(360) => 'm' 'a' 'x'.
	p(NT(360), (T(21)+T(16)+T(29)));
//G570: solver_mode_maximum(359) => __E_solver_mode_165(360).
	p(NT(359), (NT(360)));
//G571: solver_mode(356)     => solver_mode_maximum(359).
	p(NT(356), (NT(359)));
//G572: q_file_name(68)      => '"' file_name(21) '"'.
	p(NT(68), (T(59)+NT(21)+T(59)));
//G573: __E_file_name_166(361) => printable(8).
	p(NT(361), (NT(8)));
//G574: __E_file_name_166(361) => printable(8) __E_file_name_166(361).
	p(NT(361), (NT(8)+NT(361)));
//G575: file_name(21)        => __E_file_name_166(361).
	p(NT(21), (NT(361)));
//G576: __E_option_name_167(362) => alnum(6).
	p(NT(362), (NT(6)));
//G577: __E_option_name_167(362) => alnum(6) __E_option_name_167(362).
	p(NT(362), (NT(6)+NT(362)));
//G578: option_name(23)      => __E_option_name_167(362).
	p(NT(23), (NT(362)));
//G579: __E_option_value_168(363) => alnum(6).
	p(NT(363), (NT(6)));
//G580: __E_option_value_168(363) => '.'.
	p(NT(363), (T(1)));
//G581: __E_option_value_169(364) => __E_option_value_168(363).
	p(NT(364), (NT(363)));
//G582: __E_option_value_169(364) => __E_option_value_168(363) __E_option_value_169(364).
	p(NT(364), (NT(363)+NT(364)));
//G583: option_value(24)     => __E_option_value_169(364).
	p(NT(24), (NT(364)));
//G584: bf_cmd_arg(31)       => history(349).
	p(NT(31), (NT(349)));
//G585: bf_cmd_arg(31)       => bf(38).
	p(NT(31), (NT(38)));
//G586: wff_cmd_arg(32)      => history(349).
	p(NT(32), (NT(349)));
//G587: wff_cmd_arg(32)      => wff(36).
	p(NT(32), (NT(36)));
//G588: nf_cmd_arg(33)       => history(349).
	p(NT(33), (NT(349)));
//G589: nf_cmd_arg(33)       => ref(62).
	p(NT(33), (NT(62)));
//G590: nf_cmd_arg(33)       => wff(36).
	p(NT(33), (NT(36)));
//G591: nf_cmd_arg(33)       => bf(38).
	p(NT(33), (NT(38)));
//G592: onf_cmd_arg(34)      => history(349).
	p(NT(34), (NT(349)));
//G593: onf_cmd_arg(34)      => wff(36).
	p(NT(34), (NT(36)));
//G594: spec_arg(35)         => history(349).
	p(NT(35), (NT(349)));
//G595: spec_arg(35)         => spec(47).
	p(NT(35), (NT(47)));
//G596: spec_arg(35)         => ref(62).
	p(NT(35), (NT(62)));
//G597: spec_arg(35)         => wff(36).
	p(NT(35), (NT(36)));
//G598: spec_arg(35)         => bf(38).
	p(NT(35), (NT(38)));
//G599: inst_args(30)        => wff_cmd_arg(32) _(9) '[' _(9) bf(38) _(9) '/' _(9) bf_cmd_arg(31) _(9) ']'.
	p(NT(30), (NT(32)+NT(9)+T(26)+NT(9)+NT(38)+NT(9)+T(41)+NT(9)+NT(31)+NT(9)+T(27)));
//G600: inst_args(30)        => bf_cmd_arg(31) _(9) '[' _(9) bf(38) _(9) '/' _(9) bf_cmd_arg(31) _(9) ']'.
	p(NT(30), (NT(31)+NT(9)+T(26)+NT(9)+NT(38)+NT(9)+T(41)+NT(9)+NT(31)+NT(9)+T(27)));
//G601: help_arg(261)        => help_sym(258).
	p(NT(261), (NT(258)));
//G602: help_arg(261)        => version_sym(251).
	p(NT(261), (NT(251)));
//G603: help_arg(261)        => quit_sym(248).
	p(NT(261), (NT(248)));
//G604: help_arg(261)        => clear_sym(254).
	p(NT(261), (NT(254)));
//G605: help_arg(261)        => get_sym(319).
	p(NT(261), (NT(319)));
//G606: help_arg(261)        => set_sym(323).
	p(NT(261), (NT(323)));
//G607: help_arg(261)        => enable_sym(327).
	p(NT(261), (NT(327)));
//G608: help_arg(261)        => disable_sym(330).
	p(NT(261), (NT(330)));
//G609: help_arg(261)        => toggle_sym(333).
	p(NT(261), (NT(333)));
//G610: help_arg(261)        => file_sym(264).
	p(NT(261), (NT(264)));
//G611: help_arg(261)        => history_sym(345).
	p(NT(261), (NT(345)));
//G612: help_arg(261)        => abs_history_sym(365).
	p(NT(261), (NT(365)));
//G613: help_arg(261)        => rel_history_sym(366).
	p(NT(261), (NT(366)));
//G614: help_arg(261)        => selection_sym(367).
	p(NT(261), (NT(367)));
//G615: help_arg(261)        => def_sym(336).
	p(NT(261), (NT(336)));
//G616: help_arg(261)        => inst_sym(297).
	p(NT(261), (NT(297)));
//G617: help_arg(261)        => subst_sym(293).
	p(NT(261), (NT(293)));
//G618: help_arg(261)        => normalize_sym(289).
	p(NT(261), (NT(289)));
//G619: help_arg(261)        => execute_sym(368).
	p(NT(261), (NT(368)));
//G620: help_arg(261)        => solve_sym(277).
	p(NT(261), (NT(277)));
//G621: help_arg(261)        => valid_sym(268).
	p(NT(261), (NT(268)));
//G622: help_arg(261)        => sat_sym(271).
	p(NT(261), (NT(271)));
//G623: help_arg(261)        => unsat_sym(274).
	p(NT(261), (NT(274)));
//G624: help_arg(261)        => run_sym(285).
	p(NT(261), (NT(285)));
//G625: help_arg(261)        => dnf_sym(301).
	p(NT(261), (NT(301)));
//G626: help_arg(261)        => cnf_sym(304).
	p(NT(261), (NT(304)));
//G627: help_arg(261)        => snf_sym(369).
	p(NT(261), (NT(369)));
//G628: help_arg(261)        => nnf_sym(307).
	p(NT(261), (NT(307)));
//G629: help_arg(261)        => mnf_sym(310).
	p(NT(261), (NT(310)));
//G630: help_arg(261)        => onf_sym(313).
	p(NT(261), (NT(313)));
//G631: help_arg(261)        => qelim_sym(316).
	p(NT(261), (NT(316)));
//G632: __E___E_help_arg_170_171(372) => 's'.
	p(NT(372), (T(15)));
//G633: __E___E_help_arg_170_171(372) => null.
	p(NT(372), (nul));
//G634: __E_help_arg_170(371) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_170_171(372).
	p(NT(371), (T(11)+T(29)+T(16)+T(21)+T(55)+T(10)+T(11)+NT(372)));
//G635: examples_sym(370)    => __E_help_arg_170(371).
	p(NT(370), (NT(371)));
//G636: help_arg(261)        => examples_sym(370).
	p(NT(261), (NT(370)));
//G637: __E_history_172(374) => '%'.
	p(NT(374), (T(39)));
//G638: rel_history_sym(366) => '%' '-'.
	p(NT(366), (T(39)+T(30)));
//G639: history_id(376)      => digits(12).
	p(NT(376), (NT(12)));
//G640: __E___E_history_172_173(375) => history_id(376).
	p(NT(375), (NT(376)));
//G641: __E___E_history_172_173(375) => null.
	p(NT(375), (nul));
//G642: __E_history_172(374) => rel_history_sym(366) __E___E_history_172_173(375).
	p(NT(374), (NT(366)+NT(375)));
//G643: rel_history(373)     => __E_history_172(374).
	p(NT(373), (NT(374)));
//G644: history(349)         => rel_history(373).
	p(NT(349), (NT(373)));
//G645: abs_history_sym(365) => '%'.
	p(NT(365), (T(39)));
//G646: __E_history_174(378) => abs_history_sym(365) history_id(376).
	p(NT(378), (NT(365)+NT(376)));
//G647: abs_history(377)     => __E_history_174(378).
	p(NT(377), (NT(378)));
//G648: history(349)         => abs_history(377).
	p(NT(349), (NT(377)));
//G649: unreachable(379)     => BDD_ID(380).
	p(NT(379), (NT(380)));
//G650: stream_def(381)      => input_def(55).
	p(NT(381), (NT(55)));
//G651: stream_def(381)      => output_def(56).
	p(NT(381), (NT(56)));
//G652: fm_or_term(382)      => wff(36).
	p(NT(382), (NT(36)));
//G653: fm_or_term(382)      => bf(38).
	p(NT(382), (NT(38)));
	#undef T
	#undef NT
	return loaded = true, p;
}

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace tau_parser_data

struct tau_parser_nonterminals {
	enum nonterminal {
		nul, eof, space, digit, xdigit, alpha, alnum, punct, printable, _, 
		__, sym, digits, type, subtype, integer, source, capture, var_name, io_var_name, 
		uconst_name, file_name, ctnvar, option_name, option_value, chars, q_var, src_c, uconst, spec_part, 
		inst_args, bf_cmd_arg, wff_cmd_arg, nf_cmd_arg, onf_cmd_arg, spec_arg, wff, wff_parenthesis, bf, bf_parenthesis, 
		bf_neg_oprnd, bf_cast_oprnd, bf_and_nosep_1st_oprnd, bf_and_nosep_2nd_oprnd, bf_and_nosep, bf_and, start, spec, __E_spec_0, definitions, 
		main, __E_spec_1, __E_definitions_2, __E___E_definitions_2_3, rec_relation, input_def, output_def, __E_definitions_4, spec_multiline, __E_spec_multiline_5, 
		__E_spec_multiline_6, __E_spec_multiline_7, ref, __E_rec_relation_8, __E_input_def_9, typed, stream, __E_output_def_10, q_file_name, console_sym, 
		__E_ref_11, offsets, ref_args, __E_ref_12, __E_ref_13, fp_fallback, first_sym, last_sym, __E_ref_args_14, __E___E_ref_args_14_15, 
		ref_arg, __E___E_ref_args_14_16, __E___E_ref_args_14_17, __E_wff_18, wff_sometimes, __E_wff_19, __E___E_wff_19_20, wff_always, __E_wff_21, __E___E_wff_21_22, 
		wff_conditional, __E_wff_23, wff_all, __E_wff_24, q_vars, wff_ex, __E_wff_25, wff_ref, constraint, wff_imply, 
		__E_wff_26, wff_rimply, __E_wff_27, wff_equiv, __E_wff_28, wff_or, __E_wff_29, wff_xor, __E_wff_30, wff_and, 
		__E_wff_31, wff_neg, __E_wff_32, wff_t, wff_f, bf_interval, __E_wff_33, bf_eq, __E_wff_34, bf_neq, 
		__E_wff_35, bf_lt, __E_wff_36, bf_nlt, __E_wff_37, bf_lteq, __E_wff_38, bf_nlteq, __E_wff_39, bf_gt, 
		__E_wff_40, bf_ngt, __E_wff_41, bf_gteq, __E_wff_42, bf_ngteq, __E_wff_43, __E_bf_44, ba_constant, variable, 
		bf_fall, __E_bf_45, bf_fex, __E_bf_46, bf_ref, bf_cast, __E_bf_47, num, __E___E_bf_47_48, bf_t, 
		bf_f, bf_neg, bf_shr, __E_bf_49, __E___E_bf_49_50, bf_shl, __E_bf_51, __E___E_bf_51_52, bf_add, __E_bf_53, 
		bf_sub, __E_bf_54, __E___E_bf_54_55, bf_mod, __E_bf_56, __E___E_bf_56_57, bf_mul, __E_bf_58, bf_div, __E_bf_59, 
		__E___E_bf_59_60, bf_nor, __E_bf_61, __E___E_bf_61_62, bf_xnor, __E_bf_63, __E___E_bf_63_64, bf_nand, __E_bf_65, __E___E_bf_65_66, 
		bf_or, __E_bf_67, bf_xor, __E_bf_68, __E_bf_69, __E___E_bf_69_70, __E_bf_71, __E___E_bf_71_72, __E_bf_73, __E___E_bf_73_74, 
		__E_bf_75, __E___E_bf_75_76, __E_bf_77, __E___E_bf_77_78, __E___E_bf_77_79, ctn_neq, __E_constraint_80, ctn_eq, __E_constraint_81, ctn_gteq, 
		__E_constraint_82, ctn_gt, __E_constraint_83, ctn_lteq, __E_constraint_84, ctn_lt, __E_constraint_85, __E_ba_constant_86, __E_ba_constant_87, __E_source_88, 
		__E___E_source_88_89, __E___E_source_88_90, __E_src_c_91, __E_src_c_92, __E_variable_93, io_var, __E_variable_94, __E_var_name_95, __E_var_name_96, __E_var_name_97, 
		__E_var_name_98, __E_var_name_99, offset, __E_uconst_100, __E___E_uconst_100_101, __E_q_vars_102, __E_q_vars_103, __E_offsets_104, __E_offsets_105, shift, 
		__E_offset_106, __E_shift_107, __E___E_shift_107_108, __E_chars_109, __E_chars_110, __E_digits_111, __E_integer_112, __E_typed_113, comment, __E_comment_114, 
		__E_comment_115, __E_comment_116, cli, cli_command, __E_cli_117, __E_cli_118, __E_cli_119, quit_cmd, quit_sym, __E_cli_command_120, 
		version_cmd, version_sym, __E_cli_command_121, clear_cmd, clear_sym, __E_cli_command_122, help_cmd, __E_cli_command_123, help_sym, __E___E_cli_command_123_124, 
		__E___E_cli_command_123_125, help_arg, file_cmd, __E_cli_command_126, file_sym, q_string, valid_cmd, __E_cli_command_127, valid_sym, sat_cmd, 
		__E_cli_command_128, sat_sym, unsat_cmd, __E_cli_command_129, unsat_sym, solve_cmd, __E_cli_command_130, solve_sym, __E___E_cli_command_130_131, solve_options, 
		lgrs_cmd, __E_cli_command_132, lgrs_sym, run_cmd, __E_cli_command_133, run_sym, __E___E_cli_command_133_134, normalize_cmd, __E_cli_command_135, normalize_sym, 
		__E___E_cli_command_135_136, subst_cmd, __E_cli_command_137, subst_sym, __E___E_cli_command_137_138, inst_cmd, __E_cli_command_139, inst_sym, __E___E_cli_command_139_140, dnf_cmd, 
		__E_cli_command_141, dnf_sym, cnf_cmd, __E_cli_command_142, cnf_sym, nnf_cmd, __E_cli_command_143, nnf_sym, mnf_cmd, __E_cli_command_144, 
		mnf_sym, onf_cmd, __E_cli_command_145, onf_sym, qelim_cmd, __E_cli_command_146, qelim_sym, get_cmd, __E_cli_command_147, get_sym, 
		__E___E_cli_command_147_148, set_cmd, __E_cli_command_149, set_sym, __E___E_cli_command_149_150, enable_cmd, __E_cli_command_151, enable_sym, disable_cmd, __E_cli_command_152, 
		disable_sym, toggle_cmd, __E_cli_command_153, toggle_sym, def_list_cmd, __E_cli_command_154, def_sym, __E___E_cli_command_154_155, def_print_cmd, __E_cli_command_156, 
		def_rr_cmd, def_input_cmd, def_output_cmd, history_list_cmd, __E_cli_command_157, history_sym, __E___E_cli_command_157_158, history_print_cmd, __E_cli_command_159, history, 
		history_store_cmd, __E_cli_command_160, __E_solve_options_161, solve_option, __E_solve_options_162, __E_solve_option_163, solver_mode, solver_mode_minimum, __E_solver_mode_164, solver_mode_maximum, 
		__E_solver_mode_165, __E_file_name_166, __E_option_name_167, __E_option_value_168, __E_option_value_169, abs_history_sym, rel_history_sym, selection_sym, execute_sym, snf_sym, 
		examples_sym, __E_help_arg_170, __E___E_help_arg_170_171, rel_history, __E_history_172, __E___E_history_172_173, history_id, abs_history, __E_history_174, unreachable, 
		BDD_ID, stream_def, fm_or_term, __N_0, __N_1, __N_2, __N_3, __N_4, __N_5, __N_6, 
		__N_7, __N_8, __N_9, __N_10, __N_11, __N_12, __N_13, __N_14, __N_15, __N_16, 
		__N_17, 
	};
};

struct tau_parser : public idni::parser<char, char>, public tau_parser_nonterminals {
	static tau_parser& instance() {
		static tau_parser inst;
		return inst;
	}
	tau_parser() : idni::parser<char_type, terminal_type>(
		tau_parser_data::grammar,
		tau_parser_data::make_parser_options()) {}
	size_t id(const std::basic_string<char_type>& name) {
		return tau_parser_data::nts.get(name);
	}
	const std::basic_string<char_type>& name(size_t id) {
		return tau_parser_data::nts.get(id);
	}
	symbol_type literal(const nonterminal& nt) {
		return symbol_type(nt, &tau_parser_data::nts);
	}
};

#endif // __TAU_PARSER_H__
