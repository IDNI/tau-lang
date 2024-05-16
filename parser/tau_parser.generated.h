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
	"__E_start_0", "inputs", "builder", "library", "nso_rr", "gssotc_rr", "_", "gssotc_rec_relations", "gssotc_main", "__E_gssotc_rr_1", 
	"tau", "__E_gssotc_rec_relations_2", "gssotc_rec_relation", "__E_gssotc_rec_relations_3", "gssotc_rec_relation_form", "__E_gssotc_rec_relation_4", "bf_rec_relation", "wff_rec_relation", "tau_rec_relation", "nso_rec_relations", 
	"nso_main", "__E_nso_rr_5", "wff", "__E_nso_rec_relations_6", "nso_rec_relation", "__E_nso_rec_relations_7", "nso_rec_relation_form", "__E_nso_rec_relation_8", "tau_ref", "wff_ref", 
	"bf_ref", "bf", "__E_inputs_9", "__E_inputs_10", "input", "in", "source_binding", "rules", "__E_library_11", "__E___E_library_11_12", 
	"rule", "__E___E___E_library_11_12_13", "wff_rule", "bf_rule", "tau_rule", "__E___E_library_11_14", "tau_matcher", "tau_body", "__E_tau_rule_15", "tau_cb", 
	"wff_matcher", "wff_body", "__E_wff_rule_16", "wff_cb", "bf_matcher", "bf_body", "__E_bf_rule_17", "bf_cb", "builder_head", "builder_body", 
	"capture", "__E_builder_head_18", "__", "__E_builder_head_19", "bf_builder_body", "__E_builder_body_20", "wff_builder_body", "__E_builder_body_21", "tau_builder_body", "__E_builder_body_22", 
	"tau_parenthesis", "__E_tau_23", "tau_wff", "__E_tau_24", "__E_tau_25", "sym", "__E___E_tau_25_26", "offsets", "ref_args", "tau_or", 
	"__E_tau_27", "tau_and", "__E_tau_28", "tau_neg", "__E_tau_29", "wff_parenthesis", "__E_wff_30", "wff_conditional", "__E_wff_31", "wff_ball", 
	"__E_wff_32", "q_bool_var", "wff_bex", "__E_wff_33", "wff_all", "__E_wff_34", "q_var", "wff_ex", "__E_wff_35", "__E_wff_36", 
	"__E___E_wff_36_37", "wff_imply", "__E_wff_38", "wff_equiv", "__E_wff_39", "wff_or", "__E_wff_40", "wff_xor", "__E_wff_41", "wff_and", 
	"__E_wff_42", "wff_neg", "__E_wff_43", "wff_t", "wff_f", "bf_interval", "__E_wff_44", "bf_neq", "__E_wff_45", "bf_eq", 
	"__E_wff_46", "bf_nleq_lower", "__E_wff_47", "bf_nleq_upper", "__E_wff_48", "bf_greater", "__E_wff_49", "bf_less_equal", "__E_wff_50", "bf_less", 
	"__E_wff_51", "bool_variable", "bf_parenthesis", "__E_bf_52", "bf_splitter", "__E_bf_53", "bf_all", "__E_bf_54", "bf_ex", "__E_bf_55", 
	"__E_bf_56", "__E___E_bf_56_57", "bf_or", "__E_bf_58", "bf_xor", "__E_bf_59", "bf_and", "__E_bf_60", "__E___E_bf_60_61", "bf_neg", 
	"__E_bf_62", "bf_constant", "__E_bf_63", "constant", "bf_t", "bf_f", "variable", "__E_ref_args_64", "__E___E_ref_args_64_65", "ref_arg", 
	"__E___E_ref_args_64_66", "__E___E_ref_args_64_67", "bf_cb_args1", "bf_cb_arg", "bf_cb_args2", "bf_cb_args3", "bf_cb_args4", "wff_cb_args1", "wff_cb_arg", "wff_cb_args2", 
	"wff_cb_args3", "wff_cb_args4", "tau_cb_args1", "tau_cb_arg", "__E_tau_cb_args1_68", "tau_cb_args2", "tau_cb_args3", "tau_collapse_positives_cb", "__E_tau_cb_69", "__E___E_tau_cb_69_70", 
	"tau_positives_upwards_cb", "__E_tau_cb_71", "bf_eq_cb", "__E_wff_cb_72", "bf_neq_cb", "__E_wff_cb_73", "wff_has_clashing_subformulas_cb", "__E_wff_cb_74", "wff_has_subformula_cb", "__E_wff_cb_75", 
	"wff_remove_existential_cb", "__E_wff_cb_76", "wff_remove_bexistential_cb", "__E_wff_cb_77", "wff_remove_buniversal_cb", "__E_wff_cb_78", "bf_is_zero_cb", "__E_bf_cb_79", "bf_is_one_cb", "__E_bf_cb_80", 
	"bf_has_subformula_cb", "__E_bf_cb_81", "bf_remove_funiversal_cb", "__E_bf_cb_82", "bf_remove_fexistential_cb", "__E_bf_cb_83", "bf_and_cb", "__E_bf_cb_84", "bf_or_cb", "__E_bf_cb_85", 
	"bf_xor_cb", "__E_bf_cb_86", "bf_neg_cb", "__E_bf_cb_87", "charvar", "__E_charvar_88", "__E_charvar_89", "__E_charvar_90", "io_var", "__E_variable_91", 
	"out", "in_var_name", "__E_in_92", "chars", "offset", "out_var_name", "__E_out_93", "__E_offsets_94", "__E_offsets_95", "num", 
	"shift", "__E_offset_96", "__E_shift_97", "__E___E_shift_97_98", "digits", "binding", "__E_binding_99", "type", "__E___E_binding_99_100", "source", 
	"named_binding", "src_c", "__E_source_101", "__E_source_102", "__E___E_source_102_103", "__E___E_source_102_104", "__E_chars_105", "__E_digits_106", "comment", "__E_comment_107", 
	"__E_comment_108", "__E_comment_109", "__E____110", "__E___111", "cli", "cli_command", "__E_cli_112", "__E_cli_113", "quit_cmd", "quit_sym", 
	"__E_cli_command_114", "version_cmd", "version_sym", "__E_cli_command_115", "clear_cmd", "clear_sym", "__E_cli_command_116", "help_cmd", "__E_cli_command_117", "help_sym", 
	"__E___E_cli_command_117_118", "__E___E_cli_command_117_119", "help_arg", "file_cmd", "__E_cli_command_120", "file_sym", "q_string", "valid_cmd", "__E_cli_command_121", "valid_sym", 
	"sat_cmd", "__E_cli_command_122", "sat_sym", "unsat_cmd", "__E_cli_command_123", "unsat_sym", "solve_cmd", "__E_cli_command_124", "solve_sym", "solve_cmd_arg", 
	"wff_cmd_arg", "execute_cmd", "__E_cli_command_125", "execute_sym", "__E___E_cli_command_125_126", "execute_cmd_arg", "__E___E_cli_command_125_127", "normalize_cmd", "__E_cli_command_128", "normalize_sym", 
	"__E___E_cli_command_128_129", "normalize_cmd_arg", "wff_inst_cmd", "__E_cli_command_130", "inst_sym", "__E___E_cli_command_130_131", "wff_cmd_out_arg", "wff_subst_cmd", "__E_cli_command_132", "subst_sym", 
	"__E___E_cli_command_132_133", "bf_inst_cmd", "__E_cli_command_134", "bf_cmd_arg", "bf_cmd_out_arg", "bf_subst_cmd", "__E_cli_command_135", "dnf_cmd", "__E_cli_command_136", "dnf_sym", 
	"nf_cmd_arg", "cnf_cmd", "__E_cli_command_137", "cnf_sym", "anf_cmd", "__E_cli_command_138", "anf_sym", "nnf_cmd", "__E_cli_command_139", "nnf_sym", 
	"pnf_cmd", "__E_cli_command_140", "pnf_sym", "mnf_cmd", "__E_cli_command_141", "mnf_sym", "snf_cmd", "__E_cli_command_142", "snf_sym", "onf_cmd", 
	"__E_cli_command_143", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_144", "def_sym", "def_rr_cmd", "__E_cli_command_145", "def_del_cmd", "__E_cli_command_146", 
	"del_sym", "__E___E_cli_command_146_147", "memory", "def_clear_cmd", "__E_cli_command_148", "def_print_cmd", "__E_cli_command_149", "memory_list_cmd", "__E_cli_command_150", "memory_sym", 
	"__E___E_cli_command_150_151", "memory_del_cmd", "__E_cli_command_152", "memory_clear_cmd", "__E_cli_command_153", "memory_print_cmd", "__E_cli_command_154", "memory_store_cmd", "__E_cli_command_155", "memory_store_cmd_arg", 
	"__E___E_cli_command_155_156", "get_cmd", "__E_cli_command_157", "get_sym", "__E___E_cli_command_157_158", "option", "set_cmd", "__E_cli_command_159", "set_sym", "__E___E_cli_command_159_160", 
	"option_value", "toggle_cmd", "__E_cli_command_161", "toggle_sym", "bool_option", "bf_selection", "wff_selection", "abs_memory_sym", "rel_memory_sym", "selection_sym", 
	"examples_sym", "__E_help_arg_162", "__E___E_help_arg_162_163", "rel_memory", "__E_memory_164", "__E___E_memory_164_165", "memory_id", "abs_memory", "__E_memory_166", "__E___E_memory_166_167", 
	"bf_var_selection", "wff_var_selection", "enum_option", "severity_opt", "__E_option_168", "status_opt", "__E_bool_option_169", "colors_opt", "__E_bool_option_170", "debug_repl_opt", 
	"__E_bool_option_171", "option_value_true", "option_value_false", "severity", "error_sym", "__E_severity_172", "info_sym", "__E_severity_173", "debug_sym", "__E_severity_174", 
	"trace_sym", "__E_severity_175", "__N_0", "__N_1", "__N_2", "__N_3", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', ';', '.', ':', '=', '<', '{', '}', '(', 
	')', '|', '&', '!', '?', 'b', 'o', 'l', '_', 'a', 
	'e', 'x', '-', '>', '^', 'T', 'F', 'S', 'f', '+', 
	'\'', '1', '0', ',', 't', 'u', 'c', 'p', 's', 'i', 
	'v', 'w', 'r', 'd', 'q', 'n', 'h', 'g', 'm', 'z', 
	'[', ']', '$', '\t', '\n', '\r', '#', 'y', '%', 
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
			16, 72
		},
		.to_trim_children_terminals = {
			11, 12, 18, 22, 26, 27, 28, 30, 34, 44,
			46, 52, 53, 54, 68, 74, 76, 78, 80, 82,
			87, 88, 89, 91, 93, 95, 97, 99, 102, 104,
			107, 111, 113, 115, 117, 119, 121, 123, 124, 125,
			127, 129, 131, 133, 135, 137, 139, 142, 144, 146,
			148, 152, 154, 156, 159, 161, 164, 165, 187, 190,
			192, 194, 196, 198, 200, 202, 204, 206, 208, 210,
			212, 214, 216, 218, 220, 222, 240, 264, 269, 272,
			275, 279, 285, 289, 292, 295, 298, 303, 309, 314,
			319, 329, 333, 336, 339, 342, 345, 348, 351, 355,
			360, 369, 383, 386, 388, 393, 397, 398, 399, 400,
			413, 415, 417, 419, 421, 422, 424, 426, 428, 430
		},
		.trim_terminals = false,
		.to_inline = {
			{ 20, 80, 20 },
			{ 32, 95, 32 },
			{ 41, 142, 41 },
			{ 59 },
			{ 63 },
			{ 67 },
			{ 101 },
			{ 106 },
			{ 172 },
			{ 174 },
			{ 175 },
			{ 176 },
			{ 177 },
			{ 179 },
			{ 180 },
			{ 181 },
			{ 182 },
			{ 185 },
			{ 186 },
			{ 233 },
			{ 251 },
			{ 300 },
			{ 316 },
			{ 323 },
			{ 324 }
		},
		.inline_char_classes = true
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
//G0:   __E_start_0(10)      => inputs(11).
	p(NT(10), (NT(11)));
//G1:   __E_start_0(10)      => builder(12).
	p(NT(10), (NT(12)));
//G2:   __E_start_0(10)      => library(13).
	p(NT(10), (NT(13)));
//G3:   __E_start_0(10)      => nso_rr(14).
	p(NT(10), (NT(14)));
//G4:   __E_start_0(10)      => gssotc_rr(15).
	p(NT(10), (NT(15)));
//G5:   start(9)             => __E_start_0(10) _(16).
	p(NT(9), (NT(10)+NT(16)));
//G6:   __E_gssotc_rr_1(19)  => _(16) tau(20) _(16) ';'.
	p(NT(19), (NT(16)+NT(20)+NT(16)+T(1)));
//G7:   gssotc_main(18)      => __E_gssotc_rr_1(19).
	p(NT(18), (NT(19)));
//G8:   gssotc_rr(15)        => gssotc_rec_relations(17) gssotc_main(18).
	p(NT(15), (NT(17)+NT(18)));
//G9:   __E_gssotc_rec_relations_2(21) => _(16) gssotc_rec_relation(22).
	p(NT(21), (NT(16)+NT(22)));
//G10:  __E_gssotc_rec_relations_3(23) => null.
	p(NT(23), (nul));
