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
	"__E___E_bf_67_68", "bf_and_nosep_2nd_oprnd", "__E___E_bf_67_69", "bv_parenthesis", "__E_bv_70", "bv_add", "__E_bv_71", "bv_mul", "__E_bv_72", "bv_div", 
	"__E_bv_73", "bv_mod", "__E_bv_74", "bv_sub", "__E_bv_75", "bv_neg", "bv_and", "__E_bv_76", "bv_or", "__E_bv_77", 
	"bv_xor", "__E_bv_78", "bv_shl", "__E_bv_79", "bv_shr", "__E_bv_80", "ctn_neq", "__E_constraint_81", "ctnvar", "num", 
	"ctn_eq", "__E_constraint_82", "ctn_gteq", "__E_constraint_83", "ctn_gt", "__E_constraint_84", "ctn_lteq", "__E_constraint_85", "ctn_lt", "__E_constraint_86", 
	"__E_ba_constant_87", "source", "__E_ba_constant_88", "src_c", "__E_source_89", "__E___E_source_89_90", "__E___E_source_89_91", "__E_src_c_92", "__E_src_c_93", "__E_variable_94", 
	"uconst", "io_var", "var_name", "__E_variable_95", "__E_var_name_96", "__E_var_name_97", "__E_var_name_98", "__E_var_name_99", "__E_var_name_100", "offset", 
	"chars", "uconst_name", "__E_uconst_101", "__E___E_uconst_101_102", "q_var", "__E_q_vars_103", "__E_q_vars_104", "__E_offsets_105", "__E_offsets_106", "integer", 
	"shift", "__E_offset_107", "__E_shift_108", "__E___E_shift_108_109", "__E_chars_110", "__E_chars_111", "digits", "__E_digits_112", "__E_integer_113", "type", 
	"__E_typed_114", "subtype", "comment", "__E_comment_115", "__E_comment_116", "__E_comment_117", "cli", "cli_command", "__E_cli_118", "__E_cli_119", 
	"quit_cmd", "quit_sym", "__E_cli_command_120", "version_cmd", "version_sym", "__E_cli_command_121", "clear_cmd", "clear_sym", "__E_cli_command_122", "help_cmd", 
	"__E_cli_command_123", "help_sym", "__E___E_cli_command_123_124", "__E___E_cli_command_123_125", "help_arg", "file_cmd", "__E_cli_command_126", "file_sym", "q_string", "valid_cmd", 
	"__E_cli_command_127", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_128", "sat_sym", "unsat_cmd", "__E_cli_command_129", "unsat_sym", "solve_cmd", 
	"__E_cli_command_130", "solve_sym", "__E___E_cli_command_130_131", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_132", "lgrs_sym", "run_cmd", "__E_cli_command_133", 
	"run_sym", "__E___E_cli_command_133_134", "__E___E_cli_command_133_135", "history", "normalize_cmd", "__E_cli_command_136", "normalize_sym", "__E___E_cli_command_136_137", "subst_cmd", "__E_cli_command_138", 
	"subst_sym", "__E___E_cli_command_138_139", "nf_cmd_arg", "inst_cmd", "__E_cli_command_140", "inst_sym", "__E___E_cli_command_140_141", "inst_args", "dnf_cmd", "__E_cli_command_142", 
	"dnf_sym", "cnf_cmd", "__E_cli_command_143", "cnf_sym", "anf_cmd", "__E_cli_command_144", "anf_sym", "nnf_cmd", "__E_cli_command_145", "nnf_sym", 
	"pnf_cmd", "__E_cli_command_146", "pnf_sym", "mnf_cmd", "__E_cli_command_147", "mnf_sym", "onf_cmd", "__E_cli_command_148", "onf_sym", "onf_cmd_arg", 
	"qelim_cmd", "__E_cli_command_149", "qelim_sym", "get_cmd", "__E_cli_command_150", "get_sym", "__E___E_cli_command_150_151", "option_name", "set_cmd", "__E_cli_command_152", 
	"set_sym", "__E___E_cli_command_152_153", "option_value", "enable_cmd", "__E_cli_command_154", "enable_sym", "disable_cmd", "__E_cli_command_155", "disable_sym", "toggle_cmd", 
	"__E_cli_command_156", "toggle_sym", "def_list_cmd", "__E_cli_command_157", "def_sym", "__E___E_cli_command_157_158", "def_print_cmd", "__E_cli_command_159", "def_rr_cmd", "def_input_cmd", 
	"def_output_cmd", "history_list_cmd", "__E_cli_command_160", "history_sym", "__E___E_cli_command_160_161", "history_print_cmd", "__E_cli_command_162", "history_store_cmd", "__E_cli_command_163", "__E_solve_options_164", 
	"solve_option", "__E_solve_options_165", "__E_solve_option_166", "solver_mode", "solver_mode_minimum", "__E_solver_mode_167", "solver_mode_maximum", "__E_solver_mode_168", "file_name", "__E_file_name_169", 
	"__E_option_name_170", "__E_option_value_171", "bf_cmd_arg", "abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", "snf_sym", "examples_sym", "__E_help_arg_172", 
	"__E___E_help_arg_172_173", "rel_history", "__E_history_174", "__E___E_history_174_175", "history_id", "abs_history", "__E_history_176", "__N_0", "__N_1", "__N_2", 
	"__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', 'a', 'b', 'k', 
	'r', ',', 'm', '<', '>', 'w', 'y', '[', ']', '?', 
	'x', '-', '|', '^', '&', '!', 'T', 'F', '_', '1', 
	'0', '\'', '+', '*', '/', '%', '~', '{', '}', '$', 
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
			29, 31, 40, 188, 201, 212, 221, 229, 236, 239,
			241, 337, 342, 378
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
			{ 203 },
			{ 210 },
			{ 210, 221 },
			{ 220 },
			{ 224 },
			{ 272 },
			{ 284 },
			{ 302 },
			{ 307 },
			{ 329 },
			{ 382 }
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
//G192: __E_bf_58(143)       => bf(30) _(11) '^' _(11) bf(30).
	p(NT(143), (NT(30)+NT(11)+T(32)+NT(11)+NT(30)));
//G193: bf_xor(142)          => __E_bf_58(143).
	p(NT(142), (NT(143)));
//G194: bf(30)               => bf_xor(142).
	p(NT(30), (NT(142)));
//G195: __E___E_bf_59_60(146) => typed(33).
	p(NT(146), (NT(33)));
//G196: __E___E_bf_59_60(146) => null.
	p(NT(146), (nul));
//G197: __E_bf_59(145)       => '1' __E___E_bf_59_60(146).
	p(NT(145), (T(38)+NT(146)));
//G198: bf_t(144)            => __E_bf_59(145).
	p(NT(144), (NT(145)));
//G199: bf(30)               => bf_t(144).
	p(NT(30), (NT(144)));
//G200: __E___E_bf_61_62(149) => typed(33).
	p(NT(149), (NT(33)));
//G201: __E___E_bf_61_62(149) => null.
	p(NT(149), (nul));
//G202: __E_bf_61(148)       => '0' __E___E_bf_61_62(149).
	p(NT(148), (T(39)+NT(149)));
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
	p(NT(154), (NT(155)+NT(11)+T(40)));
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
//G243: bv(112)              => variable(134).
	p(NT(112), (NT(134)));
//G244: __E_bv_71(166)       => bv(112) _(11) '+' _(11) bv(112).
	p(NT(166), (NT(112)+NT(11)+T(41)+NT(11)+NT(112)));
//G245: bv_add(165)          => __E_bv_71(166).
	p(NT(165), (NT(166)));
//G246: bv(112)              => bv_add(165).
	p(NT(112), (NT(165)));
//G247: __E_bv_72(168)       => bv(112) _(11) '*' _(11) bv(112).
	p(NT(168), (NT(112)+NT(11)+T(42)+NT(11)+NT(112)));
