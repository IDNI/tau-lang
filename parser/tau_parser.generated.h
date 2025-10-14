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
	"ref_arg", "__E___E_ref_args_13_15", "__E___E_ref_args_13_16", "tau_constant_source", "__E_tau_constant_source_17", "wff_parenthesis", "__E_wff_18", "wff_sometimes", "__E_wff_19", "__E___E_wff_19_20", 
	"wff_always", "__E_wff_21", "__E___E_wff_21_22", "wff_conditional", "__E_wff_23", "wff_all", "__E_wff_24", "q_vars", "wff_ex", "__E_wff_25", 
	"wff_ref", "constraint", "wff_imply", "__E_wff_26", "wff_rimply", "__E_wff_27", "wff_equiv", "__E_wff_28", "wff_or", "__E_wff_29", 
	"wff_xor", "__E_wff_30", "wff_and", "__E_wff_31", "wff_neg", "__E_wff_32", "wff_t", "wff_f", "bf_interval", "__E_wff_33", 
	"bf_eq", "__E_wff_34", "bf_neq", "__E_wff_35", "bf_lteq", "__E_wff_36", "bf_nlteq", "__E_wff_37", "bf_gt", "__E_wff_38", 
	"bf_ngt", "__E_wff_39", "bf_gteq", "__E_wff_40", "bf_ngteq", "__E_wff_41", "bf_lt", "__E_wff_42", "bf_nlt", "__E_wff_43", 
	"bv_eq", "__E_wff_44", "bv", "bv_neq", "__E_wff_45", "bv_lteq", "__E_wff_46", "bv_nlteq", "__E_wff_47", "bv_gt", 
	"__E_wff_48", "bv_ngt", "__E_wff_49", "bv_gteq", "__E_wff_50", "bv_ngteq", "__E_wff_51", "bv_lt", "__E_wff_52", "bv_nlt", 
	"__E_wff_53", "bf_parenthesis", "__E_bf_54", "ba_constant", "variable", "bf_fall", "__E_bf_55", "bf_fex", "__E_bf_56", "bf_ref", 
	"bf_or", "__E_bf_57", "bf_xor", "__E_bf_58", "bf_t", "__E_bf_59", "__E___E_bf_59_60", "bf_f", "__E_bf_61", "__E___E_bf_61_62", 
	"bf_and", "__E_bf_63", "__E___E_bf_63_64", "bf_neg", "__E_bf_65", "bf_neg_oprnd", "__E___E_bf_65_66", "bf_and_nosep", "__E_bf_67", "bf_and_nosep_1st_oprnd", 
	"__E___E_bf_67_68", "bf_and_nosep_2nd_oprnd", "__E___E_bf_67_69", "bv_parenthesis", "__E_bv_70", "bv_checked", "__E_bv_71", "bv_add", "__E_bv_72", "bv_mul", 
	"__E_bv_73", "bv_div", "__E_bv_74", "bv_mod", "__E_bv_75", "bv_sub", "__E_bv_76", "bv_neg", "bv_and", "__E_bv_77", 
	"bv_nand", "__E_bv_78", "bv_or", "__E_bv_79", "bv_nor", "__E_bv_80", "bv_xor", "__E_bv_81", "bv_xnor", "__E_bv_82", 
	"bv_shl", "__E_bv_83", "bv_shr", "__E_bv_84", "bv_min", "__E_bv_85", "bv_max", "__E_bv_86", "bv_constant", "binary", 
	"__E_bv_constant_87", "decimal", "__E_bv_constant_88", "hexadecimal", "__E_bv_constant_89", "__E_binary_90", "__E_binary_91", "__E_hexadecimal_92", "__E_decimal_93", "ctn_neq", 
	"__E_constraint_94", "ctnvar", "num", "ctn_eq", "__E_constraint_95", "ctn_gteq", "__E_constraint_96", "ctn_gt", "__E_constraint_97", "ctn_lteq", 
	"__E_constraint_98", "ctn_lt", "__E_constraint_99", "__E_ba_constant_100", "source", "__E_ba_constant_101", "src_c", "__E_source_102", "__E___E_source_102_103", "__E___E_source_102_104", 
	"__E_src_c_105", "__E_src_c_106", "__E_variable_107", "uconst", "io_var", "var_name", "__E_variable_108", "__E_var_name_109", "__E_var_name_110", "__E_var_name_111", 
	"__E_var_name_112", "__E_var_name_113", "offset", "chars", "uconst_name", "__E_uconst_114", "__E___E_uconst_114_115", "q_var", "__E_q_vars_116", "__E_q_vars_117", 
	"__E_offsets_118", "__E_offsets_119", "integer", "shift", "__E_offset_120", "__E_shift_121", "__E___E_shift_121_122", "__E_chars_123", "__E_chars_124", "digits", 
	"__E_digits_125", "__E_integer_126", "type", "__E_typed_127", "subtype", "comment", "__E_comment_128", "__E_comment_129", "__E_comment_130", "cli", 
	"cli_command", "__E_cli_131", "__E_cli_132", "quit_cmd", "quit_sym", "__E_cli_command_133", "version_cmd", "version_sym", "__E_cli_command_134", "clear_cmd", 
	"clear_sym", "__E_cli_command_135", "help_cmd", "__E_cli_command_136", "help_sym", "__E___E_cli_command_136_137", "__E___E_cli_command_136_138", "help_arg", "file_cmd", "__E_cli_command_139", 
	"file_sym", "q_string", "valid_cmd", "__E_cli_command_140", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_141", "sat_sym", "unsat_cmd", 
	"__E_cli_command_142", "unsat_sym", "solve_cmd", "__E_cli_command_143", "solve_sym", "__E___E_cli_command_143_144", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_145", 
	"lgrs_sym", "run_cmd", "__E_cli_command_146", "run_sym", "__E___E_cli_command_146_147", "__E___E_cli_command_146_148", "history", "normalize_cmd", "__E_cli_command_149", "normalize_sym", 
	"__E___E_cli_command_149_150", "subst_cmd", "__E_cli_command_151", "subst_sym", "__E___E_cli_command_151_152", "nf_cmd_arg", "inst_cmd", "__E_cli_command_153", "inst_sym", "__E___E_cli_command_153_154", 
	"inst_args", "dnf_cmd", "__E_cli_command_155", "dnf_sym", "cnf_cmd", "__E_cli_command_156", "cnf_sym", "anf_cmd", "__E_cli_command_157", "anf_sym", 
	"nnf_cmd", "__E_cli_command_158", "nnf_sym", "pnf_cmd", "__E_cli_command_159", "pnf_sym", "mnf_cmd", "__E_cli_command_160", "mnf_sym", "onf_cmd", 
	"__E_cli_command_161", "onf_sym", "onf_cmd_arg", "qelim_cmd", "__E_cli_command_162", "qelim_sym", "get_cmd", "__E_cli_command_163", "get_sym", "__E___E_cli_command_163_164", 
	"option_name", "set_cmd", "__E_cli_command_165", "set_sym", "__E___E_cli_command_165_166", "option_value", "enable_cmd", "__E_cli_command_167", "enable_sym", "disable_cmd", 
	"__E_cli_command_168", "disable_sym", "toggle_cmd", "__E_cli_command_169", "toggle_sym", "def_list_cmd", "__E_cli_command_170", "def_sym", "__E___E_cli_command_170_171", "def_print_cmd", 
	"__E_cli_command_172", "def_rr_cmd", "def_input_cmd", "def_output_cmd", "history_list_cmd", "__E_cli_command_173", "history_sym", "__E___E_cli_command_173_174", "history_print_cmd", "__E_cli_command_175", 
	"history_store_cmd", "__E_cli_command_176", "__E_solve_options_177", "solve_option", "__E_solve_options_178", "__E_solve_option_179", "solver_mode", "solver_mode_minimum", "__E_solver_mode_180", "solver_mode_maximum", 
	"__E_solver_mode_181", "file_name", "__E_file_name_182", "__E_option_name_183", "__E_option_value_184", "bf_cmd_arg", "abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", 
	"snf_sym", "examples_sym", "__E_help_arg_185", "__E___E_help_arg_185_186", "rel_history", "__E_history_187", "__E___E_history_187_188", "history_id", "abs_history", "__E_history_189", 
	"__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
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
			29, 31, 40, 199, 201, 203, 211, 224, 235, 244,
			252, 259, 262, 264, 360, 365, 401
		},
		.to_inline = {
			{ 14, 55, 14 },
			{ 30, 131, 30 },
			{ 33 },
			{ 112, 163, 112 },
			{ 155, 131, 30 },
			{ 157, 131, 150 },
			{ 159, 131, 30 },
			{ 161, 131, 30 },
			{ 226 },
			{ 233 },
			{ 233, 244 },
			{ 243 },
			{ 247 },
			{ 295 },
			{ 307 },
			{ 325 },
			{ 330 },
			{ 352 },
			{ 405 }
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
//G59:  __E_tau_constant_source_17(54) => '.' _(11).
	p(NT(54), (T(1)+NT(11)));
//G60:  __E_tau_constant_source_17(54) => null.
	p(NT(54), (nul));
//G61:  tau_constant_source(53) => rec_relations(24) _(11) main(13) _(11) __E_tau_constant_source_17(54).
	p(NT(53), (NT(24)+NT(11)+NT(13)+NT(11)+NT(54)));
//G62:  __E_wff_18(56)       => '(' _(11) wff(14) _(11) ')'.
	p(NT(56), (T(12)+NT(11)+NT(14)+NT(11)+T(13)));
//G63:  wff_parenthesis(55)  => __E_wff_18(56).
	p(NT(55), (NT(56)));
//G64:  wff(14)              => wff_parenthesis(55).
	p(NT(14), (NT(55)));
//G65:  __E___E_wff_19_20(59) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(59), (T(15)+T(6)+T(21)+T(11)+T(8)+T(4)+T(21)+T(11)+T(15)));
//G66:  __E___E_wff_19_20(59) => '<' '>'.
	p(NT(59), (T(22)+T(23)));
//G67:  __E_wff_19(58)       => __E___E_wff_19_20(59) _(11) wff(14).
	p(NT(58), (NT(59)+NT(11)+NT(14)));
//G68:  wff_sometimes(57)    => __E_wff_19(58).
	p(NT(57), (NT(58)));
//G69:  wff(14)              => wff_sometimes(57).
	p(NT(14), (NT(57)));
//G70:  __E___E_wff_21_22(62) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(62), (T(16)+T(10)+T(24)+T(16)+T(25)+T(15)));
//G71:  __E___E_wff_21_22(62) => '[' ']'.
	p(NT(62), (T(26)+T(27)));
//G72:  __E_wff_21(61)       => __E___E_wff_21_22(62) _(11) wff(14).
	p(NT(61), (NT(62)+NT(11)+NT(14)));
//G73:  wff_always(60)       => __E_wff_21(61).
	p(NT(60), (NT(61)));
//G74:  wff(14)              => wff_always(60).
	p(NT(14), (NT(60)));
//G75:  __E_wff_23(64)       => wff(14) _(11) '?' _(11) wff(14) _(11) ':' _(11) wff(14).
	p(NT(64), (NT(14)+NT(11)+T(28)+NT(11)+NT(14)+NT(11)+T(2)+NT(11)+NT(14)));
//G76:  wff_conditional(63)  => __E_wff_23(64).
	p(NT(63), (NT(64)));
//G77:  wff(14)              => wff_conditional(63).
	p(NT(14), (NT(63)));
//G78:  __E_wff_24(66)       => 'a' 'l' 'l' __(34) q_vars(67) __(34) wff(14).
	p(NT(66), (T(16)+T(10)+T(10)+NT(34)+NT(67)+NT(34)+NT(14)));
//G79:  wff_all(65)          => __E_wff_24(66).
	p(NT(65), (NT(66)));
//G80:  wff(14)              => wff_all(65).
	p(NT(14), (NT(65)));
//G81:  __E_wff_25(69)       => 'e' 'x' __(34) q_vars(67) __(34) wff(14).
	p(NT(69), (T(11)+T(29)+NT(34)+NT(67)+NT(34)+NT(14)));
//G82:  wff_ex(68)           => __E_wff_25(69).
	p(NT(68), (NT(69)));
//G83:  wff(14)              => wff_ex(68).
	p(NT(14), (NT(68)));
//G84:  wff_ref(70)          => ref(27).
	p(NT(70), (NT(27)));
//G85:  wff(14)              => wff_ref(70).
	p(NT(14), (NT(70)));
//G86:  wff(14)              => constraint(71).
	p(NT(14), (NT(71)));
//G87:  __E_wff_26(73)       => wff(14) _(11) '-' '>' _(11) wff(14).
	p(NT(73), (NT(14)+NT(11)+T(30)+T(23)+NT(11)+NT(14)));
//G88:  wff_imply(72)        => __E_wff_26(73).
	p(NT(72), (NT(73)));
//G89:  wff(14)              => wff_imply(72).
	p(NT(14), (NT(72)));
//G90:  __E_wff_27(75)       => wff(14) _(11) '<' '-' _(11) wff(14).
	p(NT(75), (NT(14)+NT(11)+T(22)+T(30)+NT(11)+NT(14)));
//G91:  wff_rimply(74)       => __E_wff_27(75).
	p(NT(74), (NT(75)));
//G92:  wff(14)              => wff_rimply(74).
	p(NT(14), (NT(74)));
//G93:  __E_wff_28(77)       => wff(14) _(11) '<' '-' '>' _(11) wff(14).
	p(NT(77), (NT(14)+NT(11)+T(22)+T(30)+T(23)+NT(11)+NT(14)));
//G94:  wff_equiv(76)        => __E_wff_28(77).
	p(NT(76), (NT(77)));
//G95:  wff(14)              => wff_equiv(76).
	p(NT(14), (NT(76)));
//G96:  __E_wff_29(79)       => wff(14) _(11) '|' '|' _(11) wff(14).
	p(NT(79), (NT(14)+NT(11)+T(31)+T(31)+NT(11)+NT(14)));
//G97:  wff_or(78)           => __E_wff_29(79).
	p(NT(78), (NT(79)));
//G98:  wff(14)              => wff_or(78).
	p(NT(14), (NT(78)));
//G99:  __E_wff_30(81)       => wff(14) _(11) '^' '^' _(11) wff(14).
	p(NT(81), (NT(14)+NT(11)+T(32)+T(32)+NT(11)+NT(14)));
//G100: wff_xor(80)          => __E_wff_30(81).
	p(NT(80), (NT(81)));
//G101: wff(14)              => wff_xor(80).
	p(NT(14), (NT(80)));
//G102: __E_wff_31(83)       => wff(14) _(11) '&' '&' _(11) wff(14).
	p(NT(83), (NT(14)+NT(11)+T(33)+T(33)+NT(11)+NT(14)));
//G103: wff_and(82)          => __E_wff_31(83).
	p(NT(82), (NT(83)));
//G104: wff(14)              => wff_and(82).
	p(NT(14), (NT(82)));
//G105: __E_wff_32(85)       => '!' _(11) wff(14).
	p(NT(85), (T(34)+NT(11)+NT(14)));
//G106: wff_neg(84)          => __E_wff_32(85).
	p(NT(84), (NT(85)));
//G107: wff(14)              => wff_neg(84).
	p(NT(14), (NT(84)));
//G108: wff_t(86)            => 'T'.
	p(NT(86), (T(35)));
//G109: wff(14)              => wff_t(86).
	p(NT(14), (NT(86)));
//G110: wff_f(87)            => 'F'.
	p(NT(87), (T(36)));
//G111: wff(14)              => wff_f(87).
	p(NT(14), (NT(87)));
//G112: __E_wff_33(89)       => bf(30) _(11) '<' '=' _(11) bf(30) _(11) '<' '=' _(11) bf(30).
	p(NT(89), (NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)));