//G11:  __E_gssotc_rec_relations_3(23) => __E_gssotc_rec_relations_2(21) __E_gssotc_rec_relations_3(23).
	p(NT(23), (NT(21)+NT(23)));
//G12:  gssotc_rec_relations(17) => __E_gssotc_rec_relations_3(23).
	p(NT(17), (NT(23)));
//G13:  __E_gssotc_rec_relation_4(25) => bf_rec_relation(26).
	p(NT(25), (NT(26)));
//G14:  __E_gssotc_rec_relation_4(25) => wff_rec_relation(27).
	p(NT(25), (NT(27)));
//G15:  __E_gssotc_rec_relation_4(25) => tau_rec_relation(28).
	p(NT(25), (NT(28)));
//G16:  gssotc_rec_relation_form(24) => __E_gssotc_rec_relation_4(25).
	p(NT(24), (NT(25)));
//G17:  gssotc_rec_relation(22) => gssotc_rec_relation_form(24) _(16) '.'.
	p(NT(22), (NT(24)+NT(16)+T(2)));
//G18:  __E_nso_rr_5(31)     => _(16) wff(32) _(16) '.'.
	p(NT(31), (NT(16)+NT(32)+NT(16)+T(2)));
//G19:  nso_main(30)         => __E_nso_rr_5(31).
	p(NT(30), (NT(31)));
//G20:  nso_rr(14)           => nso_rec_relations(29) nso_main(30).
	p(NT(14), (NT(29)+NT(30)));
//G21:  __E_nso_rec_relations_6(33) => _(16) nso_rec_relation(34).
	p(NT(33), (NT(16)+NT(34)));
//G22:  __E_nso_rec_relations_7(35) => null.
	p(NT(35), (nul));
//G23:  __E_nso_rec_relations_7(35) => __E_nso_rec_relations_6(33) __E_nso_rec_relations_7(35).
	p(NT(35), (NT(33)+NT(35)));
//G24:  nso_rec_relations(29) => __E_nso_rec_relations_7(35).
	p(NT(29), (NT(35)));
//G25:  __E_nso_rec_relation_8(37) => bf_rec_relation(26).
	p(NT(37), (NT(26)));
//G26:  __E_nso_rec_relation_8(37) => wff_rec_relation(27).
	p(NT(37), (NT(27)));
//G27:  nso_rec_relation_form(36) => __E_nso_rec_relation_8(37).
	p(NT(36), (NT(37)));
//G28:  nso_rec_relation(34) => nso_rec_relation_form(36) _(16) '.'.
	p(NT(34), (NT(36)+NT(16)+T(2)));
//G29:  tau_rec_relation(28) => tau_ref(38) _(16) ':' ':' ':' '=' _(16) tau(20).
	p(NT(28), (NT(38)+NT(16)+T(3)+T(3)+T(3)+T(4)+NT(16)+NT(20)));
//G30:  wff_rec_relation(27) => wff_ref(39) _(16) ':' ':' '=' _(16) wff(32).
	p(NT(27), (NT(39)+NT(16)+T(3)+T(3)+T(4)+NT(16)+NT(32)));
//G31:  bf_rec_relation(26)  => bf_ref(40) _(16) ':' '=' _(16) bf(41).
	p(NT(26), (NT(40)+NT(16)+T(3)+T(4)+NT(16)+NT(41)));
//G32:  __E_inputs_10(43)    => _(16) input(44).
	p(NT(43), (NT(16)+NT(44)));
//G33:  __E_inputs_9(42)     => __E_inputs_10(43).
	p(NT(42), (NT(43)));
//G34:  __E_inputs_9(42)     => __E_inputs_10(43) __E_inputs_9(42).
	p(NT(42), (NT(43)+NT(42)));
//G35:  inputs(11)           => _(16) '<' __E_inputs_9(42) _(16) '.'.
	p(NT(11), (NT(16)+T(5)+NT(42)+NT(16)+T(2)));
//G36:  input(44)            => in(45) _(16) ':' _(16) '{' _(16) source_binding(46) _(16) '}'.
	p(NT(44), (NT(45)+NT(16)+T(3)+NT(16)+T(6)+NT(16)+NT(46)+NT(16)+T(7)));
//G37:  __E___E___E_library_11_12_13(51) => wff_rule(52).
	p(NT(51), (NT(52)));
//G38:  __E___E___E_library_11_12_13(51) => bf_rule(53).
	p(NT(51), (NT(53)));
//G39:  __E___E___E_library_11_12_13(51) => tau_rule(54).
	p(NT(51), (NT(54)));
//G40:  rule(50)             => __E___E___E_library_11_12_13(51).
	p(NT(50), (NT(51)));
//G41:  __E___E_library_11_12(49) => _(16) rule(50).
	p(NT(49), (NT(16)+NT(50)));
//G42:  __E___E_library_11_14(55) => null.
	p(NT(55), (nul));
//G43:  __E___E_library_11_14(55) => __E___E_library_11_12(49) __E___E_library_11_14(55).
	p(NT(55), (NT(49)+NT(55)));
//G44:  __E_library_11(48)   => __E___E_library_11_14(55).
	p(NT(48), (NT(55)));
//G45:  rules(47)            => __E_library_11(48).
	p(NT(47), (NT(48)));
//G46:  library(13)          => rules(47).
	p(NT(13), (NT(47)));
//G47:  tau_matcher(56)      => tau(20).
	p(NT(56), (NT(20)));
//G48:  __E_tau_rule_15(58)  => tau(20).
	p(NT(58), (NT(20)));
//G49:  __E_tau_rule_15(58)  => tau_cb(59).
	p(NT(58), (NT(59)));
//G50:  tau_body(57)         => __E_tau_rule_15(58).
	p(NT(57), (NT(58)));
//G51:  tau_rule(54)         => tau_matcher(56) _(16) ':' ':' ':' '=' _(16) tau_body(57) _(16) '.'.
	p(NT(54), (NT(56)+NT(16)+T(3)+T(3)+T(3)+T(4)+NT(16)+NT(57)+NT(16)+T(2)));
//G52:  wff_matcher(60)      => wff(32).
	p(NT(60), (NT(32)));
//G53:  __E_wff_rule_16(62)  => wff(32).
	p(NT(62), (NT(32)));
//G54:  __E_wff_rule_16(62)  => wff_cb(63).
	p(NT(62), (NT(63)));
//G55:  wff_body(61)         => __E_wff_rule_16(62).
	p(NT(61), (NT(62)));
//G56:  wff_rule(52)         => wff_matcher(60) _(16) ':' ':' '=' _(16) wff_body(61) _(16) '.'.
	p(NT(52), (NT(60)+NT(16)+T(3)+T(3)+T(4)+NT(16)+NT(61)+NT(16)+T(2)));
//G57:  bf_matcher(64)       => bf(41).
	p(NT(64), (NT(41)));
//G58:  __E_bf_rule_17(66)   => bf(41).
	p(NT(66), (NT(41)));
//G59:  __E_bf_rule_17(66)   => bf_cb(67).
	p(NT(66), (NT(67)));
//G60:  bf_body(65)          => __E_bf_rule_17(66).
	p(NT(65), (NT(66)));
//G61:  bf_rule(53)          => bf_matcher(64) _(16) ':' '=' _(16) bf_body(65) _(16) '.'.
	p(NT(53), (NT(64)+NT(16)+T(3)+T(4)+NT(16)+NT(65)+NT(16)+T(2)));
//G62:  builder(12)          => _(16) builder_head(68) _(16) builder_body(69) _(16) '.'.
	p(NT(12), (NT(16)+NT(68)+NT(16)+NT(69)+NT(16)+T(2)));
//G63:  __E_builder_head_18(71) => __(72) capture(70).
	p(NT(71), (NT(72)+NT(70)));
//G64:  __E_builder_head_19(73) => null.
	p(NT(73), (nul));
//G65:  __E_builder_head_19(73) => __E_builder_head_18(71) __E_builder_head_19(73).
	p(NT(73), (NT(71)+NT(73)));
//G66:  builder_head(68)     => '(' _(16) capture(70) __E_builder_head_19(73) _(16) ')'.
	p(NT(68), (T(8)+NT(16)+NT(70)+NT(73)+NT(16)+T(9)));
//G67:  __E_builder_body_20(75) => '=' ':' _(16) bf(41).
	p(NT(75), (T(4)+T(3)+NT(16)+NT(41)));
//G68:  bf_builder_body(74)  => __E_builder_body_20(75).
	p(NT(74), (NT(75)));
//G69:  builder_body(69)     => bf_builder_body(74).
	p(NT(69), (NT(74)));
//G70:  __E_builder_body_21(77) => '=' ':' ':' _(16) wff(32).
	p(NT(77), (T(4)+T(3)+T(3)+NT(16)+NT(32)));
//G71:  wff_builder_body(76) => __E_builder_body_21(77).
	p(NT(76), (NT(77)));
//G72:  builder_body(69)     => wff_builder_body(76).
	p(NT(69), (NT(76)));
//G73:  __E_builder_body_22(79) => '=' ':' ':' ':' _(16) tau(20).
	p(NT(79), (T(4)+T(3)+T(3)+T(3)+NT(16)+NT(20)));
//G74:  tau_builder_body(78) => __E_builder_body_22(79).
	p(NT(78), (NT(79)));
//G75:  builder_body(69)     => tau_builder_body(78).
	p(NT(69), (NT(78)));
//G76:  __E_tau_23(81)       => '(' _(16) tau(20) _(16) ')'.
	p(NT(81), (T(8)+NT(16)+NT(20)+NT(16)+T(9)));
//G77:  tau_parenthesis(80)  => __E_tau_23(81).
	p(NT(80), (NT(81)));
//G78:  tau(20)              => tau_parenthesis(80).
	p(NT(20), (NT(80)));
//G79:  __E_tau_24(83)       => '{' _(16) wff(32) _(16) '}'.
	p(NT(83), (T(6)+NT(16)+NT(32)+NT(16)+T(7)));
//G80:  tau_wff(82)          => __E_tau_24(83).
	p(NT(82), (NT(83)));
//G81:  tau(20)              => tau_wff(82).
	p(NT(20), (NT(82)));
//G82:  __E___E_tau_25_26(86) => _(16) offsets(87).
	p(NT(86), (NT(16)+NT(87)));
//G83:  __E___E_tau_25_26(86) => null.
	p(NT(86), (nul));
//G84:  __E_tau_25(84)       => sym(85) __E___E_tau_25_26(86) _(16) ref_args(88).
	p(NT(84), (NT(85)+NT(86)+NT(16)+NT(88)));
//G85:  tau_ref(38)          => __E_tau_25(84).
	p(NT(38), (NT(84)));
//G86:  tau(20)              => tau_ref(38).
	p(NT(20), (NT(38)));
//G87:  __E_tau_27(90)       => tau(20) _(16) '|' '|' '|' _(16) tau(20).
	p(NT(90), (NT(20)+NT(16)+T(10)+T(10)+T(10)+NT(16)+NT(20)));
//G88:  tau_or(89)           => __E_tau_27(90).
	p(NT(89), (NT(90)));
//G89:  tau(20)              => tau_or(89).
	p(NT(20), (NT(89)));
//G90:  __E_tau_28(92)       => tau(20) _(16) '&' '&' '&' _(16) tau(20).
	p(NT(92), (NT(20)+NT(16)+T(11)+T(11)+T(11)+NT(16)+NT(20)));
//G91:  tau_and(91)          => __E_tau_28(92).
	p(NT(91), (NT(92)));
//G92:  tau(20)              => tau_and(91).
	p(NT(20), (NT(91)));
//G93:  __E_tau_29(94)       => '!' '!' '!' _(16) tau(20).
	p(NT(94), (T(12)+T(12)+T(12)+NT(16)+NT(20)));
//G94:  tau_neg(93)          => __E_tau_29(94).
	p(NT(93), (NT(94)));
//G95:  tau(20)              => tau_neg(93).
	p(NT(20), (NT(93)));
//G96:  tau(20)              => capture(70).
	p(NT(20), (NT(70)));
//G97:  __E_wff_30(96)       => '(' _(16) wff(32) _(16) ')'.
	p(NT(96), (T(8)+NT(16)+NT(32)+NT(16)+T(9)));
//G98:  wff_parenthesis(95)  => __E_wff_30(96).
	p(NT(95), (NT(96)));
//G99:  wff(32)              => wff_parenthesis(95).
	p(NT(32), (NT(95)));
//G100: __E_wff_31(98)       => wff(32) _(16) '?' _(16) wff(32) _(16) ':' _(16) wff(32).
	p(NT(98), (NT(32)+NT(16)+T(13)+NT(16)+NT(32)+NT(16)+T(3)+NT(16)+NT(32)));
//G101: wff_conditional(97)  => __E_wff_31(98).
	p(NT(97), (NT(98)));
//G102: wff(32)              => wff_conditional(97).
	p(NT(32), (NT(97)));
//G103: __E_wff_32(100)      => 'b' 'o' 'o' 'l' '_' 'a' 'l' 'l' __(72) q_bool_var(101) __(72) wff(32).
	p(NT(100), (T(14)+T(15)+T(15)+T(16)+T(17)+T(18)+T(16)+T(16)+NT(72)+NT(101)+NT(72)+NT(32)));
//G104: wff_ball(99)         => __E_wff_32(100).
	p(NT(99), (NT(100)));
