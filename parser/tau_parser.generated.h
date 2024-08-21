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
	"wff_always", "__E_wff_24", "__E___E_wff_24_25", "wff_conditional", "__E_wff_26", "wff_ball", "__E_wff_27", "q_bool_vars", "wff_bex", "__E_wff_28", 
	"wff_all", "__E_wff_29", "q_vars", "wff_ex", "__E_wff_30", "wff_ref", "wff_imply", "__E_wff_31", "wff_equiv", "__E_wff_32", 
	"wff_or", "__E_wff_33", "wff_xor", "__E_wff_34", "wff_and", "__E_wff_35", "wff_neg", "__E_wff_36", "wff_t", "wff_f", 
	"bf_interval", "__E_wff_37", "bf_neq", "__E_wff_38", "bf_eq", "__E_wff_39", "bf_nleq", "__E_wff_40", "bf_greater", "__E_wff_41", 
	"bf_less_equal", "__E_wff_42", "bf_less", "__E_wff_43", "bool_variable", "uninterpreted_constant", "bf_parenthesis", "__E_bf_44", "bf_splitter", "__E_bf_45", 
	"bf_ref", "bf_or", "__E_bf_46", "bf_xor", "__E_bf_47", "bf_and", "__E_bf_48", "__E___E_bf_48_49", "bf_neg", "__E_bf_50", 
	"bf_constant", "__E_bf_51", "constant", "bf_t", "bf_f", "variable", "charvar", "__E_charvar_52", "__E_charvar_53", "io_var", 
	"__E_variable_54", "out", "in_var_name", "__E_in_55", "digits", "offset", "out_var_name", "__E_out_56", "q_bool_var", "__E_q_bool_vars_57", 
	"__E_q_bool_vars_58", "q_var", "__E_q_vars_59", "__E_q_vars_60", "__E_offsets_61", "__E_offsets_62", "num", "shift", "__E_offset_63", "__E_shift_64", 
	"__E___E_shift_64_65", "uninter_const_name", "__E_uninterpreted_constant_66", "__E___E_uninterpreted_constant_66_67", "chars", "binding", "__E_binding_68", "type", "__E___E_binding_68_69", "source", 
	"named_binding", "src_c", "__E_source_70", "__E_source_71", "__E___E_source_71_72", "__E___E_source_71_73", "__E_chars_74", "__E_digits_75", "comment", "__E_comment_76", 
	"__E_comment_77", "__E_comment_78", "__E____79", "__E___80", "bf_cb_args1", "bf_cb_arg", "bf_cb_args2", "bf_cb_args3", "bf_cb_args4", "wff_cb_args1", 
	"wff_cb_arg", "wff_cb_args2", "wff_cb_args3", "wff_cb_args4", "bf_eq_cb", "__E_wff_cb_81", "bf_neq_cb", "__E_wff_cb_82", "wff_has_clashing_subformulas_cb", "__E_wff_cb_83", 
	"wff_has_subformula_cb", "__E_wff_cb_84", "wff_remove_existential_cb", "__E_wff_cb_85", "wff_remove_bexistential_cb", "__E_wff_cb_86", "wff_remove_buniversal_cb", "__E_wff_cb_87", "bf_is_zero_cb", "__E_bf_cb_88", 
	"bf_is_one_cb", "__E_bf_cb_89", "bf_has_subformula_cb", "__E_bf_cb_90", "bf_remove_funiversal_cb", "__E_bf_cb_91", "bf_remove_fexistential_cb", "__E_bf_cb_92", "bf_and_cb", "__E_bf_cb_93", 
	"bf_or_cb", "__E_bf_cb_94", "bf_xor_cb", "__E_bf_cb_95", "bf_neg_cb", "__E_bf_cb_96", "bf_normalize_cb", "__E_bf_cb_97", "cli", "cli_command", 
	"__E_cli_98", "__E_cli_99", "quit_cmd", "quit_sym", "__E_cli_command_100", "version_cmd", "version_sym", "__E_cli_command_101", "clear_cmd", "clear_sym", 
	"__E_cli_command_102", "help_cmd", "__E_cli_command_103", "help_sym", "__E___E_cli_command_103_104", "__E___E_cli_command_103_105", "help_arg", "file_cmd", "__E_cli_command_106", "file_sym", 
	"q_string", "valid_cmd", "__E_cli_command_107", "valid_sym", "sat_cmd", "__E_cli_command_108", "sat_sym", "unsat_cmd", "__E_cli_command_109", "unsat_sym", 
	"solve_cmd", "__E_cli_command_110", "solve_sym", "wff_cmd_arg", "execute_cmd", "__E_cli_command_111", "execute_sym", "__E___E_cli_command_111_112", "__E___E_cli_command_111_113", "wff_typed", 
	"normalize_cmd", "__E_cli_command_114", "normalize_sym", "__E___E_cli_command_114_115", "normalize_cmd_arg", "subst_cmd", "__E_cli_command_116", "subst_sym", "__E___E_cli_command_116_117", "nf_cmd_arg", 
	"inst_cmd", "__E_cli_command_118", "inst_sym", "__E___E_cli_command_118_119", "inst_args", "dnf_cmd", "__E_cli_command_120", "dnf_sym", "cnf_cmd", "__E_cli_command_121", 
	"cnf_sym", "anf_cmd", "__E_cli_command_122", "anf_sym", "nnf_cmd", "__E_cli_command_123", "nnf_sym", "pnf_cmd", "__E_cli_command_124", "pnf_sym", 
	"mnf_cmd", "__E_cli_command_125", "mnf_sym", "snf_cmd", "__E_cli_command_126", "snf_sym", "onf_cmd", "__E_cli_command_127", "onf_sym", "onf_cmd_arg", 
	"def_list_cmd", "__E_cli_command_128", "def_sym", "__E___E_cli_command_128_129", "def_print_cmd", "__E_cli_command_130", "number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_131", 
	"qelim_sym", "get_cmd", "__E_cli_command_132", "get_sym", "__E___E_cli_command_132_133", "option", "set_cmd", "__E_cli_command_134", "set_sym", "__E___E_cli_command_134_135", 
	"option_value", "toggle_cmd", "__E_cli_command_136", "toggle_sym", "bool_option", "history_list_cmd", "__E_cli_command_137", "history_sym", "__E___E_cli_command_137_138", "history_print_cmd", 
	"__E_cli_command_139", "memory", "history_store_cmd", "__E_cli_command_140", "bf_typed", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "examples_sym", 
	"__E_help_arg_141", "__E___E_help_arg_141_142", "rel_memory", "__E_memory_143", "__E___E_memory_143_144", "memory_id", "abs_memory", "__E_memory_145", "enum_option", "severity_opt", 
	"__E_option_146", "status_opt", "__E_bool_option_147", "colors_opt", "__E_bool_option_148", "debug_repl_opt", "__E_bool_option_149", "option_value_true", "option_value_false", "severity", 
	"error_sym", "__E_severity_150", "info_sym", "__E_severity_151", "debug_sym", "__E_severity_152", "trace_sym", "__E_severity_153", "tau_cb", "tau_rule", 
	"tau_matcher", "tau", "tau_body", "__E_tau_rule_154", "tau_builder_body", "__E_builder_body_155", "tau_parenthesis", "__E_tau_156", "tau_wff", "__E_tau_157", 
	"tau_ref", "__E_tau_158", "__E___E_tau_158_159", "tau_or", "__E_tau_160", "tau_and", "__E_tau_161", "tau_neg", "__E_tau_162", "tau_cb_args1", 
	"tau_cb_arg", "__E_tau_cb_args1_163", "tau_cb_args2", "tau_cb_args3", "tau_collapse_positives_cb", "__E_tau_cb_164", "__E___E_tau_cb_164_165", "tau_positives_upwards_cb", "__E_tau_cb_166", "__N_0", 
	"__N_1", "__N_2", "__N_3", "__N_4", "__N_5", 
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
			24, 26, 114, 136, 142, 144, 146, 161, 167, 169,
			170
		},
		.to_inline = {
			{ 18, 65, 18 },
			{ 25, 116, 25 },
			{ 51 },
			{ 55 },
			{ 148 },
			{ 151 },
			{ 164 },
			{ 171 },
			{ 184 },
			{ 186 },
			{ 187 },
			{ 188 },
			{ 189 },
			{ 191 },
			{ 192 },
			{ 193 },
			{ 263 },
			{ 269 },
			{ 274 },
			{ 279 },
			{ 284 },
			{ 309 },
			{ 344 },
			{ 345 },
			{ 378 },
			{ 381, 386, 381 },
			{ 399 },
			{ 402 },
			{ 403 }
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
//G78:  __E_wff_27(76)       => 'b' '_' 'a' 'l' 'l' __(59) q_bool_vars(77) __(59) wff(18).
	p(NT(76), (T(24)+T(25)+T(17)+T(18)+T(18)+NT(59)+NT(77)+NT(59)+NT(18)));
//G79:  wff_ball(75)         => __E_wff_27(76).
	p(NT(75), (NT(76)));
//G80:  wff(18)              => wff_ball(75).
	p(NT(18), (NT(75)));
//G81:  __E_wff_28(79)       => 'b' '_' 'e' 'x' __(59) q_bool_vars(77) __(59) wff(18).
	p(NT(79), (T(24)+T(25)+T(13)+T(26)+NT(59)+NT(77)+NT(59)+NT(18)));
//G82:  wff_bex(78)          => __E_wff_28(79).
	p(NT(78), (NT(79)));
//G83:  wff(18)              => wff_bex(78).
	p(NT(18), (NT(78)));
//G84:  __E_wff_29(81)       => 'a' 'l' 'l' __(59) q_vars(82) __(59) wff(18).
	p(NT(81), (T(17)+T(18)+T(18)+NT(59)+NT(82)+NT(59)+NT(18)));
//G85:  wff_all(80)          => __E_wff_29(81).
	p(NT(80), (NT(81)));
//G86:  wff(18)              => wff_all(80).
	p(NT(18), (NT(80)));
//G87:  __E_wff_30(84)       => 'e' 'x' __(59) q_vars(82) __(59) wff(18).
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
//G169: __N_0(409)           => 'F'.
	p(NT(409), (T(33)));
//G170: __N_1(410)           => 'T'.
	p(NT(410), (T(32)));
//G171: __N_2(411)           => 'i'.
	p(NT(411), (T(15)));
//G172: __N_3(412)           => 'o'.
	p(NT(412), (T(11)));
//G173: __E_charvar_52(137)  => ~( __N_0(409) ) & ~( __N_1(410) ) & ~( __N_2(411) ) & ~( __N_3(412) ) & alpha(5).	 # conjunctive
	p(NT(137), ~(NT(409)) & ~(NT(410)) & ~(NT(411)) & ~(NT(412)) & (NT(5)));
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
//G190: __E_q_bool_vars_57(149) => _(15) ',' _(15) q_bool_var(148).
	p(NT(149), (NT(15)+T(4)+NT(15)+NT(148)));
//G191: __E_q_bool_vars_58(150) => null.
	p(NT(150), (nul));
//G192: __E_q_bool_vars_58(150) => __E_q_bool_vars_57(149) __E_q_bool_vars_58(150).
	p(NT(150), (NT(149)+NT(150)));
//G193: q_bool_vars(77)      => q_bool_var(148) __E_q_bool_vars_58(150).
	p(NT(77), (NT(148)+NT(150)));
//G194: __E_q_vars_59(152)   => _(15) ',' _(15) q_var(151).
	p(NT(152), (NT(15)+T(4)+NT(15)+NT(151)));
//G195: __E_q_vars_60(153)   => null.
	p(NT(153), (nul));
//G196: __E_q_vars_60(153)   => __E_q_vars_59(152) __E_q_vars_60(153).
	p(NT(153), (NT(152)+NT(153)));
//G197: q_vars(82)           => q_var(151) __E_q_vars_60(153).
	p(NT(82), (NT(151)+NT(153)));
//G198: q_bool_var(148)      => capture(24).
	p(NT(148), (NT(24)));
//G199: q_bool_var(148)      => bool_variable(114).
	p(NT(148), (NT(114)));
//G200: q_var(151)           => capture(24).
	p(NT(151), (NT(24)));
//G201: q_var(151)           => variable(135).
	p(NT(151), (NT(135)));
//G202: __E_offsets_61(154)  => _(15) ',' _(15) offset(145).
	p(NT(154), (NT(15)+T(4)+NT(15)+NT(145)));
//G203: __E_offsets_62(155)  => null.
	p(NT(155), (nul));
//G204: __E_offsets_62(155)  => __E_offsets_61(154) __E_offsets_62(155).
	p(NT(155), (NT(154)+NT(155)));
//G205: offsets(28)          => '[' _(15) offset(145) __E_offsets_62(155) _(15) ']'.
	p(NT(28), (T(21)+NT(15)+NT(145)+NT(155)+NT(15)+T(22)));
//G206: offset(145)          => num(156).
	p(NT(145), (NT(156)));
//G207: offset(145)          => capture(24).
	p(NT(145), (NT(24)));
//G208: offset(145)          => shift(157).
	p(NT(145), (NT(157)));
//G209: __N_4(413)           => io_var(139).
	p(NT(413), (NT(139)));
//G210: __E_offset_63(158)   => variable(135) & ~( __N_4(413) ).	 # conjunctive
	p(NT(158), (NT(135)) & ~(NT(413)));
//G211: offset(145)          => __E_offset_63(158).
	p(NT(145), (NT(158)));
//G212: __E_shift_64(159)    => capture(24).
	p(NT(159), (NT(24)));
//G213: __N_5(414)           => io_var(139).
	p(NT(414), (NT(139)));
//G214: __E___E_shift_64_65(160) => variable(135) & ~( __N_5(414) ).	 # conjunctive
	p(NT(160), (NT(135)) & ~(NT(414)));
//G215: __E_shift_64(159)    => __E___E_shift_64_65(160).
	p(NT(159), (NT(160)));
//G216: shift(157)           => __E_shift_64(159) _(15) '-' _(15) num(156).
	p(NT(157), (NT(159)+NT(15)+T(27)+NT(15)+NT(156)));
//G217: num(156)             => digits(144).
	p(NT(156), (NT(144)));
//G218: __E___E_uninterpreted_constant_66_67(163) => chars(164).
	p(NT(163), (NT(164)));
//G219: __E___E_uninterpreted_constant_66_67(163) => _(15).
	p(NT(163), (NT(15)));
//G220: __E_uninterpreted_constant_66(162) => __E___E_uninterpreted_constant_66_67(163) ':' chars(164).
	p(NT(162), (NT(163)+T(2)+NT(164)));
//G221: uninter_const_name(161) => __E_uninterpreted_constant_66(162).
	p(NT(161), (NT(162)));
//G222: uninterpreted_constant(115) => '<' uninter_const_name(161) _(15) '>'.
	p(NT(115), (T(7)+NT(161)+NT(15)+T(16)));
//G223: constant(132)        => binding(165).
	p(NT(132), (NT(165)));
//G224: constant(132)        => capture(24).
	p(NT(132), (NT(24)));
//G225: __E___E_binding_68_69(168) => chars(164) _(15).
	p(NT(168), (NT(164)+NT(15)));
//G226: __E___E_binding_68_69(168) => _(15).
	p(NT(168), (NT(15)));
//G227: type(167)            => __E___E_binding_68_69(168).
	p(NT(167), (NT(168)));
//G228: __E_binding_68(166)  => type(167) ':' _(15) source(169).
	p(NT(166), (NT(167)+T(2)+NT(15)+NT(169)));
//G229: source_binding(39)   => __E_binding_68(166).
	p(NT(39), (NT(166)));
//G230: binding(165)         => source_binding(39).
	p(NT(165), (NT(39)));
//G231: named_binding(170)   => chars(164).
	p(NT(170), (NT(164)));
//G232: binding(165)         => named_binding(170).
	p(NT(165), (NT(170)));
//G233: __E_source_70(172)   => alnum(6).
	p(NT(172), (NT(6)));
//G234: __E_source_70(172)   => punct(7).
	p(NT(172), (NT(7)));
//G235: src_c(171)           => __E_source_70(172).
	p(NT(171), (NT(172)));
//G236: __E___E_source_71_72(174) => src_c(171).
	p(NT(174), (NT(171)));
//G237: __E___E_source_71_72(174) => space(2).
	p(NT(174), (NT(2)));
//G238: __E___E_source_71_73(175) => null.
	p(NT(175), (nul));
//G239: __E___E_source_71_73(175) => __E___E_source_71_72(174) __E___E_source_71_73(175).
	p(NT(175), (NT(174)+NT(175)));
//G240: __E_source_71(173)   => __E___E_source_71_73(175) src_c(171).
	p(NT(173), (NT(175)+NT(171)));
//G241: __E_source_71(173)   => null.
	p(NT(173), (nul));
//G242: source(169)          => src_c(171) __E_source_71(173).
	p(NT(169), (NT(171)+NT(173)));
//G243: __E_chars_74(176)    => null.
	p(NT(176), (nul));
//G244: __E_chars_74(176)    => alnum(6) __E_chars_74(176).
	p(NT(176), (NT(6)+NT(176)));
//G245: chars(164)           => alpha(5) __E_chars_74(176).
	p(NT(164), (NT(5)+NT(176)));
//G246: __E_digits_75(177)   => digit(3).
	p(NT(177), (NT(3)));
//G247: __E_digits_75(177)   => digit(3) __E_digits_75(177).
	p(NT(177), (NT(3)+NT(177)));
//G248: digits(144)          => __E_digits_75(177).
	p(NT(144), (NT(177)));
//G249: sym(26)              => chars(164).
	p(NT(26), (NT(164)));
//G250: __E_comment_76(179)  => printable(8).
	p(NT(179), (NT(8)));
//G251: __E_comment_76(179)  => '\t'.
	p(NT(179), (T(40)));
//G252: __E_comment_77(180)  => null.
	p(NT(180), (nul));
//G253: __E_comment_77(180)  => __E_comment_76(179) __E_comment_77(180).
	p(NT(180), (NT(179)+NT(180)));
//G254: __E_comment_78(181)  => '\n'.
	p(NT(181), (T(41)));
//G255: __E_comment_78(181)  => '\r'.
	p(NT(181), (T(42)));
//G256: __E_comment_78(181)  => eof(1).
	p(NT(181), (NT(1)));
//G257: comment(178)         => '#' __E_comment_77(180) __E_comment_78(181).
	p(NT(178), (T(43)+NT(180)+NT(181)));
//G258: __E____79(182)       => space(2).
	p(NT(182), (NT(2)));
//G259: __E____79(182)       => comment(178).
	p(NT(182), (NT(178)));
//G260: __(59)               => __E____79(182) _(15).
	p(NT(59), (NT(182)+NT(15)));
//G261: __E___80(183)        => __(59).
	p(NT(183), (NT(59)));
//G262: __E___80(183)        => null.
	p(NT(183), (nul));
//G263: _(15)                => __E___80(183).
	p(NT(15), (NT(183)));
//G264: bf_cb_arg(185)       => bf(25).
	p(NT(185), (NT(25)));
//G265: bf_cb_args1(184)     => __(59) bf_cb_arg(185).
	p(NT(184), (NT(59)+NT(185)));
//G266: bf_cb_args2(186)     => bf_cb_args1(184) bf_cb_args1(184).
	p(NT(186), (NT(184)+NT(184)));
//G267: bf_cb_args3(187)     => bf_cb_args2(186) bf_cb_args1(184).
	p(NT(187), (NT(186)+NT(184)));
//G268: bf_cb_args4(188)     => bf_cb_args3(187) bf_cb_args1(184).
	p(NT(188), (NT(187)+NT(184)));
//G269: wff_cb_arg(190)      => wff(18).
	p(NT(190), (NT(18)));
//G270: wff_cb_args1(189)    => __(59) wff_cb_arg(190).
	p(NT(189), (NT(59)+NT(190)));
//G271: wff_cb_args2(191)    => wff_cb_args1(189) wff_cb_args1(189).
	p(NT(191), (NT(189)+NT(189)));
//G272: wff_cb_args3(192)    => wff_cb_args2(191) wff_cb_args1(189).
	p(NT(192), (NT(191)+NT(189)));
//G273: wff_cb_args4(193)    => wff_cb_args3(192) wff_cb_args1(189).
	p(NT(193), (NT(192)+NT(189)));
//G274: __E_wff_cb_81(195)   => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b' bf_cb_args1(184) wff_cb_args2(191).
	p(NT(195), (T(24)+T(44)+T(25)+T(13)+T(45)+T(25)+T(46)+T(24)+NT(184)+NT(191)));
//G275: bf_eq_cb(194)        => __E_wff_cb_81(195).
	p(NT(194), (NT(195)));
//G276: wff_cb(51)           => bf_eq_cb(194).
	p(NT(51), (NT(194)));
//G277: __E_wff_cb_82(197)   => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b' bf_cb_args1(184) wff_cb_args2(191).
	p(NT(197), (T(24)+T(44)+T(25)+T(47)+T(13)+T(45)+T(25)+T(46)+T(24)+NT(184)+NT(191)));
//G278: bf_neq_cb(196)       => __E_wff_cb_82(197).
	p(NT(196), (NT(197)));
//G279: wff_cb(51)           => bf_neq_cb(196).
	p(NT(51), (NT(196)));
//G280: __E_wff_cb_83(199)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(191).
	p(NT(199), (T(19)+T(44)+T(44)+T(25)+T(48)+T(17)+T(10)+T(25)+T(46)+T(18)+T(17)+T(10)+T(48)+T(15)+T(47)+T(49)+T(25)+T(10)+T(50)+T(24)+T(44)+T(11)+T(51)+T(12)+T(50)+T(18)+T(17)+T(10)+T(25)+T(46)+T(24)+NT(191)));
//G281: wff_has_clashing_subformulas_cb(198) => __E_wff_cb_83(199).
	p(NT(198), (NT(199)));
//G282: wff_cb(51)           => wff_has_clashing_subformulas_cb(198).
	p(NT(51), (NT(198)));
//G283: __E_wff_cb_84(201)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(192).
	p(NT(201), (T(19)+T(44)+T(44)+T(25)+T(48)+T(17)+T(10)+T(25)+T(10)+T(50)+T(24)+T(44)+T(11)+T(51)+T(12)+T(50)+T(18)+T(17)+T(25)+T(46)+T(24)+NT(192)));
//G284: wff_has_subformula_cb(200) => __E_wff_cb_84(201).
	p(NT(200), (NT(201)));
//G285: wff_cb(51)           => wff_has_subformula_cb(200).
	p(NT(51), (NT(200)));
//G286: __E_wff_cb_85(203)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(191).
	p(NT(203), (T(19)+T(44)+T(44)+T(25)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(25)+T(13)+T(26)+T(15)+T(10)+T(14)+T(13)+T(47)+T(14)+T(15)+T(17)+T(18)+T(25)+T(46)+T(24)+NT(191)));
//G287: wff_remove_existential_cb(202) => __E_wff_cb_85(203).
	p(NT(202), (NT(203)));
//G288: wff_cb(51)           => wff_remove_existential_cb(202).
	p(NT(51), (NT(202)));
//G289: __E_wff_cb_86(205)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args4(193).
	p(NT(205), (T(19)+T(44)+T(44)+T(25)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(25)+T(24)+T(13)+T(26)+T(15)+T(10)+T(14)+T(13)+T(47)+T(14)+T(15)+T(17)+T(18)+T(25)+T(46)+T(24)+NT(193)));
//G290: wff_remove_bexistential_cb(204) => __E_wff_cb_86(205).
	p(NT(204), (NT(205)));
//G291: wff_cb(51)           => wff_remove_bexistential_cb(204).
	p(NT(51), (NT(204)));
//G292: __E_wff_cb_87(207)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' wff_cb_args4(193).
	p(NT(207), (T(19)+T(44)+T(44)+T(25)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(25)+T(24)+T(50)+T(47)+T(15)+T(52)+T(13)+T(51)+T(10)+T(17)+T(18)+T(25)+T(46)+T(24)+NT(193)));
//G293: wff_remove_buniversal_cb(206) => __E_wff_cb_87(207).
	p(NT(206), (NT(207)));
//G294: wff_cb(51)           => wff_remove_buniversal_cb(206).
	p(NT(51), (NT(206)));
//G295: __E_bf_cb_88(209)    => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b' bf_cb_args2(186).
	p(NT(209), (T(24)+T(44)+T(25)+T(15)+T(10)+T(25)+T(53)+T(13)+T(51)+T(11)+T(25)+T(46)+T(24)+NT(186)));
//G296: bf_is_zero_cb(208)   => __E_bf_cb_88(209).
	p(NT(208), (NT(209)));
//G297: bf_cb(55)            => bf_is_zero_cb(208).
	p(NT(55), (NT(208)));
//G298: __E_bf_cb_89(211)    => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b' bf_cb_args2(186).
	p(NT(211), (T(24)+T(44)+T(25)+T(15)+T(10)+T(25)+T(11)+T(47)+T(13)+T(25)+T(46)+T(24)+NT(186)));
//G299: bf_is_one_cb(210)    => __E_bf_cb_89(211).
	p(NT(210), (NT(211)));
//G300: bf_cb(55)            => bf_is_one_cb(210).
	p(NT(55), (NT(210)));
//G301: __E_bf_cb_90(213)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(187).
	p(NT(213), (T(24)+T(44)+T(25)+T(48)+T(17)+T(10)+T(25)+T(10)+T(50)+T(24)+T(44)+T(11)+T(51)+T(12)+T(50)+T(18)+T(17)+T(25)+T(46)+T(24)+NT(187)));
//G302: bf_has_subformula_cb(212) => __E_bf_cb_90(213).
	p(NT(212), (NT(213)));
//G303: bf_cb(55)            => bf_has_subformula_cb(212).
	p(NT(55), (NT(212)));
//G304: __E_bf_cb_91(215)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(188).
	p(NT(215), (T(24)+T(44)+T(25)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(25)+T(44)+T(50)+T(47)+T(15)+T(52)+T(13)+T(51)+T(10)+T(17)+T(18)+T(25)+T(46)+T(24)+NT(188)));
//G305: bf_remove_funiversal_cb(214) => __E_bf_cb_91(215).
	p(NT(214), (NT(215)));
//G306: bf_cb(55)            => bf_remove_funiversal_cb(214).
	p(NT(55), (NT(214)));
//G307: __E_bf_cb_92(217)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(188).
	p(NT(217), (T(24)+T(44)+T(25)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(25)+T(44)+T(13)+T(26)+T(15)+T(10)+T(14)+T(13)+T(47)+T(14)+T(15)+T(17)+T(18)+T(25)+T(46)+T(24)+NT(188)));
//G308: bf_remove_fexistential_cb(216) => __E_bf_cb_92(217).
	p(NT(216), (NT(217)));
//G309: bf_cb(55)            => bf_remove_fexistential_cb(216).
	p(NT(55), (NT(216)));
//G310: __E_bf_cb_93(219)    => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b' bf_cb_args2(186).
	p(NT(219), (T(24)+T(44)+T(25)+T(17)+T(47)+T(54)+T(25)+T(46)+T(24)+NT(186)));
//G311: bf_and_cb(218)       => __E_bf_cb_93(219).
	p(NT(218), (NT(219)));
//G312: bf_cb(55)            => bf_and_cb(218).
	p(NT(55), (NT(218)));
//G313: __E_bf_cb_94(221)    => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b' bf_cb_args2(186).
	p(NT(221), (T(24)+T(44)+T(25)+T(11)+T(51)+T(25)+T(46)+T(24)+NT(186)));
//G314: bf_or_cb(220)        => __E_bf_cb_94(221).
	p(NT(220), (NT(221)));
//G315: bf_cb(55)            => bf_or_cb(220).
	p(NT(55), (NT(220)));
//G316: __E_bf_cb_95(223)    => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b' bf_cb_args2(186).
	p(NT(223), (T(24)+T(44)+T(25)+T(26)+T(11)+T(51)+T(25)+T(46)+T(24)+NT(186)));
//G317: bf_xor_cb(222)       => __E_bf_cb_95(223).
	p(NT(222), (NT(223)));
//G318: bf_cb(55)            => bf_xor_cb(222).
	p(NT(55), (NT(222)));
//G319: __E_bf_cb_96(225)    => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b' bf_cb_args1(184).
	p(NT(225), (T(24)+T(44)+T(25)+T(47)+T(13)+T(49)+T(25)+T(46)+T(24)+NT(184)));
//G320: bf_neg_cb(224)       => __E_bf_cb_96(225).
	p(NT(224), (NT(225)));
//G321: bf_cb(55)            => bf_neg_cb(224).
	p(NT(55), (NT(224)));
//G322: __E_bf_cb_97(227)    => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(184).
	p(NT(227), (T(24)+T(44)+T(25)+T(47)+T(11)+T(51)+T(12)+T(17)+T(18)+T(15)+T(53)+T(13)+T(25)+T(46)+T(24)+NT(184)));
//G323: bf_normalize_cb(226) => __E_bf_cb_97(227).
	p(NT(226), (NT(227)));
//G324: bf_cb(55)            => bf_normalize_cb(226).
	p(NT(55), (NT(226)));
//G325: cli(228)             => _(15).
	p(NT(228), (NT(15)));
//G326: __E_cli_98(230)      => '.' _(15) cli_command(229) _(15).
	p(NT(230), (T(1)+NT(15)+NT(229)+NT(15)));
//G327: __E_cli_99(231)      => null.
	p(NT(231), (nul));
//G328: __E_cli_99(231)      => __E_cli_98(230) __E_cli_99(231).
	p(NT(231), (NT(230)+NT(231)));
//G329: cli(228)             => _(15) cli_command(229) _(15) __E_cli_99(231).
	p(NT(228), (NT(15)+NT(229)+NT(15)+NT(231)));
//G330: __E_cli_command_100(234) => 'q'.
	p(NT(234), (T(45)));
//G331: __E_cli_command_100(234) => 'q' 'u' 'i' 't'.
	p(NT(234), (T(45)+T(50)+T(15)+T(14)));
//G332: quit_sym(233)        => __E_cli_command_100(234).
	p(NT(233), (NT(234)));
//G333: quit_cmd(232)        => quit_sym(233).
	p(NT(232), (NT(233)));
//G334: cli_command(229)     => quit_cmd(232).
	p(NT(229), (NT(232)));
//G335: __E_cli_command_101(237) => 'v'.
	p(NT(237), (T(52)));
//G336: __E_cli_command_101(237) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(237), (T(52)+T(13)+T(51)+T(10)+T(15)+T(11)+T(47)));
//G337: version_sym(236)     => __E_cli_command_101(237).
	p(NT(236), (NT(237)));
//G338: version_cmd(235)     => version_sym(236).
	p(NT(235), (NT(236)));
//G339: cli_command(229)     => version_cmd(235).
	p(NT(229), (NT(235)));
//G340: __E_cli_command_102(240) => 'c'.
	p(NT(240), (T(46)));
//G341: __E_cli_command_102(240) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(240), (T(46)+T(18)+T(13)+T(17)+T(51)));
//G342: clear_sym(239)       => __E_cli_command_102(240).
	p(NT(239), (NT(240)));
//G343: clear_cmd(238)       => clear_sym(239).
	p(NT(238), (NT(239)));
//G344: cli_command(229)     => clear_cmd(238).
	p(NT(229), (NT(238)));
//G345: __E___E_cli_command_103_104(244) => 'h'.
	p(NT(244), (T(48)));
//G346: __E___E_cli_command_103_104(244) => 'h' 'e' 'l' 'p'.
	p(NT(244), (T(48)+T(13)+T(18)+T(55)));
//G347: help_sym(243)        => __E___E_cli_command_103_104(244).
	p(NT(243), (NT(244)));
//G348: __E___E_cli_command_103_105(245) => __(59) help_arg(246).
	p(NT(245), (NT(59)+NT(246)));
//G349: __E___E_cli_command_103_105(245) => null.
	p(NT(245), (nul));
//G350: __E_cli_command_103(242) => help_sym(243) __E___E_cli_command_103_105(245).
	p(NT(242), (NT(243)+NT(245)));
//G351: help_cmd(241)        => __E_cli_command_103(242).
	p(NT(241), (NT(242)));
//G352: cli_command(229)     => help_cmd(241).
	p(NT(229), (NT(241)));
//G353: file_sym(249)        => 'f' 'i' 'l' 'e'.
	p(NT(249), (T(44)+T(15)+T(18)+T(13)));
//G354: __E_cli_command_106(248) => file_sym(249) __(59) q_string(250).
	p(NT(248), (NT(249)+NT(59)+NT(250)));
//G355: file_cmd(247)        => __E_cli_command_106(248).
	p(NT(247), (NT(248)));
//G356: cli_command(229)     => file_cmd(247).
	p(NT(229), (NT(247)));
//G357: valid_sym(253)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(253), (T(52)+T(17)+T(18)+T(15)+T(54)));
//G358: __E_cli_command_107(252) => valid_sym(253) __(59) rr(14).
	p(NT(252), (NT(253)+NT(59)+NT(14)));
//G359: valid_cmd(251)       => __E_cli_command_107(252).
	p(NT(251), (NT(252)));
//G360: cli_command(229)     => valid_cmd(251).
	p(NT(229), (NT(251)));
//G361: sat_sym(256)         => 's' 'a' 't'.
	p(NT(256), (T(10)+T(17)+T(14)));
//G362: __E_cli_command_108(255) => sat_sym(256) __(59) rr(14).
	p(NT(255), (NT(256)+NT(59)+NT(14)));
//G363: sat_cmd(254)         => __E_cli_command_108(255).
	p(NT(254), (NT(255)));
//G364: cli_command(229)     => sat_cmd(254).
	p(NT(229), (NT(254)));
//G365: unsat_sym(259)       => 'u' 'n' 's' 'a' 't'.
	p(NT(259), (T(50)+T(47)+T(10)+T(17)+T(14)));
//G366: __E_cli_command_109(258) => unsat_sym(259) __(59) rr(14).
	p(NT(258), (NT(259)+NT(59)+NT(14)));
//G367: unsat_cmd(257)       => __E_cli_command_109(258).
	p(NT(257), (NT(258)));
//G368: cli_command(229)     => unsat_cmd(257).
	p(NT(229), (NT(257)));
//G369: solve_sym(262)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(262), (T(10)+T(11)+T(18)+T(52)+T(13)));
//G370: __E_cli_command_110(261) => solve_sym(262) __(59) wff_cmd_arg(263).
	p(NT(261), (NT(262)+NT(59)+NT(263)));
//G371: solve_cmd(260)       => __E_cli_command_110(261).
	p(NT(260), (NT(261)));
//G372: cli_command(229)     => solve_cmd(260).
	p(NT(229), (NT(260)));
//G373: __E___E_cli_command_111_112(267) => 'e'.
	p(NT(267), (T(13)));
//G374: __E___E_cli_command_111_112(267) => 'e' 'x' 'e' 'c' 'u' 't' 'e'.
	p(NT(267), (T(13)+T(26)+T(13)+T(46)+T(50)+T(14)+T(13)));
//G375: execute_sym(266)     => __E___E_cli_command_111_112(267).
	p(NT(266), (NT(267)));
//G376: __E___E_cli_command_111_113(268) => wff_typed(269).
	p(NT(268), (NT(269)));
//G377: __E___E_cli_command_111_113(268) => rr(14).
	p(NT(268), (NT(14)));
//G378: __E_cli_command_111(265) => execute_sym(266) __(59) __E___E_cli_command_111_113(268).
	p(NT(265), (NT(266)+NT(59)+NT(268)));
//G379: execute_cmd(264)     => __E_cli_command_111(265).
	p(NT(264), (NT(265)));
//G380: cli_command(229)     => execute_cmd(264).
	p(NT(229), (NT(264)));
//G381: __E___E_cli_command_114_115(273) => 'n'.
	p(NT(273), (T(47)));
//G382: __E___E_cli_command_114_115(273) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(273), (T(47)+T(11)+T(51)+T(12)+T(17)+T(18)+T(15)+T(53)+T(13)));
//G383: normalize_sym(272)   => __E___E_cli_command_114_115(273).
	p(NT(272), (NT(273)));
//G384: __E_cli_command_114(271) => normalize_sym(272) __(59) normalize_cmd_arg(274).
	p(NT(271), (NT(272)+NT(59)+NT(274)));
//G385: normalize_cmd(270)   => __E_cli_command_114(271).
	p(NT(270), (NT(271)));
//G386: cli_command(229)     => normalize_cmd(270).
	p(NT(229), (NT(270)));
//G387: __E___E_cli_command_116_117(278) => 's'.
	p(NT(278), (T(10)));
//G388: __E___E_cli_command_116_117(278) => 's' 'u' 'b' 's' 't'.
	p(NT(278), (T(10)+T(50)+T(24)+T(10)+T(14)));
//G389: __E___E_cli_command_116_117(278) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(278), (T(10)+T(50)+T(24)+T(10)+T(14)+T(15)+T(14)+T(50)+T(14)+T(13)));
//G390: subst_sym(277)       => __E___E_cli_command_116_117(278).
	p(NT(277), (NT(278)));
//G391: __E_cli_command_116(276) => subst_sym(277) __(59) nf_cmd_arg(279) _(15) '[' _(15) nf_cmd_arg(279) _(15) '/' _(15) nf_cmd_arg(279) _(15) ']'.
	p(NT(276), (NT(277)+NT(59)+NT(279)+NT(15)+T(21)+NT(15)+NT(279)+NT(15)+T(56)+NT(15)+NT(279)+NT(15)+T(22)));
//G392: subst_cmd(275)       => __E_cli_command_116(276).
	p(NT(275), (NT(276)));
//G393: cli_command(229)     => subst_cmd(275).
	p(NT(229), (NT(275)));
//G394: __E___E_cli_command_118_119(283) => 'i'.
	p(NT(283), (T(15)));
//G395: __E___E_cli_command_118_119(283) => 'i' 'n' 's' 't'.
	p(NT(283), (T(15)+T(47)+T(10)+T(14)));
//G396: __E___E_cli_command_118_119(283) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(283), (T(15)+T(47)+T(10)+T(14)+T(17)+T(47)+T(14)+T(15)+T(17)+T(14)+T(13)));
//G397: inst_sym(282)        => __E___E_cli_command_118_119(283).
	p(NT(282), (NT(283)));
//G398: __E_cli_command_118(281) => inst_sym(282) __(59) inst_args(284).
	p(NT(281), (NT(282)+NT(59)+NT(284)));
//G399: inst_cmd(280)        => __E_cli_command_118(281).
	p(NT(280), (NT(281)));
//G400: cli_command(229)     => inst_cmd(280).
	p(NT(229), (NT(280)));
//G401: dnf_sym(287)         => 'd' 'n' 'f'.
	p(NT(287), (T(54)+T(47)+T(44)));
//G402: __E_cli_command_120(286) => dnf_sym(287) __(59) nf_cmd_arg(279).
	p(NT(286), (NT(287)+NT(59)+NT(279)));
//G403: dnf_cmd(285)         => __E_cli_command_120(286).
	p(NT(285), (NT(286)));
//G404: cli_command(229)     => dnf_cmd(285).
	p(NT(229), (NT(285)));
//G405: cnf_sym(290)         => 'c' 'n' 'f'.
	p(NT(290), (T(46)+T(47)+T(44)));
//G406: __E_cli_command_121(289) => cnf_sym(290) __(59) nf_cmd_arg(279).
	p(NT(289), (NT(290)+NT(59)+NT(279)));
//G407: cnf_cmd(288)         => __E_cli_command_121(289).
	p(NT(288), (NT(289)));
//G408: cli_command(229)     => cnf_cmd(288).
	p(NT(229), (NT(288)));
//G409: anf_sym(293)         => 'a' 'n' 'f'.
	p(NT(293), (T(17)+T(47)+T(44)));
//G410: __E_cli_command_122(292) => anf_sym(293) __(59) nf_cmd_arg(279).
	p(NT(292), (NT(293)+NT(59)+NT(279)));
//G411: anf_cmd(291)         => __E_cli_command_122(292).
	p(NT(291), (NT(292)));
//G412: cli_command(229)     => anf_cmd(291).
	p(NT(229), (NT(291)));
//G413: nnf_sym(296)         => 'n' 'n' 'f'.
	p(NT(296), (T(47)+T(47)+T(44)));
//G414: __E_cli_command_123(295) => nnf_sym(296) __(59) nf_cmd_arg(279).
	p(NT(295), (NT(296)+NT(59)+NT(279)));
//G415: nnf_cmd(294)         => __E_cli_command_123(295).
	p(NT(294), (NT(295)));
//G416: cli_command(229)     => nnf_cmd(294).
	p(NT(229), (NT(294)));
//G417: pnf_sym(299)         => 'p' 'n' 'f'.
	p(NT(299), (T(55)+T(47)+T(44)));
//G418: __E_cli_command_124(298) => pnf_sym(299) __(59) nf_cmd_arg(279).
	p(NT(298), (NT(299)+NT(59)+NT(279)));
//G419: pnf_cmd(297)         => __E_cli_command_124(298).
	p(NT(297), (NT(298)));
//G420: cli_command(229)     => pnf_cmd(297).
	p(NT(229), (NT(297)));
//G421: mnf_sym(302)         => 'm' 'n' 'f'.
	p(NT(302), (T(12)+T(47)+T(44)));
//G422: __E_cli_command_125(301) => mnf_sym(302) __(59) nf_cmd_arg(279).
	p(NT(301), (NT(302)+NT(59)+NT(279)));
//G423: mnf_cmd(300)         => __E_cli_command_125(301).
	p(NT(300), (NT(301)));
//G424: cli_command(229)     => mnf_cmd(300).
	p(NT(229), (NT(300)));
//G425: snf_sym(305)         => 's' 'n' 'f'.
	p(NT(305), (T(10)+T(47)+T(44)));
//G426: __E_cli_command_126(304) => snf_sym(305) __(59) nf_cmd_arg(279).
	p(NT(304), (NT(305)+NT(59)+NT(279)));
//G427: snf_cmd(303)         => __E_cli_command_126(304).
	p(NT(303), (NT(304)));
//G428: cli_command(229)     => snf_cmd(303).
	p(NT(229), (NT(303)));
//G429: onf_sym(308)         => 'o' 'n' 'f'.
	p(NT(308), (T(11)+T(47)+T(44)));
//G430: __E_cli_command_127(307) => onf_sym(308) __(59) variable(135) __(59) onf_cmd_arg(309).
	p(NT(307), (NT(308)+NT(59)+NT(135)+NT(59)+NT(309)));
//G431: onf_cmd(306)         => __E_cli_command_127(307).
	p(NT(306), (NT(307)));
//G432: cli_command(229)     => onf_cmd(306).
	p(NT(229), (NT(306)));
//G433: __E___E_cli_command_128_129(313) => 'd' 'e' 'f' 's'.
	p(NT(313), (T(54)+T(13)+T(44)+T(10)));
//G434: __E___E_cli_command_128_129(313) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(313), (T(54)+T(13)+T(44)+T(15)+T(47)+T(15)+T(14)+T(15)+T(11)+T(47)+T(10)));
//G435: def_sym(312)         => __E___E_cli_command_128_129(313).
	p(NT(312), (NT(313)));
//G436: __E_cli_command_128(311) => def_sym(312).
	p(NT(311), (NT(312)));
//G437: def_list_cmd(310)    => __E_cli_command_128(311).
	p(NT(310), (NT(311)));
//G438: cli_command(229)     => def_list_cmd(310).
	p(NT(229), (NT(310)));
//G439: __E_cli_command_130(315) => def_sym(312) __(59) number(316).
	p(NT(315), (NT(312)+NT(59)+NT(316)));
//G440: def_print_cmd(314)   => __E_cli_command_130(315).
	p(NT(314), (NT(315)));
//G441: cli_command(229)     => def_print_cmd(314).
	p(NT(229), (NT(314)));
//G442: def_rr_cmd(317)      => rec_relation(20).
	p(NT(317), (NT(20)));
//G443: cli_command(229)     => def_rr_cmd(317).
	p(NT(229), (NT(317)));
//G444: qelim_sym(320)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(320), (T(45)+T(13)+T(18)+T(15)+T(12)));
//G445: __E_cli_command_131(319) => qelim_sym(320) __(59) wff_cmd_arg(263).
	p(NT(319), (NT(320)+NT(59)+NT(263)));
//G446: qelim_cmd(318)       => __E_cli_command_131(319).
	p(NT(318), (NT(319)));
//G447: cli_command(229)     => qelim_cmd(318).
	p(NT(229), (NT(318)));
//G448: get_sym(323)         => 'g' 'e' 't'.
	p(NT(323), (T(49)+T(13)+T(14)));
//G449: __E___E_cli_command_132_133(324) => __(59) option(325).
	p(NT(324), (NT(59)+NT(325)));
//G450: __E___E_cli_command_132_133(324) => null.
	p(NT(324), (nul));
//G451: __E_cli_command_132(322) => get_sym(323) __E___E_cli_command_132_133(324).
	p(NT(322), (NT(323)+NT(324)));
//G452: get_cmd(321)         => __E_cli_command_132(322).
	p(NT(321), (NT(322)));
//G453: cli_command(229)     => get_cmd(321).
	p(NT(229), (NT(321)));
//G454: set_sym(328)         => 's' 'e' 't'.
	p(NT(328), (T(10)+T(13)+T(14)));
//G455: __E___E_cli_command_134_135(329) => __(59).
	p(NT(329), (NT(59)));
//G456: __E___E_cli_command_134_135(329) => _(15) '=' _(15).
	p(NT(329), (NT(15)+T(3)+NT(15)));
//G457: __E_cli_command_134(327) => set_sym(328) __(59) option(325) __E___E_cli_command_134_135(329) option_value(330).
	p(NT(327), (NT(328)+NT(59)+NT(325)+NT(329)+NT(330)));
//G458: set_cmd(326)         => __E_cli_command_134(327).
	p(NT(326), (NT(327)));
//G459: cli_command(229)     => set_cmd(326).
	p(NT(229), (NT(326)));
//G460: toggle_sym(333)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(333), (T(14)+T(11)+T(49)+T(49)+T(18)+T(13)));
//G461: __E_cli_command_136(332) => toggle_sym(333) __(59) bool_option(334).
	p(NT(332), (NT(333)+NT(59)+NT(334)));
//G462: toggle_cmd(331)      => __E_cli_command_136(332).
	p(NT(331), (NT(332)));
//G463: cli_command(229)     => toggle_cmd(331).
	p(NT(229), (NT(331)));
//G464: __E___E_cli_command_137_138(338) => 'h' 'i' 's' 't'.
	p(NT(338), (T(48)+T(15)+T(10)+T(14)));
//G465: __E___E_cli_command_137_138(338) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(338), (T(48)+T(15)+T(10)+T(14)+T(11)+T(51)+T(20)));
//G466: history_sym(337)     => __E___E_cli_command_137_138(338).
	p(NT(337), (NT(338)));
//G467: __E_cli_command_137(336) => history_sym(337).
	p(NT(336), (NT(337)));
//G468: history_list_cmd(335) => __E_cli_command_137(336).
	p(NT(335), (NT(336)));
//G469: cli_command(229)     => history_list_cmd(335).
	p(NT(229), (NT(335)));
//G470: __E_cli_command_139(340) => history_sym(337) __(59) memory(341).
	p(NT(340), (NT(337)+NT(59)+NT(341)));
//G471: history_print_cmd(339) => __E_cli_command_139(340).
	p(NT(339), (NT(340)));
//G472: cli_command(229)     => history_print_cmd(339).
	p(NT(229), (NT(339)));
//G473: __E_cli_command_140(343) => bf_typed(344).
	p(NT(343), (NT(344)));
//G474: __E_cli_command_140(343) => wff_typed(269).
	p(NT(343), (NT(269)));
//G475: history_store_cmd(342) => __E_cli_command_140(343).
	p(NT(342), (NT(343)));
//G476: cli_command(229)     => history_store_cmd(342).
	p(NT(229), (NT(342)));
//G477: number(316)          => digits(144).
	p(NT(316), (NT(144)));
//G478: bf_typed(344)        => bf(25).
	p(NT(344), (NT(25)));
//G479: bf_typed(344)        => bf(25) _(15) ':' _(15) 'b' 'f'.
	p(NT(344), (NT(25)+NT(15)+T(2)+NT(15)+T(24)+T(44)));
//G480: wff_typed(269)       => wff(18).
	p(NT(269), (NT(18)));
//G481: wff_typed(269)       => wff(18) _(15) ':' _(15) 'w' 'f' 'f'.
	p(NT(269), (NT(18)+NT(15)+T(2)+NT(15)+T(19)+T(44)+T(44)));
//G482: bf_cmd_arg(345)      => memory(341).
	p(NT(345), (NT(341)));
//G483: bf_cmd_arg(345)      => bf_typed(344).
	p(NT(345), (NT(344)));
//G484: wff_cmd_arg(263)     => memory(341).
	p(NT(263), (NT(341)));
//G485: wff_cmd_arg(263)     => wff_typed(269).
	p(NT(263), (NT(269)));
//G486: nf_cmd_arg(279)      => memory(341).
	p(NT(279), (NT(341)));
//G487: nf_cmd_arg(279)      => bf_typed(344).
	p(NT(279), (NT(344)));
//G488: nf_cmd_arg(279)      => wff_typed(269).
	p(NT(279), (NT(269)));
//G489: onf_cmd_arg(309)     => memory(341).
	p(NT(309), (NT(341)));
//G490: onf_cmd_arg(309)     => wff_typed(269).
	p(NT(309), (NT(269)));
//G491: normalize_cmd_arg(274) => memory(341).
	p(NT(274), (NT(341)));
//G492: normalize_cmd_arg(274) => rr(14).
	p(NT(274), (NT(14)));
//G493: normalize_cmd_arg(274) => ref(22).
	p(NT(274), (NT(22)));
//G494: normalize_cmd_arg(274) => bf_typed(344).
	p(NT(274), (NT(344)));
//G495: normalize_cmd_arg(274) => wff_typed(269).
	p(NT(274), (NT(269)));
//G496: inst_args(284)       => bf_cmd_arg(345) _(15) '[' _(15) variable(135) _(15) '/' _(15) bf_cmd_arg(345) _(15) ']'.
	p(NT(284), (NT(345)+NT(15)+T(21)+NT(15)+NT(135)+NT(15)+T(56)+NT(15)+NT(345)+NT(15)+T(22)));
//G497: inst_args(284)       => wff_cmd_arg(263) _(15) '[' _(15) variable(135) _(15) '/' _(15) bf_cmd_arg(345) _(15) ']'.
	p(NT(284), (NT(263)+NT(15)+T(21)+NT(15)+NT(135)+NT(15)+T(56)+NT(15)+NT(345)+NT(15)+T(22)));
//G498: inst_args(284)       => wff_cmd_arg(263) _(15) '[' _(15) bool_variable(114) _(15) '/' _(15) wff_cmd_arg(263) _(15) ']'.
	p(NT(284), (NT(263)+NT(15)+T(21)+NT(15)+NT(114)+NT(15)+T(56)+NT(15)+NT(263)+NT(15)+T(22)));
//G499: help_arg(246)        => help_sym(243).
	p(NT(246), (NT(243)));
//G500: help_arg(246)        => version_sym(236).
	p(NT(246), (NT(236)));
//G501: help_arg(246)        => quit_sym(233).
	p(NT(246), (NT(233)));
//G502: help_arg(246)        => clear_sym(239).
	p(NT(246), (NT(239)));
//G503: help_arg(246)        => get_sym(323).
	p(NT(246), (NT(323)));
//G504: help_arg(246)        => set_sym(328).
	p(NT(246), (NT(328)));
//G505: help_arg(246)        => toggle_sym(333).
	p(NT(246), (NT(333)));
//G506: help_arg(246)        => file_sym(249).
	p(NT(246), (NT(249)));
//G507: help_arg(246)        => history_sym(337).
	p(NT(246), (NT(337)));
//G508: help_arg(246)        => abs_memory_sym(346).
	p(NT(246), (NT(346)));
//G509: help_arg(246)        => rel_memory_sym(347).
	p(NT(246), (NT(347)));
//G510: help_arg(246)        => selection_sym(348).
	p(NT(246), (NT(348)));
//G511: help_arg(246)        => def_sym(312).
	p(NT(246), (NT(312)));
//G512: help_arg(246)        => inst_sym(282).
	p(NT(246), (NT(282)));
//G513: help_arg(246)        => subst_sym(277).
	p(NT(246), (NT(277)));
//G514: help_arg(246)        => normalize_sym(272).
	p(NT(246), (NT(272)));
//G515: help_arg(246)        => execute_sym(266).
	p(NT(246), (NT(266)));
//G516: help_arg(246)        => solve_sym(262).
	p(NT(246), (NT(262)));
//G517: help_arg(246)        => valid_sym(253).
	p(NT(246), (NT(253)));
//G518: help_arg(246)        => sat_sym(256).
	p(NT(246), (NT(256)));
//G519: help_arg(246)        => unsat_sym(259).
	p(NT(246), (NT(259)));
//G520: help_arg(246)        => dnf_sym(287).
	p(NT(246), (NT(287)));
//G521: help_arg(246)        => cnf_sym(290).
	p(NT(246), (NT(290)));
//G522: help_arg(246)        => anf_sym(293).
	p(NT(246), (NT(293)));
//G523: help_arg(246)        => snf_sym(305).
	p(NT(246), (NT(305)));
//G524: help_arg(246)        => nnf_sym(296).
	p(NT(246), (NT(296)));
//G525: help_arg(246)        => pnf_sym(299).
	p(NT(246), (NT(299)));
//G526: help_arg(246)        => mnf_sym(302).
	p(NT(246), (NT(302)));
//G527: help_arg(246)        => onf_sym(308).
	p(NT(246), (NT(308)));
//G528: help_arg(246)        => qelim_sym(320).
	p(NT(246), (NT(320)));
//G529: __E___E_help_arg_141_142(351) => 's'.
	p(NT(351), (T(10)));
//G530: __E___E_help_arg_141_142(351) => null.
	p(NT(351), (nul));
//G531: __E_help_arg_141(350) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_141_142(351).
	p(NT(350), (T(13)+T(26)+T(17)+T(12)+T(55)+T(18)+T(13)+NT(351)));
//G532: examples_sym(349)    => __E_help_arg_141(350).
	p(NT(349), (NT(350)));
//G533: help_arg(246)        => examples_sym(349).
	p(NT(246), (NT(349)));
//G534: __E_memory_143(353)  => '%'.
	p(NT(353), (T(57)));
//G535: rel_memory_sym(347)  => '%' '-'.
	p(NT(347), (T(57)+T(27)));
//G536: memory_id(355)       => digits(144).
	p(NT(355), (NT(144)));
//G537: __E___E_memory_143_144(354) => memory_id(355).
	p(NT(354), (NT(355)));
//G538: __E___E_memory_143_144(354) => null.
	p(NT(354), (nul));
//G539: __E_memory_143(353)  => rel_memory_sym(347) __E___E_memory_143_144(354).
	p(NT(353), (NT(347)+NT(354)));
//G540: rel_memory(352)      => __E_memory_143(353).
	p(NT(352), (NT(353)));
//G541: rel_memory(352)      => rel_memory(352).
	p(NT(352), (NT(352)));
//G542: memory(341)          => rel_memory(352).
	p(NT(341), (NT(352)));
//G543: abs_memory_sym(346)  => '%'.
	p(NT(346), (T(57)));
//G544: __E_memory_145(357)  => abs_memory_sym(346) memory_id(355).
	p(NT(357), (NT(346)+NT(355)));
//G545: abs_memory(356)      => __E_memory_145(357).
	p(NT(356), (NT(357)));
//G546: memory(341)          => abs_memory(356).
	p(NT(341), (NT(356)));
//G547: option(325)          => bool_option(334).
	p(NT(325), (NT(334)));
//G548: __E_option_146(360)  => 's' 'e' 'v'.
	p(NT(360), (T(10)+T(13)+T(52)));
//G549: __E_option_146(360)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(360), (T(10)+T(13)+T(52)+T(13)+T(51)+T(15)+T(14)+T(20)));
//G550: severity_opt(359)    => __E_option_146(360).
	p(NT(359), (NT(360)));
//G551: enum_option(358)     => severity_opt(359).
	p(NT(358), (NT(359)));
//G552: option(325)          => enum_option(358).
	p(NT(325), (NT(358)));
//G553: __E_bool_option_147(362) => 's'.
	p(NT(362), (T(10)));
//G554: __E_bool_option_147(362) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(362), (T(10)+T(14)+T(17)+T(14)+T(50)+T(10)));
//G555: status_opt(361)      => __E_bool_option_147(362).
	p(NT(361), (NT(362)));
//G556: bool_option(334)     => status_opt(361).
	p(NT(334), (NT(361)));
//G557: __E_bool_option_148(364) => 'c'.
	p(NT(364), (T(46)));
//G558: __E_bool_option_148(364) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(364), (T(46)+T(11)+T(18)+T(11)+T(51)));
//G559: __E_bool_option_148(364) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(364), (T(46)+T(11)+T(18)+T(11)+T(51)+T(10)));
//G560: colors_opt(363)      => __E_bool_option_148(364).
	p(NT(363), (NT(364)));
//G561: bool_option(334)     => colors_opt(363).
	p(NT(334), (NT(363)));
//G562: __E_bool_option_149(366) => 'd'.
	p(NT(366), (T(54)));
//G563: __E_bool_option_149(366) => 'd' 'b' 'g'.
	p(NT(366), (T(54)+T(24)+T(49)));
//G564: __E_bool_option_149(366) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(366), (T(54)+T(13)+T(24)+T(50)+T(49)));
//G565: debug_repl_opt(365)  => __E_bool_option_149(366).
	p(NT(365), (NT(366)));
//G566: bool_option(334)     => debug_repl_opt(365).
	p(NT(334), (NT(365)));
//G567: option_value(330)    => option_value_true(367).
	p(NT(330), (NT(367)));
//G568: option_value(330)    => option_value_false(368).
	p(NT(330), (NT(368)));
//G569: option_value(330)    => severity(369).
	p(NT(330), (NT(369)));
//G570: option_value_true(367) => 't'.
	p(NT(367), (T(14)));
//G571: option_value_true(367) => 't' 'r' 'u' 'e'.
	p(NT(367), (T(14)+T(51)+T(50)+T(13)));
//G572: option_value_true(367) => 'o' 'n'.
	p(NT(367), (T(11)+T(47)));
//G573: option_value_true(367) => '1'.
	p(NT(367), (T(37)));
//G574: option_value_true(367) => 'y'.
	p(NT(367), (T(20)));
//G575: option_value_true(367) => 'y' 'e' 's'.
	p(NT(367), (T(20)+T(13)+T(10)));
//G576: option_value_false(368) => 'f'.
	p(NT(368), (T(44)));
//G577: option_value_false(368) => 'f' 'a' 'l' 's' 'e'.
	p(NT(368), (T(44)+T(17)+T(18)+T(10)+T(13)));
//G578: option_value_false(368) => 'o' 'f' 'f'.
	p(NT(368), (T(11)+T(44)+T(44)));
//G579: option_value_false(368) => '0'.
	p(NT(368), (T(38)));
//G580: option_value_false(368) => 'n'.
	p(NT(368), (T(47)));
//G581: option_value_false(368) => 'n' 'o'.
	p(NT(368), (T(47)+T(11)));
//G582: __E_severity_150(371) => 'e'.
	p(NT(371), (T(13)));
//G583: __E_severity_150(371) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(371), (T(13)+T(51)+T(51)+T(11)+T(51)));
//G584: error_sym(370)       => __E_severity_150(371).
	p(NT(370), (NT(371)));
//G585: severity(369)        => error_sym(370).
	p(NT(369), (NT(370)));
//G586: __E_severity_151(373) => 'i'.
	p(NT(373), (T(15)));
//G587: __E_severity_151(373) => 'i' 'n' 'f' 'o'.
	p(NT(373), (T(15)+T(47)+T(44)+T(11)));
//G588: info_sym(372)        => __E_severity_151(373).
	p(NT(372), (NT(373)));
//G589: severity(369)        => info_sym(372).
	p(NT(369), (NT(372)));
//G590: __E_severity_152(375) => 'd'.
	p(NT(375), (T(54)));
//G591: __E_severity_152(375) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(375), (T(54)+T(13)+T(24)+T(50)+T(49)));
//G592: debug_sym(374)       => __E_severity_152(375).
	p(NT(374), (NT(375)));
//G593: severity(369)        => debug_sym(374).
	p(NT(369), (NT(374)));
//G594: __E_severity_153(377) => 't'.
	p(NT(377), (T(14)));
//G595: __E_severity_153(377) => 't' 'r' 'a' 'c' 'e'.
	p(NT(377), (T(14)+T(51)+T(17)+T(46)+T(13)));
//G596: trace_sym(376)       => __E_severity_153(377).
	p(NT(376), (NT(377)));
//G597: severity(369)        => trace_sym(376).
	p(NT(369), (NT(376)));
//G598: rule(43)             => tau_rule(379).
	p(NT(43), (NT(379)));
//G599: tau_matcher(380)     => tau(381).
	p(NT(380), (NT(381)));
//G600: __E_tau_rule_154(383) => tau(381).
	p(NT(383), (NT(381)));
//G601: __E_tau_rule_154(383) => tau_cb(378).
	p(NT(383), (NT(378)));
//G602: tau_body(382)        => __E_tau_rule_154(383).
	p(NT(382), (NT(383)));
//G603: tau_rule(379)        => tau_matcher(380) _(15) ':' ':' ':' '=' _(15) tau_body(382) _(15) '.'.
	p(NT(379), (NT(380)+NT(15)+T(2)+T(2)+T(2)+T(3)+NT(15)+NT(382)+NT(15)+T(1)));
//G604: __E_builder_body_155(385) => '=' ':' ':' ':' _(15) tau(381).
	p(NT(385), (T(3)+T(2)+T(2)+T(2)+NT(15)+NT(381)));
//G605: tau_builder_body(384) => __E_builder_body_155(385).
	p(NT(384), (NT(385)));
//G606: builder_body(57)     => tau_builder_body(384).
	p(NT(57), (NT(384)));
//G607: __E_tau_156(387)     => '(' _(15) tau(381) _(15) ')'.
	p(NT(387), (T(5)+NT(15)+NT(381)+NT(15)+T(6)));
//G608: tau_parenthesis(386) => __E_tau_156(387).
	p(NT(386), (NT(387)));
//G609: tau(381)             => tau_parenthesis(386).
	p(NT(381), (NT(386)));
//G610: __E_tau_157(389)     => '{' _(15) wff(18) _(15) '}'.
	p(NT(389), (T(8)+NT(15)+NT(18)+NT(15)+T(9)));
//G611: tau_wff(388)         => __E_tau_157(389).
	p(NT(388), (NT(389)));
//G612: tau(381)             => tau_wff(388).
	p(NT(381), (NT(388)));
//G613: __E___E_tau_158_159(392) => _(15) offsets(28).
	p(NT(392), (NT(15)+NT(28)));
//G614: __E___E_tau_158_159(392) => null.
	p(NT(392), (nul));
//G615: __E_tau_158(391)     => sym(26) __E___E_tau_158_159(392) _(15) ref_args(29).
	p(NT(391), (NT(26)+NT(392)+NT(15)+NT(29)));
//G616: tau_ref(390)         => __E_tau_158(391).
	p(NT(390), (NT(391)));
//G617: tau(381)             => tau_ref(390).
	p(NT(381), (NT(390)));
//G618: __E_tau_160(394)     => tau(381) _(15) '|' '|' '|' _(15) tau(381).
	p(NT(394), (NT(381)+NT(15)+T(28)+T(28)+T(28)+NT(15)+NT(381)));
//G619: tau_or(393)          => __E_tau_160(394).
	p(NT(393), (NT(394)));
//G620: tau(381)             => tau_or(393).
	p(NT(381), (NT(393)));
//G621: __E_tau_161(396)     => tau(381) _(15) '&' '&' '&' _(15) tau(381).
	p(NT(396), (NT(381)+NT(15)+T(30)+T(30)+T(30)+NT(15)+NT(381)));
//G622: tau_and(395)         => __E_tau_161(396).
	p(NT(395), (NT(396)));
//G623: tau(381)             => tau_and(395).
	p(NT(381), (NT(395)));
//G624: __E_tau_162(398)     => '-' _(15) tau(381).
	p(NT(398), (T(27)+NT(15)+NT(381)));
//G625: tau_neg(397)         => __E_tau_162(398).
	p(NT(397), (NT(398)));
//G626: tau(381)             => tau_neg(397).
	p(NT(381), (NT(397)));
//G627: tau(381)             => capture(24).
	p(NT(381), (NT(24)));
//G628: __E_tau_cb_args1_163(401) => capture(24).
	p(NT(401), (NT(24)));
//G629: __E_tau_cb_args1_163(401) => tau(381).
	p(NT(401), (NT(381)));
//G630: tau_cb_arg(400)      => __E_tau_cb_args1_163(401).
	p(NT(400), (NT(401)));
//G631: tau_cb_args1(399)    => __(59) tau_cb_arg(400).
	p(NT(399), (NT(59)+NT(400)));
//G632: tau_cb_args2(402)    => tau_cb_args1(399) tau_cb_args1(399).
	p(NT(402), (NT(399)+NT(399)));
//G633: tau_cb_args3(403)    => tau_cb_args2(402) tau_cb_args1(399).
	p(NT(403), (NT(402)+NT(399)));
//G634: __E___E_tau_cb_164_165(406) => tau_cb_args3(403).
	p(NT(406), (NT(403)));
//G635: __E___E_tau_cb_164_165(406) => tau_cb_args2(402).
	p(NT(406), (NT(402)));
//G636: __E_tau_cb_164(405)  => 't' 'a' 'u' '_' 'c' 'o' 'l' 'l' 'a' 'p' 's' 'e' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'c' 'b' __E___E_tau_cb_164_165(406).
	p(NT(405), (T(14)+T(17)+T(50)+T(25)+T(46)+T(11)+T(18)+T(18)+T(17)+T(55)+T(10)+T(13)+T(25)+T(55)+T(11)+T(10)+T(15)+T(14)+T(15)+T(52)+T(13)+T(10)+T(25)+T(46)+T(24)+NT(406)));
//G637: tau_collapse_positives_cb(404) => __E_tau_cb_164(405).
	p(NT(404), (NT(405)));
//G638: tau_cb(378)          => tau_collapse_positives_cb(404).
	p(NT(378), (NT(404)));
//G639: __E_tau_cb_166(408)  => 't' 'a' 'u' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'u' 'p' 'w' 'a' 'r' 'd' 's' '_' 'c' 'b' tau_cb_args2(402).
	p(NT(408), (T(14)+T(17)+T(50)+T(25)+T(55)+T(11)+T(10)+T(15)+T(14)+T(15)+T(52)+T(13)+T(10)+T(25)+T(50)+T(55)+T(19)+T(17)+T(51)+T(54)+T(10)+T(25)+T(46)+T(24)+NT(402)));
//G640: tau_positives_upwards_cb(407) => __E_tau_cb_166(408).
	p(NT(407), (NT(408)));
//G641: tau_cb(378)          => tau_positives_upwards_cb(407).
	p(NT(378), (NT(407)));
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
		wff_always, __E_wff_24, __E___E_wff_24_25, wff_conditional, __E_wff_26, wff_ball, __E_wff_27, q_bool_vars, wff_bex, __E_wff_28, 
		wff_all, __E_wff_29, q_vars, wff_ex, __E_wff_30, wff_ref, wff_imply, __E_wff_31, wff_equiv, __E_wff_32, 
		wff_or, __E_wff_33, wff_xor, __E_wff_34, wff_and, __E_wff_35, wff_neg, __E_wff_36, wff_t, wff_f, 
		bf_interval, __E_wff_37, bf_neq, __E_wff_38, bf_eq, __E_wff_39, bf_nleq, __E_wff_40, bf_greater, __E_wff_41, 
		bf_less_equal, __E_wff_42, bf_less, __E_wff_43, bool_variable, uninterpreted_constant, bf_parenthesis, __E_bf_44, bf_splitter, __E_bf_45, 
		bf_ref, bf_or, __E_bf_46, bf_xor, __E_bf_47, bf_and, __E_bf_48, __E___E_bf_48_49, bf_neg, __E_bf_50, 
		bf_constant, __E_bf_51, constant, bf_t, bf_f, variable, charvar, __E_charvar_52, __E_charvar_53, io_var, 
		__E_variable_54, out, in_var_name, __E_in_55, digits, offset, out_var_name, __E_out_56, q_bool_var, __E_q_bool_vars_57, 
		__E_q_bool_vars_58, q_var, __E_q_vars_59, __E_q_vars_60, __E_offsets_61, __E_offsets_62, num, shift, __E_offset_63, __E_shift_64, 
		__E___E_shift_64_65, uninter_const_name, __E_uninterpreted_constant_66, __E___E_uninterpreted_constant_66_67, chars, binding, __E_binding_68, type, __E___E_binding_68_69, source, 
		named_binding, src_c, __E_source_70, __E_source_71, __E___E_source_71_72, __E___E_source_71_73, __E_chars_74, __E_digits_75, comment, __E_comment_76, 
		__E_comment_77, __E_comment_78, __E____79, __E___80, bf_cb_args1, bf_cb_arg, bf_cb_args2, bf_cb_args3, bf_cb_args4, wff_cb_args1, 
		wff_cb_arg, wff_cb_args2, wff_cb_args3, wff_cb_args4, bf_eq_cb, __E_wff_cb_81, bf_neq_cb, __E_wff_cb_82, wff_has_clashing_subformulas_cb, __E_wff_cb_83, 
		wff_has_subformula_cb, __E_wff_cb_84, wff_remove_existential_cb, __E_wff_cb_85, wff_remove_bexistential_cb, __E_wff_cb_86, wff_remove_buniversal_cb, __E_wff_cb_87, bf_is_zero_cb, __E_bf_cb_88, 
		bf_is_one_cb, __E_bf_cb_89, bf_has_subformula_cb, __E_bf_cb_90, bf_remove_funiversal_cb, __E_bf_cb_91, bf_remove_fexistential_cb, __E_bf_cb_92, bf_and_cb, __E_bf_cb_93, 
		bf_or_cb, __E_bf_cb_94, bf_xor_cb, __E_bf_cb_95, bf_neg_cb, __E_bf_cb_96, bf_normalize_cb, __E_bf_cb_97, cli, cli_command, 
		__E_cli_98, __E_cli_99, quit_cmd, quit_sym, __E_cli_command_100, version_cmd, version_sym, __E_cli_command_101, clear_cmd, clear_sym, 
		__E_cli_command_102, help_cmd, __E_cli_command_103, help_sym, __E___E_cli_command_103_104, __E___E_cli_command_103_105, help_arg, file_cmd, __E_cli_command_106, file_sym, 
		q_string, valid_cmd, __E_cli_command_107, valid_sym, sat_cmd, __E_cli_command_108, sat_sym, unsat_cmd, __E_cli_command_109, unsat_sym, 
		solve_cmd, __E_cli_command_110, solve_sym, wff_cmd_arg, execute_cmd, __E_cli_command_111, execute_sym, __E___E_cli_command_111_112, __E___E_cli_command_111_113, wff_typed, 
		normalize_cmd, __E_cli_command_114, normalize_sym, __E___E_cli_command_114_115, normalize_cmd_arg, subst_cmd, __E_cli_command_116, subst_sym, __E___E_cli_command_116_117, nf_cmd_arg, 
		inst_cmd, __E_cli_command_118, inst_sym, __E___E_cli_command_118_119, inst_args, dnf_cmd, __E_cli_command_120, dnf_sym, cnf_cmd, __E_cli_command_121, 
		cnf_sym, anf_cmd, __E_cli_command_122, anf_sym, nnf_cmd, __E_cli_command_123, nnf_sym, pnf_cmd, __E_cli_command_124, pnf_sym, 
		mnf_cmd, __E_cli_command_125, mnf_sym, snf_cmd, __E_cli_command_126, snf_sym, onf_cmd, __E_cli_command_127, onf_sym, onf_cmd_arg, 
		def_list_cmd, __E_cli_command_128, def_sym, __E___E_cli_command_128_129, def_print_cmd, __E_cli_command_130, number, def_rr_cmd, qelim_cmd, __E_cli_command_131, 
		qelim_sym, get_cmd, __E_cli_command_132, get_sym, __E___E_cli_command_132_133, option, set_cmd, __E_cli_command_134, set_sym, __E___E_cli_command_134_135, 
		option_value, toggle_cmd, __E_cli_command_136, toggle_sym, bool_option, history_list_cmd, __E_cli_command_137, history_sym, __E___E_cli_command_137_138, history_print_cmd, 
		__E_cli_command_139, memory, history_store_cmd, __E_cli_command_140, bf_typed, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, examples_sym, 
		__E_help_arg_141, __E___E_help_arg_141_142, rel_memory, __E_memory_143, __E___E_memory_143_144, memory_id, abs_memory, __E_memory_145, enum_option, severity_opt, 
		__E_option_146, status_opt, __E_bool_option_147, colors_opt, __E_bool_option_148, debug_repl_opt, __E_bool_option_149, option_value_true, option_value_false, severity, 
		error_sym, __E_severity_150, info_sym, __E_severity_151, debug_sym, __E_severity_152, trace_sym, __E_severity_153, tau_cb, tau_rule, 
		tau_matcher, tau, tau_body, __E_tau_rule_154, tau_builder_body, __E_builder_body_155, tau_parenthesis, __E_tau_156, tau_wff, __E_tau_157, 
		tau_ref, __E_tau_158, __E___E_tau_158_159, tau_or, __E_tau_160, tau_and, __E_tau_161, tau_neg, __E_tau_162, tau_cb_args1, 
		tau_cb_arg, __E_tau_cb_args1_163, tau_cb_args2, tau_cb_args3, tau_collapse_positives_cb, __E_tau_cb_164, __E___E_tau_cb_164_165, tau_positives_upwards_cb, __E_tau_cb_166, __N_0, 
		__N_1, __N_2, __N_3, __N_4, __N_5, 
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
