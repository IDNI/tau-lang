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
	"__E___E_wff_34_35", "wff_A", "__E_wff_36", "wff_E", "__E_wff_37", "wff_semantic_neg", "__E_wff_38", "wff_conditional", "__E_wff_39", "wff_all", 
	"__E_wff_40", "q_vars", "wff_ex", "__E_wff_41", "wff_ref", "constraint", "wff_imply", "__E_wff_42", "wff_rimply", "__E_wff_43", 
	"wff_equiv", "__E_wff_44", "wff_or", "__E_wff_45", "wff_xor", "__E_wff_46", "wff_and", "__E_wff_47", "wff_neg", "__E_wff_48", 
	"wff_t", "wff_f", "bf_interval", "__E_wff_49", "bf_eq", "__E_wff_50", "bf_neq", "__E_wff_51", "bf_lt", "__E_wff_52", 
	"bf_nlt", "__E_wff_53", "bf_lteq", "__E_wff_54", "bf_nlteq", "__E_wff_55", "bf_gt", "__E_wff_56", "bf_ngt", "__E_wff_57", 
	"bf_gteq", "__E_wff_58", "bf_ngteq", "__E_wff_59", "wff_U", "__E_wff_60", "__E___E_wff_60_61", "wff_R", "__E_wff_62", "__E___E_wff_62_63", 
	"wff_W", "__E_wff_64", "__E___E_wff_64_65", "wff_S", "__E_wff_66", "__E___E_wff_66_67", "wff_T", "__E_wff_68", "__E___E_wff_68_69", "__E_bf_70", 
	"__E___E_bf_70_71", "bv_type_like", "ba_constant", "variable", "bf_fall", "__E_bf_72", "bf_fex", "__E_bf_73", "bf_ref", "bf_cast", 
	"__E_bf_74", "num", "__E___E_bf_74_75", "bf_t", "bf_f", "bf_neg", "bf_func_app", "__E_bf_76", "func_sym", "func_args", 
	"bf_shr", "__E_bf_77", "__E___E_bf_77_78", "bf_shl", "__E_bf_79", "__E___E_bf_79_80", "bf_add", "__E_bf_81", "__E___E_bf_81_82", "bf_sub", 
	"__E_bf_83", "__E___E_bf_83_84", "bf_mod", "__E_bf_85", "__E___E_bf_85_86", "bf_mul", "bf_div", "__E_bf_87", "__E___E_bf_87_88", "__E_bf_89", 
	"__E___E_bf_89_90", "bf_nor", "__E_bf_91", "__E___E_bf_91_92", "bf_xnor", "__E_bf_93", "__E___E_bf_93_94", "bf_nand", "__E_bf_95", "__E___E_bf_95_96", 
	"bf_or", "__E_bf_97", "bf_xor", "__E_bf_98", "__E_bf_99", "__E___E_bf_99_100", "__E_bf_101", "__E___E_bf_101_102", "__E_bf_103", "__E___E_bf_103_104", 
	"__E_bf_105", "__E___E_bf_105_106", "__E_bf_107", "__E___E_bf_107_108", "__E___E_bf_107_109", "__E_func_args_110", "__E_func_args_111", "func_mode", "func_dynamic", "func_static", 
	"func_sort_list", "__E_func_sort_list_112", "__E_func_sort_list_113", "func_decl", "ctn_neq", "__E_constraint_114", "ctn_eq", "__E_constraint_115", "ctn_gteq", "__E_constraint_116", 
	"ctn_gt", "__E_constraint_117", "ctn_lteq", "__E_constraint_118", "ctn_lt", "__E_constraint_119", "__E_ba_constant_120", "__E_ba_constant_121", "__E_source_122", "__E___E_source_122_123", 
	"__E___E_source_122_124", "__E_src_c_125", "__E_src_c_126", "__E_variable_127", "io_var", "__E_variable_128", "__E_variable_129", "__E_var_name_130", "__E_var_name_131", "__E_var_name_132", 
	"__E_var_name_133", "__E_var_name_134", "offset", "__E_uconst_135", "__E___E_uconst_135_136", "__E_q_vars_137", "__E_q_vars_138", "__E_offsets_139", "__E_offsets_140", "shift", 
	"__E_offset_141", "__E_shift_142", "__E___E_shift_142_143", "__E_chars_144", "__E_chars_145", "__E_digits_146", "__E_integer_147", "__E_typed_148", "comment", "__E_comment_149", 
	"__E_comment_150", "__E_comment_151", "cli", "cli_command", "__E_cli_152", "__E_cli_153", "__E_cli_154", "quit_cmd", "quit_sym", "__E_cli_command_155", 
	"version_cmd", "version_sym", "__E_cli_command_156", "clear_cmd", "clear_sym", "__E_cli_command_157", "help_cmd", "__E_cli_command_158", "help_sym", "__E___E_cli_command_158_159", 
	"__E___E_cli_command_158_160", "help_arg", "file_cmd", "__E_cli_command_161", "file_sym", "q_string", "valid_cmd", "__E_cli_command_162", "valid_sym", "sat_cmd", 
	"__E_cli_command_163", "sat_sym", "unsat_cmd", "__E_cli_command_164", "unsat_sym", "realizable_cmd", "__E_cli_command_165", "realizable_sym", "unrealizable_cmd", "__E_cli_command_166", 
	"unrealizable_sym", "solve_cmd", "__E_cli_command_167", "solve_sym", "__E___E_cli_command_167_168", "solve_options", "lgrs_cmd", "__E_cli_command_169", "lgrs_sym", "run_cmd", 
	"__E_cli_command_170", "run_sym", "__E___E_cli_command_170_171", "__E___E_cli_command_170_172", "__E___E___E_cli_command_170_172_173", "__E___E_cli_command_170_174", "stop_cmd", "__E_cli_command_175", "stop_sym", "memory_cmd", 
	"__E_cli_command_176", "memory_sym", "ltl_cmd", "__E_cli_command_177", "ltl_sym", "func_cmd", "__E_cli_command_178", "func_sym_cmd", "normalize_cmd", "__E_cli_command_179", 
	"normalize_sym", "__E___E_cli_command_179_180", "subst_cmd", "__E_cli_command_181", "subst_sym", "__E___E_cli_command_181_182", "inst_cmd", "__E_cli_command_183", "inst_sym", "__E___E_cli_command_183_184", 
	"dnf_cmd", "__E_cli_command_185", "dnf_sym", "cnf_cmd", "__E_cli_command_186", "cnf_sym", "nnf_cmd", "__E_cli_command_187", "nnf_sym", "mnf_cmd", 
	"__E_cli_command_188", "mnf_sym", "onf_cmd", "__E_cli_command_189", "onf_sym", "qelim_cmd", "__E_cli_command_190", "qelim_sym", "whatis_cmd", "__E_cli_command_191", 
	"whatis_sym", "reset_cmd", "__E_cli_command_192", "reset_sym", "fragment_cmd", "__E_cli_command_193", "fragment_sym", "fragment_name", "get_cmd", "__E_cli_command_194", 
	"get_sym", "__E___E_cli_command_194_195", "set_cmd", "__E_cli_command_196", "set_sym", "__E___E_cli_command_196_197", "enable_cmd", "__E_cli_command_198", "enable_sym", "disable_cmd", 
	"__E_cli_command_199", "disable_sym", "toggle_cmd", "__E_cli_command_200", "toggle_sym", "def_list_cmd", "__E_cli_command_201", "def_sym", "__E___E_cli_command_201_202", "def_print_cmd", 
	"__E_cli_command_203", "def_rr_cmd", "def_input_cmd", "def_output_cmd", "def_type_cmd", "history_list_cmd", "__E_cli_command_204", "history_sym", "__E___E_cli_command_204_205", "history_print_cmd", 
	"__E_cli_command_206", "history", "history_store_cmd", "__E_cli_command_207", "solve_option", "__E_solve_option_208", "solver_mode", "solver_mode_minimum", "__E_solver_mode_209", "solver_mode_maximum", 
	"__E_solver_mode_210", "__E_file_name_211", "__E_option_name_212", "__E_option_value_213", "__E_option_value_214", "__E_fragment_name_215", "fragment_ltl", "fragment_ctl_star", "abs_history_sym", "rel_history_sym", 
	"selection_sym", "execute_sym", "snf_sym", "examples_sym", "__E_help_arg_216", "__E___E_help_arg_216_217", "rel_history", "__E_history_218", "__E___E_history_218_219", "history_id", 
	"abs_history", "__E_history_220", "unreachable", "BDD_ID", "stream_def", "fm_or_term", "__N_0", "__N_1", "__N_2", "__N_3", 
	"__N_4", "__N_5", "__N_6", "__N_7", "__N_8", "__N_9", "__N_10", "__N_11", "__N_12", "__N_13", 
	"__N_14", "__N_15", "__N_16", "__N_17", "__N_18", "__N_19", "__N_20", "__N_21", "__N_22", "__N_23", 
	"__N_24", "__N_25", "__N_26", "__N_27", "__N_28", "__N_29", "__N_30", 
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