//G105: wff(32)              => wff_ball(99).
	p(NT(32), (NT(99)));
//G106: __E_wff_33(103)      => 'b' 'o' 'o' 'l' '_' 'e' 'x' __(72) q_bool_var(101) __(72) wff(32).
	p(NT(103), (T(14)+T(15)+T(15)+T(16)+T(17)+T(19)+T(20)+NT(72)+NT(101)+NT(72)+NT(32)));
//G107: wff_bex(102)         => __E_wff_33(103).
	p(NT(102), (NT(103)));
//G108: wff(32)              => wff_bex(102).
	p(NT(32), (NT(102)));
//G109: __E_wff_34(105)      => 'a' 'l' 'l' __(72) q_var(106) __(72) wff(32).
	p(NT(105), (T(18)+T(16)+T(16)+NT(72)+NT(106)+NT(72)+NT(32)));
//G110: wff_all(104)         => __E_wff_34(105).
	p(NT(104), (NT(105)));
//G111: wff(32)              => wff_all(104).
	p(NT(32), (NT(104)));
//G112: __E_wff_35(108)      => 'e' 'x' __(72) q_var(106) __(72) wff(32).
	p(NT(108), (T(19)+T(20)+NT(72)+NT(106)+NT(72)+NT(32)));
//G113: wff_ex(107)          => __E_wff_35(108).
	p(NT(107), (NT(108)));
//G114: wff(32)              => wff_ex(107).
	p(NT(32), (NT(107)));
//G115: __E___E_wff_36_37(110) => _(16) offsets(87).
	p(NT(110), (NT(16)+NT(87)));
//G116: __E___E_wff_36_37(110) => null.
	p(NT(110), (nul));
//G117: __E_wff_36(109)      => sym(85) __E___E_wff_36_37(110) _(16) ref_args(88).
	p(NT(109), (NT(85)+NT(110)+NT(16)+NT(88)));
//G118: wff_ref(39)          => __E_wff_36(109).
	p(NT(39), (NT(109)));
//G119: wff(32)              => wff_ref(39).
	p(NT(32), (NT(39)));
//G120: __E_wff_38(112)      => wff(32) _(16) '-' '>' _(16) wff(32).
	p(NT(112), (NT(32)+NT(16)+T(21)+T(22)+NT(16)+NT(32)));
//G121: wff_imply(111)       => __E_wff_38(112).
	p(NT(111), (NT(112)));
//G122: wff(32)              => wff_imply(111).
	p(NT(32), (NT(111)));
//G123: __E_wff_39(114)      => wff(32) _(16) '<' '-' '>' _(16) wff(32).
	p(NT(114), (NT(32)+NT(16)+T(5)+T(21)+T(22)+NT(16)+NT(32)));
//G124: wff_equiv(113)       => __E_wff_39(114).
	p(NT(113), (NT(114)));
//G125: wff(32)              => wff_equiv(113).
	p(NT(32), (NT(113)));
//G126: __E_wff_40(116)      => wff(32) _(16) '|' '|' _(16) wff(32).
	p(NT(116), (NT(32)+NT(16)+T(10)+T(10)+NT(16)+NT(32)));
//G127: wff_or(115)          => __E_wff_40(116).
	p(NT(115), (NT(116)));
//G128: wff(32)              => wff_or(115).
	p(NT(32), (NT(115)));
//G129: __E_wff_41(118)      => wff(32) _(16) '^' _(16) wff(32).
	p(NT(118), (NT(32)+NT(16)+T(23)+NT(16)+NT(32)));
//G130: wff_xor(117)         => __E_wff_41(118).
	p(NT(117), (NT(118)));
//G131: wff(32)              => wff_xor(117).
	p(NT(32), (NT(117)));
//G132: __E_wff_42(120)      => wff(32) _(16) '&' '&' _(16) wff(32).
	p(NT(120), (NT(32)+NT(16)+T(11)+T(11)+NT(16)+NT(32)));
//G133: wff_and(119)         => __E_wff_42(120).
	p(NT(119), (NT(120)));
//G134: wff(32)              => wff_and(119).
	p(NT(32), (NT(119)));
//G135: __E_wff_43(122)      => '!' _(16) wff(32).
	p(NT(122), (T(12)+NT(16)+NT(32)));
//G136: wff_neg(121)         => __E_wff_43(122).
	p(NT(121), (NT(122)));
//G137: wff(32)              => wff_neg(121).
	p(NT(32), (NT(121)));
//G138: wff_t(123)           => 'T'.
	p(NT(123), (T(24)));
//G139: wff(32)              => wff_t(123).
	p(NT(32), (NT(123)));
//G140: wff_f(124)           => 'F'.
	p(NT(124), (T(25)));
//G141: wff(32)              => wff_f(124).
	p(NT(32), (NT(124)));
//G142: __E_wff_44(126)      => bf(41) _(16) '<' '=' _(16) q_var(106) _(16) '<' '=' _(16) bf(41).
	p(NT(126), (NT(41)+NT(16)+T(5)+T(4)+NT(16)+NT(106)+NT(16)+T(5)+T(4)+NT(16)+NT(41)));
//G143: bf_interval(125)     => __E_wff_44(126).
	p(NT(125), (NT(126)));
//G144: wff(32)              => bf_interval(125).
	p(NT(32), (NT(125)));
//G145: __E_wff_45(128)      => bf(41) _(16) '!' '=' _(16) bf(41).
	p(NT(128), (NT(41)+NT(16)+T(12)+T(4)+NT(16)+NT(41)));
//G146: bf_neq(127)          => __E_wff_45(128).
	p(NT(127), (NT(128)));
//G147: wff(32)              => bf_neq(127).
	p(NT(32), (NT(127)));
//G148: __E_wff_46(130)      => bf(41) _(16) '=' _(16) bf(41).
	p(NT(130), (NT(41)+NT(16)+T(4)+NT(16)+NT(41)));
//G149: bf_eq(129)           => __E_wff_46(130).
	p(NT(129), (NT(130)));
//G150: wff(32)              => bf_eq(129).
	p(NT(32), (NT(129)));
//G151: __E_wff_47(132)      => bf(41) _(16) '!' '<' '=' _(16) q_var(106).
	p(NT(132), (NT(41)+NT(16)+T(12)+T(5)+T(4)+NT(16)+NT(106)));
//G152: bf_nleq_lower(131)   => __E_wff_47(132).
	p(NT(131), (NT(132)));
//G153: wff(32)              => bf_nleq_lower(131).
	p(NT(32), (NT(131)));
//G154: __E_wff_48(134)      => q_var(106) _(16) '!' '<' '=' _(16) bf(41).
	p(NT(134), (NT(106)+NT(16)+T(12)+T(5)+T(4)+NT(16)+NT(41)));
//G155: bf_nleq_upper(133)   => __E_wff_48(134).
	p(NT(133), (NT(134)));
//G156: wff(32)              => bf_nleq_upper(133).
	p(NT(32), (NT(133)));
//G157: __E_wff_49(136)      => bf(41) _(16) '>' _(16) bf(41).
	p(NT(136), (NT(41)+NT(16)+T(22)+NT(16)+NT(41)));
//G158: bf_greater(135)      => __E_wff_49(136).
	p(NT(135), (NT(136)));
//G159: wff(32)              => bf_greater(135).
	p(NT(32), (NT(135)));
//G160: __E_wff_50(138)      => bf(41) _(16) '<' '=' _(16) bf(41).
	p(NT(138), (NT(41)+NT(16)+T(5)+T(4)+NT(16)+NT(41)));
//G161: bf_less_equal(137)   => __E_wff_50(138).
	p(NT(137), (NT(138)));
//G162: wff(32)              => bf_less_equal(137).
	p(NT(32), (NT(137)));
//G163: __E_wff_51(140)      => bf(41) _(16) '<' _(16) bf(41).
	p(NT(140), (NT(41)+NT(16)+T(5)+NT(16)+NT(41)));
//G164: bf_less(139)         => __E_wff_51(140).
	p(NT(139), (NT(140)));
//G165: wff(32)              => bf_less(139).
	p(NT(32), (NT(139)));
//G166: wff(32)              => capture(70).
	p(NT(32), (NT(70)));
//G167: wff(32)              => bool_variable(141).
	p(NT(32), (NT(141)));
//G168: __E_bf_52(143)       => '(' _(16) bf(41) _(16) ')'.
	p(NT(143), (T(8)+NT(16)+NT(41)+NT(16)+T(9)));
//G169: bf_parenthesis(142)  => __E_bf_52(143).
	p(NT(142), (NT(143)));
//G170: bf(41)               => bf_parenthesis(142).
	p(NT(41), (NT(142)));
//G171: __E_bf_53(145)       => 'S' _(16) '(' _(16) bf(41) _(16) ')'.
	p(NT(145), (T(26)+NT(16)+T(8)+NT(16)+NT(41)+NT(16)+T(9)));
//G172: bf_splitter(144)     => __E_bf_53(145).
	p(NT(144), (NT(145)));
//G173: bf(41)               => bf_splitter(144).
	p(NT(41), (NT(144)));
//G174: __E_bf_54(147)       => 'f' 'a' 'l' 'l' __(72) q_var(106) __(72) bf(41).
	p(NT(147), (T(27)+T(18)+T(16)+T(16)+NT(72)+NT(106)+NT(72)+NT(41)));
//G175: bf_all(146)          => __E_bf_54(147).
	p(NT(146), (NT(147)));
//G176: bf(41)               => bf_all(146).
	p(NT(41), (NT(146)));
//G177: __E_bf_55(149)       => 'f' 'e' 'x' __(72) q_var(106) __(72) bf(41).
	p(NT(149), (T(27)+T(19)+T(20)+NT(72)+NT(106)+NT(72)+NT(41)));
//G178: bf_ex(148)           => __E_bf_55(149).
	p(NT(148), (NT(149)));
//G179: bf(41)               => bf_ex(148).
	p(NT(41), (NT(148)));
//G180: __E___E_bf_56_57(151) => _(16) offsets(87).
	p(NT(151), (NT(16)+NT(87)));
//G181: __E___E_bf_56_57(151) => null.
	p(NT(151), (nul));
//G182: __E_bf_56(150)       => sym(85) __E___E_bf_56_57(151) _(16) ref_args(88).
	p(NT(150), (NT(85)+NT(151)+NT(16)+NT(88)));
//G183: bf_ref(40)           => __E_bf_56(150).
	p(NT(40), (NT(150)));
//G184: bf(41)               => bf_ref(40).
	p(NT(41), (NT(40)));
//G185: __E_bf_58(153)       => bf(41) _(16) '|' _(16) bf(41).
	p(NT(153), (NT(41)+NT(16)+T(10)+NT(16)+NT(41)));
//G186: bf_or(152)           => __E_bf_58(153).
	p(NT(152), (NT(153)));
//G187: bf(41)               => bf_or(152).
	p(NT(41), (NT(152)));
//G188: __E_bf_59(155)       => bf(41) _(16) '+' _(16) bf(41).
	p(NT(155), (NT(41)+NT(16)+T(28)+NT(16)+NT(41)));
//G189: bf_xor(154)          => __E_bf_59(155).
	p(NT(154), (NT(155)));
//G190: bf(41)               => bf_xor(154).
	p(NT(41), (NT(154)));
//G191: __E___E_bf_60_61(158) => _(16).
	p(NT(158), (NT(16)));
//G192: __E___E_bf_60_61(158) => _(16) '&' _(16).
	p(NT(158), (NT(16)+T(11)+NT(16)));
//G193: __E_bf_60(157)       => bf(41) __E___E_bf_60_61(158) bf(41).
	p(NT(157), (NT(41)+NT(158)+NT(41)));
//G194: bf_and(156)          => __E_bf_60(157).
	p(NT(156), (NT(157)));
//G195: bf(41)               => bf_and(156).
	p(NT(41), (NT(156)));
//G196: __E_bf_62(160)       => bf(41) _(16) '\''.
	p(NT(160), (NT(41)+NT(16)+T(29)));
//G197: bf_neg(159)          => __E_bf_62(160).
	p(NT(159), (NT(160)));
//G198: bf(41)               => bf_neg(159).
	p(NT(41), (NT(159)));
//G199: __E_bf_63(162)       => '{' _(16) constant(163) _(16) '}'.
	p(NT(162), (T(6)+NT(16)+NT(163)+NT(16)+T(7)));
//G200: bf_constant(161)     => __E_bf_63(162).
	p(NT(161), (NT(162)));
//G201: bf(41)               => bf_constant(161).
	p(NT(41), (NT(161)));
//G202: bf_t(164)            => '1'.
	p(NT(164), (T(30)));
//G203: bf(41)               => bf_t(164).
	p(NT(41), (NT(164)));
//G204: bf_f(165)            => '0'.
	p(NT(165), (T(31)));
//G205: bf(41)               => bf_f(165).
	p(NT(41), (NT(165)));
//G206: bf(41)               => capture(70).
	p(NT(41), (NT(70)));
//G207: bf(41)               => variable(166).
	p(NT(41), (NT(166)));
//G208: ref_arg(169)         => bf(41).
	p(NT(169), (NT(41)));
//G209: __E___E_ref_args_64_65(168) => _(16) ref_arg(169).
	p(NT(168), (NT(16)+NT(169)));
