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
	"ref_arg", "__E___E_ref_args_13_15", "__E___E_ref_args_13_16", "library", "rules", "__E_library_17", "__E___E_library_17_18", "rule", "__E___E___E_library_17_18_19", "wff_rule", 
	"bf_rule", "__E___E_library_17_20", "wff_matcher", "wff_body", "bf_matcher", "bf_body", "builder", "builder_head", "builder_body", "__E_builder_head_21", 
	"__E_builder_head_22", "bf_builder_body", "__E_builder_body_23", "wff_builder_body", "__E_builder_body_24", "tau_constant_source", "__E_tau_constant_source_25", "wff_parenthesis", "__E_wff_26", "wff_sometimes", 
	"__E_wff_27", "__E___E_wff_27_28", "wff_always", "__E_wff_29", "__E___E_wff_29_30", "wff_conditional", "__E_wff_31", "wff_all", "__E_wff_32", "q_vars", 
	"wff_ex", "__E_wff_33", "wff_ref", "constraint", "wff_imply", "__E_wff_34", "wff_rimply", "__E_wff_35", "wff_equiv", "__E_wff_36", 
	"wff_or", "__E_wff_37", "wff_xor", "__E_wff_38", "wff_and", "__E_wff_39", "wff_neg", "__E_wff_40", "wff_t", "wff_f", 
	"bf_interval", "__E_wff_41", "bf_eq", "__E_wff_42", "bf_neq", "__E_wff_43", "bf_lteq", "__E_wff_44", "bf_nlteq", "__E_wff_45", 
	"bf_gt", "__E_wff_46", "bf_ngt", "__E_wff_47", "bf_gteq", "__E_wff_48", "bf_ngteq", "__E_wff_49", "bf_lt", "__E_wff_50", 
	"bf_nlt", "__E_wff_51", "bv_eq", "__E_wff_52", "bv", "bv_neq", "__E_wff_53", "bv_lteq", "__E_wff_54", "bv_nlteq", 
	"__E_wff_55", "bv_gt", "__E_wff_56", "bv_ngt", "__E_wff_57", "bv_gteq", "__E_wff_58", "bv_ngteq", "__E_wff_59", "bv_lt", 
	"__E_wff_60", "bv_nlt", "__E_wff_61", "bf_parenthesis", "__E_bf_62", "bf_constant", "variable", "bf_splitter", "__E_bf_63", "bf_fall", 
	"__E_bf_64", "bf_fex", "__E_bf_65", "bf_ref", "bf_or", "__E_bf_66", "bf_xor", "__E_bf_67", "bf_t", "__E_bf_68", 
	"__E___E_bf_68_69", "bf_f", "__E_bf_70", "__E___E_bf_70_71", "bf_and", "__E_bf_72", "__E___E_bf_72_73", "bf_neg", "__E_bf_74", "bf_neg_oprnd", 
	"__E___E_bf_74_75", "bf_and_nosep", "__E_bf_76", "bf_and_nosep_1st_oprnd", "__E___E_bf_76_77", "bf_and_nosep_2nd_oprnd", "__E___E_bf_76_78", "bv_parenthesis", "__E_bv_79", "bv_checked", 
	"__E_bv_80", "bv_add", "__E_bv_81", "bv_mul", "__E_bv_82", "bv_div", "__E_bv_83", "bv_mod", "__E_bv_84", "bv_sub", 
	"__E_bv_85", "bv_neg", "bv_and", "__E_bv_86", "bv_nand", "__E_bv_87", "bv_or", "__E_bv_88", "bv_nor", "__E_bv_89", 
	"bv_xor", "__E_bv_90", "bv_xnor", "__E_bv_91", "bv_rotate_left", "__E_bv_92", "bv_rotate_right", "__E_bv_93", "bv_min", "__E_bv_94", 
	"bv_max", "__E_bv_95", "bv_constant", "binary", "__E_bv_constant_96", "type", "decimal", "__E_bv_constant_97", "hexadecimal", "__E_bv_constant_98", 
	"__E_binary_99", "__E_binary_100", "__E_hexadecimal_101", "__E_decimal_102", "ctn_neq", "__E_constraint_103", "ctnvar", "num", "ctn_eq", "__E_constraint_104", 
	"ctn_gteq", "__E_constraint_105", "ctn_gt", "__E_constraint_106", "ctn_lteq", "__E_constraint_107", "ctn_lt", "__E_constraint_108", "__E_bf_constant_109", "source", 
	"__E_bf_constant_110", "src_c", "__E_source_111", "__E___E_source_111_112", "__E___E_source_111_113", "__E_src_c_114", "__E_src_c_115", "__E_variable_116", "uconst", "io_var", 
	"var_name", "__E_variable_117", "__E_var_name_118", "__E_var_name_119", "__E_var_name_120", "__E_var_name_121", "__E_var_name_122", "offset", "chars", "uconst_name", 
	"__E_uconst_123", "__E___E_uconst_123_124", "q_var", "__E_q_vars_125", "__E_q_vars_126", "__E_offsets_127", "__E_offsets_128", "integer", "shift", "__E_offset_129", 
	"__E_shift_130", "__E___E_shift_130_131", "__E_chars_132", "__E_chars_133", "digits", "__E_digits_134", "__E_integer_135", "scope_id", "comment", "__E_comment_136", 
	"__E_comment_137", "__E_comment_138", "cli", "cli_command", "__E_cli_139", "__E_cli_140", "quit_cmd", "quit_sym", "__E_cli_command_141", "version_cmd", 
	"version_sym", "__E_cli_command_142", "clear_cmd", "clear_sym", "__E_cli_command_143", "help_cmd", "__E_cli_command_144", "help_sym", "__E___E_cli_command_144_145", "__E___E_cli_command_144_146", 
	"help_arg", "file_cmd", "__E_cli_command_147", "file_sym", "q_string", "valid_cmd", "__E_cli_command_148", "valid_sym", "normalize_cmd_arg", "sat_cmd", 
	"__E_cli_command_149", "sat_sym", "unsat_cmd", "__E_cli_command_150", "unsat_sym", "solve_cmd", "__E_cli_command_151", "solve_sym", "__E___E_cli_command_151_152", "solve_options", 
	"wff_cmd_arg", "lgrs_cmd", "__E_cli_command_153", "lgrs_sym", "run_cmd", "__E_cli_command_154", "run_sym", "__E___E_cli_command_154_155", "__E___E_cli_command_154_156", "history", 
	"normalize_cmd", "__E_cli_command_157", "normalize_sym", "__E___E_cli_command_157_158", "subst_cmd", "__E_cli_command_159", "subst_sym", "__E___E_cli_command_159_160", "nf_cmd_arg", "inst_cmd", 
	"__E_cli_command_161", "inst_sym", "__E___E_cli_command_161_162", "inst_args", "dnf_cmd", "__E_cli_command_163", "dnf_sym", "cnf_cmd", "__E_cli_command_164", "cnf_sym", 
	"anf_cmd", "__E_cli_command_165", "anf_sym", "nnf_cmd", "__E_cli_command_166", "nnf_sym", "pnf_cmd", "__E_cli_command_167", "pnf_sym", "mnf_cmd", 
	"__E_cli_command_168", "mnf_sym", "snf_cmd", "__E_cli_command_169", "snf_sym", "onf_cmd", "__E_cli_command_170", "onf_sym", "onf_cmd_arg", "qelim_cmd", 
	"__E_cli_command_171", "qelim_sym", "get_cmd", "__E_cli_command_172", "get_sym", "__E___E_cli_command_172_173", "option_name", "set_cmd", "__E_cli_command_174", "set_sym", 
	"__E___E_cli_command_174_175", "option_value", "enable_cmd", "__E_cli_command_176", "enable_sym", "disable_cmd", "__E_cli_command_177", "disable_sym", "toggle_cmd", "__E_cli_command_178", 
	"toggle_sym", "def_list_cmd", "__E_cli_command_179", "def_sym", "__E___E_cli_command_179_180", "def_print_cmd", "__E_cli_command_181", "def_rr_cmd", "def_input_cmd", "def_output_cmd", 
	"history_list_cmd", "__E_cli_command_182", "history_sym", "__E___E_cli_command_182_183", "history_print_cmd", "__E_cli_command_184", "history_store_cmd", "__E_cli_command_185", "__E_solve_options_186", "solve_option", 
	"__E_solve_options_187", "__E_solve_option_188", "solver_mode", "solver_mode_minimum", "__E_solver_mode_189", "solver_mode_maximum", "__E_solver_mode_190", "file_name", "__E_file_name_191", "__E_option_name_192", 
	"__E_option_value_193", "bf_cmd_arg", "abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", "examples_sym", "__E_help_arg_194", "__E___E_help_arg_194_195", "rel_history", 
	"__E_history_196", "__E___E_history_196_197", "history_id", "abs_history", "__E_history_198", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", 
	"__N_5", "__N_6", "__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', 'a', 'b', 'k', 
	'r', ',', 'm', '<', '>', 'w', 'y', '[', ']', '?', 
	'x', '-', '|', '^', '&', '!', 'T', 'F', '_', 'S', 
	'+', '1', '0', '\'', '*', '/', '%', '~', '#', '{', 
	'}', '$', '\t', '\n', '\r', 'q', 'v', 'h', 'p', 'd', 
	'g', 'z', '"', 
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
			29, 31, 40, 223, 225, 226, 228, 236, 249, 260,
			269, 277, 284, 386, 391, 427
		},
		.to_inline = {
			{ 14, 77, 14 },
			{ 30, 153, 30 },
			{ 33 },
			{ 134, 187, 134 },
			{ 179, 153, 30 },
			{ 181, 153, 174 },
			{ 183, 153, 30 },
			{ 185, 153, 30 },
			{ 251 },
			{ 258 },
			{ 258, 269 },
			{ 268 },
			{ 272 },
			{ 318 },
			{ 330 },
			{ 348 },
			{ 353 },
			{ 378 },
			{ 431 }
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
//G59:  __E___E___E_library_17_18_19(58) => wff_rule(59).
	p(NT(58), (NT(59)));
//G60:  __E___E___E_library_17_18_19(58) => bf_rule(60).
	p(NT(58), (NT(60)));
//G61:  rule(57)             => __E___E___E_library_17_18_19(58).
	p(NT(57), (NT(58)));
//G62:  __E___E_library_17_18(56) => _(11) rule(57).
	p(NT(56), (NT(11)+NT(57)));
//G63:  __E___E_library_17_20(61) => null.
	p(NT(61), (nul));
//G64:  __E___E_library_17_20(61) => __E___E_library_17_18(56) __E___E_library_17_20(61).
	p(NT(61), (NT(56)+NT(61)));
//G65:  __E_library_17(55)   => __E___E_library_17_20(61).
	p(NT(55), (NT(61)));
//G66:  rules(54)            => __E_library_17(55).
	p(NT(54), (NT(55)));
//G67:  library(53)          => rules(54).
	p(NT(53), (NT(54)));
//G68:  wff_matcher(62)      => wff(14).
	p(NT(62), (NT(14)));
//G69:  wff_body(63)         => wff(14).
	p(NT(63), (NT(14)));
//G70:  wff_rule(59)         => wff_matcher(62) _(11) ':' ':' '=' _(11) wff_body(63) _(11) '.'.
	p(NT(59), (NT(62)+NT(11)+T(2)+T(2)+T(3)+NT(11)+NT(63)+NT(11)+T(1)));
//G71:  bf_matcher(64)       => bf(30).
	p(NT(64), (NT(30)));
//G72:  bf_body(65)          => bf(30).
	p(NT(65), (NT(30)));
//G73:  bf_rule(60)          => bf_matcher(64) _(11) ':' '=' _(11) bf_body(65) _(11) '.'.
	p(NT(60), (NT(64)+NT(11)+T(2)+T(3)+NT(11)+NT(65)+NT(11)+T(1)));
//G74:  builder(66)          => _(11) builder_head(67) _(11) builder_body(68) _(11) '.'.
	p(NT(66), (NT(11)+NT(67)+NT(11)+NT(68)+NT(11)+T(1)));
//G75:  __E_builder_head_21(69) => __(34) capture(29).
	p(NT(69), (NT(34)+NT(29)));
//G76:  __E_builder_head_22(70) => null.
	p(NT(70), (nul));
//G77:  __E_builder_head_22(70) => __E_builder_head_21(69) __E_builder_head_22(70).
	p(NT(70), (NT(69)+NT(70)));
//G78:  builder_head(67)     => '(' _(11) capture(29) __E_builder_head_22(70) _(11) ')'.
	p(NT(67), (T(12)+NT(11)+NT(29)+NT(70)+NT(11)+T(13)));
//G79:  __E_builder_body_23(72) => '=' ':' _(11) bf(30).
	p(NT(72), (T(3)+T(2)+NT(11)+NT(30)));
//G80:  bf_builder_body(71)  => __E_builder_body_23(72).
	p(NT(71), (NT(72)));
//G81:  builder_body(68)     => bf_builder_body(71).
	p(NT(68), (NT(71)));
//G82:  __E_builder_body_24(74) => '=' ':' ':' _(11) wff(14).
	p(NT(74), (T(3)+T(2)+T(2)+NT(11)+NT(14)));
//G83:  wff_builder_body(73) => __E_builder_body_24(74).
	p(NT(73), (NT(74)));
//G84:  builder_body(68)     => wff_builder_body(73).
	p(NT(68), (NT(73)));
//G85:  __E_tau_constant_source_25(76) => '.' _(11).
	p(NT(76), (T(1)+NT(11)));
//G86:  __E_tau_constant_source_25(76) => null.
	p(NT(76), (nul));
//G87:  tau_constant_source(75) => rec_relations(24) _(11) main(13) _(11) __E_tau_constant_source_25(76).
	p(NT(75), (NT(24)+NT(11)+NT(13)+NT(11)+NT(76)));
//G88:  __E_wff_26(78)       => '(' _(11) wff(14) _(11) ')'.
	p(NT(78), (T(12)+NT(11)+NT(14)+NT(11)+T(13)));
//G89:  wff_parenthesis(77)  => __E_wff_26(78).
	p(NT(77), (NT(78)));
//G90:  wff(14)              => wff_parenthesis(77).
	p(NT(14), (NT(77)));
//G91:  __E___E_wff_27_28(81) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(81), (T(15)+T(6)+T(21)+T(11)+T(8)+T(4)+T(21)+T(11)+T(15)));
//G92:  __E___E_wff_27_28(81) => '<' '>'.
	p(NT(81), (T(22)+T(23)));
//G93:  __E_wff_27(80)       => __E___E_wff_27_28(81) _(11) wff(14).
	p(NT(80), (NT(81)+NT(11)+NT(14)));
//G94:  wff_sometimes(79)    => __E_wff_27(80).
	p(NT(79), (NT(80)));
//G95:  wff(14)              => wff_sometimes(79).
	p(NT(14), (NT(79)));
//G96:  __E___E_wff_29_30(84) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(84), (T(16)+T(10)+T(24)+T(16)+T(25)+T(15)));
//G97:  __E___E_wff_29_30(84) => '[' ']'.
	p(NT(84), (T(26)+T(27)));
//G98:  __E_wff_29(83)       => __E___E_wff_29_30(84) _(11) wff(14).
	p(NT(83), (NT(84)+NT(11)+NT(14)));
//G99:  wff_always(82)       => __E_wff_29(83).
	p(NT(82), (NT(83)));
//G100: wff(14)              => wff_always(82).
	p(NT(14), (NT(82)));
//G101: __E_wff_31(86)       => wff(14) _(11) '?' _(11) wff(14) _(11) ':' _(11) wff(14).
	p(NT(86), (NT(14)+NT(11)+T(28)+NT(11)+NT(14)+NT(11)+T(2)+NT(11)+NT(14)));
//G102: wff_conditional(85)  => __E_wff_31(86).
	p(NT(85), (NT(86)));
//G103: wff(14)              => wff_conditional(85).
	p(NT(14), (NT(85)));
//G104: __E_wff_32(88)       => 'a' 'l' 'l' __(34) q_vars(89) __(34) wff(14).
	p(NT(88), (T(16)+T(10)+T(10)+NT(34)+NT(89)+NT(34)+NT(14)));
//G105: wff_all(87)          => __E_wff_32(88).
	p(NT(87), (NT(88)));
//G106: wff(14)              => wff_all(87).
	p(NT(14), (NT(87)));
//G107: __E_wff_33(91)       => 'e' 'x' __(34) q_vars(89) __(34) wff(14).
	p(NT(91), (T(11)+T(29)+NT(34)+NT(89)+NT(34)+NT(14)));
//G108: wff_ex(90)           => __E_wff_33(91).
	p(NT(90), (NT(91)));
//G109: wff(14)              => wff_ex(90).
	p(NT(14), (NT(90)));
//G110: wff_ref(92)          => ref(27).
	p(NT(92), (NT(27)));
//G111: wff(14)              => wff_ref(92).
	p(NT(14), (NT(92)));
//G112: wff(14)              => constraint(93).
	p(NT(14), (NT(93)));
//G113: __E_wff_34(95)       => wff(14) _(11) '-' '>' _(11) wff(14).
	p(NT(95), (NT(14)+NT(11)+T(30)+T(23)+NT(11)+NT(14)));
//G114: wff_imply(94)        => __E_wff_34(95).
	p(NT(94), (NT(95)));
//G115: wff(14)              => wff_imply(94).
	p(NT(14), (NT(94)));
//G116: __E_wff_35(97)       => wff(14) _(11) '<' '-' _(11) wff(14).
	p(NT(97), (NT(14)+NT(11)+T(22)+T(30)+NT(11)+NT(14)));
//G117: wff_rimply(96)       => __E_wff_35(97).
	p(NT(96), (NT(97)));
//G118: wff(14)              => wff_rimply(96).
	p(NT(14), (NT(96)));
//G119: __E_wff_36(99)       => wff(14) _(11) '<' '-' '>' _(11) wff(14).
	p(NT(99), (NT(14)+NT(11)+T(22)+T(30)+T(23)+NT(11)+NT(14)));
//G120: wff_equiv(98)        => __E_wff_36(99).
	p(NT(98), (NT(99)));
//G121: wff(14)              => wff_equiv(98).
	p(NT(14), (NT(98)));
//G122: __E_wff_37(101)      => wff(14) _(11) '|' '|' _(11) wff(14).
	p(NT(101), (NT(14)+NT(11)+T(31)+T(31)+NT(11)+NT(14)));
//G123: wff_or(100)          => __E_wff_37(101).
	p(NT(100), (NT(101)));
//G124: wff(14)              => wff_or(100).
	p(NT(14), (NT(100)));
//G125: __E_wff_38(103)      => wff(14) _(11) '^' _(11) wff(14).
	p(NT(103), (NT(14)+NT(11)+T(32)+NT(11)+NT(14)));
//G126: wff_xor(102)         => __E_wff_38(103).
	p(NT(102), (NT(103)));
//G127: wff(14)              => wff_xor(102).
	p(NT(14), (NT(102)));
//G128: __E_wff_39(105)      => wff(14) _(11) '&' '&' _(11) wff(14).
	p(NT(105), (NT(14)+NT(11)+T(33)+T(33)+NT(11)+NT(14)));
//G129: wff_and(104)         => __E_wff_39(105).
	p(NT(104), (NT(105)));
//G130: wff(14)              => wff_and(104).
	p(NT(14), (NT(104)));
//G131: __E_wff_40(107)      => '!' _(11) wff(14).
	p(NT(107), (T(34)+NT(11)+NT(14)));
//G132: wff_neg(106)         => __E_wff_40(107).
	p(NT(106), (NT(107)));
//G133: wff(14)              => wff_neg(106).
	p(NT(14), (NT(106)));
//G134: wff_t(108)           => 'T'.
	p(NT(108), (T(35)));
//G135: wff(14)              => wff_t(108).
	p(NT(14), (NT(108)));
//G136: wff_f(109)           => 'F'.
	p(NT(109), (T(36)));
//G137: wff(14)              => wff_f(109).
	p(NT(14), (NT(109)));
//G138: __E_wff_41(111)      => bf(30) _(11) '<' '=' _(11) bf(30) _(11) '<' '=' _(11) bf(30).
	p(NT(111), (NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)));
