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
	"capture", "bf", "sym", "__E_ref_3", "offsets", "ref_args", "__E_ref_4", "__", "fp_fallback", "first_sym", 
	"last_sym", "__E_ref_args_5", "__E___E_ref_args_5_6", "ref_arg", "__E___E_ref_args_5_7", "__E___E_ref_args_5_8", "library", "rules", "__E_library_9", "__E___E_library_9_10", 
	"rule", "__E___E___E_library_9_10_11", "wff_rule", "bf_rule", "__E___E_library_9_12", "wff_matcher", "wff_body", "__E_wff_rule_13", "wff_cb", "bf_matcher", 
	"bf_body", "__E_bf_rule_14", "bf_cb", "builder", "builder_head", "builder_body", "__E_builder_head_15", "__E_builder_head_16", "bf_builder_body", "__E_builder_body_17", 
	"wff_builder_body", "__E_builder_body_18", "tau_constant_source", "__E_tau_constant_source_19", "wff_parenthesis", "__E_wff_20", "wff_sometimes", "__E_wff_21", "__E___E_wff_21_22", "wff_always", 
	"__E_wff_23", "__E___E_wff_23_24", "wff_conditional", "__E_wff_25", "wff_all", "__E_wff_26", "q_vars", "wff_ex", "__E_wff_27", "wff_ref", 
	"constraint", "wff_imply", "__E_wff_28", "wff_equiv", "__E_wff_29", "wff_or", "__E_wff_30", "wff_xor", "__E_wff_31", "wff_and", 
	"__E_wff_32", "wff_neg", "__E_wff_33", "wff_t", "wff_f", "bf_interval", "__E_wff_34", "bf_eq", "__E_wff_35", "bf_neq", 
	"__E_wff_36", "bf_less_equal", "__E_wff_37", "bf_nleq", "__E_wff_38", "bf_greater", "__E_wff_39", "bf_ngreater", "__E_wff_40", "bf_greater_equal", 
	"__E_wff_41", "bf_ngeq", "__E_wff_42", "bf_less", "__E_wff_43", "bf_nless", "__E_wff_44", "bf_parenthesis", "__E_bf_45", "bf_constant", 
	"variable", "bf_splitter", "__E_bf_46", "bf_ref", "bf_or", "__E_bf_47", "bf_xor", "__E_bf_48", "bf_t", "__E_bf_49", 
	"__E___E_bf_49_50", "type", "bf_f", "__E_bf_51", "__E___E_bf_51_52", "bf_and", "__E_bf_53", "__E___E_bf_53_54", "bf_and_nosep", "bf_neg", 
	"__E_bf_55", "__E_bf_and_nosep_56", "ctn_neq", "__E_constraint_57", "ctnvar", "num", "ctn_eq", "__E_constraint_58", "ctn_greater_equal", "__E_constraint_59", 
	"ctn_greater", "__E_constraint_60", "ctn_less_equal", "__E_constraint_61", "ctn_less", "__E_constraint_62", "__E_variable_63", "chars", "io_var", "__E_variable_64", 
	"in", "out", "uninterpreted_constant", "in_var_name", "__E_in_65", "__E___E_in_65_66", "offset", "out_var_name", "__E_out_67", "__E___E_out_67_68", 
	"q_var", "__E_q_vars_69", "__E_q_vars_70", "__E_offsets_71", "__E_offsets_72", "shift", "__E_offset_73", "__E_shift_74", "__E___E_shift_74_75", "digits", 
	"uninter_const_name", "__E_uninterpreted_constant_76", "__E___E_uninterpreted_constant_76_77", "constant", "__E_bf_constant_78", "binding", "__E_bf_constant_79", "source", "__E_binding_80", "src_c", 
	"__E___E_binding_80_81", "__E___E___E_binding_80_81_82", "__E___E___E_binding_80_81_83", "__E_src_c_84", "__E_src_c_85", "__E_chars_86", "__E_chars_87", "__E_digits_88", "extra", "comment", 
	"__E_comment_89", "__E_comment_90", "__E_comment_91", "__E____92", "__E___93", "bf_cb_args1", "bf_cb_arg", "bf_cb_args2", "bf_cb_args3", "bf_cb_args4", 
	"wff_cb_args1", "wff_cb_arg", "wff_cb_args2", "wff_cb_args3", "wff_cb_args4", "wff_has_clashing_subformulas_cb", "__E_wff_cb_94", "wff_has_subformula_cb", "__E_wff_cb_95", "wff_remove_existential_cb", 
	"__E_wff_cb_96", "bf_has_subformula_cb", "__E_bf_cb_97", "bf_remove_funiversal_cb", "__E_bf_cb_98", "bf_remove_fexistential_cb", "__E_bf_cb_99", "bf_normalize_cb", "__E_bf_cb_100", "cli", 
	"cli_command", "__E_cli_101", "__E_cli_102", "quit_cmd", "quit_sym", "__E_cli_command_103", "version_cmd", "version_sym", "__E_cli_command_104", "clear_cmd", 
	"clear_sym", "__E_cli_command_105", "help_cmd", "__E_cli_command_106", "help_sym", "__E___E_cli_command_106_107", "__E___E_cli_command_106_108", "help_arg", "file_cmd", "__E_cli_command_109", 
	"file_sym", "q_string", "valid_cmd", "__E_cli_command_110", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_111", "sat_sym", "unsat_cmd", 
	"__E_cli_command_112", "unsat_sym", "solve_cmd", "__E_cli_command_113", "solve_sym", "__E___E_cli_command_113_114", "solve_cmd_arg", "run_cmd", "__E_cli_command_115", "run_sym", 
	"__E___E_cli_command_115_116", "__E___E_cli_command_115_117", "memory", "normalize_cmd", "__E_cli_command_118", "normalize_sym", "__E___E_cli_command_118_119", "subst_cmd", "__E_cli_command_120", "subst_sym", 
	"__E___E_cli_command_120_121", "nf_cmd_arg", "inst_cmd", "__E_cli_command_122", "inst_sym", "__E___E_cli_command_122_123", "inst_args", "dnf_cmd", "__E_cli_command_124", "dnf_sym", 
	"cnf_cmd", "__E_cli_command_125", "cnf_sym", "anf_cmd", "__E_cli_command_126", "anf_sym", "nnf_cmd", "__E_cli_command_127", "nnf_sym", "pnf_cmd", 
	"__E_cli_command_128", "pnf_sym", "mnf_cmd", "__E_cli_command_129", "mnf_sym", "snf_cmd", "__E_cli_command_130", "snf_sym", "onf_cmd", "__E_cli_command_131", 
	"onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_132", "def_sym", "__E___E_cli_command_132_133", "def_print_cmd", "__E_cli_command_134", "number", "def_rr_cmd", 
	"qelim_cmd", "__E_cli_command_135", "qelim_sym", "wff_cmd_arg", "get_cmd", "__E_cli_command_136", "get_sym", "__E___E_cli_command_136_137", "option", "set_cmd", 
	"__E_cli_command_138", "set_sym", "__E___E_cli_command_138_139", "option_value", "enable_cmd", "__E_cli_command_140", "enable_sym", "bool_option", "disable_cmd", "__E_cli_command_141", 
	"disable_sym", "toggle_cmd", "__E_cli_command_142", "toggle_sym", "def_input_cmd", "__E_cli_command_143", "input_stream", "def_output_cmd", "__E_cli_command_144", "output_stream", 
	"history_list_cmd", "__E_cli_command_145", "history_sym", "__E___E_cli_command_145_146", "history_print_cmd", "__E_cli_command_147", "history_store_cmd", "__E_cli_command_148", "__E_input_stream_149", "q_file_name", 
	"console_sym", "__E_output_stream_150", "file_name", "__E_file_name_151", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "execute_sym", "examples_sym", 
	"__E_help_arg_152", "__E___E_help_arg_152_153", "rel_memory", "__E_memory_154", "__E___E_memory_154_155", "memory_id", "abs_memory", "__E_memory_156", "enum_option", "severity_opt", 
	"__E_option_157", "status_opt", "__E_bool_option_158", "colors_opt", "__E_bool_option_159", "charvar_opt", "__E_bool_option_160", "highlighting_opt", "__E_bool_option_161", "indenting_opt", 
	"__E_bool_option_162", "debug_repl_opt", "__E_bool_option_163", "option_value_true", "option_value_false", "severity", "error_sym", "__E_severity_164", "info_sym", "__E_severity_165", 
	"debug_sym", "__E_severity_166", "trace_sym", "__E_severity_167", "named_binding", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", 
	"__N_5", "__N_6", "__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'f', 'a', 'l', 'b', 'c', 
	'k', 'i', 'r', 's', 't', ',', '(', ')', 'o', 'm', 
	'e', '<', '>', 'w', 'y', '[', ']', '?', 'x', '-', 
	'|', '^', '&', '!', 'T', 'F', 'S', '+', '1', '0', 
	'\'', '$', '{', '}', '_', '\t', '\n', '\r', '#', 'h', 
	'n', 'g', 'u', 'v', 'z', 'q', 'p', 'd', '/', '"', 
	'%', 'V', 'H', 'I', 
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
			11, 27
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			20, 22, 120, 131, 144, 163, 167, 179, 180, 187,
			362, 404
		},
		.to_inline = {
			{ 14, 64, 14 },
			{ 21, 117, 21 },
			{ 48 },
			{ 52 },
			{ 138 },
			{ 157 },
			{ 170 },
			{ 189 },
			{ 205 },
			{ 207 },
			{ 208 },
			{ 209 },
			{ 210 },
			{ 212 },
			{ 213 },
			{ 214 },
			{ 255 },
			{ 266 },
			{ 281 },
			{ 286 },
			{ 311 },
			{ 323 },
			{ 364 }
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
//G14:  __E_ref_4(26)        => _(11) 'f' 'a' 'l' 'l' 'b' 'a' 'c' 'k' __(27) fp_fallback(28).
	p(NT(26), (NT(11)+T(4)+T(5)+T(6)+T(6)+T(7)+T(5)+T(8)+T(9)+NT(27)+NT(28)));
//G15:  __E_ref_4(26)        => null.
	p(NT(26), (nul));
//G16:  ref(18)              => sym(22) __E_ref_3(23) _(11) ref_args(25) __E_ref_4(26).
	p(NT(18), (NT(22)+NT(23)+NT(11)+NT(25)+NT(26)));
//G17:  first_sym(29)        => 'f' 'i' 'r' 's' 't'.
	p(NT(29), (T(4)+T(10)+T(11)+T(12)+T(13)));
//G18:  fp_fallback(28)      => first_sym(29).
	p(NT(28), (NT(29)));
//G19:  last_sym(30)         => 'l' 'a' 's' 't'.
	p(NT(30), (T(6)+T(5)+T(12)+T(13)));
//G20:  fp_fallback(28)      => last_sym(30).
	p(NT(28), (NT(30)));
//G21:  fp_fallback(28)      => capture(20).
	p(NT(28), (NT(20)));
//G22:  fp_fallback(28)      => ref(18).
	p(NT(28), (NT(18)));
//G23:  fp_fallback(28)      => wff(14).
	p(NT(28), (NT(14)));
//G24:  fp_fallback(28)      => bf(21).
	p(NT(28), (NT(21)));
//G25:  ref_arg(33)          => bf(21).
	p(NT(33), (NT(21)));
//G26:  __E___E_ref_args_5_6(32) => _(11) ref_arg(33).
	p(NT(32), (NT(11)+NT(33)));
//G27:  __E___E_ref_args_5_7(34) => _(11) ',' _(11) ref_arg(33).
	p(NT(34), (NT(11)+T(14)+NT(11)+NT(33)));
//G28:  __E___E_ref_args_5_8(35) => null.
	p(NT(35), (nul));
//G29:  __E___E_ref_args_5_8(35) => __E___E_ref_args_5_7(34) __E___E_ref_args_5_8(35).
	p(NT(35), (NT(34)+NT(35)));
//G30:  __E_ref_args_5(31)   => __E___E_ref_args_5_6(32) __E___E_ref_args_5_8(35).
	p(NT(31), (NT(32)+NT(35)));
//G31:  __E_ref_args_5(31)   => null.
	p(NT(31), (nul));
//G32:  ref_args(25)         => '(' __E_ref_args_5(31) _(11) ')'.
	p(NT(25), (T(15)+NT(31)+NT(11)+T(16)));
//G33:  __E___E___E_library_9_10_11(41) => wff_rule(42).
	p(NT(41), (NT(42)));
//G34:  __E___E___E_library_9_10_11(41) => bf_rule(43).
	p(NT(41), (NT(43)));
//G35:  rule(40)             => __E___E___E_library_9_10_11(41).
	p(NT(40), (NT(41)));
//G36:  __E___E_library_9_10(39) => _(11) rule(40).
	p(NT(39), (NT(11)+NT(40)));
//G37:  __E___E_library_9_12(44) => null.
	p(NT(44), (nul));
//G38:  __E___E_library_9_12(44) => __E___E_library_9_10(39) __E___E_library_9_12(44).
	p(NT(44), (NT(39)+NT(44)));
//G39:  __E_library_9(38)    => __E___E_library_9_12(44).
	p(NT(38), (NT(44)));
//G40:  rules(37)            => __E_library_9(38).
	p(NT(37), (NT(38)));
//G41:  library(36)          => rules(37).
	p(NT(36), (NT(37)));
//G42:  wff_matcher(45)      => wff(14).
	p(NT(45), (NT(14)));
//G43:  __E_wff_rule_13(47)  => wff(14).
	p(NT(47), (NT(14)));
//G44:  __E_wff_rule_13(47)  => wff_cb(48).
	p(NT(47), (NT(48)));
//G45:  wff_body(46)         => __E_wff_rule_13(47).
	p(NT(46), (NT(47)));
//G46:  wff_rule(42)         => wff_matcher(45) _(11) ':' ':' '=' _(11) wff_body(46) _(11) '.'.
	p(NT(42), (NT(45)+NT(11)+T(2)+T(2)+T(3)+NT(11)+NT(46)+NT(11)+T(1)));
//G47:  bf_matcher(49)       => bf(21).
	p(NT(49), (NT(21)));
//G48:  __E_bf_rule_14(51)   => bf(21).
	p(NT(51), (NT(21)));
//G49:  __E_bf_rule_14(51)   => bf_cb(52).
	p(NT(51), (NT(52)));
//G50:  bf_body(50)          => __E_bf_rule_14(51).
	p(NT(50), (NT(51)));
//G51:  bf_rule(43)          => bf_matcher(49) _(11) ':' '=' _(11) bf_body(50) _(11) '.'.
	p(NT(43), (NT(49)+NT(11)+T(2)+T(3)+NT(11)+NT(50)+NT(11)+T(1)));
//G52:  builder(53)          => _(11) builder_head(54) _(11) builder_body(55) _(11) '.'.
	p(NT(53), (NT(11)+NT(54)+NT(11)+NT(55)+NT(11)+T(1)));
//G53:  __E_builder_head_15(56) => __(27) capture(20).
	p(NT(56), (NT(27)+NT(20)));
//G54:  __E_builder_head_16(57) => null.
	p(NT(57), (nul));
//G55:  __E_builder_head_16(57) => __E_builder_head_15(56) __E_builder_head_16(57).
	p(NT(57), (NT(56)+NT(57)));
//G56:  builder_head(54)     => '(' _(11) capture(20) __E_builder_head_16(57) _(11) ')'.
	p(NT(54), (T(15)+NT(11)+NT(20)+NT(57)+NT(11)+T(16)));
//G57:  __E_builder_body_17(59) => '=' ':' _(11) bf(21).
	p(NT(59), (T(3)+T(2)+NT(11)+NT(21)));
//G58:  bf_builder_body(58)  => __E_builder_body_17(59).
	p(NT(58), (NT(59)));
//G59:  builder_body(55)     => bf_builder_body(58).
	p(NT(55), (NT(58)));
//G60:  __E_builder_body_18(61) => '=' ':' ':' _(11) wff(14).
	p(NT(61), (T(3)+T(2)+T(2)+NT(11)+NT(14)));
//G61:  wff_builder_body(60) => __E_builder_body_18(61).
	p(NT(60), (NT(61)));
//G62:  builder_body(55)     => wff_builder_body(60).
	p(NT(55), (NT(60)));
//G63:  __E_tau_constant_source_19(63) => '.' _(11).
	p(NT(63), (T(1)+NT(11)));
//G64:  __E_tau_constant_source_19(63) => null.
	p(NT(63), (nul));
//G65:  tau_constant_source(62) => rec_relations(12) _(11) main(13) _(11) __E_tau_constant_source_19(63).
	p(NT(62), (NT(12)+NT(11)+NT(13)+NT(11)+NT(63)));
//G66:  __E_wff_20(65)       => '(' _(11) wff(14) _(11) ')'.
	p(NT(65), (T(15)+NT(11)+NT(14)+NT(11)+T(16)));
//G67:  wff_parenthesis(64)  => __E_wff_20(65).
	p(NT(64), (NT(65)));
//G68:  wff(14)              => wff_parenthesis(64).
	p(NT(14), (NT(64)));
//G69:  __E___E_wff_21_22(68) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(68), (T(12)+T(17)+T(18)+T(19)+T(13)+T(10)+T(18)+T(19)+T(12)));
//G70:  __E___E_wff_21_22(68) => '<' '>'.
	p(NT(68), (T(20)+T(21)));
//G71:  __E_wff_21(67)       => __E___E_wff_21_22(68) _(11) wff(14).
	p(NT(67), (NT(68)+NT(11)+NT(14)));
//G72:  wff_sometimes(66)    => __E_wff_21(67).
	p(NT(66), (NT(67)));
//G73:  wff(14)              => wff_sometimes(66).
	p(NT(14), (NT(66)));
//G74:  __E___E_wff_23_24(71) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(71), (T(5)+T(6)+T(22)+T(5)+T(23)+T(12)));
//G75:  __E___E_wff_23_24(71) => '[' ']'.
	p(NT(71), (T(24)+T(25)));
//G76:  __E_wff_23(70)       => __E___E_wff_23_24(71) _(11) wff(14).
	p(NT(70), (NT(71)+NT(11)+NT(14)));
//G77:  wff_always(69)       => __E_wff_23(70).
	p(NT(69), (NT(70)));
//G78:  wff(14)              => wff_always(69).
	p(NT(14), (NT(69)));
//G79:  __E_wff_25(73)       => wff(14) _(11) '?' _(11) wff(14) _(11) ':' _(11) wff(14).
	p(NT(73), (NT(14)+NT(11)+T(26)+NT(11)+NT(14)+NT(11)+T(2)+NT(11)+NT(14)));
//G80:  wff_conditional(72)  => __E_wff_25(73).
	p(NT(72), (NT(73)));
//G81:  wff(14)              => wff_conditional(72).
	p(NT(14), (NT(72)));
//G82:  __E_wff_26(75)       => 'a' 'l' 'l' __(27) q_vars(76) __(27) wff(14).
	p(NT(75), (T(5)+T(6)+T(6)+NT(27)+NT(76)+NT(27)+NT(14)));
//G83:  wff_all(74)          => __E_wff_26(75).
	p(NT(74), (NT(75)));
//G84:  wff(14)              => wff_all(74).
	p(NT(14), (NT(74)));
//G85:  __E_wff_27(78)       => 'e' 'x' __(27) q_vars(76) __(27) wff(14).
	p(NT(78), (T(19)+T(27)+NT(27)+NT(76)+NT(27)+NT(14)));
//G86:  wff_ex(77)           => __E_wff_27(78).
	p(NT(77), (NT(78)));
//G87:  wff(14)              => wff_ex(77).
	p(NT(14), (NT(77)));
//G88:  wff_ref(79)          => ref(18).
	p(NT(79), (NT(18)));
//G89:  wff(14)              => wff_ref(79).
	p(NT(14), (NT(79)));
//G90:  wff(14)              => constraint(80).
	p(NT(14), (NT(80)));
//G91:  __E_wff_28(82)       => wff(14) _(11) '-' '>' _(11) wff(14).
	p(NT(82), (NT(14)+NT(11)+T(28)+T(21)+NT(11)+NT(14)));
//G92:  wff_imply(81)        => __E_wff_28(82).
	p(NT(81), (NT(82)));
//G93:  wff(14)              => wff_imply(81).
	p(NT(14), (NT(81)));
//G94:  __E_wff_29(84)       => wff(14) _(11) '<' '-' '>' _(11) wff(14).
	p(NT(84), (NT(14)+NT(11)+T(20)+T(28)+T(21)+NT(11)+NT(14)));
//G95:  wff_equiv(83)        => __E_wff_29(84).
	p(NT(83), (NT(84)));
//G96:  wff(14)              => wff_equiv(83).
	p(NT(14), (NT(83)));
//G97:  __E_wff_30(86)       => wff(14) _(11) '|' '|' _(11) wff(14).
	p(NT(86), (NT(14)+NT(11)+T(29)+T(29)+NT(11)+NT(14)));
//G98:  wff_or(85)           => __E_wff_30(86).
	p(NT(85), (NT(86)));
//G99:  wff(14)              => wff_or(85).
	p(NT(14), (NT(85)));
//G100: __E_wff_31(88)       => wff(14) _(11) '^' _(11) wff(14).
	p(NT(88), (NT(14)+NT(11)+T(30)+NT(11)+NT(14)));
//G101: wff_xor(87)          => __E_wff_31(88).
	p(NT(87), (NT(88)));
//G102: wff(14)              => wff_xor(87).
	p(NT(14), (NT(87)));
//G103: __E_wff_32(90)       => wff(14) _(11) '&' '&' _(11) wff(14).
	p(NT(90), (NT(14)+NT(11)+T(31)+T(31)+NT(11)+NT(14)));
//G104: wff_and(89)          => __E_wff_32(90).
	p(NT(89), (NT(90)));
//G105: wff(14)              => wff_and(89).
	p(NT(14), (NT(89)));
//G106: __E_wff_33(92)       => '!' _(11) wff(14).
	p(NT(92), (T(32)+NT(11)+NT(14)));
//G107: wff_neg(91)          => __E_wff_33(92).
	p(NT(91), (NT(92)));
//G108: wff(14)              => wff_neg(91).
	p(NT(14), (NT(91)));
//G109: wff_t(93)            => 'T'.
	p(NT(93), (T(33)));
//G110: wff(14)              => wff_t(93).
	p(NT(14), (NT(93)));
//G111: wff_f(94)            => 'F'.
	p(NT(94), (T(34)));
//G112: wff(14)              => wff_f(94).
	p(NT(14), (NT(94)));
//G113: __E_wff_34(96)       => bf(21) _(11) '<' '=' _(11) bf(21) _(11) '<' '=' _(11) bf(21).
	p(NT(96), (NT(21)+NT(11)+T(20)+T(3)+NT(11)+NT(21)+NT(11)+T(20)+T(3)+NT(11)+NT(21)));
//G114: bf_interval(95)      => __E_wff_34(96).
	p(NT(95), (NT(96)));
//G115: wff(14)              => bf_interval(95).
	p(NT(14), (NT(95)));
//G116: __E_wff_35(98)       => bf(21) _(11) '=' _(11) bf(21).
	p(NT(98), (NT(21)+NT(11)+T(3)+NT(11)+NT(21)));
//G117: bf_eq(97)            => __E_wff_35(98).
	p(NT(97), (NT(98)));
//G118: wff(14)              => bf_eq(97).
	p(NT(14), (NT(97)));
//G119: __E_wff_36(100)      => bf(21) _(11) '!' '=' _(11) bf(21).
	p(NT(100), (NT(21)+NT(11)+T(32)+T(3)+NT(11)+NT(21)));
//G120: bf_neq(99)           => __E_wff_36(100).
	p(NT(99), (NT(100)));
//G121: wff(14)              => bf_neq(99).
	p(NT(14), (NT(99)));
//G122: __E_wff_37(102)      => bf(21) _(11) '<' '=' _(11) bf(21).
	p(NT(102), (NT(21)+NT(11)+T(20)+T(3)+NT(11)+NT(21)));
//G123: bf_less_equal(101)   => __E_wff_37(102).
	p(NT(101), (NT(102)));
//G124: wff(14)              => bf_less_equal(101).
	p(NT(14), (NT(101)));
//G125: __E_wff_38(104)      => bf(21) _(11) '!' '<' '=' _(11) bf(21).
	p(NT(104), (NT(21)+NT(11)+T(32)+T(20)+T(3)+NT(11)+NT(21)));
//G126: bf_nleq(103)         => __E_wff_38(104).
	p(NT(103), (NT(104)));
//G127: wff(14)              => bf_nleq(103).
	p(NT(14), (NT(103)));
//G128: __E_wff_39(106)      => bf(21) _(11) '>' _(11) bf(21).
	p(NT(106), (NT(21)+NT(11)+T(21)+NT(11)+NT(21)));
//G129: bf_greater(105)      => __E_wff_39(106).
	p(NT(105), (NT(106)));
//G130: wff(14)              => bf_greater(105).
	p(NT(14), (NT(105)));
//G131: __E_wff_40(108)      => bf(21) _(11) '!' '>' _(11) bf(21).
	p(NT(108), (NT(21)+NT(11)+T(32)+T(21)+NT(11)+NT(21)));
//G132: bf_ngreater(107)     => __E_wff_40(108).
	p(NT(107), (NT(108)));
//G133: wff(14)              => bf_ngreater(107).
	p(NT(14), (NT(107)));
//G134: __E_wff_41(110)      => bf(21) _(11) '>' '=' _(11) bf(21).
	p(NT(110), (NT(21)+NT(11)+T(21)+T(3)+NT(11)+NT(21)));
//G135: bf_greater_equal(109) => __E_wff_41(110).
	p(NT(109), (NT(110)));
//G136: wff(14)              => bf_greater_equal(109).
	p(NT(14), (NT(109)));
//G137: __E_wff_42(112)      => bf(21) _(11) '!' '>' '=' _(11) bf(21).
	p(NT(112), (NT(21)+NT(11)+T(32)+T(21)+T(3)+NT(11)+NT(21)));
//G138: bf_ngeq(111)         => __E_wff_42(112).
	p(NT(111), (NT(112)));
//G139: wff(14)              => bf_ngeq(111).
	p(NT(14), (NT(111)));
//G140: __E_wff_43(114)      => bf(21) _(11) '<' _(11) bf(21).
	p(NT(114), (NT(21)+NT(11)+T(20)+NT(11)+NT(21)));
//G141: bf_less(113)         => __E_wff_43(114).
	p(NT(113), (NT(114)));
//G142: wff(14)              => bf_less(113).
	p(NT(14), (NT(113)));
//G143: __E_wff_44(116)      => bf(21) _(11) '!' '<' _(11) bf(21).
	p(NT(116), (NT(21)+NT(11)+T(32)+T(20)+NT(11)+NT(21)));
//G144: bf_nless(115)        => __E_wff_44(116).
	p(NT(115), (NT(116)));
//G145: wff(14)              => bf_nless(115).
	p(NT(14), (NT(115)));
//G146: wff(14)              => capture(20).
	p(NT(14), (NT(20)));
//G147: __E_bf_45(118)       => '(' _(11) bf(21) _(11) ')'.
	p(NT(118), (T(15)+NT(11)+NT(21)+NT(11)+T(16)));
//G148: bf_parenthesis(117)  => __E_bf_45(118).
	p(NT(117), (NT(118)));
//G149: bf(21)               => bf_parenthesis(117).
	p(NT(21), (NT(117)));
//G150: bf(21)               => bf_constant(119).
	p(NT(21), (NT(119)));
//G151: bf(21)               => variable(120).
	p(NT(21), (NT(120)));
//G152: __E_bf_46(122)       => 'S' _(11) '(' _(11) bf(21) _(11) ')'.
	p(NT(122), (T(35)+NT(11)+T(15)+NT(11)+NT(21)+NT(11)+T(16)));
//G153: bf_splitter(121)     => __E_bf_46(122).
	p(NT(121), (NT(122)));
//G154: bf(21)               => bf_splitter(121).
	p(NT(21), (NT(121)));
//G155: bf_ref(123)          => ref(18).
	p(NT(123), (NT(18)));
//G156: bf(21)               => bf_ref(123).
	p(NT(21), (NT(123)));
//G157: __E_bf_47(125)       => bf(21) _(11) '|' _(11) bf(21).
	p(NT(125), (NT(21)+NT(11)+T(29)+NT(11)+NT(21)));
//G158: bf_or(124)           => __E_bf_47(125).
	p(NT(124), (NT(125)));
//G159: bf(21)               => bf_or(124).
	p(NT(21), (NT(124)));
//G160: __E_bf_48(127)       => bf(21) _(11) '+' _(11) bf(21).
	p(NT(127), (NT(21)+NT(11)+T(36)+NT(11)+NT(21)));
//G161: bf_xor(126)          => __E_bf_48(127).
	p(NT(126), (NT(127)));
//G162: bf(21)               => bf_xor(126).
	p(NT(21), (NT(126)));
//G163: __E___E_bf_49_50(130) => _(11) ':' _(11) type(131).
	p(NT(130), (NT(11)+T(2)+NT(11)+NT(131)));
//G164: __E___E_bf_49_50(130) => null.
	p(NT(130), (nul));
//G165: __E_bf_49(129)       => '1' __E___E_bf_49_50(130).
	p(NT(129), (T(37)+NT(130)));
//G166: bf_t(128)            => __E_bf_49(129).
	p(NT(128), (NT(129)));
//G167: bf(21)               => bf_t(128).
	p(NT(21), (NT(128)));
//G168: __E___E_bf_51_52(134) => _(11) ':' _(11) type(131).
	p(NT(134), (NT(11)+T(2)+NT(11)+NT(131)));
//G169: __E___E_bf_51_52(134) => null.
	p(NT(134), (nul));
//G170: __E_bf_51(133)       => '0' __E___E_bf_51_52(134).
	p(NT(133), (T(38)+NT(134)));
//G171: bf_f(132)            => __E_bf_51(133).
	p(NT(132), (NT(133)));
//G172: bf(21)               => bf_f(132).
	p(NT(21), (NT(132)));
//G173: __E___E_bf_53_54(137) => _(11) '&' _(11).
	p(NT(137), (NT(11)+T(31)+NT(11)));
//G174: __E___E_bf_53_54(137) => __(27).
	p(NT(137), (NT(27)));
//G175: __E_bf_53(136)       => bf(21) __E___E_bf_53_54(137) bf(21).
	p(NT(136), (NT(21)+NT(137)+NT(21)));
//G176: bf_and(135)          => __E_bf_53(136).
	p(NT(135), (NT(136)));
//G177: bf(21)               => bf_and(135).
	p(NT(21), (NT(135)));
//G178: bf_and(135)          => bf_and_nosep(138).
	p(NT(135), (NT(138)));
//G179: bf(21)               => bf_and(135).
	p(NT(21), (NT(135)));
//G180: __E_bf_55(140)       => bf(21) _(11) '\''.
	p(NT(140), (NT(21)+NT(11)+T(39)));
//G181: bf_neg(139)          => __E_bf_55(140).
	p(NT(139), (NT(140)));
//G182: bf(21)               => bf_neg(139).
	p(NT(21), (NT(139)));
//G183: bf(21)               => capture(20).
	p(NT(21), (NT(20)));
//G184: __N_0(405)           => bf_constant(119).
	p(NT(405), (NT(119)));
//G185: __N_1(406)           => bf_t(128).
	p(NT(406), (NT(128)));
//G186: __N_2(407)           => bf_f(132).
	p(NT(407), (NT(132)));
//G187: __N_3(408)           => bf_and(135).
	p(NT(408), (NT(135)));
//G188: __E_bf_and_nosep_56(141) => bf(21) & ~( __N_0(405) ) & ~( __N_1(406) ) & ~( __N_2(407) ) & ~( __N_3(408) ).	 # conjunctive
	p(NT(141), (NT(21)) & ~(NT(405)) & ~(NT(406)) & ~(NT(407)) & ~(NT(408)));
//G189: bf_and_nosep(138)    => __E_bf_and_nosep_56(141) bf(21).
	p(NT(138), (NT(141)+NT(21)));
//G190: __E_constraint_57(143) => '[' ctnvar(144) _(11) '!' '=' _(11) num(145) ']'.
	p(NT(143), (T(24)+NT(144)+NT(11)+T(32)+T(3)+NT(11)+NT(145)+T(25)));
//G191: __E_constraint_57(143) => '[' num(145) _(11) '!' '=' _(11) ctnvar(144) ']'.
	p(NT(143), (T(24)+NT(145)+NT(11)+T(32)+T(3)+NT(11)+NT(144)+T(25)));
//G192: ctn_neq(142)         => __E_constraint_57(143).
	p(NT(142), (NT(143)));
//G193: constraint(80)       => ctn_neq(142).
	p(NT(80), (NT(142)));
//G194: __E_constraint_58(147) => '[' ctnvar(144) _(11) '=' _(11) num(145) ']'.
	p(NT(147), (T(24)+NT(144)+NT(11)+T(3)+NT(11)+NT(145)+T(25)));
//G195: __E_constraint_58(147) => '[' num(145) _(11) '=' _(11) ctnvar(144) ']'.
	p(NT(147), (T(24)+NT(145)+NT(11)+T(3)+NT(11)+NT(144)+T(25)));
//G196: ctn_eq(146)          => __E_constraint_58(147).
	p(NT(146), (NT(147)));
//G197: constraint(80)       => ctn_eq(146).
	p(NT(80), (NT(146)));
//G198: __E_constraint_59(149) => '[' ctnvar(144) _(11) '>' '=' _(11) num(145) ']'.
	p(NT(149), (T(24)+NT(144)+NT(11)+T(21)+T(3)+NT(11)+NT(145)+T(25)));
//G199: __E_constraint_59(149) => '[' num(145) _(11) '>' '=' _(11) ctnvar(144) ']'.
	p(NT(149), (T(24)+NT(145)+NT(11)+T(21)+T(3)+NT(11)+NT(144)+T(25)));
//G200: ctn_greater_equal(148) => __E_constraint_59(149).
	p(NT(148), (NT(149)));
//G201: constraint(80)       => ctn_greater_equal(148).
	p(NT(80), (NT(148)));
//G202: __E_constraint_60(151) => '[' ctnvar(144) _(11) '>' _(11) num(145) ']'.
	p(NT(151), (T(24)+NT(144)+NT(11)+T(21)+NT(11)+NT(145)+T(25)));
//G203: __E_constraint_60(151) => '[' num(145) _(11) '>' _(11) ctnvar(144) ']'.
	p(NT(151), (T(24)+NT(145)+NT(11)+T(21)+NT(11)+NT(144)+T(25)));
//G204: ctn_greater(150)     => __E_constraint_60(151).
	p(NT(150), (NT(151)));
//G205: constraint(80)       => ctn_greater(150).
	p(NT(80), (NT(150)));
//G206: __E_constraint_61(153) => '[' ctnvar(144) _(11) '<' '=' _(11) num(145) ']'.
	p(NT(153), (T(24)+NT(144)+NT(11)+T(20)+T(3)+NT(11)+NT(145)+T(25)));
//G207: __E_constraint_61(153) => '[' num(145) _(11) '<' '=' _(11) ctnvar(144) ']'.
	p(NT(153), (T(24)+NT(145)+NT(11)+T(20)+T(3)+NT(11)+NT(144)+T(25)));
//G208: ctn_less_equal(152)  => __E_constraint_61(153).
	p(NT(152), (NT(153)));
//G209: constraint(80)       => ctn_less_equal(152).
	p(NT(80), (NT(152)));
//G210: __E_constraint_62(155) => '[' ctnvar(144) _(11) '<' _(11) num(145) ']'.
	p(NT(155), (T(24)+NT(144)+NT(11)+T(20)+NT(11)+NT(145)+T(25)));
//G211: __E_constraint_62(155) => '[' num(145) _(11) '<' _(11) ctnvar(144) ']'.
	p(NT(155), (T(24)+NT(145)+NT(11)+T(20)+NT(11)+NT(144)+T(25)));
//G212: ctn_less(154)        => __E_constraint_62(155).
	p(NT(154), (NT(155)));
//G213: constraint(80)       => ctn_less(154).
	p(NT(80), (NT(154)));
//G214: __E_variable_63(156) => null.
	p(NT(156), (nul));
//G215: __E_variable_63(156) => digit(3) __E_variable_63(156).
	p(NT(156), (NT(3)+NT(156)));
//G216: variable(120)        => alpha(5) __E_variable_63(156).	 # guarded: charvar
	p(NT(120), (NT(5)+NT(156)));
	p.back().guard = "charvar";
//G217: variable(120)        => chars(157).	 # guarded: var
	p(NT(120), (NT(157)));
	p.back().guard = "var";
//G218: __E_variable_64(159) => in(160).
	p(NT(159), (NT(160)));
//G219: __E_variable_64(159) => out(161).
	p(NT(159), (NT(161)));
//G220: io_var(158)          => __E_variable_64(159).
	p(NT(158), (NT(159)));
//G221: variable(120)        => io_var(158).
	p(NT(120), (NT(158)));
//G222: variable(120)        => uninterpreted_constant(162).
	p(NT(120), (NT(162)));
//G223: __E___E_in_65_66(165) => digit(3).
	p(NT(165), (NT(3)));
//G224: __E___E_in_65_66(165) => digit(3) __E___E_in_65_66(165).
	p(NT(165), (NT(3)+NT(165)));
//G225: __E_in_65(164)       => 'i' __E___E_in_65_66(165).
	p(NT(164), (T(10)+NT(165)));
//G226: in_var_name(163)     => __E_in_65(164).
	p(NT(163), (NT(164)));
//G227: in(160)              => in_var_name(163) '[' offset(166) ']'.
	p(NT(160), (NT(163)+T(24)+NT(166)+T(25)));
//G228: __E___E_out_67_68(169) => digit(3).
	p(NT(169), (NT(3)));
//G229: __E___E_out_67_68(169) => digit(3) __E___E_out_67_68(169).
	p(NT(169), (NT(3)+NT(169)));
//G230: __E_out_67(168)      => 'o' __E___E_out_67_68(169).
	p(NT(168), (T(17)+NT(169)));
//G231: out_var_name(167)    => __E_out_67(168).
	p(NT(167), (NT(168)));
//G232: out(161)             => out_var_name(167) '[' offset(166) ']'.
	p(NT(161), (NT(167)+T(24)+NT(166)+T(25)));
//G233: ctnvar(144)          => chars(157).
	p(NT(144), (NT(157)));
//G234: capture(20)          => '$' chars(157).
	p(NT(20), (T(40)+NT(157)));
//G235: __E_q_vars_69(171)   => _(11) ',' _(11) q_var(170).
	p(NT(171), (NT(11)+T(14)+NT(11)+NT(170)));
//G236: __E_q_vars_70(172)   => null.
	p(NT(172), (nul));
//G237: __E_q_vars_70(172)   => __E_q_vars_69(171) __E_q_vars_70(172).
	p(NT(172), (NT(171)+NT(172)));
//G238: q_vars(76)           => q_var(170) __E_q_vars_70(172).
	p(NT(76), (NT(170)+NT(172)));
//G239: q_var(170)           => capture(20).
	p(NT(170), (NT(20)));
//G240: __N_4(409)           => uninterpreted_constant(162).
	p(NT(409), (NT(162)));
//G241: q_var(170)           => variable(120) & ~( __N_4(409) ).	 # conjunctive
	p(NT(170), (NT(120)) & ~(NT(409)));
//G242: __E_offsets_71(173)  => _(11) ',' _(11) offset(166).
	p(NT(173), (NT(11)+T(14)+NT(11)+NT(166)));
//G243: __E_offsets_72(174)  => null.
	p(NT(174), (nul));
//G244: __E_offsets_72(174)  => __E_offsets_71(173) __E_offsets_72(174).
	p(NT(174), (NT(173)+NT(174)));
//G245: offsets(24)          => '[' _(11) offset(166) __E_offsets_72(174) _(11) ']'.
	p(NT(24), (T(24)+NT(11)+NT(166)+NT(174)+NT(11)+T(25)));
//G246: offset(166)          => num(145).
	p(NT(166), (NT(145)));
//G247: offset(166)          => capture(20).
	p(NT(166), (NT(20)));
//G248: offset(166)          => shift(175).
	p(NT(166), (NT(175)));
//G249: __N_5(410)           => io_var(158).
	p(NT(410), (NT(158)));
//G250: __E_offset_73(176)   => variable(120) & ~( __N_5(410) ).	 # conjunctive
	p(NT(176), (NT(120)) & ~(NT(410)));
//G251: offset(166)          => __E_offset_73(176).
	p(NT(166), (NT(176)));
//G252: __E_shift_74(177)    => capture(20).
	p(NT(177), (NT(20)));
//G253: __N_6(411)           => io_var(158).
	p(NT(411), (NT(158)));
//G254: __E___E_shift_74_75(178) => variable(120) & ~( __N_6(411) ).	 # conjunctive
	p(NT(178), (NT(120)) & ~(NT(411)));
//G255: __E_shift_74(177)    => __E___E_shift_74_75(178).
	p(NT(177), (NT(178)));
//G256: shift(175)           => __E_shift_74(177) _(11) '-' _(11) num(145).
	p(NT(175), (NT(177)+NT(11)+T(28)+NT(11)+NT(145)));
//G257: num(145)             => digits(179).
	p(NT(145), (NT(179)));
//G258: __E___E_uninterpreted_constant_76_77(182) => chars(157).
	p(NT(182), (NT(157)));
//G259: __E___E_uninterpreted_constant_76_77(182) => _(11).
	p(NT(182), (NT(11)));
//G260: __E_uninterpreted_constant_76(181) => __E___E_uninterpreted_constant_76_77(182) ':' chars(157).
	p(NT(181), (NT(182)+T(2)+NT(157)));
//G261: uninter_const_name(180) => __E_uninterpreted_constant_76(181).
	p(NT(180), (NT(181)));
//G262: uninterpreted_constant(162) => '<' uninter_const_name(180) _(11) '>'.
	p(NT(162), (T(20)+NT(180)+NT(11)+T(21)));
//G263: __E_bf_constant_78(184) => capture(20).
	p(NT(184), (NT(20)));
//G264: __E_bf_constant_78(184) => binding(185).
	p(NT(184), (NT(185)));
//G265: constant(183)        => __E_bf_constant_78(184).
	p(NT(183), (NT(184)));
//G266: type(131)            => chars(157).
	p(NT(131), (NT(157)));
//G267: __E_bf_constant_79(186) => _(11) ':' _(11) type(131).
	p(NT(186), (NT(11)+T(2)+NT(11)+NT(131)));
//G268: __E_bf_constant_79(186) => null.
	p(NT(186), (nul));
//G269: bf_constant(119)     => '{' _(11) constant(183) _(11) '}' __E_bf_constant_79(186).
	p(NT(119), (T(41)+NT(11)+NT(183)+NT(11)+T(42)+NT(186)));
//G270: __E___E___E_binding_80_81_82(191) => src_c(189).
	p(NT(191), (NT(189)));
//G271: __E___E___E_binding_80_81_82(191) => space(2).
	p(NT(191), (NT(2)));
//G272: __E___E___E_binding_80_81_83(192) => null.
	p(NT(192), (nul));
//G273: __E___E___E_binding_80_81_83(192) => __E___E___E_binding_80_81_82(191) __E___E___E_binding_80_81_83(192).
	p(NT(192), (NT(191)+NT(192)));
//G274: __E___E_binding_80_81(190) => __E___E___E_binding_80_81_83(192) src_c(189).
	p(NT(190), (NT(192)+NT(189)));
//G275: __E___E_binding_80_81(190) => null.
	p(NT(190), (nul));
//G276: __E_binding_80(188)  => src_c(189) __E___E_binding_80_81(190).
	p(NT(188), (NT(189)+NT(190)));
//G277: source(187)          => __E_binding_80(188).
	p(NT(187), (NT(188)));
//G278: binding(185)         => source(187).
	p(NT(185), (NT(187)));
//G279: src_c(189)           => alnum(6).
	p(NT(189), (NT(6)));
//G280: __N_7(412)           => '{'.
	p(NT(412), (T(41)));
//G281: __N_8(413)           => '}'.
	p(NT(413), (T(42)));
//G282: src_c(189)           => ~( __N_7(412) ) & ~( __N_8(413) ) & punct(7).	 # conjunctive
	p(NT(189), ~(NT(412)) & ~(NT(413)) & (NT(7)));
//G283: __E_src_c_84(193)    => src_c(189).
	p(NT(193), (NT(189)));
//G284: __E_src_c_84(193)    => space(2).
	p(NT(193), (NT(2)));
//G285: __E_src_c_85(194)    => null.
	p(NT(194), (nul));
//G286: __E_src_c_85(194)    => __E_src_c_84(193) __E_src_c_85(194).
	p(NT(194), (NT(193)+NT(194)));
//G287: src_c(189)           => '{' __E_src_c_85(194) '}'.
	p(NT(189), (T(41)+NT(194)+T(42)));
//G288: __E_chars_86(195)    => alnum(6).
	p(NT(195), (NT(6)));
//G289: __E_chars_86(195)    => '_'.
	p(NT(195), (T(43)));
//G290: __E_chars_87(196)    => null.
	p(NT(196), (nul));
//G291: __E_chars_87(196)    => __E_chars_86(195) __E_chars_87(196).
	p(NT(196), (NT(195)+NT(196)));
//G292: chars(157)           => alpha(5) __E_chars_87(196).
	p(NT(157), (NT(5)+NT(196)));
//G293: __E_digits_88(197)   => digit(3).
	p(NT(197), (NT(3)));
//G294: __E_digits_88(197)   => digit(3) __E_digits_88(197).
	p(NT(197), (NT(3)+NT(197)));
//G295: digits(179)          => __E_digits_88(197).
	p(NT(179), (NT(197)));
//G296: sym(22)              => chars(157).
	p(NT(22), (NT(157)));
//G297: extra(198)           => chars(157).
	p(NT(198), (NT(157)));
//G298: __E_comment_89(200)  => printable(8).
	p(NT(200), (NT(8)));
//G299: __E_comment_89(200)  => '\t'.
	p(NT(200), (T(44)));
//G300: __E_comment_90(201)  => null.
	p(NT(201), (nul));
//G301: __E_comment_90(201)  => __E_comment_89(200) __E_comment_90(201).
	p(NT(201), (NT(200)+NT(201)));
//G302: __E_comment_91(202)  => '\n'.
	p(NT(202), (T(45)));
//G303: __E_comment_91(202)  => '\r'.
	p(NT(202), (T(46)));
//G304: __E_comment_91(202)  => eof(1).
	p(NT(202), (NT(1)));
//G305: comment(199)         => '#' __E_comment_90(201) __E_comment_91(202).
	p(NT(199), (T(47)+NT(201)+NT(202)));
//G306: __E____92(203)       => space(2).
	p(NT(203), (NT(2)));
//G307: __E____92(203)       => comment(199).
	p(NT(203), (NT(199)));
//G308: __(27)               => __E____92(203) _(11).
	p(NT(27), (NT(203)+NT(11)));
//G309: __E___93(204)        => __(27).
	p(NT(204), (NT(27)));
//G310: __E___93(204)        => null.
	p(NT(204), (nul));
//G311: _(11)                => __E___93(204).
	p(NT(11), (NT(204)));
//G312: bf_cb_arg(206)       => bf(21).
	p(NT(206), (NT(21)));
//G313: bf_cb_args1(205)     => __(27) bf_cb_arg(206).
	p(NT(205), (NT(27)+NT(206)));
//G314: bf_cb_args2(207)     => bf_cb_args1(205) bf_cb_args1(205).
	p(NT(207), (NT(205)+NT(205)));
//G315: bf_cb_args3(208)     => bf_cb_args2(207) bf_cb_args1(205).
	p(NT(208), (NT(207)+NT(205)));
//G316: bf_cb_args4(209)     => bf_cb_args3(208) bf_cb_args1(205).
	p(NT(209), (NT(208)+NT(205)));
//G317: wff_cb_arg(211)      => wff(14).
	p(NT(211), (NT(14)));
//G318: wff_cb_args1(210)    => __(27) wff_cb_arg(211).
	p(NT(210), (NT(27)+NT(211)));
//G319: wff_cb_args2(212)    => wff_cb_args1(210) wff_cb_args1(210).
	p(NT(212), (NT(210)+NT(210)));
//G320: wff_cb_args3(213)    => wff_cb_args2(212) wff_cb_args1(210).
	p(NT(213), (NT(212)+NT(210)));
//G321: wff_cb_args4(214)    => wff_cb_args3(213) wff_cb_args1(210).
	p(NT(214), (NT(213)+NT(210)));
//G322: __E_wff_cb_94(216)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(212).
	p(NT(216), (T(22)+T(4)+T(4)+T(43)+T(48)+T(5)+T(12)+T(43)+T(8)+T(6)+T(5)+T(12)+T(48)+T(10)+T(49)+T(50)+T(43)+T(12)+T(51)+T(7)+T(4)+T(17)+T(11)+T(18)+T(51)+T(6)+T(5)+T(12)+T(43)+T(8)+T(7)+NT(212)));
//G323: wff_has_clashing_subformulas_cb(215) => __E_wff_cb_94(216).
	p(NT(215), (NT(216)));
//G324: wff_cb(48)           => wff_has_clashing_subformulas_cb(215).
	p(NT(48), (NT(215)));
//G325: __E_wff_cb_95(218)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(213).
	p(NT(218), (T(22)+T(4)+T(4)+T(43)+T(48)+T(5)+T(12)+T(43)+T(12)+T(51)+T(7)+T(4)+T(17)+T(11)+T(18)+T(51)+T(6)+T(5)+T(43)+T(8)+T(7)+NT(213)));
//G326: wff_has_subformula_cb(217) => __E_wff_cb_95(218).
	p(NT(217), (NT(218)));
//G327: wff_cb(48)           => wff_has_subformula_cb(217).
	p(NT(48), (NT(217)));
//G328: __E_wff_cb_96(220)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(212).
	p(NT(220), (T(22)+T(4)+T(4)+T(43)+T(11)+T(19)+T(18)+T(17)+T(52)+T(19)+T(43)+T(19)+T(27)+T(10)+T(12)+T(13)+T(19)+T(49)+T(13)+T(10)+T(5)+T(6)+T(43)+T(8)+T(7)+NT(212)));
//G329: wff_remove_existential_cb(219) => __E_wff_cb_96(220).
	p(NT(219), (NT(220)));
//G330: wff_cb(48)           => wff_remove_existential_cb(219).
	p(NT(48), (NT(219)));
//G331: __E_bf_cb_97(222)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(208).
	p(NT(222), (T(7)+T(4)+T(43)+T(48)+T(5)+T(12)+T(43)+T(12)+T(51)+T(7)+T(4)+T(17)+T(11)+T(18)+T(51)+T(6)+T(5)+T(43)+T(8)+T(7)+NT(208)));
//G332: bf_has_subformula_cb(221) => __E_bf_cb_97(222).
	p(NT(221), (NT(222)));
//G333: bf_cb(52)            => bf_has_subformula_cb(221).
	p(NT(52), (NT(221)));
//G334: __E_bf_cb_98(224)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(209).
	p(NT(224), (T(7)+T(4)+T(43)+T(11)+T(19)+T(18)+T(17)+T(52)+T(19)+T(43)+T(4)+T(51)+T(49)+T(10)+T(52)+T(19)+T(11)+T(12)+T(5)+T(6)+T(43)+T(8)+T(7)+NT(209)));
//G335: bf_remove_funiversal_cb(223) => __E_bf_cb_98(224).
	p(NT(223), (NT(224)));
//G336: bf_cb(52)            => bf_remove_funiversal_cb(223).
	p(NT(52), (NT(223)));
//G337: __E_bf_cb_99(226)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(209).
	p(NT(226), (T(7)+T(4)+T(43)+T(11)+T(19)+T(18)+T(17)+T(52)+T(19)+T(43)+T(4)+T(19)+T(27)+T(10)+T(12)+T(13)+T(19)+T(49)+T(13)+T(10)+T(5)+T(6)+T(43)+T(8)+T(7)+NT(209)));
//G338: bf_remove_fexistential_cb(225) => __E_bf_cb_99(226).
	p(NT(225), (NT(226)));
//G339: bf_cb(52)            => bf_remove_fexistential_cb(225).
	p(NT(52), (NT(225)));
//G340: __E_bf_cb_100(228)   => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(205).
	p(NT(228), (T(7)+T(4)+T(43)+T(49)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(53)+T(19)+T(43)+T(8)+T(7)+NT(205)));
//G341: bf_normalize_cb(227) => __E_bf_cb_100(228).
	p(NT(227), (NT(228)));
//G342: bf_cb(52)            => bf_normalize_cb(227).
	p(NT(52), (NT(227)));
//G343: cli(229)             => _(11).
	p(NT(229), (NT(11)));
//G344: __E_cli_101(231)     => '.' _(11) cli_command(230) _(11).
	p(NT(231), (T(1)+NT(11)+NT(230)+NT(11)));
//G345: __E_cli_102(232)     => null.
	p(NT(232), (nul));
//G346: __E_cli_102(232)     => __E_cli_101(231) __E_cli_102(232).
	p(NT(232), (NT(231)+NT(232)));
//G347: cli(229)             => _(11) cli_command(230) _(11) __E_cli_102(232).
	p(NT(229), (NT(11)+NT(230)+NT(11)+NT(232)));
//G348: __E_cli_command_103(235) => 'q'.
	p(NT(235), (T(54)));
//G349: __E_cli_command_103(235) => 'q' 'u' 'i' 't'.
	p(NT(235), (T(54)+T(51)+T(10)+T(13)));
//G350: quit_sym(234)        => __E_cli_command_103(235).
	p(NT(234), (NT(235)));
//G351: quit_cmd(233)        => quit_sym(234).
	p(NT(233), (NT(234)));
//G352: cli_command(230)     => quit_cmd(233).
	p(NT(230), (NT(233)));
//G353: __E_cli_command_104(238) => 'v'.
	p(NT(238), (T(52)));
//G354: __E_cli_command_104(238) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(238), (T(52)+T(19)+T(11)+T(12)+T(10)+T(17)+T(49)));
//G355: version_sym(237)     => __E_cli_command_104(238).
	p(NT(237), (NT(238)));
//G356: version_cmd(236)     => version_sym(237).
	p(NT(236), (NT(237)));
//G357: cli_command(230)     => version_cmd(236).
	p(NT(230), (NT(236)));
//G358: __E_cli_command_105(241) => 'c'.
	p(NT(241), (T(8)));
//G359: __E_cli_command_105(241) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(241), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G360: clear_sym(240)       => __E_cli_command_105(241).
	p(NT(240), (NT(241)));
//G361: clear_cmd(239)       => clear_sym(240).
	p(NT(239), (NT(240)));
//G362: cli_command(230)     => clear_cmd(239).
	p(NT(230), (NT(239)));
//G363: __E___E_cli_command_106_107(245) => 'h'.
	p(NT(245), (T(48)));
//G364: __E___E_cli_command_106_107(245) => 'h' 'e' 'l' 'p'.
	p(NT(245), (T(48)+T(19)+T(6)+T(55)));
//G365: help_sym(244)        => __E___E_cli_command_106_107(245).
	p(NT(244), (NT(245)));
//G366: __E___E_cli_command_106_108(246) => __(27) help_arg(247).
	p(NT(246), (NT(27)+NT(247)));
//G367: __E___E_cli_command_106_108(246) => null.
	p(NT(246), (nul));
//G368: __E_cli_command_106(243) => help_sym(244) __E___E_cli_command_106_108(246).
	p(NT(243), (NT(244)+NT(246)));
//G369: help_cmd(242)        => __E_cli_command_106(243).
	p(NT(242), (NT(243)));
//G370: cli_command(230)     => help_cmd(242).
	p(NT(230), (NT(242)));
//G371: file_sym(250)        => 'f' 'i' 'l' 'e'.
	p(NT(250), (T(4)+T(10)+T(6)+T(19)));
//G372: __E_cli_command_109(249) => file_sym(250) __(27) q_string(251).
	p(NT(249), (NT(250)+NT(27)+NT(251)));
//G373: file_cmd(248)        => __E_cli_command_109(249).
	p(NT(248), (NT(249)));
//G374: cli_command(230)     => file_cmd(248).
	p(NT(230), (NT(248)));
//G375: valid_sym(254)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(254), (T(52)+T(5)+T(6)+T(10)+T(56)));
//G376: __E_cli_command_110(253) => valid_sym(254) __(27) normalize_cmd_arg(255).
	p(NT(253), (NT(254)+NT(27)+NT(255)));
//G377: valid_cmd(252)       => __E_cli_command_110(253).
	p(NT(252), (NT(253)));
//G378: cli_command(230)     => valid_cmd(252).
	p(NT(230), (NT(252)));
//G379: sat_sym(258)         => 's' 'a' 't'.
	p(NT(258), (T(12)+T(5)+T(13)));
//G380: __E_cli_command_111(257) => sat_sym(258) __(27) normalize_cmd_arg(255).
	p(NT(257), (NT(258)+NT(27)+NT(255)));
//G381: sat_cmd(256)         => __E_cli_command_111(257).
	p(NT(256), (NT(257)));
//G382: cli_command(230)     => sat_cmd(256).
	p(NT(230), (NT(256)));
//G383: unsat_sym(261)       => 'u' 'n' 's' 'a' 't'.
	p(NT(261), (T(51)+T(49)+T(12)+T(5)+T(13)));
//G384: __E_cli_command_112(260) => unsat_sym(261) __(27) normalize_cmd_arg(255).
	p(NT(260), (NT(261)+NT(27)+NT(255)));
//G385: unsat_cmd(259)       => __E_cli_command_112(260).
	p(NT(259), (NT(260)));
//G386: cli_command(230)     => unsat_cmd(259).
	p(NT(230), (NT(259)));
//G387: solve_sym(264)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(264), (T(12)+T(17)+T(6)+T(52)+T(19)));
//G388: __E___E_cli_command_113_114(265) => __(27) type(131).
	p(NT(265), (NT(27)+NT(131)));
//G389: __E___E_cli_command_113_114(265) => null.
	p(NT(265), (nul));
//G390: __E_cli_command_113(263) => solve_sym(264) __E___E_cli_command_113_114(265) __(27) solve_cmd_arg(266).
	p(NT(263), (NT(264)+NT(265)+NT(27)+NT(266)));
//G391: solve_cmd(262)       => __E_cli_command_113(263).
	p(NT(262), (NT(263)));
//G392: cli_command(230)     => solve_cmd(262).
	p(NT(230), (NT(262)));
//G393: __E___E_cli_command_115_116(270) => 'r'.
	p(NT(270), (T(11)));
//G394: __E___E_cli_command_115_116(270) => 'r' 'u' 'n'.
	p(NT(270), (T(11)+T(51)+T(49)));
//G395: run_sym(269)         => __E___E_cli_command_115_116(270).
	p(NT(269), (NT(270)));
//G396: __E___E_cli_command_115_117(271) => memory(272).
	p(NT(271), (NT(272)));
//G397: __E___E_cli_command_115_117(271) => wff(14).
	p(NT(271), (NT(14)));
//G398: __E_cli_command_115(268) => run_sym(269) __(27) __E___E_cli_command_115_117(271).
	p(NT(268), (NT(269)+NT(27)+NT(271)));
//G399: run_cmd(267)         => __E_cli_command_115(268).
	p(NT(267), (NT(268)));
//G400: cli_command(230)     => run_cmd(267).
	p(NT(230), (NT(267)));
//G401: __E___E_cli_command_118_119(276) => 'n'.
	p(NT(276), (T(49)));
//G402: __E___E_cli_command_118_119(276) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(276), (T(49)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(53)+T(19)));
//G403: normalize_sym(275)   => __E___E_cli_command_118_119(276).
	p(NT(275), (NT(276)));
//G404: __E_cli_command_118(274) => normalize_sym(275) __(27) normalize_cmd_arg(255).
	p(NT(274), (NT(275)+NT(27)+NT(255)));
//G405: normalize_cmd(273)   => __E_cli_command_118(274).
	p(NT(273), (NT(274)));
//G406: cli_command(230)     => normalize_cmd(273).
	p(NT(230), (NT(273)));
//G407: __E___E_cli_command_120_121(280) => 's'.
	p(NT(280), (T(12)));
//G408: __E___E_cli_command_120_121(280) => 's' 'u' 'b' 's' 't'.
	p(NT(280), (T(12)+T(51)+T(7)+T(12)+T(13)));
//G409: __E___E_cli_command_120_121(280) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(280), (T(12)+T(51)+T(7)+T(12)+T(13)+T(10)+T(13)+T(51)+T(13)+T(19)));
//G410: subst_sym(279)       => __E___E_cli_command_120_121(280).
	p(NT(279), (NT(280)));
//G411: __E_cli_command_120(278) => subst_sym(279) __(27) nf_cmd_arg(281) _(11) '[' _(11) nf_cmd_arg(281) _(11) '/' _(11) nf_cmd_arg(281) _(11) ']'.
	p(NT(278), (NT(279)+NT(27)+NT(281)+NT(11)+T(24)+NT(11)+NT(281)+NT(11)+T(57)+NT(11)+NT(281)+NT(11)+T(25)));
//G412: subst_cmd(277)       => __E_cli_command_120(278).
	p(NT(277), (NT(278)));
//G413: cli_command(230)     => subst_cmd(277).
	p(NT(230), (NT(277)));
//G414: __E___E_cli_command_122_123(285) => 'i'.
	p(NT(285), (T(10)));
//G415: __E___E_cli_command_122_123(285) => 'i' 'n' 's' 't'.
	p(NT(285), (T(10)+T(49)+T(12)+T(13)));
//G416: __E___E_cli_command_122_123(285) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(285), (T(10)+T(49)+T(12)+T(13)+T(5)+T(49)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G417: inst_sym(284)        => __E___E_cli_command_122_123(285).
	p(NT(284), (NT(285)));
//G418: __E_cli_command_122(283) => inst_sym(284) __(27) inst_args(286).
	p(NT(283), (NT(284)+NT(27)+NT(286)));
//G419: inst_cmd(282)        => __E_cli_command_122(283).
	p(NT(282), (NT(283)));
//G420: cli_command(230)     => inst_cmd(282).
	p(NT(230), (NT(282)));
//G421: dnf_sym(289)         => 'd' 'n' 'f'.
	p(NT(289), (T(56)+T(49)+T(4)));
//G422: __E_cli_command_124(288) => dnf_sym(289) __(27) nf_cmd_arg(281).
	p(NT(288), (NT(289)+NT(27)+NT(281)));
//G423: dnf_cmd(287)         => __E_cli_command_124(288).
	p(NT(287), (NT(288)));
//G424: cli_command(230)     => dnf_cmd(287).
	p(NT(230), (NT(287)));
//G425: cnf_sym(292)         => 'c' 'n' 'f'.
	p(NT(292), (T(8)+T(49)+T(4)));
//G426: __E_cli_command_125(291) => cnf_sym(292) __(27) nf_cmd_arg(281).
	p(NT(291), (NT(292)+NT(27)+NT(281)));
//G427: cnf_cmd(290)         => __E_cli_command_125(291).
	p(NT(290), (NT(291)));
//G428: cli_command(230)     => cnf_cmd(290).
	p(NT(230), (NT(290)));
//G429: anf_sym(295)         => 'a' 'n' 'f'.
	p(NT(295), (T(5)+T(49)+T(4)));
//G430: __E_cli_command_126(294) => anf_sym(295) __(27) nf_cmd_arg(281).
	p(NT(294), (NT(295)+NT(27)+NT(281)));
//G431: anf_cmd(293)         => __E_cli_command_126(294).
	p(NT(293), (NT(294)));
//G432: cli_command(230)     => anf_cmd(293).
	p(NT(230), (NT(293)));
//G433: nnf_sym(298)         => 'n' 'n' 'f'.
	p(NT(298), (T(49)+T(49)+T(4)));
//G434: __E_cli_command_127(297) => nnf_sym(298) __(27) nf_cmd_arg(281).
	p(NT(297), (NT(298)+NT(27)+NT(281)));
//G435: nnf_cmd(296)         => __E_cli_command_127(297).
	p(NT(296), (NT(297)));
//G436: cli_command(230)     => nnf_cmd(296).
	p(NT(230), (NT(296)));
//G437: pnf_sym(301)         => 'p' 'n' 'f'.
	p(NT(301), (T(55)+T(49)+T(4)));
//G438: __E_cli_command_128(300) => pnf_sym(301) __(27) nf_cmd_arg(281).
	p(NT(300), (NT(301)+NT(27)+NT(281)));
//G439: pnf_cmd(299)         => __E_cli_command_128(300).
	p(NT(299), (NT(300)));
//G440: cli_command(230)     => pnf_cmd(299).
	p(NT(230), (NT(299)));
//G441: mnf_sym(304)         => 'm' 'n' 'f'.
	p(NT(304), (T(18)+T(49)+T(4)));
//G442: __E_cli_command_129(303) => mnf_sym(304) __(27) nf_cmd_arg(281).
	p(NT(303), (NT(304)+NT(27)+NT(281)));
//G443: mnf_cmd(302)         => __E_cli_command_129(303).
	p(NT(302), (NT(303)));
//G444: cli_command(230)     => mnf_cmd(302).
	p(NT(230), (NT(302)));
//G445: snf_sym(307)         => 's' 'n' 'f'.
	p(NT(307), (T(12)+T(49)+T(4)));
//G446: __E_cli_command_130(306) => snf_sym(307) __(27) nf_cmd_arg(281).
	p(NT(306), (NT(307)+NT(27)+NT(281)));
//G447: snf_cmd(305)         => __E_cli_command_130(306).
	p(NT(305), (NT(306)));
//G448: cli_command(230)     => snf_cmd(305).
	p(NT(230), (NT(305)));
//G449: onf_sym(310)         => 'o' 'n' 'f'.
	p(NT(310), (T(17)+T(49)+T(4)));
//G450: __E_cli_command_131(309) => onf_sym(310) __(27) variable(120) __(27) onf_cmd_arg(311).
	p(NT(309), (NT(310)+NT(27)+NT(120)+NT(27)+NT(311)));
//G451: onf_cmd(308)         => __E_cli_command_131(309).
	p(NT(308), (NT(309)));
//G452: cli_command(230)     => onf_cmd(308).
	p(NT(230), (NT(308)));
//G453: __E___E_cli_command_132_133(315) => 'd' 'e' 'f' 's'.
	p(NT(315), (T(56)+T(19)+T(4)+T(12)));
//G454: __E___E_cli_command_132_133(315) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(315), (T(56)+T(19)+T(4)+T(10)+T(49)+T(10)+T(13)+T(10)+T(17)+T(49)+T(12)));
//G455: def_sym(314)         => __E___E_cli_command_132_133(315).
	p(NT(314), (NT(315)));
//G456: __E_cli_command_132(313) => def_sym(314).
	p(NT(313), (NT(314)));
//G457: def_list_cmd(312)    => __E_cli_command_132(313).
	p(NT(312), (NT(313)));
//G458: cli_command(230)     => def_list_cmd(312).
	p(NT(230), (NT(312)));
//G459: __E_cli_command_134(317) => def_sym(314) __(27) number(318).
	p(NT(317), (NT(314)+NT(27)+NT(318)));
//G460: def_print_cmd(316)   => __E_cli_command_134(317).
	p(NT(316), (NT(317)));
//G461: cli_command(230)     => def_print_cmd(316).
	p(NT(230), (NT(316)));
//G462: def_rr_cmd(319)      => rec_relation(16).
	p(NT(319), (NT(16)));
//G463: cli_command(230)     => def_rr_cmd(319).
	p(NT(230), (NT(319)));
//G464: qelim_sym(322)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(322), (T(54)+T(19)+T(6)+T(10)+T(18)));
//G465: __E_cli_command_135(321) => qelim_sym(322) __(27) wff_cmd_arg(323).
	p(NT(321), (NT(322)+NT(27)+NT(323)));
//G466: qelim_cmd(320)       => __E_cli_command_135(321).
	p(NT(320), (NT(321)));
//G467: cli_command(230)     => qelim_cmd(320).
	p(NT(230), (NT(320)));
//G468: get_sym(326)         => 'g' 'e' 't'.
	p(NT(326), (T(50)+T(19)+T(13)));
//G469: __E___E_cli_command_136_137(327) => __(27) option(328).
	p(NT(327), (NT(27)+NT(328)));
//G470: __E___E_cli_command_136_137(327) => null.
	p(NT(327), (nul));
//G471: __E_cli_command_136(325) => get_sym(326) __E___E_cli_command_136_137(327).
	p(NT(325), (NT(326)+NT(327)));
//G472: get_cmd(324)         => __E_cli_command_136(325).
	p(NT(324), (NT(325)));
//G473: cli_command(230)     => get_cmd(324).
	p(NT(230), (NT(324)));
//G474: set_sym(331)         => 's' 'e' 't'.
	p(NT(331), (T(12)+T(19)+T(13)));
//G475: __E___E_cli_command_138_139(332) => __(27).
	p(NT(332), (NT(27)));
//G476: __E___E_cli_command_138_139(332) => _(11) '=' _(11).
	p(NT(332), (NT(11)+T(3)+NT(11)));
//G477: __E_cli_command_138(330) => set_sym(331) __(27) option(328) __E___E_cli_command_138_139(332) option_value(333).
	p(NT(330), (NT(331)+NT(27)+NT(328)+NT(332)+NT(333)));
//G478: set_cmd(329)         => __E_cli_command_138(330).
	p(NT(329), (NT(330)));
//G479: cli_command(230)     => set_cmd(329).
	p(NT(230), (NT(329)));
//G480: enable_sym(336)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(336), (T(19)+T(49)+T(5)+T(7)+T(6)+T(19)));
//G481: __E_cli_command_140(335) => enable_sym(336) __(27) bool_option(337).
	p(NT(335), (NT(336)+NT(27)+NT(337)));
//G482: enable_cmd(334)      => __E_cli_command_140(335).
	p(NT(334), (NT(335)));
//G483: cli_command(230)     => enable_cmd(334).
	p(NT(230), (NT(334)));
//G484: disable_sym(340)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(340), (T(56)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G485: __E_cli_command_141(339) => disable_sym(340) __(27) bool_option(337).
	p(NT(339), (NT(340)+NT(27)+NT(337)));
//G486: disable_cmd(338)     => __E_cli_command_141(339).
	p(NT(338), (NT(339)));
//G487: cli_command(230)     => disable_cmd(338).
	p(NT(230), (NT(338)));
//G488: toggle_sym(343)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(343), (T(13)+T(17)+T(50)+T(50)+T(6)+T(19)));
//G489: __E_cli_command_142(342) => toggle_sym(343) __(27) bool_option(337).
	p(NT(342), (NT(343)+NT(27)+NT(337)));
//G490: toggle_cmd(341)      => __E_cli_command_142(342).
	p(NT(341), (NT(342)));
//G491: cli_command(230)     => toggle_cmd(341).
	p(NT(230), (NT(341)));
//G492: __E_cli_command_143(345) => type(131) __(27) in_var_name(163) _(11) '=' _(11) input_stream(346).
	p(NT(345), (NT(131)+NT(27)+NT(163)+NT(11)+T(3)+NT(11)+NT(346)));
//G493: def_input_cmd(344)   => __E_cli_command_143(345).
	p(NT(344), (NT(345)));
//G494: cli_command(230)     => def_input_cmd(344).
	p(NT(230), (NT(344)));
//G495: __E_cli_command_144(348) => type(131) __(27) out_var_name(167) _(11) '=' _(11) output_stream(349).
	p(NT(348), (NT(131)+NT(27)+NT(167)+NT(11)+T(3)+NT(11)+NT(349)));
//G496: def_output_cmd(347)  => __E_cli_command_144(348).
	p(NT(347), (NT(348)));
//G497: cli_command(230)     => def_output_cmd(347).
	p(NT(230), (NT(347)));
//G498: __E___E_cli_command_145_146(353) => 'h' 'i' 's' 't'.
	p(NT(353), (T(48)+T(10)+T(12)+T(13)));
//G499: __E___E_cli_command_145_146(353) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(353), (T(48)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G500: history_sym(352)     => __E___E_cli_command_145_146(353).
	p(NT(352), (NT(353)));
//G501: __E_cli_command_145(351) => history_sym(352).
	p(NT(351), (NT(352)));
//G502: history_list_cmd(350) => __E_cli_command_145(351).
	p(NT(350), (NT(351)));
//G503: cli_command(230)     => history_list_cmd(350).
	p(NT(230), (NT(350)));
//G504: __E_cli_command_147(355) => history_sym(352) __(27) memory(272).
	p(NT(355), (NT(352)+NT(27)+NT(272)));
//G505: history_print_cmd(354) => __E_cli_command_147(355).
	p(NT(354), (NT(355)));
//G506: cli_command(230)     => history_print_cmd(354).
	p(NT(230), (NT(354)));
//G507: __E_cli_command_148(357) => wff(14).
	p(NT(357), (NT(14)));
//G508: __E_cli_command_148(357) => bf(21).
	p(NT(357), (NT(21)));
//G509: history_store_cmd(356) => __E_cli_command_148(357).
	p(NT(356), (NT(357)));
//G510: cli_command(230)     => history_store_cmd(356).
	p(NT(230), (NT(356)));
//G511: __E_input_stream_149(358) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(359) _(11) ')'.
	p(NT(358), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(359)+NT(11)+T(16)));
