// This file is generated from a file parser/tau.tgf by
//       https://github.com/IDNI/parser/tools/tgf
//
#ifndef __TAU_PARSER_H__
#define __TAU_PARSER_H__

#include "parser.h"

namespace tau_parser_data {

using char_type     = char;
using terminal_type = char;

inline std::vector<std::string> symbol_names{
	"", "eof", "space", "digit", "xdigit", "alpha", "alnum", "punct", "printable", "start", 
	"rr", "_", "rec_relations", "main", "wff", "__E_rec_relations_0", "rec_relation", "__E_rec_relations_1", "ref", "__E_rec_relation_2", 
	"capture", "bf", "sym", "__E_ref_3", "offsets", "ref_args", "__E_ref_4", "__", "fp_fallback", "first_sym", 
	"last_sym", "__E_ref_args_5", "__E___E_ref_args_5_6", "ref_arg", "__E___E_ref_args_5_7", "__E___E_ref_args_5_8", "library", "rules", "__E_library_9", "__E___E_library_9_10", 
	"rule", "__E___E___E_library_9_10_11", "wff_rule", "bf_rule", "__E___E_library_9_12", "wff_matcher", "wff_body", "__E_wff_rule_13", "bf_matcher", "bf_body", 
	"__E_bf_rule_14", "builder", "builder_head", "builder_body", "__E_builder_head_15", "__E_builder_head_16", "bf_builder_body", "__E_builder_body_17", "wff_builder_body", "__E_builder_body_18", 
	"tau_constant_source", "__E_tau_constant_source_19", "wff_parenthesis", "__E_wff_20", "wff_sometimes", "__E_wff_21", "__E___E_wff_21_22", "wff_always", "__E_wff_23", "__E___E_wff_23_24", 
	"wff_conditional", "__E_wff_25", "wff_all", "__E_wff_26", "q_vars", "wff_ex", "__E_wff_27", "wff_ref", "constraint", "wff_imply", 
	"__E_wff_28", "wff_rimply", "__E_wff_29", "wff_equiv", "__E_wff_30", "wff_or", "__E_wff_31", "wff_xor", "__E_wff_32", "wff_and", 
	"__E_wff_33", "wff_neg", "__E_wff_34", "wff_t", "wff_f", "bf_interval", "__E_wff_35", "bf_eq", "__E_wff_36", "bf_neq", 
	"__E_wff_37", "bf_less_equal", "__E_wff_38", "bf_nleq", "__E_wff_39", "bf_greater", "__E_wff_40", "bf_ngreater", "__E_wff_41", "bf_greater_equal", 
	"__E_wff_42", "bf_ngeq", "__E_wff_43", "bf_less", "__E_wff_44", "bf_nless", "__E_wff_45", "bf_parenthesis", "__E_bf_46", "bf_constant", 
	"variable", "bf_splitter", "__E_bf_47", "bf_ref", "bf_or", "__E_bf_48", "bf_xor", "__E_bf_49", "bf_t", "__E_bf_50", 
	"__E___E_bf_50_51", "type", "bf_f", "__E_bf_52", "__E___E_bf_52_53", "bf_and", "__E_bf_54", "__E___E_bf_54_55", "bf_neg", "__E_bf_56", 
	"bf_neg_oprnd", "__E___E_bf_56_57", "bf_and_nosep", "__E_bf_58", "bf_and_nosep_1st_oprnd", "__E___E_bf_58_59", "ctn_neq", "__E_constraint_60", "ctnvar", "num", 
	"ctn_eq", "__E_constraint_61", "ctn_greater_equal", "__E_constraint_62", "ctn_greater", "__E_constraint_63", "ctn_less_equal", "__E_constraint_64", "ctn_less", "__E_constraint_65", 
	"__E_variable_66", "__E_variable_67", "__E_variable_68", "__E_variable_69", "__E_variable_70", "io_var", "__E_variable_71", "in", "out", "uninterpreted_constant", 
	"in_var_name", "__E_in_72", "__E___E_in_72_73", "offset", "out_var_name", "__E_out_74", "__E___E_out_74_75", "chars", "q_var", "__E_q_vars_76", 
	"__E_q_vars_77", "__E_offsets_78", "__E_offsets_79", "shift", "__E_offset_80", "__E_shift_81", "__E___E_shift_81_82", "digits", "uninter_const_name", "__E_uninterpreted_constant_83", 
	"__E___E_uninterpreted_constant_83_84", "constant", "__E_bf_constant_85", "binding", "__E_bf_constant_86", "source", "__E_binding_87", "src_c", "__E___E_binding_87_88", "__E___E___E_binding_87_88_89", 
	"__E___E___E_binding_87_88_90", "__E_src_c_91", "__E_src_c_92", "__E_chars_93", "__E_chars_94", "__E_digits_95", "comment", "__E_comment_96", "__E_comment_97", "__E_comment_98", 
	"__E____99", "__E___100", "cli", "cli_command", "__E_cli_101", "__E_cli_102", "quit_cmd", "quit_sym", "__E_cli_command_103", "version_cmd", 
	"version_sym", "__E_cli_command_104", "clear_cmd", "clear_sym", "__E_cli_command_105", "help_cmd", "__E_cli_command_106", "help_sym", "__E___E_cli_command_106_107", "__E___E_cli_command_106_108", 
	"help_arg", "file_cmd", "__E_cli_command_109", "file_sym", "q_string", "valid_cmd", "__E_cli_command_110", "valid_sym", "normalize_cmd_arg", "sat_cmd", 
	"__E_cli_command_111", "sat_sym", "unsat_cmd", "__E_cli_command_112", "unsat_sym", "solve_cmd", "__E_cli_command_113", "solve_sym", "__E___E_cli_command_113_114", "solve_options", 
	"solve_cmd_arg", "lgrs_cmd", "__E_cli_command_115", "lgrs_sym", "__E___E_cli_command_115_116", "wff_cmd_arg", "run_cmd", "__E_cli_command_117", "run_sym", "__E___E_cli_command_117_118", 
	"__E___E_cli_command_117_119", "memory", "normalize_cmd", "__E_cli_command_120", "normalize_sym", "__E___E_cli_command_120_121", "subst_cmd", "__E_cli_command_122", "subst_sym", "__E___E_cli_command_122_123", 
	"nf_cmd_arg", "inst_cmd", "__E_cli_command_124", "inst_sym", "__E___E_cli_command_124_125", "inst_args", "dnf_cmd", "__E_cli_command_126", "dnf_sym", "cnf_cmd", 
	"__E_cli_command_127", "cnf_sym", "anf_cmd", "__E_cli_command_128", "anf_sym", "nnf_cmd", "__E_cli_command_129", "nnf_sym", "pnf_cmd", "__E_cli_command_130", 
	"pnf_sym", "mnf_cmd", "__E_cli_command_131", "mnf_sym", "snf_cmd", "__E_cli_command_132", "snf_sym", "onf_cmd", "__E_cli_command_133", "onf_sym", 
	"onf_cmd_arg", "def_list_cmd", "__E_cli_command_134", "def_sym", "__E___E_cli_command_134_135", "def_print_cmd", "__E_cli_command_136", "number", "def_rr_cmd", "qelim_cmd", 
	"__E_cli_command_137", "qelim_sym", "get_cmd", "__E_cli_command_138", "get_sym", "__E___E_cli_command_138_139", "option_name", "set_cmd", "__E_cli_command_140", "set_sym", 
	"__E___E_cli_command_140_141", "option_value", "enable_cmd", "__E_cli_command_142", "enable_sym", "disable_cmd", "__E_cli_command_143", "disable_sym", "toggle_cmd", "__E_cli_command_144", 
	"toggle_sym", "def_input_cmd", "__E_cli_command_145", "input_stream", "def_output_cmd", "__E_cli_command_146", "output_stream", "history_list_cmd", "__E_cli_command_147", "history_sym", 
	"__E___E_cli_command_147_148", "history_print_cmd", "__E_cli_command_149", "history_store_cmd", "__E_cli_command_150", "__E_solve_options_151", "solve_option", "__E_solve_options_152", "__E_solve_option_153", "solver_mode", 
	"__E_solve_option_154", "solver_mode_minimum", "__E_solver_mode_155", "solver_mode_maximum", "__E_solver_mode_156", "__E_input_stream_157", "q_file_name", "console_sym", "__E_output_stream_158", "file_name", 
	"__E_file_name_159", "__E_option_name_160", "__E_option_value_161", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "execute_sym", "examples_sym", "__E_help_arg_162", 
	"__E___E_help_arg_162_163", "rel_memory", "__E_memory_164", "__E___E_memory_164_165", "memory_id", "abs_memory", "__E_memory_166", "named_binding", "__N_0", "__N_1", 
	"__N_2", "__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'f', 'a', 'l', 'b', 'c', 
	'k', 'i', 'r', 's', 't', ',', '(', ')', 'o', 'm', 
	'e', '<', '>', 'w', 'y', '[', ']', '?', 'x', '-', 
	'|', '^', '&', '!', 'T', 'F', 'S', '+', '1', '0', 
	'\'', '_', '$', '{', '}', '\t', '\n', '\r', '#', 'q', 
	'u', 'v', 'n', 'h', 'p', 'd', 'g', 'z', '/', '"', 
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
			11, 27
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			20, 22, 120, 131, 148, 170, 174, 187, 188, 195,
			316, 321, 359, 377
		},
		.to_inline = {
			{ 14, 62, 14 },
			{ 21, 117, 21 },
			{ 140, 117, 21 },
			{ 142, 117, 135 },
			{ 144, 117, 21 },
			{ 177 },
			{ 178 },
			{ 197 },
			{ 238 },
			{ 255 },
			{ 270 },
			{ 275 },
			{ 300 },
			{ 363 }
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
//G0:   start(9)             => rr(10) _(11).
	p(NT(9), (NT(10)+NT(11)));
//G1:   main(13)             => wff(14).
	p(NT(13), (NT(14)));
//G2:   rr(10)               => rec_relations(12) _(11) main(13) _(11) '.'.
	p(NT(10), (NT(12)+NT(11)+NT(13)+NT(11)+T(1)));
//G3:   __E_rec_relations_0(15) => _(11) rec_relation(16) _(11) '.'.
	p(NT(15), (NT(11)+NT(16)+NT(11)+T(1)));
//G4:   __E_rec_relations_1(17) => null.
	p(NT(17), (nul));
//G5:   __E_rec_relations_1(17) => __E_rec_relations_0(15) __E_rec_relations_1(17).
	p(NT(17), (NT(15)+NT(17)));
//G6:   rec_relations(12)    => __E_rec_relations_1(17).
	p(NT(12), (NT(17)));
//G7:   __E_rec_relation_2(19) => capture(20).
	p(NT(19), (NT(20)));
//G8:   __E_rec_relation_2(19) => ref(18).
	p(NT(19), (NT(18)));
//G9:   __E_rec_relation_2(19) => wff(14).
	p(NT(19), (NT(14)));
//G10:  __E_rec_relation_2(19) => bf(21).
	p(NT(19), (NT(21)));
//G11:  rec_relation(16)     => ref(18) _(11) ':' '=' _(11) __E_rec_relation_2(19).
	p(NT(16), (NT(18)+NT(11)+T(2)+T(3)+NT(11)+NT(19)));
//G12:  __E_ref_3(23)        => _(11) offsets(24).
	p(NT(23), (NT(11)+NT(24)));
//G13:  __E_ref_3(23)        => null.
	p(NT(23), (nul));
//G14:  __E_ref_4(26)        => _(11) 'f' 'a' 'l' 'l' 'b' 'a' 'c' 'k' __(27) fp_fallback(28).
	p(NT(26), (NT(11)+T(4)+T(5)+T(6)+T(6)+T(7)+T(5)+T(8)+T(9)+NT(27)+NT(28)));
//G15:  __E_ref_4(26)        => null.
	p(NT(26), (nul));
//G16:  ref(18)              => sym(22) __E_ref_3(23) _(11) ref_args(25) __E_ref_4(26).
	p(NT(18), (NT(22)+NT(23)+NT(11)+NT(25)+NT(26)));
//G17:  first_sym(29)        => 'f' 'i' 'r' 's' 't'.
	p(NT(29), (T(4)+T(10)+T(11)+T(12)+T(13)));
//G18:  fp_fallback(28)      => first_sym(29).
	p(NT(28), (NT(29)));
//G19:  last_sym(30)         => 'l' 'a' 's' 't'.
	p(NT(30), (T(6)+T(5)+T(12)+T(13)));
//G20:  fp_fallback(28)      => last_sym(30).
	p(NT(28), (NT(30)));
//G21:  fp_fallback(28)      => capture(20).
	p(NT(28), (NT(20)));
//G22:  fp_fallback(28)      => ref(18).
	p(NT(28), (NT(18)));
//G23:  fp_fallback(28)      => wff(14).
	p(NT(28), (NT(14)));
//G24:  fp_fallback(28)      => bf(21).
	p(NT(28), (NT(21)));
//G25:  ref_arg(33)          => bf(21).
	p(NT(33), (NT(21)));
//G26:  __E___E_ref_args_5_6(32) => _(11) ref_arg(33).
	p(NT(32), (NT(11)+NT(33)));
//G27:  __E___E_ref_args_5_7(34) => _(11) ',' _(11) ref_arg(33).
	p(NT(34), (NT(11)+T(14)+NT(11)+NT(33)));
//G28:  __E___E_ref_args_5_8(35) => null.
	p(NT(35), (nul));
//G29:  __E___E_ref_args_5_8(35) => __E___E_ref_args_5_7(34) __E___E_ref_args_5_8(35).
	p(NT(35), (NT(34)+NT(35)));
//G30:  __E_ref_args_5(31)   => __E___E_ref_args_5_6(32) __E___E_ref_args_5_8(35).
	p(NT(31), (NT(32)+NT(35)));
//G31:  __E_ref_args_5(31)   => null.
	p(NT(31), (nul));
//G32:  ref_args(25)         => '(' __E_ref_args_5(31) _(11) ')'.
	p(NT(25), (T(15)+NT(31)+NT(11)+T(16)));
//G33:  __E___E___E_library_9_10_11(41) => wff_rule(42).
	p(NT(41), (NT(42)));
//G34:  __E___E___E_library_9_10_11(41) => bf_rule(43).
	p(NT(41), (NT(43)));
//G35:  rule(40)             => __E___E___E_library_9_10_11(41).
	p(NT(40), (NT(41)));
//G36:  __E___E_library_9_10(39) => _(11) rule(40).
	p(NT(39), (NT(11)+NT(40)));
//G37:  __E___E_library_9_12(44) => null.
	p(NT(44), (nul));
//G38:  __E___E_library_9_12(44) => __E___E_library_9_10(39) __E___E_library_9_12(44).
	p(NT(44), (NT(39)+NT(44)));
//G39:  __E_library_9(38)    => __E___E_library_9_12(44).
	p(NT(38), (NT(44)));
//G40:  rules(37)            => __E_library_9(38).
	p(NT(37), (NT(38)));
//G41:  library(36)          => rules(37).
	p(NT(36), (NT(37)));
//G42:  wff_matcher(45)      => wff(14).
	p(NT(45), (NT(14)));
//G43:  __E_wff_rule_13(47)  => wff(14).
	p(NT(47), (NT(14)));
//G44:  wff_body(46)         => __E_wff_rule_13(47).
	p(NT(46), (NT(47)));
//G45:  wff_rule(42)         => wff_matcher(45) _(11) ':' ':' '=' _(11) wff_body(46) _(11) '.'.
	p(NT(42), (NT(45)+NT(11)+T(2)+T(2)+T(3)+NT(11)+NT(46)+NT(11)+T(1)));
//G46:  bf_matcher(48)       => bf(21).
	p(NT(48), (NT(21)));
//G47:  __E_bf_rule_14(50)   => bf(21).
	p(NT(50), (NT(21)));
//G48:  bf_body(49)          => __E_bf_rule_14(50).
	p(NT(49), (NT(50)));
//G49:  bf_rule(43)          => bf_matcher(48) _(11) ':' '=' _(11) bf_body(49) _(11) '.'.
	p(NT(43), (NT(48)+NT(11)+T(2)+T(3)+NT(11)+NT(49)+NT(11)+T(1)));
//G50:  builder(51)          => _(11) builder_head(52) _(11) builder_body(53) _(11) '.'.
	p(NT(51), (NT(11)+NT(52)+NT(11)+NT(53)+NT(11)+T(1)));
//G51:  __E_builder_head_15(54) => __(27) capture(20).
	p(NT(54), (NT(27)+NT(20)));
//G52:  __E_builder_head_16(55) => null.
	p(NT(55), (nul));
//G53:  __E_builder_head_16(55) => __E_builder_head_15(54) __E_builder_head_16(55).
	p(NT(55), (NT(54)+NT(55)));
//G54:  builder_head(52)     => '(' _(11) capture(20) __E_builder_head_16(55) _(11) ')'.
	p(NT(52), (T(15)+NT(11)+NT(20)+NT(55)+NT(11)+T(16)));
//G55:  __E_builder_body_17(57) => '=' ':' _(11) bf(21).
	p(NT(57), (T(3)+T(2)+NT(11)+NT(21)));
//G56:  bf_builder_body(56)  => __E_builder_body_17(57).
	p(NT(56), (NT(57)));
//G57:  builder_body(53)     => bf_builder_body(56).
	p(NT(53), (NT(56)));
//G58:  __E_builder_body_18(59) => '=' ':' ':' _(11) wff(14).
	p(NT(59), (T(3)+T(2)+T(2)+NT(11)+NT(14)));
//G59:  wff_builder_body(58) => __E_builder_body_18(59).
	p(NT(58), (NT(59)));
//G60:  builder_body(53)     => wff_builder_body(58).
	p(NT(53), (NT(58)));
//G61:  __E_tau_constant_source_19(61) => '.' _(11).
	p(NT(61), (T(1)+NT(11)));
//G62:  __E_tau_constant_source_19(61) => null.
	p(NT(61), (nul));
//G63:  tau_constant_source(60) => rec_relations(12) _(11) main(13) _(11) __E_tau_constant_source_19(61).
	p(NT(60), (NT(12)+NT(11)+NT(13)+NT(11)+NT(61)));
//G64:  __E_wff_20(63)       => '(' _(11) wff(14) _(11) ')'.
	p(NT(63), (T(15)+NT(11)+NT(14)+NT(11)+T(16)));
//G65:  wff_parenthesis(62)  => __E_wff_20(63).
	p(NT(62), (NT(63)));
//G66:  wff(14)              => wff_parenthesis(62).
	p(NT(14), (NT(62)));
//G67:  __E___E_wff_21_22(66) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(66), (T(12)+T(17)+T(18)+T(19)+T(13)+T(10)+T(18)+T(19)+T(12)));
//G68:  __E___E_wff_21_22(66) => '<' '>'.
	p(NT(66), (T(20)+T(21)));
//G69:  __E_wff_21(65)       => __E___E_wff_21_22(66) _(11) wff(14).
	p(NT(65), (NT(66)+NT(11)+NT(14)));
//G70:  wff_sometimes(64)    => __E_wff_21(65).
	p(NT(64), (NT(65)));
//G71:  wff(14)              => wff_sometimes(64).
	p(NT(14), (NT(64)));
//G72:  __E___E_wff_23_24(69) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(69), (T(5)+T(6)+T(22)+T(5)+T(23)+T(12)));
//G73:  __E___E_wff_23_24(69) => '[' ']'.
	p(NT(69), (T(24)+T(25)));
//G74:  __E_wff_23(68)       => __E___E_wff_23_24(69) _(11) wff(14).
	p(NT(68), (NT(69)+NT(11)+NT(14)));
//G75:  wff_always(67)       => __E_wff_23(68).
	p(NT(67), (NT(68)));
//G76:  wff(14)              => wff_always(67).
	p(NT(14), (NT(67)));
//G77:  __E_wff_25(71)       => wff(14) _(11) '?' _(11) wff(14) _(11) ':' _(11) wff(14).
	p(NT(71), (NT(14)+NT(11)+T(26)+NT(11)+NT(14)+NT(11)+T(2)+NT(11)+NT(14)));
//G78:  wff_conditional(70)  => __E_wff_25(71).
	p(NT(70), (NT(71)));
//G79:  wff(14)              => wff_conditional(70).
	p(NT(14), (NT(70)));
//G80:  __E_wff_26(73)       => 'a' 'l' 'l' __(27) q_vars(74) __(27) wff(14).
	p(NT(73), (T(5)+T(6)+T(6)+NT(27)+NT(74)+NT(27)+NT(14)));
//G81:  wff_all(72)          => __E_wff_26(73).
	p(NT(72), (NT(73)));
//G82:  wff(14)              => wff_all(72).
	p(NT(14), (NT(72)));
//G83:  __E_wff_27(76)       => 'e' 'x' __(27) q_vars(74) __(27) wff(14).
	p(NT(76), (T(19)+T(27)+NT(27)+NT(74)+NT(27)+NT(14)));
//G84:  wff_ex(75)           => __E_wff_27(76).
	p(NT(75), (NT(76)));
//G85:  wff(14)              => wff_ex(75).
	p(NT(14), (NT(75)));
//G86:  wff_ref(77)          => ref(18).
	p(NT(77), (NT(18)));
//G87:  wff(14)              => wff_ref(77).
	p(NT(14), (NT(77)));
//G88:  wff(14)              => constraint(78).
	p(NT(14), (NT(78)));
//G89:  __E_wff_28(80)       => wff(14) _(11) '-' '>' _(11) wff(14).
	p(NT(80), (NT(14)+NT(11)+T(28)+T(21)+NT(11)+NT(14)));
//G90:  wff_imply(79)        => __E_wff_28(80).
	p(NT(79), (NT(80)));
//G91:  wff(14)              => wff_imply(79).
	p(NT(14), (NT(79)));
//G92:  __E_wff_29(82)       => wff(14) _(11) '<' '-' _(11) wff(14).
	p(NT(82), (NT(14)+NT(11)+T(20)+T(28)+NT(11)+NT(14)));
//G93:  wff_rimply(81)       => __E_wff_29(82).
	p(NT(81), (NT(82)));
//G94:  wff(14)              => wff_rimply(81).
	p(NT(14), (NT(81)));
//G95:  __E_wff_30(84)       => wff(14) _(11) '<' '-' '>' _(11) wff(14).
	p(NT(84), (NT(14)+NT(11)+T(20)+T(28)+T(21)+NT(11)+NT(14)));
//G96:  wff_equiv(83)        => __E_wff_30(84).
	p(NT(83), (NT(84)));
//G97:  wff(14)              => wff_equiv(83).
	p(NT(14), (NT(83)));
//G98:  __E_wff_31(86)       => wff(14) _(11) '|' '|' _(11) wff(14).
	p(NT(86), (NT(14)+NT(11)+T(29)+T(29)+NT(11)+NT(14)));
//G99:  wff_or(85)           => __E_wff_31(86).
	p(NT(85), (NT(86)));
//G100: wff(14)              => wff_or(85).
	p(NT(14), (NT(85)));
//G101: __E_wff_32(88)       => wff(14) _(11) '^' _(11) wff(14).
	p(NT(88), (NT(14)+NT(11)+T(30)+NT(11)+NT(14)));
//G102: wff_xor(87)          => __E_wff_32(88).
	p(NT(87), (NT(88)));
//G103: wff(14)              => wff_xor(87).
	p(NT(14), (NT(87)));
//G104: __E_wff_33(90)       => wff(14) _(11) '&' '&' _(11) wff(14).
	p(NT(90), (NT(14)+NT(11)+T(31)+T(31)+NT(11)+NT(14)));
//G105: wff_and(89)          => __E_wff_33(90).
	p(NT(89), (NT(90)));
//G106: wff(14)              => wff_and(89).
	p(NT(14), (NT(89)));
//G107: __E_wff_34(92)       => '!' _(11) wff(14).
	p(NT(92), (T(32)+NT(11)+NT(14)));
//G108: wff_neg(91)          => __E_wff_34(92).
	p(NT(91), (NT(92)));
//G109: wff(14)              => wff_neg(91).
	p(NT(14), (NT(91)));
//G110: wff_t(93)            => 'T'.
	p(NT(93), (T(33)));
//G111: wff(14)              => wff_t(93).
	p(NT(14), (NT(93)));
//G112: wff_f(94)            => 'F'.
	p(NT(94), (T(34)));
//G113: wff(14)              => wff_f(94).
	p(NT(14), (NT(94)));
//G114: __E_wff_35(96)       => bf(21) _(11) '<' '=' _(11) bf(21) _(11) '<' '=' _(11) bf(21).
	p(NT(96), (NT(21)+NT(11)+T(20)+T(3)+NT(11)+NT(21)+NT(11)+T(20)+T(3)+NT(11)+NT(21)));
//G115: bf_interval(95)      => __E_wff_35(96).
	p(NT(95), (NT(96)));
//G116: wff(14)              => bf_interval(95).
	p(NT(14), (NT(95)));
//G117: __E_wff_36(98)       => bf(21) _(11) '=' _(11) bf(21).
	p(NT(98), (NT(21)+NT(11)+T(3)+NT(11)+NT(21)));
//G118: bf_eq(97)            => __E_wff_36(98).
	p(NT(97), (NT(98)));
//G119: wff(14)              => bf_eq(97).
	p(NT(14), (NT(97)));
//G120: __E_wff_37(100)      => bf(21) _(11) '!' '=' _(11) bf(21).
	p(NT(100), (NT(21)+NT(11)+T(32)+T(3)+NT(11)+NT(21)));
//G121: bf_neq(99)           => __E_wff_37(100).
	p(NT(99), (NT(100)));
//G122: wff(14)              => bf_neq(99).
	p(NT(14), (NT(99)));
//G123: __E_wff_38(102)      => bf(21) _(11) '<' '=' _(11) bf(21).
	p(NT(102), (NT(21)+NT(11)+T(20)+T(3)+NT(11)+NT(21)));
//G124: bf_less_equal(101)   => __E_wff_38(102).
	p(NT(101), (NT(102)));
//G125: wff(14)              => bf_less_equal(101).
	p(NT(14), (NT(101)));
//G126: __E_wff_39(104)      => bf(21) _(11) '!' '<' '=' _(11) bf(21).
	p(NT(104), (NT(21)+NT(11)+T(32)+T(20)+T(3)+NT(11)+NT(21)));
//G127: bf_nleq(103)         => __E_wff_39(104).
	p(NT(103), (NT(104)));
//G128: wff(14)              => bf_nleq(103).
	p(NT(14), (NT(103)));
//G129: __E_wff_40(106)      => bf(21) _(11) '>' _(11) bf(21).
	p(NT(106), (NT(21)+NT(11)+T(21)+NT(11)+NT(21)));
//G130: bf_greater(105)      => __E_wff_40(106).
	p(NT(105), (NT(106)));
//G131: wff(14)              => bf_greater(105).
	p(NT(14), (NT(105)));
//G132: __E_wff_41(108)      => bf(21) _(11) '!' '>' _(11) bf(21).
	p(NT(108), (NT(21)+NT(11)+T(32)+T(21)+NT(11)+NT(21)));
//G133: bf_ngreater(107)     => __E_wff_41(108).
	p(NT(107), (NT(108)));
//G134: wff(14)              => bf_ngreater(107).
	p(NT(14), (NT(107)));
//G135: __E_wff_42(110)      => bf(21) _(11) '>' '=' _(11) bf(21).
	p(NT(110), (NT(21)+NT(11)+T(21)+T(3)+NT(11)+NT(21)));
//G136: bf_greater_equal(109) => __E_wff_42(110).
	p(NT(109), (NT(110)));
//G137: wff(14)              => bf_greater_equal(109).
	p(NT(14), (NT(109)));
//G138: __E_wff_43(112)      => bf(21) _(11) '!' '>' '=' _(11) bf(21).
	p(NT(112), (NT(21)+NT(11)+T(32)+T(21)+T(3)+NT(11)+NT(21)));
//G139: bf_ngeq(111)         => __E_wff_43(112).
	p(NT(111), (NT(112)));
//G140: wff(14)              => bf_ngeq(111).
	p(NT(14), (NT(111)));
//G141: __E_wff_44(114)      => bf(21) _(11) '<' _(11) bf(21).
	p(NT(114), (NT(21)+NT(11)+T(20)+NT(11)+NT(21)));
//G142: bf_less(113)         => __E_wff_44(114).
	p(NT(113), (NT(114)));
//G143: wff(14)              => bf_less(113).
	p(NT(14), (NT(113)));
//G144: __E_wff_45(116)      => bf(21) _(11) '!' '<' _(11) bf(21).
	p(NT(116), (NT(21)+NT(11)+T(32)+T(20)+NT(11)+NT(21)));
//G145: bf_nless(115)        => __E_wff_45(116).
	p(NT(115), (NT(116)));
//G146: wff(14)              => bf_nless(115).
	p(NT(14), (NT(115)));
//G147: wff(14)              => capture(20).
	p(NT(14), (NT(20)));
//G148: __E_bf_46(118)       => '(' _(11) bf(21) _(11) ')'.
	p(NT(118), (T(15)+NT(11)+NT(21)+NT(11)+T(16)));
//G149: bf_parenthesis(117)  => __E_bf_46(118).
	p(NT(117), (NT(118)));
//G150: bf(21)               => bf_parenthesis(117).
	p(NT(21), (NT(117)));
//G151: bf(21)               => bf_constant(119).
	p(NT(21), (NT(119)));
//G152: bf(21)               => variable(120).
	p(NT(21), (NT(120)));
//G153: __E_bf_47(122)       => 'S' _(11) '(' _(11) bf(21) _(11) ')'.
	p(NT(122), (T(35)+NT(11)+T(15)+NT(11)+NT(21)+NT(11)+T(16)));
//G154: bf_splitter(121)     => __E_bf_47(122).
	p(NT(121), (NT(122)));
//G155: bf(21)               => bf_splitter(121).
	p(NT(21), (NT(121)));
//G156: bf_ref(123)          => ref(18).
	p(NT(123), (NT(18)));
//G157: bf(21)               => bf_ref(123).
	p(NT(21), (NT(123)));
//G158: __E_bf_48(125)       => bf(21) _(11) '|' _(11) bf(21).
	p(NT(125), (NT(21)+NT(11)+T(29)+NT(11)+NT(21)));
//G159: bf_or(124)           => __E_bf_48(125).
	p(NT(124), (NT(125)));
//G160: bf(21)               => bf_or(124).
	p(NT(21), (NT(124)));
//G161: __E_bf_49(127)       => bf(21) _(11) '+' _(11) bf(21).
	p(NT(127), (NT(21)+NT(11)+T(36)+NT(11)+NT(21)));
//G162: bf_xor(126)          => __E_bf_49(127).
	p(NT(126), (NT(127)));
//G163: bf(21)               => bf_xor(126).
	p(NT(21), (NT(126)));
//G164: __E___E_bf_50_51(130) => _(11) ':' _(11) type(131).
	p(NT(130), (NT(11)+T(2)+NT(11)+NT(131)));
//G165: __E___E_bf_50_51(130) => null.
	p(NT(130), (nul));
//G166: __E_bf_50(129)       => '1' __E___E_bf_50_51(130).
	p(NT(129), (T(37)+NT(130)));
//G167: bf_t(128)            => __E_bf_50(129).
	p(NT(128), (NT(129)));
//G168: bf(21)               => bf_t(128).
	p(NT(21), (NT(128)));
//G169: __E___E_bf_52_53(134) => _(11) ':' _(11) type(131).
	p(NT(134), (NT(11)+T(2)+NT(11)+NT(131)));
//G170: __E___E_bf_52_53(134) => null.
	p(NT(134), (nul));
//G171: __E_bf_52(133)       => '0' __E___E_bf_52_53(134).
	p(NT(133), (T(38)+NT(134)));
//G172: bf_f(132)            => __E_bf_52(133).
	p(NT(132), (NT(133)));
//G173: bf(21)               => bf_f(132).
	p(NT(21), (NT(132)));
//G174: __E___E_bf_54_55(137) => _(11) '&' _(11).
	p(NT(137), (NT(11)+T(31)+NT(11)));
//G175: __E___E_bf_54_55(137) => __(27).
	p(NT(137), (NT(27)));
//G176: __E_bf_54(136)       => bf(21) __E___E_bf_54_55(137) bf(21).
	p(NT(136), (NT(21)+NT(137)+NT(21)));
//G177: bf_and(135)          => __E_bf_54(136).
	p(NT(135), (NT(136)));
//G178: bf(21)               => bf_and(135).
	p(NT(21), (NT(135)));
//G179: __E___E_bf_56_57(141) => bf_parenthesis(117).
	p(NT(141), (NT(117)));
//G180: __E___E_bf_56_57(141) => bf_constant(119).
	p(NT(141), (NT(119)));
//G181: __E___E_bf_56_57(141) => variable(120).
	p(NT(141), (NT(120)));
//G182: __E___E_bf_56_57(141) => bf_splitter(121).
	p(NT(141), (NT(121)));
//G183: __E___E_bf_56_57(141) => bf_ref(123).
	p(NT(141), (NT(123)));
//G184: __E___E_bf_56_57(141) => bf_t(128).
	p(NT(141), (NT(128)));
//G185: __E___E_bf_56_57(141) => bf_f(132).
	p(NT(141), (NT(132)));
//G186: __E___E_bf_56_57(141) => bf_neg(138).
	p(NT(141), (NT(138)));
//G187: __E___E_bf_56_57(141) => capture(20).
	p(NT(141), (NT(20)));
//G188: bf_neg_oprnd(140)    => __E___E_bf_56_57(141).
	p(NT(140), (NT(141)));
//G189: __E_bf_56(139)       => bf_neg_oprnd(140) _(11) '\''.
	p(NT(139), (NT(140)+NT(11)+T(39)));
//G190: bf_neg(138)          => __E_bf_56(139).
	p(NT(138), (NT(139)));
//G191: bf(21)               => bf_neg(138).
	p(NT(21), (NT(138)));
//G192: __E___E_bf_58_59(145) => bf_parenthesis(117).
	p(NT(145), (NT(117)));
//G193: __E___E_bf_58_59(145) => variable(120).
	p(NT(145), (NT(120)));
//G194: __E___E_bf_58_59(145) => bf_splitter(121).
	p(NT(145), (NT(121)));
//G195: __E___E_bf_58_59(145) => bf_ref(123).
	p(NT(145), (NT(123)));
//G196: __E___E_bf_58_59(145) => bf_or(124).
	p(NT(145), (NT(124)));
//G197: __E___E_bf_58_59(145) => bf_xor(126).
	p(NT(145), (NT(126)));
//G198: __E___E_bf_58_59(145) => bf_neg(138).
	p(NT(145), (NT(138)));
//G199: __E___E_bf_58_59(145) => capture(20).
	p(NT(145), (NT(20)));
//G200: bf_and_nosep_1st_oprnd(144) => __E___E_bf_58_59(145).
	p(NT(144), (NT(145)));
//G201: __E_bf_58(143)       => bf_and_nosep_1st_oprnd(144) bf(21).
	p(NT(143), (NT(144)+NT(21)));
//G202: bf_and_nosep(142)    => __E_bf_58(143).
	p(NT(142), (NT(143)));
//G203: bf(21)               => bf_and_nosep(142).
	p(NT(21), (NT(142)));
//G204: bf(21)               => capture(20).
	p(NT(21), (NT(20)));
//G205: __E_constraint_60(147) => '[' ctnvar(148) _(11) '!' '=' _(11) num(149) ']'.
	p(NT(147), (T(24)+NT(148)+NT(11)+T(32)+T(3)+NT(11)+NT(149)+T(25)));
//G206: __E_constraint_60(147) => '[' num(149) _(11) '!' '=' _(11) ctnvar(148) ']'.
	p(NT(147), (T(24)+NT(149)+NT(11)+T(32)+T(3)+NT(11)+NT(148)+T(25)));
//G207: ctn_neq(146)         => __E_constraint_60(147).
	p(NT(146), (NT(147)));
//G208: constraint(78)       => ctn_neq(146).
	p(NT(78), (NT(146)));
//G209: __E_constraint_61(151) => '[' ctnvar(148) _(11) '=' _(11) num(149) ']'.
	p(NT(151), (T(24)+NT(148)+NT(11)+T(3)+NT(11)+NT(149)+T(25)));
//G210: __E_constraint_61(151) => '[' num(149) _(11) '=' _(11) ctnvar(148) ']'.
	p(NT(151), (T(24)+NT(149)+NT(11)+T(3)+NT(11)+NT(148)+T(25)));
//G211: ctn_eq(150)          => __E_constraint_61(151).
	p(NT(150), (NT(151)));
//G212: constraint(78)       => ctn_eq(150).
	p(NT(78), (NT(150)));
//G213: __E_constraint_62(153) => '[' ctnvar(148) _(11) '>' '=' _(11) num(149) ']'.
	p(NT(153), (T(24)+NT(148)+NT(11)+T(21)+T(3)+NT(11)+NT(149)+T(25)));
//G214: __E_constraint_62(153) => '[' num(149) _(11) '>' '=' _(11) ctnvar(148) ']'.
	p(NT(153), (T(24)+NT(149)+NT(11)+T(21)+T(3)+NT(11)+NT(148)+T(25)));
//G215: ctn_greater_equal(152) => __E_constraint_62(153).
	p(NT(152), (NT(153)));
//G216: constraint(78)       => ctn_greater_equal(152).
	p(NT(78), (NT(152)));
//G217: __E_constraint_63(155) => '[' ctnvar(148) _(11) '>' _(11) num(149) ']'.
	p(NT(155), (T(24)+NT(148)+NT(11)+T(21)+NT(11)+NT(149)+T(25)));
//G218: __E_constraint_63(155) => '[' num(149) _(11) '>' _(11) ctnvar(148) ']'.
	p(NT(155), (T(24)+NT(149)+NT(11)+T(21)+NT(11)+NT(148)+T(25)));
//G219: ctn_greater(154)     => __E_constraint_63(155).
	p(NT(154), (NT(155)));
//G220: constraint(78)       => ctn_greater(154).
	p(NT(78), (NT(154)));
//G221: __E_constraint_64(157) => '[' ctnvar(148) _(11) '<' '=' _(11) num(149) ']'.
	p(NT(157), (T(24)+NT(148)+NT(11)+T(20)+T(3)+NT(11)+NT(149)+T(25)));
//G222: __E_constraint_64(157) => '[' num(149) _(11) '<' '=' _(11) ctnvar(148) ']'.
	p(NT(157), (T(24)+NT(149)+NT(11)+T(20)+T(3)+NT(11)+NT(148)+T(25)));
//G223: ctn_less_equal(156)  => __E_constraint_64(157).
	p(NT(156), (NT(157)));
//G224: constraint(78)       => ctn_less_equal(156).
	p(NT(78), (NT(156)));
//G225: __E_constraint_65(159) => '[' ctnvar(148) _(11) '<' _(11) num(149) ']'.
	p(NT(159), (T(24)+NT(148)+NT(11)+T(20)+NT(11)+NT(149)+T(25)));
//G226: __E_constraint_65(159) => '[' num(149) _(11) '<' _(11) ctnvar(148) ']'.
	p(NT(159), (T(24)+NT(149)+NT(11)+T(20)+NT(11)+NT(148)+T(25)));
//G227: ctn_less(158)        => __E_constraint_65(159).
	p(NT(158), (NT(159)));
//G228: constraint(78)       => ctn_less(158).
	p(NT(78), (NT(158)));
//G229: __N_0(378)           => 'F'.
	p(NT(378), (T(34)));
//G230: __N_1(379)           => 'T'.
	p(NT(379), (T(33)));
//G231: __E_variable_66(160) => ~( __N_0(378) ) & ~( __N_1(379) ) & alpha(5).	 # conjunctive
	p(NT(160), ~(NT(378)) & ~(NT(379)) & (NT(5)));
//G232: __E_variable_67(161) => null.
	p(NT(161), (nul));
//G233: __E_variable_67(161) => digit(3) __E_variable_67(161).
	p(NT(161), (NT(3)+NT(161)));
//G234: variable(120)        => __E_variable_66(160) __E_variable_67(161).	 # guarded: charvar
	p(NT(120), (NT(160)+NT(161)));
	p.back().guard = "charvar";
//G235: __N_2(380)           => 'F'.
	p(NT(380), (T(34)));
//G236: __N_3(381)           => 'T'.
	p(NT(381), (T(33)));
//G237: __E_variable_68(162) => ~( __N_2(380) ) & ~( __N_3(381) ) & alpha(5).	 # conjunctive
	p(NT(162), ~(NT(380)) & ~(NT(381)) & (NT(5)));
//G238: __E_variable_69(163) => alnum(6).
	p(NT(163), (NT(6)));
//G239: __E_variable_69(163) => '_'.
	p(NT(163), (T(40)));
//G240: __E_variable_70(164) => null.
	p(NT(164), (nul));
//G241: __E_variable_70(164) => __E_variable_69(163) __E_variable_70(164).
	p(NT(164), (NT(163)+NT(164)));
//G242: variable(120)        => __E_variable_68(162) __E_variable_70(164).	 # guarded: var
	p(NT(120), (NT(162)+NT(164)));
	p.back().guard = "var";
//G243: __E_variable_71(166) => in(167).
	p(NT(166), (NT(167)));
//G244: __E_variable_71(166) => out(168).
	p(NT(166), (NT(168)));
//G245: io_var(165)          => __E_variable_71(166).
	p(NT(165), (NT(166)));
//G246: variable(120)        => io_var(165).
	p(NT(120), (NT(165)));
//G247: variable(120)        => uninterpreted_constant(169).
	p(NT(120), (NT(169)));
//G248: __E___E_in_72_73(172) => digit(3).
	p(NT(172), (NT(3)));
//G249: __E___E_in_72_73(172) => digit(3) __E___E_in_72_73(172).
	p(NT(172), (NT(3)+NT(172)));
//G250: __E_in_72(171)       => 'i' __E___E_in_72_73(172).
	p(NT(171), (T(10)+NT(172)));
//G251: in_var_name(170)     => __E_in_72(171).
	p(NT(170), (NT(171)));
//G252: in(167)              => in_var_name(170) '[' offset(173) ']'.
	p(NT(167), (NT(170)+T(24)+NT(173)+T(25)));
//G253: __E___E_out_74_75(176) => digit(3).
	p(NT(176), (NT(3)));
//G254: __E___E_out_74_75(176) => digit(3) __E___E_out_74_75(176).
	p(NT(176), (NT(3)+NT(176)));
//G255: __E_out_74(175)      => 'o' __E___E_out_74_75(176).
	p(NT(175), (T(17)+NT(176)));
//G256: out_var_name(174)    => __E_out_74(175).
	p(NT(174), (NT(175)));
//G257: out(168)             => out_var_name(174) '[' offset(173) ']'.
	p(NT(168), (NT(174)+T(24)+NT(173)+T(25)));
//G258: ctnvar(148)          => chars(177).
	p(NT(148), (NT(177)));
//G259: capture(20)          => '$' chars(177).
	p(NT(20), (T(41)+NT(177)));
//G260: __E_q_vars_76(179)   => _(11) ',' _(11) q_var(178).
	p(NT(179), (NT(11)+T(14)+NT(11)+NT(178)));
//G261: __E_q_vars_77(180)   => null.
	p(NT(180), (nul));
//G262: __E_q_vars_77(180)   => __E_q_vars_76(179) __E_q_vars_77(180).
	p(NT(180), (NT(179)+NT(180)));
//G263: q_vars(74)           => q_var(178) __E_q_vars_77(180).
	p(NT(74), (NT(178)+NT(180)));
//G264: q_var(178)           => capture(20).
	p(NT(178), (NT(20)));
//G265: __N_4(382)           => uninterpreted_constant(169).
	p(NT(382), (NT(169)));
//G266: q_var(178)           => variable(120) & ~( __N_4(382) ).	 # conjunctive
	p(NT(178), (NT(120)) & ~(NT(382)));
//G267: __E_offsets_78(181)  => _(11) ',' _(11) offset(173).
	p(NT(181), (NT(11)+T(14)+NT(11)+NT(173)));
//G268: __E_offsets_79(182)  => null.
	p(NT(182), (nul));
//G269: __E_offsets_79(182)  => __E_offsets_78(181) __E_offsets_79(182).
	p(NT(182), (NT(181)+NT(182)));
//G270: offsets(24)          => '[' _(11) offset(173) __E_offsets_79(182) _(11) ']'.
	p(NT(24), (T(24)+NT(11)+NT(173)+NT(182)+NT(11)+T(25)));
//G271: offset(173)          => num(149).
	p(NT(173), (NT(149)));
//G272: offset(173)          => capture(20).
	p(NT(173), (NT(20)));
//G273: offset(173)          => shift(183).
	p(NT(173), (NT(183)));
//G274: __N_5(383)           => io_var(165).
	p(NT(383), (NT(165)));
//G275: __E_offset_80(184)   => variable(120) & ~( __N_5(383) ).	 # conjunctive
	p(NT(184), (NT(120)) & ~(NT(383)));
//G276: offset(173)          => __E_offset_80(184).
	p(NT(173), (NT(184)));
//G277: __E_shift_81(185)    => capture(20).
	p(NT(185), (NT(20)));
//G278: __N_6(384)           => io_var(165).
	p(NT(384), (NT(165)));
//G279: __E___E_shift_81_82(186) => variable(120) & ~( __N_6(384) ).	 # conjunctive
	p(NT(186), (NT(120)) & ~(NT(384)));
//G280: __E_shift_81(185)    => __E___E_shift_81_82(186).
	p(NT(185), (NT(186)));
//G281: shift(183)           => __E_shift_81(185) _(11) '-' _(11) num(149).
	p(NT(183), (NT(185)+NT(11)+T(28)+NT(11)+NT(149)));
//G282: num(149)             => digits(187).
	p(NT(149), (NT(187)));
//G283: __E___E_uninterpreted_constant_83_84(190) => chars(177).
	p(NT(190), (NT(177)));
//G284: __E___E_uninterpreted_constant_83_84(190) => _(11).
	p(NT(190), (NT(11)));
//G285: __E_uninterpreted_constant_83(189) => __E___E_uninterpreted_constant_83_84(190) ':' chars(177).
	p(NT(189), (NT(190)+T(2)+NT(177)));
//G286: uninter_const_name(188) => __E_uninterpreted_constant_83(189).
	p(NT(188), (NT(189)));
//G287: uninterpreted_constant(169) => '<' uninter_const_name(188) _(11) '>'.
	p(NT(169), (T(20)+NT(188)+NT(11)+T(21)));
//G288: __E_bf_constant_85(192) => capture(20).
	p(NT(192), (NT(20)));
//G289: __E_bf_constant_85(192) => binding(193).
	p(NT(192), (NT(193)));
//G290: constant(191)        => __E_bf_constant_85(192).
	p(NT(191), (NT(192)));
//G291: type(131)            => chars(177).
	p(NT(131), (NT(177)));
//G292: __E_bf_constant_86(194) => _(11) ':' _(11) type(131).
	p(NT(194), (NT(11)+T(2)+NT(11)+NT(131)));
//G293: __E_bf_constant_86(194) => null.
	p(NT(194), (nul));
//G294: bf_constant(119)     => '{' _(11) constant(191) _(11) '}' __E_bf_constant_86(194).
	p(NT(119), (T(42)+NT(11)+NT(191)+NT(11)+T(43)+NT(194)));
//G295: __E___E___E_binding_87_88_89(199) => src_c(197).
	p(NT(199), (NT(197)));
//G296: __E___E___E_binding_87_88_89(199) => space(2).
	p(NT(199), (NT(2)));
//G297: __E___E___E_binding_87_88_90(200) => null.
	p(NT(200), (nul));
//G298: __E___E___E_binding_87_88_90(200) => __E___E___E_binding_87_88_89(199) __E___E___E_binding_87_88_90(200).
	p(NT(200), (NT(199)+NT(200)));
//G299: __E___E_binding_87_88(198) => __E___E___E_binding_87_88_90(200) src_c(197).
	p(NT(198), (NT(200)+NT(197)));
//G300: __E___E_binding_87_88(198) => null.
	p(NT(198), (nul));
//G301: __E_binding_87(196)  => src_c(197) __E___E_binding_87_88(198).
	p(NT(196), (NT(197)+NT(198)));
//G302: source(195)          => __E_binding_87(196).
	p(NT(195), (NT(196)));
//G303: binding(193)         => source(195).
	p(NT(193), (NT(195)));
//G304: src_c(197)           => alnum(6).
	p(NT(197), (NT(6)));
//G305: __N_7(385)           => '{'.
	p(NT(385), (T(42)));
//G306: __N_8(386)           => '}'.
	p(NT(386), (T(43)));
//G307: src_c(197)           => ~( __N_7(385) ) & ~( __N_8(386) ) & punct(7).	 # conjunctive
	p(NT(197), ~(NT(385)) & ~(NT(386)) & (NT(7)));
//G308: __E_src_c_91(201)    => src_c(197).
	p(NT(201), (NT(197)));
//G309: __E_src_c_91(201)    => space(2).
	p(NT(201), (NT(2)));
//G310: __E_src_c_92(202)    => null.
	p(NT(202), (nul));
//G311: __E_src_c_92(202)    => __E_src_c_91(201) __E_src_c_92(202).
	p(NT(202), (NT(201)+NT(202)));
//G312: src_c(197)           => '{' __E_src_c_92(202) '}'.
	p(NT(197), (T(42)+NT(202)+T(43)));
//G313: __E_chars_93(203)    => alnum(6).
	p(NT(203), (NT(6)));
//G314: __E_chars_93(203)    => '_'.
	p(NT(203), (T(40)));
//G315: __E_chars_94(204)    => null.
	p(NT(204), (nul));
//G316: __E_chars_94(204)    => __E_chars_93(203) __E_chars_94(204).
	p(NT(204), (NT(203)+NT(204)));
//G317: chars(177)           => alpha(5) __E_chars_94(204).
	p(NT(177), (NT(5)+NT(204)));
//G318: __E_digits_95(205)   => digit(3).
	p(NT(205), (NT(3)));
//G319: __E_digits_95(205)   => digit(3) __E_digits_95(205).
	p(NT(205), (NT(3)+NT(205)));
//G320: digits(187)          => __E_digits_95(205).
	p(NT(187), (NT(205)));
//G321: sym(22)              => chars(177).
	p(NT(22), (NT(177)));
//G322: __E_comment_96(207)  => printable(8).
	p(NT(207), (NT(8)));
//G323: __E_comment_96(207)  => '\t'.
	p(NT(207), (T(44)));
//G324: __E_comment_97(208)  => null.
	p(NT(208), (nul));
//G325: __E_comment_97(208)  => __E_comment_96(207) __E_comment_97(208).
	p(NT(208), (NT(207)+NT(208)));
//G326: __E_comment_98(209)  => '\n'.
	p(NT(209), (T(45)));
//G327: __E_comment_98(209)  => '\r'.
	p(NT(209), (T(46)));
//G328: __E_comment_98(209)  => eof(1).
	p(NT(209), (NT(1)));
//G329: comment(206)         => '#' __E_comment_97(208) __E_comment_98(209).
	p(NT(206), (T(47)+NT(208)+NT(209)));
//G330: __E____99(210)       => space(2).
	p(NT(210), (NT(2)));
//G331: __E____99(210)       => comment(206).
	p(NT(210), (NT(206)));
//G332: __(27)               => __E____99(210) _(11).
	p(NT(27), (NT(210)+NT(11)));
//G333: __E___100(211)       => __(27).
	p(NT(211), (NT(27)));
//G334: __E___100(211)       => null.
	p(NT(211), (nul));
//G335: _(11)                => __E___100(211).
	p(NT(11), (NT(211)));
//G336: cli(212)             => _(11).
	p(NT(212), (NT(11)));
//G337: __E_cli_101(214)     => '.' _(11) cli_command(213) _(11).
	p(NT(214), (T(1)+NT(11)+NT(213)+NT(11)));
//G338: __E_cli_102(215)     => null.
	p(NT(215), (nul));
//G339: __E_cli_102(215)     => __E_cli_101(214) __E_cli_102(215).
	p(NT(215), (NT(214)+NT(215)));
//G340: cli(212)             => _(11) cli_command(213) _(11) __E_cli_102(215).
	p(NT(212), (NT(11)+NT(213)+NT(11)+NT(215)));
//G341: __E_cli_command_103(218) => 'q'.
	p(NT(218), (T(48)));
//G342: __E_cli_command_103(218) => 'q' 'u' 'i' 't'.
	p(NT(218), (T(48)+T(49)+T(10)+T(13)));
//G343: quit_sym(217)        => __E_cli_command_103(218).
	p(NT(217), (NT(218)));
//G344: quit_cmd(216)        => quit_sym(217).
	p(NT(216), (NT(217)));
//G345: cli_command(213)     => quit_cmd(216).
	p(NT(213), (NT(216)));
//G346: __E_cli_command_104(221) => 'v'.
	p(NT(221), (T(50)));
//G347: __E_cli_command_104(221) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(221), (T(50)+T(19)+T(11)+T(12)+T(10)+T(17)+T(51)));
//G348: version_sym(220)     => __E_cli_command_104(221).
	p(NT(220), (NT(221)));
//G349: version_cmd(219)     => version_sym(220).
	p(NT(219), (NT(220)));
//G350: cli_command(213)     => version_cmd(219).
	p(NT(213), (NT(219)));
//G351: __E_cli_command_105(224) => 'c'.
	p(NT(224), (T(8)));
//G352: __E_cli_command_105(224) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(224), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G353: clear_sym(223)       => __E_cli_command_105(224).
	p(NT(223), (NT(224)));
//G354: clear_cmd(222)       => clear_sym(223).
	p(NT(222), (NT(223)));
//G355: cli_command(213)     => clear_cmd(222).
	p(NT(213), (NT(222)));
//G356: __E___E_cli_command_106_107(228) => 'h'.
	p(NT(228), (T(52)));
//G357: __E___E_cli_command_106_107(228) => 'h' 'e' 'l' 'p'.
	p(NT(228), (T(52)+T(19)+T(6)+T(53)));
//G358: help_sym(227)        => __E___E_cli_command_106_107(228).
	p(NT(227), (NT(228)));
//G359: __E___E_cli_command_106_108(229) => __(27) help_arg(230).
	p(NT(229), (NT(27)+NT(230)));
//G360: __E___E_cli_command_106_108(229) => null.
	p(NT(229), (nul));
//G361: __E_cli_command_106(226) => help_sym(227) __E___E_cli_command_106_108(229).
	p(NT(226), (NT(227)+NT(229)));
//G362: help_cmd(225)        => __E_cli_command_106(226).
	p(NT(225), (NT(226)));
//G363: cli_command(213)     => help_cmd(225).
	p(NT(213), (NT(225)));
//G364: file_sym(233)        => 'f' 'i' 'l' 'e'.
	p(NT(233), (T(4)+T(10)+T(6)+T(19)));
//G365: __E_cli_command_109(232) => file_sym(233) __(27) q_string(234).
	p(NT(232), (NT(233)+NT(27)+NT(234)));
//G366: file_cmd(231)        => __E_cli_command_109(232).
	p(NT(231), (NT(232)));
//G367: cli_command(213)     => file_cmd(231).
	p(NT(213), (NT(231)));
//G368: valid_sym(237)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(237), (T(50)+T(5)+T(6)+T(10)+T(54)));
//G369: __E_cli_command_110(236) => valid_sym(237) __(27) normalize_cmd_arg(238).
	p(NT(236), (NT(237)+NT(27)+NT(238)));
//G370: valid_cmd(235)       => __E_cli_command_110(236).
	p(NT(235), (NT(236)));
//G371: cli_command(213)     => valid_cmd(235).
	p(NT(213), (NT(235)));
//G372: sat_sym(241)         => 's' 'a' 't'.
	p(NT(241), (T(12)+T(5)+T(13)));
//G373: __E_cli_command_111(240) => sat_sym(241) __(27) normalize_cmd_arg(238).
	p(NT(240), (NT(241)+NT(27)+NT(238)));
//G374: sat_cmd(239)         => __E_cli_command_111(240).
	p(NT(239), (NT(240)));
//G375: cli_command(213)     => sat_cmd(239).
	p(NT(213), (NT(239)));
//G376: unsat_sym(244)       => 'u' 'n' 's' 'a' 't'.
	p(NT(244), (T(49)+T(51)+T(12)+T(5)+T(13)));
//G377: __E_cli_command_112(243) => unsat_sym(244) __(27) normalize_cmd_arg(238).
	p(NT(243), (NT(244)+NT(27)+NT(238)));
//G378: unsat_cmd(242)       => __E_cli_command_112(243).
	p(NT(242), (NT(243)));
//G379: cli_command(213)     => unsat_cmd(242).
	p(NT(213), (NT(242)));
//G380: solve_sym(247)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(247), (T(12)+T(17)+T(6)+T(50)+T(19)));
//G381: __E___E_cli_command_113_114(248) => solve_options(249).
	p(NT(248), (NT(249)));
//G382: __E___E_cli_command_113_114(248) => null.
	p(NT(248), (nul));
//G383: __E_cli_command_113(246) => solve_sym(247) __E___E_cli_command_113_114(248) __(27) solve_cmd_arg(250).
	p(NT(246), (NT(247)+NT(248)+NT(27)+NT(250)));
//G384: solve_cmd(245)       => __E_cli_command_113(246).
	p(NT(245), (NT(246)));
//G385: cli_command(213)     => solve_cmd(245).
	p(NT(213), (NT(245)));
//G386: lgrs_sym(253)        => 'l' 'g' 'r' 's'.
	p(NT(253), (T(6)+T(55)+T(11)+T(12)));
//G387: __E___E_cli_command_115_116(254) => __(27) type(131).
	p(NT(254), (NT(27)+NT(131)));
//G388: __E___E_cli_command_115_116(254) => null.
	p(NT(254), (nul));
//G389: __E_cli_command_115(252) => lgrs_sym(253) __E___E_cli_command_115_116(254) __(27) wff_cmd_arg(255).
	p(NT(252), (NT(253)+NT(254)+NT(27)+NT(255)));
//G390: lgrs_cmd(251)        => __E_cli_command_115(252).
	p(NT(251), (NT(252)));
//G391: cli_command(213)     => lgrs_cmd(251).
	p(NT(213), (NT(251)));
//G392: __E___E_cli_command_117_118(259) => 'r'.
	p(NT(259), (T(11)));
//G393: __E___E_cli_command_117_118(259) => 'r' 'u' 'n'.
	p(NT(259), (T(11)+T(49)+T(51)));
//G394: run_sym(258)         => __E___E_cli_command_117_118(259).
	p(NT(258), (NT(259)));
//G395: __E___E_cli_command_117_119(260) => memory(261).
	p(NT(260), (NT(261)));
//G396: __E___E_cli_command_117_119(260) => wff(14).
	p(NT(260), (NT(14)));
//G397: __E_cli_command_117(257) => run_sym(258) __(27) __E___E_cli_command_117_119(260).
	p(NT(257), (NT(258)+NT(27)+NT(260)));
//G398: run_cmd(256)         => __E_cli_command_117(257).
	p(NT(256), (NT(257)));
//G399: cli_command(213)     => run_cmd(256).
	p(NT(213), (NT(256)));
//G400: __E___E_cli_command_120_121(265) => 'n'.
	p(NT(265), (T(51)));
//G401: __E___E_cli_command_120_121(265) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(265), (T(51)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(56)+T(19)));
//G402: normalize_sym(264)   => __E___E_cli_command_120_121(265).
	p(NT(264), (NT(265)));
//G403: __E_cli_command_120(263) => normalize_sym(264) __(27) normalize_cmd_arg(238).
	p(NT(263), (NT(264)+NT(27)+NT(238)));
//G404: normalize_cmd(262)   => __E_cli_command_120(263).
	p(NT(262), (NT(263)));
//G405: cli_command(213)     => normalize_cmd(262).
	p(NT(213), (NT(262)));
//G406: __E___E_cli_command_122_123(269) => 's'.
	p(NT(269), (T(12)));
//G407: __E___E_cli_command_122_123(269) => 's' 'u' 'b' 's' 't'.
	p(NT(269), (T(12)+T(49)+T(7)+T(12)+T(13)));
//G408: __E___E_cli_command_122_123(269) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(269), (T(12)+T(49)+T(7)+T(12)+T(13)+T(10)+T(13)+T(49)+T(13)+T(19)));
//G409: subst_sym(268)       => __E___E_cli_command_122_123(269).
	p(NT(268), (NT(269)));
//G410: __E_cli_command_122(267) => subst_sym(268) __(27) nf_cmd_arg(270) _(11) '[' _(11) nf_cmd_arg(270) _(11) '/' _(11) nf_cmd_arg(270) _(11) ']'.
	p(NT(267), (NT(268)+NT(27)+NT(270)+NT(11)+T(24)+NT(11)+NT(270)+NT(11)+T(57)+NT(11)+NT(270)+NT(11)+T(25)));
//G411: subst_cmd(266)       => __E_cli_command_122(267).
	p(NT(266), (NT(267)));
//G412: cli_command(213)     => subst_cmd(266).
	p(NT(213), (NT(266)));
//G413: __E___E_cli_command_124_125(274) => 'i'.
	p(NT(274), (T(10)));
//G414: __E___E_cli_command_124_125(274) => 'i' 'n' 's' 't'.
	p(NT(274), (T(10)+T(51)+T(12)+T(13)));
//G415: __E___E_cli_command_124_125(274) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(274), (T(10)+T(51)+T(12)+T(13)+T(5)+T(51)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G416: inst_sym(273)        => __E___E_cli_command_124_125(274).
	p(NT(273), (NT(274)));
//G417: __E_cli_command_124(272) => inst_sym(273) __(27) inst_args(275).
	p(NT(272), (NT(273)+NT(27)+NT(275)));
//G418: inst_cmd(271)        => __E_cli_command_124(272).
	p(NT(271), (NT(272)));
//G419: cli_command(213)     => inst_cmd(271).
	p(NT(213), (NT(271)));
//G420: dnf_sym(278)         => 'd' 'n' 'f'.
	p(NT(278), (T(54)+T(51)+T(4)));
//G421: __E_cli_command_126(277) => dnf_sym(278) __(27) nf_cmd_arg(270).
	p(NT(277), (NT(278)+NT(27)+NT(270)));
//G422: dnf_cmd(276)         => __E_cli_command_126(277).
	p(NT(276), (NT(277)));
//G423: cli_command(213)     => dnf_cmd(276).
	p(NT(213), (NT(276)));
//G424: cnf_sym(281)         => 'c' 'n' 'f'.
	p(NT(281), (T(8)+T(51)+T(4)));
//G425: __E_cli_command_127(280) => cnf_sym(281) __(27) nf_cmd_arg(270).
	p(NT(280), (NT(281)+NT(27)+NT(270)));
//G426: cnf_cmd(279)         => __E_cli_command_127(280).
	p(NT(279), (NT(280)));
//G427: cli_command(213)     => cnf_cmd(279).
	p(NT(213), (NT(279)));
//G428: anf_sym(284)         => 'a' 'n' 'f'.
	p(NT(284), (T(5)+T(51)+T(4)));
//G429: __E_cli_command_128(283) => anf_sym(284) __(27) nf_cmd_arg(270).
	p(NT(283), (NT(284)+NT(27)+NT(270)));
//G430: anf_cmd(282)         => __E_cli_command_128(283).
	p(NT(282), (NT(283)));
//G431: cli_command(213)     => anf_cmd(282).
	p(NT(213), (NT(282)));
//G432: nnf_sym(287)         => 'n' 'n' 'f'.
	p(NT(287), (T(51)+T(51)+T(4)));
//G433: __E_cli_command_129(286) => nnf_sym(287) __(27) nf_cmd_arg(270).
	p(NT(286), (NT(287)+NT(27)+NT(270)));
//G434: nnf_cmd(285)         => __E_cli_command_129(286).
	p(NT(285), (NT(286)));
//G435: cli_command(213)     => nnf_cmd(285).
	p(NT(213), (NT(285)));
//G436: pnf_sym(290)         => 'p' 'n' 'f'.
	p(NT(290), (T(53)+T(51)+T(4)));
//G437: __E_cli_command_130(289) => pnf_sym(290) __(27) nf_cmd_arg(270).
	p(NT(289), (NT(290)+NT(27)+NT(270)));
//G438: pnf_cmd(288)         => __E_cli_command_130(289).
	p(NT(288), (NT(289)));
//G439: cli_command(213)     => pnf_cmd(288).
	p(NT(213), (NT(288)));
//G440: mnf_sym(293)         => 'm' 'n' 'f'.
	p(NT(293), (T(18)+T(51)+T(4)));
//G441: __E_cli_command_131(292) => mnf_sym(293) __(27) nf_cmd_arg(270).
	p(NT(292), (NT(293)+NT(27)+NT(270)));
//G442: mnf_cmd(291)         => __E_cli_command_131(292).
	p(NT(291), (NT(292)));
//G443: cli_command(213)     => mnf_cmd(291).
	p(NT(213), (NT(291)));
//G444: snf_sym(296)         => 's' 'n' 'f'.
	p(NT(296), (T(12)+T(51)+T(4)));
//G445: __E_cli_command_132(295) => snf_sym(296) __(27) nf_cmd_arg(270).
	p(NT(295), (NT(296)+NT(27)+NT(270)));
//G446: snf_cmd(294)         => __E_cli_command_132(295).
	p(NT(294), (NT(295)));
//G447: cli_command(213)     => snf_cmd(294).
	p(NT(213), (NT(294)));
//G448: onf_sym(299)         => 'o' 'n' 'f'.
	p(NT(299), (T(17)+T(51)+T(4)));
//G449: __E_cli_command_133(298) => onf_sym(299) __(27) variable(120) __(27) onf_cmd_arg(300).
	p(NT(298), (NT(299)+NT(27)+NT(120)+NT(27)+NT(300)));
//G450: onf_cmd(297)         => __E_cli_command_133(298).
	p(NT(297), (NT(298)));
//G451: cli_command(213)     => onf_cmd(297).
	p(NT(213), (NT(297)));
//G452: __E___E_cli_command_134_135(304) => 'd' 'e' 'f' 's'.
	p(NT(304), (T(54)+T(19)+T(4)+T(12)));
//G453: __E___E_cli_command_134_135(304) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(304), (T(54)+T(19)+T(4)+T(10)+T(51)+T(10)+T(13)+T(10)+T(17)+T(51)+T(12)));
//G454: def_sym(303)         => __E___E_cli_command_134_135(304).
	p(NT(303), (NT(304)));
//G455: __E_cli_command_134(302) => def_sym(303).
	p(NT(302), (NT(303)));
//G456: def_list_cmd(301)    => __E_cli_command_134(302).
	p(NT(301), (NT(302)));
//G457: cli_command(213)     => def_list_cmd(301).
	p(NT(213), (NT(301)));
//G458: __E_cli_command_136(306) => def_sym(303) __(27) number(307).
	p(NT(306), (NT(303)+NT(27)+NT(307)));
//G459: def_print_cmd(305)   => __E_cli_command_136(306).
	p(NT(305), (NT(306)));
//G460: cli_command(213)     => def_print_cmd(305).
	p(NT(213), (NT(305)));
//G461: def_rr_cmd(308)      => rec_relation(16).
	p(NT(308), (NT(16)));
//G462: cli_command(213)     => def_rr_cmd(308).
	p(NT(213), (NT(308)));
//G463: qelim_sym(311)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(311), (T(48)+T(19)+T(6)+T(10)+T(18)));
//G464: __E_cli_command_137(310) => qelim_sym(311) __(27) wff_cmd_arg(255).
	p(NT(310), (NT(311)+NT(27)+NT(255)));
//G465: qelim_cmd(309)       => __E_cli_command_137(310).
	p(NT(309), (NT(310)));
//G466: cli_command(213)     => qelim_cmd(309).
	p(NT(213), (NT(309)));
//G467: get_sym(314)         => 'g' 'e' 't'.
	p(NT(314), (T(55)+T(19)+T(13)));
//G468: __E___E_cli_command_138_139(315) => __(27) option_name(316).
	p(NT(315), (NT(27)+NT(316)));
//G469: __E___E_cli_command_138_139(315) => null.
	p(NT(315), (nul));
//G470: __E_cli_command_138(313) => get_sym(314) __E___E_cli_command_138_139(315).
	p(NT(313), (NT(314)+NT(315)));
//G471: get_cmd(312)         => __E_cli_command_138(313).
	p(NT(312), (NT(313)));
//G472: cli_command(213)     => get_cmd(312).
	p(NT(213), (NT(312)));
//G473: set_sym(319)         => 's' 'e' 't'.
	p(NT(319), (T(12)+T(19)+T(13)));
//G474: __E___E_cli_command_140_141(320) => __(27).
	p(NT(320), (NT(27)));
//G475: __E___E_cli_command_140_141(320) => _(11) '=' _(11).
	p(NT(320), (NT(11)+T(3)+NT(11)));
//G476: __E_cli_command_140(318) => set_sym(319) __(27) option_name(316) __E___E_cli_command_140_141(320) option_value(321).
	p(NT(318), (NT(319)+NT(27)+NT(316)+NT(320)+NT(321)));
//G477: set_cmd(317)         => __E_cli_command_140(318).
	p(NT(317), (NT(318)));
//G478: cli_command(213)     => set_cmd(317).
	p(NT(213), (NT(317)));
//G479: enable_sym(324)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(324), (T(19)+T(51)+T(5)+T(7)+T(6)+T(19)));
//G480: __E_cli_command_142(323) => enable_sym(324) __(27) option_name(316).
	p(NT(323), (NT(324)+NT(27)+NT(316)));
//G481: enable_cmd(322)      => __E_cli_command_142(323).
	p(NT(322), (NT(323)));
//G482: cli_command(213)     => enable_cmd(322).
	p(NT(213), (NT(322)));
//G483: disable_sym(327)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(327), (T(54)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G484: __E_cli_command_143(326) => disable_sym(327) __(27) option_name(316).
	p(NT(326), (NT(327)+NT(27)+NT(316)));
//G485: disable_cmd(325)     => __E_cli_command_143(326).
	p(NT(325), (NT(326)));
//G486: cli_command(213)     => disable_cmd(325).
	p(NT(213), (NT(325)));
//G487: toggle_sym(330)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(330), (T(13)+T(17)+T(55)+T(55)+T(6)+T(19)));
//G488: __E_cli_command_144(329) => toggle_sym(330) __(27) option_name(316).
	p(NT(329), (NT(330)+NT(27)+NT(316)));
//G489: toggle_cmd(328)      => __E_cli_command_144(329).
	p(NT(328), (NT(329)));
//G490: cli_command(213)     => toggle_cmd(328).
	p(NT(213), (NT(328)));
//G491: __E_cli_command_145(332) => type(131) __(27) in_var_name(170) _(11) '=' _(11) input_stream(333).
	p(NT(332), (NT(131)+NT(27)+NT(170)+NT(11)+T(3)+NT(11)+NT(333)));
//G492: def_input_cmd(331)   => __E_cli_command_145(332).
	p(NT(331), (NT(332)));
//G493: cli_command(213)     => def_input_cmd(331).
	p(NT(213), (NT(331)));
//G494: __E_cli_command_146(335) => type(131) __(27) out_var_name(174) _(11) '=' _(11) output_stream(336).
	p(NT(335), (NT(131)+NT(27)+NT(174)+NT(11)+T(3)+NT(11)+NT(336)));
//G495: def_output_cmd(334)  => __E_cli_command_146(335).
	p(NT(334), (NT(335)));
//G496: cli_command(213)     => def_output_cmd(334).
	p(NT(213), (NT(334)));
//G497: __E___E_cli_command_147_148(340) => 'h' 'i' 's' 't'.
	p(NT(340), (T(52)+T(10)+T(12)+T(13)));
//G498: __E___E_cli_command_147_148(340) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(340), (T(52)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G499: history_sym(339)     => __E___E_cli_command_147_148(340).
	p(NT(339), (NT(340)));
//G500: __E_cli_command_147(338) => history_sym(339).
	p(NT(338), (NT(339)));
//G501: history_list_cmd(337) => __E_cli_command_147(338).
	p(NT(337), (NT(338)));
//G502: cli_command(213)     => history_list_cmd(337).
	p(NT(213), (NT(337)));
//G503: __E_cli_command_149(342) => history_sym(339) __(27) memory(261).
	p(NT(342), (NT(339)+NT(27)+NT(261)));
//G504: history_print_cmd(341) => __E_cli_command_149(342).
	p(NT(341), (NT(342)));
//G505: cli_command(213)     => history_print_cmd(341).
	p(NT(213), (NT(341)));
//G506: __E_cli_command_150(344) => wff(14).
	p(NT(344), (NT(14)));
//G507: __E_cli_command_150(344) => bf(21).
	p(NT(344), (NT(21)));
//G508: history_store_cmd(343) => __E_cli_command_150(344).
	p(NT(343), (NT(344)));
//G509: cli_command(213)     => history_store_cmd(343).
	p(NT(213), (NT(343)));
//G510: __E_solve_options_151(345) => __(27) solve_option(346).
	p(NT(345), (NT(27)+NT(346)));
//G511: __E_solve_options_152(347) => null.
	p(NT(347), (nul));
//G512: __E_solve_options_152(347) => __E_solve_options_151(345) __E_solve_options_152(347).
	p(NT(347), (NT(345)+NT(347)));
//G513: solve_options(249)   => __E_solve_options_152(347).
	p(NT(249), (NT(347)));
//G514: __E_solve_option_153(348) => '-' '-' solver_mode(349).
	p(NT(348), (T(28)+T(28)+NT(349)));
//G515: solve_option(346)    => __E_solve_option_153(348).
	p(NT(346), (NT(348)));
//G516: __E_solve_option_154(350) => '-' '-' type(131).
	p(NT(350), (T(28)+T(28)+NT(131)));
//G517: solve_option(346)    => __E_solve_option_154(350).
	p(NT(346), (NT(350)));
//G518: __E_solver_mode_155(352) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(352), (T(18)+T(10)+T(51)+T(10)+T(18)+T(49)+T(18)));
//G519: __E_solver_mode_155(352) => 'm' 'i' 'n'.
	p(NT(352), (T(18)+T(10)+T(51)));
//G520: solver_mode_minimum(351) => __E_solver_mode_155(352).
	p(NT(351), (NT(352)));
//G521: solver_mode(349)     => solver_mode_minimum(351).
	p(NT(349), (NT(351)));
//G522: __E_solver_mode_156(354) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(354), (T(18)+T(5)+T(27)+T(10)+T(18)+T(49)+T(18)));
//G523: __E_solver_mode_156(354) => 'm' 'a' 'x'.
	p(NT(354), (T(18)+T(5)+T(27)));
//G524: solver_mode_maximum(353) => __E_solver_mode_156(354).
	p(NT(353), (NT(354)));
//G525: solver_mode(349)     => solver_mode_maximum(353).
	p(NT(349), (NT(353)));
//G526: __E_input_stream_157(355) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(356) _(11) ')'.
	p(NT(355), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(356)+NT(11)+T(16)));
//G527: input_stream(333)    => __E_input_stream_157(355).
	p(NT(333), (NT(355)));
//G528: console_sym(357)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(357), (T(8)+T(17)+T(51)+T(12)+T(17)+T(6)+T(19)));
//G529: input_stream(333)    => console_sym(357).
	p(NT(333), (NT(357)));
//G530: __E_output_stream_158(358) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(356) _(11) ')'.
	p(NT(358), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(356)+NT(11)+T(16)));
