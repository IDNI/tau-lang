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
	"", "eof", "space", "digit", "xdigit", "alpha", "alnum", "punct", "printable", "start", 
	"spec", "_", "definitions", "main", "wff", "spec_multiline", "__E_spec_multiline_0", "rec_relation", "input_def", "output_def", 
	"__E_spec_multiline_1", "__E_definitions_2", "__E___E_definitions_2_3", "__E_definitions_4", "rec_relations", "__E_rec_relations_5", "__E_rec_relations_6", "ref", "__E_rec_relation_7", "capture", 
	"bf", "io_var_name", "__E_input_def_8", "typed", "__", "stream", "__E_output_def_9", "__E_output_def_10", "q_file_name", "console_sym", 
	"sym", "__E_ref_11", "offsets", "ref_args", "__E_ref_12", "fp_fallback", "first_sym", "last_sym", "__E_ref_args_13", "__E___E_ref_args_13_14", 
	"ref_arg", "__E___E_ref_args_13_15", "__E___E_ref_args_13_16", "wff_rule", "wff_matcher", "wff_body", "bf_rule", "bf_matcher", "bf_body", "tau_constant_source", 
	"__E_tau_constant_source_17", "wff_parenthesis", "__E_wff_18", "wff_sometimes", "__E_wff_19", "__E___E_wff_19_20", "wff_always", "__E_wff_21", "__E___E_wff_21_22", "wff_conditional", 
	"__E_wff_23", "wff_all", "__E_wff_24", "q_vars", "wff_ex", "__E_wff_25", "wff_ref", "constraint", "wff_imply", "__E_wff_26", 
	"wff_rimply", "__E_wff_27", "wff_equiv", "__E_wff_28", "wff_or", "__E_wff_29", "wff_xor", "__E_wff_30", "wff_and", "__E_wff_31", 
	"wff_neg", "__E_wff_32", "wff_t", "wff_f", "bf_interval", "__E_wff_33", "bf_eq", "__E_wff_34", "bf_neq", "__E_wff_35", 
	"bf_lteq", "__E_wff_36", "bf_nlteq", "__E_wff_37", "bf_gt", "__E_wff_38", "bf_ngt", "__E_wff_39", "bf_gteq", "__E_wff_40", 
	"bf_ngteq", "__E_wff_41", "bf_lt", "__E_wff_42", "bf_nlt", "__E_wff_43", "bv_eq", "__E_wff_44", "bv", "bv_neq", 
	"__E_wff_45", "bv_lteq", "__E_wff_46", "bv_nlteq", "__E_wff_47", "bv_gt", "__E_wff_48", "bv_ngt", "__E_wff_49", "bv_gteq", 
	"__E_wff_50", "bv_ngteq", "__E_wff_51", "bv_lt", "__E_wff_52", "bv_nlt", "__E_wff_53", "bf_parenthesis", "__E_bf_54", "bf_constant", 
	"variable", "bf_fall", "__E_bf_55", "bf_fex", "__E_bf_56", "bf_ref", "bf_or", "__E_bf_57", "bf_xor", "__E_bf_58", 
	"bf_t", "__E_bf_59", "__E___E_bf_59_60", "bf_f", "__E_bf_61", "__E___E_bf_61_62", "bf_and", "__E_bf_63", "__E___E_bf_63_64", "bf_neg", 
	"__E_bf_65", "bf_neg_oprnd", "__E___E_bf_65_66", "bf_and_nosep", "__E_bf_67", "bf_and_nosep_1st_oprnd", "__E___E_bf_67_68", "bf_and_nosep_2nd_oprnd", "__E___E_bf_67_69", "bv_parenthesis", 
	"__E_bv_70", "bv_checked", "__E_bv_71", "bv_add", "__E_bv_72", "bv_mul", "__E_bv_73", "bv_div", "__E_bv_74", "bv_mod", 
	"__E_bv_75", "bv_sub", "__E_bv_76", "bv_neg", "bv_and", "__E_bv_77", "bv_nand", "__E_bv_78", "bv_or", "__E_bv_79", 
	"bv_nor", "__E_bv_80", "bv_xor", "__E_bv_81", "bv_xnor", "__E_bv_82", "bv_shl", "__E_bv_83", "bv_shr", "__E_bv_84", 
	"bv_min", "__E_bv_85", "bv_max", "__E_bv_86", "bv_constant", "binary", "__E_bv_constant_87", "decimal", "__E_bv_constant_88", "hexadecimal", 
	"__E_bv_constant_89", "__E_binary_90", "__E_binary_91", "__E_hexadecimal_92", "__E_decimal_93", "ctn_neq", "__E_constraint_94", "ctnvar", "num", "ctn_eq", 
	"__E_constraint_95", "ctn_gteq", "__E_constraint_96", "ctn_gt", "__E_constraint_97", "ctn_lteq", "__E_constraint_98", "ctn_lt", "__E_constraint_99", "__E_bf_constant_100", 
	"source", "__E_bf_constant_101", "src_c", "__E_source_102", "__E___E_source_102_103", "__E___E_source_102_104", "__E_src_c_105", "__E_src_c_106", "__E_variable_107", "uconst", 
	"io_var", "var_name", "__E_variable_108", "__E_var_name_109", "__E_var_name_110", "__E_var_name_111", "__E_var_name_112", "__E_var_name_113", "offset", "chars", 
	"uconst_name", "__E_uconst_114", "__E___E_uconst_114_115", "q_var", "__E_q_vars_116", "__E_q_vars_117", "__E_offsets_118", "__E_offsets_119", "integer", "shift", 
	"__E_offset_120", "__E_shift_121", "__E___E_shift_121_122", "__E_chars_123", "__E_chars_124", "digits", "__E_digits_125", "__E_integer_126", "type", "__E_typed_127", 
	"subtype", "scope_id", "comment", "__E_comment_128", "__E_comment_129", "__E_comment_130", "cli", "cli_command", "__E_cli_131", "__E_cli_132", 
	"quit_cmd", "quit_sym", "__E_cli_command_133", "version_cmd", "version_sym", "__E_cli_command_134", "clear_cmd", "clear_sym", "__E_cli_command_135", "help_cmd", 
	"__E_cli_command_136", "help_sym", "__E___E_cli_command_136_137", "__E___E_cli_command_136_138", "help_arg", "file_cmd", "__E_cli_command_139", "file_sym", "q_string", "valid_cmd", 
	"__E_cli_command_140", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_141", "sat_sym", "unsat_cmd", "__E_cli_command_142", "unsat_sym", "solve_cmd", 
	"__E_cli_command_143", "solve_sym", "__E___E_cli_command_143_144", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_145", "lgrs_sym", "run_cmd", "__E_cli_command_146", 
	"run_sym", "__E___E_cli_command_146_147", "__E___E_cli_command_146_148", "history", "normalize_cmd", "__E_cli_command_149", "normalize_sym", "__E___E_cli_command_149_150", "subst_cmd", "__E_cli_command_151", 
	"subst_sym", "__E___E_cli_command_151_152", "nf_cmd_arg", "inst_cmd", "__E_cli_command_153", "inst_sym", "__E___E_cli_command_153_154", "inst_args", "dnf_cmd", "__E_cli_command_155", 
	"dnf_sym", "cnf_cmd", "__E_cli_command_156", "cnf_sym", "anf_cmd", "__E_cli_command_157", "anf_sym", "nnf_cmd", "__E_cli_command_158", "nnf_sym", 
	"pnf_cmd", "__E_cli_command_159", "pnf_sym", "mnf_cmd", "__E_cli_command_160", "mnf_sym", "onf_cmd", "__E_cli_command_161", "onf_sym", "onf_cmd_arg", 
	"qelim_cmd", "__E_cli_command_162", "qelim_sym", "get_cmd", "__E_cli_command_163", "get_sym", "__E___E_cli_command_163_164", "option_name", "set_cmd", "__E_cli_command_165", 
	"set_sym", "__E___E_cli_command_165_166", "option_value", "enable_cmd", "__E_cli_command_167", "enable_sym", "disable_cmd", "__E_cli_command_168", "disable_sym", "toggle_cmd", 
	"__E_cli_command_169", "toggle_sym", "def_list_cmd", "__E_cli_command_170", "def_sym", "__E___E_cli_command_170_171", "def_print_cmd", "__E_cli_command_172", "def_rr_cmd", "def_input_cmd", 
	"def_output_cmd", "history_list_cmd", "__E_cli_command_173", "history_sym", "__E___E_cli_command_173_174", "history_print_cmd", "__E_cli_command_175", "history_store_cmd", "__E_cli_command_176", "__E_solve_options_177", 
	"solve_option", "__E_solve_options_178", "__E_solve_option_179", "solver_mode", "solver_mode_minimum", "__E_solver_mode_180", "solver_mode_maximum", "__E_solver_mode_181", "file_name", "__E_file_name_182", 
	"__E_option_name_183", "__E_option_value_184", "bf_cmd_arg", "abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", "snf_sym", "examples_sym", "__E_help_arg_185", 
	"__E___E_help_arg_185_186", "rel_history", "__E_history_187", "__E___E_history_187_188", "history_id", "abs_history", "__E_history_189", "__N_0", "__N_1", "__N_2", 
	"__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', 'a', 'b', 'k', 
	'r', ',', 'm', '<', '>', 'w', 'y', '[', ']', '?', 
	'x', '-', '|', '^', '&', '!', 'T', 'F', '_', '+', 
	'1', '0', '\'', '*', '/', '%', '~', '#', '{', '}', 
	'$', '\t', '\n', '\r', 'q', 'v', 'h', 'p', 'd', 'g', 
	'z', '"', 
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
			11, 34
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			29, 31, 40, 205, 207, 209, 217, 230, 241, 250,
			258, 265, 268, 270, 367, 372, 408
		},
		.to_inline = {
			{ 14, 61, 14 },
			{ 30, 137, 30 },
			{ 33 },
			{ 118, 169, 118 },
			{ 161, 137, 30 },
			{ 163, 137, 156 },
			{ 165, 137, 30 },
			{ 167, 137, 30 },
			{ 232 },
			{ 239 },
			{ 239, 250 },
			{ 249 },
			{ 253 },
			{ 302 },
			{ 314 },
			{ 332 },
			{ 337 },
			{ 359 },
			{ 412 }
		},
		.inline_char_classes = true
	},
	.enabled_guards = {
		"charvar"
	}
};

inline ::idni::parser<char_type, terminal_type>::options parser_options{
};

inline ::idni::prods<char_type, terminal_type> start_symbol{ nts(9) };

