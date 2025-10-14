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
	"bf_rule", "__E___E_library_17_20", "wff_matcher", "wff_body", "bf_matcher", "bf_body", "tau_constant_source", "__E_tau_constant_source_21", "wff_parenthesis", "__E_wff_22", 
	"wff_sometimes", "__E_wff_23", "__E___E_wff_23_24", "wff_always", "__E_wff_25", "__E___E_wff_25_26", "wff_conditional", "__E_wff_27", "wff_all", "__E_wff_28", 
	"q_vars", "wff_ex", "__E_wff_29", "wff_ref", "constraint", "wff_imply", "__E_wff_30", "wff_rimply", "__E_wff_31", "wff_equiv", 
	"__E_wff_32", "wff_or", "__E_wff_33", "wff_xor", "__E_wff_34", "wff_and", "__E_wff_35", "wff_neg", "__E_wff_36", "wff_t", 
	"wff_f", "bf_interval", "__E_wff_37", "bf_eq", "__E_wff_38", "bf_neq", "__E_wff_39", "bf_lteq", "__E_wff_40", "bf_nlteq", 
	"__E_wff_41", "bf_gt", "__E_wff_42", "bf_ngt", "__E_wff_43", "bf_gteq", "__E_wff_44", "bf_ngteq", "__E_wff_45", "bf_lt", 
	"__E_wff_46", "bf_nlt", "__E_wff_47", "bv_eq", "__E_wff_48", "bv", "bv_neq", "__E_wff_49", "bv_lteq", "__E_wff_50", 
	"bv_nlteq", "__E_wff_51", "bv_gt", "__E_wff_52", "bv_ngt", "__E_wff_53", "bv_gteq", "__E_wff_54", "bv_ngteq", "__E_wff_55", 
	"bv_lt", "__E_wff_56", "bv_nlt", "__E_wff_57", "bf_parenthesis", "__E_bf_58", "bf_constant", "variable", "bf_fall", "__E_bf_59", 
	"bf_fex", "__E_bf_60", "bf_ref", "bf_or", "__E_bf_61", "bf_xor", "__E_bf_62", "bf_t", "__E_bf_63", "__E___E_bf_63_64", 
	"bf_f", "__E_bf_65", "__E___E_bf_65_66", "bf_and", "__E_bf_67", "__E___E_bf_67_68", "bf_neg", "__E_bf_69", "bf_neg_oprnd", "__E___E_bf_69_70", 
	"bf_and_nosep", "__E_bf_71", "bf_and_nosep_1st_oprnd", "__E___E_bf_71_72", "bf_and_nosep_2nd_oprnd", "__E___E_bf_71_73", "bv_parenthesis", "__E_bv_74", "bv_checked", "__E_bv_75", 
	"bv_add", "__E_bv_76", "bv_mul", "__E_bv_77", "bv_div", "__E_bv_78", "bv_mod", "__E_bv_79", "bv_sub", "__E_bv_80", 
	"bv_neg", "bv_and", "__E_bv_81", "bv_nand", "__E_bv_82", "bv_or", "__E_bv_83", "bv_nor", "__E_bv_84", "bv_xor", 
	"__E_bv_85", "bv_xnor", "__E_bv_86", "bv_shl", "__E_bv_87", "bv_shr", "__E_bv_88", "bv_min", "__E_bv_89", "bv_max", 
	"__E_bv_90", "bv_constant", "binary", "__E_bv_constant_91", "decimal", "__E_bv_constant_92", "hexadecimal", "__E_bv_constant_93", "__E_binary_94", "__E_binary_95", 
	"__E_hexadecimal_96", "__E_decimal_97", "ctn_neq", "__E_constraint_98", "ctnvar", "num", "ctn_eq", "__E_constraint_99", "ctn_gteq", "__E_constraint_100", 
	"ctn_gt", "__E_constraint_101", "ctn_lteq", "__E_constraint_102", "ctn_lt", "__E_constraint_103", "__E_bf_constant_104", "source", "__E_bf_constant_105", "src_c", 
	"__E_source_106", "__E___E_source_106_107", "__E___E_source_106_108", "__E_src_c_109", "__E_src_c_110", "__E_variable_111", "uconst", "io_var", "var_name", "__E_variable_112", 
	"__E_var_name_113", "__E_var_name_114", "__E_var_name_115", "__E_var_name_116", "__E_var_name_117", "offset", "chars", "uconst_name", "__E_uconst_118", "__E___E_uconst_118_119", 
	"q_var", "__E_q_vars_120", "__E_q_vars_121", "__E_offsets_122", "__E_offsets_123", "integer", "shift", "__E_offset_124", "__E_shift_125", "__E___E_shift_125_126", 
	"__E_chars_127", "__E_chars_128", "digits", "__E_digits_129", "__E_integer_130", "type", "__E_typed_131", "subtype", "scope_id", "comment", 
	"__E_comment_132", "__E_comment_133", "__E_comment_134", "cli", "cli_command", "__E_cli_135", "__E_cli_136", "quit_cmd", "quit_sym", "__E_cli_command_137", 
	"version_cmd", "version_sym", "__E_cli_command_138", "clear_cmd", "clear_sym", "__E_cli_command_139", "help_cmd", "__E_cli_command_140", "help_sym", "__E___E_cli_command_140_141", 
	"__E___E_cli_command_140_142", "help_arg", "file_cmd", "__E_cli_command_143", "file_sym", "q_string", "valid_cmd", "__E_cli_command_144", "valid_sym", "normalize_cmd_arg", 
	"sat_cmd", "__E_cli_command_145", "sat_sym", "unsat_cmd", "__E_cli_command_146", "unsat_sym", "solve_cmd", "__E_cli_command_147", "solve_sym", "__E___E_cli_command_147_148", 
	"solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_149", "lgrs_sym", "run_cmd", "__E_cli_command_150", "run_sym", "__E___E_cli_command_150_151", "__E___E_cli_command_150_152", 
	"history", "normalize_cmd", "__E_cli_command_153", "normalize_sym", "__E___E_cli_command_153_154", "subst_cmd", "__E_cli_command_155", "subst_sym", "__E___E_cli_command_155_156", "nf_cmd_arg", 
	"inst_cmd", "__E_cli_command_157", "inst_sym", "__E___E_cli_command_157_158", "inst_args", "dnf_cmd", "__E_cli_command_159", "dnf_sym", "cnf_cmd", "__E_cli_command_160", 
	"cnf_sym", "anf_cmd", "__E_cli_command_161", "anf_sym", "nnf_cmd", "__E_cli_command_162", "nnf_sym", "pnf_cmd", "__E_cli_command_163", "pnf_sym", 
	"mnf_cmd", "__E_cli_command_164", "mnf_sym", "snf_cmd", "__E_cli_command_165", "snf_sym", "onf_cmd", "__E_cli_command_166", "onf_sym", "onf_cmd_arg", 
	"qelim_cmd", "__E_cli_command_167", "qelim_sym", "get_cmd", "__E_cli_command_168", "get_sym", "__E___E_cli_command_168_169", "option_name", "set_cmd", "__E_cli_command_170", 
	"set_sym", "__E___E_cli_command_170_171", "option_value", "enable_cmd", "__E_cli_command_172", "enable_sym", "disable_cmd", "__E_cli_command_173", "disable_sym", "toggle_cmd", 
	"__E_cli_command_174", "toggle_sym", "def_list_cmd", "__E_cli_command_175", "def_sym", "__E___E_cli_command_175_176", "def_print_cmd", "__E_cli_command_177", "def_rr_cmd", "def_input_cmd", 
	"def_output_cmd", "history_list_cmd", "__E_cli_command_178", "history_sym", "__E___E_cli_command_178_179", "history_print_cmd", "__E_cli_command_180", "history_store_cmd", "__E_cli_command_181", "__E_solve_options_182", 
	"solve_option", "__E_solve_options_183", "__E_solve_option_184", "solver_mode", "solver_mode_minimum", "__E_solver_mode_185", "solver_mode_maximum", "__E_solver_mode_186", "file_name", "__E_file_name_187", 
	"__E_option_name_188", "__E_option_value_189", "bf_cmd_arg", "abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", "examples_sym", "__E_help_arg_190", "__E___E_help_arg_190_191", 
	"rel_history", "__E_history_192", "__E___E_history_192_193", "history_id", "abs_history", "__E_history_194", "__N_0", "__N_1", "__N_2", "__N_3", 
	"__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
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
			29, 31, 40, 212, 214, 216, 224, 237, 248, 257,
			265, 272, 275, 277, 377, 382, 418
		},
		.to_inline = {
			{ 14, 68, 14 },
			{ 30, 144, 30 },
			{ 33 },
			{ 125, 176, 125 },
			{ 168, 144, 30 },
			{ 170, 144, 163 },
			{ 172, 144, 30 },
			{ 174, 144, 30 },
			{ 239 },
			{ 246 },
			{ 246, 257 },
			{ 256 },
			{ 260 },
			{ 309 },
			{ 321 },
			{ 339 },
			{ 344 },
			{ 369 },
			{ 422 }
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
//G74:  __E_tau_constant_source_21(67) => '.' _(11).
	p(NT(67), (T(1)+NT(11)));
//G75:  __E_tau_constant_source_21(67) => null.
	p(NT(67), (nul));
//G76:  tau_constant_source(66) => rec_relations(24) _(11) main(13) _(11) __E_tau_constant_source_21(67).
	p(NT(66), (NT(24)+NT(11)+NT(13)+NT(11)+NT(67)));
//G77:  __E_wff_22(69)       => '(' _(11) wff(14) _(11) ')'.
	p(NT(69), (T(12)+NT(11)+NT(14)+NT(11)+T(13)));
//G78:  wff_parenthesis(68)  => __E_wff_22(69).
	p(NT(68), (NT(69)));
//G79:  wff(14)              => wff_parenthesis(68).
	p(NT(14), (NT(68)));
//G80:  __E___E_wff_23_24(72) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(72), (T(15)+T(6)+T(21)+T(11)+T(8)+T(4)+T(21)+T(11)+T(15)));
//G81:  __E___E_wff_23_24(72) => '<' '>'.
	p(NT(72), (T(22)+T(23)));
//G82:  __E_wff_23(71)       => __E___E_wff_23_24(72) _(11) wff(14).
	p(NT(71), (NT(72)+NT(11)+NT(14)));
//G83:  wff_sometimes(70)    => __E_wff_23(71).
	p(NT(70), (NT(71)));
//G84:  wff(14)              => wff_sometimes(70).
	p(NT(14), (NT(70)));
//G85:  __E___E_wff_25_26(75) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(75), (T(16)+T(10)+T(24)+T(16)+T(25)+T(15)));
//G86:  __E___E_wff_25_26(75) => '[' ']'.
	p(NT(75), (T(26)+T(27)));
//G87:  __E_wff_25(74)       => __E___E_wff_25_26(75) _(11) wff(14).
	p(NT(74), (NT(75)+NT(11)+NT(14)));
//G88:  wff_always(73)       => __E_wff_25(74).
	p(NT(73), (NT(74)));
//G89:  wff(14)              => wff_always(73).
	p(NT(14), (NT(73)));
//G90:  __E_wff_27(77)       => wff(14) _(11) '?' _(11) wff(14) _(11) ':' _(11) wff(14).
	p(NT(77), (NT(14)+NT(11)+T(28)+NT(11)+NT(14)+NT(11)+T(2)+NT(11)+NT(14)));
//G91:  wff_conditional(76)  => __E_wff_27(77).
	p(NT(76), (NT(77)));
//G92:  wff(14)              => wff_conditional(76).
	p(NT(14), (NT(76)));
//G93:  __E_wff_28(79)       => 'a' 'l' 'l' __(34) q_vars(80) __(34) wff(14).
	p(NT(79), (T(16)+T(10)+T(10)+NT(34)+NT(80)+NT(34)+NT(14)));
//G94:  wff_all(78)          => __E_wff_28(79).
	p(NT(78), (NT(79)));
//G95:  wff(14)              => wff_all(78).
	p(NT(14), (NT(78)));
//G96:  __E_wff_29(82)       => 'e' 'x' __(34) q_vars(80) __(34) wff(14).
	p(NT(82), (T(11)+T(29)+NT(34)+NT(80)+NT(34)+NT(14)));
//G97:  wff_ex(81)           => __E_wff_29(82).
	p(NT(81), (NT(82)));
//G98:  wff(14)              => wff_ex(81).
	p(NT(14), (NT(81)));
//G99:  wff_ref(83)          => ref(27).
	p(NT(83), (NT(27)));
//G100: wff(14)              => wff_ref(83).
	p(NT(14), (NT(83)));
//G101: wff(14)              => constraint(84).
	p(NT(14), (NT(84)));
//G102: __E_wff_30(86)       => wff(14) _(11) '-' '>' _(11) wff(14).
	p(NT(86), (NT(14)+NT(11)+T(30)+T(23)+NT(11)+NT(14)));
//G103: wff_imply(85)        => __E_wff_30(86).
	p(NT(85), (NT(86)));
//G104: wff(14)              => wff_imply(85).
	p(NT(14), (NT(85)));
//G105: __E_wff_31(88)       => wff(14) _(11) '<' '-' _(11) wff(14).
	p(NT(88), (NT(14)+NT(11)+T(22)+T(30)+NT(11)+NT(14)));
//G106: wff_rimply(87)       => __E_wff_31(88).
	p(NT(87), (NT(88)));
//G107: wff(14)              => wff_rimply(87).
	p(NT(14), (NT(87)));
//G108: __E_wff_32(90)       => wff(14) _(11) '<' '-' '>' _(11) wff(14).
	p(NT(90), (NT(14)+NT(11)+T(22)+T(30)+T(23)+NT(11)+NT(14)));
//G109: wff_equiv(89)        => __E_wff_32(90).
	p(NT(89), (NT(90)));
//G110: wff(14)              => wff_equiv(89).
	p(NT(14), (NT(89)));
//G111: __E_wff_33(92)       => wff(14) _(11) '|' '|' _(11) wff(14).
	p(NT(92), (NT(14)+NT(11)+T(31)+T(31)+NT(11)+NT(14)));
//G112: wff_or(91)           => __E_wff_33(92).
	p(NT(91), (NT(92)));
//G113: wff(14)              => wff_or(91).
	p(NT(14), (NT(91)));
//G114: __E_wff_34(94)       => wff(14) _(11) '^' _(11) wff(14).
	p(NT(94), (NT(14)+NT(11)+T(32)+NT(11)+NT(14)));
//G115: wff_xor(93)          => __E_wff_34(94).
	p(NT(93), (NT(94)));
//G116: wff(14)              => wff_xor(93).
	p(NT(14), (NT(93)));
//G117: __E_wff_35(96)       => wff(14) _(11) '&' '&' _(11) wff(14).
	p(NT(96), (NT(14)+NT(11)+T(33)+T(33)+NT(11)+NT(14)));
//G118: wff_and(95)          => __E_wff_35(96).
	p(NT(95), (NT(96)));
//G119: wff(14)              => wff_and(95).
	p(NT(14), (NT(95)));
//G120: __E_wff_36(98)       => '!' _(11) wff(14).
	p(NT(98), (T(34)+NT(11)+NT(14)));
//G121: wff_neg(97)          => __E_wff_36(98).
	p(NT(97), (NT(98)));
//G122: wff(14)              => wff_neg(97).
	p(NT(14), (NT(97)));
//G123: wff_t(99)            => 'T'.
	p(NT(99), (T(35)));
//G124: wff(14)              => wff_t(99).
	p(NT(14), (NT(99)));
//G125: wff_f(100)           => 'F'.
	p(NT(100), (T(36)));
//G126: wff(14)              => wff_f(100).
	p(NT(14), (NT(100)));
//G127: __E_wff_37(102)      => bf(30) _(11) '<' '=' _(11) bf(30) _(11) '<' '=' _(11) bf(30).
	p(NT(102), (NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)));
