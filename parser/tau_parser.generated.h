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
	"__E___E_bf_58_59", "bf_neg", "__E_bf_60", "bf_constant", "__E_bf_61", "constant", "bf_t", "bf_f", "variable", "bf_cb_args1", 
	"bf_cb_arg", "bf_cb_args2", "bf_cb_args3", "bf_cb_args4", "wff_cb_args1", "wff_cb_arg", "wff_cb_args2", "wff_cb_args3", "wff_cb_args4", "tau_cb_args1", 
	"tau_cb_arg", "__E_tau_cb_args1_62", "tau_cb_args2", "tau_cb_args3", "tau_collapse_positives_cb", "__E_tau_cb_63", "__E___E_tau_cb_63_64", "tau_positives_upwards_cb", "__E_tau_cb_65", "bf_eq_cb", 
	"__E_wff_cb_66", "bf_neq_cb", "__E_wff_cb_67", "wff_has_clashing_subformulas_cb", "__E_wff_cb_68", "wff_has_subformula_cb", "__E_wff_cb_69", "wff_remove_existential_cb", "__E_wff_cb_70", "wff_remove_bexistential_cb", 
	"__E_wff_cb_71", "wff_remove_buniversal_cb", "__E_wff_cb_72", "bf_is_zero_cb", "__E_bf_cb_73", "bf_is_one_cb", "__E_bf_cb_74", "bf_has_subformula_cb", "__E_bf_cb_75", "bf_remove_funiversal_cb", 
	"__E_bf_cb_76", "bf_remove_fexistential_cb", "__E_bf_cb_77", "bf_and_cb", "__E_bf_cb_78", "bf_or_cb", "__E_bf_cb_79", "bf_xor_cb", "__E_bf_cb_80", "bf_neg_cb", 
	"__E_bf_cb_81", "bf_normalize_cb", "__E_bf_cb_82", "charvar", "__E_charvar_83", "__E_charvar_84", "io_var", "__E_variable_85", "out", "in_var_name", 
	"__E_in_86", "digits", "offset", "out_var_name", "__E_out_87", "__E_offsets_88", "__E_offsets_89", "num", "shift", "__E_offset_90", 
	"__E_shift_91", "__E___E_shift_91_92", "uninter_const_name", "__E_uninterpreted_constant_93", "__E___E_uninterpreted_constant_93_94", "chars", "binding", "__E_binding_95", "type", "__E___E_binding_95_96", 
	"source", "named_binding", "src_c", "__E_source_97", "__E_source_98", "__E___E_source_98_99", "__E___E_source_98_100", "__E_chars_101", "__E_digits_102", "comment", 
	"__E_comment_103", "__E_comment_104", "__E_comment_105", "__E____106", "__E___107", "cli", "cli_command", "__E_cli_108", "__E_cli_109", "quit_cmd", 
	"quit_sym", "__E_cli_command_110", "version_cmd", "version_sym", "__E_cli_command_111", "clear_cmd", "clear_sym", "__E_cli_command_112", "help_cmd", "__E_cli_command_113", 
	"help_sym", "__E___E_cli_command_113_114", "__E___E_cli_command_113_115", "help_arg", "file_cmd", "__E_cli_command_116", "file_sym", "q_string", "valid_cmd", "__E_cli_command_117", 
	"valid_sym", "sat_cmd", "__E_cli_command_118", "sat_sym", "unsat_cmd", "__E_cli_command_119", "unsat_sym", "solve_cmd", "__E_cli_command_120", "solve_sym", 
	"solve_cmd_arg", "wff_cmd_arg", "execute_cmd", "__E_cli_command_121", "execute_sym", "__E___E_cli_command_121_122", "execute_cmd_arg", "__E___E_cli_command_121_123", "wff_typed", "normalize_cmd", 
	"__E_cli_command_124", "normalize_sym", "__E___E_cli_command_124_125", "normalize_cmd_arg", "wff_inst_cmd", "__E_cli_command_126", "inst_sym", "__E___E_cli_command_126_127", "wff_cmd_out_arg", "subst_cmd", 
	"__E_cli_command_128", "subst_sym", "__E___E_cli_command_128_129", "nf_cmd_arg", "bf_inst_cmd", "__E_cli_command_130", "bf_cmd_arg", "bf_cmd_out_arg", "dnf_cmd", "__E_cli_command_131", 
	"dnf_sym", "cnf_cmd", "__E_cli_command_132", "cnf_sym", "anf_cmd", "__E_cli_command_133", "anf_sym", "nnf_cmd", "__E_cli_command_134", "nnf_sym", 
	"pnf_cmd", "__E_cli_command_135", "pnf_sym", "mnf_cmd", "__E_cli_command_136", "mnf_sym", "snf_cmd", "__E_cli_command_137", "snf_sym", "onf_cmd", 
	"__E_cli_command_138", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_139", "def_sym", "__E___E_cli_command_139_140", "def_rr_cmd", "def_print_cmd", "__E_cli_command_141", 
	"memory", "history_list_cmd", "__E_cli_command_142", "history_sym", "__E___E_cli_command_142_143", "history_print_cmd", "__E_cli_command_144", "history_store_cmd", "__E_cli_command_145", "history_store_cmd_arg", 
	"__E___E_cli_command_145_146", "bf_typed", "qelim_cmd", "__E_cli_command_147", "qelim_sym", "get_cmd", "__E_cli_command_148", "get_sym", "__E___E_cli_command_148_149", "option", 
	"set_cmd", "__E_cli_command_150", "set_sym", "__E___E_cli_command_150_151", "option_value", "toggle_cmd", "__E_cli_command_152", "toggle_sym", "bool_option", "bf_selection", 
	"wff_selection", "abs_memory_sym", "rel_memory_sym", "selection_sym", "examples_sym", "__E_help_arg_153", "__E___E_help_arg_153_154", "rel_memory", "__E_memory_155", "__E___E_memory_155_156", 
	"__E___E___E_memory_155_156_157", "memory_id", "abs_memory", "__E_memory_158", "bf_var_selection", "wff_var_selection", "enum_option", "severity_opt", "__E_option_159", "status_opt", 
	"__E_bool_option_160", "colors_opt", "__E_bool_option_161", "debug_repl_opt", "__E_bool_option_162", "option_value_true", "option_value_false", "severity", "error_sym", "__E_severity_163", 
	"info_sym", "__E_severity_164", "debug_sym", "__E_severity_165", "trace_sym", "__E_severity_166", "__N_0", "__N_1", "__N_2", "__N_3", 
	"__N_4", "__N_5", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', ',', '(', ')', '<', '{', 
	'}', '|', '&', '-', 's', 'o', 'm', 'e', 't', 'i', 
	'>', 'a', 'l', 'w', 'y', '[', ']', '?', 'b', '_', 
	'x', '^', '!', 'T', 'F', 'S', '+', '\'', '1', '0', 
	'u', 'c', 'p', 'v', 'r', 'd', 'f', 'q', 'n', 'h', 
	'g', 'z', '$', '\t', '\n', '\r', '#', '/', '%', 
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
			24, 26, 137, 213, 219, 221, 223, 232, 238, 240,
			241
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
			{ 159 },
			{ 161 },
			{ 162 },
			{ 163 },
			{ 164 },
			{ 166 },
			{ 167 },
			{ 168 },
			{ 169 },
			{ 172 },
			{ 173 },
			{ 235 },
			{ 242 },
			{ 291 },
			{ 298 },
			{ 308 },
			{ 316 },
			{ 317 },
			{ 361 }
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
//G203: bf_cb_arg(160)       => bf(25).
	p(NT(160), (NT(25)));
//G204: bf_cb_args1(159)     => __(65) bf_cb_arg(160).
	p(NT(159), (NT(65)+NT(160)));
//G205: bf_cb_args2(161)     => bf_cb_args1(159) bf_cb_args1(159).
	p(NT(161), (NT(159)+NT(159)));
//G206: bf_cb_args3(162)     => bf_cb_args2(161) bf_cb_args1(159).
	p(NT(162), (NT(161)+NT(159)));
//G207: bf_cb_args4(163)     => bf_cb_args3(162) bf_cb_args1(159).
	p(NT(163), (NT(162)+NT(159)));
//G208: wff_cb_arg(165)      => wff(18).
	p(NT(165), (NT(18)));
//G209: wff_cb_args1(164)    => __(65) wff_cb_arg(165).
	p(NT(164), (NT(65)+NT(165)));
//G210: wff_cb_args2(166)    => wff_cb_args1(164) wff_cb_args1(164).
	p(NT(166), (NT(164)+NT(164)));
//G211: wff_cb_args3(167)    => wff_cb_args2(166) wff_cb_args1(164).
	p(NT(167), (NT(166)+NT(164)));
//G212: wff_cb_args4(168)    => wff_cb_args3(167) wff_cb_args1(164).
	p(NT(168), (NT(167)+NT(164)));
//G213: __E_tau_cb_args1_62(171) => capture(24).
	p(NT(171), (NT(24)));
//G214: __E_tau_cb_args1_62(171) => tau(50).
	p(NT(171), (NT(50)));
//G215: tau_cb_arg(170)      => __E_tau_cb_args1_62(171).
	p(NT(170), (NT(171)));
//G216: tau_cb_args1(169)    => __(65) tau_cb_arg(170).
	p(NT(169), (NT(65)+NT(170)));
//G217: tau_cb_args2(172)    => tau_cb_args1(169) tau_cb_args1(169).
	p(NT(172), (NT(169)+NT(169)));
//G218: tau_cb_args3(173)    => tau_cb_args2(172) tau_cb_args1(169).
	p(NT(173), (NT(172)+NT(169)));
//G219: __E___E_tau_cb_63_64(176) => tau_cb_args3(173).
	p(NT(176), (NT(173)));
//G220: __E___E_tau_cb_63_64(176) => tau_cb_args2(172).
	p(NT(176), (NT(172)));
//G221: __E_tau_cb_63(175)   => 't' 'a' 'u' '_' 'c' 'o' 'l' 'l' 'a' 'p' 's' 'e' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'c' 'b' __E___E_tau_cb_63_64(176).
	p(NT(175), (T(17)+T(20)+T(39)+T(28)+T(40)+T(14)+T(21)+T(21)+T(20)+T(41)+T(13)+T(16)+T(28)+T(41)+T(14)+T(13)+T(18)+T(17)+T(18)+T(42)+T(16)+T(13)+T(28)+T(40)+T(27)+NT(176)));
//G222: tau_collapse_positives_cb(174) => __E_tau_cb_63(175).
	p(NT(174), (NT(175)));
//G223: tau_cb(53)           => tau_collapse_positives_cb(174).
	p(NT(53), (NT(174)));
//G224: __E_tau_cb_65(178)   => 't' 'a' 'u' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'u' 'p' 'w' 'a' 'r' 'd' 's' '_' 'c' 'b' tau_cb_args2(172).
	p(NT(178), (T(17)+T(20)+T(39)+T(28)+T(41)+T(14)+T(13)+T(18)+T(17)+T(18)+T(42)+T(16)+T(13)+T(28)+T(39)+T(41)+T(22)+T(20)+T(43)+T(44)+T(13)+T(28)+T(40)+T(27)+NT(172)));
//G225: tau_positives_upwards_cb(177) => __E_tau_cb_65(178).
	p(NT(177), (NT(178)));
//G226: tau_cb(53)           => tau_positives_upwards_cb(177).
	p(NT(53), (NT(177)));
//G227: __E_wff_cb_66(180)   => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b' bf_cb_args1(159) wff_cb_args2(166).
	p(NT(180), (T(27)+T(45)+T(28)+T(16)+T(46)+T(28)+T(40)+T(27)+NT(159)+NT(166)));
//G228: bf_eq_cb(179)        => __E_wff_cb_66(180).
	p(NT(179), (NT(180)));
//G229: wff_cb(57)           => bf_eq_cb(179).
	p(NT(57), (NT(179)));
//G230: __E_wff_cb_67(182)   => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b' bf_cb_args1(159) wff_cb_args2(166).
	p(NT(182), (T(27)+T(45)+T(28)+T(47)+T(16)+T(46)+T(28)+T(40)+T(27)+NT(159)+NT(166)));
//G231: bf_neq_cb(181)       => __E_wff_cb_67(182).
	p(NT(181), (NT(182)));
//G232: wff_cb(57)           => bf_neq_cb(181).
	p(NT(57), (NT(181)));
//G233: __E_wff_cb_68(184)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(166).
	p(NT(184), (T(22)+T(45)+T(45)+T(28)+T(48)+T(20)+T(13)+T(28)+T(40)+T(21)+T(20)+T(13)+T(48)+T(18)+T(47)+T(49)+T(28)+T(13)+T(39)+T(27)+T(45)+T(14)+T(43)+T(15)+T(39)+T(21)+T(20)+T(13)+T(28)+T(40)+T(27)+NT(166)));
//G234: wff_has_clashing_subformulas_cb(183) => __E_wff_cb_68(184).
	p(NT(183), (NT(184)));
//G235: wff_cb(57)           => wff_has_clashing_subformulas_cb(183).
	p(NT(57), (NT(183)));
//G236: __E_wff_cb_69(186)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(167).
	p(NT(186), (T(22)+T(45)+T(45)+T(28)+T(48)+T(20)+T(13)+T(28)+T(13)+T(39)+T(27)+T(45)+T(14)+T(43)+T(15)+T(39)+T(21)+T(20)+T(28)+T(40)+T(27)+NT(167)));
//G237: wff_has_subformula_cb(185) => __E_wff_cb_69(186).
	p(NT(185), (NT(186)));
//G238: wff_cb(57)           => wff_has_subformula_cb(185).
	p(NT(57), (NT(185)));
//G239: __E_wff_cb_70(188)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(166).
	p(NT(188), (T(22)+T(45)+T(45)+T(28)+T(43)+T(16)+T(15)+T(14)+T(42)+T(16)+T(28)+T(16)+T(29)+T(18)+T(13)+T(17)+T(16)+T(47)+T(17)+T(18)+T(20)+T(21)+T(28)+T(40)+T(27)+NT(166)));
//G240: wff_remove_existential_cb(187) => __E_wff_cb_70(188).
	p(NT(187), (NT(188)));
//G241: wff_cb(57)           => wff_remove_existential_cb(187).
	p(NT(57), (NT(187)));
//G242: __E_wff_cb_71(190)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args4(168).
	p(NT(190), (T(22)+T(45)+T(45)+T(28)+T(43)+T(16)+T(15)+T(14)+T(42)+T(16)+T(28)+T(27)+T(16)+T(29)+T(18)+T(13)+T(17)+T(16)+T(47)+T(17)+T(18)+T(20)+T(21)+T(28)+T(40)+T(27)+NT(168)));
//G243: wff_remove_bexistential_cb(189) => __E_wff_cb_71(190).
	p(NT(189), (NT(190)));
//G244: wff_cb(57)           => wff_remove_bexistential_cb(189).
	p(NT(57), (NT(189)));
//G245: __E_wff_cb_72(192)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' wff_cb_args4(168).
	p(NT(192), (T(22)+T(45)+T(45)+T(28)+T(43)+T(16)+T(15)+T(14)+T(42)+T(16)+T(28)+T(27)+T(39)+T(47)+T(18)+T(42)+T(16)+T(43)+T(13)+T(20)+T(21)+T(28)+T(40)+T(27)+NT(168)));
//G246: wff_remove_buniversal_cb(191) => __E_wff_cb_72(192).
	p(NT(191), (NT(192)));
//G247: wff_cb(57)           => wff_remove_buniversal_cb(191).
	p(NT(57), (NT(191)));
//G248: __E_bf_cb_73(194)    => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b' bf_cb_args2(161).
	p(NT(194), (T(27)+T(45)+T(28)+T(18)+T(13)+T(28)+T(50)+T(16)+T(43)+T(14)+T(28)+T(40)+T(27)+NT(161)));
//G249: bf_is_zero_cb(193)   => __E_bf_cb_73(194).
	p(NT(193), (NT(194)));
//G250: bf_cb(61)            => bf_is_zero_cb(193).
	p(NT(61), (NT(193)));
//G251: __E_bf_cb_74(196)    => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b' bf_cb_args2(161).
	p(NT(196), (T(27)+T(45)+T(28)+T(18)+T(13)+T(28)+T(14)+T(47)+T(16)+T(28)+T(40)+T(27)+NT(161)));
//G252: bf_is_one_cb(195)    => __E_bf_cb_74(196).
	p(NT(195), (NT(196)));
//G253: bf_cb(61)            => bf_is_one_cb(195).
	p(NT(61), (NT(195)));
//G254: __E_bf_cb_75(198)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(162).
	p(NT(198), (T(27)+T(45)+T(28)+T(48)+T(20)+T(13)+T(28)+T(13)+T(39)+T(27)+T(45)+T(14)+T(43)+T(15)+T(39)+T(21)+T(20)+T(28)+T(40)+T(27)+NT(162)));
//G255: bf_has_subformula_cb(197) => __E_bf_cb_75(198).
	p(NT(197), (NT(198)));
//G256: bf_cb(61)            => bf_has_subformula_cb(197).
	p(NT(61), (NT(197)));
//G257: __E_bf_cb_76(200)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(163).
	p(NT(200), (T(27)+T(45)+T(28)+T(43)+T(16)+T(15)+T(14)+T(42)+T(16)+T(28)+T(45)+T(39)+T(47)+T(18)+T(42)+T(16)+T(43)+T(13)+T(20)+T(21)+T(28)+T(40)+T(27)+NT(163)));
//G258: bf_remove_funiversal_cb(199) => __E_bf_cb_76(200).
	p(NT(199), (NT(200)));
//G259: bf_cb(61)            => bf_remove_funiversal_cb(199).
	p(NT(61), (NT(199)));
//G260: __E_bf_cb_77(202)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(163).
	p(NT(202), (T(27)+T(45)+T(28)+T(43)+T(16)+T(15)+T(14)+T(42)+T(16)+T(28)+T(45)+T(16)+T(29)+T(18)+T(13)+T(17)+T(16)+T(47)+T(17)+T(18)+T(20)+T(21)+T(28)+T(40)+T(27)+NT(163)));
//G261: bf_remove_fexistential_cb(201) => __E_bf_cb_77(202).
	p(NT(201), (NT(202)));
//G262: bf_cb(61)            => bf_remove_fexistential_cb(201).
	p(NT(61), (NT(201)));
//G263: __E_bf_cb_78(204)    => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b' bf_cb_args2(161).
	p(NT(204), (T(27)+T(45)+T(28)+T(20)+T(47)+T(44)+T(28)+T(40)+T(27)+NT(161)));
//G264: bf_and_cb(203)       => __E_bf_cb_78(204).
	p(NT(203), (NT(204)));
//G265: bf_cb(61)            => bf_and_cb(203).
	p(NT(61), (NT(203)));
//G266: __E_bf_cb_79(206)    => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b' bf_cb_args2(161).
	p(NT(206), (T(27)+T(45)+T(28)+T(14)+T(43)+T(28)+T(40)+T(27)+NT(161)));
//G267: bf_or_cb(205)        => __E_bf_cb_79(206).
	p(NT(205), (NT(206)));
//G268: bf_cb(61)            => bf_or_cb(205).
	p(NT(61), (NT(205)));
//G269: __E_bf_cb_80(208)    => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b' bf_cb_args2(161).
	p(NT(208), (T(27)+T(45)+T(28)+T(29)+T(14)+T(43)+T(28)+T(40)+T(27)+NT(161)));
//G270: bf_xor_cb(207)       => __E_bf_cb_80(208).
	p(NT(207), (NT(208)));
//G271: bf_cb(61)            => bf_xor_cb(207).
	p(NT(61), (NT(207)));
//G272: __E_bf_cb_81(210)    => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b' bf_cb_args1(159).
	p(NT(210), (T(27)+T(45)+T(28)+T(47)+T(16)+T(49)+T(28)+T(40)+T(27)+NT(159)));
//G273: bf_neg_cb(209)       => __E_bf_cb_81(210).
	p(NT(209), (NT(210)));
//G274: bf_cb(61)            => bf_neg_cb(209).
	p(NT(61), (NT(209)));
//G275: __E_bf_cb_82(212)    => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(159).
	p(NT(212), (T(27)+T(45)+T(28)+T(47)+T(14)+T(43)+T(15)+T(20)+T(21)+T(18)+T(50)+T(16)+T(28)+T(40)+T(27)+NT(159)));
//G276: bf_normalize_cb(211) => __E_bf_cb_82(212).
	p(NT(211), (NT(212)));
//G277: bf_cb(61)            => bf_normalize_cb(211).
	p(NT(61), (NT(211)));
//G278: __N_0(416)           => 'F'.
	p(NT(416), (T(33)));
//G279: __N_1(417)           => 'T'.
	p(NT(417), (T(32)));
//G280: __N_2(418)           => 'i'.
	p(NT(418), (T(18)));
//G281: __N_3(419)           => 'o'.
	p(NT(419), (T(14)));
//G282: __E_charvar_83(214)  => ~( __N_0(416) ) & ~( __N_1(417) ) & ~( __N_2(418) ) & ~( __N_3(419) ) & alpha(5).	 # conjunctive
	p(NT(214), ~(NT(416)) & ~(NT(417)) & ~(NT(418)) & ~(NT(419)) & (NT(5)));
//G283: __E_charvar_84(215)  => null.
	p(NT(215), (nul));
//G284: __E_charvar_84(215)  => digit(3) __E_charvar_84(215).
	p(NT(215), (NT(3)+NT(215)));
//G285: charvar(213)         => __E_charvar_83(214) __E_charvar_84(215).
	p(NT(213), (NT(214)+NT(215)));
//G286: bool_variable(137)   => '?' charvar(213).
	p(NT(137), (T(26)+NT(213)));
//G287: variable(158)        => charvar(213).
	p(NT(158), (NT(213)));
//G288: __E_variable_85(217) => in(38).
	p(NT(217), (NT(38)));
//G289: __E_variable_85(217) => out(218).
	p(NT(217), (NT(218)));
//G290: io_var(216)          => __E_variable_85(217).
	p(NT(216), (NT(217)));
//G291: variable(158)        => io_var(216).
	p(NT(158), (NT(216)));
//G292: __E_in_86(220)       => 'i' digits(221).
	p(NT(220), (T(18)+NT(221)));
//G293: in_var_name(219)     => __E_in_86(220).
	p(NT(219), (NT(220)));
//G294: in(38)               => in_var_name(219) '[' offset(222) ']'.
	p(NT(38), (NT(219)+T(24)+NT(222)+T(25)));
//G295: __E_out_87(224)      => 'o' digits(221).
	p(NT(224), (T(14)+NT(221)));
//G296: out_var_name(223)    => __E_out_87(224).
	p(NT(223), (NT(224)));
//G297: out(218)             => out_var_name(223) '[' offset(222) ']'.
	p(NT(218), (NT(223)+T(24)+NT(222)+T(25)));
//G298: capture(24)          => '$' charvar(213).
	p(NT(24), (T(51)+NT(213)));
//G299: q_bool_var(98)       => capture(24).
	p(NT(98), (NT(24)));
//G300: q_bool_var(98)       => bool_variable(137).
	p(NT(98), (NT(137)));
//G301: q_var(103)           => capture(24).
	p(NT(103), (NT(24)));
//G302: q_var(103)           => variable(158).
	p(NT(103), (NT(158)));
//G303: __E_offsets_88(225)  => _(15) ',' _(15) offset(222).
	p(NT(225), (NT(15)+T(4)+NT(15)+NT(222)));
//G304: __E_offsets_89(226)  => null.
	p(NT(226), (nul));
//G305: __E_offsets_89(226)  => __E_offsets_88(225) __E_offsets_89(226).
	p(NT(226), (NT(225)+NT(226)));
//G306: offsets(28)          => '[' _(15) offset(222) __E_offsets_89(226) _(15) ']'.
	p(NT(28), (T(24)+NT(15)+NT(222)+NT(226)+NT(15)+T(25)));
//G307: offset(222)          => num(227).
	p(NT(222), (NT(227)));
//G308: offset(222)          => capture(24).
	p(NT(222), (NT(24)));
//G309: offset(222)          => shift(228).
	p(NT(222), (NT(228)));
//G310: __N_4(420)           => io_var(216).
	p(NT(420), (NT(216)));
//G311: __E_offset_90(229)   => variable(158) & ~( __N_4(420) ).	 # conjunctive
	p(NT(229), (NT(158)) & ~(NT(420)));
//G312: offset(222)          => __E_offset_90(229).
	p(NT(222), (NT(229)));
//G313: __E_shift_91(230)    => capture(24).
	p(NT(230), (NT(24)));
//G314: __N_5(421)           => io_var(216).
	p(NT(421), (NT(216)));
//G315: __E___E_shift_91_92(231) => variable(158) & ~( __N_5(421) ).	 # conjunctive
	p(NT(231), (NT(158)) & ~(NT(421)));
//G316: __E_shift_91(230)    => __E___E_shift_91_92(231).
	p(NT(230), (NT(231)));
//G317: shift(228)           => __E_shift_91(230) _(15) '-' _(15) num(227).
	p(NT(228), (NT(230)+NT(15)+T(12)+NT(15)+NT(227)));
//G318: num(227)             => digits(221).
	p(NT(227), (NT(221)));
//G319: __E___E_uninterpreted_constant_93_94(234) => chars(235).
	p(NT(234), (NT(235)));
//G320: __E___E_uninterpreted_constant_93_94(234) => _(15).
	p(NT(234), (NT(15)));
//G321: __E_uninterpreted_constant_93(233) => __E___E_uninterpreted_constant_93_94(234) ':' chars(235).
	p(NT(233), (NT(234)+T(2)+NT(235)));
//G322: uninter_const_name(232) => __E_uninterpreted_constant_93(233).
	p(NT(232), (NT(233)));
//G323: uninterpreted_constant(138) => '<' uninter_const_name(232) _(15) '>'.
	p(NT(138), (T(7)+NT(232)+NT(15)+T(19)));
//G324: constant(155)        => binding(236).
	p(NT(155), (NT(236)));
//G325: constant(155)        => capture(24).
	p(NT(155), (NT(24)));
//G326: __E___E_binding_95_96(239) => chars(235) _(15).
	p(NT(239), (NT(235)+NT(15)));
//G327: __E___E_binding_95_96(239) => _(15).
	p(NT(239), (NT(15)));
//G328: type(238)            => __E___E_binding_95_96(239).
	p(NT(238), (NT(239)));
//G329: __E_binding_95(237)  => type(238) ':' _(15) source(240).
	p(NT(237), (NT(238)+T(2)+NT(15)+NT(240)));
//G330: source_binding(39)   => __E_binding_95(237).
	p(NT(39), (NT(237)));
//G331: binding(236)         => source_binding(39).
	p(NT(236), (NT(39)));
//G332: named_binding(241)   => chars(235).
	p(NT(241), (NT(235)));
//G333: binding(236)         => named_binding(241).
	p(NT(236), (NT(241)));
//G334: __E_source_97(243)   => alnum(6).
	p(NT(243), (NT(6)));
//G335: __E_source_97(243)   => punct(7).
	p(NT(243), (NT(7)));
//G336: src_c(242)           => __E_source_97(243).
	p(NT(242), (NT(243)));
//G337: __E___E_source_98_99(245) => src_c(242).
	p(NT(245), (NT(242)));
//G338: __E___E_source_98_99(245) => space(2).
	p(NT(245), (NT(2)));
//G339: __E___E_source_98_100(246) => null.
	p(NT(246), (nul));
//G340: __E___E_source_98_100(246) => __E___E_source_98_99(245) __E___E_source_98_100(246).
	p(NT(246), (NT(245)+NT(246)));
//G341: __E_source_98(244)   => __E___E_source_98_100(246) src_c(242).
	p(NT(244), (NT(246)+NT(242)));
//G342: __E_source_98(244)   => null.
	p(NT(244), (nul));
//G343: source(240)          => src_c(242) __E_source_98(244).
	p(NT(240), (NT(242)+NT(244)));
//G344: __E_chars_101(247)   => null.
	p(NT(247), (nul));
//G345: __E_chars_101(247)   => alnum(6) __E_chars_101(247).
	p(NT(247), (NT(6)+NT(247)));
//G346: chars(235)           => alpha(5) __E_chars_101(247).
	p(NT(235), (NT(5)+NT(247)));
//G347: __E_digits_102(248)  => digit(3).
	p(NT(248), (NT(3)));
//G348: __E_digits_102(248)  => digit(3) __E_digits_102(248).
	p(NT(248), (NT(3)+NT(248)));
//G349: digits(221)          => __E_digits_102(248).
	p(NT(221), (NT(248)));
//G350: sym(26)              => chars(235).
	p(NT(26), (NT(235)));
//G351: __E_comment_103(250) => printable(8).
	p(NT(250), (NT(8)));
//G352: __E_comment_103(250) => '\t'.
	p(NT(250), (T(52)));
//G353: __E_comment_104(251) => null.
	p(NT(251), (nul));
//G354: __E_comment_104(251) => __E_comment_103(250) __E_comment_104(251).
	p(NT(251), (NT(250)+NT(251)));
//G355: __E_comment_105(252) => '\n'.
	p(NT(252), (T(53)));
//G356: __E_comment_105(252) => '\r'.
	p(NT(252), (T(54)));
//G357: __E_comment_105(252) => eof(1).
	p(NT(252), (NT(1)));
//G358: comment(249)         => '#' __E_comment_104(251) __E_comment_105(252).
	p(NT(249), (T(55)+NT(251)+NT(252)));
//G359: __E____106(253)      => space(2).
	p(NT(253), (NT(2)));
//G360: __E____106(253)      => comment(249).
	p(NT(253), (NT(249)));
//G361: __(65)               => __E____106(253) _(15).
	p(NT(65), (NT(253)+NT(15)));
//G362: __E___107(254)       => __(65).
	p(NT(254), (NT(65)));
//G363: __E___107(254)       => null.
	p(NT(254), (nul));
//G364: _(15)                => __E___107(254).
	p(NT(15), (NT(254)));
//G365: __E_cli_108(257)     => '.' _(15) cli_command(256) _(15).
	p(NT(257), (T(1)+NT(15)+NT(256)+NT(15)));
//G366: __E_cli_109(258)     => null.
	p(NT(258), (nul));
//G367: __E_cli_109(258)     => __E_cli_108(257) __E_cli_109(258).
	p(NT(258), (NT(257)+NT(258)));
//G368: cli(255)             => _(15) cli_command(256) _(15) __E_cli_109(258).
	p(NT(255), (NT(15)+NT(256)+NT(15)+NT(258)));
//G369: __E_cli_command_110(261) => 'q'.
	p(NT(261), (T(46)));
//G370: __E_cli_command_110(261) => 'q' 'u' 'i' 't'.
	p(NT(261), (T(46)+T(39)+T(18)+T(17)));
//G371: quit_sym(260)        => __E_cli_command_110(261).
	p(NT(260), (NT(261)));
//G372: quit_cmd(259)        => quit_sym(260).
	p(NT(259), (NT(260)));
//G373: cli_command(256)     => quit_cmd(259).
	p(NT(256), (NT(259)));
//G374: __E_cli_command_111(264) => 'v'.
	p(NT(264), (T(42)));
//G375: __E_cli_command_111(264) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(264), (T(42)+T(16)+T(43)+T(13)+T(18)+T(14)+T(47)));
//G376: version_sym(263)     => __E_cli_command_111(264).
	p(NT(263), (NT(264)));
//G377: version_cmd(262)     => version_sym(263).
	p(NT(262), (NT(263)));
//G378: cli_command(256)     => version_cmd(262).
	p(NT(256), (NT(262)));
//G379: __E_cli_command_112(267) => 'c'.
	p(NT(267), (T(40)));
//G380: __E_cli_command_112(267) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(267), (T(40)+T(21)+T(16)+T(20)+T(43)));
//G381: clear_sym(266)       => __E_cli_command_112(267).
	p(NT(266), (NT(267)));
//G382: clear_cmd(265)       => clear_sym(266).
	p(NT(265), (NT(266)));
//G383: cli_command(256)     => clear_cmd(265).
	p(NT(256), (NT(265)));
//G384: __E___E_cli_command_113_114(271) => 'h'.
	p(NT(271), (T(48)));
//G385: __E___E_cli_command_113_114(271) => 'h' 'e' 'l' 'p'.
	p(NT(271), (T(48)+T(16)+T(21)+T(41)));
//G386: help_sym(270)        => __E___E_cli_command_113_114(271).
	p(NT(270), (NT(271)));
//G387: __E___E_cli_command_113_115(272) => __(65) help_arg(273).
	p(NT(272), (NT(65)+NT(273)));
//G388: __E___E_cli_command_113_115(272) => null.
	p(NT(272), (nul));
//G389: __E_cli_command_113(269) => help_sym(270) __E___E_cli_command_113_115(272).
	p(NT(269), (NT(270)+NT(272)));
//G390: help_cmd(268)        => __E_cli_command_113(269).
	p(NT(268), (NT(269)));
//G391: cli_command(256)     => help_cmd(268).
	p(NT(256), (NT(268)));
//G392: file_sym(276)        => 'f' 'i' 'l' 'e'.
	p(NT(276), (T(45)+T(18)+T(21)+T(16)));
//G393: __E_cli_command_116(275) => file_sym(276) __(65) q_string(277).
	p(NT(275), (NT(276)+NT(65)+NT(277)));
//G394: file_cmd(274)        => __E_cli_command_116(275).
	p(NT(274), (NT(275)));
//G395: cli_command(256)     => file_cmd(274).
	p(NT(256), (NT(274)));
//G396: valid_sym(280)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(280), (T(42)+T(20)+T(21)+T(18)+T(44)));
//G397: __E_cli_command_117(279) => valid_sym(280) __(65) rr(14).
	p(NT(279), (NT(280)+NT(65)+NT(14)));
//G398: valid_cmd(278)       => __E_cli_command_117(279).
	p(NT(278), (NT(279)));
//G399: cli_command(256)     => valid_cmd(278).
	p(NT(256), (NT(278)));
//G400: sat_sym(283)         => 's' 'a' 't'.
	p(NT(283), (T(13)+T(20)+T(17)));
//G401: __E_cli_command_118(282) => sat_sym(283) __(65) rr(14).
	p(NT(282), (NT(283)+NT(65)+NT(14)));
//G402: sat_cmd(281)         => __E_cli_command_118(282).
	p(NT(281), (NT(282)));
//G403: cli_command(256)     => sat_cmd(281).
	p(NT(256), (NT(281)));
//G404: unsat_sym(286)       => 'u' 'n' 's' 'a' 't'.
	p(NT(286), (T(39)+T(47)+T(13)+T(20)+T(17)));
//G405: __E_cli_command_119(285) => unsat_sym(286) __(65) rr(14).
	p(NT(285), (NT(286)+NT(65)+NT(14)));
//G406: unsat_cmd(284)       => __E_cli_command_119(285).
	p(NT(284), (NT(285)));
//G407: cli_command(256)     => unsat_cmd(284).
	p(NT(256), (NT(284)));
//G408: solve_sym(289)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(289), (T(13)+T(14)+T(21)+T(42)+T(16)));
//G409: solve_cmd_arg(290)   => wff_cmd_arg(291).
	p(NT(290), (NT(291)));
//G410: __E_cli_command_120(288) => solve_sym(289) __(65) solve_cmd_arg(290).
	p(NT(288), (NT(289)+NT(65)+NT(290)));
//G411: solve_cmd(287)       => __E_cli_command_120(288).
	p(NT(287), (NT(288)));
//G412: cli_command(256)     => solve_cmd(287).
	p(NT(256), (NT(287)));
//G413: __E___E_cli_command_121_122(295) => 'e'.
	p(NT(295), (T(16)));
//G414: __E___E_cli_command_121_122(295) => 'e' 'x' 'e' 'c' 'u' 't' 'e'.
	p(NT(295), (T(16)+T(29)+T(16)+T(40)+T(39)+T(17)+T(16)));
//G415: execute_sym(294)     => __E___E_cli_command_121_122(295).
	p(NT(294), (NT(295)));
//G416: __E___E_cli_command_121_123(297) => wff_typed(298).
	p(NT(297), (NT(298)));
//G417: __E___E_cli_command_121_123(297) => rr(14).
	p(NT(297), (NT(14)));
//G418: execute_cmd_arg(296) => __E___E_cli_command_121_123(297).
	p(NT(296), (NT(297)));
//G419: __E_cli_command_121(293) => execute_sym(294) __(65) execute_cmd_arg(296).
	p(NT(293), (NT(294)+NT(65)+NT(296)));
//G420: execute_cmd(292)     => __E_cli_command_121(293).
	p(NT(292), (NT(293)));
//G421: cli_command(256)     => execute_cmd(292).
	p(NT(256), (NT(292)));
//G422: __E___E_cli_command_124_125(302) => 'n'.
	p(NT(302), (T(47)));
//G423: __E___E_cli_command_124_125(302) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(302), (T(47)+T(14)+T(43)+T(15)+T(20)+T(21)+T(18)+T(50)+T(16)));
//G424: normalize_sym(301)   => __E___E_cli_command_124_125(302).
	p(NT(301), (NT(302)));
//G425: __E_cli_command_124(300) => normalize_sym(301) __(65) normalize_cmd_arg(303).
	p(NT(300), (NT(301)+NT(65)+NT(303)));
//G426: normalize_cmd(299)   => __E_cli_command_124(300).
	p(NT(299), (NT(300)));
//G427: cli_command(256)     => normalize_cmd(299).
	p(NT(256), (NT(299)));
//G428: __E___E_cli_command_126_127(307) => 'i'.
	p(NT(307), (T(18)));
//G429: __E___E_cli_command_126_127(307) => 'i' 'n' 's' 't'.
	p(NT(307), (T(18)+T(47)+T(13)+T(17)));
//G430: __E___E_cli_command_126_127(307) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(307), (T(18)+T(47)+T(13)+T(17)+T(20)+T(47)+T(17)+T(18)+T(20)+T(17)+T(16)));
//G431: inst_sym(306)        => __E___E_cli_command_126_127(307).
	p(NT(306), (NT(307)));
//G432: __E_cli_command_126(305) => inst_sym(306) __(65) variable(158) __(65) wff_cmd_arg(291) __(65) wff_cmd_out_arg(308).
	p(NT(305), (NT(306)+NT(65)+NT(158)+NT(65)+NT(291)+NT(65)+NT(308)));
//G433: wff_inst_cmd(304)    => __E_cli_command_126(305).
	p(NT(304), (NT(305)));
//G434: cli_command(256)     => wff_inst_cmd(304).
	p(NT(256), (NT(304)));
//G435: __E___E_cli_command_128_129(312) => 's'.
	p(NT(312), (T(13)));
//G436: __E___E_cli_command_128_129(312) => 's' 'u' 'b' 's' 't'.
	p(NT(312), (T(13)+T(39)+T(27)+T(13)+T(17)));
//G437: __E___E_cli_command_128_129(312) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(312), (T(13)+T(39)+T(27)+T(13)+T(17)+T(18)+T(17)+T(39)+T(17)+T(16)));
//G438: subst_sym(311)       => __E___E_cli_command_128_129(312).
	p(NT(311), (NT(312)));
//G439: __E_cli_command_128(310) => subst_sym(311) __(65) nf_cmd_arg(313) _(15) '[' _(15) nf_cmd_arg(313) _(15) '/' _(15) nf_cmd_arg(313) _(15) ']'.
	p(NT(310), (NT(311)+NT(65)+NT(313)+NT(15)+T(24)+NT(15)+NT(313)+NT(15)+T(56)+NT(15)+NT(313)+NT(15)+T(25)));
//G440: subst_cmd(309)       => __E_cli_command_128(310).
	p(NT(309), (NT(310)));
//G441: cli_command(256)     => subst_cmd(309).
	p(NT(256), (NT(309)));
//G442: __E_cli_command_130(315) => inst_sym(306) __(65) variable(158) __(65) bf_cmd_arg(316) __(65) bf_cmd_out_arg(317).
	p(NT(315), (NT(306)+NT(65)+NT(158)+NT(65)+NT(316)+NT(65)+NT(317)));
//G443: bf_inst_cmd(314)     => __E_cli_command_130(315).
	p(NT(314), (NT(315)));
//G444: cli_command(256)     => bf_inst_cmd(314).
	p(NT(256), (NT(314)));
//G445: dnf_sym(320)         => 'd' 'n' 'f'.
	p(NT(320), (T(44)+T(47)+T(45)));
//G446: __E_cli_command_131(319) => dnf_sym(320) __(65) nf_cmd_arg(313).
	p(NT(319), (NT(320)+NT(65)+NT(313)));
//G447: dnf_cmd(318)         => __E_cli_command_131(319).
	p(NT(318), (NT(319)));
//G448: cli_command(256)     => dnf_cmd(318).
	p(NT(256), (NT(318)));
//G449: cnf_sym(323)         => 'c' 'n' 'f'.
	p(NT(323), (T(40)+T(47)+T(45)));
//G450: __E_cli_command_132(322) => cnf_sym(323) __(65) nf_cmd_arg(313).
	p(NT(322), (NT(323)+NT(65)+NT(313)));
//G451: cnf_cmd(321)         => __E_cli_command_132(322).
	p(NT(321), (NT(322)));
//G452: cli_command(256)     => cnf_cmd(321).
	p(NT(256), (NT(321)));
//G453: anf_sym(326)         => 'a' 'n' 'f'.
	p(NT(326), (T(20)+T(47)+T(45)));
//G454: __E_cli_command_133(325) => anf_sym(326) __(65) nf_cmd_arg(313).
	p(NT(325), (NT(326)+NT(65)+NT(313)));
//G455: anf_cmd(324)         => __E_cli_command_133(325).
	p(NT(324), (NT(325)));
//G456: cli_command(256)     => anf_cmd(324).
	p(NT(256), (NT(324)));
//G457: nnf_sym(329)         => 'n' 'n' 'f'.
	p(NT(329), (T(47)+T(47)+T(45)));
//G458: __E_cli_command_134(328) => nnf_sym(329) __(65) nf_cmd_arg(313).
	p(NT(328), (NT(329)+NT(65)+NT(313)));
//G459: nnf_cmd(327)         => __E_cli_command_134(328).
	p(NT(327), (NT(328)));
//G460: cli_command(256)     => nnf_cmd(327).
	p(NT(256), (NT(327)));
//G461: pnf_sym(332)         => 'p' 'n' 'f'.
	p(NT(332), (T(41)+T(47)+T(45)));
//G462: __E_cli_command_135(331) => pnf_sym(332) __(65) nf_cmd_arg(313).
	p(NT(331), (NT(332)+NT(65)+NT(313)));
//G463: pnf_cmd(330)         => __E_cli_command_135(331).
	p(NT(330), (NT(331)));
//G464: cli_command(256)     => pnf_cmd(330).
	p(NT(256), (NT(330)));
//G465: mnf_sym(335)         => 'm' 'n' 'f'.
	p(NT(335), (T(15)+T(47)+T(45)));
//G466: __E_cli_command_136(334) => mnf_sym(335) __(65) nf_cmd_arg(313).
	p(NT(334), (NT(335)+NT(65)+NT(313)));
//G467: mnf_cmd(333)         => __E_cli_command_136(334).
	p(NT(333), (NT(334)));
//G468: cli_command(256)     => mnf_cmd(333).
	p(NT(256), (NT(333)));
//G469: snf_sym(338)         => 's' 'n' 'f'.
	p(NT(338), (T(13)+T(47)+T(45)));
//G470: __E_cli_command_137(337) => snf_sym(338) __(65) nf_cmd_arg(313).
	p(NT(337), (NT(338)+NT(65)+NT(313)));
//G471: snf_cmd(336)         => __E_cli_command_137(337).
	p(NT(336), (NT(337)));
//G472: cli_command(256)     => snf_cmd(336).
	p(NT(256), (NT(336)));
//G473: onf_sym(341)         => 'o' 'n' 'f'.
	p(NT(341), (T(14)+T(47)+T(45)));
//G474: __E_cli_command_138(340) => onf_sym(341) __(65) variable(158) __(65) onf_cmd_arg(342).
	p(NT(340), (NT(341)+NT(65)+NT(158)+NT(65)+NT(342)));
//G475: onf_cmd(339)         => __E_cli_command_138(340).
	p(NT(339), (NT(340)));
//G476: cli_command(256)     => onf_cmd(339).
	p(NT(256), (NT(339)));
//G477: __E___E_cli_command_139_140(346) => 'd' 'e' 'f' 's'.
	p(NT(346), (T(44)+T(16)+T(45)+T(13)));
//G478: __E___E_cli_command_139_140(346) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(346), (T(44)+T(16)+T(45)+T(18)+T(47)+T(18)+T(17)+T(18)+T(14)+T(47)+T(13)));
//G479: def_sym(345)         => __E___E_cli_command_139_140(346).
	p(NT(345), (NT(346)));
//G480: __E_cli_command_139(344) => def_sym(345).
	p(NT(344), (NT(345)));
//G481: def_list_cmd(343)    => __E_cli_command_139(344).
	p(NT(343), (NT(344)));
//G482: cli_command(256)     => def_list_cmd(343).
	p(NT(256), (NT(343)));
//G483: def_rr_cmd(347)      => rec_relation(20).
	p(NT(347), (NT(20)));
//G484: cli_command(256)     => def_rr_cmd(347).
	p(NT(256), (NT(347)));
//G485: __E_cli_command_141(349) => def_sym(345) __(65) memory(350).
	p(NT(349), (NT(345)+NT(65)+NT(350)));
//G486: def_print_cmd(348)   => __E_cli_command_141(349).
	p(NT(348), (NT(349)));
//G487: cli_command(256)     => def_print_cmd(348).
	p(NT(256), (NT(348)));
//G488: __E___E_cli_command_142_143(354) => 'h' 'i' 's' 't'.
	p(NT(354), (T(48)+T(18)+T(13)+T(17)));
//G489: __E___E_cli_command_142_143(354) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(354), (T(48)+T(18)+T(13)+T(17)+T(14)+T(43)+T(23)));
//G490: history_sym(353)     => __E___E_cli_command_142_143(354).
	p(NT(353), (NT(354)));
//G491: __E_cli_command_142(352) => history_sym(353).
	p(NT(352), (NT(353)));
//G492: history_list_cmd(351) => __E_cli_command_142(352).
	p(NT(351), (NT(352)));
//G493: cli_command(256)     => history_list_cmd(351).
	p(NT(256), (NT(351)));
//G494: __E_cli_command_144(356) => history_sym(353) __(65) memory(350).
	p(NT(356), (NT(353)+NT(65)+NT(350)));
//G495: history_print_cmd(355) => __E_cli_command_144(356).
	p(NT(355), (NT(356)));
//G496: cli_command(256)     => history_print_cmd(355).
	p(NT(256), (NT(355)));
//G497: __E___E_cli_command_145_146(360) => bf_typed(361).
	p(NT(360), (NT(361)));
//G498: __E___E_cli_command_145_146(360) => wff_typed(298).
	p(NT(360), (NT(298)));
//G499: history_store_cmd_arg(359) => __E___E_cli_command_145_146(360).
	p(NT(359), (NT(360)));
//G500: __E_cli_command_145(358) => history_store_cmd_arg(359).
	p(NT(358), (NT(359)));
//G501: history_store_cmd(357) => __E_cli_command_145(358).
	p(NT(357), (NT(358)));
//G502: cli_command(256)     => history_store_cmd(357).
	p(NT(256), (NT(357)));
//G503: qelim_sym(364)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(364), (T(46)+T(16)+T(21)+T(18)+T(15)));
//G504: __E_cli_command_147(363) => qelim_sym(364) __(65) wff_cmd_arg(291).
	p(NT(363), (NT(364)+NT(65)+NT(291)));
//G505: qelim_cmd(362)       => __E_cli_command_147(363).
	p(NT(362), (NT(363)));
//G506: cli_command(256)     => qelim_cmd(362).
	p(NT(256), (NT(362)));
//G507: get_sym(367)         => 'g' 'e' 't'.
	p(NT(367), (T(49)+T(16)+T(17)));
//G508: __E___E_cli_command_148_149(368) => __(65) option(369).
	p(NT(368), (NT(65)+NT(369)));
//G509: __E___E_cli_command_148_149(368) => null.
	p(NT(368), (nul));
//G510: __E_cli_command_148(366) => get_sym(367) __E___E_cli_command_148_149(368).
	p(NT(366), (NT(367)+NT(368)));
//G511: get_cmd(365)         => __E_cli_command_148(366).
	p(NT(365), (NT(366)));
//G512: cli_command(256)     => get_cmd(365).
	p(NT(256), (NT(365)));
//G513: set_sym(372)         => 's' 'e' 't'.
	p(NT(372), (T(13)+T(16)+T(17)));
//G514: __E___E_cli_command_150_151(373) => __(65).
	p(NT(373), (NT(65)));
//G515: __E___E_cli_command_150_151(373) => _(15) '=' _(15).
	p(NT(373), (NT(15)+T(3)+NT(15)));
//G516: __E_cli_command_150(371) => set_sym(372) __(65) option(369) __E___E_cli_command_150_151(373) option_value(374).
	p(NT(371), (NT(372)+NT(65)+NT(369)+NT(373)+NT(374)));
//G517: set_cmd(370)         => __E_cli_command_150(371).
	p(NT(370), (NT(371)));
//G518: cli_command(256)     => set_cmd(370).
	p(NT(256), (NT(370)));
//G519: toggle_sym(377)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(377), (T(17)+T(14)+T(49)+T(49)+T(21)+T(16)));
//G520: __E_cli_command_152(376) => toggle_sym(377) __(65) bool_option(378).
	p(NT(376), (NT(377)+NT(65)+NT(378)));
//G521: toggle_cmd(375)      => __E_cli_command_152(376).
	p(NT(375), (NT(376)));
//G522: cli_command(256)     => toggle_cmd(375).
	p(NT(256), (NT(375)));
//G523: cli_command(256)     => comment(249).
	p(NT(256), (NT(249)));
//G524: bf_typed(361)        => bf(25).
	p(NT(361), (NT(25)));
//G525: bf_typed(361)        => bf(25) _(15) ':' _(15) 'b' 'f'.
	p(NT(361), (NT(25)+NT(15)+T(2)+NT(15)+T(27)+T(45)));
//G526: wff_typed(298)       => wff(18).
	p(NT(298), (NT(18)));
//G527: wff_typed(298)       => wff(18) _(15) ':' _(15) 'w' 'f' 'f'.
	p(NT(298), (NT(18)+NT(15)+T(2)+NT(15)+T(22)+T(45)+T(45)));
//G528: bf_cmd_arg(316)      => memory(350).
	p(NT(316), (NT(350)));
//G529: bf_cmd_arg(316)      => bf_selection(379).
	p(NT(316), (NT(379)));
//G530: bf_cmd_arg(316)      => bf_typed(361).
	p(NT(316), (NT(361)));
//G531: bf_cmd_out_arg(317)  => memory(350).
	p(NT(317), (NT(350)));
//G532: bf_cmd_out_arg(317)  => bf_selection(379).
	p(NT(317), (NT(379)));
//G533: wff_cmd_arg(291)     => memory(350).
	p(NT(291), (NT(350)));
//G534: wff_cmd_arg(291)     => wff_selection(380).
	p(NT(291), (NT(380)));
//G535: wff_cmd_arg(291)     => wff_typed(298).
	p(NT(291), (NT(298)));
//G536: wff_cmd_out_arg(308) => memory(350).
	p(NT(308), (NT(350)));
//G537: wff_cmd_out_arg(308) => wff_selection(380).
	p(NT(308), (NT(380)));
//G538: nf_cmd_arg(313)      => memory(350).
	p(NT(313), (NT(350)));
//G539: nf_cmd_arg(313)      => bf_typed(361).
	p(NT(313), (NT(361)));
//G540: nf_cmd_arg(313)      => wff_typed(298).
	p(NT(313), (NT(298)));
//G541: onf_cmd_arg(342)     => memory(350).
	p(NT(342), (NT(350)));
//G542: onf_cmd_arg(342)     => wff_typed(298).
	p(NT(342), (NT(298)));
//G543: normalize_cmd_arg(303) => memory(350).
	p(NT(303), (NT(350)));
//G544: normalize_cmd_arg(303) => bf_typed(361).
	p(NT(303), (NT(361)));
//G545: normalize_cmd_arg(303) => wff_typed(298).
	p(NT(303), (NT(298)));
//G546: normalize_cmd_arg(303) => rr(14).
	p(NT(303), (NT(14)));
//G547: help_arg(273)        => help_sym(270).
	p(NT(273), (NT(270)));
//G548: help_arg(273)        => version_sym(263).
	p(NT(273), (NT(263)));
//G549: help_arg(273)        => quit_sym(260).
	p(NT(273), (NT(260)));
//G550: help_arg(273)        => clear_sym(266).
	p(NT(273), (NT(266)));
//G551: help_arg(273)        => get_sym(367).
	p(NT(273), (NT(367)));
//G552: help_arg(273)        => set_sym(372).
	p(NT(273), (NT(372)));
//G553: help_arg(273)        => toggle_sym(377).
	p(NT(273), (NT(377)));
//G554: help_arg(273)        => file_sym(276).
	p(NT(273), (NT(276)));
//G555: help_arg(273)        => history_sym(353).
	p(NT(273), (NT(353)));
//G556: help_arg(273)        => abs_memory_sym(381).
	p(NT(273), (NT(381)));
//G557: help_arg(273)        => rel_memory_sym(382).
	p(NT(273), (NT(382)));
//G558: help_arg(273)        => selection_sym(383).
	p(NT(273), (NT(383)));
//G559: help_arg(273)        => def_sym(345).
	p(NT(273), (NT(345)));
//G560: help_arg(273)        => inst_sym(306).
	p(NT(273), (NT(306)));
//G561: help_arg(273)        => subst_sym(311).
	p(NT(273), (NT(311)));
//G562: help_arg(273)        => normalize_sym(301).
	p(NT(273), (NT(301)));
//G563: help_arg(273)        => execute_sym(294).
	p(NT(273), (NT(294)));
//G564: help_arg(273)        => solve_sym(289).
	p(NT(273), (NT(289)));
//G565: help_arg(273)        => valid_sym(280).
	p(NT(273), (NT(280)));
//G566: help_arg(273)        => sat_sym(283).
	p(NT(273), (NT(283)));
//G567: help_arg(273)        => unsat_sym(286).
	p(NT(273), (NT(286)));
//G568: help_arg(273)        => dnf_sym(320).
	p(NT(273), (NT(320)));
//G569: help_arg(273)        => cnf_sym(323).
	p(NT(273), (NT(323)));
//G570: help_arg(273)        => anf_sym(326).
	p(NT(273), (NT(326)));
//G571: help_arg(273)        => snf_sym(338).
	p(NT(273), (NT(338)));
//G572: help_arg(273)        => nnf_sym(329).
	p(NT(273), (NT(329)));
//G573: help_arg(273)        => pnf_sym(332).
	p(NT(273), (NT(332)));
//G574: help_arg(273)        => mnf_sym(335).
	p(NT(273), (NT(335)));
//G575: help_arg(273)        => onf_sym(341).
	p(NT(273), (NT(341)));
//G576: help_arg(273)        => qelim_sym(364).
	p(NT(273), (NT(364)));
//G577: __E___E_help_arg_153_154(386) => 's'.
	p(NT(386), (T(13)));
//G578: __E___E_help_arg_153_154(386) => null.
	p(NT(386), (nul));
//G579: __E_help_arg_153(385) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_153_154(386).
	p(NT(385), (T(16)+T(29)+T(20)+T(15)+T(41)+T(21)+T(16)+NT(386)));
//G580: examples_sym(384)    => __E_help_arg_153(385).
	p(NT(384), (NT(385)));
//G581: help_arg(273)        => examples_sym(384).
	p(NT(273), (NT(384)));
//G582: rel_memory_sym(382)  => '%' '-'.
	p(NT(382), (T(57)+T(12)));
//G583: memory_id(391)       => digits(221).
	p(NT(391), (NT(221)));
//G584: __E___E___E_memory_155_156_157(390) => memory_id(391).
	p(NT(390), (NT(391)));
//G585: __E___E___E_memory_155_156_157(390) => null.
	p(NT(390), (nul));
//G586: __E___E_memory_155_156(389) => rel_memory_sym(382) __E___E___E_memory_155_156_157(390).
	p(NT(389), (NT(382)+NT(390)));
//G587: __E___E_memory_155_156(389) => '%'.
	p(NT(389), (T(57)));
//G588: __E_memory_155(388)  => __E___E_memory_155_156(389).
	p(NT(388), (NT(389)));
//G589: rel_memory(387)      => __E_memory_155(388).
	p(NT(387), (NT(388)));
//G590: memory(350)          => rel_memory(387).
	p(NT(350), (NT(387)));
//G591: abs_memory_sym(381)  => '%'.
	p(NT(381), (T(57)));
//G592: __E_memory_158(393)  => abs_memory_sym(381) memory_id(391).
	p(NT(393), (NT(381)+NT(391)));
//G593: abs_memory(392)      => __E_memory_158(393).
	p(NT(392), (NT(393)));
//G594: memory(350)          => abs_memory(392).
	p(NT(350), (NT(392)));
//G595: selection_sym(383)   => 's'.
	p(NT(383), (T(13)));
//G596: selection_sym(383)   => 's' 'e' 'l' 'e' 'c' 't' 'i' 'o' 'n'.
	p(NT(383), (T(13)+T(16)+T(21)+T(16)+T(40)+T(17)+T(18)+T(14)+T(47)));
//G597: wff_selection(380)   => selection_sym(383) __(65) digits(221).
	p(NT(380), (NT(383)+NT(65)+NT(221)));
//G598: bf_selection(379)    => selection_sym(383) __(65) digits(221).
	p(NT(379), (NT(383)+NT(65)+NT(221)));
//G599: bf_var_selection(394) => selection_sym(383) __(65) digits(221).
	p(NT(394), (NT(383)+NT(65)+NT(221)));
//G600: wff_var_selection(395) => selection_sym(383) __(65) digits(221).
	p(NT(395), (NT(383)+NT(65)+NT(221)));
//G601: option(369)          => bool_option(378).
	p(NT(369), (NT(378)));
//G602: __E_option_159(398)  => 's' 'e' 'v'.
	p(NT(398), (T(13)+T(16)+T(42)));
//G603: __E_option_159(398)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(398), (T(13)+T(16)+T(42)+T(16)+T(43)+T(18)+T(17)+T(23)));
//G604: severity_opt(397)    => __E_option_159(398).
	p(NT(397), (NT(398)));
//G605: enum_option(396)     => severity_opt(397).
	p(NT(396), (NT(397)));
//G606: option(369)          => enum_option(396).
	p(NT(369), (NT(396)));
//G607: __E_bool_option_160(400) => 's'.
	p(NT(400), (T(13)));
//G608: __E_bool_option_160(400) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(400), (T(13)+T(17)+T(20)+T(17)+T(39)+T(13)));
//G609: status_opt(399)      => __E_bool_option_160(400).
	p(NT(399), (NT(400)));
//G610: bool_option(378)     => status_opt(399).
	p(NT(378), (NT(399)));
//G611: __E_bool_option_161(402) => 'c'.
	p(NT(402), (T(40)));
//G612: __E_bool_option_161(402) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(402), (T(40)+T(14)+T(21)+T(14)+T(43)));
//G613: __E_bool_option_161(402) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(402), (T(40)+T(14)+T(21)+T(14)+T(43)+T(13)));
//G614: colors_opt(401)      => __E_bool_option_161(402).
	p(NT(401), (NT(402)));
//G615: bool_option(378)     => colors_opt(401).
	p(NT(378), (NT(401)));
//G616: __E_bool_option_162(404) => 'd'.
	p(NT(404), (T(44)));
//G617: __E_bool_option_162(404) => 'd' 'b' 'g'.
	p(NT(404), (T(44)+T(27)+T(49)));
//G618: __E_bool_option_162(404) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(404), (T(44)+T(16)+T(27)+T(39)+T(49)));
//G619: debug_repl_opt(403)  => __E_bool_option_162(404).
	p(NT(403), (NT(404)));
//G620: bool_option(378)     => debug_repl_opt(403).
	p(NT(378), (NT(403)));
//G621: option_value(374)    => option_value_true(405).
	p(NT(374), (NT(405)));
//G622: option_value(374)    => option_value_false(406).
	p(NT(374), (NT(406)));
//G623: option_value(374)    => severity(407).
	p(NT(374), (NT(407)));
//G624: option_value_true(405) => 't'.
	p(NT(405), (T(17)));
//G625: option_value_true(405) => 't' 'r' 'u' 'e'.
	p(NT(405), (T(17)+T(43)+T(39)+T(16)));
//G626: option_value_true(405) => 'o' 'n'.
	p(NT(405), (T(14)+T(47)));
//G627: option_value_true(405) => '1'.
	p(NT(405), (T(37)));
//G628: option_value_true(405) => 'y'.
	p(NT(405), (T(23)));
//G629: option_value_true(405) => 'y' 'e' 's'.
	p(NT(405), (T(23)+T(16)+T(13)));
//G630: option_value_false(406) => 'f'.
	p(NT(406), (T(45)));
//G631: option_value_false(406) => 'f' 'a' 'l' 's' 'e'.
	p(NT(406), (T(45)+T(20)+T(21)+T(13)+T(16)));
//G632: option_value_false(406) => 'o' 'f' 'f'.
	p(NT(406), (T(14)+T(45)+T(45)));
//G633: option_value_false(406) => '0'.
	p(NT(406), (T(38)));
//G634: option_value_false(406) => 'n'.
	p(NT(406), (T(47)));
//G635: option_value_false(406) => 'n' 'o'.
	p(NT(406), (T(47)+T(14)));
//G636: __E_severity_163(409) => 'e'.
	p(NT(409), (T(16)));
//G637: __E_severity_163(409) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(409), (T(16)+T(43)+T(43)+T(14)+T(43)));
//G638: error_sym(408)       => __E_severity_163(409).
	p(NT(408), (NT(409)));
//G639: severity(407)        => error_sym(408).
	p(NT(407), (NT(408)));
//G640: __E_severity_164(411) => 'i'.
	p(NT(411), (T(18)));
//G641: __E_severity_164(411) => 'i' 'n' 'f' 'o'.
	p(NT(411), (T(18)+T(47)+T(45)+T(14)));
//G642: info_sym(410)        => __E_severity_164(411).
	p(NT(410), (NT(411)));
//G643: severity(407)        => info_sym(410).
	p(NT(407), (NT(410)));
//G644: __E_severity_165(413) => 'd'.
	p(NT(413), (T(44)));
//G645: __E_severity_165(413) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(413), (T(44)+T(16)+T(27)+T(39)+T(49)));
//G646: debug_sym(412)       => __E_severity_165(413).
	p(NT(412), (NT(413)));
//G647: severity(407)        => debug_sym(412).
	p(NT(407), (NT(412)));
//G648: __E_severity_166(415) => 't'.
	p(NT(415), (T(17)));
//G649: __E_severity_166(415) => 't' 'r' 'a' 'c' 'e'.
	p(NT(415), (T(17)+T(43)+T(20)+T(40)+T(16)));
//G650: trace_sym(414)       => __E_severity_166(415).
	p(NT(414), (NT(415)));
//G651: severity(407)        => trace_sym(414).
	p(NT(407), (NT(414)));
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
		__E___E_bf_58_59, bf_neg, __E_bf_60, bf_constant, __E_bf_61, constant, bf_t, bf_f, variable, bf_cb_args1, 
		bf_cb_arg, bf_cb_args2, bf_cb_args3, bf_cb_args4, wff_cb_args1, wff_cb_arg, wff_cb_args2, wff_cb_args3, wff_cb_args4, tau_cb_args1, 
		tau_cb_arg, __E_tau_cb_args1_62, tau_cb_args2, tau_cb_args3, tau_collapse_positives_cb, __E_tau_cb_63, __E___E_tau_cb_63_64, tau_positives_upwards_cb, __E_tau_cb_65, bf_eq_cb, 
		__E_wff_cb_66, bf_neq_cb, __E_wff_cb_67, wff_has_clashing_subformulas_cb, __E_wff_cb_68, wff_has_subformula_cb, __E_wff_cb_69, wff_remove_existential_cb, __E_wff_cb_70, wff_remove_bexistential_cb, 
		__E_wff_cb_71, wff_remove_buniversal_cb, __E_wff_cb_72, bf_is_zero_cb, __E_bf_cb_73, bf_is_one_cb, __E_bf_cb_74, bf_has_subformula_cb, __E_bf_cb_75, bf_remove_funiversal_cb, 
		__E_bf_cb_76, bf_remove_fexistential_cb, __E_bf_cb_77, bf_and_cb, __E_bf_cb_78, bf_or_cb, __E_bf_cb_79, bf_xor_cb, __E_bf_cb_80, bf_neg_cb, 
		__E_bf_cb_81, bf_normalize_cb, __E_bf_cb_82, charvar, __E_charvar_83, __E_charvar_84, io_var, __E_variable_85, out, in_var_name, 
		__E_in_86, digits, offset, out_var_name, __E_out_87, __E_offsets_88, __E_offsets_89, num, shift, __E_offset_90, 
		__E_shift_91, __E___E_shift_91_92, uninter_const_name, __E_uninterpreted_constant_93, __E___E_uninterpreted_constant_93_94, chars, binding, __E_binding_95, type, __E___E_binding_95_96, 
		source, named_binding, src_c, __E_source_97, __E_source_98, __E___E_source_98_99, __E___E_source_98_100, __E_chars_101, __E_digits_102, comment, 
		__E_comment_103, __E_comment_104, __E_comment_105, __E____106, __E___107, cli, cli_command, __E_cli_108, __E_cli_109, quit_cmd, 
		quit_sym, __E_cli_command_110, version_cmd, version_sym, __E_cli_command_111, clear_cmd, clear_sym, __E_cli_command_112, help_cmd, __E_cli_command_113, 
		help_sym, __E___E_cli_command_113_114, __E___E_cli_command_113_115, help_arg, file_cmd, __E_cli_command_116, file_sym, q_string, valid_cmd, __E_cli_command_117, 
		valid_sym, sat_cmd, __E_cli_command_118, sat_sym, unsat_cmd, __E_cli_command_119, unsat_sym, solve_cmd, __E_cli_command_120, solve_sym, 
		solve_cmd_arg, wff_cmd_arg, execute_cmd, __E_cli_command_121, execute_sym, __E___E_cli_command_121_122, execute_cmd_arg, __E___E_cli_command_121_123, wff_typed, normalize_cmd, 
		__E_cli_command_124, normalize_sym, __E___E_cli_command_124_125, normalize_cmd_arg, wff_inst_cmd, __E_cli_command_126, inst_sym, __E___E_cli_command_126_127, wff_cmd_out_arg, subst_cmd, 
		__E_cli_command_128, subst_sym, __E___E_cli_command_128_129, nf_cmd_arg, bf_inst_cmd, __E_cli_command_130, bf_cmd_arg, bf_cmd_out_arg, dnf_cmd, __E_cli_command_131, 
		dnf_sym, cnf_cmd, __E_cli_command_132, cnf_sym, anf_cmd, __E_cli_command_133, anf_sym, nnf_cmd, __E_cli_command_134, nnf_sym, 
		pnf_cmd, __E_cli_command_135, pnf_sym, mnf_cmd, __E_cli_command_136, mnf_sym, snf_cmd, __E_cli_command_137, snf_sym, onf_cmd, 
		__E_cli_command_138, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_139, def_sym, __E___E_cli_command_139_140, def_rr_cmd, def_print_cmd, __E_cli_command_141, 
		memory, history_list_cmd, __E_cli_command_142, history_sym, __E___E_cli_command_142_143, history_print_cmd, __E_cli_command_144, history_store_cmd, __E_cli_command_145, history_store_cmd_arg, 
		__E___E_cli_command_145_146, bf_typed, qelim_cmd, __E_cli_command_147, qelim_sym, get_cmd, __E_cli_command_148, get_sym, __E___E_cli_command_148_149, option, 
		set_cmd, __E_cli_command_150, set_sym, __E___E_cli_command_150_151, option_value, toggle_cmd, __E_cli_command_152, toggle_sym, bool_option, bf_selection, 
		wff_selection, abs_memory_sym, rel_memory_sym, selection_sym, examples_sym, __E_help_arg_153, __E___E_help_arg_153_154, rel_memory, __E_memory_155, __E___E_memory_155_156, 
		__E___E___E_memory_155_156_157, memory_id, abs_memory, __E_memory_158, bf_var_selection, wff_var_selection, enum_option, severity_opt, __E_option_159, status_opt, 
		__E_bool_option_160, colors_opt, __E_bool_option_161, debug_repl_opt, __E_bool_option_162, option_value_true, option_value_false, severity, error_sym, __E_severity_163, 
		info_sym, __E_severity_164, debug_sym, __E_severity_165, trace_sym, __E_severity_166, __N_0, __N_1, __N_2, __N_3, 
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