inline idni::prods<char_type, terminal_type>& productions() {
	static bool loaded = false;
	static idni::prods<char_type, terminal_type>
		p, nul(idni::lit<char_type, terminal_type>{});
	if (loaded) return p;
	#define  T(x) (idni::prods<char_type, terminal_type>{ terminals[x] })
	#define NT(x) (idni::prods<char_type, terminal_type>{ nts(x) })
//G0:   start(9)             => spec(10) _(11).
	p(NT(9), (NT(10)+NT(11)));
//G1:   main(13)             => wff(14).
	p(NT(13), (NT(14)));
//G2:   spec(10)             => definitions(12) _(11) main(13) _(11) '.'.
	p(NT(10), (NT(12)+NT(11)+NT(13)+NT(11)+T(1)));
//G3:   __E_spec_multiline_0(16) => rec_relation(17).
	p(NT(16), (NT(17)));
//G4:   __E_spec_multiline_0(16) => input_def(18).
	p(NT(16), (NT(18)));
//G5:   __E_spec_multiline_0(16) => output_def(19).
	p(NT(16), (NT(19)));
//G6:   __E_spec_multiline_0(16) => main(13).
	p(NT(16), (NT(13)));
//G7:   __E_spec_multiline_1(20) => _(11) '.'.
	p(NT(20), (NT(11)+T(1)));
//G8:   __E_spec_multiline_1(20) => null.
	p(NT(20), (nul));
//G9:   spec_multiline(15)   => _(11) __E_spec_multiline_0(16) __E_spec_multiline_1(20) _(11).
	p(NT(15), (NT(11)+NT(16)+NT(20)+NT(11)));
//G10:  spec_multiline(15)   => _(11).
	p(NT(15), (NT(11)));
//G11:  __E___E_definitions_2_3(22) => rec_relation(17).
	p(NT(22), (NT(17)));
//G12:  __E___E_definitions_2_3(22) => input_def(18).
	p(NT(22), (NT(18)));
//G13:  __E___E_definitions_2_3(22) => output_def(19).
	p(NT(22), (NT(19)));
//G14:  __E_definitions_2(21) => _(11) __E___E_definitions_2_3(22) _(11) '.'.
	p(NT(21), (NT(11)+NT(22)+NT(11)+T(1)));
//G15:  __E_definitions_4(23) => null.
	p(NT(23), (nul));
//G16:  __E_definitions_4(23) => __E_definitions_2(21) __E_definitions_4(23).
	p(NT(23), (NT(21)+NT(23)));
//G17:  definitions(12)      => __E_definitions_4(23).
	p(NT(12), (NT(23)));
//G18:  __E_rec_relations_5(25) => _(11) rec_relation(17) _(11) '.'.
	p(NT(25), (NT(11)+NT(17)+NT(11)+T(1)));
//G19:  __E_rec_relations_6(26) => null.
	p(NT(26), (nul));
//G20:  __E_rec_relations_6(26) => __E_rec_relations_5(25) __E_rec_relations_6(26).
	p(NT(26), (NT(25)+NT(26)));
//G21:  rec_relations(24)    => __E_rec_relations_6(26).
	p(NT(24), (NT(26)));
//G22:  __E_rec_relation_7(28) => capture(29).
	p(NT(28), (NT(29)));
//G23:  __E_rec_relation_7(28) => ref(27).
	p(NT(28), (NT(27)));
//G24:  __E_rec_relation_7(28) => wff(14).
	p(NT(28), (NT(14)));
//G25:  __E_rec_relation_7(28) => bf(30).
	p(NT(28), (NT(30)));
//G26:  rec_relation(17)     => ref(27) _(11) ':' '=' _(11) __E_rec_relation_7(28).
	p(NT(17), (NT(27)+NT(11)+T(2)+T(3)+NT(11)+NT(28)));
//G27:  __E_input_def_8(32)  => typed(33).
	p(NT(32), (NT(33)));
//G28:  __E_input_def_8(32)  => null.
	p(NT(32), (nul));
//G29:  input_def(18)        => io_var_name(31) __E_input_def_8(32) _(11) '=' _(11) 'i' 'n' __(34) stream(35).
	p(NT(18), (NT(31)+NT(32)+NT(11)+T(3)+NT(11)+T(4)+T(5)+NT(34)+NT(35)));
//G30:  __E_output_def_9(36) => typed(33).
	p(NT(36), (NT(33)));
//G31:  __E_output_def_9(36) => null.
	p(NT(36), (nul));
//G32:  __E_output_def_10(37) => __(34) stream(35).
	p(NT(37), (NT(34)+NT(35)));
//G33:  __E_output_def_10(37) => null.
	p(NT(37), (nul));
//G34:  output_def(19)       => io_var_name(31) __E_output_def_9(36) _(11) '=' _(11) 'o' 'u' 't' __E_output_def_10(37).
	p(NT(19), (NT(31)+NT(36)+NT(11)+T(3)+NT(11)+T(6)+T(7)+T(8)+NT(37)));
//G35:  stream(35)           => 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(38) _(11) ')'.
	p(NT(35), (T(9)+T(4)+T(10)+T(11)+NT(11)+T(12)+NT(11)+NT(38)+NT(11)+T(13)));
//G36:  console_sym(39)      => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(39), (T(14)+T(6)+T(5)+T(15)+T(6)+T(10)+T(11)));
//G37:  stream(35)           => console_sym(39).
	p(NT(35), (NT(39)));
//G38:  __E_ref_11(41)       => offsets(42).
	p(NT(41), (NT(42)));
//G39:  __E_ref_11(41)       => null.
	p(NT(41), (nul));
//G40:  __E_ref_12(44)       => _(11) 'f' 'a' 'l' 'l' 'b' 'a' 'c' 'k' __(34) fp_fallback(45).
	p(NT(44), (NT(11)+T(9)+T(16)+T(10)+T(10)+T(17)+T(16)+T(14)+T(18)+NT(34)+NT(45)));
//G41:  __E_ref_12(44)       => null.
	p(NT(44), (nul));
//G42:  ref(27)              => sym(40) __E_ref_11(41) ref_args(43) __E_ref_12(44).
	p(NT(27), (NT(40)+NT(41)+NT(43)+NT(44)));
//G43:  first_sym(46)        => 'f' 'i' 'r' 's' 't'.
	p(NT(46), (T(9)+T(4)+T(19)+T(15)+T(8)));
//G44:  fp_fallback(45)      => first_sym(46).
	p(NT(45), (NT(46)));
//G45:  last_sym(47)         => 'l' 'a' 's' 't'.
	p(NT(47), (T(10)+T(16)+T(15)+T(8)));
//G46:  fp_fallback(45)      => last_sym(47).
	p(NT(45), (NT(47)));
//G47:  fp_fallback(45)      => capture(29).
	p(NT(45), (NT(29)));
//G48:  fp_fallback(45)      => ref(27).
	p(NT(45), (NT(27)));
//G49:  fp_fallback(45)      => wff(14).
	p(NT(45), (NT(14)));
//G50:  fp_fallback(45)      => bf(30).
	p(NT(45), (NT(30)));
//G51:  ref_arg(50)          => bf(30).
	p(NT(50), (NT(30)));
//G52:  __E___E_ref_args_13_14(49) => _(11) ref_arg(50).
	p(NT(49), (NT(11)+NT(50)));
//G53:  __E___E_ref_args_13_15(51) => _(11) ',' _(11) ref_arg(50).
	p(NT(51), (NT(11)+T(20)+NT(11)+NT(50)));
//G54:  __E___E_ref_args_13_16(52) => null.
	p(NT(52), (nul));
//G55:  __E___E_ref_args_13_16(52) => __E___E_ref_args_13_15(51) __E___E_ref_args_13_16(52).
	p(NT(52), (NT(51)+NT(52)));
//G56:  __E_ref_args_13(48)  => __E___E_ref_args_13_14(49) __E___E_ref_args_13_16(52).
	p(NT(48), (NT(49)+NT(52)));
//G57:  __E_ref_args_13(48)  => null.
	p(NT(48), (nul));
//G58:  ref_args(43)         => '(' __E_ref_args_13(48) _(11) ')'.
	p(NT(43), (T(12)+NT(48)+NT(11)+T(13)));
//G59:  wff_matcher(54)      => wff(14).
	p(NT(54), (NT(14)));
//G60:  wff_body(55)         => wff(14).
	p(NT(55), (NT(14)));
//G61:  wff_rule(53)         => wff_matcher(54) _(11) ':' ':' '=' _(11) wff_body(55) _(11) '.'.
	p(NT(53), (NT(54)+NT(11)+T(2)+T(2)+T(3)+NT(11)+NT(55)+NT(11)+T(1)));
//G62:  bf_matcher(57)       => bf(30).
	p(NT(57), (NT(30)));
//G63:  bf_body(58)          => bf(30).
	p(NT(58), (NT(30)));
//G64:  bf_rule(56)          => bf_matcher(57) _(11) ':' '=' _(11) bf_body(58) _(11) '.'.
	p(NT(56), (NT(57)+NT(11)+T(2)+T(3)+NT(11)+NT(58)+NT(11)+T(1)));
//G65:  __E_tau_constant_source_17(60) => '.' _(11).
	p(NT(60), (T(1)+NT(11)));
//G66:  __E_tau_constant_source_17(60) => null.
	p(NT(60), (nul));
//G67:  tau_constant_source(59) => rec_relations(24) _(11) main(13) _(11) __E_tau_constant_source_17(60).
	p(NT(59), (NT(24)+NT(11)+NT(13)+NT(11)+NT(60)));
//G68:  __E_wff_18(62)       => '(' _(11) wff(14) _(11) ')'.
	p(NT(62), (T(12)+NT(11)+NT(14)+NT(11)+T(13)));
//G69:  wff_parenthesis(61)  => __E_wff_18(62).
	p(NT(61), (NT(62)));
//G70:  wff(14)              => wff_parenthesis(61).
	p(NT(14), (NT(61)));
//G71:  __E___E_wff_19_20(65) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(65), (T(15)+T(6)+T(21)+T(11)+T(8)+T(4)+T(21)+T(11)+T(15)));
//G72:  __E___E_wff_19_20(65) => '<' '>'.
	p(NT(65), (T(22)+T(23)));
//G73:  __E_wff_19(64)       => __E___E_wff_19_20(65) _(11) wff(14).
	p(NT(64), (NT(65)+NT(11)+NT(14)));
//G74:  wff_sometimes(63)    => __E_wff_19(64).
	p(NT(63), (NT(64)));
//G75:  wff(14)              => wff_sometimes(63).
	p(NT(14), (NT(63)));
//G76:  __E___E_wff_21_22(68) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(68), (T(16)+T(10)+T(24)+T(16)+T(25)+T(15)));
//G77:  __E___E_wff_21_22(68) => '[' ']'.
	p(NT(68), (T(26)+T(27)));
//G78:  __E_wff_21(67)       => __E___E_wff_21_22(68) _(11) wff(14).
	p(NT(67), (NT(68)+NT(11)+NT(14)));
//G79:  wff_always(66)       => __E_wff_21(67).
	p(NT(66), (NT(67)));
//G80:  wff(14)              => wff_always(66).
	p(NT(14), (NT(66)));
//G81:  __E_wff_23(70)       => wff(14) _(11) '?' _(11) wff(14) _(11) ':' _(11) wff(14).
	p(NT(70), (NT(14)+NT(11)+T(28)+NT(11)+NT(14)+NT(11)+T(2)+NT(11)+NT(14)));
//G82:  wff_conditional(69)  => __E_wff_23(70).
	p(NT(69), (NT(70)));
//G83:  wff(14)              => wff_conditional(69).
	p(NT(14), (NT(69)));
//G84:  __E_wff_24(72)       => 'a' 'l' 'l' __(34) q_vars(73) __(34) wff(14).
	p(NT(72), (T(16)+T(10)+T(10)+NT(34)+NT(73)+NT(34)+NT(14)));
//G85:  wff_all(71)          => __E_wff_24(72).
	p(NT(71), (NT(72)));
//G86:  wff(14)              => wff_all(71).
	p(NT(14), (NT(71)));
//G87:  __E_wff_25(75)       => 'e' 'x' __(34) q_vars(73) __(34) wff(14).
	p(NT(75), (T(11)+T(29)+NT(34)+NT(73)+NT(34)+NT(14)));
//G88:  wff_ex(74)           => __E_wff_25(75).
	p(NT(74), (NT(75)));
//G89:  wff(14)              => wff_ex(74).
	p(NT(14), (NT(74)));
//G90:  wff_ref(76)          => ref(27).
	p(NT(76), (NT(27)));
//G91:  wff(14)              => wff_ref(76).
	p(NT(14), (NT(76)));
//G92:  wff(14)              => constraint(77).
	p(NT(14), (NT(77)));
//G93:  __E_wff_26(79)       => wff(14) _(11) '-' '>' _(11) wff(14).
	p(NT(79), (NT(14)+NT(11)+T(30)+T(23)+NT(11)+NT(14)));
//G94:  wff_imply(78)        => __E_wff_26(79).
	p(NT(78), (NT(79)));
//G95:  wff(14)              => wff_imply(78).
	p(NT(14), (NT(78)));
//G96:  __E_wff_27(81)       => wff(14) _(11) '<' '-' _(11) wff(14).
	p(NT(81), (NT(14)+NT(11)+T(22)+T(30)+NT(11)+NT(14)));
//G97:  wff_rimply(80)       => __E_wff_27(81).
	p(NT(80), (NT(81)));
//G98:  wff(14)              => wff_rimply(80).
	p(NT(14), (NT(80)));
//G99:  __E_wff_28(83)       => wff(14) _(11) '<' '-' '>' _(11) wff(14).
	p(NT(83), (NT(14)+NT(11)+T(22)+T(30)+T(23)+NT(11)+NT(14)));
//G100: wff_equiv(82)        => __E_wff_28(83).
	p(NT(82), (NT(83)));
//G101: wff(14)              => wff_equiv(82).
	p(NT(14), (NT(82)));
//G102: __E_wff_29(85)       => wff(14) _(11) '|' '|' _(11) wff(14).
	p(NT(85), (NT(14)+NT(11)+T(31)+T(31)+NT(11)+NT(14)));
//G103: wff_or(84)           => __E_wff_29(85).
	p(NT(84), (NT(85)));
//G104: wff(14)              => wff_or(84).
	p(NT(14), (NT(84)));
//G105: __E_wff_30(87)       => wff(14) _(11) '^' _(11) wff(14).
	p(NT(87), (NT(14)+NT(11)+T(32)+NT(11)+NT(14)));
//G106: wff_xor(86)          => __E_wff_30(87).
	p(NT(86), (NT(87)));
//G107: wff(14)              => wff_xor(86).
	p(NT(14), (NT(86)));
//G108: __E_wff_31(89)       => wff(14) _(11) '&' '&' _(11) wff(14).
	p(NT(89), (NT(14)+NT(11)+T(33)+T(33)+NT(11)+NT(14)));
//G109: wff_and(88)          => __E_wff_31(89).
	p(NT(88), (NT(89)));
//G110: wff(14)              => wff_and(88).
	p(NT(14), (NT(88)));
//G111: __E_wff_32(91)       => '!' _(11) wff(14).
	p(NT(91), (T(34)+NT(11)+NT(14)));
//G112: wff_neg(90)          => __E_wff_32(91).
	p(NT(90), (NT(91)));
//G113: wff(14)              => wff_neg(90).
	p(NT(14), (NT(90)));
//G114: wff_t(92)            => 'T'.
	p(NT(92), (T(35)));
//G115: wff(14)              => wff_t(92).
	p(NT(14), (NT(92)));
//G116: wff_f(93)            => 'F'.
	p(NT(93), (T(36)));
//G117: wff(14)              => wff_f(93).
	p(NT(14), (NT(93)));
//G118: __E_wff_33(95)       => bf(30) _(11) '<' '=' _(11) bf(30) _(11) '<' '=' _(11) bf(30).
	p(NT(95), (NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)));
