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
//G169: __N_0(403)           => 'F'.
	p(NT(403), (T(33)));
//G170: __N_1(404)           => 'T'.
	p(NT(404), (T(32)));
//G171: __N_2(405)           => 'i'.
	p(NT(405), (T(15)));
//G172: __N_3(406)           => 'o'.
	p(NT(406), (T(11)));
//G173: __E_charvar_52(137)  => ~( __N_0(403) ) & ~( __N_1(404) ) & ~( __N_2(405) ) & ~( __N_3(406) ) & alpha(5).	 # conjunctive
	p(NT(137), ~(NT(403)) & ~(NT(404)) & ~(NT(405)) & ~(NT(406)) & (NT(5)));
//G174: __E_charvar_53(138)  => null.
	p(NT(138), (nul));
//G175: __E_charvar_53(138)  => digit(3) __E_charvar_53(138).
	p(NT(138), (NT(3)+NT(138)));
//G176: charvar(136)         => __E_charvar_52(137) __E_charvar_53(138).
	p(NT(136), (NT(137)+NT(138)));
//G177: bool_variable(114)   => '?' charvar(136).
	p(NT(114), (T(23)+NT(136)));
//G178: variable(135)        => charvar(136).
	p(NT(135), (NT(136)));
//G179: __E_variable_54(140) => in(38).
	p(NT(140), (NT(38)));
//G180: __E_variable_54(140) => out(141).
	p(NT(140), (NT(141)));
//G181: io_var(139)          => __E_variable_54(140).
	p(NT(139), (NT(140)));
//G182: variable(135)        => io_var(139).
	p(NT(135), (NT(139)));
//G183: __E_in_55(143)       => 'i' digits(144).
	p(NT(143), (T(15)+NT(144)));
//G184: in_var_name(142)     => __E_in_55(143).
	p(NT(142), (NT(143)));
//G185: in(38)               => in_var_name(142) '[' offset(145) ']'.
	p(NT(38), (NT(142)+T(21)+NT(145)+T(22)));
//G186: __E_out_56(147)      => 'o' digits(144).
	p(NT(147), (T(11)+NT(144)));
//G187: out_var_name(146)    => __E_out_56(147).
	p(NT(146), (NT(147)));
//G188: out(141)             => out_var_name(146) '[' offset(145) ']'.
	p(NT(141), (NT(146)+T(21)+NT(145)+T(22)));
//G189: capture(24)          => '$' charvar(136).
	p(NT(24), (T(39)+NT(136)));
//G190: q_bool_var(77)       => capture(24).
	p(NT(77), (NT(24)));
//G191: q_bool_var(77)       => bool_variable(114).
	p(NT(77), (NT(114)));
//G192: q_var(82)            => capture(24).
	p(NT(82), (NT(24)));
//G193: q_var(82)            => variable(135).
	p(NT(82), (NT(135)));
//G194: __E_offsets_57(148)  => _(15) ',' _(15) offset(145).
	p(NT(148), (NT(15)+T(4)+NT(15)+NT(145)));
//G195: __E_offsets_58(149)  => null.
	p(NT(149), (nul));
//G196: __E_offsets_58(149)  => __E_offsets_57(148) __E_offsets_58(149).
	p(NT(149), (NT(148)+NT(149)));
//G197: offsets(28)          => '[' _(15) offset(145) __E_offsets_58(149) _(15) ']'.
	p(NT(28), (T(21)+NT(15)+NT(145)+NT(149)+NT(15)+T(22)));
//G198: offset(145)          => num(150).
	p(NT(145), (NT(150)));
//G199: offset(145)          => capture(24).
	p(NT(145), (NT(24)));
//G200: offset(145)          => shift(151).
	p(NT(145), (NT(151)));
//G201: __N_4(407)           => io_var(139).
	p(NT(407), (NT(139)));
//G202: __E_offset_59(152)   => variable(135) & ~( __N_4(407) ).	 # conjunctive
	p(NT(152), (NT(135)) & ~(NT(407)));
//G203: offset(145)          => __E_offset_59(152).
	p(NT(145), (NT(152)));
//G204: __E_shift_60(153)    => capture(24).
	p(NT(153), (NT(24)));
//G205: __N_5(408)           => io_var(139).
	p(NT(408), (NT(139)));
//G206: __E___E_shift_60_61(154) => variable(135) & ~( __N_5(408) ).	 # conjunctive
	p(NT(154), (NT(135)) & ~(NT(408)));
//G207: __E_shift_60(153)    => __E___E_shift_60_61(154).
	p(NT(153), (NT(154)));
//G208: shift(151)           => __E_shift_60(153) _(15) '-' _(15) num(150).
	p(NT(151), (NT(153)+NT(15)+T(27)+NT(15)+NT(150)));
//G209: num(150)             => digits(144).
	p(NT(150), (NT(144)));
//G210: __E___E_uninterpreted_constant_62_63(157) => chars(158).
	p(NT(157), (NT(158)));
//G211: __E___E_uninterpreted_constant_62_63(157) => _(15).
	p(NT(157), (NT(15)));
//G212: __E_uninterpreted_constant_62(156) => __E___E_uninterpreted_constant_62_63(157) ':' chars(158).
	p(NT(156), (NT(157)+T(2)+NT(158)));
//G213: uninter_const_name(155) => __E_uninterpreted_constant_62(156).
	p(NT(155), (NT(156)));
//G214: uninterpreted_constant(115) => '<' uninter_const_name(155) _(15) '>'.
	p(NT(115), (T(7)+NT(155)+NT(15)+T(16)));
//G215: constant(132)        => binding(159).
	p(NT(132), (NT(159)));
//G216: constant(132)        => capture(24).
	p(NT(132), (NT(24)));
//G217: __E___E_binding_64_65(162) => chars(158) _(15).
	p(NT(162), (NT(158)+NT(15)));
//G218: __E___E_binding_64_65(162) => _(15).
	p(NT(162), (NT(15)));
//G219: type(161)            => __E___E_binding_64_65(162).
	p(NT(161), (NT(162)));
//G220: __E_binding_64(160)  => type(161) ':' _(15) source(163).
	p(NT(160), (NT(161)+T(2)+NT(15)+NT(163)));
//G221: source_binding(39)   => __E_binding_64(160).
	p(NT(39), (NT(160)));
//G222: binding(159)         => source_binding(39).
	p(NT(159), (NT(39)));
//G223: named_binding(164)   => chars(158).
	p(NT(164), (NT(158)));
//G224: binding(159)         => named_binding(164).
	p(NT(159), (NT(164)));
//G225: __E_source_66(166)   => alnum(6).
	p(NT(166), (NT(6)));
//G226: __E_source_66(166)   => punct(7).
	p(NT(166), (NT(7)));
//G227: src_c(165)           => __E_source_66(166).
	p(NT(165), (NT(166)));
//G228: __E___E_source_67_68(168) => src_c(165).
	p(NT(168), (NT(165)));
//G229: __E___E_source_67_68(168) => space(2).
	p(NT(168), (NT(2)));
//G230: __E___E_source_67_69(169) => null.
	p(NT(169), (nul));
//G231: __E___E_source_67_69(169) => __E___E_source_67_68(168) __E___E_source_67_69(169).
	p(NT(169), (NT(168)+NT(169)));
//G232: __E_source_67(167)   => __E___E_source_67_69(169) src_c(165).
	p(NT(167), (NT(169)+NT(165)));
