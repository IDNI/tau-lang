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
	"bf_eq", "__E_wff_34", "bf_neq", "__E_wff_35", "bf_lt", "__E_wff_36", "bf_nlt", "__E_wff_37", "bf_lteq", "__E_wff_38", 
	"bf_nlteq", "__E_wff_39", "bf_gt", "__E_wff_40", "bf_ngt", "__E_wff_41", "bf_gteq", "__E_wff_42", "bf_ngteq", "__E_wff_43", 
	"bf_parenthesis", "__E_bf_44", "ba_constant", "variable", "bf_fall", "__E_bf_45", "bf_fex", "__E_bf_46", "bf_ref", "bf_shr", 
	"__E_bf_47", "bf_shl", "__E_bf_48", "bf_nor", "__E_bf_49", "bf_xnor", "__E_bf_50", "bf_nand", "__E_bf_51", "bf_sub", 
	"__E_bf_52", "bf_add", "__E_bf_53", "bf_mod", "__E_bf_54", "bf_div", "__E_bf_55", "bf_mul", "__E_bf_56", "bf_or", 
	"__E_bf_57", "bf_xor", "__E_bf_58", "bf_t", "__E_bf_59", "__E___E_bf_59_60", "bf_f", "__E_bf_61", "__E___E_bf_61_62", "bf_and", 
	"__E_bf_63", "__E___E_bf_63_64", "bf_neg", "__E_bf_65", "bf_neg_oprnd", "__E___E_bf_65_66", "bf_and_nosep", "__E_bf_67", "bf_and_nosep_1st_oprnd", "__E___E_bf_67_68", 
	"bf_and_nosep_2nd_oprnd", "__E___E_bf_67_69", "ctn_neq", "__E_constraint_70", "ctnvar", "num", "ctn_eq", "__E_constraint_71", "ctn_gteq", "__E_constraint_72", 
	"ctn_gt", "__E_constraint_73", "ctn_lteq", "__E_constraint_74", "ctn_lt", "__E_constraint_75", "__E_ba_constant_76", "source", "__E_ba_constant_77", "src_c", 
	"__E_source_78", "__E___E_source_78_79", "__E___E_source_78_80", "__E_src_c_81", "__E_src_c_82", "__E_variable_83", "uconst", "io_var", "var_name", "__E_variable_84", 
	"__E_var_name_85", "__E_var_name_86", "__E_var_name_87", "__E_var_name_88", "__E_var_name_89", "offset", "chars", "uconst_name", "__E_uconst_90", "__E___E_uconst_90_91", 
	"q_var", "__E_q_vars_92", "__E_q_vars_93", "__E_offsets_94", "__E_offsets_95", "integer", "shift", "__E_offset_96", "__E_shift_97", "__E___E_shift_97_98", 
	"__E_chars_99", "__E_chars_100", "digits", "__E_digits_101", "__E_integer_102", "type", "__E_typed_103", "subtype", "comment", "__E_comment_104", 
	"__E_comment_105", "__E_comment_106", "cli", "cli_command", "__E_cli_107", "__E_cli_108", "quit_cmd", "quit_sym", "__E_cli_command_109", "version_cmd", 
	"version_sym", "__E_cli_command_110", "clear_cmd", "clear_sym", "__E_cli_command_111", "help_cmd", "__E_cli_command_112", "help_sym", "__E___E_cli_command_112_113", "__E___E_cli_command_112_114", 
	"help_arg", "file_cmd", "__E_cli_command_115", "file_sym", "q_string", "valid_cmd", "__E_cli_command_116", "valid_sym", "normalize_cmd_arg", "sat_cmd", 
	"__E_cli_command_117", "sat_sym", "unsat_cmd", "__E_cli_command_118", "unsat_sym", "solve_cmd", "__E_cli_command_119", "solve_sym", "__E___E_cli_command_119_120", "solve_options", 
	"wff_cmd_arg", "lgrs_cmd", "__E_cli_command_121", "lgrs_sym", "run_cmd", "__E_cli_command_122", "run_sym", "__E___E_cli_command_122_123", "__E___E_cli_command_122_124", "history", 
	"normalize_cmd", "__E_cli_command_125", "normalize_sym", "__E___E_cli_command_125_126", "subst_cmd", "__E_cli_command_127", "subst_sym", "__E___E_cli_command_127_128", "nf_cmd_arg", "inst_cmd", 
	"__E_cli_command_129", "inst_sym", "__E___E_cli_command_129_130", "inst_args", "dnf_cmd", "__E_cli_command_131", "dnf_sym", "cnf_cmd", "__E_cli_command_132", "cnf_sym", 
	"anf_cmd", "__E_cli_command_133", "anf_sym", "nnf_cmd", "__E_cli_command_134", "nnf_sym", "pnf_cmd", "__E_cli_command_135", "pnf_sym", "mnf_cmd", 
	"__E_cli_command_136", "mnf_sym", "onf_cmd", "__E_cli_command_137", "onf_sym", "onf_cmd_arg", "qelim_cmd", "__E_cli_command_138", "qelim_sym", "get_cmd", 
	"__E_cli_command_139", "get_sym", "__E___E_cli_command_139_140", "option_name", "set_cmd", "__E_cli_command_141", "set_sym", "__E___E_cli_command_141_142", "option_value", "enable_cmd", 
	"__E_cli_command_143", "enable_sym", "disable_cmd", "__E_cli_command_144", "disable_sym", "toggle_cmd", "__E_cli_command_145", "toggle_sym", "def_list_cmd", "__E_cli_command_146", 
	"def_sym", "__E___E_cli_command_146_147", "def_print_cmd", "__E_cli_command_148", "def_rr_cmd", "def_input_cmd", "def_output_cmd", "history_list_cmd", "__E_cli_command_149", "history_sym", 
	"__E___E_cli_command_149_150", "history_print_cmd", "__E_cli_command_151", "history_store_cmd", "__E_cli_command_152", "__E_solve_options_153", "solve_option", "__E_solve_options_154", "__E_solve_option_155", "solver_mode", 
	"solver_mode_minimum", "__E_solver_mode_156", "solver_mode_maximum", "__E_solver_mode_157", "file_name", "__E_file_name_158", "__E_option_name_159", "__E_option_value_160", "bf_cmd_arg", "abs_history_sym", 
	"rel_history_sym", "selection_sym", "execute_sym", "snf_sym", "examples_sym", "__E_help_arg_161", "__E___E_help_arg_161_162", "rel_history", "__E_history_163", "__E___E_history_163_164", 
	"history_id", "abs_history", "__E_history_165", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", 
	"__N_7", "__N_8", 
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
			11, 34
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			29, 31, 40, 164, 177, 188, 197, 205, 212, 215,
			217, 313, 318, 354
		},
		.to_inline = {
			{ 14, 55, 14 },
			{ 30, 110, 30 },
			{ 154, 110, 30 },
			{ 156, 110, 149 },
			{ 158, 110, 30 },
			{ 160, 110, 30 },
			{ 179 },
			{ 186 },
			{ 186, 197 },
			{ 196 },
			{ 200 },
			{ 248 },
			{ 260 },
			{ 278 },
			{ 283 },
			{ 305 },
			{ 358 }
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
//G40:  __E_ref_12(44)       => typed(33).
	p(NT(44), (NT(33)));
//G41:  __E_ref_12(44)       => null.
	p(NT(44), (nul));
//G42:  __E_ref_13(45)       => _(11) 'f' 'a' 'l' 'l' 'b' 'a' 'c' 'k' __(34) fp_fallback(46).
	p(NT(45), (NT(11)+T(9)+T(16)+T(10)+T(10)+T(17)+T(16)+T(14)+T(18)+NT(34)+NT(46)));
//G43:  __E_ref_13(45)       => null.
	p(NT(45), (nul));
//G44:  ref(27)              => sym(40) __E_ref_11(41) ref_args(43) __E_ref_12(44) __E_ref_13(45).
	p(NT(27), (NT(40)+NT(41)+NT(43)+NT(44)+NT(45)));
//G45:  first_sym(47)        => 'f' 'i' 'r' 's' 't'.
	p(NT(47), (T(9)+T(4)+T(19)+T(15)+T(8)));
//G46:  fp_fallback(46)      => first_sym(47).
	p(NT(46), (NT(47)));
//G47:  last_sym(48)         => 'l' 'a' 's' 't'.
	p(NT(48), (T(10)+T(16)+T(15)+T(8)));
//G48:  fp_fallback(46)      => last_sym(48).
	p(NT(46), (NT(48)));
//G49:  fp_fallback(46)      => capture(29).
	p(NT(46), (NT(29)));
//G50:  fp_fallback(46)      => ref(27).
	p(NT(46), (NT(27)));
//G51:  fp_fallback(46)      => wff(14).
	p(NT(46), (NT(14)));
//G52:  fp_fallback(46)      => bf(30).
	p(NT(46), (NT(30)));
//G53:  ref_arg(51)          => bf(30).
	p(NT(51), (NT(30)));
//G54:  __E___E_ref_args_14_15(50) => _(11) ref_arg(51).
	p(NT(50), (NT(11)+NT(51)));
//G55:  __E___E_ref_args_14_16(52) => _(11) ',' _(11) ref_arg(51).
	p(NT(52), (NT(11)+T(20)+NT(11)+NT(51)));
//G56:  __E___E_ref_args_14_17(53) => null.
	p(NT(53), (nul));
//G57:  __E___E_ref_args_14_17(53) => __E___E_ref_args_14_16(52) __E___E_ref_args_14_17(53).
	p(NT(53), (NT(52)+NT(53)));
//G58:  __E_ref_args_14(49)  => __E___E_ref_args_14_15(50) __E___E_ref_args_14_17(53).
	p(NT(49), (NT(50)+NT(53)));
//G59:  __E_ref_args_14(49)  => null.
	p(NT(49), (nul));
//G60:  ref_args(43)         => '(' __E_ref_args_14(49) _(11) ')'.
	p(NT(43), (T(12)+NT(49)+NT(11)+T(13)));
//G61:  __E_tau_constant_source_18(55) => '.' _(11).
	p(NT(55), (T(1)+NT(11)));
//G62:  __E_tau_constant_source_18(55) => null.
	p(NT(55), (nul));
//G63:  tau_constant_source(54) => rec_relations(24) _(11) main(13) _(11) __E_tau_constant_source_18(55).
	p(NT(54), (NT(24)+NT(11)+NT(13)+NT(11)+NT(55)));
//G64:  __E_wff_19(57)       => '(' _(11) wff(14) _(11) ')'.
	p(NT(57), (T(12)+NT(11)+NT(14)+NT(11)+T(13)));
//G65:  wff_parenthesis(56)  => __E_wff_19(57).
	p(NT(56), (NT(57)));
//G66:  wff(14)              => wff_parenthesis(56).
	p(NT(14), (NT(56)));
//G67:  __E___E_wff_20_21(60) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(60), (T(15)+T(6)+T(21)+T(11)+T(8)+T(4)+T(21)+T(11)+T(15)));
//G68:  __E___E_wff_20_21(60) => '<' '>'.
	p(NT(60), (T(22)+T(23)));
//G69:  __E_wff_20(59)       => __E___E_wff_20_21(60) _(11) wff(14).
	p(NT(59), (NT(60)+NT(11)+NT(14)));
//G70:  wff_sometimes(58)    => __E_wff_20(59).
	p(NT(58), (NT(59)));
//G71:  wff(14)              => wff_sometimes(58).
	p(NT(14), (NT(58)));
//G72:  __E___E_wff_22_23(63) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(63), (T(16)+T(10)+T(24)+T(16)+T(25)+T(15)));
//G73:  __E___E_wff_22_23(63) => '[' ']'.
	p(NT(63), (T(26)+T(27)));
//G74:  __E_wff_22(62)       => __E___E_wff_22_23(63) _(11) wff(14).
	p(NT(62), (NT(63)+NT(11)+NT(14)));
//G75:  wff_always(61)       => __E_wff_22(62).
	p(NT(61), (NT(62)));
//G76:  wff(14)              => wff_always(61).
	p(NT(14), (NT(61)));
//G77:  __E_wff_24(65)       => wff(14) _(11) '?' _(11) wff(14) _(11) ':' _(11) wff(14).
	p(NT(65), (NT(14)+NT(11)+T(28)+NT(11)+NT(14)+NT(11)+T(2)+NT(11)+NT(14)));
//G78:  wff_conditional(64)  => __E_wff_24(65).
	p(NT(64), (NT(65)));
//G79:  wff(14)              => wff_conditional(64).
	p(NT(14), (NT(64)));
//G80:  __E_wff_25(67)       => 'a' 'l' 'l' __(34) q_vars(68) __(34) wff(14).
	p(NT(67), (T(16)+T(10)+T(10)+NT(34)+NT(68)+NT(34)+NT(14)));
//G81:  wff_all(66)          => __E_wff_25(67).
	p(NT(66), (NT(67)));
//G82:  wff(14)              => wff_all(66).
	p(NT(14), (NT(66)));
//G83:  __E_wff_26(70)       => 'e' 'x' __(34) q_vars(68) __(34) wff(14).
	p(NT(70), (T(11)+T(29)+NT(34)+NT(68)+NT(34)+NT(14)));
//G84:  wff_ex(69)           => __E_wff_26(70).
	p(NT(69), (NT(70)));
//G85:  wff(14)              => wff_ex(69).
	p(NT(14), (NT(69)));
//G86:  wff_ref(71)          => ref(27).
	p(NT(71), (NT(27)));
//G87:  wff(14)              => wff_ref(71).
	p(NT(14), (NT(71)));
//G88:  wff(14)              => constraint(72).
	p(NT(14), (NT(72)));
//G89:  __E_wff_27(74)       => wff(14) _(11) '-' '>' _(11) wff(14).
	p(NT(74), (NT(14)+NT(11)+T(30)+T(23)+NT(11)+NT(14)));
//G90:  wff_imply(73)        => __E_wff_27(74).
	p(NT(73), (NT(74)));
//G91:  wff(14)              => wff_imply(73).
	p(NT(14), (NT(73)));
//G92:  __E_wff_28(76)       => wff(14) _(11) '<' '-' _(11) wff(14).
	p(NT(76), (NT(14)+NT(11)+T(22)+T(30)+NT(11)+NT(14)));
//G93:  wff_rimply(75)       => __E_wff_28(76).
	p(NT(75), (NT(76)));
//G94:  wff(14)              => wff_rimply(75).
	p(NT(14), (NT(75)));
//G95:  __E_wff_29(78)       => wff(14) _(11) '<' '-' '>' _(11) wff(14).
	p(NT(78), (NT(14)+NT(11)+T(22)+T(30)+T(23)+NT(11)+NT(14)));
//G96:  wff_equiv(77)        => __E_wff_29(78).
	p(NT(77), (NT(78)));
//G97:  wff(14)              => wff_equiv(77).
	p(NT(14), (NT(77)));
//G98:  __E_wff_30(80)       => wff(14) _(11) '|' '|' _(11) wff(14).
	p(NT(80), (NT(14)+NT(11)+T(31)+T(31)+NT(11)+NT(14)));
//G99:  wff_or(79)           => __E_wff_30(80).
	p(NT(79), (NT(80)));
//G100: wff(14)              => wff_or(79).
	p(NT(14), (NT(79)));
//G101: __E_wff_31(82)       => wff(14) _(11) '^' '^' _(11) wff(14).
	p(NT(82), (NT(14)+NT(11)+T(32)+T(32)+NT(11)+NT(14)));
//G102: wff_xor(81)          => __E_wff_31(82).
	p(NT(81), (NT(82)));
//G103: wff(14)              => wff_xor(81).
	p(NT(14), (NT(81)));
//G104: __E_wff_32(84)       => wff(14) _(11) '&' '&' _(11) wff(14).
	p(NT(84), (NT(14)+NT(11)+T(33)+T(33)+NT(11)+NT(14)));
//G105: wff_and(83)          => __E_wff_32(84).
	p(NT(83), (NT(84)));
//G106: wff(14)              => wff_and(83).
	p(NT(14), (NT(83)));
//G107: __E_wff_33(86)       => '!' _(11) wff(14).
	p(NT(86), (T(34)+NT(11)+NT(14)));
//G108: wff_neg(85)          => __E_wff_33(86).
	p(NT(85), (NT(86)));
//G109: wff(14)              => wff_neg(85).
	p(NT(14), (NT(85)));
//G110: wff_t(87)            => 'T'.
	p(NT(87), (T(35)));
//G111: wff(14)              => wff_t(87).
	p(NT(14), (NT(87)));
//G112: wff_f(88)            => 'F'.
	p(NT(88), (T(36)));
//G113: wff(14)              => wff_f(88).
	p(NT(14), (NT(88)));
//G114: __E_wff_34(90)       => bf(30) _(11) '<' '=' _(11) bf(30) _(11) '<' '=' _(11) bf(30).
	p(NT(90), (NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)));