//G119: bf_interval(94)      => __E_wff_33(95).
	p(NT(94), (NT(95)));
//G120: wff(14)              => bf_interval(94).
	p(NT(14), (NT(94)));
//G121: __E_wff_34(97)       => bf(30) _(11) '=' _(11) bf(30).
	p(NT(97), (NT(30)+NT(11)+T(3)+NT(11)+NT(30)));
//G122: bf_eq(96)            => __E_wff_34(97).
	p(NT(96), (NT(97)));
//G123: wff(14)              => bf_eq(96).
	p(NT(14), (NT(96)));
//G124: __E_wff_35(99)       => bf(30) _(11) '!' '=' _(11) bf(30).
	p(NT(99), (NT(30)+NT(11)+T(34)+T(3)+NT(11)+NT(30)));
//G125: bf_neq(98)           => __E_wff_35(99).
	p(NT(98), (NT(99)));
//G126: wff(14)              => bf_neq(98).
	p(NT(14), (NT(98)));
//G127: __E_wff_36(101)      => bf(30) _(11) '<' '=' _(11) bf(30).
	p(NT(101), (NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)));
//G128: bf_lteq(100)         => __E_wff_36(101).
	p(NT(100), (NT(101)));
//G129: wff(14)              => bf_lteq(100).
	p(NT(14), (NT(100)));
//G130: __E_wff_37(103)      => bf(30) _(11) '!' '<' '=' _(11) bf(30).
	p(NT(103), (NT(30)+NT(11)+T(34)+T(22)+T(3)+NT(11)+NT(30)));
//G131: bf_nlteq(102)        => __E_wff_37(103).
	p(NT(102), (NT(103)));
//G132: wff(14)              => bf_nlteq(102).
	p(NT(14), (NT(102)));
//G133: __E_wff_38(105)      => bf(30) _(11) '>' _(11) bf(30).
	p(NT(105), (NT(30)+NT(11)+T(23)+NT(11)+NT(30)));
//G134: bf_gt(104)           => __E_wff_38(105).
	p(NT(104), (NT(105)));
//G135: wff(14)              => bf_gt(104).
	p(NT(14), (NT(104)));
//G136: __E_wff_39(107)      => bf(30) _(11) '!' '>' _(11) bf(30).
	p(NT(107), (NT(30)+NT(11)+T(34)+T(23)+NT(11)+NT(30)));
//G137: bf_ngt(106)          => __E_wff_39(107).
	p(NT(106), (NT(107)));
//G138: wff(14)              => bf_ngt(106).
	p(NT(14), (NT(106)));
//G139: __E_wff_40(109)      => bf(30) _(11) '>' '=' _(11) bf(30).
	p(NT(109), (NT(30)+NT(11)+T(23)+T(3)+NT(11)+NT(30)));
//G140: bf_gteq(108)         => __E_wff_40(109).
	p(NT(108), (NT(109)));
//G141: wff(14)              => bf_gteq(108).
	p(NT(14), (NT(108)));
//G142: __E_wff_41(111)      => bf(30) _(11) '!' '>' '=' _(11) bf(30).
	p(NT(111), (NT(30)+NT(11)+T(34)+T(23)+T(3)+NT(11)+NT(30)));
//G143: bf_ngteq(110)        => __E_wff_41(111).
	p(NT(110), (NT(111)));
//G144: wff(14)              => bf_ngteq(110).
	p(NT(14), (NT(110)));
//G145: __E_wff_42(113)      => bf(30) _(11) '<' _(11) bf(30).
	p(NT(113), (NT(30)+NT(11)+T(22)+NT(11)+NT(30)));
//G146: bf_lt(112)           => __E_wff_42(113).
	p(NT(112), (NT(113)));
//G147: wff(14)              => bf_lt(112).
	p(NT(14), (NT(112)));
//G148: __E_wff_43(115)      => bf(30) _(11) '!' '<' _(11) bf(30).
	p(NT(115), (NT(30)+NT(11)+T(34)+T(22)+NT(11)+NT(30)));
//G149: bf_nlt(114)          => __E_wff_43(115).
	p(NT(114), (NT(115)));
//G150: wff(14)              => bf_nlt(114).
	p(NT(14), (NT(114)));
//G151: __E_wff_44(117)      => bv(118) _(11) '=' '_' _(11) bv(118).
	p(NT(117), (NT(118)+NT(11)+T(3)+T(37)+NT(11)+NT(118)));
//G152: bv_eq(116)           => __E_wff_44(117).
	p(NT(116), (NT(117)));
//G153: wff(14)              => bv_eq(116).
	p(NT(14), (NT(116)));
//G154: __E_wff_45(120)      => bv(118) _(11) '!' '=' '_' _(11) bv(118).
	p(NT(120), (NT(118)+NT(11)+T(34)+T(3)+T(37)+NT(11)+NT(118)));
//G155: bv_neq(119)          => __E_wff_45(120).
	p(NT(119), (NT(120)));
//G156: wff(14)              => bv_neq(119).
	p(NT(14), (NT(119)));
//G157: __E_wff_46(122)      => bv(118) _(11) '<' '=' '_' _(11) bv(118).
	p(NT(122), (NT(118)+NT(11)+T(22)+T(3)+T(37)+NT(11)+NT(118)));
//G158: bv_lteq(121)         => __E_wff_46(122).
	p(NT(121), (NT(122)));
//G159: wff(14)              => bv_lteq(121).
	p(NT(14), (NT(121)));
//G160: __E_wff_47(124)      => bv(118) _(11) '!' '<' '=' '_' _(11) bv(118).
	p(NT(124), (NT(118)+NT(11)+T(34)+T(22)+T(3)+T(37)+NT(11)+NT(118)));
//G161: bv_nlteq(123)        => __E_wff_47(124).
	p(NT(123), (NT(124)));
//G162: wff(14)              => bv_nlteq(123).
	p(NT(14), (NT(123)));
//G163: __E_wff_48(126)      => bv(118) _(11) '>' '_' _(11) bv(118).
	p(NT(126), (NT(118)+NT(11)+T(23)+T(37)+NT(11)+NT(118)));
//G164: bv_gt(125)           => __E_wff_48(126).
	p(NT(125), (NT(126)));
//G165: wff(14)              => bv_gt(125).
	p(NT(14), (NT(125)));
//G166: __E_wff_49(128)      => bv(118) _(11) '!' '>' '_' _(11) bv(118).
	p(NT(128), (NT(118)+NT(11)+T(34)+T(23)+T(37)+NT(11)+NT(118)));
//G167: bv_ngt(127)          => __E_wff_49(128).
	p(NT(127), (NT(128)));
//G168: wff(14)              => bv_ngt(127).
	p(NT(14), (NT(127)));
//G169: __E_wff_50(130)      => bv(118) _(11) '>' '=' '_' _(11) bv(118).
	p(NT(130), (NT(118)+NT(11)+T(23)+T(3)+T(37)+NT(11)+NT(118)));
//G170: bv_gteq(129)         => __E_wff_50(130).
	p(NT(129), (NT(130)));
//G171: wff(14)              => bv_gteq(129).
	p(NT(14), (NT(129)));
//G172: __E_wff_51(132)      => bv(118) _(11) '!' '>' '=' '_' _(11) bv(118).
	p(NT(132), (NT(118)+NT(11)+T(34)+T(23)+T(3)+T(37)+NT(11)+NT(118)));
//G173: bv_ngteq(131)        => __E_wff_51(132).
	p(NT(131), (NT(132)));
//G174: wff(14)              => bv_ngteq(131).
	p(NT(14), (NT(131)));
//G175: __E_wff_52(134)      => bv(118) _(11) '<' '_' _(11) bv(118).
	p(NT(134), (NT(118)+NT(11)+T(22)+T(37)+NT(11)+NT(118)));
//G176: bv_lt(133)           => __E_wff_52(134).
	p(NT(133), (NT(134)));
//G177: wff(14)              => bv_lt(133).
	p(NT(14), (NT(133)));
//G178: __E_wff_53(136)      => bv(118) _(11) '!' '<' '_' _(11) bv(118).
	p(NT(136), (NT(118)+NT(11)+T(34)+T(22)+T(37)+NT(11)+NT(118)));
//G179: bv_nlt(135)          => __E_wff_53(136).
	p(NT(135), (NT(136)));
//G180: wff(14)              => bv_nlt(135).
	p(NT(14), (NT(135)));
//G181: wff(14)              => capture(29).
	p(NT(14), (NT(29)));
//G182: __E_bf_54(138)       => '(' _(11) bf(30) _(11) ')'.
	p(NT(138), (T(12)+NT(11)+NT(30)+NT(11)+T(13)));
//G183: bf_parenthesis(137)  => __E_bf_54(138).
	p(NT(137), (NT(138)));
//G184: bf(30)               => bf_parenthesis(137).
	p(NT(30), (NT(137)));
//G185: bf(30)               => bf_constant(139).
	p(NT(30), (NT(139)));
//G186: bf(30)               => variable(140).
	p(NT(30), (NT(140)));
//G187: __E_bf_55(142)       => 'f' 'a' 'l' 'l' __(34) q_vars(73) __(34) bf(30).
	p(NT(142), (T(9)+T(16)+T(10)+T(10)+NT(34)+NT(73)+NT(34)+NT(30)));
//G188: bf_fall(141)         => __E_bf_55(142).
	p(NT(141), (NT(142)));
//G189: bf(30)               => bf_fall(141).
	p(NT(30), (NT(141)));
//G190: __E_bf_56(144)       => 'f' 'e' 'x' __(34) q_vars(73) __(34) bf(30).
	p(NT(144), (T(9)+T(11)+T(29)+NT(34)+NT(73)+NT(34)+NT(30)));
//G191: bf_fex(143)          => __E_bf_56(144).
	p(NT(143), (NT(144)));
//G192: bf(30)               => bf_fex(143).
	p(NT(30), (NT(143)));
//G193: bf_ref(145)          => ref(27).
	p(NT(145), (NT(27)));
//G194: bf(30)               => bf_ref(145).
	p(NT(30), (NT(145)));
//G195: __E_bf_57(147)       => bf(30) _(11) '|' _(11) bf(30).
	p(NT(147), (NT(30)+NT(11)+T(31)+NT(11)+NT(30)));
//G196: bf_or(146)           => __E_bf_57(147).
	p(NT(146), (NT(147)));
//G197: bf(30)               => bf_or(146).
	p(NT(30), (NT(146)));
//G198: __E_bf_58(149)       => bf(30) _(11) '+' _(11) bf(30).
	p(NT(149), (NT(30)+NT(11)+T(38)+NT(11)+NT(30)));
//G199: bf_xor(148)          => __E_bf_58(149).
	p(NT(148), (NT(149)));
//G200: bf(30)               => bf_xor(148).
	p(NT(30), (NT(148)));
//G201: __E___E_bf_59_60(152) => typed(33).
	p(NT(152), (NT(33)));
//G202: __E___E_bf_59_60(152) => null.
	p(NT(152), (nul));
//G203: __E_bf_59(151)       => '1' __E___E_bf_59_60(152).
	p(NT(151), (T(39)+NT(152)));
//G204: bf_t(150)            => __E_bf_59(151).
	p(NT(150), (NT(151)));
//G205: bf(30)               => bf_t(150).
	p(NT(30), (NT(150)));
//G206: __E___E_bf_61_62(155) => typed(33).
	p(NT(155), (NT(33)));
//G207: __E___E_bf_61_62(155) => null.
	p(NT(155), (nul));
//G208: __E_bf_61(154)       => '0' __E___E_bf_61_62(155).
	p(NT(154), (T(40)+NT(155)));
//G209: bf_f(153)            => __E_bf_61(154).
	p(NT(153), (NT(154)));
//G210: bf(30)               => bf_f(153).
	p(NT(30), (NT(153)));
//G211: __E___E_bf_63_64(158) => _(11) '&' _(11).
	p(NT(158), (NT(11)+T(33)+NT(11)));
//G212: __E___E_bf_63_64(158) => __(34).
	p(NT(158), (NT(34)));
//G213: __E_bf_63(157)       => bf(30) __E___E_bf_63_64(158) bf(30).
	p(NT(157), (NT(30)+NT(158)+NT(30)));
//G214: bf_and(156)          => __E_bf_63(157).
	p(NT(156), (NT(157)));
//G215: bf(30)               => bf_and(156).
	p(NT(30), (NT(156)));
//G216: __E___E_bf_65_66(162) => bf_parenthesis(137).
	p(NT(162), (NT(137)));
//G217: __E___E_bf_65_66(162) => bf_constant(139).
	p(NT(162), (NT(139)));
//G218: __E___E_bf_65_66(162) => variable(140).
	p(NT(162), (NT(140)));
//G219: __E___E_bf_65_66(162) => bf_ref(145).
	p(NT(162), (NT(145)));
//G220: __E___E_bf_65_66(162) => bf_t(150).
	p(NT(162), (NT(150)));
//G221: __E___E_bf_65_66(162) => bf_f(153).
	p(NT(162), (NT(153)));
//G222: __E___E_bf_65_66(162) => bf_neg(159).
	p(NT(162), (NT(159)));
//G223: __E___E_bf_65_66(162) => capture(29).
	p(NT(162), (NT(29)));
