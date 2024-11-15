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
	"capture", "bf", "sym", "__E_ref_3", "offsets", "ref_args", "__E_ref_args_4", "__E___E_ref_args_4_5", "ref_arg", "__E___E_ref_args_4_6", 
	"__E___E_ref_args_4_7", "library", "rules", "__E_library_8", "__E___E_library_8_9", "rule", "__E___E___E_library_8_9_10", "wff_rule", "bf_rule", "__E___E_library_8_11", 
	"wff_matcher", "wff_body", "__E_wff_rule_12", "wff_cb", "bf_matcher", "bf_body", "__E_bf_rule_13", "bf_cb", "builder", "builder_head", 
	"builder_body", "__E_builder_head_14", "__", "__E_builder_head_15", "bf_builder_body", "__E_builder_body_16", "wff_builder_body", "__E_builder_body_17", "tau_constant_source", "__E_tau_constant_source_18", 
	"wff_parenthesis", "__E_wff_19", "wff_sometimes", "__E_wff_20", "__E___E_wff_20_21", "wff_always", "__E_wff_22", "__E___E_wff_22_23", "wff_conditional", "__E_wff_24", 
	"wff_all", "__E_wff_25", "q_vars", "wff_ex", "__E_wff_26", "wff_ref", "constraint", "wff_imply", "__E_wff_27", "wff_equiv", 
	"__E_wff_28", "wff_or", "__E_wff_29", "wff_xor", "__E_wff_30", "wff_and", "__E_wff_31", "wff_neg", "__E_wff_32", "wff_t", 
	"wff_f", "bf_interval", "__E_wff_33", "bf_eq", "__E_wff_34", "bf_neq", "__E_wff_35", "bf_less_equal", "__E_wff_36", "bf_nleq", 
	"__E_wff_37", "bf_greater", "__E_wff_38", "bf_ngreater", "__E_wff_39", "bf_greater_equal", "__E_wff_40", "bf_ngeq", "__E_wff_41", "bf_less", 
	"__E_wff_42", "bf_nless", "__E_wff_43", "bf_parenthesis", "__E_bf_44", "bf_constant", "variable", "bf_splitter", "__E_bf_45", "bf_ref", 
	"bf_or", "__E_bf_46", "bf_xor", "__E_bf_47", "bf_and", "__E_bf_48", "__E___E_bf_48_49", "bf_neg", "__E_bf_50", "bf_t", 
	"bf_f", "bf_and_nosep_1st_op", "__E_bf_and_51", "uninterpreted_constant", "ctn_neq", "__E_constraint_52", "ctnvar", "num", "ctn_eq", "__E_constraint_53", 
	"ctn_greater_equal", "__E_constraint_54", "ctn_greater", "__E_constraint_55", "ctn_less_equal", "__E_constraint_56", "ctn_less", "__E_constraint_57", "__E_variable_58", "chars", 
	"io_var", "__E_variable_59", "in", "out", "in_var_name", "__E_in_60", "__E___E_in_60_61", "offset", "out_var_name", "__E_out_62", 
	"__E___E_out_62_63", "q_var", "__E_q_vars_64", "__E_q_vars_65", "__E_offsets_66", "__E_offsets_67", "shift", "__E_offset_68", "__E_shift_69", "__E___E_shift_69_70", 
	"digits", "uninter_const_name", "__E_uninterpreted_constant_71", "__E___E_uninterpreted_constant_71_72", "constant", "__E_bf_constant_73", "binding", "__E_bf_constant_74", "type", "source", 
	"__E_binding_75", "src_c", "__E___E_binding_75_76", "__E___E___E_binding_75_76_77", "__E___E___E_binding_75_76_78", "__E_src_c_79", "__E_src_c_80", "__E_chars_81", "__E_chars_82", "__E_digits_83", 
	"extra", "comment", "__E_comment_84", "__E_comment_85", "__E_comment_86", "__E____87", "__E___88", "bf_cb_args1", "bf_cb_arg", "bf_cb_args2", 
	"bf_cb_args3", "bf_cb_args4", "wff_cb_args1", "wff_cb_arg", "wff_cb_args2", "wff_cb_args3", "wff_cb_args4", "wff_has_clashing_subformulas_cb", "__E_wff_cb_89", "wff_has_subformula_cb", 
	"__E_wff_cb_90", "wff_remove_existential_cb", "__E_wff_cb_91", "bf_has_subformula_cb", "__E_bf_cb_92", "bf_remove_funiversal_cb", "__E_bf_cb_93", "bf_remove_fexistential_cb", "__E_bf_cb_94", "bf_normalize_cb", 
	"__E_bf_cb_95", "cli", "cli_command", "__E_cli_96", "__E_cli_97", "quit_cmd", "quit_sym", "__E_cli_command_98", "version_cmd", "version_sym", 
	"__E_cli_command_99", "clear_cmd", "clear_sym", "__E_cli_command_100", "help_cmd", "__E_cli_command_101", "help_sym", "__E___E_cli_command_101_102", "__E___E_cli_command_101_103", "help_arg", 
	"file_cmd", "__E_cli_command_104", "file_sym", "q_string", "valid_cmd", "__E_cli_command_105", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_106", 
	"sat_sym", "unsat_cmd", "__E_cli_command_107", "unsat_sym", "solve_cmd", "__E_cli_command_108", "solve_sym", "__E___E_cli_command_108_109", "solve_cmd_arg", "run_cmd", 
	"__E_cli_command_110", "run_sym", "__E___E_cli_command_110_111", "__E___E_cli_command_110_112", "memory", "normalize_cmd", "__E_cli_command_113", "normalize_sym", "__E___E_cli_command_113_114", "subst_cmd", 
	"__E_cli_command_115", "subst_sym", "__E___E_cli_command_115_116", "nf_cmd_arg", "inst_cmd", "__E_cli_command_117", "inst_sym", "__E___E_cli_command_117_118", "inst_args", "dnf_cmd", 
	"__E_cli_command_119", "dnf_sym", "cnf_cmd", "__E_cli_command_120", "cnf_sym", "anf_cmd", "__E_cli_command_121", "anf_sym", "nnf_cmd", "__E_cli_command_122", 
	"nnf_sym", "pnf_cmd", "__E_cli_command_123", "pnf_sym", "mnf_cmd", "__E_cli_command_124", "mnf_sym", "snf_cmd", "__E_cli_command_125", "snf_sym", 
	"onf_cmd", "__E_cli_command_126", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_127", "def_sym", "__E___E_cli_command_127_128", "def_print_cmd", "__E_cli_command_129", 
	"number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_130", "qelim_sym", "wff_cmd_arg", "get_cmd", "__E_cli_command_131", "get_sym", "__E___E_cli_command_131_132", 
	"option", "set_cmd", "__E_cli_command_133", "set_sym", "__E___E_cli_command_133_134", "option_value", "enable_cmd", "__E_cli_command_135", "enable_sym", "bool_option", 
	"disable_cmd", "__E_cli_command_136", "disable_sym", "toggle_cmd", "__E_cli_command_137", "toggle_sym", "def_input_cmd", "__E_cli_command_138", "input_stream", "def_output_cmd", 
	"__E_cli_command_139", "output_stream", "history_list_cmd", "__E_cli_command_140", "history_sym", "__E___E_cli_command_140_141", "history_print_cmd", "__E_cli_command_142", "history_store_cmd", "__E_cli_command_143", 
	"__E_input_stream_144", "q_file_name", "console_sym", "__E_output_stream_145", "file_name", "__E_file_name_146", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", 
	"execute_sym", "examples_sym", "__E_help_arg_147", "__E___E_help_arg_147_148", "rel_memory", "__E_memory_149", "__E___E_memory_149_150", "memory_id", "abs_memory", "__E_memory_151", 
	"enum_option", "severity_opt", "__E_option_152", "status_opt", "__E_bool_option_153", "colors_opt", "__E_bool_option_154", "charvar_opt", "__E_bool_option_155", "highlighting_opt", 
	"__E_bool_option_156", "indenting_opt", "__E_bool_option_157", "debug_repl_opt", "__E_bool_option_158", "option_value_true", "option_value_false", "severity", "error_sym", "__E_severity_159", 
	"info_sym", "__E_severity_160", "debug_sym", "__E_severity_161", "trace_sym", "__E_severity_162", "named_binding", "__N_0", "__N_1", "__N_2", 
	"__N_3", "__N_4", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', ',', '(', ')', 's', 'o', 
	'm', 'e', 't', 'i', '<', '>', 'a', 'l', 'w', 'y', 
	'[', ']', '?', 'x', '-', '|', '^', '&', '!', 'T', 
	'F', 'S', '+', '\'', '1', '0', '$', '{', '}', '_', 
	'\t', '\n', '\r', '#', 'f', 'h', 'c', 'n', 'g', 'u', 
	'b', 'r', 'v', 'z', 'q', 'p', 'd', '/', '"', '%', 
	'V', 'H', 'I', 
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
			11, 52
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			20, 22, 116, 136, 154, 158, 170, 171, 178, 179,
			354, 396
		},
		.to_inline = {
			{ 14, 60, 14 },
			{ 21, 113, 21 },
			{ 43 },
			{ 47 },
			{ 149 },
			{ 161 },
			{ 181 },
			{ 197 },
			{ 199 },
			{ 200 },
			{ 201 },
			{ 202 },
			{ 204 },
			{ 205 },
			{ 206 },
			{ 247 },
			{ 258 },
			{ 273 },
			{ 278 },
			{ 303 },
			{ 315 },
			{ 356 }
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
//G14:  ref(18)              => sym(22) __E_ref_3(23) _(11) ref_args(25).
	p(NT(18), (NT(22)+NT(23)+NT(11)+NT(25)));
//G15:  ref_arg(28)          => bf(21).
	p(NT(28), (NT(21)));
//G16:  __E___E_ref_args_4_5(27) => _(11) ref_arg(28).
	p(NT(27), (NT(11)+NT(28)));
//G17:  __E___E_ref_args_4_6(29) => _(11) ',' _(11) ref_arg(28).
	p(NT(29), (NT(11)+T(4)+NT(11)+NT(28)));
//G18:  __E___E_ref_args_4_7(30) => null.
	p(NT(30), (nul));
//G19:  __E___E_ref_args_4_7(30) => __E___E_ref_args_4_6(29) __E___E_ref_args_4_7(30).
	p(NT(30), (NT(29)+NT(30)));
//G20:  __E_ref_args_4(26)   => __E___E_ref_args_4_5(27) __E___E_ref_args_4_7(30).
	p(NT(26), (NT(27)+NT(30)));
//G21:  __E_ref_args_4(26)   => null.
	p(NT(26), (nul));
//G22:  ref_args(25)         => '(' __E_ref_args_4(26) _(11) ')'.
	p(NT(25), (T(5)+NT(26)+NT(11)+T(6)));
//G23:  __E___E___E_library_8_9_10(36) => wff_rule(37).
	p(NT(36), (NT(37)));
//G24:  __E___E___E_library_8_9_10(36) => bf_rule(38).
	p(NT(36), (NT(38)));
//G25:  rule(35)             => __E___E___E_library_8_9_10(36).
	p(NT(35), (NT(36)));
//G26:  __E___E_library_8_9(34) => _(11) rule(35).
	p(NT(34), (NT(11)+NT(35)));
//G27:  __E___E_library_8_11(39) => null.
	p(NT(39), (nul));
//G28:  __E___E_library_8_11(39) => __E___E_library_8_9(34) __E___E_library_8_11(39).
	p(NT(39), (NT(34)+NT(39)));
//G29:  __E_library_8(33)    => __E___E_library_8_11(39).
	p(NT(33), (NT(39)));
//G30:  rules(32)            => __E_library_8(33).
	p(NT(32), (NT(33)));
//G31:  library(31)          => rules(32).
	p(NT(31), (NT(32)));
//G32:  wff_matcher(40)      => wff(14).
	p(NT(40), (NT(14)));
//G33:  __E_wff_rule_12(42)  => wff(14).
	p(NT(42), (NT(14)));
//G34:  __E_wff_rule_12(42)  => wff_cb(43).
	p(NT(42), (NT(43)));
//G35:  wff_body(41)         => __E_wff_rule_12(42).
	p(NT(41), (NT(42)));
//G36:  wff_rule(37)         => wff_matcher(40) _(11) ':' ':' '=' _(11) wff_body(41) _(11) '.'.
	p(NT(37), (NT(40)+NT(11)+T(2)+T(2)+T(3)+NT(11)+NT(41)+NT(11)+T(1)));
//G37:  bf_matcher(44)       => bf(21).
	p(NT(44), (NT(21)));
//G38:  __E_bf_rule_13(46)   => bf(21).
	p(NT(46), (NT(21)));
//G39:  __E_bf_rule_13(46)   => bf_cb(47).
	p(NT(46), (NT(47)));
//G40:  bf_body(45)          => __E_bf_rule_13(46).
	p(NT(45), (NT(46)));
//G41:  bf_rule(38)          => bf_matcher(44) _(11) ':' '=' _(11) bf_body(45) _(11) '.'.
	p(NT(38), (NT(44)+NT(11)+T(2)+T(3)+NT(11)+NT(45)+NT(11)+T(1)));
//G42:  builder(48)          => _(11) builder_head(49) _(11) builder_body(50) _(11) '.'.
	p(NT(48), (NT(11)+NT(49)+NT(11)+NT(50)+NT(11)+T(1)));
//G43:  __E_builder_head_14(51) => __(52) capture(20).
	p(NT(51), (NT(52)+NT(20)));
//G44:  __E_builder_head_15(53) => null.
	p(NT(53), (nul));
//G45:  __E_builder_head_15(53) => __E_builder_head_14(51) __E_builder_head_15(53).
	p(NT(53), (NT(51)+NT(53)));
//G46:  builder_head(49)     => '(' _(11) capture(20) __E_builder_head_15(53) _(11) ')'.
	p(NT(49), (T(5)+NT(11)+NT(20)+NT(53)+NT(11)+T(6)));
//G47:  __E_builder_body_16(55) => '=' ':' _(11) bf(21).
	p(NT(55), (T(3)+T(2)+NT(11)+NT(21)));
//G48:  bf_builder_body(54)  => __E_builder_body_16(55).
	p(NT(54), (NT(55)));
//G49:  builder_body(50)     => bf_builder_body(54).
	p(NT(50), (NT(54)));
//G50:  __E_builder_body_17(57) => '=' ':' ':' _(11) wff(14).
	p(NT(57), (T(3)+T(2)+T(2)+NT(11)+NT(14)));
//G51:  wff_builder_body(56) => __E_builder_body_17(57).
	p(NT(56), (NT(57)));
//G52:  builder_body(50)     => wff_builder_body(56).
	p(NT(50), (NT(56)));
//G53:  __E_tau_constant_source_18(59) => _(11) '.'.
	p(NT(59), (NT(11)+T(1)));
//G54:  __E_tau_constant_source_18(59) => null.
	p(NT(59), (nul));
//G55:  tau_constant_source(58) => rec_relations(12) _(11) main(13) __E_tau_constant_source_18(59).
	p(NT(58), (NT(12)+NT(11)+NT(13)+NT(59)));
//G56:  __E_wff_19(61)       => '(' _(11) wff(14) _(11) ')'.
	p(NT(61), (T(5)+NT(11)+NT(14)+NT(11)+T(6)));
//G57:  wff_parenthesis(60)  => __E_wff_19(61).
	p(NT(60), (NT(61)));
//G58:  wff(14)              => wff_parenthesis(60).
	p(NT(14), (NT(60)));
//G59:  __E___E_wff_20_21(64) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(64), (T(7)+T(8)+T(9)+T(10)+T(11)+T(12)+T(9)+T(10)+T(7)));
//G60:  __E___E_wff_20_21(64) => '<' '>'.
	p(NT(64), (T(13)+T(14)));
//G61:  __E_wff_20(63)       => __E___E_wff_20_21(64) _(11) wff(14).
	p(NT(63), (NT(64)+NT(11)+NT(14)));
//G62:  wff_sometimes(62)    => __E_wff_20(63).
	p(NT(62), (NT(63)));
//G63:  wff(14)              => wff_sometimes(62).
	p(NT(14), (NT(62)));
//G64:  __E___E_wff_22_23(67) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(67), (T(15)+T(16)+T(17)+T(15)+T(18)+T(7)));
//G65:  __E___E_wff_22_23(67) => '[' ']'.
	p(NT(67), (T(19)+T(20)));
//G66:  __E_wff_22(66)       => __E___E_wff_22_23(67) _(11) wff(14).
	p(NT(66), (NT(67)+NT(11)+NT(14)));
//G67:  wff_always(65)       => __E_wff_22(66).
	p(NT(65), (NT(66)));
//G68:  wff(14)              => wff_always(65).
	p(NT(14), (NT(65)));
//G69:  __E_wff_24(69)       => wff(14) _(11) '?' _(11) wff(14) _(11) ':' _(11) wff(14).
	p(NT(69), (NT(14)+NT(11)+T(21)+NT(11)+NT(14)+NT(11)+T(2)+NT(11)+NT(14)));
//G70:  wff_conditional(68)  => __E_wff_24(69).
	p(NT(68), (NT(69)));
//G71:  wff(14)              => wff_conditional(68).
	p(NT(14), (NT(68)));
//G72:  __E_wff_25(71)       => 'a' 'l' 'l' __(52) q_vars(72) __(52) wff(14).
	p(NT(71), (T(15)+T(16)+T(16)+NT(52)+NT(72)+NT(52)+NT(14)));
//G73:  wff_all(70)          => __E_wff_25(71).
	p(NT(70), (NT(71)));
//G74:  wff(14)              => wff_all(70).
	p(NT(14), (NT(70)));
//G75:  __E_wff_26(74)       => 'e' 'x' __(52) q_vars(72) __(52) wff(14).
	p(NT(74), (T(10)+T(22)+NT(52)+NT(72)+NT(52)+NT(14)));
//G76:  wff_ex(73)           => __E_wff_26(74).
	p(NT(73), (NT(74)));
//G77:  wff(14)              => wff_ex(73).
	p(NT(14), (NT(73)));
//G78:  wff_ref(75)          => ref(18).
	p(NT(75), (NT(18)));
//G79:  wff(14)              => wff_ref(75).
	p(NT(14), (NT(75)));
//G80:  wff(14)              => constraint(76).
	p(NT(14), (NT(76)));
//G81:  __E_wff_27(78)       => wff(14) _(11) '-' '>' _(11) wff(14).
	p(NT(78), (NT(14)+NT(11)+T(23)+T(14)+NT(11)+NT(14)));
//G82:  wff_imply(77)        => __E_wff_27(78).
	p(NT(77), (NT(78)));
//G83:  wff(14)              => wff_imply(77).
	p(NT(14), (NT(77)));
//G84:  __E_wff_28(80)       => wff(14) _(11) '<' '-' '>' _(11) wff(14).
	p(NT(80), (NT(14)+NT(11)+T(13)+T(23)+T(14)+NT(11)+NT(14)));
//G85:  wff_equiv(79)        => __E_wff_28(80).
	p(NT(79), (NT(80)));
//G86:  wff(14)              => wff_equiv(79).
	p(NT(14), (NT(79)));
//G87:  __E_wff_29(82)       => wff(14) _(11) '|' '|' _(11) wff(14).
	p(NT(82), (NT(14)+NT(11)+T(24)+T(24)+NT(11)+NT(14)));
//G88:  wff_or(81)           => __E_wff_29(82).
	p(NT(81), (NT(82)));
//G89:  wff(14)              => wff_or(81).
	p(NT(14), (NT(81)));
//G90:  __E_wff_30(84)       => wff(14) _(11) '^' _(11) wff(14).
	p(NT(84), (NT(14)+NT(11)+T(25)+NT(11)+NT(14)));
//G91:  wff_xor(83)          => __E_wff_30(84).
	p(NT(83), (NT(84)));
//G92:  wff(14)              => wff_xor(83).
	p(NT(14), (NT(83)));
//G93:  __E_wff_31(86)       => wff(14) _(11) '&' '&' _(11) wff(14).
	p(NT(86), (NT(14)+NT(11)+T(26)+T(26)+NT(11)+NT(14)));
//G94:  wff_and(85)          => __E_wff_31(86).
	p(NT(85), (NT(86)));
//G95:  wff(14)              => wff_and(85).
	p(NT(14), (NT(85)));
//G96:  __E_wff_32(88)       => '!' _(11) wff(14).
	p(NT(88), (T(27)+NT(11)+NT(14)));
//G97:  wff_neg(87)          => __E_wff_32(88).
	p(NT(87), (NT(88)));
//G98:  wff(14)              => wff_neg(87).
	p(NT(14), (NT(87)));
//G99:  wff_t(89)            => 'T'.
	p(NT(89), (T(28)));
//G100: wff(14)              => wff_t(89).
	p(NT(14), (NT(89)));
//G101: wff_f(90)            => 'F'.
	p(NT(90), (T(29)));
//G102: wff(14)              => wff_f(90).
	p(NT(14), (NT(90)));
//G103: __E_wff_33(92)       => bf(21) _(11) '<' '=' _(11) bf(21) _(11) '<' '=' _(11) bf(21).
	p(NT(92), (NT(21)+NT(11)+T(13)+T(3)+NT(11)+NT(21)+NT(11)+T(13)+T(3)+NT(11)+NT(21)));
//G104: bf_interval(91)      => __E_wff_33(92).
	p(NT(91), (NT(92)));
//G105: wff(14)              => bf_interval(91).
	p(NT(14), (NT(91)));
//G106: __E_wff_34(94)       => bf(21) _(11) '=' _(11) bf(21).
	p(NT(94), (NT(21)+NT(11)+T(3)+NT(11)+NT(21)));
//G107: bf_eq(93)            => __E_wff_34(94).
	p(NT(93), (NT(94)));
//G108: wff(14)              => bf_eq(93).
	p(NT(14), (NT(93)));
//G109: __E_wff_35(96)       => bf(21) _(11) '!' '=' _(11) bf(21).
	p(NT(96), (NT(21)+NT(11)+T(27)+T(3)+NT(11)+NT(21)));
//G110: bf_neq(95)           => __E_wff_35(96).
	p(NT(95), (NT(96)));
//G111: wff(14)              => bf_neq(95).
	p(NT(14), (NT(95)));
//G112: __E_wff_36(98)       => bf(21) _(11) '<' '=' _(11) bf(21).
	p(NT(98), (NT(21)+NT(11)+T(13)+T(3)+NT(11)+NT(21)));
//G113: bf_less_equal(97)    => __E_wff_36(98).
	p(NT(97), (NT(98)));
//G114: wff(14)              => bf_less_equal(97).
	p(NT(14), (NT(97)));
//G115: __E_wff_37(100)      => bf(21) _(11) '!' '<' '=' _(11) bf(21).
	p(NT(100), (NT(21)+NT(11)+T(27)+T(13)+T(3)+NT(11)+NT(21)));
//G116: bf_nleq(99)          => __E_wff_37(100).
	p(NT(99), (NT(100)));
//G117: wff(14)              => bf_nleq(99).
	p(NT(14), (NT(99)));
//G118: __E_wff_38(102)      => bf(21) _(11) '>' _(11) bf(21).
	p(NT(102), (NT(21)+NT(11)+T(14)+NT(11)+NT(21)));
//G119: bf_greater(101)      => __E_wff_38(102).
	p(NT(101), (NT(102)));
//G120: wff(14)              => bf_greater(101).
	p(NT(14), (NT(101)));
//G121: __E_wff_39(104)      => bf(21) _(11) '!' '>' _(11) bf(21).
	p(NT(104), (NT(21)+NT(11)+T(27)+T(14)+NT(11)+NT(21)));
//G122: bf_ngreater(103)     => __E_wff_39(104).
	p(NT(103), (NT(104)));
//G123: wff(14)              => bf_ngreater(103).
	p(NT(14), (NT(103)));
//G124: __E_wff_40(106)      => bf(21) _(11) '>' '=' _(11) bf(21).
	p(NT(106), (NT(21)+NT(11)+T(14)+T(3)+NT(11)+NT(21)));
//G125: bf_greater_equal(105) => __E_wff_40(106).
	p(NT(105), (NT(106)));
//G126: wff(14)              => bf_greater_equal(105).
	p(NT(14), (NT(105)));
//G127: __E_wff_41(108)      => bf(21) _(11) '!' '>' '=' _(11) bf(21).
	p(NT(108), (NT(21)+NT(11)+T(27)+T(14)+T(3)+NT(11)+NT(21)));
//G128: bf_ngeq(107)         => __E_wff_41(108).
	p(NT(107), (NT(108)));
//G129: wff(14)              => bf_ngeq(107).
	p(NT(14), (NT(107)));
//G130: __E_wff_42(110)      => bf(21) _(11) '<' _(11) bf(21).
	p(NT(110), (NT(21)+NT(11)+T(13)+NT(11)+NT(21)));
//G131: bf_less(109)         => __E_wff_42(110).
	p(NT(109), (NT(110)));
//G132: wff(14)              => bf_less(109).
	p(NT(14), (NT(109)));
//G133: __E_wff_43(112)      => bf(21) _(11) '!' '<' _(11) bf(21).
	p(NT(112), (NT(21)+NT(11)+T(27)+T(13)+NT(11)+NT(21)));
//G134: bf_nless(111)        => __E_wff_43(112).
	p(NT(111), (NT(112)));
//G135: wff(14)              => bf_nless(111).
	p(NT(14), (NT(111)));
//G136: wff(14)              => capture(20).
	p(NT(14), (NT(20)));
//G137: __E_bf_44(114)       => '(' _(11) bf(21) _(11) ')'.
	p(NT(114), (T(5)+NT(11)+NT(21)+NT(11)+T(6)));
//G138: bf_parenthesis(113)  => __E_bf_44(114).
	p(NT(113), (NT(114)));
//G139: bf(21)               => bf_parenthesis(113).
	p(NT(21), (NT(113)));
//G140: bf(21)               => bf_constant(115).
	p(NT(21), (NT(115)));
//G141: bf(21)               => variable(116).
	p(NT(21), (NT(116)));
//G142: __E_bf_45(118)       => 'S' _(11) '(' _(11) bf(21) _(11) ')'.
	p(NT(118), (T(30)+NT(11)+T(5)+NT(11)+NT(21)+NT(11)+T(6)));
//G143: bf_splitter(117)     => __E_bf_45(118).
	p(NT(117), (NT(118)));
//G144: bf(21)               => bf_splitter(117).
	p(NT(21), (NT(117)));
//G145: bf_ref(119)          => ref(18).
	p(NT(119), (NT(18)));
//G146: bf(21)               => bf_ref(119).
	p(NT(21), (NT(119)));
//G147: __E_bf_46(121)       => bf(21) _(11) '|' _(11) bf(21).
	p(NT(121), (NT(21)+NT(11)+T(24)+NT(11)+NT(21)));
//G148: bf_or(120)           => __E_bf_46(121).
	p(NT(120), (NT(121)));
//G149: bf(21)               => bf_or(120).
	p(NT(21), (NT(120)));
//G150: __E_bf_47(123)       => bf(21) _(11) '+' _(11) bf(21).
	p(NT(123), (NT(21)+NT(11)+T(31)+NT(11)+NT(21)));
//G151: bf_xor(122)          => __E_bf_47(123).
	p(NT(122), (NT(123)));
//G152: bf(21)               => bf_xor(122).
	p(NT(21), (NT(122)));
//G153: __E___E_bf_48_49(126) => _(11) '&' _(11).
	p(NT(126), (NT(11)+T(26)+NT(11)));
//G154: __E___E_bf_48_49(126) => __(52).
	p(NT(126), (NT(52)));
//G155: __E_bf_48(125)       => bf(21) __E___E_bf_48_49(126) bf(21).
	p(NT(125), (NT(21)+NT(126)+NT(21)));
//G156: bf_and(124)          => __E_bf_48(125).
	p(NT(124), (NT(125)));
//G157: bf(21)               => bf_and(124).
	p(NT(21), (NT(124)));
//G158: __E_bf_50(128)       => bf(21) _(11) '\''.
	p(NT(128), (NT(21)+NT(11)+T(32)));
//G159: bf_neg(127)          => __E_bf_50(128).
	p(NT(127), (NT(128)));
//G160: bf(21)               => bf_neg(127).
	p(NT(21), (NT(127)));
//G161: bf_t(129)            => '1'.
	p(NT(129), (T(33)));
//G162: bf(21)               => bf_t(129).
	p(NT(21), (NT(129)));
//G163: bf_f(130)            => '0'.
	p(NT(130), (T(34)));
//G164: bf(21)               => bf_f(130).
	p(NT(21), (NT(130)));
//G165: bf(21)               => capture(20).
	p(NT(21), (NT(20)));
//G166: __E_bf_and_51(132)   => bf_parenthesis(113).
	p(NT(132), (NT(113)));
//G167: __E_bf_and_51(132)   => variable(116).
	p(NT(132), (NT(116)));
//G168: __E_bf_and_51(132)   => bf_splitter(117).
	p(NT(132), (NT(117)));
//G169: __E_bf_and_51(132)   => bf_ref(119).
	p(NT(132), (NT(119)));
//G170: __E_bf_and_51(132)   => bf_or(120).
	p(NT(132), (NT(120)));
//G171: __E_bf_and_51(132)   => bf_xor(122).
	p(NT(132), (NT(122)));
//G172: __E_bf_and_51(132)   => bf_neg(127).
	p(NT(132), (NT(127)));
//G173: __E_bf_and_51(132)   => capture(20).
	p(NT(132), (NT(20)));
//G174: __E_bf_and_51(132)   => uninterpreted_constant(133).
	p(NT(132), (NT(133)));
//G175: bf_and_nosep_1st_op(131) => __E_bf_and_51(132).
	p(NT(131), (NT(132)));
//G176: bf_and(124)          => bf_and_nosep_1st_op(131) bf(21).
	p(NT(124), (NT(131)+NT(21)));
//G177: __E_constraint_52(135) => '[' ctnvar(136) _(11) '!' '=' _(11) num(137) ']'.
	p(NT(135), (T(19)+NT(136)+NT(11)+T(27)+T(3)+NT(11)+NT(137)+T(20)));
//G178: __E_constraint_52(135) => '[' num(137) _(11) '!' '=' _(11) ctnvar(136) ']'.
	p(NT(135), (T(19)+NT(137)+NT(11)+T(27)+T(3)+NT(11)+NT(136)+T(20)));
//G179: ctn_neq(134)         => __E_constraint_52(135).
	p(NT(134), (NT(135)));
//G180: constraint(76)       => ctn_neq(134).
	p(NT(76), (NT(134)));
//G181: __E_constraint_53(139) => '[' ctnvar(136) _(11) '=' _(11) num(137) ']'.
	p(NT(139), (T(19)+NT(136)+NT(11)+T(3)+NT(11)+NT(137)+T(20)));
//G182: __E_constraint_53(139) => '[' num(137) _(11) '=' _(11) ctnvar(136) ']'.
	p(NT(139), (T(19)+NT(137)+NT(11)+T(3)+NT(11)+NT(136)+T(20)));
//G183: ctn_eq(138)          => __E_constraint_53(139).
	p(NT(138), (NT(139)));
//G184: constraint(76)       => ctn_eq(138).
	p(NT(76), (NT(138)));
//G185: __E_constraint_54(141) => '[' ctnvar(136) _(11) '>' '=' _(11) num(137) ']'.
	p(NT(141), (T(19)+NT(136)+NT(11)+T(14)+T(3)+NT(11)+NT(137)+T(20)));
//G186: __E_constraint_54(141) => '[' num(137) _(11) '>' '=' _(11) ctnvar(136) ']'.
	p(NT(141), (T(19)+NT(137)+NT(11)+T(14)+T(3)+NT(11)+NT(136)+T(20)));
//G187: ctn_greater_equal(140) => __E_constraint_54(141).
	p(NT(140), (NT(141)));
//G188: constraint(76)       => ctn_greater_equal(140).
	p(NT(76), (NT(140)));
//G189: __E_constraint_55(143) => '[' ctnvar(136) _(11) '>' _(11) num(137) ']'.
	p(NT(143), (T(19)+NT(136)+NT(11)+T(14)+NT(11)+NT(137)+T(20)));
//G190: __E_constraint_55(143) => '[' num(137) _(11) '>' _(11) ctnvar(136) ']'.
	p(NT(143), (T(19)+NT(137)+NT(11)+T(14)+NT(11)+NT(136)+T(20)));
//G191: ctn_greater(142)     => __E_constraint_55(143).
	p(NT(142), (NT(143)));
//G192: constraint(76)       => ctn_greater(142).
	p(NT(76), (NT(142)));
//G193: __E_constraint_56(145) => '[' ctnvar(136) _(11) '<' '=' _(11) num(137) ']'.
	p(NT(145), (T(19)+NT(136)+NT(11)+T(13)+T(3)+NT(11)+NT(137)+T(20)));
//G194: __E_constraint_56(145) => '[' num(137) _(11) '<' '=' _(11) ctnvar(136) ']'.
	p(NT(145), (T(19)+NT(137)+NT(11)+T(13)+T(3)+NT(11)+NT(136)+T(20)));
//G195: ctn_less_equal(144)  => __E_constraint_56(145).
	p(NT(144), (NT(145)));
//G196: constraint(76)       => ctn_less_equal(144).
	p(NT(76), (NT(144)));
//G197: __E_constraint_57(147) => '[' ctnvar(136) _(11) '<' _(11) num(137) ']'.
	p(NT(147), (T(19)+NT(136)+NT(11)+T(13)+NT(11)+NT(137)+T(20)));
//G198: __E_constraint_57(147) => '[' num(137) _(11) '<' _(11) ctnvar(136) ']'.
	p(NT(147), (T(19)+NT(137)+NT(11)+T(13)+NT(11)+NT(136)+T(20)));
//G199: ctn_less(146)        => __E_constraint_57(147).
	p(NT(146), (NT(147)));
//G200: constraint(76)       => ctn_less(146).
	p(NT(76), (NT(146)));
//G201: __E_variable_58(148) => null.
	p(NT(148), (nul));
//G202: __E_variable_58(148) => digit(3) __E_variable_58(148).
	p(NT(148), (NT(3)+NT(148)));
//G203: variable(116)        => alpha(5) __E_variable_58(148).	 # guarded: charvar
	p(NT(116), (NT(5)+NT(148)));
	p.back().guard = "charvar";
//G204: variable(116)        => chars(149).	 # guarded: var
	p(NT(116), (NT(149)));
	p.back().guard = "var";
//G205: __E_variable_59(151) => in(152).
	p(NT(151), (NT(152)));
//G206: __E_variable_59(151) => out(153).
	p(NT(151), (NT(153)));
//G207: io_var(150)          => __E_variable_59(151).
	p(NT(150), (NT(151)));
//G208: variable(116)        => io_var(150).
	p(NT(116), (NT(150)));
//G209: variable(116)        => uninterpreted_constant(133).
	p(NT(116), (NT(133)));
//G210: __E___E_in_60_61(156) => digit(3).
	p(NT(156), (NT(3)));
//G211: __E___E_in_60_61(156) => digit(3) __E___E_in_60_61(156).
	p(NT(156), (NT(3)+NT(156)));
//G212: __E_in_60(155)       => 'i' __E___E_in_60_61(156).
	p(NT(155), (T(12)+NT(156)));
//G213: in_var_name(154)     => __E_in_60(155).
	p(NT(154), (NT(155)));
//G214: in(152)              => in_var_name(154) '[' offset(157) ']'.
	p(NT(152), (NT(154)+T(19)+NT(157)+T(20)));
//G215: __E___E_out_62_63(160) => digit(3).
	p(NT(160), (NT(3)));
//G216: __E___E_out_62_63(160) => digit(3) __E___E_out_62_63(160).
	p(NT(160), (NT(3)+NT(160)));
//G217: __E_out_62(159)      => 'o' __E___E_out_62_63(160).
	p(NT(159), (T(8)+NT(160)));
//G218: out_var_name(158)    => __E_out_62(159).
	p(NT(158), (NT(159)));
//G219: out(153)             => out_var_name(158) '[' offset(157) ']'.
	p(NT(153), (NT(158)+T(19)+NT(157)+T(20)));
//G220: ctnvar(136)          => chars(149).
	p(NT(136), (NT(149)));
//G221: capture(20)          => '$' chars(149).
	p(NT(20), (T(35)+NT(149)));
//G222: __E_q_vars_64(162)   => _(11) ',' _(11) q_var(161).
	p(NT(162), (NT(11)+T(4)+NT(11)+NT(161)));
//G223: __E_q_vars_65(163)   => null.
	p(NT(163), (nul));
//G224: __E_q_vars_65(163)   => __E_q_vars_64(162) __E_q_vars_65(163).
	p(NT(163), (NT(162)+NT(163)));
//G225: q_vars(72)           => q_var(161) __E_q_vars_65(163).
	p(NT(72), (NT(161)+NT(163)));
//G226: q_var(161)           => capture(20).
	p(NT(161), (NT(20)));
//G227: __N_0(397)           => uninterpreted_constant(133).
	p(NT(397), (NT(133)));
//G228: q_var(161)           => variable(116) & ~( __N_0(397) ).	 # conjunctive
	p(NT(161), (NT(116)) & ~(NT(397)));
//G229: __E_offsets_66(164)  => _(11) ',' _(11) offset(157).
	p(NT(164), (NT(11)+T(4)+NT(11)+NT(157)));
//G230: __E_offsets_67(165)  => null.
	p(NT(165), (nul));
//G231: __E_offsets_67(165)  => __E_offsets_66(164) __E_offsets_67(165).
	p(NT(165), (NT(164)+NT(165)));
//G232: offsets(24)          => '[' _(11) offset(157) __E_offsets_67(165) _(11) ']'.
	p(NT(24), (T(19)+NT(11)+NT(157)+NT(165)+NT(11)+T(20)));
//G233: offset(157)          => num(137).
	p(NT(157), (NT(137)));
//G234: offset(157)          => capture(20).
	p(NT(157), (NT(20)));
//G235: offset(157)          => shift(166).
	p(NT(157), (NT(166)));
//G236: __N_1(398)           => io_var(150).
	p(NT(398), (NT(150)));
//G237: __E_offset_68(167)   => variable(116) & ~( __N_1(398) ).	 # conjunctive
	p(NT(167), (NT(116)) & ~(NT(398)));
//G238: offset(157)          => __E_offset_68(167).
	p(NT(157), (NT(167)));
//G239: __E_shift_69(168)    => capture(20).
	p(NT(168), (NT(20)));
//G240: __N_2(399)           => io_var(150).
	p(NT(399), (NT(150)));
//G241: __E___E_shift_69_70(169) => variable(116) & ~( __N_2(399) ).	 # conjunctive
	p(NT(169), (NT(116)) & ~(NT(399)));
//G242: __E_shift_69(168)    => __E___E_shift_69_70(169).
	p(NT(168), (NT(169)));
//G243: shift(166)           => __E_shift_69(168) _(11) '-' _(11) num(137).
	p(NT(166), (NT(168)+NT(11)+T(23)+NT(11)+NT(137)));
//G244: num(137)             => digits(170).
	p(NT(137), (NT(170)));
//G245: __E___E_uninterpreted_constant_71_72(173) => chars(149).
	p(NT(173), (NT(149)));
//G246: __E___E_uninterpreted_constant_71_72(173) => _(11).
	p(NT(173), (NT(11)));
//G247: __E_uninterpreted_constant_71(172) => __E___E_uninterpreted_constant_71_72(173) ':' chars(149).
	p(NT(172), (NT(173)+T(2)+NT(149)));
//G248: uninter_const_name(171) => __E_uninterpreted_constant_71(172).
	p(NT(171), (NT(172)));
//G249: uninterpreted_constant(133) => '<' uninter_const_name(171) _(11) '>'.
	p(NT(133), (T(13)+NT(171)+NT(11)+T(14)));
//G250: __E_bf_constant_73(175) => capture(20).
	p(NT(175), (NT(20)));
//G251: __E_bf_constant_73(175) => binding(176).
	p(NT(175), (NT(176)));
//G252: constant(174)        => __E_bf_constant_73(175).
	p(NT(174), (NT(175)));
//G253: type(178)            => chars(149).
	p(NT(178), (NT(149)));
//G254: __E_bf_constant_74(177) => _(11) ':' _(11) type(178).
	p(NT(177), (NT(11)+T(2)+NT(11)+NT(178)));
//G255: __E_bf_constant_74(177) => null.
	p(NT(177), (nul));
//G256: bf_constant(115)     => '{' _(11) constant(174) _(11) '}' __E_bf_constant_74(177).
	p(NT(115), (T(36)+NT(11)+NT(174)+NT(11)+T(37)+NT(177)));
//G257: __E___E___E_binding_75_76_77(183) => src_c(181).
	p(NT(183), (NT(181)));
//G258: __E___E___E_binding_75_76_77(183) => space(2).
	p(NT(183), (NT(2)));
//G259: __E___E___E_binding_75_76_78(184) => null.
	p(NT(184), (nul));
//G260: __E___E___E_binding_75_76_78(184) => __E___E___E_binding_75_76_77(183) __E___E___E_binding_75_76_78(184).
	p(NT(184), (NT(183)+NT(184)));
//G261: __E___E_binding_75_76(182) => __E___E___E_binding_75_76_78(184) src_c(181).
	p(NT(182), (NT(184)+NT(181)));
//G262: __E___E_binding_75_76(182) => null.
	p(NT(182), (nul));
//G263: __E_binding_75(180)  => src_c(181) __E___E_binding_75_76(182).
	p(NT(180), (NT(181)+NT(182)));
//G264: source(179)          => __E_binding_75(180).
	p(NT(179), (NT(180)));
//G265: binding(176)         => source(179).
	p(NT(176), (NT(179)));
//G266: src_c(181)           => alnum(6).
	p(NT(181), (NT(6)));
//G267: __N_3(400)           => '{'.
	p(NT(400), (T(36)));
//G268: __N_4(401)           => '}'.
	p(NT(401), (T(37)));
//G269: src_c(181)           => ~( __N_3(400) ) & ~( __N_4(401) ) & punct(7).	 # conjunctive
	p(NT(181), ~(NT(400)) & ~(NT(401)) & (NT(7)));
//G270: __E_src_c_79(185)    => src_c(181).
	p(NT(185), (NT(181)));
//G271: __E_src_c_79(185)    => space(2).
	p(NT(185), (NT(2)));
//G272: __E_src_c_80(186)    => null.
	p(NT(186), (nul));
//G273: __E_src_c_80(186)    => __E_src_c_79(185) __E_src_c_80(186).
	p(NT(186), (NT(185)+NT(186)));
//G274: src_c(181)           => '{' __E_src_c_80(186) '}'.
	p(NT(181), (T(36)+NT(186)+T(37)));
//G275: __E_chars_81(187)    => alnum(6).
	p(NT(187), (NT(6)));
//G276: __E_chars_81(187)    => '_'.
	p(NT(187), (T(38)));
//G277: __E_chars_82(188)    => null.
	p(NT(188), (nul));
//G278: __E_chars_82(188)    => __E_chars_81(187) __E_chars_82(188).
	p(NT(188), (NT(187)+NT(188)));
//G279: chars(149)           => alpha(5) __E_chars_82(188).
	p(NT(149), (NT(5)+NT(188)));
//G280: __E_digits_83(189)   => digit(3).
	p(NT(189), (NT(3)));
//G281: __E_digits_83(189)   => digit(3) __E_digits_83(189).
	p(NT(189), (NT(3)+NT(189)));
//G282: digits(170)          => __E_digits_83(189).
	p(NT(170), (NT(189)));
//G283: sym(22)              => chars(149).
	p(NT(22), (NT(149)));
//G284: extra(190)           => chars(149).
	p(NT(190), (NT(149)));
//G285: __E_comment_84(192)  => printable(8).
	p(NT(192), (NT(8)));
//G286: __E_comment_84(192)  => '\t'.
	p(NT(192), (T(39)));
//G287: __E_comment_85(193)  => null.
	p(NT(193), (nul));
//G288: __E_comment_85(193)  => __E_comment_84(192) __E_comment_85(193).
	p(NT(193), (NT(192)+NT(193)));
//G289: __E_comment_86(194)  => '\n'.
	p(NT(194), (T(40)));
//G290: __E_comment_86(194)  => '\r'.
	p(NT(194), (T(41)));
//G291: __E_comment_86(194)  => eof(1).
	p(NT(194), (NT(1)));
//G292: comment(191)         => '#' __E_comment_85(193) __E_comment_86(194).
	p(NT(191), (T(42)+NT(193)+NT(194)));
//G293: __E____87(195)       => space(2).
	p(NT(195), (NT(2)));
//G294: __E____87(195)       => comment(191).
	p(NT(195), (NT(191)));
//G295: __(52)               => __E____87(195) _(11).
	p(NT(52), (NT(195)+NT(11)));
//G296: __E___88(196)        => __(52).
	p(NT(196), (NT(52)));
//G297: __E___88(196)        => null.
	p(NT(196), (nul));
//G298: _(11)                => __E___88(196).
	p(NT(11), (NT(196)));
//G299: bf_cb_arg(198)       => bf(21).
	p(NT(198), (NT(21)));
//G300: bf_cb_args1(197)     => __(52) bf_cb_arg(198).
	p(NT(197), (NT(52)+NT(198)));
//G301: bf_cb_args2(199)     => bf_cb_args1(197) bf_cb_args1(197).
	p(NT(199), (NT(197)+NT(197)));
//G302: bf_cb_args3(200)     => bf_cb_args2(199) bf_cb_args1(197).
	p(NT(200), (NT(199)+NT(197)));
//G303: bf_cb_args4(201)     => bf_cb_args3(200) bf_cb_args1(197).
	p(NT(201), (NT(200)+NT(197)));
//G304: wff_cb_arg(203)      => wff(14).
	p(NT(203), (NT(14)));
//G305: wff_cb_args1(202)    => __(52) wff_cb_arg(203).
	p(NT(202), (NT(52)+NT(203)));
//G306: wff_cb_args2(204)    => wff_cb_args1(202) wff_cb_args1(202).
	p(NT(204), (NT(202)+NT(202)));
//G307: wff_cb_args3(205)    => wff_cb_args2(204) wff_cb_args1(202).
	p(NT(205), (NT(204)+NT(202)));
//G308: wff_cb_args4(206)    => wff_cb_args3(205) wff_cb_args1(202).
	p(NT(206), (NT(205)+NT(202)));
//G309: __E_wff_cb_89(208)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(204).
	p(NT(208), (T(17)+T(43)+T(43)+T(38)+T(44)+T(15)+T(7)+T(38)+T(45)+T(16)+T(15)+T(7)+T(44)+T(12)+T(46)+T(47)+T(38)+T(7)+T(48)+T(49)+T(43)+T(8)+T(50)+T(9)+T(48)+T(16)+T(15)+T(7)+T(38)+T(45)+T(49)+NT(204)));
//G310: wff_has_clashing_subformulas_cb(207) => __E_wff_cb_89(208).
	p(NT(207), (NT(208)));
//G311: wff_cb(43)           => wff_has_clashing_subformulas_cb(207).
	p(NT(43), (NT(207)));
//G312: __E_wff_cb_90(210)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(205).
	p(NT(210), (T(17)+T(43)+T(43)+T(38)+T(44)+T(15)+T(7)+T(38)+T(7)+T(48)+T(49)+T(43)+T(8)+T(50)+T(9)+T(48)+T(16)+T(15)+T(38)+T(45)+T(49)+NT(205)));
//G313: wff_has_subformula_cb(209) => __E_wff_cb_90(210).
	p(NT(209), (NT(210)));
//G314: wff_cb(43)           => wff_has_subformula_cb(209).
	p(NT(43), (NT(209)));
//G315: __E_wff_cb_91(212)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(204).
	p(NT(212), (T(17)+T(43)+T(43)+T(38)+T(50)+T(10)+T(9)+T(8)+T(51)+T(10)+T(38)+T(10)+T(22)+T(12)+T(7)+T(11)+T(10)+T(46)+T(11)+T(12)+T(15)+T(16)+T(38)+T(45)+T(49)+NT(204)));
//G316: wff_remove_existential_cb(211) => __E_wff_cb_91(212).
	p(NT(211), (NT(212)));
//G317: wff_cb(43)           => wff_remove_existential_cb(211).
	p(NT(43), (NT(211)));
//G318: __E_bf_cb_92(214)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(200).
	p(NT(214), (T(49)+T(43)+T(38)+T(44)+T(15)+T(7)+T(38)+T(7)+T(48)+T(49)+T(43)+T(8)+T(50)+T(9)+T(48)+T(16)+T(15)+T(38)+T(45)+T(49)+NT(200)));
//G319: bf_has_subformula_cb(213) => __E_bf_cb_92(214).
	p(NT(213), (NT(214)));
//G320: bf_cb(47)            => bf_has_subformula_cb(213).
	p(NT(47), (NT(213)));
//G321: __E_bf_cb_93(216)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(201).
	p(NT(216), (T(49)+T(43)+T(38)+T(50)+T(10)+T(9)+T(8)+T(51)+T(10)+T(38)+T(43)+T(48)+T(46)+T(12)+T(51)+T(10)+T(50)+T(7)+T(15)+T(16)+T(38)+T(45)+T(49)+NT(201)));
//G322: bf_remove_funiversal_cb(215) => __E_bf_cb_93(216).
	p(NT(215), (NT(216)));
//G323: bf_cb(47)            => bf_remove_funiversal_cb(215).
	p(NT(47), (NT(215)));
//G324: __E_bf_cb_94(218)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(201).
	p(NT(218), (T(49)+T(43)+T(38)+T(50)+T(10)+T(9)+T(8)+T(51)+T(10)+T(38)+T(43)+T(10)+T(22)+T(12)+T(7)+T(11)+T(10)+T(46)+T(11)+T(12)+T(15)+T(16)+T(38)+T(45)+T(49)+NT(201)));
//G325: bf_remove_fexistential_cb(217) => __E_bf_cb_94(218).
	p(NT(217), (NT(218)));
//G326: bf_cb(47)            => bf_remove_fexistential_cb(217).
	p(NT(47), (NT(217)));
//G327: __E_bf_cb_95(220)    => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(197).
	p(NT(220), (T(49)+T(43)+T(38)+T(46)+T(8)+T(50)+T(9)+T(15)+T(16)+T(12)+T(52)+T(10)+T(38)+T(45)+T(49)+NT(197)));
//G328: bf_normalize_cb(219) => __E_bf_cb_95(220).
	p(NT(219), (NT(220)));
//G329: bf_cb(47)            => bf_normalize_cb(219).
	p(NT(47), (NT(219)));
//G330: cli(221)             => _(11).
	p(NT(221), (NT(11)));
//G331: __E_cli_96(223)      => '.' _(11) cli_command(222) _(11).
	p(NT(223), (T(1)+NT(11)+NT(222)+NT(11)));
//G332: __E_cli_97(224)      => null.
	p(NT(224), (nul));
//G333: __E_cli_97(224)      => __E_cli_96(223) __E_cli_97(224).
	p(NT(224), (NT(223)+NT(224)));
//G334: cli(221)             => _(11) cli_command(222) _(11) __E_cli_97(224).
	p(NT(221), (NT(11)+NT(222)+NT(11)+NT(224)));
//G335: __E_cli_command_98(227) => 'q'.
	p(NT(227), (T(53)));
//G336: __E_cli_command_98(227) => 'q' 'u' 'i' 't'.
	p(NT(227), (T(53)+T(48)+T(12)+T(11)));
//G337: quit_sym(226)        => __E_cli_command_98(227).
	p(NT(226), (NT(227)));
//G338: quit_cmd(225)        => quit_sym(226).
	p(NT(225), (NT(226)));
//G339: cli_command(222)     => quit_cmd(225).
	p(NT(222), (NT(225)));
//G340: __E_cli_command_99(230) => 'v'.
	p(NT(230), (T(51)));
//G341: __E_cli_command_99(230) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(230), (T(51)+T(10)+T(50)+T(7)+T(12)+T(8)+T(46)));
//G342: version_sym(229)     => __E_cli_command_99(230).
	p(NT(229), (NT(230)));
//G343: version_cmd(228)     => version_sym(229).
	p(NT(228), (NT(229)));
//G344: cli_command(222)     => version_cmd(228).
	p(NT(222), (NT(228)));
//G345: __E_cli_command_100(233) => 'c'.
	p(NT(233), (T(45)));
//G346: __E_cli_command_100(233) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(233), (T(45)+T(16)+T(10)+T(15)+T(50)));
//G347: clear_sym(232)       => __E_cli_command_100(233).
	p(NT(232), (NT(233)));
//G348: clear_cmd(231)       => clear_sym(232).
	p(NT(231), (NT(232)));
//G349: cli_command(222)     => clear_cmd(231).
	p(NT(222), (NT(231)));
//G350: __E___E_cli_command_101_102(237) => 'h'.
	p(NT(237), (T(44)));
//G351: __E___E_cli_command_101_102(237) => 'h' 'e' 'l' 'p'.
	p(NT(237), (T(44)+T(10)+T(16)+T(54)));
//G352: help_sym(236)        => __E___E_cli_command_101_102(237).
	p(NT(236), (NT(237)));
//G353: __E___E_cli_command_101_103(238) => __(52) help_arg(239).
	p(NT(238), (NT(52)+NT(239)));
//G354: __E___E_cli_command_101_103(238) => null.
	p(NT(238), (nul));
//G355: __E_cli_command_101(235) => help_sym(236) __E___E_cli_command_101_103(238).
	p(NT(235), (NT(236)+NT(238)));
//G356: help_cmd(234)        => __E_cli_command_101(235).
	p(NT(234), (NT(235)));
//G357: cli_command(222)     => help_cmd(234).
	p(NT(222), (NT(234)));
//G358: file_sym(242)        => 'f' 'i' 'l' 'e'.
	p(NT(242), (T(43)+T(12)+T(16)+T(10)));
//G359: __E_cli_command_104(241) => file_sym(242) __(52) q_string(243).
	p(NT(241), (NT(242)+NT(52)+NT(243)));
//G360: file_cmd(240)        => __E_cli_command_104(241).
	p(NT(240), (NT(241)));
//G361: cli_command(222)     => file_cmd(240).
	p(NT(222), (NT(240)));
//G362: valid_sym(246)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(246), (T(51)+T(15)+T(16)+T(12)+T(55)));
//G363: __E_cli_command_105(245) => valid_sym(246) __(52) normalize_cmd_arg(247).
	p(NT(245), (NT(246)+NT(52)+NT(247)));
//G364: valid_cmd(244)       => __E_cli_command_105(245).
	p(NT(244), (NT(245)));
//G365: cli_command(222)     => valid_cmd(244).
	p(NT(222), (NT(244)));
//G366: sat_sym(250)         => 's' 'a' 't'.
	p(NT(250), (T(7)+T(15)+T(11)));
//G367: __E_cli_command_106(249) => sat_sym(250) __(52) normalize_cmd_arg(247).
	p(NT(249), (NT(250)+NT(52)+NT(247)));
//G368: sat_cmd(248)         => __E_cli_command_106(249).
	p(NT(248), (NT(249)));
//G369: cli_command(222)     => sat_cmd(248).
	p(NT(222), (NT(248)));
//G370: unsat_sym(253)       => 'u' 'n' 's' 'a' 't'.
	p(NT(253), (T(48)+T(46)+T(7)+T(15)+T(11)));
//G371: __E_cli_command_107(252) => unsat_sym(253) __(52) normalize_cmd_arg(247).
	p(NT(252), (NT(253)+NT(52)+NT(247)));
//G372: unsat_cmd(251)       => __E_cli_command_107(252).
	p(NT(251), (NT(252)));
//G373: cli_command(222)     => unsat_cmd(251).
	p(NT(222), (NT(251)));
//G374: solve_sym(256)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(256), (T(7)+T(8)+T(16)+T(51)+T(10)));
//G375: __E___E_cli_command_108_109(257) => __(52) type(178).
	p(NT(257), (NT(52)+NT(178)));
//G376: __E___E_cli_command_108_109(257) => null.
	p(NT(257), (nul));
//G377: __E_cli_command_108(255) => solve_sym(256) __E___E_cli_command_108_109(257) __(52) solve_cmd_arg(258).
	p(NT(255), (NT(256)+NT(257)+NT(52)+NT(258)));
//G378: solve_cmd(254)       => __E_cli_command_108(255).
	p(NT(254), (NT(255)));
//G379: cli_command(222)     => solve_cmd(254).
	p(NT(222), (NT(254)));
//G380: __E___E_cli_command_110_111(262) => 'r'.
	p(NT(262), (T(50)));
//G381: __E___E_cli_command_110_111(262) => 'r' 'u' 'n'.
	p(NT(262), (T(50)+T(48)+T(46)));
//G382: run_sym(261)         => __E___E_cli_command_110_111(262).
	p(NT(261), (NT(262)));
//G383: __E___E_cli_command_110_112(263) => memory(264).
	p(NT(263), (NT(264)));
//G384: __E___E_cli_command_110_112(263) => wff(14).
	p(NT(263), (NT(14)));
//G385: __E_cli_command_110(260) => run_sym(261) __(52) __E___E_cli_command_110_112(263).
	p(NT(260), (NT(261)+NT(52)+NT(263)));
//G386: run_cmd(259)         => __E_cli_command_110(260).
	p(NT(259), (NT(260)));
//G387: cli_command(222)     => run_cmd(259).
	p(NT(222), (NT(259)));
//G388: __E___E_cli_command_113_114(268) => 'n'.
	p(NT(268), (T(46)));
//G389: __E___E_cli_command_113_114(268) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(268), (T(46)+T(8)+T(50)+T(9)+T(15)+T(16)+T(12)+T(52)+T(10)));
//G390: normalize_sym(267)   => __E___E_cli_command_113_114(268).
	p(NT(267), (NT(268)));
//G391: __E_cli_command_113(266) => normalize_sym(267) __(52) normalize_cmd_arg(247).
	p(NT(266), (NT(267)+NT(52)+NT(247)));
//G392: normalize_cmd(265)   => __E_cli_command_113(266).
	p(NT(265), (NT(266)));
//G393: cli_command(222)     => normalize_cmd(265).
	p(NT(222), (NT(265)));
//G394: __E___E_cli_command_115_116(272) => 's'.
	p(NT(272), (T(7)));
//G395: __E___E_cli_command_115_116(272) => 's' 'u' 'b' 's' 't'.
	p(NT(272), (T(7)+T(48)+T(49)+T(7)+T(11)));
//G396: __E___E_cli_command_115_116(272) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(272), (T(7)+T(48)+T(49)+T(7)+T(11)+T(12)+T(11)+T(48)+T(11)+T(10)));
//G397: subst_sym(271)       => __E___E_cli_command_115_116(272).
	p(NT(271), (NT(272)));
//G398: __E_cli_command_115(270) => subst_sym(271) __(52) nf_cmd_arg(273) _(11) '[' _(11) nf_cmd_arg(273) _(11) '/' _(11) nf_cmd_arg(273) _(11) ']'.
	p(NT(270), (NT(271)+NT(52)+NT(273)+NT(11)+T(19)+NT(11)+NT(273)+NT(11)+T(56)+NT(11)+NT(273)+NT(11)+T(20)));
//G399: subst_cmd(269)       => __E_cli_command_115(270).
	p(NT(269), (NT(270)));
//G400: cli_command(222)     => subst_cmd(269).
	p(NT(222), (NT(269)));
//G401: __E___E_cli_command_117_118(277) => 'i'.
	p(NT(277), (T(12)));
//G402: __E___E_cli_command_117_118(277) => 'i' 'n' 's' 't'.
	p(NT(277), (T(12)+T(46)+T(7)+T(11)));
//G403: __E___E_cli_command_117_118(277) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(277), (T(12)+T(46)+T(7)+T(11)+T(15)+T(46)+T(11)+T(12)+T(15)+T(11)+T(10)));
//G404: inst_sym(276)        => __E___E_cli_command_117_118(277).
	p(NT(276), (NT(277)));
//G405: __E_cli_command_117(275) => inst_sym(276) __(52) inst_args(278).
	p(NT(275), (NT(276)+NT(52)+NT(278)));
//G406: inst_cmd(274)        => __E_cli_command_117(275).
	p(NT(274), (NT(275)));
//G407: cli_command(222)     => inst_cmd(274).
	p(NT(222), (NT(274)));
//G408: dnf_sym(281)         => 'd' 'n' 'f'.
	p(NT(281), (T(55)+T(46)+T(43)));
//G409: __E_cli_command_119(280) => dnf_sym(281) __(52) nf_cmd_arg(273).
	p(NT(280), (NT(281)+NT(52)+NT(273)));
//G410: dnf_cmd(279)         => __E_cli_command_119(280).
	p(NT(279), (NT(280)));
//G411: cli_command(222)     => dnf_cmd(279).
	p(NT(222), (NT(279)));
//G412: cnf_sym(284)         => 'c' 'n' 'f'.
	p(NT(284), (T(45)+T(46)+T(43)));
//G413: __E_cli_command_120(283) => cnf_sym(284) __(52) nf_cmd_arg(273).
	p(NT(283), (NT(284)+NT(52)+NT(273)));
//G414: cnf_cmd(282)         => __E_cli_command_120(283).
	p(NT(282), (NT(283)));
//G415: cli_command(222)     => cnf_cmd(282).
	p(NT(222), (NT(282)));
//G416: anf_sym(287)         => 'a' 'n' 'f'.
	p(NT(287), (T(15)+T(46)+T(43)));
//G417: __E_cli_command_121(286) => anf_sym(287) __(52) nf_cmd_arg(273).
	p(NT(286), (NT(287)+NT(52)+NT(273)));
//G418: anf_cmd(285)         => __E_cli_command_121(286).
	p(NT(285), (NT(286)));
//G419: cli_command(222)     => anf_cmd(285).
	p(NT(222), (NT(285)));
//G420: nnf_sym(290)         => 'n' 'n' 'f'.
	p(NT(290), (T(46)+T(46)+T(43)));
//G421: __E_cli_command_122(289) => nnf_sym(290) __(52) nf_cmd_arg(273).
	p(NT(289), (NT(290)+NT(52)+NT(273)));
//G422: nnf_cmd(288)         => __E_cli_command_122(289).
	p(NT(288), (NT(289)));
//G423: cli_command(222)     => nnf_cmd(288).
	p(NT(222), (NT(288)));
//G424: pnf_sym(293)         => 'p' 'n' 'f'.
	p(NT(293), (T(54)+T(46)+T(43)));
//G425: __E_cli_command_123(292) => pnf_sym(293) __(52) nf_cmd_arg(273).
	p(NT(292), (NT(293)+NT(52)+NT(273)));
//G426: pnf_cmd(291)         => __E_cli_command_123(292).
	p(NT(291), (NT(292)));
//G427: cli_command(222)     => pnf_cmd(291).
	p(NT(222), (NT(291)));
//G428: mnf_sym(296)         => 'm' 'n' 'f'.
	p(NT(296), (T(9)+T(46)+T(43)));
//G429: __E_cli_command_124(295) => mnf_sym(296) __(52) nf_cmd_arg(273).
	p(NT(295), (NT(296)+NT(52)+NT(273)));
//G430: mnf_cmd(294)         => __E_cli_command_124(295).
	p(NT(294), (NT(295)));
//G431: cli_command(222)     => mnf_cmd(294).
	p(NT(222), (NT(294)));
//G432: snf_sym(299)         => 's' 'n' 'f'.
	p(NT(299), (T(7)+T(46)+T(43)));
//G433: __E_cli_command_125(298) => snf_sym(299) __(52) nf_cmd_arg(273).
	p(NT(298), (NT(299)+NT(52)+NT(273)));
//G434: snf_cmd(297)         => __E_cli_command_125(298).
	p(NT(297), (NT(298)));
//G435: cli_command(222)     => snf_cmd(297).
	p(NT(222), (NT(297)));
//G436: onf_sym(302)         => 'o' 'n' 'f'.
	p(NT(302), (T(8)+T(46)+T(43)));
//G437: __E_cli_command_126(301) => onf_sym(302) __(52) variable(116) __(52) onf_cmd_arg(303).
	p(NT(301), (NT(302)+NT(52)+NT(116)+NT(52)+NT(303)));
//G438: onf_cmd(300)         => __E_cli_command_126(301).
	p(NT(300), (NT(301)));
//G439: cli_command(222)     => onf_cmd(300).
	p(NT(222), (NT(300)));
//G440: __E___E_cli_command_127_128(307) => 'd' 'e' 'f' 's'.
	p(NT(307), (T(55)+T(10)+T(43)+T(7)));
//G441: __E___E_cli_command_127_128(307) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(307), (T(55)+T(10)+T(43)+T(12)+T(46)+T(12)+T(11)+T(12)+T(8)+T(46)+T(7)));
//G442: def_sym(306)         => __E___E_cli_command_127_128(307).
	p(NT(306), (NT(307)));
//G443: __E_cli_command_127(305) => def_sym(306).
	p(NT(305), (NT(306)));
//G444: def_list_cmd(304)    => __E_cli_command_127(305).
	p(NT(304), (NT(305)));
//G445: cli_command(222)     => def_list_cmd(304).
	p(NT(222), (NT(304)));
//G446: __E_cli_command_129(309) => def_sym(306) __(52) number(310).
	p(NT(309), (NT(306)+NT(52)+NT(310)));
//G447: def_print_cmd(308)   => __E_cli_command_129(309).
	p(NT(308), (NT(309)));
//G448: cli_command(222)     => def_print_cmd(308).
	p(NT(222), (NT(308)));
//G449: def_rr_cmd(311)      => rec_relation(16).
	p(NT(311), (NT(16)));
//G450: cli_command(222)     => def_rr_cmd(311).
	p(NT(222), (NT(311)));
//G451: qelim_sym(314)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(314), (T(53)+T(10)+T(16)+T(12)+T(9)));
//G452: __E_cli_command_130(313) => qelim_sym(314) __(52) wff_cmd_arg(315).
	p(NT(313), (NT(314)+NT(52)+NT(315)));
//G453: qelim_cmd(312)       => __E_cli_command_130(313).
	p(NT(312), (NT(313)));
//G454: cli_command(222)     => qelim_cmd(312).
	p(NT(222), (NT(312)));
//G455: get_sym(318)         => 'g' 'e' 't'.
	p(NT(318), (T(47)+T(10)+T(11)));
//G456: __E___E_cli_command_131_132(319) => __(52) option(320).
	p(NT(319), (NT(52)+NT(320)));
//G457: __E___E_cli_command_131_132(319) => null.
	p(NT(319), (nul));
//G458: __E_cli_command_131(317) => get_sym(318) __E___E_cli_command_131_132(319).
	p(NT(317), (NT(318)+NT(319)));
//G459: get_cmd(316)         => __E_cli_command_131(317).
	p(NT(316), (NT(317)));
//G460: cli_command(222)     => get_cmd(316).
	p(NT(222), (NT(316)));
//G461: set_sym(323)         => 's' 'e' 't'.
	p(NT(323), (T(7)+T(10)+T(11)));
//G462: __E___E_cli_command_133_134(324) => __(52).
	p(NT(324), (NT(52)));
//G463: __E___E_cli_command_133_134(324) => _(11) '=' _(11).
	p(NT(324), (NT(11)+T(3)+NT(11)));
//G464: __E_cli_command_133(322) => set_sym(323) __(52) option(320) __E___E_cli_command_133_134(324) option_value(325).
	p(NT(322), (NT(323)+NT(52)+NT(320)+NT(324)+NT(325)));
//G465: set_cmd(321)         => __E_cli_command_133(322).
	p(NT(321), (NT(322)));
//G466: cli_command(222)     => set_cmd(321).
	p(NT(222), (NT(321)));
//G467: enable_sym(328)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(328), (T(10)+T(46)+T(15)+T(49)+T(16)+T(10)));
//G468: __E_cli_command_135(327) => enable_sym(328) __(52) bool_option(329).
	p(NT(327), (NT(328)+NT(52)+NT(329)));
//G469: enable_cmd(326)      => __E_cli_command_135(327).
	p(NT(326), (NT(327)));
//G470: cli_command(222)     => enable_cmd(326).
	p(NT(222), (NT(326)));
//G471: disable_sym(332)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(332), (T(55)+T(12)+T(7)+T(15)+T(49)+T(16)+T(10)));
//G472: __E_cli_command_136(331) => disable_sym(332) __(52) bool_option(329).
	p(NT(331), (NT(332)+NT(52)+NT(329)));
//G473: disable_cmd(330)     => __E_cli_command_136(331).
	p(NT(330), (NT(331)));
//G474: cli_command(222)     => disable_cmd(330).
	p(NT(222), (NT(330)));
//G475: toggle_sym(335)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(335), (T(11)+T(8)+T(47)+T(47)+T(16)+T(10)));
//G476: __E_cli_command_137(334) => toggle_sym(335) __(52) bool_option(329).
	p(NT(334), (NT(335)+NT(52)+NT(329)));
//G477: toggle_cmd(333)      => __E_cli_command_137(334).
	p(NT(333), (NT(334)));
//G478: cli_command(222)     => toggle_cmd(333).
	p(NT(222), (NT(333)));
//G479: __E_cli_command_138(337) => type(178) __(52) in_var_name(154) _(11) '=' _(11) input_stream(338).
	p(NT(337), (NT(178)+NT(52)+NT(154)+NT(11)+T(3)+NT(11)+NT(338)));
//G480: def_input_cmd(336)   => __E_cli_command_138(337).
	p(NT(336), (NT(337)));
//G481: cli_command(222)     => def_input_cmd(336).
	p(NT(222), (NT(336)));
//G482: __E_cli_command_139(340) => type(178) __(52) out_var_name(158) _(11) '=' _(11) output_stream(341).
	p(NT(340), (NT(178)+NT(52)+NT(158)+NT(11)+T(3)+NT(11)+NT(341)));
//G483: def_output_cmd(339)  => __E_cli_command_139(340).
	p(NT(339), (NT(340)));
//G484: cli_command(222)     => def_output_cmd(339).
	p(NT(222), (NT(339)));
//G485: __E___E_cli_command_140_141(345) => 'h' 'i' 's' 't'.
	p(NT(345), (T(44)+T(12)+T(7)+T(11)));
//G486: __E___E_cli_command_140_141(345) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(345), (T(44)+T(12)+T(7)+T(11)+T(8)+T(50)+T(18)));
//G487: history_sym(344)     => __E___E_cli_command_140_141(345).
	p(NT(344), (NT(345)));
//G488: __E_cli_command_140(343) => history_sym(344).
	p(NT(343), (NT(344)));
//G489: history_list_cmd(342) => __E_cli_command_140(343).
	p(NT(342), (NT(343)));
//G490: cli_command(222)     => history_list_cmd(342).
	p(NT(222), (NT(342)));
//G491: __E_cli_command_142(347) => history_sym(344) __(52) memory(264).
	p(NT(347), (NT(344)+NT(52)+NT(264)));
//G492: history_print_cmd(346) => __E_cli_command_142(347).
	p(NT(346), (NT(347)));
//G493: cli_command(222)     => history_print_cmd(346).
	p(NT(222), (NT(346)));
//G494: __E_cli_command_143(349) => wff(14).
	p(NT(349), (NT(14)));
//G495: __E_cli_command_143(349) => bf(21).
	p(NT(349), (NT(21)));
//G496: history_store_cmd(348) => __E_cli_command_143(349).
	p(NT(348), (NT(349)));
//G497: cli_command(222)     => history_store_cmd(348).
	p(NT(222), (NT(348)));
//G498: __E_input_stream_144(350) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(351) _(11) ')'.
	p(NT(350), (T(12)+T(43)+T(12)+T(16)+T(10)+NT(11)+T(5)+NT(11)+NT(351)+NT(11)+T(6)));
//G499: input_stream(338)    => __E_input_stream_144(350).
	p(NT(338), (NT(350)));
//G500: console_sym(352)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(352), (T(45)+T(8)+T(46)+T(7)+T(8)+T(16)+T(10)));
//G501: input_stream(338)    => console_sym(352).
	p(NT(338), (NT(352)));
//G502: __E_output_stream_145(353) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(351) _(11) ')'.
	p(NT(353), (T(8)+T(43)+T(12)+T(16)+T(10)+NT(11)+T(5)+NT(11)+NT(351)+NT(11)+T(6)));
//G503: output_stream(341)   => __E_output_stream_145(353).
	p(NT(341), (NT(353)));
//G504: output_stream(341)   => console_sym(352).
	p(NT(341), (NT(352)));
//G505: q_file_name(351)     => '"' file_name(354) '"'.
	p(NT(351), (T(57)+NT(354)+T(57)));
//G506: __E_file_name_146(355) => printable(8).
	p(NT(355), (NT(8)));
//G507: __E_file_name_146(355) => printable(8) __E_file_name_146(355).
	p(NT(355), (NT(8)+NT(355)));
//G508: file_name(354)       => __E_file_name_146(355).
	p(NT(354), (NT(355)));
//G509: number(310)          => digits(170).
	p(NT(310), (NT(170)));
//G510: bf_cmd_arg(356)      => memory(264).
	p(NT(356), (NT(264)));
//G511: bf_cmd_arg(356)      => bf(21).
	p(NT(356), (NT(21)));
//G512: wff_cmd_arg(315)     => memory(264).
	p(NT(315), (NT(264)));
//G513: wff_cmd_arg(315)     => wff(14).
	p(NT(315), (NT(14)));
//G514: solve_cmd_arg(258)   => memory(264).
	p(NT(258), (NT(264)));
//G515: solve_cmd_arg(258)   => wff(14).
	p(NT(258), (NT(14)));
//G516: nf_cmd_arg(273)      => memory(264).
	p(NT(273), (NT(264)));
//G517: nf_cmd_arg(273)      => ref(18).
	p(NT(273), (NT(18)));
//G518: nf_cmd_arg(273)      => wff(14).
	p(NT(273), (NT(14)));
//G519: nf_cmd_arg(273)      => bf(21).
	p(NT(273), (NT(21)));
//G520: onf_cmd_arg(303)     => memory(264).
	p(NT(303), (NT(264)));
//G521: onf_cmd_arg(303)     => wff(14).
	p(NT(303), (NT(14)));
//G522: normalize_cmd_arg(247) => memory(264).
	p(NT(247), (NT(264)));
//G523: normalize_cmd_arg(247) => rr(10).
	p(NT(247), (NT(10)));
//G524: normalize_cmd_arg(247) => ref(18).
	p(NT(247), (NT(18)));
//G525: normalize_cmd_arg(247) => wff(14).
	p(NT(247), (NT(14)));
//G526: normalize_cmd_arg(247) => bf(21).
	p(NT(247), (NT(21)));
//G527: inst_args(278)       => wff_cmd_arg(315) _(11) '[' _(11) variable(116) _(11) '/' _(11) bf_cmd_arg(356) _(11) ']'.
	p(NT(278), (NT(315)+NT(11)+T(19)+NT(11)+NT(116)+NT(11)+T(56)+NT(11)+NT(356)+NT(11)+T(20)));
//G528: inst_args(278)       => bf_cmd_arg(356) _(11) '[' _(11) variable(116) _(11) '/' _(11) bf_cmd_arg(356) _(11) ']'.
	p(NT(278), (NT(356)+NT(11)+T(19)+NT(11)+NT(116)+NT(11)+T(56)+NT(11)+NT(356)+NT(11)+T(20)));
//G529: help_arg(239)        => help_sym(236).
	p(NT(239), (NT(236)));
//G530: help_arg(239)        => version_sym(229).
	p(NT(239), (NT(229)));
//G531: help_arg(239)        => quit_sym(226).
	p(NT(239), (NT(226)));
//G532: help_arg(239)        => clear_sym(232).
	p(NT(239), (NT(232)));
//G533: help_arg(239)        => get_sym(318).
	p(NT(239), (NT(318)));
//G534: help_arg(239)        => set_sym(323).
	p(NT(239), (NT(323)));
//G535: help_arg(239)        => enable_sym(328).
	p(NT(239), (NT(328)));
//G536: help_arg(239)        => disable_sym(332).
	p(NT(239), (NT(332)));
//G537: help_arg(239)        => toggle_sym(335).
	p(NT(239), (NT(335)));
//G538: help_arg(239)        => file_sym(242).
	p(NT(239), (NT(242)));
//G539: help_arg(239)        => history_sym(344).
	p(NT(239), (NT(344)));
//G540: help_arg(239)        => abs_memory_sym(357).
	p(NT(239), (NT(357)));
//G541: help_arg(239)        => rel_memory_sym(358).
	p(NT(239), (NT(358)));
//G542: help_arg(239)        => selection_sym(359).
	p(NT(239), (NT(359)));
//G543: help_arg(239)        => def_sym(306).
	p(NT(239), (NT(306)));
//G544: help_arg(239)        => inst_sym(276).
	p(NT(239), (NT(276)));
//G545: help_arg(239)        => subst_sym(271).
	p(NT(239), (NT(271)));
//G546: help_arg(239)        => normalize_sym(267).
	p(NT(239), (NT(267)));
//G547: help_arg(239)        => execute_sym(360).
	p(NT(239), (NT(360)));
//G548: help_arg(239)        => solve_sym(256).
	p(NT(239), (NT(256)));
//G549: help_arg(239)        => valid_sym(246).
	p(NT(239), (NT(246)));
//G550: help_arg(239)        => sat_sym(250).
	p(NT(239), (NT(250)));
//G551: help_arg(239)        => unsat_sym(253).
	p(NT(239), (NT(253)));
//G552: help_arg(239)        => run_sym(261).
	p(NT(239), (NT(261)));
//G553: help_arg(239)        => dnf_sym(281).
	p(NT(239), (NT(281)));
//G554: help_arg(239)        => cnf_sym(284).
	p(NT(239), (NT(284)));
//G555: help_arg(239)        => anf_sym(287).
	p(NT(239), (NT(287)));
//G556: help_arg(239)        => snf_sym(299).
	p(NT(239), (NT(299)));
//G557: help_arg(239)        => nnf_sym(290).
	p(NT(239), (NT(290)));
//G558: help_arg(239)        => pnf_sym(293).
	p(NT(239), (NT(293)));
//G559: help_arg(239)        => mnf_sym(296).
	p(NT(239), (NT(296)));
//G560: help_arg(239)        => onf_sym(302).
	p(NT(239), (NT(302)));
//G561: help_arg(239)        => qelim_sym(314).
	p(NT(239), (NT(314)));
//G562: __E___E_help_arg_147_148(363) => 's'.
	p(NT(363), (T(7)));
//G563: __E___E_help_arg_147_148(363) => null.
	p(NT(363), (nul));
//G564: __E_help_arg_147(362) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_147_148(363).
	p(NT(362), (T(10)+T(22)+T(15)+T(9)+T(54)+T(16)+T(10)+NT(363)));
//G565: examples_sym(361)    => __E_help_arg_147(362).
	p(NT(361), (NT(362)));
//G566: help_arg(239)        => examples_sym(361).
	p(NT(239), (NT(361)));
//G567: __E_memory_149(365)  => '%'.
	p(NT(365), (T(58)));
//G568: rel_memory_sym(358)  => '%' '-'.
	p(NT(358), (T(58)+T(23)));
//G569: memory_id(367)       => digits(170).
	p(NT(367), (NT(170)));
//G570: __E___E_memory_149_150(366) => memory_id(367).
	p(NT(366), (NT(367)));
//G571: __E___E_memory_149_150(366) => null.
	p(NT(366), (nul));
//G572: __E_memory_149(365)  => rel_memory_sym(358) __E___E_memory_149_150(366).
	p(NT(365), (NT(358)+NT(366)));
//G573: rel_memory(364)      => __E_memory_149(365).
	p(NT(364), (NT(365)));
//G574: memory(264)          => rel_memory(364).
	p(NT(264), (NT(364)));
//G575: abs_memory_sym(357)  => '%'.
	p(NT(357), (T(58)));
//G576: __E_memory_151(369)  => abs_memory_sym(357) memory_id(367).
	p(NT(369), (NT(357)+NT(367)));
//G577: abs_memory(368)      => __E_memory_151(369).
	p(NT(368), (NT(369)));
//G578: memory(264)          => abs_memory(368).
	p(NT(264), (NT(368)));
//G579: option(320)          => bool_option(329).
	p(NT(320), (NT(329)));
//G580: __E_option_152(372)  => 'S'.
	p(NT(372), (T(30)));
//G581: __E_option_152(372)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(372), (T(7)+T(10)+T(51)+T(10)+T(50)+T(12)+T(11)+T(18)));
//G582: severity_opt(371)    => __E_option_152(372).
	p(NT(371), (NT(372)));
//G583: enum_option(370)     => severity_opt(371).
	p(NT(370), (NT(371)));
//G584: option(320)          => enum_option(370).
	p(NT(320), (NT(370)));
//G585: __E_bool_option_153(374) => 's'.
	p(NT(374), (T(7)));
//G586: __E_bool_option_153(374) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(374), (T(7)+T(11)+T(15)+T(11)+T(48)+T(7)));
//G587: status_opt(373)      => __E_bool_option_153(374).
	p(NT(373), (NT(374)));
//G588: bool_option(329)     => status_opt(373).
	p(NT(329), (NT(373)));
//G589: __E_bool_option_154(376) => 'c'.
	p(NT(376), (T(45)));
//G590: __E_bool_option_154(376) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(376), (T(45)+T(8)+T(16)+T(8)+T(50)));
//G591: __E_bool_option_154(376) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(376), (T(45)+T(8)+T(16)+T(8)+T(50)+T(7)));
//G592: colors_opt(375)      => __E_bool_option_154(376).
	p(NT(375), (NT(376)));
//G593: bool_option(329)     => colors_opt(375).
	p(NT(329), (NT(375)));
//G594: __E_bool_option_155(378) => 'V'.
	p(NT(378), (T(59)));
//G595: __E_bool_option_155(378) => 'c' 'h' 'a' 'r' 'v' 'a' 'r'.
	p(NT(378), (T(45)+T(44)+T(15)+T(50)+T(51)+T(15)+T(50)));
//G596: charvar_opt(377)     => __E_bool_option_155(378).
	p(NT(377), (NT(378)));
//G597: bool_option(329)     => charvar_opt(377).
	p(NT(329), (NT(377)));
//G598: __E_bool_option_156(380) => 'H'.
	p(NT(380), (T(60)));
//G599: __E_bool_option_156(380) => 'h' 'i' 'g' 'h' 'l' 'i' 'g' 'h' 't'.
	p(NT(380), (T(44)+T(12)+T(47)+T(44)+T(16)+T(12)+T(47)+T(44)+T(11)));
//G600: __E_bool_option_156(380) => 'h' 'i' 'g' 'h' 'l' 'i' 'g' 'h' 't' 'i' 'n' 'g'.
	p(NT(380), (T(44)+T(12)+T(47)+T(44)+T(16)+T(12)+T(47)+T(44)+T(11)+T(12)+T(46)+T(47)));
//G601: highlighting_opt(379) => __E_bool_option_156(380).
	p(NT(379), (NT(380)));
//G602: bool_option(329)     => highlighting_opt(379).
	p(NT(329), (NT(379)));
//G603: __E_bool_option_157(382) => 'I'.
	p(NT(382), (T(61)));
//G604: __E_bool_option_157(382) => 'i' 'n' 'd' 'e' 'n' 't'.
	p(NT(382), (T(12)+T(46)+T(55)+T(10)+T(46)+T(11)));
//G605: __E_bool_option_157(382) => 'i' 'n' 'd' 'e' 'n' 't' 'i' 'n' 'g'.
	p(NT(382), (T(12)+T(46)+T(55)+T(10)+T(46)+T(11)+T(12)+T(46)+T(47)));
//G606: indenting_opt(381)   => __E_bool_option_157(382).
	p(NT(381), (NT(382)));
//G607: bool_option(329)     => indenting_opt(381).
	p(NT(329), (NT(381)));
//G608: __E_bool_option_158(384) => 'd'.
	p(NT(384), (T(55)));
//G609: __E_bool_option_158(384) => 'd' 'b' 'g'.
	p(NT(384), (T(55)+T(49)+T(47)));
//G610: __E_bool_option_158(384) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(384), (T(55)+T(10)+T(49)+T(48)+T(47)));
//G611: debug_repl_opt(383)  => __E_bool_option_158(384).
	p(NT(383), (NT(384)));
//G612: bool_option(329)     => debug_repl_opt(383).
	p(NT(329), (NT(383)));
//G613: option_value(325)    => option_value_true(385).
	p(NT(325), (NT(385)));
//G614: option_value(325)    => option_value_false(386).
	p(NT(325), (NT(386)));
//G615: option_value(325)    => severity(387).
	p(NT(325), (NT(387)));
//G616: option_value_true(385) => 't'.
	p(NT(385), (T(11)));
//G617: option_value_true(385) => 't' 'r' 'u' 'e'.
	p(NT(385), (T(11)+T(50)+T(48)+T(10)));
//G618: option_value_true(385) => 'o' 'n'.
	p(NT(385), (T(8)+T(46)));
//G619: option_value_true(385) => '1'.
	p(NT(385), (T(33)));
//G620: option_value_true(385) => 'y'.
	p(NT(385), (T(18)));
//G621: option_value_true(385) => 'y' 'e' 's'.
	p(NT(385), (T(18)+T(10)+T(7)));
//G622: option_value_false(386) => 'f'.
	p(NT(386), (T(43)));
//G623: option_value_false(386) => 'f' 'a' 'l' 's' 'e'.
	p(NT(386), (T(43)+T(15)+T(16)+T(7)+T(10)));
//G624: option_value_false(386) => 'o' 'f' 'f'.
	p(NT(386), (T(8)+T(43)+T(43)));
//G625: option_value_false(386) => '0'.
	p(NT(386), (T(34)));
//G626: option_value_false(386) => 'n'.
	p(NT(386), (T(46)));
//G627: option_value_false(386) => 'n' 'o'.
	p(NT(386), (T(46)+T(8)));
//G628: __E_severity_159(389) => 'e'.
	p(NT(389), (T(10)));
//G629: __E_severity_159(389) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(389), (T(10)+T(50)+T(50)+T(8)+T(50)));
//G630: error_sym(388)       => __E_severity_159(389).
	p(NT(388), (NT(389)));
//G631: severity(387)        => error_sym(388).
	p(NT(387), (NT(388)));
//G632: __E_severity_160(391) => 'i'.
	p(NT(391), (T(12)));
//G633: __E_severity_160(391) => 'i' 'n' 'f' 'o'.
	p(NT(391), (T(12)+T(46)+T(43)+T(8)));
//G634: info_sym(390)        => __E_severity_160(391).
	p(NT(390), (NT(391)));
//G635: severity(387)        => info_sym(390).
	p(NT(387), (NT(390)));
//G636: __E_severity_161(393) => 'd'.
	p(NT(393), (T(55)));
//G637: __E_severity_161(393) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(393), (T(55)+T(10)+T(49)+T(48)+T(47)));
//G638: debug_sym(392)       => __E_severity_161(393).
	p(NT(392), (NT(393)));
//G639: severity(387)        => debug_sym(392).
	p(NT(387), (NT(392)));
//G640: __E_severity_162(395) => 't'.
	p(NT(395), (T(11)));
//G641: __E_severity_162(395) => 't' 'r' 'a' 'c' 'e'.
	p(NT(395), (T(11)+T(50)+T(15)+T(45)+T(10)));
//G642: trace_sym(394)       => __E_severity_162(395).
	p(NT(394), (NT(395)));
//G643: severity(387)        => trace_sym(394).
	p(NT(387), (NT(394)));
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
		capture, bf, sym, __E_ref_3, offsets, ref_args, __E_ref_args_4, __E___E_ref_args_4_5, ref_arg, __E___E_ref_args_4_6, 
		__E___E_ref_args_4_7, library, rules, __E_library_8, __E___E_library_8_9, rule, __E___E___E_library_8_9_10, wff_rule, bf_rule, __E___E_library_8_11, 
		wff_matcher, wff_body, __E_wff_rule_12, wff_cb, bf_matcher, bf_body, __E_bf_rule_13, bf_cb, builder, builder_head, 
		builder_body, __E_builder_head_14, __, __E_builder_head_15, bf_builder_body, __E_builder_body_16, wff_builder_body, __E_builder_body_17, tau_constant_source, __E_tau_constant_source_18, 
		wff_parenthesis, __E_wff_19, wff_sometimes, __E_wff_20, __E___E_wff_20_21, wff_always, __E_wff_22, __E___E_wff_22_23, wff_conditional, __E_wff_24, 
		wff_all, __E_wff_25, q_vars, wff_ex, __E_wff_26, wff_ref, constraint, wff_imply, __E_wff_27, wff_equiv, 
		__E_wff_28, wff_or, __E_wff_29, wff_xor, __E_wff_30, wff_and, __E_wff_31, wff_neg, __E_wff_32, wff_t, 
		wff_f, bf_interval, __E_wff_33, bf_eq, __E_wff_34, bf_neq, __E_wff_35, bf_less_equal, __E_wff_36, bf_nleq, 
		__E_wff_37, bf_greater, __E_wff_38, bf_ngreater, __E_wff_39, bf_greater_equal, __E_wff_40, bf_ngeq, __E_wff_41, bf_less, 
		__E_wff_42, bf_nless, __E_wff_43, bf_parenthesis, __E_bf_44, bf_constant, variable, bf_splitter, __E_bf_45, bf_ref, 
		bf_or, __E_bf_46, bf_xor, __E_bf_47, bf_and, __E_bf_48, __E___E_bf_48_49, bf_neg, __E_bf_50, bf_t, 
		bf_f, bf_and_nosep_1st_op, __E_bf_and_51, uninterpreted_constant, ctn_neq, __E_constraint_52, ctnvar, num, ctn_eq, __E_constraint_53, 
		ctn_greater_equal, __E_constraint_54, ctn_greater, __E_constraint_55, ctn_less_equal, __E_constraint_56, ctn_less, __E_constraint_57, __E_variable_58, chars, 
		io_var, __E_variable_59, in, out, in_var_name, __E_in_60, __E___E_in_60_61, offset, out_var_name, __E_out_62, 
		__E___E_out_62_63, q_var, __E_q_vars_64, __E_q_vars_65, __E_offsets_66, __E_offsets_67, shift, __E_offset_68, __E_shift_69, __E___E_shift_69_70, 
		digits, uninter_const_name, __E_uninterpreted_constant_71, __E___E_uninterpreted_constant_71_72, constant, __E_bf_constant_73, binding, __E_bf_constant_74, type, source, 
		__E_binding_75, src_c, __E___E_binding_75_76, __E___E___E_binding_75_76_77, __E___E___E_binding_75_76_78, __E_src_c_79, __E_src_c_80, __E_chars_81, __E_chars_82, __E_digits_83, 
		extra, comment, __E_comment_84, __E_comment_85, __E_comment_86, __E____87, __E___88, bf_cb_args1, bf_cb_arg, bf_cb_args2, 
		bf_cb_args3, bf_cb_args4, wff_cb_args1, wff_cb_arg, wff_cb_args2, wff_cb_args3, wff_cb_args4, wff_has_clashing_subformulas_cb, __E_wff_cb_89, wff_has_subformula_cb, 
		__E_wff_cb_90, wff_remove_existential_cb, __E_wff_cb_91, bf_has_subformula_cb, __E_bf_cb_92, bf_remove_funiversal_cb, __E_bf_cb_93, bf_remove_fexistential_cb, __E_bf_cb_94, bf_normalize_cb, 
		__E_bf_cb_95, cli, cli_command, __E_cli_96, __E_cli_97, quit_cmd, quit_sym, __E_cli_command_98, version_cmd, version_sym, 
		__E_cli_command_99, clear_cmd, clear_sym, __E_cli_command_100, help_cmd, __E_cli_command_101, help_sym, __E___E_cli_command_101_102, __E___E_cli_command_101_103, help_arg, 
		file_cmd, __E_cli_command_104, file_sym, q_string, valid_cmd, __E_cli_command_105, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_106, 
		sat_sym, unsat_cmd, __E_cli_command_107, unsat_sym, solve_cmd, __E_cli_command_108, solve_sym, __E___E_cli_command_108_109, solve_cmd_arg, run_cmd, 
		__E_cli_command_110, run_sym, __E___E_cli_command_110_111, __E___E_cli_command_110_112, memory, normalize_cmd, __E_cli_command_113, normalize_sym, __E___E_cli_command_113_114, subst_cmd, 
		__E_cli_command_115, subst_sym, __E___E_cli_command_115_116, nf_cmd_arg, inst_cmd, __E_cli_command_117, inst_sym, __E___E_cli_command_117_118, inst_args, dnf_cmd, 
		__E_cli_command_119, dnf_sym, cnf_cmd, __E_cli_command_120, cnf_sym, anf_cmd, __E_cli_command_121, anf_sym, nnf_cmd, __E_cli_command_122, 
		nnf_sym, pnf_cmd, __E_cli_command_123, pnf_sym, mnf_cmd, __E_cli_command_124, mnf_sym, snf_cmd, __E_cli_command_125, snf_sym, 
		onf_cmd, __E_cli_command_126, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_127, def_sym, __E___E_cli_command_127_128, def_print_cmd, __E_cli_command_129, 
		number, def_rr_cmd, qelim_cmd, __E_cli_command_130, qelim_sym, wff_cmd_arg, get_cmd, __E_cli_command_131, get_sym, __E___E_cli_command_131_132, 
		option, set_cmd, __E_cli_command_133, set_sym, __E___E_cli_command_133_134, option_value, enable_cmd, __E_cli_command_135, enable_sym, bool_option, 
		disable_cmd, __E_cli_command_136, disable_sym, toggle_cmd, __E_cli_command_137, toggle_sym, def_input_cmd, __E_cli_command_138, input_stream, def_output_cmd, 
		__E_cli_command_139, output_stream, history_list_cmd, __E_cli_command_140, history_sym, __E___E_cli_command_140_141, history_print_cmd, __E_cli_command_142, history_store_cmd, __E_cli_command_143, 
		__E_input_stream_144, q_file_name, console_sym, __E_output_stream_145, file_name, __E_file_name_146, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, 
		execute_sym, examples_sym, __E_help_arg_147, __E___E_help_arg_147_148, rel_memory, __E_memory_149, __E___E_memory_149_150, memory_id, abs_memory, __E_memory_151, 
		enum_option, severity_opt, __E_option_152, status_opt, __E_bool_option_153, colors_opt, __E_bool_option_154, charvar_opt, __E_bool_option_155, highlighting_opt, 
		__E_bool_option_156, indenting_opt, __E_bool_option_157, debug_repl_opt, __E_bool_option_158, option_value_true, option_value_false, severity, error_sym, __E_severity_159, 
		info_sym, __E_severity_160, debug_sym, __E_severity_161, trace_sym, __E_severity_162, named_binding, __N_0, __N_1, __N_2, 
		__N_3, __N_4, 
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