//G210: __E___E_ref_args_64_66(170) => _(16) ',' _(16) ref_arg(169).
	p(NT(170), (NT(16)+T(32)+NT(16)+NT(169)));
//G211: __E___E_ref_args_64_67(171) => null.
	p(NT(171), (nul));
//G212: __E___E_ref_args_64_67(171) => __E___E_ref_args_64_66(170) __E___E_ref_args_64_67(171).
	p(NT(171), (NT(170)+NT(171)));
//G213: __E_ref_args_64(167) => __E___E_ref_args_64_65(168) __E___E_ref_args_64_67(171).
	p(NT(167), (NT(168)+NT(171)));
//G214: __E_ref_args_64(167) => null.
	p(NT(167), (nul));
//G215: ref_args(88)         => '(' __E_ref_args_64(167) _(16) ')'.
	p(NT(88), (T(8)+NT(167)+NT(16)+T(9)));
//G216: bf_cb_arg(173)       => bf(41).
	p(NT(173), (NT(41)));
//G217: bf_cb_args1(172)     => __(72) bf_cb_arg(173).
	p(NT(172), (NT(72)+NT(173)));
//G218: bf_cb_args2(174)     => bf_cb_args1(172) bf_cb_args1(172).
	p(NT(174), (NT(172)+NT(172)));
//G219: bf_cb_args3(175)     => bf_cb_args2(174) bf_cb_args1(172).
	p(NT(175), (NT(174)+NT(172)));
//G220: bf_cb_args4(176)     => bf_cb_args3(175) bf_cb_args1(172).
	p(NT(176), (NT(175)+NT(172)));
//G221: wff_cb_arg(178)      => wff(32).
	p(NT(178), (NT(32)));
//G222: wff_cb_args1(177)    => __(72) wff_cb_arg(178).
	p(NT(177), (NT(72)+NT(178)));
//G223: wff_cb_args2(179)    => wff_cb_args1(177) wff_cb_args1(177).
	p(NT(179), (NT(177)+NT(177)));
//G224: wff_cb_args3(180)    => wff_cb_args2(179) wff_cb_args1(177).
	p(NT(180), (NT(179)+NT(177)));
//G225: wff_cb_args4(181)    => wff_cb_args3(180) wff_cb_args1(177).
	p(NT(181), (NT(180)+NT(177)));
//G226: __E_tau_cb_args1_68(184) => capture(70).
	p(NT(184), (NT(70)));
//G227: __E_tau_cb_args1_68(184) => tau(20).
	p(NT(184), (NT(20)));
//G228: tau_cb_arg(183)      => __E_tau_cb_args1_68(184).
	p(NT(183), (NT(184)));
//G229: tau_cb_args1(182)    => __(72) tau_cb_arg(183).
	p(NT(182), (NT(72)+NT(183)));
//G230: tau_cb_args2(185)    => tau_cb_args1(182) tau_cb_args1(182).
	p(NT(185), (NT(182)+NT(182)));
//G231: tau_cb_args3(186)    => tau_cb_args2(185) tau_cb_args1(182).
	p(NT(186), (NT(185)+NT(182)));
//G232: __E___E_tau_cb_69_70(189) => tau_cb_args3(186).
	p(NT(189), (NT(186)));
//G233: __E___E_tau_cb_69_70(189) => tau_cb_args2(185).
	p(NT(189), (NT(185)));
//G234: __E_tau_cb_69(188)   => 't' 'a' 'u' '_' 'c' 'o' 'l' 'l' 'a' 'p' 's' 'e' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'c' 'b' __E___E_tau_cb_69_70(189).
	p(NT(188), (T(33)+T(18)+T(34)+T(17)+T(35)+T(15)+T(16)+T(16)+T(18)+T(36)+T(37)+T(19)+T(17)+T(36)+T(15)+T(37)+T(38)+T(33)+T(38)+T(39)+T(19)+T(37)+T(17)+T(35)+T(14)+NT(189)));
//G235: tau_collapse_positives_cb(187) => __E_tau_cb_69(188).
	p(NT(187), (NT(188)));
//G236: tau_cb(59)           => tau_collapse_positives_cb(187).
	p(NT(59), (NT(187)));
//G237: __E_tau_cb_71(191)   => 't' 'a' 'u' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'u' 'p' 'w' 'a' 'r' 'd' 's' '_' 'c' 'b' tau_cb_args2(185).
	p(NT(191), (T(33)+T(18)+T(34)+T(17)+T(36)+T(15)+T(37)+T(38)+T(33)+T(38)+T(39)+T(19)+T(37)+T(17)+T(34)+T(36)+T(40)+T(18)+T(41)+T(42)+T(37)+T(17)+T(35)+T(14)+NT(185)));
//G238: tau_positives_upwards_cb(190) => __E_tau_cb_71(191).
	p(NT(190), (NT(191)));
//G239: tau_cb(59)           => tau_positives_upwards_cb(190).
	p(NT(59), (NT(190)));
//G240: __E_wff_cb_72(193)   => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b' bf_cb_args1(172) wff_cb_args2(179).
	p(NT(193), (T(14)+T(27)+T(17)+T(19)+T(43)+T(17)+T(35)+T(14)+NT(172)+NT(179)));
//G241: bf_eq_cb(192)        => __E_wff_cb_72(193).
	p(NT(192), (NT(193)));
//G242: wff_cb(63)           => bf_eq_cb(192).
	p(NT(63), (NT(192)));
//G243: __E_wff_cb_73(195)   => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b' bf_cb_args1(172) wff_cb_args2(179).
	p(NT(195), (T(14)+T(27)+T(17)+T(44)+T(19)+T(43)+T(17)+T(35)+T(14)+NT(172)+NT(179)));
//G244: bf_neq_cb(194)       => __E_wff_cb_73(195).
	p(NT(194), (NT(195)));
//G245: wff_cb(63)           => bf_neq_cb(194).
	p(NT(63), (NT(194)));
//G246: __E_wff_cb_74(197)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(179).
	p(NT(197), (T(40)+T(27)+T(27)+T(17)+T(45)+T(18)+T(37)+T(17)+T(35)+T(16)+T(18)+T(37)+T(45)+T(38)+T(44)+T(46)+T(17)+T(37)+T(34)+T(14)+T(27)+T(15)+T(41)+T(47)+T(34)+T(16)+T(18)+T(37)+T(17)+T(35)+T(14)+NT(179)));
//G247: wff_has_clashing_subformulas_cb(196) => __E_wff_cb_74(197).
	p(NT(196), (NT(197)));
//G248: wff_cb(63)           => wff_has_clashing_subformulas_cb(196).
	p(NT(63), (NT(196)));
//G249: __E_wff_cb_75(199)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(180).
	p(NT(199), (T(40)+T(27)+T(27)+T(17)+T(45)+T(18)+T(37)+T(17)+T(37)+T(34)+T(14)+T(27)+T(15)+T(41)+T(47)+T(34)+T(16)+T(18)+T(17)+T(35)+T(14)+NT(180)));
//G250: wff_has_subformula_cb(198) => __E_wff_cb_75(199).
	p(NT(198), (NT(199)));
//G251: wff_cb(63)           => wff_has_subformula_cb(198).
	p(NT(63), (NT(198)));
//G252: __E_wff_cb_76(201)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(179).
	p(NT(201), (T(40)+T(27)+T(27)+T(17)+T(41)+T(19)+T(47)+T(15)+T(39)+T(19)+T(17)+T(19)+T(20)+T(38)+T(37)+T(33)+T(19)+T(44)+T(33)+T(38)+T(18)+T(16)+T(17)+T(35)+T(14)+NT(179)));
//G253: wff_remove_existential_cb(200) => __E_wff_cb_76(201).
	p(NT(200), (NT(201)));
//G254: wff_cb(63)           => wff_remove_existential_cb(200).
	p(NT(63), (NT(200)));
//G255: __E_wff_cb_77(203)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args4(181).
	p(NT(203), (T(40)+T(27)+T(27)+T(17)+T(41)+T(19)+T(47)+T(15)+T(39)+T(19)+T(17)+T(14)+T(19)+T(20)+T(38)+T(37)+T(33)+T(19)+T(44)+T(33)+T(38)+T(18)+T(16)+T(17)+T(35)+T(14)+NT(181)));
//G256: wff_remove_bexistential_cb(202) => __E_wff_cb_77(203).
	p(NT(202), (NT(203)));
//G257: wff_cb(63)           => wff_remove_bexistential_cb(202).
	p(NT(63), (NT(202)));
//G258: __E_wff_cb_78(205)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' wff_cb_args4(181).
	p(NT(205), (T(40)+T(27)+T(27)+T(17)+T(41)+T(19)+T(47)+T(15)+T(39)+T(19)+T(17)+T(14)+T(34)+T(44)+T(38)+T(39)+T(19)+T(41)+T(37)+T(18)+T(16)+T(17)+T(35)+T(14)+NT(181)));
//G259: wff_remove_buniversal_cb(204) => __E_wff_cb_78(205).
	p(NT(204), (NT(205)));
//G260: wff_cb(63)           => wff_remove_buniversal_cb(204).
	p(NT(63), (NT(204)));
//G261: __E_bf_cb_79(207)    => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b' bf_cb_args2(174).
	p(NT(207), (T(14)+T(27)+T(17)+T(38)+T(37)+T(17)+T(48)+T(19)+T(41)+T(15)+T(17)+T(35)+T(14)+NT(174)));
//G262: bf_is_zero_cb(206)   => __E_bf_cb_79(207).
	p(NT(206), (NT(207)));
//G263: bf_cb(67)            => bf_is_zero_cb(206).
	p(NT(67), (NT(206)));
//G264: __E_bf_cb_80(209)    => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b' bf_cb_args2(174).
	p(NT(209), (T(14)+T(27)+T(17)+T(38)+T(37)+T(17)+T(15)+T(44)+T(19)+T(17)+T(35)+T(14)+NT(174)));
//G265: bf_is_one_cb(208)    => __E_bf_cb_80(209).
	p(NT(208), (NT(209)));
//G266: bf_cb(67)            => bf_is_one_cb(208).
	p(NT(67), (NT(208)));
//G267: __E_bf_cb_81(211)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(175).
	p(NT(211), (T(14)+T(27)+T(17)+T(45)+T(18)+T(37)+T(17)+T(37)+T(34)+T(14)+T(27)+T(15)+T(41)+T(47)+T(34)+T(16)+T(18)+T(17)+T(35)+T(14)+NT(175)));
//G268: bf_has_subformula_cb(210) => __E_bf_cb_81(211).
	p(NT(210), (NT(211)));
//G269: bf_cb(67)            => bf_has_subformula_cb(210).
	p(NT(67), (NT(210)));
//G270: __E_bf_cb_82(213)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(176).
	p(NT(213), (T(14)+T(27)+T(17)+T(41)+T(19)+T(47)+T(15)+T(39)+T(19)+T(17)+T(27)+T(34)+T(44)+T(38)+T(39)+T(19)+T(41)+T(37)+T(18)+T(16)+T(17)+T(35)+T(14)+NT(176)));
//G271: bf_remove_funiversal_cb(212) => __E_bf_cb_82(213).
	p(NT(212), (NT(213)));
//G272: bf_cb(67)            => bf_remove_funiversal_cb(212).
	p(NT(67), (NT(212)));
//G273: __E_bf_cb_83(215)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(176).
	p(NT(215), (T(14)+T(27)+T(17)+T(41)+T(19)+T(47)+T(15)+T(39)+T(19)+T(17)+T(27)+T(19)+T(20)+T(38)+T(37)+T(33)+T(19)+T(44)+T(33)+T(38)+T(18)+T(16)+T(17)+T(35)+T(14)+NT(176)));
//G274: bf_remove_fexistential_cb(214) => __E_bf_cb_83(215).
	p(NT(214), (NT(215)));
//G275: bf_cb(67)            => bf_remove_fexistential_cb(214).
	p(NT(67), (NT(214)));
//G276: __E_bf_cb_84(217)    => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b' bf_cb_args2(174).
	p(NT(217), (T(14)+T(27)+T(17)+T(18)+T(44)+T(42)+T(17)+T(35)+T(14)+NT(174)));
//G277: bf_and_cb(216)       => __E_bf_cb_84(217).
	p(NT(216), (NT(217)));
//G278: bf_cb(67)            => bf_and_cb(216).
	p(NT(67), (NT(216)));
//G279: __E_bf_cb_85(219)    => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b' bf_cb_args2(174).
	p(NT(219), (T(14)+T(27)+T(17)+T(15)+T(41)+T(17)+T(35)+T(14)+NT(174)));
//G280: bf_or_cb(218)        => __E_bf_cb_85(219).
	p(NT(218), (NT(219)));
//G281: bf_cb(67)            => bf_or_cb(218).
	p(NT(67), (NT(218)));
//G282: __E_bf_cb_86(221)    => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b' bf_cb_args2(174).
	p(NT(221), (T(14)+T(27)+T(17)+T(20)+T(15)+T(41)+T(17)+T(35)+T(14)+NT(174)));
//G283: bf_xor_cb(220)       => __E_bf_cb_86(221).
	p(NT(220), (NT(221)));
//G284: bf_cb(67)            => bf_xor_cb(220).
	p(NT(67), (NT(220)));
