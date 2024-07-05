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
	"__E_tau_27", "tau_and", "__E_tau_28", "tau_neg", "__E_tau_29", "wff_parenthesis", "__E_wff_30", "wff_sometimes", "__E_wff_31", "__E___E_wff_31_32", 
	"wff_always", "__E_wff_33", "__E___E_wff_33_34", "wff_conditional", "__E_wff_35", "wff_ball", "__E_wff_36", "q_bool_var", "wff_bex", "__E_wff_37", 
	"wff_all", "__E_wff_38", "q_var", "wff_ex", "__E_wff_39", "__E_wff_40", "__E___E_wff_40_41", "wff_imply", "__E_wff_42", "wff_equiv", 
	"__E_wff_43", "wff_or", "__E_wff_44", "wff_xor", "__E_wff_45", "wff_and", "__E_wff_46", "wff_neg", "__E_wff_47", "wff_t", 
	"wff_f", "bf_interval", "__E_wff_48", "bf_neq", "__E_wff_49", "bf_eq", "__E_wff_50", "bf_nleq_lower", "__E_wff_51", "bf_nleq_upper", 
	"__E_wff_52", "bf_greater", "__E_wff_53", "bf_less_equal", "__E_wff_54", "bf_less", "__E_wff_55", "bool_variable", "uninterpreted_constant", "bf_parenthesis", 
	"__E_bf_56", "bf_splitter", "__E_bf_57", "__E_bf_58", "__E___E_bf_58_59", "bf_or", "__E_bf_60", "bf_xor", "__E_bf_61", "bf_and", 
	"__E_bf_62", "__E___E_bf_62_63", "bf_neg", "__E_bf_64", "bf_constant", "__E_bf_65", "constant", "bf_t", "bf_f", "variable", 
	"__E_ref_args_66", "__E___E_ref_args_66_67", "ref_arg", "__E___E_ref_args_66_68", "__E___E_ref_args_66_69", "bf_cb_args1", "bf_cb_arg", "bf_cb_args2", "bf_cb_args3", "bf_cb_args4", 
	"wff_cb_args1", "wff_cb_arg", "wff_cb_args2", "wff_cb_args3", "wff_cb_args4", "tau_cb_args1", "tau_cb_arg", "__E_tau_cb_args1_70", "tau_cb_args2", "tau_cb_args3", 
	"tau_collapse_positives_cb", "__E_tau_cb_71", "__E___E_tau_cb_71_72", "tau_positives_upwards_cb", "__E_tau_cb_73", "bf_eq_cb", "__E_wff_cb_74", "bf_neq_cb", "__E_wff_cb_75", "wff_has_clashing_subformulas_cb", 
	"__E_wff_cb_76", "wff_has_subformula_cb", "__E_wff_cb_77", "wff_remove_existential_cb", "__E_wff_cb_78", "wff_remove_bexistential_cb", "__E_wff_cb_79", "wff_remove_buniversal_cb", "__E_wff_cb_80", "bf_is_zero_cb", 
	"__E_bf_cb_81", "bf_is_one_cb", "__E_bf_cb_82", "bf_has_subformula_cb", "__E_bf_cb_83", "bf_remove_funiversal_cb", "__E_bf_cb_84", "bf_remove_fexistential_cb", "__E_bf_cb_85", "bf_and_cb", 
	"__E_bf_cb_86", "bf_or_cb", "__E_bf_cb_87", "bf_xor_cb", "__E_bf_cb_88", "bf_neg_cb", "__E_bf_cb_89", "bf_normalize_cb", "__E_bf_cb_90", "charvar", 
	"__E_charvar_91", "__E_charvar_92", "io_var", "__E_variable_93", "out", "in_var_name", "__E_in_94", "digits", "offset", "out_var_name", 
	"__E_out_95", "__E_offsets_96", "__E_offsets_97", "num", "shift", "__E_offset_98", "__E_shift_99", "__E___E_shift_99_100", "uninter_const_name", "__E_uninterpreted_constant_101", 
	"__E___E_uninterpreted_constant_101_102", "chars", "binding", "__E_binding_103", "type", "__E___E_binding_103_104", "source", "named_binding", "src_c", "__E_source_105", 
	"__E_source_106", "__E___E_source_106_107", "__E___E_source_106_108", "__E_chars_109", "__E_digits_110", "comment", "__E_comment_111", "__E_comment_112", "__E_comment_113", "__E____114", 
	"__E___115", "cli", "cli_command", "__E_cli_116", "__E_cli_117", "quit_cmd", "quit_sym", "__E_cli_command_118", "version_cmd", "version_sym", 
	"__E_cli_command_119", "clear_cmd", "clear_sym", "__E_cli_command_120", "help_cmd", "__E_cli_command_121", "help_sym", "__E___E_cli_command_121_122", "__E___E_cli_command_121_123", "help_arg", 
	"file_cmd", "__E_cli_command_124", "file_sym", "q_string", "valid_cmd", "__E_cli_command_125", "valid_sym", "sat_cmd", "__E_cli_command_126", "sat_sym", 
	"unsat_cmd", "__E_cli_command_127", "unsat_sym", "solve_cmd", "__E_cli_command_128", "solve_sym", "solve_cmd_arg", "wff_cmd_arg", "execute_cmd", "__E_cli_command_129", 
	"execute_sym", "__E___E_cli_command_129_130", "execute_cmd_arg", "__E___E_cli_command_129_131", "wff_typed", "normalize_cmd", "__E_cli_command_132", "normalize_sym", "__E___E_cli_command_132_133", "normalize_cmd_arg", 
	"wff_inst_cmd", "__E_cli_command_134", "inst_sym", "__E___E_cli_command_134_135", "wff_cmd_out_arg", "subst_cmd", "__E_cli_command_136", "subst_sym", "__E___E_cli_command_136_137", "nf_cmd_arg", 
	"bf_inst_cmd", "__E_cli_command_138", "bf_cmd_arg", "bf_cmd_out_arg", "dnf_cmd", "__E_cli_command_139", "dnf_sym", "cnf_cmd", "__E_cli_command_140", "cnf_sym", 
	"anf_cmd", "__E_cli_command_141", "anf_sym", "nnf_cmd", "__E_cli_command_142", "nnf_sym", "pnf_cmd", "__E_cli_command_143", "pnf_sym", "mnf_cmd", 
	"__E_cli_command_144", "mnf_sym", "snf_cmd", "__E_cli_command_145", "snf_sym", "onf_cmd", "__E_cli_command_146", "onf_sym", "onf_cmd_arg", "def_list_cmd", 
	"__E_cli_command_147", "def_sym", "__E___E_cli_command_147_148", "def_rr_cmd", "__E_cli_command_149", "def_print_cmd", "__E_cli_command_150", "memory", "history_list_cmd", "__E_cli_command_151", 
	"history_sym", "__E___E_cli_command_151_152", "history_print_cmd", "__E_cli_command_153", "history_store_cmd", "__E_cli_command_154", "history_store_cmd_arg", "__E___E_cli_command_154_155", "bf_typed", "qelim_cmd", 
	"__E_cli_command_156", "qelim_sym", "get_cmd", "__E_cli_command_157", "get_sym", "__E___E_cli_command_157_158", "option", "set_cmd", "__E_cli_command_159", "set_sym", 
	"__E___E_cli_command_159_160", "option_value", "toggle_cmd", "__E_cli_command_161", "toggle_sym", "bool_option", "bf_selection", "wff_selection", "abs_memory_sym", "rel_memory_sym", 
	"selection_sym", "examples_sym", "__E_help_arg_162", "__E___E_help_arg_162_163", "rel_memory", "__E_memory_164", "__E___E_memory_164_165", "__E___E___E_memory_164_165_166", "memory_id", "abs_memory", 
	"__E_memory_167", "bf_var_selection", "wff_var_selection", "enum_option", "severity_opt", "__E_option_168", "status_opt", "__E_bool_option_169", "colors_opt", "__E_bool_option_170", 
	"debug_repl_opt", "__E_bool_option_171", "option_value_true", "option_value_false", "severity", "error_sym", "__E_severity_172", "info_sym", "__E_severity_173", "debug_sym", 
	"__E_severity_174", "trace_sym", "__E_severity_175", "bf_all", "bf_ex", "del_sym", "__N_0", "__N_1", "__N_2", "__N_3", 
	"__N_4", "__N_5", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', ';', '.', ':', '=', '<', '{', '}', '(', 
	')', '|', '&', '-', 's', 'o', 'm', 'e', 't', 'i', 
	'>', 'a', 'l', 'w', 'y', '[', ']', '?', 'b', '_', 
	'x', '^', '!', 'T', 'F', 'S', '+', '\'', '1', '0', 
	',', 'u', 'c', 'p', 'v', 'r', 'd', 'f', 'q', 'n', 
	'h', 'g', 'z', '$', '\t', '\n', '\r', '#', '/', '%', 
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
			87, 88, 89, 91, 93, 95, 97, 100, 103, 105,
			108, 110, 113, 117, 119, 121, 123, 125, 127, 129,
			130, 131, 133, 135, 137, 139, 141, 143, 145, 148,
			149, 151, 155, 157, 159, 162, 164, 167, 168, 190,
			193, 195, 197, 199, 201, 203, 205, 207, 209, 211,
			213, 215, 217, 219, 221, 223, 225, 227, 244, 271,
			276, 279, 282, 286, 292, 296, 299, 302, 305, 310,
			314, 317, 322, 325, 327, 336, 339, 342, 345, 348,
			351, 354, 357, 361, 370, 378, 381, 384, 387, 389,
			394, 398, 399, 400, 401, 414, 416, 418, 420, 422,
			423, 425, 427, 429, 431, 433, 434, 435
		},
		.trim_terminals = false,
		.to_inline = {
			{ 20, 80, 20 },
			{ 32, 95, 32 },
			{ 41, 149, 41 },
			{ 59 },
			{ 63 },
			{ 67 },
			{ 107 },
			{ 112 },
			{ 175 },
			{ 177 },
			{ 178 },
			{ 179 },
			{ 180 },
			{ 182 },
			{ 183 },
			{ 184 },
			{ 185 },
			{ 188 },
			{ 189 },
			{ 251 },
			{ 258 },
			{ 307 },
			{ 314 },
			{ 324 },
			{ 332 },
			{ 333 },
			{ 378 }
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
//G93:  __E_tau_29(94)       => '-' _(16) tau(20).
	p(NT(94), (T(12)+NT(16)+NT(20)));
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
//G100: __E___E_wff_31_32(99) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(99), (T(13)+T(14)+T(15)+T(16)+T(17)+T(18)+T(15)+T(16)+T(13)));
//G101: __E___E_wff_31_32(99) => '<' '>'.
	p(NT(99), (T(5)+T(19)));
//G102: __E_wff_31(98)       => __E___E_wff_31_32(99) _(16) wff(32).
	p(NT(98), (NT(99)+NT(16)+NT(32)));
//G103: wff_sometimes(97)    => __E_wff_31(98).
	p(NT(97), (NT(98)));
//G104: wff(32)              => wff_sometimes(97).
	p(NT(32), (NT(97)));
//G105: __E___E_wff_33_34(102) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(102), (T(20)+T(21)+T(22)+T(20)+T(23)+T(13)));
//G106: __E___E_wff_33_34(102) => '[' ']'.
	p(NT(102), (T(24)+T(25)));
//G107: __E_wff_33(101)      => __E___E_wff_33_34(102) _(16) wff(32).
	p(NT(101), (NT(102)+NT(16)+NT(32)));
//G108: wff_always(100)      => __E_wff_33(101).
	p(NT(100), (NT(101)));
//G109: wff(32)              => wff_always(100).
	p(NT(32), (NT(100)));
//G110: __E_wff_35(104)      => wff(32) _(16) '?' _(16) wff(32) _(16) ':' _(16) wff(32).
	p(NT(104), (NT(32)+NT(16)+T(26)+NT(16)+NT(32)+NT(16)+T(3)+NT(16)+NT(32)));
//G111: wff_conditional(103) => __E_wff_35(104).
	p(NT(103), (NT(104)));
//G112: wff(32)              => wff_conditional(103).
	p(NT(32), (NT(103)));
//G113: __E_wff_36(106)      => 'b' '_' 'a' 'l' 'l' __(72) q_bool_var(107) __(72) wff(32).
	p(NT(106), (T(27)+T(28)+T(20)+T(21)+T(21)+NT(72)+NT(107)+NT(72)+NT(32)));
//G114: wff_ball(105)        => __E_wff_36(106).
	p(NT(105), (NT(106)));
//G115: wff(32)              => wff_ball(105).
	p(NT(32), (NT(105)));
//G116: __E_wff_37(109)      => 'b' '_' 'e' 'x' __(72) q_bool_var(107) __(72) wff(32).
	p(NT(109), (T(27)+T(28)+T(16)+T(29)+NT(72)+NT(107)+NT(72)+NT(32)));
//G117: wff_bex(108)         => __E_wff_37(109).
	p(NT(108), (NT(109)));
//G118: wff(32)              => wff_bex(108).
	p(NT(32), (NT(108)));
//G119: __E_wff_38(111)      => 'a' 'l' 'l' __(72) q_var(112) __(72) wff(32).
	p(NT(111), (T(20)+T(21)+T(21)+NT(72)+NT(112)+NT(72)+NT(32)));
//G120: wff_all(110)         => __E_wff_38(111).
	p(NT(110), (NT(111)));
//G121: wff(32)              => wff_all(110).
	p(NT(32), (NT(110)));
//G122: __E_wff_39(114)      => 'e' 'x' __(72) q_var(112) __(72) wff(32).
	p(NT(114), (T(16)+T(29)+NT(72)+NT(112)+NT(72)+NT(32)));
//G123: wff_ex(113)          => __E_wff_39(114).
	p(NT(113), (NT(114)));
//G124: wff(32)              => wff_ex(113).
	p(NT(32), (NT(113)));
//G125: __E___E_wff_40_41(116) => _(16) offsets(87).
	p(NT(116), (NT(16)+NT(87)));
//G126: __E___E_wff_40_41(116) => null.
	p(NT(116), (nul));
//G127: __E_wff_40(115)      => sym(85) __E___E_wff_40_41(116) _(16) ref_args(88).
	p(NT(115), (NT(85)+NT(116)+NT(16)+NT(88)));
//G128: wff_ref(39)          => __E_wff_40(115).
	p(NT(39), (NT(115)));
//G129: wff(32)              => wff_ref(39).
	p(NT(32), (NT(39)));
//G130: __E_wff_42(118)      => wff(32) _(16) '-' '>' _(16) wff(32).
	p(NT(118), (NT(32)+NT(16)+T(12)+T(19)+NT(16)+NT(32)));
//G131: wff_imply(117)       => __E_wff_42(118).
	p(NT(117), (NT(118)));
//G132: wff(32)              => wff_imply(117).
	p(NT(32), (NT(117)));
//G133: __E_wff_43(120)      => wff(32) _(16) '<' '-' '>' _(16) wff(32).
	p(NT(120), (NT(32)+NT(16)+T(5)+T(12)+T(19)+NT(16)+NT(32)));
//G134: wff_equiv(119)       => __E_wff_43(120).
	p(NT(119), (NT(120)));
//G135: wff(32)              => wff_equiv(119).
	p(NT(32), (NT(119)));
//G136: __E_wff_44(122)      => wff(32) _(16) '|' '|' _(16) wff(32).
	p(NT(122), (NT(32)+NT(16)+T(10)+T(10)+NT(16)+NT(32)));
//G137: wff_or(121)          => __E_wff_44(122).
	p(NT(121), (NT(122)));
//G138: wff(32)              => wff_or(121).
	p(NT(32), (NT(121)));
//G139: __E_wff_45(124)      => wff(32) _(16) '^' _(16) wff(32).
	p(NT(124), (NT(32)+NT(16)+T(30)+NT(16)+NT(32)));
//G140: wff_xor(123)         => __E_wff_45(124).
	p(NT(123), (NT(124)));
//G141: wff(32)              => wff_xor(123).
	p(NT(32), (NT(123)));
//G142: __E_wff_46(126)      => wff(32) _(16) '&' '&' _(16) wff(32).
	p(NT(126), (NT(32)+NT(16)+T(11)+T(11)+NT(16)+NT(32)));
//G143: wff_and(125)         => __E_wff_46(126).
	p(NT(125), (NT(126)));
//G144: wff(32)              => wff_and(125).
	p(NT(32), (NT(125)));
//G145: __E_wff_47(128)      => '!' _(16) wff(32).
	p(NT(128), (T(31)+NT(16)+NT(32)));
//G146: wff_neg(127)         => __E_wff_47(128).
	p(NT(127), (NT(128)));
//G147: wff(32)              => wff_neg(127).
	p(NT(32), (NT(127)));
//G148: wff_t(129)           => 'T'.
	p(NT(129), (T(32)));
//G149: wff(32)              => wff_t(129).
	p(NT(32), (NT(129)));
//G150: wff_f(130)           => 'F'.
	p(NT(130), (T(33)));
//G151: wff(32)              => wff_f(130).
	p(NT(32), (NT(130)));
//G152: __E_wff_48(132)      => bf(41) _(16) '<' '=' _(16) q_var(112) _(16) '<' '=' _(16) bf(41).
	p(NT(132), (NT(41)+NT(16)+T(5)+T(4)+NT(16)+NT(112)+NT(16)+T(5)+T(4)+NT(16)+NT(41)));
//G153: bf_interval(131)     => __E_wff_48(132).
	p(NT(131), (NT(132)));
//G154: wff(32)              => bf_interval(131).
	p(NT(32), (NT(131)));
//G155: __E_wff_49(134)      => bf(41) _(16) '!' '=' _(16) bf(41).
	p(NT(134), (NT(41)+NT(16)+T(31)+T(4)+NT(16)+NT(41)));
//G156: bf_neq(133)          => __E_wff_49(134).
	p(NT(133), (NT(134)));
//G157: wff(32)              => bf_neq(133).
	p(NT(32), (NT(133)));
//G158: __E_wff_50(136)      => bf(41) _(16) '=' _(16) bf(41).
	p(NT(136), (NT(41)+NT(16)+T(4)+NT(16)+NT(41)));
//G159: bf_eq(135)           => __E_wff_50(136).
	p(NT(135), (NT(136)));
//G160: wff(32)              => bf_eq(135).
	p(NT(32), (NT(135)));
//G161: __E_wff_51(138)      => bf(41) _(16) '!' '<' '=' _(16) q_var(112).
	p(NT(138), (NT(41)+NT(16)+T(31)+T(5)+T(4)+NT(16)+NT(112)));
//G162: bf_nleq_lower(137)   => __E_wff_51(138).
	p(NT(137), (NT(138)));
//G163: wff(32)              => bf_nleq_lower(137).
	p(NT(32), (NT(137)));
//G164: __E_wff_52(140)      => q_var(112) _(16) '!' '<' '=' _(16) bf(41).
	p(NT(140), (NT(112)+NT(16)+T(31)+T(5)+T(4)+NT(16)+NT(41)));
//G165: bf_nleq_upper(139)   => __E_wff_52(140).
	p(NT(139), (NT(140)));
//G166: wff(32)              => bf_nleq_upper(139).
	p(NT(32), (NT(139)));
//G167: __E_wff_53(142)      => bf(41) _(16) '>' _(16) bf(41).
	p(NT(142), (NT(41)+NT(16)+T(19)+NT(16)+NT(41)));
//G168: bf_greater(141)      => __E_wff_53(142).
	p(NT(141), (NT(142)));
//G169: wff(32)              => bf_greater(141).
	p(NT(32), (NT(141)));
//G170: __E_wff_54(144)      => bf(41) _(16) '<' '=' _(16) bf(41).
	p(NT(144), (NT(41)+NT(16)+T(5)+T(4)+NT(16)+NT(41)));
//G171: bf_less_equal(143)   => __E_wff_54(144).
	p(NT(143), (NT(144)));
//G172: wff(32)              => bf_less_equal(143).
	p(NT(32), (NT(143)));
//G173: __E_wff_55(146)      => bf(41) _(16) '<' _(16) bf(41).
	p(NT(146), (NT(41)+NT(16)+T(5)+NT(16)+NT(41)));
//G174: bf_less(145)         => __E_wff_55(146).
	p(NT(145), (NT(146)));
//G175: wff(32)              => bf_less(145).
	p(NT(32), (NT(145)));
//G176: wff(32)              => capture(70).
	p(NT(32), (NT(70)));
//G177: wff(32)              => bool_variable(147).
	p(NT(32), (NT(147)));
//G178: wff(32)              => uninterpreted_constant(148).
	p(NT(32), (NT(148)));
//G179: __E_bf_56(150)       => '(' _(16) bf(41) _(16) ')'.
	p(NT(150), (T(8)+NT(16)+NT(41)+NT(16)+T(9)));
//G180: bf_parenthesis(149)  => __E_bf_56(150).
	p(NT(149), (NT(150)));
//G181: bf(41)               => bf_parenthesis(149).
	p(NT(41), (NT(149)));
//G182: __E_bf_57(152)       => 'S' _(16) '(' _(16) bf(41) _(16) ')'.
	p(NT(152), (T(34)+NT(16)+T(8)+NT(16)+NT(41)+NT(16)+T(9)));
//G183: bf_splitter(151)     => __E_bf_57(152).
	p(NT(151), (NT(152)));
//G184: bf(41)               => bf_splitter(151).
	p(NT(41), (NT(151)));
//G185: __E___E_bf_58_59(154) => _(16) offsets(87).
	p(NT(154), (NT(16)+NT(87)));
//G186: __E___E_bf_58_59(154) => null.
	p(NT(154), (nul));
//G187: __E_bf_58(153)       => sym(85) __E___E_bf_58_59(154) _(16) ref_args(88).
	p(NT(153), (NT(85)+NT(154)+NT(16)+NT(88)));
//G188: bf_ref(40)           => __E_bf_58(153).
	p(NT(40), (NT(153)));
//G189: bf(41)               => bf_ref(40).
	p(NT(41), (NT(40)));
//G190: __E_bf_60(156)       => bf(41) _(16) '|' _(16) bf(41).
	p(NT(156), (NT(41)+NT(16)+T(10)+NT(16)+NT(41)));
//G191: bf_or(155)           => __E_bf_60(156).
	p(NT(155), (NT(156)));
//G192: bf(41)               => bf_or(155).
	p(NT(41), (NT(155)));
//G193: __E_bf_61(158)       => bf(41) _(16) '+' _(16) bf(41).
	p(NT(158), (NT(41)+NT(16)+T(35)+NT(16)+NT(41)));
//G194: bf_xor(157)          => __E_bf_61(158).
	p(NT(157), (NT(158)));
//G195: bf(41)               => bf_xor(157).
	p(NT(41), (NT(157)));
//G196: __E___E_bf_62_63(161) => _(16).
	p(NT(161), (NT(16)));
//G197: __E___E_bf_62_63(161) => _(16) '&' _(16).
	p(NT(161), (NT(16)+T(11)+NT(16)));
//G198: __E_bf_62(160)       => bf(41) __E___E_bf_62_63(161) bf(41).
	p(NT(160), (NT(41)+NT(161)+NT(41)));
//G199: bf_and(159)          => __E_bf_62(160).
	p(NT(159), (NT(160)));
//G200: bf(41)               => bf_and(159).
	p(NT(41), (NT(159)));
//G201: __E_bf_64(163)       => bf(41) _(16) '\''.
	p(NT(163), (NT(41)+NT(16)+T(36)));
//G202: bf_neg(162)          => __E_bf_64(163).
	p(NT(162), (NT(163)));
//G203: bf(41)               => bf_neg(162).
	p(NT(41), (NT(162)));
//G204: __E_bf_65(165)       => '{' _(16) constant(166) _(16) '}'.
	p(NT(165), (T(6)+NT(16)+NT(166)+NT(16)+T(7)));
//G205: bf_constant(164)     => __E_bf_65(165).
	p(NT(164), (NT(165)));
//G206: bf(41)               => bf_constant(164).
	p(NT(41), (NT(164)));
//G207: bf_t(167)            => '1'.
	p(NT(167), (T(37)));
//G208: bf(41)               => bf_t(167).
	p(NT(41), (NT(167)));
//G209: bf_f(168)            => '0'.
	p(NT(168), (T(38)));
//G210: bf(41)               => bf_f(168).
	p(NT(41), (NT(168)));
//G211: bf(41)               => capture(70).
	p(NT(41), (NT(70)));
//G212: bf(41)               => variable(169).
	p(NT(41), (NT(169)));
//G213: bf(41)               => uninterpreted_constant(148).
	p(NT(41), (NT(148)));
//G214: ref_arg(172)         => bf(41).
	p(NT(172), (NT(41)));
//G215: __E___E_ref_args_66_67(171) => _(16) ref_arg(172).
	p(NT(171), (NT(16)+NT(172)));
//G216: __E___E_ref_args_66_68(173) => _(16) ',' _(16) ref_arg(172).
	p(NT(173), (NT(16)+T(39)+NT(16)+NT(172)));
//G217: __E___E_ref_args_66_69(174) => null.
	p(NT(174), (nul));
//G218: __E___E_ref_args_66_69(174) => __E___E_ref_args_66_68(173) __E___E_ref_args_66_69(174).
	p(NT(174), (NT(173)+NT(174)));
//G219: __E_ref_args_66(170) => __E___E_ref_args_66_67(171) __E___E_ref_args_66_69(174).
	p(NT(170), (NT(171)+NT(174)));
//G220: __E_ref_args_66(170) => null.
	p(NT(170), (nul));
//G221: ref_args(88)         => '(' __E_ref_args_66(170) _(16) ')'.
	p(NT(88), (T(8)+NT(170)+NT(16)+T(9)));
//G222: bf_cb_arg(176)       => bf(41).
	p(NT(176), (NT(41)));
//G223: bf_cb_args1(175)     => __(72) bf_cb_arg(176).
	p(NT(175), (NT(72)+NT(176)));
//G224: bf_cb_args2(177)     => bf_cb_args1(175) bf_cb_args1(175).
	p(NT(177), (NT(175)+NT(175)));
//G225: bf_cb_args3(178)     => bf_cb_args2(177) bf_cb_args1(175).
	p(NT(178), (NT(177)+NT(175)));
//G226: bf_cb_args4(179)     => bf_cb_args3(178) bf_cb_args1(175).
	p(NT(179), (NT(178)+NT(175)));
//G227: wff_cb_arg(181)      => wff(32).
	p(NT(181), (NT(32)));
//G228: wff_cb_args1(180)    => __(72) wff_cb_arg(181).
	p(NT(180), (NT(72)+NT(181)));
//G229: wff_cb_args2(182)    => wff_cb_args1(180) wff_cb_args1(180).
	p(NT(182), (NT(180)+NT(180)));
//G230: wff_cb_args3(183)    => wff_cb_args2(182) wff_cb_args1(180).
	p(NT(183), (NT(182)+NT(180)));
//G231: wff_cb_args4(184)    => wff_cb_args3(183) wff_cb_args1(180).
	p(NT(184), (NT(183)+NT(180)));
//G232: __E_tau_cb_args1_70(187) => capture(70).
	p(NT(187), (NT(70)));
//G233: __E_tau_cb_args1_70(187) => tau(20).
	p(NT(187), (NT(20)));
//G234: tau_cb_arg(186)      => __E_tau_cb_args1_70(187).
	p(NT(186), (NT(187)));
//G235: tau_cb_args1(185)    => __(72) tau_cb_arg(186).
	p(NT(185), (NT(72)+NT(186)));
//G236: tau_cb_args2(188)    => tau_cb_args1(185) tau_cb_args1(185).
	p(NT(188), (NT(185)+NT(185)));
//G237: tau_cb_args3(189)    => tau_cb_args2(188) tau_cb_args1(185).
	p(NT(189), (NT(188)+NT(185)));
//G238: __E___E_tau_cb_71_72(192) => tau_cb_args3(189).
	p(NT(192), (NT(189)));
//G239: __E___E_tau_cb_71_72(192) => tau_cb_args2(188).
	p(NT(192), (NT(188)));
//G240: __E_tau_cb_71(191)   => 't' 'a' 'u' '_' 'c' 'o' 'l' 'l' 'a' 'p' 's' 'e' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'c' 'b' __E___E_tau_cb_71_72(192).
	p(NT(191), (T(17)+T(20)+T(40)+T(28)+T(41)+T(14)+T(21)+T(21)+T(20)+T(42)+T(13)+T(16)+T(28)+T(42)+T(14)+T(13)+T(18)+T(17)+T(18)+T(43)+T(16)+T(13)+T(28)+T(41)+T(27)+NT(192)));
//G241: tau_collapse_positives_cb(190) => __E_tau_cb_71(191).
	p(NT(190), (NT(191)));
//G242: tau_cb(59)           => tau_collapse_positives_cb(190).
	p(NT(59), (NT(190)));
//G243: __E_tau_cb_73(194)   => 't' 'a' 'u' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'u' 'p' 'w' 'a' 'r' 'd' 's' '_' 'c' 'b' tau_cb_args2(188).
	p(NT(194), (T(17)+T(20)+T(40)+T(28)+T(42)+T(14)+T(13)+T(18)+T(17)+T(18)+T(43)+T(16)+T(13)+T(28)+T(40)+T(42)+T(22)+T(20)+T(44)+T(45)+T(13)+T(28)+T(41)+T(27)+NT(188)));
//G244: tau_positives_upwards_cb(193) => __E_tau_cb_73(194).
	p(NT(193), (NT(194)));
//G245: tau_cb(59)           => tau_positives_upwards_cb(193).
	p(NT(59), (NT(193)));
//G246: __E_wff_cb_74(196)   => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b' bf_cb_args1(175) wff_cb_args2(182).
	p(NT(196), (T(27)+T(46)+T(28)+T(16)+T(47)+T(28)+T(41)+T(27)+NT(175)+NT(182)));
//G247: bf_eq_cb(195)        => __E_wff_cb_74(196).
	p(NT(195), (NT(196)));
//G248: wff_cb(63)           => bf_eq_cb(195).
	p(NT(63), (NT(195)));
//G249: __E_wff_cb_75(198)   => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b' bf_cb_args1(175) wff_cb_args2(182).
	p(NT(198), (T(27)+T(46)+T(28)+T(48)+T(16)+T(47)+T(28)+T(41)+T(27)+NT(175)+NT(182)));
//G250: bf_neq_cb(197)       => __E_wff_cb_75(198).
	p(NT(197), (NT(198)));
//G251: wff_cb(63)           => bf_neq_cb(197).
	p(NT(63), (NT(197)));
//G252: __E_wff_cb_76(200)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(182).
	p(NT(200), (T(22)+T(46)+T(46)+T(28)+T(49)+T(20)+T(13)+T(28)+T(41)+T(21)+T(20)+T(13)+T(49)+T(18)+T(48)+T(50)+T(28)+T(13)+T(40)+T(27)+T(46)+T(14)+T(44)+T(15)+T(40)+T(21)+T(20)+T(13)+T(28)+T(41)+T(27)+NT(182)));
//G253: wff_has_clashing_subformulas_cb(199) => __E_wff_cb_76(200).
	p(NT(199), (NT(200)));
//G254: wff_cb(63)           => wff_has_clashing_subformulas_cb(199).
	p(NT(63), (NT(199)));
//G255: __E_wff_cb_77(202)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(183).
	p(NT(202), (T(22)+T(46)+T(46)+T(28)+T(49)+T(20)+T(13)+T(28)+T(13)+T(40)+T(27)+T(46)+T(14)+T(44)+T(15)+T(40)+T(21)+T(20)+T(28)+T(41)+T(27)+NT(183)));
//G256: wff_has_subformula_cb(201) => __E_wff_cb_77(202).
	p(NT(201), (NT(202)));
//G257: wff_cb(63)           => wff_has_subformula_cb(201).
	p(NT(63), (NT(201)));
//G258: __E_wff_cb_78(204)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(182).
	p(NT(204), (T(22)+T(46)+T(46)+T(28)+T(44)+T(16)+T(15)+T(14)+T(43)+T(16)+T(28)+T(16)+T(29)+T(18)+T(13)+T(17)+T(16)+T(48)+T(17)+T(18)+T(20)+T(21)+T(28)+T(41)+T(27)+NT(182)));
//G259: wff_remove_existential_cb(203) => __E_wff_cb_78(204).
	p(NT(203), (NT(204)));
//G260: wff_cb(63)           => wff_remove_existential_cb(203).
	p(NT(63), (NT(203)));
//G261: __E_wff_cb_79(206)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args4(184).
	p(NT(206), (T(22)+T(46)+T(46)+T(28)+T(44)+T(16)+T(15)+T(14)+T(43)+T(16)+T(28)+T(27)+T(16)+T(29)+T(18)+T(13)+T(17)+T(16)+T(48)+T(17)+T(18)+T(20)+T(21)+T(28)+T(41)+T(27)+NT(184)));
//G262: wff_remove_bexistential_cb(205) => __E_wff_cb_79(206).
	p(NT(205), (NT(206)));
//G263: wff_cb(63)           => wff_remove_bexistential_cb(205).
	p(NT(63), (NT(205)));
//G264: __E_wff_cb_80(208)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' wff_cb_args4(184).
	p(NT(208), (T(22)+T(46)+T(46)+T(28)+T(44)+T(16)+T(15)+T(14)+T(43)+T(16)+T(28)+T(27)+T(40)+T(48)+T(18)+T(43)+T(16)+T(44)+T(13)+T(20)+T(21)+T(28)+T(41)+T(27)+NT(184)));
//G265: wff_remove_buniversal_cb(207) => __E_wff_cb_80(208).
	p(NT(207), (NT(208)));
//G266: wff_cb(63)           => wff_remove_buniversal_cb(207).
	p(NT(63), (NT(207)));
//G267: __E_bf_cb_81(210)    => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b' bf_cb_args2(177).
	p(NT(210), (T(27)+T(46)+T(28)+T(18)+T(13)+T(28)+T(51)+T(16)+T(44)+T(14)+T(28)+T(41)+T(27)+NT(177)));
//G268: bf_is_zero_cb(209)   => __E_bf_cb_81(210).
	p(NT(209), (NT(210)));
//G269: bf_cb(67)            => bf_is_zero_cb(209).
	p(NT(67), (NT(209)));
//G270: __E_bf_cb_82(212)    => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b' bf_cb_args2(177).
	p(NT(212), (T(27)+T(46)+T(28)+T(18)+T(13)+T(28)+T(14)+T(48)+T(16)+T(28)+T(41)+T(27)+NT(177)));
//G271: bf_is_one_cb(211)    => __E_bf_cb_82(212).
	p(NT(211), (NT(212)));
//G272: bf_cb(67)            => bf_is_one_cb(211).
	p(NT(67), (NT(211)));
//G273: __E_bf_cb_83(214)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(178).
	p(NT(214), (T(27)+T(46)+T(28)+T(49)+T(20)+T(13)+T(28)+T(13)+T(40)+T(27)+T(46)+T(14)+T(44)+T(15)+T(40)+T(21)+T(20)+T(28)+T(41)+T(27)+NT(178)));
//G274: bf_has_subformula_cb(213) => __E_bf_cb_83(214).
	p(NT(213), (NT(214)));
//G275: bf_cb(67)            => bf_has_subformula_cb(213).
	p(NT(67), (NT(213)));
//G276: __E_bf_cb_84(216)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(179).
	p(NT(216), (T(27)+T(46)+T(28)+T(44)+T(16)+T(15)+T(14)+T(43)+T(16)+T(28)+T(46)+T(40)+T(48)+T(18)+T(43)+T(16)+T(44)+T(13)+T(20)+T(21)+T(28)+T(41)+T(27)+NT(179)));
//G277: bf_remove_funiversal_cb(215) => __E_bf_cb_84(216).
	p(NT(215), (NT(216)));
//G278: bf_cb(67)            => bf_remove_funiversal_cb(215).
	p(NT(67), (NT(215)));
//G279: __E_bf_cb_85(218)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(179).
	p(NT(218), (T(27)+T(46)+T(28)+T(44)+T(16)+T(15)+T(14)+T(43)+T(16)+T(28)+T(46)+T(16)+T(29)+T(18)+T(13)+T(17)+T(16)+T(48)+T(17)+T(18)+T(20)+T(21)+T(28)+T(41)+T(27)+NT(179)));
//G280: bf_remove_fexistential_cb(217) => __E_bf_cb_85(218).
	p(NT(217), (NT(218)));
//G281: bf_cb(67)            => bf_remove_fexistential_cb(217).
	p(NT(67), (NT(217)));
//G282: __E_bf_cb_86(220)    => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b' bf_cb_args2(177).
	p(NT(220), (T(27)+T(46)+T(28)+T(20)+T(48)+T(45)+T(28)+T(41)+T(27)+NT(177)));
//G283: bf_and_cb(219)       => __E_bf_cb_86(220).
	p(NT(219), (NT(220)));
//G284: bf_cb(67)            => bf_and_cb(219).
	p(NT(67), (NT(219)));
//G285: __E_bf_cb_87(222)    => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b' bf_cb_args2(177).
	p(NT(222), (T(27)+T(46)+T(28)+T(14)+T(44)+T(28)+T(41)+T(27)+NT(177)));
//G286: bf_or_cb(221)        => __E_bf_cb_87(222).
	p(NT(221), (NT(222)));
//G287: bf_cb(67)            => bf_or_cb(221).
	p(NT(67), (NT(221)));
//G288: __E_bf_cb_88(224)    => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b' bf_cb_args2(177).
	p(NT(224), (T(27)+T(46)+T(28)+T(29)+T(14)+T(44)+T(28)+T(41)+T(27)+NT(177)));
//G289: bf_xor_cb(223)       => __E_bf_cb_88(224).
	p(NT(223), (NT(224)));
//G290: bf_cb(67)            => bf_xor_cb(223).
	p(NT(67), (NT(223)));
//G291: __E_bf_cb_89(226)    => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b' bf_cb_args1(175).
	p(NT(226), (T(27)+T(46)+T(28)+T(48)+T(16)+T(50)+T(28)+T(41)+T(27)+NT(175)));
//G292: bf_neg_cb(225)       => __E_bf_cb_89(226).
	p(NT(225), (NT(226)));
//G293: bf_cb(67)            => bf_neg_cb(225).
	p(NT(67), (NT(225)));
//G294: __E_bf_cb_90(228)    => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(175).
	p(NT(228), (T(27)+T(46)+T(28)+T(48)+T(14)+T(44)+T(15)+T(20)+T(21)+T(18)+T(51)+T(16)+T(28)+T(41)+T(27)+NT(175)));
//G295: bf_normalize_cb(227) => __E_bf_cb_90(228).
	p(NT(227), (NT(228)));
//G296: bf_cb(67)            => bf_normalize_cb(227).
	p(NT(67), (NT(227)));
//G297: __N_0(436)           => 'F'.
	p(NT(436), (T(33)));
//G298: __N_1(437)           => 'T'.
	p(NT(437), (T(32)));
//G299: __N_2(438)           => 'i'.
	p(NT(438), (T(18)));
//G300: __N_3(439)           => 'o'.
	p(NT(439), (T(14)));
//G301: __E_charvar_91(230)  => ~( __N_0(436) ) & ~( __N_1(437) ) & ~( __N_2(438) ) & ~( __N_3(439) ) & alpha(5).	 # conjunctive
	p(NT(230), ~(NT(436)) & ~(NT(437)) & ~(NT(438)) & ~(NT(439)) & (NT(5)));
//G302: __E_charvar_92(231)  => null.
	p(NT(231), (nul));
//G303: __E_charvar_92(231)  => digit(3) __E_charvar_92(231).
	p(NT(231), (NT(3)+NT(231)));
//G304: charvar(229)         => __E_charvar_91(230) __E_charvar_92(231).
	p(NT(229), (NT(230)+NT(231)));
//G305: bool_variable(147)   => '?' charvar(229).
	p(NT(147), (T(26)+NT(229)));
//G306: variable(169)        => charvar(229).
	p(NT(169), (NT(229)));
//G307: __E_variable_93(233) => in(45).
	p(NT(233), (NT(45)));
//G308: __E_variable_93(233) => out(234).
	p(NT(233), (NT(234)));
//G309: io_var(232)          => __E_variable_93(233).
	p(NT(232), (NT(233)));
//G310: variable(169)        => io_var(232).
	p(NT(169), (NT(232)));
//G311: __E_in_94(236)       => 'i' digits(237).
	p(NT(236), (T(18)+NT(237)));
//G312: in_var_name(235)     => __E_in_94(236).
	p(NT(235), (NT(236)));
//G313: in(45)               => in_var_name(235) '[' offset(238) ']'.
	p(NT(45), (NT(235)+T(24)+NT(238)+T(25)));
//G314: __E_out_95(240)      => 'o' digits(237).
	p(NT(240), (T(14)+NT(237)));
//G315: out_var_name(239)    => __E_out_95(240).
	p(NT(239), (NT(240)));
//G316: out(234)             => out_var_name(239) '[' offset(238) ']'.
	p(NT(234), (NT(239)+T(24)+NT(238)+T(25)));
//G317: capture(70)          => '$' charvar(229).
	p(NT(70), (T(52)+NT(229)));
//G318: q_bool_var(107)      => capture(70).
	p(NT(107), (NT(70)));
//G319: q_bool_var(107)      => bool_variable(147).
	p(NT(107), (NT(147)));
//G320: q_var(112)           => capture(70).
	p(NT(112), (NT(70)));
//G321: q_var(112)           => variable(169).
	p(NT(112), (NT(169)));
//G322: __E_offsets_96(241)  => _(16) ',' _(16) offset(238).
	p(NT(241), (NT(16)+T(39)+NT(16)+NT(238)));
//G323: __E_offsets_97(242)  => null.
	p(NT(242), (nul));
//G324: __E_offsets_97(242)  => __E_offsets_96(241) __E_offsets_97(242).
	p(NT(242), (NT(241)+NT(242)));
//G325: offsets(87)          => '[' _(16) offset(238) __E_offsets_97(242) _(16) ']'.
	p(NT(87), (T(24)+NT(16)+NT(238)+NT(242)+NT(16)+T(25)));
//G326: offset(238)          => num(243).
	p(NT(238), (NT(243)));
//G327: offset(238)          => capture(70).
	p(NT(238), (NT(70)));
//G328: offset(238)          => shift(244).
	p(NT(238), (NT(244)));
//G329: __N_4(440)           => io_var(232).
	p(NT(440), (NT(232)));
//G330: __E_offset_98(245)   => variable(169) & ~( __N_4(440) ).	 # conjunctive
	p(NT(245), (NT(169)) & ~(NT(440)));
//G331: offset(238)          => __E_offset_98(245).
	p(NT(238), (NT(245)));
//G332: __E_shift_99(246)    => capture(70).
	p(NT(246), (NT(70)));
//G333: __N_5(441)           => io_var(232).
	p(NT(441), (NT(232)));
//G334: __E___E_shift_99_100(247) => variable(169) & ~( __N_5(441) ).	 # conjunctive
	p(NT(247), (NT(169)) & ~(NT(441)));
//G335: __E_shift_99(246)    => __E___E_shift_99_100(247).
	p(NT(246), (NT(247)));
//G336: shift(244)           => __E_shift_99(246) _(16) '-' _(16) num(243).
	p(NT(244), (NT(246)+NT(16)+T(12)+NT(16)+NT(243)));
//G337: num(243)             => digits(237).
	p(NT(243), (NT(237)));
//G338: __E___E_uninterpreted_constant_101_102(250) => chars(251).
	p(NT(250), (NT(251)));
//G339: __E___E_uninterpreted_constant_101_102(250) => _(16).
	p(NT(250), (NT(16)));
//G340: __E_uninterpreted_constant_101(249) => __E___E_uninterpreted_constant_101_102(250) ':' chars(251).
	p(NT(249), (NT(250)+T(3)+NT(251)));
//G341: uninter_const_name(248) => __E_uninterpreted_constant_101(249).
	p(NT(248), (NT(249)));
//G342: uninterpreted_constant(148) => '<' uninter_const_name(248) _(16) '>'.
	p(NT(148), (T(5)+NT(248)+NT(16)+T(19)));
//G343: constant(166)        => binding(252).
	p(NT(166), (NT(252)));
//G344: constant(166)        => capture(70).
	p(NT(166), (NT(70)));
//G345: __E___E_binding_103_104(255) => chars(251) _(16).
	p(NT(255), (NT(251)+NT(16)));
//G346: __E___E_binding_103_104(255) => _(16).
	p(NT(255), (NT(16)));
//G347: type(254)            => __E___E_binding_103_104(255).
	p(NT(254), (NT(255)));
//G348: __E_binding_103(253) => type(254) ':' _(16) source(256).
	p(NT(253), (NT(254)+T(3)+NT(16)+NT(256)));
//G349: source_binding(46)   => __E_binding_103(253).
	p(NT(46), (NT(253)));
//G350: binding(252)         => source_binding(46).
	p(NT(252), (NT(46)));
//G351: named_binding(257)   => chars(251).
	p(NT(257), (NT(251)));
//G352: binding(252)         => named_binding(257).
	p(NT(252), (NT(257)));
//G353: __E_source_105(259)  => alnum(6).
	p(NT(259), (NT(6)));
//G354: __E_source_105(259)  => punct(7).
	p(NT(259), (NT(7)));
//G355: src_c(258)           => __E_source_105(259).
	p(NT(258), (NT(259)));
//G356: __E___E_source_106_107(261) => src_c(258).
	p(NT(261), (NT(258)));
//G357: __E___E_source_106_107(261) => space(2).
	p(NT(261), (NT(2)));
//G358: __E___E_source_106_108(262) => null.
	p(NT(262), (nul));
//G359: __E___E_source_106_108(262) => __E___E_source_106_107(261) __E___E_source_106_108(262).
	p(NT(262), (NT(261)+NT(262)));
//G360: __E_source_106(260)  => __E___E_source_106_108(262) src_c(258).
	p(NT(260), (NT(262)+NT(258)));
//G361: __E_source_106(260)  => null.
	p(NT(260), (nul));
//G362: source(256)          => src_c(258) __E_source_106(260).
	p(NT(256), (NT(258)+NT(260)));
//G363: __E_chars_109(263)   => null.
	p(NT(263), (nul));
//G364: __E_chars_109(263)   => alnum(6) __E_chars_109(263).
	p(NT(263), (NT(6)+NT(263)));
//G365: chars(251)           => alpha(5) __E_chars_109(263).
	p(NT(251), (NT(5)+NT(263)));
//G366: __E_digits_110(264)  => digit(3).
	p(NT(264), (NT(3)));
//G367: __E_digits_110(264)  => digit(3) __E_digits_110(264).
	p(NT(264), (NT(3)+NT(264)));
//G368: digits(237)          => __E_digits_110(264).
	p(NT(237), (NT(264)));
//G369: sym(85)              => chars(251).
	p(NT(85), (NT(251)));
//G370: __E_comment_111(266) => printable(8).
	p(NT(266), (NT(8)));
//G371: __E_comment_111(266) => '\t'.
	p(NT(266), (T(53)));
//G372: __E_comment_112(267) => null.
	p(NT(267), (nul));
//G373: __E_comment_112(267) => __E_comment_111(266) __E_comment_112(267).
	p(NT(267), (NT(266)+NT(267)));
//G374: __E_comment_113(268) => '\n'.
	p(NT(268), (T(54)));
//G375: __E_comment_113(268) => '\r'.
	p(NT(268), (T(55)));
//G376: __E_comment_113(268) => eof(1).
	p(NT(268), (NT(1)));
//G377: comment(265)         => '#' __E_comment_112(267) __E_comment_113(268).
	p(NT(265), (T(56)+NT(267)+NT(268)));
//G378: __E____114(269)      => space(2).
	p(NT(269), (NT(2)));
//G379: __E____114(269)      => comment(265).
	p(NT(269), (NT(265)));
//G380: __(72)               => __E____114(269) _(16).
	p(NT(72), (NT(269)+NT(16)));
//G381: __E___115(270)       => __(72).
	p(NT(270), (NT(72)));
//G382: __E___115(270)       => null.
	p(NT(270), (nul));
//G383: _(16)                => __E___115(270).
	p(NT(16), (NT(270)));
//G384: __E_cli_116(273)     => '.' _(16) cli_command(272) _(16).
	p(NT(273), (T(2)+NT(16)+NT(272)+NT(16)));
//G385: __E_cli_117(274)     => null.
	p(NT(274), (nul));
//G386: __E_cli_117(274)     => __E_cli_116(273) __E_cli_117(274).
	p(NT(274), (NT(273)+NT(274)));
//G387: cli(271)             => _(16) cli_command(272) _(16) __E_cli_117(274).
	p(NT(271), (NT(16)+NT(272)+NT(16)+NT(274)));
//G388: __E_cli_command_118(277) => 'q'.
	p(NT(277), (T(47)));
//G389: __E_cli_command_118(277) => 'q' 'u' 'i' 't'.
	p(NT(277), (T(47)+T(40)+T(18)+T(17)));
//G390: quit_sym(276)        => __E_cli_command_118(277).
	p(NT(276), (NT(277)));
//G391: quit_cmd(275)        => quit_sym(276).
	p(NT(275), (NT(276)));
//G392: cli_command(272)     => quit_cmd(275).
	p(NT(272), (NT(275)));
//G393: __E_cli_command_119(280) => 'v'.
	p(NT(280), (T(43)));
//G394: __E_cli_command_119(280) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(280), (T(43)+T(16)+T(44)+T(13)+T(18)+T(14)+T(48)));
//G395: version_sym(279)     => __E_cli_command_119(280).
	p(NT(279), (NT(280)));
//G396: version_cmd(278)     => version_sym(279).
	p(NT(278), (NT(279)));
//G397: cli_command(272)     => version_cmd(278).
	p(NT(272), (NT(278)));
//G398: __E_cli_command_120(283) => 'c'.
	p(NT(283), (T(41)));
//G399: __E_cli_command_120(283) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(283), (T(41)+T(21)+T(16)+T(20)+T(44)));
//G400: clear_sym(282)       => __E_cli_command_120(283).
	p(NT(282), (NT(283)));
//G401: clear_cmd(281)       => clear_sym(282).
	p(NT(281), (NT(282)));
//G402: cli_command(272)     => clear_cmd(281).
	p(NT(272), (NT(281)));
//G403: __E___E_cli_command_121_122(287) => 'h'.
	p(NT(287), (T(49)));
//G404: __E___E_cli_command_121_122(287) => 'h' 'e' 'l' 'p'.
	p(NT(287), (T(49)+T(16)+T(21)+T(42)));
//G405: help_sym(286)        => __E___E_cli_command_121_122(287).
	p(NT(286), (NT(287)));
//G406: __E___E_cli_command_121_123(288) => __(72) help_arg(289).
	p(NT(288), (NT(72)+NT(289)));
//G407: __E___E_cli_command_121_123(288) => null.
	p(NT(288), (nul));
//G408: __E_cli_command_121(285) => help_sym(286) __E___E_cli_command_121_123(288).
	p(NT(285), (NT(286)+NT(288)));
//G409: help_cmd(284)        => __E_cli_command_121(285).
	p(NT(284), (NT(285)));
//G410: cli_command(272)     => help_cmd(284).
	p(NT(272), (NT(284)));
//G411: file_sym(292)        => 'f' 'i' 'l' 'e'.
	p(NT(292), (T(46)+T(18)+T(21)+T(16)));
//G412: __E_cli_command_124(291) => file_sym(292) __(72) q_string(293).
	p(NT(291), (NT(292)+NT(72)+NT(293)));
//G413: file_cmd(290)        => __E_cli_command_124(291).
	p(NT(290), (NT(291)));
//G414: cli_command(272)     => file_cmd(290).
	p(NT(272), (NT(290)));
//G415: valid_sym(296)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(296), (T(43)+T(20)+T(21)+T(18)+T(45)));
//G416: __E_cli_command_125(295) => valid_sym(296) __(72) nso_rr(14).
	p(NT(295), (NT(296)+NT(72)+NT(14)));
//G417: valid_cmd(294)       => __E_cli_command_125(295).
	p(NT(294), (NT(295)));
//G418: cli_command(272)     => valid_cmd(294).
	p(NT(272), (NT(294)));
//G419: sat_sym(299)         => 's' 'a' 't'.
	p(NT(299), (T(13)+T(20)+T(17)));
//G420: __E_cli_command_126(298) => sat_sym(299) __(72) nso_rr(14).
	p(NT(298), (NT(299)+NT(72)+NT(14)));
//G421: sat_cmd(297)         => __E_cli_command_126(298).
	p(NT(297), (NT(298)));
//G422: cli_command(272)     => sat_cmd(297).
	p(NT(272), (NT(297)));
//G423: unsat_sym(302)       => 'u' 'n' 's' 'a' 't'.
	p(NT(302), (T(40)+T(48)+T(13)+T(20)+T(17)));
//G424: __E_cli_command_127(301) => unsat_sym(302) __(72) nso_rr(14).
	p(NT(301), (NT(302)+NT(72)+NT(14)));
//G425: unsat_cmd(300)       => __E_cli_command_127(301).
	p(NT(300), (NT(301)));
//G426: cli_command(272)     => unsat_cmd(300).
	p(NT(272), (NT(300)));
//G427: solve_sym(305)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(305), (T(13)+T(14)+T(21)+T(43)+T(16)));
//G428: solve_cmd_arg(306)   => wff_cmd_arg(307).
	p(NT(306), (NT(307)));
//G429: __E_cli_command_128(304) => solve_sym(305) __(72) solve_cmd_arg(306).
	p(NT(304), (NT(305)+NT(72)+NT(306)));
//G430: solve_cmd(303)       => __E_cli_command_128(304).
	p(NT(303), (NT(304)));
//G431: cli_command(272)     => solve_cmd(303).
	p(NT(272), (NT(303)));
//G432: __E___E_cli_command_129_130(311) => 'e'.
	p(NT(311), (T(16)));
//G433: __E___E_cli_command_129_130(311) => 'e' 'x' 'e' 'c' 'u' 't' 'e'.
	p(NT(311), (T(16)+T(29)+T(16)+T(41)+T(40)+T(17)+T(16)));
//G434: execute_sym(310)     => __E___E_cli_command_129_130(311).
	p(NT(310), (NT(311)));
//G435: __E___E_cli_command_129_131(313) => wff_typed(314).
	p(NT(313), (NT(314)));
//G436: __E___E_cli_command_129_131(313) => nso_rr(14).
	p(NT(313), (NT(14)));
//G437: execute_cmd_arg(312) => __E___E_cli_command_129_131(313).
	p(NT(312), (NT(313)));
//G438: __E_cli_command_129(309) => execute_sym(310) __(72) execute_cmd_arg(312).
	p(NT(309), (NT(310)+NT(72)+NT(312)));
//G439: execute_cmd(308)     => __E_cli_command_129(309).
	p(NT(308), (NT(309)));
//G440: cli_command(272)     => execute_cmd(308).
	p(NT(272), (NT(308)));
//G441: __E___E_cli_command_132_133(318) => 'n'.
	p(NT(318), (T(48)));
//G442: __E___E_cli_command_132_133(318) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(318), (T(48)+T(14)+T(44)+T(15)+T(20)+T(21)+T(18)+T(51)+T(16)));
//G443: normalize_sym(317)   => __E___E_cli_command_132_133(318).
	p(NT(317), (NT(318)));
//G444: __E_cli_command_132(316) => normalize_sym(317) __(72) normalize_cmd_arg(319).
	p(NT(316), (NT(317)+NT(72)+NT(319)));
//G445: normalize_cmd(315)   => __E_cli_command_132(316).
	p(NT(315), (NT(316)));
//G446: cli_command(272)     => normalize_cmd(315).
	p(NT(272), (NT(315)));
//G447: __E___E_cli_command_134_135(323) => 'i'.
	p(NT(323), (T(18)));
//G448: __E___E_cli_command_134_135(323) => 'i' 'n' 's' 't'.
	p(NT(323), (T(18)+T(48)+T(13)+T(17)));
//G449: __E___E_cli_command_134_135(323) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(323), (T(18)+T(48)+T(13)+T(17)+T(20)+T(48)+T(17)+T(18)+T(20)+T(17)+T(16)));
//G450: inst_sym(322)        => __E___E_cli_command_134_135(323).
	p(NT(322), (NT(323)));
//G451: __E_cli_command_134(321) => inst_sym(322) __(72) variable(169) __(72) wff_cmd_arg(307) __(72) wff_cmd_out_arg(324).
	p(NT(321), (NT(322)+NT(72)+NT(169)+NT(72)+NT(307)+NT(72)+NT(324)));
//G452: wff_inst_cmd(320)    => __E_cli_command_134(321).
	p(NT(320), (NT(321)));
//G453: cli_command(272)     => wff_inst_cmd(320).
	p(NT(272), (NT(320)));
//G454: __E___E_cli_command_136_137(328) => 's'.
	p(NT(328), (T(13)));
//G455: __E___E_cli_command_136_137(328) => 's' 'u' 'b' 's' 't'.
	p(NT(328), (T(13)+T(40)+T(27)+T(13)+T(17)));
//G456: __E___E_cli_command_136_137(328) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(328), (T(13)+T(40)+T(27)+T(13)+T(17)+T(18)+T(17)+T(40)+T(17)+T(16)));
//G457: subst_sym(327)       => __E___E_cli_command_136_137(328).
	p(NT(327), (NT(328)));
//G458: __E_cli_command_136(326) => subst_sym(327) __(72) nf_cmd_arg(329) _(16) '[' _(16) nf_cmd_arg(329) _(16) '/' _(16) nf_cmd_arg(329) _(16) ']'.
	p(NT(326), (NT(327)+NT(72)+NT(329)+NT(16)+T(24)+NT(16)+NT(329)+NT(16)+T(57)+NT(16)+NT(329)+NT(16)+T(25)));
//G459: subst_cmd(325)       => __E_cli_command_136(326).
	p(NT(325), (NT(326)));
//G460: cli_command(272)     => subst_cmd(325).
	p(NT(272), (NT(325)));
//G461: __E_cli_command_138(331) => inst_sym(322) __(72) variable(169) __(72) bf_cmd_arg(332) __(72) bf_cmd_out_arg(333).
	p(NT(331), (NT(322)+NT(72)+NT(169)+NT(72)+NT(332)+NT(72)+NT(333)));
//G462: bf_inst_cmd(330)     => __E_cli_command_138(331).
	p(NT(330), (NT(331)));
//G463: cli_command(272)     => bf_inst_cmd(330).
	p(NT(272), (NT(330)));
//G464: dnf_sym(336)         => 'd' 'n' 'f'.
	p(NT(336), (T(45)+T(48)+T(46)));
//G465: __E_cli_command_139(335) => dnf_sym(336) __(72) nf_cmd_arg(329).
	p(NT(335), (NT(336)+NT(72)+NT(329)));
//G466: dnf_cmd(334)         => __E_cli_command_139(335).
	p(NT(334), (NT(335)));
//G467: cli_command(272)     => dnf_cmd(334).
	p(NT(272), (NT(334)));
//G468: cnf_sym(339)         => 'c' 'n' 'f'.
	p(NT(339), (T(41)+T(48)+T(46)));
//G469: __E_cli_command_140(338) => cnf_sym(339) __(72) nf_cmd_arg(329).
	p(NT(338), (NT(339)+NT(72)+NT(329)));
//G470: cnf_cmd(337)         => __E_cli_command_140(338).
	p(NT(337), (NT(338)));
//G471: cli_command(272)     => cnf_cmd(337).
	p(NT(272), (NT(337)));
//G472: anf_sym(342)         => 'a' 'n' 'f'.
	p(NT(342), (T(20)+T(48)+T(46)));
//G473: __E_cli_command_141(341) => anf_sym(342) __(72) nf_cmd_arg(329).
	p(NT(341), (NT(342)+NT(72)+NT(329)));
//G474: anf_cmd(340)         => __E_cli_command_141(341).
	p(NT(340), (NT(341)));
//G475: cli_command(272)     => anf_cmd(340).
	p(NT(272), (NT(340)));
//G476: nnf_sym(345)         => 'n' 'n' 'f'.
	p(NT(345), (T(48)+T(48)+T(46)));
//G477: __E_cli_command_142(344) => nnf_sym(345) __(72) nf_cmd_arg(329).
	p(NT(344), (NT(345)+NT(72)+NT(329)));
//G478: nnf_cmd(343)         => __E_cli_command_142(344).
	p(NT(343), (NT(344)));
//G479: cli_command(272)     => nnf_cmd(343).
	p(NT(272), (NT(343)));
//G480: pnf_sym(348)         => 'p' 'n' 'f'.
	p(NT(348), (T(42)+T(48)+T(46)));
//G481: __E_cli_command_143(347) => pnf_sym(348) __(72) nf_cmd_arg(329).
	p(NT(347), (NT(348)+NT(72)+NT(329)));
//G482: pnf_cmd(346)         => __E_cli_command_143(347).
	p(NT(346), (NT(347)));
//G483: cli_command(272)     => pnf_cmd(346).
	p(NT(272), (NT(346)));
//G484: mnf_sym(351)         => 'm' 'n' 'f'.
	p(NT(351), (T(15)+T(48)+T(46)));
//G485: __E_cli_command_144(350) => mnf_sym(351) __(72) nf_cmd_arg(329).
	p(NT(350), (NT(351)+NT(72)+NT(329)));
//G486: mnf_cmd(349)         => __E_cli_command_144(350).
	p(NT(349), (NT(350)));
//G487: cli_command(272)     => mnf_cmd(349).
	p(NT(272), (NT(349)));
//G488: snf_sym(354)         => 's' 'n' 'f'.
	p(NT(354), (T(13)+T(48)+T(46)));
//G489: __E_cli_command_145(353) => snf_sym(354) __(72) nf_cmd_arg(329).
	p(NT(353), (NT(354)+NT(72)+NT(329)));
//G490: snf_cmd(352)         => __E_cli_command_145(353).
	p(NT(352), (NT(353)));
//G491: cli_command(272)     => snf_cmd(352).
	p(NT(272), (NT(352)));
//G492: onf_sym(357)         => 'o' 'n' 'f'.
	p(NT(357), (T(14)+T(48)+T(46)));
//G493: __E_cli_command_146(356) => onf_sym(357) __(72) variable(169) __(72) onf_cmd_arg(358).
	p(NT(356), (NT(357)+NT(72)+NT(169)+NT(72)+NT(358)));
//G494: onf_cmd(355)         => __E_cli_command_146(356).
	p(NT(355), (NT(356)));
//G495: cli_command(272)     => onf_cmd(355).
	p(NT(272), (NT(355)));
//G496: __E___E_cli_command_147_148(362) => 'd' 'e' 'f' 's'.
	p(NT(362), (T(45)+T(16)+T(46)+T(13)));
//G497: __E___E_cli_command_147_148(362) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(362), (T(45)+T(16)+T(46)+T(18)+T(48)+T(18)+T(17)+T(18)+T(14)+T(48)+T(13)));
//G498: def_sym(361)         => __E___E_cli_command_147_148(362).
	p(NT(361), (NT(362)));
//G499: __E_cli_command_147(360) => def_sym(361).
	p(NT(360), (NT(361)));
//G500: def_list_cmd(359)    => __E_cli_command_147(360).
	p(NT(359), (NT(360)));
//G501: cli_command(272)     => def_list_cmd(359).
	p(NT(272), (NT(359)));
//G502: __E_cli_command_149(364) => nso_rec_relation_form(36).
	p(NT(364), (NT(36)));
//G503: def_rr_cmd(363)      => __E_cli_command_149(364).
	p(NT(363), (NT(364)));
//G504: cli_command(272)     => def_rr_cmd(363).
	p(NT(272), (NT(363)));
//G505: __E_cli_command_150(366) => def_sym(361) __(72) memory(367).
	p(NT(366), (NT(361)+NT(72)+NT(367)));
//G506: def_print_cmd(365)   => __E_cli_command_150(366).
	p(NT(365), (NT(366)));
//G507: cli_command(272)     => def_print_cmd(365).
	p(NT(272), (NT(365)));
//G508: __E___E_cli_command_151_152(371) => 'h' 'i' 's' 't'.
	p(NT(371), (T(49)+T(18)+T(13)+T(17)));
//G509: __E___E_cli_command_151_152(371) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(371), (T(49)+T(18)+T(13)+T(17)+T(14)+T(44)+T(23)));
//G510: history_sym(370)     => __E___E_cli_command_151_152(371).
	p(NT(370), (NT(371)));
//G511: __E_cli_command_151(369) => history_sym(370).
	p(NT(369), (NT(370)));
//G512: history_list_cmd(368) => __E_cli_command_151(369).
	p(NT(368), (NT(369)));
//G513: cli_command(272)     => history_list_cmd(368).
	p(NT(272), (NT(368)));
//G514: __E_cli_command_153(373) => history_sym(370) __(72) memory(367).
	p(NT(373), (NT(370)+NT(72)+NT(367)));
//G515: history_print_cmd(372) => __E_cli_command_153(373).
	p(NT(372), (NT(373)));
//G516: cli_command(272)     => history_print_cmd(372).
	p(NT(272), (NT(372)));
//G517: __E___E_cli_command_154_155(377) => bf_typed(378).
	p(NT(377), (NT(378)));
//G518: __E___E_cli_command_154_155(377) => wff_typed(314).
	p(NT(377), (NT(314)));
//G519: history_store_cmd_arg(376) => __E___E_cli_command_154_155(377).
	p(NT(376), (NT(377)));
//G520: __E_cli_command_154(375) => history_store_cmd_arg(376).
	p(NT(375), (NT(376)));
//G521: history_store_cmd(374) => __E_cli_command_154(375).
	p(NT(374), (NT(375)));
//G522: cli_command(272)     => history_store_cmd(374).
	p(NT(272), (NT(374)));
//G523: qelim_sym(381)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(381), (T(47)+T(16)+T(21)+T(18)+T(15)));
//G524: __E_cli_command_156(380) => qelim_sym(381) __(72) wff_cmd_arg(307).
	p(NT(380), (NT(381)+NT(72)+NT(307)));
//G525: qelim_cmd(379)       => __E_cli_command_156(380).
	p(NT(379), (NT(380)));
//G526: cli_command(272)     => qelim_cmd(379).
	p(NT(272), (NT(379)));
//G527: get_sym(384)         => 'g' 'e' 't'.
	p(NT(384), (T(50)+T(16)+T(17)));
//G528: __E___E_cli_command_157_158(385) => __(72) option(386).
	p(NT(385), (NT(72)+NT(386)));
//G529: __E___E_cli_command_157_158(385) => null.
	p(NT(385), (nul));
//G530: __E_cli_command_157(383) => get_sym(384) __E___E_cli_command_157_158(385).
	p(NT(383), (NT(384)+NT(385)));
//G531: get_cmd(382)         => __E_cli_command_157(383).
	p(NT(382), (NT(383)));
//G532: cli_command(272)     => get_cmd(382).
	p(NT(272), (NT(382)));
//G533: set_sym(389)         => 's' 'e' 't'.
	p(NT(389), (T(13)+T(16)+T(17)));
//G534: __E___E_cli_command_159_160(390) => __(72).
	p(NT(390), (NT(72)));
//G535: __E___E_cli_command_159_160(390) => _(16) '=' _(16).
	p(NT(390), (NT(16)+T(4)+NT(16)));
//G536: __E_cli_command_159(388) => set_sym(389) __(72) option(386) __E___E_cli_command_159_160(390) option_value(391).
	p(NT(388), (NT(389)+NT(72)+NT(386)+NT(390)+NT(391)));
//G537: set_cmd(387)         => __E_cli_command_159(388).
	p(NT(387), (NT(388)));
//G538: cli_command(272)     => set_cmd(387).
	p(NT(272), (NT(387)));
//G539: toggle_sym(394)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(394), (T(17)+T(14)+T(50)+T(50)+T(21)+T(16)));
//G540: __E_cli_command_161(393) => toggle_sym(394) __(72) bool_option(395).
	p(NT(393), (NT(394)+NT(72)+NT(395)));
//G541: toggle_cmd(392)      => __E_cli_command_161(393).
	p(NT(392), (NT(393)));
//G542: cli_command(272)     => toggle_cmd(392).
	p(NT(272), (NT(392)));
//G543: cli_command(272)     => comment(265).
	p(NT(272), (NT(265)));
//G544: bf_typed(378)        => bf(41).
	p(NT(378), (NT(41)));
//G545: bf_typed(378)        => bf(41) _(16) ':' _(16) 'b' 'f'.
	p(NT(378), (NT(41)+NT(16)+T(3)+NT(16)+T(27)+T(46)));
//G546: wff_typed(314)       => wff(32).
	p(NT(314), (NT(32)));
//G547: wff_typed(314)       => wff(32) _(16) ':' _(16) 'w' 'f' 'f'.
	p(NT(314), (NT(32)+NT(16)+T(3)+NT(16)+T(22)+T(46)+T(46)));
//G548: bf_cmd_arg(332)      => memory(367).
	p(NT(332), (NT(367)));
//G549: bf_cmd_arg(332)      => bf_selection(396).
	p(NT(332), (NT(396)));
//G550: bf_cmd_arg(332)      => bf_typed(378).
	p(NT(332), (NT(378)));
//G551: bf_cmd_out_arg(333)  => memory(367).
	p(NT(333), (NT(367)));
//G552: bf_cmd_out_arg(333)  => bf_selection(396).
	p(NT(333), (NT(396)));
//G553: wff_cmd_arg(307)     => memory(367).
	p(NT(307), (NT(367)));
//G554: wff_cmd_arg(307)     => wff_selection(397).
	p(NT(307), (NT(397)));
//G555: wff_cmd_arg(307)     => wff_typed(314).
	p(NT(307), (NT(314)));
//G556: wff_cmd_out_arg(324) => memory(367).
	p(NT(324), (NT(367)));
//G557: wff_cmd_out_arg(324) => wff_selection(397).
	p(NT(324), (NT(397)));
//G558: nf_cmd_arg(329)      => memory(367).
	p(NT(329), (NT(367)));
//G559: nf_cmd_arg(329)      => bf_typed(378).
	p(NT(329), (NT(378)));
//G560: nf_cmd_arg(329)      => wff_typed(314).
	p(NT(329), (NT(314)));
//G561: onf_cmd_arg(358)     => memory(367).
	p(NT(358), (NT(367)));
//G562: onf_cmd_arg(358)     => wff_typed(314).
	p(NT(358), (NT(314)));
//G563: normalize_cmd_arg(319) => memory(367).
	p(NT(319), (NT(367)));
//G564: normalize_cmd_arg(319) => bf_typed(378).
	p(NT(319), (NT(378)));
//G565: normalize_cmd_arg(319) => wff_typed(314).
	p(NT(319), (NT(314)));
//G566: normalize_cmd_arg(319) => nso_rr(14).
	p(NT(319), (NT(14)));
//G567: help_arg(289)        => help_sym(286).
	p(NT(289), (NT(286)));
//G568: help_arg(289)        => version_sym(279).
	p(NT(289), (NT(279)));
//G569: help_arg(289)        => quit_sym(276).
	p(NT(289), (NT(276)));
//G570: help_arg(289)        => clear_sym(282).
	p(NT(289), (NT(282)));
//G571: help_arg(289)        => get_sym(384).
	p(NT(289), (NT(384)));
//G572: help_arg(289)        => set_sym(389).
	p(NT(289), (NT(389)));
//G573: help_arg(289)        => toggle_sym(394).
	p(NT(289), (NT(394)));
//G574: help_arg(289)        => file_sym(292).
	p(NT(289), (NT(292)));
//G575: help_arg(289)        => history_sym(370).
	p(NT(289), (NT(370)));
//G576: help_arg(289)        => abs_memory_sym(398).
	p(NT(289), (NT(398)));
//G577: help_arg(289)        => rel_memory_sym(399).
	p(NT(289), (NT(399)));
//G578: help_arg(289)        => selection_sym(400).
	p(NT(289), (NT(400)));
//G579: help_arg(289)        => def_sym(361).
	p(NT(289), (NT(361)));
//G580: help_arg(289)        => inst_sym(322).
	p(NT(289), (NT(322)));
//G581: help_arg(289)        => subst_sym(327).
	p(NT(289), (NT(327)));
//G582: help_arg(289)        => normalize_sym(317).
	p(NT(289), (NT(317)));
//G583: help_arg(289)        => execute_sym(310).
	p(NT(289), (NT(310)));
//G584: help_arg(289)        => solve_sym(305).
	p(NT(289), (NT(305)));
//G585: help_arg(289)        => valid_sym(296).
	p(NT(289), (NT(296)));
//G586: help_arg(289)        => sat_sym(299).
	p(NT(289), (NT(299)));
//G587: help_arg(289)        => unsat_sym(302).
	p(NT(289), (NT(302)));
//G588: help_arg(289)        => dnf_sym(336).
	p(NT(289), (NT(336)));
//G589: help_arg(289)        => cnf_sym(339).
	p(NT(289), (NT(339)));
//G590: help_arg(289)        => anf_sym(342).
	p(NT(289), (NT(342)));
//G591: help_arg(289)        => snf_sym(354).
	p(NT(289), (NT(354)));
//G592: help_arg(289)        => nnf_sym(345).
	p(NT(289), (NT(345)));
//G593: help_arg(289)        => pnf_sym(348).
	p(NT(289), (NT(348)));
//G594: help_arg(289)        => mnf_sym(351).
	p(NT(289), (NT(351)));
//G595: help_arg(289)        => onf_sym(357).
	p(NT(289), (NT(357)));
//G596: help_arg(289)        => qelim_sym(381).
	p(NT(289), (NT(381)));
//G597: __E___E_help_arg_162_163(403) => 's'.
	p(NT(403), (T(13)));
//G598: __E___E_help_arg_162_163(403) => null.
	p(NT(403), (nul));
//G599: __E_help_arg_162(402) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_162_163(403).
	p(NT(402), (T(16)+T(29)+T(20)+T(15)+T(42)+T(21)+T(16)+NT(403)));
//G600: examples_sym(401)    => __E_help_arg_162(402).
	p(NT(401), (NT(402)));
//G601: help_arg(289)        => examples_sym(401).
	p(NT(289), (NT(401)));
//G602: rel_memory_sym(399)  => '%' '-'.
	p(NT(399), (T(58)+T(12)));
//G603: memory_id(408)       => digits(237).
	p(NT(408), (NT(237)));
//G604: __E___E___E_memory_164_165_166(407) => memory_id(408).
	p(NT(407), (NT(408)));
//G605: __E___E___E_memory_164_165_166(407) => null.
	p(NT(407), (nul));
//G606: __E___E_memory_164_165(406) => rel_memory_sym(399) __E___E___E_memory_164_165_166(407).
	p(NT(406), (NT(399)+NT(407)));
//G607: __E___E_memory_164_165(406) => '%'.
	p(NT(406), (T(58)));
//G608: __E_memory_164(405)  => __E___E_memory_164_165(406).
	p(NT(405), (NT(406)));
//G609: rel_memory(404)      => __E_memory_164(405).
	p(NT(404), (NT(405)));
//G610: memory(367)          => rel_memory(404).
	p(NT(367), (NT(404)));
//G611: abs_memory_sym(398)  => '%'.
	p(NT(398), (T(58)));
//G612: __E_memory_167(410)  => abs_memory_sym(398) memory_id(408).
	p(NT(410), (NT(398)+NT(408)));
//G613: abs_memory(409)      => __E_memory_167(410).
	p(NT(409), (NT(410)));
//G614: memory(367)          => abs_memory(409).
	p(NT(367), (NT(409)));
//G615: selection_sym(400)   => 's'.
	p(NT(400), (T(13)));
//G616: selection_sym(400)   => 's' 'e' 'l' 'e' 'c' 't' 'i' 'o' 'n'.
	p(NT(400), (T(13)+T(16)+T(21)+T(16)+T(41)+T(17)+T(18)+T(14)+T(48)));
//G617: wff_selection(397)   => selection_sym(400) __(72) digits(237).
	p(NT(397), (NT(400)+NT(72)+NT(237)));
//G618: bf_selection(396)    => selection_sym(400) __(72) digits(237).
	p(NT(396), (NT(400)+NT(72)+NT(237)));
//G619: bf_var_selection(411) => selection_sym(400) __(72) digits(237).
	p(NT(411), (NT(400)+NT(72)+NT(237)));
//G620: wff_var_selection(412) => selection_sym(400) __(72) digits(237).
	p(NT(412), (NT(400)+NT(72)+NT(237)));
//G621: option(386)          => bool_option(395).
	p(NT(386), (NT(395)));
//G622: __E_option_168(415)  => 's' 'e' 'v'.
	p(NT(415), (T(13)+T(16)+T(43)));
//G623: __E_option_168(415)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(415), (T(13)+T(16)+T(43)+T(16)+T(44)+T(18)+T(17)+T(23)));
//G624: severity_opt(414)    => __E_option_168(415).
	p(NT(414), (NT(415)));
//G625: enum_option(413)     => severity_opt(414).
	p(NT(413), (NT(414)));
//G626: option(386)          => enum_option(413).
	p(NT(386), (NT(413)));
//G627: __E_bool_option_169(417) => 's'.
	p(NT(417), (T(13)));
//G628: __E_bool_option_169(417) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(417), (T(13)+T(17)+T(20)+T(17)+T(40)+T(13)));
//G629: status_opt(416)      => __E_bool_option_169(417).
	p(NT(416), (NT(417)));
//G630: bool_option(395)     => status_opt(416).
	p(NT(395), (NT(416)));
//G631: __E_bool_option_170(419) => 'c'.
	p(NT(419), (T(41)));
//G632: __E_bool_option_170(419) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(419), (T(41)+T(14)+T(21)+T(14)+T(44)));
//G633: __E_bool_option_170(419) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(419), (T(41)+T(14)+T(21)+T(14)+T(44)+T(13)));
//G634: colors_opt(418)      => __E_bool_option_170(419).
	p(NT(418), (NT(419)));
//G635: bool_option(395)     => colors_opt(418).
	p(NT(395), (NT(418)));
//G636: __E_bool_option_171(421) => 'd'.
	p(NT(421), (T(45)));
//G637: __E_bool_option_171(421) => 'd' 'b' 'g'.
	p(NT(421), (T(45)+T(27)+T(50)));
//G638: __E_bool_option_171(421) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(421), (T(45)+T(16)+T(27)+T(40)+T(50)));
//G639: debug_repl_opt(420)  => __E_bool_option_171(421).
	p(NT(420), (NT(421)));
//G640: bool_option(395)     => debug_repl_opt(420).
	p(NT(395), (NT(420)));
//G641: option_value(391)    => option_value_true(422).
	p(NT(391), (NT(422)));
//G642: option_value(391)    => option_value_false(423).
	p(NT(391), (NT(423)));
//G643: option_value(391)    => severity(424).
	p(NT(391), (NT(424)));
//G644: option_value_true(422) => 't'.
	p(NT(422), (T(17)));
//G645: option_value_true(422) => 't' 'r' 'u' 'e'.
	p(NT(422), (T(17)+T(44)+T(40)+T(16)));
//G646: option_value_true(422) => 'o' 'n'.
	p(NT(422), (T(14)+T(48)));
//G647: option_value_true(422) => '1'.
	p(NT(422), (T(37)));
//G648: option_value_true(422) => 'y'.
	p(NT(422), (T(23)));
//G649: option_value_true(422) => 'y' 'e' 's'.
	p(NT(422), (T(23)+T(16)+T(13)));
//G650: option_value_false(423) => 'f'.
	p(NT(423), (T(46)));
//G651: option_value_false(423) => 'f' 'a' 'l' 's' 'e'.
	p(NT(423), (T(46)+T(20)+T(21)+T(13)+T(16)));
//G652: option_value_false(423) => 'o' 'f' 'f'.
	p(NT(423), (T(14)+T(46)+T(46)));
//G653: option_value_false(423) => '0'.
	p(NT(423), (T(38)));
//G654: option_value_false(423) => 'n'.
	p(NT(423), (T(48)));
//G655: option_value_false(423) => 'n' 'o'.
	p(NT(423), (T(48)+T(14)));
//G656: __E_severity_172(426) => 'e'.
	p(NT(426), (T(16)));
//G657: __E_severity_172(426) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(426), (T(16)+T(44)+T(44)+T(14)+T(44)));
//G658: error_sym(425)       => __E_severity_172(426).
	p(NT(425), (NT(426)));
//G659: severity(424)        => error_sym(425).
	p(NT(424), (NT(425)));
//G660: __E_severity_173(428) => 'i'.
	p(NT(428), (T(18)));
//G661: __E_severity_173(428) => 'i' 'n' 'f' 'o'.
	p(NT(428), (T(18)+T(48)+T(46)+T(14)));
//G662: info_sym(427)        => __E_severity_173(428).
	p(NT(427), (NT(428)));
//G663: severity(424)        => info_sym(427).
	p(NT(424), (NT(427)));
//G664: __E_severity_174(430) => 'd'.
	p(NT(430), (T(45)));
//G665: __E_severity_174(430) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(430), (T(45)+T(16)+T(27)+T(40)+T(50)));
//G666: debug_sym(429)       => __E_severity_174(430).
	p(NT(429), (NT(430)));
//G667: severity(424)        => debug_sym(429).
	p(NT(424), (NT(429)));
//G668: __E_severity_175(432) => 't'.
	p(NT(432), (T(17)));
//G669: __E_severity_175(432) => 't' 'r' 'a' 'c' 'e'.
	p(NT(432), (T(17)+T(44)+T(20)+T(41)+T(16)));
//G670: trace_sym(431)       => __E_severity_175(432).
	p(NT(431), (NT(432)));
//G671: severity(424)        => trace_sym(431).
	p(NT(424), (NT(431)));
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
		__E_tau_27, tau_and, __E_tau_28, tau_neg, __E_tau_29, wff_parenthesis, __E_wff_30, wff_sometimes, __E_wff_31, __E___E_wff_31_32, 
		wff_always, __E_wff_33, __E___E_wff_33_34, wff_conditional, __E_wff_35, wff_ball, __E_wff_36, q_bool_var, wff_bex, __E_wff_37, 
		wff_all, __E_wff_38, q_var, wff_ex, __E_wff_39, __E_wff_40, __E___E_wff_40_41, wff_imply, __E_wff_42, wff_equiv, 
		__E_wff_43, wff_or, __E_wff_44, wff_xor, __E_wff_45, wff_and, __E_wff_46, wff_neg, __E_wff_47, wff_t, 
		wff_f, bf_interval, __E_wff_48, bf_neq, __E_wff_49, bf_eq, __E_wff_50, bf_nleq_lower, __E_wff_51, bf_nleq_upper, 
		__E_wff_52, bf_greater, __E_wff_53, bf_less_equal, __E_wff_54, bf_less, __E_wff_55, bool_variable, uninterpreted_constant, bf_parenthesis, 
		__E_bf_56, bf_splitter, __E_bf_57, __E_bf_58, __E___E_bf_58_59, bf_or, __E_bf_60, bf_xor, __E_bf_61, bf_and, 
		__E_bf_62, __E___E_bf_62_63, bf_neg, __E_bf_64, bf_constant, __E_bf_65, constant, bf_t, bf_f, variable, 
		__E_ref_args_66, __E___E_ref_args_66_67, ref_arg, __E___E_ref_args_66_68, __E___E_ref_args_66_69, bf_cb_args1, bf_cb_arg, bf_cb_args2, bf_cb_args3, bf_cb_args4, 
		wff_cb_args1, wff_cb_arg, wff_cb_args2, wff_cb_args3, wff_cb_args4, tau_cb_args1, tau_cb_arg, __E_tau_cb_args1_70, tau_cb_args2, tau_cb_args3, 
		tau_collapse_positives_cb, __E_tau_cb_71, __E___E_tau_cb_71_72, tau_positives_upwards_cb, __E_tau_cb_73, bf_eq_cb, __E_wff_cb_74, bf_neq_cb, __E_wff_cb_75, wff_has_clashing_subformulas_cb, 
		__E_wff_cb_76, wff_has_subformula_cb, __E_wff_cb_77, wff_remove_existential_cb, __E_wff_cb_78, wff_remove_bexistential_cb, __E_wff_cb_79, wff_remove_buniversal_cb, __E_wff_cb_80, bf_is_zero_cb, 
		__E_bf_cb_81, bf_is_one_cb, __E_bf_cb_82, bf_has_subformula_cb, __E_bf_cb_83, bf_remove_funiversal_cb, __E_bf_cb_84, bf_remove_fexistential_cb, __E_bf_cb_85, bf_and_cb, 
		__E_bf_cb_86, bf_or_cb, __E_bf_cb_87, bf_xor_cb, __E_bf_cb_88, bf_neg_cb, __E_bf_cb_89, bf_normalize_cb, __E_bf_cb_90, charvar, 
		__E_charvar_91, __E_charvar_92, io_var, __E_variable_93, out, in_var_name, __E_in_94, digits, offset, out_var_name, 
		__E_out_95, __E_offsets_96, __E_offsets_97, num, shift, __E_offset_98, __E_shift_99, __E___E_shift_99_100, uninter_const_name, __E_uninterpreted_constant_101, 
		__E___E_uninterpreted_constant_101_102, chars, binding, __E_binding_103, type, __E___E_binding_103_104, source, named_binding, src_c, __E_source_105, 
		__E_source_106, __E___E_source_106_107, __E___E_source_106_108, __E_chars_109, __E_digits_110, comment, __E_comment_111, __E_comment_112, __E_comment_113, __E____114, 
		__E___115, cli, cli_command, __E_cli_116, __E_cli_117, quit_cmd, quit_sym, __E_cli_command_118, version_cmd, version_sym, 
		__E_cli_command_119, clear_cmd, clear_sym, __E_cli_command_120, help_cmd, __E_cli_command_121, help_sym, __E___E_cli_command_121_122, __E___E_cli_command_121_123, help_arg, 
		file_cmd, __E_cli_command_124, file_sym, q_string, valid_cmd, __E_cli_command_125, valid_sym, sat_cmd, __E_cli_command_126, sat_sym, 
		unsat_cmd, __E_cli_command_127, unsat_sym, solve_cmd, __E_cli_command_128, solve_sym, solve_cmd_arg, wff_cmd_arg, execute_cmd, __E_cli_command_129, 
		execute_sym, __E___E_cli_command_129_130, execute_cmd_arg, __E___E_cli_command_129_131, wff_typed, normalize_cmd, __E_cli_command_132, normalize_sym, __E___E_cli_command_132_133, normalize_cmd_arg, 
		wff_inst_cmd, __E_cli_command_134, inst_sym, __E___E_cli_command_134_135, wff_cmd_out_arg, subst_cmd, __E_cli_command_136, subst_sym, __E___E_cli_command_136_137, nf_cmd_arg, 
		bf_inst_cmd, __E_cli_command_138, bf_cmd_arg, bf_cmd_out_arg, dnf_cmd, __E_cli_command_139, dnf_sym, cnf_cmd, __E_cli_command_140, cnf_sym, 
		anf_cmd, __E_cli_command_141, anf_sym, nnf_cmd, __E_cli_command_142, nnf_sym, pnf_cmd, __E_cli_command_143, pnf_sym, mnf_cmd, 
		__E_cli_command_144, mnf_sym, snf_cmd, __E_cli_command_145, snf_sym, onf_cmd, __E_cli_command_146, onf_sym, onf_cmd_arg, def_list_cmd, 
		__E_cli_command_147, def_sym, __E___E_cli_command_147_148, def_rr_cmd, __E_cli_command_149, def_print_cmd, __E_cli_command_150, memory, history_list_cmd, __E_cli_command_151, 
		history_sym, __E___E_cli_command_151_152, history_print_cmd, __E_cli_command_153, history_store_cmd, __E_cli_command_154, history_store_cmd_arg, __E___E_cli_command_154_155, bf_typed, qelim_cmd, 
		__E_cli_command_156, qelim_sym, get_cmd, __E_cli_command_157, get_sym, __E___E_cli_command_157_158, option, set_cmd, __E_cli_command_159, set_sym, 
		__E___E_cli_command_159_160, option_value, toggle_cmd, __E_cli_command_161, toggle_sym, bool_option, bf_selection, wff_selection, abs_memory_sym, rel_memory_sym, 
		selection_sym, examples_sym, __E_help_arg_162, __E___E_help_arg_162_163, rel_memory, __E_memory_164, __E___E_memory_164_165, __E___E___E_memory_164_165_166, memory_id, abs_memory, 
		__E_memory_167, bf_var_selection, wff_var_selection, enum_option, severity_opt, __E_option_168, status_opt, __E_bool_option_169, colors_opt, __E_bool_option_170, 
		debug_repl_opt, __E_bool_option_171, option_value_true, option_value_false, severity, error_sym, __E_severity_172, info_sym, __E_severity_173, debug_sym, 
		__E_severity_174, trace_sym, __E_severity_175, bf_all, bf_ex, del_sym, __N_0, __N_1, __N_2, __N_3, 
		__N_4, __N_5, 
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
