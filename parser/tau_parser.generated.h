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
	"bf_nlt", "__E_wff_51", "bf_parenthesis", "__E_bf_52", "bf_constant", "variable", "bf_splitter", "__E_bf_53", "bf_ref", "bf_or", 
	"__E_bf_54", "bf_xor", "__E_bf_55", "bf_t", "__E_bf_56", "__E___E_bf_56_57", "bf_f", "__E_bf_58", "__E___E_bf_58_59", "bf_and", 
	"__E_bf_60", "__E___E_bf_60_61", "bf_neg", "__E_bf_62", "bf_neg_oprnd", "__E___E_bf_62_63", "bf_and_nosep", "__E_bf_64", "bf_and_nosep_1st_oprnd", "__E___E_bf_64_65", 
	"bf_and_nosep_2nd_oprnd", "__E___E_bf_64_66", "ctn_neq", "__E_constraint_67", "ctnvar", "num", "ctn_eq", "__E_constraint_68", "ctn_gteq", "__E_constraint_69", 
	"ctn_gt", "__E_constraint_70", "ctn_lteq", "__E_constraint_71", "ctn_lt", "__E_constraint_72", "__E_bf_constant_73", "source", "__E_bf_constant_74", "src_c", 
	"__E_source_75", "__E___E_source_75_76", "__E___E_source_75_77", "__E_src_c_78", "__E_src_c_79", "__E_variable_80", "uconst", "io_var", "var_name", "__E_variable_81", 
	"__E_var_name_82", "__E_var_name_83", "__E_var_name_84", "__E_var_name_85", "__E_var_name_86", "offset", "chars", "uconst_name", "__E_uconst_87", "__E___E_uconst_87_88", 
	"q_var", "__E_q_vars_89", "__E_q_vars_90", "__E_offsets_91", "__E_offsets_92", "integer", "shift", "__E_offset_93", "__E_shift_94", "__E___E_shift_94_95", 
	"__E_chars_96", "__E_chars_97", "digits", "__E_digits_98", "__E_integer_99", "type", "scope_id", "comment", "__E_comment_100", "__E_comment_101", 
	"__E_comment_102", "cli", "cli_command", "__E_cli_103", "__E_cli_104", "quit_cmd", "quit_sym", "__E_cli_command_105", "version_cmd", "version_sym", 
	"__E_cli_command_106", "clear_cmd", "clear_sym", "__E_cli_command_107", "help_cmd", "__E_cli_command_108", "help_sym", "__E___E_cli_command_108_109", "__E___E_cli_command_108_110", "help_arg", 
	"file_cmd", "__E_cli_command_111", "file_sym", "q_string", "valid_cmd", "__E_cli_command_112", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_113", 
	"sat_sym", "unsat_cmd", "__E_cli_command_114", "unsat_sym", "solve_cmd", "__E_cli_command_115", "solve_sym", "__E___E_cli_command_115_116", "solve_options", "wff_cmd_arg", 
	"lgrs_cmd", "__E_cli_command_117", "lgrs_sym", "run_cmd", "__E_cli_command_118", "run_sym", "__E___E_cli_command_118_119", "__E___E_cli_command_118_120", "history", "normalize_cmd", 
	"__E_cli_command_121", "normalize_sym", "__E___E_cli_command_121_122", "subst_cmd", "__E_cli_command_123", "subst_sym", "__E___E_cli_command_123_124", "nf_cmd_arg", "inst_cmd", "__E_cli_command_125", 
	"inst_sym", "__E___E_cli_command_125_126", "inst_args", "dnf_cmd", "__E_cli_command_127", "dnf_sym", "cnf_cmd", "__E_cli_command_128", "cnf_sym", "anf_cmd", 
	"__E_cli_command_129", "anf_sym", "nnf_cmd", "__E_cli_command_130", "nnf_sym", "pnf_cmd", "__E_cli_command_131", "pnf_sym", "mnf_cmd", "__E_cli_command_132", 
	"mnf_sym", "snf_cmd", "__E_cli_command_133", "snf_sym", "onf_cmd", "__E_cli_command_134", "onf_sym", "onf_cmd_arg", "qelim_cmd", "__E_cli_command_135", 
	"qelim_sym", "get_cmd", "__E_cli_command_136", "get_sym", "__E___E_cli_command_136_137", "option_name", "set_cmd", "__E_cli_command_138", "set_sym", "__E___E_cli_command_138_139", 
	"option_value", "enable_cmd", "__E_cli_command_140", "enable_sym", "disable_cmd", "__E_cli_command_141", "disable_sym", "toggle_cmd", "__E_cli_command_142", "toggle_sym", 
	"def_list_cmd", "__E_cli_command_143", "def_sym", "__E___E_cli_command_143_144", "def_print_cmd", "__E_cli_command_145", "def_rr_cmd", "def_input_cmd", "def_output_cmd", "history_list_cmd", 
	"__E_cli_command_146", "history_sym", "__E___E_cli_command_146_147", "history_print_cmd", "__E_cli_command_148", "history_store_cmd", "__E_cli_command_149", "__E_solve_options_150", "solve_option", "__E_solve_options_151", 
	"__E_solve_option_152", "solver_mode", "solver_mode_minimum", "__E_solver_mode_153", "solver_mode_maximum", "__E_solver_mode_154", "file_name", "__E_file_name_155", "__E_option_name_156", "__E_option_value_157", 
	"bf_cmd_arg", "abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", "examples_sym", "__E_help_arg_158", "__E___E_help_arg_158_159", "rel_history", "__E_history_160", 
	"__E___E_history_160_161", "history_id", "abs_history", "__E_history_162", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", 
	"__N_6", "__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', 'a', 'b', 'k', 
	'r', ',', 'm', '<', '>', 'w', 'y', '[', ']', '?', 
	'x', '-', '|', '^', '&', '!', 'T', 'F', 'S', '+', 
	'1', '0', '\'', '{', '}', '_', '$', '\t', '\n', '\r', 
	'#', 'q', 'v', 'h', 'p', 'd', 'g', 'z', '/', '"', 
	'%', 
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
			315, 320, 356
		},
		.to_inline = {
			{ 14, 77, 14 },
			{ 30, 132, 30 },
			{ 33 },
			{ 154, 132, 30 },
			{ 156, 132, 149 },
			{ 158, 132, 30 },
			{ 160, 132, 30 },
			{ 179 },
			{ 186 },
			{ 186, 197 },
			{ 196 },
			{ 200 },
			{ 247 },
			{ 259 },
			{ 277 },
			{ 282 },
			{ 307 },
			{ 360 }
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
//G171: wff(14)              => capture(29).
	p(NT(14), (NT(29)));
//G172: __E_bf_52(133)       => '(' _(11) bf(30) _(11) ')'.
	p(NT(133), (T(12)+NT(11)+NT(30)+NT(11)+T(13)));
//G173: bf_parenthesis(132)  => __E_bf_52(133).
	p(NT(132), (NT(133)));
//G174: bf(30)               => bf_parenthesis(132).
	p(NT(30), (NT(132)));
//G175: bf(30)               => bf_constant(134).
	p(NT(30), (NT(134)));
//G176: bf(30)               => variable(135).
	p(NT(30), (NT(135)));
//G177: __E_bf_53(137)       => 'S' _(11) '(' _(11) bf(30) _(11) ')'.
	p(NT(137), (T(37)+NT(11)+T(12)+NT(11)+NT(30)+NT(11)+T(13)));
//G178: bf_splitter(136)     => __E_bf_53(137).
	p(NT(136), (NT(137)));
//G179: bf(30)               => bf_splitter(136).
	p(NT(30), (NT(136)));
//G180: bf_ref(138)          => ref(27).
	p(NT(138), (NT(27)));
//G181: bf(30)               => bf_ref(138).
	p(NT(30), (NT(138)));
//G182: __E_bf_54(140)       => bf(30) _(11) '|' _(11) bf(30).
	p(NT(140), (NT(30)+NT(11)+T(31)+NT(11)+NT(30)));
//G183: bf_or(139)           => __E_bf_54(140).
	p(NT(139), (NT(140)));
//G184: bf(30)               => bf_or(139).
	p(NT(30), (NT(139)));
//G185: __E_bf_55(142)       => bf(30) _(11) '+' _(11) bf(30).
	p(NT(142), (NT(30)+NT(11)+T(38)+NT(11)+NT(30)));
//G186: bf_xor(141)          => __E_bf_55(142).
	p(NT(141), (NT(142)));
//G187: bf(30)               => bf_xor(141).
	p(NT(30), (NT(141)));
//G188: __E___E_bf_56_57(145) => typed(33).
	p(NT(145), (NT(33)));
//G189: __E___E_bf_56_57(145) => null.
	p(NT(145), (nul));
//G190: __E_bf_56(144)       => '1' __E___E_bf_56_57(145).
	p(NT(144), (T(39)+NT(145)));
//G191: bf_t(143)            => __E_bf_56(144).
	p(NT(143), (NT(144)));
//G192: bf(30)               => bf_t(143).
	p(NT(30), (NT(143)));
//G193: __E___E_bf_58_59(148) => typed(33).
	p(NT(148), (NT(33)));
//G194: __E___E_bf_58_59(148) => null.
	p(NT(148), (nul));
//G195: __E_bf_58(147)       => '0' __E___E_bf_58_59(148).
	p(NT(147), (T(40)+NT(148)));
//G196: bf_f(146)            => __E_bf_58(147).
	p(NT(146), (NT(147)));
//G197: bf(30)               => bf_f(146).
	p(NT(30), (NT(146)));
//G198: __E___E_bf_60_61(151) => _(11) '&' _(11).
	p(NT(151), (NT(11)+T(33)+NT(11)));
//G199: __E___E_bf_60_61(151) => __(34).
	p(NT(151), (NT(34)));
//G200: __E_bf_60(150)       => bf(30) __E___E_bf_60_61(151) bf(30).
	p(NT(150), (NT(30)+NT(151)+NT(30)));
//G201: bf_and(149)          => __E_bf_60(150).
	p(NT(149), (NT(150)));
//G202: bf(30)               => bf_and(149).
	p(NT(30), (NT(149)));
//G203: __E___E_bf_62_63(155) => bf_parenthesis(132).
	p(NT(155), (NT(132)));
//G204: __E___E_bf_62_63(155) => bf_constant(134).
	p(NT(155), (NT(134)));
//G205: __E___E_bf_62_63(155) => variable(135).
	p(NT(155), (NT(135)));
//G206: __E___E_bf_62_63(155) => bf_splitter(136).
	p(NT(155), (NT(136)));
//G207: __E___E_bf_62_63(155) => bf_ref(138).
	p(NT(155), (NT(138)));
//G208: __E___E_bf_62_63(155) => bf_t(143).
	p(NT(155), (NT(143)));
//G209: __E___E_bf_62_63(155) => bf_f(146).
	p(NT(155), (NT(146)));
//G210: __E___E_bf_62_63(155) => bf_neg(152).
	p(NT(155), (NT(152)));
//G211: __E___E_bf_62_63(155) => capture(29).
	p(NT(155), (NT(29)));
//G212: bf_neg_oprnd(154)    => __E___E_bf_62_63(155).
	p(NT(154), (NT(155)));
//G213: __E_bf_62(153)       => bf_neg_oprnd(154) _(11) '\''.
	p(NT(153), (NT(154)+NT(11)+T(41)));
//G214: bf_neg(152)          => __E_bf_62(153).
	p(NT(152), (NT(153)));
//G215: bf(30)               => bf_neg(152).
	p(NT(30), (NT(152)));
//G216: __E___E_bf_64_65(159) => bf_parenthesis(132).
	p(NT(159), (NT(132)));
//G217: __E___E_bf_64_65(159) => variable(135).
	p(NT(159), (NT(135)));
//G218: __E___E_bf_64_65(159) => bf_splitter(136).
	p(NT(159), (NT(136)));
//G219: __E___E_bf_64_65(159) => bf_ref(138).
	p(NT(159), (NT(138)));
//G220: __E___E_bf_64_65(159) => bf_neg(152).
	p(NT(159), (NT(152)));
//G221: __E___E_bf_64_65(159) => bf_and_nosep(156).
	p(NT(159), (NT(156)));
//G222: __E___E_bf_64_65(159) => capture(29).
	p(NT(159), (NT(29)));
//G223: bf_and_nosep_1st_oprnd(158) => __E___E_bf_64_65(159).
	p(NT(158), (NT(159)));
//G224: __E___E_bf_64_66(161) => bf_parenthesis(132).
	p(NT(161), (NT(132)));
//G225: __E___E_bf_64_66(161) => bf_constant(134).
	p(NT(161), (NT(134)));
//G226: __E___E_bf_64_66(161) => variable(135).
	p(NT(161), (NT(135)));
//G227: __E___E_bf_64_66(161) => bf_splitter(136).
	p(NT(161), (NT(136)));
//G228: __E___E_bf_64_66(161) => bf_ref(138).
	p(NT(161), (NT(138)));
//G229: __E___E_bf_64_66(161) => bf_neg(152).
	p(NT(161), (NT(152)));
//G230: __E___E_bf_64_66(161) => capture(29).
	p(NT(161), (NT(29)));
//G231: bf_and_nosep_2nd_oprnd(160) => __E___E_bf_64_66(161).
	p(NT(160), (NT(161)));
//G232: __E_bf_64(157)       => bf_and_nosep_1st_oprnd(158) bf_and_nosep_2nd_oprnd(160).
	p(NT(157), (NT(158)+NT(160)));
//G233: bf_and_nosep(156)    => __E_bf_64(157).
	p(NT(156), (NT(157)));
//G234: bf(30)               => bf_and_nosep(156).
	p(NT(30), (NT(156)));
//G235: bf(30)               => capture(29).
	p(NT(30), (NT(29)));
//G236: __E_constraint_67(163) => '[' ctnvar(164) _(11) '!' '=' _(11) num(165) ']'.
	p(NT(163), (T(26)+NT(164)+NT(11)+T(34)+T(3)+NT(11)+NT(165)+T(27)));
//G237: __E_constraint_67(163) => '[' num(165) _(11) '!' '=' _(11) ctnvar(164) ']'.
	p(NT(163), (T(26)+NT(165)+NT(11)+T(34)+T(3)+NT(11)+NT(164)+T(27)));
//G238: ctn_neq(162)         => __E_constraint_67(163).
	p(NT(162), (NT(163)));
//G239: constraint(93)       => ctn_neq(162).
	p(NT(93), (NT(162)));
//G240: __E_constraint_68(167) => '[' ctnvar(164) _(11) '=' _(11) num(165) ']'.
	p(NT(167), (T(26)+NT(164)+NT(11)+T(3)+NT(11)+NT(165)+T(27)));
//G241: __E_constraint_68(167) => '[' num(165) _(11) '=' _(11) ctnvar(164) ']'.
	p(NT(167), (T(26)+NT(165)+NT(11)+T(3)+NT(11)+NT(164)+T(27)));
//G242: ctn_eq(166)          => __E_constraint_68(167).
	p(NT(166), (NT(167)));
//G243: constraint(93)       => ctn_eq(166).
	p(NT(93), (NT(166)));
//G244: __E_constraint_69(169) => '[' ctnvar(164) _(11) '>' '=' _(11) num(165) ']'.
	p(NT(169), (T(26)+NT(164)+NT(11)+T(23)+T(3)+NT(11)+NT(165)+T(27)));
//G245: __E_constraint_69(169) => '[' num(165) _(11) '>' '=' _(11) ctnvar(164) ']'.
	p(NT(169), (T(26)+NT(165)+NT(11)+T(23)+T(3)+NT(11)+NT(164)+T(27)));
//G246: ctn_gteq(168)        => __E_constraint_69(169).
	p(NT(168), (NT(169)));
//G247: constraint(93)       => ctn_gteq(168).
	p(NT(93), (NT(168)));
//G248: __E_constraint_70(171) => '[' ctnvar(164) _(11) '>' _(11) num(165) ']'.
	p(NT(171), (T(26)+NT(164)+NT(11)+T(23)+NT(11)+NT(165)+T(27)));
//G249: __E_constraint_70(171) => '[' num(165) _(11) '>' _(11) ctnvar(164) ']'.
	p(NT(171), (T(26)+NT(165)+NT(11)+T(23)+NT(11)+NT(164)+T(27)));
//G250: ctn_gt(170)          => __E_constraint_70(171).
	p(NT(170), (NT(171)));
//G251: constraint(93)       => ctn_gt(170).
	p(NT(93), (NT(170)));
//G252: __E_constraint_71(173) => '[' ctnvar(164) _(11) '<' '=' _(11) num(165) ']'.
	p(NT(173), (T(26)+NT(164)+NT(11)+T(22)+T(3)+NT(11)+NT(165)+T(27)));
//G253: __E_constraint_71(173) => '[' num(165) _(11) '<' '=' _(11) ctnvar(164) ']'.
	p(NT(173), (T(26)+NT(165)+NT(11)+T(22)+T(3)+NT(11)+NT(164)+T(27)));
//G254: ctn_lteq(172)        => __E_constraint_71(173).
	p(NT(172), (NT(173)));
//G255: constraint(93)       => ctn_lteq(172).
	p(NT(93), (NT(172)));
//G256: __E_constraint_72(175) => '[' ctnvar(164) _(11) '<' _(11) num(165) ']'.
	p(NT(175), (T(26)+NT(164)+NT(11)+T(22)+NT(11)+NT(165)+T(27)));
//G257: __E_constraint_72(175) => '[' num(165) _(11) '<' _(11) ctnvar(164) ']'.
	p(NT(175), (T(26)+NT(165)+NT(11)+T(22)+NT(11)+NT(164)+T(27)));
//G258: ctn_lt(174)          => __E_constraint_72(175).
	p(NT(174), (NT(175)));
//G259: constraint(93)       => ctn_lt(174).
	p(NT(93), (NT(174)));
//G260: __E_bf_constant_73(176) => capture(29).
	p(NT(176), (NT(29)));
//G261: __E_bf_constant_73(176) => source(177).
	p(NT(176), (NT(177)));
//G262: __E_bf_constant_74(178) => typed(33).
	p(NT(178), (NT(33)));
//G263: __E_bf_constant_74(178) => null.
	p(NT(178), (nul));
//G264: bf_constant(134)     => '{' _(11) __E_bf_constant_73(176) _(11) '}' __E_bf_constant_74(178).
	p(NT(134), (T(42)+NT(11)+NT(176)+NT(11)+T(43)+NT(178)));
//G265: __E___E_source_75_76(181) => src_c(179).
	p(NT(181), (NT(179)));
//G266: __E___E_source_75_76(181) => space(2).
	p(NT(181), (NT(2)));
//G267: __E___E_source_75_77(182) => null.
	p(NT(182), (nul));
//G268: __E___E_source_75_77(182) => __E___E_source_75_76(181) __E___E_source_75_77(182).
	p(NT(182), (NT(181)+NT(182)));
//G269: __E_source_75(180)   => __E___E_source_75_77(182) src_c(179).
	p(NT(180), (NT(182)+NT(179)));
//G270: __E_source_75(180)   => null.
	p(NT(180), (nul));
//G271: source(177)          => src_c(179) __E_source_75(180).
	p(NT(177), (NT(179)+NT(180)));
//G272: src_c(179)           => alnum(6).
	p(NT(179), (NT(6)));
//G273: __N_0(374)           => '{'.
	p(NT(374), (T(42)));
//G274: __N_1(375)           => '}'.
	p(NT(375), (T(43)));
//G275: src_c(179)           => ~( __N_0(374) ) & ~( __N_1(375) ) & punct(7).	 # conjunctive
	p(NT(179), ~(NT(374)) & ~(NT(375)) & (NT(7)));
//G276: __E_src_c_78(183)    => src_c(179).
	p(NT(183), (NT(179)));
//G277: __E_src_c_78(183)    => space(2).
	p(NT(183), (NT(2)));
//G278: __E_src_c_79(184)    => null.
	p(NT(184), (nul));
//G279: __E_src_c_79(184)    => __E_src_c_78(183) __E_src_c_79(184).
	p(NT(184), (NT(183)+NT(184)));
//G280: src_c(179)           => '{' __E_src_c_79(184) '}'.
	p(NT(179), (T(42)+NT(184)+T(43)));
//G281: __E_variable_80(185) => uconst(186).
	p(NT(185), (NT(186)));
//G282: __E_variable_80(185) => io_var(187).
	p(NT(185), (NT(187)));
//G283: __E_variable_80(185) => var_name(188).
	p(NT(185), (NT(188)));
//G284: __E_variable_81(189) => typed(33).
	p(NT(189), (NT(33)));
//G285: __E_variable_81(189) => null.
	p(NT(189), (nul));
//G286: variable(135)        => __E_variable_80(185) __E_variable_81(189).
	p(NT(135), (NT(185)+NT(189)));
//G287: __N_2(376)           => 'F'.
	p(NT(376), (T(36)));
//G288: __N_3(377)           => 'T'.
	p(NT(377), (T(35)));
//G289: __E_var_name_82(190) => ~( __N_2(376) ) & ~( __N_3(377) ) & alpha(5).	 # conjunctive
	p(NT(190), ~(NT(376)) & ~(NT(377)) & (NT(5)));
//G290: __E_var_name_83(191) => null.
	p(NT(191), (nul));
//G291: __E_var_name_83(191) => digit(3) __E_var_name_83(191).
	p(NT(191), (NT(3)+NT(191)));
//G292: var_name(188)        => __E_var_name_82(190) __E_var_name_83(191).	 # guarded: charvar
	p(NT(188), (NT(190)+NT(191)));
	p.back().guard = "charvar";
//G293: __N_4(378)           => 'F'.
	p(NT(378), (T(36)));
//G294: __N_5(379)           => 'T'.
	p(NT(379), (T(35)));
//G295: __E_var_name_84(192) => ~( __N_4(378) ) & ~( __N_5(379) ) & alpha(5).	 # conjunctive
	p(NT(192), ~(NT(378)) & ~(NT(379)) & (NT(5)));
//G296: __E_var_name_85(193) => alnum(6).
	p(NT(193), (NT(6)));
//G297: __E_var_name_85(193) => '_'.
	p(NT(193), (T(44)));
//G298: __E_var_name_86(194) => null.
	p(NT(194), (nul));
//G299: __E_var_name_86(194) => __E_var_name_85(193) __E_var_name_86(194).
	p(NT(194), (NT(193)+NT(194)));
//G300: var_name(188)        => __E_var_name_84(192) __E_var_name_86(194).	 # guarded: var
	p(NT(188), (NT(192)+NT(194)));
	p.back().guard = "var";
//G301: io_var(187)          => io_var_name(31) '[' offset(195) ']'.
	p(NT(187), (NT(31)+T(26)+NT(195)+T(27)));
//G302: io_var_name(31)      => chars(196).
	p(NT(31), (NT(196)));
//G303: __E___E_uconst_87_88(199) => chars(196) _(11).
	p(NT(199), (NT(196)+NT(11)));
//G304: __E___E_uconst_87_88(199) => null.
	p(NT(199), (nul));
//G305: __E_uconst_87(198)   => __E___E_uconst_87_88(199) ':' _(11) chars(196).
	p(NT(198), (NT(199)+T(2)+NT(11)+NT(196)));
//G306: uconst_name(197)     => __E_uconst_87(198).
	p(NT(197), (NT(198)));
//G307: uconst(186)          => '<' _(11) uconst_name(197) _(11) '>'.
	p(NT(186), (T(22)+NT(11)+NT(197)+NT(11)+T(23)));
//G308: __E_q_vars_89(201)   => _(11) ',' _(11) q_var(200).
	p(NT(201), (NT(11)+T(20)+NT(11)+NT(200)));
//G309: __E_q_vars_90(202)   => null.
	p(NT(202), (nul));
//G310: __E_q_vars_90(202)   => __E_q_vars_89(201) __E_q_vars_90(202).
	p(NT(202), (NT(201)+NT(202)));
//G311: q_vars(89)           => q_var(200) __E_q_vars_90(202).
	p(NT(89), (NT(200)+NT(202)));
//G312: q_var(200)           => capture(29).
	p(NT(200), (NT(29)));
//G313: __N_6(380)           => uconst(186).
	p(NT(380), (NT(186)));
//G314: q_var(200)           => variable(135) & ~( __N_6(380) ).	 # conjunctive
	p(NT(200), (NT(135)) & ~(NT(380)));
//G315: ctnvar(164)          => chars(196).
	p(NT(164), (NT(196)));
//G316: __E_offsets_91(203)  => _(11) ',' _(11) offset(195).
	p(NT(203), (NT(11)+T(20)+NT(11)+NT(195)));
//G317: __E_offsets_92(204)  => null.
	p(NT(204), (nul));
//G318: __E_offsets_92(204)  => __E_offsets_91(203) __E_offsets_92(204).
	p(NT(204), (NT(203)+NT(204)));
//G319: offsets(42)          => '[' _(11) offset(195) __E_offsets_92(204) _(11) ']'.
	p(NT(42), (T(26)+NT(11)+NT(195)+NT(204)+NT(11)+T(27)));
//G320: offset(195)          => integer(205).
	p(NT(195), (NT(205)));
//G321: offset(195)          => capture(29).
	p(NT(195), (NT(29)));
//G322: offset(195)          => shift(206).
	p(NT(195), (NT(206)));
//G323: __N_7(381)           => io_var(187).
	p(NT(381), (NT(187)));
//G324: __E_offset_93(207)   => variable(135) & ~( __N_7(381) ).	 # conjunctive
	p(NT(207), (NT(135)) & ~(NT(381)));
//G325: offset(195)          => __E_offset_93(207).
	p(NT(195), (NT(207)));
//G326: __E_shift_94(208)    => capture(29).
	p(NT(208), (NT(29)));
//G327: __N_8(382)           => io_var(187).
	p(NT(382), (NT(187)));
//G328: __E___E_shift_94_95(209) => variable(135) & ~( __N_8(382) ).	 # conjunctive
	p(NT(209), (NT(135)) & ~(NT(382)));
//G329: __E_shift_94(208)    => __E___E_shift_94_95(209).
	p(NT(208), (NT(209)));
//G330: shift(206)           => __E_shift_94(208) _(11) '-' _(11) num(165).
	p(NT(206), (NT(208)+NT(11)+T(30)+NT(11)+NT(165)));
//G331: __E_chars_96(210)    => alnum(6).
	p(NT(210), (NT(6)));
//G332: __E_chars_96(210)    => '_'.
	p(NT(210), (T(44)));
//G333: __E_chars_97(211)    => null.
	p(NT(211), (nul));
//G334: __E_chars_97(211)    => __E_chars_96(210) __E_chars_97(211).
	p(NT(211), (NT(210)+NT(211)));
//G335: chars(196)           => alpha(5) __E_chars_97(211).
	p(NT(196), (NT(5)+NT(211)));
//G336: __E_digits_98(213)   => digit(3).
	p(NT(213), (NT(3)));
//G337: __E_digits_98(213)   => digit(3) __E_digits_98(213).
	p(NT(213), (NT(3)+NT(213)));
//G338: digits(212)          => __E_digits_98(213).
	p(NT(212), (NT(213)));
//G339: num(165)             => digits(212).
	p(NT(165), (NT(212)));
//G340: __E_integer_99(214)  => '-'.
	p(NT(214), (T(30)));
//G341: __E_integer_99(214)  => null.
	p(NT(214), (nul));
//G342: integer(205)         => __E_integer_99(214) _(11) digits(212).
	p(NT(205), (NT(214)+NT(11)+NT(212)));
//G343: sym(40)              => chars(196).
	p(NT(40), (NT(196)));
//G344: capture(29)          => '$' chars(196).
	p(NT(29), (T(45)+NT(196)));
//G345: type(215)            => chars(196).
	p(NT(215), (NT(196)));
//G346: typed(33)            => _(11) ':' _(11) type(215).
	p(NT(33), (NT(11)+T(2)+NT(11)+NT(215)));
//G347: scope_id(216)        => digits(212).
	p(NT(216), (NT(212)));
//G348: __E_comment_100(218) => printable(8).
	p(NT(218), (NT(8)));
//G349: __E_comment_100(218) => '\t'.
	p(NT(218), (T(46)));
//G350: __E_comment_101(219) => null.
	p(NT(219), (nul));
//G351: __E_comment_101(219) => __E_comment_100(218) __E_comment_101(219).
	p(NT(219), (NT(218)+NT(219)));
//G352: __E_comment_102(220) => '\n'.
	p(NT(220), (T(47)));
//G353: __E_comment_102(220) => '\r'.
	p(NT(220), (T(48)));
//G354: __E_comment_102(220) => eof(1).
	p(NT(220), (NT(1)));
//G355: comment(217)         => '#' __E_comment_101(219) __E_comment_102(220).
	p(NT(217), (T(49)+NT(219)+NT(220)));
//G356: __(34)               => space(2).
	p(NT(34), (NT(2)));
//G357: __(34)               => comment(217).
	p(NT(34), (NT(217)));
//G358: __(34)               => __(34) space(2).
	p(NT(34), (NT(34)+NT(2)));
//G359: __(34)               => __(34) comment(217).
	p(NT(34), (NT(34)+NT(217)));
//G360: _(11)                => __(34).
	p(NT(11), (NT(34)));
//G361: _(11)                => null.
	p(NT(11), (nul));
//G362: cli(221)             => _(11).
	p(NT(221), (NT(11)));
//G363: __E_cli_103(223)     => '.' _(11) cli_command(222) _(11).
	p(NT(223), (T(1)+NT(11)+NT(222)+NT(11)));
//G364: __E_cli_104(224)     => null.
	p(NT(224), (nul));
//G365: __E_cli_104(224)     => __E_cli_103(223) __E_cli_104(224).
	p(NT(224), (NT(223)+NT(224)));
//G366: cli(221)             => _(11) cli_command(222) _(11) __E_cli_104(224).
	p(NT(221), (NT(11)+NT(222)+NT(11)+NT(224)));
//G367: __E_cli_command_105(227) => 'q'.
	p(NT(227), (T(50)));
//G368: __E_cli_command_105(227) => 'q' 'u' 'i' 't'.
	p(NT(227), (T(50)+T(7)+T(4)+T(8)));
//G369: quit_sym(226)        => __E_cli_command_105(227).
	p(NT(226), (NT(227)));
//G370: quit_cmd(225)        => quit_sym(226).
	p(NT(225), (NT(226)));
//G371: cli_command(222)     => quit_cmd(225).
	p(NT(222), (NT(225)));
//G372: __E_cli_command_106(230) => 'v'.
	p(NT(230), (T(51)));
//G373: __E_cli_command_106(230) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(230), (T(51)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G374: version_sym(229)     => __E_cli_command_106(230).
	p(NT(229), (NT(230)));
//G375: version_cmd(228)     => version_sym(229).
	p(NT(228), (NT(229)));
//G376: cli_command(222)     => version_cmd(228).
	p(NT(222), (NT(228)));
//G377: __E_cli_command_107(233) => 'c'.
	p(NT(233), (T(14)));
//G378: __E_cli_command_107(233) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(233), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G379: clear_sym(232)       => __E_cli_command_107(233).
	p(NT(232), (NT(233)));
//G380: clear_cmd(231)       => clear_sym(232).
	p(NT(231), (NT(232)));
//G381: cli_command(222)     => clear_cmd(231).
	p(NT(222), (NT(231)));
//G382: __E___E_cli_command_108_109(237) => 'h'.
	p(NT(237), (T(52)));
//G383: __E___E_cli_command_108_109(237) => 'h' 'e' 'l' 'p'.
	p(NT(237), (T(52)+T(11)+T(10)+T(53)));
//G384: help_sym(236)        => __E___E_cli_command_108_109(237).
	p(NT(236), (NT(237)));
//G385: __E___E_cli_command_108_110(238) => __(34) help_arg(239).
	p(NT(238), (NT(34)+NT(239)));
//G386: __E___E_cli_command_108_110(238) => null.
	p(NT(238), (nul));
//G387: __E_cli_command_108(235) => help_sym(236) __E___E_cli_command_108_110(238).
	p(NT(235), (NT(236)+NT(238)));
//G388: help_cmd(234)        => __E_cli_command_108(235).
	p(NT(234), (NT(235)));
//G389: cli_command(222)     => help_cmd(234).
	p(NT(222), (NT(234)));
//G390: file_sym(242)        => 'f' 'i' 'l' 'e'.
	p(NT(242), (T(9)+T(4)+T(10)+T(11)));
//G391: __E_cli_command_111(241) => file_sym(242) __(34) q_string(243).
	p(NT(241), (NT(242)+NT(34)+NT(243)));
//G392: file_cmd(240)        => __E_cli_command_111(241).
	p(NT(240), (NT(241)));
//G393: cli_command(222)     => file_cmd(240).
	p(NT(222), (NT(240)));
//G394: valid_sym(246)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(246), (T(51)+T(16)+T(10)+T(4)+T(54)));
//G395: __E_cli_command_112(245) => valid_sym(246) __(34) normalize_cmd_arg(247).
	p(NT(245), (NT(246)+NT(34)+NT(247)));
//G396: valid_cmd(244)       => __E_cli_command_112(245).
	p(NT(244), (NT(245)));
//G397: cli_command(222)     => valid_cmd(244).
	p(NT(222), (NT(244)));
//G398: sat_sym(250)         => 's' 'a' 't'.
	p(NT(250), (T(15)+T(16)+T(8)));
//G399: __E_cli_command_113(249) => sat_sym(250) __(34) normalize_cmd_arg(247).
	p(NT(249), (NT(250)+NT(34)+NT(247)));
//G400: sat_cmd(248)         => __E_cli_command_113(249).
	p(NT(248), (NT(249)));
//G401: cli_command(222)     => sat_cmd(248).
	p(NT(222), (NT(248)));
//G402: unsat_sym(253)       => 'u' 'n' 's' 'a' 't'.
	p(NT(253), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G403: __E_cli_command_114(252) => unsat_sym(253) __(34) normalize_cmd_arg(247).
	p(NT(252), (NT(253)+NT(34)+NT(247)));
//G404: unsat_cmd(251)       => __E_cli_command_114(252).
	p(NT(251), (NT(252)));
//G405: cli_command(222)     => unsat_cmd(251).
	p(NT(222), (NT(251)));
//G406: solve_sym(256)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(256), (T(15)+T(6)+T(10)+T(51)+T(11)));
//G407: __E___E_cli_command_115_116(257) => solve_options(258).
	p(NT(257), (NT(258)));
//G408: __E___E_cli_command_115_116(257) => null.
	p(NT(257), (nul));
//G409: __E_cli_command_115(255) => solve_sym(256) __E___E_cli_command_115_116(257) __(34) wff_cmd_arg(259).
	p(NT(255), (NT(256)+NT(257)+NT(34)+NT(259)));
//G410: solve_cmd(254)       => __E_cli_command_115(255).
	p(NT(254), (NT(255)));
//G411: cli_command(222)     => solve_cmd(254).
	p(NT(222), (NT(254)));
//G412: lgrs_sym(262)        => 'l' 'g' 'r' 's'.
	p(NT(262), (T(10)+T(55)+T(19)+T(15)));
//G413: __E_cli_command_117(261) => lgrs_sym(262) __(34) wff_cmd_arg(259).
	p(NT(261), (NT(262)+NT(34)+NT(259)));
//G414: lgrs_cmd(260)        => __E_cli_command_117(261).
	p(NT(260), (NT(261)));
//G415: cli_command(222)     => lgrs_cmd(260).
	p(NT(222), (NT(260)));
//G416: __E___E_cli_command_118_119(266) => 'r'.
	p(NT(266), (T(19)));
//G417: __E___E_cli_command_118_119(266) => 'r' 'u' 'n'.
	p(NT(266), (T(19)+T(7)+T(5)));
//G418: run_sym(265)         => __E___E_cli_command_118_119(266).
	p(NT(265), (NT(266)));
//G419: __E___E_cli_command_118_120(267) => history(268).
	p(NT(267), (NT(268)));
//G420: __E___E_cli_command_118_120(267) => wff(14).
	p(NT(267), (NT(14)));
//G421: __E_cli_command_118(264) => run_sym(265) __(34) __E___E_cli_command_118_120(267).
	p(NT(264), (NT(265)+NT(34)+NT(267)));
//G422: run_cmd(263)         => __E_cli_command_118(264).
	p(NT(263), (NT(264)));
//G423: cli_command(222)     => run_cmd(263).
	p(NT(222), (NT(263)));
//G424: __E___E_cli_command_121_122(272) => 'n'.
	p(NT(272), (T(5)));
//G425: __E___E_cli_command_121_122(272) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(272), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(56)+T(11)));
//G426: normalize_sym(271)   => __E___E_cli_command_121_122(272).
	p(NT(271), (NT(272)));
//G427: __E_cli_command_121(270) => normalize_sym(271) __(34) normalize_cmd_arg(247).
	p(NT(270), (NT(271)+NT(34)+NT(247)));
//G428: normalize_cmd(269)   => __E_cli_command_121(270).
	p(NT(269), (NT(270)));
//G429: cli_command(222)     => normalize_cmd(269).
	p(NT(222), (NT(269)));
//G430: __E___E_cli_command_123_124(276) => 's'.
	p(NT(276), (T(15)));
//G431: __E___E_cli_command_123_124(276) => 's' 'u' 'b' 's' 't'.
	p(NT(276), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G432: __E___E_cli_command_123_124(276) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(276), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G433: subst_sym(275)       => __E___E_cli_command_123_124(276).
	p(NT(275), (NT(276)));
//G434: __E_cli_command_123(274) => subst_sym(275) __(34) nf_cmd_arg(277) _(11) '[' _(11) nf_cmd_arg(277) _(11) '/' _(11) nf_cmd_arg(277) _(11) ']'.
	p(NT(274), (NT(275)+NT(34)+NT(277)+NT(11)+T(26)+NT(11)+NT(277)+NT(11)+T(57)+NT(11)+NT(277)+NT(11)+T(27)));
//G435: subst_cmd(273)       => __E_cli_command_123(274).
	p(NT(273), (NT(274)));
//G436: cli_command(222)     => subst_cmd(273).
	p(NT(222), (NT(273)));
//G437: __E___E_cli_command_125_126(281) => 'i'.
	p(NT(281), (T(4)));
//G438: __E___E_cli_command_125_126(281) => 'i' 'n' 's' 't'.
	p(NT(281), (T(4)+T(5)+T(15)+T(8)));
//G439: __E___E_cli_command_125_126(281) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(281), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G440: inst_sym(280)        => __E___E_cli_command_125_126(281).
	p(NT(280), (NT(281)));
//G441: __E_cli_command_125(279) => inst_sym(280) __(34) inst_args(282).
	p(NT(279), (NT(280)+NT(34)+NT(282)));
//G442: inst_cmd(278)        => __E_cli_command_125(279).
	p(NT(278), (NT(279)));
//G443: cli_command(222)     => inst_cmd(278).
	p(NT(222), (NT(278)));
//G444: dnf_sym(285)         => 'd' 'n' 'f'.
	p(NT(285), (T(54)+T(5)+T(9)));
//G445: __E_cli_command_127(284) => dnf_sym(285) __(34) nf_cmd_arg(277).
	p(NT(284), (NT(285)+NT(34)+NT(277)));
//G446: dnf_cmd(283)         => __E_cli_command_127(284).
	p(NT(283), (NT(284)));
//G447: cli_command(222)     => dnf_cmd(283).
	p(NT(222), (NT(283)));
//G448: cnf_sym(288)         => 'c' 'n' 'f'.
	p(NT(288), (T(14)+T(5)+T(9)));
//G449: __E_cli_command_128(287) => cnf_sym(288) __(34) nf_cmd_arg(277).
	p(NT(287), (NT(288)+NT(34)+NT(277)));
//G450: cnf_cmd(286)         => __E_cli_command_128(287).
	p(NT(286), (NT(287)));
//G451: cli_command(222)     => cnf_cmd(286).
	p(NT(222), (NT(286)));
//G452: anf_sym(291)         => 'a' 'n' 'f'.
	p(NT(291), (T(16)+T(5)+T(9)));
//G453: __E_cli_command_129(290) => anf_sym(291) __(34) nf_cmd_arg(277).
	p(NT(290), (NT(291)+NT(34)+NT(277)));
//G454: anf_cmd(289)         => __E_cli_command_129(290).
	p(NT(289), (NT(290)));
//G455: cli_command(222)     => anf_cmd(289).
	p(NT(222), (NT(289)));
//G456: nnf_sym(294)         => 'n' 'n' 'f'.
	p(NT(294), (T(5)+T(5)+T(9)));
//G457: __E_cli_command_130(293) => nnf_sym(294) __(34) nf_cmd_arg(277).
	p(NT(293), (NT(294)+NT(34)+NT(277)));
//G458: nnf_cmd(292)         => __E_cli_command_130(293).
	p(NT(292), (NT(293)));
//G459: cli_command(222)     => nnf_cmd(292).
	p(NT(222), (NT(292)));
//G460: pnf_sym(297)         => 'p' 'n' 'f'.
	p(NT(297), (T(53)+T(5)+T(9)));
//G461: __E_cli_command_131(296) => pnf_sym(297) __(34) nf_cmd_arg(277).
	p(NT(296), (NT(297)+NT(34)+NT(277)));
//G462: pnf_cmd(295)         => __E_cli_command_131(296).
	p(NT(295), (NT(296)));
//G463: cli_command(222)     => pnf_cmd(295).
	p(NT(222), (NT(295)));
//G464: mnf_sym(300)         => 'm' 'n' 'f'.
	p(NT(300), (T(21)+T(5)+T(9)));
//G465: __E_cli_command_132(299) => mnf_sym(300) __(34) nf_cmd_arg(277).
	p(NT(299), (NT(300)+NT(34)+NT(277)));
//G466: mnf_cmd(298)         => __E_cli_command_132(299).
	p(NT(298), (NT(299)));
//G467: cli_command(222)     => mnf_cmd(298).
	p(NT(222), (NT(298)));
//G468: snf_sym(303)         => 's' 'n' 'f'.
	p(NT(303), (T(15)+T(5)+T(9)));
//G469: __E_cli_command_133(302) => snf_sym(303) __(34) nf_cmd_arg(277).
	p(NT(302), (NT(303)+NT(34)+NT(277)));
//G470: snf_cmd(301)         => __E_cli_command_133(302).
	p(NT(301), (NT(302)));
//G471: cli_command(222)     => snf_cmd(301).
	p(NT(222), (NT(301)));
//G472: onf_sym(306)         => 'o' 'n' 'f'.
	p(NT(306), (T(6)+T(5)+T(9)));
//G473: __E_cli_command_134(305) => onf_sym(306) __(34) variable(135) __(34) onf_cmd_arg(307).
	p(NT(305), (NT(306)+NT(34)+NT(135)+NT(34)+NT(307)));
//G474: onf_cmd(304)         => __E_cli_command_134(305).
	p(NT(304), (NT(305)));
//G475: cli_command(222)     => onf_cmd(304).
	p(NT(222), (NT(304)));
//G476: qelim_sym(310)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(310), (T(50)+T(11)+T(10)+T(4)+T(21)));
//G477: __E_cli_command_135(309) => qelim_sym(310) __(34) wff_cmd_arg(259).
	p(NT(309), (NT(310)+NT(34)+NT(259)));
//G478: qelim_cmd(308)       => __E_cli_command_135(309).
	p(NT(308), (NT(309)));
//G479: cli_command(222)     => qelim_cmd(308).
	p(NT(222), (NT(308)));
//G480: get_sym(313)         => 'g' 'e' 't'.
	p(NT(313), (T(55)+T(11)+T(8)));
//G481: __E___E_cli_command_136_137(314) => __(34) option_name(315).
	p(NT(314), (NT(34)+NT(315)));
//G482: __E___E_cli_command_136_137(314) => null.
	p(NT(314), (nul));
//G483: __E_cli_command_136(312) => get_sym(313) __E___E_cli_command_136_137(314).
	p(NT(312), (NT(313)+NT(314)));
//G484: get_cmd(311)         => __E_cli_command_136(312).
	p(NT(311), (NT(312)));
//G485: cli_command(222)     => get_cmd(311).
	p(NT(222), (NT(311)));
//G486: set_sym(318)         => 's' 'e' 't'.
	p(NT(318), (T(15)+T(11)+T(8)));
//G487: __E___E_cli_command_138_139(319) => __(34).
	p(NT(319), (NT(34)));
//G488: __E___E_cli_command_138_139(319) => _(11) '=' _(11).
	p(NT(319), (NT(11)+T(3)+NT(11)));
//G489: __E_cli_command_138(317) => set_sym(318) __(34) option_name(315) __E___E_cli_command_138_139(319) option_value(320).
	p(NT(317), (NT(318)+NT(34)+NT(315)+NT(319)+NT(320)));
//G490: set_cmd(316)         => __E_cli_command_138(317).
	p(NT(316), (NT(317)));
//G491: cli_command(222)     => set_cmd(316).
	p(NT(222), (NT(316)));
//G492: enable_sym(323)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(323), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G493: __E_cli_command_140(322) => enable_sym(323) __(34) option_name(315).
	p(NT(322), (NT(323)+NT(34)+NT(315)));
//G494: enable_cmd(321)      => __E_cli_command_140(322).
	p(NT(321), (NT(322)));
//G495: cli_command(222)     => enable_cmd(321).
	p(NT(222), (NT(321)));
//G496: disable_sym(326)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(326), (T(54)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G497: __E_cli_command_141(325) => disable_sym(326) __(34) option_name(315).
	p(NT(325), (NT(326)+NT(34)+NT(315)));
//G498: disable_cmd(324)     => __E_cli_command_141(325).
	p(NT(324), (NT(325)));
//G499: cli_command(222)     => disable_cmd(324).
	p(NT(222), (NT(324)));
//G500: toggle_sym(329)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(329), (T(8)+T(6)+T(55)+T(55)+T(10)+T(11)));
//G501: __E_cli_command_142(328) => toggle_sym(329) __(34) option_name(315).
	p(NT(328), (NT(329)+NT(34)+NT(315)));
//G502: toggle_cmd(327)      => __E_cli_command_142(328).
	p(NT(327), (NT(328)));
//G503: cli_command(222)     => toggle_cmd(327).
	p(NT(222), (NT(327)));
//G504: __E___E_cli_command_143_144(333) => 'd' 'e' 'f' 's'.
	p(NT(333), (T(54)+T(11)+T(9)+T(15)));
//G505: __E___E_cli_command_143_144(333) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(333), (T(54)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G506: def_sym(332)         => __E___E_cli_command_143_144(333).
	p(NT(332), (NT(333)));
//G507: __E_cli_command_143(331) => def_sym(332).
	p(NT(331), (NT(332)));
//G508: def_list_cmd(330)    => __E_cli_command_143(331).
	p(NT(330), (NT(331)));
//G509: cli_command(222)     => def_list_cmd(330).
	p(NT(222), (NT(330)));
//G510: __E_cli_command_145(335) => def_sym(332) __(34) num(165).
	p(NT(335), (NT(332)+NT(34)+NT(165)));
//G511: def_print_cmd(334)   => __E_cli_command_145(335).
	p(NT(334), (NT(335)));
//G512: cli_command(222)     => def_print_cmd(334).
	p(NT(222), (NT(334)));
//G513: def_rr_cmd(336)      => rec_relation(17).
	p(NT(336), (NT(17)));
//G514: cli_command(222)     => def_rr_cmd(336).
	p(NT(222), (NT(336)));
//G515: def_input_cmd(337)   => input_def(18).
	p(NT(337), (NT(18)));
//G516: cli_command(222)     => def_input_cmd(337).
	p(NT(222), (NT(337)));
//G517: def_output_cmd(338)  => output_def(19).
	p(NT(338), (NT(19)));
//G518: cli_command(222)     => def_output_cmd(338).
	p(NT(222), (NT(338)));
//G519: __E___E_cli_command_146_147(342) => 'h' 'i' 's' 't'.
	p(NT(342), (T(52)+T(4)+T(15)+T(8)));
//G520: __E___E_cli_command_146_147(342) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(342), (T(52)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G521: history_sym(341)     => __E___E_cli_command_146_147(342).
	p(NT(341), (NT(342)));
//G522: __E_cli_command_146(340) => history_sym(341).
	p(NT(340), (NT(341)));
//G523: history_list_cmd(339) => __E_cli_command_146(340).
	p(NT(339), (NT(340)));
//G524: cli_command(222)     => history_list_cmd(339).
	p(NT(222), (NT(339)));
//G525: __E_cli_command_148(344) => history_sym(341) __(34) history(268).
	p(NT(344), (NT(341)+NT(34)+NT(268)));
//G526: history_print_cmd(343) => __E_cli_command_148(344).
	p(NT(343), (NT(344)));
//G527: cli_command(222)     => history_print_cmd(343).
	p(NT(222), (NT(343)));
//G528: __E_cli_command_149(346) => wff(14).
	p(NT(346), (NT(14)));
//G529: __E_cli_command_149(346) => bf(30).
	p(NT(346), (NT(30)));
//G530: history_store_cmd(345) => __E_cli_command_149(346).
	p(NT(345), (NT(346)));
//G531: cli_command(222)     => history_store_cmd(345).
	p(NT(222), (NT(345)));
//G532: __E_solve_options_150(347) => __(34) solve_option(348).
	p(NT(347), (NT(34)+NT(348)));
//G533: __E_solve_options_151(349) => null.
	p(NT(349), (nul));
//G534: __E_solve_options_151(349) => __E_solve_options_150(347) __E_solve_options_151(349).
	p(NT(349), (NT(347)+NT(349)));
//G535: solve_options(258)   => __E_solve_options_151(349).
	p(NT(258), (NT(349)));
//G536: __E_solve_option_152(350) => solver_mode(351).
	p(NT(350), (NT(351)));
//G537: __E_solve_option_152(350) => type(215).
	p(NT(350), (NT(215)));
//G538: solve_option(348)    => '-' '-' __E_solve_option_152(350).
	p(NT(348), (T(30)+T(30)+NT(350)));
//G539: __E_solver_mode_153(353) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(353), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G540: __E_solver_mode_153(353) => 'm' 'i' 'n'.
	p(NT(353), (T(21)+T(4)+T(5)));
//G541: solver_mode_minimum(352) => __E_solver_mode_153(353).
	p(NT(352), (NT(353)));
//G542: solver_mode(351)     => solver_mode_minimum(352).
	p(NT(351), (NT(352)));
//G543: __E_solver_mode_154(355) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(355), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G544: __E_solver_mode_154(355) => 'm' 'a' 'x'.
	p(NT(355), (T(21)+T(16)+T(29)));
//G545: solver_mode_maximum(354) => __E_solver_mode_154(355).
	p(NT(354), (NT(355)));
//G546: solver_mode(351)     => solver_mode_maximum(354).
	p(NT(351), (NT(354)));
//G547: q_file_name(38)      => '"' file_name(356) '"'.
	p(NT(38), (T(58)+NT(356)+T(58)));
//G548: __E_file_name_155(357) => printable(8).
	p(NT(357), (NT(8)));
//G549: __E_file_name_155(357) => printable(8) __E_file_name_155(357).
	p(NT(357), (NT(8)+NT(357)));
//G550: file_name(356)       => __E_file_name_155(357).
	p(NT(356), (NT(357)));
//G551: __E_option_name_156(358) => alnum(6).
	p(NT(358), (NT(6)));
//G552: __E_option_name_156(358) => alnum(6) __E_option_name_156(358).
	p(NT(358), (NT(6)+NT(358)));
//G553: option_name(315)     => __E_option_name_156(358).
	p(NT(315), (NT(358)));
//G554: __E_option_value_157(359) => alnum(6).
	p(NT(359), (NT(6)));
//G555: __E_option_value_157(359) => alnum(6) __E_option_value_157(359).
	p(NT(359), (NT(6)+NT(359)));
//G556: option_value(320)    => __E_option_value_157(359).
	p(NT(320), (NT(359)));
//G557: bf_cmd_arg(360)      => history(268).
	p(NT(360), (NT(268)));
//G558: bf_cmd_arg(360)      => bf(30).
	p(NT(360), (NT(30)));
//G559: wff_cmd_arg(259)     => history(268).
	p(NT(259), (NT(268)));
//G560: wff_cmd_arg(259)     => wff(14).
	p(NT(259), (NT(14)));
//G561: nf_cmd_arg(277)      => history(268).
	p(NT(277), (NT(268)));
//G562: nf_cmd_arg(277)      => ref(27).
	p(NT(277), (NT(27)));
//G563: nf_cmd_arg(277)      => wff(14).
	p(NT(277), (NT(14)));
//G564: nf_cmd_arg(277)      => bf(30).
	p(NT(277), (NT(30)));
//G565: onf_cmd_arg(307)     => history(268).
	p(NT(307), (NT(268)));
//G566: onf_cmd_arg(307)     => wff(14).
	p(NT(307), (NT(14)));
//G567: normalize_cmd_arg(247) => history(268).
	p(NT(247), (NT(268)));
//G568: normalize_cmd_arg(247) => spec(10).
	p(NT(247), (NT(10)));
//G569: normalize_cmd_arg(247) => ref(27).
	p(NT(247), (NT(27)));
//G570: normalize_cmd_arg(247) => wff(14).
	p(NT(247), (NT(14)));
//G571: normalize_cmd_arg(247) => bf(30).
	p(NT(247), (NT(30)));
//G572: inst_args(282)       => wff_cmd_arg(259) _(11) '[' _(11) variable(135) _(11) '/' _(11) bf_cmd_arg(360) _(11) ']'.
	p(NT(282), (NT(259)+NT(11)+T(26)+NT(11)+NT(135)+NT(11)+T(57)+NT(11)+NT(360)+NT(11)+T(27)));
//G573: inst_args(282)       => bf_cmd_arg(360) _(11) '[' _(11) variable(135) _(11) '/' _(11) bf_cmd_arg(360) _(11) ']'.
	p(NT(282), (NT(360)+NT(11)+T(26)+NT(11)+NT(135)+NT(11)+T(57)+NT(11)+NT(360)+NT(11)+T(27)));
//G574: help_arg(239)        => help_sym(236).
	p(NT(239), (NT(236)));
//G575: help_arg(239)        => version_sym(229).
	p(NT(239), (NT(229)));
//G576: help_arg(239)        => quit_sym(226).
	p(NT(239), (NT(226)));
//G577: help_arg(239)        => clear_sym(232).
	p(NT(239), (NT(232)));
//G578: help_arg(239)        => get_sym(313).
	p(NT(239), (NT(313)));
//G579: help_arg(239)        => set_sym(318).
	p(NT(239), (NT(318)));
//G580: help_arg(239)        => enable_sym(323).
	p(NT(239), (NT(323)));
//G581: help_arg(239)        => disable_sym(326).
	p(NT(239), (NT(326)));
//G582: help_arg(239)        => toggle_sym(329).
	p(NT(239), (NT(329)));
//G583: help_arg(239)        => file_sym(242).
	p(NT(239), (NT(242)));
//G584: help_arg(239)        => history_sym(341).
	p(NT(239), (NT(341)));
//G585: help_arg(239)        => abs_history_sym(361).
	p(NT(239), (NT(361)));
//G586: help_arg(239)        => rel_history_sym(362).
	p(NT(239), (NT(362)));
//G587: help_arg(239)        => selection_sym(363).
	p(NT(239), (NT(363)));
//G588: help_arg(239)        => def_sym(332).
	p(NT(239), (NT(332)));
//G589: help_arg(239)        => inst_sym(280).
	p(NT(239), (NT(280)));
//G590: help_arg(239)        => subst_sym(275).
	p(NT(239), (NT(275)));
//G591: help_arg(239)        => normalize_sym(271).
	p(NT(239), (NT(271)));
//G592: help_arg(239)        => execute_sym(364).
	p(NT(239), (NT(364)));
//G593: help_arg(239)        => solve_sym(256).
	p(NT(239), (NT(256)));
//G594: help_arg(239)        => valid_sym(246).
	p(NT(239), (NT(246)));
//G595: help_arg(239)        => sat_sym(250).
	p(NT(239), (NT(250)));
//G596: help_arg(239)        => unsat_sym(253).
	p(NT(239), (NT(253)));
//G597: help_arg(239)        => run_sym(265).
	p(NT(239), (NT(265)));
//G598: help_arg(239)        => dnf_sym(285).
	p(NT(239), (NT(285)));
//G599: help_arg(239)        => cnf_sym(288).
	p(NT(239), (NT(288)));
//G600: help_arg(239)        => anf_sym(291).
	p(NT(239), (NT(291)));
//G601: help_arg(239)        => snf_sym(303).
	p(NT(239), (NT(303)));
//G602: help_arg(239)        => nnf_sym(294).
	p(NT(239), (NT(294)));
//G603: help_arg(239)        => pnf_sym(297).
	p(NT(239), (NT(297)));
//G604: help_arg(239)        => mnf_sym(300).
	p(NT(239), (NT(300)));
//G605: help_arg(239)        => onf_sym(306).
	p(NT(239), (NT(306)));
//G606: help_arg(239)        => qelim_sym(310).
	p(NT(239), (NT(310)));
//G607: __E___E_help_arg_158_159(367) => 's'.
	p(NT(367), (T(15)));
//G608: __E___E_help_arg_158_159(367) => null.
	p(NT(367), (nul));
//G609: __E_help_arg_158(366) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_158_159(367).
	p(NT(366), (T(11)+T(29)+T(16)+T(21)+T(53)+T(10)+T(11)+NT(367)));
//G610: examples_sym(365)    => __E_help_arg_158(366).
	p(NT(365), (NT(366)));
//G611: help_arg(239)        => examples_sym(365).
	p(NT(239), (NT(365)));
//G612: __E_history_160(369) => '%'.
	p(NT(369), (T(59)));
//G613: rel_history_sym(362) => '%' '-'.
	p(NT(362), (T(59)+T(30)));
//G614: history_id(371)      => digits(212).
	p(NT(371), (NT(212)));
//G615: __E___E_history_160_161(370) => history_id(371).
	p(NT(370), (NT(371)));
//G616: __E___E_history_160_161(370) => null.
	p(NT(370), (nul));
//G617: __E_history_160(369) => rel_history_sym(362) __E___E_history_160_161(370).
	p(NT(369), (NT(362)+NT(370)));
//G618: rel_history(368)     => __E_history_160(369).
	p(NT(368), (NT(369)));
//G619: history(268)         => rel_history(368).
	p(NT(268), (NT(368)));
//G620: abs_history_sym(361) => '%'.
	p(NT(361), (T(59)));
//G621: __E_history_162(373) => abs_history_sym(361) history_id(371).
	p(NT(373), (NT(361)+NT(371)));
//G622: abs_history(372)     => __E_history_162(373).
	p(NT(372), (NT(373)));
//G623: history(268)         => abs_history(372).
	p(NT(268), (NT(372)));
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
		bf_nlt, __E_wff_51, bf_parenthesis, __E_bf_52, bf_constant, variable, bf_splitter, __E_bf_53, bf_ref, bf_or, 
		__E_bf_54, bf_xor, __E_bf_55, bf_t, __E_bf_56, __E___E_bf_56_57, bf_f, __E_bf_58, __E___E_bf_58_59, bf_and, 
		__E_bf_60, __E___E_bf_60_61, bf_neg, __E_bf_62, bf_neg_oprnd, __E___E_bf_62_63, bf_and_nosep, __E_bf_64, bf_and_nosep_1st_oprnd, __E___E_bf_64_65, 
		bf_and_nosep_2nd_oprnd, __E___E_bf_64_66, ctn_neq, __E_constraint_67, ctnvar, num, ctn_eq, __E_constraint_68, ctn_gteq, __E_constraint_69, 
		ctn_gt, __E_constraint_70, ctn_lteq, __E_constraint_71, ctn_lt, __E_constraint_72, __E_bf_constant_73, source, __E_bf_constant_74, src_c, 
		__E_source_75, __E___E_source_75_76, __E___E_source_75_77, __E_src_c_78, __E_src_c_79, __E_variable_80, uconst, io_var, var_name, __E_variable_81, 
		__E_var_name_82, __E_var_name_83, __E_var_name_84, __E_var_name_85, __E_var_name_86, offset, chars, uconst_name, __E_uconst_87, __E___E_uconst_87_88, 
		q_var, __E_q_vars_89, __E_q_vars_90, __E_offsets_91, __E_offsets_92, integer, shift, __E_offset_93, __E_shift_94, __E___E_shift_94_95, 
		__E_chars_96, __E_chars_97, digits, __E_digits_98, __E_integer_99, type, scope_id, comment, __E_comment_100, __E_comment_101, 
		__E_comment_102, cli, cli_command, __E_cli_103, __E_cli_104, quit_cmd, quit_sym, __E_cli_command_105, version_cmd, version_sym, 
		__E_cli_command_106, clear_cmd, clear_sym, __E_cli_command_107, help_cmd, __E_cli_command_108, help_sym, __E___E_cli_command_108_109, __E___E_cli_command_108_110, help_arg, 
		file_cmd, __E_cli_command_111, file_sym, q_string, valid_cmd, __E_cli_command_112, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_113, 
		sat_sym, unsat_cmd, __E_cli_command_114, unsat_sym, solve_cmd, __E_cli_command_115, solve_sym, __E___E_cli_command_115_116, solve_options, wff_cmd_arg, 
		lgrs_cmd, __E_cli_command_117, lgrs_sym, run_cmd, __E_cli_command_118, run_sym, __E___E_cli_command_118_119, __E___E_cli_command_118_120, history, normalize_cmd, 
		__E_cli_command_121, normalize_sym, __E___E_cli_command_121_122, subst_cmd, __E_cli_command_123, subst_sym, __E___E_cli_command_123_124, nf_cmd_arg, inst_cmd, __E_cli_command_125, 
		inst_sym, __E___E_cli_command_125_126, inst_args, dnf_cmd, __E_cli_command_127, dnf_sym, cnf_cmd, __E_cli_command_128, cnf_sym, anf_cmd, 
		__E_cli_command_129, anf_sym, nnf_cmd, __E_cli_command_130, nnf_sym, pnf_cmd, __E_cli_command_131, pnf_sym, mnf_cmd, __E_cli_command_132, 
		mnf_sym, snf_cmd, __E_cli_command_133, snf_sym, onf_cmd, __E_cli_command_134, onf_sym, onf_cmd_arg, qelim_cmd, __E_cli_command_135, 
		qelim_sym, get_cmd, __E_cli_command_136, get_sym, __E___E_cli_command_136_137, option_name, set_cmd, __E_cli_command_138, set_sym, __E___E_cli_command_138_139, 
		option_value, enable_cmd, __E_cli_command_140, enable_sym, disable_cmd, __E_cli_command_141, disable_sym, toggle_cmd, __E_cli_command_142, toggle_sym, 
		def_list_cmd, __E_cli_command_143, def_sym, __E___E_cli_command_143_144, def_print_cmd, __E_cli_command_145, def_rr_cmd, def_input_cmd, def_output_cmd, history_list_cmd, 
		__E_cli_command_146, history_sym, __E___E_cli_command_146_147, history_print_cmd, __E_cli_command_148, history_store_cmd, __E_cli_command_149, __E_solve_options_150, solve_option, __E_solve_options_151, 
		__E_solve_option_152, solver_mode, solver_mode_minimum, __E_solver_mode_153, solver_mode_maximum, __E_solver_mode_154, file_name, __E_file_name_155, __E_option_name_156, __E_option_value_157, 
		bf_cmd_arg, abs_history_sym, rel_history_sym, selection_sym, execute_sym, examples_sym, __E_help_arg_158, __E___E_help_arg_158_159, rel_history, __E_history_160, 
		__E___E_history_160_161, history_id, abs_history, __E_history_162, __N_0, __N_1, __N_2, __N_3, __N_4, __N_5, 
		__N_6, __N_7, __N_8, 
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