//G113: bf_interval(88)      => __E_wff_33(89).
	p(NT(88), (NT(89)));
//G114: wff(14)              => bf_interval(88).
	p(NT(14), (NT(88)));
//G115: __E_wff_34(91)       => bf(30) _(11) '=' _(11) bf(30).
	p(NT(91), (NT(30)+NT(11)+T(3)+NT(11)+NT(30)));
//G116: bf_eq(90)            => __E_wff_34(91).
	p(NT(90), (NT(91)));
//G117: wff(14)              => bf_eq(90).
	p(NT(14), (NT(90)));
//G118: __E_wff_35(93)       => bf(30) _(11) '!' '=' _(11) bf(30).
	p(NT(93), (NT(30)+NT(11)+T(34)+T(3)+NT(11)+NT(30)));
//G119: bf_neq(92)           => __E_wff_35(93).
	p(NT(92), (NT(93)));
//G120: wff(14)              => bf_neq(92).
	p(NT(14), (NT(92)));
//G121: __E_wff_36(95)       => bf(30) _(11) '<' '=' _(11) bf(30).
	p(NT(95), (NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)));
//G122: bf_lteq(94)          => __E_wff_36(95).
	p(NT(94), (NT(95)));
//G123: wff(14)              => bf_lteq(94).
	p(NT(14), (NT(94)));
//G124: __E_wff_37(97)       => bf(30) _(11) '!' '<' '=' _(11) bf(30).
	p(NT(97), (NT(30)+NT(11)+T(34)+T(22)+T(3)+NT(11)+NT(30)));
//G125: bf_nlteq(96)         => __E_wff_37(97).
	p(NT(96), (NT(97)));
//G126: wff(14)              => bf_nlteq(96).
	p(NT(14), (NT(96)));
//G127: __E_wff_38(99)       => bf(30) _(11) '>' _(11) bf(30).
	p(NT(99), (NT(30)+NT(11)+T(23)+NT(11)+NT(30)));
//G128: bf_gt(98)            => __E_wff_38(99).
	p(NT(98), (NT(99)));
//G129: wff(14)              => bf_gt(98).
	p(NT(14), (NT(98)));
//G130: __E_wff_39(101)      => bf(30) _(11) '!' '>' _(11) bf(30).
	p(NT(101), (NT(30)+NT(11)+T(34)+T(23)+NT(11)+NT(30)));
//G131: bf_ngt(100)          => __E_wff_39(101).
	p(NT(100), (NT(101)));
//G132: wff(14)              => bf_ngt(100).
	p(NT(14), (NT(100)));
//G133: __E_wff_40(103)      => bf(30) _(11) '>' '=' _(11) bf(30).
	p(NT(103), (NT(30)+NT(11)+T(23)+T(3)+NT(11)+NT(30)));
//G134: bf_gteq(102)         => __E_wff_40(103).
	p(NT(102), (NT(103)));
//G135: wff(14)              => bf_gteq(102).
	p(NT(14), (NT(102)));
//G136: __E_wff_41(105)      => bf(30) _(11) '!' '>' '=' _(11) bf(30).
	p(NT(105), (NT(30)+NT(11)+T(34)+T(23)+T(3)+NT(11)+NT(30)));
//G137: bf_ngteq(104)        => __E_wff_41(105).
	p(NT(104), (NT(105)));
//G138: wff(14)              => bf_ngteq(104).
	p(NT(14), (NT(104)));
//G139: __E_wff_42(107)      => bf(30) _(11) '<' _(11) bf(30).
	p(NT(107), (NT(30)+NT(11)+T(22)+NT(11)+NT(30)));
//G140: bf_lt(106)           => __E_wff_42(107).
	p(NT(106), (NT(107)));
//G141: wff(14)              => bf_lt(106).
	p(NT(14), (NT(106)));
//G142: __E_wff_43(109)      => bf(30) _(11) '!' '<' _(11) bf(30).
	p(NT(109), (NT(30)+NT(11)+T(34)+T(22)+NT(11)+NT(30)));
//G143: bf_nlt(108)          => __E_wff_43(109).
	p(NT(108), (NT(109)));
//G144: wff(14)              => bf_nlt(108).
	p(NT(14), (NT(108)));
//G145: __E_wff_44(111)      => bv(112) _(11) '=' '_' _(11) bv(112).
	p(NT(111), (NT(112)+NT(11)+T(3)+T(37)+NT(11)+NT(112)));
//G146: bv_eq(110)           => __E_wff_44(111).
	p(NT(110), (NT(111)));
//G147: wff(14)              => bv_eq(110).
	p(NT(14), (NT(110)));
//G148: __E_wff_45(114)      => bv(112) _(11) '!' '=' '_' _(11) bv(112).
	p(NT(114), (NT(112)+NT(11)+T(34)+T(3)+T(37)+NT(11)+NT(112)));
//G149: bv_neq(113)          => __E_wff_45(114).
	p(NT(113), (NT(114)));
//G150: wff(14)              => bv_neq(113).
	p(NT(14), (NT(113)));
//G151: __E_wff_46(116)      => bv(112) _(11) '<' '=' '_' _(11) bv(112).
	p(NT(116), (NT(112)+NT(11)+T(22)+T(3)+T(37)+NT(11)+NT(112)));
//G152: bv_lteq(115)         => __E_wff_46(116).
	p(NT(115), (NT(116)));
//G153: wff(14)              => bv_lteq(115).
	p(NT(14), (NT(115)));
//G154: __E_wff_47(118)      => bv(112) _(11) '!' '<' '=' '_' _(11) bv(112).
	p(NT(118), (NT(112)+NT(11)+T(34)+T(22)+T(3)+T(37)+NT(11)+NT(112)));
//G155: bv_nlteq(117)        => __E_wff_47(118).
	p(NT(117), (NT(118)));
//G156: wff(14)              => bv_nlteq(117).
	p(NT(14), (NT(117)));
//G157: __E_wff_48(120)      => bv(112) _(11) '>' '_' _(11) bv(112).
	p(NT(120), (NT(112)+NT(11)+T(23)+T(37)+NT(11)+NT(112)));
//G158: bv_gt(119)           => __E_wff_48(120).
	p(NT(119), (NT(120)));
//G159: wff(14)              => bv_gt(119).
	p(NT(14), (NT(119)));
//G160: __E_wff_49(122)      => bv(112) _(11) '!' '>' '_' _(11) bv(112).
	p(NT(122), (NT(112)+NT(11)+T(34)+T(23)+T(37)+NT(11)+NT(112)));
//G161: bv_ngt(121)          => __E_wff_49(122).
	p(NT(121), (NT(122)));
//G162: wff(14)              => bv_ngt(121).
	p(NT(14), (NT(121)));
//G163: __E_wff_50(124)      => bv(112) _(11) '>' '=' '_' _(11) bv(112).
	p(NT(124), (NT(112)+NT(11)+T(23)+T(3)+T(37)+NT(11)+NT(112)));
//G164: bv_gteq(123)         => __E_wff_50(124).
	p(NT(123), (NT(124)));
//G165: wff(14)              => bv_gteq(123).
	p(NT(14), (NT(123)));
//G166: __E_wff_51(126)      => bv(112) _(11) '!' '>' '=' '_' _(11) bv(112).
	p(NT(126), (NT(112)+NT(11)+T(34)+T(23)+T(3)+T(37)+NT(11)+NT(112)));
//G167: bv_ngteq(125)        => __E_wff_51(126).
	p(NT(125), (NT(126)));
//G168: wff(14)              => bv_ngteq(125).
	p(NT(14), (NT(125)));
//G169: __E_wff_52(128)      => bv(112) _(11) '<' '_' _(11) bv(112).
	p(NT(128), (NT(112)+NT(11)+T(22)+T(37)+NT(11)+NT(112)));