//G233: __E_source_67(167)   => null.
	p(NT(167), (nul));
//G234: source(163)          => src_c(165) __E_source_67(167).
	p(NT(163), (NT(165)+NT(167)));
//G235: __E_chars_70(170)    => null.
	p(NT(170), (nul));
//G236: __E_chars_70(170)    => alnum(6) __E_chars_70(170).
	p(NT(170), (NT(6)+NT(170)));
//G237: chars(158)           => alpha(5) __E_chars_70(170).
	p(NT(158), (NT(5)+NT(170)));
//G238: __E_digits_71(171)   => digit(3).
	p(NT(171), (NT(3)));
//G239: __E_digits_71(171)   => digit(3) __E_digits_71(171).
	p(NT(171), (NT(3)+NT(171)));
//G240: digits(144)          => __E_digits_71(171).
	p(NT(144), (NT(171)));
//G241: sym(26)              => chars(158).
	p(NT(26), (NT(158)));
//G242: __E_comment_72(173)  => printable(8).
	p(NT(173), (NT(8)));
//G243: __E_comment_72(173)  => '\t'.
	p(NT(173), (T(40)));
//G244: __E_comment_73(174)  => null.
	p(NT(174), (nul));
//G245: __E_comment_73(174)  => __E_comment_72(173) __E_comment_73(174).
	p(NT(174), (NT(173)+NT(174)));
//G246: __E_comment_74(175)  => '\n'.
	p(NT(175), (T(41)));
//G247: __E_comment_74(175)  => '\r'.
	p(NT(175), (T(42)));
//G248: __E_comment_74(175)  => eof(1).
	p(NT(175), (NT(1)));
//G249: comment(172)         => '#' __E_comment_73(174) __E_comment_74(175).
	p(NT(172), (T(43)+NT(174)+NT(175)));
//G250: __E____75(176)       => space(2).
	p(NT(176), (NT(2)));
//G251: __E____75(176)       => comment(172).
	p(NT(176), (NT(172)));
//G252: __(59)               => __E____75(176) _(15).
	p(NT(59), (NT(176)+NT(15)));
//G253: __E___76(177)        => __(59).
	p(NT(177), (NT(59)));
//G254: __E___76(177)        => null.
	p(NT(177), (nul));
//G255: _(15)                => __E___76(177).
	p(NT(15), (NT(177)));
//G256: bf_cb_arg(179)       => bf(25).
	p(NT(179), (NT(25)));
//G257: bf_cb_args1(178)     => __(59) bf_cb_arg(179).
	p(NT(178), (NT(59)+NT(179)));
//G258: bf_cb_args2(180)     => bf_cb_args1(178) bf_cb_args1(178).
	p(NT(180), (NT(178)+NT(178)));
//G259: bf_cb_args3(181)     => bf_cb_args2(180) bf_cb_args1(178).
	p(NT(181), (NT(180)+NT(178)));
//G260: bf_cb_args4(182)     => bf_cb_args3(181) bf_cb_args1(178).
	p(NT(182), (NT(181)+NT(178)));
//G261: wff_cb_arg(184)      => wff(18).
	p(NT(184), (NT(18)));
//G262: wff_cb_args1(183)    => __(59) wff_cb_arg(184).
	p(NT(183), (NT(59)+NT(184)));
//G263: wff_cb_args2(185)    => wff_cb_args1(183) wff_cb_args1(183).
	p(NT(185), (NT(183)+NT(183)));
//G264: wff_cb_args3(186)    => wff_cb_args2(185) wff_cb_args1(183).
	p(NT(186), (NT(185)+NT(183)));
//G265: wff_cb_args4(187)    => wff_cb_args3(186) wff_cb_args1(183).
	p(NT(187), (NT(186)+NT(183)));
//G266: __E_wff_cb_77(189)   => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b' bf_cb_args1(178) wff_cb_args2(185).
	p(NT(189), (T(24)+T(44)+T(25)+T(13)+T(45)+T(25)+T(46)+T(24)+NT(178)+NT(185)));
//G267: bf_eq_cb(188)        => __E_wff_cb_77(189).
	p(NT(188), (NT(189)));
//G268: wff_cb(51)           => bf_eq_cb(188).
	p(NT(51), (NT(188)));
//G269: __E_wff_cb_78(191)   => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b' bf_cb_args1(178) wff_cb_args2(185).
	p(NT(191), (T(24)+T(44)+T(25)+T(47)+T(13)+T(45)+T(25)+T(46)+T(24)+NT(178)+NT(185)));
//G270: bf_neq_cb(190)       => __E_wff_cb_78(191).
	p(NT(190), (NT(191)));
//G271: wff_cb(51)           => bf_neq_cb(190).
	p(NT(51), (NT(190)));
//G272: __E_wff_cb_79(193)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(185).
	p(NT(193), (T(19)+T(44)+T(44)+T(25)+T(48)+T(17)+T(10)+T(25)+T(46)+T(18)+T(17)+T(10)+T(48)+T(15)+T(47)+T(49)+T(25)+T(10)+T(50)+T(24)+T(44)+T(11)+T(51)+T(12)+T(50)+T(18)+T(17)+T(10)+T(25)+T(46)+T(24)+NT(185)));
//G273: wff_has_clashing_subformulas_cb(192) => __E_wff_cb_79(193).
	p(NT(192), (NT(193)));
//G274: wff_cb(51)           => wff_has_clashing_subformulas_cb(192).
	p(NT(51), (NT(192)));
//G275: __E_wff_cb_80(195)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(186).
	p(NT(195), (T(19)+T(44)+T(44)+T(25)+T(48)+T(17)+T(10)+T(25)+T(10)+T(50)+T(24)+T(44)+T(11)+T(51)+T(12)+T(50)+T(18)+T(17)+T(25)+T(46)+T(24)+NT(186)));
//G276: wff_has_subformula_cb(194) => __E_wff_cb_80(195).
	p(NT(194), (NT(195)));
//G277: wff_cb(51)           => wff_has_subformula_cb(194).
	p(NT(51), (NT(194)));
//G278: __E_wff_cb_81(197)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(185).
	p(NT(197), (T(19)+T(44)+T(44)+T(25)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(25)+T(13)+T(26)+T(15)+T(10)+T(14)+T(13)+T(47)+T(14)+T(15)+T(17)+T(18)+T(25)+T(46)+T(24)+NT(185)));
//G279: wff_remove_existential_cb(196) => __E_wff_cb_81(197).
	p(NT(196), (NT(197)));
//G280: wff_cb(51)           => wff_remove_existential_cb(196).
	p(NT(51), (NT(196)));
//G281: __E_wff_cb_82(199)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args4(187).
	p(NT(199), (T(19)+T(44)+T(44)+T(25)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(25)+T(24)+T(13)+T(26)+T(15)+T(10)+T(14)+T(13)+T(47)+T(14)+T(15)+T(17)+T(18)+T(25)+T(46)+T(24)+NT(187)));
//G282: wff_remove_bexistential_cb(198) => __E_wff_cb_82(199).
	p(NT(198), (NT(199)));
//G283: wff_cb(51)           => wff_remove_bexistential_cb(198).
	p(NT(51), (NT(198)));
//G284: __E_wff_cb_83(201)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' wff_cb_args4(187).
	p(NT(201), (T(19)+T(44)+T(44)+T(25)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(25)+T(24)+T(50)+T(47)+T(15)+T(52)+T(13)+T(51)+T(10)+T(17)+T(18)+T(25)+T(46)+T(24)+NT(187)));