//G115: bf_interval(89)      => __E_wff_34(90).
	p(NT(89), (NT(90)));
//G116: wff(14)              => bf_interval(89).
	p(NT(14), (NT(89)));
//G117: __E_wff_35(92)       => bf(30) _(11) '=' _(11) bf(30).
	p(NT(92), (NT(30)+NT(11)+T(3)+NT(11)+NT(30)));
//G118: bf_eq(91)            => __E_wff_35(92).
	p(NT(91), (NT(92)));
//G119: wff(14)              => bf_eq(91).
	p(NT(14), (NT(91)));
//G120: __E_wff_36(94)       => bf(30) _(11) '!' '=' _(11) bf(30).
	p(NT(94), (NT(30)+NT(11)+T(34)+T(3)+NT(11)+NT(30)));
//G121: bf_neq(93)           => __E_wff_36(94).
	p(NT(93), (NT(94)));
//G122: wff(14)              => bf_neq(93).
	p(NT(14), (NT(93)));
//G123: __E_wff_37(96)       => bf(30) _(11) '<' _(11) bf(30).
	p(NT(96), (NT(30)+NT(11)+T(22)+NT(11)+NT(30)));
//G124: bf_lt(95)            => __E_wff_37(96).
	p(NT(95), (NT(96)));
//G125: wff(14)              => bf_lt(95).
	p(NT(14), (NT(95)));
//G126: __E_wff_38(98)       => bf(30) _(11) '!' '<' _(11) bf(30).
	p(NT(98), (NT(30)+NT(11)+T(34)+T(22)+NT(11)+NT(30)));
//G127: bf_nlt(97)           => __E_wff_38(98).
	p(NT(97), (NT(98)));
//G128: wff(14)              => bf_nlt(97).
	p(NT(14), (NT(97)));
//G129: __E_wff_39(100)      => bf(30) _(11) '<' '=' _(11) bf(30).
	p(NT(100), (NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)));
//G130: bf_lteq(99)          => __E_wff_39(100).
	p(NT(99), (NT(100)));
//G131: wff(14)              => bf_lteq(99).
	p(NT(14), (NT(99)));
//G132: __E_wff_40(102)      => bf(30) _(11) '!' '<' '=' _(11) bf(30).
	p(NT(102), (NT(30)+NT(11)+T(34)+T(22)+T(3)+NT(11)+NT(30)));
//G133: bf_nlteq(101)        => __E_wff_40(102).
	p(NT(101), (NT(102)));
//G134: wff(14)              => bf_nlteq(101).
	p(NT(14), (NT(101)));
//G135: __E_wff_41(104)      => bf(30) _(11) '>' _(11) bf(30).
	p(NT(104), (NT(30)+NT(11)+T(23)+NT(11)+NT(30)));
//G136: bf_gt(103)           => __E_wff_41(104).
	p(NT(103), (NT(104)));
//G137: wff(14)              => bf_gt(103).
	p(NT(14), (NT(103)));
//G138: __E_wff_42(106)      => bf(30) _(11) '!' '>' _(11) bf(30).
	p(NT(106), (NT(30)+NT(11)+T(34)+T(23)+NT(11)+NT(30)));
//G139: bf_ngt(105)          => __E_wff_42(106).
	p(NT(105), (NT(106)));
//G140: wff(14)              => bf_ngt(105).
	p(NT(14), (NT(105)));
//G141: __E_wff_43(108)      => bf(30) _(11) '>' '=' _(11) bf(30).
	p(NT(108), (NT(30)+NT(11)+T(23)+T(3)+NT(11)+NT(30)));
//G142: bf_gteq(107)         => __E_wff_43(108).
	p(NT(107), (NT(108)));
//G143: wff(14)              => bf_gteq(107).
	p(NT(14), (NT(107)));
//G144: __E_wff_44(110)      => bf(30) _(11) '!' '>' '=' _(11) bf(30).
	p(NT(110), (NT(30)+NT(11)+T(34)+T(23)+T(3)+NT(11)+NT(30)));
