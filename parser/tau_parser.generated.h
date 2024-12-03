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
	"variable", "bf_splitter", "__E_bf_46", "bf_ref", "bf_or", "__E_bf_47", "bf_xor", "__E_bf_48", "bf_and", "__E_bf_49", 
	"__E___E_bf_49_50", "__E___E___E_bf_49_50_51", "bf_and_nosep", "bf_neg", "__E_bf_52", "bf_t", "__E_bf_53", "__E___E_bf_53_54", "type", "bf_f", 
	"__E_bf_55", "__E___E_bf_55_56", "bf_and_nosep_1st_op", "__E_bf_and_nosep_57", "ctn_neq", "__E_constraint_58", "ctnvar", "num", "ctn_eq", "__E_constraint_59", 
	"ctn_greater_equal", "__E_constraint_60", "ctn_greater", "__E_constraint_61", "ctn_less_equal", "__E_constraint_62", "ctn_less", "__E_constraint_63", "__E_variable_64", "chars", 
	"io_var", "__E_variable_65", "in", "out", "uninterpreted_constant", "in_var_name", "__E_in_66", "__E___E_in_66_67", "offset", "out_var_name", 
	"__E_out_68", "__E___E_out_68_69", "q_var", "__E_q_vars_70", "__E_q_vars_71", "__E_offsets_72", "__E_offsets_73", "shift", "__E_offset_74", "__E_shift_75", 
	"__E___E_shift_75_76", "digits", "uninter_const_name", "__E_uninterpreted_constant_77", "__E___E_uninterpreted_constant_77_78", "constant", "__E_bf_constant_79", "binding", "__E_bf_constant_80", "source", 
	"__E_binding_81", "src_c", "__E___E_binding_81_82", "__E___E___E_binding_81_82_83", "__E___E___E_binding_81_82_84", "__E_src_c_85", "__E_src_c_86", "__E_chars_87", "__E_chars_88", "__E_digits_89", 
	"extra", "comment", "__E_comment_90", "__E_comment_91", "__E_comment_92", "__E____93", "__E___94", "bf_cb_args1", "bf_cb_arg", "bf_cb_args2", 
	"bf_cb_args3", "bf_cb_args4", "wff_cb_args1", "wff_cb_arg", "wff_cb_args2", "wff_cb_args3", "wff_cb_args4", "wff_has_clashing_subformulas_cb", "__E_wff_cb_95", "wff_has_subformula_cb", 
	"__E_wff_cb_96", "wff_remove_existential_cb", "__E_wff_cb_97", "bf_has_subformula_cb", "__E_bf_cb_98", "bf_remove_funiversal_cb", "__E_bf_cb_99", "bf_remove_fexistential_cb", "__E_bf_cb_100", "bf_normalize_cb", 
	"__E_bf_cb_101", "cli", "cli_command", "__E_cli_102", "__E_cli_103", "quit_cmd", "quit_sym", "__E_cli_command_104", "version_cmd", "version_sym", 
	"__E_cli_command_105", "clear_cmd", "clear_sym", "__E_cli_command_106", "help_cmd", "__E_cli_command_107", "help_sym", "__E___E_cli_command_107_108", "__E___E_cli_command_107_109", "help_arg", 
	"file_cmd", "__E_cli_command_110", "file_sym", "q_string", "valid_cmd", "__E_cli_command_111", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_112", 
	"sat_sym", "unsat_cmd", "__E_cli_command_113", "unsat_sym", "solve_cmd", "__E_cli_command_114", "solve_sym", "__E___E_cli_command_114_115", "solve_cmd_arg", "run_cmd", 
	"__E_cli_command_116", "run_sym", "__E___E_cli_command_116_117", "__E___E_cli_command_116_118", "memory", "normalize_cmd", "__E_cli_command_119", "normalize_sym", "__E___E_cli_command_119_120", "subst_cmd", 
	"__E_cli_command_121", "subst_sym", "__E___E_cli_command_121_122", "nf_cmd_arg", "inst_cmd", "__E_cli_command_123", "inst_sym", "__E___E_cli_command_123_124", "inst_args", "dnf_cmd", 
	"__E_cli_command_125", "dnf_sym", "cnf_cmd", "__E_cli_command_126", "cnf_sym", "anf_cmd", "__E_cli_command_127", "anf_sym", "nnf_cmd", "__E_cli_command_128", 
	"nnf_sym", "pnf_cmd", "__E_cli_command_129", "pnf_sym", "mnf_cmd", "__E_cli_command_130", "mnf_sym", "snf_cmd", "__E_cli_command_131", "snf_sym", 
	"onf_cmd", "__E_cli_command_132", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_133", "def_sym", "__E___E_cli_command_133_134", "def_print_cmd", "__E_cli_command_135", 
	"number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_136", "qelim_sym", "wff_cmd_arg", "get_cmd", "__E_cli_command_137", "get_sym", "__E___E_cli_command_137_138", 
	"option", "set_cmd", "__E_cli_command_139", "set_sym", "__E___E_cli_command_139_140", "option_value", "enable_cmd", "__E_cli_command_141", "enable_sym", "bool_option", 
	"disable_cmd", "__E_cli_command_142", "disable_sym", "toggle_cmd", "__E_cli_command_143", "toggle_sym", "def_input_cmd", "__E_cli_command_144", "input_stream", "def_output_cmd", 
	"__E_cli_command_145", "output_stream", "history_list_cmd", "__E_cli_command_146", "history_sym", "__E___E_cli_command_146_147", "history_print_cmd", "__E_cli_command_148", "history_store_cmd", "__E_cli_command_149", 
	"__E_input_stream_150", "q_file_name", "console_sym", "__E_output_stream_151", "file_name", "__E_file_name_152", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", 
	"execute_sym", "examples_sym", "__E_help_arg_153", "__E___E_help_arg_153_154", "rel_memory", "__E_memory_155", "__E___E_memory_155_156", "memory_id", "abs_memory", "__E_memory_157", 
	"enum_option", "severity_opt", "__E_option_158", "status_opt", "__E_bool_option_159", "colors_opt", "__E_bool_option_160", "charvar_opt", "__E_bool_option_161", "highlighting_opt", 
	"__E_bool_option_162", "indenting_opt", "__E_bool_option_163", "debug_repl_opt", "__E_bool_option_164", "option_value_true", "option_value_false", "severity", "error_sym", "__E_severity_165", 
	"info_sym", "__E_severity_166", "debug_sym", "__E_severity_167", "trace_sym", "__E_severity_168", "named_binding", "__N_0", "__N_1", "__N_2", 
	"__N_3", "__N_4", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'f', 'a', 'l', 'b', 'c', 
	'k', 'i', 'r', 's', 't', ',', '(', ')', 'o', 'm', 
	'e', '<', '>', 'w', 'y', '[', ']', '?', 'x', '-', 
	'|', '^', '&', '!', 'T', 'F', 'S', '+', '\'', '1', 
	'0', '$', '{', '}', '_', '\t', '\n', '\r', '#', 'h', 
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
			20, 22, 120, 138, 146, 165, 169, 181, 182, 189,
			364, 406
		},
		.to_inline = {
			{ 14, 64, 14 },
			{ 21, 117, 21 },
			{ 48 },
			{ 52 },
			{ 132 },
			{ 159 },
			{ 172 },
			{ 191 },
			{ 207 },
			{ 209 },
			{ 210 },
			{ 211 },
			{ 212 },
			{ 214 },
			{ 215 },
			{ 216 },
			{ 257 },
			{ 268 },
			{ 283 },
			{ 288 },
			{ 313 },
			{ 325 },
			{ 366 }
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
//G163: __E___E___E_bf_49_50_51(131) => _(11) '&' _(11).
	p(NT(131), (NT(11)+T(31)+NT(11)));
//G164: __E___E___E_bf_49_50_51(131) => __(27).
	p(NT(131), (NT(27)));
//G165: __E___E_bf_49_50(130) => bf(21) __E___E___E_bf_49_50_51(131) bf(21).
	p(NT(130), (NT(21)+NT(131)+NT(21)));
//G166: __E_bf_49(129)       => __E___E_bf_49_50(130).
	p(NT(129), (NT(130)));
//G167: __E_bf_49(129)       => bf_and_nosep(132).
	p(NT(129), (NT(132)));
//G168: bf_and(128)          => __E_bf_49(129).
	p(NT(128), (NT(129)));
//G169: bf(21)               => bf_and(128).
	p(NT(21), (NT(128)));
//G170: __E_bf_52(134)       => bf(21) _(11) '\''.
	p(NT(134), (NT(21)+NT(11)+T(37)));
//G171: bf_neg(133)          => __E_bf_52(134).
	p(NT(133), (NT(134)));
//G172: bf(21)               => bf_neg(133).
	p(NT(21), (NT(133)));
//G173: __E___E_bf_53_54(137) => _(11) ':' _(11) type(138).
	p(NT(137), (NT(11)+T(2)+NT(11)+NT(138)));
//G174: __E___E_bf_53_54(137) => null.
	p(NT(137), (nul));
//G175: __E_bf_53(136)       => '1' __E___E_bf_53_54(137).
	p(NT(136), (T(38)+NT(137)));
//G176: bf_t(135)            => __E_bf_53(136).
	p(NT(135), (NT(136)));
//G177: bf(21)               => bf_t(135).
	p(NT(21), (NT(135)));
//G178: __E___E_bf_55_56(141) => _(11) ':' _(11) type(138).
	p(NT(141), (NT(11)+T(2)+NT(11)+NT(138)));
//G179: __E___E_bf_55_56(141) => null.
	p(NT(141), (nul));
//G180: __E_bf_55(140)       => '0' __E___E_bf_55_56(141).
	p(NT(140), (T(39)+NT(141)));
//G181: bf_f(139)            => __E_bf_55(140).
	p(NT(139), (NT(140)));
//G182: bf(21)               => bf_f(139).
	p(NT(21), (NT(139)));
//G183: bf(21)               => capture(20).
	p(NT(21), (NT(20)));
//G184: __E_bf_and_nosep_57(143) => bf_parenthesis(117).
	p(NT(143), (NT(117)));
//G185: __E_bf_and_nosep_57(143) => variable(120).
	p(NT(143), (NT(120)));
//G186: __E_bf_and_nosep_57(143) => bf_splitter(121).
	p(NT(143), (NT(121)));
//G187: __E_bf_and_nosep_57(143) => bf_ref(123).
	p(NT(143), (NT(123)));
//G188: __E_bf_and_nosep_57(143) => bf_or(124).
	p(NT(143), (NT(124)));
//G189: __E_bf_and_nosep_57(143) => bf_xor(126).
	p(NT(143), (NT(126)));
//G190: __E_bf_and_nosep_57(143) => bf_neg(133).
	p(NT(143), (NT(133)));
//G191: __E_bf_and_nosep_57(143) => capture(20).
	p(NT(143), (NT(20)));
//G192: bf_and_nosep_1st_op(142) => __E_bf_and_nosep_57(143).
	p(NT(142), (NT(143)));
//G193: bf_and_nosep(132)    => bf_and_nosep_1st_op(142) bf(21).
	p(NT(132), (NT(142)+NT(21)));
//G194: __E_constraint_58(145) => '[' ctnvar(146) _(11) '!' '=' _(11) num(147) ']'.
	p(NT(145), (T(24)+NT(146)+NT(11)+T(32)+T(3)+NT(11)+NT(147)+T(25)));
//G195: __E_constraint_58(145) => '[' num(147) _(11) '!' '=' _(11) ctnvar(146) ']'.
	p(NT(145), (T(24)+NT(147)+NT(11)+T(32)+T(3)+NT(11)+NT(146)+T(25)));
//G196: ctn_neq(144)         => __E_constraint_58(145).
	p(NT(144), (NT(145)));
//G197: constraint(80)       => ctn_neq(144).
	p(NT(80), (NT(144)));
//G198: __E_constraint_59(149) => '[' ctnvar(146) _(11) '=' _(11) num(147) ']'.
	p(NT(149), (T(24)+NT(146)+NT(11)+T(3)+NT(11)+NT(147)+T(25)));
//G199: __E_constraint_59(149) => '[' num(147) _(11) '=' _(11) ctnvar(146) ']'.
	p(NT(149), (T(24)+NT(147)+NT(11)+T(3)+NT(11)+NT(146)+T(25)));
//G200: ctn_eq(148)          => __E_constraint_59(149).
	p(NT(148), (NT(149)));
//G201: constraint(80)       => ctn_eq(148).
	p(NT(80), (NT(148)));
//G202: __E_constraint_60(151) => '[' ctnvar(146) _(11) '>' '=' _(11) num(147) ']'.
	p(NT(151), (T(24)+NT(146)+NT(11)+T(21)+T(3)+NT(11)+NT(147)+T(25)));
//G203: __E_constraint_60(151) => '[' num(147) _(11) '>' '=' _(11) ctnvar(146) ']'.
	p(NT(151), (T(24)+NT(147)+NT(11)+T(21)+T(3)+NT(11)+NT(146)+T(25)));
//G204: ctn_greater_equal(150) => __E_constraint_60(151).
	p(NT(150), (NT(151)));
//G205: constraint(80)       => ctn_greater_equal(150).
	p(NT(80), (NT(150)));
//G206: __E_constraint_61(153) => '[' ctnvar(146) _(11) '>' _(11) num(147) ']'.
	p(NT(153), (T(24)+NT(146)+NT(11)+T(21)+NT(11)+NT(147)+T(25)));
//G207: __E_constraint_61(153) => '[' num(147) _(11) '>' _(11) ctnvar(146) ']'.
	p(NT(153), (T(24)+NT(147)+NT(11)+T(21)+NT(11)+NT(146)+T(25)));
//G208: ctn_greater(152)     => __E_constraint_61(153).
	p(NT(152), (NT(153)));
//G209: constraint(80)       => ctn_greater(152).
	p(NT(80), (NT(152)));
//G210: __E_constraint_62(155) => '[' ctnvar(146) _(11) '<' '=' _(11) num(147) ']'.
	p(NT(155), (T(24)+NT(146)+NT(11)+T(20)+T(3)+NT(11)+NT(147)+T(25)));
//G211: __E_constraint_62(155) => '[' num(147) _(11) '<' '=' _(11) ctnvar(146) ']'.
	p(NT(155), (T(24)+NT(147)+NT(11)+T(20)+T(3)+NT(11)+NT(146)+T(25)));
//G212: ctn_less_equal(154)  => __E_constraint_62(155).
	p(NT(154), (NT(155)));
//G213: constraint(80)       => ctn_less_equal(154).
	p(NT(80), (NT(154)));
//G214: __E_constraint_63(157) => '[' ctnvar(146) _(11) '<' _(11) num(147) ']'.
	p(NT(157), (T(24)+NT(146)+NT(11)+T(20)+NT(11)+NT(147)+T(25)));
//G215: __E_constraint_63(157) => '[' num(147) _(11) '<' _(11) ctnvar(146) ']'.
	p(NT(157), (T(24)+NT(147)+NT(11)+T(20)+NT(11)+NT(146)+T(25)));
//G216: ctn_less(156)        => __E_constraint_63(157).
	p(NT(156), (NT(157)));
//G217: constraint(80)       => ctn_less(156).
	p(NT(80), (NT(156)));
//G218: __E_variable_64(158) => null.
	p(NT(158), (nul));
//G219: __E_variable_64(158) => digit(3) __E_variable_64(158).
	p(NT(158), (NT(3)+NT(158)));
//G220: variable(120)        => alpha(5) __E_variable_64(158).	 # guarded: charvar
	p(NT(120), (NT(5)+NT(158)));
	p.back().guard = "charvar";
//G221: variable(120)        => chars(159).	 # guarded: var
	p(NT(120), (NT(159)));
	p.back().guard = "var";
//G222: __E_variable_65(161) => in(162).
	p(NT(161), (NT(162)));
//G223: __E_variable_65(161) => out(163).
	p(NT(161), (NT(163)));
//G224: io_var(160)          => __E_variable_65(161).
	p(NT(160), (NT(161)));
//G225: variable(120)        => io_var(160).
	p(NT(120), (NT(160)));
//G226: variable(120)        => uninterpreted_constant(164).
	p(NT(120), (NT(164)));
//G227: __E___E_in_66_67(167) => digit(3).
	p(NT(167), (NT(3)));
//G228: __E___E_in_66_67(167) => digit(3) __E___E_in_66_67(167).
	p(NT(167), (NT(3)+NT(167)));
//G229: __E_in_66(166)       => 'i' __E___E_in_66_67(167).
	p(NT(166), (T(10)+NT(167)));
//G230: in_var_name(165)     => __E_in_66(166).
	p(NT(165), (NT(166)));
//G231: in(162)              => in_var_name(165) '[' offset(168) ']'.
	p(NT(162), (NT(165)+T(24)+NT(168)+T(25)));
//G232: __E___E_out_68_69(171) => digit(3).
	p(NT(171), (NT(3)));
//G233: __E___E_out_68_69(171) => digit(3) __E___E_out_68_69(171).
	p(NT(171), (NT(3)+NT(171)));
//G234: __E_out_68(170)      => 'o' __E___E_out_68_69(171).
	p(NT(170), (T(17)+NT(171)));
//G235: out_var_name(169)    => __E_out_68(170).
	p(NT(169), (NT(170)));
//G236: out(163)             => out_var_name(169) '[' offset(168) ']'.
	p(NT(163), (NT(169)+T(24)+NT(168)+T(25)));
//G237: ctnvar(146)          => chars(159).
	p(NT(146), (NT(159)));
//G238: capture(20)          => '$' chars(159).
	p(NT(20), (T(40)+NT(159)));
//G239: __E_q_vars_70(173)   => _(11) ',' _(11) q_var(172).
	p(NT(173), (NT(11)+T(14)+NT(11)+NT(172)));
//G240: __E_q_vars_71(174)   => null.
	p(NT(174), (nul));
//G241: __E_q_vars_71(174)   => __E_q_vars_70(173) __E_q_vars_71(174).
	p(NT(174), (NT(173)+NT(174)));
//G242: q_vars(76)           => q_var(172) __E_q_vars_71(174).
	p(NT(76), (NT(172)+NT(174)));
//G243: q_var(172)           => capture(20).
	p(NT(172), (NT(20)));
//G244: __N_0(407)           => uninterpreted_constant(164).
	p(NT(407), (NT(164)));
//G245: q_var(172)           => variable(120) & ~( __N_0(407) ).	 # conjunctive
	p(NT(172), (NT(120)) & ~(NT(407)));
//G246: __E_offsets_72(175)  => _(11) ',' _(11) offset(168).
	p(NT(175), (NT(11)+T(14)+NT(11)+NT(168)));
//G247: __E_offsets_73(176)  => null.
	p(NT(176), (nul));
//G248: __E_offsets_73(176)  => __E_offsets_72(175) __E_offsets_73(176).
	p(NT(176), (NT(175)+NT(176)));
//G249: offsets(24)          => '[' _(11) offset(168) __E_offsets_73(176) _(11) ']'.
	p(NT(24), (T(24)+NT(11)+NT(168)+NT(176)+NT(11)+T(25)));
//G250: offset(168)          => num(147).
	p(NT(168), (NT(147)));
//G251: offset(168)          => capture(20).
	p(NT(168), (NT(20)));
//G252: offset(168)          => shift(177).
	p(NT(168), (NT(177)));
//G253: __N_1(408)           => io_var(160).
	p(NT(408), (NT(160)));
//G254: __E_offset_74(178)   => variable(120) & ~( __N_1(408) ).	 # conjunctive
	p(NT(178), (NT(120)) & ~(NT(408)));
//G255: offset(168)          => __E_offset_74(178).
	p(NT(168), (NT(178)));
//G256: __E_shift_75(179)    => capture(20).
	p(NT(179), (NT(20)));
//G257: __N_2(409)           => io_var(160).
	p(NT(409), (NT(160)));
//G258: __E___E_shift_75_76(180) => variable(120) & ~( __N_2(409) ).	 # conjunctive
	p(NT(180), (NT(120)) & ~(NT(409)));
//G259: __E_shift_75(179)    => __E___E_shift_75_76(180).
	p(NT(179), (NT(180)));
//G260: shift(177)           => __E_shift_75(179) _(11) '-' _(11) num(147).
	p(NT(177), (NT(179)+NT(11)+T(28)+NT(11)+NT(147)));
//G261: num(147)             => digits(181).
	p(NT(147), (NT(181)));
//G262: __E___E_uninterpreted_constant_77_78(184) => chars(159).
	p(NT(184), (NT(159)));
//G263: __E___E_uninterpreted_constant_77_78(184) => _(11).
	p(NT(184), (NT(11)));
//G264: __E_uninterpreted_constant_77(183) => __E___E_uninterpreted_constant_77_78(184) ':' chars(159).
	p(NT(183), (NT(184)+T(2)+NT(159)));
//G265: uninter_const_name(182) => __E_uninterpreted_constant_77(183).
	p(NT(182), (NT(183)));
//G266: uninterpreted_constant(164) => '<' uninter_const_name(182) _(11) '>'.
	p(NT(164), (T(20)+NT(182)+NT(11)+T(21)));
//G267: __E_bf_constant_79(186) => capture(20).
	p(NT(186), (NT(20)));
//G268: __E_bf_constant_79(186) => binding(187).
	p(NT(186), (NT(187)));
//G269: constant(185)        => __E_bf_constant_79(186).
	p(NT(185), (NT(186)));
//G270: type(138)            => chars(159).
	p(NT(138), (NT(159)));
//G271: __E_bf_constant_80(188) => _(11) ':' _(11) type(138).
	p(NT(188), (NT(11)+T(2)+NT(11)+NT(138)));
//G272: __E_bf_constant_80(188) => null.
	p(NT(188), (nul));
//G273: bf_constant(119)     => '{' _(11) constant(185) _(11) '}' __E_bf_constant_80(188).
	p(NT(119), (T(41)+NT(11)+NT(185)+NT(11)+T(42)+NT(188)));
//G274: __E___E___E_binding_81_82_83(193) => src_c(191).
	p(NT(193), (NT(191)));
//G275: __E___E___E_binding_81_82_83(193) => space(2).
	p(NT(193), (NT(2)));
//G276: __E___E___E_binding_81_82_84(194) => null.
	p(NT(194), (nul));
//G277: __E___E___E_binding_81_82_84(194) => __E___E___E_binding_81_82_83(193) __E___E___E_binding_81_82_84(194).
	p(NT(194), (NT(193)+NT(194)));
//G278: __E___E_binding_81_82(192) => __E___E___E_binding_81_82_84(194) src_c(191).
	p(NT(192), (NT(194)+NT(191)));
//G279: __E___E_binding_81_82(192) => null.
	p(NT(192), (nul));
//G280: __E_binding_81(190)  => src_c(191) __E___E_binding_81_82(192).
	p(NT(190), (NT(191)+NT(192)));
//G281: source(189)          => __E_binding_81(190).
	p(NT(189), (NT(190)));
//G282: binding(187)         => source(189).
	p(NT(187), (NT(189)));
//G283: src_c(191)           => alnum(6).
	p(NT(191), (NT(6)));
//G284: __N_3(410)           => '{'.
	p(NT(410), (T(41)));
//G285: __N_4(411)           => '}'.
	p(NT(411), (T(42)));
//G286: src_c(191)           => ~( __N_3(410) ) & ~( __N_4(411) ) & punct(7).	 # conjunctive
	p(NT(191), ~(NT(410)) & ~(NT(411)) & (NT(7)));
//G287: __E_src_c_85(195)    => src_c(191).
	p(NT(195), (NT(191)));
//G288: __E_src_c_85(195)    => space(2).
	p(NT(195), (NT(2)));
//G289: __E_src_c_86(196)    => null.
	p(NT(196), (nul));
//G290: __E_src_c_86(196)    => __E_src_c_85(195) __E_src_c_86(196).
	p(NT(196), (NT(195)+NT(196)));
//G291: src_c(191)           => '{' __E_src_c_86(196) '}'.
	p(NT(191), (T(41)+NT(196)+T(42)));
//G292: __E_chars_87(197)    => alnum(6).
	p(NT(197), (NT(6)));
//G293: __E_chars_87(197)    => '_'.
	p(NT(197), (T(43)));
//G294: __E_chars_88(198)    => null.
	p(NT(198), (nul));
//G295: __E_chars_88(198)    => __E_chars_87(197) __E_chars_88(198).
	p(NT(198), (NT(197)+NT(198)));
//G296: chars(159)           => alpha(5) __E_chars_88(198).
	p(NT(159), (NT(5)+NT(198)));
//G297: __E_digits_89(199)   => digit(3).
	p(NT(199), (NT(3)));
//G298: __E_digits_89(199)   => digit(3) __E_digits_89(199).
	p(NT(199), (NT(3)+NT(199)));
//G299: digits(181)          => __E_digits_89(199).
	p(NT(181), (NT(199)));
//G300: sym(22)              => chars(159).
	p(NT(22), (NT(159)));
//G301: extra(200)           => chars(159).
	p(NT(200), (NT(159)));
//G302: __E_comment_90(202)  => printable(8).
	p(NT(202), (NT(8)));
//G303: __E_comment_90(202)  => '\t'.
	p(NT(202), (T(44)));
//G304: __E_comment_91(203)  => null.
	p(NT(203), (nul));
//G305: __E_comment_91(203)  => __E_comment_90(202) __E_comment_91(203).
	p(NT(203), (NT(202)+NT(203)));
//G306: __E_comment_92(204)  => '\n'.
	p(NT(204), (T(45)));
//G307: __E_comment_92(204)  => '\r'.
	p(NT(204), (T(46)));
//G308: __E_comment_92(204)  => eof(1).
	p(NT(204), (NT(1)));
//G309: comment(201)         => '#' __E_comment_91(203) __E_comment_92(204).
	p(NT(201), (T(47)+NT(203)+NT(204)));
//G310: __E____93(205)       => space(2).
	p(NT(205), (NT(2)));
//G311: __E____93(205)       => comment(201).
	p(NT(205), (NT(201)));
//G312: __(27)               => __E____93(205) _(11).
	p(NT(27), (NT(205)+NT(11)));
//G313: __E___94(206)        => __(27).
	p(NT(206), (NT(27)));
//G314: __E___94(206)        => null.
	p(NT(206), (nul));
//G315: _(11)                => __E___94(206).
	p(NT(11), (NT(206)));
//G316: bf_cb_arg(208)       => bf(21).
	p(NT(208), (NT(21)));
//G317: bf_cb_args1(207)     => __(27) bf_cb_arg(208).
	p(NT(207), (NT(27)+NT(208)));
//G318: bf_cb_args2(209)     => bf_cb_args1(207) bf_cb_args1(207).
	p(NT(209), (NT(207)+NT(207)));
//G319: bf_cb_args3(210)     => bf_cb_args2(209) bf_cb_args1(207).
	p(NT(210), (NT(209)+NT(207)));
//G320: bf_cb_args4(211)     => bf_cb_args3(210) bf_cb_args1(207).
	p(NT(211), (NT(210)+NT(207)));
//G321: wff_cb_arg(213)      => wff(14).
	p(NT(213), (NT(14)));
//G322: wff_cb_args1(212)    => __(27) wff_cb_arg(213).
	p(NT(212), (NT(27)+NT(213)));
//G323: wff_cb_args2(214)    => wff_cb_args1(212) wff_cb_args1(212).
	p(NT(214), (NT(212)+NT(212)));
//G324: wff_cb_args3(215)    => wff_cb_args2(214) wff_cb_args1(212).
	p(NT(215), (NT(214)+NT(212)));
//G325: wff_cb_args4(216)    => wff_cb_args3(215) wff_cb_args1(212).
	p(NT(216), (NT(215)+NT(212)));
//G326: __E_wff_cb_95(218)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(214).
	p(NT(218), (T(22)+T(4)+T(4)+T(43)+T(48)+T(5)+T(12)+T(43)+T(8)+T(6)+T(5)+T(12)+T(48)+T(10)+T(49)+T(50)+T(43)+T(12)+T(51)+T(7)+T(4)+T(17)+T(11)+T(18)+T(51)+T(6)+T(5)+T(12)+T(43)+T(8)+T(7)+NT(214)));
//G327: wff_has_clashing_subformulas_cb(217) => __E_wff_cb_95(218).
	p(NT(217), (NT(218)));
//G328: wff_cb(48)           => wff_has_clashing_subformulas_cb(217).
	p(NT(48), (NT(217)));
//G329: __E_wff_cb_96(220)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(215).
	p(NT(220), (T(22)+T(4)+T(4)+T(43)+T(48)+T(5)+T(12)+T(43)+T(12)+T(51)+T(7)+T(4)+T(17)+T(11)+T(18)+T(51)+T(6)+T(5)+T(43)+T(8)+T(7)+NT(215)));
//G330: wff_has_subformula_cb(219) => __E_wff_cb_96(220).
	p(NT(219), (NT(220)));
//G331: wff_cb(48)           => wff_has_subformula_cb(219).
	p(NT(48), (NT(219)));
//G332: __E_wff_cb_97(222)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(214).
	p(NT(222), (T(22)+T(4)+T(4)+T(43)+T(11)+T(19)+T(18)+T(17)+T(52)+T(19)+T(43)+T(19)+T(27)+T(10)+T(12)+T(13)+T(19)+T(49)+T(13)+T(10)+T(5)+T(6)+T(43)+T(8)+T(7)+NT(214)));
//G333: wff_remove_existential_cb(221) => __E_wff_cb_97(222).
	p(NT(221), (NT(222)));
//G334: wff_cb(48)           => wff_remove_existential_cb(221).
	p(NT(48), (NT(221)));
//G335: __E_bf_cb_98(224)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(210).
	p(NT(224), (T(7)+T(4)+T(43)+T(48)+T(5)+T(12)+T(43)+T(12)+T(51)+T(7)+T(4)+T(17)+T(11)+T(18)+T(51)+T(6)+T(5)+T(43)+T(8)+T(7)+NT(210)));
//G336: bf_has_subformula_cb(223) => __E_bf_cb_98(224).
	p(NT(223), (NT(224)));
//G337: bf_cb(52)            => bf_has_subformula_cb(223).
	p(NT(52), (NT(223)));
//G338: __E_bf_cb_99(226)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(211).
	p(NT(226), (T(7)+T(4)+T(43)+T(11)+T(19)+T(18)+T(17)+T(52)+T(19)+T(43)+T(4)+T(51)+T(49)+T(10)+T(52)+T(19)+T(11)+T(12)+T(5)+T(6)+T(43)+T(8)+T(7)+NT(211)));
//G339: bf_remove_funiversal_cb(225) => __E_bf_cb_99(226).
	p(NT(225), (NT(226)));
//G340: bf_cb(52)            => bf_remove_funiversal_cb(225).
	p(NT(52), (NT(225)));
//G341: __E_bf_cb_100(228)   => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(211).
	p(NT(228), (T(7)+T(4)+T(43)+T(11)+T(19)+T(18)+T(17)+T(52)+T(19)+T(43)+T(4)+T(19)+T(27)+T(10)+T(12)+T(13)+T(19)+T(49)+T(13)+T(10)+T(5)+T(6)+T(43)+T(8)+T(7)+NT(211)));
//G342: bf_remove_fexistential_cb(227) => __E_bf_cb_100(228).
	p(NT(227), (NT(228)));
//G343: bf_cb(52)            => bf_remove_fexistential_cb(227).
	p(NT(52), (NT(227)));
//G344: __E_bf_cb_101(230)   => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(207).
	p(NT(230), (T(7)+T(4)+T(43)+T(49)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(53)+T(19)+T(43)+T(8)+T(7)+NT(207)));
//G345: bf_normalize_cb(229) => __E_bf_cb_101(230).
	p(NT(229), (NT(230)));
//G346: bf_cb(52)            => bf_normalize_cb(229).
	p(NT(52), (NT(229)));
//G347: cli(231)             => _(11).
	p(NT(231), (NT(11)));
//G348: __E_cli_102(233)     => '.' _(11) cli_command(232) _(11).
	p(NT(233), (T(1)+NT(11)+NT(232)+NT(11)));
//G349: __E_cli_103(234)     => null.
	p(NT(234), (nul));
//G350: __E_cli_103(234)     => __E_cli_102(233) __E_cli_103(234).
	p(NT(234), (NT(233)+NT(234)));
//G351: cli(231)             => _(11) cli_command(232) _(11) __E_cli_103(234).
	p(NT(231), (NT(11)+NT(232)+NT(11)+NT(234)));
//G352: __E_cli_command_104(237) => 'q'.
	p(NT(237), (T(54)));
//G353: __E_cli_command_104(237) => 'q' 'u' 'i' 't'.
	p(NT(237), (T(54)+T(51)+T(10)+T(13)));
//G354: quit_sym(236)        => __E_cli_command_104(237).
	p(NT(236), (NT(237)));
//G355: quit_cmd(235)        => quit_sym(236).
	p(NT(235), (NT(236)));
//G356: cli_command(232)     => quit_cmd(235).
	p(NT(232), (NT(235)));
//G357: __E_cli_command_105(240) => 'v'.
	p(NT(240), (T(52)));
//G358: __E_cli_command_105(240) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(240), (T(52)+T(19)+T(11)+T(12)+T(10)+T(17)+T(49)));
//G359: version_sym(239)     => __E_cli_command_105(240).
	p(NT(239), (NT(240)));
//G360: version_cmd(238)     => version_sym(239).
	p(NT(238), (NT(239)));
//G361: cli_command(232)     => version_cmd(238).
	p(NT(232), (NT(238)));
//G362: __E_cli_command_106(243) => 'c'.
	p(NT(243), (T(8)));
//G363: __E_cli_command_106(243) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(243), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G364: clear_sym(242)       => __E_cli_command_106(243).
	p(NT(242), (NT(243)));
//G365: clear_cmd(241)       => clear_sym(242).
	p(NT(241), (NT(242)));
//G366: cli_command(232)     => clear_cmd(241).
	p(NT(232), (NT(241)));
//G367: __E___E_cli_command_107_108(247) => 'h'.
	p(NT(247), (T(48)));
//G368: __E___E_cli_command_107_108(247) => 'h' 'e' 'l' 'p'.
	p(NT(247), (T(48)+T(19)+T(6)+T(55)));
//G369: help_sym(246)        => __E___E_cli_command_107_108(247).
	p(NT(246), (NT(247)));
//G370: __E___E_cli_command_107_109(248) => __(27) help_arg(249).
	p(NT(248), (NT(27)+NT(249)));
//G371: __E___E_cli_command_107_109(248) => null.
	p(NT(248), (nul));
//G372: __E_cli_command_107(245) => help_sym(246) __E___E_cli_command_107_109(248).
	p(NT(245), (NT(246)+NT(248)));
//G373: help_cmd(244)        => __E_cli_command_107(245).
	p(NT(244), (NT(245)));
//G374: cli_command(232)     => help_cmd(244).
	p(NT(232), (NT(244)));
//G375: file_sym(252)        => 'f' 'i' 'l' 'e'.
	p(NT(252), (T(4)+T(10)+T(6)+T(19)));
//G376: __E_cli_command_110(251) => file_sym(252) __(27) q_string(253).
	p(NT(251), (NT(252)+NT(27)+NT(253)));
//G377: file_cmd(250)        => __E_cli_command_110(251).
	p(NT(250), (NT(251)));
//G378: cli_command(232)     => file_cmd(250).
	p(NT(232), (NT(250)));
//G379: valid_sym(256)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(256), (T(52)+T(5)+T(6)+T(10)+T(56)));
//G380: __E_cli_command_111(255) => valid_sym(256) __(27) normalize_cmd_arg(257).
	p(NT(255), (NT(256)+NT(27)+NT(257)));
//G381: valid_cmd(254)       => __E_cli_command_111(255).
	p(NT(254), (NT(255)));
//G382: cli_command(232)     => valid_cmd(254).
	p(NT(232), (NT(254)));
//G383: sat_sym(260)         => 's' 'a' 't'.
	p(NT(260), (T(12)+T(5)+T(13)));
//G384: __E_cli_command_112(259) => sat_sym(260) __(27) normalize_cmd_arg(257).
	p(NT(259), (NT(260)+NT(27)+NT(257)));
//G385: sat_cmd(258)         => __E_cli_command_112(259).
	p(NT(258), (NT(259)));
//G386: cli_command(232)     => sat_cmd(258).
	p(NT(232), (NT(258)));
//G387: unsat_sym(263)       => 'u' 'n' 's' 'a' 't'.
	p(NT(263), (T(51)+T(49)+T(12)+T(5)+T(13)));
//G388: __E_cli_command_113(262) => unsat_sym(263) __(27) normalize_cmd_arg(257).
	p(NT(262), (NT(263)+NT(27)+NT(257)));
//G389: unsat_cmd(261)       => __E_cli_command_113(262).
	p(NT(261), (NT(262)));
//G390: cli_command(232)     => unsat_cmd(261).
	p(NT(232), (NT(261)));
//G391: solve_sym(266)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(266), (T(12)+T(17)+T(6)+T(52)+T(19)));
//G392: __E___E_cli_command_114_115(267) => __(27) type(138).
	p(NT(267), (NT(27)+NT(138)));
//G393: __E___E_cli_command_114_115(267) => null.
	p(NT(267), (nul));
//G394: __E_cli_command_114(265) => solve_sym(266) __E___E_cli_command_114_115(267) __(27) solve_cmd_arg(268).
	p(NT(265), (NT(266)+NT(267)+NT(27)+NT(268)));
//G395: solve_cmd(264)       => __E_cli_command_114(265).
	p(NT(264), (NT(265)));
//G396: cli_command(232)     => solve_cmd(264).
	p(NT(232), (NT(264)));
//G397: __E___E_cli_command_116_117(272) => 'r'.
	p(NT(272), (T(11)));
//G398: __E___E_cli_command_116_117(272) => 'r' 'u' 'n'.
	p(NT(272), (T(11)+T(51)+T(49)));
//G399: run_sym(271)         => __E___E_cli_command_116_117(272).
	p(NT(271), (NT(272)));
//G400: __E___E_cli_command_116_118(273) => memory(274).
	p(NT(273), (NT(274)));
//G401: __E___E_cli_command_116_118(273) => wff(14).
	p(NT(273), (NT(14)));
//G402: __E_cli_command_116(270) => run_sym(271) __(27) __E___E_cli_command_116_118(273).
	p(NT(270), (NT(271)+NT(27)+NT(273)));
//G403: run_cmd(269)         => __E_cli_command_116(270).
	p(NT(269), (NT(270)));
//G404: cli_command(232)     => run_cmd(269).
	p(NT(232), (NT(269)));
//G405: __E___E_cli_command_119_120(278) => 'n'.
	p(NT(278), (T(49)));
//G406: __E___E_cli_command_119_120(278) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(278), (T(49)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(53)+T(19)));
//G407: normalize_sym(277)   => __E___E_cli_command_119_120(278).
	p(NT(277), (NT(278)));
//G408: __E_cli_command_119(276) => normalize_sym(277) __(27) normalize_cmd_arg(257).
	p(NT(276), (NT(277)+NT(27)+NT(257)));
//G409: normalize_cmd(275)   => __E_cli_command_119(276).
	p(NT(275), (NT(276)));
//G410: cli_command(232)     => normalize_cmd(275).
	p(NT(232), (NT(275)));
//G411: __E___E_cli_command_121_122(282) => 's'.
	p(NT(282), (T(12)));
//G412: __E___E_cli_command_121_122(282) => 's' 'u' 'b' 's' 't'.
	p(NT(282), (T(12)+T(51)+T(7)+T(12)+T(13)));
//G413: __E___E_cli_command_121_122(282) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(282), (T(12)+T(51)+T(7)+T(12)+T(13)+T(10)+T(13)+T(51)+T(13)+T(19)));
//G414: subst_sym(281)       => __E___E_cli_command_121_122(282).
	p(NT(281), (NT(282)));
//G415: __E_cli_command_121(280) => subst_sym(281) __(27) nf_cmd_arg(283) _(11) '[' _(11) nf_cmd_arg(283) _(11) '/' _(11) nf_cmd_arg(283) _(11) ']'.
	p(NT(280), (NT(281)+NT(27)+NT(283)+NT(11)+T(24)+NT(11)+NT(283)+NT(11)+T(57)+NT(11)+NT(283)+NT(11)+T(25)));
//G416: subst_cmd(279)       => __E_cli_command_121(280).
	p(NT(279), (NT(280)));
//G417: cli_command(232)     => subst_cmd(279).
	p(NT(232), (NT(279)));
//G418: __E___E_cli_command_123_124(287) => 'i'.
	p(NT(287), (T(10)));
//G419: __E___E_cli_command_123_124(287) => 'i' 'n' 's' 't'.
	p(NT(287), (T(10)+T(49)+T(12)+T(13)));
//G420: __E___E_cli_command_123_124(287) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(287), (T(10)+T(49)+T(12)+T(13)+T(5)+T(49)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G421: inst_sym(286)        => __E___E_cli_command_123_124(287).
	p(NT(286), (NT(287)));
//G422: __E_cli_command_123(285) => inst_sym(286) __(27) inst_args(288).
	p(NT(285), (NT(286)+NT(27)+NT(288)));
//G423: inst_cmd(284)        => __E_cli_command_123(285).
	p(NT(284), (NT(285)));
//G424: cli_command(232)     => inst_cmd(284).
	p(NT(232), (NT(284)));
//G425: dnf_sym(291)         => 'd' 'n' 'f'.
	p(NT(291), (T(56)+T(49)+T(4)));
//G426: __E_cli_command_125(290) => dnf_sym(291) __(27) nf_cmd_arg(283).
	p(NT(290), (NT(291)+NT(27)+NT(283)));
//G427: dnf_cmd(289)         => __E_cli_command_125(290).
	p(NT(289), (NT(290)));
//G428: cli_command(232)     => dnf_cmd(289).
	p(NT(232), (NT(289)));
//G429: cnf_sym(294)         => 'c' 'n' 'f'.
	p(NT(294), (T(8)+T(49)+T(4)));
//G430: __E_cli_command_126(293) => cnf_sym(294) __(27) nf_cmd_arg(283).
	p(NT(293), (NT(294)+NT(27)+NT(283)));
//G431: cnf_cmd(292)         => __E_cli_command_126(293).
	p(NT(292), (NT(293)));
//G432: cli_command(232)     => cnf_cmd(292).
	p(NT(232), (NT(292)));
//G433: anf_sym(297)         => 'a' 'n' 'f'.
	p(NT(297), (T(5)+T(49)+T(4)));
//G434: __E_cli_command_127(296) => anf_sym(297) __(27) nf_cmd_arg(283).
	p(NT(296), (NT(297)+NT(27)+NT(283)));
//G435: anf_cmd(295)         => __E_cli_command_127(296).
	p(NT(295), (NT(296)));
//G436: cli_command(232)     => anf_cmd(295).
	p(NT(232), (NT(295)));
//G437: nnf_sym(300)         => 'n' 'n' 'f'.
	p(NT(300), (T(49)+T(49)+T(4)));
//G438: __E_cli_command_128(299) => nnf_sym(300) __(27) nf_cmd_arg(283).
	p(NT(299), (NT(300)+NT(27)+NT(283)));
//G439: nnf_cmd(298)         => __E_cli_command_128(299).
	p(NT(298), (NT(299)));
//G440: cli_command(232)     => nnf_cmd(298).
	p(NT(232), (NT(298)));
//G441: pnf_sym(303)         => 'p' 'n' 'f'.
	p(NT(303), (T(55)+T(49)+T(4)));
//G442: __E_cli_command_129(302) => pnf_sym(303) __(27) nf_cmd_arg(283).
	p(NT(302), (NT(303)+NT(27)+NT(283)));
//G443: pnf_cmd(301)         => __E_cli_command_129(302).
	p(NT(301), (NT(302)));
//G444: cli_command(232)     => pnf_cmd(301).
	p(NT(232), (NT(301)));
//G445: mnf_sym(306)         => 'm' 'n' 'f'.
	p(NT(306), (T(18)+T(49)+T(4)));
//G446: __E_cli_command_130(305) => mnf_sym(306) __(27) nf_cmd_arg(283).
	p(NT(305), (NT(306)+NT(27)+NT(283)));
//G447: mnf_cmd(304)         => __E_cli_command_130(305).
	p(NT(304), (NT(305)));
//G448: cli_command(232)     => mnf_cmd(304).
	p(NT(232), (NT(304)));
//G449: snf_sym(309)         => 's' 'n' 'f'.
	p(NT(309), (T(12)+T(49)+T(4)));
//G450: __E_cli_command_131(308) => snf_sym(309) __(27) nf_cmd_arg(283).
	p(NT(308), (NT(309)+NT(27)+NT(283)));
//G451: snf_cmd(307)         => __E_cli_command_131(308).
	p(NT(307), (NT(308)));
//G452: cli_command(232)     => snf_cmd(307).
	p(NT(232), (NT(307)));
//G453: onf_sym(312)         => 'o' 'n' 'f'.
	p(NT(312), (T(17)+T(49)+T(4)));
//G454: __E_cli_command_132(311) => onf_sym(312) __(27) variable(120) __(27) onf_cmd_arg(313).
	p(NT(311), (NT(312)+NT(27)+NT(120)+NT(27)+NT(313)));
//G455: onf_cmd(310)         => __E_cli_command_132(311).
	p(NT(310), (NT(311)));
//G456: cli_command(232)     => onf_cmd(310).
	p(NT(232), (NT(310)));
//G457: __E___E_cli_command_133_134(317) => 'd' 'e' 'f' 's'.
	p(NT(317), (T(56)+T(19)+T(4)+T(12)));
//G458: __E___E_cli_command_133_134(317) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(317), (T(56)+T(19)+T(4)+T(10)+T(49)+T(10)+T(13)+T(10)+T(17)+T(49)+T(12)));
//G459: def_sym(316)         => __E___E_cli_command_133_134(317).
	p(NT(316), (NT(317)));
//G460: __E_cli_command_133(315) => def_sym(316).
	p(NT(315), (NT(316)));
//G461: def_list_cmd(314)    => __E_cli_command_133(315).
	p(NT(314), (NT(315)));
//G462: cli_command(232)     => def_list_cmd(314).
	p(NT(232), (NT(314)));
//G463: __E_cli_command_135(319) => def_sym(316) __(27) number(320).
	p(NT(319), (NT(316)+NT(27)+NT(320)));
//G464: def_print_cmd(318)   => __E_cli_command_135(319).
	p(NT(318), (NT(319)));
//G465: cli_command(232)     => def_print_cmd(318).
	p(NT(232), (NT(318)));
//G466: def_rr_cmd(321)      => rec_relation(16).
	p(NT(321), (NT(16)));
//G467: cli_command(232)     => def_rr_cmd(321).
	p(NT(232), (NT(321)));
//G468: qelim_sym(324)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(324), (T(54)+T(19)+T(6)+T(10)+T(18)));
//G469: __E_cli_command_136(323) => qelim_sym(324) __(27) wff_cmd_arg(325).
	p(NT(323), (NT(324)+NT(27)+NT(325)));
//G470: qelim_cmd(322)       => __E_cli_command_136(323).
	p(NT(322), (NT(323)));
//G471: cli_command(232)     => qelim_cmd(322).
	p(NT(232), (NT(322)));
//G472: get_sym(328)         => 'g' 'e' 't'.
	p(NT(328), (T(50)+T(19)+T(13)));
//G473: __E___E_cli_command_137_138(329) => __(27) option(330).
	p(NT(329), (NT(27)+NT(330)));
//G474: __E___E_cli_command_137_138(329) => null.
	p(NT(329), (nul));
//G475: __E_cli_command_137(327) => get_sym(328) __E___E_cli_command_137_138(329).
	p(NT(327), (NT(328)+NT(329)));
//G476: get_cmd(326)         => __E_cli_command_137(327).
	p(NT(326), (NT(327)));
//G477: cli_command(232)     => get_cmd(326).
	p(NT(232), (NT(326)));
//G478: set_sym(333)         => 's' 'e' 't'.
	p(NT(333), (T(12)+T(19)+T(13)));
//G479: __E___E_cli_command_139_140(334) => __(27).
	p(NT(334), (NT(27)));
//G480: __E___E_cli_command_139_140(334) => _(11) '=' _(11).
	p(NT(334), (NT(11)+T(3)+NT(11)));
//G481: __E_cli_command_139(332) => set_sym(333) __(27) option(330) __E___E_cli_command_139_140(334) option_value(335).
	p(NT(332), (NT(333)+NT(27)+NT(330)+NT(334)+NT(335)));
//G482: set_cmd(331)         => __E_cli_command_139(332).
	p(NT(331), (NT(332)));
//G483: cli_command(232)     => set_cmd(331).
	p(NT(232), (NT(331)));
//G484: enable_sym(338)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(338), (T(19)+T(49)+T(5)+T(7)+T(6)+T(19)));
//G485: __E_cli_command_141(337) => enable_sym(338) __(27) bool_option(339).
	p(NT(337), (NT(338)+NT(27)+NT(339)));
//G486: enable_cmd(336)      => __E_cli_command_141(337).
	p(NT(336), (NT(337)));
//G487: cli_command(232)     => enable_cmd(336).
	p(NT(232), (NT(336)));
//G488: disable_sym(342)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(342), (T(56)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G489: __E_cli_command_142(341) => disable_sym(342) __(27) bool_option(339).
	p(NT(341), (NT(342)+NT(27)+NT(339)));
//G490: disable_cmd(340)     => __E_cli_command_142(341).
	p(NT(340), (NT(341)));
//G491: cli_command(232)     => disable_cmd(340).
	p(NT(232), (NT(340)));
//G492: toggle_sym(345)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(345), (T(13)+T(17)+T(50)+T(50)+T(6)+T(19)));
//G493: __E_cli_command_143(344) => toggle_sym(345) __(27) bool_option(339).
	p(NT(344), (NT(345)+NT(27)+NT(339)));
//G494: toggle_cmd(343)      => __E_cli_command_143(344).
	p(NT(343), (NT(344)));
//G495: cli_command(232)     => toggle_cmd(343).
	p(NT(232), (NT(343)));
//G496: __E_cli_command_144(347) => type(138) __(27) in_var_name(165) _(11) '=' _(11) input_stream(348).
	p(NT(347), (NT(138)+NT(27)+NT(165)+NT(11)+T(3)+NT(11)+NT(348)));
//G497: def_input_cmd(346)   => __E_cli_command_144(347).
	p(NT(346), (NT(347)));
//G498: cli_command(232)     => def_input_cmd(346).
	p(NT(232), (NT(346)));
//G499: __E_cli_command_145(350) => type(138) __(27) out_var_name(169) _(11) '=' _(11) output_stream(351).
	p(NT(350), (NT(138)+NT(27)+NT(169)+NT(11)+T(3)+NT(11)+NT(351)));
//G500: def_output_cmd(349)  => __E_cli_command_145(350).
	p(NT(349), (NT(350)));
//G501: cli_command(232)     => def_output_cmd(349).
	p(NT(232), (NT(349)));
//G502: __E___E_cli_command_146_147(355) => 'h' 'i' 's' 't'.
	p(NT(355), (T(48)+T(10)+T(12)+T(13)));
//G503: __E___E_cli_command_146_147(355) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(355), (T(48)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G504: history_sym(354)     => __E___E_cli_command_146_147(355).
	p(NT(354), (NT(355)));
//G505: __E_cli_command_146(353) => history_sym(354).
	p(NT(353), (NT(354)));
//G506: history_list_cmd(352) => __E_cli_command_146(353).
	p(NT(352), (NT(353)));
//G507: cli_command(232)     => history_list_cmd(352).
	p(NT(232), (NT(352)));
//G508: __E_cli_command_148(357) => history_sym(354) __(27) memory(274).
	p(NT(357), (NT(354)+NT(27)+NT(274)));
//G509: history_print_cmd(356) => __E_cli_command_148(357).
	p(NT(356), (NT(357)));
//G510: cli_command(232)     => history_print_cmd(356).
	p(NT(232), (NT(356)));
//G511: __E_cli_command_149(359) => wff(14).
	p(NT(359), (NT(14)));
//G512: __E_cli_command_149(359) => bf(21).
	p(NT(359), (NT(21)));
//G513: history_store_cmd(358) => __E_cli_command_149(359).
	p(NT(358), (NT(359)));
//G514: cli_command(232)     => history_store_cmd(358).
	p(NT(232), (NT(358)));
//G515: __E_input_stream_150(360) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(361) _(11) ')'.
	p(NT(360), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(361)+NT(11)+T(16)));