//G512: input_stream(346)    => __E_input_stream_149(358).
	p(NT(346), (NT(358)));
//G513: console_sym(360)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(360), (T(8)+T(17)+T(49)+T(12)+T(17)+T(6)+T(19)));
//G514: input_stream(346)    => console_sym(360).
	p(NT(346), (NT(360)));
//G515: __E_output_stream_150(361) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(359) _(11) ')'.
	p(NT(361), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(359)+NT(11)+T(16)));
//G516: output_stream(349)   => __E_output_stream_150(361).
	p(NT(349), (NT(361)));
//G517: output_stream(349)   => console_sym(360).
	p(NT(349), (NT(360)));
//G518: q_file_name(359)     => '"' file_name(362) '"'.
	p(NT(359), (T(58)+NT(362)+T(58)));
//G519: __E_file_name_151(363) => printable(8).
	p(NT(363), (NT(8)));
//G520: __E_file_name_151(363) => printable(8) __E_file_name_151(363).
	p(NT(363), (NT(8)+NT(363)));
//G521: file_name(362)       => __E_file_name_151(363).
	p(NT(362), (NT(363)));
//G522: number(318)          => digits(179).
	p(NT(318), (NT(179)));
//G523: bf_cmd_arg(364)      => memory(272).
	p(NT(364), (NT(272)));