//G145: bf_ngteq(109)        => __E_wff_44(110).
	p(NT(109), (NT(110)));
//G146: wff(14)              => bf_ngteq(109).
	p(NT(14), (NT(109)));
//G147: wff(14)              => capture(29).
	p(NT(14), (NT(29)));
//G148: __E_bf_45(112)       => '(' _(11) bf(30) _(11) ')'.
	p(NT(112), (T(12)+NT(11)+NT(30)+NT(11)+T(13)));
//G149: bf_parenthesis(111)  => __E_bf_45(112).
	p(NT(111), (NT(112)));
//G150: bf(30)               => bf_parenthesis(111).
	p(NT(30), (NT(111)));
//G151: bf(30)               => ba_constant(113).
	p(NT(30), (NT(113)));
//G152: bf(30)               => variable(114).
	p(NT(30), (NT(114)));
//G154: __E_bf_46(117)       => 'f' 'e' 'x' __(34) q_vars(67) __(34) bf(30).
	p(NT(117), (T(9)+T(11)+T(29)+NT(34)+NT(67)+NT(34)+NT(30)));
//G155: bf_fex(116)          => __E_bf_46(117).
	p(NT(116), (NT(117)));
//G156: bf(30)               => bf_fex(116).
	p(NT(30), (NT(116)));
//G157: bf_ref(118)          => ref(27).
	p(NT(118), (NT(27)));
//G158: bf(30)               => bf_ref(118).
	p(NT(30), (NT(118)));
//G159: __E_bf_47(120)       => bf(30) _(11) '>' '>' _(11) bf(30).
	p(NT(120), (NT(30)+NT(11)+T(23)+T(23)+NT(11)+NT(30)));
//G160: bf_shr(119)          => __E_bf_47(120).
	p(NT(119), (NT(120)));
//G161: bf(30)               => bf_shr(119).
	p(NT(30), (NT(119)));
//G162: __E_bf_48(122)       => bf(30) _(11) '<' '<' _(11) bf(30).
	p(NT(122), (NT(30)+NT(11)+T(22)+T(22)+NT(11)+NT(30)));
//G163: bf_shl(121)          => __E_bf_48(122).
	p(NT(121), (NT(122)));
//G164: bf(30)               => bf_shl(121).
	p(NT(30), (NT(121)));
//G165: __E_bf_49(124)       => bf(30) _(11) '!' '|' _(11) bf(30).
	p(NT(124), (NT(30)+NT(11)+T(34)+T(31)+NT(11)+NT(30)));
//G166: bf_nor(123)          => __E_bf_49(124).
	p(NT(123), (NT(124)));
//G167: bf(30)               => bf_nor(123).
	p(NT(30), (NT(123)));
//G168: __E_bf_50(126)       => bf(30) _(11) '!' '^' _(11) bf(30).
	p(NT(126), (NT(30)+NT(11)+T(34)+T(32)+NT(11)+NT(30)));
//G169: bf_xnor(125)         => __E_bf_50(126).
	p(NT(125), (NT(126)));
//G170: bf(30)               => bf_xnor(125).
	p(NT(30), (NT(125)));
//G171: __E_bf_51(128)       => bf(30) _(11) '!' '&' _(11) bf(30).
	p(NT(128), (NT(30)+NT(11)+T(34)+T(33)+NT(11)+NT(30)));
//G172: bf_nand(127)         => __E_bf_51(128).
	p(NT(127), (NT(128)));
//G173: bf(30)               => bf_nand(127).
	p(NT(30), (NT(127)));
//G174: __E_bf_52(130)       => bf(30) _(11) '-' _(11) bf(30).
	p(NT(130), (NT(30)+NT(11)+T(30)+NT(11)+NT(30)));
//G175: bf_sub(129)          => __E_bf_52(130).
	p(NT(129), (NT(130)));
//G176: bf(30)               => bf_sub(129).
	p(NT(30), (NT(129)));
//G177: __E_bf_53(132)       => bf(30) _(11) '+' _(11) bf(30).
	p(NT(132), (NT(30)+NT(11)+T(37)+NT(11)+NT(30)));
//G178: bf_add(131)          => __E_bf_53(132).
	p(NT(131), (NT(132)));
//G179: bf(30)               => bf_add(131).
	p(NT(30), (NT(131)));
//G180: __E_bf_54(134)       => bf(30) _(11) '%' _(11) bf(30).
	p(NT(134), (NT(30)+NT(11)+T(38)+NT(11)+NT(30)));
//G181: bf_mod(133)          => __E_bf_54(134).
	p(NT(133), (NT(134)));
//G182: bf(30)               => bf_mod(133).
	p(NT(30), (NT(133)));
//G183: __E_bf_55(136)       => bf(30) _(11) '/' _(11) bf(30).
	p(NT(136), (NT(30)+NT(11)+T(39)+NT(11)+NT(30)));
//G184: bf_div(135)          => __E_bf_55(136).
	p(NT(135), (NT(136)));
//G185: bf(30)               => bf_div(135).
	p(NT(30), (NT(135)));
//G186: __E_bf_56(138)       => bf(30) _(11) '*' _(11) bf(30).
	p(NT(138), (NT(30)+NT(11)+T(40)+NT(11)+NT(30)));
//G187: bf_mul(137)          => __E_bf_56(138).
	p(NT(137), (NT(138)));
//G188: bf(30)               => bf_mul(137).
	p(NT(30), (NT(137)));
//G189: __E_bf_57(140)       => bf(30) _(11) '|' _(11) bf(30).
	p(NT(140), (NT(30)+NT(11)+T(31)+NT(11)+NT(30)));
//G190: bf_or(139)           => __E_bf_57(140).
	p(NT(139), (NT(140)));
//G191: bf(30)               => bf_or(139).
	p(NT(30), (NT(139)));
//G192: __E_bf_58(142)       => bf(30) _(11) '^' _(11) bf(30).
	p(NT(142), (NT(30)+NT(11)+T(32)+NT(11)+NT(30)));
//G193: bf_xor(141)          => __E_bf_58(142).
	p(NT(141), (NT(142)));
//G194: bf(30)               => bf_xor(141).
	p(NT(30), (NT(141)));
//G195: __E___E_bf_59_60(145) => typed(33).
	p(NT(145), (NT(33)));
//G196: __E___E_bf_59_60(145) => null.
	p(NT(145), (nul));
//G197: __E_bf_59(144)       => '1' __E___E_bf_59_60(145).
	p(NT(144), (T(41)+NT(145)));
//G198: bf_t(143)            => __E_bf_59(144).
	p(NT(143), (NT(144)));
//G199: bf(30)               => bf_t(143).
	p(NT(30), (NT(143)));
//G200: __E___E_bf_61_62(148) => typed(33).
	p(NT(148), (NT(33)));
//G201: __E___E_bf_61_62(148) => null.
	p(NT(148), (nul));
//G202: __E_bf_61(147)       => '0' __E___E_bf_61_62(148).
	p(NT(147), (T(42)+NT(148)));
//G203: bf_f(146)            => __E_bf_61(147).
	p(NT(146), (NT(147)));
//G204: bf(30)               => bf_f(146).
	p(NT(30), (NT(146)));
//G205: __E___E_bf_63_64(151) => _(11) '&' _(11).
	p(NT(151), (NT(11)+T(33)+NT(11)));
//G206: __E___E_bf_63_64(151) => __(34).
	p(NT(151), (NT(34)));
//G207: __E_bf_63(150)       => bf(30) __E___E_bf_63_64(151) bf(30).
	p(NT(150), (NT(30)+NT(151)+NT(30)));
//G208: bf_and(149)          => __E_bf_63(150).
	p(NT(149), (NT(150)));
//G209: bf(30)               => bf_and(149).
	p(NT(30), (NT(149)));
//G210: __E___E_bf_65_66(155) => bf_parenthesis(110).
	p(NT(155), (NT(110)));
//G211: __E___E_bf_65_66(155) => ba_constant(112).
	p(NT(155), (NT(112)));
//G212: __E___E_bf_65_66(155) => variable(113).
	p(NT(155), (NT(113)));
//G213: __E___E_bf_65_66(155) => bf_ref(118).
	p(NT(155), (NT(118)));
//G214: __E___E_bf_65_66(155) => bf_t(143).
	p(NT(155), (NT(143)));
//G215: __E___E_bf_65_66(155) => bf_f(146).
	p(NT(155), (NT(146)));
//G216: __E___E_bf_65_66(155) => bf_neg(152).
	p(NT(155), (NT(152)));
//G217: __E___E_bf_65_66(155) => capture(29).
	p(NT(155), (NT(29)));
//G218: bf_neg_oprnd(154)    => __E___E_bf_65_66(155).
	p(NT(154), (NT(155)));
//G219: __E_bf_65(153)       => bf_neg_oprnd(154) _(11) '\''.
	p(NT(153), (NT(154)+NT(11)+T(43)));
//G220: bf_neg(152)          => __E_bf_65(153).
	p(NT(152), (NT(153)));
//G221: bf(30)               => bf_neg(152).
	p(NT(30), (NT(152)));
//G222: __E___E_bf_67_68(159) => bf_parenthesis(110).
	p(NT(159), (NT(110)));
//G223: __E___E_bf_67_68(159) => variable(113).
	p(NT(159), (NT(113)));
//G224: __E___E_bf_67_68(159) => bf_ref(118).
	p(NT(159), (NT(118)));
//G225: __E___E_bf_67_68(159) => bf_neg(152).
	p(NT(159), (NT(152)));
//G226: __E___E_bf_67_68(159) => bf_and_nosep(156).
	p(NT(159), (NT(156)));
//G227: __E___E_bf_67_68(159) => capture(29).
	p(NT(159), (NT(29)));
//G228: bf_and_nosep_1st_oprnd(158) => __E___E_bf_67_68(159).
	p(NT(158), (NT(159)));
//G229: __E___E_bf_67_69(161) => bf_parenthesis(110).
	p(NT(161), (NT(110)));
//G230: __E___E_bf_67_69(161) => ba_constant(112).
	p(NT(161), (NT(112)));
//G231: __E___E_bf_67_69(161) => variable(113).
	p(NT(161), (NT(113)));
//G232: __E___E_bf_67_69(161) => bf_ref(118).
	p(NT(161), (NT(118)));
//G233: __E___E_bf_67_69(161) => bf_neg(152).
	p(NT(161), (NT(152)));
//G234: __E___E_bf_67_69(161) => capture(29).
	p(NT(161), (NT(29)));