//G516: input_stream(348)    => __E_input_stream_150(360).
	p(NT(348), (NT(360)));
//G517: console_sym(362)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(362), (T(8)+T(17)+T(49)+T(12)+T(17)+T(6)+T(19)));
//G518: input_stream(348)    => console_sym(362).
	p(NT(348), (NT(362)));
//G519: __E_output_stream_151(363) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(361) _(11) ')'.
	p(NT(363), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(361)+NT(11)+T(16)));
//G520: output_stream(351)   => __E_output_stream_151(363).
	p(NT(351), (NT(363)));
//G521: output_stream(351)   => console_sym(362).
	p(NT(351), (NT(362)));
//G522: q_file_name(361)     => '"' file_name(364) '"'.
	p(NT(361), (T(58)+NT(364)+T(58)));
//G523: __E_file_name_152(365) => printable(8).
	p(NT(365), (NT(8)));
//G524: __E_file_name_152(365) => printable(8) __E_file_name_152(365).
	p(NT(365), (NT(8)+NT(365)));
//G525: file_name(364)       => __E_file_name_152(365).
	p(NT(364), (NT(365)));
//G526: number(320)          => digits(181).
	p(NT(320), (NT(181)));
//G527: bf_cmd_arg(366)      => memory(274).
	p(NT(366), (NT(274)));