//G128: bf_interval(101)     => __E_wff_37(102).
	p(NT(101), (NT(102)));
//G129: wff(14)              => bf_interval(101).
	p(NT(14), (NT(101)));
//G130: __E_wff_38(104)      => bf(30) _(11) '=' _(11) bf(30).
	p(NT(104), (NT(30)+NT(11)+T(3)+NT(11)+NT(30)));
//G131: bf_eq(103)           => __E_wff_38(104).
	p(NT(103), (NT(104)));
//G132: wff(14)              => bf_eq(103).
	p(NT(14), (NT(103)));
//G133: __E_wff_39(106)      => bf(30) _(11) '!' '=' _(11) bf(30).
	p(NT(106), (NT(30)+NT(11)+T(34)+T(3)+NT(11)+NT(30)));
//G134: bf_neq(105)          => __E_wff_39(106).
	p(NT(105), (NT(106)));
//G135: wff(14)              => bf_neq(105).
	p(NT(14), (NT(105)));
//G136: __E_wff_40(108)      => bf(30) _(11) '<' '=' _(11) bf(30).
	p(NT(108), (NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)));
//G137: bf_lteq(107)         => __E_wff_40(108).
	p(NT(107), (NT(108)));
//G138: wff(14)              => bf_lteq(107).
	p(NT(14), (NT(107)));
//G139: __E_wff_41(110)      => bf(30) _(11) '!' '<' '=' _(11) bf(30).
	p(NT(110), (NT(30)+NT(11)+T(34)+T(22)+T(3)+NT(11)+NT(30)));
//G140: bf_nlteq(109)        => __E_wff_41(110).
	p(NT(109), (NT(110)));
//G141: wff(14)              => bf_nlteq(109).
	p(NT(14), (NT(109)));
//G142: __E_wff_42(112)      => bf(30) _(11) '>' _(11) bf(30).
	p(NT(112), (NT(30)+NT(11)+T(23)+NT(11)+NT(30)));
//G143: bf_gt(111)           => __E_wff_42(112).
	p(NT(111), (NT(112)));
//G144: wff(14)              => bf_gt(111).
	p(NT(14), (NT(111)));
//G145: __E_wff_43(114)      => bf(30) _(11) '!' '>' _(11) bf(30).
	p(NT(114), (NT(30)+NT(11)+T(34)+T(23)+NT(11)+NT(30)));
//G146: bf_ngt(113)          => __E_wff_43(114).
	p(NT(113), (NT(114)));
//G147: wff(14)              => bf_ngt(113).
	p(NT(14), (NT(113)));
//G148: __E_wff_44(116)      => bf(30) _(11) '>' '=' _(11) bf(30).
	p(NT(116), (NT(30)+NT(11)+T(23)+T(3)+NT(11)+NT(30)));
//G149: bf_gteq(115)         => __E_wff_44(116).
	p(NT(115), (NT(116)));
//G150: wff(14)              => bf_gteq(115).
	p(NT(14), (NT(115)));
//G151: __E_wff_45(118)      => bf(30) _(11) '!' '>' '=' _(11) bf(30).
	p(NT(118), (NT(30)+NT(11)+T(34)+T(23)+T(3)+NT(11)+NT(30)));
//G152: bf_ngteq(117)        => __E_wff_45(118).
	p(NT(117), (NT(118)));
//G153: wff(14)              => bf_ngteq(117).
	p(NT(14), (NT(117)));
//G154: __E_wff_46(120)      => bf(30) _(11) '<' _(11) bf(30).
	p(NT(120), (NT(30)+NT(11)+T(22)+NT(11)+NT(30)));
//G155: bf_lt(119)           => __E_wff_46(120).
	p(NT(119), (NT(120)));
//G156: wff(14)              => bf_lt(119).
	p(NT(14), (NT(119)));
//G157: __E_wff_47(122)      => bf(30) _(11) '!' '<' _(11) bf(30).
	p(NT(122), (NT(30)+NT(11)+T(34)+T(22)+NT(11)+NT(30)));
//G158: bf_nlt(121)          => __E_wff_47(122).
	p(NT(121), (NT(122)));
//G159: wff(14)              => bf_nlt(121).
	p(NT(14), (NT(121)));
//G160: __E_wff_48(124)      => bv(125) _(11) '=' '_' _(11) bv(125).
	p(NT(124), (NT(125)+NT(11)+T(3)+T(37)+NT(11)+NT(125)));
//G161: bv_eq(123)           => __E_wff_48(124).
	p(NT(123), (NT(124)));
//G162: wff(14)              => bv_eq(123).
	p(NT(14), (NT(123)));
//G163: __E_wff_49(127)      => bv(125) _(11) '!' '=' '_' _(11) bv(125).
	p(NT(127), (NT(125)+NT(11)+T(34)+T(3)+T(37)+NT(11)+NT(125)));
//G164: bv_neq(126)          => __E_wff_49(127).
	p(NT(126), (NT(127)));
//G165: wff(14)              => bv_neq(126).
	p(NT(14), (NT(126)));
//G166: __E_wff_50(129)      => bv(125) _(11) '<' '=' '_' _(11) bv(125).
	p(NT(129), (NT(125)+NT(11)+T(22)+T(3)+T(37)+NT(11)+NT(125)));
//G167: bv_lteq(128)         => __E_wff_50(129).
	p(NT(128), (NT(129)));
//G168: wff(14)              => bv_lteq(128).
	p(NT(14), (NT(128)));
//G169: __E_wff_51(131)      => bv(125) _(11) '!' '<' '=' '_' _(11) bv(125).
	p(NT(131), (NT(125)+NT(11)+T(34)+T(22)+T(3)+T(37)+NT(11)+NT(125)));
//G170: bv_nlteq(130)        => __E_wff_51(131).
	p(NT(130), (NT(131)));
//G171: wff(14)              => bv_nlteq(130).
	p(NT(14), (NT(130)));
//G172: __E_wff_52(133)      => bv(125) _(11) '>' '_' _(11) bv(125).
	p(NT(133), (NT(125)+NT(11)+T(23)+T(37)+NT(11)+NT(125)));
//G173: bv_gt(132)           => __E_wff_52(133).
	p(NT(132), (NT(133)));
//G174: wff(14)              => bv_gt(132).
	p(NT(14), (NT(132)));
//G175: __E_wff_53(135)      => bv(125) _(11) '!' '>' '_' _(11) bv(125).
	p(NT(135), (NT(125)+NT(11)+T(34)+T(23)+T(37)+NT(11)+NT(125)));
//G176: bv_ngt(134)          => __E_wff_53(135).
	p(NT(134), (NT(135)));
//G177: wff(14)              => bv_ngt(134).
	p(NT(14), (NT(134)));
//G178: __E_wff_54(137)      => bv(125) _(11) '>' '=' '_' _(11) bv(125).
	p(NT(137), (NT(125)+NT(11)+T(23)+T(3)+T(37)+NT(11)+NT(125)));
//G179: bv_gteq(136)         => __E_wff_54(137).
	p(NT(136), (NT(137)));
//G180: wff(14)              => bv_gteq(136).
	p(NT(14), (NT(136)));
//G181: __E_wff_55(139)      => bv(125) _(11) '!' '>' '=' '_' _(11) bv(125).
	p(NT(139), (NT(125)+NT(11)+T(34)+T(23)+T(3)+T(37)+NT(11)+NT(125)));
//G182: bv_ngteq(138)        => __E_wff_55(139).
	p(NT(138), (NT(139)));
//G183: wff(14)              => bv_ngteq(138).
	p(NT(14), (NT(138)));
//G184: __E_wff_56(141)      => bv(125) _(11) '<' '_' _(11) bv(125).
	p(NT(141), (NT(125)+NT(11)+T(22)+T(37)+NT(11)+NT(125)));
//G185: bv_lt(140)           => __E_wff_56(141).
	p(NT(140), (NT(141)));
