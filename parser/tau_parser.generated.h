// This file is generated from a file parser/tau.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
#ifndef __TAU_PARSER_H__
#define __TAU_PARSER_H__

#include "parser.h"

namespace tau_parser_data {

using char_type     = char;
using terminal_type = char;

inline static constexpr size_t nt_bits = 10;
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
	"__E___E_wff_34_35", "wff_A", "__E_wff_36", "wff_E", "__E_wff_37", "wff_semantic_neg", "__E_wff_38", "wff_conditional", "__E_wff_39", "wff_all", 
	"__E_wff_40", "q_vars", "wff_ex", "__E_wff_41", "wff_ref", "__E_wff_42", "bf_min", "bf_max", "constraint", "wff_imply", 
	"__E_wff_43", "wff_rimply", "__E_wff_44", "wff_equiv", "__E_wff_45", "wff_or", "__E_wff_46", "wff_xor", "__E_wff_47", "wff_and", 
	"__E_wff_48", "wff_neg", "__E_wff_49", "wff_t", "wff_f", "bf_interval", "__E_wff_50", "bf_eq", "__E_wff_51", "bf_neq", 
	"__E_wff_52", "bf_lt", "__E_wff_53", "bf_nlt", "__E_wff_54", "bf_lteq", "__E_wff_55", "bf_nlteq", "__E_wff_56", "bf_gt", 
	"__E_wff_57", "bf_ngt", "__E_wff_58", "bf_gteq", "__E_wff_59", "bf_ngteq", "__E_wff_60", "wff_U", "__E_wff_61", "__E___E_wff_61_62", 
	"wff_R", "__E_wff_63", "__E___E_wff_63_64", "wff_W", "__E_wff_65", "__E___E_wff_65_66", "wff_S", "__E_wff_67", "__E___E_wff_67_68", "wff_T", 
	"__E_wff_69", "__E___E_wff_69_70", "__E_bf_71", "__E___E_bf_71_72", "bv_type_like", "ba_constant", "variable", "bf_fall", "__E_bf_73", "bf_fex", 
	"__E_bf_74", "__E_bf_75", "__E_bf_76", "bf_ref", "bf_cast", "__E_bf_77", "num", "__E___E_bf_77_78", "bf_t", "bf_f", 
	"bf_neg", "bf_func_app", "__E_bf_79", "func_sym", "func_args", "bf_shr", "__E_bf_80", "__E___E_bf_80_81", "bf_shl", "__E_bf_82", 
	"__E___E_bf_82_83", "bf_add", "__E_bf_84", "__E___E_bf_84_85", "bf_sub", "__E_bf_86", "__E___E_bf_86_87", "bf_mod", "__E_bf_88", "__E___E_bf_88_89", 
	"bf_mul", "bf_div", "__E_bf_90", "__E___E_bf_90_91", "__E_bf_92", "__E___E_bf_92_93", "bf_nor", "__E_bf_94", "__E___E_bf_94_95", "bf_xnor", 
	"__E_bf_96", "__E___E_bf_96_97", "bf_nand", "__E_bf_98", "__E___E_bf_98_99", "bf_or", "__E_bf_100", "bf_xor", "__E_bf_101", "__E_bf_102", 
	"__E___E_bf_102_103", "__E_bf_104", "__E___E_bf_104_105", "__E_bf_106", "__E___E_bf_106_107", "__E_bf_108", "__E___E_bf_108_109", "__E_bf_110", "__E___E_bf_110_111", "__E___E_bf_110_112", 
	"__E_func_args_113", "__E_func_args_114", "func_mode", "func_dynamic", "func_static", "func_sort_list", "__E_func_sort_list_115", "__E_func_sort_list_116", "func_decl", "ctn_neq", 
	"__E_constraint_117", "ctn_eq", "__E_constraint_118", "ctn_gteq", "__E_constraint_119", "ctn_gt", "__E_constraint_120", "ctn_lteq", "__E_constraint_121", "ctn_lt", 
	"__E_constraint_122", "__E_ba_constant_123", "__E_ba_constant_124", "__E_ba_constant_125", "__E_ba_constant_126", "__E_source_127", "__E___E_source_127_128", "__E___E_source_127_129", "__E_src_c_130", "__E_src_c_131", 
	"__E_variable_132", "io_var", "__E_variable_133", "__E_variable_134", "__E_var_name_135", "__E_var_name_136", "__E_var_name_137", "__E_var_name_138", "__E_var_name_139", "offset", 
	"__E_uconst_140", "__E___E_uconst_140_141", "__E_q_vars_142", "__E_q_vars_143", "__E_offsets_144", "__E_offsets_145", "shift", "__E_offset_146", "__E_shift_147", "__E___E_shift_147_148", 
	"__E_chars_149", "__E_chars_150", "__E_digits_151", "__E_integer_152", "__E_typed_153", "comment", "__E_comment_154", "__E_comment_155", "__E_comment_156", "cli", 
	"cli_command", "__E_cli_157", "__E_cli_158", "__E_cli_159", "quit_cmd", "quit_sym", "__E_cli_command_160", "version_cmd", "version_sym", "__E_cli_command_161", 
	"clear_cmd", "clear_sym", "__E_cli_command_162", "help_cmd", "__E_cli_command_163", "help_sym", "__E___E_cli_command_163_164", "__E___E_cli_command_163_165", "help_arg", "file_cmd", 
	"__E_cli_command_166", "file_sym", "q_string", "valid_cmd", "__E_cli_command_167", "valid_sym", "sat_cmd", "__E_cli_command_168", "sat_sym", "unsat_cmd", 
	"__E_cli_command_169", "unsat_sym", "realizable_cmd", "__E_cli_command_170", "realizable_sym", "unrealizable_cmd", "__E_cli_command_171", "unrealizable_sym", "solve_cmd", "__E_cli_command_172", 
	"solve_sym", "__E___E_cli_command_172_173", "solve_options", "lgrs_cmd", "__E_cli_command_174", "lgrs_sym", "run_cmd", "__E_cli_command_175", "run_sym", "__E___E_cli_command_175_176", 
	"__E___E_cli_command_175_177", "__E___E___E_cli_command_175_177_178", "__E___E_cli_command_175_179", "stop_cmd", "__E_cli_command_180", "stop_sym", "memory_cmd", "__E_cli_command_181", "memory_sym", "ltl_cmd", 
	"__E_cli_command_182", "ltl_sym", "func_cmd", "__E_cli_command_183", "func_sym_cmd", "normalize_cmd", "__E_cli_command_184", "normalize_sym", "__E___E_cli_command_184_185", "subst_cmd", 
	"__E_cli_command_186", "subst_sym", "__E___E_cli_command_186_187", "__E___E_cli_command_186_188", "subst_group", "__E___E_cli_command_186_189", "inst_cmd", "__E_cli_command_190", "inst_sym", "__E___E_cli_command_190_191", 
	"dnf_cmd", "__E_cli_command_192", "dnf_sym", "cnf_cmd", "__E_cli_command_193", "cnf_sym", "nnf_cmd", "__E_cli_command_194", "nnf_sym", "mnf_cmd", 
	"__E_cli_command_195", "mnf_sym", "onf_cmd", "__E_cli_command_196", "onf_sym", "qelim_cmd", "__E_cli_command_197", "qelim_sym", "whatis_cmd", "__E_cli_command_198", 
	"whatis_sym", "reset_cmd", "__E_cli_command_199", "reset_sym", "fragment_cmd", "__E_cli_command_200", "fragment_sym", "fragment_name", "get_cmd", "__E_cli_command_201", 
	"get_sym", "__E___E_cli_command_201_202", "set_cmd", "__E_cli_command_203", "set_sym", "__E___E_cli_command_203_204", "enable_cmd", "__E_cli_command_205", "enable_sym", "disable_cmd", 
	"__E_cli_command_206", "disable_sym", "toggle_cmd", "__E_cli_command_207", "toggle_sym", "def_list_cmd", "__E_cli_command_208", "def_sym", "__E___E_cli_command_208_209", "def_print_cmd", 
	"__E_cli_command_210", "def_rr_cmd", "def_input_cmd", "def_output_cmd", "def_type_cmd", "history_list_cmd", "__E_cli_command_211", "history_sym", "__E___E_cli_command_211_212", "history_print_cmd", 
	"__E_cli_command_213", "history", "history_store_cmd", "__E_cli_command_214", "solve_option", "__E_solve_option_215", "solver_mode", "solver_mode_minimum", "__E_solver_mode_216", "solver_mode_maximum", 
	"__E_solver_mode_217", "__E_file_name_218", "__E_option_name_219", "__E_option_value_220", "__E_option_value_221", "__E_fragment_name_222", "fragment_ltl", "fragment_ctl_star", "__E_nf_cmd_arg_223", "__E_spec_arg_224", 
	"__E_subst_group_225", "__E_subst_group_226", "__E_inst_args_227", "inst_group", "__E_inst_args_228", "__E_inst_args_229", "__E_inst_args_230", "__E_inst_group_231", "__E_inst_group_232", "abs_history_sym", 
	"rel_history_sym", "selection_sym", "execute_sym", "snf_sym", "examples_sym", "__E_help_arg_233", "__E___E_help_arg_233_234", "rel_history", "__E_history_235", "__E___E_history_235_236", 
	"history_id", "abs_history", "__E_history_237", "unreachable", "BDD_ID", "stream_def", "fm_or_term", "__N_0", "__N_1", "__N_2", 
	"__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", "__N_9", "__N_10", "__N_11", "__N_12", 
	"__N_13", "__N_14", "__N_15", "__N_16", "__N_17", "__N_18", "__N_19", "__N_20", "__N_21", "__N_22", 
	"__N_23", "__N_24", "__N_25", "__N_26", "__N_27", "__N_28", "__N_29", "__N_30", "__N_31", "__N_32", 
	"__N_33", "__N_34", "__N_35", "__N_36", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', '/', 'y', 'p', 
	',', '[', ']', '{', '}', 'a', 'b', 'k', 'r', 'm', 
	'<', '>', 'F', 'w', 'G', 'A', 'E', '-', '?', 'x', 
	'|', '^', '&', '!', 'T', 'U', 'R', 'W', '_', 'S', 
	'g', 'v', '+', '%', '*', '1', '0', '\'', 'd', '$', 
	'\t', '\n', '\r', '#', 'q', 'h', 'z', '"', 
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
//G128: __N_0(497)           => bf_min(126).
	p(NT(497), (NT(126)));
//G129: __N_1(498)           => bf_max(127).
	p(NT(498), (NT(127)));
//G130: __E_wff_42(125)      => ref(65) & ~( __N_0(497) ) & ~( __N_1(498) ).	 # conjunctive
	p(NT(125), (NT(65)) & ~(NT(497)) & ~(NT(498)));
//G131: wff_ref(124)         => __E_wff_42(125).
	p(NT(124), (NT(125)));
//G132: wff(38)              => wff_ref(124).
	p(NT(38), (NT(124)));
//G133: wff(38)              => constraint(128).
	p(NT(38), (NT(128)));
//G134: __E_wff_43(130)      => wff(38) _(9) '-' '>' _(9) wff(38).
	p(NT(130), (NT(38)+NT(9)+T(36)+T(30)+NT(9)+NT(38)));
//G135: wff_imply(129)       => __E_wff_43(130).
	p(NT(129), (NT(130)));
//G136: wff(38)              => wff_imply(129).
	p(NT(38), (NT(129)));
//G137: __E_wff_44(132)      => wff(38) _(9) '<' '-' _(9) wff(38).
	p(NT(132), (NT(38)+NT(9)+T(29)+T(36)+NT(9)+NT(38)));
//G138: wff_rimply(131)      => __E_wff_44(132).
	p(NT(131), (NT(132)));
//G139: wff(38)              => wff_rimply(131).
	p(NT(38), (NT(131)));
//G140: __E_wff_45(134)      => wff(38) _(9) '<' '-' '>' _(9) wff(38).
	p(NT(134), (NT(38)+NT(9)+T(29)+T(36)+T(30)+NT(9)+NT(38)));
//G141: wff_equiv(133)       => __E_wff_45(134).
	p(NT(133), (NT(134)));
//G142: wff(38)              => wff_equiv(133).
	p(NT(38), (NT(133)));
//G143: __E_wff_46(136)      => wff(38) _(9) '|' '|' _(9) wff(38).
	p(NT(136), (NT(38)+NT(9)+T(39)+T(39)+NT(9)+NT(38)));
//G144: wff_or(135)          => __E_wff_46(136).
	p(NT(135), (NT(136)));
//G145: wff(38)              => wff_or(135).
	p(NT(38), (NT(135)));
//G146: __E_wff_47(138)      => wff(38) _(9) '^' '^' _(9) wff(38).
	p(NT(138), (NT(38)+NT(9)+T(40)+T(40)+NT(9)+NT(38)));
//G147: wff_xor(137)         => __E_wff_47(138).
	p(NT(137), (NT(138)));
//G148: wff(38)              => wff_xor(137).
	p(NT(38), (NT(137)));
//G149: __E_wff_48(140)      => wff(38) _(9) '&' '&' _(9) wff(38).
	p(NT(140), (NT(38)+NT(9)+T(41)+T(41)+NT(9)+NT(38)));
//G150: wff_and(139)         => __E_wff_48(140).
	p(NT(139), (NT(140)));
//G151: wff(38)              => wff_and(139).
	p(NT(38), (NT(139)));
//G152: __E_wff_49(142)      => '!' _(9) wff(38).
	p(NT(142), (T(42)+NT(9)+NT(38)));
//G153: wff_neg(141)         => __E_wff_49(142).
	p(NT(141), (NT(142)));
//G154: wff(38)              => wff_neg(141).
	p(NT(38), (NT(141)));
//G155: wff_t(143)           => 'T'.
	p(NT(143), (T(43)));
//G156: wff(38)              => wff_t(143).
	p(NT(38), (NT(143)));
//G157: wff_f(144)           => 'F'.
	p(NT(144), (T(31)));
//G158: wff(38)              => wff_f(144).
	p(NT(38), (NT(144)));
//G159: __E_wff_50(146)      => bf(40) _(9) '<' '=' _(9) bf(40) _(9) '<' '=' _(9) bf(40).
	p(NT(146), (NT(40)+NT(9)+T(29)+T(3)+NT(9)+NT(40)+NT(9)+T(29)+T(3)+NT(9)+NT(40)));
//G160: bf_interval(145)     => __E_wff_50(146).
	p(NT(145), (NT(146)));
//G161: wff(38)              => bf_interval(145).
	p(NT(38), (NT(145)));
//G162: __E_wff_51(148)      => bf(40) _(9) '=' _(9) bf(40).
	p(NT(148), (NT(40)+NT(9)+T(3)+NT(9)+NT(40)));
//G163: bf_eq(147)           => __E_wff_51(148).
	p(NT(147), (NT(148)));
//G164: wff(38)              => bf_eq(147).
	p(NT(38), (NT(147)));
//G165: __E_wff_52(150)      => bf(40) _(9) '!' '=' _(9) bf(40).
	p(NT(150), (NT(40)+NT(9)+T(42)+T(3)+NT(9)+NT(40)));
//G166: bf_neq(149)          => __E_wff_52(150).
	p(NT(149), (NT(150)));
//G167: wff(38)              => bf_neq(149).
	p(NT(38), (NT(149)));
//G168: __E_wff_53(152)      => bf(40) _(9) '<' _(9) bf(40).
	p(NT(152), (NT(40)+NT(9)+T(29)+NT(9)+NT(40)));
//G169: bf_lt(151)           => __E_wff_53(152).
	p(NT(151), (NT(152)));
//G170: wff(38)              => bf_lt(151).
	p(NT(38), (NT(151)));
//G171: __E_wff_54(154)      => bf(40) _(9) '!' '<' _(9) bf(40).
	p(NT(154), (NT(40)+NT(9)+T(42)+T(29)+NT(9)+NT(40)));
//G172: bf_nlt(153)          => __E_wff_54(154).
	p(NT(153), (NT(154)));
//G173: wff(38)              => bf_nlt(153).
	p(NT(38), (NT(153)));
//G174: __E_wff_55(156)      => bf(40) _(9) '<' '=' _(9) bf(40).
	p(NT(156), (NT(40)+NT(9)+T(29)+T(3)+NT(9)+NT(40)));
//G175: bf_lteq(155)         => __E_wff_55(156).
	p(NT(155), (NT(156)));
//G176: wff(38)              => bf_lteq(155).
	p(NT(38), (NT(155)));
//G177: __E_wff_56(158)      => bf(40) _(9) '!' '<' '=' _(9) bf(40).
	p(NT(158), (NT(40)+NT(9)+T(42)+T(29)+T(3)+NT(9)+NT(40)));
//G178: bf_nlteq(157)        => __E_wff_56(158).
	p(NT(157), (NT(158)));
//G179: wff(38)              => bf_nlteq(157).
	p(NT(38), (NT(157)));
//G180: __E_wff_57(160)      => bf(40) _(9) '>' _(9) bf(40).
	p(NT(160), (NT(40)+NT(9)+T(30)+NT(9)+NT(40)));
//G181: bf_gt(159)           => __E_wff_57(160).
	p(NT(159), (NT(160)));
//G182: wff(38)              => bf_gt(159).
	p(NT(38), (NT(159)));
//G183: __E_wff_58(162)      => bf(40) _(9) '!' '>' _(9) bf(40).
	p(NT(162), (NT(40)+NT(9)+T(42)+T(30)+NT(9)+NT(40)));
//G184: bf_ngt(161)          => __E_wff_58(162).
	p(NT(161), (NT(162)));
//G185: wff(38)              => bf_ngt(161).
	p(NT(38), (NT(161)));
//G186: __E_wff_59(164)      => bf(40) _(9) '>' '=' _(9) bf(40).
	p(NT(164), (NT(40)+NT(9)+T(30)+T(3)+NT(9)+NT(40)));
//G187: bf_gteq(163)         => __E_wff_59(164).
	p(NT(163), (NT(164)));
//G188: wff(38)              => bf_gteq(163).
	p(NT(38), (NT(163)));
//G189: __E_wff_60(166)      => bf(40) _(9) '!' '>' '=' _(9) bf(40).
	p(NT(166), (NT(40)+NT(9)+T(42)+T(30)+T(3)+NT(9)+NT(40)));
//G190: bf_ngteq(165)        => __E_wff_60(166).
	p(NT(165), (NT(166)));
//G191: wff(38)              => bf_ngteq(165).
	p(NT(38), (NT(165)));
//G192: __E___E_wff_61_62(169) => 'U'.
	p(NT(169), (T(44)));
//G193: __E___E_wff_61_62(169) => 'u' 'n' 't' 'i' 'l'.
	p(NT(169), (T(7)+T(5)+T(8)+T(4)+T(10)));
//G194: __E_wff_61(168)      => wff(38) _(9) __E___E_wff_61_62(169) _(9) wff(38).
	p(NT(168), (NT(38)+NT(9)+NT(169)+NT(9)+NT(38)));
//G195: wff_U(167)           => __E_wff_61(168).
	p(NT(167), (NT(168)));
//G196: wff(38)              => wff_U(167).
	p(NT(38), (NT(167)));
//G197: __E___E_wff_63_64(172) => 'R'.
	p(NT(172), (T(45)));
//G198: __E___E_wff_63_64(172) => 'r' 'e' 'l' 'e' 'a' 's' 'e'.
	p(NT(172), (T(27)+T(11)+T(10)+T(11)+T(24)+T(15)+T(11)));
//G199: __E_wff_63(171)      => wff(38) _(9) __E___E_wff_63_64(172) _(9) wff(38).
	p(NT(171), (NT(38)+NT(9)+NT(172)+NT(9)+NT(38)));
//G200: wff_R(170)           => __E_wff_63(171).
	p(NT(170), (NT(171)));
//G201: wff(38)              => wff_R(170).
	p(NT(38), (NT(170)));
//G202: __E___E_wff_65_66(175) => 'W'.
	p(NT(175), (T(46)));
//G203: __E___E_wff_65_66(175) => 'w' 'e' 'a' 'k' '_' 'u' 'n' 't' 'i' 'l'.
	p(NT(175), (T(32)+T(11)+T(24)+T(26)+T(47)+T(7)+T(5)+T(8)+T(4)+T(10)));
//G204: __E_wff_65(174)      => wff(38) _(9) __E___E_wff_65_66(175) _(9) wff(38).
	p(NT(174), (NT(38)+NT(9)+NT(175)+NT(9)+NT(38)));
//G205: wff_W(173)           => __E_wff_65(174).
	p(NT(173), (NT(174)));
//G206: wff(38)              => wff_W(173).
	p(NT(38), (NT(173)));
//G207: __E___E_wff_67_68(178) => 'S'.
	p(NT(178), (T(48)));
//G208: __E___E_wff_67_68(178) => 's' 'i' 'n' 'c' 'e'.
	p(NT(178), (T(15)+T(4)+T(5)+T(14)+T(11)));
//G209: __E_wff_67(177)      => wff(38) _(9) __E___E_wff_67_68(178) _(9) wff(38).
	p(NT(177), (NT(38)+NT(9)+NT(178)+NT(9)+NT(38)));
//G210: wff_S(176)           => __E_wff_67(177).
	p(NT(176), (NT(177)));
//G211: wff(38)              => wff_S(176).
	p(NT(38), (NT(176)));
//G212: __E___E_wff_69_70(181) => 'T'.
	p(NT(181), (T(43)));
//G213: __E___E_wff_69_70(181) => 't' 'r' 'i' 'g' 'g' 'e' 'r'.
	p(NT(181), (T(8)+T(27)+T(4)+T(49)+T(49)+T(11)+T(27)));
//G214: __E_wff_69(180)      => wff(38) _(9) __E___E_wff_69_70(181) _(9) wff(38).
	p(NT(180), (NT(38)+NT(9)+NT(181)+NT(9)+NT(38)));
//G215: wff_T(179)           => __E_wff_69(180).
	p(NT(179), (NT(180)));
//G216: wff(38)              => wff_T(179).
	p(NT(38), (NT(179)));
//G217: wff(38)              => capture(17).
	p(NT(38), (NT(17)));
//G218: __N_2(499)           => bv_type_like(184).
	p(NT(499), (NT(184)));
//G219: __E___E_bf_71_72(183) => bf(40) & ~( __N_2(499) ).	 # conjunctive
	p(NT(183), (NT(40)) & ~(NT(499)));
//G220: __E_bf_71(182)       => '(' _(9) __E___E_bf_71_72(183) _(9) ')'.
	p(NT(182), (T(12)+NT(9)+NT(183)+NT(9)+T(13)));
//G221: bf_parenthesis(41)   => __E_bf_71(182).
	p(NT(41), (NT(182)));
//G222: bf(40)               => bf_parenthesis(41).
	p(NT(40), (NT(41)));
//G223: bf(40)               => ba_constant(185).
	p(NT(40), (NT(185)));
//G224: bf(40)               => variable(186).
	p(NT(40), (NT(186)));
//G225: __E_bf_73(188)       => 'f' 'a' 'l' 'l' __(10) q_vars(121) __(10) bf(40).
	p(NT(188), (T(9)+T(24)+T(10)+T(10)+NT(10)+NT(121)+NT(10)+NT(40)));
//G226: bf_fall(187)         => __E_bf_73(188).
	p(NT(187), (NT(188)));
//G227: bf(40)               => bf_fall(187).
	p(NT(40), (NT(187)));
//G228: __E_bf_74(190)       => 'f' 'e' 'x' __(10) q_vars(121) __(10) bf(40).
	p(NT(190), (T(9)+T(11)+T(38)+NT(10)+NT(121)+NT(10)+NT(40)));
//G229: bf_fex(189)          => __E_bf_74(190).
	p(NT(189), (NT(190)));
//G230: bf(40)               => bf_fex(189).
	p(NT(40), (NT(189)));
//G231: __E_bf_75(191)       => 'm' 'i' 'n' _(9) '(' _(9) bf(40) _(9) ',' _(9) bf(40) _(9) ')'.
	p(NT(191), (T(28)+T(4)+T(5)+NT(9)+T(12)+NT(9)+NT(40)+NT(9)+T(19)+NT(9)+NT(40)+NT(9)+T(13)));
//G232: bf_min(126)          => __E_bf_75(191).
	p(NT(126), (NT(191)));
//G233: bf(40)               => bf_min(126).
	p(NT(40), (NT(126)));
//G234: __E_bf_76(192)       => 'm' 'a' 'x' _(9) '(' _(9) bf(40) _(9) ',' _(9) bf(40) _(9) ')'.
	p(NT(192), (T(28)+T(24)+T(38)+NT(9)+T(12)+NT(9)+NT(40)+NT(9)+T(19)+NT(9)+NT(40)+NT(9)+T(13)));
//G235: bf_max(127)          => __E_bf_76(192).
	p(NT(127), (NT(192)));
//G236: bf(40)               => bf_max(127).
	p(NT(40), (NT(127)));
//G237: bf_ref(193)          => ref(65).
	p(NT(193), (NT(65)));
//G238: bf(40)               => bf_ref(193).
	p(NT(40), (NT(193)));
//G239: __E___E_bf_77_78(197) => bf_parenthesis(41).
	p(NT(197), (NT(41)));
//G240: __E___E_bf_77_78(197) => ba_constant(185).
	p(NT(197), (NT(185)));
//G241: __E___E_bf_77_78(197) => variable(186).
	p(NT(197), (NT(186)));
//G242: __E___E_bf_77_78(197) => bf_ref(193).
	p(NT(197), (NT(193)));
//G243: __E___E_bf_77_78(197) => bf_t(198).
	p(NT(197), (NT(198)));
//G244: __E___E_bf_77_78(197) => bf_f(199).
	p(NT(197), (NT(199)));
//G245: __E___E_bf_77_78(197) => bf_neg(200).
	p(NT(197), (NT(200)));
//G246: __E___E_bf_77_78(197) => bf_cast(194).
	p(NT(197), (NT(194)));
//G247: __E___E_bf_77_78(197) => capture(17).
	p(NT(197), (NT(17)));
//G248: __E___E_bf_77_78(197) => bf_fall(187).
	p(NT(197), (NT(187)));
//G249: __E___E_bf_77_78(197) => bf_fex(189).
	p(NT(197), (NT(189)));
//G250: bf_cast_oprnd(43)    => __E___E_bf_77_78(197).
	p(NT(43), (NT(197)));
//G251: __E_bf_77(195)       => '(' _(9) 'b' 'v' _(9) '[' _(9) num(196) _(9) ']' _(9) ')' _(9) bf_cast_oprnd(43).
	p(NT(195), (T(12)+NT(9)+T(25)+T(50)+NT(9)+T(20)+NT(9)+NT(196)+NT(9)+T(21)+NT(9)+T(13)+NT(9)+NT(43)));
//G252: bf_cast(194)         => __E_bf_77(195).
	p(NT(194), (NT(195)));
//G253: bf(40)               => bf_cast(194).
	p(NT(40), (NT(194)));
//G254: __E_bf_79(202)       => func_sym(203) '(' _(9) func_args(204) _(9) ')'.
	p(NT(202), (NT(203)+T(12)+NT(9)+NT(204)+NT(9)+T(13)));
//G255: bf_func_app(201)     => __E_bf_79(202).
	p(NT(201), (NT(202)));
//G256: bf(40)               => bf_func_app(201).
	p(NT(40), (NT(201)));
//G257: __N_3(500)           => bf_shr(205).
	p(NT(500), (NT(205)));
//G258: __N_4(501)           => bf_shl(208).
	p(NT(501), (NT(208)));
//G259: __E___E_bf_80_81(207) => bf(40) & ~( __N_3(500) ) & ~( __N_4(501) ).	 # conjunctive
	p(NT(207), (NT(40)) & ~(NT(500)) & ~(NT(501)));
//G260: __E_bf_80(206)       => bf(40) _(9) '>' '>' _(9) __E___E_bf_80_81(207).
	p(NT(206), (NT(40)+NT(9)+T(30)+T(30)+NT(9)+NT(207)));
//G261: bf_shr(205)          => __E_bf_80(206).
	p(NT(205), (NT(206)));
//G262: bf(40)               => bf_shr(205).
	p(NT(40), (NT(205)));
//G263: __N_5(502)           => bf_shr(205).
	p(NT(502), (NT(205)));
//G264: __N_6(503)           => bf_shl(208).
	p(NT(503), (NT(208)));
//G265: __E___E_bf_82_83(210) => bf(40) & ~( __N_5(502) ) & ~( __N_6(503) ).	 # conjunctive
	p(NT(210), (NT(40)) & ~(NT(502)) & ~(NT(503)));
//G266: __E_bf_82(209)       => bf(40) _(9) '<' '<' _(9) __E___E_bf_82_83(210).
	p(NT(209), (NT(40)+NT(9)+T(29)+T(29)+NT(9)+NT(210)));
//G267: bf_shl(208)          => __E_bf_82(209).
	p(NT(208), (NT(209)));
//G268: bf(40)               => bf_shl(208).
	p(NT(40), (NT(208)));
//G269: __N_7(504)           => bf_add(211).
	p(NT(504), (NT(211)));
//G270: __N_8(505)           => bf_sub(214).
	p(NT(505), (NT(214)));
//G271: __E___E_bf_84_85(213) => bf(40) & ~( __N_7(504) ) & ~( __N_8(505) ).	 # conjunctive
	p(NT(213), (NT(40)) & ~(NT(504)) & ~(NT(505)));
//G272: __E_bf_84(212)       => bf(40) _(9) '+' _(9) __E___E_bf_84_85(213).
	p(NT(212), (NT(40)+NT(9)+T(51)+NT(9)+NT(213)));
//G273: bf_add(211)          => __E_bf_84(212).
	p(NT(211), (NT(212)));
//G274: bf(40)               => bf_add(211).
	p(NT(40), (NT(211)));
//G275: __N_9(506)           => bf_add(211).
	p(NT(506), (NT(211)));
//G276: __N_10(507)          => bf_sub(214).
	p(NT(507), (NT(214)));
//G277: __E___E_bf_86_87(216) => bf(40) & ~( __N_9(506) ) & ~( __N_10(507) ).	 # conjunctive
	p(NT(216), (NT(40)) & ~(NT(506)) & ~(NT(507)));
//G278: __E_bf_86(215)       => bf(40) _(9) '-' _(9) __E___E_bf_86_87(216).
	p(NT(215), (NT(40)+NT(9)+T(36)+NT(9)+NT(216)));
//G279: bf_sub(214)          => __E_bf_86(215).
	p(NT(214), (NT(215)));
//G280: bf(40)               => bf_sub(214).
	p(NT(40), (NT(214)));
//G281: __N_11(508)          => bf_mod(217).
	p(NT(508), (NT(217)));
//G282: __N_12(509)          => bf_mul(220).
	p(NT(509), (NT(220)));
//G283: __N_13(510)          => bf_div(221).
	p(NT(510), (NT(221)));
//G284: __E___E_bf_88_89(219) => bf(40) & ~( __N_11(508) ) & ~( __N_12(509) ) & ~( __N_13(510) ).	 # conjunctive
	p(NT(219), (NT(40)) & ~(NT(508)) & ~(NT(509)) & ~(NT(510)));
//G285: __E_bf_88(218)       => bf(40) _(9) '%' _(9) __E___E_bf_88_89(219).
	p(NT(218), (NT(40)+NT(9)+T(52)+NT(9)+NT(219)));
//G286: bf_mod(217)          => __E_bf_88(218).
	p(NT(217), (NT(218)));
//G287: bf(40)               => bf_mod(217).
	p(NT(40), (NT(217)));
//G288: __N_14(511)          => bf_mod(217).
	p(NT(511), (NT(217)));
//G289: __N_15(512)          => bf_mul(220).
	p(NT(512), (NT(220)));
//G290: __N_16(513)          => bf_div(221).
	p(NT(513), (NT(221)));
//G291: __E___E_bf_90_91(223) => bf(40) & ~( __N_14(511) ) & ~( __N_15(512) ) & ~( __N_16(513) ).	 # conjunctive
	p(NT(223), (NT(40)) & ~(NT(511)) & ~(NT(512)) & ~(NT(513)));
//G292: __E_bf_90(222)       => bf(40) _(9) '*' _(9) __E___E_bf_90_91(223).
	p(NT(222), (NT(40)+NT(9)+T(53)+NT(9)+NT(223)));
//G293: bf_mul(220)          => __E_bf_90(222).
	p(NT(220), (NT(222)));
//G294: bf(40)               => bf_mul(220).
	p(NT(40), (NT(220)));
//G295: __N_17(514)          => bf_mod(217).
	p(NT(514), (NT(217)));
//G296: __N_18(515)          => bf_mul(220).
	p(NT(515), (NT(220)));
//G297: __N_19(516)          => bf_div(221).
	p(NT(516), (NT(221)));
//G298: __E___E_bf_92_93(225) => bf(40) & ~( __N_17(514) ) & ~( __N_18(515) ) & ~( __N_19(516) ).	 # conjunctive
	p(NT(225), (NT(40)) & ~(NT(514)) & ~(NT(515)) & ~(NT(516)));
//G299: __E_bf_92(224)       => bf(40) _(9) '/' _(9) __E___E_bf_92_93(225).
	p(NT(224), (NT(40)+NT(9)+T(16)+NT(9)+NT(225)));
//G300: bf_div(221)          => __E_bf_92(224).
	p(NT(221), (NT(224)));
//G301: bf(40)               => bf_div(221).
	p(NT(40), (NT(221)));
//G302: __N_20(517)          => bf_nor(226).
	p(NT(517), (NT(226)));
//G303: __E___E_bf_94_95(228) => bf(40) & ~( __N_20(517) ).	 # conjunctive
	p(NT(228), (NT(40)) & ~(NT(517)));
//G304: __E_bf_94(227)       => bf(40) _(9) '!' '|' _(9) __E___E_bf_94_95(228).
	p(NT(227), (NT(40)+NT(9)+T(42)+T(39)+NT(9)+NT(228)));
//G305: bf_nor(226)          => __E_bf_94(227).
	p(NT(226), (NT(227)));
//G306: bf(40)               => bf_nor(226).
	p(NT(40), (NT(226)));
//G307: __N_21(518)          => bf_xnor(229).
	p(NT(518), (NT(229)));
//G308: __E___E_bf_96_97(231) => bf(40) & ~( __N_21(518) ).	 # conjunctive
	p(NT(231), (NT(40)) & ~(NT(518)));
//G309: __E_bf_96(230)       => bf(40) _(9) '!' '^' _(9) __E___E_bf_96_97(231).
	p(NT(230), (NT(40)+NT(9)+T(42)+T(40)+NT(9)+NT(231)));
//G310: bf_xnor(229)         => __E_bf_96(230).
	p(NT(229), (NT(230)));
//G311: bf(40)               => bf_xnor(229).
	p(NT(40), (NT(229)));
//G312: __N_22(519)          => bf_nand(232).
	p(NT(519), (NT(232)));
//G313: __E___E_bf_98_99(234) => bf(40) & ~( __N_22(519) ).	 # conjunctive
	p(NT(234), (NT(40)) & ~(NT(519)));
//G314: __E_bf_98(233)       => bf(40) _(9) '!' '&' _(9) __E___E_bf_98_99(234).
	p(NT(233), (NT(40)+NT(9)+T(42)+T(41)+NT(9)+NT(234)));
//G315: bf_nand(232)         => __E_bf_98(233).
	p(NT(232), (NT(233)));
//G316: bf(40)               => bf_nand(232).
	p(NT(40), (NT(232)));
//G317: __E_bf_100(236)      => bf(40) _(9) '|' _(9) bf(40).
	p(NT(236), (NT(40)+NT(9)+T(39)+NT(9)+NT(40)));
//G318: bf_or(235)           => __E_bf_100(236).
	p(NT(235), (NT(236)));
//G319: bf(40)               => bf_or(235).
	p(NT(40), (NT(235)));
//G320: __E_bf_101(238)      => bf(40) _(9) '^' _(9) bf(40).
	p(NT(238), (NT(40)+NT(9)+T(40)+NT(9)+NT(40)));
//G321: bf_xor(237)          => __E_bf_101(238).
	p(NT(237), (NT(238)));
//G322: bf(40)               => bf_xor(237).
	p(NT(40), (NT(237)));
//G323: __E___E_bf_102_103(240) => typed(70).
	p(NT(240), (NT(70)));
//G324: __E___E_bf_102_103(240) => null.
	p(NT(240), (nul));
//G325: __E_bf_102(239)      => '1' __E___E_bf_102_103(240).
	p(NT(239), (T(54)+NT(240)));
//G326: bf_t(198)            => __E_bf_102(239).
	p(NT(198), (NT(239)));
//G327: bf(40)               => bf_t(198).
	p(NT(40), (NT(198)));
//G328: __E___E_bf_104_105(242) => typed(70).
	p(NT(242), (NT(70)));
//G329: __E___E_bf_104_105(242) => null.
	p(NT(242), (nul));
//G330: __E_bf_104(241)      => '0' __E___E_bf_104_105(242).
	p(NT(241), (T(55)+NT(242)));
//G331: bf_f(199)            => __E_bf_104(241).
	p(NT(199), (NT(241)));
//G332: bf(40)               => bf_f(199).
	p(NT(40), (NT(199)));
//G333: __E___E_bf_106_107(244) => _(9) '&' _(9).
	p(NT(244), (NT(9)+T(41)+NT(9)));
//G334: __E___E_bf_106_107(244) => __(10).
	p(NT(244), (NT(10)));
//G335: __E_bf_106(243)      => bf(40) __E___E_bf_106_107(244) bf(40).
	p(NT(243), (NT(40)+NT(244)+NT(40)));
//G336: bf_and(47)           => __E_bf_106(243).
	p(NT(47), (NT(243)));
//G337: bf(40)               => bf_and(47).
	p(NT(40), (NT(47)));
//G338: __E___E_bf_108_109(246) => bf_parenthesis(41).
	p(NT(246), (NT(41)));
//G339: __E___E_bf_108_109(246) => ba_constant(185).
	p(NT(246), (NT(185)));
//G340: __E___E_bf_108_109(246) => variable(186).
	p(NT(246), (NT(186)));
//G341: __E___E_bf_108_109(246) => bf_ref(193).
	p(NT(246), (NT(193)));
//G342: __E___E_bf_108_109(246) => bf_t(198).
	p(NT(246), (NT(198)));
//G343: __E___E_bf_108_109(246) => bf_f(199).
	p(NT(246), (NT(199)));
//G344: __E___E_bf_108_109(246) => bf_neg(200).
	p(NT(246), (NT(200)));
//G345: __E___E_bf_108_109(246) => capture(17).
	p(NT(246), (NT(17)));
//G346: bf_neg_oprnd(42)     => __E___E_bf_108_109(246).
	p(NT(42), (NT(246)));
//G347: __E_bf_108(245)      => bf_neg_oprnd(42) _(9) '\''.
	p(NT(245), (NT(42)+NT(9)+T(56)));
//G348: bf_neg(200)          => __E_bf_108(245).
	p(NT(200), (NT(245)));
//G349: bf(40)               => bf_neg(200).
	p(NT(40), (NT(200)));
//G350: __E___E_bf_110_111(248) => bf_parenthesis(41).
	p(NT(248), (NT(41)));
//G351: __E___E_bf_110_111(248) => variable(186).
	p(NT(248), (NT(186)));
//G352: __E___E_bf_110_111(248) => bf_ref(193).
	p(NT(248), (NT(193)));
//G353: __E___E_bf_110_111(248) => bf_neg(200).
	p(NT(248), (NT(200)));
//G354: __E___E_bf_110_111(248) => bf_and_nosep(46).
	p(NT(248), (NT(46)));
//G355: __E___E_bf_110_111(248) => capture(17).
	p(NT(248), (NT(17)));
//G356: bf_and_nosep_1st_oprnd(44) => __E___E_bf_110_111(248).
	p(NT(44), (NT(248)));
//G357: __E___E_bf_110_112(249) => bf_parenthesis(41).
	p(NT(249), (NT(41)));
//G358: __E___E_bf_110_112(249) => ba_constant(185).
	p(NT(249), (NT(185)));
//G359: __E___E_bf_110_112(249) => variable(186).
	p(NT(249), (NT(186)));
//G360: __E___E_bf_110_112(249) => bf_ref(193).
	p(NT(249), (NT(193)));
//G361: __E___E_bf_110_112(249) => bf_neg(200).
	p(NT(249), (NT(200)));
//G362: __E___E_bf_110_112(249) => capture(17).
	p(NT(249), (NT(17)));
//G363: bf_and_nosep_2nd_oprnd(45) => __E___E_bf_110_112(249).
	p(NT(45), (NT(249)));
//G364: __E_bf_110(247)      => bf_and_nosep_1st_oprnd(44) bf_and_nosep_2nd_oprnd(45).
	p(NT(247), (NT(44)+NT(45)));
//G365: bf_and_nosep(46)     => __E_bf_110(247).
	p(NT(46), (NT(247)));
//G366: bf(40)               => bf_and_nosep(46).
	p(NT(40), (NT(46)));
//G367: bf(40)               => capture(17).
	p(NT(40), (NT(17)));
//G368: func_sym(203)        => chars(27).
	p(NT(203), (NT(27)));
//G369: __E_func_args_113(250) => _(9) ',' _(9) bf(40).
	p(NT(250), (NT(9)+T(19)+NT(9)+NT(40)));
//G370: __E_func_args_114(251) => null.
	p(NT(251), (nul));
//G371: __E_func_args_114(251) => __E_func_args_114(251) __E_func_args_113(250).
	p(NT(251), (NT(251)+NT(250)));
//G372: func_args(204)       => bf(40) __E_func_args_114(251).
	p(NT(204), (NT(40)+NT(251)));
//G373: func_dynamic(253)    => 'd' 'y' 'n' 'a' 'm' 'i' 'c'.
	p(NT(253), (T(57)+T(17)+T(5)+T(24)+T(28)+T(4)+T(14)));
//G374: func_mode(252)       => func_dynamic(253).
	p(NT(252), (NT(253)));
//G375: func_static(254)     => 's' 't' 'a' 't' 'i' 'c'.
	p(NT(254), (T(15)+T(8)+T(24)+T(8)+T(4)+T(14)));
//G376: func_mode(252)       => func_static(254).
	p(NT(252), (NT(254)));
//G377: __E_func_sort_list_115(256) => _(9) ',' _(9) type(13).
	p(NT(256), (NT(9)+T(19)+NT(9)+NT(13)));
//G378: __E_func_sort_list_116(257) => null.
	p(NT(257), (nul));
//G379: __E_func_sort_list_116(257) => __E_func_sort_list_116(257) __E_func_sort_list_115(256).
	p(NT(257), (NT(257)+NT(256)));
//G380: func_sort_list(255)  => type(13) __E_func_sort_list_116(257).
	p(NT(255), (NT(13)+NT(257)));
//G381: func_decl(258)       => func_mode(252) __(10) func_sym(203) _(9) ':' _(9) func_sort_list(255) _(9) '-' '>' _(9) type(13).
	p(NT(258), (NT(252)+NT(10)+NT(203)+NT(9)+T(2)+NT(9)+NT(255)+NT(9)+T(36)+T(30)+NT(9)+NT(13)));
//G382: __E_constraint_117(260) => '[' ctnvar(22) _(9) '!' '=' _(9) num(196) ']'.
	p(NT(260), (T(20)+NT(22)+NT(9)+T(42)+T(3)+NT(9)+NT(196)+T(21)));
//G383: __E_constraint_117(260) => '[' num(196) _(9) '!' '=' _(9) ctnvar(22) ']'.
	p(NT(260), (T(20)+NT(196)+NT(9)+T(42)+T(3)+NT(9)+NT(22)+T(21)));
//G384: ctn_neq(259)         => __E_constraint_117(260).
	p(NT(259), (NT(260)));
//G385: constraint(128)      => ctn_neq(259).
	p(NT(128), (NT(259)));
//G386: __E_constraint_118(262) => '[' ctnvar(22) _(9) '=' _(9) num(196) ']'.
	p(NT(262), (T(20)+NT(22)+NT(9)+T(3)+NT(9)+NT(196)+T(21)));
//G387: __E_constraint_118(262) => '[' num(196) _(9) '=' _(9) ctnvar(22) ']'.
	p(NT(262), (T(20)+NT(196)+NT(9)+T(3)+NT(9)+NT(22)+T(21)));
//G388: ctn_eq(261)          => __E_constraint_118(262).
	p(NT(261), (NT(262)));
//G389: constraint(128)      => ctn_eq(261).
	p(NT(128), (NT(261)));
//G390: __E_constraint_119(264) => '[' ctnvar(22) _(9) '>' '=' _(9) num(196) ']'.
	p(NT(264), (T(20)+NT(22)+NT(9)+T(30)+T(3)+NT(9)+NT(196)+T(21)));
//G391: __E_constraint_119(264) => '[' num(196) _(9) '>' '=' _(9) ctnvar(22) ']'.
	p(NT(264), (T(20)+NT(196)+NT(9)+T(30)+T(3)+NT(9)+NT(22)+T(21)));
//G392: ctn_gteq(263)        => __E_constraint_119(264).
	p(NT(263), (NT(264)));
//G393: constraint(128)      => ctn_gteq(263).
	p(NT(128), (NT(263)));
//G394: __E_constraint_120(266) => '[' ctnvar(22) _(9) '>' _(9) num(196) ']'.
	p(NT(266), (T(20)+NT(22)+NT(9)+T(30)+NT(9)+NT(196)+T(21)));
//G395: __E_constraint_120(266) => '[' num(196) _(9) '>' _(9) ctnvar(22) ']'.
	p(NT(266), (T(20)+NT(196)+NT(9)+T(30)+NT(9)+NT(22)+T(21)));
//G396: ctn_gt(265)          => __E_constraint_120(266).
	p(NT(265), (NT(266)));
//G397: constraint(128)      => ctn_gt(265).
	p(NT(128), (NT(265)));
//G398: __E_constraint_121(268) => '[' ctnvar(22) _(9) '<' '=' _(9) num(196) ']'.
	p(NT(268), (T(20)+NT(22)+NT(9)+T(29)+T(3)+NT(9)+NT(196)+T(21)));
//G399: __E_constraint_121(268) => '[' num(196) _(9) '<' '=' _(9) ctnvar(22) ']'.
	p(NT(268), (T(20)+NT(196)+NT(9)+T(29)+T(3)+NT(9)+NT(22)+T(21)));
//G400: ctn_lteq(267)        => __E_constraint_121(268).
	p(NT(267), (NT(268)));
//G401: constraint(128)      => ctn_lteq(267).
	p(NT(128), (NT(267)));
//G402: __E_constraint_122(270) => '[' ctnvar(22) _(9) '<' _(9) num(196) ']'.
	p(NT(270), (T(20)+NT(22)+NT(9)+T(29)+NT(9)+NT(196)+T(21)));
//G403: __E_constraint_122(270) => '[' num(196) _(9) '<' _(9) ctnvar(22) ']'.
	p(NT(270), (T(20)+NT(196)+NT(9)+T(29)+NT(9)+NT(22)+T(21)));
//G404: ctn_lt(269)          => __E_constraint_122(270).
	p(NT(269), (NT(270)));
//G405: constraint(128)      => ctn_lt(269).
	p(NT(128), (NT(269)));
//G406: __E_ba_constant_123(271) => null.
	p(NT(271), (nul));
//G407: __E_ba_constant_123(271) => __E_ba_constant_123(271) space(2).
	p(NT(271), (NT(271)+NT(2)));
//G408: __E_ba_constant_124(272) => capture(17).
	p(NT(272), (NT(17)));
//G409: __E_ba_constant_124(272) => source(16).
	p(NT(272), (NT(16)));
//G410: __E_ba_constant_125(273) => null.
	p(NT(273), (nul));
//G411: __E_ba_constant_125(273) => __E_ba_constant_125(273) space(2).
	p(NT(273), (NT(273)+NT(2)));
//G412: __E_ba_constant_126(274) => typed(70).
	p(NT(274), (NT(70)));
//G413: __E_ba_constant_126(274) => null.
	p(NT(274), (nul));
//G414: ba_constant(185)     => '{' __E_ba_constant_123(271) __E_ba_constant_124(272) __E_ba_constant_125(273) '}' __E_ba_constant_126(274).
	p(NT(185), (T(22)+NT(271)+NT(272)+NT(273)+T(23)+NT(274)));
//G415: __E___E_source_127_128(276) => src_c(29).
	p(NT(276), (NT(29)));
//G416: __E___E_source_127_128(276) => space(2).
	p(NT(276), (NT(2)));
//G417: __E___E_source_127_129(277) => null.
	p(NT(277), (nul));
//G418: __E___E_source_127_129(277) => __E___E_source_127_129(277) __E___E_source_127_128(276).
	p(NT(277), (NT(277)+NT(276)));
//G419: __E_source_127(275)  => __E___E_source_127_129(277) src_c(29).
	p(NT(275), (NT(277)+NT(29)));
//G420: __E_source_127(275)  => null.
	p(NT(275), (nul));
//G421: source(16)           => src_c(29) __E_source_127(275).
	p(NT(16), (NT(29)+NT(275)));
//G422: src_c(29)            => alnum(6).
	p(NT(29), (NT(6)));
//G423: __N_23(520)          => '{'.
	p(NT(520), (T(22)));
//G424: __N_24(521)          => '}'.
	p(NT(521), (T(23)));
//G425: src_c(29)            => ~( __N_23(520) ) & ~( __N_24(521) ) & punct(7).	 # conjunctive
	p(NT(29), ~(NT(520)) & ~(NT(521)) & (NT(7)));
//G426: __E_src_c_130(278)   => src_c(29).
	p(NT(278), (NT(29)));
//G427: __E_src_c_130(278)   => space(2).
	p(NT(278), (NT(2)));
//G428: __E_src_c_131(279)   => null.
	p(NT(279), (nul));
//G429: __E_src_c_131(279)   => __E_src_c_131(279) __E_src_c_130(278).
	p(NT(279), (NT(279)+NT(278)));
//G430: src_c(29)            => '{' __E_src_c_131(279) '}'.
	p(NT(29), (T(22)+NT(279)+T(23)));
//G431: __E_variable_132(280) => uconst(30).
	p(NT(280), (NT(30)));
//G432: __E_variable_132(280) => io_var(281).
	p(NT(280), (NT(281)));
//G433: __E_variable_132(280) => var_name(18).
	p(NT(280), (NT(18)));
//G434: __E_variable_133(282) => member_path(68).
	p(NT(282), (NT(68)));
//G435: __E_variable_133(282) => null.
	p(NT(282), (nul));
//G436: __E_variable_134(283) => typed(70).
	p(NT(283), (NT(70)));
//G437: __E_variable_134(283) => null.
	p(NT(283), (nul));
//G438: variable(186)        => __E_variable_132(280) __E_variable_133(282) __E_variable_134(283).
	p(NT(186), (NT(280)+NT(282)+NT(283)));
//G439: __N_25(522)          => 'F'.
	p(NT(522), (T(31)));
//G440: __N_26(523)          => 'T'.
	p(NT(523), (T(43)));
//G441: __E_var_name_135(284) => ~( __N_25(522) ) & ~( __N_26(523) ) & alpha(5).	 # conjunctive
	p(NT(284), ~(NT(522)) & ~(NT(523)) & (NT(5)));
//G442: __E_var_name_136(285) => null.
	p(NT(285), (nul));
//G443: __E_var_name_136(285) => __E_var_name_136(285) digit(3).
	p(NT(285), (NT(285)+NT(3)));
//G444: var_name(18)         => __E_var_name_135(284) __E_var_name_136(285).	 # guarded: charvar
	p(NT(18), (NT(284)+NT(285)));
	p.back().guard = "charvar";
//G445: __N_27(524)          => 'F'.
	p(NT(524), (T(31)));
//G446: __N_28(525)          => 'T'.
	p(NT(525), (T(43)));
//G447: __E_var_name_137(286) => ~( __N_27(524) ) & ~( __N_28(525) ) & alpha(5).	 # conjunctive
	p(NT(286), ~(NT(524)) & ~(NT(525)) & (NT(5)));
//G448: __E_var_name_138(287) => alnum(6).
	p(NT(287), (NT(6)));
//G449: __E_var_name_138(287) => '_'.
	p(NT(287), (T(47)));
//G450: __E_var_name_139(288) => null.
	p(NT(288), (nul));
//G451: __E_var_name_139(288) => __E_var_name_139(288) __E_var_name_138(287).
	p(NT(288), (NT(288)+NT(287)));
//G452: var_name(18)         => __E_var_name_137(286) __E_var_name_139(288).	 # guarded: var
	p(NT(18), (NT(286)+NT(288)));
	p.back().guard = "var";
//G453: io_var(281)          => io_var_name(19) '[' offset(289) ']'.
	p(NT(281), (NT(19)+T(20)+NT(289)+T(21)));
//G454: io_var_name(19)      => chars(27).
	p(NT(19), (NT(27)));
//G455: bv_type_like(184)    => 'b' 'v' _(9) '[' _(9) num(196) _(9) ']'.
	p(NT(184), (T(25)+T(50)+NT(9)+T(20)+NT(9)+NT(196)+NT(9)+T(21)));
//G456: __E___E_uconst_140_141(291) => chars(27) _(9).
	p(NT(291), (NT(27)+NT(9)));
//G457: __E___E_uconst_140_141(291) => null.
	p(NT(291), (nul));
//G458: __E_uconst_140(290)  => __E___E_uconst_140_141(291) ':' _(9) chars(27).
	p(NT(290), (NT(291)+T(2)+NT(9)+NT(27)));
//G459: uconst_name(20)      => __E_uconst_140(290).
	p(NT(20), (NT(290)));
//G460: uconst(30)           => '<' _(9) uconst_name(20) _(9) '>'.
	p(NT(30), (T(29)+NT(9)+NT(20)+NT(9)+T(30)));
//G461: __E_q_vars_142(292)  => _(9) ',' _(9) q_var(28).
	p(NT(292), (NT(9)+T(19)+NT(9)+NT(28)));
//G462: __E_q_vars_143(293)  => null.
	p(NT(293), (nul));
//G463: __E_q_vars_143(293)  => __E_q_vars_143(293) __E_q_vars_142(292).
	p(NT(293), (NT(293)+NT(292)));
//G464: q_vars(121)          => q_var(28) __E_q_vars_143(293).
	p(NT(121), (NT(28)+NT(293)));
//G465: q_var(28)            => capture(17).
	p(NT(28), (NT(17)));
//G466: __N_29(526)          => uconst(30).
	p(NT(526), (NT(30)));
//G467: q_var(28)            => ~( __N_29(526) ) & variable(186).	 # conjunctive
	p(NT(28), ~(NT(526)) & (NT(186)));
//G468: ctnvar(22)           => chars(27).
	p(NT(22), (NT(27)));
//G469: __E_offsets_144(294) => _(9) ',' _(9) offset(289).
	p(NT(294), (NT(9)+T(19)+NT(9)+NT(289)));
//G470: __E_offsets_145(295) => null.
	p(NT(295), (nul));
//G471: __E_offsets_145(295) => __E_offsets_145(295) __E_offsets_144(294).
	p(NT(295), (NT(295)+NT(294)));
//G472: offsets(92)          => '[' _(9) offset(289) __E_offsets_145(295) _(9) ']'.
	p(NT(92), (T(20)+NT(9)+NT(289)+NT(295)+NT(9)+T(21)));
//G473: offset(289)          => integer(15).
	p(NT(289), (NT(15)));
//G474: offset(289)          => capture(17).
	p(NT(289), (NT(17)));
//G475: offset(289)          => shift(296).
	p(NT(289), (NT(296)));
//G476: __N_30(527)          => io_var(281).
	p(NT(527), (NT(281)));
//G477: __E_offset_146(297)  => variable(186) & ~( __N_30(527) ).	 # conjunctive
	p(NT(297), (NT(186)) & ~(NT(527)));
//G478: offset(289)          => __E_offset_146(297).
	p(NT(289), (NT(297)));
//G479: __E_shift_147(298)   => capture(17).
	p(NT(298), (NT(17)));
//G480: __N_31(528)          => io_var(281).
	p(NT(528), (NT(281)));
//G481: __E___E_shift_147_148(299) => variable(186) & ~( __N_31(528) ).	 # conjunctive
	p(NT(299), (NT(186)) & ~(NT(528)));
//G482: __E_shift_147(298)   => __E___E_shift_147_148(299).
	p(NT(298), (NT(299)));
//G483: shift(296)           => __E_shift_147(298) _(9) '-' _(9) num(196).
	p(NT(296), (NT(298)+NT(9)+T(36)+NT(9)+NT(196)));
//G484: __E_chars_149(300)   => alnum(6).
	p(NT(300), (NT(6)));
//G485: __E_chars_149(300)   => '_'.
	p(NT(300), (T(47)));
//G486: __E_chars_150(301)   => null.
	p(NT(301), (nul));
//G487: __E_chars_150(301)   => __E_chars_150(301) __E_chars_149(300).
	p(NT(301), (NT(301)+NT(300)));
//G488: chars(27)            => alpha(5) __E_chars_150(301).
	p(NT(27), (NT(5)+NT(301)));
//G489: __E_digits_151(302)  => digit(3).
	p(NT(302), (NT(3)));
//G490: __E_digits_151(302)  => __E_digits_151(302) digit(3).
	p(NT(302), (NT(302)+NT(3)));
//G491: digits(12)           => __E_digits_151(302).
	p(NT(12), (NT(302)));
//G492: num(196)             => digits(12).
	p(NT(196), (NT(12)));
//G493: __E_integer_152(303) => '-'.
	p(NT(303), (T(36)));
//G494: __E_integer_152(303) => null.
	p(NT(303), (nul));
//G495: integer(15)          => __E_integer_152(303) _(9) digits(12).
	p(NT(15), (NT(303)+NT(9)+NT(12)));
//G496: sym(11)              => chars(27).
	p(NT(11), (NT(27)));
//G497: capture(17)          => '$' chars(27).
	p(NT(17), (T(58)+NT(27)));
//G498: typed(70)            => _(9) ':' _(9) type(13) _(9) '[' _(9) subtype(14) _(9) ']'.
	p(NT(70), (NT(9)+T(2)+NT(9)+NT(13)+NT(9)+T(20)+NT(9)+NT(14)+NT(9)+T(21)));
//G499: __N_32(529)          => 'b' 'v'.
	p(NT(529), (T(25)+T(50)));
//G500: __E_typed_153(304)   => ~( __N_32(529) ) & type(13).	 # conjunctive
	p(NT(304), ~(NT(529)) & (NT(13)));
//G501: typed(70)            => _(9) ':' _(9) __E_typed_153(304).
	p(NT(70), (NT(9)+T(2)+NT(9)+NT(304)));
//G502: type(13)             => chars(27).
	p(NT(13), (NT(27)));
//G503: subtype(14)          => num(196).
	p(NT(14), (NT(196)));
//G504: __E_comment_154(306) => printable(8).
	p(NT(306), (NT(8)));
//G505: __E_comment_154(306) => '\t'.
	p(NT(306), (T(59)));
//G506: __E_comment_155(307) => null.
	p(NT(307), (nul));
//G507: __E_comment_155(307) => __E_comment_155(307) __E_comment_154(306).
	p(NT(307), (NT(307)+NT(306)));
//G508: __E_comment_156(308) => '\n'.
	p(NT(308), (T(60)));
//G509: __E_comment_156(308) => '\r'.
	p(NT(308), (T(61)));
//G510: __E_comment_156(308) => eof(1).
	p(NT(308), (NT(1)));
//G511: comment(305)         => '#' __E_comment_155(307) __E_comment_156(308).
	p(NT(305), (T(62)+NT(307)+NT(308)));
//G512: __(10)               => space(2).
	p(NT(10), (NT(2)));
//G513: __(10)               => comment(305).
	p(NT(10), (NT(305)));
//G514: __(10)               => __(10) space(2).
	p(NT(10), (NT(10)+NT(2)));
//G515: __(10)               => __(10) comment(305).
	p(NT(10), (NT(10)+NT(305)));
//G516: _(9)                 => __(10).
	p(NT(9), (NT(10)));
//G517: _(9)                 => null.
	p(NT(9), (nul));
//G518: cli(309)             => _(9).
	p(NT(309), (NT(9)));
//G519: __E_cli_157(311)     => '.' _(9) cli_command(310) _(9).
	p(NT(311), (T(1)+NT(9)+NT(310)+NT(9)));
//G520: __E_cli_158(312)     => null.
	p(NT(312), (nul));
//G521: __E_cli_158(312)     => __E_cli_158(312) __E_cli_157(311).
	p(NT(312), (NT(312)+NT(311)));
//G522: __E_cli_159(313)     => '.' _(9).
	p(NT(313), (T(1)+NT(9)));
//G523: __E_cli_159(313)     => null.
	p(NT(313), (nul));
//G524: cli(309)             => _(9) cli_command(310) _(9) __E_cli_158(312) __E_cli_159(313).
	p(NT(309), (NT(9)+NT(310)+NT(9)+NT(312)+NT(313)));
//G525: __E_cli_command_160(316) => 'q'.
	p(NT(316), (T(63)));
//G526: __E_cli_command_160(316) => 'q' 'u' 'i' 't'.
	p(NT(316), (T(63)+T(7)+T(4)+T(8)));
//G527: quit_sym(315)        => __E_cli_command_160(316).
	p(NT(315), (NT(316)));
//G528: quit_cmd(314)        => quit_sym(315).
	p(NT(314), (NT(315)));
//G529: cli_command(310)     => quit_cmd(314).
	p(NT(310), (NT(314)));
//G530: __E_cli_command_161(319) => 'v'.
	p(NT(319), (T(50)));
//G531: __E_cli_command_161(319) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(319), (T(50)+T(11)+T(27)+T(15)+T(4)+T(6)+T(5)));
//G532: version_sym(318)     => __E_cli_command_161(319).
	p(NT(318), (NT(319)));
//G533: version_cmd(317)     => version_sym(318).
	p(NT(317), (NT(318)));
//G534: cli_command(310)     => version_cmd(317).
	p(NT(310), (NT(317)));
//G535: __E_cli_command_162(322) => 'c'.
	p(NT(322), (T(14)));
//G536: __E_cli_command_162(322) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(322), (T(14)+T(10)+T(11)+T(24)+T(27)));
//G537: clear_sym(321)       => __E_cli_command_162(322).
	p(NT(321), (NT(322)));
//G538: clear_cmd(320)       => clear_sym(321).
	p(NT(320), (NT(321)));
//G539: cli_command(310)     => clear_cmd(320).
	p(NT(310), (NT(320)));
//G540: __E___E_cli_command_163_164(326) => 'h'.
	p(NT(326), (T(64)));
//G541: __E___E_cli_command_163_164(326) => 'h' 'e' 'l' 'p'.
	p(NT(326), (T(64)+T(11)+T(10)+T(18)));
//G542: help_sym(325)        => __E___E_cli_command_163_164(326).
	p(NT(325), (NT(326)));
//G543: __E___E_cli_command_163_165(327) => __(10) help_arg(328).
	p(NT(327), (NT(10)+NT(328)));
//G544: __E___E_cli_command_163_165(327) => null.
	p(NT(327), (nul));
//G545: __E_cli_command_163(324) => help_sym(325) __E___E_cli_command_163_165(327).
	p(NT(324), (NT(325)+NT(327)));
//G546: help_cmd(323)        => __E_cli_command_163(324).
	p(NT(323), (NT(324)));
//G547: cli_command(310)     => help_cmd(323).
	p(NT(310), (NT(323)));
//G548: file_sym(331)        => 'f' 'i' 'l' 'e'.
	p(NT(331), (T(9)+T(4)+T(10)+T(11)));
//G549: __E_cli_command_166(330) => file_sym(331) __(10) q_string(332).
	p(NT(330), (NT(331)+NT(10)+NT(332)));
//G550: file_cmd(329)        => __E_cli_command_166(330).
	p(NT(329), (NT(330)));
//G551: cli_command(310)     => file_cmd(329).
	p(NT(310), (NT(329)));
//G552: valid_sym(335)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(335), (T(50)+T(24)+T(10)+T(4)+T(57)));
//G553: __E_cli_command_167(334) => valid_sym(335) __(10) spec_arg(37).
	p(NT(334), (NT(335)+NT(10)+NT(37)));
//G554: valid_cmd(333)       => __E_cli_command_167(334).
	p(NT(333), (NT(334)));
//G555: cli_command(310)     => valid_cmd(333).
	p(NT(310), (NT(333)));
//G556: sat_sym(338)         => 's' 'a' 't'.
	p(NT(338), (T(15)+T(24)+T(8)));
//G557: __E_cli_command_168(337) => sat_sym(338) __(10) spec_arg(37).
	p(NT(337), (NT(338)+NT(10)+NT(37)));
//G558: sat_cmd(336)         => __E_cli_command_168(337).
	p(NT(336), (NT(337)));
//G559: cli_command(310)     => sat_cmd(336).
	p(NT(310), (NT(336)));
//G560: unsat_sym(341)       => 'u' 'n' 's' 'a' 't'.
	p(NT(341), (T(7)+T(5)+T(15)+T(24)+T(8)));
//G561: __E_cli_command_169(340) => unsat_sym(341) __(10) spec_arg(37).
	p(NT(340), (NT(341)+NT(10)+NT(37)));
//G562: unsat_cmd(339)       => __E_cli_command_169(340).
	p(NT(339), (NT(340)));
//G563: cli_command(310)     => unsat_cmd(339).
	p(NT(310), (NT(339)));
//G564: realizable_sym(344)  => 'r' 'e' 'a' 'l' 'i' 'z' 'a' 'b' 'l' 'e'.
	p(NT(344), (T(27)+T(11)+T(24)+T(10)+T(4)+T(65)+T(24)+T(25)+T(10)+T(11)));
//G565: __E_cli_command_170(343) => realizable_sym(344) __(10) spec_arg(37).
	p(NT(343), (NT(344)+NT(10)+NT(37)));
//G566: realizable_cmd(342)  => __E_cli_command_170(343).
	p(NT(342), (NT(343)));
//G567: cli_command(310)     => realizable_cmd(342).
	p(NT(310), (NT(342)));
//G568: unrealizable_sym(347) => 'u' 'n' 'r' 'e' 'a' 'l' 'i' 'z' 'a' 'b' 'l' 'e'.
	p(NT(347), (T(7)+T(5)+T(27)+T(11)+T(24)+T(10)+T(4)+T(65)+T(24)+T(25)+T(10)+T(11)));
//G569: __E_cli_command_171(346) => unrealizable_sym(347) __(10) spec_arg(37).
	p(NT(346), (NT(347)+NT(10)+NT(37)));
//G570: unrealizable_cmd(345) => __E_cli_command_171(346).
	p(NT(345), (NT(346)));
//G571: cli_command(310)     => unrealizable_cmd(345).
	p(NT(310), (NT(345)));
//G572: solve_sym(350)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(350), (T(15)+T(6)+T(10)+T(50)+T(11)));
//G573: __E___E_cli_command_172_173(351) => solve_options(352).
	p(NT(351), (NT(352)));
//G574: __E___E_cli_command_172_173(351) => null.
	p(NT(351), (nul));
//G575: __E_cli_command_172(349) => solve_sym(350) __E___E_cli_command_172_173(351) __(10) wff_cmd_arg(34).
	p(NT(349), (NT(350)+NT(351)+NT(10)+NT(34)));
//G576: solve_cmd(348)       => __E_cli_command_172(349).
	p(NT(348), (NT(349)));
//G577: cli_command(310)     => solve_cmd(348).
	p(NT(310), (NT(348)));
//G578: lgrs_sym(355)        => 'l' 'g' 'r' 's'.
	p(NT(355), (T(10)+T(49)+T(27)+T(15)));
//G579: __E_cli_command_174(354) => lgrs_sym(355) __(10) wff_cmd_arg(34).
	p(NT(354), (NT(355)+NT(10)+NT(34)));
//G580: lgrs_cmd(353)        => __E_cli_command_174(354).
	p(NT(353), (NT(354)));
//G581: cli_command(310)     => lgrs_cmd(353).
	p(NT(310), (NT(353)));
//G582: __E___E_cli_command_175_176(359) => 'r'.
	p(NT(359), (T(27)));
//G583: __E___E_cli_command_175_176(359) => 'r' 'u' 'n'.
	p(NT(359), (T(27)+T(7)+T(5)));
//G584: run_sym(358)         => __E___E_cli_command_175_176(359).
	p(NT(358), (NT(359)));
//G585: __E___E___E_cli_command_175_177_178(361) => 's' 't' 'e' 'p' 's'.
	p(NT(361), (T(15)+T(8)+T(11)+T(18)+T(15)));
//G586: __E___E___E_cli_command_175_177_178(361) => 's' 't' 'e' 'p'.
	p(NT(361), (T(15)+T(8)+T(11)+T(18)));
//G587: __E___E_cli_command_175_177(360) => __(10) num(196) __(10) __E___E___E_cli_command_175_177_178(361).
	p(NT(360), (NT(10)+NT(196)+NT(10)+NT(361)));
//G588: __E___E_cli_command_175_177(360) => null.
	p(NT(360), (nul));
//G589: __E___E_cli_command_175_179(362) => __(10) spec_arg(37).
	p(NT(362), (NT(10)+NT(37)));
//G590: __E___E_cli_command_175_179(362) => null.
	p(NT(362), (nul));
//G591: __E_cli_command_175(357) => run_sym(358) __E___E_cli_command_175_177(360) __E___E_cli_command_175_179(362).
	p(NT(357), (NT(358)+NT(360)+NT(362)));
//G592: run_cmd(356)         => __E_cli_command_175(357).
	p(NT(356), (NT(357)));
//G593: cli_command(310)     => run_cmd(356).
	p(NT(310), (NT(356)));
//G594: stop_sym(365)        => 's' 't' 'o' 'p'.
	p(NT(365), (T(15)+T(8)+T(6)+T(18)));
//G595: __E_cli_command_180(364) => stop_sym(365).
	p(NT(364), (NT(365)));
//G596: stop_cmd(363)        => __E_cli_command_180(364).
	p(NT(363), (NT(364)));
//G597: cli_command(310)     => stop_cmd(363).
	p(NT(310), (NT(363)));
//G598: memory_sym(368)      => 'm' 'e' 'm' 'o' 'r' 'y'.
	p(NT(368), (T(28)+T(11)+T(28)+T(6)+T(27)+T(17)));
//G599: __E_cli_command_181(367) => memory_sym(368).
	p(NT(367), (NT(368)));
//G600: memory_cmd(366)      => __E_cli_command_181(367).
	p(NT(366), (NT(367)));
//G601: cli_command(310)     => memory_cmd(366).
	p(NT(310), (NT(366)));
//G602: ltl_sym(371)         => 'l' 't' 'l'.
	p(NT(371), (T(10)+T(8)+T(10)));
//G603: __E_cli_command_182(370) => ltl_sym(371) __(10) spec_arg(37).
	p(NT(370), (NT(371)+NT(10)+NT(37)));
//G604: ltl_cmd(369)         => __E_cli_command_182(370).
	p(NT(369), (NT(370)));
//G605: cli_command(310)     => ltl_cmd(369).
	p(NT(310), (NT(369)));
//G606: func_sym_cmd(374)    => 'f' 'u' 'n' 'c'.
	p(NT(374), (T(9)+T(7)+T(5)+T(14)));
//G607: __E_cli_command_183(373) => func_sym_cmd(374) __(10) func_decl(258).
	p(NT(373), (NT(374)+NT(10)+NT(258)));
//G608: func_cmd(372)        => __E_cli_command_183(373).
	p(NT(372), (NT(373)));
//G609: cli_command(310)     => func_cmd(372).
	p(NT(310), (NT(372)));
//G610: __E___E_cli_command_184_185(378) => 'n'.
	p(NT(378), (T(5)));
//G611: __E___E_cli_command_184_185(378) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(378), (T(5)+T(6)+T(27)+T(28)+T(24)+T(10)+T(4)+T(65)+T(11)));
//G612: normalize_sym(377)   => __E___E_cli_command_184_185(378).
	p(NT(377), (NT(378)));
//G613: __E_cli_command_184(376) => normalize_sym(377) __(10) spec_arg(37).
	p(NT(376), (NT(377)+NT(10)+NT(37)));
//G614: normalize_cmd(375)   => __E_cli_command_184(376).
	p(NT(375), (NT(376)));
//G615: cli_command(310)     => normalize_cmd(375).
	p(NT(310), (NT(375)));
//G616: __E___E_cli_command_186_187(382) => 's'.
	p(NT(382), (T(15)));
//G617: __E___E_cli_command_186_187(382) => 's' 'u' 'b' 's' 't'.
	p(NT(382), (T(15)+T(7)+T(25)+T(15)+T(8)));
//G618: __E___E_cli_command_186_187(382) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(382), (T(15)+T(7)+T(25)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G619: subst_sym(381)       => __E___E_cli_command_186_187(382).
	p(NT(381), (NT(382)));
//G620: __E___E_cli_command_186_188(383) => _(9) subst_group(384).
	p(NT(383), (NT(9)+NT(384)));
//G621: __E___E_cli_command_186_189(385) => __E___E_cli_command_186_188(383).
	p(NT(385), (NT(383)));
//G622: __E___E_cli_command_186_189(385) => __E___E_cli_command_186_189(385) __E___E_cli_command_186_188(383).
	p(NT(385), (NT(385)+NT(383)));
//G623: __E_cli_command_186(380) => subst_sym(381) __(10) nf_cmd_arg(35) __E___E_cli_command_186_189(385).
	p(NT(380), (NT(381)+NT(10)+NT(35)+NT(385)));
//G624: subst_cmd(379)       => __E_cli_command_186(380).
	p(NT(379), (NT(380)));
//G625: cli_command(310)     => subst_cmd(379).
	p(NT(310), (NT(379)));
//G626: __E___E_cli_command_190_191(389) => 'i'.
	p(NT(389), (T(4)));
//G627: __E___E_cli_command_190_191(389) => 'i' 'n' 's' 't'.
	p(NT(389), (T(4)+T(5)+T(15)+T(8)));
//G628: __E___E_cli_command_190_191(389) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(389), (T(4)+T(5)+T(15)+T(8)+T(24)+T(5)+T(8)+T(4)+T(24)+T(8)+T(11)));
//G629: inst_sym(388)        => __E___E_cli_command_190_191(389).
	p(NT(388), (NT(389)));
//G630: __E_cli_command_190(387) => inst_sym(388) __(10) inst_args(32).
	p(NT(387), (NT(388)+NT(10)+NT(32)));
//G631: inst_cmd(386)        => __E_cli_command_190(387).
	p(NT(386), (NT(387)));
//G632: cli_command(310)     => inst_cmd(386).
	p(NT(310), (NT(386)));
//G633: dnf_sym(392)         => 'd' 'n' 'f'.
	p(NT(392), (T(57)+T(5)+T(9)));
//G634: __E_cli_command_192(391) => dnf_sym(392) __(10) nf_cmd_arg(35).
	p(NT(391), (NT(392)+NT(10)+NT(35)));
//G635: dnf_cmd(390)         => __E_cli_command_192(391).
	p(NT(390), (NT(391)));
//G636: cli_command(310)     => dnf_cmd(390).
	p(NT(310), (NT(390)));
//G637: cnf_sym(395)         => 'c' 'n' 'f'.
	p(NT(395), (T(14)+T(5)+T(9)));
//G638: __E_cli_command_193(394) => cnf_sym(395) __(10) nf_cmd_arg(35).
	p(NT(394), (NT(395)+NT(10)+NT(35)));
//G639: cnf_cmd(393)         => __E_cli_command_193(394).
	p(NT(393), (NT(394)));
//G640: cli_command(310)     => cnf_cmd(393).
	p(NT(310), (NT(393)));
//G641: nnf_sym(398)         => 'n' 'n' 'f'.
	p(NT(398), (T(5)+T(5)+T(9)));
//G642: __E_cli_command_194(397) => nnf_sym(398) __(10) nf_cmd_arg(35).
	p(NT(397), (NT(398)+NT(10)+NT(35)));
//G643: nnf_cmd(396)         => __E_cli_command_194(397).
	p(NT(396), (NT(397)));
//G644: cli_command(310)     => nnf_cmd(396).
	p(NT(310), (NT(396)));
//G645: mnf_sym(401)         => 'm' 'n' 'f'.
	p(NT(401), (T(28)+T(5)+T(9)));
//G646: __E_cli_command_195(400) => mnf_sym(401) __(10) nf_cmd_arg(35).
	p(NT(400), (NT(401)+NT(10)+NT(35)));
//G647: mnf_cmd(399)         => __E_cli_command_195(400).
	p(NT(399), (NT(400)));
//G648: cli_command(310)     => mnf_cmd(399).
	p(NT(310), (NT(399)));
//G649: onf_sym(404)         => 'o' 'n' 'f'.
	p(NT(404), (T(6)+T(5)+T(9)));
//G650: __E_cli_command_196(403) => onf_sym(404) __(10) variable(186) __(10) onf_cmd_arg(36).
	p(NT(403), (NT(404)+NT(10)+NT(186)+NT(10)+NT(36)));
//G651: onf_cmd(402)         => __E_cli_command_196(403).
	p(NT(402), (NT(403)));
//G652: cli_command(310)     => onf_cmd(402).
	p(NT(310), (NT(402)));
//G653: qelim_sym(407)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(407), (T(63)+T(11)+T(10)+T(4)+T(28)));
//G654: __E_cli_command_197(406) => qelim_sym(407) __(10) wff_cmd_arg(34).
	p(NT(406), (NT(407)+NT(10)+NT(34)));
//G655: qelim_cmd(405)       => __E_cli_command_197(406).
	p(NT(405), (NT(406)));
//G656: cli_command(310)     => qelim_cmd(405).
	p(NT(310), (NT(405)));
//G657: whatis_sym(410)      => 'w' 'h' 'a' 't' 'i' 's'.
	p(NT(410), (T(32)+T(64)+T(24)+T(8)+T(4)+T(15)));
//G658: __E_cli_command_198(409) => whatis_sym(410) __(10) spec_arg(37).
	p(NT(409), (NT(410)+NT(10)+NT(37)));
//G659: whatis_cmd(408)      => __E_cli_command_198(409).
	p(NT(408), (NT(409)));
//G660: cli_command(310)     => whatis_cmd(408).
	p(NT(310), (NT(408)));
//G661: reset_sym(413)       => 'r' 'e' 's' 'e' 't'.
	p(NT(413), (T(27)+T(11)+T(15)+T(11)+T(8)));
//G662: __E_cli_command_199(412) => reset_sym(413).
	p(NT(412), (NT(413)));
//G663: reset_cmd(411)       => __E_cli_command_199(412).
	p(NT(411), (NT(412)));
//G664: cli_command(310)     => reset_cmd(411).
	p(NT(310), (NT(411)));
//G665: fragment_sym(416)    => 'f' 'r' 'a' 'g' 'm' 'e' 'n' 't'.
	p(NT(416), (T(9)+T(27)+T(24)+T(49)+T(28)+T(11)+T(5)+T(8)));
//G666: __E_cli_command_200(415) => fragment_sym(416) __(10) fragment_name(417).
	p(NT(415), (NT(416)+NT(10)+NT(417)));
//G667: fragment_cmd(414)    => __E_cli_command_200(415).
	p(NT(414), (NT(415)));
//G668: cli_command(310)     => fragment_cmd(414).
	p(NT(310), (NT(414)));
//G669: get_sym(420)         => 'g' 'e' 't'.
	p(NT(420), (T(49)+T(11)+T(8)));
//G670: __E___E_cli_command_201_202(421) => __(10) option_name(23).
	p(NT(421), (NT(10)+NT(23)));
//G671: __E___E_cli_command_201_202(421) => null.
	p(NT(421), (nul));
//G672: __E_cli_command_201(419) => get_sym(420) __E___E_cli_command_201_202(421).
	p(NT(419), (NT(420)+NT(421)));
//G673: get_cmd(418)         => __E_cli_command_201(419).
	p(NT(418), (NT(419)));
//G674: cli_command(310)     => get_cmd(418).
	p(NT(310), (NT(418)));
//G675: set_sym(424)         => 's' 'e' 't'.
	p(NT(424), (T(15)+T(11)+T(8)));
//G676: __E___E_cli_command_203_204(425) => __(10).
	p(NT(425), (NT(10)));
//G677: __E___E_cli_command_203_204(425) => _(9) '=' _(9).
	p(NT(425), (NT(9)+T(3)+NT(9)));
//G678: __E_cli_command_203(423) => set_sym(424) __(10) option_name(23) __E___E_cli_command_203_204(425) option_value(24).
	p(NT(423), (NT(424)+NT(10)+NT(23)+NT(425)+NT(24)));
//G679: set_cmd(422)         => __E_cli_command_203(423).
	p(NT(422), (NT(423)));
//G680: cli_command(310)     => set_cmd(422).
	p(NT(310), (NT(422)));
//G681: enable_sym(428)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(428), (T(11)+T(5)+T(24)+T(25)+T(10)+T(11)));
//G682: __E_cli_command_205(427) => enable_sym(428) __(10) option_name(23).
	p(NT(427), (NT(428)+NT(10)+NT(23)));
//G683: enable_cmd(426)      => __E_cli_command_205(427).
	p(NT(426), (NT(427)));
//G684: cli_command(310)     => enable_cmd(426).
	p(NT(310), (NT(426)));
//G685: disable_sym(431)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(431), (T(57)+T(4)+T(15)+T(24)+T(25)+T(10)+T(11)));
//G686: __E_cli_command_206(430) => disable_sym(431) __(10) option_name(23).
	p(NT(430), (NT(431)+NT(10)+NT(23)));
//G687: disable_cmd(429)     => __E_cli_command_206(430).
	p(NT(429), (NT(430)));
//G688: cli_command(310)     => disable_cmd(429).
	p(NT(310), (NT(429)));
//G689: toggle_sym(434)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(434), (T(8)+T(6)+T(49)+T(49)+T(10)+T(11)));
//G690: __E_cli_command_207(433) => toggle_sym(434) __(10) option_name(23).
	p(NT(433), (NT(434)+NT(10)+NT(23)));
//G691: toggle_cmd(432)      => __E_cli_command_207(433).
	p(NT(432), (NT(433)));
//G692: cli_command(310)     => toggle_cmd(432).
	p(NT(310), (NT(432)));
//G693: __E___E_cli_command_208_209(438) => 'd' 'e' 'f' 's'.
	p(NT(438), (T(57)+T(11)+T(9)+T(15)));
//G694: __E___E_cli_command_208_209(438) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(438), (T(57)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G695: def_sym(437)         => __E___E_cli_command_208_209(438).
	p(NT(437), (NT(438)));
//G696: __E_cli_command_208(436) => def_sym(437).
	p(NT(436), (NT(437)));
//G697: def_list_cmd(435)    => __E_cli_command_208(436).
	p(NT(435), (NT(436)));
//G698: cli_command(310)     => def_list_cmd(435).
	p(NT(310), (NT(435)));
//G699: __E_cli_command_210(440) => def_sym(437) __(10) num(196).
	p(NT(440), (NT(437)+NT(10)+NT(196)));
//G700: def_print_cmd(439)   => __E_cli_command_210(440).
	p(NT(439), (NT(440)));
//G701: cli_command(310)     => def_print_cmd(439).
	p(NT(310), (NT(439)));
//G702: def_rr_cmd(441)      => rec_relation(56).
	p(NT(441), (NT(56)));
//G703: cli_command(310)     => def_rr_cmd(441).
	p(NT(310), (NT(441)));
//G704: def_input_cmd(442)   => input_def(57).
	p(NT(442), (NT(57)));
//G705: cli_command(310)     => def_input_cmd(442).
	p(NT(310), (NT(442)));
//G706: def_output_cmd(443)  => output_def(58).
	p(NT(443), (NT(58)));
//G707: cli_command(310)     => def_output_cmd(443).
	p(NT(310), (NT(443)));
//G708: def_type_cmd(444)    => type_def(59).
	p(NT(444), (NT(59)));
//G709: cli_command(310)     => def_type_cmd(444).
	p(NT(310), (NT(444)));
//G710: __E___E_cli_command_211_212(448) => 'h' 'i' 's' 't'.
	p(NT(448), (T(64)+T(4)+T(15)+T(8)));
//G711: __E___E_cli_command_211_212(448) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(448), (T(64)+T(4)+T(15)+T(8)+T(6)+T(27)+T(17)));
//G712: history_sym(447)     => __E___E_cli_command_211_212(448).
	p(NT(447), (NT(448)));
//G713: __E_cli_command_211(446) => history_sym(447).
	p(NT(446), (NT(447)));
//G714: history_list_cmd(445) => __E_cli_command_211(446).
	p(NT(445), (NT(446)));
//G715: cli_command(310)     => history_list_cmd(445).
	p(NT(310), (NT(445)));
//G716: __E_cli_command_213(450) => history_sym(447) __(10) history(451).
	p(NT(450), (NT(447)+NT(10)+NT(451)));
//G717: history_print_cmd(449) => __E_cli_command_213(450).
	p(NT(449), (NT(450)));
//G718: cli_command(310)     => history_print_cmd(449).
	p(NT(310), (NT(449)));
//G719: __E_cli_command_214(453) => ref(65).
	p(NT(453), (NT(65)));
//G720: __E_cli_command_214(453) => wff(38).
	p(NT(453), (NT(38)));
//G721: __E_cli_command_214(453) => bf(40).
	p(NT(453), (NT(40)));
//G722: history_store_cmd(452) => __E_cli_command_214(453).
	p(NT(452), (NT(453)));
//G723: cli_command(310)     => history_store_cmd(452).
	p(NT(310), (NT(452)));
//G724: solve_options(352)   => __(10) solve_option(454) solve_options(352).
	p(NT(352), (NT(10)+NT(454)+NT(352)));
//G725: solve_options(352)   => null.
	p(NT(352), (nul));
//G726: __E_solve_option_215(455) => solver_mode(456).
	p(NT(455), (NT(456)));
//G727: __E_solve_option_215(455) => type(13).
	p(NT(455), (NT(13)));
//G728: solve_option(454)    => '-' '-' __E_solve_option_215(455).
	p(NT(454), (T(36)+T(36)+NT(455)));
//G729: __E_solver_mode_216(458) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(458), (T(28)+T(4)+T(5)+T(4)+T(28)+T(7)+T(28)));
//G730: __E_solver_mode_216(458) => 'm' 'i' 'n'.
	p(NT(458), (T(28)+T(4)+T(5)));
//G731: solver_mode_minimum(457) => __E_solver_mode_216(458).
	p(NT(457), (NT(458)));
//G732: solver_mode(456)     => solver_mode_minimum(457).
	p(NT(456), (NT(457)));
//G733: __E_solver_mode_217(460) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(460), (T(28)+T(24)+T(38)+T(4)+T(28)+T(7)+T(28)));
//G734: __E_solver_mode_217(460) => 'm' 'a' 'x'.
	p(NT(460), (T(28)+T(24)+T(38)));
//G735: solver_mode_maximum(459) => __E_solver_mode_217(460).
	p(NT(459), (NT(460)));
//G736: solver_mode(456)     => solver_mode_maximum(459).
	p(NT(456), (NT(459)));
//G737: q_file_name(74)      => '"' file_name(21) '"'.
	p(NT(74), (T(66)+NT(21)+T(66)));
//G738: __E_file_name_218(461) => printable(8).
	p(NT(461), (NT(8)));
//G739: __E_file_name_218(461) => __E_file_name_218(461) printable(8).
	p(NT(461), (NT(461)+NT(8)));
//G740: file_name(21)        => __E_file_name_218(461).
	p(NT(21), (NT(461)));
//G741: __E_option_name_219(462) => alnum(6).
	p(NT(462), (NT(6)));
//G742: __E_option_name_219(462) => __E_option_name_219(462) alnum(6).
	p(NT(462), (NT(462)+NT(6)));
//G743: option_name(23)      => __E_option_name_219(462).
	p(NT(23), (NT(462)));
//G744: __E_option_value_220(463) => alnum(6).
	p(NT(463), (NT(6)));
//G745: __E_option_value_220(463) => '.'.
	p(NT(463), (T(1)));
//G746: __E_option_value_221(464) => __E_option_value_220(463).
	p(NT(464), (NT(463)));
//G747: __E_option_value_221(464) => __E_option_value_221(464) __E_option_value_220(463).
	p(NT(464), (NT(464)+NT(463)));
//G748: option_value(24)     => __E_option_value_221(464).
	p(NT(24), (NT(464)));
//G749: fragment_ltl(466)    => 'l' 't' 'l'.
	p(NT(466), (T(10)+T(8)+T(10)));
//G750: __E_fragment_name_222(465) => fragment_ltl(466).
	p(NT(465), (NT(466)));
//G751: fragment_ctl_star(467) => 'c' 't' 'l' '_' 's' 't' 'a' 'r'.
	p(NT(467), (T(14)+T(8)+T(10)+T(47)+T(15)+T(8)+T(24)+T(27)));
//G752: __E_fragment_name_222(465) => fragment_ctl_star(467).
	p(NT(465), (NT(467)));
//G753: fragment_ctl_star(467) => 'c' 't' 'l' '*'.
	p(NT(467), (T(14)+T(8)+T(10)+T(53)));
//G754: __E_fragment_name_222(465) => fragment_ctl_star(467).
	p(NT(465), (NT(467)));
//G755: fragment_name(417)   => __E_fragment_name_222(465).
	p(NT(417), (NT(465)));
//G756: bf_cmd_arg(33)       => history(451).
	p(NT(33), (NT(451)));
//G757: bf_cmd_arg(33)       => bf(40).
	p(NT(33), (NT(40)));
//G758: wff_cmd_arg(34)      => history(451).
	p(NT(34), (NT(451)));
//G759: wff_cmd_arg(34)      => wff(38).
	p(NT(34), (NT(38)));
//G760: nf_cmd_arg(35)       => history(451).
	p(NT(35), (NT(451)));
//G761: __N_33(530)          => bf_min(126).
	p(NT(530), (NT(126)));
//G762: __N_34(531)          => bf_max(127).
	p(NT(531), (NT(127)));
//G763: __E_nf_cmd_arg_223(468) => ref(65) & ~( __N_33(530) ) & ~( __N_34(531) ).	 # conjunctive
	p(NT(468), (NT(65)) & ~(NT(530)) & ~(NT(531)));
//G764: nf_cmd_arg(35)       => __E_nf_cmd_arg_223(468).
	p(NT(35), (NT(468)));
//G765: nf_cmd_arg(35)       => wff(38).
	p(NT(35), (NT(38)));
//G766: nf_cmd_arg(35)       => bf(40).
	p(NT(35), (NT(40)));
//G767: onf_cmd_arg(36)      => history(451).
	p(NT(36), (NT(451)));
//G768: onf_cmd_arg(36)      => wff(38).
	p(NT(36), (NT(38)));
//G769: spec_arg(37)         => history(451).
	p(NT(37), (NT(451)));
//G770: spec_arg(37)         => spec(49).
	p(NT(37), (NT(49)));
//G771: __N_35(532)          => bf_min(126).
	p(NT(532), (NT(126)));
//G772: __N_36(533)          => bf_max(127).
	p(NT(533), (NT(127)));
//G773: __E_spec_arg_224(469) => ref(65) & ~( __N_35(532) ) & ~( __N_36(533) ).	 # conjunctive
	p(NT(469), (NT(65)) & ~(NT(532)) & ~(NT(533)));
//G774: spec_arg(37)         => __E_spec_arg_224(469).
	p(NT(37), (NT(469)));
//G775: spec_arg(37)         => wff(38).
	p(NT(37), (NT(38)));
//G776: spec_arg(37)         => bf(40).
	p(NT(37), (NT(40)));
//G777: __E_subst_group_225(470) => _(9) ',' _(9) nf_cmd_arg(35) _(9) '/' _(9) nf_cmd_arg(35).
	p(NT(470), (NT(9)+T(19)+NT(9)+NT(35)+NT(9)+T(16)+NT(9)+NT(35)));
//G778: __E_subst_group_226(471) => null.
	p(NT(471), (nul));
//G779: __E_subst_group_226(471) => __E_subst_group_226(471) __E_subst_group_225(470).
	p(NT(471), (NT(471)+NT(470)));
//G780: subst_group(384)     => '[' _(9) nf_cmd_arg(35) _(9) '/' _(9) nf_cmd_arg(35) __E_subst_group_226(471) _(9) ']'.
	p(NT(384), (T(20)+NT(9)+NT(35)+NT(9)+T(16)+NT(9)+NT(35)+NT(471)+NT(9)+T(21)));
//G781: __E_inst_args_227(472) => _(9) inst_group(473).
	p(NT(472), (NT(9)+NT(473)));
//G782: __E_inst_args_228(474) => __E_inst_args_227(472).
	p(NT(474), (NT(472)));
//G783: __E_inst_args_228(474) => __E_inst_args_228(474) __E_inst_args_227(472).
	p(NT(474), (NT(474)+NT(472)));
//G784: inst_args(32)        => wff_cmd_arg(34) __E_inst_args_228(474).
	p(NT(32), (NT(34)+NT(474)));
//G785: __E_inst_args_229(475) => _(9) inst_group(473).
	p(NT(475), (NT(9)+NT(473)));
//G786: __E_inst_args_230(476) => __E_inst_args_229(475).
	p(NT(476), (NT(475)));
//G787: __E_inst_args_230(476) => __E_inst_args_230(476) __E_inst_args_229(475).
	p(NT(476), (NT(476)+NT(475)));
//G788: inst_args(32)        => bf_cmd_arg(33) __E_inst_args_230(476).
	p(NT(32), (NT(33)+NT(476)));
//G789: __E_inst_group_231(477) => _(9) ',' _(9) bf(40) _(9) '/' _(9) bf_cmd_arg(33).
	p(NT(477), (NT(9)+T(19)+NT(9)+NT(40)+NT(9)+T(16)+NT(9)+NT(33)));
//G790: __E_inst_group_232(478) => null.
	p(NT(478), (nul));
//G791: __E_inst_group_232(478) => __E_inst_group_232(478) __E_inst_group_231(477).
	p(NT(478), (NT(478)+NT(477)));
//G792: inst_group(473)      => '[' _(9) bf(40) _(9) '/' _(9) bf_cmd_arg(33) __E_inst_group_232(478) _(9) ']'.
	p(NT(473), (T(20)+NT(9)+NT(40)+NT(9)+T(16)+NT(9)+NT(33)+NT(478)+NT(9)+T(21)));
//G793: help_arg(328)        => help_sym(325).
	p(NT(328), (NT(325)));
//G794: help_arg(328)        => version_sym(318).
	p(NT(328), (NT(318)));
//G795: help_arg(328)        => quit_sym(315).
	p(NT(328), (NT(315)));
//G796: help_arg(328)        => clear_sym(321).
	p(NT(328), (NT(321)));
//G797: help_arg(328)        => get_sym(420).
	p(NT(328), (NT(420)));
//G798: help_arg(328)        => set_sym(424).
	p(NT(328), (NT(424)));
//G799: help_arg(328)        => enable_sym(428).
	p(NT(328), (NT(428)));
//G800: help_arg(328)        => disable_sym(431).
	p(NT(328), (NT(431)));
//G801: help_arg(328)        => toggle_sym(434).
	p(NT(328), (NT(434)));
//G802: help_arg(328)        => file_sym(331).
	p(NT(328), (NT(331)));
//G803: help_arg(328)        => history_sym(447).
	p(NT(328), (NT(447)));
//G804: help_arg(328)        => abs_history_sym(479).
	p(NT(328), (NT(479)));
//G805: help_arg(328)        => rel_history_sym(480).
	p(NT(328), (NT(480)));
//G806: help_arg(328)        => selection_sym(481).
	p(NT(328), (NT(481)));
//G807: help_arg(328)        => def_sym(437).
	p(NT(328), (NT(437)));
//G808: help_arg(328)        => inst_sym(388).
	p(NT(328), (NT(388)));
//G809: help_arg(328)        => subst_sym(381).
	p(NT(328), (NT(381)));
//G810: help_arg(328)        => normalize_sym(377).
	p(NT(328), (NT(377)));
//G811: help_arg(328)        => execute_sym(482).
	p(NT(328), (NT(482)));
//G812: help_arg(328)        => solve_sym(350).
	p(NT(328), (NT(350)));
//G813: help_arg(328)        => lgrs_sym(355).
	p(NT(328), (NT(355)));
//G814: help_arg(328)        => valid_sym(335).
	p(NT(328), (NT(335)));
//G815: help_arg(328)        => sat_sym(338).
	p(NT(328), (NT(338)));
//G816: help_arg(328)        => unsat_sym(341).
	p(NT(328), (NT(341)));
//G817: help_arg(328)        => realizable_sym(344).
	p(NT(328), (NT(344)));
//G818: help_arg(328)        => unrealizable_sym(347).
	p(NT(328), (NT(347)));
//G819: help_arg(328)        => run_sym(358).
	p(NT(328), (NT(358)));
//G820: help_arg(328)        => ltl_sym(371).
	p(NT(328), (NT(371)));
//G821: help_arg(328)        => stop_sym(365).
	p(NT(328), (NT(365)));
//G822: help_arg(328)        => memory_sym(368).
	p(NT(328), (NT(368)));
//G823: help_arg(328)        => dnf_sym(392).
	p(NT(328), (NT(392)));
//G824: help_arg(328)        => cnf_sym(395).
	p(NT(328), (NT(395)));
//G825: help_arg(328)        => snf_sym(483).
	p(NT(328), (NT(483)));
//G826: help_arg(328)        => nnf_sym(398).
	p(NT(328), (NT(398)));
//G827: help_arg(328)        => mnf_sym(401).
	p(NT(328), (NT(401)));
//G828: help_arg(328)        => onf_sym(404).
	p(NT(328), (NT(404)));
//G829: help_arg(328)        => qelim_sym(407).
	p(NT(328), (NT(407)));
//G830: help_arg(328)        => whatis_sym(410).
	p(NT(328), (NT(410)));
//G831: help_arg(328)        => reset_sym(413).
	p(NT(328), (NT(413)));
//G832: help_arg(328)        => fragment_sym(416).
	p(NT(328), (NT(416)));
//G833: __E___E_help_arg_233_234(486) => 's'.
	p(NT(486), (T(15)));
//G834: __E___E_help_arg_233_234(486) => null.
	p(NT(486), (nul));
//G835: __E_help_arg_233(485) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_233_234(486).
	p(NT(485), (T(11)+T(38)+T(24)+T(28)+T(18)+T(10)+T(11)+NT(486)));
//G836: examples_sym(484)    => __E_help_arg_233(485).
	p(NT(484), (NT(485)));
//G837: help_arg(328)        => examples_sym(484).
	p(NT(328), (NT(484)));
//G838: __E_history_235(488) => '%'.
	p(NT(488), (T(52)));
//G839: rel_history_sym(480) => '%' '-'.
	p(NT(480), (T(52)+T(36)));
//G840: history_id(490)      => digits(12).
	p(NT(490), (NT(12)));
//G841: __E___E_history_235_236(489) => history_id(490).
	p(NT(489), (NT(490)));
//G842: __E___E_history_235_236(489) => null.
	p(NT(489), (nul));
//G843: __E_history_235(488) => rel_history_sym(480) __E___E_history_235_236(489).
	p(NT(488), (NT(480)+NT(489)));
//G844: rel_history(487)     => __E_history_235(488).
	p(NT(487), (NT(488)));
//G845: history(451)         => rel_history(487).
	p(NT(451), (NT(487)));
//G846: abs_history_sym(479) => '%'.
	p(NT(479), (T(52)));
//G847: __E_history_237(492) => abs_history_sym(479) history_id(490).
	p(NT(492), (NT(479)+NT(490)));
//G848: abs_history(491)     => __E_history_237(492).
	p(NT(491), (NT(492)));
//G849: history(451)         => abs_history(491).
	p(NT(451), (NT(491)));
//G850: unreachable(493)     => BDD_ID(494).
	p(NT(493), (NT(494)));
//G851: stream_def(495)      => input_def(57).
	p(NT(495), (NT(57)));
//G852: stream_def(495)      => output_def(58).
	p(NT(495), (NT(58)));
//G853: fm_or_term(496)      => wff(38).
	p(NT(496), (NT(38)));
//G854: fm_or_term(496)      => bf(40).
	p(NT(496), (NT(40)));
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
		__E___E_wff_34_35, wff_A, __E_wff_36, wff_E, __E_wff_37, wff_semantic_neg, __E_wff_38, wff_conditional, __E_wff_39, wff_all, 
		__E_wff_40, q_vars, wff_ex, __E_wff_41, wff_ref, __E_wff_42, bf_min, bf_max, constraint, wff_imply, 
		__E_wff_43, wff_rimply, __E_wff_44, wff_equiv, __E_wff_45, wff_or, __E_wff_46, wff_xor, __E_wff_47, wff_and, 
		__E_wff_48, wff_neg, __E_wff_49, wff_t, wff_f, bf_interval, __E_wff_50, bf_eq, __E_wff_51, bf_neq, 
		__E_wff_52, bf_lt, __E_wff_53, bf_nlt, __E_wff_54, bf_lteq, __E_wff_55, bf_nlteq, __E_wff_56, bf_gt, 
		__E_wff_57, bf_ngt, __E_wff_58, bf_gteq, __E_wff_59, bf_ngteq, __E_wff_60, wff_U, __E_wff_61, __E___E_wff_61_62, 
		wff_R, __E_wff_63, __E___E_wff_63_64, wff_W, __E_wff_65, __E___E_wff_65_66, wff_S, __E_wff_67, __E___E_wff_67_68, wff_T, 
		__E_wff_69, __E___E_wff_69_70, __E_bf_71, __E___E_bf_71_72, bv_type_like, ba_constant, variable, bf_fall, __E_bf_73, bf_fex, 
		__E_bf_74, __E_bf_75, __E_bf_76, bf_ref, bf_cast, __E_bf_77, num, __E___E_bf_77_78, bf_t, bf_f, 
		bf_neg, bf_func_app, __E_bf_79, func_sym, func_args, bf_shr, __E_bf_80, __E___E_bf_80_81, bf_shl, __E_bf_82, 
		__E___E_bf_82_83, bf_add, __E_bf_84, __E___E_bf_84_85, bf_sub, __E_bf_86, __E___E_bf_86_87, bf_mod, __E_bf_88, __E___E_bf_88_89, 
		bf_mul, bf_div, __E_bf_90, __E___E_bf_90_91, __E_bf_92, __E___E_bf_92_93, bf_nor, __E_bf_94, __E___E_bf_94_95, bf_xnor, 
		__E_bf_96, __E___E_bf_96_97, bf_nand, __E_bf_98, __E___E_bf_98_99, bf_or, __E_bf_100, bf_xor, __E_bf_101, __E_bf_102, 
		__E___E_bf_102_103, __E_bf_104, __E___E_bf_104_105, __E_bf_106, __E___E_bf_106_107, __E_bf_108, __E___E_bf_108_109, __E_bf_110, __E___E_bf_110_111, __E___E_bf_110_112, 
		__E_func_args_113, __E_func_args_114, func_mode, func_dynamic, func_static, func_sort_list, __E_func_sort_list_115, __E_func_sort_list_116, func_decl, ctn_neq, 
		__E_constraint_117, ctn_eq, __E_constraint_118, ctn_gteq, __E_constraint_119, ctn_gt, __E_constraint_120, ctn_lteq, __E_constraint_121, ctn_lt, 
		__E_constraint_122, __E_ba_constant_123, __E_ba_constant_124, __E_ba_constant_125, __E_ba_constant_126, __E_source_127, __E___E_source_127_128, __E___E_source_127_129, __E_src_c_130, __E_src_c_131, 
		__E_variable_132, io_var, __E_variable_133, __E_variable_134, __E_var_name_135, __E_var_name_136, __E_var_name_137, __E_var_name_138, __E_var_name_139, offset, 
		__E_uconst_140, __E___E_uconst_140_141, __E_q_vars_142, __E_q_vars_143, __E_offsets_144, __E_offsets_145, shift, __E_offset_146, __E_shift_147, __E___E_shift_147_148, 
		__E_chars_149, __E_chars_150, __E_digits_151, __E_integer_152, __E_typed_153, comment, __E_comment_154, __E_comment_155, __E_comment_156, cli, 
		cli_command, __E_cli_157, __E_cli_158, __E_cli_159, quit_cmd, quit_sym, __E_cli_command_160, version_cmd, version_sym, __E_cli_command_161, 
		clear_cmd, clear_sym, __E_cli_command_162, help_cmd, __E_cli_command_163, help_sym, __E___E_cli_command_163_164, __E___E_cli_command_163_165, help_arg, file_cmd, 
		__E_cli_command_166, file_sym, q_string, valid_cmd, __E_cli_command_167, valid_sym, sat_cmd, __E_cli_command_168, sat_sym, unsat_cmd, 
		__E_cli_command_169, unsat_sym, realizable_cmd, __E_cli_command_170, realizable_sym, unrealizable_cmd, __E_cli_command_171, unrealizable_sym, solve_cmd, __E_cli_command_172, 
		solve_sym, __E___E_cli_command_172_173, solve_options, lgrs_cmd, __E_cli_command_174, lgrs_sym, run_cmd, __E_cli_command_175, run_sym, __E___E_cli_command_175_176, 
		__E___E_cli_command_175_177, __E___E___E_cli_command_175_177_178, __E___E_cli_command_175_179, stop_cmd, __E_cli_command_180, stop_sym, memory_cmd, __E_cli_command_181, memory_sym, ltl_cmd, 
		__E_cli_command_182, ltl_sym, func_cmd, __E_cli_command_183, func_sym_cmd, normalize_cmd, __E_cli_command_184, normalize_sym, __E___E_cli_command_184_185, subst_cmd, 
		__E_cli_command_186, subst_sym, __E___E_cli_command_186_187, __E___E_cli_command_186_188, subst_group, __E___E_cli_command_186_189, inst_cmd, __E_cli_command_190, inst_sym, __E___E_cli_command_190_191, 
		dnf_cmd, __E_cli_command_192, dnf_sym, cnf_cmd, __E_cli_command_193, cnf_sym, nnf_cmd, __E_cli_command_194, nnf_sym, mnf_cmd, 
		__E_cli_command_195, mnf_sym, onf_cmd, __E_cli_command_196, onf_sym, qelim_cmd, __E_cli_command_197, qelim_sym, whatis_cmd, __E_cli_command_198, 
		whatis_sym, reset_cmd, __E_cli_command_199, reset_sym, fragment_cmd, __E_cli_command_200, fragment_sym, fragment_name, get_cmd, __E_cli_command_201, 
		get_sym, __E___E_cli_command_201_202, set_cmd, __E_cli_command_203, set_sym, __E___E_cli_command_203_204, enable_cmd, __E_cli_command_205, enable_sym, disable_cmd, 
		__E_cli_command_206, disable_sym, toggle_cmd, __E_cli_command_207, toggle_sym, def_list_cmd, __E_cli_command_208, def_sym, __E___E_cli_command_208_209, def_print_cmd, 
		__E_cli_command_210, def_rr_cmd, def_input_cmd, def_output_cmd, def_type_cmd, history_list_cmd, __E_cli_command_211, history_sym, __E___E_cli_command_211_212, history_print_cmd, 
		__E_cli_command_213, history, history_store_cmd, __E_cli_command_214, solve_option, __E_solve_option_215, solver_mode, solver_mode_minimum, __E_solver_mode_216, solver_mode_maximum, 
		__E_solver_mode_217, __E_file_name_218, __E_option_name_219, __E_option_value_220, __E_option_value_221, __E_fragment_name_222, fragment_ltl, fragment_ctl_star, __E_nf_cmd_arg_223, __E_spec_arg_224, 
		__E_subst_group_225, __E_subst_group_226, __E_inst_args_227, inst_group, __E_inst_args_228, __E_inst_args_229, __E_inst_args_230, __E_inst_group_231, __E_inst_group_232, abs_history_sym, 
		rel_history_sym, selection_sym, execute_sym, snf_sym, examples_sym, __E_help_arg_233, __E___E_help_arg_233_234, rel_history, __E_history_235, __E___E_history_235_236, 
		history_id, abs_history, __E_history_237, unreachable, BDD_ID, stream_def, fm_or_term, __N_0, __N_1, __N_2, 
		__N_3, __N_4, __N_5, __N_6, __N_7, __N_8, __N_9, __N_10, __N_11, __N_12, 
		__N_13, __N_14, __N_15, __N_16, __N_17, __N_18, __N_19, __N_20, __N_21, __N_22, 
		__N_23, __N_24, __N_25, __N_26, __N_27, __N_28, __N_29, __N_30, __N_31, __N_32, 
		__N_33, __N_34, __N_35, __N_36, 
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