//G528: bf_cmd_arg(366)      => bf(21).
	p(NT(366), (NT(21)));
//G529: wff_cmd_arg(325)     => memory(274).
	p(NT(325), (NT(274)));
//G530: wff_cmd_arg(325)     => wff(14).
	p(NT(325), (NT(14)));
//G531: solve_cmd_arg(268)   => memory(274).
	p(NT(268), (NT(274)));
//G532: solve_cmd_arg(268)   => wff(14).
	p(NT(268), (NT(14)));
//G533: nf_cmd_arg(283)      => memory(274).
	p(NT(283), (NT(274)));
//G534: nf_cmd_arg(283)      => ref(18).
	p(NT(283), (NT(18)));
//G535: nf_cmd_arg(283)      => wff(14).
	p(NT(283), (NT(14)));
//G536: nf_cmd_arg(283)      => bf(21).
	p(NT(283), (NT(21)));
//G537: onf_cmd_arg(313)     => memory(274).
	p(NT(313), (NT(274)));
//G538: onf_cmd_arg(313)     => wff(14).
	p(NT(313), (NT(14)));
//G539: normalize_cmd_arg(257) => memory(274).
	p(NT(257), (NT(274)));
//G540: normalize_cmd_arg(257) => rr(10).
	p(NT(257), (NT(10)));
