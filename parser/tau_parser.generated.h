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
	"__E___E_bf_49_50", "bf_neg", "__E_bf_51", "bf_t", "bf_f", "bf_and_nosep_1st_op", "__E_bf_and_52", "uninterpreted_constant", "ctn_neq", "__E_constraint_53", 
	"ctnvar", "num", "ctn_eq", "__E_constraint_54", "ctn_greater_equal", "__E_constraint_55", "ctn_greater", "__E_constraint_56", "ctn_less_equal", "__E_constraint_57", 
	"ctn_less", "__E_constraint_58", "__E_variable_59", "chars", "io_var", "__E_variable_60", "in", "out", "in_var_name", "__E_in_61", 
	"__E___E_in_61_62", "offset", "out_var_name", "__E_out_63", "__E___E_out_63_64", "q_var", "__E_q_vars_65", "__E_q_vars_66", "__E_offsets_67", "__E_offsets_68", 
	"shift", "__E_offset_69", "__E_shift_70", "__E___E_shift_70_71", "digits", "uninter_const_name", "__E_uninterpreted_constant_72", "__E___E_uninterpreted_constant_72_73", "constant", "__E_bf_constant_74", 
	"binding", "__E_bf_constant_75", "type", "source", "__E_binding_76", "src_c", "__E___E_binding_76_77", "__E___E___E_binding_76_77_78", "__E___E___E_binding_76_77_79", "__E_src_c_80", 
	"__E_src_c_81", "__E_chars_82", "__E_chars_83", "__E_digits_84", "extra", "comment", "__E_comment_85", "__E_comment_86", "__E_comment_87", "__E____88", 
	"__E___89", "bf_cb_args1", "bf_cb_arg", "bf_cb_args2", "bf_cb_args3", "bf_cb_args4", "wff_cb_args1", "wff_cb_arg", "wff_cb_args2", "wff_cb_args3", 
	"wff_cb_args4", "wff_has_clashing_subformulas_cb", "__E_wff_cb_90", "wff_has_subformula_cb", "__E_wff_cb_91", "wff_remove_existential_cb", "__E_wff_cb_92", "bf_has_subformula_cb", "__E_bf_cb_93", "bf_remove_funiversal_cb", 
	"__E_bf_cb_94", "bf_remove_fexistential_cb", "__E_bf_cb_95", "bf_normalize_cb", "__E_bf_cb_96", "cli", "cli_command", "__E_cli_97", "__E_cli_98", "quit_cmd", 
	"quit_sym", "__E_cli_command_99", "version_cmd", "version_sym", "__E_cli_command_100", "clear_cmd", "clear_sym", "__E_cli_command_101", "help_cmd", "__E_cli_command_102", 
	"help_sym", "__E___E_cli_command_102_103", "__E___E_cli_command_102_104", "help_arg", "file_cmd", "__E_cli_command_105", "file_sym", "q_string", "valid_cmd", "__E_cli_command_106", 
	"valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_107", "sat_sym", "unsat_cmd", "__E_cli_command_108", "unsat_sym", "solve_cmd", "__E_cli_command_109", 
	"solve_sym", "__E___E_cli_command_109_110", "solve_cmd_arg", "run_cmd", "__E_cli_command_111", "run_sym", "__E___E_cli_command_111_112", "__E___E_cli_command_111_113", "memory", "normalize_cmd", 
	"__E_cli_command_114", "normalize_sym", "__E___E_cli_command_114_115", "subst_cmd", "__E_cli_command_116", "subst_sym", "__E___E_cli_command_116_117", "nf_cmd_arg", "inst_cmd", "__E_cli_command_118", 
	"inst_sym", "__E___E_cli_command_118_119", "inst_args", "dnf_cmd", "__E_cli_command_120", "dnf_sym", "cnf_cmd", "__E_cli_command_121", "cnf_sym", "anf_cmd", 
	"__E_cli_command_122", "anf_sym", "nnf_cmd", "__E_cli_command_123", "nnf_sym", "pnf_cmd", "__E_cli_command_124", "pnf_sym", "mnf_cmd", "__E_cli_command_125", 
	"mnf_sym", "snf_cmd", "__E_cli_command_126", "snf_sym", "onf_cmd", "__E_cli_command_127", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_128", 
	"def_sym", "__E___E_cli_command_128_129", "def_print_cmd", "__E_cli_command_130", "number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_131", "qelim_sym", "wff_cmd_arg", 
	"get_cmd", "__E_cli_command_132", "get_sym", "__E___E_cli_command_132_133", "option", "set_cmd", "__E_cli_command_134", "set_sym", "__E___E_cli_command_134_135", "option_value", 
	"enable_cmd", "__E_cli_command_136", "enable_sym", "bool_option", "disable_cmd", "__E_cli_command_137", "disable_sym", "toggle_cmd", "__E_cli_command_138", "toggle_sym", 
	"def_input_cmd", "__E_cli_command_139", "input_stream", "def_output_cmd", "__E_cli_command_140", "output_stream", "history_list_cmd", "__E_cli_command_141", "history_sym", "__E___E_cli_command_141_142", 
	"history_print_cmd", "__E_cli_command_143", "history_store_cmd", "__E_cli_command_144", "__E_input_stream_145", "q_file_name", "console_sym", "__E_output_stream_146", "file_name", "__E_file_name_147", 
	"bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "execute_sym", "examples_sym", "__E_help_arg_148", "__E___E_help_arg_148_149", "rel_memory", "__E_memory_150", 
	"__E___E_memory_150_151", "memory_id", "abs_memory", "__E_memory_152", "enum_option", "severity_opt", "__E_option_153", "status_opt", "__E_bool_option_154", "colors_opt", 
	"__E_bool_option_155", "charvar_opt", "__E_bool_option_156", "highlighting_opt", "__E_bool_option_157", "indenting_opt", "__E_bool_option_158", "debug_repl_opt", "__E_bool_option_159", "option_value_true", 
	"option_value_false", "severity", "error_sym", "__E_severity_160", "info_sym", "__E_severity_161", "debug_sym", "__E_severity_162", "trace_sym", "__E_severity_163", 
	"named_binding", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", 
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
			20, 22, 120, 140, 158, 162, 174, 175, 182, 183,
			358, 400
		},
		.to_inline = {
			{ 14, 64, 14 },
			{ 21, 117, 21 },
			{ 48 },
			{ 52 },
			{ 153 },
			{ 165 },
			{ 185 },
			{ 201 },
			{ 203 },
			{ 204 },
			{ 205 },
			{ 206 },
			{ 208 },
			{ 209 },
			{ 210 },
			{ 251 },
			{ 262 },
			{ 277 },
			{ 282 },
			{ 307 },
			{ 319 },
			{ 360 }
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
//G163: __E___E_bf_49_50(130) => _(11) '&' _(11).
	p(NT(130), (NT(11)+T(31)+NT(11)));
//G164: __E___E_bf_49_50(130) => __(27).
	p(NT(130), (NT(27)));
//G165: __E_bf_49(129)       => bf(21) __E___E_bf_49_50(130) bf(21).
	p(NT(129), (NT(21)+NT(130)+NT(21)));
//G166: bf_and(128)          => __E_bf_49(129).
	p(NT(128), (NT(129)));
//G167: bf(21)               => bf_and(128).
	p(NT(21), (NT(128)));
//G168: __E_bf_51(132)       => bf(21) _(11) '\''.
	p(NT(132), (NT(21)+NT(11)+T(37)));
//G169: bf_neg(131)          => __E_bf_51(132).
	p(NT(131), (NT(132)));
//G170: bf(21)               => bf_neg(131).
	p(NT(21), (NT(131)));
//G171: bf_t(133)            => '1'.
	p(NT(133), (T(38)));
//G172: bf(21)               => bf_t(133).
	p(NT(21), (NT(133)));
//G173: bf_f(134)            => '0'.
	p(NT(134), (T(39)));
//G174: bf(21)               => bf_f(134).
	p(NT(21), (NT(134)));
//G175: bf(21)               => capture(20).
	p(NT(21), (NT(20)));
//G176: __E_bf_and_52(136)   => bf_parenthesis(117).
	p(NT(136), (NT(117)));
//G177: __E_bf_and_52(136)   => variable(120).
	p(NT(136), (NT(120)));
//G178: __E_bf_and_52(136)   => bf_splitter(121).
	p(NT(136), (NT(121)));
//G179: __E_bf_and_52(136)   => bf_ref(123).
	p(NT(136), (NT(123)));
//G180: __E_bf_and_52(136)   => bf_or(124).
	p(NT(136), (NT(124)));
//G181: __E_bf_and_52(136)   => bf_xor(126).
	p(NT(136), (NT(126)));
//G182: __E_bf_and_52(136)   => bf_neg(131).
	p(NT(136), (NT(131)));
//G183: __E_bf_and_52(136)   => capture(20).
	p(NT(136), (NT(20)));
//G184: __E_bf_and_52(136)   => uninterpreted_constant(137).
	p(NT(136), (NT(137)));
//G185: bf_and_nosep_1st_op(135) => __E_bf_and_52(136).
	p(NT(135), (NT(136)));
//G186: bf_and(128)          => bf_and_nosep_1st_op(135) bf(21).
	p(NT(128), (NT(135)+NT(21)));
//G187: __E_constraint_53(139) => '[' ctnvar(140) _(11) '!' '=' _(11) num(141) ']'.
	p(NT(139), (T(24)+NT(140)+NT(11)+T(32)+T(3)+NT(11)+NT(141)+T(25)));
//G188: __E_constraint_53(139) => '[' num(141) _(11) '!' '=' _(11) ctnvar(140) ']'.
	p(NT(139), (T(24)+NT(141)+NT(11)+T(32)+T(3)+NT(11)+NT(140)+T(25)));
//G189: ctn_neq(138)         => __E_constraint_53(139).
	p(NT(138), (NT(139)));
//G190: constraint(80)       => ctn_neq(138).
	p(NT(80), (NT(138)));
//G191: __E_constraint_54(143) => '[' ctnvar(140) _(11) '=' _(11) num(141) ']'.
	p(NT(143), (T(24)+NT(140)+NT(11)+T(3)+NT(11)+NT(141)+T(25)));
//G192: __E_constraint_54(143) => '[' num(141) _(11) '=' _(11) ctnvar(140) ']'.
	p(NT(143), (T(24)+NT(141)+NT(11)+T(3)+NT(11)+NT(140)+T(25)));
//G193: ctn_eq(142)          => __E_constraint_54(143).
	p(NT(142), (NT(143)));
//G194: constraint(80)       => ctn_eq(142).
	p(NT(80), (NT(142)));
//G195: __E_constraint_55(145) => '[' ctnvar(140) _(11) '>' '=' _(11) num(141) ']'.
	p(NT(145), (T(24)+NT(140)+NT(11)+T(21)+T(3)+NT(11)+NT(141)+T(25)));
//G196: __E_constraint_55(145) => '[' num(141) _(11) '>' '=' _(11) ctnvar(140) ']'.
	p(NT(145), (T(24)+NT(141)+NT(11)+T(21)+T(3)+NT(11)+NT(140)+T(25)));
//G197: ctn_greater_equal(144) => __E_constraint_55(145).
	p(NT(144), (NT(145)));
//G198: constraint(80)       => ctn_greater_equal(144).
	p(NT(80), (NT(144)));
//G199: __E_constraint_56(147) => '[' ctnvar(140) _(11) '>' _(11) num(141) ']'.
	p(NT(147), (T(24)+NT(140)+NT(11)+T(21)+NT(11)+NT(141)+T(25)));
//G200: __E_constraint_56(147) => '[' num(141) _(11) '>' _(11) ctnvar(140) ']'.
	p(NT(147), (T(24)+NT(141)+NT(11)+T(21)+NT(11)+NT(140)+T(25)));
//G201: ctn_greater(146)     => __E_constraint_56(147).
	p(NT(146), (NT(147)));
//G202: constraint(80)       => ctn_greater(146).
	p(NT(80), (NT(146)));
//G203: __E_constraint_57(149) => '[' ctnvar(140) _(11) '<' '=' _(11) num(141) ']'.
	p(NT(149), (T(24)+NT(140)+NT(11)+T(20)+T(3)+NT(11)+NT(141)+T(25)));
//G204: __E_constraint_57(149) => '[' num(141) _(11) '<' '=' _(11) ctnvar(140) ']'.
	p(NT(149), (T(24)+NT(141)+NT(11)+T(20)+T(3)+NT(11)+NT(140)+T(25)));
//G205: ctn_less_equal(148)  => __E_constraint_57(149).
	p(NT(148), (NT(149)));
//G206: constraint(80)       => ctn_less_equal(148).
	p(NT(80), (NT(148)));
//G207: __E_constraint_58(151) => '[' ctnvar(140) _(11) '<' _(11) num(141) ']'.
	p(NT(151), (T(24)+NT(140)+NT(11)+T(20)+NT(11)+NT(141)+T(25)));
//G208: __E_constraint_58(151) => '[' num(141) _(11) '<' _(11) ctnvar(140) ']'.
	p(NT(151), (T(24)+NT(141)+NT(11)+T(20)+NT(11)+NT(140)+T(25)));
//G209: ctn_less(150)        => __E_constraint_58(151).
	p(NT(150), (NT(151)));
//G210: constraint(80)       => ctn_less(150).
	p(NT(80), (NT(150)));
//G211: __E_variable_59(152) => null.
	p(NT(152), (nul));
//G212: __E_variable_59(152) => digit(3) __E_variable_59(152).
	p(NT(152), (NT(3)+NT(152)));
//G213: variable(120)        => alpha(5) __E_variable_59(152).	 # guarded: charvar
	p(NT(120), (NT(5)+NT(152)));
	p.back().guard = "charvar";
//G214: variable(120)        => chars(153).	 # guarded: var
	p(NT(120), (NT(153)));
	p.back().guard = "var";
//G215: __E_variable_60(155) => in(156).
	p(NT(155), (NT(156)));
//G216: __E_variable_60(155) => out(157).
	p(NT(155), (NT(157)));
//G217: io_var(154)          => __E_variable_60(155).
	p(NT(154), (NT(155)));
//G218: variable(120)        => io_var(154).
	p(NT(120), (NT(154)));
//G219: variable(120)        => uninterpreted_constant(137).
	p(NT(120), (NT(137)));
//G220: __E___E_in_61_62(160) => digit(3).
	p(NT(160), (NT(3)));
//G221: __E___E_in_61_62(160) => digit(3) __E___E_in_61_62(160).
	p(NT(160), (NT(3)+NT(160)));
//G222: __E_in_61(159)       => 'i' __E___E_in_61_62(160).
	p(NT(159), (T(10)+NT(160)));
//G223: in_var_name(158)     => __E_in_61(159).
	p(NT(158), (NT(159)));
//G224: in(156)              => in_var_name(158) '[' offset(161) ']'.
	p(NT(156), (NT(158)+T(24)+NT(161)+T(25)));
//G225: __E___E_out_63_64(164) => digit(3).
	p(NT(164), (NT(3)));
//G226: __E___E_out_63_64(164) => digit(3) __E___E_out_63_64(164).
	p(NT(164), (NT(3)+NT(164)));
//G227: __E_out_63(163)      => 'o' __E___E_out_63_64(164).
	p(NT(163), (T(17)+NT(164)));
//G228: out_var_name(162)    => __E_out_63(163).
	p(NT(162), (NT(163)));
//G229: out(157)             => out_var_name(162) '[' offset(161) ']'.
	p(NT(157), (NT(162)+T(24)+NT(161)+T(25)));
//G230: ctnvar(140)          => chars(153).
	p(NT(140), (NT(153)));
//G231: capture(20)          => '$' chars(153).
	p(NT(20), (T(40)+NT(153)));
//G232: __E_q_vars_65(166)   => _(11) ',' _(11) q_var(165).
	p(NT(166), (NT(11)+T(14)+NT(11)+NT(165)));
//G233: __E_q_vars_66(167)   => null.
	p(NT(167), (nul));
//G234: __E_q_vars_66(167)   => __E_q_vars_65(166) __E_q_vars_66(167).
	p(NT(167), (NT(166)+NT(167)));
//G235: q_vars(76)           => q_var(165) __E_q_vars_66(167).
	p(NT(76), (NT(165)+NT(167)));
//G236: q_var(165)           => capture(20).
	p(NT(165), (NT(20)));
//G237: __N_0(401)           => uninterpreted_constant(137).
	p(NT(401), (NT(137)));
//G238: q_var(165)           => variable(120) & ~( __N_0(401) ).	 # conjunctive
	p(NT(165), (NT(120)) & ~(NT(401)));
//G239: __E_offsets_67(168)  => _(11) ',' _(11) offset(161).
	p(NT(168), (NT(11)+T(14)+NT(11)+NT(161)));
//G240: __E_offsets_68(169)  => null.
	p(NT(169), (nul));
//G241: __E_offsets_68(169)  => __E_offsets_67(168) __E_offsets_68(169).
	p(NT(169), (NT(168)+NT(169)));
//G242: offsets(24)          => '[' _(11) offset(161) __E_offsets_68(169) _(11) ']'.
	p(NT(24), (T(24)+NT(11)+NT(161)+NT(169)+NT(11)+T(25)));
//G243: offset(161)          => num(141).
	p(NT(161), (NT(141)));
//G244: offset(161)          => capture(20).
	p(NT(161), (NT(20)));
//G245: offset(161)          => shift(170).
	p(NT(161), (NT(170)));
//G246: __N_1(402)           => io_var(154).
	p(NT(402), (NT(154)));
//G247: __E_offset_69(171)   => variable(120) & ~( __N_1(402) ).	 # conjunctive
	p(NT(171), (NT(120)) & ~(NT(402)));
//G248: offset(161)          => __E_offset_69(171).
	p(NT(161), (NT(171)));
//G249: __E_shift_70(172)    => capture(20).
	p(NT(172), (NT(20)));
//G250: __N_2(403)           => io_var(154).
	p(NT(403), (NT(154)));
//G251: __E___E_shift_70_71(173) => variable(120) & ~( __N_2(403) ).	 # conjunctive
	p(NT(173), (NT(120)) & ~(NT(403)));
//G252: __E_shift_70(172)    => __E___E_shift_70_71(173).
	p(NT(172), (NT(173)));
//G253: shift(170)           => __E_shift_70(172) _(11) '-' _(11) num(141).
	p(NT(170), (NT(172)+NT(11)+T(28)+NT(11)+NT(141)));
//G254: num(141)             => digits(174).
	p(NT(141), (NT(174)));
//G255: __E___E_uninterpreted_constant_72_73(177) => chars(153).
	p(NT(177), (NT(153)));
//G256: __E___E_uninterpreted_constant_72_73(177) => _(11).
	p(NT(177), (NT(11)));
//G257: __E_uninterpreted_constant_72(176) => __E___E_uninterpreted_constant_72_73(177) ':' chars(153).
	p(NT(176), (NT(177)+T(2)+NT(153)));
//G258: uninter_const_name(175) => __E_uninterpreted_constant_72(176).
	p(NT(175), (NT(176)));
//G259: uninterpreted_constant(137) => '<' uninter_const_name(175) _(11) '>'.
	p(NT(137), (T(20)+NT(175)+NT(11)+T(21)));
//G260: __E_bf_constant_74(179) => capture(20).
	p(NT(179), (NT(20)));
//G261: __E_bf_constant_74(179) => binding(180).
	p(NT(179), (NT(180)));
//G262: constant(178)        => __E_bf_constant_74(179).
	p(NT(178), (NT(179)));
//G263: type(182)            => chars(153).
	p(NT(182), (NT(153)));
//G264: __E_bf_constant_75(181) => _(11) ':' _(11) type(182).
	p(NT(181), (NT(11)+T(2)+NT(11)+NT(182)));
//G265: __E_bf_constant_75(181) => null.
	p(NT(181), (nul));
//G266: bf_constant(119)     => '{' _(11) constant(178) _(11) '}' __E_bf_constant_75(181).
	p(NT(119), (T(41)+NT(11)+NT(178)+NT(11)+T(42)+NT(181)));
//G267: __E___E___E_binding_76_77_78(187) => src_c(185).
	p(NT(187), (NT(185)));
//G268: __E___E___E_binding_76_77_78(187) => space(2).
	p(NT(187), (NT(2)));
//G269: __E___E___E_binding_76_77_79(188) => null.
	p(NT(188), (nul));
//G270: __E___E___E_binding_76_77_79(188) => __E___E___E_binding_76_77_78(187) __E___E___E_binding_76_77_79(188).
	p(NT(188), (NT(187)+NT(188)));
//G271: __E___E_binding_76_77(186) => __E___E___E_binding_76_77_79(188) src_c(185).
	p(NT(186), (NT(188)+NT(185)));
//G272: __E___E_binding_76_77(186) => null.
	p(NT(186), (nul));
//G273: __E_binding_76(184)  => src_c(185) __E___E_binding_76_77(186).
	p(NT(184), (NT(185)+NT(186)));
//G274: source(183)          => __E_binding_76(184).
	p(NT(183), (NT(184)));
//G275: binding(180)         => source(183).
	p(NT(180), (NT(183)));
//G276: src_c(185)           => alnum(6).
	p(NT(185), (NT(6)));
//G277: __N_3(404)           => '{'.
	p(NT(404), (T(41)));
//G278: __N_4(405)           => '}'.
	p(NT(405), (T(42)));
//G279: src_c(185)           => ~( __N_3(404) ) & ~( __N_4(405) ) & punct(7).	 # conjunctive
	p(NT(185), ~(NT(404)) & ~(NT(405)) & (NT(7)));
//G280: __E_src_c_80(189)    => src_c(185).
	p(NT(189), (NT(185)));
//G281: __E_src_c_80(189)    => space(2).
	p(NT(189), (NT(2)));
//G282: __E_src_c_81(190)    => null.
	p(NT(190), (nul));
//G283: __E_src_c_81(190)    => __E_src_c_80(189) __E_src_c_81(190).
	p(NT(190), (NT(189)+NT(190)));
//G284: src_c(185)           => '{' __E_src_c_81(190) '}'.
	p(NT(185), (T(41)+NT(190)+T(42)));
//G285: __E_chars_82(191)    => alnum(6).
	p(NT(191), (NT(6)));
//G286: __E_chars_82(191)    => '_'.
	p(NT(191), (T(43)));
//G287: __E_chars_83(192)    => null.
	p(NT(192), (nul));
//G288: __E_chars_83(192)    => __E_chars_82(191) __E_chars_83(192).
	p(NT(192), (NT(191)+NT(192)));
//G289: chars(153)           => alpha(5) __E_chars_83(192).
	p(NT(153), (NT(5)+NT(192)));
//G290: __E_digits_84(193)   => digit(3).
	p(NT(193), (NT(3)));
//G291: __E_digits_84(193)   => digit(3) __E_digits_84(193).
	p(NT(193), (NT(3)+NT(193)));
//G292: digits(174)          => __E_digits_84(193).
	p(NT(174), (NT(193)));
//G293: sym(22)              => chars(153).
	p(NT(22), (NT(153)));
//G294: extra(194)           => chars(153).
	p(NT(194), (NT(153)));
//G295: __E_comment_85(196)  => printable(8).
	p(NT(196), (NT(8)));
//G296: __E_comment_85(196)  => '\t'.
	p(NT(196), (T(44)));
//G297: __E_comment_86(197)  => null.
	p(NT(197), (nul));
//G298: __E_comment_86(197)  => __E_comment_85(196) __E_comment_86(197).
	p(NT(197), (NT(196)+NT(197)));
//G299: __E_comment_87(198)  => '\n'.
	p(NT(198), (T(45)));
//G300: __E_comment_87(198)  => '\r'.
	p(NT(198), (T(46)));
//G301: __E_comment_87(198)  => eof(1).
	p(NT(198), (NT(1)));
//G302: comment(195)         => '#' __E_comment_86(197) __E_comment_87(198).
	p(NT(195), (T(47)+NT(197)+NT(198)));
//G303: __E____88(199)       => space(2).
	p(NT(199), (NT(2)));
//G304: __E____88(199)       => comment(195).
	p(NT(199), (NT(195)));
//G305: __(27)               => __E____88(199) _(11).
	p(NT(27), (NT(199)+NT(11)));
//G306: __E___89(200)        => __(27).
	p(NT(200), (NT(27)));
//G307: __E___89(200)        => null.
	p(NT(200), (nul));
//G308: _(11)                => __E___89(200).
	p(NT(11), (NT(200)));
//G309: bf_cb_arg(202)       => bf(21).
	p(NT(202), (NT(21)));
//G310: bf_cb_args1(201)     => __(27) bf_cb_arg(202).
	p(NT(201), (NT(27)+NT(202)));
//G311: bf_cb_args2(203)     => bf_cb_args1(201) bf_cb_args1(201).
	p(NT(203), (NT(201)+NT(201)));
//G312: bf_cb_args3(204)     => bf_cb_args2(203) bf_cb_args1(201).
	p(NT(204), (NT(203)+NT(201)));
//G313: bf_cb_args4(205)     => bf_cb_args3(204) bf_cb_args1(201).
	p(NT(205), (NT(204)+NT(201)));
//G314: wff_cb_arg(207)      => wff(14).
	p(NT(207), (NT(14)));
//G315: wff_cb_args1(206)    => __(27) wff_cb_arg(207).
	p(NT(206), (NT(27)+NT(207)));
//G316: wff_cb_args2(208)    => wff_cb_args1(206) wff_cb_args1(206).
	p(NT(208), (NT(206)+NT(206)));
//G317: wff_cb_args3(209)    => wff_cb_args2(208) wff_cb_args1(206).
	p(NT(209), (NT(208)+NT(206)));
//G318: wff_cb_args4(210)    => wff_cb_args3(209) wff_cb_args1(206).
	p(NT(210), (NT(209)+NT(206)));
//G319: __E_wff_cb_90(212)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(208).
	p(NT(212), (T(22)+T(4)+T(4)+T(43)+T(48)+T(5)+T(12)+T(43)+T(8)+T(6)+T(5)+T(12)+T(48)+T(10)+T(49)+T(50)+T(43)+T(12)+T(51)+T(7)+T(4)+T(17)+T(11)+T(18)+T(51)+T(6)+T(5)+T(12)+T(43)+T(8)+T(7)+NT(208)));
//G320: wff_has_clashing_subformulas_cb(211) => __E_wff_cb_90(212).
	p(NT(211), (NT(212)));
//G321: wff_cb(48)           => wff_has_clashing_subformulas_cb(211).
	p(NT(48), (NT(211)));
//G322: __E_wff_cb_91(214)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(209).
	p(NT(214), (T(22)+T(4)+T(4)+T(43)+T(48)+T(5)+T(12)+T(43)+T(12)+T(51)+T(7)+T(4)+T(17)+T(11)+T(18)+T(51)+T(6)+T(5)+T(43)+T(8)+T(7)+NT(209)));
//G323: wff_has_subformula_cb(213) => __E_wff_cb_91(214).
	p(NT(213), (NT(214)));
//G324: wff_cb(48)           => wff_has_subformula_cb(213).
	p(NT(48), (NT(213)));
//G325: __E_wff_cb_92(216)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(208).
	p(NT(216), (T(22)+T(4)+T(4)+T(43)+T(11)+T(19)+T(18)+T(17)+T(52)+T(19)+T(43)+T(19)+T(27)+T(10)+T(12)+T(13)+T(19)+T(49)+T(13)+T(10)+T(5)+T(6)+T(43)+T(8)+T(7)+NT(208)));
//G326: wff_remove_existential_cb(215) => __E_wff_cb_92(216).
	p(NT(215), (NT(216)));
//G327: wff_cb(48)           => wff_remove_existential_cb(215).
	p(NT(48), (NT(215)));
//G328: __E_bf_cb_93(218)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(204).
	p(NT(218), (T(7)+T(4)+T(43)+T(48)+T(5)+T(12)+T(43)+T(12)+T(51)+T(7)+T(4)+T(17)+T(11)+T(18)+T(51)+T(6)+T(5)+T(43)+T(8)+T(7)+NT(204)));
//G329: bf_has_subformula_cb(217) => __E_bf_cb_93(218).
	p(NT(217), (NT(218)));
//G330: bf_cb(52)            => bf_has_subformula_cb(217).
	p(NT(52), (NT(217)));
//G331: __E_bf_cb_94(220)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(205).
	p(NT(220), (T(7)+T(4)+T(43)+T(11)+T(19)+T(18)+T(17)+T(52)+T(19)+T(43)+T(4)+T(51)+T(49)+T(10)+T(52)+T(19)+T(11)+T(12)+T(5)+T(6)+T(43)+T(8)+T(7)+NT(205)));
//G332: bf_remove_funiversal_cb(219) => __E_bf_cb_94(220).
	p(NT(219), (NT(220)));
//G333: bf_cb(52)            => bf_remove_funiversal_cb(219).
	p(NT(52), (NT(219)));
//G334: __E_bf_cb_95(222)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(205).
	p(NT(222), (T(7)+T(4)+T(43)+T(11)+T(19)+T(18)+T(17)+T(52)+T(19)+T(43)+T(4)+T(19)+T(27)+T(10)+T(12)+T(13)+T(19)+T(49)+T(13)+T(10)+T(5)+T(6)+T(43)+T(8)+T(7)+NT(205)));
//G335: bf_remove_fexistential_cb(221) => __E_bf_cb_95(222).
	p(NT(221), (NT(222)));
//G336: bf_cb(52)            => bf_remove_fexistential_cb(221).
	p(NT(52), (NT(221)));
//G337: __E_bf_cb_96(224)    => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(201).
	p(NT(224), (T(7)+T(4)+T(43)+T(49)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(53)+T(19)+T(43)+T(8)+T(7)+NT(201)));
//G338: bf_normalize_cb(223) => __E_bf_cb_96(224).
	p(NT(223), (NT(224)));
//G339: bf_cb(52)            => bf_normalize_cb(223).
	p(NT(52), (NT(223)));
//G340: cli(225)             => _(11).
	p(NT(225), (NT(11)));
//G341: __E_cli_97(227)      => '.' _(11) cli_command(226) _(11).
	p(NT(227), (T(1)+NT(11)+NT(226)+NT(11)));
//G342: __E_cli_98(228)      => null.
	p(NT(228), (nul));
//G343: __E_cli_98(228)      => __E_cli_97(227) __E_cli_98(228).
	p(NT(228), (NT(227)+NT(228)));
//G344: cli(225)             => _(11) cli_command(226) _(11) __E_cli_98(228).
	p(NT(225), (NT(11)+NT(226)+NT(11)+NT(228)));
//G345: __E_cli_command_99(231) => 'q'.
	p(NT(231), (T(54)));
//G346: __E_cli_command_99(231) => 'q' 'u' 'i' 't'.
	p(NT(231), (T(54)+T(51)+T(10)+T(13)));
//G347: quit_sym(230)        => __E_cli_command_99(231).
	p(NT(230), (NT(231)));
//G348: quit_cmd(229)        => quit_sym(230).
	p(NT(229), (NT(230)));
//G349: cli_command(226)     => quit_cmd(229).
	p(NT(226), (NT(229)));
//G350: __E_cli_command_100(234) => 'v'.
	p(NT(234), (T(52)));
//G351: __E_cli_command_100(234) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(234), (T(52)+T(19)+T(11)+T(12)+T(10)+T(17)+T(49)));
//G352: version_sym(233)     => __E_cli_command_100(234).
	p(NT(233), (NT(234)));
//G353: version_cmd(232)     => version_sym(233).
	p(NT(232), (NT(233)));
//G354: cli_command(226)     => version_cmd(232).
	p(NT(226), (NT(232)));
//G355: __E_cli_command_101(237) => 'c'.
	p(NT(237), (T(8)));
//G356: __E_cli_command_101(237) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(237), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G357: clear_sym(236)       => __E_cli_command_101(237).
	p(NT(236), (NT(237)));
//G358: clear_cmd(235)       => clear_sym(236).
	p(NT(235), (NT(236)));
//G359: cli_command(226)     => clear_cmd(235).
	p(NT(226), (NT(235)));
//G360: __E___E_cli_command_102_103(241) => 'h'.
	p(NT(241), (T(48)));
//G361: __E___E_cli_command_102_103(241) => 'h' 'e' 'l' 'p'.
	p(NT(241), (T(48)+T(19)+T(6)+T(55)));
//G362: help_sym(240)        => __E___E_cli_command_102_103(241).
	p(NT(240), (NT(241)));
//G363: __E___E_cli_command_102_104(242) => __(27) help_arg(243).
	p(NT(242), (NT(27)+NT(243)));
//G364: __E___E_cli_command_102_104(242) => null.
	p(NT(242), (nul));
//G365: __E_cli_command_102(239) => help_sym(240) __E___E_cli_command_102_104(242).
	p(NT(239), (NT(240)+NT(242)));
//G366: help_cmd(238)        => __E_cli_command_102(239).
	p(NT(238), (NT(239)));
//G367: cli_command(226)     => help_cmd(238).
	p(NT(226), (NT(238)));
//G368: file_sym(246)        => 'f' 'i' 'l' 'e'.
	p(NT(246), (T(4)+T(10)+T(6)+T(19)));
//G369: __E_cli_command_105(245) => file_sym(246) __(27) q_string(247).
	p(NT(245), (NT(246)+NT(27)+NT(247)));
//G370: file_cmd(244)        => __E_cli_command_105(245).
	p(NT(244), (NT(245)));
//G371: cli_command(226)     => file_cmd(244).
	p(NT(226), (NT(244)));
//G372: valid_sym(250)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(250), (T(52)+T(5)+T(6)+T(10)+T(56)));
//G373: __E_cli_command_106(249) => valid_sym(250) __(27) normalize_cmd_arg(251).
	p(NT(249), (NT(250)+NT(27)+NT(251)));
//G374: valid_cmd(248)       => __E_cli_command_106(249).
	p(NT(248), (NT(249)));
//G375: cli_command(226)     => valid_cmd(248).
	p(NT(226), (NT(248)));
//G376: sat_sym(254)         => 's' 'a' 't'.
	p(NT(254), (T(12)+T(5)+T(13)));
//G377: __E_cli_command_107(253) => sat_sym(254) __(27) normalize_cmd_arg(251).
	p(NT(253), (NT(254)+NT(27)+NT(251)));
//G378: sat_cmd(252)         => __E_cli_command_107(253).
	p(NT(252), (NT(253)));
//G379: cli_command(226)     => sat_cmd(252).
	p(NT(226), (NT(252)));
//G380: unsat_sym(257)       => 'u' 'n' 's' 'a' 't'.
	p(NT(257), (T(51)+T(49)+T(12)+T(5)+T(13)));
//G381: __E_cli_command_108(256) => unsat_sym(257) __(27) normalize_cmd_arg(251).
	p(NT(256), (NT(257)+NT(27)+NT(251)));
//G382: unsat_cmd(255)       => __E_cli_command_108(256).
	p(NT(255), (NT(256)));
//G383: cli_command(226)     => unsat_cmd(255).
	p(NT(226), (NT(255)));
//G384: solve_sym(260)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(260), (T(12)+T(17)+T(6)+T(52)+T(19)));
//G385: __E___E_cli_command_109_110(261) => __(27) type(182).
	p(NT(261), (NT(27)+NT(182)));
//G386: __E___E_cli_command_109_110(261) => null.
	p(NT(261), (nul));
//G387: __E_cli_command_109(259) => solve_sym(260) __E___E_cli_command_109_110(261) __(27) solve_cmd_arg(262).
	p(NT(259), (NT(260)+NT(261)+NT(27)+NT(262)));
//G388: solve_cmd(258)       => __E_cli_command_109(259).
	p(NT(258), (NT(259)));
//G389: cli_command(226)     => solve_cmd(258).
	p(NT(226), (NT(258)));
//G390: __E___E_cli_command_111_112(266) => 'r'.
	p(NT(266), (T(11)));
//G391: __E___E_cli_command_111_112(266) => 'r' 'u' 'n'.
	p(NT(266), (T(11)+T(51)+T(49)));
//G392: run_sym(265)         => __E___E_cli_command_111_112(266).
	p(NT(265), (NT(266)));
//G393: __E___E_cli_command_111_113(267) => memory(268).
	p(NT(267), (NT(268)));
//G394: __E___E_cli_command_111_113(267) => wff(14).
	p(NT(267), (NT(14)));
//G395: __E_cli_command_111(264) => run_sym(265) __(27) __E___E_cli_command_111_113(267).
	p(NT(264), (NT(265)+NT(27)+NT(267)));
//G396: run_cmd(263)         => __E_cli_command_111(264).
	p(NT(263), (NT(264)));
//G397: cli_command(226)     => run_cmd(263).
	p(NT(226), (NT(263)));
//G398: __E___E_cli_command_114_115(272) => 'n'.
	p(NT(272), (T(49)));
//G399: __E___E_cli_command_114_115(272) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(272), (T(49)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(53)+T(19)));
//G400: normalize_sym(271)   => __E___E_cli_command_114_115(272).
	p(NT(271), (NT(272)));
//G401: __E_cli_command_114(270) => normalize_sym(271) __(27) normalize_cmd_arg(251).
	p(NT(270), (NT(271)+NT(27)+NT(251)));
//G402: normalize_cmd(269)   => __E_cli_command_114(270).
	p(NT(269), (NT(270)));
//G403: cli_command(226)     => normalize_cmd(269).
	p(NT(226), (NT(269)));
//G404: __E___E_cli_command_116_117(276) => 's'.
	p(NT(276), (T(12)));
//G405: __E___E_cli_command_116_117(276) => 's' 'u' 'b' 's' 't'.
	p(NT(276), (T(12)+T(51)+T(7)+T(12)+T(13)));
//G406: __E___E_cli_command_116_117(276) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(276), (T(12)+T(51)+T(7)+T(12)+T(13)+T(10)+T(13)+T(51)+T(13)+T(19)));
//G407: subst_sym(275)       => __E___E_cli_command_116_117(276).
	p(NT(275), (NT(276)));
//G408: __E_cli_command_116(274) => subst_sym(275) __(27) nf_cmd_arg(277) _(11) '[' _(11) nf_cmd_arg(277) _(11) '/' _(11) nf_cmd_arg(277) _(11) ']'.
	p(NT(274), (NT(275)+NT(27)+NT(277)+NT(11)+T(24)+NT(11)+NT(277)+NT(11)+T(57)+NT(11)+NT(277)+NT(11)+T(25)));
//G409: subst_cmd(273)       => __E_cli_command_116(274).
	p(NT(273), (NT(274)));
//G410: cli_command(226)     => subst_cmd(273).
	p(NT(226), (NT(273)));
//G411: __E___E_cli_command_118_119(281) => 'i'.
	p(NT(281), (T(10)));
//G412: __E___E_cli_command_118_119(281) => 'i' 'n' 's' 't'.
	p(NT(281), (T(10)+T(49)+T(12)+T(13)));
//G413: __E___E_cli_command_118_119(281) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(281), (T(10)+T(49)+T(12)+T(13)+T(5)+T(49)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G414: inst_sym(280)        => __E___E_cli_command_118_119(281).
	p(NT(280), (NT(281)));
//G415: __E_cli_command_118(279) => inst_sym(280) __(27) inst_args(282).
	p(NT(279), (NT(280)+NT(27)+NT(282)));
//G416: inst_cmd(278)        => __E_cli_command_118(279).
	p(NT(278), (NT(279)));
//G417: cli_command(226)     => inst_cmd(278).
	p(NT(226), (NT(278)));
//G418: dnf_sym(285)         => 'd' 'n' 'f'.
	p(NT(285), (T(56)+T(49)+T(4)));
//G419: __E_cli_command_120(284) => dnf_sym(285) __(27) nf_cmd_arg(277).
	p(NT(284), (NT(285)+NT(27)+NT(277)));
//G420: dnf_cmd(283)         => __E_cli_command_120(284).
	p(NT(283), (NT(284)));
//G421: cli_command(226)     => dnf_cmd(283).
	p(NT(226), (NT(283)));
//G422: cnf_sym(288)         => 'c' 'n' 'f'.
	p(NT(288), (T(8)+T(49)+T(4)));
//G423: __E_cli_command_121(287) => cnf_sym(288) __(27) nf_cmd_arg(277).
	p(NT(287), (NT(288)+NT(27)+NT(277)));
//G424: cnf_cmd(286)         => __E_cli_command_121(287).
	p(NT(286), (NT(287)));
//G425: cli_command(226)     => cnf_cmd(286).
	p(NT(226), (NT(286)));
//G426: anf_sym(291)         => 'a' 'n' 'f'.
	p(NT(291), (T(5)+T(49)+T(4)));
//G427: __E_cli_command_122(290) => anf_sym(291) __(27) nf_cmd_arg(277).
	p(NT(290), (NT(291)+NT(27)+NT(277)));
//G428: anf_cmd(289)         => __E_cli_command_122(290).
	p(NT(289), (NT(290)));
//G429: cli_command(226)     => anf_cmd(289).
	p(NT(226), (NT(289)));
//G430: nnf_sym(294)         => 'n' 'n' 'f'.
	p(NT(294), (T(49)+T(49)+T(4)));
//G431: __E_cli_command_123(293) => nnf_sym(294) __(27) nf_cmd_arg(277).
	p(NT(293), (NT(294)+NT(27)+NT(277)));
//G432: nnf_cmd(292)         => __E_cli_command_123(293).
	p(NT(292), (NT(293)));
//G433: cli_command(226)     => nnf_cmd(292).
	p(NT(226), (NT(292)));
//G434: pnf_sym(297)         => 'p' 'n' 'f'.
	p(NT(297), (T(55)+T(49)+T(4)));
//G435: __E_cli_command_124(296) => pnf_sym(297) __(27) nf_cmd_arg(277).
	p(NT(296), (NT(297)+NT(27)+NT(277)));
//G436: pnf_cmd(295)         => __E_cli_command_124(296).
	p(NT(295), (NT(296)));
//G437: cli_command(226)     => pnf_cmd(295).
	p(NT(226), (NT(295)));
//G438: mnf_sym(300)         => 'm' 'n' 'f'.
	p(NT(300), (T(18)+T(49)+T(4)));
//G439: __E_cli_command_125(299) => mnf_sym(300) __(27) nf_cmd_arg(277).
	p(NT(299), (NT(300)+NT(27)+NT(277)));
//G440: mnf_cmd(298)         => __E_cli_command_125(299).
	p(NT(298), (NT(299)));
//G441: cli_command(226)     => mnf_cmd(298).
	p(NT(226), (NT(298)));
//G442: snf_sym(303)         => 's' 'n' 'f'.
	p(NT(303), (T(12)+T(49)+T(4)));
//G443: __E_cli_command_126(302) => snf_sym(303) __(27) nf_cmd_arg(277).
	p(NT(302), (NT(303)+NT(27)+NT(277)));
//G444: snf_cmd(301)         => __E_cli_command_126(302).
	p(NT(301), (NT(302)));
//G445: cli_command(226)     => snf_cmd(301).
	p(NT(226), (NT(301)));
//G446: onf_sym(306)         => 'o' 'n' 'f'.
	p(NT(306), (T(17)+T(49)+T(4)));
//G447: __E_cli_command_127(305) => onf_sym(306) __(27) variable(120) __(27) onf_cmd_arg(307).
	p(NT(305), (NT(306)+NT(27)+NT(120)+NT(27)+NT(307)));
//G448: onf_cmd(304)         => __E_cli_command_127(305).
	p(NT(304), (NT(305)));
//G449: cli_command(226)     => onf_cmd(304).
	p(NT(226), (NT(304)));
//G450: __E___E_cli_command_128_129(311) => 'd' 'e' 'f' 's'.
	p(NT(311), (T(56)+T(19)+T(4)+T(12)));
//G451: __E___E_cli_command_128_129(311) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(311), (T(56)+T(19)+T(4)+T(10)+T(49)+T(10)+T(13)+T(10)+T(17)+T(49)+T(12)));
//G452: def_sym(310)         => __E___E_cli_command_128_129(311).
	p(NT(310), (NT(311)));
//G453: __E_cli_command_128(309) => def_sym(310).
	p(NT(309), (NT(310)));
//G454: def_list_cmd(308)    => __E_cli_command_128(309).
	p(NT(308), (NT(309)));
//G455: cli_command(226)     => def_list_cmd(308).
	p(NT(226), (NT(308)));
//G456: __E_cli_command_130(313) => def_sym(310) __(27) number(314).
	p(NT(313), (NT(310)+NT(27)+NT(314)));
//G457: def_print_cmd(312)   => __E_cli_command_130(313).
	p(NT(312), (NT(313)));
//G458: cli_command(226)     => def_print_cmd(312).
	p(NT(226), (NT(312)));
//G459: def_rr_cmd(315)      => rec_relation(16).
	p(NT(315), (NT(16)));
//G460: cli_command(226)     => def_rr_cmd(315).
	p(NT(226), (NT(315)));
//G461: qelim_sym(318)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(318), (T(54)+T(19)+T(6)+T(10)+T(18)));
//G462: __E_cli_command_131(317) => qelim_sym(318) __(27) wff_cmd_arg(319).
	p(NT(317), (NT(318)+NT(27)+NT(319)));
//G463: qelim_cmd(316)       => __E_cli_command_131(317).
	p(NT(316), (NT(317)));
//G464: cli_command(226)     => qelim_cmd(316).
	p(NT(226), (NT(316)));
//G465: get_sym(322)         => 'g' 'e' 't'.
	p(NT(322), (T(50)+T(19)+T(13)));
//G466: __E___E_cli_command_132_133(323) => __(27) option(324).
	p(NT(323), (NT(27)+NT(324)));
//G467: __E___E_cli_command_132_133(323) => null.
	p(NT(323), (nul));
//G468: __E_cli_command_132(321) => get_sym(322) __E___E_cli_command_132_133(323).
	p(NT(321), (NT(322)+NT(323)));
//G469: get_cmd(320)         => __E_cli_command_132(321).
	p(NT(320), (NT(321)));
//G470: cli_command(226)     => get_cmd(320).
	p(NT(226), (NT(320)));
//G471: set_sym(327)         => 's' 'e' 't'.
	p(NT(327), (T(12)+T(19)+T(13)));
//G472: __E___E_cli_command_134_135(328) => __(27).
	p(NT(328), (NT(27)));
//G473: __E___E_cli_command_134_135(328) => _(11) '=' _(11).
	p(NT(328), (NT(11)+T(3)+NT(11)));
//G474: __E_cli_command_134(326) => set_sym(327) __(27) option(324) __E___E_cli_command_134_135(328) option_value(329).
	p(NT(326), (NT(327)+NT(27)+NT(324)+NT(328)+NT(329)));
//G475: set_cmd(325)         => __E_cli_command_134(326).
	p(NT(325), (NT(326)));
//G476: cli_command(226)     => set_cmd(325).
	p(NT(226), (NT(325)));
//G477: enable_sym(332)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(332), (T(19)+T(49)+T(5)+T(7)+T(6)+T(19)));
//G478: __E_cli_command_136(331) => enable_sym(332) __(27) bool_option(333).
	p(NT(331), (NT(332)+NT(27)+NT(333)));
//G479: enable_cmd(330)      => __E_cli_command_136(331).
	p(NT(330), (NT(331)));
//G480: cli_command(226)     => enable_cmd(330).
	p(NT(226), (NT(330)));
//G481: disable_sym(336)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(336), (T(56)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G482: __E_cli_command_137(335) => disable_sym(336) __(27) bool_option(333).
	p(NT(335), (NT(336)+NT(27)+NT(333)));
//G483: disable_cmd(334)     => __E_cli_command_137(335).
	p(NT(334), (NT(335)));
//G484: cli_command(226)     => disable_cmd(334).
	p(NT(226), (NT(334)));
//G485: toggle_sym(339)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(339), (T(13)+T(17)+T(50)+T(50)+T(6)+T(19)));
//G486: __E_cli_command_138(338) => toggle_sym(339) __(27) bool_option(333).
	p(NT(338), (NT(339)+NT(27)+NT(333)));
//G487: toggle_cmd(337)      => __E_cli_command_138(338).
	p(NT(337), (NT(338)));
//G488: cli_command(226)     => toggle_cmd(337).
	p(NT(226), (NT(337)));
//G489: __E_cli_command_139(341) => type(182) __(27) in_var_name(158) _(11) '=' _(11) input_stream(342).
	p(NT(341), (NT(182)+NT(27)+NT(158)+NT(11)+T(3)+NT(11)+NT(342)));
//G490: def_input_cmd(340)   => __E_cli_command_139(341).
	p(NT(340), (NT(341)));
//G491: cli_command(226)     => def_input_cmd(340).
	p(NT(226), (NT(340)));
//G492: __E_cli_command_140(344) => type(182) __(27) out_var_name(162) _(11) '=' _(11) output_stream(345).
	p(NT(344), (NT(182)+NT(27)+NT(162)+NT(11)+T(3)+NT(11)+NT(345)));
//G493: def_output_cmd(343)  => __E_cli_command_140(344).
	p(NT(343), (NT(344)));
//G494: cli_command(226)     => def_output_cmd(343).
	p(NT(226), (NT(343)));
//G495: __E___E_cli_command_141_142(349) => 'h' 'i' 's' 't'.
	p(NT(349), (T(48)+T(10)+T(12)+T(13)));
//G496: __E___E_cli_command_141_142(349) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(349), (T(48)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G497: history_sym(348)     => __E___E_cli_command_141_142(349).
	p(NT(348), (NT(349)));
//G498: __E_cli_command_141(347) => history_sym(348).
	p(NT(347), (NT(348)));
//G499: history_list_cmd(346) => __E_cli_command_141(347).
	p(NT(346), (NT(347)));
//G500: cli_command(226)     => history_list_cmd(346).
	p(NT(226), (NT(346)));
//G501: __E_cli_command_143(351) => history_sym(348) __(27) memory(268).
	p(NT(351), (NT(348)+NT(27)+NT(268)));
//G502: history_print_cmd(350) => __E_cli_command_143(351).
	p(NT(350), (NT(351)));
//G503: cli_command(226)     => history_print_cmd(350).
	p(NT(226), (NT(350)));
//G504: __E_cli_command_144(353) => wff(14).
	p(NT(353), (NT(14)));
//G505: __E_cli_command_144(353) => bf(21).
	p(NT(353), (NT(21)));
//G506: history_store_cmd(352) => __E_cli_command_144(353).
	p(NT(352), (NT(353)));
//G507: cli_command(226)     => history_store_cmd(352).
	p(NT(226), (NT(352)));
//G508: __E_input_stream_145(354) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(355) _(11) ')'.
	p(NT(354), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(355)+NT(11)+T(16)));