//G285: wff_remove_buniversal_cb(200) => __E_wff_cb_83(201).
	p(NT(200), (NT(201)));
//G286: wff_cb(51)           => wff_remove_buniversal_cb(200).
	p(NT(51), (NT(200)));
//G287: __E_bf_cb_84(203)    => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b' bf_cb_args2(180).
	p(NT(203), (T(24)+T(44)+T(25)+T(15)+T(10)+T(25)+T(53)+T(13)+T(51)+T(11)+T(25)+T(46)+T(24)+NT(180)));
//G288: bf_is_zero_cb(202)   => __E_bf_cb_84(203).
	p(NT(202), (NT(203)));
//G289: bf_cb(55)            => bf_is_zero_cb(202).
	p(NT(55), (NT(202)));
//G290: __E_bf_cb_85(205)    => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b' bf_cb_args2(180).
	p(NT(205), (T(24)+T(44)+T(25)+T(15)+T(10)+T(25)+T(11)+T(47)+T(13)+T(25)+T(46)+T(24)+NT(180)));
//G291: bf_is_one_cb(204)    => __E_bf_cb_85(205).
	p(NT(204), (NT(205)));
//G292: bf_cb(55)            => bf_is_one_cb(204).
	p(NT(55), (NT(204)));
//G293: __E_bf_cb_86(207)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(181).
	p(NT(207), (T(24)+T(44)+T(25)+T(48)+T(17)+T(10)+T(25)+T(10)+T(50)+T(24)+T(44)+T(11)+T(51)+T(12)+T(50)+T(18)+T(17)+T(25)+T(46)+T(24)+NT(181)));
//G294: bf_has_subformula_cb(206) => __E_bf_cb_86(207).
	p(NT(206), (NT(207)));
//G295: bf_cb(55)            => bf_has_subformula_cb(206).
	p(NT(55), (NT(206)));
//G296: __E_bf_cb_87(209)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(182).
	p(NT(209), (T(24)+T(44)+T(25)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(25)+T(44)+T(50)+T(47)+T(15)+T(52)+T(13)+T(51)+T(10)+T(17)+T(18)+T(25)+T(46)+T(24)+NT(182)));
//G297: bf_remove_funiversal_cb(208) => __E_bf_cb_87(209).
	p(NT(208), (NT(209)));
//G298: bf_cb(55)            => bf_remove_funiversal_cb(208).
	p(NT(55), (NT(208)));
//G299: __E_bf_cb_88(211)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(182).
	p(NT(211), (T(24)+T(44)+T(25)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(25)+T(44)+T(13)+T(26)+T(15)+T(10)+T(14)+T(13)+T(47)+T(14)+T(15)+T(17)+T(18)+T(25)+T(46)+T(24)+NT(182)));
//G300: bf_remove_fexistential_cb(210) => __E_bf_cb_88(211).
	p(NT(210), (NT(211)));
//G301: bf_cb(55)            => bf_remove_fexistential_cb(210).
	p(NT(55), (NT(210)));
//G302: __E_bf_cb_89(213)    => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b' bf_cb_args2(180).
	p(NT(213), (T(24)+T(44)+T(25)+T(17)+T(47)+T(54)+T(25)+T(46)+T(24)+NT(180)));
//G303: bf_and_cb(212)       => __E_bf_cb_89(213).
	p(NT(212), (NT(213)));
//G304: bf_cb(55)            => bf_and_cb(212).
	p(NT(55), (NT(212)));
//G305: __E_bf_cb_90(215)    => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b' bf_cb_args2(180).
	p(NT(215), (T(24)+T(44)+T(25)+T(11)+T(51)+T(25)+T(46)+T(24)+NT(180)));
//G306: bf_or_cb(214)        => __E_bf_cb_90(215).
	p(NT(214), (NT(215)));
//G307: bf_cb(55)            => bf_or_cb(214).
	p(NT(55), (NT(214)));
//G308: __E_bf_cb_91(217)    => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b' bf_cb_args2(180).
	p(NT(217), (T(24)+T(44)+T(25)+T(26)+T(11)+T(51)+T(25)+T(46)+T(24)+NT(180)));
//G309: bf_xor_cb(216)       => __E_bf_cb_91(217).
	p(NT(216), (NT(217)));
//G310: bf_cb(55)            => bf_xor_cb(216).
	p(NT(55), (NT(216)));
//G311: __E_bf_cb_92(219)    => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b' bf_cb_args1(178).
	p(NT(219), (T(24)+T(44)+T(25)+T(47)+T(13)+T(49)+T(25)+T(46)+T(24)+NT(178)));
//G312: bf_neg_cb(218)       => __E_bf_cb_92(219).
	p(NT(218), (NT(219)));
//G313: bf_cb(55)            => bf_neg_cb(218).
	p(NT(55), (NT(218)));
//G314: __E_bf_cb_93(221)    => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(178).
	p(NT(221), (T(24)+T(44)+T(25)+T(47)+T(11)+T(51)+T(12)+T(17)+T(18)+T(15)+T(53)+T(13)+T(25)+T(46)+T(24)+NT(178)));
//G315: bf_normalize_cb(220) => __E_bf_cb_93(221).
	p(NT(220), (NT(221)));
//G316: bf_cb(55)            => bf_normalize_cb(220).
	p(NT(55), (NT(220)));
//G317: cli(222)             => _(15).
	p(NT(222), (NT(15)));
//G318: __E_cli_94(224)      => '.' _(15) cli_command(223) _(15).
	p(NT(224), (T(1)+NT(15)+NT(223)+NT(15)));
//G319: __E_cli_95(225)      => null.
	p(NT(225), (nul));
//G320: __E_cli_95(225)      => __E_cli_94(224) __E_cli_95(225).
	p(NT(225), (NT(224)+NT(225)));
//G321: cli(222)             => _(15) cli_command(223) _(15) __E_cli_95(225).
	p(NT(222), (NT(15)+NT(223)+NT(15)+NT(225)));
//G322: __E_cli_command_96(228) => 'q'.
	p(NT(228), (T(45)));
//G323: __E_cli_command_96(228) => 'q' 'u' 'i' 't'.
	p(NT(228), (T(45)+T(50)+T(15)+T(14)));
//G324: quit_sym(227)        => __E_cli_command_96(228).
	p(NT(227), (NT(228)));
//G325: quit_cmd(226)        => quit_sym(227).
	p(NT(226), (NT(227)));
//G326: cli_command(223)     => quit_cmd(226).
	p(NT(223), (NT(226)));
//G327: __E_cli_command_97(231) => 'v'.
	p(NT(231), (T(52)));
//G328: __E_cli_command_97(231) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(231), (T(52)+T(13)+T(51)+T(10)+T(15)+T(11)+T(47)));
//G329: version_sym(230)     => __E_cli_command_97(231).
	p(NT(230), (NT(231)));
//G330: version_cmd(229)     => version_sym(230).
	p(NT(229), (NT(230)));
//G331: cli_command(223)     => version_cmd(229).
	p(NT(223), (NT(229)));
//G332: __E_cli_command_98(234) => 'c'.
	p(NT(234), (T(46)));
//G333: __E_cli_command_98(234) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(234), (T(46)+T(18)+T(13)+T(17)+T(51)));
//G334: clear_sym(233)       => __E_cli_command_98(234).
	p(NT(233), (NT(234)));
