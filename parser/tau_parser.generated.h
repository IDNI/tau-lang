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
	"wff_F", "__E_wff_23", "wff_A", "__E_wff_24", "wff_E", "__E_wff_25", "wff_semantic_neg", "__E_wff_26", "wff_conditional", "__E_wff_27", 
	"wff_all", "__E_wff_28", "q_vars", "wff_ex", "__E_wff_29", "wff_ref", "constraint", "wff_imply", "__E_wff_30", "wff_rimply", 
	"__E_wff_31", "wff_equiv", "__E_wff_32", "wff_or", "__E_wff_33", "wff_xor", "__E_wff_34", "wff_and", "__E_wff_35", "wff_neg", 
	"__E_wff_36", "wff_t", "wff_f", "bf_interval", "__E_wff_37", "bf_eq", "__E_wff_38", "bf_neq", "__E_wff_39", "bf_lt", 
	"__E_wff_40", "bf_nlt", "__E_wff_41", "bf_lteq", "__E_wff_42", "bf_nlteq", "__E_wff_43", "bf_gt", "__E_wff_44", "bf_ngt", 
	"__E_wff_45", "bf_gteq", "__E_wff_46", "bf_ngteq", "__E_wff_47", "wff_U", "__E_wff_48", "wff_R", "__E_wff_49", "wff_W", 
	"__E_wff_50", "wff_S", "__E_wff_51", "wff_T", "__E_wff_52", "__E_bf_53", "ba_constant", "variable", "bf_fall", "__E_bf_54", 
	"bf_fex", "__E_bf_55", "bf_ref", "bf_cast", "__E_bf_56", "num", "__E___E_bf_56_57", "bf_t", "bf_f", "bf_neg", 
	"bf_func_app", "__E_bf_58", "func_sym", "func_args", "bf_shr", "__E_bf_59", "__E___E_bf_59_60", "bf_shl", "__E_bf_61", "__E___E_bf_61_62", 
	"bf_add", "__E_bf_63", "bf_sub", "__E_bf_64", "__E___E_bf_64_65", "bf_mod", "__E_bf_66", "__E___E_bf_66_67", "bf_mul", "__E_bf_68", 
	"bf_div", "__E_bf_69", "__E___E_bf_69_70", "bf_nor", "__E_bf_71", "__E___E_bf_71_72", "bf_xnor", "__E_bf_73", "__E___E_bf_73_74", "bf_nand", 
	"__E_bf_75", "__E___E_bf_75_76", "bf_or", "__E_bf_77", "bf_xor", "__E_bf_78", "__E_bf_79", "__E___E_bf_79_80", "__E_bf_81", "__E___E_bf_81_82", 
	"__E_bf_83", "__E___E_bf_83_84", "__E_bf_85", "__E___E_bf_85_86", "__E_bf_87", "__E___E_bf_87_88", "__E___E_bf_87_89", "__E_func_args_90", "__E_func_args_91", "func_mode", 
	"func_dynamic", "func_static", "func_sort_list", "__E_func_sort_list_92", "__E_func_sort_list_93", "func_decl", "ctn_neq", "__E_constraint_94", "ctn_eq", "__E_constraint_95", 
	"ctn_gteq", "__E_constraint_96", "ctn_gt", "__E_constraint_97", "ctn_lteq", "__E_constraint_98", "ctn_lt", "__E_constraint_99", "__E_ba_constant_100", "__E_ba_constant_101", 
	"__E_source_102", "__E___E_source_102_103", "__E___E_source_102_104", "__E_src_c_105", "__E_src_c_106", "__E_variable_107", "io_var", "__E_variable_108", "__E_var_name_109", "__E_var_name_110", 
	"__E_var_name_111", "__E_var_name_112", "__E_var_name_113", "offset", "__E_uconst_114", "__E___E_uconst_114_115", "__E_q_vars_116", "__E_q_vars_117", "__E_offsets_118", "__E_offsets_119", 
	"shift", "__E_offset_120", "__E_shift_121", "__E___E_shift_121_122", "__E_chars_123", "__E_chars_124", "__E_digits_125", "__E_integer_126", "__E_typed_127", "comment", 
	"__E_comment_128", "__E_comment_129", "__E_comment_130", "cli", "cli_command", "__E_cli_131", "__E_cli_132", "__E_cli_133", "quit_cmd", "quit_sym", 
	"__E_cli_command_134", "version_cmd", "version_sym", "__E_cli_command_135", "clear_cmd", "clear_sym", "__E_cli_command_136", "help_cmd", "__E_cli_command_137", "help_sym", 
	"__E___E_cli_command_137_138", "__E___E_cli_command_137_139", "help_arg", "file_cmd", "__E_cli_command_140", "file_sym", "q_string", "valid_cmd", "__E_cli_command_141", "valid_sym", 
	"sat_cmd", "__E_cli_command_142", "sat_sym", "unsat_cmd", "__E_cli_command_143", "unsat_sym", "solve_cmd", "__E_cli_command_144", "solve_sym", "__E___E_cli_command_144_145", 
	"solve_options", "lgrs_cmd", "__E_cli_command_146", "lgrs_sym", "run_cmd", "__E_cli_command_147", "run_sym", "__E___E_cli_command_147_148", "ltl_cmd", "__E_cli_command_149", 
	"ltl_sym", "func_cmd", "__E_cli_command_150", "func_sym_cmd", "normalize_cmd", "__E_cli_command_151", "normalize_sym", "__E___E_cli_command_151_152", "subst_cmd", "__E_cli_command_153", 
	"subst_sym", "__E___E_cli_command_153_154", "inst_cmd", "__E_cli_command_155", "inst_sym", "__E___E_cli_command_155_156", "dnf_cmd", "__E_cli_command_157", "dnf_sym", "cnf_cmd", 
	"__E_cli_command_158", "cnf_sym", "nnf_cmd", "__E_cli_command_159", "nnf_sym", "mnf_cmd", "__E_cli_command_160", "mnf_sym", "onf_cmd", "__E_cli_command_161", 
	"onf_sym", "qelim_cmd", "__E_cli_command_162", "qelim_sym", "whatis_cmd", "__E_cli_command_163", "whatis_sym", "reset_cmd", "__E_cli_command_164", "reset_sym", 
	"fragment_cmd", "__E_cli_command_165", "fragment_sym", "fragment_name", "get_cmd", "__E_cli_command_166", "get_sym", "__E___E_cli_command_166_167", "set_cmd", "__E_cli_command_168", 
	"set_sym", "__E___E_cli_command_168_169", "enable_cmd", "__E_cli_command_170", "enable_sym", "disable_cmd", "__E_cli_command_171", "disable_sym", "toggle_cmd", "__E_cli_command_172", 
	"toggle_sym", "def_list_cmd", "__E_cli_command_173", "def_sym", "__E___E_cli_command_173_174", "def_print_cmd", "__E_cli_command_175", "def_rr_cmd", "def_input_cmd", "def_output_cmd", 
	"history_list_cmd", "__E_cli_command_176", "history_sym", "__E___E_cli_command_176_177", "history_print_cmd", "__E_cli_command_178", "history", "history_store_cmd", "__E_cli_command_179", "__E_solve_options_180", 
	"solve_option", "__E_solve_options_181", "__E_solve_option_182", "solver_mode", "solver_mode_minimum", "__E_solver_mode_183", "solver_mode_maximum", "__E_solver_mode_184", "__E_file_name_185", "__E_option_name_186", 
	"__E_option_value_187", "__E_option_value_188", "__E_fragment_name_189", "fragment_ltl", "fragment_ctl_star", "abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", "snf_sym", 
	"examples_sym", "__E_help_arg_190", "__E___E_help_arg_190_191", "rel_history", "__E_history_192", "__E___E_history_192_193", "history_id", "abs_history", "__E_history_194", "unreachable", 
	"BDD_ID", "stream_def", "fm_or_term", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", 
	"__N_7", "__N_8", "__N_9", "__N_10", "__N_11", "__N_12", "__N_13", "__N_14", "__N_15", "__N_16", 
	"__N_17", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', 'a', 'b', 'k', 
	'r', ',', 'm', '<', '>', 'w', 'y', '[', ']', 'G', 
	'F', 'A', 'E', '-', '?', 'x', '|', '^', '&', '!', 
	'T', 'U', 'R', 'W', 'S', 'v', '+', '%', '*', '/', 
	'1', '0', '\'', 'd', '{', '}', '_', '$', '\t', '\n', 
	'\r', '#', 'q', 'h', 'p', 'g', 'z', '"', 
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
//G187: __E___E_bf_56_57(166) => bf_parenthesis(39).
	p(NT(166), (NT(39)));
//G188: __E___E_bf_56_57(166) => ba_constant(156).
	p(NT(166), (NT(156)));
//G189: __E___E_bf_56_57(166) => variable(157).
	p(NT(166), (NT(157)));
//G190: __E___E_bf_56_57(166) => bf_ref(162).
	p(NT(166), (NT(162)));
//G191: __E___E_bf_56_57(166) => bf_t(167).
	p(NT(166), (NT(167)));
//G192: __E___E_bf_56_57(166) => bf_f(168).
	p(NT(166), (NT(168)));
//G193: __E___E_bf_56_57(166) => bf_neg(169).
	p(NT(166), (NT(169)));
//G194: __E___E_bf_56_57(166) => bf_cast(163).
	p(NT(166), (NT(163)));
//G195: __E___E_bf_56_57(166) => capture(17).
	p(NT(166), (NT(17)));
//G196: bf_cast_oprnd(41)    => __E___E_bf_56_57(166).
	p(NT(41), (NT(166)));
//G197: __E_bf_56(164)       => '(' _(9) 'b' 'v' _(9) '[' _(9) num(165) _(9) ']' _(9) ')' _(9) bf_cast_oprnd(41).
	p(NT(164), (T(12)+NT(9)+T(17)+T(44)+NT(9)+T(26)+NT(9)+NT(165)+NT(9)+T(27)+NT(9)+T(13)+NT(9)+NT(41)));
//G198: bf_cast(163)         => __E_bf_56(164).
	p(NT(163), (NT(164)));
//G199: bf(38)               => bf_cast(163).
	p(NT(38), (NT(163)));
