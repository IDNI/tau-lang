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
	"__E___E_ref_args_4_7", "library", "rules", "__E_library_8", "__E___E_library_8_9", "rule", "__E___E___E_library_8_9_10", "wff_rule", "bf_rule", "__E___E_library_8_11", 
	"wff_matcher", "wff_body", "__E_wff_rule_12", "wff_cb", "bf_matcher", "bf_body", "__E_bf_rule_13", "bf_cb", "builder", "builder_head", 
	"builder_body", "__E_builder_head_14", "__", "__E_builder_head_15", "bf_builder_body", "__E_builder_body_16", "wff_builder_body", "__E_builder_body_17", "wff_parenthesis", "__E_wff_18", 
	"wff_sometimes", "__E_wff_19", "__E___E_wff_19_20", "wff_always", "__E_wff_21", "__E___E_wff_21_22", "wff_conditional", "__E_wff_23", "wff_all", "__E_wff_24", 
	"q_vars", "wff_ex", "__E_wff_25", "wff_ref", "constraint", "wff_imply", "__E_wff_26", "wff_equiv", "__E_wff_27", "wff_or", 
	"__E_wff_28", "wff_xor", "__E_wff_29", "wff_and", "__E_wff_30", "wff_neg", "__E_wff_31", "wff_t", "wff_f", "bf_interval", 
	"__E_wff_32", "bf_neq", "__E_wff_33", "bf_eq", "__E_wff_34", "bf_nleq", "__E_wff_35", "bf_greater", "__E_wff_36", "bf_less_equal", 
	"__E_wff_37", "bf_less", "__E_wff_38", "bf_parenthesis", "__E_bf_39", "bf_constant", "variable", "bf_splitter", "__E_bf_40", "bf_ref", 
	"bf_or", "__E_bf_41", "bf_xor", "__E_bf_42", "bf_and", "__E_bf_43", "__E___E_bf_43_44", "bf_neg", "__E_bf_45", "bf_t", 
	"bf_f", "uninterpreted_constant", "ctn_neq", "__E_constraint_46", "ctnvar", "num", "ctn_eq", "__E_constraint_47", "ctn_greater_equal", "__E_constraint_48", 
	"ctn_greater", "__E_constraint_49", "ctn_less_equal", "__E_constraint_50", "ctn_less", "__E_constraint_51", "charvar", "__E_charvar_52", "__E_charvar_53", "io_var", 
	"__E_variable_54", "in", "out", "in_var_name", "__E_in_55", "__E___E_in_55_56", "__E___E___E_in_55_56_57", "offset", "out_var_name", "__E_out_58", 
	"__E___E_out_58_59", "__E___E___E_out_58_59_60", "q_var", "__E_q_vars_61", "__E_q_vars_62", "__E_offsets_63", "__E_offsets_64", "shift", "__E_offset_65", "__E_shift_66", 
	"__E___E_shift_66_67", "digits", "uninter_const_name", "__E_uninterpreted_constant_68", "__E___E_uninterpreted_constant_68_69", "chars", "constant", "__E_bf_constant_70", "binding", "__E_bf_constant_71", 
	"type", "source", "__E_binding_72", "src_c", "__E___E_binding_72_73", "__E___E___E_binding_72_73_74", "__E___E___E_binding_72_73_75", "__E_src_c_76", "__E_src_c_77", "__E_chars_78", 
	"__E_digits_79", "extra", "comment", "__E_comment_80", "__E_comment_81", "__E_comment_82", "__E____83", "__E___84", "bf_cb_args1", "bf_cb_arg", 
	"bf_cb_args2", "bf_cb_args3", "bf_cb_args4", "wff_cb_args1", "wff_cb_arg", "wff_cb_args2", "wff_cb_args3", "wff_cb_args4", "wff_has_clashing_subformulas_cb", "__E_wff_cb_85", 
	"wff_has_subformula_cb", "__E_wff_cb_86", "wff_remove_existential_cb", "__E_wff_cb_87", "bf_has_subformula_cb", "__E_bf_cb_88", "bf_remove_funiversal_cb", "__E_bf_cb_89", "bf_remove_fexistential_cb", "__E_bf_cb_90", 
	"bf_normalize_cb", "__E_bf_cb_91", "cli", "cli_command", "__E_cli_92", "__E_cli_93", "quit_cmd", "quit_sym", "__E_cli_command_94", "version_cmd", 
	"version_sym", "__E_cli_command_95", "clear_cmd", "clear_sym", "__E_cli_command_96", "help_cmd", "__E_cli_command_97", "help_sym", "__E___E_cli_command_97_98", "__E___E_cli_command_97_99", 
	"help_arg", "file_cmd", "__E_cli_command_100", "file_sym", "q_string", "valid_cmd", "__E_cli_command_101", "valid_sym", "sat_cmd", "__E_cli_command_102", 
	"sat_sym", "normalize_cmd_arg", "unsat_cmd", "__E_cli_command_103", "unsat_sym", "solve_cmd", "__E_cli_command_104", "solve_sym", "__E___E_cli_command_104_105", "solve_cmd_arg", 
	"run_cmd", "__E_cli_command_106", "run_sym", "__E___E_cli_command_106_107", "normalize_cmd", "__E_cli_command_108", "normalize_sym", "__E___E_cli_command_108_109", "subst_cmd", "__E_cli_command_110", 
	"subst_sym", "__E___E_cli_command_110_111", "nf_cmd_arg", "inst_cmd", "__E_cli_command_112", "inst_sym", "__E___E_cli_command_112_113", "inst_args", "dnf_cmd", "__E_cli_command_114", 
	"dnf_sym", "cnf_cmd", "__E_cli_command_115", "cnf_sym", "anf_cmd", "__E_cli_command_116", "anf_sym", "nnf_cmd", "__E_cli_command_117", "nnf_sym", 
	"pnf_cmd", "__E_cli_command_118", "pnf_sym", "mnf_cmd", "__E_cli_command_119", "mnf_sym", "snf_cmd", "__E_cli_command_120", "snf_sym", "onf_cmd", 
	"__E_cli_command_121", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_122", "def_sym", "__E___E_cli_command_122_123", "def_print_cmd", "__E_cli_command_124", "number", 
	"def_rr_cmd", "qelim_cmd", "__E_cli_command_125", "qelim_sym", "wff_cmd_arg", "get_cmd", "__E_cli_command_126", "get_sym", "__E___E_cli_command_126_127", "option", 
	"set_cmd", "__E_cli_command_128", "set_sym", "__E___E_cli_command_128_129", "option_value", "toggle_cmd", "__E_cli_command_130", "toggle_sym", "bool_option", "def_input_cmd", 
	"__E_cli_command_131", "input_stream", "def_output_cmd", "__E_cli_command_132", "output_stream", "history_list_cmd", "__E_cli_command_133", "history_sym", "__E___E_cli_command_133_134", "history_print_cmd", 
	"__E_cli_command_135", "memory", "history_store_cmd", "__E_cli_command_136", "__E_input_stream_137", "q_file_name", "keyboard_sym", "__E_output_stream_138", "console_sym", "file_name", 
	"__E_file_name_139", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "execute_sym", "examples_sym", "__E_help_arg_140", "__E___E_help_arg_140_141", "rel_memory", 
	"__E_memory_142", "__E___E_memory_142_143", "memory_id", "abs_memory", "__E_memory_144", "enum_option", "severity_opt", "__E_option_145", "status_opt", "__E_bool_option_146", 
	"colors_opt", "__E_bool_option_147", "debug_repl_opt", "__E_bool_option_148", "option_value_true", "option_value_false", "severity", "error_sym", "__E_severity_149", "info_sym", 
	"__E_severity_150", "debug_sym", "__E_severity_151", "trace_sym", "__E_severity_152", "named_binding", "__N_0", "__N_1", "__N_2", "__N_3", 
	"__N_4", "__N_5", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', ',', '(', ')', 's', 'o', 
	'm', 'e', 't', 'i', '<', '>', 'a', 'l', 'w', 'y', 
	'[', ']', '?', 'x', '-', '|', '^', '&', '!', 'T', 
	'F', 'S', '+', '\'', '1', '0', '$', '{', '}', '\t', 
	'\n', '\r', '#', 'f', '_', 'h', 'c', 'n', 'g', 'u', 
	'b', 'r', 'v', 'z', 'q', 'p', 'd', '/', 'k', '"', 
	'%', 
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
			11, 52
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			20, 22, 136, 143, 148, 161, 162, 170, 171, 339,
			375
		},
		.to_inline = {
			{ 14, 58, 14 },
			{ 21, 103, 21 },
			{ 43 },
			{ 47 },
			{ 152 },
			{ 165 },
			{ 173 },
			{ 188 },
			{ 190 },
			{ 191 },
			{ 192 },
			{ 193 },
			{ 195 },
			{ 196 },
			{ 197 },
			{ 241 },
			{ 249 },
			{ 262 },
			{ 267 },
			{ 292 },
			{ 304 },
			{ 341 }
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
//G23:  __E___E___E_library_8_9_10(36) => wff_rule(37).
	p(NT(36), (NT(37)));
//G24:  __E___E___E_library_8_9_10(36) => bf_rule(38).
	p(NT(36), (NT(38)));
//G25:  rule(35)             => __E___E___E_library_8_9_10(36).
	p(NT(35), (NT(36)));
//G26:  __E___E_library_8_9(34) => _(11) rule(35).
	p(NT(34), (NT(11)+NT(35)));
//G27:  __E___E_library_8_11(39) => null.
	p(NT(39), (nul));
//G28:  __E___E_library_8_11(39) => __E___E_library_8_9(34) __E___E_library_8_11(39).
	p(NT(39), (NT(34)+NT(39)));
//G29:  __E_library_8(33)    => __E___E_library_8_11(39).
	p(NT(33), (NT(39)));
//G30:  rules(32)            => __E_library_8(33).
	p(NT(32), (NT(33)));
//G31:  library(31)          => rules(32).
	p(NT(31), (NT(32)));
//G32:  wff_matcher(40)      => wff(14).
	p(NT(40), (NT(14)));
//G33:  __E_wff_rule_12(42)  => wff(14).
	p(NT(42), (NT(14)));
//G34:  __E_wff_rule_12(42)  => wff_cb(43).
	p(NT(42), (NT(43)));
//G35:  wff_body(41)         => __E_wff_rule_12(42).
	p(NT(41), (NT(42)));
//G36:  wff_rule(37)         => wff_matcher(40) _(11) ':' ':' '=' _(11) wff_body(41) _(11) '.'.
	p(NT(37), (NT(40)+NT(11)+T(2)+T(2)+T(3)+NT(11)+NT(41)+NT(11)+T(1)));
//G37:  bf_matcher(44)       => bf(21).
	p(NT(44), (NT(21)));
//G38:  __E_bf_rule_13(46)   => bf(21).
	p(NT(46), (NT(21)));
//G39:  __E_bf_rule_13(46)   => bf_cb(47).
	p(NT(46), (NT(47)));
//G40:  bf_body(45)          => __E_bf_rule_13(46).
	p(NT(45), (NT(46)));
//G41:  bf_rule(38)          => bf_matcher(44) _(11) ':' '=' _(11) bf_body(45) _(11) '.'.
	p(NT(38), (NT(44)+NT(11)+T(2)+T(3)+NT(11)+NT(45)+NT(11)+T(1)));
//G42:  builder(48)          => _(11) builder_head(49) _(11) builder_body(50) _(11) '.'.
	p(NT(48), (NT(11)+NT(49)+NT(11)+NT(50)+NT(11)+T(1)));
//G43:  __E_builder_head_14(51) => __(52) capture(20).
	p(NT(51), (NT(52)+NT(20)));
//G44:  __E_builder_head_15(53) => null.
	p(NT(53), (nul));
//G45:  __E_builder_head_15(53) => __E_builder_head_14(51) __E_builder_head_15(53).
	p(NT(53), (NT(51)+NT(53)));
//G46:  builder_head(49)     => '(' _(11) capture(20) __E_builder_head_15(53) _(11) ')'.
	p(NT(49), (T(5)+NT(11)+NT(20)+NT(53)+NT(11)+T(6)));
//G47:  __E_builder_body_16(55) => '=' ':' _(11) bf(21).
	p(NT(55), (T(3)+T(2)+NT(11)+NT(21)));
//G48:  bf_builder_body(54)  => __E_builder_body_16(55).
	p(NT(54), (NT(55)));
//G49:  builder_body(50)     => bf_builder_body(54).
	p(NT(50), (NT(54)));
//G50:  __E_builder_body_17(57) => '=' ':' ':' _(11) wff(14).
	p(NT(57), (T(3)+T(2)+T(2)+NT(11)+NT(14)));
//G51:  wff_builder_body(56) => __E_builder_body_17(57).
	p(NT(56), (NT(57)));
//G52:  builder_body(50)     => wff_builder_body(56).
	p(NT(50), (NT(56)));
//G53:  __E_wff_18(59)       => '(' _(11) wff(14) _(11) ')'.
	p(NT(59), (T(5)+NT(11)+NT(14)+NT(11)+T(6)));
//G54:  wff_parenthesis(58)  => __E_wff_18(59).
	p(NT(58), (NT(59)));
//G55:  wff(14)              => wff_parenthesis(58).
	p(NT(14), (NT(58)));
//G56:  __E___E_wff_19_20(62) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(62), (T(7)+T(8)+T(9)+T(10)+T(11)+T(12)+T(9)+T(10)+T(7)));
//G57:  __E___E_wff_19_20(62) => '<' '>'.
	p(NT(62), (T(13)+T(14)));
//G58:  __E_wff_19(61)       => __E___E_wff_19_20(62) _(11) wff(14).
	p(NT(61), (NT(62)+NT(11)+NT(14)));
//G59:  wff_sometimes(60)    => __E_wff_19(61).
	p(NT(60), (NT(61)));
//G60:  wff(14)              => wff_sometimes(60).
	p(NT(14), (NT(60)));
//G61:  __E___E_wff_21_22(65) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(65), (T(15)+T(16)+T(17)+T(15)+T(18)+T(7)));
//G62:  __E___E_wff_21_22(65) => '[' ']'.
	p(NT(65), (T(19)+T(20)));
//G63:  __E_wff_21(64)       => __E___E_wff_21_22(65) _(11) wff(14).
	p(NT(64), (NT(65)+NT(11)+NT(14)));
//G64:  wff_always(63)       => __E_wff_21(64).
	p(NT(63), (NT(64)));
//G65:  wff(14)              => wff_always(63).
	p(NT(14), (NT(63)));
//G66:  __E_wff_23(67)       => wff(14) _(11) '?' _(11) wff(14) _(11) ':' _(11) wff(14).
	p(NT(67), (NT(14)+NT(11)+T(21)+NT(11)+NT(14)+NT(11)+T(2)+NT(11)+NT(14)));
//G67:  wff_conditional(66)  => __E_wff_23(67).
	p(NT(66), (NT(67)));
//G68:  wff(14)              => wff_conditional(66).
	p(NT(14), (NT(66)));
//G69:  __E_wff_24(69)       => 'a' 'l' 'l' __(52) q_vars(70) __(52) wff(14).
	p(NT(69), (T(15)+T(16)+T(16)+NT(52)+NT(70)+NT(52)+NT(14)));
//G70:  wff_all(68)          => __E_wff_24(69).
	p(NT(68), (NT(69)));
//G71:  wff(14)              => wff_all(68).
	p(NT(14), (NT(68)));
//G72:  __E_wff_25(72)       => 'e' 'x' __(52) q_vars(70) __(52) wff(14).
	p(NT(72), (T(10)+T(22)+NT(52)+NT(70)+NT(52)+NT(14)));
//G73:  wff_ex(71)           => __E_wff_25(72).
	p(NT(71), (NT(72)));
//G74:  wff(14)              => wff_ex(71).
	p(NT(14), (NT(71)));
//G75:  wff_ref(73)          => ref(18).
	p(NT(73), (NT(18)));
//G76:  wff(14)              => wff_ref(73).
	p(NT(14), (NT(73)));
//G77:  wff(14)              => constraint(74).
	p(NT(14), (NT(74)));
//G78:  __E_wff_26(76)       => wff(14) _(11) '-' '>' _(11) wff(14).
	p(NT(76), (NT(14)+NT(11)+T(23)+T(14)+NT(11)+NT(14)));
//G79:  wff_imply(75)        => __E_wff_26(76).
	p(NT(75), (NT(76)));
//G80:  wff(14)              => wff_imply(75).
	p(NT(14), (NT(75)));
//G81:  __E_wff_27(78)       => wff(14) _(11) '<' '-' '>' _(11) wff(14).
	p(NT(78), (NT(14)+NT(11)+T(13)+T(23)+T(14)+NT(11)+NT(14)));
//G82:  wff_equiv(77)        => __E_wff_27(78).
	p(NT(77), (NT(78)));
//G83:  wff(14)              => wff_equiv(77).
	p(NT(14), (NT(77)));
//G84:  __E_wff_28(80)       => wff(14) _(11) '|' '|' _(11) wff(14).
	p(NT(80), (NT(14)+NT(11)+T(24)+T(24)+NT(11)+NT(14)));
//G85:  wff_or(79)           => __E_wff_28(80).
	p(NT(79), (NT(80)));
//G86:  wff(14)              => wff_or(79).
	p(NT(14), (NT(79)));
//G87:  __E_wff_29(82)       => wff(14) _(11) '^' _(11) wff(14).
	p(NT(82), (NT(14)+NT(11)+T(25)+NT(11)+NT(14)));
//G88:  wff_xor(81)          => __E_wff_29(82).
	p(NT(81), (NT(82)));
//G89:  wff(14)              => wff_xor(81).
	p(NT(14), (NT(81)));
//G90:  __E_wff_30(84)       => wff(14) _(11) '&' '&' _(11) wff(14).
	p(NT(84), (NT(14)+NT(11)+T(26)+T(26)+NT(11)+NT(14)));
//G91:  wff_and(83)          => __E_wff_30(84).
	p(NT(83), (NT(84)));
//G92:  wff(14)              => wff_and(83).
	p(NT(14), (NT(83)));
//G93:  __E_wff_31(86)       => '!' _(11) wff(14).
	p(NT(86), (T(27)+NT(11)+NT(14)));
//G94:  wff_neg(85)          => __E_wff_31(86).
	p(NT(85), (NT(86)));
//G95:  wff(14)              => wff_neg(85).
	p(NT(14), (NT(85)));
//G96:  wff_t(87)            => 'T'.
	p(NT(87), (T(28)));
//G97:  wff(14)              => wff_t(87).
	p(NT(14), (NT(87)));
//G98:  wff_f(88)            => 'F'.
	p(NT(88), (T(29)));
//G99:  wff(14)              => wff_f(88).
	p(NT(14), (NT(88)));
//G100: __E_wff_32(90)       => bf(21) _(11) '<' '=' _(11) bf(21) _(11) '<' '=' _(11) bf(21).
	p(NT(90), (NT(21)+NT(11)+T(13)+T(3)+NT(11)+NT(21)+NT(11)+T(13)+T(3)+NT(11)+NT(21)));
//G101: bf_interval(89)      => __E_wff_32(90).
	p(NT(89), (NT(90)));
//G102: wff(14)              => bf_interval(89).
	p(NT(14), (NT(89)));
//G103: __E_wff_33(92)       => bf(21) _(11) '!' '=' _(11) bf(21).
	p(NT(92), (NT(21)+NT(11)+T(27)+T(3)+NT(11)+NT(21)));
//G104: bf_neq(91)           => __E_wff_33(92).
	p(NT(91), (NT(92)));
//G105: wff(14)              => bf_neq(91).
	p(NT(14), (NT(91)));
//G106: __E_wff_34(94)       => bf(21) _(11) '=' _(11) bf(21).
	p(NT(94), (NT(21)+NT(11)+T(3)+NT(11)+NT(21)));
//G107: bf_eq(93)            => __E_wff_34(94).
	p(NT(93), (NT(94)));
//G108: wff(14)              => bf_eq(93).
	p(NT(14), (NT(93)));
//G109: __E_wff_35(96)       => bf(21) _(11) '!' '<' '=' _(11) bf(21).
	p(NT(96), (NT(21)+NT(11)+T(27)+T(13)+T(3)+NT(11)+NT(21)));
//G110: bf_nleq(95)          => __E_wff_35(96).
	p(NT(95), (NT(96)));
//G111: wff(14)              => bf_nleq(95).
	p(NT(14), (NT(95)));
//G112: __E_wff_36(98)       => bf(21) _(11) '>' _(11) bf(21).
	p(NT(98), (NT(21)+NT(11)+T(14)+NT(11)+NT(21)));
//G113: bf_greater(97)       => __E_wff_36(98).
	p(NT(97), (NT(98)));
//G114: wff(14)              => bf_greater(97).
	p(NT(14), (NT(97)));
//G115: __E_wff_37(100)      => bf(21) _(11) '<' '=' _(11) bf(21).
	p(NT(100), (NT(21)+NT(11)+T(13)+T(3)+NT(11)+NT(21)));
//G116: bf_less_equal(99)    => __E_wff_37(100).
	p(NT(99), (NT(100)));
//G117: wff(14)              => bf_less_equal(99).
	p(NT(14), (NT(99)));
//G118: __E_wff_38(102)      => bf(21) _(11) '<' _(11) bf(21).
	p(NT(102), (NT(21)+NT(11)+T(13)+NT(11)+NT(21)));
//G119: bf_less(101)         => __E_wff_38(102).
	p(NT(101), (NT(102)));
//G120: wff(14)              => bf_less(101).
	p(NT(14), (NT(101)));
//G121: wff(14)              => capture(20).
	p(NT(14), (NT(20)));
//G122: __E_bf_39(104)       => '(' _(11) bf(21) _(11) ')'.
	p(NT(104), (T(5)+NT(11)+NT(21)+NT(11)+T(6)));
//G123: bf_parenthesis(103)  => __E_bf_39(104).
	p(NT(103), (NT(104)));
//G124: bf(21)               => bf_parenthesis(103).
	p(NT(21), (NT(103)));
//G125: bf(21)               => bf_constant(105).
	p(NT(21), (NT(105)));
//G126: bf(21)               => variable(106).
	p(NT(21), (NT(106)));
//G127: __E_bf_40(108)       => 'S' _(11) '(' _(11) bf(21) _(11) ')'.
	p(NT(108), (T(30)+NT(11)+T(5)+NT(11)+NT(21)+NT(11)+T(6)));
//G128: bf_splitter(107)     => __E_bf_40(108).
	p(NT(107), (NT(108)));
//G129: bf(21)               => bf_splitter(107).
	p(NT(21), (NT(107)));
//G130: bf_ref(109)          => ref(18).
	p(NT(109), (NT(18)));
//G131: bf(21)               => bf_ref(109).
	p(NT(21), (NT(109)));
//G132: __E_bf_41(111)       => bf(21) _(11) '|' _(11) bf(21).
	p(NT(111), (NT(21)+NT(11)+T(24)+NT(11)+NT(21)));
//G133: bf_or(110)           => __E_bf_41(111).
	p(NT(110), (NT(111)));
//G134: bf(21)               => bf_or(110).
	p(NT(21), (NT(110)));
//G135: __E_bf_42(113)       => bf(21) _(11) '+' _(11) bf(21).
	p(NT(113), (NT(21)+NT(11)+T(31)+NT(11)+NT(21)));
//G136: bf_xor(112)          => __E_bf_42(113).
	p(NT(112), (NT(113)));
//G137: bf(21)               => bf_xor(112).
	p(NT(21), (NT(112)));
//G138: __E___E_bf_43_44(116) => _(11) '&' _(11).
	p(NT(116), (NT(11)+T(26)+NT(11)));
//G139: __E___E_bf_43_44(116) => _(11).
	p(NT(116), (NT(11)));
//G140: __E_bf_43(115)       => bf(21) __E___E_bf_43_44(116) bf(21).
	p(NT(115), (NT(21)+NT(116)+NT(21)));
//G141: bf_and(114)          => __E_bf_43(115).
	p(NT(114), (NT(115)));
//G142: bf(21)               => bf_and(114).
	p(NT(21), (NT(114)));
//G143: __E_bf_45(118)       => bf(21) _(11) '\''.
	p(NT(118), (NT(21)+NT(11)+T(32)));
//G144: bf_neg(117)          => __E_bf_45(118).
	p(NT(117), (NT(118)));
//G145: bf(21)               => bf_neg(117).
	p(NT(21), (NT(117)));
//G146: bf_t(119)            => '1'.
	p(NT(119), (T(33)));
//G147: bf(21)               => bf_t(119).
	p(NT(21), (NT(119)));
//G148: bf_f(120)            => '0'.
	p(NT(120), (T(34)));
//G149: bf(21)               => bf_f(120).
	p(NT(21), (NT(120)));
//G150: bf(21)               => capture(20).
	p(NT(21), (NT(20)));
//G151: bf(21)               => uninterpreted_constant(121).
	p(NT(21), (NT(121)));
//G152: __E_constraint_46(123) => '[' ctnvar(124) _(11) '!' '=' _(11) num(125) ']'.
	p(NT(123), (T(19)+NT(124)+NT(11)+T(27)+T(3)+NT(11)+NT(125)+T(20)));
//G153: __E_constraint_46(123) => '[' num(125) _(11) '!' '=' _(11) ctnvar(124) ']'.
	p(NT(123), (T(19)+NT(125)+NT(11)+T(27)+T(3)+NT(11)+NT(124)+T(20)));
//G154: ctn_neq(122)         => __E_constraint_46(123).
	p(NT(122), (NT(123)));
//G155: constraint(74)       => ctn_neq(122).
	p(NT(74), (NT(122)));
//G156: __E_constraint_47(127) => '[' ctnvar(124) _(11) '=' _(11) num(125) ']'.
	p(NT(127), (T(19)+NT(124)+NT(11)+T(3)+NT(11)+NT(125)+T(20)));
//G157: __E_constraint_47(127) => '[' num(125) _(11) '=' _(11) ctnvar(124) ']'.
	p(NT(127), (T(19)+NT(125)+NT(11)+T(3)+NT(11)+NT(124)+T(20)));
//G158: ctn_eq(126)          => __E_constraint_47(127).
	p(NT(126), (NT(127)));
//G159: constraint(74)       => ctn_eq(126).
	p(NT(74), (NT(126)));
//G160: __E_constraint_48(129) => '[' ctnvar(124) _(11) '>' '=' _(11) num(125) ']'.
	p(NT(129), (T(19)+NT(124)+NT(11)+T(14)+T(3)+NT(11)+NT(125)+T(20)));
//G161: __E_constraint_48(129) => '[' num(125) _(11) '>' '=' _(11) ctnvar(124) ']'.
	p(NT(129), (T(19)+NT(125)+NT(11)+T(14)+T(3)+NT(11)+NT(124)+T(20)));
//G162: ctn_greater_equal(128) => __E_constraint_48(129).
	p(NT(128), (NT(129)));
//G163: constraint(74)       => ctn_greater_equal(128).
	p(NT(74), (NT(128)));
//G164: __E_constraint_49(131) => '[' ctnvar(124) _(11) '>' _(11) num(125) ']'.
	p(NT(131), (T(19)+NT(124)+NT(11)+T(14)+NT(11)+NT(125)+T(20)));
//G165: __E_constraint_49(131) => '[' num(125) _(11) '>' _(11) ctnvar(124) ']'.
	p(NT(131), (T(19)+NT(125)+NT(11)+T(14)+NT(11)+NT(124)+T(20)));
//G166: ctn_greater(130)     => __E_constraint_49(131).
	p(NT(130), (NT(131)));
//G167: constraint(74)       => ctn_greater(130).
	p(NT(74), (NT(130)));
//G168: __E_constraint_50(133) => '[' ctnvar(124) _(11) '<' '=' _(11) num(125) ']'.
	p(NT(133), (T(19)+NT(124)+NT(11)+T(13)+T(3)+NT(11)+NT(125)+T(20)));
//G169: __E_constraint_50(133) => '[' num(125) _(11) '<' '=' _(11) ctnvar(124) ']'.
	p(NT(133), (T(19)+NT(125)+NT(11)+T(13)+T(3)+NT(11)+NT(124)+T(20)));
//G170: ctn_less_equal(132)  => __E_constraint_50(133).
	p(NT(132), (NT(133)));
//G171: constraint(74)       => ctn_less_equal(132).
	p(NT(74), (NT(132)));
//G172: __E_constraint_51(135) => '[' ctnvar(124) _(11) '<' _(11) num(125) ']'.
	p(NT(135), (T(19)+NT(124)+NT(11)+T(13)+NT(11)+NT(125)+T(20)));
//G173: __E_constraint_51(135) => '[' num(125) _(11) '<' _(11) ctnvar(124) ']'.
	p(NT(135), (T(19)+NT(125)+NT(11)+T(13)+NT(11)+NT(124)+T(20)));
//G174: ctn_less(134)        => __E_constraint_51(135).
	p(NT(134), (NT(135)));
//G175: constraint(74)       => ctn_less(134).
	p(NT(74), (NT(134)));
//G176: __N_0(376)           => 'F'.
	p(NT(376), (T(29)));
//G177: __N_1(377)           => 'T'.
	p(NT(377), (T(28)));
//G178: __E_charvar_52(137)  => ~( __N_0(376) ) & ~( __N_1(377) ) & alpha(5).	 # conjunctive
	p(NT(137), ~(NT(376)) & ~(NT(377)) & (NT(5)));
//G179: __E_charvar_53(138)  => null.
	p(NT(138), (nul));
//G180: __E_charvar_53(138)  => digit(3) __E_charvar_53(138).
	p(NT(138), (NT(3)+NT(138)));
//G181: charvar(136)         => __E_charvar_52(137) __E_charvar_53(138).
	p(NT(136), (NT(137)+NT(138)));
//G182: ctnvar(124)          => charvar(136).
	p(NT(124), (NT(136)));
//G183: variable(106)        => charvar(136).
	p(NT(106), (NT(136)));
//G184: __E_variable_54(140) => in(141).
	p(NT(140), (NT(141)));
//G185: __E_variable_54(140) => out(142).
	p(NT(140), (NT(142)));
//G186: io_var(139)          => __E_variable_54(140).
	p(NT(139), (NT(140)));
//G187: variable(106)        => io_var(139).
	p(NT(106), (NT(139)));
//G188: __E___E___E_in_55_56_57(146) => digit(3).
	p(NT(146), (NT(3)));
//G189: __E___E___E_in_55_56_57(146) => digit(3) __E___E___E_in_55_56_57(146).
	p(NT(146), (NT(3)+NT(146)));
//G190: __E___E_in_55_56(145) => __E___E___E_in_55_56_57(146).
	p(NT(145), (NT(146)));
//G191: __E_in_55(144)       => 'i' __E___E_in_55_56(145).
	p(NT(144), (T(12)+NT(145)));
//G192: in_var_name(143)     => __E_in_55(144).
	p(NT(143), (NT(144)));
//G193: in(141)              => in_var_name(143) '[' offset(147) ']'.
	p(NT(141), (NT(143)+T(19)+NT(147)+T(20)));
//G194: __E___E___E_out_58_59_60(151) => digit(3).
	p(NT(151), (NT(3)));
//G195: __E___E___E_out_58_59_60(151) => digit(3) __E___E___E_out_58_59_60(151).
	p(NT(151), (NT(3)+NT(151)));
//G196: __E___E_out_58_59(150) => __E___E___E_out_58_59_60(151).
	p(NT(150), (NT(151)));
//G197: __E_out_58(149)      => 'o' __E___E_out_58_59(150).
	p(NT(149), (T(8)+NT(150)));
//G198: out_var_name(148)    => __E_out_58(149).
	p(NT(148), (NT(149)));
//G199: out(142)             => out_var_name(148) '[' offset(147) ']'.
	p(NT(142), (NT(148)+T(19)+NT(147)+T(20)));
//G200: capture(20)          => '$' charvar(136).
	p(NT(20), (T(35)+NT(136)));
//G201: __E_q_vars_61(153)   => _(11) ',' _(11) q_var(152).
	p(NT(153), (NT(11)+T(4)+NT(11)+NT(152)));
//G202: __E_q_vars_62(154)   => null.
	p(NT(154), (nul));
//G203: __E_q_vars_62(154)   => __E_q_vars_61(153) __E_q_vars_62(154).
	p(NT(154), (NT(153)+NT(154)));
//G204: q_vars(70)           => q_var(152) __E_q_vars_62(154).
	p(NT(70), (NT(152)+NT(154)));
//G205: q_var(152)           => capture(20).
	p(NT(152), (NT(20)));
//G206: q_var(152)           => variable(106).
	p(NT(152), (NT(106)));
//G207: __E_offsets_63(155)  => _(11) ',' _(11) offset(147).
	p(NT(155), (NT(11)+T(4)+NT(11)+NT(147)));
//G208: __E_offsets_64(156)  => null.
	p(NT(156), (nul));
//G209: __E_offsets_64(156)  => __E_offsets_63(155) __E_offsets_64(156).
	p(NT(156), (NT(155)+NT(156)));
//G210: offsets(24)          => '[' _(11) offset(147) __E_offsets_64(156) _(11) ']'.
	p(NT(24), (T(19)+NT(11)+NT(147)+NT(156)+NT(11)+T(20)));
//G211: offset(147)          => num(125).
	p(NT(147), (NT(125)));
//G212: offset(147)          => capture(20).
	p(NT(147), (NT(20)));
//G213: offset(147)          => shift(157).
	p(NT(147), (NT(157)));
//G214: __N_2(378)           => io_var(139).
	p(NT(378), (NT(139)));
//G215: __E_offset_65(158)   => variable(106) & ~( __N_2(378) ).	 # conjunctive
	p(NT(158), (NT(106)) & ~(NT(378)));
//G216: offset(147)          => __E_offset_65(158).
	p(NT(147), (NT(158)));
//G217: __E_shift_66(159)    => capture(20).
	p(NT(159), (NT(20)));
//G218: __N_3(379)           => io_var(139).
	p(NT(379), (NT(139)));
//G219: __E___E_shift_66_67(160) => variable(106) & ~( __N_3(379) ).	 # conjunctive
	p(NT(160), (NT(106)) & ~(NT(379)));
//G220: __E_shift_66(159)    => __E___E_shift_66_67(160).
	p(NT(159), (NT(160)));
//G221: shift(157)           => __E_shift_66(159) _(11) '-' _(11) num(125).
	p(NT(157), (NT(159)+NT(11)+T(23)+NT(11)+NT(125)));
//G222: num(125)             => digits(161).
	p(NT(125), (NT(161)));
//G223: __E___E_uninterpreted_constant_68_69(164) => chars(165).
	p(NT(164), (NT(165)));
//G224: __E___E_uninterpreted_constant_68_69(164) => _(11).
	p(NT(164), (NT(11)));
//G225: __E_uninterpreted_constant_68(163) => __E___E_uninterpreted_constant_68_69(164) ':' chars(165).
	p(NT(163), (NT(164)+T(2)+NT(165)));
//G226: uninter_const_name(162) => __E_uninterpreted_constant_68(163).
	p(NT(162), (NT(163)));
//G227: uninterpreted_constant(121) => '<' uninter_const_name(162) _(11) '>'.
	p(NT(121), (T(13)+NT(162)+NT(11)+T(14)));
//G228: __E_bf_constant_70(167) => capture(20).
	p(NT(167), (NT(20)));
//G229: __E_bf_constant_70(167) => binding(168).
	p(NT(167), (NT(168)));
//G230: constant(166)        => __E_bf_constant_70(167).
	p(NT(166), (NT(167)));
//G231: type(170)            => chars(165).
	p(NT(170), (NT(165)));
//G232: __E_bf_constant_71(169) => _(11) ':' _(11) type(170).
	p(NT(169), (NT(11)+T(2)+NT(11)+NT(170)));
//G233: __E_bf_constant_71(169) => null.
	p(NT(169), (nul));
//G234: bf_constant(105)     => '{' _(11) constant(166) _(11) '}' __E_bf_constant_71(169).
	p(NT(105), (T(36)+NT(11)+NT(166)+NT(11)+T(37)+NT(169)));
//G235: __E___E___E_binding_72_73_74(175) => src_c(173).
	p(NT(175), (NT(173)));
//G236: __E___E___E_binding_72_73_74(175) => space(2).
	p(NT(175), (NT(2)));
//G237: __E___E___E_binding_72_73_75(176) => null.
	p(NT(176), (nul));
//G238: __E___E___E_binding_72_73_75(176) => __E___E___E_binding_72_73_74(175) __E___E___E_binding_72_73_75(176).
	p(NT(176), (NT(175)+NT(176)));
//G239: __E___E_binding_72_73(174) => __E___E___E_binding_72_73_75(176) src_c(173).
	p(NT(174), (NT(176)+NT(173)));
//G240: __E___E_binding_72_73(174) => null.
	p(NT(174), (nul));
//G241: __E_binding_72(172)  => src_c(173) __E___E_binding_72_73(174).
	p(NT(172), (NT(173)+NT(174)));
//G242: source(171)          => __E_binding_72(172).
	p(NT(171), (NT(172)));
//G243: binding(168)         => source(171).
	p(NT(168), (NT(171)));
//G244: src_c(173)           => alnum(6).
	p(NT(173), (NT(6)));
//G245: __N_4(380)           => '{'.
	p(NT(380), (T(36)));
//G246: __N_5(381)           => '}'.
	p(NT(381), (T(37)));
//G247: src_c(173)           => ~( __N_4(380) ) & ~( __N_5(381) ) & punct(7).	 # conjunctive
	p(NT(173), ~(NT(380)) & ~(NT(381)) & (NT(7)));
//G248: __E_src_c_76(177)    => src_c(173).
	p(NT(177), (NT(173)));
//G249: __E_src_c_76(177)    => space(2).
	p(NT(177), (NT(2)));
//G250: __E_src_c_77(178)    => null.
	p(NT(178), (nul));
//G251: __E_src_c_77(178)    => __E_src_c_76(177) __E_src_c_77(178).
	p(NT(178), (NT(177)+NT(178)));
//G252: src_c(173)           => '{' __E_src_c_77(178) '}'.
	p(NT(173), (T(36)+NT(178)+T(37)));
//G253: __E_chars_78(179)    => null.
	p(NT(179), (nul));
//G254: __E_chars_78(179)    => alnum(6) __E_chars_78(179).
	p(NT(179), (NT(6)+NT(179)));
//G255: chars(165)           => alpha(5) __E_chars_78(179).
	p(NT(165), (NT(5)+NT(179)));
//G256: __E_digits_79(180)   => digit(3).
	p(NT(180), (NT(3)));
//G257: __E_digits_79(180)   => digit(3) __E_digits_79(180).
	p(NT(180), (NT(3)+NT(180)));
//G258: digits(161)          => __E_digits_79(180).
	p(NT(161), (NT(180)));
//G259: sym(22)              => chars(165).
	p(NT(22), (NT(165)));
//G260: extra(181)           => chars(165).
	p(NT(181), (NT(165)));
//G261: __E_comment_80(183)  => printable(8).
	p(NT(183), (NT(8)));
//G262: __E_comment_80(183)  => '\t'.
	p(NT(183), (T(38)));
//G263: __E_comment_81(184)  => null.
	p(NT(184), (nul));
//G264: __E_comment_81(184)  => __E_comment_80(183) __E_comment_81(184).
	p(NT(184), (NT(183)+NT(184)));
//G265: __E_comment_82(185)  => '\n'.
	p(NT(185), (T(39)));
//G266: __E_comment_82(185)  => '\r'.
	p(NT(185), (T(40)));
//G267: __E_comment_82(185)  => eof(1).
	p(NT(185), (NT(1)));
//G268: comment(182)         => '#' __E_comment_81(184) __E_comment_82(185).
	p(NT(182), (T(41)+NT(184)+NT(185)));
//G269: __E____83(186)       => space(2).
	p(NT(186), (NT(2)));
//G270: __E____83(186)       => comment(182).
	p(NT(186), (NT(182)));
//G271: __(52)               => __E____83(186) _(11).
	p(NT(52), (NT(186)+NT(11)));
//G272: __E___84(187)        => __(52).
	p(NT(187), (NT(52)));
//G273: __E___84(187)        => null.
	p(NT(187), (nul));
//G274: _(11)                => __E___84(187).
	p(NT(11), (NT(187)));
//G275: bf_cb_arg(189)       => bf(21).
	p(NT(189), (NT(21)));
//G276: bf_cb_args1(188)     => __(52) bf_cb_arg(189).
	p(NT(188), (NT(52)+NT(189)));
//G277: bf_cb_args2(190)     => bf_cb_args1(188) bf_cb_args1(188).
	p(NT(190), (NT(188)+NT(188)));
//G278: bf_cb_args3(191)     => bf_cb_args2(190) bf_cb_args1(188).
	p(NT(191), (NT(190)+NT(188)));
//G279: bf_cb_args4(192)     => bf_cb_args3(191) bf_cb_args1(188).
	p(NT(192), (NT(191)+NT(188)));
//G280: wff_cb_arg(194)      => wff(14).
	p(NT(194), (NT(14)));
//G281: wff_cb_args1(193)    => __(52) wff_cb_arg(194).
	p(NT(193), (NT(52)+NT(194)));
//G282: wff_cb_args2(195)    => wff_cb_args1(193) wff_cb_args1(193).
	p(NT(195), (NT(193)+NT(193)));
//G283: wff_cb_args3(196)    => wff_cb_args2(195) wff_cb_args1(193).
	p(NT(196), (NT(195)+NT(193)));
//G284: wff_cb_args4(197)    => wff_cb_args3(196) wff_cb_args1(193).
	p(NT(197), (NT(196)+NT(193)));
//G285: __E_wff_cb_85(199)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(195).
	p(NT(199), (T(17)+T(42)+T(42)+T(43)+T(44)+T(15)+T(7)+T(43)+T(45)+T(16)+T(15)+T(7)+T(44)+T(12)+T(46)+T(47)+T(43)+T(7)+T(48)+T(49)+T(42)+T(8)+T(50)+T(9)+T(48)+T(16)+T(15)+T(7)+T(43)+T(45)+T(49)+NT(195)));
//G286: wff_has_clashing_subformulas_cb(198) => __E_wff_cb_85(199).
	p(NT(198), (NT(199)));
//G287: wff_cb(43)           => wff_has_clashing_subformulas_cb(198).
	p(NT(43), (NT(198)));
//G288: __E_wff_cb_86(201)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(196).
	p(NT(201), (T(17)+T(42)+T(42)+T(43)+T(44)+T(15)+T(7)+T(43)+T(7)+T(48)+T(49)+T(42)+T(8)+T(50)+T(9)+T(48)+T(16)+T(15)+T(43)+T(45)+T(49)+NT(196)));
//G289: wff_has_subformula_cb(200) => __E_wff_cb_86(201).
	p(NT(200), (NT(201)));
//G290: wff_cb(43)           => wff_has_subformula_cb(200).
	p(NT(43), (NT(200)));
//G291: __E_wff_cb_87(203)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(195).
	p(NT(203), (T(17)+T(42)+T(42)+T(43)+T(50)+T(10)+T(9)+T(8)+T(51)+T(10)+T(43)+T(10)+T(22)+T(12)+T(7)+T(11)+T(10)+T(46)+T(11)+T(12)+T(15)+T(16)+T(43)+T(45)+T(49)+NT(195)));
//G292: wff_remove_existential_cb(202) => __E_wff_cb_87(203).
	p(NT(202), (NT(203)));
//G293: wff_cb(43)           => wff_remove_existential_cb(202).
	p(NT(43), (NT(202)));
//G294: __E_bf_cb_88(205)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(191).
	p(NT(205), (T(49)+T(42)+T(43)+T(44)+T(15)+T(7)+T(43)+T(7)+T(48)+T(49)+T(42)+T(8)+T(50)+T(9)+T(48)+T(16)+T(15)+T(43)+T(45)+T(49)+NT(191)));
//G295: bf_has_subformula_cb(204) => __E_bf_cb_88(205).
	p(NT(204), (NT(205)));
//G296: bf_cb(47)            => bf_has_subformula_cb(204).
	p(NT(47), (NT(204)));
//G297: __E_bf_cb_89(207)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(192).
	p(NT(207), (T(49)+T(42)+T(43)+T(50)+T(10)+T(9)+T(8)+T(51)+T(10)+T(43)+T(42)+T(48)+T(46)+T(12)+T(51)+T(10)+T(50)+T(7)+T(15)+T(16)+T(43)+T(45)+T(49)+NT(192)));
//G298: bf_remove_funiversal_cb(206) => __E_bf_cb_89(207).
	p(NT(206), (NT(207)));
//G299: bf_cb(47)            => bf_remove_funiversal_cb(206).
	p(NT(47), (NT(206)));
//G300: __E_bf_cb_90(209)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(192).
	p(NT(209), (T(49)+T(42)+T(43)+T(50)+T(10)+T(9)+T(8)+T(51)+T(10)+T(43)+T(42)+T(10)+T(22)+T(12)+T(7)+T(11)+T(10)+T(46)+T(11)+T(12)+T(15)+T(16)+T(43)+T(45)+T(49)+NT(192)));
//G301: bf_remove_fexistential_cb(208) => __E_bf_cb_90(209).
	p(NT(208), (NT(209)));
//G302: bf_cb(47)            => bf_remove_fexistential_cb(208).
	p(NT(47), (NT(208)));
//G303: __E_bf_cb_91(211)    => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(188).
	p(NT(211), (T(49)+T(42)+T(43)+T(46)+T(8)+T(50)+T(9)+T(15)+T(16)+T(12)+T(52)+T(10)+T(43)+T(45)+T(49)+NT(188)));
//G304: bf_normalize_cb(210) => __E_bf_cb_91(211).
	p(NT(210), (NT(211)));
//G305: bf_cb(47)            => bf_normalize_cb(210).
	p(NT(47), (NT(210)));
//G306: cli(212)             => _(11).
	p(NT(212), (NT(11)));
//G307: __E_cli_92(214)      => '.' _(11) cli_command(213) _(11).
	p(NT(214), (T(1)+NT(11)+NT(213)+NT(11)));
//G308: __E_cli_93(215)      => null.
	p(NT(215), (nul));
//G309: __E_cli_93(215)      => __E_cli_92(214) __E_cli_93(215).
	p(NT(215), (NT(214)+NT(215)));
//G310: cli(212)             => _(11) cli_command(213) _(11) __E_cli_93(215).
	p(NT(212), (NT(11)+NT(213)+NT(11)+NT(215)));
//G311: __E_cli_command_94(218) => 'q'.
	p(NT(218), (T(53)));
//G312: __E_cli_command_94(218) => 'q' 'u' 'i' 't'.
	p(NT(218), (T(53)+T(48)+T(12)+T(11)));
//G313: quit_sym(217)        => __E_cli_command_94(218).
	p(NT(217), (NT(218)));
//G314: quit_cmd(216)        => quit_sym(217).
	p(NT(216), (NT(217)));
//G315: cli_command(213)     => quit_cmd(216).
	p(NT(213), (NT(216)));
//G316: __E_cli_command_95(221) => 'v'.
	p(NT(221), (T(51)));
//G317: __E_cli_command_95(221) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(221), (T(51)+T(10)+T(50)+T(7)+T(12)+T(8)+T(46)));
//G318: version_sym(220)     => __E_cli_command_95(221).
	p(NT(220), (NT(221)));
//G319: version_cmd(219)     => version_sym(220).
	p(NT(219), (NT(220)));
//G320: cli_command(213)     => version_cmd(219).
	p(NT(213), (NT(219)));
//G321: __E_cli_command_96(224) => 'c'.
	p(NT(224), (T(45)));
//G322: __E_cli_command_96(224) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(224), (T(45)+T(16)+T(10)+T(15)+T(50)));
//G323: clear_sym(223)       => __E_cli_command_96(224).
	p(NT(223), (NT(224)));
//G324: clear_cmd(222)       => clear_sym(223).
	p(NT(222), (NT(223)));
//G325: cli_command(213)     => clear_cmd(222).
	p(NT(213), (NT(222)));
//G326: __E___E_cli_command_97_98(228) => 'h'.
	p(NT(228), (T(44)));
//G327: __E___E_cli_command_97_98(228) => 'h' 'e' 'l' 'p'.
	p(NT(228), (T(44)+T(10)+T(16)+T(54)));
//G328: help_sym(227)        => __E___E_cli_command_97_98(228).
	p(NT(227), (NT(228)));
//G329: __E___E_cli_command_97_99(229) => __(52) help_arg(230).
	p(NT(229), (NT(52)+NT(230)));
//G330: __E___E_cli_command_97_99(229) => null.
	p(NT(229), (nul));
//G331: __E_cli_command_97(226) => help_sym(227) __E___E_cli_command_97_99(229).
	p(NT(226), (NT(227)+NT(229)));
//G332: help_cmd(225)        => __E_cli_command_97(226).
	p(NT(225), (NT(226)));
//G333: cli_command(213)     => help_cmd(225).
	p(NT(213), (NT(225)));
//G334: file_sym(233)        => 'f' 'i' 'l' 'e'.
	p(NT(233), (T(42)+T(12)+T(16)+T(10)));
//G335: __E_cli_command_100(232) => file_sym(233) __(52) q_string(234).
	p(NT(232), (NT(233)+NT(52)+NT(234)));
//G336: file_cmd(231)        => __E_cli_command_100(232).
	p(NT(231), (NT(232)));
//G337: cli_command(213)     => file_cmd(231).
	p(NT(213), (NT(231)));
//G338: valid_sym(237)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(237), (T(51)+T(15)+T(16)+T(12)+T(55)));
//G339: __E_cli_command_101(236) => valid_sym(237) __(52) rr(10).
	p(NT(236), (NT(237)+NT(52)+NT(10)));
//G340: valid_cmd(235)       => __E_cli_command_101(236).
	p(NT(235), (NT(236)));
//G341: cli_command(213)     => valid_cmd(235).
	p(NT(213), (NT(235)));
//G342: sat_sym(240)         => 's' 'a' 't'.
	p(NT(240), (T(7)+T(15)+T(11)));
//G343: __E_cli_command_102(239) => sat_sym(240) __(52) normalize_cmd_arg(241).
	p(NT(239), (NT(240)+NT(52)+NT(241)));
//G344: sat_cmd(238)         => __E_cli_command_102(239).
	p(NT(238), (NT(239)));
//G345: cli_command(213)     => sat_cmd(238).
	p(NT(213), (NT(238)));
//G346: unsat_sym(244)       => 'u' 'n' 's' 'a' 't'.
	p(NT(244), (T(48)+T(46)+T(7)+T(15)+T(11)));
//G347: __E_cli_command_103(243) => unsat_sym(244) __(52) rr(10).
	p(NT(243), (NT(244)+NT(52)+NT(10)));
//G348: unsat_cmd(242)       => __E_cli_command_103(243).
	p(NT(242), (NT(243)));
//G349: cli_command(213)     => unsat_cmd(242).
	p(NT(213), (NT(242)));
//G350: solve_sym(247)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(247), (T(7)+T(8)+T(16)+T(51)+T(10)));
//G351: __E___E_cli_command_104_105(248) => __(52) type(170).
	p(NT(248), (NT(52)+NT(170)));
//G352: __E___E_cli_command_104_105(248) => null.
	p(NT(248), (nul));
//G353: __E_cli_command_104(246) => solve_sym(247) __E___E_cli_command_104_105(248) __(52) solve_cmd_arg(249).
	p(NT(246), (NT(247)+NT(248)+NT(52)+NT(249)));
//G354: solve_cmd(245)       => __E_cli_command_104(246).
	p(NT(245), (NT(246)));
//G355: cli_command(213)     => solve_cmd(245).
	p(NT(213), (NT(245)));
//G356: __E___E_cli_command_106_107(253) => 'r'.
	p(NT(253), (T(50)));
//G357: __E___E_cli_command_106_107(253) => 'r' 'u' 'n'.
	p(NT(253), (T(50)+T(48)+T(46)));
//G358: run_sym(252)         => __E___E_cli_command_106_107(253).
	p(NT(252), (NT(253)));
//G359: __E_cli_command_106(251) => run_sym(252) __(52) wff(14).
	p(NT(251), (NT(252)+NT(52)+NT(14)));
//G360: run_cmd(250)         => __E_cli_command_106(251).
	p(NT(250), (NT(251)));
//G361: cli_command(213)     => run_cmd(250).
	p(NT(213), (NT(250)));
//G362: __E___E_cli_command_108_109(257) => 'n'.
	p(NT(257), (T(46)));
//G363: __E___E_cli_command_108_109(257) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(257), (T(46)+T(8)+T(50)+T(9)+T(15)+T(16)+T(12)+T(52)+T(10)));
//G364: normalize_sym(256)   => __E___E_cli_command_108_109(257).
	p(NT(256), (NT(257)));
//G365: __E_cli_command_108(255) => normalize_sym(256) __(52) normalize_cmd_arg(241).
	p(NT(255), (NT(256)+NT(52)+NT(241)));
//G366: normalize_cmd(254)   => __E_cli_command_108(255).
	p(NT(254), (NT(255)));
//G367: cli_command(213)     => normalize_cmd(254).
	p(NT(213), (NT(254)));
//G368: __E___E_cli_command_110_111(261) => 's'.
	p(NT(261), (T(7)));
//G369: __E___E_cli_command_110_111(261) => 's' 'u' 'b' 's' 't'.
	p(NT(261), (T(7)+T(48)+T(49)+T(7)+T(11)));
//G370: __E___E_cli_command_110_111(261) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(261), (T(7)+T(48)+T(49)+T(7)+T(11)+T(12)+T(11)+T(48)+T(11)+T(10)));
//G371: subst_sym(260)       => __E___E_cli_command_110_111(261).
	p(NT(260), (NT(261)));
//G372: __E_cli_command_110(259) => subst_sym(260) __(52) nf_cmd_arg(262) _(11) '[' _(11) nf_cmd_arg(262) _(11) '/' _(11) nf_cmd_arg(262) _(11) ']'.
	p(NT(259), (NT(260)+NT(52)+NT(262)+NT(11)+T(19)+NT(11)+NT(262)+NT(11)+T(56)+NT(11)+NT(262)+NT(11)+T(20)));
//G373: subst_cmd(258)       => __E_cli_command_110(259).
	p(NT(258), (NT(259)));
//G374: cli_command(213)     => subst_cmd(258).
	p(NT(213), (NT(258)));
//G375: __E___E_cli_command_112_113(266) => 'i'.
	p(NT(266), (T(12)));
//G376: __E___E_cli_command_112_113(266) => 'i' 'n' 's' 't'.
	p(NT(266), (T(12)+T(46)+T(7)+T(11)));
//G377: __E___E_cli_command_112_113(266) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(266), (T(12)+T(46)+T(7)+T(11)+T(15)+T(46)+T(11)+T(12)+T(15)+T(11)+T(10)));
//G378: inst_sym(265)        => __E___E_cli_command_112_113(266).
	p(NT(265), (NT(266)));
//G379: __E_cli_command_112(264) => inst_sym(265) __(52) inst_args(267).
	p(NT(264), (NT(265)+NT(52)+NT(267)));
//G380: inst_cmd(263)        => __E_cli_command_112(264).
	p(NT(263), (NT(264)));
//G381: cli_command(213)     => inst_cmd(263).
	p(NT(213), (NT(263)));
//G382: dnf_sym(270)         => 'd' 'n' 'f'.
	p(NT(270), (T(55)+T(46)+T(42)));
//G383: __E_cli_command_114(269) => dnf_sym(270) __(52) nf_cmd_arg(262).
	p(NT(269), (NT(270)+NT(52)+NT(262)));
//G384: dnf_cmd(268)         => __E_cli_command_114(269).
	p(NT(268), (NT(269)));
//G385: cli_command(213)     => dnf_cmd(268).
	p(NT(213), (NT(268)));
//G386: cnf_sym(273)         => 'c' 'n' 'f'.
	p(NT(273), (T(45)+T(46)+T(42)));
//G387: __E_cli_command_115(272) => cnf_sym(273) __(52) nf_cmd_arg(262).
	p(NT(272), (NT(273)+NT(52)+NT(262)));
//G388: cnf_cmd(271)         => __E_cli_command_115(272).
	p(NT(271), (NT(272)));
//G389: cli_command(213)     => cnf_cmd(271).
	p(NT(213), (NT(271)));
//G390: anf_sym(276)         => 'a' 'n' 'f'.
	p(NT(276), (T(15)+T(46)+T(42)));
//G391: __E_cli_command_116(275) => anf_sym(276) __(52) nf_cmd_arg(262).
	p(NT(275), (NT(276)+NT(52)+NT(262)));
//G392: anf_cmd(274)         => __E_cli_command_116(275).
	p(NT(274), (NT(275)));
//G393: cli_command(213)     => anf_cmd(274).
	p(NT(213), (NT(274)));
//G394: nnf_sym(279)         => 'n' 'n' 'f'.
	p(NT(279), (T(46)+T(46)+T(42)));
//G395: __E_cli_command_117(278) => nnf_sym(279) __(52) nf_cmd_arg(262).
	p(NT(278), (NT(279)+NT(52)+NT(262)));
//G396: nnf_cmd(277)         => __E_cli_command_117(278).
	p(NT(277), (NT(278)));
//G397: cli_command(213)     => nnf_cmd(277).
	p(NT(213), (NT(277)));
//G398: pnf_sym(282)         => 'p' 'n' 'f'.
	p(NT(282), (T(54)+T(46)+T(42)));
//G399: __E_cli_command_118(281) => pnf_sym(282) __(52) nf_cmd_arg(262).
	p(NT(281), (NT(282)+NT(52)+NT(262)));
//G400: pnf_cmd(280)         => __E_cli_command_118(281).
	p(NT(280), (NT(281)));
//G401: cli_command(213)     => pnf_cmd(280).
	p(NT(213), (NT(280)));
//G402: mnf_sym(285)         => 'm' 'n' 'f'.
	p(NT(285), (T(9)+T(46)+T(42)));
//G403: __E_cli_command_119(284) => mnf_sym(285) __(52) nf_cmd_arg(262).
	p(NT(284), (NT(285)+NT(52)+NT(262)));
//G404: mnf_cmd(283)         => __E_cli_command_119(284).
	p(NT(283), (NT(284)));
//G405: cli_command(213)     => mnf_cmd(283).
	p(NT(213), (NT(283)));
//G406: snf_sym(288)         => 's' 'n' 'f'.
	p(NT(288), (T(7)+T(46)+T(42)));
//G407: __E_cli_command_120(287) => snf_sym(288) __(52) nf_cmd_arg(262).
	p(NT(287), (NT(288)+NT(52)+NT(262)));
//G408: snf_cmd(286)         => __E_cli_command_120(287).
	p(NT(286), (NT(287)));
//G409: cli_command(213)     => snf_cmd(286).
	p(NT(213), (NT(286)));
//G410: onf_sym(291)         => 'o' 'n' 'f'.
	p(NT(291), (T(8)+T(46)+T(42)));
//G411: __E_cli_command_121(290) => onf_sym(291) __(52) variable(106) __(52) onf_cmd_arg(292).
	p(NT(290), (NT(291)+NT(52)+NT(106)+NT(52)+NT(292)));
//G412: onf_cmd(289)         => __E_cli_command_121(290).
	p(NT(289), (NT(290)));
//G413: cli_command(213)     => onf_cmd(289).
	p(NT(213), (NT(289)));
//G414: __E___E_cli_command_122_123(296) => 'd' 'e' 'f' 's'.
	p(NT(296), (T(55)+T(10)+T(42)+T(7)));
//G415: __E___E_cli_command_122_123(296) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(296), (T(55)+T(10)+T(42)+T(12)+T(46)+T(12)+T(11)+T(12)+T(8)+T(46)+T(7)));
//G416: def_sym(295)         => __E___E_cli_command_122_123(296).
	p(NT(295), (NT(296)));
//G417: __E_cli_command_122(294) => def_sym(295).
	p(NT(294), (NT(295)));
//G418: def_list_cmd(293)    => __E_cli_command_122(294).
	p(NT(293), (NT(294)));
//G419: cli_command(213)     => def_list_cmd(293).
	p(NT(213), (NT(293)));
//G420: __E_cli_command_124(298) => def_sym(295) __(52) number(299).
	p(NT(298), (NT(295)+NT(52)+NT(299)));
//G421: def_print_cmd(297)   => __E_cli_command_124(298).
	p(NT(297), (NT(298)));
//G422: cli_command(213)     => def_print_cmd(297).
	p(NT(213), (NT(297)));
//G423: def_rr_cmd(300)      => rec_relation(16).
	p(NT(300), (NT(16)));
//G424: cli_command(213)     => def_rr_cmd(300).
	p(NT(213), (NT(300)));
//G425: qelim_sym(303)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(303), (T(53)+T(10)+T(16)+T(12)+T(9)));
//G426: __E_cli_command_125(302) => qelim_sym(303) __(52) wff_cmd_arg(304).
	p(NT(302), (NT(303)+NT(52)+NT(304)));
//G427: qelim_cmd(301)       => __E_cli_command_125(302).
	p(NT(301), (NT(302)));
//G428: cli_command(213)     => qelim_cmd(301).
	p(NT(213), (NT(301)));
//G429: get_sym(307)         => 'g' 'e' 't'.
	p(NT(307), (T(47)+T(10)+T(11)));
//G430: __E___E_cli_command_126_127(308) => __(52) option(309).
	p(NT(308), (NT(52)+NT(309)));
//G431: __E___E_cli_command_126_127(308) => null.
	p(NT(308), (nul));
//G432: __E_cli_command_126(306) => get_sym(307) __E___E_cli_command_126_127(308).
	p(NT(306), (NT(307)+NT(308)));
//G433: get_cmd(305)         => __E_cli_command_126(306).
	p(NT(305), (NT(306)));
//G434: cli_command(213)     => get_cmd(305).
	p(NT(213), (NT(305)));
//G435: set_sym(312)         => 's' 'e' 't'.
	p(NT(312), (T(7)+T(10)+T(11)));
//G436: __E___E_cli_command_128_129(313) => __(52).
	p(NT(313), (NT(52)));
//G437: __E___E_cli_command_128_129(313) => _(11) '=' _(11).
	p(NT(313), (NT(11)+T(3)+NT(11)));
//G438: __E_cli_command_128(311) => set_sym(312) __(52) option(309) __E___E_cli_command_128_129(313) option_value(314).
	p(NT(311), (NT(312)+NT(52)+NT(309)+NT(313)+NT(314)));
//G439: set_cmd(310)         => __E_cli_command_128(311).
	p(NT(310), (NT(311)));
//G440: cli_command(213)     => set_cmd(310).
	p(NT(213), (NT(310)));
//G441: toggle_sym(317)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(317), (T(11)+T(8)+T(47)+T(47)+T(16)+T(10)));
//G442: __E_cli_command_130(316) => toggle_sym(317) __(52) bool_option(318).
	p(NT(316), (NT(317)+NT(52)+NT(318)));
//G443: toggle_cmd(315)      => __E_cli_command_130(316).
	p(NT(315), (NT(316)));
//G444: cli_command(213)     => toggle_cmd(315).
	p(NT(213), (NT(315)));
//G445: __E_cli_command_131(320) => type(170) __(52) in_var_name(143) _(11) '=' _(11) input_stream(321).
	p(NT(320), (NT(170)+NT(52)+NT(143)+NT(11)+T(3)+NT(11)+NT(321)));
//G446: def_input_cmd(319)   => __E_cli_command_131(320).
	p(NT(319), (NT(320)));
//G447: cli_command(213)     => def_input_cmd(319).
	p(NT(213), (NT(319)));
//G448: __E_cli_command_132(323) => type(170) __(52) out_var_name(148) _(11) '=' _(11) output_stream(324).
	p(NT(323), (NT(170)+NT(52)+NT(148)+NT(11)+T(3)+NT(11)+NT(324)));
//G449: def_output_cmd(322)  => __E_cli_command_132(323).
	p(NT(322), (NT(323)));
//G450: cli_command(213)     => def_output_cmd(322).
	p(NT(213), (NT(322)));
//G451: __E___E_cli_command_133_134(328) => 'h' 'i' 's' 't'.
	p(NT(328), (T(44)+T(12)+T(7)+T(11)));
//G452: __E___E_cli_command_133_134(328) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(328), (T(44)+T(12)+T(7)+T(11)+T(8)+T(50)+T(18)));
//G453: history_sym(327)     => __E___E_cli_command_133_134(328).
	p(NT(327), (NT(328)));
//G454: __E_cli_command_133(326) => history_sym(327).
	p(NT(326), (NT(327)));
//G455: history_list_cmd(325) => __E_cli_command_133(326).
	p(NT(325), (NT(326)));
//G456: cli_command(213)     => history_list_cmd(325).
	p(NT(213), (NT(325)));
//G457: __E_cli_command_135(330) => history_sym(327) __(52) memory(331).
	p(NT(330), (NT(327)+NT(52)+NT(331)));
//G458: history_print_cmd(329) => __E_cli_command_135(330).
	p(NT(329), (NT(330)));
//G459: cli_command(213)     => history_print_cmd(329).
	p(NT(213), (NT(329)));
//G460: __E_cli_command_136(333) => wff(14).
	p(NT(333), (NT(14)));
//G461: __E_cli_command_136(333) => bf(21).
	p(NT(333), (NT(21)));
//G462: history_store_cmd(332) => __E_cli_command_136(333).
	p(NT(332), (NT(333)));
//G463: cli_command(213)     => history_store_cmd(332).
	p(NT(213), (NT(332)));
//G464: __E_input_stream_137(334) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(335) _(11) ')'.
	p(NT(334), (T(12)+T(42)+T(12)+T(16)+T(10)+NT(11)+T(5)+NT(11)+NT(335)+NT(11)+T(6)));
//G465: input_stream(321)    => __E_input_stream_137(334).
	p(NT(321), (NT(334)));
//G466: keyboard_sym(336)    => 'k' 'e' 'y' 'b' 'o' 'a' 'r' 'd'.
	p(NT(336), (T(57)+T(10)+T(18)+T(49)+T(8)+T(15)+T(50)+T(55)));
//G467: input_stream(321)    => keyboard_sym(336).
	p(NT(321), (NT(336)));
//G468: __E_output_stream_138(337) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(335) _(11) ')'.
	p(NT(337), (T(8)+T(42)+T(12)+T(16)+T(10)+NT(11)+T(5)+NT(11)+NT(335)+NT(11)+T(6)));
//G469: output_stream(324)   => __E_output_stream_138(337).
	p(NT(324), (NT(337)));
//G470: console_sym(338)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(338), (T(45)+T(8)+T(46)+T(7)+T(8)+T(16)+T(10)));
//G471: output_stream(324)   => console_sym(338).
	p(NT(324), (NT(338)));
//G472: q_file_name(335)     => '"' file_name(339) '"'.
	p(NT(335), (T(58)+NT(339)+T(58)));
//G473: __E_file_name_139(340) => printable(8).
	p(NT(340), (NT(8)));
//G474: __E_file_name_139(340) => printable(8) __E_file_name_139(340).
	p(NT(340), (NT(8)+NT(340)));
//G475: file_name(339)       => __E_file_name_139(340).
	p(NT(339), (NT(340)));
//G476: number(299)          => digits(161).
	p(NT(299), (NT(161)));
//G477: bf_cmd_arg(341)      => memory(331).
	p(NT(341), (NT(331)));
//G478: bf_cmd_arg(341)      => bf(21).
	p(NT(341), (NT(21)));
//G479: wff_cmd_arg(304)     => memory(331).
	p(NT(304), (NT(331)));
//G480: wff_cmd_arg(304)     => wff(14).
	p(NT(304), (NT(14)));
//G481: solve_cmd_arg(249)   => memory(331).
	p(NT(249), (NT(331)));
//G482: solve_cmd_arg(249)   => wff(14).
	p(NT(249), (NT(14)));
//G483: nf_cmd_arg(262)      => memory(331).
	p(NT(262), (NT(331)));
//G484: nf_cmd_arg(262)      => wff(14).
	p(NT(262), (NT(14)));
//G485: nf_cmd_arg(262)      => bf(21).
	p(NT(262), (NT(21)));
//G486: onf_cmd_arg(292)     => memory(331).
	p(NT(292), (NT(331)));
//G487: onf_cmd_arg(292)     => wff(14).
	p(NT(292), (NT(14)));
//G488: normalize_cmd_arg(241) => memory(331).
	p(NT(241), (NT(331)));
//G489: normalize_cmd_arg(241) => rr(10).
	p(NT(241), (NT(10)));
//G490: normalize_cmd_arg(241) => ref(18).
	p(NT(241), (NT(18)));
//G491: normalize_cmd_arg(241) => wff(14).
	p(NT(241), (NT(14)));
//G492: normalize_cmd_arg(241) => bf(21).
	p(NT(241), (NT(21)));
//G493: inst_args(267)       => wff_cmd_arg(304) _(11) '[' _(11) variable(106) _(11) '/' _(11) bf_cmd_arg(341) _(11) ']'.
	p(NT(267), (NT(304)+NT(11)+T(19)+NT(11)+NT(106)+NT(11)+T(56)+NT(11)+NT(341)+NT(11)+T(20)));
//G494: inst_args(267)       => bf_cmd_arg(341) _(11) '[' _(11) variable(106) _(11) '/' _(11) bf_cmd_arg(341) _(11) ']'.
	p(NT(267), (NT(341)+NT(11)+T(19)+NT(11)+NT(106)+NT(11)+T(56)+NT(11)+NT(341)+NT(11)+T(20)));
//G495: help_arg(230)        => help_sym(227).
	p(NT(230), (NT(227)));
//G496: help_arg(230)        => version_sym(220).
	p(NT(230), (NT(220)));
//G497: help_arg(230)        => quit_sym(217).
	p(NT(230), (NT(217)));
//G498: help_arg(230)        => clear_sym(223).
	p(NT(230), (NT(223)));
//G499: help_arg(230)        => get_sym(307).
	p(NT(230), (NT(307)));
//G500: help_arg(230)        => set_sym(312).
	p(NT(230), (NT(312)));
//G501: help_arg(230)        => toggle_sym(317).
	p(NT(230), (NT(317)));
//G502: help_arg(230)        => file_sym(233).
	p(NT(230), (NT(233)));
//G503: help_arg(230)        => history_sym(327).
	p(NT(230), (NT(327)));
//G504: help_arg(230)        => abs_memory_sym(342).
	p(NT(230), (NT(342)));
//G505: help_arg(230)        => rel_memory_sym(343).
	p(NT(230), (NT(343)));
//G506: help_arg(230)        => selection_sym(344).
	p(NT(230), (NT(344)));
//G507: help_arg(230)        => def_sym(295).
	p(NT(230), (NT(295)));
//G508: help_arg(230)        => inst_sym(265).
	p(NT(230), (NT(265)));
//G509: help_arg(230)        => subst_sym(260).
	p(NT(230), (NT(260)));
//G510: help_arg(230)        => normalize_sym(256).
	p(NT(230), (NT(256)));
//G511: help_arg(230)        => execute_sym(345).
	p(NT(230), (NT(345)));
//G512: help_arg(230)        => solve_sym(247).
	p(NT(230), (NT(247)));
//G513: help_arg(230)        => valid_sym(237).
	p(NT(230), (NT(237)));
//G514: help_arg(230)        => sat_sym(240).
	p(NT(230), (NT(240)));
//G515: help_arg(230)        => unsat_sym(244).
	p(NT(230), (NT(244)));
//G516: help_arg(230)        => dnf_sym(270).
	p(NT(230), (NT(270)));
//G517: help_arg(230)        => cnf_sym(273).
	p(NT(230), (NT(273)));
//G518: help_arg(230)        => anf_sym(276).
	p(NT(230), (NT(276)));
//G519: help_arg(230)        => snf_sym(288).
	p(NT(230), (NT(288)));
//G520: help_arg(230)        => nnf_sym(279).
	p(NT(230), (NT(279)));
//G521: help_arg(230)        => pnf_sym(282).
	p(NT(230), (NT(282)));
//G522: help_arg(230)        => mnf_sym(285).
	p(NT(230), (NT(285)));
//G523: help_arg(230)        => onf_sym(291).
	p(NT(230), (NT(291)));
//G524: help_arg(230)        => qelim_sym(303).
	p(NT(230), (NT(303)));
//G525: __E___E_help_arg_140_141(348) => 's'.
	p(NT(348), (T(7)));
//G526: __E___E_help_arg_140_141(348) => null.
	p(NT(348), (nul));
//G527: __E_help_arg_140(347) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_140_141(348).
	p(NT(347), (T(10)+T(22)+T(15)+T(9)+T(54)+T(16)+T(10)+NT(348)));
//G528: examples_sym(346)    => __E_help_arg_140(347).
	p(NT(346), (NT(347)));
//G529: help_arg(230)        => examples_sym(346).
	p(NT(230), (NT(346)));
//G530: __E_memory_142(350)  => '%'.
	p(NT(350), (T(59)));
//G531: rel_memory_sym(343)  => '%' '-'.
	p(NT(343), (T(59)+T(23)));
//G532: memory_id(352)       => digits(161).
	p(NT(352), (NT(161)));
//G533: __E___E_memory_142_143(351) => memory_id(352).
	p(NT(351), (NT(352)));
//G534: __E___E_memory_142_143(351) => null.
	p(NT(351), (nul));
//G535: __E_memory_142(350)  => rel_memory_sym(343) __E___E_memory_142_143(351).
	p(NT(350), (NT(343)+NT(351)));
//G536: rel_memory(349)      => __E_memory_142(350).
	p(NT(349), (NT(350)));
//G537: memory(331)          => rel_memory(349).
	p(NT(331), (NT(349)));
//G538: abs_memory_sym(342)  => '%'.
	p(NT(342), (T(59)));
//G539: __E_memory_144(354)  => abs_memory_sym(342) memory_id(352).
	p(NT(354), (NT(342)+NT(352)));
//G540: abs_memory(353)      => __E_memory_144(354).
	p(NT(353), (NT(354)));
//G541: memory(331)          => abs_memory(353).
	p(NT(331), (NT(353)));
//G542: option(309)          => bool_option(318).
	p(NT(309), (NT(318)));
//G543: __E_option_145(357)  => 's' 'e' 'v'.
	p(NT(357), (T(7)+T(10)+T(51)));
//G544: __E_option_145(357)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(357), (T(7)+T(10)+T(51)+T(10)+T(50)+T(12)+T(11)+T(18)));
//G545: severity_opt(356)    => __E_option_145(357).
	p(NT(356), (NT(357)));
//G546: enum_option(355)     => severity_opt(356).
	p(NT(355), (NT(356)));
//G547: option(309)          => enum_option(355).
	p(NT(309), (NT(355)));
//G548: __E_bool_option_146(359) => 's'.
	p(NT(359), (T(7)));
//G549: __E_bool_option_146(359) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(359), (T(7)+T(11)+T(15)+T(11)+T(48)+T(7)));
//G550: status_opt(358)      => __E_bool_option_146(359).
	p(NT(358), (NT(359)));
//G551: bool_option(318)     => status_opt(358).
	p(NT(318), (NT(358)));
//G552: __E_bool_option_147(361) => 'c'.
	p(NT(361), (T(45)));
//G553: __E_bool_option_147(361) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(361), (T(45)+T(8)+T(16)+T(8)+T(50)));
//G554: __E_bool_option_147(361) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(361), (T(45)+T(8)+T(16)+T(8)+T(50)+T(7)));
//G555: colors_opt(360)      => __E_bool_option_147(361).
	p(NT(360), (NT(361)));
//G556: bool_option(318)     => colors_opt(360).
	p(NT(318), (NT(360)));
//G557: __E_bool_option_148(363) => 'd'.
	p(NT(363), (T(55)));
//G558: __E_bool_option_148(363) => 'd' 'b' 'g'.
	p(NT(363), (T(55)+T(49)+T(47)));
//G559: __E_bool_option_148(363) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(363), (T(55)+T(10)+T(49)+T(48)+T(47)));
//G560: debug_repl_opt(362)  => __E_bool_option_148(363).
	p(NT(362), (NT(363)));
//G561: bool_option(318)     => debug_repl_opt(362).
	p(NT(318), (NT(362)));
//G562: option_value(314)    => option_value_true(364).
	p(NT(314), (NT(364)));
//G563: option_value(314)    => option_value_false(365).
	p(NT(314), (NT(365)));
//G564: option_value(314)    => severity(366).
	p(NT(314), (NT(366)));
//G565: option_value_true(364) => 't'.
	p(NT(364), (T(11)));
//G566: option_value_true(364) => 't' 'r' 'u' 'e'.
	p(NT(364), (T(11)+T(50)+T(48)+T(10)));
//G567: option_value_true(364) => 'o' 'n'.
	p(NT(364), (T(8)+T(46)));
//G568: option_value_true(364) => '1'.
	p(NT(364), (T(33)));
//G569: option_value_true(364) => 'y'.
	p(NT(364), (T(18)));
//G570: option_value_true(364) => 'y' 'e' 's'.
	p(NT(364), (T(18)+T(10)+T(7)));
//G571: option_value_false(365) => 'f'.
	p(NT(365), (T(42)));
//G572: option_value_false(365) => 'f' 'a' 'l' 's' 'e'.
	p(NT(365), (T(42)+T(15)+T(16)+T(7)+T(10)));
//G573: option_value_false(365) => 'o' 'f' 'f'.
	p(NT(365), (T(8)+T(42)+T(42)));
//G574: option_value_false(365) => '0'.
	p(NT(365), (T(34)));
//G575: option_value_false(365) => 'n'.
	p(NT(365), (T(46)));
//G576: option_value_false(365) => 'n' 'o'.
	p(NT(365), (T(46)+T(8)));
//G577: __E_severity_149(368) => 'e'.
	p(NT(368), (T(10)));
//G578: __E_severity_149(368) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(368), (T(10)+T(50)+T(50)+T(8)+T(50)));
//G579: error_sym(367)       => __E_severity_149(368).
	p(NT(367), (NT(368)));
//G580: severity(366)        => error_sym(367).
	p(NT(366), (NT(367)));
//G581: __E_severity_150(370) => 'i'.
	p(NT(370), (T(12)));
//G582: __E_severity_150(370) => 'i' 'n' 'f' 'o'.
	p(NT(370), (T(12)+T(46)+T(42)+T(8)));
//G583: info_sym(369)        => __E_severity_150(370).
	p(NT(369), (NT(370)));
//G584: severity(366)        => info_sym(369).
	p(NT(366), (NT(369)));
//G585: __E_severity_151(372) => 'd'.
	p(NT(372), (T(55)));
//G586: __E_severity_151(372) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(372), (T(55)+T(10)+T(49)+T(48)+T(47)));
//G587: debug_sym(371)       => __E_severity_151(372).
	p(NT(371), (NT(372)));
//G588: severity(366)        => debug_sym(371).
	p(NT(366), (NT(371)));
//G589: __E_severity_152(374) => 't'.
	p(NT(374), (T(11)));
//G590: __E_severity_152(374) => 't' 'r' 'a' 'c' 'e'.
	p(NT(374), (T(11)+T(50)+T(15)+T(45)+T(10)));
//G591: trace_sym(373)       => __E_severity_152(374).
	p(NT(373), (NT(374)));
//G592: severity(366)        => trace_sym(373).
	p(NT(366), (NT(373)));
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
		__E___E_ref_args_4_7, library, rules, __E_library_8, __E___E_library_8_9, rule, __E___E___E_library_8_9_10, wff_rule, bf_rule, __E___E_library_8_11, 
		wff_matcher, wff_body, __E_wff_rule_12, wff_cb, bf_matcher, bf_body, __E_bf_rule_13, bf_cb, builder, builder_head, 
		builder_body, __E_builder_head_14, __, __E_builder_head_15, bf_builder_body, __E_builder_body_16, wff_builder_body, __E_builder_body_17, wff_parenthesis, __E_wff_18, 
		wff_sometimes, __E_wff_19, __E___E_wff_19_20, wff_always, __E_wff_21, __E___E_wff_21_22, wff_conditional, __E_wff_23, wff_all, __E_wff_24, 
		q_vars, wff_ex, __E_wff_25, wff_ref, constraint, wff_imply, __E_wff_26, wff_equiv, __E_wff_27, wff_or, 
		__E_wff_28, wff_xor, __E_wff_29, wff_and, __E_wff_30, wff_neg, __E_wff_31, wff_t, wff_f, bf_interval, 
		__E_wff_32, bf_neq, __E_wff_33, bf_eq, __E_wff_34, bf_nleq, __E_wff_35, bf_greater, __E_wff_36, bf_less_equal, 
		__E_wff_37, bf_less, __E_wff_38, bf_parenthesis, __E_bf_39, bf_constant, variable, bf_splitter, __E_bf_40, bf_ref, 
		bf_or, __E_bf_41, bf_xor, __E_bf_42, bf_and, __E_bf_43, __E___E_bf_43_44, bf_neg, __E_bf_45, bf_t, 
		bf_f, uninterpreted_constant, ctn_neq, __E_constraint_46, ctnvar, num, ctn_eq, __E_constraint_47, ctn_greater_equal, __E_constraint_48, 
		ctn_greater, __E_constraint_49, ctn_less_equal, __E_constraint_50, ctn_less, __E_constraint_51, charvar, __E_charvar_52, __E_charvar_53, io_var, 
		__E_variable_54, in, out, in_var_name, __E_in_55, __E___E_in_55_56, __E___E___E_in_55_56_57, offset, out_var_name, __E_out_58, 
		__E___E_out_58_59, __E___E___E_out_58_59_60, q_var, __E_q_vars_61, __E_q_vars_62, __E_offsets_63, __E_offsets_64, shift, __E_offset_65, __E_shift_66, 
		__E___E_shift_66_67, digits, uninter_const_name, __E_uninterpreted_constant_68, __E___E_uninterpreted_constant_68_69, chars, constant, __E_bf_constant_70, binding, __E_bf_constant_71, 
		type, source, __E_binding_72, src_c, __E___E_binding_72_73, __E___E___E_binding_72_73_74, __E___E___E_binding_72_73_75, __E_src_c_76, __E_src_c_77, __E_chars_78, 
		__E_digits_79, extra, comment, __E_comment_80, __E_comment_81, __E_comment_82, __E____83, __E___84, bf_cb_args1, bf_cb_arg, 
		bf_cb_args2, bf_cb_args3, bf_cb_args4, wff_cb_args1, wff_cb_arg, wff_cb_args2, wff_cb_args3, wff_cb_args4, wff_has_clashing_subformulas_cb, __E_wff_cb_85, 
		wff_has_subformula_cb, __E_wff_cb_86, wff_remove_existential_cb, __E_wff_cb_87, bf_has_subformula_cb, __E_bf_cb_88, bf_remove_funiversal_cb, __E_bf_cb_89, bf_remove_fexistential_cb, __E_bf_cb_90, 
		bf_normalize_cb, __E_bf_cb_91, cli, cli_command, __E_cli_92, __E_cli_93, quit_cmd, quit_sym, __E_cli_command_94, version_cmd, 
		version_sym, __E_cli_command_95, clear_cmd, clear_sym, __E_cli_command_96, help_cmd, __E_cli_command_97, help_sym, __E___E_cli_command_97_98, __E___E_cli_command_97_99, 
		help_arg, file_cmd, __E_cli_command_100, file_sym, q_string, valid_cmd, __E_cli_command_101, valid_sym, sat_cmd, __E_cli_command_102, 
		sat_sym, normalize_cmd_arg, unsat_cmd, __E_cli_command_103, unsat_sym, solve_cmd, __E_cli_command_104, solve_sym, __E___E_cli_command_104_105, solve_cmd_arg, 
		run_cmd, __E_cli_command_106, run_sym, __E___E_cli_command_106_107, normalize_cmd, __E_cli_command_108, normalize_sym, __E___E_cli_command_108_109, subst_cmd, __E_cli_command_110, 
		subst_sym, __E___E_cli_command_110_111, nf_cmd_arg, inst_cmd, __E_cli_command_112, inst_sym, __E___E_cli_command_112_113, inst_args, dnf_cmd, __E_cli_command_114, 
		dnf_sym, cnf_cmd, __E_cli_command_115, cnf_sym, anf_cmd, __E_cli_command_116, anf_sym, nnf_cmd, __E_cli_command_117, nnf_sym, 
		pnf_cmd, __E_cli_command_118, pnf_sym, mnf_cmd, __E_cli_command_119, mnf_sym, snf_cmd, __E_cli_command_120, snf_sym, onf_cmd, 
		__E_cli_command_121, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_122, def_sym, __E___E_cli_command_122_123, def_print_cmd, __E_cli_command_124, number, 
		def_rr_cmd, qelim_cmd, __E_cli_command_125, qelim_sym, wff_cmd_arg, get_cmd, __E_cli_command_126, get_sym, __E___E_cli_command_126_127, option, 
		set_cmd, __E_cli_command_128, set_sym, __E___E_cli_command_128_129, option_value, toggle_cmd, __E_cli_command_130, toggle_sym, bool_option, def_input_cmd, 
		__E_cli_command_131, input_stream, def_output_cmd, __E_cli_command_132, output_stream, history_list_cmd, __E_cli_command_133, history_sym, __E___E_cli_command_133_134, history_print_cmd, 
		__E_cli_command_135, memory, history_store_cmd, __E_cli_command_136, __E_input_stream_137, q_file_name, keyboard_sym, __E_output_stream_138, console_sym, file_name, 
		__E_file_name_139, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, execute_sym, examples_sym, __E_help_arg_140, __E___E_help_arg_140_141, rel_memory, 
		__E_memory_142, __E___E_memory_142_143, memory_id, abs_memory, __E_memory_144, enum_option, severity_opt, __E_option_145, status_opt, __E_bool_option_146, 
		colors_opt, __E_bool_option_147, debug_repl_opt, __E_bool_option_148, option_value_true, option_value_false, severity, error_sym, __E_severity_149, info_sym, 
		__E_severity_150, debug_sym, __E_severity_151, trace_sym, __E_severity_152, named_binding, __N_0, __N_1, __N_2, __N_3, 
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