//G509: input_stream(342)    => __E_input_stream_145(354).
	p(NT(342), (NT(354)));
//G510: console_sym(356)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(356), (T(8)+T(17)+T(49)+T(12)+T(17)+T(6)+T(19)));
//G511: input_stream(342)    => console_sym(356).
	p(NT(342), (NT(356)));
//G512: __E_output_stream_146(357) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(355) _(11) ')'.
	p(NT(357), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(355)+NT(11)+T(16)));
//G513: output_stream(345)   => __E_output_stream_146(357).
	p(NT(345), (NT(357)));
//G514: output_stream(345)   => console_sym(356).
	p(NT(345), (NT(356)));
//G515: q_file_name(355)     => '"' file_name(358) '"'.
	p(NT(355), (T(58)+NT(358)+T(58)));
//G516: __E_file_name_147(359) => printable(8).
	p(NT(359), (NT(8)));
//G517: __E_file_name_147(359) => printable(8) __E_file_name_147(359).
	p(NT(359), (NT(8)+NT(359)));
//G518: file_name(358)       => __E_file_name_147(359).
	p(NT(358), (NT(359)));
//G519: number(314)          => digits(174).
	p(NT(314), (NT(174)));
//G520: bf_cmd_arg(360)      => memory(268).
	p(NT(360), (NT(268)));