//G248: bv_mul(167)          => __E_bv_72(168).
	p(NT(167), (NT(168)));
//G249: bv(112)              => bv_mul(167).
	p(NT(112), (NT(167)));
//G250: __E_bv_73(170)       => bv(112) _(11) '/' _(11) bv(112).
	p(NT(170), (NT(112)+NT(11)+T(43)+NT(11)+NT(112)));
//G251: bv_div(169)          => __E_bv_73(170).
	p(NT(169), (NT(170)));
//G252: bv(112)              => bv_div(169).
	p(NT(112), (NT(169)));
//G253: __E_bv_74(172)       => bv(112) _(11) '%' _(11) bv(112).
	p(NT(172), (NT(112)+NT(11)+T(44)+NT(11)+NT(112)));
//G254: bv_mod(171)          => __E_bv_74(172).
	p(NT(171), (NT(172)));
//G255: bv(112)              => bv_mod(171).
	p(NT(112), (NT(171)));
//G256: __E_bv_75(174)       => bv(112) _(11) '-' _(11) bv(112).
	p(NT(174), (NT(112)+NT(11)+T(30)+NT(11)+NT(112)));
//G257: bv_sub(173)          => __E_bv_75(174).
	p(NT(173), (NT(174)));
//G258: bv(112)              => bv_sub(173).
	p(NT(112), (NT(173)));
//G259: bv_neg(175)          => bv(112).
	p(NT(175), (NT(112)));
//G260: bv(112)              => '~' _(11) bv_neg(175).
	p(NT(112), (T(45)+NT(11)+NT(175)));
//G261: __E_bv_76(177)       => bv(112) _(11) '&' _(11) bv(112).
	p(NT(177), (NT(112)+NT(11)+T(33)+NT(11)+NT(112)));
//G262: bv_and(176)          => __E_bv_76(177).
	p(NT(176), (NT(177)));
//G263: bv(112)              => bv_and(176).
	p(NT(112), (NT(176)));
//G264: __E_bv_77(179)       => bv(112) _(11) '|' _(11) bv(112).
	p(NT(179), (NT(112)+NT(11)+T(31)+NT(11)+NT(112)));
//G265: bv_or(178)           => __E_bv_77(179).
	p(NT(178), (NT(179)));
//G266: bv(112)              => bv_or(178).
	p(NT(112), (NT(178)));
//G267: __E_bv_78(181)       => bv(112) _(11) '(' '+' ')' _(11) bv(112).
	p(NT(181), (NT(112)+NT(11)+T(12)+T(41)+T(13)+NT(11)+NT(112)));
//G268: bv_xor(180)          => __E_bv_78(181).
	p(NT(180), (NT(181)));
//G269: bv(112)              => bv_xor(180).
	p(NT(112), (NT(180)));
//G270: __E_bv_79(183)       => bv(112) _(11) '<' '<' _(11) bv(112).
	p(NT(183), (NT(112)+NT(11)+T(22)+T(22)+NT(11)+NT(112)));
//G271: bv_shl(182)          => __E_bv_79(183).
	p(NT(182), (NT(183)));
//G272: bv(112)              => bv_shl(182).
	p(NT(112), (NT(182)));
//G273: __E_bv_80(185)       => bv(112) _(11) '>' '>' _(11) bv(112).
	p(NT(185), (NT(112)+NT(11)+T(23)+T(23)+NT(11)+NT(112)));
//G274: bv_shr(184)          => __E_bv_80(185).
	p(NT(184), (NT(185)));
//G275: bv(112)              => bv_shr(184).
	p(NT(112), (NT(184)));
//G276: bv(112)              => ba_constant(133).
	p(NT(112), (NT(133)));
//G277: __E_constraint_81(187) => '[' ctnvar(188) _(11) '!' '=' _(11) num(189) ']'.
	p(NT(187), (T(26)+NT(188)+NT(11)+T(34)+T(3)+NT(11)+NT(189)+T(27)));
//G278: __E_constraint_81(187) => '[' num(189) _(11) '!' '=' _(11) ctnvar(188) ']'.
	p(NT(187), (T(26)+NT(189)+NT(11)+T(34)+T(3)+NT(11)+NT(188)+T(27)));
//G279: ctn_neq(186)         => __E_constraint_81(187).
	p(NT(186), (NT(187)));
//G280: constraint(71)       => ctn_neq(186).
	p(NT(71), (NT(186)));
//G281: __E_constraint_82(191) => '[' ctnvar(188) _(11) '=' _(11) num(189) ']'.
	p(NT(191), (T(26)+NT(188)+NT(11)+T(3)+NT(11)+NT(189)+T(27)));
//G282: __E_constraint_82(191) => '[' num(189) _(11) '=' _(11) ctnvar(188) ']'.
	p(NT(191), (T(26)+NT(189)+NT(11)+T(3)+NT(11)+NT(188)+T(27)));
//G283: ctn_eq(190)          => __E_constraint_82(191).
	p(NT(190), (NT(191)));
//G284: constraint(71)       => ctn_eq(190).
	p(NT(71), (NT(190)));
//G285: __E_constraint_83(193) => '[' ctnvar(188) _(11) '>' '=' _(11) num(189) ']'.
	p(NT(193), (T(26)+NT(188)+NT(11)+T(23)+T(3)+NT(11)+NT(189)+T(27)));
//G286: __E_constraint_83(193) => '[' num(189) _(11) '>' '=' _(11) ctnvar(188) ']'.
	p(NT(193), (T(26)+NT(189)+NT(11)+T(23)+T(3)+NT(11)+NT(188)+T(27)));
//G287: ctn_gteq(192)        => __E_constraint_83(193).
	p(NT(192), (NT(193)));
//G288: constraint(71)       => ctn_gteq(192).
	p(NT(71), (NT(192)));
//G289: __E_constraint_84(195) => '[' ctnvar(188) _(11) '>' _(11) num(189) ']'.
	p(NT(195), (T(26)+NT(188)+NT(11)+T(23)+NT(11)+NT(189)+T(27)));
//G290: __E_constraint_84(195) => '[' num(189) _(11) '>' _(11) ctnvar(188) ']'.
	p(NT(195), (T(26)+NT(189)+NT(11)+T(23)+NT(11)+NT(188)+T(27)));
//G291: ctn_gt(194)          => __E_constraint_84(195).
	p(NT(194), (NT(195)));
//G292: constraint(71)       => ctn_gt(194).
	p(NT(71), (NT(194)));
//G293: __E_constraint_85(197) => '[' ctnvar(188) _(11) '<' '=' _(11) num(189) ']'.
	p(NT(197), (T(26)+NT(188)+NT(11)+T(22)+T(3)+NT(11)+NT(189)+T(27)));
//G294: __E_constraint_85(197) => '[' num(189) _(11) '<' '=' _(11) ctnvar(188) ']'.
	p(NT(197), (T(26)+NT(189)+NT(11)+T(22)+T(3)+NT(11)+NT(188)+T(27)));
//G295: ctn_lteq(196)        => __E_constraint_85(197).
	p(NT(196), (NT(197)));
//G296: constraint(71)       => ctn_lteq(196).
	p(NT(71), (NT(196)));
//G297: __E_constraint_86(199) => '[' ctnvar(188) _(11) '<' _(11) num(189) ']'.
	p(NT(199), (T(26)+NT(188)+NT(11)+T(22)+NT(11)+NT(189)+T(27)));
//G298: __E_constraint_86(199) => '[' num(189) _(11) '<' _(11) ctnvar(188) ']'.
	p(NT(199), (T(26)+NT(189)+NT(11)+T(22)+NT(11)+NT(188)+T(27)));
//G299: ctn_lt(198)          => __E_constraint_86(199).
	p(NT(198), (NT(199)));