//G139: bf_interval(110)     => __E_wff_41(111).
	p(NT(110), (NT(111)));
//G140: wff(14)              => bf_interval(110).
	p(NT(14), (NT(110)));
//G141: __E_wff_42(113)      => bf(30) _(11) '=' _(11) bf(30).
	p(NT(113), (NT(30)+NT(11)+T(3)+NT(11)+NT(30)));
//G142: bf_eq(112)           => __E_wff_42(113).
	p(NT(112), (NT(113)));
//G143: wff(14)              => bf_eq(112).
	p(NT(14), (NT(112)));
//G144: __E_wff_43(115)      => bf(30) _(11) '!' '=' _(11) bf(30).
	p(NT(115), (NT(30)+NT(11)+T(34)+T(3)+NT(11)+NT(30)));
//G145: bf_neq(114)          => __E_wff_43(115).
	p(NT(114), (NT(115)));
//G146: wff(14)              => bf_neq(114).
	p(NT(14), (NT(114)));
//G147: __E_wff_44(117)      => bf(30) _(11) '<' '=' _(11) bf(30).
	p(NT(117), (NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)));
//G148: bf_lteq(116)         => __E_wff_44(117).
	p(NT(116), (NT(117)));
//G149: wff(14)              => bf_lteq(116).
	p(NT(14), (NT(116)));
//G150: __E_wff_45(119)      => bf(30) _(11) '!' '<' '=' _(11) bf(30).
	p(NT(119), (NT(30)+NT(11)+T(34)+T(22)+T(3)+NT(11)+NT(30)));
//G151: bf_nlteq(118)        => __E_wff_45(119).
	p(NT(118), (NT(119)));
//G152: wff(14)              => bf_nlteq(118).
	p(NT(14), (NT(118)));
//G153: __E_wff_46(121)      => bf(30) _(11) '>' _(11) bf(30).
	p(NT(121), (NT(30)+NT(11)+T(23)+NT(11)+NT(30)));
//G154: bf_gt(120)           => __E_wff_46(121).
	p(NT(120), (NT(121)));
//G155: wff(14)              => bf_gt(120).
	p(NT(14), (NT(120)));
//G156: __E_wff_47(123)      => bf(30) _(11) '!' '>' _(11) bf(30).
	p(NT(123), (NT(30)+NT(11)+T(34)+T(23)+NT(11)+NT(30)));
//G157: bf_ngt(122)          => __E_wff_47(123).
	p(NT(122), (NT(123)));
//G158: wff(14)              => bf_ngt(122).
	p(NT(14), (NT(122)));
//G159: __E_wff_48(125)      => bf(30) _(11) '>' '=' _(11) bf(30).
	p(NT(125), (NT(30)+NT(11)+T(23)+T(3)+NT(11)+NT(30)));
//G160: bf_gteq(124)         => __E_wff_48(125).
	p(NT(124), (NT(125)));
//G161: wff(14)              => bf_gteq(124).
	p(NT(14), (NT(124)));
//G162: __E_wff_49(127)      => bf(30) _(11) '!' '>' '=' _(11) bf(30).
	p(NT(127), (NT(30)+NT(11)+T(34)+T(23)+T(3)+NT(11)+NT(30)));
//G163: bf_ngteq(126)        => __E_wff_49(127).
	p(NT(126), (NT(127)));
//G164: wff(14)              => bf_ngteq(126).
	p(NT(14), (NT(126)));
//G165: __E_wff_50(129)      => bf(30) _(11) '<' _(11) bf(30).
	p(NT(129), (NT(30)+NT(11)+T(22)+NT(11)+NT(30)));
//G166: bf_lt(128)           => __E_wff_50(129).
	p(NT(128), (NT(129)));
//G167: wff(14)              => bf_lt(128).
	p(NT(14), (NT(128)));
//G168: __E_wff_51(131)      => bf(30) _(11) '!' '<' _(11) bf(30).
	p(NT(131), (NT(30)+NT(11)+T(34)+T(22)+NT(11)+NT(30)));
//G169: bf_nlt(130)          => __E_wff_51(131).
	p(NT(130), (NT(131)));
//G170: wff(14)              => bf_nlt(130).
	p(NT(14), (NT(130)));
//G171: __E_wff_52(133)      => bv(134) _(11) '=' '_' _(11) bv(134).
	p(NT(133), (NT(134)+NT(11)+T(3)+T(37)+NT(11)+NT(134)));
//G172: bv_eq(132)           => __E_wff_52(133).
	p(NT(132), (NT(133)));
//G173: wff(14)              => bv_eq(132).
	p(NT(14), (NT(132)));
//G174: __E_wff_53(136)      => bv(134) _(11) '!' '=' '_' _(11) bv(134).
	p(NT(136), (NT(134)+NT(11)+T(34)+T(3)+T(37)+NT(11)+NT(134)));