//G531: output_stream(336)   => __E_output_stream_158(358).
	p(NT(336), (NT(358)));
//G532: output_stream(336)   => console_sym(357).
	p(NT(336), (NT(357)));
//G533: q_file_name(356)     => '"' file_name(359) '"'.
	p(NT(356), (T(58)+NT(359)+T(58)));
//G534: __E_file_name_159(360) => printable(8).
	p(NT(360), (NT(8)));
//G535: __E_file_name_159(360) => printable(8) __E_file_name_159(360).
	p(NT(360), (NT(8)+NT(360)));
//G536: file_name(359)       => __E_file_name_159(360).
	p(NT(359), (NT(360)));
//G537: __E_option_name_160(361) => alnum(6).
	p(NT(361), (NT(6)));
//G538: __E_option_name_160(361) => alnum(6) __E_option_name_160(361).
	p(NT(361), (NT(6)+NT(361)));
//G539: option_name(316)     => __E_option_name_160(361).
	p(NT(316), (NT(361)));
//G540: __E_option_value_161(362) => alnum(6).
	p(NT(362), (NT(6)));
//G541: __E_option_value_161(362) => alnum(6) __E_option_value_161(362).
	p(NT(362), (NT(6)+NT(362)));
//G542: option_value(321)    => __E_option_value_161(362).
	p(NT(321), (NT(362)));
