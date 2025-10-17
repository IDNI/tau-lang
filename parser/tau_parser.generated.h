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
	"bf_parenthesis", "__E_bf_44", "ba_constant", "variable", "bf_fall", "__E_bf_45", "bf_fex", "__E_bf_46", "bf_ref", "bf_or", 
	"__E_bf_47", "bf_xor", "__E_bf_48", "bf_add", "__E_bf_49", "bf_sub", "__E_bf_50", "bf_mul", "__E_bf_51", "bf_div", 
	"__E_bf_52", "bf_mod", "__E_bf_53", "bf_shr", "__E_bf_54", "bf_shl", "__E_bf_55", "bf_t", "__E_bf_56", "__E___E_bf_56_57", 
	"bf_f", "__E_bf_58", "__E___E_bf_58_59", "bf_and", "__E_bf_60", "__E___E_bf_60_61", "bf_neg", "__E_bf_62", "bf_neg_oprnd", "__E___E_bf_62_63", 
	"bf_and_nosep", "__E_bf_64", "bf_and_nosep_1st_oprnd", "__E___E_bf_64_65", "bf_and_nosep_2nd_oprnd", "__E___E_bf_64_66", "ctn_neq", "__E_constraint_67", "ctnvar", "num", 
	"ctn_eq", "__E_constraint_68", "ctn_gteq", "__E_constraint_69", "ctn_gt", "__E_constraint_70", "ctn_lteq", "__E_constraint_71", "ctn_lt", "__E_constraint_72", 
	"__E_ba_constant_73", "source", "__E_ba_constant_74", "src_c", "__E_source_75", "__E___E_source_75_76", "__E___E_source_75_77", "__E_src_c_78", "__E_src_c_79", "__E_variable_80", 
	"uconst", "io_var", "var_name", "__E_variable_81", "__E_var_name_82", "__E_var_name_83", "__E_var_name_84", "__E_var_name_85", "__E_var_name_86", "offset", 
	"chars", "uconst_name", "__E_uconst_87", "__E___E_uconst_87_88", "q_var", "__E_q_vars_89", "__E_q_vars_90", "__E_offsets_91", "__E_offsets_92", "integer", 
	"shift", "__E_offset_93", "__E_shift_94", "__E___E_shift_94_95", "__E_chars_96", "__E_chars_97", "digits", "__E_digits_98", "__E_integer_99", "type", 
	"__E_typed_100", "subtype", "comment", "__E_comment_101", "__E_comment_102", "__E_comment_103", "cli", "cli_command", "__E_cli_104", "__E_cli_105", 
	"quit_cmd", "quit_sym", "__E_cli_command_106", "version_cmd", "version_sym", "__E_cli_command_107", "clear_cmd", "clear_sym", "__E_cli_command_108", "help_cmd", 
	"__E_cli_command_109", "help_sym", "__E___E_cli_command_109_110", "__E___E_cli_command_109_111", "help_arg", "file_cmd", "__E_cli_command_112", "file_sym", "q_string", "valid_cmd", 
	"__E_cli_command_113", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_114", "sat_sym", "unsat_cmd", "__E_cli_command_115", "unsat_sym", "solve_cmd", 
	"__E_cli_command_116", "solve_sym", "__E___E_cli_command_116_117", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_118", "lgrs_sym", "run_cmd", "__E_cli_command_119", 
	"run_sym", "__E___E_cli_command_119_120", "__E___E_cli_command_119_121", "history", "normalize_cmd", "__E_cli_command_122", "normalize_sym", "__E___E_cli_command_122_123", "subst_cmd", "__E_cli_command_124", 
	"subst_sym", "__E___E_cli_command_124_125", "nf_cmd_arg", "inst_cmd", "__E_cli_command_126", "inst_sym", "__E___E_cli_command_126_127", "inst_args", "dnf_cmd", "__E_cli_command_128", 
	"dnf_sym", "cnf_cmd", "__E_cli_command_129", "cnf_sym", "anf_cmd", "__E_cli_command_130", "anf_sym", "nnf_cmd", "__E_cli_command_131", "nnf_sym", 
	"pnf_cmd", "__E_cli_command_132", "pnf_sym", "mnf_cmd", "__E_cli_command_133", "mnf_sym", "onf_cmd", "__E_cli_command_134", "onf_sym", "onf_cmd_arg", 
	"qelim_cmd", "__E_cli_command_135", "qelim_sym", "get_cmd", "__E_cli_command_136", "get_sym", "__E___E_cli_command_136_137", "option_name", "set_cmd", "__E_cli_command_138", 
	"set_sym", "__E___E_cli_command_138_139", "option_value", "enable_cmd", "__E_cli_command_140", "enable_sym", "disable_cmd", "__E_cli_command_141", "disable_sym", "toggle_cmd", 
	"__E_cli_command_142", "toggle_sym", "def_list_cmd", "__E_cli_command_143", "def_sym", "__E___E_cli_command_143_144", "def_print_cmd", "__E_cli_command_145", "def_rr_cmd", "def_input_cmd", 
	"def_output_cmd", "history_list_cmd", "__E_cli_command_146", "history_sym", "__E___E_cli_command_146_147", "history_print_cmd", "__E_cli_command_148", "history_store_cmd", "__E_cli_command_149", "__E_solve_options_150", 
	"solve_option", "__E_solve_options_151", "__E_solve_option_152", "solver_mode", "solver_mode_minimum", "__E_solver_mode_153", "solver_mode_maximum", "__E_solver_mode_154", "file_name", "__E_file_name_155", 
	"__E_option_name_156", "__E_option_value_157", "bf_cmd_arg", "abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", "snf_sym", "examples_sym", "__E_help_arg_158", 
	"__E___E_help_arg_158_159", "rel_history", "__E_history_160", "__E___E_history_160_161", "history_id", "abs_history", "__E_history_162", "__N_0", "__N_1", "__N_2", 
	"__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', 'a', 'b', 'k', 
	'r', ',', 'm', '<', '>', 'w', 'y', '[', ']', '?', 
	'x', '-', '|', '^', '&', '!', 'T', 'F', '+', '*', 
	'/', '%', '1', '0', '\'', '{', '}', '_', '$', '\t', 
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
			29, 31, 40, 158, 171, 182, 191, 199, 206, 209,
			211, 307, 312, 348
		},
		.to_inline = {
			{ 14, 55, 14 },
			{ 30, 110, 30 },
			{ 33 },
			{ 148, 110, 30 },
			{ 150, 110, 143 },
			{ 152, 110, 30 },
			{ 154, 110, 30 },
			{ 173 },
			{ 180 },
			{ 180, 191 },
			{ 190 },
			{ 194 },
			{ 242 },
			{ 254 },
			{ 272 },
			{ 277 },
			{ 299 },
			{ 352 }
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
//G145: wff(14)              => capture(29).
	p(NT(14), (NT(29)));
//G146: __E_bf_44(111)       => '(' _(11) bf(30) _(11) ')'.
	p(NT(111), (T(12)+NT(11)+NT(30)+NT(11)+T(13)));
//G147: bf_parenthesis(110)  => __E_bf_44(111).
	p(NT(110), (NT(111)));
//G148: bf(30)               => bf_parenthesis(110).
	p(NT(30), (NT(110)));
//G149: bf(30)               => ba_constant(112).
	p(NT(30), (NT(112)));
//G150: bf(30)               => variable(113).
	p(NT(30), (NT(113)));
//G151: __E_bf_45(115)       => 'f' 'a' 'l' 'l' __(34) q_vars(67) __(34) bf(30).
	p(NT(115), (T(9)+T(16)+T(10)+T(10)+NT(34)+NT(67)+NT(34)+NT(30)));
//G152: bf_fall(114)         => __E_bf_45(115).
	p(NT(114), (NT(115)));
//G153: bf(30)               => bf_fall(114).
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
//G159: __E_bf_47(120)       => bf(30) _(11) '|' _(11) bf(30).
	p(NT(120), (NT(30)+NT(11)+T(31)+NT(11)+NT(30)));
//G160: bf_or(119)           => __E_bf_47(120).
	p(NT(119), (NT(120)));
//G161: bf(30)               => bf_or(119).
	p(NT(30), (NT(119)));
//G162: __E_bf_48(122)       => bf(30) _(11) '^' _(11) bf(30).
	p(NT(122), (NT(30)+NT(11)+T(32)+NT(11)+NT(30)));
//G163: bf_xor(121)          => __E_bf_48(122).
	p(NT(121), (NT(122)));
//G164: bf(30)               => bf_xor(121).
	p(NT(30), (NT(121)));
//G165: __E_bf_49(124)       => bf(30) _(11) '+' _(11) bf(30).
	p(NT(124), (NT(30)+NT(11)+T(37)+NT(11)+NT(30)));
//G166: bf_add(123)          => __E_bf_49(124).
	p(NT(123), (NT(124)));
//G167: bf(30)               => bf_add(123).
	p(NT(30), (NT(123)));
//G168: __E_bf_50(126)       => bf(30) _(11) '-' _(11) bf(30).
	p(NT(126), (NT(30)+NT(11)+T(30)+NT(11)+NT(30)));
//G169: bf_sub(125)          => __E_bf_50(126).
	p(NT(125), (NT(126)));
//G170: bf(30)               => bf_sub(125).
	p(NT(30), (NT(125)));
//G171: __E_bf_51(128)       => bf(30) _(11) '*' _(11) bf(30).
	p(NT(128), (NT(30)+NT(11)+T(38)+NT(11)+NT(30)));
//G172: bf_mul(127)          => __E_bf_51(128).
	p(NT(127), (NT(128)));
//G173: bf(30)               => bf_mul(127).
	p(NT(30), (NT(127)));
//G174: __E_bf_52(130)       => bf(30) _(11) '/' _(11) bf(30).
	p(NT(130), (NT(30)+NT(11)+T(39)+NT(11)+NT(30)));
//G175: bf_div(129)          => __E_bf_52(130).
	p(NT(129), (NT(130)));
//G176: bf(30)               => bf_div(129).
	p(NT(30), (NT(129)));
//G177: __E_bf_53(132)       => bf(30) _(11) '%' _(11) bf(30).
	p(NT(132), (NT(30)+NT(11)+T(40)+NT(11)+NT(30)));
//G178: bf_mod(131)          => __E_bf_53(132).
	p(NT(131), (NT(132)));
//G179: bf(30)               => bf_mod(131).
	p(NT(30), (NT(131)));
//G180: __E_bf_54(134)       => bf(30) _(11) '>' '>' _(11) bf(30).
	p(NT(134), (NT(30)+NT(11)+T(23)+T(23)+NT(11)+NT(30)));
//G181: bf_shr(133)          => __E_bf_54(134).
	p(NT(133), (NT(134)));
//G182: bf(30)               => bf_shr(133).
	p(NT(30), (NT(133)));
//G183: __E_bf_55(136)       => bf(30) _(11) '<' '<' _(11) bf(30).
	p(NT(136), (NT(30)+NT(11)+T(22)+T(22)+NT(11)+NT(30)));
//G184: bf_shl(135)          => __E_bf_55(136).
	p(NT(135), (NT(136)));
//G185: bf(30)               => bf_shl(135).
	p(NT(30), (NT(135)));
//G186: __E___E_bf_56_57(139) => typed(33).
	p(NT(139), (NT(33)));
//G187: __E___E_bf_56_57(139) => null.
	p(NT(139), (nul));
//G188: __E_bf_56(138)       => '1' __E___E_bf_56_57(139).
	p(NT(138), (T(41)+NT(139)));
//G189: bf_t(137)            => __E_bf_56(138).
	p(NT(137), (NT(138)));
//G190: bf(30)               => bf_t(137).
	p(NT(30), (NT(137)));
//G191: __E___E_bf_58_59(142) => typed(33).
	p(NT(142), (NT(33)));
//G192: __E___E_bf_58_59(142) => null.
	p(NT(142), (nul));
//G193: __E_bf_58(141)       => '0' __E___E_bf_58_59(142).
	p(NT(141), (T(42)+NT(142)));
//G194: bf_f(140)            => __E_bf_58(141).
	p(NT(140), (NT(141)));
//G195: bf(30)               => bf_f(140).
	p(NT(30), (NT(140)));
//G196: __E___E_bf_60_61(145) => _(11) '&' _(11).
	p(NT(145), (NT(11)+T(33)+NT(11)));
//G197: __E___E_bf_60_61(145) => __(34).
	p(NT(145), (NT(34)));
//G198: __E_bf_60(144)       => bf(30) __E___E_bf_60_61(145) bf(30).
	p(NT(144), (NT(30)+NT(145)+NT(30)));
//G199: bf_and(143)          => __E_bf_60(144).
	p(NT(143), (NT(144)));
//G200: bf(30)               => bf_and(143).
	p(NT(30), (NT(143)));
//G201: __E___E_bf_62_63(149) => bf_parenthesis(110).
	p(NT(149), (NT(110)));
//G202: __E___E_bf_62_63(149) => ba_constant(112).
	p(NT(149), (NT(112)));
//G203: __E___E_bf_62_63(149) => variable(113).
	p(NT(149), (NT(113)));
//G204: __E___E_bf_62_63(149) => bf_ref(118).
	p(NT(149), (NT(118)));
//G205: __E___E_bf_62_63(149) => bf_t(137).
	p(NT(149), (NT(137)));
//G206: __E___E_bf_62_63(149) => bf_f(140).
	p(NT(149), (NT(140)));
//G207: __E___E_bf_62_63(149) => bf_neg(146).
	p(NT(149), (NT(146)));
//G208: __E___E_bf_62_63(149) => capture(29).
	p(NT(149), (NT(29)));
//G209: bf_neg_oprnd(148)    => __E___E_bf_62_63(149).
	p(NT(148), (NT(149)));
//G210: __E_bf_62(147)       => bf_neg_oprnd(148) _(11) '\''.
	p(NT(147), (NT(148)+NT(11)+T(43)));
//G211: bf_neg(146)          => __E_bf_62(147).
	p(NT(146), (NT(147)));
//G212: bf(30)               => bf_neg(146).
	p(NT(30), (NT(146)));
//G213: __E___E_bf_64_65(153) => bf_parenthesis(110).
	p(NT(153), (NT(110)));
//G214: __E___E_bf_64_65(153) => variable(113).
	p(NT(153), (NT(113)));
//G215: __E___E_bf_64_65(153) => bf_ref(118).
	p(NT(153), (NT(118)));
//G216: __E___E_bf_64_65(153) => bf_neg(146).
	p(NT(153), (NT(146)));
//G217: __E___E_bf_64_65(153) => bf_and_nosep(150).
	p(NT(153), (NT(150)));
//G218: __E___E_bf_64_65(153) => capture(29).
	p(NT(153), (NT(29)));
//G219: bf_and_nosep_1st_oprnd(152) => __E___E_bf_64_65(153).
	p(NT(152), (NT(153)));
//G220: __E___E_bf_64_66(155) => bf_parenthesis(110).
	p(NT(155), (NT(110)));
//G221: __E___E_bf_64_66(155) => ba_constant(112).
	p(NT(155), (NT(112)));
//G222: __E___E_bf_64_66(155) => variable(113).
	p(NT(155), (NT(113)));
//G223: __E___E_bf_64_66(155) => bf_ref(118).
	p(NT(155), (NT(118)));
//G224: __E___E_bf_64_66(155) => bf_neg(146).
	p(NT(155), (NT(146)));
//G225: __E___E_bf_64_66(155) => capture(29).
	p(NT(155), (NT(29)));
//G226: bf_and_nosep_2nd_oprnd(154) => __E___E_bf_64_66(155).
	p(NT(154), (NT(155)));
//G227: __E_bf_64(151)       => bf_and_nosep_1st_oprnd(152) bf_and_nosep_2nd_oprnd(154).
	p(NT(151), (NT(152)+NT(154)));
//G228: bf_and_nosep(150)    => __E_bf_64(151).
	p(NT(150), (NT(151)));
//G229: bf(30)               => bf_and_nosep(150).
	p(NT(30), (NT(150)));
//G230: bf(30)               => capture(29).
	p(NT(30), (NT(29)));
//G231: __E_constraint_67(157) => '[' ctnvar(158) _(11) '!' '=' _(11) num(159) ']'.
	p(NT(157), (T(26)+NT(158)+NT(11)+T(34)+T(3)+NT(11)+NT(159)+T(27)));
//G232: __E_constraint_67(157) => '[' num(159) _(11) '!' '=' _(11) ctnvar(158) ']'.
	p(NT(157), (T(26)+NT(159)+NT(11)+T(34)+T(3)+NT(11)+NT(158)+T(27)));
//G233: ctn_neq(156)         => __E_constraint_67(157).
	p(NT(156), (NT(157)));
//G234: constraint(71)       => ctn_neq(156).
	p(NT(71), (NT(156)));
//G235: __E_constraint_68(161) => '[' ctnvar(158) _(11) '=' _(11) num(159) ']'.
	p(NT(161), (T(26)+NT(158)+NT(11)+T(3)+NT(11)+NT(159)+T(27)));
//G236: __E_constraint_68(161) => '[' num(159) _(11) '=' _(11) ctnvar(158) ']'.
	p(NT(161), (T(26)+NT(159)+NT(11)+T(3)+NT(11)+NT(158)+T(27)));
//G237: ctn_eq(160)          => __E_constraint_68(161).
	p(NT(160), (NT(161)));
//G238: constraint(71)       => ctn_eq(160).
	p(NT(71), (NT(160)));
//G239: __E_constraint_69(163) => '[' ctnvar(158) _(11) '>' '=' _(11) num(159) ']'.
	p(NT(163), (T(26)+NT(158)+NT(11)+T(23)+T(3)+NT(11)+NT(159)+T(27)));
//G240: __E_constraint_69(163) => '[' num(159) _(11) '>' '=' _(11) ctnvar(158) ']'.
	p(NT(163), (T(26)+NT(159)+NT(11)+T(23)+T(3)+NT(11)+NT(158)+T(27)));
//G241: ctn_gteq(162)        => __E_constraint_69(163).
	p(NT(162), (NT(163)));
//G242: constraint(71)       => ctn_gteq(162).
	p(NT(71), (NT(162)));
//G243: __E_constraint_70(165) => '[' ctnvar(158) _(11) '>' _(11) num(159) ']'.
	p(NT(165), (T(26)+NT(158)+NT(11)+T(23)+NT(11)+NT(159)+T(27)));
//G244: __E_constraint_70(165) => '[' num(159) _(11) '>' _(11) ctnvar(158) ']'.
	p(NT(165), (T(26)+NT(159)+NT(11)+T(23)+NT(11)+NT(158)+T(27)));
//G245: ctn_gt(164)          => __E_constraint_70(165).
	p(NT(164), (NT(165)));
//G246: constraint(71)       => ctn_gt(164).
	p(NT(71), (NT(164)));
//G247: __E_constraint_71(167) => '[' ctnvar(158) _(11) '<' '=' _(11) num(159) ']'.
	p(NT(167), (T(26)+NT(158)+NT(11)+T(22)+T(3)+NT(11)+NT(159)+T(27)));
//G248: __E_constraint_71(167) => '[' num(159) _(11) '<' '=' _(11) ctnvar(158) ']'.
	p(NT(167), (T(26)+NT(159)+NT(11)+T(22)+T(3)+NT(11)+NT(158)+T(27)));
//G249: ctn_lteq(166)        => __E_constraint_71(167).
	p(NT(166), (NT(167)));
//G250: constraint(71)       => ctn_lteq(166).
	p(NT(71), (NT(166)));
//G251: __E_constraint_72(169) => '[' ctnvar(158) _(11) '<' _(11) num(159) ']'.
	p(NT(169), (T(26)+NT(158)+NT(11)+T(22)+NT(11)+NT(159)+T(27)));
//G252: __E_constraint_72(169) => '[' num(159) _(11) '<' _(11) ctnvar(158) ']'.
	p(NT(169), (T(26)+NT(159)+NT(11)+T(22)+NT(11)+NT(158)+T(27)));
//G253: ctn_lt(168)          => __E_constraint_72(169).
	p(NT(168), (NT(169)));
//G254: constraint(71)       => ctn_lt(168).
	p(NT(71), (NT(168)));
//G255: __E_ba_constant_73(170) => capture(29).
	p(NT(170), (NT(29)));
//G256: __E_ba_constant_73(170) => source(171).
	p(NT(170), (NT(171)));
//G257: __E_ba_constant_74(172) => typed(33).
	p(NT(172), (NT(33)));
//G258: __E_ba_constant_74(172) => null.
	p(NT(172), (nul));
//G259: ba_constant(112)     => '{' _(11) __E_ba_constant_73(170) _(11) '}' __E_ba_constant_74(172).
	p(NT(112), (T(44)+NT(11)+NT(170)+NT(11)+T(45)+NT(172)));
//G260: __E___E_source_75_76(175) => src_c(173).
	p(NT(175), (NT(173)));
//G261: __E___E_source_75_76(175) => space(2).
	p(NT(175), (NT(2)));
//G262: __E___E_source_75_77(176) => null.
	p(NT(176), (nul));
//G263: __E___E_source_75_77(176) => __E___E_source_75_76(175) __E___E_source_75_77(176).
	p(NT(176), (NT(175)+NT(176)));
//G264: __E_source_75(174)   => __E___E_source_75_77(176) src_c(173).
	p(NT(174), (NT(176)+NT(173)));
//G265: __E_source_75(174)   => null.
	p(NT(174), (nul));
//G266: source(171)          => src_c(173) __E_source_75(174).
	p(NT(171), (NT(173)+NT(174)));
//G267: src_c(173)           => alnum(6).
	p(NT(173), (NT(6)));
//G268: __N_0(367)           => '{'.
	p(NT(367), (T(44)));
//G269: __N_1(368)           => '}'.
	p(NT(368), (T(45)));
//G270: src_c(173)           => ~( __N_0(367) ) & ~( __N_1(368) ) & punct(7).	 # conjunctive
	p(NT(173), ~(NT(367)) & ~(NT(368)) & (NT(7)));
//G271: __E_src_c_78(177)    => src_c(173).
	p(NT(177), (NT(173)));
//G272: __E_src_c_78(177)    => space(2).
	p(NT(177), (NT(2)));
//G273: __E_src_c_79(178)    => null.
	p(NT(178), (nul));
//G274: __E_src_c_79(178)    => __E_src_c_78(177) __E_src_c_79(178).
	p(NT(178), (NT(177)+NT(178)));
//G275: src_c(173)           => '{' __E_src_c_79(178) '}'.
	p(NT(173), (T(44)+NT(178)+T(45)));
//G276: __E_variable_80(179) => uconst(180).
	p(NT(179), (NT(180)));
//G277: __E_variable_80(179) => io_var(181).
	p(NT(179), (NT(181)));
//G278: __E_variable_80(179) => var_name(182).
	p(NT(179), (NT(182)));
//G279: __E_variable_81(183) => typed(33).
	p(NT(183), (NT(33)));
//G280: __E_variable_81(183) => null.
	p(NT(183), (nul));
//G281: variable(113)        => __E_variable_80(179) __E_variable_81(183).
	p(NT(113), (NT(179)+NT(183)));
//G282: __N_2(369)           => 'F'.
	p(NT(369), (T(36)));
//G283: __N_3(370)           => 'T'.
	p(NT(370), (T(35)));
//G284: __E_var_name_82(184) => ~( __N_2(369) ) & ~( __N_3(370) ) & alpha(5).	 # conjunctive
	p(NT(184), ~(NT(369)) & ~(NT(370)) & (NT(5)));
//G285: __E_var_name_83(185) => null.
	p(NT(185), (nul));
//G286: __E_var_name_83(185) => digit(3) __E_var_name_83(185).
	p(NT(185), (NT(3)+NT(185)));
//G287: var_name(182)        => __E_var_name_82(184) __E_var_name_83(185).	 # guarded: charvar
	p(NT(182), (NT(184)+NT(185)));
	p.back().guard = "charvar";
//G288: __N_4(371)           => 'F'.
	p(NT(371), (T(36)));
//G289: __N_5(372)           => 'T'.
	p(NT(372), (T(35)));
//G290: __E_var_name_84(186) => ~( __N_4(371) ) & ~( __N_5(372) ) & alpha(5).	 # conjunctive
	p(NT(186), ~(NT(371)) & ~(NT(372)) & (NT(5)));
//G291: __E_var_name_85(187) => alnum(6).
	p(NT(187), (NT(6)));
//G292: __E_var_name_85(187) => '_'.
	p(NT(187), (T(46)));
//G293: __E_var_name_86(188) => null.
	p(NT(188), (nul));
//G294: __E_var_name_86(188) => __E_var_name_85(187) __E_var_name_86(188).
	p(NT(188), (NT(187)+NT(188)));
//G295: var_name(182)        => __E_var_name_84(186) __E_var_name_86(188).	 # guarded: var
	p(NT(182), (NT(186)+NT(188)));
	p.back().guard = "var";
//G296: io_var(181)          => io_var_name(31) '[' offset(189) ']'.
	p(NT(181), (NT(31)+T(26)+NT(189)+T(27)));
//G297: io_var_name(31)      => chars(190).
	p(NT(31), (NT(190)));
//G298: __E___E_uconst_87_88(193) => chars(190) _(11).
	p(NT(193), (NT(190)+NT(11)));
//G299: __E___E_uconst_87_88(193) => null.
	p(NT(193), (nul));
//G300: __E_uconst_87(192)   => __E___E_uconst_87_88(193) ':' _(11) chars(190).
	p(NT(192), (NT(193)+T(2)+NT(11)+NT(190)));
//G301: uconst_name(191)     => __E_uconst_87(192).
	p(NT(191), (NT(192)));
//G302: uconst(180)          => '<' _(11) uconst_name(191) _(11) '>'.
	p(NT(180), (T(22)+NT(11)+NT(191)+NT(11)+T(23)));
//G303: __E_q_vars_89(195)   => _(11) ',' _(11) q_var(194).
	p(NT(195), (NT(11)+T(20)+NT(11)+NT(194)));
//G304: __E_q_vars_90(196)   => null.
	p(NT(196), (nul));
//G305: __E_q_vars_90(196)   => __E_q_vars_89(195) __E_q_vars_90(196).
	p(NT(196), (NT(195)+NT(196)));
//G306: q_vars(67)           => q_var(194) __E_q_vars_90(196).
	p(NT(67), (NT(194)+NT(196)));
//G307: q_var(194)           => capture(29).
	p(NT(194), (NT(29)));
//G308: __N_6(373)           => uconst(180).
	p(NT(373), (NT(180)));
//G309: q_var(194)           => variable(113) & ~( __N_6(373) ).	 # conjunctive
	p(NT(194), (NT(113)) & ~(NT(373)));
//G310: ctnvar(158)          => chars(190).
	p(NT(158), (NT(190)));
//G311: __E_offsets_91(197)  => _(11) ',' _(11) offset(189).
	p(NT(197), (NT(11)+T(20)+NT(11)+NT(189)));
//G312: __E_offsets_92(198)  => null.
	p(NT(198), (nul));
//G313: __E_offsets_92(198)  => __E_offsets_91(197) __E_offsets_92(198).
	p(NT(198), (NT(197)+NT(198)));
//G314: offsets(42)          => '[' _(11) offset(189) __E_offsets_92(198) _(11) ']'.
	p(NT(42), (T(26)+NT(11)+NT(189)+NT(198)+NT(11)+T(27)));
//G315: offset(189)          => integer(199).
	p(NT(189), (NT(199)));
//G316: offset(189)          => capture(29).
	p(NT(189), (NT(29)));
//G317: offset(189)          => shift(200).
	p(NT(189), (NT(200)));
//G318: __N_7(374)           => io_var(181).
	p(NT(374), (NT(181)));
//G319: __E_offset_93(201)   => variable(113) & ~( __N_7(374) ).	 # conjunctive
	p(NT(201), (NT(113)) & ~(NT(374)));
//G320: offset(189)          => __E_offset_93(201).
	p(NT(189), (NT(201)));
//G321: __E_shift_94(202)    => capture(29).
	p(NT(202), (NT(29)));
//G322: __N_8(375)           => io_var(181).
	p(NT(375), (NT(181)));
//G323: __E___E_shift_94_95(203) => variable(113) & ~( __N_8(375) ).	 # conjunctive
	p(NT(203), (NT(113)) & ~(NT(375)));
//G324: __E_shift_94(202)    => __E___E_shift_94_95(203).
	p(NT(202), (NT(203)));
//G325: shift(200)           => __E_shift_94(202) _(11) '-' _(11) num(159).
	p(NT(200), (NT(202)+NT(11)+T(30)+NT(11)+NT(159)));
//G326: __E_chars_96(204)    => alnum(6).
	p(NT(204), (NT(6)));
//G327: __E_chars_96(204)    => '_'.
	p(NT(204), (T(46)));
//G328: __E_chars_97(205)    => null.
	p(NT(205), (nul));
//G329: __E_chars_97(205)    => __E_chars_96(204) __E_chars_97(205).
	p(NT(205), (NT(204)+NT(205)));
//G330: chars(190)           => alpha(5) __E_chars_97(205).
	p(NT(190), (NT(5)+NT(205)));
//G331: __E_digits_98(207)   => digit(3).
	p(NT(207), (NT(3)));
//G332: __E_digits_98(207)   => digit(3) __E_digits_98(207).
	p(NT(207), (NT(3)+NT(207)));
//G333: digits(206)          => __E_digits_98(207).
	p(NT(206), (NT(207)));
//G334: num(159)             => digits(206).
	p(NT(159), (NT(206)));
//G335: __E_integer_99(208)  => '-'.
	p(NT(208), (T(30)));
//G336: __E_integer_99(208)  => null.
	p(NT(208), (nul));
//G337: integer(199)         => __E_integer_99(208) _(11) digits(206).
	p(NT(199), (NT(208)+NT(11)+NT(206)));
//G338: sym(40)              => chars(190).
	p(NT(40), (NT(190)));
//G339: capture(29)          => '$' chars(190).
	p(NT(29), (T(47)+NT(190)));
//G340: __E_typed_100(210)   => _(11) '[' _(11) subtype(211) _(11) ']' _(11).
	p(NT(210), (NT(11)+T(26)+NT(11)+NT(211)+NT(11)+T(27)+NT(11)));
//G341: __E_typed_100(210)   => null.
	p(NT(210), (nul));
//G342: typed(33)            => _(11) ':' _(11) type(209) __E_typed_100(210).
	p(NT(33), (NT(11)+T(2)+NT(11)+NT(209)+NT(210)));
//G343: type(209)            => chars(190).
	p(NT(209), (NT(190)));
//G344: subtype(211)         => num(159).
	p(NT(211), (NT(159)));
//G345: __E_comment_101(213) => printable(8).
	p(NT(213), (NT(8)));
//G346: __E_comment_101(213) => '\t'.
	p(NT(213), (T(48)));
//G347: __E_comment_102(214) => null.
	p(NT(214), (nul));
//G348: __E_comment_102(214) => __E_comment_101(213) __E_comment_102(214).
	p(NT(214), (NT(213)+NT(214)));
//G349: __E_comment_103(215) => '\n'.
	p(NT(215), (T(49)));
//G350: __E_comment_103(215) => '\r'.
	p(NT(215), (T(50)));
//G351: __E_comment_103(215) => eof(1).
	p(NT(215), (NT(1)));
//G352: comment(212)         => '#' __E_comment_102(214) __E_comment_103(215).
	p(NT(212), (T(51)+NT(214)+NT(215)));
//G353: __(34)               => space(2).
	p(NT(34), (NT(2)));
//G354: __(34)               => comment(212).
	p(NT(34), (NT(212)));
//G355: __(34)               => __(34) space(2).
	p(NT(34), (NT(34)+NT(2)));
//G356: __(34)               => __(34) comment(212).
	p(NT(34), (NT(34)+NT(212)));
//G357: _(11)                => __(34).
	p(NT(11), (NT(34)));
//G358: _(11)                => null.
	p(NT(11), (nul));
//G359: cli(216)             => _(11).
	p(NT(216), (NT(11)));
//G360: __E_cli_104(218)     => '.' _(11) cli_command(217) _(11).
	p(NT(218), (T(1)+NT(11)+NT(217)+NT(11)));
//G361: __E_cli_105(219)     => null.
	p(NT(219), (nul));
//G362: __E_cli_105(219)     => __E_cli_104(218) __E_cli_105(219).
	p(NT(219), (NT(218)+NT(219)));
//G363: cli(216)             => _(11) cli_command(217) _(11) __E_cli_105(219).
	p(NT(216), (NT(11)+NT(217)+NT(11)+NT(219)));
//G364: __E_cli_command_106(222) => 'q'.
	p(NT(222), (T(52)));
//G365: __E_cli_command_106(222) => 'q' 'u' 'i' 't'.
	p(NT(222), (T(52)+T(7)+T(4)+T(8)));
//G366: quit_sym(221)        => __E_cli_command_106(222).
	p(NT(221), (NT(222)));
//G367: quit_cmd(220)        => quit_sym(221).
	p(NT(220), (NT(221)));
//G368: cli_command(217)     => quit_cmd(220).
	p(NT(217), (NT(220)));
//G369: __E_cli_command_107(225) => 'v'.
	p(NT(225), (T(53)));
//G370: __E_cli_command_107(225) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(225), (T(53)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G371: version_sym(224)     => __E_cli_command_107(225).
	p(NT(224), (NT(225)));
//G372: version_cmd(223)     => version_sym(224).
	p(NT(223), (NT(224)));
//G373: cli_command(217)     => version_cmd(223).
	p(NT(217), (NT(223)));
//G374: __E_cli_command_108(228) => 'c'.
	p(NT(228), (T(14)));
//G375: __E_cli_command_108(228) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(228), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G376: clear_sym(227)       => __E_cli_command_108(228).
	p(NT(227), (NT(228)));
//G377: clear_cmd(226)       => clear_sym(227).
	p(NT(226), (NT(227)));
//G378: cli_command(217)     => clear_cmd(226).
	p(NT(217), (NT(226)));
//G379: __E___E_cli_command_109_110(232) => 'h'.
	p(NT(232), (T(54)));
//G380: __E___E_cli_command_109_110(232) => 'h' 'e' 'l' 'p'.
	p(NT(232), (T(54)+T(11)+T(10)+T(55)));
//G381: help_sym(231)        => __E___E_cli_command_109_110(232).
	p(NT(231), (NT(232)));
//G382: __E___E_cli_command_109_111(233) => __(34) help_arg(234).
	p(NT(233), (NT(34)+NT(234)));
//G383: __E___E_cli_command_109_111(233) => null.
	p(NT(233), (nul));
//G384: __E_cli_command_109(230) => help_sym(231) __E___E_cli_command_109_111(233).
	p(NT(230), (NT(231)+NT(233)));
//G385: help_cmd(229)        => __E_cli_command_109(230).
	p(NT(229), (NT(230)));
//G386: cli_command(217)     => help_cmd(229).
	p(NT(217), (NT(229)));
//G387: file_sym(237)        => 'f' 'i' 'l' 'e'.
	p(NT(237), (T(9)+T(4)+T(10)+T(11)));
//G388: __E_cli_command_112(236) => file_sym(237) __(34) q_string(238).
	p(NT(236), (NT(237)+NT(34)+NT(238)));
//G389: file_cmd(235)        => __E_cli_command_112(236).
	p(NT(235), (NT(236)));
//G390: cli_command(217)     => file_cmd(235).
	p(NT(217), (NT(235)));
//G391: valid_sym(241)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(241), (T(53)+T(16)+T(10)+T(4)+T(56)));
//G392: __E_cli_command_113(240) => valid_sym(241) __(34) normalize_cmd_arg(242).
	p(NT(240), (NT(241)+NT(34)+NT(242)));
//G393: valid_cmd(239)       => __E_cli_command_113(240).
	p(NT(239), (NT(240)));
//G394: cli_command(217)     => valid_cmd(239).
	p(NT(217), (NT(239)));
//G395: sat_sym(245)         => 's' 'a' 't'.
	p(NT(245), (T(15)+T(16)+T(8)));
//G396: __E_cli_command_114(244) => sat_sym(245) __(34) normalize_cmd_arg(242).
	p(NT(244), (NT(245)+NT(34)+NT(242)));
//G397: sat_cmd(243)         => __E_cli_command_114(244).
	p(NT(243), (NT(244)));
//G398: cli_command(217)     => sat_cmd(243).
	p(NT(217), (NT(243)));
//G399: unsat_sym(248)       => 'u' 'n' 's' 'a' 't'.
	p(NT(248), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G400: __E_cli_command_115(247) => unsat_sym(248) __(34) normalize_cmd_arg(242).
	p(NT(247), (NT(248)+NT(34)+NT(242)));
//G401: unsat_cmd(246)       => __E_cli_command_115(247).
	p(NT(246), (NT(247)));
//G402: cli_command(217)     => unsat_cmd(246).
	p(NT(217), (NT(246)));
//G403: solve_sym(251)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(251), (T(15)+T(6)+T(10)+T(53)+T(11)));
//G404: __E___E_cli_command_116_117(252) => solve_options(253).
	p(NT(252), (NT(253)));
//G405: __E___E_cli_command_116_117(252) => null.
	p(NT(252), (nul));
//G406: __E_cli_command_116(250) => solve_sym(251) __E___E_cli_command_116_117(252) __(34) wff_cmd_arg(254).
	p(NT(250), (NT(251)+NT(252)+NT(34)+NT(254)));
//G407: solve_cmd(249)       => __E_cli_command_116(250).
	p(NT(249), (NT(250)));
//G408: cli_command(217)     => solve_cmd(249).
	p(NT(217), (NT(249)));
//G409: lgrs_sym(257)        => 'l' 'g' 'r' 's'.
	p(NT(257), (T(10)+T(57)+T(19)+T(15)));
//G410: __E_cli_command_118(256) => lgrs_sym(257) __(34) wff_cmd_arg(254).
	p(NT(256), (NT(257)+NT(34)+NT(254)));
//G411: lgrs_cmd(255)        => __E_cli_command_118(256).
	p(NT(255), (NT(256)));
//G412: cli_command(217)     => lgrs_cmd(255).
	p(NT(217), (NT(255)));
//G413: __E___E_cli_command_119_120(261) => 'r'.
	p(NT(261), (T(19)));
//G414: __E___E_cli_command_119_120(261) => 'r' 'u' 'n'.
	p(NT(261), (T(19)+T(7)+T(5)));
//G415: run_sym(260)         => __E___E_cli_command_119_120(261).
	p(NT(260), (NT(261)));
//G416: __E___E_cli_command_119_121(262) => history(263).
	p(NT(262), (NT(263)));
//G417: __E___E_cli_command_119_121(262) => wff(14).
	p(NT(262), (NT(14)));
//G418: __E_cli_command_119(259) => run_sym(260) __(34) __E___E_cli_command_119_121(262).
	p(NT(259), (NT(260)+NT(34)+NT(262)));
//G419: run_cmd(258)         => __E_cli_command_119(259).
	p(NT(258), (NT(259)));
//G420: cli_command(217)     => run_cmd(258).
	p(NT(217), (NT(258)));
//G421: __E___E_cli_command_122_123(267) => 'n'.
	p(NT(267), (T(5)));
//G422: __E___E_cli_command_122_123(267) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(267), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(58)+T(11)));
//G423: normalize_sym(266)   => __E___E_cli_command_122_123(267).
	p(NT(266), (NT(267)));
//G424: __E_cli_command_122(265) => normalize_sym(266) __(34) normalize_cmd_arg(242).
	p(NT(265), (NT(266)+NT(34)+NT(242)));
//G425: normalize_cmd(264)   => __E_cli_command_122(265).
	p(NT(264), (NT(265)));
//G426: cli_command(217)     => normalize_cmd(264).
	p(NT(217), (NT(264)));
//G427: __E___E_cli_command_124_125(271) => 's'.
	p(NT(271), (T(15)));
//G428: __E___E_cli_command_124_125(271) => 's' 'u' 'b' 's' 't'.
	p(NT(271), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G429: __E___E_cli_command_124_125(271) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(271), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G430: subst_sym(270)       => __E___E_cli_command_124_125(271).
	p(NT(270), (NT(271)));
//G431: __E_cli_command_124(269) => subst_sym(270) __(34) nf_cmd_arg(272) _(11) '[' _(11) nf_cmd_arg(272) _(11) '/' _(11) nf_cmd_arg(272) _(11) ']'.
	p(NT(269), (NT(270)+NT(34)+NT(272)+NT(11)+T(26)+NT(11)+NT(272)+NT(11)+T(39)+NT(11)+NT(272)+NT(11)+T(27)));
//G432: subst_cmd(268)       => __E_cli_command_124(269).
	p(NT(268), (NT(269)));
//G433: cli_command(217)     => subst_cmd(268).
	p(NT(217), (NT(268)));
//G434: __E___E_cli_command_126_127(276) => 'i'.
	p(NT(276), (T(4)));
//G435: __E___E_cli_command_126_127(276) => 'i' 'n' 's' 't'.
	p(NT(276), (T(4)+T(5)+T(15)+T(8)));
//G436: __E___E_cli_command_126_127(276) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(276), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G437: inst_sym(275)        => __E___E_cli_command_126_127(276).
	p(NT(275), (NT(276)));
//G438: __E_cli_command_126(274) => inst_sym(275) __(34) inst_args(277).
	p(NT(274), (NT(275)+NT(34)+NT(277)));
//G439: inst_cmd(273)        => __E_cli_command_126(274).
	p(NT(273), (NT(274)));
//G440: cli_command(217)     => inst_cmd(273).
	p(NT(217), (NT(273)));
//G441: dnf_sym(280)         => 'd' 'n' 'f'.
	p(NT(280), (T(56)+T(5)+T(9)));
//G442: __E_cli_command_128(279) => dnf_sym(280) __(34) nf_cmd_arg(272).
	p(NT(279), (NT(280)+NT(34)+NT(272)));
//G443: dnf_cmd(278)         => __E_cli_command_128(279).
	p(NT(278), (NT(279)));
//G444: cli_command(217)     => dnf_cmd(278).
	p(NT(217), (NT(278)));
//G445: cnf_sym(283)         => 'c' 'n' 'f'.
	p(NT(283), (T(14)+T(5)+T(9)));
//G446: __E_cli_command_129(282) => cnf_sym(283) __(34) nf_cmd_arg(272).
	p(NT(282), (NT(283)+NT(34)+NT(272)));
//G447: cnf_cmd(281)         => __E_cli_command_129(282).
	p(NT(281), (NT(282)));
//G448: cli_command(217)     => cnf_cmd(281).
	p(NT(217), (NT(281)));
//G449: anf_sym(286)         => 'a' 'n' 'f'.
	p(NT(286), (T(16)+T(5)+T(9)));
//G450: __E_cli_command_130(285) => anf_sym(286) __(34) nf_cmd_arg(272).
	p(NT(285), (NT(286)+NT(34)+NT(272)));
//G451: anf_cmd(284)         => __E_cli_command_130(285).
	p(NT(284), (NT(285)));
//G452: cli_command(217)     => anf_cmd(284).
	p(NT(217), (NT(284)));
//G453: nnf_sym(289)         => 'n' 'n' 'f'.
	p(NT(289), (T(5)+T(5)+T(9)));
//G454: __E_cli_command_131(288) => nnf_sym(289) __(34) nf_cmd_arg(272).
	p(NT(288), (NT(289)+NT(34)+NT(272)));
//G455: nnf_cmd(287)         => __E_cli_command_131(288).
	p(NT(287), (NT(288)));
//G456: cli_command(217)     => nnf_cmd(287).
	p(NT(217), (NT(287)));
//G457: pnf_sym(292)         => 'p' 'n' 'f'.
	p(NT(292), (T(55)+T(5)+T(9)));
//G458: __E_cli_command_132(291) => pnf_sym(292) __(34) nf_cmd_arg(272).
	p(NT(291), (NT(292)+NT(34)+NT(272)));
//G459: pnf_cmd(290)         => __E_cli_command_132(291).
	p(NT(290), (NT(291)));
//G460: cli_command(217)     => pnf_cmd(290).
	p(NT(217), (NT(290)));
//G461: mnf_sym(295)         => 'm' 'n' 'f'.
	p(NT(295), (T(21)+T(5)+T(9)));
//G462: __E_cli_command_133(294) => mnf_sym(295) __(34) nf_cmd_arg(272).
	p(NT(294), (NT(295)+NT(34)+NT(272)));
//G463: mnf_cmd(293)         => __E_cli_command_133(294).
	p(NT(293), (NT(294)));
//G464: cli_command(217)     => mnf_cmd(293).
	p(NT(217), (NT(293)));
//G465: onf_sym(298)         => 'o' 'n' 'f'.
	p(NT(298), (T(6)+T(5)+T(9)));
//G466: __E_cli_command_134(297) => onf_sym(298) __(34) variable(113) __(34) onf_cmd_arg(299).
	p(NT(297), (NT(298)+NT(34)+NT(113)+NT(34)+NT(299)));
//G467: onf_cmd(296)         => __E_cli_command_134(297).
	p(NT(296), (NT(297)));
//G468: cli_command(217)     => onf_cmd(296).
	p(NT(217), (NT(296)));
//G469: qelim_sym(302)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(302), (T(52)+T(11)+T(10)+T(4)+T(21)));
//G470: __E_cli_command_135(301) => qelim_sym(302) __(34) wff_cmd_arg(254).
	p(NT(301), (NT(302)+NT(34)+NT(254)));
//G471: qelim_cmd(300)       => __E_cli_command_135(301).
	p(NT(300), (NT(301)));
//G472: cli_command(217)     => qelim_cmd(300).
	p(NT(217), (NT(300)));
//G473: get_sym(305)         => 'g' 'e' 't'.
	p(NT(305), (T(57)+T(11)+T(8)));
//G474: __E___E_cli_command_136_137(306) => __(34) option_name(307).
	p(NT(306), (NT(34)+NT(307)));
//G475: __E___E_cli_command_136_137(306) => null.
	p(NT(306), (nul));
//G476: __E_cli_command_136(304) => get_sym(305) __E___E_cli_command_136_137(306).
	p(NT(304), (NT(305)+NT(306)));
//G477: get_cmd(303)         => __E_cli_command_136(304).
	p(NT(303), (NT(304)));
//G478: cli_command(217)     => get_cmd(303).
	p(NT(217), (NT(303)));
//G479: set_sym(310)         => 's' 'e' 't'.
	p(NT(310), (T(15)+T(11)+T(8)));
//G480: __E___E_cli_command_138_139(311) => __(34).
	p(NT(311), (NT(34)));
//G481: __E___E_cli_command_138_139(311) => _(11) '=' _(11).
	p(NT(311), (NT(11)+T(3)+NT(11)));
//G482: __E_cli_command_138(309) => set_sym(310) __(34) option_name(307) __E___E_cli_command_138_139(311) option_value(312).
	p(NT(309), (NT(310)+NT(34)+NT(307)+NT(311)+NT(312)));
//G483: set_cmd(308)         => __E_cli_command_138(309).
	p(NT(308), (NT(309)));
//G484: cli_command(217)     => set_cmd(308).
	p(NT(217), (NT(308)));
//G485: enable_sym(315)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(315), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G486: __E_cli_command_140(314) => enable_sym(315) __(34) option_name(307).
	p(NT(314), (NT(315)+NT(34)+NT(307)));
//G487: enable_cmd(313)      => __E_cli_command_140(314).
	p(NT(313), (NT(314)));
//G488: cli_command(217)     => enable_cmd(313).
	p(NT(217), (NT(313)));
//G489: disable_sym(318)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(318), (T(56)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G490: __E_cli_command_141(317) => disable_sym(318) __(34) option_name(307).
	p(NT(317), (NT(318)+NT(34)+NT(307)));
//G491: disable_cmd(316)     => __E_cli_command_141(317).
	p(NT(316), (NT(317)));
//G492: cli_command(217)     => disable_cmd(316).
	p(NT(217), (NT(316)));
//G493: toggle_sym(321)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(321), (T(8)+T(6)+T(57)+T(57)+T(10)+T(11)));
//G494: __E_cli_command_142(320) => toggle_sym(321) __(34) option_name(307).
	p(NT(320), (NT(321)+NT(34)+NT(307)));
//G495: toggle_cmd(319)      => __E_cli_command_142(320).
	p(NT(319), (NT(320)));
//G496: cli_command(217)     => toggle_cmd(319).
	p(NT(217), (NT(319)));
//G497: __E___E_cli_command_143_144(325) => 'd' 'e' 'f' 's'.
	p(NT(325), (T(56)+T(11)+T(9)+T(15)));
//G498: __E___E_cli_command_143_144(325) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(325), (T(56)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G499: def_sym(324)         => __E___E_cli_command_143_144(325).
	p(NT(324), (NT(325)));
//G500: __E_cli_command_143(323) => def_sym(324).
	p(NT(323), (NT(324)));
//G501: def_list_cmd(322)    => __E_cli_command_143(323).
	p(NT(322), (NT(323)));
//G502: cli_command(217)     => def_list_cmd(322).
	p(NT(217), (NT(322)));
//G503: __E_cli_command_145(327) => def_sym(324) __(34) num(159).
	p(NT(327), (NT(324)+NT(34)+NT(159)));
//G504: def_print_cmd(326)   => __E_cli_command_145(327).
	p(NT(326), (NT(327)));
//G505: cli_command(217)     => def_print_cmd(326).
	p(NT(217), (NT(326)));
//G506: def_rr_cmd(328)      => rec_relation(17).
	p(NT(328), (NT(17)));
//G507: cli_command(217)     => def_rr_cmd(328).
	p(NT(217), (NT(328)));
//G508: def_input_cmd(329)   => input_def(18).
	p(NT(329), (NT(18)));
//G509: cli_command(217)     => def_input_cmd(329).
	p(NT(217), (NT(329)));
//G510: def_output_cmd(330)  => output_def(19).
	p(NT(330), (NT(19)));
//G511: cli_command(217)     => def_output_cmd(330).
	p(NT(217), (NT(330)));
//G512: __E___E_cli_command_146_147(334) => 'h' 'i' 's' 't'.
	p(NT(334), (T(54)+T(4)+T(15)+T(8)));
//G513: __E___E_cli_command_146_147(334) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(334), (T(54)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G514: history_sym(333)     => __E___E_cli_command_146_147(334).
	p(NT(333), (NT(334)));
//G515: __E_cli_command_146(332) => history_sym(333).
	p(NT(332), (NT(333)));
//G516: history_list_cmd(331) => __E_cli_command_146(332).
	p(NT(331), (NT(332)));
//G517: cli_command(217)     => history_list_cmd(331).
	p(NT(217), (NT(331)));
//G518: __E_cli_command_148(336) => history_sym(333) __(34) history(263).
	p(NT(336), (NT(333)+NT(34)+NT(263)));
//G519: history_print_cmd(335) => __E_cli_command_148(336).
	p(NT(335), (NT(336)));
//G520: cli_command(217)     => history_print_cmd(335).
	p(NT(217), (NT(335)));
//G521: __E_cli_command_149(338) => wff(14).
	p(NT(338), (NT(14)));
//G522: __E_cli_command_149(338) => bf(30).
	p(NT(338), (NT(30)));
//G523: history_store_cmd(337) => __E_cli_command_149(338).
	p(NT(337), (NT(338)));
//G524: cli_command(217)     => history_store_cmd(337).
	p(NT(217), (NT(337)));
//G525: __E_solve_options_150(339) => __(34) solve_option(340).
	p(NT(339), (NT(34)+NT(340)));
//G526: __E_solve_options_151(341) => null.
	p(NT(341), (nul));
//G527: __E_solve_options_151(341) => __E_solve_options_150(339) __E_solve_options_151(341).
	p(NT(341), (NT(339)+NT(341)));
//G528: solve_options(253)   => __E_solve_options_151(341).
	p(NT(253), (NT(341)));
//G529: __E_solve_option_152(342) => solver_mode(343).
	p(NT(342), (NT(343)));
//G530: __E_solve_option_152(342) => type(209).
	p(NT(342), (NT(209)));
//G531: solve_option(340)    => '-' '-' __E_solve_option_152(342).
	p(NT(340), (T(30)+T(30)+NT(342)));
//G532: __E_solver_mode_153(345) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(345), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G533: __E_solver_mode_153(345) => 'm' 'i' 'n'.
	p(NT(345), (T(21)+T(4)+T(5)));
//G534: solver_mode_minimum(344) => __E_solver_mode_153(345).
	p(NT(344), (NT(345)));
//G535: solver_mode(343)     => solver_mode_minimum(344).
	p(NT(343), (NT(344)));
//G536: __E_solver_mode_154(347) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(347), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G537: __E_solver_mode_154(347) => 'm' 'a' 'x'.
	p(NT(347), (T(21)+T(16)+T(29)));
//G538: solver_mode_maximum(346) => __E_solver_mode_154(347).
	p(NT(346), (NT(347)));
//G539: solver_mode(343)     => solver_mode_maximum(346).
	p(NT(343), (NT(346)));
//G540: q_file_name(38)      => '"' file_name(348) '"'.
	p(NT(38), (T(59)+NT(348)+T(59)));
//G541: __E_file_name_155(349) => printable(8).
	p(NT(349), (NT(8)));
//G542: __E_file_name_155(349) => printable(8) __E_file_name_155(349).
	p(NT(349), (NT(8)+NT(349)));
//G543: file_name(348)       => __E_file_name_155(349).
	p(NT(348), (NT(349)));
//G544: __E_option_name_156(350) => alnum(6).
	p(NT(350), (NT(6)));
//G545: __E_option_name_156(350) => alnum(6) __E_option_name_156(350).
	p(NT(350), (NT(6)+NT(350)));
//G546: option_name(307)     => __E_option_name_156(350).
	p(NT(307), (NT(350)));
//G547: __E_option_value_157(351) => alnum(6).
	p(NT(351), (NT(6)));
//G548: __E_option_value_157(351) => alnum(6) __E_option_value_157(351).
	p(NT(351), (NT(6)+NT(351)));
//G549: option_value(312)    => __E_option_value_157(351).
	p(NT(312), (NT(351)));
//G550: bf_cmd_arg(352)      => history(263).
	p(NT(352), (NT(263)));
//G551: bf_cmd_arg(352)      => bf(30).
	p(NT(352), (NT(30)));
//G552: wff_cmd_arg(254)     => history(263).
	p(NT(254), (NT(263)));
//G553: wff_cmd_arg(254)     => wff(14).
	p(NT(254), (NT(14)));
//G554: nf_cmd_arg(272)      => history(263).
	p(NT(272), (NT(263)));
//G555: nf_cmd_arg(272)      => ref(27).
	p(NT(272), (NT(27)));
//G556: nf_cmd_arg(272)      => wff(14).
	p(NT(272), (NT(14)));
//G557: nf_cmd_arg(272)      => bf(30).
	p(NT(272), (NT(30)));
//G558: onf_cmd_arg(299)     => history(263).
	p(NT(299), (NT(263)));
//G559: onf_cmd_arg(299)     => wff(14).
	p(NT(299), (NT(14)));
//G560: normalize_cmd_arg(242) => history(263).
	p(NT(242), (NT(263)));
//G561: normalize_cmd_arg(242) => spec(10).
	p(NT(242), (NT(10)));
//G562: normalize_cmd_arg(242) => ref(27).
	p(NT(242), (NT(27)));
//G563: normalize_cmd_arg(242) => wff(14).
	p(NT(242), (NT(14)));
//G564: normalize_cmd_arg(242) => bf(30).
	p(NT(242), (NT(30)));
//G565: inst_args(277)       => wff_cmd_arg(254) _(11) '[' _(11) variable(113) _(11) '/' _(11) bf_cmd_arg(352) _(11) ']'.
	p(NT(277), (NT(254)+NT(11)+T(26)+NT(11)+NT(113)+NT(11)+T(39)+NT(11)+NT(352)+NT(11)+T(27)));
//G566: inst_args(277)       => bf_cmd_arg(352) _(11) '[' _(11) variable(113) _(11) '/' _(11) bf_cmd_arg(352) _(11) ']'.
	p(NT(277), (NT(352)+NT(11)+T(26)+NT(11)+NT(113)+NT(11)+T(39)+NT(11)+NT(352)+NT(11)+T(27)));
//G567: help_arg(234)        => help_sym(231).
	p(NT(234), (NT(231)));
//G568: help_arg(234)        => version_sym(224).
	p(NT(234), (NT(224)));
//G569: help_arg(234)        => quit_sym(221).
	p(NT(234), (NT(221)));
//G570: help_arg(234)        => clear_sym(227).
	p(NT(234), (NT(227)));
//G571: help_arg(234)        => get_sym(305).
	p(NT(234), (NT(305)));
//G572: help_arg(234)        => set_sym(310).
	p(NT(234), (NT(310)));
//G573: help_arg(234)        => enable_sym(315).
	p(NT(234), (NT(315)));
//G574: help_arg(234)        => disable_sym(318).
	p(NT(234), (NT(318)));
//G575: help_arg(234)        => toggle_sym(321).
	p(NT(234), (NT(321)));
//G576: help_arg(234)        => file_sym(237).
	p(NT(234), (NT(237)));
//G577: help_arg(234)        => history_sym(333).
	p(NT(234), (NT(333)));
//G578: help_arg(234)        => abs_history_sym(353).
	p(NT(234), (NT(353)));
//G579: help_arg(234)        => rel_history_sym(354).
	p(NT(234), (NT(354)));
//G580: help_arg(234)        => selection_sym(355).
	p(NT(234), (NT(355)));
//G581: help_arg(234)        => def_sym(324).
	p(NT(234), (NT(324)));
//G582: help_arg(234)        => inst_sym(275).
	p(NT(234), (NT(275)));
//G583: help_arg(234)        => subst_sym(270).
	p(NT(234), (NT(270)));
//G584: help_arg(234)        => normalize_sym(266).
	p(NT(234), (NT(266)));
//G585: help_arg(234)        => execute_sym(356).
	p(NT(234), (NT(356)));
//G586: help_arg(234)        => solve_sym(251).
	p(NT(234), (NT(251)));
//G587: help_arg(234)        => valid_sym(241).
	p(NT(234), (NT(241)));
//G588: help_arg(234)        => sat_sym(245).
	p(NT(234), (NT(245)));
//G589: help_arg(234)        => unsat_sym(248).
	p(NT(234), (NT(248)));
//G590: help_arg(234)        => run_sym(260).
	p(NT(234), (NT(260)));
//G591: help_arg(234)        => dnf_sym(280).
	p(NT(234), (NT(280)));
//G592: help_arg(234)        => cnf_sym(283).
	p(NT(234), (NT(283)));
//G593: help_arg(234)        => anf_sym(286).
	p(NT(234), (NT(286)));
//G594: help_arg(234)        => snf_sym(357).
	p(NT(234), (NT(357)));
//G595: help_arg(234)        => nnf_sym(289).
	p(NT(234), (NT(289)));
//G596: help_arg(234)        => pnf_sym(292).
	p(NT(234), (NT(292)));
//G597: help_arg(234)        => mnf_sym(295).
	p(NT(234), (NT(295)));
//G598: help_arg(234)        => onf_sym(298).
	p(NT(234), (NT(298)));
//G599: help_arg(234)        => qelim_sym(302).
	p(NT(234), (NT(302)));
//G600: __E___E_help_arg_158_159(360) => 's'.
	p(NT(360), (T(15)));
//G601: __E___E_help_arg_158_159(360) => null.
	p(NT(360), (nul));
//G602: __E_help_arg_158(359) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_158_159(360).
	p(NT(359), (T(11)+T(29)+T(16)+T(21)+T(55)+T(10)+T(11)+NT(360)));
//G603: examples_sym(358)    => __E_help_arg_158(359).
	p(NT(358), (NT(359)));
//G604: help_arg(234)        => examples_sym(358).
	p(NT(234), (NT(358)));
//G605: __E_history_160(362) => '%'.
	p(NT(362), (T(40)));
//G606: rel_history_sym(354) => '%' '-'.
	p(NT(354), (T(40)+T(30)));
//G607: history_id(364)      => digits(206).
	p(NT(364), (NT(206)));
//G608: __E___E_history_160_161(363) => history_id(364).
	p(NT(363), (NT(364)));
//G609: __E___E_history_160_161(363) => null.
	p(NT(363), (nul));
//G610: __E_history_160(362) => rel_history_sym(354) __E___E_history_160_161(363).
	p(NT(362), (NT(354)+NT(363)));
//G611: rel_history(361)     => __E_history_160(362).
	p(NT(361), (NT(362)));
//G612: history(263)         => rel_history(361).
	p(NT(263), (NT(361)));
//G613: abs_history_sym(353) => '%'.
	p(NT(353), (T(40)));
//G614: __E_history_162(366) => abs_history_sym(353) history_id(364).
	p(NT(366), (NT(353)+NT(364)));
//G615: abs_history(365)     => __E_history_162(366).
	p(NT(365), (NT(366)));
//G616: history(263)         => abs_history(365).
	p(NT(263), (NT(365)));
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
		bf_parenthesis, __E_bf_44, ba_constant, variable, bf_fall, __E_bf_45, bf_fex, __E_bf_46, bf_ref, bf_or, 
		__E_bf_47, bf_xor, __E_bf_48, bf_add, __E_bf_49, bf_sub, __E_bf_50, bf_mul, __E_bf_51, bf_div, 
		__E_bf_52, bf_mod, __E_bf_53, bf_shr, __E_bf_54, bf_shl, __E_bf_55, bf_t, __E_bf_56, __E___E_bf_56_57, 
		bf_f, __E_bf_58, __E___E_bf_58_59, bf_and, __E_bf_60, __E___E_bf_60_61, bf_neg, __E_bf_62, bf_neg_oprnd, __E___E_bf_62_63, 
		bf_and_nosep, __E_bf_64, bf_and_nosep_1st_oprnd, __E___E_bf_64_65, bf_and_nosep_2nd_oprnd, __E___E_bf_64_66, ctn_neq, __E_constraint_67, ctnvar, num, 
		ctn_eq, __E_constraint_68, ctn_gteq, __E_constraint_69, ctn_gt, __E_constraint_70, ctn_lteq, __E_constraint_71, ctn_lt, __E_constraint_72, 
		__E_ba_constant_73, source, __E_ba_constant_74, src_c, __E_source_75, __E___E_source_75_76, __E___E_source_75_77, __E_src_c_78, __E_src_c_79, __E_variable_80, 
		uconst, io_var, var_name, __E_variable_81, __E_var_name_82, __E_var_name_83, __E_var_name_84, __E_var_name_85, __E_var_name_86, offset, 
		chars, uconst_name, __E_uconst_87, __E___E_uconst_87_88, q_var, __E_q_vars_89, __E_q_vars_90, __E_offsets_91, __E_offsets_92, integer, 
		shift, __E_offset_93, __E_shift_94, __E___E_shift_94_95, __E_chars_96, __E_chars_97, digits, __E_digits_98, __E_integer_99, type, 
		__E_typed_100, subtype, comment, __E_comment_101, __E_comment_102, __E_comment_103, cli, cli_command, __E_cli_104, __E_cli_105, 
		quit_cmd, quit_sym, __E_cli_command_106, version_cmd, version_sym, __E_cli_command_107, clear_cmd, clear_sym, __E_cli_command_108, help_cmd, 
		__E_cli_command_109, help_sym, __E___E_cli_command_109_110, __E___E_cli_command_109_111, help_arg, file_cmd, __E_cli_command_112, file_sym, q_string, valid_cmd, 
		__E_cli_command_113, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_114, sat_sym, unsat_cmd, __E_cli_command_115, unsat_sym, solve_cmd, 
		__E_cli_command_116, solve_sym, __E___E_cli_command_116_117, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_118, lgrs_sym, run_cmd, __E_cli_command_119, 
		run_sym, __E___E_cli_command_119_120, __E___E_cli_command_119_121, history, normalize_cmd, __E_cli_command_122, normalize_sym, __E___E_cli_command_122_123, subst_cmd, __E_cli_command_124, 
		subst_sym, __E___E_cli_command_124_125, nf_cmd_arg, inst_cmd, __E_cli_command_126, inst_sym, __E___E_cli_command_126_127, inst_args, dnf_cmd, __E_cli_command_128, 
		dnf_sym, cnf_cmd, __E_cli_command_129, cnf_sym, anf_cmd, __E_cli_command_130, anf_sym, nnf_cmd, __E_cli_command_131, nnf_sym, 
		pnf_cmd, __E_cli_command_132, pnf_sym, mnf_cmd, __E_cli_command_133, mnf_sym, onf_cmd, __E_cli_command_134, onf_sym, onf_cmd_arg, 
		qelim_cmd, __E_cli_command_135, qelim_sym, get_cmd, __E_cli_command_136, get_sym, __E___E_cli_command_136_137, option_name, set_cmd, __E_cli_command_138, 
		set_sym, __E___E_cli_command_138_139, option_value, enable_cmd, __E_cli_command_140, enable_sym, disable_cmd, __E_cli_command_141, disable_sym, toggle_cmd, 
		__E_cli_command_142, toggle_sym, def_list_cmd, __E_cli_command_143, def_sym, __E___E_cli_command_143_144, def_print_cmd, __E_cli_command_145, def_rr_cmd, def_input_cmd, 
		def_output_cmd, history_list_cmd, __E_cli_command_146, history_sym, __E___E_cli_command_146_147, history_print_cmd, __E_cli_command_148, history_store_cmd, __E_cli_command_149, __E_solve_options_150, 
		solve_option, __E_solve_options_151, __E_solve_option_152, solver_mode, solver_mode_minimum, __E_solver_mode_153, solver_mode_maximum, __E_solver_mode_154, file_name, __E_file_name_155, 
		__E_option_name_156, __E_option_value_157, bf_cmd_arg, abs_history_sym, rel_history_sym, selection_sym, execute_sym, snf_sym, examples_sym, __E_help_arg_158, 
		__E___E_help_arg_158_159, rel_history, __E_history_160, __E___E_history_160_161, history_id, abs_history, __E_history_162, __N_0, __N_1, __N_2, 
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