//G335: clear_cmd(232)       => clear_sym(233).
	p(NT(232), (NT(233)));
//G336: cli_command(223)     => clear_cmd(232).
	p(NT(223), (NT(232)));
//G337: __E___E_cli_command_99_100(238) => 'h'.
	p(NT(238), (T(48)));
//G338: __E___E_cli_command_99_100(238) => 'h' 'e' 'l' 'p'.
	p(NT(238), (T(48)+T(13)+T(18)+T(55)));
//G339: help_sym(237)        => __E___E_cli_command_99_100(238).
	p(NT(237), (NT(238)));
//G340: __E___E_cli_command_99_101(239) => __(59) help_arg(240).
	p(NT(239), (NT(59)+NT(240)));
//G341: __E___E_cli_command_99_101(239) => null.
	p(NT(239), (nul));
//G342: __E_cli_command_99(236) => help_sym(237) __E___E_cli_command_99_101(239).
	p(NT(236), (NT(237)+NT(239)));
//G343: help_cmd(235)        => __E_cli_command_99(236).
	p(NT(235), (NT(236)));
//G344: cli_command(223)     => help_cmd(235).
	p(NT(223), (NT(235)));
//G345: file_sym(243)        => 'f' 'i' 'l' 'e'.
	p(NT(243), (T(44)+T(15)+T(18)+T(13)));
//G346: __E_cli_command_102(242) => file_sym(243) __(59) q_string(244).
	p(NT(242), (NT(243)+NT(59)+NT(244)));
//G347: file_cmd(241)        => __E_cli_command_102(242).
	p(NT(241), (NT(242)));
//G348: cli_command(223)     => file_cmd(241).
	p(NT(223), (NT(241)));
//G349: valid_sym(247)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(247), (T(52)+T(17)+T(18)+T(15)+T(54)));
//G350: __E_cli_command_103(246) => valid_sym(247) __(59) rr(14).
	p(NT(246), (NT(247)+NT(59)+NT(14)));
//G351: valid_cmd(245)       => __E_cli_command_103(246).
	p(NT(245), (NT(246)));
//G352: cli_command(223)     => valid_cmd(245).
	p(NT(223), (NT(245)));
//G353: sat_sym(250)         => 's' 'a' 't'.
	p(NT(250), (T(10)+T(17)+T(14)));
//G354: __E_cli_command_104(249) => sat_sym(250) __(59) rr(14).
	p(NT(249), (NT(250)+NT(59)+NT(14)));
//G355: sat_cmd(248)         => __E_cli_command_104(249).
	p(NT(248), (NT(249)));
//G356: cli_command(223)     => sat_cmd(248).
	p(NT(223), (NT(248)));
//G357: unsat_sym(253)       => 'u' 'n' 's' 'a' 't'.
	p(NT(253), (T(50)+T(47)+T(10)+T(17)+T(14)));
//G358: __E_cli_command_105(252) => unsat_sym(253) __(59) rr(14).
	p(NT(252), (NT(253)+NT(59)+NT(14)));
//G359: unsat_cmd(251)       => __E_cli_command_105(252).
	p(NT(251), (NT(252)));
//G360: cli_command(223)     => unsat_cmd(251).
	p(NT(223), (NT(251)));
//G361: solve_sym(256)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(256), (T(10)+T(11)+T(18)+T(52)+T(13)));
//G362: __E_cli_command_106(255) => solve_sym(256) __(59) wff_cmd_arg(257).
	p(NT(255), (NT(256)+NT(59)+NT(257)));
//G363: solve_cmd(254)       => __E_cli_command_106(255).
	p(NT(254), (NT(255)));
//G364: cli_command(223)     => solve_cmd(254).
	p(NT(223), (NT(254)));
//G365: __E___E_cli_command_107_108(261) => 'e'.
	p(NT(261), (T(13)));
//G366: __E___E_cli_command_107_108(261) => 'e' 'x' 'e' 'c' 'u' 't' 'e'.
	p(NT(261), (T(13)+T(26)+T(13)+T(46)+T(50)+T(14)+T(13)));
//G367: execute_sym(260)     => __E___E_cli_command_107_108(261).
	p(NT(260), (NT(261)));
//G368: __E___E_cli_command_107_109(262) => wff_typed(263).
	p(NT(262), (NT(263)));
//G369: __E___E_cli_command_107_109(262) => rr(14).
	p(NT(262), (NT(14)));
//G370: __E_cli_command_107(259) => execute_sym(260) __(59) __E___E_cli_command_107_109(262).
	p(NT(259), (NT(260)+NT(59)+NT(262)));
//G371: execute_cmd(258)     => __E_cli_command_107(259).
	p(NT(258), (NT(259)));
//G372: cli_command(223)     => execute_cmd(258).
	p(NT(223), (NT(258)));
//G373: __E___E_cli_command_110_111(267) => 'n'.
	p(NT(267), (T(47)));
//G374: __E___E_cli_command_110_111(267) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(267), (T(47)+T(11)+T(51)+T(12)+T(17)+T(18)+T(15)+T(53)+T(13)));
//G375: normalize_sym(266)   => __E___E_cli_command_110_111(267).
	p(NT(266), (NT(267)));
//G376: __E_cli_command_110(265) => normalize_sym(266) __(59) normalize_cmd_arg(268).
	p(NT(265), (NT(266)+NT(59)+NT(268)));
//G377: normalize_cmd(264)   => __E_cli_command_110(265).
	p(NT(264), (NT(265)));
//G378: cli_command(223)     => normalize_cmd(264).
	p(NT(223), (NT(264)));
//G379: __E___E_cli_command_112_113(272) => 's'.
	p(NT(272), (T(10)));
//G380: __E___E_cli_command_112_113(272) => 's' 'u' 'b' 's' 't'.
	p(NT(272), (T(10)+T(50)+T(24)+T(10)+T(14)));
//G381: __E___E_cli_command_112_113(272) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(272), (T(10)+T(50)+T(24)+T(10)+T(14)+T(15)+T(14)+T(50)+T(14)+T(13)));
//G382: subst_sym(271)       => __E___E_cli_command_112_113(272).
	p(NT(271), (NT(272)));
//G383: __E_cli_command_112(270) => subst_sym(271) __(59) nf_cmd_arg(273) _(15) '[' _(15) nf_cmd_arg(273) _(15) '/' _(15) nf_cmd_arg(273) _(15) ']'.
	p(NT(270), (NT(271)+NT(59)+NT(273)+NT(15)+T(21)+NT(15)+NT(273)+NT(15)+T(56)+NT(15)+NT(273)+NT(15)+T(22)));
//G384: subst_cmd(269)       => __E_cli_command_112(270).
	p(NT(269), (NT(270)));
//G385: cli_command(223)     => subst_cmd(269).
	p(NT(223), (NT(269)));
//G386: __E___E_cli_command_114_115(277) => 'i'.
	p(NT(277), (T(15)));
//G387: __E___E_cli_command_114_115(277) => 'i' 'n' 's' 't'.
	p(NT(277), (T(15)+T(47)+T(10)+T(14)));
//G388: __E___E_cli_command_114_115(277) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(277), (T(15)+T(47)+T(10)+T(14)+T(17)+T(47)+T(14)+T(15)+T(17)+T(14)+T(13)));
//G389: inst_sym(276)        => __E___E_cli_command_114_115(277).
	p(NT(276), (NT(277)));