//G300: constraint(71)       => ctn_lt(198).
	p(NT(71), (NT(198)));
//G301: __E_ba_constant_87(200) => capture(29).
	p(NT(200), (NT(29)));
//G302: __E_ba_constant_87(200) => source(201).
	p(NT(200), (NT(201)));
//G303: __E_ba_constant_88(202) => typed(33).
	p(NT(202), (NT(33)));
//G304: __E_ba_constant_88(202) => null.
	p(NT(202), (nul));
//G305: ba_constant(133)     => '{' _(11) __E_ba_constant_87(200) _(11) '}' __E_ba_constant_88(202).
	p(NT(133), (T(46)+NT(11)+NT(200)+NT(11)+T(47)+NT(202)));
//G306: __E___E_source_89_90(205) => src_c(203).
	p(NT(205), (NT(203)));
//G307: __E___E_source_89_90(205) => space(2).
	p(NT(205), (NT(2)));
//G308: __E___E_source_89_91(206) => null.
	p(NT(206), (nul));
//G309: __E___E_source_89_91(206) => __E___E_source_89_90(205) __E___E_source_89_91(206).
	p(NT(206), (NT(205)+NT(206)));
//G310: __E_source_89(204)   => __E___E_source_89_91(206) src_c(203).
	p(NT(204), (NT(206)+NT(203)));
//G311: __E_source_89(204)   => null.
	p(NT(204), (nul));
//G312: source(201)          => src_c(203) __E_source_89(204).
	p(NT(201), (NT(203)+NT(204)));
//G313: src_c(203)           => alnum(6).
	p(NT(203), (NT(6)));
//G314: __N_0(397)           => '{'.
	p(NT(397), (T(46)));
//G315: __N_1(398)           => '}'.
	p(NT(398), (T(47)));
//G316: src_c(203)           => ~( __N_0(397) ) & ~( __N_1(398) ) & punct(7).	 # conjunctive
	p(NT(203), ~(NT(397)) & ~(NT(398)) & (NT(7)));
//G317: __E_src_c_92(207)    => src_c(203).
	p(NT(207), (NT(203)));
//G318: __E_src_c_92(207)    => space(2).
	p(NT(207), (NT(2)));
//G319: __E_src_c_93(208)    => null.
	p(NT(208), (nul));
//G320: __E_src_c_93(208)    => __E_src_c_92(207) __E_src_c_93(208).
	p(NT(208), (NT(207)+NT(208)));
//G321: src_c(203)           => '{' __E_src_c_93(208) '}'.
	p(NT(203), (T(46)+NT(208)+T(47)));
//G322: __E_variable_94(209) => uconst(210).
	p(NT(209), (NT(210)));
//G323: __E_variable_94(209) => io_var(211).
	p(NT(209), (NT(211)));
//G324: __E_variable_94(209) => var_name(212).
	p(NT(209), (NT(212)));
//G325: __E_variable_95(213) => typed(33).
	p(NT(213), (NT(33)));
//G326: __E_variable_95(213) => null.
	p(NT(213), (nul));
//G327: variable(134)        => __E_variable_94(209) __E_variable_95(213).
	p(NT(134), (NT(209)+NT(213)));
//G328: __N_2(399)           => 'F'.
	p(NT(399), (T(36)));
//G329: __N_3(400)           => 'T'.
	p(NT(400), (T(35)));
//G330: __E_var_name_96(214) => ~( __N_2(399) ) & ~( __N_3(400) ) & alpha(5).	 # conjunctive
	p(NT(214), ~(NT(399)) & ~(NT(400)) & (NT(5)));
//G331: __E_var_name_97(215) => null.
	p(NT(215), (nul));
//G332: __E_var_name_97(215) => digit(3) __E_var_name_97(215).
	p(NT(215), (NT(3)+NT(215)));
//G333: var_name(212)        => __E_var_name_96(214) __E_var_name_97(215).	 # guarded: charvar
	p(NT(212), (NT(214)+NT(215)));
	p.back().guard = "charvar";
//G334: __N_4(401)           => 'F'.
	p(NT(401), (T(36)));
//G335: __N_5(402)           => 'T'.
	p(NT(402), (T(35)));
//G336: __E_var_name_98(216) => ~( __N_4(401) ) & ~( __N_5(402) ) & alpha(5).	 # conjunctive
	p(NT(216), ~(NT(401)) & ~(NT(402)) & (NT(5)));
//G337: __E_var_name_99(217) => alnum(6).
	p(NT(217), (NT(6)));
//G338: __E_var_name_99(217) => '_'.
	p(NT(217), (T(37)));
//G339: __E_var_name_100(218) => null.
	p(NT(218), (nul));
//G340: __E_var_name_100(218) => __E_var_name_99(217) __E_var_name_100(218).
	p(NT(218), (NT(217)+NT(218)));
//G341: var_name(212)        => __E_var_name_98(216) __E_var_name_100(218).	 # guarded: var
	p(NT(212), (NT(216)+NT(218)));
	p.back().guard = "var";
//G342: io_var(211)          => io_var_name(31) '[' offset(219) ']'.
	p(NT(211), (NT(31)+T(26)+NT(219)+T(27)));
//G343: io_var_name(31)      => chars(220).
	p(NT(31), (NT(220)));
//G344: __E___E_uconst_101_102(223) => chars(220) _(11).
	p(NT(223), (NT(220)+NT(11)));
//G345: __E___E_uconst_101_102(223) => null.
	p(NT(223), (nul));
//G346: __E_uconst_101(222)  => __E___E_uconst_101_102(223) ':' _(11) chars(220).
	p(NT(222), (NT(223)+T(2)+NT(11)+NT(220)));
//G347: uconst_name(221)     => __E_uconst_101(222).
	p(NT(221), (NT(222)));
//G348: uconst(210)          => '<' _(11) uconst_name(221) _(11) '>'.
	p(NT(210), (T(22)+NT(11)+NT(221)+NT(11)+T(23)));
//G349: __E_q_vars_103(225)  => _(11) ',' _(11) q_var(224).
	p(NT(225), (NT(11)+T(20)+NT(11)+NT(224)));
//G350: __E_q_vars_104(226)  => null.
	p(NT(226), (nul));
//G351: __E_q_vars_104(226)  => __E_q_vars_103(225) __E_q_vars_104(226).
	p(NT(226), (NT(225)+NT(226)));
//G352: q_vars(67)           => q_var(224) __E_q_vars_104(226).
	p(NT(67), (NT(224)+NT(226)));
//G353: q_var(224)           => capture(29).
	p(NT(224), (NT(29)));
//G354: __N_6(403)           => uconst(210).
	p(NT(403), (NT(210)));
//G355: q_var(224)           => variable(134) & ~( __N_6(403) ).	 # conjunctive
	p(NT(224), (NT(134)) & ~(NT(403)));
//G356: ctnvar(188)          => chars(220).
	p(NT(188), (NT(220)));
//G357: __E_offsets_105(227) => _(11) ',' _(11) offset(219).
	p(NT(227), (NT(11)+T(20)+NT(11)+NT(219)));
//G358: __E_offsets_106(228) => null.
	p(NT(228), (nul));
//G359: __E_offsets_106(228) => __E_offsets_105(227) __E_offsets_106(228).
	p(NT(228), (NT(227)+NT(228)));
//G360: offsets(42)          => '[' _(11) offset(219) __E_offsets_106(228) _(11) ']'.
	p(NT(42), (T(26)+NT(11)+NT(219)+NT(228)+NT(11)+T(27)));
//G361: offset(219)          => integer(229).
	p(NT(219), (NT(229)));
//G362: offset(219)          => capture(29).
	p(NT(219), (NT(29)));
//G363: offset(219)          => shift(230).
	p(NT(219), (NT(230)));
//G364: __N_7(404)           => io_var(211).
	p(NT(404), (NT(211)));