//G186: wff(14)              => bv_lt(140).
	p(NT(14), (NT(140)));
//G187: __E_wff_57(143)      => bv(125) _(11) '!' '<' '_' _(11) bv(125).
	p(NT(143), (NT(125)+NT(11)+T(34)+T(22)+T(37)+NT(11)+NT(125)));
//G188: bv_nlt(142)          => __E_wff_57(143).
	p(NT(142), (NT(143)));
//G189: wff(14)              => bv_nlt(142).
	p(NT(14), (NT(142)));
//G190: wff(14)              => capture(29).
	p(NT(14), (NT(29)));
//G191: __E_bf_58(145)       => '(' _(11) bf(30) _(11) ')'.
	p(NT(145), (T(12)+NT(11)+NT(30)+NT(11)+T(13)));
//G192: bf_parenthesis(144)  => __E_bf_58(145).
	p(NT(144), (NT(145)));
//G193: bf(30)               => bf_parenthesis(144).
	p(NT(30), (NT(144)));
//G194: bf(30)               => bf_constant(146).
	p(NT(30), (NT(146)));
//G195: bf(30)               => variable(147).
	p(NT(30), (NT(147)));
//G196: __E_bf_59(149)       => 'f' 'a' 'l' 'l' __(34) q_vars(80) __(34) bf(30).
	p(NT(149), (T(9)+T(16)+T(10)+T(10)+NT(34)+NT(80)+NT(34)+NT(30)));
//G197: bf_fall(148)         => __E_bf_59(149).
	p(NT(148), (NT(149)));
//G198: bf(30)               => bf_fall(148).
	p(NT(30), (NT(148)));
//G199: __E_bf_60(151)       => 'f' 'e' 'x' __(34) q_vars(80) __(34) bf(30).
	p(NT(151), (T(9)+T(11)+T(29)+NT(34)+NT(80)+NT(34)+NT(30)));
//G200: bf_fex(150)          => __E_bf_60(151).
	p(NT(150), (NT(151)));
//G201: bf(30)               => bf_fex(150).
	p(NT(30), (NT(150)));
//G202: bf_ref(152)          => ref(27).
	p(NT(152), (NT(27)));
//G203: bf(30)               => bf_ref(152).
	p(NT(30), (NT(152)));
//G204: __E_bf_61(154)       => bf(30) _(11) '|' _(11) bf(30).
	p(NT(154), (NT(30)+NT(11)+T(31)+NT(11)+NT(30)));
//G205: bf_or(153)           => __E_bf_61(154).
	p(NT(153), (NT(154)));
//G206: bf(30)               => bf_or(153).
	p(NT(30), (NT(153)));
//G207: __E_bf_62(156)       => bf(30) _(11) '+' _(11) bf(30).
	p(NT(156), (NT(30)+NT(11)+T(38)+NT(11)+NT(30)));
//G208: bf_xor(155)          => __E_bf_62(156).
	p(NT(155), (NT(156)));
//G209: bf(30)               => bf_xor(155).
	p(NT(30), (NT(155)));
//G210: __E___E_bf_63_64(159) => typed(33).
	p(NT(159), (NT(33)));
//G211: __E___E_bf_63_64(159) => null.
	p(NT(159), (nul));
//G212: __E_bf_63(158)       => '1' __E___E_bf_63_64(159).
	p(NT(158), (T(39)+NT(159)));
//G213: bf_t(157)            => __E_bf_63(158).
	p(NT(157), (NT(158)));
//G214: bf(30)               => bf_t(157).
	p(NT(30), (NT(157)));
//G215: __E___E_bf_65_66(162) => typed(33).
	p(NT(162), (NT(33)));
//G216: __E___E_bf_65_66(162) => null.
	p(NT(162), (nul));
//G217: __E_bf_65(161)       => '0' __E___E_bf_65_66(162).
	p(NT(161), (T(40)+NT(162)));
//G218: bf_f(160)            => __E_bf_65(161).
	p(NT(160), (NT(161)));
//G219: bf(30)               => bf_f(160).
	p(NT(30), (NT(160)));
//G220: __E___E_bf_67_68(165) => _(11) '&' _(11).
	p(NT(165), (NT(11)+T(33)+NT(11)));
//G221: __E___E_bf_67_68(165) => __(34).
	p(NT(165), (NT(34)));
//G222: __E_bf_67(164)       => bf(30) __E___E_bf_67_68(165) bf(30).
	p(NT(164), (NT(30)+NT(165)+NT(30)));
//G223: bf_and(163)          => __E_bf_67(164).
	p(NT(163), (NT(164)));
//G224: bf(30)               => bf_and(163).
	p(NT(30), (NT(163)));
//G225: __E___E_bf_69_70(169) => bf_parenthesis(144).
	p(NT(169), (NT(144)));
//G226: __E___E_bf_69_70(169) => bf_constant(146).
	p(NT(169), (NT(146)));
//G227: __E___E_bf_69_70(169) => variable(147).
	p(NT(169), (NT(147)));
//G228: __E___E_bf_69_70(169) => bf_ref(152).
	p(NT(169), (NT(152)));
//G229: __E___E_bf_69_70(169) => bf_t(157).
	p(NT(169), (NT(157)));
//G230: __E___E_bf_69_70(169) => bf_f(160).
	p(NT(169), (NT(160)));
//G231: __E___E_bf_69_70(169) => bf_neg(166).
	p(NT(169), (NT(166)));
//G232: __E___E_bf_69_70(169) => capture(29).
	p(NT(169), (NT(29)));
//G233: bf_neg_oprnd(168)    => __E___E_bf_69_70(169).
	p(NT(168), (NT(169)));
//G234: __E_bf_69(167)       => bf_neg_oprnd(168) _(11) '\''.
	p(NT(167), (NT(168)+NT(11)+T(41)));
//G235: bf_neg(166)          => __E_bf_69(167).
	p(NT(166), (NT(167)));
//G236: bf(30)               => bf_neg(166).
	p(NT(30), (NT(166)));
//G237: __E___E_bf_71_72(173) => bf_parenthesis(144).
	p(NT(173), (NT(144)));
//G238: __E___E_bf_71_72(173) => variable(147).
	p(NT(173), (NT(147)));
//G239: __E___E_bf_71_72(173) => bf_ref(152).
	p(NT(173), (NT(152)));
//G240: __E___E_bf_71_72(173) => bf_neg(166).
	p(NT(173), (NT(166)));
//G241: __E___E_bf_71_72(173) => bf_and_nosep(170).
	p(NT(173), (NT(170)));
//G242: __E___E_bf_71_72(173) => capture(29).
	p(NT(173), (NT(29)));
//G243: bf_and_nosep_1st_oprnd(172) => __E___E_bf_71_72(173).
	p(NT(172), (NT(173)));
//G244: __E___E_bf_71_73(175) => bf_parenthesis(144).
	p(NT(175), (NT(144)));
//G245: __E___E_bf_71_73(175) => bf_constant(146).
	p(NT(175), (NT(146)));
//G246: __E___E_bf_71_73(175) => variable(147).
	p(NT(175), (NT(147)));
//G247: __E___E_bf_71_73(175) => bf_ref(152).
	p(NT(175), (NT(152)));
//G248: __E___E_bf_71_73(175) => bf_neg(166).
	p(NT(175), (NT(166)));
//G249: __E___E_bf_71_73(175) => capture(29).
	p(NT(175), (NT(29)));
//G250: bf_and_nosep_2nd_oprnd(174) => __E___E_bf_71_73(175).
	p(NT(174), (NT(175)));
//G251: __E_bf_71(171)       => bf_and_nosep_1st_oprnd(172) bf_and_nosep_2nd_oprnd(174).
	p(NT(171), (NT(172)+NT(174)));
//G252: bf_and_nosep(170)    => __E_bf_71(171).
	p(NT(170), (NT(171)));
//G253: bf(30)               => bf_and_nosep(170).
	p(NT(30), (NT(170)));
//G254: bf(30)               => capture(29).
	p(NT(30), (NT(29)));
//G255: __E_bv_74(177)       => '(' _(11) bv(125) _(11) ')'.
	p(NT(177), (T(12)+NT(11)+NT(125)+NT(11)+T(13)));
//G256: bv_parenthesis(176)  => __E_bv_74(177).
	p(NT(176), (NT(177)));
//G257: bv(125)              => bv_parenthesis(176).
	p(NT(125), (NT(176)));
//G258: __E_bv_75(179)       => '[' bv(125) ']'.
	p(NT(179), (T(26)+NT(125)+T(27)));
//G259: bv_checked(178)      => __E_bv_75(179).
	p(NT(178), (NT(179)));
//G260: bv(125)              => bv_checked(178).
	p(NT(125), (NT(178)));
//G261: bv(125)              => variable(147).
	p(NT(125), (NT(147)));
//G262: __E_bv_76(181)       => bv(125) _(11) '+' _(11) bv(125).
	p(NT(181), (NT(125)+NT(11)+T(38)+NT(11)+NT(125)));
//G263: bv_add(180)          => __E_bv_76(181).
	p(NT(180), (NT(181)));
//G264: bv(125)              => bv_add(180).
	p(NT(125), (NT(180)));
//G265: __E_bv_77(183)       => bv(125) _(11) '*' _(11) bv(125).
	p(NT(183), (NT(125)+NT(11)+T(42)+NT(11)+NT(125)));
//G266: bv_mul(182)          => __E_bv_77(183).
	p(NT(182), (NT(183)));
//G267: bv(125)              => bv_mul(182).
	p(NT(125), (NT(182)));
//G268: __E_bv_78(185)       => bv(125) _(11) '/' _(11) bv(125).
	p(NT(185), (NT(125)+NT(11)+T(43)+NT(11)+NT(125)));
//G269: bv_div(184)          => __E_bv_78(185).
	p(NT(184), (NT(185)));
//G270: bv(125)              => bv_div(184).
	p(NT(125), (NT(184)));
//G271: __E_bv_79(187)       => bv(125) _(11) '%' _(11) bv(125).
	p(NT(187), (NT(125)+NT(11)+T(44)+NT(11)+NT(125)));
//G272: bv_mod(186)          => __E_bv_79(187).
	p(NT(186), (NT(187)));
//G273: bv(125)              => bv_mod(186).
	p(NT(125), (NT(186)));
//G274: __E_bv_80(189)       => bv(125) _(11) '-' _(11) bv(125).
	p(NT(189), (NT(125)+NT(11)+T(30)+NT(11)+NT(125)));
//G275: bv_sub(188)          => __E_bv_80(189).
	p(NT(188), (NT(189)));
//G276: bv(125)              => bv_sub(188).
	p(NT(125), (NT(188)));
//G277: bv_neg(190)          => bv(125).
	p(NT(190), (NT(125)));
//G278: bv(125)              => '~' _(11) bv_neg(190).
	p(NT(125), (T(45)+NT(11)+NT(190)));
//G279: __E_bv_81(192)       => bv(125) _(11) '&' _(11) bv(125).
	p(NT(192), (NT(125)+NT(11)+T(33)+NT(11)+NT(125)));
//G280: bv_and(191)          => __E_bv_81(192).
	p(NT(191), (NT(192)));
//G281: bv(125)              => bv_and(191).
	p(NT(125), (NT(191)));
//G282: __E_bv_82(194)       => bv(125) _(11) '!' '&' _(11) bv(125).
	p(NT(194), (NT(125)+NT(11)+T(34)+T(33)+NT(11)+NT(125)));
//G283: bv_nand(193)         => __E_bv_82(194).
	p(NT(193), (NT(194)));
//G284: bv(125)              => bv_nand(193).
	p(NT(125), (NT(193)));
//G285: __E_bv_83(196)       => bv(125) _(11) '|' _(11) bv(125).
	p(NT(196), (NT(125)+NT(11)+T(31)+NT(11)+NT(125)));