//G543: number(307)          => digits(187).
	p(NT(307), (NT(187)));
//G544: bf_cmd_arg(363)      => memory(261).
	p(NT(363), (NT(261)));
//G545: bf_cmd_arg(363)      => bf(21).
	p(NT(363), (NT(21)));
//G546: wff_cmd_arg(255)     => memory(261).
	p(NT(255), (NT(261)));
//G547: wff_cmd_arg(255)     => wff(14).
	p(NT(255), (NT(14)));
//G548: solve_cmd_arg(250)   => memory(261).
	p(NT(250), (NT(261)));
//G549: solve_cmd_arg(250)   => wff(14).
	p(NT(250), (NT(14)));
//G550: nf_cmd_arg(270)      => memory(261).
	p(NT(270), (NT(261)));
//G551: nf_cmd_arg(270)      => ref(18).
	p(NT(270), (NT(18)));
//G552: nf_cmd_arg(270)      => wff(14).
	p(NT(270), (NT(14)));
//G553: nf_cmd_arg(270)      => bf(21).
	p(NT(270), (NT(21)));
//G554: onf_cmd_arg(300)     => memory(261).
	p(NT(300), (NT(261)));
//G555: onf_cmd_arg(300)     => wff(14).
	p(NT(300), (NT(14)));