//G175: bv_neq(135)          => __E_wff_53(136).
	p(NT(135), (NT(136)));
//G176: wff(14)              => bv_neq(135).
	p(NT(14), (NT(135)));
//G177: __E_wff_54(138)      => bv(134) _(11) '<' '=' '_' _(11) bv(134).
	p(NT(138), (NT(134)+NT(11)+T(22)+T(3)+T(37)+NT(11)+NT(134)));
//G178: bv_lteq(137)         => __E_wff_54(138).
	p(NT(137), (NT(138)));
//G179: wff(14)              => bv_lteq(137).
	p(NT(14), (NT(137)));
//G180: __E_wff_55(140)      => bv(134) _(11) '!' '<' '=' '_' _(11) bv(134).
	p(NT(140), (NT(134)+NT(11)+T(34)+T(22)+T(3)+T(37)+NT(11)+NT(134)));
//G181: bv_nlteq(139)        => __E_wff_55(140).
	p(NT(139), (NT(140)));
//G182: wff(14)              => bv_nlteq(139).
	p(NT(14), (NT(139)));
//G183: __E_wff_56(142)      => bv(134) _(11) '>' '_' _(11) bv(134).
	p(NT(142), (NT(134)+NT(11)+T(23)+T(37)+NT(11)+NT(134)));
//G184: bv_gt(141)           => __E_wff_56(142).
	p(NT(141), (NT(142)));
//G185: wff(14)              => bv_gt(141).
	p(NT(14), (NT(141)));
//G186: __E_wff_57(144)      => bv(134) _(11) '!' '>' '_' _(11) bv(134).
	p(NT(144), (NT(134)+NT(11)+T(34)+T(23)+T(37)+NT(11)+NT(134)));
//G187: bv_ngt(143)          => __E_wff_57(144).
	p(NT(143), (NT(144)));
//G188: wff(14)              => bv_ngt(143).
	p(NT(14), (NT(143)));
//G189: __E_wff_58(146)      => bv(134) _(11) '>' '=' '_' _(11) bv(134).
	p(NT(146), (NT(134)+NT(11)+T(23)+T(3)+T(37)+NT(11)+NT(134)));
//G190: bv_gteq(145)         => __E_wff_58(146).
	p(NT(145), (NT(146)));
//G191: wff(14)              => bv_gteq(145).
	p(NT(14), (NT(145)));
//G192: __E_wff_59(148)      => bv(134) _(11) '!' '>' '=' '_' _(11) bv(134).
	p(NT(148), (NT(134)+NT(11)+T(34)+T(23)+T(3)+T(37)+NT(11)+NT(134)));
//G193: bv_ngteq(147)        => __E_wff_59(148).
	p(NT(147), (NT(148)));
//G194: wff(14)              => bv_ngteq(147).
	p(NT(14), (NT(147)));
//G195: __E_wff_60(150)      => bv(134) _(11) '<' '_' _(11) bv(134).
	p(NT(150), (NT(134)+NT(11)+T(22)+T(37)+NT(11)+NT(134)));
//G196: bv_lt(149)           => __E_wff_60(150).
	p(NT(149), (NT(150)));
//G197: wff(14)              => bv_lt(149).
	p(NT(14), (NT(149)));
//G198: __E_wff_61(152)      => bv(134) _(11) '!' '<' '_' _(11) bv(134).
	p(NT(152), (NT(134)+NT(11)+T(34)+T(22)+T(37)+NT(11)+NT(134)));
//G199: bv_nlt(151)          => __E_wff_61(152).
	p(NT(151), (NT(152)));
//G200: wff(14)              => bv_nlt(151).
	p(NT(14), (NT(151)));
//G201: wff(14)              => capture(29).
	p(NT(14), (NT(29)));
//G202: __E_bf_62(154)       => '(' _(11) bf(30) _(11) ')'.
	p(NT(154), (T(12)+NT(11)+NT(30)+NT(11)+T(13)));
//G203: bf_parenthesis(153)  => __E_bf_62(154).
	p(NT(153), (NT(154)));
//G204: bf(30)               => bf_parenthesis(153).
	p(NT(30), (NT(153)));
//G205: bf(30)               => bf_constant(155).
	p(NT(30), (NT(155)));
//G206: bf(30)               => variable(156).
	p(NT(30), (NT(156)));
//G207: __E_bf_63(158)       => 'S' _(11) '(' _(11) bf(30) _(11) ')'.
	p(NT(158), (T(38)+NT(11)+T(12)+NT(11)+NT(30)+NT(11)+T(13)));
//G208: bf_splitter(157)     => __E_bf_63(158).
	p(NT(157), (NT(158)));
//G209: bf(30)               => bf_splitter(157).
	p(NT(30), (NT(157)));
//G210: __E_bf_64(160)       => 'f' 'a' 'l' 'l' __(34) q_vars(89) __(34) bf(30).
	p(NT(160), (T(9)+T(16)+T(10)+T(10)+NT(34)+NT(89)+NT(34)+NT(30)));
//G211: bf_fall(159)         => __E_bf_64(160).
	p(NT(159), (NT(160)));
//G212: bf(30)               => bf_fall(159).
	p(NT(30), (NT(159)));
//G213: __E_bf_65(162)       => 'f' 'e' 'x' __(34) q_vars(89) __(34) bf(30).
	p(NT(162), (T(9)+T(11)+T(29)+NT(34)+NT(89)+NT(34)+NT(30)));
//G214: bf_fex(161)          => __E_bf_65(162).
	p(NT(161), (NT(162)));
//G215: bf(30)               => bf_fex(161).
	p(NT(30), (NT(161)));
//G216: bf_ref(163)          => ref(27).
	p(NT(163), (NT(27)));
//G217: bf(30)               => bf_ref(163).
	p(NT(30), (NT(163)));
//G218: __E_bf_66(165)       => bf(30) _(11) '|' _(11) bf(30).
	p(NT(165), (NT(30)+NT(11)+T(31)+NT(11)+NT(30)));
//G219: bf_or(164)           => __E_bf_66(165).
	p(NT(164), (NT(165)));
//G220: bf(30)               => bf_or(164).
	p(NT(30), (NT(164)));
//G221: __E_bf_67(167)       => bf(30) _(11) '+' _(11) bf(30).
	p(NT(167), (NT(30)+NT(11)+T(39)+NT(11)+NT(30)));
//G222: bf_xor(166)          => __E_bf_67(167).
	p(NT(166), (NT(167)));
//G223: bf(30)               => bf_xor(166).
	p(NT(30), (NT(166)));
//G224: __E___E_bf_68_69(170) => typed(33).
	p(NT(170), (NT(33)));
//G225: __E___E_bf_68_69(170) => null.
	p(NT(170), (nul));
//G226: __E_bf_68(169)       => '1' __E___E_bf_68_69(170).
	p(NT(169), (T(40)+NT(170)));
//G227: bf_t(168)            => __E_bf_68(169).
	p(NT(168), (NT(169)));
//G228: bf(30)               => bf_t(168).
	p(NT(30), (NT(168)));
//G229: __E___E_bf_70_71(173) => typed(33).
	p(NT(173), (NT(33)));
//G230: __E___E_bf_70_71(173) => null.
	p(NT(173), (nul));
//G231: __E_bf_70(172)       => '0' __E___E_bf_70_71(173).
	p(NT(172), (T(41)+NT(173)));
//G232: bf_f(171)            => __E_bf_70(172).
	p(NT(171), (NT(172)));
//G233: bf(30)               => bf_f(171).
	p(NT(30), (NT(171)));
//G234: __E___E_bf_72_73(176) => _(11) '&' _(11).
	p(NT(176), (NT(11)+T(33)+NT(11)));
//G235: __E___E_bf_72_73(176) => __(34).
	p(NT(176), (NT(34)));
//G236: __E_bf_72(175)       => bf(30) __E___E_bf_72_73(176) bf(30).
	p(NT(175), (NT(30)+NT(176)+NT(30)));
//G237: bf_and(174)          => __E_bf_72(175).
	p(NT(174), (NT(175)));
//G238: bf(30)               => bf_and(174).
	p(NT(30), (NT(174)));
//G239: __E___E_bf_74_75(180) => bf_parenthesis(153).
	p(NT(180), (NT(153)));
//G240: __E___E_bf_74_75(180) => bf_constant(155).
	p(NT(180), (NT(155)));
//G241: __E___E_bf_74_75(180) => variable(156).
	p(NT(180), (NT(156)));
//G242: __E___E_bf_74_75(180) => bf_splitter(157).
	p(NT(180), (NT(157)));
//G243: __E___E_bf_74_75(180) => bf_ref(163).
	p(NT(180), (NT(163)));
//G244: __E___E_bf_74_75(180) => bf_t(168).
	p(NT(180), (NT(168)));
//G245: __E___E_bf_74_75(180) => bf_f(171).
	p(NT(180), (NT(171)));
//G246: __E___E_bf_74_75(180) => bf_neg(177).
	p(NT(180), (NT(177)));
//G247: __E___E_bf_74_75(180) => capture(29).
	p(NT(180), (NT(29)));
//G248: bf_neg_oprnd(179)    => __E___E_bf_74_75(180).
	p(NT(179), (NT(180)));
//G249: __E_bf_74(178)       => bf_neg_oprnd(179) _(11) '\''.
	p(NT(178), (NT(179)+NT(11)+T(42)));
//G250: bf_neg(177)          => __E_bf_74(178).
	p(NT(177), (NT(178)));
//G251: bf(30)               => bf_neg(177).
	p(NT(30), (NT(177)));
//G252: __E___E_bf_76_77(184) => bf_parenthesis(153).
	p(NT(184), (NT(153)));
//G253: __E___E_bf_76_77(184) => variable(156).
	p(NT(184), (NT(156)));
//G254: __E___E_bf_76_77(184) => bf_splitter(157).
	p(NT(184), (NT(157)));
//G255: __E___E_bf_76_77(184) => bf_ref(163).
	p(NT(184), (NT(163)));
//G256: __E___E_bf_76_77(184) => bf_neg(177).
	p(NT(184), (NT(177)));
//G257: __E___E_bf_76_77(184) => bf_and_nosep(181).
	p(NT(184), (NT(181)));
//G258: __E___E_bf_76_77(184) => capture(29).
	p(NT(184), (NT(29)));
//G259: bf_and_nosep_1st_oprnd(183) => __E___E_bf_76_77(184).
	p(NT(183), (NT(184)));
//G260: __E___E_bf_76_78(186) => bf_parenthesis(153).
	p(NT(186), (NT(153)));
//G261: __E___E_bf_76_78(186) => bf_constant(155).
	p(NT(186), (NT(155)));
//G262: __E___E_bf_76_78(186) => variable(156).
	p(NT(186), (NT(156)));
//G263: __E___E_bf_76_78(186) => bf_splitter(157).
	p(NT(186), (NT(157)));
//G264: __E___E_bf_76_78(186) => bf_ref(163).
	p(NT(186), (NT(163)));
//G265: __E___E_bf_76_78(186) => bf_neg(177).
	p(NT(186), (NT(177)));
//G266: __E___E_bf_76_78(186) => capture(29).
	p(NT(186), (NT(29)));
//G267: bf_and_nosep_2nd_oprnd(185) => __E___E_bf_76_78(186).
	p(NT(185), (NT(186)));
//G268: __E_bf_76(182)       => bf_and_nosep_1st_oprnd(183) bf_and_nosep_2nd_oprnd(185).
	p(NT(182), (NT(183)+NT(185)));
//G269: bf_and_nosep(181)    => __E_bf_76(182).
	p(NT(181), (NT(182)));
//G270: bf(30)               => bf_and_nosep(181).
	p(NT(30), (NT(181)));
//G271: bf(30)               => capture(29).
	p(NT(30), (NT(29)));
//G272: __E_bv_79(188)       => '(' _(11) bv(134) _(11) ')'.
	p(NT(188), (T(12)+NT(11)+NT(134)+NT(11)+T(13)));
//G273: bv_parenthesis(187)  => __E_bv_79(188).
	p(NT(187), (NT(188)));
//G274: bv(134)              => bv_parenthesis(187).
	p(NT(134), (NT(187)));
//G275: __E_bv_80(190)       => '[' bv(134) ']'.
	p(NT(190), (T(26)+NT(134)+T(27)));
//G276: bv_checked(189)      => __E_bv_80(190).
	p(NT(189), (NT(190)));
//G277: bv(134)              => bv_checked(189).
	p(NT(134), (NT(189)));
//G278: bv(134)              => variable(156).
	p(NT(134), (NT(156)));