//G541: normalize_cmd_arg(257) => ref(18).
	p(NT(257), (NT(18)));
//G542: normalize_cmd_arg(257) => wff(14).
	p(NT(257), (NT(14)));
//G543: normalize_cmd_arg(257) => bf(21).
	p(NT(257), (NT(21)));
//G544: inst_args(288)       => wff_cmd_arg(325) _(11) '[' _(11) variable(120) _(11) '/' _(11) bf_cmd_arg(366) _(11) ']'.
	p(NT(288), (NT(325)+NT(11)+T(24)+NT(11)+NT(120)+NT(11)+T(57)+NT(11)+NT(366)+NT(11)+T(25)));
//G545: inst_args(288)       => bf_cmd_arg(366) _(11) '[' _(11) variable(120) _(11) '/' _(11) bf_cmd_arg(366) _(11) ']'.
	p(NT(288), (NT(366)+NT(11)+T(24)+NT(11)+NT(120)+NT(11)+T(57)+NT(11)+NT(366)+NT(11)+T(25)));
//G546: help_arg(249)        => help_sym(246).
	p(NT(249), (NT(246)));
//G547: help_arg(249)        => version_sym(239).
	p(NT(249), (NT(239)));
//G548: help_arg(249)        => quit_sym(236).
	p(NT(249), (NT(236)));