//G556: normalize_cmd_arg(238) => memory(261).
	p(NT(238), (NT(261)));
//G557: normalize_cmd_arg(238) => rr(10).
	p(NT(238), (NT(10)));
//G558: normalize_cmd_arg(238) => ref(18).
	p(NT(238), (NT(18)));
//G559: normalize_cmd_arg(238) => wff(14).
	p(NT(238), (NT(14)));
//G560: normalize_cmd_arg(238) => bf(21).
	p(NT(238), (NT(21)));
//G561: inst_args(275)       => wff_cmd_arg(255) _(11) '[' _(11) variable(120) _(11) '/' _(11) bf_cmd_arg(363) _(11) ']'.
	p(NT(275), (NT(255)+NT(11)+T(24)+NT(11)+NT(120)+NT(11)+T(57)+NT(11)+NT(363)+NT(11)+T(25)));
//G562: inst_args(275)       => bf_cmd_arg(363) _(11) '[' _(11) variable(120) _(11) '/' _(11) bf_cmd_arg(363) _(11) ']'.
	p(NT(275), (NT(363)+NT(11)+T(24)+NT(11)+NT(120)+NT(11)+T(57)+NT(11)+NT(363)+NT(11)+T(25)));
//G563: help_arg(230)        => help_sym(227).
	p(NT(230), (NT(227)));
