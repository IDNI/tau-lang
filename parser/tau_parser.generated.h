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
	"bv_shl", "__E_bv_83", "bv_shr", "__E_bv_84", "bv_min", "__E_bv_85", "bv_max", "__E_bv_86", "ctn_neq", "__E_constraint_87", 
	"ctnvar", "num", "ctn_eq", "__E_constraint_88", "ctn_gteq", "__E_constraint_89", "ctn_gt", "__E_constraint_90", "ctn_lteq", "__E_constraint_91", 
	"ctn_lt", "__E_constraint_92", "__E_ba_constant_93", "source", "__E_ba_constant_94", "src_c", "__E_source_95", "__E___E_source_95_96", "__E___E_source_95_97", "__E_src_c_98", 
	"__E_src_c_99", "__E_variable_100", "uconst", "io_var", "var_name", "__E_variable_101", "__E_var_name_102", "__E_var_name_103", "__E_var_name_104", "__E_var_name_105", 
	"__E_var_name_106", "offset", "chars", "uconst_name", "__E_uconst_107", "__E___E_uconst_107_108", "q_var", "__E_q_vars_109", "__E_q_vars_110", "__E_offsets_111", 
	"__E_offsets_112", "integer", "shift", "__E_offset_113", "__E_shift_114", "__E___E_shift_114_115", "__E_chars_116", "__E_chars_117", "digits", "__E_digits_118", 
	"__E_integer_119", "type", "__E_typed_120", "subtype", "comment", "__E_comment_121", "__E_comment_122", "__E_comment_123", "cli", "cli_command", 
	"__E_cli_124", "__E_cli_125", "quit_cmd", "quit_sym", "__E_cli_command_126", "version_cmd", "version_sym", "__E_cli_command_127", "clear_cmd", "clear_sym", 
	"__E_cli_command_128", "help_cmd", "__E_cli_command_129", "help_sym", "__E___E_cli_command_129_130", "__E___E_cli_command_129_131", "help_arg", "file_cmd", "__E_cli_command_132", "file_sym", 
	"q_string", "valid_cmd", "__E_cli_command_133", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_134", "sat_sym", "unsat_cmd", "__E_cli_command_135", 
	"unsat_sym", "solve_cmd", "__E_cli_command_136", "solve_sym", "__E___E_cli_command_136_137", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_138", "lgrs_sym", 
	"run_cmd", "__E_cli_command_139", "run_sym", "__E___E_cli_command_139_140", "__E___E_cli_command_139_141", "history", "normalize_cmd", "__E_cli_command_142", "normalize_sym", "__E___E_cli_command_142_143", 
	"subst_cmd", "__E_cli_command_144", "subst_sym", "__E___E_cli_command_144_145", "nf_cmd_arg", "inst_cmd", "__E_cli_command_146", "inst_sym", "__E___E_cli_command_146_147", "inst_args", 
	"dnf_cmd", "__E_cli_command_148", "dnf_sym", "cnf_cmd", "__E_cli_command_149", "cnf_sym", "anf_cmd", "__E_cli_command_150", "anf_sym", "nnf_cmd", 
	"__E_cli_command_151", "nnf_sym", "pnf_cmd", "__E_cli_command_152", "pnf_sym", "mnf_cmd", "__E_cli_command_153", "mnf_sym", "onf_cmd", "__E_cli_command_154", 
	"onf_sym", "onf_cmd_arg", "qelim_cmd", "__E_cli_command_155", "qelim_sym", "get_cmd", "__E_cli_command_156", "get_sym", "__E___E_cli_command_156_157", "option_name", 
	"set_cmd", "__E_cli_command_158", "set_sym", "__E___E_cli_command_158_159", "option_value", "enable_cmd", "__E_cli_command_160", "enable_sym", "disable_cmd", "__E_cli_command_161", 
	"disable_sym", "toggle_cmd", "__E_cli_command_162", "toggle_sym", "def_list_cmd", "__E_cli_command_163", "def_sym", "__E___E_cli_command_163_164", "def_print_cmd", "__E_cli_command_165", 
	"def_rr_cmd", "def_input_cmd", "def_output_cmd", "history_list_cmd", "__E_cli_command_166", "history_sym", "__E___E_cli_command_166_167", "history_print_cmd", "__E_cli_command_168", "history_store_cmd", 
	"__E_cli_command_169", "__E_solve_options_170", "solve_option", "__E_solve_options_171", "__E_solve_option_172", "solver_mode", "solver_mode_minimum", "__E_solver_mode_173", "solver_mode_maximum", "__E_solver_mode_174", 
	"file_name", "__E_file_name_175", "__E_option_name_176", "__E_option_value_177", "bf_cmd_arg", "abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", "snf_sym", 
	"examples_sym", "__E_help_arg_178", "__E___E_help_arg_178_179", "rel_history", "__E_history_180", "__E___E_history_180_181", "history_id", "abs_history", "__E_history_182", "__N_0", 
	"__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', 'a', 'b', 'k', 
	'r', ',', 'm', '<', '>', 'w', 'y', '[', ']', '?', 
	'x', '-', '|', '^', '&', '!', 'T', 'F', '_', '+', 
	'1', '0', '\'', '*', '/', '%', '~', '{', '}', '$', 
	'\t', '\n', '\r', '#', 'q', 'v', 'h', 'p', 'd', 'g', 
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
			29, 31, 40, 200, 213, 224, 233, 241, 248, 251,
			253, 349, 354, 390
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
			{ 215 },
			{ 222 },
			{ 222, 233 },
			{ 232 },
			{ 236 },
			{ 284 },
			{ 296 },
			{ 314 },
			{ 319 },
			{ 341 },
			{ 394 }
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
//G294: bv(112)              => ba_constant(133).
	p(NT(112), (NT(133)));
//G295: __E_constraint_87(199) => '[' ctnvar(200) _(11) '!' '=' _(11) num(201) ']'.
	p(NT(199), (T(26)+NT(200)+NT(11)+T(34)+T(3)+NT(11)+NT(201)+T(27)));
//G296: __E_constraint_87(199) => '[' num(201) _(11) '!' '=' _(11) ctnvar(200) ']'.
	p(NT(199), (T(26)+NT(201)+NT(11)+T(34)+T(3)+NT(11)+NT(200)+T(27)));
//G297: ctn_neq(198)         => __E_constraint_87(199).
	p(NT(198), (NT(199)));
//G298: constraint(71)       => ctn_neq(198).
	p(NT(71), (NT(198)));
//G299: __E_constraint_88(203) => '[' ctnvar(200) _(11) '=' _(11) num(201) ']'.
	p(NT(203), (T(26)+NT(200)+NT(11)+T(3)+NT(11)+NT(201)+T(27)));
//G300: __E_constraint_88(203) => '[' num(201) _(11) '=' _(11) ctnvar(200) ']'.
	p(NT(203), (T(26)+NT(201)+NT(11)+T(3)+NT(11)+NT(200)+T(27)));
//G301: ctn_eq(202)          => __E_constraint_88(203).
	p(NT(202), (NT(203)));