//G549: help_arg(249)        => clear_sym(242).
	p(NT(249), (NT(242)));
//G550: help_arg(249)        => get_sym(328).
	p(NT(249), (NT(328)));
//G551: help_arg(249)        => set_sym(333).
	p(NT(249), (NT(333)));
//G552: help_arg(249)        => enable_sym(338).
	p(NT(249), (NT(338)));
//G553: help_arg(249)        => disable_sym(342).
	p(NT(249), (NT(342)));
//G554: help_arg(249)        => toggle_sym(345).
	p(NT(249), (NT(345)));
//G555: help_arg(249)        => file_sym(252).
	p(NT(249), (NT(252)));
//G556: help_arg(249)        => history_sym(354).
	p(NT(249), (NT(354)));
//G557: help_arg(249)        => abs_memory_sym(367).
	p(NT(249), (NT(367)));
//G558: help_arg(249)        => rel_memory_sym(368).
	p(NT(249), (NT(368)));
//G559: help_arg(249)        => selection_sym(369).
	p(NT(249), (NT(369)));
//G560: help_arg(249)        => def_sym(316).
	p(NT(249), (NT(316)));
//G561: help_arg(249)        => inst_sym(286).
	p(NT(249), (NT(286)));
//G562: help_arg(249)        => subst_sym(281).
	p(NT(249), (NT(281)));
