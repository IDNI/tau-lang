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
	"wff_cb_arg", "wff_cb_args2", "wff_cb_args3", "wff_cb_args4", "bf_eq_cb", "__E_wff_cb_76", "bf_neq_cb", "__E_wff_cb_77", "wff_has_clashing_subformulas_cb", "__E_wff_cb_78", 
	"wff_has_subformula_cb", "__E_wff_cb_79", "wff_remove_existential_cb", "__E_wff_cb_80", "bf_is_zero_cb", "__E_bf_cb_81", "bf_is_one_cb", "__E_bf_cb_82", "bf_has_subformula_cb", "__E_bf_cb_83", 
	"bf_remove_funiversal_cb", "__E_bf_cb_84", "bf_remove_fexistential_cb", "__E_bf_cb_85", "bf_and_cb", "__E_bf_cb_86", "bf_or_cb", "__E_bf_cb_87", "bf_xor_cb", "__E_bf_cb_88", 
	"bf_neg_cb", "__E_bf_cb_89", "bf_normalize_cb", "__E_bf_cb_90", "cli", "cli_command", "__E_cli_91", "__E_cli_92", "quit_cmd", "quit_sym", 
	"__E_cli_command_93", "version_cmd", "version_sym", "__E_cli_command_94", "clear_cmd", "clear_sym", "__E_cli_command_95", "help_cmd", "__E_cli_command_96", "help_sym", 
	"__E___E_cli_command_96_97", "__E___E_cli_command_96_98", "help_arg", "file_cmd", "__E_cli_command_99", "file_sym", "q_string", "valid_cmd", "__E_cli_command_100", "valid_sym", 
	"sat_cmd", "__E_cli_command_101", "sat_sym", "unsat_cmd", "__E_cli_command_102", "unsat_sym", "solve_cmd", "__E_cli_command_103", "solve_sym", "solve_cmd_arg", 
	"__E___E_cli_command_103_104", "__E___E_cli_command_103_105", "execute_cmd", "__E_cli_command_106", "execute_sym", "__E___E_cli_command_106_107", "__E___E_cli_command_106_108", "wff_typed", "normalize_cmd", "__E_cli_command_109", 
	"normalize_sym", "__E___E_cli_command_109_110", "normalize_cmd_arg", "subst_cmd", "__E_cli_command_111", "subst_sym", "__E___E_cli_command_111_112", "nf_cmd_arg", "inst_cmd", "__E_cli_command_113", 
	"inst_sym", "__E___E_cli_command_113_114", "inst_args", "dnf_cmd", "__E_cli_command_115", "dnf_sym", "cnf_cmd", "__E_cli_command_116", "cnf_sym", "anf_cmd", 
	"__E_cli_command_117", "anf_sym", "nnf_cmd", "__E_cli_command_118", "nnf_sym", "pnf_cmd", "__E_cli_command_119", "pnf_sym", "mnf_cmd", "__E_cli_command_120", 
	"mnf_sym", "snf_cmd", "__E_cli_command_121", "snf_sym", "onf_cmd", "__E_cli_command_122", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_123", 
	"def_sym", "__E___E_cli_command_123_124", "def_print_cmd", "__E_cli_command_125", "number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_126", "qelim_sym", "wff_cmd_arg", 
	"get_cmd", "__E_cli_command_127", "get_sym", "__E___E_cli_command_127_128", "option", "set_cmd", "__E_cli_command_129", "set_sym", "__E___E_cli_command_129_130", "option_value", 
	"toggle_cmd", "__E_cli_command_131", "toggle_sym", "bool_option", "history_list_cmd", "__E_cli_command_132", "history_sym", "__E___E_cli_command_132_133", "history_print_cmd", "__E_cli_command_134", 
	"memory", "history_store_cmd", "__E_cli_command_135", "bf_typed", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "examples_sym", "__E_help_arg_136", 
	"__E___E_help_arg_136_137", "rel_memory", "__E_memory_138", "__E___E_memory_138_139", "memory_id", "abs_memory", "__E_memory_140", "enum_option", "severity_opt", "__E_option_141", 
	"status_opt", "__E_bool_option_142", "colors_opt", "__E_bool_option_143", "debug_repl_opt", "__E_bool_option_144", "option_value_true", "option_value_false", "severity", "error_sym", 
	"__E_severity_145", "info_sym", "__E_severity_146", "debug_sym", "__E_severity_147", "trace_sym", "__E_severity_148", "__N_0", "__N_1", "__N_2", 
	"__N_3", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', ',', '(', ')', '<', '{', 
	'}', 's', 'o', 'm', 'e', 't', 'i', '>', 'a', 'l', 
	'w', 'y', '[', ']', '?', 'x', '-', '|', '^', '&', 
	'!', 'T', 'F', 'S', '+', '\'', '1', '0', '$', '\t', 
	'\n', '\r', '#', 'b', 'f', '_', 'q', 'c', 'n', 'h', 
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
			{ 257 },
			{ 262 },
			{ 267 },
			{ 272 },
			{ 297 },
			{ 309 },
			{ 333 },
			{ 334 }
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
//G158: __N_0(367)           => 'F'.
	p(NT(367), (T(31)));
//G159: __N_1(368)           => 'T'.
	p(NT(368), (T(30)));
//G160: __E_charvar_49(130)  => ~( __N_0(367) ) & ~( __N_1(368) ) & alpha(5).	 # conjunctive
	p(NT(130), ~(NT(367)) & ~(NT(368)) & (NT(5)));
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
//G189: __N_2(369)           => io_var(132).
	p(NT(369), (NT(132)));
//G190: __E_offset_58(148)   => variable(111) & ~( __N_2(369) ).	 # conjunctive
	p(NT(148), (NT(111)) & ~(NT(369)));
//G191: offset(138)          => __E_offset_58(148).
	p(NT(138), (NT(148)));
//G192: __E_shift_59(149)    => capture(20).
	p(NT(149), (NT(20)));
//G193: __N_3(370)           => io_var(132).
	p(NT(370), (NT(132)));
//G194: __E___E_shift_59_60(150) => variable(111) & ~( __N_3(370) ).	 # conjunctive
	p(NT(150), (NT(111)) & ~(NT(370)));
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
//G254: __E_wff_cb_76(185)   => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b' bf_cb_args1(174) wff_cb_args2(181).
	p(NT(185), (T(42)+T(43)+T(44)+T(13)+T(45)+T(44)+T(46)+T(42)+NT(174)+NT(181)));
//G255: bf_eq_cb(184)        => __E_wff_cb_76(185).
	p(NT(184), (NT(185)));
//G256: wff_cb(49)           => bf_eq_cb(184).
	p(NT(49), (NT(184)));
//G257: __E_wff_cb_77(187)   => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b' bf_cb_args1(174) wff_cb_args2(181).
	p(NT(187), (T(42)+T(43)+T(44)+T(47)+T(13)+T(45)+T(44)+T(46)+T(42)+NT(174)+NT(181)));
//G258: bf_neq_cb(186)       => __E_wff_cb_77(187).
	p(NT(186), (NT(187)));
//G259: wff_cb(49)           => bf_neq_cb(186).
	p(NT(49), (NT(186)));
//G260: __E_wff_cb_78(189)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(181).
	p(NT(189), (T(19)+T(43)+T(43)+T(44)+T(48)+T(17)+T(10)+T(44)+T(46)+T(18)+T(17)+T(10)+T(48)+T(15)+T(47)+T(49)+T(44)+T(10)+T(50)+T(42)+T(43)+T(11)+T(51)+T(12)+T(50)+T(18)+T(17)+T(10)+T(44)+T(46)+T(42)+NT(181)));
//G261: wff_has_clashing_subformulas_cb(188) => __E_wff_cb_78(189).
	p(NT(188), (NT(189)));
//G262: wff_cb(49)           => wff_has_clashing_subformulas_cb(188).
	p(NT(49), (NT(188)));
//G263: __E_wff_cb_79(191)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(182).
	p(NT(191), (T(19)+T(43)+T(43)+T(44)+T(48)+T(17)+T(10)+T(44)+T(10)+T(50)+T(42)+T(43)+T(11)+T(51)+T(12)+T(50)+T(18)+T(17)+T(44)+T(46)+T(42)+NT(182)));
//G264: wff_has_subformula_cb(190) => __E_wff_cb_79(191).
	p(NT(190), (NT(191)));
//G265: wff_cb(49)           => wff_has_subformula_cb(190).
	p(NT(49), (NT(190)));
//G266: __E_wff_cb_80(193)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(181).
	p(NT(193), (T(19)+T(43)+T(43)+T(44)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(44)+T(13)+T(24)+T(15)+T(10)+T(14)+T(13)+T(47)+T(14)+T(15)+T(17)+T(18)+T(44)+T(46)+T(42)+NT(181)));
//G267: wff_remove_existential_cb(192) => __E_wff_cb_80(193).
	p(NT(192), (NT(193)));
//G268: wff_cb(49)           => wff_remove_existential_cb(192).
	p(NT(49), (NT(192)));
//G269: __E_bf_cb_81(195)    => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b' bf_cb_args2(176).
	p(NT(195), (T(42)+T(43)+T(44)+T(15)+T(10)+T(44)+T(53)+T(13)+T(51)+T(11)+T(44)+T(46)+T(42)+NT(176)));
//G270: bf_is_zero_cb(194)   => __E_bf_cb_81(195).
	p(NT(194), (NT(195)));
//G271: bf_cb(53)            => bf_is_zero_cb(194).
	p(NT(53), (NT(194)));
//G272: __E_bf_cb_82(197)    => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b' bf_cb_args2(176).
	p(NT(197), (T(42)+T(43)+T(44)+T(15)+T(10)+T(44)+T(11)+T(47)+T(13)+T(44)+T(46)+T(42)+NT(176)));
//G273: bf_is_one_cb(196)    => __E_bf_cb_82(197).
	p(NT(196), (NT(197)));
//G274: bf_cb(53)            => bf_is_one_cb(196).
	p(NT(53), (NT(196)));
//G275: __E_bf_cb_83(199)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(177).
	p(NT(199), (T(42)+T(43)+T(44)+T(48)+T(17)+T(10)+T(44)+T(10)+T(50)+T(42)+T(43)+T(11)+T(51)+T(12)+T(50)+T(18)+T(17)+T(44)+T(46)+T(42)+NT(177)));
//G276: bf_has_subformula_cb(198) => __E_bf_cb_83(199).
	p(NT(198), (NT(199)));
//G277: bf_cb(53)            => bf_has_subformula_cb(198).
	p(NT(53), (NT(198)));
//G278: __E_bf_cb_84(201)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(178).
	p(NT(201), (T(42)+T(43)+T(44)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(44)+T(43)+T(50)+T(47)+T(15)+T(52)+T(13)+T(51)+T(10)+T(17)+T(18)+T(44)+T(46)+T(42)+NT(178)));
//G279: bf_remove_funiversal_cb(200) => __E_bf_cb_84(201).
	p(NT(200), (NT(201)));
//G280: bf_cb(53)            => bf_remove_funiversal_cb(200).
	p(NT(53), (NT(200)));
//G281: __E_bf_cb_85(203)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(178).
	p(NT(203), (T(42)+T(43)+T(44)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(44)+T(43)+T(13)+T(24)+T(15)+T(10)+T(14)+T(13)+T(47)+T(14)+T(15)+T(17)+T(18)+T(44)+T(46)+T(42)+NT(178)));
//G282: bf_remove_fexistential_cb(202) => __E_bf_cb_85(203).
	p(NT(202), (NT(203)));
//G283: bf_cb(53)            => bf_remove_fexistential_cb(202).
	p(NT(53), (NT(202)));
//G284: __E_bf_cb_86(205)    => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b' bf_cb_args2(176).
	p(NT(205), (T(42)+T(43)+T(44)+T(17)+T(47)+T(54)+T(44)+T(46)+T(42)+NT(176)));
//G285: bf_and_cb(204)       => __E_bf_cb_86(205).
	p(NT(204), (NT(205)));
//G286: bf_cb(53)            => bf_and_cb(204).
	p(NT(53), (NT(204)));
//G287: __E_bf_cb_87(207)    => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b' bf_cb_args2(176).
	p(NT(207), (T(42)+T(43)+T(44)+T(11)+T(51)+T(44)+T(46)+T(42)+NT(176)));
//G288: bf_or_cb(206)        => __E_bf_cb_87(207).
	p(NT(206), (NT(207)));
//G289: bf_cb(53)            => bf_or_cb(206).
	p(NT(53), (NT(206)));
//G290: __E_bf_cb_88(209)    => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b' bf_cb_args2(176).
	p(NT(209), (T(42)+T(43)+T(44)+T(24)+T(11)+T(51)+T(44)+T(46)+T(42)+NT(176)));
//G291: bf_xor_cb(208)       => __E_bf_cb_88(209).
	p(NT(208), (NT(209)));
//G292: bf_cb(53)            => bf_xor_cb(208).
	p(NT(53), (NT(208)));
//G293: __E_bf_cb_89(211)    => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b' bf_cb_args1(174).
	p(NT(211), (T(42)+T(43)+T(44)+T(47)+T(13)+T(49)+T(44)+T(46)+T(42)+NT(174)));
//G294: bf_neg_cb(210)       => __E_bf_cb_89(211).
	p(NT(210), (NT(211)));
//G295: bf_cb(53)            => bf_neg_cb(210).
	p(NT(53), (NT(210)));
//G296: __E_bf_cb_90(213)    => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(174).
	p(NT(213), (T(42)+T(43)+T(44)+T(47)+T(11)+T(51)+T(12)+T(17)+T(18)+T(15)+T(53)+T(13)+T(44)+T(46)+T(42)+NT(174)));
//G297: bf_normalize_cb(212) => __E_bf_cb_90(213).
	p(NT(212), (NT(213)));
//G298: bf_cb(53)            => bf_normalize_cb(212).
	p(NT(53), (NT(212)));
//G299: cli(214)             => _(11).
	p(NT(214), (NT(11)));
//G300: __E_cli_91(216)      => '.' _(11) cli_command(215) _(11).
	p(NT(216), (T(1)+NT(11)+NT(215)+NT(11)));
//G301: __E_cli_92(217)      => null.
	p(NT(217), (nul));
//G302: __E_cli_92(217)      => __E_cli_91(216) __E_cli_92(217).
	p(NT(217), (NT(216)+NT(217)));
//G303: cli(214)             => _(11) cli_command(215) _(11) __E_cli_92(217).
	p(NT(214), (NT(11)+NT(215)+NT(11)+NT(217)));
//G304: __E_cli_command_93(220) => 'q'.
	p(NT(220), (T(45)));
//G305: __E_cli_command_93(220) => 'q' 'u' 'i' 't'.
	p(NT(220), (T(45)+T(50)+T(15)+T(14)));
//G306: quit_sym(219)        => __E_cli_command_93(220).
	p(NT(219), (NT(220)));
//G307: quit_cmd(218)        => quit_sym(219).
	p(NT(218), (NT(219)));
//G308: cli_command(215)     => quit_cmd(218).
	p(NT(215), (NT(218)));
//G309: __E_cli_command_94(223) => 'v'.
	p(NT(223), (T(52)));
//G310: __E_cli_command_94(223) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(223), (T(52)+T(13)+T(51)+T(10)+T(15)+T(11)+T(47)));
//G311: version_sym(222)     => __E_cli_command_94(223).
	p(NT(222), (NT(223)));
//G312: version_cmd(221)     => version_sym(222).
	p(NT(221), (NT(222)));
//G313: cli_command(215)     => version_cmd(221).
	p(NT(215), (NT(221)));
//G314: __E_cli_command_95(226) => 'c'.
	p(NT(226), (T(46)));
//G315: __E_cli_command_95(226) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(226), (T(46)+T(18)+T(13)+T(17)+T(51)));
//G316: clear_sym(225)       => __E_cli_command_95(226).
	p(NT(225), (NT(226)));
//G317: clear_cmd(224)       => clear_sym(225).
	p(NT(224), (NT(225)));
//G318: cli_command(215)     => clear_cmd(224).
	p(NT(215), (NT(224)));
//G319: __E___E_cli_command_96_97(230) => 'h'.
	p(NT(230), (T(48)));
//G320: __E___E_cli_command_96_97(230) => 'h' 'e' 'l' 'p'.
	p(NT(230), (T(48)+T(13)+T(18)+T(55)));
//G321: help_sym(229)        => __E___E_cli_command_96_97(230).
	p(NT(229), (NT(230)));
//G322: __E___E_cli_command_96_98(231) => __(58) help_arg(232).
	p(NT(231), (NT(58)+NT(232)));
//G323: __E___E_cli_command_96_98(231) => null.
	p(NT(231), (nul));
//G324: __E_cli_command_96(228) => help_sym(229) __E___E_cli_command_96_98(231).
	p(NT(228), (NT(229)+NT(231)));
//G325: help_cmd(227)        => __E_cli_command_96(228).
	p(NT(227), (NT(228)));
//G326: cli_command(215)     => help_cmd(227).
	p(NT(215), (NT(227)));
//G327: file_sym(235)        => 'f' 'i' 'l' 'e'.
	p(NT(235), (T(43)+T(15)+T(18)+T(13)));
//G328: __E_cli_command_99(234) => file_sym(235) __(58) q_string(236).
	p(NT(234), (NT(235)+NT(58)+NT(236)));
//G329: file_cmd(233)        => __E_cli_command_99(234).
	p(NT(233), (NT(234)));
//G330: cli_command(215)     => file_cmd(233).
	p(NT(215), (NT(233)));
//G331: valid_sym(239)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(239), (T(52)+T(17)+T(18)+T(15)+T(54)));
//G332: __E_cli_command_100(238) => valid_sym(239) __(58) rr(10).
	p(NT(238), (NT(239)+NT(58)+NT(10)));
//G333: valid_cmd(237)       => __E_cli_command_100(238).
	p(NT(237), (NT(238)));
//G334: cli_command(215)     => valid_cmd(237).
	p(NT(215), (NT(237)));
//G335: sat_sym(242)         => 's' 'a' 't'.
	p(NT(242), (T(10)+T(17)+T(14)));
//G336: __E_cli_command_101(241) => sat_sym(242) __(58) rr(10).
	p(NT(241), (NT(242)+NT(58)+NT(10)));
//G337: sat_cmd(240)         => __E_cli_command_101(241).
	p(NT(240), (NT(241)));
//G338: cli_command(215)     => sat_cmd(240).
	p(NT(215), (NT(240)));
//G339: unsat_sym(245)       => 'u' 'n' 's' 'a' 't'.
	p(NT(245), (T(50)+T(47)+T(10)+T(17)+T(14)));
//G340: __E_cli_command_102(244) => unsat_sym(245) __(58) rr(10).
	p(NT(244), (NT(245)+NT(58)+NT(10)));
//G341: unsat_cmd(243)       => __E_cli_command_102(244).
	p(NT(243), (NT(244)));
//G342: cli_command(215)     => unsat_cmd(243).
	p(NT(215), (NT(243)));
//G343: solve_sym(248)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(248), (T(10)+T(11)+T(18)+T(52)+T(13)));
//G344: __E___E_cli_command_103_104(250) => ',' _(11) solve_cmd_arg(249).
	p(NT(250), (T(4)+NT(11)+NT(249)));
//G345: __E___E_cli_command_103_105(251) => __E___E_cli_command_103_104(250).
	p(NT(251), (NT(250)));
//G346: __E___E_cli_command_103_105(251) => __E___E_cli_command_103_104(250) __E___E_cli_command_103_105(251).
	p(NT(251), (NT(250)+NT(251)));
//G347: __E_cli_command_103(247) => solve_sym(248) __(58) solve_cmd_arg(249) __E___E_cli_command_103_105(251).
	p(NT(247), (NT(248)+NT(58)+NT(249)+NT(251)));
//G348: solve_cmd(246)       => __E_cli_command_103(247).
	p(NT(246), (NT(247)));
//G349: cli_command(215)     => solve_cmd(246).
	p(NT(215), (NT(246)));
//G350: __E___E_cli_command_106_107(255) => 'e'.
	p(NT(255), (T(13)));
//G351: __E___E_cli_command_106_107(255) => 'e' 'x' 'e' 'c' 'u' 't' 'e'.
	p(NT(255), (T(13)+T(24)+T(13)+T(46)+T(50)+T(14)+T(13)));
//G352: execute_sym(254)     => __E___E_cli_command_106_107(255).
	p(NT(254), (NT(255)));
//G353: __E___E_cli_command_106_108(256) => wff_typed(257).
	p(NT(256), (NT(257)));
//G354: __E___E_cli_command_106_108(256) => rr(10).
	p(NT(256), (NT(10)));
//G355: __E_cli_command_106(253) => execute_sym(254) __(58) __E___E_cli_command_106_108(256).
	p(NT(253), (NT(254)+NT(58)+NT(256)));
//G356: execute_cmd(252)     => __E_cli_command_106(253).
	p(NT(252), (NT(253)));
//G357: cli_command(215)     => execute_cmd(252).
	p(NT(215), (NT(252)));
//G358: __E___E_cli_command_109_110(261) => 'n'.
	p(NT(261), (T(47)));
//G359: __E___E_cli_command_109_110(261) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(261), (T(47)+T(11)+T(51)+T(12)+T(17)+T(18)+T(15)+T(53)+T(13)));
//G360: normalize_sym(260)   => __E___E_cli_command_109_110(261).
	p(NT(260), (NT(261)));
//G361: __E_cli_command_109(259) => normalize_sym(260) __(58) normalize_cmd_arg(262).
	p(NT(259), (NT(260)+NT(58)+NT(262)));
//G362: normalize_cmd(258)   => __E_cli_command_109(259).
	p(NT(258), (NT(259)));
//G363: cli_command(215)     => normalize_cmd(258).
	p(NT(215), (NT(258)));
//G364: __E___E_cli_command_111_112(266) => 's'.
	p(NT(266), (T(10)));
//G365: __E___E_cli_command_111_112(266) => 's' 'u' 'b' 's' 't'.
	p(NT(266), (T(10)+T(50)+T(42)+T(10)+T(14)));
//G366: __E___E_cli_command_111_112(266) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(266), (T(10)+T(50)+T(42)+T(10)+T(14)+T(15)+T(14)+T(50)+T(14)+T(13)));
//G367: subst_sym(265)       => __E___E_cli_command_111_112(266).
	p(NT(265), (NT(266)));
//G368: __E_cli_command_111(264) => subst_sym(265) __(58) nf_cmd_arg(267) _(11) '[' _(11) nf_cmd_arg(267) _(11) '/' _(11) nf_cmd_arg(267) _(11) ']'.
	p(NT(264), (NT(265)+NT(58)+NT(267)+NT(11)+T(21)+NT(11)+NT(267)+NT(11)+T(56)+NT(11)+NT(267)+NT(11)+T(22)));
//G369: subst_cmd(263)       => __E_cli_command_111(264).
	p(NT(263), (NT(264)));
//G370: cli_command(215)     => subst_cmd(263).
	p(NT(215), (NT(263)));
//G371: __E___E_cli_command_113_114(271) => 'i'.
	p(NT(271), (T(15)));
//G372: __E___E_cli_command_113_114(271) => 'i' 'n' 's' 't'.
	p(NT(271), (T(15)+T(47)+T(10)+T(14)));
//G373: __E___E_cli_command_113_114(271) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(271), (T(15)+T(47)+T(10)+T(14)+T(17)+T(47)+T(14)+T(15)+T(17)+T(14)+T(13)));
//G374: inst_sym(270)        => __E___E_cli_command_113_114(271).
	p(NT(270), (NT(271)));
//G375: __E_cli_command_113(269) => inst_sym(270) __(58) inst_args(272).
	p(NT(269), (NT(270)+NT(58)+NT(272)));
//G376: inst_cmd(268)        => __E_cli_command_113(269).
	p(NT(268), (NT(269)));
//G377: cli_command(215)     => inst_cmd(268).
	p(NT(215), (NT(268)));
//G378: dnf_sym(275)         => 'd' 'n' 'f'.
	p(NT(275), (T(54)+T(47)+T(43)));
//G379: __E_cli_command_115(274) => dnf_sym(275) __(58) nf_cmd_arg(267).
	p(NT(274), (NT(275)+NT(58)+NT(267)));
//G380: dnf_cmd(273)         => __E_cli_command_115(274).
	p(NT(273), (NT(274)));
//G381: cli_command(215)     => dnf_cmd(273).
	p(NT(215), (NT(273)));
//G382: cnf_sym(278)         => 'c' 'n' 'f'.
	p(NT(278), (T(46)+T(47)+T(43)));
//G383: __E_cli_command_116(277) => cnf_sym(278) __(58) nf_cmd_arg(267).
	p(NT(277), (NT(278)+NT(58)+NT(267)));
//G384: cnf_cmd(276)         => __E_cli_command_116(277).
	p(NT(276), (NT(277)));
//G385: cli_command(215)     => cnf_cmd(276).
	p(NT(215), (NT(276)));
//G386: anf_sym(281)         => 'a' 'n' 'f'.
	p(NT(281), (T(17)+T(47)+T(43)));
//G387: __E_cli_command_117(280) => anf_sym(281) __(58) nf_cmd_arg(267).
	p(NT(280), (NT(281)+NT(58)+NT(267)));
//G388: anf_cmd(279)         => __E_cli_command_117(280).
	p(NT(279), (NT(280)));
//G389: cli_command(215)     => anf_cmd(279).
	p(NT(215), (NT(279)));
//G390: nnf_sym(284)         => 'n' 'n' 'f'.
	p(NT(284), (T(47)+T(47)+T(43)));
//G391: __E_cli_command_118(283) => nnf_sym(284) __(58) nf_cmd_arg(267).
	p(NT(283), (NT(284)+NT(58)+NT(267)));
//G392: nnf_cmd(282)         => __E_cli_command_118(283).
	p(NT(282), (NT(283)));
//G393: cli_command(215)     => nnf_cmd(282).
	p(NT(215), (NT(282)));
//G394: pnf_sym(287)         => 'p' 'n' 'f'.
	p(NT(287), (T(55)+T(47)+T(43)));
//G395: __E_cli_command_119(286) => pnf_sym(287) __(58) nf_cmd_arg(267).
	p(NT(286), (NT(287)+NT(58)+NT(267)));
//G396: pnf_cmd(285)         => __E_cli_command_119(286).
	p(NT(285), (NT(286)));
//G397: cli_command(215)     => pnf_cmd(285).
	p(NT(215), (NT(285)));
//G398: mnf_sym(290)         => 'm' 'n' 'f'.
	p(NT(290), (T(12)+T(47)+T(43)));
//G399: __E_cli_command_120(289) => mnf_sym(290) __(58) nf_cmd_arg(267).
	p(NT(289), (NT(290)+NT(58)+NT(267)));
//G400: mnf_cmd(288)         => __E_cli_command_120(289).
	p(NT(288), (NT(289)));
//G401: cli_command(215)     => mnf_cmd(288).
	p(NT(215), (NT(288)));
//G402: snf_sym(293)         => 's' 'n' 'f'.
	p(NT(293), (T(10)+T(47)+T(43)));
//G403: __E_cli_command_121(292) => snf_sym(293) __(58) nf_cmd_arg(267).
	p(NT(292), (NT(293)+NT(58)+NT(267)));
//G404: snf_cmd(291)         => __E_cli_command_121(292).
	p(NT(291), (NT(292)));
//G405: cli_command(215)     => snf_cmd(291).
	p(NT(215), (NT(291)));
//G406: onf_sym(296)         => 'o' 'n' 'f'.
	p(NT(296), (T(11)+T(47)+T(43)));
//G407: __E_cli_command_122(295) => onf_sym(296) __(58) variable(111) __(58) onf_cmd_arg(297).
	p(NT(295), (NT(296)+NT(58)+NT(111)+NT(58)+NT(297)));
//G408: onf_cmd(294)         => __E_cli_command_122(295).
	p(NT(294), (NT(295)));
//G409: cli_command(215)     => onf_cmd(294).
	p(NT(215), (NT(294)));
//G410: __E___E_cli_command_123_124(301) => 'd' 'e' 'f' 's'.
	p(NT(301), (T(54)+T(13)+T(43)+T(10)));
//G411: __E___E_cli_command_123_124(301) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(301), (T(54)+T(13)+T(43)+T(15)+T(47)+T(15)+T(14)+T(15)+T(11)+T(47)+T(10)));
//G412: def_sym(300)         => __E___E_cli_command_123_124(301).
	p(NT(300), (NT(301)));
//G413: __E_cli_command_123(299) => def_sym(300).
	p(NT(299), (NT(300)));
//G414: def_list_cmd(298)    => __E_cli_command_123(299).
	p(NT(298), (NT(299)));
//G415: cli_command(215)     => def_list_cmd(298).
	p(NT(215), (NT(298)));
//G416: __E_cli_command_125(303) => def_sym(300) __(58) number(304).
	p(NT(303), (NT(300)+NT(58)+NT(304)));
//G417: def_print_cmd(302)   => __E_cli_command_125(303).
	p(NT(302), (NT(303)));
//G418: cli_command(215)     => def_print_cmd(302).
	p(NT(215), (NT(302)));
//G419: def_rr_cmd(305)      => rec_relation(16).
	p(NT(305), (NT(16)));
//G420: cli_command(215)     => def_rr_cmd(305).
	p(NT(215), (NT(305)));
//G421: qelim_sym(308)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(308), (T(45)+T(13)+T(18)+T(15)+T(12)));
//G422: __E_cli_command_126(307) => qelim_sym(308) __(58) wff_cmd_arg(309).
	p(NT(307), (NT(308)+NT(58)+NT(309)));
//G423: qelim_cmd(306)       => __E_cli_command_126(307).
	p(NT(306), (NT(307)));
//G424: cli_command(215)     => qelim_cmd(306).
	p(NT(215), (NT(306)));
//G425: get_sym(312)         => 'g' 'e' 't'.
	p(NT(312), (T(49)+T(13)+T(14)));
//G426: __E___E_cli_command_127_128(313) => __(58) option(314).
	p(NT(313), (NT(58)+NT(314)));
//G427: __E___E_cli_command_127_128(313) => null.
	p(NT(313), (nul));
//G428: __E_cli_command_127(311) => get_sym(312) __E___E_cli_command_127_128(313).
	p(NT(311), (NT(312)+NT(313)));
//G429: get_cmd(310)         => __E_cli_command_127(311).
	p(NT(310), (NT(311)));
//G430: cli_command(215)     => get_cmd(310).
	p(NT(215), (NT(310)));
//G431: set_sym(317)         => 's' 'e' 't'.
	p(NT(317), (T(10)+T(13)+T(14)));
//G432: __E___E_cli_command_129_130(318) => __(58).
	p(NT(318), (NT(58)));
//G433: __E___E_cli_command_129_130(318) => _(11) '=' _(11).
	p(NT(318), (NT(11)+T(3)+NT(11)));
//G434: __E_cli_command_129(316) => set_sym(317) __(58) option(314) __E___E_cli_command_129_130(318) option_value(319).
	p(NT(316), (NT(317)+NT(58)+NT(314)+NT(318)+NT(319)));
//G435: set_cmd(315)         => __E_cli_command_129(316).
	p(NT(315), (NT(316)));
//G436: cli_command(215)     => set_cmd(315).
	p(NT(215), (NT(315)));
//G437: toggle_sym(322)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(322), (T(14)+T(11)+T(49)+T(49)+T(18)+T(13)));
//G438: __E_cli_command_131(321) => toggle_sym(322) __(58) bool_option(323).
	p(NT(321), (NT(322)+NT(58)+NT(323)));
//G439: toggle_cmd(320)      => __E_cli_command_131(321).
	p(NT(320), (NT(321)));
//G440: cli_command(215)     => toggle_cmd(320).
	p(NT(215), (NT(320)));
//G441: __E___E_cli_command_132_133(327) => 'h' 'i' 's' 't'.
	p(NT(327), (T(48)+T(15)+T(10)+T(14)));
//G442: __E___E_cli_command_132_133(327) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(327), (T(48)+T(15)+T(10)+T(14)+T(11)+T(51)+T(20)));
//G443: history_sym(326)     => __E___E_cli_command_132_133(327).
	p(NT(326), (NT(327)));
//G444: __E_cli_command_132(325) => history_sym(326).
	p(NT(325), (NT(326)));
//G445: history_list_cmd(324) => __E_cli_command_132(325).
	p(NT(324), (NT(325)));
//G446: cli_command(215)     => history_list_cmd(324).
	p(NT(215), (NT(324)));
//G447: __E_cli_command_134(329) => history_sym(326) __(58) memory(330).
	p(NT(329), (NT(326)+NT(58)+NT(330)));
//G448: history_print_cmd(328) => __E_cli_command_134(329).
	p(NT(328), (NT(329)));
//G449: cli_command(215)     => history_print_cmd(328).
	p(NT(215), (NT(328)));
//G450: __E_cli_command_135(332) => bf_typed(333).
	p(NT(332), (NT(333)));
//G451: __E_cli_command_135(332) => wff_typed(257).
	p(NT(332), (NT(257)));
//G452: history_store_cmd(331) => __E_cli_command_135(332).
	p(NT(331), (NT(332)));
//G453: cli_command(215)     => history_store_cmd(331).
	p(NT(215), (NT(331)));
//G454: number(304)          => digits(137).
	p(NT(304), (NT(137)));
//G455: bf_typed(333)        => bf(21).
	p(NT(333), (NT(21)));
//G456: bf_typed(333)        => bf(21) _(11) ':' _(11) 'b' 'f'.
	p(NT(333), (NT(21)+NT(11)+T(2)+NT(11)+T(42)+T(43)));
//G457: wff_typed(257)       => wff(14).
	p(NT(257), (NT(14)));
//G458: wff_typed(257)       => wff(14) _(11) ':' _(11) 'w' 'f' 'f'.
	p(NT(257), (NT(14)+NT(11)+T(2)+NT(11)+T(19)+T(43)+T(43)));
//G459: bf_cmd_arg(334)      => memory(330).
	p(NT(334), (NT(330)));
//G460: bf_cmd_arg(334)      => bf_typed(333).
	p(NT(334), (NT(333)));
//G461: wff_cmd_arg(309)     => memory(330).
	p(NT(309), (NT(330)));
//G462: wff_cmd_arg(309)     => wff_typed(257).
	p(NT(309), (NT(257)));
//G463: nf_cmd_arg(267)      => memory(330).
	p(NT(267), (NT(330)));
//G464: nf_cmd_arg(267)      => bf_typed(333).
	p(NT(267), (NT(333)));
//G465: nf_cmd_arg(267)      => wff_typed(257).
	p(NT(267), (NT(257)));
//G466: onf_cmd_arg(297)     => memory(330).
	p(NT(297), (NT(330)));
//G467: onf_cmd_arg(297)     => wff_typed(257).
	p(NT(297), (NT(257)));
//G468: normalize_cmd_arg(262) => memory(330).
	p(NT(262), (NT(330)));
//G469: normalize_cmd_arg(262) => rr(10).
	p(NT(262), (NT(10)));
//G470: normalize_cmd_arg(262) => ref(18).
	p(NT(262), (NT(18)));
//G471: normalize_cmd_arg(262) => bf_typed(333).
	p(NT(262), (NT(333)));
//G472: normalize_cmd_arg(262) => wff_typed(257).
	p(NT(262), (NT(257)));
//G473: solve_cmd_arg(249)   => memory(330).
	p(NT(249), (NT(330)));
//G474: solve_cmd_arg(249)   => bf_eq(98).
	p(NT(249), (NT(98)));
//G475: solve_cmd_arg(249)   => bf_neq(96).
	p(NT(249), (NT(96)));
//G476: inst_args(272)       => bf_cmd_arg(334) _(11) '[' _(11) variable(111) _(11) '/' _(11) bf_cmd_arg(334) _(11) ']'.
	p(NT(272), (NT(334)+NT(11)+T(21)+NT(11)+NT(111)+NT(11)+T(56)+NT(11)+NT(334)+NT(11)+T(22)));
//G477: inst_args(272)       => wff_cmd_arg(309) _(11) '[' _(11) variable(111) _(11) '/' _(11) bf_cmd_arg(334) _(11) ']'.
	p(NT(272), (NT(309)+NT(11)+T(21)+NT(11)+NT(111)+NT(11)+T(56)+NT(11)+NT(334)+NT(11)+T(22)));
//G478: help_arg(232)        => help_sym(229).
	p(NT(232), (NT(229)));
//G479: help_arg(232)        => version_sym(222).
	p(NT(232), (NT(222)));
//G480: help_arg(232)        => quit_sym(219).
	p(NT(232), (NT(219)));
//G481: help_arg(232)        => clear_sym(225).
	p(NT(232), (NT(225)));
//G482: help_arg(232)        => get_sym(312).
	p(NT(232), (NT(312)));
//G483: help_arg(232)        => set_sym(317).
	p(NT(232), (NT(317)));
//G484: help_arg(232)        => toggle_sym(322).
	p(NT(232), (NT(322)));
//G485: help_arg(232)        => file_sym(235).
	p(NT(232), (NT(235)));
//G486: help_arg(232)        => history_sym(326).
	p(NT(232), (NT(326)));
//G487: help_arg(232)        => abs_memory_sym(335).
	p(NT(232), (NT(335)));
//G488: help_arg(232)        => rel_memory_sym(336).
	p(NT(232), (NT(336)));
//G489: help_arg(232)        => selection_sym(337).
	p(NT(232), (NT(337)));
//G490: help_arg(232)        => def_sym(300).
	p(NT(232), (NT(300)));
//G491: help_arg(232)        => inst_sym(270).
	p(NT(232), (NT(270)));
//G492: help_arg(232)        => subst_sym(265).
	p(NT(232), (NT(265)));
//G493: help_arg(232)        => normalize_sym(260).
	p(NT(232), (NT(260)));
//G494: help_arg(232)        => execute_sym(254).
	p(NT(232), (NT(254)));
//G495: help_arg(232)        => solve_sym(248).
	p(NT(232), (NT(248)));
//G496: help_arg(232)        => valid_sym(239).
	p(NT(232), (NT(239)));
//G497: help_arg(232)        => sat_sym(242).
	p(NT(232), (NT(242)));
//G498: help_arg(232)        => unsat_sym(245).
	p(NT(232), (NT(245)));
//G499: help_arg(232)        => dnf_sym(275).
	p(NT(232), (NT(275)));
//G500: help_arg(232)        => cnf_sym(278).
	p(NT(232), (NT(278)));
//G501: help_arg(232)        => anf_sym(281).
	p(NT(232), (NT(281)));
//G502: help_arg(232)        => snf_sym(293).
	p(NT(232), (NT(293)));
//G503: help_arg(232)        => nnf_sym(284).
	p(NT(232), (NT(284)));
//G504: help_arg(232)        => pnf_sym(287).
	p(NT(232), (NT(287)));
//G505: help_arg(232)        => mnf_sym(290).
	p(NT(232), (NT(290)));
//G506: help_arg(232)        => onf_sym(296).
	p(NT(232), (NT(296)));
//G507: help_arg(232)        => qelim_sym(308).
	p(NT(232), (NT(308)));
//G508: __E___E_help_arg_136_137(340) => 's'.
	p(NT(340), (T(10)));
//G509: __E___E_help_arg_136_137(340) => null.
	p(NT(340), (nul));
//G510: __E_help_arg_136(339) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_136_137(340).
	p(NT(339), (T(13)+T(24)+T(17)+T(12)+T(55)+T(18)+T(13)+NT(340)));
//G511: examples_sym(338)    => __E_help_arg_136(339).
	p(NT(338), (NT(339)));
//G512: help_arg(232)        => examples_sym(338).
	p(NT(232), (NT(338)));
//G513: __E_memory_138(342)  => '%'.
	p(NT(342), (T(57)));
//G514: rel_memory_sym(336)  => '%' '-'.
	p(NT(336), (T(57)+T(25)));
//G515: memory_id(344)       => digits(137).
	p(NT(344), (NT(137)));
//G516: __E___E_memory_138_139(343) => memory_id(344).
	p(NT(343), (NT(344)));
//G517: __E___E_memory_138_139(343) => null.
	p(NT(343), (nul));
//G518: __E_memory_138(342)  => rel_memory_sym(336) __E___E_memory_138_139(343).
	p(NT(342), (NT(336)+NT(343)));
//G519: rel_memory(341)      => __E_memory_138(342).
	p(NT(341), (NT(342)));
//G520: rel_memory(341)      => rel_memory(341).
	p(NT(341), (NT(341)));
//G521: memory(330)          => rel_memory(341).
	p(NT(330), (NT(341)));
//G522: abs_memory_sym(335)  => '%'.
	p(NT(335), (T(57)));
//G523: __E_memory_140(346)  => abs_memory_sym(335) memory_id(344).
	p(NT(346), (NT(335)+NT(344)));
//G524: abs_memory(345)      => __E_memory_140(346).
	p(NT(345), (NT(346)));
//G525: memory(330)          => abs_memory(345).
	p(NT(330), (NT(345)));
//G526: option(314)          => bool_option(323).
	p(NT(314), (NT(323)));
//G527: __E_option_141(349)  => 's' 'e' 'v'.
	p(NT(349), (T(10)+T(13)+T(52)));
//G528: __E_option_141(349)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(349), (T(10)+T(13)+T(52)+T(13)+T(51)+T(15)+T(14)+T(20)));
//G529: severity_opt(348)    => __E_option_141(349).
	p(NT(348), (NT(349)));
//G530: enum_option(347)     => severity_opt(348).
	p(NT(347), (NT(348)));
//G531: option(314)          => enum_option(347).
	p(NT(314), (NT(347)));
//G532: __E_bool_option_142(351) => 's'.
	p(NT(351), (T(10)));
//G533: __E_bool_option_142(351) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(351), (T(10)+T(14)+T(17)+T(14)+T(50)+T(10)));
//G534: status_opt(350)      => __E_bool_option_142(351).
	p(NT(350), (NT(351)));
//G535: bool_option(323)     => status_opt(350).
	p(NT(323), (NT(350)));
//G536: __E_bool_option_143(353) => 'c'.
	p(NT(353), (T(46)));
//G537: __E_bool_option_143(353) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(353), (T(46)+T(11)+T(18)+T(11)+T(51)));
//G538: __E_bool_option_143(353) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(353), (T(46)+T(11)+T(18)+T(11)+T(51)+T(10)));
//G539: colors_opt(352)      => __E_bool_option_143(353).
	p(NT(352), (NT(353)));
//G540: bool_option(323)     => colors_opt(352).
	p(NT(323), (NT(352)));
//G541: __E_bool_option_144(355) => 'd'.
	p(NT(355), (T(54)));
//G542: __E_bool_option_144(355) => 'd' 'b' 'g'.
	p(NT(355), (T(54)+T(42)+T(49)));
//G543: __E_bool_option_144(355) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(355), (T(54)+T(13)+T(42)+T(50)+T(49)));
//G544: debug_repl_opt(354)  => __E_bool_option_144(355).
	p(NT(354), (NT(355)));
//G545: bool_option(323)     => debug_repl_opt(354).
	p(NT(323), (NT(354)));
//G546: option_value(319)    => option_value_true(356).
	p(NT(319), (NT(356)));
//G547: option_value(319)    => option_value_false(357).
	p(NT(319), (NT(357)));
//G548: option_value(319)    => severity(358).
	p(NT(319), (NT(358)));
//G549: option_value_true(356) => 't'.
	p(NT(356), (T(14)));
//G550: option_value_true(356) => 't' 'r' 'u' 'e'.
	p(NT(356), (T(14)+T(51)+T(50)+T(13)));
//G551: option_value_true(356) => 'o' 'n'.
	p(NT(356), (T(11)+T(47)));
//G552: option_value_true(356) => '1'.
	p(NT(356), (T(35)));
//G553: option_value_true(356) => 'y'.
	p(NT(356), (T(20)));
//G554: option_value_true(356) => 'y' 'e' 's'.
	p(NT(356), (T(20)+T(13)+T(10)));
//G555: option_value_false(357) => 'f'.
	p(NT(357), (T(43)));
//G556: option_value_false(357) => 'f' 'a' 'l' 's' 'e'.
	p(NT(357), (T(43)+T(17)+T(18)+T(10)+T(13)));
//G557: option_value_false(357) => 'o' 'f' 'f'.
	p(NT(357), (T(11)+T(43)+T(43)));
//G558: option_value_false(357) => '0'.
	p(NT(357), (T(36)));
//G559: option_value_false(357) => 'n'.
	p(NT(357), (T(47)));
//G560: option_value_false(357) => 'n' 'o'.
	p(NT(357), (T(47)+T(11)));
//G561: __E_severity_145(360) => 'e'.
	p(NT(360), (T(13)));
//G562: __E_severity_145(360) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(360), (T(13)+T(51)+T(51)+T(11)+T(51)));
//G563: error_sym(359)       => __E_severity_145(360).
	p(NT(359), (NT(360)));
//G564: severity(358)        => error_sym(359).
	p(NT(358), (NT(359)));
//G565: __E_severity_146(362) => 'i'.
	p(NT(362), (T(15)));
//G566: __E_severity_146(362) => 'i' 'n' 'f' 'o'.
	p(NT(362), (T(15)+T(47)+T(43)+T(11)));
//G567: info_sym(361)        => __E_severity_146(362).
	p(NT(361), (NT(362)));
//G568: severity(358)        => info_sym(361).
	p(NT(358), (NT(361)));
//G569: __E_severity_147(364) => 'd'.
	p(NT(364), (T(54)));
//G570: __E_severity_147(364) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(364), (T(54)+T(13)+T(42)+T(50)+T(49)));
//G571: debug_sym(363)       => __E_severity_147(364).
	p(NT(363), (NT(364)));
//G572: severity(358)        => debug_sym(363).
	p(NT(358), (NT(363)));
//G573: __E_severity_148(366) => 't'.
	p(NT(366), (T(14)));
//G574: __E_severity_148(366) => 't' 'r' 'a' 'c' 'e'.
	p(NT(366), (T(14)+T(51)+T(17)+T(46)+T(13)));
//G575: trace_sym(365)       => __E_severity_148(366).
	p(NT(365), (NT(366)));
//G576: severity(358)        => trace_sym(365).
	p(NT(358), (NT(365)));
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
		wff_cb_arg, wff_cb_args2, wff_cb_args3, wff_cb_args4, bf_eq_cb, __E_wff_cb_76, bf_neq_cb, __E_wff_cb_77, wff_has_clashing_subformulas_cb, __E_wff_cb_78, 
		wff_has_subformula_cb, __E_wff_cb_79, wff_remove_existential_cb, __E_wff_cb_80, bf_is_zero_cb, __E_bf_cb_81, bf_is_one_cb, __E_bf_cb_82, bf_has_subformula_cb, __E_bf_cb_83, 
		bf_remove_funiversal_cb, __E_bf_cb_84, bf_remove_fexistential_cb, __E_bf_cb_85, bf_and_cb, __E_bf_cb_86, bf_or_cb, __E_bf_cb_87, bf_xor_cb, __E_bf_cb_88, 
		bf_neg_cb, __E_bf_cb_89, bf_normalize_cb, __E_bf_cb_90, cli, cli_command, __E_cli_91, __E_cli_92, quit_cmd, quit_sym, 
		__E_cli_command_93, version_cmd, version_sym, __E_cli_command_94, clear_cmd, clear_sym, __E_cli_command_95, help_cmd, __E_cli_command_96, help_sym, 
		__E___E_cli_command_96_97, __E___E_cli_command_96_98, help_arg, file_cmd, __E_cli_command_99, file_sym, q_string, valid_cmd, __E_cli_command_100, valid_sym, 
		sat_cmd, __E_cli_command_101, sat_sym, unsat_cmd, __E_cli_command_102, unsat_sym, solve_cmd, __E_cli_command_103, solve_sym, solve_cmd_arg, 
		__E___E_cli_command_103_104, __E___E_cli_command_103_105, execute_cmd, __E_cli_command_106, execute_sym, __E___E_cli_command_106_107, __E___E_cli_command_106_108, wff_typed, normalize_cmd, __E_cli_command_109, 
		normalize_sym, __E___E_cli_command_109_110, normalize_cmd_arg, subst_cmd, __E_cli_command_111, subst_sym, __E___E_cli_command_111_112, nf_cmd_arg, inst_cmd, __E_cli_command_113, 
		inst_sym, __E___E_cli_command_113_114, inst_args, dnf_cmd, __E_cli_command_115, dnf_sym, cnf_cmd, __E_cli_command_116, cnf_sym, anf_cmd, 
		__E_cli_command_117, anf_sym, nnf_cmd, __E_cli_command_118, nnf_sym, pnf_cmd, __E_cli_command_119, pnf_sym, mnf_cmd, __E_cli_command_120, 
		mnf_sym, snf_cmd, __E_cli_command_121, snf_sym, onf_cmd, __E_cli_command_122, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_123, 
		def_sym, __E___E_cli_command_123_124, def_print_cmd, __E_cli_command_125, number, def_rr_cmd, qelim_cmd, __E_cli_command_126, qelim_sym, wff_cmd_arg, 
		get_cmd, __E_cli_command_127, get_sym, __E___E_cli_command_127_128, option, set_cmd, __E_cli_command_129, set_sym, __E___E_cli_command_129_130, option_value, 
		toggle_cmd, __E_cli_command_131, toggle_sym, bool_option, history_list_cmd, __E_cli_command_132, history_sym, __E___E_cli_command_132_133, history_print_cmd, __E_cli_command_134, 
		memory, history_store_cmd, __E_cli_command_135, bf_typed, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, examples_sym, __E_help_arg_136, 
		__E___E_help_arg_136_137, rel_memory, __E_memory_138, __E___E_memory_138_139, memory_id, abs_memory, __E_memory_140, enum_option, severity_opt, __E_option_141, 
		status_opt, __E_bool_option_142, colors_opt, __E_bool_option_143, debug_repl_opt, __E_bool_option_144, option_value_true, option_value_false, severity, error_sym, 
		__E_severity_145, info_sym, __E_severity_146, debug_sym, __E_severity_147, trace_sym, __E_severity_148, __N_0, __N_1, __N_2, 
		__N_3, 
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