//G170: bv_lt(127)           => __E_wff_52(128).
	p(NT(127), (NT(128)));
//G171: wff(14)              => bv_lt(127).
	p(NT(14), (NT(127)));
//G172: __E_wff_53(130)      => bv(112) _(11) '!' '<' '_' _(11) bv(112).
	p(NT(130), (NT(112)+NT(11)+T(34)+T(22)+T(37)+NT(11)+NT(112)));
//G173: bv_nlt(129)          => __E_wff_53(130).
	p(NT(129), (NT(130)));
//G174: wff(14)              => bv_nlt(129).
	p(NT(14), (NT(129)));
//G175: wff(14)              => capture(29).
	p(NT(14), (NT(29)));
//G176: __E_bf_54(132)       => '(' _(11) bf(30) _(11) ')'.
	p(NT(132), (T(12)+NT(11)+NT(30)+NT(11)+T(13)));
//G177: bf_parenthesis(131)  => __E_bf_54(132).
	p(NT(131), (NT(132)));
//G178: bf(30)               => bf_parenthesis(131).
	p(NT(30), (NT(131)));
//G179: bf(30)               => ba_constant(133).
	p(NT(30), (NT(133)));
//G180: bf(30)               => variable(134).
	p(NT(30), (NT(134)));
//G181: __E_bf_55(136)       => 'f' 'a' 'l' 'l' __(34) q_vars(67) __(34) bf(30).
	p(NT(136), (T(9)+T(16)+T(10)+T(10)+NT(34)+NT(67)+NT(34)+NT(30)));
//G182: bf_fall(135)         => __E_bf_55(136).
	p(NT(135), (NT(136)));
//G183: bf(30)               => bf_fall(135).
	p(NT(30), (NT(135)));
//G184: __E_bf_56(138)       => 'f' 'e' 'x' __(34) q_vars(67) __(34) bf(30).
	p(NT(138), (T(9)+T(11)+T(29)+NT(34)+NT(67)+NT(34)+NT(30)));
//G185: bf_fex(137)          => __E_bf_56(138).
	p(NT(137), (NT(138)));
//G186: bf(30)               => bf_fex(137).
	p(NT(30), (NT(137)));
//G187: bf_ref(139)          => ref(27).
	p(NT(139), (NT(27)));
//G188: bf(30)               => bf_ref(139).
	p(NT(30), (NT(139)));
//G189: __E_bf_57(141)       => bf(30) _(11) '|' _(11) bf(30).
	p(NT(141), (NT(30)+NT(11)+T(31)+NT(11)+NT(30)));
//G190: bf_or(140)           => __E_bf_57(141).
	p(NT(140), (NT(141)));
//G191: bf(30)               => bf_or(140).
	p(NT(30), (NT(140)));
//G192: __E_bf_58(143)       => bf(30) _(11) '+' _(11) bf(30).
	p(NT(143), (NT(30)+NT(11)+T(38)+NT(11)+NT(30)));
//G193: bf_xor(142)          => __E_bf_58(143).
	p(NT(142), (NT(143)));
//G194: bf(30)               => bf_xor(142).
	p(NT(30), (NT(142)));
//G195: __E___E_bf_59_60(146) => typed(33).
	p(NT(146), (NT(33)));
//G196: __E___E_bf_59_60(146) => null.
	p(NT(146), (nul));
//G197: __E_bf_59(145)       => '1' __E___E_bf_59_60(146).
	p(NT(145), (T(39)+NT(146)));
//G198: bf_t(144)            => __E_bf_59(145).
	p(NT(144), (NT(145)));
//G199: bf(30)               => bf_t(144).
	p(NT(30), (NT(144)));
//G200: __E___E_bf_61_62(149) => typed(33).
	p(NT(149), (NT(33)));
//G201: __E___E_bf_61_62(149) => null.
	p(NT(149), (nul));
//G202: __E_bf_61(148)       => '0' __E___E_bf_61_62(149).
	p(NT(148), (T(40)+NT(149)));
//G203: bf_f(147)            => __E_bf_61(148).
	p(NT(147), (NT(148)));
//G204: bf(30)               => bf_f(147).
	p(NT(30), (NT(147)));
//G205: __E___E_bf_63_64(152) => _(11) '&' _(11).
	p(NT(152), (NT(11)+T(33)+NT(11)));
//G206: __E___E_bf_63_64(152) => __(34).
	p(NT(152), (NT(34)));
//G207: __E_bf_63(151)       => bf(30) __E___E_bf_63_64(152) bf(30).
	p(NT(151), (NT(30)+NT(152)+NT(30)));
//G208: bf_and(150)          => __E_bf_63(151).
	p(NT(150), (NT(151)));
//G209: bf(30)               => bf_and(150).
	p(NT(30), (NT(150)));
//G210: __E___E_bf_65_66(156) => bf_parenthesis(131).
	p(NT(156), (NT(131)));
//G211: __E___E_bf_65_66(156) => ba_constant(133).
	p(NT(156), (NT(133)));
//G212: __E___E_bf_65_66(156) => variable(134).
	p(NT(156), (NT(134)));
//G213: __E___E_bf_65_66(156) => bf_ref(139).
	p(NT(156), (NT(139)));
//G214: __E___E_bf_65_66(156) => bf_t(144).
	p(NT(156), (NT(144)));
//G215: __E___E_bf_65_66(156) => bf_f(147).
	p(NT(156), (NT(147)));
//G216: __E___E_bf_65_66(156) => bf_neg(153).
	p(NT(156), (NT(153)));
//G217: __E___E_bf_65_66(156) => capture(29).
	p(NT(156), (NT(29)));
//G218: bf_neg_oprnd(155)    => __E___E_bf_65_66(156).
	p(NT(155), (NT(156)));
//G219: __E_bf_65(154)       => bf_neg_oprnd(155) _(11) '\''.
	p(NT(154), (NT(155)+NT(11)+T(41)));
//G220: bf_neg(153)          => __E_bf_65(154).
	p(NT(153), (NT(154)));
//G221: bf(30)               => bf_neg(153).
	p(NT(30), (NT(153)));
//G222: __E___E_bf_67_68(160) => bf_parenthesis(131).
	p(NT(160), (NT(131)));
//G223: __E___E_bf_67_68(160) => variable(134).
	p(NT(160), (NT(134)));
//G224: __E___E_bf_67_68(160) => bf_ref(139).
	p(NT(160), (NT(139)));
//G225: __E___E_bf_67_68(160) => bf_neg(153).
	p(NT(160), (NT(153)));
//G226: __E___E_bf_67_68(160) => bf_and_nosep(157).
	p(NT(160), (NT(157)));
//G227: __E___E_bf_67_68(160) => capture(29).
	p(NT(160), (NT(29)));
//G228: bf_and_nosep_1st_oprnd(159) => __E___E_bf_67_68(160).
	p(NT(159), (NT(160)));
//G229: __E___E_bf_67_69(162) => bf_parenthesis(131).
	p(NT(162), (NT(131)));
//G230: __E___E_bf_67_69(162) => ba_constant(133).
	p(NT(162), (NT(133)));
//G231: __E___E_bf_67_69(162) => variable(134).
	p(NT(162), (NT(134)));
//G232: __E___E_bf_67_69(162) => bf_ref(139).
	p(NT(162), (NT(139)));
//G233: __E___E_bf_67_69(162) => bf_neg(153).
	p(NT(162), (NT(153)));
//G234: __E___E_bf_67_69(162) => capture(29).
	p(NT(162), (NT(29)));
//G235: bf_and_nosep_2nd_oprnd(161) => __E___E_bf_67_69(162).
	p(NT(161), (NT(162)));
//G236: __E_bf_67(158)       => bf_and_nosep_1st_oprnd(159) bf_and_nosep_2nd_oprnd(161).
	p(NT(158), (NT(159)+NT(161)));
//G237: bf_and_nosep(157)    => __E_bf_67(158).
	p(NT(157), (NT(158)));
//G238: bf(30)               => bf_and_nosep(157).
	p(NT(30), (NT(157)));
//G239: bf(30)               => capture(29).
	p(NT(30), (NT(29)));
//G240: __E_bv_70(164)       => '(' _(11) bv(112) _(11) ')'.
	p(NT(164), (T(12)+NT(11)+NT(112)+NT(11)+T(13)));
//G241: bv_parenthesis(163)  => __E_bv_70(164).
	p(NT(163), (NT(164)));
//G242: bv(112)              => bv_parenthesis(163).
	p(NT(112), (NT(163)));
//G243: __E_bv_71(166)       => '[' bv(112) ']'.
	p(NT(166), (T(26)+NT(112)+T(27)));
//G244: bv_checked(165)      => __E_bv_71(166).
	p(NT(165), (NT(166)));
//G245: bv(112)              => bv_checked(165).
	p(NT(112), (NT(165)));
//G246: bv(112)              => variable(134).
	p(NT(112), (NT(134)));
//G247: __E_bv_72(168)       => bv(112) _(11) '+' _(11) bv(112).
	p(NT(168), (NT(112)+NT(11)+T(38)+NT(11)+NT(112)));
//G248: bv_add(167)          => __E_bv_72(168).
	p(NT(167), (NT(168)));
//G249: bv(112)              => bv_add(167).
	p(NT(112), (NT(167)));
//G250: __E_bv_73(170)       => bv(112) _(11) '*' _(11) bv(112).
	p(NT(170), (NT(112)+NT(11)+T(42)+NT(11)+NT(112)));
//G251: bv_mul(169)          => __E_bv_73(170).
	p(NT(169), (NT(170)));
//G252: bv(112)              => bv_mul(169).
	p(NT(112), (NT(169)));
//G253: __E_bv_74(172)       => bv(112) _(11) '/' _(11) bv(112).
	p(NT(172), (NT(112)+NT(11)+T(43)+NT(11)+NT(112)));
//G254: bv_div(171)          => __E_bv_74(172).
	p(NT(171), (NT(172)));
//G255: bv(112)              => bv_div(171).
	p(NT(112), (NT(171)));
//G256: __E_bv_75(174)       => bv(112) _(11) '%' _(11) bv(112).
	p(NT(174), (NT(112)+NT(11)+T(44)+NT(11)+NT(112)));
//G257: bv_mod(173)          => __E_bv_75(174).
	p(NT(173), (NT(174)));
//G258: bv(112)              => bv_mod(173).
	p(NT(112), (NT(173)));
//G259: __E_bv_76(176)       => bv(112) _(11) '-' _(11) bv(112).
	p(NT(176), (NT(112)+NT(11)+T(30)+NT(11)+NT(112)));
//G260: bv_sub(175)          => __E_bv_76(176).
	p(NT(175), (NT(176)));
//G261: bv(112)              => bv_sub(175).
	p(NT(112), (NT(175)));
//G262: bv_neg(177)          => bv(112).
	p(NT(177), (NT(112)));
//G263: bv(112)              => '~' _(11) bv_neg(177).
	p(NT(112), (T(45)+NT(11)+NT(177)));
//G264: __E_bv_77(179)       => bv(112) _(11) '&' _(11) bv(112).
	p(NT(179), (NT(112)+NT(11)+T(33)+NT(11)+NT(112)));
//G265: bv_and(178)          => __E_bv_77(179).
	p(NT(178), (NT(179)));
//G266: bv(112)              => bv_and(178).
	p(NT(112), (NT(178)));
//G267: __E_bv_78(181)       => bv(112) _(11) '!' '&' _(11) bv(112).
	p(NT(181), (NT(112)+NT(11)+T(34)+T(33)+NT(11)+NT(112)));
//G268: bv_nand(180)         => __E_bv_78(181).
	p(NT(180), (NT(181)));
//G269: bv(112)              => bv_nand(180).
	p(NT(112), (NT(180)));