//G390: __E_cli_command_114(275) => inst_sym(276) __(59) inst_args(278).
	p(NT(275), (NT(276)+NT(59)+NT(278)));
//G391: inst_cmd(274)        => __E_cli_command_114(275).
	p(NT(274), (NT(275)));
//G392: cli_command(223)     => inst_cmd(274).
	p(NT(223), (NT(274)));
//G393: dnf_sym(281)         => 'd' 'n' 'f'.
	p(NT(281), (T(54)+T(47)+T(44)));
//G394: __E_cli_command_116(280) => dnf_sym(281) __(59) nf_cmd_arg(273).
	p(NT(280), (NT(281)+NT(59)+NT(273)));
//G395: dnf_cmd(279)         => __E_cli_command_116(280).
	p(NT(279), (NT(280)));
//G396: cli_command(223)     => dnf_cmd(279).
	p(NT(223), (NT(279)));
//G397: cnf_sym(284)         => 'c' 'n' 'f'.
	p(NT(284), (T(46)+T(47)+T(44)));
//G398: __E_cli_command_117(283) => cnf_sym(284) __(59) nf_cmd_arg(273).
	p(NT(283), (NT(284)+NT(59)+NT(273)));
//G399: cnf_cmd(282)         => __E_cli_command_117(283).
	p(NT(282), (NT(283)));
//G400: cli_command(223)     => cnf_cmd(282).
	p(NT(223), (NT(282)));
//G401: anf_sym(287)         => 'a' 'n' 'f'.
	p(NT(287), (T(17)+T(47)+T(44)));
//G402: __E_cli_command_118(286) => anf_sym(287) __(59) nf_cmd_arg(273).
	p(NT(286), (NT(287)+NT(59)+NT(273)));
//G403: anf_cmd(285)         => __E_cli_command_118(286).
	p(NT(285), (NT(286)));
//G404: cli_command(223)     => anf_cmd(285).
	p(NT(223), (NT(285)));
//G405: nnf_sym(290)         => 'n' 'n' 'f'.
	p(NT(290), (T(47)+T(47)+T(44)));
//G406: __E_cli_command_119(289) => nnf_sym(290) __(59) nf_cmd_arg(273).
	p(NT(289), (NT(290)+NT(59)+NT(273)));
//G407: nnf_cmd(288)         => __E_cli_command_119(289).
	p(NT(288), (NT(289)));
//G408: cli_command(223)     => nnf_cmd(288).
	p(NT(223), (NT(288)));
//G409: pnf_sym(293)         => 'p' 'n' 'f'.
	p(NT(293), (T(55)+T(47)+T(44)));
//G410: __E_cli_command_120(292) => pnf_sym(293) __(59) nf_cmd_arg(273).
	p(NT(292), (NT(293)+NT(59)+NT(273)));
//G411: pnf_cmd(291)         => __E_cli_command_120(292).
	p(NT(291), (NT(292)));
//G412: cli_command(223)     => pnf_cmd(291).
	p(NT(223), (NT(291)));
//G413: mnf_sym(296)         => 'm' 'n' 'f'.
	p(NT(296), (T(12)+T(47)+T(44)));
//G414: __E_cli_command_121(295) => mnf_sym(296) __(59) nf_cmd_arg(273).
	p(NT(295), (NT(296)+NT(59)+NT(273)));
//G415: mnf_cmd(294)         => __E_cli_command_121(295).
	p(NT(294), (NT(295)));
//G416: cli_command(223)     => mnf_cmd(294).
	p(NT(223), (NT(294)));
//G417: snf_sym(299)         => 's' 'n' 'f'.
	p(NT(299), (T(10)+T(47)+T(44)));
//G418: __E_cli_command_122(298) => snf_sym(299) __(59) nf_cmd_arg(273).
	p(NT(298), (NT(299)+NT(59)+NT(273)));
//G419: snf_cmd(297)         => __E_cli_command_122(298).
	p(NT(297), (NT(298)));
//G420: cli_command(223)     => snf_cmd(297).
	p(NT(223), (NT(297)));
//G421: onf_sym(302)         => 'o' 'n' 'f'.
	p(NT(302), (T(11)+T(47)+T(44)));
//G422: __E_cli_command_123(301) => onf_sym(302) __(59) variable(135) __(59) onf_cmd_arg(303).
	p(NT(301), (NT(302)+NT(59)+NT(135)+NT(59)+NT(303)));
//G423: onf_cmd(300)         => __E_cli_command_123(301).
	p(NT(300), (NT(301)));
//G424: cli_command(223)     => onf_cmd(300).
	p(NT(223), (NT(300)));
//G425: __E___E_cli_command_124_125(307) => 'd' 'e' 'f' 's'.
	p(NT(307), (T(54)+T(13)+T(44)+T(10)));
//G426: __E___E_cli_command_124_125(307) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(307), (T(54)+T(13)+T(44)+T(15)+T(47)+T(15)+T(14)+T(15)+T(11)+T(47)+T(10)));
//G427: def_sym(306)         => __E___E_cli_command_124_125(307).
	p(NT(306), (NT(307)));
//G428: __E_cli_command_124(305) => def_sym(306).
	p(NT(305), (NT(306)));
//G429: def_list_cmd(304)    => __E_cli_command_124(305).
	p(NT(304), (NT(305)));
//G430: cli_command(223)     => def_list_cmd(304).
	p(NT(223), (NT(304)));
//G431: __E_cli_command_126(309) => def_sym(306) __(59) number(310).
	p(NT(309), (NT(306)+NT(59)+NT(310)));
//G432: def_print_cmd(308)   => __E_cli_command_126(309).
	p(NT(308), (NT(309)));
//G433: cli_command(223)     => def_print_cmd(308).
	p(NT(223), (NT(308)));
//G434: def_rr_cmd(311)      => rec_relation(20).
	p(NT(311), (NT(20)));
//G435: cli_command(223)     => def_rr_cmd(311).
	p(NT(223), (NT(311)));
//G436: qelim_sym(314)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(314), (T(45)+T(13)+T(18)+T(15)+T(12)));
//G437: __E_cli_command_127(313) => qelim_sym(314) __(59) wff_cmd_arg(257).
	p(NT(313), (NT(314)+NT(59)+NT(257)));
//G438: qelim_cmd(312)       => __E_cli_command_127(313).
	p(NT(312), (NT(313)));
//G439: cli_command(223)     => qelim_cmd(312).
	p(NT(223), (NT(312)));
//G440: get_sym(317)         => 'g' 'e' 't'.
	p(NT(317), (T(49)+T(13)+T(14)));
//G441: __E___E_cli_command_128_129(318) => __(59) option(319).
	p(NT(318), (NT(59)+NT(319)));
//G442: __E___E_cli_command_128_129(318) => null.
	p(NT(318), (nul));
//G443: __E_cli_command_128(316) => get_sym(317) __E___E_cli_command_128_129(318).
	p(NT(316), (NT(317)+NT(318)));
//G444: get_cmd(315)         => __E_cli_command_128(316).
	p(NT(315), (NT(316)));
//G445: cli_command(223)     => get_cmd(315).
	p(NT(223), (NT(315)));
//G446: set_sym(322)         => 's' 'e' 't'.
	p(NT(322), (T(10)+T(13)+T(14)));
//G447: __E___E_cli_command_130_131(323) => __(59).
	p(NT(323), (NT(59)));
//G448: __E___E_cli_command_130_131(323) => _(15) '=' _(15).
	p(NT(323), (NT(15)+T(3)+NT(15)));