//G521: bf_cmd_arg(360)      => bf(21).
	p(NT(360), (NT(21)));
//G522: wff_cmd_arg(319)     => memory(268).
	p(NT(319), (NT(268)));
//G523: wff_cmd_arg(319)     => wff(14).
	p(NT(319), (NT(14)));
//G524: solve_cmd_arg(262)   => memory(268).
	p(NT(262), (NT(268)));
//G525: solve_cmd_arg(262)   => wff(14).
	p(NT(262), (NT(14)));
//G526: nf_cmd_arg(277)      => memory(268).
	p(NT(277), (NT(268)));
//G527: nf_cmd_arg(277)      => ref(18).
	p(NT(277), (NT(18)));
//G528: nf_cmd_arg(277)      => wff(14).
	p(NT(277), (NT(14)));
//G529: nf_cmd_arg(277)      => bf(21).
	p(NT(277), (NT(21)));
//G530: onf_cmd_arg(307)     => memory(268).
	p(NT(307), (NT(268)));
//G531: onf_cmd_arg(307)     => wff(14).
	p(NT(307), (NT(14)));
//G532: normalize_cmd_arg(251) => memory(268).
	p(NT(251), (NT(268)));
//G533: normalize_cmd_arg(251) => rr(10).
	p(NT(251), (NT(10)));