//G285: __E_bf_cb_87(223)    => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b' bf_cb_args1(172).
	p(NT(223), (T(14)+T(27)+T(17)+T(44)+T(19)+T(46)+T(17)+T(35)+T(14)+NT(172)));
//G286: bf_neg_cb(222)       => __E_bf_cb_87(223).
	p(NT(222), (NT(223)));
//G287: bf_cb(67)            => bf_neg_cb(222).
	p(NT(67), (NT(222)));
//G288: __N_0(432)           => 'F'.
	p(NT(432), (T(25)));
//G289: __N_1(433)           => 'T'.
	p(NT(433), (T(24)));
//G290: __E_charvar_88(225)  => ~( __N_0(432) ) & ~( __N_1(433) ) & alpha(5).	 # conjunctive
	p(NT(225), ~(NT(432)) & ~(NT(433)) & (NT(5)));
//G291: __E_charvar_89(226)  => digit(3).
	p(NT(226), (NT(3)));
//G292: __E_charvar_90(227)  => null.
	p(NT(227), (nul));
//G293: __E_charvar_90(227)  => __E_charvar_89(226) __E_charvar_90(227).
	p(NT(227), (NT(226)+NT(227)));
//G294: charvar(224)         => __E_charvar_88(225) __E_charvar_90(227).
	p(NT(224), (NT(225)+NT(227)));
//G295: bool_variable(141)   => '?' charvar(224).
	p(NT(141), (T(13)+NT(224)));
//G296: variable(166)        => charvar(224).
	p(NT(166), (NT(224)));
//G297: __E_variable_91(229) => in(45).
	p(NT(229), (NT(45)));
//G298: __E_variable_91(229) => out(230).
	p(NT(229), (NT(230)));
//G299: io_var(228)          => __E_variable_91(229).
	p(NT(228), (NT(229)));
//G300: variable(166)        => io_var(228).
	p(NT(166), (NT(228)));
//G301: __E_in_92(232)       => 'i' '_' chars(233).
	p(NT(232), (T(38)+T(17)+NT(233)));
//G302: in_var_name(231)     => __E_in_92(232).
	p(NT(231), (NT(232)));
//G303: in(45)               => in_var_name(231) '[' offset(234) ']'.
	p(NT(45), (NT(231)+T(49)+NT(234)+T(50)));
//G304: __E_out_93(236)      => 'o' '_' chars(233).
	p(NT(236), (T(15)+T(17)+NT(233)));
//G305: out_var_name(235)    => __E_out_93(236).
	p(NT(235), (NT(236)));
//G306: out(230)             => out_var_name(235) '[' offset(234) ']'.
	p(NT(230), (NT(235)+T(49)+NT(234)+T(50)));
//G307: capture(70)          => '$' charvar(224).
	p(NT(70), (T(51)+NT(224)));
//G308: q_bool_var(101)      => capture(70).
	p(NT(101), (NT(70)));
//G309: q_bool_var(101)      => bool_variable(141).
	p(NT(101), (NT(141)));
//G310: q_var(106)           => capture(70).
	p(NT(106), (NT(70)));
//G311: q_var(106)           => variable(166).
	p(NT(106), (NT(166)));
//G312: __E_offsets_94(237)  => _(16) ',' _(16) offset(234).
	p(NT(237), (NT(16)+T(32)+NT(16)+NT(234)));
//G313: __E_offsets_95(238)  => null.
	p(NT(238), (nul));
//G314: __E_offsets_95(238)  => __E_offsets_94(237) __E_offsets_95(238).
	p(NT(238), (NT(237)+NT(238)));
//G315: offsets(87)          => '[' _(16) offset(234) __E_offsets_95(238) _(16) ']'.
	p(NT(87), (T(49)+NT(16)+NT(234)+NT(238)+NT(16)+T(50)));
//G316: offset(234)          => num(239).
	p(NT(234), (NT(239)));
//G317: offset(234)          => capture(70).
	p(NT(234), (NT(70)));
//G318: offset(234)          => shift(240).
	p(NT(234), (NT(240)));
//G319: __N_2(434)           => io_var(228).
	p(NT(434), (NT(228)));
//G320: __E_offset_96(241)   => variable(166) & ~( __N_2(434) ).	 # conjunctive
	p(NT(241), (NT(166)) & ~(NT(434)));
//G321: offset(234)          => __E_offset_96(241).
	p(NT(234), (NT(241)));
//G322: __E_shift_97(242)    => capture(70).
	p(NT(242), (NT(70)));
//G323: __N_3(435)           => io_var(228).
	p(NT(435), (NT(228)));
//G324: __E___E_shift_97_98(243) => variable(166) & ~( __N_3(435) ).	 # conjunctive
	p(NT(243), (NT(166)) & ~(NT(435)));
//G325: __E_shift_97(242)    => __E___E_shift_97_98(243).
	p(NT(242), (NT(243)));
//G326: shift(240)           => __E_shift_97(242) _(16) '-' _(16) num(239).
	p(NT(240), (NT(242)+NT(16)+T(21)+NT(16)+NT(239)));
//G327: num(239)             => digits(244).
	p(NT(239), (NT(244)));
//G328: constant(163)        => binding(245).
	p(NT(163), (NT(245)));
//G329: constant(163)        => capture(70).
	p(NT(163), (NT(70)));
//G330: __E___E_binding_99_100(248) => chars(233) _(16).
	p(NT(248), (NT(233)+NT(16)));
//G331: __E___E_binding_99_100(248) => _(16).
	p(NT(248), (NT(16)));
//G332: type(247)            => __E___E_binding_99_100(248).
	p(NT(247), (NT(248)));
//G333: __E_binding_99(246)  => type(247) ':' _(16) source(249).
	p(NT(246), (NT(247)+T(3)+NT(16)+NT(249)));
//G334: source_binding(46)   => __E_binding_99(246).
	p(NT(46), (NT(246)));
//G335: binding(245)         => source_binding(46).
	p(NT(245), (NT(46)));
//G336: named_binding(250)   => chars(233).
	p(NT(250), (NT(233)));
//G337: binding(245)         => named_binding(250).
	p(NT(245), (NT(250)));
//G338: __E_source_101(252)  => alnum(6).
	p(NT(252), (NT(6)));
//G339: __E_source_101(252)  => punct(7).
	p(NT(252), (NT(7)));
//G340: src_c(251)           => __E_source_101(252).
	p(NT(251), (NT(252)));
//G341: __E___E_source_102_103(254) => src_c(251).
	p(NT(254), (NT(251)));
//G342: __E___E_source_102_103(254) => space(2).
	p(NT(254), (NT(2)));
//G343: __E___E_source_102_104(255) => null.
	p(NT(255), (nul));
//G344: __E___E_source_102_104(255) => __E___E_source_102_103(254) __E___E_source_102_104(255).
	p(NT(255), (NT(254)+NT(255)));
//G345: __E_source_102(253)  => __E___E_source_102_104(255) src_c(251).
	p(NT(253), (NT(255)+NT(251)));
//G346: __E_source_102(253)  => null.
	p(NT(253), (nul));
//G347: source(249)          => src_c(251) __E_source_102(253).
	p(NT(249), (NT(251)+NT(253)));
//G348: __E_chars_105(256)   => null.
	p(NT(256), (nul));
//G349: __E_chars_105(256)   => alnum(6) __E_chars_105(256).
	p(NT(256), (NT(6)+NT(256)));
//G350: chars(233)           => alpha(5) __E_chars_105(256).
	p(NT(233), (NT(5)+NT(256)));
//G351: __E_digits_106(257)  => digit(3).
	p(NT(257), (NT(3)));
//G352: __E_digits_106(257)  => digit(3) __E_digits_106(257).
	p(NT(257), (NT(3)+NT(257)));
//G353: digits(244)          => __E_digits_106(257).
	p(NT(244), (NT(257)));
//G354: sym(85)              => chars(233).
	p(NT(85), (NT(233)));
//G355: __E_comment_107(259) => printable(8).
	p(NT(259), (NT(8)));
//G356: __E_comment_107(259) => '\t'.
	p(NT(259), (T(52)));
//G357: __E_comment_108(260) => null.
	p(NT(260), (nul));
//G358: __E_comment_108(260) => __E_comment_107(259) __E_comment_108(260).
	p(NT(260), (NT(259)+NT(260)));
//G359: __E_comment_109(261) => '\n'.
	p(NT(261), (T(53)));
//G360: __E_comment_109(261) => '\r'.
	p(NT(261), (T(54)));
//G361: __E_comment_109(261) => eof(1).
	p(NT(261), (NT(1)));
//G362: comment(258)         => '#' __E_comment_108(260) __E_comment_109(261).
	p(NT(258), (T(55)+NT(260)+NT(261)));
//G363: __E____110(262)      => space(2).
	p(NT(262), (NT(2)));
//G364: __E____110(262)      => comment(258).
	p(NT(262), (NT(258)));
//G365: __(72)               => __E____110(262) _(16).
	p(NT(72), (NT(262)+NT(16)));
//G366: __E___111(263)       => __(72).
	p(NT(263), (NT(72)));
//G367: __E___111(263)       => null.
	p(NT(263), (nul));
//G368: _(16)                => __E___111(263).
	p(NT(16), (NT(263)));
//G369: __E_cli_112(266)     => '.' _(16) cli_command(265) _(16).
	p(NT(266), (T(2)+NT(16)+NT(265)+NT(16)));
//G370: __E_cli_113(267)     => null.
	p(NT(267), (nul));
//G371: __E_cli_113(267)     => __E_cli_112(266) __E_cli_113(267).
	p(NT(267), (NT(266)+NT(267)));
//G372: cli(264)             => _(16) cli_command(265) _(16) __E_cli_113(267).
	p(NT(264), (NT(16)+NT(265)+NT(16)+NT(267)));
//G373: __E_cli_command_114(270) => 'q'.
	p(NT(270), (T(43)));
//G374: __E_cli_command_114(270) => 'q' 'u' 'i' 't'.
	p(NT(270), (T(43)+T(34)+T(38)+T(33)));
//G375: quit_sym(269)        => __E_cli_command_114(270).
	p(NT(269), (NT(270)));
//G376: quit_cmd(268)        => quit_sym(269).
	p(NT(268), (NT(269)));
//G377: cli_command(265)     => quit_cmd(268).
	p(NT(265), (NT(268)));
//G378: __E_cli_command_115(273) => 'v'.
	p(NT(273), (T(39)));
//G379: __E_cli_command_115(273) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(273), (T(39)+T(19)+T(41)+T(37)+T(38)+T(15)+T(44)));
//G380: version_sym(272)     => __E_cli_command_115(273).
	p(NT(272), (NT(273)));
//G381: version_cmd(271)     => version_sym(272).
	p(NT(271), (NT(272)));
//G382: cli_command(265)     => version_cmd(271).
	p(NT(265), (NT(271)));
//G383: __E_cli_command_116(276) => 'c'.
	p(NT(276), (T(35)));
//G384: __E_cli_command_116(276) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(276), (T(35)+T(16)+T(19)+T(18)+T(41)));
//G385: clear_sym(275)       => __E_cli_command_116(276).
	p(NT(275), (NT(276)));
//G386: clear_cmd(274)       => clear_sym(275).
	p(NT(274), (NT(275)));
//G387: cli_command(265)     => clear_cmd(274).
	p(NT(265), (NT(274)));
//G388: __E___E_cli_command_117_118(280) => 'h'.
	p(NT(280), (T(45)));
//G389: __E___E_cli_command_117_118(280) => 'h' 'e' 'l' 'p'.
	p(NT(280), (T(45)+T(19)+T(16)+T(36)));
//G390: help_sym(279)        => __E___E_cli_command_117_118(280).
	p(NT(279), (NT(280)));
//G391: __E___E_cli_command_117_119(281) => __(72) help_arg(282).
	p(NT(281), (NT(72)+NT(282)));
//G392: __E___E_cli_command_117_119(281) => null.
	p(NT(281), (nul));
//G393: __E_cli_command_117(278) => help_sym(279) __E___E_cli_command_117_119(281).
	p(NT(278), (NT(279)+NT(281)));
//G394: help_cmd(277)        => __E_cli_command_117(278).
	p(NT(277), (NT(278)));
//G395: cli_command(265)     => help_cmd(277).
	p(NT(265), (NT(277)));
//G396: file_sym(285)        => 'f' 'i' 'l' 'e'.
	p(NT(285), (T(27)+T(38)+T(16)+T(19)));
//G397: __E_cli_command_120(284) => file_sym(285) __(72) q_string(286).
	p(NT(284), (NT(285)+NT(72)+NT(286)));
//G398: file_cmd(283)        => __E_cli_command_120(284).
	p(NT(283), (NT(284)));
//G399: cli_command(265)     => file_cmd(283).
	p(NT(265), (NT(283)));
//G400: valid_sym(289)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(289), (T(39)+T(18)+T(16)+T(38)+T(42)));
//G401: __E_cli_command_121(288) => valid_sym(289) __(72) gssotc_rr(15).
	p(NT(288), (NT(289)+NT(72)+NT(15)));
//G402: valid_cmd(287)       => __E_cli_command_121(288).
	p(NT(287), (NT(288)));
//G403: cli_command(265)     => valid_cmd(287).
	p(NT(265), (NT(287)));
//G404: sat_sym(292)         => 's' 'a' 't'.
	p(NT(292), (T(37)+T(18)+T(33)));
