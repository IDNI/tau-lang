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
	"__E_wff_32", "bf_eq", "__E_wff_33", "bf_neq", "__E_wff_34", "bf_less_equal", "__E_wff_35", "bf_nleq", "__E_wff_36", "bf_greater", 
	"__E_wff_37", "bf_ngreater", "__E_wff_38", "bf_greater_equal", "__E_wff_39", "bf_ngeq", "__E_wff_40", "bf_less", "__E_wff_41", "bf_nless", 
	"__E_wff_42", "bf_parenthesis", "__E_bf_43", "bf_constant", "variable", "bf_splitter", "__E_bf_44", "bf_ref", "bf_or", "__E_bf_45", 
	"bf_xor", "__E_bf_46", "bf_and", "__E_bf_47", "__E___E_bf_47_48", "bf_neg", "__E_bf_49", "bf_t", "bf_f", "uninterpreted_constant", 
	"bf_and_nosep_1st_op", "__E_bf_and_50", "ctn_neq", "__E_constraint_51", "ctnvar", "num", "ctn_eq", "__E_constraint_52", "ctn_greater_equal", "__E_constraint_53", 
	"ctn_greater", "__E_constraint_54", "ctn_less_equal", "__E_constraint_55", "ctn_less", "__E_constraint_56", "__E_variable_57", "chars", "io_var", "__E_variable_58", 
	"in", "out", "in_var_name", "__E_in_59", "__E___E_in_59_60", "offset", "out_var_name", "__E_out_61", "__E___E_out_61_62", "q_var", 
	"__E_q_vars_63", "__E_q_vars_64", "__E_offsets_65", "__E_offsets_66", "shift", "__E_offset_67", "__E_shift_68", "__E___E_shift_68_69", "digits", "uninter_const_name", 
	"__E_uninterpreted_constant_70", "__E___E_uninterpreted_constant_70_71", "constant", "__E_bf_constant_72", "binding", "__E_bf_constant_73", "type", "source", "__E_binding_74", "src_c", 
	"__E___E_binding_74_75", "__E___E___E_binding_74_75_76", "__E___E___E_binding_74_75_77", "__E_src_c_78", "__E_src_c_79", "__E_chars_80", "__E_chars_81", "__E_digits_82", "extra", "comment", 
	"__E_comment_83", "__E_comment_84", "__E_comment_85", "__E____86", "__E___87", "bf_cb_args1", "bf_cb_arg", "bf_cb_args2", "bf_cb_args3", "bf_cb_args4", 
	"wff_cb_args1", "wff_cb_arg", "wff_cb_args2", "wff_cb_args3", "wff_cb_args4", "wff_has_clashing_subformulas_cb", "__E_wff_cb_88", "wff_has_subformula_cb", "__E_wff_cb_89", "wff_remove_existential_cb", 
	"__E_wff_cb_90", "bf_has_subformula_cb", "__E_bf_cb_91", "bf_remove_funiversal_cb", "__E_bf_cb_92", "bf_remove_fexistential_cb", "__E_bf_cb_93", "bf_normalize_cb", "__E_bf_cb_94", "cli", 
	"cli_command", "__E_cli_95", "__E_cli_96", "quit_cmd", "quit_sym", "__E_cli_command_97", "version_cmd", "version_sym", "__E_cli_command_98", "clear_cmd", 
	"clear_sym", "__E_cli_command_99", "help_cmd", "__E_cli_command_100", "help_sym", "__E___E_cli_command_100_101", "__E___E_cli_command_100_102", "help_arg", "file_cmd", "__E_cli_command_103", 
	"file_sym", "q_string", "valid_cmd", "__E_cli_command_104", "valid_sym", "sat_cmd", "__E_cli_command_105", "sat_sym", "normalize_cmd_arg", "unsat_cmd", 
	"__E_cli_command_106", "unsat_sym", "solve_cmd", "__E_cli_command_107", "solve_sym", "__E___E_cli_command_107_108", "solve_cmd_arg", "run_cmd", "__E_cli_command_109", "run_sym", 
	"__E___E_cli_command_109_110", "normalize_cmd", "__E_cli_command_111", "normalize_sym", "__E___E_cli_command_111_112", "subst_cmd", "__E_cli_command_113", "subst_sym", "__E___E_cli_command_113_114", "nf_cmd_arg", 
	"inst_cmd", "__E_cli_command_115", "inst_sym", "__E___E_cli_command_115_116", "inst_args", "dnf_cmd", "__E_cli_command_117", "dnf_sym", "cnf_cmd", "__E_cli_command_118", 
	"cnf_sym", "anf_cmd", "__E_cli_command_119", "anf_sym", "nnf_cmd", "__E_cli_command_120", "nnf_sym", "pnf_cmd", "__E_cli_command_121", "pnf_sym", 
	"mnf_cmd", "__E_cli_command_122", "mnf_sym", "snf_cmd", "__E_cli_command_123", "snf_sym", "onf_cmd", "__E_cli_command_124", "onf_sym", "onf_cmd_arg", 
	"def_list_cmd", "__E_cli_command_125", "def_sym", "__E___E_cli_command_125_126", "def_print_cmd", "__E_cli_command_127", "number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_128", 
	"qelim_sym", "wff_cmd_arg", "get_cmd", "__E_cli_command_129", "get_sym", "__E___E_cli_command_129_130", "option", "set_cmd", "__E_cli_command_131", "set_sym", 
	"__E___E_cli_command_131_132", "option_value", "enable_cmd", "__E_cli_command_133", "enable_sym", "bool_option", "disable_cmd", "__E_cli_command_134", "disable_sym", "toggle_cmd", 
	"__E_cli_command_135", "toggle_sym", "def_input_cmd", "__E_cli_command_136", "input_stream", "def_output_cmd", "__E_cli_command_137", "output_stream", "history_list_cmd", "__E_cli_command_138", 
	"history_sym", "__E___E_cli_command_138_139", "history_print_cmd", "__E_cli_command_140", "memory", "history_store_cmd", "__E_cli_command_141", "__E_input_stream_142", "q_file_name", "console_sym", 
	"__E_output_stream_143", "file_name", "__E_file_name_144", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "execute_sym", "examples_sym", "__E_help_arg_145", 
	"__E___E_help_arg_145_146", "rel_memory", "__E_memory_147", "__E___E_memory_147_148", "memory_id", "abs_memory", "__E_memory_149", "enum_option", "severity_opt", "__E_option_150", 
	"status_opt", "__E_bool_option_151", "colors_opt", "__E_bool_option_152", "charvar_opt", "__E_bool_option_153", "hilighting_opt", "__E_bool_option_154", "indenting_opt", "__E_bool_option_155", 
	"debug_repl_opt", "__E_bool_option_156", "option_value_true", "option_value_false", "severity", "error_sym", "__E_severity_157", "info_sym", "__E_severity_158", "debug_sym", 
	"__E_severity_159", "trace_sym", "__E_severity_160", "named_binding", "__N_0", "__N_1", "__N_2", "__N_3", 
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
			20, 22, 114, 152, 156, 168, 169, 176, 177, 351,
			393
		},
		.to_inline = {
			{ 14, 58, 14 },
			{ 21, 111, 21 },
			{ 43 },
			{ 47 },
			{ 147 },
			{ 159 },
			{ 179 },
			{ 195 },
			{ 197 },
			{ 198 },
			{ 199 },
			{ 200 },
			{ 202 },
			{ 203 },
			{ 204 },
			{ 248 },
			{ 256 },
			{ 269 },
			{ 274 },
			{ 299 },
			{ 311 },
			{ 353 }
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
//G103: __E_wff_33(92)       => bf(21) _(11) '=' _(11) bf(21).
	p(NT(92), (NT(21)+NT(11)+T(3)+NT(11)+NT(21)));
//G104: bf_eq(91)            => __E_wff_33(92).
	p(NT(91), (NT(92)));
//G105: wff(14)              => bf_eq(91).
	p(NT(14), (NT(91)));
//G106: __E_wff_34(94)       => bf(21) _(11) '!' '=' _(11) bf(21).
	p(NT(94), (NT(21)+NT(11)+T(27)+T(3)+NT(11)+NT(21)));
//G107: bf_neq(93)           => __E_wff_34(94).
	p(NT(93), (NT(94)));
//G108: wff(14)              => bf_neq(93).
	p(NT(14), (NT(93)));
//G109: __E_wff_35(96)       => bf(21) _(11) '<' '=' _(11) bf(21).
	p(NT(96), (NT(21)+NT(11)+T(13)+T(3)+NT(11)+NT(21)));
//G110: bf_less_equal(95)    => __E_wff_35(96).
	p(NT(95), (NT(96)));
//G111: wff(14)              => bf_less_equal(95).
	p(NT(14), (NT(95)));
//G112: __E_wff_36(98)       => bf(21) _(11) '!' '<' '=' _(11) bf(21).
	p(NT(98), (NT(21)+NT(11)+T(27)+T(13)+T(3)+NT(11)+NT(21)));
//G113: bf_nleq(97)          => __E_wff_36(98).
	p(NT(97), (NT(98)));
//G114: wff(14)              => bf_nleq(97).
	p(NT(14), (NT(97)));
//G115: __E_wff_37(100)      => bf(21) _(11) '>' _(11) bf(21).
	p(NT(100), (NT(21)+NT(11)+T(14)+NT(11)+NT(21)));
//G116: bf_greater(99)       => __E_wff_37(100).
	p(NT(99), (NT(100)));
//G117: wff(14)              => bf_greater(99).
	p(NT(14), (NT(99)));
//G118: __E_wff_38(102)      => bf(21) _(11) '!' '>' _(11) bf(21).
	p(NT(102), (NT(21)+NT(11)+T(27)+T(14)+NT(11)+NT(21)));
//G119: bf_ngreater(101)     => __E_wff_38(102).
	p(NT(101), (NT(102)));
//G120: wff(14)              => bf_ngreater(101).
	p(NT(14), (NT(101)));
//G121: __E_wff_39(104)      => bf(21) _(11) '>' '=' _(11) bf(21).
	p(NT(104), (NT(21)+NT(11)+T(14)+T(3)+NT(11)+NT(21)));
//G122: bf_greater_equal(103) => __E_wff_39(104).
	p(NT(103), (NT(104)));
//G123: wff(14)              => bf_greater_equal(103).
	p(NT(14), (NT(103)));
//G124: __E_wff_40(106)      => bf(21) _(11) '!' '>' '=' _(11) bf(21).
	p(NT(106), (NT(21)+NT(11)+T(27)+T(14)+T(3)+NT(11)+NT(21)));
//G125: bf_ngeq(105)         => __E_wff_40(106).
	p(NT(105), (NT(106)));
//G126: wff(14)              => bf_ngeq(105).
	p(NT(14), (NT(105)));
//G127: __E_wff_41(108)      => bf(21) _(11) '<' _(11) bf(21).
	p(NT(108), (NT(21)+NT(11)+T(13)+NT(11)+NT(21)));
//G128: bf_less(107)         => __E_wff_41(108).
	p(NT(107), (NT(108)));
//G129: wff(14)              => bf_less(107).
	p(NT(14), (NT(107)));
//G130: __E_wff_42(110)      => bf(21) _(11) '!' '<' _(11) bf(21).
	p(NT(110), (NT(21)+NT(11)+T(27)+T(13)+NT(11)+NT(21)));
//G131: bf_nless(109)        => __E_wff_42(110).
	p(NT(109), (NT(110)));
//G132: wff(14)              => bf_nless(109).
	p(NT(14), (NT(109)));
//G133: wff(14)              => capture(20).
	p(NT(14), (NT(20)));
//G134: __E_bf_43(112)       => '(' _(11) bf(21) _(11) ')'.
	p(NT(112), (T(5)+NT(11)+NT(21)+NT(11)+T(6)));
//G135: bf_parenthesis(111)  => __E_bf_43(112).
	p(NT(111), (NT(112)));
//G136: bf(21)               => bf_parenthesis(111).
	p(NT(21), (NT(111)));
//G137: bf(21)               => bf_constant(113).
	p(NT(21), (NT(113)));
//G138: bf(21)               => variable(114).
	p(NT(21), (NT(114)));
//G139: __E_bf_44(116)       => 'S' _(11) '(' _(11) bf(21) _(11) ')'.
	p(NT(116), (T(30)+NT(11)+T(5)+NT(11)+NT(21)+NT(11)+T(6)));
//G140: bf_splitter(115)     => __E_bf_44(116).
	p(NT(115), (NT(116)));
//G141: bf(21)               => bf_splitter(115).
	p(NT(21), (NT(115)));
//G142: bf_ref(117)          => ref(18).
	p(NT(117), (NT(18)));
//G143: bf(21)               => bf_ref(117).
	p(NT(21), (NT(117)));
//G144: __E_bf_45(119)       => bf(21) _(11) '|' _(11) bf(21).
	p(NT(119), (NT(21)+NT(11)+T(24)+NT(11)+NT(21)));
//G145: bf_or(118)           => __E_bf_45(119).
	p(NT(118), (NT(119)));
//G146: bf(21)               => bf_or(118).
	p(NT(21), (NT(118)));
//G147: __E_bf_46(121)       => bf(21) _(11) '+' _(11) bf(21).
	p(NT(121), (NT(21)+NT(11)+T(31)+NT(11)+NT(21)));
//G148: bf_xor(120)          => __E_bf_46(121).
	p(NT(120), (NT(121)));
//G149: bf(21)               => bf_xor(120).
	p(NT(21), (NT(120)));
//G150: __E___E_bf_47_48(124) => _(11) '&' _(11).
	p(NT(124), (NT(11)+T(26)+NT(11)));
//G151: __E___E_bf_47_48(124) => __(52).
	p(NT(124), (NT(52)));
//G152: __E_bf_47(123)       => bf(21) __E___E_bf_47_48(124) bf(21).
	p(NT(123), (NT(21)+NT(124)+NT(21)));
//G153: bf_and(122)          => __E_bf_47(123).
	p(NT(122), (NT(123)));
//G154: bf(21)               => bf_and(122).
	p(NT(21), (NT(122)));
//G155: __E_bf_49(126)       => bf(21) _(11) '\''.
	p(NT(126), (NT(21)+NT(11)+T(32)));
//G156: bf_neg(125)          => __E_bf_49(126).
	p(NT(125), (NT(126)));
//G157: bf(21)               => bf_neg(125).
	p(NT(21), (NT(125)));
//G158: bf_t(127)            => '1'.
	p(NT(127), (T(33)));
//G159: bf(21)               => bf_t(127).
	p(NT(21), (NT(127)));
//G160: bf_f(128)            => '0'.
	p(NT(128), (T(34)));
//G161: bf(21)               => bf_f(128).
	p(NT(21), (NT(128)));
//G162: bf(21)               => capture(20).
	p(NT(21), (NT(20)));
//G163: bf(21)               => uninterpreted_constant(129).
	p(NT(21), (NT(129)));
//G164: __E_bf_and_50(131)   => bf_parenthesis(111).
	p(NT(131), (NT(111)));
//G165: __E_bf_and_50(131)   => variable(114).
	p(NT(131), (NT(114)));
//G166: __E_bf_and_50(131)   => bf_splitter(115).
	p(NT(131), (NT(115)));
//G167: __E_bf_and_50(131)   => bf_ref(117).
	p(NT(131), (NT(117)));
//G168: __E_bf_and_50(131)   => bf_or(118).
	p(NT(131), (NT(118)));
//G169: __E_bf_and_50(131)   => bf_xor(120).
	p(NT(131), (NT(120)));
//G170: __E_bf_and_50(131)   => bf_neg(125).
	p(NT(131), (NT(125)));
//G171: __E_bf_and_50(131)   => capture(20).
	p(NT(131), (NT(20)));
//G172: __E_bf_and_50(131)   => uninterpreted_constant(129).
	p(NT(131), (NT(129)));
//G173: bf_and_nosep_1st_op(130) => __E_bf_and_50(131).
	p(NT(130), (NT(131)));
//G174: bf_and(122)          => bf_and_nosep_1st_op(130) bf(21).
	p(NT(122), (NT(130)+NT(21)));
//G175: __E_constraint_51(133) => '[' ctnvar(134) _(11) '!' '=' _(11) num(135) ']'.
	p(NT(133), (T(19)+NT(134)+NT(11)+T(27)+T(3)+NT(11)+NT(135)+T(20)));
//G176: __E_constraint_51(133) => '[' num(135) _(11) '!' '=' _(11) ctnvar(134) ']'.
	p(NT(133), (T(19)+NT(135)+NT(11)+T(27)+T(3)+NT(11)+NT(134)+T(20)));
//G177: ctn_neq(132)         => __E_constraint_51(133).
	p(NT(132), (NT(133)));
//G178: constraint(74)       => ctn_neq(132).
	p(NT(74), (NT(132)));
//G179: __E_constraint_52(137) => '[' ctnvar(134) _(11) '=' _(11) num(135) ']'.
	p(NT(137), (T(19)+NT(134)+NT(11)+T(3)+NT(11)+NT(135)+T(20)));
//G180: __E_constraint_52(137) => '[' num(135) _(11) '=' _(11) ctnvar(134) ']'.
	p(NT(137), (T(19)+NT(135)+NT(11)+T(3)+NT(11)+NT(134)+T(20)));
//G181: ctn_eq(136)          => __E_constraint_52(137).
	p(NT(136), (NT(137)));
//G182: constraint(74)       => ctn_eq(136).
	p(NT(74), (NT(136)));
//G183: __E_constraint_53(139) => '[' ctnvar(134) _(11) '>' '=' _(11) num(135) ']'.
	p(NT(139), (T(19)+NT(134)+NT(11)+T(14)+T(3)+NT(11)+NT(135)+T(20)));
//G184: __E_constraint_53(139) => '[' num(135) _(11) '>' '=' _(11) ctnvar(134) ']'.
	p(NT(139), (T(19)+NT(135)+NT(11)+T(14)+T(3)+NT(11)+NT(134)+T(20)));
//G185: ctn_greater_equal(138) => __E_constraint_53(139).
	p(NT(138), (NT(139)));
//G186: constraint(74)       => ctn_greater_equal(138).
	p(NT(74), (NT(138)));
//G187: __E_constraint_54(141) => '[' ctnvar(134) _(11) '>' _(11) num(135) ']'.
	p(NT(141), (T(19)+NT(134)+NT(11)+T(14)+NT(11)+NT(135)+T(20)));
//G188: __E_constraint_54(141) => '[' num(135) _(11) '>' _(11) ctnvar(134) ']'.
	p(NT(141), (T(19)+NT(135)+NT(11)+T(14)+NT(11)+NT(134)+T(20)));
//G189: ctn_greater(140)     => __E_constraint_54(141).
	p(NT(140), (NT(141)));
//G190: constraint(74)       => ctn_greater(140).
	p(NT(74), (NT(140)));
//G191: __E_constraint_55(143) => '[' ctnvar(134) _(11) '<' '=' _(11) num(135) ']'.
	p(NT(143), (T(19)+NT(134)+NT(11)+T(13)+T(3)+NT(11)+NT(135)+T(20)));
//G192: __E_constraint_55(143) => '[' num(135) _(11) '<' '=' _(11) ctnvar(134) ']'.
	p(NT(143), (T(19)+NT(135)+NT(11)+T(13)+T(3)+NT(11)+NT(134)+T(20)));
//G193: ctn_less_equal(142)  => __E_constraint_55(143).
	p(NT(142), (NT(143)));
//G194: constraint(74)       => ctn_less_equal(142).
	p(NT(74), (NT(142)));
//G195: __E_constraint_56(145) => '[' ctnvar(134) _(11) '<' _(11) num(135) ']'.
	p(NT(145), (T(19)+NT(134)+NT(11)+T(13)+NT(11)+NT(135)+T(20)));
//G196: __E_constraint_56(145) => '[' num(135) _(11) '<' _(11) ctnvar(134) ']'.
	p(NT(145), (T(19)+NT(135)+NT(11)+T(13)+NT(11)+NT(134)+T(20)));
//G197: ctn_less(144)        => __E_constraint_56(145).
	p(NT(144), (NT(145)));
//G198: constraint(74)       => ctn_less(144).
	p(NT(74), (NT(144)));
//G199: __E_variable_57(146) => null.
	p(NT(146), (nul));
//G200: __E_variable_57(146) => digit(3) __E_variable_57(146).
	p(NT(146), (NT(3)+NT(146)));
//G201: variable(114)        => alpha(5) __E_variable_57(146).	 # guarded: charvar
	p(NT(114), (NT(5)+NT(146)));
	p.back().guard = "charvar";
//G202: variable(114)        => chars(147).	 # guarded: var
	p(NT(114), (NT(147)));
	p.back().guard = "var";
//G203: __E_variable_58(149) => in(150).
	p(NT(149), (NT(150)));
//G204: __E_variable_58(149) => out(151).
	p(NT(149), (NT(151)));
//G205: io_var(148)          => __E_variable_58(149).
	p(NT(148), (NT(149)));
//G206: variable(114)        => io_var(148).
	p(NT(114), (NT(148)));
//G207: __E___E_in_59_60(154) => digit(3).
	p(NT(154), (NT(3)));
//G208: __E___E_in_59_60(154) => digit(3) __E___E_in_59_60(154).
	p(NT(154), (NT(3)+NT(154)));
//G209: __E_in_59(153)       => 'i' __E___E_in_59_60(154).
	p(NT(153), (T(12)+NT(154)));
//G210: in_var_name(152)     => __E_in_59(153).
	p(NT(152), (NT(153)));
//G211: in(150)              => in_var_name(152) '[' offset(155) ']'.
	p(NT(150), (NT(152)+T(19)+NT(155)+T(20)));
//G212: __E___E_out_61_62(158) => digit(3).
	p(NT(158), (NT(3)));
//G213: __E___E_out_61_62(158) => digit(3) __E___E_out_61_62(158).
	p(NT(158), (NT(3)+NT(158)));
//G214: __E_out_61(157)      => 'o' __E___E_out_61_62(158).
	p(NT(157), (T(8)+NT(158)));
//G215: out_var_name(156)    => __E_out_61(157).
	p(NT(156), (NT(157)));
//G216: out(151)             => out_var_name(156) '[' offset(155) ']'.
	p(NT(151), (NT(156)+T(19)+NT(155)+T(20)));
//G217: ctnvar(134)          => chars(147).
	p(NT(134), (NT(147)));
//G218: capture(20)          => '$' chars(147).
	p(NT(20), (T(35)+NT(147)));
//G219: __E_q_vars_63(160)   => _(11) ',' _(11) q_var(159).
	p(NT(160), (NT(11)+T(4)+NT(11)+NT(159)));
//G220: __E_q_vars_64(161)   => null.
	p(NT(161), (nul));
//G221: __E_q_vars_64(161)   => __E_q_vars_63(160) __E_q_vars_64(161).
	p(NT(161), (NT(160)+NT(161)));
//G222: q_vars(70)           => q_var(159) __E_q_vars_64(161).
	p(NT(70), (NT(159)+NT(161)));
//G223: q_var(159)           => capture(20).
	p(NT(159), (NT(20)));
//G224: q_var(159)           => variable(114).
	p(NT(159), (NT(114)));
//G225: __E_offsets_65(162)  => _(11) ',' _(11) offset(155).
	p(NT(162), (NT(11)+T(4)+NT(11)+NT(155)));
//G226: __E_offsets_66(163)  => null.
	p(NT(163), (nul));
//G227: __E_offsets_66(163)  => __E_offsets_65(162) __E_offsets_66(163).
	p(NT(163), (NT(162)+NT(163)));
//G228: offsets(24)          => '[' _(11) offset(155) __E_offsets_66(163) _(11) ']'.
	p(NT(24), (T(19)+NT(11)+NT(155)+NT(163)+NT(11)+T(20)));
//G229: offset(155)          => num(135).
	p(NT(155), (NT(135)));
//G230: offset(155)          => capture(20).
	p(NT(155), (NT(20)));
//G231: offset(155)          => shift(164).
	p(NT(155), (NT(164)));
//G232: __N_0(394)           => io_var(148).
	p(NT(394), (NT(148)));
//G233: __E_offset_67(165)   => variable(114) & ~( __N_0(394) ).	 # conjunctive
	p(NT(165), (NT(114)) & ~(NT(394)));
//G234: offset(155)          => __E_offset_67(165).
	p(NT(155), (NT(165)));
//G235: __E_shift_68(166)    => capture(20).
	p(NT(166), (NT(20)));
//G236: __N_1(395)           => io_var(148).
	p(NT(395), (NT(148)));
//G237: __E___E_shift_68_69(167) => variable(114) & ~( __N_1(395) ).	 # conjunctive
	p(NT(167), (NT(114)) & ~(NT(395)));
//G238: __E_shift_68(166)    => __E___E_shift_68_69(167).
	p(NT(166), (NT(167)));
//G239: shift(164)           => __E_shift_68(166) _(11) '-' _(11) num(135).
	p(NT(164), (NT(166)+NT(11)+T(23)+NT(11)+NT(135)));
//G240: num(135)             => digits(168).
	p(NT(135), (NT(168)));
//G241: __E___E_uninterpreted_constant_70_71(171) => chars(147).
	p(NT(171), (NT(147)));
//G242: __E___E_uninterpreted_constant_70_71(171) => _(11).
	p(NT(171), (NT(11)));
//G243: __E_uninterpreted_constant_70(170) => __E___E_uninterpreted_constant_70_71(171) ':' chars(147).
	p(NT(170), (NT(171)+T(2)+NT(147)));
//G244: uninter_const_name(169) => __E_uninterpreted_constant_70(170).
	p(NT(169), (NT(170)));
//G245: uninterpreted_constant(129) => '<' uninter_const_name(169) _(11) '>'.
	p(NT(129), (T(13)+NT(169)+NT(11)+T(14)));
//G246: __E_bf_constant_72(173) => capture(20).
	p(NT(173), (NT(20)));
//G247: __E_bf_constant_72(173) => binding(174).
	p(NT(173), (NT(174)));
//G248: constant(172)        => __E_bf_constant_72(173).
	p(NT(172), (NT(173)));
//G249: type(176)            => chars(147).
	p(NT(176), (NT(147)));
//G250: __E_bf_constant_73(175) => _(11) ':' _(11) type(176).
	p(NT(175), (NT(11)+T(2)+NT(11)+NT(176)));
//G251: __E_bf_constant_73(175) => null.
	p(NT(175), (nul));
//G252: bf_constant(113)     => '{' _(11) constant(172) _(11) '}' __E_bf_constant_73(175).
	p(NT(113), (T(36)+NT(11)+NT(172)+NT(11)+T(37)+NT(175)));
//G253: __E___E___E_binding_74_75_76(181) => src_c(179).
	p(NT(181), (NT(179)));
//G254: __E___E___E_binding_74_75_76(181) => space(2).
	p(NT(181), (NT(2)));
//G255: __E___E___E_binding_74_75_77(182) => null.
	p(NT(182), (nul));
//G256: __E___E___E_binding_74_75_77(182) => __E___E___E_binding_74_75_76(181) __E___E___E_binding_74_75_77(182).
	p(NT(182), (NT(181)+NT(182)));
//G257: __E___E_binding_74_75(180) => __E___E___E_binding_74_75_77(182) src_c(179).
	p(NT(180), (NT(182)+NT(179)));
//G258: __E___E_binding_74_75(180) => null.
	p(NT(180), (nul));
//G259: __E_binding_74(178)  => src_c(179) __E___E_binding_74_75(180).
	p(NT(178), (NT(179)+NT(180)));
//G260: source(177)          => __E_binding_74(178).
	p(NT(177), (NT(178)));
//G261: binding(174)         => source(177).
	p(NT(174), (NT(177)));
//G262: src_c(179)           => alnum(6).
	p(NT(179), (NT(6)));
//G263: __N_2(396)           => '{'.
	p(NT(396), (T(36)));
//G264: __N_3(397)           => '}'.
	p(NT(397), (T(37)));
//G265: src_c(179)           => ~( __N_2(396) ) & ~( __N_3(397) ) & punct(7).	 # conjunctive
	p(NT(179), ~(NT(396)) & ~(NT(397)) & (NT(7)));
//G266: __E_src_c_78(183)    => src_c(179).
	p(NT(183), (NT(179)));
//G267: __E_src_c_78(183)    => space(2).
	p(NT(183), (NT(2)));
//G268: __E_src_c_79(184)    => null.
	p(NT(184), (nul));
//G269: __E_src_c_79(184)    => __E_src_c_78(183) __E_src_c_79(184).
	p(NT(184), (NT(183)+NT(184)));
//G270: src_c(179)           => '{' __E_src_c_79(184) '}'.
	p(NT(179), (T(36)+NT(184)+T(37)));
//G271: __E_chars_80(185)    => alnum(6).
	p(NT(185), (NT(6)));
//G272: __E_chars_80(185)    => '_'.
	p(NT(185), (T(38)));
//G273: __E_chars_81(186)    => null.
	p(NT(186), (nul));
//G274: __E_chars_81(186)    => __E_chars_80(185) __E_chars_81(186).
	p(NT(186), (NT(185)+NT(186)));
//G275: chars(147)           => alpha(5) __E_chars_81(186).
	p(NT(147), (NT(5)+NT(186)));
//G276: __E_digits_82(187)   => digit(3).
	p(NT(187), (NT(3)));
//G277: __E_digits_82(187)   => digit(3) __E_digits_82(187).
	p(NT(187), (NT(3)+NT(187)));
//G278: digits(168)          => __E_digits_82(187).
	p(NT(168), (NT(187)));
//G279: sym(22)              => chars(147).
	p(NT(22), (NT(147)));
//G280: extra(188)           => chars(147).
	p(NT(188), (NT(147)));
//G281: __E_comment_83(190)  => printable(8).
	p(NT(190), (NT(8)));
//G282: __E_comment_83(190)  => '\t'.
	p(NT(190), (T(39)));
//G283: __E_comment_84(191)  => null.
	p(NT(191), (nul));
//G284: __E_comment_84(191)  => __E_comment_83(190) __E_comment_84(191).
	p(NT(191), (NT(190)+NT(191)));
//G285: __E_comment_85(192)  => '\n'.
	p(NT(192), (T(40)));
//G286: __E_comment_85(192)  => '\r'.
	p(NT(192), (T(41)));
//G287: __E_comment_85(192)  => eof(1).
	p(NT(192), (NT(1)));
//G288: comment(189)         => '#' __E_comment_84(191) __E_comment_85(192).
	p(NT(189), (T(42)+NT(191)+NT(192)));
//G289: __E____86(193)       => space(2).
	p(NT(193), (NT(2)));
//G290: __E____86(193)       => comment(189).
	p(NT(193), (NT(189)));
//G291: __(52)               => __E____86(193) _(11).
	p(NT(52), (NT(193)+NT(11)));
//G292: __E___87(194)        => __(52).
	p(NT(194), (NT(52)));
//G293: __E___87(194)        => null.
	p(NT(194), (nul));
//G294: _(11)                => __E___87(194).
	p(NT(11), (NT(194)));
//G295: bf_cb_arg(196)       => bf(21).
	p(NT(196), (NT(21)));
//G296: bf_cb_args1(195)     => __(52) bf_cb_arg(196).
	p(NT(195), (NT(52)+NT(196)));
//G297: bf_cb_args2(197)     => bf_cb_args1(195) bf_cb_args1(195).
	p(NT(197), (NT(195)+NT(195)));
//G298: bf_cb_args3(198)     => bf_cb_args2(197) bf_cb_args1(195).
	p(NT(198), (NT(197)+NT(195)));
//G299: bf_cb_args4(199)     => bf_cb_args3(198) bf_cb_args1(195).
	p(NT(199), (NT(198)+NT(195)));
//G300: wff_cb_arg(201)      => wff(14).
	p(NT(201), (NT(14)));
//G301: wff_cb_args1(200)    => __(52) wff_cb_arg(201).
	p(NT(200), (NT(52)+NT(201)));
//G302: wff_cb_args2(202)    => wff_cb_args1(200) wff_cb_args1(200).
	p(NT(202), (NT(200)+NT(200)));
//G303: wff_cb_args3(203)    => wff_cb_args2(202) wff_cb_args1(200).
	p(NT(203), (NT(202)+NT(200)));
//G304: wff_cb_args4(204)    => wff_cb_args3(203) wff_cb_args1(200).
	p(NT(204), (NT(203)+NT(200)));
//G305: __E_wff_cb_88(206)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(202).
	p(NT(206), (T(17)+T(43)+T(43)+T(38)+T(44)+T(15)+T(7)+T(38)+T(45)+T(16)+T(15)+T(7)+T(44)+T(12)+T(46)+T(47)+T(38)+T(7)+T(48)+T(49)+T(43)+T(8)+T(50)+T(9)+T(48)+T(16)+T(15)+T(7)+T(38)+T(45)+T(49)+NT(202)));
//G306: wff_has_clashing_subformulas_cb(205) => __E_wff_cb_88(206).
	p(NT(205), (NT(206)));
//G307: wff_cb(43)           => wff_has_clashing_subformulas_cb(205).
	p(NT(43), (NT(205)));
//G308: __E_wff_cb_89(208)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(203).
	p(NT(208), (T(17)+T(43)+T(43)+T(38)+T(44)+T(15)+T(7)+T(38)+T(7)+T(48)+T(49)+T(43)+T(8)+T(50)+T(9)+T(48)+T(16)+T(15)+T(38)+T(45)+T(49)+NT(203)));
//G309: wff_has_subformula_cb(207) => __E_wff_cb_89(208).
	p(NT(207), (NT(208)));
//G310: wff_cb(43)           => wff_has_subformula_cb(207).
	p(NT(43), (NT(207)));
//G311: __E_wff_cb_90(210)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(202).
	p(NT(210), (T(17)+T(43)+T(43)+T(38)+T(50)+T(10)+T(9)+T(8)+T(51)+T(10)+T(38)+T(10)+T(22)+T(12)+T(7)+T(11)+T(10)+T(46)+T(11)+T(12)+T(15)+T(16)+T(38)+T(45)+T(49)+NT(202)));
//G312: wff_remove_existential_cb(209) => __E_wff_cb_90(210).
	p(NT(209), (NT(210)));
//G313: wff_cb(43)           => wff_remove_existential_cb(209).
	p(NT(43), (NT(209)));
//G314: __E_bf_cb_91(212)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(198).
	p(NT(212), (T(49)+T(43)+T(38)+T(44)+T(15)+T(7)+T(38)+T(7)+T(48)+T(49)+T(43)+T(8)+T(50)+T(9)+T(48)+T(16)+T(15)+T(38)+T(45)+T(49)+NT(198)));
//G315: bf_has_subformula_cb(211) => __E_bf_cb_91(212).
	p(NT(211), (NT(212)));
//G316: bf_cb(47)            => bf_has_subformula_cb(211).
	p(NT(47), (NT(211)));
//G317: __E_bf_cb_92(214)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(199).
	p(NT(214), (T(49)+T(43)+T(38)+T(50)+T(10)+T(9)+T(8)+T(51)+T(10)+T(38)+T(43)+T(48)+T(46)+T(12)+T(51)+T(10)+T(50)+T(7)+T(15)+T(16)+T(38)+T(45)+T(49)+NT(199)));
//G318: bf_remove_funiversal_cb(213) => __E_bf_cb_92(214).
	p(NT(213), (NT(214)));
//G319: bf_cb(47)            => bf_remove_funiversal_cb(213).
	p(NT(47), (NT(213)));
//G320: __E_bf_cb_93(216)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(199).
	p(NT(216), (T(49)+T(43)+T(38)+T(50)+T(10)+T(9)+T(8)+T(51)+T(10)+T(38)+T(43)+T(10)+T(22)+T(12)+T(7)+T(11)+T(10)+T(46)+T(11)+T(12)+T(15)+T(16)+T(38)+T(45)+T(49)+NT(199)));
//G321: bf_remove_fexistential_cb(215) => __E_bf_cb_93(216).
	p(NT(215), (NT(216)));
//G322: bf_cb(47)            => bf_remove_fexistential_cb(215).
	p(NT(47), (NT(215)));
//G323: __E_bf_cb_94(218)    => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(195).
	p(NT(218), (T(49)+T(43)+T(38)+T(46)+T(8)+T(50)+T(9)+T(15)+T(16)+T(12)+T(52)+T(10)+T(38)+T(45)+T(49)+NT(195)));
//G324: bf_normalize_cb(217) => __E_bf_cb_94(218).
	p(NT(217), (NT(218)));
//G325: bf_cb(47)            => bf_normalize_cb(217).
	p(NT(47), (NT(217)));
//G326: cli(219)             => _(11).
	p(NT(219), (NT(11)));
//G327: __E_cli_95(221)      => '.' _(11) cli_command(220) _(11).
	p(NT(221), (T(1)+NT(11)+NT(220)+NT(11)));
//G328: __E_cli_96(222)      => null.
	p(NT(222), (nul));
//G329: __E_cli_96(222)      => __E_cli_95(221) __E_cli_96(222).
	p(NT(222), (NT(221)+NT(222)));
//G330: cli(219)             => _(11) cli_command(220) _(11) __E_cli_96(222).
	p(NT(219), (NT(11)+NT(220)+NT(11)+NT(222)));
//G331: __E_cli_command_97(225) => 'q'.
	p(NT(225), (T(53)));
//G332: __E_cli_command_97(225) => 'q' 'u' 'i' 't'.
	p(NT(225), (T(53)+T(48)+T(12)+T(11)));
//G333: quit_sym(224)        => __E_cli_command_97(225).
	p(NT(224), (NT(225)));
//G334: quit_cmd(223)        => quit_sym(224).
	p(NT(223), (NT(224)));
//G335: cli_command(220)     => quit_cmd(223).
	p(NT(220), (NT(223)));
//G336: __E_cli_command_98(228) => 'v'.
	p(NT(228), (T(51)));
//G337: __E_cli_command_98(228) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(228), (T(51)+T(10)+T(50)+T(7)+T(12)+T(8)+T(46)));
//G338: version_sym(227)     => __E_cli_command_98(228).
	p(NT(227), (NT(228)));
//G339: version_cmd(226)     => version_sym(227).
	p(NT(226), (NT(227)));
//G340: cli_command(220)     => version_cmd(226).
	p(NT(220), (NT(226)));
//G341: __E_cli_command_99(231) => 'c'.
	p(NT(231), (T(45)));
//G342: __E_cli_command_99(231) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(231), (T(45)+T(16)+T(10)+T(15)+T(50)));
//G343: clear_sym(230)       => __E_cli_command_99(231).
	p(NT(230), (NT(231)));
//G344: clear_cmd(229)       => clear_sym(230).
	p(NT(229), (NT(230)));
//G345: cli_command(220)     => clear_cmd(229).
	p(NT(220), (NT(229)));
//G346: __E___E_cli_command_100_101(235) => 'h'.
	p(NT(235), (T(44)));
//G347: __E___E_cli_command_100_101(235) => 'h' 'e' 'l' 'p'.
	p(NT(235), (T(44)+T(10)+T(16)+T(54)));
//G348: help_sym(234)        => __E___E_cli_command_100_101(235).
	p(NT(234), (NT(235)));
//G349: __E___E_cli_command_100_102(236) => __(52) help_arg(237).
	p(NT(236), (NT(52)+NT(237)));
//G350: __E___E_cli_command_100_102(236) => null.
	p(NT(236), (nul));
//G351: __E_cli_command_100(233) => help_sym(234) __E___E_cli_command_100_102(236).
	p(NT(233), (NT(234)+NT(236)));
//G352: help_cmd(232)        => __E_cli_command_100(233).
	p(NT(232), (NT(233)));
//G353: cli_command(220)     => help_cmd(232).
	p(NT(220), (NT(232)));
//G354: file_sym(240)        => 'f' 'i' 'l' 'e'.
	p(NT(240), (T(43)+T(12)+T(16)+T(10)));
//G355: __E_cli_command_103(239) => file_sym(240) __(52) q_string(241).
	p(NT(239), (NT(240)+NT(52)+NT(241)));
//G356: file_cmd(238)        => __E_cli_command_103(239).
	p(NT(238), (NT(239)));
//G357: cli_command(220)     => file_cmd(238).
	p(NT(220), (NT(238)));
//G358: valid_sym(244)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(244), (T(51)+T(15)+T(16)+T(12)+T(55)));
//G359: __E_cli_command_104(243) => valid_sym(244) __(52) rr(10).
	p(NT(243), (NT(244)+NT(52)+NT(10)));
//G360: valid_cmd(242)       => __E_cli_command_104(243).
	p(NT(242), (NT(243)));
//G361: cli_command(220)     => valid_cmd(242).
	p(NT(220), (NT(242)));
//G362: sat_sym(247)         => 's' 'a' 't'.
	p(NT(247), (T(7)+T(15)+T(11)));
//G363: __E_cli_command_105(246) => sat_sym(247) __(52) normalize_cmd_arg(248).
	p(NT(246), (NT(247)+NT(52)+NT(248)));
//G364: sat_cmd(245)         => __E_cli_command_105(246).
	p(NT(245), (NT(246)));
//G365: cli_command(220)     => sat_cmd(245).
	p(NT(220), (NT(245)));
//G366: unsat_sym(251)       => 'u' 'n' 's' 'a' 't'.
	p(NT(251), (T(48)+T(46)+T(7)+T(15)+T(11)));
//G367: __E_cli_command_106(250) => unsat_sym(251) __(52) rr(10).
	p(NT(250), (NT(251)+NT(52)+NT(10)));
//G368: unsat_cmd(249)       => __E_cli_command_106(250).
	p(NT(249), (NT(250)));
//G369: cli_command(220)     => unsat_cmd(249).
	p(NT(220), (NT(249)));
//G370: solve_sym(254)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(254), (T(7)+T(8)+T(16)+T(51)+T(10)));
//G371: __E___E_cli_command_107_108(255) => __(52) type(176).
	p(NT(255), (NT(52)+NT(176)));
//G372: __E___E_cli_command_107_108(255) => null.
	p(NT(255), (nul));
//G373: __E_cli_command_107(253) => solve_sym(254) __E___E_cli_command_107_108(255) __(52) solve_cmd_arg(256).
	p(NT(253), (NT(254)+NT(255)+NT(52)+NT(256)));
//G374: solve_cmd(252)       => __E_cli_command_107(253).
	p(NT(252), (NT(253)));
//G375: cli_command(220)     => solve_cmd(252).
	p(NT(220), (NT(252)));
//G376: __E___E_cli_command_109_110(260) => 'r'.
	p(NT(260), (T(50)));
//G377: __E___E_cli_command_109_110(260) => 'r' 'u' 'n'.
	p(NT(260), (T(50)+T(48)+T(46)));
//G378: run_sym(259)         => __E___E_cli_command_109_110(260).
	p(NT(259), (NT(260)));
//G379: __E_cli_command_109(258) => run_sym(259) __(52) wff(14).
	p(NT(258), (NT(259)+NT(52)+NT(14)));
//G380: run_cmd(257)         => __E_cli_command_109(258).
	p(NT(257), (NT(258)));
//G381: cli_command(220)     => run_cmd(257).
	p(NT(220), (NT(257)));
//G382: __E___E_cli_command_111_112(264) => 'n'.
	p(NT(264), (T(46)));
//G383: __E___E_cli_command_111_112(264) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(264), (T(46)+T(8)+T(50)+T(9)+T(15)+T(16)+T(12)+T(52)+T(10)));
//G384: normalize_sym(263)   => __E___E_cli_command_111_112(264).
	p(NT(263), (NT(264)));
//G385: __E_cli_command_111(262) => normalize_sym(263) __(52) normalize_cmd_arg(248).
	p(NT(262), (NT(263)+NT(52)+NT(248)));
//G386: normalize_cmd(261)   => __E_cli_command_111(262).
	p(NT(261), (NT(262)));
//G387: cli_command(220)     => normalize_cmd(261).
	p(NT(220), (NT(261)));
//G388: __E___E_cli_command_113_114(268) => 's'.
	p(NT(268), (T(7)));
//G389: __E___E_cli_command_113_114(268) => 's' 'u' 'b' 's' 't'.
	p(NT(268), (T(7)+T(48)+T(49)+T(7)+T(11)));
//G390: __E___E_cli_command_113_114(268) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(268), (T(7)+T(48)+T(49)+T(7)+T(11)+T(12)+T(11)+T(48)+T(11)+T(10)));
//G391: subst_sym(267)       => __E___E_cli_command_113_114(268).
	p(NT(267), (NT(268)));
//G392: __E_cli_command_113(266) => subst_sym(267) __(52) nf_cmd_arg(269) _(11) '[' _(11) nf_cmd_arg(269) _(11) '/' _(11) nf_cmd_arg(269) _(11) ']'.
	p(NT(266), (NT(267)+NT(52)+NT(269)+NT(11)+T(19)+NT(11)+NT(269)+NT(11)+T(56)+NT(11)+NT(269)+NT(11)+T(20)));
//G393: subst_cmd(265)       => __E_cli_command_113(266).
	p(NT(265), (NT(266)));
//G394: cli_command(220)     => subst_cmd(265).
	p(NT(220), (NT(265)));
//G395: __E___E_cli_command_115_116(273) => 'i'.
	p(NT(273), (T(12)));
//G396: __E___E_cli_command_115_116(273) => 'i' 'n' 's' 't'.
	p(NT(273), (T(12)+T(46)+T(7)+T(11)));
//G397: __E___E_cli_command_115_116(273) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(273), (T(12)+T(46)+T(7)+T(11)+T(15)+T(46)+T(11)+T(12)+T(15)+T(11)+T(10)));
//G398: inst_sym(272)        => __E___E_cli_command_115_116(273).
	p(NT(272), (NT(273)));
//G399: __E_cli_command_115(271) => inst_sym(272) __(52) inst_args(274).
	p(NT(271), (NT(272)+NT(52)+NT(274)));
//G400: inst_cmd(270)        => __E_cli_command_115(271).
	p(NT(270), (NT(271)));
//G401: cli_command(220)     => inst_cmd(270).
	p(NT(220), (NT(270)));
//G402: dnf_sym(277)         => 'd' 'n' 'f'.
	p(NT(277), (T(55)+T(46)+T(43)));
//G403: __E_cli_command_117(276) => dnf_sym(277) __(52) nf_cmd_arg(269).
	p(NT(276), (NT(277)+NT(52)+NT(269)));
//G404: dnf_cmd(275)         => __E_cli_command_117(276).
	p(NT(275), (NT(276)));
//G405: cli_command(220)     => dnf_cmd(275).
	p(NT(220), (NT(275)));
//G406: cnf_sym(280)         => 'c' 'n' 'f'.
	p(NT(280), (T(45)+T(46)+T(43)));
//G407: __E_cli_command_118(279) => cnf_sym(280) __(52) nf_cmd_arg(269).
	p(NT(279), (NT(280)+NT(52)+NT(269)));
//G408: cnf_cmd(278)         => __E_cli_command_118(279).
	p(NT(278), (NT(279)));
//G409: cli_command(220)     => cnf_cmd(278).
	p(NT(220), (NT(278)));
//G410: anf_sym(283)         => 'a' 'n' 'f'.
	p(NT(283), (T(15)+T(46)+T(43)));
//G411: __E_cli_command_119(282) => anf_sym(283) __(52) nf_cmd_arg(269).
	p(NT(282), (NT(283)+NT(52)+NT(269)));
//G412: anf_cmd(281)         => __E_cli_command_119(282).
	p(NT(281), (NT(282)));
//G413: cli_command(220)     => anf_cmd(281).
	p(NT(220), (NT(281)));
//G414: nnf_sym(286)         => 'n' 'n' 'f'.
	p(NT(286), (T(46)+T(46)+T(43)));
//G415: __E_cli_command_120(285) => nnf_sym(286) __(52) nf_cmd_arg(269).
	p(NT(285), (NT(286)+NT(52)+NT(269)));
//G416: nnf_cmd(284)         => __E_cli_command_120(285).
	p(NT(284), (NT(285)));
//G417: cli_command(220)     => nnf_cmd(284).
	p(NT(220), (NT(284)));
//G418: pnf_sym(289)         => 'p' 'n' 'f'.
	p(NT(289), (T(54)+T(46)+T(43)));
//G419: __E_cli_command_121(288) => pnf_sym(289) __(52) nf_cmd_arg(269).
	p(NT(288), (NT(289)+NT(52)+NT(269)));
//G420: pnf_cmd(287)         => __E_cli_command_121(288).
	p(NT(287), (NT(288)));
//G421: cli_command(220)     => pnf_cmd(287).
	p(NT(220), (NT(287)));
//G422: mnf_sym(292)         => 'm' 'n' 'f'.
	p(NT(292), (T(9)+T(46)+T(43)));
//G423: __E_cli_command_122(291) => mnf_sym(292) __(52) nf_cmd_arg(269).
	p(NT(291), (NT(292)+NT(52)+NT(269)));
//G424: mnf_cmd(290)         => __E_cli_command_122(291).
	p(NT(290), (NT(291)));
//G425: cli_command(220)     => mnf_cmd(290).
	p(NT(220), (NT(290)));
//G426: snf_sym(295)         => 's' 'n' 'f'.
	p(NT(295), (T(7)+T(46)+T(43)));
//G427: __E_cli_command_123(294) => snf_sym(295) __(52) nf_cmd_arg(269).
	p(NT(294), (NT(295)+NT(52)+NT(269)));
//G428: snf_cmd(293)         => __E_cli_command_123(294).
	p(NT(293), (NT(294)));
//G429: cli_command(220)     => snf_cmd(293).
	p(NT(220), (NT(293)));
//G430: onf_sym(298)         => 'o' 'n' 'f'.
	p(NT(298), (T(8)+T(46)+T(43)));
//G431: __E_cli_command_124(297) => onf_sym(298) __(52) variable(114) __(52) onf_cmd_arg(299).
	p(NT(297), (NT(298)+NT(52)+NT(114)+NT(52)+NT(299)));
//G432: onf_cmd(296)         => __E_cli_command_124(297).
	p(NT(296), (NT(297)));
//G433: cli_command(220)     => onf_cmd(296).
	p(NT(220), (NT(296)));
//G434: __E___E_cli_command_125_126(303) => 'd' 'e' 'f' 's'.
	p(NT(303), (T(55)+T(10)+T(43)+T(7)));
//G435: __E___E_cli_command_125_126(303) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(303), (T(55)+T(10)+T(43)+T(12)+T(46)+T(12)+T(11)+T(12)+T(8)+T(46)+T(7)));
//G436: def_sym(302)         => __E___E_cli_command_125_126(303).
	p(NT(302), (NT(303)));
//G437: __E_cli_command_125(301) => def_sym(302).
	p(NT(301), (NT(302)));
//G438: def_list_cmd(300)    => __E_cli_command_125(301).
	p(NT(300), (NT(301)));
//G439: cli_command(220)     => def_list_cmd(300).
	p(NT(220), (NT(300)));
//G440: __E_cli_command_127(305) => def_sym(302) __(52) number(306).
	p(NT(305), (NT(302)+NT(52)+NT(306)));
//G441: def_print_cmd(304)   => __E_cli_command_127(305).
	p(NT(304), (NT(305)));
//G442: cli_command(220)     => def_print_cmd(304).
	p(NT(220), (NT(304)));
//G443: def_rr_cmd(307)      => rec_relation(16).
	p(NT(307), (NT(16)));
//G444: cli_command(220)     => def_rr_cmd(307).
	p(NT(220), (NT(307)));
//G445: qelim_sym(310)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(310), (T(53)+T(10)+T(16)+T(12)+T(9)));
//G446: __E_cli_command_128(309) => qelim_sym(310) __(52) wff_cmd_arg(311).
	p(NT(309), (NT(310)+NT(52)+NT(311)));
//G447: qelim_cmd(308)       => __E_cli_command_128(309).
	p(NT(308), (NT(309)));
//G448: cli_command(220)     => qelim_cmd(308).
	p(NT(220), (NT(308)));
//G449: get_sym(314)         => 'g' 'e' 't'.
	p(NT(314), (T(47)+T(10)+T(11)));
//G450: __E___E_cli_command_129_130(315) => __(52) option(316).
	p(NT(315), (NT(52)+NT(316)));
//G451: __E___E_cli_command_129_130(315) => null.
	p(NT(315), (nul));
//G452: __E_cli_command_129(313) => get_sym(314) __E___E_cli_command_129_130(315).
	p(NT(313), (NT(314)+NT(315)));
//G453: get_cmd(312)         => __E_cli_command_129(313).
	p(NT(312), (NT(313)));
//G454: cli_command(220)     => get_cmd(312).
	p(NT(220), (NT(312)));
//G455: set_sym(319)         => 's' 'e' 't'.
	p(NT(319), (T(7)+T(10)+T(11)));
//G456: __E___E_cli_command_131_132(320) => __(52).
	p(NT(320), (NT(52)));
//G457: __E___E_cli_command_131_132(320) => _(11) '=' _(11).
	p(NT(320), (NT(11)+T(3)+NT(11)));
//G458: __E_cli_command_131(318) => set_sym(319) __(52) option(316) __E___E_cli_command_131_132(320) option_value(321).
	p(NT(318), (NT(319)+NT(52)+NT(316)+NT(320)+NT(321)));
//G459: set_cmd(317)         => __E_cli_command_131(318).
	p(NT(317), (NT(318)));
//G460: cli_command(220)     => set_cmd(317).
	p(NT(220), (NT(317)));
//G461: enable_sym(324)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(324), (T(10)+T(46)+T(15)+T(49)+T(16)+T(10)));
//G462: __E_cli_command_133(323) => enable_sym(324) __(52) bool_option(325).
	p(NT(323), (NT(324)+NT(52)+NT(325)));
//G463: enable_cmd(322)      => __E_cli_command_133(323).
	p(NT(322), (NT(323)));
//G464: cli_command(220)     => enable_cmd(322).
	p(NT(220), (NT(322)));
//G465: disable_sym(328)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(328), (T(55)+T(12)+T(7)+T(15)+T(49)+T(16)+T(10)));
//G466: __E_cli_command_134(327) => disable_sym(328) __(52) bool_option(325).
	p(NT(327), (NT(328)+NT(52)+NT(325)));
//G467: disable_cmd(326)     => __E_cli_command_134(327).
	p(NT(326), (NT(327)));
//G468: cli_command(220)     => disable_cmd(326).
	p(NT(220), (NT(326)));
//G469: toggle_sym(331)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(331), (T(11)+T(8)+T(47)+T(47)+T(16)+T(10)));
//G470: __E_cli_command_135(330) => toggle_sym(331) __(52) bool_option(325).
	p(NT(330), (NT(331)+NT(52)+NT(325)));
//G471: toggle_cmd(329)      => __E_cli_command_135(330).
	p(NT(329), (NT(330)));
//G472: cli_command(220)     => toggle_cmd(329).
	p(NT(220), (NT(329)));
//G473: __E_cli_command_136(333) => type(176) __(52) in_var_name(152) _(11) '=' _(11) input_stream(334).
	p(NT(333), (NT(176)+NT(52)+NT(152)+NT(11)+T(3)+NT(11)+NT(334)));
//G474: def_input_cmd(332)   => __E_cli_command_136(333).
	p(NT(332), (NT(333)));
//G475: cli_command(220)     => def_input_cmd(332).
	p(NT(220), (NT(332)));
//G476: __E_cli_command_137(336) => type(176) __(52) out_var_name(156) _(11) '=' _(11) output_stream(337).
	p(NT(336), (NT(176)+NT(52)+NT(156)+NT(11)+T(3)+NT(11)+NT(337)));
//G477: def_output_cmd(335)  => __E_cli_command_137(336).
	p(NT(335), (NT(336)));
//G478: cli_command(220)     => def_output_cmd(335).
	p(NT(220), (NT(335)));
//G479: __E___E_cli_command_138_139(341) => 'h' 'i' 's' 't'.
	p(NT(341), (T(44)+T(12)+T(7)+T(11)));
//G480: __E___E_cli_command_138_139(341) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(341), (T(44)+T(12)+T(7)+T(11)+T(8)+T(50)+T(18)));
//G481: history_sym(340)     => __E___E_cli_command_138_139(341).
	p(NT(340), (NT(341)));
//G482: __E_cli_command_138(339) => history_sym(340).
	p(NT(339), (NT(340)));
//G483: history_list_cmd(338) => __E_cli_command_138(339).
	p(NT(338), (NT(339)));
//G484: cli_command(220)     => history_list_cmd(338).
	p(NT(220), (NT(338)));
//G485: __E_cli_command_140(343) => history_sym(340) __(52) memory(344).
	p(NT(343), (NT(340)+NT(52)+NT(344)));
//G486: history_print_cmd(342) => __E_cli_command_140(343).
	p(NT(342), (NT(343)));
//G487: cli_command(220)     => history_print_cmd(342).
	p(NT(220), (NT(342)));
//G488: __E_cli_command_141(346) => wff(14).
	p(NT(346), (NT(14)));
//G489: __E_cli_command_141(346) => bf(21).
	p(NT(346), (NT(21)));
//G490: history_store_cmd(345) => __E_cli_command_141(346).
	p(NT(345), (NT(346)));
//G491: cli_command(220)     => history_store_cmd(345).
	p(NT(220), (NT(345)));
//G492: __E_input_stream_142(347) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(348) _(11) ')'.
	p(NT(347), (T(12)+T(43)+T(12)+T(16)+T(10)+NT(11)+T(5)+NT(11)+NT(348)+NT(11)+T(6)));
//G493: input_stream(334)    => __E_input_stream_142(347).
	p(NT(334), (NT(347)));
//G494: console_sym(349)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(349), (T(45)+T(8)+T(46)+T(7)+T(8)+T(16)+T(10)));
//G495: input_stream(334)    => console_sym(349).
	p(NT(334), (NT(349)));
//G496: __E_output_stream_143(350) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(348) _(11) ')'.
	p(NT(350), (T(8)+T(43)+T(12)+T(16)+T(10)+NT(11)+T(5)+NT(11)+NT(348)+NT(11)+T(6)));
//G497: output_stream(337)   => __E_output_stream_143(350).
	p(NT(337), (NT(350)));
//G498: output_stream(337)   => console_sym(349).
	p(NT(337), (NT(349)));
//G499: q_file_name(348)     => '"' file_name(351) '"'.
	p(NT(348), (T(57)+NT(351)+T(57)));
//G500: __E_file_name_144(352) => printable(8).
	p(NT(352), (NT(8)));
//G501: __E_file_name_144(352) => printable(8) __E_file_name_144(352).
	p(NT(352), (NT(8)+NT(352)));
//G502: file_name(351)       => __E_file_name_144(352).
	p(NT(351), (NT(352)));
//G503: number(306)          => digits(168).
	p(NT(306), (NT(168)));
//G504: bf_cmd_arg(353)      => memory(344).
	p(NT(353), (NT(344)));
//G505: bf_cmd_arg(353)      => bf(21).
	p(NT(353), (NT(21)));
//G506: wff_cmd_arg(311)     => memory(344).
	p(NT(311), (NT(344)));
//G507: wff_cmd_arg(311)     => wff(14).
	p(NT(311), (NT(14)));
//G508: solve_cmd_arg(256)   => memory(344).
	p(NT(256), (NT(344)));
//G509: solve_cmd_arg(256)   => wff(14).
	p(NT(256), (NT(14)));
//G510: nf_cmd_arg(269)      => memory(344).
	p(NT(269), (NT(344)));
//G511: nf_cmd_arg(269)      => wff(14).
	p(NT(269), (NT(14)));
//G512: nf_cmd_arg(269)      => bf(21).
	p(NT(269), (NT(21)));
//G513: onf_cmd_arg(299)     => memory(344).
	p(NT(299), (NT(344)));
//G514: onf_cmd_arg(299)     => wff(14).
	p(NT(299), (NT(14)));
//G515: normalize_cmd_arg(248) => memory(344).
	p(NT(248), (NT(344)));
//G516: normalize_cmd_arg(248) => rr(10).
	p(NT(248), (NT(10)));
//G517: normalize_cmd_arg(248) => ref(18).
	p(NT(248), (NT(18)));
//G518: normalize_cmd_arg(248) => wff(14).
	p(NT(248), (NT(14)));
//G519: normalize_cmd_arg(248) => bf(21).
	p(NT(248), (NT(21)));
//G520: inst_args(274)       => wff_cmd_arg(311) _(11) '[' _(11) variable(114) _(11) '/' _(11) bf_cmd_arg(353) _(11) ']'.
	p(NT(274), (NT(311)+NT(11)+T(19)+NT(11)+NT(114)+NT(11)+T(56)+NT(11)+NT(353)+NT(11)+T(20)));
//G521: inst_args(274)       => bf_cmd_arg(353) _(11) '[' _(11) variable(114) _(11) '/' _(11) bf_cmd_arg(353) _(11) ']'.
	p(NT(274), (NT(353)+NT(11)+T(19)+NT(11)+NT(114)+NT(11)+T(56)+NT(11)+NT(353)+NT(11)+T(20)));
//G522: help_arg(237)        => help_sym(234).
	p(NT(237), (NT(234)));
//G523: help_arg(237)        => version_sym(227).
	p(NT(237), (NT(227)));
//G524: help_arg(237)        => quit_sym(224).
	p(NT(237), (NT(224)));
//G525: help_arg(237)        => clear_sym(230).
	p(NT(237), (NT(230)));
//G526: help_arg(237)        => get_sym(314).
	p(NT(237), (NT(314)));
//G527: help_arg(237)        => set_sym(319).
	p(NT(237), (NT(319)));
//G528: help_arg(237)        => enable_sym(324).
	p(NT(237), (NT(324)));
//G529: help_arg(237)        => disable_sym(328).
	p(NT(237), (NT(328)));
//G530: help_arg(237)        => toggle_sym(331).
	p(NT(237), (NT(331)));
//G531: help_arg(237)        => file_sym(240).
	p(NT(237), (NT(240)));
//G532: help_arg(237)        => history_sym(340).
	p(NT(237), (NT(340)));
//G533: help_arg(237)        => abs_memory_sym(354).
	p(NT(237), (NT(354)));
//G534: help_arg(237)        => rel_memory_sym(355).
	p(NT(237), (NT(355)));
//G535: help_arg(237)        => selection_sym(356).
	p(NT(237), (NT(356)));
//G536: help_arg(237)        => def_sym(302).
	p(NT(237), (NT(302)));
//G537: help_arg(237)        => inst_sym(272).
	p(NT(237), (NT(272)));
//G538: help_arg(237)        => subst_sym(267).
	p(NT(237), (NT(267)));
//G539: help_arg(237)        => normalize_sym(263).
	p(NT(237), (NT(263)));
//G540: help_arg(237)        => execute_sym(357).
	p(NT(237), (NT(357)));
//G541: help_arg(237)        => solve_sym(254).
	p(NT(237), (NT(254)));
//G542: help_arg(237)        => valid_sym(244).
	p(NT(237), (NT(244)));
//G543: help_arg(237)        => sat_sym(247).
	p(NT(237), (NT(247)));
//G544: help_arg(237)        => unsat_sym(251).
	p(NT(237), (NT(251)));
//G545: help_arg(237)        => dnf_sym(277).
	p(NT(237), (NT(277)));
//G546: help_arg(237)        => cnf_sym(280).
	p(NT(237), (NT(280)));
//G547: help_arg(237)        => anf_sym(283).
	p(NT(237), (NT(283)));
//G548: help_arg(237)        => snf_sym(295).
	p(NT(237), (NT(295)));
//G549: help_arg(237)        => nnf_sym(286).
	p(NT(237), (NT(286)));
//G550: help_arg(237)        => pnf_sym(289).
	p(NT(237), (NT(289)));
//G551: help_arg(237)        => mnf_sym(292).
	p(NT(237), (NT(292)));
//G552: help_arg(237)        => onf_sym(298).
	p(NT(237), (NT(298)));
//G553: help_arg(237)        => qelim_sym(310).
	p(NT(237), (NT(310)));
//G554: __E___E_help_arg_145_146(360) => 's'.
	p(NT(360), (T(7)));
//G555: __E___E_help_arg_145_146(360) => null.
	p(NT(360), (nul));
//G556: __E_help_arg_145(359) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_145_146(360).
	p(NT(359), (T(10)+T(22)+T(15)+T(9)+T(54)+T(16)+T(10)+NT(360)));
//G557: examples_sym(358)    => __E_help_arg_145(359).
	p(NT(358), (NT(359)));
//G558: help_arg(237)        => examples_sym(358).
	p(NT(237), (NT(358)));
//G559: __E_memory_147(362)  => '%'.
	p(NT(362), (T(58)));
//G560: rel_memory_sym(355)  => '%' '-'.
	p(NT(355), (T(58)+T(23)));
//G561: memory_id(364)       => digits(168).
	p(NT(364), (NT(168)));
//G562: __E___E_memory_147_148(363) => memory_id(364).
	p(NT(363), (NT(364)));
//G563: __E___E_memory_147_148(363) => null.
	p(NT(363), (nul));
//G564: __E_memory_147(362)  => rel_memory_sym(355) __E___E_memory_147_148(363).
	p(NT(362), (NT(355)+NT(363)));
//G565: rel_memory(361)      => __E_memory_147(362).
	p(NT(361), (NT(362)));
//G566: memory(344)          => rel_memory(361).
	p(NT(344), (NT(361)));
//G567: abs_memory_sym(354)  => '%'.
	p(NT(354), (T(58)));
//G568: __E_memory_149(366)  => abs_memory_sym(354) memory_id(364).
	p(NT(366), (NT(354)+NT(364)));
//G569: abs_memory(365)      => __E_memory_149(366).
	p(NT(365), (NT(366)));
//G570: memory(344)          => abs_memory(365).
	p(NT(344), (NT(365)));
//G571: option(316)          => bool_option(325).
	p(NT(316), (NT(325)));
//G572: __E_option_150(369)  => 's' 'e' 'v'.
	p(NT(369), (T(7)+T(10)+T(51)));
//G573: __E_option_150(369)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(369), (T(7)+T(10)+T(51)+T(10)+T(50)+T(12)+T(11)+T(18)));
//G574: severity_opt(368)    => __E_option_150(369).
	p(NT(368), (NT(369)));
//G575: enum_option(367)     => severity_opt(368).
	p(NT(367), (NT(368)));
//G576: option(316)          => enum_option(367).
	p(NT(316), (NT(367)));
//G577: __E_bool_option_151(371) => 's'.
	p(NT(371), (T(7)));
//G578: __E_bool_option_151(371) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(371), (T(7)+T(11)+T(15)+T(11)+T(48)+T(7)));
//G579: status_opt(370)      => __E_bool_option_151(371).
	p(NT(370), (NT(371)));
//G580: bool_option(325)     => status_opt(370).
	p(NT(325), (NT(370)));
//G581: __E_bool_option_152(373) => 'c'.
	p(NT(373), (T(45)));
//G582: __E_bool_option_152(373) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(373), (T(45)+T(8)+T(16)+T(8)+T(50)));
//G583: __E_bool_option_152(373) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(373), (T(45)+T(8)+T(16)+T(8)+T(50)+T(7)));
//G584: colors_opt(372)      => __E_bool_option_152(373).
	p(NT(372), (NT(373)));
//G585: bool_option(325)     => colors_opt(372).
	p(NT(325), (NT(372)));
//G586: __E_bool_option_153(375) => 'v'.
	p(NT(375), (T(51)));
//G587: __E_bool_option_153(375) => 'c' 'h' 'a' 'r' 'v' 'a' 'r'.
	p(NT(375), (T(45)+T(44)+T(15)+T(50)+T(51)+T(15)+T(50)));
//G588: charvar_opt(374)     => __E_bool_option_153(375).
	p(NT(374), (NT(375)));
//G589: bool_option(325)     => charvar_opt(374).
	p(NT(325), (NT(374)));
//G590: __E_bool_option_154(377) => 'h'.
	p(NT(377), (T(44)));
//G591: __E_bool_option_154(377) => 'h' 'i' 'l' 'i' 'g' 'h' 't'.
	p(NT(377), (T(44)+T(12)+T(16)+T(12)+T(47)+T(44)+T(11)));
//G592: __E_bool_option_154(377) => 'h' 'i' 'l' 'i' 'g' 'h' 't' 'i' 'n' 'g'.
	p(NT(377), (T(44)+T(12)+T(16)+T(12)+T(47)+T(44)+T(11)+T(12)+T(46)+T(47)));
//G593: hilighting_opt(376)  => __E_bool_option_154(377).
	p(NT(376), (NT(377)));
//G594: bool_option(325)     => hilighting_opt(376).
	p(NT(325), (NT(376)));
//G595: __E_bool_option_155(379) => 'i'.
	p(NT(379), (T(12)));
//G596: __E_bool_option_155(379) => 'i' 'n' 'd' 'e' 'n' 't'.
	p(NT(379), (T(12)+T(46)+T(55)+T(10)+T(46)+T(11)));
//G597: __E_bool_option_155(379) => 'i' 'n' 'd' 'e' 'n' 't' 'i' 'n' 'g'.
	p(NT(379), (T(12)+T(46)+T(55)+T(10)+T(46)+T(11)+T(12)+T(46)+T(47)));
//G598: indenting_opt(378)   => __E_bool_option_155(379).
	p(NT(378), (NT(379)));
//G599: bool_option(325)     => indenting_opt(378).
	p(NT(325), (NT(378)));
//G600: __E_bool_option_156(381) => 'd'.
	p(NT(381), (T(55)));
//G601: __E_bool_option_156(381) => 'd' 'b' 'g'.
	p(NT(381), (T(55)+T(49)+T(47)));
//G602: __E_bool_option_156(381) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(381), (T(55)+T(10)+T(49)+T(48)+T(47)));
//G603: debug_repl_opt(380)  => __E_bool_option_156(381).
	p(NT(380), (NT(381)));
//G604: bool_option(325)     => debug_repl_opt(380).
	p(NT(325), (NT(380)));
//G605: option_value(321)    => option_value_true(382).
	p(NT(321), (NT(382)));
//G606: option_value(321)    => option_value_false(383).
	p(NT(321), (NT(383)));
//G607: option_value(321)    => severity(384).
	p(NT(321), (NT(384)));
//G608: option_value_true(382) => 't'.
	p(NT(382), (T(11)));
//G609: option_value_true(382) => 't' 'r' 'u' 'e'.
	p(NT(382), (T(11)+T(50)+T(48)+T(10)));
//G610: option_value_true(382) => 'o' 'n'.
	p(NT(382), (T(8)+T(46)));
//G611: option_value_true(382) => '1'.
	p(NT(382), (T(33)));
//G612: option_value_true(382) => 'y'.
	p(NT(382), (T(18)));
//G613: option_value_true(382) => 'y' 'e' 's'.
	p(NT(382), (T(18)+T(10)+T(7)));
//G614: option_value_false(383) => 'f'.
	p(NT(383), (T(43)));
//G615: option_value_false(383) => 'f' 'a' 'l' 's' 'e'.
	p(NT(383), (T(43)+T(15)+T(16)+T(7)+T(10)));
//G616: option_value_false(383) => 'o' 'f' 'f'.
	p(NT(383), (T(8)+T(43)+T(43)));
//G617: option_value_false(383) => '0'.
	p(NT(383), (T(34)));
//G618: option_value_false(383) => 'n'.
	p(NT(383), (T(46)));
//G619: option_value_false(383) => 'n' 'o'.
	p(NT(383), (T(46)+T(8)));
//G620: __E_severity_157(386) => 'e'.
	p(NT(386), (T(10)));
//G621: __E_severity_157(386) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(386), (T(10)+T(50)+T(50)+T(8)+T(50)));
//G622: error_sym(385)       => __E_severity_157(386).
	p(NT(385), (NT(386)));
//G623: severity(384)        => error_sym(385).
	p(NT(384), (NT(385)));
//G624: __E_severity_158(388) => 'i'.
	p(NT(388), (T(12)));
//G625: __E_severity_158(388) => 'i' 'n' 'f' 'o'.
	p(NT(388), (T(12)+T(46)+T(43)+T(8)));
//G626: info_sym(387)        => __E_severity_158(388).
	p(NT(387), (NT(388)));
//G627: severity(384)        => info_sym(387).
	p(NT(384), (NT(387)));
//G628: __E_severity_159(390) => 'd'.
	p(NT(390), (T(55)));
//G629: __E_severity_159(390) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(390), (T(55)+T(10)+T(49)+T(48)+T(47)));
//G630: debug_sym(389)       => __E_severity_159(390).
	p(NT(389), (NT(390)));
//G631: severity(384)        => debug_sym(389).
	p(NT(384), (NT(389)));
//G632: __E_severity_160(392) => 't'.
	p(NT(392), (T(11)));
//G633: __E_severity_160(392) => 't' 'r' 'a' 'c' 'e'.
	p(NT(392), (T(11)+T(50)+T(15)+T(45)+T(10)));
//G634: trace_sym(391)       => __E_severity_160(392).
	p(NT(391), (NT(392)));
//G635: severity(384)        => trace_sym(391).
	p(NT(384), (NT(391)));
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
		__E_wff_32, bf_eq, __E_wff_33, bf_neq, __E_wff_34, bf_less_equal, __E_wff_35, bf_nleq, __E_wff_36, bf_greater, 
		__E_wff_37, bf_ngreater, __E_wff_38, bf_greater_equal, __E_wff_39, bf_ngeq, __E_wff_40, bf_less, __E_wff_41, bf_nless, 
		__E_wff_42, bf_parenthesis, __E_bf_43, bf_constant, variable, bf_splitter, __E_bf_44, bf_ref, bf_or, __E_bf_45, 
		bf_xor, __E_bf_46, bf_and, __E_bf_47, __E___E_bf_47_48, bf_neg, __E_bf_49, bf_t, bf_f, uninterpreted_constant, 
		bf_and_nosep_1st_op, __E_bf_and_50, ctn_neq, __E_constraint_51, ctnvar, num, ctn_eq, __E_constraint_52, ctn_greater_equal, __E_constraint_53, 
		ctn_greater, __E_constraint_54, ctn_less_equal, __E_constraint_55, ctn_less, __E_constraint_56, __E_variable_57, chars, io_var, __E_variable_58, 
		in, out, in_var_name, __E_in_59, __E___E_in_59_60, offset, out_var_name, __E_out_61, __E___E_out_61_62, q_var, 
		__E_q_vars_63, __E_q_vars_64, __E_offsets_65, __E_offsets_66, shift, __E_offset_67, __E_shift_68, __E___E_shift_68_69, digits, uninter_const_name, 
		__E_uninterpreted_constant_70, __E___E_uninterpreted_constant_70_71, constant, __E_bf_constant_72, binding, __E_bf_constant_73, type, source, __E_binding_74, src_c, 
		__E___E_binding_74_75, __E___E___E_binding_74_75_76, __E___E___E_binding_74_75_77, __E_src_c_78, __E_src_c_79, __E_chars_80, __E_chars_81, __E_digits_82, extra, comment, 
		__E_comment_83, __E_comment_84, __E_comment_85, __E____86, __E___87, bf_cb_args1, bf_cb_arg, bf_cb_args2, bf_cb_args3, bf_cb_args4, 
		wff_cb_args1, wff_cb_arg, wff_cb_args2, wff_cb_args3, wff_cb_args4, wff_has_clashing_subformulas_cb, __E_wff_cb_88, wff_has_subformula_cb, __E_wff_cb_89, wff_remove_existential_cb, 
		__E_wff_cb_90, bf_has_subformula_cb, __E_bf_cb_91, bf_remove_funiversal_cb, __E_bf_cb_92, bf_remove_fexistential_cb, __E_bf_cb_93, bf_normalize_cb, __E_bf_cb_94, cli, 
		cli_command, __E_cli_95, __E_cli_96, quit_cmd, quit_sym, __E_cli_command_97, version_cmd, version_sym, __E_cli_command_98, clear_cmd, 
		clear_sym, __E_cli_command_99, help_cmd, __E_cli_command_100, help_sym, __E___E_cli_command_100_101, __E___E_cli_command_100_102, help_arg, file_cmd, __E_cli_command_103, 
		file_sym, q_string, valid_cmd, __E_cli_command_104, valid_sym, sat_cmd, __E_cli_command_105, sat_sym, normalize_cmd_arg, unsat_cmd, 
		__E_cli_command_106, unsat_sym, solve_cmd, __E_cli_command_107, solve_sym, __E___E_cli_command_107_108, solve_cmd_arg, run_cmd, __E_cli_command_109, run_sym, 
		__E___E_cli_command_109_110, normalize_cmd, __E_cli_command_111, normalize_sym, __E___E_cli_command_111_112, subst_cmd, __E_cli_command_113, subst_sym, __E___E_cli_command_113_114, nf_cmd_arg, 
		inst_cmd, __E_cli_command_115, inst_sym, __E___E_cli_command_115_116, inst_args, dnf_cmd, __E_cli_command_117, dnf_sym, cnf_cmd, __E_cli_command_118, 
		cnf_sym, anf_cmd, __E_cli_command_119, anf_sym, nnf_cmd, __E_cli_command_120, nnf_sym, pnf_cmd, __E_cli_command_121, pnf_sym, 
		mnf_cmd, __E_cli_command_122, mnf_sym, snf_cmd, __E_cli_command_123, snf_sym, onf_cmd, __E_cli_command_124, onf_sym, onf_cmd_arg, 
		def_list_cmd, __E_cli_command_125, def_sym, __E___E_cli_command_125_126, def_print_cmd, __E_cli_command_127, number, def_rr_cmd, qelim_cmd, __E_cli_command_128, 
		qelim_sym, wff_cmd_arg, get_cmd, __E_cli_command_129, get_sym, __E___E_cli_command_129_130, option, set_cmd, __E_cli_command_131, set_sym, 
		__E___E_cli_command_131_132, option_value, enable_cmd, __E_cli_command_133, enable_sym, bool_option, disable_cmd, __E_cli_command_134, disable_sym, toggle_cmd, 
		__E_cli_command_135, toggle_sym, def_input_cmd, __E_cli_command_136, input_stream, def_output_cmd, __E_cli_command_137, output_stream, history_list_cmd, __E_cli_command_138, 
		history_sym, __E___E_cli_command_138_139, history_print_cmd, __E_cli_command_140, memory, history_store_cmd, __E_cli_command_141, __E_input_stream_142, q_file_name, console_sym, 
		__E_output_stream_143, file_name, __E_file_name_144, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, execute_sym, examples_sym, __E_help_arg_145, 
		__E___E_help_arg_145_146, rel_memory, __E_memory_147, __E___E_memory_147_148, memory_id, abs_memory, __E_memory_149, enum_option, severity_opt, __E_option_150, 
		status_opt, __E_bool_option_151, colors_opt, __E_bool_option_152, charvar_opt, __E_bool_option_153, hilighting_opt, __E_bool_option_154, indenting_opt, __E_bool_option_155, 
		debug_repl_opt, __E_bool_option_156, option_value_true, option_value_false, severity, error_sym, __E_severity_157, info_sym, __E_severity_158, debug_sym, 
		__E_severity_159, trace_sym, __E_severity_160, named_binding, __N_0, __N_1, __N_2, __N_3, 
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