//G524: bf_cmd_arg(364)      => bf(21).
	p(NT(364), (NT(21)));
//G525: wff_cmd_arg(323)     => memory(272).
	p(NT(323), (NT(272)));
//G526: wff_cmd_arg(323)     => wff(14).
	p(NT(323), (NT(14)));
//G527: solve_cmd_arg(266)   => memory(272).
	p(NT(266), (NT(272)));
//G528: solve_cmd_arg(266)   => wff(14).
	p(NT(266), (NT(14)));
//G529: nf_cmd_arg(281)      => memory(272).
	p(NT(281), (NT(272)));
//G530: nf_cmd_arg(281)      => ref(18).
	p(NT(281), (NT(18)));
//G531: nf_cmd_arg(281)      => wff(14).
	p(NT(281), (NT(14)));
//G532: nf_cmd_arg(281)      => bf(21).
	p(NT(281), (NT(21)));
//G533: onf_cmd_arg(311)     => memory(272).
	p(NT(311), (NT(272)));
//G534: onf_cmd_arg(311)     => wff(14).
	p(NT(311), (NT(14)));
//G535: normalize_cmd_arg(255) => memory(272).
	p(NT(255), (NT(272)));
//G536: normalize_cmd_arg(255) => rr(10).
	p(NT(255), (NT(10)));