//G270: __E_bv_79(183)       => bv(112) _(11) '|' _(11) bv(112).
	p(NT(183), (NT(112)+NT(11)+T(31)+NT(11)+NT(112)));
//G271: bv_or(182)           => __E_bv_79(183).
	p(NT(182), (NT(183)));
//G272: bv(112)              => bv_or(182).
	p(NT(112), (NT(182)));
//G273: __E_bv_80(185)       => bv(112) _(11) '!' '|' _(11) bv(112).
	p(NT(185), (NT(112)+NT(11)+T(34)+T(31)+NT(11)+NT(112)));
//G274: bv_nor(184)          => __E_bv_80(185).
	p(NT(184), (NT(185)));
//G275: bv(112)              => bv_nor(184).
	p(NT(112), (NT(184)));
//G276: __E_bv_81(187)       => bv(112) _(11) '(' '+' ')' _(11) bv(112).
	p(NT(187), (NT(112)+NT(11)+T(12)+T(38)+T(13)+NT(11)+NT(112)));
//G277: bv_xor(186)          => __E_bv_81(187).
	p(NT(186), (NT(187)));
//G278: bv(112)              => bv_xor(186).
	p(NT(112), (NT(186)));
//G279: __E_bv_82(189)       => bv(112) _(11) '!' '(' '+' ')' _(11) bv(112).
	p(NT(189), (NT(112)+NT(11)+T(34)+T(12)+T(38)+T(13)+NT(11)+NT(112)));
//G280: bv_xnor(188)         => __E_bv_82(189).
	p(NT(188), (NT(189)));
//G281: bv(112)              => bv_xnor(188).
	p(NT(112), (NT(188)));
//G282: __E_bv_83(191)       => bv(112) _(11) '<' '<' _(11) bv(112).
	p(NT(191), (NT(112)+NT(11)+T(22)+T(22)+NT(11)+NT(112)));
//G283: bv_shl(190)          => __E_bv_83(191).
	p(NT(190), (NT(191)));
//G284: bv(112)              => bv_shl(190).
	p(NT(112), (NT(190)));
//G285: __E_bv_84(193)       => bv(112) _(11) '>' '>' _(11) bv(112).
	p(NT(193), (NT(112)+NT(11)+T(23)+T(23)+NT(11)+NT(112)));
//G286: bv_shr(192)          => __E_bv_84(193).
	p(NT(192), (NT(193)));
//G287: bv(112)              => bv_shr(192).
	p(NT(112), (NT(192)));
//G288: __E_bv_85(195)       => 'm' 'i' 'n' __(34) bv(112) __(34) bv(112).
	p(NT(195), (T(21)+T(4)+T(5)+NT(34)+NT(112)+NT(34)+NT(112)));
//G289: bv_min(194)          => __E_bv_85(195).
	p(NT(194), (NT(195)));
//G290: bv(112)              => bv_min(194).
	p(NT(112), (NT(194)));
//G291: __E_bv_86(197)       => 'm' 'a' 'x' __(34) bv(112) __(34) bv(112).
	p(NT(197), (T(21)+T(16)+T(29)+NT(34)+NT(112)+NT(34)+NT(112)));
//G292: bv_max(196)          => __E_bv_86(197).
	p(NT(196), (NT(197)));
//G293: bv(112)              => bv_max(196).
	p(NT(112), (NT(196)));
//G294: bv(112)              => bv_constant(198).
	p(NT(112), (NT(198)));
//G295: __E_bv_constant_87(200) => typed(33).
	p(NT(200), (NT(33)));
//G296: __E_bv_constant_87(200) => null.
	p(NT(200), (nul));
//G297: bv_constant(198)     => '#' 'b' binary(199) __E_bv_constant_87(200).
	p(NT(198), (T(46)+T(17)+NT(199)+NT(200)));
//G298: __E_bv_constant_88(202) => typed(33).
	p(NT(202), (NT(33)));
//G299: __E_bv_constant_88(202) => null.
	p(NT(202), (nul));
//G300: bv_constant(198)     => decimal(201) __E_bv_constant_88(202).
	p(NT(198), (NT(201)+NT(202)));
//G301: __E_bv_constant_89(204) => typed(33).
	p(NT(204), (NT(33)));
//G302: __E_bv_constant_89(204) => null.
	p(NT(204), (nul));
//G303: bv_constant(198)     => '#' 'x' hexadecimal(203) __E_bv_constant_89(204).
	p(NT(198), (T(46)+T(29)+NT(203)+NT(204)));
//G304: __E_binary_90(205)   => '0'.
	p(NT(205), (T(40)));
//G305: __E_binary_90(205)   => '1'.
	p(NT(205), (T(39)));
//G306: __E_binary_91(206)   => __E_binary_90(205).
	p(NT(206), (NT(205)));
//G307: __E_binary_91(206)   => __E_binary_90(205) __E_binary_91(206).
	p(NT(206), (NT(205)+NT(206)));
//G308: binary(199)          => __E_binary_91(206).
	p(NT(199), (NT(206)));
//G309: __E_hexadecimal_92(207) => xdigit(4).
	p(NT(207), (NT(4)));
//G310: __E_hexadecimal_92(207) => xdigit(4) __E_hexadecimal_92(207).
	p(NT(207), (NT(4)+NT(207)));
//G311: hexadecimal(203)     => __E_hexadecimal_92(207).
	p(NT(203), (NT(207)));
//G312: __E_decimal_93(208)  => digit(3).
	p(NT(208), (NT(3)));
//G313: __E_decimal_93(208)  => digit(3) __E_decimal_93(208).
	p(NT(208), (NT(3)+NT(208)));
//G314: decimal(201)         => __E_decimal_93(208).
	p(NT(201), (NT(208)));
//G315: __E_constraint_94(210) => '[' ctnvar(211) _(11) '!' '=' _(11) num(212) ']'.
	p(NT(210), (T(26)+NT(211)+NT(11)+T(34)+T(3)+NT(11)+NT(212)+T(27)));
//G316: __E_constraint_94(210) => '[' num(212) _(11) '!' '=' _(11) ctnvar(211) ']'.
	p(NT(210), (T(26)+NT(212)+NT(11)+T(34)+T(3)+NT(11)+NT(211)+T(27)));
//G317: ctn_neq(209)         => __E_constraint_94(210).
	p(NT(209), (NT(210)));
//G318: constraint(71)       => ctn_neq(209).
	p(NT(71), (NT(209)));
//G319: __E_constraint_95(214) => '[' ctnvar(211) _(11) '=' _(11) num(212) ']'.
	p(NT(214), (T(26)+NT(211)+NT(11)+T(3)+NT(11)+NT(212)+T(27)));
//G320: __E_constraint_95(214) => '[' num(212) _(11) '=' _(11) ctnvar(211) ']'.
	p(NT(214), (T(26)+NT(212)+NT(11)+T(3)+NT(11)+NT(211)+T(27)));
//G321: ctn_eq(213)          => __E_constraint_95(214).
	p(NT(213), (NT(214)));
//G322: constraint(71)       => ctn_eq(213).
	p(NT(71), (NT(213)));
//G323: __E_constraint_96(216) => '[' ctnvar(211) _(11) '>' '=' _(11) num(212) ']'.
	p(NT(216), (T(26)+NT(211)+NT(11)+T(23)+T(3)+NT(11)+NT(212)+T(27)));
//G324: __E_constraint_96(216) => '[' num(212) _(11) '>' '=' _(11) ctnvar(211) ']'.
	p(NT(216), (T(26)+NT(212)+NT(11)+T(23)+T(3)+NT(11)+NT(211)+T(27)));
//G325: ctn_gteq(215)        => __E_constraint_96(216).
	p(NT(215), (NT(216)));
//G326: constraint(71)       => ctn_gteq(215).
	p(NT(71), (NT(215)));
//G327: __E_constraint_97(218) => '[' ctnvar(211) _(11) '>' _(11) num(212) ']'.
	p(NT(218), (T(26)+NT(211)+NT(11)+T(23)+NT(11)+NT(212)+T(27)));
//G328: __E_constraint_97(218) => '[' num(212) _(11) '>' _(11) ctnvar(211) ']'.
	p(NT(218), (T(26)+NT(212)+NT(11)+T(23)+NT(11)+NT(211)+T(27)));
//G329: ctn_gt(217)          => __E_constraint_97(218).
	p(NT(217), (NT(218)));
//G330: constraint(71)       => ctn_gt(217).
	p(NT(71), (NT(217)));
//G331: __E_constraint_98(220) => '[' ctnvar(211) _(11) '<' '=' _(11) num(212) ']'.
	p(NT(220), (T(26)+NT(211)+NT(11)+T(22)+T(3)+NT(11)+NT(212)+T(27)));
//G332: __E_constraint_98(220) => '[' num(212) _(11) '<' '=' _(11) ctnvar(211) ']'.
	p(NT(220), (T(26)+NT(212)+NT(11)+T(22)+T(3)+NT(11)+NT(211)+T(27)));
//G333: ctn_lteq(219)        => __E_constraint_98(220).
	p(NT(219), (NT(220)));
//G334: constraint(71)       => ctn_lteq(219).
	p(NT(71), (NT(219)));
//G335: __E_constraint_99(222) => '[' ctnvar(211) _(11) '<' _(11) num(212) ']'.
	p(NT(222), (T(26)+NT(211)+NT(11)+T(22)+NT(11)+NT(212)+T(27)));
//G336: __E_constraint_99(222) => '[' num(212) _(11) '<' _(11) ctnvar(211) ']'.
	p(NT(222), (T(26)+NT(212)+NT(11)+T(22)+NT(11)+NT(211)+T(27)));
//G337: ctn_lt(221)          => __E_constraint_99(222).
	p(NT(221), (NT(222)));
//G338: constraint(71)       => ctn_lt(221).
	p(NT(71), (NT(221)));
//G339: __E_ba_constant_100(223) => capture(29).
	p(NT(223), (NT(29)));
//G340: __E_ba_constant_100(223) => source(224).
	p(NT(223), (NT(224)));
//G341: __E_ba_constant_101(225) => typed(33).
	p(NT(225), (NT(33)));
//G342: __E_ba_constant_101(225) => null.
	p(NT(225), (nul));
//G343: ba_constant(133)     => '{' _(11) __E_ba_constant_100(223) _(11) '}' __E_ba_constant_101(225).
	p(NT(133), (T(47)+NT(11)+NT(223)+NT(11)+T(48)+NT(225)));
//G344: __E___E_source_102_103(228) => src_c(226).
	p(NT(228), (NT(226)));
//G345: __E___E_source_102_103(228) => space(2).
	p(NT(228), (NT(2)));
//G346: __E___E_source_102_104(229) => null.
	p(NT(229), (nul));
//G347: __E___E_source_102_104(229) => __E___E_source_102_103(228) __E___E_source_102_104(229).
	p(NT(229), (NT(228)+NT(229)));
//G348: __E_source_102(227)  => __E___E_source_102_104(229) src_c(226).
	p(NT(227), (NT(229)+NT(226)));
//G349: __E_source_102(227)  => null.
	p(NT(227), (nul));
//G350: source(224)          => src_c(226) __E_source_102(227).
	p(NT(224), (NT(226)+NT(227)));
//G351: src_c(226)           => alnum(6).
	p(NT(226), (NT(6)));
//G352: __N_0(420)           => '{'.
	p(NT(420), (T(47)));
//G353: __N_1(421)           => '}'.
	p(NT(421), (T(48)));
//G354: src_c(226)           => ~( __N_0(420) ) & ~( __N_1(421) ) & punct(7).	 # conjunctive
	p(NT(226), ~(NT(420)) & ~(NT(421)) & (NT(7)));
//G355: __E_src_c_105(230)   => src_c(226).
	p(NT(230), (NT(226)));