//G449: __E_cli_command_130(321) => set_sym(322) __(59) option(319) __E___E_cli_command_130_131(323) option_value(324).
	p(NT(321), (NT(322)+NT(59)+NT(319)+NT(323)+NT(324)));
//G450: set_cmd(320)         => __E_cli_command_130(321).
	p(NT(320), (NT(321)));
//G451: cli_command(223)     => set_cmd(320).
	p(NT(223), (NT(320)));
//G452: toggle_sym(327)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(327), (T(14)+T(11)+T(49)+T(49)+T(18)+T(13)));
//G453: __E_cli_command_132(326) => toggle_sym(327) __(59) bool_option(328).
	p(NT(326), (NT(327)+NT(59)+NT(328)));
//G454: toggle_cmd(325)      => __E_cli_command_132(326).
	p(NT(325), (NT(326)));
//G455: cli_command(223)     => toggle_cmd(325).
	p(NT(223), (NT(325)));
//G456: __E___E_cli_command_133_134(332) => 'h' 'i' 's' 't'.
	p(NT(332), (T(48)+T(15)+T(10)+T(14)));
//G457: __E___E_cli_command_133_134(332) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(332), (T(48)+T(15)+T(10)+T(14)+T(11)+T(51)+T(20)));
//G458: history_sym(331)     => __E___E_cli_command_133_134(332).
	p(NT(331), (NT(332)));
//G459: __E_cli_command_133(330) => history_sym(331).
	p(NT(330), (NT(331)));
//G460: history_list_cmd(329) => __E_cli_command_133(330).
	p(NT(329), (NT(330)));
//G461: cli_command(223)     => history_list_cmd(329).
	p(NT(223), (NT(329)));
//G462: __E_cli_command_135(334) => history_sym(331) __(59) memory(335).
	p(NT(334), (NT(331)+NT(59)+NT(335)));
//G463: history_print_cmd(333) => __E_cli_command_135(334).
	p(NT(333), (NT(334)));
//G464: cli_command(223)     => history_print_cmd(333).
	p(NT(223), (NT(333)));
//G465: __E_cli_command_136(337) => bf_typed(338).
	p(NT(337), (NT(338)));
//G466: __E_cli_command_136(337) => wff_typed(263).
	p(NT(337), (NT(263)));
//G467: history_store_cmd(336) => __E_cli_command_136(337).
	p(NT(336), (NT(337)));
//G468: cli_command(223)     => history_store_cmd(336).
	p(NT(223), (NT(336)));
//G469: number(310)          => digits(144).
	p(NT(310), (NT(144)));
//G470: bf_typed(338)        => bf(25).
	p(NT(338), (NT(25)));
//G471: bf_typed(338)        => bf(25) _(15) ':' _(15) 'b' 'f'.
	p(NT(338), (NT(25)+NT(15)+T(2)+NT(15)+T(24)+T(44)));
//G472: wff_typed(263)       => wff(18).
	p(NT(263), (NT(18)));
//G473: wff_typed(263)       => wff(18) _(15) ':' _(15) 'w' 'f' 'f'.
	p(NT(263), (NT(18)+NT(15)+T(2)+NT(15)+T(19)+T(44)+T(44)));
//G474: bf_cmd_arg(339)      => memory(335).
	p(NT(339), (NT(335)));
//G475: bf_cmd_arg(339)      => bf_typed(338).
	p(NT(339), (NT(338)));
//G476: wff_cmd_arg(257)     => memory(335).
	p(NT(257), (NT(335)));
//G477: wff_cmd_arg(257)     => wff_typed(263).
	p(NT(257), (NT(263)));
//G478: nf_cmd_arg(273)      => memory(335).
	p(NT(273), (NT(335)));
//G479: nf_cmd_arg(273)      => bf_typed(338).
	p(NT(273), (NT(338)));
//G480: nf_cmd_arg(273)      => wff_typed(263).
	p(NT(273), (NT(263)));
//G481: onf_cmd_arg(303)     => memory(335).
	p(NT(303), (NT(335)));
//G482: onf_cmd_arg(303)     => wff_typed(263).
	p(NT(303), (NT(263)));
//G483: normalize_cmd_arg(268) => memory(335).
	p(NT(268), (NT(335)));
//G484: normalize_cmd_arg(268) => bf_typed(338).
	p(NT(268), (NT(338)));
//G485: normalize_cmd_arg(268) => wff_typed(263).
	p(NT(268), (NT(263)));
//G486: normalize_cmd_arg(268) => rr(14).
	p(NT(268), (NT(14)));
//G487: inst_args(278)       => bf_cmd_arg(339) _(15) '[' _(15) variable(135) _(15) '/' _(15) bf_cmd_arg(339) _(15) ']'.
	p(NT(278), (NT(339)+NT(15)+T(21)+NT(15)+NT(135)+NT(15)+T(56)+NT(15)+NT(339)+NT(15)+T(22)));
//G488: inst_args(278)       => wff_cmd_arg(257) _(15) '[' _(15) variable(135) _(15) '/' _(15) bf_cmd_arg(339) _(15) ']'.
	p(NT(278), (NT(257)+NT(15)+T(21)+NT(15)+NT(135)+NT(15)+T(56)+NT(15)+NT(339)+NT(15)+T(22)));
//G489: inst_args(278)       => wff_cmd_arg(257) _(15) '[' _(15) bool_variable(114) _(15) '/' _(15) wff_cmd_arg(257) _(15) ']'.
	p(NT(278), (NT(257)+NT(15)+T(21)+NT(15)+NT(114)+NT(15)+T(56)+NT(15)+NT(257)+NT(15)+T(22)));
//G490: help_arg(240)        => help_sym(237).
	p(NT(240), (NT(237)));
//G491: help_arg(240)        => version_sym(230).
	p(NT(240), (NT(230)));
//G492: help_arg(240)        => quit_sym(227).
	p(NT(240), (NT(227)));
//G493: help_arg(240)        => clear_sym(233).
	p(NT(240), (NT(233)));
//G494: help_arg(240)        => get_sym(317).
	p(NT(240), (NT(317)));
//G495: help_arg(240)        => set_sym(322).
	p(NT(240), (NT(322)));
//G496: help_arg(240)        => toggle_sym(327).
	p(NT(240), (NT(327)));
//G497: help_arg(240)        => file_sym(243).
	p(NT(240), (NT(243)));
//G498: help_arg(240)        => history_sym(331).
	p(NT(240), (NT(331)));
//G499: help_arg(240)        => abs_memory_sym(340).
	p(NT(240), (NT(340)));
//G500: help_arg(240)        => rel_memory_sym(341).
	p(NT(240), (NT(341)));
//G501: help_arg(240)        => selection_sym(342).
	p(NT(240), (NT(342)));
//G502: help_arg(240)        => def_sym(306).
	p(NT(240), (NT(306)));
//G503: help_arg(240)        => inst_sym(276).
	p(NT(240), (NT(276)));
//G504: help_arg(240)        => subst_sym(271).
	p(NT(240), (NT(271)));
//G505: help_arg(240)        => normalize_sym(266).
	p(NT(240), (NT(266)));
//G506: help_arg(240)        => execute_sym(260).
	p(NT(240), (NT(260)));
//G507: help_arg(240)        => solve_sym(256).
	p(NT(240), (NT(256)));
//G508: help_arg(240)        => valid_sym(247).
	p(NT(240), (NT(247)));