//G537: normalize_cmd_arg(255) => ref(18).
	p(NT(255), (NT(18)));
//G538: normalize_cmd_arg(255) => wff(14).
	p(NT(255), (NT(14)));
//G539: normalize_cmd_arg(255) => bf(21).
	p(NT(255), (NT(21)));
//G540: inst_args(286)       => wff_cmd_arg(323) _(11) '[' _(11) variable(120) _(11) '/' _(11) bf_cmd_arg(364) _(11) ']'.
	p(NT(286), (NT(323)+NT(11)+T(24)+NT(11)+NT(120)+NT(11)+T(57)+NT(11)+NT(364)+NT(11)+T(25)));
//G541: inst_args(286)       => bf_cmd_arg(364) _(11) '[' _(11) variable(120) _(11) '/' _(11) bf_cmd_arg(364) _(11) ']'.
	p(NT(286), (NT(364)+NT(11)+T(24)+NT(11)+NT(120)+NT(11)+T(57)+NT(11)+NT(364)+NT(11)+T(25)));
//G542: help_arg(247)        => help_sym(244).
	p(NT(247), (NT(244)));
//G543: help_arg(247)        => version_sym(237).
	p(NT(247), (NT(237)));
//G544: help_arg(247)        => quit_sym(234).
	p(NT(247), (NT(234)));
