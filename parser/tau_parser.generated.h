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
	"__E_wff_40", "bf_ngt", "__E_wff_41", "bf_gteq", "__E_wff_42", "bf_ngteq", "__E_wff_43", "__E_bf_44", "__E___E_bf_44_45", "bv_type_like", 
	"ba_constant", "variable", "bf_fall", "__E_bf_46", "bf_fex", "__E_bf_47", "bf_ref", "bf_cast", "__E_bf_48", "num", 
	"__E___E_bf_48_49", "bf_t", "bf_f", "bf_neg", "bf_shr", "__E_bf_50", "__E___E_bf_50_51", "bf_shl", "__E_bf_52", "__E___E_bf_52_53", 
	"bf_add", "__E_bf_54", "__E___E_bf_54_55", "bf_sub", "__E_bf_56", "__E___E_bf_56_57", "bf_mod", "__E_bf_58", "__E___E_bf_58_59", "bf_mul", 
	"bf_div", "__E_bf_60", "__E___E_bf_60_61", "__E_bf_62", "__E___E_bf_62_63", "bf_nor", "__E_bf_64", "__E___E_bf_64_65", "bf_xnor", "__E_bf_66", 
	"__E___E_bf_66_67", "bf_nand", "__E_bf_68", "__E___E_bf_68_69", "bf_or", "__E_bf_70", "bf_xor", "__E_bf_71", "__E_bf_72", "__E___E_bf_72_73", 
	"__E_bf_74", "__E___E_bf_74_75", "__E_bf_76", "__E___E_bf_76_77", "__E_bf_78", "__E___E_bf_78_79", "__E_bf_80", "__E___E_bf_80_81", "__E___E_bf_80_82", "ctn_neq", 
	"__E_constraint_83", "ctn_eq", "__E_constraint_84", "ctn_gteq", "__E_constraint_85", "ctn_gt", "__E_constraint_86", "ctn_lteq", "__E_constraint_87", "ctn_lt", 
	"__E_constraint_88", "__E_ba_constant_89", "__E_ba_constant_90", "__E_source_91", "__E___E_source_91_92", "__E___E_source_91_93", "__E_src_c_94", "__E_src_c_95", "__E_variable_96", "io_var", 
	"__E_variable_97", "__E_var_name_98", "__E_var_name_99", "__E_var_name_100", "__E_var_name_101", "__E_var_name_102", "offset", "__E_uconst_103", "__E___E_uconst_103_104", "__E_q_vars_105", 
	"__E_q_vars_106", "__E_offsets_107", "__E_offsets_108", "shift", "__E_offset_109", "__E_shift_110", "__E___E_shift_110_111", "__E_chars_112", "__E_chars_113", "__E_digits_114", 
	"__E_integer_115", "__E_typed_116", "comment", "__E_comment_117", "__E_comment_118", "__E_comment_119", "cli", "cli_command", "__E_cli_120", "__E_cli_121", 
	"__E_cli_122", "quit_cmd", "quit_sym", "__E_cli_command_123", "version_cmd", "version_sym", "__E_cli_command_124", "clear_cmd", "clear_sym", "__E_cli_command_125", 
	"help_cmd", "__E_cli_command_126", "help_sym", "__E___E_cli_command_126_127", "__E___E_cli_command_126_128", "help_arg", "file_cmd", "__E_cli_command_129", "file_sym", "q_string", 
	"valid_cmd", "__E_cli_command_130", "valid_sym", "sat_cmd", "__E_cli_command_131", "sat_sym", "unsat_cmd", "__E_cli_command_132", "unsat_sym", "solve_cmd", 
	"__E_cli_command_133", "solve_sym", "__E___E_cli_command_133_134", "solve_options", "lgrs_cmd", "__E_cli_command_135", "lgrs_sym", "run_cmd", "__E_cli_command_136", "run_sym", 
	"__E___E_cli_command_136_137", "normalize_cmd", "__E_cli_command_138", "normalize_sym", "__E___E_cli_command_138_139", "subst_cmd", "__E_cli_command_140", "subst_sym", "__E___E_cli_command_140_141", "inst_cmd", 
	"__E_cli_command_142", "inst_sym", "__E___E_cli_command_142_143", "dnf_cmd", "__E_cli_command_144", "dnf_sym", "cnf_cmd", "__E_cli_command_145", "cnf_sym", "nnf_cmd", 
	"__E_cli_command_146", "nnf_sym", "mnf_cmd", "__E_cli_command_147", "mnf_sym", "onf_cmd", "__E_cli_command_148", "onf_sym", "qelim_cmd", "__E_cli_command_149", 
	"qelim_sym", "get_cmd", "__E_cli_command_150", "get_sym", "__E___E_cli_command_150_151", "set_cmd", "__E_cli_command_152", "set_sym", "__E___E_cli_command_152_153", "enable_cmd", 
	"__E_cli_command_154", "enable_sym", "disable_cmd", "__E_cli_command_155", "disable_sym", "toggle_cmd", "__E_cli_command_156", "toggle_sym", "def_list_cmd", "__E_cli_command_157", 
	"def_sym", "__E___E_cli_command_157_158", "def_print_cmd", "__E_cli_command_159", "def_rr_cmd", "def_input_cmd", "def_output_cmd", "history_list_cmd", "__E_cli_command_160", "history_sym", 
	"__E___E_cli_command_160_161", "history_print_cmd", "__E_cli_command_162", "history", "history_store_cmd", "__E_cli_command_163", "__E_solve_options_164", "solve_option", "__E_solve_options_165", "__E_solve_option_166", 
	"solver_mode", "solver_mode_minimum", "__E_solver_mode_167", "solver_mode_maximum", "__E_solver_mode_168", "__E_file_name_169", "__E_option_name_170", "__E_option_value_171", "__E_option_value_172", "abs_history_sym", 
	"rel_history_sym", "selection_sym", "execute_sym", "snf_sym", "examples_sym", "__E_help_arg_173", "__E___E_help_arg_173_174", "rel_history", "__E_history_175", "__E___E_history_175_176", 
	"history_id", "abs_history", "__E_history_177", "unreachable", "BDD_ID", "stream_def", "fm_or_term", "__N_0", "__N_1", "__N_2", 
	"__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", "__N_9", "__N_10", "__N_11", "__N_12", 
	"__N_13", "__N_14", "__N_15", "__N_16", "__N_17", "__N_18", "__N_19", "__N_20", "__N_21", "__N_22", 
	"__N_23", "__N_24", "__N_25", "__N_26", "__N_27", "__N_28", "__N_29", "__N_30", 
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
		wff_conditional, __E_wff_23, wff_all, __E_wff_24, q_vars, wff_ex, __E_wff_25, wff_ref, constraint, wff_imply, 
		__E_wff_26, wff_rimply, __E_wff_27, wff_equiv, __E_wff_28, wff_or, __E_wff_29, wff_xor, __E_wff_30, wff_and, 
		__E_wff_31, wff_neg, __E_wff_32, wff_t, wff_f, bf_interval, __E_wff_33, bf_eq, __E_wff_34, bf_neq, 
		__E_wff_35, bf_lt, __E_wff_36, bf_nlt, __E_wff_37, bf_lteq, __E_wff_38, bf_nlteq, __E_wff_39, bf_gt, 
		__E_wff_40, bf_ngt, __E_wff_41, bf_gteq, __E_wff_42, bf_ngteq, __E_wff_43, __E_bf_44, __E___E_bf_44_45, bv_type_like, 
		ba_constant, variable, bf_fall, __E_bf_46, bf_fex, __E_bf_47, bf_ref, bf_cast, __E_bf_48, num, 
		__E___E_bf_48_49, bf_t, bf_f, bf_neg, bf_shr, __E_bf_50, __E___E_bf_50_51, bf_shl, __E_bf_52, __E___E_bf_52_53, 
		bf_add, __E_bf_54, __E___E_bf_54_55, bf_sub, __E_bf_56, __E___E_bf_56_57, bf_mod, __E_bf_58, __E___E_bf_58_59, bf_mul, 
		bf_div, __E_bf_60, __E___E_bf_60_61, __E_bf_62, __E___E_bf_62_63, bf_nor, __E_bf_64, __E___E_bf_64_65, bf_xnor, __E_bf_66, 
		__E___E_bf_66_67, bf_nand, __E_bf_68, __E___E_bf_68_69, bf_or, __E_bf_70, bf_xor, __E_bf_71, __E_bf_72, __E___E_bf_72_73, 
		__E_bf_74, __E___E_bf_74_75, __E_bf_76, __E___E_bf_76_77, __E_bf_78, __E___E_bf_78_79, __E_bf_80, __E___E_bf_80_81, __E___E_bf_80_82, ctn_neq, 
		__E_constraint_83, ctn_eq, __E_constraint_84, ctn_gteq, __E_constraint_85, ctn_gt, __E_constraint_86, ctn_lteq, __E_constraint_87, ctn_lt, 
		__E_constraint_88, __E_ba_constant_89, __E_ba_constant_90, __E_source_91, __E___E_source_91_92, __E___E_source_91_93, __E_src_c_94, __E_src_c_95, __E_variable_96, io_var, 
		__E_variable_97, __E_var_name_98, __E_var_name_99, __E_var_name_100, __E_var_name_101, __E_var_name_102, offset, __E_uconst_103, __E___E_uconst_103_104, __E_q_vars_105, 
		__E_q_vars_106, __E_offsets_107, __E_offsets_108, shift, __E_offset_109, __E_shift_110, __E___E_shift_110_111, __E_chars_112, __E_chars_113, __E_digits_114, 
		__E_integer_115, __E_typed_116, comment, __E_comment_117, __E_comment_118, __E_comment_119, cli, cli_command, __E_cli_120, __E_cli_121, 
		__E_cli_122, quit_cmd, quit_sym, __E_cli_command_123, version_cmd, version_sym, __E_cli_command_124, clear_cmd, clear_sym, __E_cli_command_125, 
		help_cmd, __E_cli_command_126, help_sym, __E___E_cli_command_126_127, __E___E_cli_command_126_128, help_arg, file_cmd, __E_cli_command_129, file_sym, q_string, 
		valid_cmd, __E_cli_command_130, valid_sym, sat_cmd, __E_cli_command_131, sat_sym, unsat_cmd, __E_cli_command_132, unsat_sym, solve_cmd, 
		__E_cli_command_133, solve_sym, __E___E_cli_command_133_134, solve_options, lgrs_cmd, __E_cli_command_135, lgrs_sym, run_cmd, __E_cli_command_136, run_sym, 
		__E___E_cli_command_136_137, normalize_cmd, __E_cli_command_138, normalize_sym, __E___E_cli_command_138_139, subst_cmd, __E_cli_command_140, subst_sym, __E___E_cli_command_140_141, inst_cmd, 
		__E_cli_command_142, inst_sym, __E___E_cli_command_142_143, dnf_cmd, __E_cli_command_144, dnf_sym, cnf_cmd, __E_cli_command_145, cnf_sym, nnf_cmd, 
		__E_cli_command_146, nnf_sym, mnf_cmd, __E_cli_command_147, mnf_sym, onf_cmd, __E_cli_command_148, onf_sym, qelim_cmd, __E_cli_command_149, 
		qelim_sym, get_cmd, __E_cli_command_150, get_sym, __E___E_cli_command_150_151, set_cmd, __E_cli_command_152, set_sym, __E___E_cli_command_152_153, enable_cmd, 
		__E_cli_command_154, enable_sym, disable_cmd, __E_cli_command_155, disable_sym, toggle_cmd, __E_cli_command_156, toggle_sym, def_list_cmd, __E_cli_command_157, 
		def_sym, __E___E_cli_command_157_158, def_print_cmd, __E_cli_command_159, def_rr_cmd, def_input_cmd, def_output_cmd, history_list_cmd, __E_cli_command_160, history_sym, 
		__E___E_cli_command_160_161, history_print_cmd, __E_cli_command_162, history, history_store_cmd, __E_cli_command_163, __E_solve_options_164, solve_option, __E_solve_options_165, __E_solve_option_166, 
		solver_mode, solver_mode_minimum, __E_solver_mode_167, solver_mode_maximum, __E_solver_mode_168, __E_file_name_169, __E_option_name_170, __E_option_value_171, __E_option_value_172, abs_history_sym, 
		rel_history_sym, selection_sym, execute_sym, snf_sym, examples_sym, __E_help_arg_173, __E___E_help_arg_173_174, rel_history, __E_history_175, __E___E_history_175_176, 
		history_id, abs_history, __E_history_177, unreachable, BDD_ID, stream_def, fm_or_term, __N_0, __N_1, __N_2, 
		__N_3, __N_4, __N_5, __N_6, __N_7, __N_8, __N_9, __N_10, __N_11, __N_12, 
		__N_13, __N_14, __N_15, __N_16, __N_17, __N_18, __N_19, __N_20, __N_21, __N_22, 
		__N_23, __N_24, __N_25, __N_26, __N_27, __N_28, __N_29, __N_30, 
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