//G302: constraint(71)       => ctn_eq(202).
	p(NT(71), (NT(202)));
//G303: __E_constraint_89(205) => '[' ctnvar(200) _(11) '>' '=' _(11) num(201) ']'.
	p(NT(205), (T(26)+NT(200)+NT(11)+T(23)+T(3)+NT(11)+NT(201)+T(27)));
//G304: __E_constraint_89(205) => '[' num(201) _(11) '>' '=' _(11) ctnvar(200) ']'.
	p(NT(205), (T(26)+NT(201)+NT(11)+T(23)+T(3)+NT(11)+NT(200)+T(27)));
//G305: ctn_gteq(204)        => __E_constraint_89(205).
	p(NT(204), (NT(205)));
//G306: constraint(71)       => ctn_gteq(204).
	p(NT(71), (NT(204)));
//G307: __E_constraint_90(207) => '[' ctnvar(200) _(11) '>' _(11) num(201) ']'.
	p(NT(207), (T(26)+NT(200)+NT(11)+T(23)+NT(11)+NT(201)+T(27)));
//G308: __E_constraint_90(207) => '[' num(201) _(11) '>' _(11) ctnvar(200) ']'.
	p(NT(207), (T(26)+NT(201)+NT(11)+T(23)+NT(11)+NT(200)+T(27)));
//G309: ctn_gt(206)          => __E_constraint_90(207).
	p(NT(206), (NT(207)));
//G310: constraint(71)       => ctn_gt(206).
	p(NT(71), (NT(206)));
//G311: __E_constraint_91(209) => '[' ctnvar(200) _(11) '<' '=' _(11) num(201) ']'.
	p(NT(209), (T(26)+NT(200)+NT(11)+T(22)+T(3)+NT(11)+NT(201)+T(27)));
//G312: __E_constraint_91(209) => '[' num(201) _(11) '<' '=' _(11) ctnvar(200) ']'.
	p(NT(209), (T(26)+NT(201)+NT(11)+T(22)+T(3)+NT(11)+NT(200)+T(27)));
//G313: ctn_lteq(208)        => __E_constraint_91(209).
	p(NT(208), (NT(209)));
//G314: constraint(71)       => ctn_lteq(208).
	p(NT(71), (NT(208)));
//G315: __E_constraint_92(211) => '[' ctnvar(200) _(11) '<' _(11) num(201) ']'.
	p(NT(211), (T(26)+NT(200)+NT(11)+T(22)+NT(11)+NT(201)+T(27)));
//G316: __E_constraint_92(211) => '[' num(201) _(11) '<' _(11) ctnvar(200) ']'.
	p(NT(211), (T(26)+NT(201)+NT(11)+T(22)+NT(11)+NT(200)+T(27)));
//G317: ctn_lt(210)          => __E_constraint_92(211).
	p(NT(210), (NT(211)));
//G318: constraint(71)       => ctn_lt(210).
	p(NT(71), (NT(210)));
//G319: __E_ba_constant_93(212) => capture(29).
	p(NT(212), (NT(29)));
//G320: __E_ba_constant_93(212) => source(213).
	p(NT(212), (NT(213)));
//G321: __E_ba_constant_94(214) => typed(33).
	p(NT(214), (NT(33)));
//G322: __E_ba_constant_94(214) => null.
	p(NT(214), (nul));
//G323: ba_constant(133)     => '{' _(11) __E_ba_constant_93(212) _(11) '}' __E_ba_constant_94(214).
	p(NT(133), (T(46)+NT(11)+NT(212)+NT(11)+T(47)+NT(214)));
//G324: __E___E_source_95_96(217) => src_c(215).
	p(NT(217), (NT(215)));
//G325: __E___E_source_95_96(217) => space(2).
	p(NT(217), (NT(2)));
//G326: __E___E_source_95_97(218) => null.
	p(NT(218), (nul));
//G327: __E___E_source_95_97(218) => __E___E_source_95_96(217) __E___E_source_95_97(218).
	p(NT(218), (NT(217)+NT(218)));
//G328: __E_source_95(216)   => __E___E_source_95_97(218) src_c(215).
	p(NT(216), (NT(218)+NT(215)));
//G329: __E_source_95(216)   => null.
	p(NT(216), (nul));
//G330: source(213)          => src_c(215) __E_source_95(216).
	p(NT(213), (NT(215)+NT(216)));
//G331: src_c(215)           => alnum(6).
	p(NT(215), (NT(6)));
//G332: __N_0(409)           => '{'.
	p(NT(409), (T(46)));
//G333: __N_1(410)           => '}'.
	p(NT(410), (T(47)));
//G334: src_c(215)           => ~( __N_0(409) ) & ~( __N_1(410) ) & punct(7).	 # conjunctive
	p(NT(215), ~(NT(409)) & ~(NT(410)) & (NT(7)));
//G335: __E_src_c_98(219)    => src_c(215).
	p(NT(219), (NT(215)));
//G336: __E_src_c_98(219)    => space(2).
	p(NT(219), (NT(2)));
//G337: __E_src_c_99(220)    => null.
	p(NT(220), (nul));
//G338: __E_src_c_99(220)    => __E_src_c_98(219) __E_src_c_99(220).
	p(NT(220), (NT(219)+NT(220)));
//G339: src_c(215)           => '{' __E_src_c_99(220) '}'.
	p(NT(215), (T(46)+NT(220)+T(47)));
//G340: __E_variable_100(221) => uconst(222).
	p(NT(221), (NT(222)));
//G341: __E_variable_100(221) => io_var(223).
	p(NT(221), (NT(223)));
//G342: __E_variable_100(221) => var_name(224).
	p(NT(221), (NT(224)));
//G343: __E_variable_101(225) => typed(33).
	p(NT(225), (NT(33)));
//G344: __E_variable_101(225) => null.
	p(NT(225), (nul));
//G345: variable(134)        => __E_variable_100(221) __E_variable_101(225).
	p(NT(134), (NT(221)+NT(225)));
//G346: __N_2(411)           => 'F'.
	p(NT(411), (T(36)));
//G347: __N_3(412)           => 'T'.
	p(NT(412), (T(35)));
//G348: __E_var_name_102(226) => ~( __N_2(411) ) & ~( __N_3(412) ) & alpha(5).	 # conjunctive
	p(NT(226), ~(NT(411)) & ~(NT(412)) & (NT(5)));
//G349: __E_var_name_103(227) => null.
	p(NT(227), (nul));
//G350: __E_var_name_103(227) => digit(3) __E_var_name_103(227).
	p(NT(227), (NT(3)+NT(227)));
//G351: var_name(224)        => __E_var_name_102(226) __E_var_name_103(227).	 # guarded: charvar
	p(NT(224), (NT(226)+NT(227)));
	p.back().guard = "charvar";
//G352: __N_4(413)           => 'F'.
	p(NT(413), (T(36)));
//G353: __N_5(414)           => 'T'.
	p(NT(414), (T(35)));
//G354: __E_var_name_104(228) => ~( __N_4(413) ) & ~( __N_5(414) ) & alpha(5).	 # conjunctive
	p(NT(228), ~(NT(413)) & ~(NT(414)) & (NT(5)));