//G534: normalize_cmd_arg(251) => ref(18).
	p(NT(251), (NT(18)));
//G535: normalize_cmd_arg(251) => wff(14).
	p(NT(251), (NT(14)));
//G536: normalize_cmd_arg(251) => bf(21).
	p(NT(251), (NT(21)));
//G537: inst_args(282)       => wff_cmd_arg(319) _(11) '[' _(11) variable(120) _(11) '/' _(11) bf_cmd_arg(360) _(11) ']'.
	p(NT(282), (NT(319)+NT(11)+T(24)+NT(11)+NT(120)+NT(11)+T(57)+NT(11)+NT(360)+NT(11)+T(25)));
//G538: inst_args(282)       => bf_cmd_arg(360) _(11) '[' _(11) variable(120) _(11) '/' _(11) bf_cmd_arg(360) _(11) ']'.
	p(NT(282), (NT(360)+NT(11)+T(24)+NT(11)+NT(120)+NT(11)+T(57)+NT(11)+NT(360)+NT(11)+T(25)));
//G539: help_arg(243)        => help_sym(240).
	p(NT(243), (NT(240)));
//G540: help_arg(243)        => version_sym(233).
	p(NT(243), (NT(233)));
//G541: help_arg(243)        => quit_sym(230).
	p(NT(243), (NT(230)));