//G545: help_arg(247)        => clear_sym(240).
	p(NT(247), (NT(240)));
//G546: help_arg(247)        => get_sym(326).
	p(NT(247), (NT(326)));
//G547: help_arg(247)        => set_sym(331).
	p(NT(247), (NT(331)));
//G548: help_arg(247)        => enable_sym(336).
	p(NT(247), (NT(336)));
//G549: help_arg(247)        => disable_sym(340).
	p(NT(247), (NT(340)));
//G550: help_arg(247)        => toggle_sym(343).
	p(NT(247), (NT(343)));
//G551: help_arg(247)        => file_sym(250).
	p(NT(247), (NT(250)));
//G552: help_arg(247)        => history_sym(352).
	p(NT(247), (NT(352)));
//G553: help_arg(247)        => abs_memory_sym(365).
	p(NT(247), (NT(365)));
//G554: help_arg(247)        => rel_memory_sym(366).
	p(NT(247), (NT(366)));
//G555: help_arg(247)        => selection_sym(367).
	p(NT(247), (NT(367)));
//G556: help_arg(247)        => def_sym(314).
	p(NT(247), (NT(314)));
//G557: help_arg(247)        => inst_sym(284).
	p(NT(247), (NT(284)));
//G558: help_arg(247)        => subst_sym(279).
	p(NT(247), (NT(279)));