//G200: __E_bf_58(171)       => func_sym(172) '(' _(9) func_args(173) _(9) ')'.
	p(NT(171), (NT(172)+T(12)+NT(9)+NT(173)+NT(9)+T(13)));
//G201: bf_func_app(170)     => __E_bf_58(171).
	p(NT(170), (NT(171)));
//G202: bf(38)               => bf_func_app(170).
	p(NT(38), (NT(170)));
//G203: __N_0(433)           => bf_shr(174).
	p(NT(433), (NT(174)));
//G204: __E___E_bf_59_60(176) => bf(38) & ~( __N_0(433) ).	 # conjunctive
	p(NT(176), (NT(38)) & ~(NT(433)));
//G205: __E_bf_59(175)       => bf(38) _(9) '>' '>' _(9) __E___E_bf_59_60(176).
	p(NT(175), (NT(38)+NT(9)+T(23)+T(23)+NT(9)+NT(176)));
//G206: bf_shr(174)          => __E_bf_59(175).
	p(NT(174), (NT(175)));
//G207: bf(38)               => bf_shr(174).
	p(NT(38), (NT(174)));
//G208: __N_1(434)           => bf_shl(177).
	p(NT(434), (NT(177)));
//G209: __E___E_bf_61_62(179) => bf(38) & ~( __N_1(434) ).	 # conjunctive
	p(NT(179), (NT(38)) & ~(NT(434)));
//G210: __E_bf_61(178)       => bf(38) _(9) '<' '<' _(9) __E___E_bf_61_62(179).
	p(NT(178), (NT(38)+NT(9)+T(22)+T(22)+NT(9)+NT(179)));
//G211: bf_shl(177)          => __E_bf_61(178).
	p(NT(177), (NT(178)));
//G212: bf(38)               => bf_shl(177).
	p(NT(38), (NT(177)));
//G213: __E_bf_63(181)       => bf(38) _(9) '+' _(9) bf(38).
	p(NT(181), (NT(38)+NT(9)+T(45)+NT(9)+NT(38)));
//G214: bf_add(180)          => __E_bf_63(181).
	p(NT(180), (NT(181)));
//G215: bf(38)               => bf_add(180).
	p(NT(38), (NT(180)));
//G216: __N_2(435)           => bf_sub(182).
	p(NT(435), (NT(182)));
//G217: __E___E_bf_64_65(184) => bf(38) & ~( __N_2(435) ).	 # conjunctive
	p(NT(184), (NT(38)) & ~(NT(435)));
//G218: __E_bf_64(183)       => bf(38) _(9) '-' _(9) __E___E_bf_64_65(184).
	p(NT(183), (NT(38)+NT(9)+T(32)+NT(9)+NT(184)));
//G219: bf_sub(182)          => __E_bf_64(183).
	p(NT(182), (NT(183)));
//G220: bf(38)               => bf_sub(182).
	p(NT(38), (NT(182)));
//G221: __N_3(436)           => bf_mod(185).
	p(NT(436), (NT(185)));
//G222: __E___E_bf_66_67(187) => bf(38) & ~( __N_3(436) ).	 # conjunctive
	p(NT(187), (NT(38)) & ~(NT(436)));
//G223: __E_bf_66(186)       => bf(38) _(9) '%' _(9) __E___E_bf_66_67(187).
	p(NT(186), (NT(38)+NT(9)+T(46)+NT(9)+NT(187)));
//G224: bf_mod(185)          => __E_bf_66(186).
	p(NT(185), (NT(186)));
//G225: bf(38)               => bf_mod(185).
	p(NT(38), (NT(185)));
//G226: __E_bf_68(189)       => bf(38) _(9) '*' _(9) bf(38).
	p(NT(189), (NT(38)+NT(9)+T(47)+NT(9)+NT(38)));
//G227: bf_mul(188)          => __E_bf_68(189).
	p(NT(188), (NT(189)));
//G228: bf(38)               => bf_mul(188).
	p(NT(38), (NT(188)));
//G229: __N_4(437)           => bf_div(190).
	p(NT(437), (NT(190)));
//G230: __E___E_bf_69_70(192) => bf(38) & ~( __N_4(437) ).	 # conjunctive
	p(NT(192), (NT(38)) & ~(NT(437)));
//G231: __E_bf_69(191)       => bf(38) _(9) '/' _(9) __E___E_bf_69_70(192).
	p(NT(191), (NT(38)+NT(9)+T(48)+NT(9)+NT(192)));
//G232: bf_div(190)          => __E_bf_69(191).
	p(NT(190), (NT(191)));
//G233: bf(38)               => bf_div(190).
	p(NT(38), (NT(190)));
//G234: __N_5(438)           => bf_nor(193).
	p(NT(438), (NT(193)));
//G235: __E___E_bf_71_72(195) => bf(38) & ~( __N_5(438) ).	 # conjunctive
	p(NT(195), (NT(38)) & ~(NT(438)));
//G236: __E_bf_71(194)       => bf(38) _(9) '!' '|' _(9) __E___E_bf_71_72(195).
	p(NT(194), (NT(38)+NT(9)+T(38)+T(35)+NT(9)+NT(195)));
//G237: bf_nor(193)          => __E_bf_71(194).
	p(NT(193), (NT(194)));
//G238: bf(38)               => bf_nor(193).
	p(NT(38), (NT(193)));
//G239: __N_6(439)           => bf_xnor(196).
	p(NT(439), (NT(196)));
//G240: __E___E_bf_73_74(198) => bf(38) & ~( __N_6(439) ).	 # conjunctive
	p(NT(198), (NT(38)) & ~(NT(439)));
//G241: __E_bf_73(197)       => bf(38) _(9) '!' '^' _(9) __E___E_bf_73_74(198).
	p(NT(197), (NT(38)+NT(9)+T(38)+T(36)+NT(9)+NT(198)));
//G242: bf_xnor(196)         => __E_bf_73(197).
	p(NT(196), (NT(197)));
//G243: bf(38)               => bf_xnor(196).
	p(NT(38), (NT(196)));
//G244: __N_7(440)           => bf_nand(199).
	p(NT(440), (NT(199)));
//G245: __E___E_bf_75_76(201) => bf(38) & ~( __N_7(440) ).	 # conjunctive
	p(NT(201), (NT(38)) & ~(NT(440)));
//G246: __E_bf_75(200)       => bf(38) _(9) '!' '&' _(9) __E___E_bf_75_76(201).
	p(NT(200), (NT(38)+NT(9)+T(38)+T(37)+NT(9)+NT(201)));
//G247: bf_nand(199)         => __E_bf_75(200).
	p(NT(199), (NT(200)));
//G248: bf(38)               => bf_nand(199).
	p(NT(38), (NT(199)));
//G249: __E_bf_77(203)       => bf(38) _(9) '|' _(9) bf(38).
	p(NT(203), (NT(38)+NT(9)+T(35)+NT(9)+NT(38)));
//G250: bf_or(202)           => __E_bf_77(203).
	p(NT(202), (NT(203)));
//G251: bf(38)               => bf_or(202).
	p(NT(38), (NT(202)));
//G252: __E_bf_78(205)       => bf(38) _(9) '^' _(9) bf(38).
	p(NT(205), (NT(38)+NT(9)+T(36)+NT(9)+NT(38)));
//G253: bf_xor(204)          => __E_bf_78(205).
	p(NT(204), (NT(205)));
//G254: bf(38)               => bf_xor(204).
	p(NT(38), (NT(204)));
//G255: __E___E_bf_79_80(207) => typed(65).
	p(NT(207), (NT(65)));
//G256: __E___E_bf_79_80(207) => null.
	p(NT(207), (nul));
//G257: __E_bf_79(206)       => '1' __E___E_bf_79_80(207).
	p(NT(206), (T(49)+NT(207)));
//G258: bf_t(167)            => __E_bf_79(206).
	p(NT(167), (NT(206)));
//G259: bf(38)               => bf_t(167).
	p(NT(38), (NT(167)));
//G260: __E___E_bf_81_82(209) => typed(65).
	p(NT(209), (NT(65)));
//G261: __E___E_bf_81_82(209) => null.
	p(NT(209), (nul));
//G262: __E_bf_81(208)       => '0' __E___E_bf_81_82(209).
	p(NT(208), (T(50)+NT(209)));
//G263: bf_f(168)            => __E_bf_81(208).
	p(NT(168), (NT(208)));
//G264: bf(38)               => bf_f(168).
	p(NT(38), (NT(168)));
//G265: __E___E_bf_83_84(211) => _(9) '&' _(9).
	p(NT(211), (NT(9)+T(37)+NT(9)));
//G266: __E___E_bf_83_84(211) => __(10).
	p(NT(211), (NT(10)));
//G267: __E_bf_83(210)       => bf(38) __E___E_bf_83_84(211) bf(38).
	p(NT(210), (NT(38)+NT(211)+NT(38)));
//G268: bf_and(45)           => __E_bf_83(210).
	p(NT(45), (NT(210)));
//G269: bf(38)               => bf_and(45).
	p(NT(38), (NT(45)));
//G270: __E___E_bf_85_86(213) => bf_parenthesis(39).
	p(NT(213), (NT(39)));
//G271: __E___E_bf_85_86(213) => ba_constant(156).
	p(NT(213), (NT(156)));
//G272: __E___E_bf_85_86(213) => variable(157).
	p(NT(213), (NT(157)));
//G273: __E___E_bf_85_86(213) => bf_ref(162).
	p(NT(213), (NT(162)));
//G274: __E___E_bf_85_86(213) => bf_t(167).
	p(NT(213), (NT(167)));
//G275: __E___E_bf_85_86(213) => bf_f(168).
	p(NT(213), (NT(168)));
//G276: __E___E_bf_85_86(213) => bf_neg(169).
	p(NT(213), (NT(169)));
//G277: __E___E_bf_85_86(213) => capture(17).
	p(NT(213), (NT(17)));
//G278: bf_neg_oprnd(40)     => __E___E_bf_85_86(213).
	p(NT(40), (NT(213)));
//G279: __E_bf_85(212)       => bf_neg_oprnd(40) _(9) '\''.
	p(NT(212), (NT(40)+NT(9)+T(51)));
