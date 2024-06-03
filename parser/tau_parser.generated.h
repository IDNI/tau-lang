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
	"__E_wff_51", "bool_variable", "uninterpreted_constant", "bf_parenthesis", "__E_bf_52", "bf_splitter", "__E_bf_53", "__E_bf_54", "__E___E_bf_54_55", "bf_or", 
	"__E_bf_56", "bf_xor", "__E_bf_57", "bf_and", "__E_bf_58", "__E___E_bf_58_59", "bf_neg", "__E_bf_60", "bf_constant", "__E_bf_61", 
	"constant", "bf_t", "bf_f", "variable", "__E_ref_args_62", "__E___E_ref_args_62_63", "ref_arg", "__E___E_ref_args_62_64", "__E___E_ref_args_62_65", "bf_cb_args1", 
	"bf_cb_arg", "bf_cb_args2", "bf_cb_args3", "bf_cb_args4", "wff_cb_args1", "wff_cb_arg", "wff_cb_args2", "wff_cb_args3", "wff_cb_args4", "tau_cb_args1", 
	"tau_cb_arg", "__E_tau_cb_args1_66", "tau_cb_args2", "tau_cb_args3", "tau_collapse_positives_cb", "__E_tau_cb_67", "__E___E_tau_cb_67_68", "tau_positives_upwards_cb", "__E_tau_cb_69", "bf_eq_cb", 
	"__E_wff_cb_70", "bf_neq_cb", "__E_wff_cb_71", "wff_has_clashing_subformulas_cb", "__E_wff_cb_72", "wff_has_subformula_cb", "__E_wff_cb_73", "wff_remove_existential_cb", "__E_wff_cb_74", "wff_remove_bexistential_cb", 
	"__E_wff_cb_75", "wff_remove_buniversal_cb", "__E_wff_cb_76", "bf_is_zero_cb", "__E_bf_cb_77", "bf_is_one_cb", "__E_bf_cb_78", "bf_has_subformula_cb", "__E_bf_cb_79", "bf_remove_funiversal_cb", 
	"__E_bf_cb_80", "bf_remove_fexistential_cb", "__E_bf_cb_81", "bf_and_cb", "__E_bf_cb_82", "bf_or_cb", "__E_bf_cb_83", "bf_xor_cb", "__E_bf_cb_84", "bf_neg_cb", 
	"__E_bf_cb_85", "charvar", "__E_charvar_86", "__E_charvar_87", "io_var", "__E_variable_88", "out", "in_var_name", "__E_in_89", "chars", 
	"offset", "out_var_name", "__E_out_90", "__E_offsets_91", "__E_offsets_92", "num", "shift", "__E_offset_93", "__E_shift_94", "__E___E_shift_94_95", 
	"digits", "uninter_const_name", "__E_uninterpreted_constant_96", "__E___E_uninterpreted_constant_96_97", "binding", "__E_binding_98", "type", "__E___E_binding_98_99", "source", "named_binding", 
	"src_c", "__E_source_100", "__E_source_101", "__E___E_source_101_102", "__E___E_source_101_103", "__E_chars_104", "__E_digits_105", "comment", "__E_comment_106", "__E_comment_107", 
	"__E_comment_108", "__E____109", "__E___110", "cli", "cli_command", "__E_cli_111", "__E_cli_112", "quit_cmd", "quit_sym", "__E_cli_command_113", 
	"version_cmd", "version_sym", "__E_cli_command_114", "clear_cmd", "clear_sym", "__E_cli_command_115", "help_cmd", "__E_cli_command_116", "help_sym", "__E___E_cli_command_116_117", 
	"__E___E_cli_command_116_118", "help_arg", "file_cmd", "__E_cli_command_119", "file_sym", "q_string", "valid_cmd", "__E_cli_command_120", "valid_sym", "sat_cmd", 
	"__E_cli_command_121", "sat_sym", "unsat_cmd", "__E_cli_command_122", "unsat_sym", "solve_cmd", "__E_cli_command_123", "solve_sym", "solve_cmd_arg", "wff_cmd_arg", 
	"execute_cmd", "__E_cli_command_124", "execute_sym", "__E___E_cli_command_124_125", "execute_cmd_arg", "__E___E_cli_command_124_126", "normalize_cmd", "__E_cli_command_127", "normalize_sym", "__E___E_cli_command_127_128", 
	"normalize_cmd_arg", "wff_inst_cmd", "__E_cli_command_129", "inst_sym", "__E___E_cli_command_129_130", "wff_cmd_out_arg", "wff_subst_cmd", "__E_cli_command_131", "subst_sym", "__E___E_cli_command_131_132", 
	"bf_inst_cmd", "__E_cli_command_133", "bf_cmd_arg", "bf_cmd_out_arg", "bf_subst_cmd", "__E_cli_command_134", "dnf_cmd", "__E_cli_command_135", "dnf_sym", "nf_cmd_arg", 
	"cnf_cmd", "__E_cli_command_136", "cnf_sym", "anf_cmd", "__E_cli_command_137", "anf_sym", "nnf_cmd", "__E_cli_command_138", "nnf_sym", "pnf_cmd", 
	"__E_cli_command_139", "pnf_sym", "mnf_cmd", "__E_cli_command_140", "mnf_sym", "snf_cmd", "__E_cli_command_141", "snf_sym", "onf_cmd", "__E_cli_command_142", 
	"onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_143", "def_sym", "def_rr_cmd", "__E_cli_command_144", "def_del_cmd", "__E_cli_command_145", "del_sym", 
	"__E___E_cli_command_145_146", "memory", "def_clear_cmd", "__E_cli_command_147", "def_print_cmd", "__E_cli_command_148", "memory_list_cmd", "__E_cli_command_149", "memory_sym", "__E___E_cli_command_149_150", 
	"memory_del_cmd", "__E_cli_command_151", "memory_clear_cmd", "__E_cli_command_152", "memory_print_cmd", "__E_cli_command_153", "memory_store_cmd", "__E_cli_command_154", "memory_store_cmd_arg", "__E___E_cli_command_154_155", 
	"qelim_cmd", "__E_cli_command_156", "qelim_sym", "get_cmd", "__E_cli_command_157", "get_sym", "__E___E_cli_command_157_158", "option", "set_cmd", "__E_cli_command_159", 
	"set_sym", "__E___E_cli_command_159_160", "option_value", "toggle_cmd", "__E_cli_command_161", "toggle_sym", "bool_option", "bf_selection", "wff_selection", "abs_memory_sym", 
	"rel_memory_sym", "selection_sym", "examples_sym", "__E_help_arg_162", "__E___E_help_arg_162_163", "rel_memory", "__E_memory_164", "__E___E_memory_164_165", "memory_id", "abs_memory", 
	"__E_memory_166", "__E___E_memory_166_167", "bf_var_selection", "wff_var_selection", "enum_option", "severity_opt", "__E_option_168", "status_opt", "__E_bool_option_169", "colors_opt", 
	"__E_bool_option_170", "debug_repl_opt", "__E_bool_option_171", "option_value_true", "option_value_false", "severity", "error_sym", "__E_severity_172", "info_sym", "__E_severity_173", 
	"debug_sym", "__E_severity_174", "trace_sym", "__E_severity_175", "bf_all", "bf_ex", "__N_0", "__N_1", "__N_2", "__N_3", 
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
			127, 129, 131, 133, 135, 137, 139, 143, 145, 149,
			151, 153, 156, 158, 161, 162, 184, 187, 189, 191,
			193, 195, 197, 199, 201, 203, 205, 207, 209, 211,
			213, 215, 217, 219, 236, 263, 268, 271, 274, 278,
			284, 288, 291, 294, 297, 302, 308, 313, 318, 328,
			332, 335, 338, 341, 344, 347, 350, 354, 359, 368,
			382, 385, 388, 390, 395, 399, 400, 401, 402, 415,
			417, 419, 421, 423, 424, 426, 428, 430, 432, 434,
			435
		},
		.trim_terminals = false,
		.to_inline = {
			{ 20, 80, 20 },
			{ 32, 95, 32 },
			{ 41, 143, 41 },
			{ 59 },
			{ 63 },
			{ 67 },
			{ 101 },
			{ 106 },
			{ 169 },
			{ 171 },
			{ 172 },
			{ 173 },
			{ 174 },
			{ 176 },
			{ 177 },
			{ 178 },
			{ 179 },
			{ 182 },
			{ 183 },
			{ 229 },
			{ 250 },
			{ 299 },
			{ 315 },
			{ 322 },
			{ 323 }
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
//G168: wff(32)              => uninterpreted_constant(142).
	p(NT(32), (NT(142)));
//G169: __E_bf_52(144)       => '(' _(16) bf(41) _(16) ')'.
	p(NT(144), (T(8)+NT(16)+NT(41)+NT(16)+T(9)));
//G170: bf_parenthesis(143)  => __E_bf_52(144).
	p(NT(143), (NT(144)));
//G171: bf(41)               => bf_parenthesis(143).
	p(NT(41), (NT(143)));
//G172: __E_bf_53(146)       => 'S' _(16) '(' _(16) bf(41) _(16) ')'.
	p(NT(146), (T(26)+NT(16)+T(8)+NT(16)+NT(41)+NT(16)+T(9)));
//G173: bf_splitter(145)     => __E_bf_53(146).
	p(NT(145), (NT(146)));
//G174: bf(41)               => bf_splitter(145).
	p(NT(41), (NT(145)));
//G175: __E___E_bf_54_55(148) => _(16) offsets(87).
	p(NT(148), (NT(16)+NT(87)));
//G176: __E___E_bf_54_55(148) => null.
	p(NT(148), (nul));
//G177: __E_bf_54(147)       => sym(85) __E___E_bf_54_55(148) _(16) ref_args(88).
	p(NT(147), (NT(85)+NT(148)+NT(16)+NT(88)));
//G178: bf_ref(40)           => __E_bf_54(147).
	p(NT(40), (NT(147)));
//G179: bf(41)               => bf_ref(40).
	p(NT(41), (NT(40)));
//G180: __E_bf_56(150)       => bf(41) _(16) '|' _(16) bf(41).
	p(NT(150), (NT(41)+NT(16)+T(10)+NT(16)+NT(41)));
//G181: bf_or(149)           => __E_bf_56(150).
	p(NT(149), (NT(150)));
//G182: bf(41)               => bf_or(149).
	p(NT(41), (NT(149)));
//G183: __E_bf_57(152)       => bf(41) _(16) '+' _(16) bf(41).
	p(NT(152), (NT(41)+NT(16)+T(27)+NT(16)+NT(41)));
//G184: bf_xor(151)          => __E_bf_57(152).
	p(NT(151), (NT(152)));
//G185: bf(41)               => bf_xor(151).
	p(NT(41), (NT(151)));
//G186: __E___E_bf_58_59(155) => _(16).
	p(NT(155), (NT(16)));
//G187: __E___E_bf_58_59(155) => _(16) '&' _(16).
	p(NT(155), (NT(16)+T(11)+NT(16)));
//G188: __E_bf_58(154)       => bf(41) __E___E_bf_58_59(155) bf(41).
	p(NT(154), (NT(41)+NT(155)+NT(41)));
//G189: bf_and(153)          => __E_bf_58(154).
	p(NT(153), (NT(154)));
//G190: bf(41)               => bf_and(153).
	p(NT(41), (NT(153)));
//G191: __E_bf_60(157)       => bf(41) _(16) '\''.
	p(NT(157), (NT(41)+NT(16)+T(28)));
//G192: bf_neg(156)          => __E_bf_60(157).
	p(NT(156), (NT(157)));
//G193: bf(41)               => bf_neg(156).
	p(NT(41), (NT(156)));
//G194: __E_bf_61(159)       => '{' _(16) constant(160) _(16) '}'.
	p(NT(159), (T(6)+NT(16)+NT(160)+NT(16)+T(7)));
//G195: bf_constant(158)     => __E_bf_61(159).
	p(NT(158), (NT(159)));
//G196: bf(41)               => bf_constant(158).
	p(NT(41), (NT(158)));
//G197: bf_t(161)            => '1'.
	p(NT(161), (T(29)));
//G198: bf(41)               => bf_t(161).
	p(NT(41), (NT(161)));
//G199: bf_f(162)            => '0'.
	p(NT(162), (T(30)));
//G200: bf(41)               => bf_f(162).
	p(NT(41), (NT(162)));
//G201: bf(41)               => capture(70).
	p(NT(41), (NT(70)));
//G202: bf(41)               => variable(163).
	p(NT(41), (NT(163)));
//G203: bf(41)               => uninterpreted_constant(142).
	p(NT(41), (NT(142)));
//G204: ref_arg(166)         => bf(41).
	p(NT(166), (NT(41)));
//G205: __E___E_ref_args_62_63(165) => _(16) ref_arg(166).
	p(NT(165), (NT(16)+NT(166)));
//G206: __E___E_ref_args_62_64(167) => _(16) ',' _(16) ref_arg(166).
	p(NT(167), (NT(16)+T(31)+NT(16)+NT(166)));
//G207: __E___E_ref_args_62_65(168) => null.
	p(NT(168), (nul));
//G208: __E___E_ref_args_62_65(168) => __E___E_ref_args_62_64(167) __E___E_ref_args_62_65(168).
	p(NT(168), (NT(167)+NT(168)));
//G209: __E_ref_args_62(164) => __E___E_ref_args_62_63(165) __E___E_ref_args_62_65(168).
	p(NT(164), (NT(165)+NT(168)));
//G210: __E_ref_args_62(164) => null.
	p(NT(164), (nul));
//G211: ref_args(88)         => '(' __E_ref_args_62(164) _(16) ')'.
	p(NT(88), (T(8)+NT(164)+NT(16)+T(9)));
//G212: bf_cb_arg(170)       => bf(41).
	p(NT(170), (NT(41)));
//G213: bf_cb_args1(169)     => __(72) bf_cb_arg(170).
	p(NT(169), (NT(72)+NT(170)));
//G214: bf_cb_args2(171)     => bf_cb_args1(169) bf_cb_args1(169).
	p(NT(171), (NT(169)+NT(169)));
//G215: bf_cb_args3(172)     => bf_cb_args2(171) bf_cb_args1(169).
	p(NT(172), (NT(171)+NT(169)));
//G216: bf_cb_args4(173)     => bf_cb_args3(172) bf_cb_args1(169).
	p(NT(173), (NT(172)+NT(169)));
//G217: wff_cb_arg(175)      => wff(32).
	p(NT(175), (NT(32)));
//G218: wff_cb_args1(174)    => __(72) wff_cb_arg(175).
	p(NT(174), (NT(72)+NT(175)));
//G219: wff_cb_args2(176)    => wff_cb_args1(174) wff_cb_args1(174).
	p(NT(176), (NT(174)+NT(174)));
//G220: wff_cb_args3(177)    => wff_cb_args2(176) wff_cb_args1(174).
	p(NT(177), (NT(176)+NT(174)));
//G221: wff_cb_args4(178)    => wff_cb_args3(177) wff_cb_args1(174).
	p(NT(178), (NT(177)+NT(174)));
//G222: __E_tau_cb_args1_66(181) => capture(70).
	p(NT(181), (NT(70)));
//G223: __E_tau_cb_args1_66(181) => tau(20).
	p(NT(181), (NT(20)));
//G224: tau_cb_arg(180)      => __E_tau_cb_args1_66(181).
	p(NT(180), (NT(181)));
//G225: tau_cb_args1(179)    => __(72) tau_cb_arg(180).
	p(NT(179), (NT(72)+NT(180)));
//G226: tau_cb_args2(182)    => tau_cb_args1(179) tau_cb_args1(179).
	p(NT(182), (NT(179)+NT(179)));
//G227: tau_cb_args3(183)    => tau_cb_args2(182) tau_cb_args1(179).
	p(NT(183), (NT(182)+NT(179)));
//G228: __E___E_tau_cb_67_68(186) => tau_cb_args3(183).
	p(NT(186), (NT(183)));
//G229: __E___E_tau_cb_67_68(186) => tau_cb_args2(182).
	p(NT(186), (NT(182)));
//G230: __E_tau_cb_67(185)   => 't' 'a' 'u' '_' 'c' 'o' 'l' 'l' 'a' 'p' 's' 'e' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'c' 'b' __E___E_tau_cb_67_68(186).
	p(NT(185), (T(32)+T(18)+T(33)+T(17)+T(34)+T(15)+T(16)+T(16)+T(18)+T(35)+T(36)+T(19)+T(17)+T(35)+T(15)+T(36)+T(37)+T(32)+T(37)+T(38)+T(19)+T(36)+T(17)+T(34)+T(14)+NT(186)));
//G231: tau_collapse_positives_cb(184) => __E_tau_cb_67(185).
	p(NT(184), (NT(185)));
//G232: tau_cb(59)           => tau_collapse_positives_cb(184).
	p(NT(59), (NT(184)));
//G233: __E_tau_cb_69(188)   => 't' 'a' 'u' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'u' 'p' 'w' 'a' 'r' 'd' 's' '_' 'c' 'b' tau_cb_args2(182).
	p(NT(188), (T(32)+T(18)+T(33)+T(17)+T(35)+T(15)+T(36)+T(37)+T(32)+T(37)+T(38)+T(19)+T(36)+T(17)+T(33)+T(35)+T(39)+T(18)+T(40)+T(41)+T(36)+T(17)+T(34)+T(14)+NT(182)));
//G234: tau_positives_upwards_cb(187) => __E_tau_cb_69(188).
	p(NT(187), (NT(188)));
//G235: tau_cb(59)           => tau_positives_upwards_cb(187).
	p(NT(59), (NT(187)));
//G236: __E_wff_cb_70(190)   => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b' bf_cb_args1(169) wff_cb_args2(176).
	p(NT(190), (T(14)+T(42)+T(17)+T(19)+T(43)+T(17)+T(34)+T(14)+NT(169)+NT(176)));
//G237: bf_eq_cb(189)        => __E_wff_cb_70(190).
	p(NT(189), (NT(190)));
//G238: wff_cb(63)           => bf_eq_cb(189).
	p(NT(63), (NT(189)));
//G239: __E_wff_cb_71(192)   => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b' bf_cb_args1(169) wff_cb_args2(176).
	p(NT(192), (T(14)+T(42)+T(17)+T(44)+T(19)+T(43)+T(17)+T(34)+T(14)+NT(169)+NT(176)));
//G240: bf_neq_cb(191)       => __E_wff_cb_71(192).
	p(NT(191), (NT(192)));
//G241: wff_cb(63)           => bf_neq_cb(191).
	p(NT(63), (NT(191)));
//G242: __E_wff_cb_72(194)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(176).
	p(NT(194), (T(39)+T(42)+T(42)+T(17)+T(45)+T(18)+T(36)+T(17)+T(34)+T(16)+T(18)+T(36)+T(45)+T(37)+T(44)+T(46)+T(17)+T(36)+T(33)+T(14)+T(42)+T(15)+T(40)+T(47)+T(33)+T(16)+T(18)+T(36)+T(17)+T(34)+T(14)+NT(176)));
//G243: wff_has_clashing_subformulas_cb(193) => __E_wff_cb_72(194).
	p(NT(193), (NT(194)));
//G244: wff_cb(63)           => wff_has_clashing_subformulas_cb(193).
	p(NT(63), (NT(193)));
//G245: __E_wff_cb_73(196)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(177).
	p(NT(196), (T(39)+T(42)+T(42)+T(17)+T(45)+T(18)+T(36)+T(17)+T(36)+T(33)+T(14)+T(42)+T(15)+T(40)+T(47)+T(33)+T(16)+T(18)+T(17)+T(34)+T(14)+NT(177)));
//G246: wff_has_subformula_cb(195) => __E_wff_cb_73(196).
	p(NT(195), (NT(196)));
//G247: wff_cb(63)           => wff_has_subformula_cb(195).
	p(NT(63), (NT(195)));
//G248: __E_wff_cb_74(198)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(176).
	p(NT(198), (T(39)+T(42)+T(42)+T(17)+T(40)+T(19)+T(47)+T(15)+T(38)+T(19)+T(17)+T(19)+T(20)+T(37)+T(36)+T(32)+T(19)+T(44)+T(32)+T(37)+T(18)+T(16)+T(17)+T(34)+T(14)+NT(176)));
//G249: wff_remove_existential_cb(197) => __E_wff_cb_74(198).
	p(NT(197), (NT(198)));
//G250: wff_cb(63)           => wff_remove_existential_cb(197).
	p(NT(63), (NT(197)));
//G251: __E_wff_cb_75(200)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args4(178).
	p(NT(200), (T(39)+T(42)+T(42)+T(17)+T(40)+T(19)+T(47)+T(15)+T(38)+T(19)+T(17)+T(14)+T(19)+T(20)+T(37)+T(36)+T(32)+T(19)+T(44)+T(32)+T(37)+T(18)+T(16)+T(17)+T(34)+T(14)+NT(178)));
//G252: wff_remove_bexistential_cb(199) => __E_wff_cb_75(200).
	p(NT(199), (NT(200)));
//G253: wff_cb(63)           => wff_remove_bexistential_cb(199).
	p(NT(63), (NT(199)));
//G254: __E_wff_cb_76(202)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' wff_cb_args4(178).
	p(NT(202), (T(39)+T(42)+T(42)+T(17)+T(40)+T(19)+T(47)+T(15)+T(38)+T(19)+T(17)+T(14)+T(33)+T(44)+T(37)+T(38)+T(19)+T(40)+T(36)+T(18)+T(16)+T(17)+T(34)+T(14)+NT(178)));
//G255: wff_remove_buniversal_cb(201) => __E_wff_cb_76(202).
	p(NT(201), (NT(202)));
//G256: wff_cb(63)           => wff_remove_buniversal_cb(201).
	p(NT(63), (NT(201)));
//G257: __E_bf_cb_77(204)    => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b' bf_cb_args2(171).
	p(NT(204), (T(14)+T(42)+T(17)+T(37)+T(36)+T(17)+T(48)+T(19)+T(40)+T(15)+T(17)+T(34)+T(14)+NT(171)));
//G258: bf_is_zero_cb(203)   => __E_bf_cb_77(204).
	p(NT(203), (NT(204)));
//G259: bf_cb(67)            => bf_is_zero_cb(203).
	p(NT(67), (NT(203)));
//G260: __E_bf_cb_78(206)    => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b' bf_cb_args2(171).
	p(NT(206), (T(14)+T(42)+T(17)+T(37)+T(36)+T(17)+T(15)+T(44)+T(19)+T(17)+T(34)+T(14)+NT(171)));
//G261: bf_is_one_cb(205)    => __E_bf_cb_78(206).
	p(NT(205), (NT(206)));
//G262: bf_cb(67)            => bf_is_one_cb(205).
	p(NT(67), (NT(205)));
//G263: __E_bf_cb_79(208)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(172).
	p(NT(208), (T(14)+T(42)+T(17)+T(45)+T(18)+T(36)+T(17)+T(36)+T(33)+T(14)+T(42)+T(15)+T(40)+T(47)+T(33)+T(16)+T(18)+T(17)+T(34)+T(14)+NT(172)));
//G264: bf_has_subformula_cb(207) => __E_bf_cb_79(208).
	p(NT(207), (NT(208)));
//G265: bf_cb(67)            => bf_has_subformula_cb(207).
	p(NT(67), (NT(207)));
//G266: __E_bf_cb_80(210)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(173).
	p(NT(210), (T(14)+T(42)+T(17)+T(40)+T(19)+T(47)+T(15)+T(38)+T(19)+T(17)+T(42)+T(33)+T(44)+T(37)+T(38)+T(19)+T(40)+T(36)+T(18)+T(16)+T(17)+T(34)+T(14)+NT(173)));
//G267: bf_remove_funiversal_cb(209) => __E_bf_cb_80(210).
	p(NT(209), (NT(210)));
//G268: bf_cb(67)            => bf_remove_funiversal_cb(209).
	p(NT(67), (NT(209)));
//G269: __E_bf_cb_81(212)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(173).
	p(NT(212), (T(14)+T(42)+T(17)+T(40)+T(19)+T(47)+T(15)+T(38)+T(19)+T(17)+T(42)+T(19)+T(20)+T(37)+T(36)+T(32)+T(19)+T(44)+T(32)+T(37)+T(18)+T(16)+T(17)+T(34)+T(14)+NT(173)));
//G270: bf_remove_fexistential_cb(211) => __E_bf_cb_81(212).
	p(NT(211), (NT(212)));
//G271: bf_cb(67)            => bf_remove_fexistential_cb(211).
	p(NT(67), (NT(211)));
//G272: __E_bf_cb_82(214)    => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b' bf_cb_args2(171).
	p(NT(214), (T(14)+T(42)+T(17)+T(18)+T(44)+T(41)+T(17)+T(34)+T(14)+NT(171)));
//G273: bf_and_cb(213)       => __E_bf_cb_82(214).
	p(NT(213), (NT(214)));
//G274: bf_cb(67)            => bf_and_cb(213).
	p(NT(67), (NT(213)));
//G275: __E_bf_cb_83(216)    => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b' bf_cb_args2(171).
	p(NT(216), (T(14)+T(42)+T(17)+T(15)+T(40)+T(17)+T(34)+T(14)+NT(171)));
//G276: bf_or_cb(215)        => __E_bf_cb_83(216).
	p(NT(215), (NT(216)));
//G277: bf_cb(67)            => bf_or_cb(215).
	p(NT(67), (NT(215)));
//G278: __E_bf_cb_84(218)    => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b' bf_cb_args2(171).
	p(NT(218), (T(14)+T(42)+T(17)+T(20)+T(15)+T(40)+T(17)+T(34)+T(14)+NT(171)));
//G279: bf_xor_cb(217)       => __E_bf_cb_84(218).
	p(NT(217), (NT(218)));
//G280: bf_cb(67)            => bf_xor_cb(217).
	p(NT(67), (NT(217)));
//G281: __E_bf_cb_85(220)    => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b' bf_cb_args1(169).
	p(NT(220), (T(14)+T(42)+T(17)+T(44)+T(19)+T(46)+T(17)+T(34)+T(14)+NT(169)));
//G282: bf_neg_cb(219)       => __E_bf_cb_85(220).
	p(NT(219), (NT(220)));
//G283: bf_cb(67)            => bf_neg_cb(219).
	p(NT(67), (NT(219)));
//G284: __N_0(436)           => 'F'.
	p(NT(436), (T(25)));
//G285: __N_1(437)           => 'T'.
	p(NT(437), (T(24)));
//G286: __E_charvar_86(222)  => ~( __N_0(436) ) & ~( __N_1(437) ) & alpha(5).	 # conjunctive
	p(NT(222), ~(NT(436)) & ~(NT(437)) & (NT(5)));
//G287: __E_charvar_87(223)  => null.
	p(NT(223), (nul));
//G288: __E_charvar_87(223)  => digit(3) __E_charvar_87(223).
	p(NT(223), (NT(3)+NT(223)));
//G289: charvar(221)         => __E_charvar_86(222) __E_charvar_87(223).
	p(NT(221), (NT(222)+NT(223)));
//G290: bool_variable(141)   => '?' charvar(221).
	p(NT(141), (T(13)+NT(221)));
//G291: variable(163)        => charvar(221).
	p(NT(163), (NT(221)));
//G292: __E_variable_88(225) => in(45).
	p(NT(225), (NT(45)));
//G293: __E_variable_88(225) => out(226).
	p(NT(225), (NT(226)));
//G294: io_var(224)          => __E_variable_88(225).
	p(NT(224), (NT(225)));
//G295: variable(163)        => io_var(224).
	p(NT(163), (NT(224)));
//G296: __E_in_89(228)       => 'i' '_' chars(229).
	p(NT(228), (T(37)+T(17)+NT(229)));
//G297: in_var_name(227)     => __E_in_89(228).
	p(NT(227), (NT(228)));
//G298: in(45)               => in_var_name(227) '[' offset(230) ']'.
	p(NT(45), (NT(227)+T(49)+NT(230)+T(50)));
//G299: __E_out_90(232)      => 'o' '_' chars(229).
	p(NT(232), (T(15)+T(17)+NT(229)));
//G300: out_var_name(231)    => __E_out_90(232).
	p(NT(231), (NT(232)));
//G301: out(226)             => out_var_name(231) '[' offset(230) ']'.
	p(NT(226), (NT(231)+T(49)+NT(230)+T(50)));
//G302: capture(70)          => '$' charvar(221).
	p(NT(70), (T(51)+NT(221)));
//G303: q_bool_var(101)      => capture(70).
	p(NT(101), (NT(70)));
//G304: q_bool_var(101)      => bool_variable(141).
	p(NT(101), (NT(141)));
//G305: q_var(106)           => capture(70).
	p(NT(106), (NT(70)));
//G306: q_var(106)           => variable(163).
	p(NT(106), (NT(163)));
//G307: __E_offsets_91(233)  => _(16) ',' _(16) offset(230).
	p(NT(233), (NT(16)+T(31)+NT(16)+NT(230)));
//G308: __E_offsets_92(234)  => null.
	p(NT(234), (nul));
//G309: __E_offsets_92(234)  => __E_offsets_91(233) __E_offsets_92(234).
	p(NT(234), (NT(233)+NT(234)));
//G310: offsets(87)          => '[' _(16) offset(230) __E_offsets_92(234) _(16) ']'.
	p(NT(87), (T(49)+NT(16)+NT(230)+NT(234)+NT(16)+T(50)));
//G311: offset(230)          => num(235).
	p(NT(230), (NT(235)));
//G312: offset(230)          => capture(70).
	p(NT(230), (NT(70)));
//G313: offset(230)          => shift(236).
	p(NT(230), (NT(236)));
//G314: __N_2(438)           => io_var(224).
	p(NT(438), (NT(224)));
//G315: __E_offset_93(237)   => variable(163) & ~( __N_2(438) ).	 # conjunctive
	p(NT(237), (NT(163)) & ~(NT(438)));
//G316: offset(230)          => __E_offset_93(237).
	p(NT(230), (NT(237)));
//G317: __E_shift_94(238)    => capture(70).
	p(NT(238), (NT(70)));
//G318: __N_3(439)           => io_var(224).
	p(NT(439), (NT(224)));
//G319: __E___E_shift_94_95(239) => variable(163) & ~( __N_3(439) ).	 # conjunctive
	p(NT(239), (NT(163)) & ~(NT(439)));
//G320: __E_shift_94(238)    => __E___E_shift_94_95(239).
	p(NT(238), (NT(239)));
//G321: shift(236)           => __E_shift_94(238) _(16) '-' _(16) num(235).
	p(NT(236), (NT(238)+NT(16)+T(21)+NT(16)+NT(235)));
//G322: num(235)             => digits(240).
	p(NT(235), (NT(240)));
//G323: __E___E_uninterpreted_constant_96_97(243) => null.
	p(NT(243), (nul));
//G324: __E___E_uninterpreted_constant_96_97(243) => chars(229) __E___E_uninterpreted_constant_96_97(243).
	p(NT(243), (NT(229)+NT(243)));
//G325: __E_uninterpreted_constant_96(242) => __E___E_uninterpreted_constant_96_97(243) ':' chars(229).
	p(NT(242), (NT(243)+T(3)+NT(229)));
//G326: uninter_const_name(241) => __E_uninterpreted_constant_96(242).
	p(NT(241), (NT(242)));
//G327: uninterpreted_constant(142) => '<' _(16) uninter_const_name(241) _(16) '>'.
	p(NT(142), (T(5)+NT(16)+NT(241)+NT(16)+T(22)));
//G328: constant(160)        => binding(244).
	p(NT(160), (NT(244)));
//G329: constant(160)        => capture(70).
	p(NT(160), (NT(70)));
//G330: __E___E_binding_98_99(247) => chars(229) _(16).
	p(NT(247), (NT(229)+NT(16)));
//G331: __E___E_binding_98_99(247) => _(16).
	p(NT(247), (NT(16)));
//G332: type(246)            => __E___E_binding_98_99(247).
	p(NT(246), (NT(247)));
//G333: __E_binding_98(245)  => type(246) ':' _(16) source(248).
	p(NT(245), (NT(246)+T(3)+NT(16)+NT(248)));
//G334: source_binding(46)   => __E_binding_98(245).
	p(NT(46), (NT(245)));
//G335: binding(244)         => source_binding(46).
	p(NT(244), (NT(46)));
//G336: named_binding(249)   => chars(229).
	p(NT(249), (NT(229)));
//G337: binding(244)         => named_binding(249).
	p(NT(244), (NT(249)));
//G338: __E_source_100(251)  => alnum(6).
	p(NT(251), (NT(6)));
//G339: __E_source_100(251)  => punct(7).
	p(NT(251), (NT(7)));
//G340: src_c(250)           => __E_source_100(251).
	p(NT(250), (NT(251)));
//G341: __E___E_source_101_102(253) => src_c(250).
	p(NT(253), (NT(250)));
//G342: __E___E_source_101_102(253) => space(2).
	p(NT(253), (NT(2)));
//G343: __E___E_source_101_103(254) => null.
	p(NT(254), (nul));
//G344: __E___E_source_101_103(254) => __E___E_source_101_102(253) __E___E_source_101_103(254).
	p(NT(254), (NT(253)+NT(254)));
//G345: __E_source_101(252)  => __E___E_source_101_103(254) src_c(250).
	p(NT(252), (NT(254)+NT(250)));
//G346: __E_source_101(252)  => null.
	p(NT(252), (nul));
//G347: source(248)          => src_c(250) __E_source_101(252).
	p(NT(248), (NT(250)+NT(252)));
//G348: __E_chars_104(255)   => null.
	p(NT(255), (nul));
//G349: __E_chars_104(255)   => alnum(6) __E_chars_104(255).
	p(NT(255), (NT(6)+NT(255)));
//G350: chars(229)           => alpha(5) __E_chars_104(255).
	p(NT(229), (NT(5)+NT(255)));
//G351: __E_digits_105(256)  => digit(3).
	p(NT(256), (NT(3)));
//G352: __E_digits_105(256)  => digit(3) __E_digits_105(256).
	p(NT(256), (NT(3)+NT(256)));
//G353: digits(240)          => __E_digits_105(256).
	p(NT(240), (NT(256)));
//G354: sym(85)              => chars(229).
	p(NT(85), (NT(229)));
//G355: __E_comment_106(258) => printable(8).
	p(NT(258), (NT(8)));
//G356: __E_comment_106(258) => '\t'.
	p(NT(258), (T(52)));
//G357: __E_comment_107(259) => null.
	p(NT(259), (nul));
//G358: __E_comment_107(259) => __E_comment_106(258) __E_comment_107(259).
	p(NT(259), (NT(258)+NT(259)));
//G359: __E_comment_108(260) => '\n'.
	p(NT(260), (T(53)));
//G360: __E_comment_108(260) => '\r'.
	p(NT(260), (T(54)));
//G361: __E_comment_108(260) => eof(1).
	p(NT(260), (NT(1)));
//G362: comment(257)         => '#' __E_comment_107(259) __E_comment_108(260).
	p(NT(257), (T(55)+NT(259)+NT(260)));
//G363: __E____109(261)      => space(2).
	p(NT(261), (NT(2)));
//G364: __E____109(261)      => comment(257).
	p(NT(261), (NT(257)));
//G365: __(72)               => __E____109(261) _(16).
	p(NT(72), (NT(261)+NT(16)));
//G366: __E___110(262)       => __(72).
	p(NT(262), (NT(72)));
//G367: __E___110(262)       => null.
	p(NT(262), (nul));
//G368: _(16)                => __E___110(262).
	p(NT(16), (NT(262)));
//G369: __E_cli_111(265)     => '.' _(16) cli_command(264) _(16).
	p(NT(265), (T(2)+NT(16)+NT(264)+NT(16)));
//G370: __E_cli_112(266)     => null.
	p(NT(266), (nul));
//G371: __E_cli_112(266)     => __E_cli_111(265) __E_cli_112(266).
	p(NT(266), (NT(265)+NT(266)));
//G372: cli(263)             => _(16) cli_command(264) _(16) __E_cli_112(266).
	p(NT(263), (NT(16)+NT(264)+NT(16)+NT(266)));
//G373: __E_cli_command_113(269) => 'q'.
	p(NT(269), (T(43)));
//G374: __E_cli_command_113(269) => 'q' 'u' 'i' 't'.
	p(NT(269), (T(43)+T(33)+T(37)+T(32)));
//G375: quit_sym(268)        => __E_cli_command_113(269).
	p(NT(268), (NT(269)));
//G376: quit_cmd(267)        => quit_sym(268).
	p(NT(267), (NT(268)));
//G377: cli_command(264)     => quit_cmd(267).
	p(NT(264), (NT(267)));
//G378: __E_cli_command_114(272) => 'v'.
	p(NT(272), (T(38)));
//G379: __E_cli_command_114(272) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(272), (T(38)+T(19)+T(40)+T(36)+T(37)+T(15)+T(44)));
//G380: version_sym(271)     => __E_cli_command_114(272).
	p(NT(271), (NT(272)));
//G381: version_cmd(270)     => version_sym(271).
	p(NT(270), (NT(271)));
//G382: cli_command(264)     => version_cmd(270).
	p(NT(264), (NT(270)));
//G383: __E_cli_command_115(275) => 'c'.
	p(NT(275), (T(34)));
//G384: __E_cli_command_115(275) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(275), (T(34)+T(16)+T(19)+T(18)+T(40)));
//G385: clear_sym(274)       => __E_cli_command_115(275).
	p(NT(274), (NT(275)));
//G386: clear_cmd(273)       => clear_sym(274).
	p(NT(273), (NT(274)));
//G387: cli_command(264)     => clear_cmd(273).
	p(NT(264), (NT(273)));
//G388: __E___E_cli_command_116_117(279) => 'h'.
	p(NT(279), (T(45)));
//G389: __E___E_cli_command_116_117(279) => 'h' 'e' 'l' 'p'.
	p(NT(279), (T(45)+T(19)+T(16)+T(35)));
//G390: help_sym(278)        => __E___E_cli_command_116_117(279).
	p(NT(278), (NT(279)));
//G391: __E___E_cli_command_116_118(280) => __(72) help_arg(281).
	p(NT(280), (NT(72)+NT(281)));
//G392: __E___E_cli_command_116_118(280) => null.
	p(NT(280), (nul));
//G393: __E_cli_command_116(277) => help_sym(278) __E___E_cli_command_116_118(280).
	p(NT(277), (NT(278)+NT(280)));
//G394: help_cmd(276)        => __E_cli_command_116(277).
	p(NT(276), (NT(277)));
//G395: cli_command(264)     => help_cmd(276).
	p(NT(264), (NT(276)));
//G396: file_sym(284)        => 'f' 'i' 'l' 'e'.
	p(NT(284), (T(42)+T(37)+T(16)+T(19)));
//G397: __E_cli_command_119(283) => file_sym(284) __(72) q_string(285).
	p(NT(283), (NT(284)+NT(72)+NT(285)));
//G398: file_cmd(282)        => __E_cli_command_119(283).
	p(NT(282), (NT(283)));
//G399: cli_command(264)     => file_cmd(282).
	p(NT(264), (NT(282)));
//G400: valid_sym(288)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(288), (T(38)+T(18)+T(16)+T(37)+T(41)));
//G401: __E_cli_command_120(287) => valid_sym(288) __(72) gssotc_rr(15).
	p(NT(287), (NT(288)+NT(72)+NT(15)));
//G402: valid_cmd(286)       => __E_cli_command_120(287).
	p(NT(286), (NT(287)));
//G403: cli_command(264)     => valid_cmd(286).
	p(NT(264), (NT(286)));
//G404: sat_sym(291)         => 's' 'a' 't'.
	p(NT(291), (T(36)+T(18)+T(32)));
//G405: __E_cli_command_121(290) => sat_sym(291) __(72) gssotc_rr(15).
	p(NT(290), (NT(291)+NT(72)+NT(15)));
//G406: sat_cmd(289)         => __E_cli_command_121(290).
	p(NT(289), (NT(290)));
//G407: cli_command(264)     => sat_cmd(289).
	p(NT(264), (NT(289)));
//G408: unsat_sym(294)       => 'u' 'n' 's' 'a' 't'.
	p(NT(294), (T(33)+T(44)+T(36)+T(18)+T(32)));
//G409: __E_cli_command_122(293) => unsat_sym(294) __(72) gssotc_rr(15).
	p(NT(293), (NT(294)+NT(72)+NT(15)));
//G410: unsat_cmd(292)       => __E_cli_command_122(293).
	p(NT(292), (NT(293)));
//G411: cli_command(264)     => unsat_cmd(292).
	p(NT(264), (NT(292)));
//G412: solve_sym(297)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(297), (T(36)+T(15)+T(16)+T(38)+T(19)));
//G413: solve_cmd_arg(298)   => wff_cmd_arg(299).
	p(NT(298), (NT(299)));
//G414: __E_cli_command_123(296) => solve_sym(297) __(72) solve_cmd_arg(298).
	p(NT(296), (NT(297)+NT(72)+NT(298)));
//G415: solve_cmd(295)       => __E_cli_command_123(296).
	p(NT(295), (NT(296)));
//G416: cli_command(264)     => solve_cmd(295).
	p(NT(264), (NT(295)));
//G417: __E___E_cli_command_124_125(303) => 'e'.
	p(NT(303), (T(19)));
//G418: __E___E_cli_command_124_125(303) => 'e' 'x' 'e' 'c' 'u' 't' 'e'.
	p(NT(303), (T(19)+T(20)+T(19)+T(34)+T(33)+T(32)+T(19)));
//G419: execute_sym(302)     => __E___E_cli_command_124_125(303).
	p(NT(302), (NT(303)));
//G420: __E___E_cli_command_124_126(305) => tau(20).
	p(NT(305), (NT(20)));
//G421: __E___E_cli_command_124_126(305) => gssotc_rr(15).
	p(NT(305), (NT(15)));
//G422: execute_cmd_arg(304) => __E___E_cli_command_124_126(305).
	p(NT(304), (NT(305)));
//G423: __E_cli_command_124(301) => execute_sym(302) __(72) execute_cmd_arg(304).
	p(NT(301), (NT(302)+NT(72)+NT(304)));
//G424: execute_cmd(300)     => __E_cli_command_124(301).
	p(NT(300), (NT(301)));
//G425: cli_command(264)     => execute_cmd(300).
	p(NT(264), (NT(300)));
//G426: __E___E_cli_command_127_128(309) => 'n'.
	p(NT(309), (T(44)));
//G427: __E___E_cli_command_127_128(309) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(309), (T(44)+T(15)+T(40)+T(47)+T(18)+T(16)+T(37)+T(48)+T(19)));
//G428: normalize_sym(308)   => __E___E_cli_command_127_128(309).
	p(NT(308), (NT(309)));
//G429: __E_cli_command_127(307) => normalize_sym(308) __(72) normalize_cmd_arg(310).
	p(NT(307), (NT(308)+NT(72)+NT(310)));
//G430: normalize_cmd(306)   => __E_cli_command_127(307).
	p(NT(306), (NT(307)));
//G431: cli_command(264)     => normalize_cmd(306).
	p(NT(264), (NT(306)));
//G432: __E___E_cli_command_129_130(314) => 'i'.
	p(NT(314), (T(37)));
//G433: __E___E_cli_command_129_130(314) => 'i' 'n' 's' 't'.
	p(NT(314), (T(37)+T(44)+T(36)+T(32)));
//G434: __E___E_cli_command_129_130(314) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(314), (T(37)+T(44)+T(36)+T(32)+T(18)+T(44)+T(32)+T(37)+T(18)+T(32)+T(19)));
//G435: inst_sym(313)        => __E___E_cli_command_129_130(314).
	p(NT(313), (NT(314)));
//G436: __E_cli_command_129(312) => inst_sym(313) __(72) variable(163) __(72) wff_cmd_arg(299) __(72) wff_cmd_out_arg(315).
	p(NT(312), (NT(313)+NT(72)+NT(163)+NT(72)+NT(299)+NT(72)+NT(315)));
//G437: wff_inst_cmd(311)    => __E_cli_command_129(312).
	p(NT(311), (NT(312)));
//G438: cli_command(264)     => wff_inst_cmd(311).
	p(NT(264), (NT(311)));
//G439: __E___E_cli_command_131_132(319) => 's'.
	p(NT(319), (T(36)));
//G440: __E___E_cli_command_131_132(319) => 's' 'u' 'b' 's' 't'.
	p(NT(319), (T(36)+T(33)+T(14)+T(36)+T(32)));
//G441: __E___E_cli_command_131_132(319) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(319), (T(36)+T(33)+T(14)+T(36)+T(32)+T(37)+T(32)+T(33)+T(32)+T(19)));
//G442: subst_sym(318)       => __E___E_cli_command_131_132(319).
	p(NT(318), (NT(319)));
//G443: __E_cli_command_131(317) => subst_sym(318) __(72) wff_cmd_arg(299) __(72) wff_cmd_arg(299) __(72) wff_cmd_out_arg(315).
	p(NT(317), (NT(318)+NT(72)+NT(299)+NT(72)+NT(299)+NT(72)+NT(315)));
//G444: wff_subst_cmd(316)   => __E_cli_command_131(317).
	p(NT(316), (NT(317)));
//G445: cli_command(264)     => wff_subst_cmd(316).
	p(NT(264), (NT(316)));
//G446: __E_cli_command_133(321) => inst_sym(313) __(72) variable(163) __(72) bf_cmd_arg(322) __(72) bf_cmd_out_arg(323).
	p(NT(321), (NT(313)+NT(72)+NT(163)+NT(72)+NT(322)+NT(72)+NT(323)));
//G447: bf_inst_cmd(320)     => __E_cli_command_133(321).
	p(NT(320), (NT(321)));
//G448: cli_command(264)     => bf_inst_cmd(320).
	p(NT(264), (NT(320)));
//G449: __E_cli_command_134(325) => subst_sym(318) __(72) bf_cmd_arg(322) __(72) bf_cmd_arg(322) __(72) bf_cmd_out_arg(323).
	p(NT(325), (NT(318)+NT(72)+NT(322)+NT(72)+NT(322)+NT(72)+NT(323)));
//G450: bf_subst_cmd(324)    => __E_cli_command_134(325).
	p(NT(324), (NT(325)));
//G451: cli_command(264)     => bf_subst_cmd(324).
	p(NT(264), (NT(324)));
//G452: dnf_sym(328)         => 'd' 'n' 'f'.
	p(NT(328), (T(41)+T(44)+T(42)));
//G453: __E_cli_command_135(327) => dnf_sym(328) __(72) nf_cmd_arg(329).
	p(NT(327), (NT(328)+NT(72)+NT(329)));
//G454: dnf_cmd(326)         => __E_cli_command_135(327).
	p(NT(326), (NT(327)));
//G455: cli_command(264)     => dnf_cmd(326).
	p(NT(264), (NT(326)));
//G456: cnf_sym(332)         => 'c' 'n' 'f'.
	p(NT(332), (T(34)+T(44)+T(42)));
//G457: __E_cli_command_136(331) => cnf_sym(332) __(72) nf_cmd_arg(329).
	p(NT(331), (NT(332)+NT(72)+NT(329)));
//G458: cnf_cmd(330)         => __E_cli_command_136(331).
	p(NT(330), (NT(331)));
//G459: cli_command(264)     => cnf_cmd(330).
	p(NT(264), (NT(330)));
//G460: anf_sym(335)         => 'a' 'n' 'f'.
	p(NT(335), (T(18)+T(44)+T(42)));
//G461: __E_cli_command_137(334) => anf_sym(335) __(72) nf_cmd_arg(329).
	p(NT(334), (NT(335)+NT(72)+NT(329)));
//G462: anf_cmd(333)         => __E_cli_command_137(334).
	p(NT(333), (NT(334)));
//G463: cli_command(264)     => anf_cmd(333).
	p(NT(264), (NT(333)));
//G464: nnf_sym(338)         => 'n' 'n' 'f'.
	p(NT(338), (T(44)+T(44)+T(42)));
//G465: __E_cli_command_138(337) => nnf_sym(338) __(72) nf_cmd_arg(329).
	p(NT(337), (NT(338)+NT(72)+NT(329)));
//G466: nnf_cmd(336)         => __E_cli_command_138(337).
	p(NT(336), (NT(337)));
//G467: cli_command(264)     => nnf_cmd(336).
	p(NT(264), (NT(336)));
//G468: pnf_sym(341)         => 'p' 'n' 'f'.
	p(NT(341), (T(35)+T(44)+T(42)));
//G469: __E_cli_command_139(340) => pnf_sym(341) __(72) nf_cmd_arg(329).
	p(NT(340), (NT(341)+NT(72)+NT(329)));
//G470: pnf_cmd(339)         => __E_cli_command_139(340).
	p(NT(339), (NT(340)));
//G471: cli_command(264)     => pnf_cmd(339).
	p(NT(264), (NT(339)));
//G472: mnf_sym(344)         => 'm' 'n' 'f'.
	p(NT(344), (T(47)+T(44)+T(42)));
//G473: __E_cli_command_140(343) => mnf_sym(344) __(72) nf_cmd_arg(329).
	p(NT(343), (NT(344)+NT(72)+NT(329)));
//G474: mnf_cmd(342)         => __E_cli_command_140(343).
	p(NT(342), (NT(343)));
//G475: cli_command(264)     => mnf_cmd(342).
	p(NT(264), (NT(342)));
//G476: snf_sym(347)         => 's' 'n' 'f'.
	p(NT(347), (T(36)+T(44)+T(42)));
//G477: __E_cli_command_141(346) => snf_sym(347) __(72) nf_cmd_arg(329).
	p(NT(346), (NT(347)+NT(72)+NT(329)));
//G478: snf_cmd(345)         => __E_cli_command_141(346).
	p(NT(345), (NT(346)));
//G479: cli_command(264)     => snf_cmd(345).
	p(NT(264), (NT(345)));
//G480: onf_sym(350)         => 'o' 'n' 'f'.
	p(NT(350), (T(15)+T(44)+T(42)));
//G481: __E_cli_command_142(349) => onf_sym(350) __(72) variable(163) __(72) onf_cmd_arg(351).
	p(NT(349), (NT(350)+NT(72)+NT(163)+NT(72)+NT(351)));
//G482: onf_cmd(348)         => __E_cli_command_142(349).
	p(NT(348), (NT(349)));
//G483: cli_command(264)     => onf_cmd(348).
	p(NT(264), (NT(348)));
//G484: def_sym(354)         => 'd' 'e' 'f'.
	p(NT(354), (T(41)+T(19)+T(42)));
//G485: __E_cli_command_143(353) => def_sym(354).
	p(NT(353), (NT(354)));
//G486: def_list_cmd(352)    => __E_cli_command_143(353).
	p(NT(352), (NT(353)));
//G487: cli_command(264)     => def_list_cmd(352).
	p(NT(264), (NT(352)));
//G488: __E_cli_command_144(356) => def_sym(354) __(72) gssotc_rec_relation_form(24).
	p(NT(356), (NT(354)+NT(72)+NT(24)));
//G489: def_rr_cmd(355)      => __E_cli_command_144(356).
	p(NT(355), (NT(356)));
//G490: cli_command(264)     => def_rr_cmd(355).
	p(NT(264), (NT(355)));
//G491: __E___E_cli_command_145_146(360) => 'd'.
	p(NT(360), (T(41)));
//G492: __E___E_cli_command_145_146(360) => 'd' 'e' 'l'.
	p(NT(360), (T(41)+T(19)+T(16)));
//G493: __E___E_cli_command_145_146(360) => 'd' 'e' 'l' 'e' 't' 'e'.
	p(NT(360), (T(41)+T(19)+T(16)+T(19)+T(32)+T(19)));
//G494: del_sym(359)         => __E___E_cli_command_145_146(360).
	p(NT(359), (NT(360)));
//G495: __E_cli_command_145(358) => def_sym(354) __(72) del_sym(359) __(72) memory(361).
	p(NT(358), (NT(354)+NT(72)+NT(359)+NT(72)+NT(361)));
//G496: def_del_cmd(357)     => __E_cli_command_145(358).
	p(NT(357), (NT(358)));
//G497: cli_command(264)     => def_del_cmd(357).
	p(NT(264), (NT(357)));
//G498: __E_cli_command_147(363) => def_sym(354) __(72) clear_sym(274).
	p(NT(363), (NT(354)+NT(72)+NT(274)));
//G499: def_clear_cmd(362)   => __E_cli_command_147(363).
	p(NT(362), (NT(363)));
//G500: cli_command(264)     => def_clear_cmd(362).
	p(NT(264), (NT(362)));
//G501: __E_cli_command_148(365) => def_sym(354) __(72) memory(361).
	p(NT(365), (NT(354)+NT(72)+NT(361)));
//G502: def_print_cmd(364)   => __E_cli_command_148(365).
	p(NT(364), (NT(365)));
//G503: cli_command(264)     => def_print_cmd(364).
	p(NT(264), (NT(364)));
//G504: __E___E_cli_command_149_150(369) => 'm'.
	p(NT(369), (T(47)));
//G505: __E___E_cli_command_149_150(369) => 'm' 'e' 'm'.
	p(NT(369), (T(47)+T(19)+T(47)));
//G506: __E___E_cli_command_149_150(369) => 'm' 'e' 'm' 'o' 'r' 'y'.
	p(NT(369), (T(47)+T(19)+T(47)+T(15)+T(40)+T(56)));
//G507: memory_sym(368)      => __E___E_cli_command_149_150(369).
	p(NT(368), (NT(369)));
//G508: __E_cli_command_149(367) => memory_sym(368).
	p(NT(367), (NT(368)));
//G509: memory_list_cmd(366) => __E_cli_command_149(367).
	p(NT(366), (NT(367)));
//G510: cli_command(264)     => memory_list_cmd(366).
	p(NT(264), (NT(366)));
//G511: __E_cli_command_151(371) => memory_sym(368) __(72) del_sym(359) __(72) memory(361).
	p(NT(371), (NT(368)+NT(72)+NT(359)+NT(72)+NT(361)));
//G512: memory_del_cmd(370)  => __E_cli_command_151(371).
	p(NT(370), (NT(371)));
//G513: cli_command(264)     => memory_del_cmd(370).
	p(NT(264), (NT(370)));
//G514: __E_cli_command_152(373) => memory_sym(368) __(72) clear_sym(274).
	p(NT(373), (NT(368)+NT(72)+NT(274)));
//G515: memory_clear_cmd(372) => __E_cli_command_152(373).
	p(NT(372), (NT(373)));
//G516: cli_command(264)     => memory_clear_cmd(372).
	p(NT(264), (NT(372)));
//G517: __E_cli_command_153(375) => memory_sym(368) __(72) memory(361).
	p(NT(375), (NT(368)+NT(72)+NT(361)));
//G518: memory_print_cmd(374) => __E_cli_command_153(375).
	p(NT(374), (NT(375)));
//G519: cli_command(264)     => memory_print_cmd(374).
	p(NT(264), (NT(374)));
//G520: __E___E_cli_command_154_155(379) => bf(41).
	p(NT(379), (NT(41)));
//G521: __E___E_cli_command_154_155(379) => wff(32).
	p(NT(379), (NT(32)));
//G522: __E___E_cli_command_154_155(379) => tau(20).
	p(NT(379), (NT(20)));
//G523: memory_store_cmd_arg(378) => __E___E_cli_command_154_155(379).
	p(NT(378), (NT(379)));
//G524: __E_cli_command_154(377) => memory_sym(368) __(72) memory_store_cmd_arg(378).
	p(NT(377), (NT(368)+NT(72)+NT(378)));
//G525: memory_store_cmd(376) => __E_cli_command_154(377).
	p(NT(376), (NT(377)));
//G526: cli_command(264)     => memory_store_cmd(376).
	p(NT(264), (NT(376)));
//G527: qelim_sym(382)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(382), (T(43)+T(19)+T(16)+T(37)+T(47)));
//G528: __E_cli_command_156(381) => qelim_sym(382) __(72) wff_cmd_arg(299).
	p(NT(381), (NT(382)+NT(72)+NT(299)));
//G529: qelim_cmd(380)       => __E_cli_command_156(381).
	p(NT(380), (NT(381)));
//G530: cli_command(264)     => qelim_cmd(380).
	p(NT(264), (NT(380)));
//G531: get_sym(385)         => 'g' 'e' 't'.
	p(NT(385), (T(46)+T(19)+T(32)));
//G532: __E___E_cli_command_157_158(386) => __(72) option(387).
	p(NT(386), (NT(72)+NT(387)));
//G533: __E___E_cli_command_157_158(386) => null.
	p(NT(386), (nul));
//G534: __E_cli_command_157(384) => get_sym(385) __E___E_cli_command_157_158(386).
	p(NT(384), (NT(385)+NT(386)));
//G535: get_cmd(383)         => __E_cli_command_157(384).
	p(NT(383), (NT(384)));
//G536: cli_command(264)     => get_cmd(383).
	p(NT(264), (NT(383)));
//G537: set_sym(390)         => 's' 'e' 't'.
	p(NT(390), (T(36)+T(19)+T(32)));
//G538: __E___E_cli_command_159_160(391) => __(72).
	p(NT(391), (NT(72)));
//G539: __E___E_cli_command_159_160(391) => _(16) '=' _(16).
	p(NT(391), (NT(16)+T(4)+NT(16)));
//G540: __E_cli_command_159(389) => set_sym(390) __(72) option(387) __E___E_cli_command_159_160(391) option_value(392).
	p(NT(389), (NT(390)+NT(72)+NT(387)+NT(391)+NT(392)));
//G541: set_cmd(388)         => __E_cli_command_159(389).
	p(NT(388), (NT(389)));
//G542: cli_command(264)     => set_cmd(388).
	p(NT(264), (NT(388)));
//G543: toggle_sym(395)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(395), (T(32)+T(15)+T(46)+T(46)+T(16)+T(19)));
//G544: __E_cli_command_161(394) => toggle_sym(395) __(72) bool_option(396).
	p(NT(394), (NT(395)+NT(72)+NT(396)));
//G545: toggle_cmd(393)      => __E_cli_command_161(394).
	p(NT(393), (NT(394)));
//G546: cli_command(264)     => toggle_cmd(393).
	p(NT(264), (NT(393)));
//G547: bf_cmd_arg(322)      => memory(361).
	p(NT(322), (NT(361)));
//G548: bf_cmd_arg(322)      => bf_selection(397).
	p(NT(322), (NT(397)));
//G549: bf_cmd_arg(322)      => bf(41).
	p(NT(322), (NT(41)));
//G550: bf_cmd_out_arg(323)  => memory(361).
	p(NT(323), (NT(361)));
//G551: bf_cmd_out_arg(323)  => bf_selection(397).
	p(NT(323), (NT(397)));
//G552: wff_cmd_arg(299)     => memory(361).
	p(NT(299), (NT(361)));
//G553: wff_cmd_arg(299)     => wff_selection(398).
	p(NT(299), (NT(398)));
//G554: wff_cmd_arg(299)     => wff(32).
	p(NT(299), (NT(32)));
//G555: wff_cmd_out_arg(315) => memory(361).
	p(NT(315), (NT(361)));
//G556: wff_cmd_out_arg(315) => wff_selection(398).
	p(NT(315), (NT(398)));
//G557: nf_cmd_arg(329)      => memory(361).
	p(NT(329), (NT(361)));
//G558: nf_cmd_arg(329)      => bf(41).
	p(NT(329), (NT(41)));
//G559: nf_cmd_arg(329)      => wff(32).
	p(NT(329), (NT(32)));
//G560: onf_cmd_arg(351)     => memory(361).
	p(NT(351), (NT(361)));
//G561: onf_cmd_arg(351)     => wff(32).
	p(NT(351), (NT(32)));
//G562: normalize_cmd_arg(310) => memory(361).
	p(NT(310), (NT(361)));
//G563: normalize_cmd_arg(310) => bf(41).
	p(NT(310), (NT(41)));
//G564: normalize_cmd_arg(310) => wff(32).
	p(NT(310), (NT(32)));
//G565: normalize_cmd_arg(310) => nso_rr(14).
	p(NT(310), (NT(14)));
//G566: help_arg(281)        => help_sym(278).
	p(NT(281), (NT(278)));
//G567: help_arg(281)        => version_sym(271).
	p(NT(281), (NT(271)));
//G568: help_arg(281)        => quit_sym(268).
	p(NT(281), (NT(268)));
//G569: help_arg(281)        => clear_sym(274).
	p(NT(281), (NT(274)));
//G570: help_arg(281)        => get_sym(385).
	p(NT(281), (NT(385)));
//G571: help_arg(281)        => set_sym(390).
	p(NT(281), (NT(390)));
//G572: help_arg(281)        => toggle_sym(395).
	p(NT(281), (NT(395)));
//G573: help_arg(281)        => file_sym(284).
	p(NT(281), (NT(284)));
//G574: help_arg(281)        => memory_sym(368).
	p(NT(281), (NT(368)));
//G575: help_arg(281)        => abs_memory_sym(399).
	p(NT(281), (NT(399)));
//G576: help_arg(281)        => rel_memory_sym(400).
	p(NT(281), (NT(400)));
//G577: help_arg(281)        => selection_sym(401).
	p(NT(281), (NT(401)));
//G578: help_arg(281)        => def_sym(354).
	p(NT(281), (NT(354)));
//G579: help_arg(281)        => inst_sym(313).
	p(NT(281), (NT(313)));
//G580: help_arg(281)        => subst_sym(318).
	p(NT(281), (NT(318)));
//G581: help_arg(281)        => normalize_sym(308).
	p(NT(281), (NT(308)));
//G582: help_arg(281)        => execute_sym(302).
	p(NT(281), (NT(302)));
//G583: help_arg(281)        => solve_sym(297).
	p(NT(281), (NT(297)));
//G584: help_arg(281)        => valid_sym(288).
	p(NT(281), (NT(288)));
//G585: help_arg(281)        => sat_sym(291).
	p(NT(281), (NT(291)));
//G586: help_arg(281)        => unsat_sym(294).
	p(NT(281), (NT(294)));
//G587: help_arg(281)        => dnf_sym(328).
	p(NT(281), (NT(328)));
//G588: help_arg(281)        => cnf_sym(332).
	p(NT(281), (NT(332)));
//G589: help_arg(281)        => anf_sym(335).
	p(NT(281), (NT(335)));
//G590: help_arg(281)        => snf_sym(347).
	p(NT(281), (NT(347)));
//G591: help_arg(281)        => nnf_sym(338).
	p(NT(281), (NT(338)));
//G592: help_arg(281)        => pnf_sym(341).
	p(NT(281), (NT(341)));
//G593: help_arg(281)        => mnf_sym(344).
	p(NT(281), (NT(344)));
//G594: help_arg(281)        => onf_sym(350).
	p(NT(281), (NT(350)));
//G595: help_arg(281)        => qelim_sym(382).
	p(NT(281), (NT(382)));
//G596: __E___E_help_arg_162_163(404) => 's'.
	p(NT(404), (T(36)));
//G597: __E___E_help_arg_162_163(404) => null.
	p(NT(404), (nul));
//G598: __E_help_arg_162(403) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_162_163(404).
	p(NT(403), (T(19)+T(20)+T(18)+T(47)+T(35)+T(16)+T(19)+NT(404)));
//G599: examples_sym(402)    => __E_help_arg_162(403).
	p(NT(402), (NT(403)));
//G600: help_arg(281)        => examples_sym(402).
	p(NT(281), (NT(402)));
//G601: rel_memory_sym(400)  => '%'.
	p(NT(400), (T(57)));
//G602: memory_id(408)       => digits(240).
	p(NT(408), (NT(240)));
//G603: __E___E_memory_164_165(407) => memory_id(408).
	p(NT(407), (NT(408)));
//G604: __E___E_memory_164_165(407) => null.
	p(NT(407), (nul));
//G605: __E_memory_164(406)  => rel_memory_sym(400) __E___E_memory_164_165(407).
	p(NT(406), (NT(400)+NT(407)));
//G606: rel_memory(405)      => __E_memory_164(406).
	p(NT(405), (NT(406)));
//G607: memory(361)          => rel_memory(405).
	p(NT(361), (NT(405)));
//G608: abs_memory_sym(399)  => '%' '-'.
	p(NT(399), (T(57)+T(21)));
//G609: __E___E_memory_166_167(411) => abs_memory_sym(399).
	p(NT(411), (NT(399)));
//G610: __E___E_memory_166_167(411) => memory_sym(368) __(72).
	p(NT(411), (NT(368)+NT(72)));
//G611: __E_memory_166(410)  => __E___E_memory_166_167(411) memory_id(408).
	p(NT(410), (NT(411)+NT(408)));
//G612: abs_memory(409)      => __E_memory_166(410).
	p(NT(409), (NT(410)));
//G613: memory(361)          => abs_memory(409).
	p(NT(361), (NT(409)));
//G614: selection_sym(401)   => 's'.
	p(NT(401), (T(36)));
//G615: selection_sym(401)   => 's' 'e' 'l' 'e' 'c' 't' 'i' 'o' 'n'.
	p(NT(401), (T(36)+T(19)+T(16)+T(19)+T(34)+T(32)+T(37)+T(15)+T(44)));
//G616: wff_selection(398)   => selection_sym(401) __(72) digits(240).
	p(NT(398), (NT(401)+NT(72)+NT(240)));
//G617: bf_selection(397)    => selection_sym(401) __(72) digits(240).
	p(NT(397), (NT(401)+NT(72)+NT(240)));
//G618: bf_var_selection(412) => selection_sym(401) __(72) digits(240).
	p(NT(412), (NT(401)+NT(72)+NT(240)));
//G619: wff_var_selection(413) => selection_sym(401) __(72) digits(240).
	p(NT(413), (NT(401)+NT(72)+NT(240)));
//G620: option(387)          => bool_option(396).
	p(NT(387), (NT(396)));
//G621: __E_option_168(416)  => 's' 'e' 'v'.
	p(NT(416), (T(36)+T(19)+T(38)));
//G622: __E_option_168(416)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(416), (T(36)+T(19)+T(38)+T(19)+T(40)+T(37)+T(32)+T(56)));
//G623: severity_opt(415)    => __E_option_168(416).
	p(NT(415), (NT(416)));
//G624: enum_option(414)     => severity_opt(415).
	p(NT(414), (NT(415)));
//G625: option(387)          => enum_option(414).
	p(NT(387), (NT(414)));
//G626: __E_bool_option_169(418) => 's'.
	p(NT(418), (T(36)));
//G627: __E_bool_option_169(418) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(418), (T(36)+T(32)+T(18)+T(32)+T(33)+T(36)));
//G628: status_opt(417)      => __E_bool_option_169(418).
	p(NT(417), (NT(418)));
//G629: bool_option(396)     => status_opt(417).
	p(NT(396), (NT(417)));
//G630: __E_bool_option_170(420) => 'c'.
	p(NT(420), (T(34)));
//G631: __E_bool_option_170(420) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(420), (T(34)+T(15)+T(16)+T(15)+T(40)));
//G632: __E_bool_option_170(420) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(420), (T(34)+T(15)+T(16)+T(15)+T(40)+T(36)));
//G633: colors_opt(419)      => __E_bool_option_170(420).
	p(NT(419), (NT(420)));
//G634: bool_option(396)     => colors_opt(419).
	p(NT(396), (NT(419)));
//G635: __E_bool_option_171(422) => 'd'.
	p(NT(422), (T(41)));
//G636: __E_bool_option_171(422) => 'd' 'b' 'g'.
	p(NT(422), (T(41)+T(14)+T(46)));
//G637: __E_bool_option_171(422) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(422), (T(41)+T(19)+T(14)+T(33)+T(46)));
//G638: debug_repl_opt(421)  => __E_bool_option_171(422).
	p(NT(421), (NT(422)));
//G639: bool_option(396)     => debug_repl_opt(421).
	p(NT(396), (NT(421)));
//G640: option_value(392)    => option_value_true(423).
	p(NT(392), (NT(423)));
//G641: option_value(392)    => option_value_false(424).
	p(NT(392), (NT(424)));
//G642: option_value(392)    => severity(425).
	p(NT(392), (NT(425)));
//G643: option_value_true(423) => 't'.
	p(NT(423), (T(32)));
//G644: option_value_true(423) => 't' 'r' 'u' 'e'.
	p(NT(423), (T(32)+T(40)+T(33)+T(19)));
//G645: option_value_true(423) => 'o' 'n'.
	p(NT(423), (T(15)+T(44)));
//G646: option_value_true(423) => '1'.
	p(NT(423), (T(29)));
//G647: option_value_true(423) => 'y'.
	p(NT(423), (T(56)));
//G648: option_value_true(423) => 'y' 'e' 's'.
	p(NT(423), (T(56)+T(19)+T(36)));
//G649: option_value_false(424) => 'f'.
	p(NT(424), (T(42)));
//G650: option_value_false(424) => 'f' 'a' 'l' 's' 'e'.
	p(NT(424), (T(42)+T(18)+T(16)+T(36)+T(19)));
//G651: option_value_false(424) => 'o' 'f' 'f'.
	p(NT(424), (T(15)+T(42)+T(42)));
//G652: option_value_false(424) => '0'.
	p(NT(424), (T(30)));
//G653: option_value_false(424) => 'n'.
	p(NT(424), (T(44)));
//G654: option_value_false(424) => 'n' 'o'.
	p(NT(424), (T(44)+T(15)));
//G655: __E_severity_172(427) => 'e'.
	p(NT(427), (T(19)));
//G656: __E_severity_172(427) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(427), (T(19)+T(40)+T(40)+T(15)+T(40)));
//G657: error_sym(426)       => __E_severity_172(427).
	p(NT(426), (NT(427)));
//G658: severity(425)        => error_sym(426).
	p(NT(425), (NT(426)));
//G659: __E_severity_173(429) => 'i'.
	p(NT(429), (T(37)));
//G660: __E_severity_173(429) => 'i' 'n' 'f' 'o'.
	p(NT(429), (T(37)+T(44)+T(42)+T(15)));
//G661: info_sym(428)        => __E_severity_173(429).
	p(NT(428), (NT(429)));
//G662: severity(425)        => info_sym(428).
	p(NT(425), (NT(428)));
//G663: __E_severity_174(431) => 'd'.
	p(NT(431), (T(41)));
//G664: __E_severity_174(431) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(431), (T(41)+T(19)+T(14)+T(33)+T(46)));
//G665: debug_sym(430)       => __E_severity_174(431).
	p(NT(430), (NT(431)));
//G666: severity(425)        => debug_sym(430).
	p(NT(425), (NT(430)));
//G667: __E_severity_175(433) => 't'.
	p(NT(433), (T(32)));
//G668: __E_severity_175(433) => 't' 'r' 'a' 'c' 'e'.
	p(NT(433), (T(32)+T(40)+T(18)+T(34)+T(19)));
//G669: trace_sym(432)       => __E_severity_175(433).
	p(NT(432), (NT(433)));
//G670: severity(425)        => trace_sym(432).
	p(NT(425), (NT(432)));
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
		__E_wff_51, bool_variable, uninterpreted_constant, bf_parenthesis, __E_bf_52, bf_splitter, __E_bf_53, __E_bf_54, __E___E_bf_54_55, bf_or, 
		__E_bf_56, bf_xor, __E_bf_57, bf_and, __E_bf_58, __E___E_bf_58_59, bf_neg, __E_bf_60, bf_constant, __E_bf_61, 
		constant, bf_t, bf_f, variable, __E_ref_args_62, __E___E_ref_args_62_63, ref_arg, __E___E_ref_args_62_64, __E___E_ref_args_62_65, bf_cb_args1, 
		bf_cb_arg, bf_cb_args2, bf_cb_args3, bf_cb_args4, wff_cb_args1, wff_cb_arg, wff_cb_args2, wff_cb_args3, wff_cb_args4, tau_cb_args1, 
		tau_cb_arg, __E_tau_cb_args1_66, tau_cb_args2, tau_cb_args3, tau_collapse_positives_cb, __E_tau_cb_67, __E___E_tau_cb_67_68, tau_positives_upwards_cb, __E_tau_cb_69, bf_eq_cb, 
		__E_wff_cb_70, bf_neq_cb, __E_wff_cb_71, wff_has_clashing_subformulas_cb, __E_wff_cb_72, wff_has_subformula_cb, __E_wff_cb_73, wff_remove_existential_cb, __E_wff_cb_74, wff_remove_bexistential_cb, 
		__E_wff_cb_75, wff_remove_buniversal_cb, __E_wff_cb_76, bf_is_zero_cb, __E_bf_cb_77, bf_is_one_cb, __E_bf_cb_78, bf_has_subformula_cb, __E_bf_cb_79, bf_remove_funiversal_cb, 
		__E_bf_cb_80, bf_remove_fexistential_cb, __E_bf_cb_81, bf_and_cb, __E_bf_cb_82, bf_or_cb, __E_bf_cb_83, bf_xor_cb, __E_bf_cb_84, bf_neg_cb, 
		__E_bf_cb_85, charvar, __E_charvar_86, __E_charvar_87, io_var, __E_variable_88, out, in_var_name, __E_in_89, chars, 
		offset, out_var_name, __E_out_90, __E_offsets_91, __E_offsets_92, num, shift, __E_offset_93, __E_shift_94, __E___E_shift_94_95, 
		digits, uninter_const_name, __E_uninterpreted_constant_96, __E___E_uninterpreted_constant_96_97, binding, __E_binding_98, type, __E___E_binding_98_99, source, named_binding, 
		src_c, __E_source_100, __E_source_101, __E___E_source_101_102, __E___E_source_101_103, __E_chars_104, __E_digits_105, comment, __E_comment_106, __E_comment_107, 
		__E_comment_108, __E____109, __E___110, cli, cli_command, __E_cli_111, __E_cli_112, quit_cmd, quit_sym, __E_cli_command_113, 
		version_cmd, version_sym, __E_cli_command_114, clear_cmd, clear_sym, __E_cli_command_115, help_cmd, __E_cli_command_116, help_sym, __E___E_cli_command_116_117, 
		__E___E_cli_command_116_118, help_arg, file_cmd, __E_cli_command_119, file_sym, q_string, valid_cmd, __E_cli_command_120, valid_sym, sat_cmd, 
		__E_cli_command_121, sat_sym, unsat_cmd, __E_cli_command_122, unsat_sym, solve_cmd, __E_cli_command_123, solve_sym, solve_cmd_arg, wff_cmd_arg, 
		execute_cmd, __E_cli_command_124, execute_sym, __E___E_cli_command_124_125, execute_cmd_arg, __E___E_cli_command_124_126, normalize_cmd, __E_cli_command_127, normalize_sym, __E___E_cli_command_127_128, 
		normalize_cmd_arg, wff_inst_cmd, __E_cli_command_129, inst_sym, __E___E_cli_command_129_130, wff_cmd_out_arg, wff_subst_cmd, __E_cli_command_131, subst_sym, __E___E_cli_command_131_132, 
		bf_inst_cmd, __E_cli_command_133, bf_cmd_arg, bf_cmd_out_arg, bf_subst_cmd, __E_cli_command_134, dnf_cmd, __E_cli_command_135, dnf_sym, nf_cmd_arg, 
		cnf_cmd, __E_cli_command_136, cnf_sym, anf_cmd, __E_cli_command_137, anf_sym, nnf_cmd, __E_cli_command_138, nnf_sym, pnf_cmd, 
		__E_cli_command_139, pnf_sym, mnf_cmd, __E_cli_command_140, mnf_sym, snf_cmd, __E_cli_command_141, snf_sym, onf_cmd, __E_cli_command_142, 
		onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_143, def_sym, def_rr_cmd, __E_cli_command_144, def_del_cmd, __E_cli_command_145, del_sym, 
		__E___E_cli_command_145_146, memory, def_clear_cmd, __E_cli_command_147, def_print_cmd, __E_cli_command_148, memory_list_cmd, __E_cli_command_149, memory_sym, __E___E_cli_command_149_150, 
		memory_del_cmd, __E_cli_command_151, memory_clear_cmd, __E_cli_command_152, memory_print_cmd, __E_cli_command_153, memory_store_cmd, __E_cli_command_154, memory_store_cmd_arg, __E___E_cli_command_154_155, 
		qelim_cmd, __E_cli_command_156, qelim_sym, get_cmd, __E_cli_command_157, get_sym, __E___E_cli_command_157_158, option, set_cmd, __E_cli_command_159, 
		set_sym, __E___E_cli_command_159_160, option_value, toggle_cmd, __E_cli_command_161, toggle_sym, bool_option, bf_selection, wff_selection, abs_memory_sym, 
		rel_memory_sym, selection_sym, examples_sym, __E_help_arg_162, __E___E_help_arg_162_163, rel_memory, __E_memory_164, __E___E_memory_164_165, memory_id, abs_memory, 
		__E_memory_166, __E___E_memory_166_167, bf_var_selection, wff_var_selection, enum_option, severity_opt, __E_option_168, status_opt, __E_bool_option_169, colors_opt, 
		__E_bool_option_170, debug_repl_opt, __E_bool_option_171, option_value_true, option_value_false, severity, error_sym, __E_severity_172, info_sym, __E_severity_173, 
		debug_sym, __E_severity_174, trace_sym, __E_severity_175, bf_all, bf_ex, __N_0, __N_1, __N_2, __N_3, 
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
