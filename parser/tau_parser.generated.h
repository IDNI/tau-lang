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