//G559: help_arg(247)        => normalize_sym(275).
	p(NT(247), (NT(275)));
//G560: help_arg(247)        => execute_sym(368).
	p(NT(247), (NT(368)));
//G561: help_arg(247)        => solve_sym(264).
	p(NT(247), (NT(264)));
//G562: help_arg(247)        => valid_sym(254).
	p(NT(247), (NT(254)));
//G563: help_arg(247)        => sat_sym(258).
	p(NT(247), (NT(258)));
//G564: help_arg(247)        => unsat_sym(261).
	p(NT(247), (NT(261)));
//G565: help_arg(247)        => run_sym(269).
	p(NT(247), (NT(269)));
//G566: help_arg(247)        => dnf_sym(289).
	p(NT(247), (NT(289)));
//G567: help_arg(247)        => cnf_sym(292).
	p(NT(247), (NT(292)));
//G568: help_arg(247)        => anf_sym(295).
	p(NT(247), (NT(295)));
//G569: help_arg(247)        => snf_sym(307).
	p(NT(247), (NT(307)));
//G570: help_arg(247)        => nnf_sym(298).
	p(NT(247), (NT(298)));
//G571: help_arg(247)        => pnf_sym(301).
	p(NT(247), (NT(301)));
//G572: help_arg(247)        => mnf_sym(304).
	p(NT(247), (NT(304)));