//G563: help_arg(249)        => normalize_sym(277).
	p(NT(249), (NT(277)));
//G564: help_arg(249)        => execute_sym(370).
	p(NT(249), (NT(370)));
//G565: help_arg(249)        => solve_sym(266).
	p(NT(249), (NT(266)));
//G566: help_arg(249)        => valid_sym(256).
	p(NT(249), (NT(256)));
//G567: help_arg(249)        => sat_sym(260).
	p(NT(249), (NT(260)));
//G568: help_arg(249)        => unsat_sym(263).
	p(NT(249), (NT(263)));
//G569: help_arg(249)        => run_sym(271).
	p(NT(249), (NT(271)));
//G570: help_arg(249)        => dnf_sym(291).
	p(NT(249), (NT(291)));
//G571: help_arg(249)        => cnf_sym(294).
	p(NT(249), (NT(294)));
//G572: help_arg(249)        => anf_sym(297).
	p(NT(249), (NT(297)));
//G573: help_arg(249)        => snf_sym(309).
	p(NT(249), (NT(309)));
//G574: help_arg(249)        => nnf_sym(300).
	p(NT(249), (NT(300)));
//G575: help_arg(249)        => pnf_sym(303).
	p(NT(249), (NT(303)));
//G576: help_arg(249)        => mnf_sym(306).
	p(NT(249), (NT(306)));