//G564: help_arg(230)        => version_sym(220).
	p(NT(230), (NT(220)));
//G565: help_arg(230)        => quit_sym(217).
	p(NT(230), (NT(217)));
//G566: help_arg(230)        => clear_sym(223).
	p(NT(230), (NT(223)));
//G567: help_arg(230)        => get_sym(314).
	p(NT(230), (NT(314)));
//G568: help_arg(230)        => set_sym(319).
	p(NT(230), (NT(319)));
//G569: help_arg(230)        => enable_sym(324).
	p(NT(230), (NT(324)));
//G570: help_arg(230)        => disable_sym(327).
	p(NT(230), (NT(327)));
//G571: help_arg(230)        => toggle_sym(330).
	p(NT(230), (NT(330)));
//G572: help_arg(230)        => file_sym(233).
	p(NT(230), (NT(233)));
//G573: help_arg(230)        => history_sym(339).
	p(NT(230), (NT(339)));
//G574: help_arg(230)        => abs_memory_sym(364).
	p(NT(230), (NT(364)));
//G575: help_arg(230)        => rel_memory_sym(365).
	p(NT(230), (NT(365)));
//G576: help_arg(230)        => selection_sym(366).
	p(NT(230), (NT(366)));
//G577: help_arg(230)        => def_sym(303).
	p(NT(230), (NT(303)));