//G235: bf_and_nosep_2nd_oprnd(160) => __E___E_bf_67_69(161).
	p(NT(160), (NT(161)));
//G236: __E_bf_67(157)       => bf_and_nosep_1st_oprnd(158) bf_and_nosep_2nd_oprnd(160).
	p(NT(157), (NT(158)+NT(160)));
//G237: bf_and_nosep(156)    => __E_bf_67(157).
	p(NT(156), (NT(157)));
//G238: bf(30)               => bf_and_nosep(156).
	p(NT(30), (NT(156)));
//G239: bf(30)               => capture(29).
	p(NT(30), (NT(29)));
//G240: __E_constraint_70(163) => '[' ctnvar(164) _(11) '!' '=' _(11) num(165) ']'.
	p(NT(163), (T(26)+NT(164)+NT(11)+T(34)+T(3)+NT(11)+NT(165)+T(27)));
//G241: __E_constraint_70(163) => '[' num(165) _(11) '!' '=' _(11) ctnvar(164) ']'.
	p(NT(163), (T(26)+NT(165)+NT(11)+T(34)+T(3)+NT(11)+NT(164)+T(27)));
//G242: ctn_neq(162)         => __E_constraint_70(163).
	p(NT(162), (NT(163)));
//G243: constraint(71)       => ctn_neq(162).
	p(NT(71), (NT(162)));
//G244: __E_constraint_71(167) => '[' ctnvar(164) _(11) '=' _(11) num(165) ']'.
	p(NT(167), (T(26)+NT(164)+NT(11)+T(3)+NT(11)+NT(165)+T(27)));
//G245: __E_constraint_71(167) => '[' num(165) _(11) '=' _(11) ctnvar(164) ']'.
	p(NT(167), (T(26)+NT(165)+NT(11)+T(3)+NT(11)+NT(164)+T(27)));
//G246: ctn_eq(166)          => __E_constraint_71(167).
	p(NT(166), (NT(167)));
//G247: constraint(71)       => ctn_eq(166).
	p(NT(71), (NT(166)));
//G248: __E_constraint_72(169) => '[' ctnvar(164) _(11) '>' '=' _(11) num(165) ']'.
	p(NT(169), (T(26)+NT(164)+NT(11)+T(23)+T(3)+NT(11)+NT(165)+T(27)));
//G249: __E_constraint_72(169) => '[' num(165) _(11) '>' '=' _(11) ctnvar(164) ']'.
	p(NT(169), (T(26)+NT(165)+NT(11)+T(23)+T(3)+NT(11)+NT(164)+T(27)));
//G250: ctn_gteq(168)        => __E_constraint_72(169).
	p(NT(168), (NT(169)));
//G251: constraint(71)       => ctn_gteq(168).
	p(NT(71), (NT(168)));
//G252: __E_constraint_73(171) => '[' ctnvar(164) _(11) '>' _(11) num(165) ']'.
	p(NT(171), (T(26)+NT(164)+NT(11)+T(23)+NT(11)+NT(165)+T(27)));
//G253: __E_constraint_73(171) => '[' num(165) _(11) '>' _(11) ctnvar(164) ']'.
	p(NT(171), (T(26)+NT(165)+NT(11)+T(23)+NT(11)+NT(164)+T(27)));
//G254: ctn_gt(170)          => __E_constraint_73(171).
	p(NT(170), (NT(171)));
//G255: constraint(71)       => ctn_gt(170).
	p(NT(71), (NT(170)));
//G256: __E_constraint_74(173) => '[' ctnvar(164) _(11) '<' '=' _(11) num(165) ']'.
	p(NT(173), (T(26)+NT(164)+NT(11)+T(22)+T(3)+NT(11)+NT(165)+T(27)));
//G257: __E_constraint_74(173) => '[' num(165) _(11) '<' '=' _(11) ctnvar(164) ']'.
	p(NT(173), (T(26)+NT(165)+NT(11)+T(22)+T(3)+NT(11)+NT(164)+T(27)));
//G258: ctn_lteq(172)        => __E_constraint_74(173).
	p(NT(172), (NT(173)));
//G259: constraint(71)       => ctn_lteq(172).
	p(NT(71), (NT(172)));
//G260: __E_constraint_75(175) => '[' ctnvar(164) _(11) '<' _(11) num(165) ']'.
	p(NT(175), (T(26)+NT(164)+NT(11)+T(22)+NT(11)+NT(165)+T(27)));
//G261: __E_constraint_75(175) => '[' num(165) _(11) '<' _(11) ctnvar(164) ']'.
	p(NT(175), (T(26)+NT(165)+NT(11)+T(22)+NT(11)+NT(164)+T(27)));
//G262: ctn_lt(174)          => __E_constraint_75(175).
	p(NT(174), (NT(175)));
//G263: constraint(71)       => ctn_lt(174).
	p(NT(71), (NT(174)));
//G264: __E_ba_constant_76(176) => capture(29).
	p(NT(176), (NT(29)));
//G265: __E_ba_constant_76(176) => source(177).
	p(NT(176), (NT(177)));
//G266: __E_ba_constant_77(178) => typed(33).
	p(NT(178), (NT(33)));
//G267: __E_ba_constant_77(178) => null.
	p(NT(178), (nul));
//G268: ba_constant(112)     => '{' _(11) __E_ba_constant_76(176) _(11) '}' __E_ba_constant_77(178).
	p(NT(112), (T(44)+NT(11)+NT(176)+NT(11)+T(45)+NT(178)));
//G269: __E___E_source_78_79(181) => src_c(179).
	p(NT(181), (NT(179)));
//G270: __E___E_source_78_79(181) => space(2).
	p(NT(181), (NT(2)));
//G271: __E___E_source_78_80(182) => null.
	p(NT(182), (nul));
//G272: __E___E_source_78_80(182) => __E___E_source_78_79(181) __E___E_source_78_80(182).
	p(NT(182), (NT(181)+NT(182)));
//G273: __E_source_78(180)   => __E___E_source_78_80(182) src_c(179).
	p(NT(180), (NT(182)+NT(179)));
//G274: __E_source_78(180)   => null.
	p(NT(180), (nul));
//G275: source(177)          => src_c(179) __E_source_78(180).
	p(NT(177), (NT(179)+NT(180)));
//G276: src_c(179)           => alnum(6).
	p(NT(179), (NT(6)));
//G277: __N_0(373)           => '{'.
	p(NT(373), (T(44)));
//G278: __N_1(374)           => '}'.
	p(NT(374), (T(45)));
//G279: src_c(179)           => ~( __N_0(373) ) & ~( __N_1(374) ) & punct(7).	 # conjunctive
	p(NT(179), ~(NT(373)) & ~(NT(374)) & (NT(7)));
//G280: __E_src_c_81(183)    => src_c(179).
	p(NT(183), (NT(179)));
//G281: __E_src_c_81(183)    => space(2).
	p(NT(183), (NT(2)));
//G282: __E_src_c_82(184)    => null.
	p(NT(184), (nul));
//G283: __E_src_c_82(184)    => __E_src_c_81(183) __E_src_c_82(184).
	p(NT(184), (NT(183)+NT(184)));
//G284: src_c(179)           => '{' __E_src_c_82(184) '}'.
	p(NT(179), (T(44)+NT(184)+T(45)));
//G285: __E_variable_83(185) => uconst(186).
	p(NT(185), (NT(186)));
//G286: __E_variable_83(185) => io_var(187).
	p(NT(185), (NT(187)));
//G287: __E_variable_83(185) => var_name(188).
	p(NT(185), (NT(188)));
//G288: __E_variable_84(189) => typed(33).
	p(NT(189), (NT(33)));
//G289: __E_variable_84(189) => null.
	p(NT(189), (nul));
//G290: variable(113)        => __E_variable_83(185) __E_variable_84(189).
	p(NT(113), (NT(185)+NT(189)));
//G291: __N_2(375)           => 'F'.
	p(NT(375), (T(36)));
//G292: __N_3(376)           => 'T'.
	p(NT(376), (T(35)));
//G293: __E_var_name_85(190) => ~( __N_2(375) ) & ~( __N_3(376) ) & alpha(5).	 # conjunctive
	p(NT(190), ~(NT(375)) & ~(NT(376)) & (NT(5)));
//G294: __E_var_name_86(191) => null.
	p(NT(191), (nul));
//G295: __E_var_name_86(191) => digit(3) __E_var_name_86(191).
	p(NT(191), (NT(3)+NT(191)));
//G296: var_name(188)        => __E_var_name_85(190) __E_var_name_86(191).	 # guarded: charvar
	p(NT(188), (NT(190)+NT(191)));
	p.back().guard = "charvar";
//G297: __N_4(377)           => 'F'.
	p(NT(377), (T(36)));
//G298: __N_5(378)           => 'T'.
	p(NT(378), (T(35)));
//G299: __E_var_name_87(192) => ~( __N_4(377) ) & ~( __N_5(378) ) & alpha(5).	 # conjunctive
	p(NT(192), ~(NT(377)) & ~(NT(378)) & (NT(5)));
//G300: __E_var_name_88(193) => alnum(6).
	p(NT(193), (NT(6)));
//G301: __E_var_name_88(193) => '_'.
	p(NT(193), (T(46)));
//G302: __E_var_name_89(194) => null.
	p(NT(194), (nul));
//G303: __E_var_name_89(194) => __E_var_name_88(193) __E_var_name_89(194).
	p(NT(194), (NT(193)+NT(194)));
//G304: var_name(188)        => __E_var_name_87(192) __E_var_name_89(194).	 # guarded: var
	p(NT(188), (NT(192)+NT(194)));
	p.back().guard = "var";
//G305: io_var(187)          => io_var_name(31) '[' offset(195) ']'.
	p(NT(187), (NT(31)+T(26)+NT(195)+T(27)));
//G306: io_var_name(31)      => chars(196).
	p(NT(31), (NT(196)));
//G307: __E___E_uconst_90_91(199) => chars(196) _(11).
	p(NT(199), (NT(196)+NT(11)));
//G308: __E___E_uconst_90_91(199) => null.
	p(NT(199), (nul));
//G309: __E_uconst_90(198)   => __E___E_uconst_90_91(199) ':' _(11) chars(196).
	p(NT(198), (NT(199)+T(2)+NT(11)+NT(196)));
//G310: uconst_name(197)     => __E_uconst_90(198).
	p(NT(197), (NT(198)));
//G311: uconst(186)          => '<' _(11) uconst_name(197) _(11) '>'.
	p(NT(186), (T(22)+NT(11)+NT(197)+NT(11)+T(23)));