//G577: help_arg(249)        => onf_sym(312).
	p(NT(249), (NT(312)));
//G578: help_arg(249)        => qelim_sym(324).
	p(NT(249), (NT(324)));
//G579: __E___E_help_arg_153_154(373) => 's'.
	p(NT(373), (T(12)));
//G580: __E___E_help_arg_153_154(373) => null.
	p(NT(373), (nul));
//G581: __E_help_arg_153(372) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_153_154(373).
	p(NT(372), (T(19)+T(27)+T(5)+T(18)+T(55)+T(6)+T(19)+NT(373)));
//G582: examples_sym(371)    => __E_help_arg_153(372).
	p(NT(371), (NT(372)));
//G583: help_arg(249)        => examples_sym(371).
	p(NT(249), (NT(371)));
//G584: __E_memory_155(375)  => '%'.
	p(NT(375), (T(59)));
//G585: rel_memory_sym(368)  => '%' '-'.
	p(NT(368), (T(59)+T(28)));
//G586: memory_id(377)       => digits(181).
	p(NT(377), (NT(181)));
//G587: __E___E_memory_155_156(376) => memory_id(377).
	p(NT(376), (NT(377)));
//G588: __E___E_memory_155_156(376) => null.
	p(NT(376), (nul));
//G589: __E_memory_155(375)  => rel_memory_sym(368) __E___E_memory_155_156(376).
	p(NT(375), (NT(368)+NT(376)));
//G590: rel_memory(374)      => __E_memory_155(375).
	p(NT(374), (NT(375)));
//G591: memory(274)          => rel_memory(374).
	p(NT(274), (NT(374)));
//G592: abs_memory_sym(367)  => '%'.
	p(NT(367), (T(59)));
//G593: __E_memory_157(379)  => abs_memory_sym(367) memory_id(377).
	p(NT(379), (NT(367)+NT(377)));
//G594: abs_memory(378)      => __E_memory_157(379).
	p(NT(378), (NT(379)));
//G595: memory(274)          => abs_memory(378).
	p(NT(274), (NT(378)));
//G596: option(330)          => bool_option(339).
	p(NT(330), (NT(339)));
//G597: __E_option_158(382)  => 'S'.
	p(NT(382), (T(35)));
//G598: __E_option_158(382)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(382), (T(12)+T(19)+T(52)+T(19)+T(11)+T(10)+T(13)+T(23)));
//G599: severity_opt(381)    => __E_option_158(382).
	p(NT(381), (NT(382)));
//G600: enum_option(380)     => severity_opt(381).
	p(NT(380), (NT(381)));
//G601: option(330)          => enum_option(380).
	p(NT(330), (NT(380)));
//G602: __E_bool_option_159(384) => 's'.
	p(NT(384), (T(12)));
//G603: __E_bool_option_159(384) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(384), (T(12)+T(13)+T(5)+T(13)+T(51)+T(12)));
//G604: status_opt(383)      => __E_bool_option_159(384).
	p(NT(383), (NT(384)));
//G605: bool_option(339)     => status_opt(383).
	p(NT(339), (NT(383)));
//G606: __E_bool_option_160(386) => 'c'.
	p(NT(386), (T(8)));
//G607: __E_bool_option_160(386) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(386), (T(8)+T(17)+T(6)+T(17)+T(11)));
//G608: __E_bool_option_160(386) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(386), (T(8)+T(17)+T(6)+T(17)+T(11)+T(12)));
//G609: colors_opt(385)      => __E_bool_option_160(386).
	p(NT(385), (NT(386)));
//G610: bool_option(339)     => colors_opt(385).
	p(NT(339), (NT(385)));
//G611: __E_bool_option_161(388) => 'V'.
	p(NT(388), (T(60)));
//G612: __E_bool_option_161(388) => 'c' 'h' 'a' 'r' 'v' 'a' 'r'.
	p(NT(388), (T(8)+T(48)+T(5)+T(11)+T(52)+T(5)+T(11)));
//G613: charvar_opt(387)     => __E_bool_option_161(388).
	p(NT(387), (NT(388)));
//G614: bool_option(339)     => charvar_opt(387).
	p(NT(339), (NT(387)));
//G615: __E_bool_option_162(390) => 'H'.
	p(NT(390), (T(61)));
//G616: __E_bool_option_162(390) => 'h' 'i' 'g' 'h' 'l' 'i' 'g' 'h' 't'.
	p(NT(390), (T(48)+T(10)+T(50)+T(48)+T(6)+T(10)+T(50)+T(48)+T(13)));
//G617: __E_bool_option_162(390) => 'h' 'i' 'g' 'h' 'l' 'i' 'g' 'h' 't' 'i' 'n' 'g'.
	p(NT(390), (T(48)+T(10)+T(50)+T(48)+T(6)+T(10)+T(50)+T(48)+T(13)+T(10)+T(49)+T(50)));
//G618: highlighting_opt(389) => __E_bool_option_162(390).
	p(NT(389), (NT(390)));
//G619: bool_option(339)     => highlighting_opt(389).
	p(NT(339), (NT(389)));
//G620: __E_bool_option_163(392) => 'I'.
	p(NT(392), (T(62)));
//G621: __E_bool_option_163(392) => 'i' 'n' 'd' 'e' 'n' 't'.
	p(NT(392), (T(10)+T(49)+T(56)+T(19)+T(49)+T(13)));
//G622: __E_bool_option_163(392) => 'i' 'n' 'd' 'e' 'n' 't' 'i' 'n' 'g'.
	p(NT(392), (T(10)+T(49)+T(56)+T(19)+T(49)+T(13)+T(10)+T(49)+T(50)));
//G623: indenting_opt(391)   => __E_bool_option_163(392).
	p(NT(391), (NT(392)));
//G624: bool_option(339)     => indenting_opt(391).
	p(NT(339), (NT(391)));
//G625: __E_bool_option_164(394) => 'd'.
	p(NT(394), (T(56)));
//G626: __E_bool_option_164(394) => 'd' 'b' 'g'.
	p(NT(394), (T(56)+T(7)+T(50)));