//G286: bv_or(195)           => __E_bv_83(196).
	p(NT(195), (NT(196)));
//G287: bv(125)              => bv_or(195).
	p(NT(125), (NT(195)));
//G288: __E_bv_84(198)       => bv(125) _(11) '!' '|' _(11) bv(125).
	p(NT(198), (NT(125)+NT(11)+T(34)+T(31)+NT(11)+NT(125)));
//G289: bv_nor(197)          => __E_bv_84(198).
	p(NT(197), (NT(198)));
//G290: bv(125)              => bv_nor(197).
	p(NT(125), (NT(197)));
//G291: __E_bv_85(200)       => bv(125) _(11) '(' '+' ')' _(11) bv(125).
	p(NT(200), (NT(125)+NT(11)+T(12)+T(38)+T(13)+NT(11)+NT(125)));
//G292: bv_xor(199)          => __E_bv_85(200).
	p(NT(199), (NT(200)));
//G293: bv(125)              => bv_xor(199).
	p(NT(125), (NT(199)));
//G294: __E_bv_86(202)       => bv(125) _(11) '!' '(' '+' ')' _(11) bv(125).
	p(NT(202), (NT(125)+NT(11)+T(34)+T(12)+T(38)+T(13)+NT(11)+NT(125)));
//G295: bv_xnor(201)         => __E_bv_86(202).
	p(NT(201), (NT(202)));
//G296: bv(125)              => bv_xnor(201).
	p(NT(125), (NT(201)));
//G297: __E_bv_87(204)       => bv(125) _(11) '<' '<' _(11) bv(125).
	p(NT(204), (NT(125)+NT(11)+T(22)+T(22)+NT(11)+NT(125)));
//G298: bv_shl(203)          => __E_bv_87(204).
	p(NT(203), (NT(204)));
//G299: bv(125)              => bv_shl(203).
	p(NT(125), (NT(203)));
//G300: __E_bv_88(206)       => bv(125) _(11) '>' '>' _(11) bv(125).
	p(NT(206), (NT(125)+NT(11)+T(23)+T(23)+NT(11)+NT(125)));
//G301: bv_shr(205)          => __E_bv_88(206).
	p(NT(205), (NT(206)));
//G302: bv(125)              => bv_shr(205).
	p(NT(125), (NT(205)));
//G303: __E_bv_89(208)       => 'm' 'i' 'n' __(34) bv(125) __(34) bv(125).
	p(NT(208), (T(21)+T(4)+T(5)+NT(34)+NT(125)+NT(34)+NT(125)));
//G304: bv_min(207)          => __E_bv_89(208).
	p(NT(207), (NT(208)));
//G305: bv(125)              => bv_min(207).
	p(NT(125), (NT(207)));
//G306: __E_bv_90(210)       => 'm' 'a' 'x' __(34) bv(125) __(34) bv(125).
	p(NT(210), (T(21)+T(16)+T(29)+NT(34)+NT(125)+NT(34)+NT(125)));
//G307: bv_max(209)          => __E_bv_90(210).
	p(NT(209), (NT(210)));
//G308: bv(125)              => bv_max(209).
	p(NT(125), (NT(209)));
//G309: bv(125)              => bv_constant(211).
	p(NT(125), (NT(211)));
//G310: __E_bv_constant_91(213) => typed(33).
	p(NT(213), (NT(33)));
//G311: __E_bv_constant_91(213) => null.
	p(NT(213), (nul));
//G312: bv_constant(211)     => '#' 'b' binary(212) __E_bv_constant_91(213).
	p(NT(211), (T(46)+T(17)+NT(212)+NT(213)));
//G313: __E_bv_constant_92(215) => typed(33).
	p(NT(215), (NT(33)));
//G314: __E_bv_constant_92(215) => null.
	p(NT(215), (nul));
//G315: bv_constant(211)     => decimal(214) __E_bv_constant_92(215).
	p(NT(211), (NT(214)+NT(215)));
//G316: __E_bv_constant_93(217) => typed(33).
	p(NT(217), (NT(33)));
//G317: __E_bv_constant_93(217) => null.
	p(NT(217), (nul));
//G318: bv_constant(211)     => '#' 'x' hexadecimal(216) __E_bv_constant_93(217).
	p(NT(211), (T(46)+T(29)+NT(216)+NT(217)));
//G319: __E_binary_94(218)   => '0'.
	p(NT(218), (T(40)));
//G320: __E_binary_94(218)   => '1'.
	p(NT(218), (T(39)));
//G321: __E_binary_95(219)   => __E_binary_94(218).
	p(NT(219), (NT(218)));
//G322: __E_binary_95(219)   => __E_binary_94(218) __E_binary_95(219).
	p(NT(219), (NT(218)+NT(219)));
//G323: binary(212)          => __E_binary_95(219).
	p(NT(212), (NT(219)));
//G324: __E_hexadecimal_96(220) => xdigit(4).
	p(NT(220), (NT(4)));
//G325: __E_hexadecimal_96(220) => xdigit(4) __E_hexadecimal_96(220).
	p(NT(220), (NT(4)+NT(220)));
//G326: hexadecimal(216)     => __E_hexadecimal_96(220).
	p(NT(216), (NT(220)));
//G327: __E_decimal_97(221)  => digit(3).
	p(NT(221), (NT(3)));
//G328: __E_decimal_97(221)  => digit(3) __E_decimal_97(221).
	p(NT(221), (NT(3)+NT(221)));
//G329: decimal(214)         => __E_decimal_97(221).
	p(NT(214), (NT(221)));
//G330: __E_constraint_98(223) => '[' ctnvar(224) _(11) '!' '=' _(11) num(225) ']'.
	p(NT(223), (T(26)+NT(224)+NT(11)+T(34)+T(3)+NT(11)+NT(225)+T(27)));
//G331: __E_constraint_98(223) => '[' num(225) _(11) '!' '=' _(11) ctnvar(224) ']'.
	p(NT(223), (T(26)+NT(225)+NT(11)+T(34)+T(3)+NT(11)+NT(224)+T(27)));
//G332: ctn_neq(222)         => __E_constraint_98(223).
	p(NT(222), (NT(223)));
//G333: constraint(84)       => ctn_neq(222).
	p(NT(84), (NT(222)));
//G334: __E_constraint_99(227) => '[' ctnvar(224) _(11) '=' _(11) num(225) ']'.
	p(NT(227), (T(26)+NT(224)+NT(11)+T(3)+NT(11)+NT(225)+T(27)));
//G335: __E_constraint_99(227) => '[' num(225) _(11) '=' _(11) ctnvar(224) ']'.
	p(NT(227), (T(26)+NT(225)+NT(11)+T(3)+NT(11)+NT(224)+T(27)));
//G336: ctn_eq(226)          => __E_constraint_99(227).
	p(NT(226), (NT(227)));
//G337: constraint(84)       => ctn_eq(226).
	p(NT(84), (NT(226)));
//G338: __E_constraint_100(229) => '[' ctnvar(224) _(11) '>' '=' _(11) num(225) ']'.
	p(NT(229), (T(26)+NT(224)+NT(11)+T(23)+T(3)+NT(11)+NT(225)+T(27)));
//G339: __E_constraint_100(229) => '[' num(225) _(11) '>' '=' _(11) ctnvar(224) ']'.
	p(NT(229), (T(26)+NT(225)+NT(11)+T(23)+T(3)+NT(11)+NT(224)+T(27)));
//G340: ctn_gteq(228)        => __E_constraint_100(229).
	p(NT(228), (NT(229)));
//G341: constraint(84)       => ctn_gteq(228).
	p(NT(84), (NT(228)));
//G342: __E_constraint_101(231) => '[' ctnvar(224) _(11) '>' _(11) num(225) ']'.
	p(NT(231), (T(26)+NT(224)+NT(11)+T(23)+NT(11)+NT(225)+T(27)));
//G343: __E_constraint_101(231) => '[' num(225) _(11) '>' _(11) ctnvar(224) ']'.
	p(NT(231), (T(26)+NT(225)+NT(11)+T(23)+NT(11)+NT(224)+T(27)));
//G344: ctn_gt(230)          => __E_constraint_101(231).
	p(NT(230), (NT(231)));
//G345: constraint(84)       => ctn_gt(230).
	p(NT(84), (NT(230)));
//G346: __E_constraint_102(233) => '[' ctnvar(224) _(11) '<' '=' _(11) num(225) ']'.
	p(NT(233), (T(26)+NT(224)+NT(11)+T(22)+T(3)+NT(11)+NT(225)+T(27)));
//G347: __E_constraint_102(233) => '[' num(225) _(11) '<' '=' _(11) ctnvar(224) ']'.
	p(NT(233), (T(26)+NT(225)+NT(11)+T(22)+T(3)+NT(11)+NT(224)+T(27)));
//G348: ctn_lteq(232)        => __E_constraint_102(233).
	p(NT(232), (NT(233)));
//G349: constraint(84)       => ctn_lteq(232).
	p(NT(84), (NT(232)));
//G350: __E_constraint_103(235) => '[' ctnvar(224) _(11) '<' _(11) num(225) ']'.
	p(NT(235), (T(26)+NT(224)+NT(11)+T(22)+NT(11)+NT(225)+T(27)));
//G351: __E_constraint_103(235) => '[' num(225) _(11) '<' _(11) ctnvar(224) ']'.
	p(NT(235), (T(26)+NT(225)+NT(11)+T(22)+NT(11)+NT(224)+T(27)));
//G352: ctn_lt(234)          => __E_constraint_103(235).
	p(NT(234), (NT(235)));
//G353: constraint(84)       => ctn_lt(234).
	p(NT(84), (NT(234)));
//G354: __E_bf_constant_104(236) => capture(29).
	p(NT(236), (NT(29)));
//G355: __E_bf_constant_104(236) => source(237).
	p(NT(236), (NT(237)));
//G356: __E_bf_constant_105(238) => typed(33).
	p(NT(238), (NT(33)));
//G357: __E_bf_constant_105(238) => null.
	p(NT(238), (nul));
//G358: bf_constant(146)     => '{' _(11) __E_bf_constant_104(236) _(11) '}' __E_bf_constant_105(238).
	p(NT(146), (T(47)+NT(11)+NT(236)+NT(11)+T(48)+NT(238)));
//G359: __E___E_source_106_107(241) => src_c(239).
	p(NT(241), (NT(239)));
//G360: __E___E_source_106_107(241) => space(2).
	p(NT(241), (NT(2)));
//G361: __E___E_source_106_108(242) => null.
	p(NT(242), (nul));
//G362: __E___E_source_106_108(242) => __E___E_source_106_107(241) __E___E_source_106_108(242).
	p(NT(242), (NT(241)+NT(242)));
//G363: __E_source_106(240)  => __E___E_source_106_108(242) src_c(239).
	p(NT(240), (NT(242)+NT(239)));
//G364: __E_source_106(240)  => null.
	p(NT(240), (nul));
//G365: source(237)          => src_c(239) __E_source_106(240).
	p(NT(237), (NT(239)+NT(240)));
//G366: src_c(239)           => alnum(6).
	p(NT(239), (NT(6)));
//G367: __N_0(436)           => '{'.
	p(NT(436), (T(47)));
//G368: __N_1(437)           => '}'.
	p(NT(437), (T(48)));
//G369: src_c(239)           => ~( __N_0(436) ) & ~( __N_1(437) ) & punct(7).	 # conjunctive
	p(NT(239), ~(NT(436)) & ~(NT(437)) & (NT(7)));
//G370: __E_src_c_109(243)   => src_c(239).
	p(NT(243), (NT(239)));
//G371: __E_src_c_109(243)   => space(2).
	p(NT(243), (NT(2)));
//G372: __E_src_c_110(244)   => null.
	p(NT(244), (nul));
//G373: __E_src_c_110(244)   => __E_src_c_109(243) __E_src_c_110(244).
	p(NT(244), (NT(243)+NT(244)));