//G279: __E_bv_81(192)       => bv(134) _(11) '+' _(11) bv(134).
	p(NT(192), (NT(134)+NT(11)+T(39)+NT(11)+NT(134)));
//G280: bv_add(191)          => __E_bv_81(192).
	p(NT(191), (NT(192)));
//G281: bv(134)              => bv_add(191).
	p(NT(134), (NT(191)));
//G282: __E_bv_82(194)       => bv(134) _(11) '*' _(11) bv(134).
	p(NT(194), (NT(134)+NT(11)+T(43)+NT(11)+NT(134)));
//G283: bv_mul(193)          => __E_bv_82(194).
	p(NT(193), (NT(194)));
//G284: bv(134)              => bv_mul(193).
	p(NT(134), (NT(193)));
//G285: __E_bv_83(196)       => bv(134) _(11) '/' _(11) bv(134).
	p(NT(196), (NT(134)+NT(11)+T(44)+NT(11)+NT(134)));
//G286: bv_div(195)          => __E_bv_83(196).
	p(NT(195), (NT(196)));
//G287: bv(134)              => bv_div(195).
	p(NT(134), (NT(195)));
//G288: __E_bv_84(198)       => bv(134) _(11) '%' _(11) bv(134).
	p(NT(198), (NT(134)+NT(11)+T(45)+NT(11)+NT(134)));
//G289: bv_mod(197)          => __E_bv_84(198).
	p(NT(197), (NT(198)));
//G290: bv(134)              => bv_mod(197).
	p(NT(134), (NT(197)));
//G291: __E_bv_85(200)       => bv(134) _(11) '-' _(11) bv(134).
	p(NT(200), (NT(134)+NT(11)+T(30)+NT(11)+NT(134)));
//G292: bv_sub(199)          => __E_bv_85(200).
	p(NT(199), (NT(200)));
//G293: bv(134)              => bv_sub(199).
	p(NT(134), (NT(199)));
//G294: bv_neg(201)          => bv(134).
	p(NT(201), (NT(134)));
//G295: bv(134)              => '~' _(11) bv_neg(201).
	p(NT(134), (T(46)+NT(11)+NT(201)));
//G296: __E_bv_86(203)       => bv(134) _(11) '&' _(11) bv(134).
	p(NT(203), (NT(134)+NT(11)+T(33)+NT(11)+NT(134)));
//G297: bv_and(202)          => __E_bv_86(203).
	p(NT(202), (NT(203)));
//G298: bv(134)              => bv_and(202).
	p(NT(134), (NT(202)));
//G299: __E_bv_87(205)       => bv(134) _(11) '!' '&' _(11) bv(134).
	p(NT(205), (NT(134)+NT(11)+T(34)+T(33)+NT(11)+NT(134)));
//G300: bv_nand(204)         => __E_bv_87(205).
	p(NT(204), (NT(205)));
//G301: bv(134)              => bv_nand(204).
	p(NT(134), (NT(204)));
//G302: __E_bv_88(207)       => bv(134) _(11) '|' _(11) bv(134).
	p(NT(207), (NT(134)+NT(11)+T(31)+NT(11)+NT(134)));
//G303: bv_or(206)           => __E_bv_88(207).
	p(NT(206), (NT(207)));
//G304: bv(134)              => bv_or(206).
	p(NT(134), (NT(206)));
//G305: __E_bv_89(209)       => bv(134) _(11) '!' '|' _(11) bv(134).
	p(NT(209), (NT(134)+NT(11)+T(34)+T(31)+NT(11)+NT(134)));
//G306: bv_nor(208)          => __E_bv_89(209).
	p(NT(208), (NT(209)));
//G307: bv(134)              => bv_nor(208).
	p(NT(134), (NT(208)));
//G308: __E_bv_90(211)       => bv(134) _(11) '(' '+' ')' _(11) bv(134).
	p(NT(211), (NT(134)+NT(11)+T(12)+T(39)+T(13)+NT(11)+NT(134)));
//G309: bv_xor(210)          => __E_bv_90(211).
	p(NT(210), (NT(211)));
//G310: bv(134)              => bv_xor(210).
	p(NT(134), (NT(210)));
//G311: __E_bv_91(213)       => bv(134) _(11) '!' '(' '+' ')' _(11) bv(134).
	p(NT(213), (NT(134)+NT(11)+T(34)+T(12)+T(39)+T(13)+NT(11)+NT(134)));
//G312: bv_xnor(212)         => __E_bv_91(213).
	p(NT(212), (NT(213)));
//G313: bv(134)              => bv_xnor(212).
	p(NT(134), (NT(212)));
//G314: __E_bv_92(215)       => bv(134) _(11) '<' '<' _(11) bv(134).
	p(NT(215), (NT(134)+NT(11)+T(22)+T(22)+NT(11)+NT(134)));
//G315: bv_rotate_left(214)  => __E_bv_92(215).
	p(NT(214), (NT(215)));
//G316: bv(134)              => bv_rotate_left(214).
	p(NT(134), (NT(214)));
//G317: __E_bv_93(217)       => bv(134) _(11) '>' '>' _(11) bv(134).
	p(NT(217), (NT(134)+NT(11)+T(23)+T(23)+NT(11)+NT(134)));
//G318: bv_rotate_right(216) => __E_bv_93(217).
	p(NT(216), (NT(217)));
//G319: bv(134)              => bv_rotate_right(216).
	p(NT(134), (NT(216)));
//G320: __E_bv_94(219)       => 'm' 'i' 'n' __(34) bv(134) __(34) bv(134).
	p(NT(219), (T(21)+T(4)+T(5)+NT(34)+NT(134)+NT(34)+NT(134)));
//G321: bv_min(218)          => __E_bv_94(219).
	p(NT(218), (NT(219)));
//G322: bv(134)              => bv_min(218).
	p(NT(134), (NT(218)));
//G323: __E_bv_95(221)       => 'm' 'a' 'x' __(34) bv(134) __(34) bv(134).
	p(NT(221), (T(21)+T(16)+T(29)+NT(34)+NT(134)+NT(34)+NT(134)));
//G324: bv_max(220)          => __E_bv_95(221).
	p(NT(220), (NT(221)));
//G325: bv(134)              => bv_max(220).
	p(NT(134), (NT(220)));
//G326: bv(134)              => bv_constant(222).
	p(NT(134), (NT(222)));
//G327: __E_bv_constant_96(224) => _(11) ':' _(11) type(225) _(11).
	p(NT(224), (NT(11)+T(2)+NT(11)+NT(225)+NT(11)));
//G328: __E_bv_constant_96(224) => null.
	p(NT(224), (nul));
//G329: bv_constant(222)     => '#' 'b' binary(223) __E_bv_constant_96(224).
	p(NT(222), (T(47)+T(17)+NT(223)+NT(224)));
//G330: __E_bv_constant_97(227) => _(11) ':' _(11) type(225) _(11).
	p(NT(227), (NT(11)+T(2)+NT(11)+NT(225)+NT(11)));
//G331: __E_bv_constant_97(227) => null.
	p(NT(227), (nul));
//G332: bv_constant(222)     => decimal(226) __E_bv_constant_97(227).
	p(NT(222), (NT(226)+NT(227)));
//G333: __E_bv_constant_98(229) => _(11) ':' _(11) type(225) _(11).
	p(NT(229), (NT(11)+T(2)+NT(11)+NT(225)+NT(11)));
//G334: __E_bv_constant_98(229) => null.
	p(NT(229), (nul));
//G335: bv_constant(222)     => '#' 'x' hexadecimal(228) __E_bv_constant_98(229).
	p(NT(222), (T(47)+T(29)+NT(228)+NT(229)));
//G336: __E_binary_99(230)   => '0'.
	p(NT(230), (T(41)));
//G337: __E_binary_99(230)   => '1'.
	p(NT(230), (T(40)));
//G338: __E_binary_100(231)  => __E_binary_99(230).
	p(NT(231), (NT(230)));
//G339: __E_binary_100(231)  => __E_binary_99(230) __E_binary_100(231).
	p(NT(231), (NT(230)+NT(231)));
//G340: binary(223)          => __E_binary_100(231).
	p(NT(223), (NT(231)));
//G341: __E_hexadecimal_101(232) => xdigit(4).
	p(NT(232), (NT(4)));
//G342: __E_hexadecimal_101(232) => xdigit(4) __E_hexadecimal_101(232).
	p(NT(232), (NT(4)+NT(232)));
//G343: hexadecimal(228)     => __E_hexadecimal_101(232).
	p(NT(228), (NT(232)));
//G344: __E_decimal_102(233) => digit(3).
	p(NT(233), (NT(3)));
//G345: __E_decimal_102(233) => digit(3) __E_decimal_102(233).
	p(NT(233), (NT(3)+NT(233)));
//G346: decimal(226)         => __E_decimal_102(233).
	p(NT(226), (NT(233)));
//G347: __E_constraint_103(235) => '[' ctnvar(236) _(11) '!' '=' _(11) num(237) ']'.
	p(NT(235), (T(26)+NT(236)+NT(11)+T(34)+T(3)+NT(11)+NT(237)+T(27)));
//G348: __E_constraint_103(235) => '[' num(237) _(11) '!' '=' _(11) ctnvar(236) ']'.
	p(NT(235), (T(26)+NT(237)+NT(11)+T(34)+T(3)+NT(11)+NT(236)+T(27)));
//G349: ctn_neq(234)         => __E_constraint_103(235).
	p(NT(234), (NT(235)));
//G350: constraint(93)       => ctn_neq(234).
	p(NT(93), (NT(234)));
//G351: __E_constraint_104(239) => '[' ctnvar(236) _(11) '=' _(11) num(237) ']'.
	p(NT(239), (T(26)+NT(236)+NT(11)+T(3)+NT(11)+NT(237)+T(27)));
//G352: __E_constraint_104(239) => '[' num(237) _(11) '=' _(11) ctnvar(236) ']'.
	p(NT(239), (T(26)+NT(237)+NT(11)+T(3)+NT(11)+NT(236)+T(27)));
//G353: ctn_eq(238)          => __E_constraint_104(239).
	p(NT(238), (NT(239)));
//G354: constraint(93)       => ctn_eq(238).
	p(NT(93), (NT(238)));
//G355: __E_constraint_105(241) => '[' ctnvar(236) _(11) '>' '=' _(11) num(237) ']'.
	p(NT(241), (T(26)+NT(236)+NT(11)+T(23)+T(3)+NT(11)+NT(237)+T(27)));
//G356: __E_constraint_105(241) => '[' num(237) _(11) '>' '=' _(11) ctnvar(236) ']'.
	p(NT(241), (T(26)+NT(237)+NT(11)+T(23)+T(3)+NT(11)+NT(236)+T(27)));
//G357: ctn_gteq(240)        => __E_constraint_105(241).
	p(NT(240), (NT(241)));
//G358: constraint(93)       => ctn_gteq(240).
	p(NT(93), (NT(240)));
//G359: __E_constraint_106(243) => '[' ctnvar(236) _(11) '>' _(11) num(237) ']'.
	p(NT(243), (T(26)+NT(236)+NT(11)+T(23)+NT(11)+NT(237)+T(27)));
//G360: __E_constraint_106(243) => '[' num(237) _(11) '>' _(11) ctnvar(236) ']'.
	p(NT(243), (T(26)+NT(237)+NT(11)+T(23)+NT(11)+NT(236)+T(27)));
//G361: ctn_gt(242)          => __E_constraint_106(243).
	p(NT(242), (NT(243)));
//G362: constraint(93)       => ctn_gt(242).
	p(NT(93), (NT(242)));
//G363: __E_constraint_107(245) => '[' ctnvar(236) _(11) '<' '=' _(11) num(237) ']'.
	p(NT(245), (T(26)+NT(236)+NT(11)+T(22)+T(3)+NT(11)+NT(237)+T(27)));
//G364: __E_constraint_107(245) => '[' num(237) _(11) '<' '=' _(11) ctnvar(236) ']'.
	p(NT(245), (T(26)+NT(237)+NT(11)+T(22)+T(3)+NT(11)+NT(236)+T(27)));
//G365: ctn_lteq(244)        => __E_constraint_107(245).
	p(NT(244), (NT(245)));
//G366: constraint(93)       => ctn_lteq(244).
	p(NT(93), (NT(244)));
//G367: __E_constraint_108(247) => '[' ctnvar(236) _(11) '<' _(11) num(237) ']'.
	p(NT(247), (T(26)+NT(236)+NT(11)+T(22)+NT(11)+NT(237)+T(27)));