//G573: help_arg(247)        => onf_sym(310).
	p(NT(247), (NT(310)));
//G574: help_arg(247)        => qelim_sym(322).
	p(NT(247), (NT(322)));
//G575: __E___E_help_arg_152_153(371) => 's'.
	p(NT(371), (T(12)));
//G576: __E___E_help_arg_152_153(371) => null.
	p(NT(371), (nul));
//G577: __E_help_arg_152(370) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_152_153(371).
	p(NT(370), (T(19)+T(27)+T(5)+T(18)+T(55)+T(6)+T(19)+NT(371)));
//G578: examples_sym(369)    => __E_help_arg_152(370).
	p(NT(369), (NT(370)));
//G579: help_arg(247)        => examples_sym(369).
	p(NT(247), (NT(369)));
//G580: __E_memory_154(373)  => '%'.
	p(NT(373), (T(59)));
//G581: rel_memory_sym(366)  => '%' '-'.
	p(NT(366), (T(59)+T(28)));
//G582: memory_id(375)       => digits(179).
	p(NT(375), (NT(179)));
//G583: __E___E_memory_154_155(374) => memory_id(375).
	p(NT(374), (NT(375)));
//G584: __E___E_memory_154_155(374) => null.
	p(NT(374), (nul));
//G585: __E_memory_154(373)  => rel_memory_sym(366) __E___E_memory_154_155(374).
	p(NT(373), (NT(366)+NT(374)));
//G586: rel_memory(372)      => __E_memory_154(373).
	p(NT(372), (NT(373)));
//G587: memory(272)          => rel_memory(372).
	p(NT(272), (NT(372)));
//G588: abs_memory_sym(365)  => '%'.
	p(NT(365), (T(59)));
//G589: __E_memory_156(377)  => abs_memory_sym(365) memory_id(375).
	p(NT(377), (NT(365)+NT(375)));
//G590: abs_memory(376)      => __E_memory_156(377).
	p(NT(376), (NT(377)));
//G591: memory(272)          => abs_memory(376).
	p(NT(272), (NT(376)));
//G592: option(328)          => bool_option(337).
	p(NT(328), (NT(337)));
//G593: __E_option_157(380)  => 'S'.
	p(NT(380), (T(35)));
//G594: __E_option_157(380)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(380), (T(12)+T(19)+T(52)+T(19)+T(11)+T(10)+T(13)+T(23)));
//G595: severity_opt(379)    => __E_option_157(380).
	p(NT(379), (NT(380)));
//G596: enum_option(378)     => severity_opt(379).
	p(NT(378), (NT(379)));
//G597: option(328)          => enum_option(378).
	p(NT(328), (NT(378)));
//G598: __E_bool_option_158(382) => 's'.
	p(NT(382), (T(12)));
//G599: __E_bool_option_158(382) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(382), (T(12)+T(13)+T(5)+T(13)+T(51)+T(12)));
//G600: status_opt(381)      => __E_bool_option_158(382).
	p(NT(381), (NT(382)));
//G601: bool_option(337)     => status_opt(381).
	p(NT(337), (NT(381)));
//G602: __E_bool_option_159(384) => 'c'.
	p(NT(384), (T(8)));
//G603: __E_bool_option_159(384) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(384), (T(8)+T(17)+T(6)+T(17)+T(11)));
//G604: __E_bool_option_159(384) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(384), (T(8)+T(17)+T(6)+T(17)+T(11)+T(12)));
//G605: colors_opt(383)      => __E_bool_option_159(384).
	p(NT(383), (NT(384)));
//G606: bool_option(337)     => colors_opt(383).
	p(NT(337), (NT(383)));
//G607: __E_bool_option_160(386) => 'V'.
	p(NT(386), (T(60)));
//G608: __E_bool_option_160(386) => 'c' 'h' 'a' 'r' 'v' 'a' 'r'.
	p(NT(386), (T(8)+T(48)+T(5)+T(11)+T(52)+T(5)+T(11)));
//G609: charvar_opt(385)     => __E_bool_option_160(386).
	p(NT(385), (NT(386)));
//G610: bool_option(337)     => charvar_opt(385).
	p(NT(337), (NT(385)));
//G611: __E_bool_option_161(388) => 'H'.
	p(NT(388), (T(61)));
//G612: __E_bool_option_161(388) => 'h' 'i' 'g' 'h' 'l' 'i' 'g' 'h' 't'.
	p(NT(388), (T(48)+T(10)+T(50)+T(48)+T(6)+T(10)+T(50)+T(48)+T(13)));
//G613: __E_bool_option_161(388) => 'h' 'i' 'g' 'h' 'l' 'i' 'g' 'h' 't' 'i' 'n' 'g'.
	p(NT(388), (T(48)+T(10)+T(50)+T(48)+T(6)+T(10)+T(50)+T(48)+T(13)+T(10)+T(49)+T(50)));
//G614: highlighting_opt(387) => __E_bool_option_161(388).
	p(NT(387), (NT(388)));