//G509: help_arg(240)        => sat_sym(250).
	p(NT(240), (NT(250)));
//G510: help_arg(240)        => unsat_sym(253).
	p(NT(240), (NT(253)));
//G511: help_arg(240)        => dnf_sym(281).
	p(NT(240), (NT(281)));
//G512: help_arg(240)        => cnf_sym(284).
	p(NT(240), (NT(284)));
//G513: help_arg(240)        => anf_sym(287).
	p(NT(240), (NT(287)));
//G514: help_arg(240)        => snf_sym(299).
	p(NT(240), (NT(299)));
//G515: help_arg(240)        => nnf_sym(290).
	p(NT(240), (NT(290)));
//G516: help_arg(240)        => pnf_sym(293).
	p(NT(240), (NT(293)));
//G517: help_arg(240)        => mnf_sym(296).
	p(NT(240), (NT(296)));
//G518: help_arg(240)        => onf_sym(302).
	p(NT(240), (NT(302)));
//G519: help_arg(240)        => qelim_sym(314).
	p(NT(240), (NT(314)));
//G520: __E___E_help_arg_137_138(345) => 's'.
	p(NT(345), (T(10)));
//G521: __E___E_help_arg_137_138(345) => null.
	p(NT(345), (nul));
//G522: __E_help_arg_137(344) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_137_138(345).
	p(NT(344), (T(13)+T(26)+T(17)+T(12)+T(55)+T(18)+T(13)+NT(345)));
//G523: examples_sym(343)    => __E_help_arg_137(344).
	p(NT(343), (NT(344)));
//G524: help_arg(240)        => examples_sym(343).
	p(NT(240), (NT(343)));
//G525: __E_memory_139(347)  => '%'.
	p(NT(347), (T(57)));
//G526: rel_memory_sym(341)  => '%' '-'.
	p(NT(341), (T(57)+T(27)));
//G527: memory_id(349)       => digits(144).
	p(NT(349), (NT(144)));
//G528: __E___E_memory_139_140(348) => memory_id(349).
	p(NT(348), (NT(349)));
//G529: __E___E_memory_139_140(348) => null.
	p(NT(348), (nul));
//G530: __E_memory_139(347)  => rel_memory_sym(341) __E___E_memory_139_140(348).
	p(NT(347), (NT(341)+NT(348)));
//G531: rel_memory(346)      => __E_memory_139(347).
	p(NT(346), (NT(347)));
//G532: rel_memory(346)      => rel_memory(346).
	p(NT(346), (NT(346)));
//G533: memory(335)          => rel_memory(346).
	p(NT(335), (NT(346)));
//G534: abs_memory_sym(340)  => '%'.
	p(NT(340), (T(57)));
//G535: __E_memory_141(351)  => abs_memory_sym(340) memory_id(349).
	p(NT(351), (NT(340)+NT(349)));
//G536: abs_memory(350)      => __E_memory_141(351).
	p(NT(350), (NT(351)));
//G537: memory(335)          => abs_memory(350).
	p(NT(335), (NT(350)));
//G538: option(319)          => bool_option(328).
	p(NT(319), (NT(328)));
//G539: __E_option_142(354)  => 's' 'e' 'v'.
	p(NT(354), (T(10)+T(13)+T(52)));
//G540: __E_option_142(354)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(354), (T(10)+T(13)+T(52)+T(13)+T(51)+T(15)+T(14)+T(20)));
//G541: severity_opt(353)    => __E_option_142(354).
	p(NT(353), (NT(354)));
//G542: enum_option(352)     => severity_opt(353).
	p(NT(352), (NT(353)));
//G543: option(319)          => enum_option(352).
	p(NT(319), (NT(352)));
//G544: __E_bool_option_143(356) => 's'.
	p(NT(356), (T(10)));
//G545: __E_bool_option_143(356) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(356), (T(10)+T(14)+T(17)+T(14)+T(50)+T(10)));
//G546: status_opt(355)      => __E_bool_option_143(356).
	p(NT(355), (NT(356)));
//G547: bool_option(328)     => status_opt(355).
	p(NT(328), (NT(355)));
//G548: __E_bool_option_144(358) => 'c'.
	p(NT(358), (T(46)));
//G549: __E_bool_option_144(358) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(358), (T(46)+T(11)+T(18)+T(11)+T(51)));
//G550: __E_bool_option_144(358) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(358), (T(46)+T(11)+T(18)+T(11)+T(51)+T(10)));
//G551: colors_opt(357)      => __E_bool_option_144(358).
	p(NT(357), (NT(358)));
//G552: bool_option(328)     => colors_opt(357).
	p(NT(328), (NT(357)));
//G553: __E_bool_option_145(360) => 'd'.
	p(NT(360), (T(54)));
//G554: __E_bool_option_145(360) => 'd' 'b' 'g'.
	p(NT(360), (T(54)+T(24)+T(49)));
//G555: __E_bool_option_145(360) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(360), (T(54)+T(13)+T(24)+T(50)+T(49)));
//G556: debug_repl_opt(359)  => __E_bool_option_145(360).
	p(NT(359), (NT(360)));
//G557: bool_option(328)     => debug_repl_opt(359).
	p(NT(328), (NT(359)));
//G558: option_value(324)    => option_value_true(361).
	p(NT(324), (NT(361)));
//G559: option_value(324)    => option_value_false(362).
	p(NT(324), (NT(362)));
//G560: option_value(324)    => severity(363).
	p(NT(324), (NT(363)));
//G561: option_value_true(361) => 't'.
	p(NT(361), (T(14)));
//G562: option_value_true(361) => 't' 'r' 'u' 'e'.
	p(NT(361), (T(14)+T(51)+T(50)+T(13)));
//G563: option_value_true(361) => 'o' 'n'.
	p(NT(361), (T(11)+T(47)));
//G564: option_value_true(361) => '1'.
	p(NT(361), (T(37)));
//G565: option_value_true(361) => 'y'.
	p(NT(361), (T(20)));
//G566: option_value_true(361) => 'y' 'e' 's'.
	p(NT(361), (T(20)+T(13)+T(10)));
//G567: option_value_false(362) => 'f'.
	p(NT(362), (T(44)));
//G568: option_value_false(362) => 'f' 'a' 'l' 's' 'e'.
	p(NT(362), (T(44)+T(17)+T(18)+T(10)+T(13)));
//G569: option_value_false(362) => 'o' 'f' 'f'.
	p(NT(362), (T(11)+T(44)+T(44)));
//G570: option_value_false(362) => '0'.
	p(NT(362), (T(38)));
//G571: option_value_false(362) => 'n'.
	p(NT(362), (T(47)));
//G572: option_value_false(362) => 'n' 'o'.
	p(NT(362), (T(47)+T(11)));
//G573: __E_severity_146(365) => 'e'.
	p(NT(365), (T(13)));
//G574: __E_severity_146(365) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(365), (T(13)+T(51)+T(51)+T(11)+T(51)));
//G575: error_sym(364)       => __E_severity_146(365).
	p(NT(364), (NT(365)));
//G576: severity(363)        => error_sym(364).
	p(NT(363), (NT(364)));
//G577: __E_severity_147(367) => 'i'.
	p(NT(367), (T(15)));
//G578: __E_severity_147(367) => 'i' 'n' 'f' 'o'.
	p(NT(367), (T(15)+T(47)+T(44)+T(11)));
