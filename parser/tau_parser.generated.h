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
	"__E_tau_27", "tau_and", "__E_tau_28", "tau_neg", "__E_tau_29", "wff_parenthesis", "__E_wff_30", "global_or", "__E_wff_31", "global_and", 
	"__E_wff_32", "global_neg", "__E_wff_33", "wff_sometimes", "__E_wff_34", "__E___E_wff_34_35", "wff_always", "__E_wff_36", "__E___E_wff_36_37", "wff_conditional", 
	"__E_wff_38", "wff_ball", "__E_wff_39", "q_bool_var", "wff_bex", "__E_wff_40", "wff_all", "__E_wff_41", "q_var", "wff_ex", 
	"__E_wff_42", "__E_wff_43", "__E___E_wff_43_44", "wff_imply", "__E_wff_45", "wff_equiv", "__E_wff_46", "wff_or", "__E_wff_47", "wff_xor", 
	"__E_wff_48", "wff_and", "__E_wff_49", "wff_neg", "__E_wff_50", "wff_t", "wff_f", "bf_interval", "__E_wff_51", "bf_neq", 
	"__E_wff_52", "bf_eq", "__E_wff_53", "bf_nleq_lower", "__E_wff_54", "bf_nleq_upper", "__E_wff_55", "bf_greater", "__E_wff_56", "bf_less_equal", 
	"__E_wff_57", "bf_less", "__E_wff_58", "bool_variable", "uninterpreted_constant", "bf_parenthesis", "__E_bf_59", "bf_splitter", "__E_bf_60", "__E_bf_61", 
	"__E___E_bf_61_62", "bf_or", "__E_bf_63", "bf_xor", "__E_bf_64", "bf_and", "__E_bf_65", "__E___E_bf_65_66", "bf_neg", "__E_bf_67", 
	"bf_constant", "__E_bf_68", "constant", "bf_t", "bf_f", "variable", "__E_ref_args_69", "__E___E_ref_args_69_70", "ref_arg", "__E___E_ref_args_69_71", 
	"__E___E_ref_args_69_72", "bf_cb_args1", "bf_cb_arg", "bf_cb_args2", "bf_cb_args3", "bf_cb_args4", "wff_cb_args1", "wff_cb_arg", "wff_cb_args2", "wff_cb_args3", 
	"wff_cb_args4", "tau_cb_args1", "tau_cb_arg", "__E_tau_cb_args1_73", "tau_cb_args2", "tau_cb_args3", "tau_collapse_positives_cb", "__E_tau_cb_74", "__E___E_tau_cb_74_75", "tau_positives_upwards_cb", 
	"__E_tau_cb_76", "bf_eq_cb", "__E_wff_cb_77", "bf_neq_cb", "__E_wff_cb_78", "wff_has_clashing_subformulas_cb", "__E_wff_cb_79", "wff_has_subformula_cb", "__E_wff_cb_80", "wff_remove_existential_cb", 
	"__E_wff_cb_81", "wff_remove_bexistential_cb", "__E_wff_cb_82", "wff_remove_buniversal_cb", "__E_wff_cb_83", "bf_is_zero_cb", "__E_bf_cb_84", "bf_is_one_cb", "__E_bf_cb_85", "bf_has_subformula_cb", 
	"__E_bf_cb_86", "bf_remove_funiversal_cb", "__E_bf_cb_87", "bf_remove_fexistential_cb", "__E_bf_cb_88", "bf_and_cb", "__E_bf_cb_89", "bf_or_cb", "__E_bf_cb_90", "bf_xor_cb", 
	"__E_bf_cb_91", "bf_neg_cb", "__E_bf_cb_92", "bf_normalize_cb", "__E_bf_cb_93", "charvar", "__E_charvar_94", "__E_charvar_95", "io_var", "__E_variable_96", 
	"out", "in_var_name", "__E_in_97", "digits", "offset", "out_var_name", "__E_out_98", "__E_offsets_99", "__E_offsets_100", "num", 
	"shift", "__E_offset_101", "__E_shift_102", "__E___E_shift_102_103", "uninter_const_name", "__E_uninterpreted_constant_104", "__E___E_uninterpreted_constant_104_105", "chars", "binding", "__E_binding_106", 
	"type", "__E___E_binding_106_107", "source", "named_binding", "src_c", "__E_source_108", "__E_source_109", "__E___E_source_109_110", "__E___E_source_109_111", "__E_chars_112", 
	"__E_digits_113", "comment", "__E_comment_114", "__E_comment_115", "__E_comment_116", "__E____117", "__E___118", "cli", "cli_command", "__E_cli_119", 
	"__E_cli_120", "quit_cmd", "quit_sym", "__E_cli_command_121", "version_cmd", "version_sym", "__E_cli_command_122", "clear_cmd", "clear_sym", "__E_cli_command_123", 
	"help_cmd", "__E_cli_command_124", "help_sym", "__E___E_cli_command_124_125", "__E___E_cli_command_124_126", "help_arg", "file_cmd", "__E_cli_command_127", "file_sym", "q_string", 
	"valid_cmd", "__E_cli_command_128", "valid_sym", "sat_cmd", "__E_cli_command_129", "sat_sym", "unsat_cmd", "__E_cli_command_130", "unsat_sym", "solve_cmd", 
	"__E_cli_command_131", "solve_sym", "solve_cmd_arg", "wff_cmd_arg", "execute_cmd", "__E_cli_command_132", "execute_sym", "__E___E_cli_command_132_133", "execute_cmd_arg", "__E___E_cli_command_132_134", 
	"wff_typed", "normalize_cmd", "__E_cli_command_135", "normalize_sym", "__E___E_cli_command_135_136", "normalize_cmd_arg", "wff_inst_cmd", "__E_cli_command_137", "inst_sym", "__E___E_cli_command_137_138", 
	"wff_cmd_out_arg", "subst_cmd", "__E_cli_command_139", "subst_sym", "__E___E_cli_command_139_140", "nf_cmd_arg", "bf_inst_cmd", "__E_cli_command_141", "bf_cmd_arg", "bf_cmd_out_arg", 
	"dnf_cmd", "__E_cli_command_142", "dnf_sym", "cnf_cmd", "__E_cli_command_143", "cnf_sym", "anf_cmd", "__E_cli_command_144", "anf_sym", "nnf_cmd", 
	"__E_cli_command_145", "nnf_sym", "pnf_cmd", "__E_cli_command_146", "pnf_sym", "mnf_cmd", "__E_cli_command_147", "mnf_sym", "snf_cmd", "__E_cli_command_148", 
	"snf_sym", "onf_cmd", "__E_cli_command_149", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_150", "def_sym", "__E___E_cli_command_150_151", "def_rr_cmd", 
	"__E_cli_command_152", "def_print_cmd", "__E_cli_command_153", "memory", "history_list_cmd", "__E_cli_command_154", "history_sym", "__E___E_cli_command_154_155", "history_print_cmd", "__E_cli_command_156", 
	"history_store_cmd", "__E_cli_command_157", "history_store_cmd_arg", "__E___E_cli_command_157_158", "bf_typed", "qelim_cmd", "__E_cli_command_159", "qelim_sym", "get_cmd", "__E_cli_command_160", 
	"get_sym", "__E___E_cli_command_160_161", "option", "set_cmd", "__E_cli_command_162", "set_sym", "__E___E_cli_command_162_163", "option_value", "toggle_cmd", "__E_cli_command_164", 
	"toggle_sym", "bool_option", "bf_selection", "wff_selection", "abs_memory_sym", "rel_memory_sym", "selection_sym", "examples_sym", "__E_help_arg_165", "__E___E_help_arg_165_166", 
	"rel_memory", "__E_memory_167", "__E___E_memory_167_168", "__E___E___E_memory_167_168_169", "memory_id", "abs_memory", "__E_memory_170", "bf_var_selection", "wff_var_selection", "enum_option", 
	"severity_opt", "__E_option_171", "status_opt", "__E_bool_option_172", "colors_opt", "__E_bool_option_173", "debug_repl_opt", "__E_bool_option_174", "option_value_true", "option_value_false", 
	"severity", "error_sym", "__E_severity_175", "info_sym", "__E_severity_176", "debug_sym", "__E_severity_177", "trace_sym", "__E_severity_178", "bf_all", 
	"bf_ex", "del_sym", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", 
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
			87, 88, 89, 91, 93, 95, 97, 99, 101, 103,
			106, 109, 111, 114, 116, 119, 123, 125, 127, 129,
			131, 133, 135, 136, 137, 139, 141, 143, 145, 147,
			149, 151, 154, 155, 157, 161, 163, 165, 168, 170,
			173, 174, 196, 199, 201, 203, 205, 207, 209, 211,
			213, 215, 217, 219, 221, 223, 225, 227, 229, 231,
			233, 250, 277, 282, 285, 288, 292, 298, 302, 305,
			308, 311, 316, 320, 323, 328, 331, 333, 342, 345,
			348, 351, 354, 357, 360, 363, 367, 376, 384, 387,
			390, 393, 395, 400, 404, 405, 406, 407, 420, 422,
			424, 426, 428, 429, 431, 433, 435, 437, 439, 440,
			441
		},
		.trim_terminals = false,
		.to_inline = {
			{ 20, 80, 20 },
			{ 32, 95, 32 },
			{ 41, 155, 41 },
			{ 59 },
			{ 63 },
			{ 67 },
			{ 113 },
			{ 118 },
			{ 181 },
			{ 183 },
			{ 184 },
			{ 185 },
			{ 186 },
			{ 188 },
			{ 189 },
			{ 190 },
			{ 191 },
			{ 194 },
			{ 195 },
			{ 257 },
			{ 264 },
			{ 313 },
			{ 320 },
			{ 330 },
			{ 338 },
			{ 339 },
			{ 384 }
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
//G100: __E_wff_31(98)       => wff(32) _(16) '|' '|' '|' _(16) wff(32).
	p(NT(98), (NT(32)+NT(16)+T(10)+T(10)+T(10)+NT(16)+NT(32)));
//G101: global_or(97)        => __E_wff_31(98).
	p(NT(97), (NT(98)));
//G102: wff(32)              => global_or(97).
	p(NT(32), (NT(97)));
//G103: __E_wff_32(100)      => wff(32) _(16) '&' '&' '&' _(16) wff(32).
	p(NT(100), (NT(32)+NT(16)+T(11)+T(11)+T(11)+NT(16)+NT(32)));
//G104: global_and(99)       => __E_wff_32(100).
	p(NT(99), (NT(100)));
//G105: wff(32)              => global_and(99).
	p(NT(32), (NT(99)));
//G106: __E_wff_33(102)      => '-' _(16) wff(32).
	p(NT(102), (T(12)+NT(16)+NT(32)));
//G107: global_neg(101)      => __E_wff_33(102).
	p(NT(101), (NT(102)));
//G108: wff(32)              => global_neg(101).
	p(NT(32), (NT(101)));
//G109: __E___E_wff_34_35(105) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(105), (T(13)+T(14)+T(15)+T(16)+T(17)+T(18)+T(15)+T(16)+T(13)));
//G110: __E___E_wff_34_35(105) => '<' '>'.
	p(NT(105), (T(5)+T(19)));
//G111: __E_wff_34(104)      => __E___E_wff_34_35(105) _(16) wff(32).
	p(NT(104), (NT(105)+NT(16)+NT(32)));
//G112: wff_sometimes(103)   => __E_wff_34(104).
	p(NT(103), (NT(104)));
//G113: wff(32)              => wff_sometimes(103).
	p(NT(32), (NT(103)));
//G114: __E___E_wff_36_37(108) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(108), (T(20)+T(21)+T(22)+T(20)+T(23)+T(13)));
//G115: __E___E_wff_36_37(108) => '[' ']'.
	p(NT(108), (T(24)+T(25)));
//G116: __E_wff_36(107)      => __E___E_wff_36_37(108) _(16) wff(32).
	p(NT(107), (NT(108)+NT(16)+NT(32)));
//G117: wff_always(106)      => __E_wff_36(107).
	p(NT(106), (NT(107)));
//G118: wff(32)              => wff_always(106).
	p(NT(32), (NT(106)));
//G119: __E_wff_38(110)      => wff(32) _(16) '?' _(16) wff(32) _(16) ':' _(16) wff(32).
	p(NT(110), (NT(32)+NT(16)+T(26)+NT(16)+NT(32)+NT(16)+T(3)+NT(16)+NT(32)));
//G120: wff_conditional(109) => __E_wff_38(110).
	p(NT(109), (NT(110)));
//G121: wff(32)              => wff_conditional(109).
	p(NT(32), (NT(109)));
//G122: __E_wff_39(112)      => 'b' '_' 'a' 'l' 'l' __(72) q_bool_var(113) __(72) wff(32).
	p(NT(112), (T(27)+T(28)+T(20)+T(21)+T(21)+NT(72)+NT(113)+NT(72)+NT(32)));
//G123: wff_ball(111)        => __E_wff_39(112).
	p(NT(111), (NT(112)));
//G124: wff(32)              => wff_ball(111).
	p(NT(32), (NT(111)));
//G125: __E_wff_40(115)      => 'b' '_' 'e' 'x' __(72) q_bool_var(113) __(72) wff(32).
	p(NT(115), (T(27)+T(28)+T(16)+T(29)+NT(72)+NT(113)+NT(72)+NT(32)));
//G126: wff_bex(114)         => __E_wff_40(115).
	p(NT(114), (NT(115)));
//G127: wff(32)              => wff_bex(114).
	p(NT(32), (NT(114)));
//G128: __E_wff_41(117)      => 'a' 'l' 'l' __(72) q_var(118) __(72) wff(32).
	p(NT(117), (T(20)+T(21)+T(21)+NT(72)+NT(118)+NT(72)+NT(32)));
//G129: wff_all(116)         => __E_wff_41(117).
	p(NT(116), (NT(117)));
//G130: wff(32)              => wff_all(116).
	p(NT(32), (NT(116)));
//G131: __E_wff_42(120)      => 'e' 'x' __(72) q_var(118) __(72) wff(32).
	p(NT(120), (T(16)+T(29)+NT(72)+NT(118)+NT(72)+NT(32)));
//G132: wff_ex(119)          => __E_wff_42(120).
	p(NT(119), (NT(120)));
//G133: wff(32)              => wff_ex(119).
	p(NT(32), (NT(119)));
//G134: __E___E_wff_43_44(122) => _(16) offsets(87).
	p(NT(122), (NT(16)+NT(87)));
//G135: __E___E_wff_43_44(122) => null.
	p(NT(122), (nul));
//G136: __E_wff_43(121)      => sym(85) __E___E_wff_43_44(122) _(16) ref_args(88).
	p(NT(121), (NT(85)+NT(122)+NT(16)+NT(88)));
//G137: wff_ref(39)          => __E_wff_43(121).
	p(NT(39), (NT(121)));
//G138: wff(32)              => wff_ref(39).
	p(NT(32), (NT(39)));
//G139: __E_wff_45(124)      => wff(32) _(16) '-' '>' _(16) wff(32).
	p(NT(124), (NT(32)+NT(16)+T(12)+T(19)+NT(16)+NT(32)));
//G140: wff_imply(123)       => __E_wff_45(124).
	p(NT(123), (NT(124)));
//G141: wff(32)              => wff_imply(123).
	p(NT(32), (NT(123)));
//G142: __E_wff_46(126)      => wff(32) _(16) '<' '-' '>' _(16) wff(32).
	p(NT(126), (NT(32)+NT(16)+T(5)+T(12)+T(19)+NT(16)+NT(32)));
//G143: wff_equiv(125)       => __E_wff_46(126).
	p(NT(125), (NT(126)));
//G144: wff(32)              => wff_equiv(125).
	p(NT(32), (NT(125)));
//G145: __E_wff_47(128)      => wff(32) _(16) '|' '|' _(16) wff(32).
	p(NT(128), (NT(32)+NT(16)+T(10)+T(10)+NT(16)+NT(32)));
//G146: wff_or(127)          => __E_wff_47(128).
	p(NT(127), (NT(128)));
//G147: wff(32)              => wff_or(127).
	p(NT(32), (NT(127)));
//G148: __E_wff_48(130)      => wff(32) _(16) '^' _(16) wff(32).
	p(NT(130), (NT(32)+NT(16)+T(30)+NT(16)+NT(32)));
//G149: wff_xor(129)         => __E_wff_48(130).
	p(NT(129), (NT(130)));
//G150: wff(32)              => wff_xor(129).
	p(NT(32), (NT(129)));
//G151: __E_wff_49(132)      => wff(32) _(16) '&' '&' _(16) wff(32).
	p(NT(132), (NT(32)+NT(16)+T(11)+T(11)+NT(16)+NT(32)));
//G152: wff_and(131)         => __E_wff_49(132).
	p(NT(131), (NT(132)));
//G153: wff(32)              => wff_and(131).
	p(NT(32), (NT(131)));
//G154: __E_wff_50(134)      => '!' _(16) wff(32).
	p(NT(134), (T(31)+NT(16)+NT(32)));
//G155: wff_neg(133)         => __E_wff_50(134).
	p(NT(133), (NT(134)));
//G156: wff(32)              => wff_neg(133).
	p(NT(32), (NT(133)));
//G157: wff_t(135)           => 'T'.
	p(NT(135), (T(32)));
//G158: wff(32)              => wff_t(135).
	p(NT(32), (NT(135)));
//G159: wff_f(136)           => 'F'.
	p(NT(136), (T(33)));
//G160: wff(32)              => wff_f(136).
	p(NT(32), (NT(136)));
//G161: __E_wff_51(138)      => bf(41) _(16) '<' '=' _(16) q_var(118) _(16) '<' '=' _(16) bf(41).
	p(NT(138), (NT(41)+NT(16)+T(5)+T(4)+NT(16)+NT(118)+NT(16)+T(5)+T(4)+NT(16)+NT(41)));
//G162: bf_interval(137)     => __E_wff_51(138).
	p(NT(137), (NT(138)));
//G163: wff(32)              => bf_interval(137).
	p(NT(32), (NT(137)));
//G164: __E_wff_52(140)      => bf(41) _(16) '!' '=' _(16) bf(41).
	p(NT(140), (NT(41)+NT(16)+T(31)+T(4)+NT(16)+NT(41)));
//G165: bf_neq(139)          => __E_wff_52(140).
	p(NT(139), (NT(140)));
//G166: wff(32)              => bf_neq(139).
	p(NT(32), (NT(139)));
//G167: __E_wff_53(142)      => bf(41) _(16) '=' _(16) bf(41).
	p(NT(142), (NT(41)+NT(16)+T(4)+NT(16)+NT(41)));
//G168: bf_eq(141)           => __E_wff_53(142).
	p(NT(141), (NT(142)));
//G169: wff(32)              => bf_eq(141).
	p(NT(32), (NT(141)));
//G170: __E_wff_54(144)      => bf(41) _(16) '!' '<' '=' _(16) q_var(118).
	p(NT(144), (NT(41)+NT(16)+T(31)+T(5)+T(4)+NT(16)+NT(118)));
//G171: bf_nleq_lower(143)   => __E_wff_54(144).
	p(NT(143), (NT(144)));
//G172: wff(32)              => bf_nleq_lower(143).
	p(NT(32), (NT(143)));
//G173: __E_wff_55(146)      => q_var(118) _(16) '!' '<' '=' _(16) bf(41).
	p(NT(146), (NT(118)+NT(16)+T(31)+T(5)+T(4)+NT(16)+NT(41)));
//G174: bf_nleq_upper(145)   => __E_wff_55(146).
	p(NT(145), (NT(146)));
//G175: wff(32)              => bf_nleq_upper(145).
	p(NT(32), (NT(145)));
//G176: __E_wff_56(148)      => bf(41) _(16) '>' _(16) bf(41).
	p(NT(148), (NT(41)+NT(16)+T(19)+NT(16)+NT(41)));
//G177: bf_greater(147)      => __E_wff_56(148).
	p(NT(147), (NT(148)));
//G178: wff(32)              => bf_greater(147).
	p(NT(32), (NT(147)));
//G179: __E_wff_57(150)      => bf(41) _(16) '<' '=' _(16) bf(41).
	p(NT(150), (NT(41)+NT(16)+T(5)+T(4)+NT(16)+NT(41)));
//G180: bf_less_equal(149)   => __E_wff_57(150).
	p(NT(149), (NT(150)));
//G181: wff(32)              => bf_less_equal(149).
	p(NT(32), (NT(149)));
//G182: __E_wff_58(152)      => bf(41) _(16) '<' _(16) bf(41).
	p(NT(152), (NT(41)+NT(16)+T(5)+NT(16)+NT(41)));
//G183: bf_less(151)         => __E_wff_58(152).
	p(NT(151), (NT(152)));
//G184: wff(32)              => bf_less(151).
	p(NT(32), (NT(151)));
//G185: wff(32)              => capture(70).
	p(NT(32), (NT(70)));
//G186: wff(32)              => bool_variable(153).
	p(NT(32), (NT(153)));
//G187: wff(32)              => uninterpreted_constant(154).
	p(NT(32), (NT(154)));
//G188: __E_bf_59(156)       => '(' _(16) bf(41) _(16) ')'.
	p(NT(156), (T(8)+NT(16)+NT(41)+NT(16)+T(9)));
//G189: bf_parenthesis(155)  => __E_bf_59(156).
	p(NT(155), (NT(156)));
//G190: bf(41)               => bf_parenthesis(155).
	p(NT(41), (NT(155)));
//G191: __E_bf_60(158)       => 'S' _(16) '(' _(16) bf(41) _(16) ')'.
	p(NT(158), (T(34)+NT(16)+T(8)+NT(16)+NT(41)+NT(16)+T(9)));
//G192: bf_splitter(157)     => __E_bf_60(158).
	p(NT(157), (NT(158)));
//G193: bf(41)               => bf_splitter(157).
	p(NT(41), (NT(157)));
//G194: __E___E_bf_61_62(160) => _(16) offsets(87).
	p(NT(160), (NT(16)+NT(87)));
//G195: __E___E_bf_61_62(160) => null.
	p(NT(160), (nul));
//G196: __E_bf_61(159)       => sym(85) __E___E_bf_61_62(160) _(16) ref_args(88).
	p(NT(159), (NT(85)+NT(160)+NT(16)+NT(88)));
//G197: bf_ref(40)           => __E_bf_61(159).
	p(NT(40), (NT(159)));
//G198: bf(41)               => bf_ref(40).
	p(NT(41), (NT(40)));
//G199: __E_bf_63(162)       => bf(41) _(16) '|' _(16) bf(41).
	p(NT(162), (NT(41)+NT(16)+T(10)+NT(16)+NT(41)));
//G200: bf_or(161)           => __E_bf_63(162).
	p(NT(161), (NT(162)));
//G201: bf(41)               => bf_or(161).
	p(NT(41), (NT(161)));
//G202: __E_bf_64(164)       => bf(41) _(16) '+' _(16) bf(41).
	p(NT(164), (NT(41)+NT(16)+T(35)+NT(16)+NT(41)));
//G203: bf_xor(163)          => __E_bf_64(164).
	p(NT(163), (NT(164)));
//G204: bf(41)               => bf_xor(163).
	p(NT(41), (NT(163)));
//G205: __E___E_bf_65_66(167) => _(16).
	p(NT(167), (NT(16)));
//G206: __E___E_bf_65_66(167) => _(16) '&' _(16).
	p(NT(167), (NT(16)+T(11)+NT(16)));
//G207: __E_bf_65(166)       => bf(41) __E___E_bf_65_66(167) bf(41).
	p(NT(166), (NT(41)+NT(167)+NT(41)));
//G208: bf_and(165)          => __E_bf_65(166).
	p(NT(165), (NT(166)));
//G209: bf(41)               => bf_and(165).
	p(NT(41), (NT(165)));
//G210: __E_bf_67(169)       => bf(41) _(16) '\''.
	p(NT(169), (NT(41)+NT(16)+T(36)));
//G211: bf_neg(168)          => __E_bf_67(169).
	p(NT(168), (NT(169)));
//G212: bf(41)               => bf_neg(168).
	p(NT(41), (NT(168)));
//G213: __E_bf_68(171)       => '{' _(16) constant(172) _(16) '}'.
	p(NT(171), (T(6)+NT(16)+NT(172)+NT(16)+T(7)));
//G214: bf_constant(170)     => __E_bf_68(171).
	p(NT(170), (NT(171)));
//G215: bf(41)               => bf_constant(170).
	p(NT(41), (NT(170)));
//G216: bf_t(173)            => '1'.
	p(NT(173), (T(37)));
//G217: bf(41)               => bf_t(173).
	p(NT(41), (NT(173)));
//G218: bf_f(174)            => '0'.
	p(NT(174), (T(38)));
//G219: bf(41)               => bf_f(174).
	p(NT(41), (NT(174)));
//G220: bf(41)               => capture(70).
	p(NT(41), (NT(70)));
//G221: bf(41)               => variable(175).
	p(NT(41), (NT(175)));
//G222: bf(41)               => uninterpreted_constant(154).
	p(NT(41), (NT(154)));
//G223: ref_arg(178)         => bf(41).
	p(NT(178), (NT(41)));
//G224: __E___E_ref_args_69_70(177) => _(16) ref_arg(178).
	p(NT(177), (NT(16)+NT(178)));
//G225: __E___E_ref_args_69_71(179) => _(16) ',' _(16) ref_arg(178).
	p(NT(179), (NT(16)+T(39)+NT(16)+NT(178)));
//G226: __E___E_ref_args_69_72(180) => null.
	p(NT(180), (nul));
//G227: __E___E_ref_args_69_72(180) => __E___E_ref_args_69_71(179) __E___E_ref_args_69_72(180).
	p(NT(180), (NT(179)+NT(180)));
//G228: __E_ref_args_69(176) => __E___E_ref_args_69_70(177) __E___E_ref_args_69_72(180).
	p(NT(176), (NT(177)+NT(180)));
//G229: __E_ref_args_69(176) => null.
	p(NT(176), (nul));
//G230: ref_args(88)         => '(' __E_ref_args_69(176) _(16) ')'.
	p(NT(88), (T(8)+NT(176)+NT(16)+T(9)));
//G231: bf_cb_arg(182)       => bf(41).
	p(NT(182), (NT(41)));
//G232: bf_cb_args1(181)     => __(72) bf_cb_arg(182).
	p(NT(181), (NT(72)+NT(182)));
//G233: bf_cb_args2(183)     => bf_cb_args1(181) bf_cb_args1(181).
	p(NT(183), (NT(181)+NT(181)));
//G234: bf_cb_args3(184)     => bf_cb_args2(183) bf_cb_args1(181).
	p(NT(184), (NT(183)+NT(181)));
//G235: bf_cb_args4(185)     => bf_cb_args3(184) bf_cb_args1(181).
	p(NT(185), (NT(184)+NT(181)));
//G236: wff_cb_arg(187)      => wff(32).
	p(NT(187), (NT(32)));
//G237: wff_cb_args1(186)    => __(72) wff_cb_arg(187).
	p(NT(186), (NT(72)+NT(187)));
//G238: wff_cb_args2(188)    => wff_cb_args1(186) wff_cb_args1(186).
	p(NT(188), (NT(186)+NT(186)));
//G239: wff_cb_args3(189)    => wff_cb_args2(188) wff_cb_args1(186).
	p(NT(189), (NT(188)+NT(186)));
//G240: wff_cb_args4(190)    => wff_cb_args3(189) wff_cb_args1(186).
	p(NT(190), (NT(189)+NT(186)));
//G241: __E_tau_cb_args1_73(193) => capture(70).
	p(NT(193), (NT(70)));
//G242: __E_tau_cb_args1_73(193) => tau(20).
	p(NT(193), (NT(20)));
//G243: tau_cb_arg(192)      => __E_tau_cb_args1_73(193).
	p(NT(192), (NT(193)));
//G244: tau_cb_args1(191)    => __(72) tau_cb_arg(192).
	p(NT(191), (NT(72)+NT(192)));
//G245: tau_cb_args2(194)    => tau_cb_args1(191) tau_cb_args1(191).
	p(NT(194), (NT(191)+NT(191)));
//G246: tau_cb_args3(195)    => tau_cb_args2(194) tau_cb_args1(191).
	p(NT(195), (NT(194)+NT(191)));
//G247: __E___E_tau_cb_74_75(198) => tau_cb_args3(195).
	p(NT(198), (NT(195)));
//G248: __E___E_tau_cb_74_75(198) => tau_cb_args2(194).
	p(NT(198), (NT(194)));
//G249: __E_tau_cb_74(197)   => 't' 'a' 'u' '_' 'c' 'o' 'l' 'l' 'a' 'p' 's' 'e' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'c' 'b' __E___E_tau_cb_74_75(198).
	p(NT(197), (T(17)+T(20)+T(40)+T(28)+T(41)+T(14)+T(21)+T(21)+T(20)+T(42)+T(13)+T(16)+T(28)+T(42)+T(14)+T(13)+T(18)+T(17)+T(18)+T(43)+T(16)+T(13)+T(28)+T(41)+T(27)+NT(198)));
//G250: tau_collapse_positives_cb(196) => __E_tau_cb_74(197).
	p(NT(196), (NT(197)));
//G251: tau_cb(59)           => tau_collapse_positives_cb(196).
	p(NT(59), (NT(196)));
//G252: __E_tau_cb_76(200)   => 't' 'a' 'u' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'u' 'p' 'w' 'a' 'r' 'd' 's' '_' 'c' 'b' tau_cb_args2(194).
	p(NT(200), (T(17)+T(20)+T(40)+T(28)+T(42)+T(14)+T(13)+T(18)+T(17)+T(18)+T(43)+T(16)+T(13)+T(28)+T(40)+T(42)+T(22)+T(20)+T(44)+T(45)+T(13)+T(28)+T(41)+T(27)+NT(194)));
//G253: tau_positives_upwards_cb(199) => __E_tau_cb_76(200).
	p(NT(199), (NT(200)));
//G254: tau_cb(59)           => tau_positives_upwards_cb(199).
	p(NT(59), (NT(199)));
//G255: __E_wff_cb_77(202)   => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b' bf_cb_args1(181) wff_cb_args2(188).
	p(NT(202), (T(27)+T(46)+T(28)+T(16)+T(47)+T(28)+T(41)+T(27)+NT(181)+NT(188)));
//G256: bf_eq_cb(201)        => __E_wff_cb_77(202).
	p(NT(201), (NT(202)));
//G257: wff_cb(63)           => bf_eq_cb(201).
	p(NT(63), (NT(201)));
//G258: __E_wff_cb_78(204)   => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b' bf_cb_args1(181) wff_cb_args2(188).
	p(NT(204), (T(27)+T(46)+T(28)+T(48)+T(16)+T(47)+T(28)+T(41)+T(27)+NT(181)+NT(188)));
//G259: bf_neq_cb(203)       => __E_wff_cb_78(204).
	p(NT(203), (NT(204)));
//G260: wff_cb(63)           => bf_neq_cb(203).
	p(NT(63), (NT(203)));
//G261: __E_wff_cb_79(206)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(188).
	p(NT(206), (T(22)+T(46)+T(46)+T(28)+T(49)+T(20)+T(13)+T(28)+T(41)+T(21)+T(20)+T(13)+T(49)+T(18)+T(48)+T(50)+T(28)+T(13)+T(40)+T(27)+T(46)+T(14)+T(44)+T(15)+T(40)+T(21)+T(20)+T(13)+T(28)+T(41)+T(27)+NT(188)));
//G262: wff_has_clashing_subformulas_cb(205) => __E_wff_cb_79(206).
	p(NT(205), (NT(206)));
//G263: wff_cb(63)           => wff_has_clashing_subformulas_cb(205).
	p(NT(63), (NT(205)));
//G264: __E_wff_cb_80(208)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(189).
	p(NT(208), (T(22)+T(46)+T(46)+T(28)+T(49)+T(20)+T(13)+T(28)+T(13)+T(40)+T(27)+T(46)+T(14)+T(44)+T(15)+T(40)+T(21)+T(20)+T(28)+T(41)+T(27)+NT(189)));
//G265: wff_has_subformula_cb(207) => __E_wff_cb_80(208).
	p(NT(207), (NT(208)));
//G266: wff_cb(63)           => wff_has_subformula_cb(207).
	p(NT(63), (NT(207)));
//G267: __E_wff_cb_81(210)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(188).
	p(NT(210), (T(22)+T(46)+T(46)+T(28)+T(44)+T(16)+T(15)+T(14)+T(43)+T(16)+T(28)+T(16)+T(29)+T(18)+T(13)+T(17)+T(16)+T(48)+T(17)+T(18)+T(20)+T(21)+T(28)+T(41)+T(27)+NT(188)));
//G268: wff_remove_existential_cb(209) => __E_wff_cb_81(210).
	p(NT(209), (NT(210)));
//G269: wff_cb(63)           => wff_remove_existential_cb(209).
	p(NT(63), (NT(209)));
//G270: __E_wff_cb_82(212)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args4(190).
	p(NT(212), (T(22)+T(46)+T(46)+T(28)+T(44)+T(16)+T(15)+T(14)+T(43)+T(16)+T(28)+T(27)+T(16)+T(29)+T(18)+T(13)+T(17)+T(16)+T(48)+T(17)+T(18)+T(20)+T(21)+T(28)+T(41)+T(27)+NT(190)));
//G271: wff_remove_bexistential_cb(211) => __E_wff_cb_82(212).
	p(NT(211), (NT(212)));
//G272: wff_cb(63)           => wff_remove_bexistential_cb(211).
	p(NT(63), (NT(211)));
//G273: __E_wff_cb_83(214)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' wff_cb_args4(190).
	p(NT(214), (T(22)+T(46)+T(46)+T(28)+T(44)+T(16)+T(15)+T(14)+T(43)+T(16)+T(28)+T(27)+T(40)+T(48)+T(18)+T(43)+T(16)+T(44)+T(13)+T(20)+T(21)+T(28)+T(41)+T(27)+NT(190)));
//G274: wff_remove_buniversal_cb(213) => __E_wff_cb_83(214).
	p(NT(213), (NT(214)));
//G275: wff_cb(63)           => wff_remove_buniversal_cb(213).
	p(NT(63), (NT(213)));
//G276: __E_bf_cb_84(216)    => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b' bf_cb_args2(183).
	p(NT(216), (T(27)+T(46)+T(28)+T(18)+T(13)+T(28)+T(51)+T(16)+T(44)+T(14)+T(28)+T(41)+T(27)+NT(183)));
//G277: bf_is_zero_cb(215)   => __E_bf_cb_84(216).
	p(NT(215), (NT(216)));
//G278: bf_cb(67)            => bf_is_zero_cb(215).
	p(NT(67), (NT(215)));
//G279: __E_bf_cb_85(218)    => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b' bf_cb_args2(183).
	p(NT(218), (T(27)+T(46)+T(28)+T(18)+T(13)+T(28)+T(14)+T(48)+T(16)+T(28)+T(41)+T(27)+NT(183)));
//G280: bf_is_one_cb(217)    => __E_bf_cb_85(218).
	p(NT(217), (NT(218)));
//G281: bf_cb(67)            => bf_is_one_cb(217).
	p(NT(67), (NT(217)));
//G282: __E_bf_cb_86(220)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(184).
	p(NT(220), (T(27)+T(46)+T(28)+T(49)+T(20)+T(13)+T(28)+T(13)+T(40)+T(27)+T(46)+T(14)+T(44)+T(15)+T(40)+T(21)+T(20)+T(28)+T(41)+T(27)+NT(184)));
//G283: bf_has_subformula_cb(219) => __E_bf_cb_86(220).
	p(NT(219), (NT(220)));
//G284: bf_cb(67)            => bf_has_subformula_cb(219).
	p(NT(67), (NT(219)));
//G285: __E_bf_cb_87(222)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(185).
	p(NT(222), (T(27)+T(46)+T(28)+T(44)+T(16)+T(15)+T(14)+T(43)+T(16)+T(28)+T(46)+T(40)+T(48)+T(18)+T(43)+T(16)+T(44)+T(13)+T(20)+T(21)+T(28)+T(41)+T(27)+NT(185)));
//G286: bf_remove_funiversal_cb(221) => __E_bf_cb_87(222).
	p(NT(221), (NT(222)));
//G287: bf_cb(67)            => bf_remove_funiversal_cb(221).
	p(NT(67), (NT(221)));
//G288: __E_bf_cb_88(224)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(185).
	p(NT(224), (T(27)+T(46)+T(28)+T(44)+T(16)+T(15)+T(14)+T(43)+T(16)+T(28)+T(46)+T(16)+T(29)+T(18)+T(13)+T(17)+T(16)+T(48)+T(17)+T(18)+T(20)+T(21)+T(28)+T(41)+T(27)+NT(185)));
//G289: bf_remove_fexistential_cb(223) => __E_bf_cb_88(224).
	p(NT(223), (NT(224)));
//G290: bf_cb(67)            => bf_remove_fexistential_cb(223).
	p(NT(67), (NT(223)));
//G291: __E_bf_cb_89(226)    => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b' bf_cb_args2(183).
	p(NT(226), (T(27)+T(46)+T(28)+T(20)+T(48)+T(45)+T(28)+T(41)+T(27)+NT(183)));
//G292: bf_and_cb(225)       => __E_bf_cb_89(226).
	p(NT(225), (NT(226)));
//G293: bf_cb(67)            => bf_and_cb(225).
	p(NT(67), (NT(225)));
//G294: __E_bf_cb_90(228)    => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b' bf_cb_args2(183).
	p(NT(228), (T(27)+T(46)+T(28)+T(14)+T(44)+T(28)+T(41)+T(27)+NT(183)));
//G295: bf_or_cb(227)        => __E_bf_cb_90(228).
	p(NT(227), (NT(228)));
//G296: bf_cb(67)            => bf_or_cb(227).
	p(NT(67), (NT(227)));
//G297: __E_bf_cb_91(230)    => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b' bf_cb_args2(183).
	p(NT(230), (T(27)+T(46)+T(28)+T(29)+T(14)+T(44)+T(28)+T(41)+T(27)+NT(183)));
//G298: bf_xor_cb(229)       => __E_bf_cb_91(230).
	p(NT(229), (NT(230)));
//G299: bf_cb(67)            => bf_xor_cb(229).
	p(NT(67), (NT(229)));
//G300: __E_bf_cb_92(232)    => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b' bf_cb_args1(181).
	p(NT(232), (T(27)+T(46)+T(28)+T(48)+T(16)+T(50)+T(28)+T(41)+T(27)+NT(181)));
//G301: bf_neg_cb(231)       => __E_bf_cb_92(232).
	p(NT(231), (NT(232)));
//G302: bf_cb(67)            => bf_neg_cb(231).
	p(NT(67), (NT(231)));
//G303: __E_bf_cb_93(234)    => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(181).
	p(NT(234), (T(27)+T(46)+T(28)+T(48)+T(14)+T(44)+T(15)+T(20)+T(21)+T(18)+T(51)+T(16)+T(28)+T(41)+T(27)+NT(181)));
//G304: bf_normalize_cb(233) => __E_bf_cb_93(234).
	p(NT(233), (NT(234)));
//G305: bf_cb(67)            => bf_normalize_cb(233).
	p(NT(67), (NT(233)));
//G306: __N_0(442)           => 'F'.
	p(NT(442), (T(33)));
//G307: __N_1(443)           => 'T'.
	p(NT(443), (T(32)));
//G308: __N_2(444)           => 'i'.
	p(NT(444), (T(18)));
//G309: __N_3(445)           => 'o'.
	p(NT(445), (T(14)));
//G310: __E_charvar_94(236)  => ~( __N_0(442) ) & ~( __N_1(443) ) & ~( __N_2(444) ) & ~( __N_3(445) ) & alpha(5).	 # conjunctive
	p(NT(236), ~(NT(442)) & ~(NT(443)) & ~(NT(444)) & ~(NT(445)) & (NT(5)));
//G311: __E_charvar_95(237)  => null.
	p(NT(237), (nul));
//G312: __E_charvar_95(237)  => digit(3) __E_charvar_95(237).
	p(NT(237), (NT(3)+NT(237)));
//G313: charvar(235)         => __E_charvar_94(236) __E_charvar_95(237).
	p(NT(235), (NT(236)+NT(237)));
//G314: bool_variable(153)   => '?' charvar(235).
	p(NT(153), (T(26)+NT(235)));
//G315: variable(175)        => charvar(235).
	p(NT(175), (NT(235)));
//G316: __E_variable_96(239) => in(45).
	p(NT(239), (NT(45)));
//G317: __E_variable_96(239) => out(240).
	p(NT(239), (NT(240)));
//G318: io_var(238)          => __E_variable_96(239).
	p(NT(238), (NT(239)));
//G319: variable(175)        => io_var(238).
	p(NT(175), (NT(238)));
//G320: __E_in_97(242)       => 'i' digits(243).
	p(NT(242), (T(18)+NT(243)));
//G321: in_var_name(241)     => __E_in_97(242).
	p(NT(241), (NT(242)));
//G322: in(45)               => in_var_name(241) '[' offset(244) ']'.
	p(NT(45), (NT(241)+T(24)+NT(244)+T(25)));
//G323: __E_out_98(246)      => 'o' digits(243).
	p(NT(246), (T(14)+NT(243)));
//G324: out_var_name(245)    => __E_out_98(246).
	p(NT(245), (NT(246)));
//G325: out(240)             => out_var_name(245) '[' offset(244) ']'.
	p(NT(240), (NT(245)+T(24)+NT(244)+T(25)));
//G326: capture(70)          => '$' charvar(235).
	p(NT(70), (T(52)+NT(235)));
//G327: q_bool_var(113)      => capture(70).
	p(NT(113), (NT(70)));
//G328: q_bool_var(113)      => bool_variable(153).
	p(NT(113), (NT(153)));
//G329: q_var(118)           => capture(70).
	p(NT(118), (NT(70)));
//G330: q_var(118)           => variable(175).
	p(NT(118), (NT(175)));
//G331: __E_offsets_99(247)  => _(16) ',' _(16) offset(244).
	p(NT(247), (NT(16)+T(39)+NT(16)+NT(244)));
//G332: __E_offsets_100(248) => null.
	p(NT(248), (nul));
//G333: __E_offsets_100(248) => __E_offsets_99(247) __E_offsets_100(248).
	p(NT(248), (NT(247)+NT(248)));
//G334: offsets(87)          => '[' _(16) offset(244) __E_offsets_100(248) _(16) ']'.
	p(NT(87), (T(24)+NT(16)+NT(244)+NT(248)+NT(16)+T(25)));
//G335: offset(244)          => num(249).
	p(NT(244), (NT(249)));
//G336: offset(244)          => capture(70).
	p(NT(244), (NT(70)));
//G337: offset(244)          => shift(250).
	p(NT(244), (NT(250)));
//G338: __N_4(446)           => io_var(238).
	p(NT(446), (NT(238)));
//G339: __E_offset_101(251)  => variable(175) & ~( __N_4(446) ).	 # conjunctive
	p(NT(251), (NT(175)) & ~(NT(446)));
//G340: offset(244)          => __E_offset_101(251).
	p(NT(244), (NT(251)));
//G341: __E_shift_102(252)   => capture(70).
	p(NT(252), (NT(70)));
//G342: __N_5(447)           => io_var(238).
	p(NT(447), (NT(238)));
//G343: __E___E_shift_102_103(253) => variable(175) & ~( __N_5(447) ).	 # conjunctive
	p(NT(253), (NT(175)) & ~(NT(447)));
//G344: __E_shift_102(252)   => __E___E_shift_102_103(253).
	p(NT(252), (NT(253)));
//G345: shift(250)           => __E_shift_102(252) _(16) '-' _(16) num(249).
	p(NT(250), (NT(252)+NT(16)+T(12)+NT(16)+NT(249)));
//G346: num(249)             => digits(243).
	p(NT(249), (NT(243)));
//G347: __E___E_uninterpreted_constant_104_105(256) => chars(257).
	p(NT(256), (NT(257)));
//G348: __E___E_uninterpreted_constant_104_105(256) => _(16).
	p(NT(256), (NT(16)));
//G349: __E_uninterpreted_constant_104(255) => __E___E_uninterpreted_constant_104_105(256) ':' chars(257).
	p(NT(255), (NT(256)+T(3)+NT(257)));
//G350: uninter_const_name(254) => __E_uninterpreted_constant_104(255).
	p(NT(254), (NT(255)));
//G351: uninterpreted_constant(154) => '<' uninter_const_name(254) _(16) '>'.
	p(NT(154), (T(5)+NT(254)+NT(16)+T(19)));
//G352: constant(172)        => binding(258).
	p(NT(172), (NT(258)));
//G353: constant(172)        => capture(70).
	p(NT(172), (NT(70)));
//G354: __E___E_binding_106_107(261) => chars(257) _(16).
	p(NT(261), (NT(257)+NT(16)));
//G355: __E___E_binding_106_107(261) => _(16).
	p(NT(261), (NT(16)));
//G356: type(260)            => __E___E_binding_106_107(261).
	p(NT(260), (NT(261)));
//G357: __E_binding_106(259) => type(260) ':' _(16) source(262).
	p(NT(259), (NT(260)+T(3)+NT(16)+NT(262)));
//G358: source_binding(46)   => __E_binding_106(259).
	p(NT(46), (NT(259)));
//G359: binding(258)         => source_binding(46).
	p(NT(258), (NT(46)));
//G360: named_binding(263)   => chars(257).
	p(NT(263), (NT(257)));
//G361: binding(258)         => named_binding(263).
	p(NT(258), (NT(263)));
//G362: __E_source_108(265)  => alnum(6).
	p(NT(265), (NT(6)));
//G363: __E_source_108(265)  => punct(7).
	p(NT(265), (NT(7)));
//G364: src_c(264)           => __E_source_108(265).
	p(NT(264), (NT(265)));
//G365: __E___E_source_109_110(267) => src_c(264).
	p(NT(267), (NT(264)));
//G366: __E___E_source_109_110(267) => space(2).
	p(NT(267), (NT(2)));
//G367: __E___E_source_109_111(268) => null.
	p(NT(268), (nul));
//G368: __E___E_source_109_111(268) => __E___E_source_109_110(267) __E___E_source_109_111(268).
	p(NT(268), (NT(267)+NT(268)));
//G369: __E_source_109(266)  => __E___E_source_109_111(268) src_c(264).
	p(NT(266), (NT(268)+NT(264)));
//G370: __E_source_109(266)  => null.
	p(NT(266), (nul));
//G371: source(262)          => src_c(264) __E_source_109(266).
	p(NT(262), (NT(264)+NT(266)));
//G372: __E_chars_112(269)   => null.
	p(NT(269), (nul));
//G373: __E_chars_112(269)   => alnum(6) __E_chars_112(269).
	p(NT(269), (NT(6)+NT(269)));
//G374: chars(257)           => alpha(5) __E_chars_112(269).
	p(NT(257), (NT(5)+NT(269)));
//G375: __E_digits_113(270)  => digit(3).
	p(NT(270), (NT(3)));
//G376: __E_digits_113(270)  => digit(3) __E_digits_113(270).
	p(NT(270), (NT(3)+NT(270)));
//G377: digits(243)          => __E_digits_113(270).
	p(NT(243), (NT(270)));
//G378: sym(85)              => chars(257).
	p(NT(85), (NT(257)));
//G379: __E_comment_114(272) => printable(8).
	p(NT(272), (NT(8)));
//G380: __E_comment_114(272) => '\t'.
	p(NT(272), (T(53)));
//G381: __E_comment_115(273) => null.
	p(NT(273), (nul));
//G382: __E_comment_115(273) => __E_comment_114(272) __E_comment_115(273).
	p(NT(273), (NT(272)+NT(273)));
//G383: __E_comment_116(274) => '\n'.
	p(NT(274), (T(54)));
//G384: __E_comment_116(274) => '\r'.
	p(NT(274), (T(55)));
//G385: __E_comment_116(274) => eof(1).
	p(NT(274), (NT(1)));
//G386: comment(271)         => '#' __E_comment_115(273) __E_comment_116(274).
	p(NT(271), (T(56)+NT(273)+NT(274)));
//G387: __E____117(275)      => space(2).
	p(NT(275), (NT(2)));
//G388: __E____117(275)      => comment(271).
	p(NT(275), (NT(271)));
//G389: __(72)               => __E____117(275) _(16).
	p(NT(72), (NT(275)+NT(16)));
//G390: __E___118(276)       => __(72).
	p(NT(276), (NT(72)));
//G391: __E___118(276)       => null.
	p(NT(276), (nul));
//G392: _(16)                => __E___118(276).
	p(NT(16), (NT(276)));
//G393: __E_cli_119(279)     => '.' _(16) cli_command(278) _(16).
	p(NT(279), (T(2)+NT(16)+NT(278)+NT(16)));
//G394: __E_cli_120(280)     => null.
	p(NT(280), (nul));
//G395: __E_cli_120(280)     => __E_cli_119(279) __E_cli_120(280).
	p(NT(280), (NT(279)+NT(280)));
//G396: cli(277)             => _(16) cli_command(278) _(16) __E_cli_120(280).
	p(NT(277), (NT(16)+NT(278)+NT(16)+NT(280)));
//G397: __E_cli_command_121(283) => 'q'.
	p(NT(283), (T(47)));
//G398: __E_cli_command_121(283) => 'q' 'u' 'i' 't'.
	p(NT(283), (T(47)+T(40)+T(18)+T(17)));
//G399: quit_sym(282)        => __E_cli_command_121(283).
	p(NT(282), (NT(283)));
//G400: quit_cmd(281)        => quit_sym(282).
	p(NT(281), (NT(282)));
//G401: cli_command(278)     => quit_cmd(281).
	p(NT(278), (NT(281)));
//G402: __E_cli_command_122(286) => 'v'.
	p(NT(286), (T(43)));
//G403: __E_cli_command_122(286) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(286), (T(43)+T(16)+T(44)+T(13)+T(18)+T(14)+T(48)));
//G404: version_sym(285)     => __E_cli_command_122(286).
	p(NT(285), (NT(286)));
//G405: version_cmd(284)     => version_sym(285).
	p(NT(284), (NT(285)));
//G406: cli_command(278)     => version_cmd(284).
	p(NT(278), (NT(284)));
//G407: __E_cli_command_123(289) => 'c'.
	p(NT(289), (T(41)));
//G408: __E_cli_command_123(289) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(289), (T(41)+T(21)+T(16)+T(20)+T(44)));
//G409: clear_sym(288)       => __E_cli_command_123(289).
	p(NT(288), (NT(289)));
//G410: clear_cmd(287)       => clear_sym(288).
	p(NT(287), (NT(288)));
//G411: cli_command(278)     => clear_cmd(287).
	p(NT(278), (NT(287)));
//G412: __E___E_cli_command_124_125(293) => 'h'.
	p(NT(293), (T(49)));
//G413: __E___E_cli_command_124_125(293) => 'h' 'e' 'l' 'p'.
	p(NT(293), (T(49)+T(16)+T(21)+T(42)));
//G414: help_sym(292)        => __E___E_cli_command_124_125(293).
	p(NT(292), (NT(293)));
//G415: __E___E_cli_command_124_126(294) => __(72) help_arg(295).
	p(NT(294), (NT(72)+NT(295)));
//G416: __E___E_cli_command_124_126(294) => null.
	p(NT(294), (nul));
//G417: __E_cli_command_124(291) => help_sym(292) __E___E_cli_command_124_126(294).
	p(NT(291), (NT(292)+NT(294)));
//G418: help_cmd(290)        => __E_cli_command_124(291).
	p(NT(290), (NT(291)));
//G419: cli_command(278)     => help_cmd(290).
	p(NT(278), (NT(290)));
//G420: file_sym(298)        => 'f' 'i' 'l' 'e'.
	p(NT(298), (T(46)+T(18)+T(21)+T(16)));
//G421: __E_cli_command_127(297) => file_sym(298) __(72) q_string(299).
	p(NT(297), (NT(298)+NT(72)+NT(299)));
//G422: file_cmd(296)        => __E_cli_command_127(297).
	p(NT(296), (NT(297)));
//G423: cli_command(278)     => file_cmd(296).
	p(NT(278), (NT(296)));
//G424: valid_sym(302)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(302), (T(43)+T(20)+T(21)+T(18)+T(45)));
//G425: __E_cli_command_128(301) => valid_sym(302) __(72) nso_rr(14).
	p(NT(301), (NT(302)+NT(72)+NT(14)));
//G426: valid_cmd(300)       => __E_cli_command_128(301).
	p(NT(300), (NT(301)));
//G427: cli_command(278)     => valid_cmd(300).
	p(NT(278), (NT(300)));
//G428: sat_sym(305)         => 's' 'a' 't'.
	p(NT(305), (T(13)+T(20)+T(17)));
//G429: __E_cli_command_129(304) => sat_sym(305) __(72) nso_rr(14).
	p(NT(304), (NT(305)+NT(72)+NT(14)));
//G430: sat_cmd(303)         => __E_cli_command_129(304).
	p(NT(303), (NT(304)));
//G431: cli_command(278)     => sat_cmd(303).
	p(NT(278), (NT(303)));
//G432: unsat_sym(308)       => 'u' 'n' 's' 'a' 't'.
	p(NT(308), (T(40)+T(48)+T(13)+T(20)+T(17)));
//G433: __E_cli_command_130(307) => unsat_sym(308) __(72) nso_rr(14).
	p(NT(307), (NT(308)+NT(72)+NT(14)));
//G434: unsat_cmd(306)       => __E_cli_command_130(307).
	p(NT(306), (NT(307)));
//G435: cli_command(278)     => unsat_cmd(306).
	p(NT(278), (NT(306)));
//G436: solve_sym(311)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(311), (T(13)+T(14)+T(21)+T(43)+T(16)));
//G437: solve_cmd_arg(312)   => wff_cmd_arg(313).
	p(NT(312), (NT(313)));
//G438: __E_cli_command_131(310) => solve_sym(311) __(72) solve_cmd_arg(312).
	p(NT(310), (NT(311)+NT(72)+NT(312)));
//G439: solve_cmd(309)       => __E_cli_command_131(310).
	p(NT(309), (NT(310)));
//G440: cli_command(278)     => solve_cmd(309).
	p(NT(278), (NT(309)));
//G441: __E___E_cli_command_132_133(317) => 'e'.
	p(NT(317), (T(16)));
//G442: __E___E_cli_command_132_133(317) => 'e' 'x' 'e' 'c' 'u' 't' 'e'.
	p(NT(317), (T(16)+T(29)+T(16)+T(41)+T(40)+T(17)+T(16)));
//G443: execute_sym(316)     => __E___E_cli_command_132_133(317).
	p(NT(316), (NT(317)));
//G444: __E___E_cli_command_132_134(319) => wff_typed(320).
	p(NT(319), (NT(320)));
//G445: __E___E_cli_command_132_134(319) => nso_rr(14).
	p(NT(319), (NT(14)));
//G446: execute_cmd_arg(318) => __E___E_cli_command_132_134(319).
	p(NT(318), (NT(319)));
//G447: __E_cli_command_132(315) => execute_sym(316) __(72) execute_cmd_arg(318).
	p(NT(315), (NT(316)+NT(72)+NT(318)));
//G448: execute_cmd(314)     => __E_cli_command_132(315).
	p(NT(314), (NT(315)));
//G449: cli_command(278)     => execute_cmd(314).
	p(NT(278), (NT(314)));
//G450: __E___E_cli_command_135_136(324) => 'n'.
	p(NT(324), (T(48)));
//G451: __E___E_cli_command_135_136(324) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(324), (T(48)+T(14)+T(44)+T(15)+T(20)+T(21)+T(18)+T(51)+T(16)));
//G452: normalize_sym(323)   => __E___E_cli_command_135_136(324).
	p(NT(323), (NT(324)));
//G453: __E_cli_command_135(322) => normalize_sym(323) __(72) normalize_cmd_arg(325).
	p(NT(322), (NT(323)+NT(72)+NT(325)));
//G454: normalize_cmd(321)   => __E_cli_command_135(322).
	p(NT(321), (NT(322)));
//G455: cli_command(278)     => normalize_cmd(321).
	p(NT(278), (NT(321)));
//G456: __E___E_cli_command_137_138(329) => 'i'.
	p(NT(329), (T(18)));
//G457: __E___E_cli_command_137_138(329) => 'i' 'n' 's' 't'.
	p(NT(329), (T(18)+T(48)+T(13)+T(17)));
//G458: __E___E_cli_command_137_138(329) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(329), (T(18)+T(48)+T(13)+T(17)+T(20)+T(48)+T(17)+T(18)+T(20)+T(17)+T(16)));
//G459: inst_sym(328)        => __E___E_cli_command_137_138(329).
	p(NT(328), (NT(329)));
//G460: __E_cli_command_137(327) => inst_sym(328) __(72) variable(175) __(72) wff_cmd_arg(313) __(72) wff_cmd_out_arg(330).
	p(NT(327), (NT(328)+NT(72)+NT(175)+NT(72)+NT(313)+NT(72)+NT(330)));
//G461: wff_inst_cmd(326)    => __E_cli_command_137(327).
	p(NT(326), (NT(327)));
//G462: cli_command(278)     => wff_inst_cmd(326).
	p(NT(278), (NT(326)));
//G463: __E___E_cli_command_139_140(334) => 's'.
	p(NT(334), (T(13)));
//G464: __E___E_cli_command_139_140(334) => 's' 'u' 'b' 's' 't'.
	p(NT(334), (T(13)+T(40)+T(27)+T(13)+T(17)));
//G465: __E___E_cli_command_139_140(334) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(334), (T(13)+T(40)+T(27)+T(13)+T(17)+T(18)+T(17)+T(40)+T(17)+T(16)));
//G466: subst_sym(333)       => __E___E_cli_command_139_140(334).
	p(NT(333), (NT(334)));
//G467: __E_cli_command_139(332) => subst_sym(333) __(72) nf_cmd_arg(335) _(16) '[' _(16) nf_cmd_arg(335) _(16) '/' _(16) nf_cmd_arg(335) _(16) ']'.
	p(NT(332), (NT(333)+NT(72)+NT(335)+NT(16)+T(24)+NT(16)+NT(335)+NT(16)+T(57)+NT(16)+NT(335)+NT(16)+T(25)));
//G468: subst_cmd(331)       => __E_cli_command_139(332).
	p(NT(331), (NT(332)));
//G469: cli_command(278)     => subst_cmd(331).
	p(NT(278), (NT(331)));
//G470: __E_cli_command_141(337) => inst_sym(328) __(72) variable(175) __(72) bf_cmd_arg(338) __(72) bf_cmd_out_arg(339).
	p(NT(337), (NT(328)+NT(72)+NT(175)+NT(72)+NT(338)+NT(72)+NT(339)));
//G471: bf_inst_cmd(336)     => __E_cli_command_141(337).
	p(NT(336), (NT(337)));
//G472: cli_command(278)     => bf_inst_cmd(336).
	p(NT(278), (NT(336)));
//G473: dnf_sym(342)         => 'd' 'n' 'f'.
	p(NT(342), (T(45)+T(48)+T(46)));
//G474: __E_cli_command_142(341) => dnf_sym(342) __(72) nf_cmd_arg(335).
	p(NT(341), (NT(342)+NT(72)+NT(335)));
//G475: dnf_cmd(340)         => __E_cli_command_142(341).
	p(NT(340), (NT(341)));
//G476: cli_command(278)     => dnf_cmd(340).
	p(NT(278), (NT(340)));
//G477: cnf_sym(345)         => 'c' 'n' 'f'.
	p(NT(345), (T(41)+T(48)+T(46)));
//G478: __E_cli_command_143(344) => cnf_sym(345) __(72) nf_cmd_arg(335).
	p(NT(344), (NT(345)+NT(72)+NT(335)));
//G479: cnf_cmd(343)         => __E_cli_command_143(344).
	p(NT(343), (NT(344)));
//G480: cli_command(278)     => cnf_cmd(343).
	p(NT(278), (NT(343)));
//G481: anf_sym(348)         => 'a' 'n' 'f'.
	p(NT(348), (T(20)+T(48)+T(46)));
//G482: __E_cli_command_144(347) => anf_sym(348) __(72) nf_cmd_arg(335).
	p(NT(347), (NT(348)+NT(72)+NT(335)));
//G483: anf_cmd(346)         => __E_cli_command_144(347).
	p(NT(346), (NT(347)));
//G484: cli_command(278)     => anf_cmd(346).
	p(NT(278), (NT(346)));
//G485: nnf_sym(351)         => 'n' 'n' 'f'.
	p(NT(351), (T(48)+T(48)+T(46)));
//G486: __E_cli_command_145(350) => nnf_sym(351) __(72) nf_cmd_arg(335).
	p(NT(350), (NT(351)+NT(72)+NT(335)));
//G487: nnf_cmd(349)         => __E_cli_command_145(350).
	p(NT(349), (NT(350)));
//G488: cli_command(278)     => nnf_cmd(349).
	p(NT(278), (NT(349)));
//G489: pnf_sym(354)         => 'p' 'n' 'f'.
	p(NT(354), (T(42)+T(48)+T(46)));
//G490: __E_cli_command_146(353) => pnf_sym(354) __(72) nf_cmd_arg(335).
	p(NT(353), (NT(354)+NT(72)+NT(335)));
//G491: pnf_cmd(352)         => __E_cli_command_146(353).
	p(NT(352), (NT(353)));
//G492: cli_command(278)     => pnf_cmd(352).
	p(NT(278), (NT(352)));
//G493: mnf_sym(357)         => 'm' 'n' 'f'.
	p(NT(357), (T(15)+T(48)+T(46)));
//G494: __E_cli_command_147(356) => mnf_sym(357) __(72) nf_cmd_arg(335).
	p(NT(356), (NT(357)+NT(72)+NT(335)));
//G495: mnf_cmd(355)         => __E_cli_command_147(356).
	p(NT(355), (NT(356)));
//G496: cli_command(278)     => mnf_cmd(355).
	p(NT(278), (NT(355)));
//G497: snf_sym(360)         => 's' 'n' 'f'.
	p(NT(360), (T(13)+T(48)+T(46)));
//G498: __E_cli_command_148(359) => snf_sym(360) __(72) nf_cmd_arg(335).
	p(NT(359), (NT(360)+NT(72)+NT(335)));
//G499: snf_cmd(358)         => __E_cli_command_148(359).
	p(NT(358), (NT(359)));
//G500: cli_command(278)     => snf_cmd(358).
	p(NT(278), (NT(358)));
//G501: onf_sym(363)         => 'o' 'n' 'f'.
	p(NT(363), (T(14)+T(48)+T(46)));
//G502: __E_cli_command_149(362) => onf_sym(363) __(72) variable(175) __(72) onf_cmd_arg(364).
	p(NT(362), (NT(363)+NT(72)+NT(175)+NT(72)+NT(364)));
//G503: onf_cmd(361)         => __E_cli_command_149(362).
	p(NT(361), (NT(362)));
//G504: cli_command(278)     => onf_cmd(361).
	p(NT(278), (NT(361)));
//G505: __E___E_cli_command_150_151(368) => 'd' 'e' 'f' 's'.
	p(NT(368), (T(45)+T(16)+T(46)+T(13)));
//G506: __E___E_cli_command_150_151(368) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(368), (T(45)+T(16)+T(46)+T(18)+T(48)+T(18)+T(17)+T(18)+T(14)+T(48)+T(13)));
//G507: def_sym(367)         => __E___E_cli_command_150_151(368).
	p(NT(367), (NT(368)));
//G508: __E_cli_command_150(366) => def_sym(367).
	p(NT(366), (NT(367)));
//G509: def_list_cmd(365)    => __E_cli_command_150(366).
	p(NT(365), (NT(366)));
//G510: cli_command(278)     => def_list_cmd(365).
	p(NT(278), (NT(365)));
//G511: __E_cli_command_152(370) => nso_rec_relation_form(36).
	p(NT(370), (NT(36)));
//G512: def_rr_cmd(369)      => __E_cli_command_152(370).
	p(NT(369), (NT(370)));
//G513: cli_command(278)     => def_rr_cmd(369).
	p(NT(278), (NT(369)));
//G514: __E_cli_command_153(372) => def_sym(367) __(72) memory(373).
	p(NT(372), (NT(367)+NT(72)+NT(373)));
//G515: def_print_cmd(371)   => __E_cli_command_153(372).
	p(NT(371), (NT(372)));
//G516: cli_command(278)     => def_print_cmd(371).
	p(NT(278), (NT(371)));
//G517: __E___E_cli_command_154_155(377) => 'h' 'i' 's' 't'.
	p(NT(377), (T(49)+T(18)+T(13)+T(17)));
//G518: __E___E_cli_command_154_155(377) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(377), (T(49)+T(18)+T(13)+T(17)+T(14)+T(44)+T(23)));
//G519: history_sym(376)     => __E___E_cli_command_154_155(377).
	p(NT(376), (NT(377)));
//G520: __E_cli_command_154(375) => history_sym(376).
	p(NT(375), (NT(376)));
//G521: history_list_cmd(374) => __E_cli_command_154(375).
	p(NT(374), (NT(375)));
//G522: cli_command(278)     => history_list_cmd(374).
	p(NT(278), (NT(374)));
//G523: __E_cli_command_156(379) => history_sym(376) __(72) memory(373).
	p(NT(379), (NT(376)+NT(72)+NT(373)));
//G524: history_print_cmd(378) => __E_cli_command_156(379).
	p(NT(378), (NT(379)));
//G525: cli_command(278)     => history_print_cmd(378).
	p(NT(278), (NT(378)));
//G526: __E___E_cli_command_157_158(383) => bf_typed(384).
	p(NT(383), (NT(384)));
//G527: __E___E_cli_command_157_158(383) => wff_typed(320).
	p(NT(383), (NT(320)));
//G528: history_store_cmd_arg(382) => __E___E_cli_command_157_158(383).
	p(NT(382), (NT(383)));
//G529: __E_cli_command_157(381) => history_store_cmd_arg(382).
	p(NT(381), (NT(382)));
//G530: history_store_cmd(380) => __E_cli_command_157(381).
	p(NT(380), (NT(381)));
//G531: cli_command(278)     => history_store_cmd(380).
	p(NT(278), (NT(380)));
//G532: qelim_sym(387)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(387), (T(47)+T(16)+T(21)+T(18)+T(15)));
//G533: __E_cli_command_159(386) => qelim_sym(387) __(72) wff_cmd_arg(313).
	p(NT(386), (NT(387)+NT(72)+NT(313)));
//G534: qelim_cmd(385)       => __E_cli_command_159(386).
	p(NT(385), (NT(386)));
//G535: cli_command(278)     => qelim_cmd(385).
	p(NT(278), (NT(385)));
//G536: get_sym(390)         => 'g' 'e' 't'.
	p(NT(390), (T(50)+T(16)+T(17)));
//G537: __E___E_cli_command_160_161(391) => __(72) option(392).
	p(NT(391), (NT(72)+NT(392)));
//G538: __E___E_cli_command_160_161(391) => null.
	p(NT(391), (nul));
//G539: __E_cli_command_160(389) => get_sym(390) __E___E_cli_command_160_161(391).
	p(NT(389), (NT(390)+NT(391)));
//G540: get_cmd(388)         => __E_cli_command_160(389).
	p(NT(388), (NT(389)));
//G541: cli_command(278)     => get_cmd(388).
	p(NT(278), (NT(388)));
//G542: set_sym(395)         => 's' 'e' 't'.
	p(NT(395), (T(13)+T(16)+T(17)));
//G543: __E___E_cli_command_162_163(396) => __(72).
	p(NT(396), (NT(72)));
//G544: __E___E_cli_command_162_163(396) => _(16) '=' _(16).
	p(NT(396), (NT(16)+T(4)+NT(16)));
//G545: __E_cli_command_162(394) => set_sym(395) __(72) option(392) __E___E_cli_command_162_163(396) option_value(397).
	p(NT(394), (NT(395)+NT(72)+NT(392)+NT(396)+NT(397)));
//G546: set_cmd(393)         => __E_cli_command_162(394).
	p(NT(393), (NT(394)));
//G547: cli_command(278)     => set_cmd(393).
	p(NT(278), (NT(393)));
//G548: toggle_sym(400)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(400), (T(17)+T(14)+T(50)+T(50)+T(21)+T(16)));
//G549: __E_cli_command_164(399) => toggle_sym(400) __(72) bool_option(401).
	p(NT(399), (NT(400)+NT(72)+NT(401)));
//G550: toggle_cmd(398)      => __E_cli_command_164(399).
	p(NT(398), (NT(399)));
//G551: cli_command(278)     => toggle_cmd(398).
	p(NT(278), (NT(398)));
//G552: cli_command(278)     => comment(271).
	p(NT(278), (NT(271)));
//G553: bf_typed(384)        => bf(41).
	p(NT(384), (NT(41)));
//G554: bf_typed(384)        => bf(41) _(16) ':' _(16) 'b' 'f'.
	p(NT(384), (NT(41)+NT(16)+T(3)+NT(16)+T(27)+T(46)));
//G555: wff_typed(320)       => wff(32).
	p(NT(320), (NT(32)));
//G556: wff_typed(320)       => wff(32) _(16) ':' _(16) 'w' 'f' 'f'.
	p(NT(320), (NT(32)+NT(16)+T(3)+NT(16)+T(22)+T(46)+T(46)));
//G557: bf_cmd_arg(338)      => memory(373).
	p(NT(338), (NT(373)));
//G558: bf_cmd_arg(338)      => bf_selection(402).
	p(NT(338), (NT(402)));
//G559: bf_cmd_arg(338)      => bf_typed(384).
	p(NT(338), (NT(384)));
//G560: bf_cmd_out_arg(339)  => memory(373).
	p(NT(339), (NT(373)));
//G561: bf_cmd_out_arg(339)  => bf_selection(402).
	p(NT(339), (NT(402)));
//G562: wff_cmd_arg(313)     => memory(373).
	p(NT(313), (NT(373)));
//G563: wff_cmd_arg(313)     => wff_selection(403).
	p(NT(313), (NT(403)));
//G564: wff_cmd_arg(313)     => wff_typed(320).
	p(NT(313), (NT(320)));
//G565: wff_cmd_out_arg(330) => memory(373).
	p(NT(330), (NT(373)));
//G566: wff_cmd_out_arg(330) => wff_selection(403).
	p(NT(330), (NT(403)));
//G567: nf_cmd_arg(335)      => memory(373).
	p(NT(335), (NT(373)));
//G568: nf_cmd_arg(335)      => bf_typed(384).
	p(NT(335), (NT(384)));
//G569: nf_cmd_arg(335)      => wff_typed(320).
	p(NT(335), (NT(320)));
//G570: onf_cmd_arg(364)     => memory(373).
	p(NT(364), (NT(373)));
//G571: onf_cmd_arg(364)     => wff_typed(320).
	p(NT(364), (NT(320)));
//G572: normalize_cmd_arg(325) => memory(373).
	p(NT(325), (NT(373)));
//G573: normalize_cmd_arg(325) => bf_typed(384).
	p(NT(325), (NT(384)));
//G574: normalize_cmd_arg(325) => wff_typed(320).
	p(NT(325), (NT(320)));
//G575: normalize_cmd_arg(325) => nso_rr(14).
	p(NT(325), (NT(14)));
//G576: help_arg(295)        => help_sym(292).
	p(NT(295), (NT(292)));
//G577: help_arg(295)        => version_sym(285).
	p(NT(295), (NT(285)));
//G578: help_arg(295)        => quit_sym(282).
	p(NT(295), (NT(282)));
//G579: help_arg(295)        => clear_sym(288).
	p(NT(295), (NT(288)));
//G580: help_arg(295)        => get_sym(390).
	p(NT(295), (NT(390)));
//G581: help_arg(295)        => set_sym(395).
	p(NT(295), (NT(395)));
//G582: help_arg(295)        => toggle_sym(400).
	p(NT(295), (NT(400)));
//G583: help_arg(295)        => file_sym(298).
	p(NT(295), (NT(298)));
//G584: help_arg(295)        => history_sym(376).
	p(NT(295), (NT(376)));
//G585: help_arg(295)        => abs_memory_sym(404).
	p(NT(295), (NT(404)));
//G586: help_arg(295)        => rel_memory_sym(405).
	p(NT(295), (NT(405)));
//G587: help_arg(295)        => selection_sym(406).
	p(NT(295), (NT(406)));
//G588: help_arg(295)        => def_sym(367).
	p(NT(295), (NT(367)));
//G589: help_arg(295)        => inst_sym(328).
	p(NT(295), (NT(328)));
//G590: help_arg(295)        => subst_sym(333).
	p(NT(295), (NT(333)));
//G591: help_arg(295)        => normalize_sym(323).
	p(NT(295), (NT(323)));
//G592: help_arg(295)        => execute_sym(316).
	p(NT(295), (NT(316)));
//G593: help_arg(295)        => solve_sym(311).
	p(NT(295), (NT(311)));
//G594: help_arg(295)        => valid_sym(302).
	p(NT(295), (NT(302)));
//G595: help_arg(295)        => sat_sym(305).
	p(NT(295), (NT(305)));
//G596: help_arg(295)        => unsat_sym(308).
	p(NT(295), (NT(308)));
//G597: help_arg(295)        => dnf_sym(342).
	p(NT(295), (NT(342)));
//G598: help_arg(295)        => cnf_sym(345).
	p(NT(295), (NT(345)));
//G599: help_arg(295)        => anf_sym(348).
	p(NT(295), (NT(348)));
//G600: help_arg(295)        => snf_sym(360).
	p(NT(295), (NT(360)));
//G601: help_arg(295)        => nnf_sym(351).
	p(NT(295), (NT(351)));
//G602: help_arg(295)        => pnf_sym(354).
	p(NT(295), (NT(354)));
//G603: help_arg(295)        => mnf_sym(357).
	p(NT(295), (NT(357)));
//G604: help_arg(295)        => onf_sym(363).
	p(NT(295), (NT(363)));
//G605: help_arg(295)        => qelim_sym(387).
	p(NT(295), (NT(387)));
//G606: __E___E_help_arg_165_166(409) => 's'.
	p(NT(409), (T(13)));
//G607: __E___E_help_arg_165_166(409) => null.
	p(NT(409), (nul));
//G608: __E_help_arg_165(408) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_165_166(409).
	p(NT(408), (T(16)+T(29)+T(20)+T(15)+T(42)+T(21)+T(16)+NT(409)));
//G609: examples_sym(407)    => __E_help_arg_165(408).
	p(NT(407), (NT(408)));
//G610: help_arg(295)        => examples_sym(407).
	p(NT(295), (NT(407)));
//G611: rel_memory_sym(405)  => '%' '-'.
	p(NT(405), (T(58)+T(12)));
//G612: memory_id(414)       => digits(243).
	p(NT(414), (NT(243)));
//G613: __E___E___E_memory_167_168_169(413) => memory_id(414).
	p(NT(413), (NT(414)));
//G614: __E___E___E_memory_167_168_169(413) => null.
	p(NT(413), (nul));
//G615: __E___E_memory_167_168(412) => rel_memory_sym(405) __E___E___E_memory_167_168_169(413).
	p(NT(412), (NT(405)+NT(413)));
//G616: __E___E_memory_167_168(412) => '%'.
	p(NT(412), (T(58)));
//G617: __E_memory_167(411)  => __E___E_memory_167_168(412).
	p(NT(411), (NT(412)));
//G618: rel_memory(410)      => __E_memory_167(411).
	p(NT(410), (NT(411)));
//G619: memory(373)          => rel_memory(410).
	p(NT(373), (NT(410)));
//G620: abs_memory_sym(404)  => '%'.
	p(NT(404), (T(58)));
//G621: __E_memory_170(416)  => abs_memory_sym(404) memory_id(414).
	p(NT(416), (NT(404)+NT(414)));
//G622: abs_memory(415)      => __E_memory_170(416).
	p(NT(415), (NT(416)));
//G623: memory(373)          => abs_memory(415).
	p(NT(373), (NT(415)));
//G624: selection_sym(406)   => 's'.
	p(NT(406), (T(13)));
//G625: selection_sym(406)   => 's' 'e' 'l' 'e' 'c' 't' 'i' 'o' 'n'.
	p(NT(406), (T(13)+T(16)+T(21)+T(16)+T(41)+T(17)+T(18)+T(14)+T(48)));
//G626: wff_selection(403)   => selection_sym(406) __(72) digits(243).
	p(NT(403), (NT(406)+NT(72)+NT(243)));
//G627: bf_selection(402)    => selection_sym(406) __(72) digits(243).
	p(NT(402), (NT(406)+NT(72)+NT(243)));
//G628: bf_var_selection(417) => selection_sym(406) __(72) digits(243).
	p(NT(417), (NT(406)+NT(72)+NT(243)));
//G629: wff_var_selection(418) => selection_sym(406) __(72) digits(243).
	p(NT(418), (NT(406)+NT(72)+NT(243)));
//G630: option(392)          => bool_option(401).
	p(NT(392), (NT(401)));
//G631: __E_option_171(421)  => 's' 'e' 'v'.
	p(NT(421), (T(13)+T(16)+T(43)));
//G632: __E_option_171(421)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(421), (T(13)+T(16)+T(43)+T(16)+T(44)+T(18)+T(17)+T(23)));
//G633: severity_opt(420)    => __E_option_171(421).
	p(NT(420), (NT(421)));
//G634: enum_option(419)     => severity_opt(420).
	p(NT(419), (NT(420)));
//G635: option(392)          => enum_option(419).
	p(NT(392), (NT(419)));
//G636: __E_bool_option_172(423) => 's'.
	p(NT(423), (T(13)));
//G637: __E_bool_option_172(423) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(423), (T(13)+T(17)+T(20)+T(17)+T(40)+T(13)));
//G638: status_opt(422)      => __E_bool_option_172(423).
	p(NT(422), (NT(423)));
//G639: bool_option(401)     => status_opt(422).
	p(NT(401), (NT(422)));
//G640: __E_bool_option_173(425) => 'c'.
	p(NT(425), (T(41)));
//G641: __E_bool_option_173(425) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(425), (T(41)+T(14)+T(21)+T(14)+T(44)));
//G642: __E_bool_option_173(425) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(425), (T(41)+T(14)+T(21)+T(14)+T(44)+T(13)));
//G643: colors_opt(424)      => __E_bool_option_173(425).
	p(NT(424), (NT(425)));
//G644: bool_option(401)     => colors_opt(424).
	p(NT(401), (NT(424)));
//G645: __E_bool_option_174(427) => 'd'.
	p(NT(427), (T(45)));
//G646: __E_bool_option_174(427) => 'd' 'b' 'g'.
	p(NT(427), (T(45)+T(27)+T(50)));
//G647: __E_bool_option_174(427) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(427), (T(45)+T(16)+T(27)+T(40)+T(50)));
//G648: debug_repl_opt(426)  => __E_bool_option_174(427).
	p(NT(426), (NT(427)));
//G649: bool_option(401)     => debug_repl_opt(426).
	p(NT(401), (NT(426)));
//G650: option_value(397)    => option_value_true(428).
	p(NT(397), (NT(428)));
//G651: option_value(397)    => option_value_false(429).
	p(NT(397), (NT(429)));
//G652: option_value(397)    => severity(430).
	p(NT(397), (NT(430)));
//G653: option_value_true(428) => 't'.
	p(NT(428), (T(17)));
//G654: option_value_true(428) => 't' 'r' 'u' 'e'.
	p(NT(428), (T(17)+T(44)+T(40)+T(16)));
//G655: option_value_true(428) => 'o' 'n'.
	p(NT(428), (T(14)+T(48)));
//G656: option_value_true(428) => '1'.
	p(NT(428), (T(37)));
//G657: option_value_true(428) => 'y'.
	p(NT(428), (T(23)));
//G658: option_value_true(428) => 'y' 'e' 's'.
	p(NT(428), (T(23)+T(16)+T(13)));
//G659: option_value_false(429) => 'f'.
	p(NT(429), (T(46)));
//G660: option_value_false(429) => 'f' 'a' 'l' 's' 'e'.
	p(NT(429), (T(46)+T(20)+T(21)+T(13)+T(16)));
//G661: option_value_false(429) => 'o' 'f' 'f'.
	p(NT(429), (T(14)+T(46)+T(46)));
//G662: option_value_false(429) => '0'.
	p(NT(429), (T(38)));
//G663: option_value_false(429) => 'n'.
	p(NT(429), (T(48)));
//G664: option_value_false(429) => 'n' 'o'.
	p(NT(429), (T(48)+T(14)));
//G665: __E_severity_175(432) => 'e'.
	p(NT(432), (T(16)));
//G666: __E_severity_175(432) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(432), (T(16)+T(44)+T(44)+T(14)+T(44)));
//G667: error_sym(431)       => __E_severity_175(432).
	p(NT(431), (NT(432)));
//G668: severity(430)        => error_sym(431).
	p(NT(430), (NT(431)));
//G669: __E_severity_176(434) => 'i'.
	p(NT(434), (T(18)));
//G670: __E_severity_176(434) => 'i' 'n' 'f' 'o'.
	p(NT(434), (T(18)+T(48)+T(46)+T(14)));
//G671: info_sym(433)        => __E_severity_176(434).
	p(NT(433), (NT(434)));
//G672: severity(430)        => info_sym(433).
	p(NT(430), (NT(433)));
//G673: __E_severity_177(436) => 'd'.
	p(NT(436), (T(45)));
//G674: __E_severity_177(436) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(436), (T(45)+T(16)+T(27)+T(40)+T(50)));
//G675: debug_sym(435)       => __E_severity_177(436).
	p(NT(435), (NT(436)));
//G676: severity(430)        => debug_sym(435).
	p(NT(430), (NT(435)));
//G677: __E_severity_178(438) => 't'.
	p(NT(438), (T(17)));
//G678: __E_severity_178(438) => 't' 'r' 'a' 'c' 'e'.
	p(NT(438), (T(17)+T(44)+T(20)+T(41)+T(16)));
//G679: trace_sym(437)       => __E_severity_178(438).
	p(NT(437), (NT(438)));
//G680: severity(430)        => trace_sym(437).
	p(NT(430), (NT(437)));
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
		__E_tau_27, tau_and, __E_tau_28, tau_neg, __E_tau_29, wff_parenthesis, __E_wff_30, global_or, __E_wff_31, global_and, 
		__E_wff_32, global_neg, __E_wff_33, wff_sometimes, __E_wff_34, __E___E_wff_34_35, wff_always, __E_wff_36, __E___E_wff_36_37, wff_conditional, 
		__E_wff_38, wff_ball, __E_wff_39, q_bool_var, wff_bex, __E_wff_40, wff_all, __E_wff_41, q_var, wff_ex, 
		__E_wff_42, __E_wff_43, __E___E_wff_43_44, wff_imply, __E_wff_45, wff_equiv, __E_wff_46, wff_or, __E_wff_47, wff_xor, 
		__E_wff_48, wff_and, __E_wff_49, wff_neg, __E_wff_50, wff_t, wff_f, bf_interval, __E_wff_51, bf_neq, 
		__E_wff_52, bf_eq, __E_wff_53, bf_nleq_lower, __E_wff_54, bf_nleq_upper, __E_wff_55, bf_greater, __E_wff_56, bf_less_equal, 
		__E_wff_57, bf_less, __E_wff_58, bool_variable, uninterpreted_constant, bf_parenthesis, __E_bf_59, bf_splitter, __E_bf_60, __E_bf_61, 
		__E___E_bf_61_62, bf_or, __E_bf_63, bf_xor, __E_bf_64, bf_and, __E_bf_65, __E___E_bf_65_66, bf_neg, __E_bf_67, 
		bf_constant, __E_bf_68, constant, bf_t, bf_f, variable, __E_ref_args_69, __E___E_ref_args_69_70, ref_arg, __E___E_ref_args_69_71, 
		__E___E_ref_args_69_72, bf_cb_args1, bf_cb_arg, bf_cb_args2, bf_cb_args3, bf_cb_args4, wff_cb_args1, wff_cb_arg, wff_cb_args2, wff_cb_args3, 
		wff_cb_args4, tau_cb_args1, tau_cb_arg, __E_tau_cb_args1_73, tau_cb_args2, tau_cb_args3, tau_collapse_positives_cb, __E_tau_cb_74, __E___E_tau_cb_74_75, tau_positives_upwards_cb, 
		__E_tau_cb_76, bf_eq_cb, __E_wff_cb_77, bf_neq_cb, __E_wff_cb_78, wff_has_clashing_subformulas_cb, __E_wff_cb_79, wff_has_subformula_cb, __E_wff_cb_80, wff_remove_existential_cb, 
		__E_wff_cb_81, wff_remove_bexistential_cb, __E_wff_cb_82, wff_remove_buniversal_cb, __E_wff_cb_83, bf_is_zero_cb, __E_bf_cb_84, bf_is_one_cb, __E_bf_cb_85, bf_has_subformula_cb, 
		__E_bf_cb_86, bf_remove_funiversal_cb, __E_bf_cb_87, bf_remove_fexistential_cb, __E_bf_cb_88, bf_and_cb, __E_bf_cb_89, bf_or_cb, __E_bf_cb_90, bf_xor_cb, 
		__E_bf_cb_91, bf_neg_cb, __E_bf_cb_92, bf_normalize_cb, __E_bf_cb_93, charvar, __E_charvar_94, __E_charvar_95, io_var, __E_variable_96, 
		out, in_var_name, __E_in_97, digits, offset, out_var_name, __E_out_98, __E_offsets_99, __E_offsets_100, num, 
		shift, __E_offset_101, __E_shift_102, __E___E_shift_102_103, uninter_const_name, __E_uninterpreted_constant_104, __E___E_uninterpreted_constant_104_105, chars, binding, __E_binding_106, 
		type, __E___E_binding_106_107, source, named_binding, src_c, __E_source_108, __E_source_109, __E___E_source_109_110, __E___E_source_109_111, __E_chars_112, 
		__E_digits_113, comment, __E_comment_114, __E_comment_115, __E_comment_116, __E____117, __E___118, cli, cli_command, __E_cli_119, 
		__E_cli_120, quit_cmd, quit_sym, __E_cli_command_121, version_cmd, version_sym, __E_cli_command_122, clear_cmd, clear_sym, __E_cli_command_123, 
		help_cmd, __E_cli_command_124, help_sym, __E___E_cli_command_124_125, __E___E_cli_command_124_126, help_arg, file_cmd, __E_cli_command_127, file_sym, q_string, 
		valid_cmd, __E_cli_command_128, valid_sym, sat_cmd, __E_cli_command_129, sat_sym, unsat_cmd, __E_cli_command_130, unsat_sym, solve_cmd, 
		__E_cli_command_131, solve_sym, solve_cmd_arg, wff_cmd_arg, execute_cmd, __E_cli_command_132, execute_sym, __E___E_cli_command_132_133, execute_cmd_arg, __E___E_cli_command_132_134, 
		wff_typed, normalize_cmd, __E_cli_command_135, normalize_sym, __E___E_cli_command_135_136, normalize_cmd_arg, wff_inst_cmd, __E_cli_command_137, inst_sym, __E___E_cli_command_137_138, 
		wff_cmd_out_arg, subst_cmd, __E_cli_command_139, subst_sym, __E___E_cli_command_139_140, nf_cmd_arg, bf_inst_cmd, __E_cli_command_141, bf_cmd_arg, bf_cmd_out_arg, 
		dnf_cmd, __E_cli_command_142, dnf_sym, cnf_cmd, __E_cli_command_143, cnf_sym, anf_cmd, __E_cli_command_144, anf_sym, nnf_cmd, 
		__E_cli_command_145, nnf_sym, pnf_cmd, __E_cli_command_146, pnf_sym, mnf_cmd, __E_cli_command_147, mnf_sym, snf_cmd, __E_cli_command_148, 
		snf_sym, onf_cmd, __E_cli_command_149, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_150, def_sym, __E___E_cli_command_150_151, def_rr_cmd, 
		__E_cli_command_152, def_print_cmd, __E_cli_command_153, memory, history_list_cmd, __E_cli_command_154, history_sym, __E___E_cli_command_154_155, history_print_cmd, __E_cli_command_156, 
		history_store_cmd, __E_cli_command_157, history_store_cmd_arg, __E___E_cli_command_157_158, bf_typed, qelim_cmd, __E_cli_command_159, qelim_sym, get_cmd, __E_cli_command_160, 
		get_sym, __E___E_cli_command_160_161, option, set_cmd, __E_cli_command_162, set_sym, __E___E_cli_command_162_163, option_value, toggle_cmd, __E_cli_command_164, 
		toggle_sym, bool_option, bf_selection, wff_selection, abs_memory_sym, rel_memory_sym, selection_sym, examples_sym, __E_help_arg_165, __E___E_help_arg_165_166, 
		rel_memory, __E_memory_167, __E___E_memory_167_168, __E___E___E_memory_167_168_169, memory_id, abs_memory, __E_memory_170, bf_var_selection, wff_var_selection, enum_option, 
		severity_opt, __E_option_171, status_opt, __E_bool_option_172, colors_opt, __E_bool_option_173, debug_repl_opt, __E_bool_option_174, option_value_true, option_value_false, 
		severity, error_sym, __E_severity_175, info_sym, __E_severity_176, debug_sym, __E_severity_177, trace_sym, __E_severity_178, bf_all, 
		bf_ex, del_sym, __N_0, __N_1, __N_2, __N_3, __N_4, __N_5, 
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