//G224: bf_neg_oprnd(161)    => __E___E_bf_65_66(162).
	p(NT(161), (NT(162)));
//G225: __E_bf_65(160)       => bf_neg_oprnd(161) _(11) '\''.
	p(NT(160), (NT(161)+NT(11)+T(41)));
//G226: bf_neg(159)          => __E_bf_65(160).
	p(NT(159), (NT(160)));
//G227: bf(30)               => bf_neg(159).
	p(NT(30), (NT(159)));
//G228: __E___E_bf_67_68(166) => bf_parenthesis(137).
	p(NT(166), (NT(137)));
//G229: __E___E_bf_67_68(166) => variable(140).
	p(NT(166), (NT(140)));
//G230: __E___E_bf_67_68(166) => bf_ref(145).
	p(NT(166), (NT(145)));
//G231: __E___E_bf_67_68(166) => bf_neg(159).
	p(NT(166), (NT(159)));
//G232: __E___E_bf_67_68(166) => bf_and_nosep(163).
	p(NT(166), (NT(163)));
//G233: __E___E_bf_67_68(166) => capture(29).
	p(NT(166), (NT(29)));
//G234: bf_and_nosep_1st_oprnd(165) => __E___E_bf_67_68(166).
	p(NT(165), (NT(166)));
//G235: __E___E_bf_67_69(168) => bf_parenthesis(137).
	p(NT(168), (NT(137)));
//G236: __E___E_bf_67_69(168) => bf_constant(139).
	p(NT(168), (NT(139)));
//G237: __E___E_bf_67_69(168) => variable(140).
	p(NT(168), (NT(140)));
//G238: __E___E_bf_67_69(168) => bf_ref(145).
	p(NT(168), (NT(145)));
//G239: __E___E_bf_67_69(168) => bf_neg(159).
	p(NT(168), (NT(159)));
//G240: __E___E_bf_67_69(168) => capture(29).
	p(NT(168), (NT(29)));
//G241: bf_and_nosep_2nd_oprnd(167) => __E___E_bf_67_69(168).
	p(NT(167), (NT(168)));
//G242: __E_bf_67(164)       => bf_and_nosep_1st_oprnd(165) bf_and_nosep_2nd_oprnd(167).
	p(NT(164), (NT(165)+NT(167)));
//G243: bf_and_nosep(163)    => __E_bf_67(164).
	p(NT(163), (NT(164)));
//G244: bf(30)               => bf_and_nosep(163).
	p(NT(30), (NT(163)));
//G245: bf(30)               => capture(29).
	p(NT(30), (NT(29)));
//G246: __E_bv_70(170)       => '(' _(11) bv(118) _(11) ')'.
	p(NT(170), (T(12)+NT(11)+NT(118)+NT(11)+T(13)));
//G247: bv_parenthesis(169)  => __E_bv_70(170).
	p(NT(169), (NT(170)));
//G248: bv(118)              => bv_parenthesis(169).
	p(NT(118), (NT(169)));
//G249: __E_bv_71(172)       => '[' bv(118) ']'.
	p(NT(172), (T(26)+NT(118)+T(27)));
//G250: bv_checked(171)      => __E_bv_71(172).
	p(NT(171), (NT(172)));
//G251: bv(118)              => bv_checked(171).
	p(NT(118), (NT(171)));
//G252: bv(118)              => variable(140).
	p(NT(118), (NT(140)));
//G253: __E_bv_72(174)       => bv(118) _(11) '+' _(11) bv(118).
	p(NT(174), (NT(118)+NT(11)+T(38)+NT(11)+NT(118)));
//G254: bv_add(173)          => __E_bv_72(174).
	p(NT(173), (NT(174)));
//G255: bv(118)              => bv_add(173).
	p(NT(118), (NT(173)));
//G256: __E_bv_73(176)       => bv(118) _(11) '*' _(11) bv(118).
	p(NT(176), (NT(118)+NT(11)+T(42)+NT(11)+NT(118)));
//G257: bv_mul(175)          => __E_bv_73(176).
	p(NT(175), (NT(176)));
//G258: bv(118)              => bv_mul(175).
	p(NT(118), (NT(175)));
//G259: __E_bv_74(178)       => bv(118) _(11) '/' _(11) bv(118).
	p(NT(178), (NT(118)+NT(11)+T(43)+NT(11)+NT(118)));
//G260: bv_div(177)          => __E_bv_74(178).
	p(NT(177), (NT(178)));
//G261: bv(118)              => bv_div(177).
	p(NT(118), (NT(177)));
//G262: __E_bv_75(180)       => bv(118) _(11) '%' _(11) bv(118).
	p(NT(180), (NT(118)+NT(11)+T(44)+NT(11)+NT(118)));
//G263: bv_mod(179)          => __E_bv_75(180).
	p(NT(179), (NT(180)));
//G264: bv(118)              => bv_mod(179).
	p(NT(118), (NT(179)));
//G265: __E_bv_76(182)       => bv(118) _(11) '-' _(11) bv(118).
	p(NT(182), (NT(118)+NT(11)+T(30)+NT(11)+NT(118)));
//G266: bv_sub(181)          => __E_bv_76(182).
	p(NT(181), (NT(182)));
//G267: bv(118)              => bv_sub(181).
	p(NT(118), (NT(181)));
//G268: bv_neg(183)          => bv(118).
	p(NT(183), (NT(118)));
//G269: bv(118)              => '~' _(11) bv_neg(183).
	p(NT(118), (T(45)+NT(11)+NT(183)));
//G270: __E_bv_77(185)       => bv(118) _(11) '&' _(11) bv(118).
	p(NT(185), (NT(118)+NT(11)+T(33)+NT(11)+NT(118)));
//G271: bv_and(184)          => __E_bv_77(185).
	p(NT(184), (NT(185)));
//G272: bv(118)              => bv_and(184).
	p(NT(118), (NT(184)));
//G273: __E_bv_78(187)       => bv(118) _(11) '!' '&' _(11) bv(118).
	p(NT(187), (NT(118)+NT(11)+T(34)+T(33)+NT(11)+NT(118)));
//G274: bv_nand(186)         => __E_bv_78(187).
	p(NT(186), (NT(187)));
//G275: bv(118)              => bv_nand(186).
	p(NT(118), (NT(186)));
//G276: __E_bv_79(189)       => bv(118) _(11) '|' _(11) bv(118).
	p(NT(189), (NT(118)+NT(11)+T(31)+NT(11)+NT(118)));
//G277: bv_or(188)           => __E_bv_79(189).
	p(NT(188), (NT(189)));
//G278: bv(118)              => bv_or(188).
	p(NT(118), (NT(188)));
//G279: __E_bv_80(191)       => bv(118) _(11) '!' '|' _(11) bv(118).
	p(NT(191), (NT(118)+NT(11)+T(34)+T(31)+NT(11)+NT(118)));
//G280: bv_nor(190)          => __E_bv_80(191).
	p(NT(190), (NT(191)));
//G281: bv(118)              => bv_nor(190).
	p(NT(118), (NT(190)));
//G282: __E_bv_81(193)       => bv(118) _(11) '(' '+' ')' _(11) bv(118).
	p(NT(193), (NT(118)+NT(11)+T(12)+T(38)+T(13)+NT(11)+NT(118)));
//G283: bv_xor(192)          => __E_bv_81(193).
	p(NT(192), (NT(193)));
//G284: bv(118)              => bv_xor(192).
	p(NT(118), (NT(192)));
//G285: __E_bv_82(195)       => bv(118) _(11) '!' '(' '+' ')' _(11) bv(118).
	p(NT(195), (NT(118)+NT(11)+T(34)+T(12)+T(38)+T(13)+NT(11)+NT(118)));
//G286: bv_xnor(194)         => __E_bv_82(195).
	p(NT(194), (NT(195)));
//G287: bv(118)              => bv_xnor(194).
	p(NT(118), (NT(194)));
//G288: __E_bv_83(197)       => bv(118) _(11) '<' '<' _(11) bv(118).
	p(NT(197), (NT(118)+NT(11)+T(22)+T(22)+NT(11)+NT(118)));
//G289: bv_shl(196)          => __E_bv_83(197).
	p(NT(196), (NT(197)));
//G290: bv(118)              => bv_shl(196).
	p(NT(118), (NT(196)));
//G291: __E_bv_84(199)       => bv(118) _(11) '>' '>' _(11) bv(118).
	p(NT(199), (NT(118)+NT(11)+T(23)+T(23)+NT(11)+NT(118)));
//G292: bv_shr(198)          => __E_bv_84(199).
	p(NT(198), (NT(199)));
//G293: bv(118)              => bv_shr(198).
	p(NT(118), (NT(198)));
//G294: __E_bv_85(201)       => 'm' 'i' 'n' __(34) bv(118) __(34) bv(118).
	p(NT(201), (T(21)+T(4)+T(5)+NT(34)+NT(118)+NT(34)+NT(118)));
//G295: bv_min(200)          => __E_bv_85(201).
	p(NT(200), (NT(201)));
//G296: bv(118)              => bv_min(200).
	p(NT(118), (NT(200)));
//G297: __E_bv_86(203)       => 'm' 'a' 'x' __(34) bv(118) __(34) bv(118).
	p(NT(203), (T(21)+T(16)+T(29)+NT(34)+NT(118)+NT(34)+NT(118)));
//G298: bv_max(202)          => __E_bv_86(203).
	p(NT(202), (NT(203)));
//G299: bv(118)              => bv_max(202).
	p(NT(118), (NT(202)));
//G300: bv(118)              => bv_constant(204).
	p(NT(118), (NT(204)));
//G301: __E_bv_constant_87(206) => typed(33).
	p(NT(206), (NT(33)));
//G302: __E_bv_constant_87(206) => null.
	p(NT(206), (nul));
//G303: bv_constant(204)     => '#' 'b' binary(205) __E_bv_constant_87(206).
	p(NT(204), (T(46)+T(17)+NT(205)+NT(206)));
//G304: __E_bv_constant_88(208) => typed(33).
	p(NT(208), (NT(33)));
//G305: __E_bv_constant_88(208) => null.
	p(NT(208), (nul));
//G306: bv_constant(204)     => decimal(207) __E_bv_constant_88(208).
	p(NT(204), (NT(207)+NT(208)));
//G307: __E_bv_constant_89(210) => typed(33).
	p(NT(210), (NT(33)));
//G308: __E_bv_constant_89(210) => null.
	p(NT(210), (nul));
//G309: bv_constant(204)     => '#' 'x' hexadecimal(209) __E_bv_constant_89(210).
	p(NT(204), (T(46)+T(29)+NT(209)+NT(210)));
//G310: __E_binary_90(211)   => '0'.
	p(NT(211), (T(40)));
//G311: __E_binary_90(211)   => '1'.
	p(NT(211), (T(39)));
//G312: __E_binary_91(212)   => __E_binary_90(211).
	p(NT(212), (NT(211)));
//G313: __E_binary_91(212)   => __E_binary_90(211) __E_binary_91(212).
	p(NT(212), (NT(211)+NT(212)));
//G314: binary(205)          => __E_binary_91(212).
	p(NT(205), (NT(212)));
//G315: __E_hexadecimal_92(213) => xdigit(4).
	p(NT(213), (NT(4)));
//G316: __E_hexadecimal_92(213) => xdigit(4) __E_hexadecimal_92(213).
	p(NT(213), (NT(4)+NT(213)));
//G317: hexadecimal(209)     => __E_hexadecimal_92(213).
	p(NT(209), (NT(213)));
//G318: __E_decimal_93(214)  => digit(3).
	p(NT(214), (NT(3)));
//G319: __E_decimal_93(214)  => digit(3) __E_decimal_93(214).
	p(NT(214), (NT(3)+NT(214)));
//G320: decimal(207)         => __E_decimal_93(214).
	p(NT(207), (NT(214)));
//G321: __E_constraint_94(216) => '[' ctnvar(217) _(11) '!' '=' _(11) num(218) ']'.
	p(NT(216), (T(26)+NT(217)+NT(11)+T(34)+T(3)+NT(11)+NT(218)+T(27)));
//G322: __E_constraint_94(216) => '[' num(218) _(11) '!' '=' _(11) ctnvar(217) ']'.
	p(NT(216), (T(26)+NT(218)+NT(11)+T(34)+T(3)+NT(11)+NT(217)+T(27)));
//G323: ctn_neq(215)         => __E_constraint_94(216).
	p(NT(215), (NT(216)));
//G324: constraint(77)       => ctn_neq(215).
	p(NT(77), (NT(215)));
//G325: __E_constraint_95(220) => '[' ctnvar(217) _(11) '=' _(11) num(218) ']'.
	p(NT(220), (T(26)+NT(217)+NT(11)+T(3)+NT(11)+NT(218)+T(27)));
//G326: __E_constraint_95(220) => '[' num(218) _(11) '=' _(11) ctnvar(217) ']'.
	p(NT(220), (T(26)+NT(218)+NT(11)+T(3)+NT(11)+NT(217)+T(27)));
//G327: ctn_eq(219)          => __E_constraint_95(220).
	p(NT(219), (NT(220)));
//G328: constraint(77)       => ctn_eq(219).
	p(NT(77), (NT(219)));