//G356: __E_src_c_105(230)   => space(2).
	p(NT(230), (NT(2)));
//G357: __E_src_c_106(231)   => null.
	p(NT(231), (nul));
//G358: __E_src_c_106(231)   => __E_src_c_105(230) __E_src_c_106(231).
	p(NT(231), (NT(230)+NT(231)));
//G359: src_c(226)           => '{' __E_src_c_106(231) '}'.
	p(NT(226), (T(47)+NT(231)+T(48)));
//G360: __E_variable_107(232) => uconst(233).
	p(NT(232), (NT(233)));
//G361: __E_variable_107(232) => io_var(234).
	p(NT(232), (NT(234)));
//G362: __E_variable_107(232) => var_name(235).
	p(NT(232), (NT(235)));
//G363: __E_variable_108(236) => typed(33).
	p(NT(236), (NT(33)));
//G364: __E_variable_108(236) => null.
	p(NT(236), (nul));
//G365: variable(134)        => __E_variable_107(232) __E_variable_108(236).
	p(NT(134), (NT(232)+NT(236)));
//G366: __N_2(422)           => 'F'.
	p(NT(422), (T(36)));
//G367: __N_3(423)           => 'T'.
	p(NT(423), (T(35)));
//G368: __E_var_name_109(237) => ~( __N_2(422) ) & ~( __N_3(423) ) & alpha(5).	 # conjunctive
	p(NT(237), ~(NT(422)) & ~(NT(423)) & (NT(5)));
//G369: __E_var_name_110(238) => null.
	p(NT(238), (nul));
//G370: __E_var_name_110(238) => digit(3) __E_var_name_110(238).
	p(NT(238), (NT(3)+NT(238)));
//G371: var_name(235)        => __E_var_name_109(237) __E_var_name_110(238).	 # guarded: charvar
	p(NT(235), (NT(237)+NT(238)));
	p.back().guard = "charvar";
//G372: __N_4(424)           => 'F'.
	p(NT(424), (T(36)));
//G373: __N_5(425)           => 'T'.
	p(NT(425), (T(35)));
//G374: __E_var_name_111(239) => ~( __N_4(424) ) & ~( __N_5(425) ) & alpha(5).	 # conjunctive
	p(NT(239), ~(NT(424)) & ~(NT(425)) & (NT(5)));
//G375: __E_var_name_112(240) => alnum(6).
	p(NT(240), (NT(6)));
//G376: __E_var_name_112(240) => '_'.
	p(NT(240), (T(37)));
//G377: __E_var_name_113(241) => null.
	p(NT(241), (nul));
//G378: __E_var_name_113(241) => __E_var_name_112(240) __E_var_name_113(241).
	p(NT(241), (NT(240)+NT(241)));
//G379: var_name(235)        => __E_var_name_111(239) __E_var_name_113(241).	 # guarded: var
	p(NT(235), (NT(239)+NT(241)));
	p.back().guard = "var";
//G380: io_var(234)          => io_var_name(31) '[' offset(242) ']'.
	p(NT(234), (NT(31)+T(26)+NT(242)+T(27)));
//G381: io_var_name(31)      => chars(243).
	p(NT(31), (NT(243)));
//G382: __E___E_uconst_114_115(246) => chars(243) _(11).
	p(NT(246), (NT(243)+NT(11)));
//G383: __E___E_uconst_114_115(246) => null.
	p(NT(246), (nul));
//G384: __E_uconst_114(245)  => __E___E_uconst_114_115(246) ':' _(11) chars(243).
	p(NT(245), (NT(246)+T(2)+NT(11)+NT(243)));
//G385: uconst_name(244)     => __E_uconst_114(245).
	p(NT(244), (NT(245)));
//G386: uconst(233)          => '<' _(11) uconst_name(244) _(11) '>'.
	p(NT(233), (T(22)+NT(11)+NT(244)+NT(11)+T(23)));
//G387: __E_q_vars_116(248)  => _(11) ',' _(11) q_var(247).
	p(NT(248), (NT(11)+T(20)+NT(11)+NT(247)));
//G388: __E_q_vars_117(249)  => null.
	p(NT(249), (nul));
//G389: __E_q_vars_117(249)  => __E_q_vars_116(248) __E_q_vars_117(249).
	p(NT(249), (NT(248)+NT(249)));
//G390: q_vars(67)           => q_var(247) __E_q_vars_117(249).
	p(NT(67), (NT(247)+NT(249)));
//G391: q_var(247)           => capture(29).
	p(NT(247), (NT(29)));
//G392: __N_6(426)           => uconst(233).
	p(NT(426), (NT(233)));
//G393: q_var(247)           => variable(134) & ~( __N_6(426) ).	 # conjunctive
	p(NT(247), (NT(134)) & ~(NT(426)));
//G394: ctnvar(211)          => chars(243).
	p(NT(211), (NT(243)));
//G395: __E_offsets_118(250) => _(11) ',' _(11) offset(242).
	p(NT(250), (NT(11)+T(20)+NT(11)+NT(242)));
//G396: __E_offsets_119(251) => null.
	p(NT(251), (nul));
//G397: __E_offsets_119(251) => __E_offsets_118(250) __E_offsets_119(251).
	p(NT(251), (NT(250)+NT(251)));
//G398: offsets(42)          => '[' _(11) offset(242) __E_offsets_119(251) _(11) ']'.
	p(NT(42), (T(26)+NT(11)+NT(242)+NT(251)+NT(11)+T(27)));
//G399: offset(242)          => integer(252).
	p(NT(242), (NT(252)));
//G400: offset(242)          => capture(29).
	p(NT(242), (NT(29)));
//G401: offset(242)          => shift(253).
	p(NT(242), (NT(253)));
//G402: __N_7(427)           => io_var(234).
	p(NT(427), (NT(234)));
//G403: __E_offset_120(254)  => variable(134) & ~( __N_7(427) ).	 # conjunctive
	p(NT(254), (NT(134)) & ~(NT(427)));
//G404: offset(242)          => __E_offset_120(254).
	p(NT(242), (NT(254)));
//G405: __E_shift_121(255)   => capture(29).
	p(NT(255), (NT(29)));
//G406: __N_8(428)           => io_var(234).
	p(NT(428), (NT(234)));
//G407: __E___E_shift_121_122(256) => variable(134) & ~( __N_8(428) ).	 # conjunctive
	p(NT(256), (NT(134)) & ~(NT(428)));
//G408: __E_shift_121(255)   => __E___E_shift_121_122(256).
	p(NT(255), (NT(256)));
//G409: shift(253)           => __E_shift_121(255) _(11) '-' _(11) num(212).
	p(NT(253), (NT(255)+NT(11)+T(30)+NT(11)+NT(212)));
//G410: __E_chars_123(257)   => alnum(6).
	p(NT(257), (NT(6)));
//G411: __E_chars_123(257)   => '_'.
	p(NT(257), (T(37)));
//G412: __E_chars_124(258)   => null.
	p(NT(258), (nul));
//G413: __E_chars_124(258)   => __E_chars_123(257) __E_chars_124(258).
	p(NT(258), (NT(257)+NT(258)));
//G414: chars(243)           => alpha(5) __E_chars_124(258).
	p(NT(243), (NT(5)+NT(258)));
//G415: __E_digits_125(260)  => digit(3).
	p(NT(260), (NT(3)));
//G416: __E_digits_125(260)  => digit(3) __E_digits_125(260).
	p(NT(260), (NT(3)+NT(260)));
//G417: digits(259)          => __E_digits_125(260).
	p(NT(259), (NT(260)));
//G418: num(212)             => digits(259).
	p(NT(212), (NT(259)));
//G419: __E_integer_126(261) => '-'.
	p(NT(261), (T(30)));
//G420: __E_integer_126(261) => null.
	p(NT(261), (nul));
//G421: integer(252)         => __E_integer_126(261) _(11) digits(259).
	p(NT(252), (NT(261)+NT(11)+NT(259)));
//G422: sym(40)              => chars(243).
	p(NT(40), (NT(243)));
//G423: capture(29)          => '$' chars(243).
	p(NT(29), (T(49)+NT(243)));
//G424: __E_typed_127(263)   => _(11) '[' _(11) subtype(264) _(11) ']' _(11).
	p(NT(263), (NT(11)+T(26)+NT(11)+NT(264)+NT(11)+T(27)+NT(11)));
//G425: __E_typed_127(263)   => null.
	p(NT(263), (nul));
//G426: typed(33)            => _(11) ':' _(11) type(262) __E_typed_127(263).
	p(NT(33), (NT(11)+T(2)+NT(11)+NT(262)+NT(263)));
//G427: type(262)            => chars(243).
	p(NT(262), (NT(243)));
//G428: subtype(264)         => num(212).
	p(NT(264), (NT(212)));
//G429: __E_comment_128(266) => printable(8).
	p(NT(266), (NT(8)));
//G430: __E_comment_128(266) => '\t'.
	p(NT(266), (T(50)));
//G431: __E_comment_129(267) => null.
	p(NT(267), (nul));
//G432: __E_comment_129(267) => __E_comment_128(266) __E_comment_129(267).
	p(NT(267), (NT(266)+NT(267)));
//G433: __E_comment_130(268) => '\n'.
	p(NT(268), (T(51)));
//G434: __E_comment_130(268) => '\r'.
	p(NT(268), (T(52)));
//G435: __E_comment_130(268) => eof(1).
	p(NT(268), (NT(1)));
//G436: comment(265)         => '#' __E_comment_129(267) __E_comment_130(268).
	p(NT(265), (T(46)+NT(267)+NT(268)));
//G437: __(34)               => space(2).
	p(NT(34), (NT(2)));
//G438: __(34)               => comment(265).
	p(NT(34), (NT(265)));
//G439: __(34)               => __(34) space(2).
	p(NT(34), (NT(34)+NT(2)));
//G440: __(34)               => __(34) comment(265).
	p(NT(34), (NT(34)+NT(265)));
//G441: _(11)                => __(34).
	p(NT(11), (NT(34)));
//G442: _(11)                => null.
	p(NT(11), (nul));
//G443: cli(269)             => _(11).
	p(NT(269), (NT(11)));
//G444: __E_cli_131(271)     => '.' _(11) cli_command(270) _(11).
	p(NT(271), (T(1)+NT(11)+NT(270)+NT(11)));
//G445: __E_cli_132(272)     => null.
	p(NT(272), (nul));
//G446: __E_cli_132(272)     => __E_cli_131(271) __E_cli_132(272).
	p(NT(272), (NT(271)+NT(272)));
//G447: cli(269)             => _(11) cli_command(270) _(11) __E_cli_132(272).
	p(NT(269), (NT(11)+NT(270)+NT(11)+NT(272)));
//G448: __E_cli_command_133(275) => 'q'.
	p(NT(275), (T(53)));
//G449: __E_cli_command_133(275) => 'q' 'u' 'i' 't'.
	p(NT(275), (T(53)+T(7)+T(4)+T(8)));
//G450: quit_sym(274)        => __E_cli_command_133(275).
	p(NT(274), (NT(275)));
//G451: quit_cmd(273)        => quit_sym(274).
	p(NT(273), (NT(274)));
//G452: cli_command(270)     => quit_cmd(273).
	p(NT(270), (NT(273)));
//G453: __E_cli_command_134(278) => 'v'.
	p(NT(278), (T(54)));
//G454: __E_cli_command_134(278) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(278), (T(54)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G455: version_sym(277)     => __E_cli_command_134(278).
	p(NT(277), (NT(278)));
//G456: version_cmd(276)     => version_sym(277).
	p(NT(276), (NT(277)));
//G457: cli_command(270)     => version_cmd(276).
	p(NT(270), (NT(276)));
//G458: __E_cli_command_135(281) => 'c'.
	p(NT(281), (T(14)));