//G280: bf_neg(169)          => __E_bf_85(212).
	p(NT(169), (NT(212)));
//G281: bf(38)               => bf_neg(169).
	p(NT(38), (NT(169)));
//G282: __E___E_bf_87_88(215) => bf_parenthesis(39).
	p(NT(215), (NT(39)));
//G283: __E___E_bf_87_88(215) => variable(157).
	p(NT(215), (NT(157)));
//G284: __E___E_bf_87_88(215) => bf_ref(162).
	p(NT(215), (NT(162)));
//G285: __E___E_bf_87_88(215) => bf_neg(169).
	p(NT(215), (NT(169)));
//G286: __E___E_bf_87_88(215) => bf_and_nosep(44).
	p(NT(215), (NT(44)));
//G287: __E___E_bf_87_88(215) => capture(17).
	p(NT(215), (NT(17)));
//G288: bf_and_nosep_1st_oprnd(42) => __E___E_bf_87_88(215).
	p(NT(42), (NT(215)));
//G289: __E___E_bf_87_89(216) => bf_parenthesis(39).
	p(NT(216), (NT(39)));
//G290: __E___E_bf_87_89(216) => ba_constant(156).
	p(NT(216), (NT(156)));
//G291: __E___E_bf_87_89(216) => variable(157).
	p(NT(216), (NT(157)));
//G292: __E___E_bf_87_89(216) => bf_ref(162).
	p(NT(216), (NT(162)));
//G293: __E___E_bf_87_89(216) => bf_neg(169).
	p(NT(216), (NT(169)));
//G294: __E___E_bf_87_89(216) => capture(17).
	p(NT(216), (NT(17)));
//G295: bf_and_nosep_2nd_oprnd(43) => __E___E_bf_87_89(216).
	p(NT(43), (NT(216)));
//G296: __E_bf_87(214)       => bf_and_nosep_1st_oprnd(42) bf_and_nosep_2nd_oprnd(43).
	p(NT(214), (NT(42)+NT(43)));
//G297: bf_and_nosep(44)     => __E_bf_87(214).
	p(NT(44), (NT(214)));
//G298: bf(38)               => bf_and_nosep(44).
	p(NT(38), (NT(44)));
//G299: bf(38)               => capture(17).
	p(NT(38), (NT(17)));
//G300: func_sym(172)        => chars(25).
	p(NT(172), (NT(25)));
//G301: __E_func_args_90(217) => _(9) ',' _(9) bf(38).
	p(NT(217), (NT(9)+T(20)+NT(9)+NT(38)));
//G302: __E_func_args_91(218) => null.
	p(NT(218), (nul));
//G303: __E_func_args_91(218) => __E_func_args_90(217) __E_func_args_91(218).
	p(NT(218), (NT(217)+NT(218)));
//G304: func_args(173)       => bf(38) __E_func_args_91(218).
	p(NT(173), (NT(38)+NT(218)));
//G305: func_dynamic(220)    => 'd' 'y' 'n' 'a' 'm' 'i' 'c'.
	p(NT(220), (T(52)+T(25)+T(5)+T(16)+T(21)+T(4)+T(14)));
//G306: func_mode(219)       => func_dynamic(220).
	p(NT(219), (NT(220)));
//G307: func_static(221)     => 's' 't' 'a' 't' 'i' 'c'.
	p(NT(221), (T(15)+T(8)+T(16)+T(8)+T(4)+T(14)));
//G308: func_mode(219)       => func_static(221).
	p(NT(219), (NT(221)));
//G309: __E_func_sort_list_92(223) => _(9) ',' _(9) type(13).
	p(NT(223), (NT(9)+T(20)+NT(9)+NT(13)));
//G310: __E_func_sort_list_93(224) => null.
	p(NT(224), (nul));
//G311: __E_func_sort_list_93(224) => __E_func_sort_list_92(223) __E_func_sort_list_93(224).
	p(NT(224), (NT(223)+NT(224)));
//G312: func_sort_list(222)  => type(13) __E_func_sort_list_93(224).
	p(NT(222), (NT(13)+NT(224)));
//G313: func_decl(225)       => func_mode(219) __(10) func_sym(172) _(9) ':' _(9) func_sort_list(222) _(9) '-' '>' _(9) type(13).
	p(NT(225), (NT(219)+NT(10)+NT(172)+NT(9)+T(2)+NT(9)+NT(222)+NT(9)+T(32)+T(23)+NT(9)+NT(13)));
//G314: __E_constraint_94(227) => '[' ctnvar(22) _(9) '!' '=' _(9) num(165) ']'.
	p(NT(227), (T(26)+NT(22)+NT(9)+T(38)+T(3)+NT(9)+NT(165)+T(27)));
//G315: __E_constraint_94(227) => '[' num(165) _(9) '!' '=' _(9) ctnvar(22) ']'.
	p(NT(227), (T(26)+NT(165)+NT(9)+T(38)+T(3)+NT(9)+NT(22)+T(27)));
//G316: ctn_neq(226)         => __E_constraint_94(227).
	p(NT(226), (NT(227)));
//G317: constraint(106)      => ctn_neq(226).
	p(NT(106), (NT(226)));
//G318: __E_constraint_95(229) => '[' ctnvar(22) _(9) '=' _(9) num(165) ']'.
	p(NT(229), (T(26)+NT(22)+NT(9)+T(3)+NT(9)+NT(165)+T(27)));
//G319: __E_constraint_95(229) => '[' num(165) _(9) '=' _(9) ctnvar(22) ']'.
	p(NT(229), (T(26)+NT(165)+NT(9)+T(3)+NT(9)+NT(22)+T(27)));
//G320: ctn_eq(228)          => __E_constraint_95(229).
	p(NT(228), (NT(229)));
//G321: constraint(106)      => ctn_eq(228).
	p(NT(106), (NT(228)));
//G322: __E_constraint_96(231) => '[' ctnvar(22) _(9) '>' '=' _(9) num(165) ']'.
	p(NT(231), (T(26)+NT(22)+NT(9)+T(23)+T(3)+NT(9)+NT(165)+T(27)));
//G323: __E_constraint_96(231) => '[' num(165) _(9) '>' '=' _(9) ctnvar(22) ']'.
	p(NT(231), (T(26)+NT(165)+NT(9)+T(23)+T(3)+NT(9)+NT(22)+T(27)));
//G324: ctn_gteq(230)        => __E_constraint_96(231).
	p(NT(230), (NT(231)));
//G325: constraint(106)      => ctn_gteq(230).
	p(NT(106), (NT(230)));
//G326: __E_constraint_97(233) => '[' ctnvar(22) _(9) '>' _(9) num(165) ']'.
	p(NT(233), (T(26)+NT(22)+NT(9)+T(23)+NT(9)+NT(165)+T(27)));
//G327: __E_constraint_97(233) => '[' num(165) _(9) '>' _(9) ctnvar(22) ']'.
	p(NT(233), (T(26)+NT(165)+NT(9)+T(23)+NT(9)+NT(22)+T(27)));
//G328: ctn_gt(232)          => __E_constraint_97(233).
	p(NT(232), (NT(233)));
//G329: constraint(106)      => ctn_gt(232).
	p(NT(106), (NT(232)));
//G330: __E_constraint_98(235) => '[' ctnvar(22) _(9) '<' '=' _(9) num(165) ']'.
	p(NT(235), (T(26)+NT(22)+NT(9)+T(22)+T(3)+NT(9)+NT(165)+T(27)));
//G331: __E_constraint_98(235) => '[' num(165) _(9) '<' '=' _(9) ctnvar(22) ']'.
	p(NT(235), (T(26)+NT(165)+NT(9)+T(22)+T(3)+NT(9)+NT(22)+T(27)));
//G332: ctn_lteq(234)        => __E_constraint_98(235).
	p(NT(234), (NT(235)));
//G333: constraint(106)      => ctn_lteq(234).
	p(NT(106), (NT(234)));
//G334: __E_constraint_99(237) => '[' ctnvar(22) _(9) '<' _(9) num(165) ']'.
	p(NT(237), (T(26)+NT(22)+NT(9)+T(22)+NT(9)+NT(165)+T(27)));
//G335: __E_constraint_99(237) => '[' num(165) _(9) '<' _(9) ctnvar(22) ']'.
	p(NT(237), (T(26)+NT(165)+NT(9)+T(22)+NT(9)+NT(22)+T(27)));
//G336: ctn_lt(236)          => __E_constraint_99(237).
	p(NT(236), (NT(237)));
//G337: constraint(106)      => ctn_lt(236).
	p(NT(106), (NT(236)));
//G338: __E_ba_constant_100(238) => capture(17).
	p(NT(238), (NT(17)));
//G339: __E_ba_constant_100(238) => source(16).
	p(NT(238), (NT(16)));
//G340: __E_ba_constant_101(239) => typed(65).
	p(NT(239), (NT(65)));
//G341: __E_ba_constant_101(239) => null.
	p(NT(239), (nul));
//G342: ba_constant(156)     => '{' _(9) __E_ba_constant_100(238) _(9) '}' __E_ba_constant_101(239).
	p(NT(156), (T(53)+NT(9)+NT(238)+NT(9)+T(54)+NT(239)));
//G343: __E___E_source_102_103(241) => src_c(27).
	p(NT(241), (NT(27)));
//G344: __E___E_source_102_103(241) => space(2).
	p(NT(241), (NT(2)));
//G345: __E___E_source_102_104(242) => null.
	p(NT(242), (nul));
//G346: __E___E_source_102_104(242) => __E___E_source_102_103(241) __E___E_source_102_104(242).
	p(NT(242), (NT(241)+NT(242)));
//G347: __E_source_102(240)  => __E___E_source_102_104(242) src_c(27).
	p(NT(240), (NT(242)+NT(27)));
//G348: __E_source_102(240)  => null.
	p(NT(240), (nul));
//G349: source(16)           => src_c(27) __E_source_102(240).
	p(NT(16), (NT(27)+NT(240)));
//G350: src_c(27)            => alnum(6).
	p(NT(27), (NT(6)));
//G351: __N_8(441)           => '{'.
	p(NT(441), (T(53)));
//G352: __N_9(442)           => '}'.
	p(NT(442), (T(54)));