//G578: help_arg(230)        => inst_sym(273).
	p(NT(230), (NT(273)));
//G579: help_arg(230)        => subst_sym(268).
	p(NT(230), (NT(268)));
//G580: help_arg(230)        => normalize_sym(264).
	p(NT(230), (NT(264)));
//G581: help_arg(230)        => execute_sym(367).
	p(NT(230), (NT(367)));
//G582: help_arg(230)        => solve_sym(247).
	p(NT(230), (NT(247)));
//G583: help_arg(230)        => valid_sym(237).
	p(NT(230), (NT(237)));
//G584: help_arg(230)        => sat_sym(241).
	p(NT(230), (NT(241)));
//G585: help_arg(230)        => unsat_sym(244).
	p(NT(230), (NT(244)));
//G586: help_arg(230)        => run_sym(258).
	p(NT(230), (NT(258)));
//G587: help_arg(230)        => dnf_sym(278).
	p(NT(230), (NT(278)));
//G588: help_arg(230)        => cnf_sym(281).
	p(NT(230), (NT(281)));
//G589: help_arg(230)        => anf_sym(284).
	p(NT(230), (NT(284)));
//G590: help_arg(230)        => snf_sym(296).
	p(NT(230), (NT(296)));
//G591: help_arg(230)        => nnf_sym(287).
	p(NT(230), (NT(287)));