//G368: __E_constraint_108(247) => '[' num(237) _(11) '<' _(11) ctnvar(236) ']'.
	p(NT(247), (T(26)+NT(237)+NT(11)+T(22)+NT(11)+NT(236)+T(27)));
//G369: ctn_lt(246)          => __E_constraint_108(247).
	p(NT(246), (NT(247)));
//G370: constraint(93)       => ctn_lt(246).
	p(NT(93), (NT(246)));
//G371: __E_bf_constant_109(248) => capture(29).
	p(NT(248), (NT(29)));
//G372: __E_bf_constant_109(248) => source(249).
	p(NT(248), (NT(249)));
//G373: __E_bf_constant_110(250) => typed(33).
	p(NT(250), (NT(33)));
//G374: __E_bf_constant_110(250) => null.
	p(NT(250), (nul));
//G375: bf_constant(155)     => '{' _(11) __E_bf_constant_109(248) _(11) '}' __E_bf_constant_110(250).
	p(NT(155), (T(48)+NT(11)+NT(248)+NT(11)+T(49)+NT(250)));
//G376: __E___E_source_111_112(253) => src_c(251).
	p(NT(253), (NT(251)));
//G377: __E___E_source_111_112(253) => space(2).
	p(NT(253), (NT(2)));
//G378: __E___E_source_111_113(254) => null.
	p(NT(254), (nul));
//G379: __E___E_source_111_113(254) => __E___E_source_111_112(253) __E___E_source_111_113(254).
	p(NT(254), (NT(253)+NT(254)));
//G380: __E_source_111(252)  => __E___E_source_111_113(254) src_c(251).
	p(NT(252), (NT(254)+NT(251)));
//G381: __E_source_111(252)  => null.
	p(NT(252), (nul));
//G382: source(249)          => src_c(251) __E_source_111(252).
	p(NT(249), (NT(251)+NT(252)));
//G383: src_c(251)           => alnum(6).
	p(NT(251), (NT(6)));
//G384: __N_0(445)           => '{'.
	p(NT(445), (T(48)));
//G385: __N_1(446)           => '}'.
	p(NT(446), (T(49)));
//G386: src_c(251)           => ~( __N_0(445) ) & ~( __N_1(446) ) & punct(7).	 # conjunctive
	p(NT(251), ~(NT(445)) & ~(NT(446)) & (NT(7)));
//G387: __E_src_c_114(255)   => src_c(251).
	p(NT(255), (NT(251)));
//G388: __E_src_c_114(255)   => space(2).
	p(NT(255), (NT(2)));
//G389: __E_src_c_115(256)   => null.
	p(NT(256), (nul));
//G390: __E_src_c_115(256)   => __E_src_c_114(255) __E_src_c_115(256).
	p(NT(256), (NT(255)+NT(256)));
//G391: src_c(251)           => '{' __E_src_c_115(256) '}'.
	p(NT(251), (T(48)+NT(256)+T(49)));
//G392: __E_variable_116(257) => uconst(258).
	p(NT(257), (NT(258)));
//G393: __E_variable_116(257) => io_var(259).
	p(NT(257), (NT(259)));
//G394: __E_variable_116(257) => var_name(260).
	p(NT(257), (NT(260)));
//G395: __E_variable_117(261) => typed(33).
	p(NT(261), (NT(33)));
//G396: __E_variable_117(261) => null.
	p(NT(261), (nul));
//G397: variable(156)        => __E_variable_116(257) __E_variable_117(261).
	p(NT(156), (NT(257)+NT(261)));
//G398: __N_2(447)           => 'F'.
	p(NT(447), (T(36)));
//G399: __N_3(448)           => 'T'.
	p(NT(448), (T(35)));
//G400: __E_var_name_118(262) => ~( __N_2(447) ) & ~( __N_3(448) ) & alpha(5).	 # conjunctive
	p(NT(262), ~(NT(447)) & ~(NT(448)) & (NT(5)));
//G401: __E_var_name_119(263) => null.
	p(NT(263), (nul));
//G402: __E_var_name_119(263) => digit(3) __E_var_name_119(263).
	p(NT(263), (NT(3)+NT(263)));
//G403: var_name(260)        => __E_var_name_118(262) __E_var_name_119(263).	 # guarded: charvar
	p(NT(260), (NT(262)+NT(263)));
	p.back().guard = "charvar";
//G404: __N_4(449)           => 'F'.
	p(NT(449), (T(36)));
//G405: __N_5(450)           => 'T'.
	p(NT(450), (T(35)));
//G406: __E_var_name_120(264) => ~( __N_4(449) ) & ~( __N_5(450) ) & alpha(5).	 # conjunctive
	p(NT(264), ~(NT(449)) & ~(NT(450)) & (NT(5)));
//G407: __E_var_name_121(265) => alnum(6).
	p(NT(265), (NT(6)));
//G408: __E_var_name_121(265) => '_'.
	p(NT(265), (T(37)));
//G409: __E_var_name_122(266) => null.
	p(NT(266), (nul));
//G410: __E_var_name_122(266) => __E_var_name_121(265) __E_var_name_122(266).
	p(NT(266), (NT(265)+NT(266)));
//G411: var_name(260)        => __E_var_name_120(264) __E_var_name_122(266).	 # guarded: var
	p(NT(260), (NT(264)+NT(266)));
	p.back().guard = "var";
//G412: io_var(259)          => io_var_name(31) '[' offset(267) ']'.
	p(NT(259), (NT(31)+T(26)+NT(267)+T(27)));
//G413: io_var_name(31)      => chars(268).
	p(NT(31), (NT(268)));
//G414: __E___E_uconst_123_124(271) => chars(268) _(11).
	p(NT(271), (NT(268)+NT(11)));
//G415: __E___E_uconst_123_124(271) => null.
	p(NT(271), (nul));
//G416: __E_uconst_123(270)  => __E___E_uconst_123_124(271) ':' _(11) chars(268).
	p(NT(270), (NT(271)+T(2)+NT(11)+NT(268)));
//G417: uconst_name(269)     => __E_uconst_123(270).
	p(NT(269), (NT(270)));
//G418: uconst(258)          => '<' _(11) uconst_name(269) _(11) '>'.
	p(NT(258), (T(22)+NT(11)+NT(269)+NT(11)+T(23)));
//G419: __E_q_vars_125(273)  => _(11) ',' _(11) q_var(272).
	p(NT(273), (NT(11)+T(20)+NT(11)+NT(272)));
//G420: __E_q_vars_126(274)  => null.
	p(NT(274), (nul));
//G421: __E_q_vars_126(274)  => __E_q_vars_125(273) __E_q_vars_126(274).
	p(NT(274), (NT(273)+NT(274)));
//G422: q_vars(89)           => q_var(272) __E_q_vars_126(274).
	p(NT(89), (NT(272)+NT(274)));
//G423: q_var(272)           => capture(29).
	p(NT(272), (NT(29)));
//G424: __N_6(451)           => uconst(258).
	p(NT(451), (NT(258)));
//G425: q_var(272)           => variable(156) & ~( __N_6(451) ).	 # conjunctive
	p(NT(272), (NT(156)) & ~(NT(451)));
//G426: ctnvar(236)          => chars(268).
	p(NT(236), (NT(268)));
//G427: __E_offsets_127(275) => _(11) ',' _(11) offset(267).
	p(NT(275), (NT(11)+T(20)+NT(11)+NT(267)));
//G428: __E_offsets_128(276) => null.
	p(NT(276), (nul));
//G429: __E_offsets_128(276) => __E_offsets_127(275) __E_offsets_128(276).
	p(NT(276), (NT(275)+NT(276)));
//G430: offsets(42)          => '[' _(11) offset(267) __E_offsets_128(276) _(11) ']'.
	p(NT(42), (T(26)+NT(11)+NT(267)+NT(276)+NT(11)+T(27)));
//G431: offset(267)          => integer(277).
	p(NT(267), (NT(277)));
//G432: offset(267)          => capture(29).
	p(NT(267), (NT(29)));
//G433: offset(267)          => shift(278).
	p(NT(267), (NT(278)));
//G434: __N_7(452)           => io_var(259).
	p(NT(452), (NT(259)));
//G435: __E_offset_129(279)  => variable(156) & ~( __N_7(452) ).	 # conjunctive
	p(NT(279), (NT(156)) & ~(NT(452)));
//G436: offset(267)          => __E_offset_129(279).
	p(NT(267), (NT(279)));
//G437: __E_shift_130(280)   => capture(29).
	p(NT(280), (NT(29)));
//G438: __N_8(453)           => io_var(259).
	p(NT(453), (NT(259)));
//G439: __E___E_shift_130_131(281) => variable(156) & ~( __N_8(453) ).	 # conjunctive
	p(NT(281), (NT(156)) & ~(NT(453)));
//G440: __E_shift_130(280)   => __E___E_shift_130_131(281).
	p(NT(280), (NT(281)));
//G441: shift(278)           => __E_shift_130(280) _(11) '-' _(11) num(237).
	p(NT(278), (NT(280)+NT(11)+T(30)+NT(11)+NT(237)));
//G442: __E_chars_132(282)   => alnum(6).
	p(NT(282), (NT(6)));
//G443: __E_chars_132(282)   => '_'.
	p(NT(282), (T(37)));
//G444: __E_chars_133(283)   => null.
	p(NT(283), (nul));
//G445: __E_chars_133(283)   => __E_chars_132(282) __E_chars_133(283).
	p(NT(283), (NT(282)+NT(283)));
//G446: chars(268)           => alpha(5) __E_chars_133(283).
	p(NT(268), (NT(5)+NT(283)));
//G447: __E_digits_134(285)  => digit(3).
	p(NT(285), (NT(3)));
//G448: __E_digits_134(285)  => digit(3) __E_digits_134(285).
	p(NT(285), (NT(3)+NT(285)));
//G449: digits(284)          => __E_digits_134(285).
	p(NT(284), (NT(285)));
//G450: num(237)             => digits(284).
	p(NT(237), (NT(284)));
//G451: __E_integer_135(286) => '-'.
	p(NT(286), (T(30)));
//G452: __E_integer_135(286) => null.
	p(NT(286), (nul));
//G453: integer(277)         => __E_integer_135(286) _(11) digits(284).
	p(NT(277), (NT(286)+NT(11)+NT(284)));
//G454: sym(40)              => chars(268).
	p(NT(40), (NT(268)));
//G455: capture(29)          => '$' chars(268).
	p(NT(29), (T(50)+NT(268)));
//G456: typed(33)            => _(11) ':' _(11) type(225).
	p(NT(33), (NT(11)+T(2)+NT(11)+NT(225)));
//G457: type(225)            => chars(268).
	p(NT(225), (NT(268)));
//G458: scope_id(287)        => digits(284).
	p(NT(287), (NT(284)));
//G459: __E_comment_136(289) => printable(8).
	p(NT(289), (NT(8)));
//G460: __E_comment_136(289) => '\t'.
	p(NT(289), (T(51)));
//G461: __E_comment_137(290) => null.
	p(NT(290), (nul));
//G462: __E_comment_137(290) => __E_comment_136(289) __E_comment_137(290).
	p(NT(290), (NT(289)+NT(290)));
//G463: __E_comment_138(291) => '\n'.
	p(NT(291), (T(52)));
//G464: __E_comment_138(291) => '\r'.
	p(NT(291), (T(53)));
//G465: __E_comment_138(291) => eof(1).
	p(NT(291), (NT(1)));
//G466: comment(288)         => '#' __E_comment_137(290) __E_comment_138(291).
	p(NT(288), (T(47)+NT(290)+NT(291)));
//G467: __(34)               => space(2).
	p(NT(34), (NT(2)));
//G468: __(34)               => comment(288).
	p(NT(34), (NT(288)));
//G469: __(34)               => __(34) space(2).
	p(NT(34), (NT(34)+NT(2)));
//G470: __(34)               => __(34) comment(288).
	p(NT(34), (NT(34)+NT(288)));
//G471: _(11)                => __(34).
	p(NT(11), (NT(34)));
//G472: _(11)                => null.
	p(NT(11), (nul));
//G473: cli(292)             => _(11).
	p(NT(292), (NT(11)));
//G474: __E_cli_139(294)     => '.' _(11) cli_command(293) _(11).
	p(NT(294), (T(1)+NT(11)+NT(293)+NT(11)));
//G475: __E_cli_140(295)     => null.
	p(NT(295), (nul));
//G476: __E_cli_140(295)     => __E_cli_139(294) __E_cli_140(295).
	p(NT(295), (NT(294)+NT(295)));