//G312: __E_q_vars_92(201)   => _(11) ',' _(11) q_var(200).
	p(NT(201), (NT(11)+T(20)+NT(11)+NT(200)));
//G313: __E_q_vars_93(202)   => null.
	p(NT(202), (nul));
//G314: __E_q_vars_93(202)   => __E_q_vars_92(201) __E_q_vars_93(202).
	p(NT(202), (NT(201)+NT(202)));
//G315: q_vars(67)           => q_var(200) __E_q_vars_93(202).
	p(NT(67), (NT(200)+NT(202)));
//G316: q_var(200)           => capture(29).
	p(NT(200), (NT(29)));
//G317: __N_6(379)           => uconst(186).
	p(NT(379), (NT(186)));
//G318: q_var(200)           => variable(113) & ~( __N_6(379) ).	 # conjunctive
	p(NT(200), (NT(113)) & ~(NT(379)));
//G319: ctnvar(164)          => chars(196).
	p(NT(164), (NT(196)));
//G320: __E_offsets_94(203)  => _(11) ',' _(11) offset(195).
	p(NT(203), (NT(11)+T(20)+NT(11)+NT(195)));
//G321: __E_offsets_95(204)  => null.
	p(NT(204), (nul));
//G322: __E_offsets_95(204)  => __E_offsets_94(203) __E_offsets_95(204).
	p(NT(204), (NT(203)+NT(204)));
//G323: offsets(42)          => '[' _(11) offset(195) __E_offsets_95(204) _(11) ']'.
	p(NT(42), (T(26)+NT(11)+NT(195)+NT(204)+NT(11)+T(27)));
//G324: offset(195)          => integer(205).
	p(NT(195), (NT(205)));
//G325: offset(195)          => capture(29).
	p(NT(195), (NT(29)));
//G326: offset(195)          => shift(206).
	p(NT(195), (NT(206)));
//G327: __N_7(380)           => io_var(187).
	p(NT(380), (NT(187)));
//G328: __E_offset_96(207)   => variable(113) & ~( __N_7(380) ).	 # conjunctive
	p(NT(207), (NT(113)) & ~(NT(380)));
//G329: offset(195)          => __E_offset_96(207).
	p(NT(195), (NT(207)));
//G330: __E_shift_97(208)    => capture(29).
	p(NT(208), (NT(29)));
//G331: __N_8(381)           => io_var(187).
	p(NT(381), (NT(187)));
//G332: __E___E_shift_97_98(209) => variable(113) & ~( __N_8(381) ).	 # conjunctive
	p(NT(209), (NT(113)) & ~(NT(381)));
//G333: __E_shift_97(208)    => __E___E_shift_97_98(209).
	p(NT(208), (NT(209)));
//G334: shift(206)           => __E_shift_97(208) _(11) '-' _(11) num(165).
	p(NT(206), (NT(208)+NT(11)+T(30)+NT(11)+NT(165)));
//G335: __E_chars_99(210)    => alnum(6).
	p(NT(210), (NT(6)));
//G336: __E_chars_99(210)    => '_'.
	p(NT(210), (T(46)));
//G337: __E_chars_100(211)   => null.
	p(NT(211), (nul));
//G338: __E_chars_100(211)   => __E_chars_99(210) __E_chars_100(211).
	p(NT(211), (NT(210)+NT(211)));
//G339: chars(196)           => alpha(5) __E_chars_100(211).
	p(NT(196), (NT(5)+NT(211)));
//G340: __E_digits_101(213)  => digit(3).
	p(NT(213), (NT(3)));
//G341: __E_digits_101(213)  => digit(3) __E_digits_101(213).
	p(NT(213), (NT(3)+NT(213)));
//G342: digits(212)          => __E_digits_101(213).
	p(NT(212), (NT(213)));
//G343: num(165)             => digits(212).
	p(NT(165), (NT(212)));
//G344: __E_integer_102(214) => '-'.
	p(NT(214), (T(30)));
//G345: __E_integer_102(214) => null.
	p(NT(214), (nul));
//G346: integer(205)         => __E_integer_102(214) _(11) digits(212).
	p(NT(205), (NT(214)+NT(11)+NT(212)));
//G347: sym(40)              => chars(196).
	p(NT(40), (NT(196)));
//G348: capture(29)          => '$' chars(196).
	p(NT(29), (T(47)+NT(196)));
//G349: __E_typed_103(216)   => _(11) '[' _(11) subtype(217) _(11) ']' _(11).
	p(NT(216), (NT(11)+T(26)+NT(11)+NT(217)+NT(11)+T(27)+NT(11)));
//G350: __E_typed_103(216)   => null.
	p(NT(216), (nul));
//G351: typed(33)            => _(11) ':' _(11) type(215) __E_typed_103(216).
	p(NT(33), (NT(11)+T(2)+NT(11)+NT(215)+NT(216)));
//G352: type(215)            => chars(196).
	p(NT(215), (NT(196)));
//G353: subtype(217)         => num(165).
	p(NT(217), (NT(165)));
//G354: __E_comment_104(219) => printable(8).
	p(NT(219), (NT(8)));
//G355: __E_comment_104(219) => '\t'.
	p(NT(219), (T(48)));
//G356: __E_comment_105(220) => null.
	p(NT(220), (nul));
//G357: __E_comment_105(220) => __E_comment_104(219) __E_comment_105(220).
	p(NT(220), (NT(219)+NT(220)));
//G358: __E_comment_106(221) => '\n'.
	p(NT(221), (T(49)));
//G359: __E_comment_106(221) => '\r'.
	p(NT(221), (T(50)));
//G360: __E_comment_106(221) => eof(1).
	p(NT(221), (NT(1)));
//G361: comment(218)         => '#' __E_comment_105(220) __E_comment_106(221).
	p(NT(218), (T(51)+NT(220)+NT(221)));
//G362: __(34)               => space(2).
	p(NT(34), (NT(2)));
//G363: __(34)               => comment(218).
	p(NT(34), (NT(218)));
//G364: __(34)               => __(34) space(2).
	p(NT(34), (NT(34)+NT(2)));
//G365: __(34)               => __(34) comment(218).
	p(NT(34), (NT(34)+NT(218)));
//G366: _(11)                => __(34).
	p(NT(11), (NT(34)));
//G367: _(11)                => null.
	p(NT(11), (nul));
//G368: cli(222)             => _(11).
	p(NT(222), (NT(11)));
//G369: __E_cli_107(224)     => '.' _(11) cli_command(223) _(11).
	p(NT(224), (T(1)+NT(11)+NT(223)+NT(11)));
//G370: __E_cli_108(225)     => null.
	p(NT(225), (nul));
//G371: __E_cli_108(225)     => __E_cli_107(224) __E_cli_108(225).
	p(NT(225), (NT(224)+NT(225)));
//G372: cli(222)             => _(11) cli_command(223) _(11) __E_cli_108(225).
	p(NT(222), (NT(11)+NT(223)+NT(11)+NT(225)));
//G373: __E_cli_command_109(228) => 'q'.
	p(NT(228), (T(52)));
//G374: __E_cli_command_109(228) => 'q' 'u' 'i' 't'.
	p(NT(228), (T(52)+T(7)+T(4)+T(8)));
//G375: quit_sym(227)        => __E_cli_command_109(228).
	p(NT(227), (NT(228)));
//G376: quit_cmd(226)        => quit_sym(227).
	p(NT(226), (NT(227)));
//G377: cli_command(223)     => quit_cmd(226).
	p(NT(223), (NT(226)));
//G378: __E_cli_command_110(231) => 'v'.
	p(NT(231), (T(53)));
//G379: __E_cli_command_110(231) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(231), (T(53)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G380: version_sym(230)     => __E_cli_command_110(231).
	p(NT(230), (NT(231)));
//G381: version_cmd(229)     => version_sym(230).
	p(NT(229), (NT(230)));
//G382: cli_command(223)     => version_cmd(229).
	p(NT(223), (NT(229)));
//G383: __E_cli_command_111(234) => 'c'.
	p(NT(234), (T(14)));
//G384: __E_cli_command_111(234) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(234), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G385: clear_sym(233)       => __E_cli_command_111(234).
	p(NT(233), (NT(234)));
//G386: clear_cmd(232)       => clear_sym(233).
	p(NT(232), (NT(233)));
//G387: cli_command(223)     => clear_cmd(232).
	p(NT(223), (NT(232)));
//G388: __E___E_cli_command_112_113(238) => 'h'.
	p(NT(238), (T(54)));
//G389: __E___E_cli_command_112_113(238) => 'h' 'e' 'l' 'p'.
	p(NT(238), (T(54)+T(11)+T(10)+T(55)));
//G390: help_sym(237)        => __E___E_cli_command_112_113(238).
	p(NT(237), (NT(238)));
//G391: __E___E_cli_command_112_114(239) => __(34) help_arg(240).
	p(NT(239), (NT(34)+NT(240)));
//G392: __E___E_cli_command_112_114(239) => null.
	p(NT(239), (nul));
//G393: __E_cli_command_112(236) => help_sym(237) __E___E_cli_command_112_114(239).
	p(NT(236), (NT(237)+NT(239)));
//G394: help_cmd(235)        => __E_cli_command_112(236).
	p(NT(235), (NT(236)));
//G395: cli_command(223)     => help_cmd(235).
	p(NT(223), (NT(235)));
//G396: file_sym(243)        => 'f' 'i' 'l' 'e'.
	p(NT(243), (T(9)+T(4)+T(10)+T(11)));
//G397: __E_cli_command_115(242) => file_sym(243) __(34) q_string(244).
	p(NT(242), (NT(243)+NT(34)+NT(244)));
//G398: file_cmd(241)        => __E_cli_command_115(242).
	p(NT(241), (NT(242)));
//G399: cli_command(223)     => file_cmd(241).
	p(NT(223), (NT(241)));
//G400: valid_sym(247)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(247), (T(53)+T(16)+T(10)+T(4)+T(56)));
//G401: __E_cli_command_116(246) => valid_sym(247) __(34) normalize_cmd_arg(248).
	p(NT(246), (NT(247)+NT(34)+NT(248)));
//G402: valid_cmd(245)       => __E_cli_command_116(246).
	p(NT(245), (NT(246)));
//G403: cli_command(223)     => valid_cmd(245).
	p(NT(223), (NT(245)));
//G404: sat_sym(251)         => 's' 'a' 't'.
	p(NT(251), (T(15)+T(16)+T(8)));