//G365: __E_offset_107(231)  => variable(134) & ~( __N_7(404) ).	 # conjunctive
	p(NT(231), (NT(134)) & ~(NT(404)));
//G366: offset(219)          => __E_offset_107(231).
	p(NT(219), (NT(231)));
//G367: __E_shift_108(232)   => capture(29).
	p(NT(232), (NT(29)));
//G368: __N_8(405)           => io_var(211).
	p(NT(405), (NT(211)));
//G369: __E___E_shift_108_109(233) => variable(134) & ~( __N_8(405) ).	 # conjunctive
	p(NT(233), (NT(134)) & ~(NT(405)));
//G370: __E_shift_108(232)   => __E___E_shift_108_109(233).
	p(NT(232), (NT(233)));
//G371: shift(230)           => __E_shift_108(232) _(11) '-' _(11) num(189).
	p(NT(230), (NT(232)+NT(11)+T(30)+NT(11)+NT(189)));
//G372: __E_chars_110(234)   => alnum(6).
	p(NT(234), (NT(6)));
//G373: __E_chars_110(234)   => '_'.
	p(NT(234), (T(37)));
//G374: __E_chars_111(235)   => null.
	p(NT(235), (nul));
//G375: __E_chars_111(235)   => __E_chars_110(234) __E_chars_111(235).
	p(NT(235), (NT(234)+NT(235)));
//G376: chars(220)           => alpha(5) __E_chars_111(235).
	p(NT(220), (NT(5)+NT(235)));
//G377: __E_digits_112(237)  => digit(3).
	p(NT(237), (NT(3)));
//G378: __E_digits_112(237)  => digit(3) __E_digits_112(237).
	p(NT(237), (NT(3)+NT(237)));
//G379: digits(236)          => __E_digits_112(237).
	p(NT(236), (NT(237)));
//G380: num(189)             => digits(236).
	p(NT(189), (NT(236)));
//G381: __E_integer_113(238) => '-'.
	p(NT(238), (T(30)));
//G382: __E_integer_113(238) => null.
	p(NT(238), (nul));
//G383: integer(229)         => __E_integer_113(238) _(11) digits(236).
	p(NT(229), (NT(238)+NT(11)+NT(236)));
//G384: sym(40)              => chars(220).
	p(NT(40), (NT(220)));
//G385: capture(29)          => '$' chars(220).
	p(NT(29), (T(48)+NT(220)));
//G386: __E_typed_114(240)   => _(11) '[' _(11) subtype(241) _(11) ']' _(11).
	p(NT(240), (NT(11)+T(26)+NT(11)+NT(241)+NT(11)+T(27)+NT(11)));
//G387: __E_typed_114(240)   => null.
	p(NT(240), (nul));
//G388: typed(33)            => _(11) ':' _(11) type(239) __E_typed_114(240).
	p(NT(33), (NT(11)+T(2)+NT(11)+NT(239)+NT(240)));
//G389: type(239)            => chars(220).
	p(NT(239), (NT(220)));
//G390: subtype(241)         => num(189).
	p(NT(241), (NT(189)));
//G391: __E_comment_115(243) => printable(8).
	p(NT(243), (NT(8)));
//G392: __E_comment_115(243) => '\t'.
	p(NT(243), (T(49)));
//G393: __E_comment_116(244) => null.
	p(NT(244), (nul));
//G394: __E_comment_116(244) => __E_comment_115(243) __E_comment_116(244).
	p(NT(244), (NT(243)+NT(244)));
//G395: __E_comment_117(245) => '\n'.
	p(NT(245), (T(50)));
//G396: __E_comment_117(245) => '\r'.
	p(NT(245), (T(51)));
//G397: __E_comment_117(245) => eof(1).
	p(NT(245), (NT(1)));
//G398: comment(242)         => '#' __E_comment_116(244) __E_comment_117(245).
	p(NT(242), (T(52)+NT(244)+NT(245)));
//G399: __(34)               => space(2).
	p(NT(34), (NT(2)));
//G400: __(34)               => comment(242).
	p(NT(34), (NT(242)));
//G401: __(34)               => __(34) space(2).
	p(NT(34), (NT(34)+NT(2)));
//G402: __(34)               => __(34) comment(242).
	p(NT(34), (NT(34)+NT(242)));
//G403: _(11)                => __(34).
	p(NT(11), (NT(34)));
//G404: _(11)                => null.
	p(NT(11), (nul));
//G405: cli(246)             => _(11).
	p(NT(246), (NT(11)));
//G406: __E_cli_118(248)     => '.' _(11) cli_command(247) _(11).
	p(NT(248), (T(1)+NT(11)+NT(247)+NT(11)));
//G407: __E_cli_119(249)     => null.
	p(NT(249), (nul));
//G408: __E_cli_119(249)     => __E_cli_118(248) __E_cli_119(249).
	p(NT(249), (NT(248)+NT(249)));
//G409: cli(246)             => _(11) cli_command(247) _(11) __E_cli_119(249).
	p(NT(246), (NT(11)+NT(247)+NT(11)+NT(249)));
//G410: __E_cli_command_120(252) => 'q'.
	p(NT(252), (T(53)));
//G411: __E_cli_command_120(252) => 'q' 'u' 'i' 't'.
	p(NT(252), (T(53)+T(7)+T(4)+T(8)));
//G412: quit_sym(251)        => __E_cli_command_120(252).
	p(NT(251), (NT(252)));
//G413: quit_cmd(250)        => quit_sym(251).
	p(NT(250), (NT(251)));
//G414: cli_command(247)     => quit_cmd(250).
	p(NT(247), (NT(250)));
//G415: __E_cli_command_121(255) => 'v'.
	p(NT(255), (T(54)));
//G416: __E_cli_command_121(255) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(255), (T(54)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G417: version_sym(254)     => __E_cli_command_121(255).
	p(NT(254), (NT(255)));
//G418: version_cmd(253)     => version_sym(254).
	p(NT(253), (NT(254)));
//G419: cli_command(247)     => version_cmd(253).
	p(NT(247), (NT(253)));
//G420: __E_cli_command_122(258) => 'c'.
	p(NT(258), (T(14)));
//G421: __E_cli_command_122(258) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(258), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G422: clear_sym(257)       => __E_cli_command_122(258).
	p(NT(257), (NT(258)));
//G423: clear_cmd(256)       => clear_sym(257).
	p(NT(256), (NT(257)));
//G424: cli_command(247)     => clear_cmd(256).
	p(NT(247), (NT(256)));
//G425: __E___E_cli_command_123_124(262) => 'h'.
	p(NT(262), (T(55)));
//G426: __E___E_cli_command_123_124(262) => 'h' 'e' 'l' 'p'.
	p(NT(262), (T(55)+T(11)+T(10)+T(56)));
//G427: help_sym(261)        => __E___E_cli_command_123_124(262).
	p(NT(261), (NT(262)));
//G428: __E___E_cli_command_123_125(263) => __(34) help_arg(264).
	p(NT(263), (NT(34)+NT(264)));
//G429: __E___E_cli_command_123_125(263) => null.
	p(NT(263), (nul));
//G430: __E_cli_command_123(260) => help_sym(261) __E___E_cli_command_123_125(263).
	p(NT(260), (NT(261)+NT(263)));
//G431: help_cmd(259)        => __E_cli_command_123(260).
	p(NT(259), (NT(260)));
//G432: cli_command(247)     => help_cmd(259).
	p(NT(247), (NT(259)));
//G433: file_sym(267)        => 'f' 'i' 'l' 'e'.
	p(NT(267), (T(9)+T(4)+T(10)+T(11)));
//G434: __E_cli_command_126(266) => file_sym(267) __(34) q_string(268).
	p(NT(266), (NT(267)+NT(34)+NT(268)));