//G374: src_c(239)           => '{' __E_src_c_110(244) '}'.
	p(NT(239), (T(47)+NT(244)+T(48)));
//G375: __E_variable_111(245) => uconst(246).
	p(NT(245), (NT(246)));
//G376: __E_variable_111(245) => io_var(247).
	p(NT(245), (NT(247)));
//G377: __E_variable_111(245) => var_name(248).
	p(NT(245), (NT(248)));
//G378: __E_variable_112(249) => typed(33).
	p(NT(249), (NT(33)));
//G379: __E_variable_112(249) => null.
	p(NT(249), (nul));
//G380: variable(147)        => __E_variable_111(245) __E_variable_112(249).
	p(NT(147), (NT(245)+NT(249)));
//G381: __N_2(438)           => 'F'.
	p(NT(438), (T(36)));
//G382: __N_3(439)           => 'T'.
	p(NT(439), (T(35)));
//G383: __E_var_name_113(250) => ~( __N_2(438) ) & ~( __N_3(439) ) & alpha(5).	 # conjunctive
	p(NT(250), ~(NT(438)) & ~(NT(439)) & (NT(5)));
//G384: __E_var_name_114(251) => null.
	p(NT(251), (nul));
//G385: __E_var_name_114(251) => digit(3) __E_var_name_114(251).
	p(NT(251), (NT(3)+NT(251)));
//G386: var_name(248)        => __E_var_name_113(250) __E_var_name_114(251).	 # guarded: charvar
	p(NT(248), (NT(250)+NT(251)));
	p.back().guard = "charvar";
//G387: __N_4(440)           => 'F'.
	p(NT(440), (T(36)));
//G388: __N_5(441)           => 'T'.
	p(NT(441), (T(35)));
//G389: __E_var_name_115(252) => ~( __N_4(440) ) & ~( __N_5(441) ) & alpha(5).	 # conjunctive
	p(NT(252), ~(NT(440)) & ~(NT(441)) & (NT(5)));
//G390: __E_var_name_116(253) => alnum(6).
	p(NT(253), (NT(6)));
//G391: __E_var_name_116(253) => '_'.
	p(NT(253), (T(37)));
//G392: __E_var_name_117(254) => null.
	p(NT(254), (nul));
//G393: __E_var_name_117(254) => __E_var_name_116(253) __E_var_name_117(254).
	p(NT(254), (NT(253)+NT(254)));
//G394: var_name(248)        => __E_var_name_115(252) __E_var_name_117(254).	 # guarded: var
	p(NT(248), (NT(252)+NT(254)));
	p.back().guard = "var";
//G395: io_var(247)          => io_var_name(31) '[' offset(255) ']'.
	p(NT(247), (NT(31)+T(26)+NT(255)+T(27)));
//G396: io_var_name(31)      => chars(256).
	p(NT(31), (NT(256)));
//G397: __E___E_uconst_118_119(259) => chars(256) _(11).
	p(NT(259), (NT(256)+NT(11)));
//G398: __E___E_uconst_118_119(259) => null.
	p(NT(259), (nul));
//G399: __E_uconst_118(258)  => __E___E_uconst_118_119(259) ':' _(11) chars(256).
	p(NT(258), (NT(259)+T(2)+NT(11)+NT(256)));
//G400: uconst_name(257)     => __E_uconst_118(258).
	p(NT(257), (NT(258)));
//G401: uconst(246)          => '<' _(11) uconst_name(257) _(11) '>'.
	p(NT(246), (T(22)+NT(11)+NT(257)+NT(11)+T(23)));
//G402: __E_q_vars_120(261)  => _(11) ',' _(11) q_var(260).
	p(NT(261), (NT(11)+T(20)+NT(11)+NT(260)));
//G403: __E_q_vars_121(262)  => null.
	p(NT(262), (nul));
//G404: __E_q_vars_121(262)  => __E_q_vars_120(261) __E_q_vars_121(262).
	p(NT(262), (NT(261)+NT(262)));
//G405: q_vars(80)           => q_var(260) __E_q_vars_121(262).
	p(NT(80), (NT(260)+NT(262)));
//G406: q_var(260)           => capture(29).
	p(NT(260), (NT(29)));
//G407: __N_6(442)           => uconst(246).
	p(NT(442), (NT(246)));
//G408: q_var(260)           => variable(147) & ~( __N_6(442) ).	 # conjunctive
	p(NT(260), (NT(147)) & ~(NT(442)));
//G409: ctnvar(224)          => chars(256).
	p(NT(224), (NT(256)));
//G410: __E_offsets_122(263) => _(11) ',' _(11) offset(255).
	p(NT(263), (NT(11)+T(20)+NT(11)+NT(255)));
//G411: __E_offsets_123(264) => null.
	p(NT(264), (nul));
//G412: __E_offsets_123(264) => __E_offsets_122(263) __E_offsets_123(264).
	p(NT(264), (NT(263)+NT(264)));
//G413: offsets(42)          => '[' _(11) offset(255) __E_offsets_123(264) _(11) ']'.
	p(NT(42), (T(26)+NT(11)+NT(255)+NT(264)+NT(11)+T(27)));
//G414: offset(255)          => integer(265).
	p(NT(255), (NT(265)));
//G415: offset(255)          => capture(29).
	p(NT(255), (NT(29)));
//G416: offset(255)          => shift(266).
	p(NT(255), (NT(266)));
//G417: __N_7(443)           => io_var(247).
	p(NT(443), (NT(247)));
//G418: __E_offset_124(267)  => variable(147) & ~( __N_7(443) ).	 # conjunctive
	p(NT(267), (NT(147)) & ~(NT(443)));
//G419: offset(255)          => __E_offset_124(267).
	p(NT(255), (NT(267)));
//G420: __E_shift_125(268)   => capture(29).
	p(NT(268), (NT(29)));
//G421: __N_8(444)           => io_var(247).
	p(NT(444), (NT(247)));
//G422: __E___E_shift_125_126(269) => variable(147) & ~( __N_8(444) ).	 # conjunctive
	p(NT(269), (NT(147)) & ~(NT(444)));
//G423: __E_shift_125(268)   => __E___E_shift_125_126(269).
	p(NT(268), (NT(269)));
//G424: shift(266)           => __E_shift_125(268) _(11) '-' _(11) num(225).
	p(NT(266), (NT(268)+NT(11)+T(30)+NT(11)+NT(225)));
//G425: __E_chars_127(270)   => alnum(6).
	p(NT(270), (NT(6)));
//G426: __E_chars_127(270)   => '_'.
	p(NT(270), (T(37)));
//G427: __E_chars_128(271)   => null.
	p(NT(271), (nul));
//G428: __E_chars_128(271)   => __E_chars_127(270) __E_chars_128(271).
	p(NT(271), (NT(270)+NT(271)));
//G429: chars(256)           => alpha(5) __E_chars_128(271).
	p(NT(256), (NT(5)+NT(271)));
//G430: __E_digits_129(273)  => digit(3).
	p(NT(273), (NT(3)));
//G431: __E_digits_129(273)  => digit(3) __E_digits_129(273).
	p(NT(273), (NT(3)+NT(273)));
//G432: digits(272)          => __E_digits_129(273).
	p(NT(272), (NT(273)));
//G433: num(225)             => digits(272).
	p(NT(225), (NT(272)));
//G434: __E_integer_130(274) => '-'.
	p(NT(274), (T(30)));
//G435: __E_integer_130(274) => null.
	p(NT(274), (nul));
//G436: integer(265)         => __E_integer_130(274) _(11) digits(272).
	p(NT(265), (NT(274)+NT(11)+NT(272)));
//G437: sym(40)              => chars(256).
	p(NT(40), (NT(256)));
//G438: capture(29)          => '$' chars(256).
	p(NT(29), (T(49)+NT(256)));
//G439: __E_typed_131(276)   => _(11) '[' _(11) subtype(277) _(11) ']' _(11).
	p(NT(276), (NT(11)+T(26)+NT(11)+NT(277)+NT(11)+T(27)+NT(11)));
//G440: __E_typed_131(276)   => null.
	p(NT(276), (nul));
//G441: typed(33)            => _(11) ':' _(11) type(275) __E_typed_131(276).
	p(NT(33), (NT(11)+T(2)+NT(11)+NT(275)+NT(276)));
//G442: type(275)            => chars(256).
	p(NT(275), (NT(256)));
//G443: subtype(277)         => num(225).
	p(NT(277), (NT(225)));
//G444: scope_id(278)        => digits(272).
	p(NT(278), (NT(272)));
//G445: __E_comment_132(280) => printable(8).
	p(NT(280), (NT(8)));
//G446: __E_comment_132(280) => '\t'.
	p(NT(280), (T(50)));
//G447: __E_comment_133(281) => null.
	p(NT(281), (nul));
//G448: __E_comment_133(281) => __E_comment_132(280) __E_comment_133(281).
	p(NT(281), (NT(280)+NT(281)));
//G449: __E_comment_134(282) => '\n'.
	p(NT(282), (T(51)));
//G450: __E_comment_134(282) => '\r'.
	p(NT(282), (T(52)));
//G451: __E_comment_134(282) => eof(1).
	p(NT(282), (NT(1)));
//G452: comment(279)         => '#' __E_comment_133(281) __E_comment_134(282).
	p(NT(279), (T(46)+NT(281)+NT(282)));
//G453: __(34)               => space(2).
	p(NT(34), (NT(2)));
//G454: __(34)               => comment(279).
	p(NT(34), (NT(279)));
//G455: __(34)               => __(34) space(2).
	p(NT(34), (NT(34)+NT(2)));
//G456: __(34)               => __(34) comment(279).
	p(NT(34), (NT(34)+NT(279)));
//G457: _(11)                => __(34).
	p(NT(11), (NT(34)));
//G458: _(11)                => null.
	p(NT(11), (nul));
//G459: cli(283)             => _(11).
	p(NT(283), (NT(11)));
//G460: __E_cli_135(285)     => '.' _(11) cli_command(284) _(11).
	p(NT(285), (T(1)+NT(11)+NT(284)+NT(11)));
//G461: __E_cli_136(286)     => null.
	p(NT(286), (nul));
//G462: __E_cli_136(286)     => __E_cli_135(285) __E_cli_136(286).
	p(NT(286), (NT(285)+NT(286)));
//G463: cli(283)             => _(11) cli_command(284) _(11) __E_cli_136(286).
	p(NT(283), (NT(11)+NT(284)+NT(11)+NT(286)));
//G464: __E_cli_command_137(289) => 'q'.
	p(NT(289), (T(53)));
//G465: __E_cli_command_137(289) => 'q' 'u' 'i' 't'.
	p(NT(289), (T(53)+T(7)+T(4)+T(8)));
//G466: quit_sym(288)        => __E_cli_command_137(289).
	p(NT(288), (NT(289)));
//G467: quit_cmd(287)        => quit_sym(288).
	p(NT(287), (NT(288)));
//G468: cli_command(284)     => quit_cmd(287).
	p(NT(284), (NT(287)));
//G469: __E_cli_command_138(292) => 'v'.
	p(NT(292), (T(54)));
//G470: __E_cli_command_138(292) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(292), (T(54)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G471: version_sym(291)     => __E_cli_command_138(292).
	p(NT(291), (NT(292)));
//G472: version_cmd(290)     => version_sym(291).
	p(NT(290), (NT(291)));
//G473: cli_command(284)     => version_cmd(290).
	p(NT(284), (NT(290)));
//G474: __E_cli_command_139(295) => 'c'.
	p(NT(295), (T(14)));
//G475: __E_cli_command_139(295) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(295), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G476: clear_sym(294)       => __E_cli_command_139(295).
	p(NT(294), (NT(295)));
//G477: clear_cmd(293)       => clear_sym(294).
	p(NT(293), (NT(294)));
//G478: cli_command(284)     => clear_cmd(293).
	p(NT(284), (NT(293)));
