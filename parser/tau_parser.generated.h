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
	"bf_nlt", "__E_wff_51", "bf_parenthesis", "__E_bf_52", "bf_constant", "variable", "bf_splitter", "__E_bf_53", "bf_fall", "__E_bf_54", 
	"bf_fex", "__E_bf_55", "bf_ref", "bf_or", "__E_bf_56", "bf_xor", "__E_bf_57", "bf_t", "__E_bf_58", "__E___E_bf_58_59", 
	"bf_f", "__E_bf_60", "__E___E_bf_60_61", "bf_and", "__E_bf_62", "__E___E_bf_62_63", "bf_neg", "__E_bf_64", "bf_neg_oprnd", "__E___E_bf_64_65", 
	"bf_and_nosep", "__E_bf_66", "bf_and_nosep_1st_oprnd", "__E___E_bf_66_67", "bf_and_nosep_2nd_oprnd", "__E___E_bf_66_68", "ctn_neq", "__E_constraint_69", "ctnvar", "num", 
	"ctn_eq", "__E_constraint_70", "ctn_gteq", "__E_constraint_71", "ctn_gt", "__E_constraint_72", "ctn_lteq", "__E_constraint_73", "ctn_lt", "__E_constraint_74", 
	"__E_bf_constant_75", "source", "__E_bf_constant_76", "src_c", "__E_source_77", "__E___E_source_77_78", "__E___E_source_77_79", "__E_src_c_80", "__E_src_c_81", "__E_variable_82", 
	"uconst", "io_var", "var_name", "__E_variable_83", "__E_var_name_84", "__E_var_name_85", "__E_var_name_86", "__E_var_name_87", "__E_var_name_88", "offset", 
	"chars", "uconst_name", "__E_uconst_89", "__E___E_uconst_89_90", "q_var", "__E_q_vars_91", "__E_q_vars_92", "__E_offsets_93", "__E_offsets_94", "integer", 
	"shift", "__E_offset_95", "__E_shift_96", "__E___E_shift_96_97", "__E_chars_98", "__E_chars_99", "digits", "__E_digits_100", "__E_integer_101", "type", 
	"scope_id", "comment", "__E_comment_102", "__E_comment_103", "__E_comment_104", "cli", "cli_command", "__E_cli_105", "__E_cli_106", "quit_cmd", 
	"quit_sym", "__E_cli_command_107", "version_cmd", "version_sym", "__E_cli_command_108", "clear_cmd", "clear_sym", "__E_cli_command_109", "help_cmd", "__E_cli_command_110", 
	"help_sym", "__E___E_cli_command_110_111", "__E___E_cli_command_110_112", "help_arg", "file_cmd", "__E_cli_command_113", "file_sym", "q_string", "valid_cmd", "__E_cli_command_114", 
	"valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_115", "sat_sym", "unsat_cmd", "__E_cli_command_116", "unsat_sym", "solve_cmd", "__E_cli_command_117", 
	"solve_sym", "__E___E_cli_command_117_118", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_119", "lgrs_sym", "run_cmd", "__E_cli_command_120", "run_sym", 
	"__E___E_cli_command_120_121", "__E___E_cli_command_120_122", "history", "normalize_cmd", "__E_cli_command_123", "normalize_sym", "__E___E_cli_command_123_124", "subst_cmd", "__E_cli_command_125", "subst_sym", 
	"__E___E_cli_command_125_126", "nf_cmd_arg", "inst_cmd", "__E_cli_command_127", "inst_sym", "__E___E_cli_command_127_128", "inst_args", "dnf_cmd", "__E_cli_command_129", "dnf_sym", 
	"cnf_cmd", "__E_cli_command_130", "cnf_sym", "anf_cmd", "__E_cli_command_131", "anf_sym", "nnf_cmd", "__E_cli_command_132", "nnf_sym", "pnf_cmd", 
	"__E_cli_command_133", "pnf_sym", "mnf_cmd", "__E_cli_command_134", "mnf_sym", "snf_cmd", "__E_cli_command_135", "snf_sym", "onf_cmd", "__E_cli_command_136", 
	"onf_sym", "onf_cmd_arg", "qelim_cmd", "__E_cli_command_137", "qelim_sym", "get_cmd", "__E_cli_command_138", "get_sym", "__E___E_cli_command_138_139", "option_name", 
	"set_cmd", "__E_cli_command_140", "set_sym", "__E___E_cli_command_140_141", "option_value", "enable_cmd", "__E_cli_command_142", "enable_sym", "disable_cmd", "__E_cli_command_143", 
	"disable_sym", "toggle_cmd", "__E_cli_command_144", "toggle_sym", "def_list_cmd", "__E_cli_command_145", "def_sym", "__E___E_cli_command_145_146", "def_print_cmd", "__E_cli_command_147", 
	"def_rr_cmd", "def_input_cmd", "def_output_cmd", "history_list_cmd", "__E_cli_command_148", "history_sym", "__E___E_cli_command_148_149", "history_print_cmd", "__E_cli_command_150", "history_store_cmd", 
	"__E_cli_command_151", "__E_solve_options_152", "solve_option", "__E_solve_options_153", "__E_solve_option_154", "solver_mode", "solver_mode_minimum", "__E_solver_mode_155", "solver_mode_maximum", "__E_solver_mode_156", 
	"file_name", "__E_file_name_157", "__E_option_name_158", "__E_option_value_159", "bf_cmd_arg", "abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", "examples_sym", 
	"__E_help_arg_160", "__E___E_help_arg_160_161", "rel_history", "__E_history_162", "__E___E_history_162_163", "history_id", "abs_history", "__E_history_164", "__N_0", "__N_1", 
	"__N_2", "__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
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
			29, 31, 40, 168, 181, 192, 201, 209, 216, 219,
			319, 324, 360
		},
		.to_inline = {
			{ 14, 77, 14 },
			{ 30, 132, 30 },
			{ 33 },
			{ 158, 132, 30 },
			{ 160, 132, 153 },
			{ 162, 132, 30 },
			{ 164, 132, 30 },
			{ 183 },
			{ 190 },
			{ 190, 201 },
			{ 200 },
			{ 204 },
			{ 251 },
			{ 263 },
			{ 281 },
			{ 286 },
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
//G180: __E_bf_54(139)       => 'f' 'a' 'l' 'l' __(34) q_vars(89) __(34) bf(30).
	p(NT(139), (T(9)+T(16)+T(10)+T(10)+NT(34)+NT(89)+NT(34)+NT(30)));
//G181: bf_fall(138)         => __E_bf_54(139).
	p(NT(138), (NT(139)));
//G182: bf(30)               => bf_fall(138).
	p(NT(30), (NT(138)));
//G183: __E_bf_55(141)       => 'f' 'e' 'x' __(34) q_vars(89) __(34) bf(30).
	p(NT(141), (T(9)+T(11)+T(29)+NT(34)+NT(89)+NT(34)+NT(30)));
//G184: bf_fex(140)          => __E_bf_55(141).
	p(NT(140), (NT(141)));
//G185: bf(30)               => bf_fex(140).
	p(NT(30), (NT(140)));
//G186: bf_ref(142)          => ref(27).
	p(NT(142), (NT(27)));
//G187: bf(30)               => bf_ref(142).
	p(NT(30), (NT(142)));
//G188: __E_bf_56(144)       => bf(30) _(11) '|' _(11) bf(30).
	p(NT(144), (NT(30)+NT(11)+T(31)+NT(11)+NT(30)));
//G189: bf_or(143)           => __E_bf_56(144).
	p(NT(143), (NT(144)));
//G190: bf(30)               => bf_or(143).
	p(NT(30), (NT(143)));
//G191: __E_bf_57(146)       => bf(30) _(11) '+' _(11) bf(30).
	p(NT(146), (NT(30)+NT(11)+T(38)+NT(11)+NT(30)));
//G192: bf_xor(145)          => __E_bf_57(146).
	p(NT(145), (NT(146)));
//G193: bf(30)               => bf_xor(145).
	p(NT(30), (NT(145)));
//G194: __E___E_bf_58_59(149) => typed(33).
	p(NT(149), (NT(33)));
//G195: __E___E_bf_58_59(149) => null.
	p(NT(149), (nul));
//G196: __E_bf_58(148)       => '1' __E___E_bf_58_59(149).
	p(NT(148), (T(39)+NT(149)));
//G197: bf_t(147)            => __E_bf_58(148).
	p(NT(147), (NT(148)));
//G198: bf(30)               => bf_t(147).
	p(NT(30), (NT(147)));
//G199: __E___E_bf_60_61(152) => typed(33).
	p(NT(152), (NT(33)));
//G200: __E___E_bf_60_61(152) => null.
	p(NT(152), (nul));
//G201: __E_bf_60(151)       => '0' __E___E_bf_60_61(152).
	p(NT(151), (T(40)+NT(152)));
//G202: bf_f(150)            => __E_bf_60(151).
	p(NT(150), (NT(151)));
//G203: bf(30)               => bf_f(150).
	p(NT(30), (NT(150)));
//G204: __E___E_bf_62_63(155) => _(11) '&' _(11).
	p(NT(155), (NT(11)+T(33)+NT(11)));
//G205: __E___E_bf_62_63(155) => __(34).
	p(NT(155), (NT(34)));
//G206: __E_bf_62(154)       => bf(30) __E___E_bf_62_63(155) bf(30).
	p(NT(154), (NT(30)+NT(155)+NT(30)));
//G207: bf_and(153)          => __E_bf_62(154).
	p(NT(153), (NT(154)));
//G208: bf(30)               => bf_and(153).
	p(NT(30), (NT(153)));
//G209: __E___E_bf_64_65(159) => bf_parenthesis(132).
	p(NT(159), (NT(132)));
//G210: __E___E_bf_64_65(159) => bf_constant(134).
	p(NT(159), (NT(134)));
//G211: __E___E_bf_64_65(159) => variable(135).
	p(NT(159), (NT(135)));
//G212: __E___E_bf_64_65(159) => bf_splitter(136).
	p(NT(159), (NT(136)));
//G213: __E___E_bf_64_65(159) => bf_ref(142).
	p(NT(159), (NT(142)));
//G214: __E___E_bf_64_65(159) => bf_t(147).
	p(NT(159), (NT(147)));
//G215: __E___E_bf_64_65(159) => bf_f(150).
	p(NT(159), (NT(150)));
//G216: __E___E_bf_64_65(159) => bf_neg(156).
	p(NT(159), (NT(156)));
//G217: __E___E_bf_64_65(159) => capture(29).
	p(NT(159), (NT(29)));
//G218: bf_neg_oprnd(158)    => __E___E_bf_64_65(159).
	p(NT(158), (NT(159)));
//G219: __E_bf_64(157)       => bf_neg_oprnd(158) _(11) '\''.
	p(NT(157), (NT(158)+NT(11)+T(41)));
//G220: bf_neg(156)          => __E_bf_64(157).
	p(NT(156), (NT(157)));
//G221: bf(30)               => bf_neg(156).
	p(NT(30), (NT(156)));
//G222: __E___E_bf_66_67(163) => bf_parenthesis(132).
	p(NT(163), (NT(132)));
//G223: __E___E_bf_66_67(163) => variable(135).
	p(NT(163), (NT(135)));
//G224: __E___E_bf_66_67(163) => bf_splitter(136).
	p(NT(163), (NT(136)));
//G225: __E___E_bf_66_67(163) => bf_ref(142).
	p(NT(163), (NT(142)));
//G226: __E___E_bf_66_67(163) => bf_neg(156).
	p(NT(163), (NT(156)));
//G227: __E___E_bf_66_67(163) => bf_and_nosep(160).
	p(NT(163), (NT(160)));
//G228: __E___E_bf_66_67(163) => capture(29).
	p(NT(163), (NT(29)));
//G229: bf_and_nosep_1st_oprnd(162) => __E___E_bf_66_67(163).
	p(NT(162), (NT(163)));
//G230: __E___E_bf_66_68(165) => bf_parenthesis(132).
	p(NT(165), (NT(132)));
//G231: __E___E_bf_66_68(165) => bf_constant(134).
	p(NT(165), (NT(134)));
//G232: __E___E_bf_66_68(165) => variable(135).
	p(NT(165), (NT(135)));
//G233: __E___E_bf_66_68(165) => bf_splitter(136).
	p(NT(165), (NT(136)));
//G234: __E___E_bf_66_68(165) => bf_ref(142).
	p(NT(165), (NT(142)));
//G235: __E___E_bf_66_68(165) => bf_neg(156).
	p(NT(165), (NT(156)));
//G236: __E___E_bf_66_68(165) => capture(29).
	p(NT(165), (NT(29)));
//G237: bf_and_nosep_2nd_oprnd(164) => __E___E_bf_66_68(165).
	p(NT(164), (NT(165)));
//G238: __E_bf_66(161)       => bf_and_nosep_1st_oprnd(162) bf_and_nosep_2nd_oprnd(164).
	p(NT(161), (NT(162)+NT(164)));
//G239: bf_and_nosep(160)    => __E_bf_66(161).
	p(NT(160), (NT(161)));
//G240: bf(30)               => bf_and_nosep(160).
	p(NT(30), (NT(160)));
//G241: bf(30)               => capture(29).
	p(NT(30), (NT(29)));
//G242: __E_constraint_69(167) => '[' ctnvar(168) _(11) '!' '=' _(11) num(169) ']'.
	p(NT(167), (T(26)+NT(168)+NT(11)+T(34)+T(3)+NT(11)+NT(169)+T(27)));
//G243: __E_constraint_69(167) => '[' num(169) _(11) '!' '=' _(11) ctnvar(168) ']'.
	p(NT(167), (T(26)+NT(169)+NT(11)+T(34)+T(3)+NT(11)+NT(168)+T(27)));
//G244: ctn_neq(166)         => __E_constraint_69(167).
	p(NT(166), (NT(167)));
//G245: constraint(93)       => ctn_neq(166).
	p(NT(93), (NT(166)));
//G246: __E_constraint_70(171) => '[' ctnvar(168) _(11) '=' _(11) num(169) ']'.
	p(NT(171), (T(26)+NT(168)+NT(11)+T(3)+NT(11)+NT(169)+T(27)));
//G247: __E_constraint_70(171) => '[' num(169) _(11) '=' _(11) ctnvar(168) ']'.
	p(NT(171), (T(26)+NT(169)+NT(11)+T(3)+NT(11)+NT(168)+T(27)));
//G248: ctn_eq(170)          => __E_constraint_70(171).
	p(NT(170), (NT(171)));
//G249: constraint(93)       => ctn_eq(170).
	p(NT(93), (NT(170)));
//G250: __E_constraint_71(173) => '[' ctnvar(168) _(11) '>' '=' _(11) num(169) ']'.
	p(NT(173), (T(26)+NT(168)+NT(11)+T(23)+T(3)+NT(11)+NT(169)+T(27)));
//G251: __E_constraint_71(173) => '[' num(169) _(11) '>' '=' _(11) ctnvar(168) ']'.
	p(NT(173), (T(26)+NT(169)+NT(11)+T(23)+T(3)+NT(11)+NT(168)+T(27)));
//G252: ctn_gteq(172)        => __E_constraint_71(173).
	p(NT(172), (NT(173)));
//G253: constraint(93)       => ctn_gteq(172).
	p(NT(93), (NT(172)));
//G254: __E_constraint_72(175) => '[' ctnvar(168) _(11) '>' _(11) num(169) ']'.
	p(NT(175), (T(26)+NT(168)+NT(11)+T(23)+NT(11)+NT(169)+T(27)));
//G255: __E_constraint_72(175) => '[' num(169) _(11) '>' _(11) ctnvar(168) ']'.
	p(NT(175), (T(26)+NT(169)+NT(11)+T(23)+NT(11)+NT(168)+T(27)));
//G256: ctn_gt(174)          => __E_constraint_72(175).
	p(NT(174), (NT(175)));
//G257: constraint(93)       => ctn_gt(174).
	p(NT(93), (NT(174)));
//G258: __E_constraint_73(177) => '[' ctnvar(168) _(11) '<' '=' _(11) num(169) ']'.
	p(NT(177), (T(26)+NT(168)+NT(11)+T(22)+T(3)+NT(11)+NT(169)+T(27)));
//G259: __E_constraint_73(177) => '[' num(169) _(11) '<' '=' _(11) ctnvar(168) ']'.
	p(NT(177), (T(26)+NT(169)+NT(11)+T(22)+T(3)+NT(11)+NT(168)+T(27)));
//G260: ctn_lteq(176)        => __E_constraint_73(177).
	p(NT(176), (NT(177)));
//G261: constraint(93)       => ctn_lteq(176).
	p(NT(93), (NT(176)));
//G262: __E_constraint_74(179) => '[' ctnvar(168) _(11) '<' _(11) num(169) ']'.
	p(NT(179), (T(26)+NT(168)+NT(11)+T(22)+NT(11)+NT(169)+T(27)));
//G263: __E_constraint_74(179) => '[' num(169) _(11) '<' _(11) ctnvar(168) ']'.
	p(NT(179), (T(26)+NT(169)+NT(11)+T(22)+NT(11)+NT(168)+T(27)));
//G264: ctn_lt(178)          => __E_constraint_74(179).
	p(NT(178), (NT(179)));
//G265: constraint(93)       => ctn_lt(178).
	p(NT(93), (NT(178)));
//G266: __E_bf_constant_75(180) => capture(29).
	p(NT(180), (NT(29)));
//G267: __E_bf_constant_75(180) => source(181).
	p(NT(180), (NT(181)));
//G268: __E_bf_constant_76(182) => typed(33).
	p(NT(182), (NT(33)));
//G269: __E_bf_constant_76(182) => null.
	p(NT(182), (nul));
//G270: bf_constant(134)     => '{' _(11) __E_bf_constant_75(180) _(11) '}' __E_bf_constant_76(182).
	p(NT(134), (T(42)+NT(11)+NT(180)+NT(11)+T(43)+NT(182)));
//G271: __E___E_source_77_78(185) => src_c(183).
	p(NT(185), (NT(183)));
//G272: __E___E_source_77_78(185) => space(2).
	p(NT(185), (NT(2)));
//G273: __E___E_source_77_79(186) => null.
	p(NT(186), (nul));
//G274: __E___E_source_77_79(186) => __E___E_source_77_78(185) __E___E_source_77_79(186).
	p(NT(186), (NT(185)+NT(186)));
//G275: __E_source_77(184)   => __E___E_source_77_79(186) src_c(183).
	p(NT(184), (NT(186)+NT(183)));
//G276: __E_source_77(184)   => null.
	p(NT(184), (nul));
//G277: source(181)          => src_c(183) __E_source_77(184).
	p(NT(181), (NT(183)+NT(184)));
//G278: src_c(183)           => alnum(6).
	p(NT(183), (NT(6)));
//G279: __N_0(378)           => '{'.
	p(NT(378), (T(42)));
//G280: __N_1(379)           => '}'.
	p(NT(379), (T(43)));
//G281: src_c(183)           => ~( __N_0(378) ) & ~( __N_1(379) ) & punct(7).	 # conjunctive
	p(NT(183), ~(NT(378)) & ~(NT(379)) & (NT(7)));
//G282: __E_src_c_80(187)    => src_c(183).
	p(NT(187), (NT(183)));
//G283: __E_src_c_80(187)    => space(2).
	p(NT(187), (NT(2)));
//G284: __E_src_c_81(188)    => null.
	p(NT(188), (nul));
//G285: __E_src_c_81(188)    => __E_src_c_80(187) __E_src_c_81(188).
	p(NT(188), (NT(187)+NT(188)));
//G286: src_c(183)           => '{' __E_src_c_81(188) '}'.
	p(NT(183), (T(42)+NT(188)+T(43)));
//G287: __E_variable_82(189) => uconst(190).
	p(NT(189), (NT(190)));
//G288: __E_variable_82(189) => io_var(191).
	p(NT(189), (NT(191)));
//G289: __E_variable_82(189) => var_name(192).
	p(NT(189), (NT(192)));
//G290: __E_variable_83(193) => typed(33).
	p(NT(193), (NT(33)));
//G291: __E_variable_83(193) => null.
	p(NT(193), (nul));
//G292: variable(135)        => __E_variable_82(189) __E_variable_83(193).
	p(NT(135), (NT(189)+NT(193)));
//G293: __N_2(380)           => 'F'.
	p(NT(380), (T(36)));
//G294: __N_3(381)           => 'T'.
	p(NT(381), (T(35)));
//G295: __E_var_name_84(194) => ~( __N_2(380) ) & ~( __N_3(381) ) & alpha(5).	 # conjunctive
	p(NT(194), ~(NT(380)) & ~(NT(381)) & (NT(5)));
//G296: __E_var_name_85(195) => null.
	p(NT(195), (nul));
//G297: __E_var_name_85(195) => digit(3) __E_var_name_85(195).
	p(NT(195), (NT(3)+NT(195)));
//G298: var_name(192)        => __E_var_name_84(194) __E_var_name_85(195).	 # guarded: charvar
	p(NT(192), (NT(194)+NT(195)));
	p.back().guard = "charvar";
//G299: __N_4(382)           => 'F'.
	p(NT(382), (T(36)));
//G300: __N_5(383)           => 'T'.
	p(NT(383), (T(35)));
//G301: __E_var_name_86(196) => ~( __N_4(382) ) & ~( __N_5(383) ) & alpha(5).	 # conjunctive
	p(NT(196), ~(NT(382)) & ~(NT(383)) & (NT(5)));
//G302: __E_var_name_87(197) => alnum(6).
	p(NT(197), (NT(6)));
//G303: __E_var_name_87(197) => '_'.
	p(NT(197), (T(44)));
//G304: __E_var_name_88(198) => null.
	p(NT(198), (nul));
//G305: __E_var_name_88(198) => __E_var_name_87(197) __E_var_name_88(198).
	p(NT(198), (NT(197)+NT(198)));
//G306: var_name(192)        => __E_var_name_86(196) __E_var_name_88(198).	 # guarded: var
	p(NT(192), (NT(196)+NT(198)));
	p.back().guard = "var";
//G307: io_var(191)          => io_var_name(31) '[' offset(199) ']'.
	p(NT(191), (NT(31)+T(26)+NT(199)+T(27)));
//G308: io_var_name(31)      => chars(200).
	p(NT(31), (NT(200)));
//G309: __E___E_uconst_89_90(203) => chars(200) _(11).
	p(NT(203), (NT(200)+NT(11)));
//G310: __E___E_uconst_89_90(203) => null.
	p(NT(203), (nul));
//G311: __E_uconst_89(202)   => __E___E_uconst_89_90(203) ':' _(11) chars(200).
	p(NT(202), (NT(203)+T(2)+NT(11)+NT(200)));
//G312: uconst_name(201)     => __E_uconst_89(202).
	p(NT(201), (NT(202)));
//G313: uconst(190)          => '<' _(11) uconst_name(201) _(11) '>'.
	p(NT(190), (T(22)+NT(11)+NT(201)+NT(11)+T(23)));
//G314: __E_q_vars_91(205)   => _(11) ',' _(11) q_var(204).
	p(NT(205), (NT(11)+T(20)+NT(11)+NT(204)));
//G315: __E_q_vars_92(206)   => null.
	p(NT(206), (nul));
//G316: __E_q_vars_92(206)   => __E_q_vars_91(205) __E_q_vars_92(206).
	p(NT(206), (NT(205)+NT(206)));
//G317: q_vars(89)           => q_var(204) __E_q_vars_92(206).
	p(NT(89), (NT(204)+NT(206)));
//G318: q_var(204)           => capture(29).
	p(NT(204), (NT(29)));
//G319: __N_6(384)           => uconst(190).
	p(NT(384), (NT(190)));
//G320: q_var(204)           => variable(135) & ~( __N_6(384) ).	 # conjunctive
	p(NT(204), (NT(135)) & ~(NT(384)));
//G321: ctnvar(168)          => chars(200).
	p(NT(168), (NT(200)));
//G322: __E_offsets_93(207)  => _(11) ',' _(11) offset(199).
	p(NT(207), (NT(11)+T(20)+NT(11)+NT(199)));
//G323: __E_offsets_94(208)  => null.
	p(NT(208), (nul));
//G324: __E_offsets_94(208)  => __E_offsets_93(207) __E_offsets_94(208).
	p(NT(208), (NT(207)+NT(208)));
//G325: offsets(42)          => '[' _(11) offset(199) __E_offsets_94(208) _(11) ']'.
	p(NT(42), (T(26)+NT(11)+NT(199)+NT(208)+NT(11)+T(27)));
//G326: offset(199)          => integer(209).
	p(NT(199), (NT(209)));
//G327: offset(199)          => capture(29).
	p(NT(199), (NT(29)));
//G328: offset(199)          => shift(210).
	p(NT(199), (NT(210)));
//G329: __N_7(385)           => io_var(191).
	p(NT(385), (NT(191)));
//G330: __E_offset_95(211)   => variable(135) & ~( __N_7(385) ).	 # conjunctive
	p(NT(211), (NT(135)) & ~(NT(385)));
//G331: offset(199)          => __E_offset_95(211).
	p(NT(199), (NT(211)));
//G332: __E_shift_96(212)    => capture(29).
	p(NT(212), (NT(29)));
//G333: __N_8(386)           => io_var(191).
	p(NT(386), (NT(191)));
//G334: __E___E_shift_96_97(213) => variable(135) & ~( __N_8(386) ).	 # conjunctive
	p(NT(213), (NT(135)) & ~(NT(386)));
//G335: __E_shift_96(212)    => __E___E_shift_96_97(213).
	p(NT(212), (NT(213)));
//G336: shift(210)           => __E_shift_96(212) _(11) '-' _(11) num(169).
	p(NT(210), (NT(212)+NT(11)+T(30)+NT(11)+NT(169)));
//G337: __E_chars_98(214)    => alnum(6).
	p(NT(214), (NT(6)));
//G338: __E_chars_98(214)    => '_'.
	p(NT(214), (T(44)));
//G339: __E_chars_99(215)    => null.
	p(NT(215), (nul));
//G340: __E_chars_99(215)    => __E_chars_98(214) __E_chars_99(215).
	p(NT(215), (NT(214)+NT(215)));
//G341: chars(200)           => alpha(5) __E_chars_99(215).
	p(NT(200), (NT(5)+NT(215)));
//G342: __E_digits_100(217)  => digit(3).
	p(NT(217), (NT(3)));
//G343: __E_digits_100(217)  => digit(3) __E_digits_100(217).
	p(NT(217), (NT(3)+NT(217)));
//G344: digits(216)          => __E_digits_100(217).
	p(NT(216), (NT(217)));
//G345: num(169)             => digits(216).
	p(NT(169), (NT(216)));
//G346: __E_integer_101(218) => '-'.
	p(NT(218), (T(30)));
//G347: __E_integer_101(218) => null.
	p(NT(218), (nul));
//G348: integer(209)         => __E_integer_101(218) _(11) digits(216).
	p(NT(209), (NT(218)+NT(11)+NT(216)));
//G349: sym(40)              => chars(200).
	p(NT(40), (NT(200)));
//G350: capture(29)          => '$' chars(200).
	p(NT(29), (T(45)+NT(200)));
//G351: type(219)            => chars(200).
	p(NT(219), (NT(200)));
//G352: typed(33)            => _(11) ':' _(11) type(219).
	p(NT(33), (NT(11)+T(2)+NT(11)+NT(219)));
//G353: scope_id(220)        => digits(216).
	p(NT(220), (NT(216)));
//G354: __E_comment_102(222) => printable(8).
	p(NT(222), (NT(8)));
//G355: __E_comment_102(222) => '\t'.
	p(NT(222), (T(46)));
//G356: __E_comment_103(223) => null.
	p(NT(223), (nul));
//G357: __E_comment_103(223) => __E_comment_102(222) __E_comment_103(223).
	p(NT(223), (NT(222)+NT(223)));
//G358: __E_comment_104(224) => '\n'.
	p(NT(224), (T(47)));
//G359: __E_comment_104(224) => '\r'.
	p(NT(224), (T(48)));
//G360: __E_comment_104(224) => eof(1).
	p(NT(224), (NT(1)));
//G361: comment(221)         => '#' __E_comment_103(223) __E_comment_104(224).
	p(NT(221), (T(49)+NT(223)+NT(224)));
//G362: __(34)               => space(2).
	p(NT(34), (NT(2)));
//G363: __(34)               => comment(221).
	p(NT(34), (NT(221)));
//G364: __(34)               => __(34) space(2).
	p(NT(34), (NT(34)+NT(2)));
//G365: __(34)               => __(34) comment(221).
	p(NT(34), (NT(34)+NT(221)));
//G366: _(11)                => __(34).
	p(NT(11), (NT(34)));
//G367: _(11)                => null.
	p(NT(11), (nul));
//G368: cli(225)             => _(11).
	p(NT(225), (NT(11)));
//G369: __E_cli_105(227)     => '.' _(11) cli_command(226) _(11).
	p(NT(227), (T(1)+NT(11)+NT(226)+NT(11)));
//G370: __E_cli_106(228)     => null.
	p(NT(228), (nul));
//G371: __E_cli_106(228)     => __E_cli_105(227) __E_cli_106(228).
	p(NT(228), (NT(227)+NT(228)));
//G372: cli(225)             => _(11) cli_command(226) _(11) __E_cli_106(228).
	p(NT(225), (NT(11)+NT(226)+NT(11)+NT(228)));
//G373: __E_cli_command_107(231) => 'q'.
	p(NT(231), (T(50)));
//G374: __E_cli_command_107(231) => 'q' 'u' 'i' 't'.
	p(NT(231), (T(50)+T(7)+T(4)+T(8)));
//G375: quit_sym(230)        => __E_cli_command_107(231).
	p(NT(230), (NT(231)));
//G376: quit_cmd(229)        => quit_sym(230).
	p(NT(229), (NT(230)));
//G377: cli_command(226)     => quit_cmd(229).
	p(NT(226), (NT(229)));
//G378: __E_cli_command_108(234) => 'v'.
	p(NT(234), (T(51)));
//G379: __E_cli_command_108(234) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(234), (T(51)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G380: version_sym(233)     => __E_cli_command_108(234).
	p(NT(233), (NT(234)));
//G381: version_cmd(232)     => version_sym(233).
	p(NT(232), (NT(233)));
//G382: cli_command(226)     => version_cmd(232).
	p(NT(226), (NT(232)));
//G383: __E_cli_command_109(237) => 'c'.
	p(NT(237), (T(14)));
//G384: __E_cli_command_109(237) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(237), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G385: clear_sym(236)       => __E_cli_command_109(237).
	p(NT(236), (NT(237)));
//G386: clear_cmd(235)       => clear_sym(236).
	p(NT(235), (NT(236)));
//G387: cli_command(226)     => clear_cmd(235).
	p(NT(226), (NT(235)));
//G388: __E___E_cli_command_110_111(241) => 'h'.
	p(NT(241), (T(52)));
//G389: __E___E_cli_command_110_111(241) => 'h' 'e' 'l' 'p'.
	p(NT(241), (T(52)+T(11)+T(10)+T(53)));
//G390: help_sym(240)        => __E___E_cli_command_110_111(241).
	p(NT(240), (NT(241)));
//G391: __E___E_cli_command_110_112(242) => __(34) help_arg(243).
	p(NT(242), (NT(34)+NT(243)));
//G392: __E___E_cli_command_110_112(242) => null.
	p(NT(242), (nul));
//G393: __E_cli_command_110(239) => help_sym(240) __E___E_cli_command_110_112(242).
	p(NT(239), (NT(240)+NT(242)));
//G394: help_cmd(238)        => __E_cli_command_110(239).
	p(NT(238), (NT(239)));
//G395: cli_command(226)     => help_cmd(238).
	p(NT(226), (NT(238)));
//G396: file_sym(246)        => 'f' 'i' 'l' 'e'.
	p(NT(246), (T(9)+T(4)+T(10)+T(11)));
//G397: __E_cli_command_113(245) => file_sym(246) __(34) q_string(247).
	p(NT(245), (NT(246)+NT(34)+NT(247)));
//G398: file_cmd(244)        => __E_cli_command_113(245).
	p(NT(244), (NT(245)));
//G399: cli_command(226)     => file_cmd(244).
	p(NT(226), (NT(244)));
//G400: valid_sym(250)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(250), (T(51)+T(16)+T(10)+T(4)+T(54)));
//G401: __E_cli_command_114(249) => valid_sym(250) __(34) normalize_cmd_arg(251).
	p(NT(249), (NT(250)+NT(34)+NT(251)));
//G402: valid_cmd(248)       => __E_cli_command_114(249).
	p(NT(248), (NT(249)));
//G403: cli_command(226)     => valid_cmd(248).
	p(NT(226), (NT(248)));
//G404: sat_sym(254)         => 's' 'a' 't'.
	p(NT(254), (T(15)+T(16)+T(8)));
//G405: __E_cli_command_115(253) => sat_sym(254) __(34) normalize_cmd_arg(251).
	p(NT(253), (NT(254)+NT(34)+NT(251)));
//G406: sat_cmd(252)         => __E_cli_command_115(253).
	p(NT(252), (NT(253)));
//G407: cli_command(226)     => sat_cmd(252).
	p(NT(226), (NT(252)));
//G408: unsat_sym(257)       => 'u' 'n' 's' 'a' 't'.
	p(NT(257), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G409: __E_cli_command_116(256) => unsat_sym(257) __(34) normalize_cmd_arg(251).
	p(NT(256), (NT(257)+NT(34)+NT(251)));
//G410: unsat_cmd(255)       => __E_cli_command_116(256).
	p(NT(255), (NT(256)));
//G411: cli_command(226)     => unsat_cmd(255).
	p(NT(226), (NT(255)));
//G412: solve_sym(260)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(260), (T(15)+T(6)+T(10)+T(51)+T(11)));
//G413: __E___E_cli_command_117_118(261) => solve_options(262).
	p(NT(261), (NT(262)));
//G414: __E___E_cli_command_117_118(261) => null.
	p(NT(261), (nul));
//G415: __E_cli_command_117(259) => solve_sym(260) __E___E_cli_command_117_118(261) __(34) wff_cmd_arg(263).
	p(NT(259), (NT(260)+NT(261)+NT(34)+NT(263)));
//G416: solve_cmd(258)       => __E_cli_command_117(259).
	p(NT(258), (NT(259)));
//G417: cli_command(226)     => solve_cmd(258).
	p(NT(226), (NT(258)));
//G418: lgrs_sym(266)        => 'l' 'g' 'r' 's'.
	p(NT(266), (T(10)+T(55)+T(19)+T(15)));
//G419: __E_cli_command_119(265) => lgrs_sym(266) __(34) wff_cmd_arg(263).
	p(NT(265), (NT(266)+NT(34)+NT(263)));
//G420: lgrs_cmd(264)        => __E_cli_command_119(265).
	p(NT(264), (NT(265)));
//G421: cli_command(226)     => lgrs_cmd(264).
	p(NT(226), (NT(264)));
//G422: __E___E_cli_command_120_121(270) => 'r'.
	p(NT(270), (T(19)));
//G423: __E___E_cli_command_120_121(270) => 'r' 'u' 'n'.
	p(NT(270), (T(19)+T(7)+T(5)));
//G424: run_sym(269)         => __E___E_cli_command_120_121(270).
	p(NT(269), (NT(270)));
//G425: __E___E_cli_command_120_122(271) => history(272).
	p(NT(271), (NT(272)));
//G426: __E___E_cli_command_120_122(271) => wff(14).
	p(NT(271), (NT(14)));
//G427: __E_cli_command_120(268) => run_sym(269) __(34) __E___E_cli_command_120_122(271).
	p(NT(268), (NT(269)+NT(34)+NT(271)));
//G428: run_cmd(267)         => __E_cli_command_120(268).
	p(NT(267), (NT(268)));
//G429: cli_command(226)     => run_cmd(267).
	p(NT(226), (NT(267)));
//G430: __E___E_cli_command_123_124(276) => 'n'.
	p(NT(276), (T(5)));
//G431: __E___E_cli_command_123_124(276) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(276), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(56)+T(11)));
//G432: normalize_sym(275)   => __E___E_cli_command_123_124(276).
	p(NT(275), (NT(276)));
//G433: __E_cli_command_123(274) => normalize_sym(275) __(34) normalize_cmd_arg(251).
	p(NT(274), (NT(275)+NT(34)+NT(251)));
//G434: normalize_cmd(273)   => __E_cli_command_123(274).
	p(NT(273), (NT(274)));
//G435: cli_command(226)     => normalize_cmd(273).
	p(NT(226), (NT(273)));
//G436: __E___E_cli_command_125_126(280) => 's'.
	p(NT(280), (T(15)));
//G437: __E___E_cli_command_125_126(280) => 's' 'u' 'b' 's' 't'.
	p(NT(280), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G438: __E___E_cli_command_125_126(280) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(280), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G439: subst_sym(279)       => __E___E_cli_command_125_126(280).
	p(NT(279), (NT(280)));
//G440: __E_cli_command_125(278) => subst_sym(279) __(34) nf_cmd_arg(281) _(11) '[' _(11) nf_cmd_arg(281) _(11) '/' _(11) nf_cmd_arg(281) _(11) ']'.
	p(NT(278), (NT(279)+NT(34)+NT(281)+NT(11)+T(26)+NT(11)+NT(281)+NT(11)+T(57)+NT(11)+NT(281)+NT(11)+T(27)));
//G441: subst_cmd(277)       => __E_cli_command_125(278).
	p(NT(277), (NT(278)));
//G442: cli_command(226)     => subst_cmd(277).
	p(NT(226), (NT(277)));
//G443: __E___E_cli_command_127_128(285) => 'i'.
	p(NT(285), (T(4)));
//G444: __E___E_cli_command_127_128(285) => 'i' 'n' 's' 't'.
	p(NT(285), (T(4)+T(5)+T(15)+T(8)));
//G445: __E___E_cli_command_127_128(285) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(285), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G446: inst_sym(284)        => __E___E_cli_command_127_128(285).
	p(NT(284), (NT(285)));
//G447: __E_cli_command_127(283) => inst_sym(284) __(34) inst_args(286).
	p(NT(283), (NT(284)+NT(34)+NT(286)));
//G448: inst_cmd(282)        => __E_cli_command_127(283).
	p(NT(282), (NT(283)));
//G449: cli_command(226)     => inst_cmd(282).
	p(NT(226), (NT(282)));
//G450: dnf_sym(289)         => 'd' 'n' 'f'.
	p(NT(289), (T(54)+T(5)+T(9)));
//G451: __E_cli_command_129(288) => dnf_sym(289) __(34) nf_cmd_arg(281).
	p(NT(288), (NT(289)+NT(34)+NT(281)));
//G452: dnf_cmd(287)         => __E_cli_command_129(288).
	p(NT(287), (NT(288)));
//G453: cli_command(226)     => dnf_cmd(287).
	p(NT(226), (NT(287)));
//G454: cnf_sym(292)         => 'c' 'n' 'f'.
	p(NT(292), (T(14)+T(5)+T(9)));
//G455: __E_cli_command_130(291) => cnf_sym(292) __(34) nf_cmd_arg(281).
	p(NT(291), (NT(292)+NT(34)+NT(281)));
//G456: cnf_cmd(290)         => __E_cli_command_130(291).
	p(NT(290), (NT(291)));
//G457: cli_command(226)     => cnf_cmd(290).
	p(NT(226), (NT(290)));
//G458: anf_sym(295)         => 'a' 'n' 'f'.
	p(NT(295), (T(16)+T(5)+T(9)));
//G459: __E_cli_command_131(294) => anf_sym(295) __(34) nf_cmd_arg(281).
	p(NT(294), (NT(295)+NT(34)+NT(281)));
//G460: anf_cmd(293)         => __E_cli_command_131(294).
	p(NT(293), (NT(294)));
//G461: cli_command(226)     => anf_cmd(293).
	p(NT(226), (NT(293)));
//G462: nnf_sym(298)         => 'n' 'n' 'f'.
	p(NT(298), (T(5)+T(5)+T(9)));
//G463: __E_cli_command_132(297) => nnf_sym(298) __(34) nf_cmd_arg(281).
	p(NT(297), (NT(298)+NT(34)+NT(281)));
//G464: nnf_cmd(296)         => __E_cli_command_132(297).
	p(NT(296), (NT(297)));
//G465: cli_command(226)     => nnf_cmd(296).
	p(NT(226), (NT(296)));
//G466: pnf_sym(301)         => 'p' 'n' 'f'.
	p(NT(301), (T(53)+T(5)+T(9)));
//G467: __E_cli_command_133(300) => pnf_sym(301) __(34) nf_cmd_arg(281).
	p(NT(300), (NT(301)+NT(34)+NT(281)));
//G468: pnf_cmd(299)         => __E_cli_command_133(300).
	p(NT(299), (NT(300)));
//G469: cli_command(226)     => pnf_cmd(299).
	p(NT(226), (NT(299)));
//G470: mnf_sym(304)         => 'm' 'n' 'f'.
	p(NT(304), (T(21)+T(5)+T(9)));
//G471: __E_cli_command_134(303) => mnf_sym(304) __(34) nf_cmd_arg(281).
	p(NT(303), (NT(304)+NT(34)+NT(281)));
//G472: mnf_cmd(302)         => __E_cli_command_134(303).
	p(NT(302), (NT(303)));
//G473: cli_command(226)     => mnf_cmd(302).
	p(NT(226), (NT(302)));
//G474: snf_sym(307)         => 's' 'n' 'f'.
	p(NT(307), (T(15)+T(5)+T(9)));
//G475: __E_cli_command_135(306) => snf_sym(307) __(34) nf_cmd_arg(281).
	p(NT(306), (NT(307)+NT(34)+NT(281)));
//G476: snf_cmd(305)         => __E_cli_command_135(306).
	p(NT(305), (NT(306)));
//G477: cli_command(226)     => snf_cmd(305).
	p(NT(226), (NT(305)));
//G478: onf_sym(310)         => 'o' 'n' 'f'.
	p(NT(310), (T(6)+T(5)+T(9)));
//G479: __E_cli_command_136(309) => onf_sym(310) __(34) variable(135) __(34) onf_cmd_arg(311).
	p(NT(309), (NT(310)+NT(34)+NT(135)+NT(34)+NT(311)));
//G480: onf_cmd(308)         => __E_cli_command_136(309).
	p(NT(308), (NT(309)));
//G481: cli_command(226)     => onf_cmd(308).
	p(NT(226), (NT(308)));
//G482: qelim_sym(314)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(314), (T(50)+T(11)+T(10)+T(4)+T(21)));
//G483: __E_cli_command_137(313) => qelim_sym(314) __(34) wff_cmd_arg(263).
	p(NT(313), (NT(314)+NT(34)+NT(263)));
//G484: qelim_cmd(312)       => __E_cli_command_137(313).
	p(NT(312), (NT(313)));
//G485: cli_command(226)     => qelim_cmd(312).
	p(NT(226), (NT(312)));
//G486: get_sym(317)         => 'g' 'e' 't'.
	p(NT(317), (T(55)+T(11)+T(8)));
//G487: __E___E_cli_command_138_139(318) => __(34) option_name(319).
	p(NT(318), (NT(34)+NT(319)));
//G488: __E___E_cli_command_138_139(318) => null.
	p(NT(318), (nul));
//G489: __E_cli_command_138(316) => get_sym(317) __E___E_cli_command_138_139(318).
	p(NT(316), (NT(317)+NT(318)));
//G490: get_cmd(315)         => __E_cli_command_138(316).
	p(NT(315), (NT(316)));
//G491: cli_command(226)     => get_cmd(315).
	p(NT(226), (NT(315)));
//G492: set_sym(322)         => 's' 'e' 't'.
	p(NT(322), (T(15)+T(11)+T(8)));
//G493: __E___E_cli_command_140_141(323) => __(34).
	p(NT(323), (NT(34)));
//G494: __E___E_cli_command_140_141(323) => _(11) '=' _(11).
	p(NT(323), (NT(11)+T(3)+NT(11)));
//G495: __E_cli_command_140(321) => set_sym(322) __(34) option_name(319) __E___E_cli_command_140_141(323) option_value(324).
	p(NT(321), (NT(322)+NT(34)+NT(319)+NT(323)+NT(324)));
//G496: set_cmd(320)         => __E_cli_command_140(321).
	p(NT(320), (NT(321)));
//G497: cli_command(226)     => set_cmd(320).
	p(NT(226), (NT(320)));
//G498: enable_sym(327)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(327), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G499: __E_cli_command_142(326) => enable_sym(327) __(34) option_name(319).
	p(NT(326), (NT(327)+NT(34)+NT(319)));
//G500: enable_cmd(325)      => __E_cli_command_142(326).
	p(NT(325), (NT(326)));
//G501: cli_command(226)     => enable_cmd(325).
	p(NT(226), (NT(325)));
//G502: disable_sym(330)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(330), (T(54)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G503: __E_cli_command_143(329) => disable_sym(330) __(34) option_name(319).
	p(NT(329), (NT(330)+NT(34)+NT(319)));
//G504: disable_cmd(328)     => __E_cli_command_143(329).
	p(NT(328), (NT(329)));
//G505: cli_command(226)     => disable_cmd(328).
	p(NT(226), (NT(328)));
//G506: toggle_sym(333)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(333), (T(8)+T(6)+T(55)+T(55)+T(10)+T(11)));
//G507: __E_cli_command_144(332) => toggle_sym(333) __(34) option_name(319).
	p(NT(332), (NT(333)+NT(34)+NT(319)));
//G508: toggle_cmd(331)      => __E_cli_command_144(332).
	p(NT(331), (NT(332)));
//G509: cli_command(226)     => toggle_cmd(331).
	p(NT(226), (NT(331)));
//G510: __E___E_cli_command_145_146(337) => 'd' 'e' 'f' 's'.
	p(NT(337), (T(54)+T(11)+T(9)+T(15)));
//G511: __E___E_cli_command_145_146(337) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(337), (T(54)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G512: def_sym(336)         => __E___E_cli_command_145_146(337).
	p(NT(336), (NT(337)));
//G513: __E_cli_command_145(335) => def_sym(336).
	p(NT(335), (NT(336)));
//G514: def_list_cmd(334)    => __E_cli_command_145(335).
	p(NT(334), (NT(335)));
//G515: cli_command(226)     => def_list_cmd(334).
	p(NT(226), (NT(334)));
//G516: __E_cli_command_147(339) => def_sym(336) __(34) num(169).
	p(NT(339), (NT(336)+NT(34)+NT(169)));
//G517: def_print_cmd(338)   => __E_cli_command_147(339).
	p(NT(338), (NT(339)));
//G518: cli_command(226)     => def_print_cmd(338).
	p(NT(226), (NT(338)));
//G519: def_rr_cmd(340)      => rec_relation(17).
	p(NT(340), (NT(17)));
//G520: cli_command(226)     => def_rr_cmd(340).
	p(NT(226), (NT(340)));
//G521: def_input_cmd(341)   => input_def(18).
	p(NT(341), (NT(18)));
//G522: cli_command(226)     => def_input_cmd(341).
	p(NT(226), (NT(341)));
//G523: def_output_cmd(342)  => output_def(19).
	p(NT(342), (NT(19)));
//G524: cli_command(226)     => def_output_cmd(342).
	p(NT(226), (NT(342)));
//G525: __E___E_cli_command_148_149(346) => 'h' 'i' 's' 't'.
	p(NT(346), (T(52)+T(4)+T(15)+T(8)));
//G526: __E___E_cli_command_148_149(346) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(346), (T(52)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G527: history_sym(345)     => __E___E_cli_command_148_149(346).
	p(NT(345), (NT(346)));
//G528: __E_cli_command_148(344) => history_sym(345).
	p(NT(344), (NT(345)));
//G529: history_list_cmd(343) => __E_cli_command_148(344).
	p(NT(343), (NT(344)));
//G530: cli_command(226)     => history_list_cmd(343).
	p(NT(226), (NT(343)));
//G531: __E_cli_command_150(348) => history_sym(345) __(34) history(272).
	p(NT(348), (NT(345)+NT(34)+NT(272)));
//G532: history_print_cmd(347) => __E_cli_command_150(348).
	p(NT(347), (NT(348)));
//G533: cli_command(226)     => history_print_cmd(347).
	p(NT(226), (NT(347)));
//G534: __E_cli_command_151(350) => wff(14).
	p(NT(350), (NT(14)));
//G535: __E_cli_command_151(350) => bf(30).
	p(NT(350), (NT(30)));
//G536: history_store_cmd(349) => __E_cli_command_151(350).
	p(NT(349), (NT(350)));
//G537: cli_command(226)     => history_store_cmd(349).
	p(NT(226), (NT(349)));
//G538: __E_solve_options_152(351) => __(34) solve_option(352).
	p(NT(351), (NT(34)+NT(352)));
//G539: __E_solve_options_153(353) => null.
	p(NT(353), (nul));
//G540: __E_solve_options_153(353) => __E_solve_options_152(351) __E_solve_options_153(353).
	p(NT(353), (NT(351)+NT(353)));
//G541: solve_options(262)   => __E_solve_options_153(353).
	p(NT(262), (NT(353)));
//G542: __E_solve_option_154(354) => solver_mode(355).
	p(NT(354), (NT(355)));
//G543: __E_solve_option_154(354) => type(219).
	p(NT(354), (NT(219)));
//G544: solve_option(352)    => '-' '-' __E_solve_option_154(354).
	p(NT(352), (T(30)+T(30)+NT(354)));
//G545: __E_solver_mode_155(357) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(357), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G546: __E_solver_mode_155(357) => 'm' 'i' 'n'.
	p(NT(357), (T(21)+T(4)+T(5)));
//G547: solver_mode_minimum(356) => __E_solver_mode_155(357).
	p(NT(356), (NT(357)));
//G548: solver_mode(355)     => solver_mode_minimum(356).
	p(NT(355), (NT(356)));
//G549: __E_solver_mode_156(359) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(359), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G550: __E_solver_mode_156(359) => 'm' 'a' 'x'.
	p(NT(359), (T(21)+T(16)+T(29)));
//G551: solver_mode_maximum(358) => __E_solver_mode_156(359).
	p(NT(358), (NT(359)));
//G552: solver_mode(355)     => solver_mode_maximum(358).
	p(NT(355), (NT(358)));
//G553: q_file_name(38)      => '"' file_name(360) '"'.
	p(NT(38), (T(58)+NT(360)+T(58)));
//G554: __E_file_name_157(361) => printable(8).
	p(NT(361), (NT(8)));
//G555: __E_file_name_157(361) => printable(8) __E_file_name_157(361).
	p(NT(361), (NT(8)+NT(361)));
//G556: file_name(360)       => __E_file_name_157(361).
	p(NT(360), (NT(361)));
//G557: __E_option_name_158(362) => alnum(6).
	p(NT(362), (NT(6)));
//G558: __E_option_name_158(362) => alnum(6) __E_option_name_158(362).
	p(NT(362), (NT(6)+NT(362)));
//G559: option_name(319)     => __E_option_name_158(362).
	p(NT(319), (NT(362)));
//G560: __E_option_value_159(363) => alnum(6).
	p(NT(363), (NT(6)));
//G561: __E_option_value_159(363) => alnum(6) __E_option_value_159(363).
	p(NT(363), (NT(6)+NT(363)));
//G562: option_value(324)    => __E_option_value_159(363).
	p(NT(324), (NT(363)));
//G563: bf_cmd_arg(364)      => history(272).
	p(NT(364), (NT(272)));
//G564: bf_cmd_arg(364)      => bf(30).
	p(NT(364), (NT(30)));
//G565: wff_cmd_arg(263)     => history(272).
	p(NT(263), (NT(272)));
//G566: wff_cmd_arg(263)     => wff(14).
	p(NT(263), (NT(14)));
//G567: nf_cmd_arg(281)      => history(272).
	p(NT(281), (NT(272)));
//G568: nf_cmd_arg(281)      => ref(27).
	p(NT(281), (NT(27)));
//G569: nf_cmd_arg(281)      => wff(14).
	p(NT(281), (NT(14)));
//G570: nf_cmd_arg(281)      => bf(30).
	p(NT(281), (NT(30)));
//G571: onf_cmd_arg(311)     => history(272).
	p(NT(311), (NT(272)));
//G572: onf_cmd_arg(311)     => wff(14).
	p(NT(311), (NT(14)));
//G573: normalize_cmd_arg(251) => history(272).
	p(NT(251), (NT(272)));
//G574: normalize_cmd_arg(251) => spec(10).
	p(NT(251), (NT(10)));
//G575: normalize_cmd_arg(251) => ref(27).
	p(NT(251), (NT(27)));
//G576: normalize_cmd_arg(251) => wff(14).
	p(NT(251), (NT(14)));
//G577: normalize_cmd_arg(251) => bf(30).
	p(NT(251), (NT(30)));
//G578: inst_args(286)       => wff_cmd_arg(263) _(11) '[' _(11) variable(135) _(11) '/' _(11) bf_cmd_arg(364) _(11) ']'.
	p(NT(286), (NT(263)+NT(11)+T(26)+NT(11)+NT(135)+NT(11)+T(57)+NT(11)+NT(364)+NT(11)+T(27)));
//G579: inst_args(286)       => bf_cmd_arg(364) _(11) '[' _(11) variable(135) _(11) '/' _(11) bf_cmd_arg(364) _(11) ']'.
	p(NT(286), (NT(364)+NT(11)+T(26)+NT(11)+NT(135)+NT(11)+T(57)+NT(11)+NT(364)+NT(11)+T(27)));
//G580: help_arg(243)        => help_sym(240).
	p(NT(243), (NT(240)));
//G581: help_arg(243)        => version_sym(233).
	p(NT(243), (NT(233)));
//G582: help_arg(243)        => quit_sym(230).
	p(NT(243), (NT(230)));
//G583: help_arg(243)        => clear_sym(236).
	p(NT(243), (NT(236)));
//G584: help_arg(243)        => get_sym(317).
	p(NT(243), (NT(317)));
//G585: help_arg(243)        => set_sym(322).
	p(NT(243), (NT(322)));
//G586: help_arg(243)        => enable_sym(327).
	p(NT(243), (NT(327)));
//G587: help_arg(243)        => disable_sym(330).
	p(NT(243), (NT(330)));
//G588: help_arg(243)        => toggle_sym(333).
	p(NT(243), (NT(333)));
//G589: help_arg(243)        => file_sym(246).
	p(NT(243), (NT(246)));
//G590: help_arg(243)        => history_sym(345).
	p(NT(243), (NT(345)));
//G591: help_arg(243)        => abs_history_sym(365).
	p(NT(243), (NT(365)));
//G592: help_arg(243)        => rel_history_sym(366).
	p(NT(243), (NT(366)));
//G593: help_arg(243)        => selection_sym(367).
	p(NT(243), (NT(367)));
//G594: help_arg(243)        => def_sym(336).
	p(NT(243), (NT(336)));
//G595: help_arg(243)        => inst_sym(284).
	p(NT(243), (NT(284)));
//G596: help_arg(243)        => subst_sym(279).
	p(NT(243), (NT(279)));
//G597: help_arg(243)        => normalize_sym(275).
	p(NT(243), (NT(275)));
//G598: help_arg(243)        => execute_sym(368).
	p(NT(243), (NT(368)));
//G599: help_arg(243)        => solve_sym(260).
	p(NT(243), (NT(260)));
//G600: help_arg(243)        => valid_sym(250).
	p(NT(243), (NT(250)));
//G601: help_arg(243)        => sat_sym(254).
	p(NT(243), (NT(254)));
//G602: help_arg(243)        => unsat_sym(257).
	p(NT(243), (NT(257)));
//G603: help_arg(243)        => run_sym(269).
	p(NT(243), (NT(269)));
//G604: help_arg(243)        => dnf_sym(289).
	p(NT(243), (NT(289)));
//G605: help_arg(243)        => cnf_sym(292).
	p(NT(243), (NT(292)));
//G606: help_arg(243)        => anf_sym(295).
	p(NT(243), (NT(295)));
//G607: help_arg(243)        => snf_sym(307).
	p(NT(243), (NT(307)));
//G608: help_arg(243)        => nnf_sym(298).
	p(NT(243), (NT(298)));
//G609: help_arg(243)        => pnf_sym(301).
	p(NT(243), (NT(301)));
//G610: help_arg(243)        => mnf_sym(304).
	p(NT(243), (NT(304)));
//G611: help_arg(243)        => onf_sym(310).
	p(NT(243), (NT(310)));
//G612: help_arg(243)        => qelim_sym(314).
	p(NT(243), (NT(314)));
//G613: __E___E_help_arg_160_161(371) => 's'.
	p(NT(371), (T(15)));
//G614: __E___E_help_arg_160_161(371) => null.
	p(NT(371), (nul));
//G615: __E_help_arg_160(370) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_160_161(371).
	p(NT(370), (T(11)+T(29)+T(16)+T(21)+T(53)+T(10)+T(11)+NT(371)));
//G616: examples_sym(369)    => __E_help_arg_160(370).
	p(NT(369), (NT(370)));
//G617: help_arg(243)        => examples_sym(369).
	p(NT(243), (NT(369)));
//G618: __E_history_162(373) => '%'.
	p(NT(373), (T(59)));
//G619: rel_history_sym(366) => '%' '-'.
	p(NT(366), (T(59)+T(30)));
//G620: history_id(375)      => digits(216).
	p(NT(375), (NT(216)));
//G621: __E___E_history_162_163(374) => history_id(375).
	p(NT(374), (NT(375)));
//G622: __E___E_history_162_163(374) => null.
	p(NT(374), (nul));
//G623: __E_history_162(373) => rel_history_sym(366) __E___E_history_162_163(374).
	p(NT(373), (NT(366)+NT(374)));
//G624: rel_history(372)     => __E_history_162(373).
	p(NT(372), (NT(373)));
//G625: history(272)         => rel_history(372).
	p(NT(272), (NT(372)));
//G626: abs_history_sym(365) => '%'.
	p(NT(365), (T(59)));
//G627: __E_history_164(377) => abs_history_sym(365) history_id(375).
	p(NT(377), (NT(365)+NT(375)));
//G628: abs_history(376)     => __E_history_164(377).
	p(NT(376), (NT(377)));
//G629: history(272)         => abs_history(376).
	p(NT(272), (NT(376)));
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
		bf_nlt, __E_wff_51, bf_parenthesis, __E_bf_52, bf_constant, variable, bf_splitter, __E_bf_53, bf_fall, __E_bf_54, 
		bf_fex, __E_bf_55, bf_ref, bf_or, __E_bf_56, bf_xor, __E_bf_57, bf_t, __E_bf_58, __E___E_bf_58_59, 
		bf_f, __E_bf_60, __E___E_bf_60_61, bf_and, __E_bf_62, __E___E_bf_62_63, bf_neg, __E_bf_64, bf_neg_oprnd, __E___E_bf_64_65, 
		bf_and_nosep, __E_bf_66, bf_and_nosep_1st_oprnd, __E___E_bf_66_67, bf_and_nosep_2nd_oprnd, __E___E_bf_66_68, ctn_neq, __E_constraint_69, ctnvar, num, 
		ctn_eq, __E_constraint_70, ctn_gteq, __E_constraint_71, ctn_gt, __E_constraint_72, ctn_lteq, __E_constraint_73, ctn_lt, __E_constraint_74, 
		__E_bf_constant_75, source, __E_bf_constant_76, src_c, __E_source_77, __E___E_source_77_78, __E___E_source_77_79, __E_src_c_80, __E_src_c_81, __E_variable_82, 
		uconst, io_var, var_name, __E_variable_83, __E_var_name_84, __E_var_name_85, __E_var_name_86, __E_var_name_87, __E_var_name_88, offset, 
		chars, uconst_name, __E_uconst_89, __E___E_uconst_89_90, q_var, __E_q_vars_91, __E_q_vars_92, __E_offsets_93, __E_offsets_94, integer, 
		shift, __E_offset_95, __E_shift_96, __E___E_shift_96_97, __E_chars_98, __E_chars_99, digits, __E_digits_100, __E_integer_101, type, 
		scope_id, comment, __E_comment_102, __E_comment_103, __E_comment_104, cli, cli_command, __E_cli_105, __E_cli_106, quit_cmd, 
		quit_sym, __E_cli_command_107, version_cmd, version_sym, __E_cli_command_108, clear_cmd, clear_sym, __E_cli_command_109, help_cmd, __E_cli_command_110, 
		help_sym, __E___E_cli_command_110_111, __E___E_cli_command_110_112, help_arg, file_cmd, __E_cli_command_113, file_sym, q_string, valid_cmd, __E_cli_command_114, 
		valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_115, sat_sym, unsat_cmd, __E_cli_command_116, unsat_sym, solve_cmd, __E_cli_command_117, 
		solve_sym, __E___E_cli_command_117_118, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_119, lgrs_sym, run_cmd, __E_cli_command_120, run_sym, 
		__E___E_cli_command_120_121, __E___E_cli_command_120_122, history, normalize_cmd, __E_cli_command_123, normalize_sym, __E___E_cli_command_123_124, subst_cmd, __E_cli_command_125, subst_sym, 
		__E___E_cli_command_125_126, nf_cmd_arg, inst_cmd, __E_cli_command_127, inst_sym, __E___E_cli_command_127_128, inst_args, dnf_cmd, __E_cli_command_129, dnf_sym, 
		cnf_cmd, __E_cli_command_130, cnf_sym, anf_cmd, __E_cli_command_131, anf_sym, nnf_cmd, __E_cli_command_132, nnf_sym, pnf_cmd, 
		__E_cli_command_133, pnf_sym, mnf_cmd, __E_cli_command_134, mnf_sym, snf_cmd, __E_cli_command_135, snf_sym, onf_cmd, __E_cli_command_136, 
		onf_sym, onf_cmd_arg, qelim_cmd, __E_cli_command_137, qelim_sym, get_cmd, __E_cli_command_138, get_sym, __E___E_cli_command_138_139, option_name, 
		set_cmd, __E_cli_command_140, set_sym, __E___E_cli_command_140_141, option_value, enable_cmd, __E_cli_command_142, enable_sym, disable_cmd, __E_cli_command_143, 
		disable_sym, toggle_cmd, __E_cli_command_144, toggle_sym, def_list_cmd, __E_cli_command_145, def_sym, __E___E_cli_command_145_146, def_print_cmd, __E_cli_command_147, 
		def_rr_cmd, def_input_cmd, def_output_cmd, history_list_cmd, __E_cli_command_148, history_sym, __E___E_cli_command_148_149, history_print_cmd, __E_cli_command_150, history_store_cmd, 
		__E_cli_command_151, __E_solve_options_152, solve_option, __E_solve_options_153, __E_solve_option_154, solver_mode, solver_mode_minimum, __E_solver_mode_155, solver_mode_maximum, __E_solver_mode_156, 
		file_name, __E_file_name_157, __E_option_name_158, __E_option_value_159, bf_cmd_arg, abs_history_sym, rel_history_sym, selection_sym, execute_sym, examples_sym, 
		__E_help_arg_160, __E___E_help_arg_160_161, rel_history, __E_history_162, __E___E_history_162_163, history_id, abs_history, __E_history_164, __N_0, __N_1, 
		__N_2, __N_3, __N_4, __N_5, __N_6, __N_7, __N_8, 
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