//G329: __E_constraint_96(222) => '[' ctnvar(217) _(11) '>' '=' _(11) num(218) ']'.
	p(NT(222), (T(26)+NT(217)+NT(11)+T(23)+T(3)+NT(11)+NT(218)+T(27)));
//G330: __E_constraint_96(222) => '[' num(218) _(11) '>' '=' _(11) ctnvar(217) ']'.
	p(NT(222), (T(26)+NT(218)+NT(11)+T(23)+T(3)+NT(11)+NT(217)+T(27)));
//G331: ctn_gteq(221)        => __E_constraint_96(222).
	p(NT(221), (NT(222)));
//G332: constraint(77)       => ctn_gteq(221).
	p(NT(77), (NT(221)));
//G333: __E_constraint_97(224) => '[' ctnvar(217) _(11) '>' _(11) num(218) ']'.
	p(NT(224), (T(26)+NT(217)+NT(11)+T(23)+NT(11)+NT(218)+T(27)));
//G334: __E_constraint_97(224) => '[' num(218) _(11) '>' _(11) ctnvar(217) ']'.
	p(NT(224), (T(26)+NT(218)+NT(11)+T(23)+NT(11)+NT(217)+T(27)));
//G335: ctn_gt(223)          => __E_constraint_97(224).
	p(NT(223), (NT(224)));
//G336: constraint(77)       => ctn_gt(223).
	p(NT(77), (NT(223)));
//G337: __E_constraint_98(226) => '[' ctnvar(217) _(11) '<' '=' _(11) num(218) ']'.
	p(NT(226), (T(26)+NT(217)+NT(11)+T(22)+T(3)+NT(11)+NT(218)+T(27)));
//G338: __E_constraint_98(226) => '[' num(218) _(11) '<' '=' _(11) ctnvar(217) ']'.
	p(NT(226), (T(26)+NT(218)+NT(11)+T(22)+T(3)+NT(11)+NT(217)+T(27)));
//G339: ctn_lteq(225)        => __E_constraint_98(226).
	p(NT(225), (NT(226)));
//G340: constraint(77)       => ctn_lteq(225).
	p(NT(77), (NT(225)));
//G341: __E_constraint_99(228) => '[' ctnvar(217) _(11) '<' _(11) num(218) ']'.
	p(NT(228), (T(26)+NT(217)+NT(11)+T(22)+NT(11)+NT(218)+T(27)));
//G342: __E_constraint_99(228) => '[' num(218) _(11) '<' _(11) ctnvar(217) ']'.
	p(NT(228), (T(26)+NT(218)+NT(11)+T(22)+NT(11)+NT(217)+T(27)));
//G343: ctn_lt(227)          => __E_constraint_99(228).
	p(NT(227), (NT(228)));
//G344: constraint(77)       => ctn_lt(227).
	p(NT(77), (NT(227)));
//G345: __E_bf_constant_100(229) => capture(29).
	p(NT(229), (NT(29)));
//G346: __E_bf_constant_100(229) => source(230).
	p(NT(229), (NT(230)));
//G347: __E_bf_constant_101(231) => typed(33).
	p(NT(231), (NT(33)));
//G348: __E_bf_constant_101(231) => null.
	p(NT(231), (nul));
//G349: bf_constant(139)     => '{' _(11) __E_bf_constant_100(229) _(11) '}' __E_bf_constant_101(231).
	p(NT(139), (T(47)+NT(11)+NT(229)+NT(11)+T(48)+NT(231)));
//G350: __E___E_source_102_103(234) => src_c(232).
	p(NT(234), (NT(232)));
//G351: __E___E_source_102_103(234) => space(2).
	p(NT(234), (NT(2)));
//G352: __E___E_source_102_104(235) => null.
	p(NT(235), (nul));
//G353: __E___E_source_102_104(235) => __E___E_source_102_103(234) __E___E_source_102_104(235).
	p(NT(235), (NT(234)+NT(235)));
//G354: __E_source_102(233)  => __E___E_source_102_104(235) src_c(232).
	p(NT(233), (NT(235)+NT(232)));
//G355: __E_source_102(233)  => null.
	p(NT(233), (nul));
//G356: source(230)          => src_c(232) __E_source_102(233).
	p(NT(230), (NT(232)+NT(233)));
//G357: src_c(232)           => alnum(6).
	p(NT(232), (NT(6)));
//G358: __N_0(427)           => '{'.
	p(NT(427), (T(47)));
//G359: __N_1(428)           => '}'.
	p(NT(428), (T(48)));
//G360: src_c(232)           => ~( __N_0(427) ) & ~( __N_1(428) ) & punct(7).	 # conjunctive
	p(NT(232), ~(NT(427)) & ~(NT(428)) & (NT(7)));
//G361: __E_src_c_105(236)   => src_c(232).
	p(NT(236), (NT(232)));
//G362: __E_src_c_105(236)   => space(2).
	p(NT(236), (NT(2)));
//G363: __E_src_c_106(237)   => null.
	p(NT(237), (nul));
//G364: __E_src_c_106(237)   => __E_src_c_105(236) __E_src_c_106(237).
	p(NT(237), (NT(236)+NT(237)));
//G365: src_c(232)           => '{' __E_src_c_106(237) '}'.
	p(NT(232), (T(47)+NT(237)+T(48)));
//G366: __E_variable_107(238) => uconst(239).
	p(NT(238), (NT(239)));
//G367: __E_variable_107(238) => io_var(240).
	p(NT(238), (NT(240)));
//G368: __E_variable_107(238) => var_name(241).
	p(NT(238), (NT(241)));
//G369: __E_variable_108(242) => typed(33).
	p(NT(242), (NT(33)));
//G370: __E_variable_108(242) => null.
	p(NT(242), (nul));
//G371: variable(140)        => __E_variable_107(238) __E_variable_108(242).
	p(NT(140), (NT(238)+NT(242)));
//G372: __N_2(429)           => 'F'.
	p(NT(429), (T(36)));
//G373: __N_3(430)           => 'T'.
	p(NT(430), (T(35)));
//G374: __E_var_name_109(243) => ~( __N_2(429) ) & ~( __N_3(430) ) & alpha(5).	 # conjunctive
	p(NT(243), ~(NT(429)) & ~(NT(430)) & (NT(5)));
//G375: __E_var_name_110(244) => null.
	p(NT(244), (nul));
//G376: __E_var_name_110(244) => digit(3) __E_var_name_110(244).
	p(NT(244), (NT(3)+NT(244)));
//G377: var_name(241)        => __E_var_name_109(243) __E_var_name_110(244).	 # guarded: charvar
	p(NT(241), (NT(243)+NT(244)));
	p.back().guard = "charvar";
//G378: __N_4(431)           => 'F'.
	p(NT(431), (T(36)));
//G379: __N_5(432)           => 'T'.
	p(NT(432), (T(35)));
//G380: __E_var_name_111(245) => ~( __N_4(431) ) & ~( __N_5(432) ) & alpha(5).	 # conjunctive
	p(NT(245), ~(NT(431)) & ~(NT(432)) & (NT(5)));
//G381: __E_var_name_112(246) => alnum(6).
	p(NT(246), (NT(6)));
//G382: __E_var_name_112(246) => '_'.
	p(NT(246), (T(37)));
//G383: __E_var_name_113(247) => null.
	p(NT(247), (nul));
//G384: __E_var_name_113(247) => __E_var_name_112(246) __E_var_name_113(247).
	p(NT(247), (NT(246)+NT(247)));
//G385: var_name(241)        => __E_var_name_111(245) __E_var_name_113(247).	 # guarded: var
	p(NT(241), (NT(245)+NT(247)));
	p.back().guard = "var";
//G386: io_var(240)          => io_var_name(31) '[' offset(248) ']'.
	p(NT(240), (NT(31)+T(26)+NT(248)+T(27)));
//G387: io_var_name(31)      => chars(249).
	p(NT(31), (NT(249)));
//G388: __E___E_uconst_114_115(252) => chars(249) _(11).
	p(NT(252), (NT(249)+NT(11)));
//G389: __E___E_uconst_114_115(252) => null.
	p(NT(252), (nul));
//G390: __E_uconst_114(251)  => __E___E_uconst_114_115(252) ':' _(11) chars(249).
	p(NT(251), (NT(252)+T(2)+NT(11)+NT(249)));
//G391: uconst_name(250)     => __E_uconst_114(251).
	p(NT(250), (NT(251)));
//G392: uconst(239)          => '<' _(11) uconst_name(250) _(11) '>'.
	p(NT(239), (T(22)+NT(11)+NT(250)+NT(11)+T(23)));
//G393: __E_q_vars_116(254)  => _(11) ',' _(11) q_var(253).
	p(NT(254), (NT(11)+T(20)+NT(11)+NT(253)));
//G394: __E_q_vars_117(255)  => null.
	p(NT(255), (nul));
//G395: __E_q_vars_117(255)  => __E_q_vars_116(254) __E_q_vars_117(255).
	p(NT(255), (NT(254)+NT(255)));
//G396: q_vars(73)           => q_var(253) __E_q_vars_117(255).
	p(NT(73), (NT(253)+NT(255)));
//G397: q_var(253)           => capture(29).
	p(NT(253), (NT(29)));
//G398: __N_6(433)           => uconst(239).
	p(NT(433), (NT(239)));
//G399: q_var(253)           => variable(140) & ~( __N_6(433) ).	 # conjunctive
	p(NT(253), (NT(140)) & ~(NT(433)));
//G400: ctnvar(217)          => chars(249).
	p(NT(217), (NT(249)));
//G401: __E_offsets_118(256) => _(11) ',' _(11) offset(248).
	p(NT(256), (NT(11)+T(20)+NT(11)+NT(248)));
//G402: __E_offsets_119(257) => null.
	p(NT(257), (nul));
//G403: __E_offsets_119(257) => __E_offsets_118(256) __E_offsets_119(257).
	p(NT(257), (NT(256)+NT(257)));
//G404: offsets(42)          => '[' _(11) offset(248) __E_offsets_119(257) _(11) ']'.
	p(NT(42), (T(26)+NT(11)+NT(248)+NT(257)+NT(11)+T(27)));
//G405: offset(248)          => integer(258).
	p(NT(248), (NT(258)));
//G406: offset(248)          => capture(29).
	p(NT(248), (NT(29)));
//G407: offset(248)          => shift(259).
	p(NT(248), (NT(259)));
//G408: __N_7(434)           => io_var(240).
	p(NT(434), (NT(240)));
//G409: __E_offset_120(260)  => variable(140) & ~( __N_7(434) ).	 # conjunctive
	p(NT(260), (NT(140)) & ~(NT(434)));
//G410: offset(248)          => __E_offset_120(260).
	p(NT(248), (NT(260)));
//G411: __E_shift_121(261)   => capture(29).
	p(NT(261), (NT(29)));
//G412: __N_8(435)           => io_var(240).
	p(NT(435), (NT(240)));
//G413: __E___E_shift_121_122(262) => variable(140) & ~( __N_8(435) ).	 # conjunctive
	p(NT(262), (NT(140)) & ~(NT(435)));
//G414: __E_shift_121(261)   => __E___E_shift_121_122(262).
	p(NT(261), (NT(262)));
//G415: shift(259)           => __E_shift_121(261) _(11) '-' _(11) num(218).
	p(NT(259), (NT(261)+NT(11)+T(30)+NT(11)+NT(218)));
//G416: __E_chars_123(263)   => alnum(6).
	p(NT(263), (NT(6)));
//G417: __E_chars_123(263)   => '_'.
	p(NT(263), (T(37)));
//G418: __E_chars_124(264)   => null.
	p(NT(264), (nul));
//G419: __E_chars_124(264)   => __E_chars_123(263) __E_chars_124(264).
	p(NT(264), (NT(263)+NT(264)));
//G420: chars(249)           => alpha(5) __E_chars_124(264).
	p(NT(249), (NT(5)+NT(264)));
//G421: __E_digits_125(266)  => digit(3).
	p(NT(266), (NT(3)));
//G422: __E_digits_125(266)  => digit(3) __E_digits_125(266).
	p(NT(266), (NT(3)+NT(266)));
//G423: digits(265)          => __E_digits_125(266).
	p(NT(265), (NT(266)));
//G424: num(218)             => digits(265).
	p(NT(218), (NT(265)));
//G425: __E_integer_126(267) => '-'.
	p(NT(267), (T(30)));
//G426: __E_integer_126(267) => null.
	p(NT(267), (nul));
//G427: integer(258)         => __E_integer_126(267) _(11) digits(265).
	p(NT(258), (NT(267)+NT(11)+NT(265)));
//G428: sym(40)              => chars(249).
	p(NT(40), (NT(249)));
//G429: capture(29)          => '$' chars(249).
	p(NT(29), (T(49)+NT(249)));
//G430: __E_typed_127(269)   => _(11) '[' _(11) subtype(270) _(11) ']' _(11).
	p(NT(269), (NT(11)+T(26)+NT(11)+NT(270)+NT(11)+T(27)+NT(11)));
//G431: __E_typed_127(269)   => null.
	p(NT(269), (nul));
//G432: typed(33)            => _(11) ':' _(11) type(268) __E_typed_127(269).
	p(NT(33), (NT(11)+T(2)+NT(11)+NT(268)+NT(269)));
//G433: type(268)            => chars(249).
	p(NT(268), (NT(249)));