//G479: __E___E_cli_command_140_141(299) => 'h'.
	p(NT(299), (T(55)));
//G480: __E___E_cli_command_140_141(299) => 'h' 'e' 'l' 'p'.
	p(NT(299), (T(55)+T(11)+T(10)+T(56)));
//G481: help_sym(298)        => __E___E_cli_command_140_141(299).
	p(NT(298), (NT(299)));
//G482: __E___E_cli_command_140_142(300) => __(34) help_arg(301).
	p(NT(300), (NT(34)+NT(301)));
//G483: __E___E_cli_command_140_142(300) => null.
	p(NT(300), (nul));
//G484: __E_cli_command_140(297) => help_sym(298) __E___E_cli_command_140_142(300).
	p(NT(297), (NT(298)+NT(300)));
//G485: help_cmd(296)        => __E_cli_command_140(297).
	p(NT(296), (NT(297)));
//G486: cli_command(284)     => help_cmd(296).
	p(NT(284), (NT(296)));
//G487: file_sym(304)        => 'f' 'i' 'l' 'e'.
	p(NT(304), (T(9)+T(4)+T(10)+T(11)));
//G488: __E_cli_command_143(303) => file_sym(304) __(34) q_string(305).
	p(NT(303), (NT(304)+NT(34)+NT(305)));
//G489: file_cmd(302)        => __E_cli_command_143(303).
	p(NT(302), (NT(303)));
//G490: cli_command(284)     => file_cmd(302).
	p(NT(284), (NT(302)));
//G491: valid_sym(308)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(308), (T(54)+T(16)+T(10)+T(4)+T(57)));
//G492: __E_cli_command_144(307) => valid_sym(308) __(34) normalize_cmd_arg(309).
	p(NT(307), (NT(308)+NT(34)+NT(309)));
//G493: valid_cmd(306)       => __E_cli_command_144(307).
	p(NT(306), (NT(307)));
//G494: cli_command(284)     => valid_cmd(306).
	p(NT(284), (NT(306)));
//G495: sat_sym(312)         => 's' 'a' 't'.
	p(NT(312), (T(15)+T(16)+T(8)));
//G496: __E_cli_command_145(311) => sat_sym(312) __(34) normalize_cmd_arg(309).
	p(NT(311), (NT(312)+NT(34)+NT(309)));
//G497: sat_cmd(310)         => __E_cli_command_145(311).
	p(NT(310), (NT(311)));
//G498: cli_command(284)     => sat_cmd(310).
	p(NT(284), (NT(310)));
