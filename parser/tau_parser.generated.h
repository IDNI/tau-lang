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
	"__E_wff_51", "bool_variable", "bf_parenthesis", "__E_bf_52", "bf_splitter", "__E_bf_53", "__E_bf_54", "__E___E_bf_54_55", "bf_or", "__E_bf_56", 
	"bf_xor", "__E_bf_57", "bf_and", "__E_bf_58", "__E___E_bf_58_59", "bf_neg", "__E_bf_60", "bf_constant", "__E_bf_61", "constant", 
	"bf_t", "bf_f", "variable", "__E_ref_args_62", "__E___E_ref_args_62_63", "ref_arg", "__E___E_ref_args_62_64", "__E___E_ref_args_62_65", "bf_cb_args1", "bf_cb_arg", 
	"bf_cb_args2", "bf_cb_args3", "bf_cb_args4", "wff_cb_args1", "wff_cb_arg", "wff_cb_args2", "wff_cb_args3", "wff_cb_args4", "tau_cb_args1", "tau_cb_arg", 
	"__E_tau_cb_args1_66", "tau_cb_args2", "tau_cb_args3", "tau_collapse_positives_cb", "__E_tau_cb_67", "__E___E_tau_cb_67_68", "tau_positives_upwards_cb", "__E_tau_cb_69", "bf_eq_cb", "__E_wff_cb_70", 
	"bf_neq_cb", "__E_wff_cb_71", "wff_has_clashing_subformulas_cb", "__E_wff_cb_72", "wff_has_subformula_cb", "__E_wff_cb_73", "wff_remove_existential_cb", "__E_wff_cb_74", "wff_remove_bexistential_cb", "__E_wff_cb_75", 
	"wff_remove_buniversal_cb", "__E_wff_cb_76", "bf_is_zero_cb", "__E_bf_cb_77", "bf_is_one_cb", "__E_bf_cb_78", "bf_has_subformula_cb", "__E_bf_cb_79", "bf_remove_funiversal_cb", "__E_bf_cb_80", 
	"bf_remove_fexistential_cb", "__E_bf_cb_81", "bf_and_cb", "__E_bf_cb_82", "bf_or_cb", "__E_bf_cb_83", "bf_xor_cb", "__E_bf_cb_84", "bf_neg_cb", "__E_bf_cb_85", 
	"charvar", "__E_charvar_86", "__E_charvar_87", "io_var", "__E_variable_88", "out", "in_var_name", "__E_in_89", "chars", "offset", 
	"out_var_name", "__E_out_90", "__E_offsets_91", "__E_offsets_92", "num", "shift", "__E_offset_93", "__E_shift_94", "__E___E_shift_94_95", "digits", 
	"binding", "__E_binding_96", "type", "__E___E_binding_96_97", "source", "named_binding", "src_c", "__E_source_98", "__E_source_99", "__E___E_source_99_100", 
	"__E___E_source_99_101", "__E_chars_102", "__E_digits_103", "comment", "__E_comment_104", "__E_comment_105", "__E_comment_106", "__E____107", "__E___108", "cli", 
	"cli_command", "__E_cli_109", "__E_cli_110", "quit_cmd", "quit_sym", "__E_cli_command_111", "version_cmd", "version_sym", "__E_cli_command_112", "clear_cmd", 
	"clear_sym", "__E_cli_command_113", "help_cmd", "__E_cli_command_114", "help_sym", "__E___E_cli_command_114_115", "__E___E_cli_command_114_116", "help_arg", "file_cmd", "__E_cli_command_117", 
	"file_sym", "q_string", "valid_cmd", "__E_cli_command_118", "valid_sym", "sat_cmd", "__E_cli_command_119", "sat_sym", "unsat_cmd", "__E_cli_command_120", 
	"unsat_sym", "solve_cmd", "__E_cli_command_121", "solve_sym", "solve_cmd_arg", "wff_cmd_arg", "execute_cmd", "__E_cli_command_122", "execute_sym", "__E___E_cli_command_122_123", 
	"execute_cmd_arg", "__E___E_cli_command_122_124", "normalize_cmd", "__E_cli_command_125", "normalize_sym", "__E___E_cli_command_125_126", "normalize_cmd_arg", "wff_inst_cmd", "__E_cli_command_127", "inst_sym", 
	"__E___E_cli_command_127_128", "wff_cmd_out_arg", "wff_subst_cmd", "__E_cli_command_129", "subst_sym", "__E___E_cli_command_129_130", "bf_inst_cmd", "__E_cli_command_131", "bf_cmd_arg", "bf_cmd_out_arg", 
	"bf_subst_cmd", "__E_cli_command_132", "dnf_cmd", "__E_cli_command_133", "dnf_sym", "nf_cmd_arg", "cnf_cmd", "__E_cli_command_134", "cnf_sym", "anf_cmd", 
	"__E_cli_command_135", "anf_sym", "nnf_cmd", "__E_cli_command_136", "nnf_sym", "pnf_cmd", "__E_cli_command_137", "pnf_sym", "mnf_cmd", "__E_cli_command_138", 
	"mnf_sym", "snf_cmd", "__E_cli_command_139", "snf_sym", "onf_cmd", "__E_cli_command_140", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_141", 
	"def_sym", "def_rr_cmd", "__E_cli_command_142", "def_del_cmd", "__E_cli_command_143", "del_sym", "__E___E_cli_command_143_144", "memory", "def_clear_cmd", "__E_cli_command_145", 
	"def_print_cmd", "__E_cli_command_146", "memory_list_cmd", "__E_cli_command_147", "memory_sym", "__E___E_cli_command_147_148", "memory_del_cmd", "__E_cli_command_149", "memory_clear_cmd", "__E_cli_command_150", 
	"memory_print_cmd", "__E_cli_command_151", "memory_store_cmd", "__E_cli_command_152", "memory_store_cmd_arg", "__E___E_cli_command_152_153", "qelim_cmd", "__E_cli_command_154", "qelim_sym", "get_cmd", 
	"__E_cli_command_155", "get_sym", "__E___E_cli_command_155_156", "option", "set_cmd", "__E_cli_command_157", "set_sym", "__E___E_cli_command_157_158", "option_value", "toggle_cmd", 
	"__E_cli_command_159", "toggle_sym", "bool_option", "bf_selection", "wff_selection", "abs_memory_sym", "rel_memory_sym", "selection_sym", "examples_sym", "__E_help_arg_160", 
	"__E___E_help_arg_160_161", "rel_memory", "__E_memory_162", "__E___E_memory_162_163", "memory_id", "abs_memory", "__E_memory_164", "__E___E_memory_164_165", "bf_var_selection", "wff_var_selection", 
	"enum_option", "severity_opt", "__E_option_166", "status_opt", "__E_bool_option_167", "colors_opt", "__E_bool_option_168", "debug_repl_opt", "__E_bool_option_169", "option_value_true", 
	"option_value_false", "severity", "error_sym", "__E_severity_170", "info_sym", "__E_severity_171", "debug_sym", "__E_severity_172", "trace_sym", "__E_severity_173", 
	"bf_all", "bf_ex", "__N_0", "__N_1", "__N_2", "__N_3", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', ';', '.', ':', '=', '<', '{', '}', '(', 
	')', '|', '&', '!', '?', 'b', 'o', 'l', '_', 'a', 
	'e', 'x', '-', '>', '^', 'T', 'F', 'S', '+', '\'', 
	'1', '0', ',', 't', 'u', 'c', 'p', 's', 'i', 'v', 
	'w', 'r', 'd', 'f', 'q', 'n', 'h', 'g', 'm', 'z', 
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
			127, 129, 131, 133, 135, 137, 139, 142, 144, 148,
			150, 152, 155, 157, 160, 161, 183, 186, 188, 190,
			192, 194, 196, 198, 200, 202, 204, 206, 208, 210,
			212, 214, 216, 218, 235, 259, 264, 267, 270, 274,
			280, 284, 287, 290, 293, 298, 304, 309, 314, 324,
			328, 331, 334, 337, 340, 343, 346, 350, 355, 364,
			378, 381, 384, 386, 391, 395, 396, 397, 398, 411,
			413, 415, 417, 419, 420, 422, 424, 426, 428, 430,
			431
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
			{ 168 },
			{ 170 },
			{ 171 },
			{ 172 },
			{ 173 },
			{ 175 },
			{ 176 },
			{ 177 },
			{ 178 },
			{ 181 },
			{ 182 },
			{ 228 },
			{ 246 },
			{ 295 },
			{ 311 },
			{ 318 },
			{ 319 }
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
//G174: __E___E_bf_54_55(147) => _(16) offsets(87).
	p(NT(147), (NT(16)+NT(87)));
//G175: __E___E_bf_54_55(147) => null.
	p(NT(147), (nul));
//G176: __E_bf_54(146)       => sym(85) __E___E_bf_54_55(147) _(16) ref_args(88).
	p(NT(146), (NT(85)+NT(147)+NT(16)+NT(88)));
//G177: bf_ref(40)           => __E_bf_54(146).
	p(NT(40), (NT(146)));
//G178: bf(41)               => bf_ref(40).
	p(NT(41), (NT(40)));
//G179: __E_bf_56(149)       => bf(41) _(16) '|' _(16) bf(41).
	p(NT(149), (NT(41)+NT(16)+T(10)+NT(16)+NT(41)));
//G180: bf_or(148)           => __E_bf_56(149).
	p(NT(148), (NT(149)));
//G181: bf(41)               => bf_or(148).
	p(NT(41), (NT(148)));
//G182: __E_bf_57(151)       => bf(41) _(16) '+' _(16) bf(41).
	p(NT(151), (NT(41)+NT(16)+T(27)+NT(16)+NT(41)));
//G183: bf_xor(150)          => __E_bf_57(151).
	p(NT(150), (NT(151)));
//G184: bf(41)               => bf_xor(150).
	p(NT(41), (NT(150)));
//G185: __E___E_bf_58_59(154) => _(16).
	p(NT(154), (NT(16)));
//G186: __E___E_bf_58_59(154) => _(16) '&' _(16).
	p(NT(154), (NT(16)+T(11)+NT(16)));
//G187: __E_bf_58(153)       => bf(41) __E___E_bf_58_59(154) bf(41).
	p(NT(153), (NT(41)+NT(154)+NT(41)));
//G188: bf_and(152)          => __E_bf_58(153).
	p(NT(152), (NT(153)));
//G189: bf(41)               => bf_and(152).
	p(NT(41), (NT(152)));
//G190: __E_bf_60(156)       => bf(41) _(16) '\''.
	p(NT(156), (NT(41)+NT(16)+T(28)));
//G191: bf_neg(155)          => __E_bf_60(156).
	p(NT(155), (NT(156)));
//G192: bf(41)               => bf_neg(155).
	p(NT(41), (NT(155)));
//G193: __E_bf_61(158)       => '{' _(16) constant(159) _(16) '}'.
	p(NT(158), (T(6)+NT(16)+NT(159)+NT(16)+T(7)));
//G194: bf_constant(157)     => __E_bf_61(158).
	p(NT(157), (NT(158)));
//G195: bf(41)               => bf_constant(157).
	p(NT(41), (NT(157)));
//G196: bf_t(160)            => '1'.
	p(NT(160), (T(29)));
//G197: bf(41)               => bf_t(160).
	p(NT(41), (NT(160)));
//G198: bf_f(161)            => '0'.
	p(NT(161), (T(30)));
//G199: bf(41)               => bf_f(161).
	p(NT(41), (NT(161)));
//G200: bf(41)               => capture(70).
	p(NT(41), (NT(70)));
//G201: bf(41)               => variable(162).
	p(NT(41), (NT(162)));
//G202: ref_arg(165)         => bf(41).
	p(NT(165), (NT(41)));
//G203: __E___E_ref_args_62_63(164) => _(16) ref_arg(165).
	p(NT(164), (NT(16)+NT(165)));
//G204: __E___E_ref_args_62_64(166) => _(16) ',' _(16) ref_arg(165).
	p(NT(166), (NT(16)+T(31)+NT(16)+NT(165)));
//G205: __E___E_ref_args_62_65(167) => null.
	p(NT(167), (nul));
//G206: __E___E_ref_args_62_65(167) => __E___E_ref_args_62_64(166) __E___E_ref_args_62_65(167).
	p(NT(167), (NT(166)+NT(167)));
//G207: __E_ref_args_62(163) => __E___E_ref_args_62_63(164) __E___E_ref_args_62_65(167).
	p(NT(163), (NT(164)+NT(167)));
//G208: __E_ref_args_62(163) => null.
	p(NT(163), (nul));
//G209: ref_args(88)         => '(' __E_ref_args_62(163) _(16) ')'.
	p(NT(88), (T(8)+NT(163)+NT(16)+T(9)));
//G210: bf_cb_arg(169)       => bf(41).
	p(NT(169), (NT(41)));
//G211: bf_cb_args1(168)     => __(72) bf_cb_arg(169).
	p(NT(168), (NT(72)+NT(169)));
//G212: bf_cb_args2(170)     => bf_cb_args1(168) bf_cb_args1(168).
	p(NT(170), (NT(168)+NT(168)));
//G213: bf_cb_args3(171)     => bf_cb_args2(170) bf_cb_args1(168).
	p(NT(171), (NT(170)+NT(168)));
//G214: bf_cb_args4(172)     => bf_cb_args3(171) bf_cb_args1(168).
	p(NT(172), (NT(171)+NT(168)));
//G215: wff_cb_arg(174)      => wff(32).
	p(NT(174), (NT(32)));
//G216: wff_cb_args1(173)    => __(72) wff_cb_arg(174).
	p(NT(173), (NT(72)+NT(174)));
//G217: wff_cb_args2(175)    => wff_cb_args1(173) wff_cb_args1(173).
	p(NT(175), (NT(173)+NT(173)));
//G218: wff_cb_args3(176)    => wff_cb_args2(175) wff_cb_args1(173).
	p(NT(176), (NT(175)+NT(173)));
//G219: wff_cb_args4(177)    => wff_cb_args3(176) wff_cb_args1(173).
	p(NT(177), (NT(176)+NT(173)));
//G220: __E_tau_cb_args1_66(180) => capture(70).
	p(NT(180), (NT(70)));
//G221: __E_tau_cb_args1_66(180) => tau(20).
	p(NT(180), (NT(20)));
//G222: tau_cb_arg(179)      => __E_tau_cb_args1_66(180).
	p(NT(179), (NT(180)));
//G223: tau_cb_args1(178)    => __(72) tau_cb_arg(179).
	p(NT(178), (NT(72)+NT(179)));
//G224: tau_cb_args2(181)    => tau_cb_args1(178) tau_cb_args1(178).
	p(NT(181), (NT(178)+NT(178)));
//G225: tau_cb_args3(182)    => tau_cb_args2(181) tau_cb_args1(178).
	p(NT(182), (NT(181)+NT(178)));
//G226: __E___E_tau_cb_67_68(185) => tau_cb_args3(182).
	p(NT(185), (NT(182)));
//G227: __E___E_tau_cb_67_68(185) => tau_cb_args2(181).
	p(NT(185), (NT(181)));
//G228: __E_tau_cb_67(184)   => 't' 'a' 'u' '_' 'c' 'o' 'l' 'l' 'a' 'p' 's' 'e' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'c' 'b' __E___E_tau_cb_67_68(185).
	p(NT(184), (T(32)+T(18)+T(33)+T(17)+T(34)+T(15)+T(16)+T(16)+T(18)+T(35)+T(36)+T(19)+T(17)+T(35)+T(15)+T(36)+T(37)+T(32)+T(37)+T(38)+T(19)+T(36)+T(17)+T(34)+T(14)+NT(185)));
//G229: tau_collapse_positives_cb(183) => __E_tau_cb_67(184).
	p(NT(183), (NT(184)));
//G230: tau_cb(59)           => tau_collapse_positives_cb(183).
	p(NT(59), (NT(183)));
//G231: __E_tau_cb_69(187)   => 't' 'a' 'u' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'u' 'p' 'w' 'a' 'r' 'd' 's' '_' 'c' 'b' tau_cb_args2(181).
	p(NT(187), (T(32)+T(18)+T(33)+T(17)+T(35)+T(15)+T(36)+T(37)+T(32)+T(37)+T(38)+T(19)+T(36)+T(17)+T(33)+T(35)+T(39)+T(18)+T(40)+T(41)+T(36)+T(17)+T(34)+T(14)+NT(181)));
//G232: tau_positives_upwards_cb(186) => __E_tau_cb_69(187).
	p(NT(186), (NT(187)));
//G233: tau_cb(59)           => tau_positives_upwards_cb(186).
	p(NT(59), (NT(186)));
//G234: __E_wff_cb_70(189)   => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b' bf_cb_args1(168) wff_cb_args2(175).
	p(NT(189), (T(14)+T(42)+T(17)+T(19)+T(43)+T(17)+T(34)+T(14)+NT(168)+NT(175)));
//G235: bf_eq_cb(188)        => __E_wff_cb_70(189).
	p(NT(188), (NT(189)));
//G236: wff_cb(63)           => bf_eq_cb(188).
	p(NT(63), (NT(188)));
//G237: __E_wff_cb_71(191)   => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b' bf_cb_args1(168) wff_cb_args2(175).
	p(NT(191), (T(14)+T(42)+T(17)+T(44)+T(19)+T(43)+T(17)+T(34)+T(14)+NT(168)+NT(175)));
//G238: bf_neq_cb(190)       => __E_wff_cb_71(191).
	p(NT(190), (NT(191)));
//G239: wff_cb(63)           => bf_neq_cb(190).
	p(NT(63), (NT(190)));
//G240: __E_wff_cb_72(193)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(175).
	p(NT(193), (T(39)+T(42)+T(42)+T(17)+T(45)+T(18)+T(36)+T(17)+T(34)+T(16)+T(18)+T(36)+T(45)+T(37)+T(44)+T(46)+T(17)+T(36)+T(33)+T(14)+T(42)+T(15)+T(40)+T(47)+T(33)+T(16)+T(18)+T(36)+T(17)+T(34)+T(14)+NT(175)));
//G241: wff_has_clashing_subformulas_cb(192) => __E_wff_cb_72(193).
	p(NT(192), (NT(193)));
//G242: wff_cb(63)           => wff_has_clashing_subformulas_cb(192).
	p(NT(63), (NT(192)));
//G243: __E_wff_cb_73(195)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(176).
	p(NT(195), (T(39)+T(42)+T(42)+T(17)+T(45)+T(18)+T(36)+T(17)+T(36)+T(33)+T(14)+T(42)+T(15)+T(40)+T(47)+T(33)+T(16)+T(18)+T(17)+T(34)+T(14)+NT(176)));
//G244: wff_has_subformula_cb(194) => __E_wff_cb_73(195).
	p(NT(194), (NT(195)));
//G245: wff_cb(63)           => wff_has_subformula_cb(194).
	p(NT(63), (NT(194)));
//G246: __E_wff_cb_74(197)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(175).
	p(NT(197), (T(39)+T(42)+T(42)+T(17)+T(40)+T(19)+T(47)+T(15)+T(38)+T(19)+T(17)+T(19)+T(20)+T(37)+T(36)+T(32)+T(19)+T(44)+T(32)+T(37)+T(18)+T(16)+T(17)+T(34)+T(14)+NT(175)));
//G247: wff_remove_existential_cb(196) => __E_wff_cb_74(197).
	p(NT(196), (NT(197)));
//G248: wff_cb(63)           => wff_remove_existential_cb(196).
	p(NT(63), (NT(196)));
//G249: __E_wff_cb_75(199)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args4(177).
	p(NT(199), (T(39)+T(42)+T(42)+T(17)+T(40)+T(19)+T(47)+T(15)+T(38)+T(19)+T(17)+T(14)+T(19)+T(20)+T(37)+T(36)+T(32)+T(19)+T(44)+T(32)+T(37)+T(18)+T(16)+T(17)+T(34)+T(14)+NT(177)));
//G250: wff_remove_bexistential_cb(198) => __E_wff_cb_75(199).
	p(NT(198), (NT(199)));
//G251: wff_cb(63)           => wff_remove_bexistential_cb(198).
	p(NT(63), (NT(198)));
//G252: __E_wff_cb_76(201)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' wff_cb_args4(177).
	p(NT(201), (T(39)+T(42)+T(42)+T(17)+T(40)+T(19)+T(47)+T(15)+T(38)+T(19)+T(17)+T(14)+T(33)+T(44)+T(37)+T(38)+T(19)+T(40)+T(36)+T(18)+T(16)+T(17)+T(34)+T(14)+NT(177)));
//G253: wff_remove_buniversal_cb(200) => __E_wff_cb_76(201).
	p(NT(200), (NT(201)));
//G254: wff_cb(63)           => wff_remove_buniversal_cb(200).
	p(NT(63), (NT(200)));
//G255: __E_bf_cb_77(203)    => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b' bf_cb_args2(170).
	p(NT(203), (T(14)+T(42)+T(17)+T(37)+T(36)+T(17)+T(48)+T(19)+T(40)+T(15)+T(17)+T(34)+T(14)+NT(170)));
//G256: bf_is_zero_cb(202)   => __E_bf_cb_77(203).
	p(NT(202), (NT(203)));
//G257: bf_cb(67)            => bf_is_zero_cb(202).
	p(NT(67), (NT(202)));
//G258: __E_bf_cb_78(205)    => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b' bf_cb_args2(170).
	p(NT(205), (T(14)+T(42)+T(17)+T(37)+T(36)+T(17)+T(15)+T(44)+T(19)+T(17)+T(34)+T(14)+NT(170)));
//G259: bf_is_one_cb(204)    => __E_bf_cb_78(205).
	p(NT(204), (NT(205)));
//G260: bf_cb(67)            => bf_is_one_cb(204).
	p(NT(67), (NT(204)));
//G261: __E_bf_cb_79(207)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(171).
	p(NT(207), (T(14)+T(42)+T(17)+T(45)+T(18)+T(36)+T(17)+T(36)+T(33)+T(14)+T(42)+T(15)+T(40)+T(47)+T(33)+T(16)+T(18)+T(17)+T(34)+T(14)+NT(171)));
//G262: bf_has_subformula_cb(206) => __E_bf_cb_79(207).
	p(NT(206), (NT(207)));
//G263: bf_cb(67)            => bf_has_subformula_cb(206).
	p(NT(67), (NT(206)));
//G264: __E_bf_cb_80(209)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(172).
	p(NT(209), (T(14)+T(42)+T(17)+T(40)+T(19)+T(47)+T(15)+T(38)+T(19)+T(17)+T(42)+T(33)+T(44)+T(37)+T(38)+T(19)+T(40)+T(36)+T(18)+T(16)+T(17)+T(34)+T(14)+NT(172)));
//G265: bf_remove_funiversal_cb(208) => __E_bf_cb_80(209).
	p(NT(208), (NT(209)));
//G266: bf_cb(67)            => bf_remove_funiversal_cb(208).
	p(NT(67), (NT(208)));
//G267: __E_bf_cb_81(211)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(172).
	p(NT(211), (T(14)+T(42)+T(17)+T(40)+T(19)+T(47)+T(15)+T(38)+T(19)+T(17)+T(42)+T(19)+T(20)+T(37)+T(36)+T(32)+T(19)+T(44)+T(32)+T(37)+T(18)+T(16)+T(17)+T(34)+T(14)+NT(172)));
//G268: bf_remove_fexistential_cb(210) => __E_bf_cb_81(211).
	p(NT(210), (NT(211)));
//G269: bf_cb(67)            => bf_remove_fexistential_cb(210).
	p(NT(67), (NT(210)));
//G270: __E_bf_cb_82(213)    => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b' bf_cb_args2(170).
	p(NT(213), (T(14)+T(42)+T(17)+T(18)+T(44)+T(41)+T(17)+T(34)+T(14)+NT(170)));
//G271: bf_and_cb(212)       => __E_bf_cb_82(213).
	p(NT(212), (NT(213)));
//G272: bf_cb(67)            => bf_and_cb(212).
	p(NT(67), (NT(212)));
//G273: __E_bf_cb_83(215)    => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b' bf_cb_args2(170).
	p(NT(215), (T(14)+T(42)+T(17)+T(15)+T(40)+T(17)+T(34)+T(14)+NT(170)));
//G274: bf_or_cb(214)        => __E_bf_cb_83(215).
	p(NT(214), (NT(215)));
//G275: bf_cb(67)            => bf_or_cb(214).
	p(NT(67), (NT(214)));
//G276: __E_bf_cb_84(217)    => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b' bf_cb_args2(170).
	p(NT(217), (T(14)+T(42)+T(17)+T(20)+T(15)+T(40)+T(17)+T(34)+T(14)+NT(170)));
//G277: bf_xor_cb(216)       => __E_bf_cb_84(217).
	p(NT(216), (NT(217)));
//G278: bf_cb(67)            => bf_xor_cb(216).
	p(NT(67), (NT(216)));
//G279: __E_bf_cb_85(219)    => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b' bf_cb_args1(168).
	p(NT(219), (T(14)+T(42)+T(17)+T(44)+T(19)+T(46)+T(17)+T(34)+T(14)+NT(168)));
//G280: bf_neg_cb(218)       => __E_bf_cb_85(219).
	p(NT(218), (NT(219)));
//G281: bf_cb(67)            => bf_neg_cb(218).
	p(NT(67), (NT(218)));
//G282: __N_0(432)           => 'F'.
	p(NT(432), (T(25)));
//G283: __N_1(433)           => 'T'.
	p(NT(433), (T(24)));
//G284: __E_charvar_86(221)  => ~( __N_0(432) ) & ~( __N_1(433) ) & alpha(5).	 # conjunctive
	p(NT(221), ~(NT(432)) & ~(NT(433)) & (NT(5)));
//G285: __E_charvar_87(222)  => null.
	p(NT(222), (nul));
//G286: __E_charvar_87(222)  => digit(3) __E_charvar_87(222).
	p(NT(222), (NT(3)+NT(222)));
//G287: charvar(220)         => __E_charvar_86(221) __E_charvar_87(222).
	p(NT(220), (NT(221)+NT(222)));
//G288: bool_variable(141)   => '?' charvar(220).
	p(NT(141), (T(13)+NT(220)));
//G289: variable(162)        => charvar(220).
	p(NT(162), (NT(220)));
//G290: __E_variable_88(224) => in(45).
	p(NT(224), (NT(45)));
//G291: __E_variable_88(224) => out(225).
	p(NT(224), (NT(225)));
//G292: io_var(223)          => __E_variable_88(224).
	p(NT(223), (NT(224)));
//G293: variable(162)        => io_var(223).
	p(NT(162), (NT(223)));
//G294: __E_in_89(227)       => 'i' '_' chars(228).
	p(NT(227), (T(37)+T(17)+NT(228)));
//G295: in_var_name(226)     => __E_in_89(227).
	p(NT(226), (NT(227)));
//G296: in(45)               => in_var_name(226) '[' offset(229) ']'.
	p(NT(45), (NT(226)+T(49)+NT(229)+T(50)));
//G297: __E_out_90(231)      => 'o' '_' chars(228).
	p(NT(231), (T(15)+T(17)+NT(228)));
//G298: out_var_name(230)    => __E_out_90(231).
	p(NT(230), (NT(231)));
//G299: out(225)             => out_var_name(230) '[' offset(229) ']'.
	p(NT(225), (NT(230)+T(49)+NT(229)+T(50)));
//G300: capture(70)          => '$' charvar(220).
	p(NT(70), (T(51)+NT(220)));
//G301: q_bool_var(101)      => capture(70).
	p(NT(101), (NT(70)));
//G302: q_bool_var(101)      => bool_variable(141).
	p(NT(101), (NT(141)));
//G303: q_var(106)           => capture(70).
	p(NT(106), (NT(70)));
//G304: q_var(106)           => variable(162).
	p(NT(106), (NT(162)));
//G305: __E_offsets_91(232)  => _(16) ',' _(16) offset(229).
	p(NT(232), (NT(16)+T(31)+NT(16)+NT(229)));
//G306: __E_offsets_92(233)  => null.
	p(NT(233), (nul));
//G307: __E_offsets_92(233)  => __E_offsets_91(232) __E_offsets_92(233).
	p(NT(233), (NT(232)+NT(233)));
//G308: offsets(87)          => '[' _(16) offset(229) __E_offsets_92(233) _(16) ']'.
	p(NT(87), (T(49)+NT(16)+NT(229)+NT(233)+NT(16)+T(50)));
//G309: offset(229)          => num(234).
	p(NT(229), (NT(234)));
//G310: offset(229)          => capture(70).
	p(NT(229), (NT(70)));
//G311: offset(229)          => shift(235).
	p(NT(229), (NT(235)));
//G312: __N_2(434)           => io_var(223).
	p(NT(434), (NT(223)));
//G313: __E_offset_93(236)   => variable(162) & ~( __N_2(434) ).	 # conjunctive
	p(NT(236), (NT(162)) & ~(NT(434)));
//G314: offset(229)          => __E_offset_93(236).
	p(NT(229), (NT(236)));
//G315: __E_shift_94(237)    => capture(70).
	p(NT(237), (NT(70)));
//G316: __N_3(435)           => io_var(223).
	p(NT(435), (NT(223)));
//G317: __E___E_shift_94_95(238) => variable(162) & ~( __N_3(435) ).	 # conjunctive
	p(NT(238), (NT(162)) & ~(NT(435)));
//G318: __E_shift_94(237)    => __E___E_shift_94_95(238).
	p(NT(237), (NT(238)));
//G319: shift(235)           => __E_shift_94(237) _(16) '-' _(16) num(234).
	p(NT(235), (NT(237)+NT(16)+T(21)+NT(16)+NT(234)));
//G320: num(234)             => digits(239).
	p(NT(234), (NT(239)));
//G321: constant(159)        => binding(240).
	p(NT(159), (NT(240)));
//G322: constant(159)        => capture(70).
	p(NT(159), (NT(70)));
//G323: __E___E_binding_96_97(243) => chars(228) _(16).
	p(NT(243), (NT(228)+NT(16)));
//G324: __E___E_binding_96_97(243) => _(16).
	p(NT(243), (NT(16)));
//G325: type(242)            => __E___E_binding_96_97(243).
	p(NT(242), (NT(243)));
//G326: __E_binding_96(241)  => type(242) ':' _(16) source(244).
	p(NT(241), (NT(242)+T(3)+NT(16)+NT(244)));
//G327: source_binding(46)   => __E_binding_96(241).
	p(NT(46), (NT(241)));
//G328: binding(240)         => source_binding(46).
	p(NT(240), (NT(46)));
//G329: named_binding(245)   => chars(228).
	p(NT(245), (NT(228)));
//G330: binding(240)         => named_binding(245).
	p(NT(240), (NT(245)));
//G331: __E_source_98(247)   => alnum(6).
	p(NT(247), (NT(6)));
//G332: __E_source_98(247)   => punct(7).
	p(NT(247), (NT(7)));
//G333: src_c(246)           => __E_source_98(247).
	p(NT(246), (NT(247)));
//G334: __E___E_source_99_100(249) => src_c(246).
	p(NT(249), (NT(246)));
//G335: __E___E_source_99_100(249) => space(2).
	p(NT(249), (NT(2)));
//G336: __E___E_source_99_101(250) => null.
	p(NT(250), (nul));
//G337: __E___E_source_99_101(250) => __E___E_source_99_100(249) __E___E_source_99_101(250).
	p(NT(250), (NT(249)+NT(250)));
//G338: __E_source_99(248)   => __E___E_source_99_101(250) src_c(246).
	p(NT(248), (NT(250)+NT(246)));
//G339: __E_source_99(248)   => null.
	p(NT(248), (nul));
//G340: source(244)          => src_c(246) __E_source_99(248).
	p(NT(244), (NT(246)+NT(248)));
//G341: __E_chars_102(251)   => null.
	p(NT(251), (nul));
//G342: __E_chars_102(251)   => alnum(6) __E_chars_102(251).
	p(NT(251), (NT(6)+NT(251)));
//G343: chars(228)           => alpha(5) __E_chars_102(251).
	p(NT(228), (NT(5)+NT(251)));
//G344: __E_digits_103(252)  => digit(3).
	p(NT(252), (NT(3)));
//G345: __E_digits_103(252)  => digit(3) __E_digits_103(252).
	p(NT(252), (NT(3)+NT(252)));
//G346: digits(239)          => __E_digits_103(252).
	p(NT(239), (NT(252)));
//G347: sym(85)              => chars(228).
	p(NT(85), (NT(228)));
//G348: __E_comment_104(254) => printable(8).
	p(NT(254), (NT(8)));
//G349: __E_comment_104(254) => '\t'.
	p(NT(254), (T(52)));
//G350: __E_comment_105(255) => null.
	p(NT(255), (nul));
//G351: __E_comment_105(255) => __E_comment_104(254) __E_comment_105(255).
	p(NT(255), (NT(254)+NT(255)));
//G352: __E_comment_106(256) => '\n'.
	p(NT(256), (T(53)));
//G353: __E_comment_106(256) => '\r'.
	p(NT(256), (T(54)));
//G354: __E_comment_106(256) => eof(1).
	p(NT(256), (NT(1)));
//G355: comment(253)         => '#' __E_comment_105(255) __E_comment_106(256).
	p(NT(253), (T(55)+NT(255)+NT(256)));
//G356: __E____107(257)      => space(2).
	p(NT(257), (NT(2)));
//G357: __E____107(257)      => comment(253).
	p(NT(257), (NT(253)));
//G358: __(72)               => __E____107(257) _(16).
	p(NT(72), (NT(257)+NT(16)));
//G359: __E___108(258)       => __(72).
	p(NT(258), (NT(72)));
//G360: __E___108(258)       => null.
	p(NT(258), (nul));
//G361: _(16)                => __E___108(258).
	p(NT(16), (NT(258)));
//G362: __E_cli_109(261)     => '.' _(16) cli_command(260) _(16).
	p(NT(261), (T(2)+NT(16)+NT(260)+NT(16)));
//G363: __E_cli_110(262)     => null.
	p(NT(262), (nul));
//G364: __E_cli_110(262)     => __E_cli_109(261) __E_cli_110(262).
	p(NT(262), (NT(261)+NT(262)));
//G365: cli(259)             => _(16) cli_command(260) _(16) __E_cli_110(262).
	p(NT(259), (NT(16)+NT(260)+NT(16)+NT(262)));
//G366: __E_cli_command_111(265) => 'q'.
	p(NT(265), (T(43)));
//G367: __E_cli_command_111(265) => 'q' 'u' 'i' 't'.
	p(NT(265), (T(43)+T(33)+T(37)+T(32)));
//G368: quit_sym(264)        => __E_cli_command_111(265).
	p(NT(264), (NT(265)));
//G369: quit_cmd(263)        => quit_sym(264).
	p(NT(263), (NT(264)));
//G370: cli_command(260)     => quit_cmd(263).
	p(NT(260), (NT(263)));
//G371: __E_cli_command_112(268) => 'v'.
	p(NT(268), (T(38)));
//G372: __E_cli_command_112(268) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(268), (T(38)+T(19)+T(40)+T(36)+T(37)+T(15)+T(44)));
//G373: version_sym(267)     => __E_cli_command_112(268).
	p(NT(267), (NT(268)));
//G374: version_cmd(266)     => version_sym(267).
	p(NT(266), (NT(267)));
//G375: cli_command(260)     => version_cmd(266).
	p(NT(260), (NT(266)));
//G376: __E_cli_command_113(271) => 'c'.
	p(NT(271), (T(34)));
//G377: __E_cli_command_113(271) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(271), (T(34)+T(16)+T(19)+T(18)+T(40)));
//G378: clear_sym(270)       => __E_cli_command_113(271).
	p(NT(270), (NT(271)));
//G379: clear_cmd(269)       => clear_sym(270).
	p(NT(269), (NT(270)));
//G380: cli_command(260)     => clear_cmd(269).
	p(NT(260), (NT(269)));
//G381: __E___E_cli_command_114_115(275) => 'h'.
	p(NT(275), (T(45)));
//G382: __E___E_cli_command_114_115(275) => 'h' 'e' 'l' 'p'.
	p(NT(275), (T(45)+T(19)+T(16)+T(35)));
//G383: help_sym(274)        => __E___E_cli_command_114_115(275).
	p(NT(274), (NT(275)));
//G384: __E___E_cli_command_114_116(276) => __(72) help_arg(277).
	p(NT(276), (NT(72)+NT(277)));
//G385: __E___E_cli_command_114_116(276) => null.
	p(NT(276), (nul));
//G386: __E_cli_command_114(273) => help_sym(274) __E___E_cli_command_114_116(276).
	p(NT(273), (NT(274)+NT(276)));
//G387: help_cmd(272)        => __E_cli_command_114(273).
	p(NT(272), (NT(273)));
//G388: cli_command(260)     => help_cmd(272).
	p(NT(260), (NT(272)));
//G389: file_sym(280)        => 'f' 'i' 'l' 'e'.
	p(NT(280), (T(42)+T(37)+T(16)+T(19)));
//G390: __E_cli_command_117(279) => file_sym(280) __(72) q_string(281).
	p(NT(279), (NT(280)+NT(72)+NT(281)));
//G391: file_cmd(278)        => __E_cli_command_117(279).
	p(NT(278), (NT(279)));
//G392: cli_command(260)     => file_cmd(278).
	p(NT(260), (NT(278)));
//G393: valid_sym(284)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(284), (T(38)+T(18)+T(16)+T(37)+T(41)));
//G394: __E_cli_command_118(283) => valid_sym(284) __(72) gssotc_rr(15).
	p(NT(283), (NT(284)+NT(72)+NT(15)));
//G395: valid_cmd(282)       => __E_cli_command_118(283).
	p(NT(282), (NT(283)));
//G396: cli_command(260)     => valid_cmd(282).
	p(NT(260), (NT(282)));
//G397: sat_sym(287)         => 's' 'a' 't'.
	p(NT(287), (T(36)+T(18)+T(32)));
//G398: __E_cli_command_119(286) => sat_sym(287) __(72) gssotc_rr(15).
	p(NT(286), (NT(287)+NT(72)+NT(15)));
//G399: sat_cmd(285)         => __E_cli_command_119(286).
	p(NT(285), (NT(286)));
//G400: cli_command(260)     => sat_cmd(285).
	p(NT(260), (NT(285)));
//G401: unsat_sym(290)       => 'u' 'n' 's' 'a' 't'.
	p(NT(290), (T(33)+T(44)+T(36)+T(18)+T(32)));
//G402: __E_cli_command_120(289) => unsat_sym(290) __(72) gssotc_rr(15).
	p(NT(289), (NT(290)+NT(72)+NT(15)));
//G403: unsat_cmd(288)       => __E_cli_command_120(289).
	p(NT(288), (NT(289)));
//G404: cli_command(260)     => unsat_cmd(288).
	p(NT(260), (NT(288)));
//G405: solve_sym(293)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(293), (T(36)+T(15)+T(16)+T(38)+T(19)));
//G406: solve_cmd_arg(294)   => wff_cmd_arg(295).
	p(NT(294), (NT(295)));
//G407: __E_cli_command_121(292) => solve_sym(293) __(72) solve_cmd_arg(294).
	p(NT(292), (NT(293)+NT(72)+NT(294)));
//G408: solve_cmd(291)       => __E_cli_command_121(292).
	p(NT(291), (NT(292)));
//G409: cli_command(260)     => solve_cmd(291).
	p(NT(260), (NT(291)));
//G410: __E___E_cli_command_122_123(299) => 'e'.
	p(NT(299), (T(19)));
//G411: __E___E_cli_command_122_123(299) => 'e' 'x' 'e' 'c' 'u' 't' 'e'.
	p(NT(299), (T(19)+T(20)+T(19)+T(34)+T(33)+T(32)+T(19)));
//G412: execute_sym(298)     => __E___E_cli_command_122_123(299).
	p(NT(298), (NT(299)));
//G413: __E___E_cli_command_122_124(301) => tau(20).
	p(NT(301), (NT(20)));
//G414: __E___E_cli_command_122_124(301) => gssotc_rr(15).
	p(NT(301), (NT(15)));
//G415: execute_cmd_arg(300) => __E___E_cli_command_122_124(301).
	p(NT(300), (NT(301)));
//G416: __E_cli_command_122(297) => execute_sym(298) __(72) execute_cmd_arg(300).
	p(NT(297), (NT(298)+NT(72)+NT(300)));
//G417: execute_cmd(296)     => __E_cli_command_122(297).
	p(NT(296), (NT(297)));
//G418: cli_command(260)     => execute_cmd(296).
	p(NT(260), (NT(296)));
//G419: __E___E_cli_command_125_126(305) => 'n'.
	p(NT(305), (T(44)));
//G420: __E___E_cli_command_125_126(305) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(305), (T(44)+T(15)+T(40)+T(47)+T(18)+T(16)+T(37)+T(48)+T(19)));
//G421: normalize_sym(304)   => __E___E_cli_command_125_126(305).
	p(NT(304), (NT(305)));
//G422: __E_cli_command_125(303) => normalize_sym(304) __(72) normalize_cmd_arg(306).
	p(NT(303), (NT(304)+NT(72)+NT(306)));
//G423: normalize_cmd(302)   => __E_cli_command_125(303).
	p(NT(302), (NT(303)));
//G424: cli_command(260)     => normalize_cmd(302).
	p(NT(260), (NT(302)));
//G425: __E___E_cli_command_127_128(310) => 'i'.
	p(NT(310), (T(37)));
//G426: __E___E_cli_command_127_128(310) => 'i' 'n' 's' 't'.
	p(NT(310), (T(37)+T(44)+T(36)+T(32)));
//G427: __E___E_cli_command_127_128(310) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(310), (T(37)+T(44)+T(36)+T(32)+T(18)+T(44)+T(32)+T(37)+T(18)+T(32)+T(19)));
//G428: inst_sym(309)        => __E___E_cli_command_127_128(310).
	p(NT(309), (NT(310)));
//G429: __E_cli_command_127(308) => inst_sym(309) __(72) variable(162) __(72) wff_cmd_arg(295) __(72) wff_cmd_out_arg(311).
	p(NT(308), (NT(309)+NT(72)+NT(162)+NT(72)+NT(295)+NT(72)+NT(311)));
//G430: wff_inst_cmd(307)    => __E_cli_command_127(308).
	p(NT(307), (NT(308)));
//G431: cli_command(260)     => wff_inst_cmd(307).
	p(NT(260), (NT(307)));
//G432: __E___E_cli_command_129_130(315) => 's'.
	p(NT(315), (T(36)));
//G433: __E___E_cli_command_129_130(315) => 's' 'u' 'b' 's' 't'.
	p(NT(315), (T(36)+T(33)+T(14)+T(36)+T(32)));
//G434: __E___E_cli_command_129_130(315) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(315), (T(36)+T(33)+T(14)+T(36)+T(32)+T(37)+T(32)+T(33)+T(32)+T(19)));
//G435: subst_sym(314)       => __E___E_cli_command_129_130(315).
	p(NT(314), (NT(315)));
//G436: __E_cli_command_129(313) => subst_sym(314) __(72) wff_cmd_arg(295) __(72) wff_cmd_arg(295) __(72) wff_cmd_out_arg(311).
	p(NT(313), (NT(314)+NT(72)+NT(295)+NT(72)+NT(295)+NT(72)+NT(311)));
//G437: wff_subst_cmd(312)   => __E_cli_command_129(313).
	p(NT(312), (NT(313)));
//G438: cli_command(260)     => wff_subst_cmd(312).
	p(NT(260), (NT(312)));
//G439: __E_cli_command_131(317) => inst_sym(309) __(72) variable(162) __(72) bf_cmd_arg(318) __(72) bf_cmd_out_arg(319).
	p(NT(317), (NT(309)+NT(72)+NT(162)+NT(72)+NT(318)+NT(72)+NT(319)));
//G440: bf_inst_cmd(316)     => __E_cli_command_131(317).
	p(NT(316), (NT(317)));
//G441: cli_command(260)     => bf_inst_cmd(316).
	p(NT(260), (NT(316)));
//G442: __E_cli_command_132(321) => subst_sym(314) __(72) bf_cmd_arg(318) __(72) bf_cmd_arg(318) __(72) bf_cmd_out_arg(319).
	p(NT(321), (NT(314)+NT(72)+NT(318)+NT(72)+NT(318)+NT(72)+NT(319)));
//G443: bf_subst_cmd(320)    => __E_cli_command_132(321).
	p(NT(320), (NT(321)));
//G444: cli_command(260)     => bf_subst_cmd(320).
	p(NT(260), (NT(320)));
//G445: dnf_sym(324)         => 'd' 'n' 'f'.
	p(NT(324), (T(41)+T(44)+T(42)));
//G446: __E_cli_command_133(323) => dnf_sym(324) __(72) nf_cmd_arg(325).
	p(NT(323), (NT(324)+NT(72)+NT(325)));
//G447: dnf_cmd(322)         => __E_cli_command_133(323).
	p(NT(322), (NT(323)));
//G448: cli_command(260)     => dnf_cmd(322).
	p(NT(260), (NT(322)));
//G449: cnf_sym(328)         => 'c' 'n' 'f'.
	p(NT(328), (T(34)+T(44)+T(42)));
//G450: __E_cli_command_134(327) => cnf_sym(328) __(72) nf_cmd_arg(325).
	p(NT(327), (NT(328)+NT(72)+NT(325)));
//G451: cnf_cmd(326)         => __E_cli_command_134(327).
	p(NT(326), (NT(327)));
//G452: cli_command(260)     => cnf_cmd(326).
	p(NT(260), (NT(326)));
//G453: anf_sym(331)         => 'a' 'n' 'f'.
	p(NT(331), (T(18)+T(44)+T(42)));
//G454: __E_cli_command_135(330) => anf_sym(331) __(72) nf_cmd_arg(325).
	p(NT(330), (NT(331)+NT(72)+NT(325)));
//G455: anf_cmd(329)         => __E_cli_command_135(330).
	p(NT(329), (NT(330)));
//G456: cli_command(260)     => anf_cmd(329).
	p(NT(260), (NT(329)));
//G457: nnf_sym(334)         => 'n' 'n' 'f'.
	p(NT(334), (T(44)+T(44)+T(42)));
//G458: __E_cli_command_136(333) => nnf_sym(334) __(72) nf_cmd_arg(325).
	p(NT(333), (NT(334)+NT(72)+NT(325)));
//G459: nnf_cmd(332)         => __E_cli_command_136(333).
	p(NT(332), (NT(333)));
//G460: cli_command(260)     => nnf_cmd(332).
	p(NT(260), (NT(332)));
//G461: pnf_sym(337)         => 'p' 'n' 'f'.
	p(NT(337), (T(35)+T(44)+T(42)));
//G462: __E_cli_command_137(336) => pnf_sym(337) __(72) nf_cmd_arg(325).
	p(NT(336), (NT(337)+NT(72)+NT(325)));
//G463: pnf_cmd(335)         => __E_cli_command_137(336).
	p(NT(335), (NT(336)));
//G464: cli_command(260)     => pnf_cmd(335).
	p(NT(260), (NT(335)));
//G465: mnf_sym(340)         => 'm' 'n' 'f'.
	p(NT(340), (T(47)+T(44)+T(42)));
//G466: __E_cli_command_138(339) => mnf_sym(340) __(72) nf_cmd_arg(325).
	p(NT(339), (NT(340)+NT(72)+NT(325)));
//G467: mnf_cmd(338)         => __E_cli_command_138(339).
	p(NT(338), (NT(339)));
//G468: cli_command(260)     => mnf_cmd(338).
	p(NT(260), (NT(338)));
//G469: snf_sym(343)         => 's' 'n' 'f'.
	p(NT(343), (T(36)+T(44)+T(42)));
//G470: __E_cli_command_139(342) => snf_sym(343) __(72) nf_cmd_arg(325).
	p(NT(342), (NT(343)+NT(72)+NT(325)));
//G471: snf_cmd(341)         => __E_cli_command_139(342).
	p(NT(341), (NT(342)));
//G472: cli_command(260)     => snf_cmd(341).
	p(NT(260), (NT(341)));
//G473: onf_sym(346)         => 'o' 'n' 'f'.
	p(NT(346), (T(15)+T(44)+T(42)));
//G474: __E_cli_command_140(345) => onf_sym(346) __(72) variable(162) __(72) onf_cmd_arg(347).
	p(NT(345), (NT(346)+NT(72)+NT(162)+NT(72)+NT(347)));
//G475: onf_cmd(344)         => __E_cli_command_140(345).
	p(NT(344), (NT(345)));
//G476: cli_command(260)     => onf_cmd(344).
	p(NT(260), (NT(344)));
//G477: def_sym(350)         => 'd' 'e' 'f'.
	p(NT(350), (T(41)+T(19)+T(42)));
//G478: __E_cli_command_141(349) => def_sym(350).
	p(NT(349), (NT(350)));
//G479: def_list_cmd(348)    => __E_cli_command_141(349).
	p(NT(348), (NT(349)));
//G480: cli_command(260)     => def_list_cmd(348).
	p(NT(260), (NT(348)));
//G481: __E_cli_command_142(352) => def_sym(350) __(72) gssotc_rec_relation_form(24).
	p(NT(352), (NT(350)+NT(72)+NT(24)));
//G482: def_rr_cmd(351)      => __E_cli_command_142(352).
	p(NT(351), (NT(352)));
//G483: cli_command(260)     => def_rr_cmd(351).
	p(NT(260), (NT(351)));
//G484: __E___E_cli_command_143_144(356) => 'd'.
	p(NT(356), (T(41)));
//G485: __E___E_cli_command_143_144(356) => 'd' 'e' 'l'.
	p(NT(356), (T(41)+T(19)+T(16)));
//G486: __E___E_cli_command_143_144(356) => 'd' 'e' 'l' 'e' 't' 'e'.
	p(NT(356), (T(41)+T(19)+T(16)+T(19)+T(32)+T(19)));
//G487: del_sym(355)         => __E___E_cli_command_143_144(356).
	p(NT(355), (NT(356)));
//G488: __E_cli_command_143(354) => def_sym(350) __(72) del_sym(355) __(72) memory(357).
	p(NT(354), (NT(350)+NT(72)+NT(355)+NT(72)+NT(357)));
//G489: def_del_cmd(353)     => __E_cli_command_143(354).
	p(NT(353), (NT(354)));
//G490: cli_command(260)     => def_del_cmd(353).
	p(NT(260), (NT(353)));
//G491: __E_cli_command_145(359) => def_sym(350) __(72) clear_sym(270).
	p(NT(359), (NT(350)+NT(72)+NT(270)));
//G492: def_clear_cmd(358)   => __E_cli_command_145(359).
	p(NT(358), (NT(359)));
//G493: cli_command(260)     => def_clear_cmd(358).
	p(NT(260), (NT(358)));
//G494: __E_cli_command_146(361) => def_sym(350) __(72) memory(357).
	p(NT(361), (NT(350)+NT(72)+NT(357)));
//G495: def_print_cmd(360)   => __E_cli_command_146(361).
	p(NT(360), (NT(361)));
//G496: cli_command(260)     => def_print_cmd(360).
	p(NT(260), (NT(360)));
//G497: __E___E_cli_command_147_148(365) => 'm'.
	p(NT(365), (T(47)));
//G498: __E___E_cli_command_147_148(365) => 'm' 'e' 'm'.
	p(NT(365), (T(47)+T(19)+T(47)));
//G499: __E___E_cli_command_147_148(365) => 'm' 'e' 'm' 'o' 'r' 'y'.
	p(NT(365), (T(47)+T(19)+T(47)+T(15)+T(40)+T(56)));
//G500: memory_sym(364)      => __E___E_cli_command_147_148(365).
	p(NT(364), (NT(365)));
//G501: __E_cli_command_147(363) => memory_sym(364).
	p(NT(363), (NT(364)));
//G502: memory_list_cmd(362) => __E_cli_command_147(363).
	p(NT(362), (NT(363)));
//G503: cli_command(260)     => memory_list_cmd(362).
	p(NT(260), (NT(362)));
//G504: __E_cli_command_149(367) => memory_sym(364) __(72) del_sym(355) __(72) memory(357).
	p(NT(367), (NT(364)+NT(72)+NT(355)+NT(72)+NT(357)));
//G505: memory_del_cmd(366)  => __E_cli_command_149(367).
	p(NT(366), (NT(367)));
//G506: cli_command(260)     => memory_del_cmd(366).
	p(NT(260), (NT(366)));
//G507: __E_cli_command_150(369) => memory_sym(364) __(72) clear_sym(270).
	p(NT(369), (NT(364)+NT(72)+NT(270)));
//G508: memory_clear_cmd(368) => __E_cli_command_150(369).
	p(NT(368), (NT(369)));
//G509: cli_command(260)     => memory_clear_cmd(368).
	p(NT(260), (NT(368)));
//G510: __E_cli_command_151(371) => memory_sym(364) __(72) memory(357).
	p(NT(371), (NT(364)+NT(72)+NT(357)));
//G511: memory_print_cmd(370) => __E_cli_command_151(371).
	p(NT(370), (NT(371)));
//G512: cli_command(260)     => memory_print_cmd(370).
	p(NT(260), (NT(370)));
//G513: __E___E_cli_command_152_153(375) => bf(41).
	p(NT(375), (NT(41)));
//G514: __E___E_cli_command_152_153(375) => wff(32).
	p(NT(375), (NT(32)));
//G515: __E___E_cli_command_152_153(375) => tau(20).
	p(NT(375), (NT(20)));
//G516: memory_store_cmd_arg(374) => __E___E_cli_command_152_153(375).
	p(NT(374), (NT(375)));
//G517: __E_cli_command_152(373) => memory_sym(364) __(72) memory_store_cmd_arg(374).
	p(NT(373), (NT(364)+NT(72)+NT(374)));
//G518: memory_store_cmd(372) => __E_cli_command_152(373).
	p(NT(372), (NT(373)));
//G519: cli_command(260)     => memory_store_cmd(372).
	p(NT(260), (NT(372)));
//G520: qelim_sym(378)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(378), (T(43)+T(19)+T(16)+T(37)+T(47)));
//G521: __E_cli_command_154(377) => qelim_sym(378) __(72) wff_cmd_arg(295).
	p(NT(377), (NT(378)+NT(72)+NT(295)));
//G522: qelim_cmd(376)       => __E_cli_command_154(377).
	p(NT(376), (NT(377)));
//G523: cli_command(260)     => qelim_cmd(376).
	p(NT(260), (NT(376)));
//G524: get_sym(381)         => 'g' 'e' 't'.
	p(NT(381), (T(46)+T(19)+T(32)));
//G525: __E___E_cli_command_155_156(382) => __(72) option(383).
	p(NT(382), (NT(72)+NT(383)));
//G526: __E___E_cli_command_155_156(382) => null.
	p(NT(382), (nul));
//G527: __E_cli_command_155(380) => get_sym(381) __E___E_cli_command_155_156(382).
	p(NT(380), (NT(381)+NT(382)));
//G528: get_cmd(379)         => __E_cli_command_155(380).
	p(NT(379), (NT(380)));
//G529: cli_command(260)     => get_cmd(379).
	p(NT(260), (NT(379)));
//G530: set_sym(386)         => 's' 'e' 't'.
	p(NT(386), (T(36)+T(19)+T(32)));
//G531: __E___E_cli_command_157_158(387) => __(72).
	p(NT(387), (NT(72)));
//G532: __E___E_cli_command_157_158(387) => _(16) '=' _(16).
	p(NT(387), (NT(16)+T(4)+NT(16)));
//G533: __E_cli_command_157(385) => set_sym(386) __(72) option(383) __E___E_cli_command_157_158(387) option_value(388).
	p(NT(385), (NT(386)+NT(72)+NT(383)+NT(387)+NT(388)));
//G534: set_cmd(384)         => __E_cli_command_157(385).
	p(NT(384), (NT(385)));
//G535: cli_command(260)     => set_cmd(384).
	p(NT(260), (NT(384)));
//G536: toggle_sym(391)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(391), (T(32)+T(15)+T(46)+T(46)+T(16)+T(19)));
//G537: __E_cli_command_159(390) => toggle_sym(391) __(72) bool_option(392).
	p(NT(390), (NT(391)+NT(72)+NT(392)));
//G538: toggle_cmd(389)      => __E_cli_command_159(390).
	p(NT(389), (NT(390)));
//G539: cli_command(260)     => toggle_cmd(389).
	p(NT(260), (NT(389)));
//G540: bf_cmd_arg(318)      => memory(357).
	p(NT(318), (NT(357)));
//G541: bf_cmd_arg(318)      => bf_selection(393).
	p(NT(318), (NT(393)));
//G542: bf_cmd_arg(318)      => bf(41).
	p(NT(318), (NT(41)));
//G543: bf_cmd_out_arg(319)  => memory(357).
	p(NT(319), (NT(357)));
//G544: bf_cmd_out_arg(319)  => bf_selection(393).
	p(NT(319), (NT(393)));
//G545: wff_cmd_arg(295)     => memory(357).
	p(NT(295), (NT(357)));
//G546: wff_cmd_arg(295)     => wff_selection(394).
	p(NT(295), (NT(394)));
//G547: wff_cmd_arg(295)     => wff(32).
	p(NT(295), (NT(32)));
//G548: wff_cmd_out_arg(311) => memory(357).
	p(NT(311), (NT(357)));
//G549: wff_cmd_out_arg(311) => wff_selection(394).
	p(NT(311), (NT(394)));
//G550: nf_cmd_arg(325)      => memory(357).
	p(NT(325), (NT(357)));
//G551: nf_cmd_arg(325)      => bf(41).
	p(NT(325), (NT(41)));
//G552: nf_cmd_arg(325)      => wff(32).
	p(NT(325), (NT(32)));
//G553: onf_cmd_arg(347)     => memory(357).
	p(NT(347), (NT(357)));
//G554: onf_cmd_arg(347)     => wff(32).
	p(NT(347), (NT(32)));
//G555: normalize_cmd_arg(306) => memory(357).
	p(NT(306), (NT(357)));
//G556: normalize_cmd_arg(306) => bf(41).
	p(NT(306), (NT(41)));
//G557: normalize_cmd_arg(306) => wff(32).
	p(NT(306), (NT(32)));
//G558: normalize_cmd_arg(306) => nso_rr(14).
	p(NT(306), (NT(14)));
//G559: help_arg(277)        => help_sym(274).
	p(NT(277), (NT(274)));
//G560: help_arg(277)        => version_sym(267).
	p(NT(277), (NT(267)));
//G561: help_arg(277)        => quit_sym(264).
	p(NT(277), (NT(264)));
//G562: help_arg(277)        => clear_sym(270).
	p(NT(277), (NT(270)));
//G563: help_arg(277)        => get_sym(381).
	p(NT(277), (NT(381)));
//G564: help_arg(277)        => set_sym(386).
	p(NT(277), (NT(386)));
//G565: help_arg(277)        => toggle_sym(391).
	p(NT(277), (NT(391)));
//G566: help_arg(277)        => file_sym(280).
	p(NT(277), (NT(280)));
//G567: help_arg(277)        => memory_sym(364).
	p(NT(277), (NT(364)));
//G568: help_arg(277)        => abs_memory_sym(395).
	p(NT(277), (NT(395)));
//G569: help_arg(277)        => rel_memory_sym(396).
	p(NT(277), (NT(396)));
//G570: help_arg(277)        => selection_sym(397).
	p(NT(277), (NT(397)));
//G571: help_arg(277)        => def_sym(350).
	p(NT(277), (NT(350)));
//G572: help_arg(277)        => inst_sym(309).
	p(NT(277), (NT(309)));
//G573: help_arg(277)        => subst_sym(314).
	p(NT(277), (NT(314)));
//G574: help_arg(277)        => normalize_sym(304).
	p(NT(277), (NT(304)));
//G575: help_arg(277)        => execute_sym(298).
	p(NT(277), (NT(298)));
//G576: help_arg(277)        => solve_sym(293).
	p(NT(277), (NT(293)));
//G577: help_arg(277)        => valid_sym(284).
	p(NT(277), (NT(284)));
//G578: help_arg(277)        => sat_sym(287).
	p(NT(277), (NT(287)));
//G579: help_arg(277)        => unsat_sym(290).
	p(NT(277), (NT(290)));
//G580: help_arg(277)        => dnf_sym(324).
	p(NT(277), (NT(324)));
//G581: help_arg(277)        => cnf_sym(328).
	p(NT(277), (NT(328)));
//G582: help_arg(277)        => anf_sym(331).
	p(NT(277), (NT(331)));
//G583: help_arg(277)        => snf_sym(343).
	p(NT(277), (NT(343)));
//G584: help_arg(277)        => nnf_sym(334).
	p(NT(277), (NT(334)));
//G585: help_arg(277)        => pnf_sym(337).
	p(NT(277), (NT(337)));
//G586: help_arg(277)        => mnf_sym(340).
	p(NT(277), (NT(340)));
//G587: help_arg(277)        => onf_sym(346).
	p(NT(277), (NT(346)));
//G588: help_arg(277)        => qelim_sym(378).
	p(NT(277), (NT(378)));
//G589: __E___E_help_arg_160_161(400) => 's'.
	p(NT(400), (T(36)));
//G590: __E___E_help_arg_160_161(400) => null.
	p(NT(400), (nul));
//G591: __E_help_arg_160(399) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_160_161(400).
	p(NT(399), (T(19)+T(20)+T(18)+T(47)+T(35)+T(16)+T(19)+NT(400)));
//G592: examples_sym(398)    => __E_help_arg_160(399).
	p(NT(398), (NT(399)));
//G593: help_arg(277)        => examples_sym(398).
	p(NT(277), (NT(398)));
//G594: rel_memory_sym(396)  => '%'.
	p(NT(396), (T(57)));
//G595: memory_id(404)       => digits(239).
	p(NT(404), (NT(239)));
//G596: __E___E_memory_162_163(403) => memory_id(404).
	p(NT(403), (NT(404)));
//G597: __E___E_memory_162_163(403) => null.
	p(NT(403), (nul));
//G598: __E_memory_162(402)  => rel_memory_sym(396) __E___E_memory_162_163(403).
	p(NT(402), (NT(396)+NT(403)));
//G599: rel_memory(401)      => __E_memory_162(402).
	p(NT(401), (NT(402)));
//G600: memory(357)          => rel_memory(401).
	p(NT(357), (NT(401)));
//G601: abs_memory_sym(395)  => '%' '-'.
	p(NT(395), (T(57)+T(21)));
//G602: __E___E_memory_164_165(407) => abs_memory_sym(395).
	p(NT(407), (NT(395)));
//G603: __E___E_memory_164_165(407) => memory_sym(364) __(72).
	p(NT(407), (NT(364)+NT(72)));
//G604: __E_memory_164(406)  => __E___E_memory_164_165(407) memory_id(404).
	p(NT(406), (NT(407)+NT(404)));
//G605: abs_memory(405)      => __E_memory_164(406).
	p(NT(405), (NT(406)));
//G606: memory(357)          => abs_memory(405).
	p(NT(357), (NT(405)));
//G607: selection_sym(397)   => 's'.
	p(NT(397), (T(36)));
//G608: selection_sym(397)   => 's' 'e' 'l' 'e' 'c' 't' 'i' 'o' 'n'.
	p(NT(397), (T(36)+T(19)+T(16)+T(19)+T(34)+T(32)+T(37)+T(15)+T(44)));
//G609: wff_selection(394)   => selection_sym(397) __(72) digits(239).
	p(NT(394), (NT(397)+NT(72)+NT(239)));
//G610: bf_selection(393)    => selection_sym(397) __(72) digits(239).
	p(NT(393), (NT(397)+NT(72)+NT(239)));
//G611: bf_var_selection(408) => selection_sym(397) __(72) digits(239).
	p(NT(408), (NT(397)+NT(72)+NT(239)));
//G612: wff_var_selection(409) => selection_sym(397) __(72) digits(239).
	p(NT(409), (NT(397)+NT(72)+NT(239)));
//G613: option(383)          => bool_option(392).
	p(NT(383), (NT(392)));
//G614: __E_option_166(412)  => 's' 'e' 'v'.
	p(NT(412), (T(36)+T(19)+T(38)));
//G615: __E_option_166(412)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(412), (T(36)+T(19)+T(38)+T(19)+T(40)+T(37)+T(32)+T(56)));
//G616: severity_opt(411)    => __E_option_166(412).
	p(NT(411), (NT(412)));
//G617: enum_option(410)     => severity_opt(411).
	p(NT(410), (NT(411)));
//G618: option(383)          => enum_option(410).
	p(NT(383), (NT(410)));
//G619: __E_bool_option_167(414) => 's'.
	p(NT(414), (T(36)));
//G620: __E_bool_option_167(414) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(414), (T(36)+T(32)+T(18)+T(32)+T(33)+T(36)));
//G621: status_opt(413)      => __E_bool_option_167(414).
	p(NT(413), (NT(414)));
//G622: bool_option(392)     => status_opt(413).
	p(NT(392), (NT(413)));
//G623: __E_bool_option_168(416) => 'c'.
	p(NT(416), (T(34)));
//G624: __E_bool_option_168(416) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(416), (T(34)+T(15)+T(16)+T(15)+T(40)));
//G625: __E_bool_option_168(416) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(416), (T(34)+T(15)+T(16)+T(15)+T(40)+T(36)));
//G626: colors_opt(415)      => __E_bool_option_168(416).
	p(NT(415), (NT(416)));
//G627: bool_option(392)     => colors_opt(415).
	p(NT(392), (NT(415)));
//G628: __E_bool_option_169(418) => 'd'.
	p(NT(418), (T(41)));
//G629: __E_bool_option_169(418) => 'd' 'b' 'g'.
	p(NT(418), (T(41)+T(14)+T(46)));
//G630: __E_bool_option_169(418) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(418), (T(41)+T(19)+T(14)+T(33)+T(46)));
//G631: debug_repl_opt(417)  => __E_bool_option_169(418).
	p(NT(417), (NT(418)));
//G632: bool_option(392)     => debug_repl_opt(417).
	p(NT(392), (NT(417)));
//G633: option_value(388)    => option_value_true(419).
	p(NT(388), (NT(419)));
//G634: option_value(388)    => option_value_false(420).
	p(NT(388), (NT(420)));
//G635: option_value(388)    => severity(421).
	p(NT(388), (NT(421)));
//G636: option_value_true(419) => 't'.
	p(NT(419), (T(32)));
//G637: option_value_true(419) => 't' 'r' 'u' 'e'.
	p(NT(419), (T(32)+T(40)+T(33)+T(19)));
//G638: option_value_true(419) => 'o' 'n'.
	p(NT(419), (T(15)+T(44)));
//G639: option_value_true(419) => '1'.
	p(NT(419), (T(29)));
//G640: option_value_true(419) => 'y'.
	p(NT(419), (T(56)));
//G641: option_value_true(419) => 'y' 'e' 's'.
	p(NT(419), (T(56)+T(19)+T(36)));
//G642: option_value_false(420) => 'f'.
	p(NT(420), (T(42)));
//G643: option_value_false(420) => 'f' 'a' 'l' 's' 'e'.
	p(NT(420), (T(42)+T(18)+T(16)+T(36)+T(19)));
//G644: option_value_false(420) => 'o' 'f' 'f'.
	p(NT(420), (T(15)+T(42)+T(42)));
//G645: option_value_false(420) => '0'.
	p(NT(420), (T(30)));
//G646: option_value_false(420) => 'n'.
	p(NT(420), (T(44)));
//G647: option_value_false(420) => 'n' 'o'.
	p(NT(420), (T(44)+T(15)));
//G648: __E_severity_170(423) => 'e'.
	p(NT(423), (T(19)));
//G649: __E_severity_170(423) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(423), (T(19)+T(40)+T(40)+T(15)+T(40)));
//G650: error_sym(422)       => __E_severity_170(423).
	p(NT(422), (NT(423)));
//G651: severity(421)        => error_sym(422).
	p(NT(421), (NT(422)));
//G652: __E_severity_171(425) => 'i'.
	p(NT(425), (T(37)));
//G653: __E_severity_171(425) => 'i' 'n' 'f' 'o'.
	p(NT(425), (T(37)+T(44)+T(42)+T(15)));
//G654: info_sym(424)        => __E_severity_171(425).
	p(NT(424), (NT(425)));
//G655: severity(421)        => info_sym(424).
	p(NT(421), (NT(424)));
//G656: __E_severity_172(427) => 'd'.
	p(NT(427), (T(41)));
//G657: __E_severity_172(427) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(427), (T(41)+T(19)+T(14)+T(33)+T(46)));
//G658: debug_sym(426)       => __E_severity_172(427).
	p(NT(426), (NT(427)));
//G659: severity(421)        => debug_sym(426).
	p(NT(421), (NT(426)));
//G660: __E_severity_173(429) => 't'.
	p(NT(429), (T(32)));
//G661: __E_severity_173(429) => 't' 'r' 'a' 'c' 'e'.
	p(NT(429), (T(32)+T(40)+T(18)+T(34)+T(19)));
//G662: trace_sym(428)       => __E_severity_173(429).
	p(NT(428), (NT(429)));
//G663: severity(421)        => trace_sym(428).
	p(NT(421), (NT(428)));
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
		__E_wff_51, bool_variable, bf_parenthesis, __E_bf_52, bf_splitter, __E_bf_53, __E_bf_54, __E___E_bf_54_55, bf_or, __E_bf_56, 
		bf_xor, __E_bf_57, bf_and, __E_bf_58, __E___E_bf_58_59, bf_neg, __E_bf_60, bf_constant, __E_bf_61, constant, 
		bf_t, bf_f, variable, __E_ref_args_62, __E___E_ref_args_62_63, ref_arg, __E___E_ref_args_62_64, __E___E_ref_args_62_65, bf_cb_args1, bf_cb_arg, 
		bf_cb_args2, bf_cb_args3, bf_cb_args4, wff_cb_args1, wff_cb_arg, wff_cb_args2, wff_cb_args3, wff_cb_args4, tau_cb_args1, tau_cb_arg, 
		__E_tau_cb_args1_66, tau_cb_args2, tau_cb_args3, tau_collapse_positives_cb, __E_tau_cb_67, __E___E_tau_cb_67_68, tau_positives_upwards_cb, __E_tau_cb_69, bf_eq_cb, __E_wff_cb_70, 
		bf_neq_cb, __E_wff_cb_71, wff_has_clashing_subformulas_cb, __E_wff_cb_72, wff_has_subformula_cb, __E_wff_cb_73, wff_remove_existential_cb, __E_wff_cb_74, wff_remove_bexistential_cb, __E_wff_cb_75, 
		wff_remove_buniversal_cb, __E_wff_cb_76, bf_is_zero_cb, __E_bf_cb_77, bf_is_one_cb, __E_bf_cb_78, bf_has_subformula_cb, __E_bf_cb_79, bf_remove_funiversal_cb, __E_bf_cb_80, 
		bf_remove_fexistential_cb, __E_bf_cb_81, bf_and_cb, __E_bf_cb_82, bf_or_cb, __E_bf_cb_83, bf_xor_cb, __E_bf_cb_84, bf_neg_cb, __E_bf_cb_85, 
		charvar, __E_charvar_86, __E_charvar_87, io_var, __E_variable_88, out, in_var_name, __E_in_89, chars, offset, 
		out_var_name, __E_out_90, __E_offsets_91, __E_offsets_92, num, shift, __E_offset_93, __E_shift_94, __E___E_shift_94_95, digits, 
		binding, __E_binding_96, type, __E___E_binding_96_97, source, named_binding, src_c, __E_source_98, __E_source_99, __E___E_source_99_100, 
		__E___E_source_99_101, __E_chars_102, __E_digits_103, comment, __E_comment_104, __E_comment_105, __E_comment_106, __E____107, __E___108, cli, 
		cli_command, __E_cli_109, __E_cli_110, quit_cmd, quit_sym, __E_cli_command_111, version_cmd, version_sym, __E_cli_command_112, clear_cmd, 
		clear_sym, __E_cli_command_113, help_cmd, __E_cli_command_114, help_sym, __E___E_cli_command_114_115, __E___E_cli_command_114_116, help_arg, file_cmd, __E_cli_command_117, 
		file_sym, q_string, valid_cmd, __E_cli_command_118, valid_sym, sat_cmd, __E_cli_command_119, sat_sym, unsat_cmd, __E_cli_command_120, 
		unsat_sym, solve_cmd, __E_cli_command_121, solve_sym, solve_cmd_arg, wff_cmd_arg, execute_cmd, __E_cli_command_122, execute_sym, __E___E_cli_command_122_123, 
		execute_cmd_arg, __E___E_cli_command_122_124, normalize_cmd, __E_cli_command_125, normalize_sym, __E___E_cli_command_125_126, normalize_cmd_arg, wff_inst_cmd, __E_cli_command_127, inst_sym, 
		__E___E_cli_command_127_128, wff_cmd_out_arg, wff_subst_cmd, __E_cli_command_129, subst_sym, __E___E_cli_command_129_130, bf_inst_cmd, __E_cli_command_131, bf_cmd_arg, bf_cmd_out_arg, 
		bf_subst_cmd, __E_cli_command_132, dnf_cmd, __E_cli_command_133, dnf_sym, nf_cmd_arg, cnf_cmd, __E_cli_command_134, cnf_sym, anf_cmd, 
		__E_cli_command_135, anf_sym, nnf_cmd, __E_cli_command_136, nnf_sym, pnf_cmd, __E_cli_command_137, pnf_sym, mnf_cmd, __E_cli_command_138, 
		mnf_sym, snf_cmd, __E_cli_command_139, snf_sym, onf_cmd, __E_cli_command_140, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_141, 
		def_sym, def_rr_cmd, __E_cli_command_142, def_del_cmd, __E_cli_command_143, del_sym, __E___E_cli_command_143_144, memory, def_clear_cmd, __E_cli_command_145, 
		def_print_cmd, __E_cli_command_146, memory_list_cmd, __E_cli_command_147, memory_sym, __E___E_cli_command_147_148, memory_del_cmd, __E_cli_command_149, memory_clear_cmd, __E_cli_command_150, 
		memory_print_cmd, __E_cli_command_151, memory_store_cmd, __E_cli_command_152, memory_store_cmd_arg, __E___E_cli_command_152_153, qelim_cmd, __E_cli_command_154, qelim_sym, get_cmd, 
		__E_cli_command_155, get_sym, __E___E_cli_command_155_156, option, set_cmd, __E_cli_command_157, set_sym, __E___E_cli_command_157_158, option_value, toggle_cmd, 
		__E_cli_command_159, toggle_sym, bool_option, bf_selection, wff_selection, abs_memory_sym, rel_memory_sym, selection_sym, examples_sym, __E_help_arg_160, 
		__E___E_help_arg_160_161, rel_memory, __E_memory_162, __E___E_memory_162_163, memory_id, abs_memory, __E_memory_164, __E___E_memory_164_165, bf_var_selection, wff_var_selection, 
		enum_option, severity_opt, __E_option_166, status_opt, __E_bool_option_167, colors_opt, __E_bool_option_168, debug_repl_opt, __E_bool_option_169, option_value_true, 
		option_value_false, severity, error_sym, __E_severity_170, info_sym, __E_severity_171, debug_sym, __E_severity_172, trace_sym, __E_severity_173, 
		bf_all, bf_ex, __N_0, __N_1, __N_2, __N_3, 
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
