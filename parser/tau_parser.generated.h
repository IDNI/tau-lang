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
	"sat_cmd", "__E_cli_command_142", "sat_sym", "unsat_cmd", "__E_cli_command_143", "unsat_sym", "realizable_cmd", "__E_cli_command_144", "realizable_sym", "unrealizable_cmd", 
	"__E_cli_command_145", "unrealizable_sym", "solve_cmd", "__E_cli_command_146", "solve_sym", "__E___E_cli_command_146_147", "solve_options", "lgrs_cmd", "__E_cli_command_148", "lgrs_sym", 
	"run_cmd", "__E_cli_command_149", "run_sym", "__E___E_cli_command_149_150", "__E___E_cli_command_149_151", "__E___E___E_cli_command_149_151_152", "__E___E_cli_command_149_153", "stop_cmd", "__E_cli_command_154", "stop_sym", 
	"memory_cmd", "__E_cli_command_155", "memory_sym", "ltl_cmd", "__E_cli_command_156", "ltl_sym", "func_cmd", "__E_cli_command_157", "func_sym_cmd", "normalize_cmd", 
	"__E_cli_command_158", "normalize_sym", "__E___E_cli_command_158_159", "subst_cmd", "__E_cli_command_160", "subst_sym", "__E___E_cli_command_160_161", "inst_cmd", "__E_cli_command_162", "inst_sym", 
	"__E___E_cli_command_162_163", "dnf_cmd", "__E_cli_command_164", "dnf_sym", "cnf_cmd", "__E_cli_command_165", "cnf_sym", "nnf_cmd", "__E_cli_command_166", "nnf_sym", 
	"mnf_cmd", "__E_cli_command_167", "mnf_sym", "onf_cmd", "__E_cli_command_168", "onf_sym", "qelim_cmd", "__E_cli_command_169", "qelim_sym", "whatis_cmd", 
	"__E_cli_command_170", "whatis_sym", "reset_cmd", "__E_cli_command_171", "reset_sym", "fragment_cmd", "__E_cli_command_172", "fragment_sym", "fragment_name", "get_cmd", 
	"__E_cli_command_173", "get_sym", "__E___E_cli_command_173_174", "set_cmd", "__E_cli_command_175", "set_sym", "__E___E_cli_command_175_176", "enable_cmd", "__E_cli_command_177", "enable_sym", 
	"disable_cmd", "__E_cli_command_178", "disable_sym", "toggle_cmd", "__E_cli_command_179", "toggle_sym", "def_list_cmd", "__E_cli_command_180", "def_sym", "__E___E_cli_command_180_181", 
	"def_print_cmd", "__E_cli_command_182", "def_rr_cmd", "def_input_cmd", "def_output_cmd", "history_list_cmd", "__E_cli_command_183", "history_sym", "__E___E_cli_command_183_184", "history_print_cmd", 
	"__E_cli_command_185", "history", "history_store_cmd", "__E_cli_command_186", "solve_option", "__E_solve_option_187", "solver_mode", "solver_mode_minimum", "__E_solver_mode_188", "solver_mode_maximum", 
	"__E_solver_mode_189", "__E_file_name_190", "__E_option_name_191", "__E_option_value_192", "__E_option_value_193", "__E_fragment_name_194", "fragment_ltl", "fragment_ctl_star", "abs_history_sym", "rel_history_sym", 
	"selection_sym", "execute_sym", "snf_sym", "examples_sym", "__E_help_arg_195", "__E___E_help_arg_195_196", "rel_history", "__E_history_197", "__E___E_history_197_198", "history_id", 
	"abs_history", "__E_history_199", "unreachable", "BDD_ID", "stream_def", "fm_or_term", "__N_0", "__N_1", "__N_2", "__N_3", 
	"__N_4", "__N_5", "__N_6", "__N_7", "__N_8", "__N_9", "__N_10", "__N_11", "__N_12", "__N_13", 
	"__N_14", "__N_15", "__N_16", "__N_17", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', 'a', 'b', 'k', 
	'r', ',', 'm', '<', '>', 'w', 'y', '[', ']', 'G', 
	'F', 'A', 'E', '-', '?', 'x', '|', '^', '&', '!', 
	'T', 'U', 'R', 'W', 'S', 'v', '+', '%', '*', '/', 
	'1', '0', '\'', 'd', '{', '}', '_', '$', '\t', '\n', 
	'\r', '#', 'q', 'h', 'p', 'z', 'g', '"', 
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
		sat_cmd, __E_cli_command_142, sat_sym, unsat_cmd, __E_cli_command_143, unsat_sym, realizable_cmd, __E_cli_command_144, realizable_sym, unrealizable_cmd, 
		__E_cli_command_145, unrealizable_sym, solve_cmd, __E_cli_command_146, solve_sym, __E___E_cli_command_146_147, solve_options, lgrs_cmd, __E_cli_command_148, lgrs_sym, 
		run_cmd, __E_cli_command_149, run_sym, __E___E_cli_command_149_150, __E___E_cli_command_149_151, __E___E___E_cli_command_149_151_152, __E___E_cli_command_149_153, stop_cmd, __E_cli_command_154, stop_sym, 
		memory_cmd, __E_cli_command_155, memory_sym, ltl_cmd, __E_cli_command_156, ltl_sym, func_cmd, __E_cli_command_157, func_sym_cmd, normalize_cmd, 
		__E_cli_command_158, normalize_sym, __E___E_cli_command_158_159, subst_cmd, __E_cli_command_160, subst_sym, __E___E_cli_command_160_161, inst_cmd, __E_cli_command_162, inst_sym, 
		__E___E_cli_command_162_163, dnf_cmd, __E_cli_command_164, dnf_sym, cnf_cmd, __E_cli_command_165, cnf_sym, nnf_cmd, __E_cli_command_166, nnf_sym, 
		mnf_cmd, __E_cli_command_167, mnf_sym, onf_cmd, __E_cli_command_168, onf_sym, qelim_cmd, __E_cli_command_169, qelim_sym, whatis_cmd, 
		__E_cli_command_170, whatis_sym, reset_cmd, __E_cli_command_171, reset_sym, fragment_cmd, __E_cli_command_172, fragment_sym, fragment_name, get_cmd, 
		__E_cli_command_173, get_sym, __E___E_cli_command_173_174, set_cmd, __E_cli_command_175, set_sym, __E___E_cli_command_175_176, enable_cmd, __E_cli_command_177, enable_sym, 
		disable_cmd, __E_cli_command_178, disable_sym, toggle_cmd, __E_cli_command_179, toggle_sym, def_list_cmd, __E_cli_command_180, def_sym, __E___E_cli_command_180_181, 
		def_print_cmd, __E_cli_command_182, def_rr_cmd, def_input_cmd, def_output_cmd, history_list_cmd, __E_cli_command_183, history_sym, __E___E_cli_command_183_184, history_print_cmd, 
		__E_cli_command_185, history, history_store_cmd, __E_cli_command_186, solve_option, __E_solve_option_187, solver_mode, solver_mode_minimum, __E_solver_mode_188, solver_mode_maximum, 
		__E_solver_mode_189, __E_file_name_190, __E_option_name_191, __E_option_value_192, __E_option_value_193, __E_fragment_name_194, fragment_ltl, fragment_ctl_star, abs_history_sym, rel_history_sym, 
		selection_sym, execute_sym, snf_sym, examples_sym, __E_help_arg_195, __E___E_help_arg_195_196, rel_history, __E_history_197, __E___E_history_197_198, history_id, 
		abs_history, __E_history_199, unreachable, BDD_ID, stream_def, fm_or_term, __N_0, __N_1, __N_2, __N_3, 
		__N_4, __N_5, __N_6, __N_7, __N_8, __N_9, __N_10, __N_11, __N_12, __N_13, 
		__N_14, __N_15, __N_16, __N_17, 
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
