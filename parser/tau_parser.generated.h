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
	"rules", "__E_library_11", "__E___E_library_11_12", "rule", "__E___E___E_library_11_12_13", "wff_rule", "bf_rule", "__E___E_library_11_14", "wff_matcher", "wff_body", 
	"__E_wff_rule_15", "wff_cb", "bf_matcher", "bf_body", "__E_bf_rule_16", "bf_cb", "builder_head", "builder_body", "__E_builder_head_17", "__", 
	"__E_builder_head_18", "bf_builder_body", "__E_builder_body_19", "wff_builder_body", "__E_builder_body_20", "wff_parenthesis", "__E_wff_21", "wff_sometimes", "__E_wff_22", "__E___E_wff_22_23", 
	"wff_always", "__E_wff_24", "__E___E_wff_24_25", "wff_conditional", "__E_wff_26", "wff_ball", "__E_wff_27", "q_bool_var", "wff_bex", "__E_wff_28", 
	"wff_all", "__E_wff_29", "q_var", "wff_ex", "__E_wff_30", "wff_ref", "wff_imply", "__E_wff_31", "wff_equiv", "__E_wff_32", 
	"wff_or", "__E_wff_33", "wff_xor", "__E_wff_34", "wff_and", "__E_wff_35", "wff_neg", "__E_wff_36", "wff_t", "wff_f", 
	"bf_interval", "__E_wff_37", "bf_neq", "__E_wff_38", "bf_eq", "__E_wff_39", "bf_nleq", "__E_wff_40", "bf_greater", "__E_wff_41", 
	"bf_less_equal", "__E_wff_42", "bf_less", "__E_wff_43", "bool_variable", "uninterpreted_constant", "bf_parenthesis", "__E_bf_44", "bf_splitter", "__E_bf_45", 
	"bf_ref", "bf_or", "__E_bf_46", "bf_xor", "__E_bf_47", "bf_and", "__E_bf_48", "__E___E_bf_48_49", "bf_neg", "__E_bf_50", 
	"bf_constant", "__E_bf_51", "constant", "bf_t", "bf_f", "variable", "charvar", "__E_charvar_52", "__E_charvar_53", "io_var", 
	"__E_variable_54", "out", "in_var_name", "__E_in_55", "digits", "offset", "out_var_name", "__E_out_56", "__E_offsets_57", "__E_offsets_58", 
	"num", "shift", "__E_offset_59", "__E_shift_60", "__E___E_shift_60_61", "uninter_const_name", "__E_uninterpreted_constant_62", "__E___E_uninterpreted_constant_62_63", "chars", "binding", 
	"__E_binding_64", "type", "__E___E_binding_64_65", "source", "named_binding", "src_c", "__E_source_66", "__E_source_67", "__E___E_source_67_68", "__E___E_source_67_69", 
	"__E_chars_70", "__E_digits_71", "comment", "__E_comment_72", "__E_comment_73", "__E_comment_74", "__E____75", "__E___76", "bf_cb_args1", "bf_cb_arg", 
	"bf_cb_args2", "bf_cb_args3", "bf_cb_args4", "wff_cb_args1", "wff_cb_arg", "wff_cb_args2", "wff_cb_args3", "wff_cb_args4", "bf_eq_cb", "__E_wff_cb_77", 
	"bf_neq_cb", "__E_wff_cb_78", "wff_has_clashing_subformulas_cb", "__E_wff_cb_79", "wff_has_subformula_cb", "__E_wff_cb_80", "wff_remove_existential_cb", "__E_wff_cb_81", "wff_remove_bexistential_cb", "__E_wff_cb_82", 
	"wff_remove_buniversal_cb", "__E_wff_cb_83", "bf_is_zero_cb", "__E_bf_cb_84", "bf_is_one_cb", "__E_bf_cb_85", "bf_has_subformula_cb", "__E_bf_cb_86", "bf_remove_funiversal_cb", "__E_bf_cb_87", 
	"bf_remove_fexistential_cb", "__E_bf_cb_88", "bf_and_cb", "__E_bf_cb_89", "bf_or_cb", "__E_bf_cb_90", "bf_xor_cb", "__E_bf_cb_91", "bf_neg_cb", "__E_bf_cb_92", 
	"bf_normalize_cb", "__E_bf_cb_93", "cli", "cli_command", "__E_cli_94", "__E_cli_95", "quit_cmd", "quit_sym", "__E_cli_command_96", "version_cmd", 
	"version_sym", "__E_cli_command_97", "clear_cmd", "clear_sym", "__E_cli_command_98", "help_cmd", "__E_cli_command_99", "help_sym", "__E___E_cli_command_99_100", "__E___E_cli_command_99_101", 
	"help_arg", "file_cmd", "__E_cli_command_102", "file_sym", "q_string", "valid_cmd", "__E_cli_command_103", "valid_sym", "sat_cmd", "__E_cli_command_104", 
	"sat_sym", "unsat_cmd", "__E_cli_command_105", "unsat_sym", "solve_cmd", "__E_cli_command_106", "solve_sym", "wff_cmd_arg", "execute_cmd", "__E_cli_command_107", 
	"execute_sym", "__E___E_cli_command_107_108", "__E___E_cli_command_107_109", "wff_typed", "normalize_cmd", "__E_cli_command_110", "normalize_sym", "__E___E_cli_command_110_111", "normalize_cmd_arg", "subst_cmd", 
	"__E_cli_command_112", "subst_sym", "__E___E_cli_command_112_113", "nf_cmd_arg", "inst_cmd", "__E_cli_command_114", "inst_sym", "__E___E_cli_command_114_115", "inst_args", "dnf_cmd", 
	"__E_cli_command_116", "dnf_sym", "cnf_cmd", "__E_cli_command_117", "cnf_sym", "anf_cmd", "__E_cli_command_118", "anf_sym", "nnf_cmd", "__E_cli_command_119", 
	"nnf_sym", "pnf_cmd", "__E_cli_command_120", "pnf_sym", "mnf_cmd", "__E_cli_command_121", "mnf_sym", "snf_cmd", "__E_cli_command_122", "snf_sym", 
	"onf_cmd", "__E_cli_command_123", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_124", "def_sym", "__E___E_cli_command_124_125", "def_print_cmd", "__E_cli_command_126", 
	"number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_127", "qelim_sym", "get_cmd", "__E_cli_command_128", "get_sym", "__E___E_cli_command_128_129", "option", 
	"set_cmd", "__E_cli_command_130", "set_sym", "__E___E_cli_command_130_131", "option_value", "toggle_cmd", "__E_cli_command_132", "toggle_sym", "bool_option", "history_list_cmd", 
	"__E_cli_command_133", "history_sym", "__E___E_cli_command_133_134", "history_print_cmd", "__E_cli_command_135", "memory", "history_store_cmd", "__E_cli_command_136", "bf_typed", "bf_cmd_arg", 
	"abs_memory_sym", "rel_memory_sym", "selection_sym", "examples_sym", "__E_help_arg_137", "__E___E_help_arg_137_138", "rel_memory", "__E_memory_139", "__E___E_memory_139_140", "memory_id", 
	"abs_memory", "__E_memory_141", "enum_option", "severity_opt", "__E_option_142", "status_opt", "__E_bool_option_143", "colors_opt", "__E_bool_option_144", "debug_repl_opt", 
	"__E_bool_option_145", "option_value_true", "option_value_false", "severity", "error_sym", "__E_severity_146", "info_sym", "__E_severity_147", "debug_sym", "__E_severity_148", 
	"trace_sym", "__E_severity_149", "tau_cb", "tau_rule", "tau_matcher", "tau", "tau_body", "__E_tau_rule_150", "tau_builder_body", "__E_builder_body_151", 
	"tau_parenthesis", "__E_tau_152", "tau_wff", "__E_tau_153", "tau_ref", "__E_tau_154", "__E___E_tau_154_155", "tau_or", "__E_tau_156", "tau_and", 
	"__E_tau_157", "tau_neg", "__E_tau_158", "tau_cb_args1", "tau_cb_arg", "__E_tau_cb_args1_159", "tau_cb_args2", "tau_cb_args3", "tau_collapse_positives_cb", "__E_tau_cb_160", 
	"__E___E_tau_cb_160_161", "tau_positives_upwards_cb", "__E_tau_cb_162", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', ',', '(', ')', '<', '{', 
	'}', 's', 'o', 'm', 'e', 't', 'i', '>', 'a', 'l', 
	'w', 'y', '[', ']', '?', 'b', '_', 'x', '-', '|', 
	'^', '&', '!', 'T', 'F', 'S', '+', '\'', '1', '0', 
	'$', '\t', '\n', '\r', '#', 'f', 'q', 'c', 'n', 'h', 
	'g', 'u', 'r', 'v', 'z', 'd', 'p', '/', '%', 
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
			15, 59
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			24, 26, 114, 136, 142, 144, 146, 155, 161, 163,
			164
		},
		.to_inline = {
			{ 18, 65, 18 },
			{ 25, 116, 25 },
			{ 51 },
			{ 55 },
			{ 77 },
			{ 82 },
			{ 158 },
			{ 165 },
			{ 178 },
			{ 180 },
			{ 181 },
			{ 182 },
			{ 183 },
			{ 185 },
			{ 186 },
			{ 187 },
			{ 257 },
			{ 263 },
			{ 268 },
			{ 273 },
			{ 278 },
			{ 303 },
			{ 338 },
			{ 339 },
			{ 372 },
			{ 375, 380, 375 },
			{ 393 },
			{ 396 },
			{ 397 }
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
//G34:  rule(43)             => __E___E___E_library_11_12_13(44).
	p(NT(43), (NT(44)));
//G35:  __E___E_library_11_12(42) => _(15) rule(43).
	p(NT(42), (NT(15)+NT(43)));
//G36:  __E___E_library_11_14(47) => null.
	p(NT(47), (nul));
//G37:  __E___E_library_11_14(47) => __E___E_library_11_12(42) __E___E_library_11_14(47).
	p(NT(47), (NT(42)+NT(47)));
//G38:  __E_library_11(41)   => __E___E_library_11_14(47).
	p(NT(41), (NT(47)));
//G39:  rules(40)            => __E_library_11(41).
	p(NT(40), (NT(41)));
//G40:  library(13)          => rules(40).
	p(NT(13), (NT(40)));
//G41:  wff_matcher(48)      => wff(18).
	p(NT(48), (NT(18)));
//G42:  __E_wff_rule_15(50)  => wff(18).
	p(NT(50), (NT(18)));
//G43:  __E_wff_rule_15(50)  => wff_cb(51).
	p(NT(50), (NT(51)));
//G44:  wff_body(49)         => __E_wff_rule_15(50).
	p(NT(49), (NT(50)));
//G45:  wff_rule(45)         => wff_matcher(48) _(15) ':' ':' '=' _(15) wff_body(49) _(15) '.'.
	p(NT(45), (NT(48)+NT(15)+T(2)+T(2)+T(3)+NT(15)+NT(49)+NT(15)+T(1)));
//G46:  bf_matcher(52)       => bf(25).
	p(NT(52), (NT(25)));
//G47:  __E_bf_rule_16(54)   => bf(25).
	p(NT(54), (NT(25)));
//G48:  __E_bf_rule_16(54)   => bf_cb(55).
	p(NT(54), (NT(55)));
//G49:  bf_body(53)          => __E_bf_rule_16(54).
	p(NT(53), (NT(54)));
//G50:  bf_rule(46)          => bf_matcher(52) _(15) ':' '=' _(15) bf_body(53) _(15) '.'.
	p(NT(46), (NT(52)+NT(15)+T(2)+T(3)+NT(15)+NT(53)+NT(15)+T(1)));
//G51:  builder(12)          => _(15) builder_head(56) _(15) builder_body(57) _(15) '.'.
	p(NT(12), (NT(15)+NT(56)+NT(15)+NT(57)+NT(15)+T(1)));
//G52:  __E_builder_head_17(58) => __(59) capture(24).
	p(NT(58), (NT(59)+NT(24)));
//G53:  __E_builder_head_18(60) => null.
	p(NT(60), (nul));
//G54:  __E_builder_head_18(60) => __E_builder_head_17(58) __E_builder_head_18(60).
	p(NT(60), (NT(58)+NT(60)));
//G55:  builder_head(56)     => '(' _(15) capture(24) __E_builder_head_18(60) _(15) ')'.
	p(NT(56), (T(5)+NT(15)+NT(24)+NT(60)+NT(15)+T(6)));
//G56:  __E_builder_body_19(62) => '=' ':' _(15) bf(25).
	p(NT(62), (T(3)+T(2)+NT(15)+NT(25)));
//G57:  bf_builder_body(61)  => __E_builder_body_19(62).
	p(NT(61), (NT(62)));
//G58:  builder_body(57)     => bf_builder_body(61).
	p(NT(57), (NT(61)));
//G59:  __E_builder_body_20(64) => '=' ':' ':' _(15) wff(18).
	p(NT(64), (T(3)+T(2)+T(2)+NT(15)+NT(18)));
//G60:  wff_builder_body(63) => __E_builder_body_20(64).
	p(NT(63), (NT(64)));
//G61:  builder_body(57)     => wff_builder_body(63).
	p(NT(57), (NT(63)));
//G62:  __E_wff_21(66)       => '(' _(15) wff(18) _(15) ')'.
	p(NT(66), (T(5)+NT(15)+NT(18)+NT(15)+T(6)));
//G63:  wff_parenthesis(65)  => __E_wff_21(66).
	p(NT(65), (NT(66)));
//G64:  wff(18)              => wff_parenthesis(65).
	p(NT(18), (NT(65)));
//G65:  __E___E_wff_22_23(69) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(69), (T(10)+T(11)+T(12)+T(13)+T(14)+T(15)+T(12)+T(13)+T(10)));
//G66:  __E___E_wff_22_23(69) => '<' '>'.
	p(NT(69), (T(7)+T(16)));
//G67:  __E_wff_22(68)       => __E___E_wff_22_23(69) _(15) wff(18).
	p(NT(68), (NT(69)+NT(15)+NT(18)));
//G68:  wff_sometimes(67)    => __E_wff_22(68).
	p(NT(67), (NT(68)));
//G69:  wff(18)              => wff_sometimes(67).
	p(NT(18), (NT(67)));
//G70:  __E___E_wff_24_25(72) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(72), (T(17)+T(18)+T(19)+T(17)+T(20)+T(10)));
//G71:  __E___E_wff_24_25(72) => '[' ']'.
	p(NT(72), (T(21)+T(22)));
//G72:  __E_wff_24(71)       => __E___E_wff_24_25(72) _(15) wff(18).
	p(NT(71), (NT(72)+NT(15)+NT(18)));
//G73:  wff_always(70)       => __E_wff_24(71).
	p(NT(70), (NT(71)));
//G74:  wff(18)              => wff_always(70).
	p(NT(18), (NT(70)));
//G75:  __E_wff_26(74)       => wff(18) _(15) '?' _(15) wff(18) _(15) ':' _(15) wff(18).
	p(NT(74), (NT(18)+NT(15)+T(23)+NT(15)+NT(18)+NT(15)+T(2)+NT(15)+NT(18)));
//G76:  wff_conditional(73)  => __E_wff_26(74).
	p(NT(73), (NT(74)));
//G77:  wff(18)              => wff_conditional(73).
	p(NT(18), (NT(73)));
//G78:  __E_wff_27(76)       => 'b' '_' 'a' 'l' 'l' __(59) q_bool_var(77) __(59) wff(18).
	p(NT(76), (T(24)+T(25)+T(17)+T(18)+T(18)+NT(59)+NT(77)+NT(59)+NT(18)));
//G79:  wff_ball(75)         => __E_wff_27(76).
	p(NT(75), (NT(76)));
//G80:  wff(18)              => wff_ball(75).
	p(NT(18), (NT(75)));
//G81:  __E_wff_28(79)       => 'b' '_' 'e' 'x' __(59) q_bool_var(77) __(59) wff(18).
	p(NT(79), (T(24)+T(25)+T(13)+T(26)+NT(59)+NT(77)+NT(59)+NT(18)));
//G82:  wff_bex(78)          => __E_wff_28(79).
	p(NT(78), (NT(79)));
//G83:  wff(18)              => wff_bex(78).
	p(NT(18), (NT(78)));
//G84:  __E_wff_29(81)       => 'a' 'l' 'l' __(59) q_var(82) __(59) wff(18).
	p(NT(81), (T(17)+T(18)+T(18)+NT(59)+NT(82)+NT(59)+NT(18)));
//G85:  wff_all(80)          => __E_wff_29(81).
	p(NT(80), (NT(81)));
//G86:  wff(18)              => wff_all(80).
	p(NT(18), (NT(80)));
//G87:  __E_wff_30(84)       => 'e' 'x' __(59) q_var(82) __(59) wff(18).
	p(NT(84), (T(13)+T(26)+NT(59)+NT(82)+NT(59)+NT(18)));
//G88:  wff_ex(83)           => __E_wff_30(84).
	p(NT(83), (NT(84)));
//G89:  wff(18)              => wff_ex(83).
	p(NT(18), (NT(83)));
//G90:  wff_ref(85)          => ref(22).
	p(NT(85), (NT(22)));
//G91:  wff(18)              => wff_ref(85).
	p(NT(18), (NT(85)));
//G92:  __E_wff_31(87)       => wff(18) _(15) '-' '>' _(15) wff(18).
	p(NT(87), (NT(18)+NT(15)+T(27)+T(16)+NT(15)+NT(18)));
//G93:  wff_imply(86)        => __E_wff_31(87).
	p(NT(86), (NT(87)));
//G94:  wff(18)              => wff_imply(86).
	p(NT(18), (NT(86)));
//G95:  __E_wff_32(89)       => wff(18) _(15) '<' '-' '>' _(15) wff(18).
	p(NT(89), (NT(18)+NT(15)+T(7)+T(27)+T(16)+NT(15)+NT(18)));
//G96:  wff_equiv(88)        => __E_wff_32(89).
	p(NT(88), (NT(89)));
//G97:  wff(18)              => wff_equiv(88).
	p(NT(18), (NT(88)));
//G98:  __E_wff_33(91)       => wff(18) _(15) '|' '|' _(15) wff(18).
	p(NT(91), (NT(18)+NT(15)+T(28)+T(28)+NT(15)+NT(18)));
//G99:  wff_or(90)           => __E_wff_33(91).
	p(NT(90), (NT(91)));
//G100: wff(18)              => wff_or(90).
	p(NT(18), (NT(90)));
//G101: __E_wff_34(93)       => wff(18) _(15) '^' _(15) wff(18).
	p(NT(93), (NT(18)+NT(15)+T(29)+NT(15)+NT(18)));
//G102: wff_xor(92)          => __E_wff_34(93).
	p(NT(92), (NT(93)));
//G103: wff(18)              => wff_xor(92).
	p(NT(18), (NT(92)));
//G104: __E_wff_35(95)       => wff(18) _(15) '&' '&' _(15) wff(18).
	p(NT(95), (NT(18)+NT(15)+T(30)+T(30)+NT(15)+NT(18)));
//G105: wff_and(94)          => __E_wff_35(95).
	p(NT(94), (NT(95)));
//G106: wff(18)              => wff_and(94).
	p(NT(18), (NT(94)));
//G107: __E_wff_36(97)       => '!' _(15) wff(18).
	p(NT(97), (T(31)+NT(15)+NT(18)));
//G108: wff_neg(96)          => __E_wff_36(97).
	p(NT(96), (NT(97)));
//G109: wff(18)              => wff_neg(96).
	p(NT(18), (NT(96)));
//G110: wff_t(98)            => 'T'.
	p(NT(98), (T(32)));
//G111: wff(18)              => wff_t(98).
	p(NT(18), (NT(98)));
//G112: wff_f(99)            => 'F'.
	p(NT(99), (T(33)));
//G113: wff(18)              => wff_f(99).
	p(NT(18), (NT(99)));
//G114: __E_wff_37(101)      => bf(25) _(15) '<' '=' _(15) bf(25) _(15) '<' '=' _(15) bf(25).
	p(NT(101), (NT(25)+NT(15)+T(7)+T(3)+NT(15)+NT(25)+NT(15)+T(7)+T(3)+NT(15)+NT(25)));
//G115: bf_interval(100)     => __E_wff_37(101).
	p(NT(100), (NT(101)));
//G116: wff(18)              => bf_interval(100).
	p(NT(18), (NT(100)));
//G117: __E_wff_38(103)      => bf(25) _(15) '!' '=' _(15) bf(25).
	p(NT(103), (NT(25)+NT(15)+T(31)+T(3)+NT(15)+NT(25)));
//G118: bf_neq(102)          => __E_wff_38(103).
	p(NT(102), (NT(103)));
//G119: wff(18)              => bf_neq(102).
	p(NT(18), (NT(102)));
//G120: __E_wff_39(105)      => bf(25) _(15) '=' _(15) bf(25).
	p(NT(105), (NT(25)+NT(15)+T(3)+NT(15)+NT(25)));
//G121: bf_eq(104)           => __E_wff_39(105).
	p(NT(104), (NT(105)));
//G122: wff(18)              => bf_eq(104).
	p(NT(18), (NT(104)));
//G123: __E_wff_40(107)      => bf(25) _(15) '!' '<' '=' _(15) bf(25).
	p(NT(107), (NT(25)+NT(15)+T(31)+T(7)+T(3)+NT(15)+NT(25)));
//G124: bf_nleq(106)         => __E_wff_40(107).
	p(NT(106), (NT(107)));
//G125: wff(18)              => bf_nleq(106).
	p(NT(18), (NT(106)));
//G126: __E_wff_41(109)      => bf(25) _(15) '>' _(15) bf(25).
	p(NT(109), (NT(25)+NT(15)+T(16)+NT(15)+NT(25)));
//G127: bf_greater(108)      => __E_wff_41(109).
	p(NT(108), (NT(109)));
//G128: wff(18)              => bf_greater(108).
	p(NT(18), (NT(108)));
//G129: __E_wff_42(111)      => bf(25) _(15) '<' '=' _(15) bf(25).
	p(NT(111), (NT(25)+NT(15)+T(7)+T(3)+NT(15)+NT(25)));
//G130: bf_less_equal(110)   => __E_wff_42(111).
	p(NT(110), (NT(111)));
//G131: wff(18)              => bf_less_equal(110).
	p(NT(18), (NT(110)));
//G132: __E_wff_43(113)      => bf(25) _(15) '<' _(15) bf(25).
	p(NT(113), (NT(25)+NT(15)+T(7)+NT(15)+NT(25)));
//G133: bf_less(112)         => __E_wff_43(113).
	p(NT(112), (NT(113)));
//G134: wff(18)              => bf_less(112).
	p(NT(18), (NT(112)));
//G135: wff(18)              => capture(24).
	p(NT(18), (NT(24)));
//G136: wff(18)              => bool_variable(114).
	p(NT(18), (NT(114)));
//G137: wff(18)              => uninterpreted_constant(115).
	p(NT(18), (NT(115)));
//G138: __E_bf_44(117)       => '(' _(15) bf(25) _(15) ')'.
	p(NT(117), (T(5)+NT(15)+NT(25)+NT(15)+T(6)));
//G139: bf_parenthesis(116)  => __E_bf_44(117).
	p(NT(116), (NT(117)));
//G140: bf(25)               => bf_parenthesis(116).
	p(NT(25), (NT(116)));
//G141: __E_bf_45(119)       => 'S' _(15) '(' _(15) bf(25) _(15) ')'.
	p(NT(119), (T(34)+NT(15)+T(5)+NT(15)+NT(25)+NT(15)+T(6)));
//G142: bf_splitter(118)     => __E_bf_45(119).
	p(NT(118), (NT(119)));
//G143: bf(25)               => bf_splitter(118).
	p(NT(25), (NT(118)));
//G144: bf_ref(120)          => ref(22).
	p(NT(120), (NT(22)));
//G145: bf(25)               => bf_ref(120).
	p(NT(25), (NT(120)));
//G146: __E_bf_46(122)       => bf(25) _(15) '|' _(15) bf(25).
	p(NT(122), (NT(25)+NT(15)+T(28)+NT(15)+NT(25)));
//G147: bf_or(121)           => __E_bf_46(122).
	p(NT(121), (NT(122)));
//G148: bf(25)               => bf_or(121).
	p(NT(25), (NT(121)));
//G149: __E_bf_47(124)       => bf(25) _(15) '+' _(15) bf(25).
	p(NT(124), (NT(25)+NT(15)+T(35)+NT(15)+NT(25)));
//G150: bf_xor(123)          => __E_bf_47(124).
	p(NT(123), (NT(124)));
//G151: bf(25)               => bf_xor(123).
	p(NT(25), (NT(123)));
//G152: __E___E_bf_48_49(127) => _(15).
	p(NT(127), (NT(15)));
//G153: __E___E_bf_48_49(127) => _(15) '&' _(15).
	p(NT(127), (NT(15)+T(30)+NT(15)));
//G154: __E_bf_48(126)       => bf(25) __E___E_bf_48_49(127) bf(25).
	p(NT(126), (NT(25)+NT(127)+NT(25)));
//G155: bf_and(125)          => __E_bf_48(126).
	p(NT(125), (NT(126)));
//G156: bf(25)               => bf_and(125).
	p(NT(25), (NT(125)));
//G157: __E_bf_50(129)       => bf(25) _(15) '\''.
	p(NT(129), (NT(25)+NT(15)+T(36)));
//G158: bf_neg(128)          => __E_bf_50(129).
	p(NT(128), (NT(129)));
//G159: bf(25)               => bf_neg(128).
	p(NT(25), (NT(128)));
//G160: __E_bf_51(131)       => '{' _(15) constant(132) _(15) '}'.
	p(NT(131), (T(8)+NT(15)+NT(132)+NT(15)+T(9)));
//G161: bf_constant(130)     => __E_bf_51(131).
	p(NT(130), (NT(131)));
//G162: bf(25)               => bf_constant(130).
	p(NT(25), (NT(130)));
//G163: bf_t(133)            => '1'.
	p(NT(133), (T(37)));
//G164: bf(25)               => bf_t(133).
	p(NT(25), (NT(133)));
//G165: bf_f(134)            => '0'.
	p(NT(134), (T(38)));
//G166: bf(25)               => bf_f(134).
	p(NT(25), (NT(134)));
//G167: bf(25)               => capture(24).
	p(NT(25), (NT(24)));
//G168: bf(25)               => variable(135).
	p(NT(25), (NT(135)));
//G169: bf(25)               => uninterpreted_constant(115).
	p(NT(25), (NT(115)));
//G170: __N_0(403)           => 'F'.
	p(NT(403), (T(33)));
//G171: __N_1(404)           => 'T'.
	p(NT(404), (T(32)));
//G172: __N_2(405)           => 'i'.
	p(NT(405), (T(15)));
//G173: __N_3(406)           => 'o'.
	p(NT(406), (T(11)));
//G174: __E_charvar_52(137)  => ~( __N_0(403) ) & ~( __N_1(404) ) & ~( __N_2(405) ) & ~( __N_3(406) ) & alpha(5).	 # conjunctive
	p(NT(137), ~(NT(403)) & ~(NT(404)) & ~(NT(405)) & ~(NT(406)) & (NT(5)));
//G175: __E_charvar_53(138)  => null.
	p(NT(138), (nul));
//G176: __E_charvar_53(138)  => digit(3) __E_charvar_53(138).
	p(NT(138), (NT(3)+NT(138)));
//G177: charvar(136)         => __E_charvar_52(137) __E_charvar_53(138).
	p(NT(136), (NT(137)+NT(138)));
//G178: bool_variable(114)   => '?' charvar(136).
	p(NT(114), (T(23)+NT(136)));
//G179: variable(135)        => charvar(136).
	p(NT(135), (NT(136)));
//G180: __E_variable_54(140) => in(38).
	p(NT(140), (NT(38)));
//G181: __E_variable_54(140) => out(141).
	p(NT(140), (NT(141)));
//G182: io_var(139)          => __E_variable_54(140).
	p(NT(139), (NT(140)));
//G183: variable(135)        => io_var(139).
	p(NT(135), (NT(139)));
//G184: __E_in_55(143)       => 'i' digits(144).
	p(NT(143), (T(15)+NT(144)));
//G185: in_var_name(142)     => __E_in_55(143).
	p(NT(142), (NT(143)));
//G186: in(38)               => in_var_name(142) '[' offset(145) ']'.
	p(NT(38), (NT(142)+T(21)+NT(145)+T(22)));
//G187: __E_out_56(147)      => 'o' digits(144).
	p(NT(147), (T(11)+NT(144)));
//G188: out_var_name(146)    => __E_out_56(147).
	p(NT(146), (NT(147)));
//G189: out(141)             => out_var_name(146) '[' offset(145) ']'.
	p(NT(141), (NT(146)+T(21)+NT(145)+T(22)));
//G190: capture(24)          => '$' charvar(136).
	p(NT(24), (T(39)+NT(136)));
//G191: q_bool_var(77)       => capture(24).
	p(NT(77), (NT(24)));
//G192: q_bool_var(77)       => bool_variable(114).
	p(NT(77), (NT(114)));
//G193: q_var(82)            => capture(24).
	p(NT(82), (NT(24)));
//G194: q_var(82)            => variable(135).
	p(NT(82), (NT(135)));
//G195: __E_offsets_57(148)  => _(15) ',' _(15) offset(145).
	p(NT(148), (NT(15)+T(4)+NT(15)+NT(145)));
//G196: __E_offsets_58(149)  => null.
	p(NT(149), (nul));
//G197: __E_offsets_58(149)  => __E_offsets_57(148) __E_offsets_58(149).
	p(NT(149), (NT(148)+NT(149)));
//G198: offsets(28)          => '[' _(15) offset(145) __E_offsets_58(149) _(15) ']'.
	p(NT(28), (T(21)+NT(15)+NT(145)+NT(149)+NT(15)+T(22)));
//G199: offset(145)          => num(150).
	p(NT(145), (NT(150)));
//G200: offset(145)          => capture(24).
	p(NT(145), (NT(24)));
//G201: offset(145)          => shift(151).
	p(NT(145), (NT(151)));
//G202: __N_4(407)           => io_var(139).
	p(NT(407), (NT(139)));
//G203: __E_offset_59(152)   => variable(135) & ~( __N_4(407) ).	 # conjunctive
	p(NT(152), (NT(135)) & ~(NT(407)));
//G204: offset(145)          => __E_offset_59(152).
	p(NT(145), (NT(152)));
//G205: __E_shift_60(153)    => capture(24).
	p(NT(153), (NT(24)));
//G206: __N_5(408)           => io_var(139).
	p(NT(408), (NT(139)));
//G207: __E___E_shift_60_61(154) => variable(135) & ~( __N_5(408) ).	 # conjunctive
	p(NT(154), (NT(135)) & ~(NT(408)));
//G208: __E_shift_60(153)    => __E___E_shift_60_61(154).
	p(NT(153), (NT(154)));
//G209: shift(151)           => __E_shift_60(153) _(15) '-' _(15) num(150).
	p(NT(151), (NT(153)+NT(15)+T(27)+NT(15)+NT(150)));
//G210: num(150)             => digits(144).
	p(NT(150), (NT(144)));
//G211: __E___E_uninterpreted_constant_62_63(157) => chars(158).
	p(NT(157), (NT(158)));
//G212: __E___E_uninterpreted_constant_62_63(157) => _(15).
	p(NT(157), (NT(15)));
//G213: __E_uninterpreted_constant_62(156) => __E___E_uninterpreted_constant_62_63(157) ':' chars(158).
	p(NT(156), (NT(157)+T(2)+NT(158)));
//G214: uninter_const_name(155) => __E_uninterpreted_constant_62(156).
	p(NT(155), (NT(156)));
//G215: uninterpreted_constant(115) => '<' uninter_const_name(155) _(15) '>'.
	p(NT(115), (T(7)+NT(155)+NT(15)+T(16)));
//G216: constant(132)        => binding(159).
	p(NT(132), (NT(159)));
//G217: constant(132)        => capture(24).
	p(NT(132), (NT(24)));
//G218: __E___E_binding_64_65(162) => chars(158) _(15).
	p(NT(162), (NT(158)+NT(15)));
//G219: __E___E_binding_64_65(162) => _(15).
	p(NT(162), (NT(15)));
//G220: type(161)            => __E___E_binding_64_65(162).
	p(NT(161), (NT(162)));
//G221: __E_binding_64(160)  => type(161) ':' _(15) source(163).
	p(NT(160), (NT(161)+T(2)+NT(15)+NT(163)));
//G222: source_binding(39)   => __E_binding_64(160).
	p(NT(39), (NT(160)));
//G223: binding(159)         => source_binding(39).
	p(NT(159), (NT(39)));
//G224: named_binding(164)   => chars(158).
	p(NT(164), (NT(158)));
//G225: binding(159)         => named_binding(164).
	p(NT(159), (NT(164)));
//G226: __E_source_66(166)   => alnum(6).
	p(NT(166), (NT(6)));
//G227: __E_source_66(166)   => punct(7).
	p(NT(166), (NT(7)));
//G228: src_c(165)           => __E_source_66(166).
	p(NT(165), (NT(166)));
//G229: __E___E_source_67_68(168) => src_c(165).
	p(NT(168), (NT(165)));
//G230: __E___E_source_67_68(168) => space(2).
	p(NT(168), (NT(2)));
//G231: __E___E_source_67_69(169) => null.
	p(NT(169), (nul));
//G232: __E___E_source_67_69(169) => __E___E_source_67_68(168) __E___E_source_67_69(169).
	p(NT(169), (NT(168)+NT(169)));
//G233: __E_source_67(167)   => __E___E_source_67_69(169) src_c(165).
	p(NT(167), (NT(169)+NT(165)));
//G234: __E_source_67(167)   => null.
	p(NT(167), (nul));
//G235: source(163)          => src_c(165) __E_source_67(167).
	p(NT(163), (NT(165)+NT(167)));
//G236: __E_chars_70(170)    => null.
	p(NT(170), (nul));
//G237: __E_chars_70(170)    => alnum(6) __E_chars_70(170).
	p(NT(170), (NT(6)+NT(170)));
//G238: chars(158)           => alpha(5) __E_chars_70(170).
	p(NT(158), (NT(5)+NT(170)));
//G239: __E_digits_71(171)   => digit(3).
	p(NT(171), (NT(3)));
//G240: __E_digits_71(171)   => digit(3) __E_digits_71(171).
	p(NT(171), (NT(3)+NT(171)));
//G241: digits(144)          => __E_digits_71(171).
	p(NT(144), (NT(171)));
//G242: sym(26)              => chars(158).
	p(NT(26), (NT(158)));
//G243: __E_comment_72(173)  => printable(8).
	p(NT(173), (NT(8)));
//G244: __E_comment_72(173)  => '\t'.
	p(NT(173), (T(40)));
//G245: __E_comment_73(174)  => null.
	p(NT(174), (nul));
//G246: __E_comment_73(174)  => __E_comment_72(173) __E_comment_73(174).
	p(NT(174), (NT(173)+NT(174)));
//G247: __E_comment_74(175)  => '\n'.
	p(NT(175), (T(41)));
//G248: __E_comment_74(175)  => '\r'.
	p(NT(175), (T(42)));
//G249: __E_comment_74(175)  => eof(1).
	p(NT(175), (NT(1)));
//G250: comment(172)         => '#' __E_comment_73(174) __E_comment_74(175).
	p(NT(172), (T(43)+NT(174)+NT(175)));
//G251: __E____75(176)       => space(2).
	p(NT(176), (NT(2)));
//G252: __E____75(176)       => comment(172).
	p(NT(176), (NT(172)));
//G253: __(59)               => __E____75(176) _(15).
	p(NT(59), (NT(176)+NT(15)));
//G254: __E___76(177)        => __(59).
	p(NT(177), (NT(59)));
//G255: __E___76(177)        => null.
	p(NT(177), (nul));
//G256: _(15)                => __E___76(177).
	p(NT(15), (NT(177)));
//G257: bf_cb_arg(179)       => bf(25).
	p(NT(179), (NT(25)));
//G258: bf_cb_args1(178)     => __(59) bf_cb_arg(179).
	p(NT(178), (NT(59)+NT(179)));
//G259: bf_cb_args2(180)     => bf_cb_args1(178) bf_cb_args1(178).
	p(NT(180), (NT(178)+NT(178)));
//G260: bf_cb_args3(181)     => bf_cb_args2(180) bf_cb_args1(178).
	p(NT(181), (NT(180)+NT(178)));
//G261: bf_cb_args4(182)     => bf_cb_args3(181) bf_cb_args1(178).
	p(NT(182), (NT(181)+NT(178)));
//G262: wff_cb_arg(184)      => wff(18).
	p(NT(184), (NT(18)));
//G263: wff_cb_args1(183)    => __(59) wff_cb_arg(184).
	p(NT(183), (NT(59)+NT(184)));
//G264: wff_cb_args2(185)    => wff_cb_args1(183) wff_cb_args1(183).
	p(NT(185), (NT(183)+NT(183)));
//G265: wff_cb_args3(186)    => wff_cb_args2(185) wff_cb_args1(183).
	p(NT(186), (NT(185)+NT(183)));
//G266: wff_cb_args4(187)    => wff_cb_args3(186) wff_cb_args1(183).
	p(NT(187), (NT(186)+NT(183)));
//G267: __E_wff_cb_77(189)   => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b' bf_cb_args1(178) wff_cb_args2(185).
	p(NT(189), (T(24)+T(44)+T(25)+T(13)+T(45)+T(25)+T(46)+T(24)+NT(178)+NT(185)));
//G268: bf_eq_cb(188)        => __E_wff_cb_77(189).
	p(NT(188), (NT(189)));
//G269: wff_cb(51)           => bf_eq_cb(188).
	p(NT(51), (NT(188)));
//G270: __E_wff_cb_78(191)   => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b' bf_cb_args1(178) wff_cb_args2(185).
	p(NT(191), (T(24)+T(44)+T(25)+T(47)+T(13)+T(45)+T(25)+T(46)+T(24)+NT(178)+NT(185)));
//G271: bf_neq_cb(190)       => __E_wff_cb_78(191).
	p(NT(190), (NT(191)));
//G272: wff_cb(51)           => bf_neq_cb(190).
	p(NT(51), (NT(190)));
//G273: __E_wff_cb_79(193)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(185).
	p(NT(193), (T(19)+T(44)+T(44)+T(25)+T(48)+T(17)+T(10)+T(25)+T(46)+T(18)+T(17)+T(10)+T(48)+T(15)+T(47)+T(49)+T(25)+T(10)+T(50)+T(24)+T(44)+T(11)+T(51)+T(12)+T(50)+T(18)+T(17)+T(10)+T(25)+T(46)+T(24)+NT(185)));
//G274: wff_has_clashing_subformulas_cb(192) => __E_wff_cb_79(193).
	p(NT(192), (NT(193)));
//G275: wff_cb(51)           => wff_has_clashing_subformulas_cb(192).
	p(NT(51), (NT(192)));
//G276: __E_wff_cb_80(195)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(186).
	p(NT(195), (T(19)+T(44)+T(44)+T(25)+T(48)+T(17)+T(10)+T(25)+T(10)+T(50)+T(24)+T(44)+T(11)+T(51)+T(12)+T(50)+T(18)+T(17)+T(25)+T(46)+T(24)+NT(186)));
//G277: wff_has_subformula_cb(194) => __E_wff_cb_80(195).
	p(NT(194), (NT(195)));
//G278: wff_cb(51)           => wff_has_subformula_cb(194).
	p(NT(51), (NT(194)));
//G279: __E_wff_cb_81(197)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(185).
	p(NT(197), (T(19)+T(44)+T(44)+T(25)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(25)+T(13)+T(26)+T(15)+T(10)+T(14)+T(13)+T(47)+T(14)+T(15)+T(17)+T(18)+T(25)+T(46)+T(24)+NT(185)));
//G280: wff_remove_existential_cb(196) => __E_wff_cb_81(197).
	p(NT(196), (NT(197)));
//G281: wff_cb(51)           => wff_remove_existential_cb(196).
	p(NT(51), (NT(196)));
//G282: __E_wff_cb_82(199)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args4(187).
	p(NT(199), (T(19)+T(44)+T(44)+T(25)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(25)+T(24)+T(13)+T(26)+T(15)+T(10)+T(14)+T(13)+T(47)+T(14)+T(15)+T(17)+T(18)+T(25)+T(46)+T(24)+NT(187)));
//G283: wff_remove_bexistential_cb(198) => __E_wff_cb_82(199).
	p(NT(198), (NT(199)));
//G284: wff_cb(51)           => wff_remove_bexistential_cb(198).
	p(NT(51), (NT(198)));
//G285: __E_wff_cb_83(201)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' wff_cb_args4(187).
	p(NT(201), (T(19)+T(44)+T(44)+T(25)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(25)+T(24)+T(50)+T(47)+T(15)+T(52)+T(13)+T(51)+T(10)+T(17)+T(18)+T(25)+T(46)+T(24)+NT(187)));
//G286: wff_remove_buniversal_cb(200) => __E_wff_cb_83(201).
	p(NT(200), (NT(201)));
//G287: wff_cb(51)           => wff_remove_buniversal_cb(200).
	p(NT(51), (NT(200)));
//G288: __E_bf_cb_84(203)    => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b' bf_cb_args2(180).
	p(NT(203), (T(24)+T(44)+T(25)+T(15)+T(10)+T(25)+T(53)+T(13)+T(51)+T(11)+T(25)+T(46)+T(24)+NT(180)));
//G289: bf_is_zero_cb(202)   => __E_bf_cb_84(203).
	p(NT(202), (NT(203)));
//G290: bf_cb(55)            => bf_is_zero_cb(202).
	p(NT(55), (NT(202)));
//G291: __E_bf_cb_85(205)    => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b' bf_cb_args2(180).
	p(NT(205), (T(24)+T(44)+T(25)+T(15)+T(10)+T(25)+T(11)+T(47)+T(13)+T(25)+T(46)+T(24)+NT(180)));
//G292: bf_is_one_cb(204)    => __E_bf_cb_85(205).
	p(NT(204), (NT(205)));
//G293: bf_cb(55)            => bf_is_one_cb(204).
	p(NT(55), (NT(204)));
//G294: __E_bf_cb_86(207)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(181).
	p(NT(207), (T(24)+T(44)+T(25)+T(48)+T(17)+T(10)+T(25)+T(10)+T(50)+T(24)+T(44)+T(11)+T(51)+T(12)+T(50)+T(18)+T(17)+T(25)+T(46)+T(24)+NT(181)));
//G295: bf_has_subformula_cb(206) => __E_bf_cb_86(207).
	p(NT(206), (NT(207)));
//G296: bf_cb(55)            => bf_has_subformula_cb(206).
	p(NT(55), (NT(206)));
//G297: __E_bf_cb_87(209)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(182).
	p(NT(209), (T(24)+T(44)+T(25)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(25)+T(44)+T(50)+T(47)+T(15)+T(52)+T(13)+T(51)+T(10)+T(17)+T(18)+T(25)+T(46)+T(24)+NT(182)));
//G298: bf_remove_funiversal_cb(208) => __E_bf_cb_87(209).
	p(NT(208), (NT(209)));
//G299: bf_cb(55)            => bf_remove_funiversal_cb(208).
	p(NT(55), (NT(208)));
//G300: __E_bf_cb_88(211)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(182).
	p(NT(211), (T(24)+T(44)+T(25)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(25)+T(44)+T(13)+T(26)+T(15)+T(10)+T(14)+T(13)+T(47)+T(14)+T(15)+T(17)+T(18)+T(25)+T(46)+T(24)+NT(182)));
//G301: bf_remove_fexistential_cb(210) => __E_bf_cb_88(211).
	p(NT(210), (NT(211)));
//G302: bf_cb(55)            => bf_remove_fexistential_cb(210).
	p(NT(55), (NT(210)));
//G303: __E_bf_cb_89(213)    => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b' bf_cb_args2(180).
	p(NT(213), (T(24)+T(44)+T(25)+T(17)+T(47)+T(54)+T(25)+T(46)+T(24)+NT(180)));
//G304: bf_and_cb(212)       => __E_bf_cb_89(213).
	p(NT(212), (NT(213)));
//G305: bf_cb(55)            => bf_and_cb(212).
	p(NT(55), (NT(212)));
//G306: __E_bf_cb_90(215)    => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b' bf_cb_args2(180).
	p(NT(215), (T(24)+T(44)+T(25)+T(11)+T(51)+T(25)+T(46)+T(24)+NT(180)));
//G307: bf_or_cb(214)        => __E_bf_cb_90(215).
	p(NT(214), (NT(215)));
//G308: bf_cb(55)            => bf_or_cb(214).
	p(NT(55), (NT(214)));
//G309: __E_bf_cb_91(217)    => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b' bf_cb_args2(180).
	p(NT(217), (T(24)+T(44)+T(25)+T(26)+T(11)+T(51)+T(25)+T(46)+T(24)+NT(180)));
//G310: bf_xor_cb(216)       => __E_bf_cb_91(217).
	p(NT(216), (NT(217)));
//G311: bf_cb(55)            => bf_xor_cb(216).
	p(NT(55), (NT(216)));
//G312: __E_bf_cb_92(219)    => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b' bf_cb_args1(178).
	p(NT(219), (T(24)+T(44)+T(25)+T(47)+T(13)+T(49)+T(25)+T(46)+T(24)+NT(178)));
//G313: bf_neg_cb(218)       => __E_bf_cb_92(219).
	p(NT(218), (NT(219)));
//G314: bf_cb(55)            => bf_neg_cb(218).
	p(NT(55), (NT(218)));
//G315: __E_bf_cb_93(221)    => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(178).
	p(NT(221), (T(24)+T(44)+T(25)+T(47)+T(11)+T(51)+T(12)+T(17)+T(18)+T(15)+T(53)+T(13)+T(25)+T(46)+T(24)+NT(178)));
//G316: bf_normalize_cb(220) => __E_bf_cb_93(221).
	p(NT(220), (NT(221)));
//G317: bf_cb(55)            => bf_normalize_cb(220).
	p(NT(55), (NT(220)));
//G318: cli(222)             => _(15).
	p(NT(222), (NT(15)));
//G319: __E_cli_94(224)      => '.' _(15) cli_command(223) _(15).
	p(NT(224), (T(1)+NT(15)+NT(223)+NT(15)));
//G320: __E_cli_95(225)      => null.
	p(NT(225), (nul));
//G321: __E_cli_95(225)      => __E_cli_94(224) __E_cli_95(225).
	p(NT(225), (NT(224)+NT(225)));
//G322: cli(222)             => _(15) cli_command(223) _(15) __E_cli_95(225).
	p(NT(222), (NT(15)+NT(223)+NT(15)+NT(225)));
//G323: __E_cli_command_96(228) => 'q'.
	p(NT(228), (T(45)));
//G324: __E_cli_command_96(228) => 'q' 'u' 'i' 't'.
	p(NT(228), (T(45)+T(50)+T(15)+T(14)));
//G325: quit_sym(227)        => __E_cli_command_96(228).
	p(NT(227), (NT(228)));
//G326: quit_cmd(226)        => quit_sym(227).
	p(NT(226), (NT(227)));
//G327: cli_command(223)     => quit_cmd(226).
	p(NT(223), (NT(226)));
//G328: __E_cli_command_97(231) => 'v'.
	p(NT(231), (T(52)));
//G329: __E_cli_command_97(231) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(231), (T(52)+T(13)+T(51)+T(10)+T(15)+T(11)+T(47)));
//G330: version_sym(230)     => __E_cli_command_97(231).
	p(NT(230), (NT(231)));
//G331: version_cmd(229)     => version_sym(230).
	p(NT(229), (NT(230)));
//G332: cli_command(223)     => version_cmd(229).
	p(NT(223), (NT(229)));
//G333: __E_cli_command_98(234) => 'c'.
	p(NT(234), (T(46)));
//G334: __E_cli_command_98(234) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(234), (T(46)+T(18)+T(13)+T(17)+T(51)));
//G335: clear_sym(233)       => __E_cli_command_98(234).
	p(NT(233), (NT(234)));
//G336: clear_cmd(232)       => clear_sym(233).
	p(NT(232), (NT(233)));
//G337: cli_command(223)     => clear_cmd(232).
	p(NT(223), (NT(232)));
//G338: __E___E_cli_command_99_100(238) => 'h'.
	p(NT(238), (T(48)));
//G339: __E___E_cli_command_99_100(238) => 'h' 'e' 'l' 'p'.
	p(NT(238), (T(48)+T(13)+T(18)+T(55)));
//G340: help_sym(237)        => __E___E_cli_command_99_100(238).
	p(NT(237), (NT(238)));
//G341: __E___E_cli_command_99_101(239) => __(59) help_arg(240).
	p(NT(239), (NT(59)+NT(240)));
//G342: __E___E_cli_command_99_101(239) => null.
	p(NT(239), (nul));
//G343: __E_cli_command_99(236) => help_sym(237) __E___E_cli_command_99_101(239).
	p(NT(236), (NT(237)+NT(239)));
//G344: help_cmd(235)        => __E_cli_command_99(236).
	p(NT(235), (NT(236)));
//G345: cli_command(223)     => help_cmd(235).
	p(NT(223), (NT(235)));
//G346: file_sym(243)        => 'f' 'i' 'l' 'e'.
	p(NT(243), (T(44)+T(15)+T(18)+T(13)));
//G347: __E_cli_command_102(242) => file_sym(243) __(59) q_string(244).
	p(NT(242), (NT(243)+NT(59)+NT(244)));
//G348: file_cmd(241)        => __E_cli_command_102(242).
	p(NT(241), (NT(242)));
//G349: cli_command(223)     => file_cmd(241).
	p(NT(223), (NT(241)));
//G350: valid_sym(247)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(247), (T(52)+T(17)+T(18)+T(15)+T(54)));
//G351: __E_cli_command_103(246) => valid_sym(247) __(59) rr(14).
	p(NT(246), (NT(247)+NT(59)+NT(14)));
//G352: valid_cmd(245)       => __E_cli_command_103(246).
	p(NT(245), (NT(246)));
//G353: cli_command(223)     => valid_cmd(245).
	p(NT(223), (NT(245)));
//G354: sat_sym(250)         => 's' 'a' 't'.
	p(NT(250), (T(10)+T(17)+T(14)));
//G355: __E_cli_command_104(249) => sat_sym(250) __(59) rr(14).
	p(NT(249), (NT(250)+NT(59)+NT(14)));
//G356: sat_cmd(248)         => __E_cli_command_104(249).
	p(NT(248), (NT(249)));
//G357: cli_command(223)     => sat_cmd(248).
	p(NT(223), (NT(248)));
//G358: unsat_sym(253)       => 'u' 'n' 's' 'a' 't'.
	p(NT(253), (T(50)+T(47)+T(10)+T(17)+T(14)));
//G359: __E_cli_command_105(252) => unsat_sym(253) __(59) rr(14).
	p(NT(252), (NT(253)+NT(59)+NT(14)));
//G360: unsat_cmd(251)       => __E_cli_command_105(252).
	p(NT(251), (NT(252)));
//G361: cli_command(223)     => unsat_cmd(251).
	p(NT(223), (NT(251)));
//G362: solve_sym(256)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(256), (T(10)+T(11)+T(18)+T(52)+T(13)));
//G363: __E_cli_command_106(255) => solve_sym(256) __(59) wff_cmd_arg(257).
	p(NT(255), (NT(256)+NT(59)+NT(257)));
//G364: solve_cmd(254)       => __E_cli_command_106(255).
	p(NT(254), (NT(255)));
//G365: cli_command(223)     => solve_cmd(254).
	p(NT(223), (NT(254)));
//G366: __E___E_cli_command_107_108(261) => 'e'.
	p(NT(261), (T(13)));
//G367: __E___E_cli_command_107_108(261) => 'e' 'x' 'e' 'c' 'u' 't' 'e'.
	p(NT(261), (T(13)+T(26)+T(13)+T(46)+T(50)+T(14)+T(13)));
//G368: execute_sym(260)     => __E___E_cli_command_107_108(261).
	p(NT(260), (NT(261)));
//G369: __E___E_cli_command_107_109(262) => wff_typed(263).
	p(NT(262), (NT(263)));
//G370: __E___E_cli_command_107_109(262) => rr(14).
	p(NT(262), (NT(14)));
//G371: __E_cli_command_107(259) => execute_sym(260) __(59) __E___E_cli_command_107_109(262).
	p(NT(259), (NT(260)+NT(59)+NT(262)));
//G372: execute_cmd(258)     => __E_cli_command_107(259).
	p(NT(258), (NT(259)));
//G373: cli_command(223)     => execute_cmd(258).
	p(NT(223), (NT(258)));
//G374: __E___E_cli_command_110_111(267) => 'n'.
	p(NT(267), (T(47)));
//G375: __E___E_cli_command_110_111(267) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(267), (T(47)+T(11)+T(51)+T(12)+T(17)+T(18)+T(15)+T(53)+T(13)));
//G376: normalize_sym(266)   => __E___E_cli_command_110_111(267).
	p(NT(266), (NT(267)));
//G377: __E_cli_command_110(265) => normalize_sym(266) __(59) normalize_cmd_arg(268).
	p(NT(265), (NT(266)+NT(59)+NT(268)));
//G378: normalize_cmd(264)   => __E_cli_command_110(265).
	p(NT(264), (NT(265)));
//G379: cli_command(223)     => normalize_cmd(264).
	p(NT(223), (NT(264)));
//G380: __E___E_cli_command_112_113(272) => 's'.
	p(NT(272), (T(10)));
//G381: __E___E_cli_command_112_113(272) => 's' 'u' 'b' 's' 't'.
	p(NT(272), (T(10)+T(50)+T(24)+T(10)+T(14)));
//G382: __E___E_cli_command_112_113(272) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(272), (T(10)+T(50)+T(24)+T(10)+T(14)+T(15)+T(14)+T(50)+T(14)+T(13)));
//G383: subst_sym(271)       => __E___E_cli_command_112_113(272).
	p(NT(271), (NT(272)));
//G384: __E_cli_command_112(270) => subst_sym(271) __(59) nf_cmd_arg(273) _(15) '[' _(15) nf_cmd_arg(273) _(15) '/' _(15) nf_cmd_arg(273) _(15) ']'.
	p(NT(270), (NT(271)+NT(59)+NT(273)+NT(15)+T(21)+NT(15)+NT(273)+NT(15)+T(56)+NT(15)+NT(273)+NT(15)+T(22)));
//G385: subst_cmd(269)       => __E_cli_command_112(270).
	p(NT(269), (NT(270)));
//G386: cli_command(223)     => subst_cmd(269).
	p(NT(223), (NT(269)));
//G387: __E___E_cli_command_114_115(277) => 'i'.
	p(NT(277), (T(15)));
//G388: __E___E_cli_command_114_115(277) => 'i' 'n' 's' 't'.
	p(NT(277), (T(15)+T(47)+T(10)+T(14)));
//G389: __E___E_cli_command_114_115(277) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(277), (T(15)+T(47)+T(10)+T(14)+T(17)+T(47)+T(14)+T(15)+T(17)+T(14)+T(13)));
//G390: inst_sym(276)        => __E___E_cli_command_114_115(277).
	p(NT(276), (NT(277)));
//G391: __E_cli_command_114(275) => inst_sym(276) __(59) inst_args(278).
	p(NT(275), (NT(276)+NT(59)+NT(278)));
//G392: inst_cmd(274)        => __E_cli_command_114(275).
	p(NT(274), (NT(275)));
//G393: cli_command(223)     => inst_cmd(274).
	p(NT(223), (NT(274)));
//G394: dnf_sym(281)         => 'd' 'n' 'f'.
	p(NT(281), (T(54)+T(47)+T(44)));
//G395: __E_cli_command_116(280) => dnf_sym(281) __(59) nf_cmd_arg(273).
	p(NT(280), (NT(281)+NT(59)+NT(273)));
//G396: dnf_cmd(279)         => __E_cli_command_116(280).
	p(NT(279), (NT(280)));
//G397: cli_command(223)     => dnf_cmd(279).
	p(NT(223), (NT(279)));
//G398: cnf_sym(284)         => 'c' 'n' 'f'.
	p(NT(284), (T(46)+T(47)+T(44)));
//G399: __E_cli_command_117(283) => cnf_sym(284) __(59) nf_cmd_arg(273).
	p(NT(283), (NT(284)+NT(59)+NT(273)));
//G400: cnf_cmd(282)         => __E_cli_command_117(283).
	p(NT(282), (NT(283)));
//G401: cli_command(223)     => cnf_cmd(282).
	p(NT(223), (NT(282)));
//G402: anf_sym(287)         => 'a' 'n' 'f'.
	p(NT(287), (T(17)+T(47)+T(44)));
//G403: __E_cli_command_118(286) => anf_sym(287) __(59) nf_cmd_arg(273).
	p(NT(286), (NT(287)+NT(59)+NT(273)));
//G404: anf_cmd(285)         => __E_cli_command_118(286).
	p(NT(285), (NT(286)));
//G405: cli_command(223)     => anf_cmd(285).
	p(NT(223), (NT(285)));
//G406: nnf_sym(290)         => 'n' 'n' 'f'.
	p(NT(290), (T(47)+T(47)+T(44)));
//G407: __E_cli_command_119(289) => nnf_sym(290) __(59) nf_cmd_arg(273).
	p(NT(289), (NT(290)+NT(59)+NT(273)));
//G408: nnf_cmd(288)         => __E_cli_command_119(289).
	p(NT(288), (NT(289)));
//G409: cli_command(223)     => nnf_cmd(288).
	p(NT(223), (NT(288)));
//G410: pnf_sym(293)         => 'p' 'n' 'f'.
	p(NT(293), (T(55)+T(47)+T(44)));
//G411: __E_cli_command_120(292) => pnf_sym(293) __(59) nf_cmd_arg(273).
	p(NT(292), (NT(293)+NT(59)+NT(273)));
//G412: pnf_cmd(291)         => __E_cli_command_120(292).
	p(NT(291), (NT(292)));
//G413: cli_command(223)     => pnf_cmd(291).
	p(NT(223), (NT(291)));
//G414: mnf_sym(296)         => 'm' 'n' 'f'.
	p(NT(296), (T(12)+T(47)+T(44)));
//G415: __E_cli_command_121(295) => mnf_sym(296) __(59) nf_cmd_arg(273).
	p(NT(295), (NT(296)+NT(59)+NT(273)));
//G416: mnf_cmd(294)         => __E_cli_command_121(295).
	p(NT(294), (NT(295)));
//G417: cli_command(223)     => mnf_cmd(294).
	p(NT(223), (NT(294)));
//G418: snf_sym(299)         => 's' 'n' 'f'.
	p(NT(299), (T(10)+T(47)+T(44)));
//G419: __E_cli_command_122(298) => snf_sym(299) __(59) nf_cmd_arg(273).
	p(NT(298), (NT(299)+NT(59)+NT(273)));
//G420: snf_cmd(297)         => __E_cli_command_122(298).
	p(NT(297), (NT(298)));
//G421: cli_command(223)     => snf_cmd(297).
	p(NT(223), (NT(297)));
//G422: onf_sym(302)         => 'o' 'n' 'f'.
	p(NT(302), (T(11)+T(47)+T(44)));
//G423: __E_cli_command_123(301) => onf_sym(302) __(59) variable(135) __(59) onf_cmd_arg(303).
	p(NT(301), (NT(302)+NT(59)+NT(135)+NT(59)+NT(303)));
//G424: onf_cmd(300)         => __E_cli_command_123(301).
	p(NT(300), (NT(301)));
//G425: cli_command(223)     => onf_cmd(300).
	p(NT(223), (NT(300)));
//G426: __E___E_cli_command_124_125(307) => 'd' 'e' 'f' 's'.
	p(NT(307), (T(54)+T(13)+T(44)+T(10)));
//G427: __E___E_cli_command_124_125(307) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(307), (T(54)+T(13)+T(44)+T(15)+T(47)+T(15)+T(14)+T(15)+T(11)+T(47)+T(10)));
//G428: def_sym(306)         => __E___E_cli_command_124_125(307).
	p(NT(306), (NT(307)));
//G429: __E_cli_command_124(305) => def_sym(306).
	p(NT(305), (NT(306)));
//G430: def_list_cmd(304)    => __E_cli_command_124(305).
	p(NT(304), (NT(305)));
//G431: cli_command(223)     => def_list_cmd(304).
	p(NT(223), (NT(304)));
//G432: __E_cli_command_126(309) => def_sym(306) __(59) number(310).
	p(NT(309), (NT(306)+NT(59)+NT(310)));
//G433: def_print_cmd(308)   => __E_cli_command_126(309).
	p(NT(308), (NT(309)));
//G434: cli_command(223)     => def_print_cmd(308).
	p(NT(223), (NT(308)));
//G435: def_rr_cmd(311)      => rec_relation(20).
	p(NT(311), (NT(20)));
//G436: cli_command(223)     => def_rr_cmd(311).
	p(NT(223), (NT(311)));
//G437: qelim_sym(314)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(314), (T(45)+T(13)+T(18)+T(15)+T(12)));
//G438: __E_cli_command_127(313) => qelim_sym(314) __(59) wff_cmd_arg(257).
	p(NT(313), (NT(314)+NT(59)+NT(257)));
//G439: qelim_cmd(312)       => __E_cli_command_127(313).
	p(NT(312), (NT(313)));
//G440: cli_command(223)     => qelim_cmd(312).
	p(NT(223), (NT(312)));
//G441: get_sym(317)         => 'g' 'e' 't'.
	p(NT(317), (T(49)+T(13)+T(14)));
//G442: __E___E_cli_command_128_129(318) => __(59) option(319).
	p(NT(318), (NT(59)+NT(319)));
//G443: __E___E_cli_command_128_129(318) => null.
	p(NT(318), (nul));
//G444: __E_cli_command_128(316) => get_sym(317) __E___E_cli_command_128_129(318).
	p(NT(316), (NT(317)+NT(318)));
//G445: get_cmd(315)         => __E_cli_command_128(316).
	p(NT(315), (NT(316)));
//G446: cli_command(223)     => get_cmd(315).
	p(NT(223), (NT(315)));
//G447: set_sym(322)         => 's' 'e' 't'.
	p(NT(322), (T(10)+T(13)+T(14)));
//G448: __E___E_cli_command_130_131(323) => __(59).
	p(NT(323), (NT(59)));
//G449: __E___E_cli_command_130_131(323) => _(15) '=' _(15).
	p(NT(323), (NT(15)+T(3)+NT(15)));
//G450: __E_cli_command_130(321) => set_sym(322) __(59) option(319) __E___E_cli_command_130_131(323) option_value(324).
	p(NT(321), (NT(322)+NT(59)+NT(319)+NT(323)+NT(324)));
//G451: set_cmd(320)         => __E_cli_command_130(321).
	p(NT(320), (NT(321)));
//G452: cli_command(223)     => set_cmd(320).
	p(NT(223), (NT(320)));
//G453: toggle_sym(327)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(327), (T(14)+T(11)+T(49)+T(49)+T(18)+T(13)));
//G454: __E_cli_command_132(326) => toggle_sym(327) __(59) bool_option(328).
	p(NT(326), (NT(327)+NT(59)+NT(328)));
//G455: toggle_cmd(325)      => __E_cli_command_132(326).
	p(NT(325), (NT(326)));
//G456: cli_command(223)     => toggle_cmd(325).
	p(NT(223), (NT(325)));
//G457: __E___E_cli_command_133_134(332) => 'h' 'i' 's' 't'.
	p(NT(332), (T(48)+T(15)+T(10)+T(14)));
//G458: __E___E_cli_command_133_134(332) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(332), (T(48)+T(15)+T(10)+T(14)+T(11)+T(51)+T(20)));
//G459: history_sym(331)     => __E___E_cli_command_133_134(332).
	p(NT(331), (NT(332)));
//G460: __E_cli_command_133(330) => history_sym(331).
	p(NT(330), (NT(331)));
//G461: history_list_cmd(329) => __E_cli_command_133(330).
	p(NT(329), (NT(330)));
//G462: cli_command(223)     => history_list_cmd(329).
	p(NT(223), (NT(329)));
//G463: __E_cli_command_135(334) => history_sym(331) __(59) memory(335).
	p(NT(334), (NT(331)+NT(59)+NT(335)));
//G464: history_print_cmd(333) => __E_cli_command_135(334).
	p(NT(333), (NT(334)));
//G465: cli_command(223)     => history_print_cmd(333).
	p(NT(223), (NT(333)));
//G466: __E_cli_command_136(337) => bf_typed(338).
	p(NT(337), (NT(338)));
//G467: __E_cli_command_136(337) => wff_typed(263).
	p(NT(337), (NT(263)));
//G468: history_store_cmd(336) => __E_cli_command_136(337).
	p(NT(336), (NT(337)));
//G469: cli_command(223)     => history_store_cmd(336).
	p(NT(223), (NT(336)));
//G470: number(310)          => digits(144).
	p(NT(310), (NT(144)));
//G471: bf_typed(338)        => bf(25).
	p(NT(338), (NT(25)));
//G472: bf_typed(338)        => bf(25) _(15) ':' _(15) 'b' 'f'.
	p(NT(338), (NT(25)+NT(15)+T(2)+NT(15)+T(24)+T(44)));
//G473: wff_typed(263)       => wff(18).
	p(NT(263), (NT(18)));
//G474: wff_typed(263)       => wff(18) _(15) ':' _(15) 'w' 'f' 'f'.
	p(NT(263), (NT(18)+NT(15)+T(2)+NT(15)+T(19)+T(44)+T(44)));
//G475: bf_cmd_arg(339)      => memory(335).
	p(NT(339), (NT(335)));
//G476: bf_cmd_arg(339)      => bf_typed(338).
	p(NT(339), (NT(338)));
//G477: wff_cmd_arg(257)     => memory(335).
	p(NT(257), (NT(335)));
//G478: wff_cmd_arg(257)     => wff_typed(263).
	p(NT(257), (NT(263)));
//G479: nf_cmd_arg(273)      => memory(335).
	p(NT(273), (NT(335)));
//G480: nf_cmd_arg(273)      => bf_typed(338).
	p(NT(273), (NT(338)));
//G481: nf_cmd_arg(273)      => wff_typed(263).
	p(NT(273), (NT(263)));
//G482: onf_cmd_arg(303)     => memory(335).
	p(NT(303), (NT(335)));
//G483: onf_cmd_arg(303)     => wff_typed(263).
	p(NT(303), (NT(263)));
//G484: normalize_cmd_arg(268) => memory(335).
	p(NT(268), (NT(335)));
//G485: normalize_cmd_arg(268) => bf_typed(338).
	p(NT(268), (NT(338)));
//G486: normalize_cmd_arg(268) => wff_typed(263).
	p(NT(268), (NT(263)));
//G487: normalize_cmd_arg(268) => rr(14).
	p(NT(268), (NT(14)));
//G488: inst_args(278)       => bf_cmd_arg(339) _(15) '[' _(15) variable(135) _(15) '/' _(15) bf_cmd_arg(339) _(15) ']'.
	p(NT(278), (NT(339)+NT(15)+T(21)+NT(15)+NT(135)+NT(15)+T(56)+NT(15)+NT(339)+NT(15)+T(22)));
//G489: inst_args(278)       => wff_cmd_arg(257) _(15) '[' _(15) variable(135) _(15) '/' _(15) bf_cmd_arg(339) _(15) ']'.
	p(NT(278), (NT(257)+NT(15)+T(21)+NT(15)+NT(135)+NT(15)+T(56)+NT(15)+NT(339)+NT(15)+T(22)));
//G490: inst_args(278)       => wff_cmd_arg(257) _(15) '[' _(15) bool_variable(114) _(15) '/' _(15) wff_cmd_arg(257) _(15) ']'.
	p(NT(278), (NT(257)+NT(15)+T(21)+NT(15)+NT(114)+NT(15)+T(56)+NT(15)+NT(257)+NT(15)+T(22)));
//G491: help_arg(240)        => help_sym(237).
	p(NT(240), (NT(237)));
//G492: help_arg(240)        => version_sym(230).
	p(NT(240), (NT(230)));
//G493: help_arg(240)        => quit_sym(227).
	p(NT(240), (NT(227)));
//G494: help_arg(240)        => clear_sym(233).
	p(NT(240), (NT(233)));
//G495: help_arg(240)        => get_sym(317).
	p(NT(240), (NT(317)));
//G496: help_arg(240)        => set_sym(322).
	p(NT(240), (NT(322)));
//G497: help_arg(240)        => toggle_sym(327).
	p(NT(240), (NT(327)));
//G498: help_arg(240)        => file_sym(243).
	p(NT(240), (NT(243)));
//G499: help_arg(240)        => history_sym(331).
	p(NT(240), (NT(331)));
//G500: help_arg(240)        => abs_memory_sym(340).
	p(NT(240), (NT(340)));
//G501: help_arg(240)        => rel_memory_sym(341).
	p(NT(240), (NT(341)));
//G502: help_arg(240)        => selection_sym(342).
	p(NT(240), (NT(342)));
//G503: help_arg(240)        => def_sym(306).
	p(NT(240), (NT(306)));
//G504: help_arg(240)        => inst_sym(276).
	p(NT(240), (NT(276)));
//G505: help_arg(240)        => subst_sym(271).
	p(NT(240), (NT(271)));
//G506: help_arg(240)        => normalize_sym(266).
	p(NT(240), (NT(266)));
//G507: help_arg(240)        => execute_sym(260).
	p(NT(240), (NT(260)));
//G508: help_arg(240)        => solve_sym(256).
	p(NT(240), (NT(256)));
//G509: help_arg(240)        => valid_sym(247).
	p(NT(240), (NT(247)));
//G510: help_arg(240)        => sat_sym(250).
	p(NT(240), (NT(250)));
//G511: help_arg(240)        => unsat_sym(253).
	p(NT(240), (NT(253)));
//G512: help_arg(240)        => dnf_sym(281).
	p(NT(240), (NT(281)));
//G513: help_arg(240)        => cnf_sym(284).
	p(NT(240), (NT(284)));
//G514: help_arg(240)        => anf_sym(287).
	p(NT(240), (NT(287)));
//G515: help_arg(240)        => snf_sym(299).
	p(NT(240), (NT(299)));
//G516: help_arg(240)        => nnf_sym(290).
	p(NT(240), (NT(290)));
//G517: help_arg(240)        => pnf_sym(293).
	p(NT(240), (NT(293)));
//G518: help_arg(240)        => mnf_sym(296).
	p(NT(240), (NT(296)));
//G519: help_arg(240)        => onf_sym(302).
	p(NT(240), (NT(302)));
//G520: help_arg(240)        => qelim_sym(314).
	p(NT(240), (NT(314)));
//G521: __E___E_help_arg_137_138(345) => 's'.
	p(NT(345), (T(10)));
//G522: __E___E_help_arg_137_138(345) => null.
	p(NT(345), (nul));
//G523: __E_help_arg_137(344) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_137_138(345).
	p(NT(344), (T(13)+T(26)+T(17)+T(12)+T(55)+T(18)+T(13)+NT(345)));
//G524: examples_sym(343)    => __E_help_arg_137(344).
	p(NT(343), (NT(344)));
//G525: help_arg(240)        => examples_sym(343).
	p(NT(240), (NT(343)));
//G526: __E_memory_139(347)  => '%'.
	p(NT(347), (T(57)));
//G527: rel_memory_sym(341)  => '%' '-'.
	p(NT(341), (T(57)+T(27)));
//G528: memory_id(349)       => digits(144).
	p(NT(349), (NT(144)));
//G529: __E___E_memory_139_140(348) => memory_id(349).
	p(NT(348), (NT(349)));
//G530: __E___E_memory_139_140(348) => null.
	p(NT(348), (nul));
//G531: __E_memory_139(347)  => rel_memory_sym(341) __E___E_memory_139_140(348).
	p(NT(347), (NT(341)+NT(348)));
//G532: rel_memory(346)      => __E_memory_139(347).
	p(NT(346), (NT(347)));
//G533: rel_memory(346)      => rel_memory(346).
	p(NT(346), (NT(346)));
//G534: memory(335)          => rel_memory(346).
	p(NT(335), (NT(346)));
//G535: abs_memory_sym(340)  => '%'.
	p(NT(340), (T(57)));
//G536: __E_memory_141(351)  => abs_memory_sym(340) memory_id(349).
	p(NT(351), (NT(340)+NT(349)));
//G537: abs_memory(350)      => __E_memory_141(351).
	p(NT(350), (NT(351)));
//G538: memory(335)          => abs_memory(350).
	p(NT(335), (NT(350)));
//G539: option(319)          => bool_option(328).
	p(NT(319), (NT(328)));
//G540: __E_option_142(354)  => 's' 'e' 'v'.
	p(NT(354), (T(10)+T(13)+T(52)));
//G541: __E_option_142(354)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(354), (T(10)+T(13)+T(52)+T(13)+T(51)+T(15)+T(14)+T(20)));
//G542: severity_opt(353)    => __E_option_142(354).
	p(NT(353), (NT(354)));
//G543: enum_option(352)     => severity_opt(353).
	p(NT(352), (NT(353)));
//G544: option(319)          => enum_option(352).
	p(NT(319), (NT(352)));
//G545: __E_bool_option_143(356) => 's'.
	p(NT(356), (T(10)));
//G546: __E_bool_option_143(356) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(356), (T(10)+T(14)+T(17)+T(14)+T(50)+T(10)));
//G547: status_opt(355)      => __E_bool_option_143(356).
	p(NT(355), (NT(356)));
//G548: bool_option(328)     => status_opt(355).
	p(NT(328), (NT(355)));
//G549: __E_bool_option_144(358) => 'c'.
	p(NT(358), (T(46)));
//G550: __E_bool_option_144(358) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(358), (T(46)+T(11)+T(18)+T(11)+T(51)));
//G551: __E_bool_option_144(358) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(358), (T(46)+T(11)+T(18)+T(11)+T(51)+T(10)));
//G552: colors_opt(357)      => __E_bool_option_144(358).
	p(NT(357), (NT(358)));
//G553: bool_option(328)     => colors_opt(357).
	p(NT(328), (NT(357)));
//G554: __E_bool_option_145(360) => 'd'.
	p(NT(360), (T(54)));
//G555: __E_bool_option_145(360) => 'd' 'b' 'g'.
	p(NT(360), (T(54)+T(24)+T(49)));
//G556: __E_bool_option_145(360) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(360), (T(54)+T(13)+T(24)+T(50)+T(49)));
//G557: debug_repl_opt(359)  => __E_bool_option_145(360).
	p(NT(359), (NT(360)));
//G558: bool_option(328)     => debug_repl_opt(359).
	p(NT(328), (NT(359)));
//G559: option_value(324)    => option_value_true(361).
	p(NT(324), (NT(361)));
//G560: option_value(324)    => option_value_false(362).
	p(NT(324), (NT(362)));
//G561: option_value(324)    => severity(363).
	p(NT(324), (NT(363)));
//G562: option_value_true(361) => 't'.
	p(NT(361), (T(14)));
//G563: option_value_true(361) => 't' 'r' 'u' 'e'.
	p(NT(361), (T(14)+T(51)+T(50)+T(13)));
//G564: option_value_true(361) => 'o' 'n'.
	p(NT(361), (T(11)+T(47)));
//G565: option_value_true(361) => '1'.
	p(NT(361), (T(37)));
//G566: option_value_true(361) => 'y'.
	p(NT(361), (T(20)));
//G567: option_value_true(361) => 'y' 'e' 's'.
	p(NT(361), (T(20)+T(13)+T(10)));
//G568: option_value_false(362) => 'f'.
	p(NT(362), (T(44)));
//G569: option_value_false(362) => 'f' 'a' 'l' 's' 'e'.
	p(NT(362), (T(44)+T(17)+T(18)+T(10)+T(13)));
//G570: option_value_false(362) => 'o' 'f' 'f'.
	p(NT(362), (T(11)+T(44)+T(44)));
//G571: option_value_false(362) => '0'.
	p(NT(362), (T(38)));
//G572: option_value_false(362) => 'n'.
	p(NT(362), (T(47)));
//G573: option_value_false(362) => 'n' 'o'.
	p(NT(362), (T(47)+T(11)));
//G574: __E_severity_146(365) => 'e'.
	p(NT(365), (T(13)));
//G575: __E_severity_146(365) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(365), (T(13)+T(51)+T(51)+T(11)+T(51)));
//G576: error_sym(364)       => __E_severity_146(365).
	p(NT(364), (NT(365)));
//G577: severity(363)        => error_sym(364).
	p(NT(363), (NT(364)));
//G578: __E_severity_147(367) => 'i'.
	p(NT(367), (T(15)));
//G579: __E_severity_147(367) => 'i' 'n' 'f' 'o'.
	p(NT(367), (T(15)+T(47)+T(44)+T(11)));
//G580: info_sym(366)        => __E_severity_147(367).
	p(NT(366), (NT(367)));
//G581: severity(363)        => info_sym(366).
	p(NT(363), (NT(366)));
//G582: __E_severity_148(369) => 'd'.
	p(NT(369), (T(54)));
//G583: __E_severity_148(369) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(369), (T(54)+T(13)+T(24)+T(50)+T(49)));
//G584: debug_sym(368)       => __E_severity_148(369).
	p(NT(368), (NT(369)));
//G585: severity(363)        => debug_sym(368).
	p(NT(363), (NT(368)));
//G586: __E_severity_149(371) => 't'.
	p(NT(371), (T(14)));
//G587: __E_severity_149(371) => 't' 'r' 'a' 'c' 'e'.
	p(NT(371), (T(14)+T(51)+T(17)+T(46)+T(13)));
//G588: trace_sym(370)       => __E_severity_149(371).
	p(NT(370), (NT(371)));
//G589: severity(363)        => trace_sym(370).
	p(NT(363), (NT(370)));
//G590: rule(43)             => tau_rule(373).
	p(NT(43), (NT(373)));
//G591: tau_matcher(374)     => tau(375).
	p(NT(374), (NT(375)));
//G592: __E_tau_rule_150(377) => tau(375).
	p(NT(377), (NT(375)));
//G593: __E_tau_rule_150(377) => tau_cb(372).
	p(NT(377), (NT(372)));
//G594: tau_body(376)        => __E_tau_rule_150(377).
	p(NT(376), (NT(377)));
//G595: tau_rule(373)        => tau_matcher(374) _(15) ':' ':' ':' '=' _(15) tau_body(376) _(15) '.'.
	p(NT(373), (NT(374)+NT(15)+T(2)+T(2)+T(2)+T(3)+NT(15)+NT(376)+NT(15)+T(1)));
//G596: __E_builder_body_151(379) => '=' ':' ':' ':' _(15) tau(375).
	p(NT(379), (T(3)+T(2)+T(2)+T(2)+NT(15)+NT(375)));
//G597: tau_builder_body(378) => __E_builder_body_151(379).
	p(NT(378), (NT(379)));
//G598: builder_body(57)     => tau_builder_body(378).
	p(NT(57), (NT(378)));
//G599: __E_tau_152(381)     => '(' _(15) tau(375) _(15) ')'.
	p(NT(381), (T(5)+NT(15)+NT(375)+NT(15)+T(6)));
//G600: tau_parenthesis(380) => __E_tau_152(381).
	p(NT(380), (NT(381)));
//G601: tau(375)             => tau_parenthesis(380).
	p(NT(375), (NT(380)));
//G602: __E_tau_153(383)     => '{' _(15) wff(18) _(15) '}'.
	p(NT(383), (T(8)+NT(15)+NT(18)+NT(15)+T(9)));
//G603: tau_wff(382)         => __E_tau_153(383).
	p(NT(382), (NT(383)));
//G604: tau(375)             => tau_wff(382).
	p(NT(375), (NT(382)));
//G605: __E___E_tau_154_155(386) => _(15) offsets(28).
	p(NT(386), (NT(15)+NT(28)));
//G606: __E___E_tau_154_155(386) => null.
	p(NT(386), (nul));
//G607: __E_tau_154(385)     => sym(26) __E___E_tau_154_155(386) _(15) ref_args(29).
	p(NT(385), (NT(26)+NT(386)+NT(15)+NT(29)));
//G608: tau_ref(384)         => __E_tau_154(385).
	p(NT(384), (NT(385)));
//G609: tau(375)             => tau_ref(384).
	p(NT(375), (NT(384)));
//G610: __E_tau_156(388)     => tau(375) _(15) '|' '|' '|' _(15) tau(375).
	p(NT(388), (NT(375)+NT(15)+T(28)+T(28)+T(28)+NT(15)+NT(375)));
//G611: tau_or(387)          => __E_tau_156(388).
	p(NT(387), (NT(388)));
//G612: tau(375)             => tau_or(387).
	p(NT(375), (NT(387)));
//G613: __E_tau_157(390)     => tau(375) _(15) '&' '&' '&' _(15) tau(375).
	p(NT(390), (NT(375)+NT(15)+T(30)+T(30)+T(30)+NT(15)+NT(375)));
//G614: tau_and(389)         => __E_tau_157(390).
	p(NT(389), (NT(390)));
//G615: tau(375)             => tau_and(389).
	p(NT(375), (NT(389)));
//G616: __E_tau_158(392)     => '-' _(15) tau(375).
	p(NT(392), (T(27)+NT(15)+NT(375)));
//G617: tau_neg(391)         => __E_tau_158(392).
	p(NT(391), (NT(392)));
//G618: tau(375)             => tau_neg(391).
	p(NT(375), (NT(391)));
//G619: tau(375)             => capture(24).
	p(NT(375), (NT(24)));
//G620: __E_tau_cb_args1_159(395) => capture(24).
	p(NT(395), (NT(24)));
//G621: __E_tau_cb_args1_159(395) => tau(375).
	p(NT(395), (NT(375)));
//G622: tau_cb_arg(394)      => __E_tau_cb_args1_159(395).
	p(NT(394), (NT(395)));
//G623: tau_cb_args1(393)    => __(59) tau_cb_arg(394).
	p(NT(393), (NT(59)+NT(394)));
//G624: tau_cb_args2(396)    => tau_cb_args1(393) tau_cb_args1(393).
	p(NT(396), (NT(393)+NT(393)));
//G625: tau_cb_args3(397)    => tau_cb_args2(396) tau_cb_args1(393).
	p(NT(397), (NT(396)+NT(393)));
//G626: __E___E_tau_cb_160_161(400) => tau_cb_args3(397).
	p(NT(400), (NT(397)));
//G627: __E___E_tau_cb_160_161(400) => tau_cb_args2(396).
	p(NT(400), (NT(396)));
//G628: __E_tau_cb_160(399)  => 't' 'a' 'u' '_' 'c' 'o' 'l' 'l' 'a' 'p' 's' 'e' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'c' 'b' __E___E_tau_cb_160_161(400).
	p(NT(399), (T(14)+T(17)+T(50)+T(25)+T(46)+T(11)+T(18)+T(18)+T(17)+T(55)+T(10)+T(13)+T(25)+T(55)+T(11)+T(10)+T(15)+T(14)+T(15)+T(52)+T(13)+T(10)+T(25)+T(46)+T(24)+NT(400)));
//G629: tau_collapse_positives_cb(398) => __E_tau_cb_160(399).
	p(NT(398), (NT(399)));
//G630: tau_cb(372)          => tau_collapse_positives_cb(398).
	p(NT(372), (NT(398)));
//G631: __E_tau_cb_162(402)  => 't' 'a' 'u' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'u' 'p' 'w' 'a' 'r' 'd' 's' '_' 'c' 'b' tau_cb_args2(396).
	p(NT(402), (T(14)+T(17)+T(50)+T(25)+T(55)+T(11)+T(10)+T(15)+T(14)+T(15)+T(52)+T(13)+T(10)+T(25)+T(50)+T(55)+T(19)+T(17)+T(51)+T(54)+T(10)+T(25)+T(46)+T(24)+NT(396)));
//G632: tau_positives_upwards_cb(401) => __E_tau_cb_162(402).
	p(NT(401), (NT(402)));
//G633: tau_cb(372)          => tau_positives_upwards_cb(401).
	p(NT(372), (NT(401)));
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
		rules, __E_library_11, __E___E_library_11_12, rule, __E___E___E_library_11_12_13, wff_rule, bf_rule, __E___E_library_11_14, wff_matcher, wff_body, 
		__E_wff_rule_15, wff_cb, bf_matcher, bf_body, __E_bf_rule_16, bf_cb, builder_head, builder_body, __E_builder_head_17, __, 
		__E_builder_head_18, bf_builder_body, __E_builder_body_19, wff_builder_body, __E_builder_body_20, wff_parenthesis, __E_wff_21, wff_sometimes, __E_wff_22, __E___E_wff_22_23, 
		wff_always, __E_wff_24, __E___E_wff_24_25, wff_conditional, __E_wff_26, wff_ball, __E_wff_27, q_bool_var, wff_bex, __E_wff_28, 
		wff_all, __E_wff_29, q_var, wff_ex, __E_wff_30, wff_ref, wff_imply, __E_wff_31, wff_equiv, __E_wff_32, 
		wff_or, __E_wff_33, wff_xor, __E_wff_34, wff_and, __E_wff_35, wff_neg, __E_wff_36, wff_t, wff_f, 
		bf_interval, __E_wff_37, bf_neq, __E_wff_38, bf_eq, __E_wff_39, bf_nleq, __E_wff_40, bf_greater, __E_wff_41, 
		bf_less_equal, __E_wff_42, bf_less, __E_wff_43, bool_variable, uninterpreted_constant, bf_parenthesis, __E_bf_44, bf_splitter, __E_bf_45, 
		bf_ref, bf_or, __E_bf_46, bf_xor, __E_bf_47, bf_and, __E_bf_48, __E___E_bf_48_49, bf_neg, __E_bf_50, 
		bf_constant, __E_bf_51, constant, bf_t, bf_f, variable, charvar, __E_charvar_52, __E_charvar_53, io_var, 
		__E_variable_54, out, in_var_name, __E_in_55, digits, offset, out_var_name, __E_out_56, __E_offsets_57, __E_offsets_58, 
		num, shift, __E_offset_59, __E_shift_60, __E___E_shift_60_61, uninter_const_name, __E_uninterpreted_constant_62, __E___E_uninterpreted_constant_62_63, chars, binding, 
		__E_binding_64, type, __E___E_binding_64_65, source, named_binding, src_c, __E_source_66, __E_source_67, __E___E_source_67_68, __E___E_source_67_69, 
		__E_chars_70, __E_digits_71, comment, __E_comment_72, __E_comment_73, __E_comment_74, __E____75, __E___76, bf_cb_args1, bf_cb_arg, 
		bf_cb_args2, bf_cb_args3, bf_cb_args4, wff_cb_args1, wff_cb_arg, wff_cb_args2, wff_cb_args3, wff_cb_args4, bf_eq_cb, __E_wff_cb_77, 
		bf_neq_cb, __E_wff_cb_78, wff_has_clashing_subformulas_cb, __E_wff_cb_79, wff_has_subformula_cb, __E_wff_cb_80, wff_remove_existential_cb, __E_wff_cb_81, wff_remove_bexistential_cb, __E_wff_cb_82, 
		wff_remove_buniversal_cb, __E_wff_cb_83, bf_is_zero_cb, __E_bf_cb_84, bf_is_one_cb, __E_bf_cb_85, bf_has_subformula_cb, __E_bf_cb_86, bf_remove_funiversal_cb, __E_bf_cb_87, 
		bf_remove_fexistential_cb, __E_bf_cb_88, bf_and_cb, __E_bf_cb_89, bf_or_cb, __E_bf_cb_90, bf_xor_cb, __E_bf_cb_91, bf_neg_cb, __E_bf_cb_92, 
		bf_normalize_cb, __E_bf_cb_93, cli, cli_command, __E_cli_94, __E_cli_95, quit_cmd, quit_sym, __E_cli_command_96, version_cmd, 
		version_sym, __E_cli_command_97, clear_cmd, clear_sym, __E_cli_command_98, help_cmd, __E_cli_command_99, help_sym, __E___E_cli_command_99_100, __E___E_cli_command_99_101, 
		help_arg, file_cmd, __E_cli_command_102, file_sym, q_string, valid_cmd, __E_cli_command_103, valid_sym, sat_cmd, __E_cli_command_104, 
		sat_sym, unsat_cmd, __E_cli_command_105, unsat_sym, solve_cmd, __E_cli_command_106, solve_sym, wff_cmd_arg, execute_cmd, __E_cli_command_107, 
		execute_sym, __E___E_cli_command_107_108, __E___E_cli_command_107_109, wff_typed, normalize_cmd, __E_cli_command_110, normalize_sym, __E___E_cli_command_110_111, normalize_cmd_arg, subst_cmd, 
		__E_cli_command_112, subst_sym, __E___E_cli_command_112_113, nf_cmd_arg, inst_cmd, __E_cli_command_114, inst_sym, __E___E_cli_command_114_115, inst_args, dnf_cmd, 
		__E_cli_command_116, dnf_sym, cnf_cmd, __E_cli_command_117, cnf_sym, anf_cmd, __E_cli_command_118, anf_sym, nnf_cmd, __E_cli_command_119, 
		nnf_sym, pnf_cmd, __E_cli_command_120, pnf_sym, mnf_cmd, __E_cli_command_121, mnf_sym, snf_cmd, __E_cli_command_122, snf_sym, 
		onf_cmd, __E_cli_command_123, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_124, def_sym, __E___E_cli_command_124_125, def_print_cmd, __E_cli_command_126, 
		number, def_rr_cmd, qelim_cmd, __E_cli_command_127, qelim_sym, get_cmd, __E_cli_command_128, get_sym, __E___E_cli_command_128_129, option, 
		set_cmd, __E_cli_command_130, set_sym, __E___E_cli_command_130_131, option_value, toggle_cmd, __E_cli_command_132, toggle_sym, bool_option, history_list_cmd, 
		__E_cli_command_133, history_sym, __E___E_cli_command_133_134, history_print_cmd, __E_cli_command_135, memory, history_store_cmd, __E_cli_command_136, bf_typed, bf_cmd_arg, 
		abs_memory_sym, rel_memory_sym, selection_sym, examples_sym, __E_help_arg_137, __E___E_help_arg_137_138, rel_memory, __E_memory_139, __E___E_memory_139_140, memory_id, 
		abs_memory, __E_memory_141, enum_option, severity_opt, __E_option_142, status_opt, __E_bool_option_143, colors_opt, __E_bool_option_144, debug_repl_opt, 
		__E_bool_option_145, option_value_true, option_value_false, severity, error_sym, __E_severity_146, info_sym, __E_severity_147, debug_sym, __E_severity_148, 
		trace_sym, __E_severity_149, tau_cb, tau_rule, tau_matcher, tau, tau_body, __E_tau_rule_150, tau_builder_body, __E_builder_body_151, 
		tau_parenthesis, __E_tau_152, tau_wff, __E_tau_153, tau_ref, __E_tau_154, __E___E_tau_154_155, tau_or, __E_tau_156, tau_and, 
		__E_tau_157, tau_neg, __E_tau_158, tau_cb_args1, tau_cb_arg, __E_tau_cb_args1_159, tau_cb_args2, tau_cb_args3, tau_collapse_positives_cb, __E_tau_cb_160, 
		__E___E_tau_cb_160_161, tau_positives_upwards_cb, __E_tau_cb_162, __N_0, __N_1, __N_2, __N_3, __N_4, __N_5, 
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