//G355: __E_var_name_105(229) => alnum(6).
	p(NT(229), (NT(6)));
//G356: __E_var_name_105(229) => '_'.
	p(NT(229), (T(37)));
//G357: __E_var_name_106(230) => null.
	p(NT(230), (nul));
//G358: __E_var_name_106(230) => __E_var_name_105(229) __E_var_name_106(230).
	p(NT(230), (NT(229)+NT(230)));
//G359: var_name(224)        => __E_var_name_104(228) __E_var_name_106(230).	 # guarded: var
	p(NT(224), (NT(228)+NT(230)));
	p.back().guard = "var";
//G360: io_var(223)          => io_var_name(31) '[' offset(231) ']'.
	p(NT(223), (NT(31)+T(26)+NT(231)+T(27)));
//G361: io_var_name(31)      => chars(232).
	p(NT(31), (NT(232)));
//G362: __E___E_uconst_107_108(235) => chars(232) _(11).
	p(NT(235), (NT(232)+NT(11)));
//G363: __E___E_uconst_107_108(235) => null.
	p(NT(235), (nul));
//G364: __E_uconst_107(234)  => __E___E_uconst_107_108(235) ':' _(11) chars(232).
	p(NT(234), (NT(235)+T(2)+NT(11)+NT(232)));
//G365: uconst_name(233)     => __E_uconst_107(234).
	p(NT(233), (NT(234)));
//G366: uconst(222)          => '<' _(11) uconst_name(233) _(11) '>'.
	p(NT(222), (T(22)+NT(11)+NT(233)+NT(11)+T(23)));
//G367: __E_q_vars_109(237)  => _(11) ',' _(11) q_var(236).
	p(NT(237), (NT(11)+T(20)+NT(11)+NT(236)));
//G368: __E_q_vars_110(238)  => null.
	p(NT(238), (nul));
//G369: __E_q_vars_110(238)  => __E_q_vars_109(237) __E_q_vars_110(238).
	p(NT(238), (NT(237)+NT(238)));
//G370: q_vars(67)           => q_var(236) __E_q_vars_110(238).
	p(NT(67), (NT(236)+NT(238)));
//G371: q_var(236)           => capture(29).
	p(NT(236), (NT(29)));
//G372: __N_6(415)           => uconst(222).
	p(NT(415), (NT(222)));
//G373: q_var(236)           => variable(134) & ~( __N_6(415) ).	 # conjunctive
	p(NT(236), (NT(134)) & ~(NT(415)));
//G374: ctnvar(200)          => chars(232).
	p(NT(200), (NT(232)));
//G375: __E_offsets_111(239) => _(11) ',' _(11) offset(231).
	p(NT(239), (NT(11)+T(20)+NT(11)+NT(231)));
//G376: __E_offsets_112(240) => null.
	p(NT(240), (nul));
//G377: __E_offsets_112(240) => __E_offsets_111(239) __E_offsets_112(240).
	p(NT(240), (NT(239)+NT(240)));
//G378: offsets(42)          => '[' _(11) offset(231) __E_offsets_112(240) _(11) ']'.
	p(NT(42), (T(26)+NT(11)+NT(231)+NT(240)+NT(11)+T(27)));
//G379: offset(231)          => integer(241).
	p(NT(231), (NT(241)));
//G380: offset(231)          => capture(29).
	p(NT(231), (NT(29)));
//G381: offset(231)          => shift(242).
	p(NT(231), (NT(242)));
//G382: __N_7(416)           => io_var(223).
	p(NT(416), (NT(223)));
//G383: __E_offset_113(243)  => variable(134) & ~( __N_7(416) ).	 # conjunctive
	p(NT(243), (NT(134)) & ~(NT(416)));
//G384: offset(231)          => __E_offset_113(243).
	p(NT(231), (NT(243)));
//G385: __E_shift_114(244)   => capture(29).
	p(NT(244), (NT(29)));
//G386: __N_8(417)           => io_var(223).
	p(NT(417), (NT(223)));
//G387: __E___E_shift_114_115(245) => variable(134) & ~( __N_8(417) ).	 # conjunctive
	p(NT(245), (NT(134)) & ~(NT(417)));
//G388: __E_shift_114(244)   => __E___E_shift_114_115(245).
	p(NT(244), (NT(245)));
//G389: shift(242)           => __E_shift_114(244) _(11) '-' _(11) num(201).
	p(NT(242), (NT(244)+NT(11)+T(30)+NT(11)+NT(201)));
//G390: __E_chars_116(246)   => alnum(6).
	p(NT(246), (NT(6)));
//G391: __E_chars_116(246)   => '_'.
	p(NT(246), (T(37)));
//G392: __E_chars_117(247)   => null.
	p(NT(247), (nul));
//G393: __E_chars_117(247)   => __E_chars_116(246) __E_chars_117(247).
	p(NT(247), (NT(246)+NT(247)));
//G394: chars(232)           => alpha(5) __E_chars_117(247).
	p(NT(232), (NT(5)+NT(247)));
//G395: __E_digits_118(249)  => digit(3).
	p(NT(249), (NT(3)));
//G396: __E_digits_118(249)  => digit(3) __E_digits_118(249).
	p(NT(249), (NT(3)+NT(249)));
//G397: digits(248)          => __E_digits_118(249).
	p(NT(248), (NT(249)));
//G398: num(201)             => digits(248).
	p(NT(201), (NT(248)));
//G399: __E_integer_119(250) => '-'.
	p(NT(250), (T(30)));
//G400: __E_integer_119(250) => null.
	p(NT(250), (nul));
//G401: integer(241)         => __E_integer_119(250) _(11) digits(248).
	p(NT(241), (NT(250)+NT(11)+NT(248)));
//G402: sym(40)              => chars(232).
	p(NT(40), (NT(232)));
//G403: capture(29)          => '$' chars(232).
	p(NT(29), (T(48)+NT(232)));
//G404: __E_typed_120(252)   => _(11) '[' _(11) subtype(253) _(11) ']' _(11).
	p(NT(252), (NT(11)+T(26)+NT(11)+NT(253)+NT(11)+T(27)+NT(11)));
//G405: __E_typed_120(252)   => null.
	p(NT(252), (nul));
//G406: typed(33)            => _(11) ':' _(11) type(251) __E_typed_120(252).
	p(NT(33), (NT(11)+T(2)+NT(11)+NT(251)+NT(252)));
//G407: type(251)            => chars(232).
	p(NT(251), (NT(232)));
//G408: subtype(253)         => num(201).
	p(NT(253), (NT(201)));
//G409: __E_comment_121(255) => printable(8).
	p(NT(255), (NT(8)));
//G410: __E_comment_121(255) => '\t'.
	p(NT(255), (T(49)));
//G411: __E_comment_122(256) => null.
	p(NT(256), (nul));
//G412: __E_comment_122(256) => __E_comment_121(255) __E_comment_122(256).
	p(NT(256), (NT(255)+NT(256)));