//G353: src_c(27)            => ~( __N_8(441) ) & ~( __N_9(442) ) & punct(7).	 # conjunctive
	p(NT(27), ~(NT(441)) & ~(NT(442)) & (NT(7)));
//G354: __E_src_c_105(243)   => src_c(27).
	p(NT(243), (NT(27)));
//G355: __E_src_c_105(243)   => space(2).
	p(NT(243), (NT(2)));
//G356: __E_src_c_106(244)   => null.
	p(NT(244), (nul));
//G357: __E_src_c_106(244)   => __E_src_c_105(243) __E_src_c_106(244).
	p(NT(244), (NT(243)+NT(244)));
//G358: src_c(27)            => '{' __E_src_c_106(244) '}'.
	p(NT(27), (T(53)+NT(244)+T(54)));
//G359: __E_variable_107(245) => uconst(28).
	p(NT(245), (NT(28)));
//G360: __E_variable_107(245) => io_var(246).
	p(NT(245), (NT(246)));
//G361: __E_variable_107(245) => var_name(18).
	p(NT(245), (NT(18)));
//G362: __E_variable_108(247) => typed(65).
	p(NT(247), (NT(65)));
//G363: __E_variable_108(247) => null.
	p(NT(247), (nul));
//G364: variable(157)        => __E_variable_107(245) __E_variable_108(247).
	p(NT(157), (NT(245)+NT(247)));
//G365: __N_10(443)          => 'F'.
	p(NT(443), (T(29)));
//G366: __N_11(444)          => 'T'.
	p(NT(444), (T(39)));
//G367: __E_var_name_109(248) => ~( __N_10(443) ) & ~( __N_11(444) ) & alpha(5).	 # conjunctive
	p(NT(248), ~(NT(443)) & ~(NT(444)) & (NT(5)));
//G368: __E_var_name_110(249) => null.
	p(NT(249), (nul));
//G369: __E_var_name_110(249) => digit(3) __E_var_name_110(249).
	p(NT(249), (NT(3)+NT(249)));
//G370: var_name(18)         => __E_var_name_109(248) __E_var_name_110(249).	 # guarded: charvar
	p(NT(18), (NT(248)+NT(249)));
	p.back().guard = "charvar";
//G371: __N_12(445)          => 'F'.
	p(NT(445), (T(29)));
//G372: __N_13(446)          => 'T'.
	p(NT(446), (T(39)));
//G373: __E_var_name_111(250) => ~( __N_12(445) ) & ~( __N_13(446) ) & alpha(5).	 # conjunctive
	p(NT(250), ~(NT(445)) & ~(NT(446)) & (NT(5)));
//G374: __E_var_name_112(251) => alnum(6).
	p(NT(251), (NT(6)));
//G375: __E_var_name_112(251) => '_'.
	p(NT(251), (T(55)));
//G376: __E_var_name_113(252) => null.
	p(NT(252), (nul));
//G377: __E_var_name_113(252) => __E_var_name_112(251) __E_var_name_113(252).
	p(NT(252), (NT(251)+NT(252)));
//G378: var_name(18)         => __E_var_name_111(250) __E_var_name_113(252).	 # guarded: var
	p(NT(18), (NT(250)+NT(252)));
	p.back().guard = "var";
//G379: io_var(246)          => io_var_name(19) '[' offset(253) ']'.
	p(NT(246), (NT(19)+T(26)+NT(253)+T(27)));
//G380: io_var_name(19)      => chars(25).
	p(NT(19), (NT(25)));
//G381: __E___E_uconst_114_115(255) => chars(25) _(9).
	p(NT(255), (NT(25)+NT(9)));
//G382: __E___E_uconst_114_115(255) => null.
	p(NT(255), (nul));
//G383: __E_uconst_114(254)  => __E___E_uconst_114_115(255) ':' _(9) chars(25).
	p(NT(254), (NT(255)+T(2)+NT(9)+NT(25)));
//G384: uconst_name(20)      => __E_uconst_114(254).
	p(NT(20), (NT(254)));
//G385: uconst(28)           => '<' _(9) uconst_name(20) _(9) '>'.
	p(NT(28), (T(22)+NT(9)+NT(20)+NT(9)+T(23)));
//G386: __E_q_vars_116(256)  => _(9) ',' _(9) q_var(26).
	p(NT(256), (NT(9)+T(20)+NT(9)+NT(26)));
//G387: __E_q_vars_117(257)  => null.
	p(NT(257), (nul));
//G388: __E_q_vars_117(257)  => __E_q_vars_116(256) __E_q_vars_117(257).
	p(NT(257), (NT(256)+NT(257)));
//G389: q_vars(102)          => q_var(26) __E_q_vars_117(257).
	p(NT(102), (NT(26)+NT(257)));
//G390: q_var(26)            => capture(17).
	p(NT(26), (NT(17)));
//G391: __N_14(447)          => uconst(28).
	p(NT(447), (NT(28)));
//G392: q_var(26)            => ~( __N_14(447) ) & variable(157).	 # conjunctive
	p(NT(26), ~(NT(447)) & (NT(157)));
//G393: ctnvar(22)           => chars(25).
	p(NT(22), (NT(25)));
//G394: __E_offsets_118(258) => _(9) ',' _(9) offset(253).
	p(NT(258), (NT(9)+T(20)+NT(9)+NT(253)));
//G395: __E_offsets_119(259) => null.
	p(NT(259), (nul));
//G396: __E_offsets_119(259) => __E_offsets_118(258) __E_offsets_119(259).
	p(NT(259), (NT(258)+NT(259)));
//G397: offsets(71)          => '[' _(9) offset(253) __E_offsets_119(259) _(9) ']'.
	p(NT(71), (T(26)+NT(9)+NT(253)+NT(259)+NT(9)+T(27)));
//G398: offset(253)          => integer(15).
	p(NT(253), (NT(15)));
//G399: offset(253)          => capture(17).
	p(NT(253), (NT(17)));
//G400: offset(253)          => shift(260).
	p(NT(253), (NT(260)));
//G401: __N_15(448)          => io_var(246).
	p(NT(448), (NT(246)));
//G402: __E_offset_120(261)  => variable(157) & ~( __N_15(448) ).	 # conjunctive
	p(NT(261), (NT(157)) & ~(NT(448)));
//G403: offset(253)          => __E_offset_120(261).
	p(NT(253), (NT(261)));
//G404: __E_shift_121(262)   => capture(17).
	p(NT(262), (NT(17)));
//G405: __N_16(449)          => io_var(246).
	p(NT(449), (NT(246)));
//G406: __E___E_shift_121_122(263) => variable(157) & ~( __N_16(449) ).	 # conjunctive
	p(NT(263), (NT(157)) & ~(NT(449)));
//G407: __E_shift_121(262)   => __E___E_shift_121_122(263).
	p(NT(262), (NT(263)));
//G408: shift(260)           => __E_shift_121(262) _(9) '-' _(9) num(165).
	p(NT(260), (NT(262)+NT(9)+T(32)+NT(9)+NT(165)));
//G409: __E_chars_123(264)   => alnum(6).
	p(NT(264), (NT(6)));
//G410: __E_chars_123(264)   => '_'.
	p(NT(264), (T(55)));
//G411: __E_chars_124(265)   => null.
	p(NT(265), (nul));
//G412: __E_chars_124(265)   => __E_chars_123(264) __E_chars_124(265).
	p(NT(265), (NT(264)+NT(265)));
//G413: chars(25)            => alpha(5) __E_chars_124(265).
	p(NT(25), (NT(5)+NT(265)));
//G414: __E_digits_125(266)  => digit(3).
	p(NT(266), (NT(3)));
//G415: __E_digits_125(266)  => digit(3) __E_digits_125(266).
	p(NT(266), (NT(3)+NT(266)));
//G416: digits(12)           => __E_digits_125(266).
	p(NT(12), (NT(266)));
//G417: num(165)             => digits(12).
	p(NT(165), (NT(12)));
//G418: __E_integer_126(267) => '-'.
	p(NT(267), (T(32)));
//G419: __E_integer_126(267) => null.
	p(NT(267), (nul));
//G420: integer(15)          => __E_integer_126(267) _(9) digits(12).
	p(NT(15), (NT(267)+NT(9)+NT(12)));
//G421: sym(11)              => chars(25).
	p(NT(11), (NT(25)));
//G422: capture(17)          => '$' chars(25).
	p(NT(17), (T(56)+NT(25)));
//G423: typed(65)            => _(9) ':' _(9) type(13) _(9) '[' _(9) subtype(14) _(9) ']'.
	p(NT(65), (NT(9)+T(2)+NT(9)+NT(13)+NT(9)+T(26)+NT(9)+NT(14)+NT(9)+T(27)));
//G424: __N_17(450)          => 'b' 'v'.
	p(NT(450), (T(17)+T(44)));
//G425: __E_typed_127(268)   => ~( __N_17(450) ) & type(13).	 # conjunctive
	p(NT(268), ~(NT(450)) & (NT(13)));
//G426: typed(65)            => _(9) ':' _(9) __E_typed_127(268).
	p(NT(65), (NT(9)+T(2)+NT(9)+NT(268)));
//G427: type(13)             => chars(25).
	p(NT(13), (NT(25)));
//G428: subtype(14)          => num(165).
	p(NT(14), (NT(165)));
//G429: __E_comment_128(270) => printable(8).
	p(NT(270), (NT(8)));
//G430: __E_comment_128(270) => '\t'.
	p(NT(270), (T(57)));
//G431: __E_comment_129(271) => null.
	p(NT(271), (nul));
//G432: __E_comment_129(271) => __E_comment_128(270) __E_comment_129(271).
	p(NT(271), (NT(270)+NT(271)));
//G433: __E_comment_130(272) => '\n'.
	p(NT(272), (T(58)));
//G434: __E_comment_130(272) => '\r'.
	p(NT(272), (T(59)));
//G435: __E_comment_130(272) => eof(1).
	p(NT(272), (NT(1)));
//G436: comment(269)         => '#' __E_comment_129(271) __E_comment_130(272).
	p(NT(269), (T(60)+NT(271)+NT(272)));
//G437: __(10)               => space(2).
	p(NT(10), (NT(2)));