//G405: __E_cli_command_117(250) => sat_sym(251) __(34) normalize_cmd_arg(248).
	p(NT(250), (NT(251)+NT(34)+NT(248)));
//G406: sat_cmd(249)         => __E_cli_command_117(250).
	p(NT(249), (NT(250)));
//G407: cli_command(223)     => sat_cmd(249).
	p(NT(223), (NT(249)));
//G408: unsat_sym(254)       => 'u' 'n' 's' 'a' 't'.
	p(NT(254), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G409: __E_cli_command_118(253) => unsat_sym(254) __(34) normalize_cmd_arg(248).
	p(NT(253), (NT(254)+NT(34)+NT(248)));
//G410: unsat_cmd(252)       => __E_cli_command_118(253).
	p(NT(252), (NT(253)));
//G411: cli_command(223)     => unsat_cmd(252).
	p(NT(223), (NT(252)));
//G412: solve_sym(257)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(257), (T(15)+T(6)+T(10)+T(53)+T(11)));
//G413: __E___E_cli_command_119_120(258) => solve_options(259).
	p(NT(258), (NT(259)));
//G414: __E___E_cli_command_119_120(258) => null.
	p(NT(258), (nul));
//G415: __E_cli_command_119(256) => solve_sym(257) __E___E_cli_command_119_120(258) __(34) wff_cmd_arg(260).
	p(NT(256), (NT(257)+NT(258)+NT(34)+NT(260)));
//G416: solve_cmd(255)       => __E_cli_command_119(256).
	p(NT(255), (NT(256)));
//G417: cli_command(223)     => solve_cmd(255).
	p(NT(223), (NT(255)));
//G418: lgrs_sym(263)        => 'l' 'g' 'r' 's'.
	p(NT(263), (T(10)+T(57)+T(19)+T(15)));
//G419: __E_cli_command_121(262) => lgrs_sym(263) __(34) wff_cmd_arg(260).
	p(NT(262), (NT(263)+NT(34)+NT(260)));
//G420: lgrs_cmd(261)        => __E_cli_command_121(262).
	p(NT(261), (NT(262)));
//G421: cli_command(223)     => lgrs_cmd(261).
	p(NT(223), (NT(261)));
//G422: __E___E_cli_command_122_123(267) => 'r'.
	p(NT(267), (T(19)));
//G423: __E___E_cli_command_122_123(267) => 'r' 'u' 'n'.
	p(NT(267), (T(19)+T(7)+T(5)));
//G424: run_sym(266)         => __E___E_cli_command_122_123(267).
	p(NT(266), (NT(267)));
//G425: __E___E_cli_command_122_124(268) => history(269).
	p(NT(268), (NT(269)));
//G426: __E___E_cli_command_122_124(268) => wff(14).
	p(NT(268), (NT(14)));
//G427: __E_cli_command_122(265) => run_sym(266) __(34) __E___E_cli_command_122_124(268).
	p(NT(265), (NT(266)+NT(34)+NT(268)));
//G428: run_cmd(264)         => __E_cli_command_122(265).
	p(NT(264), (NT(265)));
//G429: cli_command(223)     => run_cmd(264).
	p(NT(223), (NT(264)));
//G430: __E___E_cli_command_125_126(273) => 'n'.
	p(NT(273), (T(5)));
//G431: __E___E_cli_command_125_126(273) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(273), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(58)+T(11)));
//G432: normalize_sym(272)   => __E___E_cli_command_125_126(273).
	p(NT(272), (NT(273)));
//G433: __E_cli_command_125(271) => normalize_sym(272) __(34) normalize_cmd_arg(248).
	p(NT(271), (NT(272)+NT(34)+NT(248)));
//G434: normalize_cmd(270)   => __E_cli_command_125(271).
	p(NT(270), (NT(271)));
//G435: cli_command(223)     => normalize_cmd(270).
	p(NT(223), (NT(270)));
//G436: __E___E_cli_command_127_128(277) => 's'.
	p(NT(277), (T(15)));
//G437: __E___E_cli_command_127_128(277) => 's' 'u' 'b' 's' 't'.
	p(NT(277), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G438: __E___E_cli_command_127_128(277) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(277), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G439: subst_sym(276)       => __E___E_cli_command_127_128(277).
	p(NT(276), (NT(277)));
//G440: __E_cli_command_127(275) => subst_sym(276) __(34) nf_cmd_arg(278) _(11) '[' _(11) nf_cmd_arg(278) _(11) '/' _(11) nf_cmd_arg(278) _(11) ']'.
	p(NT(275), (NT(276)+NT(34)+NT(278)+NT(11)+T(26)+NT(11)+NT(278)+NT(11)+T(39)+NT(11)+NT(278)+NT(11)+T(27)));
//G441: subst_cmd(274)       => __E_cli_command_127(275).
	p(NT(274), (NT(275)));
//G442: cli_command(223)     => subst_cmd(274).
	p(NT(223), (NT(274)));
//G443: __E___E_cli_command_129_130(282) => 'i'.
	p(NT(282), (T(4)));
//G444: __E___E_cli_command_129_130(282) => 'i' 'n' 's' 't'.
	p(NT(282), (T(4)+T(5)+T(15)+T(8)));
//G445: __E___E_cli_command_129_130(282) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(282), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G446: inst_sym(281)        => __E___E_cli_command_129_130(282).
	p(NT(281), (NT(282)));
//G447: __E_cli_command_129(280) => inst_sym(281) __(34) inst_args(283).
	p(NT(280), (NT(281)+NT(34)+NT(283)));
//G448: inst_cmd(279)        => __E_cli_command_129(280).
	p(NT(279), (NT(280)));
//G449: cli_command(223)     => inst_cmd(279).
	p(NT(223), (NT(279)));
//G450: dnf_sym(286)         => 'd' 'n' 'f'.
	p(NT(286), (T(56)+T(5)+T(9)));
//G451: __E_cli_command_131(285) => dnf_sym(286) __(34) nf_cmd_arg(278).
	p(NT(285), (NT(286)+NT(34)+NT(278)));
//G452: dnf_cmd(284)         => __E_cli_command_131(285).
	p(NT(284), (NT(285)));
//G453: cli_command(223)     => dnf_cmd(284).
	p(NT(223), (NT(284)));
//G454: cnf_sym(289)         => 'c' 'n' 'f'.
	p(NT(289), (T(14)+T(5)+T(9)));
//G455: __E_cli_command_132(288) => cnf_sym(289) __(34) nf_cmd_arg(278).
	p(NT(288), (NT(289)+NT(34)+NT(278)));
//G456: cnf_cmd(287)         => __E_cli_command_132(288).
	p(NT(287), (NT(288)));
//G457: cli_command(223)     => cnf_cmd(287).
	p(NT(223), (NT(287)));
//G458: anf_sym(292)         => 'a' 'n' 'f'.
	p(NT(292), (T(16)+T(5)+T(9)));
//G459: __E_cli_command_133(291) => anf_sym(292) __(34) nf_cmd_arg(278).
	p(NT(291), (NT(292)+NT(34)+NT(278)));
//G460: anf_cmd(290)         => __E_cli_command_133(291).
	p(NT(290), (NT(291)));
//G461: cli_command(223)     => anf_cmd(290).
	p(NT(223), (NT(290)));
//G462: nnf_sym(295)         => 'n' 'n' 'f'.
	p(NT(295), (T(5)+T(5)+T(9)));
//G463: __E_cli_command_134(294) => nnf_sym(295) __(34) nf_cmd_arg(278).
	p(NT(294), (NT(295)+NT(34)+NT(278)));
//G464: nnf_cmd(293)         => __E_cli_command_134(294).
	p(NT(293), (NT(294)));
//G465: cli_command(223)     => nnf_cmd(293).
	p(NT(223), (NT(293)));
//G466: pnf_sym(298)         => 'p' 'n' 'f'.
	p(NT(298), (T(55)+T(5)+T(9)));
//G467: __E_cli_command_135(297) => pnf_sym(298) __(34) nf_cmd_arg(278).
	p(NT(297), (NT(298)+NT(34)+NT(278)));
//G468: pnf_cmd(296)         => __E_cli_command_135(297).
	p(NT(296), (NT(297)));
//G469: cli_command(223)     => pnf_cmd(296).
	p(NT(223), (NT(296)));
//G470: mnf_sym(301)         => 'm' 'n' 'f'.
	p(NT(301), (T(21)+T(5)+T(9)));
//G471: __E_cli_command_136(300) => mnf_sym(301) __(34) nf_cmd_arg(278).
	p(NT(300), (NT(301)+NT(34)+NT(278)));
//G472: mnf_cmd(299)         => __E_cli_command_136(300).
	p(NT(299), (NT(300)));
//G473: cli_command(223)     => mnf_cmd(299).
	p(NT(223), (NT(299)));
//G474: onf_sym(304)         => 'o' 'n' 'f'.
	p(NT(304), (T(6)+T(5)+T(9)));
//G475: __E_cli_command_137(303) => onf_sym(304) __(34) variable(113) __(34) onf_cmd_arg(305).
	p(NT(303), (NT(304)+NT(34)+NT(113)+NT(34)+NT(305)));
//G476: onf_cmd(302)         => __E_cli_command_137(303).
	p(NT(302), (NT(303)));
//G477: cli_command(223)     => onf_cmd(302).
	p(NT(223), (NT(302)));
//G478: qelim_sym(308)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(308), (T(52)+T(11)+T(10)+T(4)+T(21)));
//G479: __E_cli_command_138(307) => qelim_sym(308) __(34) wff_cmd_arg(260).
	p(NT(307), (NT(308)+NT(34)+NT(260)));
//G480: qelim_cmd(306)       => __E_cli_command_138(307).
	p(NT(306), (NT(307)));
//G481: cli_command(223)     => qelim_cmd(306).
	p(NT(223), (NT(306)));
//G482: get_sym(311)         => 'g' 'e' 't'.
	p(NT(311), (T(57)+T(11)+T(8)));
//G483: __E___E_cli_command_139_140(312) => __(34) option_name(313).
	p(NT(312), (NT(34)+NT(313)));
//G484: __E___E_cli_command_139_140(312) => null.
	p(NT(312), (nul));
//G485: __E_cli_command_139(310) => get_sym(311) __E___E_cli_command_139_140(312).
	p(NT(310), (NT(311)+NT(312)));
//G486: get_cmd(309)         => __E_cli_command_139(310).
	p(NT(309), (NT(310)));
//G487: cli_command(223)     => get_cmd(309).
	p(NT(223), (NT(309)));
//G488: set_sym(316)         => 's' 'e' 't'.
	p(NT(316), (T(15)+T(11)+T(8)));
//G489: __E___E_cli_command_141_142(317) => __(34).
	p(NT(317), (NT(34)));
//G490: __E___E_cli_command_141_142(317) => _(11) '=' _(11).
	p(NT(317), (NT(11)+T(3)+NT(11)));
//G491: __E_cli_command_141(315) => set_sym(316) __(34) option_name(313) __E___E_cli_command_141_142(317) option_value(318).
	p(NT(315), (NT(316)+NT(34)+NT(313)+NT(317)+NT(318)));
//G492: set_cmd(314)         => __E_cli_command_141(315).
	p(NT(314), (NT(315)));
//G493: cli_command(223)     => set_cmd(314).
	p(NT(223), (NT(314)));
//G494: enable_sym(321)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(321), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G495: __E_cli_command_143(320) => enable_sym(321) __(34) option_name(313).
	p(NT(320), (NT(321)+NT(34)+NT(313)));
//G496: enable_cmd(319)      => __E_cli_command_143(320).
	p(NT(319), (NT(320)));
//G497: cli_command(223)     => enable_cmd(319).
	p(NT(223), (NT(319)));
//G498: disable_sym(324)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(324), (T(56)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G499: __E_cli_command_144(323) => disable_sym(324) __(34) option_name(313).
	p(NT(323), (NT(324)+NT(34)+NT(313)));
//G500: disable_cmd(322)     => __E_cli_command_144(323).
	p(NT(322), (NT(323)));
//G501: cli_command(223)     => disable_cmd(322).
	p(NT(223), (NT(322)));
//G502: toggle_sym(327)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(327), (T(8)+T(6)+T(57)+T(57)+T(10)+T(11)));
//G503: __E_cli_command_145(326) => toggle_sym(327) __(34) option_name(313).
	p(NT(326), (NT(327)+NT(34)+NT(313)));
//G504: toggle_cmd(325)      => __E_cli_command_145(326).
	p(NT(325), (NT(326)));
//G505: cli_command(223)     => toggle_cmd(325).
	p(NT(223), (NT(325)));
//G506: __E___E_cli_command_146_147(331) => 'd' 'e' 'f' 's'.
	p(NT(331), (T(56)+T(11)+T(9)+T(15)));
//G507: __E___E_cli_command_146_147(331) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(331), (T(56)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G508: def_sym(330)         => __E___E_cli_command_146_147(331).
	p(NT(330), (NT(331)));
//G509: __E_cli_command_146(329) => def_sym(330).
	p(NT(329), (NT(330)));
//G510: def_list_cmd(328)    => __E_cli_command_146(329).
	p(NT(328), (NT(329)));
//G511: cli_command(223)     => def_list_cmd(328).
	p(NT(223), (NT(328)));
//G512: __E_cli_command_148(333) => def_sym(330) __(34) num(165).
	p(NT(333), (NT(330)+NT(34)+NT(165)));
//G513: def_print_cmd(332)   => __E_cli_command_148(333).
	p(NT(332), (NT(333)));
//G514: cli_command(223)     => def_print_cmd(332).
	p(NT(223), (NT(332)));
//G515: def_rr_cmd(334)      => rec_relation(17).
	p(NT(334), (NT(17)));
//G516: cli_command(223)     => def_rr_cmd(334).
	p(NT(223), (NT(334)));
//G517: def_input_cmd(335)   => input_def(18).
	p(NT(335), (NT(18)));
//G518: cli_command(223)     => def_input_cmd(335).
	p(NT(223), (NT(335)));
//G519: def_output_cmd(336)  => output_def(19).
	p(NT(336), (NT(19)));
//G520: cli_command(223)     => def_output_cmd(336).
	p(NT(223), (NT(336)));
//G521: __E___E_cli_command_149_150(340) => 'h' 'i' 's' 't'.
	p(NT(340), (T(54)+T(4)+T(15)+T(8)));
//G522: __E___E_cli_command_149_150(340) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(340), (T(54)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G523: history_sym(339)     => __E___E_cli_command_149_150(340).
	p(NT(339), (NT(340)));
//G524: __E_cli_command_149(338) => history_sym(339).
	p(NT(338), (NT(339)));
//G525: history_list_cmd(337) => __E_cli_command_149(338).
	p(NT(337), (NT(338)));
//G526: cli_command(223)     => history_list_cmd(337).
	p(NT(223), (NT(337)));
//G527: __E_cli_command_151(342) => history_sym(339) __(34) history(269).
	p(NT(342), (NT(339)+NT(34)+NT(269)));
//G528: history_print_cmd(341) => __E_cli_command_151(342).
	p(NT(341), (NT(342)));
//G529: cli_command(223)     => history_print_cmd(341).
	p(NT(223), (NT(341)));
//G530: __E_cli_command_152(344) => wff(14).
	p(NT(344), (NT(14)));
//G531: __E_cli_command_152(344) => bf(30).
	p(NT(344), (NT(30)));
//G532: history_store_cmd(343) => __E_cli_command_152(344).
	p(NT(343), (NT(344)));
//G533: cli_command(223)     => history_store_cmd(343).
	p(NT(223), (NT(343)));
//G534: __E_solve_options_153(345) => __(34) solve_option(346).
	p(NT(345), (NT(34)+NT(346)));
//G535: __E_solve_options_154(347) => null.
	p(NT(347), (nul));
//G536: __E_solve_options_154(347) => __E_solve_options_153(345) __E_solve_options_154(347).
	p(NT(347), (NT(345)+NT(347)));
//G537: solve_options(259)   => __E_solve_options_154(347).
	p(NT(259), (NT(347)));
//G538: __E_solve_option_155(348) => solver_mode(349).
	p(NT(348), (NT(349)));
//G539: __E_solve_option_155(348) => type(215).
	p(NT(348), (NT(215)));
//G540: solve_option(346)    => '-' '-' __E_solve_option_155(348).
	p(NT(346), (T(30)+T(30)+NT(348)));
//G541: __E_solver_mode_156(351) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(351), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G542: __E_solver_mode_156(351) => 'm' 'i' 'n'.
	p(NT(351), (T(21)+T(4)+T(5)));
//G543: solver_mode_minimum(350) => __E_solver_mode_156(351).
	p(NT(350), (NT(351)));
//G544: solver_mode(349)     => solver_mode_minimum(350).
	p(NT(349), (NT(350)));
//G545: __E_solver_mode_157(353) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(353), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G546: __E_solver_mode_157(353) => 'm' 'a' 'x'.
	p(NT(353), (T(21)+T(16)+T(29)));
//G547: solver_mode_maximum(352) => __E_solver_mode_157(353).
	p(NT(352), (NT(353)));
//G548: solver_mode(349)     => solver_mode_maximum(352).
	p(NT(349), (NT(352)));
//G549: q_file_name(38)      => '"' file_name(354) '"'.
	p(NT(38), (T(59)+NT(354)+T(59)));
//G550: __E_file_name_158(355) => printable(8).
	p(NT(355), (NT(8)));
//G551: __E_file_name_158(355) => printable(8) __E_file_name_158(355).
	p(NT(355), (NT(8)+NT(355)));
//G552: file_name(354)       => __E_file_name_158(355).
	p(NT(354), (NT(355)));
//G553: __E_option_name_159(356) => alnum(6).
	p(NT(356), (NT(6)));
//G554: __E_option_name_159(356) => alnum(6) __E_option_name_159(356).
	p(NT(356), (NT(6)+NT(356)));
//G555: option_name(313)     => __E_option_name_159(356).
	p(NT(313), (NT(356)));
//G556: __E_option_value_160(357) => alnum(6).
	p(NT(357), (NT(6)));
//G557: __E_option_value_160(357) => alnum(6) __E_option_value_160(357).
	p(NT(357), (NT(6)+NT(357)));
//G558: option_value(318)    => __E_option_value_160(357).
	p(NT(318), (NT(357)));
//G559: bf_cmd_arg(358)      => history(269).
	p(NT(358), (NT(269)));
//G560: bf_cmd_arg(358)      => bf(30).
	p(NT(358), (NT(30)));
//G561: wff_cmd_arg(260)     => history(269).
	p(NT(260), (NT(269)));
//G562: wff_cmd_arg(260)     => wff(14).
	p(NT(260), (NT(14)));
//G563: nf_cmd_arg(278)      => history(269).
	p(NT(278), (NT(269)));
//G564: nf_cmd_arg(278)      => ref(27).
	p(NT(278), (NT(27)));
//G565: nf_cmd_arg(278)      => wff(14).
	p(NT(278), (NT(14)));
//G566: nf_cmd_arg(278)      => bf(30).
	p(NT(278), (NT(30)));
//G567: onf_cmd_arg(305)     => history(269).
	p(NT(305), (NT(269)));
//G568: onf_cmd_arg(305)     => wff(14).
	p(NT(305), (NT(14)));
//G569: normalize_cmd_arg(248) => history(269).
	p(NT(248), (NT(269)));
//G570: normalize_cmd_arg(248) => spec(10).
	p(NT(248), (NT(10)));
//G571: normalize_cmd_arg(248) => ref(27).
	p(NT(248), (NT(27)));
//G572: normalize_cmd_arg(248) => wff(14).
	p(NT(248), (NT(14)));
//G573: normalize_cmd_arg(248) => bf(30).
	p(NT(248), (NT(30)));
//G574: inst_args(283)       => wff_cmd_arg(260) _(11) '[' _(11) bf(30) _(11) '/' _(11) bf_cmd_arg(358) _(11) ']'.
	p(NT(283), (NT(260)+NT(11)+T(26)+NT(11)+NT(30)+NT(11)+T(39)+NT(11)+NT(358)+NT(11)+T(27)));
//G575: inst_args(283)       => bf_cmd_arg(358) _(11) '[' _(11) bf(30) _(11) '/' _(11) bf_cmd_arg(358) _(11) ']'.
	p(NT(283), (NT(358)+NT(11)+T(26)+NT(11)+NT(30)+NT(11)+T(39)+NT(11)+NT(358)+NT(11)+T(27)));
//G576: help_arg(240)        => help_sym(237).
	p(NT(240), (NT(237)));
//G577: help_arg(240)        => version_sym(230).
	p(NT(240), (NT(230)));
//G578: help_arg(240)        => quit_sym(227).
	p(NT(240), (NT(227)));
//G579: help_arg(240)        => clear_sym(233).
	p(NT(240), (NT(233)));
//G580: help_arg(240)        => get_sym(311).
	p(NT(240), (NT(311)));
//G581: help_arg(240)        => set_sym(316).
	p(NT(240), (NT(316)));
//G582: help_arg(240)        => enable_sym(321).
	p(NT(240), (NT(321)));
//G583: help_arg(240)        => disable_sym(324).
	p(NT(240), (NT(324)));
//G584: help_arg(240)        => toggle_sym(327).
	p(NT(240), (NT(327)));
//G585: help_arg(240)        => file_sym(243).
	p(NT(240), (NT(243)));
//G586: help_arg(240)        => history_sym(339).
	p(NT(240), (NT(339)));
//G587: help_arg(240)        => abs_history_sym(359).
	p(NT(240), (NT(359)));
//G588: help_arg(240)        => rel_history_sym(360).
	p(NT(240), (NT(360)));
//G589: help_arg(240)        => selection_sym(361).
	p(NT(240), (NT(361)));
//G590: help_arg(240)        => def_sym(330).
	p(NT(240), (NT(330)));
//G591: help_arg(240)        => inst_sym(281).
	p(NT(240), (NT(281)));
//G592: help_arg(240)        => subst_sym(276).
	p(NT(240), (NT(276)));
//G593: help_arg(240)        => normalize_sym(272).
	p(NT(240), (NT(272)));
//G594: help_arg(240)        => execute_sym(362).
	p(NT(240), (NT(362)));
//G595: help_arg(240)        => solve_sym(257).
	p(NT(240), (NT(257)));
//G596: help_arg(240)        => valid_sym(247).
	p(NT(240), (NT(247)));
//G597: help_arg(240)        => sat_sym(251).
	p(NT(240), (NT(251)));
//G598: help_arg(240)        => unsat_sym(254).
	p(NT(240), (NT(254)));
//G599: help_arg(240)        => run_sym(266).
	p(NT(240), (NT(266)));
//G600: help_arg(240)        => dnf_sym(286).
	p(NT(240), (NT(286)));
//G601: help_arg(240)        => cnf_sym(289).
	p(NT(240), (NT(289)));
//G602: help_arg(240)        => anf_sym(292).
	p(NT(240), (NT(292)));
//G603: help_arg(240)        => snf_sym(363).
	p(NT(240), (NT(363)));
//G604: help_arg(240)        => nnf_sym(295).
	p(NT(240), (NT(295)));
//G605: help_arg(240)        => pnf_sym(298).
	p(NT(240), (NT(298)));
//G606: help_arg(240)        => mnf_sym(301).
	p(NT(240), (NT(301)));
//G607: help_arg(240)        => onf_sym(304).
	p(NT(240), (NT(304)));
//G608: help_arg(240)        => qelim_sym(308).
	p(NT(240), (NT(308)));
//G609: __E___E_help_arg_161_162(366) => 's'.
	p(NT(366), (T(15)));
//G610: __E___E_help_arg_161_162(366) => null.
	p(NT(366), (nul));
//G611: __E_help_arg_161(365) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_161_162(366).
	p(NT(365), (T(11)+T(29)+T(16)+T(21)+T(55)+T(10)+T(11)+NT(366)));
//G612: examples_sym(364)    => __E_help_arg_161(365).
	p(NT(364), (NT(365)));
//G613: help_arg(240)        => examples_sym(364).
	p(NT(240), (NT(364)));
//G614: __E_history_163(368) => '%'.
	p(NT(368), (T(38)));
//G615: rel_history_sym(360) => '%' '-'.
	p(NT(360), (T(38)+T(30)));
//G616: history_id(370)      => digits(212).
	p(NT(370), (NT(212)));
//G617: __E___E_history_163_164(369) => history_id(370).
	p(NT(369), (NT(370)));
//G618: __E___E_history_163_164(369) => null.
	p(NT(369), (nul));
//G619: __E_history_163(368) => rel_history_sym(360) __E___E_history_163_164(369).
	p(NT(368), (NT(360)+NT(369)));
//G620: rel_history(367)     => __E_history_163(368).
	p(NT(367), (NT(368)));
//G621: history(269)         => rel_history(367).
	p(NT(269), (NT(367)));
//G622: abs_history_sym(359) => '%'.
	p(NT(359), (T(38)));
//G623: __E_history_165(372) => abs_history_sym(359) history_id(370).
	p(NT(372), (NT(359)+NT(370)));
//G624: abs_history(371)     => __E_history_165(372).
	p(NT(371), (NT(372)));
//G625: history(269)         => abs_history(371).
	p(NT(269), (NT(371)));
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
		bf_eq, __E_wff_34, bf_neq, __E_wff_35, bf_lt, __E_wff_36, bf_nlt, __E_wff_37, bf_lteq, __E_wff_38, 
		bf_nlteq, __E_wff_39, bf_gt, __E_wff_40, bf_ngt, __E_wff_41, bf_gteq, __E_wff_42, bf_ngteq, __E_wff_43, 
		bf_parenthesis, __E_bf_44, ba_constant, variable, bf_fall, __E_bf_45, bf_fex, __E_bf_46, bf_ref, bf_shr, 
		__E_bf_47, bf_shl, __E_bf_48, bf_nor, __E_bf_49, bf_xnor, __E_bf_50, bf_nand, __E_bf_51, bf_sub, 
		__E_bf_52, bf_add, __E_bf_53, bf_mod, __E_bf_54, bf_div, __E_bf_55, bf_mul, __E_bf_56, bf_or, 
		__E_bf_57, bf_xor, __E_bf_58, bf_t, __E_bf_59, __E___E_bf_59_60, bf_f, __E_bf_61, __E___E_bf_61_62, bf_and, 
		__E_bf_63, __E___E_bf_63_64, bf_neg, __E_bf_65, bf_neg_oprnd, __E___E_bf_65_66, bf_and_nosep, __E_bf_67, bf_and_nosep_1st_oprnd, __E___E_bf_67_68, 
		bf_and_nosep_2nd_oprnd, __E___E_bf_67_69, ctn_neq, __E_constraint_70, ctnvar, num, ctn_eq, __E_constraint_71, ctn_gteq, __E_constraint_72, 
		ctn_gt, __E_constraint_73, ctn_lteq, __E_constraint_74, ctn_lt, __E_constraint_75, __E_ba_constant_76, source, __E_ba_constant_77, src_c, 
		__E_source_78, __E___E_source_78_79, __E___E_source_78_80, __E_src_c_81, __E_src_c_82, __E_variable_83, uconst, io_var, var_name, __E_variable_84, 
		__E_var_name_85, __E_var_name_86, __E_var_name_87, __E_var_name_88, __E_var_name_89, offset, chars, uconst_name, __E_uconst_90, __E___E_uconst_90_91, 
		q_var, __E_q_vars_92, __E_q_vars_93, __E_offsets_94, __E_offsets_95, integer, shift, __E_offset_96, __E_shift_97, __E___E_shift_97_98, 
		__E_chars_99, __E_chars_100, digits, __E_digits_101, __E_integer_102, type, __E_typed_103, subtype, comment, __E_comment_104, 
		__E_comment_105, __E_comment_106, cli, cli_command, __E_cli_107, __E_cli_108, quit_cmd, quit_sym, __E_cli_command_109, version_cmd, 
		version_sym, __E_cli_command_110, clear_cmd, clear_sym, __E_cli_command_111, help_cmd, __E_cli_command_112, help_sym, __E___E_cli_command_112_113, __E___E_cli_command_112_114, 
		help_arg, file_cmd, __E_cli_command_115, file_sym, q_string, valid_cmd, __E_cli_command_116, valid_sym, normalize_cmd_arg, sat_cmd, 
		__E_cli_command_117, sat_sym, unsat_cmd, __E_cli_command_118, unsat_sym, solve_cmd, __E_cli_command_119, solve_sym, __E___E_cli_command_119_120, solve_options, 
		wff_cmd_arg, lgrs_cmd, __E_cli_command_121, lgrs_sym, run_cmd, __E_cli_command_122, run_sym, __E___E_cli_command_122_123, __E___E_cli_command_122_124, history, 
		normalize_cmd, __E_cli_command_125, normalize_sym, __E___E_cli_command_125_126, subst_cmd, __E_cli_command_127, subst_sym, __E___E_cli_command_127_128, nf_cmd_arg, inst_cmd, 
		__E_cli_command_129, inst_sym, __E___E_cli_command_129_130, inst_args, dnf_cmd, __E_cli_command_131, dnf_sym, cnf_cmd, __E_cli_command_132, cnf_sym, 
		anf_cmd, __E_cli_command_133, anf_sym, nnf_cmd, __E_cli_command_134, nnf_sym, pnf_cmd, __E_cli_command_135, pnf_sym, mnf_cmd, 
		__E_cli_command_136, mnf_sym, onf_cmd, __E_cli_command_137, onf_sym, onf_cmd_arg, qelim_cmd, __E_cli_command_138, qelim_sym, get_cmd, 
		__E_cli_command_139, get_sym, __E___E_cli_command_139_140, option_name, set_cmd, __E_cli_command_141, set_sym, __E___E_cli_command_141_142, option_value, enable_cmd, 
		__E_cli_command_143, enable_sym, disable_cmd, __E_cli_command_144, disable_sym, toggle_cmd, __E_cli_command_145, toggle_sym, def_list_cmd, __E_cli_command_146, 
		def_sym, __E___E_cli_command_146_147, def_print_cmd, __E_cli_command_148, def_rr_cmd, def_input_cmd, def_output_cmd, history_list_cmd, __E_cli_command_149, history_sym, 
		__E___E_cli_command_149_150, history_print_cmd, __E_cli_command_151, history_store_cmd, __E_cli_command_152, __E_solve_options_153, solve_option, __E_solve_options_154, __E_solve_option_155, solver_mode, 
		solver_mode_minimum, __E_solver_mode_156, solver_mode_maximum, __E_solver_mode_157, file_name, __E_file_name_158, __E_option_name_159, __E_option_value_160, bf_cmd_arg, abs_history_sym, 
		rel_history_sym, selection_sym, execute_sym, snf_sym, examples_sym, __E_help_arg_161, __E___E_help_arg_161_162, rel_history, __E_history_163, __E___E_history_163_164, 
		history_id, abs_history, __E_history_165, __N_0, __N_1, __N_2, __N_3, __N_4, __N_5, __N_6, 
		__N_7, __N_8, 
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