//G405: __E_cli_command_122(291) => sat_sym(292) __(72) gssotc_rr(15).
	p(NT(291), (NT(292)+NT(72)+NT(15)));
//G406: sat_cmd(290)         => __E_cli_command_122(291).
	p(NT(290), (NT(291)));
//G407: cli_command(265)     => sat_cmd(290).
	p(NT(265), (NT(290)));
//G408: unsat_sym(295)       => 'u' 'n' 's' 'a' 't'.
	p(NT(295), (T(34)+T(44)+T(37)+T(18)+T(33)));
//G409: __E_cli_command_123(294) => unsat_sym(295) __(72) gssotc_rr(15).
	p(NT(294), (NT(295)+NT(72)+NT(15)));
//G410: unsat_cmd(293)       => __E_cli_command_123(294).
	p(NT(293), (NT(294)));
//G411: cli_command(265)     => unsat_cmd(293).
	p(NT(265), (NT(293)));
//G412: solve_sym(298)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(298), (T(37)+T(15)+T(16)+T(39)+T(19)));
//G413: solve_cmd_arg(299)   => wff_cmd_arg(300).
	p(NT(299), (NT(300)));
//G414: __E_cli_command_124(297) => solve_sym(298) __(72) solve_cmd_arg(299).
	p(NT(297), (NT(298)+NT(72)+NT(299)));
//G415: solve_cmd(296)       => __E_cli_command_124(297).
	p(NT(296), (NT(297)));
//G416: cli_command(265)     => solve_cmd(296).
	p(NT(265), (NT(296)));
//G417: __E___E_cli_command_125_126(304) => 'e'.
	p(NT(304), (T(19)));
//G418: __E___E_cli_command_125_126(304) => 'e' 'x' 'e' 'c' 'u' 't' 'e'.
	p(NT(304), (T(19)+T(20)+T(19)+T(35)+T(34)+T(33)+T(19)));
//G419: execute_sym(303)     => __E___E_cli_command_125_126(304).
	p(NT(303), (NT(304)));
//G420: __E___E_cli_command_125_127(306) => tau(20).
	p(NT(306), (NT(20)));
//G421: __E___E_cli_command_125_127(306) => gssotc_rr(15).
	p(NT(306), (NT(15)));
//G422: execute_cmd_arg(305) => __E___E_cli_command_125_127(306).
	p(NT(305), (NT(306)));
//G423: __E_cli_command_125(302) => execute_sym(303) __(72) execute_cmd_arg(305).
	p(NT(302), (NT(303)+NT(72)+NT(305)));
//G424: execute_cmd(301)     => __E_cli_command_125(302).
	p(NT(301), (NT(302)));
//G425: cli_command(265)     => execute_cmd(301).
	p(NT(265), (NT(301)));
//G426: __E___E_cli_command_128_129(310) => 'n'.
	p(NT(310), (T(44)));
//G427: __E___E_cli_command_128_129(310) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(310), (T(44)+T(15)+T(41)+T(47)+T(18)+T(16)+T(38)+T(48)+T(19)));
//G428: normalize_sym(309)   => __E___E_cli_command_128_129(310).
	p(NT(309), (NT(310)));
//G429: __E_cli_command_128(308) => normalize_sym(309) __(72) normalize_cmd_arg(311).
	p(NT(308), (NT(309)+NT(72)+NT(311)));
//G430: normalize_cmd(307)   => __E_cli_command_128(308).
	p(NT(307), (NT(308)));
//G431: cli_command(265)     => normalize_cmd(307).
	p(NT(265), (NT(307)));
//G432: __E___E_cli_command_130_131(315) => 'i'.
	p(NT(315), (T(38)));
//G433: __E___E_cli_command_130_131(315) => 'i' 'n' 's' 't'.
	p(NT(315), (T(38)+T(44)+T(37)+T(33)));
//G434: __E___E_cli_command_130_131(315) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(315), (T(38)+T(44)+T(37)+T(33)+T(18)+T(44)+T(33)+T(38)+T(18)+T(33)+T(19)));
//G435: inst_sym(314)        => __E___E_cli_command_130_131(315).
	p(NT(314), (NT(315)));
//G436: __E_cli_command_130(313) => inst_sym(314) __(72) variable(166) __(72) wff_cmd_arg(300) __(72) wff_cmd_out_arg(316).
	p(NT(313), (NT(314)+NT(72)+NT(166)+NT(72)+NT(300)+NT(72)+NT(316)));
//G437: wff_inst_cmd(312)    => __E_cli_command_130(313).
	p(NT(312), (NT(313)));
//G438: cli_command(265)     => wff_inst_cmd(312).
	p(NT(265), (NT(312)));
//G439: __E___E_cli_command_132_133(320) => 's'.
	p(NT(320), (T(37)));
//G440: __E___E_cli_command_132_133(320) => 's' 'u' 'b' 's' 't'.
	p(NT(320), (T(37)+T(34)+T(14)+T(37)+T(33)));
//G441: __E___E_cli_command_132_133(320) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(320), (T(37)+T(34)+T(14)+T(37)+T(33)+T(38)+T(33)+T(34)+T(33)+T(19)));
//G442: subst_sym(319)       => __E___E_cli_command_132_133(320).
	p(NT(319), (NT(320)));
//G443: __E_cli_command_132(318) => subst_sym(319) __(72) wff_cmd_arg(300) __(72) wff_cmd_arg(300) __(72) wff_cmd_out_arg(316).
	p(NT(318), (NT(319)+NT(72)+NT(300)+NT(72)+NT(300)+NT(72)+NT(316)));
//G444: wff_subst_cmd(317)   => __E_cli_command_132(318).
	p(NT(317), (NT(318)));
//G445: cli_command(265)     => wff_subst_cmd(317).
	p(NT(265), (NT(317)));
//G446: __E_cli_command_134(322) => inst_sym(314) __(72) variable(166) __(72) bf_cmd_arg(323) __(72) bf_cmd_out_arg(324).
	p(NT(322), (NT(314)+NT(72)+NT(166)+NT(72)+NT(323)+NT(72)+NT(324)));
//G447: bf_inst_cmd(321)     => __E_cli_command_134(322).
	p(NT(321), (NT(322)));
//G448: cli_command(265)     => bf_inst_cmd(321).
	p(NT(265), (NT(321)));
//G449: __E_cli_command_135(326) => subst_sym(319) __(72) bf_cmd_arg(323) __(72) bf_cmd_arg(323) __(72) bf_cmd_out_arg(324).
	p(NT(326), (NT(319)+NT(72)+NT(323)+NT(72)+NT(323)+NT(72)+NT(324)));
//G450: bf_subst_cmd(325)    => __E_cli_command_135(326).
	p(NT(325), (NT(326)));
//G451: cli_command(265)     => bf_subst_cmd(325).
	p(NT(265), (NT(325)));
//G452: dnf_sym(329)         => 'd' 'n' 'f'.
	p(NT(329), (T(42)+T(44)+T(27)));
//G453: __E_cli_command_136(328) => dnf_sym(329) __(72) nf_cmd_arg(330).
	p(NT(328), (NT(329)+NT(72)+NT(330)));
//G454: dnf_cmd(327)         => __E_cli_command_136(328).
	p(NT(327), (NT(328)));
//G455: cli_command(265)     => dnf_cmd(327).
	p(NT(265), (NT(327)));
//G456: cnf_sym(333)         => 'c' 'n' 'f'.
	p(NT(333), (T(35)+T(44)+T(27)));
//G457: __E_cli_command_137(332) => cnf_sym(333) __(72) nf_cmd_arg(330).
	p(NT(332), (NT(333)+NT(72)+NT(330)));
//G458: cnf_cmd(331)         => __E_cli_command_137(332).
	p(NT(331), (NT(332)));
//G459: cli_command(265)     => cnf_cmd(331).
	p(NT(265), (NT(331)));
//G460: anf_sym(336)         => 'a' 'n' 'f'.
	p(NT(336), (T(18)+T(44)+T(27)));
//G461: __E_cli_command_138(335) => anf_sym(336) __(72) nf_cmd_arg(330).
	p(NT(335), (NT(336)+NT(72)+NT(330)));
//G462: anf_cmd(334)         => __E_cli_command_138(335).
	p(NT(334), (NT(335)));
//G463: cli_command(265)     => anf_cmd(334).
	p(NT(265), (NT(334)));
//G464: nnf_sym(339)         => 'n' 'n' 'f'.
	p(NT(339), (T(44)+T(44)+T(27)));
//G465: __E_cli_command_139(338) => nnf_sym(339) __(72) nf_cmd_arg(330).
	p(NT(338), (NT(339)+NT(72)+NT(330)));
//G466: nnf_cmd(337)         => __E_cli_command_139(338).
	p(NT(337), (NT(338)));
//G467: cli_command(265)     => nnf_cmd(337).
	p(NT(265), (NT(337)));
//G468: pnf_sym(342)         => 'p' 'n' 'f'.
	p(NT(342), (T(36)+T(44)+T(27)));
//G469: __E_cli_command_140(341) => pnf_sym(342) __(72) nf_cmd_arg(330).
	p(NT(341), (NT(342)+NT(72)+NT(330)));
//G470: pnf_cmd(340)         => __E_cli_command_140(341).
	p(NT(340), (NT(341)));
//G471: cli_command(265)     => pnf_cmd(340).
	p(NT(265), (NT(340)));
//G472: mnf_sym(345)         => 'm' 'n' 'f'.
	p(NT(345), (T(47)+T(44)+T(27)));
//G473: __E_cli_command_141(344) => mnf_sym(345) __(72) nf_cmd_arg(330).
	p(NT(344), (NT(345)+NT(72)+NT(330)));
//G474: mnf_cmd(343)         => __E_cli_command_141(344).
	p(NT(343), (NT(344)));
//G475: cli_command(265)     => mnf_cmd(343).
	p(NT(265), (NT(343)));
//G476: snf_sym(348)         => 's' 'n' 'f'.
	p(NT(348), (T(37)+T(44)+T(27)));
//G477: __E_cli_command_142(347) => snf_sym(348) __(72) nf_cmd_arg(330).
	p(NT(347), (NT(348)+NT(72)+NT(330)));
//G478: snf_cmd(346)         => __E_cli_command_142(347).
	p(NT(346), (NT(347)));
//G479: cli_command(265)     => snf_cmd(346).
	p(NT(265), (NT(346)));
//G480: onf_sym(351)         => 'o' 'n' 'f'.
	p(NT(351), (T(15)+T(44)+T(27)));
//G481: __E_cli_command_143(350) => onf_sym(351) __(72) variable(166) __(72) onf_cmd_arg(352).
	p(NT(350), (NT(351)+NT(72)+NT(166)+NT(72)+NT(352)));
//G482: onf_cmd(349)         => __E_cli_command_143(350).
	p(NT(349), (NT(350)));
//G483: cli_command(265)     => onf_cmd(349).
	p(NT(265), (NT(349)));
//G484: def_sym(355)         => 'd' 'e' 'f'.
	p(NT(355), (T(42)+T(19)+T(27)));
//G485: __E_cli_command_144(354) => def_sym(355).
	p(NT(354), (NT(355)));
//G486: def_list_cmd(353)    => __E_cli_command_144(354).
	p(NT(353), (NT(354)));
//G487: cli_command(265)     => def_list_cmd(353).
	p(NT(265), (NT(353)));
//G488: __E_cli_command_145(357) => def_sym(355) __(72) gssotc_rec_relation_form(24).
	p(NT(357), (NT(355)+NT(72)+NT(24)));
//G489: def_rr_cmd(356)      => __E_cli_command_145(357).
	p(NT(356), (NT(357)));
//G490: cli_command(265)     => def_rr_cmd(356).
	p(NT(265), (NT(356)));
//G491: __E___E_cli_command_146_147(361) => 'd'.
	p(NT(361), (T(42)));
//G492: __E___E_cli_command_146_147(361) => 'd' 'e' 'l'.
	p(NT(361), (T(42)+T(19)+T(16)));
//G493: __E___E_cli_command_146_147(361) => 'd' 'e' 'l' 'e' 't' 'e'.
	p(NT(361), (T(42)+T(19)+T(16)+T(19)+T(33)+T(19)));
//G494: del_sym(360)         => __E___E_cli_command_146_147(361).
	p(NT(360), (NT(361)));
//G495: __E_cli_command_146(359) => def_sym(355) __(72) del_sym(360) __(72) memory(362).
	p(NT(359), (NT(355)+NT(72)+NT(360)+NT(72)+NT(362)));
//G496: def_del_cmd(358)     => __E_cli_command_146(359).
	p(NT(358), (NT(359)));
//G497: cli_command(265)     => def_del_cmd(358).
	p(NT(265), (NT(358)));
//G498: __E_cli_command_148(364) => def_sym(355) __(72) clear_sym(275).
	p(NT(364), (NT(355)+NT(72)+NT(275)));
//G499: def_clear_cmd(363)   => __E_cli_command_148(364).
	p(NT(363), (NT(364)));
//G500: cli_command(265)     => def_clear_cmd(363).
	p(NT(265), (NT(363)));
//G501: __E_cli_command_149(366) => def_sym(355) __(72) memory(362).
	p(NT(366), (NT(355)+NT(72)+NT(362)));
//G502: def_print_cmd(365)   => __E_cli_command_149(366).
	p(NT(365), (NT(366)));