//G438: __(10)               => comment(269).
	p(NT(10), (NT(269)));
//G439: __(10)               => __(10) space(2).
	p(NT(10), (NT(10)+NT(2)));
//G440: __(10)               => __(10) comment(269).
	p(NT(10), (NT(10)+NT(269)));
//G441: _(9)                 => __(10).
	p(NT(9), (NT(10)));
//G442: _(9)                 => null.
	p(NT(9), (nul));
//G443: cli(273)             => _(9).
	p(NT(273), (NT(9)));
//G444: __E_cli_131(275)     => '.' _(9) cli_command(274) _(9).
	p(NT(275), (T(1)+NT(9)+NT(274)+NT(9)));
//G445: __E_cli_132(276)     => null.
	p(NT(276), (nul));
//G446: __E_cli_132(276)     => __E_cli_131(275) __E_cli_132(276).
	p(NT(276), (NT(275)+NT(276)));
//G447: __E_cli_133(277)     => '.' _(9).
	p(NT(277), (T(1)+NT(9)));
//G448: __E_cli_133(277)     => null.
	p(NT(277), (nul));
//G449: cli(273)             => _(9) cli_command(274) _(9) __E_cli_132(276) __E_cli_133(277).
	p(NT(273), (NT(9)+NT(274)+NT(9)+NT(276)+NT(277)));
//G450: __E_cli_command_134(280) => 'q'.
	p(NT(280), (T(61)));
//G451: __E_cli_command_134(280) => 'q' 'u' 'i' 't'.
	p(NT(280), (T(61)+T(7)+T(4)+T(8)));
//G452: quit_sym(279)        => __E_cli_command_134(280).
	p(NT(279), (NT(280)));
//G453: quit_cmd(278)        => quit_sym(279).
	p(NT(278), (NT(279)));
//G454: cli_command(274)     => quit_cmd(278).
	p(NT(274), (NT(278)));
//G455: __E_cli_command_135(283) => 'v'.
	p(NT(283), (T(44)));
//G456: __E_cli_command_135(283) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(283), (T(44)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G457: version_sym(282)     => __E_cli_command_135(283).
	p(NT(282), (NT(283)));
//G458: version_cmd(281)     => version_sym(282).
	p(NT(281), (NT(282)));
//G459: cli_command(274)     => version_cmd(281).
	p(NT(274), (NT(281)));
//G460: __E_cli_command_136(286) => 'c'.
	p(NT(286), (T(14)));
//G461: __E_cli_command_136(286) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(286), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G462: clear_sym(285)       => __E_cli_command_136(286).
	p(NT(285), (NT(286)));
//G463: clear_cmd(284)       => clear_sym(285).
	p(NT(284), (NT(285)));
//G464: cli_command(274)     => clear_cmd(284).
	p(NT(274), (NT(284)));
//G465: __E___E_cli_command_137_138(290) => 'h'.
	p(NT(290), (T(62)));
//G466: __E___E_cli_command_137_138(290) => 'h' 'e' 'l' 'p'.
	p(NT(290), (T(62)+T(11)+T(10)+T(63)));
//G467: help_sym(289)        => __E___E_cli_command_137_138(290).
	p(NT(289), (NT(290)));
//G468: __E___E_cli_command_137_139(291) => __(10) help_arg(292).
	p(NT(291), (NT(10)+NT(292)));
//G469: __E___E_cli_command_137_139(291) => null.
	p(NT(291), (nul));
//G470: __E_cli_command_137(288) => help_sym(289) __E___E_cli_command_137_139(291).
	p(NT(288), (NT(289)+NT(291)));
//G471: help_cmd(287)        => __E_cli_command_137(288).
	p(NT(287), (NT(288)));
//G472: cli_command(274)     => help_cmd(287).
	p(NT(274), (NT(287)));
//G473: file_sym(295)        => 'f' 'i' 'l' 'e'.
	p(NT(295), (T(9)+T(4)+T(10)+T(11)));
//G474: __E_cli_command_140(294) => file_sym(295) __(10) q_string(296).
	p(NT(294), (NT(295)+NT(10)+NT(296)));
//G475: file_cmd(293)        => __E_cli_command_140(294).
	p(NT(293), (NT(294)));
//G476: cli_command(274)     => file_cmd(293).
	p(NT(274), (NT(293)));
//G477: valid_sym(299)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(299), (T(44)+T(16)+T(10)+T(4)+T(52)));
//G478: __E_cli_command_141(298) => valid_sym(299) __(10) spec_arg(35).
	p(NT(298), (NT(299)+NT(10)+NT(35)));
//G479: valid_cmd(297)       => __E_cli_command_141(298).
	p(NT(297), (NT(298)));
//G480: cli_command(274)     => valid_cmd(297).
	p(NT(274), (NT(297)));
//G481: sat_sym(302)         => 's' 'a' 't'.
	p(NT(302), (T(15)+T(16)+T(8)));
//G482: __E_cli_command_142(301) => sat_sym(302) __(10) spec_arg(35).
	p(NT(301), (NT(302)+NT(10)+NT(35)));
//G483: sat_cmd(300)         => __E_cli_command_142(301).
	p(NT(300), (NT(301)));
//G484: cli_command(274)     => sat_cmd(300).
	p(NT(274), (NT(300)));