//G592: help_arg(230)        => pnf_sym(290).
	p(NT(230), (NT(290)));
//G593: help_arg(230)        => mnf_sym(293).
	p(NT(230), (NT(293)));
//G594: help_arg(230)        => onf_sym(299).
	p(NT(230), (NT(299)));
//G595: help_arg(230)        => qelim_sym(311).
	p(NT(230), (NT(311)));
//G596: __E___E_help_arg_162_163(370) => 's'.
	p(NT(370), (T(12)));
//G597: __E___E_help_arg_162_163(370) => null.
	p(NT(370), (nul));
//G598: __E_help_arg_162(369) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_162_163(370).
	p(NT(369), (T(19)+T(27)+T(5)+T(18)+T(53)+T(6)+T(19)+NT(370)));
//G599: examples_sym(368)    => __E_help_arg_162(369).
	p(NT(368), (NT(369)));
//G600: help_arg(230)        => examples_sym(368).
	p(NT(230), (NT(368)));
//G601: __E_memory_164(372)  => '%'.
	p(NT(372), (T(59)));
//G602: rel_memory_sym(365)  => '%' '-'.
	p(NT(365), (T(59)+T(28)));
//G603: memory_id(374)       => digits(187).
	p(NT(374), (NT(187)));
//G604: __E___E_memory_164_165(373) => memory_id(374).
	p(NT(373), (NT(374)));
