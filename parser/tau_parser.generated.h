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
	"__E_start_0", "inputs", "builder", "library", "rr", "_", "rec_relations", "main", "wff", "__E_rec_relations_1", 
	"rec_relation", "__E_rec_relations_2", "ref", "__E_rec_relation_3", "capture", "bf", "sym", "__E_ref_4", "offsets", "ref_args", 
	"__E_ref_args_5", "__E___E_ref_args_5_6", "ref_arg", "__E___E_ref_args_5_7", "__E___E_ref_args_5_8", "__E_inputs_9", "__E_inputs_10", "input", "in", "source_binding", 
	"rules", "__E_library_11", "__E___E_library_11_12", "rule", "__E___E___E_library_11_12_13", "wff_rule", "bf_rule", "tau_rule", "__E___E_library_11_14", "tau_matcher", 
	"tau", "tau_body", "__E_tau_rule_15", "tau_cb", "wff_matcher", "wff_body", "__E_wff_rule_16", "wff_cb", "bf_matcher", "bf_body", 
	"__E_bf_rule_17", "bf_cb", "builder_head", "builder_body", "__E_builder_head_18", "__", "__E_builder_head_19", "bf_builder_body", "__E_builder_body_20", "wff_builder_body", 
	"__E_builder_body_21", "tau_builder_body", "__E_builder_body_22", "tau_parenthesis", "__E_tau_23", "tau_wff", "__E_tau_24", "tau_ref", "__E_tau_25", "__E___E_tau_25_26", 
	"tau_or", "__E_tau_27", "tau_and", "__E_tau_28", "tau_neg", "__E_tau_29", "wff_parenthesis", "__E_wff_30", "wff_sometimes", "__E_wff_31", 
	"__E___E_wff_31_32", "wff_always", "__E_wff_33", "__E___E_wff_33_34", "wff_conditional", "__E_wff_35", "wff_ball", "__E_wff_36", "q_bool_var", "wff_bex", 
	"__E_wff_37", "wff_all", "__E_wff_38", "q_var", "wff_ex", "__E_wff_39", "wff_ref", "wff_imply", "__E_wff_40", "wff_equiv", 
	"__E_wff_41", "wff_or", "__E_wff_42", "wff_xor", "__E_wff_43", "wff_and", "__E_wff_44", "wff_neg", "__E_wff_45", "wff_t", 
	"wff_f", "bf_interval", "__E_wff_46", "bf_neq", "__E_wff_47", "bf_eq", "__E_wff_48", "bf_nleq_lower", "__E_wff_49", "bf_nleq_upper", 
	"__E_wff_50", "bf_greater", "__E_wff_51", "bf_less_equal", "__E_wff_52", "bf_less", "__E_wff_53", "bool_variable", "uninterpreted_constant", "bf_parenthesis", 
	"__E_bf_54", "bf_splitter", "__E_bf_55", "bf_ref", "bf_or", "__E_bf_56", "bf_xor", "__E_bf_57", "bf_and", "__E_bf_58", 
	"__E___E_bf_58_59", "bf_neg", "__E_bf_60", "bf_constant", "__E_bf_61", "constant", "bf_t", "bf_f", "variable", "charvar", 
	"__E_charvar_62", "__E_charvar_63", "io_var", "__E_variable_64", "out", "in_var_name", "__E_in_65", "digits", "offset", "out_var_name", 
	"__E_out_66", "__E_offsets_67", "__E_offsets_68", "num", "shift", "__E_offset_69", "__E_shift_70", "__E___E_shift_70_71", "uninter_const_name", "__E_uninterpreted_constant_72", 
	"__E___E_uninterpreted_constant_72_73", "chars", "binding", "__E_binding_74", "type", "__E___E_binding_74_75", "source", "named_binding", "src_c", "__E_source_76", 
	"__E_source_77", "__E___E_source_77_78", "__E___E_source_77_79", "__E_chars_80", "__E_digits_81", "comment", "__E_comment_82", "__E_comment_83", "__E_comment_84", "__E____85", 
	"__E___86", "bf_cb_args1", "bf_cb_arg", "bf_cb_args2", "bf_cb_args3", "bf_cb_args4", "wff_cb_args1", "wff_cb_arg", "wff_cb_args2", "wff_cb_args3", 
	"wff_cb_args4", "tau_cb_args1", "tau_cb_arg", "__E_tau_cb_args1_87", "tau_cb_args2", "tau_cb_args3", "tau_collapse_positives_cb", "__E_tau_cb_88", "__E___E_tau_cb_88_89", "tau_positives_upwards_cb", 
	"__E_tau_cb_90", "bf_eq_cb", "__E_wff_cb_91", "bf_neq_cb", "__E_wff_cb_92", "wff_has_clashing_subformulas_cb", "__E_wff_cb_93", "wff_has_subformula_cb", "__E_wff_cb_94", "wff_remove_existential_cb", 
	"__E_wff_cb_95", "wff_remove_bexistential_cb", "__E_wff_cb_96", "wff_remove_buniversal_cb", "__E_wff_cb_97", "bf_is_zero_cb", "__E_bf_cb_98", "bf_is_one_cb", "__E_bf_cb_99", "bf_has_subformula_cb", 
	"__E_bf_cb_100", "bf_remove_funiversal_cb", "__E_bf_cb_101", "bf_remove_fexistential_cb", "__E_bf_cb_102", "bf_and_cb", "__E_bf_cb_103", "bf_or_cb", "__E_bf_cb_104", "bf_xor_cb", 
	"__E_bf_cb_105", "bf_neg_cb", "__E_bf_cb_106", "bf_normalize_cb", "__E_bf_cb_107", "cli", "cli_command", "__E_cli_108", "__E_cli_109", "quit_cmd", 
	"quit_sym", "__E_cli_command_110", "version_cmd", "version_sym", "__E_cli_command_111", "clear_cmd", "clear_sym", "__E_cli_command_112", "help_cmd", "__E_cli_command_113", 
	"help_sym", "__E___E_cli_command_113_114", "__E___E_cli_command_113_115", "help_arg", "file_cmd", "__E_cli_command_116", "file_sym", "q_string", "valid_cmd", "__E_cli_command_117", 
	"valid_sym", "sat_cmd", "__E_cli_command_118", "sat_sym", "unsat_cmd", "__E_cli_command_119", "unsat_sym", "solve_cmd", "__E_cli_command_120", "solve_sym", 
	"wff_cmd_arg", "execute_cmd", "__E_cli_command_121", "execute_sym", "__E___E_cli_command_121_122", "__E___E_cli_command_121_123", "wff_typed", "normalize_cmd", "__E_cli_command_124", "normalize_sym", 
	"__E___E_cli_command_124_125", "normalize_cmd_arg", "subst_cmd", "__E_cli_command_126", "subst_sym", "__E___E_cli_command_126_127", "nf_cmd_arg", "inst_cmd", "__E_cli_command_128", "inst_sym", 
	"__E___E_cli_command_128_129", "inst_args", "dnf_cmd", "__E_cli_command_130", "dnf_sym", "cnf_cmd", "__E_cli_command_131", "cnf_sym", "anf_cmd", "__E_cli_command_132", 
	"anf_sym", "nnf_cmd", "__E_cli_command_133", "nnf_sym", "pnf_cmd", "__E_cli_command_134", "pnf_sym", "mnf_cmd", "__E_cli_command_135", "mnf_sym", 
	"snf_cmd", "__E_cli_command_136", "snf_sym", "onf_cmd", "__E_cli_command_137", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_138", "def_sym", 
	"__E___E_cli_command_138_139", "def_print_cmd", "__E_cli_command_140", "number", "def_rr_cmd", "history_list_cmd", "__E_cli_command_141", "history_sym", "__E___E_cli_command_141_142", "history_print_cmd", 
	"__E_cli_command_143", "memory", "history_store_cmd", "__E_cli_command_144", "bf_typed", "qelim_cmd", "__E_cli_command_145", "qelim_sym", "get_cmd", "__E_cli_command_146", 
	"get_sym", "__E___E_cli_command_146_147", "option", "set_cmd", "__E_cli_command_148", "set_sym", "__E___E_cli_command_148_149", "option_value", "toggle_cmd", "__E_cli_command_150", 
	"toggle_sym", "bool_option", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "examples_sym", "__E_help_arg_151", "__E___E_help_arg_151_152", "rel_memory", 
	"__E_memory_153", "__E___E_memory_153_154", "memory_id", "abs_memory", "__E_memory_155", "enum_option", "severity_opt", "__E_option_156", "status_opt", "__E_bool_option_157", 
	"colors_opt", "__E_bool_option_158", "debug_repl_opt", "__E_bool_option_159", "option_value_true", "option_value_false", "severity", "error_sym", "__E_severity_160", "info_sym", 
	"__E_severity_161", "debug_sym", "__E_severity_162", "trace_sym", "__E_severity_163", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", 
	"__N_5", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', ',', '(', ')', '<', '{', 
	'}', '|', '&', '-', 's', 'o', 'm', 'e', 't', 'i', 
	'>', 'a', 'l', 'w', 'y', '[', ']', '?', 'b', '_', 
	'x', '^', '!', 'T', 'F', 'S', '+', '\'', '1', '0', 
	'$', '\t', '\n', '\r', '#', 'u', 'c', 'p', 'v', 'r', 
	'd', 'f', 'q', 'n', 'h', 'g', 'z', '/', '%', 
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
			15, 65
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			24, 26, 137, 159, 165, 167, 169, 178, 184, 186,
			187
		},
		.to_inline = {
			{ 18, 86, 18 },
			{ 25, 139, 25 },
			{ 50, 73, 50 },
			{ 53 },
			{ 57 },
			{ 61 },
			{ 98 },
			{ 103 },
			{ 181 },
			{ 188 },
			{ 201 },
			{ 203 },
			{ 204 },
			{ 205 },
			{ 206 },
			{ 208 },
			{ 209 },
			{ 210 },
			{ 211 },
			{ 214 },
			{ 215 },
			{ 290 },
			{ 296 },
			{ 301 },
			{ 306 },
			{ 311 },
			{ 336 },
			{ 354 },
			{ 372 }
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
//G3:   __E_start_0(10)      => rr(14).
	p(NT(10), (NT(14)));
//G4:   start(9)             => __E_start_0(10) _(15).
	p(NT(9), (NT(10)+NT(15)));
//G5:   main(17)             => wff(18).
	p(NT(17), (NT(18)));
//G6:   rr(14)               => rec_relations(16) _(15) main(17) _(15) '.'.
	p(NT(14), (NT(16)+NT(15)+NT(17)+NT(15)+T(1)));
//G7:   __E_rec_relations_1(19) => _(15) rec_relation(20) _(15) '.'.
	p(NT(19), (NT(15)+NT(20)+NT(15)+T(1)));
//G8:   __E_rec_relations_2(21) => null.
	p(NT(21), (nul));
//G9:   __E_rec_relations_2(21) => __E_rec_relations_1(19) __E_rec_relations_2(21).
	p(NT(21), (NT(19)+NT(21)));
//G10:  rec_relations(16)    => __E_rec_relations_2(21).
	p(NT(16), (NT(21)));
//G11:  __E_rec_relation_3(23) => capture(24).
	p(NT(23), (NT(24)));
//G12:  __E_rec_relation_3(23) => ref(22).
	p(NT(23), (NT(22)));
//G13:  __E_rec_relation_3(23) => wff(18).
	p(NT(23), (NT(18)));
//G14:  __E_rec_relation_3(23) => bf(25).
	p(NT(23), (NT(25)));
//G15:  rec_relation(20)     => ref(22) _(15) ':' '=' _(15) __E_rec_relation_3(23).
	p(NT(20), (NT(22)+NT(15)+T(2)+T(3)+NT(15)+NT(23)));
//G16:  __E_ref_4(27)        => _(15) offsets(28).
	p(NT(27), (NT(15)+NT(28)));
//G17:  __E_ref_4(27)        => null.
	p(NT(27), (nul));
//G18:  ref(22)              => sym(26) __E_ref_4(27) _(15) ref_args(29).
	p(NT(22), (NT(26)+NT(27)+NT(15)+NT(29)));
//G19:  ref_arg(32)          => bf(25).
	p(NT(32), (NT(25)));
//G20:  __E___E_ref_args_5_6(31) => _(15) ref_arg(32).
	p(NT(31), (NT(15)+NT(32)));
//G21:  __E___E_ref_args_5_7(33) => _(15) ',' _(15) ref_arg(32).
	p(NT(33), (NT(15)+T(4)+NT(15)+NT(32)));
//G22:  __E___E_ref_args_5_8(34) => null.
	p(NT(34), (nul));
//G23:  __E___E_ref_args_5_8(34) => __E___E_ref_args_5_7(33) __E___E_ref_args_5_8(34).
	p(NT(34), (NT(33)+NT(34)));
//G24:  __E_ref_args_5(30)   => __E___E_ref_args_5_6(31) __E___E_ref_args_5_8(34).
	p(NT(30), (NT(31)+NT(34)));
//G25:  __E_ref_args_5(30)   => null.
	p(NT(30), (nul));
//G26:  ref_args(29)         => '(' __E_ref_args_5(30) _(15) ')'.
	p(NT(29), (T(5)+NT(30)+NT(15)+T(6)));
//G27:  __E_inputs_10(36)    => _(15) input(37).
	p(NT(36), (NT(15)+NT(37)));
//G28:  __E_inputs_9(35)     => __E_inputs_10(36).
	p(NT(35), (NT(36)));
//G29:  __E_inputs_9(35)     => __E_inputs_10(36) __E_inputs_9(35).
	p(NT(35), (NT(36)+NT(35)));
//G30:  inputs(11)           => _(15) '<' __E_inputs_9(35) _(15) '.'.
	p(NT(11), (NT(15)+T(7)+NT(35)+NT(15)+T(1)));
//G31:  input(37)            => in(38) _(15) ':' _(15) '{' _(15) source_binding(39) _(15) '}'.
	p(NT(37), (NT(38)+NT(15)+T(2)+NT(15)+T(8)+NT(15)+NT(39)+NT(15)+T(9)));
//G32:  __E___E___E_library_11_12_13(44) => wff_rule(45).
	p(NT(44), (NT(45)));
//G33:  __E___E___E_library_11_12_13(44) => bf_rule(46).
	p(NT(44), (NT(46)));
//G34:  __E___E___E_library_11_12_13(44) => tau_rule(47).
	p(NT(44), (NT(47)));
//G35:  rule(43)             => __E___E___E_library_11_12_13(44).
	p(NT(43), (NT(44)));
//G36:  __E___E_library_11_12(42) => _(15) rule(43).
	p(NT(42), (NT(15)+NT(43)));
//G37:  __E___E_library_11_14(48) => null.
	p(NT(48), (nul));
//G38:  __E___E_library_11_14(48) => __E___E_library_11_12(42) __E___E_library_11_14(48).
	p(NT(48), (NT(42)+NT(48)));
//G39:  __E_library_11(41)   => __E___E_library_11_14(48).
	p(NT(41), (NT(48)));
//G40:  rules(40)            => __E_library_11(41).
	p(NT(40), (NT(41)));
//G41:  library(13)          => rules(40).
	p(NT(13), (NT(40)));
//G42:  tau_matcher(49)      => tau(50).
	p(NT(49), (NT(50)));
//G43:  __E_tau_rule_15(52)  => tau(50).
	p(NT(52), (NT(50)));
//G44:  __E_tau_rule_15(52)  => tau_cb(53).
	p(NT(52), (NT(53)));
//G45:  tau_body(51)         => __E_tau_rule_15(52).
	p(NT(51), (NT(52)));
//G46:  tau_rule(47)         => tau_matcher(49) _(15) ':' ':' ':' '=' _(15) tau_body(51) _(15) '.'.
	p(NT(47), (NT(49)+NT(15)+T(2)+T(2)+T(2)+T(3)+NT(15)+NT(51)+NT(15)+T(1)));
//G47:  wff_matcher(54)      => wff(18).
	p(NT(54), (NT(18)));
//G48:  __E_wff_rule_16(56)  => wff(18).
	p(NT(56), (NT(18)));
//G49:  __E_wff_rule_16(56)  => wff_cb(57).
	p(NT(56), (NT(57)));
//G50:  wff_body(55)         => __E_wff_rule_16(56).
	p(NT(55), (NT(56)));
//G51:  wff_rule(45)         => wff_matcher(54) _(15) ':' ':' '=' _(15) wff_body(55) _(15) '.'.
	p(NT(45), (NT(54)+NT(15)+T(2)+T(2)+T(3)+NT(15)+NT(55)+NT(15)+T(1)));
//G52:  bf_matcher(58)       => bf(25).
	p(NT(58), (NT(25)));
//G53:  __E_bf_rule_17(60)   => bf(25).
	p(NT(60), (NT(25)));
//G54:  __E_bf_rule_17(60)   => bf_cb(61).
	p(NT(60), (NT(61)));
//G55:  bf_body(59)          => __E_bf_rule_17(60).
	p(NT(59), (NT(60)));
//G56:  bf_rule(46)          => bf_matcher(58) _(15) ':' '=' _(15) bf_body(59) _(15) '.'.
	p(NT(46), (NT(58)+NT(15)+T(2)+T(3)+NT(15)+NT(59)+NT(15)+T(1)));
//G57:  builder(12)          => _(15) builder_head(62) _(15) builder_body(63) _(15) '.'.
	p(NT(12), (NT(15)+NT(62)+NT(15)+NT(63)+NT(15)+T(1)));
//G58:  __E_builder_head_18(64) => __(65) capture(24).
	p(NT(64), (NT(65)+NT(24)));
//G59:  __E_builder_head_19(66) => null.
	p(NT(66), (nul));
//G60:  __E_builder_head_19(66) => __E_builder_head_18(64) __E_builder_head_19(66).
	p(NT(66), (NT(64)+NT(66)));
//G61:  builder_head(62)     => '(' _(15) capture(24) __E_builder_head_19(66) _(15) ')'.
	p(NT(62), (T(5)+NT(15)+NT(24)+NT(66)+NT(15)+T(6)));
//G62:  __E_builder_body_20(68) => '=' ':' _(15) bf(25).
	p(NT(68), (T(3)+T(2)+NT(15)+NT(25)));
//G63:  bf_builder_body(67)  => __E_builder_body_20(68).
	p(NT(67), (NT(68)));
//G64:  builder_body(63)     => bf_builder_body(67).
	p(NT(63), (NT(67)));
//G65:  __E_builder_body_21(70) => '=' ':' ':' _(15) wff(18).
	p(NT(70), (T(3)+T(2)+T(2)+NT(15)+NT(18)));
//G66:  wff_builder_body(69) => __E_builder_body_21(70).
	p(NT(69), (NT(70)));
//G67:  builder_body(63)     => wff_builder_body(69).
	p(NT(63), (NT(69)));
//G68:  __E_builder_body_22(72) => '=' ':' ':' ':' _(15) tau(50).
	p(NT(72), (T(3)+T(2)+T(2)+T(2)+NT(15)+NT(50)));
//G69:  tau_builder_body(71) => __E_builder_body_22(72).
	p(NT(71), (NT(72)));
//G70:  builder_body(63)     => tau_builder_body(71).
	p(NT(63), (NT(71)));
//G71:  __E_tau_23(74)       => '(' _(15) tau(50) _(15) ')'.
	p(NT(74), (T(5)+NT(15)+NT(50)+NT(15)+T(6)));
//G72:  tau_parenthesis(73)  => __E_tau_23(74).
	p(NT(73), (NT(74)));
//G73:  tau(50)              => tau_parenthesis(73).
	p(NT(50), (NT(73)));
//G74:  __E_tau_24(76)       => '{' _(15) wff(18) _(15) '}'.
	p(NT(76), (T(8)+NT(15)+NT(18)+NT(15)+T(9)));
//G75:  tau_wff(75)          => __E_tau_24(76).
	p(NT(75), (NT(76)));
//G76:  tau(50)              => tau_wff(75).
	p(NT(50), (NT(75)));
//G77:  __E___E_tau_25_26(79) => _(15) offsets(28).
	p(NT(79), (NT(15)+NT(28)));
//G78:  __E___E_tau_25_26(79) => null.
	p(NT(79), (nul));
//G79:  __E_tau_25(78)       => sym(26) __E___E_tau_25_26(79) _(15) ref_args(29).
	p(NT(78), (NT(26)+NT(79)+NT(15)+NT(29)));
//G80:  tau_ref(77)          => __E_tau_25(78).
	p(NT(77), (NT(78)));
//G81:  tau(50)              => tau_ref(77).
	p(NT(50), (NT(77)));
//G82:  __E_tau_27(81)       => tau(50) _(15) '|' '|' '|' _(15) tau(50).
	p(NT(81), (NT(50)+NT(15)+T(10)+T(10)+T(10)+NT(15)+NT(50)));
//G83:  tau_or(80)           => __E_tau_27(81).
	p(NT(80), (NT(81)));
//G84:  tau(50)              => tau_or(80).
	p(NT(50), (NT(80)));
//G85:  __E_tau_28(83)       => tau(50) _(15) '&' '&' '&' _(15) tau(50).
	p(NT(83), (NT(50)+NT(15)+T(11)+T(11)+T(11)+NT(15)+NT(50)));
//G86:  tau_and(82)          => __E_tau_28(83).
	p(NT(82), (NT(83)));
//G87:  tau(50)              => tau_and(82).
	p(NT(50), (NT(82)));
//G88:  __E_tau_29(85)       => '-' _(15) tau(50).
	p(NT(85), (T(12)+NT(15)+NT(50)));
//G89:  tau_neg(84)          => __E_tau_29(85).
	p(NT(84), (NT(85)));
//G90:  tau(50)              => tau_neg(84).
	p(NT(50), (NT(84)));
//G91:  tau(50)              => capture(24).
	p(NT(50), (NT(24)));
//G92:  __E_wff_30(87)       => '(' _(15) wff(18) _(15) ')'.
	p(NT(87), (T(5)+NT(15)+NT(18)+NT(15)+T(6)));
//G93:  wff_parenthesis(86)  => __E_wff_30(87).
	p(NT(86), (NT(87)));
//G94:  wff(18)              => wff_parenthesis(86).
	p(NT(18), (NT(86)));
//G95:  __E___E_wff_31_32(90) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(90), (T(13)+T(14)+T(15)+T(16)+T(17)+T(18)+T(15)+T(16)+T(13)));
//G96:  __E___E_wff_31_32(90) => '<' '>'.
	p(NT(90), (T(7)+T(19)));
//G97:  __E_wff_31(89)       => __E___E_wff_31_32(90) _(15) wff(18).
	p(NT(89), (NT(90)+NT(15)+NT(18)));
//G98:  wff_sometimes(88)    => __E_wff_31(89).
	p(NT(88), (NT(89)));
//G99:  wff(18)              => wff_sometimes(88).
	p(NT(18), (NT(88)));
//G100: __E___E_wff_33_34(93) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(93), (T(20)+T(21)+T(22)+T(20)+T(23)+T(13)));
//G101: __E___E_wff_33_34(93) => '[' ']'.
	p(NT(93), (T(24)+T(25)));
//G102: __E_wff_33(92)       => __E___E_wff_33_34(93) _(15) wff(18).
	p(NT(92), (NT(93)+NT(15)+NT(18)));
//G103: wff_always(91)       => __E_wff_33(92).
	p(NT(91), (NT(92)));
//G104: wff(18)              => wff_always(91).
	p(NT(18), (NT(91)));
//G105: __E_wff_35(95)       => wff(18) _(15) '?' _(15) wff(18) _(15) ':' _(15) wff(18).
	p(NT(95), (NT(18)+NT(15)+T(26)+NT(15)+NT(18)+NT(15)+T(2)+NT(15)+NT(18)));
//G106: wff_conditional(94)  => __E_wff_35(95).
	p(NT(94), (NT(95)));
//G107: wff(18)              => wff_conditional(94).
	p(NT(18), (NT(94)));
//G108: __E_wff_36(97)       => 'b' '_' 'a' 'l' 'l' __(65) q_bool_var(98) __(65) wff(18).
	p(NT(97), (T(27)+T(28)+T(20)+T(21)+T(21)+NT(65)+NT(98)+NT(65)+NT(18)));
//G109: wff_ball(96)         => __E_wff_36(97).
	p(NT(96), (NT(97)));
//G110: wff(18)              => wff_ball(96).
	p(NT(18), (NT(96)));
//G111: __E_wff_37(100)      => 'b' '_' 'e' 'x' __(65) q_bool_var(98) __(65) wff(18).
	p(NT(100), (T(27)+T(28)+T(16)+T(29)+NT(65)+NT(98)+NT(65)+NT(18)));
//G112: wff_bex(99)          => __E_wff_37(100).
	p(NT(99), (NT(100)));
//G113: wff(18)              => wff_bex(99).
	p(NT(18), (NT(99)));
//G114: __E_wff_38(102)      => 'a' 'l' 'l' __(65) q_var(103) __(65) wff(18).
	p(NT(102), (T(20)+T(21)+T(21)+NT(65)+NT(103)+NT(65)+NT(18)));
//G115: wff_all(101)         => __E_wff_38(102).
	p(NT(101), (NT(102)));
//G116: wff(18)              => wff_all(101).
	p(NT(18), (NT(101)));
//G117: __E_wff_39(105)      => 'e' 'x' __(65) q_var(103) __(65) wff(18).
	p(NT(105), (T(16)+T(29)+NT(65)+NT(103)+NT(65)+NT(18)));
//G118: wff_ex(104)          => __E_wff_39(105).
	p(NT(104), (NT(105)));
//G119: wff(18)              => wff_ex(104).
	p(NT(18), (NT(104)));
//G120: wff_ref(106)         => ref(22).
	p(NT(106), (NT(22)));
//G121: wff(18)              => wff_ref(106).
	p(NT(18), (NT(106)));
//G122: __E_wff_40(108)      => wff(18) _(15) '-' '>' _(15) wff(18).
	p(NT(108), (NT(18)+NT(15)+T(12)+T(19)+NT(15)+NT(18)));
//G123: wff_imply(107)       => __E_wff_40(108).
	p(NT(107), (NT(108)));
//G124: wff(18)              => wff_imply(107).
	p(NT(18), (NT(107)));
//G125: __E_wff_41(110)      => wff(18) _(15) '<' '-' '>' _(15) wff(18).
	p(NT(110), (NT(18)+NT(15)+T(7)+T(12)+T(19)+NT(15)+NT(18)));
//G126: wff_equiv(109)       => __E_wff_41(110).
	p(NT(109), (NT(110)));
//G127: wff(18)              => wff_equiv(109).
	p(NT(18), (NT(109)));
//G128: __E_wff_42(112)      => wff(18) _(15) '|' '|' _(15) wff(18).
	p(NT(112), (NT(18)+NT(15)+T(10)+T(10)+NT(15)+NT(18)));
//G129: wff_or(111)          => __E_wff_42(112).
	p(NT(111), (NT(112)));
//G130: wff(18)              => wff_or(111).
	p(NT(18), (NT(111)));
//G131: __E_wff_43(114)      => wff(18) _(15) '^' _(15) wff(18).
	p(NT(114), (NT(18)+NT(15)+T(30)+NT(15)+NT(18)));
//G132: wff_xor(113)         => __E_wff_43(114).
	p(NT(113), (NT(114)));
//G133: wff(18)              => wff_xor(113).
	p(NT(18), (NT(113)));
//G134: __E_wff_44(116)      => wff(18) _(15) '&' '&' _(15) wff(18).
	p(NT(116), (NT(18)+NT(15)+T(11)+T(11)+NT(15)+NT(18)));
//G135: wff_and(115)         => __E_wff_44(116).
	p(NT(115), (NT(116)));
//G136: wff(18)              => wff_and(115).
	p(NT(18), (NT(115)));
//G137: __E_wff_45(118)      => '!' _(15) wff(18).
	p(NT(118), (T(31)+NT(15)+NT(18)));
//G138: wff_neg(117)         => __E_wff_45(118).
	p(NT(117), (NT(118)));
//G139: wff(18)              => wff_neg(117).
	p(NT(18), (NT(117)));
//G140: wff_t(119)           => 'T'.
	p(NT(119), (T(32)));
//G141: wff(18)              => wff_t(119).
	p(NT(18), (NT(119)));
//G142: wff_f(120)           => 'F'.
	p(NT(120), (T(33)));
//G143: wff(18)              => wff_f(120).
	p(NT(18), (NT(120)));
//G144: __E_wff_46(122)      => bf(25) _(15) '<' '=' _(15) q_var(103) _(15) '<' '=' _(15) bf(25).
	p(NT(122), (NT(25)+NT(15)+T(7)+T(3)+NT(15)+NT(103)+NT(15)+T(7)+T(3)+NT(15)+NT(25)));
//G145: bf_interval(121)     => __E_wff_46(122).
	p(NT(121), (NT(122)));
//G146: wff(18)              => bf_interval(121).
	p(NT(18), (NT(121)));
//G147: __E_wff_47(124)      => bf(25) _(15) '!' '=' _(15) bf(25).
	p(NT(124), (NT(25)+NT(15)+T(31)+T(3)+NT(15)+NT(25)));
//G148: bf_neq(123)          => __E_wff_47(124).
	p(NT(123), (NT(124)));
//G149: wff(18)              => bf_neq(123).
	p(NT(18), (NT(123)));
//G150: __E_wff_48(126)      => bf(25) _(15) '=' _(15) bf(25).
	p(NT(126), (NT(25)+NT(15)+T(3)+NT(15)+NT(25)));
//G151: bf_eq(125)           => __E_wff_48(126).
	p(NT(125), (NT(126)));
//G152: wff(18)              => bf_eq(125).
	p(NT(18), (NT(125)));
//G153: __E_wff_49(128)      => bf(25) _(15) '!' '<' '=' _(15) q_var(103).
	p(NT(128), (NT(25)+NT(15)+T(31)+T(7)+T(3)+NT(15)+NT(103)));
//G154: bf_nleq_lower(127)   => __E_wff_49(128).
	p(NT(127), (NT(128)));
//G155: wff(18)              => bf_nleq_lower(127).
	p(NT(18), (NT(127)));
//G156: __E_wff_50(130)      => q_var(103) _(15) '!' '<' '=' _(15) bf(25).
	p(NT(130), (NT(103)+NT(15)+T(31)+T(7)+T(3)+NT(15)+NT(25)));
//G157: bf_nleq_upper(129)   => __E_wff_50(130).
	p(NT(129), (NT(130)));
//G158: wff(18)              => bf_nleq_upper(129).
	p(NT(18), (NT(129)));
//G159: __E_wff_51(132)      => bf(25) _(15) '>' _(15) bf(25).
	p(NT(132), (NT(25)+NT(15)+T(19)+NT(15)+NT(25)));
//G160: bf_greater(131)      => __E_wff_51(132).
	p(NT(131), (NT(132)));
//G161: wff(18)              => bf_greater(131).
	p(NT(18), (NT(131)));
//G162: __E_wff_52(134)      => bf(25) _(15) '<' '=' _(15) bf(25).
	p(NT(134), (NT(25)+NT(15)+T(7)+T(3)+NT(15)+NT(25)));
//G163: bf_less_equal(133)   => __E_wff_52(134).
	p(NT(133), (NT(134)));
//G164: wff(18)              => bf_less_equal(133).
	p(NT(18), (NT(133)));
//G165: __E_wff_53(136)      => bf(25) _(15) '<' _(15) bf(25).
	p(NT(136), (NT(25)+NT(15)+T(7)+NT(15)+NT(25)));
//G166: bf_less(135)         => __E_wff_53(136).
	p(NT(135), (NT(136)));
//G167: wff(18)              => bf_less(135).
	p(NT(18), (NT(135)));
//G168: wff(18)              => capture(24).
	p(NT(18), (NT(24)));
//G169: wff(18)              => bool_variable(137).
	p(NT(18), (NT(137)));
//G170: wff(18)              => uninterpreted_constant(138).
	p(NT(18), (NT(138)));
//G171: __E_bf_54(140)       => '(' _(15) bf(25) _(15) ')'.
	p(NT(140), (T(5)+NT(15)+NT(25)+NT(15)+T(6)));
//G172: bf_parenthesis(139)  => __E_bf_54(140).
	p(NT(139), (NT(140)));
//G173: bf(25)               => bf_parenthesis(139).
	p(NT(25), (NT(139)));
//G174: __E_bf_55(142)       => 'S' _(15) '(' _(15) bf(25) _(15) ')'.
	p(NT(142), (T(34)+NT(15)+T(5)+NT(15)+NT(25)+NT(15)+T(6)));
//G175: bf_splitter(141)     => __E_bf_55(142).
	p(NT(141), (NT(142)));
//G176: bf(25)               => bf_splitter(141).
	p(NT(25), (NT(141)));
//G177: bf_ref(143)          => ref(22).
	p(NT(143), (NT(22)));
//G178: bf(25)               => bf_ref(143).
	p(NT(25), (NT(143)));
//G179: __E_bf_56(145)       => bf(25) _(15) '|' _(15) bf(25).
	p(NT(145), (NT(25)+NT(15)+T(10)+NT(15)+NT(25)));
//G180: bf_or(144)           => __E_bf_56(145).
	p(NT(144), (NT(145)));
//G181: bf(25)               => bf_or(144).
	p(NT(25), (NT(144)));
//G182: __E_bf_57(147)       => bf(25) _(15) '+' _(15) bf(25).
	p(NT(147), (NT(25)+NT(15)+T(35)+NT(15)+NT(25)));
//G183: bf_xor(146)          => __E_bf_57(147).
	p(NT(146), (NT(147)));
//G184: bf(25)               => bf_xor(146).
	p(NT(25), (NT(146)));
//G185: __E___E_bf_58_59(150) => _(15).
	p(NT(150), (NT(15)));
//G186: __E___E_bf_58_59(150) => _(15) '&' _(15).
	p(NT(150), (NT(15)+T(11)+NT(15)));
//G187: __E_bf_58(149)       => bf(25) __E___E_bf_58_59(150) bf(25).
	p(NT(149), (NT(25)+NT(150)+NT(25)));
//G188: bf_and(148)          => __E_bf_58(149).
	p(NT(148), (NT(149)));
//G189: bf(25)               => bf_and(148).
	p(NT(25), (NT(148)));
//G190: __E_bf_60(152)       => bf(25) _(15) '\''.
	p(NT(152), (NT(25)+NT(15)+T(36)));
//G191: bf_neg(151)          => __E_bf_60(152).
	p(NT(151), (NT(152)));
//G192: bf(25)               => bf_neg(151).
	p(NT(25), (NT(151)));
//G193: __E_bf_61(154)       => '{' _(15) constant(155) _(15) '}'.
	p(NT(154), (T(8)+NT(15)+NT(155)+NT(15)+T(9)));
//G194: bf_constant(153)     => __E_bf_61(154).
	p(NT(153), (NT(154)));
//G195: bf(25)               => bf_constant(153).
	p(NT(25), (NT(153)));
//G196: bf_t(156)            => '1'.
	p(NT(156), (T(37)));
//G197: bf(25)               => bf_t(156).
	p(NT(25), (NT(156)));
//G198: bf_f(157)            => '0'.
	p(NT(157), (T(38)));
//G199: bf(25)               => bf_f(157).
	p(NT(25), (NT(157)));
//G200: bf(25)               => capture(24).
	p(NT(25), (NT(24)));
//G201: bf(25)               => variable(158).
	p(NT(25), (NT(158)));
//G202: bf(25)               => uninterpreted_constant(138).
	p(NT(25), (NT(138)));
//G203: __N_0(405)           => 'F'.
	p(NT(405), (T(33)));
//G204: __N_1(406)           => 'T'.
	p(NT(406), (T(32)));
//G205: __N_2(407)           => 'i'.
	p(NT(407), (T(18)));
//G206: __N_3(408)           => 'o'.
	p(NT(408), (T(14)));
//G207: __E_charvar_62(160)  => ~( __N_0(405) ) & ~( __N_1(406) ) & ~( __N_2(407) ) & ~( __N_3(408) ) & alpha(5).	 # conjunctive
	p(NT(160), ~(NT(405)) & ~(NT(406)) & ~(NT(407)) & ~(NT(408)) & (NT(5)));
//G208: __E_charvar_63(161)  => null.
	p(NT(161), (nul));
//G209: __E_charvar_63(161)  => digit(3) __E_charvar_63(161).
	p(NT(161), (NT(3)+NT(161)));
//G210: charvar(159)         => __E_charvar_62(160) __E_charvar_63(161).
	p(NT(159), (NT(160)+NT(161)));
//G211: bool_variable(137)   => '?' charvar(159).
	p(NT(137), (T(26)+NT(159)));
//G212: variable(158)        => charvar(159).
	p(NT(158), (NT(159)));
//G213: __E_variable_64(163) => in(38).
	p(NT(163), (NT(38)));
//G214: __E_variable_64(163) => out(164).
	p(NT(163), (NT(164)));
//G215: io_var(162)          => __E_variable_64(163).
	p(NT(162), (NT(163)));
//G216: variable(158)        => io_var(162).
	p(NT(158), (NT(162)));
//G217: __E_in_65(166)       => 'i' digits(167).
	p(NT(166), (T(18)+NT(167)));
//G218: in_var_name(165)     => __E_in_65(166).
	p(NT(165), (NT(166)));
//G219: in(38)               => in_var_name(165) '[' offset(168) ']'.
	p(NT(38), (NT(165)+T(24)+NT(168)+T(25)));
//G220: __E_out_66(170)      => 'o' digits(167).
	p(NT(170), (T(14)+NT(167)));
//G221: out_var_name(169)    => __E_out_66(170).
	p(NT(169), (NT(170)));
//G222: out(164)             => out_var_name(169) '[' offset(168) ']'.
	p(NT(164), (NT(169)+T(24)+NT(168)+T(25)));
//G223: capture(24)          => '$' charvar(159).
	p(NT(24), (T(39)+NT(159)));
//G224: q_bool_var(98)       => capture(24).
	p(NT(98), (NT(24)));
//G225: q_bool_var(98)       => bool_variable(137).
	p(NT(98), (NT(137)));
//G226: q_var(103)           => capture(24).
	p(NT(103), (NT(24)));
//G227: q_var(103)           => variable(158).
	p(NT(103), (NT(158)));
//G228: __E_offsets_67(171)  => _(15) ',' _(15) offset(168).
	p(NT(171), (NT(15)+T(4)+NT(15)+NT(168)));
//G229: __E_offsets_68(172)  => null.
	p(NT(172), (nul));
//G230: __E_offsets_68(172)  => __E_offsets_67(171) __E_offsets_68(172).
	p(NT(172), (NT(171)+NT(172)));
//G231: offsets(28)          => '[' _(15) offset(168) __E_offsets_68(172) _(15) ']'.
	p(NT(28), (T(24)+NT(15)+NT(168)+NT(172)+NT(15)+T(25)));
//G232: offset(168)          => num(173).
	p(NT(168), (NT(173)));
//G233: offset(168)          => capture(24).
	p(NT(168), (NT(24)));
//G234: offset(168)          => shift(174).
	p(NT(168), (NT(174)));
//G235: __N_4(409)           => io_var(162).
	p(NT(409), (NT(162)));
//G236: __E_offset_69(175)   => variable(158) & ~( __N_4(409) ).	 # conjunctive
	p(NT(175), (NT(158)) & ~(NT(409)));
//G237: offset(168)          => __E_offset_69(175).
	p(NT(168), (NT(175)));
//G238: __E_shift_70(176)    => capture(24).
	p(NT(176), (NT(24)));
//G239: __N_5(410)           => io_var(162).
	p(NT(410), (NT(162)));
//G240: __E___E_shift_70_71(177) => variable(158) & ~( __N_5(410) ).	 # conjunctive
	p(NT(177), (NT(158)) & ~(NT(410)));
//G241: __E_shift_70(176)    => __E___E_shift_70_71(177).
	p(NT(176), (NT(177)));
//G242: shift(174)           => __E_shift_70(176) _(15) '-' _(15) num(173).
	p(NT(174), (NT(176)+NT(15)+T(12)+NT(15)+NT(173)));
//G243: num(173)             => digits(167).
	p(NT(173), (NT(167)));
//G244: __E___E_uninterpreted_constant_72_73(180) => chars(181).
	p(NT(180), (NT(181)));
//G245: __E___E_uninterpreted_constant_72_73(180) => _(15).
	p(NT(180), (NT(15)));
//G246: __E_uninterpreted_constant_72(179) => __E___E_uninterpreted_constant_72_73(180) ':' chars(181).
	p(NT(179), (NT(180)+T(2)+NT(181)));
//G247: uninter_const_name(178) => __E_uninterpreted_constant_72(179).
	p(NT(178), (NT(179)));
//G248: uninterpreted_constant(138) => '<' uninter_const_name(178) _(15) '>'.
	p(NT(138), (T(7)+NT(178)+NT(15)+T(19)));
//G249: constant(155)        => binding(182).
	p(NT(155), (NT(182)));
//G250: constant(155)        => capture(24).
	p(NT(155), (NT(24)));
//G251: __E___E_binding_74_75(185) => chars(181) _(15).
	p(NT(185), (NT(181)+NT(15)));
//G252: __E___E_binding_74_75(185) => _(15).
	p(NT(185), (NT(15)));
//G253: type(184)            => __E___E_binding_74_75(185).
	p(NT(184), (NT(185)));
//G254: __E_binding_74(183)  => type(184) ':' _(15) source(186).
	p(NT(183), (NT(184)+T(2)+NT(15)+NT(186)));
//G255: source_binding(39)   => __E_binding_74(183).
	p(NT(39), (NT(183)));
//G256: binding(182)         => source_binding(39).
	p(NT(182), (NT(39)));
//G257: named_binding(187)   => chars(181).
	p(NT(187), (NT(181)));
//G258: binding(182)         => named_binding(187).
	p(NT(182), (NT(187)));
//G259: __E_source_76(189)   => alnum(6).
	p(NT(189), (NT(6)));
//G260: __E_source_76(189)   => punct(7).
	p(NT(189), (NT(7)));
//G261: src_c(188)           => __E_source_76(189).
	p(NT(188), (NT(189)));
//G262: __E___E_source_77_78(191) => src_c(188).
	p(NT(191), (NT(188)));
//G263: __E___E_source_77_78(191) => space(2).
	p(NT(191), (NT(2)));
//G264: __E___E_source_77_79(192) => null.
	p(NT(192), (nul));
//G265: __E___E_source_77_79(192) => __E___E_source_77_78(191) __E___E_source_77_79(192).
	p(NT(192), (NT(191)+NT(192)));
//G266: __E_source_77(190)   => __E___E_source_77_79(192) src_c(188).
	p(NT(190), (NT(192)+NT(188)));
//G267: __E_source_77(190)   => null.
	p(NT(190), (nul));
//G268: source(186)          => src_c(188) __E_source_77(190).
	p(NT(186), (NT(188)+NT(190)));
//G269: __E_chars_80(193)    => null.
	p(NT(193), (nul));
//G270: __E_chars_80(193)    => alnum(6) __E_chars_80(193).
	p(NT(193), (NT(6)+NT(193)));
//G271: chars(181)           => alpha(5) __E_chars_80(193).
	p(NT(181), (NT(5)+NT(193)));
//G272: __E_digits_81(194)   => digit(3).
	p(NT(194), (NT(3)));
//G273: __E_digits_81(194)   => digit(3) __E_digits_81(194).
	p(NT(194), (NT(3)+NT(194)));
//G274: digits(167)          => __E_digits_81(194).
	p(NT(167), (NT(194)));
//G275: sym(26)              => chars(181).
	p(NT(26), (NT(181)));
//G276: __E_comment_82(196)  => printable(8).
	p(NT(196), (NT(8)));
//G277: __E_comment_82(196)  => '\t'.
	p(NT(196), (T(40)));
//G278: __E_comment_83(197)  => null.
	p(NT(197), (nul));
//G279: __E_comment_83(197)  => __E_comment_82(196) __E_comment_83(197).
	p(NT(197), (NT(196)+NT(197)));
//G280: __E_comment_84(198)  => '\n'.
	p(NT(198), (T(41)));
//G281: __E_comment_84(198)  => '\r'.
	p(NT(198), (T(42)));
//G282: __E_comment_84(198)  => eof(1).
	p(NT(198), (NT(1)));
//G283: comment(195)         => '#' __E_comment_83(197) __E_comment_84(198).
	p(NT(195), (T(43)+NT(197)+NT(198)));
//G284: __E____85(199)       => space(2).
	p(NT(199), (NT(2)));
//G285: __E____85(199)       => comment(195).
	p(NT(199), (NT(195)));
//G286: __(65)               => __E____85(199) _(15).
	p(NT(65), (NT(199)+NT(15)));
//G287: __E___86(200)        => __(65).
	p(NT(200), (NT(65)));
//G288: __E___86(200)        => null.
	p(NT(200), (nul));
//G289: _(15)                => __E___86(200).
	p(NT(15), (NT(200)));
//G290: bf_cb_arg(202)       => bf(25).
	p(NT(202), (NT(25)));
//G291: bf_cb_args1(201)     => __(65) bf_cb_arg(202).
	p(NT(201), (NT(65)+NT(202)));
//G292: bf_cb_args2(203)     => bf_cb_args1(201) bf_cb_args1(201).
	p(NT(203), (NT(201)+NT(201)));
//G293: bf_cb_args3(204)     => bf_cb_args2(203) bf_cb_args1(201).
	p(NT(204), (NT(203)+NT(201)));
//G294: bf_cb_args4(205)     => bf_cb_args3(204) bf_cb_args1(201).
	p(NT(205), (NT(204)+NT(201)));
//G295: wff_cb_arg(207)      => wff(18).
	p(NT(207), (NT(18)));
//G296: wff_cb_args1(206)    => __(65) wff_cb_arg(207).
	p(NT(206), (NT(65)+NT(207)));
//G297: wff_cb_args2(208)    => wff_cb_args1(206) wff_cb_args1(206).
	p(NT(208), (NT(206)+NT(206)));
//G298: wff_cb_args3(209)    => wff_cb_args2(208) wff_cb_args1(206).
	p(NT(209), (NT(208)+NT(206)));
//G299: wff_cb_args4(210)    => wff_cb_args3(209) wff_cb_args1(206).
	p(NT(210), (NT(209)+NT(206)));
//G300: __E_tau_cb_args1_87(213) => capture(24).
	p(NT(213), (NT(24)));
//G301: __E_tau_cb_args1_87(213) => tau(50).
	p(NT(213), (NT(50)));
//G302: tau_cb_arg(212)      => __E_tau_cb_args1_87(213).
	p(NT(212), (NT(213)));
//G303: tau_cb_args1(211)    => __(65) tau_cb_arg(212).
	p(NT(211), (NT(65)+NT(212)));
//G304: tau_cb_args2(214)    => tau_cb_args1(211) tau_cb_args1(211).
	p(NT(214), (NT(211)+NT(211)));
//G305: tau_cb_args3(215)    => tau_cb_args2(214) tau_cb_args1(211).
	p(NT(215), (NT(214)+NT(211)));
//G306: __E___E_tau_cb_88_89(218) => tau_cb_args3(215).
	p(NT(218), (NT(215)));
//G307: __E___E_tau_cb_88_89(218) => tau_cb_args2(214).
	p(NT(218), (NT(214)));
//G308: __E_tau_cb_88(217)   => 't' 'a' 'u' '_' 'c' 'o' 'l' 'l' 'a' 'p' 's' 'e' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'c' 'b' __E___E_tau_cb_88_89(218).
	p(NT(217), (T(17)+T(20)+T(44)+T(28)+T(45)+T(14)+T(21)+T(21)+T(20)+T(46)+T(13)+T(16)+T(28)+T(46)+T(14)+T(13)+T(18)+T(17)+T(18)+T(47)+T(16)+T(13)+T(28)+T(45)+T(27)+NT(218)));
//G309: tau_collapse_positives_cb(216) => __E_tau_cb_88(217).
	p(NT(216), (NT(217)));
//G310: tau_cb(53)           => tau_collapse_positives_cb(216).
	p(NT(53), (NT(216)));
//G311: __E_tau_cb_90(220)   => 't' 'a' 'u' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'u' 'p' 'w' 'a' 'r' 'd' 's' '_' 'c' 'b' tau_cb_args2(214).
	p(NT(220), (T(17)+T(20)+T(44)+T(28)+T(46)+T(14)+T(13)+T(18)+T(17)+T(18)+T(47)+T(16)+T(13)+T(28)+T(44)+T(46)+T(22)+T(20)+T(48)+T(49)+T(13)+T(28)+T(45)+T(27)+NT(214)));
//G312: tau_positives_upwards_cb(219) => __E_tau_cb_90(220).
	p(NT(219), (NT(220)));
//G313: tau_cb(53)           => tau_positives_upwards_cb(219).
	p(NT(53), (NT(219)));
//G314: __E_wff_cb_91(222)   => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b' bf_cb_args1(201) wff_cb_args2(208).
	p(NT(222), (T(27)+T(50)+T(28)+T(16)+T(51)+T(28)+T(45)+T(27)+NT(201)+NT(208)));
//G315: bf_eq_cb(221)        => __E_wff_cb_91(222).
	p(NT(221), (NT(222)));
//G316: wff_cb(57)           => bf_eq_cb(221).
	p(NT(57), (NT(221)));
//G317: __E_wff_cb_92(224)   => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b' bf_cb_args1(201) wff_cb_args2(208).
	p(NT(224), (T(27)+T(50)+T(28)+T(52)+T(16)+T(51)+T(28)+T(45)+T(27)+NT(201)+NT(208)));
//G318: bf_neq_cb(223)       => __E_wff_cb_92(224).
	p(NT(223), (NT(224)));
//G319: wff_cb(57)           => bf_neq_cb(223).
	p(NT(57), (NT(223)));
//G320: __E_wff_cb_93(226)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(208).
	p(NT(226), (T(22)+T(50)+T(50)+T(28)+T(53)+T(20)+T(13)+T(28)+T(45)+T(21)+T(20)+T(13)+T(53)+T(18)+T(52)+T(54)+T(28)+T(13)+T(44)+T(27)+T(50)+T(14)+T(48)+T(15)+T(44)+T(21)+T(20)+T(13)+T(28)+T(45)+T(27)+NT(208)));
//G321: wff_has_clashing_subformulas_cb(225) => __E_wff_cb_93(226).
	p(NT(225), (NT(226)));
//G322: wff_cb(57)           => wff_has_clashing_subformulas_cb(225).
	p(NT(57), (NT(225)));
//G323: __E_wff_cb_94(228)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(209).
	p(NT(228), (T(22)+T(50)+T(50)+T(28)+T(53)+T(20)+T(13)+T(28)+T(13)+T(44)+T(27)+T(50)+T(14)+T(48)+T(15)+T(44)+T(21)+T(20)+T(28)+T(45)+T(27)+NT(209)));
//G324: wff_has_subformula_cb(227) => __E_wff_cb_94(228).
	p(NT(227), (NT(228)));
//G325: wff_cb(57)           => wff_has_subformula_cb(227).
	p(NT(57), (NT(227)));
//G326: __E_wff_cb_95(230)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(208).
	p(NT(230), (T(22)+T(50)+T(50)+T(28)+T(48)+T(16)+T(15)+T(14)+T(47)+T(16)+T(28)+T(16)+T(29)+T(18)+T(13)+T(17)+T(16)+T(52)+T(17)+T(18)+T(20)+T(21)+T(28)+T(45)+T(27)+NT(208)));
//G327: wff_remove_existential_cb(229) => __E_wff_cb_95(230).
	p(NT(229), (NT(230)));
//G328: wff_cb(57)           => wff_remove_existential_cb(229).
	p(NT(57), (NT(229)));
//G329: __E_wff_cb_96(232)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args4(210).
	p(NT(232), (T(22)+T(50)+T(50)+T(28)+T(48)+T(16)+T(15)+T(14)+T(47)+T(16)+T(28)+T(27)+T(16)+T(29)+T(18)+T(13)+T(17)+T(16)+T(52)+T(17)+T(18)+T(20)+T(21)+T(28)+T(45)+T(27)+NT(210)));
//G330: wff_remove_bexistential_cb(231) => __E_wff_cb_96(232).
	p(NT(231), (NT(232)));
//G331: wff_cb(57)           => wff_remove_bexistential_cb(231).
	p(NT(57), (NT(231)));
//G332: __E_wff_cb_97(234)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' wff_cb_args4(210).
	p(NT(234), (T(22)+T(50)+T(50)+T(28)+T(48)+T(16)+T(15)+T(14)+T(47)+T(16)+T(28)+T(27)+T(44)+T(52)+T(18)+T(47)+T(16)+T(48)+T(13)+T(20)+T(21)+T(28)+T(45)+T(27)+NT(210)));
//G333: wff_remove_buniversal_cb(233) => __E_wff_cb_97(234).
	p(NT(233), (NT(234)));
//G334: wff_cb(57)           => wff_remove_buniversal_cb(233).
	p(NT(57), (NT(233)));
//G335: __E_bf_cb_98(236)    => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b' bf_cb_args2(203).
	p(NT(236), (T(27)+T(50)+T(28)+T(18)+T(13)+T(28)+T(55)+T(16)+T(48)+T(14)+T(28)+T(45)+T(27)+NT(203)));
//G336: bf_is_zero_cb(235)   => __E_bf_cb_98(236).
	p(NT(235), (NT(236)));
//G337: bf_cb(61)            => bf_is_zero_cb(235).
	p(NT(61), (NT(235)));
//G338: __E_bf_cb_99(238)    => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b' bf_cb_args2(203).
	p(NT(238), (T(27)+T(50)+T(28)+T(18)+T(13)+T(28)+T(14)+T(52)+T(16)+T(28)+T(45)+T(27)+NT(203)));
//G339: bf_is_one_cb(237)    => __E_bf_cb_99(238).
	p(NT(237), (NT(238)));
//G340: bf_cb(61)            => bf_is_one_cb(237).
	p(NT(61), (NT(237)));
//G341: __E_bf_cb_100(240)   => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(204).
	p(NT(240), (T(27)+T(50)+T(28)+T(53)+T(20)+T(13)+T(28)+T(13)+T(44)+T(27)+T(50)+T(14)+T(48)+T(15)+T(44)+T(21)+T(20)+T(28)+T(45)+T(27)+NT(204)));
//G342: bf_has_subformula_cb(239) => __E_bf_cb_100(240).
	p(NT(239), (NT(240)));
//G343: bf_cb(61)            => bf_has_subformula_cb(239).
	p(NT(61), (NT(239)));
//G344: __E_bf_cb_101(242)   => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(205).
	p(NT(242), (T(27)+T(50)+T(28)+T(48)+T(16)+T(15)+T(14)+T(47)+T(16)+T(28)+T(50)+T(44)+T(52)+T(18)+T(47)+T(16)+T(48)+T(13)+T(20)+T(21)+T(28)+T(45)+T(27)+NT(205)));
//G345: bf_remove_funiversal_cb(241) => __E_bf_cb_101(242).
	p(NT(241), (NT(242)));
//G346: bf_cb(61)            => bf_remove_funiversal_cb(241).
	p(NT(61), (NT(241)));
//G347: __E_bf_cb_102(244)   => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(205).
	p(NT(244), (T(27)+T(50)+T(28)+T(48)+T(16)+T(15)+T(14)+T(47)+T(16)+T(28)+T(50)+T(16)+T(29)+T(18)+T(13)+T(17)+T(16)+T(52)+T(17)+T(18)+T(20)+T(21)+T(28)+T(45)+T(27)+NT(205)));
//G348: bf_remove_fexistential_cb(243) => __E_bf_cb_102(244).
	p(NT(243), (NT(244)));
//G349: bf_cb(61)            => bf_remove_fexistential_cb(243).
	p(NT(61), (NT(243)));
//G350: __E_bf_cb_103(246)   => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b' bf_cb_args2(203).
	p(NT(246), (T(27)+T(50)+T(28)+T(20)+T(52)+T(49)+T(28)+T(45)+T(27)+NT(203)));
//G351: bf_and_cb(245)       => __E_bf_cb_103(246).
	p(NT(245), (NT(246)));
//G352: bf_cb(61)            => bf_and_cb(245).
	p(NT(61), (NT(245)));
//G353: __E_bf_cb_104(248)   => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b' bf_cb_args2(203).
	p(NT(248), (T(27)+T(50)+T(28)+T(14)+T(48)+T(28)+T(45)+T(27)+NT(203)));
//G354: bf_or_cb(247)        => __E_bf_cb_104(248).
	p(NT(247), (NT(248)));
//G355: bf_cb(61)            => bf_or_cb(247).
	p(NT(61), (NT(247)));
//G356: __E_bf_cb_105(250)   => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b' bf_cb_args2(203).
	p(NT(250), (T(27)+T(50)+T(28)+T(29)+T(14)+T(48)+T(28)+T(45)+T(27)+NT(203)));
//G357: bf_xor_cb(249)       => __E_bf_cb_105(250).
	p(NT(249), (NT(250)));
//G358: bf_cb(61)            => bf_xor_cb(249).
	p(NT(61), (NT(249)));
//G359: __E_bf_cb_106(252)   => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b' bf_cb_args1(201).
	p(NT(252), (T(27)+T(50)+T(28)+T(52)+T(16)+T(54)+T(28)+T(45)+T(27)+NT(201)));
//G360: bf_neg_cb(251)       => __E_bf_cb_106(252).
	p(NT(251), (NT(252)));
//G361: bf_cb(61)            => bf_neg_cb(251).
	p(NT(61), (NT(251)));
//G362: __E_bf_cb_107(254)   => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(201).
	p(NT(254), (T(27)+T(50)+T(28)+T(52)+T(14)+T(48)+T(15)+T(20)+T(21)+T(18)+T(55)+T(16)+T(28)+T(45)+T(27)+NT(201)));
//G363: bf_normalize_cb(253) => __E_bf_cb_107(254).
	p(NT(253), (NT(254)));
//G364: bf_cb(61)            => bf_normalize_cb(253).
	p(NT(61), (NT(253)));
//G365: cli(255)             => _(15).
	p(NT(255), (NT(15)));
//G366: __E_cli_108(257)     => '.' _(15) cli_command(256) _(15).
	p(NT(257), (T(1)+NT(15)+NT(256)+NT(15)));
//G367: __E_cli_109(258)     => null.
	p(NT(258), (nul));
//G368: __E_cli_109(258)     => __E_cli_108(257) __E_cli_109(258).
	p(NT(258), (NT(257)+NT(258)));
//G369: cli(255)             => _(15) cli_command(256) _(15) __E_cli_109(258).
	p(NT(255), (NT(15)+NT(256)+NT(15)+NT(258)));
//G370: __E_cli_command_110(261) => 'q'.
	p(NT(261), (T(51)));
//G371: __E_cli_command_110(261) => 'q' 'u' 'i' 't'.
	p(NT(261), (T(51)+T(44)+T(18)+T(17)));
//G372: quit_sym(260)        => __E_cli_command_110(261).
	p(NT(260), (NT(261)));
//G373: quit_cmd(259)        => quit_sym(260).
	p(NT(259), (NT(260)));
//G374: cli_command(256)     => quit_cmd(259).
	p(NT(256), (NT(259)));
//G375: __E_cli_command_111(264) => 'v'.
	p(NT(264), (T(47)));
//G376: __E_cli_command_111(264) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(264), (T(47)+T(16)+T(48)+T(13)+T(18)+T(14)+T(52)));
//G377: version_sym(263)     => __E_cli_command_111(264).
	p(NT(263), (NT(264)));
//G378: version_cmd(262)     => version_sym(263).
	p(NT(262), (NT(263)));
//G379: cli_command(256)     => version_cmd(262).
	p(NT(256), (NT(262)));
//G380: __E_cli_command_112(267) => 'c'.
	p(NT(267), (T(45)));
//G381: __E_cli_command_112(267) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(267), (T(45)+T(21)+T(16)+T(20)+T(48)));
//G382: clear_sym(266)       => __E_cli_command_112(267).
	p(NT(266), (NT(267)));
//G383: clear_cmd(265)       => clear_sym(266).
	p(NT(265), (NT(266)));
//G384: cli_command(256)     => clear_cmd(265).
	p(NT(256), (NT(265)));
//G385: __E___E_cli_command_113_114(271) => 'h'.
	p(NT(271), (T(53)));
//G386: __E___E_cli_command_113_114(271) => 'h' 'e' 'l' 'p'.
	p(NT(271), (T(53)+T(16)+T(21)+T(46)));
//G387: help_sym(270)        => __E___E_cli_command_113_114(271).
	p(NT(270), (NT(271)));
//G388: __E___E_cli_command_113_115(272) => __(65) help_arg(273).
	p(NT(272), (NT(65)+NT(273)));
//G389: __E___E_cli_command_113_115(272) => null.
	p(NT(272), (nul));
//G390: __E_cli_command_113(269) => help_sym(270) __E___E_cli_command_113_115(272).
	p(NT(269), (NT(270)+NT(272)));
//G391: help_cmd(268)        => __E_cli_command_113(269).
	p(NT(268), (NT(269)));
//G392: cli_command(256)     => help_cmd(268).
	p(NT(256), (NT(268)));
//G393: file_sym(276)        => 'f' 'i' 'l' 'e'.
	p(NT(276), (T(50)+T(18)+T(21)+T(16)));
//G394: __E_cli_command_116(275) => file_sym(276) __(65) q_string(277).
	p(NT(275), (NT(276)+NT(65)+NT(277)));
//G395: file_cmd(274)        => __E_cli_command_116(275).
	p(NT(274), (NT(275)));
//G396: cli_command(256)     => file_cmd(274).
	p(NT(256), (NT(274)));
//G397: valid_sym(280)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(280), (T(47)+T(20)+T(21)+T(18)+T(49)));
//G398: __E_cli_command_117(279) => valid_sym(280) __(65) rr(14).
	p(NT(279), (NT(280)+NT(65)+NT(14)));
//G399: valid_cmd(278)       => __E_cli_command_117(279).
	p(NT(278), (NT(279)));
//G400: cli_command(256)     => valid_cmd(278).
	p(NT(256), (NT(278)));
//G401: sat_sym(283)         => 's' 'a' 't'.
	p(NT(283), (T(13)+T(20)+T(17)));
//G402: __E_cli_command_118(282) => sat_sym(283) __(65) rr(14).
	p(NT(282), (NT(283)+NT(65)+NT(14)));
//G403: sat_cmd(281)         => __E_cli_command_118(282).
	p(NT(281), (NT(282)));
//G404: cli_command(256)     => sat_cmd(281).
	p(NT(256), (NT(281)));
//G405: unsat_sym(286)       => 'u' 'n' 's' 'a' 't'.
	p(NT(286), (T(44)+T(52)+T(13)+T(20)+T(17)));
//G406: __E_cli_command_119(285) => unsat_sym(286) __(65) rr(14).
	p(NT(285), (NT(286)+NT(65)+NT(14)));
//G407: unsat_cmd(284)       => __E_cli_command_119(285).
	p(NT(284), (NT(285)));
//G408: cli_command(256)     => unsat_cmd(284).
	p(NT(256), (NT(284)));
//G409: solve_sym(289)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(289), (T(13)+T(14)+T(21)+T(47)+T(16)));
//G410: __E_cli_command_120(288) => solve_sym(289) __(65) wff_cmd_arg(290).
	p(NT(288), (NT(289)+NT(65)+NT(290)));
//G411: solve_cmd(287)       => __E_cli_command_120(288).
	p(NT(287), (NT(288)));
//G412: cli_command(256)     => solve_cmd(287).
	p(NT(256), (NT(287)));
//G413: __E___E_cli_command_121_122(294) => 'e'.
	p(NT(294), (T(16)));
//G414: __E___E_cli_command_121_122(294) => 'e' 'x' 'e' 'c' 'u' 't' 'e'.
	p(NT(294), (T(16)+T(29)+T(16)+T(45)+T(44)+T(17)+T(16)));
//G415: execute_sym(293)     => __E___E_cli_command_121_122(294).
	p(NT(293), (NT(294)));
//G416: __E___E_cli_command_121_123(295) => wff_typed(296).
	p(NT(295), (NT(296)));
//G417: __E___E_cli_command_121_123(295) => rr(14).
	p(NT(295), (NT(14)));
//G418: __E_cli_command_121(292) => execute_sym(293) __(65) __E___E_cli_command_121_123(295).
	p(NT(292), (NT(293)+NT(65)+NT(295)));
//G419: execute_cmd(291)     => __E_cli_command_121(292).
	p(NT(291), (NT(292)));
//G420: cli_command(256)     => execute_cmd(291).
	p(NT(256), (NT(291)));
//G421: __E___E_cli_command_124_125(300) => 'n'.
	p(NT(300), (T(52)));
//G422: __E___E_cli_command_124_125(300) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(300), (T(52)+T(14)+T(48)+T(15)+T(20)+T(21)+T(18)+T(55)+T(16)));
//G423: normalize_sym(299)   => __E___E_cli_command_124_125(300).
	p(NT(299), (NT(300)));
//G424: __E_cli_command_124(298) => normalize_sym(299) __(65) normalize_cmd_arg(301).
	p(NT(298), (NT(299)+NT(65)+NT(301)));
//G425: normalize_cmd(297)   => __E_cli_command_124(298).
	p(NT(297), (NT(298)));
//G426: cli_command(256)     => normalize_cmd(297).
	p(NT(256), (NT(297)));
//G427: __E___E_cli_command_126_127(305) => 's'.
	p(NT(305), (T(13)));
//G428: __E___E_cli_command_126_127(305) => 's' 'u' 'b' 's' 't'.
	p(NT(305), (T(13)+T(44)+T(27)+T(13)+T(17)));
//G429: __E___E_cli_command_126_127(305) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(305), (T(13)+T(44)+T(27)+T(13)+T(17)+T(18)+T(17)+T(44)+T(17)+T(16)));
//G430: subst_sym(304)       => __E___E_cli_command_126_127(305).
	p(NT(304), (NT(305)));
//G431: __E_cli_command_126(303) => subst_sym(304) __(65) nf_cmd_arg(306) _(15) '[' _(15) nf_cmd_arg(306) _(15) '/' _(15) nf_cmd_arg(306) _(15) ']'.
	p(NT(303), (NT(304)+NT(65)+NT(306)+NT(15)+T(24)+NT(15)+NT(306)+NT(15)+T(56)+NT(15)+NT(306)+NT(15)+T(25)));
//G432: subst_cmd(302)       => __E_cli_command_126(303).
	p(NT(302), (NT(303)));
//G433: cli_command(256)     => subst_cmd(302).
	p(NT(256), (NT(302)));
//G434: __E___E_cli_command_128_129(310) => 'i'.
	p(NT(310), (T(18)));
//G435: __E___E_cli_command_128_129(310) => 'i' 'n' 's' 't'.
	p(NT(310), (T(18)+T(52)+T(13)+T(17)));
//G436: __E___E_cli_command_128_129(310) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(310), (T(18)+T(52)+T(13)+T(17)+T(20)+T(52)+T(17)+T(18)+T(20)+T(17)+T(16)));
//G437: inst_sym(309)        => __E___E_cli_command_128_129(310).
	p(NT(309), (NT(310)));
//G438: __E_cli_command_128(308) => inst_sym(309) __(65) inst_args(311).
	p(NT(308), (NT(309)+NT(65)+NT(311)));
//G439: inst_cmd(307)        => __E_cli_command_128(308).
	p(NT(307), (NT(308)));
//G440: cli_command(256)     => inst_cmd(307).
	p(NT(256), (NT(307)));
//G441: dnf_sym(314)         => 'd' 'n' 'f'.
	p(NT(314), (T(49)+T(52)+T(50)));
//G442: __E_cli_command_130(313) => dnf_sym(314) __(65) nf_cmd_arg(306).
	p(NT(313), (NT(314)+NT(65)+NT(306)));
//G443: dnf_cmd(312)         => __E_cli_command_130(313).
	p(NT(312), (NT(313)));
//G444: cli_command(256)     => dnf_cmd(312).
	p(NT(256), (NT(312)));
//G445: cnf_sym(317)         => 'c' 'n' 'f'.
	p(NT(317), (T(45)+T(52)+T(50)));
//G446: __E_cli_command_131(316) => cnf_sym(317) __(65) nf_cmd_arg(306).
	p(NT(316), (NT(317)+NT(65)+NT(306)));
//G447: cnf_cmd(315)         => __E_cli_command_131(316).
	p(NT(315), (NT(316)));
//G448: cli_command(256)     => cnf_cmd(315).
	p(NT(256), (NT(315)));
//G449: anf_sym(320)         => 'a' 'n' 'f'.
	p(NT(320), (T(20)+T(52)+T(50)));
//G450: __E_cli_command_132(319) => anf_sym(320) __(65) nf_cmd_arg(306).
	p(NT(319), (NT(320)+NT(65)+NT(306)));
//G451: anf_cmd(318)         => __E_cli_command_132(319).
	p(NT(318), (NT(319)));
//G452: cli_command(256)     => anf_cmd(318).
	p(NT(256), (NT(318)));
//G453: nnf_sym(323)         => 'n' 'n' 'f'.
	p(NT(323), (T(52)+T(52)+T(50)));
//G454: __E_cli_command_133(322) => nnf_sym(323) __(65) nf_cmd_arg(306).
	p(NT(322), (NT(323)+NT(65)+NT(306)));
//G455: nnf_cmd(321)         => __E_cli_command_133(322).
	p(NT(321), (NT(322)));
//G456: cli_command(256)     => nnf_cmd(321).
	p(NT(256), (NT(321)));
//G457: pnf_sym(326)         => 'p' 'n' 'f'.
	p(NT(326), (T(46)+T(52)+T(50)));
//G458: __E_cli_command_134(325) => pnf_sym(326) __(65) nf_cmd_arg(306).
	p(NT(325), (NT(326)+NT(65)+NT(306)));
//G459: pnf_cmd(324)         => __E_cli_command_134(325).
	p(NT(324), (NT(325)));
//G460: cli_command(256)     => pnf_cmd(324).
	p(NT(256), (NT(324)));
//G461: mnf_sym(329)         => 'm' 'n' 'f'.
	p(NT(329), (T(15)+T(52)+T(50)));
//G462: __E_cli_command_135(328) => mnf_sym(329) __(65) nf_cmd_arg(306).
	p(NT(328), (NT(329)+NT(65)+NT(306)));
//G463: mnf_cmd(327)         => __E_cli_command_135(328).
	p(NT(327), (NT(328)));
//G464: cli_command(256)     => mnf_cmd(327).
	p(NT(256), (NT(327)));
//G465: snf_sym(332)         => 's' 'n' 'f'.
	p(NT(332), (T(13)+T(52)+T(50)));
//G466: __E_cli_command_136(331) => snf_sym(332) __(65) nf_cmd_arg(306).
	p(NT(331), (NT(332)+NT(65)+NT(306)));
//G467: snf_cmd(330)         => __E_cli_command_136(331).
	p(NT(330), (NT(331)));
//G468: cli_command(256)     => snf_cmd(330).
	p(NT(256), (NT(330)));
//G469: onf_sym(335)         => 'o' 'n' 'f'.
	p(NT(335), (T(14)+T(52)+T(50)));
//G470: __E_cli_command_137(334) => onf_sym(335) __(65) variable(158) __(65) onf_cmd_arg(336).
	p(NT(334), (NT(335)+NT(65)+NT(158)+NT(65)+NT(336)));
//G471: onf_cmd(333)         => __E_cli_command_137(334).
	p(NT(333), (NT(334)));
//G472: cli_command(256)     => onf_cmd(333).
	p(NT(256), (NT(333)));
//G473: __E___E_cli_command_138_139(340) => 'd' 'e' 'f' 's'.
	p(NT(340), (T(49)+T(16)+T(50)+T(13)));
//G474: __E___E_cli_command_138_139(340) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(340), (T(49)+T(16)+T(50)+T(18)+T(52)+T(18)+T(17)+T(18)+T(14)+T(52)+T(13)));
//G475: def_sym(339)         => __E___E_cli_command_138_139(340).
	p(NT(339), (NT(340)));
//G476: __E_cli_command_138(338) => def_sym(339).
	p(NT(338), (NT(339)));
//G477: def_list_cmd(337)    => __E_cli_command_138(338).
	p(NT(337), (NT(338)));
//G478: cli_command(256)     => def_list_cmd(337).
	p(NT(256), (NT(337)));
//G479: __E_cli_command_140(342) => def_sym(339) __(65) number(343).
	p(NT(342), (NT(339)+NT(65)+NT(343)));
//G480: def_print_cmd(341)   => __E_cli_command_140(342).
	p(NT(341), (NT(342)));
//G481: cli_command(256)     => def_print_cmd(341).
	p(NT(256), (NT(341)));
//G482: def_rr_cmd(344)      => rec_relation(20).
	p(NT(344), (NT(20)));
//G483: cli_command(256)     => def_rr_cmd(344).
	p(NT(256), (NT(344)));
//G484: __E___E_cli_command_141_142(348) => 'h' 'i' 's' 't'.
	p(NT(348), (T(53)+T(18)+T(13)+T(17)));
//G485: __E___E_cli_command_141_142(348) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(348), (T(53)+T(18)+T(13)+T(17)+T(14)+T(48)+T(23)));
//G486: history_sym(347)     => __E___E_cli_command_141_142(348).
	p(NT(347), (NT(348)));
//G487: __E_cli_command_141(346) => history_sym(347).
	p(NT(346), (NT(347)));
//G488: history_list_cmd(345) => __E_cli_command_141(346).
	p(NT(345), (NT(346)));
//G489: cli_command(256)     => history_list_cmd(345).
	p(NT(256), (NT(345)));
//G490: __E_cli_command_143(350) => history_sym(347) __(65) memory(351).
	p(NT(350), (NT(347)+NT(65)+NT(351)));
//G491: history_print_cmd(349) => __E_cli_command_143(350).
	p(NT(349), (NT(350)));
//G492: cli_command(256)     => history_print_cmd(349).
	p(NT(256), (NT(349)));
//G493: __E_cli_command_144(353) => bf_typed(354).
	p(NT(353), (NT(354)));
//G494: __E_cli_command_144(353) => wff_typed(296).
	p(NT(353), (NT(296)));
//G495: history_store_cmd(352) => __E_cli_command_144(353).
	p(NT(352), (NT(353)));
//G496: cli_command(256)     => history_store_cmd(352).
	p(NT(256), (NT(352)));
//G497: qelim_sym(357)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(357), (T(51)+T(16)+T(21)+T(18)+T(15)));
//G498: __E_cli_command_145(356) => qelim_sym(357) __(65) wff_cmd_arg(290).
	p(NT(356), (NT(357)+NT(65)+NT(290)));
//G499: qelim_cmd(355)       => __E_cli_command_145(356).
	p(NT(355), (NT(356)));
//G500: cli_command(256)     => qelim_cmd(355).
	p(NT(256), (NT(355)));
//G501: get_sym(360)         => 'g' 'e' 't'.
	p(NT(360), (T(54)+T(16)+T(17)));
//G502: __E___E_cli_command_146_147(361) => __(65) option(362).
	p(NT(361), (NT(65)+NT(362)));
//G503: __E___E_cli_command_146_147(361) => null.
	p(NT(361), (nul));
//G504: __E_cli_command_146(359) => get_sym(360) __E___E_cli_command_146_147(361).
	p(NT(359), (NT(360)+NT(361)));
//G505: get_cmd(358)         => __E_cli_command_146(359).
	p(NT(358), (NT(359)));
//G506: cli_command(256)     => get_cmd(358).
	p(NT(256), (NT(358)));
//G507: set_sym(365)         => 's' 'e' 't'.
	p(NT(365), (T(13)+T(16)+T(17)));
//G508: __E___E_cli_command_148_149(366) => __(65).
	p(NT(366), (NT(65)));
//G509: __E___E_cli_command_148_149(366) => _(15) '=' _(15).
	p(NT(366), (NT(15)+T(3)+NT(15)));
//G510: __E_cli_command_148(364) => set_sym(365) __(65) option(362) __E___E_cli_command_148_149(366) option_value(367).
	p(NT(364), (NT(365)+NT(65)+NT(362)+NT(366)+NT(367)));
//G511: set_cmd(363)         => __E_cli_command_148(364).
	p(NT(363), (NT(364)));
//G512: cli_command(256)     => set_cmd(363).
	p(NT(256), (NT(363)));
//G513: toggle_sym(370)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(370), (T(17)+T(14)+T(54)+T(54)+T(21)+T(16)));
//G514: __E_cli_command_150(369) => toggle_sym(370) __(65) bool_option(371).
	p(NT(369), (NT(370)+NT(65)+NT(371)));
//G515: toggle_cmd(368)      => __E_cli_command_150(369).
	p(NT(368), (NT(369)));
//G516: cli_command(256)     => toggle_cmd(368).
	p(NT(256), (NT(368)));
//G517: number(343)          => digits(167).
	p(NT(343), (NT(167)));
//G518: bf_typed(354)        => bf(25).
	p(NT(354), (NT(25)));
//G519: bf_typed(354)        => bf(25) _(15) ':' _(15) 'b' 'f'.
	p(NT(354), (NT(25)+NT(15)+T(2)+NT(15)+T(27)+T(50)));
//G520: wff_typed(296)       => wff(18).
	p(NT(296), (NT(18)));
//G521: wff_typed(296)       => wff(18) _(15) ':' _(15) 'w' 'f' 'f'.
	p(NT(296), (NT(18)+NT(15)+T(2)+NT(15)+T(22)+T(50)+T(50)));
//G522: bf_cmd_arg(372)      => memory(351).
	p(NT(372), (NT(351)));
//G523: bf_cmd_arg(372)      => bf_typed(354).
	p(NT(372), (NT(354)));
//G524: wff_cmd_arg(290)     => memory(351).
	p(NT(290), (NT(351)));
//G525: wff_cmd_arg(290)     => wff_typed(296).
	p(NT(290), (NT(296)));
//G526: nf_cmd_arg(306)      => memory(351).
	p(NT(306), (NT(351)));
//G527: nf_cmd_arg(306)      => bf_typed(354).
	p(NT(306), (NT(354)));
//G528: nf_cmd_arg(306)      => wff_typed(296).
	p(NT(306), (NT(296)));
//G529: onf_cmd_arg(336)     => memory(351).
	p(NT(336), (NT(351)));
//G530: onf_cmd_arg(336)     => wff_typed(296).
	p(NT(336), (NT(296)));
//G531: normalize_cmd_arg(301) => memory(351).
	p(NT(301), (NT(351)));
//G532: normalize_cmd_arg(301) => bf_typed(354).
	p(NT(301), (NT(354)));
//G533: normalize_cmd_arg(301) => wff_typed(296).
	p(NT(301), (NT(296)));
//G534: normalize_cmd_arg(301) => rr(14).
	p(NT(301), (NT(14)));
//G535: inst_args(311)       => bf_cmd_arg(372) _(15) '[' _(15) variable(158) _(15) '/' _(15) bf_cmd_arg(372) _(15) ']'.
	p(NT(311), (NT(372)+NT(15)+T(24)+NT(15)+NT(158)+NT(15)+T(56)+NT(15)+NT(372)+NT(15)+T(25)));
//G536: inst_args(311)       => wff_cmd_arg(290) _(15) '[' _(15) variable(158) _(15) '/' _(15) bf_cmd_arg(372) _(15) ']'.
	p(NT(311), (NT(290)+NT(15)+T(24)+NT(15)+NT(158)+NT(15)+T(56)+NT(15)+NT(372)+NT(15)+T(25)));
//G537: inst_args(311)       => wff_cmd_arg(290) _(15) '[' _(15) bool_variable(137) _(15) '/' _(15) wff_cmd_arg(290) _(15) ']'.
	p(NT(311), (NT(290)+NT(15)+T(24)+NT(15)+NT(137)+NT(15)+T(56)+NT(15)+NT(290)+NT(15)+T(25)));
//G538: help_arg(273)        => help_sym(270).
	p(NT(273), (NT(270)));
//G539: help_arg(273)        => version_sym(263).
	p(NT(273), (NT(263)));
//G540: help_arg(273)        => quit_sym(260).
	p(NT(273), (NT(260)));
//G541: help_arg(273)        => clear_sym(266).
	p(NT(273), (NT(266)));
//G542: help_arg(273)        => get_sym(360).
	p(NT(273), (NT(360)));
//G543: help_arg(273)        => set_sym(365).
	p(NT(273), (NT(365)));
//G544: help_arg(273)        => toggle_sym(370).
	p(NT(273), (NT(370)));
//G545: help_arg(273)        => file_sym(276).
	p(NT(273), (NT(276)));
//G546: help_arg(273)        => history_sym(347).
	p(NT(273), (NT(347)));
//G547: help_arg(273)        => abs_memory_sym(373).
	p(NT(273), (NT(373)));
//G548: help_arg(273)        => rel_memory_sym(374).
	p(NT(273), (NT(374)));
//G549: help_arg(273)        => selection_sym(375).
	p(NT(273), (NT(375)));
//G550: help_arg(273)        => def_sym(339).
	p(NT(273), (NT(339)));
//G551: help_arg(273)        => inst_sym(309).
	p(NT(273), (NT(309)));
//G552: help_arg(273)        => subst_sym(304).
	p(NT(273), (NT(304)));
//G553: help_arg(273)        => normalize_sym(299).
	p(NT(273), (NT(299)));
//G554: help_arg(273)        => execute_sym(293).
	p(NT(273), (NT(293)));
//G555: help_arg(273)        => solve_sym(289).
	p(NT(273), (NT(289)));
//G556: help_arg(273)        => valid_sym(280).
	p(NT(273), (NT(280)));
//G557: help_arg(273)        => sat_sym(283).
	p(NT(273), (NT(283)));
//G558: help_arg(273)        => unsat_sym(286).
	p(NT(273), (NT(286)));
//G559: help_arg(273)        => dnf_sym(314).
	p(NT(273), (NT(314)));
//G560: help_arg(273)        => cnf_sym(317).
	p(NT(273), (NT(317)));
//G561: help_arg(273)        => anf_sym(320).
	p(NT(273), (NT(320)));
//G562: help_arg(273)        => snf_sym(332).
	p(NT(273), (NT(332)));
//G563: help_arg(273)        => nnf_sym(323).
	p(NT(273), (NT(323)));
//G564: help_arg(273)        => pnf_sym(326).
	p(NT(273), (NT(326)));
//G565: help_arg(273)        => mnf_sym(329).
	p(NT(273), (NT(329)));
//G566: help_arg(273)        => onf_sym(335).
	p(NT(273), (NT(335)));
//G567: help_arg(273)        => qelim_sym(357).
	p(NT(273), (NT(357)));
//G568: __E___E_help_arg_151_152(378) => 's'.
	p(NT(378), (T(13)));
//G569: __E___E_help_arg_151_152(378) => null.
	p(NT(378), (nul));
//G570: __E_help_arg_151(377) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_151_152(378).
	p(NT(377), (T(16)+T(29)+T(20)+T(15)+T(46)+T(21)+T(16)+NT(378)));
//G571: examples_sym(376)    => __E_help_arg_151(377).
	p(NT(376), (NT(377)));
//G572: help_arg(273)        => examples_sym(376).
	p(NT(273), (NT(376)));
//G573: __E_memory_153(380)  => '%'.
	p(NT(380), (T(57)));
//G574: rel_memory_sym(374)  => '%' '-'.
	p(NT(374), (T(57)+T(12)));
//G575: memory_id(382)       => digits(167).
	p(NT(382), (NT(167)));
//G576: __E___E_memory_153_154(381) => memory_id(382).
	p(NT(381), (NT(382)));
//G577: __E___E_memory_153_154(381) => null.
	p(NT(381), (nul));
//G578: __E_memory_153(380)  => rel_memory_sym(374) __E___E_memory_153_154(381).
	p(NT(380), (NT(374)+NT(381)));
//G579: rel_memory(379)      => __E_memory_153(380).
	p(NT(379), (NT(380)));
//G580: rel_memory(379)      => rel_memory(379).
	p(NT(379), (NT(379)));
//G581: memory(351)          => rel_memory(379).
	p(NT(351), (NT(379)));
//G582: abs_memory_sym(373)  => '%'.
	p(NT(373), (T(57)));
//G583: __E_memory_155(384)  => abs_memory_sym(373) memory_id(382).
	p(NT(384), (NT(373)+NT(382)));
//G584: abs_memory(383)      => __E_memory_155(384).
	p(NT(383), (NT(384)));
//G585: memory(351)          => abs_memory(383).
	p(NT(351), (NT(383)));
//G586: option(362)          => bool_option(371).
	p(NT(362), (NT(371)));
//G587: __E_option_156(387)  => 's' 'e' 'v'.
	p(NT(387), (T(13)+T(16)+T(47)));
//G588: __E_option_156(387)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(387), (T(13)+T(16)+T(47)+T(16)+T(48)+T(18)+T(17)+T(23)));
//G589: severity_opt(386)    => __E_option_156(387).
	p(NT(386), (NT(387)));
//G590: enum_option(385)     => severity_opt(386).
	p(NT(385), (NT(386)));
//G591: option(362)          => enum_option(385).
	p(NT(362), (NT(385)));
//G592: __E_bool_option_157(389) => 's'.
	p(NT(389), (T(13)));
//G593: __E_bool_option_157(389) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(389), (T(13)+T(17)+T(20)+T(17)+T(44)+T(13)));
//G594: status_opt(388)      => __E_bool_option_157(389).
	p(NT(388), (NT(389)));
//G595: bool_option(371)     => status_opt(388).
	p(NT(371), (NT(388)));
//G596: __E_bool_option_158(391) => 'c'.
	p(NT(391), (T(45)));
//G597: __E_bool_option_158(391) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(391), (T(45)+T(14)+T(21)+T(14)+T(48)));
//G598: __E_bool_option_158(391) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(391), (T(45)+T(14)+T(21)+T(14)+T(48)+T(13)));
//G599: colors_opt(390)      => __E_bool_option_158(391).
	p(NT(390), (NT(391)));
//G600: bool_option(371)     => colors_opt(390).
	p(NT(371), (NT(390)));
//G601: __E_bool_option_159(393) => 'd'.
	p(NT(393), (T(49)));
//G602: __E_bool_option_159(393) => 'd' 'b' 'g'.
	p(NT(393), (T(49)+T(27)+T(54)));
//G603: __E_bool_option_159(393) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(393), (T(49)+T(16)+T(27)+T(44)+T(54)));
//G604: debug_repl_opt(392)  => __E_bool_option_159(393).
	p(NT(392), (NT(393)));
//G605: bool_option(371)     => debug_repl_opt(392).
	p(NT(371), (NT(392)));
//G606: option_value(367)    => option_value_true(394).
	p(NT(367), (NT(394)));
//G607: option_value(367)    => option_value_false(395).
	p(NT(367), (NT(395)));
//G608: option_value(367)    => severity(396).
	p(NT(367), (NT(396)));
//G609: option_value_true(394) => 't'.
	p(NT(394), (T(17)));
//G610: option_value_true(394) => 't' 'r' 'u' 'e'.
	p(NT(394), (T(17)+T(48)+T(44)+T(16)));
//G611: option_value_true(394) => 'o' 'n'.
	p(NT(394), (T(14)+T(52)));
//G612: option_value_true(394) => '1'.
	p(NT(394), (T(37)));
//G613: option_value_true(394) => 'y'.
	p(NT(394), (T(23)));
//G614: option_value_true(394) => 'y' 'e' 's'.
	p(NT(394), (T(23)+T(16)+T(13)));
//G615: option_value_false(395) => 'f'.
	p(NT(395), (T(50)));
//G616: option_value_false(395) => 'f' 'a' 'l' 's' 'e'.
	p(NT(395), (T(50)+T(20)+T(21)+T(13)+T(16)));
//G617: option_value_false(395) => 'o' 'f' 'f'.
	p(NT(395), (T(14)+T(50)+T(50)));
//G618: option_value_false(395) => '0'.
	p(NT(395), (T(38)));
//G619: option_value_false(395) => 'n'.
	p(NT(395), (T(52)));
//G620: option_value_false(395) => 'n' 'o'.
	p(NT(395), (T(52)+T(14)));
//G621: __E_severity_160(398) => 'e'.
	p(NT(398), (T(16)));
//G622: __E_severity_160(398) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(398), (T(16)+T(48)+T(48)+T(14)+T(48)));
//G623: error_sym(397)       => __E_severity_160(398).
	p(NT(397), (NT(398)));
//G624: severity(396)        => error_sym(397).
	p(NT(396), (NT(397)));
//G625: __E_severity_161(400) => 'i'.
	p(NT(400), (T(18)));
//G626: __E_severity_161(400) => 'i' 'n' 'f' 'o'.
	p(NT(400), (T(18)+T(52)+T(50)+T(14)));
//G627: info_sym(399)        => __E_severity_161(400).
	p(NT(399), (NT(400)));
//G628: severity(396)        => info_sym(399).
	p(NT(396), (NT(399)));
//G629: __E_severity_162(402) => 'd'.
	p(NT(402), (T(49)));
//G630: __E_severity_162(402) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(402), (T(49)+T(16)+T(27)+T(44)+T(54)));
//G631: debug_sym(401)       => __E_severity_162(402).
	p(NT(401), (NT(402)));
//G632: severity(396)        => debug_sym(401).
	p(NT(396), (NT(401)));
//G633: __E_severity_163(404) => 't'.
	p(NT(404), (T(17)));
//G634: __E_severity_163(404) => 't' 'r' 'a' 'c' 'e'.
	p(NT(404), (T(17)+T(48)+T(20)+T(45)+T(16)));
//G635: trace_sym(403)       => __E_severity_163(404).
	p(NT(403), (NT(404)));
//G636: severity(396)        => trace_sym(403).
	p(NT(396), (NT(403)));
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
		__E_start_0, inputs, builder, library, rr, _, rec_relations, main, wff, __E_rec_relations_1, 
		rec_relation, __E_rec_relations_2, ref, __E_rec_relation_3, capture, bf, sym, __E_ref_4, offsets, ref_args, 
		__E_ref_args_5, __E___E_ref_args_5_6, ref_arg, __E___E_ref_args_5_7, __E___E_ref_args_5_8, __E_inputs_9, __E_inputs_10, input, in, source_binding, 
		rules, __E_library_11, __E___E_library_11_12, rule, __E___E___E_library_11_12_13, wff_rule, bf_rule, tau_rule, __E___E_library_11_14, tau_matcher, 
		tau, tau_body, __E_tau_rule_15, tau_cb, wff_matcher, wff_body, __E_wff_rule_16, wff_cb, bf_matcher, bf_body, 
		__E_bf_rule_17, bf_cb, builder_head, builder_body, __E_builder_head_18, __, __E_builder_head_19, bf_builder_body, __E_builder_body_20, wff_builder_body, 
		__E_builder_body_21, tau_builder_body, __E_builder_body_22, tau_parenthesis, __E_tau_23, tau_wff, __E_tau_24, tau_ref, __E_tau_25, __E___E_tau_25_26, 
		tau_or, __E_tau_27, tau_and, __E_tau_28, tau_neg, __E_tau_29, wff_parenthesis, __E_wff_30, wff_sometimes, __E_wff_31, 
		__E___E_wff_31_32, wff_always, __E_wff_33, __E___E_wff_33_34, wff_conditional, __E_wff_35, wff_ball, __E_wff_36, q_bool_var, wff_bex, 
		__E_wff_37, wff_all, __E_wff_38, q_var, wff_ex, __E_wff_39, wff_ref, wff_imply, __E_wff_40, wff_equiv, 
		__E_wff_41, wff_or, __E_wff_42, wff_xor, __E_wff_43, wff_and, __E_wff_44, wff_neg, __E_wff_45, wff_t, 
		wff_f, bf_interval, __E_wff_46, bf_neq, __E_wff_47, bf_eq, __E_wff_48, bf_nleq_lower, __E_wff_49, bf_nleq_upper, 
		__E_wff_50, bf_greater, __E_wff_51, bf_less_equal, __E_wff_52, bf_less, __E_wff_53, bool_variable, uninterpreted_constant, bf_parenthesis, 
		__E_bf_54, bf_splitter, __E_bf_55, bf_ref, bf_or, __E_bf_56, bf_xor, __E_bf_57, bf_and, __E_bf_58, 
		__E___E_bf_58_59, bf_neg, __E_bf_60, bf_constant, __E_bf_61, constant, bf_t, bf_f, variable, charvar, 
		__E_charvar_62, __E_charvar_63, io_var, __E_variable_64, out, in_var_name, __E_in_65, digits, offset, out_var_name, 
		__E_out_66, __E_offsets_67, __E_offsets_68, num, shift, __E_offset_69, __E_shift_70, __E___E_shift_70_71, uninter_const_name, __E_uninterpreted_constant_72, 
		__E___E_uninterpreted_constant_72_73, chars, binding, __E_binding_74, type, __E___E_binding_74_75, source, named_binding, src_c, __E_source_76, 
		__E_source_77, __E___E_source_77_78, __E___E_source_77_79, __E_chars_80, __E_digits_81, comment, __E_comment_82, __E_comment_83, __E_comment_84, __E____85, 
		__E___86, bf_cb_args1, bf_cb_arg, bf_cb_args2, bf_cb_args3, bf_cb_args4, wff_cb_args1, wff_cb_arg, wff_cb_args2, wff_cb_args3, 
		wff_cb_args4, tau_cb_args1, tau_cb_arg, __E_tau_cb_args1_87, tau_cb_args2, tau_cb_args3, tau_collapse_positives_cb, __E_tau_cb_88, __E___E_tau_cb_88_89, tau_positives_upwards_cb, 
		__E_tau_cb_90, bf_eq_cb, __E_wff_cb_91, bf_neq_cb, __E_wff_cb_92, wff_has_clashing_subformulas_cb, __E_wff_cb_93, wff_has_subformula_cb, __E_wff_cb_94, wff_remove_existential_cb, 
		__E_wff_cb_95, wff_remove_bexistential_cb, __E_wff_cb_96, wff_remove_buniversal_cb, __E_wff_cb_97, bf_is_zero_cb, __E_bf_cb_98, bf_is_one_cb, __E_bf_cb_99, bf_has_subformula_cb, 
		__E_bf_cb_100, bf_remove_funiversal_cb, __E_bf_cb_101, bf_remove_fexistential_cb, __E_bf_cb_102, bf_and_cb, __E_bf_cb_103, bf_or_cb, __E_bf_cb_104, bf_xor_cb, 
		__E_bf_cb_105, bf_neg_cb, __E_bf_cb_106, bf_normalize_cb, __E_bf_cb_107, cli, cli_command, __E_cli_108, __E_cli_109, quit_cmd, 
		quit_sym, __E_cli_command_110, version_cmd, version_sym, __E_cli_command_111, clear_cmd, clear_sym, __E_cli_command_112, help_cmd, __E_cli_command_113, 
		help_sym, __E___E_cli_command_113_114, __E___E_cli_command_113_115, help_arg, file_cmd, __E_cli_command_116, file_sym, q_string, valid_cmd, __E_cli_command_117, 
		valid_sym, sat_cmd, __E_cli_command_118, sat_sym, unsat_cmd, __E_cli_command_119, unsat_sym, solve_cmd, __E_cli_command_120, solve_sym, 
		wff_cmd_arg, execute_cmd, __E_cli_command_121, execute_sym, __E___E_cli_command_121_122, __E___E_cli_command_121_123, wff_typed, normalize_cmd, __E_cli_command_124, normalize_sym, 
		__E___E_cli_command_124_125, normalize_cmd_arg, subst_cmd, __E_cli_command_126, subst_sym, __E___E_cli_command_126_127, nf_cmd_arg, inst_cmd, __E_cli_command_128, inst_sym, 
		__E___E_cli_command_128_129, inst_args, dnf_cmd, __E_cli_command_130, dnf_sym, cnf_cmd, __E_cli_command_131, cnf_sym, anf_cmd, __E_cli_command_132, 
		anf_sym, nnf_cmd, __E_cli_command_133, nnf_sym, pnf_cmd, __E_cli_command_134, pnf_sym, mnf_cmd, __E_cli_command_135, mnf_sym, 
		snf_cmd, __E_cli_command_136, snf_sym, onf_cmd, __E_cli_command_137, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_138, def_sym, 
		__E___E_cli_command_138_139, def_print_cmd, __E_cli_command_140, number, def_rr_cmd, history_list_cmd, __E_cli_command_141, history_sym, __E___E_cli_command_141_142, history_print_cmd, 
		__E_cli_command_143, memory, history_store_cmd, __E_cli_command_144, bf_typed, qelim_cmd, __E_cli_command_145, qelim_sym, get_cmd, __E_cli_command_146, 
		get_sym, __E___E_cli_command_146_147, option, set_cmd, __E_cli_command_148, set_sym, __E___E_cli_command_148_149, option_value, toggle_cmd, __E_cli_command_150, 
		toggle_sym, bool_option, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, examples_sym, __E_help_arg_151, __E___E_help_arg_151_152, rel_memory, 
		__E_memory_153, __E___E_memory_153_154, memory_id, abs_memory, __E_memory_155, enum_option, severity_opt, __E_option_156, status_opt, __E_bool_option_157, 
		colors_opt, __E_bool_option_158, debug_repl_opt, __E_bool_option_159, option_value_true, option_value_false, severity, error_sym, __E_severity_160, info_sym, 
		__E_severity_161, debug_sym, __E_severity_162, trace_sym, __E_severity_163, __N_0, __N_1, __N_2, __N_3, __N_4, 
		__N_5, 
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
