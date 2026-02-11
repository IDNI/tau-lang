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
	"spec", "__E_spec_0", "definitions", "_", "main", "wff", "__E_definitions_1", "__E___E_definitions_1_2", "rec_relation", "input_def", 
	"output_def", "__E_definitions_3", "spec_multiline", "__E_spec_multiline_4", "spec_part", "__E_spec_multiline_5", "__E_spec_multiline_6", "ref", "__E_rec_relation_7", "capture", 
	"bf", "io_var_name", "__E_input_def_8", "typed", "__", "stream", "__E_output_def_9", "q_file_name", "console_sym", "sym", 
	"__E_ref_10", "offsets", "ref_args", "__E_ref_11", "__E_ref_12", "fp_fallback", "first_sym", "last_sym", "__E_ref_args_13", "__E___E_ref_args_13_14", 
	"ref_arg", "__E___E_ref_args_13_15", "__E___E_ref_args_13_16", "wff_parenthesis", "__E_wff_17", "wff_sometimes", "__E_wff_18", "__E___E_wff_18_19", "wff_always", "__E_wff_20", 
	"__E___E_wff_20_21", "wff_conditional", "__E_wff_22", "wff_all", "__E_wff_23", "q_vars", "wff_ex", "__E_wff_24", "wff_ref", "constraint", 
	"wff_imply", "__E_wff_25", "wff_rimply", "__E_wff_26", "wff_equiv", "__E_wff_27", "wff_or", "__E_wff_28", "wff_xor", "__E_wff_29", 
	"wff_and", "__E_wff_30", "wff_neg", "__E_wff_31", "wff_t", "wff_f", "bf_interval", "__E_wff_32", "bf_eq", "__E_wff_33", 
	"bf_neq", "__E_wff_34", "bf_lt", "__E_wff_35", "bf_nlt", "__E_wff_36", "bf_lteq", "__E_wff_37", "bf_nlteq", "__E_wff_38", 
	"bf_gt", "__E_wff_39", "bf_ngt", "__E_wff_40", "bf_gteq", "__E_wff_41", "bf_ngteq", "__E_wff_42", "bf_parenthesis", "__E_bf_43", 
	"ba_constant", "variable", "bf_fall", "__E_bf_44", "bf_fex", "__E_bf_45", "bf_ref", "bf_shr", "__E_bf_46", "__E___E_bf_46_47", 
	"bf_shl", "__E_bf_48", "__E___E_bf_48_49", "bf_add", "__E_bf_50", "bf_sub", "__E_bf_51", "__E___E_bf_51_52", "bf_mod", "__E_bf_53", 
	"__E___E_bf_53_54", "bf_mul", "__E_bf_55", "bf_div", "__E_bf_56", "__E___E_bf_56_57", "bf_nor", "__E_bf_58", "__E___E_bf_58_59", "bf_xnor", 
	"__E_bf_60", "__E___E_bf_60_61", "bf_nand", "__E_bf_62", "__E___E_bf_62_63", "bf_or", "__E_bf_64", "bf_xor", "__E_bf_65", "bf_t", 
	"__E_bf_66", "__E___E_bf_66_67", "bf_f", "__E_bf_68", "__E___E_bf_68_69", "bf_and", "__E_bf_70", "__E___E_bf_70_71", "bf_neg", "__E_bf_72", 
	"bf_neg_oprnd", "__E___E_bf_72_73", "bf_and_nosep", "__E_bf_74", "bf_and_nosep_1st_oprnd", "__E___E_bf_74_75", "bf_and_nosep_2nd_oprnd", "__E___E_bf_74_76", "ctn_neq", "__E_constraint_77", 
	"ctnvar", "num", "ctn_eq", "__E_constraint_78", "ctn_gteq", "__E_constraint_79", "ctn_gt", "__E_constraint_80", "ctn_lteq", "__E_constraint_81", 
	"ctn_lt", "__E_constraint_82", "__E_ba_constant_83", "source", "__E_ba_constant_84", "src_c", "__E_source_85", "__E___E_source_85_86", "__E___E_source_85_87", "__E_src_c_88", 
	"__E_src_c_89", "__E_variable_90", "uconst", "io_var", "var_name", "__E_variable_91", "__E_var_name_92", "__E_var_name_93", "__E_var_name_94", "__E_var_name_95", 
	"__E_var_name_96", "offset", "chars", "uconst_name", "__E_uconst_97", "__E___E_uconst_97_98", "q_var", "__E_q_vars_99", "__E_q_vars_100", "__E_offsets_101", 
	"__E_offsets_102", "integer", "shift", "__E_offset_103", "__E_shift_104", "__E___E_shift_104_105", "__E_chars_106", "__E_chars_107", "digits", "__E_digits_108", 
	"__E_integer_109", "type", "__E_typed_110", "subtype", "comment", "__E_comment_111", "__E_comment_112", "__E_comment_113", "cli", "cli_command", 
	"__E_cli_114", "__E_cli_115", "quit_cmd", "quit_sym", "__E_cli_command_116", "version_cmd", "version_sym", "__E_cli_command_117", "clear_cmd", "clear_sym", 
	"__E_cli_command_118", "help_cmd", "__E_cli_command_119", "help_sym", "__E___E_cli_command_119_120", "__E___E_cli_command_119_121", "help_arg", "file_cmd", "__E_cli_command_122", "file_sym", 
	"q_string", "valid_cmd", "__E_cli_command_123", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_124", "sat_sym", "unsat_cmd", "__E_cli_command_125", 
	"unsat_sym", "solve_cmd", "__E_cli_command_126", "solve_sym", "__E___E_cli_command_126_127", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_128", "lgrs_sym", 
	"run_cmd", "__E_cli_command_129", "run_sym", "__E___E_cli_command_129_130", "__E___E_cli_command_129_131", "history", "normalize_cmd", "__E_cli_command_132", "normalize_sym", "__E___E_cli_command_132_133", 
	"subst_cmd", "__E_cli_command_134", "subst_sym", "__E___E_cli_command_134_135", "nf_cmd_arg", "inst_cmd", "__E_cli_command_136", "inst_sym", "__E___E_cli_command_136_137", "inst_args", 
	"dnf_cmd", "__E_cli_command_138", "dnf_sym", "cnf_cmd", "__E_cli_command_139", "cnf_sym", "anf_cmd", "__E_cli_command_140", "anf_sym", "nnf_cmd", 
	"__E_cli_command_141", "nnf_sym", "pnf_cmd", "__E_cli_command_142", "pnf_sym", "mnf_cmd", "__E_cli_command_143", "mnf_sym", "onf_cmd", "__E_cli_command_144", 
	"onf_sym", "onf_cmd_arg", "qelim_cmd", "__E_cli_command_145", "qelim_sym", "get_cmd", "__E_cli_command_146", "get_sym", "__E___E_cli_command_146_147", "option_name", 
	"set_cmd", "__E_cli_command_148", "set_sym", "__E___E_cli_command_148_149", "option_value", "enable_cmd", "__E_cli_command_150", "enable_sym", "disable_cmd", "__E_cli_command_151", 
	"disable_sym", "toggle_cmd", "__E_cli_command_152", "toggle_sym", "def_list_cmd", "__E_cli_command_153", "def_sym", "__E___E_cli_command_153_154", "def_print_cmd", "__E_cli_command_155", 
	"def_rr_cmd", "def_input_cmd", "def_output_cmd", "history_list_cmd", "__E_cli_command_156", "history_sym", "__E___E_cli_command_156_157", "history_print_cmd", "__E_cli_command_158", "history_store_cmd", 
	"__E_cli_command_159", "__E_solve_options_160", "solve_option", "__E_solve_options_161", "__E_solve_option_162", "solver_mode", "solver_mode_minimum", "__E_solver_mode_163", "solver_mode_maximum", "__E_solver_mode_164", 
	"file_name", "__E_file_name_165", "__E_option_name_166", "__E_option_value_167", "bf_cmd_arg", "abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", "snf_sym", 
	"examples_sym", "__E_help_arg_168", "__E___E_help_arg_168_169", "rel_history", "__E_history_170", "__E___E_history_170_171", "history_id", "abs_history", "__E_history_172", "unreachable", 
	"processed", "start_get_stream_def", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", "__N_7", 
	"__N_8", "__N_9", "__N_10", "__N_11", "__N_12", "__N_13", "__N_14", "__N_15", "__N_16", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', 'a', 'b', 'k', 
	'r', ',', 'm', '<', '>', 'w', 'y', '[', ']', '?', 
	'x', '-', '|', '^', '&', '!', 'T', 'F', '+', '%', 
	'*', '/', '1', '0', '\'', '{', '}', '_', '$', '\t', 
	'\n', '\r', '#', 'q', 'v', 'h', 'p', 'd', 'g', 'z', 
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
			13, 34
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			29, 31, 39, 170, 183, 194, 203, 211, 218, 221,
			223, 319, 324, 360
		},
		.to_inline = {
			{ 15, 53, 15 },
			{ 24 },
			{ 30, 108, 30 },
			{ 160, 108, 30 },
			{ 162, 108, 155 },
			{ 164, 108, 30 },
			{ 166, 108, 30 },
			{ 185 },
			{ 192 },
			{ 192, 203 },
			{ 202 },
			{ 206 },
			{ 254 },
			{ 266 },
			{ 284 },
			{ 289 },
			{ 311 },
			{ 364 }
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
//G0:   start(9)             => spec(10).
	p(NT(9), (NT(10)));
//G1:   __E_spec_0(11)       => definitions(12).
	p(NT(11), (NT(12)));
//G2:   __E_spec_0(11)       => null.
	p(NT(11), (nul));
//G3:   main(14)             => wff(15).
	p(NT(14), (NT(15)));
//G4:   spec(10)             => __E_spec_0(11) _(13) main(14) _(13) '.' _(13).
	p(NT(10), (NT(11)+NT(13)+NT(14)+NT(13)+T(1)+NT(13)));
//G5:   __E___E_definitions_1_2(17) => rec_relation(18).
	p(NT(17), (NT(18)));
//G6:   __E___E_definitions_1_2(17) => input_def(19).
	p(NT(17), (NT(19)));
//G7:   __E___E_definitions_1_2(17) => output_def(20).
	p(NT(17), (NT(20)));
//G8:   __E_definitions_1(16) => _(13) __E___E_definitions_1_2(17) _(13) '.'.
	p(NT(16), (NT(13)+NT(17)+NT(13)+T(1)));
//G9:   __E_definitions_3(21) => __E_definitions_1(16).
	p(NT(21), (NT(16)));
//G10:  __E_definitions_3(21) => __E_definitions_1(16) __E_definitions_3(21).
	p(NT(21), (NT(16)+NT(21)));
//G11:  definitions(12)      => __E_definitions_3(21).
	p(NT(12), (NT(21)));
//G12:  __E_spec_multiline_4(23) => _(13) spec_part(24) _(13) '.'.
	p(NT(23), (NT(13)+NT(24)+NT(13)+T(1)));
//G13:  __E_spec_multiline_5(25) => null.
	p(NT(25), (nul));
//G14:  __E_spec_multiline_5(25) => __E_spec_multiline_4(23) __E_spec_multiline_5(25).
	p(NT(25), (NT(23)+NT(25)));
//G15:  __E_spec_multiline_6(26) => _(13) '.'.
	p(NT(26), (NT(13)+T(1)));
//G16:  __E_spec_multiline_6(26) => null.
	p(NT(26), (nul));
//G17:  spec_multiline(22)   => __E_spec_multiline_5(25) _(13) spec_part(24) __E_spec_multiline_6(26) _(13).
	p(NT(22), (NT(25)+NT(13)+NT(24)+NT(26)+NT(13)));
//G18:  spec_multiline(22)   => _(13).
	p(NT(22), (NT(13)));
//G19:  spec_part(24)        => rec_relation(18).
	p(NT(24), (NT(18)));
//G20:  spec_part(24)        => input_def(19).
	p(NT(24), (NT(19)));
//G21:  spec_part(24)        => output_def(20).
	p(NT(24), (NT(20)));
//G22:  spec_part(24)        => main(14).
	p(NT(24), (NT(14)));
//G23:  __E_rec_relation_7(28) => capture(29).
	p(NT(28), (NT(29)));
//G24:  __E_rec_relation_7(28) => ref(27).
	p(NT(28), (NT(27)));
//G25:  __E_rec_relation_7(28) => wff(15).
	p(NT(28), (NT(15)));
//G26:  __E_rec_relation_7(28) => bf(30).
	p(NT(28), (NT(30)));
//G27:  rec_relation(18)     => ref(27) _(13) ':' '=' _(13) __E_rec_relation_7(28).
	p(NT(18), (NT(27)+NT(13)+T(2)+T(3)+NT(13)+NT(28)));
//G28:  __E_input_def_8(32)  => typed(33).
	p(NT(32), (NT(33)));
//G29:  __E_input_def_8(32)  => null.
	p(NT(32), (nul));
//G30:  input_def(19)        => io_var_name(31) __E_input_def_8(32) _(13) ':' '=' _(13) 'i' 'n' __(34) stream(35).
	p(NT(19), (NT(31)+NT(32)+NT(13)+T(2)+T(3)+NT(13)+T(4)+T(5)+NT(34)+NT(35)));
//G31:  __E_output_def_9(36) => typed(33).
	p(NT(36), (NT(33)));
//G32:  __E_output_def_9(36) => null.
	p(NT(36), (nul));
//G33:  output_def(20)       => io_var_name(31) __E_output_def_9(36) _(13) ':' '=' _(13) 'o' 'u' 't' __(34) stream(35).
	p(NT(20), (NT(31)+NT(36)+NT(13)+T(2)+T(3)+NT(13)+T(6)+T(7)+T(8)+NT(34)+NT(35)));
//G34:  stream(35)           => 'f' 'i' 'l' 'e' _(13) '(' _(13) q_file_name(37) _(13) ')'.
	p(NT(35), (T(9)+T(4)+T(10)+T(11)+NT(13)+T(12)+NT(13)+NT(37)+NT(13)+T(13)));
//G35:  console_sym(38)      => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(38), (T(14)+T(6)+T(5)+T(15)+T(6)+T(10)+T(11)));
//G36:  stream(35)           => console_sym(38).
	p(NT(35), (NT(38)));
//G37:  __E_ref_10(40)       => offsets(41).
	p(NT(40), (NT(41)));
//G38:  __E_ref_10(40)       => null.
	p(NT(40), (nul));
//G39:  __E_ref_11(43)       => typed(33).
	p(NT(43), (NT(33)));
//G40:  __E_ref_11(43)       => null.
	p(NT(43), (nul));
//G41:  __E_ref_12(44)       => _(13) 'f' 'a' 'l' 'l' 'b' 'a' 'c' 'k' __(34) fp_fallback(45).
	p(NT(44), (NT(13)+T(9)+T(16)+T(10)+T(10)+T(17)+T(16)+T(14)+T(18)+NT(34)+NT(45)));
//G42:  __E_ref_12(44)       => null.
	p(NT(44), (nul));
//G43:  ref(27)              => sym(39) __E_ref_10(40) ref_args(42) __E_ref_11(43) __E_ref_12(44).
	p(NT(27), (NT(39)+NT(40)+NT(42)+NT(43)+NT(44)));
//G44:  first_sym(46)        => 'f' 'i' 'r' 's' 't'.
	p(NT(46), (T(9)+T(4)+T(19)+T(15)+T(8)));
//G45:  fp_fallback(45)      => first_sym(46).
	p(NT(45), (NT(46)));
//G46:  last_sym(47)         => 'l' 'a' 's' 't'.
	p(NT(47), (T(10)+T(16)+T(15)+T(8)));
//G47:  fp_fallback(45)      => last_sym(47).
	p(NT(45), (NT(47)));
//G48:  fp_fallback(45)      => capture(29).
	p(NT(45), (NT(29)));
//G49:  fp_fallback(45)      => ref(27).
	p(NT(45), (NT(27)));
//G50:  fp_fallback(45)      => wff(15).
	p(NT(45), (NT(15)));
//G51:  fp_fallback(45)      => bf(30).
	p(NT(45), (NT(30)));
//G52:  ref_arg(50)          => bf(30).
	p(NT(50), (NT(30)));
//G53:  __E___E_ref_args_13_14(49) => _(13) ref_arg(50).
	p(NT(49), (NT(13)+NT(50)));
//G54:  __E___E_ref_args_13_15(51) => _(13) ',' _(13) ref_arg(50).
	p(NT(51), (NT(13)+T(20)+NT(13)+NT(50)));
//G55:  __E___E_ref_args_13_16(52) => null.
	p(NT(52), (nul));
//G56:  __E___E_ref_args_13_16(52) => __E___E_ref_args_13_15(51) __E___E_ref_args_13_16(52).
	p(NT(52), (NT(51)+NT(52)));
//G57:  __E_ref_args_13(48)  => __E___E_ref_args_13_14(49) __E___E_ref_args_13_16(52).
	p(NT(48), (NT(49)+NT(52)));
//G58:  __E_ref_args_13(48)  => null.
	p(NT(48), (nul));
//G59:  ref_args(42)         => '(' __E_ref_args_13(48) _(13) ')'.
	p(NT(42), (T(12)+NT(48)+NT(13)+T(13)));
//G60:  __E_wff_17(54)       => '(' _(13) wff(15) _(13) ')'.
	p(NT(54), (T(12)+NT(13)+NT(15)+NT(13)+T(13)));
//G61:  wff_parenthesis(53)  => __E_wff_17(54).
	p(NT(53), (NT(54)));
//G62:  wff(15)              => wff_parenthesis(53).
	p(NT(15), (NT(53)));
//G63:  __E___E_wff_18_19(57) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(57), (T(15)+T(6)+T(21)+T(11)+T(8)+T(4)+T(21)+T(11)+T(15)));
//G64:  __E___E_wff_18_19(57) => '<' '>'.
	p(NT(57), (T(22)+T(23)));
//G65:  __E_wff_18(56)       => __E___E_wff_18_19(57) _(13) wff(15).
	p(NT(56), (NT(57)+NT(13)+NT(15)));
//G66:  wff_sometimes(55)    => __E_wff_18(56).
	p(NT(55), (NT(56)));
//G67:  wff(15)              => wff_sometimes(55).
	p(NT(15), (NT(55)));
//G68:  __E___E_wff_20_21(60) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(60), (T(16)+T(10)+T(24)+T(16)+T(25)+T(15)));
//G69:  __E___E_wff_20_21(60) => '[' ']'.
	p(NT(60), (T(26)+T(27)));
//G70:  __E_wff_20(59)       => __E___E_wff_20_21(60) _(13) wff(15).
	p(NT(59), (NT(60)+NT(13)+NT(15)));
//G71:  wff_always(58)       => __E_wff_20(59).
	p(NT(58), (NT(59)));
//G72:  wff(15)              => wff_always(58).
	p(NT(15), (NT(58)));
//G73:  __E_wff_22(62)       => wff(15) _(13) '?' _(13) wff(15) _(13) ':' _(13) wff(15).
	p(NT(62), (NT(15)+NT(13)+T(28)+NT(13)+NT(15)+NT(13)+T(2)+NT(13)+NT(15)));
//G74:  wff_conditional(61)  => __E_wff_22(62).
	p(NT(61), (NT(62)));
//G75:  wff(15)              => wff_conditional(61).
	p(NT(15), (NT(61)));
//G76:  __E_wff_23(64)       => 'a' 'l' 'l' __(34) q_vars(65) __(34) wff(15).
	p(NT(64), (T(16)+T(10)+T(10)+NT(34)+NT(65)+NT(34)+NT(15)));
//G77:  wff_all(63)          => __E_wff_23(64).
	p(NT(63), (NT(64)));
//G78:  wff(15)              => wff_all(63).
	p(NT(15), (NT(63)));
//G79:  __E_wff_24(67)       => 'e' 'x' __(34) q_vars(65) __(34) wff(15).
	p(NT(67), (T(11)+T(29)+NT(34)+NT(65)+NT(34)+NT(15)));
//G80:  wff_ex(66)           => __E_wff_24(67).
	p(NT(66), (NT(67)));
//G81:  wff(15)              => wff_ex(66).
	p(NT(15), (NT(66)));
//G82:  wff_ref(68)          => ref(27).
	p(NT(68), (NT(27)));
//G83:  wff(15)              => wff_ref(68).
	p(NT(15), (NT(68)));
//G84:  wff(15)              => constraint(69).
	p(NT(15), (NT(69)));
//G85:  __E_wff_25(71)       => wff(15) _(13) '-' '>' _(13) wff(15).
	p(NT(71), (NT(15)+NT(13)+T(30)+T(23)+NT(13)+NT(15)));
//G86:  wff_imply(70)        => __E_wff_25(71).
	p(NT(70), (NT(71)));
//G87:  wff(15)              => wff_imply(70).
	p(NT(15), (NT(70)));
//G88:  __E_wff_26(73)       => wff(15) _(13) '<' '-' _(13) wff(15).
	p(NT(73), (NT(15)+NT(13)+T(22)+T(30)+NT(13)+NT(15)));
//G89:  wff_rimply(72)       => __E_wff_26(73).
	p(NT(72), (NT(73)));
//G90:  wff(15)              => wff_rimply(72).
	p(NT(15), (NT(72)));
//G91:  __E_wff_27(75)       => wff(15) _(13) '<' '-' '>' _(13) wff(15).
	p(NT(75), (NT(15)+NT(13)+T(22)+T(30)+T(23)+NT(13)+NT(15)));
//G92:  wff_equiv(74)        => __E_wff_27(75).
	p(NT(74), (NT(75)));
//G93:  wff(15)              => wff_equiv(74).
	p(NT(15), (NT(74)));
//G94:  __E_wff_28(77)       => wff(15) _(13) '|' '|' _(13) wff(15).
	p(NT(77), (NT(15)+NT(13)+T(31)+T(31)+NT(13)+NT(15)));
//G95:  wff_or(76)           => __E_wff_28(77).
	p(NT(76), (NT(77)));
//G96:  wff(15)              => wff_or(76).
	p(NT(15), (NT(76)));
//G97:  __E_wff_29(79)       => wff(15) _(13) '^' '^' _(13) wff(15).
	p(NT(79), (NT(15)+NT(13)+T(32)+T(32)+NT(13)+NT(15)));
//G98:  wff_xor(78)          => __E_wff_29(79).
	p(NT(78), (NT(79)));
//G99:  wff(15)              => wff_xor(78).
	p(NT(15), (NT(78)));
//G100: __E_wff_30(81)       => wff(15) _(13) '&' '&' _(13) wff(15).
	p(NT(81), (NT(15)+NT(13)+T(33)+T(33)+NT(13)+NT(15)));
//G101: wff_and(80)          => __E_wff_30(81).
	p(NT(80), (NT(81)));
//G102: wff(15)              => wff_and(80).
	p(NT(15), (NT(80)));
//G103: __E_wff_31(83)       => '!' _(13) wff(15).
	p(NT(83), (T(34)+NT(13)+NT(15)));
//G104: wff_neg(82)          => __E_wff_31(83).
	p(NT(82), (NT(83)));
//G105: wff(15)              => wff_neg(82).
	p(NT(15), (NT(82)));
//G106: wff_t(84)            => 'T'.
	p(NT(84), (T(35)));
//G107: wff(15)              => wff_t(84).
	p(NT(15), (NT(84)));
//G108: wff_f(85)            => 'F'.
	p(NT(85), (T(36)));
//G109: wff(15)              => wff_f(85).
	p(NT(15), (NT(85)));
//G110: __E_wff_32(87)       => bf(30) _(13) '<' '=' _(13) bf(30) _(13) '<' '=' _(13) bf(30).
	p(NT(87), (NT(30)+NT(13)+T(22)+T(3)+NT(13)+NT(30)+NT(13)+T(22)+T(3)+NT(13)+NT(30)));
//G111: bf_interval(86)      => __E_wff_32(87).
	p(NT(86), (NT(87)));
//G112: wff(15)              => bf_interval(86).
	p(NT(15), (NT(86)));
//G113: __E_wff_33(89)       => bf(30) _(13) '=' _(13) bf(30).
	p(NT(89), (NT(30)+NT(13)+T(3)+NT(13)+NT(30)));
//G114: bf_eq(88)            => __E_wff_33(89).
	p(NT(88), (NT(89)));
//G115: wff(15)              => bf_eq(88).
	p(NT(15), (NT(88)));
//G116: __E_wff_34(91)       => bf(30) _(13) '!' '=' _(13) bf(30).
	p(NT(91), (NT(30)+NT(13)+T(34)+T(3)+NT(13)+NT(30)));
//G117: bf_neq(90)           => __E_wff_34(91).
	p(NT(90), (NT(91)));
//G118: wff(15)              => bf_neq(90).
	p(NT(15), (NT(90)));
//G119: __E_wff_35(93)       => bf(30) _(13) '<' _(13) bf(30).
	p(NT(93), (NT(30)+NT(13)+T(22)+NT(13)+NT(30)));
//G120: bf_lt(92)            => __E_wff_35(93).
	p(NT(92), (NT(93)));
//G121: wff(15)              => bf_lt(92).
	p(NT(15), (NT(92)));
//G122: __E_wff_36(95)       => bf(30) _(13) '!' '<' _(13) bf(30).
	p(NT(95), (NT(30)+NT(13)+T(34)+T(22)+NT(13)+NT(30)));
//G123: bf_nlt(94)           => __E_wff_36(95).
	p(NT(94), (NT(95)));
//G124: wff(15)              => bf_nlt(94).
	p(NT(15), (NT(94)));
//G125: __E_wff_37(97)       => bf(30) _(13) '<' '=' _(13) bf(30).
	p(NT(97), (NT(30)+NT(13)+T(22)+T(3)+NT(13)+NT(30)));
//G126: bf_lteq(96)          => __E_wff_37(97).
	p(NT(96), (NT(97)));
//G127: wff(15)              => bf_lteq(96).
	p(NT(15), (NT(96)));
//G128: __E_wff_38(99)       => bf(30) _(13) '!' '<' '=' _(13) bf(30).
	p(NT(99), (NT(30)+NT(13)+T(34)+T(22)+T(3)+NT(13)+NT(30)));
//G129: bf_nlteq(98)         => __E_wff_38(99).
	p(NT(98), (NT(99)));
//G130: wff(15)              => bf_nlteq(98).
	p(NT(15), (NT(98)));
//G131: __E_wff_39(101)      => bf(30) _(13) '>' _(13) bf(30).
	p(NT(101), (NT(30)+NT(13)+T(23)+NT(13)+NT(30)));
//G132: bf_gt(100)           => __E_wff_39(101).
	p(NT(100), (NT(101)));
//G133: wff(15)              => bf_gt(100).
	p(NT(15), (NT(100)));
//G134: __E_wff_40(103)      => bf(30) _(13) '!' '>' _(13) bf(30).
	p(NT(103), (NT(30)+NT(13)+T(34)+T(23)+NT(13)+NT(30)));
//G135: bf_ngt(102)          => __E_wff_40(103).
	p(NT(102), (NT(103)));
//G136: wff(15)              => bf_ngt(102).
	p(NT(15), (NT(102)));
//G137: __E_wff_41(105)      => bf(30) _(13) '>' '=' _(13) bf(30).
	p(NT(105), (NT(30)+NT(13)+T(23)+T(3)+NT(13)+NT(30)));
//G138: bf_gteq(104)         => __E_wff_41(105).
	p(NT(104), (NT(105)));
//G139: wff(15)              => bf_gteq(104).
	p(NT(15), (NT(104)));
//G140: __E_wff_42(107)      => bf(30) _(13) '!' '>' '=' _(13) bf(30).
	p(NT(107), (NT(30)+NT(13)+T(34)+T(23)+T(3)+NT(13)+NT(30)));
//G141: bf_ngteq(106)        => __E_wff_42(107).
	p(NT(106), (NT(107)));
//G142: wff(15)              => bf_ngteq(106).
	p(NT(15), (NT(106)));
//G143: wff(15)              => capture(29).
	p(NT(15), (NT(29)));
//G144: __E_bf_43(109)       => '(' _(13) bf(30) _(13) ')'.
	p(NT(109), (T(12)+NT(13)+NT(30)+NT(13)+T(13)));
//G145: bf_parenthesis(108)  => __E_bf_43(109).
	p(NT(108), (NT(109)));
//G146: bf(30)               => bf_parenthesis(108).
	p(NT(30), (NT(108)));
//G147: bf(30)               => ba_constant(110).
	p(NT(30), (NT(110)));
//G148: bf(30)               => variable(111).
	p(NT(30), (NT(111)));
//G149: __E_bf_44(113)       => 'f' 'a' 'l' 'l' __(34) q_vars(65) __(34) bf(30).
	p(NT(113), (T(9)+T(16)+T(10)+T(10)+NT(34)+NT(65)+NT(34)+NT(30)));
//G150: bf_fall(112)         => __E_bf_44(113).
	p(NT(112), (NT(113)));
//G151: bf(30)               => bf_fall(112).
	p(NT(30), (NT(112)));
//G152: __E_bf_45(115)       => 'f' 'e' 'x' __(34) q_vars(65) __(34) bf(30).
	p(NT(115), (T(9)+T(11)+T(29)+NT(34)+NT(65)+NT(34)+NT(30)));
//G153: bf_fex(114)          => __E_bf_45(115).
	p(NT(114), (NT(115)));
//G154: bf(30)               => bf_fex(114).
	p(NT(30), (NT(114)));
//G155: bf_ref(116)          => ref(27).
	p(NT(116), (NT(27)));
//G156: bf(30)               => bf_ref(116).
	p(NT(30), (NT(116)));
//G157: __N_0(382)           => bf_shr(117).
	p(NT(382), (NT(117)));
//G158: __E___E_bf_46_47(119) => bf(30) & ~( __N_0(382) ).	 # conjunctive
	p(NT(119), (NT(30)) & ~(NT(382)));
//G159: __E_bf_46(118)       => bf(30) _(13) '>' '>' _(13) __E___E_bf_46_47(119).
	p(NT(118), (NT(30)+NT(13)+T(23)+T(23)+NT(13)+NT(119)));
//G160: bf_shr(117)          => __E_bf_46(118).
	p(NT(117), (NT(118)));
//G161: bf(30)               => bf_shr(117).
	p(NT(30), (NT(117)));
//G162: __N_1(383)           => bf_shl(120).
	p(NT(383), (NT(120)));
//G163: __E___E_bf_48_49(122) => bf(30) & ~( __N_1(383) ).	 # conjunctive
	p(NT(122), (NT(30)) & ~(NT(383)));
//G164: __E_bf_48(121)       => bf(30) _(13) '<' '<' _(13) __E___E_bf_48_49(122).
	p(NT(121), (NT(30)+NT(13)+T(22)+T(22)+NT(13)+NT(122)));
//G165: bf_shl(120)          => __E_bf_48(121).
	p(NT(120), (NT(121)));
//G166: bf(30)               => bf_shl(120).
	p(NT(30), (NT(120)));
//G167: __E_bf_50(124)       => bf(30) _(13) '+' _(13) bf(30).
	p(NT(124), (NT(30)+NT(13)+T(37)+NT(13)+NT(30)));
//G168: bf_add(123)          => __E_bf_50(124).
	p(NT(123), (NT(124)));
//G169: bf(30)               => bf_add(123).
	p(NT(30), (NT(123)));
//G170: __N_2(384)           => bf_sub(125).
	p(NT(384), (NT(125)));
//G171: __E___E_bf_51_52(127) => bf(30) & ~( __N_2(384) ).	 # conjunctive
	p(NT(127), (NT(30)) & ~(NT(384)));
//G172: __E_bf_51(126)       => bf(30) _(13) '-' _(13) __E___E_bf_51_52(127).
	p(NT(126), (NT(30)+NT(13)+T(30)+NT(13)+NT(127)));
//G173: bf_sub(125)          => __E_bf_51(126).
	p(NT(125), (NT(126)));
//G174: bf(30)               => bf_sub(125).
	p(NT(30), (NT(125)));
//G175: __N_3(385)           => bf_mod(128).
	p(NT(385), (NT(128)));
//G176: __E___E_bf_53_54(130) => bf(30) & ~( __N_3(385) ).	 # conjunctive
	p(NT(130), (NT(30)) & ~(NT(385)));
//G177: __E_bf_53(129)       => bf(30) _(13) '%' _(13) __E___E_bf_53_54(130).
	p(NT(129), (NT(30)+NT(13)+T(38)+NT(13)+NT(130)));
//G178: bf_mod(128)          => __E_bf_53(129).
	p(NT(128), (NT(129)));
//G179: bf(30)               => bf_mod(128).
	p(NT(30), (NT(128)));
//G180: __E_bf_55(132)       => bf(30) _(13) '*' _(13) bf(30).
	p(NT(132), (NT(30)+NT(13)+T(39)+NT(13)+NT(30)));
//G181: bf_mul(131)          => __E_bf_55(132).
	p(NT(131), (NT(132)));
//G182: bf(30)               => bf_mul(131).
	p(NT(30), (NT(131)));
//G183: __N_4(386)           => bf_div(133).
	p(NT(386), (NT(133)));
//G184: __E___E_bf_56_57(135) => bf(30) & ~( __N_4(386) ).	 # conjunctive
	p(NT(135), (NT(30)) & ~(NT(386)));
//G185: __E_bf_56(134)       => bf(30) _(13) '/' _(13) __E___E_bf_56_57(135).
	p(NT(134), (NT(30)+NT(13)+T(40)+NT(13)+NT(135)));
//G186: bf_div(133)          => __E_bf_56(134).
	p(NT(133), (NT(134)));
//G187: bf(30)               => bf_div(133).
	p(NT(30), (NT(133)));
//G188: __N_5(387)           => bf_nor(136).
	p(NT(387), (NT(136)));
//G189: __E___E_bf_58_59(138) => bf(30) & ~( __N_5(387) ).	 # conjunctive
	p(NT(138), (NT(30)) & ~(NT(387)));
//G190: __E_bf_58(137)       => bf(30) _(13) '!' '|' _(13) __E___E_bf_58_59(138).
	p(NT(137), (NT(30)+NT(13)+T(34)+T(31)+NT(13)+NT(138)));
//G191: bf_nor(136)          => __E_bf_58(137).
	p(NT(136), (NT(137)));
//G192: bf(30)               => bf_nor(136).
	p(NT(30), (NT(136)));
//G193: __N_6(388)           => bf_xnor(139).
	p(NT(388), (NT(139)));
//G194: __E___E_bf_60_61(141) => bf(30) & ~( __N_6(388) ).	 # conjunctive
	p(NT(141), (NT(30)) & ~(NT(388)));
//G195: __E_bf_60(140)       => bf(30) _(13) '!' '^' _(13) __E___E_bf_60_61(141).
	p(NT(140), (NT(30)+NT(13)+T(34)+T(32)+NT(13)+NT(141)));
//G196: bf_xnor(139)         => __E_bf_60(140).
	p(NT(139), (NT(140)));
//G197: bf(30)               => bf_xnor(139).
	p(NT(30), (NT(139)));
//G198: __N_7(389)           => bf_nand(142).
	p(NT(389), (NT(142)));
//G199: __E___E_bf_62_63(144) => bf(30) & ~( __N_7(389) ).	 # conjunctive
	p(NT(144), (NT(30)) & ~(NT(389)));
//G200: __E_bf_62(143)       => bf(30) _(13) '!' '&' _(13) __E___E_bf_62_63(144).
	p(NT(143), (NT(30)+NT(13)+T(34)+T(33)+NT(13)+NT(144)));
//G201: bf_nand(142)         => __E_bf_62(143).
	p(NT(142), (NT(143)));
//G202: bf(30)               => bf_nand(142).
	p(NT(30), (NT(142)));
//G203: __E_bf_64(146)       => bf(30) _(13) '|' _(13) bf(30).
	p(NT(146), (NT(30)+NT(13)+T(31)+NT(13)+NT(30)));
//G204: bf_or(145)           => __E_bf_64(146).
	p(NT(145), (NT(146)));
//G205: bf(30)               => bf_or(145).
	p(NT(30), (NT(145)));
//G206: __E_bf_65(148)       => bf(30) _(13) '^' _(13) bf(30).
	p(NT(148), (NT(30)+NT(13)+T(32)+NT(13)+NT(30)));
//G207: bf_xor(147)          => __E_bf_65(148).
	p(NT(147), (NT(148)));
//G208: bf(30)               => bf_xor(147).
	p(NT(30), (NT(147)));
//G209: __E___E_bf_66_67(151) => typed(33).
	p(NT(151), (NT(33)));
//G210: __E___E_bf_66_67(151) => null.
	p(NT(151), (nul));
//G211: __E_bf_66(150)       => '1' __E___E_bf_66_67(151).
	p(NT(150), (T(41)+NT(151)));
//G212: bf_t(149)            => __E_bf_66(150).
	p(NT(149), (NT(150)));
//G213: bf(30)               => bf_t(149).
	p(NT(30), (NT(149)));
//G214: __E___E_bf_68_69(154) => typed(33).
	p(NT(154), (NT(33)));
//G215: __E___E_bf_68_69(154) => null.
	p(NT(154), (nul));
//G216: __E_bf_68(153)       => '0' __E___E_bf_68_69(154).
	p(NT(153), (T(42)+NT(154)));
//G217: bf_f(152)            => __E_bf_68(153).
	p(NT(152), (NT(153)));
//G218: bf(30)               => bf_f(152).
	p(NT(30), (NT(152)));
//G219: __E___E_bf_70_71(157) => _(13) '&' _(13).
	p(NT(157), (NT(13)+T(33)+NT(13)));
//G220: __E___E_bf_70_71(157) => __(34).
	p(NT(157), (NT(34)));
//G221: __E_bf_70(156)       => bf(30) __E___E_bf_70_71(157) bf(30).
	p(NT(156), (NT(30)+NT(157)+NT(30)));
//G222: bf_and(155)          => __E_bf_70(156).
	p(NT(155), (NT(156)));
//G223: bf(30)               => bf_and(155).
	p(NT(30), (NT(155)));
//G224: __E___E_bf_72_73(161) => bf_parenthesis(108).
	p(NT(161), (NT(108)));
//G225: __E___E_bf_72_73(161) => ba_constant(110).
	p(NT(161), (NT(110)));
//G226: __E___E_bf_72_73(161) => variable(111).
	p(NT(161), (NT(111)));
//G227: __E___E_bf_72_73(161) => bf_ref(116).
	p(NT(161), (NT(116)));
//G228: __E___E_bf_72_73(161) => bf_t(149).
	p(NT(161), (NT(149)));
//G229: __E___E_bf_72_73(161) => bf_f(152).
	p(NT(161), (NT(152)));
//G230: __E___E_bf_72_73(161) => bf_neg(158).
	p(NT(161), (NT(158)));
//G231: __E___E_bf_72_73(161) => capture(29).
	p(NT(161), (NT(29)));
//G232: bf_neg_oprnd(160)    => __E___E_bf_72_73(161).
	p(NT(160), (NT(161)));
//G233: __E_bf_72(159)       => bf_neg_oprnd(160) _(13) '\''.
	p(NT(159), (NT(160)+NT(13)+T(43)));
//G234: bf_neg(158)          => __E_bf_72(159).
	p(NT(158), (NT(159)));
//G235: bf(30)               => bf_neg(158).
	p(NT(30), (NT(158)));
//G236: __E___E_bf_74_75(165) => bf_parenthesis(108).
	p(NT(165), (NT(108)));
//G237: __E___E_bf_74_75(165) => variable(111).
	p(NT(165), (NT(111)));
//G238: __E___E_bf_74_75(165) => bf_ref(116).
	p(NT(165), (NT(116)));
//G239: __E___E_bf_74_75(165) => bf_neg(158).
	p(NT(165), (NT(158)));
//G240: __E___E_bf_74_75(165) => bf_and_nosep(162).
	p(NT(165), (NT(162)));
//G241: __E___E_bf_74_75(165) => capture(29).
	p(NT(165), (NT(29)));
//G242: bf_and_nosep_1st_oprnd(164) => __E___E_bf_74_75(165).
	p(NT(164), (NT(165)));
//G243: __E___E_bf_74_76(167) => bf_parenthesis(108).
	p(NT(167), (NT(108)));
//G244: __E___E_bf_74_76(167) => ba_constant(110).
	p(NT(167), (NT(110)));
//G245: __E___E_bf_74_76(167) => variable(111).
	p(NT(167), (NT(111)));
//G246: __E___E_bf_74_76(167) => bf_ref(116).
	p(NT(167), (NT(116)));
//G247: __E___E_bf_74_76(167) => bf_neg(158).
	p(NT(167), (NT(158)));
//G248: __E___E_bf_74_76(167) => capture(29).
	p(NT(167), (NT(29)));
//G249: bf_and_nosep_2nd_oprnd(166) => __E___E_bf_74_76(167).
	p(NT(166), (NT(167)));
//G250: __E_bf_74(163)       => bf_and_nosep_1st_oprnd(164) bf_and_nosep_2nd_oprnd(166).
	p(NT(163), (NT(164)+NT(166)));
//G251: bf_and_nosep(162)    => __E_bf_74(163).
	p(NT(162), (NT(163)));
//G252: bf(30)               => bf_and_nosep(162).
	p(NT(30), (NT(162)));
//G253: bf(30)               => capture(29).
	p(NT(30), (NT(29)));
//G254: __E_constraint_77(169) => '[' ctnvar(170) _(13) '!' '=' _(13) num(171) ']'.
	p(NT(169), (T(26)+NT(170)+NT(13)+T(34)+T(3)+NT(13)+NT(171)+T(27)));
//G255: __E_constraint_77(169) => '[' num(171) _(13) '!' '=' _(13) ctnvar(170) ']'.
	p(NT(169), (T(26)+NT(171)+NT(13)+T(34)+T(3)+NT(13)+NT(170)+T(27)));
//G256: ctn_neq(168)         => __E_constraint_77(169).
	p(NT(168), (NT(169)));
//G257: constraint(69)       => ctn_neq(168).
	p(NT(69), (NT(168)));
//G258: __E_constraint_78(173) => '[' ctnvar(170) _(13) '=' _(13) num(171) ']'.
	p(NT(173), (T(26)+NT(170)+NT(13)+T(3)+NT(13)+NT(171)+T(27)));
//G259: __E_constraint_78(173) => '[' num(171) _(13) '=' _(13) ctnvar(170) ']'.
	p(NT(173), (T(26)+NT(171)+NT(13)+T(3)+NT(13)+NT(170)+T(27)));
//G260: ctn_eq(172)          => __E_constraint_78(173).
	p(NT(172), (NT(173)));
//G261: constraint(69)       => ctn_eq(172).
	p(NT(69), (NT(172)));
//G262: __E_constraint_79(175) => '[' ctnvar(170) _(13) '>' '=' _(13) num(171) ']'.
	p(NT(175), (T(26)+NT(170)+NT(13)+T(23)+T(3)+NT(13)+NT(171)+T(27)));
//G263: __E_constraint_79(175) => '[' num(171) _(13) '>' '=' _(13) ctnvar(170) ']'.
	p(NT(175), (T(26)+NT(171)+NT(13)+T(23)+T(3)+NT(13)+NT(170)+T(27)));
//G264: ctn_gteq(174)        => __E_constraint_79(175).
	p(NT(174), (NT(175)));
//G265: constraint(69)       => ctn_gteq(174).
	p(NT(69), (NT(174)));
//G266: __E_constraint_80(177) => '[' ctnvar(170) _(13) '>' _(13) num(171) ']'.
	p(NT(177), (T(26)+NT(170)+NT(13)+T(23)+NT(13)+NT(171)+T(27)));
//G267: __E_constraint_80(177) => '[' num(171) _(13) '>' _(13) ctnvar(170) ']'.
	p(NT(177), (T(26)+NT(171)+NT(13)+T(23)+NT(13)+NT(170)+T(27)));
//G268: ctn_gt(176)          => __E_constraint_80(177).
	p(NT(176), (NT(177)));
//G269: constraint(69)       => ctn_gt(176).
	p(NT(69), (NT(176)));
//G270: __E_constraint_81(179) => '[' ctnvar(170) _(13) '<' '=' _(13) num(171) ']'.
	p(NT(179), (T(26)+NT(170)+NT(13)+T(22)+T(3)+NT(13)+NT(171)+T(27)));
//G271: __E_constraint_81(179) => '[' num(171) _(13) '<' '=' _(13) ctnvar(170) ']'.
	p(NT(179), (T(26)+NT(171)+NT(13)+T(22)+T(3)+NT(13)+NT(170)+T(27)));
//G272: ctn_lteq(178)        => __E_constraint_81(179).
	p(NT(178), (NT(179)));
//G273: constraint(69)       => ctn_lteq(178).
	p(NT(69), (NT(178)));
//G274: __E_constraint_82(181) => '[' ctnvar(170) _(13) '<' _(13) num(171) ']'.
	p(NT(181), (T(26)+NT(170)+NT(13)+T(22)+NT(13)+NT(171)+T(27)));
//G275: __E_constraint_82(181) => '[' num(171) _(13) '<' _(13) ctnvar(170) ']'.
	p(NT(181), (T(26)+NT(171)+NT(13)+T(22)+NT(13)+NT(170)+T(27)));
//G276: ctn_lt(180)          => __E_constraint_82(181).
	p(NT(180), (NT(181)));
//G277: constraint(69)       => ctn_lt(180).
	p(NT(69), (NT(180)));
//G278: __E_ba_constant_83(182) => capture(29).
	p(NT(182), (NT(29)));
//G279: __E_ba_constant_83(182) => source(183).
	p(NT(182), (NT(183)));
//G280: __E_ba_constant_84(184) => typed(33).
	p(NT(184), (NT(33)));
//G281: __E_ba_constant_84(184) => null.
	p(NT(184), (nul));
//G282: ba_constant(110)     => '{' _(13) __E_ba_constant_83(182) _(13) '}' __E_ba_constant_84(184).
	p(NT(110), (T(44)+NT(13)+NT(182)+NT(13)+T(45)+NT(184)));
//G283: __E___E_source_85_86(187) => src_c(185).
	p(NT(187), (NT(185)));
//G284: __E___E_source_85_86(187) => space(2).
	p(NT(187), (NT(2)));
//G285: __E___E_source_85_87(188) => null.
	p(NT(188), (nul));
//G286: __E___E_source_85_87(188) => __E___E_source_85_86(187) __E___E_source_85_87(188).
	p(NT(188), (NT(187)+NT(188)));
//G287: __E_source_85(186)   => __E___E_source_85_87(188) src_c(185).
	p(NT(186), (NT(188)+NT(185)));
//G288: __E_source_85(186)   => null.
	p(NT(186), (nul));
//G289: source(183)          => src_c(185) __E_source_85(186).
	p(NT(183), (NT(185)+NT(186)));
//G290: src_c(185)           => alnum(6).
	p(NT(185), (NT(6)));
//G291: __N_8(390)           => '{'.
	p(NT(390), (T(44)));
//G292: __N_9(391)           => '}'.
	p(NT(391), (T(45)));
//G293: src_c(185)           => ~( __N_8(390) ) & ~( __N_9(391) ) & punct(7).	 # conjunctive
	p(NT(185), ~(NT(390)) & ~(NT(391)) & (NT(7)));
//G294: __E_src_c_88(189)    => src_c(185).
	p(NT(189), (NT(185)));
//G295: __E_src_c_88(189)    => space(2).
	p(NT(189), (NT(2)));
//G296: __E_src_c_89(190)    => null.
	p(NT(190), (nul));
//G297: __E_src_c_89(190)    => __E_src_c_88(189) __E_src_c_89(190).
	p(NT(190), (NT(189)+NT(190)));
//G298: src_c(185)           => '{' __E_src_c_89(190) '}'.
	p(NT(185), (T(44)+NT(190)+T(45)));
//G299: __E_variable_90(191) => uconst(192).
	p(NT(191), (NT(192)));
//G300: __E_variable_90(191) => io_var(193).
	p(NT(191), (NT(193)));
//G301: __E_variable_90(191) => var_name(194).
	p(NT(191), (NT(194)));
//G302: __E_variable_91(195) => typed(33).
	p(NT(195), (NT(33)));
//G303: __E_variable_91(195) => null.
	p(NT(195), (nul));
//G304: variable(111)        => __E_variable_90(191) __E_variable_91(195).
	p(NT(111), (NT(191)+NT(195)));
//G305: __N_10(392)          => 'F'.
	p(NT(392), (T(36)));
//G306: __N_11(393)          => 'T'.
	p(NT(393), (T(35)));
//G307: __E_var_name_92(196) => ~( __N_10(392) ) & ~( __N_11(393) ) & alpha(5).	 # conjunctive
	p(NT(196), ~(NT(392)) & ~(NT(393)) & (NT(5)));
//G308: __E_var_name_93(197) => null.
	p(NT(197), (nul));
//G309: __E_var_name_93(197) => digit(3) __E_var_name_93(197).
	p(NT(197), (NT(3)+NT(197)));
//G310: var_name(194)        => __E_var_name_92(196) __E_var_name_93(197).	 # guarded: charvar
	p(NT(194), (NT(196)+NT(197)));
	p.back().guard = "charvar";
//G311: __N_12(394)          => 'F'.
	p(NT(394), (T(36)));
//G312: __N_13(395)          => 'T'.
	p(NT(395), (T(35)));
//G313: __E_var_name_94(198) => ~( __N_12(394) ) & ~( __N_13(395) ) & alpha(5).	 # conjunctive
	p(NT(198), ~(NT(394)) & ~(NT(395)) & (NT(5)));
//G314: __E_var_name_95(199) => alnum(6).
	p(NT(199), (NT(6)));
//G315: __E_var_name_95(199) => '_'.
	p(NT(199), (T(46)));
//G316: __E_var_name_96(200) => null.
	p(NT(200), (nul));
//G317: __E_var_name_96(200) => __E_var_name_95(199) __E_var_name_96(200).
	p(NT(200), (NT(199)+NT(200)));
//G318: var_name(194)        => __E_var_name_94(198) __E_var_name_96(200).	 # guarded: var
	p(NT(194), (NT(198)+NT(200)));
	p.back().guard = "var";
//G319: io_var(193)          => io_var_name(31) '[' offset(201) ']'.
	p(NT(193), (NT(31)+T(26)+NT(201)+T(27)));
//G320: io_var_name(31)      => chars(202).
	p(NT(31), (NT(202)));
//G321: __E___E_uconst_97_98(205) => chars(202) _(13).
	p(NT(205), (NT(202)+NT(13)));
//G322: __E___E_uconst_97_98(205) => null.
	p(NT(205), (nul));
//G323: __E_uconst_97(204)   => __E___E_uconst_97_98(205) ':' _(13) chars(202).
	p(NT(204), (NT(205)+T(2)+NT(13)+NT(202)));
//G324: uconst_name(203)     => __E_uconst_97(204).
	p(NT(203), (NT(204)));
//G325: uconst(192)          => '<' _(13) uconst_name(203) _(13) '>'.
	p(NT(192), (T(22)+NT(13)+NT(203)+NT(13)+T(23)));
//G326: __E_q_vars_99(207)   => _(13) ',' _(13) q_var(206).
	p(NT(207), (NT(13)+T(20)+NT(13)+NT(206)));
//G327: __E_q_vars_100(208)  => null.
	p(NT(208), (nul));
//G328: __E_q_vars_100(208)  => __E_q_vars_99(207) __E_q_vars_100(208).
	p(NT(208), (NT(207)+NT(208)));
//G329: q_vars(65)           => q_var(206) __E_q_vars_100(208).
	p(NT(65), (NT(206)+NT(208)));
//G330: q_var(206)           => capture(29).
	p(NT(206), (NT(29)));
//G331: __N_14(396)          => uconst(192).
	p(NT(396), (NT(192)));
//G332: q_var(206)           => variable(111) & ~( __N_14(396) ).	 # conjunctive
	p(NT(206), (NT(111)) & ~(NT(396)));
//G333: ctnvar(170)          => chars(202).
	p(NT(170), (NT(202)));
//G334: __E_offsets_101(209) => _(13) ',' _(13) offset(201).
	p(NT(209), (NT(13)+T(20)+NT(13)+NT(201)));
//G335: __E_offsets_102(210) => null.
	p(NT(210), (nul));
//G336: __E_offsets_102(210) => __E_offsets_101(209) __E_offsets_102(210).
	p(NT(210), (NT(209)+NT(210)));
//G337: offsets(41)          => '[' _(13) offset(201) __E_offsets_102(210) _(13) ']'.
	p(NT(41), (T(26)+NT(13)+NT(201)+NT(210)+NT(13)+T(27)));
//G338: offset(201)          => integer(211).
	p(NT(201), (NT(211)));
//G339: offset(201)          => capture(29).
	p(NT(201), (NT(29)));
//G340: offset(201)          => shift(212).
	p(NT(201), (NT(212)));
//G341: __N_15(397)          => io_var(193).
	p(NT(397), (NT(193)));
//G342: __E_offset_103(213)  => variable(111) & ~( __N_15(397) ).	 # conjunctive
	p(NT(213), (NT(111)) & ~(NT(397)));
//G343: offset(201)          => __E_offset_103(213).
	p(NT(201), (NT(213)));
//G344: __E_shift_104(214)   => capture(29).
	p(NT(214), (NT(29)));
//G345: __N_16(398)          => io_var(193).
	p(NT(398), (NT(193)));
//G346: __E___E_shift_104_105(215) => variable(111) & ~( __N_16(398) ).	 # conjunctive
	p(NT(215), (NT(111)) & ~(NT(398)));
//G347: __E_shift_104(214)   => __E___E_shift_104_105(215).
	p(NT(214), (NT(215)));
//G348: shift(212)           => __E_shift_104(214) _(13) '-' _(13) num(171).
	p(NT(212), (NT(214)+NT(13)+T(30)+NT(13)+NT(171)));
//G349: __E_chars_106(216)   => alnum(6).
	p(NT(216), (NT(6)));
//G350: __E_chars_106(216)   => '_'.
	p(NT(216), (T(46)));
//G351: __E_chars_107(217)   => null.
	p(NT(217), (nul));
//G352: __E_chars_107(217)   => __E_chars_106(216) __E_chars_107(217).
	p(NT(217), (NT(216)+NT(217)));
//G353: chars(202)           => alpha(5) __E_chars_107(217).
	p(NT(202), (NT(5)+NT(217)));
//G354: __E_digits_108(219)  => digit(3).
	p(NT(219), (NT(3)));
//G355: __E_digits_108(219)  => digit(3) __E_digits_108(219).
	p(NT(219), (NT(3)+NT(219)));
//G356: digits(218)          => __E_digits_108(219).
	p(NT(218), (NT(219)));
//G357: num(171)             => digits(218).
	p(NT(171), (NT(218)));
//G358: __E_integer_109(220) => '-'.
	p(NT(220), (T(30)));
//G359: __E_integer_109(220) => null.
	p(NT(220), (nul));
//G360: integer(211)         => __E_integer_109(220) _(13) digits(218).
	p(NT(211), (NT(220)+NT(13)+NT(218)));
//G361: sym(39)              => chars(202).
	p(NT(39), (NT(202)));
//G362: capture(29)          => '$' chars(202).
	p(NT(29), (T(47)+NT(202)));
//G363: __E_typed_110(222)   => _(13) '[' _(13) subtype(223) _(13) ']' _(13).
	p(NT(222), (NT(13)+T(26)+NT(13)+NT(223)+NT(13)+T(27)+NT(13)));
//G364: __E_typed_110(222)   => null.
	p(NT(222), (nul));
//G365: typed(33)            => _(13) ':' _(13) type(221) __E_typed_110(222).
	p(NT(33), (NT(13)+T(2)+NT(13)+NT(221)+NT(222)));
//G366: type(221)            => chars(202).
	p(NT(221), (NT(202)));
//G367: subtype(223)         => num(171).
	p(NT(223), (NT(171)));
//G368: __E_comment_111(225) => printable(8).
	p(NT(225), (NT(8)));
//G369: __E_comment_111(225) => '\t'.
	p(NT(225), (T(48)));
//G370: __E_comment_112(226) => null.
	p(NT(226), (nul));
//G371: __E_comment_112(226) => __E_comment_111(225) __E_comment_112(226).
	p(NT(226), (NT(225)+NT(226)));
//G372: __E_comment_113(227) => '\n'.
	p(NT(227), (T(49)));
//G373: __E_comment_113(227) => '\r'.
	p(NT(227), (T(50)));
//G374: __E_comment_113(227) => eof(1).
	p(NT(227), (NT(1)));
//G375: comment(224)         => '#' __E_comment_112(226) __E_comment_113(227).
	p(NT(224), (T(51)+NT(226)+NT(227)));
//G376: __(34)               => space(2).
	p(NT(34), (NT(2)));
//G377: __(34)               => comment(224).
	p(NT(34), (NT(224)));
//G378: __(34)               => __(34) space(2).
	p(NT(34), (NT(34)+NT(2)));
//G379: __(34)               => __(34) comment(224).
	p(NT(34), (NT(34)+NT(224)));
//G380: _(13)                => __(34).
	p(NT(13), (NT(34)));
//G381: _(13)                => null.
	p(NT(13), (nul));
//G382: cli(228)             => _(13).
	p(NT(228), (NT(13)));
//G383: __E_cli_114(230)     => '.' _(13) cli_command(229) _(13).
	p(NT(230), (T(1)+NT(13)+NT(229)+NT(13)));
//G384: __E_cli_115(231)     => null.
	p(NT(231), (nul));
//G385: __E_cli_115(231)     => __E_cli_114(230) __E_cli_115(231).
	p(NT(231), (NT(230)+NT(231)));
//G386: cli(228)             => _(13) cli_command(229) _(13) __E_cli_115(231).
	p(NT(228), (NT(13)+NT(229)+NT(13)+NT(231)));
//G387: __E_cli_command_116(234) => 'q'.
	p(NT(234), (T(52)));
//G388: __E_cli_command_116(234) => 'q' 'u' 'i' 't'.
	p(NT(234), (T(52)+T(7)+T(4)+T(8)));
//G389: quit_sym(233)        => __E_cli_command_116(234).
	p(NT(233), (NT(234)));
//G390: quit_cmd(232)        => quit_sym(233).
	p(NT(232), (NT(233)));
//G391: cli_command(229)     => quit_cmd(232).
	p(NT(229), (NT(232)));
//G392: __E_cli_command_117(237) => 'v'.
	p(NT(237), (T(53)));
//G393: __E_cli_command_117(237) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(237), (T(53)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G394: version_sym(236)     => __E_cli_command_117(237).
	p(NT(236), (NT(237)));
//G395: version_cmd(235)     => version_sym(236).
	p(NT(235), (NT(236)));
//G396: cli_command(229)     => version_cmd(235).
	p(NT(229), (NT(235)));
//G397: __E_cli_command_118(240) => 'c'.
	p(NT(240), (T(14)));
//G398: __E_cli_command_118(240) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(240), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G399: clear_sym(239)       => __E_cli_command_118(240).
	p(NT(239), (NT(240)));
//G400: clear_cmd(238)       => clear_sym(239).
	p(NT(238), (NT(239)));
//G401: cli_command(229)     => clear_cmd(238).
	p(NT(229), (NT(238)));
//G402: __E___E_cli_command_119_120(244) => 'h'.
	p(NT(244), (T(54)));
//G403: __E___E_cli_command_119_120(244) => 'h' 'e' 'l' 'p'.
	p(NT(244), (T(54)+T(11)+T(10)+T(55)));
//G404: help_sym(243)        => __E___E_cli_command_119_120(244).
	p(NT(243), (NT(244)));
//G405: __E___E_cli_command_119_121(245) => __(34) help_arg(246).
	p(NT(245), (NT(34)+NT(246)));
//G406: __E___E_cli_command_119_121(245) => null.
	p(NT(245), (nul));
//G407: __E_cli_command_119(242) => help_sym(243) __E___E_cli_command_119_121(245).
	p(NT(242), (NT(243)+NT(245)));
//G408: help_cmd(241)        => __E_cli_command_119(242).
	p(NT(241), (NT(242)));
//G409: cli_command(229)     => help_cmd(241).
	p(NT(229), (NT(241)));
//G410: file_sym(249)        => 'f' 'i' 'l' 'e'.
	p(NT(249), (T(9)+T(4)+T(10)+T(11)));
//G411: __E_cli_command_122(248) => file_sym(249) __(34) q_string(250).
	p(NT(248), (NT(249)+NT(34)+NT(250)));
//G412: file_cmd(247)        => __E_cli_command_122(248).
	p(NT(247), (NT(248)));
//G413: cli_command(229)     => file_cmd(247).
	p(NT(229), (NT(247)));
//G414: valid_sym(253)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(253), (T(53)+T(16)+T(10)+T(4)+T(56)));
//G415: __E_cli_command_123(252) => valid_sym(253) __(34) normalize_cmd_arg(254).
	p(NT(252), (NT(253)+NT(34)+NT(254)));
//G416: valid_cmd(251)       => __E_cli_command_123(252).
	p(NT(251), (NT(252)));
//G417: cli_command(229)     => valid_cmd(251).
	p(NT(229), (NT(251)));
//G418: sat_sym(257)         => 's' 'a' 't'.
	p(NT(257), (T(15)+T(16)+T(8)));
//G419: __E_cli_command_124(256) => sat_sym(257) __(34) normalize_cmd_arg(254).
	p(NT(256), (NT(257)+NT(34)+NT(254)));
//G420: sat_cmd(255)         => __E_cli_command_124(256).
	p(NT(255), (NT(256)));
//G421: cli_command(229)     => sat_cmd(255).
	p(NT(229), (NT(255)));
//G422: unsat_sym(260)       => 'u' 'n' 's' 'a' 't'.
	p(NT(260), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G423: __E_cli_command_125(259) => unsat_sym(260) __(34) normalize_cmd_arg(254).
	p(NT(259), (NT(260)+NT(34)+NT(254)));
//G424: unsat_cmd(258)       => __E_cli_command_125(259).
	p(NT(258), (NT(259)));
//G425: cli_command(229)     => unsat_cmd(258).
	p(NT(229), (NT(258)));
//G426: solve_sym(263)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(263), (T(15)+T(6)+T(10)+T(53)+T(11)));
//G427: __E___E_cli_command_126_127(264) => solve_options(265).
	p(NT(264), (NT(265)));
//G428: __E___E_cli_command_126_127(264) => null.
	p(NT(264), (nul));
//G429: __E_cli_command_126(262) => solve_sym(263) __E___E_cli_command_126_127(264) __(34) wff_cmd_arg(266).
	p(NT(262), (NT(263)+NT(264)+NT(34)+NT(266)));
//G430: solve_cmd(261)       => __E_cli_command_126(262).
	p(NT(261), (NT(262)));
//G431: cli_command(229)     => solve_cmd(261).
	p(NT(229), (NT(261)));
//G432: lgrs_sym(269)        => 'l' 'g' 'r' 's'.
	p(NT(269), (T(10)+T(57)+T(19)+T(15)));
//G433: __E_cli_command_128(268) => lgrs_sym(269) __(34) wff_cmd_arg(266).
	p(NT(268), (NT(269)+NT(34)+NT(266)));
//G434: lgrs_cmd(267)        => __E_cli_command_128(268).
	p(NT(267), (NT(268)));
//G435: cli_command(229)     => lgrs_cmd(267).
	p(NT(229), (NT(267)));
//G436: __E___E_cli_command_129_130(273) => 'r'.
	p(NT(273), (T(19)));
//G437: __E___E_cli_command_129_130(273) => 'r' 'u' 'n'.
	p(NT(273), (T(19)+T(7)+T(5)));
//G438: run_sym(272)         => __E___E_cli_command_129_130(273).
	p(NT(272), (NT(273)));
//G439: __E___E_cli_command_129_131(274) => history(275).
	p(NT(274), (NT(275)));
//G440: __E___E_cli_command_129_131(274) => wff(15).
	p(NT(274), (NT(15)));
//G441: __E_cli_command_129(271) => run_sym(272) __(34) __E___E_cli_command_129_131(274).
	p(NT(271), (NT(272)+NT(34)+NT(274)));
//G442: run_cmd(270)         => __E_cli_command_129(271).
	p(NT(270), (NT(271)));
//G443: cli_command(229)     => run_cmd(270).
	p(NT(229), (NT(270)));
//G444: __E___E_cli_command_132_133(279) => 'n'.
	p(NT(279), (T(5)));
//G445: __E___E_cli_command_132_133(279) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(279), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(58)+T(11)));
//G446: normalize_sym(278)   => __E___E_cli_command_132_133(279).
	p(NT(278), (NT(279)));
//G447: __E_cli_command_132(277) => normalize_sym(278) __(34) normalize_cmd_arg(254).
	p(NT(277), (NT(278)+NT(34)+NT(254)));
//G448: normalize_cmd(276)   => __E_cli_command_132(277).
	p(NT(276), (NT(277)));
//G449: cli_command(229)     => normalize_cmd(276).
	p(NT(229), (NT(276)));
//G450: __E___E_cli_command_134_135(283) => 's'.
	p(NT(283), (T(15)));
//G451: __E___E_cli_command_134_135(283) => 's' 'u' 'b' 's' 't'.
	p(NT(283), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G452: __E___E_cli_command_134_135(283) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(283), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G453: subst_sym(282)       => __E___E_cli_command_134_135(283).
	p(NT(282), (NT(283)));
//G454: __E_cli_command_134(281) => subst_sym(282) __(34) nf_cmd_arg(284) _(13) '[' _(13) nf_cmd_arg(284) _(13) '/' _(13) nf_cmd_arg(284) _(13) ']'.
	p(NT(281), (NT(282)+NT(34)+NT(284)+NT(13)+T(26)+NT(13)+NT(284)+NT(13)+T(40)+NT(13)+NT(284)+NT(13)+T(27)));
//G455: subst_cmd(280)       => __E_cli_command_134(281).
	p(NT(280), (NT(281)));
//G456: cli_command(229)     => subst_cmd(280).
	p(NT(229), (NT(280)));
//G457: __E___E_cli_command_136_137(288) => 'i'.
	p(NT(288), (T(4)));
//G458: __E___E_cli_command_136_137(288) => 'i' 'n' 's' 't'.
	p(NT(288), (T(4)+T(5)+T(15)+T(8)));
//G459: __E___E_cli_command_136_137(288) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(288), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G460: inst_sym(287)        => __E___E_cli_command_136_137(288).
	p(NT(287), (NT(288)));
//G461: __E_cli_command_136(286) => inst_sym(287) __(34) inst_args(289).
	p(NT(286), (NT(287)+NT(34)+NT(289)));
//G462: inst_cmd(285)        => __E_cli_command_136(286).
	p(NT(285), (NT(286)));
//G463: cli_command(229)     => inst_cmd(285).
	p(NT(229), (NT(285)));
//G464: dnf_sym(292)         => 'd' 'n' 'f'.
	p(NT(292), (T(56)+T(5)+T(9)));
//G465: __E_cli_command_138(291) => dnf_sym(292) __(34) nf_cmd_arg(284).
	p(NT(291), (NT(292)+NT(34)+NT(284)));
//G466: dnf_cmd(290)         => __E_cli_command_138(291).
	p(NT(290), (NT(291)));
//G467: cli_command(229)     => dnf_cmd(290).
	p(NT(229), (NT(290)));
//G468: cnf_sym(295)         => 'c' 'n' 'f'.
	p(NT(295), (T(14)+T(5)+T(9)));
//G469: __E_cli_command_139(294) => cnf_sym(295) __(34) nf_cmd_arg(284).
	p(NT(294), (NT(295)+NT(34)+NT(284)));
//G470: cnf_cmd(293)         => __E_cli_command_139(294).
	p(NT(293), (NT(294)));
//G471: cli_command(229)     => cnf_cmd(293).
	p(NT(229), (NT(293)));
//G472: anf_sym(298)         => 'a' 'n' 'f'.
	p(NT(298), (T(16)+T(5)+T(9)));
//G473: __E_cli_command_140(297) => anf_sym(298) __(34) nf_cmd_arg(284).
	p(NT(297), (NT(298)+NT(34)+NT(284)));
//G474: anf_cmd(296)         => __E_cli_command_140(297).
	p(NT(296), (NT(297)));
//G475: cli_command(229)     => anf_cmd(296).
	p(NT(229), (NT(296)));
//G476: nnf_sym(301)         => 'n' 'n' 'f'.
	p(NT(301), (T(5)+T(5)+T(9)));
//G477: __E_cli_command_141(300) => nnf_sym(301) __(34) nf_cmd_arg(284).
	p(NT(300), (NT(301)+NT(34)+NT(284)));
//G478: nnf_cmd(299)         => __E_cli_command_141(300).
	p(NT(299), (NT(300)));
//G479: cli_command(229)     => nnf_cmd(299).
	p(NT(229), (NT(299)));
//G480: pnf_sym(304)         => 'p' 'n' 'f'.
	p(NT(304), (T(55)+T(5)+T(9)));
//G481: __E_cli_command_142(303) => pnf_sym(304) __(34) nf_cmd_arg(284).
	p(NT(303), (NT(304)+NT(34)+NT(284)));
//G482: pnf_cmd(302)         => __E_cli_command_142(303).
	p(NT(302), (NT(303)));
//G483: cli_command(229)     => pnf_cmd(302).
	p(NT(229), (NT(302)));
//G484: mnf_sym(307)         => 'm' 'n' 'f'.
	p(NT(307), (T(21)+T(5)+T(9)));
//G485: __E_cli_command_143(306) => mnf_sym(307) __(34) nf_cmd_arg(284).
	p(NT(306), (NT(307)+NT(34)+NT(284)));
//G486: mnf_cmd(305)         => __E_cli_command_143(306).
	p(NT(305), (NT(306)));
//G487: cli_command(229)     => mnf_cmd(305).
	p(NT(229), (NT(305)));
//G488: onf_sym(310)         => 'o' 'n' 'f'.
	p(NT(310), (T(6)+T(5)+T(9)));
//G489: __E_cli_command_144(309) => onf_sym(310) __(34) variable(111) __(34) onf_cmd_arg(311).
	p(NT(309), (NT(310)+NT(34)+NT(111)+NT(34)+NT(311)));
//G490: onf_cmd(308)         => __E_cli_command_144(309).
	p(NT(308), (NT(309)));
//G491: cli_command(229)     => onf_cmd(308).
	p(NT(229), (NT(308)));
//G492: qelim_sym(314)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(314), (T(52)+T(11)+T(10)+T(4)+T(21)));
//G493: __E_cli_command_145(313) => qelim_sym(314) __(34) wff_cmd_arg(266).
	p(NT(313), (NT(314)+NT(34)+NT(266)));
//G494: qelim_cmd(312)       => __E_cli_command_145(313).
	p(NT(312), (NT(313)));
//G495: cli_command(229)     => qelim_cmd(312).
	p(NT(229), (NT(312)));
//G496: get_sym(317)         => 'g' 'e' 't'.
	p(NT(317), (T(57)+T(11)+T(8)));
//G497: __E___E_cli_command_146_147(318) => __(34) option_name(319).
	p(NT(318), (NT(34)+NT(319)));
//G498: __E___E_cli_command_146_147(318) => null.
	p(NT(318), (nul));
//G499: __E_cli_command_146(316) => get_sym(317) __E___E_cli_command_146_147(318).
	p(NT(316), (NT(317)+NT(318)));
//G500: get_cmd(315)         => __E_cli_command_146(316).
	p(NT(315), (NT(316)));
//G501: cli_command(229)     => get_cmd(315).
	p(NT(229), (NT(315)));
//G502: set_sym(322)         => 's' 'e' 't'.
	p(NT(322), (T(15)+T(11)+T(8)));
//G503: __E___E_cli_command_148_149(323) => __(34).
	p(NT(323), (NT(34)));
//G504: __E___E_cli_command_148_149(323) => _(13) '=' _(13).
	p(NT(323), (NT(13)+T(3)+NT(13)));
//G505: __E_cli_command_148(321) => set_sym(322) __(34) option_name(319) __E___E_cli_command_148_149(323) option_value(324).
	p(NT(321), (NT(322)+NT(34)+NT(319)+NT(323)+NT(324)));
//G506: set_cmd(320)         => __E_cli_command_148(321).
	p(NT(320), (NT(321)));
//G507: cli_command(229)     => set_cmd(320).
	p(NT(229), (NT(320)));
//G508: enable_sym(327)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(327), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G509: __E_cli_command_150(326) => enable_sym(327) __(34) option_name(319).
	p(NT(326), (NT(327)+NT(34)+NT(319)));
//G510: enable_cmd(325)      => __E_cli_command_150(326).
	p(NT(325), (NT(326)));
//G511: cli_command(229)     => enable_cmd(325).
	p(NT(229), (NT(325)));
//G512: disable_sym(330)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(330), (T(56)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G513: __E_cli_command_151(329) => disable_sym(330) __(34) option_name(319).
	p(NT(329), (NT(330)+NT(34)+NT(319)));
//G514: disable_cmd(328)     => __E_cli_command_151(329).
	p(NT(328), (NT(329)));
//G515: cli_command(229)     => disable_cmd(328).
	p(NT(229), (NT(328)));
//G516: toggle_sym(333)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(333), (T(8)+T(6)+T(57)+T(57)+T(10)+T(11)));
//G517: __E_cli_command_152(332) => toggle_sym(333) __(34) option_name(319).
	p(NT(332), (NT(333)+NT(34)+NT(319)));
//G518: toggle_cmd(331)      => __E_cli_command_152(332).
	p(NT(331), (NT(332)));
//G519: cli_command(229)     => toggle_cmd(331).
	p(NT(229), (NT(331)));
//G520: __E___E_cli_command_153_154(337) => 'd' 'e' 'f' 's'.
	p(NT(337), (T(56)+T(11)+T(9)+T(15)));
//G521: __E___E_cli_command_153_154(337) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(337), (T(56)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G522: def_sym(336)         => __E___E_cli_command_153_154(337).
	p(NT(336), (NT(337)));
//G523: __E_cli_command_153(335) => def_sym(336).
	p(NT(335), (NT(336)));
//G524: def_list_cmd(334)    => __E_cli_command_153(335).
	p(NT(334), (NT(335)));
//G525: cli_command(229)     => def_list_cmd(334).
	p(NT(229), (NT(334)));
//G526: __E_cli_command_155(339) => def_sym(336) __(34) num(171).
	p(NT(339), (NT(336)+NT(34)+NT(171)));
//G527: def_print_cmd(338)   => __E_cli_command_155(339).
	p(NT(338), (NT(339)));
//G528: cli_command(229)     => def_print_cmd(338).
	p(NT(229), (NT(338)));
//G529: def_rr_cmd(340)      => rec_relation(18).
	p(NT(340), (NT(18)));
//G530: cli_command(229)     => def_rr_cmd(340).
	p(NT(229), (NT(340)));
//G531: def_input_cmd(341)   => input_def(19).
	p(NT(341), (NT(19)));
//G532: cli_command(229)     => def_input_cmd(341).
	p(NT(229), (NT(341)));
//G533: def_output_cmd(342)  => output_def(20).
	p(NT(342), (NT(20)));
//G534: cli_command(229)     => def_output_cmd(342).
	p(NT(229), (NT(342)));
//G535: __E___E_cli_command_156_157(346) => 'h' 'i' 's' 't'.
	p(NT(346), (T(54)+T(4)+T(15)+T(8)));
//G536: __E___E_cli_command_156_157(346) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(346), (T(54)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G537: history_sym(345)     => __E___E_cli_command_156_157(346).
	p(NT(345), (NT(346)));
//G538: __E_cli_command_156(344) => history_sym(345).
	p(NT(344), (NT(345)));
//G539: history_list_cmd(343) => __E_cli_command_156(344).
	p(NT(343), (NT(344)));
//G540: cli_command(229)     => history_list_cmd(343).
	p(NT(229), (NT(343)));
//G541: __E_cli_command_158(348) => history_sym(345) __(34) history(275).
	p(NT(348), (NT(345)+NT(34)+NT(275)));
//G542: history_print_cmd(347) => __E_cli_command_158(348).
	p(NT(347), (NT(348)));
//G543: cli_command(229)     => history_print_cmd(347).
	p(NT(229), (NT(347)));
//G544: __E_cli_command_159(350) => ref(27).
	p(NT(350), (NT(27)));
//G545: __E_cli_command_159(350) => wff(15).
	p(NT(350), (NT(15)));
//G546: __E_cli_command_159(350) => bf(30).
	p(NT(350), (NT(30)));
//G547: history_store_cmd(349) => __E_cli_command_159(350).
	p(NT(349), (NT(350)));
//G548: cli_command(229)     => history_store_cmd(349).
	p(NT(229), (NT(349)));
//G549: __E_solve_options_160(351) => __(34) solve_option(352).
	p(NT(351), (NT(34)+NT(352)));
//G550: __E_solve_options_161(353) => null.
	p(NT(353), (nul));
//G551: __E_solve_options_161(353) => __E_solve_options_160(351) __E_solve_options_161(353).
	p(NT(353), (NT(351)+NT(353)));
//G552: solve_options(265)   => __E_solve_options_161(353).
	p(NT(265), (NT(353)));
//G553: __E_solve_option_162(354) => solver_mode(355).
	p(NT(354), (NT(355)));
//G554: __E_solve_option_162(354) => type(221).
	p(NT(354), (NT(221)));
//G555: solve_option(352)    => '-' '-' __E_solve_option_162(354).
	p(NT(352), (T(30)+T(30)+NT(354)));
//G556: __E_solver_mode_163(357) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(357), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G557: __E_solver_mode_163(357) => 'm' 'i' 'n'.
	p(NT(357), (T(21)+T(4)+T(5)));
//G558: solver_mode_minimum(356) => __E_solver_mode_163(357).
	p(NT(356), (NT(357)));
//G559: solver_mode(355)     => solver_mode_minimum(356).
	p(NT(355), (NT(356)));
//G560: __E_solver_mode_164(359) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(359), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G561: __E_solver_mode_164(359) => 'm' 'a' 'x'.
	p(NT(359), (T(21)+T(16)+T(29)));
//G562: solver_mode_maximum(358) => __E_solver_mode_164(359).
	p(NT(358), (NT(359)));
//G563: solver_mode(355)     => solver_mode_maximum(358).
	p(NT(355), (NT(358)));
//G564: q_file_name(37)      => '"' file_name(360) '"'.
	p(NT(37), (T(59)+NT(360)+T(59)));
//G565: __E_file_name_165(361) => printable(8).
	p(NT(361), (NT(8)));
//G566: __E_file_name_165(361) => printable(8) __E_file_name_165(361).
	p(NT(361), (NT(8)+NT(361)));
//G567: file_name(360)       => __E_file_name_165(361).
	p(NT(360), (NT(361)));
//G568: __E_option_name_166(362) => alnum(6).
	p(NT(362), (NT(6)));
//G569: __E_option_name_166(362) => alnum(6) __E_option_name_166(362).
	p(NT(362), (NT(6)+NT(362)));
//G570: option_name(319)     => __E_option_name_166(362).
	p(NT(319), (NT(362)));
//G571: __E_option_value_167(363) => alnum(6).
	p(NT(363), (NT(6)));
//G572: __E_option_value_167(363) => alnum(6) __E_option_value_167(363).
	p(NT(363), (NT(6)+NT(363)));
//G573: option_value(324)    => __E_option_value_167(363).
	p(NT(324), (NT(363)));
//G574: bf_cmd_arg(364)      => history(275).
	p(NT(364), (NT(275)));
//G575: bf_cmd_arg(364)      => bf(30).
	p(NT(364), (NT(30)));
//G576: wff_cmd_arg(266)     => history(275).
	p(NT(266), (NT(275)));
//G577: wff_cmd_arg(266)     => wff(15).
	p(NT(266), (NT(15)));
//G578: nf_cmd_arg(284)      => history(275).
	p(NT(284), (NT(275)));
//G579: nf_cmd_arg(284)      => ref(27).
	p(NT(284), (NT(27)));
//G580: nf_cmd_arg(284)      => wff(15).
	p(NT(284), (NT(15)));
//G581: nf_cmd_arg(284)      => bf(30).
	p(NT(284), (NT(30)));
//G582: onf_cmd_arg(311)     => history(275).
	p(NT(311), (NT(275)));
//G583: onf_cmd_arg(311)     => wff(15).
	p(NT(311), (NT(15)));
//G584: normalize_cmd_arg(254) => history(275).
	p(NT(254), (NT(275)));
//G585: normalize_cmd_arg(254) => spec(10).
	p(NT(254), (NT(10)));
//G586: normalize_cmd_arg(254) => ref(27).
	p(NT(254), (NT(27)));
//G587: normalize_cmd_arg(254) => wff(15).
	p(NT(254), (NT(15)));
//G588: normalize_cmd_arg(254) => bf(30).
	p(NT(254), (NT(30)));
//G589: inst_args(289)       => wff_cmd_arg(266) _(13) '[' _(13) bf(30) _(13) '/' _(13) bf_cmd_arg(364) _(13) ']'.
	p(NT(289), (NT(266)+NT(13)+T(26)+NT(13)+NT(30)+NT(13)+T(40)+NT(13)+NT(364)+NT(13)+T(27)));
//G590: inst_args(289)       => bf_cmd_arg(364) _(13) '[' _(13) bf(30) _(13) '/' _(13) bf_cmd_arg(364) _(13) ']'.
	p(NT(289), (NT(364)+NT(13)+T(26)+NT(13)+NT(30)+NT(13)+T(40)+NT(13)+NT(364)+NT(13)+T(27)));
//G591: help_arg(246)        => help_sym(243).
	p(NT(246), (NT(243)));
//G592: help_arg(246)        => version_sym(236).
	p(NT(246), (NT(236)));
//G593: help_arg(246)        => quit_sym(233).
	p(NT(246), (NT(233)));
//G594: help_arg(246)        => clear_sym(239).
	p(NT(246), (NT(239)));
//G595: help_arg(246)        => get_sym(317).
	p(NT(246), (NT(317)));
//G596: help_arg(246)        => set_sym(322).
	p(NT(246), (NT(322)));
//G597: help_arg(246)        => enable_sym(327).
	p(NT(246), (NT(327)));
//G598: help_arg(246)        => disable_sym(330).
	p(NT(246), (NT(330)));
//G599: help_arg(246)        => toggle_sym(333).
	p(NT(246), (NT(333)));
//G600: help_arg(246)        => file_sym(249).
	p(NT(246), (NT(249)));
//G601: help_arg(246)        => history_sym(345).
	p(NT(246), (NT(345)));
//G602: help_arg(246)        => abs_history_sym(365).
	p(NT(246), (NT(365)));
//G603: help_arg(246)        => rel_history_sym(366).
	p(NT(246), (NT(366)));
//G604: help_arg(246)        => selection_sym(367).
	p(NT(246), (NT(367)));
//G605: help_arg(246)        => def_sym(336).
	p(NT(246), (NT(336)));
//G606: help_arg(246)        => inst_sym(287).
	p(NT(246), (NT(287)));
//G607: help_arg(246)        => subst_sym(282).
	p(NT(246), (NT(282)));
//G608: help_arg(246)        => normalize_sym(278).
	p(NT(246), (NT(278)));
//G609: help_arg(246)        => execute_sym(368).
	p(NT(246), (NT(368)));
//G610: help_arg(246)        => solve_sym(263).
	p(NT(246), (NT(263)));
//G611: help_arg(246)        => valid_sym(253).
	p(NT(246), (NT(253)));
//G612: help_arg(246)        => sat_sym(257).
	p(NT(246), (NT(257)));
//G613: help_arg(246)        => unsat_sym(260).
	p(NT(246), (NT(260)));
//G614: help_arg(246)        => run_sym(272).
	p(NT(246), (NT(272)));
//G615: help_arg(246)        => dnf_sym(292).
	p(NT(246), (NT(292)));
//G616: help_arg(246)        => cnf_sym(295).
	p(NT(246), (NT(295)));
//G617: help_arg(246)        => anf_sym(298).
	p(NT(246), (NT(298)));
//G618: help_arg(246)        => snf_sym(369).
	p(NT(246), (NT(369)));
//G619: help_arg(246)        => nnf_sym(301).
	p(NT(246), (NT(301)));
//G620: help_arg(246)        => pnf_sym(304).
	p(NT(246), (NT(304)));
//G621: help_arg(246)        => mnf_sym(307).
	p(NT(246), (NT(307)));
//G622: help_arg(246)        => onf_sym(310).
	p(NT(246), (NT(310)));
//G623: help_arg(246)        => qelim_sym(314).
	p(NT(246), (NT(314)));
//G624: __E___E_help_arg_168_169(372) => 's'.
	p(NT(372), (T(15)));
//G625: __E___E_help_arg_168_169(372) => null.
	p(NT(372), (nul));
//G626: __E_help_arg_168(371) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_168_169(372).
	p(NT(371), (T(11)+T(29)+T(16)+T(21)+T(55)+T(10)+T(11)+NT(372)));
//G627: examples_sym(370)    => __E_help_arg_168(371).
	p(NT(370), (NT(371)));
//G628: help_arg(246)        => examples_sym(370).
	p(NT(246), (NT(370)));
//G629: __E_history_170(374) => '%'.
	p(NT(374), (T(38)));
//G630: rel_history_sym(366) => '%' '-'.
	p(NT(366), (T(38)+T(30)));
//G631: history_id(376)      => digits(218).
	p(NT(376), (NT(218)));
//G632: __E___E_history_170_171(375) => history_id(376).
	p(NT(375), (NT(376)));
//G633: __E___E_history_170_171(375) => null.
	p(NT(375), (nul));
//G634: __E_history_170(374) => rel_history_sym(366) __E___E_history_170_171(375).
	p(NT(374), (NT(366)+NT(375)));
//G635: rel_history(373)     => __E_history_170(374).
	p(NT(373), (NT(374)));
//G636: history(275)         => rel_history(373).
	p(NT(275), (NT(373)));
//G637: abs_history_sym(365) => '%'.
	p(NT(365), (T(38)));
//G638: __E_history_172(378) => abs_history_sym(365) history_id(376).
	p(NT(378), (NT(365)+NT(376)));
//G639: abs_history(377)     => __E_history_172(378).
	p(NT(377), (NT(378)));
//G640: history(275)         => abs_history(377).
	p(NT(275), (NT(377)));
//G641: unreachable(379)     => processed(380).
	p(NT(379), (NT(380)));
//G642: start_get_stream_def(381) => input_def(19).
	p(NT(381), (NT(19)));
//G643: start_get_stream_def(381) => output_def(20).
	p(NT(381), (NT(20)));
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
		spec, __E_spec_0, definitions, _, main, wff, __E_definitions_1, __E___E_definitions_1_2, rec_relation, input_def, 
		output_def, __E_definitions_3, spec_multiline, __E_spec_multiline_4, spec_part, __E_spec_multiline_5, __E_spec_multiline_6, ref, __E_rec_relation_7, capture, 
		bf, io_var_name, __E_input_def_8, typed, __, stream, __E_output_def_9, q_file_name, console_sym, sym, 
		__E_ref_10, offsets, ref_args, __E_ref_11, __E_ref_12, fp_fallback, first_sym, last_sym, __E_ref_args_13, __E___E_ref_args_13_14, 
		ref_arg, __E___E_ref_args_13_15, __E___E_ref_args_13_16, wff_parenthesis, __E_wff_17, wff_sometimes, __E_wff_18, __E___E_wff_18_19, wff_always, __E_wff_20, 
		__E___E_wff_20_21, wff_conditional, __E_wff_22, wff_all, __E_wff_23, q_vars, wff_ex, __E_wff_24, wff_ref, constraint, 
		wff_imply, __E_wff_25, wff_rimply, __E_wff_26, wff_equiv, __E_wff_27, wff_or, __E_wff_28, wff_xor, __E_wff_29, 
		wff_and, __E_wff_30, wff_neg, __E_wff_31, wff_t, wff_f, bf_interval, __E_wff_32, bf_eq, __E_wff_33, 
		bf_neq, __E_wff_34, bf_lt, __E_wff_35, bf_nlt, __E_wff_36, bf_lteq, __E_wff_37, bf_nlteq, __E_wff_38, 
		bf_gt, __E_wff_39, bf_ngt, __E_wff_40, bf_gteq, __E_wff_41, bf_ngteq, __E_wff_42, bf_parenthesis, __E_bf_43, 
		ba_constant, variable, bf_fall, __E_bf_44, bf_fex, __E_bf_45, bf_ref, bf_shr, __E_bf_46, __E___E_bf_46_47, 
		bf_shl, __E_bf_48, __E___E_bf_48_49, bf_add, __E_bf_50, bf_sub, __E_bf_51, __E___E_bf_51_52, bf_mod, __E_bf_53, 
		__E___E_bf_53_54, bf_mul, __E_bf_55, bf_div, __E_bf_56, __E___E_bf_56_57, bf_nor, __E_bf_58, __E___E_bf_58_59, bf_xnor, 
		__E_bf_60, __E___E_bf_60_61, bf_nand, __E_bf_62, __E___E_bf_62_63, bf_or, __E_bf_64, bf_xor, __E_bf_65, bf_t, 
		__E_bf_66, __E___E_bf_66_67, bf_f, __E_bf_68, __E___E_bf_68_69, bf_and, __E_bf_70, __E___E_bf_70_71, bf_neg, __E_bf_72, 
		bf_neg_oprnd, __E___E_bf_72_73, bf_and_nosep, __E_bf_74, bf_and_nosep_1st_oprnd, __E___E_bf_74_75, bf_and_nosep_2nd_oprnd, __E___E_bf_74_76, ctn_neq, __E_constraint_77, 
		ctnvar, num, ctn_eq, __E_constraint_78, ctn_gteq, __E_constraint_79, ctn_gt, __E_constraint_80, ctn_lteq, __E_constraint_81, 
		ctn_lt, __E_constraint_82, __E_ba_constant_83, source, __E_ba_constant_84, src_c, __E_source_85, __E___E_source_85_86, __E___E_source_85_87, __E_src_c_88, 
		__E_src_c_89, __E_variable_90, uconst, io_var, var_name, __E_variable_91, __E_var_name_92, __E_var_name_93, __E_var_name_94, __E_var_name_95, 
		__E_var_name_96, offset, chars, uconst_name, __E_uconst_97, __E___E_uconst_97_98, q_var, __E_q_vars_99, __E_q_vars_100, __E_offsets_101, 
		__E_offsets_102, integer, shift, __E_offset_103, __E_shift_104, __E___E_shift_104_105, __E_chars_106, __E_chars_107, digits, __E_digits_108, 
		__E_integer_109, type, __E_typed_110, subtype, comment, __E_comment_111, __E_comment_112, __E_comment_113, cli, cli_command, 
		__E_cli_114, __E_cli_115, quit_cmd, quit_sym, __E_cli_command_116, version_cmd, version_sym, __E_cli_command_117, clear_cmd, clear_sym, 
		__E_cli_command_118, help_cmd, __E_cli_command_119, help_sym, __E___E_cli_command_119_120, __E___E_cli_command_119_121, help_arg, file_cmd, __E_cli_command_122, file_sym, 
		q_string, valid_cmd, __E_cli_command_123, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_124, sat_sym, unsat_cmd, __E_cli_command_125, 
		unsat_sym, solve_cmd, __E_cli_command_126, solve_sym, __E___E_cli_command_126_127, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_128, lgrs_sym, 
		run_cmd, __E_cli_command_129, run_sym, __E___E_cli_command_129_130, __E___E_cli_command_129_131, history, normalize_cmd, __E_cli_command_132, normalize_sym, __E___E_cli_command_132_133, 
		subst_cmd, __E_cli_command_134, subst_sym, __E___E_cli_command_134_135, nf_cmd_arg, inst_cmd, __E_cli_command_136, inst_sym, __E___E_cli_command_136_137, inst_args, 
		dnf_cmd, __E_cli_command_138, dnf_sym, cnf_cmd, __E_cli_command_139, cnf_sym, anf_cmd, __E_cli_command_140, anf_sym, nnf_cmd, 
		__E_cli_command_141, nnf_sym, pnf_cmd, __E_cli_command_142, pnf_sym, mnf_cmd, __E_cli_command_143, mnf_sym, onf_cmd, __E_cli_command_144, 
		onf_sym, onf_cmd_arg, qelim_cmd, __E_cli_command_145, qelim_sym, get_cmd, __E_cli_command_146, get_sym, __E___E_cli_command_146_147, option_name, 
		set_cmd, __E_cli_command_148, set_sym, __E___E_cli_command_148_149, option_value, enable_cmd, __E_cli_command_150, enable_sym, disable_cmd, __E_cli_command_151, 
		disable_sym, toggle_cmd, __E_cli_command_152, toggle_sym, def_list_cmd, __E_cli_command_153, def_sym, __E___E_cli_command_153_154, def_print_cmd, __E_cli_command_155, 
		def_rr_cmd, def_input_cmd, def_output_cmd, history_list_cmd, __E_cli_command_156, history_sym, __E___E_cli_command_156_157, history_print_cmd, __E_cli_command_158, history_store_cmd, 
		__E_cli_command_159, __E_solve_options_160, solve_option, __E_solve_options_161, __E_solve_option_162, solver_mode, solver_mode_minimum, __E_solver_mode_163, solver_mode_maximum, __E_solver_mode_164, 
		file_name, __E_file_name_165, __E_option_name_166, __E_option_value_167, bf_cmd_arg, abs_history_sym, rel_history_sym, selection_sym, execute_sym, snf_sym, 
		examples_sym, __E_help_arg_168, __E___E_help_arg_168_169, rel_history, __E_history_170, __E___E_history_170_171, history_id, abs_history, __E_history_172, unreachable, 
		processed, start_get_stream_def, __N_0, __N_1, __N_2, __N_3, __N_4, __N_5, __N_6, __N_7, 
		__N_8, __N_9, __N_10, __N_11, __N_12, __N_13, __N_14, __N_15, __N_16, 
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