//G615: bool_option(337)     => highlighting_opt(387).
	p(NT(337), (NT(387)));
//G616: __E_bool_option_162(390) => 'I'.
	p(NT(390), (T(62)));
//G617: __E_bool_option_162(390) => 'i' 'n' 'd' 'e' 'n' 't'.
	p(NT(390), (T(10)+T(49)+T(56)+T(19)+T(49)+T(13)));
//G618: __E_bool_option_162(390) => 'i' 'n' 'd' 'e' 'n' 't' 'i' 'n' 'g'.
	p(NT(390), (T(10)+T(49)+T(56)+T(19)+T(49)+T(13)+T(10)+T(49)+T(50)));
//G619: indenting_opt(389)   => __E_bool_option_162(390).
	p(NT(389), (NT(390)));
//G620: bool_option(337)     => indenting_opt(389).
	p(NT(337), (NT(389)));
//G621: __E_bool_option_163(392) => 'd'.
	p(NT(392), (T(56)));
//G622: __E_bool_option_163(392) => 'd' 'b' 'g'.
	p(NT(392), (T(56)+T(7)+T(50)));
//G623: __E_bool_option_163(392) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(392), (T(56)+T(19)+T(7)+T(51)+T(50)));
//G624: debug_repl_opt(391)  => __E_bool_option_163(392).
	p(NT(391), (NT(392)));
//G625: bool_option(337)     => debug_repl_opt(391).
	p(NT(337), (NT(391)));
//G626: option_value(333)    => option_value_true(393).
	p(NT(333), (NT(393)));
//G627: option_value(333)    => option_value_false(394).
	p(NT(333), (NT(394)));
//G628: option_value(333)    => severity(395).
	p(NT(333), (NT(395)));
//G629: option_value_true(393) => 't'.
	p(NT(393), (T(13)));
//G630: option_value_true(393) => 't' 'r' 'u' 'e'.
	p(NT(393), (T(13)+T(11)+T(51)+T(19)));
//G631: option_value_true(393) => 'o' 'n'.
	p(NT(393), (T(17)+T(49)));
//G632: option_value_true(393) => '1'.
	p(NT(393), (T(37)));
//G633: option_value_true(393) => 'y'.
	p(NT(393), (T(23)));
//G634: option_value_true(393) => 'y' 'e' 's'.
	p(NT(393), (T(23)+T(19)+T(12)));
//G635: option_value_false(394) => 'f'.
	p(NT(394), (T(4)));
//G636: option_value_false(394) => 'f' 'a' 'l' 's' 'e'.
	p(NT(394), (T(4)+T(5)+T(6)+T(12)+T(19)));
//G637: option_value_false(394) => 'o' 'f' 'f'.
	p(NT(394), (T(17)+T(4)+T(4)));
//G638: option_value_false(394) => '0'.
	p(NT(394), (T(38)));
//G639: option_value_false(394) => 'n'.
	p(NT(394), (T(49)));
//G640: option_value_false(394) => 'n' 'o'.
	p(NT(394), (T(49)+T(17)));
//G641: __E_severity_164(397) => 'e'.
	p(NT(397), (T(19)));
//G642: __E_severity_164(397) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(397), (T(19)+T(11)+T(11)+T(17)+T(11)));
//G643: error_sym(396)       => __E_severity_164(397).
	p(NT(396), (NT(397)));
//G644: severity(395)        => error_sym(396).
	p(NT(395), (NT(396)));
//G645: __E_severity_165(399) => 'i'.
	p(NT(399), (T(10)));
//G646: __E_severity_165(399) => 'i' 'n' 'f' 'o'.
	p(NT(399), (T(10)+T(49)+T(4)+T(17)));
//G647: info_sym(398)        => __E_severity_165(399).
	p(NT(398), (NT(399)));
//G648: severity(395)        => info_sym(398).
	p(NT(395), (NT(398)));
//G649: __E_severity_166(401) => 'd'.
	p(NT(401), (T(56)));
//G650: __E_severity_166(401) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(401), (T(56)+T(19)+T(7)+T(51)+T(50)));
//G651: debug_sym(400)       => __E_severity_166(401).
	p(NT(400), (NT(401)));
//G652: severity(395)        => debug_sym(400).
	p(NT(395), (NT(400)));
//G653: __E_severity_167(403) => 't'.
	p(NT(403), (T(13)));
//G654: __E_severity_167(403) => 't' 'r' 'a' 'c' 'e'.
	p(NT(403), (T(13)+T(11)+T(5)+T(8)+T(19)));
//G655: trace_sym(402)       => __E_severity_167(403).
	p(NT(402), (NT(403)));
//G656: severity(395)        => trace_sym(402).
	p(NT(395), (NT(402)));
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
		capture, bf, sym, __E_ref_3, offsets, ref_args, __E_ref_4, __, fp_fallback, first_sym, 
		last_sym, __E_ref_args_5, __E___E_ref_args_5_6, ref_arg, __E___E_ref_args_5_7, __E___E_ref_args_5_8, library, rules, __E_library_9, __E___E_library_9_10, 
		rule, __E___E___E_library_9_10_11, wff_rule, bf_rule, __E___E_library_9_12, wff_matcher, wff_body, __E_wff_rule_13, wff_cb, bf_matcher, 
		bf_body, __E_bf_rule_14, bf_cb, builder, builder_head, builder_body, __E_builder_head_15, __E_builder_head_16, bf_builder_body, __E_builder_body_17, 
		wff_builder_body, __E_builder_body_18, tau_constant_source, __E_tau_constant_source_19, wff_parenthesis, __E_wff_20, wff_sometimes, __E_wff_21, __E___E_wff_21_22, wff_always, 
		__E_wff_23, __E___E_wff_23_24, wff_conditional, __E_wff_25, wff_all, __E_wff_26, q_vars, wff_ex, __E_wff_27, wff_ref, 
		constraint, wff_imply, __E_wff_28, wff_equiv, __E_wff_29, wff_or, __E_wff_30, wff_xor, __E_wff_31, wff_and, 
		__E_wff_32, wff_neg, __E_wff_33, wff_t, wff_f, bf_interval, __E_wff_34, bf_eq, __E_wff_35, bf_neq, 
		__E_wff_36, bf_less_equal, __E_wff_37, bf_nleq, __E_wff_38, bf_greater, __E_wff_39, bf_ngreater, __E_wff_40, bf_greater_equal, 
		__E_wff_41, bf_ngeq, __E_wff_42, bf_less, __E_wff_43, bf_nless, __E_wff_44, bf_parenthesis, __E_bf_45, bf_constant, 
		variable, bf_splitter, __E_bf_46, bf_ref, bf_or, __E_bf_47, bf_xor, __E_bf_48, bf_t, __E_bf_49, 
		__E___E_bf_49_50, type, bf_f, __E_bf_51, __E___E_bf_51_52, bf_and, __E_bf_53, __E___E_bf_53_54, bf_and_nosep, bf_neg, 
		__E_bf_55, __E_bf_and_nosep_56, ctn_neq, __E_constraint_57, ctnvar, num, ctn_eq, __E_constraint_58, ctn_greater_equal, __E_constraint_59, 
		ctn_greater, __E_constraint_60, ctn_less_equal, __E_constraint_61, ctn_less, __E_constraint_62, __E_variable_63, chars, io_var, __E_variable_64, 
		in, out, uninterpreted_constant, in_var_name, __E_in_65, __E___E_in_65_66, offset, out_var_name, __E_out_67, __E___E_out_67_68, 
		q_var, __E_q_vars_69, __E_q_vars_70, __E_offsets_71, __E_offsets_72, shift, __E_offset_73, __E_shift_74, __E___E_shift_74_75, digits, 
		uninter_const_name, __E_uninterpreted_constant_76, __E___E_uninterpreted_constant_76_77, constant, __E_bf_constant_78, binding, __E_bf_constant_79, source, __E_binding_80, src_c, 
		__E___E_binding_80_81, __E___E___E_binding_80_81_82, __E___E___E_binding_80_81_83, __E_src_c_84, __E_src_c_85, __E_chars_86, __E_chars_87, __E_digits_88, extra, comment, 
		__E_comment_89, __E_comment_90, __E_comment_91, __E____92, __E___93, bf_cb_args1, bf_cb_arg, bf_cb_args2, bf_cb_args3, bf_cb_args4, 
		wff_cb_args1, wff_cb_arg, wff_cb_args2, wff_cb_args3, wff_cb_args4, wff_has_clashing_subformulas_cb, __E_wff_cb_94, wff_has_subformula_cb, __E_wff_cb_95, wff_remove_existential_cb, 
		__E_wff_cb_96, bf_has_subformula_cb, __E_bf_cb_97, bf_remove_funiversal_cb, __E_bf_cb_98, bf_remove_fexistential_cb, __E_bf_cb_99, bf_normalize_cb, __E_bf_cb_100, cli, 
		cli_command, __E_cli_101, __E_cli_102, quit_cmd, quit_sym, __E_cli_command_103, version_cmd, version_sym, __E_cli_command_104, clear_cmd, 
		clear_sym, __E_cli_command_105, help_cmd, __E_cli_command_106, help_sym, __E___E_cli_command_106_107, __E___E_cli_command_106_108, help_arg, file_cmd, __E_cli_command_109, 
		file_sym, q_string, valid_cmd, __E_cli_command_110, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_111, sat_sym, unsat_cmd, 
		__E_cli_command_112, unsat_sym, solve_cmd, __E_cli_command_113, solve_sym, __E___E_cli_command_113_114, solve_cmd_arg, run_cmd, __E_cli_command_115, run_sym, 
		__E___E_cli_command_115_116, __E___E_cli_command_115_117, memory, normalize_cmd, __E_cli_command_118, normalize_sym, __E___E_cli_command_118_119, subst_cmd, __E_cli_command_120, subst_sym, 
		__E___E_cli_command_120_121, nf_cmd_arg, inst_cmd, __E_cli_command_122, inst_sym, __E___E_cli_command_122_123, inst_args, dnf_cmd, __E_cli_command_124, dnf_sym, 
		cnf_cmd, __E_cli_command_125, cnf_sym, anf_cmd, __E_cli_command_126, anf_sym, nnf_cmd, __E_cli_command_127, nnf_sym, pnf_cmd, 
		__E_cli_command_128, pnf_sym, mnf_cmd, __E_cli_command_129, mnf_sym, snf_cmd, __E_cli_command_130, snf_sym, onf_cmd, __E_cli_command_131, 
		onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_132, def_sym, __E___E_cli_command_132_133, def_print_cmd, __E_cli_command_134, number, def_rr_cmd, 
		qelim_cmd, __E_cli_command_135, qelim_sym, wff_cmd_arg, get_cmd, __E_cli_command_136, get_sym, __E___E_cli_command_136_137, option, set_cmd, 
		__E_cli_command_138, set_sym, __E___E_cli_command_138_139, option_value, enable_cmd, __E_cli_command_140, enable_sym, bool_option, disable_cmd, __E_cli_command_141, 
		disable_sym, toggle_cmd, __E_cli_command_142, toggle_sym, def_input_cmd, __E_cli_command_143, input_stream, def_output_cmd, __E_cli_command_144, output_stream, 
		history_list_cmd, __E_cli_command_145, history_sym, __E___E_cli_command_145_146, history_print_cmd, __E_cli_command_147, history_store_cmd, __E_cli_command_148, __E_input_stream_149, q_file_name, 
		console_sym, __E_output_stream_150, file_name, __E_file_name_151, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, execute_sym, examples_sym, 
		__E_help_arg_152, __E___E_help_arg_152_153, rel_memory, __E_memory_154, __E___E_memory_154_155, memory_id, abs_memory, __E_memory_156, enum_option, severity_opt, 
		__E_option_157, status_opt, __E_bool_option_158, colors_opt, __E_bool_option_159, charvar_opt, __E_bool_option_160, highlighting_opt, __E_bool_option_161, indenting_opt, 
		__E_bool_option_162, debug_repl_opt, __E_bool_option_163, option_value_true, option_value_false, severity, error_sym, __E_severity_164, info_sym, __E_severity_165, 
		debug_sym, __E_severity_166, trace_sym, __E_severity_167, named_binding, __N_0, __N_1, __N_2, __N_3, __N_4, 
		__N_5, __N_6, __N_7, __N_8, 
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
