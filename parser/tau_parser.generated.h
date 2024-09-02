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
	"rr", "_", "rec_relations", "main", "wff", "__E_rec_relations_0", "rec_relation", "__E_rec_relations_1", "ref", "__E_rec_relation_2", 
	"capture", "bf", "sym", "__E_ref_3", "offsets", "ref_args", "__E_ref_args_4", "__E___E_ref_args_4_5", "ref_arg", "__E___E_ref_args_4_6", 
	"__E___E_ref_args_4_7", "inputs", "__E_inputs_8", "__E_inputs_9", "input", "in", "source_binding", "library", "rules", "__E_library_10", 
	"__E___E_library_10_11", "rule", "__E___E___E_library_10_11_12", "wff_rule", "bf_rule", "__E___E_library_10_13", "wff_matcher", "wff_body", "__E_wff_rule_14", "wff_cb", 
	"bf_matcher", "bf_body", "__E_bf_rule_15", "bf_cb", "builder", "builder_head", "builder_body", "__E_builder_head_16", "__", "__E_builder_head_17", 
	"bf_builder_body", "__E_builder_body_18", "wff_builder_body", "__E_builder_body_19", "wff_parenthesis", "__E_wff_20", "wff_sometimes", "__E_wff_21", "__E___E_wff_21_22", "wff_always", 
	"__E_wff_23", "__E___E_wff_23_24", "wff_conditional", "__E_wff_25", "wff_all", "__E_wff_26", "q_vars", "wff_ex", "__E_wff_27", "wff_ref", 
	"wff_imply", "__E_wff_28", "wff_equiv", "__E_wff_29", "wff_or", "__E_wff_30", "wff_xor", "__E_wff_31", "wff_and", "__E_wff_32", 
	"wff_neg", "__E_wff_33", "wff_t", "wff_f", "bf_interval", "__E_wff_34", "bf_neq", "__E_wff_35", "bf_eq", "__E_wff_36", 
	"bf_nleq", "__E_wff_37", "bf_greater", "__E_wff_38", "bf_less_equal", "__E_wff_39", "bf_less", "__E_wff_40", "uninterpreted_constant", "bf_parenthesis", 
	"__E_bf_41", "variable", "bf_splitter", "__E_bf_42", "bf_ref", "bf_or", "__E_bf_43", "bf_xor", "__E_bf_44", "bf_and", 
	"__E_bf_45", "__E___E_bf_45_46", "bf_neg", "__E_bf_47", "bf_constant", "__E_bf_48", "constant", "bf_t", "bf_f", "flag", 
	"flag_neq", "__E_flag_49", "flagvar", "num", "flag_eq", "__E_flag_50", "flag_greater_equal", "__E_flag_51", "flag_greater", "__E_flag_52", 
	"flag_less_equal", "__E_flag_53", "flag_less", "__E_flag_54", "charvar", "__E_charvar_55", "__E_charvar_56", "io_var", "__E_variable_57", "out", 
	"in_var_name", "__E_in_58", "digits", "offset", "out_var_name", "__E_out_59", "q_var", "__E_q_vars_60", "__E_q_vars_61", "__E_offsets_62", 
	"__E_offsets_63", "shift", "__E_offset_64", "__E_shift_65", "__E___E_shift_65_66", "uninter_const_name", "__E_uninterpreted_constant_67", "__E___E_uninterpreted_constant_67_68", "chars", "binding", 
	"__E_binding_69", "type", "__E___E_binding_69_70", "source", "named_binding", "src_c", "__E_source_71", "__E_source_72", "__E___E_source_72_73", "__E___E_source_72_74", 
	"__E_chars_75", "__E_digits_76", "comment", "__E_comment_77", "__E_comment_78", "__E_comment_79", "__E____80", "__E___81", "bf_cb_args1", "bf_cb_arg", 
	"bf_cb_args2", "bf_cb_args3", "bf_cb_args4", "wff_cb_args1", "wff_cb_arg", "wff_cb_args2", "wff_cb_args3", "wff_cb_args4", "wff_has_clashing_subformulas_cb", "__E_wff_cb_82", 
	"wff_has_subformula_cb", "__E_wff_cb_83", "wff_remove_existential_cb", "__E_wff_cb_84", "bf_has_subformula_cb", "__E_bf_cb_85", "bf_remove_funiversal_cb", "__E_bf_cb_86", "bf_remove_fexistential_cb", "__E_bf_cb_87", 
	"bf_normalize_cb", "__E_bf_cb_88", "cli", "cli_command", "__E_cli_89", "__E_cli_90", "quit_cmd", "quit_sym", "__E_cli_command_91", "version_cmd", 
	"version_sym", "__E_cli_command_92", "clear_cmd", "clear_sym", "__E_cli_command_93", "help_cmd", "__E_cli_command_94", "help_sym", "__E___E_cli_command_94_95", "__E___E_cli_command_94_96", 
	"help_arg", "file_cmd", "__E_cli_command_97", "file_sym", "q_string", "valid_cmd", "__E_cli_command_98", "valid_sym", "sat_cmd", "__E_cli_command_99", 
	"sat_sym", "normalize_cmd_arg", "unsat_cmd", "__E_cli_command_100", "unsat_sym", "solve_cmd", "__E_cli_command_101", "solve_sym", "wff_cmd_arg", "execute_cmd", 
	"__E_cli_command_102", "execute_sym", "__E___E_cli_command_102_103", "__E___E_cli_command_102_104", "normalize_cmd", "__E_cli_command_105", "normalize_sym", "__E___E_cli_command_105_106", "subst_cmd", "__E_cli_command_107", 
	"subst_sym", "__E___E_cli_command_107_108", "nf_cmd_arg", "inst_cmd", "__E_cli_command_109", "inst_sym", "__E___E_cli_command_109_110", "inst_args", "dnf_cmd", "__E_cli_command_111", 
	"dnf_sym", "cnf_cmd", "__E_cli_command_112", "cnf_sym", "anf_cmd", "__E_cli_command_113", "anf_sym", "nnf_cmd", "__E_cli_command_114", "nnf_sym", 
	"pnf_cmd", "__E_cli_command_115", "pnf_sym", "mnf_cmd", "__E_cli_command_116", "mnf_sym", "snf_cmd", "__E_cli_command_117", "snf_sym", "onf_cmd", 
	"__E_cli_command_118", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_119", "def_sym", "__E___E_cli_command_119_120", "def_print_cmd", "__E_cli_command_121", "number", 
	"def_rr_cmd", "qelim_cmd", "__E_cli_command_122", "qelim_sym", "get_cmd", "__E_cli_command_123", "get_sym", "__E___E_cli_command_123_124", "option", "set_cmd", 
	"__E_cli_command_125", "set_sym", "__E___E_cli_command_125_126", "option_value", "toggle_cmd", "__E_cli_command_127", "toggle_sym", "bool_option", "history_list_cmd", "__E_cli_command_128", 
	"history_sym", "__E___E_cli_command_128_129", "history_print_cmd", "__E_cli_command_130", "memory", "history_store_cmd", "__E_cli_command_131", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", 
	"selection_sym", "examples_sym", "__E_help_arg_132", "__E___E_help_arg_132_133", "rel_memory", "__E_memory_134", "__E___E_memory_134_135", "memory_id", "abs_memory", "__E_memory_136", 
	"enum_option", "severity_opt", "__E_option_137", "status_opt", "__E_bool_option_138", "colors_opt", "__E_bool_option_139", "debug_repl_opt", "__E_bool_option_140", "option_value_true", 
	"option_value_false", "severity", "error_sym", "__E_severity_141", "info_sym", "__E_severity_142", "debug_sym", "__E_severity_143", "trace_sym", "__E_severity_144", 
	"__N_0", "__N_1", "__N_2", "__N_3", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', ',', '(', ')', '<', '{', 
	'}', 's', 'o', 'm', 'e', 't', 'i', '>', 'a', 'l', 
	'w', 'y', '[', ']', '?', 'x', '-', '|', '^', '&', 
	'!', 'T', 'F', 'S', '+', '\'', '1', '0', '$', '\t', 
	'\n', '\r', '#', 'f', '_', 'h', 'c', 'n', 'g', 'u', 
	'b', 'r', 'v', 'z', 'q', 'p', 'd', '/', '%', 
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
			11, 58
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			20, 22, 144, 150, 152, 154, 165, 171, 173, 174
		},
		.to_inline = {
			{ 14, 64, 14 },
			{ 21, 109, 21 },
			{ 49 },
			{ 53 },
			{ 156 },
			{ 168 },
			{ 175 },
			{ 188 },
			{ 190 },
			{ 191 },
			{ 192 },
			{ 193 },
			{ 195 },
			{ 196 },
			{ 197 },
			{ 241 },
			{ 248 },
			{ 262 },
			{ 267 },
			{ 292 },
			{ 327 }
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
//G0:   start(9)             => rr(10) _(11).
	p(NT(9), (NT(10)+NT(11)));
//G1:   main(13)             => wff(14).
	p(NT(13), (NT(14)));
//G2:   rr(10)               => rec_relations(12) _(11) main(13) _(11) '.'.
	p(NT(10), (NT(12)+NT(11)+NT(13)+NT(11)+T(1)));
//G3:   __E_rec_relations_0(15) => _(11) rec_relation(16) _(11) '.'.
	p(NT(15), (NT(11)+NT(16)+NT(11)+T(1)));
//G4:   __E_rec_relations_1(17) => null.
	p(NT(17), (nul));
//G5:   __E_rec_relations_1(17) => __E_rec_relations_0(15) __E_rec_relations_1(17).
	p(NT(17), (NT(15)+NT(17)));
//G6:   rec_relations(12)    => __E_rec_relations_1(17).
	p(NT(12), (NT(17)));
//G7:   __E_rec_relation_2(19) => capture(20).
	p(NT(19), (NT(20)));
//G8:   __E_rec_relation_2(19) => ref(18).
	p(NT(19), (NT(18)));
//G9:   __E_rec_relation_2(19) => wff(14).
	p(NT(19), (NT(14)));
//G10:  __E_rec_relation_2(19) => bf(21).
	p(NT(19), (NT(21)));
//G11:  rec_relation(16)     => ref(18) _(11) ':' '=' _(11) __E_rec_relation_2(19).
	p(NT(16), (NT(18)+NT(11)+T(2)+T(3)+NT(11)+NT(19)));
//G12:  __E_ref_3(23)        => _(11) offsets(24).
	p(NT(23), (NT(11)+NT(24)));
//G13:  __E_ref_3(23)        => null.
	p(NT(23), (nul));
//G14:  ref(18)              => sym(22) __E_ref_3(23) _(11) ref_args(25).
	p(NT(18), (NT(22)+NT(23)+NT(11)+NT(25)));
//G15:  ref_arg(28)          => bf(21).
	p(NT(28), (NT(21)));
//G16:  __E___E_ref_args_4_5(27) => _(11) ref_arg(28).
	p(NT(27), (NT(11)+NT(28)));
//G17:  __E___E_ref_args_4_6(29) => _(11) ',' _(11) ref_arg(28).
	p(NT(29), (NT(11)+T(4)+NT(11)+NT(28)));
//G18:  __E___E_ref_args_4_7(30) => null.
	p(NT(30), (nul));
//G19:  __E___E_ref_args_4_7(30) => __E___E_ref_args_4_6(29) __E___E_ref_args_4_7(30).
	p(NT(30), (NT(29)+NT(30)));
//G20:  __E_ref_args_4(26)   => __E___E_ref_args_4_5(27) __E___E_ref_args_4_7(30).
	p(NT(26), (NT(27)+NT(30)));
//G21:  __E_ref_args_4(26)   => null.
	p(NT(26), (nul));
//G22:  ref_args(25)         => '(' __E_ref_args_4(26) _(11) ')'.
	p(NT(25), (T(5)+NT(26)+NT(11)+T(6)));
//G23:  __E_inputs_9(33)     => _(11) input(34).
	p(NT(33), (NT(11)+NT(34)));
//G24:  __E_inputs_8(32)     => __E_inputs_9(33).
	p(NT(32), (NT(33)));
//G25:  __E_inputs_8(32)     => __E_inputs_9(33) __E_inputs_8(32).
	p(NT(32), (NT(33)+NT(32)));
//G26:  inputs(31)           => _(11) '<' __E_inputs_8(32) _(11) '.'.
	p(NT(31), (NT(11)+T(7)+NT(32)+NT(11)+T(1)));
//G27:  input(34)            => in(35) _(11) ':' _(11) '{' _(11) source_binding(36) _(11) '}'.
	p(NT(34), (NT(35)+NT(11)+T(2)+NT(11)+T(8)+NT(11)+NT(36)+NT(11)+T(9)));
//G28:  __E___E___E_library_10_11_12(42) => wff_rule(43).
	p(NT(42), (NT(43)));
//G29:  __E___E___E_library_10_11_12(42) => bf_rule(44).
	p(NT(42), (NT(44)));
//G30:  rule(41)             => __E___E___E_library_10_11_12(42).
	p(NT(41), (NT(42)));
//G31:  __E___E_library_10_11(40) => _(11) rule(41).
	p(NT(40), (NT(11)+NT(41)));
//G32:  __E___E_library_10_13(45) => null.
	p(NT(45), (nul));
//G33:  __E___E_library_10_13(45) => __E___E_library_10_11(40) __E___E_library_10_13(45).
	p(NT(45), (NT(40)+NT(45)));
//G34:  __E_library_10(39)   => __E___E_library_10_13(45).
	p(NT(39), (NT(45)));
//G35:  rules(38)            => __E_library_10(39).
	p(NT(38), (NT(39)));
//G36:  library(37)          => rules(38).
	p(NT(37), (NT(38)));
//G37:  wff_matcher(46)      => wff(14).
	p(NT(46), (NT(14)));
//G38:  __E_wff_rule_14(48)  => wff(14).
	p(NT(48), (NT(14)));
//G39:  __E_wff_rule_14(48)  => wff_cb(49).
	p(NT(48), (NT(49)));
//G40:  wff_body(47)         => __E_wff_rule_14(48).
	p(NT(47), (NT(48)));
//G41:  wff_rule(43)         => wff_matcher(46) _(11) ':' ':' '=' _(11) wff_body(47) _(11) '.'.
	p(NT(43), (NT(46)+NT(11)+T(2)+T(2)+T(3)+NT(11)+NT(47)+NT(11)+T(1)));
//G42:  bf_matcher(50)       => bf(21).
	p(NT(50), (NT(21)));
//G43:  __E_bf_rule_15(52)   => bf(21).
	p(NT(52), (NT(21)));
//G44:  __E_bf_rule_15(52)   => bf_cb(53).
	p(NT(52), (NT(53)));
//G45:  bf_body(51)          => __E_bf_rule_15(52).
	p(NT(51), (NT(52)));
//G46:  bf_rule(44)          => bf_matcher(50) _(11) ':' '=' _(11) bf_body(51) _(11) '.'.
	p(NT(44), (NT(50)+NT(11)+T(2)+T(3)+NT(11)+NT(51)+NT(11)+T(1)));
//G47:  builder(54)          => _(11) builder_head(55) _(11) builder_body(56) _(11) '.'.
	p(NT(54), (NT(11)+NT(55)+NT(11)+NT(56)+NT(11)+T(1)));
//G48:  __E_builder_head_16(57) => __(58) capture(20).
	p(NT(57), (NT(58)+NT(20)));
//G49:  __E_builder_head_17(59) => null.
	p(NT(59), (nul));
//G50:  __E_builder_head_17(59) => __E_builder_head_16(57) __E_builder_head_17(59).
	p(NT(59), (NT(57)+NT(59)));
//G51:  builder_head(55)     => '(' _(11) capture(20) __E_builder_head_17(59) _(11) ')'.
	p(NT(55), (T(5)+NT(11)+NT(20)+NT(59)+NT(11)+T(6)));
//G52:  __E_builder_body_18(61) => '=' ':' _(11) bf(21).
	p(NT(61), (T(3)+T(2)+NT(11)+NT(21)));
//G53:  bf_builder_body(60)  => __E_builder_body_18(61).
	p(NT(60), (NT(61)));
//G54:  builder_body(56)     => bf_builder_body(60).
	p(NT(56), (NT(60)));
//G55:  __E_builder_body_19(63) => '=' ':' ':' _(11) wff(14).
	p(NT(63), (T(3)+T(2)+T(2)+NT(11)+NT(14)));
//G56:  wff_builder_body(62) => __E_builder_body_19(63).
	p(NT(62), (NT(63)));
//G57:  builder_body(56)     => wff_builder_body(62).
	p(NT(56), (NT(62)));
//G58:  __E_wff_20(65)       => '(' _(11) wff(14) _(11) ')'.
	p(NT(65), (T(5)+NT(11)+NT(14)+NT(11)+T(6)));
//G59:  wff_parenthesis(64)  => __E_wff_20(65).
	p(NT(64), (NT(65)));
//G60:  wff(14)              => wff_parenthesis(64).
	p(NT(14), (NT(64)));
//G61:  __E___E_wff_21_22(68) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(68), (T(10)+T(11)+T(12)+T(13)+T(14)+T(15)+T(12)+T(13)+T(10)));
//G62:  __E___E_wff_21_22(68) => '<' '>'.
	p(NT(68), (T(7)+T(16)));
//G63:  __E_wff_21(67)       => __E___E_wff_21_22(68) _(11) wff(14).
	p(NT(67), (NT(68)+NT(11)+NT(14)));
//G64:  wff_sometimes(66)    => __E_wff_21(67).
	p(NT(66), (NT(67)));
//G65:  wff(14)              => wff_sometimes(66).
	p(NT(14), (NT(66)));
//G66:  __E___E_wff_23_24(71) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(71), (T(17)+T(18)+T(19)+T(17)+T(20)+T(10)));
//G67:  __E___E_wff_23_24(71) => '[' ']'.
	p(NT(71), (T(21)+T(22)));
//G68:  __E_wff_23(70)       => __E___E_wff_23_24(71) _(11) wff(14).
	p(NT(70), (NT(71)+NT(11)+NT(14)));
//G69:  wff_always(69)       => __E_wff_23(70).
	p(NT(69), (NT(70)));
//G70:  wff(14)              => wff_always(69).
	p(NT(14), (NT(69)));
//G71:  __E_wff_25(73)       => wff(14) _(11) '?' _(11) wff(14) _(11) ':' _(11) wff(14).
	p(NT(73), (NT(14)+NT(11)+T(23)+NT(11)+NT(14)+NT(11)+T(2)+NT(11)+NT(14)));
//G72:  wff_conditional(72)  => __E_wff_25(73).
	p(NT(72), (NT(73)));
//G73:  wff(14)              => wff_conditional(72).
	p(NT(14), (NT(72)));
//G74:  __E_wff_26(75)       => 'a' 'l' 'l' __(58) q_vars(76) __(58) wff(14).
	p(NT(75), (T(17)+T(18)+T(18)+NT(58)+NT(76)+NT(58)+NT(14)));
//G75:  wff_all(74)          => __E_wff_26(75).
	p(NT(74), (NT(75)));
//G76:  wff(14)              => wff_all(74).
	p(NT(14), (NT(74)));
//G77:  __E_wff_27(78)       => 'e' 'x' __(58) q_vars(76) __(58) wff(14).
	p(NT(78), (T(13)+T(24)+NT(58)+NT(76)+NT(58)+NT(14)));
//G78:  wff_ex(77)           => __E_wff_27(78).
	p(NT(77), (NT(78)));
//G79:  wff(14)              => wff_ex(77).
	p(NT(14), (NT(77)));
//G80:  wff_ref(79)          => ref(18).
	p(NT(79), (NT(18)));
//G81:  wff(14)              => wff_ref(79).
	p(NT(14), (NT(79)));
//G82:  __E_wff_28(81)       => wff(14) _(11) '-' '>' _(11) wff(14).
	p(NT(81), (NT(14)+NT(11)+T(25)+T(16)+NT(11)+NT(14)));
//G83:  wff_imply(80)        => __E_wff_28(81).
	p(NT(80), (NT(81)));
//G84:  wff(14)              => wff_imply(80).
	p(NT(14), (NT(80)));
//G85:  __E_wff_29(83)       => wff(14) _(11) '<' '-' '>' _(11) wff(14).
	p(NT(83), (NT(14)+NT(11)+T(7)+T(25)+T(16)+NT(11)+NT(14)));
//G86:  wff_equiv(82)        => __E_wff_29(83).
	p(NT(82), (NT(83)));
//G87:  wff(14)              => wff_equiv(82).
	p(NT(14), (NT(82)));
//G88:  __E_wff_30(85)       => wff(14) _(11) '|' '|' _(11) wff(14).
	p(NT(85), (NT(14)+NT(11)+T(26)+T(26)+NT(11)+NT(14)));
//G89:  wff_or(84)           => __E_wff_30(85).
	p(NT(84), (NT(85)));
//G90:  wff(14)              => wff_or(84).
	p(NT(14), (NT(84)));
//G91:  __E_wff_31(87)       => wff(14) _(11) '^' _(11) wff(14).
	p(NT(87), (NT(14)+NT(11)+T(27)+NT(11)+NT(14)));
//G92:  wff_xor(86)          => __E_wff_31(87).
	p(NT(86), (NT(87)));
//G93:  wff(14)              => wff_xor(86).
	p(NT(14), (NT(86)));
//G94:  __E_wff_32(89)       => wff(14) _(11) '&' '&' _(11) wff(14).
	p(NT(89), (NT(14)+NT(11)+T(28)+T(28)+NT(11)+NT(14)));
//G95:  wff_and(88)          => __E_wff_32(89).
	p(NT(88), (NT(89)));
//G96:  wff(14)              => wff_and(88).
	p(NT(14), (NT(88)));
//G97:  __E_wff_33(91)       => '!' _(11) wff(14).
	p(NT(91), (T(29)+NT(11)+NT(14)));
//G98:  wff_neg(90)          => __E_wff_33(91).
	p(NT(90), (NT(91)));
//G99:  wff(14)              => wff_neg(90).
	p(NT(14), (NT(90)));
//G100: wff_t(92)            => 'T'.
	p(NT(92), (T(30)));
//G101: wff(14)              => wff_t(92).
	p(NT(14), (NT(92)));
//G102: wff_f(93)            => 'F'.
	p(NT(93), (T(31)));
//G103: wff(14)              => wff_f(93).
	p(NT(14), (NT(93)));
//G104: __E_wff_34(95)       => bf(21) _(11) '<' '=' _(11) bf(21) _(11) '<' '=' _(11) bf(21).
	p(NT(95), (NT(21)+NT(11)+T(7)+T(3)+NT(11)+NT(21)+NT(11)+T(7)+T(3)+NT(11)+NT(21)));
//G105: bf_interval(94)      => __E_wff_34(95).
	p(NT(94), (NT(95)));
//G106: wff(14)              => bf_interval(94).
	p(NT(14), (NT(94)));
//G107: __E_wff_35(97)       => bf(21) _(11) '!' '=' _(11) bf(21).
	p(NT(97), (NT(21)+NT(11)+T(29)+T(3)+NT(11)+NT(21)));
//G108: bf_neq(96)           => __E_wff_35(97).
	p(NT(96), (NT(97)));
//G109: wff(14)              => bf_neq(96).
	p(NT(14), (NT(96)));
//G110: __E_wff_36(99)       => bf(21) _(11) '=' _(11) bf(21).
	p(NT(99), (NT(21)+NT(11)+T(3)+NT(11)+NT(21)));
//G111: bf_eq(98)            => __E_wff_36(99).
	p(NT(98), (NT(99)));
//G112: wff(14)              => bf_eq(98).
	p(NT(14), (NT(98)));
//G113: __E_wff_37(101)      => bf(21) _(11) '!' '<' '=' _(11) bf(21).
	p(NT(101), (NT(21)+NT(11)+T(29)+T(7)+T(3)+NT(11)+NT(21)));
//G114: bf_nleq(100)         => __E_wff_37(101).
	p(NT(100), (NT(101)));
//G115: wff(14)              => bf_nleq(100).
	p(NT(14), (NT(100)));
//G116: __E_wff_38(103)      => bf(21) _(11) '>' _(11) bf(21).
	p(NT(103), (NT(21)+NT(11)+T(16)+NT(11)+NT(21)));
//G117: bf_greater(102)      => __E_wff_38(103).
	p(NT(102), (NT(103)));
//G118: wff(14)              => bf_greater(102).
	p(NT(14), (NT(102)));
//G119: __E_wff_39(105)      => bf(21) _(11) '<' '=' _(11) bf(21).
	p(NT(105), (NT(21)+NT(11)+T(7)+T(3)+NT(11)+NT(21)));
//G120: bf_less_equal(104)   => __E_wff_39(105).
	p(NT(104), (NT(105)));
//G121: wff(14)              => bf_less_equal(104).
	p(NT(14), (NT(104)));
//G122: __E_wff_40(107)      => bf(21) _(11) '<' _(11) bf(21).
	p(NT(107), (NT(21)+NT(11)+T(7)+NT(11)+NT(21)));
//G123: bf_less(106)         => __E_wff_40(107).
	p(NT(106), (NT(107)));
//G124: wff(14)              => bf_less(106).
	p(NT(14), (NT(106)));
//G125: wff(14)              => capture(20).
	p(NT(14), (NT(20)));
//G126: wff(14)              => uninterpreted_constant(108).
	p(NT(14), (NT(108)));
//G127: __E_bf_41(110)       => '(' _(11) bf(21) _(11) ')'.
	p(NT(110), (T(5)+NT(11)+NT(21)+NT(11)+T(6)));
//G128: bf_parenthesis(109)  => __E_bf_41(110).
	p(NT(109), (NT(110)));
//G129: bf(21)               => bf_parenthesis(109).
	p(NT(21), (NT(109)));
//G130: bf(21)               => variable(111).
	p(NT(21), (NT(111)));
//G131: __E_bf_42(113)       => 'S' _(11) '(' _(11) bf(21) _(11) ')'.
	p(NT(113), (T(32)+NT(11)+T(5)+NT(11)+NT(21)+NT(11)+T(6)));
//G132: bf_splitter(112)     => __E_bf_42(113).
	p(NT(112), (NT(113)));
//G133: bf(21)               => bf_splitter(112).
	p(NT(21), (NT(112)));
//G134: bf_ref(114)          => ref(18).
	p(NT(114), (NT(18)));
//G135: bf(21)               => bf_ref(114).
	p(NT(21), (NT(114)));
//G136: __E_bf_43(116)       => bf(21) _(11) '|' _(11) bf(21).
	p(NT(116), (NT(21)+NT(11)+T(26)+NT(11)+NT(21)));
//G137: bf_or(115)           => __E_bf_43(116).
	p(NT(115), (NT(116)));
//G138: bf(21)               => bf_or(115).
	p(NT(21), (NT(115)));
//G139: __E_bf_44(118)       => bf(21) _(11) '+' _(11) bf(21).
	p(NT(118), (NT(21)+NT(11)+T(33)+NT(11)+NT(21)));
//G140: bf_xor(117)          => __E_bf_44(118).
	p(NT(117), (NT(118)));
//G141: bf(21)               => bf_xor(117).
	p(NT(21), (NT(117)));
//G142: __E___E_bf_45_46(121) => _(11).
	p(NT(121), (NT(11)));
//G143: __E___E_bf_45_46(121) => _(11) '&' _(11).
	p(NT(121), (NT(11)+T(28)+NT(11)));
//G144: __E_bf_45(120)       => bf(21) __E___E_bf_45_46(121) bf(21).
	p(NT(120), (NT(21)+NT(121)+NT(21)));
//G145: bf_and(119)          => __E_bf_45(120).
	p(NT(119), (NT(120)));
//G146: bf(21)               => bf_and(119).
	p(NT(21), (NT(119)));
//G147: __E_bf_47(123)       => bf(21) _(11) '\''.
	p(NT(123), (NT(21)+NT(11)+T(34)));
//G148: bf_neg(122)          => __E_bf_47(123).
	p(NT(122), (NT(123)));
//G149: bf(21)               => bf_neg(122).
	p(NT(21), (NT(122)));
//G150: __E_bf_48(125)       => '{' _(11) constant(126) _(11) '}'.
	p(NT(125), (T(8)+NT(11)+NT(126)+NT(11)+T(9)));
//G151: bf_constant(124)     => __E_bf_48(125).
	p(NT(124), (NT(125)));
//G152: bf(21)               => bf_constant(124).
	p(NT(21), (NT(124)));
//G153: bf_t(127)            => '1'.
	p(NT(127), (T(35)));
//G154: bf(21)               => bf_t(127).
	p(NT(21), (NT(127)));
//G155: bf_f(128)            => '0'.
	p(NT(128), (T(36)));
//G156: bf(21)               => bf_f(128).
	p(NT(21), (NT(128)));
//G157: bf(21)               => flag(129).
	p(NT(21), (NT(129)));
//G158: bf(21)               => capture(20).
	p(NT(21), (NT(20)));
//G159: __E_flag_49(131)     => flagvar(132) _(11) '!' '=' _(11) num(133).
	p(NT(131), (NT(132)+NT(11)+T(29)+T(3)+NT(11)+NT(133)));
//G160: __E_flag_49(131)     => num(133) _(11) '!' '=' _(11) flagvar(132).
	p(NT(131), (NT(133)+NT(11)+T(29)+T(3)+NT(11)+NT(132)));
//G161: flag_neq(130)        => __E_flag_49(131).
	p(NT(130), (NT(131)));
//G162: flag(129)            => flag_neq(130).
	p(NT(129), (NT(130)));
//G163: __E_flag_50(135)     => flagvar(132) _(11) '=' _(11) num(133).
	p(NT(135), (NT(132)+NT(11)+T(3)+NT(11)+NT(133)));
//G164: __E_flag_50(135)     => num(133) _(11) '=' _(11) flagvar(132).
	p(NT(135), (NT(133)+NT(11)+T(3)+NT(11)+NT(132)));
//G165: flag_eq(134)         => __E_flag_50(135).
	p(NT(134), (NT(135)));
//G166: flag(129)            => flag_eq(134).
	p(NT(129), (NT(134)));
//G167: __E_flag_51(137)     => flagvar(132) _(11) '>' '=' _(11) num(133).
	p(NT(137), (NT(132)+NT(11)+T(16)+T(3)+NT(11)+NT(133)));
//G168: __E_flag_51(137)     => num(133) _(11) '>' '=' _(11) flagvar(132).
	p(NT(137), (NT(133)+NT(11)+T(16)+T(3)+NT(11)+NT(132)));
//G169: flag_greater_equal(136) => __E_flag_51(137).
	p(NT(136), (NT(137)));
//G170: flag(129)            => flag_greater_equal(136).
	p(NT(129), (NT(136)));
//G171: __E_flag_52(139)     => flagvar(132) _(11) '>' _(11) num(133).
	p(NT(139), (NT(132)+NT(11)+T(16)+NT(11)+NT(133)));
//G172: __E_flag_52(139)     => num(133) _(11) '>' _(11) flagvar(132).
	p(NT(139), (NT(133)+NT(11)+T(16)+NT(11)+NT(132)));
//G173: flag_greater(138)    => __E_flag_52(139).
	p(NT(138), (NT(139)));
//G174: flag(129)            => flag_greater(138).
	p(NT(129), (NT(138)));
//G175: __E_flag_53(141)     => flagvar(132) _(11) '<' '=' _(11) num(133).
	p(NT(141), (NT(132)+NT(11)+T(7)+T(3)+NT(11)+NT(133)));
//G176: __E_flag_53(141)     => num(133) _(11) '<' '=' _(11) flagvar(132).
	p(NT(141), (NT(133)+NT(11)+T(7)+T(3)+NT(11)+NT(132)));
//G177: flag_less_equal(140) => __E_flag_53(141).
	p(NT(140), (NT(141)));
//G178: flag(129)            => flag_less_equal(140).
	p(NT(129), (NT(140)));
//G179: __E_flag_54(143)     => flagvar(132) _(11) '<' _(11) num(133).
	p(NT(143), (NT(132)+NT(11)+T(7)+NT(11)+NT(133)));
//G180: __E_flag_54(143)     => num(133) _(11) '<' _(11) flagvar(132).
	p(NT(143), (NT(133)+NT(11)+T(7)+NT(11)+NT(132)));
//G181: flag_less(142)       => __E_flag_54(143).
	p(NT(142), (NT(143)));
//G182: flag(129)            => flag_less(142).
	p(NT(129), (NT(142)));
//G183: __N_0(360)           => 'F'.
	p(NT(360), (T(31)));
//G184: __N_1(361)           => 'T'.
	p(NT(361), (T(30)));
//G185: __E_charvar_55(145)  => ~( __N_0(360) ) & ~( __N_1(361) ) & alpha(5).	 # conjunctive
	p(NT(145), ~(NT(360)) & ~(NT(361)) & (NT(5)));
//G186: __E_charvar_56(146)  => null.
	p(NT(146), (nul));
//G187: __E_charvar_56(146)  => digit(3) __E_charvar_56(146).
	p(NT(146), (NT(3)+NT(146)));
//G188: charvar(144)         => __E_charvar_55(145) __E_charvar_56(146).
	p(NT(144), (NT(145)+NT(146)));
//G189: flagvar(132)         => charvar(144).
	p(NT(132), (NT(144)));
//G190: variable(111)        => charvar(144).
	p(NT(111), (NT(144)));
//G191: __E_variable_57(148) => in(35).
	p(NT(148), (NT(35)));
//G192: __E_variable_57(148) => out(149).
	p(NT(148), (NT(149)));
//G193: io_var(147)          => __E_variable_57(148).
	p(NT(147), (NT(148)));
//G194: variable(111)        => io_var(147).
	p(NT(111), (NT(147)));
//G195: __E_in_58(151)       => 'i' digits(152).
	p(NT(151), (T(15)+NT(152)));
//G196: in_var_name(150)     => __E_in_58(151).
	p(NT(150), (NT(151)));
//G197: in(35)               => in_var_name(150) '[' offset(153) ']'.
	p(NT(35), (NT(150)+T(21)+NT(153)+T(22)));
//G198: __E_out_59(155)      => 'o' digits(152).
	p(NT(155), (T(11)+NT(152)));
//G199: out_var_name(154)    => __E_out_59(155).
	p(NT(154), (NT(155)));
//G200: out(149)             => out_var_name(154) '[' offset(153) ']'.
	p(NT(149), (NT(154)+T(21)+NT(153)+T(22)));
//G201: capture(20)          => '$' charvar(144).
	p(NT(20), (T(37)+NT(144)));
//G202: __E_q_vars_60(157)   => _(11) ',' _(11) q_var(156).
	p(NT(157), (NT(11)+T(4)+NT(11)+NT(156)));
//G203: __E_q_vars_61(158)   => null.
	p(NT(158), (nul));
//G204: __E_q_vars_61(158)   => __E_q_vars_60(157) __E_q_vars_61(158).
	p(NT(158), (NT(157)+NT(158)));
//G205: q_vars(76)           => q_var(156) __E_q_vars_61(158).
	p(NT(76), (NT(156)+NT(158)));
//G206: q_var(156)           => capture(20).
	p(NT(156), (NT(20)));
//G207: q_var(156)           => variable(111).
	p(NT(156), (NT(111)));
//G208: __E_offsets_62(159)  => _(11) ',' _(11) offset(153).
	p(NT(159), (NT(11)+T(4)+NT(11)+NT(153)));
//G209: __E_offsets_63(160)  => null.
	p(NT(160), (nul));
//G210: __E_offsets_63(160)  => __E_offsets_62(159) __E_offsets_63(160).
	p(NT(160), (NT(159)+NT(160)));
//G211: offsets(24)          => '[' _(11) offset(153) __E_offsets_63(160) _(11) ']'.
	p(NT(24), (T(21)+NT(11)+NT(153)+NT(160)+NT(11)+T(22)));
//G212: offset(153)          => num(133).
	p(NT(153), (NT(133)));
//G213: offset(153)          => capture(20).
	p(NT(153), (NT(20)));
//G214: offset(153)          => shift(161).
	p(NT(153), (NT(161)));
//G215: __N_2(362)           => io_var(147).
	p(NT(362), (NT(147)));
//G216: __E_offset_64(162)   => variable(111) & ~( __N_2(362) ).	 # conjunctive
	p(NT(162), (NT(111)) & ~(NT(362)));
//G217: offset(153)          => __E_offset_64(162).
	p(NT(153), (NT(162)));
//G218: __E_shift_65(163)    => capture(20).
	p(NT(163), (NT(20)));
//G219: __N_3(363)           => io_var(147).
	p(NT(363), (NT(147)));
//G220: __E___E_shift_65_66(164) => variable(111) & ~( __N_3(363) ).	 # conjunctive
	p(NT(164), (NT(111)) & ~(NT(363)));
//G221: __E_shift_65(163)    => __E___E_shift_65_66(164).
	p(NT(163), (NT(164)));
//G222: shift(161)           => __E_shift_65(163) _(11) '-' _(11) num(133).
	p(NT(161), (NT(163)+NT(11)+T(25)+NT(11)+NT(133)));
//G223: num(133)             => digits(152).
	p(NT(133), (NT(152)));
//G224: __E___E_uninterpreted_constant_67_68(167) => chars(168).
	p(NT(167), (NT(168)));
//G225: __E___E_uninterpreted_constant_67_68(167) => _(11).
	p(NT(167), (NT(11)));
//G226: __E_uninterpreted_constant_67(166) => __E___E_uninterpreted_constant_67_68(167) ':' chars(168).
	p(NT(166), (NT(167)+T(2)+NT(168)));
//G227: uninter_const_name(165) => __E_uninterpreted_constant_67(166).
	p(NT(165), (NT(166)));
//G228: uninterpreted_constant(108) => '<' uninter_const_name(165) _(11) '>'.
	p(NT(108), (T(7)+NT(165)+NT(11)+T(16)));
//G229: constant(126)        => binding(169).
	p(NT(126), (NT(169)));
//G230: constant(126)        => capture(20).
	p(NT(126), (NT(20)));
//G231: __E___E_binding_69_70(172) => chars(168) _(11).
	p(NT(172), (NT(168)+NT(11)));
//G232: __E___E_binding_69_70(172) => _(11).
	p(NT(172), (NT(11)));
//G233: type(171)            => __E___E_binding_69_70(172).
	p(NT(171), (NT(172)));
//G234: __E_binding_69(170)  => type(171) ':' _(11) source(173).
	p(NT(170), (NT(171)+T(2)+NT(11)+NT(173)));
//G235: source_binding(36)   => __E_binding_69(170).
	p(NT(36), (NT(170)));
//G236: binding(169)         => source_binding(36).
	p(NT(169), (NT(36)));
//G237: named_binding(174)   => chars(168).
	p(NT(174), (NT(168)));
//G238: binding(169)         => named_binding(174).
	p(NT(169), (NT(174)));
//G239: __E_source_71(176)   => alnum(6).
	p(NT(176), (NT(6)));
//G240: __E_source_71(176)   => punct(7).
	p(NT(176), (NT(7)));
//G241: src_c(175)           => __E_source_71(176).
	p(NT(175), (NT(176)));
//G242: __E___E_source_72_73(178) => src_c(175).
	p(NT(178), (NT(175)));
//G243: __E___E_source_72_73(178) => space(2).
	p(NT(178), (NT(2)));
//G244: __E___E_source_72_74(179) => null.
	p(NT(179), (nul));
//G245: __E___E_source_72_74(179) => __E___E_source_72_73(178) __E___E_source_72_74(179).
	p(NT(179), (NT(178)+NT(179)));
//G246: __E_source_72(177)   => __E___E_source_72_74(179) src_c(175).
	p(NT(177), (NT(179)+NT(175)));
//G247: __E_source_72(177)   => null.
	p(NT(177), (nul));
//G248: source(173)          => src_c(175) __E_source_72(177).
	p(NT(173), (NT(175)+NT(177)));
//G249: __E_chars_75(180)    => null.
	p(NT(180), (nul));
//G250: __E_chars_75(180)    => alnum(6) __E_chars_75(180).
	p(NT(180), (NT(6)+NT(180)));
//G251: chars(168)           => alpha(5) __E_chars_75(180).
	p(NT(168), (NT(5)+NT(180)));
//G252: __E_digits_76(181)   => digit(3).
	p(NT(181), (NT(3)));
//G253: __E_digits_76(181)   => digit(3) __E_digits_76(181).
	p(NT(181), (NT(3)+NT(181)));
//G254: digits(152)          => __E_digits_76(181).
	p(NT(152), (NT(181)));
//G255: sym(22)              => chars(168).
	p(NT(22), (NT(168)));
//G256: __E_comment_77(183)  => printable(8).
	p(NT(183), (NT(8)));
//G257: __E_comment_77(183)  => '\t'.
	p(NT(183), (T(38)));
//G258: __E_comment_78(184)  => null.
	p(NT(184), (nul));
//G259: __E_comment_78(184)  => __E_comment_77(183) __E_comment_78(184).
	p(NT(184), (NT(183)+NT(184)));
//G260: __E_comment_79(185)  => '\n'.
	p(NT(185), (T(39)));
//G261: __E_comment_79(185)  => '\r'.
	p(NT(185), (T(40)));
//G262: __E_comment_79(185)  => eof(1).
	p(NT(185), (NT(1)));
//G263: comment(182)         => '#' __E_comment_78(184) __E_comment_79(185).
	p(NT(182), (T(41)+NT(184)+NT(185)));
//G264: __E____80(186)       => space(2).
	p(NT(186), (NT(2)));
//G265: __E____80(186)       => comment(182).
	p(NT(186), (NT(182)));
//G266: __(58)               => __E____80(186) _(11).
	p(NT(58), (NT(186)+NT(11)));
//G267: __E___81(187)        => __(58).
	p(NT(187), (NT(58)));
//G268: __E___81(187)        => null.
	p(NT(187), (nul));
//G269: _(11)                => __E___81(187).
	p(NT(11), (NT(187)));
//G270: bf_cb_arg(189)       => bf(21).
	p(NT(189), (NT(21)));
//G271: bf_cb_args1(188)     => __(58) bf_cb_arg(189).
	p(NT(188), (NT(58)+NT(189)));
//G272: bf_cb_args2(190)     => bf_cb_args1(188) bf_cb_args1(188).
	p(NT(190), (NT(188)+NT(188)));
//G273: bf_cb_args3(191)     => bf_cb_args2(190) bf_cb_args1(188).
	p(NT(191), (NT(190)+NT(188)));
//G274: bf_cb_args4(192)     => bf_cb_args3(191) bf_cb_args1(188).
	p(NT(192), (NT(191)+NT(188)));
//G275: wff_cb_arg(194)      => wff(14).
	p(NT(194), (NT(14)));
//G276: wff_cb_args1(193)    => __(58) wff_cb_arg(194).
	p(NT(193), (NT(58)+NT(194)));
//G277: wff_cb_args2(195)    => wff_cb_args1(193) wff_cb_args1(193).
	p(NT(195), (NT(193)+NT(193)));
//G278: wff_cb_args3(196)    => wff_cb_args2(195) wff_cb_args1(193).
	p(NT(196), (NT(195)+NT(193)));
//G279: wff_cb_args4(197)    => wff_cb_args3(196) wff_cb_args1(193).
	p(NT(197), (NT(196)+NT(193)));
//G280: __E_wff_cb_82(199)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(195).
	p(NT(199), (T(19)+T(42)+T(42)+T(43)+T(44)+T(17)+T(10)+T(43)+T(45)+T(18)+T(17)+T(10)+T(44)+T(15)+T(46)+T(47)+T(43)+T(10)+T(48)+T(49)+T(42)+T(11)+T(50)+T(12)+T(48)+T(18)+T(17)+T(10)+T(43)+T(45)+T(49)+NT(195)));
//G281: wff_has_clashing_subformulas_cb(198) => __E_wff_cb_82(199).
	p(NT(198), (NT(199)));
//G282: wff_cb(49)           => wff_has_clashing_subformulas_cb(198).
	p(NT(49), (NT(198)));
//G283: __E_wff_cb_83(201)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(196).
	p(NT(201), (T(19)+T(42)+T(42)+T(43)+T(44)+T(17)+T(10)+T(43)+T(10)+T(48)+T(49)+T(42)+T(11)+T(50)+T(12)+T(48)+T(18)+T(17)+T(43)+T(45)+T(49)+NT(196)));
//G284: wff_has_subformula_cb(200) => __E_wff_cb_83(201).
	p(NT(200), (NT(201)));
//G285: wff_cb(49)           => wff_has_subformula_cb(200).
	p(NT(49), (NT(200)));
//G286: __E_wff_cb_84(203)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(195).
	p(NT(203), (T(19)+T(42)+T(42)+T(43)+T(50)+T(13)+T(12)+T(11)+T(51)+T(13)+T(43)+T(13)+T(24)+T(15)+T(10)+T(14)+T(13)+T(46)+T(14)+T(15)+T(17)+T(18)+T(43)+T(45)+T(49)+NT(195)));
//G287: wff_remove_existential_cb(202) => __E_wff_cb_84(203).
	p(NT(202), (NT(203)));
//G288: wff_cb(49)           => wff_remove_existential_cb(202).
	p(NT(49), (NT(202)));
//G289: __E_bf_cb_85(205)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(191).
	p(NT(205), (T(49)+T(42)+T(43)+T(44)+T(17)+T(10)+T(43)+T(10)+T(48)+T(49)+T(42)+T(11)+T(50)+T(12)+T(48)+T(18)+T(17)+T(43)+T(45)+T(49)+NT(191)));
//G290: bf_has_subformula_cb(204) => __E_bf_cb_85(205).
	p(NT(204), (NT(205)));
//G291: bf_cb(53)            => bf_has_subformula_cb(204).
	p(NT(53), (NT(204)));
//G292: __E_bf_cb_86(207)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(192).
	p(NT(207), (T(49)+T(42)+T(43)+T(50)+T(13)+T(12)+T(11)+T(51)+T(13)+T(43)+T(42)+T(48)+T(46)+T(15)+T(51)+T(13)+T(50)+T(10)+T(17)+T(18)+T(43)+T(45)+T(49)+NT(192)));
//G293: bf_remove_funiversal_cb(206) => __E_bf_cb_86(207).
	p(NT(206), (NT(207)));
//G294: bf_cb(53)            => bf_remove_funiversal_cb(206).
	p(NT(53), (NT(206)));
//G295: __E_bf_cb_87(209)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(192).
	p(NT(209), (T(49)+T(42)+T(43)+T(50)+T(13)+T(12)+T(11)+T(51)+T(13)+T(43)+T(42)+T(13)+T(24)+T(15)+T(10)+T(14)+T(13)+T(46)+T(14)+T(15)+T(17)+T(18)+T(43)+T(45)+T(49)+NT(192)));
//G296: bf_remove_fexistential_cb(208) => __E_bf_cb_87(209).
	p(NT(208), (NT(209)));
//G297: bf_cb(53)            => bf_remove_fexistential_cb(208).
	p(NT(53), (NT(208)));
//G298: __E_bf_cb_88(211)    => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(188).
	p(NT(211), (T(49)+T(42)+T(43)+T(46)+T(11)+T(50)+T(12)+T(17)+T(18)+T(15)+T(52)+T(13)+T(43)+T(45)+T(49)+NT(188)));
//G299: bf_normalize_cb(210) => __E_bf_cb_88(211).
	p(NT(210), (NT(211)));
//G300: bf_cb(53)            => bf_normalize_cb(210).
	p(NT(53), (NT(210)));
//G301: cli(212)             => _(11).
	p(NT(212), (NT(11)));
//G302: __E_cli_89(214)      => '.' _(11) cli_command(213) _(11).
	p(NT(214), (T(1)+NT(11)+NT(213)+NT(11)));
//G303: __E_cli_90(215)      => null.
	p(NT(215), (nul));
//G304: __E_cli_90(215)      => __E_cli_89(214) __E_cli_90(215).
	p(NT(215), (NT(214)+NT(215)));
//G305: cli(212)             => _(11) cli_command(213) _(11) __E_cli_90(215).
	p(NT(212), (NT(11)+NT(213)+NT(11)+NT(215)));
//G306: __E_cli_command_91(218) => 'q'.
	p(NT(218), (T(53)));
//G307: __E_cli_command_91(218) => 'q' 'u' 'i' 't'.
	p(NT(218), (T(53)+T(48)+T(15)+T(14)));
//G308: quit_sym(217)        => __E_cli_command_91(218).
	p(NT(217), (NT(218)));
//G309: quit_cmd(216)        => quit_sym(217).
	p(NT(216), (NT(217)));
//G310: cli_command(213)     => quit_cmd(216).
	p(NT(213), (NT(216)));
//G311: __E_cli_command_92(221) => 'v'.
	p(NT(221), (T(51)));
//G312: __E_cli_command_92(221) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(221), (T(51)+T(13)+T(50)+T(10)+T(15)+T(11)+T(46)));
//G313: version_sym(220)     => __E_cli_command_92(221).
	p(NT(220), (NT(221)));
//G314: version_cmd(219)     => version_sym(220).
	p(NT(219), (NT(220)));
//G315: cli_command(213)     => version_cmd(219).
	p(NT(213), (NT(219)));
//G316: __E_cli_command_93(224) => 'c'.
	p(NT(224), (T(45)));
//G317: __E_cli_command_93(224) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(224), (T(45)+T(18)+T(13)+T(17)+T(50)));
//G318: clear_sym(223)       => __E_cli_command_93(224).
	p(NT(223), (NT(224)));
//G319: clear_cmd(222)       => clear_sym(223).
	p(NT(222), (NT(223)));
//G320: cli_command(213)     => clear_cmd(222).
	p(NT(213), (NT(222)));
//G321: __E___E_cli_command_94_95(228) => 'h'.
	p(NT(228), (T(44)));
//G322: __E___E_cli_command_94_95(228) => 'h' 'e' 'l' 'p'.
	p(NT(228), (T(44)+T(13)+T(18)+T(54)));
//G323: help_sym(227)        => __E___E_cli_command_94_95(228).
	p(NT(227), (NT(228)));
//G324: __E___E_cli_command_94_96(229) => __(58) help_arg(230).
	p(NT(229), (NT(58)+NT(230)));
//G325: __E___E_cli_command_94_96(229) => null.
	p(NT(229), (nul));
//G326: __E_cli_command_94(226) => help_sym(227) __E___E_cli_command_94_96(229).
	p(NT(226), (NT(227)+NT(229)));
//G327: help_cmd(225)        => __E_cli_command_94(226).
	p(NT(225), (NT(226)));
//G328: cli_command(213)     => help_cmd(225).
	p(NT(213), (NT(225)));
//G329: file_sym(233)        => 'f' 'i' 'l' 'e'.
	p(NT(233), (T(42)+T(15)+T(18)+T(13)));
//G330: __E_cli_command_97(232) => file_sym(233) __(58) q_string(234).
	p(NT(232), (NT(233)+NT(58)+NT(234)));
//G331: file_cmd(231)        => __E_cli_command_97(232).
	p(NT(231), (NT(232)));
//G332: cli_command(213)     => file_cmd(231).
	p(NT(213), (NT(231)));
//G333: valid_sym(237)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(237), (T(51)+T(17)+T(18)+T(15)+T(55)));
//G334: __E_cli_command_98(236) => valid_sym(237) __(58) rr(10).
	p(NT(236), (NT(237)+NT(58)+NT(10)));
//G335: valid_cmd(235)       => __E_cli_command_98(236).
	p(NT(235), (NT(236)));
//G336: cli_command(213)     => valid_cmd(235).
	p(NT(213), (NT(235)));
//G337: sat_sym(240)         => 's' 'a' 't'.
	p(NT(240), (T(10)+T(17)+T(14)));
//G338: __E_cli_command_99(239) => sat_sym(240) __(58) normalize_cmd_arg(241).
	p(NT(239), (NT(240)+NT(58)+NT(241)));
//G339: sat_cmd(238)         => __E_cli_command_99(239).
	p(NT(238), (NT(239)));
//G340: cli_command(213)     => sat_cmd(238).
	p(NT(213), (NT(238)));
//G341: unsat_sym(244)       => 'u' 'n' 's' 'a' 't'.
	p(NT(244), (T(48)+T(46)+T(10)+T(17)+T(14)));
//G342: __E_cli_command_100(243) => unsat_sym(244) __(58) rr(10).
	p(NT(243), (NT(244)+NT(58)+NT(10)));
//G343: unsat_cmd(242)       => __E_cli_command_100(243).
	p(NT(242), (NT(243)));
//G344: cli_command(213)     => unsat_cmd(242).
	p(NT(213), (NT(242)));
//G345: solve_sym(247)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(247), (T(10)+T(11)+T(18)+T(51)+T(13)));
//G346: __E_cli_command_101(246) => solve_sym(247) __(58) wff_cmd_arg(248).
	p(NT(246), (NT(247)+NT(58)+NT(248)));
//G347: solve_cmd(245)       => __E_cli_command_101(246).
	p(NT(245), (NT(246)));
//G348: cli_command(213)     => solve_cmd(245).
	p(NT(213), (NT(245)));
//G349: __E___E_cli_command_102_103(252) => 'e'.
	p(NT(252), (T(13)));
//G350: __E___E_cli_command_102_103(252) => 'e' 'x' 'e' 'c' 'u' 't' 'e'.
	p(NT(252), (T(13)+T(24)+T(13)+T(45)+T(48)+T(14)+T(13)));
//G351: execute_sym(251)     => __E___E_cli_command_102_103(252).
	p(NT(251), (NT(252)));
//G352: __E___E_cli_command_102_104(253) => wff(14).
	p(NT(253), (NT(14)));
//G353: __E___E_cli_command_102_104(253) => rr(10).
	p(NT(253), (NT(10)));
//G354: __E_cli_command_102(250) => execute_sym(251) __(58) __E___E_cli_command_102_104(253).
	p(NT(250), (NT(251)+NT(58)+NT(253)));
//G355: execute_cmd(249)     => __E_cli_command_102(250).
	p(NT(249), (NT(250)));
//G356: cli_command(213)     => execute_cmd(249).
	p(NT(213), (NT(249)));
//G357: __E___E_cli_command_105_106(257) => 'n'.
	p(NT(257), (T(46)));
//G358: __E___E_cli_command_105_106(257) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(257), (T(46)+T(11)+T(50)+T(12)+T(17)+T(18)+T(15)+T(52)+T(13)));
//G359: normalize_sym(256)   => __E___E_cli_command_105_106(257).
	p(NT(256), (NT(257)));
//G360: __E_cli_command_105(255) => normalize_sym(256) __(58) normalize_cmd_arg(241).
	p(NT(255), (NT(256)+NT(58)+NT(241)));
//G361: normalize_cmd(254)   => __E_cli_command_105(255).
	p(NT(254), (NT(255)));
//G362: cli_command(213)     => normalize_cmd(254).
	p(NT(213), (NT(254)));
//G363: __E___E_cli_command_107_108(261) => 's'.
	p(NT(261), (T(10)));
//G364: __E___E_cli_command_107_108(261) => 's' 'u' 'b' 's' 't'.
	p(NT(261), (T(10)+T(48)+T(49)+T(10)+T(14)));
//G365: __E___E_cli_command_107_108(261) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(261), (T(10)+T(48)+T(49)+T(10)+T(14)+T(15)+T(14)+T(48)+T(14)+T(13)));
//G366: subst_sym(260)       => __E___E_cli_command_107_108(261).
	p(NT(260), (NT(261)));
//G367: __E_cli_command_107(259) => subst_sym(260) __(58) nf_cmd_arg(262) _(11) '[' _(11) nf_cmd_arg(262) _(11) '/' _(11) nf_cmd_arg(262) _(11) ']'.
	p(NT(259), (NT(260)+NT(58)+NT(262)+NT(11)+T(21)+NT(11)+NT(262)+NT(11)+T(56)+NT(11)+NT(262)+NT(11)+T(22)));
//G368: subst_cmd(258)       => __E_cli_command_107(259).
	p(NT(258), (NT(259)));
//G369: cli_command(213)     => subst_cmd(258).
	p(NT(213), (NT(258)));
//G370: __E___E_cli_command_109_110(266) => 'i'.
	p(NT(266), (T(15)));
//G371: __E___E_cli_command_109_110(266) => 'i' 'n' 's' 't'.
	p(NT(266), (T(15)+T(46)+T(10)+T(14)));
//G372: __E___E_cli_command_109_110(266) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(266), (T(15)+T(46)+T(10)+T(14)+T(17)+T(46)+T(14)+T(15)+T(17)+T(14)+T(13)));
//G373: inst_sym(265)        => __E___E_cli_command_109_110(266).
	p(NT(265), (NT(266)));
//G374: __E_cli_command_109(264) => inst_sym(265) __(58) inst_args(267).
	p(NT(264), (NT(265)+NT(58)+NT(267)));
//G375: inst_cmd(263)        => __E_cli_command_109(264).
	p(NT(263), (NT(264)));
//G376: cli_command(213)     => inst_cmd(263).
	p(NT(213), (NT(263)));
//G377: dnf_sym(270)         => 'd' 'n' 'f'.
	p(NT(270), (T(55)+T(46)+T(42)));
//G378: __E_cli_command_111(269) => dnf_sym(270) __(58) nf_cmd_arg(262).
	p(NT(269), (NT(270)+NT(58)+NT(262)));
//G379: dnf_cmd(268)         => __E_cli_command_111(269).
	p(NT(268), (NT(269)));
//G380: cli_command(213)     => dnf_cmd(268).
	p(NT(213), (NT(268)));
//G381: cnf_sym(273)         => 'c' 'n' 'f'.
	p(NT(273), (T(45)+T(46)+T(42)));
//G382: __E_cli_command_112(272) => cnf_sym(273) __(58) nf_cmd_arg(262).
	p(NT(272), (NT(273)+NT(58)+NT(262)));
//G383: cnf_cmd(271)         => __E_cli_command_112(272).
	p(NT(271), (NT(272)));
//G384: cli_command(213)     => cnf_cmd(271).
	p(NT(213), (NT(271)));
//G385: anf_sym(276)         => 'a' 'n' 'f'.
	p(NT(276), (T(17)+T(46)+T(42)));
//G386: __E_cli_command_113(275) => anf_sym(276) __(58) nf_cmd_arg(262).
	p(NT(275), (NT(276)+NT(58)+NT(262)));
//G387: anf_cmd(274)         => __E_cli_command_113(275).
	p(NT(274), (NT(275)));
//G388: cli_command(213)     => anf_cmd(274).
	p(NT(213), (NT(274)));
//G389: nnf_sym(279)         => 'n' 'n' 'f'.
	p(NT(279), (T(46)+T(46)+T(42)));
//G390: __E_cli_command_114(278) => nnf_sym(279) __(58) nf_cmd_arg(262).
	p(NT(278), (NT(279)+NT(58)+NT(262)));
//G391: nnf_cmd(277)         => __E_cli_command_114(278).
	p(NT(277), (NT(278)));
//G392: cli_command(213)     => nnf_cmd(277).
	p(NT(213), (NT(277)));
//G393: pnf_sym(282)         => 'p' 'n' 'f'.
	p(NT(282), (T(54)+T(46)+T(42)));
//G394: __E_cli_command_115(281) => pnf_sym(282) __(58) nf_cmd_arg(262).
	p(NT(281), (NT(282)+NT(58)+NT(262)));
//G395: pnf_cmd(280)         => __E_cli_command_115(281).
	p(NT(280), (NT(281)));
//G396: cli_command(213)     => pnf_cmd(280).
	p(NT(213), (NT(280)));
//G397: mnf_sym(285)         => 'm' 'n' 'f'.
	p(NT(285), (T(12)+T(46)+T(42)));
//G398: __E_cli_command_116(284) => mnf_sym(285) __(58) nf_cmd_arg(262).
	p(NT(284), (NT(285)+NT(58)+NT(262)));
//G399: mnf_cmd(283)         => __E_cli_command_116(284).
	p(NT(283), (NT(284)));
//G400: cli_command(213)     => mnf_cmd(283).
	p(NT(213), (NT(283)));
//G401: snf_sym(288)         => 's' 'n' 'f'.
	p(NT(288), (T(10)+T(46)+T(42)));
//G402: __E_cli_command_117(287) => snf_sym(288) __(58) nf_cmd_arg(262).
	p(NT(287), (NT(288)+NT(58)+NT(262)));
//G403: snf_cmd(286)         => __E_cli_command_117(287).
	p(NT(286), (NT(287)));
//G404: cli_command(213)     => snf_cmd(286).
	p(NT(213), (NT(286)));
//G405: onf_sym(291)         => 'o' 'n' 'f'.
	p(NT(291), (T(11)+T(46)+T(42)));
//G406: __E_cli_command_118(290) => onf_sym(291) __(58) variable(111) __(58) onf_cmd_arg(292).
	p(NT(290), (NT(291)+NT(58)+NT(111)+NT(58)+NT(292)));
//G407: onf_cmd(289)         => __E_cli_command_118(290).
	p(NT(289), (NT(290)));
//G408: cli_command(213)     => onf_cmd(289).
	p(NT(213), (NT(289)));
//G409: __E___E_cli_command_119_120(296) => 'd' 'e' 'f' 's'.
	p(NT(296), (T(55)+T(13)+T(42)+T(10)));
//G410: __E___E_cli_command_119_120(296) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(296), (T(55)+T(13)+T(42)+T(15)+T(46)+T(15)+T(14)+T(15)+T(11)+T(46)+T(10)));
//G411: def_sym(295)         => __E___E_cli_command_119_120(296).
	p(NT(295), (NT(296)));
//G412: __E_cli_command_119(294) => def_sym(295).
	p(NT(294), (NT(295)));
//G413: def_list_cmd(293)    => __E_cli_command_119(294).
	p(NT(293), (NT(294)));
//G414: cli_command(213)     => def_list_cmd(293).
	p(NT(213), (NT(293)));
//G415: __E_cli_command_121(298) => def_sym(295) __(58) number(299).
	p(NT(298), (NT(295)+NT(58)+NT(299)));
//G416: def_print_cmd(297)   => __E_cli_command_121(298).
	p(NT(297), (NT(298)));
//G417: cli_command(213)     => def_print_cmd(297).
	p(NT(213), (NT(297)));
//G418: def_rr_cmd(300)      => rec_relation(16).
	p(NT(300), (NT(16)));
//G419: cli_command(213)     => def_rr_cmd(300).
	p(NT(213), (NT(300)));
//G420: qelim_sym(303)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(303), (T(53)+T(13)+T(18)+T(15)+T(12)));
//G421: __E_cli_command_122(302) => qelim_sym(303) __(58) wff_cmd_arg(248).
	p(NT(302), (NT(303)+NT(58)+NT(248)));
//G422: qelim_cmd(301)       => __E_cli_command_122(302).
	p(NT(301), (NT(302)));
//G423: cli_command(213)     => qelim_cmd(301).
	p(NT(213), (NT(301)));
//G424: get_sym(306)         => 'g' 'e' 't'.
	p(NT(306), (T(47)+T(13)+T(14)));
//G425: __E___E_cli_command_123_124(307) => __(58) option(308).
	p(NT(307), (NT(58)+NT(308)));
//G426: __E___E_cli_command_123_124(307) => null.
	p(NT(307), (nul));
//G427: __E_cli_command_123(305) => get_sym(306) __E___E_cli_command_123_124(307).
	p(NT(305), (NT(306)+NT(307)));
//G428: get_cmd(304)         => __E_cli_command_123(305).
	p(NT(304), (NT(305)));
//G429: cli_command(213)     => get_cmd(304).
	p(NT(213), (NT(304)));
//G430: set_sym(311)         => 's' 'e' 't'.
	p(NT(311), (T(10)+T(13)+T(14)));
//G431: __E___E_cli_command_125_126(312) => __(58).
	p(NT(312), (NT(58)));
//G432: __E___E_cli_command_125_126(312) => _(11) '=' _(11).
	p(NT(312), (NT(11)+T(3)+NT(11)));
//G433: __E_cli_command_125(310) => set_sym(311) __(58) option(308) __E___E_cli_command_125_126(312) option_value(313).
	p(NT(310), (NT(311)+NT(58)+NT(308)+NT(312)+NT(313)));
//G434: set_cmd(309)         => __E_cli_command_125(310).
	p(NT(309), (NT(310)));
//G435: cli_command(213)     => set_cmd(309).
	p(NT(213), (NT(309)));
//G436: toggle_sym(316)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(316), (T(14)+T(11)+T(47)+T(47)+T(18)+T(13)));
//G437: __E_cli_command_127(315) => toggle_sym(316) __(58) bool_option(317).
	p(NT(315), (NT(316)+NT(58)+NT(317)));
//G438: toggle_cmd(314)      => __E_cli_command_127(315).
	p(NT(314), (NT(315)));
//G439: cli_command(213)     => toggle_cmd(314).
	p(NT(213), (NT(314)));
//G440: __E___E_cli_command_128_129(321) => 'h' 'i' 's' 't'.
	p(NT(321), (T(44)+T(15)+T(10)+T(14)));
//G441: __E___E_cli_command_128_129(321) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(321), (T(44)+T(15)+T(10)+T(14)+T(11)+T(50)+T(20)));
//G442: history_sym(320)     => __E___E_cli_command_128_129(321).
	p(NT(320), (NT(321)));
//G443: __E_cli_command_128(319) => history_sym(320).
	p(NT(319), (NT(320)));
//G444: history_list_cmd(318) => __E_cli_command_128(319).
	p(NT(318), (NT(319)));
//G445: cli_command(213)     => history_list_cmd(318).
	p(NT(213), (NT(318)));
//G446: __E_cli_command_130(323) => history_sym(320) __(58) memory(324).
	p(NT(323), (NT(320)+NT(58)+NT(324)));
//G447: history_print_cmd(322) => __E_cli_command_130(323).
	p(NT(322), (NT(323)));
//G448: cli_command(213)     => history_print_cmd(322).
	p(NT(213), (NT(322)));
//G449: __E_cli_command_131(326) => wff(14).
	p(NT(326), (NT(14)));
//G450: __E_cli_command_131(326) => bf(21).
	p(NT(326), (NT(21)));
//G451: history_store_cmd(325) => __E_cli_command_131(326).
	p(NT(325), (NT(326)));
//G452: cli_command(213)     => history_store_cmd(325).
	p(NT(213), (NT(325)));
//G453: number(299)          => digits(152).
	p(NT(299), (NT(152)));
//G454: bf_cmd_arg(327)      => memory(324).
	p(NT(327), (NT(324)));
//G455: bf_cmd_arg(327)      => bf(21).
	p(NT(327), (NT(21)));
//G456: wff_cmd_arg(248)     => memory(324).
	p(NT(248), (NT(324)));
//G457: wff_cmd_arg(248)     => wff(14).
	p(NT(248), (NT(14)));
//G458: nf_cmd_arg(262)      => memory(324).
	p(NT(262), (NT(324)));
//G459: nf_cmd_arg(262)      => wff(14).
	p(NT(262), (NT(14)));
//G460: nf_cmd_arg(262)      => bf(21).
	p(NT(262), (NT(21)));
//G461: onf_cmd_arg(292)     => memory(324).
	p(NT(292), (NT(324)));
//G462: onf_cmd_arg(292)     => wff(14).
	p(NT(292), (NT(14)));
//G463: normalize_cmd_arg(241) => memory(324).
	p(NT(241), (NT(324)));
//G464: normalize_cmd_arg(241) => rr(10).
	p(NT(241), (NT(10)));
//G465: normalize_cmd_arg(241) => ref(18).
	p(NT(241), (NT(18)));
//G466: normalize_cmd_arg(241) => wff(14).
	p(NT(241), (NT(14)));
//G467: normalize_cmd_arg(241) => bf(21).
	p(NT(241), (NT(21)));
//G468: inst_args(267)       => wff_cmd_arg(248) _(11) '[' _(11) variable(111) _(11) '/' _(11) bf_cmd_arg(327) _(11) ']'.
	p(NT(267), (NT(248)+NT(11)+T(21)+NT(11)+NT(111)+NT(11)+T(56)+NT(11)+NT(327)+NT(11)+T(22)));
//G469: inst_args(267)       => bf_cmd_arg(327) _(11) '[' _(11) variable(111) _(11) '/' _(11) bf_cmd_arg(327) _(11) ']'.
	p(NT(267), (NT(327)+NT(11)+T(21)+NT(11)+NT(111)+NT(11)+T(56)+NT(11)+NT(327)+NT(11)+T(22)));
//G470: help_arg(230)        => help_sym(227).
	p(NT(230), (NT(227)));
//G471: help_arg(230)        => version_sym(220).
	p(NT(230), (NT(220)));
//G472: help_arg(230)        => quit_sym(217).
	p(NT(230), (NT(217)));
//G473: help_arg(230)        => clear_sym(223).
	p(NT(230), (NT(223)));
//G474: help_arg(230)        => get_sym(306).
	p(NT(230), (NT(306)));
//G475: help_arg(230)        => set_sym(311).
	p(NT(230), (NT(311)));
//G476: help_arg(230)        => toggle_sym(316).
	p(NT(230), (NT(316)));
//G477: help_arg(230)        => file_sym(233).
	p(NT(230), (NT(233)));
//G478: help_arg(230)        => history_sym(320).
	p(NT(230), (NT(320)));
//G479: help_arg(230)        => abs_memory_sym(328).
	p(NT(230), (NT(328)));
//G480: help_arg(230)        => rel_memory_sym(329).
	p(NT(230), (NT(329)));
//G481: help_arg(230)        => selection_sym(330).
	p(NT(230), (NT(330)));
//G482: help_arg(230)        => def_sym(295).
	p(NT(230), (NT(295)));
//G483: help_arg(230)        => inst_sym(265).
	p(NT(230), (NT(265)));
//G484: help_arg(230)        => subst_sym(260).
	p(NT(230), (NT(260)));
//G485: help_arg(230)        => normalize_sym(256).
	p(NT(230), (NT(256)));
//G486: help_arg(230)        => execute_sym(251).
	p(NT(230), (NT(251)));
//G487: help_arg(230)        => solve_sym(247).
	p(NT(230), (NT(247)));
//G488: help_arg(230)        => valid_sym(237).
	p(NT(230), (NT(237)));
//G489: help_arg(230)        => sat_sym(240).
	p(NT(230), (NT(240)));
//G490: help_arg(230)        => unsat_sym(244).
	p(NT(230), (NT(244)));
//G491: help_arg(230)        => dnf_sym(270).
	p(NT(230), (NT(270)));
//G492: help_arg(230)        => cnf_sym(273).
	p(NT(230), (NT(273)));
//G493: help_arg(230)        => anf_sym(276).
	p(NT(230), (NT(276)));
//G494: help_arg(230)        => snf_sym(288).
	p(NT(230), (NT(288)));
//G495: help_arg(230)        => nnf_sym(279).
	p(NT(230), (NT(279)));
//G496: help_arg(230)        => pnf_sym(282).
	p(NT(230), (NT(282)));
//G497: help_arg(230)        => mnf_sym(285).
	p(NT(230), (NT(285)));
//G498: help_arg(230)        => onf_sym(291).
	p(NT(230), (NT(291)));
//G499: help_arg(230)        => qelim_sym(303).
	p(NT(230), (NT(303)));
//G500: __E___E_help_arg_132_133(333) => 's'.
	p(NT(333), (T(10)));
//G501: __E___E_help_arg_132_133(333) => null.
	p(NT(333), (nul));
//G502: __E_help_arg_132(332) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_132_133(333).
	p(NT(332), (T(13)+T(24)+T(17)+T(12)+T(54)+T(18)+T(13)+NT(333)));
//G503: examples_sym(331)    => __E_help_arg_132(332).
	p(NT(331), (NT(332)));
//G504: help_arg(230)        => examples_sym(331).
	p(NT(230), (NT(331)));
//G505: __E_memory_134(335)  => '%'.
	p(NT(335), (T(57)));
//G506: rel_memory_sym(329)  => '%' '-'.
	p(NT(329), (T(57)+T(25)));
//G507: memory_id(337)       => digits(152).
	p(NT(337), (NT(152)));
//G508: __E___E_memory_134_135(336) => memory_id(337).
	p(NT(336), (NT(337)));
//G509: __E___E_memory_134_135(336) => null.
	p(NT(336), (nul));
//G510: __E_memory_134(335)  => rel_memory_sym(329) __E___E_memory_134_135(336).
	p(NT(335), (NT(329)+NT(336)));
//G511: rel_memory(334)      => __E_memory_134(335).
	p(NT(334), (NT(335)));
//G512: memory(324)          => rel_memory(334).
	p(NT(324), (NT(334)));
//G513: abs_memory_sym(328)  => '%'.
	p(NT(328), (T(57)));
//G514: __E_memory_136(339)  => abs_memory_sym(328) memory_id(337).
	p(NT(339), (NT(328)+NT(337)));
//G515: abs_memory(338)      => __E_memory_136(339).
	p(NT(338), (NT(339)));
//G516: memory(324)          => abs_memory(338).
	p(NT(324), (NT(338)));
//G517: option(308)          => bool_option(317).
	p(NT(308), (NT(317)));
//G518: __E_option_137(342)  => 's' 'e' 'v'.
	p(NT(342), (T(10)+T(13)+T(51)));
//G519: __E_option_137(342)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(342), (T(10)+T(13)+T(51)+T(13)+T(50)+T(15)+T(14)+T(20)));
//G520: severity_opt(341)    => __E_option_137(342).
	p(NT(341), (NT(342)));
//G521: enum_option(340)     => severity_opt(341).
	p(NT(340), (NT(341)));
//G522: option(308)          => enum_option(340).
	p(NT(308), (NT(340)));
//G523: __E_bool_option_138(344) => 's'.
	p(NT(344), (T(10)));
//G524: __E_bool_option_138(344) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(344), (T(10)+T(14)+T(17)+T(14)+T(48)+T(10)));
//G525: status_opt(343)      => __E_bool_option_138(344).
	p(NT(343), (NT(344)));
//G526: bool_option(317)     => status_opt(343).
	p(NT(317), (NT(343)));
//G527: __E_bool_option_139(346) => 'c'.
	p(NT(346), (T(45)));
//G528: __E_bool_option_139(346) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(346), (T(45)+T(11)+T(18)+T(11)+T(50)));
//G529: __E_bool_option_139(346) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(346), (T(45)+T(11)+T(18)+T(11)+T(50)+T(10)));
//G530: colors_opt(345)      => __E_bool_option_139(346).
	p(NT(345), (NT(346)));
//G531: bool_option(317)     => colors_opt(345).
	p(NT(317), (NT(345)));
//G532: __E_bool_option_140(348) => 'd'.
	p(NT(348), (T(55)));
//G533: __E_bool_option_140(348) => 'd' 'b' 'g'.
	p(NT(348), (T(55)+T(49)+T(47)));
//G534: __E_bool_option_140(348) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(348), (T(55)+T(13)+T(49)+T(48)+T(47)));
//G535: debug_repl_opt(347)  => __E_bool_option_140(348).
	p(NT(347), (NT(348)));
//G536: bool_option(317)     => debug_repl_opt(347).
	p(NT(317), (NT(347)));
//G537: option_value(313)    => option_value_true(349).
	p(NT(313), (NT(349)));
//G538: option_value(313)    => option_value_false(350).
	p(NT(313), (NT(350)));
//G539: option_value(313)    => severity(351).
	p(NT(313), (NT(351)));
//G540: option_value_true(349) => 't'.
	p(NT(349), (T(14)));
//G541: option_value_true(349) => 't' 'r' 'u' 'e'.
	p(NT(349), (T(14)+T(50)+T(48)+T(13)));
//G542: option_value_true(349) => 'o' 'n'.
	p(NT(349), (T(11)+T(46)));
//G543: option_value_true(349) => '1'.
	p(NT(349), (T(35)));
//G544: option_value_true(349) => 'y'.
	p(NT(349), (T(20)));
//G545: option_value_true(349) => 'y' 'e' 's'.
	p(NT(349), (T(20)+T(13)+T(10)));
//G546: option_value_false(350) => 'f'.
	p(NT(350), (T(42)));
//G547: option_value_false(350) => 'f' 'a' 'l' 's' 'e'.
	p(NT(350), (T(42)+T(17)+T(18)+T(10)+T(13)));
//G548: option_value_false(350) => 'o' 'f' 'f'.
	p(NT(350), (T(11)+T(42)+T(42)));
//G549: option_value_false(350) => '0'.
	p(NT(350), (T(36)));
//G550: option_value_false(350) => 'n'.
	p(NT(350), (T(46)));
//G551: option_value_false(350) => 'n' 'o'.
	p(NT(350), (T(46)+T(11)));
//G552: __E_severity_141(353) => 'e'.
	p(NT(353), (T(13)));
//G553: __E_severity_141(353) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(353), (T(13)+T(50)+T(50)+T(11)+T(50)));
//G554: error_sym(352)       => __E_severity_141(353).
	p(NT(352), (NT(353)));
//G555: severity(351)        => error_sym(352).
	p(NT(351), (NT(352)));
//G556: __E_severity_142(355) => 'i'.
	p(NT(355), (T(15)));
//G557: __E_severity_142(355) => 'i' 'n' 'f' 'o'.
	p(NT(355), (T(15)+T(46)+T(42)+T(11)));
//G558: info_sym(354)        => __E_severity_142(355).
	p(NT(354), (NT(355)));
//G559: severity(351)        => info_sym(354).
	p(NT(351), (NT(354)));
//G560: __E_severity_143(357) => 'd'.
	p(NT(357), (T(55)));
//G561: __E_severity_143(357) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(357), (T(55)+T(13)+T(49)+T(48)+T(47)));
//G562: debug_sym(356)       => __E_severity_143(357).
	p(NT(356), (NT(357)));
//G563: severity(351)        => debug_sym(356).
	p(NT(351), (NT(356)));
//G564: __E_severity_144(359) => 't'.
	p(NT(359), (T(14)));
//G565: __E_severity_144(359) => 't' 'r' 'a' 'c' 'e'.
	p(NT(359), (T(14)+T(50)+T(17)+T(45)+T(13)));
//G566: trace_sym(358)       => __E_severity_144(359).
	p(NT(358), (NT(359)));
//G567: severity(351)        => trace_sym(358).
	p(NT(351), (NT(358)));
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
		rr, _, rec_relations, main, wff, __E_rec_relations_0, rec_relation, __E_rec_relations_1, ref, __E_rec_relation_2, 
		capture, bf, sym, __E_ref_3, offsets, ref_args, __E_ref_args_4, __E___E_ref_args_4_5, ref_arg, __E___E_ref_args_4_6, 
		__E___E_ref_args_4_7, inputs, __E_inputs_8, __E_inputs_9, input, in, source_binding, library, rules, __E_library_10, 
		__E___E_library_10_11, rule, __E___E___E_library_10_11_12, wff_rule, bf_rule, __E___E_library_10_13, wff_matcher, wff_body, __E_wff_rule_14, wff_cb, 
		bf_matcher, bf_body, __E_bf_rule_15, bf_cb, builder, builder_head, builder_body, __E_builder_head_16, __, __E_builder_head_17, 
		bf_builder_body, __E_builder_body_18, wff_builder_body, __E_builder_body_19, wff_parenthesis, __E_wff_20, wff_sometimes, __E_wff_21, __E___E_wff_21_22, wff_always, 
		__E_wff_23, __E___E_wff_23_24, wff_conditional, __E_wff_25, wff_all, __E_wff_26, q_vars, wff_ex, __E_wff_27, wff_ref, 
		wff_imply, __E_wff_28, wff_equiv, __E_wff_29, wff_or, __E_wff_30, wff_xor, __E_wff_31, wff_and, __E_wff_32, 
		wff_neg, __E_wff_33, wff_t, wff_f, bf_interval, __E_wff_34, bf_neq, __E_wff_35, bf_eq, __E_wff_36, 
		bf_nleq, __E_wff_37, bf_greater, __E_wff_38, bf_less_equal, __E_wff_39, bf_less, __E_wff_40, uninterpreted_constant, bf_parenthesis, 
		__E_bf_41, variable, bf_splitter, __E_bf_42, bf_ref, bf_or, __E_bf_43, bf_xor, __E_bf_44, bf_and, 
		__E_bf_45, __E___E_bf_45_46, bf_neg, __E_bf_47, bf_constant, __E_bf_48, constant, bf_t, bf_f, flag, 
		flag_neq, __E_flag_49, flagvar, num, flag_eq, __E_flag_50, flag_greater_equal, __E_flag_51, flag_greater, __E_flag_52, 
		flag_less_equal, __E_flag_53, flag_less, __E_flag_54, charvar, __E_charvar_55, __E_charvar_56, io_var, __E_variable_57, out, 
		in_var_name, __E_in_58, digits, offset, out_var_name, __E_out_59, q_var, __E_q_vars_60, __E_q_vars_61, __E_offsets_62, 
		__E_offsets_63, shift, __E_offset_64, __E_shift_65, __E___E_shift_65_66, uninter_const_name, __E_uninterpreted_constant_67, __E___E_uninterpreted_constant_67_68, chars, binding, 
		__E_binding_69, type, __E___E_binding_69_70, source, named_binding, src_c, __E_source_71, __E_source_72, __E___E_source_72_73, __E___E_source_72_74, 
		__E_chars_75, __E_digits_76, comment, __E_comment_77, __E_comment_78, __E_comment_79, __E____80, __E___81, bf_cb_args1, bf_cb_arg, 
		bf_cb_args2, bf_cb_args3, bf_cb_args4, wff_cb_args1, wff_cb_arg, wff_cb_args2, wff_cb_args3, wff_cb_args4, wff_has_clashing_subformulas_cb, __E_wff_cb_82, 
		wff_has_subformula_cb, __E_wff_cb_83, wff_remove_existential_cb, __E_wff_cb_84, bf_has_subformula_cb, __E_bf_cb_85, bf_remove_funiversal_cb, __E_bf_cb_86, bf_remove_fexistential_cb, __E_bf_cb_87, 
		bf_normalize_cb, __E_bf_cb_88, cli, cli_command, __E_cli_89, __E_cli_90, quit_cmd, quit_sym, __E_cli_command_91, version_cmd, 
		version_sym, __E_cli_command_92, clear_cmd, clear_sym, __E_cli_command_93, help_cmd, __E_cli_command_94, help_sym, __E___E_cli_command_94_95, __E___E_cli_command_94_96, 
		help_arg, file_cmd, __E_cli_command_97, file_sym, q_string, valid_cmd, __E_cli_command_98, valid_sym, sat_cmd, __E_cli_command_99, 
		sat_sym, normalize_cmd_arg, unsat_cmd, __E_cli_command_100, unsat_sym, solve_cmd, __E_cli_command_101, solve_sym, wff_cmd_arg, execute_cmd, 
		__E_cli_command_102, execute_sym, __E___E_cli_command_102_103, __E___E_cli_command_102_104, normalize_cmd, __E_cli_command_105, normalize_sym, __E___E_cli_command_105_106, subst_cmd, __E_cli_command_107, 
		subst_sym, __E___E_cli_command_107_108, nf_cmd_arg, inst_cmd, __E_cli_command_109, inst_sym, __E___E_cli_command_109_110, inst_args, dnf_cmd, __E_cli_command_111, 
		dnf_sym, cnf_cmd, __E_cli_command_112, cnf_sym, anf_cmd, __E_cli_command_113, anf_sym, nnf_cmd, __E_cli_command_114, nnf_sym, 
		pnf_cmd, __E_cli_command_115, pnf_sym, mnf_cmd, __E_cli_command_116, mnf_sym, snf_cmd, __E_cli_command_117, snf_sym, onf_cmd, 
		__E_cli_command_118, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_119, def_sym, __E___E_cli_command_119_120, def_print_cmd, __E_cli_command_121, number, 
		def_rr_cmd, qelim_cmd, __E_cli_command_122, qelim_sym, get_cmd, __E_cli_command_123, get_sym, __E___E_cli_command_123_124, option, set_cmd, 
		__E_cli_command_125, set_sym, __E___E_cli_command_125_126, option_value, toggle_cmd, __E_cli_command_127, toggle_sym, bool_option, history_list_cmd, __E_cli_command_128, 
		history_sym, __E___E_cli_command_128_129, history_print_cmd, __E_cli_command_130, memory, history_store_cmd, __E_cli_command_131, bf_cmd_arg, abs_memory_sym, rel_memory_sym, 
		selection_sym, examples_sym, __E_help_arg_132, __E___E_help_arg_132_133, rel_memory, __E_memory_134, __E___E_memory_134_135, memory_id, abs_memory, __E_memory_136, 
		enum_option, severity_opt, __E_option_137, status_opt, __E_bool_option_138, colors_opt, __E_bool_option_139, debug_repl_opt, __E_bool_option_140, option_value_true, 
		option_value_false, severity, error_sym, __E_severity_141, info_sym, __E_severity_142, debug_sym, __E_severity_143, trace_sym, __E_severity_144, 
		__N_0, __N_1, __N_2, __N_3, 
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