//G413: __E_comment_123(257) => '\n'.
	p(NT(257), (T(50)));
//G414: __E_comment_123(257) => '\r'.
	p(NT(257), (T(51)));
//G415: __E_comment_123(257) => eof(1).
	p(NT(257), (NT(1)));
//G416: comment(254)         => '#' __E_comment_122(256) __E_comment_123(257).
	p(NT(254), (T(52)+NT(256)+NT(257)));
//G417: __(34)               => space(2).
	p(NT(34), (NT(2)));
//G418: __(34)               => comment(254).
	p(NT(34), (NT(254)));
//G419: __(34)               => __(34) space(2).
	p(NT(34), (NT(34)+NT(2)));
//G420: __(34)               => __(34) comment(254).
	p(NT(34), (NT(34)+NT(254)));
//G421: _(11)                => __(34).
	p(NT(11), (NT(34)));
//G422: _(11)                => null.
	p(NT(11), (nul));
//G423: cli(258)             => _(11).
	p(NT(258), (NT(11)));
//G424: __E_cli_124(260)     => '.' _(11) cli_command(259) _(11).
	p(NT(260), (T(1)+NT(11)+NT(259)+NT(11)));
//G425: __E_cli_125(261)     => null.
	p(NT(261), (nul));
//G426: __E_cli_125(261)     => __E_cli_124(260) __E_cli_125(261).
	p(NT(261), (NT(260)+NT(261)));
//G427: cli(258)             => _(11) cli_command(259) _(11) __E_cli_125(261).
	p(NT(258), (NT(11)+NT(259)+NT(11)+NT(261)));
//G428: __E_cli_command_126(264) => 'q'.
	p(NT(264), (T(53)));
//G429: __E_cli_command_126(264) => 'q' 'u' 'i' 't'.
	p(NT(264), (T(53)+T(7)+T(4)+T(8)));
//G430: quit_sym(263)        => __E_cli_command_126(264).
	p(NT(263), (NT(264)));
//G431: quit_cmd(262)        => quit_sym(263).
	p(NT(262), (NT(263)));
//G432: cli_command(259)     => quit_cmd(262).
	p(NT(259), (NT(262)));
//G433: __E_cli_command_127(267) => 'v'.
	p(NT(267), (T(54)));
//G434: __E_cli_command_127(267) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(267), (T(54)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G435: version_sym(266)     => __E_cli_command_127(267).
	p(NT(266), (NT(267)));
//G436: version_cmd(265)     => version_sym(266).
	p(NT(265), (NT(266)));
//G437: cli_command(259)     => version_cmd(265).
	p(NT(259), (NT(265)));
//G438: __E_cli_command_128(270) => 'c'.
	p(NT(270), (T(14)));
//G439: __E_cli_command_128(270) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(270), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G440: clear_sym(269)       => __E_cli_command_128(270).
	p(NT(269), (NT(270)));
//G441: clear_cmd(268)       => clear_sym(269).
	p(NT(268), (NT(269)));
//G442: cli_command(259)     => clear_cmd(268).
	p(NT(259), (NT(268)));
//G443: __E___E_cli_command_129_130(274) => 'h'.
	p(NT(274), (T(55)));
//G444: __E___E_cli_command_129_130(274) => 'h' 'e' 'l' 'p'.
	p(NT(274), (T(55)+T(11)+T(10)+T(56)));
//G445: help_sym(273)        => __E___E_cli_command_129_130(274).
	p(NT(273), (NT(274)));
//G446: __E___E_cli_command_129_131(275) => __(34) help_arg(276).
	p(NT(275), (NT(34)+NT(276)));
//G447: __E___E_cli_command_129_131(275) => null.
	p(NT(275), (nul));
//G448: __E_cli_command_129(272) => help_sym(273) __E___E_cli_command_129_131(275).
	p(NT(272), (NT(273)+NT(275)));
//G449: help_cmd(271)        => __E_cli_command_129(272).
	p(NT(271), (NT(272)));
//G450: cli_command(259)     => help_cmd(271).
	p(NT(259), (NT(271)));
//G451: file_sym(279)        => 'f' 'i' 'l' 'e'.
	p(NT(279), (T(9)+T(4)+T(10)+T(11)));
//G452: __E_cli_command_132(278) => file_sym(279) __(34) q_string(280).
	p(NT(278), (NT(279)+NT(34)+NT(280)));
//G453: file_cmd(277)        => __E_cli_command_132(278).
	p(NT(277), (NT(278)));
//G454: cli_command(259)     => file_cmd(277).
	p(NT(259), (NT(277)));
//G455: valid_sym(283)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(283), (T(54)+T(16)+T(10)+T(4)+T(57)));
//G456: __E_cli_command_133(282) => valid_sym(283) __(34) normalize_cmd_arg(284).
	p(NT(282), (NT(283)+NT(34)+NT(284)));
//G457: valid_cmd(281)       => __E_cli_command_133(282).
	p(NT(281), (NT(282)));
//G458: cli_command(259)     => valid_cmd(281).
	p(NT(259), (NT(281)));
//G459: sat_sym(287)         => 's' 'a' 't'.
	p(NT(287), (T(15)+T(16)+T(8)));
//G460: __E_cli_command_134(286) => sat_sym(287) __(34) normalize_cmd_arg(284).
	p(NT(286), (NT(287)+NT(34)+NT(284)));
//G461: sat_cmd(285)         => __E_cli_command_134(286).
	p(NT(285), (NT(286)));
//G462: cli_command(259)     => sat_cmd(285).
	p(NT(259), (NT(285)));
