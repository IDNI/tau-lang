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
	"bf_f", "uninterpreted_constant", "bf_and_nosep_1st_op", "__E_bf_and_46", "ctn_neq", "__E_constraint_47", "ctnvar", "num", "ctn_eq", "__E_constraint_48", 
	"ctn_greater_equal", "__E_constraint_49", "ctn_greater", "__E_constraint_50", "ctn_less_equal", "__E_constraint_51", "ctn_less", "__E_constraint_52", "__E_variable_53", "chars", 
	"io_var", "__E_variable_54", "in", "out", "in_var_name", "__E_in_55", "__E___E_in_55_56", "offset", "out_var_name", "__E_out_57", 
	"__E___E_out_57_58", "q_var", "__E_q_vars_59", "__E_q_vars_60", "__E_offsets_61", "__E_offsets_62", "shift", "__E_offset_63", "__E_shift_64", "__E___E_shift_64_65", 
	"digits", "uninter_const_name", "__E_uninterpreted_constant_66", "__E___E_uninterpreted_constant_66_67", "constant", "__E_bf_constant_68", "binding", "__E_bf_constant_69", "type", "source", 
	"__E_binding_70", "src_c", "__E___E_binding_70_71", "__E___E___E_binding_70_71_72", "__E___E___E_binding_70_71_73", "__E_src_c_74", "__E_src_c_75", "__E_chars_76", "__E_chars_77", "__E_digits_78", 
	"extra", "comment", "__E_comment_79", "__E_comment_80", "__E_comment_81", "__E____82", "__E___83", "bf_cb_args1", "bf_cb_arg", "bf_cb_args2", 
	"bf_cb_args3", "bf_cb_args4", "wff_cb_args1", "wff_cb_arg", "wff_cb_args2", "wff_cb_args3", "wff_cb_args4", "wff_has_clashing_subformulas_cb", "__E_wff_cb_84", "wff_has_subformula_cb", 
	"__E_wff_cb_85", "wff_remove_existential_cb", "__E_wff_cb_86", "bf_has_subformula_cb", "__E_bf_cb_87", "bf_remove_funiversal_cb", "__E_bf_cb_88", "bf_remove_fexistential_cb", "__E_bf_cb_89", "bf_normalize_cb", 
	"__E_bf_cb_90", "cli", "cli_command", "__E_cli_91", "__E_cli_92", "quit_cmd", "quit_sym", "__E_cli_command_93", "version_cmd", "version_sym", 
	"__E_cli_command_94", "clear_cmd", "clear_sym", "__E_cli_command_95", "help_cmd", "__E_cli_command_96", "help_sym", "__E___E_cli_command_96_97", "__E___E_cli_command_96_98", "help_arg", 
	"file_cmd", "__E_cli_command_99", "file_sym", "q_string", "valid_cmd", "__E_cli_command_100", "valid_sym", "sat_cmd", "__E_cli_command_101", "sat_sym", 
	"normalize_cmd_arg", "unsat_cmd", "__E_cli_command_102", "unsat_sym", "solve_cmd", "__E_cli_command_103", "solve_sym", "__E___E_cli_command_103_104", "solve_cmd_arg", "run_cmd", 
	"__E_cli_command_105", "run_sym", "__E___E_cli_command_105_106", "__E___E_cli_command_105_107", "normalize_cmd", "__E_cli_command_108", "normalize_sym", "__E___E_cli_command_108_109", "subst_cmd", "__E_cli_command_110", 
	"subst_sym", "__E___E_cli_command_110_111", "nf_cmd_arg", "inst_cmd", "__E_cli_command_112", "inst_sym", "__E___E_cli_command_112_113", "inst_args", "dnf_cmd", "__E_cli_command_114", 
	"dnf_sym", "cnf_cmd", "__E_cli_command_115", "cnf_sym", "anf_cmd", "__E_cli_command_116", "anf_sym", "nnf_cmd", "__E_cli_command_117", "nnf_sym", 
	"pnf_cmd", "__E_cli_command_118", "pnf_sym", "mnf_cmd", "__E_cli_command_119", "mnf_sym", "snf_cmd", "__E_cli_command_120", "snf_sym", "onf_cmd", 
	"__E_cli_command_121", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_122", "def_sym", "__E___E_cli_command_122_123", "def_print_cmd", "__E_cli_command_124", "number", 
	"def_rr_cmd", "qelim_cmd", "__E_cli_command_125", "qelim_sym", "wff_cmd_arg", "get_cmd", "__E_cli_command_126", "get_sym", "__E___E_cli_command_126_127", "option", 
	"set_cmd", "__E_cli_command_128", "set_sym", "__E___E_cli_command_128_129", "option_value", "toggle_cmd", "__E_cli_command_130", "toggle_sym", "bool_option", "def_input_cmd", 
	"__E_cli_command_131", "input_stream", "def_output_cmd", "__E_cli_command_132", "output_stream", "history_list_cmd", "__E_cli_command_133", "history_sym", "__E___E_cli_command_133_134", "history_print_cmd", 
	"__E_cli_command_135", "memory", "history_store_cmd", "__E_cli_command_136", "__E_input_stream_137", "q_file_name", "console_sym", "__E_output_stream_138", "file_name", "__E_file_name_139", 
	"bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "execute_sym", "examples_sym", "__E_help_arg_140", "__E___E_help_arg_140_141", "rel_memory", "__E_memory_142", 
	"__E___E_memory_142_143", "memory_id", "abs_memory", "__E_memory_144", "enum_option", "severity_opt", "__E_option_145", "status_opt", "__E_bool_option_146", "colors_opt", 
	"__E_bool_option_147", "charvar_opt", "__E_bool_option_148", "hilighting_opt", "__E_bool_option_149", "indenting_opt", "__E_bool_option_150", "debug_repl_opt", "__E_bool_option_151", "option_value_true", 
	"option_value_false", "severity", "error_sym", "__E_severity_152", "info_sym", "__E_severity_153", "debug_sym", "__E_severity_154", "trace_sym", "__E_severity_155", 
	"named_binding", "variable_not_io", "__N_0", "__N_1", "__N_2", "__N_3", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', ',', '(', ')', 's', 'o', 
	'm', 'e', 't', 'i', '<', '>', 'a', 'l', 'w', 'y', 
	'[', ']', '?', 'x', '-', '|', '^', '&', '!', 'T', 
	'F', 'S', '+', '\'', '1', '0', '$', '{', '}', '_', 
	'\t', '\n', '\r', '#', 'f', 'h', 'c', 'n', 'g', 'u', 
	'b', 'r', 'v', 'z', 'q', 'p', 'd', '/', '"', '%', 
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
			20, 22, 106, 144, 148, 160, 161, 168, 169, 338,
			380
		},
		.to_inline = {
			{ 14, 58, 14 },
			{ 21, 103, 21 },
			{ 43 },
			{ 47 },
			{ 139 },
			{ 151 },
			{ 171 },
			{ 187 },
			{ 189 },
			{ 190 },
			{ 191 },
			{ 192 },
			{ 194 },
			{ 195 },
			{ 196 },
			{ 240 },
			{ 248 },
			{ 262 },
			{ 267 },
			{ 292 },
			{ 304 },
			{ 340 },
			{ 381 }
		},
		.inline_char_classes = true
	},
	.enabled_guards = {
		"charvar"
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
//G139: __E___E_bf_43_44(116) => __(52).
	p(NT(116), (NT(52)));
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
//G152: __E_bf_and_46(123)   => bf_parenthesis(103).
	p(NT(123), (NT(103)));
//G153: __E_bf_and_46(123)   => variable(106).
	p(NT(123), (NT(106)));
//G154: __E_bf_and_46(123)   => bf_splitter(107).
	p(NT(123), (NT(107)));
//G155: __E_bf_and_46(123)   => bf_ref(109).
	p(NT(123), (NT(109)));
//G156: __E_bf_and_46(123)   => bf_or(110).
	p(NT(123), (NT(110)));
//G157: __E_bf_and_46(123)   => bf_xor(112).
	p(NT(123), (NT(112)));
//G158: __E_bf_and_46(123)   => bf_neg(117).
	p(NT(123), (NT(117)));
//G159: __E_bf_and_46(123)   => capture(20).
	p(NT(123), (NT(20)));
//G160: __E_bf_and_46(123)   => uninterpreted_constant(121).
	p(NT(123), (NT(121)));
//G161: bf_and_nosep_1st_op(122) => __E_bf_and_46(123).
	p(NT(122), (NT(123)));
//G162: bf_and(114)          => bf_and_nosep_1st_op(122) bf(21).
	p(NT(114), (NT(122)+NT(21)));
//G163: __E_constraint_47(125) => '[' ctnvar(126) _(11) '!' '=' _(11) num(127) ']'.
	p(NT(125), (T(19)+NT(126)+NT(11)+T(27)+T(3)+NT(11)+NT(127)+T(20)));
//G164: __E_constraint_47(125) => '[' num(127) _(11) '!' '=' _(11) ctnvar(126) ']'.
	p(NT(125), (T(19)+NT(127)+NT(11)+T(27)+T(3)+NT(11)+NT(126)+T(20)));
//G165: ctn_neq(124)         => __E_constraint_47(125).
	p(NT(124), (NT(125)));
//G166: constraint(74)       => ctn_neq(124).
	p(NT(74), (NT(124)));
//G167: __E_constraint_48(129) => '[' ctnvar(126) _(11) '=' _(11) num(127) ']'.
	p(NT(129), (T(19)+NT(126)+NT(11)+T(3)+NT(11)+NT(127)+T(20)));
//G168: __E_constraint_48(129) => '[' num(127) _(11) '=' _(11) ctnvar(126) ']'.
	p(NT(129), (T(19)+NT(127)+NT(11)+T(3)+NT(11)+NT(126)+T(20)));
//G169: ctn_eq(128)          => __E_constraint_48(129).
	p(NT(128), (NT(129)));
//G170: constraint(74)       => ctn_eq(128).
	p(NT(74), (NT(128)));
//G171: __E_constraint_49(131) => '[' ctnvar(126) _(11) '>' '=' _(11) num(127) ']'.
	p(NT(131), (T(19)+NT(126)+NT(11)+T(14)+T(3)+NT(11)+NT(127)+T(20)));
//G172: __E_constraint_49(131) => '[' num(127) _(11) '>' '=' _(11) ctnvar(126) ']'.
	p(NT(131), (T(19)+NT(127)+NT(11)+T(14)+T(3)+NT(11)+NT(126)+T(20)));
//G173: ctn_greater_equal(130) => __E_constraint_49(131).
	p(NT(130), (NT(131)));
//G174: constraint(74)       => ctn_greater_equal(130).
	p(NT(74), (NT(130)));
//G175: __E_constraint_50(133) => '[' ctnvar(126) _(11) '>' _(11) num(127) ']'.
	p(NT(133), (T(19)+NT(126)+NT(11)+T(14)+NT(11)+NT(127)+T(20)));
//G176: __E_constraint_50(133) => '[' num(127) _(11) '>' _(11) ctnvar(126) ']'.
	p(NT(133), (T(19)+NT(127)+NT(11)+T(14)+NT(11)+NT(126)+T(20)));
//G177: ctn_greater(132)     => __E_constraint_50(133).
	p(NT(132), (NT(133)));
//G178: constraint(74)       => ctn_greater(132).
	p(NT(74), (NT(132)));
//G179: __E_constraint_51(135) => '[' ctnvar(126) _(11) '<' '=' _(11) num(127) ']'.
	p(NT(135), (T(19)+NT(126)+NT(11)+T(13)+T(3)+NT(11)+NT(127)+T(20)));
//G180: __E_constraint_51(135) => '[' num(127) _(11) '<' '=' _(11) ctnvar(126) ']'.
	p(NT(135), (T(19)+NT(127)+NT(11)+T(13)+T(3)+NT(11)+NT(126)+T(20)));
//G181: ctn_less_equal(134)  => __E_constraint_51(135).
	p(NT(134), (NT(135)));
//G182: constraint(74)       => ctn_less_equal(134).
	p(NT(74), (NT(134)));
//G183: __E_constraint_52(137) => '[' ctnvar(126) _(11) '<' _(11) num(127) ']'.
	p(NT(137), (T(19)+NT(126)+NT(11)+T(13)+NT(11)+NT(127)+T(20)));
//G184: __E_constraint_52(137) => '[' num(127) _(11) '<' _(11) ctnvar(126) ']'.
	p(NT(137), (T(19)+NT(127)+NT(11)+T(13)+NT(11)+NT(126)+T(20)));
//G185: ctn_less(136)        => __E_constraint_52(137).
	p(NT(136), (NT(137)));
//G186: constraint(74)       => ctn_less(136).
	p(NT(74), (NT(136)));
//G187: __E_variable_53(138) => null.
	p(NT(138), (nul));
//G188: __E_variable_53(138) => digit(3) __E_variable_53(138).
	p(NT(138), (NT(3)+NT(138)));
//G189: variable(106)        => alpha(5) __E_variable_53(138).	 # guarded: charvar
	p(NT(106), (NT(5)+NT(138)));
	p.back().guard = "charvar";
//G190: variable(106)        => chars(139).	 # guarded: var
	p(NT(106), (NT(139)));
	p.back().guard = "var";
//G191: __E_variable_54(141) => in(142).
	p(NT(141), (NT(142)));
//G192: __E_variable_54(141) => out(143).
	p(NT(141), (NT(143)));
//G193: io_var(140)          => __E_variable_54(141).
	p(NT(140), (NT(141)));
//G194: variable(106)        => io_var(140).
	p(NT(106), (NT(140)));
//G195: __E___E_in_55_56(146) => digit(3).
	p(NT(146), (NT(3)));
//G196: __E___E_in_55_56(146) => digit(3) __E___E_in_55_56(146).
	p(NT(146), (NT(3)+NT(146)));
//G197: __E_in_55(145)       => 'i' __E___E_in_55_56(146).
	p(NT(145), (T(12)+NT(146)));
//G198: in_var_name(144)     => __E_in_55(145).
	p(NT(144), (NT(145)));
//G199: in(142)              => in_var_name(144) '[' offset(147) ']'.
	p(NT(142), (NT(144)+T(19)+NT(147)+T(20)));
//G200: __E___E_out_57_58(150) => digit(3).
	p(NT(150), (NT(3)));
//G201: __E___E_out_57_58(150) => digit(3) __E___E_out_57_58(150).
	p(NT(150), (NT(3)+NT(150)));
//G202: __E_out_57(149)      => 'o' __E___E_out_57_58(150).
	p(NT(149), (T(8)+NT(150)));
//G203: out_var_name(148)    => __E_out_57(149).
	p(NT(148), (NT(149)));
//G204: out(143)             => out_var_name(148) '[' offset(147) ']'.
	p(NT(143), (NT(148)+T(19)+NT(147)+T(20)));
//G205: ctnvar(126)          => chars(139).
	p(NT(126), (NT(139)));
//G206: capture(20)          => '$' chars(139).
	p(NT(20), (T(35)+NT(139)));
//G207: __E_q_vars_59(152)   => _(11) ',' _(11) q_var(151).
	p(NT(152), (NT(11)+T(4)+NT(11)+NT(151)));
//G208: __E_q_vars_60(153)   => null.
	p(NT(153), (nul));
//G209: __E_q_vars_60(153)   => __E_q_vars_59(152) __E_q_vars_60(153).
	p(NT(153), (NT(152)+NT(153)));
//G210: q_vars(70)           => q_var(151) __E_q_vars_60(153).
	p(NT(70), (NT(151)+NT(153)));
//G211: q_var(151)           => capture(20).
	p(NT(151), (NT(20)));
//G212: q_var(151)           => variable(106).
	p(NT(151), (NT(106)));
//G213: __E_offsets_61(154)  => _(11) ',' _(11) offset(147).
	p(NT(154), (NT(11)+T(4)+NT(11)+NT(147)));
//G214: __E_offsets_62(155)  => null.
	p(NT(155), (nul));
//G215: __E_offsets_62(155)  => __E_offsets_61(154) __E_offsets_62(155).
	p(NT(155), (NT(154)+NT(155)));
//G216: offsets(24)          => '[' _(11) offset(147) __E_offsets_62(155) _(11) ']'.
	p(NT(24), (T(19)+NT(11)+NT(147)+NT(155)+NT(11)+T(20)));
//G217: offset(147)          => num(127).
	p(NT(147), (NT(127)));
//G218: offset(147)          => capture(20).
	p(NT(147), (NT(20)));
//G219: offset(147)          => shift(156).
	p(NT(147), (NT(156)));
//G220: __N_0(382)           => io_var(140).
	p(NT(382), (NT(140)));
//G221: __E_offset_63(157)   => variable(106) & ~( __N_0(382) ).	 # conjunctive
	p(NT(157), (NT(106)) & ~(NT(382)));
//G222: offset(147)          => __E_offset_63(157).
	p(NT(147), (NT(157)));
//G223: __E_shift_64(158)    => capture(20).
	p(NT(158), (NT(20)));
//G224: __N_1(383)           => io_var(140).
	p(NT(383), (NT(140)));
//G225: __E___E_shift_64_65(159) => variable(106) & ~( __N_1(383) ).	 # conjunctive
	p(NT(159), (NT(106)) & ~(NT(383)));
//G226: __E_shift_64(158)    => __E___E_shift_64_65(159).
	p(NT(158), (NT(159)));
//G227: shift(156)           => __E_shift_64(158) _(11) '-' _(11) num(127).
	p(NT(156), (NT(158)+NT(11)+T(23)+NT(11)+NT(127)));
//G228: num(127)             => digits(160).
	p(NT(127), (NT(160)));
//G229: __E___E_uninterpreted_constant_66_67(163) => chars(139).
	p(NT(163), (NT(139)));
//G230: __E___E_uninterpreted_constant_66_67(163) => _(11).
	p(NT(163), (NT(11)));
//G231: __E_uninterpreted_constant_66(162) => __E___E_uninterpreted_constant_66_67(163) ':' chars(139).
	p(NT(162), (NT(163)+T(2)+NT(139)));
//G232: uninter_const_name(161) => __E_uninterpreted_constant_66(162).
	p(NT(161), (NT(162)));
//G233: uninterpreted_constant(121) => '<' uninter_const_name(161) _(11) '>'.
	p(NT(121), (T(13)+NT(161)+NT(11)+T(14)));
//G234: __E_bf_constant_68(165) => capture(20).
	p(NT(165), (NT(20)));
//G235: __E_bf_constant_68(165) => binding(166).
	p(NT(165), (NT(166)));
//G236: constant(164)        => __E_bf_constant_68(165).
	p(NT(164), (NT(165)));
//G237: type(168)            => chars(139).
	p(NT(168), (NT(139)));
//G238: __E_bf_constant_69(167) => _(11) ':' _(11) type(168).
	p(NT(167), (NT(11)+T(2)+NT(11)+NT(168)));
//G239: __E_bf_constant_69(167) => null.
	p(NT(167), (nul));
//G240: bf_constant(105)     => '{' _(11) constant(164) _(11) '}' __E_bf_constant_69(167).
	p(NT(105), (T(36)+NT(11)+NT(164)+NT(11)+T(37)+NT(167)));
//G241: __E___E___E_binding_70_71_72(173) => src_c(171).
	p(NT(173), (NT(171)));
//G242: __E___E___E_binding_70_71_72(173) => space(2).
	p(NT(173), (NT(2)));
//G243: __E___E___E_binding_70_71_73(174) => null.
	p(NT(174), (nul));
//G244: __E___E___E_binding_70_71_73(174) => __E___E___E_binding_70_71_72(173) __E___E___E_binding_70_71_73(174).
	p(NT(174), (NT(173)+NT(174)));
//G245: __E___E_binding_70_71(172) => __E___E___E_binding_70_71_73(174) src_c(171).
	p(NT(172), (NT(174)+NT(171)));
//G246: __E___E_binding_70_71(172) => null.
	p(NT(172), (nul));
//G247: __E_binding_70(170)  => src_c(171) __E___E_binding_70_71(172).
	p(NT(170), (NT(171)+NT(172)));
//G248: source(169)          => __E_binding_70(170).
	p(NT(169), (NT(170)));
//G249: binding(166)         => source(169).
	p(NT(166), (NT(169)));
//G250: src_c(171)           => alnum(6).
	p(NT(171), (NT(6)));
//G251: __N_2(384)           => '{'.
	p(NT(384), (T(36)));
//G252: __N_3(385)           => '}'.
	p(NT(385), (T(37)));
//G253: src_c(171)           => ~( __N_2(384) ) & ~( __N_3(385) ) & punct(7).	 # conjunctive
	p(NT(171), ~(NT(384)) & ~(NT(385)) & (NT(7)));
//G254: __E_src_c_74(175)    => src_c(171).
	p(NT(175), (NT(171)));
//G255: __E_src_c_74(175)    => space(2).
	p(NT(175), (NT(2)));
//G256: __E_src_c_75(176)    => null.
	p(NT(176), (nul));
//G257: __E_src_c_75(176)    => __E_src_c_74(175) __E_src_c_75(176).
	p(NT(176), (NT(175)+NT(176)));
//G258: src_c(171)           => '{' __E_src_c_75(176) '}'.
	p(NT(171), (T(36)+NT(176)+T(37)));
//G259: __E_chars_76(177)    => alnum(6).
	p(NT(177), (NT(6)));
//G260: __E_chars_76(177)    => '_'.
	p(NT(177), (T(38)));
//G261: __E_chars_77(178)    => null.
	p(NT(178), (nul));
//G262: __E_chars_77(178)    => __E_chars_76(177) __E_chars_77(178).
	p(NT(178), (NT(177)+NT(178)));
//G263: chars(139)           => alpha(5) __E_chars_77(178).
	p(NT(139), (NT(5)+NT(178)));
//G264: __E_digits_78(179)   => digit(3).
	p(NT(179), (NT(3)));
//G265: __E_digits_78(179)   => digit(3) __E_digits_78(179).
	p(NT(179), (NT(3)+NT(179)));
//G266: digits(160)          => __E_digits_78(179).
	p(NT(160), (NT(179)));
//G267: sym(22)              => chars(139).
	p(NT(22), (NT(139)));
//G268: extra(180)           => chars(139).
	p(NT(180), (NT(139)));
//G269: __E_comment_79(182)  => printable(8).
	p(NT(182), (NT(8)));
//G270: __E_comment_79(182)  => '\t'.
	p(NT(182), (T(39)));
//G271: __E_comment_80(183)  => null.
	p(NT(183), (nul));
//G272: __E_comment_80(183)  => __E_comment_79(182) __E_comment_80(183).
	p(NT(183), (NT(182)+NT(183)));
//G273: __E_comment_81(184)  => '\n'.
	p(NT(184), (T(40)));
//G274: __E_comment_81(184)  => '\r'.
	p(NT(184), (T(41)));
//G275: __E_comment_81(184)  => eof(1).
	p(NT(184), (NT(1)));
//G276: comment(181)         => '#' __E_comment_80(183) __E_comment_81(184).
	p(NT(181), (T(42)+NT(183)+NT(184)));
//G277: __E____82(185)       => space(2).
	p(NT(185), (NT(2)));
//G278: __E____82(185)       => comment(181).
	p(NT(185), (NT(181)));
//G279: __(52)               => __E____82(185) _(11).
	p(NT(52), (NT(185)+NT(11)));
//G280: __E___83(186)        => __(52).
	p(NT(186), (NT(52)));
//G281: __E___83(186)        => null.
	p(NT(186), (nul));
//G282: _(11)                => __E___83(186).
	p(NT(11), (NT(186)));
//G283: bf_cb_arg(188)       => bf(21).
	p(NT(188), (NT(21)));
//G284: bf_cb_args1(187)     => __(52) bf_cb_arg(188).
	p(NT(187), (NT(52)+NT(188)));
//G285: bf_cb_args2(189)     => bf_cb_args1(187) bf_cb_args1(187).
	p(NT(189), (NT(187)+NT(187)));
//G286: bf_cb_args3(190)     => bf_cb_args2(189) bf_cb_args1(187).
	p(NT(190), (NT(189)+NT(187)));
//G287: bf_cb_args4(191)     => bf_cb_args3(190) bf_cb_args1(187).
	p(NT(191), (NT(190)+NT(187)));
//G288: wff_cb_arg(193)      => wff(14).
	p(NT(193), (NT(14)));
//G289: wff_cb_args1(192)    => __(52) wff_cb_arg(193).
	p(NT(192), (NT(52)+NT(193)));
//G290: wff_cb_args2(194)    => wff_cb_args1(192) wff_cb_args1(192).
	p(NT(194), (NT(192)+NT(192)));
//G291: wff_cb_args3(195)    => wff_cb_args2(194) wff_cb_args1(192).
	p(NT(195), (NT(194)+NT(192)));
//G292: wff_cb_args4(196)    => wff_cb_args3(195) wff_cb_args1(192).
	p(NT(196), (NT(195)+NT(192)));
//G293: __E_wff_cb_84(198)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(194).
	p(NT(198), (T(17)+T(43)+T(43)+T(38)+T(44)+T(15)+T(7)+T(38)+T(45)+T(16)+T(15)+T(7)+T(44)+T(12)+T(46)+T(47)+T(38)+T(7)+T(48)+T(49)+T(43)+T(8)+T(50)+T(9)+T(48)+T(16)+T(15)+T(7)+T(38)+T(45)+T(49)+NT(194)));
//G294: wff_has_clashing_subformulas_cb(197) => __E_wff_cb_84(198).
	p(NT(197), (NT(198)));
//G295: wff_cb(43)           => wff_has_clashing_subformulas_cb(197).
	p(NT(43), (NT(197)));
//G296: __E_wff_cb_85(200)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(195).
	p(NT(200), (T(17)+T(43)+T(43)+T(38)+T(44)+T(15)+T(7)+T(38)+T(7)+T(48)+T(49)+T(43)+T(8)+T(50)+T(9)+T(48)+T(16)+T(15)+T(38)+T(45)+T(49)+NT(195)));
//G297: wff_has_subformula_cb(199) => __E_wff_cb_85(200).
	p(NT(199), (NT(200)));
//G298: wff_cb(43)           => wff_has_subformula_cb(199).
	p(NT(43), (NT(199)));
//G299: __E_wff_cb_86(202)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(194).
	p(NT(202), (T(17)+T(43)+T(43)+T(38)+T(50)+T(10)+T(9)+T(8)+T(51)+T(10)+T(38)+T(10)+T(22)+T(12)+T(7)+T(11)+T(10)+T(46)+T(11)+T(12)+T(15)+T(16)+T(38)+T(45)+T(49)+NT(194)));
//G300: wff_remove_existential_cb(201) => __E_wff_cb_86(202).
	p(NT(201), (NT(202)));
//G301: wff_cb(43)           => wff_remove_existential_cb(201).
	p(NT(43), (NT(201)));
//G302: __E_bf_cb_87(204)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(190).
	p(NT(204), (T(49)+T(43)+T(38)+T(44)+T(15)+T(7)+T(38)+T(7)+T(48)+T(49)+T(43)+T(8)+T(50)+T(9)+T(48)+T(16)+T(15)+T(38)+T(45)+T(49)+NT(190)));
//G303: bf_has_subformula_cb(203) => __E_bf_cb_87(204).
	p(NT(203), (NT(204)));
//G304: bf_cb(47)            => bf_has_subformula_cb(203).
	p(NT(47), (NT(203)));
//G305: __E_bf_cb_88(206)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(191).
	p(NT(206), (T(49)+T(43)+T(38)+T(50)+T(10)+T(9)+T(8)+T(51)+T(10)+T(38)+T(43)+T(48)+T(46)+T(12)+T(51)+T(10)+T(50)+T(7)+T(15)+T(16)+T(38)+T(45)+T(49)+NT(191)));
//G306: bf_remove_funiversal_cb(205) => __E_bf_cb_88(206).
	p(NT(205), (NT(206)));
//G307: bf_cb(47)            => bf_remove_funiversal_cb(205).
	p(NT(47), (NT(205)));
//G308: __E_bf_cb_89(208)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(191).
	p(NT(208), (T(49)+T(43)+T(38)+T(50)+T(10)+T(9)+T(8)+T(51)+T(10)+T(38)+T(43)+T(10)+T(22)+T(12)+T(7)+T(11)+T(10)+T(46)+T(11)+T(12)+T(15)+T(16)+T(38)+T(45)+T(49)+NT(191)));
//G309: bf_remove_fexistential_cb(207) => __E_bf_cb_89(208).
	p(NT(207), (NT(208)));
//G310: bf_cb(47)            => bf_remove_fexistential_cb(207).
	p(NT(47), (NT(207)));
//G311: __E_bf_cb_90(210)    => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(187).
	p(NT(210), (T(49)+T(43)+T(38)+T(46)+T(8)+T(50)+T(9)+T(15)+T(16)+T(12)+T(52)+T(10)+T(38)+T(45)+T(49)+NT(187)));
//G312: bf_normalize_cb(209) => __E_bf_cb_90(210).
	p(NT(209), (NT(210)));
//G313: bf_cb(47)            => bf_normalize_cb(209).
	p(NT(47), (NT(209)));
//G314: cli(211)             => _(11).
	p(NT(211), (NT(11)));
//G315: __E_cli_91(213)      => '.' _(11) cli_command(212) _(11).
	p(NT(213), (T(1)+NT(11)+NT(212)+NT(11)));
//G316: __E_cli_92(214)      => null.
	p(NT(214), (nul));
//G317: __E_cli_92(214)      => __E_cli_91(213) __E_cli_92(214).
	p(NT(214), (NT(213)+NT(214)));
//G318: cli(211)             => _(11) cli_command(212) _(11) __E_cli_92(214).
	p(NT(211), (NT(11)+NT(212)+NT(11)+NT(214)));
//G319: __E_cli_command_93(217) => 'q'.
	p(NT(217), (T(53)));
//G320: __E_cli_command_93(217) => 'q' 'u' 'i' 't'.
	p(NT(217), (T(53)+T(48)+T(12)+T(11)));
//G321: quit_sym(216)        => __E_cli_command_93(217).
	p(NT(216), (NT(217)));
//G322: quit_cmd(215)        => quit_sym(216).
	p(NT(215), (NT(216)));
//G323: cli_command(212)     => quit_cmd(215).
	p(NT(212), (NT(215)));
//G324: __E_cli_command_94(220) => 'v'.
	p(NT(220), (T(51)));
//G325: __E_cli_command_94(220) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(220), (T(51)+T(10)+T(50)+T(7)+T(12)+T(8)+T(46)));
//G326: version_sym(219)     => __E_cli_command_94(220).
	p(NT(219), (NT(220)));
//G327: version_cmd(218)     => version_sym(219).
	p(NT(218), (NT(219)));
//G328: cli_command(212)     => version_cmd(218).
	p(NT(212), (NT(218)));
//G329: __E_cli_command_95(223) => 'c'.
	p(NT(223), (T(45)));
//G330: __E_cli_command_95(223) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(223), (T(45)+T(16)+T(10)+T(15)+T(50)));
//G331: clear_sym(222)       => __E_cli_command_95(223).
	p(NT(222), (NT(223)));
//G332: clear_cmd(221)       => clear_sym(222).
	p(NT(221), (NT(222)));
//G333: cli_command(212)     => clear_cmd(221).
	p(NT(212), (NT(221)));
//G334: __E___E_cli_command_96_97(227) => 'h'.
	p(NT(227), (T(44)));
//G335: __E___E_cli_command_96_97(227) => 'h' 'e' 'l' 'p'.
	p(NT(227), (T(44)+T(10)+T(16)+T(54)));
//G336: help_sym(226)        => __E___E_cli_command_96_97(227).
	p(NT(226), (NT(227)));
//G337: __E___E_cli_command_96_98(228) => __(52) help_arg(229).
	p(NT(228), (NT(52)+NT(229)));
//G338: __E___E_cli_command_96_98(228) => null.
	p(NT(228), (nul));
//G339: __E_cli_command_96(225) => help_sym(226) __E___E_cli_command_96_98(228).
	p(NT(225), (NT(226)+NT(228)));
//G340: help_cmd(224)        => __E_cli_command_96(225).
	p(NT(224), (NT(225)));
//G341: cli_command(212)     => help_cmd(224).
	p(NT(212), (NT(224)));
//G342: file_sym(232)        => 'f' 'i' 'l' 'e'.
	p(NT(232), (T(43)+T(12)+T(16)+T(10)));
//G343: __E_cli_command_99(231) => file_sym(232) __(52) q_string(233).
	p(NT(231), (NT(232)+NT(52)+NT(233)));
//G344: file_cmd(230)        => __E_cli_command_99(231).
	p(NT(230), (NT(231)));
//G345: cli_command(212)     => file_cmd(230).
	p(NT(212), (NT(230)));
//G346: valid_sym(236)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(236), (T(51)+T(15)+T(16)+T(12)+T(55)));
//G347: __E_cli_command_100(235) => valid_sym(236) __(52) rr(10).
	p(NT(235), (NT(236)+NT(52)+NT(10)));
//G348: valid_cmd(234)       => __E_cli_command_100(235).
	p(NT(234), (NT(235)));
//G349: cli_command(212)     => valid_cmd(234).
	p(NT(212), (NT(234)));
//G350: sat_sym(239)         => 's' 'a' 't'.
	p(NT(239), (T(7)+T(15)+T(11)));
//G351: __E_cli_command_101(238) => sat_sym(239) __(52) normalize_cmd_arg(240).
	p(NT(238), (NT(239)+NT(52)+NT(240)));
//G352: sat_cmd(237)         => __E_cli_command_101(238).
	p(NT(237), (NT(238)));
//G353: cli_command(212)     => sat_cmd(237).
	p(NT(212), (NT(237)));
//G354: unsat_sym(243)       => 'u' 'n' 's' 'a' 't'.
	p(NT(243), (T(48)+T(46)+T(7)+T(15)+T(11)));
//G355: __E_cli_command_102(242) => unsat_sym(243) __(52) rr(10).
	p(NT(242), (NT(243)+NT(52)+NT(10)));
//G356: unsat_cmd(241)       => __E_cli_command_102(242).
	p(NT(241), (NT(242)));
//G357: cli_command(212)     => unsat_cmd(241).
	p(NT(212), (NT(241)));
//G358: solve_sym(246)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(246), (T(7)+T(8)+T(16)+T(51)+T(10)));
//G359: __E___E_cli_command_103_104(247) => __(52) type(168).
	p(NT(247), (NT(52)+NT(168)));
//G360: __E___E_cli_command_103_104(247) => null.
	p(NT(247), (nul));
//G361: __E_cli_command_103(245) => solve_sym(246) __E___E_cli_command_103_104(247) __(52) solve_cmd_arg(248).
	p(NT(245), (NT(246)+NT(247)+NT(52)+NT(248)));
//G362: solve_cmd(244)       => __E_cli_command_103(245).
	p(NT(244), (NT(245)));
//G363: cli_command(212)     => solve_cmd(244).
	p(NT(212), (NT(244)));
//G364: __E___E_cli_command_105_106(252) => 'r'.
	p(NT(252), (T(50)));
//G365: __E___E_cli_command_105_106(252) => 'r' 'u' 'n'.
	p(NT(252), (T(50)+T(48)+T(46)));
//G366: run_sym(251)         => __E___E_cli_command_105_106(252).
	p(NT(251), (NT(252)));
//G367: __E___E_cli_command_105_107(253) => __(52) num(127).
	p(NT(253), (NT(52)+NT(127)));
//G368: __E___E_cli_command_105_107(253) => null.
	p(NT(253), (nul));
//G369: __E_cli_command_105(250) => run_sym(251) __(52) wff(14) __E___E_cli_command_105_107(253).
	p(NT(250), (NT(251)+NT(52)+NT(14)+NT(253)));
//G370: run_cmd(249)         => __E_cli_command_105(250).
	p(NT(249), (NT(250)));
//G371: cli_command(212)     => run_cmd(249).
	p(NT(212), (NT(249)));
//G372: __E___E_cli_command_108_109(257) => 'n'.
	p(NT(257), (T(46)));
//G373: __E___E_cli_command_108_109(257) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(257), (T(46)+T(8)+T(50)+T(9)+T(15)+T(16)+T(12)+T(52)+T(10)));
//G374: normalize_sym(256)   => __E___E_cli_command_108_109(257).
	p(NT(256), (NT(257)));
//G375: __E_cli_command_108(255) => normalize_sym(256) __(52) normalize_cmd_arg(240).
	p(NT(255), (NT(256)+NT(52)+NT(240)));
//G376: normalize_cmd(254)   => __E_cli_command_108(255).
	p(NT(254), (NT(255)));
//G377: cli_command(212)     => normalize_cmd(254).
	p(NT(212), (NT(254)));
//G378: __E___E_cli_command_110_111(261) => 's'.
	p(NT(261), (T(7)));
//G379: __E___E_cli_command_110_111(261) => 's' 'u' 'b' 's' 't'.
	p(NT(261), (T(7)+T(48)+T(49)+T(7)+T(11)));
//G380: __E___E_cli_command_110_111(261) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(261), (T(7)+T(48)+T(49)+T(7)+T(11)+T(12)+T(11)+T(48)+T(11)+T(10)));
//G381: subst_sym(260)       => __E___E_cli_command_110_111(261).
	p(NT(260), (NT(261)));
//G382: __E_cli_command_110(259) => subst_sym(260) __(52) nf_cmd_arg(262) _(11) '[' _(11) nf_cmd_arg(262) _(11) '/' _(11) nf_cmd_arg(262) _(11) ']'.
	p(NT(259), (NT(260)+NT(52)+NT(262)+NT(11)+T(19)+NT(11)+NT(262)+NT(11)+T(56)+NT(11)+NT(262)+NT(11)+T(20)));
//G383: subst_cmd(258)       => __E_cli_command_110(259).
	p(NT(258), (NT(259)));
//G384: cli_command(212)     => subst_cmd(258).
	p(NT(212), (NT(258)));
//G385: __E___E_cli_command_112_113(266) => 'i'.
	p(NT(266), (T(12)));
//G386: __E___E_cli_command_112_113(266) => 'i' 'n' 's' 't'.
	p(NT(266), (T(12)+T(46)+T(7)+T(11)));
//G387: __E___E_cli_command_112_113(266) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(266), (T(12)+T(46)+T(7)+T(11)+T(15)+T(46)+T(11)+T(12)+T(15)+T(11)+T(10)));
//G388: inst_sym(265)        => __E___E_cli_command_112_113(266).
	p(NT(265), (NT(266)));
//G389: __E_cli_command_112(264) => inst_sym(265) __(52) inst_args(267).
	p(NT(264), (NT(265)+NT(52)+NT(267)));
//G390: inst_cmd(263)        => __E_cli_command_112(264).
	p(NT(263), (NT(264)));
//G391: cli_command(212)     => inst_cmd(263).
	p(NT(212), (NT(263)));
//G392: dnf_sym(270)         => 'd' 'n' 'f'.
	p(NT(270), (T(55)+T(46)+T(43)));
//G393: __E_cli_command_114(269) => dnf_sym(270) __(52) nf_cmd_arg(262).
	p(NT(269), (NT(270)+NT(52)+NT(262)));
//G394: dnf_cmd(268)         => __E_cli_command_114(269).
	p(NT(268), (NT(269)));
//G395: cli_command(212)     => dnf_cmd(268).
	p(NT(212), (NT(268)));
//G396: cnf_sym(273)         => 'c' 'n' 'f'.
	p(NT(273), (T(45)+T(46)+T(43)));
//G397: __E_cli_command_115(272) => cnf_sym(273) __(52) nf_cmd_arg(262).
	p(NT(272), (NT(273)+NT(52)+NT(262)));
//G398: cnf_cmd(271)         => __E_cli_command_115(272).
	p(NT(271), (NT(272)));
//G399: cli_command(212)     => cnf_cmd(271).
	p(NT(212), (NT(271)));
//G400: anf_sym(276)         => 'a' 'n' 'f'.
	p(NT(276), (T(15)+T(46)+T(43)));
//G401: __E_cli_command_116(275) => anf_sym(276) __(52) nf_cmd_arg(262).
	p(NT(275), (NT(276)+NT(52)+NT(262)));
//G402: anf_cmd(274)         => __E_cli_command_116(275).
	p(NT(274), (NT(275)));
//G403: cli_command(212)     => anf_cmd(274).
	p(NT(212), (NT(274)));
//G404: nnf_sym(279)         => 'n' 'n' 'f'.
	p(NT(279), (T(46)+T(46)+T(43)));
//G405: __E_cli_command_117(278) => nnf_sym(279) __(52) nf_cmd_arg(262).
	p(NT(278), (NT(279)+NT(52)+NT(262)));
//G406: nnf_cmd(277)         => __E_cli_command_117(278).
	p(NT(277), (NT(278)));
//G407: cli_command(212)     => nnf_cmd(277).
	p(NT(212), (NT(277)));
//G408: pnf_sym(282)         => 'p' 'n' 'f'.
	p(NT(282), (T(54)+T(46)+T(43)));
//G409: __E_cli_command_118(281) => pnf_sym(282) __(52) nf_cmd_arg(262).
	p(NT(281), (NT(282)+NT(52)+NT(262)));
//G410: pnf_cmd(280)         => __E_cli_command_118(281).
	p(NT(280), (NT(281)));
//G411: cli_command(212)     => pnf_cmd(280).
	p(NT(212), (NT(280)));
//G412: mnf_sym(285)         => 'm' 'n' 'f'.
	p(NT(285), (T(9)+T(46)+T(43)));
//G413: __E_cli_command_119(284) => mnf_sym(285) __(52) nf_cmd_arg(262).
	p(NT(284), (NT(285)+NT(52)+NT(262)));
//G414: mnf_cmd(283)         => __E_cli_command_119(284).
	p(NT(283), (NT(284)));
//G415: cli_command(212)     => mnf_cmd(283).
	p(NT(212), (NT(283)));
//G416: snf_sym(288)         => 's' 'n' 'f'.
	p(NT(288), (T(7)+T(46)+T(43)));
//G417: __E_cli_command_120(287) => snf_sym(288) __(52) nf_cmd_arg(262).
	p(NT(287), (NT(288)+NT(52)+NT(262)));
//G418: snf_cmd(286)         => __E_cli_command_120(287).
	p(NT(286), (NT(287)));
//G419: cli_command(212)     => snf_cmd(286).
	p(NT(212), (NT(286)));
//G420: onf_sym(291)         => 'o' 'n' 'f'.
	p(NT(291), (T(8)+T(46)+T(43)));
//G421: __E_cli_command_121(290) => onf_sym(291) __(52) variable(106) __(52) onf_cmd_arg(292).
	p(NT(290), (NT(291)+NT(52)+NT(106)+NT(52)+NT(292)));
//G422: onf_cmd(289)         => __E_cli_command_121(290).
	p(NT(289), (NT(290)));
//G423: cli_command(212)     => onf_cmd(289).
	p(NT(212), (NT(289)));
//G424: __E___E_cli_command_122_123(296) => 'd' 'e' 'f' 's'.
	p(NT(296), (T(55)+T(10)+T(43)+T(7)));
//G425: __E___E_cli_command_122_123(296) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(296), (T(55)+T(10)+T(43)+T(12)+T(46)+T(12)+T(11)+T(12)+T(8)+T(46)+T(7)));
//G426: def_sym(295)         => __E___E_cli_command_122_123(296).
	p(NT(295), (NT(296)));
//G427: __E_cli_command_122(294) => def_sym(295).
	p(NT(294), (NT(295)));
//G428: def_list_cmd(293)    => __E_cli_command_122(294).
	p(NT(293), (NT(294)));
//G429: cli_command(212)     => def_list_cmd(293).
	p(NT(212), (NT(293)));
//G430: __E_cli_command_124(298) => def_sym(295) __(52) number(299).
	p(NT(298), (NT(295)+NT(52)+NT(299)));
//G431: def_print_cmd(297)   => __E_cli_command_124(298).
	p(NT(297), (NT(298)));
//G432: cli_command(212)     => def_print_cmd(297).
	p(NT(212), (NT(297)));
//G433: def_rr_cmd(300)      => rec_relation(16).
	p(NT(300), (NT(16)));
//G434: cli_command(212)     => def_rr_cmd(300).
	p(NT(212), (NT(300)));
//G435: qelim_sym(303)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(303), (T(53)+T(10)+T(16)+T(12)+T(9)));
//G436: __E_cli_command_125(302) => qelim_sym(303) __(52) wff_cmd_arg(304).
	p(NT(302), (NT(303)+NT(52)+NT(304)));
//G437: qelim_cmd(301)       => __E_cli_command_125(302).
	p(NT(301), (NT(302)));
//G438: cli_command(212)     => qelim_cmd(301).
	p(NT(212), (NT(301)));
//G439: get_sym(307)         => 'g' 'e' 't'.
	p(NT(307), (T(47)+T(10)+T(11)));
//G440: __E___E_cli_command_126_127(308) => __(52) option(309).
	p(NT(308), (NT(52)+NT(309)));
//G441: __E___E_cli_command_126_127(308) => null.
	p(NT(308), (nul));
//G442: __E_cli_command_126(306) => get_sym(307) __E___E_cli_command_126_127(308).
	p(NT(306), (NT(307)+NT(308)));
//G443: get_cmd(305)         => __E_cli_command_126(306).
	p(NT(305), (NT(306)));
//G444: cli_command(212)     => get_cmd(305).
	p(NT(212), (NT(305)));
//G445: set_sym(312)         => 's' 'e' 't'.
	p(NT(312), (T(7)+T(10)+T(11)));
//G446: __E___E_cli_command_128_129(313) => __(52).
	p(NT(313), (NT(52)));
//G447: __E___E_cli_command_128_129(313) => _(11) '=' _(11).
	p(NT(313), (NT(11)+T(3)+NT(11)));
//G448: __E_cli_command_128(311) => set_sym(312) __(52) option(309) __E___E_cli_command_128_129(313) option_value(314).
	p(NT(311), (NT(312)+NT(52)+NT(309)+NT(313)+NT(314)));
//G449: set_cmd(310)         => __E_cli_command_128(311).
	p(NT(310), (NT(311)));
//G450: cli_command(212)     => set_cmd(310).
	p(NT(212), (NT(310)));
//G451: toggle_sym(317)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(317), (T(11)+T(8)+T(47)+T(47)+T(16)+T(10)));
//G452: __E_cli_command_130(316) => toggle_sym(317) __(52) bool_option(318).
	p(NT(316), (NT(317)+NT(52)+NT(318)));
//G453: toggle_cmd(315)      => __E_cli_command_130(316).
	p(NT(315), (NT(316)));
//G454: cli_command(212)     => toggle_cmd(315).
	p(NT(212), (NT(315)));
//G455: __E_cli_command_131(320) => type(168) __(52) in_var_name(144) _(11) '=' _(11) input_stream(321).
	p(NT(320), (NT(168)+NT(52)+NT(144)+NT(11)+T(3)+NT(11)+NT(321)));
//G456: def_input_cmd(319)   => __E_cli_command_131(320).
	p(NT(319), (NT(320)));
//G457: cli_command(212)     => def_input_cmd(319).
	p(NT(212), (NT(319)));
//G458: __E_cli_command_132(323) => type(168) __(52) out_var_name(148) _(11) '=' _(11) output_stream(324).
	p(NT(323), (NT(168)+NT(52)+NT(148)+NT(11)+T(3)+NT(11)+NT(324)));
//G459: def_output_cmd(322)  => __E_cli_command_132(323).
	p(NT(322), (NT(323)));
//G460: cli_command(212)     => def_output_cmd(322).
	p(NT(212), (NT(322)));
//G461: __E___E_cli_command_133_134(328) => 'h' 'i' 's' 't'.
	p(NT(328), (T(44)+T(12)+T(7)+T(11)));
//G462: __E___E_cli_command_133_134(328) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(328), (T(44)+T(12)+T(7)+T(11)+T(8)+T(50)+T(18)));
//G463: history_sym(327)     => __E___E_cli_command_133_134(328).
	p(NT(327), (NT(328)));
//G464: __E_cli_command_133(326) => history_sym(327).
	p(NT(326), (NT(327)));
//G465: history_list_cmd(325) => __E_cli_command_133(326).
	p(NT(325), (NT(326)));
//G466: cli_command(212)     => history_list_cmd(325).
	p(NT(212), (NT(325)));
//G467: __E_cli_command_135(330) => history_sym(327) __(52) memory(331).
	p(NT(330), (NT(327)+NT(52)+NT(331)));
//G468: history_print_cmd(329) => __E_cli_command_135(330).
	p(NT(329), (NT(330)));
//G469: cli_command(212)     => history_print_cmd(329).
	p(NT(212), (NT(329)));
//G470: __E_cli_command_136(333) => wff(14).
	p(NT(333), (NT(14)));
//G471: __E_cli_command_136(333) => bf(21).
	p(NT(333), (NT(21)));
//G472: history_store_cmd(332) => __E_cli_command_136(333).
	p(NT(332), (NT(333)));
//G473: cli_command(212)     => history_store_cmd(332).
	p(NT(212), (NT(332)));
//G474: __E_input_stream_137(334) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(335) _(11) ')'.
	p(NT(334), (T(12)+T(43)+T(12)+T(16)+T(10)+NT(11)+T(5)+NT(11)+NT(335)+NT(11)+T(6)));
//G475: input_stream(321)    => __E_input_stream_137(334).
	p(NT(321), (NT(334)));
//G476: console_sym(336)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(336), (T(45)+T(8)+T(46)+T(7)+T(8)+T(16)+T(10)));
//G477: input_stream(321)    => console_sym(336).
	p(NT(321), (NT(336)));
//G478: __E_output_stream_138(337) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(335) _(11) ')'.
	p(NT(337), (T(8)+T(43)+T(12)+T(16)+T(10)+NT(11)+T(5)+NT(11)+NT(335)+NT(11)+T(6)));
//G479: output_stream(324)   => __E_output_stream_138(337).
	p(NT(324), (NT(337)));
//G480: output_stream(324)   => console_sym(336).
	p(NT(324), (NT(336)));
//G481: q_file_name(335)     => '"' file_name(338) '"'.
	p(NT(335), (T(57)+NT(338)+T(57)));
//G482: __E_file_name_139(339) => printable(8).
	p(NT(339), (NT(8)));
//G483: __E_file_name_139(339) => printable(8) __E_file_name_139(339).
	p(NT(339), (NT(8)+NT(339)));
//G484: file_name(338)       => __E_file_name_139(339).
	p(NT(338), (NT(339)));
//G485: number(299)          => digits(160).
	p(NT(299), (NT(160)));
//G486: bf_cmd_arg(340)      => memory(331).
	p(NT(340), (NT(331)));
//G487: bf_cmd_arg(340)      => bf(21).
	p(NT(340), (NT(21)));
//G488: wff_cmd_arg(304)     => memory(331).
	p(NT(304), (NT(331)));
//G489: wff_cmd_arg(304)     => wff(14).
	p(NT(304), (NT(14)));
//G490: solve_cmd_arg(248)   => memory(331).
	p(NT(248), (NT(331)));
//G491: solve_cmd_arg(248)   => wff(14).
	p(NT(248), (NT(14)));
//G492: nf_cmd_arg(262)      => memory(331).
	p(NT(262), (NT(331)));
//G493: nf_cmd_arg(262)      => wff(14).
	p(NT(262), (NT(14)));
//G494: nf_cmd_arg(262)      => bf(21).
	p(NT(262), (NT(21)));
//G495: onf_cmd_arg(292)     => memory(331).
	p(NT(292), (NT(331)));
//G496: onf_cmd_arg(292)     => wff(14).
	p(NT(292), (NT(14)));
//G497: normalize_cmd_arg(240) => memory(331).
	p(NT(240), (NT(331)));
//G498: normalize_cmd_arg(240) => rr(10).
	p(NT(240), (NT(10)));
//G499: normalize_cmd_arg(240) => ref(18).
	p(NT(240), (NT(18)));
//G500: normalize_cmd_arg(240) => wff(14).
	p(NT(240), (NT(14)));
//G501: normalize_cmd_arg(240) => bf(21).
	p(NT(240), (NT(21)));
//G502: inst_args(267)       => wff_cmd_arg(304) _(11) '[' _(11) variable(106) _(11) '/' _(11) bf_cmd_arg(340) _(11) ']'.
	p(NT(267), (NT(304)+NT(11)+T(19)+NT(11)+NT(106)+NT(11)+T(56)+NT(11)+NT(340)+NT(11)+T(20)));
//G503: inst_args(267)       => bf_cmd_arg(340) _(11) '[' _(11) variable(106) _(11) '/' _(11) bf_cmd_arg(340) _(11) ']'.
	p(NT(267), (NT(340)+NT(11)+T(19)+NT(11)+NT(106)+NT(11)+T(56)+NT(11)+NT(340)+NT(11)+T(20)));
//G504: help_arg(229)        => help_sym(226).
	p(NT(229), (NT(226)));
//G505: help_arg(229)        => version_sym(219).
	p(NT(229), (NT(219)));
//G506: help_arg(229)        => quit_sym(216).
	p(NT(229), (NT(216)));
//G507: help_arg(229)        => clear_sym(222).
	p(NT(229), (NT(222)));
//G508: help_arg(229)        => get_sym(307).
	p(NT(229), (NT(307)));
//G509: help_arg(229)        => set_sym(312).
	p(NT(229), (NT(312)));
//G510: help_arg(229)        => toggle_sym(317).
	p(NT(229), (NT(317)));
//G511: help_arg(229)        => file_sym(232).
	p(NT(229), (NT(232)));
//G512: help_arg(229)        => history_sym(327).
	p(NT(229), (NT(327)));
//G513: help_arg(229)        => abs_memory_sym(341).
	p(NT(229), (NT(341)));
//G514: help_arg(229)        => rel_memory_sym(342).
	p(NT(229), (NT(342)));
//G515: help_arg(229)        => selection_sym(343).
	p(NT(229), (NT(343)));
//G516: help_arg(229)        => def_sym(295).
	p(NT(229), (NT(295)));
//G517: help_arg(229)        => inst_sym(265).
	p(NT(229), (NT(265)));
//G518: help_arg(229)        => subst_sym(260).
	p(NT(229), (NT(260)));
//G519: help_arg(229)        => normalize_sym(256).
	p(NT(229), (NT(256)));
//G520: help_arg(229)        => execute_sym(344).
	p(NT(229), (NT(344)));
//G521: help_arg(229)        => solve_sym(246).
	p(NT(229), (NT(246)));
//G522: help_arg(229)        => valid_sym(236).
	p(NT(229), (NT(236)));
//G523: help_arg(229)        => sat_sym(239).
	p(NT(229), (NT(239)));
//G524: help_arg(229)        => unsat_sym(243).
	p(NT(229), (NT(243)));
//G525: help_arg(229)        => dnf_sym(270).
	p(NT(229), (NT(270)));
//G526: help_arg(229)        => cnf_sym(273).
	p(NT(229), (NT(273)));
//G527: help_arg(229)        => anf_sym(276).
	p(NT(229), (NT(276)));
//G528: help_arg(229)        => snf_sym(288).
	p(NT(229), (NT(288)));
//G529: help_arg(229)        => nnf_sym(279).
	p(NT(229), (NT(279)));
//G530: help_arg(229)        => pnf_sym(282).
	p(NT(229), (NT(282)));
//G531: help_arg(229)        => mnf_sym(285).
	p(NT(229), (NT(285)));
//G532: help_arg(229)        => onf_sym(291).
	p(NT(229), (NT(291)));
//G533: help_arg(229)        => qelim_sym(303).
	p(NT(229), (NT(303)));
//G534: __E___E_help_arg_140_141(347) => 's'.
	p(NT(347), (T(7)));
//G535: __E___E_help_arg_140_141(347) => null.
	p(NT(347), (nul));
//G536: __E_help_arg_140(346) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_140_141(347).
	p(NT(346), (T(10)+T(22)+T(15)+T(9)+T(54)+T(16)+T(10)+NT(347)));
//G537: examples_sym(345)    => __E_help_arg_140(346).
	p(NT(345), (NT(346)));
//G538: help_arg(229)        => examples_sym(345).
	p(NT(229), (NT(345)));
//G539: __E_memory_142(349)  => '%'.
	p(NT(349), (T(58)));
//G540: rel_memory_sym(342)  => '%' '-'.
	p(NT(342), (T(58)+T(23)));
//G541: memory_id(351)       => digits(160).
	p(NT(351), (NT(160)));
//G542: __E___E_memory_142_143(350) => memory_id(351).
	p(NT(350), (NT(351)));
//G543: __E___E_memory_142_143(350) => null.
	p(NT(350), (nul));
//G544: __E_memory_142(349)  => rel_memory_sym(342) __E___E_memory_142_143(350).
	p(NT(349), (NT(342)+NT(350)));
//G545: rel_memory(348)      => __E_memory_142(349).
	p(NT(348), (NT(349)));
//G546: memory(331)          => rel_memory(348).
	p(NT(331), (NT(348)));
//G547: abs_memory_sym(341)  => '%'.
	p(NT(341), (T(58)));
//G548: __E_memory_144(353)  => abs_memory_sym(341) memory_id(351).
	p(NT(353), (NT(341)+NT(351)));
//G549: abs_memory(352)      => __E_memory_144(353).
	p(NT(352), (NT(353)));
//G550: memory(331)          => abs_memory(352).
	p(NT(331), (NT(352)));
//G551: option(309)          => bool_option(318).
	p(NT(309), (NT(318)));
//G552: __E_option_145(356)  => 's' 'e' 'v'.
	p(NT(356), (T(7)+T(10)+T(51)));
//G553: __E_option_145(356)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(356), (T(7)+T(10)+T(51)+T(10)+T(50)+T(12)+T(11)+T(18)));
//G554: severity_opt(355)    => __E_option_145(356).
	p(NT(355), (NT(356)));
//G555: enum_option(354)     => severity_opt(355).
	p(NT(354), (NT(355)));
//G556: option(309)          => enum_option(354).
	p(NT(309), (NT(354)));
//G557: __E_bool_option_146(358) => 's'.
	p(NT(358), (T(7)));
//G558: __E_bool_option_146(358) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(358), (T(7)+T(11)+T(15)+T(11)+T(48)+T(7)));
//G559: status_opt(357)      => __E_bool_option_146(358).
	p(NT(357), (NT(358)));
//G560: bool_option(318)     => status_opt(357).
	p(NT(318), (NT(357)));
//G561: __E_bool_option_147(360) => 'c'.
	p(NT(360), (T(45)));
//G562: __E_bool_option_147(360) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(360), (T(45)+T(8)+T(16)+T(8)+T(50)));
//G563: __E_bool_option_147(360) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(360), (T(45)+T(8)+T(16)+T(8)+T(50)+T(7)));
//G564: colors_opt(359)      => __E_bool_option_147(360).
	p(NT(359), (NT(360)));
//G565: bool_option(318)     => colors_opt(359).
	p(NT(318), (NT(359)));
//G566: __E_bool_option_148(362) => 'v'.
	p(NT(362), (T(51)));
//G567: __E_bool_option_148(362) => 'c' 'h' 'a' 'r' 'v' 'a' 'r'.
	p(NT(362), (T(45)+T(44)+T(15)+T(50)+T(51)+T(15)+T(50)));
//G568: charvar_opt(361)     => __E_bool_option_148(362).
	p(NT(361), (NT(362)));
//G569: bool_option(318)     => charvar_opt(361).
	p(NT(318), (NT(361)));
//G570: __E_bool_option_149(364) => 'h'.
	p(NT(364), (T(44)));
//G571: __E_bool_option_149(364) => 'h' 'i' 'l' 'i' 'g' 'h' 't'.
	p(NT(364), (T(44)+T(12)+T(16)+T(12)+T(47)+T(44)+T(11)));
//G572: __E_bool_option_149(364) => 'h' 'i' 'l' 'i' 'g' 'h' 't' 'i' 'n' 'g'.
	p(NT(364), (T(44)+T(12)+T(16)+T(12)+T(47)+T(44)+T(11)+T(12)+T(46)+T(47)));
//G573: hilighting_opt(363)  => __E_bool_option_149(364).
	p(NT(363), (NT(364)));
//G574: bool_option(318)     => hilighting_opt(363).
	p(NT(318), (NT(363)));
//G575: __E_bool_option_150(366) => 'i'.
	p(NT(366), (T(12)));
//G576: __E_bool_option_150(366) => 'i' 'n' 'd' 'e' 'n' 't'.
	p(NT(366), (T(12)+T(46)+T(55)+T(10)+T(46)+T(11)));
//G577: __E_bool_option_150(366) => 'i' 'n' 'd' 'e' 'n' 't' 'i' 'n' 'g'.
	p(NT(366), (T(12)+T(46)+T(55)+T(10)+T(46)+T(11)+T(12)+T(46)+T(47)));
//G578: indenting_opt(365)   => __E_bool_option_150(366).
	p(NT(365), (NT(366)));
//G579: bool_option(318)     => indenting_opt(365).
	p(NT(318), (NT(365)));
//G580: __E_bool_option_151(368) => 'd'.
	p(NT(368), (T(55)));
//G581: __E_bool_option_151(368) => 'd' 'b' 'g'.
	p(NT(368), (T(55)+T(49)+T(47)));
//G582: __E_bool_option_151(368) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(368), (T(55)+T(10)+T(49)+T(48)+T(47)));
//G583: debug_repl_opt(367)  => __E_bool_option_151(368).
	p(NT(367), (NT(368)));
//G584: bool_option(318)     => debug_repl_opt(367).
	p(NT(318), (NT(367)));
//G585: option_value(314)    => option_value_true(369).
	p(NT(314), (NT(369)));
//G586: option_value(314)    => option_value_false(370).
	p(NT(314), (NT(370)));
//G587: option_value(314)    => severity(371).
	p(NT(314), (NT(371)));
//G588: option_value_true(369) => 't'.
	p(NT(369), (T(11)));
//G589: option_value_true(369) => 't' 'r' 'u' 'e'.
	p(NT(369), (T(11)+T(50)+T(48)+T(10)));
//G590: option_value_true(369) => 'o' 'n'.
	p(NT(369), (T(8)+T(46)));
//G591: option_value_true(369) => '1'.
	p(NT(369), (T(33)));
//G592: option_value_true(369) => 'y'.
	p(NT(369), (T(18)));
//G593: option_value_true(369) => 'y' 'e' 's'.
	p(NT(369), (T(18)+T(10)+T(7)));
//G594: option_value_false(370) => 'f'.
	p(NT(370), (T(43)));
//G595: option_value_false(370) => 'f' 'a' 'l' 's' 'e'.
	p(NT(370), (T(43)+T(15)+T(16)+T(7)+T(10)));
//G596: option_value_false(370) => 'o' 'f' 'f'.
	p(NT(370), (T(8)+T(43)+T(43)));
//G597: option_value_false(370) => '0'.
	p(NT(370), (T(34)));
//G598: option_value_false(370) => 'n'.
	p(NT(370), (T(46)));
//G599: option_value_false(370) => 'n' 'o'.
	p(NT(370), (T(46)+T(8)));
//G600: __E_severity_152(373) => 'e'.
	p(NT(373), (T(10)));
//G601: __E_severity_152(373) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(373), (T(10)+T(50)+T(50)+T(8)+T(50)));
//G602: error_sym(372)       => __E_severity_152(373).
	p(NT(372), (NT(373)));
//G603: severity(371)        => error_sym(372).
	p(NT(371), (NT(372)));
//G604: __E_severity_153(375) => 'i'.
	p(NT(375), (T(12)));
//G605: __E_severity_153(375) => 'i' 'n' 'f' 'o'.
	p(NT(375), (T(12)+T(46)+T(43)+T(8)));
//G606: info_sym(374)        => __E_severity_153(375).
	p(NT(374), (NT(375)));
//G607: severity(371)        => info_sym(374).
	p(NT(371), (NT(374)));
//G608: __E_severity_154(377) => 'd'.
	p(NT(377), (T(55)));
//G609: __E_severity_154(377) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(377), (T(55)+T(10)+T(49)+T(48)+T(47)));
//G610: debug_sym(376)       => __E_severity_154(377).
	p(NT(376), (NT(377)));
//G611: severity(371)        => debug_sym(376).
	p(NT(371), (NT(376)));
//G612: __E_severity_155(379) => 't'.
	p(NT(379), (T(11)));
//G613: __E_severity_155(379) => 't' 'r' 'a' 'c' 'e'.
	p(NT(379), (T(11)+T(50)+T(15)+T(45)+T(10)));
//G614: trace_sym(378)       => __E_severity_155(379).
	p(NT(378), (NT(379)));
//G615: severity(371)        => trace_sym(378).
	p(NT(371), (NT(378)));
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
		bf_f, uninterpreted_constant, bf_and_nosep_1st_op, __E_bf_and_46, ctn_neq, __E_constraint_47, ctnvar, num, ctn_eq, __E_constraint_48, 
		ctn_greater_equal, __E_constraint_49, ctn_greater, __E_constraint_50, ctn_less_equal, __E_constraint_51, ctn_less, __E_constraint_52, __E_variable_53, chars, 
		io_var, __E_variable_54, in, out, in_var_name, __E_in_55, __E___E_in_55_56, offset, out_var_name, __E_out_57, 
		__E___E_out_57_58, q_var, __E_q_vars_59, __E_q_vars_60, __E_offsets_61, __E_offsets_62, shift, __E_offset_63, __E_shift_64, __E___E_shift_64_65, 
		digits, uninter_const_name, __E_uninterpreted_constant_66, __E___E_uninterpreted_constant_66_67, constant, __E_bf_constant_68, binding, __E_bf_constant_69, type, source, 
		__E_binding_70, src_c, __E___E_binding_70_71, __E___E___E_binding_70_71_72, __E___E___E_binding_70_71_73, __E_src_c_74, __E_src_c_75, __E_chars_76, __E_chars_77, __E_digits_78, 
		extra, comment, __E_comment_79, __E_comment_80, __E_comment_81, __E____82, __E___83, bf_cb_args1, bf_cb_arg, bf_cb_args2, 
		bf_cb_args3, bf_cb_args4, wff_cb_args1, wff_cb_arg, wff_cb_args2, wff_cb_args3, wff_cb_args4, wff_has_clashing_subformulas_cb, __E_wff_cb_84, wff_has_subformula_cb, 
		__E_wff_cb_85, wff_remove_existential_cb, __E_wff_cb_86, bf_has_subformula_cb, __E_bf_cb_87, bf_remove_funiversal_cb, __E_bf_cb_88, bf_remove_fexistential_cb, __E_bf_cb_89, bf_normalize_cb, 
		__E_bf_cb_90, cli, cli_command, __E_cli_91, __E_cli_92, quit_cmd, quit_sym, __E_cli_command_93, version_cmd, version_sym, 
		__E_cli_command_94, clear_cmd, clear_sym, __E_cli_command_95, help_cmd, __E_cli_command_96, help_sym, __E___E_cli_command_96_97, __E___E_cli_command_96_98, help_arg, 
		file_cmd, __E_cli_command_99, file_sym, q_string, valid_cmd, __E_cli_command_100, valid_sym, sat_cmd, __E_cli_command_101, sat_sym, 
		normalize_cmd_arg, unsat_cmd, __E_cli_command_102, unsat_sym, solve_cmd, __E_cli_command_103, solve_sym, __E___E_cli_command_103_104, solve_cmd_arg, run_cmd, 
		__E_cli_command_105, run_sym, __E___E_cli_command_105_106, __E___E_cli_command_105_107, normalize_cmd, __E_cli_command_108, normalize_sym, __E___E_cli_command_108_109, subst_cmd, __E_cli_command_110, 
		subst_sym, __E___E_cli_command_110_111, nf_cmd_arg, inst_cmd, __E_cli_command_112, inst_sym, __E___E_cli_command_112_113, inst_args, dnf_cmd, __E_cli_command_114, 
		dnf_sym, cnf_cmd, __E_cli_command_115, cnf_sym, anf_cmd, __E_cli_command_116, anf_sym, nnf_cmd, __E_cli_command_117, nnf_sym, 
		pnf_cmd, __E_cli_command_118, pnf_sym, mnf_cmd, __E_cli_command_119, mnf_sym, snf_cmd, __E_cli_command_120, snf_sym, onf_cmd, 
		__E_cli_command_121, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_122, def_sym, __E___E_cli_command_122_123, def_print_cmd, __E_cli_command_124, number, 
		def_rr_cmd, qelim_cmd, __E_cli_command_125, qelim_sym, wff_cmd_arg, get_cmd, __E_cli_command_126, get_sym, __E___E_cli_command_126_127, option, 
		set_cmd, __E_cli_command_128, set_sym, __E___E_cli_command_128_129, option_value, toggle_cmd, __E_cli_command_130, toggle_sym, bool_option, def_input_cmd, 
		__E_cli_command_131, input_stream, def_output_cmd, __E_cli_command_132, output_stream, history_list_cmd, __E_cli_command_133, history_sym, __E___E_cli_command_133_134, history_print_cmd, 
		__E_cli_command_135, memory, history_store_cmd, __E_cli_command_136, __E_input_stream_137, q_file_name, console_sym, __E_output_stream_138, file_name, __E_file_name_139, 
		bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, execute_sym, examples_sym, __E_help_arg_140, __E___E_help_arg_140_141, rel_memory, __E_memory_142, 
		__E___E_memory_142_143, memory_id, abs_memory, __E_memory_144, enum_option, severity_opt, __E_option_145, status_opt, __E_bool_option_146, colors_opt, 
		__E_bool_option_147, charvar_opt, __E_bool_option_148, hilighting_opt, __E_bool_option_149, indenting_opt, __E_bool_option_150, debug_repl_opt, __E_bool_option_151, option_value_true, 
		option_value_false, severity, error_sym, __E_severity_152, info_sym, __E_severity_153, debug_sym, __E_severity_154, trace_sym, __E_severity_155, 
		named_binding, variable_not_io, __N_0, __N_1, __N_2, __N_3, 
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