//G499: unsat_sym(315)       => 'u' 'n' 's' 'a' 't'.
	p(NT(315), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G500: __E_cli_command_146(314) => unsat_sym(315) __(34) normalize_cmd_arg(309).
	p(NT(314), (NT(315)+NT(34)+NT(309)));
//G501: unsat_cmd(313)       => __E_cli_command_146(314).
	p(NT(313), (NT(314)));
//G502: cli_command(284)     => unsat_cmd(313).
	p(NT(284), (NT(313)));
//G503: solve_sym(318)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(318), (T(15)+T(6)+T(10)+T(54)+T(11)));
//G504: __E___E_cli_command_147_148(319) => solve_options(320).
	p(NT(319), (NT(320)));
//G505: __E___E_cli_command_147_148(319) => null.
	p(NT(319), (nul));
//G506: __E_cli_command_147(317) => solve_sym(318) __E___E_cli_command_147_148(319) __(34) wff_cmd_arg(321).
	p(NT(317), (NT(318)+NT(319)+NT(34)+NT(321)));
//G507: solve_cmd(316)       => __E_cli_command_147(317).
	p(NT(316), (NT(317)));
//G508: cli_command(284)     => solve_cmd(316).
	p(NT(284), (NT(316)));
//G509: lgrs_sym(324)        => 'l' 'g' 'r' 's'.
	p(NT(324), (T(10)+T(58)+T(19)+T(15)));
//G510: __E_cli_command_149(323) => lgrs_sym(324) __(34) wff_cmd_arg(321).
	p(NT(323), (NT(324)+NT(34)+NT(321)));
//G511: lgrs_cmd(322)        => __E_cli_command_149(323).
	p(NT(322), (NT(323)));
//G512: cli_command(284)     => lgrs_cmd(322).
	p(NT(284), (NT(322)));
//G513: __E___E_cli_command_150_151(328) => 'r'.
	p(NT(328), (T(19)));
//G514: __E___E_cli_command_150_151(328) => 'r' 'u' 'n'.
	p(NT(328), (T(19)+T(7)+T(5)));
//G515: run_sym(327)         => __E___E_cli_command_150_151(328).
	p(NT(327), (NT(328)));
//G516: __E___E_cli_command_150_152(329) => history(330).
	p(NT(329), (NT(330)));
//G517: __E___E_cli_command_150_152(329) => wff(14).
	p(NT(329), (NT(14)));
//G518: __E_cli_command_150(326) => run_sym(327) __(34) __E___E_cli_command_150_152(329).
	p(NT(326), (NT(327)+NT(34)+NT(329)));
//G519: run_cmd(325)         => __E_cli_command_150(326).
	p(NT(325), (NT(326)));
//G520: cli_command(284)     => run_cmd(325).
	p(NT(284), (NT(325)));
//G521: __E___E_cli_command_153_154(334) => 'n'.
	p(NT(334), (T(5)));
//G522: __E___E_cli_command_153_154(334) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(334), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(59)+T(11)));
//G523: normalize_sym(333)   => __E___E_cli_command_153_154(334).
	p(NT(333), (NT(334)));
//G524: __E_cli_command_153(332) => normalize_sym(333) __(34) normalize_cmd_arg(309).
	p(NT(332), (NT(333)+NT(34)+NT(309)));
//G525: normalize_cmd(331)   => __E_cli_command_153(332).
	p(NT(331), (NT(332)));
//G526: cli_command(284)     => normalize_cmd(331).
	p(NT(284), (NT(331)));
//G527: __E___E_cli_command_155_156(338) => 's'.
	p(NT(338), (T(15)));
//G528: __E___E_cli_command_155_156(338) => 's' 'u' 'b' 's' 't'.
	p(NT(338), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G529: __E___E_cli_command_155_156(338) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(338), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G530: subst_sym(337)       => __E___E_cli_command_155_156(338).
	p(NT(337), (NT(338)));
//G531: __E_cli_command_155(336) => subst_sym(337) __(34) nf_cmd_arg(339) _(11) '[' _(11) nf_cmd_arg(339) _(11) '/' _(11) nf_cmd_arg(339) _(11) ']'.
	p(NT(336), (NT(337)+NT(34)+NT(339)+NT(11)+T(26)+NT(11)+NT(339)+NT(11)+T(43)+NT(11)+NT(339)+NT(11)+T(27)));
//G532: subst_cmd(335)       => __E_cli_command_155(336).
	p(NT(335), (NT(336)));
//G533: cli_command(284)     => subst_cmd(335).
	p(NT(284), (NT(335)));
//G534: __E___E_cli_command_157_158(343) => 'i'.
	p(NT(343), (T(4)));
//G535: __E___E_cli_command_157_158(343) => 'i' 'n' 's' 't'.
	p(NT(343), (T(4)+T(5)+T(15)+T(8)));
//G536: __E___E_cli_command_157_158(343) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(343), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G537: inst_sym(342)        => __E___E_cli_command_157_158(343).
	p(NT(342), (NT(343)));
//G538: __E_cli_command_157(341) => inst_sym(342) __(34) inst_args(344).
	p(NT(341), (NT(342)+NT(34)+NT(344)));
//G539: inst_cmd(340)        => __E_cli_command_157(341).
	p(NT(340), (NT(341)));
//G540: cli_command(284)     => inst_cmd(340).
	p(NT(284), (NT(340)));
//G541: dnf_sym(347)         => 'd' 'n' 'f'.
	p(NT(347), (T(57)+T(5)+T(9)));
//G542: __E_cli_command_159(346) => dnf_sym(347) __(34) nf_cmd_arg(339).
	p(NT(346), (NT(347)+NT(34)+NT(339)));
//G543: dnf_cmd(345)         => __E_cli_command_159(346).
	p(NT(345), (NT(346)));
//G544: cli_command(284)     => dnf_cmd(345).
	p(NT(284), (NT(345)));
//G545: cnf_sym(350)         => 'c' 'n' 'f'.
	p(NT(350), (T(14)+T(5)+T(9)));
//G546: __E_cli_command_160(349) => cnf_sym(350) __(34) nf_cmd_arg(339).
	p(NT(349), (NT(350)+NT(34)+NT(339)));
//G547: cnf_cmd(348)         => __E_cli_command_160(349).
	p(NT(348), (NT(349)));
//G548: cli_command(284)     => cnf_cmd(348).
	p(NT(284), (NT(348)));
//G549: anf_sym(353)         => 'a' 'n' 'f'.
	p(NT(353), (T(16)+T(5)+T(9)));
//G550: __E_cli_command_161(352) => anf_sym(353) __(34) nf_cmd_arg(339).
	p(NT(352), (NT(353)+NT(34)+NT(339)));
//G551: anf_cmd(351)         => __E_cli_command_161(352).
	p(NT(351), (NT(352)));
//G552: cli_command(284)     => anf_cmd(351).
	p(NT(284), (NT(351)));
//G553: nnf_sym(356)         => 'n' 'n' 'f'.
	p(NT(356), (T(5)+T(5)+T(9)));
//G554: __E_cli_command_162(355) => nnf_sym(356) __(34) nf_cmd_arg(339).
	p(NT(355), (NT(356)+NT(34)+NT(339)));
//G555: nnf_cmd(354)         => __E_cli_command_162(355).
	p(NT(354), (NT(355)));
//G556: cli_command(284)     => nnf_cmd(354).
	p(NT(284), (NT(354)));
//G557: pnf_sym(359)         => 'p' 'n' 'f'.
	p(NT(359), (T(56)+T(5)+T(9)));
//G558: __E_cli_command_163(358) => pnf_sym(359) __(34) nf_cmd_arg(339).
	p(NT(358), (NT(359)+NT(34)+NT(339)));
//G559: pnf_cmd(357)         => __E_cli_command_163(358).
	p(NT(357), (NT(358)));
//G560: cli_command(284)     => pnf_cmd(357).
	p(NT(284), (NT(357)));
//G561: mnf_sym(362)         => 'm' 'n' 'f'.
	p(NT(362), (T(21)+T(5)+T(9)));
//G562: __E_cli_command_164(361) => mnf_sym(362) __(34) nf_cmd_arg(339).
	p(NT(361), (NT(362)+NT(34)+NT(339)));
//G563: mnf_cmd(360)         => __E_cli_command_164(361).
	p(NT(360), (NT(361)));
//G564: cli_command(284)     => mnf_cmd(360).
	p(NT(284), (NT(360)));
//G565: snf_sym(365)         => 's' 'n' 'f'.
	p(NT(365), (T(15)+T(5)+T(9)));
//G566: __E_cli_command_165(364) => snf_sym(365) __(34) nf_cmd_arg(339).
	p(NT(364), (NT(365)+NT(34)+NT(339)));
//G567: snf_cmd(363)         => __E_cli_command_165(364).
	p(NT(363), (NT(364)));
//G568: cli_command(284)     => snf_cmd(363).
	p(NT(284), (NT(363)));
//G569: onf_sym(368)         => 'o' 'n' 'f'.
	p(NT(368), (T(6)+T(5)+T(9)));
//G570: __E_cli_command_166(367) => onf_sym(368) __(34) variable(147) __(34) onf_cmd_arg(369).
	p(NT(367), (NT(368)+NT(34)+NT(147)+NT(34)+NT(369)));
//G571: onf_cmd(366)         => __E_cli_command_166(367).
	p(NT(366), (NT(367)));
//G572: cli_command(284)     => onf_cmd(366).
	p(NT(284), (NT(366)));
//G573: qelim_sym(372)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(372), (T(53)+T(11)+T(10)+T(4)+T(21)));
//G574: __E_cli_command_167(371) => qelim_sym(372) __(34) wff_cmd_arg(321).
	p(NT(371), (NT(372)+NT(34)+NT(321)));
//G575: qelim_cmd(370)       => __E_cli_command_167(371).
	p(NT(370), (NT(371)));
//G576: cli_command(284)     => qelim_cmd(370).
	p(NT(284), (NT(370)));
//G577: get_sym(375)         => 'g' 'e' 't'.
	p(NT(375), (T(58)+T(11)+T(8)));
//G578: __E___E_cli_command_168_169(376) => __(34) option_name(377).
	p(NT(376), (NT(34)+NT(377)));
//G579: __E___E_cli_command_168_169(376) => null.
	p(NT(376), (nul));
//G580: __E_cli_command_168(374) => get_sym(375) __E___E_cli_command_168_169(376).
	p(NT(374), (NT(375)+NT(376)));
//G581: get_cmd(373)         => __E_cli_command_168(374).
	p(NT(373), (NT(374)));
//G582: cli_command(284)     => get_cmd(373).
	p(NT(284), (NT(373)));
//G583: set_sym(380)         => 's' 'e' 't'.
	p(NT(380), (T(15)+T(11)+T(8)));
//G584: __E___E_cli_command_170_171(381) => __(34).
	p(NT(381), (NT(34)));
//G585: __E___E_cli_command_170_171(381) => _(11) '=' _(11).
	p(NT(381), (NT(11)+T(3)+NT(11)));
//G586: __E_cli_command_170(379) => set_sym(380) __(34) option_name(377) __E___E_cli_command_170_171(381) option_value(382).
	p(NT(379), (NT(380)+NT(34)+NT(377)+NT(381)+NT(382)));
//G587: set_cmd(378)         => __E_cli_command_170(379).
	p(NT(378), (NT(379)));
//G588: cli_command(284)     => set_cmd(378).
	p(NT(284), (NT(378)));
//G589: enable_sym(385)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(385), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G590: __E_cli_command_172(384) => enable_sym(385) __(34) option_name(377).
	p(NT(384), (NT(385)+NT(34)+NT(377)));
//G591: enable_cmd(383)      => __E_cli_command_172(384).
	p(NT(383), (NT(384)));
//G592: cli_command(284)     => enable_cmd(383).
	p(NT(284), (NT(383)));
//G593: disable_sym(388)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(388), (T(57)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G594: __E_cli_command_173(387) => disable_sym(388) __(34) option_name(377).
	p(NT(387), (NT(388)+NT(34)+NT(377)));
//G595: disable_cmd(386)     => __E_cli_command_173(387).
	p(NT(386), (NT(387)));
//G596: cli_command(284)     => disable_cmd(386).
	p(NT(284), (NT(386)));
//G597: toggle_sym(391)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(391), (T(8)+T(6)+T(58)+T(58)+T(10)+T(11)));
//G598: __E_cli_command_174(390) => toggle_sym(391) __(34) option_name(377).
	p(NT(390), (NT(391)+NT(34)+NT(377)));
//G599: toggle_cmd(389)      => __E_cli_command_174(390).
	p(NT(389), (NT(390)));
//G600: cli_command(284)     => toggle_cmd(389).
	p(NT(284), (NT(389)));
//G601: __E___E_cli_command_175_176(395) => 'd' 'e' 'f' 's'.
	p(NT(395), (T(57)+T(11)+T(9)+T(15)));
//G602: __E___E_cli_command_175_176(395) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(395), (T(57)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G603: def_sym(394)         => __E___E_cli_command_175_176(395).
	p(NT(394), (NT(395)));
//G604: __E_cli_command_175(393) => def_sym(394).
	p(NT(393), (NT(394)));
//G605: def_list_cmd(392)    => __E_cli_command_175(393).
	p(NT(392), (NT(393)));
//G606: cli_command(284)     => def_list_cmd(392).
	p(NT(284), (NT(392)));
//G607: __E_cli_command_177(397) => def_sym(394) __(34) num(225).
	p(NT(397), (NT(394)+NT(34)+NT(225)));
//G608: def_print_cmd(396)   => __E_cli_command_177(397).
	p(NT(396), (NT(397)));
//G609: cli_command(284)     => def_print_cmd(396).
	p(NT(284), (NT(396)));
//G610: def_rr_cmd(398)      => rec_relation(17).
	p(NT(398), (NT(17)));
//G611: cli_command(284)     => def_rr_cmd(398).
	p(NT(284), (NT(398)));
//G612: def_input_cmd(399)   => input_def(18).
	p(NT(399), (NT(18)));
//G613: cli_command(284)     => def_input_cmd(399).
	p(NT(284), (NT(399)));
//G614: def_output_cmd(400)  => output_def(19).
	p(NT(400), (NT(19)));
//G615: cli_command(284)     => def_output_cmd(400).
	p(NT(284), (NT(400)));
//G616: __E___E_cli_command_178_179(404) => 'h' 'i' 's' 't'.
	p(NT(404), (T(55)+T(4)+T(15)+T(8)));
//G617: __E___E_cli_command_178_179(404) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(404), (T(55)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G618: history_sym(403)     => __E___E_cli_command_178_179(404).
	p(NT(403), (NT(404)));
//G619: __E_cli_command_178(402) => history_sym(403).
	p(NT(402), (NT(403)));
//G620: history_list_cmd(401) => __E_cli_command_178(402).
	p(NT(401), (NT(402)));
//G621: cli_command(284)     => history_list_cmd(401).
	p(NT(284), (NT(401)));
//G622: __E_cli_command_180(406) => history_sym(403) __(34) history(330).
	p(NT(406), (NT(403)+NT(34)+NT(330)));
//G623: history_print_cmd(405) => __E_cli_command_180(406).
	p(NT(405), (NT(406)));
//G624: cli_command(284)     => history_print_cmd(405).
	p(NT(284), (NT(405)));
//G625: __E_cli_command_181(408) => wff(14).
	p(NT(408), (NT(14)));
//G626: __E_cli_command_181(408) => bf(30).
	p(NT(408), (NT(30)));
//G627: history_store_cmd(407) => __E_cli_command_181(408).
	p(NT(407), (NT(408)));
//G628: cli_command(284)     => history_store_cmd(407).
	p(NT(284), (NT(407)));
//G629: __E_solve_options_182(409) => __(34) solve_option(410).
	p(NT(409), (NT(34)+NT(410)));
//G630: __E_solve_options_183(411) => null.
	p(NT(411), (nul));
//G631: __E_solve_options_183(411) => __E_solve_options_182(409) __E_solve_options_183(411).
	p(NT(411), (NT(409)+NT(411)));
//G632: solve_options(320)   => __E_solve_options_183(411).
	p(NT(320), (NT(411)));
//G633: __E_solve_option_184(412) => solver_mode(413).
	p(NT(412), (NT(413)));
//G634: __E_solve_option_184(412) => type(275).
	p(NT(412), (NT(275)));
//G635: solve_option(410)    => '-' '-' __E_solve_option_184(412).
	p(NT(410), (T(30)+T(30)+NT(412)));
//G636: __E_solver_mode_185(415) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(415), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G637: __E_solver_mode_185(415) => 'm' 'i' 'n'.
	p(NT(415), (T(21)+T(4)+T(5)));
//G638: solver_mode_minimum(414) => __E_solver_mode_185(415).
	p(NT(414), (NT(415)));
//G639: solver_mode(413)     => solver_mode_minimum(414).
	p(NT(413), (NT(414)));
//G640: __E_solver_mode_186(417) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(417), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G641: __E_solver_mode_186(417) => 'm' 'a' 'x'.
	p(NT(417), (T(21)+T(16)+T(29)));
//G642: solver_mode_maximum(416) => __E_solver_mode_186(417).
	p(NT(416), (NT(417)));
//G643: solver_mode(413)     => solver_mode_maximum(416).
	p(NT(413), (NT(416)));
//G644: q_file_name(38)      => '"' file_name(418) '"'.
	p(NT(38), (T(60)+NT(418)+T(60)));
//G645: __E_file_name_187(419) => printable(8).
	p(NT(419), (NT(8)));
//G646: __E_file_name_187(419) => printable(8) __E_file_name_187(419).
	p(NT(419), (NT(8)+NT(419)));
//G647: file_name(418)       => __E_file_name_187(419).
	p(NT(418), (NT(419)));
//G648: __E_option_name_188(420) => alnum(6).
	p(NT(420), (NT(6)));
//G649: __E_option_name_188(420) => alnum(6) __E_option_name_188(420).
	p(NT(420), (NT(6)+NT(420)));
//G650: option_name(377)     => __E_option_name_188(420).
	p(NT(377), (NT(420)));
//G651: __E_option_value_189(421) => alnum(6).
	p(NT(421), (NT(6)));
//G652: __E_option_value_189(421) => alnum(6) __E_option_value_189(421).
	p(NT(421), (NT(6)+NT(421)));
//G653: option_value(382)    => __E_option_value_189(421).
	p(NT(382), (NT(421)));
//G654: bf_cmd_arg(422)      => history(330).
	p(NT(422), (NT(330)));
//G655: bf_cmd_arg(422)      => bf(30).
	p(NT(422), (NT(30)));
//G656: wff_cmd_arg(321)     => history(330).
	p(NT(321), (NT(330)));
//G657: wff_cmd_arg(321)     => wff(14).
	p(NT(321), (NT(14)));
//G658: nf_cmd_arg(339)      => history(330).
	p(NT(339), (NT(330)));
//G659: nf_cmd_arg(339)      => ref(27).
	p(NT(339), (NT(27)));
//G660: nf_cmd_arg(339)      => wff(14).
	p(NT(339), (NT(14)));
//G661: nf_cmd_arg(339)      => bf(30).
	p(NT(339), (NT(30)));
//G662: onf_cmd_arg(369)     => history(330).
	p(NT(369), (NT(330)));
//G663: onf_cmd_arg(369)     => wff(14).
	p(NT(369), (NT(14)));
//G664: normalize_cmd_arg(309) => history(330).
	p(NT(309), (NT(330)));
//G665: normalize_cmd_arg(309) => spec(10).
	p(NT(309), (NT(10)));
//G666: normalize_cmd_arg(309) => ref(27).
	p(NT(309), (NT(27)));
//G667: normalize_cmd_arg(309) => wff(14).
	p(NT(309), (NT(14)));
//G668: normalize_cmd_arg(309) => bf(30).
	p(NT(309), (NT(30)));
//G669: inst_args(344)       => wff_cmd_arg(321) _(11) '[' _(11) variable(147) _(11) '/' _(11) bf_cmd_arg(422) _(11) ']'.
	p(NT(344), (NT(321)+NT(11)+T(26)+NT(11)+NT(147)+NT(11)+T(43)+NT(11)+NT(422)+NT(11)+T(27)));
//G670: inst_args(344)       => bf_cmd_arg(422) _(11) '[' _(11) variable(147) _(11) '/' _(11) bf_cmd_arg(422) _(11) ']'.
	p(NT(344), (NT(422)+NT(11)+T(26)+NT(11)+NT(147)+NT(11)+T(43)+NT(11)+NT(422)+NT(11)+T(27)));
//G671: help_arg(301)        => help_sym(298).
	p(NT(301), (NT(298)));
//G672: help_arg(301)        => version_sym(291).
	p(NT(301), (NT(291)));
//G673: help_arg(301)        => quit_sym(288).
	p(NT(301), (NT(288)));
//G674: help_arg(301)        => clear_sym(294).
	p(NT(301), (NT(294)));
//G675: help_arg(301)        => get_sym(375).
	p(NT(301), (NT(375)));
//G676: help_arg(301)        => set_sym(380).
	p(NT(301), (NT(380)));
//G677: help_arg(301)        => enable_sym(385).
	p(NT(301), (NT(385)));
//G678: help_arg(301)        => disable_sym(388).
	p(NT(301), (NT(388)));
//G679: help_arg(301)        => toggle_sym(391).
	p(NT(301), (NT(391)));
//G680: help_arg(301)        => file_sym(304).
	p(NT(301), (NT(304)));
//G681: help_arg(301)        => history_sym(403).
	p(NT(301), (NT(403)));
//G682: help_arg(301)        => abs_history_sym(423).
	p(NT(301), (NT(423)));
//G683: help_arg(301)        => rel_history_sym(424).
	p(NT(301), (NT(424)));
//G684: help_arg(301)        => selection_sym(425).
	p(NT(301), (NT(425)));
//G685: help_arg(301)        => def_sym(394).
	p(NT(301), (NT(394)));
//G686: help_arg(301)        => inst_sym(342).
	p(NT(301), (NT(342)));
//G687: help_arg(301)        => subst_sym(337).
	p(NT(301), (NT(337)));
//G688: help_arg(301)        => normalize_sym(333).
	p(NT(301), (NT(333)));
//G689: help_arg(301)        => execute_sym(426).
	p(NT(301), (NT(426)));
//G690: help_arg(301)        => solve_sym(318).
	p(NT(301), (NT(318)));
//G691: help_arg(301)        => valid_sym(308).
	p(NT(301), (NT(308)));
//G692: help_arg(301)        => sat_sym(312).
	p(NT(301), (NT(312)));
//G693: help_arg(301)        => unsat_sym(315).
	p(NT(301), (NT(315)));
//G694: help_arg(301)        => run_sym(327).
	p(NT(301), (NT(327)));
//G695: help_arg(301)        => dnf_sym(347).
	p(NT(301), (NT(347)));
//G696: help_arg(301)        => cnf_sym(350).
	p(NT(301), (NT(350)));
//G697: help_arg(301)        => anf_sym(353).
	p(NT(301), (NT(353)));
//G698: help_arg(301)        => snf_sym(365).
	p(NT(301), (NT(365)));
//G699: help_arg(301)        => nnf_sym(356).
	p(NT(301), (NT(356)));
//G700: help_arg(301)        => pnf_sym(359).
	p(NT(301), (NT(359)));
//G701: help_arg(301)        => mnf_sym(362).
	p(NT(301), (NT(362)));
//G702: help_arg(301)        => onf_sym(368).
	p(NT(301), (NT(368)));
//G703: help_arg(301)        => qelim_sym(372).
	p(NT(301), (NT(372)));
//G704: __E___E_help_arg_190_191(429) => 's'.
	p(NT(429), (T(15)));
//G705: __E___E_help_arg_190_191(429) => null.
	p(NT(429), (nul));
//G706: __E_help_arg_190(428) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_190_191(429).
	p(NT(428), (T(11)+T(29)+T(16)+T(21)+T(56)+T(10)+T(11)+NT(429)));
//G707: examples_sym(427)    => __E_help_arg_190(428).
	p(NT(427), (NT(428)));
//G708: help_arg(301)        => examples_sym(427).
	p(NT(301), (NT(427)));
//G709: __E_history_192(431) => '%'.
	p(NT(431), (T(44)));
//G710: rel_history_sym(424) => '%' '-'.
	p(NT(424), (T(44)+T(30)));
//G711: history_id(433)      => digits(272).
	p(NT(433), (NT(272)));
//G712: __E___E_history_192_193(432) => history_id(433).
	p(NT(432), (NT(433)));
//G713: __E___E_history_192_193(432) => null.
	p(NT(432), (nul));
//G714: __E_history_192(431) => rel_history_sym(424) __E___E_history_192_193(432).
	p(NT(431), (NT(424)+NT(432)));
//G715: rel_history(430)     => __E_history_192(431).
	p(NT(430), (NT(431)));
//G716: history(330)         => rel_history(430).
	p(NT(330), (NT(430)));
//G717: abs_history_sym(423) => '%'.
	p(NT(423), (T(44)));
//G718: __E_history_194(435) => abs_history_sym(423) history_id(433).
	p(NT(435), (NT(423)+NT(433)));
//G719: abs_history(434)     => __E_history_194(435).
	p(NT(434), (NT(435)));
//G720: history(330)         => abs_history(434).
	p(NT(330), (NT(434)));
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
		bf_rule, __E___E_library_17_20, wff_matcher, wff_body, bf_matcher, bf_body, tau_constant_source, __E_tau_constant_source_21, wff_parenthesis, __E_wff_22, 
		wff_sometimes, __E_wff_23, __E___E_wff_23_24, wff_always, __E_wff_25, __E___E_wff_25_26, wff_conditional, __E_wff_27, wff_all, __E_wff_28, 
		q_vars, wff_ex, __E_wff_29, wff_ref, constraint, wff_imply, __E_wff_30, wff_rimply, __E_wff_31, wff_equiv, 
		__E_wff_32, wff_or, __E_wff_33, wff_xor, __E_wff_34, wff_and, __E_wff_35, wff_neg, __E_wff_36, wff_t, 
		wff_f, bf_interval, __E_wff_37, bf_eq, __E_wff_38, bf_neq, __E_wff_39, bf_lteq, __E_wff_40, bf_nlteq, 
		__E_wff_41, bf_gt, __E_wff_42, bf_ngt, __E_wff_43, bf_gteq, __E_wff_44, bf_ngteq, __E_wff_45, bf_lt, 
		__E_wff_46, bf_nlt, __E_wff_47, bv_eq, __E_wff_48, bv, bv_neq, __E_wff_49, bv_lteq, __E_wff_50, 
		bv_nlteq, __E_wff_51, bv_gt, __E_wff_52, bv_ngt, __E_wff_53, bv_gteq, __E_wff_54, bv_ngteq, __E_wff_55, 
		bv_lt, __E_wff_56, bv_nlt, __E_wff_57, bf_parenthesis, __E_bf_58, bf_constant, variable, bf_fall, __E_bf_59, 
		bf_fex, __E_bf_60, bf_ref, bf_or, __E_bf_61, bf_xor, __E_bf_62, bf_t, __E_bf_63, __E___E_bf_63_64, 
		bf_f, __E_bf_65, __E___E_bf_65_66, bf_and, __E_bf_67, __E___E_bf_67_68, bf_neg, __E_bf_69, bf_neg_oprnd, __E___E_bf_69_70, 
		bf_and_nosep, __E_bf_71, bf_and_nosep_1st_oprnd, __E___E_bf_71_72, bf_and_nosep_2nd_oprnd, __E___E_bf_71_73, bv_parenthesis, __E_bv_74, bv_checked, __E_bv_75, 
		bv_add, __E_bv_76, bv_mul, __E_bv_77, bv_div, __E_bv_78, bv_mod, __E_bv_79, bv_sub, __E_bv_80, 
		bv_neg, bv_and, __E_bv_81, bv_nand, __E_bv_82, bv_or, __E_bv_83, bv_nor, __E_bv_84, bv_xor, 
		__E_bv_85, bv_xnor, __E_bv_86, bv_shl, __E_bv_87, bv_shr, __E_bv_88, bv_min, __E_bv_89, bv_max, 
		__E_bv_90, bv_constant, binary, __E_bv_constant_91, decimal, __E_bv_constant_92, hexadecimal, __E_bv_constant_93, __E_binary_94, __E_binary_95, 
		__E_hexadecimal_96, __E_decimal_97, ctn_neq, __E_constraint_98, ctnvar, num, ctn_eq, __E_constraint_99, ctn_gteq, __E_constraint_100, 
		ctn_gt, __E_constraint_101, ctn_lteq, __E_constraint_102, ctn_lt, __E_constraint_103, __E_bf_constant_104, source, __E_bf_constant_105, src_c, 
		__E_source_106, __E___E_source_106_107, __E___E_source_106_108, __E_src_c_109, __E_src_c_110, __E_variable_111, uconst, io_var, var_name, __E_variable_112, 
		__E_var_name_113, __E_var_name_114, __E_var_name_115, __E_var_name_116, __E_var_name_117, offset, chars, uconst_name, __E_uconst_118, __E___E_uconst_118_119, 
		q_var, __E_q_vars_120, __E_q_vars_121, __E_offsets_122, __E_offsets_123, integer, shift, __E_offset_124, __E_shift_125, __E___E_shift_125_126, 
		__E_chars_127, __E_chars_128, digits, __E_digits_129, __E_integer_130, type, __E_typed_131, subtype, scope_id, comment, 
		__E_comment_132, __E_comment_133, __E_comment_134, cli, cli_command, __E_cli_135, __E_cli_136, quit_cmd, quit_sym, __E_cli_command_137, 
		version_cmd, version_sym, __E_cli_command_138, clear_cmd, clear_sym, __E_cli_command_139, help_cmd, __E_cli_command_140, help_sym, __E___E_cli_command_140_141, 
		__E___E_cli_command_140_142, help_arg, file_cmd, __E_cli_command_143, file_sym, q_string, valid_cmd, __E_cli_command_144, valid_sym, normalize_cmd_arg, 
		sat_cmd, __E_cli_command_145, sat_sym, unsat_cmd, __E_cli_command_146, unsat_sym, solve_cmd, __E_cli_command_147, solve_sym, __E___E_cli_command_147_148, 
		solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_149, lgrs_sym, run_cmd, __E_cli_command_150, run_sym, __E___E_cli_command_150_151, __E___E_cli_command_150_152, 
		history, normalize_cmd, __E_cli_command_153, normalize_sym, __E___E_cli_command_153_154, subst_cmd, __E_cli_command_155, subst_sym, __E___E_cli_command_155_156, nf_cmd_arg, 
		inst_cmd, __E_cli_command_157, inst_sym, __E___E_cli_command_157_158, inst_args, dnf_cmd, __E_cli_command_159, dnf_sym, cnf_cmd, __E_cli_command_160, 
		cnf_sym, anf_cmd, __E_cli_command_161, anf_sym, nnf_cmd, __E_cli_command_162, nnf_sym, pnf_cmd, __E_cli_command_163, pnf_sym, 
		mnf_cmd, __E_cli_command_164, mnf_sym, snf_cmd, __E_cli_command_165, snf_sym, onf_cmd, __E_cli_command_166, onf_sym, onf_cmd_arg, 
		qelim_cmd, __E_cli_command_167, qelim_sym, get_cmd, __E_cli_command_168, get_sym, __E___E_cli_command_168_169, option_name, set_cmd, __E_cli_command_170, 
		set_sym, __E___E_cli_command_170_171, option_value, enable_cmd, __E_cli_command_172, enable_sym, disable_cmd, __E_cli_command_173, disable_sym, toggle_cmd, 
		__E_cli_command_174, toggle_sym, def_list_cmd, __E_cli_command_175, def_sym, __E___E_cli_command_175_176, def_print_cmd, __E_cli_command_177, def_rr_cmd, def_input_cmd, 
		def_output_cmd, history_list_cmd, __E_cli_command_178, history_sym, __E___E_cli_command_178_179, history_print_cmd, __E_cli_command_180, history_store_cmd, __E_cli_command_181, __E_solve_options_182, 
		solve_option, __E_solve_options_183, __E_solve_option_184, solver_mode, solver_mode_minimum, __E_solver_mode_185, solver_mode_maximum, __E_solver_mode_186, file_name, __E_file_name_187, 
		__E_option_name_188, __E_option_value_189, bf_cmd_arg, abs_history_sym, rel_history_sym, selection_sym, execute_sym, examples_sym, __E_help_arg_190, __E___E_help_arg_190_191, 
		rel_history, __E_history_192, __E___E_history_192_193, history_id, abs_history, __E_history_194, __N_0, __N_1, __N_2, __N_3, 
		__N_4, __N_5, __N_6, __N_7, __N_8, 
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