//G463: unsat_sym(290)       => 'u' 'n' 's' 'a' 't'.
	p(NT(290), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G464: __E_cli_command_135(289) => unsat_sym(290) __(34) normalize_cmd_arg(284).
	p(NT(289), (NT(290)+NT(34)+NT(284)));
//G465: unsat_cmd(288)       => __E_cli_command_135(289).
	p(NT(288), (NT(289)));
//G466: cli_command(259)     => unsat_cmd(288).
	p(NT(259), (NT(288)));
//G467: solve_sym(293)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(293), (T(15)+T(6)+T(10)+T(54)+T(11)));
//G468: __E___E_cli_command_136_137(294) => solve_options(295).
	p(NT(294), (NT(295)));
//G469: __E___E_cli_command_136_137(294) => null.
	p(NT(294), (nul));
//G470: __E_cli_command_136(292) => solve_sym(293) __E___E_cli_command_136_137(294) __(34) wff_cmd_arg(296).
	p(NT(292), (NT(293)+NT(294)+NT(34)+NT(296)));
//G471: solve_cmd(291)       => __E_cli_command_136(292).
	p(NT(291), (NT(292)));
//G472: cli_command(259)     => solve_cmd(291).
	p(NT(259), (NT(291)));
//G473: lgrs_sym(299)        => 'l' 'g' 'r' 's'.
	p(NT(299), (T(10)+T(58)+T(19)+T(15)));
//G474: __E_cli_command_138(298) => lgrs_sym(299) __(34) wff_cmd_arg(296).
	p(NT(298), (NT(299)+NT(34)+NT(296)));
//G475: lgrs_cmd(297)        => __E_cli_command_138(298).
	p(NT(297), (NT(298)));
//G476: cli_command(259)     => lgrs_cmd(297).
	p(NT(259), (NT(297)));
//G477: __E___E_cli_command_139_140(303) => 'r'.
	p(NT(303), (T(19)));
//G478: __E___E_cli_command_139_140(303) => 'r' 'u' 'n'.
	p(NT(303), (T(19)+T(7)+T(5)));
//G479: run_sym(302)         => __E___E_cli_command_139_140(303).
	p(NT(302), (NT(303)));
//G480: __E___E_cli_command_139_141(304) => history(305).
	p(NT(304), (NT(305)));
//G481: __E___E_cli_command_139_141(304) => wff(14).
	p(NT(304), (NT(14)));
//G482: __E_cli_command_139(301) => run_sym(302) __(34) __E___E_cli_command_139_141(304).
	p(NT(301), (NT(302)+NT(34)+NT(304)));
//G483: run_cmd(300)         => __E_cli_command_139(301).
	p(NT(300), (NT(301)));
//G484: cli_command(259)     => run_cmd(300).
	p(NT(259), (NT(300)));
//G485: __E___E_cli_command_142_143(309) => 'n'.
	p(NT(309), (T(5)));
//G486: __E___E_cli_command_142_143(309) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(309), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(59)+T(11)));
//G487: normalize_sym(308)   => __E___E_cli_command_142_143(309).
	p(NT(308), (NT(309)));
//G488: __E_cli_command_142(307) => normalize_sym(308) __(34) normalize_cmd_arg(284).
	p(NT(307), (NT(308)+NT(34)+NT(284)));
//G489: normalize_cmd(306)   => __E_cli_command_142(307).
	p(NT(306), (NT(307)));
//G490: cli_command(259)     => normalize_cmd(306).
	p(NT(259), (NT(306)));
//G491: __E___E_cli_command_144_145(313) => 's'.
	p(NT(313), (T(15)));
//G492: __E___E_cli_command_144_145(313) => 's' 'u' 'b' 's' 't'.
	p(NT(313), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G493: __E___E_cli_command_144_145(313) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(313), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G494: subst_sym(312)       => __E___E_cli_command_144_145(313).
	p(NT(312), (NT(313)));
//G495: __E_cli_command_144(311) => subst_sym(312) __(34) nf_cmd_arg(314) _(11) '[' _(11) nf_cmd_arg(314) _(11) '/' _(11) nf_cmd_arg(314) _(11) ']'.
	p(NT(311), (NT(312)+NT(34)+NT(314)+NT(11)+T(26)+NT(11)+NT(314)+NT(11)+T(43)+NT(11)+NT(314)+NT(11)+T(27)));
//G496: subst_cmd(310)       => __E_cli_command_144(311).
	p(NT(310), (NT(311)));
//G497: cli_command(259)     => subst_cmd(310).
	p(NT(259), (NT(310)));
//G498: __E___E_cli_command_146_147(318) => 'i'.
	p(NT(318), (T(4)));
//G499: __E___E_cli_command_146_147(318) => 'i' 'n' 's' 't'.
	p(NT(318), (T(4)+T(5)+T(15)+T(8)));
//G500: __E___E_cli_command_146_147(318) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(318), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G501: inst_sym(317)        => __E___E_cli_command_146_147(318).
	p(NT(317), (NT(318)));
//G502: __E_cli_command_146(316) => inst_sym(317) __(34) inst_args(319).
	p(NT(316), (NT(317)+NT(34)+NT(319)));
//G503: inst_cmd(315)        => __E_cli_command_146(316).
	p(NT(315), (NT(316)));
//G504: cli_command(259)     => inst_cmd(315).
	p(NT(259), (NT(315)));
//G505: dnf_sym(322)         => 'd' 'n' 'f'.
	p(NT(322), (T(57)+T(5)+T(9)));
//G506: __E_cli_command_148(321) => dnf_sym(322) __(34) nf_cmd_arg(314).
	p(NT(321), (NT(322)+NT(34)+NT(314)));
//G507: dnf_cmd(320)         => __E_cli_command_148(321).
	p(NT(320), (NT(321)));
//G508: cli_command(259)     => dnf_cmd(320).
	p(NT(259), (NT(320)));
//G509: cnf_sym(325)         => 'c' 'n' 'f'.
	p(NT(325), (T(14)+T(5)+T(9)));
//G510: __E_cli_command_149(324) => cnf_sym(325) __(34) nf_cmd_arg(314).
	p(NT(324), (NT(325)+NT(34)+NT(314)));
//G511: cnf_cmd(323)         => __E_cli_command_149(324).
	p(NT(323), (NT(324)));
//G512: cli_command(259)     => cnf_cmd(323).
	p(NT(259), (NT(323)));
//G513: anf_sym(328)         => 'a' 'n' 'f'.
	p(NT(328), (T(16)+T(5)+T(9)));
//G514: __E_cli_command_150(327) => anf_sym(328) __(34) nf_cmd_arg(314).
	p(NT(327), (NT(328)+NT(34)+NT(314)));
//G515: anf_cmd(326)         => __E_cli_command_150(327).
	p(NT(326), (NT(327)));
//G516: cli_command(259)     => anf_cmd(326).
	p(NT(259), (NT(326)));
//G517: nnf_sym(331)         => 'n' 'n' 'f'.
	p(NT(331), (T(5)+T(5)+T(9)));
//G518: __E_cli_command_151(330) => nnf_sym(331) __(34) nf_cmd_arg(314).
	p(NT(330), (NT(331)+NT(34)+NT(314)));
//G519: nnf_cmd(329)         => __E_cli_command_151(330).
	p(NT(329), (NT(330)));
//G520: cli_command(259)     => nnf_cmd(329).
	p(NT(259), (NT(329)));
//G521: pnf_sym(334)         => 'p' 'n' 'f'.
	p(NT(334), (T(56)+T(5)+T(9)));
//G522: __E_cli_command_152(333) => pnf_sym(334) __(34) nf_cmd_arg(314).
	p(NT(333), (NT(334)+NT(34)+NT(314)));
//G523: pnf_cmd(332)         => __E_cli_command_152(333).
	p(NT(332), (NT(333)));
//G524: cli_command(259)     => pnf_cmd(332).
	p(NT(259), (NT(332)));
//G525: mnf_sym(337)         => 'm' 'n' 'f'.
	p(NT(337), (T(21)+T(5)+T(9)));
//G526: __E_cli_command_153(336) => mnf_sym(337) __(34) nf_cmd_arg(314).
	p(NT(336), (NT(337)+NT(34)+NT(314)));
//G527: mnf_cmd(335)         => __E_cli_command_153(336).
	p(NT(335), (NT(336)));
//G528: cli_command(259)     => mnf_cmd(335).
	p(NT(259), (NT(335)));
//G529: onf_sym(340)         => 'o' 'n' 'f'.
	p(NT(340), (T(6)+T(5)+T(9)));
//G530: __E_cli_command_154(339) => onf_sym(340) __(34) variable(134) __(34) onf_cmd_arg(341).
	p(NT(339), (NT(340)+NT(34)+NT(134)+NT(34)+NT(341)));
//G531: onf_cmd(338)         => __E_cli_command_154(339).
	p(NT(338), (NT(339)));
//G532: cli_command(259)     => onf_cmd(338).
	p(NT(259), (NT(338)));
//G533: qelim_sym(344)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(344), (T(53)+T(11)+T(10)+T(4)+T(21)));
//G534: __E_cli_command_155(343) => qelim_sym(344) __(34) wff_cmd_arg(296).
	p(NT(343), (NT(344)+NT(34)+NT(296)));
//G535: qelim_cmd(342)       => __E_cli_command_155(343).
	p(NT(342), (NT(343)));
//G536: cli_command(259)     => qelim_cmd(342).
	p(NT(259), (NT(342)));
//G537: get_sym(347)         => 'g' 'e' 't'.
	p(NT(347), (T(58)+T(11)+T(8)));
//G538: __E___E_cli_command_156_157(348) => __(34) option_name(349).
	p(NT(348), (NT(34)+NT(349)));
//G539: __E___E_cli_command_156_157(348) => null.
	p(NT(348), (nul));
//G540: __E_cli_command_156(346) => get_sym(347) __E___E_cli_command_156_157(348).
	p(NT(346), (NT(347)+NT(348)));
//G541: get_cmd(345)         => __E_cli_command_156(346).
	p(NT(345), (NT(346)));
//G542: cli_command(259)     => get_cmd(345).
	p(NT(259), (NT(345)));
//G543: set_sym(352)         => 's' 'e' 't'.
	p(NT(352), (T(15)+T(11)+T(8)));
//G544: __E___E_cli_command_158_159(353) => __(34).
	p(NT(353), (NT(34)));
//G545: __E___E_cli_command_158_159(353) => _(11) '=' _(11).
	p(NT(353), (NT(11)+T(3)+NT(11)));
//G546: __E_cli_command_158(351) => set_sym(352) __(34) option_name(349) __E___E_cli_command_158_159(353) option_value(354).
	p(NT(351), (NT(352)+NT(34)+NT(349)+NT(353)+NT(354)));
//G547: set_cmd(350)         => __E_cli_command_158(351).
	p(NT(350), (NT(351)));
//G548: cli_command(259)     => set_cmd(350).
	p(NT(259), (NT(350)));
//G549: enable_sym(357)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(357), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G550: __E_cli_command_160(356) => enable_sym(357) __(34) option_name(349).
	p(NT(356), (NT(357)+NT(34)+NT(349)));
//G551: enable_cmd(355)      => __E_cli_command_160(356).
	p(NT(355), (NT(356)));
//G552: cli_command(259)     => enable_cmd(355).
	p(NT(259), (NT(355)));
//G553: disable_sym(360)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(360), (T(57)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G554: __E_cli_command_161(359) => disable_sym(360) __(34) option_name(349).
	p(NT(359), (NT(360)+NT(34)+NT(349)));
//G555: disable_cmd(358)     => __E_cli_command_161(359).
	p(NT(358), (NT(359)));
//G556: cli_command(259)     => disable_cmd(358).
	p(NT(259), (NT(358)));
//G557: toggle_sym(363)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(363), (T(8)+T(6)+T(58)+T(58)+T(10)+T(11)));
//G558: __E_cli_command_162(362) => toggle_sym(363) __(34) option_name(349).
	p(NT(362), (NT(363)+NT(34)+NT(349)));
//G559: toggle_cmd(361)      => __E_cli_command_162(362).
	p(NT(361), (NT(362)));
//G560: cli_command(259)     => toggle_cmd(361).
	p(NT(259), (NT(361)));
//G561: __E___E_cli_command_163_164(367) => 'd' 'e' 'f' 's'.
	p(NT(367), (T(57)+T(11)+T(9)+T(15)));
//G562: __E___E_cli_command_163_164(367) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(367), (T(57)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G563: def_sym(366)         => __E___E_cli_command_163_164(367).
	p(NT(366), (NT(367)));
//G564: __E_cli_command_163(365) => def_sym(366).
	p(NT(365), (NT(366)));
//G565: def_list_cmd(364)    => __E_cli_command_163(365).
	p(NT(364), (NT(365)));
//G566: cli_command(259)     => def_list_cmd(364).
	p(NT(259), (NT(364)));
//G567: __E_cli_command_165(369) => def_sym(366) __(34) num(201).
	p(NT(369), (NT(366)+NT(34)+NT(201)));
//G568: def_print_cmd(368)   => __E_cli_command_165(369).
	p(NT(368), (NT(369)));
//G569: cli_command(259)     => def_print_cmd(368).
	p(NT(259), (NT(368)));
//G570: def_rr_cmd(370)      => rec_relation(17).
	p(NT(370), (NT(17)));
//G571: cli_command(259)     => def_rr_cmd(370).
	p(NT(259), (NT(370)));
//G572: def_input_cmd(371)   => input_def(18).
	p(NT(371), (NT(18)));
//G573: cli_command(259)     => def_input_cmd(371).
	p(NT(259), (NT(371)));
//G574: def_output_cmd(372)  => output_def(19).
	p(NT(372), (NT(19)));
//G575: cli_command(259)     => def_output_cmd(372).
	p(NT(259), (NT(372)));
//G576: __E___E_cli_command_166_167(376) => 'h' 'i' 's' 't'.
	p(NT(376), (T(55)+T(4)+T(15)+T(8)));
//G577: __E___E_cli_command_166_167(376) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(376), (T(55)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G578: history_sym(375)     => __E___E_cli_command_166_167(376).
	p(NT(375), (NT(376)));
//G579: __E_cli_command_166(374) => history_sym(375).
	p(NT(374), (NT(375)));
//G580: history_list_cmd(373) => __E_cli_command_166(374).
	p(NT(373), (NT(374)));
//G581: cli_command(259)     => history_list_cmd(373).
	p(NT(259), (NT(373)));
//G582: __E_cli_command_168(378) => history_sym(375) __(34) history(305).
	p(NT(378), (NT(375)+NT(34)+NT(305)));
//G583: history_print_cmd(377) => __E_cli_command_168(378).
	p(NT(377), (NT(378)));
//G584: cli_command(259)     => history_print_cmd(377).
	p(NT(259), (NT(377)));
//G585: __E_cli_command_169(380) => wff(14).
	p(NT(380), (NT(14)));
//G586: __E_cli_command_169(380) => bf(30).
	p(NT(380), (NT(30)));
//G587: history_store_cmd(379) => __E_cli_command_169(380).
	p(NT(379), (NT(380)));
//G588: cli_command(259)     => history_store_cmd(379).
	p(NT(259), (NT(379)));
//G589: __E_solve_options_170(381) => __(34) solve_option(382).
	p(NT(381), (NT(34)+NT(382)));
//G590: __E_solve_options_171(383) => null.
	p(NT(383), (nul));
//G591: __E_solve_options_171(383) => __E_solve_options_170(381) __E_solve_options_171(383).
	p(NT(383), (NT(381)+NT(383)));
//G592: solve_options(295)   => __E_solve_options_171(383).
	p(NT(295), (NT(383)));
//G593: __E_solve_option_172(384) => solver_mode(385).
	p(NT(384), (NT(385)));
//G594: __E_solve_option_172(384) => type(251).
	p(NT(384), (NT(251)));
//G595: solve_option(382)    => '-' '-' __E_solve_option_172(384).
	p(NT(382), (T(30)+T(30)+NT(384)));
//G596: __E_solver_mode_173(387) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(387), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G597: __E_solver_mode_173(387) => 'm' 'i' 'n'.
	p(NT(387), (T(21)+T(4)+T(5)));
//G598: solver_mode_minimum(386) => __E_solver_mode_173(387).
	p(NT(386), (NT(387)));
//G599: solver_mode(385)     => solver_mode_minimum(386).
	p(NT(385), (NT(386)));
//G600: __E_solver_mode_174(389) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(389), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G601: __E_solver_mode_174(389) => 'm' 'a' 'x'.
	p(NT(389), (T(21)+T(16)+T(29)));
//G602: solver_mode_maximum(388) => __E_solver_mode_174(389).
	p(NT(388), (NT(389)));
//G603: solver_mode(385)     => solver_mode_maximum(388).
	p(NT(385), (NT(388)));
//G604: q_file_name(38)      => '"' file_name(390) '"'.
	p(NT(38), (T(60)+NT(390)+T(60)));
//G605: __E_file_name_175(391) => printable(8).
	p(NT(391), (NT(8)));
//G606: __E_file_name_175(391) => printable(8) __E_file_name_175(391).
	p(NT(391), (NT(8)+NT(391)));
//G607: file_name(390)       => __E_file_name_175(391).
	p(NT(390), (NT(391)));
//G608: __E_option_name_176(392) => alnum(6).
	p(NT(392), (NT(6)));
//G609: __E_option_name_176(392) => alnum(6) __E_option_name_176(392).
	p(NT(392), (NT(6)+NT(392)));
//G610: option_name(349)     => __E_option_name_176(392).
	p(NT(349), (NT(392)));
//G611: __E_option_value_177(393) => alnum(6).
	p(NT(393), (NT(6)));
//G612: __E_option_value_177(393) => alnum(6) __E_option_value_177(393).
	p(NT(393), (NT(6)+NT(393)));
//G613: option_value(354)    => __E_option_value_177(393).
	p(NT(354), (NT(393)));
//G614: bf_cmd_arg(394)      => history(305).
	p(NT(394), (NT(305)));
//G615: bf_cmd_arg(394)      => bf(30).
	p(NT(394), (NT(30)));
//G616: wff_cmd_arg(296)     => history(305).
	p(NT(296), (NT(305)));
//G617: wff_cmd_arg(296)     => wff(14).
	p(NT(296), (NT(14)));
//G618: nf_cmd_arg(314)      => history(305).
	p(NT(314), (NT(305)));
//G619: nf_cmd_arg(314)      => ref(27).
	p(NT(314), (NT(27)));
//G620: nf_cmd_arg(314)      => wff(14).
	p(NT(314), (NT(14)));
//G621: nf_cmd_arg(314)      => bf(30).
	p(NT(314), (NT(30)));
//G622: onf_cmd_arg(341)     => history(305).
	p(NT(341), (NT(305)));
//G623: onf_cmd_arg(341)     => wff(14).
	p(NT(341), (NT(14)));
//G624: normalize_cmd_arg(284) => history(305).
	p(NT(284), (NT(305)));
//G625: normalize_cmd_arg(284) => spec(10).
	p(NT(284), (NT(10)));
//G626: normalize_cmd_arg(284) => ref(27).
	p(NT(284), (NT(27)));
//G627: normalize_cmd_arg(284) => wff(14).
	p(NT(284), (NT(14)));
//G628: normalize_cmd_arg(284) => bf(30).
	p(NT(284), (NT(30)));
//G629: inst_args(319)       => wff_cmd_arg(296) _(11) '[' _(11) variable(134) _(11) '/' _(11) bf_cmd_arg(394) _(11) ']'.
	p(NT(319), (NT(296)+NT(11)+T(26)+NT(11)+NT(134)+NT(11)+T(43)+NT(11)+NT(394)+NT(11)+T(27)));
//G630: inst_args(319)       => bf_cmd_arg(394) _(11) '[' _(11) variable(134) _(11) '/' _(11) bf_cmd_arg(394) _(11) ']'.
	p(NT(319), (NT(394)+NT(11)+T(26)+NT(11)+NT(134)+NT(11)+T(43)+NT(11)+NT(394)+NT(11)+T(27)));
//G631: help_arg(276)        => help_sym(273).
	p(NT(276), (NT(273)));
//G632: help_arg(276)        => version_sym(266).
	p(NT(276), (NT(266)));
//G633: help_arg(276)        => quit_sym(263).
	p(NT(276), (NT(263)));
//G634: help_arg(276)        => clear_sym(269).
	p(NT(276), (NT(269)));
//G635: help_arg(276)        => get_sym(347).
	p(NT(276), (NT(347)));
//G636: help_arg(276)        => set_sym(352).
	p(NT(276), (NT(352)));
//G637: help_arg(276)        => enable_sym(357).
	p(NT(276), (NT(357)));
//G638: help_arg(276)        => disable_sym(360).
	p(NT(276), (NT(360)));
//G639: help_arg(276)        => toggle_sym(363).
	p(NT(276), (NT(363)));
//G640: help_arg(276)        => file_sym(279).
	p(NT(276), (NT(279)));
//G641: help_arg(276)        => history_sym(375).
	p(NT(276), (NT(375)));
//G642: help_arg(276)        => abs_history_sym(395).
	p(NT(276), (NT(395)));
//G643: help_arg(276)        => rel_history_sym(396).
	p(NT(276), (NT(396)));
//G644: help_arg(276)        => selection_sym(397).
	p(NT(276), (NT(397)));
//G645: help_arg(276)        => def_sym(366).
	p(NT(276), (NT(366)));
//G646: help_arg(276)        => inst_sym(317).
	p(NT(276), (NT(317)));
//G647: help_arg(276)        => subst_sym(312).
	p(NT(276), (NT(312)));
//G648: help_arg(276)        => normalize_sym(308).
	p(NT(276), (NT(308)));
//G649: help_arg(276)        => execute_sym(398).
	p(NT(276), (NT(398)));
//G650: help_arg(276)        => solve_sym(293).
	p(NT(276), (NT(293)));
//G651: help_arg(276)        => valid_sym(283).
	p(NT(276), (NT(283)));
//G652: help_arg(276)        => sat_sym(287).
	p(NT(276), (NT(287)));
//G653: help_arg(276)        => unsat_sym(290).
	p(NT(276), (NT(290)));
//G654: help_arg(276)        => run_sym(302).
	p(NT(276), (NT(302)));
//G655: help_arg(276)        => dnf_sym(322).
	p(NT(276), (NT(322)));
//G656: help_arg(276)        => cnf_sym(325).
	p(NT(276), (NT(325)));
//G657: help_arg(276)        => anf_sym(328).
	p(NT(276), (NT(328)));
//G658: help_arg(276)        => snf_sym(399).
	p(NT(276), (NT(399)));
//G659: help_arg(276)        => nnf_sym(331).
	p(NT(276), (NT(331)));
//G660: help_arg(276)        => pnf_sym(334).
	p(NT(276), (NT(334)));
//G661: help_arg(276)        => mnf_sym(337).
	p(NT(276), (NT(337)));
//G662: help_arg(276)        => onf_sym(340).
	p(NT(276), (NT(340)));
//G663: help_arg(276)        => qelim_sym(344).
	p(NT(276), (NT(344)));
//G664: __E___E_help_arg_178_179(402) => 's'.
	p(NT(402), (T(15)));
//G665: __E___E_help_arg_178_179(402) => null.
	p(NT(402), (nul));
//G666: __E_help_arg_178(401) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_178_179(402).
	p(NT(401), (T(11)+T(29)+T(16)+T(21)+T(56)+T(10)+T(11)+NT(402)));
//G667: examples_sym(400)    => __E_help_arg_178(401).
	p(NT(400), (NT(401)));
//G668: help_arg(276)        => examples_sym(400).
	p(NT(276), (NT(400)));
//G669: __E_history_180(404) => '%'.
	p(NT(404), (T(44)));
//G670: rel_history_sym(396) => '%' '-'.
	p(NT(396), (T(44)+T(30)));
//G671: history_id(406)      => digits(248).
	p(NT(406), (NT(248)));
//G672: __E___E_history_180_181(405) => history_id(406).
	p(NT(405), (NT(406)));
//G673: __E___E_history_180_181(405) => null.
	p(NT(405), (nul));
//G674: __E_history_180(404) => rel_history_sym(396) __E___E_history_180_181(405).
	p(NT(404), (NT(396)+NT(405)));
//G675: rel_history(403)     => __E_history_180(404).
	p(NT(403), (NT(404)));
//G676: history(305)         => rel_history(403).
	p(NT(305), (NT(403)));
//G677: abs_history_sym(395) => '%'.
	p(NT(395), (T(44)));
//G678: __E_history_182(408) => abs_history_sym(395) history_id(406).
	p(NT(408), (NT(395)+NT(406)));
//G679: abs_history(407)     => __E_history_182(408).
	p(NT(407), (NT(408)));
//G680: history(305)         => abs_history(407).
	p(NT(305), (NT(407)));
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
		bv_shl, __E_bv_83, bv_shr, __E_bv_84, bv_min, __E_bv_85, bv_max, __E_bv_86, ctn_neq, __E_constraint_87, 
		ctnvar, num, ctn_eq, __E_constraint_88, ctn_gteq, __E_constraint_89, ctn_gt, __E_constraint_90, ctn_lteq, __E_constraint_91, 
		ctn_lt, __E_constraint_92, __E_ba_constant_93, source, __E_ba_constant_94, src_c, __E_source_95, __E___E_source_95_96, __E___E_source_95_97, __E_src_c_98, 
		__E_src_c_99, __E_variable_100, uconst, io_var, var_name, __E_variable_101, __E_var_name_102, __E_var_name_103, __E_var_name_104, __E_var_name_105, 
		__E_var_name_106, offset, chars, uconst_name, __E_uconst_107, __E___E_uconst_107_108, q_var, __E_q_vars_109, __E_q_vars_110, __E_offsets_111, 
		__E_offsets_112, integer, shift, __E_offset_113, __E_shift_114, __E___E_shift_114_115, __E_chars_116, __E_chars_117, digits, __E_digits_118, 
		__E_integer_119, type, __E_typed_120, subtype, comment, __E_comment_121, __E_comment_122, __E_comment_123, cli, cli_command, 
		__E_cli_124, __E_cli_125, quit_cmd, quit_sym, __E_cli_command_126, version_cmd, version_sym, __E_cli_command_127, clear_cmd, clear_sym, 
		__E_cli_command_128, help_cmd, __E_cli_command_129, help_sym, __E___E_cli_command_129_130, __E___E_cli_command_129_131, help_arg, file_cmd, __E_cli_command_132, file_sym, 
		q_string, valid_cmd, __E_cli_command_133, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_134, sat_sym, unsat_cmd, __E_cli_command_135, 
		unsat_sym, solve_cmd, __E_cli_command_136, solve_sym, __E___E_cli_command_136_137, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_138, lgrs_sym, 
		run_cmd, __E_cli_command_139, run_sym, __E___E_cli_command_139_140, __E___E_cli_command_139_141, history, normalize_cmd, __E_cli_command_142, normalize_sym, __E___E_cli_command_142_143, 
		subst_cmd, __E_cli_command_144, subst_sym, __E___E_cli_command_144_145, nf_cmd_arg, inst_cmd, __E_cli_command_146, inst_sym, __E___E_cli_command_146_147, inst_args, 
		dnf_cmd, __E_cli_command_148, dnf_sym, cnf_cmd, __E_cli_command_149, cnf_sym, anf_cmd, __E_cli_command_150, anf_sym, nnf_cmd, 
		__E_cli_command_151, nnf_sym, pnf_cmd, __E_cli_command_152, pnf_sym, mnf_cmd, __E_cli_command_153, mnf_sym, onf_cmd, __E_cli_command_154, 
		onf_sym, onf_cmd_arg, qelim_cmd, __E_cli_command_155, qelim_sym, get_cmd, __E_cli_command_156, get_sym, __E___E_cli_command_156_157, option_name, 
		set_cmd, __E_cli_command_158, set_sym, __E___E_cli_command_158_159, option_value, enable_cmd, __E_cli_command_160, enable_sym, disable_cmd, __E_cli_command_161, 
		disable_sym, toggle_cmd, __E_cli_command_162, toggle_sym, def_list_cmd, __E_cli_command_163, def_sym, __E___E_cli_command_163_164, def_print_cmd, __E_cli_command_165, 
		def_rr_cmd, def_input_cmd, def_output_cmd, history_list_cmd, __E_cli_command_166, history_sym, __E___E_cli_command_166_167, history_print_cmd, __E_cli_command_168, history_store_cmd, 
		__E_cli_command_169, __E_solve_options_170, solve_option, __E_solve_options_171, __E_solve_option_172, solver_mode, solver_mode_minimum, __E_solver_mode_173, solver_mode_maximum, __E_solver_mode_174, 
		file_name, __E_file_name_175, __E_option_name_176, __E_option_value_177, bf_cmd_arg, abs_history_sym, rel_history_sym, selection_sym, execute_sym, snf_sym, 
		examples_sym, __E_help_arg_178, __E___E_help_arg_178_179, rel_history, __E_history_180, __E___E_history_180_181, history_id, abs_history, __E_history_182, __N_0, 
		__N_1, __N_2, __N_3, __N_4, __N_5, __N_6, __N_7, __N_8, 
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
