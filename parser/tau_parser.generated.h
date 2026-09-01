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
	"uconst_name", "file_name", "ctnvar", "option_name", "option_value", "type_name", "member_name", "chars", "q_var", "src_c", 
	"uconst", "spec_part", "inst_args", "bf_cmd_arg", "wff_cmd_arg", "nf_cmd_arg", "onf_cmd_arg", "spec_arg", "wff", "wff_parenthesis", 
	"bf", "bf_parenthesis", "bf_neg_oprnd", "bf_cast_oprnd", "bf_and_nosep_1st_oprnd", "bf_and_nosep_2nd_oprnd", "bf_and_nosep", "bf_and", "start", "spec", 
	"__E_spec_0", "definitions", "main", "__E_spec_1", "__E_definitions_2", "__E___E_definitions_2_3", "rec_relation", "input_def", "output_def", "type_def", 
	"__E_definitions_4", "spec_multiline", "__E_spec_multiline_5", "__E_spec_multiline_6", "__E_spec_multiline_7", "ref", "__E_rec_relation_8", "__E_input_def_9", "member_path", "__E_input_def_10", 
	"typed", "stream", "__E_output_def_11", "__E_output_def_12", "q_file_name", "console_sym", "__E_type_def_13", "__E___E_type_def_13_14", "type_parents", "__E_type_def_15", 
	"type_body", "__E_type_parents_16", "__E_type_parents_17", "tuple", "__E_type_body_18", "member", "__E_tuple_19", "__E_tuple_20", "__E_tuple_21", "__E_member_path_22", 
	"__E_member_path_23", "__E_ref_24", "offsets", "ref_args", "__E_ref_25", "__E_ref_26", "fp_fallback", "first_sym", "last_sym", "__E_ref_args_27", 
	"__E___E_ref_args_27_28", "ref_arg", "__E___E_ref_args_27_29", "__E___E_ref_args_27_30", "__E_wff_31", "wff_sometimes", "__E_wff_32", "__E___E_wff_32_33", "wff_always", "__E_wff_34", 
	"__E___E_wff_34_35", "wff_conditional", "__E_wff_36", "wff_all", "__E_wff_37", "q_vars", "wff_ex", "__E_wff_38", "wff_ref", "constraint", 
	"wff_imply", "__E_wff_39", "wff_rimply", "__E_wff_40", "wff_equiv", "__E_wff_41", "wff_or", "__E_wff_42", "wff_xor", "__E_wff_43", 
	"wff_and", "__E_wff_44", "wff_neg", "__E_wff_45", "wff_t", "wff_f", "bf_interval", "__E_wff_46", "bf_eq", "__E_wff_47", 
	"bf_neq", "__E_wff_48", "bf_lt", "__E_wff_49", "bf_nlt", "__E_wff_50", "bf_lteq", "__E_wff_51", "bf_nlteq", "__E_wff_52", 
	"bf_gt", "__E_wff_53", "bf_ngt", "__E_wff_54", "bf_gteq", "__E_wff_55", "bf_ngteq", "__E_wff_56", "__E_bf_57", "__E___E_bf_57_58", 
	"bv_type_like", "ba_constant", "variable", "bf_fall", "__E_bf_59", "bf_fex", "__E_bf_60", "bf_ref", "bf_cast", "__E_bf_61", 
	"num", "__E___E_bf_61_62", "bf_t", "bf_f", "bf_neg", "bf_shr", "__E_bf_63", "__E___E_bf_63_64", "bf_shl", "__E_bf_65", 
	"__E___E_bf_65_66", "bf_add", "__E_bf_67", "__E___E_bf_67_68", "bf_sub", "__E_bf_69", "__E___E_bf_69_70", "bf_mod", "__E_bf_71", "__E___E_bf_71_72", 
	"bf_mul", "bf_div", "__E_bf_73", "__E___E_bf_73_74", "__E_bf_75", "__E___E_bf_75_76", "bf_nor", "__E_bf_77", "__E___E_bf_77_78", "bf_xnor", 
	"__E_bf_79", "__E___E_bf_79_80", "bf_nand", "__E_bf_81", "__E___E_bf_81_82", "bf_or", "__E_bf_83", "bf_xor", "__E_bf_84", "__E_bf_85", 
	"__E___E_bf_85_86", "__E_bf_87", "__E___E_bf_87_88", "__E_bf_89", "__E___E_bf_89_90", "__E_bf_91", "__E___E_bf_91_92", "__E_bf_93", "__E___E_bf_93_94", "__E___E_bf_93_95", 
	"ctn_neq", "__E_constraint_96", "ctn_eq", "__E_constraint_97", "ctn_gteq", "__E_constraint_98", "ctn_gt", "__E_constraint_99", "ctn_lteq", "__E_constraint_100", 
	"ctn_lt", "__E_constraint_101", "__E_ba_constant_102", "__E_ba_constant_103", "__E_source_104", "__E___E_source_104_105", "__E___E_source_104_106", "__E_src_c_107", "__E_src_c_108", "__E_variable_109", 
	"io_var", "__E_variable_110", "__E_variable_111", "__E_var_name_112", "__E_var_name_113", "__E_var_name_114", "__E_var_name_115", "__E_var_name_116", "offset", "__E_uconst_117", 
	"__E___E_uconst_117_118", "__E_q_vars_119", "__E_q_vars_120", "__E_offsets_121", "__E_offsets_122", "shift", "__E_offset_123", "__E_shift_124", "__E___E_shift_124_125", "__E_chars_126", 
	"__E_chars_127", "__E_digits_128", "__E_integer_129", "__E_typed_130", "comment", "__E_comment_131", "__E_comment_132", "__E_comment_133", "cli", "cli_command", 
	"__E_cli_134", "__E_cli_135", "__E_cli_136", "quit_cmd", "quit_sym", "__E_cli_command_137", "version_cmd", "version_sym", "__E_cli_command_138", "clear_cmd", 
	"clear_sym", "__E_cli_command_139", "help_cmd", "__E_cli_command_140", "help_sym", "__E___E_cli_command_140_141", "__E___E_cli_command_140_142", "help_arg", "file_cmd", "__E_cli_command_143", 
	"file_sym", "q_string", "valid_cmd", "__E_cli_command_144", "valid_sym", "sat_cmd", "__E_cli_command_145", "sat_sym", "unsat_cmd", "__E_cli_command_146", 
	"unsat_sym", "solve_cmd", "__E_cli_command_147", "solve_sym", "__E___E_cli_command_147_148", "solve_options", "lgrs_cmd", "__E_cli_command_149", "lgrs_sym", "run_cmd", 
	"__E_cli_command_150", "run_sym", "__E___E_cli_command_150_151", "normalize_cmd", "__E_cli_command_152", "normalize_sym", "__E___E_cli_command_152_153", "subst_cmd", "__E_cli_command_154", "subst_sym", 
	"__E___E_cli_command_154_155", "inst_cmd", "__E_cli_command_156", "inst_sym", "__E___E_cli_command_156_157", "dnf_cmd", "__E_cli_command_158", "dnf_sym", "cnf_cmd", "__E_cli_command_159", 
	"cnf_sym", "nnf_cmd", "__E_cli_command_160", "nnf_sym", "mnf_cmd", "__E_cli_command_161", "mnf_sym", "onf_cmd", "__E_cli_command_162", "onf_sym", 
	"qelim_cmd", "__E_cli_command_163", "qelim_sym", "get_cmd", "__E_cli_command_164", "get_sym", "__E___E_cli_command_164_165", "set_cmd", "__E_cli_command_166", "set_sym", 
	"__E___E_cli_command_166_167", "enable_cmd", "__E_cli_command_168", "enable_sym", "disable_cmd", "__E_cli_command_169", "disable_sym", "toggle_cmd", "__E_cli_command_170", "toggle_sym", 
	"def_list_cmd", "__E_cli_command_171", "def_sym", "__E___E_cli_command_171_172", "def_print_cmd", "__E_cli_command_173", "def_rr_cmd", "def_input_cmd", "def_output_cmd", "def_type_cmd", 
	"history_list_cmd", "__E_cli_command_174", "history_sym", "__E___E_cli_command_174_175", "history_print_cmd", "__E_cli_command_176", "history", "history_store_cmd", "__E_cli_command_177", "__E_solve_options_178", 
	"solve_option", "__E_solve_options_179", "__E_solve_option_180", "solver_mode", "solver_mode_minimum", "__E_solver_mode_181", "solver_mode_maximum", "__E_solver_mode_182", "__E_file_name_183", "__E_option_name_184", 
	"__E_option_value_185", "__E_option_value_186", "abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", "snf_sym", "examples_sym", "__E_help_arg_187", "__E___E_help_arg_187_188", 
	"rel_history", "__E_history_189", "__E___E_history_189_190", "history_id", "abs_history", "__E_history_191", "unreachable", "BDD_ID", "stream_def", "fm_or_term", 
	"__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", "__N_9", 
	"__N_10", "__N_11", "__N_12", "__N_13", "__N_14", "__N_15", "__N_16", "__N_17", "__N_18", "__N_19", 
	"__N_20", "__N_21", "__N_22", "__N_23", "__N_24", "__N_25", "__N_26", "__N_27", "__N_28", "__N_29", 
	"__N_30", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', '/', 'y', 'p', 
	',', '[', ']', '{', '}', 'a', 'b', 'k', 'r', 'm', 
	'<', '>', 'w', '?', 'x', '-', '|', '^', '&', '!', 
	'T', 'F', 'v', '+', '%', '*', '1', '0', '\'', '_', 
	'$', '\t', '\n', '\r', '#', 'q', 'h', 'd', 'g', 'z', 
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
			21, 22, 23, 24, 25, 26
		},
		.to_inline = {
			{ 27 },
			{ 28 },
			{ 29 },
			{ 30 },
			{ 30, 20 },
			{ 31 },
			{ 32 },
			{ 33 },
			{ 34 },
			{ 35 },
			{ 36 },
			{ 37 },
			{ 38, 39, 38 },
			{ 40, 41, 40 },
			{ 42, 41, 40 },
			{ 43, 41, 40 },
			{ 44, 41, 40 },
			{ 45, 41, 40 },
			{ 46, 41, 47 }
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

inline ::idni::prods<char_type, terminal_type> start_symbol{ nts(48) };

#ifdef TAU_PARSER_BUILD_HEADER_ONLY
inline idni::prods<char_type, terminal_type>& productions() {
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
//G117: wff_ref(118)         => ref(65).
	p(NT(118), (NT(65)));
//G118: wff(38)              => wff_ref(118).
	p(NT(38), (NT(118)));
//G119: wff(38)              => constraint(119).
	p(NT(38), (NT(119)));
//G120: __E_wff_39(121)      => wff(38) _(9) '-' '>' _(9) wff(38).
	p(NT(121), (NT(38)+NT(9)+T(34)+T(30)+NT(9)+NT(38)));
//G121: wff_imply(120)       => __E_wff_39(121).
	p(NT(120), (NT(121)));
//G122: wff(38)              => wff_imply(120).
	p(NT(38), (NT(120)));
//G123: __E_wff_40(123)      => wff(38) _(9) '<' '-' _(9) wff(38).
	p(NT(123), (NT(38)+NT(9)+T(29)+T(34)+NT(9)+NT(38)));
//G124: wff_rimply(122)      => __E_wff_40(123).
	p(NT(122), (NT(123)));
//G125: wff(38)              => wff_rimply(122).
	p(NT(38), (NT(122)));
//G126: __E_wff_41(125)      => wff(38) _(9) '<' '-' '>' _(9) wff(38).
	p(NT(125), (NT(38)+NT(9)+T(29)+T(34)+T(30)+NT(9)+NT(38)));
//G127: wff_equiv(124)       => __E_wff_41(125).
	p(NT(124), (NT(125)));
//G128: wff(38)              => wff_equiv(124).
	p(NT(38), (NT(124)));
//G129: __E_wff_42(127)      => wff(38) _(9) '|' '|' _(9) wff(38).
	p(NT(127), (NT(38)+NT(9)+T(35)+T(35)+NT(9)+NT(38)));
//G130: wff_or(126)          => __E_wff_42(127).
	p(NT(126), (NT(127)));
//G131: wff(38)              => wff_or(126).
	p(NT(38), (NT(126)));
//G132: __E_wff_43(129)      => wff(38) _(9) '^' '^' _(9) wff(38).
	p(NT(129), (NT(38)+NT(9)+T(36)+T(36)+NT(9)+NT(38)));
//G133: wff_xor(128)         => __E_wff_43(129).
	p(NT(128), (NT(129)));
//G134: wff(38)              => wff_xor(128).
	p(NT(38), (NT(128)));
//G135: __E_wff_44(131)      => wff(38) _(9) '&' '&' _(9) wff(38).
	p(NT(131), (NT(38)+NT(9)+T(37)+T(37)+NT(9)+NT(38)));
//G136: wff_and(130)         => __E_wff_44(131).
	p(NT(130), (NT(131)));
//G137: wff(38)              => wff_and(130).
	p(NT(38), (NT(130)));
//G138: __E_wff_45(133)      => '!' _(9) wff(38).
	p(NT(133), (T(38)+NT(9)+NT(38)));
//G139: wff_neg(132)         => __E_wff_45(133).
	p(NT(132), (NT(133)));
//G140: wff(38)              => wff_neg(132).
	p(NT(38), (NT(132)));
//G141: wff_t(134)           => 'T'.
	p(NT(134), (T(39)));
//G142: wff(38)              => wff_t(134).
	p(NT(38), (NT(134)));
//G143: wff_f(135)           => 'F'.
	p(NT(135), (T(40)));
//G144: wff(38)              => wff_f(135).
	p(NT(38), (NT(135)));
//G145: __E_wff_46(137)      => bf(40) _(9) '<' '=' _(9) bf(40) _(9) '<' '=' _(9) bf(40).
	p(NT(137), (NT(40)+NT(9)+T(29)+T(3)+NT(9)+NT(40)+NT(9)+T(29)+T(3)+NT(9)+NT(40)));
//G146: bf_interval(136)     => __E_wff_46(137).
	p(NT(136), (NT(137)));
//G147: wff(38)              => bf_interval(136).
	p(NT(38), (NT(136)));
//G148: __E_wff_47(139)      => bf(40) _(9) '=' _(9) bf(40).
	p(NT(139), (NT(40)+NT(9)+T(3)+NT(9)+NT(40)));
//G149: bf_eq(138)           => __E_wff_47(139).
	p(NT(138), (NT(139)));
//G150: wff(38)              => bf_eq(138).
	p(NT(38), (NT(138)));
//G151: __E_wff_48(141)      => bf(40) _(9) '!' '=' _(9) bf(40).
	p(NT(141), (NT(40)+NT(9)+T(38)+T(3)+NT(9)+NT(40)));
//G152: bf_neq(140)          => __E_wff_48(141).
	p(NT(140), (NT(141)));
//G153: wff(38)              => bf_neq(140).
	p(NT(38), (NT(140)));
//G154: __E_wff_49(143)      => bf(40) _(9) '<' _(9) bf(40).
	p(NT(143), (NT(40)+NT(9)+T(29)+NT(9)+NT(40)));
//G155: bf_lt(142)           => __E_wff_49(143).
	p(NT(142), (NT(143)));
//G156: wff(38)              => bf_lt(142).
	p(NT(38), (NT(142)));
//G157: __E_wff_50(145)      => bf(40) _(9) '!' '<' _(9) bf(40).
	p(NT(145), (NT(40)+NT(9)+T(38)+T(29)+NT(9)+NT(40)));
//G158: bf_nlt(144)          => __E_wff_50(145).
	p(NT(144), (NT(145)));
//G159: wff(38)              => bf_nlt(144).
	p(NT(38), (NT(144)));
//G160: __E_wff_51(147)      => bf(40) _(9) '<' '=' _(9) bf(40).
	p(NT(147), (NT(40)+NT(9)+T(29)+T(3)+NT(9)+NT(40)));
//G161: bf_lteq(146)         => __E_wff_51(147).
	p(NT(146), (NT(147)));
//G162: wff(38)              => bf_lteq(146).
	p(NT(38), (NT(146)));
//G163: __E_wff_52(149)      => bf(40) _(9) '!' '<' '=' _(9) bf(40).
	p(NT(149), (NT(40)+NT(9)+T(38)+T(29)+T(3)+NT(9)+NT(40)));
//G164: bf_nlteq(148)        => __E_wff_52(149).
	p(NT(148), (NT(149)));
//G165: wff(38)              => bf_nlteq(148).
	p(NT(38), (NT(148)));
//G166: __E_wff_53(151)      => bf(40) _(9) '>' _(9) bf(40).
	p(NT(151), (NT(40)+NT(9)+T(30)+NT(9)+NT(40)));
//G167: bf_gt(150)           => __E_wff_53(151).
	p(NT(150), (NT(151)));
//G168: wff(38)              => bf_gt(150).
	p(NT(38), (NT(150)));
//G169: __E_wff_54(153)      => bf(40) _(9) '!' '>' _(9) bf(40).
	p(NT(153), (NT(40)+NT(9)+T(38)+T(30)+NT(9)+NT(40)));
//G170: bf_ngt(152)          => __E_wff_54(153).
	p(NT(152), (NT(153)));
//G171: wff(38)              => bf_ngt(152).
	p(NT(38), (NT(152)));
//G172: __E_wff_55(155)      => bf(40) _(9) '>' '=' _(9) bf(40).
	p(NT(155), (NT(40)+NT(9)+T(30)+T(3)+NT(9)+NT(40)));
//G173: bf_gteq(154)         => __E_wff_55(155).
	p(NT(154), (NT(155)));
//G174: wff(38)              => bf_gteq(154).
	p(NT(38), (NT(154)));
//G175: __E_wff_56(157)      => bf(40) _(9) '!' '>' '=' _(9) bf(40).
	p(NT(157), (NT(40)+NT(9)+T(38)+T(30)+T(3)+NT(9)+NT(40)));
//G176: bf_ngteq(156)        => __E_wff_56(157).
	p(NT(156), (NT(157)));
//G177: wff(38)              => bf_ngteq(156).
	p(NT(38), (NT(156)));
//G178: wff(38)              => capture(17).
	p(NT(38), (NT(17)));
//G179: __N_0(410)           => bv_type_like(160).
	p(NT(410), (NT(160)));
//G180: __E___E_bf_57_58(159) => bf(40) & ~( __N_0(410) ).	 # conjunctive
	p(NT(159), (NT(40)) & ~(NT(410)));
//G181: __E_bf_57(158)       => '(' _(9) __E___E_bf_57_58(159) _(9) ')'.
	p(NT(158), (T(12)+NT(9)+NT(159)+NT(9)+T(13)));
//G182: bf_parenthesis(41)   => __E_bf_57(158).
	p(NT(41), (NT(158)));
//G183: bf(40)               => bf_parenthesis(41).
	p(NT(40), (NT(41)));
//G184: bf(40)               => ba_constant(161).
	p(NT(40), (NT(161)));
//G185: bf(40)               => variable(162).
	p(NT(40), (NT(162)));
//G186: __E_bf_59(164)       => 'f' 'a' 'l' 'l' __(10) q_vars(115) __(10) bf(40).
	p(NT(164), (T(9)+T(24)+T(10)+T(10)+NT(10)+NT(115)+NT(10)+NT(40)));
//G187: bf_fall(163)         => __E_bf_59(164).
	p(NT(163), (NT(164)));
//G188: bf(40)               => bf_fall(163).
	p(NT(40), (NT(163)));
//G189: __E_bf_60(166)       => 'f' 'e' 'x' __(10) q_vars(115) __(10) bf(40).
	p(NT(166), (T(9)+T(11)+T(33)+NT(10)+NT(115)+NT(10)+NT(40)));
//G190: bf_fex(165)          => __E_bf_60(166).
	p(NT(165), (NT(166)));
//G191: bf(40)               => bf_fex(165).
	p(NT(40), (NT(165)));
//G192: bf_ref(167)          => ref(65).
	p(NT(167), (NT(65)));
//G193: bf(40)               => bf_ref(167).
	p(NT(40), (NT(167)));
//G194: __E___E_bf_61_62(171) => bf_parenthesis(41).
	p(NT(171), (NT(41)));
//G195: __E___E_bf_61_62(171) => ba_constant(161).
	p(NT(171), (NT(161)));
//G196: __E___E_bf_61_62(171) => variable(162).
	p(NT(171), (NT(162)));
//G197: __E___E_bf_61_62(171) => bf_ref(167).
	p(NT(171), (NT(167)));
//G198: __E___E_bf_61_62(171) => bf_t(172).
	p(NT(171), (NT(172)));
//G199: __E___E_bf_61_62(171) => bf_f(173).
	p(NT(171), (NT(173)));
//G200: __E___E_bf_61_62(171) => bf_neg(174).
	p(NT(171), (NT(174)));
//G201: __E___E_bf_61_62(171) => bf_cast(168).
	p(NT(171), (NT(168)));
//G202: __E___E_bf_61_62(171) => capture(17).
	p(NT(171), (NT(17)));
//G203: __E___E_bf_61_62(171) => bf_fall(163).
	p(NT(171), (NT(163)));
//G204: __E___E_bf_61_62(171) => bf_fex(165).
	p(NT(171), (NT(165)));
//G205: bf_cast_oprnd(43)    => __E___E_bf_61_62(171).
	p(NT(43), (NT(171)));
//G206: __E_bf_61(169)       => '(' _(9) 'b' 'v' _(9) '[' _(9) num(170) _(9) ']' _(9) ')' _(9) bf_cast_oprnd(43).
	p(NT(169), (T(12)+NT(9)+T(25)+T(41)+NT(9)+T(20)+NT(9)+NT(170)+NT(9)+T(21)+NT(9)+T(13)+NT(9)+NT(43)));
//G207: bf_cast(168)         => __E_bf_61(169).
	p(NT(168), (NT(169)));
//G208: bf(40)               => bf_cast(168).
	p(NT(40), (NT(168)));
//G209: __N_1(411)           => bf_shr(175).
	p(NT(411), (NT(175)));
//G210: __N_2(412)           => bf_shl(178).
	p(NT(412), (NT(178)));
//G211: __E___E_bf_63_64(177) => bf(40) & ~( __N_1(411) ) & ~( __N_2(412) ).	 # conjunctive
	p(NT(177), (NT(40)) & ~(NT(411)) & ~(NT(412)));
//G212: __E_bf_63(176)       => bf(40) _(9) '>' '>' _(9) __E___E_bf_63_64(177).
	p(NT(176), (NT(40)+NT(9)+T(30)+T(30)+NT(9)+NT(177)));
//G213: bf_shr(175)          => __E_bf_63(176).
	p(NT(175), (NT(176)));
//G214: bf(40)               => bf_shr(175).
	p(NT(40), (NT(175)));
//G215: __N_3(413)           => bf_shr(175).
	p(NT(413), (NT(175)));
//G216: __N_4(414)           => bf_shl(178).
	p(NT(414), (NT(178)));
//G217: __E___E_bf_65_66(180) => bf(40) & ~( __N_3(413) ) & ~( __N_4(414) ).	 # conjunctive
	p(NT(180), (NT(40)) & ~(NT(413)) & ~(NT(414)));
//G218: __E_bf_65(179)       => bf(40) _(9) '<' '<' _(9) __E___E_bf_65_66(180).
	p(NT(179), (NT(40)+NT(9)+T(29)+T(29)+NT(9)+NT(180)));
//G219: bf_shl(178)          => __E_bf_65(179).
	p(NT(178), (NT(179)));
//G220: bf(40)               => bf_shl(178).
	p(NT(40), (NT(178)));
//G221: __N_5(415)           => bf_add(181).
	p(NT(415), (NT(181)));
//G222: __N_6(416)           => bf_sub(184).
	p(NT(416), (NT(184)));
//G223: __E___E_bf_67_68(183) => bf(40) & ~( __N_5(415) ) & ~( __N_6(416) ).	 # conjunctive
	p(NT(183), (NT(40)) & ~(NT(415)) & ~(NT(416)));
//G224: __E_bf_67(182)       => bf(40) _(9) '+' _(9) __E___E_bf_67_68(183).
	p(NT(182), (NT(40)+NT(9)+T(42)+NT(9)+NT(183)));
//G225: bf_add(181)          => __E_bf_67(182).
	p(NT(181), (NT(182)));
//G226: bf(40)               => bf_add(181).
	p(NT(40), (NT(181)));
//G227: __N_7(417)           => bf_add(181).
	p(NT(417), (NT(181)));
//G228: __N_8(418)           => bf_sub(184).
	p(NT(418), (NT(184)));
//G229: __E___E_bf_69_70(186) => bf(40) & ~( __N_7(417) ) & ~( __N_8(418) ).	 # conjunctive
	p(NT(186), (NT(40)) & ~(NT(417)) & ~(NT(418)));
//G230: __E_bf_69(185)       => bf(40) _(9) '-' _(9) __E___E_bf_69_70(186).
	p(NT(185), (NT(40)+NT(9)+T(34)+NT(9)+NT(186)));
//G231: bf_sub(184)          => __E_bf_69(185).
	p(NT(184), (NT(185)));
//G232: bf(40)               => bf_sub(184).
	p(NT(40), (NT(184)));
//G233: __N_9(419)           => bf_mod(187).
	p(NT(419), (NT(187)));
//G234: __N_10(420)          => bf_mul(190).
	p(NT(420), (NT(190)));
//G235: __N_11(421)          => bf_div(191).
	p(NT(421), (NT(191)));
//G236: __E___E_bf_71_72(189) => bf(40) & ~( __N_9(419) ) & ~( __N_10(420) ) & ~( __N_11(421) ).	 # conjunctive
	p(NT(189), (NT(40)) & ~(NT(419)) & ~(NT(420)) & ~(NT(421)));
//G237: __E_bf_71(188)       => bf(40) _(9) '%' _(9) __E___E_bf_71_72(189).
	p(NT(188), (NT(40)+NT(9)+T(43)+NT(9)+NT(189)));
//G238: bf_mod(187)          => __E_bf_71(188).
	p(NT(187), (NT(188)));
//G239: bf(40)               => bf_mod(187).
	p(NT(40), (NT(187)));
//G240: __N_12(422)          => bf_mod(187).
	p(NT(422), (NT(187)));
//G241: __N_13(423)          => bf_mul(190).
	p(NT(423), (NT(190)));
//G242: __N_14(424)          => bf_div(191).
	p(NT(424), (NT(191)));
//G243: __E___E_bf_73_74(193) => bf(40) & ~( __N_12(422) ) & ~( __N_13(423) ) & ~( __N_14(424) ).	 # conjunctive
	p(NT(193), (NT(40)) & ~(NT(422)) & ~(NT(423)) & ~(NT(424)));
//G244: __E_bf_73(192)       => bf(40) _(9) '*' _(9) __E___E_bf_73_74(193).
	p(NT(192), (NT(40)+NT(9)+T(44)+NT(9)+NT(193)));
//G245: bf_mul(190)          => __E_bf_73(192).
	p(NT(190), (NT(192)));
//G246: bf(40)               => bf_mul(190).
	p(NT(40), (NT(190)));
//G247: __N_15(425)          => bf_mod(187).
	p(NT(425), (NT(187)));
//G248: __N_16(426)          => bf_mul(190).
	p(NT(426), (NT(190)));
//G249: __N_17(427)          => bf_div(191).
	p(NT(427), (NT(191)));
//G250: __E___E_bf_75_76(195) => bf(40) & ~( __N_15(425) ) & ~( __N_16(426) ) & ~( __N_17(427) ).	 # conjunctive
	p(NT(195), (NT(40)) & ~(NT(425)) & ~(NT(426)) & ~(NT(427)));
//G251: __E_bf_75(194)       => bf(40) _(9) '/' _(9) __E___E_bf_75_76(195).
	p(NT(194), (NT(40)+NT(9)+T(16)+NT(9)+NT(195)));
//G252: bf_div(191)          => __E_bf_75(194).
	p(NT(191), (NT(194)));
//G253: bf(40)               => bf_div(191).
	p(NT(40), (NT(191)));
//G254: __N_18(428)          => bf_nor(196).
	p(NT(428), (NT(196)));
//G255: __E___E_bf_77_78(198) => bf(40) & ~( __N_18(428) ).	 # conjunctive
	p(NT(198), (NT(40)) & ~(NT(428)));
//G256: __E_bf_77(197)       => bf(40) _(9) '!' '|' _(9) __E___E_bf_77_78(198).
	p(NT(197), (NT(40)+NT(9)+T(38)+T(35)+NT(9)+NT(198)));
//G257: bf_nor(196)          => __E_bf_77(197).
	p(NT(196), (NT(197)));
//G258: bf(40)               => bf_nor(196).
	p(NT(40), (NT(196)));
//G259: __N_19(429)          => bf_xnor(199).
	p(NT(429), (NT(199)));
//G260: __E___E_bf_79_80(201) => bf(40) & ~( __N_19(429) ).	 # conjunctive
	p(NT(201), (NT(40)) & ~(NT(429)));
//G261: __E_bf_79(200)       => bf(40) _(9) '!' '^' _(9) __E___E_bf_79_80(201).
	p(NT(200), (NT(40)+NT(9)+T(38)+T(36)+NT(9)+NT(201)));
//G262: bf_xnor(199)         => __E_bf_79(200).
	p(NT(199), (NT(200)));
//G263: bf(40)               => bf_xnor(199).
	p(NT(40), (NT(199)));
//G264: __N_20(430)          => bf_nand(202).
	p(NT(430), (NT(202)));
//G265: __E___E_bf_81_82(204) => bf(40) & ~( __N_20(430) ).	 # conjunctive
	p(NT(204), (NT(40)) & ~(NT(430)));
//G266: __E_bf_81(203)       => bf(40) _(9) '!' '&' _(9) __E___E_bf_81_82(204).
	p(NT(203), (NT(40)+NT(9)+T(38)+T(37)+NT(9)+NT(204)));
//G267: bf_nand(202)         => __E_bf_81(203).
	p(NT(202), (NT(203)));
//G268: bf(40)               => bf_nand(202).
	p(NT(40), (NT(202)));
//G269: __E_bf_83(206)       => bf(40) _(9) '|' _(9) bf(40).
	p(NT(206), (NT(40)+NT(9)+T(35)+NT(9)+NT(40)));
//G270: bf_or(205)           => __E_bf_83(206).
	p(NT(205), (NT(206)));
//G271: bf(40)               => bf_or(205).
	p(NT(40), (NT(205)));
//G272: __E_bf_84(208)       => bf(40) _(9) '^' _(9) bf(40).
	p(NT(208), (NT(40)+NT(9)+T(36)+NT(9)+NT(40)));
//G273: bf_xor(207)          => __E_bf_84(208).
	p(NT(207), (NT(208)));
//G274: bf(40)               => bf_xor(207).
	p(NT(40), (NT(207)));
//G275: __E___E_bf_85_86(210) => typed(70).
	p(NT(210), (NT(70)));
//G276: __E___E_bf_85_86(210) => null.
	p(NT(210), (nul));
//G277: __E_bf_85(209)       => '1' __E___E_bf_85_86(210).
	p(NT(209), (T(45)+NT(210)));
//G278: bf_t(172)            => __E_bf_85(209).
	p(NT(172), (NT(209)));
//G279: bf(40)               => bf_t(172).
	p(NT(40), (NT(172)));
//G280: __E___E_bf_87_88(212) => typed(70).
	p(NT(212), (NT(70)));
//G281: __E___E_bf_87_88(212) => null.
	p(NT(212), (nul));
//G282: __E_bf_87(211)       => '0' __E___E_bf_87_88(212).
	p(NT(211), (T(46)+NT(212)));
//G283: bf_f(173)            => __E_bf_87(211).
	p(NT(173), (NT(211)));
//G284: bf(40)               => bf_f(173).
	p(NT(40), (NT(173)));
//G285: __E___E_bf_89_90(214) => _(9) '&' _(9).
	p(NT(214), (NT(9)+T(37)+NT(9)));
//G286: __E___E_bf_89_90(214) => __(10).
	p(NT(214), (NT(10)));
//G287: __E_bf_89(213)       => bf(40) __E___E_bf_89_90(214) bf(40).
	p(NT(213), (NT(40)+NT(214)+NT(40)));
//G288: bf_and(47)           => __E_bf_89(213).
	p(NT(47), (NT(213)));
//G289: bf(40)               => bf_and(47).
	p(NT(40), (NT(47)));
//G290: __E___E_bf_91_92(216) => bf_parenthesis(41).
	p(NT(216), (NT(41)));
//G291: __E___E_bf_91_92(216) => ba_constant(161).
	p(NT(216), (NT(161)));
//G292: __E___E_bf_91_92(216) => variable(162).
	p(NT(216), (NT(162)));
//G293: __E___E_bf_91_92(216) => bf_ref(167).
	p(NT(216), (NT(167)));
//G294: __E___E_bf_91_92(216) => bf_t(172).
	p(NT(216), (NT(172)));
//G295: __E___E_bf_91_92(216) => bf_f(173).
	p(NT(216), (NT(173)));
//G296: __E___E_bf_91_92(216) => bf_neg(174).
	p(NT(216), (NT(174)));
//G297: __E___E_bf_91_92(216) => capture(17).
	p(NT(216), (NT(17)));
//G298: bf_neg_oprnd(42)     => __E___E_bf_91_92(216).
	p(NT(42), (NT(216)));
//G299: __E_bf_91(215)       => bf_neg_oprnd(42) _(9) '\''.
	p(NT(215), (NT(42)+NT(9)+T(47)));
//G300: bf_neg(174)          => __E_bf_91(215).
	p(NT(174), (NT(215)));
//G301: bf(40)               => bf_neg(174).
	p(NT(40), (NT(174)));
//G302: __E___E_bf_93_94(218) => bf_parenthesis(41).
	p(NT(218), (NT(41)));
//G303: __E___E_bf_93_94(218) => variable(162).
	p(NT(218), (NT(162)));
//G304: __E___E_bf_93_94(218) => bf_ref(167).
	p(NT(218), (NT(167)));
//G305: __E___E_bf_93_94(218) => bf_neg(174).
	p(NT(218), (NT(174)));
//G306: __E___E_bf_93_94(218) => bf_and_nosep(46).
	p(NT(218), (NT(46)));
//G307: __E___E_bf_93_94(218) => capture(17).
	p(NT(218), (NT(17)));
//G308: bf_and_nosep_1st_oprnd(44) => __E___E_bf_93_94(218).
	p(NT(44), (NT(218)));
//G309: __E___E_bf_93_95(219) => bf_parenthesis(41).
	p(NT(219), (NT(41)));
//G310: __E___E_bf_93_95(219) => ba_constant(161).
	p(NT(219), (NT(161)));
//G311: __E___E_bf_93_95(219) => variable(162).
	p(NT(219), (NT(162)));
//G312: __E___E_bf_93_95(219) => bf_ref(167).
	p(NT(219), (NT(167)));
//G313: __E___E_bf_93_95(219) => bf_neg(174).
	p(NT(219), (NT(174)));
//G314: __E___E_bf_93_95(219) => capture(17).
	p(NT(219), (NT(17)));
//G315: bf_and_nosep_2nd_oprnd(45) => __E___E_bf_93_95(219).
	p(NT(45), (NT(219)));
//G316: __E_bf_93(217)       => bf_and_nosep_1st_oprnd(44) bf_and_nosep_2nd_oprnd(45).
	p(NT(217), (NT(44)+NT(45)));
//G317: bf_and_nosep(46)     => __E_bf_93(217).
	p(NT(46), (NT(217)));
//G318: bf(40)               => bf_and_nosep(46).
	p(NT(40), (NT(46)));
//G319: bf(40)               => capture(17).
	p(NT(40), (NT(17)));
//G320: __E_constraint_96(221) => '[' ctnvar(22) _(9) '!' '=' _(9) num(170) ']'.
	p(NT(221), (T(20)+NT(22)+NT(9)+T(38)+T(3)+NT(9)+NT(170)+T(21)));
//G321: __E_constraint_96(221) => '[' num(170) _(9) '!' '=' _(9) ctnvar(22) ']'.
	p(NT(221), (T(20)+NT(170)+NT(9)+T(38)+T(3)+NT(9)+NT(22)+T(21)));
//G322: ctn_neq(220)         => __E_constraint_96(221).
	p(NT(220), (NT(221)));
//G323: constraint(119)      => ctn_neq(220).
	p(NT(119), (NT(220)));
//G324: __E_constraint_97(223) => '[' ctnvar(22) _(9) '=' _(9) num(170) ']'.
	p(NT(223), (T(20)+NT(22)+NT(9)+T(3)+NT(9)+NT(170)+T(21)));
//G325: __E_constraint_97(223) => '[' num(170) _(9) '=' _(9) ctnvar(22) ']'.
	p(NT(223), (T(20)+NT(170)+NT(9)+T(3)+NT(9)+NT(22)+T(21)));
//G326: ctn_eq(222)          => __E_constraint_97(223).
	p(NT(222), (NT(223)));
//G327: constraint(119)      => ctn_eq(222).
	p(NT(119), (NT(222)));
//G328: __E_constraint_98(225) => '[' ctnvar(22) _(9) '>' '=' _(9) num(170) ']'.
	p(NT(225), (T(20)+NT(22)+NT(9)+T(30)+T(3)+NT(9)+NT(170)+T(21)));
//G329: __E_constraint_98(225) => '[' num(170) _(9) '>' '=' _(9) ctnvar(22) ']'.
	p(NT(225), (T(20)+NT(170)+NT(9)+T(30)+T(3)+NT(9)+NT(22)+T(21)));
//G330: ctn_gteq(224)        => __E_constraint_98(225).
	p(NT(224), (NT(225)));
//G331: constraint(119)      => ctn_gteq(224).
	p(NT(119), (NT(224)));
//G332: __E_constraint_99(227) => '[' ctnvar(22) _(9) '>' _(9) num(170) ']'.
	p(NT(227), (T(20)+NT(22)+NT(9)+T(30)+NT(9)+NT(170)+T(21)));
//G333: __E_constraint_99(227) => '[' num(170) _(9) '>' _(9) ctnvar(22) ']'.
	p(NT(227), (T(20)+NT(170)+NT(9)+T(30)+NT(9)+NT(22)+T(21)));
//G334: ctn_gt(226)          => __E_constraint_99(227).
	p(NT(226), (NT(227)));
//G335: constraint(119)      => ctn_gt(226).
	p(NT(119), (NT(226)));
//G336: __E_constraint_100(229) => '[' ctnvar(22) _(9) '<' '=' _(9) num(170) ']'.
	p(NT(229), (T(20)+NT(22)+NT(9)+T(29)+T(3)+NT(9)+NT(170)+T(21)));
//G337: __E_constraint_100(229) => '[' num(170) _(9) '<' '=' _(9) ctnvar(22) ']'.
	p(NT(229), (T(20)+NT(170)+NT(9)+T(29)+T(3)+NT(9)+NT(22)+T(21)));
//G338: ctn_lteq(228)        => __E_constraint_100(229).
	p(NT(228), (NT(229)));
//G339: constraint(119)      => ctn_lteq(228).
	p(NT(119), (NT(228)));
//G340: __E_constraint_101(231) => '[' ctnvar(22) _(9) '<' _(9) num(170) ']'.
	p(NT(231), (T(20)+NT(22)+NT(9)+T(29)+NT(9)+NT(170)+T(21)));
//G341: __E_constraint_101(231) => '[' num(170) _(9) '<' _(9) ctnvar(22) ']'.
	p(NT(231), (T(20)+NT(170)+NT(9)+T(29)+NT(9)+NT(22)+T(21)));
//G342: ctn_lt(230)          => __E_constraint_101(231).
	p(NT(230), (NT(231)));
//G343: constraint(119)      => ctn_lt(230).
	p(NT(119), (NT(230)));
//G344: __E_ba_constant_102(232) => capture(17).
	p(NT(232), (NT(17)));
//G345: __E_ba_constant_102(232) => source(16).
	p(NT(232), (NT(16)));
//G346: __E_ba_constant_103(233) => typed(70).
	p(NT(233), (NT(70)));
//G347: __E_ba_constant_103(233) => null.
	p(NT(233), (nul));
//G348: ba_constant(161)     => '{' _(9) __E_ba_constant_102(232) _(9) '}' __E_ba_constant_103(233).
	p(NT(161), (T(22)+NT(9)+NT(232)+NT(9)+T(23)+NT(233)));
//G349: __E___E_source_104_105(235) => src_c(29).
	p(NT(235), (NT(29)));
//G350: __E___E_source_104_105(235) => space(2).
	p(NT(235), (NT(2)));
//G351: __E___E_source_104_106(236) => null.
	p(NT(236), (nul));
//G352: __E___E_source_104_106(236) => __E___E_source_104_106(236) __E___E_source_104_105(235).
	p(NT(236), (NT(236)+NT(235)));
//G353: __E_source_104(234)  => __E___E_source_104_106(236) src_c(29).
	p(NT(234), (NT(236)+NT(29)));
//G354: __E_source_104(234)  => null.
	p(NT(234), (nul));
//G355: source(16)           => src_c(29) __E_source_104(234).
	p(NT(16), (NT(29)+NT(234)));
//G356: src_c(29)            => alnum(6).
	p(NT(29), (NT(6)));
//G357: __N_21(431)          => '{'.
	p(NT(431), (T(22)));
//G358: __N_22(432)          => '}'.
	p(NT(432), (T(23)));
//G359: src_c(29)            => ~( __N_21(431) ) & ~( __N_22(432) ) & punct(7).	 # conjunctive
	p(NT(29), ~(NT(431)) & ~(NT(432)) & (NT(7)));
//G360: __E_src_c_107(237)   => src_c(29).
	p(NT(237), (NT(29)));
//G361: __E_src_c_107(237)   => space(2).
	p(NT(237), (NT(2)));
//G362: __E_src_c_108(238)   => null.
	p(NT(238), (nul));
//G363: __E_src_c_108(238)   => __E_src_c_108(238) __E_src_c_107(237).
	p(NT(238), (NT(238)+NT(237)));
//G364: src_c(29)            => '{' __E_src_c_108(238) '}'.
	p(NT(29), (T(22)+NT(238)+T(23)));
//G365: __E_variable_109(239) => uconst(30).
	p(NT(239), (NT(30)));
//G366: __E_variable_109(239) => io_var(240).
	p(NT(239), (NT(240)));
//G367: __E_variable_109(239) => var_name(18).
	p(NT(239), (NT(18)));
//G368: __E_variable_110(241) => member_path(68).
	p(NT(241), (NT(68)));
//G369: __E_variable_110(241) => null.
	p(NT(241), (nul));
//G370: __E_variable_111(242) => typed(70).
	p(NT(242), (NT(70)));
//G371: __E_variable_111(242) => null.
	p(NT(242), (nul));
//G372: variable(162)        => __E_variable_109(239) __E_variable_110(241) __E_variable_111(242).
	p(NT(162), (NT(239)+NT(241)+NT(242)));
//G373: __N_23(433)          => 'F'.
	p(NT(433), (T(40)));
//G374: __N_24(434)          => 'T'.
	p(NT(434), (T(39)));
//G375: __E_var_name_112(243) => ~( __N_23(433) ) & ~( __N_24(434) ) & alpha(5).	 # conjunctive
	p(NT(243), ~(NT(433)) & ~(NT(434)) & (NT(5)));
//G376: __E_var_name_113(244) => null.
	p(NT(244), (nul));
//G377: __E_var_name_113(244) => __E_var_name_113(244) digit(3).
	p(NT(244), (NT(244)+NT(3)));
//G378: var_name(18)         => __E_var_name_112(243) __E_var_name_113(244).	 # guarded: charvar
	p(NT(18), (NT(243)+NT(244)));
	p.back().guard = "charvar";
//G379: __N_25(435)          => 'F'.
	p(NT(435), (T(40)));
//G380: __N_26(436)          => 'T'.
	p(NT(436), (T(39)));
//G381: __E_var_name_114(245) => ~( __N_25(435) ) & ~( __N_26(436) ) & alpha(5).	 # conjunctive
	p(NT(245), ~(NT(435)) & ~(NT(436)) & (NT(5)));
//G382: __E_var_name_115(246) => alnum(6).
	p(NT(246), (NT(6)));
//G383: __E_var_name_115(246) => '_'.
	p(NT(246), (T(48)));
//G384: __E_var_name_116(247) => null.
	p(NT(247), (nul));
//G385: __E_var_name_116(247) => __E_var_name_116(247) __E_var_name_115(246).
	p(NT(247), (NT(247)+NT(246)));
//G386: var_name(18)         => __E_var_name_114(245) __E_var_name_116(247).	 # guarded: var
	p(NT(18), (NT(245)+NT(247)));
	p.back().guard = "var";
//G387: io_var(240)          => io_var_name(19) '[' offset(248) ']'.
	p(NT(240), (NT(19)+T(20)+NT(248)+T(21)));
//G388: io_var_name(19)      => chars(27).
	p(NT(19), (NT(27)));
//G389: bv_type_like(160)    => 'b' 'v' _(9) '[' _(9) num(170) _(9) ']'.
	p(NT(160), (T(25)+T(41)+NT(9)+T(20)+NT(9)+NT(170)+NT(9)+T(21)));
//G390: __E___E_uconst_117_118(250) => chars(27) _(9).
	p(NT(250), (NT(27)+NT(9)));
//G391: __E___E_uconst_117_118(250) => null.
	p(NT(250), (nul));
//G392: __E_uconst_117(249)  => __E___E_uconst_117_118(250) ':' _(9) chars(27).
	p(NT(249), (NT(250)+T(2)+NT(9)+NT(27)));
//G393: uconst_name(20)      => __E_uconst_117(249).
	p(NT(20), (NT(249)));
//G394: uconst(30)           => '<' _(9) uconst_name(20) _(9) '>'.
	p(NT(30), (T(29)+NT(9)+NT(20)+NT(9)+T(30)));
//G395: __E_q_vars_119(251)  => _(9) ',' _(9) q_var(28).
	p(NT(251), (NT(9)+T(19)+NT(9)+NT(28)));
//G396: __E_q_vars_120(252)  => null.
	p(NT(252), (nul));
//G397: __E_q_vars_120(252)  => __E_q_vars_120(252) __E_q_vars_119(251).
	p(NT(252), (NT(252)+NT(251)));
//G398: q_vars(115)          => q_var(28) __E_q_vars_120(252).
	p(NT(115), (NT(28)+NT(252)));
//G399: q_var(28)            => capture(17).
	p(NT(28), (NT(17)));
//G400: __N_27(437)          => uconst(30).
	p(NT(437), (NT(30)));
//G401: q_var(28)            => ~( __N_27(437) ) & variable(162).	 # conjunctive
	p(NT(28), ~(NT(437)) & (NT(162)));
//G402: ctnvar(22)           => chars(27).
	p(NT(22), (NT(27)));
//G403: __E_offsets_121(253) => _(9) ',' _(9) offset(248).
	p(NT(253), (NT(9)+T(19)+NT(9)+NT(248)));
//G404: __E_offsets_122(254) => null.
	p(NT(254), (nul));
//G405: __E_offsets_122(254) => __E_offsets_122(254) __E_offsets_121(253).
	p(NT(254), (NT(254)+NT(253)));
//G406: offsets(92)          => '[' _(9) offset(248) __E_offsets_122(254) _(9) ']'.
	p(NT(92), (T(20)+NT(9)+NT(248)+NT(254)+NT(9)+T(21)));
//G407: offset(248)          => integer(15).
	p(NT(248), (NT(15)));
//G408: offset(248)          => capture(17).
	p(NT(248), (NT(17)));
//G409: offset(248)          => shift(255).
	p(NT(248), (NT(255)));
//G410: __N_28(438)          => io_var(240).
	p(NT(438), (NT(240)));
//G411: __E_offset_123(256)  => variable(162) & ~( __N_28(438) ).	 # conjunctive
	p(NT(256), (NT(162)) & ~(NT(438)));
//G412: offset(248)          => __E_offset_123(256).
	p(NT(248), (NT(256)));
//G413: __E_shift_124(257)   => capture(17).
	p(NT(257), (NT(17)));
//G414: __N_29(439)          => io_var(240).
	p(NT(439), (NT(240)));
//G415: __E___E_shift_124_125(258) => variable(162) & ~( __N_29(439) ).	 # conjunctive
	p(NT(258), (NT(162)) & ~(NT(439)));
//G416: __E_shift_124(257)   => __E___E_shift_124_125(258).
	p(NT(257), (NT(258)));
//G417: shift(255)           => __E_shift_124(257) _(9) '-' _(9) num(170).
	p(NT(255), (NT(257)+NT(9)+T(34)+NT(9)+NT(170)));
//G418: __E_chars_126(259)   => alnum(6).
	p(NT(259), (NT(6)));
//G419: __E_chars_126(259)   => '_'.
	p(NT(259), (T(48)));
//G420: __E_chars_127(260)   => null.
	p(NT(260), (nul));
//G421: __E_chars_127(260)   => __E_chars_127(260) __E_chars_126(259).
	p(NT(260), (NT(260)+NT(259)));
//G422: chars(27)            => alpha(5) __E_chars_127(260).
	p(NT(27), (NT(5)+NT(260)));
//G423: __E_digits_128(261)  => digit(3).
	p(NT(261), (NT(3)));
//G424: __E_digits_128(261)  => __E_digits_128(261) digit(3).
	p(NT(261), (NT(261)+NT(3)));
//G425: digits(12)           => __E_digits_128(261).
	p(NT(12), (NT(261)));
//G426: num(170)             => digits(12).
	p(NT(170), (NT(12)));
//G427: __E_integer_129(262) => '-'.
	p(NT(262), (T(34)));
//G428: __E_integer_129(262) => null.
	p(NT(262), (nul));
//G429: integer(15)          => __E_integer_129(262) _(9) digits(12).
	p(NT(15), (NT(262)+NT(9)+NT(12)));
//G430: sym(11)              => chars(27).
	p(NT(11), (NT(27)));
//G431: capture(17)          => '$' chars(27).
	p(NT(17), (T(49)+NT(27)));
//G432: typed(70)            => _(9) ':' _(9) type(13) _(9) '[' _(9) subtype(14) _(9) ']'.
	p(NT(70), (NT(9)+T(2)+NT(9)+NT(13)+NT(9)+T(20)+NT(9)+NT(14)+NT(9)+T(21)));
//G433: __N_30(440)          => 'b' 'v'.
	p(NT(440), (T(25)+T(41)));
//G434: __E_typed_130(263)   => ~( __N_30(440) ) & type(13).	 # conjunctive
	p(NT(263), ~(NT(440)) & (NT(13)));
//G435: typed(70)            => _(9) ':' _(9) __E_typed_130(263).
	p(NT(70), (NT(9)+T(2)+NT(9)+NT(263)));
//G436: type(13)             => chars(27).
	p(NT(13), (NT(27)));
//G437: subtype(14)          => num(170).
	p(NT(14), (NT(170)));
//G438: __E_comment_131(265) => printable(8).
	p(NT(265), (NT(8)));
//G439: __E_comment_131(265) => '\t'.
	p(NT(265), (T(50)));
//G440: __E_comment_132(266) => null.
	p(NT(266), (nul));
//G441: __E_comment_132(266) => __E_comment_132(266) __E_comment_131(265).
	p(NT(266), (NT(266)+NT(265)));
//G442: __E_comment_133(267) => '\n'.
	p(NT(267), (T(51)));
//G443: __E_comment_133(267) => '\r'.
	p(NT(267), (T(52)));
//G444: __E_comment_133(267) => eof(1).
	p(NT(267), (NT(1)));
//G445: comment(264)         => '#' __E_comment_132(266) __E_comment_133(267).
	p(NT(264), (T(53)+NT(266)+NT(267)));
//G446: __(10)               => space(2).
	p(NT(10), (NT(2)));
//G447: __(10)               => comment(264).
	p(NT(10), (NT(264)));
//G448: __(10)               => __(10) space(2).
	p(NT(10), (NT(10)+NT(2)));
//G449: __(10)               => __(10) comment(264).
	p(NT(10), (NT(10)+NT(264)));
//G450: _(9)                 => __(10).
	p(NT(9), (NT(10)));
//G451: _(9)                 => null.
	p(NT(9), (nul));
//G452: cli(268)             => _(9).
	p(NT(268), (NT(9)));
//G453: __E_cli_134(270)     => '.' _(9) cli_command(269) _(9).
	p(NT(270), (T(1)+NT(9)+NT(269)+NT(9)));
//G454: __E_cli_135(271)     => null.
	p(NT(271), (nul));
//G455: __E_cli_135(271)     => __E_cli_135(271) __E_cli_134(270).
	p(NT(271), (NT(271)+NT(270)));
//G456: __E_cli_136(272)     => '.' _(9).
	p(NT(272), (T(1)+NT(9)));
//G457: __E_cli_136(272)     => null.
	p(NT(272), (nul));
//G458: cli(268)             => _(9) cli_command(269) _(9) __E_cli_135(271) __E_cli_136(272).
	p(NT(268), (NT(9)+NT(269)+NT(9)+NT(271)+NT(272)));
//G459: __E_cli_command_137(275) => 'q'.
	p(NT(275), (T(54)));
//G460: __E_cli_command_137(275) => 'q' 'u' 'i' 't'.
	p(NT(275), (T(54)+T(7)+T(4)+T(8)));
//G461: quit_sym(274)        => __E_cli_command_137(275).
	p(NT(274), (NT(275)));
//G462: quit_cmd(273)        => quit_sym(274).
	p(NT(273), (NT(274)));
//G463: cli_command(269)     => quit_cmd(273).
	p(NT(269), (NT(273)));
//G464: __E_cli_command_138(278) => 'v'.
	p(NT(278), (T(41)));
//G465: __E_cli_command_138(278) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(278), (T(41)+T(11)+T(27)+T(15)+T(4)+T(6)+T(5)));
//G466: version_sym(277)     => __E_cli_command_138(278).
	p(NT(277), (NT(278)));
//G467: version_cmd(276)     => version_sym(277).
	p(NT(276), (NT(277)));
//G468: cli_command(269)     => version_cmd(276).
	p(NT(269), (NT(276)));
//G469: __E_cli_command_139(281) => 'c'.
	p(NT(281), (T(14)));
//G470: __E_cli_command_139(281) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(281), (T(14)+T(10)+T(11)+T(24)+T(27)));
//G471: clear_sym(280)       => __E_cli_command_139(281).
	p(NT(280), (NT(281)));
//G472: clear_cmd(279)       => clear_sym(280).
	p(NT(279), (NT(280)));
//G473: cli_command(269)     => clear_cmd(279).
	p(NT(269), (NT(279)));
//G474: __E___E_cli_command_140_141(285) => 'h'.
	p(NT(285), (T(55)));
//G475: __E___E_cli_command_140_141(285) => 'h' 'e' 'l' 'p'.
	p(NT(285), (T(55)+T(11)+T(10)+T(18)));
//G476: help_sym(284)        => __E___E_cli_command_140_141(285).
	p(NT(284), (NT(285)));
//G477: __E___E_cli_command_140_142(286) => __(10) help_arg(287).
	p(NT(286), (NT(10)+NT(287)));
//G478: __E___E_cli_command_140_142(286) => null.
	p(NT(286), (nul));
//G479: __E_cli_command_140(283) => help_sym(284) __E___E_cli_command_140_142(286).
	p(NT(283), (NT(284)+NT(286)));
//G480: help_cmd(282)        => __E_cli_command_140(283).
	p(NT(282), (NT(283)));
//G481: cli_command(269)     => help_cmd(282).
	p(NT(269), (NT(282)));
//G482: file_sym(290)        => 'f' 'i' 'l' 'e'.
	p(NT(290), (T(9)+T(4)+T(10)+T(11)));
//G483: __E_cli_command_143(289) => file_sym(290) __(10) q_string(291).
	p(NT(289), (NT(290)+NT(10)+NT(291)));
//G484: file_cmd(288)        => __E_cli_command_143(289).
	p(NT(288), (NT(289)));
//G485: cli_command(269)     => file_cmd(288).
	p(NT(269), (NT(288)));
//G486: valid_sym(294)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(294), (T(41)+T(24)+T(10)+T(4)+T(56)));
//G487: __E_cli_command_144(293) => valid_sym(294) __(10) spec_arg(37).
	p(NT(293), (NT(294)+NT(10)+NT(37)));
//G488: valid_cmd(292)       => __E_cli_command_144(293).
	p(NT(292), (NT(293)));
//G489: cli_command(269)     => valid_cmd(292).
	p(NT(269), (NT(292)));
//G490: sat_sym(297)         => 's' 'a' 't'.
	p(NT(297), (T(15)+T(24)+T(8)));
//G491: __E_cli_command_145(296) => sat_sym(297) __(10) spec_arg(37).
	p(NT(296), (NT(297)+NT(10)+NT(37)));
//G492: sat_cmd(295)         => __E_cli_command_145(296).
	p(NT(295), (NT(296)));
//G493: cli_command(269)     => sat_cmd(295).
	p(NT(269), (NT(295)));
//G494: unsat_sym(300)       => 'u' 'n' 's' 'a' 't'.
	p(NT(300), (T(7)+T(5)+T(15)+T(24)+T(8)));
//G495: __E_cli_command_146(299) => unsat_sym(300) __(10) spec_arg(37).
	p(NT(299), (NT(300)+NT(10)+NT(37)));
//G496: unsat_cmd(298)       => __E_cli_command_146(299).
	p(NT(298), (NT(299)));
//G497: cli_command(269)     => unsat_cmd(298).
	p(NT(269), (NT(298)));
//G498: solve_sym(303)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(303), (T(15)+T(6)+T(10)+T(41)+T(11)));
//G499: __E___E_cli_command_147_148(304) => solve_options(305).
	p(NT(304), (NT(305)));
//G500: __E___E_cli_command_147_148(304) => null.
	p(NT(304), (nul));
//G501: __E_cli_command_147(302) => solve_sym(303) __E___E_cli_command_147_148(304) __(10) wff_cmd_arg(34).
	p(NT(302), (NT(303)+NT(304)+NT(10)+NT(34)));
//G502: solve_cmd(301)       => __E_cli_command_147(302).
	p(NT(301), (NT(302)));
//G503: cli_command(269)     => solve_cmd(301).
	p(NT(269), (NT(301)));
//G504: lgrs_sym(308)        => 'l' 'g' 'r' 's'.
	p(NT(308), (T(10)+T(57)+T(27)+T(15)));
//G505: __E_cli_command_149(307) => lgrs_sym(308) __(10) wff_cmd_arg(34).
	p(NT(307), (NT(308)+NT(10)+NT(34)));
//G506: lgrs_cmd(306)        => __E_cli_command_149(307).
	p(NT(306), (NT(307)));
//G507: cli_command(269)     => lgrs_cmd(306).
	p(NT(269), (NT(306)));
//G508: __E___E_cli_command_150_151(312) => 'r'.
	p(NT(312), (T(27)));
//G509: __E___E_cli_command_150_151(312) => 'r' 'u' 'n'.
	p(NT(312), (T(27)+T(7)+T(5)));
//G510: run_sym(311)         => __E___E_cli_command_150_151(312).
	p(NT(311), (NT(312)));
//G511: __E_cli_command_150(310) => run_sym(311) __(10) spec_arg(37).
	p(NT(310), (NT(311)+NT(10)+NT(37)));
//G512: run_cmd(309)         => __E_cli_command_150(310).
	p(NT(309), (NT(310)));
//G513: cli_command(269)     => run_cmd(309).
	p(NT(269), (NT(309)));
//G514: __E___E_cli_command_152_153(316) => 'n'.
	p(NT(316), (T(5)));
//G515: __E___E_cli_command_152_153(316) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(316), (T(5)+T(6)+T(27)+T(28)+T(24)+T(10)+T(4)+T(58)+T(11)));
//G516: normalize_sym(315)   => __E___E_cli_command_152_153(316).
	p(NT(315), (NT(316)));
//G517: __E_cli_command_152(314) => normalize_sym(315) __(10) spec_arg(37).
	p(NT(314), (NT(315)+NT(10)+NT(37)));
//G518: normalize_cmd(313)   => __E_cli_command_152(314).
	p(NT(313), (NT(314)));
//G519: cli_command(269)     => normalize_cmd(313).
	p(NT(269), (NT(313)));
//G520: __E___E_cli_command_154_155(320) => 's'.
	p(NT(320), (T(15)));
//G521: __E___E_cli_command_154_155(320) => 's' 'u' 'b' 's' 't'.
	p(NT(320), (T(15)+T(7)+T(25)+T(15)+T(8)));
//G522: __E___E_cli_command_154_155(320) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(320), (T(15)+T(7)+T(25)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G523: subst_sym(319)       => __E___E_cli_command_154_155(320).
	p(NT(319), (NT(320)));
//G524: __E_cli_command_154(318) => subst_sym(319) __(10) nf_cmd_arg(35) _(9) '[' _(9) nf_cmd_arg(35) _(9) '/' _(9) nf_cmd_arg(35) _(9) ']'.
	p(NT(318), (NT(319)+NT(10)+NT(35)+NT(9)+T(20)+NT(9)+NT(35)+NT(9)+T(16)+NT(9)+NT(35)+NT(9)+T(21)));
//G525: subst_cmd(317)       => __E_cli_command_154(318).
	p(NT(317), (NT(318)));
//G526: cli_command(269)     => subst_cmd(317).
	p(NT(269), (NT(317)));
//G527: __E___E_cli_command_156_157(324) => 'i'.
	p(NT(324), (T(4)));
//G528: __E___E_cli_command_156_157(324) => 'i' 'n' 's' 't'.
	p(NT(324), (T(4)+T(5)+T(15)+T(8)));
//G529: __E___E_cli_command_156_157(324) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(324), (T(4)+T(5)+T(15)+T(8)+T(24)+T(5)+T(8)+T(4)+T(24)+T(8)+T(11)));
//G530: inst_sym(323)        => __E___E_cli_command_156_157(324).
	p(NT(323), (NT(324)));
//G531: __E_cli_command_156(322) => inst_sym(323) __(10) inst_args(32).
	p(NT(322), (NT(323)+NT(10)+NT(32)));
//G532: inst_cmd(321)        => __E_cli_command_156(322).
	p(NT(321), (NT(322)));
//G533: cli_command(269)     => inst_cmd(321).
	p(NT(269), (NT(321)));
//G534: dnf_sym(327)         => 'd' 'n' 'f'.
	p(NT(327), (T(56)+T(5)+T(9)));
//G535: __E_cli_command_158(326) => dnf_sym(327) __(10) nf_cmd_arg(35).
	p(NT(326), (NT(327)+NT(10)+NT(35)));
//G536: dnf_cmd(325)         => __E_cli_command_158(326).
	p(NT(325), (NT(326)));
//G537: cli_command(269)     => dnf_cmd(325).
	p(NT(269), (NT(325)));
//G538: cnf_sym(330)         => 'c' 'n' 'f'.
	p(NT(330), (T(14)+T(5)+T(9)));
//G539: __E_cli_command_159(329) => cnf_sym(330) __(10) nf_cmd_arg(35).
	p(NT(329), (NT(330)+NT(10)+NT(35)));
//G540: cnf_cmd(328)         => __E_cli_command_159(329).
	p(NT(328), (NT(329)));
//G541: cli_command(269)     => cnf_cmd(328).
	p(NT(269), (NT(328)));
//G542: nnf_sym(333)         => 'n' 'n' 'f'.
	p(NT(333), (T(5)+T(5)+T(9)));
//G543: __E_cli_command_160(332) => nnf_sym(333) __(10) nf_cmd_arg(35).
	p(NT(332), (NT(333)+NT(10)+NT(35)));
//G544: nnf_cmd(331)         => __E_cli_command_160(332).
	p(NT(331), (NT(332)));
//G545: cli_command(269)     => nnf_cmd(331).
	p(NT(269), (NT(331)));
//G546: mnf_sym(336)         => 'm' 'n' 'f'.
	p(NT(336), (T(28)+T(5)+T(9)));
//G547: __E_cli_command_161(335) => mnf_sym(336) __(10) nf_cmd_arg(35).
	p(NT(335), (NT(336)+NT(10)+NT(35)));
//G548: mnf_cmd(334)         => __E_cli_command_161(335).
	p(NT(334), (NT(335)));
//G549: cli_command(269)     => mnf_cmd(334).
	p(NT(269), (NT(334)));
//G550: onf_sym(339)         => 'o' 'n' 'f'.
	p(NT(339), (T(6)+T(5)+T(9)));
//G551: __E_cli_command_162(338) => onf_sym(339) __(10) variable(162) __(10) onf_cmd_arg(36).
	p(NT(338), (NT(339)+NT(10)+NT(162)+NT(10)+NT(36)));
//G552: onf_cmd(337)         => __E_cli_command_162(338).
	p(NT(337), (NT(338)));
//G553: cli_command(269)     => onf_cmd(337).
	p(NT(269), (NT(337)));
//G554: qelim_sym(342)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(342), (T(54)+T(11)+T(10)+T(4)+T(28)));
//G555: __E_cli_command_163(341) => qelim_sym(342) __(10) wff_cmd_arg(34).
	p(NT(341), (NT(342)+NT(10)+NT(34)));
//G556: qelim_cmd(340)       => __E_cli_command_163(341).
	p(NT(340), (NT(341)));
//G557: cli_command(269)     => qelim_cmd(340).
	p(NT(269), (NT(340)));
//G558: get_sym(345)         => 'g' 'e' 't'.
	p(NT(345), (T(57)+T(11)+T(8)));
//G559: __E___E_cli_command_164_165(346) => __(10) option_name(23).
	p(NT(346), (NT(10)+NT(23)));
//G560: __E___E_cli_command_164_165(346) => null.
	p(NT(346), (nul));
//G561: __E_cli_command_164(344) => get_sym(345) __E___E_cli_command_164_165(346).
	p(NT(344), (NT(345)+NT(346)));
//G562: get_cmd(343)         => __E_cli_command_164(344).
	p(NT(343), (NT(344)));
//G563: cli_command(269)     => get_cmd(343).
	p(NT(269), (NT(343)));
//G564: set_sym(349)         => 's' 'e' 't'.
	p(NT(349), (T(15)+T(11)+T(8)));
//G565: __E___E_cli_command_166_167(350) => __(10).
	p(NT(350), (NT(10)));
//G566: __E___E_cli_command_166_167(350) => _(9) '=' _(9).
	p(NT(350), (NT(9)+T(3)+NT(9)));
//G567: __E_cli_command_166(348) => set_sym(349) __(10) option_name(23) __E___E_cli_command_166_167(350) option_value(24).
	p(NT(348), (NT(349)+NT(10)+NT(23)+NT(350)+NT(24)));
//G568: set_cmd(347)         => __E_cli_command_166(348).
	p(NT(347), (NT(348)));
//G569: cli_command(269)     => set_cmd(347).
	p(NT(269), (NT(347)));
//G570: enable_sym(353)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(353), (T(11)+T(5)+T(24)+T(25)+T(10)+T(11)));
//G571: __E_cli_command_168(352) => enable_sym(353) __(10) option_name(23).
	p(NT(352), (NT(353)+NT(10)+NT(23)));
//G572: enable_cmd(351)      => __E_cli_command_168(352).
	p(NT(351), (NT(352)));
//G573: cli_command(269)     => enable_cmd(351).
	p(NT(269), (NT(351)));
//G574: disable_sym(356)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(356), (T(56)+T(4)+T(15)+T(24)+T(25)+T(10)+T(11)));
//G575: __E_cli_command_169(355) => disable_sym(356) __(10) option_name(23).
	p(NT(355), (NT(356)+NT(10)+NT(23)));
//G576: disable_cmd(354)     => __E_cli_command_169(355).
	p(NT(354), (NT(355)));
//G577: cli_command(269)     => disable_cmd(354).
	p(NT(269), (NT(354)));
//G578: toggle_sym(359)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(359), (T(8)+T(6)+T(57)+T(57)+T(10)+T(11)));
//G579: __E_cli_command_170(358) => toggle_sym(359) __(10) option_name(23).
	p(NT(358), (NT(359)+NT(10)+NT(23)));
//G580: toggle_cmd(357)      => __E_cli_command_170(358).
	p(NT(357), (NT(358)));
//G581: cli_command(269)     => toggle_cmd(357).
	p(NT(269), (NT(357)));
//G582: __E___E_cli_command_171_172(363) => 'd' 'e' 'f' 's'.
	p(NT(363), (T(56)+T(11)+T(9)+T(15)));
//G583: __E___E_cli_command_171_172(363) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(363), (T(56)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G584: def_sym(362)         => __E___E_cli_command_171_172(363).
	p(NT(362), (NT(363)));
//G585: __E_cli_command_171(361) => def_sym(362).
	p(NT(361), (NT(362)));
//G586: def_list_cmd(360)    => __E_cli_command_171(361).
	p(NT(360), (NT(361)));
//G587: cli_command(269)     => def_list_cmd(360).
	p(NT(269), (NT(360)));
//G588: __E_cli_command_173(365) => def_sym(362) __(10) num(170).
	p(NT(365), (NT(362)+NT(10)+NT(170)));
//G589: def_print_cmd(364)   => __E_cli_command_173(365).
	p(NT(364), (NT(365)));
//G590: cli_command(269)     => def_print_cmd(364).
	p(NT(269), (NT(364)));
//G591: def_rr_cmd(366)      => rec_relation(56).
	p(NT(366), (NT(56)));
//G592: cli_command(269)     => def_rr_cmd(366).
	p(NT(269), (NT(366)));
//G593: def_input_cmd(367)   => input_def(57).
	p(NT(367), (NT(57)));
//G594: cli_command(269)     => def_input_cmd(367).
	p(NT(269), (NT(367)));
//G595: def_output_cmd(368)  => output_def(58).
	p(NT(368), (NT(58)));
//G596: cli_command(269)     => def_output_cmd(368).
	p(NT(269), (NT(368)));
//G597: def_type_cmd(369)    => type_def(59).
	p(NT(369), (NT(59)));
//G598: cli_command(269)     => def_type_cmd(369).
	p(NT(269), (NT(369)));
//G599: __E___E_cli_command_174_175(373) => 'h' 'i' 's' 't'.
	p(NT(373), (T(55)+T(4)+T(15)+T(8)));
//G600: __E___E_cli_command_174_175(373) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(373), (T(55)+T(4)+T(15)+T(8)+T(6)+T(27)+T(17)));
//G601: history_sym(372)     => __E___E_cli_command_174_175(373).
	p(NT(372), (NT(373)));
//G602: __E_cli_command_174(371) => history_sym(372).
	p(NT(371), (NT(372)));
//G603: history_list_cmd(370) => __E_cli_command_174(371).
	p(NT(370), (NT(371)));
//G604: cli_command(269)     => history_list_cmd(370).
	p(NT(269), (NT(370)));
//G605: __E_cli_command_176(375) => history_sym(372) __(10) history(376).
	p(NT(375), (NT(372)+NT(10)+NT(376)));
//G606: history_print_cmd(374) => __E_cli_command_176(375).
	p(NT(374), (NT(375)));
//G607: cli_command(269)     => history_print_cmd(374).
	p(NT(269), (NT(374)));
//G608: __E_cli_command_177(378) => ref(65).
	p(NT(378), (NT(65)));
//G609: __E_cli_command_177(378) => wff(38).
	p(NT(378), (NT(38)));
//G610: __E_cli_command_177(378) => bf(40).
	p(NT(378), (NT(40)));
//G611: history_store_cmd(377) => __E_cli_command_177(378).
	p(NT(377), (NT(378)));
//G612: cli_command(269)     => history_store_cmd(377).
	p(NT(269), (NT(377)));
//G613: __E_solve_options_178(379) => __(10) solve_option(380).
	p(NT(379), (NT(10)+NT(380)));
//G614: __E_solve_options_179(381) => null.
	p(NT(381), (nul));
//G615: __E_solve_options_179(381) => __E_solve_options_179(381) __E_solve_options_178(379).
	p(NT(381), (NT(381)+NT(379)));
//G616: solve_options(305)   => __E_solve_options_179(381).
	p(NT(305), (NT(381)));
//G617: __E_solve_option_180(382) => solver_mode(383).
	p(NT(382), (NT(383)));
//G618: __E_solve_option_180(382) => type(13).
	p(NT(382), (NT(13)));
//G619: solve_option(380)    => '-' '-' __E_solve_option_180(382).
	p(NT(380), (T(34)+T(34)+NT(382)));
//G620: __E_solver_mode_181(385) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(385), (T(28)+T(4)+T(5)+T(4)+T(28)+T(7)+T(28)));
//G621: __E_solver_mode_181(385) => 'm' 'i' 'n'.
	p(NT(385), (T(28)+T(4)+T(5)));
//G622: solver_mode_minimum(384) => __E_solver_mode_181(385).
	p(NT(384), (NT(385)));
//G623: solver_mode(383)     => solver_mode_minimum(384).
	p(NT(383), (NT(384)));
//G624: __E_solver_mode_182(387) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(387), (T(28)+T(24)+T(33)+T(4)+T(28)+T(7)+T(28)));
//G625: __E_solver_mode_182(387) => 'm' 'a' 'x'.
	p(NT(387), (T(28)+T(24)+T(33)));
//G626: solver_mode_maximum(386) => __E_solver_mode_182(387).
	p(NT(386), (NT(387)));
//G627: solver_mode(383)     => solver_mode_maximum(386).
	p(NT(383), (NT(386)));
//G628: q_file_name(74)      => '"' file_name(21) '"'.
	p(NT(74), (T(59)+NT(21)+T(59)));
//G629: __E_file_name_183(388) => printable(8).
	p(NT(388), (NT(8)));
//G630: __E_file_name_183(388) => __E_file_name_183(388) printable(8).
	p(NT(388), (NT(388)+NT(8)));
//G631: file_name(21)        => __E_file_name_183(388).
	p(NT(21), (NT(388)));
//G632: __E_option_name_184(389) => alnum(6).
	p(NT(389), (NT(6)));
//G633: __E_option_name_184(389) => __E_option_name_184(389) alnum(6).
	p(NT(389), (NT(389)+NT(6)));
//G634: option_name(23)      => __E_option_name_184(389).
	p(NT(23), (NT(389)));
//G635: __E_option_value_185(390) => alnum(6).
	p(NT(390), (NT(6)));
//G636: __E_option_value_185(390) => '.'.
	p(NT(390), (T(1)));
//G637: __E_option_value_186(391) => __E_option_value_185(390).
	p(NT(391), (NT(390)));
//G638: __E_option_value_186(391) => __E_option_value_186(391) __E_option_value_185(390).
	p(NT(391), (NT(391)+NT(390)));
//G639: option_value(24)     => __E_option_value_186(391).
	p(NT(24), (NT(391)));
//G640: bf_cmd_arg(33)       => history(376).
	p(NT(33), (NT(376)));
//G641: bf_cmd_arg(33)       => bf(40).
	p(NT(33), (NT(40)));
//G642: wff_cmd_arg(34)      => history(376).
	p(NT(34), (NT(376)));
//G643: wff_cmd_arg(34)      => wff(38).
	p(NT(34), (NT(38)));
//G644: nf_cmd_arg(35)       => history(376).
	p(NT(35), (NT(376)));
//G645: nf_cmd_arg(35)       => ref(65).
	p(NT(35), (NT(65)));
//G646: nf_cmd_arg(35)       => wff(38).
	p(NT(35), (NT(38)));
//G647: nf_cmd_arg(35)       => bf(40).
	p(NT(35), (NT(40)));
//G648: onf_cmd_arg(36)      => history(376).
	p(NT(36), (NT(376)));
//G649: onf_cmd_arg(36)      => wff(38).
	p(NT(36), (NT(38)));
//G650: spec_arg(37)         => history(376).
	p(NT(37), (NT(376)));
//G651: spec_arg(37)         => spec(49).
	p(NT(37), (NT(49)));
//G652: spec_arg(37)         => ref(65).
	p(NT(37), (NT(65)));
//G653: spec_arg(37)         => wff(38).
	p(NT(37), (NT(38)));
//G654: spec_arg(37)         => bf(40).
	p(NT(37), (NT(40)));
//G655: inst_args(32)        => wff_cmd_arg(34) _(9) '[' _(9) bf(40) _(9) '/' _(9) bf_cmd_arg(33) _(9) ']'.
	p(NT(32), (NT(34)+NT(9)+T(20)+NT(9)+NT(40)+NT(9)+T(16)+NT(9)+NT(33)+NT(9)+T(21)));
//G656: inst_args(32)        => bf_cmd_arg(33) _(9) '[' _(9) bf(40) _(9) '/' _(9) bf_cmd_arg(33) _(9) ']'.
	p(NT(32), (NT(33)+NT(9)+T(20)+NT(9)+NT(40)+NT(9)+T(16)+NT(9)+NT(33)+NT(9)+T(21)));
//G657: help_arg(287)        => help_sym(284).
	p(NT(287), (NT(284)));
//G658: help_arg(287)        => version_sym(277).
	p(NT(287), (NT(277)));
//G659: help_arg(287)        => quit_sym(274).
	p(NT(287), (NT(274)));
//G660: help_arg(287)        => clear_sym(280).
	p(NT(287), (NT(280)));
//G661: help_arg(287)        => get_sym(345).
	p(NT(287), (NT(345)));
//G662: help_arg(287)        => set_sym(349).
	p(NT(287), (NT(349)));
//G663: help_arg(287)        => enable_sym(353).
	p(NT(287), (NT(353)));
//G664: help_arg(287)        => disable_sym(356).
	p(NT(287), (NT(356)));
//G665: help_arg(287)        => toggle_sym(359).
	p(NT(287), (NT(359)));
//G666: help_arg(287)        => file_sym(290).
	p(NT(287), (NT(290)));
//G667: help_arg(287)        => history_sym(372).
	p(NT(287), (NT(372)));
//G668: help_arg(287)        => abs_history_sym(392).
	p(NT(287), (NT(392)));
//G669: help_arg(287)        => rel_history_sym(393).
	p(NT(287), (NT(393)));
//G670: help_arg(287)        => selection_sym(394).
	p(NT(287), (NT(394)));
//G671: help_arg(287)        => def_sym(362).
	p(NT(287), (NT(362)));
//G672: help_arg(287)        => inst_sym(323).
	p(NT(287), (NT(323)));
//G673: help_arg(287)        => subst_sym(319).
	p(NT(287), (NT(319)));
//G674: help_arg(287)        => normalize_sym(315).
	p(NT(287), (NT(315)));
//G675: help_arg(287)        => execute_sym(395).
	p(NT(287), (NT(395)));
//G676: help_arg(287)        => solve_sym(303).
	p(NT(287), (NT(303)));
//G677: help_arg(287)        => valid_sym(294).
	p(NT(287), (NT(294)));
//G678: help_arg(287)        => sat_sym(297).
	p(NT(287), (NT(297)));
//G679: help_arg(287)        => unsat_sym(300).
	p(NT(287), (NT(300)));
//G680: help_arg(287)        => run_sym(311).
	p(NT(287), (NT(311)));
//G681: help_arg(287)        => dnf_sym(327).
	p(NT(287), (NT(327)));
//G682: help_arg(287)        => cnf_sym(330).
	p(NT(287), (NT(330)));
//G683: help_arg(287)        => snf_sym(396).
	p(NT(287), (NT(396)));
//G684: help_arg(287)        => nnf_sym(333).
	p(NT(287), (NT(333)));
//G685: help_arg(287)        => mnf_sym(336).
	p(NT(287), (NT(336)));
//G686: help_arg(287)        => onf_sym(339).
	p(NT(287), (NT(339)));
//G687: help_arg(287)        => qelim_sym(342).
	p(NT(287), (NT(342)));
//G688: __E___E_help_arg_187_188(399) => 's'.
	p(NT(399), (T(15)));
//G689: __E___E_help_arg_187_188(399) => null.
	p(NT(399), (nul));
//G690: __E_help_arg_187(398) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_187_188(399).
	p(NT(398), (T(11)+T(33)+T(24)+T(28)+T(18)+T(10)+T(11)+NT(399)));
//G691: examples_sym(397)    => __E_help_arg_187(398).
	p(NT(397), (NT(398)));
//G692: help_arg(287)        => examples_sym(397).
	p(NT(287), (NT(397)));
//G693: __E_history_189(401) => '%'.
	p(NT(401), (T(43)));
//G694: rel_history_sym(393) => '%' '-'.
	p(NT(393), (T(43)+T(34)));
//G695: history_id(403)      => digits(12).
	p(NT(403), (NT(12)));
//G696: __E___E_history_189_190(402) => history_id(403).
	p(NT(402), (NT(403)));
//G697: __E___E_history_189_190(402) => null.
	p(NT(402), (nul));
//G698: __E_history_189(401) => rel_history_sym(393) __E___E_history_189_190(402).
	p(NT(401), (NT(393)+NT(402)));
//G699: rel_history(400)     => __E_history_189(401).
	p(NT(400), (NT(401)));
//G700: history(376)         => rel_history(400).
	p(NT(376), (NT(400)));
//G701: abs_history_sym(392) => '%'.
	p(NT(392), (T(43)));
//G702: __E_history_191(405) => abs_history_sym(392) history_id(403).
	p(NT(405), (NT(392)+NT(403)));
//G703: abs_history(404)     => __E_history_191(405).
	p(NT(404), (NT(405)));
//G704: history(376)         => abs_history(404).
	p(NT(376), (NT(404)));
//G705: unreachable(406)     => BDD_ID(407).
	p(NT(406), (NT(407)));
//G706: stream_def(408)      => input_def(57).
	p(NT(408), (NT(57)));
//G707: stream_def(408)      => output_def(58).
	p(NT(408), (NT(58)));
//G708: fm_or_term(409)      => wff(38).
	p(NT(409), (NT(38)));
//G709: fm_or_term(409)      => bf(40).
	p(NT(409), (NT(40)));
	#undef T
	#undef NT
	return loaded = true, p;
}
#else
idni::prods<char_type, terminal_type>& productions();
#endif

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace tau_parser_data

struct tau_parser_nonterminals {
	enum nonterminal {
		nul, eof, space, digit, xdigit, alpha, alnum, punct, printable, _, 
		__, sym, digits, type, subtype, integer, source, capture, var_name, io_var_name, 
		uconst_name, file_name, ctnvar, option_name, option_value, type_name, member_name, chars, q_var, src_c, 
		uconst, spec_part, inst_args, bf_cmd_arg, wff_cmd_arg, nf_cmd_arg, onf_cmd_arg, spec_arg, wff, wff_parenthesis, 
		bf, bf_parenthesis, bf_neg_oprnd, bf_cast_oprnd, bf_and_nosep_1st_oprnd, bf_and_nosep_2nd_oprnd, bf_and_nosep, bf_and, start, spec, 
		__E_spec_0, definitions, main, __E_spec_1, __E_definitions_2, __E___E_definitions_2_3, rec_relation, input_def, output_def, type_def, 
		__E_definitions_4, spec_multiline, __E_spec_multiline_5, __E_spec_multiline_6, __E_spec_multiline_7, ref, __E_rec_relation_8, __E_input_def_9, member_path, __E_input_def_10, 
		typed, stream, __E_output_def_11, __E_output_def_12, q_file_name, console_sym, __E_type_def_13, __E___E_type_def_13_14, type_parents, __E_type_def_15, 
		type_body, __E_type_parents_16, __E_type_parents_17, tuple, __E_type_body_18, member, __E_tuple_19, __E_tuple_20, __E_tuple_21, __E_member_path_22, 
		__E_member_path_23, __E_ref_24, offsets, ref_args, __E_ref_25, __E_ref_26, fp_fallback, first_sym, last_sym, __E_ref_args_27, 
		__E___E_ref_args_27_28, ref_arg, __E___E_ref_args_27_29, __E___E_ref_args_27_30, __E_wff_31, wff_sometimes, __E_wff_32, __E___E_wff_32_33, wff_always, __E_wff_34, 
		__E___E_wff_34_35, wff_conditional, __E_wff_36, wff_all, __E_wff_37, q_vars, wff_ex, __E_wff_38, wff_ref, constraint, 
		wff_imply, __E_wff_39, wff_rimply, __E_wff_40, wff_equiv, __E_wff_41, wff_or, __E_wff_42, wff_xor, __E_wff_43, 
		wff_and, __E_wff_44, wff_neg, __E_wff_45, wff_t, wff_f, bf_interval, __E_wff_46, bf_eq, __E_wff_47, 
		bf_neq, __E_wff_48, bf_lt, __E_wff_49, bf_nlt, __E_wff_50, bf_lteq, __E_wff_51, bf_nlteq, __E_wff_52, 
		bf_gt, __E_wff_53, bf_ngt, __E_wff_54, bf_gteq, __E_wff_55, bf_ngteq, __E_wff_56, __E_bf_57, __E___E_bf_57_58, 
		bv_type_like, ba_constant, variable, bf_fall, __E_bf_59, bf_fex, __E_bf_60, bf_ref, bf_cast, __E_bf_61, 
		num, __E___E_bf_61_62, bf_t, bf_f, bf_neg, bf_shr, __E_bf_63, __E___E_bf_63_64, bf_shl, __E_bf_65, 
		__E___E_bf_65_66, bf_add, __E_bf_67, __E___E_bf_67_68, bf_sub, __E_bf_69, __E___E_bf_69_70, bf_mod, __E_bf_71, __E___E_bf_71_72, 
		bf_mul, bf_div, __E_bf_73, __E___E_bf_73_74, __E_bf_75, __E___E_bf_75_76, bf_nor, __E_bf_77, __E___E_bf_77_78, bf_xnor, 
		__E_bf_79, __E___E_bf_79_80, bf_nand, __E_bf_81, __E___E_bf_81_82, bf_or, __E_bf_83, bf_xor, __E_bf_84, __E_bf_85, 
		__E___E_bf_85_86, __E_bf_87, __E___E_bf_87_88, __E_bf_89, __E___E_bf_89_90, __E_bf_91, __E___E_bf_91_92, __E_bf_93, __E___E_bf_93_94, __E___E_bf_93_95, 
		ctn_neq, __E_constraint_96, ctn_eq, __E_constraint_97, ctn_gteq, __E_constraint_98, ctn_gt, __E_constraint_99, ctn_lteq, __E_constraint_100, 
		ctn_lt, __E_constraint_101, __E_ba_constant_102, __E_ba_constant_103, __E_source_104, __E___E_source_104_105, __E___E_source_104_106, __E_src_c_107, __E_src_c_108, __E_variable_109, 
		io_var, __E_variable_110, __E_variable_111, __E_var_name_112, __E_var_name_113, __E_var_name_114, __E_var_name_115, __E_var_name_116, offset, __E_uconst_117, 
		__E___E_uconst_117_118, __E_q_vars_119, __E_q_vars_120, __E_offsets_121, __E_offsets_122, shift, __E_offset_123, __E_shift_124, __E___E_shift_124_125, __E_chars_126, 
		__E_chars_127, __E_digits_128, __E_integer_129, __E_typed_130, comment, __E_comment_131, __E_comment_132, __E_comment_133, cli, cli_command, 
		__E_cli_134, __E_cli_135, __E_cli_136, quit_cmd, quit_sym, __E_cli_command_137, version_cmd, version_sym, __E_cli_command_138, clear_cmd, 
		clear_sym, __E_cli_command_139, help_cmd, __E_cli_command_140, help_sym, __E___E_cli_command_140_141, __E___E_cli_command_140_142, help_arg, file_cmd, __E_cli_command_143, 
		file_sym, q_string, valid_cmd, __E_cli_command_144, valid_sym, sat_cmd, __E_cli_command_145, sat_sym, unsat_cmd, __E_cli_command_146, 
		unsat_sym, solve_cmd, __E_cli_command_147, solve_sym, __E___E_cli_command_147_148, solve_options, lgrs_cmd, __E_cli_command_149, lgrs_sym, run_cmd, 
		__E_cli_command_150, run_sym, __E___E_cli_command_150_151, normalize_cmd, __E_cli_command_152, normalize_sym, __E___E_cli_command_152_153, subst_cmd, __E_cli_command_154, subst_sym, 
		__E___E_cli_command_154_155, inst_cmd, __E_cli_command_156, inst_sym, __E___E_cli_command_156_157, dnf_cmd, __E_cli_command_158, dnf_sym, cnf_cmd, __E_cli_command_159, 
		cnf_sym, nnf_cmd, __E_cli_command_160, nnf_sym, mnf_cmd, __E_cli_command_161, mnf_sym, onf_cmd, __E_cli_command_162, onf_sym, 
		qelim_cmd, __E_cli_command_163, qelim_sym, get_cmd, __E_cli_command_164, get_sym, __E___E_cli_command_164_165, set_cmd, __E_cli_command_166, set_sym, 
		__E___E_cli_command_166_167, enable_cmd, __E_cli_command_168, enable_sym, disable_cmd, __E_cli_command_169, disable_sym, toggle_cmd, __E_cli_command_170, toggle_sym, 
		def_list_cmd, __E_cli_command_171, def_sym, __E___E_cli_command_171_172, def_print_cmd, __E_cli_command_173, def_rr_cmd, def_input_cmd, def_output_cmd, def_type_cmd, 
		history_list_cmd, __E_cli_command_174, history_sym, __E___E_cli_command_174_175, history_print_cmd, __E_cli_command_176, history, history_store_cmd, __E_cli_command_177, __E_solve_options_178, 
		solve_option, __E_solve_options_179, __E_solve_option_180, solver_mode, solver_mode_minimum, __E_solver_mode_181, solver_mode_maximum, __E_solver_mode_182, __E_file_name_183, __E_option_name_184, 
		__E_option_value_185, __E_option_value_186, abs_history_sym, rel_history_sym, selection_sym, execute_sym, snf_sym, examples_sym, __E_help_arg_187, __E___E_help_arg_187_188, 
		rel_history, __E_history_189, __E___E_history_189_190, history_id, abs_history, __E_history_191, unreachable, BDD_ID, stream_def, fm_or_term, 
		__N_0, __N_1, __N_2, __N_3, __N_4, __N_5, __N_6, __N_7, __N_8, __N_9, 
		__N_10, __N_11, __N_12, __N_13, __N_14, __N_15, __N_16, __N_17, __N_18, __N_19, 
		__N_20, __N_21, __N_22, __N_23, __N_24, __N_25, __N_26, __N_27, __N_28, __N_29, 
		__N_30, 
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