//G542: help_arg(243)        => clear_sym(236).
	p(NT(243), (NT(236)));
//G543: help_arg(243)        => get_sym(322).
	p(NT(243), (NT(322)));
//G544: help_arg(243)        => set_sym(327).
	p(NT(243), (NT(327)));
//G545: help_arg(243)        => enable_sym(332).
	p(NT(243), (NT(332)));
//G546: help_arg(243)        => disable_sym(336).
	p(NT(243), (NT(336)));
//G547: help_arg(243)        => toggle_sym(339).
	p(NT(243), (NT(339)));
//G548: help_arg(243)        => file_sym(246).
	p(NT(243), (NT(246)));
//G549: help_arg(243)        => history_sym(348).
	p(NT(243), (NT(348)));
//G550: help_arg(243)        => abs_memory_sym(361).
	p(NT(243), (NT(361)));
//G551: help_arg(243)        => rel_memory_sym(362).
	p(NT(243), (NT(362)));
//G552: help_arg(243)        => selection_sym(363).
	p(NT(243), (NT(363)));
//G553: help_arg(243)        => def_sym(310).
	p(NT(243), (NT(310)));
//G554: help_arg(243)        => inst_sym(280).
	p(NT(243), (NT(280)));
//G555: help_arg(243)        => subst_sym(275).
	p(NT(243), (NT(275)));