//G605: __E___E_memory_164_165(373) => null.
	p(NT(373), (nul));
//G606: __E_memory_164(372)  => rel_memory_sym(365) __E___E_memory_164_165(373).
	p(NT(372), (NT(365)+NT(373)));
//G607: rel_memory(371)      => __E_memory_164(372).
	p(NT(371), (NT(372)));
//G608: memory(261)          => rel_memory(371).
	p(NT(261), (NT(371)));
//G609: abs_memory_sym(364)  => '%'.
	p(NT(364), (T(59)));
//G610: __E_memory_166(376)  => abs_memory_sym(364) memory_id(374).
	p(NT(376), (NT(364)+NT(374)));
//G611: abs_memory(375)      => __E_memory_166(376).
	p(NT(375), (NT(376)));
//G612: memory(261)          => abs_memory(375).
	p(NT(261), (NT(375)));
	#undef T
	#undef NT
	return loaded = true, p;
}

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace tau_parser_data

struct tau_parser : public idni::parser<char, char> {
	enum nonterminal {
		nul, eof, space, digit, xdigit, alpha, alnum, punct, printable, start, 
		rr, _, rec_relations, main, wff, __E_rec_relations_0, rec_relation, __E_rec_relations_1, ref, __E_rec_relation_2, 
		capture, bf, sym, __E_ref_3, offsets, ref_args, __E_ref_4, __, fp_fallback, first_sym, 
		last_sym, __E_ref_args_5, __E___E_ref_args_5_6, ref_arg, __E___E_ref_args_5_7, __E___E_ref_args_5_8, library, rules, __E_library_9, __E___E_library_9_10, 
		rule, __E___E___E_library_9_10_11, wff_rule, bf_rule, __E___E_library_9_12, wff_matcher, wff_body, __E_wff_rule_13, bf_matcher, bf_body, 
		__E_bf_rule_14, builder, builder_head, builder_body, __E_builder_head_15, __E_builder_head_16, bf_builder_body, __E_builder_body_17, wff_builder_body, __E_builder_body_18, 
		tau_constant_source, __E_tau_constant_source_19, wff_parenthesis, __E_wff_20, wff_sometimes, __E_wff_21, __E___E_wff_21_22, wff_always, __E_wff_23, __E___E_wff_23_24, 
		wff_conditional, __E_wff_25, wff_all, __E_wff_26, q_vars, wff_ex, __E_wff_27, wff_ref, constraint, wff_imply, 
		__E_wff_28, wff_rimply, __E_wff_29, wff_equiv, __E_wff_30, wff_or, __E_wff_31, wff_xor, __E_wff_32, wff_and, 
		__E_wff_33, wff_neg, __E_wff_34, wff_t, wff_f, bf_interval, __E_wff_35, bf_eq, __E_wff_36, bf_neq, 
		__E_wff_37, bf_less_equal, __E_wff_38, bf_nleq, __E_wff_39, bf_greater, __E_wff_40, bf_ngreater, __E_wff_41, bf_greater_equal, 
		__E_wff_42, bf_ngeq, __E_wff_43, bf_less, __E_wff_44, bf_nless, __E_wff_45, bf_parenthesis, __E_bf_46, bf_constant, 
		variable, bf_splitter, __E_bf_47, bf_ref, bf_or, __E_bf_48, bf_xor, __E_bf_49, bf_t, __E_bf_50, 
		__E___E_bf_50_51, type, bf_f, __E_bf_52, __E___E_bf_52_53, bf_and, __E_bf_54, __E___E_bf_54_55, bf_neg, __E_bf_56, 
		bf_neg_oprnd, __E___E_bf_56_57, bf_and_nosep, __E_bf_58, bf_and_nosep_1st_oprnd, __E___E_bf_58_59, ctn_neq, __E_constraint_60, ctnvar, num, 
		ctn_eq, __E_constraint_61, ctn_greater_equal, __E_constraint_62, ctn_greater, __E_constraint_63, ctn_less_equal, __E_constraint_64, ctn_less, __E_constraint_65, 
		__E_variable_66, __E_variable_67, __E_variable_68, __E_variable_69, __E_variable_70, io_var, __E_variable_71, in, out, uninterpreted_constant, 
		in_var_name, __E_in_72, __E___E_in_72_73, offset, out_var_name, __E_out_74, __E___E_out_74_75, chars, q_var, __E_q_vars_76, 
		__E_q_vars_77, __E_offsets_78, __E_offsets_79, shift, __E_offset_80, __E_shift_81, __E___E_shift_81_82, digits, uninter_const_name, __E_uninterpreted_constant_83, 
		__E___E_uninterpreted_constant_83_84, constant, __E_bf_constant_85, binding, __E_bf_constant_86, source, __E_binding_87, src_c, __E___E_binding_87_88, __E___E___E_binding_87_88_89, 
		__E___E___E_binding_87_88_90, __E_src_c_91, __E_src_c_92, __E_chars_93, __E_chars_94, __E_digits_95, comment, __E_comment_96, __E_comment_97, __E_comment_98, 
		__E____99, __E___100, cli, cli_command, __E_cli_101, __E_cli_102, quit_cmd, quit_sym, __E_cli_command_103, version_cmd, 
		version_sym, __E_cli_command_104, clear_cmd, clear_sym, __E_cli_command_105, help_cmd, __E_cli_command_106, help_sym, __E___E_cli_command_106_107, __E___E_cli_command_106_108, 
		help_arg, file_cmd, __E_cli_command_109, file_sym, q_string, valid_cmd, __E_cli_command_110, valid_sym, normalize_cmd_arg, sat_cmd, 
		__E_cli_command_111, sat_sym, unsat_cmd, __E_cli_command_112, unsat_sym, solve_cmd, __E_cli_command_113, solve_sym, __E___E_cli_command_113_114, solve_options, 
		solve_cmd_arg, lgrs_cmd, __E_cli_command_115, lgrs_sym, __E___E_cli_command_115_116, wff_cmd_arg, run_cmd, __E_cli_command_117, run_sym, __E___E_cli_command_117_118, 
		__E___E_cli_command_117_119, memory, normalize_cmd, __E_cli_command_120, normalize_sym, __E___E_cli_command_120_121, subst_cmd, __E_cli_command_122, subst_sym, __E___E_cli_command_122_123, 
		nf_cmd_arg, inst_cmd, __E_cli_command_124, inst_sym, __E___E_cli_command_124_125, inst_args, dnf_cmd, __E_cli_command_126, dnf_sym, cnf_cmd, 
		__E_cli_command_127, cnf_sym, anf_cmd, __E_cli_command_128, anf_sym, nnf_cmd, __E_cli_command_129, nnf_sym, pnf_cmd, __E_cli_command_130, 
		pnf_sym, mnf_cmd, __E_cli_command_131, mnf_sym, snf_cmd, __E_cli_command_132, snf_sym, onf_cmd, __E_cli_command_133, onf_sym, 
		onf_cmd_arg, def_list_cmd, __E_cli_command_134, def_sym, __E___E_cli_command_134_135, def_print_cmd, __E_cli_command_136, number, def_rr_cmd, qelim_cmd, 
		__E_cli_command_137, qelim_sym, get_cmd, __E_cli_command_138, get_sym, __E___E_cli_command_138_139, option_name, set_cmd, __E_cli_command_140, set_sym, 
		__E___E_cli_command_140_141, option_value, enable_cmd, __E_cli_command_142, enable_sym, disable_cmd, __E_cli_command_143, disable_sym, toggle_cmd, __E_cli_command_144, 
		toggle_sym, def_input_cmd, __E_cli_command_145, input_stream, def_output_cmd, __E_cli_command_146, output_stream, history_list_cmd, __E_cli_command_147, history_sym, 
		__E___E_cli_command_147_148, history_print_cmd, __E_cli_command_149, history_store_cmd, __E_cli_command_150, __E_solve_options_151, solve_option, __E_solve_options_152, __E_solve_option_153, solver_mode, 
		__E_solve_option_154, solver_mode_minimum, __E_solver_mode_155, solver_mode_maximum, __E_solver_mode_156, __E_input_stream_157, q_file_name, console_sym, __E_output_stream_158, file_name, 
		__E_file_name_159, __E_option_name_160, __E_option_value_161, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, execute_sym, examples_sym, __E_help_arg_162, 
		__E___E_help_arg_162_163, rel_memory, __E_memory_164, __E___E_memory_164_165, memory_id, abs_memory, __E_memory_166, named_binding, __N_0, __N_1, 
		__N_2, __N_3, __N_4, __N_5, __N_6, __N_7, __N_8, 
	};
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