idni::prods<char_type, terminal_type>& productions();

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
		__E_wff_40, q_vars, wff_ex, __E_wff_41, wff_ref, constraint, wff_imply, __E_wff_42, wff_rimply, __E_wff_43, 
		wff_equiv, __E_wff_44, wff_or, __E_wff_45, wff_xor, __E_wff_46, wff_and, __E_wff_47, wff_neg, __E_wff_48, 
		wff_t, wff_f, bf_interval, __E_wff_49, bf_eq, __E_wff_50, bf_neq, __E_wff_51, bf_lt, __E_wff_52, 
		bf_nlt, __E_wff_53, bf_lteq, __E_wff_54, bf_nlteq, __E_wff_55, bf_gt, __E_wff_56, bf_ngt, __E_wff_57, 
		bf_gteq, __E_wff_58, bf_ngteq, __E_wff_59, wff_U, __E_wff_60, __E___E_wff_60_61, wff_R, __E_wff_62, __E___E_wff_62_63, 
		wff_W, __E_wff_64, __E___E_wff_64_65, wff_S, __E_wff_66, __E___E_wff_66_67, wff_T, __E_wff_68, __E___E_wff_68_69, __E_bf_70, 
		__E___E_bf_70_71, bv_type_like, ba_constant, variable, bf_fall, __E_bf_72, bf_fex, __E_bf_73, bf_ref, bf_cast, 
		__E_bf_74, num, __E___E_bf_74_75, bf_t, bf_f, bf_neg, bf_func_app, __E_bf_76, func_sym, func_args, 
		bf_shr, __E_bf_77, __E___E_bf_77_78, bf_shl, __E_bf_79, __E___E_bf_79_80, bf_add, __E_bf_81, __E___E_bf_81_82, bf_sub, 
		__E_bf_83, __E___E_bf_83_84, bf_mod, __E_bf_85, __E___E_bf_85_86, bf_mul, bf_div, __E_bf_87, __E___E_bf_87_88, __E_bf_89, 
		__E___E_bf_89_90, bf_nor, __E_bf_91, __E___E_bf_91_92, bf_xnor, __E_bf_93, __E___E_bf_93_94, bf_nand, __E_bf_95, __E___E_bf_95_96, 
		bf_or, __E_bf_97, bf_xor, __E_bf_98, __E_bf_99, __E___E_bf_99_100, __E_bf_101, __E___E_bf_101_102, __E_bf_103, __E___E_bf_103_104, 
		__E_bf_105, __E___E_bf_105_106, __E_bf_107, __E___E_bf_107_108, __E___E_bf_107_109, __E_func_args_110, __E_func_args_111, func_mode, func_dynamic, func_static, 
		func_sort_list, __E_func_sort_list_112, __E_func_sort_list_113, func_decl, ctn_neq, __E_constraint_114, ctn_eq, __E_constraint_115, ctn_gteq, __E_constraint_116, 
		ctn_gt, __E_constraint_117, ctn_lteq, __E_constraint_118, ctn_lt, __E_constraint_119, __E_ba_constant_120, __E_ba_constant_121, __E_source_122, __E___E_source_122_123, 
		__E___E_source_122_124, __E_src_c_125, __E_src_c_126, __E_variable_127, io_var, __E_variable_128, __E_variable_129, __E_var_name_130, __E_var_name_131, __E_var_name_132, 
		__E_var_name_133, __E_var_name_134, offset, __E_uconst_135, __E___E_uconst_135_136, __E_q_vars_137, __E_q_vars_138, __E_offsets_139, __E_offsets_140, shift, 
		__E_offset_141, __E_shift_142, __E___E_shift_142_143, __E_chars_144, __E_chars_145, __E_digits_146, __E_integer_147, __E_typed_148, comment, __E_comment_149, 
		__E_comment_150, __E_comment_151, cli, cli_command, __E_cli_152, __E_cli_153, __E_cli_154, quit_cmd, quit_sym, __E_cli_command_155, 
		version_cmd, version_sym, __E_cli_command_156, clear_cmd, clear_sym, __E_cli_command_157, help_cmd, __E_cli_command_158, help_sym, __E___E_cli_command_158_159, 
		__E___E_cli_command_158_160, help_arg, file_cmd, __E_cli_command_161, file_sym, q_string, valid_cmd, __E_cli_command_162, valid_sym, sat_cmd, 
		__E_cli_command_163, sat_sym, unsat_cmd, __E_cli_command_164, unsat_sym, realizable_cmd, __E_cli_command_165, realizable_sym, unrealizable_cmd, __E_cli_command_166, 
		unrealizable_sym, solve_cmd, __E_cli_command_167, solve_sym, __E___E_cli_command_167_168, solve_options, lgrs_cmd, __E_cli_command_169, lgrs_sym, run_cmd, 
		__E_cli_command_170, run_sym, __E___E_cli_command_170_171, __E___E_cli_command_170_172, __E___E___E_cli_command_170_172_173, __E___E_cli_command_170_174, stop_cmd, __E_cli_command_175, stop_sym, memory_cmd, 
		__E_cli_command_176, memory_sym, ltl_cmd, __E_cli_command_177, ltl_sym, func_cmd, __E_cli_command_178, func_sym_cmd, normalize_cmd, __E_cli_command_179, 
		normalize_sym, __E___E_cli_command_179_180, subst_cmd, __E_cli_command_181, subst_sym, __E___E_cli_command_181_182, inst_cmd, __E_cli_command_183, inst_sym, __E___E_cli_command_183_184, 
		dnf_cmd, __E_cli_command_185, dnf_sym, cnf_cmd, __E_cli_command_186, cnf_sym, nnf_cmd, __E_cli_command_187, nnf_sym, mnf_cmd, 
		__E_cli_command_188, mnf_sym, onf_cmd, __E_cli_command_189, onf_sym, qelim_cmd, __E_cli_command_190, qelim_sym, whatis_cmd, __E_cli_command_191, 
		whatis_sym, reset_cmd, __E_cli_command_192, reset_sym, fragment_cmd, __E_cli_command_193, fragment_sym, fragment_name, get_cmd, __E_cli_command_194, 
		get_sym, __E___E_cli_command_194_195, set_cmd, __E_cli_command_196, set_sym, __E___E_cli_command_196_197, enable_cmd, __E_cli_command_198, enable_sym, disable_cmd, 
		__E_cli_command_199, disable_sym, toggle_cmd, __E_cli_command_200, toggle_sym, def_list_cmd, __E_cli_command_201, def_sym, __E___E_cli_command_201_202, def_print_cmd, 
		__E_cli_command_203, def_rr_cmd, def_input_cmd, def_output_cmd, def_type_cmd, history_list_cmd, __E_cli_command_204, history_sym, __E___E_cli_command_204_205, history_print_cmd, 
		__E_cli_command_206, history, history_store_cmd, __E_cli_command_207, solve_option, __E_solve_option_208, solver_mode, solver_mode_minimum, __E_solver_mode_209, solver_mode_maximum, 
		__E_solver_mode_210, __E_file_name_211, __E_option_name_212, __E_option_value_213, __E_option_value_214, __E_fragment_name_215, fragment_ltl, fragment_ctl_star, abs_history_sym, rel_history_sym, 
		selection_sym, execute_sym, snf_sym, examples_sym, __E_help_arg_216, __E___E_help_arg_216_217, rel_history, __E_history_218, __E___E_history_218_219, history_id, 
		abs_history, __E_history_220, unreachable, BDD_ID, stream_def, fm_or_term, __N_0, __N_1, __N_2, __N_3, 
		__N_4, __N_5, __N_6, __N_7, __N_8, __N_9, __N_10, __N_11, __N_12, __N_13, 
		__N_14, __N_15, __N_16, __N_17, __N_18, __N_19, __N_20, __N_21, __N_22, __N_23, 
		__N_24, __N_25, __N_26, __N_27, __N_28, __N_29, __N_30, 
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
