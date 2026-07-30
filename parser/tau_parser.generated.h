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
	"bf_fex", "__E_bf_55", "bf_ref", "bf_cast", "__E_bf_56", "__E___E_bf_56_57", "__E___E_bf_56_58", "bf_t", "bf_f", "bf_neg", 
	"bf_func_app", "__E_bf_59", "func_sym", "func_args", "bf_shr", "__E_bf_60", "__E___E_bf_60_61", "bf_shl", "__E_bf_62", "__E___E_bf_62_63", 
	"bf_add", "__E_bf_64", "bf_sub", "__E_bf_65", "__E___E_bf_65_66", "bf_mod", "__E_bf_67", "__E___E_bf_67_68", "bf_mul", "__E_bf_69", 
	"bf_div", "__E_bf_70", "__E___E_bf_70_71", "bf_nor", "__E_bf_72", "__E___E_bf_72_73", "bf_xnor", "__E_bf_74", "__E___E_bf_74_75", "bf_nand", 
	"__E_bf_76", "__E___E_bf_76_77", "bf_or", "__E_bf_78", "bf_xor", "__E_bf_79", "__E_bf_80", "__E___E_bf_80_81", "__E_bf_82", "__E___E_bf_82_83", 
	"__E_bf_84", "__E___E_bf_84_85", "__E_bf_86", "__E___E_bf_86_87", "__E_bf_88", "__E___E_bf_88_89", "__E___E_bf_88_90", "__E_func_args_91", "__E_func_args_92", "func_mode", 
	"func_dynamic", "func_static", "func_sort_list", "__E_func_sort_list_93", "__E_func_sort_list_94", "func_decl", "ctn_neq", "__E_constraint_95", "num", "ctn_eq", 
	"__E_constraint_96", "ctn_gteq", "__E_constraint_97", "ctn_gt", "__E_constraint_98", "ctn_lteq", "__E_constraint_99", "ctn_lt", "__E_constraint_100", "__E_ba_constant_101", 
	"__E_ba_constant_102", "__E_source_103", "__E___E_source_103_104", "__E___E_source_103_105", "__E_src_c_106", "__E_src_c_107", "__E_variable_108", "io_var", "__E_variable_109", "__E_var_name_110", 
	"__E_var_name_111", "__E_var_name_112", "__E_var_name_113", "__E_var_name_114", "offset", "__E_uconst_115", "__E___E_uconst_115_116", "__E_q_vars_117", "__E_q_vars_118", "__E_offsets_119", 
	"__E_offsets_120", "shift", "__E_offset_121", "__E_shift_122", "__E___E_shift_122_123", "__E_chars_124", "__E_chars_125", "__E_digits_126", "__E_integer_127", "__E_typed_128", 
	"comment", "__E_comment_129", "__E_comment_130", "__E_comment_131", "cli", "cli_command", "__E_cli_132", "__E_cli_133", "__E_cli_134", "quit_cmd", 
	"quit_sym", "__E_cli_command_135", "version_cmd", "version_sym", "__E_cli_command_136", "clear_cmd", "clear_sym", "__E_cli_command_137", "help_cmd", "__E_cli_command_138", 
	"help_sym", "__E___E_cli_command_138_139", "__E___E_cli_command_138_140", "help_arg", "file_cmd", "__E_cli_command_141", "file_sym", "q_string", "valid_cmd", "__E_cli_command_142", 
	"valid_sym", "sat_cmd", "__E_cli_command_143", "sat_sym", "unsat_cmd", "__E_cli_command_144", "unsat_sym", "realizable_cmd", "__E_cli_command_145", "realizable_sym", 
	"unrealizable_cmd", "__E_cli_command_146", "unrealizable_sym", "solve_cmd", "__E_cli_command_147", "solve_sym", "__E___E_cli_command_147_148", "solve_options", "lgrs_cmd", "__E_cli_command_149", 
	"lgrs_sym", "run_cmd", "__E_cli_command_150", "run_sym", "__E___E_cli_command_150_151", "__E___E_cli_command_150_152", "__E___E___E_cli_command_150_152_153", "__E___E_cli_command_150_154", "stop_cmd", "__E_cli_command_155", 
	"stop_sym", "memory_cmd", "__E_cli_command_156", "memory_sym", "ltl_cmd", "__E_cli_command_157", "ltl_sym", "func_cmd", "__E_cli_command_158", "func_sym_cmd", 
	"normalize_cmd", "__E_cli_command_159", "normalize_sym", "__E___E_cli_command_159_160", "subst_cmd", "__E_cli_command_161", "subst_sym", "__E___E_cli_command_161_162", "inst_cmd", "__E_cli_command_163", 
	"inst_sym", "__E___E_cli_command_163_164", "dnf_cmd", "__E_cli_command_165", "dnf_sym", "cnf_cmd", "__E_cli_command_166", "cnf_sym", "nnf_cmd", "__E_cli_command_167", 
	"nnf_sym", "mnf_cmd", "__E_cli_command_168", "mnf_sym", "onf_cmd", "__E_cli_command_169", "onf_sym", "qelim_cmd", "__E_cli_command_170", "qelim_sym", 
	"whatis_cmd", "__E_cli_command_171", "whatis_sym", "reset_cmd", "__E_cli_command_172", "reset_sym", "fragment_cmd", "__E_cli_command_173", "fragment_sym", "fragment_name", 
	"get_cmd", "__E_cli_command_174", "get_sym", "__E___E_cli_command_174_175", "set_cmd", "__E_cli_command_176", "set_sym", "__E___E_cli_command_176_177", "enable_cmd", "__E_cli_command_178", 
	"enable_sym", "disable_cmd", "__E_cli_command_179", "disable_sym", "toggle_cmd", "__E_cli_command_180", "toggle_sym", "def_list_cmd", "__E_cli_command_181", "def_sym", 
	"__E___E_cli_command_181_182", "def_print_cmd", "__E_cli_command_183", "def_rr_cmd", "def_input_cmd", "def_output_cmd", "history_list_cmd", "__E_cli_command_184", "history_sym", "__E___E_cli_command_184_185", 
	"history_print_cmd", "__E_cli_command_186", "history", "history_store_cmd", "__E_cli_command_187", "solve_option", "__E_solve_option_188", "solver_mode", "solver_mode_minimum", "__E_solver_mode_189", 
	"solver_mode_maximum", "__E_solver_mode_190", "__E_file_name_191", "__E_option_name_192", "__E_option_value_193", "__E_fragment_name_194", "fragment_ltl", "fragment_ctl_star", "abs_history_sym", "rel_history_sym", 
	"selection_sym", "execute_sym", "snf_sym", "examples_sym", "__E_help_arg_195", "__E___E_help_arg_195_196", "rel_history", "__E_history_197", "__E___E_history_197_198", "history_id", 
	"abs_history", "__E_history_199", "unreachable", "BDD_ID", "stream_def", "fm_or_term", "__N_0", "__N_1", "__N_2", "__N_3", 
	"__N_4", "__N_5", "__N_6", "__N_7", "__N_8", "__N_9", "__N_10", "__N_11", "__N_12", "__N_13", 
	"__N_14", "__N_15", "__N_16", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', 'a', 'b', 'k', 
	'r', ',', 'm', '<', '>', 'w', 'y', '[', ']', 'G', 
	'F', 'A', 'E', '-', '?', 'x', '|', '^', '&', '!', 
	'T', 'U', 'R', 'W', 'S', '+', '%', '*', '/', '1', 
	'0', '\'', 'd', '{', '}', '_', '$', '\t', '\n', '\r', 
	'#', 'q', 'v', 'h', 'p', 'z', 'g', '"', 
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