//G477: cli(292)             => _(11) cli_command(293) _(11) __E_cli_140(295).
	p(NT(292), (NT(11)+NT(293)+NT(11)+NT(295)));
//G478: __E_cli_command_141(298) => 'q'.
	p(NT(298), (T(54)));
//G479: __E_cli_command_141(298) => 'q' 'u' 'i' 't'.
	p(NT(298), (T(54)+T(7)+T(4)+T(8)));
//G480: quit_sym(297)        => __E_cli_command_141(298).
	p(NT(297), (NT(298)));
//G481: quit_cmd(296)        => quit_sym(297).
	p(NT(296), (NT(297)));
//G482: cli_command(293)     => quit_cmd(296).
	p(NT(293), (NT(296)));
//G483: __E_cli_command_142(301) => 'v'.
	p(NT(301), (T(55)));
//G484: __E_cli_command_142(301) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(301), (T(55)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G485: version_sym(300)     => __E_cli_command_142(301).
	p(NT(300), (NT(301)));
//G486: version_cmd(299)     => version_sym(300).
	p(NT(299), (NT(300)));
//G487: cli_command(293)     => version_cmd(299).
	p(NT(293), (NT(299)));
//G488: __E_cli_command_143(304) => 'c'.
	p(NT(304), (T(14)));
//G489: __E_cli_command_143(304) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(304), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G490: clear_sym(303)       => __E_cli_command_143(304).
	p(NT(303), (NT(304)));
//G491: clear_cmd(302)       => clear_sym(303).
	p(NT(302), (NT(303)));
//G492: cli_command(293)     => clear_cmd(302).
	p(NT(293), (NT(302)));
//G493: __E___E_cli_command_144_145(308) => 'h'.
	p(NT(308), (T(56)));
//G494: __E___E_cli_command_144_145(308) => 'h' 'e' 'l' 'p'.
	p(NT(308), (T(56)+T(11)+T(10)+T(57)));
//G495: help_sym(307)        => __E___E_cli_command_144_145(308).
	p(NT(307), (NT(308)));
//G496: __E___E_cli_command_144_146(309) => __(34) help_arg(310).
	p(NT(309), (NT(34)+NT(310)));
//G497: __E___E_cli_command_144_146(309) => null.
	p(NT(309), (nul));
//G498: __E_cli_command_144(306) => help_sym(307) __E___E_cli_command_144_146(309).
	p(NT(306), (NT(307)+NT(309)));
//G499: help_cmd(305)        => __E_cli_command_144(306).
	p(NT(305), (NT(306)));
//G500: cli_command(293)     => help_cmd(305).
	p(NT(293), (NT(305)));
//G501: file_sym(313)        => 'f' 'i' 'l' 'e'.
	p(NT(313), (T(9)+T(4)+T(10)+T(11)));
//G502: __E_cli_command_147(312) => file_sym(313) __(34) q_string(314).
	p(NT(312), (NT(313)+NT(34)+NT(314)));
//G503: file_cmd(311)        => __E_cli_command_147(312).
	p(NT(311), (NT(312)));
//G504: cli_command(293)     => file_cmd(311).
	p(NT(293), (NT(311)));
//G505: valid_sym(317)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(317), (T(55)+T(16)+T(10)+T(4)+T(58)));
//G506: __E_cli_command_148(316) => valid_sym(317) __(34) normalize_cmd_arg(318).
	p(NT(316), (NT(317)+NT(34)+NT(318)));
//G507: valid_cmd(315)       => __E_cli_command_148(316).
	p(NT(315), (NT(316)));
//G508: cli_command(293)     => valid_cmd(315).
	p(NT(293), (NT(315)));
//G509: sat_sym(321)         => 's' 'a' 't'.
	p(NT(321), (T(15)+T(16)+T(8)));
//G510: __E_cli_command_149(320) => sat_sym(321) __(34) normalize_cmd_arg(318).
	p(NT(320), (NT(321)+NT(34)+NT(318)));
//G511: sat_cmd(319)         => __E_cli_command_149(320).
	p(NT(319), (NT(320)));
//G512: cli_command(293)     => sat_cmd(319).
	p(NT(293), (NT(319)));