//G485: unsat_sym(305)       => 'u' 'n' 's' 'a' 't'.
	p(NT(305), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G486: __E_cli_command_143(304) => unsat_sym(305) __(10) spec_arg(35).
	p(NT(304), (NT(305)+NT(10)+NT(35)));
//G487: unsat_cmd(303)       => __E_cli_command_143(304).
	p(NT(303), (NT(304)));
//G488: cli_command(274)     => unsat_cmd(303).
	p(NT(274), (NT(303)));
//G489: solve_sym(308)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(308), (T(15)+T(6)+T(10)+T(44)+T(11)));
//G490: __E___E_cli_command_144_145(309) => solve_options(310).
	p(NT(309), (NT(310)));
//G491: __E___E_cli_command_144_145(309) => null.
	p(NT(309), (nul));
//G492: __E_cli_command_144(307) => solve_sym(308) __E___E_cli_command_144_145(309) __(10) wff_cmd_arg(32).
	p(NT(307), (NT(308)+NT(309)+NT(10)+NT(32)));
//G493: solve_cmd(306)       => __E_cli_command_144(307).
	p(NT(306), (NT(307)));
//G494: cli_command(274)     => solve_cmd(306).
	p(NT(274), (NT(306)));
//G495: lgrs_sym(313)        => 'l' 'g' 'r' 's'.
	p(NT(313), (T(10)+T(64)+T(19)+T(15)));
//G496: __E_cli_command_146(312) => lgrs_sym(313) __(10) wff_cmd_arg(32).
	p(NT(312), (NT(313)+NT(10)+NT(32)));
//G497: lgrs_cmd(311)        => __E_cli_command_146(312).
	p(NT(311), (NT(312)));
//G498: cli_command(274)     => lgrs_cmd(311).
	p(NT(274), (NT(311)));
//G499: __E___E_cli_command_147_148(317) => 'r'.
	p(NT(317), (T(19)));
//G500: __E___E_cli_command_147_148(317) => 'r' 'u' 'n'.
	p(NT(317), (T(19)+T(7)+T(5)));
//G501: run_sym(316)         => __E___E_cli_command_147_148(317).
	p(NT(316), (NT(317)));
//G502: __E_cli_command_147(315) => run_sym(316) __(10) spec_arg(35).
	p(NT(315), (NT(316)+NT(10)+NT(35)));
//G503: run_cmd(314)         => __E_cli_command_147(315).
	p(NT(314), (NT(315)));
//G504: cli_command(274)     => run_cmd(314).
	p(NT(274), (NT(314)));
//G505: ltl_sym(320)         => 'l' 't' 'l'.
	p(NT(320), (T(10)+T(8)+T(10)));
//G506: __E_cli_command_149(319) => ltl_sym(320) __(10) spec_arg(35).
	p(NT(319), (NT(320)+NT(10)+NT(35)));
//G507: ltl_cmd(318)         => __E_cli_command_149(319).
	p(NT(318), (NT(319)));
//G508: cli_command(274)     => ltl_cmd(318).
	p(NT(274), (NT(318)));
//G509: func_sym_cmd(323)    => 'f' 'u' 'n' 'c'.
	p(NT(323), (T(9)+T(7)+T(5)+T(14)));
//G510: __E_cli_command_150(322) => func_sym_cmd(323) __(10) func_decl(225).
	p(NT(322), (NT(323)+NT(10)+NT(225)));
//G511: func_cmd(321)        => __E_cli_command_150(322).
	p(NT(321), (NT(322)));
//G512: cli_command(274)     => func_cmd(321).
	p(NT(274), (NT(321)));
//G513: __E___E_cli_command_151_152(327) => 'n'.
	p(NT(327), (T(5)));
//G514: __E___E_cli_command_151_152(327) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(327), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(65)+T(11)));
//G515: normalize_sym(326)   => __E___E_cli_command_151_152(327).
	p(NT(326), (NT(327)));
//G516: __E_cli_command_151(325) => normalize_sym(326) __(10) spec_arg(35).
	p(NT(325), (NT(326)+NT(10)+NT(35)));
//G517: normalize_cmd(324)   => __E_cli_command_151(325).
	p(NT(324), (NT(325)));
//G518: cli_command(274)     => normalize_cmd(324).
	p(NT(274), (NT(324)));
//G519: __E___E_cli_command_153_154(331) => 's'.
	p(NT(331), (T(15)));
//G520: __E___E_cli_command_153_154(331) => 's' 'u' 'b' 's' 't'.
	p(NT(331), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G521: __E___E_cli_command_153_154(331) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(331), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G522: subst_sym(330)       => __E___E_cli_command_153_154(331).
	p(NT(330), (NT(331)));
//G523: __E_cli_command_153(329) => subst_sym(330) __(10) nf_cmd_arg(33) _(9) '[' _(9) nf_cmd_arg(33) _(9) '/' _(9) nf_cmd_arg(33) _(9) ']'.
	p(NT(329), (NT(330)+NT(10)+NT(33)+NT(9)+T(26)+NT(9)+NT(33)+NT(9)+T(48)+NT(9)+NT(33)+NT(9)+T(27)));
//G524: subst_cmd(328)       => __E_cli_command_153(329).
	p(NT(328), (NT(329)));
//G525: cli_command(274)     => subst_cmd(328).
	p(NT(274), (NT(328)));
//G526: __E___E_cli_command_155_156(335) => 'i'.
	p(NT(335), (T(4)));
//G527: __E___E_cli_command_155_156(335) => 'i' 'n' 's' 't'.
	p(NT(335), (T(4)+T(5)+T(15)+T(8)));
//G528: __E___E_cli_command_155_156(335) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(335), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G529: inst_sym(334)        => __E___E_cli_command_155_156(335).
	p(NT(334), (NT(335)));
//G530: __E_cli_command_155(333) => inst_sym(334) __(10) inst_args(30).
	p(NT(333), (NT(334)+NT(10)+NT(30)));
//G531: inst_cmd(332)        => __E_cli_command_155(333).
	p(NT(332), (NT(333)));
//G532: cli_command(274)     => inst_cmd(332).
	p(NT(274), (NT(332)));
//G533: dnf_sym(338)         => 'd' 'n' 'f'.
	p(NT(338), (T(52)+T(5)+T(9)));
//G534: __E_cli_command_157(337) => dnf_sym(338) __(10) nf_cmd_arg(33).
	p(NT(337), (NT(338)+NT(10)+NT(33)));
//G535: dnf_cmd(336)         => __E_cli_command_157(337).
	p(NT(336), (NT(337)));
//G536: cli_command(274)     => dnf_cmd(336).
	p(NT(274), (NT(336)));
//G537: cnf_sym(341)         => 'c' 'n' 'f'.
	p(NT(341), (T(14)+T(5)+T(9)));
//G538: __E_cli_command_158(340) => cnf_sym(341) __(10) nf_cmd_arg(33).
	p(NT(340), (NT(341)+NT(10)+NT(33)));
//G539: cnf_cmd(339)         => __E_cli_command_158(340).
	p(NT(339), (NT(340)));
//G540: cli_command(274)     => cnf_cmd(339).
	p(NT(274), (NT(339)));
//G541: nnf_sym(344)         => 'n' 'n' 'f'.
	p(NT(344), (T(5)+T(5)+T(9)));
//G542: __E_cli_command_159(343) => nnf_sym(344) __(10) nf_cmd_arg(33).
	p(NT(343), (NT(344)+NT(10)+NT(33)));
//G543: nnf_cmd(342)         => __E_cli_command_159(343).
	p(NT(342), (NT(343)));
//G544: cli_command(274)     => nnf_cmd(342).
	p(NT(274), (NT(342)));
//G545: mnf_sym(347)         => 'm' 'n' 'f'.
	p(NT(347), (T(21)+T(5)+T(9)));
//G546: __E_cli_command_160(346) => mnf_sym(347) __(10) nf_cmd_arg(33).
	p(NT(346), (NT(347)+NT(10)+NT(33)));
//G547: mnf_cmd(345)         => __E_cli_command_160(346).
	p(NT(345), (NT(346)));
//G548: cli_command(274)     => mnf_cmd(345).
	p(NT(274), (NT(345)));
//G549: onf_sym(350)         => 'o' 'n' 'f'.
	p(NT(350), (T(6)+T(5)+T(9)));
//G550: __E_cli_command_161(349) => onf_sym(350) __(10) variable(157) __(10) onf_cmd_arg(34).
	p(NT(349), (NT(350)+NT(10)+NT(157)+NT(10)+NT(34)));
//G551: onf_cmd(348)         => __E_cli_command_161(349).
	p(NT(348), (NT(349)));
//G552: cli_command(274)     => onf_cmd(348).
	p(NT(274), (NT(348)));
//G553: qelim_sym(353)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(353), (T(61)+T(11)+T(10)+T(4)+T(21)));
//G554: __E_cli_command_162(352) => qelim_sym(353) __(10) wff_cmd_arg(32).
	p(NT(352), (NT(353)+NT(10)+NT(32)));
//G555: qelim_cmd(351)       => __E_cli_command_162(352).
	p(NT(351), (NT(352)));
//G556: cli_command(274)     => qelim_cmd(351).
	p(NT(274), (NT(351)));
//G557: whatis_sym(356)      => 'w' 'h' 'a' 't' 'i' 's'.
	p(NT(356), (T(24)+T(62)+T(16)+T(8)+T(4)+T(15)));
//G558: __E_cli_command_163(355) => whatis_sym(356) __(10) spec_arg(35).
	p(NT(355), (NT(356)+NT(10)+NT(35)));
//G559: whatis_cmd(354)      => __E_cli_command_163(355).
	p(NT(354), (NT(355)));
//G560: cli_command(274)     => whatis_cmd(354).
	p(NT(274), (NT(354)));
//G561: reset_sym(359)       => 'r' 'e' 's' 'e' 't'.
	p(NT(359), (T(19)+T(11)+T(15)+T(11)+T(8)));
//G562: __E_cli_command_164(358) => reset_sym(359).
	p(NT(358), (NT(359)));
//G563: reset_cmd(357)       => __E_cli_command_164(358).
	p(NT(357), (NT(358)));
//G564: cli_command(274)     => reset_cmd(357).
	p(NT(274), (NT(357)));
//G565: fragment_sym(362)    => 'f' 'r' 'a' 'g' 'm' 'e' 'n' 't'.
	p(NT(362), (T(9)+T(19)+T(16)+T(64)+T(21)+T(11)+T(5)+T(8)));
//G566: __E_cli_command_165(361) => fragment_sym(362) __(10) fragment_name(363).
	p(NT(361), (NT(362)+NT(10)+NT(363)));
//G567: fragment_cmd(360)    => __E_cli_command_165(361).
	p(NT(360), (NT(361)));
//G568: cli_command(274)     => fragment_cmd(360).
	p(NT(274), (NT(360)));
//G569: get_sym(366)         => 'g' 'e' 't'.
	p(NT(366), (T(64)+T(11)+T(8)));
//G570: __E___E_cli_command_166_167(367) => __(10) option_name(23).
	p(NT(367), (NT(10)+NT(23)));
//G571: __E___E_cli_command_166_167(367) => null.
	p(NT(367), (nul));
//G572: __E_cli_command_166(365) => get_sym(366) __E___E_cli_command_166_167(367).
	p(NT(365), (NT(366)+NT(367)));
//G573: get_cmd(364)         => __E_cli_command_166(365).
	p(NT(364), (NT(365)));
//G574: cli_command(274)     => get_cmd(364).
	p(NT(274), (NT(364)));
//G575: set_sym(370)         => 's' 'e' 't'.
	p(NT(370), (T(15)+T(11)+T(8)));
//G576: __E___E_cli_command_168_169(371) => __(10).
	p(NT(371), (NT(10)));
//G577: __E___E_cli_command_168_169(371) => _(9) '=' _(9).
	p(NT(371), (NT(9)+T(3)+NT(9)));
//G578: __E_cli_command_168(369) => set_sym(370) __(10) option_name(23) __E___E_cli_command_168_169(371) option_value(24).
	p(NT(369), (NT(370)+NT(10)+NT(23)+NT(371)+NT(24)));
//G579: set_cmd(368)         => __E_cli_command_168(369).
	p(NT(368), (NT(369)));
//G580: cli_command(274)     => set_cmd(368).
	p(NT(274), (NT(368)));
//G581: enable_sym(374)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(374), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G582: __E_cli_command_170(373) => enable_sym(374) __(10) option_name(23).
	p(NT(373), (NT(374)+NT(10)+NT(23)));
//G583: enable_cmd(372)      => __E_cli_command_170(373).
	p(NT(372), (NT(373)));
//G584: cli_command(274)     => enable_cmd(372).
	p(NT(274), (NT(372)));
//G585: disable_sym(377)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(377), (T(52)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G586: __E_cli_command_171(376) => disable_sym(377) __(10) option_name(23).
	p(NT(376), (NT(377)+NT(10)+NT(23)));
//G587: disable_cmd(375)     => __E_cli_command_171(376).
	p(NT(375), (NT(376)));
//G588: cli_command(274)     => disable_cmd(375).
	p(NT(274), (NT(375)));
//G589: toggle_sym(380)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(380), (T(8)+T(6)+T(64)+T(64)+T(10)+T(11)));
//G590: __E_cli_command_172(379) => toggle_sym(380) __(10) option_name(23).
	p(NT(379), (NT(380)+NT(10)+NT(23)));
//G591: toggle_cmd(378)      => __E_cli_command_172(379).
	p(NT(378), (NT(379)));
//G592: cli_command(274)     => toggle_cmd(378).
	p(NT(274), (NT(378)));
//G593: __E___E_cli_command_173_174(384) => 'd' 'e' 'f' 's'.
	p(NT(384), (T(52)+T(11)+T(9)+T(15)));
//G594: __E___E_cli_command_173_174(384) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(384), (T(52)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G595: def_sym(383)         => __E___E_cli_command_173_174(384).
	p(NT(383), (NT(384)));
//G596: __E_cli_command_173(382) => def_sym(383).
	p(NT(382), (NT(383)));
//G597: def_list_cmd(381)    => __E_cli_command_173(382).
	p(NT(381), (NT(382)));
//G598: cli_command(274)     => def_list_cmd(381).
	p(NT(274), (NT(381)));
//G599: __E_cli_command_175(386) => def_sym(383) __(10) num(165).
	p(NT(386), (NT(383)+NT(10)+NT(165)));
//G600: def_print_cmd(385)   => __E_cli_command_175(386).
	p(NT(385), (NT(386)));
//G601: cli_command(274)     => def_print_cmd(385).
	p(NT(274), (NT(385)));
//G602: def_rr_cmd(387)      => rec_relation(54).
	p(NT(387), (NT(54)));
//G603: cli_command(274)     => def_rr_cmd(387).
	p(NT(274), (NT(387)));
//G604: def_input_cmd(388)   => input_def(55).
	p(NT(388), (NT(55)));
//G605: cli_command(274)     => def_input_cmd(388).
	p(NT(274), (NT(388)));
//G606: def_output_cmd(389)  => output_def(56).
	p(NT(389), (NT(56)));
//G607: cli_command(274)     => def_output_cmd(389).
	p(NT(274), (NT(389)));
//G608: __E___E_cli_command_176_177(393) => 'h' 'i' 's' 't'.
	p(NT(393), (T(62)+T(4)+T(15)+T(8)));
//G609: __E___E_cli_command_176_177(393) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(393), (T(62)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G610: history_sym(392)     => __E___E_cli_command_176_177(393).
	p(NT(392), (NT(393)));
//G611: __E_cli_command_176(391) => history_sym(392).
	p(NT(391), (NT(392)));
//G612: history_list_cmd(390) => __E_cli_command_176(391).
	p(NT(390), (NT(391)));
//G613: cli_command(274)     => history_list_cmd(390).
	p(NT(274), (NT(390)));
//G614: __E_cli_command_178(395) => history_sym(392) __(10) history(396).
	p(NT(395), (NT(392)+NT(10)+NT(396)));
//G615: history_print_cmd(394) => __E_cli_command_178(395).
	p(NT(394), (NT(395)));
//G616: cli_command(274)     => history_print_cmd(394).
	p(NT(274), (NT(394)));
//G617: __E_cli_command_179(398) => ref(62).
	p(NT(398), (NT(62)));
//G618: __E_cli_command_179(398) => wff(36).
	p(NT(398), (NT(36)));
//G619: __E_cli_command_179(398) => bf(38).
	p(NT(398), (NT(38)));
//G620: history_store_cmd(397) => __E_cli_command_179(398).
	p(NT(397), (NT(398)));
//G621: cli_command(274)     => history_store_cmd(397).
	p(NT(274), (NT(397)));
//G622: __E_solve_options_180(399) => __(10) solve_option(400).
	p(NT(399), (NT(10)+NT(400)));
//G623: __E_solve_options_181(401) => null.
	p(NT(401), (nul));
//G624: __E_solve_options_181(401) => __E_solve_options_180(399) __E_solve_options_181(401).
	p(NT(401), (NT(399)+NT(401)));
//G625: solve_options(310)   => __E_solve_options_181(401).
	p(NT(310), (NT(401)));
//G626: __E_solve_option_182(402) => solver_mode(403).
	p(NT(402), (NT(403)));
//G627: __E_solve_option_182(402) => type(13).
	p(NT(402), (NT(13)));
//G628: solve_option(400)    => '-' '-' __E_solve_option_182(402).
	p(NT(400), (T(32)+T(32)+NT(402)));
//G629: __E_solver_mode_183(405) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(405), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G630: __E_solver_mode_183(405) => 'm' 'i' 'n'.
	p(NT(405), (T(21)+T(4)+T(5)));
//G631: solver_mode_minimum(404) => __E_solver_mode_183(405).
	p(NT(404), (NT(405)));
//G632: solver_mode(403)     => solver_mode_minimum(404).
	p(NT(403), (NT(404)));
//G633: __E_solver_mode_184(407) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(407), (T(21)+T(16)+T(34)+T(4)+T(21)+T(7)+T(21)));
//G634: __E_solver_mode_184(407) => 'm' 'a' 'x'.
	p(NT(407), (T(21)+T(16)+T(34)));
//G635: solver_mode_maximum(406) => __E_solver_mode_184(407).
	p(NT(406), (NT(407)));
//G636: solver_mode(403)     => solver_mode_maximum(406).
	p(NT(403), (NT(406)));
//G637: q_file_name(68)      => '"' file_name(21) '"'.
	p(NT(68), (T(66)+NT(21)+T(66)));
//G638: __E_file_name_185(408) => printable(8).
	p(NT(408), (NT(8)));
//G639: __E_file_name_185(408) => printable(8) __E_file_name_185(408).
	p(NT(408), (NT(8)+NT(408)));
//G640: file_name(21)        => __E_file_name_185(408).
	p(NT(21), (NT(408)));
//G641: __E_option_name_186(409) => alnum(6).
	p(NT(409), (NT(6)));
//G642: __E_option_name_186(409) => alnum(6) __E_option_name_186(409).
	p(NT(409), (NT(6)+NT(409)));
//G643: option_name(23)      => __E_option_name_186(409).
	p(NT(23), (NT(409)));
//G644: __E_option_value_187(410) => alnum(6).
	p(NT(410), (NT(6)));
//G645: __E_option_value_187(410) => '.'.
	p(NT(410), (T(1)));
//G646: __E_option_value_188(411) => __E_option_value_187(410).
	p(NT(411), (NT(410)));
//G647: __E_option_value_188(411) => __E_option_value_187(410) __E_option_value_188(411).
	p(NT(411), (NT(410)+NT(411)));
//G648: option_value(24)     => __E_option_value_188(411).
	p(NT(24), (NT(411)));
//G649: fragment_ltl(413)    => 'l' 't' 'l'.
	p(NT(413), (T(10)+T(8)+T(10)));
//G650: __E_fragment_name_189(412) => fragment_ltl(413).
	p(NT(412), (NT(413)));
//G651: fragment_ctl_star(414) => 'c' 't' 'l' '_' 's' 't' 'a' 'r'.
	p(NT(414), (T(14)+T(8)+T(10)+T(55)+T(15)+T(8)+T(16)+T(19)));
//G652: __E_fragment_name_189(412) => fragment_ctl_star(414).
	p(NT(412), (NT(414)));
//G653: fragment_ctl_star(414) => 'c' 't' 'l' '*'.
	p(NT(414), (T(14)+T(8)+T(10)+T(47)));
//G654: __E_fragment_name_189(412) => fragment_ctl_star(414).
	p(NT(412), (NT(414)));
//G655: fragment_name(363)   => __E_fragment_name_189(412).
	p(NT(363), (NT(412)));
//G656: bf_cmd_arg(31)       => history(396).
	p(NT(31), (NT(396)));
//G657: bf_cmd_arg(31)       => bf(38).
	p(NT(31), (NT(38)));
//G658: wff_cmd_arg(32)      => history(396).
	p(NT(32), (NT(396)));
//G659: wff_cmd_arg(32)      => wff(36).
	p(NT(32), (NT(36)));
//G660: nf_cmd_arg(33)       => history(396).
	p(NT(33), (NT(396)));
//G661: nf_cmd_arg(33)       => ref(62).
	p(NT(33), (NT(62)));
//G662: nf_cmd_arg(33)       => wff(36).
	p(NT(33), (NT(36)));
//G663: nf_cmd_arg(33)       => bf(38).
	p(NT(33), (NT(38)));
//G664: onf_cmd_arg(34)      => history(396).
	p(NT(34), (NT(396)));
//G665: onf_cmd_arg(34)      => wff(36).
	p(NT(34), (NT(36)));
//G666: spec_arg(35)         => history(396).
	p(NT(35), (NT(396)));
//G667: spec_arg(35)         => spec(47).
	p(NT(35), (NT(47)));
//G668: spec_arg(35)         => ref(62).
	p(NT(35), (NT(62)));
//G669: spec_arg(35)         => wff(36).
	p(NT(35), (NT(36)));
//G670: spec_arg(35)         => bf(38).
	p(NT(35), (NT(38)));
//G671: inst_args(30)        => wff_cmd_arg(32) _(9) '[' _(9) bf(38) _(9) '/' _(9) bf_cmd_arg(31) _(9) ']'.
	p(NT(30), (NT(32)+NT(9)+T(26)+NT(9)+NT(38)+NT(9)+T(48)+NT(9)+NT(31)+NT(9)+T(27)));
//G672: inst_args(30)        => bf_cmd_arg(31) _(9) '[' _(9) bf(38) _(9) '/' _(9) bf_cmd_arg(31) _(9) ']'.
	p(NT(30), (NT(31)+NT(9)+T(26)+NT(9)+NT(38)+NT(9)+T(48)+NT(9)+NT(31)+NT(9)+T(27)));
//G673: help_arg(292)        => help_sym(289).
	p(NT(292), (NT(289)));
//G674: help_arg(292)        => version_sym(282).
	p(NT(292), (NT(282)));
//G675: help_arg(292)        => quit_sym(279).
	p(NT(292), (NT(279)));
//G676: help_arg(292)        => clear_sym(285).
	p(NT(292), (NT(285)));
//G677: help_arg(292)        => get_sym(366).
	p(NT(292), (NT(366)));
//G678: help_arg(292)        => set_sym(370).
	p(NT(292), (NT(370)));
//G679: help_arg(292)        => enable_sym(374).
	p(NT(292), (NT(374)));
//G680: help_arg(292)        => disable_sym(377).
	p(NT(292), (NT(377)));
//G681: help_arg(292)        => toggle_sym(380).
	p(NT(292), (NT(380)));
//G682: help_arg(292)        => file_sym(295).
	p(NT(292), (NT(295)));
//G683: help_arg(292)        => history_sym(392).
	p(NT(292), (NT(392)));
//G684: help_arg(292)        => abs_history_sym(415).
	p(NT(292), (NT(415)));
//G685: help_arg(292)        => rel_history_sym(416).
	p(NT(292), (NT(416)));
//G686: help_arg(292)        => selection_sym(417).
	p(NT(292), (NT(417)));
//G687: help_arg(292)        => def_sym(383).
	p(NT(292), (NT(383)));
//G688: help_arg(292)        => inst_sym(334).
	p(NT(292), (NT(334)));
//G689: help_arg(292)        => subst_sym(330).
	p(NT(292), (NT(330)));
//G690: help_arg(292)        => normalize_sym(326).
	p(NT(292), (NT(326)));
//G691: help_arg(292)        => execute_sym(418).
	p(NT(292), (NT(418)));
//G692: help_arg(292)        => solve_sym(308).
	p(NT(292), (NT(308)));
//G693: help_arg(292)        => lgrs_sym(313).
	p(NT(292), (NT(313)));
//G694: help_arg(292)        => valid_sym(299).
	p(NT(292), (NT(299)));
//G695: help_arg(292)        => sat_sym(302).
	p(NT(292), (NT(302)));
//G696: help_arg(292)        => unsat_sym(305).
	p(NT(292), (NT(305)));
//G697: help_arg(292)        => run_sym(316).
	p(NT(292), (NT(316)));
//G698: help_arg(292)        => ltl_sym(320).
	p(NT(292), (NT(320)));
//G699: help_arg(292)        => dnf_sym(338).
	p(NT(292), (NT(338)));
//G700: help_arg(292)        => cnf_sym(341).
	p(NT(292), (NT(341)));
//G701: help_arg(292)        => snf_sym(419).
	p(NT(292), (NT(419)));
//G702: help_arg(292)        => nnf_sym(344).
	p(NT(292), (NT(344)));
//G703: help_arg(292)        => mnf_sym(347).
	p(NT(292), (NT(347)));
//G704: help_arg(292)        => onf_sym(350).
	p(NT(292), (NT(350)));
//G705: help_arg(292)        => qelim_sym(353).
	p(NT(292), (NT(353)));
//G706: help_arg(292)        => whatis_sym(356).
	p(NT(292), (NT(356)));
//G707: help_arg(292)        => reset_sym(359).
	p(NT(292), (NT(359)));
//G708: help_arg(292)        => fragment_sym(362).
	p(NT(292), (NT(362)));
//G709: __E___E_help_arg_190_191(422) => 's'.
	p(NT(422), (T(15)));
//G710: __E___E_help_arg_190_191(422) => null.
	p(NT(422), (nul));
//G711: __E_help_arg_190(421) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_190_191(422).
	p(NT(421), (T(11)+T(34)+T(16)+T(21)+T(63)+T(10)+T(11)+NT(422)));
//G712: examples_sym(420)    => __E_help_arg_190(421).
	p(NT(420), (NT(421)));
//G713: help_arg(292)        => examples_sym(420).
	p(NT(292), (NT(420)));
//G714: __E_history_192(424) => '%'.
	p(NT(424), (T(46)));
//G715: rel_history_sym(416) => '%' '-'.
	p(NT(416), (T(46)+T(32)));
//G716: history_id(426)      => digits(12).
	p(NT(426), (NT(12)));
//G717: __E___E_history_192_193(425) => history_id(426).
	p(NT(425), (NT(426)));
//G718: __E___E_history_192_193(425) => null.
	p(NT(425), (nul));
//G719: __E_history_192(424) => rel_history_sym(416) __E___E_history_192_193(425).
	p(NT(424), (NT(416)+NT(425)));
//G720: rel_history(423)     => __E_history_192(424).
	p(NT(423), (NT(424)));
//G721: history(396)         => rel_history(423).
	p(NT(396), (NT(423)));
//G722: abs_history_sym(415) => '%'.
	p(NT(415), (T(46)));
//G723: __E_history_194(428) => abs_history_sym(415) history_id(426).
	p(NT(428), (NT(415)+NT(426)));
//G724: abs_history(427)     => __E_history_194(428).
	p(NT(427), (NT(428)));
//G725: history(396)         => abs_history(427).
	p(NT(396), (NT(427)));
//G726: unreachable(429)     => BDD_ID(430).
	p(NT(429), (NT(430)));
//G727: stream_def(431)      => input_def(55).
	p(NT(431), (NT(55)));
//G728: stream_def(431)      => output_def(56).
	p(NT(431), (NT(56)));
//G729: fm_or_term(432)      => wff(36).
	p(NT(432), (NT(36)));
//G730: fm_or_term(432)      => bf(38).
	p(NT(432), (NT(38)));
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
		wff_F, __E_wff_23, wff_A, __E_wff_24, wff_E, __E_wff_25, wff_semantic_neg, __E_wff_26, wff_conditional, __E_wff_27, 
		wff_all, __E_wff_28, q_vars, wff_ex, __E_wff_29, wff_ref, constraint, wff_imply, __E_wff_30, wff_rimply, 
		__E_wff_31, wff_equiv, __E_wff_32, wff_or, __E_wff_33, wff_xor, __E_wff_34, wff_and, __E_wff_35, wff_neg, 
		__E_wff_36, wff_t, wff_f, bf_interval, __E_wff_37, bf_eq, __E_wff_38, bf_neq, __E_wff_39, bf_lt, 
		__E_wff_40, bf_nlt, __E_wff_41, bf_lteq, __E_wff_42, bf_nlteq, __E_wff_43, bf_gt, __E_wff_44, bf_ngt, 
		__E_wff_45, bf_gteq, __E_wff_46, bf_ngteq, __E_wff_47, wff_U, __E_wff_48, wff_R, __E_wff_49, wff_W, 
		__E_wff_50, wff_S, __E_wff_51, wff_T, __E_wff_52, __E_bf_53, ba_constant, variable, bf_fall, __E_bf_54, 
		bf_fex, __E_bf_55, bf_ref, bf_cast, __E_bf_56, num, __E___E_bf_56_57, bf_t, bf_f, bf_neg, 
		bf_func_app, __E_bf_58, func_sym, func_args, bf_shr, __E_bf_59, __E___E_bf_59_60, bf_shl, __E_bf_61, __E___E_bf_61_62, 
		bf_add, __E_bf_63, bf_sub, __E_bf_64, __E___E_bf_64_65, bf_mod, __E_bf_66, __E___E_bf_66_67, bf_mul, __E_bf_68, 
		bf_div, __E_bf_69, __E___E_bf_69_70, bf_nor, __E_bf_71, __E___E_bf_71_72, bf_xnor, __E_bf_73, __E___E_bf_73_74, bf_nand, 
		__E_bf_75, __E___E_bf_75_76, bf_or, __E_bf_77, bf_xor, __E_bf_78, __E_bf_79, __E___E_bf_79_80, __E_bf_81, __E___E_bf_81_82, 
		__E_bf_83, __E___E_bf_83_84, __E_bf_85, __E___E_bf_85_86, __E_bf_87, __E___E_bf_87_88, __E___E_bf_87_89, __E_func_args_90, __E_func_args_91, func_mode, 
		func_dynamic, func_static, func_sort_list, __E_func_sort_list_92, __E_func_sort_list_93, func_decl, ctn_neq, __E_constraint_94, ctn_eq, __E_constraint_95, 
		ctn_gteq, __E_constraint_96, ctn_gt, __E_constraint_97, ctn_lteq, __E_constraint_98, ctn_lt, __E_constraint_99, __E_ba_constant_100, __E_ba_constant_101, 
		__E_source_102, __E___E_source_102_103, __E___E_source_102_104, __E_src_c_105, __E_src_c_106, __E_variable_107, io_var, __E_variable_108, __E_var_name_109, __E_var_name_110, 
		__E_var_name_111, __E_var_name_112, __E_var_name_113, offset, __E_uconst_114, __E___E_uconst_114_115, __E_q_vars_116, __E_q_vars_117, __E_offsets_118, __E_offsets_119, 
		shift, __E_offset_120, __E_shift_121, __E___E_shift_121_122, __E_chars_123, __E_chars_124, __E_digits_125, __E_integer_126, __E_typed_127, comment, 
		__E_comment_128, __E_comment_129, __E_comment_130, cli, cli_command, __E_cli_131, __E_cli_132, __E_cli_133, quit_cmd, quit_sym, 
		__E_cli_command_134, version_cmd, version_sym, __E_cli_command_135, clear_cmd, clear_sym, __E_cli_command_136, help_cmd, __E_cli_command_137, help_sym, 
		__E___E_cli_command_137_138, __E___E_cli_command_137_139, help_arg, file_cmd, __E_cli_command_140, file_sym, q_string, valid_cmd, __E_cli_command_141, valid_sym, 
		sat_cmd, __E_cli_command_142, sat_sym, unsat_cmd, __E_cli_command_143, unsat_sym, solve_cmd, __E_cli_command_144, solve_sym, __E___E_cli_command_144_145, 
		solve_options, lgrs_cmd, __E_cli_command_146, lgrs_sym, run_cmd, __E_cli_command_147, run_sym, __E___E_cli_command_147_148, ltl_cmd, __E_cli_command_149, 
		ltl_sym, func_cmd, __E_cli_command_150, func_sym_cmd, normalize_cmd, __E_cli_command_151, normalize_sym, __E___E_cli_command_151_152, subst_cmd, __E_cli_command_153, 
		subst_sym, __E___E_cli_command_153_154, inst_cmd, __E_cli_command_155, inst_sym, __E___E_cli_command_155_156, dnf_cmd, __E_cli_command_157, dnf_sym, cnf_cmd, 
		__E_cli_command_158, cnf_sym, nnf_cmd, __E_cli_command_159, nnf_sym, mnf_cmd, __E_cli_command_160, mnf_sym, onf_cmd, __E_cli_command_161, 
		onf_sym, qelim_cmd, __E_cli_command_162, qelim_sym, whatis_cmd, __E_cli_command_163, whatis_sym, reset_cmd, __E_cli_command_164, reset_sym, 
		fragment_cmd, __E_cli_command_165, fragment_sym, fragment_name, get_cmd, __E_cli_command_166, get_sym, __E___E_cli_command_166_167, set_cmd, __E_cli_command_168, 
		set_sym, __E___E_cli_command_168_169, enable_cmd, __E_cli_command_170, enable_sym, disable_cmd, __E_cli_command_171, disable_sym, toggle_cmd, __E_cli_command_172, 
		toggle_sym, def_list_cmd, __E_cli_command_173, def_sym, __E___E_cli_command_173_174, def_print_cmd, __E_cli_command_175, def_rr_cmd, def_input_cmd, def_output_cmd, 
		history_list_cmd, __E_cli_command_176, history_sym, __E___E_cli_command_176_177, history_print_cmd, __E_cli_command_178, history, history_store_cmd, __E_cli_command_179, __E_solve_options_180, 
		solve_option, __E_solve_options_181, __E_solve_option_182, solver_mode, solver_mode_minimum, __E_solver_mode_183, solver_mode_maximum, __E_solver_mode_184, __E_file_name_185, __E_option_name_186, 
		__E_option_value_187, __E_option_value_188, __E_fragment_name_189, fragment_ltl, fragment_ctl_star, abs_history_sym, rel_history_sym, selection_sym, execute_sym, snf_sym, 
		examples_sym, __E_help_arg_190, __E___E_help_arg_190_191, rel_history, __E_history_192, __E___E_history_192_193, history_id, abs_history, __E_history_194, unreachable, 
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