//G556: help_arg(243)        => normalize_sym(271).
	p(NT(243), (NT(271)));
//G557: help_arg(243)        => execute_sym(364).
	p(NT(243), (NT(364)));
//G558: help_arg(243)        => solve_sym(260).
	p(NT(243), (NT(260)));
//G559: help_arg(243)        => valid_sym(250).
	p(NT(243), (NT(250)));
//G560: help_arg(243)        => sat_sym(254).
	p(NT(243), (NT(254)));
//G561: help_arg(243)        => unsat_sym(257).
	p(NT(243), (NT(257)));
//G562: help_arg(243)        => run_sym(265).
	p(NT(243), (NT(265)));
//G563: help_arg(243)        => dnf_sym(285).
	p(NT(243), (NT(285)));
//G564: help_arg(243)        => cnf_sym(288).
	p(NT(243), (NT(288)));
//G565: help_arg(243)        => anf_sym(291).
	p(NT(243), (NT(291)));
//G566: help_arg(243)        => snf_sym(303).
	p(NT(243), (NT(303)));
//G567: help_arg(243)        => nnf_sym(294).
	p(NT(243), (NT(294)));
//G568: help_arg(243)        => pnf_sym(297).
	p(NT(243), (NT(297)));
//G569: help_arg(243)        => mnf_sym(300).
	p(NT(243), (NT(300)));
