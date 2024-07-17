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
	"bf_interval", "__E_wff_37", "bf_neq", "__E_wff_38", "bf_eq", "__E_wff_39", "bf_nleq_lower", "__E_wff_40", "bf_nleq_upper", "__E_wff_41", 
	"bf_greater", "__E_wff_42", "bf_less_equal", "__E_wff_43", "bf_less", "__E_wff_44", "bool_variable", "uninterpreted_constant", "bf_parenthesis", "__E_bf_45", 
	"bf_splitter", "__E_bf_46", "bf_ref", "bf_or", "__E_bf_47", "bf_xor", "__E_bf_48", "bf_and", "__E_bf_49", "__E___E_bf_49_50", 
	"bf_neg", "__E_bf_51", "bf_constant", "__E_bf_52", "constant", "bf_t", "bf_f", "variable", "charvar", "__E_charvar_53", 
	"__E_charvar_54", "io_var", "__E_variable_55", "out", "in_var_name", "__E_in_56", "digits", "offset", "out_var_name", "__E_out_57", 
	"__E_offsets_58", "__E_offsets_59", "num", "shift", "__E_offset_60", "__E_shift_61", "__E___E_shift_61_62", "uninter_const_name", "__E_uninterpreted_constant_63", "__E___E_uninterpreted_constant_63_64", 
	"chars", "binding", "__E_binding_65", "type", "__E___E_binding_65_66", "source", "named_binding", "src_c", "__E_source_67", "__E_source_68", 
	"__E___E_source_68_69", "__E___E_source_68_70", "__E_chars_71", "__E_digits_72", "comment", "__E_comment_73", "__E_comment_74", "__E_comment_75", "__E____76", "__E___77", 
	"bf_cb_args1", "bf_cb_arg", "bf_cb_args2", "bf_cb_args3", "bf_cb_args4", "wff_cb_args1", "wff_cb_arg", "wff_cb_args2", "wff_cb_args3", "wff_cb_args4", 
	"bf_eq_cb", "__E_wff_cb_78", "bf_neq_cb", "__E_wff_cb_79", "wff_has_clashing_subformulas_cb", "__E_wff_cb_80", "wff_has_subformula_cb", "__E_wff_cb_81", "wff_remove_existential_cb", "__E_wff_cb_82", 
	"wff_remove_bexistential_cb", "__E_wff_cb_83", "wff_remove_buniversal_cb", "__E_wff_cb_84", "bf_is_zero_cb", "__E_bf_cb_85", "bf_is_one_cb", "__E_bf_cb_86", "bf_has_subformula_cb", "__E_bf_cb_87", 
	"bf_remove_funiversal_cb", "__E_bf_cb_88", "bf_remove_fexistential_cb", "__E_bf_cb_89", "bf_and_cb", "__E_bf_cb_90", "bf_or_cb", "__E_bf_cb_91", "bf_xor_cb", "__E_bf_cb_92", 
	"bf_neg_cb", "__E_bf_cb_93", "bf_normalize_cb", "__E_bf_cb_94", "cli", "cli_command", "__E_cli_95", "__E_cli_96", "quit_cmd", "quit_sym", 
	"__E_cli_command_97", "version_cmd", "version_sym", "__E_cli_command_98", "clear_cmd", "clear_sym", "__E_cli_command_99", "help_cmd", "__E_cli_command_100", "help_sym", 
	"__E___E_cli_command_100_101", "__E___E_cli_command_100_102", "help_arg", "file_cmd", "__E_cli_command_103", "file_sym", "q_string", "valid_cmd", "__E_cli_command_104", "valid_sym", 
	"sat_cmd", "__E_cli_command_105", "sat_sym", "unsat_cmd", "__E_cli_command_106", "unsat_sym", "solve_cmd", "__E_cli_command_107", "solve_sym", "wff_cmd_arg", 
	"execute_cmd", "__E_cli_command_108", "execute_sym", "__E___E_cli_command_108_109", "__E___E_cli_command_108_110", "wff_typed", "normalize_cmd", "__E_cli_command_111", "normalize_sym", "__E___E_cli_command_111_112", 
	"normalize_cmd_arg", "subst_cmd", "__E_cli_command_113", "subst_sym", "__E___E_cli_command_113_114", "nf_cmd_arg", "inst_cmd", "__E_cli_command_115", "inst_sym", "__E___E_cli_command_115_116", 
	"inst_args", "dnf_cmd", "__E_cli_command_117", "dnf_sym", "cnf_cmd", "__E_cli_command_118", "cnf_sym", "anf_cmd", "__E_cli_command_119", "anf_sym", 
	"nnf_cmd", "__E_cli_command_120", "nnf_sym", "pnf_cmd", "__E_cli_command_121", "pnf_sym", "mnf_cmd", "__E_cli_command_122", "mnf_sym", "snf_cmd", 
	"__E_cli_command_123", "snf_sym", "onf_cmd", "__E_cli_command_124", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_125", "def_sym", "__E___E_cli_command_125_126", 
	"def_print_cmd", "__E_cli_command_127", "number", "def_rr_cmd", "history_list_cmd", "__E_cli_command_128", "history_sym", "__E___E_cli_command_128_129", "history_print_cmd", "__E_cli_command_130", 
	"memory", "history_store_cmd", "__E_cli_command_131", "bf_typed", "qelim_cmd", "__E_cli_command_132", "qelim_sym", "get_cmd", "__E_cli_command_133", "get_sym", 
	"__E___E_cli_command_133_134", "option", "set_cmd", "__E_cli_command_135", "set_sym", "__E___E_cli_command_135_136", "option_value", "toggle_cmd", "__E_cli_command_137", "toggle_sym", 
	"bool_option", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "examples_sym", "__E_help_arg_138", "__E___E_help_arg_138_139", "rel_memory", "__E_memory_140", 
	"__E___E_memory_140_141", "memory_id", "abs_memory", "__E_memory_142", "enum_option", "severity_opt", "__E_option_143", "status_opt", "__E_bool_option_144", "colors_opt", 
	"__E_bool_option_145", "debug_repl_opt", "__E_bool_option_146", "option_value_true", "option_value_false", "severity", "error_sym", "__E_severity_147", "info_sym", "__E_severity_148", 
	"debug_sym", "__E_severity_149", "trace_sym", "__E_severity_150", "tau_cb", "tau_rule", "tau_matcher", "tau", "tau_body", "__E_tau_rule_151", 
	"tau_builder_body", "__E_builder_body_152", "tau_parenthesis", "__E_tau_153", "tau_wff", "__E_tau_154", "tau_ref", "__E_tau_155", "__E___E_tau_155_156", "tau_or", 
	"__E_tau_157", "tau_and", "__E_tau_158", "tau_neg", "__E_tau_159", "tau_cb_args1", "tau_cb_arg", "__E_tau_cb_args1_160", "tau_cb_args2", "tau_cb_args3", 
	"tau_collapse_positives_cb", "__E_tau_cb_161", "__E___E_tau_cb_161_162", "tau_positives_upwards_cb", "__E_tau_cb_163", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", 
	"__N_5", 
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
			24, 26, 116, 138, 144, 146, 148, 157, 163, 165,
			166
		},
		.to_inline = {
			{ 18, 65, 18 },
			{ 25, 118, 25 },
			{ 51 },
			{ 55 },
			{ 77 },
			{ 82 },
			{ 160 },
			{ 167 },
			{ 180 },
			{ 182 },
			{ 183 },
			{ 184 },
			{ 185 },
			{ 187 },
			{ 188 },
			{ 189 },
			{ 259 },
			{ 265 },
			{ 270 },
			{ 275 },
			{ 280 },
			{ 305 },
			{ 323 },
			{ 341 },
			{ 374 },
			{ 377, 382, 377 },
			{ 395 },
			{ 398 },
			{ 399 }
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
//G114: __E_wff_37(101)      => bf(25) _(15) '<' '=' _(15) q_var(82) _(15) '<' '=' _(15) bf(25).
	p(NT(101), (NT(25)+NT(15)+T(7)+T(3)+NT(15)+NT(82)+NT(15)+T(7)+T(3)+NT(15)+NT(25)));
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
//G123: __E_wff_40(107)      => bf(25) _(15) '!' '<' '=' _(15) q_var(82).
	p(NT(107), (NT(25)+NT(15)+T(31)+T(7)+T(3)+NT(15)+NT(82)));
//G124: bf_nleq_lower(106)   => __E_wff_40(107).
	p(NT(106), (NT(107)));
//G125: wff(18)              => bf_nleq_lower(106).
	p(NT(18), (NT(106)));
//G126: __E_wff_41(109)      => q_var(82) _(15) '!' '<' '=' _(15) bf(25).
	p(NT(109), (NT(82)+NT(15)+T(31)+T(7)+T(3)+NT(15)+NT(25)));
//G127: bf_nleq_upper(108)   => __E_wff_41(109).
	p(NT(108), (NT(109)));
//G128: wff(18)              => bf_nleq_upper(108).
	p(NT(18), (NT(108)));
//G129: __E_wff_42(111)      => bf(25) _(15) '>' _(15) bf(25).
	p(NT(111), (NT(25)+NT(15)+T(16)+NT(15)+NT(25)));
//G130: bf_greater(110)      => __E_wff_42(111).
	p(NT(110), (NT(111)));
//G131: wff(18)              => bf_greater(110).
	p(NT(18), (NT(110)));
//G132: __E_wff_43(113)      => bf(25) _(15) '<' '=' _(15) bf(25).
	p(NT(113), (NT(25)+NT(15)+T(7)+T(3)+NT(15)+NT(25)));
//G133: bf_less_equal(112)   => __E_wff_43(113).
	p(NT(112), (NT(113)));
//G134: wff(18)              => bf_less_equal(112).
	p(NT(18), (NT(112)));
//G135: __E_wff_44(115)      => bf(25) _(15) '<' _(15) bf(25).
	p(NT(115), (NT(25)+NT(15)+T(7)+NT(15)+NT(25)));
//G136: bf_less(114)         => __E_wff_44(115).
	p(NT(114), (NT(115)));
//G137: wff(18)              => bf_less(114).
	p(NT(18), (NT(114)));
//G138: wff(18)              => capture(24).
	p(NT(18), (NT(24)));
//G139: wff(18)              => bool_variable(116).
	p(NT(18), (NT(116)));
//G140: wff(18)              => uninterpreted_constant(117).
	p(NT(18), (NT(117)));
//G141: __E_bf_45(119)       => '(' _(15) bf(25) _(15) ')'.
	p(NT(119), (T(5)+NT(15)+NT(25)+NT(15)+T(6)));
//G142: bf_parenthesis(118)  => __E_bf_45(119).
	p(NT(118), (NT(119)));
//G143: bf(25)               => bf_parenthesis(118).
	p(NT(25), (NT(118)));
//G144: __E_bf_46(121)       => 'S' _(15) '(' _(15) bf(25) _(15) ')'.
	p(NT(121), (T(34)+NT(15)+T(5)+NT(15)+NT(25)+NT(15)+T(6)));
//G145: bf_splitter(120)     => __E_bf_46(121).
	p(NT(120), (NT(121)));
//G146: bf(25)               => bf_splitter(120).
	p(NT(25), (NT(120)));
//G147: bf_ref(122)          => ref(22).
	p(NT(122), (NT(22)));
//G148: bf(25)               => bf_ref(122).
	p(NT(25), (NT(122)));
//G149: __E_bf_47(124)       => bf(25) _(15) '|' _(15) bf(25).
	p(NT(124), (NT(25)+NT(15)+T(28)+NT(15)+NT(25)));
//G150: bf_or(123)           => __E_bf_47(124).
	p(NT(123), (NT(124)));
//G151: bf(25)               => bf_or(123).
	p(NT(25), (NT(123)));
//G152: __E_bf_48(126)       => bf(25) _(15) '+' _(15) bf(25).
	p(NT(126), (NT(25)+NT(15)+T(35)+NT(15)+NT(25)));
//G153: bf_xor(125)          => __E_bf_48(126).
	p(NT(125), (NT(126)));
//G154: bf(25)               => bf_xor(125).
	p(NT(25), (NT(125)));
//G155: __E___E_bf_49_50(129) => _(15).
	p(NT(129), (NT(15)));
//G156: __E___E_bf_49_50(129) => _(15) '&' _(15).
	p(NT(129), (NT(15)+T(30)+NT(15)));
//G157: __E_bf_49(128)       => bf(25) __E___E_bf_49_50(129) bf(25).
	p(NT(128), (NT(25)+NT(129)+NT(25)));
//G158: bf_and(127)          => __E_bf_49(128).
	p(NT(127), (NT(128)));
//G159: bf(25)               => bf_and(127).
	p(NT(25), (NT(127)));
//G160: __E_bf_51(131)       => bf(25) _(15) '\''.
	p(NT(131), (NT(25)+NT(15)+T(36)));
//G161: bf_neg(130)          => __E_bf_51(131).
	p(NT(130), (NT(131)));
//G162: bf(25)               => bf_neg(130).
	p(NT(25), (NT(130)));
//G163: __E_bf_52(133)       => '{' _(15) constant(134) _(15) '}'.
	p(NT(133), (T(8)+NT(15)+NT(134)+NT(15)+T(9)));
//G164: bf_constant(132)     => __E_bf_52(133).
	p(NT(132), (NT(133)));
//G165: bf(25)               => bf_constant(132).
	p(NT(25), (NT(132)));
//G166: bf_t(135)            => '1'.
	p(NT(135), (T(37)));
//G167: bf(25)               => bf_t(135).
	p(NT(25), (NT(135)));
//G168: bf_f(136)            => '0'.
	p(NT(136), (T(38)));
//G169: bf(25)               => bf_f(136).
	p(NT(25), (NT(136)));
//G170: bf(25)               => capture(24).
	p(NT(25), (NT(24)));
//G171: bf(25)               => variable(137).
	p(NT(25), (NT(137)));
//G172: bf(25)               => uninterpreted_constant(117).
	p(NT(25), (NT(117)));
//G173: __N_0(405)           => 'F'.
	p(NT(405), (T(33)));
//G174: __N_1(406)           => 'T'.
	p(NT(406), (T(32)));
//G175: __N_2(407)           => 'i'.
	p(NT(407), (T(15)));
//G176: __N_3(408)           => 'o'.
	p(NT(408), (T(11)));
//G177: __E_charvar_53(139)  => ~( __N_0(405) ) & ~( __N_1(406) ) & ~( __N_2(407) ) & ~( __N_3(408) ) & alpha(5).	 # conjunctive
	p(NT(139), ~(NT(405)) & ~(NT(406)) & ~(NT(407)) & ~(NT(408)) & (NT(5)));
//G178: __E_charvar_54(140)  => null.
	p(NT(140), (nul));
//G179: __E_charvar_54(140)  => digit(3) __E_charvar_54(140).
	p(NT(140), (NT(3)+NT(140)));
//G180: charvar(138)         => __E_charvar_53(139) __E_charvar_54(140).
	p(NT(138), (NT(139)+NT(140)));
//G181: bool_variable(116)   => '?' charvar(138).
	p(NT(116), (T(23)+NT(138)));
//G182: variable(137)        => charvar(138).
	p(NT(137), (NT(138)));
//G183: __E_variable_55(142) => in(38).
	p(NT(142), (NT(38)));
//G184: __E_variable_55(142) => out(143).
	p(NT(142), (NT(143)));
//G185: io_var(141)          => __E_variable_55(142).
	p(NT(141), (NT(142)));
//G186: variable(137)        => io_var(141).
	p(NT(137), (NT(141)));
//G187: __E_in_56(145)       => 'i' digits(146).
	p(NT(145), (T(15)+NT(146)));
//G188: in_var_name(144)     => __E_in_56(145).
	p(NT(144), (NT(145)));
//G189: in(38)               => in_var_name(144) '[' offset(147) ']'.
	p(NT(38), (NT(144)+T(21)+NT(147)+T(22)));
//G190: __E_out_57(149)      => 'o' digits(146).
	p(NT(149), (T(11)+NT(146)));
//G191: out_var_name(148)    => __E_out_57(149).
	p(NT(148), (NT(149)));
//G192: out(143)             => out_var_name(148) '[' offset(147) ']'.
	p(NT(143), (NT(148)+T(21)+NT(147)+T(22)));
//G193: capture(24)          => '$' charvar(138).
	p(NT(24), (T(39)+NT(138)));
//G194: q_bool_var(77)       => capture(24).
	p(NT(77), (NT(24)));
//G195: q_bool_var(77)       => bool_variable(116).
	p(NT(77), (NT(116)));
//G196: q_var(82)            => capture(24).
	p(NT(82), (NT(24)));
//G197: q_var(82)            => variable(137).
	p(NT(82), (NT(137)));
//G198: __E_offsets_58(150)  => _(15) ',' _(15) offset(147).
	p(NT(150), (NT(15)+T(4)+NT(15)+NT(147)));
//G199: __E_offsets_59(151)  => null.
	p(NT(151), (nul));
//G200: __E_offsets_59(151)  => __E_offsets_58(150) __E_offsets_59(151).
	p(NT(151), (NT(150)+NT(151)));
//G201: offsets(28)          => '[' _(15) offset(147) __E_offsets_59(151) _(15) ']'.
	p(NT(28), (T(21)+NT(15)+NT(147)+NT(151)+NT(15)+T(22)));
//G202: offset(147)          => num(152).
	p(NT(147), (NT(152)));
//G203: offset(147)          => capture(24).
	p(NT(147), (NT(24)));
//G204: offset(147)          => shift(153).
	p(NT(147), (NT(153)));
//G205: __N_4(409)           => io_var(141).
	p(NT(409), (NT(141)));
//G206: __E_offset_60(154)   => variable(137) & ~( __N_4(409) ).	 # conjunctive
	p(NT(154), (NT(137)) & ~(NT(409)));
//G207: offset(147)          => __E_offset_60(154).
	p(NT(147), (NT(154)));
//G208: __E_shift_61(155)    => capture(24).
	p(NT(155), (NT(24)));
//G209: __N_5(410)           => io_var(141).
	p(NT(410), (NT(141)));
//G210: __E___E_shift_61_62(156) => variable(137) & ~( __N_5(410) ).	 # conjunctive
	p(NT(156), (NT(137)) & ~(NT(410)));
//G211: __E_shift_61(155)    => __E___E_shift_61_62(156).
	p(NT(155), (NT(156)));
//G212: shift(153)           => __E_shift_61(155) _(15) '-' _(15) num(152).
	p(NT(153), (NT(155)+NT(15)+T(27)+NT(15)+NT(152)));
//G213: num(152)             => digits(146).
	p(NT(152), (NT(146)));
//G214: __E___E_uninterpreted_constant_63_64(159) => chars(160).
	p(NT(159), (NT(160)));
//G215: __E___E_uninterpreted_constant_63_64(159) => _(15).
	p(NT(159), (NT(15)));
//G216: __E_uninterpreted_constant_63(158) => __E___E_uninterpreted_constant_63_64(159) ':' chars(160).
	p(NT(158), (NT(159)+T(2)+NT(160)));
//G217: uninter_const_name(157) => __E_uninterpreted_constant_63(158).
	p(NT(157), (NT(158)));
//G218: uninterpreted_constant(117) => '<' uninter_const_name(157) _(15) '>'.
	p(NT(117), (T(7)+NT(157)+NT(15)+T(16)));
//G219: constant(134)        => binding(161).
	p(NT(134), (NT(161)));
//G220: constant(134)        => capture(24).
	p(NT(134), (NT(24)));
//G221: __E___E_binding_65_66(164) => chars(160) _(15).
	p(NT(164), (NT(160)+NT(15)));
//G222: __E___E_binding_65_66(164) => _(15).
	p(NT(164), (NT(15)));
//G223: type(163)            => __E___E_binding_65_66(164).
	p(NT(163), (NT(164)));
//G224: __E_binding_65(162)  => type(163) ':' _(15) source(165).
	p(NT(162), (NT(163)+T(2)+NT(15)+NT(165)));
//G225: source_binding(39)   => __E_binding_65(162).
	p(NT(39), (NT(162)));
//G226: binding(161)         => source_binding(39).
	p(NT(161), (NT(39)));
//G227: named_binding(166)   => chars(160).
	p(NT(166), (NT(160)));
//G228: binding(161)         => named_binding(166).
	p(NT(161), (NT(166)));
//G229: __E_source_67(168)   => alnum(6).
	p(NT(168), (NT(6)));
//G230: __E_source_67(168)   => punct(7).
	p(NT(168), (NT(7)));
//G231: src_c(167)           => __E_source_67(168).
	p(NT(167), (NT(168)));
//G232: __E___E_source_68_69(170) => src_c(167).
	p(NT(170), (NT(167)));
//G233: __E___E_source_68_69(170) => space(2).
	p(NT(170), (NT(2)));
//G234: __E___E_source_68_70(171) => null.
	p(NT(171), (nul));
//G235: __E___E_source_68_70(171) => __E___E_source_68_69(170) __E___E_source_68_70(171).
	p(NT(171), (NT(170)+NT(171)));
//G236: __E_source_68(169)   => __E___E_source_68_70(171) src_c(167).
	p(NT(169), (NT(171)+NT(167)));
//G237: __E_source_68(169)   => null.
	p(NT(169), (nul));
//G238: source(165)          => src_c(167) __E_source_68(169).
	p(NT(165), (NT(167)+NT(169)));
//G239: __E_chars_71(172)    => null.
	p(NT(172), (nul));
//G240: __E_chars_71(172)    => alnum(6) __E_chars_71(172).
	p(NT(172), (NT(6)+NT(172)));
//G241: chars(160)           => alpha(5) __E_chars_71(172).
	p(NT(160), (NT(5)+NT(172)));
//G242: __E_digits_72(173)   => digit(3).
	p(NT(173), (NT(3)));
//G243: __E_digits_72(173)   => digit(3) __E_digits_72(173).
	p(NT(173), (NT(3)+NT(173)));
//G244: digits(146)          => __E_digits_72(173).
	p(NT(146), (NT(173)));
//G245: sym(26)              => chars(160).
	p(NT(26), (NT(160)));
//G246: __E_comment_73(175)  => printable(8).
	p(NT(175), (NT(8)));
//G247: __E_comment_73(175)  => '\t'.
	p(NT(175), (T(40)));
//G248: __E_comment_74(176)  => null.
	p(NT(176), (nul));
//G249: __E_comment_74(176)  => __E_comment_73(175) __E_comment_74(176).
	p(NT(176), (NT(175)+NT(176)));
//G250: __E_comment_75(177)  => '\n'.
	p(NT(177), (T(41)));
//G251: __E_comment_75(177)  => '\r'.
	p(NT(177), (T(42)));
//G252: __E_comment_75(177)  => eof(1).
	p(NT(177), (NT(1)));
//G253: comment(174)         => '#' __E_comment_74(176) __E_comment_75(177).
	p(NT(174), (T(43)+NT(176)+NT(177)));
//G254: __E____76(178)       => space(2).
	p(NT(178), (NT(2)));
//G255: __E____76(178)       => comment(174).
	p(NT(178), (NT(174)));
//G256: __(59)               => __E____76(178) _(15).
	p(NT(59), (NT(178)+NT(15)));
//G257: __E___77(179)        => __(59).
	p(NT(179), (NT(59)));
//G258: __E___77(179)        => null.
	p(NT(179), (nul));
//G259: _(15)                => __E___77(179).
	p(NT(15), (NT(179)));
//G260: bf_cb_arg(181)       => bf(25).
	p(NT(181), (NT(25)));
//G261: bf_cb_args1(180)     => __(59) bf_cb_arg(181).
	p(NT(180), (NT(59)+NT(181)));
//G262: bf_cb_args2(182)     => bf_cb_args1(180) bf_cb_args1(180).
	p(NT(182), (NT(180)+NT(180)));
//G263: bf_cb_args3(183)     => bf_cb_args2(182) bf_cb_args1(180).
	p(NT(183), (NT(182)+NT(180)));
//G264: bf_cb_args4(184)     => bf_cb_args3(183) bf_cb_args1(180).
	p(NT(184), (NT(183)+NT(180)));
//G265: wff_cb_arg(186)      => wff(18).
	p(NT(186), (NT(18)));
//G266: wff_cb_args1(185)    => __(59) wff_cb_arg(186).
	p(NT(185), (NT(59)+NT(186)));
//G267: wff_cb_args2(187)    => wff_cb_args1(185) wff_cb_args1(185).
	p(NT(187), (NT(185)+NT(185)));
//G268: wff_cb_args3(188)    => wff_cb_args2(187) wff_cb_args1(185).
	p(NT(188), (NT(187)+NT(185)));
//G269: wff_cb_args4(189)    => wff_cb_args3(188) wff_cb_args1(185).
	p(NT(189), (NT(188)+NT(185)));
//G270: __E_wff_cb_78(191)   => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b' bf_cb_args1(180) wff_cb_args2(187).
	p(NT(191), (T(24)+T(44)+T(25)+T(13)+T(45)+T(25)+T(46)+T(24)+NT(180)+NT(187)));
//G271: bf_eq_cb(190)        => __E_wff_cb_78(191).
	p(NT(190), (NT(191)));
//G272: wff_cb(51)           => bf_eq_cb(190).
	p(NT(51), (NT(190)));
//G273: __E_wff_cb_79(193)   => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b' bf_cb_args1(180) wff_cb_args2(187).
	p(NT(193), (T(24)+T(44)+T(25)+T(47)+T(13)+T(45)+T(25)+T(46)+T(24)+NT(180)+NT(187)));
//G274: bf_neq_cb(192)       => __E_wff_cb_79(193).
	p(NT(192), (NT(193)));
//G275: wff_cb(51)           => bf_neq_cb(192).
	p(NT(51), (NT(192)));
//G276: __E_wff_cb_80(195)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(187).
	p(NT(195), (T(19)+T(44)+T(44)+T(25)+T(48)+T(17)+T(10)+T(25)+T(46)+T(18)+T(17)+T(10)+T(48)+T(15)+T(47)+T(49)+T(25)+T(10)+T(50)+T(24)+T(44)+T(11)+T(51)+T(12)+T(50)+T(18)+T(17)+T(10)+T(25)+T(46)+T(24)+NT(187)));
//G277: wff_has_clashing_subformulas_cb(194) => __E_wff_cb_80(195).
	p(NT(194), (NT(195)));
//G278: wff_cb(51)           => wff_has_clashing_subformulas_cb(194).
	p(NT(51), (NT(194)));
//G279: __E_wff_cb_81(197)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(188).
	p(NT(197), (T(19)+T(44)+T(44)+T(25)+T(48)+T(17)+T(10)+T(25)+T(10)+T(50)+T(24)+T(44)+T(11)+T(51)+T(12)+T(50)+T(18)+T(17)+T(25)+T(46)+T(24)+NT(188)));
//G280: wff_has_subformula_cb(196) => __E_wff_cb_81(197).
	p(NT(196), (NT(197)));
//G281: wff_cb(51)           => wff_has_subformula_cb(196).
	p(NT(51), (NT(196)));
//G282: __E_wff_cb_82(199)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(187).
	p(NT(199), (T(19)+T(44)+T(44)+T(25)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(25)+T(13)+T(26)+T(15)+T(10)+T(14)+T(13)+T(47)+T(14)+T(15)+T(17)+T(18)+T(25)+T(46)+T(24)+NT(187)));
//G283: wff_remove_existential_cb(198) => __E_wff_cb_82(199).
	p(NT(198), (NT(199)));
//G284: wff_cb(51)           => wff_remove_existential_cb(198).
	p(NT(51), (NT(198)));
//G285: __E_wff_cb_83(201)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args4(189).
	p(NT(201), (T(19)+T(44)+T(44)+T(25)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(25)+T(24)+T(13)+T(26)+T(15)+T(10)+T(14)+T(13)+T(47)+T(14)+T(15)+T(17)+T(18)+T(25)+T(46)+T(24)+NT(189)));
//G286: wff_remove_bexistential_cb(200) => __E_wff_cb_83(201).
	p(NT(200), (NT(201)));
//G287: wff_cb(51)           => wff_remove_bexistential_cb(200).
	p(NT(51), (NT(200)));
//G288: __E_wff_cb_84(203)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' wff_cb_args4(189).
	p(NT(203), (T(19)+T(44)+T(44)+T(25)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(25)+T(24)+T(50)+T(47)+T(15)+T(52)+T(13)+T(51)+T(10)+T(17)+T(18)+T(25)+T(46)+T(24)+NT(189)));
//G289: wff_remove_buniversal_cb(202) => __E_wff_cb_84(203).
	p(NT(202), (NT(203)));
//G290: wff_cb(51)           => wff_remove_buniversal_cb(202).
	p(NT(51), (NT(202)));
//G291: __E_bf_cb_85(205)    => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b' bf_cb_args2(182).
	p(NT(205), (T(24)+T(44)+T(25)+T(15)+T(10)+T(25)+T(53)+T(13)+T(51)+T(11)+T(25)+T(46)+T(24)+NT(182)));
//G292: bf_is_zero_cb(204)   => __E_bf_cb_85(205).
	p(NT(204), (NT(205)));
//G293: bf_cb(55)            => bf_is_zero_cb(204).
	p(NT(55), (NT(204)));
//G294: __E_bf_cb_86(207)    => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b' bf_cb_args2(182).
	p(NT(207), (T(24)+T(44)+T(25)+T(15)+T(10)+T(25)+T(11)+T(47)+T(13)+T(25)+T(46)+T(24)+NT(182)));
//G295: bf_is_one_cb(206)    => __E_bf_cb_86(207).
	p(NT(206), (NT(207)));
//G296: bf_cb(55)            => bf_is_one_cb(206).
	p(NT(55), (NT(206)));
//G297: __E_bf_cb_87(209)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(183).
	p(NT(209), (T(24)+T(44)+T(25)+T(48)+T(17)+T(10)+T(25)+T(10)+T(50)+T(24)+T(44)+T(11)+T(51)+T(12)+T(50)+T(18)+T(17)+T(25)+T(46)+T(24)+NT(183)));
//G298: bf_has_subformula_cb(208) => __E_bf_cb_87(209).
	p(NT(208), (NT(209)));
//G299: bf_cb(55)            => bf_has_subformula_cb(208).
	p(NT(55), (NT(208)));
//G300: __E_bf_cb_88(211)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(184).
	p(NT(211), (T(24)+T(44)+T(25)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(25)+T(44)+T(50)+T(47)+T(15)+T(52)+T(13)+T(51)+T(10)+T(17)+T(18)+T(25)+T(46)+T(24)+NT(184)));
//G301: bf_remove_funiversal_cb(210) => __E_bf_cb_88(211).
	p(NT(210), (NT(211)));
//G302: bf_cb(55)            => bf_remove_funiversal_cb(210).
	p(NT(55), (NT(210)));
//G303: __E_bf_cb_89(213)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(184).
	p(NT(213), (T(24)+T(44)+T(25)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(25)+T(44)+T(13)+T(26)+T(15)+T(10)+T(14)+T(13)+T(47)+T(14)+T(15)+T(17)+T(18)+T(25)+T(46)+T(24)+NT(184)));
//G304: bf_remove_fexistential_cb(212) => __E_bf_cb_89(213).
	p(NT(212), (NT(213)));
//G305: bf_cb(55)            => bf_remove_fexistential_cb(212).
	p(NT(55), (NT(212)));
//G306: __E_bf_cb_90(215)    => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b' bf_cb_args2(182).
	p(NT(215), (T(24)+T(44)+T(25)+T(17)+T(47)+T(54)+T(25)+T(46)+T(24)+NT(182)));
//G307: bf_and_cb(214)       => __E_bf_cb_90(215).
	p(NT(214), (NT(215)));
//G308: bf_cb(55)            => bf_and_cb(214).
	p(NT(55), (NT(214)));
//G309: __E_bf_cb_91(217)    => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b' bf_cb_args2(182).
	p(NT(217), (T(24)+T(44)+T(25)+T(11)+T(51)+T(25)+T(46)+T(24)+NT(182)));
//G310: bf_or_cb(216)        => __E_bf_cb_91(217).
	p(NT(216), (NT(217)));
//G311: bf_cb(55)            => bf_or_cb(216).
	p(NT(55), (NT(216)));
//G312: __E_bf_cb_92(219)    => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b' bf_cb_args2(182).
	p(NT(219), (T(24)+T(44)+T(25)+T(26)+T(11)+T(51)+T(25)+T(46)+T(24)+NT(182)));
//G313: bf_xor_cb(218)       => __E_bf_cb_92(219).
	p(NT(218), (NT(219)));
//G314: bf_cb(55)            => bf_xor_cb(218).
	p(NT(55), (NT(218)));
//G315: __E_bf_cb_93(221)    => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b' bf_cb_args1(180).
	p(NT(221), (T(24)+T(44)+T(25)+T(47)+T(13)+T(49)+T(25)+T(46)+T(24)+NT(180)));
//G316: bf_neg_cb(220)       => __E_bf_cb_93(221).
	p(NT(220), (NT(221)));
//G317: bf_cb(55)            => bf_neg_cb(220).
	p(NT(55), (NT(220)));
//G318: __E_bf_cb_94(223)    => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(180).
	p(NT(223), (T(24)+T(44)+T(25)+T(47)+T(11)+T(51)+T(12)+T(17)+T(18)+T(15)+T(53)+T(13)+T(25)+T(46)+T(24)+NT(180)));
//G319: bf_normalize_cb(222) => __E_bf_cb_94(223).
	p(NT(222), (NT(223)));
//G320: bf_cb(55)            => bf_normalize_cb(222).
	p(NT(55), (NT(222)));
//G321: cli(224)             => _(15).
	p(NT(224), (NT(15)));
//G322: __E_cli_95(226)      => '.' _(15) cli_command(225) _(15).
	p(NT(226), (T(1)+NT(15)+NT(225)+NT(15)));
//G323: __E_cli_96(227)      => null.
	p(NT(227), (nul));
//G324: __E_cli_96(227)      => __E_cli_95(226) __E_cli_96(227).
	p(NT(227), (NT(226)+NT(227)));
//G325: cli(224)             => _(15) cli_command(225) _(15) __E_cli_96(227).
	p(NT(224), (NT(15)+NT(225)+NT(15)+NT(227)));
//G326: __E_cli_command_97(230) => 'q'.
	p(NT(230), (T(45)));
//G327: __E_cli_command_97(230) => 'q' 'u' 'i' 't'.
	p(NT(230), (T(45)+T(50)+T(15)+T(14)));
//G328: quit_sym(229)        => __E_cli_command_97(230).
	p(NT(229), (NT(230)));
//G329: quit_cmd(228)        => quit_sym(229).
	p(NT(228), (NT(229)));
//G330: cli_command(225)     => quit_cmd(228).
	p(NT(225), (NT(228)));
//G331: __E_cli_command_98(233) => 'v'.
	p(NT(233), (T(52)));
//G332: __E_cli_command_98(233) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(233), (T(52)+T(13)+T(51)+T(10)+T(15)+T(11)+T(47)));
//G333: version_sym(232)     => __E_cli_command_98(233).
	p(NT(232), (NT(233)));
//G334: version_cmd(231)     => version_sym(232).
	p(NT(231), (NT(232)));
//G335: cli_command(225)     => version_cmd(231).
	p(NT(225), (NT(231)));
//G336: __E_cli_command_99(236) => 'c'.
	p(NT(236), (T(46)));
//G337: __E_cli_command_99(236) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(236), (T(46)+T(18)+T(13)+T(17)+T(51)));
//G338: clear_sym(235)       => __E_cli_command_99(236).
	p(NT(235), (NT(236)));
//G339: clear_cmd(234)       => clear_sym(235).
	p(NT(234), (NT(235)));
//G340: cli_command(225)     => clear_cmd(234).
	p(NT(225), (NT(234)));
//G341: __E___E_cli_command_100_101(240) => 'h'.
	p(NT(240), (T(48)));
//G342: __E___E_cli_command_100_101(240) => 'h' 'e' 'l' 'p'.
	p(NT(240), (T(48)+T(13)+T(18)+T(55)));
//G343: help_sym(239)        => __E___E_cli_command_100_101(240).
	p(NT(239), (NT(240)));
//G344: __E___E_cli_command_100_102(241) => __(59) help_arg(242).
	p(NT(241), (NT(59)+NT(242)));
//G345: __E___E_cli_command_100_102(241) => null.
	p(NT(241), (nul));
//G346: __E_cli_command_100(238) => help_sym(239) __E___E_cli_command_100_102(241).
	p(NT(238), (NT(239)+NT(241)));
//G347: help_cmd(237)        => __E_cli_command_100(238).
	p(NT(237), (NT(238)));
//G348: cli_command(225)     => help_cmd(237).
	p(NT(225), (NT(237)));
//G349: file_sym(245)        => 'f' 'i' 'l' 'e'.
	p(NT(245), (T(44)+T(15)+T(18)+T(13)));
//G350: __E_cli_command_103(244) => file_sym(245) __(59) q_string(246).
	p(NT(244), (NT(245)+NT(59)+NT(246)));
//G351: file_cmd(243)        => __E_cli_command_103(244).
	p(NT(243), (NT(244)));
//G352: cli_command(225)     => file_cmd(243).
	p(NT(225), (NT(243)));
//G353: valid_sym(249)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(249), (T(52)+T(17)+T(18)+T(15)+T(54)));
//G354: __E_cli_command_104(248) => valid_sym(249) __(59) rr(14).
	p(NT(248), (NT(249)+NT(59)+NT(14)));
//G355: valid_cmd(247)       => __E_cli_command_104(248).
	p(NT(247), (NT(248)));
//G356: cli_command(225)     => valid_cmd(247).
	p(NT(225), (NT(247)));
//G357: sat_sym(252)         => 's' 'a' 't'.
	p(NT(252), (T(10)+T(17)+T(14)));
//G358: __E_cli_command_105(251) => sat_sym(252) __(59) rr(14).
	p(NT(251), (NT(252)+NT(59)+NT(14)));
//G359: sat_cmd(250)         => __E_cli_command_105(251).
	p(NT(250), (NT(251)));
//G360: cli_command(225)     => sat_cmd(250).
	p(NT(225), (NT(250)));
//G361: unsat_sym(255)       => 'u' 'n' 's' 'a' 't'.
	p(NT(255), (T(50)+T(47)+T(10)+T(17)+T(14)));
//G362: __E_cli_command_106(254) => unsat_sym(255) __(59) rr(14).
	p(NT(254), (NT(255)+NT(59)+NT(14)));
//G363: unsat_cmd(253)       => __E_cli_command_106(254).
	p(NT(253), (NT(254)));
//G364: cli_command(225)     => unsat_cmd(253).
	p(NT(225), (NT(253)));
//G365: solve_sym(258)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(258), (T(10)+T(11)+T(18)+T(52)+T(13)));
//G366: __E_cli_command_107(257) => solve_sym(258) __(59) wff_cmd_arg(259).
	p(NT(257), (NT(258)+NT(59)+NT(259)));
//G367: solve_cmd(256)       => __E_cli_command_107(257).
	p(NT(256), (NT(257)));
//G368: cli_command(225)     => solve_cmd(256).
	p(NT(225), (NT(256)));
//G369: __E___E_cli_command_108_109(263) => 'e'.
	p(NT(263), (T(13)));
//G370: __E___E_cli_command_108_109(263) => 'e' 'x' 'e' 'c' 'u' 't' 'e'.
	p(NT(263), (T(13)+T(26)+T(13)+T(46)+T(50)+T(14)+T(13)));
//G371: execute_sym(262)     => __E___E_cli_command_108_109(263).
	p(NT(262), (NT(263)));
//G372: __E___E_cli_command_108_110(264) => wff_typed(265).
	p(NT(264), (NT(265)));
//G373: __E___E_cli_command_108_110(264) => rr(14).
	p(NT(264), (NT(14)));
//G374: __E_cli_command_108(261) => execute_sym(262) __(59) __E___E_cli_command_108_110(264).
	p(NT(261), (NT(262)+NT(59)+NT(264)));
//G375: execute_cmd(260)     => __E_cli_command_108(261).
	p(NT(260), (NT(261)));
//G376: cli_command(225)     => execute_cmd(260).
	p(NT(225), (NT(260)));
//G377: __E___E_cli_command_111_112(269) => 'n'.
	p(NT(269), (T(47)));
//G378: __E___E_cli_command_111_112(269) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(269), (T(47)+T(11)+T(51)+T(12)+T(17)+T(18)+T(15)+T(53)+T(13)));
//G379: normalize_sym(268)   => __E___E_cli_command_111_112(269).
	p(NT(268), (NT(269)));
//G380: __E_cli_command_111(267) => normalize_sym(268) __(59) normalize_cmd_arg(270).
	p(NT(267), (NT(268)+NT(59)+NT(270)));
//G381: normalize_cmd(266)   => __E_cli_command_111(267).
	p(NT(266), (NT(267)));
//G382: cli_command(225)     => normalize_cmd(266).
	p(NT(225), (NT(266)));
//G383: __E___E_cli_command_113_114(274) => 's'.
	p(NT(274), (T(10)));
//G384: __E___E_cli_command_113_114(274) => 's' 'u' 'b' 's' 't'.
	p(NT(274), (T(10)+T(50)+T(24)+T(10)+T(14)));
//G385: __E___E_cli_command_113_114(274) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(274), (T(10)+T(50)+T(24)+T(10)+T(14)+T(15)+T(14)+T(50)+T(14)+T(13)));
//G386: subst_sym(273)       => __E___E_cli_command_113_114(274).
	p(NT(273), (NT(274)));
//G387: __E_cli_command_113(272) => subst_sym(273) __(59) nf_cmd_arg(275) _(15) '[' _(15) nf_cmd_arg(275) _(15) '/' _(15) nf_cmd_arg(275) _(15) ']'.
	p(NT(272), (NT(273)+NT(59)+NT(275)+NT(15)+T(21)+NT(15)+NT(275)+NT(15)+T(56)+NT(15)+NT(275)+NT(15)+T(22)));
//G388: subst_cmd(271)       => __E_cli_command_113(272).
	p(NT(271), (NT(272)));
//G389: cli_command(225)     => subst_cmd(271).
	p(NT(225), (NT(271)));
//G390: __E___E_cli_command_115_116(279) => 'i'.
	p(NT(279), (T(15)));
//G391: __E___E_cli_command_115_116(279) => 'i' 'n' 's' 't'.
	p(NT(279), (T(15)+T(47)+T(10)+T(14)));
//G392: __E___E_cli_command_115_116(279) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(279), (T(15)+T(47)+T(10)+T(14)+T(17)+T(47)+T(14)+T(15)+T(17)+T(14)+T(13)));
//G393: inst_sym(278)        => __E___E_cli_command_115_116(279).
	p(NT(278), (NT(279)));
//G394: __E_cli_command_115(277) => inst_sym(278) __(59) inst_args(280).
	p(NT(277), (NT(278)+NT(59)+NT(280)));
//G395: inst_cmd(276)        => __E_cli_command_115(277).
	p(NT(276), (NT(277)));
//G396: cli_command(225)     => inst_cmd(276).
	p(NT(225), (NT(276)));
//G397: dnf_sym(283)         => 'd' 'n' 'f'.
	p(NT(283), (T(54)+T(47)+T(44)));
//G398: __E_cli_command_117(282) => dnf_sym(283) __(59) nf_cmd_arg(275).
	p(NT(282), (NT(283)+NT(59)+NT(275)));
//G399: dnf_cmd(281)         => __E_cli_command_117(282).
	p(NT(281), (NT(282)));
//G400: cli_command(225)     => dnf_cmd(281).
	p(NT(225), (NT(281)));
//G401: cnf_sym(286)         => 'c' 'n' 'f'.
	p(NT(286), (T(46)+T(47)+T(44)));
//G402: __E_cli_command_118(285) => cnf_sym(286) __(59) nf_cmd_arg(275).
	p(NT(285), (NT(286)+NT(59)+NT(275)));
//G403: cnf_cmd(284)         => __E_cli_command_118(285).
	p(NT(284), (NT(285)));
//G404: cli_command(225)     => cnf_cmd(284).
	p(NT(225), (NT(284)));
//G405: anf_sym(289)         => 'a' 'n' 'f'.
	p(NT(289), (T(17)+T(47)+T(44)));
//G406: __E_cli_command_119(288) => anf_sym(289) __(59) nf_cmd_arg(275).
	p(NT(288), (NT(289)+NT(59)+NT(275)));
//G407: anf_cmd(287)         => __E_cli_command_119(288).
	p(NT(287), (NT(288)));
//G408: cli_command(225)     => anf_cmd(287).
	p(NT(225), (NT(287)));
//G409: nnf_sym(292)         => 'n' 'n' 'f'.
	p(NT(292), (T(47)+T(47)+T(44)));
//G410: __E_cli_command_120(291) => nnf_sym(292) __(59) nf_cmd_arg(275).
	p(NT(291), (NT(292)+NT(59)+NT(275)));
//G411: nnf_cmd(290)         => __E_cli_command_120(291).
	p(NT(290), (NT(291)));
//G412: cli_command(225)     => nnf_cmd(290).
	p(NT(225), (NT(290)));
//G413: pnf_sym(295)         => 'p' 'n' 'f'.
	p(NT(295), (T(55)+T(47)+T(44)));
//G414: __E_cli_command_121(294) => pnf_sym(295) __(59) nf_cmd_arg(275).
	p(NT(294), (NT(295)+NT(59)+NT(275)));
//G415: pnf_cmd(293)         => __E_cli_command_121(294).
	p(NT(293), (NT(294)));
//G416: cli_command(225)     => pnf_cmd(293).
	p(NT(225), (NT(293)));
//G417: mnf_sym(298)         => 'm' 'n' 'f'.
	p(NT(298), (T(12)+T(47)+T(44)));
//G418: __E_cli_command_122(297) => mnf_sym(298) __(59) nf_cmd_arg(275).
	p(NT(297), (NT(298)+NT(59)+NT(275)));
//G419: mnf_cmd(296)         => __E_cli_command_122(297).
	p(NT(296), (NT(297)));
//G420: cli_command(225)     => mnf_cmd(296).
	p(NT(225), (NT(296)));
//G421: snf_sym(301)         => 's' 'n' 'f'.
	p(NT(301), (T(10)+T(47)+T(44)));
//G422: __E_cli_command_123(300) => snf_sym(301) __(59) nf_cmd_arg(275).
	p(NT(300), (NT(301)+NT(59)+NT(275)));
//G423: snf_cmd(299)         => __E_cli_command_123(300).
	p(NT(299), (NT(300)));
//G424: cli_command(225)     => snf_cmd(299).
	p(NT(225), (NT(299)));
//G425: onf_sym(304)         => 'o' 'n' 'f'.
	p(NT(304), (T(11)+T(47)+T(44)));
//G426: __E_cli_command_124(303) => onf_sym(304) __(59) variable(137) __(59) onf_cmd_arg(305).
	p(NT(303), (NT(304)+NT(59)+NT(137)+NT(59)+NT(305)));
//G427: onf_cmd(302)         => __E_cli_command_124(303).
	p(NT(302), (NT(303)));
//G428: cli_command(225)     => onf_cmd(302).
	p(NT(225), (NT(302)));
//G429: __E___E_cli_command_125_126(309) => 'd' 'e' 'f' 's'.
	p(NT(309), (T(54)+T(13)+T(44)+T(10)));
//G430: __E___E_cli_command_125_126(309) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(309), (T(54)+T(13)+T(44)+T(15)+T(47)+T(15)+T(14)+T(15)+T(11)+T(47)+T(10)));
//G431: def_sym(308)         => __E___E_cli_command_125_126(309).
	p(NT(308), (NT(309)));
//G432: __E_cli_command_125(307) => def_sym(308).
	p(NT(307), (NT(308)));
//G433: def_list_cmd(306)    => __E_cli_command_125(307).
	p(NT(306), (NT(307)));
//G434: cli_command(225)     => def_list_cmd(306).
	p(NT(225), (NT(306)));
//G435: __E_cli_command_127(311) => def_sym(308) __(59) number(312).
	p(NT(311), (NT(308)+NT(59)+NT(312)));
//G436: def_print_cmd(310)   => __E_cli_command_127(311).
	p(NT(310), (NT(311)));
//G437: cli_command(225)     => def_print_cmd(310).
	p(NT(225), (NT(310)));
//G438: def_rr_cmd(313)      => rec_relation(20).
	p(NT(313), (NT(20)));
//G439: cli_command(225)     => def_rr_cmd(313).
	p(NT(225), (NT(313)));
//G440: __E___E_cli_command_128_129(317) => 'h' 'i' 's' 't'.
	p(NT(317), (T(48)+T(15)+T(10)+T(14)));
//G441: __E___E_cli_command_128_129(317) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(317), (T(48)+T(15)+T(10)+T(14)+T(11)+T(51)+T(20)));
//G442: history_sym(316)     => __E___E_cli_command_128_129(317).
	p(NT(316), (NT(317)));
//G443: __E_cli_command_128(315) => history_sym(316).
	p(NT(315), (NT(316)));
//G444: history_list_cmd(314) => __E_cli_command_128(315).
	p(NT(314), (NT(315)));
//G445: cli_command(225)     => history_list_cmd(314).
	p(NT(225), (NT(314)));
//G446: __E_cli_command_130(319) => history_sym(316) __(59) memory(320).
	p(NT(319), (NT(316)+NT(59)+NT(320)));
//G447: history_print_cmd(318) => __E_cli_command_130(319).
	p(NT(318), (NT(319)));
//G448: cli_command(225)     => history_print_cmd(318).
	p(NT(225), (NT(318)));
//G449: __E_cli_command_131(322) => bf_typed(323).
	p(NT(322), (NT(323)));
//G450: __E_cli_command_131(322) => wff_typed(265).
	p(NT(322), (NT(265)));
//G451: history_store_cmd(321) => __E_cli_command_131(322).
	p(NT(321), (NT(322)));
//G452: cli_command(225)     => history_store_cmd(321).
	p(NT(225), (NT(321)));
//G453: qelim_sym(326)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(326), (T(45)+T(13)+T(18)+T(15)+T(12)));
//G454: __E_cli_command_132(325) => qelim_sym(326) __(59) wff_cmd_arg(259).
	p(NT(325), (NT(326)+NT(59)+NT(259)));
//G455: qelim_cmd(324)       => __E_cli_command_132(325).
	p(NT(324), (NT(325)));
//G456: cli_command(225)     => qelim_cmd(324).
	p(NT(225), (NT(324)));
//G457: get_sym(329)         => 'g' 'e' 't'.
	p(NT(329), (T(49)+T(13)+T(14)));
//G458: __E___E_cli_command_133_134(330) => __(59) option(331).
	p(NT(330), (NT(59)+NT(331)));
//G459: __E___E_cli_command_133_134(330) => null.
	p(NT(330), (nul));
//G460: __E_cli_command_133(328) => get_sym(329) __E___E_cli_command_133_134(330).
	p(NT(328), (NT(329)+NT(330)));
//G461: get_cmd(327)         => __E_cli_command_133(328).
	p(NT(327), (NT(328)));
//G462: cli_command(225)     => get_cmd(327).
	p(NT(225), (NT(327)));
//G463: set_sym(334)         => 's' 'e' 't'.
	p(NT(334), (T(10)+T(13)+T(14)));
//G464: __E___E_cli_command_135_136(335) => __(59).
	p(NT(335), (NT(59)));
//G465: __E___E_cli_command_135_136(335) => _(15) '=' _(15).
	p(NT(335), (NT(15)+T(3)+NT(15)));
//G466: __E_cli_command_135(333) => set_sym(334) __(59) option(331) __E___E_cli_command_135_136(335) option_value(336).
	p(NT(333), (NT(334)+NT(59)+NT(331)+NT(335)+NT(336)));
//G467: set_cmd(332)         => __E_cli_command_135(333).
	p(NT(332), (NT(333)));
//G468: cli_command(225)     => set_cmd(332).
	p(NT(225), (NT(332)));
//G469: toggle_sym(339)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(339), (T(14)+T(11)+T(49)+T(49)+T(18)+T(13)));
//G470: __E_cli_command_137(338) => toggle_sym(339) __(59) bool_option(340).
	p(NT(338), (NT(339)+NT(59)+NT(340)));
//G471: toggle_cmd(337)      => __E_cli_command_137(338).
	p(NT(337), (NT(338)));
//G472: cli_command(225)     => toggle_cmd(337).
	p(NT(225), (NT(337)));
//G473: number(312)          => digits(146).
	p(NT(312), (NT(146)));
//G474: bf_typed(323)        => bf(25).
	p(NT(323), (NT(25)));
//G475: bf_typed(323)        => bf(25) _(15) ':' _(15) 'b' 'f'.
	p(NT(323), (NT(25)+NT(15)+T(2)+NT(15)+T(24)+T(44)));
//G476: wff_typed(265)       => wff(18).
	p(NT(265), (NT(18)));
//G477: wff_typed(265)       => wff(18) _(15) ':' _(15) 'w' 'f' 'f'.
	p(NT(265), (NT(18)+NT(15)+T(2)+NT(15)+T(19)+T(44)+T(44)));
//G478: bf_cmd_arg(341)      => memory(320).
	p(NT(341), (NT(320)));
//G479: bf_cmd_arg(341)      => bf_typed(323).
	p(NT(341), (NT(323)));
//G480: wff_cmd_arg(259)     => memory(320).
	p(NT(259), (NT(320)));
//G481: wff_cmd_arg(259)     => wff_typed(265).
	p(NT(259), (NT(265)));
//G482: nf_cmd_arg(275)      => memory(320).
	p(NT(275), (NT(320)));
//G483: nf_cmd_arg(275)      => bf_typed(323).
	p(NT(275), (NT(323)));
//G484: nf_cmd_arg(275)      => wff_typed(265).
	p(NT(275), (NT(265)));
//G485: onf_cmd_arg(305)     => memory(320).
	p(NT(305), (NT(320)));
//G486: onf_cmd_arg(305)     => wff_typed(265).
	p(NT(305), (NT(265)));
//G487: normalize_cmd_arg(270) => memory(320).
	p(NT(270), (NT(320)));
//G488: normalize_cmd_arg(270) => bf_typed(323).
	p(NT(270), (NT(323)));
//G489: normalize_cmd_arg(270) => wff_typed(265).
	p(NT(270), (NT(265)));
//G490: normalize_cmd_arg(270) => rr(14).
	p(NT(270), (NT(14)));
//G491: inst_args(280)       => bf_cmd_arg(341) _(15) '[' _(15) variable(137) _(15) '/' _(15) bf_cmd_arg(341) _(15) ']'.
	p(NT(280), (NT(341)+NT(15)+T(21)+NT(15)+NT(137)+NT(15)+T(56)+NT(15)+NT(341)+NT(15)+T(22)));
//G492: inst_args(280)       => wff_cmd_arg(259) _(15) '[' _(15) variable(137) _(15) '/' _(15) bf_cmd_arg(341) _(15) ']'.
	p(NT(280), (NT(259)+NT(15)+T(21)+NT(15)+NT(137)+NT(15)+T(56)+NT(15)+NT(341)+NT(15)+T(22)));
//G493: inst_args(280)       => wff_cmd_arg(259) _(15) '[' _(15) bool_variable(116) _(15) '/' _(15) wff_cmd_arg(259) _(15) ']'.
	p(NT(280), (NT(259)+NT(15)+T(21)+NT(15)+NT(116)+NT(15)+T(56)+NT(15)+NT(259)+NT(15)+T(22)));
//G494: help_arg(242)        => help_sym(239).
	p(NT(242), (NT(239)));
//G495: help_arg(242)        => version_sym(232).
	p(NT(242), (NT(232)));
//G496: help_arg(242)        => quit_sym(229).
	p(NT(242), (NT(229)));
//G497: help_arg(242)        => clear_sym(235).
	p(NT(242), (NT(235)));
//G498: help_arg(242)        => get_sym(329).
	p(NT(242), (NT(329)));
//G499: help_arg(242)        => set_sym(334).
	p(NT(242), (NT(334)));
//G500: help_arg(242)        => toggle_sym(339).
	p(NT(242), (NT(339)));
//G501: help_arg(242)        => file_sym(245).
	p(NT(242), (NT(245)));
//G502: help_arg(242)        => history_sym(316).
	p(NT(242), (NT(316)));
//G503: help_arg(242)        => abs_memory_sym(342).
	p(NT(242), (NT(342)));
//G504: help_arg(242)        => rel_memory_sym(343).
	p(NT(242), (NT(343)));
//G505: help_arg(242)        => selection_sym(344).
	p(NT(242), (NT(344)));
//G506: help_arg(242)        => def_sym(308).
	p(NT(242), (NT(308)));
//G507: help_arg(242)        => inst_sym(278).
	p(NT(242), (NT(278)));
//G508: help_arg(242)        => subst_sym(273).
	p(NT(242), (NT(273)));
//G509: help_arg(242)        => normalize_sym(268).
	p(NT(242), (NT(268)));
//G510: help_arg(242)        => execute_sym(262).
	p(NT(242), (NT(262)));
//G511: help_arg(242)        => solve_sym(258).
	p(NT(242), (NT(258)));
//G512: help_arg(242)        => valid_sym(249).
	p(NT(242), (NT(249)));
//G513: help_arg(242)        => sat_sym(252).
	p(NT(242), (NT(252)));
//G514: help_arg(242)        => unsat_sym(255).
	p(NT(242), (NT(255)));
//G515: help_arg(242)        => dnf_sym(283).
	p(NT(242), (NT(283)));
//G516: help_arg(242)        => cnf_sym(286).
	p(NT(242), (NT(286)));
//G517: help_arg(242)        => anf_sym(289).
	p(NT(242), (NT(289)));
//G518: help_arg(242)        => snf_sym(301).
	p(NT(242), (NT(301)));
//G519: help_arg(242)        => nnf_sym(292).
	p(NT(242), (NT(292)));
//G520: help_arg(242)        => pnf_sym(295).
	p(NT(242), (NT(295)));
//G521: help_arg(242)        => mnf_sym(298).
	p(NT(242), (NT(298)));
//G522: help_arg(242)        => onf_sym(304).
	p(NT(242), (NT(304)));
//G523: help_arg(242)        => qelim_sym(326).
	p(NT(242), (NT(326)));
//G524: __E___E_help_arg_138_139(347) => 's'.
	p(NT(347), (T(10)));
//G525: __E___E_help_arg_138_139(347) => null.
	p(NT(347), (nul));
//G526: __E_help_arg_138(346) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_138_139(347).
	p(NT(346), (T(13)+T(26)+T(17)+T(12)+T(55)+T(18)+T(13)+NT(347)));
//G527: examples_sym(345)    => __E_help_arg_138(346).
	p(NT(345), (NT(346)));
//G528: help_arg(242)        => examples_sym(345).
	p(NT(242), (NT(345)));
//G529: __E_memory_140(349)  => '%'.
	p(NT(349), (T(57)));
//G530: rel_memory_sym(343)  => '%' '-'.
	p(NT(343), (T(57)+T(27)));
//G531: memory_id(351)       => digits(146).
	p(NT(351), (NT(146)));
//G532: __E___E_memory_140_141(350) => memory_id(351).
	p(NT(350), (NT(351)));
//G533: __E___E_memory_140_141(350) => null.
	p(NT(350), (nul));
//G534: __E_memory_140(349)  => rel_memory_sym(343) __E___E_memory_140_141(350).
	p(NT(349), (NT(343)+NT(350)));
//G535: rel_memory(348)      => __E_memory_140(349).
	p(NT(348), (NT(349)));
//G536: rel_memory(348)      => rel_memory(348).
	p(NT(348), (NT(348)));
//G537: memory(320)          => rel_memory(348).
	p(NT(320), (NT(348)));
//G538: abs_memory_sym(342)  => '%'.
	p(NT(342), (T(57)));
//G539: __E_memory_142(353)  => abs_memory_sym(342) memory_id(351).
	p(NT(353), (NT(342)+NT(351)));
//G540: abs_memory(352)      => __E_memory_142(353).
	p(NT(352), (NT(353)));
//G541: memory(320)          => abs_memory(352).
	p(NT(320), (NT(352)));
//G542: option(331)          => bool_option(340).
	p(NT(331), (NT(340)));
//G543: __E_option_143(356)  => 's' 'e' 'v'.
	p(NT(356), (T(10)+T(13)+T(52)));
//G544: __E_option_143(356)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(356), (T(10)+T(13)+T(52)+T(13)+T(51)+T(15)+T(14)+T(20)));
//G545: severity_opt(355)    => __E_option_143(356).
	p(NT(355), (NT(356)));
//G546: enum_option(354)     => severity_opt(355).
	p(NT(354), (NT(355)));
//G547: option(331)          => enum_option(354).
	p(NT(331), (NT(354)));
//G548: __E_bool_option_144(358) => 's'.
	p(NT(358), (T(10)));
//G549: __E_bool_option_144(358) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(358), (T(10)+T(14)+T(17)+T(14)+T(50)+T(10)));
//G550: status_opt(357)      => __E_bool_option_144(358).
	p(NT(357), (NT(358)));
//G551: bool_option(340)     => status_opt(357).
	p(NT(340), (NT(357)));
//G552: __E_bool_option_145(360) => 'c'.
	p(NT(360), (T(46)));
//G553: __E_bool_option_145(360) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(360), (T(46)+T(11)+T(18)+T(11)+T(51)));
//G554: __E_bool_option_145(360) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(360), (T(46)+T(11)+T(18)+T(11)+T(51)+T(10)));
//G555: colors_opt(359)      => __E_bool_option_145(360).
	p(NT(359), (NT(360)));
//G556: bool_option(340)     => colors_opt(359).
	p(NT(340), (NT(359)));
//G557: __E_bool_option_146(362) => 'd'.
	p(NT(362), (T(54)));
//G558: __E_bool_option_146(362) => 'd' 'b' 'g'.
	p(NT(362), (T(54)+T(24)+T(49)));
//G559: __E_bool_option_146(362) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(362), (T(54)+T(13)+T(24)+T(50)+T(49)));
//G560: debug_repl_opt(361)  => __E_bool_option_146(362).
	p(NT(361), (NT(362)));
//G561: bool_option(340)     => debug_repl_opt(361).
	p(NT(340), (NT(361)));
//G562: option_value(336)    => option_value_true(363).
	p(NT(336), (NT(363)));
//G563: option_value(336)    => option_value_false(364).
	p(NT(336), (NT(364)));
//G564: option_value(336)    => severity(365).
	p(NT(336), (NT(365)));
//G565: option_value_true(363) => 't'.
	p(NT(363), (T(14)));
//G566: option_value_true(363) => 't' 'r' 'u' 'e'.
	p(NT(363), (T(14)+T(51)+T(50)+T(13)));
//G567: option_value_true(363) => 'o' 'n'.
	p(NT(363), (T(11)+T(47)));
//G568: option_value_true(363) => '1'.
	p(NT(363), (T(37)));
//G569: option_value_true(363) => 'y'.
	p(NT(363), (T(20)));
//G570: option_value_true(363) => 'y' 'e' 's'.
	p(NT(363), (T(20)+T(13)+T(10)));
//G571: option_value_false(364) => 'f'.
	p(NT(364), (T(44)));
//G572: option_value_false(364) => 'f' 'a' 'l' 's' 'e'.
	p(NT(364), (T(44)+T(17)+T(18)+T(10)+T(13)));
//G573: option_value_false(364) => 'o' 'f' 'f'.
	p(NT(364), (T(11)+T(44)+T(44)));
//G574: option_value_false(364) => '0'.
	p(NT(364), (T(38)));
//G575: option_value_false(364) => 'n'.
	p(NT(364), (T(47)));
//G576: option_value_false(364) => 'n' 'o'.
	p(NT(364), (T(47)+T(11)));
//G577: __E_severity_147(367) => 'e'.
	p(NT(367), (T(13)));
//G578: __E_severity_147(367) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(367), (T(13)+T(51)+T(51)+T(11)+T(51)));
//G579: error_sym(366)       => __E_severity_147(367).
	p(NT(366), (NT(367)));
//G580: severity(365)        => error_sym(366).
	p(NT(365), (NT(366)));
//G581: __E_severity_148(369) => 'i'.
	p(NT(369), (T(15)));
//G582: __E_severity_148(369) => 'i' 'n' 'f' 'o'.
	p(NT(369), (T(15)+T(47)+T(44)+T(11)));
//G583: info_sym(368)        => __E_severity_148(369).
	p(NT(368), (NT(369)));
//G584: severity(365)        => info_sym(368).
	p(NT(365), (NT(368)));
//G585: __E_severity_149(371) => 'd'.
	p(NT(371), (T(54)));
//G586: __E_severity_149(371) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(371), (T(54)+T(13)+T(24)+T(50)+T(49)));
//G587: debug_sym(370)       => __E_severity_149(371).
	p(NT(370), (NT(371)));
//G588: severity(365)        => debug_sym(370).
	p(NT(365), (NT(370)));
//G589: __E_severity_150(373) => 't'.
	p(NT(373), (T(14)));
//G590: __E_severity_150(373) => 't' 'r' 'a' 'c' 'e'.
	p(NT(373), (T(14)+T(51)+T(17)+T(46)+T(13)));
//G591: trace_sym(372)       => __E_severity_150(373).
	p(NT(372), (NT(373)));
//G592: severity(365)        => trace_sym(372).
	p(NT(365), (NT(372)));
//G593: rule(43)             => tau_rule(375).
	p(NT(43), (NT(375)));
//G594: tau_matcher(376)     => tau(377).
	p(NT(376), (NT(377)));
//G595: __E_tau_rule_151(379) => tau(377).
	p(NT(379), (NT(377)));
//G596: __E_tau_rule_151(379) => tau_cb(374).
	p(NT(379), (NT(374)));
//G597: tau_body(378)        => __E_tau_rule_151(379).
	p(NT(378), (NT(379)));
//G598: tau_rule(375)        => tau_matcher(376) _(15) ':' ':' ':' '=' _(15) tau_body(378) _(15) '.'.
	p(NT(375), (NT(376)+NT(15)+T(2)+T(2)+T(2)+T(3)+NT(15)+NT(378)+NT(15)+T(1)));
//G599: __E_builder_body_152(381) => '=' ':' ':' ':' _(15) tau(377).
	p(NT(381), (T(3)+T(2)+T(2)+T(2)+NT(15)+NT(377)));
//G600: tau_builder_body(380) => __E_builder_body_152(381).
	p(NT(380), (NT(381)));
//G601: builder_body(57)     => tau_builder_body(380).
	p(NT(57), (NT(380)));
//G602: __E_tau_153(383)     => '(' _(15) tau(377) _(15) ')'.
	p(NT(383), (T(5)+NT(15)+NT(377)+NT(15)+T(6)));
//G603: tau_parenthesis(382) => __E_tau_153(383).
	p(NT(382), (NT(383)));
//G604: tau(377)             => tau_parenthesis(382).
	p(NT(377), (NT(382)));
//G605: __E_tau_154(385)     => '{' _(15) wff(18) _(15) '}'.
	p(NT(385), (T(8)+NT(15)+NT(18)+NT(15)+T(9)));
//G606: tau_wff(384)         => __E_tau_154(385).
	p(NT(384), (NT(385)));
//G607: tau(377)             => tau_wff(384).
	p(NT(377), (NT(384)));
//G608: __E___E_tau_155_156(388) => _(15) offsets(28).
	p(NT(388), (NT(15)+NT(28)));
//G609: __E___E_tau_155_156(388) => null.
	p(NT(388), (nul));
//G610: __E_tau_155(387)     => sym(26) __E___E_tau_155_156(388) _(15) ref_args(29).
	p(NT(387), (NT(26)+NT(388)+NT(15)+NT(29)));
//G611: tau_ref(386)         => __E_tau_155(387).
	p(NT(386), (NT(387)));
//G612: tau(377)             => tau_ref(386).
	p(NT(377), (NT(386)));
//G613: __E_tau_157(390)     => tau(377) _(15) '|' '|' '|' _(15) tau(377).
	p(NT(390), (NT(377)+NT(15)+T(28)+T(28)+T(28)+NT(15)+NT(377)));
//G614: tau_or(389)          => __E_tau_157(390).
	p(NT(389), (NT(390)));
//G615: tau(377)             => tau_or(389).
	p(NT(377), (NT(389)));
//G616: __E_tau_158(392)     => tau(377) _(15) '&' '&' '&' _(15) tau(377).
	p(NT(392), (NT(377)+NT(15)+T(30)+T(30)+T(30)+NT(15)+NT(377)));
//G617: tau_and(391)         => __E_tau_158(392).
	p(NT(391), (NT(392)));
//G618: tau(377)             => tau_and(391).
	p(NT(377), (NT(391)));
//G619: __E_tau_159(394)     => '-' _(15) tau(377).
	p(NT(394), (T(27)+NT(15)+NT(377)));
//G620: tau_neg(393)         => __E_tau_159(394).
	p(NT(393), (NT(394)));
//G621: tau(377)             => tau_neg(393).
	p(NT(377), (NT(393)));
//G622: tau(377)             => capture(24).
	p(NT(377), (NT(24)));
//G623: __E_tau_cb_args1_160(397) => capture(24).
	p(NT(397), (NT(24)));
//G624: __E_tau_cb_args1_160(397) => tau(377).
	p(NT(397), (NT(377)));
//G625: tau_cb_arg(396)      => __E_tau_cb_args1_160(397).
	p(NT(396), (NT(397)));
//G626: tau_cb_args1(395)    => __(59) tau_cb_arg(396).
	p(NT(395), (NT(59)+NT(396)));
//G627: tau_cb_args2(398)    => tau_cb_args1(395) tau_cb_args1(395).
	p(NT(398), (NT(395)+NT(395)));
//G628: tau_cb_args3(399)    => tau_cb_args2(398) tau_cb_args1(395).
	p(NT(399), (NT(398)+NT(395)));
//G629: __E___E_tau_cb_161_162(402) => tau_cb_args3(399).
	p(NT(402), (NT(399)));
//G630: __E___E_tau_cb_161_162(402) => tau_cb_args2(398).
	p(NT(402), (NT(398)));
//G631: __E_tau_cb_161(401)  => 't' 'a' 'u' '_' 'c' 'o' 'l' 'l' 'a' 'p' 's' 'e' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'c' 'b' __E___E_tau_cb_161_162(402).
	p(NT(401), (T(14)+T(17)+T(50)+T(25)+T(46)+T(11)+T(18)+T(18)+T(17)+T(55)+T(10)+T(13)+T(25)+T(55)+T(11)+T(10)+T(15)+T(14)+T(15)+T(52)+T(13)+T(10)+T(25)+T(46)+T(24)+NT(402)));
//G632: tau_collapse_positives_cb(400) => __E_tau_cb_161(401).
	p(NT(400), (NT(401)));
//G633: tau_cb(374)          => tau_collapse_positives_cb(400).
	p(NT(374), (NT(400)));
//G634: __E_tau_cb_163(404)  => 't' 'a' 'u' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'u' 'p' 'w' 'a' 'r' 'd' 's' '_' 'c' 'b' tau_cb_args2(398).
	p(NT(404), (T(14)+T(17)+T(50)+T(25)+T(55)+T(11)+T(10)+T(15)+T(14)+T(15)+T(52)+T(13)+T(10)+T(25)+T(50)+T(55)+T(19)+T(17)+T(51)+T(54)+T(10)+T(25)+T(46)+T(24)+NT(398)));
//G635: tau_positives_upwards_cb(403) => __E_tau_cb_163(404).
	p(NT(403), (NT(404)));
//G636: tau_cb(374)          => tau_positives_upwards_cb(403).
	p(NT(374), (NT(403)));
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
		bf_interval, __E_wff_37, bf_neq, __E_wff_38, bf_eq, __E_wff_39, bf_nleq_lower, __E_wff_40, bf_nleq_upper, __E_wff_41, 
		bf_greater, __E_wff_42, bf_less_equal, __E_wff_43, bf_less, __E_wff_44, bool_variable, uninterpreted_constant, bf_parenthesis, __E_bf_45, 
		bf_splitter, __E_bf_46, bf_ref, bf_or, __E_bf_47, bf_xor, __E_bf_48, bf_and, __E_bf_49, __E___E_bf_49_50, 
		bf_neg, __E_bf_51, bf_constant, __E_bf_52, constant, bf_t, bf_f, variable, charvar, __E_charvar_53, 
		__E_charvar_54, io_var, __E_variable_55, out, in_var_name, __E_in_56, digits, offset, out_var_name, __E_out_57, 
		__E_offsets_58, __E_offsets_59, num, shift, __E_offset_60, __E_shift_61, __E___E_shift_61_62, uninter_const_name, __E_uninterpreted_constant_63, __E___E_uninterpreted_constant_63_64, 
		chars, binding, __E_binding_65, type, __E___E_binding_65_66, source, named_binding, src_c, __E_source_67, __E_source_68, 
		__E___E_source_68_69, __E___E_source_68_70, __E_chars_71, __E_digits_72, comment, __E_comment_73, __E_comment_74, __E_comment_75, __E____76, __E___77, 
		bf_cb_args1, bf_cb_arg, bf_cb_args2, bf_cb_args3, bf_cb_args4, wff_cb_args1, wff_cb_arg, wff_cb_args2, wff_cb_args3, wff_cb_args4, 
		bf_eq_cb, __E_wff_cb_78, bf_neq_cb, __E_wff_cb_79, wff_has_clashing_subformulas_cb, __E_wff_cb_80, wff_has_subformula_cb, __E_wff_cb_81, wff_remove_existential_cb, __E_wff_cb_82, 
		wff_remove_bexistential_cb, __E_wff_cb_83, wff_remove_buniversal_cb, __E_wff_cb_84, bf_is_zero_cb, __E_bf_cb_85, bf_is_one_cb, __E_bf_cb_86, bf_has_subformula_cb, __E_bf_cb_87, 
		bf_remove_funiversal_cb, __E_bf_cb_88, bf_remove_fexistential_cb, __E_bf_cb_89, bf_and_cb, __E_bf_cb_90, bf_or_cb, __E_bf_cb_91, bf_xor_cb, __E_bf_cb_92, 
		bf_neg_cb, __E_bf_cb_93, bf_normalize_cb, __E_bf_cb_94, cli, cli_command, __E_cli_95, __E_cli_96, quit_cmd, quit_sym, 
		__E_cli_command_97, version_cmd, version_sym, __E_cli_command_98, clear_cmd, clear_sym, __E_cli_command_99, help_cmd, __E_cli_command_100, help_sym, 
		__E___E_cli_command_100_101, __E___E_cli_command_100_102, help_arg, file_cmd, __E_cli_command_103, file_sym, q_string, valid_cmd, __E_cli_command_104, valid_sym, 
		sat_cmd, __E_cli_command_105, sat_sym, unsat_cmd, __E_cli_command_106, unsat_sym, solve_cmd, __E_cli_command_107, solve_sym, wff_cmd_arg, 
		execute_cmd, __E_cli_command_108, execute_sym, __E___E_cli_command_108_109, __E___E_cli_command_108_110, wff_typed, normalize_cmd, __E_cli_command_111, normalize_sym, __E___E_cli_command_111_112, 
		normalize_cmd_arg, subst_cmd, __E_cli_command_113, subst_sym, __E___E_cli_command_113_114, nf_cmd_arg, inst_cmd, __E_cli_command_115, inst_sym, __E___E_cli_command_115_116, 
		inst_args, dnf_cmd, __E_cli_command_117, dnf_sym, cnf_cmd, __E_cli_command_118, cnf_sym, anf_cmd, __E_cli_command_119, anf_sym, 
		nnf_cmd, __E_cli_command_120, nnf_sym, pnf_cmd, __E_cli_command_121, pnf_sym, mnf_cmd, __E_cli_command_122, mnf_sym, snf_cmd, 
		__E_cli_command_123, snf_sym, onf_cmd, __E_cli_command_124, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_125, def_sym, __E___E_cli_command_125_126, 
		def_print_cmd, __E_cli_command_127, number, def_rr_cmd, history_list_cmd, __E_cli_command_128, history_sym, __E___E_cli_command_128_129, history_print_cmd, __E_cli_command_130, 
		memory, history_store_cmd, __E_cli_command_131, bf_typed, qelim_cmd, __E_cli_command_132, qelim_sym, get_cmd, __E_cli_command_133, get_sym, 
		__E___E_cli_command_133_134, option, set_cmd, __E_cli_command_135, set_sym, __E___E_cli_command_135_136, option_value, toggle_cmd, __E_cli_command_137, toggle_sym, 
		bool_option, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, examples_sym, __E_help_arg_138, __E___E_help_arg_138_139, rel_memory, __E_memory_140, 
		__E___E_memory_140_141, memory_id, abs_memory, __E_memory_142, enum_option, severity_opt, __E_option_143, status_opt, __E_bool_option_144, colors_opt, 
		__E_bool_option_145, debug_repl_opt, __E_bool_option_146, option_value_true, option_value_false, severity, error_sym, __E_severity_147, info_sym, __E_severity_148, 
		debug_sym, __E_severity_149, trace_sym, __E_severity_150, tau_cb, tau_rule, tau_matcher, tau, tau_body, __E_tau_rule_151, 
		tau_builder_body, __E_builder_body_152, tau_parenthesis, __E_tau_153, tau_wff, __E_tau_154, tau_ref, __E_tau_155, __E___E_tau_155_156, tau_or, 
		__E_tau_157, tau_and, __E_tau_158, tau_neg, __E_tau_159, tau_cb_args1, tau_cb_arg, __E_tau_cb_args1_160, tau_cb_args2, tau_cb_args3, 
		tau_collapse_positives_cb, __E_tau_cb_161, __E___E_tau_cb_161_162, tau_positives_upwards_cb, __E_tau_cb_163, __N_0, __N_1, __N_2, __N_3, __N_4, 
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