//G434: subtype(270)         => num(218).
	p(NT(270), (NT(218)));
//G435: scope_id(271)        => digits(265).
	p(NT(271), (NT(265)));
//G436: __E_comment_128(273) => printable(8).
	p(NT(273), (NT(8)));
//G437: __E_comment_128(273) => '\t'.
	p(NT(273), (T(50)));
//G438: __E_comment_129(274) => null.
	p(NT(274), (nul));
//G439: __E_comment_129(274) => __E_comment_128(273) __E_comment_129(274).
	p(NT(274), (NT(273)+NT(274)));
//G440: __E_comment_130(275) => '\n'.
	p(NT(275), (T(51)));
//G441: __E_comment_130(275) => '\r'.
	p(NT(275), (T(52)));
//G442: __E_comment_130(275) => eof(1).
	p(NT(275), (NT(1)));
//G443: comment(272)         => '#' __E_comment_129(274) __E_comment_130(275).
	p(NT(272), (T(46)+NT(274)+NT(275)));
//G444: __(34)               => space(2).
	p(NT(34), (NT(2)));
//G445: __(34)               => comment(272).
	p(NT(34), (NT(272)));
//G446: __(34)               => __(34) space(2).
	p(NT(34), (NT(34)+NT(2)));
//G447: __(34)               => __(34) comment(272).
	p(NT(34), (NT(34)+NT(272)));
//G448: _(11)                => __(34).
	p(NT(11), (NT(34)));
//G449: _(11)                => null.
	p(NT(11), (nul));
//G450: cli(276)             => _(11).
	p(NT(276), (NT(11)));
//G451: __E_cli_131(278)     => '.' _(11) cli_command(277) _(11).
	p(NT(278), (T(1)+NT(11)+NT(277)+NT(11)));
//G452: __E_cli_132(279)     => null.
	p(NT(279), (nul));
//G453: __E_cli_132(279)     => __E_cli_131(278) __E_cli_132(279).
	p(NT(279), (NT(278)+NT(279)));
//G454: cli(276)             => _(11) cli_command(277) _(11) __E_cli_132(279).
	p(NT(276), (NT(11)+NT(277)+NT(11)+NT(279)));
//G455: __E_cli_command_133(282) => 'q'.
	p(NT(282), (T(53)));
//G456: __E_cli_command_133(282) => 'q' 'u' 'i' 't'.
	p(NT(282), (T(53)+T(7)+T(4)+T(8)));
//G457: quit_sym(281)        => __E_cli_command_133(282).
	p(NT(281), (NT(282)));
//G458: quit_cmd(280)        => quit_sym(281).
	p(NT(280), (NT(281)));
//G459: cli_command(277)     => quit_cmd(280).
	p(NT(277), (NT(280)));
//G460: __E_cli_command_134(285) => 'v'.
	p(NT(285), (T(54)));
//G461: __E_cli_command_134(285) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(285), (T(54)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G462: version_sym(284)     => __E_cli_command_134(285).
	p(NT(284), (NT(285)));
//G463: version_cmd(283)     => version_sym(284).
	p(NT(283), (NT(284)));
//G464: cli_command(277)     => version_cmd(283).
	p(NT(277), (NT(283)));
//G465: __E_cli_command_135(288) => 'c'.
	p(NT(288), (T(14)));
//G466: __E_cli_command_135(288) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(288), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G467: clear_sym(287)       => __E_cli_command_135(288).
	p(NT(287), (NT(288)));
//G468: clear_cmd(286)       => clear_sym(287).
	p(NT(286), (NT(287)));
//G469: cli_command(277)     => clear_cmd(286).
	p(NT(277), (NT(286)));
//G470: __E___E_cli_command_136_137(292) => 'h'.
	p(NT(292), (T(55)));
//G471: __E___E_cli_command_136_137(292) => 'h' 'e' 'l' 'p'.
	p(NT(292), (T(55)+T(11)+T(10)+T(56)));
//G472: help_sym(291)        => __E___E_cli_command_136_137(292).
	p(NT(291), (NT(292)));
//G473: __E___E_cli_command_136_138(293) => __(34) help_arg(294).
	p(NT(293), (NT(34)+NT(294)));
//G474: __E___E_cli_command_136_138(293) => null.
	p(NT(293), (nul));
//G475: __E_cli_command_136(290) => help_sym(291) __E___E_cli_command_136_138(293).
	p(NT(290), (NT(291)+NT(293)));
//G476: help_cmd(289)        => __E_cli_command_136(290).
	p(NT(289), (NT(290)));
//G477: cli_command(277)     => help_cmd(289).
	p(NT(277), (NT(289)));
//G478: file_sym(297)        => 'f' 'i' 'l' 'e'.
	p(NT(297), (T(9)+T(4)+T(10)+T(11)));
//G479: __E_cli_command_139(296) => file_sym(297) __(34) q_string(298).
	p(NT(296), (NT(297)+NT(34)+NT(298)));
//G480: file_cmd(295)        => __E_cli_command_139(296).
	p(NT(295), (NT(296)));
//G481: cli_command(277)     => file_cmd(295).
	p(NT(277), (NT(295)));
//G482: valid_sym(301)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(301), (T(54)+T(16)+T(10)+T(4)+T(57)));
//G483: __E_cli_command_140(300) => valid_sym(301) __(34) normalize_cmd_arg(302).
	p(NT(300), (NT(301)+NT(34)+NT(302)));
//G484: valid_cmd(299)       => __E_cli_command_140(300).
	p(NT(299), (NT(300)));
//G485: cli_command(277)     => valid_cmd(299).
	p(NT(277), (NT(299)));
//G486: sat_sym(305)         => 's' 'a' 't'.
	p(NT(305), (T(15)+T(16)+T(8)));
//G487: __E_cli_command_141(304) => sat_sym(305) __(34) normalize_cmd_arg(302).
	p(NT(304), (NT(305)+NT(34)+NT(302)));
//G488: sat_cmd(303)         => __E_cli_command_141(304).
	p(NT(303), (NT(304)));
//G489: cli_command(277)     => sat_cmd(303).
	p(NT(277), (NT(303)));