//G503: cli_command(265)     => def_print_cmd(365).
	p(NT(265), (NT(365)));
//G504: __E___E_cli_command_150_151(370) => 'm'.
	p(NT(370), (T(47)));
//G505: __E___E_cli_command_150_151(370) => 'm' 'e' 'm'.
	p(NT(370), (T(47)+T(19)+T(47)));
//G506: __E___E_cli_command_150_151(370) => 'm' 'e' 'm' 'o' 'r' 'y'.
	p(NT(370), (T(47)+T(19)+T(47)+T(15)+T(41)+T(56)));
//G507: memory_sym(369)      => __E___E_cli_command_150_151(370).
	p(NT(369), (NT(370)));
//G508: __E_cli_command_150(368) => memory_sym(369).
	p(NT(368), (NT(369)));
//G509: memory_list_cmd(367) => __E_cli_command_150(368).
	p(NT(367), (NT(368)));
//G510: cli_command(265)     => memory_list_cmd(367).
	p(NT(265), (NT(367)));
//G511: __E_cli_command_152(372) => memory_sym(369) __(72) del_sym(360) __(72) memory(362).
	p(NT(372), (NT(369)+NT(72)+NT(360)+NT(72)+NT(362)));
//G512: memory_del_cmd(371)  => __E_cli_command_152(372).
	p(NT(371), (NT(372)));
//G513: cli_command(265)     => memory_del_cmd(371).
	p(NT(265), (NT(371)));
//G514: __E_cli_command_153(374) => memory_sym(369) __(72) clear_sym(275).
	p(NT(374), (NT(369)+NT(72)+NT(275)));
//G515: memory_clear_cmd(373) => __E_cli_command_153(374).
	p(NT(373), (NT(374)));
//G516: cli_command(265)     => memory_clear_cmd(373).
	p(NT(265), (NT(373)));
//G517: __E_cli_command_154(376) => memory_sym(369) __(72) memory(362).
	p(NT(376), (NT(369)+NT(72)+NT(362)));
//G518: memory_print_cmd(375) => __E_cli_command_154(376).
	p(NT(375), (NT(376)));
//G519: cli_command(265)     => memory_print_cmd(375).
	p(NT(265), (NT(375)));
//G520: __E___E_cli_command_155_156(380) => bf(41).
	p(NT(380), (NT(41)));
//G521: __E___E_cli_command_155_156(380) => wff(32).
	p(NT(380), (NT(32)));
//G522: __E___E_cli_command_155_156(380) => tau(20).
	p(NT(380), (NT(20)));
//G523: memory_store_cmd_arg(379) => __E___E_cli_command_155_156(380).
	p(NT(379), (NT(380)));
//G524: __E_cli_command_155(378) => memory_sym(369) __(72) memory_store_cmd_arg(379).
	p(NT(378), (NT(369)+NT(72)+NT(379)));
//G525: memory_store_cmd(377) => __E_cli_command_155(378).
	p(NT(377), (NT(378)));
//G526: cli_command(265)     => memory_store_cmd(377).
	p(NT(265), (NT(377)));
//G527: get_sym(383)         => 'g' 'e' 't'.
	p(NT(383), (T(46)+T(19)+T(33)));
//G528: __E___E_cli_command_157_158(384) => __(72) option(385).
	p(NT(384), (NT(72)+NT(385)));
//G529: __E___E_cli_command_157_158(384) => null.
	p(NT(384), (nul));
//G530: __E_cli_command_157(382) => get_sym(383) __E___E_cli_command_157_158(384).
	p(NT(382), (NT(383)+NT(384)));
//G531: get_cmd(381)         => __E_cli_command_157(382).
	p(NT(381), (NT(382)));
//G532: cli_command(265)     => get_cmd(381).
	p(NT(265), (NT(381)));
//G533: set_sym(388)         => 's' 'e' 't'.
	p(NT(388), (T(37)+T(19)+T(33)));
//G534: __E___E_cli_command_159_160(389) => __(72).
	p(NT(389), (NT(72)));
//G535: __E___E_cli_command_159_160(389) => _(16) '=' _(16).
	p(NT(389), (NT(16)+T(4)+NT(16)));
//G536: __E_cli_command_159(387) => set_sym(388) __(72) option(385) __E___E_cli_command_159_160(389) option_value(390).
	p(NT(387), (NT(388)+NT(72)+NT(385)+NT(389)+NT(390)));
//G537: set_cmd(386)         => __E_cli_command_159(387).
	p(NT(386), (NT(387)));
//G538: cli_command(265)     => set_cmd(386).
	p(NT(265), (NT(386)));
//G539: toggle_sym(393)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(393), (T(33)+T(15)+T(46)+T(46)+T(16)+T(19)));
//G540: __E_cli_command_161(392) => toggle_sym(393) __(72) bool_option(394).
	p(NT(392), (NT(393)+NT(72)+NT(394)));
//G541: toggle_cmd(391)      => __E_cli_command_161(392).
	p(NT(391), (NT(392)));
//G542: cli_command(265)     => toggle_cmd(391).
	p(NT(265), (NT(391)));
//G543: bf_cmd_arg(323)      => memory(362).
	p(NT(323), (NT(362)));
//G544: bf_cmd_arg(323)      => bf_selection(395).
	p(NT(323), (NT(395)));
//G545: bf_cmd_arg(323)      => bf(41).
	p(NT(323), (NT(41)));
//G546: bf_cmd_out_arg(324)  => memory(362).
	p(NT(324), (NT(362)));
//G547: bf_cmd_out_arg(324)  => bf_selection(395).
	p(NT(324), (NT(395)));
//G548: wff_cmd_arg(300)     => memory(362).
	p(NT(300), (NT(362)));
//G549: wff_cmd_arg(300)     => wff_selection(396).
	p(NT(300), (NT(396)));
//G550: wff_cmd_arg(300)     => wff(32).
	p(NT(300), (NT(32)));
//G551: wff_cmd_out_arg(316) => memory(362).
	p(NT(316), (NT(362)));
//G552: wff_cmd_out_arg(316) => wff_selection(396).
	p(NT(316), (NT(396)));
//G553: nf_cmd_arg(330)      => memory(362).
	p(NT(330), (NT(362)));
//G554: nf_cmd_arg(330)      => bf(41).
	p(NT(330), (NT(41)));
//G555: nf_cmd_arg(330)      => wff(32).
	p(NT(330), (NT(32)));
//G556: onf_cmd_arg(352)     => memory(362).
	p(NT(352), (NT(362)));
//G557: onf_cmd_arg(352)     => wff(32).
	p(NT(352), (NT(32)));
//G558: normalize_cmd_arg(311) => memory(362).
	p(NT(311), (NT(362)));
//G559: normalize_cmd_arg(311) => bf(41).
	p(NT(311), (NT(41)));
//G560: normalize_cmd_arg(311) => wff(32).
	p(NT(311), (NT(32)));
//G561: normalize_cmd_arg(311) => nso_rr(14).
	p(NT(311), (NT(14)));
//G562: help_arg(282)        => help_sym(279).
	p(NT(282), (NT(279)));
//G563: help_arg(282)        => version_sym(272).
	p(NT(282), (NT(272)));
//G564: help_arg(282)        => quit_sym(269).
	p(NT(282), (NT(269)));
//G565: help_arg(282)        => clear_sym(275).
	p(NT(282), (NT(275)));
//G566: help_arg(282)        => get_sym(383).
	p(NT(282), (NT(383)));
//G567: help_arg(282)        => set_sym(388).
	p(NT(282), (NT(388)));
//G568: help_arg(282)        => toggle_sym(393).
	p(NT(282), (NT(393)));
//G569: help_arg(282)        => file_sym(285).
	p(NT(282), (NT(285)));
//G570: help_arg(282)        => memory_sym(369).
	p(NT(282), (NT(369)));
//G571: help_arg(282)        => abs_memory_sym(397).
	p(NT(282), (NT(397)));
//G572: help_arg(282)        => rel_memory_sym(398).
	p(NT(282), (NT(398)));
//G573: help_arg(282)        => selection_sym(399).
	p(NT(282), (NT(399)));
//G574: help_arg(282)        => def_sym(355).
	p(NT(282), (NT(355)));
//G575: help_arg(282)        => inst_sym(314).
	p(NT(282), (NT(314)));
//G576: help_arg(282)        => subst_sym(319).
	p(NT(282), (NT(319)));
//G577: help_arg(282)        => normalize_sym(309).
	p(NT(282), (NT(309)));
//G578: help_arg(282)        => execute_sym(303).
	p(NT(282), (NT(303)));
//G579: help_arg(282)        => solve_sym(298).
	p(NT(282), (NT(298)));
//G580: help_arg(282)        => valid_sym(289).
	p(NT(282), (NT(289)));
//G581: help_arg(282)        => sat_sym(292).
	p(NT(282), (NT(292)));
//G582: help_arg(282)        => unsat_sym(295).
	p(NT(282), (NT(295)));
//G583: help_arg(282)        => dnf_sym(329).
	p(NT(282), (NT(329)));
//G584: help_arg(282)        => cnf_sym(333).
	p(NT(282), (NT(333)));
//G585: help_arg(282)        => anf_sym(336).
	p(NT(282), (NT(336)));
//G586: help_arg(282)        => snf_sym(348).
	p(NT(282), (NT(348)));
//G587: help_arg(282)        => nnf_sym(339).
	p(NT(282), (NT(339)));
//G588: help_arg(282)        => pnf_sym(342).
	p(NT(282), (NT(342)));
//G589: help_arg(282)        => mnf_sym(345).
	p(NT(282), (NT(345)));
//G590: help_arg(282)        => onf_sym(351).
	p(NT(282), (NT(351)));
//G591: __E___E_help_arg_162_163(402) => 's'.
	p(NT(402), (T(37)));
//G592: __E___E_help_arg_162_163(402) => null.
	p(NT(402), (nul));
//G593: __E_help_arg_162(401) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_162_163(402).
	p(NT(401), (T(19)+T(20)+T(18)+T(47)+T(36)+T(16)+T(19)+NT(402)));
//G594: examples_sym(400)    => __E_help_arg_162(401).
	p(NT(400), (NT(401)));
//G595: help_arg(282)        => examples_sym(400).
	p(NT(282), (NT(400)));
//G596: rel_memory_sym(398)  => '%'.
	p(NT(398), (T(57)));
//G597: memory_id(406)       => digits(244).
	p(NT(406), (NT(244)));
//G598: __E___E_memory_164_165(405) => memory_id(406).
	p(NT(405), (NT(406)));
//G599: __E___E_memory_164_165(405) => null.
	p(NT(405), (nul));
//G600: __E_memory_164(404)  => rel_memory_sym(398) __E___E_memory_164_165(405).
	p(NT(404), (NT(398)+NT(405)));
//G601: rel_memory(403)      => __E_memory_164(404).
	p(NT(403), (NT(404)));
//G602: memory(362)          => rel_memory(403).
	p(NT(362), (NT(403)));
//G603: abs_memory_sym(397)  => '%' '-'.
	p(NT(397), (T(57)+T(21)));
//G604: __E___E_memory_166_167(409) => abs_memory_sym(397).
	p(NT(409), (NT(397)));
//G605: __E___E_memory_166_167(409) => memory_sym(369) __(72).
	p(NT(409), (NT(369)+NT(72)));
//G606: __E_memory_166(408)  => __E___E_memory_166_167(409) memory_id(406).
	p(NT(408), (NT(409)+NT(406)));
//G607: abs_memory(407)      => __E_memory_166(408).
	p(NT(407), (NT(408)));
//G608: memory(362)          => abs_memory(407).
	p(NT(362), (NT(407)));
//G609: selection_sym(399)   => 's'.
	p(NT(399), (T(37)));
//G610: selection_sym(399)   => 's' 'e' 'l' 'e' 'c' 't' 'i' 'o' 'n'.
	p(NT(399), (T(37)+T(19)+T(16)+T(19)+T(35)+T(33)+T(38)+T(15)+T(44)));
//G611: wff_selection(396)   => selection_sym(399) __(72) digits(244).
	p(NT(396), (NT(399)+NT(72)+NT(244)));
//G612: bf_selection(395)    => selection_sym(399) __(72) digits(244).
	p(NT(395), (NT(399)+NT(72)+NT(244)));
//G613: bf_var_selection(410) => selection_sym(399) __(72) digits(244).
	p(NT(410), (NT(399)+NT(72)+NT(244)));
//G614: wff_var_selection(411) => selection_sym(399) __(72) digits(244).
	p(NT(411), (NT(399)+NT(72)+NT(244)));
//G615: option(385)          => bool_option(394).
	p(NT(385), (NT(394)));
//G616: __E_option_168(414)  => 's' 'e' 'v'.
	p(NT(414), (T(37)+T(19)+T(39)));
//G617: __E_option_168(414)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(414), (T(37)+T(19)+T(39)+T(19)+T(41)+T(38)+T(33)+T(56)));
//G618: severity_opt(413)    => __E_option_168(414).
	p(NT(413), (NT(414)));
//G619: enum_option(412)     => severity_opt(413).
	p(NT(412), (NT(413)));
//G620: option(385)          => enum_option(412).
	p(NT(385), (NT(412)));
//G621: __E_bool_option_169(416) => 's'.
	p(NT(416), (T(37)));