//G627: __E_bool_option_164(394) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(394), (T(56)+T(19)+T(7)+T(51)+T(50)));
//G628: debug_repl_opt(393)  => __E_bool_option_164(394).
	p(NT(393), (NT(394)));
//G629: bool_option(339)     => debug_repl_opt(393).
	p(NT(339), (NT(393)));
//G630: option_value(335)    => option_value_true(395).
	p(NT(335), (NT(395)));
//G631: option_value(335)    => option_value_false(396).
	p(NT(335), (NT(396)));
//G632: option_value(335)    => severity(397).
	p(NT(335), (NT(397)));
//G633: option_value_true(395) => 't'.
	p(NT(395), (T(13)));
//G634: option_value_true(395) => 't' 'r' 'u' 'e'.
	p(NT(395), (T(13)+T(11)+T(51)+T(19)));
//G635: option_value_true(395) => 'o' 'n'.
	p(NT(395), (T(17)+T(49)));
//G636: option_value_true(395) => '1'.
	p(NT(395), (T(38)));
//G637: option_value_true(395) => 'y'.
	p(NT(395), (T(23)));
//G638: option_value_true(395) => 'y' 'e' 's'.
	p(NT(395), (T(23)+T(19)+T(12)));
//G639: option_value_false(396) => 'f'.
	p(NT(396), (T(4)));
//G640: option_value_false(396) => 'f' 'a' 'l' 's' 'e'.
	p(NT(396), (T(4)+T(5)+T(6)+T(12)+T(19)));
//G641: option_value_false(396) => 'o' 'f' 'f'.
	p(NT(396), (T(17)+T(4)+T(4)));
//G642: option_value_false(396) => '0'.
	p(NT(396), (T(39)));
//G643: option_value_false(396) => 'n'.
	p(NT(396), (T(49)));
//G644: option_value_false(396) => 'n' 'o'.
	p(NT(396), (T(49)+T(17)));
//G645: __E_severity_165(399) => 'e'.
	p(NT(399), (T(19)));
//G646: __E_severity_165(399) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(399), (T(19)+T(11)+T(11)+T(17)+T(11)));
//G647: error_sym(398)       => __E_severity_165(399).
	p(NT(398), (NT(399)));
//G648: severity(397)        => error_sym(398).
	p(NT(397), (NT(398)));
//G649: __E_severity_166(401) => 'i'.
	p(NT(401), (T(10)));
//G650: __E_severity_166(401) => 'i' 'n' 'f' 'o'.
	p(NT(401), (T(10)+T(49)+T(4)+T(17)));
//G651: info_sym(400)        => __E_severity_166(401).
	p(NT(400), (NT(401)));
//G652: severity(397)        => info_sym(400).
	p(NT(397), (NT(400)));
//G653: __E_severity_167(403) => 'd'.
	p(NT(403), (T(56)));
//G654: __E_severity_167(403) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(403), (T(56)+T(19)+T(7)+T(51)+T(50)));
//G655: debug_sym(402)       => __E_severity_167(403).
	p(NT(402), (NT(403)));
//G656: severity(397)        => debug_sym(402).
	p(NT(397), (NT(402)));
//G657: __E_severity_168(405) => 't'.
	p(NT(405), (T(13)));
//G658: __E_severity_168(405) => 't' 'r' 'a' 'c' 'e'.
	p(NT(405), (T(13)+T(11)+T(5)+T(8)+T(19)));
//G659: trace_sym(404)       => __E_severity_168(405).
	p(NT(404), (NT(405)));
//G660: severity(397)        => trace_sym(404).
	p(NT(397), (NT(404)));
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
		variable, bf_splitter, __E_bf_46, bf_ref, bf_or, __E_bf_47, bf_xor, __E_bf_48, bf_and, __E_bf_49, 
		__E___E_bf_49_50, __E___E___E_bf_49_50_51, bf_and_nosep, bf_neg, __E_bf_52, bf_t, __E_bf_53, __E___E_bf_53_54, type, bf_f, 
		__E_bf_55, __E___E_bf_55_56, bf_and_nosep_1st_op, __E_bf_and_nosep_57, ctn_neq, __E_constraint_58, ctnvar, num, ctn_eq, __E_constraint_59, 
		ctn_greater_equal, __E_constraint_60, ctn_greater, __E_constraint_61, ctn_less_equal, __E_constraint_62, ctn_less, __E_constraint_63, __E_variable_64, chars, 
		io_var, __E_variable_65, in, out, uninterpreted_constant, in_var_name, __E_in_66, __E___E_in_66_67, offset, out_var_name, 
		__E_out_68, __E___E_out_68_69, q_var, __E_q_vars_70, __E_q_vars_71, __E_offsets_72, __E_offsets_73, shift, __E_offset_74, __E_shift_75, 
		__E___E_shift_75_76, digits, uninter_const_name, __E_uninterpreted_constant_77, __E___E_uninterpreted_constant_77_78, constant, __E_bf_constant_79, binding, __E_bf_constant_80, source, 
		__E_binding_81, src_c, __E___E_binding_81_82, __E___E___E_binding_81_82_83, __E___E___E_binding_81_82_84, __E_src_c_85, __E_src_c_86, __E_chars_87, __E_chars_88, __E_digits_89, 
		extra, comment, __E_comment_90, __E_comment_91, __E_comment_92, __E____93, __E___94, bf_cb_args1, bf_cb_arg, bf_cb_args2, 
		bf_cb_args3, bf_cb_args4, wff_cb_args1, wff_cb_arg, wff_cb_args2, wff_cb_args3, wff_cb_args4, wff_has_clashing_subformulas_cb, __E_wff_cb_95, wff_has_subformula_cb, 
		__E_wff_cb_96, wff_remove_existential_cb, __E_wff_cb_97, bf_has_subformula_cb, __E_bf_cb_98, bf_remove_funiversal_cb, __E_bf_cb_99, bf_remove_fexistential_cb, __E_bf_cb_100, bf_normalize_cb, 
		__E_bf_cb_101, cli, cli_command, __E_cli_102, __E_cli_103, quit_cmd, quit_sym, __E_cli_command_104, version_cmd, version_sym, 
		__E_cli_command_105, clear_cmd, clear_sym, __E_cli_command_106, help_cmd, __E_cli_command_107, help_sym, __E___E_cli_command_107_108, __E___E_cli_command_107_109, help_arg, 
		file_cmd, __E_cli_command_110, file_sym, q_string, valid_cmd, __E_cli_command_111, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_112, 
		sat_sym, unsat_cmd, __E_cli_command_113, unsat_sym, solve_cmd, __E_cli_command_114, solve_sym, __E___E_cli_command_114_115, solve_cmd_arg, run_cmd, 
		__E_cli_command_116, run_sym, __E___E_cli_command_116_117, __E___E_cli_command_116_118, memory, normalize_cmd, __E_cli_command_119, normalize_sym, __E___E_cli_command_119_120, subst_cmd, 
		__E_cli_command_121, subst_sym, __E___E_cli_command_121_122, nf_cmd_arg, inst_cmd, __E_cli_command_123, inst_sym, __E___E_cli_command_123_124, inst_args, dnf_cmd, 
		__E_cli_command_125, dnf_sym, cnf_cmd, __E_cli_command_126, cnf_sym, anf_cmd, __E_cli_command_127, anf_sym, nnf_cmd, __E_cli_command_128, 
		nnf_sym, pnf_cmd, __E_cli_command_129, pnf_sym, mnf_cmd, __E_cli_command_130, mnf_sym, snf_cmd, __E_cli_command_131, snf_sym, 
		onf_cmd, __E_cli_command_132, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_133, def_sym, __E___E_cli_command_133_134, def_print_cmd, __E_cli_command_135, 
		number, def_rr_cmd, qelim_cmd, __E_cli_command_136, qelim_sym, wff_cmd_arg, get_cmd, __E_cli_command_137, get_sym, __E___E_cli_command_137_138, 
		option, set_cmd, __E_cli_command_139, set_sym, __E___E_cli_command_139_140, option_value, enable_cmd, __E_cli_command_141, enable_sym, bool_option, 
		disable_cmd, __E_cli_command_142, disable_sym, toggle_cmd, __E_cli_command_143, toggle_sym, def_input_cmd, __E_cli_command_144, input_stream, def_output_cmd, 
		__E_cli_command_145, output_stream, history_list_cmd, __E_cli_command_146, history_sym, __E___E_cli_command_146_147, history_print_cmd, __E_cli_command_148, history_store_cmd, __E_cli_command_149, 
		__E_input_stream_150, q_file_name, console_sym, __E_output_stream_151, file_name, __E_file_name_152, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, 
		execute_sym, examples_sym, __E_help_arg_153, __E___E_help_arg_153_154, rel_memory, __E_memory_155, __E___E_memory_155_156, memory_id, abs_memory, __E_memory_157, 
		enum_option, severity_opt, __E_option_158, status_opt, __E_bool_option_159, colors_opt, __E_bool_option_160, charvar_opt, __E_bool_option_161, highlighting_opt, 
		__E_bool_option_162, indenting_opt, __E_bool_option_163, debug_repl_opt, __E_bool_option_164, option_value_true, option_value_false, severity, error_sym, __E_severity_165, 
		info_sym, __E_severity_166, debug_sym, __E_severity_167, trace_sym, __E_severity_168, named_binding, __N_0, __N_1, __N_2, 
		__N_3, __N_4, 
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