//G459: __E_cli_command_135(281) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(281), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G460: clear_sym(280)       => __E_cli_command_135(281).
	p(NT(280), (NT(281)));
//G461: clear_cmd(279)       => clear_sym(280).
	p(NT(279), (NT(280)));
//G462: cli_command(270)     => clear_cmd(279).
	p(NT(270), (NT(279)));
//G463: __E___E_cli_command_136_137(285) => 'h'.
	p(NT(285), (T(55)));
//G464: __E___E_cli_command_136_137(285) => 'h' 'e' 'l' 'p'.
	p(NT(285), (T(55)+T(11)+T(10)+T(56)));
//G465: help_sym(284)        => __E___E_cli_command_136_137(285).
	p(NT(284), (NT(285)));
//G466: __E___E_cli_command_136_138(286) => __(34) help_arg(287).
	p(NT(286), (NT(34)+NT(287)));
//G467: __E___E_cli_command_136_138(286) => null.
	p(NT(286), (nul));
//G468: __E_cli_command_136(283) => help_sym(284) __E___E_cli_command_136_138(286).
	p(NT(283), (NT(284)+NT(286)));
//G469: help_cmd(282)        => __E_cli_command_136(283).
	p(NT(282), (NT(283)));
//G470: cli_command(270)     => help_cmd(282).
	p(NT(270), (NT(282)));
//G471: file_sym(290)        => 'f' 'i' 'l' 'e'.
	p(NT(290), (T(9)+T(4)+T(10)+T(11)));
//G472: __E_cli_command_139(289) => file_sym(290) __(34) q_string(291).
	p(NT(289), (NT(290)+NT(34)+NT(291)));
//G473: file_cmd(288)        => __E_cli_command_139(289).
	p(NT(288), (NT(289)));
//G474: cli_command(270)     => file_cmd(288).
	p(NT(270), (NT(288)));
//G475: valid_sym(294)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(294), (T(54)+T(16)+T(10)+T(4)+T(57)));
//G476: __E_cli_command_140(293) => valid_sym(294) __(34) normalize_cmd_arg(295).
	p(NT(293), (NT(294)+NT(34)+NT(295)));
//G477: valid_cmd(292)       => __E_cli_command_140(293).
	p(NT(292), (NT(293)));
//G478: cli_command(270)     => valid_cmd(292).
	p(NT(270), (NT(292)));
//G479: sat_sym(298)         => 's' 'a' 't'.
	p(NT(298), (T(15)+T(16)+T(8)));
//G480: __E_cli_command_141(297) => sat_sym(298) __(34) normalize_cmd_arg(295).
	p(NT(297), (NT(298)+NT(34)+NT(295)));
//G481: sat_cmd(296)         => __E_cli_command_141(297).
	p(NT(296), (NT(297)));
//G482: cli_command(270)     => sat_cmd(296).
	p(NT(270), (NT(296)));