idni::prods<char_type, terminal_type>& productions();

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
		bf_fex, __E_bf_55, bf_ref, bf_cast, __E_bf_56, __E___E_bf_56_57, __E___E_bf_56_58, bf_t, bf_f, bf_neg, 
		bf_func_app, __E_bf_59, func_sym, func_args, bf_shr, __E_bf_60, __E___E_bf_60_61, bf_shl, __E_bf_62, __E___E_bf_62_63, 
		bf_add, __E_bf_64, bf_sub, __E_bf_65, __E___E_bf_65_66, bf_mod, __E_bf_67, __E___E_bf_67_68, bf_mul, __E_bf_69, 
		bf_div, __E_bf_70, __E___E_bf_70_71, bf_nor, __E_bf_72, __E___E_bf_72_73, bf_xnor, __E_bf_74, __E___E_bf_74_75, bf_nand, 
		__E_bf_76, __E___E_bf_76_77, bf_or, __E_bf_78, bf_xor, __E_bf_79, __E_bf_80, __E___E_bf_80_81, __E_bf_82, __E___E_bf_82_83, 
		__E_bf_84, __E___E_bf_84_85, __E_bf_86, __E___E_bf_86_87, __E_bf_88, __E___E_bf_88_89, __E___E_bf_88_90, __E_func_args_91, __E_func_args_92, func_mode, 
		func_dynamic, func_static, func_sort_list, __E_func_sort_list_93, __E_func_sort_list_94, func_decl, ctn_neq, __E_constraint_95, num, ctn_eq, 
		__E_constraint_96, ctn_gteq, __E_constraint_97, ctn_gt, __E_constraint_98, ctn_lteq, __E_constraint_99, ctn_lt, __E_constraint_100, __E_ba_constant_101, 
		__E_ba_constant_102, __E_source_103, __E___E_source_103_104, __E___E_source_103_105, __E_src_c_106, __E_src_c_107, __E_variable_108, io_var, __E_variable_109, __E_var_name_110, 
		__E_var_name_111, __E_var_name_112, __E_var_name_113, __E_var_name_114, offset, __E_uconst_115, __E___E_uconst_115_116, __E_q_vars_117, __E_q_vars_118, __E_offsets_119, 
		__E_offsets_120, shift, __E_offset_121, __E_shift_122, __E___E_shift_122_123, __E_chars_124, __E_chars_125, __E_digits_126, __E_integer_127, __E_typed_128, 
		comment, __E_comment_129, __E_comment_130, __E_comment_131, cli, cli_command, __E_cli_132, __E_cli_133, __E_cli_134, quit_cmd, 
		quit_sym, __E_cli_command_135, version_cmd, version_sym, __E_cli_command_136, clear_cmd, clear_sym, __E_cli_command_137, help_cmd, __E_cli_command_138, 
		help_sym, __E___E_cli_command_138_139, __E___E_cli_command_138_140, help_arg, file_cmd, __E_cli_command_141, file_sym, q_string, valid_cmd, __E_cli_command_142, 
		valid_sym, sat_cmd, __E_cli_command_143, sat_sym, unsat_cmd, __E_cli_command_144, unsat_sym, realizable_cmd, __E_cli_command_145, realizable_sym, 
		unrealizable_cmd, __E_cli_command_146, unrealizable_sym, solve_cmd, __E_cli_command_147, solve_sym, __E___E_cli_command_147_148, solve_options, lgrs_cmd, __E_cli_command_149, 
		lgrs_sym, run_cmd, __E_cli_command_150, run_sym, __E___E_cli_command_150_151, __E___E_cli_command_150_152, __E___E___E_cli_command_150_152_153, __E___E_cli_command_150_154, stop_cmd, __E_cli_command_155, 
		stop_sym, memory_cmd, __E_cli_command_156, memory_sym, ltl_cmd, __E_cli_command_157, ltl_sym, func_cmd, __E_cli_command_158, func_sym_cmd, 
		normalize_cmd, __E_cli_command_159, normalize_sym, __E___E_cli_command_159_160, subst_cmd, __E_cli_command_161, subst_sym, __E___E_cli_command_161_162, inst_cmd, __E_cli_command_163, 
		inst_sym, __E___E_cli_command_163_164, dnf_cmd, __E_cli_command_165, dnf_sym, cnf_cmd, __E_cli_command_166, cnf_sym, nnf_cmd, __E_cli_command_167, 
		nnf_sym, mnf_cmd, __E_cli_command_168, mnf_sym, onf_cmd, __E_cli_command_169, onf_sym, qelim_cmd, __E_cli_command_170, qelim_sym, 
		whatis_cmd, __E_cli_command_171, whatis_sym, reset_cmd, __E_cli_command_172, reset_sym, fragment_cmd, __E_cli_command_173, fragment_sym, fragment_name, 
		get_cmd, __E_cli_command_174, get_sym, __E___E_cli_command_174_175, set_cmd, __E_cli_command_176, set_sym, __E___E_cli_command_176_177, enable_cmd, __E_cli_command_178, 
		enable_sym, disable_cmd, __E_cli_command_179, disable_sym, toggle_cmd, __E_cli_command_180, toggle_sym, def_list_cmd, __E_cli_command_181, def_sym, 
		__E___E_cli_command_181_182, def_print_cmd, __E_cli_command_183, def_rr_cmd, def_input_cmd, def_output_cmd, history_list_cmd, __E_cli_command_184, history_sym, __E___E_cli_command_184_185, 
		history_print_cmd, __E_cli_command_186, history, history_store_cmd, __E_cli_command_187, solve_option, __E_solve_option_188, solver_mode, solver_mode_minimum, __E_solver_mode_189, 
		solver_mode_maximum, __E_solver_mode_190, __E_file_name_191, __E_option_name_192, __E_option_value_193, __E_fragment_name_194, fragment_ltl, fragment_ctl_star, abs_history_sym, rel_history_sym, 
		selection_sym, execute_sym, snf_sym, examples_sym, __E_help_arg_195, __E___E_help_arg_195_196, rel_history, __E_history_197, __E___E_history_197_198, history_id, 
		abs_history, __E_history_199, unreachable, BDD_ID, stream_def, fm_or_term, __N_0, __N_1, __N_2, __N_3, 
		__N_4, __N_5, __N_6, __N_7, __N_8, __N_9, __N_10, __N_11, __N_12, __N_13, 
		__N_14, __N_15, __N_16, 
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
