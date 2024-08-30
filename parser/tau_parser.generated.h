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
	"__E_bf_45", "__E___E_bf_45_46", "bf_neg", "__E_bf_47", "bf_constant", "__E_bf_48", "constant", "bf_t", "bf_f", "charvar", 
	"__E_charvar_49", "__E_charvar_50", "io_var", "__E_variable_51", "out", "in_var_name", "__E_in_52", "digits", "offset", "out_var_name", 
	"__E_out_53", "q_var", "__E_q_vars_54", "__E_q_vars_55", "__E_offsets_56", "__E_offsets_57", "num", "shift", "__E_offset_58", "__E_shift_59", 
	"__E___E_shift_59_60", "uninter_const_name", "__E_uninterpreted_constant_61", "__E___E_uninterpreted_constant_61_62", "chars", "binding", "__E_binding_63", "type", "__E___E_binding_63_64", "source", 
	"named_binding", "src_c", "__E_source_65", "__E_source_66", "__E___E_source_66_67", "__E___E_source_66_68", "__E_chars_69", "__E_digits_70", "comment", "__E_comment_71", 
	"__E_comment_72", "__E_comment_73", "__E____74", "__E___75", "bf_cb_args1", "bf_cb_arg", "bf_cb_args2", "bf_cb_args3", "bf_cb_args4", "wff_cb_args1", 
	"wff_cb_arg", "wff_cb_args2", "wff_cb_args3", "wff_cb_args4", "wff_has_clashing_subformulas_cb", "__E_wff_cb_76", "wff_has_subformula_cb", "__E_wff_cb_77", "wff_remove_existential_cb", "__E_wff_cb_78", 
	"bf_has_subformula_cb", "__E_bf_cb_79", "bf_remove_funiversal_cb", "__E_bf_cb_80", "bf_remove_fexistential_cb", "__E_bf_cb_81", "bf_normalize_cb", "__E_bf_cb_82", "cli", "cli_command", 
	"__E_cli_83", "__E_cli_84", "quit_cmd", "quit_sym", "__E_cli_command_85", "version_cmd", "version_sym", "__E_cli_command_86", "clear_cmd", "clear_sym", 
	"__E_cli_command_87", "help_cmd", "__E_cli_command_88", "help_sym", "__E___E_cli_command_88_89", "__E___E_cli_command_88_90", "help_arg", "file_cmd", "__E_cli_command_91", "file_sym", 
	"q_string", "valid_cmd", "__E_cli_command_92", "valid_sym", "sat_cmd", "__E_cli_command_93", "sat_sym", "unsat_cmd", "__E_cli_command_94", "unsat_sym", 
	"solve_cmd", "__E_cli_command_95", "solve_sym", "solve_cmd_arg", "__E___E_cli_command_95_96", "__E___E_cli_command_95_97", "execute_cmd", "__E_cli_command_98", "execute_sym", "__E___E_cli_command_98_99", 
	"__E___E_cli_command_98_100", "wff_typed", "normalize_cmd", "__E_cli_command_101", "normalize_sym", "__E___E_cli_command_101_102", "normalize_cmd_arg", "subst_cmd", "__E_cli_command_103", "subst_sym", 
	"__E___E_cli_command_103_104", "nf_cmd_arg", "inst_cmd", "__E_cli_command_105", "inst_sym", "__E___E_cli_command_105_106", "inst_args", "dnf_cmd", "__E_cli_command_107", "dnf_sym", 
	"cnf_cmd", "__E_cli_command_108", "cnf_sym", "anf_cmd", "__E_cli_command_109", "anf_sym", "nnf_cmd", "__E_cli_command_110", "nnf_sym", "pnf_cmd", 
	"__E_cli_command_111", "pnf_sym", "mnf_cmd", "__E_cli_command_112", "mnf_sym", "snf_cmd", "__E_cli_command_113", "snf_sym", "onf_cmd", "__E_cli_command_114", 
	"onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_115", "def_sym", "__E___E_cli_command_115_116", "def_print_cmd", "__E_cli_command_117", "number", "def_rr_cmd", 
	"qelim_cmd", "__E_cli_command_118", "qelim_sym", "wff_cmd_arg", "get_cmd", "__E_cli_command_119", "get_sym", "__E___E_cli_command_119_120", "option", "set_cmd", 
	"__E_cli_command_121", "set_sym", "__E___E_cli_command_121_122", "option_value", "toggle_cmd", "__E_cli_command_123", "toggle_sym", "bool_option", "history_list_cmd", "__E_cli_command_124", 
	"history_sym", "__E___E_cli_command_124_125", "history_print_cmd", "__E_cli_command_126", "memory", "history_store_cmd", "__E_cli_command_127", "bf_typed", "bf_cmd_arg", "abs_memory_sym", 
	"rel_memory_sym", "selection_sym", "examples_sym", "__E_help_arg_128", "__E___E_help_arg_128_129", "rel_memory", "__E_memory_130", "__E___E_memory_130_131", "memory_id", "abs_memory", 
	"__E_memory_132", "enum_option", "severity_opt", "__E_option_133", "status_opt", "__E_bool_option_134", "colors_opt", "__E_bool_option_135", "debug_repl_opt", "__E_bool_option_136", 
	"option_value_true", "option_value_false", "severity", "error_sym", "__E_severity_137", "info_sym", "__E_severity_138", "debug_sym", "__E_severity_139", "trace_sym", 
	"__E_severity_140", "__N_0", "__N_1", "__N_2", "__N_3", 
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
			20, 22, 129, 135, 137, 139, 151, 157, 159, 160
		},
		.to_inline = {
			{ 14, 64, 14 },
			{ 21, 109, 21 },
			{ 49 },
			{ 53 },
			{ 141 },
			{ 154 },
			{ 161 },
			{ 174 },
			{ 176 },
			{ 177 },
			{ 178 },
			{ 179 },
			{ 181 },
			{ 182 },
			{ 183 },
			{ 241 },
			{ 246 },
			{ 251 },
			{ 256 },
			{ 281 },
			{ 293 },
			{ 317 },
			{ 318 }
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
//G157: bf(21)               => capture(20).
	p(NT(21), (NT(20)));
//G158: __N_0(351)           => 'F'.
	p(NT(351), (T(31)));
//G159: __N_1(352)           => 'T'.
	p(NT(352), (T(30)));
//G160: __E_charvar_49(130)  => ~( __N_0(351) ) & ~( __N_1(352) ) & alpha(5).	 # conjunctive
	p(NT(130), ~(NT(351)) & ~(NT(352)) & (NT(5)));
//G161: __E_charvar_50(131)  => null.
	p(NT(131), (nul));
//G162: __E_charvar_50(131)  => digit(3) __E_charvar_50(131).
	p(NT(131), (NT(3)+NT(131)));
//G163: charvar(129)         => __E_charvar_49(130) __E_charvar_50(131).
	p(NT(129), (NT(130)+NT(131)));
//G164: variable(111)        => charvar(129).
	p(NT(111), (NT(129)));
//G165: __E_variable_51(133) => in(35).
	p(NT(133), (NT(35)));
//G166: __E_variable_51(133) => out(134).
	p(NT(133), (NT(134)));
//G167: io_var(132)          => __E_variable_51(133).
	p(NT(132), (NT(133)));
//G168: variable(111)        => io_var(132).
	p(NT(111), (NT(132)));
//G169: __E_in_52(136)       => 'i' digits(137).
	p(NT(136), (T(15)+NT(137)));
//G170: in_var_name(135)     => __E_in_52(136).
	p(NT(135), (NT(136)));
//G171: in(35)               => in_var_name(135) '[' offset(138) ']'.
	p(NT(35), (NT(135)+T(21)+NT(138)+T(22)));
//G172: __E_out_53(140)      => 'o' digits(137).
	p(NT(140), (T(11)+NT(137)));
//G173: out_var_name(139)    => __E_out_53(140).
	p(NT(139), (NT(140)));
//G174: out(134)             => out_var_name(139) '[' offset(138) ']'.
	p(NT(134), (NT(139)+T(21)+NT(138)+T(22)));
//G175: capture(20)          => '$' charvar(129).
	p(NT(20), (T(37)+NT(129)));
//G176: __E_q_vars_54(142)   => _(11) ',' _(11) q_var(141).
	p(NT(142), (NT(11)+T(4)+NT(11)+NT(141)));
//G177: __E_q_vars_55(143)   => null.
	p(NT(143), (nul));
//G178: __E_q_vars_55(143)   => __E_q_vars_54(142) __E_q_vars_55(143).
	p(NT(143), (NT(142)+NT(143)));
//G179: q_vars(76)           => q_var(141) __E_q_vars_55(143).
	p(NT(76), (NT(141)+NT(143)));
//G180: q_var(141)           => capture(20).
	p(NT(141), (NT(20)));
//G181: q_var(141)           => variable(111).
	p(NT(141), (NT(111)));
//G182: __E_offsets_56(144)  => _(11) ',' _(11) offset(138).
	p(NT(144), (NT(11)+T(4)+NT(11)+NT(138)));
//G183: __E_offsets_57(145)  => null.
	p(NT(145), (nul));
//G184: __E_offsets_57(145)  => __E_offsets_56(144) __E_offsets_57(145).
	p(NT(145), (NT(144)+NT(145)));
//G185: offsets(24)          => '[' _(11) offset(138) __E_offsets_57(145) _(11) ']'.
	p(NT(24), (T(21)+NT(11)+NT(138)+NT(145)+NT(11)+T(22)));
//G186: offset(138)          => num(146).
	p(NT(138), (NT(146)));
//G187: offset(138)          => capture(20).
	p(NT(138), (NT(20)));
//G188: offset(138)          => shift(147).
	p(NT(138), (NT(147)));
//G189: __N_2(353)           => io_var(132).
	p(NT(353), (NT(132)));
//G190: __E_offset_58(148)   => variable(111) & ~( __N_2(353) ).	 # conjunctive
	p(NT(148), (NT(111)) & ~(NT(353)));
//G191: offset(138)          => __E_offset_58(148).
	p(NT(138), (NT(148)));
//G192: __E_shift_59(149)    => capture(20).
	p(NT(149), (NT(20)));
//G193: __N_3(354)           => io_var(132).
	p(NT(354), (NT(132)));
//G194: __E___E_shift_59_60(150) => variable(111) & ~( __N_3(354) ).	 # conjunctive
	p(NT(150), (NT(111)) & ~(NT(354)));
//G195: __E_shift_59(149)    => __E___E_shift_59_60(150).
	p(NT(149), (NT(150)));
//G196: shift(147)           => __E_shift_59(149) _(11) '-' _(11) num(146).
	p(NT(147), (NT(149)+NT(11)+T(25)+NT(11)+NT(146)));
//G197: num(146)             => digits(137).
	p(NT(146), (NT(137)));
//G198: __E___E_uninterpreted_constant_61_62(153) => chars(154).
	p(NT(153), (NT(154)));
//G199: __E___E_uninterpreted_constant_61_62(153) => _(11).
	p(NT(153), (NT(11)));
//G200: __E_uninterpreted_constant_61(152) => __E___E_uninterpreted_constant_61_62(153) ':' chars(154).
	p(NT(152), (NT(153)+T(2)+NT(154)));
//G201: uninter_const_name(151) => __E_uninterpreted_constant_61(152).
	p(NT(151), (NT(152)));
//G202: uninterpreted_constant(108) => '<' uninter_const_name(151) _(11) '>'.
	p(NT(108), (T(7)+NT(151)+NT(11)+T(16)));
//G203: constant(126)        => binding(155).
	p(NT(126), (NT(155)));
//G204: constant(126)        => capture(20).
	p(NT(126), (NT(20)));
//G205: __E___E_binding_63_64(158) => chars(154) _(11).
	p(NT(158), (NT(154)+NT(11)));
//G206: __E___E_binding_63_64(158) => _(11).
	p(NT(158), (NT(11)));
//G207: type(157)            => __E___E_binding_63_64(158).
	p(NT(157), (NT(158)));
//G208: __E_binding_63(156)  => type(157) ':' _(11) source(159).
	p(NT(156), (NT(157)+T(2)+NT(11)+NT(159)));
//G209: source_binding(36)   => __E_binding_63(156).
	p(NT(36), (NT(156)));
//G210: binding(155)         => source_binding(36).
	p(NT(155), (NT(36)));
//G211: named_binding(160)   => chars(154).
	p(NT(160), (NT(154)));
//G212: binding(155)         => named_binding(160).
	p(NT(155), (NT(160)));
//G213: __E_source_65(162)   => alnum(6).
	p(NT(162), (NT(6)));
//G214: __E_source_65(162)   => punct(7).
	p(NT(162), (NT(7)));
//G215: src_c(161)           => __E_source_65(162).
	p(NT(161), (NT(162)));
//G216: __E___E_source_66_67(164) => src_c(161).
	p(NT(164), (NT(161)));
//G217: __E___E_source_66_67(164) => space(2).
	p(NT(164), (NT(2)));
//G218: __E___E_source_66_68(165) => null.
	p(NT(165), (nul));
//G219: __E___E_source_66_68(165) => __E___E_source_66_67(164) __E___E_source_66_68(165).
	p(NT(165), (NT(164)+NT(165)));
//G220: __E_source_66(163)   => __E___E_source_66_68(165) src_c(161).
	p(NT(163), (NT(165)+NT(161)));
//G221: __E_source_66(163)   => null.
	p(NT(163), (nul));
//G222: source(159)          => src_c(161) __E_source_66(163).
	p(NT(159), (NT(161)+NT(163)));
//G223: __E_chars_69(166)    => null.
	p(NT(166), (nul));
//G224: __E_chars_69(166)    => alnum(6) __E_chars_69(166).
	p(NT(166), (NT(6)+NT(166)));
//G225: chars(154)           => alpha(5) __E_chars_69(166).
	p(NT(154), (NT(5)+NT(166)));
//G226: __E_digits_70(167)   => digit(3).
	p(NT(167), (NT(3)));
//G227: __E_digits_70(167)   => digit(3) __E_digits_70(167).
	p(NT(167), (NT(3)+NT(167)));
//G228: digits(137)          => __E_digits_70(167).
	p(NT(137), (NT(167)));
//G229: sym(22)              => chars(154).
	p(NT(22), (NT(154)));
//G230: __E_comment_71(169)  => printable(8).
	p(NT(169), (NT(8)));
//G231: __E_comment_71(169)  => '\t'.
	p(NT(169), (T(38)));
//G232: __E_comment_72(170)  => null.
	p(NT(170), (nul));
//G233: __E_comment_72(170)  => __E_comment_71(169) __E_comment_72(170).
	p(NT(170), (NT(169)+NT(170)));
//G234: __E_comment_73(171)  => '\n'.
	p(NT(171), (T(39)));
//G235: __E_comment_73(171)  => '\r'.
	p(NT(171), (T(40)));
//G236: __E_comment_73(171)  => eof(1).
	p(NT(171), (NT(1)));
//G237: comment(168)         => '#' __E_comment_72(170) __E_comment_73(171).
	p(NT(168), (T(41)+NT(170)+NT(171)));
//G238: __E____74(172)       => space(2).
	p(NT(172), (NT(2)));
//G239: __E____74(172)       => comment(168).
	p(NT(172), (NT(168)));
//G240: __(58)               => __E____74(172) _(11).
	p(NT(58), (NT(172)+NT(11)));
//G241: __E___75(173)        => __(58).
	p(NT(173), (NT(58)));
//G242: __E___75(173)        => null.
	p(NT(173), (nul));
//G243: _(11)                => __E___75(173).
	p(NT(11), (NT(173)));
//G244: bf_cb_arg(175)       => bf(21).
	p(NT(175), (NT(21)));
//G245: bf_cb_args1(174)     => __(58) bf_cb_arg(175).
	p(NT(174), (NT(58)+NT(175)));
//G246: bf_cb_args2(176)     => bf_cb_args1(174) bf_cb_args1(174).
	p(NT(176), (NT(174)+NT(174)));
//G247: bf_cb_args3(177)     => bf_cb_args2(176) bf_cb_args1(174).
	p(NT(177), (NT(176)+NT(174)));
//G248: bf_cb_args4(178)     => bf_cb_args3(177) bf_cb_args1(174).
	p(NT(178), (NT(177)+NT(174)));
//G249: wff_cb_arg(180)      => wff(14).
	p(NT(180), (NT(14)));
//G250: wff_cb_args1(179)    => __(58) wff_cb_arg(180).
	p(NT(179), (NT(58)+NT(180)));
//G251: wff_cb_args2(181)    => wff_cb_args1(179) wff_cb_args1(179).
	p(NT(181), (NT(179)+NT(179)));
//G252: wff_cb_args3(182)    => wff_cb_args2(181) wff_cb_args1(179).
	p(NT(182), (NT(181)+NT(179)));
//G253: wff_cb_args4(183)    => wff_cb_args3(182) wff_cb_args1(179).
	p(NT(183), (NT(182)+NT(179)));
//G254: __E_wff_cb_76(185)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(181).
	p(NT(185), (T(19)+T(42)+T(42)+T(43)+T(44)+T(17)+T(10)+T(43)+T(45)+T(18)+T(17)+T(10)+T(44)+T(15)+T(46)+T(47)+T(43)+T(10)+T(48)+T(49)+T(42)+T(11)+T(50)+T(12)+T(48)+T(18)+T(17)+T(10)+T(43)+T(45)+T(49)+NT(181)));
//G255: wff_has_clashing_subformulas_cb(184) => __E_wff_cb_76(185).
	p(NT(184), (NT(185)));
//G256: wff_cb(49)           => wff_has_clashing_subformulas_cb(184).
	p(NT(49), (NT(184)));
//G257: __E_wff_cb_77(187)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(182).
	p(NT(187), (T(19)+T(42)+T(42)+T(43)+T(44)+T(17)+T(10)+T(43)+T(10)+T(48)+T(49)+T(42)+T(11)+T(50)+T(12)+T(48)+T(18)+T(17)+T(43)+T(45)+T(49)+NT(182)));
//G258: wff_has_subformula_cb(186) => __E_wff_cb_77(187).
	p(NT(186), (NT(187)));
//G259: wff_cb(49)           => wff_has_subformula_cb(186).
	p(NT(49), (NT(186)));
//G260: __E_wff_cb_78(189)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(181).
	p(NT(189), (T(19)+T(42)+T(42)+T(43)+T(50)+T(13)+T(12)+T(11)+T(51)+T(13)+T(43)+T(13)+T(24)+T(15)+T(10)+T(14)+T(13)+T(46)+T(14)+T(15)+T(17)+T(18)+T(43)+T(45)+T(49)+NT(181)));
//G261: wff_remove_existential_cb(188) => __E_wff_cb_78(189).
	p(NT(188), (NT(189)));
//G262: wff_cb(49)           => wff_remove_existential_cb(188).
	p(NT(49), (NT(188)));
//G263: __E_bf_cb_79(191)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(177).
	p(NT(191), (T(49)+T(42)+T(43)+T(44)+T(17)+T(10)+T(43)+T(10)+T(48)+T(49)+T(42)+T(11)+T(50)+T(12)+T(48)+T(18)+T(17)+T(43)+T(45)+T(49)+NT(177)));
//G264: bf_has_subformula_cb(190) => __E_bf_cb_79(191).
	p(NT(190), (NT(191)));
//G265: bf_cb(53)            => bf_has_subformula_cb(190).
	p(NT(53), (NT(190)));
//G266: __E_bf_cb_80(193)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(178).
	p(NT(193), (T(49)+T(42)+T(43)+T(50)+T(13)+T(12)+T(11)+T(51)+T(13)+T(43)+T(42)+T(48)+T(46)+T(15)+T(51)+T(13)+T(50)+T(10)+T(17)+T(18)+T(43)+T(45)+T(49)+NT(178)));
//G267: bf_remove_funiversal_cb(192) => __E_bf_cb_80(193).
	p(NT(192), (NT(193)));
//G268: bf_cb(53)            => bf_remove_funiversal_cb(192).
	p(NT(53), (NT(192)));
//G269: __E_bf_cb_81(195)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(178).
	p(NT(195), (T(49)+T(42)+T(43)+T(50)+T(13)+T(12)+T(11)+T(51)+T(13)+T(43)+T(42)+T(13)+T(24)+T(15)+T(10)+T(14)+T(13)+T(46)+T(14)+T(15)+T(17)+T(18)+T(43)+T(45)+T(49)+NT(178)));
//G270: bf_remove_fexistential_cb(194) => __E_bf_cb_81(195).
	p(NT(194), (NT(195)));
//G271: bf_cb(53)            => bf_remove_fexistential_cb(194).
	p(NT(53), (NT(194)));
//G272: __E_bf_cb_82(197)    => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(174).
	p(NT(197), (T(49)+T(42)+T(43)+T(46)+T(11)+T(50)+T(12)+T(17)+T(18)+T(15)+T(52)+T(13)+T(43)+T(45)+T(49)+NT(174)));
//G273: bf_normalize_cb(196) => __E_bf_cb_82(197).
	p(NT(196), (NT(197)));
//G274: bf_cb(53)            => bf_normalize_cb(196).
	p(NT(53), (NT(196)));
//G275: cli(198)             => _(11).
	p(NT(198), (NT(11)));
//G276: __E_cli_83(200)      => '.' _(11) cli_command(199) _(11).
	p(NT(200), (T(1)+NT(11)+NT(199)+NT(11)));
//G277: __E_cli_84(201)      => null.
	p(NT(201), (nul));
//G278: __E_cli_84(201)      => __E_cli_83(200) __E_cli_84(201).
	p(NT(201), (NT(200)+NT(201)));
//G279: cli(198)             => _(11) cli_command(199) _(11) __E_cli_84(201).
	p(NT(198), (NT(11)+NT(199)+NT(11)+NT(201)));
//G280: __E_cli_command_85(204) => 'q'.
	p(NT(204), (T(53)));
//G281: __E_cli_command_85(204) => 'q' 'u' 'i' 't'.
	p(NT(204), (T(53)+T(48)+T(15)+T(14)));
//G282: quit_sym(203)        => __E_cli_command_85(204).
	p(NT(203), (NT(204)));
//G283: quit_cmd(202)        => quit_sym(203).
	p(NT(202), (NT(203)));
//G284: cli_command(199)     => quit_cmd(202).
	p(NT(199), (NT(202)));
//G285: __E_cli_command_86(207) => 'v'.
	p(NT(207), (T(51)));
//G286: __E_cli_command_86(207) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(207), (T(51)+T(13)+T(50)+T(10)+T(15)+T(11)+T(46)));
//G287: version_sym(206)     => __E_cli_command_86(207).
	p(NT(206), (NT(207)));
//G288: version_cmd(205)     => version_sym(206).
	p(NT(205), (NT(206)));
//G289: cli_command(199)     => version_cmd(205).
	p(NT(199), (NT(205)));
//G290: __E_cli_command_87(210) => 'c'.
	p(NT(210), (T(45)));
//G291: __E_cli_command_87(210) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(210), (T(45)+T(18)+T(13)+T(17)+T(50)));
//G292: clear_sym(209)       => __E_cli_command_87(210).
	p(NT(209), (NT(210)));
//G293: clear_cmd(208)       => clear_sym(209).
	p(NT(208), (NT(209)));
//G294: cli_command(199)     => clear_cmd(208).
	p(NT(199), (NT(208)));
//G295: __E___E_cli_command_88_89(214) => 'h'.
	p(NT(214), (T(44)));
//G296: __E___E_cli_command_88_89(214) => 'h' 'e' 'l' 'p'.
	p(NT(214), (T(44)+T(13)+T(18)+T(54)));
//G297: help_sym(213)        => __E___E_cli_command_88_89(214).
	p(NT(213), (NT(214)));
//G298: __E___E_cli_command_88_90(215) => __(58) help_arg(216).
	p(NT(215), (NT(58)+NT(216)));
//G299: __E___E_cli_command_88_90(215) => null.
	p(NT(215), (nul));
//G300: __E_cli_command_88(212) => help_sym(213) __E___E_cli_command_88_90(215).
	p(NT(212), (NT(213)+NT(215)));
//G301: help_cmd(211)        => __E_cli_command_88(212).
	p(NT(211), (NT(212)));
//G302: cli_command(199)     => help_cmd(211).
	p(NT(199), (NT(211)));
//G303: file_sym(219)        => 'f' 'i' 'l' 'e'.
	p(NT(219), (T(42)+T(15)+T(18)+T(13)));
//G304: __E_cli_command_91(218) => file_sym(219) __(58) q_string(220).
	p(NT(218), (NT(219)+NT(58)+NT(220)));
//G305: file_cmd(217)        => __E_cli_command_91(218).
	p(NT(217), (NT(218)));
//G306: cli_command(199)     => file_cmd(217).
	p(NT(199), (NT(217)));
//G307: valid_sym(223)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(223), (T(51)+T(17)+T(18)+T(15)+T(55)));
//G308: __E_cli_command_92(222) => valid_sym(223) __(58) rr(10).
	p(NT(222), (NT(223)+NT(58)+NT(10)));
//G309: valid_cmd(221)       => __E_cli_command_92(222).
	p(NT(221), (NT(222)));
//G310: cli_command(199)     => valid_cmd(221).
	p(NT(199), (NT(221)));
//G311: sat_sym(226)         => 's' 'a' 't'.
	p(NT(226), (T(10)+T(17)+T(14)));
//G312: __E_cli_command_93(225) => sat_sym(226) __(58) rr(10).
	p(NT(225), (NT(226)+NT(58)+NT(10)));
//G313: sat_cmd(224)         => __E_cli_command_93(225).
	p(NT(224), (NT(225)));
//G314: cli_command(199)     => sat_cmd(224).
	p(NT(199), (NT(224)));
//G315: unsat_sym(229)       => 'u' 'n' 's' 'a' 't'.
	p(NT(229), (T(48)+T(46)+T(10)+T(17)+T(14)));
//G316: __E_cli_command_94(228) => unsat_sym(229) __(58) rr(10).
	p(NT(228), (NT(229)+NT(58)+NT(10)));
//G317: unsat_cmd(227)       => __E_cli_command_94(228).
	p(NT(227), (NT(228)));
//G318: cli_command(199)     => unsat_cmd(227).
	p(NT(199), (NT(227)));
//G319: solve_sym(232)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(232), (T(10)+T(11)+T(18)+T(51)+T(13)));
//G320: __E___E_cli_command_95_96(234) => ',' _(11) solve_cmd_arg(233).
	p(NT(234), (T(4)+NT(11)+NT(233)));
//G321: __E___E_cli_command_95_97(235) => __E___E_cli_command_95_96(234).
	p(NT(235), (NT(234)));
//G322: __E___E_cli_command_95_97(235) => __E___E_cli_command_95_96(234) __E___E_cli_command_95_97(235).
	p(NT(235), (NT(234)+NT(235)));
//G323: __E_cli_command_95(231) => solve_sym(232) __(58) solve_cmd_arg(233) __E___E_cli_command_95_97(235).
	p(NT(231), (NT(232)+NT(58)+NT(233)+NT(235)));
//G324: solve_cmd(230)       => __E_cli_command_95(231).
	p(NT(230), (NT(231)));
//G325: cli_command(199)     => solve_cmd(230).
	p(NT(199), (NT(230)));
//G326: __E___E_cli_command_98_99(239) => 'e'.
	p(NT(239), (T(13)));
//G327: __E___E_cli_command_98_99(239) => 'e' 'x' 'e' 'c' 'u' 't' 'e'.
	p(NT(239), (T(13)+T(24)+T(13)+T(45)+T(48)+T(14)+T(13)));
//G328: execute_sym(238)     => __E___E_cli_command_98_99(239).
	p(NT(238), (NT(239)));
//G329: __E___E_cli_command_98_100(240) => wff_typed(241).
	p(NT(240), (NT(241)));
//G330: __E___E_cli_command_98_100(240) => rr(10).
	p(NT(240), (NT(10)));
//G331: __E_cli_command_98(237) => execute_sym(238) __(58) __E___E_cli_command_98_100(240).
	p(NT(237), (NT(238)+NT(58)+NT(240)));
//G332: execute_cmd(236)     => __E_cli_command_98(237).
	p(NT(236), (NT(237)));
//G333: cli_command(199)     => execute_cmd(236).
	p(NT(199), (NT(236)));
//G334: __E___E_cli_command_101_102(245) => 'n'.
	p(NT(245), (T(46)));
//G335: __E___E_cli_command_101_102(245) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(245), (T(46)+T(11)+T(50)+T(12)+T(17)+T(18)+T(15)+T(52)+T(13)));
//G336: normalize_sym(244)   => __E___E_cli_command_101_102(245).
	p(NT(244), (NT(245)));
//G337: __E_cli_command_101(243) => normalize_sym(244) __(58) normalize_cmd_arg(246).
	p(NT(243), (NT(244)+NT(58)+NT(246)));
//G338: normalize_cmd(242)   => __E_cli_command_101(243).
	p(NT(242), (NT(243)));
//G339: cli_command(199)     => normalize_cmd(242).
	p(NT(199), (NT(242)));
//G340: __E___E_cli_command_103_104(250) => 's'.
	p(NT(250), (T(10)));
//G341: __E___E_cli_command_103_104(250) => 's' 'u' 'b' 's' 't'.
	p(NT(250), (T(10)+T(48)+T(49)+T(10)+T(14)));
//G342: __E___E_cli_command_103_104(250) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(250), (T(10)+T(48)+T(49)+T(10)+T(14)+T(15)+T(14)+T(48)+T(14)+T(13)));
//G343: subst_sym(249)       => __E___E_cli_command_103_104(250).
	p(NT(249), (NT(250)));
//G344: __E_cli_command_103(248) => subst_sym(249) __(58) nf_cmd_arg(251) _(11) '[' _(11) nf_cmd_arg(251) _(11) '/' _(11) nf_cmd_arg(251) _(11) ']'.
	p(NT(248), (NT(249)+NT(58)+NT(251)+NT(11)+T(21)+NT(11)+NT(251)+NT(11)+T(56)+NT(11)+NT(251)+NT(11)+T(22)));
//G345: subst_cmd(247)       => __E_cli_command_103(248).
	p(NT(247), (NT(248)));
//G346: cli_command(199)     => subst_cmd(247).
	p(NT(199), (NT(247)));
//G347: __E___E_cli_command_105_106(255) => 'i'.
	p(NT(255), (T(15)));
//G348: __E___E_cli_command_105_106(255) => 'i' 'n' 's' 't'.
	p(NT(255), (T(15)+T(46)+T(10)+T(14)));
//G349: __E___E_cli_command_105_106(255) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(255), (T(15)+T(46)+T(10)+T(14)+T(17)+T(46)+T(14)+T(15)+T(17)+T(14)+T(13)));
//G350: inst_sym(254)        => __E___E_cli_command_105_106(255).
	p(NT(254), (NT(255)));
//G351: __E_cli_command_105(253) => inst_sym(254) __(58) inst_args(256).
	p(NT(253), (NT(254)+NT(58)+NT(256)));
//G352: inst_cmd(252)        => __E_cli_command_105(253).
	p(NT(252), (NT(253)));
//G353: cli_command(199)     => inst_cmd(252).
	p(NT(199), (NT(252)));
//G354: dnf_sym(259)         => 'd' 'n' 'f'.
	p(NT(259), (T(55)+T(46)+T(42)));
//G355: __E_cli_command_107(258) => dnf_sym(259) __(58) nf_cmd_arg(251).
	p(NT(258), (NT(259)+NT(58)+NT(251)));
//G356: dnf_cmd(257)         => __E_cli_command_107(258).
	p(NT(257), (NT(258)));
//G357: cli_command(199)     => dnf_cmd(257).
	p(NT(199), (NT(257)));
//G358: cnf_sym(262)         => 'c' 'n' 'f'.
	p(NT(262), (T(45)+T(46)+T(42)));
//G359: __E_cli_command_108(261) => cnf_sym(262) __(58) nf_cmd_arg(251).
	p(NT(261), (NT(262)+NT(58)+NT(251)));
//G360: cnf_cmd(260)         => __E_cli_command_108(261).
	p(NT(260), (NT(261)));
//G361: cli_command(199)     => cnf_cmd(260).
	p(NT(199), (NT(260)));
//G362: anf_sym(265)         => 'a' 'n' 'f'.
	p(NT(265), (T(17)+T(46)+T(42)));
//G363: __E_cli_command_109(264) => anf_sym(265) __(58) nf_cmd_arg(251).
	p(NT(264), (NT(265)+NT(58)+NT(251)));
//G364: anf_cmd(263)         => __E_cli_command_109(264).
	p(NT(263), (NT(264)));
//G365: cli_command(199)     => anf_cmd(263).
	p(NT(199), (NT(263)));
//G366: nnf_sym(268)         => 'n' 'n' 'f'.
	p(NT(268), (T(46)+T(46)+T(42)));
//G367: __E_cli_command_110(267) => nnf_sym(268) __(58) nf_cmd_arg(251).
	p(NT(267), (NT(268)+NT(58)+NT(251)));
//G368: nnf_cmd(266)         => __E_cli_command_110(267).
	p(NT(266), (NT(267)));
//G369: cli_command(199)     => nnf_cmd(266).
	p(NT(199), (NT(266)));
//G370: pnf_sym(271)         => 'p' 'n' 'f'.
	p(NT(271), (T(54)+T(46)+T(42)));
//G371: __E_cli_command_111(270) => pnf_sym(271) __(58) nf_cmd_arg(251).
	p(NT(270), (NT(271)+NT(58)+NT(251)));
//G372: pnf_cmd(269)         => __E_cli_command_111(270).
	p(NT(269), (NT(270)));
//G373: cli_command(199)     => pnf_cmd(269).
	p(NT(199), (NT(269)));
//G374: mnf_sym(274)         => 'm' 'n' 'f'.
	p(NT(274), (T(12)+T(46)+T(42)));
//G375: __E_cli_command_112(273) => mnf_sym(274) __(58) nf_cmd_arg(251).
	p(NT(273), (NT(274)+NT(58)+NT(251)));
//G376: mnf_cmd(272)         => __E_cli_command_112(273).
	p(NT(272), (NT(273)));
//G377: cli_command(199)     => mnf_cmd(272).
	p(NT(199), (NT(272)));
//G378: snf_sym(277)         => 's' 'n' 'f'.
	p(NT(277), (T(10)+T(46)+T(42)));
//G379: __E_cli_command_113(276) => snf_sym(277) __(58) nf_cmd_arg(251).
	p(NT(276), (NT(277)+NT(58)+NT(251)));
//G380: snf_cmd(275)         => __E_cli_command_113(276).
	p(NT(275), (NT(276)));
//G381: cli_command(199)     => snf_cmd(275).
	p(NT(199), (NT(275)));
//G382: onf_sym(280)         => 'o' 'n' 'f'.
	p(NT(280), (T(11)+T(46)+T(42)));
//G383: __E_cli_command_114(279) => onf_sym(280) __(58) variable(111) __(58) onf_cmd_arg(281).
	p(NT(279), (NT(280)+NT(58)+NT(111)+NT(58)+NT(281)));
//G384: onf_cmd(278)         => __E_cli_command_114(279).
	p(NT(278), (NT(279)));
//G385: cli_command(199)     => onf_cmd(278).
	p(NT(199), (NT(278)));
//G386: __E___E_cli_command_115_116(285) => 'd' 'e' 'f' 's'.
	p(NT(285), (T(55)+T(13)+T(42)+T(10)));
//G387: __E___E_cli_command_115_116(285) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(285), (T(55)+T(13)+T(42)+T(15)+T(46)+T(15)+T(14)+T(15)+T(11)+T(46)+T(10)));
//G388: def_sym(284)         => __E___E_cli_command_115_116(285).
	p(NT(284), (NT(285)));
//G389: __E_cli_command_115(283) => def_sym(284).
	p(NT(283), (NT(284)));
//G390: def_list_cmd(282)    => __E_cli_command_115(283).
	p(NT(282), (NT(283)));
//G391: cli_command(199)     => def_list_cmd(282).
	p(NT(199), (NT(282)));
//G392: __E_cli_command_117(287) => def_sym(284) __(58) number(288).
	p(NT(287), (NT(284)+NT(58)+NT(288)));
//G393: def_print_cmd(286)   => __E_cli_command_117(287).
	p(NT(286), (NT(287)));
//G394: cli_command(199)     => def_print_cmd(286).
	p(NT(199), (NT(286)));
//G395: def_rr_cmd(289)      => rec_relation(16).
	p(NT(289), (NT(16)));
//G396: cli_command(199)     => def_rr_cmd(289).
	p(NT(199), (NT(289)));
//G397: qelim_sym(292)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(292), (T(53)+T(13)+T(18)+T(15)+T(12)));
//G398: __E_cli_command_118(291) => qelim_sym(292) __(58) wff_cmd_arg(293).
	p(NT(291), (NT(292)+NT(58)+NT(293)));
//G399: qelim_cmd(290)       => __E_cli_command_118(291).
	p(NT(290), (NT(291)));
//G400: cli_command(199)     => qelim_cmd(290).
	p(NT(199), (NT(290)));
//G401: get_sym(296)         => 'g' 'e' 't'.
	p(NT(296), (T(47)+T(13)+T(14)));
//G402: __E___E_cli_command_119_120(297) => __(58) option(298).
	p(NT(297), (NT(58)+NT(298)));
//G403: __E___E_cli_command_119_120(297) => null.
	p(NT(297), (nul));
//G404: __E_cli_command_119(295) => get_sym(296) __E___E_cli_command_119_120(297).
	p(NT(295), (NT(296)+NT(297)));
//G405: get_cmd(294)         => __E_cli_command_119(295).
	p(NT(294), (NT(295)));
//G406: cli_command(199)     => get_cmd(294).
	p(NT(199), (NT(294)));
//G407: set_sym(301)         => 's' 'e' 't'.
	p(NT(301), (T(10)+T(13)+T(14)));
//G408: __E___E_cli_command_121_122(302) => __(58).
	p(NT(302), (NT(58)));
//G409: __E___E_cli_command_121_122(302) => _(11) '=' _(11).
	p(NT(302), (NT(11)+T(3)+NT(11)));
//G410: __E_cli_command_121(300) => set_sym(301) __(58) option(298) __E___E_cli_command_121_122(302) option_value(303).
	p(NT(300), (NT(301)+NT(58)+NT(298)+NT(302)+NT(303)));
//G411: set_cmd(299)         => __E_cli_command_121(300).
	p(NT(299), (NT(300)));
//G412: cli_command(199)     => set_cmd(299).
	p(NT(199), (NT(299)));
//G413: toggle_sym(306)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(306), (T(14)+T(11)+T(47)+T(47)+T(18)+T(13)));
//G414: __E_cli_command_123(305) => toggle_sym(306) __(58) bool_option(307).
	p(NT(305), (NT(306)+NT(58)+NT(307)));
//G415: toggle_cmd(304)      => __E_cli_command_123(305).
	p(NT(304), (NT(305)));
//G416: cli_command(199)     => toggle_cmd(304).
	p(NT(199), (NT(304)));
//G417: __E___E_cli_command_124_125(311) => 'h' 'i' 's' 't'.
	p(NT(311), (T(44)+T(15)+T(10)+T(14)));
//G418: __E___E_cli_command_124_125(311) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(311), (T(44)+T(15)+T(10)+T(14)+T(11)+T(50)+T(20)));
//G419: history_sym(310)     => __E___E_cli_command_124_125(311).
	p(NT(310), (NT(311)));
//G420: __E_cli_command_124(309) => history_sym(310).
	p(NT(309), (NT(310)));
//G421: history_list_cmd(308) => __E_cli_command_124(309).
	p(NT(308), (NT(309)));
//G422: cli_command(199)     => history_list_cmd(308).
	p(NT(199), (NT(308)));
//G423: __E_cli_command_126(313) => history_sym(310) __(58) memory(314).
	p(NT(313), (NT(310)+NT(58)+NT(314)));
//G424: history_print_cmd(312) => __E_cli_command_126(313).
	p(NT(312), (NT(313)));
//G425: cli_command(199)     => history_print_cmd(312).
	p(NT(199), (NT(312)));
//G426: __E_cli_command_127(316) => bf_typed(317).
	p(NT(316), (NT(317)));
//G427: __E_cli_command_127(316) => wff_typed(241).
	p(NT(316), (NT(241)));
//G428: history_store_cmd(315) => __E_cli_command_127(316).
	p(NT(315), (NT(316)));
//G429: cli_command(199)     => history_store_cmd(315).
	p(NT(199), (NT(315)));
//G430: number(288)          => digits(137).
	p(NT(288), (NT(137)));
//G431: bf_typed(317)        => bf(21).
	p(NT(317), (NT(21)));
//G432: bf_typed(317)        => bf(21) _(11) ':' _(11) 'b' 'f'.
	p(NT(317), (NT(21)+NT(11)+T(2)+NT(11)+T(49)+T(42)));
//G433: wff_typed(241)       => wff(14).
	p(NT(241), (NT(14)));
//G434: wff_typed(241)       => wff(14) _(11) ':' _(11) 'w' 'f' 'f'.
	p(NT(241), (NT(14)+NT(11)+T(2)+NT(11)+T(19)+T(42)+T(42)));
//G435: bf_cmd_arg(318)      => memory(314).
	p(NT(318), (NT(314)));
//G436: bf_cmd_arg(318)      => bf_typed(317).
	p(NT(318), (NT(317)));
//G437: wff_cmd_arg(293)     => memory(314).
	p(NT(293), (NT(314)));
//G438: wff_cmd_arg(293)     => wff_typed(241).
	p(NT(293), (NT(241)));
//G439: nf_cmd_arg(251)      => memory(314).
	p(NT(251), (NT(314)));
//G440: nf_cmd_arg(251)      => bf_typed(317).
	p(NT(251), (NT(317)));
//G441: nf_cmd_arg(251)      => wff_typed(241).
	p(NT(251), (NT(241)));
//G442: onf_cmd_arg(281)     => memory(314).
	p(NT(281), (NT(314)));
//G443: onf_cmd_arg(281)     => wff_typed(241).
	p(NT(281), (NT(241)));
//G444: normalize_cmd_arg(246) => memory(314).
	p(NT(246), (NT(314)));
//G445: normalize_cmd_arg(246) => rr(10).
	p(NT(246), (NT(10)));
//G446: normalize_cmd_arg(246) => ref(18).
	p(NT(246), (NT(18)));
//G447: normalize_cmd_arg(246) => bf_typed(317).
	p(NT(246), (NT(317)));
//G448: normalize_cmd_arg(246) => wff_typed(241).
	p(NT(246), (NT(241)));
//G449: solve_cmd_arg(233)   => memory(314).
	p(NT(233), (NT(314)));
//G450: solve_cmd_arg(233)   => bf_eq(98).
	p(NT(233), (NT(98)));
//G451: solve_cmd_arg(233)   => bf_neq(96).
	p(NT(233), (NT(96)));
//G452: inst_args(256)       => bf_cmd_arg(318) _(11) '[' _(11) variable(111) _(11) '/' _(11) bf_cmd_arg(318) _(11) ']'.
	p(NT(256), (NT(318)+NT(11)+T(21)+NT(11)+NT(111)+NT(11)+T(56)+NT(11)+NT(318)+NT(11)+T(22)));
//G453: inst_args(256)       => wff_cmd_arg(293) _(11) '[' _(11) variable(111) _(11) '/' _(11) bf_cmd_arg(318) _(11) ']'.
	p(NT(256), (NT(293)+NT(11)+T(21)+NT(11)+NT(111)+NT(11)+T(56)+NT(11)+NT(318)+NT(11)+T(22)));
//G454: help_arg(216)        => help_sym(213).
	p(NT(216), (NT(213)));
//G455: help_arg(216)        => version_sym(206).
	p(NT(216), (NT(206)));
//G456: help_arg(216)        => quit_sym(203).
	p(NT(216), (NT(203)));
//G457: help_arg(216)        => clear_sym(209).
	p(NT(216), (NT(209)));
//G458: help_arg(216)        => get_sym(296).
	p(NT(216), (NT(296)));
//G459: help_arg(216)        => set_sym(301).
	p(NT(216), (NT(301)));
//G460: help_arg(216)        => toggle_sym(306).
	p(NT(216), (NT(306)));
//G461: help_arg(216)        => file_sym(219).
	p(NT(216), (NT(219)));
//G462: help_arg(216)        => history_sym(310).
	p(NT(216), (NT(310)));
//G463: help_arg(216)        => abs_memory_sym(319).
	p(NT(216), (NT(319)));
//G464: help_arg(216)        => rel_memory_sym(320).
	p(NT(216), (NT(320)));
//G465: help_arg(216)        => selection_sym(321).
	p(NT(216), (NT(321)));
//G466: help_arg(216)        => def_sym(284).
	p(NT(216), (NT(284)));
//G467: help_arg(216)        => inst_sym(254).
	p(NT(216), (NT(254)));
//G468: help_arg(216)        => subst_sym(249).
	p(NT(216), (NT(249)));
//G469: help_arg(216)        => normalize_sym(244).
	p(NT(216), (NT(244)));
//G470: help_arg(216)        => execute_sym(238).
	p(NT(216), (NT(238)));
//G471: help_arg(216)        => solve_sym(232).
	p(NT(216), (NT(232)));
//G472: help_arg(216)        => valid_sym(223).
	p(NT(216), (NT(223)));
//G473: help_arg(216)        => sat_sym(226).
	p(NT(216), (NT(226)));
//G474: help_arg(216)        => unsat_sym(229).
	p(NT(216), (NT(229)));
//G475: help_arg(216)        => dnf_sym(259).
	p(NT(216), (NT(259)));
//G476: help_arg(216)        => cnf_sym(262).
	p(NT(216), (NT(262)));
//G477: help_arg(216)        => anf_sym(265).
	p(NT(216), (NT(265)));
//G478: help_arg(216)        => snf_sym(277).
	p(NT(216), (NT(277)));
//G479: help_arg(216)        => nnf_sym(268).
	p(NT(216), (NT(268)));
//G480: help_arg(216)        => pnf_sym(271).
	p(NT(216), (NT(271)));
//G481: help_arg(216)        => mnf_sym(274).
	p(NT(216), (NT(274)));
//G482: help_arg(216)        => onf_sym(280).
	p(NT(216), (NT(280)));
//G483: help_arg(216)        => qelim_sym(292).
	p(NT(216), (NT(292)));
//G484: __E___E_help_arg_128_129(324) => 's'.
	p(NT(324), (T(10)));
//G485: __E___E_help_arg_128_129(324) => null.
	p(NT(324), (nul));
//G486: __E_help_arg_128(323) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_128_129(324).
	p(NT(323), (T(13)+T(24)+T(17)+T(12)+T(54)+T(18)+T(13)+NT(324)));
//G487: examples_sym(322)    => __E_help_arg_128(323).
	p(NT(322), (NT(323)));
//G488: help_arg(216)        => examples_sym(322).
	p(NT(216), (NT(322)));
//G489: __E_memory_130(326)  => '%'.
	p(NT(326), (T(57)));
//G490: rel_memory_sym(320)  => '%' '-'.
	p(NT(320), (T(57)+T(25)));
//G491: memory_id(328)       => digits(137).
	p(NT(328), (NT(137)));
//G492: __E___E_memory_130_131(327) => memory_id(328).
	p(NT(327), (NT(328)));
//G493: __E___E_memory_130_131(327) => null.
	p(NT(327), (nul));
//G494: __E_memory_130(326)  => rel_memory_sym(320) __E___E_memory_130_131(327).
	p(NT(326), (NT(320)+NT(327)));
//G495: rel_memory(325)      => __E_memory_130(326).
	p(NT(325), (NT(326)));
//G496: rel_memory(325)      => rel_memory(325).
	p(NT(325), (NT(325)));
//G497: memory(314)          => rel_memory(325).
	p(NT(314), (NT(325)));
//G498: abs_memory_sym(319)  => '%'.
	p(NT(319), (T(57)));
//G499: __E_memory_132(330)  => abs_memory_sym(319) memory_id(328).
	p(NT(330), (NT(319)+NT(328)));
//G500: abs_memory(329)      => __E_memory_132(330).
	p(NT(329), (NT(330)));
//G501: memory(314)          => abs_memory(329).
	p(NT(314), (NT(329)));
//G502: option(298)          => bool_option(307).
	p(NT(298), (NT(307)));
//G503: __E_option_133(333)  => 's' 'e' 'v'.
	p(NT(333), (T(10)+T(13)+T(51)));
//G504: __E_option_133(333)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(333), (T(10)+T(13)+T(51)+T(13)+T(50)+T(15)+T(14)+T(20)));
//G505: severity_opt(332)    => __E_option_133(333).
	p(NT(332), (NT(333)));
//G506: enum_option(331)     => severity_opt(332).
	p(NT(331), (NT(332)));
//G507: option(298)          => enum_option(331).
	p(NT(298), (NT(331)));
//G508: __E_bool_option_134(335) => 's'.
	p(NT(335), (T(10)));
//G509: __E_bool_option_134(335) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(335), (T(10)+T(14)+T(17)+T(14)+T(48)+T(10)));
//G510: status_opt(334)      => __E_bool_option_134(335).
	p(NT(334), (NT(335)));
//G511: bool_option(307)     => status_opt(334).
	p(NT(307), (NT(334)));
//G512: __E_bool_option_135(337) => 'c'.
	p(NT(337), (T(45)));
//G513: __E_bool_option_135(337) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(337), (T(45)+T(11)+T(18)+T(11)+T(50)));
//G514: __E_bool_option_135(337) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(337), (T(45)+T(11)+T(18)+T(11)+T(50)+T(10)));
//G515: colors_opt(336)      => __E_bool_option_135(337).
	p(NT(336), (NT(337)));
//G516: bool_option(307)     => colors_opt(336).
	p(NT(307), (NT(336)));
//G517: __E_bool_option_136(339) => 'd'.
	p(NT(339), (T(55)));
//G518: __E_bool_option_136(339) => 'd' 'b' 'g'.
	p(NT(339), (T(55)+T(49)+T(47)));
//G519: __E_bool_option_136(339) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(339), (T(55)+T(13)+T(49)+T(48)+T(47)));
//G520: debug_repl_opt(338)  => __E_bool_option_136(339).
	p(NT(338), (NT(339)));
//G521: bool_option(307)     => debug_repl_opt(338).
	p(NT(307), (NT(338)));
//G522: option_value(303)    => option_value_true(340).
	p(NT(303), (NT(340)));
//G523: option_value(303)    => option_value_false(341).
	p(NT(303), (NT(341)));
//G524: option_value(303)    => severity(342).
	p(NT(303), (NT(342)));
//G525: option_value_true(340) => 't'.
	p(NT(340), (T(14)));
//G526: option_value_true(340) => 't' 'r' 'u' 'e'.
	p(NT(340), (T(14)+T(50)+T(48)+T(13)));
//G527: option_value_true(340) => 'o' 'n'.
	p(NT(340), (T(11)+T(46)));
//G528: option_value_true(340) => '1'.
	p(NT(340), (T(35)));
//G529: option_value_true(340) => 'y'.
	p(NT(340), (T(20)));
//G530: option_value_true(340) => 'y' 'e' 's'.
	p(NT(340), (T(20)+T(13)+T(10)));
//G531: option_value_false(341) => 'f'.
	p(NT(341), (T(42)));
//G532: option_value_false(341) => 'f' 'a' 'l' 's' 'e'.
	p(NT(341), (T(42)+T(17)+T(18)+T(10)+T(13)));
//G533: option_value_false(341) => 'o' 'f' 'f'.
	p(NT(341), (T(11)+T(42)+T(42)));
//G534: option_value_false(341) => '0'.
	p(NT(341), (T(36)));
//G535: option_value_false(341) => 'n'.
	p(NT(341), (T(46)));
//G536: option_value_false(341) => 'n' 'o'.
	p(NT(341), (T(46)+T(11)));
//G537: __E_severity_137(344) => 'e'.
	p(NT(344), (T(13)));
//G538: __E_severity_137(344) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(344), (T(13)+T(50)+T(50)+T(11)+T(50)));
//G539: error_sym(343)       => __E_severity_137(344).
	p(NT(343), (NT(344)));
//G540: severity(342)        => error_sym(343).
	p(NT(342), (NT(343)));
//G541: __E_severity_138(346) => 'i'.
	p(NT(346), (T(15)));
//G542: __E_severity_138(346) => 'i' 'n' 'f' 'o'.
	p(NT(346), (T(15)+T(46)+T(42)+T(11)));
//G543: info_sym(345)        => __E_severity_138(346).
	p(NT(345), (NT(346)));
//G544: severity(342)        => info_sym(345).
	p(NT(342), (NT(345)));
//G545: __E_severity_139(348) => 'd'.
	p(NT(348), (T(55)));
//G546: __E_severity_139(348) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(348), (T(55)+T(13)+T(49)+T(48)+T(47)));
//G547: debug_sym(347)       => __E_severity_139(348).
	p(NT(347), (NT(348)));
//G548: severity(342)        => debug_sym(347).
	p(NT(342), (NT(347)));
//G549: __E_severity_140(350) => 't'.
	p(NT(350), (T(14)));
//G550: __E_severity_140(350) => 't' 'r' 'a' 'c' 'e'.
	p(NT(350), (T(14)+T(50)+T(17)+T(45)+T(13)));
//G551: trace_sym(349)       => __E_severity_140(350).
	p(NT(349), (NT(350)));
//G552: severity(342)        => trace_sym(349).
	p(NT(342), (NT(349)));
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
		__E_bf_45, __E___E_bf_45_46, bf_neg, __E_bf_47, bf_constant, __E_bf_48, constant, bf_t, bf_f, charvar, 
		__E_charvar_49, __E_charvar_50, io_var, __E_variable_51, out, in_var_name, __E_in_52, digits, offset, out_var_name, 
		__E_out_53, q_var, __E_q_vars_54, __E_q_vars_55, __E_offsets_56, __E_offsets_57, num, shift, __E_offset_58, __E_shift_59, 
		__E___E_shift_59_60, uninter_const_name, __E_uninterpreted_constant_61, __E___E_uninterpreted_constant_61_62, chars, binding, __E_binding_63, type, __E___E_binding_63_64, source, 
		named_binding, src_c, __E_source_65, __E_source_66, __E___E_source_66_67, __E___E_source_66_68, __E_chars_69, __E_digits_70, comment, __E_comment_71, 
		__E_comment_72, __E_comment_73, __E____74, __E___75, bf_cb_args1, bf_cb_arg, bf_cb_args2, bf_cb_args3, bf_cb_args4, wff_cb_args1, 
		wff_cb_arg, wff_cb_args2, wff_cb_args3, wff_cb_args4, wff_has_clashing_subformulas_cb, __E_wff_cb_76, wff_has_subformula_cb, __E_wff_cb_77, wff_remove_existential_cb, __E_wff_cb_78, 
		bf_has_subformula_cb, __E_bf_cb_79, bf_remove_funiversal_cb, __E_bf_cb_80, bf_remove_fexistential_cb, __E_bf_cb_81, bf_normalize_cb, __E_bf_cb_82, cli, cli_command, 
		__E_cli_83, __E_cli_84, quit_cmd, quit_sym, __E_cli_command_85, version_cmd, version_sym, __E_cli_command_86, clear_cmd, clear_sym, 
		__E_cli_command_87, help_cmd, __E_cli_command_88, help_sym, __E___E_cli_command_88_89, __E___E_cli_command_88_90, help_arg, file_cmd, __E_cli_command_91, file_sym, 
		q_string, valid_cmd, __E_cli_command_92, valid_sym, sat_cmd, __E_cli_command_93, sat_sym, unsat_cmd, __E_cli_command_94, unsat_sym, 
		solve_cmd, __E_cli_command_95, solve_sym, solve_cmd_arg, __E___E_cli_command_95_96, __E___E_cli_command_95_97, execute_cmd, __E_cli_command_98, execute_sym, __E___E_cli_command_98_99, 
		__E___E_cli_command_98_100, wff_typed, normalize_cmd, __E_cli_command_101, normalize_sym, __E___E_cli_command_101_102, normalize_cmd_arg, subst_cmd, __E_cli_command_103, subst_sym, 
		__E___E_cli_command_103_104, nf_cmd_arg, inst_cmd, __E_cli_command_105, inst_sym, __E___E_cli_command_105_106, inst_args, dnf_cmd, __E_cli_command_107, dnf_sym, 
		cnf_cmd, __E_cli_command_108, cnf_sym, anf_cmd, __E_cli_command_109, anf_sym, nnf_cmd, __E_cli_command_110, nnf_sym, pnf_cmd, 
		__E_cli_command_111, pnf_sym, mnf_cmd, __E_cli_command_112, mnf_sym, snf_cmd, __E_cli_command_113, snf_sym, onf_cmd, __E_cli_command_114, 
		onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_115, def_sym, __E___E_cli_command_115_116, def_print_cmd, __E_cli_command_117, number, def_rr_cmd, 
		qelim_cmd, __E_cli_command_118, qelim_sym, wff_cmd_arg, get_cmd, __E_cli_command_119, get_sym, __E___E_cli_command_119_120, option, set_cmd, 
		__E_cli_command_121, set_sym, __E___E_cli_command_121_122, option_value, toggle_cmd, __E_cli_command_123, toggle_sym, bool_option, history_list_cmd, __E_cli_command_124, 
		history_sym, __E___E_cli_command_124_125, history_print_cmd, __E_cli_command_126, memory, history_store_cmd, __E_cli_command_127, bf_typed, bf_cmd_arg, abs_memory_sym, 
		rel_memory_sym, selection_sym, examples_sym, __E_help_arg_128, __E___E_help_arg_128_129, rel_memory, __E_memory_130, __E___E_memory_130_131, memory_id, abs_memory, 
		__E_memory_132, enum_option, severity_opt, __E_option_133, status_opt, __E_bool_option_134, colors_opt, __E_bool_option_135, debug_repl_opt, __E_bool_option_136, 
		option_value_true, option_value_false, severity, error_sym, __E_severity_137, info_sym, __E_severity_138, debug_sym, __E_severity_139, trace_sym, 
		__E_severity_140, __N_0, __N_1, __N_2, __N_3, 
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