//G490: unsat_sym(308)       => 'u' 'n' 's' 'a' 't'.
	p(NT(308), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G491: __E_cli_command_142(307) => unsat_sym(308) __(34) normalize_cmd_arg(302).
	p(NT(307), (NT(308)+NT(34)+NT(302)));
//G492: unsat_cmd(306)       => __E_cli_command_142(307).
	p(NT(306), (NT(307)));
//G493: cli_command(277)     => unsat_cmd(306).
	p(NT(277), (NT(306)));
//G494: solve_sym(311)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(311), (T(15)+T(6)+T(10)+T(54)+T(11)));
//G495: __E___E_cli_command_143_144(312) => solve_options(313).
	p(NT(312), (NT(313)));
//G496: __E___E_cli_command_143_144(312) => null.
	p(NT(312), (nul));
//G497: __E_cli_command_143(310) => solve_sym(311) __E___E_cli_command_143_144(312) __(34) wff_cmd_arg(314).
	p(NT(310), (NT(311)+NT(312)+NT(34)+NT(314)));
//G498: solve_cmd(309)       => __E_cli_command_143(310).
	p(NT(309), (NT(310)));
//G499: cli_command(277)     => solve_cmd(309).
	p(NT(277), (NT(309)));
//G500: lgrs_sym(317)        => 'l' 'g' 'r' 's'.
	p(NT(317), (T(10)+T(58)+T(19)+T(15)));
//G501: __E_cli_command_145(316) => lgrs_sym(317) __(34) wff_cmd_arg(314).
	p(NT(316), (NT(317)+NT(34)+NT(314)));
//G502: lgrs_cmd(315)        => __E_cli_command_145(316).
	p(NT(315), (NT(316)));
//G503: cli_command(277)     => lgrs_cmd(315).
	p(NT(277), (NT(315)));
//G504: __E___E_cli_command_146_147(321) => 'r'.
	p(NT(321), (T(19)));
//G505: __E___E_cli_command_146_147(321) => 'r' 'u' 'n'.
	p(NT(321), (T(19)+T(7)+T(5)));
//G506: run_sym(320)         => __E___E_cli_command_146_147(321).
	p(NT(320), (NT(321)));
//G507: __E___E_cli_command_146_148(322) => history(323).
	p(NT(322), (NT(323)));
//G508: __E___E_cli_command_146_148(322) => wff(14).
	p(NT(322), (NT(14)));
//G509: __E_cli_command_146(319) => run_sym(320) __(34) __E___E_cli_command_146_148(322).
	p(NT(319), (NT(320)+NT(34)+NT(322)));
//G510: run_cmd(318)         => __E_cli_command_146(319).
	p(NT(318), (NT(319)));
//G511: cli_command(277)     => run_cmd(318).
	p(NT(277), (NT(318)));
//G512: __E___E_cli_command_149_150(327) => 'n'.
	p(NT(327), (T(5)));
//G513: __E___E_cli_command_149_150(327) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(327), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(59)+T(11)));
//G514: normalize_sym(326)   => __E___E_cli_command_149_150(327).
	p(NT(326), (NT(327)));
//G515: __E_cli_command_149(325) => normalize_sym(326) __(34) normalize_cmd_arg(302).
	p(NT(325), (NT(326)+NT(34)+NT(302)));
//G516: normalize_cmd(324)   => __E_cli_command_149(325).
	p(NT(324), (NT(325)));
//G517: cli_command(277)     => normalize_cmd(324).
	p(NT(277), (NT(324)));
//G518: __E___E_cli_command_151_152(331) => 's'.
	p(NT(331), (T(15)));
//G519: __E___E_cli_command_151_152(331) => 's' 'u' 'b' 's' 't'.
	p(NT(331), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G520: __E___E_cli_command_151_152(331) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(331), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G521: subst_sym(330)       => __E___E_cli_command_151_152(331).
	p(NT(330), (NT(331)));
//G522: __E_cli_command_151(329) => subst_sym(330) __(34) nf_cmd_arg(332) _(11) '[' _(11) nf_cmd_arg(332) _(11) '/' _(11) nf_cmd_arg(332) _(11) ']'.
	p(NT(329), (NT(330)+NT(34)+NT(332)+NT(11)+T(26)+NT(11)+NT(332)+NT(11)+T(43)+NT(11)+NT(332)+NT(11)+T(27)));
//G523: subst_cmd(328)       => __E_cli_command_151(329).
	p(NT(328), (NT(329)));
//G524: cli_command(277)     => subst_cmd(328).
	p(NT(277), (NT(328)));
//G525: __E___E_cli_command_153_154(336) => 'i'.
	p(NT(336), (T(4)));
//G526: __E___E_cli_command_153_154(336) => 'i' 'n' 's' 't'.
	p(NT(336), (T(4)+T(5)+T(15)+T(8)));
//G527: __E___E_cli_command_153_154(336) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(336), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G528: inst_sym(335)        => __E___E_cli_command_153_154(336).
	p(NT(335), (NT(336)));
//G529: __E_cli_command_153(334) => inst_sym(335) __(34) inst_args(337).
	p(NT(334), (NT(335)+NT(34)+NT(337)));
//G530: inst_cmd(333)        => __E_cli_command_153(334).
	p(NT(333), (NT(334)));
//G531: cli_command(277)     => inst_cmd(333).
	p(NT(277), (NT(333)));
//G532: dnf_sym(340)         => 'd' 'n' 'f'.
	p(NT(340), (T(57)+T(5)+T(9)));
//G533: __E_cli_command_155(339) => dnf_sym(340) __(34) nf_cmd_arg(332).
	p(NT(339), (NT(340)+NT(34)+NT(332)));
//G534: dnf_cmd(338)         => __E_cli_command_155(339).
	p(NT(338), (NT(339)));
//G535: cli_command(277)     => dnf_cmd(338).
	p(NT(277), (NT(338)));
//G536: cnf_sym(343)         => 'c' 'n' 'f'.
	p(NT(343), (T(14)+T(5)+T(9)));
//G537: __E_cli_command_156(342) => cnf_sym(343) __(34) nf_cmd_arg(332).
	p(NT(342), (NT(343)+NT(34)+NT(332)));
//G538: cnf_cmd(341)         => __E_cli_command_156(342).
	p(NT(341), (NT(342)));
//G539: cli_command(277)     => cnf_cmd(341).
	p(NT(277), (NT(341)));
//G540: anf_sym(346)         => 'a' 'n' 'f'.
	p(NT(346), (T(16)+T(5)+T(9)));
//G541: __E_cli_command_157(345) => anf_sym(346) __(34) nf_cmd_arg(332).
	p(NT(345), (NT(346)+NT(34)+NT(332)));
//G542: anf_cmd(344)         => __E_cli_command_157(345).
	p(NT(344), (NT(345)));
//G543: cli_command(277)     => anf_cmd(344).
	p(NT(277), (NT(344)));
//G544: nnf_sym(349)         => 'n' 'n' 'f'.
	p(NT(349), (T(5)+T(5)+T(9)));
//G545: __E_cli_command_158(348) => nnf_sym(349) __(34) nf_cmd_arg(332).
	p(NT(348), (NT(349)+NT(34)+NT(332)));
//G546: nnf_cmd(347)         => __E_cli_command_158(348).
	p(NT(347), (NT(348)));
//G547: cli_command(277)     => nnf_cmd(347).
	p(NT(277), (NT(347)));
//G548: pnf_sym(352)         => 'p' 'n' 'f'.
	p(NT(352), (T(56)+T(5)+T(9)));
//G549: __E_cli_command_159(351) => pnf_sym(352) __(34) nf_cmd_arg(332).
	p(NT(351), (NT(352)+NT(34)+NT(332)));
//G550: pnf_cmd(350)         => __E_cli_command_159(351).
	p(NT(350), (NT(351)));
//G551: cli_command(277)     => pnf_cmd(350).
	p(NT(277), (NT(350)));
//G552: mnf_sym(355)         => 'm' 'n' 'f'.
	p(NT(355), (T(21)+T(5)+T(9)));
//G553: __E_cli_command_160(354) => mnf_sym(355) __(34) nf_cmd_arg(332).
	p(NT(354), (NT(355)+NT(34)+NT(332)));
//G554: mnf_cmd(353)         => __E_cli_command_160(354).
	p(NT(353), (NT(354)));
//G555: cli_command(277)     => mnf_cmd(353).
	p(NT(277), (NT(353)));
//G556: onf_sym(358)         => 'o' 'n' 'f'.
	p(NT(358), (T(6)+T(5)+T(9)));
//G557: __E_cli_command_161(357) => onf_sym(358) __(34) variable(140) __(34) onf_cmd_arg(359).
	p(NT(357), (NT(358)+NT(34)+NT(140)+NT(34)+NT(359)));
//G558: onf_cmd(356)         => __E_cli_command_161(357).
	p(NT(356), (NT(357)));
//G559: cli_command(277)     => onf_cmd(356).
	p(NT(277), (NT(356)));
//G560: qelim_sym(362)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(362), (T(53)+T(11)+T(10)+T(4)+T(21)));
//G561: __E_cli_command_162(361) => qelim_sym(362) __(34) wff_cmd_arg(314).
	p(NT(361), (NT(362)+NT(34)+NT(314)));
//G562: qelim_cmd(360)       => __E_cli_command_162(361).
	p(NT(360), (NT(361)));
//G563: cli_command(277)     => qelim_cmd(360).
	p(NT(277), (NT(360)));
//G564: get_sym(365)         => 'g' 'e' 't'.
	p(NT(365), (T(58)+T(11)+T(8)));
//G565: __E___E_cli_command_163_164(366) => __(34) option_name(367).
	p(NT(366), (NT(34)+NT(367)));
//G566: __E___E_cli_command_163_164(366) => null.
	p(NT(366), (nul));
//G567: __E_cli_command_163(364) => get_sym(365) __E___E_cli_command_163_164(366).
	p(NT(364), (NT(365)+NT(366)));
//G568: get_cmd(363)         => __E_cli_command_163(364).
	p(NT(363), (NT(364)));
//G569: cli_command(277)     => get_cmd(363).
	p(NT(277), (NT(363)));
//G570: set_sym(370)         => 's' 'e' 't'.
	p(NT(370), (T(15)+T(11)+T(8)));
//G571: __E___E_cli_command_165_166(371) => __(34).
	p(NT(371), (NT(34)));
//G572: __E___E_cli_command_165_166(371) => _(11) '=' _(11).
	p(NT(371), (NT(11)+T(3)+NT(11)));
//G573: __E_cli_command_165(369) => set_sym(370) __(34) option_name(367) __E___E_cli_command_165_166(371) option_value(372).
	p(NT(369), (NT(370)+NT(34)+NT(367)+NT(371)+NT(372)));
//G574: set_cmd(368)         => __E_cli_command_165(369).
	p(NT(368), (NT(369)));
//G575: cli_command(277)     => set_cmd(368).
	p(NT(277), (NT(368)));
//G576: enable_sym(375)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(375), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G577: __E_cli_command_167(374) => enable_sym(375) __(34) option_name(367).
	p(NT(374), (NT(375)+NT(34)+NT(367)));
//G578: enable_cmd(373)      => __E_cli_command_167(374).
	p(NT(373), (NT(374)));
//G579: cli_command(277)     => enable_cmd(373).
	p(NT(277), (NT(373)));
//G580: disable_sym(378)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(378), (T(57)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G581: __E_cli_command_168(377) => disable_sym(378) __(34) option_name(367).
	p(NT(377), (NT(378)+NT(34)+NT(367)));
//G582: disable_cmd(376)     => __E_cli_command_168(377).
	p(NT(376), (NT(377)));
//G583: cli_command(277)     => disable_cmd(376).
	p(NT(277), (NT(376)));
//G584: toggle_sym(381)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(381), (T(8)+T(6)+T(58)+T(58)+T(10)+T(11)));
//G585: __E_cli_command_169(380) => toggle_sym(381) __(34) option_name(367).
	p(NT(380), (NT(381)+NT(34)+NT(367)));
//G586: toggle_cmd(379)      => __E_cli_command_169(380).
	p(NT(379), (NT(380)));
//G587: cli_command(277)     => toggle_cmd(379).
	p(NT(277), (NT(379)));
//G588: __E___E_cli_command_170_171(385) => 'd' 'e' 'f' 's'.
	p(NT(385), (T(57)+T(11)+T(9)+T(15)));
//G589: __E___E_cli_command_170_171(385) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(385), (T(57)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G590: def_sym(384)         => __E___E_cli_command_170_171(385).
	p(NT(384), (NT(385)));
//G591: __E_cli_command_170(383) => def_sym(384).
	p(NT(383), (NT(384)));
//G592: def_list_cmd(382)    => __E_cli_command_170(383).
	p(NT(382), (NT(383)));
//G593: cli_command(277)     => def_list_cmd(382).
	p(NT(277), (NT(382)));
//G594: __E_cli_command_172(387) => def_sym(384) __(34) num(218).
	p(NT(387), (NT(384)+NT(34)+NT(218)));
//G595: def_print_cmd(386)   => __E_cli_command_172(387).
	p(NT(386), (NT(387)));
//G596: cli_command(277)     => def_print_cmd(386).
	p(NT(277), (NT(386)));
//G597: def_rr_cmd(388)      => rec_relation(17).
	p(NT(388), (NT(17)));
//G598: cli_command(277)     => def_rr_cmd(388).
	p(NT(277), (NT(388)));
//G599: def_input_cmd(389)   => input_def(18).
	p(NT(389), (NT(18)));
//G600: cli_command(277)     => def_input_cmd(389).
	p(NT(277), (NT(389)));
//G601: def_output_cmd(390)  => output_def(19).
	p(NT(390), (NT(19)));
//G602: cli_command(277)     => def_output_cmd(390).
	p(NT(277), (NT(390)));
//G603: __E___E_cli_command_173_174(394) => 'h' 'i' 's' 't'.
	p(NT(394), (T(55)+T(4)+T(15)+T(8)));
//G604: __E___E_cli_command_173_174(394) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(394), (T(55)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G605: history_sym(393)     => __E___E_cli_command_173_174(394).
	p(NT(393), (NT(394)));
//G606: __E_cli_command_173(392) => history_sym(393).
	p(NT(392), (NT(393)));
//G607: history_list_cmd(391) => __E_cli_command_173(392).
	p(NT(391), (NT(392)));
//G608: cli_command(277)     => history_list_cmd(391).
	p(NT(277), (NT(391)));
//G609: __E_cli_command_175(396) => history_sym(393) __(34) history(323).
	p(NT(396), (NT(393)+NT(34)+NT(323)));
//G610: history_print_cmd(395) => __E_cli_command_175(396).
	p(NT(395), (NT(396)));
//G611: cli_command(277)     => history_print_cmd(395).
	p(NT(277), (NT(395)));
//G612: __E_cli_command_176(398) => wff(14).
	p(NT(398), (NT(14)));
//G613: __E_cli_command_176(398) => bf(30).
	p(NT(398), (NT(30)));
//G614: history_store_cmd(397) => __E_cli_command_176(398).
	p(NT(397), (NT(398)));
//G615: cli_command(277)     => history_store_cmd(397).
	p(NT(277), (NT(397)));
//G616: __E_solve_options_177(399) => __(34) solve_option(400).
	p(NT(399), (NT(34)+NT(400)));
//G617: __E_solve_options_178(401) => null.
	p(NT(401), (nul));
//G618: __E_solve_options_178(401) => __E_solve_options_177(399) __E_solve_options_178(401).
	p(NT(401), (NT(399)+NT(401)));
//G619: solve_options(313)   => __E_solve_options_178(401).
	p(NT(313), (NT(401)));
//G620: __E_solve_option_179(402) => solver_mode(403).
	p(NT(402), (NT(403)));
//G621: __E_solve_option_179(402) => type(268).
	p(NT(402), (NT(268)));
//G622: solve_option(400)    => '-' '-' __E_solve_option_179(402).
	p(NT(400), (T(30)+T(30)+NT(402)));
//G623: __E_solver_mode_180(405) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(405), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G624: __E_solver_mode_180(405) => 'm' 'i' 'n'.
	p(NT(405), (T(21)+T(4)+T(5)));
//G625: solver_mode_minimum(404) => __E_solver_mode_180(405).
	p(NT(404), (NT(405)));
//G626: solver_mode(403)     => solver_mode_minimum(404).
	p(NT(403), (NT(404)));
//G627: __E_solver_mode_181(407) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(407), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G628: __E_solver_mode_181(407) => 'm' 'a' 'x'.
	p(NT(407), (T(21)+T(16)+T(29)));
//G629: solver_mode_maximum(406) => __E_solver_mode_181(407).
	p(NT(406), (NT(407)));
//G630: solver_mode(403)     => solver_mode_maximum(406).
	p(NT(403), (NT(406)));
//G631: q_file_name(38)      => '"' file_name(408) '"'.
	p(NT(38), (T(60)+NT(408)+T(60)));
//G632: __E_file_name_182(409) => printable(8).
	p(NT(409), (NT(8)));
//G633: __E_file_name_182(409) => printable(8) __E_file_name_182(409).
	p(NT(409), (NT(8)+NT(409)));
//G634: file_name(408)       => __E_file_name_182(409).
	p(NT(408), (NT(409)));
//G635: __E_option_name_183(410) => alnum(6).
	p(NT(410), (NT(6)));
//G636: __E_option_name_183(410) => alnum(6) __E_option_name_183(410).
	p(NT(410), (NT(6)+NT(410)));
//G637: option_name(367)     => __E_option_name_183(410).
	p(NT(367), (NT(410)));
//G638: __E_option_value_184(411) => alnum(6).
	p(NT(411), (NT(6)));
//G639: __E_option_value_184(411) => alnum(6) __E_option_value_184(411).
	p(NT(411), (NT(6)+NT(411)));
//G640: option_value(372)    => __E_option_value_184(411).
	p(NT(372), (NT(411)));
//G641: bf_cmd_arg(412)      => history(323).
	p(NT(412), (NT(323)));
//G642: bf_cmd_arg(412)      => bf(30).
	p(NT(412), (NT(30)));
//G643: wff_cmd_arg(314)     => history(323).
	p(NT(314), (NT(323)));
//G644: wff_cmd_arg(314)     => wff(14).
	p(NT(314), (NT(14)));
//G645: nf_cmd_arg(332)      => history(323).
	p(NT(332), (NT(323)));
//G646: nf_cmd_arg(332)      => ref(27).
	p(NT(332), (NT(27)));
//G647: nf_cmd_arg(332)      => wff(14).
	p(NT(332), (NT(14)));
//G648: nf_cmd_arg(332)      => bf(30).
	p(NT(332), (NT(30)));
//G649: onf_cmd_arg(359)     => history(323).
	p(NT(359), (NT(323)));
//G650: onf_cmd_arg(359)     => wff(14).
	p(NT(359), (NT(14)));
//G651: normalize_cmd_arg(302) => history(323).
	p(NT(302), (NT(323)));
//G652: normalize_cmd_arg(302) => spec(10).
	p(NT(302), (NT(10)));
//G653: normalize_cmd_arg(302) => ref(27).
	p(NT(302), (NT(27)));
//G654: normalize_cmd_arg(302) => wff(14).
	p(NT(302), (NT(14)));
//G655: normalize_cmd_arg(302) => bf(30).
	p(NT(302), (NT(30)));
//G656: inst_args(337)       => wff_cmd_arg(314) _(11) '[' _(11) variable(140) _(11) '/' _(11) bf_cmd_arg(412) _(11) ']'.
	p(NT(337), (NT(314)+NT(11)+T(26)+NT(11)+NT(140)+NT(11)+T(43)+NT(11)+NT(412)+NT(11)+T(27)));
//G657: inst_args(337)       => bf_cmd_arg(412) _(11) '[' _(11) variable(140) _(11) '/' _(11) bf_cmd_arg(412) _(11) ']'.
	p(NT(337), (NT(412)+NT(11)+T(26)+NT(11)+NT(140)+NT(11)+T(43)+NT(11)+NT(412)+NT(11)+T(27)));
//G658: help_arg(294)        => help_sym(291).
	p(NT(294), (NT(291)));
//G659: help_arg(294)        => version_sym(284).
	p(NT(294), (NT(284)));
//G660: help_arg(294)        => quit_sym(281).
	p(NT(294), (NT(281)));
//G661: help_arg(294)        => clear_sym(287).
	p(NT(294), (NT(287)));
//G662: help_arg(294)        => get_sym(365).
	p(NT(294), (NT(365)));
//G663: help_arg(294)        => set_sym(370).
	p(NT(294), (NT(370)));
//G664: help_arg(294)        => enable_sym(375).
	p(NT(294), (NT(375)));
//G665: help_arg(294)        => disable_sym(378).
	p(NT(294), (NT(378)));
//G666: help_arg(294)        => toggle_sym(381).
	p(NT(294), (NT(381)));
//G667: help_arg(294)        => file_sym(297).
	p(NT(294), (NT(297)));
//G668: help_arg(294)        => history_sym(393).
	p(NT(294), (NT(393)));
//G669: help_arg(294)        => abs_history_sym(413).
	p(NT(294), (NT(413)));
//G670: help_arg(294)        => rel_history_sym(414).
	p(NT(294), (NT(414)));
//G671: help_arg(294)        => selection_sym(415).
	p(NT(294), (NT(415)));
//G672: help_arg(294)        => def_sym(384).
	p(NT(294), (NT(384)));
//G673: help_arg(294)        => inst_sym(335).
	p(NT(294), (NT(335)));
//G674: help_arg(294)        => subst_sym(330).
	p(NT(294), (NT(330)));
//G675: help_arg(294)        => normalize_sym(326).
	p(NT(294), (NT(326)));
//G676: help_arg(294)        => execute_sym(416).
	p(NT(294), (NT(416)));
//G677: help_arg(294)        => solve_sym(311).
	p(NT(294), (NT(311)));
//G678: help_arg(294)        => valid_sym(301).
	p(NT(294), (NT(301)));
//G679: help_arg(294)        => sat_sym(305).
	p(NT(294), (NT(305)));
//G680: help_arg(294)        => unsat_sym(308).
	p(NT(294), (NT(308)));
//G681: help_arg(294)        => run_sym(320).
	p(NT(294), (NT(320)));
//G682: help_arg(294)        => dnf_sym(340).
	p(NT(294), (NT(340)));
//G683: help_arg(294)        => cnf_sym(343).
	p(NT(294), (NT(343)));
//G684: help_arg(294)        => anf_sym(346).
	p(NT(294), (NT(346)));
//G685: help_arg(294)        => snf_sym(417).
	p(NT(294), (NT(417)));
//G686: help_arg(294)        => nnf_sym(349).
	p(NT(294), (NT(349)));
//G687: help_arg(294)        => pnf_sym(352).
	p(NT(294), (NT(352)));
//G688: help_arg(294)        => mnf_sym(355).
	p(NT(294), (NT(355)));
//G689: help_arg(294)        => onf_sym(358).
	p(NT(294), (NT(358)));
//G690: help_arg(294)        => qelim_sym(362).
	p(NT(294), (NT(362)));
//G691: __E___E_help_arg_185_186(420) => 's'.
	p(NT(420), (T(15)));
//G692: __E___E_help_arg_185_186(420) => null.
	p(NT(420), (nul));
//G693: __E_help_arg_185(419) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_185_186(420).
	p(NT(419), (T(11)+T(29)+T(16)+T(21)+T(56)+T(10)+T(11)+NT(420)));
//G694: examples_sym(418)    => __E_help_arg_185(419).
	p(NT(418), (NT(419)));
//G695: help_arg(294)        => examples_sym(418).
	p(NT(294), (NT(418)));
//G696: __E_history_187(422) => '%'.
	p(NT(422), (T(44)));
//G697: rel_history_sym(414) => '%' '-'.
	p(NT(414), (T(44)+T(30)));
//G698: history_id(424)      => digits(265).
	p(NT(424), (NT(265)));
//G699: __E___E_history_187_188(423) => history_id(424).
	p(NT(423), (NT(424)));
//G700: __E___E_history_187_188(423) => null.
	p(NT(423), (nul));
//G701: __E_history_187(422) => rel_history_sym(414) __E___E_history_187_188(423).
	p(NT(422), (NT(414)+NT(423)));
//G702: rel_history(421)     => __E_history_187(422).
	p(NT(421), (NT(422)));
//G703: history(323)         => rel_history(421).
	p(NT(323), (NT(421)));
//G704: abs_history_sym(413) => '%'.
	p(NT(413), (T(44)));
//G705: __E_history_189(426) => abs_history_sym(413) history_id(424).
	p(NT(426), (NT(413)+NT(424)));
//G706: abs_history(425)     => __E_history_189(426).
	p(NT(425), (NT(426)));
//G707: history(323)         => abs_history(425).
	p(NT(323), (NT(425)));
	#undef T
	#undef NT
	return loaded = true, p;
}

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace tau_parser_data

struct tau_parser_nonterminals {
	enum nonterminal {
		nul, eof, space, digit, xdigit, alpha, alnum, punct, printable, start, 
		spec, _, definitions, main, wff, spec_multiline, __E_spec_multiline_0, rec_relation, input_def, output_def, 
		__E_spec_multiline_1, __E_definitions_2, __E___E_definitions_2_3, __E_definitions_4, rec_relations, __E_rec_relations_5, __E_rec_relations_6, ref, __E_rec_relation_7, capture, 
		bf, io_var_name, __E_input_def_8, typed, __, stream, __E_output_def_9, __E_output_def_10, q_file_name, console_sym, 
		sym, __E_ref_11, offsets, ref_args, __E_ref_12, fp_fallback, first_sym, last_sym, __E_ref_args_13, __E___E_ref_args_13_14, 
		ref_arg, __E___E_ref_args_13_15, __E___E_ref_args_13_16, wff_rule, wff_matcher, wff_body, bf_rule, bf_matcher, bf_body, tau_constant_source, 
		__E_tau_constant_source_17, wff_parenthesis, __E_wff_18, wff_sometimes, __E_wff_19, __E___E_wff_19_20, wff_always, __E_wff_21, __E___E_wff_21_22, wff_conditional, 
		__E_wff_23, wff_all, __E_wff_24, q_vars, wff_ex, __E_wff_25, wff_ref, constraint, wff_imply, __E_wff_26, 
		wff_rimply, __E_wff_27, wff_equiv, __E_wff_28, wff_or, __E_wff_29, wff_xor, __E_wff_30, wff_and, __E_wff_31, 
		wff_neg, __E_wff_32, wff_t, wff_f, bf_interval, __E_wff_33, bf_eq, __E_wff_34, bf_neq, __E_wff_35, 
		bf_lteq, __E_wff_36, bf_nlteq, __E_wff_37, bf_gt, __E_wff_38, bf_ngt, __E_wff_39, bf_gteq, __E_wff_40, 
		bf_ngteq, __E_wff_41, bf_lt, __E_wff_42, bf_nlt, __E_wff_43, bv_eq, __E_wff_44, bv, bv_neq, 
		__E_wff_45, bv_lteq, __E_wff_46, bv_nlteq, __E_wff_47, bv_gt, __E_wff_48, bv_ngt, __E_wff_49, bv_gteq, 
		__E_wff_50, bv_ngteq, __E_wff_51, bv_lt, __E_wff_52, bv_nlt, __E_wff_53, bf_parenthesis, __E_bf_54, bf_constant, 
		variable, bf_fall, __E_bf_55, bf_fex, __E_bf_56, bf_ref, bf_or, __E_bf_57, bf_xor, __E_bf_58, 
		bf_t, __E_bf_59, __E___E_bf_59_60, bf_f, __E_bf_61, __E___E_bf_61_62, bf_and, __E_bf_63, __E___E_bf_63_64, bf_neg, 
		__E_bf_65, bf_neg_oprnd, __E___E_bf_65_66, bf_and_nosep, __E_bf_67, bf_and_nosep_1st_oprnd, __E___E_bf_67_68, bf_and_nosep_2nd_oprnd, __E___E_bf_67_69, bv_parenthesis, 
		__E_bv_70, bv_checked, __E_bv_71, bv_add, __E_bv_72, bv_mul, __E_bv_73, bv_div, __E_bv_74, bv_mod, 
		__E_bv_75, bv_sub, __E_bv_76, bv_neg, bv_and, __E_bv_77, bv_nand, __E_bv_78, bv_or, __E_bv_79, 
		bv_nor, __E_bv_80, bv_xor, __E_bv_81, bv_xnor, __E_bv_82, bv_shl, __E_bv_83, bv_shr, __E_bv_84, 
		bv_min, __E_bv_85, bv_max, __E_bv_86, bv_constant, binary, __E_bv_constant_87, decimal, __E_bv_constant_88, hexadecimal, 
		__E_bv_constant_89, __E_binary_90, __E_binary_91, __E_hexadecimal_92, __E_decimal_93, ctn_neq, __E_constraint_94, ctnvar, num, ctn_eq, 
		__E_constraint_95, ctn_gteq, __E_constraint_96, ctn_gt, __E_constraint_97, ctn_lteq, __E_constraint_98, ctn_lt, __E_constraint_99, __E_bf_constant_100, 
		source, __E_bf_constant_101, src_c, __E_source_102, __E___E_source_102_103, __E___E_source_102_104, __E_src_c_105, __E_src_c_106, __E_variable_107, uconst, 
		io_var, var_name, __E_variable_108, __E_var_name_109, __E_var_name_110, __E_var_name_111, __E_var_name_112, __E_var_name_113, offset, chars, 
		uconst_name, __E_uconst_114, __E___E_uconst_114_115, q_var, __E_q_vars_116, __E_q_vars_117, __E_offsets_118, __E_offsets_119, integer, shift, 
		__E_offset_120, __E_shift_121, __E___E_shift_121_122, __E_chars_123, __E_chars_124, digits, __E_digits_125, __E_integer_126, type, __E_typed_127, 
		subtype, scope_id, comment, __E_comment_128, __E_comment_129, __E_comment_130, cli, cli_command, __E_cli_131, __E_cli_132, 
		quit_cmd, quit_sym, __E_cli_command_133, version_cmd, version_sym, __E_cli_command_134, clear_cmd, clear_sym, __E_cli_command_135, help_cmd, 
		__E_cli_command_136, help_sym, __E___E_cli_command_136_137, __E___E_cli_command_136_138, help_arg, file_cmd, __E_cli_command_139, file_sym, q_string, valid_cmd, 
		__E_cli_command_140, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_141, sat_sym, unsat_cmd, __E_cli_command_142, unsat_sym, solve_cmd, 
		__E_cli_command_143, solve_sym, __E___E_cli_command_143_144, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_145, lgrs_sym, run_cmd, __E_cli_command_146, 
		run_sym, __E___E_cli_command_146_147, __E___E_cli_command_146_148, history, normalize_cmd, __E_cli_command_149, normalize_sym, __E___E_cli_command_149_150, subst_cmd, __E_cli_command_151, 
		subst_sym, __E___E_cli_command_151_152, nf_cmd_arg, inst_cmd, __E_cli_command_153, inst_sym, __E___E_cli_command_153_154, inst_args, dnf_cmd, __E_cli_command_155, 
		dnf_sym, cnf_cmd, __E_cli_command_156, cnf_sym, anf_cmd, __E_cli_command_157, anf_sym, nnf_cmd, __E_cli_command_158, nnf_sym, 
		pnf_cmd, __E_cli_command_159, pnf_sym, mnf_cmd, __E_cli_command_160, mnf_sym, onf_cmd, __E_cli_command_161, onf_sym, onf_cmd_arg, 
		qelim_cmd, __E_cli_command_162, qelim_sym, get_cmd, __E_cli_command_163, get_sym, __E___E_cli_command_163_164, option_name, set_cmd, __E_cli_command_165, 
		set_sym, __E___E_cli_command_165_166, option_value, enable_cmd, __E_cli_command_167, enable_sym, disable_cmd, __E_cli_command_168, disable_sym, toggle_cmd, 
		__E_cli_command_169, toggle_sym, def_list_cmd, __E_cli_command_170, def_sym, __E___E_cli_command_170_171, def_print_cmd, __E_cli_command_172, def_rr_cmd, def_input_cmd, 
		def_output_cmd, history_list_cmd, __E_cli_command_173, history_sym, __E___E_cli_command_173_174, history_print_cmd, __E_cli_command_175, history_store_cmd, __E_cli_command_176, __E_solve_options_177, 
		solve_option, __E_solve_options_178, __E_solve_option_179, solver_mode, solver_mode_minimum, __E_solver_mode_180, solver_mode_maximum, __E_solver_mode_181, file_name, __E_file_name_182, 
		__E_option_name_183, __E_option_value_184, bf_cmd_arg, abs_history_sym, rel_history_sym, selection_sym, execute_sym, snf_sym, examples_sym, __E_help_arg_185, 
		__E___E_help_arg_185_186, rel_history, __E_history_187, __E___E_history_187_188, history_id, abs_history, __E_history_189, __N_0, __N_1, __N_2, 
		__N_3, __N_4, __N_5, __N_6, __N_7, __N_8, 
	};
};

struct tau_parser : public idni::parser<char, char>, public tau_parser_nonterminals {
	static tau_parser& instance() {
		static tau_parser inst;
		return inst;
	}
	tau_parser() : idni::parser<char_type, terminal_type>(
		tau_parser_data::grammar,
		tau_parser_data::parser_options) {}
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