//G622: __E_bool_option_169(416) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(416), (T(37)+T(33)+T(18)+T(33)+T(34)+T(37)));
//G623: status_opt(415)      => __E_bool_option_169(416).
	p(NT(415), (NT(416)));
//G624: bool_option(394)     => status_opt(415).
	p(NT(394), (NT(415)));
//G625: __E_bool_option_170(418) => 'c'.
	p(NT(418), (T(35)));
//G626: __E_bool_option_170(418) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(418), (T(35)+T(15)+T(16)+T(15)+T(41)));
//G627: __E_bool_option_170(418) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(418), (T(35)+T(15)+T(16)+T(15)+T(41)+T(37)));
//G628: colors_opt(417)      => __E_bool_option_170(418).
	p(NT(417), (NT(418)));
//G629: bool_option(394)     => colors_opt(417).
	p(NT(394), (NT(417)));
//G630: __E_bool_option_171(420) => 'd'.
	p(NT(420), (T(42)));
//G631: __E_bool_option_171(420) => 'd' 'b' 'g'.
	p(NT(420), (T(42)+T(14)+T(46)));
//G632: __E_bool_option_171(420) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(420), (T(42)+T(19)+T(14)+T(34)+T(46)));
//G633: debug_repl_opt(419)  => __E_bool_option_171(420).
	p(NT(419), (NT(420)));
//G634: bool_option(394)     => debug_repl_opt(419).
	p(NT(394), (NT(419)));
//G635: option_value(390)    => option_value_true(421).
	p(NT(390), (NT(421)));
//G636: option_value(390)    => option_value_false(422).
	p(NT(390), (NT(422)));
//G637: option_value(390)    => severity(423).
	p(NT(390), (NT(423)));
//G638: option_value_true(421) => 't'.
	p(NT(421), (T(33)));
//G639: option_value_true(421) => 't' 'r' 'u' 'e'.
	p(NT(421), (T(33)+T(41)+T(34)+T(19)));
//G640: option_value_true(421) => 'o' 'n'.
	p(NT(421), (T(15)+T(44)));
//G641: option_value_true(421) => '1'.
	p(NT(421), (T(30)));
//G642: option_value_true(421) => 'y'.
	p(NT(421), (T(56)));
//G643: option_value_true(421) => 'y' 'e' 's'.
	p(NT(421), (T(56)+T(19)+T(37)));
//G644: option_value_false(422) => 'f'.
	p(NT(422), (T(27)));
//G645: option_value_false(422) => 'f' 'a' 'l' 's' 'e'.
	p(NT(422), (T(27)+T(18)+T(16)+T(37)+T(19)));
//G646: option_value_false(422) => 'o' 'f' 'f'.
	p(NT(422), (T(15)+T(27)+T(27)));
//G647: option_value_false(422) => '0'.
	p(NT(422), (T(31)));
//G648: option_value_false(422) => 'n'.
	p(NT(422), (T(44)));
//G649: option_value_false(422) => 'n' 'o'.
	p(NT(422), (T(44)+T(15)));
//G650: __E_severity_172(425) => 'e'.
	p(NT(425), (T(19)));
//G651: __E_severity_172(425) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(425), (T(19)+T(41)+T(41)+T(15)+T(41)));
//G652: error_sym(424)       => __E_severity_172(425).
	p(NT(424), (NT(425)));
//G653: severity(423)        => error_sym(424).
	p(NT(423), (NT(424)));
//G654: __E_severity_173(427) => 'i'.
	p(NT(427), (T(38)));
//G655: __E_severity_173(427) => 'i' 'n' 'f' 'o'.
	p(NT(427), (T(38)+T(44)+T(27)+T(15)));
//G656: info_sym(426)        => __E_severity_173(427).
	p(NT(426), (NT(427)));
//G657: severity(423)        => info_sym(426).
	p(NT(423), (NT(426)));
//G658: __E_severity_174(429) => 'd'.
	p(NT(429), (T(42)));
//G659: __E_severity_174(429) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(429), (T(42)+T(19)+T(14)+T(34)+T(46)));
//G660: debug_sym(428)       => __E_severity_174(429).
	p(NT(428), (NT(429)));
//G661: severity(423)        => debug_sym(428).
	p(NT(423), (NT(428)));
//G662: __E_severity_175(431) => 't'.
	p(NT(431), (T(33)));
//G663: __E_severity_175(431) => 't' 'r' 'a' 'c' 'e'.
	p(NT(431), (T(33)+T(41)+T(18)+T(35)+T(19)));
//G664: trace_sym(430)       => __E_severity_175(431).
	p(NT(430), (NT(431)));
//G665: severity(423)        => trace_sym(430).
	p(NT(423), (NT(430)));
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
		__E_start_0, inputs, builder, library, nso_rr, gssotc_rr, _, gssotc_rec_relations, gssotc_main, __E_gssotc_rr_1, 
		tau, __E_gssotc_rec_relations_2, gssotc_rec_relation, __E_gssotc_rec_relations_3, gssotc_rec_relation_form, __E_gssotc_rec_relation_4, bf_rec_relation, wff_rec_relation, tau_rec_relation, nso_rec_relations, 
		nso_main, __E_nso_rr_5, wff, __E_nso_rec_relations_6, nso_rec_relation, __E_nso_rec_relations_7, nso_rec_relation_form, __E_nso_rec_relation_8, tau_ref, wff_ref, 
		bf_ref, bf, __E_inputs_9, __E_inputs_10, input, in, source_binding, rules, __E_library_11, __E___E_library_11_12, 
		rule, __E___E___E_library_11_12_13, wff_rule, bf_rule, tau_rule, __E___E_library_11_14, tau_matcher, tau_body, __E_tau_rule_15, tau_cb, 
		wff_matcher, wff_body, __E_wff_rule_16, wff_cb, bf_matcher, bf_body, __E_bf_rule_17, bf_cb, builder_head, builder_body, 
		capture, __E_builder_head_18, __, __E_builder_head_19, bf_builder_body, __E_builder_body_20, wff_builder_body, __E_builder_body_21, tau_builder_body, __E_builder_body_22, 
		tau_parenthesis, __E_tau_23, tau_wff, __E_tau_24, __E_tau_25, sym, __E___E_tau_25_26, offsets, ref_args, tau_or, 
		__E_tau_27, tau_and, __E_tau_28, tau_neg, __E_tau_29, wff_parenthesis, __E_wff_30, wff_conditional, __E_wff_31, wff_ball, 
		__E_wff_32, q_bool_var, wff_bex, __E_wff_33, wff_all, __E_wff_34, q_var, wff_ex, __E_wff_35, __E_wff_36, 
		__E___E_wff_36_37, wff_imply, __E_wff_38, wff_equiv, __E_wff_39, wff_or, __E_wff_40, wff_xor, __E_wff_41, wff_and, 
		__E_wff_42, wff_neg, __E_wff_43, wff_t, wff_f, bf_interval, __E_wff_44, bf_neq, __E_wff_45, bf_eq, 
		__E_wff_46, bf_nleq_lower, __E_wff_47, bf_nleq_upper, __E_wff_48, bf_greater, __E_wff_49, bf_less_equal, __E_wff_50, bf_less, 
		__E_wff_51, bool_variable, bf_parenthesis, __E_bf_52, bf_splitter, __E_bf_53, bf_all, __E_bf_54, bf_ex, __E_bf_55, 
		__E_bf_56, __E___E_bf_56_57, bf_or, __E_bf_58, bf_xor, __E_bf_59, bf_and, __E_bf_60, __E___E_bf_60_61, bf_neg, 
		__E_bf_62, bf_constant, __E_bf_63, constant, bf_t, bf_f, variable, __E_ref_args_64, __E___E_ref_args_64_65, ref_arg, 
		__E___E_ref_args_64_66, __E___E_ref_args_64_67, bf_cb_args1, bf_cb_arg, bf_cb_args2, bf_cb_args3, bf_cb_args4, wff_cb_args1, wff_cb_arg, wff_cb_args2, 
		wff_cb_args3, wff_cb_args4, tau_cb_args1, tau_cb_arg, __E_tau_cb_args1_68, tau_cb_args2, tau_cb_args3, tau_collapse_positives_cb, __E_tau_cb_69, __E___E_tau_cb_69_70, 
		tau_positives_upwards_cb, __E_tau_cb_71, bf_eq_cb, __E_wff_cb_72, bf_neq_cb, __E_wff_cb_73, wff_has_clashing_subformulas_cb, __E_wff_cb_74, wff_has_subformula_cb, __E_wff_cb_75, 
		wff_remove_existential_cb, __E_wff_cb_76, wff_remove_bexistential_cb, __E_wff_cb_77, wff_remove_buniversal_cb, __E_wff_cb_78, bf_is_zero_cb, __E_bf_cb_79, bf_is_one_cb, __E_bf_cb_80, 
		bf_has_subformula_cb, __E_bf_cb_81, bf_remove_funiversal_cb, __E_bf_cb_82, bf_remove_fexistential_cb, __E_bf_cb_83, bf_and_cb, __E_bf_cb_84, bf_or_cb, __E_bf_cb_85, 
		bf_xor_cb, __E_bf_cb_86, bf_neg_cb, __E_bf_cb_87, charvar, __E_charvar_88, __E_charvar_89, __E_charvar_90, io_var, __E_variable_91, 
		out, in_var_name, __E_in_92, chars, offset, out_var_name, __E_out_93, __E_offsets_94, __E_offsets_95, num, 
		shift, __E_offset_96, __E_shift_97, __E___E_shift_97_98, digits, binding, __E_binding_99, type, __E___E_binding_99_100, source, 
		named_binding, src_c, __E_source_101, __E_source_102, __E___E_source_102_103, __E___E_source_102_104, __E_chars_105, __E_digits_106, comment, __E_comment_107, 
		__E_comment_108, __E_comment_109, __E____110, __E___111, cli, cli_command, __E_cli_112, __E_cli_113, quit_cmd, quit_sym, 
		__E_cli_command_114, version_cmd, version_sym, __E_cli_command_115, clear_cmd, clear_sym, __E_cli_command_116, help_cmd, __E_cli_command_117, help_sym, 
		__E___E_cli_command_117_118, __E___E_cli_command_117_119, help_arg, file_cmd, __E_cli_command_120, file_sym, q_string, valid_cmd, __E_cli_command_121, valid_sym, 
		sat_cmd, __E_cli_command_122, sat_sym, unsat_cmd, __E_cli_command_123, unsat_sym, solve_cmd, __E_cli_command_124, solve_sym, solve_cmd_arg, 
		wff_cmd_arg, execute_cmd, __E_cli_command_125, execute_sym, __E___E_cli_command_125_126, execute_cmd_arg, __E___E_cli_command_125_127, normalize_cmd, __E_cli_command_128, normalize_sym, 
		__E___E_cli_command_128_129, normalize_cmd_arg, wff_inst_cmd, __E_cli_command_130, inst_sym, __E___E_cli_command_130_131, wff_cmd_out_arg, wff_subst_cmd, __E_cli_command_132, subst_sym, 
		__E___E_cli_command_132_133, bf_inst_cmd, __E_cli_command_134, bf_cmd_arg, bf_cmd_out_arg, bf_subst_cmd, __E_cli_command_135, dnf_cmd, __E_cli_command_136, dnf_sym, 
		nf_cmd_arg, cnf_cmd, __E_cli_command_137, cnf_sym, anf_cmd, __E_cli_command_138, anf_sym, nnf_cmd, __E_cli_command_139, nnf_sym, 
		pnf_cmd, __E_cli_command_140, pnf_sym, mnf_cmd, __E_cli_command_141, mnf_sym, snf_cmd, __E_cli_command_142, snf_sym, onf_cmd, 
		__E_cli_command_143, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_144, def_sym, def_rr_cmd, __E_cli_command_145, def_del_cmd, __E_cli_command_146, 
		del_sym, __E___E_cli_command_146_147, memory, def_clear_cmd, __E_cli_command_148, def_print_cmd, __E_cli_command_149, memory_list_cmd, __E_cli_command_150, memory_sym, 
		__E___E_cli_command_150_151, memory_del_cmd, __E_cli_command_152, memory_clear_cmd, __E_cli_command_153, memory_print_cmd, __E_cli_command_154, memory_store_cmd, __E_cli_command_155, memory_store_cmd_arg, 
		__E___E_cli_command_155_156, get_cmd, __E_cli_command_157, get_sym, __E___E_cli_command_157_158, option, set_cmd, __E_cli_command_159, set_sym, __E___E_cli_command_159_160, 
		option_value, toggle_cmd, __E_cli_command_161, toggle_sym, bool_option, bf_selection, wff_selection, abs_memory_sym, rel_memory_sym, selection_sym, 
		examples_sym, __E_help_arg_162, __E___E_help_arg_162_163, rel_memory, __E_memory_164, __E___E_memory_164_165, memory_id, abs_memory, __E_memory_166, __E___E_memory_166_167, 
		bf_var_selection, wff_var_selection, enum_option, severity_opt, __E_option_168, status_opt, __E_bool_option_169, colors_opt, __E_bool_option_170, debug_repl_opt, 
		__E_bool_option_171, option_value_true, option_value_false, severity, error_sym, __E_severity_172, info_sym, __E_severity_173, debug_sym, __E_severity_174, 
		trace_sym, __E_severity_175, __N_0, __N_1, __N_2, __N_3, 
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