//G570: help_arg(243)        => onf_sym(306).
	p(NT(243), (NT(306)));
//G571: help_arg(243)        => qelim_sym(318).
	p(NT(243), (NT(318)));
//G572: __E___E_help_arg_148_149(367) => 's'.
	p(NT(367), (T(12)));
//G573: __E___E_help_arg_148_149(367) => null.
	p(NT(367), (nul));
//G574: __E_help_arg_148(366) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_148_149(367).
	p(NT(366), (T(19)+T(27)+T(5)+T(18)+T(55)+T(6)+T(19)+NT(367)));
//G575: examples_sym(365)    => __E_help_arg_148(366).
	p(NT(365), (NT(366)));
//G576: help_arg(243)        => examples_sym(365).
	p(NT(243), (NT(365)));
//G577: __E_memory_150(369)  => '%'.
	p(NT(369), (T(59)));
//G578: rel_memory_sym(362)  => '%' '-'.
	p(NT(362), (T(59)+T(28)));
//G579: memory_id(371)       => digits(174).
	p(NT(371), (NT(174)));
//G580: __E___E_memory_150_151(370) => memory_id(371).
	p(NT(370), (NT(371)));
//G581: __E___E_memory_150_151(370) => null.
	p(NT(370), (nul));
//G582: __E_memory_150(369)  => rel_memory_sym(362) __E___E_memory_150_151(370).
	p(NT(369), (NT(362)+NT(370)));
//G583: rel_memory(368)      => __E_memory_150(369).
	p(NT(368), (NT(369)));
//G584: memory(268)          => rel_memory(368).
	p(NT(268), (NT(368)));
//G585: abs_memory_sym(361)  => '%'.
	p(NT(361), (T(59)));
//G586: __E_memory_152(373)  => abs_memory_sym(361) memory_id(371).
	p(NT(373), (NT(361)+NT(371)));
//G587: abs_memory(372)      => __E_memory_152(373).
	p(NT(372), (NT(373)));
//G588: memory(268)          => abs_memory(372).
	p(NT(268), (NT(372)));
//G589: option(324)          => bool_option(333).
	p(NT(324), (NT(333)));
//G590: __E_option_153(376)  => 'S'.
	p(NT(376), (T(35)));
//G591: __E_option_153(376)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(376), (T(12)+T(19)+T(52)+T(19)+T(11)+T(10)+T(13)+T(23)));
//G592: severity_opt(375)    => __E_option_153(376).
	p(NT(375), (NT(376)));
//G593: enum_option(374)     => severity_opt(375).
	p(NT(374), (NT(375)));
//G594: option(324)          => enum_option(374).
	p(NT(324), (NT(374)));
//G595: __E_bool_option_154(378) => 's'.
	p(NT(378), (T(12)));
//G596: __E_bool_option_154(378) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(378), (T(12)+T(13)+T(5)+T(13)+T(51)+T(12)));
//G597: status_opt(377)      => __E_bool_option_154(378).
	p(NT(377), (NT(378)));
//G598: bool_option(333)     => status_opt(377).
	p(NT(333), (NT(377)));
//G599: __E_bool_option_155(380) => 'c'.
	p(NT(380), (T(8)));
//G600: __E_bool_option_155(380) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(380), (T(8)+T(17)+T(6)+T(17)+T(11)));
//G601: __E_bool_option_155(380) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(380), (T(8)+T(17)+T(6)+T(17)+T(11)+T(12)));
//G602: colors_opt(379)      => __E_bool_option_155(380).
	p(NT(379), (NT(380)));
//G603: bool_option(333)     => colors_opt(379).
	p(NT(333), (NT(379)));
//G604: __E_bool_option_156(382) => 'V'.
	p(NT(382), (T(60)));
//G605: __E_bool_option_156(382) => 'c' 'h' 'a' 'r' 'v' 'a' 'r'.
	p(NT(382), (T(8)+T(48)+T(5)+T(11)+T(52)+T(5)+T(11)));
//G606: charvar_opt(381)     => __E_bool_option_156(382).
	p(NT(381), (NT(382)));
//G607: bool_option(333)     => charvar_opt(381).
	p(NT(333), (NT(381)));
//G608: __E_bool_option_157(384) => 'H'.
	p(NT(384), (T(61)));
//G609: __E_bool_option_157(384) => 'h' 'i' 'g' 'h' 'l' 'i' 'g' 'h' 't'.
	p(NT(384), (T(48)+T(10)+T(50)+T(48)+T(6)+T(10)+T(50)+T(48)+T(13)));
//G610: __E_bool_option_157(384) => 'h' 'i' 'g' 'h' 'l' 'i' 'g' 'h' 't' 'i' 'n' 'g'.
	p(NT(384), (T(48)+T(10)+T(50)+T(48)+T(6)+T(10)+T(50)+T(48)+T(13)+T(10)+T(49)+T(50)));
//G611: highlighting_opt(383) => __E_bool_option_157(384).
	p(NT(383), (NT(384)));
//G612: bool_option(333)     => highlighting_opt(383).
	p(NT(333), (NT(383)));
//G613: __E_bool_option_158(386) => 'I'.
	p(NT(386), (T(62)));
//G614: __E_bool_option_158(386) => 'i' 'n' 'd' 'e' 'n' 't'.
	p(NT(386), (T(10)+T(49)+T(56)+T(19)+T(49)+T(13)));
//G615: __E_bool_option_158(386) => 'i' 'n' 'd' 'e' 'n' 't' 'i' 'n' 'g'.
	p(NT(386), (T(10)+T(49)+T(56)+T(19)+T(49)+T(13)+T(10)+T(49)+T(50)));
//G616: indenting_opt(385)   => __E_bool_option_158(386).
	p(NT(385), (NT(386)));
//G617: bool_option(333)     => indenting_opt(385).
	p(NT(333), (NT(385)));
//G618: __E_bool_option_159(388) => 'd'.
	p(NT(388), (T(56)));
