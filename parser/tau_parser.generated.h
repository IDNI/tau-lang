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
	"__E_bf_51", "__E___E_bf_51_52", "type", "bf_f", "__E_bf_53", "__E___E_bf_53_54", "bf_and_nosep_1st_op", "__E_bf_and_55", "uninterpreted_constant", "ctn_neq", 
	"__E_constraint_56", "ctnvar", "num", "ctn_eq", "__E_constraint_57", "ctn_greater_equal", "__E_constraint_58", "ctn_greater", "__E_constraint_59", "ctn_less_equal", 
	"__E_constraint_60", "ctn_less", "__E_constraint_61", "__E_variable_62", "chars", "io_var", "__E_variable_63", "in", "out", "in_var_name", 
	"__E_in_64", "__E___E_in_64_65", "offset", "out_var_name", "__E_out_66", "__E___E_out_66_67", "q_var", "__E_q_vars_68", "__E_q_vars_69", "__E_offsets_70", 
	"__E_offsets_71", "shift", "__E_offset_72", "__E_shift_73", "__E___E_shift_73_74", "digits", "uninter_const_name", "__E_uninterpreted_constant_75", "__E___E_uninterpreted_constant_75_76", "constant", 
	"__E_bf_constant_77", "binding", "__E_bf_constant_78", "source", "__E_binding_79", "src_c", "__E___E_binding_79_80", "__E___E___E_binding_79_80_81", "__E___E___E_binding_79_80_82", "__E_src_c_83", 
	"__E_src_c_84", "__E_chars_85", "__E_chars_86", "__E_digits_87", "extra", "comment", "__E_comment_88", "__E_comment_89", "__E_comment_90", "__E____91", 
	"__E___92", "bf_cb_args1", "bf_cb_arg", "bf_cb_args2", "bf_cb_args3", "bf_cb_args4", "wff_cb_args1", "wff_cb_arg", "wff_cb_args2", "wff_cb_args3", 
	"wff_cb_args4", "wff_has_clashing_subformulas_cb", "__E_wff_cb_93", "wff_has_subformula_cb", "__E_wff_cb_94", "wff_remove_existential_cb", "__E_wff_cb_95", "bf_has_subformula_cb", "__E_bf_cb_96", "bf_remove_funiversal_cb", 
	"__E_bf_cb_97", "bf_remove_fexistential_cb", "__E_bf_cb_98", "bf_normalize_cb", "__E_bf_cb_99", "cli", "cli_command", "__E_cli_100", "__E_cli_101", "quit_cmd", 
	"quit_sym", "__E_cli_command_102", "version_cmd", "version_sym", "__E_cli_command_103", "clear_cmd", "clear_sym", "__E_cli_command_104", "help_cmd", "__E_cli_command_105", 
	"help_sym", "__E___E_cli_command_105_106", "__E___E_cli_command_105_107", "help_arg", "file_cmd", "__E_cli_command_108", "file_sym", "q_string", "valid_cmd", "__E_cli_command_109", 
	"valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_110", "sat_sym", "unsat_cmd", "__E_cli_command_111", "unsat_sym", "solve_cmd", "__E_cli_command_112", 
	"solve_sym", "__E___E_cli_command_112_113", "solve_cmd_arg", "run_cmd", "__E_cli_command_114", "run_sym", "__E___E_cli_command_114_115", "__E___E_cli_command_114_116", "memory", "normalize_cmd", 
	"__E_cli_command_117", "normalize_sym", "__E___E_cli_command_117_118", "subst_cmd", "__E_cli_command_119", "subst_sym", "__E___E_cli_command_119_120", "nf_cmd_arg", "inst_cmd", "__E_cli_command_121", 
	"inst_sym", "__E___E_cli_command_121_122", "inst_args", "dnf_cmd", "__E_cli_command_123", "dnf_sym", "cnf_cmd", "__E_cli_command_124", "cnf_sym", "anf_cmd", 
	"__E_cli_command_125", "anf_sym", "nnf_cmd", "__E_cli_command_126", "nnf_sym", "pnf_cmd", "__E_cli_command_127", "pnf_sym", "mnf_cmd", "__E_cli_command_128", 
	"mnf_sym", "snf_cmd", "__E_cli_command_129", "snf_sym", "onf_cmd", "__E_cli_command_130", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_131", 
	"def_sym", "__E___E_cli_command_131_132", "def_print_cmd", "__E_cli_command_133", "number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_134", "qelim_sym", "wff_cmd_arg", 
	"get_cmd", "__E_cli_command_135", "get_sym", "__E___E_cli_command_135_136", "option", "set_cmd", "__E_cli_command_137", "set_sym", "__E___E_cli_command_137_138", "option_value", 
	"enable_cmd", "__E_cli_command_139", "enable_sym", "bool_option", "disable_cmd", "__E_cli_command_140", "disable_sym", "toggle_cmd", "__E_cli_command_141", "toggle_sym", 
	"def_input_cmd", "__E_cli_command_142", "input_stream", "def_output_cmd", "__E_cli_command_143", "output_stream", "history_list_cmd", "__E_cli_command_144", "history_sym", "__E___E_cli_command_144_145", 
	"history_print_cmd", "__E_cli_command_146", "history_store_cmd", "__E_cli_command_147", "__E_input_stream_148", "q_file_name", "console_sym", "__E_output_stream_149", "file_name", "__E_file_name_150", 
	"bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "execute_sym", "examples_sym", "__E_help_arg_151", "__E___E_help_arg_151_152", "rel_memory", "__E_memory_153", 
	"__E___E_memory_153_154", "memory_id", "abs_memory", "__E_memory_155", "enum_option", "severity_opt", "__E_option_156", "status_opt", "__E_bool_option_157", "colors_opt", 
	"__E_bool_option_158", "charvar_opt", "__E_bool_option_159", "highlighting_opt", "__E_bool_option_160", "indenting_opt", "__E_bool_option_161", "debug_repl_opt", "__E_bool_option_162", "option_value_true", 
	"option_value_false", "severity", "error_sym", "__E_severity_163", "info_sym", "__E_severity_164", "debug_sym", "__E_severity_165", "trace_sym", "__E_severity_166", 
	"named_binding", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", 
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
			20, 22, 116, 132, 141, 159, 163, 175, 176, 183,
			358, 400
		},
		.to_inline = {
			{ 14, 60, 14 },
			{ 21, 113, 21 },
			{ 43 },
			{ 47 },
			{ 154 },
			{ 166 },
			{ 185 },
			{ 201 },
			{ 203 },
			{ 204 },
			{ 205 },
			{ 206 },
			{ 208 },
			{ 209 },
			{ 210 },
			{ 251 },
			{ 262 },
			{ 277 },
			{ 282 },
			{ 307 },
			{ 319 },
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
//G161: __E___E_bf_51_52(131) => _(11) ':' _(11) type(132).
	p(NT(131), (NT(11)+T(2)+NT(11)+NT(132)));
//G162: __E___E_bf_51_52(131) => null.
	p(NT(131), (nul));
//G163: __E_bf_51(130)       => '1' __E___E_bf_51_52(131).
	p(NT(130), (T(33)+NT(131)));
//G164: bf_t(129)            => __E_bf_51(130).
	p(NT(129), (NT(130)));
//G165: bf(21)               => bf_t(129).
	p(NT(21), (NT(129)));
//G166: __E___E_bf_53_54(135) => _(11) ':' _(11) type(132).
	p(NT(135), (NT(11)+T(2)+NT(11)+NT(132)));
//G167: __E___E_bf_53_54(135) => null.
	p(NT(135), (nul));
//G168: __E_bf_53(134)       => '0' __E___E_bf_53_54(135).
	p(NT(134), (T(34)+NT(135)));
//G169: bf_f(133)            => __E_bf_53(134).
	p(NT(133), (NT(134)));
//G170: bf(21)               => bf_f(133).
	p(NT(21), (NT(133)));
//G171: bf(21)               => capture(20).
	p(NT(21), (NT(20)));
//G172: __E_bf_and_55(137)   => bf_parenthesis(113).
	p(NT(137), (NT(113)));
//G173: __E_bf_and_55(137)   => variable(116).
	p(NT(137), (NT(116)));
//G174: __E_bf_and_55(137)   => bf_splitter(117).
	p(NT(137), (NT(117)));
//G175: __E_bf_and_55(137)   => bf_ref(119).
	p(NT(137), (NT(119)));
//G176: __E_bf_and_55(137)   => bf_or(120).
	p(NT(137), (NT(120)));
//G177: __E_bf_and_55(137)   => bf_xor(122).
	p(NT(137), (NT(122)));
//G178: __E_bf_and_55(137)   => bf_neg(127).
	p(NT(137), (NT(127)));
//G179: __E_bf_and_55(137)   => capture(20).
	p(NT(137), (NT(20)));
//G180: __E_bf_and_55(137)   => uninterpreted_constant(138).
	p(NT(137), (NT(138)));
//G181: bf_and_nosep_1st_op(136) => __E_bf_and_55(137).
	p(NT(136), (NT(137)));
//G182: bf_and(124)          => bf_and_nosep_1st_op(136) bf(21).
	p(NT(124), (NT(136)+NT(21)));
//G183: __E_constraint_56(140) => '[' ctnvar(141) _(11) '!' '=' _(11) num(142) ']'.
	p(NT(140), (T(19)+NT(141)+NT(11)+T(27)+T(3)+NT(11)+NT(142)+T(20)));
//G184: __E_constraint_56(140) => '[' num(142) _(11) '!' '=' _(11) ctnvar(141) ']'.
	p(NT(140), (T(19)+NT(142)+NT(11)+T(27)+T(3)+NT(11)+NT(141)+T(20)));
//G185: ctn_neq(139)         => __E_constraint_56(140).
	p(NT(139), (NT(140)));
//G186: constraint(76)       => ctn_neq(139).
	p(NT(76), (NT(139)));
//G187: __E_constraint_57(144) => '[' ctnvar(141) _(11) '=' _(11) num(142) ']'.
	p(NT(144), (T(19)+NT(141)+NT(11)+T(3)+NT(11)+NT(142)+T(20)));
//G188: __E_constraint_57(144) => '[' num(142) _(11) '=' _(11) ctnvar(141) ']'.
	p(NT(144), (T(19)+NT(142)+NT(11)+T(3)+NT(11)+NT(141)+T(20)));
//G189: ctn_eq(143)          => __E_constraint_57(144).
	p(NT(143), (NT(144)));
//G190: constraint(76)       => ctn_eq(143).
	p(NT(76), (NT(143)));
//G191: __E_constraint_58(146) => '[' ctnvar(141) _(11) '>' '=' _(11) num(142) ']'.
	p(NT(146), (T(19)+NT(141)+NT(11)+T(14)+T(3)+NT(11)+NT(142)+T(20)));
//G192: __E_constraint_58(146) => '[' num(142) _(11) '>' '=' _(11) ctnvar(141) ']'.
	p(NT(146), (T(19)+NT(142)+NT(11)+T(14)+T(3)+NT(11)+NT(141)+T(20)));
//G193: ctn_greater_equal(145) => __E_constraint_58(146).
	p(NT(145), (NT(146)));
//G194: constraint(76)       => ctn_greater_equal(145).
	p(NT(76), (NT(145)));
//G195: __E_constraint_59(148) => '[' ctnvar(141) _(11) '>' _(11) num(142) ']'.
	p(NT(148), (T(19)+NT(141)+NT(11)+T(14)+NT(11)+NT(142)+T(20)));
//G196: __E_constraint_59(148) => '[' num(142) _(11) '>' _(11) ctnvar(141) ']'.
	p(NT(148), (T(19)+NT(142)+NT(11)+T(14)+NT(11)+NT(141)+T(20)));
//G197: ctn_greater(147)     => __E_constraint_59(148).
	p(NT(147), (NT(148)));
//G198: constraint(76)       => ctn_greater(147).
	p(NT(76), (NT(147)));
//G199: __E_constraint_60(150) => '[' ctnvar(141) _(11) '<' '=' _(11) num(142) ']'.
	p(NT(150), (T(19)+NT(141)+NT(11)+T(13)+T(3)+NT(11)+NT(142)+T(20)));
//G200: __E_constraint_60(150) => '[' num(142) _(11) '<' '=' _(11) ctnvar(141) ']'.
	p(NT(150), (T(19)+NT(142)+NT(11)+T(13)+T(3)+NT(11)+NT(141)+T(20)));
//G201: ctn_less_equal(149)  => __E_constraint_60(150).
	p(NT(149), (NT(150)));
//G202: constraint(76)       => ctn_less_equal(149).
	p(NT(76), (NT(149)));
//G203: __E_constraint_61(152) => '[' ctnvar(141) _(11) '<' _(11) num(142) ']'.
	p(NT(152), (T(19)+NT(141)+NT(11)+T(13)+NT(11)+NT(142)+T(20)));
//G204: __E_constraint_61(152) => '[' num(142) _(11) '<' _(11) ctnvar(141) ']'.
	p(NT(152), (T(19)+NT(142)+NT(11)+T(13)+NT(11)+NT(141)+T(20)));
//G205: ctn_less(151)        => __E_constraint_61(152).
	p(NT(151), (NT(152)));
//G206: constraint(76)       => ctn_less(151).
	p(NT(76), (NT(151)));
//G207: __E_variable_62(153) => null.
	p(NT(153), (nul));
//G208: __E_variable_62(153) => digit(3) __E_variable_62(153).
	p(NT(153), (NT(3)+NT(153)));
//G209: variable(116)        => alpha(5) __E_variable_62(153).	 # guarded: charvar
	p(NT(116), (NT(5)+NT(153)));
	p.back().guard = "charvar";
//G210: variable(116)        => chars(154).	 # guarded: var
	p(NT(116), (NT(154)));
	p.back().guard = "var";
//G211: __E_variable_63(156) => in(157).
	p(NT(156), (NT(157)));
//G212: __E_variable_63(156) => out(158).
	p(NT(156), (NT(158)));
//G213: io_var(155)          => __E_variable_63(156).
	p(NT(155), (NT(156)));
//G214: variable(116)        => io_var(155).
	p(NT(116), (NT(155)));
//G215: variable(116)        => uninterpreted_constant(138).
	p(NT(116), (NT(138)));
//G216: __E___E_in_64_65(161) => digit(3).
	p(NT(161), (NT(3)));
//G217: __E___E_in_64_65(161) => digit(3) __E___E_in_64_65(161).
	p(NT(161), (NT(3)+NT(161)));
//G218: __E_in_64(160)       => 'i' __E___E_in_64_65(161).
	p(NT(160), (T(12)+NT(161)));
//G219: in_var_name(159)     => __E_in_64(160).
	p(NT(159), (NT(160)));
//G220: in(157)              => in_var_name(159) '[' offset(162) ']'.
	p(NT(157), (NT(159)+T(19)+NT(162)+T(20)));
//G221: __E___E_out_66_67(165) => digit(3).
	p(NT(165), (NT(3)));
//G222: __E___E_out_66_67(165) => digit(3) __E___E_out_66_67(165).
	p(NT(165), (NT(3)+NT(165)));
//G223: __E_out_66(164)      => 'o' __E___E_out_66_67(165).
	p(NT(164), (T(8)+NT(165)));
//G224: out_var_name(163)    => __E_out_66(164).
	p(NT(163), (NT(164)));
//G225: out(158)             => out_var_name(163) '[' offset(162) ']'.
	p(NT(158), (NT(163)+T(19)+NT(162)+T(20)));
//G226: ctnvar(141)          => chars(154).
	p(NT(141), (NT(154)));
//G227: capture(20)          => '$' chars(154).
	p(NT(20), (T(35)+NT(154)));
//G228: __E_q_vars_68(167)   => _(11) ',' _(11) q_var(166).
	p(NT(167), (NT(11)+T(4)+NT(11)+NT(166)));
//G229: __E_q_vars_69(168)   => null.
	p(NT(168), (nul));
//G230: __E_q_vars_69(168)   => __E_q_vars_68(167) __E_q_vars_69(168).
	p(NT(168), (NT(167)+NT(168)));
//G231: q_vars(72)           => q_var(166) __E_q_vars_69(168).
	p(NT(72), (NT(166)+NT(168)));
//G232: q_var(166)           => capture(20).
	p(NT(166), (NT(20)));
//G233: __N_0(401)           => uninterpreted_constant(138).
	p(NT(401), (NT(138)));
//G234: q_var(166)           => variable(116) & ~( __N_0(401) ).	 # conjunctive
	p(NT(166), (NT(116)) & ~(NT(401)));
//G235: __E_offsets_70(169)  => _(11) ',' _(11) offset(162).
	p(NT(169), (NT(11)+T(4)+NT(11)+NT(162)));
//G236: __E_offsets_71(170)  => null.
	p(NT(170), (nul));
//G237: __E_offsets_71(170)  => __E_offsets_70(169) __E_offsets_71(170).
	p(NT(170), (NT(169)+NT(170)));
//G238: offsets(24)          => '[' _(11) offset(162) __E_offsets_71(170) _(11) ']'.
	p(NT(24), (T(19)+NT(11)+NT(162)+NT(170)+NT(11)+T(20)));
//G239: offset(162)          => num(142).
	p(NT(162), (NT(142)));
//G240: offset(162)          => capture(20).
	p(NT(162), (NT(20)));
//G241: offset(162)          => shift(171).
	p(NT(162), (NT(171)));
//G242: __N_1(402)           => io_var(155).
	p(NT(402), (NT(155)));
//G243: __E_offset_72(172)   => variable(116) & ~( __N_1(402) ).	 # conjunctive
	p(NT(172), (NT(116)) & ~(NT(402)));
//G244: offset(162)          => __E_offset_72(172).
	p(NT(162), (NT(172)));
//G245: __E_shift_73(173)    => capture(20).
	p(NT(173), (NT(20)));
//G246: __N_2(403)           => io_var(155).
	p(NT(403), (NT(155)));
//G247: __E___E_shift_73_74(174) => variable(116) & ~( __N_2(403) ).	 # conjunctive
	p(NT(174), (NT(116)) & ~(NT(403)));
//G248: __E_shift_73(173)    => __E___E_shift_73_74(174).
	p(NT(173), (NT(174)));
//G249: shift(171)           => __E_shift_73(173) _(11) '-' _(11) num(142).
	p(NT(171), (NT(173)+NT(11)+T(23)+NT(11)+NT(142)));
//G250: num(142)             => digits(175).
	p(NT(142), (NT(175)));
//G251: __E___E_uninterpreted_constant_75_76(178) => chars(154).
	p(NT(178), (NT(154)));
//G252: __E___E_uninterpreted_constant_75_76(178) => _(11).
	p(NT(178), (NT(11)));
//G253: __E_uninterpreted_constant_75(177) => __E___E_uninterpreted_constant_75_76(178) ':' chars(154).
	p(NT(177), (NT(178)+T(2)+NT(154)));
//G254: uninter_const_name(176) => __E_uninterpreted_constant_75(177).
	p(NT(176), (NT(177)));
//G255: uninterpreted_constant(138) => '<' uninter_const_name(176) _(11) '>'.
	p(NT(138), (T(13)+NT(176)+NT(11)+T(14)));
//G256: __E_bf_constant_77(180) => capture(20).
	p(NT(180), (NT(20)));
//G257: __E_bf_constant_77(180) => binding(181).
	p(NT(180), (NT(181)));
//G258: constant(179)        => __E_bf_constant_77(180).
	p(NT(179), (NT(180)));
//G259: type(132)            => chars(154).
	p(NT(132), (NT(154)));
//G260: __E_bf_constant_78(182) => _(11) ':' _(11) type(132).
	p(NT(182), (NT(11)+T(2)+NT(11)+NT(132)));
//G261: __E_bf_constant_78(182) => null.
	p(NT(182), (nul));
//G262: bf_constant(115)     => '{' _(11) constant(179) _(11) '}' __E_bf_constant_78(182).
	p(NT(115), (T(36)+NT(11)+NT(179)+NT(11)+T(37)+NT(182)));
//G263: __E___E___E_binding_79_80_81(187) => src_c(185).
	p(NT(187), (NT(185)));
//G264: __E___E___E_binding_79_80_81(187) => space(2).
	p(NT(187), (NT(2)));
//G265: __E___E___E_binding_79_80_82(188) => null.
	p(NT(188), (nul));
//G266: __E___E___E_binding_79_80_82(188) => __E___E___E_binding_79_80_81(187) __E___E___E_binding_79_80_82(188).
	p(NT(188), (NT(187)+NT(188)));
//G267: __E___E_binding_79_80(186) => __E___E___E_binding_79_80_82(188) src_c(185).
	p(NT(186), (NT(188)+NT(185)));
//G268: __E___E_binding_79_80(186) => null.
	p(NT(186), (nul));
//G269: __E_binding_79(184)  => src_c(185) __E___E_binding_79_80(186).
	p(NT(184), (NT(185)+NT(186)));
//G270: source(183)          => __E_binding_79(184).
	p(NT(183), (NT(184)));
//G271: binding(181)         => source(183).
	p(NT(181), (NT(183)));
//G272: src_c(185)           => alnum(6).
	p(NT(185), (NT(6)));
//G273: __N_3(404)           => '{'.
	p(NT(404), (T(36)));
//G274: __N_4(405)           => '}'.
	p(NT(405), (T(37)));
//G275: src_c(185)           => ~( __N_3(404) ) & ~( __N_4(405) ) & punct(7).	 # conjunctive
	p(NT(185), ~(NT(404)) & ~(NT(405)) & (NT(7)));
//G276: __E_src_c_83(189)    => src_c(185).
	p(NT(189), (NT(185)));
//G277: __E_src_c_83(189)    => space(2).
	p(NT(189), (NT(2)));
//G278: __E_src_c_84(190)    => null.
	p(NT(190), (nul));
//G279: __E_src_c_84(190)    => __E_src_c_83(189) __E_src_c_84(190).
	p(NT(190), (NT(189)+NT(190)));
//G280: src_c(185)           => '{' __E_src_c_84(190) '}'.
	p(NT(185), (T(36)+NT(190)+T(37)));
//G281: __E_chars_85(191)    => alnum(6).
	p(NT(191), (NT(6)));
//G282: __E_chars_85(191)    => '_'.
	p(NT(191), (T(38)));
//G283: __E_chars_86(192)    => null.
	p(NT(192), (nul));
//G284: __E_chars_86(192)    => __E_chars_85(191) __E_chars_86(192).
	p(NT(192), (NT(191)+NT(192)));
//G285: chars(154)           => alpha(5) __E_chars_86(192).
	p(NT(154), (NT(5)+NT(192)));
//G286: __E_digits_87(193)   => digit(3).
	p(NT(193), (NT(3)));
//G287: __E_digits_87(193)   => digit(3) __E_digits_87(193).
	p(NT(193), (NT(3)+NT(193)));
//G288: digits(175)          => __E_digits_87(193).
	p(NT(175), (NT(193)));
//G289: sym(22)              => chars(154).
	p(NT(22), (NT(154)));
//G290: extra(194)           => chars(154).
	p(NT(194), (NT(154)));
//G291: __E_comment_88(196)  => printable(8).
	p(NT(196), (NT(8)));
//G292: __E_comment_88(196)  => '\t'.
	p(NT(196), (T(39)));
//G293: __E_comment_89(197)  => null.
	p(NT(197), (nul));
//G294: __E_comment_89(197)  => __E_comment_88(196) __E_comment_89(197).
	p(NT(197), (NT(196)+NT(197)));
//G295: __E_comment_90(198)  => '\n'.
	p(NT(198), (T(40)));
//G296: __E_comment_90(198)  => '\r'.
	p(NT(198), (T(41)));
//G297: __E_comment_90(198)  => eof(1).
	p(NT(198), (NT(1)));
//G298: comment(195)         => '#' __E_comment_89(197) __E_comment_90(198).
	p(NT(195), (T(42)+NT(197)+NT(198)));
//G299: __E____91(199)       => space(2).
	p(NT(199), (NT(2)));
//G300: __E____91(199)       => comment(195).
	p(NT(199), (NT(195)));
//G301: __(52)               => __E____91(199) _(11).
	p(NT(52), (NT(199)+NT(11)));
//G302: __E___92(200)        => __(52).
	p(NT(200), (NT(52)));
//G303: __E___92(200)        => null.
	p(NT(200), (nul));
//G304: _(11)                => __E___92(200).
	p(NT(11), (NT(200)));
//G305: bf_cb_arg(202)       => bf(21).
	p(NT(202), (NT(21)));
//G306: bf_cb_args1(201)     => __(52) bf_cb_arg(202).
	p(NT(201), (NT(52)+NT(202)));
//G307: bf_cb_args2(203)     => bf_cb_args1(201) bf_cb_args1(201).
	p(NT(203), (NT(201)+NT(201)));
//G308: bf_cb_args3(204)     => bf_cb_args2(203) bf_cb_args1(201).
	p(NT(204), (NT(203)+NT(201)));
//G309: bf_cb_args4(205)     => bf_cb_args3(204) bf_cb_args1(201).
	p(NT(205), (NT(204)+NT(201)));
//G310: wff_cb_arg(207)      => wff(14).
	p(NT(207), (NT(14)));
//G311: wff_cb_args1(206)    => __(52) wff_cb_arg(207).
	p(NT(206), (NT(52)+NT(207)));
//G312: wff_cb_args2(208)    => wff_cb_args1(206) wff_cb_args1(206).
	p(NT(208), (NT(206)+NT(206)));
//G313: wff_cb_args3(209)    => wff_cb_args2(208) wff_cb_args1(206).
	p(NT(209), (NT(208)+NT(206)));
//G314: wff_cb_args4(210)    => wff_cb_args3(209) wff_cb_args1(206).
	p(NT(210), (NT(209)+NT(206)));
//G315: __E_wff_cb_93(212)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(208).
	p(NT(212), (T(17)+T(43)+T(43)+T(38)+T(44)+T(15)+T(7)+T(38)+T(45)+T(16)+T(15)+T(7)+T(44)+T(12)+T(46)+T(47)+T(38)+T(7)+T(48)+T(49)+T(43)+T(8)+T(50)+T(9)+T(48)+T(16)+T(15)+T(7)+T(38)+T(45)+T(49)+NT(208)));
//G316: wff_has_clashing_subformulas_cb(211) => __E_wff_cb_93(212).
	p(NT(211), (NT(212)));
//G317: wff_cb(43)           => wff_has_clashing_subformulas_cb(211).
	p(NT(43), (NT(211)));
//G318: __E_wff_cb_94(214)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(209).
	p(NT(214), (T(17)+T(43)+T(43)+T(38)+T(44)+T(15)+T(7)+T(38)+T(7)+T(48)+T(49)+T(43)+T(8)+T(50)+T(9)+T(48)+T(16)+T(15)+T(38)+T(45)+T(49)+NT(209)));
//G319: wff_has_subformula_cb(213) => __E_wff_cb_94(214).
	p(NT(213), (NT(214)));
//G320: wff_cb(43)           => wff_has_subformula_cb(213).
	p(NT(43), (NT(213)));
//G321: __E_wff_cb_95(216)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(208).
	p(NT(216), (T(17)+T(43)+T(43)+T(38)+T(50)+T(10)+T(9)+T(8)+T(51)+T(10)+T(38)+T(10)+T(22)+T(12)+T(7)+T(11)+T(10)+T(46)+T(11)+T(12)+T(15)+T(16)+T(38)+T(45)+T(49)+NT(208)));
//G322: wff_remove_existential_cb(215) => __E_wff_cb_95(216).
	p(NT(215), (NT(216)));
//G323: wff_cb(43)           => wff_remove_existential_cb(215).
	p(NT(43), (NT(215)));
//G324: __E_bf_cb_96(218)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(204).
	p(NT(218), (T(49)+T(43)+T(38)+T(44)+T(15)+T(7)+T(38)+T(7)+T(48)+T(49)+T(43)+T(8)+T(50)+T(9)+T(48)+T(16)+T(15)+T(38)+T(45)+T(49)+NT(204)));
//G325: bf_has_subformula_cb(217) => __E_bf_cb_96(218).
	p(NT(217), (NT(218)));
//G326: bf_cb(47)            => bf_has_subformula_cb(217).
	p(NT(47), (NT(217)));
//G327: __E_bf_cb_97(220)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(205).
	p(NT(220), (T(49)+T(43)+T(38)+T(50)+T(10)+T(9)+T(8)+T(51)+T(10)+T(38)+T(43)+T(48)+T(46)+T(12)+T(51)+T(10)+T(50)+T(7)+T(15)+T(16)+T(38)+T(45)+T(49)+NT(205)));
//G328: bf_remove_funiversal_cb(219) => __E_bf_cb_97(220).
	p(NT(219), (NT(220)));
//G329: bf_cb(47)            => bf_remove_funiversal_cb(219).
	p(NT(47), (NT(219)));
//G330: __E_bf_cb_98(222)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(205).
	p(NT(222), (T(49)+T(43)+T(38)+T(50)+T(10)+T(9)+T(8)+T(51)+T(10)+T(38)+T(43)+T(10)+T(22)+T(12)+T(7)+T(11)+T(10)+T(46)+T(11)+T(12)+T(15)+T(16)+T(38)+T(45)+T(49)+NT(205)));
//G331: bf_remove_fexistential_cb(221) => __E_bf_cb_98(222).
	p(NT(221), (NT(222)));
//G332: bf_cb(47)            => bf_remove_fexistential_cb(221).
	p(NT(47), (NT(221)));
//G333: __E_bf_cb_99(224)    => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(201).
	p(NT(224), (T(49)+T(43)+T(38)+T(46)+T(8)+T(50)+T(9)+T(15)+T(16)+T(12)+T(52)+T(10)+T(38)+T(45)+T(49)+NT(201)));
//G334: bf_normalize_cb(223) => __E_bf_cb_99(224).
	p(NT(223), (NT(224)));
//G335: bf_cb(47)            => bf_normalize_cb(223).
	p(NT(47), (NT(223)));
//G336: cli(225)             => _(11).
	p(NT(225), (NT(11)));
//G337: __E_cli_100(227)     => '.' _(11) cli_command(226) _(11).
	p(NT(227), (T(1)+NT(11)+NT(226)+NT(11)));
//G338: __E_cli_101(228)     => null.
	p(NT(228), (nul));
//G339: __E_cli_101(228)     => __E_cli_100(227) __E_cli_101(228).
	p(NT(228), (NT(227)+NT(228)));
//G340: cli(225)             => _(11) cli_command(226) _(11) __E_cli_101(228).
	p(NT(225), (NT(11)+NT(226)+NT(11)+NT(228)));
//G341: __E_cli_command_102(231) => 'q'.
	p(NT(231), (T(53)));
//G342: __E_cli_command_102(231) => 'q' 'u' 'i' 't'.
	p(NT(231), (T(53)+T(48)+T(12)+T(11)));
//G343: quit_sym(230)        => __E_cli_command_102(231).
	p(NT(230), (NT(231)));
//G344: quit_cmd(229)        => quit_sym(230).
	p(NT(229), (NT(230)));
//G345: cli_command(226)     => quit_cmd(229).
	p(NT(226), (NT(229)));
//G346: __E_cli_command_103(234) => 'v'.
	p(NT(234), (T(51)));
//G347: __E_cli_command_103(234) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(234), (T(51)+T(10)+T(50)+T(7)+T(12)+T(8)+T(46)));
//G348: version_sym(233)     => __E_cli_command_103(234).
	p(NT(233), (NT(234)));
//G349: version_cmd(232)     => version_sym(233).
	p(NT(232), (NT(233)));
//G350: cli_command(226)     => version_cmd(232).
	p(NT(226), (NT(232)));
//G351: __E_cli_command_104(237) => 'c'.
	p(NT(237), (T(45)));
//G352: __E_cli_command_104(237) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(237), (T(45)+T(16)+T(10)+T(15)+T(50)));
//G353: clear_sym(236)       => __E_cli_command_104(237).
	p(NT(236), (NT(237)));
//G354: clear_cmd(235)       => clear_sym(236).
	p(NT(235), (NT(236)));
//G355: cli_command(226)     => clear_cmd(235).
	p(NT(226), (NT(235)));
//G356: __E___E_cli_command_105_106(241) => 'h'.
	p(NT(241), (T(44)));
//G357: __E___E_cli_command_105_106(241) => 'h' 'e' 'l' 'p'.
	p(NT(241), (T(44)+T(10)+T(16)+T(54)));
//G358: help_sym(240)        => __E___E_cli_command_105_106(241).
	p(NT(240), (NT(241)));
//G359: __E___E_cli_command_105_107(242) => __(52) help_arg(243).
	p(NT(242), (NT(52)+NT(243)));
//G360: __E___E_cli_command_105_107(242) => null.
	p(NT(242), (nul));
//G361: __E_cli_command_105(239) => help_sym(240) __E___E_cli_command_105_107(242).
	p(NT(239), (NT(240)+NT(242)));
//G362: help_cmd(238)        => __E_cli_command_105(239).
	p(NT(238), (NT(239)));
//G363: cli_command(226)     => help_cmd(238).
	p(NT(226), (NT(238)));
//G364: file_sym(246)        => 'f' 'i' 'l' 'e'.
	p(NT(246), (T(43)+T(12)+T(16)+T(10)));
//G365: __E_cli_command_108(245) => file_sym(246) __(52) q_string(247).
	p(NT(245), (NT(246)+NT(52)+NT(247)));
//G366: file_cmd(244)        => __E_cli_command_108(245).
	p(NT(244), (NT(245)));
//G367: cli_command(226)     => file_cmd(244).
	p(NT(226), (NT(244)));
//G368: valid_sym(250)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(250), (T(51)+T(15)+T(16)+T(12)+T(55)));
//G369: __E_cli_command_109(249) => valid_sym(250) __(52) normalize_cmd_arg(251).
	p(NT(249), (NT(250)+NT(52)+NT(251)));
//G370: valid_cmd(248)       => __E_cli_command_109(249).
	p(NT(248), (NT(249)));
//G371: cli_command(226)     => valid_cmd(248).
	p(NT(226), (NT(248)));
//G372: sat_sym(254)         => 's' 'a' 't'.
	p(NT(254), (T(7)+T(15)+T(11)));
//G373: __E_cli_command_110(253) => sat_sym(254) __(52) normalize_cmd_arg(251).
	p(NT(253), (NT(254)+NT(52)+NT(251)));
//G374: sat_cmd(252)         => __E_cli_command_110(253).
	p(NT(252), (NT(253)));
//G375: cli_command(226)     => sat_cmd(252).
	p(NT(226), (NT(252)));
//G376: unsat_sym(257)       => 'u' 'n' 's' 'a' 't'.
	p(NT(257), (T(48)+T(46)+T(7)+T(15)+T(11)));
//G377: __E_cli_command_111(256) => unsat_sym(257) __(52) normalize_cmd_arg(251).
	p(NT(256), (NT(257)+NT(52)+NT(251)));
//G378: unsat_cmd(255)       => __E_cli_command_111(256).
	p(NT(255), (NT(256)));
//G379: cli_command(226)     => unsat_cmd(255).
	p(NT(226), (NT(255)));
//G380: solve_sym(260)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(260), (T(7)+T(8)+T(16)+T(51)+T(10)));
//G381: __E___E_cli_command_112_113(261) => __(52) type(132).
	p(NT(261), (NT(52)+NT(132)));
//G382: __E___E_cli_command_112_113(261) => null.
	p(NT(261), (nul));
//G383: __E_cli_command_112(259) => solve_sym(260) __E___E_cli_command_112_113(261) __(52) solve_cmd_arg(262).
	p(NT(259), (NT(260)+NT(261)+NT(52)+NT(262)));
//G384: solve_cmd(258)       => __E_cli_command_112(259).
	p(NT(258), (NT(259)));
//G385: cli_command(226)     => solve_cmd(258).
	p(NT(226), (NT(258)));
//G386: __E___E_cli_command_114_115(266) => 'r'.
	p(NT(266), (T(50)));
//G387: __E___E_cli_command_114_115(266) => 'r' 'u' 'n'.
	p(NT(266), (T(50)+T(48)+T(46)));
//G388: run_sym(265)         => __E___E_cli_command_114_115(266).
	p(NT(265), (NT(266)));
//G389: __E___E_cli_command_114_116(267) => memory(268).
	p(NT(267), (NT(268)));
//G390: __E___E_cli_command_114_116(267) => wff(14).
	p(NT(267), (NT(14)));
//G391: __E_cli_command_114(264) => run_sym(265) __(52) __E___E_cli_command_114_116(267).
	p(NT(264), (NT(265)+NT(52)+NT(267)));
//G392: run_cmd(263)         => __E_cli_command_114(264).
	p(NT(263), (NT(264)));
//G393: cli_command(226)     => run_cmd(263).
	p(NT(226), (NT(263)));
//G394: __E___E_cli_command_117_118(272) => 'n'.
	p(NT(272), (T(46)));
//G395: __E___E_cli_command_117_118(272) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(272), (T(46)+T(8)+T(50)+T(9)+T(15)+T(16)+T(12)+T(52)+T(10)));
//G396: normalize_sym(271)   => __E___E_cli_command_117_118(272).
	p(NT(271), (NT(272)));
//G397: __E_cli_command_117(270) => normalize_sym(271) __(52) normalize_cmd_arg(251).
	p(NT(270), (NT(271)+NT(52)+NT(251)));
//G398: normalize_cmd(269)   => __E_cli_command_117(270).
	p(NT(269), (NT(270)));
//G399: cli_command(226)     => normalize_cmd(269).
	p(NT(226), (NT(269)));
//G400: __E___E_cli_command_119_120(276) => 's'.
	p(NT(276), (T(7)));
//G401: __E___E_cli_command_119_120(276) => 's' 'u' 'b' 's' 't'.
	p(NT(276), (T(7)+T(48)+T(49)+T(7)+T(11)));
//G402: __E___E_cli_command_119_120(276) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(276), (T(7)+T(48)+T(49)+T(7)+T(11)+T(12)+T(11)+T(48)+T(11)+T(10)));
//G403: subst_sym(275)       => __E___E_cli_command_119_120(276).
	p(NT(275), (NT(276)));
//G404: __E_cli_command_119(274) => subst_sym(275) __(52) nf_cmd_arg(277) _(11) '[' _(11) nf_cmd_arg(277) _(11) '/' _(11) nf_cmd_arg(277) _(11) ']'.
	p(NT(274), (NT(275)+NT(52)+NT(277)+NT(11)+T(19)+NT(11)+NT(277)+NT(11)+T(56)+NT(11)+NT(277)+NT(11)+T(20)));
//G405: subst_cmd(273)       => __E_cli_command_119(274).
	p(NT(273), (NT(274)));
//G406: cli_command(226)     => subst_cmd(273).
	p(NT(226), (NT(273)));
//G407: __E___E_cli_command_121_122(281) => 'i'.
	p(NT(281), (T(12)));
//G408: __E___E_cli_command_121_122(281) => 'i' 'n' 's' 't'.
	p(NT(281), (T(12)+T(46)+T(7)+T(11)));
//G409: __E___E_cli_command_121_122(281) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(281), (T(12)+T(46)+T(7)+T(11)+T(15)+T(46)+T(11)+T(12)+T(15)+T(11)+T(10)));
//G410: inst_sym(280)        => __E___E_cli_command_121_122(281).
	p(NT(280), (NT(281)));
//G411: __E_cli_command_121(279) => inst_sym(280) __(52) inst_args(282).
	p(NT(279), (NT(280)+NT(52)+NT(282)));
//G412: inst_cmd(278)        => __E_cli_command_121(279).
	p(NT(278), (NT(279)));
//G413: cli_command(226)     => inst_cmd(278).
	p(NT(226), (NT(278)));
//G414: dnf_sym(285)         => 'd' 'n' 'f'.
	p(NT(285), (T(55)+T(46)+T(43)));
//G415: __E_cli_command_123(284) => dnf_sym(285) __(52) nf_cmd_arg(277).
	p(NT(284), (NT(285)+NT(52)+NT(277)));
//G416: dnf_cmd(283)         => __E_cli_command_123(284).
	p(NT(283), (NT(284)));
//G417: cli_command(226)     => dnf_cmd(283).
	p(NT(226), (NT(283)));
//G418: cnf_sym(288)         => 'c' 'n' 'f'.
	p(NT(288), (T(45)+T(46)+T(43)));
//G419: __E_cli_command_124(287) => cnf_sym(288) __(52) nf_cmd_arg(277).
	p(NT(287), (NT(288)+NT(52)+NT(277)));
//G420: cnf_cmd(286)         => __E_cli_command_124(287).
	p(NT(286), (NT(287)));
//G421: cli_command(226)     => cnf_cmd(286).
	p(NT(226), (NT(286)));
//G422: anf_sym(291)         => 'a' 'n' 'f'.
	p(NT(291), (T(15)+T(46)+T(43)));
//G423: __E_cli_command_125(290) => anf_sym(291) __(52) nf_cmd_arg(277).
	p(NT(290), (NT(291)+NT(52)+NT(277)));
//G424: anf_cmd(289)         => __E_cli_command_125(290).
	p(NT(289), (NT(290)));
//G425: cli_command(226)     => anf_cmd(289).
	p(NT(226), (NT(289)));
//G426: nnf_sym(294)         => 'n' 'n' 'f'.
	p(NT(294), (T(46)+T(46)+T(43)));
//G427: __E_cli_command_126(293) => nnf_sym(294) __(52) nf_cmd_arg(277).
	p(NT(293), (NT(294)+NT(52)+NT(277)));
//G428: nnf_cmd(292)         => __E_cli_command_126(293).
	p(NT(292), (NT(293)));
//G429: cli_command(226)     => nnf_cmd(292).
	p(NT(226), (NT(292)));
//G430: pnf_sym(297)         => 'p' 'n' 'f'.
	p(NT(297), (T(54)+T(46)+T(43)));
//G431: __E_cli_command_127(296) => pnf_sym(297) __(52) nf_cmd_arg(277).
	p(NT(296), (NT(297)+NT(52)+NT(277)));
//G432: pnf_cmd(295)         => __E_cli_command_127(296).
	p(NT(295), (NT(296)));
//G433: cli_command(226)     => pnf_cmd(295).
	p(NT(226), (NT(295)));
//G434: mnf_sym(300)         => 'm' 'n' 'f'.
	p(NT(300), (T(9)+T(46)+T(43)));
//G435: __E_cli_command_128(299) => mnf_sym(300) __(52) nf_cmd_arg(277).
	p(NT(299), (NT(300)+NT(52)+NT(277)));
//G436: mnf_cmd(298)         => __E_cli_command_128(299).
	p(NT(298), (NT(299)));
//G437: cli_command(226)     => mnf_cmd(298).
	p(NT(226), (NT(298)));
//G438: snf_sym(303)         => 's' 'n' 'f'.
	p(NT(303), (T(7)+T(46)+T(43)));
//G439: __E_cli_command_129(302) => snf_sym(303) __(52) nf_cmd_arg(277).
	p(NT(302), (NT(303)+NT(52)+NT(277)));
//G440: snf_cmd(301)         => __E_cli_command_129(302).
	p(NT(301), (NT(302)));
//G441: cli_command(226)     => snf_cmd(301).
	p(NT(226), (NT(301)));
//G442: onf_sym(306)         => 'o' 'n' 'f'.
	p(NT(306), (T(8)+T(46)+T(43)));
//G443: __E_cli_command_130(305) => onf_sym(306) __(52) variable(116) __(52) onf_cmd_arg(307).
	p(NT(305), (NT(306)+NT(52)+NT(116)+NT(52)+NT(307)));
//G444: onf_cmd(304)         => __E_cli_command_130(305).
	p(NT(304), (NT(305)));
//G445: cli_command(226)     => onf_cmd(304).
	p(NT(226), (NT(304)));
//G446: __E___E_cli_command_131_132(311) => 'd' 'e' 'f' 's'.
	p(NT(311), (T(55)+T(10)+T(43)+T(7)));
//G447: __E___E_cli_command_131_132(311) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(311), (T(55)+T(10)+T(43)+T(12)+T(46)+T(12)+T(11)+T(12)+T(8)+T(46)+T(7)));
//G448: def_sym(310)         => __E___E_cli_command_131_132(311).
	p(NT(310), (NT(311)));
//G449: __E_cli_command_131(309) => def_sym(310).
	p(NT(309), (NT(310)));
//G450: def_list_cmd(308)    => __E_cli_command_131(309).
	p(NT(308), (NT(309)));
//G451: cli_command(226)     => def_list_cmd(308).
	p(NT(226), (NT(308)));
//G452: __E_cli_command_133(313) => def_sym(310) __(52) number(314).
	p(NT(313), (NT(310)+NT(52)+NT(314)));
//G453: def_print_cmd(312)   => __E_cli_command_133(313).
	p(NT(312), (NT(313)));
//G454: cli_command(226)     => def_print_cmd(312).
	p(NT(226), (NT(312)));
//G455: def_rr_cmd(315)      => rec_relation(16).
	p(NT(315), (NT(16)));
//G456: cli_command(226)     => def_rr_cmd(315).
	p(NT(226), (NT(315)));
//G457: qelim_sym(318)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(318), (T(53)+T(10)+T(16)+T(12)+T(9)));
//G458: __E_cli_command_134(317) => qelim_sym(318) __(52) wff_cmd_arg(319).
	p(NT(317), (NT(318)+NT(52)+NT(319)));
//G459: qelim_cmd(316)       => __E_cli_command_134(317).
	p(NT(316), (NT(317)));
//G460: cli_command(226)     => qelim_cmd(316).
	p(NT(226), (NT(316)));
//G461: get_sym(322)         => 'g' 'e' 't'.
	p(NT(322), (T(47)+T(10)+T(11)));
//G462: __E___E_cli_command_135_136(323) => __(52) option(324).
	p(NT(323), (NT(52)+NT(324)));
//G463: __E___E_cli_command_135_136(323) => null.
	p(NT(323), (nul));
//G464: __E_cli_command_135(321) => get_sym(322) __E___E_cli_command_135_136(323).
	p(NT(321), (NT(322)+NT(323)));
//G465: get_cmd(320)         => __E_cli_command_135(321).
	p(NT(320), (NT(321)));
//G466: cli_command(226)     => get_cmd(320).
	p(NT(226), (NT(320)));
//G467: set_sym(327)         => 's' 'e' 't'.
	p(NT(327), (T(7)+T(10)+T(11)));
//G468: __E___E_cli_command_137_138(328) => __(52).
	p(NT(328), (NT(52)));
//G469: __E___E_cli_command_137_138(328) => _(11) '=' _(11).
	p(NT(328), (NT(11)+T(3)+NT(11)));
//G470: __E_cli_command_137(326) => set_sym(327) __(52) option(324) __E___E_cli_command_137_138(328) option_value(329).
	p(NT(326), (NT(327)+NT(52)+NT(324)+NT(328)+NT(329)));
//G471: set_cmd(325)         => __E_cli_command_137(326).
	p(NT(325), (NT(326)));
//G472: cli_command(226)     => set_cmd(325).
	p(NT(226), (NT(325)));
//G473: enable_sym(332)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(332), (T(10)+T(46)+T(15)+T(49)+T(16)+T(10)));
//G474: __E_cli_command_139(331) => enable_sym(332) __(52) bool_option(333).
	p(NT(331), (NT(332)+NT(52)+NT(333)));
//G475: enable_cmd(330)      => __E_cli_command_139(331).
	p(NT(330), (NT(331)));
//G476: cli_command(226)     => enable_cmd(330).
	p(NT(226), (NT(330)));
//G477: disable_sym(336)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(336), (T(55)+T(12)+T(7)+T(15)+T(49)+T(16)+T(10)));
//G478: __E_cli_command_140(335) => disable_sym(336) __(52) bool_option(333).
	p(NT(335), (NT(336)+NT(52)+NT(333)));
//G479: disable_cmd(334)     => __E_cli_command_140(335).
	p(NT(334), (NT(335)));
//G480: cli_command(226)     => disable_cmd(334).
	p(NT(226), (NT(334)));
//G481: toggle_sym(339)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(339), (T(11)+T(8)+T(47)+T(47)+T(16)+T(10)));
//G482: __E_cli_command_141(338) => toggle_sym(339) __(52) bool_option(333).
	p(NT(338), (NT(339)+NT(52)+NT(333)));
//G483: toggle_cmd(337)      => __E_cli_command_141(338).
	p(NT(337), (NT(338)));
//G484: cli_command(226)     => toggle_cmd(337).
	p(NT(226), (NT(337)));
//G485: __E_cli_command_142(341) => type(132) __(52) in_var_name(159) _(11) '=' _(11) input_stream(342).
	p(NT(341), (NT(132)+NT(52)+NT(159)+NT(11)+T(3)+NT(11)+NT(342)));
//G486: def_input_cmd(340)   => __E_cli_command_142(341).
	p(NT(340), (NT(341)));
//G487: cli_command(226)     => def_input_cmd(340).
	p(NT(226), (NT(340)));
//G488: __E_cli_command_143(344) => type(132) __(52) out_var_name(163) _(11) '=' _(11) output_stream(345).
	p(NT(344), (NT(132)+NT(52)+NT(163)+NT(11)+T(3)+NT(11)+NT(345)));
//G489: def_output_cmd(343)  => __E_cli_command_143(344).
	p(NT(343), (NT(344)));
//G490: cli_command(226)     => def_output_cmd(343).
	p(NT(226), (NT(343)));
//G491: __E___E_cli_command_144_145(349) => 'h' 'i' 's' 't'.
	p(NT(349), (T(44)+T(12)+T(7)+T(11)));
//G492: __E___E_cli_command_144_145(349) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(349), (T(44)+T(12)+T(7)+T(11)+T(8)+T(50)+T(18)));
//G493: history_sym(348)     => __E___E_cli_command_144_145(349).
	p(NT(348), (NT(349)));
//G494: __E_cli_command_144(347) => history_sym(348).
	p(NT(347), (NT(348)));
//G495: history_list_cmd(346) => __E_cli_command_144(347).
	p(NT(346), (NT(347)));
//G496: cli_command(226)     => history_list_cmd(346).
	p(NT(226), (NT(346)));
//G497: __E_cli_command_146(351) => history_sym(348) __(52) memory(268).
	p(NT(351), (NT(348)+NT(52)+NT(268)));
//G498: history_print_cmd(350) => __E_cli_command_146(351).
	p(NT(350), (NT(351)));
//G499: cli_command(226)     => history_print_cmd(350).
	p(NT(226), (NT(350)));
//G500: __E_cli_command_147(353) => wff(14).
	p(NT(353), (NT(14)));
//G501: __E_cli_command_147(353) => bf(21).
	p(NT(353), (NT(21)));
//G502: history_store_cmd(352) => __E_cli_command_147(353).
	p(NT(352), (NT(353)));
//G503: cli_command(226)     => history_store_cmd(352).
	p(NT(226), (NT(352)));
//G504: __E_input_stream_148(354) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(355) _(11) ')'.
	p(NT(354), (T(12)+T(43)+T(12)+T(16)+T(10)+NT(11)+T(5)+NT(11)+NT(355)+NT(11)+T(6)));
//G505: input_stream(342)    => __E_input_stream_148(354).
	p(NT(342), (NT(354)));
//G506: console_sym(356)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(356), (T(45)+T(8)+T(46)+T(7)+T(8)+T(16)+T(10)));
//G507: input_stream(342)    => console_sym(356).
	p(NT(342), (NT(356)));
//G508: __E_output_stream_149(357) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(355) _(11) ')'.
	p(NT(357), (T(8)+T(43)+T(12)+T(16)+T(10)+NT(11)+T(5)+NT(11)+NT(355)+NT(11)+T(6)));
//G509: output_stream(345)   => __E_output_stream_149(357).
	p(NT(345), (NT(357)));
//G510: output_stream(345)   => console_sym(356).
	p(NT(345), (NT(356)));
//G511: q_file_name(355)     => '"' file_name(358) '"'.
	p(NT(355), (T(57)+NT(358)+T(57)));
//G512: __E_file_name_150(359) => printable(8).
	p(NT(359), (NT(8)));
//G513: __E_file_name_150(359) => printable(8) __E_file_name_150(359).
	p(NT(359), (NT(8)+NT(359)));
//G514: file_name(358)       => __E_file_name_150(359).
	p(NT(358), (NT(359)));
//G515: number(314)          => digits(175).
	p(NT(314), (NT(175)));
//G516: bf_cmd_arg(360)      => memory(268).
	p(NT(360), (NT(268)));
//G517: bf_cmd_arg(360)      => bf(21).
	p(NT(360), (NT(21)));
//G518: wff_cmd_arg(319)     => memory(268).
	p(NT(319), (NT(268)));
//G519: wff_cmd_arg(319)     => wff(14).
	p(NT(319), (NT(14)));
//G520: solve_cmd_arg(262)   => memory(268).
	p(NT(262), (NT(268)));
//G521: solve_cmd_arg(262)   => wff(14).
	p(NT(262), (NT(14)));
//G522: nf_cmd_arg(277)      => memory(268).
	p(NT(277), (NT(268)));
//G523: nf_cmd_arg(277)      => ref(18).
	p(NT(277), (NT(18)));
//G524: nf_cmd_arg(277)      => wff(14).
	p(NT(277), (NT(14)));
//G525: nf_cmd_arg(277)      => bf(21).
	p(NT(277), (NT(21)));
//G526: onf_cmd_arg(307)     => memory(268).
	p(NT(307), (NT(268)));
//G527: onf_cmd_arg(307)     => wff(14).
	p(NT(307), (NT(14)));
//G528: normalize_cmd_arg(251) => memory(268).
	p(NT(251), (NT(268)));
//G529: normalize_cmd_arg(251) => rr(10).
	p(NT(251), (NT(10)));
//G530: normalize_cmd_arg(251) => ref(18).
	p(NT(251), (NT(18)));
//G531: normalize_cmd_arg(251) => wff(14).
	p(NT(251), (NT(14)));
//G532: normalize_cmd_arg(251) => bf(21).
	p(NT(251), (NT(21)));
//G533: inst_args(282)       => wff_cmd_arg(319) _(11) '[' _(11) variable(116) _(11) '/' _(11) bf_cmd_arg(360) _(11) ']'.
	p(NT(282), (NT(319)+NT(11)+T(19)+NT(11)+NT(116)+NT(11)+T(56)+NT(11)+NT(360)+NT(11)+T(20)));
//G534: inst_args(282)       => bf_cmd_arg(360) _(11) '[' _(11) variable(116) _(11) '/' _(11) bf_cmd_arg(360) _(11) ']'.
	p(NT(282), (NT(360)+NT(11)+T(19)+NT(11)+NT(116)+NT(11)+T(56)+NT(11)+NT(360)+NT(11)+T(20)));
//G535: help_arg(243)        => help_sym(240).
	p(NT(243), (NT(240)));
//G536: help_arg(243)        => version_sym(233).
	p(NT(243), (NT(233)));
//G537: help_arg(243)        => quit_sym(230).
	p(NT(243), (NT(230)));
//G538: help_arg(243)        => clear_sym(236).
	p(NT(243), (NT(236)));
//G539: help_arg(243)        => get_sym(322).
	p(NT(243), (NT(322)));
//G540: help_arg(243)        => set_sym(327).
	p(NT(243), (NT(327)));
//G541: help_arg(243)        => enable_sym(332).
	p(NT(243), (NT(332)));
//G542: help_arg(243)        => disable_sym(336).
	p(NT(243), (NT(336)));
//G543: help_arg(243)        => toggle_sym(339).
	p(NT(243), (NT(339)));
//G544: help_arg(243)        => file_sym(246).
	p(NT(243), (NT(246)));
//G545: help_arg(243)        => history_sym(348).
	p(NT(243), (NT(348)));
//G546: help_arg(243)        => abs_memory_sym(361).
	p(NT(243), (NT(361)));
//G547: help_arg(243)        => rel_memory_sym(362).
	p(NT(243), (NT(362)));
//G548: help_arg(243)        => selection_sym(363).
	p(NT(243), (NT(363)));
//G549: help_arg(243)        => def_sym(310).
	p(NT(243), (NT(310)));
//G550: help_arg(243)        => inst_sym(280).
	p(NT(243), (NT(280)));
//G551: help_arg(243)        => subst_sym(275).
	p(NT(243), (NT(275)));
//G552: help_arg(243)        => normalize_sym(271).
	p(NT(243), (NT(271)));
//G553: help_arg(243)        => execute_sym(364).
	p(NT(243), (NT(364)));
//G554: help_arg(243)        => solve_sym(260).
	p(NT(243), (NT(260)));
//G555: help_arg(243)        => valid_sym(250).
	p(NT(243), (NT(250)));
//G556: help_arg(243)        => sat_sym(254).
	p(NT(243), (NT(254)));
//G557: help_arg(243)        => unsat_sym(257).
	p(NT(243), (NT(257)));
//G558: help_arg(243)        => run_sym(265).
	p(NT(243), (NT(265)));
//G559: help_arg(243)        => dnf_sym(285).
	p(NT(243), (NT(285)));
//G560: help_arg(243)        => cnf_sym(288).
	p(NT(243), (NT(288)));
//G561: help_arg(243)        => anf_sym(291).
	p(NT(243), (NT(291)));
//G562: help_arg(243)        => snf_sym(303).
	p(NT(243), (NT(303)));
//G563: help_arg(243)        => nnf_sym(294).
	p(NT(243), (NT(294)));
//G564: help_arg(243)        => pnf_sym(297).
	p(NT(243), (NT(297)));
//G565: help_arg(243)        => mnf_sym(300).
	p(NT(243), (NT(300)));
//G566: help_arg(243)        => onf_sym(306).
	p(NT(243), (NT(306)));
//G567: help_arg(243)        => qelim_sym(318).
	p(NT(243), (NT(318)));
//G568: __E___E_help_arg_151_152(367) => 's'.
	p(NT(367), (T(7)));
//G569: __E___E_help_arg_151_152(367) => null.
	p(NT(367), (nul));
//G570: __E_help_arg_151(366) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_151_152(367).
	p(NT(366), (T(10)+T(22)+T(15)+T(9)+T(54)+T(16)+T(10)+NT(367)));
//G571: examples_sym(365)    => __E_help_arg_151(366).
	p(NT(365), (NT(366)));
//G572: help_arg(243)        => examples_sym(365).
	p(NT(243), (NT(365)));
//G573: __E_memory_153(369)  => '%'.
	p(NT(369), (T(58)));
//G574: rel_memory_sym(362)  => '%' '-'.
	p(NT(362), (T(58)+T(23)));
//G575: memory_id(371)       => digits(175).
	p(NT(371), (NT(175)));
//G576: __E___E_memory_153_154(370) => memory_id(371).
	p(NT(370), (NT(371)));
//G577: __E___E_memory_153_154(370) => null.
	p(NT(370), (nul));
//G578: __E_memory_153(369)  => rel_memory_sym(362) __E___E_memory_153_154(370).
	p(NT(369), (NT(362)+NT(370)));
//G579: rel_memory(368)      => __E_memory_153(369).
	p(NT(368), (NT(369)));
//G580: memory(268)          => rel_memory(368).
	p(NT(268), (NT(368)));
//G581: abs_memory_sym(361)  => '%'.
	p(NT(361), (T(58)));
//G582: __E_memory_155(373)  => abs_memory_sym(361) memory_id(371).
	p(NT(373), (NT(361)+NT(371)));
//G583: abs_memory(372)      => __E_memory_155(373).
	p(NT(372), (NT(373)));
//G584: memory(268)          => abs_memory(372).
	p(NT(268), (NT(372)));
//G585: option(324)          => bool_option(333).
	p(NT(324), (NT(333)));
//G586: __E_option_156(376)  => 'S'.
	p(NT(376), (T(30)));
//G587: __E_option_156(376)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(376), (T(7)+T(10)+T(51)+T(10)+T(50)+T(12)+T(11)+T(18)));
//G588: severity_opt(375)    => __E_option_156(376).
	p(NT(375), (NT(376)));
//G589: enum_option(374)     => severity_opt(375).
	p(NT(374), (NT(375)));
//G590: option(324)          => enum_option(374).
	p(NT(324), (NT(374)));
//G591: __E_bool_option_157(378) => 's'.
	p(NT(378), (T(7)));
//G592: __E_bool_option_157(378) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(378), (T(7)+T(11)+T(15)+T(11)+T(48)+T(7)));
//G593: status_opt(377)      => __E_bool_option_157(378).
	p(NT(377), (NT(378)));
//G594: bool_option(333)     => status_opt(377).
	p(NT(333), (NT(377)));
//G595: __E_bool_option_158(380) => 'c'.
	p(NT(380), (T(45)));
//G596: __E_bool_option_158(380) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(380), (T(45)+T(8)+T(16)+T(8)+T(50)));
//G597: __E_bool_option_158(380) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(380), (T(45)+T(8)+T(16)+T(8)+T(50)+T(7)));
//G598: colors_opt(379)      => __E_bool_option_158(380).
	p(NT(379), (NT(380)));
//G599: bool_option(333)     => colors_opt(379).
	p(NT(333), (NT(379)));
//G600: __E_bool_option_159(382) => 'V'.
	p(NT(382), (T(59)));
//G601: __E_bool_option_159(382) => 'c' 'h' 'a' 'r' 'v' 'a' 'r'.
	p(NT(382), (T(45)+T(44)+T(15)+T(50)+T(51)+T(15)+T(50)));
//G602: charvar_opt(381)     => __E_bool_option_159(382).
	p(NT(381), (NT(382)));
//G603: bool_option(333)     => charvar_opt(381).
	p(NT(333), (NT(381)));
//G604: __E_bool_option_160(384) => 'H'.
	p(NT(384), (T(60)));
//G605: __E_bool_option_160(384) => 'h' 'i' 'g' 'h' 'l' 'i' 'g' 'h' 't'.
	p(NT(384), (T(44)+T(12)+T(47)+T(44)+T(16)+T(12)+T(47)+T(44)+T(11)));
//G606: __E_bool_option_160(384) => 'h' 'i' 'g' 'h' 'l' 'i' 'g' 'h' 't' 'i' 'n' 'g'.
	p(NT(384), (T(44)+T(12)+T(47)+T(44)+T(16)+T(12)+T(47)+T(44)+T(11)+T(12)+T(46)+T(47)));
//G607: highlighting_opt(383) => __E_bool_option_160(384).
	p(NT(383), (NT(384)));
//G608: bool_option(333)     => highlighting_opt(383).
	p(NT(333), (NT(383)));
//G609: __E_bool_option_161(386) => 'I'.
	p(NT(386), (T(61)));
//G610: __E_bool_option_161(386) => 'i' 'n' 'd' 'e' 'n' 't'.
	p(NT(386), (T(12)+T(46)+T(55)+T(10)+T(46)+T(11)));
//G611: __E_bool_option_161(386) => 'i' 'n' 'd' 'e' 'n' 't' 'i' 'n' 'g'.
	p(NT(386), (T(12)+T(46)+T(55)+T(10)+T(46)+T(11)+T(12)+T(46)+T(47)));
//G612: indenting_opt(385)   => __E_bool_option_161(386).
	p(NT(385), (NT(386)));
//G613: bool_option(333)     => indenting_opt(385).
	p(NT(333), (NT(385)));
//G614: __E_bool_option_162(388) => 'd'.
	p(NT(388), (T(55)));
//G615: __E_bool_option_162(388) => 'd' 'b' 'g'.
	p(NT(388), (T(55)+T(49)+T(47)));
//G616: __E_bool_option_162(388) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(388), (T(55)+T(10)+T(49)+T(48)+T(47)));
//G617: debug_repl_opt(387)  => __E_bool_option_162(388).
	p(NT(387), (NT(388)));
//G618: bool_option(333)     => debug_repl_opt(387).
	p(NT(333), (NT(387)));
//G619: option_value(329)    => option_value_true(389).
	p(NT(329), (NT(389)));
//G620: option_value(329)    => option_value_false(390).
	p(NT(329), (NT(390)));
//G621: option_value(329)    => severity(391).
	p(NT(329), (NT(391)));
//G622: option_value_true(389) => 't'.
	p(NT(389), (T(11)));
//G623: option_value_true(389) => 't' 'r' 'u' 'e'.
	p(NT(389), (T(11)+T(50)+T(48)+T(10)));
//G624: option_value_true(389) => 'o' 'n'.
	p(NT(389), (T(8)+T(46)));
//G625: option_value_true(389) => '1'.
	p(NT(389), (T(33)));
//G626: option_value_true(389) => 'y'.
	p(NT(389), (T(18)));
//G627: option_value_true(389) => 'y' 'e' 's'.
	p(NT(389), (T(18)+T(10)+T(7)));
//G628: option_value_false(390) => 'f'.
	p(NT(390), (T(43)));
//G629: option_value_false(390) => 'f' 'a' 'l' 's' 'e'.
	p(NT(390), (T(43)+T(15)+T(16)+T(7)+T(10)));
//G630: option_value_false(390) => 'o' 'f' 'f'.
	p(NT(390), (T(8)+T(43)+T(43)));
//G631: option_value_false(390) => '0'.
	p(NT(390), (T(34)));
//G632: option_value_false(390) => 'n'.
	p(NT(390), (T(46)));
//G633: option_value_false(390) => 'n' 'o'.
	p(NT(390), (T(46)+T(8)));
//G634: __E_severity_163(393) => 'e'.
	p(NT(393), (T(10)));
//G635: __E_severity_163(393) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(393), (T(10)+T(50)+T(50)+T(8)+T(50)));
//G636: error_sym(392)       => __E_severity_163(393).
	p(NT(392), (NT(393)));
//G637: severity(391)        => error_sym(392).
	p(NT(391), (NT(392)));
//G638: __E_severity_164(395) => 'i'.
	p(NT(395), (T(12)));
//G639: __E_severity_164(395) => 'i' 'n' 'f' 'o'.
	p(NT(395), (T(12)+T(46)+T(43)+T(8)));
//G640: info_sym(394)        => __E_severity_164(395).
	p(NT(394), (NT(395)));
//G641: severity(391)        => info_sym(394).
	p(NT(391), (NT(394)));
//G642: __E_severity_165(397) => 'd'.
	p(NT(397), (T(55)));
//G643: __E_severity_165(397) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(397), (T(55)+T(10)+T(49)+T(48)+T(47)));
//G644: debug_sym(396)       => __E_severity_165(397).
	p(NT(396), (NT(397)));
//G645: severity(391)        => debug_sym(396).
	p(NT(391), (NT(396)));
//G646: __E_severity_166(399) => 't'.
	p(NT(399), (T(11)));
//G647: __E_severity_166(399) => 't' 'r' 'a' 'c' 'e'.
	p(NT(399), (T(11)+T(50)+T(15)+T(45)+T(10)));
//G648: trace_sym(398)       => __E_severity_166(399).
	p(NT(398), (NT(399)));
//G649: severity(391)        => trace_sym(398).
	p(NT(391), (NT(398)));
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
		__E_bf_51, __E___E_bf_51_52, type, bf_f, __E_bf_53, __E___E_bf_53_54, bf_and_nosep_1st_op, __E_bf_and_55, uninterpreted_constant, ctn_neq, 
		__E_constraint_56, ctnvar, num, ctn_eq, __E_constraint_57, ctn_greater_equal, __E_constraint_58, ctn_greater, __E_constraint_59, ctn_less_equal, 
		__E_constraint_60, ctn_less, __E_constraint_61, __E_variable_62, chars, io_var, __E_variable_63, in, out, in_var_name, 
		__E_in_64, __E___E_in_64_65, offset, out_var_name, __E_out_66, __E___E_out_66_67, q_var, __E_q_vars_68, __E_q_vars_69, __E_offsets_70, 
		__E_offsets_71, shift, __E_offset_72, __E_shift_73, __E___E_shift_73_74, digits, uninter_const_name, __E_uninterpreted_constant_75, __E___E_uninterpreted_constant_75_76, constant, 
		__E_bf_constant_77, binding, __E_bf_constant_78, source, __E_binding_79, src_c, __E___E_binding_79_80, __E___E___E_binding_79_80_81, __E___E___E_binding_79_80_82, __E_src_c_83, 
		__E_src_c_84, __E_chars_85, __E_chars_86, __E_digits_87, extra, comment, __E_comment_88, __E_comment_89, __E_comment_90, __E____91, 
		__E___92, bf_cb_args1, bf_cb_arg, bf_cb_args2, bf_cb_args3, bf_cb_args4, wff_cb_args1, wff_cb_arg, wff_cb_args2, wff_cb_args3, 
		wff_cb_args4, wff_has_clashing_subformulas_cb, __E_wff_cb_93, wff_has_subformula_cb, __E_wff_cb_94, wff_remove_existential_cb, __E_wff_cb_95, bf_has_subformula_cb, __E_bf_cb_96, bf_remove_funiversal_cb, 
		__E_bf_cb_97, bf_remove_fexistential_cb, __E_bf_cb_98, bf_normalize_cb, __E_bf_cb_99, cli, cli_command, __E_cli_100, __E_cli_101, quit_cmd, 
		quit_sym, __E_cli_command_102, version_cmd, version_sym, __E_cli_command_103, clear_cmd, clear_sym, __E_cli_command_104, help_cmd, __E_cli_command_105, 
		help_sym, __E___E_cli_command_105_106, __E___E_cli_command_105_107, help_arg, file_cmd, __E_cli_command_108, file_sym, q_string, valid_cmd, __E_cli_command_109, 
		valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_110, sat_sym, unsat_cmd, __E_cli_command_111, unsat_sym, solve_cmd, __E_cli_command_112, 
		solve_sym, __E___E_cli_command_112_113, solve_cmd_arg, run_cmd, __E_cli_command_114, run_sym, __E___E_cli_command_114_115, __E___E_cli_command_114_116, memory, normalize_cmd, 
		__E_cli_command_117, normalize_sym, __E___E_cli_command_117_118, subst_cmd, __E_cli_command_119, subst_sym, __E___E_cli_command_119_120, nf_cmd_arg, inst_cmd, __E_cli_command_121, 
		inst_sym, __E___E_cli_command_121_122, inst_args, dnf_cmd, __E_cli_command_123, dnf_sym, cnf_cmd, __E_cli_command_124, cnf_sym, anf_cmd, 
		__E_cli_command_125, anf_sym, nnf_cmd, __E_cli_command_126, nnf_sym, pnf_cmd, __E_cli_command_127, pnf_sym, mnf_cmd, __E_cli_command_128, 
		mnf_sym, snf_cmd, __E_cli_command_129, snf_sym, onf_cmd, __E_cli_command_130, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_131, 
		def_sym, __E___E_cli_command_131_132, def_print_cmd, __E_cli_command_133, number, def_rr_cmd, qelim_cmd, __E_cli_command_134, qelim_sym, wff_cmd_arg, 
		get_cmd, __E_cli_command_135, get_sym, __E___E_cli_command_135_136, option, set_cmd, __E_cli_command_137, set_sym, __E___E_cli_command_137_138, option_value, 
		enable_cmd, __E_cli_command_139, enable_sym, bool_option, disable_cmd, __E_cli_command_140, disable_sym, toggle_cmd, __E_cli_command_141, toggle_sym, 
		def_input_cmd, __E_cli_command_142, input_stream, def_output_cmd, __E_cli_command_143, output_stream, history_list_cmd, __E_cli_command_144, history_sym, __E___E_cli_command_144_145, 
		history_print_cmd, __E_cli_command_146, history_store_cmd, __E_cli_command_147, __E_input_stream_148, q_file_name, console_sym, __E_output_stream_149, file_name, __E_file_name_150, 
		bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, execute_sym, examples_sym, __E_help_arg_151, __E___E_help_arg_151_152, rel_memory, __E_memory_153, 
		__E___E_memory_153_154, memory_id, abs_memory, __E_memory_155, enum_option, severity_opt, __E_option_156, status_opt, __E_bool_option_157, colors_opt, 
		__E_bool_option_158, charvar_opt, __E_bool_option_159, highlighting_opt, __E_bool_option_160, indenting_opt, __E_bool_option_161, debug_repl_opt, __E_bool_option_162, option_value_true, 
		option_value_false, severity, error_sym, __E_severity_163, info_sym, __E_severity_164, debug_sym, __E_severity_165, trace_sym, __E_severity_166, 
		named_binding, __N_0, __N_1, __N_2, __N_3, __N_4, 
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