//G435: file_cmd(265)        => __E_cli_command_126(266).
	p(NT(265), (NT(266)));
//G436: cli_command(247)     => file_cmd(265).
	p(NT(247), (NT(265)));
//G437: valid_sym(271)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(271), (T(54)+T(16)+T(10)+T(4)+T(57)));
//G438: __E_cli_command_127(270) => valid_sym(271) __(34) normalize_cmd_arg(272).
	p(NT(270), (NT(271)+NT(34)+NT(272)));
//G439: valid_cmd(269)       => __E_cli_command_127(270).
	p(NT(269), (NT(270)));
//G440: cli_command(247)     => valid_cmd(269).
	p(NT(247), (NT(269)));
//G441: sat_sym(275)         => 's' 'a' 't'.
	p(NT(275), (T(15)+T(16)+T(8)));
//G442: __E_cli_command_128(274) => sat_sym(275) __(34) normalize_cmd_arg(272).
	p(NT(274), (NT(275)+NT(34)+NT(272)));
//G443: sat_cmd(273)         => __E_cli_command_128(274).
	p(NT(273), (NT(274)));
//G444: cli_command(247)     => sat_cmd(273).
	p(NT(247), (NT(273)));
//G445: unsat_sym(278)       => 'u' 'n' 's' 'a' 't'.
	p(NT(278), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G446: __E_cli_command_129(277) => unsat_sym(278) __(34) normalize_cmd_arg(272).
	p(NT(277), (NT(278)+NT(34)+NT(272)));
//G447: unsat_cmd(276)       => __E_cli_command_129(277).
	p(NT(276), (NT(277)));
//G448: cli_command(247)     => unsat_cmd(276).
	p(NT(247), (NT(276)));
//G449: solve_sym(281)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(281), (T(15)+T(6)+T(10)+T(54)+T(11)));
//G450: __E___E_cli_command_130_131(282) => solve_options(283).
	p(NT(282), (NT(283)));
//G451: __E___E_cli_command_130_131(282) => null.
	p(NT(282), (nul));
//G452: __E_cli_command_130(280) => solve_sym(281) __E___E_cli_command_130_131(282) __(34) wff_cmd_arg(284).
	p(NT(280), (NT(281)+NT(282)+NT(34)+NT(284)));
//G453: solve_cmd(279)       => __E_cli_command_130(280).
	p(NT(279), (NT(280)));
//G454: cli_command(247)     => solve_cmd(279).
	p(NT(247), (NT(279)));
//G455: lgrs_sym(287)        => 'l' 'g' 'r' 's'.
	p(NT(287), (T(10)+T(58)+T(19)+T(15)));
//G456: __E_cli_command_132(286) => lgrs_sym(287) __(34) wff_cmd_arg(284).
	p(NT(286), (NT(287)+NT(34)+NT(284)));
//G457: lgrs_cmd(285)        => __E_cli_command_132(286).
	p(NT(285), (NT(286)));
//G458: cli_command(247)     => lgrs_cmd(285).
	p(NT(247), (NT(285)));
//G459: __E___E_cli_command_133_134(291) => 'r'.
	p(NT(291), (T(19)));
//G460: __E___E_cli_command_133_134(291) => 'r' 'u' 'n'.
	p(NT(291), (T(19)+T(7)+T(5)));
//G461: run_sym(290)         => __E___E_cli_command_133_134(291).
	p(NT(290), (NT(291)));
//G462: __E___E_cli_command_133_135(292) => history(293).
	p(NT(292), (NT(293)));
//G463: __E___E_cli_command_133_135(292) => wff(14).
	p(NT(292), (NT(14)));
//G464: __E_cli_command_133(289) => run_sym(290) __(34) __E___E_cli_command_133_135(292).
	p(NT(289), (NT(290)+NT(34)+NT(292)));
//G465: run_cmd(288)         => __E_cli_command_133(289).
	p(NT(288), (NT(289)));
//G466: cli_command(247)     => run_cmd(288).
	p(NT(247), (NT(288)));
//G467: __E___E_cli_command_136_137(297) => 'n'.
	p(NT(297), (T(5)));
//G468: __E___E_cli_command_136_137(297) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(297), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(59)+T(11)));
//G469: normalize_sym(296)   => __E___E_cli_command_136_137(297).
	p(NT(296), (NT(297)));
//G470: __E_cli_command_136(295) => normalize_sym(296) __(34) normalize_cmd_arg(272).
	p(NT(295), (NT(296)+NT(34)+NT(272)));
//G471: normalize_cmd(294)   => __E_cli_command_136(295).
	p(NT(294), (NT(295)));
//G472: cli_command(247)     => normalize_cmd(294).
	p(NT(247), (NT(294)));
//G473: __E___E_cli_command_138_139(301) => 's'.
	p(NT(301), (T(15)));
//G474: __E___E_cli_command_138_139(301) => 's' 'u' 'b' 's' 't'.
	p(NT(301), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G475: __E___E_cli_command_138_139(301) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(301), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G476: subst_sym(300)       => __E___E_cli_command_138_139(301).
	p(NT(300), (NT(301)));
//G477: __E_cli_command_138(299) => subst_sym(300) __(34) nf_cmd_arg(302) _(11) '[' _(11) nf_cmd_arg(302) _(11) '/' _(11) nf_cmd_arg(302) _(11) ']'.
	p(NT(299), (NT(300)+NT(34)+NT(302)+NT(11)+T(26)+NT(11)+NT(302)+NT(11)+T(43)+NT(11)+NT(302)+NT(11)+T(27)));
//G478: subst_cmd(298)       => __E_cli_command_138(299).
	p(NT(298), (NT(299)));
//G479: cli_command(247)     => subst_cmd(298).
	p(NT(247), (NT(298)));
//G480: __E___E_cli_command_140_141(306) => 'i'.
	p(NT(306), (T(4)));
//G481: __E___E_cli_command_140_141(306) => 'i' 'n' 's' 't'.
	p(NT(306), (T(4)+T(5)+T(15)+T(8)));
//G482: __E___E_cli_command_140_141(306) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(306), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G483: inst_sym(305)        => __E___E_cli_command_140_141(306).
	p(NT(305), (NT(306)));
//G484: __E_cli_command_140(304) => inst_sym(305) __(34) inst_args(307).
	p(NT(304), (NT(305)+NT(34)+NT(307)));
//G485: inst_cmd(303)        => __E_cli_command_140(304).
	p(NT(303), (NT(304)));
//G486: cli_command(247)     => inst_cmd(303).
	p(NT(247), (NT(303)));
//G487: dnf_sym(310)         => 'd' 'n' 'f'.
	p(NT(310), (T(57)+T(5)+T(9)));
//G488: __E_cli_command_142(309) => dnf_sym(310) __(34) nf_cmd_arg(302).
	p(NT(309), (NT(310)+NT(34)+NT(302)));
//G489: dnf_cmd(308)         => __E_cli_command_142(309).
	p(NT(308), (NT(309)));
//G490: cli_command(247)     => dnf_cmd(308).
	p(NT(247), (NT(308)));
//G491: cnf_sym(313)         => 'c' 'n' 'f'.
	p(NT(313), (T(14)+T(5)+T(9)));
//G492: __E_cli_command_143(312) => cnf_sym(313) __(34) nf_cmd_arg(302).
	p(NT(312), (NT(313)+NT(34)+NT(302)));
//G493: cnf_cmd(311)         => __E_cli_command_143(312).
	p(NT(311), (NT(312)));
//G494: cli_command(247)     => cnf_cmd(311).
	p(NT(247), (NT(311)));
//G495: anf_sym(316)         => 'a' 'n' 'f'.
	p(NT(316), (T(16)+T(5)+T(9)));
//G496: __E_cli_command_144(315) => anf_sym(316) __(34) nf_cmd_arg(302).
	p(NT(315), (NT(316)+NT(34)+NT(302)));
//G497: anf_cmd(314)         => __E_cli_command_144(315).
	p(NT(314), (NT(315)));
//G498: cli_command(247)     => anf_cmd(314).
	p(NT(247), (NT(314)));
//G499: nnf_sym(319)         => 'n' 'n' 'f'.
	p(NT(319), (T(5)+T(5)+T(9)));
//G500: __E_cli_command_145(318) => nnf_sym(319) __(34) nf_cmd_arg(302).
	p(NT(318), (NT(319)+NT(34)+NT(302)));
//G501: nnf_cmd(317)         => __E_cli_command_145(318).
	p(NT(317), (NT(318)));
//G502: cli_command(247)     => nnf_cmd(317).
	p(NT(247), (NT(317)));
//G503: pnf_sym(322)         => 'p' 'n' 'f'.
	p(NT(322), (T(56)+T(5)+T(9)));
//G504: __E_cli_command_146(321) => pnf_sym(322) __(34) nf_cmd_arg(302).
	p(NT(321), (NT(322)+NT(34)+NT(302)));
//G505: pnf_cmd(320)         => __E_cli_command_146(321).
	p(NT(320), (NT(321)));
//G506: cli_command(247)     => pnf_cmd(320).
	p(NT(247), (NT(320)));
//G507: mnf_sym(325)         => 'm' 'n' 'f'.
	p(NT(325), (T(21)+T(5)+T(9)));
//G508: __E_cli_command_147(324) => mnf_sym(325) __(34) nf_cmd_arg(302).
	p(NT(324), (NT(325)+NT(34)+NT(302)));
//G509: mnf_cmd(323)         => __E_cli_command_147(324).
	p(NT(323), (NT(324)));
//G510: cli_command(247)     => mnf_cmd(323).
	p(NT(247), (NT(323)));
//G511: onf_sym(328)         => 'o' 'n' 'f'.
	p(NT(328), (T(6)+T(5)+T(9)));
//G512: __E_cli_command_148(327) => onf_sym(328) __(34) variable(134) __(34) onf_cmd_arg(329).
	p(NT(327), (NT(328)+NT(34)+NT(134)+NT(34)+NT(329)));
//G513: onf_cmd(326)         => __E_cli_command_148(327).
	p(NT(326), (NT(327)));
//G514: cli_command(247)     => onf_cmd(326).
	p(NT(247), (NT(326)));
//G515: qelim_sym(332)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(332), (T(53)+T(11)+T(10)+T(4)+T(21)));
//G516: __E_cli_command_149(331) => qelim_sym(332) __(34) wff_cmd_arg(284).
	p(NT(331), (NT(332)+NT(34)+NT(284)));
//G517: qelim_cmd(330)       => __E_cli_command_149(331).
	p(NT(330), (NT(331)));
//G518: cli_command(247)     => qelim_cmd(330).
	p(NT(247), (NT(330)));
//G519: get_sym(335)         => 'g' 'e' 't'.
	p(NT(335), (T(58)+T(11)+T(8)));
//G520: __E___E_cli_command_150_151(336) => __(34) option_name(337).
	p(NT(336), (NT(34)+NT(337)));
//G521: __E___E_cli_command_150_151(336) => null.
	p(NT(336), (nul));
//G522: __E_cli_command_150(334) => get_sym(335) __E___E_cli_command_150_151(336).
	p(NT(334), (NT(335)+NT(336)));
//G523: get_cmd(333)         => __E_cli_command_150(334).
	p(NT(333), (NT(334)));
//G524: cli_command(247)     => get_cmd(333).
	p(NT(247), (NT(333)));
//G525: set_sym(340)         => 's' 'e' 't'.
	p(NT(340), (T(15)+T(11)+T(8)));
//G526: __E___E_cli_command_152_153(341) => __(34).
	p(NT(341), (NT(34)));
//G527: __E___E_cli_command_152_153(341) => _(11) '=' _(11).
	p(NT(341), (NT(11)+T(3)+NT(11)));
//G528: __E_cli_command_152(339) => set_sym(340) __(34) option_name(337) __E___E_cli_command_152_153(341) option_value(342).
	p(NT(339), (NT(340)+NT(34)+NT(337)+NT(341)+NT(342)));
//G529: set_cmd(338)         => __E_cli_command_152(339).
	p(NT(338), (NT(339)));
//G530: cli_command(247)     => set_cmd(338).
	p(NT(247), (NT(338)));
//G531: enable_sym(345)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(345), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G532: __E_cli_command_154(344) => enable_sym(345) __(34) option_name(337).
	p(NT(344), (NT(345)+NT(34)+NT(337)));
//G533: enable_cmd(343)      => __E_cli_command_154(344).
	p(NT(343), (NT(344)));
//G534: cli_command(247)     => enable_cmd(343).
	p(NT(247), (NT(343)));
//G535: disable_sym(348)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(348), (T(57)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G536: __E_cli_command_155(347) => disable_sym(348) __(34) option_name(337).
	p(NT(347), (NT(348)+NT(34)+NT(337)));
//G537: disable_cmd(346)     => __E_cli_command_155(347).
	p(NT(346), (NT(347)));
//G538: cli_command(247)     => disable_cmd(346).
	p(NT(247), (NT(346)));
//G539: toggle_sym(351)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(351), (T(8)+T(6)+T(58)+T(58)+T(10)+T(11)));
//G540: __E_cli_command_156(350) => toggle_sym(351) __(34) option_name(337).
	p(NT(350), (NT(351)+NT(34)+NT(337)));
//G541: toggle_cmd(349)      => __E_cli_command_156(350).
	p(NT(349), (NT(350)));
//G542: cli_command(247)     => toggle_cmd(349).
	p(NT(247), (NT(349)));
//G543: __E___E_cli_command_157_158(355) => 'd' 'e' 'f' 's'.
	p(NT(355), (T(57)+T(11)+T(9)+T(15)));
//G544: __E___E_cli_command_157_158(355) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(355), (T(57)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G545: def_sym(354)         => __E___E_cli_command_157_158(355).
	p(NT(354), (NT(355)));
//G546: __E_cli_command_157(353) => def_sym(354).
	p(NT(353), (NT(354)));
//G547: def_list_cmd(352)    => __E_cli_command_157(353).
	p(NT(352), (NT(353)));
//G548: cli_command(247)     => def_list_cmd(352).
	p(NT(247), (NT(352)));
//G549: __E_cli_command_159(357) => def_sym(354) __(34) num(189).
	p(NT(357), (NT(354)+NT(34)+NT(189)));
//G550: def_print_cmd(356)   => __E_cli_command_159(357).
	p(NT(356), (NT(357)));
//G551: cli_command(247)     => def_print_cmd(356).
	p(NT(247), (NT(356)));
//G552: def_rr_cmd(358)      => rec_relation(17).
	p(NT(358), (NT(17)));
//G553: cli_command(247)     => def_rr_cmd(358).
	p(NT(247), (NT(358)));
//G554: def_input_cmd(359)   => input_def(18).
	p(NT(359), (NT(18)));
//G555: cli_command(247)     => def_input_cmd(359).
	p(NT(247), (NT(359)));
//G556: def_output_cmd(360)  => output_def(19).
	p(NT(360), (NT(19)));
//G557: cli_command(247)     => def_output_cmd(360).
	p(NT(247), (NT(360)));
//G558: __E___E_cli_command_160_161(364) => 'h' 'i' 's' 't'.
	p(NT(364), (T(55)+T(4)+T(15)+T(8)));
//G559: __E___E_cli_command_160_161(364) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(364), (T(55)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G560: history_sym(363)     => __E___E_cli_command_160_161(364).
	p(NT(363), (NT(364)));
//G561: __E_cli_command_160(362) => history_sym(363).
	p(NT(362), (NT(363)));
//G562: history_list_cmd(361) => __E_cli_command_160(362).
	p(NT(361), (NT(362)));
//G563: cli_command(247)     => history_list_cmd(361).
	p(NT(247), (NT(361)));
//G564: __E_cli_command_162(366) => history_sym(363) __(34) history(293).
	p(NT(366), (NT(363)+NT(34)+NT(293)));
//G565: history_print_cmd(365) => __E_cli_command_162(366).
	p(NT(365), (NT(366)));
//G566: cli_command(247)     => history_print_cmd(365).
	p(NT(247), (NT(365)));
//G567: __E_cli_command_163(368) => wff(14).
	p(NT(368), (NT(14)));
//G568: __E_cli_command_163(368) => bf(30).
	p(NT(368), (NT(30)));
//G569: history_store_cmd(367) => __E_cli_command_163(368).
	p(NT(367), (NT(368)));
//G570: cli_command(247)     => history_store_cmd(367).
	p(NT(247), (NT(367)));
//G571: __E_solve_options_164(369) => __(34) solve_option(370).
	p(NT(369), (NT(34)+NT(370)));
//G572: __E_solve_options_165(371) => null.
	p(NT(371), (nul));
//G573: __E_solve_options_165(371) => __E_solve_options_164(369) __E_solve_options_165(371).
	p(NT(371), (NT(369)+NT(371)));
//G574: solve_options(283)   => __E_solve_options_165(371).
	p(NT(283), (NT(371)));
//G575: __E_solve_option_166(372) => solver_mode(373).
	p(NT(372), (NT(373)));
//G576: __E_solve_option_166(372) => type(239).
	p(NT(372), (NT(239)));
//G577: solve_option(370)    => '-' '-' __E_solve_option_166(372).
	p(NT(370), (T(30)+T(30)+NT(372)));
//G578: __E_solver_mode_167(375) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(375), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G579: __E_solver_mode_167(375) => 'm' 'i' 'n'.
	p(NT(375), (T(21)+T(4)+T(5)));
//G580: solver_mode_minimum(374) => __E_solver_mode_167(375).
	p(NT(374), (NT(375)));
//G581: solver_mode(373)     => solver_mode_minimum(374).
	p(NT(373), (NT(374)));
//G582: __E_solver_mode_168(377) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(377), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G583: __E_solver_mode_168(377) => 'm' 'a' 'x'.
	p(NT(377), (T(21)+T(16)+T(29)));
//G584: solver_mode_maximum(376) => __E_solver_mode_168(377).
	p(NT(376), (NT(377)));
//G585: solver_mode(373)     => solver_mode_maximum(376).
	p(NT(373), (NT(376)));
//G586: q_file_name(38)      => '"' file_name(378) '"'.
	p(NT(38), (T(60)+NT(378)+T(60)));
//G587: __E_file_name_169(379) => printable(8).
	p(NT(379), (NT(8)));
//G588: __E_file_name_169(379) => printable(8) __E_file_name_169(379).
	p(NT(379), (NT(8)+NT(379)));
//G589: file_name(378)       => __E_file_name_169(379).
	p(NT(378), (NT(379)));
//G590: __E_option_name_170(380) => alnum(6).
	p(NT(380), (NT(6)));
//G591: __E_option_name_170(380) => alnum(6) __E_option_name_170(380).
	p(NT(380), (NT(6)+NT(380)));
//G592: option_name(337)     => __E_option_name_170(380).
	p(NT(337), (NT(380)));
//G593: __E_option_value_171(381) => alnum(6).
	p(NT(381), (NT(6)));
//G594: __E_option_value_171(381) => alnum(6) __E_option_value_171(381).
	p(NT(381), (NT(6)+NT(381)));
//G595: option_value(342)    => __E_option_value_171(381).
	p(NT(342), (NT(381)));
//G596: bf_cmd_arg(382)      => history(293).
	p(NT(382), (NT(293)));
//G597: bf_cmd_arg(382)      => bf(30).
	p(NT(382), (NT(30)));
//G598: wff_cmd_arg(284)     => history(293).
	p(NT(284), (NT(293)));
//G599: wff_cmd_arg(284)     => wff(14).
	p(NT(284), (NT(14)));
//G600: nf_cmd_arg(302)      => history(293).
	p(NT(302), (NT(293)));
//G601: nf_cmd_arg(302)      => ref(27).
	p(NT(302), (NT(27)));
//G602: nf_cmd_arg(302)      => wff(14).
	p(NT(302), (NT(14)));
//G603: nf_cmd_arg(302)      => bf(30).
	p(NT(302), (NT(30)));
//G604: onf_cmd_arg(329)     => history(293).
	p(NT(329), (NT(293)));
//G605: onf_cmd_arg(329)     => wff(14).
	p(NT(329), (NT(14)));
//G606: normalize_cmd_arg(272) => history(293).
	p(NT(272), (NT(293)));
//G607: normalize_cmd_arg(272) => spec(10).
	p(NT(272), (NT(10)));
//G608: normalize_cmd_arg(272) => ref(27).
	p(NT(272), (NT(27)));
//G609: normalize_cmd_arg(272) => wff(14).
	p(NT(272), (NT(14)));
//G610: normalize_cmd_arg(272) => bf(30).
	p(NT(272), (NT(30)));
//G611: inst_args(307)       => wff_cmd_arg(284) _(11) '[' _(11) variable(134) _(11) '/' _(11) bf_cmd_arg(382) _(11) ']'.
	p(NT(307), (NT(284)+NT(11)+T(26)+NT(11)+NT(134)+NT(11)+T(43)+NT(11)+NT(382)+NT(11)+T(27)));
//G612: inst_args(307)       => bf_cmd_arg(382) _(11) '[' _(11) variable(134) _(11) '/' _(11) bf_cmd_arg(382) _(11) ']'.
	p(NT(307), (NT(382)+NT(11)+T(26)+NT(11)+NT(134)+NT(11)+T(43)+NT(11)+NT(382)+NT(11)+T(27)));
//G613: help_arg(264)        => help_sym(261).
	p(NT(264), (NT(261)));
//G614: help_arg(264)        => version_sym(254).
	p(NT(264), (NT(254)));
//G615: help_arg(264)        => quit_sym(251).
	p(NT(264), (NT(251)));
//G616: help_arg(264)        => clear_sym(257).
	p(NT(264), (NT(257)));
//G617: help_arg(264)        => get_sym(335).
	p(NT(264), (NT(335)));
//G618: help_arg(264)        => set_sym(340).
	p(NT(264), (NT(340)));
//G619: help_arg(264)        => enable_sym(345).
	p(NT(264), (NT(345)));
//G620: help_arg(264)        => disable_sym(348).
	p(NT(264), (NT(348)));
//G621: help_arg(264)        => toggle_sym(351).
	p(NT(264), (NT(351)));
//G622: help_arg(264)        => file_sym(267).
	p(NT(264), (NT(267)));
//G623: help_arg(264)        => history_sym(363).
	p(NT(264), (NT(363)));
//G624: help_arg(264)        => abs_history_sym(383).
	p(NT(264), (NT(383)));
//G625: help_arg(264)        => rel_history_sym(384).
	p(NT(264), (NT(384)));
//G626: help_arg(264)        => selection_sym(385).
	p(NT(264), (NT(385)));
//G627: help_arg(264)        => def_sym(354).
	p(NT(264), (NT(354)));
//G628: help_arg(264)        => inst_sym(305).
	p(NT(264), (NT(305)));
//G629: help_arg(264)        => subst_sym(300).
	p(NT(264), (NT(300)));
//G630: help_arg(264)        => normalize_sym(296).
	p(NT(264), (NT(296)));
//G631: help_arg(264)        => execute_sym(386).
	p(NT(264), (NT(386)));
//G632: help_arg(264)        => solve_sym(281).
	p(NT(264), (NT(281)));
//G633: help_arg(264)        => valid_sym(271).
	p(NT(264), (NT(271)));
//G634: help_arg(264)        => sat_sym(275).
	p(NT(264), (NT(275)));
//G635: help_arg(264)        => unsat_sym(278).
	p(NT(264), (NT(278)));
//G636: help_arg(264)        => run_sym(290).
	p(NT(264), (NT(290)));
//G637: help_arg(264)        => dnf_sym(310).
	p(NT(264), (NT(310)));
//G638: help_arg(264)        => cnf_sym(313).
	p(NT(264), (NT(313)));
//G639: help_arg(264)        => anf_sym(316).
	p(NT(264), (NT(316)));
//G640: help_arg(264)        => snf_sym(387).
	p(NT(264), (NT(387)));
//G641: help_arg(264)        => nnf_sym(319).
	p(NT(264), (NT(319)));
//G642: help_arg(264)        => pnf_sym(322).
	p(NT(264), (NT(322)));
//G643: help_arg(264)        => mnf_sym(325).
	p(NT(264), (NT(325)));
//G644: help_arg(264)        => onf_sym(328).
	p(NT(264), (NT(328)));
//G645: help_arg(264)        => qelim_sym(332).
	p(NT(264), (NT(332)));
//G646: __E___E_help_arg_172_173(390) => 's'.
	p(NT(390), (T(15)));
//G647: __E___E_help_arg_172_173(390) => null.
	p(NT(390), (nul));
//G648: __E_help_arg_172(389) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_172_173(390).
	p(NT(389), (T(11)+T(29)+T(16)+T(21)+T(56)+T(10)+T(11)+NT(390)));
//G649: examples_sym(388)    => __E_help_arg_172(389).
	p(NT(388), (NT(389)));
//G650: help_arg(264)        => examples_sym(388).
	p(NT(264), (NT(388)));
//G651: __E_history_174(392) => '%'.
	p(NT(392), (T(44)));
//G652: rel_history_sym(384) => '%' '-'.
	p(NT(384), (T(44)+T(30)));
//G653: history_id(394)      => digits(236).
	p(NT(394), (NT(236)));
//G654: __E___E_history_174_175(393) => history_id(394).
	p(NT(393), (NT(394)));
//G655: __E___E_history_174_175(393) => null.
	p(NT(393), (nul));
//G656: __E_history_174(392) => rel_history_sym(384) __E___E_history_174_175(393).
	p(NT(392), (NT(384)+NT(393)));
//G657: rel_history(391)     => __E_history_174(392).
	p(NT(391), (NT(392)));
//G658: history(293)         => rel_history(391).
	p(NT(293), (NT(391)));
//G659: abs_history_sym(383) => '%'.
	p(NT(383), (T(44)));
//G660: __E_history_176(396) => abs_history_sym(383) history_id(394).
	p(NT(396), (NT(383)+NT(394)));
//G661: abs_history(395)     => __E_history_176(396).
	p(NT(395), (NT(396)));
//G662: history(293)         => abs_history(395).
	p(NT(293), (NT(395)));
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
		__E___E_bf_67_68, bf_and_nosep_2nd_oprnd, __E___E_bf_67_69, bv_parenthesis, __E_bv_70, bv_add, __E_bv_71, bv_mul, __E_bv_72, bv_div, 
		__E_bv_73, bv_mod, __E_bv_74, bv_sub, __E_bv_75, bv_neg, bv_and, __E_bv_76, bv_or, __E_bv_77, 
		bv_xor, __E_bv_78, bv_shl, __E_bv_79, bv_shr, __E_bv_80, ctn_neq, __E_constraint_81, ctnvar, num, 
		ctn_eq, __E_constraint_82, ctn_gteq, __E_constraint_83, ctn_gt, __E_constraint_84, ctn_lteq, __E_constraint_85, ctn_lt, __E_constraint_86, 
		__E_ba_constant_87, source, __E_ba_constant_88, src_c, __E_source_89, __E___E_source_89_90, __E___E_source_89_91, __E_src_c_92, __E_src_c_93, __E_variable_94, 
		uconst, io_var, var_name, __E_variable_95, __E_var_name_96, __E_var_name_97, __E_var_name_98, __E_var_name_99, __E_var_name_100, offset, 
		chars, uconst_name, __E_uconst_101, __E___E_uconst_101_102, q_var, __E_q_vars_103, __E_q_vars_104, __E_offsets_105, __E_offsets_106, integer, 
		shift, __E_offset_107, __E_shift_108, __E___E_shift_108_109, __E_chars_110, __E_chars_111, digits, __E_digits_112, __E_integer_113, type, 
		__E_typed_114, subtype, comment, __E_comment_115, __E_comment_116, __E_comment_117, cli, cli_command, __E_cli_118, __E_cli_119, 
		quit_cmd, quit_sym, __E_cli_command_120, version_cmd, version_sym, __E_cli_command_121, clear_cmd, clear_sym, __E_cli_command_122, help_cmd, 
		__E_cli_command_123, help_sym, __E___E_cli_command_123_124, __E___E_cli_command_123_125, help_arg, file_cmd, __E_cli_command_126, file_sym, q_string, valid_cmd, 
		__E_cli_command_127, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_128, sat_sym, unsat_cmd, __E_cli_command_129, unsat_sym, solve_cmd, 
		__E_cli_command_130, solve_sym, __E___E_cli_command_130_131, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_132, lgrs_sym, run_cmd, __E_cli_command_133, 
		run_sym, __E___E_cli_command_133_134, __E___E_cli_command_133_135, history, normalize_cmd, __E_cli_command_136, normalize_sym, __E___E_cli_command_136_137, subst_cmd, __E_cli_command_138, 
		subst_sym, __E___E_cli_command_138_139, nf_cmd_arg, inst_cmd, __E_cli_command_140, inst_sym, __E___E_cli_command_140_141, inst_args, dnf_cmd, __E_cli_command_142, 
		dnf_sym, cnf_cmd, __E_cli_command_143, cnf_sym, anf_cmd, __E_cli_command_144, anf_sym, nnf_cmd, __E_cli_command_145, nnf_sym, 
		pnf_cmd, __E_cli_command_146, pnf_sym, mnf_cmd, __E_cli_command_147, mnf_sym, onf_cmd, __E_cli_command_148, onf_sym, onf_cmd_arg, 
		qelim_cmd, __E_cli_command_149, qelim_sym, get_cmd, __E_cli_command_150, get_sym, __E___E_cli_command_150_151, option_name, set_cmd, __E_cli_command_152, 
		set_sym, __E___E_cli_command_152_153, option_value, enable_cmd, __E_cli_command_154, enable_sym, disable_cmd, __E_cli_command_155, disable_sym, toggle_cmd, 
		__E_cli_command_156, toggle_sym, def_list_cmd, __E_cli_command_157, def_sym, __E___E_cli_command_157_158, def_print_cmd, __E_cli_command_159, def_rr_cmd, def_input_cmd, 
		def_output_cmd, history_list_cmd, __E_cli_command_160, history_sym, __E___E_cli_command_160_161, history_print_cmd, __E_cli_command_162, history_store_cmd, __E_cli_command_163, __E_solve_options_164, 
		solve_option, __E_solve_options_165, __E_solve_option_166, solver_mode, solver_mode_minimum, __E_solver_mode_167, solver_mode_maximum, __E_solver_mode_168, file_name, __E_file_name_169, 
		__E_option_name_170, __E_option_value_171, bf_cmd_arg, abs_history_sym, rel_history_sym, selection_sym, execute_sym, snf_sym, examples_sym, __E_help_arg_172, 
		__E___E_help_arg_172_173, rel_history, __E_history_174, __E___E_history_174_175, history_id, abs_history, __E_history_176, __N_0, __N_1, __N_2, 
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