//G513: unsat_sym(324)       => 'u' 'n' 's' 'a' 't'.
	p(NT(324), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G514: __E_cli_command_150(323) => unsat_sym(324) __(34) normalize_cmd_arg(318).
	p(NT(323), (NT(324)+NT(34)+NT(318)));
//G515: unsat_cmd(322)       => __E_cli_command_150(323).
	p(NT(322), (NT(323)));
//G516: cli_command(293)     => unsat_cmd(322).
	p(NT(293), (NT(322)));
//G517: solve_sym(327)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(327), (T(15)+T(6)+T(10)+T(55)+T(11)));
//G518: __E___E_cli_command_151_152(328) => solve_options(329).
	p(NT(328), (NT(329)));
//G519: __E___E_cli_command_151_152(328) => null.
	p(NT(328), (nul));
//G520: __E_cli_command_151(326) => solve_sym(327) __E___E_cli_command_151_152(328) __(34) wff_cmd_arg(330).
	p(NT(326), (NT(327)+NT(328)+NT(34)+NT(330)));
//G521: solve_cmd(325)       => __E_cli_command_151(326).
	p(NT(325), (NT(326)));
//G522: cli_command(293)     => solve_cmd(325).
	p(NT(293), (NT(325)));
//G523: lgrs_sym(333)        => 'l' 'g' 'r' 's'.
	p(NT(333), (T(10)+T(59)+T(19)+T(15)));
//G524: __E_cli_command_153(332) => lgrs_sym(333) __(34) wff_cmd_arg(330).
	p(NT(332), (NT(333)+NT(34)+NT(330)));
//G525: lgrs_cmd(331)        => __E_cli_command_153(332).
	p(NT(331), (NT(332)));
//G526: cli_command(293)     => lgrs_cmd(331).
	p(NT(293), (NT(331)));
//G527: __E___E_cli_command_154_155(337) => 'r'.
	p(NT(337), (T(19)));
//G528: __E___E_cli_command_154_155(337) => 'r' 'u' 'n'.
	p(NT(337), (T(19)+T(7)+T(5)));
//G529: run_sym(336)         => __E___E_cli_command_154_155(337).
	p(NT(336), (NT(337)));
//G530: __E___E_cli_command_154_156(338) => history(339).
	p(NT(338), (NT(339)));
//G531: __E___E_cli_command_154_156(338) => wff(14).
	p(NT(338), (NT(14)));
//G532: __E_cli_command_154(335) => run_sym(336) __(34) __E___E_cli_command_154_156(338).
	p(NT(335), (NT(336)+NT(34)+NT(338)));
//G533: run_cmd(334)         => __E_cli_command_154(335).
	p(NT(334), (NT(335)));
//G534: cli_command(293)     => run_cmd(334).
	p(NT(293), (NT(334)));
//G535: __E___E_cli_command_157_158(343) => 'n'.
	p(NT(343), (T(5)));
//G536: __E___E_cli_command_157_158(343) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(343), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(60)+T(11)));
//G537: normalize_sym(342)   => __E___E_cli_command_157_158(343).
	p(NT(342), (NT(343)));
//G538: __E_cli_command_157(341) => normalize_sym(342) __(34) normalize_cmd_arg(318).
	p(NT(341), (NT(342)+NT(34)+NT(318)));
//G539: normalize_cmd(340)   => __E_cli_command_157(341).
	p(NT(340), (NT(341)));
//G540: cli_command(293)     => normalize_cmd(340).
	p(NT(293), (NT(340)));
//G541: __E___E_cli_command_159_160(347) => 's'.
	p(NT(347), (T(15)));
//G542: __E___E_cli_command_159_160(347) => 's' 'u' 'b' 's' 't'.
	p(NT(347), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G543: __E___E_cli_command_159_160(347) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(347), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G544: subst_sym(346)       => __E___E_cli_command_159_160(347).
	p(NT(346), (NT(347)));
//G545: __E_cli_command_159(345) => subst_sym(346) __(34) nf_cmd_arg(348) _(11) '[' _(11) nf_cmd_arg(348) _(11) '/' _(11) nf_cmd_arg(348) _(11) ']'.
	p(NT(345), (NT(346)+NT(34)+NT(348)+NT(11)+T(26)+NT(11)+NT(348)+NT(11)+T(44)+NT(11)+NT(348)+NT(11)+T(27)));
//G546: subst_cmd(344)       => __E_cli_command_159(345).
	p(NT(344), (NT(345)));
//G547: cli_command(293)     => subst_cmd(344).
	p(NT(293), (NT(344)));
//G548: __E___E_cli_command_161_162(352) => 'i'.
	p(NT(352), (T(4)));
//G549: __E___E_cli_command_161_162(352) => 'i' 'n' 's' 't'.
	p(NT(352), (T(4)+T(5)+T(15)+T(8)));
//G550: __E___E_cli_command_161_162(352) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(352), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G551: inst_sym(351)        => __E___E_cli_command_161_162(352).
	p(NT(351), (NT(352)));
//G552: __E_cli_command_161(350) => inst_sym(351) __(34) inst_args(353).
	p(NT(350), (NT(351)+NT(34)+NT(353)));
//G553: inst_cmd(349)        => __E_cli_command_161(350).
	p(NT(349), (NT(350)));
//G554: cli_command(293)     => inst_cmd(349).
	p(NT(293), (NT(349)));
//G555: dnf_sym(356)         => 'd' 'n' 'f'.
	p(NT(356), (T(58)+T(5)+T(9)));
//G556: __E_cli_command_163(355) => dnf_sym(356) __(34) nf_cmd_arg(348).
	p(NT(355), (NT(356)+NT(34)+NT(348)));
//G557: dnf_cmd(354)         => __E_cli_command_163(355).
	p(NT(354), (NT(355)));
//G558: cli_command(293)     => dnf_cmd(354).
	p(NT(293), (NT(354)));
//G559: cnf_sym(359)         => 'c' 'n' 'f'.
	p(NT(359), (T(14)+T(5)+T(9)));
//G560: __E_cli_command_164(358) => cnf_sym(359) __(34) nf_cmd_arg(348).
	p(NT(358), (NT(359)+NT(34)+NT(348)));
//G561: cnf_cmd(357)         => __E_cli_command_164(358).
	p(NT(357), (NT(358)));
//G562: cli_command(293)     => cnf_cmd(357).
	p(NT(293), (NT(357)));
//G563: anf_sym(362)         => 'a' 'n' 'f'.
	p(NT(362), (T(16)+T(5)+T(9)));
//G564: __E_cli_command_165(361) => anf_sym(362) __(34) nf_cmd_arg(348).
	p(NT(361), (NT(362)+NT(34)+NT(348)));
//G565: anf_cmd(360)         => __E_cli_command_165(361).
	p(NT(360), (NT(361)));
//G566: cli_command(293)     => anf_cmd(360).
	p(NT(293), (NT(360)));
//G567: nnf_sym(365)         => 'n' 'n' 'f'.
	p(NT(365), (T(5)+T(5)+T(9)));
//G568: __E_cli_command_166(364) => nnf_sym(365) __(34) nf_cmd_arg(348).
	p(NT(364), (NT(365)+NT(34)+NT(348)));
//G569: nnf_cmd(363)         => __E_cli_command_166(364).
	p(NT(363), (NT(364)));
//G570: cli_command(293)     => nnf_cmd(363).
	p(NT(293), (NT(363)));
//G571: pnf_sym(368)         => 'p' 'n' 'f'.
	p(NT(368), (T(57)+T(5)+T(9)));
//G572: __E_cli_command_167(367) => pnf_sym(368) __(34) nf_cmd_arg(348).
	p(NT(367), (NT(368)+NT(34)+NT(348)));
//G573: pnf_cmd(366)         => __E_cli_command_167(367).
	p(NT(366), (NT(367)));
//G574: cli_command(293)     => pnf_cmd(366).
	p(NT(293), (NT(366)));
//G575: mnf_sym(371)         => 'm' 'n' 'f'.
	p(NT(371), (T(21)+T(5)+T(9)));
//G576: __E_cli_command_168(370) => mnf_sym(371) __(34) nf_cmd_arg(348).
	p(NT(370), (NT(371)+NT(34)+NT(348)));
//G577: mnf_cmd(369)         => __E_cli_command_168(370).
	p(NT(369), (NT(370)));
//G578: cli_command(293)     => mnf_cmd(369).
	p(NT(293), (NT(369)));
//G579: snf_sym(374)         => 's' 'n' 'f'.
	p(NT(374), (T(15)+T(5)+T(9)));
//G580: __E_cli_command_169(373) => snf_sym(374) __(34) nf_cmd_arg(348).
	p(NT(373), (NT(374)+NT(34)+NT(348)));
//G581: snf_cmd(372)         => __E_cli_command_169(373).
	p(NT(372), (NT(373)));
//G582: cli_command(293)     => snf_cmd(372).
	p(NT(293), (NT(372)));
//G583: onf_sym(377)         => 'o' 'n' 'f'.
	p(NT(377), (T(6)+T(5)+T(9)));
//G584: __E_cli_command_170(376) => onf_sym(377) __(34) variable(156) __(34) onf_cmd_arg(378).
	p(NT(376), (NT(377)+NT(34)+NT(156)+NT(34)+NT(378)));
//G585: onf_cmd(375)         => __E_cli_command_170(376).
	p(NT(375), (NT(376)));
//G586: cli_command(293)     => onf_cmd(375).
	p(NT(293), (NT(375)));
//G587: qelim_sym(381)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(381), (T(54)+T(11)+T(10)+T(4)+T(21)));
//G588: __E_cli_command_171(380) => qelim_sym(381) __(34) wff_cmd_arg(330).
	p(NT(380), (NT(381)+NT(34)+NT(330)));
//G589: qelim_cmd(379)       => __E_cli_command_171(380).
	p(NT(379), (NT(380)));
//G590: cli_command(293)     => qelim_cmd(379).
	p(NT(293), (NT(379)));
//G591: get_sym(384)         => 'g' 'e' 't'.
	p(NT(384), (T(59)+T(11)+T(8)));
//G592: __E___E_cli_command_172_173(385) => __(34) option_name(386).
	p(NT(385), (NT(34)+NT(386)));
//G593: __E___E_cli_command_172_173(385) => null.
	p(NT(385), (nul));
//G594: __E_cli_command_172(383) => get_sym(384) __E___E_cli_command_172_173(385).
	p(NT(383), (NT(384)+NT(385)));
//G595: get_cmd(382)         => __E_cli_command_172(383).
	p(NT(382), (NT(383)));
//G596: cli_command(293)     => get_cmd(382).
	p(NT(293), (NT(382)));
//G597: set_sym(389)         => 's' 'e' 't'.
	p(NT(389), (T(15)+T(11)+T(8)));
//G598: __E___E_cli_command_174_175(390) => __(34).
	p(NT(390), (NT(34)));
//G599: __E___E_cli_command_174_175(390) => _(11) '=' _(11).
	p(NT(390), (NT(11)+T(3)+NT(11)));
//G600: __E_cli_command_174(388) => set_sym(389) __(34) option_name(386) __E___E_cli_command_174_175(390) option_value(391).
	p(NT(388), (NT(389)+NT(34)+NT(386)+NT(390)+NT(391)));
//G601: set_cmd(387)         => __E_cli_command_174(388).
	p(NT(387), (NT(388)));
//G602: cli_command(293)     => set_cmd(387).
	p(NT(293), (NT(387)));
//G603: enable_sym(394)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(394), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G604: __E_cli_command_176(393) => enable_sym(394) __(34) option_name(386).
	p(NT(393), (NT(394)+NT(34)+NT(386)));
//G605: enable_cmd(392)      => __E_cli_command_176(393).
	p(NT(392), (NT(393)));
//G606: cli_command(293)     => enable_cmd(392).
	p(NT(293), (NT(392)));
//G607: disable_sym(397)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(397), (T(58)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G608: __E_cli_command_177(396) => disable_sym(397) __(34) option_name(386).
	p(NT(396), (NT(397)+NT(34)+NT(386)));
//G609: disable_cmd(395)     => __E_cli_command_177(396).
	p(NT(395), (NT(396)));
//G610: cli_command(293)     => disable_cmd(395).
	p(NT(293), (NT(395)));
//G611: toggle_sym(400)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(400), (T(8)+T(6)+T(59)+T(59)+T(10)+T(11)));
//G612: __E_cli_command_178(399) => toggle_sym(400) __(34) option_name(386).
	p(NT(399), (NT(400)+NT(34)+NT(386)));
//G613: toggle_cmd(398)      => __E_cli_command_178(399).
	p(NT(398), (NT(399)));
//G614: cli_command(293)     => toggle_cmd(398).
	p(NT(293), (NT(398)));
//G615: __E___E_cli_command_179_180(404) => 'd' 'e' 'f' 's'.
	p(NT(404), (T(58)+T(11)+T(9)+T(15)));
//G616: __E___E_cli_command_179_180(404) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(404), (T(58)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G617: def_sym(403)         => __E___E_cli_command_179_180(404).
	p(NT(403), (NT(404)));
//G618: __E_cli_command_179(402) => def_sym(403).
	p(NT(402), (NT(403)));
//G619: def_list_cmd(401)    => __E_cli_command_179(402).
	p(NT(401), (NT(402)));
//G620: cli_command(293)     => def_list_cmd(401).
	p(NT(293), (NT(401)));
//G621: __E_cli_command_181(406) => def_sym(403) __(34) num(237).
	p(NT(406), (NT(403)+NT(34)+NT(237)));
//G622: def_print_cmd(405)   => __E_cli_command_181(406).
	p(NT(405), (NT(406)));
//G623: cli_command(293)     => def_print_cmd(405).
	p(NT(293), (NT(405)));
//G624: def_rr_cmd(407)      => rec_relation(17).
	p(NT(407), (NT(17)));
//G625: cli_command(293)     => def_rr_cmd(407).
	p(NT(293), (NT(407)));
//G626: def_input_cmd(408)   => input_def(18).
	p(NT(408), (NT(18)));
//G627: cli_command(293)     => def_input_cmd(408).
	p(NT(293), (NT(408)));
//G628: def_output_cmd(409)  => output_def(19).
	p(NT(409), (NT(19)));
//G629: cli_command(293)     => def_output_cmd(409).
	p(NT(293), (NT(409)));
//G630: __E___E_cli_command_182_183(413) => 'h' 'i' 's' 't'.
	p(NT(413), (T(56)+T(4)+T(15)+T(8)));
//G631: __E___E_cli_command_182_183(413) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(413), (T(56)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G632: history_sym(412)     => __E___E_cli_command_182_183(413).
	p(NT(412), (NT(413)));
//G633: __E_cli_command_182(411) => history_sym(412).
	p(NT(411), (NT(412)));
//G634: history_list_cmd(410) => __E_cli_command_182(411).
	p(NT(410), (NT(411)));
//G635: cli_command(293)     => history_list_cmd(410).
	p(NT(293), (NT(410)));
//G636: __E_cli_command_184(415) => history_sym(412) __(34) history(339).
	p(NT(415), (NT(412)+NT(34)+NT(339)));
//G637: history_print_cmd(414) => __E_cli_command_184(415).
	p(NT(414), (NT(415)));
//G638: cli_command(293)     => history_print_cmd(414).
	p(NT(293), (NT(414)));
//G639: __E_cli_command_185(417) => wff(14).
	p(NT(417), (NT(14)));
//G640: __E_cli_command_185(417) => bf(30).
	p(NT(417), (NT(30)));
//G641: history_store_cmd(416) => __E_cli_command_185(417).
	p(NT(416), (NT(417)));
//G642: cli_command(293)     => history_store_cmd(416).
	p(NT(293), (NT(416)));
//G643: __E_solve_options_186(418) => __(34) solve_option(419).
	p(NT(418), (NT(34)+NT(419)));
//G644: __E_solve_options_187(420) => null.
	p(NT(420), (nul));
//G645: __E_solve_options_187(420) => __E_solve_options_186(418) __E_solve_options_187(420).
	p(NT(420), (NT(418)+NT(420)));
//G646: solve_options(329)   => __E_solve_options_187(420).
	p(NT(329), (NT(420)));
//G647: __E_solve_option_188(421) => solver_mode(422).
	p(NT(421), (NT(422)));
//G648: __E_solve_option_188(421) => type(225).
	p(NT(421), (NT(225)));
//G649: solve_option(419)    => '-' '-' __E_solve_option_188(421).
	p(NT(419), (T(30)+T(30)+NT(421)));
//G650: __E_solver_mode_189(424) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(424), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G651: __E_solver_mode_189(424) => 'm' 'i' 'n'.
	p(NT(424), (T(21)+T(4)+T(5)));
//G652: solver_mode_minimum(423) => __E_solver_mode_189(424).
	p(NT(423), (NT(424)));
//G653: solver_mode(422)     => solver_mode_minimum(423).
	p(NT(422), (NT(423)));
//G654: __E_solver_mode_190(426) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(426), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G655: __E_solver_mode_190(426) => 'm' 'a' 'x'.
	p(NT(426), (T(21)+T(16)+T(29)));
//G656: solver_mode_maximum(425) => __E_solver_mode_190(426).
	p(NT(425), (NT(426)));
//G657: solver_mode(422)     => solver_mode_maximum(425).
	p(NT(422), (NT(425)));
//G658: q_file_name(38)      => '"' file_name(427) '"'.
	p(NT(38), (T(61)+NT(427)+T(61)));
//G659: __E_file_name_191(428) => printable(8).
	p(NT(428), (NT(8)));
//G660: __E_file_name_191(428) => printable(8) __E_file_name_191(428).
	p(NT(428), (NT(8)+NT(428)));
//G661: file_name(427)       => __E_file_name_191(428).
	p(NT(427), (NT(428)));
//G662: __E_option_name_192(429) => alnum(6).
	p(NT(429), (NT(6)));
//G663: __E_option_name_192(429) => alnum(6) __E_option_name_192(429).
	p(NT(429), (NT(6)+NT(429)));
//G664: option_name(386)     => __E_option_name_192(429).
	p(NT(386), (NT(429)));
//G665: __E_option_value_193(430) => alnum(6).
	p(NT(430), (NT(6)));
//G666: __E_option_value_193(430) => alnum(6) __E_option_value_193(430).
	p(NT(430), (NT(6)+NT(430)));
//G667: option_value(391)    => __E_option_value_193(430).
	p(NT(391), (NT(430)));
//G668: bf_cmd_arg(431)      => history(339).
	p(NT(431), (NT(339)));
//G669: bf_cmd_arg(431)      => bf(30).
	p(NT(431), (NT(30)));
//G670: wff_cmd_arg(330)     => history(339).
	p(NT(330), (NT(339)));
//G671: wff_cmd_arg(330)     => wff(14).
	p(NT(330), (NT(14)));
//G672: nf_cmd_arg(348)      => history(339).
	p(NT(348), (NT(339)));
//G673: nf_cmd_arg(348)      => ref(27).
	p(NT(348), (NT(27)));
//G674: nf_cmd_arg(348)      => wff(14).
	p(NT(348), (NT(14)));
//G675: nf_cmd_arg(348)      => bf(30).
	p(NT(348), (NT(30)));
//G676: onf_cmd_arg(378)     => history(339).
	p(NT(378), (NT(339)));
//G677: onf_cmd_arg(378)     => wff(14).
	p(NT(378), (NT(14)));
//G678: normalize_cmd_arg(318) => history(339).
	p(NT(318), (NT(339)));
//G679: normalize_cmd_arg(318) => spec(10).
	p(NT(318), (NT(10)));
//G680: normalize_cmd_arg(318) => ref(27).
	p(NT(318), (NT(27)));
//G681: normalize_cmd_arg(318) => wff(14).
	p(NT(318), (NT(14)));
//G682: normalize_cmd_arg(318) => bf(30).
	p(NT(318), (NT(30)));
//G683: inst_args(353)       => wff_cmd_arg(330) _(11) '[' _(11) variable(156) _(11) '/' _(11) bf_cmd_arg(431) _(11) ']'.
	p(NT(353), (NT(330)+NT(11)+T(26)+NT(11)+NT(156)+NT(11)+T(44)+NT(11)+NT(431)+NT(11)+T(27)));
//G684: inst_args(353)       => bf_cmd_arg(431) _(11) '[' _(11) variable(156) _(11) '/' _(11) bf_cmd_arg(431) _(11) ']'.
	p(NT(353), (NT(431)+NT(11)+T(26)+NT(11)+NT(156)+NT(11)+T(44)+NT(11)+NT(431)+NT(11)+T(27)));
//G685: help_arg(310)        => help_sym(307).
	p(NT(310), (NT(307)));
//G686: help_arg(310)        => version_sym(300).
	p(NT(310), (NT(300)));
//G687: help_arg(310)        => quit_sym(297).
	p(NT(310), (NT(297)));
//G688: help_arg(310)        => clear_sym(303).
	p(NT(310), (NT(303)));
//G689: help_arg(310)        => get_sym(384).
	p(NT(310), (NT(384)));
//G690: help_arg(310)        => set_sym(389).
	p(NT(310), (NT(389)));
//G691: help_arg(310)        => enable_sym(394).
	p(NT(310), (NT(394)));
//G692: help_arg(310)        => disable_sym(397).
	p(NT(310), (NT(397)));
//G693: help_arg(310)        => toggle_sym(400).
	p(NT(310), (NT(400)));
//G694: help_arg(310)        => file_sym(313).
	p(NT(310), (NT(313)));
//G695: help_arg(310)        => history_sym(412).
	p(NT(310), (NT(412)));
//G696: help_arg(310)        => abs_history_sym(432).
	p(NT(310), (NT(432)));
//G697: help_arg(310)        => rel_history_sym(433).
	p(NT(310), (NT(433)));
//G698: help_arg(310)        => selection_sym(434).
	p(NT(310), (NT(434)));
//G699: help_arg(310)        => def_sym(403).
	p(NT(310), (NT(403)));
//G700: help_arg(310)        => inst_sym(351).
	p(NT(310), (NT(351)));
//G701: help_arg(310)        => subst_sym(346).
	p(NT(310), (NT(346)));
//G702: help_arg(310)        => normalize_sym(342).
	p(NT(310), (NT(342)));
//G703: help_arg(310)        => execute_sym(435).
	p(NT(310), (NT(435)));
//G704: help_arg(310)        => solve_sym(327).
	p(NT(310), (NT(327)));
//G705: help_arg(310)        => valid_sym(317).
	p(NT(310), (NT(317)));
//G706: help_arg(310)        => sat_sym(321).
	p(NT(310), (NT(321)));
//G707: help_arg(310)        => unsat_sym(324).
	p(NT(310), (NT(324)));
//G708: help_arg(310)        => run_sym(336).
	p(NT(310), (NT(336)));
//G709: help_arg(310)        => dnf_sym(356).
	p(NT(310), (NT(356)));
//G710: help_arg(310)        => cnf_sym(359).
	p(NT(310), (NT(359)));
//G711: help_arg(310)        => anf_sym(362).
	p(NT(310), (NT(362)));
//G712: help_arg(310)        => snf_sym(374).
	p(NT(310), (NT(374)));
//G713: help_arg(310)        => nnf_sym(365).
	p(NT(310), (NT(365)));
//G714: help_arg(310)        => pnf_sym(368).
	p(NT(310), (NT(368)));
//G715: help_arg(310)        => mnf_sym(371).
	p(NT(310), (NT(371)));
//G716: help_arg(310)        => onf_sym(377).
	p(NT(310), (NT(377)));
//G717: help_arg(310)        => qelim_sym(381).
	p(NT(310), (NT(381)));
//G718: __E___E_help_arg_194_195(438) => 's'.
	p(NT(438), (T(15)));
//G719: __E___E_help_arg_194_195(438) => null.
	p(NT(438), (nul));
//G720: __E_help_arg_194(437) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_194_195(438).
	p(NT(437), (T(11)+T(29)+T(16)+T(21)+T(57)+T(10)+T(11)+NT(438)));
//G721: examples_sym(436)    => __E_help_arg_194(437).
	p(NT(436), (NT(437)));
//G722: help_arg(310)        => examples_sym(436).
	p(NT(310), (NT(436)));
//G723: __E_history_196(440) => '%'.
	p(NT(440), (T(45)));
//G724: rel_history_sym(433) => '%' '-'.
	p(NT(433), (T(45)+T(30)));
//G725: history_id(442)      => digits(284).
	p(NT(442), (NT(284)));
//G726: __E___E_history_196_197(441) => history_id(442).
	p(NT(441), (NT(442)));
//G727: __E___E_history_196_197(441) => null.
	p(NT(441), (nul));
//G728: __E_history_196(440) => rel_history_sym(433) __E___E_history_196_197(441).
	p(NT(440), (NT(433)+NT(441)));
//G729: rel_history(439)     => __E_history_196(440).
	p(NT(439), (NT(440)));
//G730: history(339)         => rel_history(439).
	p(NT(339), (NT(439)));
//G731: abs_history_sym(432) => '%'.
	p(NT(432), (T(45)));
//G732: __E_history_198(444) => abs_history_sym(432) history_id(442).
	p(NT(444), (NT(432)+NT(442)));
//G733: abs_history(443)     => __E_history_198(444).
	p(NT(443), (NT(444)));
//G734: history(339)         => abs_history(443).
	p(NT(339), (NT(443)));
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
		ref_arg, __E___E_ref_args_13_15, __E___E_ref_args_13_16, library, rules, __E_library_17, __E___E_library_17_18, rule, __E___E___E_library_17_18_19, wff_rule, 
		bf_rule, __E___E_library_17_20, wff_matcher, wff_body, bf_matcher, bf_body, builder, builder_head, builder_body, __E_builder_head_21, 
		__E_builder_head_22, bf_builder_body, __E_builder_body_23, wff_builder_body, __E_builder_body_24, tau_constant_source, __E_tau_constant_source_25, wff_parenthesis, __E_wff_26, wff_sometimes, 
		__E_wff_27, __E___E_wff_27_28, wff_always, __E_wff_29, __E___E_wff_29_30, wff_conditional, __E_wff_31, wff_all, __E_wff_32, q_vars, 
		wff_ex, __E_wff_33, wff_ref, constraint, wff_imply, __E_wff_34, wff_rimply, __E_wff_35, wff_equiv, __E_wff_36, 
		wff_or, __E_wff_37, wff_xor, __E_wff_38, wff_and, __E_wff_39, wff_neg, __E_wff_40, wff_t, wff_f, 
		bf_interval, __E_wff_41, bf_eq, __E_wff_42, bf_neq, __E_wff_43, bf_lteq, __E_wff_44, bf_nlteq, __E_wff_45, 
		bf_gt, __E_wff_46, bf_ngt, __E_wff_47, bf_gteq, __E_wff_48, bf_ngteq, __E_wff_49, bf_lt, __E_wff_50, 
		bf_nlt, __E_wff_51, bv_eq, __E_wff_52, bv, bv_neq, __E_wff_53, bv_lteq, __E_wff_54, bv_nlteq, 
		__E_wff_55, bv_gt, __E_wff_56, bv_ngt, __E_wff_57, bv_gteq, __E_wff_58, bv_ngteq, __E_wff_59, bv_lt, 
		__E_wff_60, bv_nlt, __E_wff_61, bf_parenthesis, __E_bf_62, bf_constant, variable, bf_splitter, __E_bf_63, bf_fall, 
		__E_bf_64, bf_fex, __E_bf_65, bf_ref, bf_or, __E_bf_66, bf_xor, __E_bf_67, bf_t, __E_bf_68, 
		__E___E_bf_68_69, bf_f, __E_bf_70, __E___E_bf_70_71, bf_and, __E_bf_72, __E___E_bf_72_73, bf_neg, __E_bf_74, bf_neg_oprnd, 
		__E___E_bf_74_75, bf_and_nosep, __E_bf_76, bf_and_nosep_1st_oprnd, __E___E_bf_76_77, bf_and_nosep_2nd_oprnd, __E___E_bf_76_78, bv_parenthesis, __E_bv_79, bv_checked, 
		__E_bv_80, bv_add, __E_bv_81, bv_mul, __E_bv_82, bv_div, __E_bv_83, bv_mod, __E_bv_84, bv_sub, 
		__E_bv_85, bv_neg, bv_and, __E_bv_86, bv_nand, __E_bv_87, bv_or, __E_bv_88, bv_nor, __E_bv_89, 
		bv_xor, __E_bv_90, bv_xnor, __E_bv_91, bv_rotate_left, __E_bv_92, bv_rotate_right, __E_bv_93, bv_min, __E_bv_94, 
		bv_max, __E_bv_95, bv_constant, binary, __E_bv_constant_96, type, decimal, __E_bv_constant_97, hexadecimal, __E_bv_constant_98, 
		__E_binary_99, __E_binary_100, __E_hexadecimal_101, __E_decimal_102, ctn_neq, __E_constraint_103, ctnvar, num, ctn_eq, __E_constraint_104, 
		ctn_gteq, __E_constraint_105, ctn_gt, __E_constraint_106, ctn_lteq, __E_constraint_107, ctn_lt, __E_constraint_108, __E_bf_constant_109, source, 
		__E_bf_constant_110, src_c, __E_source_111, __E___E_source_111_112, __E___E_source_111_113, __E_src_c_114, __E_src_c_115, __E_variable_116, uconst, io_var, 
		var_name, __E_variable_117, __E_var_name_118, __E_var_name_119, __E_var_name_120, __E_var_name_121, __E_var_name_122, offset, chars, uconst_name, 
		__E_uconst_123, __E___E_uconst_123_124, q_var, __E_q_vars_125, __E_q_vars_126, __E_offsets_127, __E_offsets_128, integer, shift, __E_offset_129, 
		__E_shift_130, __E___E_shift_130_131, __E_chars_132, __E_chars_133, digits, __E_digits_134, __E_integer_135, scope_id, comment, __E_comment_136, 
		__E_comment_137, __E_comment_138, cli, cli_command, __E_cli_139, __E_cli_140, quit_cmd, quit_sym, __E_cli_command_141, version_cmd, 
		version_sym, __E_cli_command_142, clear_cmd, clear_sym, __E_cli_command_143, help_cmd, __E_cli_command_144, help_sym, __E___E_cli_command_144_145, __E___E_cli_command_144_146, 
		help_arg, file_cmd, __E_cli_command_147, file_sym, q_string, valid_cmd, __E_cli_command_148, valid_sym, normalize_cmd_arg, sat_cmd, 
		__E_cli_command_149, sat_sym, unsat_cmd, __E_cli_command_150, unsat_sym, solve_cmd, __E_cli_command_151, solve_sym, __E___E_cli_command_151_152, solve_options, 
		wff_cmd_arg, lgrs_cmd, __E_cli_command_153, lgrs_sym, run_cmd, __E_cli_command_154, run_sym, __E___E_cli_command_154_155, __E___E_cli_command_154_156, history, 
		normalize_cmd, __E_cli_command_157, normalize_sym, __E___E_cli_command_157_158, subst_cmd, __E_cli_command_159, subst_sym, __E___E_cli_command_159_160, nf_cmd_arg, inst_cmd, 
		__E_cli_command_161, inst_sym, __E___E_cli_command_161_162, inst_args, dnf_cmd, __E_cli_command_163, dnf_sym, cnf_cmd, __E_cli_command_164, cnf_sym, 
		anf_cmd, __E_cli_command_165, anf_sym, nnf_cmd, __E_cli_command_166, nnf_sym, pnf_cmd, __E_cli_command_167, pnf_sym, mnf_cmd, 
		__E_cli_command_168, mnf_sym, snf_cmd, __E_cli_command_169, snf_sym, onf_cmd, __E_cli_command_170, onf_sym, onf_cmd_arg, qelim_cmd, 
		__E_cli_command_171, qelim_sym, get_cmd, __E_cli_command_172, get_sym, __E___E_cli_command_172_173, option_name, set_cmd, __E_cli_command_174, set_sym, 
		__E___E_cli_command_174_175, option_value, enable_cmd, __E_cli_command_176, enable_sym, disable_cmd, __E_cli_command_177, disable_sym, toggle_cmd, __E_cli_command_178, 
		toggle_sym, def_list_cmd, __E_cli_command_179, def_sym, __E___E_cli_command_179_180, def_print_cmd, __E_cli_command_181, def_rr_cmd, def_input_cmd, def_output_cmd, 
		history_list_cmd, __E_cli_command_182, history_sym, __E___E_cli_command_182_183, history_print_cmd, __E_cli_command_184, history_store_cmd, __E_cli_command_185, __E_solve_options_186, solve_option, 
		__E_solve_options_187, __E_solve_option_188, solver_mode, solver_mode_minimum, __E_solver_mode_189, solver_mode_maximum, __E_solver_mode_190, file_name, __E_file_name_191, __E_option_name_192, 
		__E_option_value_193, bf_cmd_arg, abs_history_sym, rel_history_sym, selection_sym, execute_sym, examples_sym, __E_help_arg_194, __E___E_help_arg_194_195, rel_history, 
		__E_history_196, __E___E_history_196_197, history_id, abs_history, __E_history_198, __N_0, __N_1, __N_2, __N_3, __N_4, 
		__N_5, __N_6, __N_7, __N_8, 
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