//G619: __E_bool_option_159(388) => 'd' 'b' 'g'.
	p(NT(388), (T(56)+T(7)+T(50)));
//G620: __E_bool_option_159(388) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(388), (T(56)+T(19)+T(7)+T(51)+T(50)));
//G621: debug_repl_opt(387)  => __E_bool_option_159(388).
	p(NT(387), (NT(388)));
//G622: bool_option(333)     => debug_repl_opt(387).
	p(NT(333), (NT(387)));
//G623: option_value(329)    => option_value_true(389).
	p(NT(329), (NT(389)));
//G624: option_value(329)    => option_value_false(390).
	p(NT(329), (NT(390)));
//G625: option_value(329)    => severity(391).
	p(NT(329), (NT(391)));
//G626: option_value_true(389) => 't'.
	p(NT(389), (T(13)));
//G627: option_value_true(389) => 't' 'r' 'u' 'e'.
	p(NT(389), (T(13)+T(11)+T(51)+T(19)));
//G628: option_value_true(389) => 'o' 'n'.
	p(NT(389), (T(17)+T(49)));
//G629: option_value_true(389) => '1'.
	p(NT(389), (T(38)));
//G630: option_value_true(389) => 'y'.
	p(NT(389), (T(23)));
//G631: option_value_true(389) => 'y' 'e' 's'.
	p(NT(389), (T(23)+T(19)+T(12)));
//G632: option_value_false(390) => 'f'.
	p(NT(390), (T(4)));
//G633: option_value_false(390) => 'f' 'a' 'l' 's' 'e'.
	p(NT(390), (T(4)+T(5)+T(6)+T(12)+T(19)));
//G634: option_value_false(390) => 'o' 'f' 'f'.
	p(NT(390), (T(17)+T(4)+T(4)));
//G635: option_value_false(390) => '0'.
	p(NT(390), (T(39)));
//G636: option_value_false(390) => 'n'.
	p(NT(390), (T(49)));
//G637: option_value_false(390) => 'n' 'o'.
	p(NT(390), (T(49)+T(17)));
//G638: __E_severity_160(393) => 'e'.
	p(NT(393), (T(19)));
//G639: __E_severity_160(393) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(393), (T(19)+T(11)+T(11)+T(17)+T(11)));
//G640: error_sym(392)       => __E_severity_160(393).
	p(NT(392), (NT(393)));
//G641: severity(391)        => error_sym(392).
	p(NT(391), (NT(392)));
//G642: __E_severity_161(395) => 'i'.
	p(NT(395), (T(10)));
//G643: __E_severity_161(395) => 'i' 'n' 'f' 'o'.
	p(NT(395), (T(10)+T(49)+T(4)+T(17)));
//G644: info_sym(394)        => __E_severity_161(395).
	p(NT(394), (NT(395)));
//G645: severity(391)        => info_sym(394).
	p(NT(391), (NT(394)));
//G646: __E_severity_162(397) => 'd'.
	p(NT(397), (T(56)));
//G647: __E_severity_162(397) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(397), (T(56)+T(19)+T(7)+T(51)+T(50)));
//G648: debug_sym(396)       => __E_severity_162(397).
	p(NT(396), (NT(397)));
//G649: severity(391)        => debug_sym(396).
	p(NT(391), (NT(396)));
//G650: __E_severity_163(399) => 't'.
	p(NT(399), (T(13)));
//G651: __E_severity_163(399) => 't' 'r' 'a' 'c' 'e'.
	p(NT(399), (T(13)+T(11)+T(5)+T(8)+T(19)));
//G652: trace_sym(398)       => __E_severity_163(399).
	p(NT(398), (NT(399)));
//G653: severity(391)        => trace_sym(398).
	p(NT(391), (NT(398)));
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
		__E___E_bf_49_50, bf_neg, __E_bf_51, bf_t, bf_f, bf_and_nosep_1st_op, __E_bf_and_52, uninterpreted_constant, ctn_neq, __E_constraint_53, 
		ctnvar, num, ctn_eq, __E_constraint_54, ctn_greater_equal, __E_constraint_55, ctn_greater, __E_constraint_56, ctn_less_equal, __E_constraint_57, 
		ctn_less, __E_constraint_58, __E_variable_59, chars, io_var, __E_variable_60, in, out, in_var_name, __E_in_61, 
		__E___E_in_61_62, offset, out_var_name, __E_out_63, __E___E_out_63_64, q_var, __E_q_vars_65, __E_q_vars_66, __E_offsets_67, __E_offsets_68, 
		shift, __E_offset_69, __E_shift_70, __E___E_shift_70_71, digits, uninter_const_name, __E_uninterpreted_constant_72, __E___E_uninterpreted_constant_72_73, constant, __E_bf_constant_74, 
		binding, __E_bf_constant_75, type, source, __E_binding_76, src_c, __E___E_binding_76_77, __E___E___E_binding_76_77_78, __E___E___E_binding_76_77_79, __E_src_c_80, 
		__E_src_c_81, __E_chars_82, __E_chars_83, __E_digits_84, extra, comment, __E_comment_85, __E_comment_86, __E_comment_87, __E____88, 
		__E___89, bf_cb_args1, bf_cb_arg, bf_cb_args2, bf_cb_args3, bf_cb_args4, wff_cb_args1, wff_cb_arg, wff_cb_args2, wff_cb_args3, 
		wff_cb_args4, wff_has_clashing_subformulas_cb, __E_wff_cb_90, wff_has_subformula_cb, __E_wff_cb_91, wff_remove_existential_cb, __E_wff_cb_92, bf_has_subformula_cb, __E_bf_cb_93, bf_remove_funiversal_cb, 
		__E_bf_cb_94, bf_remove_fexistential_cb, __E_bf_cb_95, bf_normalize_cb, __E_bf_cb_96, cli, cli_command, __E_cli_97, __E_cli_98, quit_cmd, 
		quit_sym, __E_cli_command_99, version_cmd, version_sym, __E_cli_command_100, clear_cmd, clear_sym, __E_cli_command_101, help_cmd, __E_cli_command_102, 
		help_sym, __E___E_cli_command_102_103, __E___E_cli_command_102_104, help_arg, file_cmd, __E_cli_command_105, file_sym, q_string, valid_cmd, __E_cli_command_106, 
		valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_107, sat_sym, unsat_cmd, __E_cli_command_108, unsat_sym, solve_cmd, __E_cli_command_109, 
		solve_sym, __E___E_cli_command_109_110, solve_cmd_arg, run_cmd, __E_cli_command_111, run_sym, __E___E_cli_command_111_112, __E___E_cli_command_111_113, memory, normalize_cmd, 
		__E_cli_command_114, normalize_sym, __E___E_cli_command_114_115, subst_cmd, __E_cli_command_116, subst_sym, __E___E_cli_command_116_117, nf_cmd_arg, inst_cmd, __E_cli_command_118, 
		inst_sym, __E___E_cli_command_118_119, inst_args, dnf_cmd, __E_cli_command_120, dnf_sym, cnf_cmd, __E_cli_command_121, cnf_sym, anf_cmd, 
		__E_cli_command_122, anf_sym, nnf_cmd, __E_cli_command_123, nnf_sym, pnf_cmd, __E_cli_command_124, pnf_sym, mnf_cmd, __E_cli_command_125, 
		mnf_sym, snf_cmd, __E_cli_command_126, snf_sym, onf_cmd, __E_cli_command_127, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_128, 
		def_sym, __E___E_cli_command_128_129, def_print_cmd, __E_cli_command_130, number, def_rr_cmd, qelim_cmd, __E_cli_command_131, qelim_sym, wff_cmd_arg, 
		get_cmd, __E_cli_command_132, get_sym, __E___E_cli_command_132_133, option, set_cmd, __E_cli_command_134, set_sym, __E___E_cli_command_134_135, option_value, 
		enable_cmd, __E_cli_command_136, enable_sym, bool_option, disable_cmd, __E_cli_command_137, disable_sym, toggle_cmd, __E_cli_command_138, toggle_sym, 
		def_input_cmd, __E_cli_command_139, input_stream, def_output_cmd, __E_cli_command_140, output_stream, history_list_cmd, __E_cli_command_141, history_sym, __E___E_cli_command_141_142, 
		history_print_cmd, __E_cli_command_143, history_store_cmd, __E_cli_command_144, __E_input_stream_145, q_file_name, console_sym, __E_output_stream_146, file_name, __E_file_name_147, 
		bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, execute_sym, examples_sym, __E_help_arg_148, __E___E_help_arg_148_149, rel_memory, __E_memory_150, 
		__E___E_memory_150_151, memory_id, abs_memory, __E_memory_152, enum_option, severity_opt, __E_option_153, status_opt, __E_bool_option_154, colors_opt, 
		__E_bool_option_155, charvar_opt, __E_bool_option_156, highlighting_opt, __E_bool_option_157, indenting_opt, __E_bool_option_158, debug_repl_opt, __E_bool_option_159, option_value_true, 
		option_value_false, severity, error_sym, __E_severity_160, info_sym, __E_severity_161, debug_sym, __E_severity_162, trace_sym, __E_severity_163, 
		named_binding, __N_0, __N_1, __N_2, __N_3, __N_4, 
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