//G483: unsat_sym(301)       => 'u' 'n' 's' 'a' 't'.
	p(NT(301), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G484: __E_cli_command_142(300) => unsat_sym(301) __(34) normalize_cmd_arg(295).
	p(NT(300), (NT(301)+NT(34)+NT(295)));
//G485: unsat_cmd(299)       => __E_cli_command_142(300).
	p(NT(299), (NT(300)));
//G486: cli_command(270)     => unsat_cmd(299).
	p(NT(270), (NT(299)));
//G487: solve_sym(304)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(304), (T(15)+T(6)+T(10)+T(54)+T(11)));
//G488: __E___E_cli_command_143_144(305) => solve_options(306).
	p(NT(305), (NT(306)));
//G489: __E___E_cli_command_143_144(305) => null.
	p(NT(305), (nul));
//G490: __E_cli_command_143(303) => solve_sym(304) __E___E_cli_command_143_144(305) __(34) wff_cmd_arg(307).
	p(NT(303), (NT(304)+NT(305)+NT(34)+NT(307)));
//G491: solve_cmd(302)       => __E_cli_command_143(303).
	p(NT(302), (NT(303)));
//G492: cli_command(270)     => solve_cmd(302).
	p(NT(270), (NT(302)));
//G493: lgrs_sym(310)        => 'l' 'g' 'r' 's'.
	p(NT(310), (T(10)+T(58)+T(19)+T(15)));
//G494: __E_cli_command_145(309) => lgrs_sym(310) __(34) wff_cmd_arg(307).
	p(NT(309), (NT(310)+NT(34)+NT(307)));
//G495: lgrs_cmd(308)        => __E_cli_command_145(309).
	p(NT(308), (NT(309)));
//G496: cli_command(270)     => lgrs_cmd(308).
	p(NT(270), (NT(308)));
//G497: __E___E_cli_command_146_147(314) => 'r'.
	p(NT(314), (T(19)));
//G498: __E___E_cli_command_146_147(314) => 'r' 'u' 'n'.
	p(NT(314), (T(19)+T(7)+T(5)));
//G499: run_sym(313)         => __E___E_cli_command_146_147(314).
	p(NT(313), (NT(314)));
//G500: __E___E_cli_command_146_148(315) => history(316).
	p(NT(315), (NT(316)));
//G501: __E___E_cli_command_146_148(315) => wff(14).
	p(NT(315), (NT(14)));
//G502: __E_cli_command_146(312) => run_sym(313) __(34) __E___E_cli_command_146_148(315).
	p(NT(312), (NT(313)+NT(34)+NT(315)));
//G503: run_cmd(311)         => __E_cli_command_146(312).
	p(NT(311), (NT(312)));
//G504: cli_command(270)     => run_cmd(311).
	p(NT(270), (NT(311)));
//G505: __E___E_cli_command_149_150(320) => 'n'.
	p(NT(320), (T(5)));
//G506: __E___E_cli_command_149_150(320) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(320), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(59)+T(11)));
//G507: normalize_sym(319)   => __E___E_cli_command_149_150(320).
	p(NT(319), (NT(320)));
//G508: __E_cli_command_149(318) => normalize_sym(319) __(34) normalize_cmd_arg(295).
	p(NT(318), (NT(319)+NT(34)+NT(295)));
//G509: normalize_cmd(317)   => __E_cli_command_149(318).
	p(NT(317), (NT(318)));
//G510: cli_command(270)     => normalize_cmd(317).
	p(NT(270), (NT(317)));
//G511: __E___E_cli_command_151_152(324) => 's'.
	p(NT(324), (T(15)));
//G512: __E___E_cli_command_151_152(324) => 's' 'u' 'b' 's' 't'.
	p(NT(324), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G513: __E___E_cli_command_151_152(324) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(324), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G514: subst_sym(323)       => __E___E_cli_command_151_152(324).
	p(NT(323), (NT(324)));
//G515: __E_cli_command_151(322) => subst_sym(323) __(34) nf_cmd_arg(325) _(11) '[' _(11) nf_cmd_arg(325) _(11) '/' _(11) nf_cmd_arg(325) _(11) ']'.
	p(NT(322), (NT(323)+NT(34)+NT(325)+NT(11)+T(26)+NT(11)+NT(325)+NT(11)+T(43)+NT(11)+NT(325)+NT(11)+T(27)));
//G516: subst_cmd(321)       => __E_cli_command_151(322).
	p(NT(321), (NT(322)));
//G517: cli_command(270)     => subst_cmd(321).
	p(NT(270), (NT(321)));
//G518: __E___E_cli_command_153_154(329) => 'i'.
	p(NT(329), (T(4)));
//G519: __E___E_cli_command_153_154(329) => 'i' 'n' 's' 't'.
	p(NT(329), (T(4)+T(5)+T(15)+T(8)));
//G520: __E___E_cli_command_153_154(329) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(329), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G521: inst_sym(328)        => __E___E_cli_command_153_154(329).
	p(NT(328), (NT(329)));
//G522: __E_cli_command_153(327) => inst_sym(328) __(34) inst_args(330).
	p(NT(327), (NT(328)+NT(34)+NT(330)));
//G523: inst_cmd(326)        => __E_cli_command_153(327).
	p(NT(326), (NT(327)));
//G524: cli_command(270)     => inst_cmd(326).
	p(NT(270), (NT(326)));
//G525: dnf_sym(333)         => 'd' 'n' 'f'.
	p(NT(333), (T(57)+T(5)+T(9)));
//G526: __E_cli_command_155(332) => dnf_sym(333) __(34) nf_cmd_arg(325).
	p(NT(332), (NT(333)+NT(34)+NT(325)));
//G527: dnf_cmd(331)         => __E_cli_command_155(332).
	p(NT(331), (NT(332)));
//G528: cli_command(270)     => dnf_cmd(331).
	p(NT(270), (NT(331)));
//G529: cnf_sym(336)         => 'c' 'n' 'f'.
	p(NT(336), (T(14)+T(5)+T(9)));
//G530: __E_cli_command_156(335) => cnf_sym(336) __(34) nf_cmd_arg(325).
	p(NT(335), (NT(336)+NT(34)+NT(325)));
//G531: cnf_cmd(334)         => __E_cli_command_156(335).
	p(NT(334), (NT(335)));
//G532: cli_command(270)     => cnf_cmd(334).
	p(NT(270), (NT(334)));
//G533: anf_sym(339)         => 'a' 'n' 'f'.
	p(NT(339), (T(16)+T(5)+T(9)));
//G534: __E_cli_command_157(338) => anf_sym(339) __(34) nf_cmd_arg(325).
	p(NT(338), (NT(339)+NT(34)+NT(325)));
//G535: anf_cmd(337)         => __E_cli_command_157(338).
	p(NT(337), (NT(338)));
//G536: cli_command(270)     => anf_cmd(337).
	p(NT(270), (NT(337)));
//G537: nnf_sym(342)         => 'n' 'n' 'f'.
	p(NT(342), (T(5)+T(5)+T(9)));
//G538: __E_cli_command_158(341) => nnf_sym(342) __(34) nf_cmd_arg(325).
	p(NT(341), (NT(342)+NT(34)+NT(325)));
//G539: nnf_cmd(340)         => __E_cli_command_158(341).
	p(NT(340), (NT(341)));
//G540: cli_command(270)     => nnf_cmd(340).
	p(NT(270), (NT(340)));
//G541: pnf_sym(345)         => 'p' 'n' 'f'.
	p(NT(345), (T(56)+T(5)+T(9)));
//G542: __E_cli_command_159(344) => pnf_sym(345) __(34) nf_cmd_arg(325).
	p(NT(344), (NT(345)+NT(34)+NT(325)));
//G543: pnf_cmd(343)         => __E_cli_command_159(344).
	p(NT(343), (NT(344)));
//G544: cli_command(270)     => pnf_cmd(343).
	p(NT(270), (NT(343)));
//G545: mnf_sym(348)         => 'm' 'n' 'f'.
	p(NT(348), (T(21)+T(5)+T(9)));
//G546: __E_cli_command_160(347) => mnf_sym(348) __(34) nf_cmd_arg(325).
	p(NT(347), (NT(348)+NT(34)+NT(325)));
//G547: mnf_cmd(346)         => __E_cli_command_160(347).
	p(NT(346), (NT(347)));
//G548: cli_command(270)     => mnf_cmd(346).
	p(NT(270), (NT(346)));
//G549: onf_sym(351)         => 'o' 'n' 'f'.
	p(NT(351), (T(6)+T(5)+T(9)));
//G550: __E_cli_command_161(350) => onf_sym(351) __(34) variable(134) __(34) onf_cmd_arg(352).
	p(NT(350), (NT(351)+NT(34)+NT(134)+NT(34)+NT(352)));
//G551: onf_cmd(349)         => __E_cli_command_161(350).
	p(NT(349), (NT(350)));
//G552: cli_command(270)     => onf_cmd(349).
	p(NT(270), (NT(349)));
//G553: qelim_sym(355)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(355), (T(53)+T(11)+T(10)+T(4)+T(21)));
//G554: __E_cli_command_162(354) => qelim_sym(355) __(34) wff_cmd_arg(307).
	p(NT(354), (NT(355)+NT(34)+NT(307)));
//G555: qelim_cmd(353)       => __E_cli_command_162(354).
	p(NT(353), (NT(354)));
//G556: cli_command(270)     => qelim_cmd(353).
	p(NT(270), (NT(353)));
//G557: get_sym(358)         => 'g' 'e' 't'.
	p(NT(358), (T(58)+T(11)+T(8)));
//G558: __E___E_cli_command_163_164(359) => __(34) option_name(360).
	p(NT(359), (NT(34)+NT(360)));
//G559: __E___E_cli_command_163_164(359) => null.
	p(NT(359), (nul));
//G560: __E_cli_command_163(357) => get_sym(358) __E___E_cli_command_163_164(359).
	p(NT(357), (NT(358)+NT(359)));
//G561: get_cmd(356)         => __E_cli_command_163(357).
	p(NT(356), (NT(357)));
//G562: cli_command(270)     => get_cmd(356).
	p(NT(270), (NT(356)));
//G563: set_sym(363)         => 's' 'e' 't'.
	p(NT(363), (T(15)+T(11)+T(8)));
//G564: __E___E_cli_command_165_166(364) => __(34).
	p(NT(364), (NT(34)));
//G565: __E___E_cli_command_165_166(364) => _(11) '=' _(11).
	p(NT(364), (NT(11)+T(3)+NT(11)));
//G566: __E_cli_command_165(362) => set_sym(363) __(34) option_name(360) __E___E_cli_command_165_166(364) option_value(365).
	p(NT(362), (NT(363)+NT(34)+NT(360)+NT(364)+NT(365)));
//G567: set_cmd(361)         => __E_cli_command_165(362).
	p(NT(361), (NT(362)));
//G568: cli_command(270)     => set_cmd(361).
	p(NT(270), (NT(361)));
//G569: enable_sym(368)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(368), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G570: __E_cli_command_167(367) => enable_sym(368) __(34) option_name(360).
	p(NT(367), (NT(368)+NT(34)+NT(360)));
//G571: enable_cmd(366)      => __E_cli_command_167(367).
	p(NT(366), (NT(367)));
//G572: cli_command(270)     => enable_cmd(366).
	p(NT(270), (NT(366)));
//G573: disable_sym(371)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(371), (T(57)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G574: __E_cli_command_168(370) => disable_sym(371) __(34) option_name(360).
	p(NT(370), (NT(371)+NT(34)+NT(360)));
//G575: disable_cmd(369)     => __E_cli_command_168(370).
	p(NT(369), (NT(370)));
//G576: cli_command(270)     => disable_cmd(369).
	p(NT(270), (NT(369)));
//G577: toggle_sym(374)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(374), (T(8)+T(6)+T(58)+T(58)+T(10)+T(11)));
//G578: __E_cli_command_169(373) => toggle_sym(374) __(34) option_name(360).
	p(NT(373), (NT(374)+NT(34)+NT(360)));
//G579: toggle_cmd(372)      => __E_cli_command_169(373).
	p(NT(372), (NT(373)));
//G580: cli_command(270)     => toggle_cmd(372).
	p(NT(270), (NT(372)));
//G581: __E___E_cli_command_170_171(378) => 'd' 'e' 'f' 's'.
	p(NT(378), (T(57)+T(11)+T(9)+T(15)));
//G582: __E___E_cli_command_170_171(378) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(378), (T(57)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G583: def_sym(377)         => __E___E_cli_command_170_171(378).
	p(NT(377), (NT(378)));
//G584: __E_cli_command_170(376) => def_sym(377).
	p(NT(376), (NT(377)));
//G585: def_list_cmd(375)    => __E_cli_command_170(376).
	p(NT(375), (NT(376)));
//G586: cli_command(270)     => def_list_cmd(375).
	p(NT(270), (NT(375)));
//G587: __E_cli_command_172(380) => def_sym(377) __(34) num(212).
	p(NT(380), (NT(377)+NT(34)+NT(212)));
//G588: def_print_cmd(379)   => __E_cli_command_172(380).
	p(NT(379), (NT(380)));
//G589: cli_command(270)     => def_print_cmd(379).
	p(NT(270), (NT(379)));
//G590: def_rr_cmd(381)      => rec_relation(17).
	p(NT(381), (NT(17)));
//G591: cli_command(270)     => def_rr_cmd(381).
	p(NT(270), (NT(381)));
//G592: def_input_cmd(382)   => input_def(18).
	p(NT(382), (NT(18)));
//G593: cli_command(270)     => def_input_cmd(382).
	p(NT(270), (NT(382)));
//G594: def_output_cmd(383)  => output_def(19).
	p(NT(383), (NT(19)));
//G595: cli_command(270)     => def_output_cmd(383).
	p(NT(270), (NT(383)));
//G596: __E___E_cli_command_173_174(387) => 'h' 'i' 's' 't'.
	p(NT(387), (T(55)+T(4)+T(15)+T(8)));
//G597: __E___E_cli_command_173_174(387) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(387), (T(55)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G598: history_sym(386)     => __E___E_cli_command_173_174(387).
	p(NT(386), (NT(387)));
//G599: __E_cli_command_173(385) => history_sym(386).
	p(NT(385), (NT(386)));
//G600: history_list_cmd(384) => __E_cli_command_173(385).
	p(NT(384), (NT(385)));
//G601: cli_command(270)     => history_list_cmd(384).
	p(NT(270), (NT(384)));
//G602: __E_cli_command_175(389) => history_sym(386) __(34) history(316).
	p(NT(389), (NT(386)+NT(34)+NT(316)));
//G603: history_print_cmd(388) => __E_cli_command_175(389).
	p(NT(388), (NT(389)));
//G604: cli_command(270)     => history_print_cmd(388).
	p(NT(270), (NT(388)));
//G605: __E_cli_command_176(391) => wff(14).
	p(NT(391), (NT(14)));
//G606: __E_cli_command_176(391) => bf(30).
	p(NT(391), (NT(30)));
//G607: history_store_cmd(390) => __E_cli_command_176(391).
	p(NT(390), (NT(391)));
//G608: cli_command(270)     => history_store_cmd(390).
	p(NT(270), (NT(390)));
//G609: __E_solve_options_177(392) => __(34) solve_option(393).
	p(NT(392), (NT(34)+NT(393)));
//G610: __E_solve_options_178(394) => null.
	p(NT(394), (nul));
//G611: __E_solve_options_178(394) => __E_solve_options_177(392) __E_solve_options_178(394).
	p(NT(394), (NT(392)+NT(394)));
//G612: solve_options(306)   => __E_solve_options_178(394).
	p(NT(306), (NT(394)));
//G613: __E_solve_option_179(395) => solver_mode(396).
	p(NT(395), (NT(396)));
//G614: __E_solve_option_179(395) => type(262).
	p(NT(395), (NT(262)));
//G615: solve_option(393)    => '-' '-' __E_solve_option_179(395).
	p(NT(393), (T(30)+T(30)+NT(395)));
//G616: __E_solver_mode_180(398) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(398), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G617: __E_solver_mode_180(398) => 'm' 'i' 'n'.
	p(NT(398), (T(21)+T(4)+T(5)));
//G618: solver_mode_minimum(397) => __E_solver_mode_180(398).
	p(NT(397), (NT(398)));
//G619: solver_mode(396)     => solver_mode_minimum(397).
	p(NT(396), (NT(397)));
//G620: __E_solver_mode_181(400) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(400), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G621: __E_solver_mode_181(400) => 'm' 'a' 'x'.
	p(NT(400), (T(21)+T(16)+T(29)));
//G622: solver_mode_maximum(399) => __E_solver_mode_181(400).
	p(NT(399), (NT(400)));
//G623: solver_mode(396)     => solver_mode_maximum(399).
	p(NT(396), (NT(399)));
//G624: q_file_name(38)      => '"' file_name(401) '"'.
	p(NT(38), (T(60)+NT(401)+T(60)));
//G625: __E_file_name_182(402) => printable(8).
	p(NT(402), (NT(8)));
//G626: __E_file_name_182(402) => printable(8) __E_file_name_182(402).
	p(NT(402), (NT(8)+NT(402)));
//G627: file_name(401)       => __E_file_name_182(402).
	p(NT(401), (NT(402)));
//G628: __E_option_name_183(403) => alnum(6).
	p(NT(403), (NT(6)));
//G629: __E_option_name_183(403) => alnum(6) __E_option_name_183(403).
	p(NT(403), (NT(6)+NT(403)));
//G630: option_name(360)     => __E_option_name_183(403).
	p(NT(360), (NT(403)));
//G631: __E_option_value_184(404) => alnum(6).
	p(NT(404), (NT(6)));
//G632: __E_option_value_184(404) => alnum(6) __E_option_value_184(404).
	p(NT(404), (NT(6)+NT(404)));
//G633: option_value(365)    => __E_option_value_184(404).
	p(NT(365), (NT(404)));
//G634: bf_cmd_arg(405)      => history(316).
	p(NT(405), (NT(316)));
//G635: bf_cmd_arg(405)      => bf(30).
	p(NT(405), (NT(30)));
//G636: wff_cmd_arg(307)     => history(316).
	p(NT(307), (NT(316)));
//G637: wff_cmd_arg(307)     => wff(14).
	p(NT(307), (NT(14)));
//G638: nf_cmd_arg(325)      => history(316).
	p(NT(325), (NT(316)));
//G639: nf_cmd_arg(325)      => ref(27).
	p(NT(325), (NT(27)));
//G640: nf_cmd_arg(325)      => wff(14).
	p(NT(325), (NT(14)));
//G641: nf_cmd_arg(325)      => bf(30).
	p(NT(325), (NT(30)));
//G642: onf_cmd_arg(352)     => history(316).
	p(NT(352), (NT(316)));
//G643: onf_cmd_arg(352)     => wff(14).
	p(NT(352), (NT(14)));
//G644: normalize_cmd_arg(295) => history(316).
	p(NT(295), (NT(316)));
//G645: normalize_cmd_arg(295) => spec(10).
	p(NT(295), (NT(10)));
//G646: normalize_cmd_arg(295) => ref(27).
	p(NT(295), (NT(27)));
//G647: normalize_cmd_arg(295) => wff(14).
	p(NT(295), (NT(14)));
//G648: normalize_cmd_arg(295) => bf(30).
	p(NT(295), (NT(30)));
//G649: inst_args(330)       => wff_cmd_arg(307) _(11) '[' _(11) variable(134) _(11) '/' _(11) bf_cmd_arg(405) _(11) ']'.
	p(NT(330), (NT(307)+NT(11)+T(26)+NT(11)+NT(134)+NT(11)+T(43)+NT(11)+NT(405)+NT(11)+T(27)));
//G650: inst_args(330)       => bf_cmd_arg(405) _(11) '[' _(11) variable(134) _(11) '/' _(11) bf_cmd_arg(405) _(11) ']'.
	p(NT(330), (NT(405)+NT(11)+T(26)+NT(11)+NT(134)+NT(11)+T(43)+NT(11)+NT(405)+NT(11)+T(27)));
//G651: help_arg(287)        => help_sym(284).
	p(NT(287), (NT(284)));
//G652: help_arg(287)        => version_sym(277).
	p(NT(287), (NT(277)));
//G653: help_arg(287)        => quit_sym(274).
	p(NT(287), (NT(274)));
//G654: help_arg(287)        => clear_sym(280).
	p(NT(287), (NT(280)));
//G655: help_arg(287)        => get_sym(358).
	p(NT(287), (NT(358)));
//G656: help_arg(287)        => set_sym(363).
	p(NT(287), (NT(363)));
//G657: help_arg(287)        => enable_sym(368).
	p(NT(287), (NT(368)));
//G658: help_arg(287)        => disable_sym(371).
	p(NT(287), (NT(371)));
//G659: help_arg(287)        => toggle_sym(374).
	p(NT(287), (NT(374)));
//G660: help_arg(287)        => file_sym(290).
	p(NT(287), (NT(290)));
//G661: help_arg(287)        => history_sym(386).
	p(NT(287), (NT(386)));
//G662: help_arg(287)        => abs_history_sym(406).
	p(NT(287), (NT(406)));
//G663: help_arg(287)        => rel_history_sym(407).
	p(NT(287), (NT(407)));
//G664: help_arg(287)        => selection_sym(408).
	p(NT(287), (NT(408)));
//G665: help_arg(287)        => def_sym(377).
	p(NT(287), (NT(377)));
//G666: help_arg(287)        => inst_sym(328).
	p(NT(287), (NT(328)));
//G667: help_arg(287)        => subst_sym(323).
	p(NT(287), (NT(323)));
//G668: help_arg(287)        => normalize_sym(319).
	p(NT(287), (NT(319)));
//G669: help_arg(287)        => execute_sym(409).
	p(NT(287), (NT(409)));
//G670: help_arg(287)        => solve_sym(304).
	p(NT(287), (NT(304)));
//G671: help_arg(287)        => valid_sym(294).
	p(NT(287), (NT(294)));
//G672: help_arg(287)        => sat_sym(298).
	p(NT(287), (NT(298)));
//G673: help_arg(287)        => unsat_sym(301).
	p(NT(287), (NT(301)));
//G674: help_arg(287)        => run_sym(313).
	p(NT(287), (NT(313)));
//G675: help_arg(287)        => dnf_sym(333).
	p(NT(287), (NT(333)));
//G676: help_arg(287)        => cnf_sym(336).
	p(NT(287), (NT(336)));
//G677: help_arg(287)        => anf_sym(339).
	p(NT(287), (NT(339)));
//G678: help_arg(287)        => snf_sym(410).
	p(NT(287), (NT(410)));
//G679: help_arg(287)        => nnf_sym(342).
	p(NT(287), (NT(342)));
//G680: help_arg(287)        => pnf_sym(345).
	p(NT(287), (NT(345)));
//G681: help_arg(287)        => mnf_sym(348).
	p(NT(287), (NT(348)));
//G682: help_arg(287)        => onf_sym(351).
	p(NT(287), (NT(351)));
//G683: help_arg(287)        => qelim_sym(355).
	p(NT(287), (NT(355)));
//G684: __E___E_help_arg_185_186(413) => 's'.
	p(NT(413), (T(15)));
//G685: __E___E_help_arg_185_186(413) => null.
	p(NT(413), (nul));
//G686: __E_help_arg_185(412) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_185_186(413).
	p(NT(412), (T(11)+T(29)+T(16)+T(21)+T(56)+T(10)+T(11)+NT(413)));
//G687: examples_sym(411)    => __E_help_arg_185(412).
	p(NT(411), (NT(412)));
//G688: help_arg(287)        => examples_sym(411).
	p(NT(287), (NT(411)));
//G689: __E_history_187(415) => '%'.
	p(NT(415), (T(44)));
//G690: rel_history_sym(407) => '%' '-'.
	p(NT(407), (T(44)+T(30)));
//G691: history_id(417)      => digits(259).
	p(NT(417), (NT(259)));
//G692: __E___E_history_187_188(416) => history_id(417).
	p(NT(416), (NT(417)));
//G693: __E___E_history_187_188(416) => null.
	p(NT(416), (nul));
//G694: __E_history_187(415) => rel_history_sym(407) __E___E_history_187_188(416).
	p(NT(415), (NT(407)+NT(416)));
//G695: rel_history(414)     => __E_history_187(415).
	p(NT(414), (NT(415)));
//G696: history(316)         => rel_history(414).
	p(NT(316), (NT(414)));
//G697: abs_history_sym(406) => '%'.
	p(NT(406), (T(44)));
//G698: __E_history_189(419) => abs_history_sym(406) history_id(417).
	p(NT(419), (NT(406)+NT(417)));
//G699: abs_history(418)     => __E_history_189(419).
	p(NT(418), (NT(419)));
//G700: history(316)         => abs_history(418).
	p(NT(316), (NT(418)));
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
		ref_arg, __E___E_ref_args_13_15, __E___E_ref_args_13_16, tau_constant_source, __E_tau_constant_source_17, wff_parenthesis, __E_wff_18, wff_sometimes, __E_wff_19, __E___E_wff_19_20, 
		wff_always, __E_wff_21, __E___E_wff_21_22, wff_conditional, __E_wff_23, wff_all, __E_wff_24, q_vars, wff_ex, __E_wff_25, 
		wff_ref, constraint, wff_imply, __E_wff_26, wff_rimply, __E_wff_27, wff_equiv, __E_wff_28, wff_or, __E_wff_29, 
		wff_xor, __E_wff_30, wff_and, __E_wff_31, wff_neg, __E_wff_32, wff_t, wff_f, bf_interval, __E_wff_33, 
		bf_eq, __E_wff_34, bf_neq, __E_wff_35, bf_lteq, __E_wff_36, bf_nlteq, __E_wff_37, bf_gt, __E_wff_38, 
		bf_ngt, __E_wff_39, bf_gteq, __E_wff_40, bf_ngteq, __E_wff_41, bf_lt, __E_wff_42, bf_nlt, __E_wff_43, 
		bv_eq, __E_wff_44, bv, bv_neq, __E_wff_45, bv_lteq, __E_wff_46, bv_nlteq, __E_wff_47, bv_gt, 
		__E_wff_48, bv_ngt, __E_wff_49, bv_gteq, __E_wff_50, bv_ngteq, __E_wff_51, bv_lt, __E_wff_52, bv_nlt, 
		__E_wff_53, bf_parenthesis, __E_bf_54, ba_constant, variable, bf_fall, __E_bf_55, bf_fex, __E_bf_56, bf_ref, 
		bf_or, __E_bf_57, bf_xor, __E_bf_58, bf_t, __E_bf_59, __E___E_bf_59_60, bf_f, __E_bf_61, __E___E_bf_61_62, 
		bf_and, __E_bf_63, __E___E_bf_63_64, bf_neg, __E_bf_65, bf_neg_oprnd, __E___E_bf_65_66, bf_and_nosep, __E_bf_67, bf_and_nosep_1st_oprnd, 
		__E___E_bf_67_68, bf_and_nosep_2nd_oprnd, __E___E_bf_67_69, bv_parenthesis, __E_bv_70, bv_checked, __E_bv_71, bv_add, __E_bv_72, bv_mul, 
		__E_bv_73, bv_div, __E_bv_74, bv_mod, __E_bv_75, bv_sub, __E_bv_76, bv_neg, bv_and, __E_bv_77, 
		bv_nand, __E_bv_78, bv_or, __E_bv_79, bv_nor, __E_bv_80, bv_xor, __E_bv_81, bv_xnor, __E_bv_82, 
		bv_shl, __E_bv_83, bv_shr, __E_bv_84, bv_min, __E_bv_85, bv_max, __E_bv_86, bv_constant, binary, 
		__E_bv_constant_87, decimal, __E_bv_constant_88, hexadecimal, __E_bv_constant_89, __E_binary_90, __E_binary_91, __E_hexadecimal_92, __E_decimal_93, ctn_neq, 
		__E_constraint_94, ctnvar, num, ctn_eq, __E_constraint_95, ctn_gteq, __E_constraint_96, ctn_gt, __E_constraint_97, ctn_lteq, 
		__E_constraint_98, ctn_lt, __E_constraint_99, __E_ba_constant_100, source, __E_ba_constant_101, src_c, __E_source_102, __E___E_source_102_103, __E___E_source_102_104, 
		__E_src_c_105, __E_src_c_106, __E_variable_107, uconst, io_var, var_name, __E_variable_108, __E_var_name_109, __E_var_name_110, __E_var_name_111, 
		__E_var_name_112, __E_var_name_113, offset, chars, uconst_name, __E_uconst_114, __E___E_uconst_114_115, q_var, __E_q_vars_116, __E_q_vars_117, 
		__E_offsets_118, __E_offsets_119, integer, shift, __E_offset_120, __E_shift_121, __E___E_shift_121_122, __E_chars_123, __E_chars_124, digits, 
		__E_digits_125, __E_integer_126, type, __E_typed_127, subtype, comment, __E_comment_128, __E_comment_129, __E_comment_130, cli, 
		cli_command, __E_cli_131, __E_cli_132, quit_cmd, quit_sym, __E_cli_command_133, version_cmd, version_sym, __E_cli_command_134, clear_cmd, 
		clear_sym, __E_cli_command_135, help_cmd, __E_cli_command_136, help_sym, __E___E_cli_command_136_137, __E___E_cli_command_136_138, help_arg, file_cmd, __E_cli_command_139, 
		file_sym, q_string, valid_cmd, __E_cli_command_140, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_141, sat_sym, unsat_cmd, 
		__E_cli_command_142, unsat_sym, solve_cmd, __E_cli_command_143, solve_sym, __E___E_cli_command_143_144, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_145, 
		lgrs_sym, run_cmd, __E_cli_command_146, run_sym, __E___E_cli_command_146_147, __E___E_cli_command_146_148, history, normalize_cmd, __E_cli_command_149, normalize_sym, 
		__E___E_cli_command_149_150, subst_cmd, __E_cli_command_151, subst_sym, __E___E_cli_command_151_152, nf_cmd_arg, inst_cmd, __E_cli_command_153, inst_sym, __E___E_cli_command_153_154, 
		inst_args, dnf_cmd, __E_cli_command_155, dnf_sym, cnf_cmd, __E_cli_command_156, cnf_sym, anf_cmd, __E_cli_command_157, anf_sym, 
		nnf_cmd, __E_cli_command_158, nnf_sym, pnf_cmd, __E_cli_command_159, pnf_sym, mnf_cmd, __E_cli_command_160, mnf_sym, onf_cmd, 
		__E_cli_command_161, onf_sym, onf_cmd_arg, qelim_cmd, __E_cli_command_162, qelim_sym, get_cmd, __E_cli_command_163, get_sym, __E___E_cli_command_163_164, 
		option_name, set_cmd, __E_cli_command_165, set_sym, __E___E_cli_command_165_166, option_value, enable_cmd, __E_cli_command_167, enable_sym, disable_cmd, 
		__E_cli_command_168, disable_sym, toggle_cmd, __E_cli_command_169, toggle_sym, def_list_cmd, __E_cli_command_170, def_sym, __E___E_cli_command_170_171, def_print_cmd, 
		__E_cli_command_172, def_rr_cmd, def_input_cmd, def_output_cmd, history_list_cmd, __E_cli_command_173, history_sym, __E___E_cli_command_173_174, history_print_cmd, __E_cli_command_175, 
		history_store_cmd, __E_cli_command_176, __E_solve_options_177, solve_option, __E_solve_options_178, __E_solve_option_179, solver_mode, solver_mode_minimum, __E_solver_mode_180, solver_mode_maximum, 
		__E_solver_mode_181, file_name, __E_file_name_182, __E_option_name_183, __E_option_value_184, bf_cmd_arg, abs_history_sym, rel_history_sym, selection_sym, execute_sym, 
		snf_sym, examples_sym, __E_help_arg_185, __E___E_help_arg_185_186, rel_history, __E_history_187, __E___E_history_187_188, history_id, abs_history, __E_history_189, 
		__N_0, __N_1, __N_2, __N_3, __N_4, __N_5, __N_6, __N_7, __N_8, 
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