//G579: info_sym(366)        => __E_severity_147(367).
	p(NT(366), (NT(367)));
//G580: severity(363)        => info_sym(366).
	p(NT(363), (NT(366)));
//G581: __E_severity_148(369) => 'd'.
	p(NT(369), (T(54)));
//G582: __E_severity_148(369) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(369), (T(54)+T(13)+T(24)+T(50)+T(49)));
//G583: debug_sym(368)       => __E_severity_148(369).
	p(NT(368), (NT(369)));
//G584: severity(363)        => debug_sym(368).
	p(NT(363), (NT(368)));
//G585: __E_severity_149(371) => 't'.
	p(NT(371), (T(14)));
//G586: __E_severity_149(371) => 't' 'r' 'a' 'c' 'e'.
	p(NT(371), (T(14)+T(51)+T(17)+T(46)+T(13)));
//G587: trace_sym(370)       => __E_severity_149(371).
	p(NT(370), (NT(371)));
//G588: severity(363)        => trace_sym(370).
	p(NT(363), (NT(370)));
//G589: rule(43)             => tau_rule(373).
	p(NT(43), (NT(373)));
//G590: tau_matcher(374)     => tau(375).
	p(NT(374), (NT(375)));
//G591: __E_tau_rule_150(377) => tau(375).
	p(NT(377), (NT(375)));
//G592: __E_tau_rule_150(377) => tau_cb(372).
	p(NT(377), (NT(372)));
//G593: tau_body(376)        => __E_tau_rule_150(377).
	p(NT(376), (NT(377)));
//G594: tau_rule(373)        => tau_matcher(374) _(15) ':' ':' ':' '=' _(15) tau_body(376) _(15) '.'.
	p(NT(373), (NT(374)+NT(15)+T(2)+T(2)+T(2)+T(3)+NT(15)+NT(376)+NT(15)+T(1)));
//G595: __E_builder_body_151(379) => '=' ':' ':' ':' _(15) tau(375).
	p(NT(379), (T(3)+T(2)+T(2)+T(2)+NT(15)+NT(375)));
//G596: tau_builder_body(378) => __E_builder_body_151(379).
	p(NT(378), (NT(379)));
//G597: builder_body(57)     => tau_builder_body(378).
	p(NT(57), (NT(378)));
//G598: __E_tau_152(381)     => '(' _(15) tau(375) _(15) ')'.
	p(NT(381), (T(5)+NT(15)+NT(375)+NT(15)+T(6)));
//G599: tau_parenthesis(380) => __E_tau_152(381).
	p(NT(380), (NT(381)));
//G600: tau(375)             => tau_parenthesis(380).
	p(NT(375), (NT(380)));
//G601: __E_tau_153(383)     => '{' _(15) wff(18) _(15) '}'.
	p(NT(383), (T(8)+NT(15)+NT(18)+NT(15)+T(9)));
//G602: tau_wff(382)         => __E_tau_153(383).
	p(NT(382), (NT(383)));
//G603: tau(375)             => tau_wff(382).
	p(NT(375), (NT(382)));
//G604: __E___E_tau_154_155(386) => _(15) offsets(28).
	p(NT(386), (NT(15)+NT(28)));
//G605: __E___E_tau_154_155(386) => null.
	p(NT(386), (nul));
//G606: __E_tau_154(385)     => sym(26) __E___E_tau_154_155(386) _(15) ref_args(29).
	p(NT(385), (NT(26)+NT(386)+NT(15)+NT(29)));
//G607: tau_ref(384)         => __E_tau_154(385).
	p(NT(384), (NT(385)));
//G608: tau(375)             => tau_ref(384).
	p(NT(375), (NT(384)));
//G609: __E_tau_156(388)     => tau(375) _(15) '|' '|' '|' _(15) tau(375).
	p(NT(388), (NT(375)+NT(15)+T(28)+T(28)+T(28)+NT(15)+NT(375)));
//G610: tau_or(387)          => __E_tau_156(388).
	p(NT(387), (NT(388)));
//G611: tau(375)             => tau_or(387).
	p(NT(375), (NT(387)));
//G612: __E_tau_157(390)     => tau(375) _(15) '&' '&' '&' _(15) tau(375).
	p(NT(390), (NT(375)+NT(15)+T(30)+T(30)+T(30)+NT(15)+NT(375)));
//G613: tau_and(389)         => __E_tau_157(390).
	p(NT(389), (NT(390)));
//G614: tau(375)             => tau_and(389).
	p(NT(375), (NT(389)));
//G615: __E_tau_158(392)     => '-' _(15) tau(375).
	p(NT(392), (T(27)+NT(15)+NT(375)));
//G616: tau_neg(391)         => __E_tau_158(392).
	p(NT(391), (NT(392)));
//G617: tau(375)             => tau_neg(391).
	p(NT(375), (NT(391)));
//G618: tau(375)             => capture(24).
	p(NT(375), (NT(24)));
//G619: __E_tau_cb_args1_159(395) => capture(24).
	p(NT(395), (NT(24)));
//G620: __E_tau_cb_args1_159(395) => tau(375).
	p(NT(395), (NT(375)));
//G621: tau_cb_arg(394)      => __E_tau_cb_args1_159(395).
	p(NT(394), (NT(395)));
//G622: tau_cb_args1(393)    => __(59) tau_cb_arg(394).
	p(NT(393), (NT(59)+NT(394)));
//G623: tau_cb_args2(396)    => tau_cb_args1(393) tau_cb_args1(393).
	p(NT(396), (NT(393)+NT(393)));
//G624: tau_cb_args3(397)    => tau_cb_args2(396) tau_cb_args1(393).
	p(NT(397), (NT(396)+NT(393)));
//G625: __E___E_tau_cb_160_161(400) => tau_cb_args3(397).
	p(NT(400), (NT(397)));
//G626: __E___E_tau_cb_160_161(400) => tau_cb_args2(396).
	p(NT(400), (NT(396)));
//G627: __E_tau_cb_160(399)  => 't' 'a' 'u' '_' 'c' 'o' 'l' 'l' 'a' 'p' 's' 'e' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'c' 'b' __E___E_tau_cb_160_161(400).
	p(NT(399), (T(14)+T(17)+T(50)+T(25)+T(46)+T(11)+T(18)+T(18)+T(17)+T(55)+T(10)+T(13)+T(25)+T(55)+T(11)+T(10)+T(15)+T(14)+T(15)+T(52)+T(13)+T(10)+T(25)+T(46)+T(24)+NT(400)));
//G628: tau_collapse_positives_cb(398) => __E_tau_cb_160(399).
	p(NT(398), (NT(399)));
//G629: tau_cb(372)          => tau_collapse_positives_cb(398).
	p(NT(372), (NT(398)));
//G630: __E_tau_cb_162(402)  => 't' 'a' 'u' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'u' 'p' 'w' 'a' 'r' 'd' 's' '_' 'c' 'b' tau_cb_args2(396).
	p(NT(402), (T(14)+T(17)+T(50)+T(25)+T(55)+T(11)+T(10)+T(15)+T(14)+T(15)+T(52)+T(13)+T(10)+T(25)+T(50)+T(55)+T(19)+T(17)+T(51)+T(54)+T(10)+T(25)+T(46)+T(24)+NT(396)));
//G631: tau_positives_upwards_cb(401) => __E_tau_cb_162(402).
	p(NT(401), (NT(402)));
//G632: tau_cb(372)          => tau_positives_upwards_cb(401).
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
