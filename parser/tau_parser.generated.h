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
	"bf_nleq", "__E_wff_37", "bf_greater", "__E_wff_38", "bf_less_equal", "__E_wff_39", "bf_less", "__E_wff_40", "bf_parenthesis", "__E_bf_41", 
	"variable", "flag", "bf_splitter", "__E_bf_42", "bf_ref", "bf_or", "__E_bf_43", "bf_xor", "__E_bf_44", "bf_and", 
	"__E_bf_45", "__E___E_bf_45_46", "bf_neg", "__E_bf_47", "bf_constant", "__E_bf_48", "constant", "bf_t", "bf_f", "uninterpreted_constant", 
	"flag_neq", "__E_flag_49", "flagvar", "num", "flag_eq", "__E_flag_50", "flag_greater_equal", "__E_flag_51", "flag_greater", "__E_flag_52", 
	"flag_less_equal", "__E_flag_53", "flag_less", "__E_flag_54", "charvar", "__E_charvar_55", "__E_charvar_56", "io_var", "__E_variable_57", "out", 
	"in_var_name", "__E_in_58", "__E___E_in_58_59", "__E___E___E_in_58_59_60", "offset", "out_var_name", "__E_out_61", "__E___E_out_61_62", "__E___E___E_out_61_62_63", "q_var", 
	"__E_q_vars_64", "__E_q_vars_65", "__E_offsets_66", "__E_offsets_67", "shift", "__E_offset_68", "__E_shift_69", "__E___E_shift_69_70", "digits", "uninter_const_name", 
	"__E_uninterpreted_constant_71", "__E___E_uninterpreted_constant_71_72", "chars", "binding", "__E_binding_73", "type", "__E___E_binding_73_74", "source", "named_binding", "src_c", 
	"__E_source_75", "__E___E_source_75_76", "__E___E_source_75_77", "__E_src_c_78", "__E_src_c_79", "__E_chars_80", "__E_digits_81", "extra", "comment", "__E_comment_82", 
	"__E_comment_83", "__E_comment_84", "__E____85", "__E___86", "bf_cb_args1", "bf_cb_arg", "bf_cb_args2", "bf_cb_args3", "bf_cb_args4", "wff_cb_args1", 
	"wff_cb_arg", "wff_cb_args2", "wff_cb_args3", "wff_cb_args4", "wff_has_clashing_subformulas_cb", "__E_wff_cb_87", "wff_has_subformula_cb", "__E_wff_cb_88", "wff_remove_existential_cb", "__E_wff_cb_89", 
	"bf_has_subformula_cb", "__E_bf_cb_90", "bf_remove_funiversal_cb", "__E_bf_cb_91", "bf_remove_fexistential_cb", "__E_bf_cb_92", "bf_normalize_cb", "__E_bf_cb_93", "cli", "cli_command", 
	"__E_cli_94", "__E_cli_95", "quit_cmd", "quit_sym", "__E_cli_command_96", "version_cmd", "version_sym", "__E_cli_command_97", "clear_cmd", "clear_sym", 
	"__E_cli_command_98", "help_cmd", "__E_cli_command_99", "help_sym", "__E___E_cli_command_99_100", "__E___E_cli_command_99_101", "help_arg", "file_cmd", "__E_cli_command_102", "file_sym", 
	"q_string", "valid_cmd", "__E_cli_command_103", "valid_sym", "sat_cmd", "__E_cli_command_104", "sat_sym", "normalize_cmd_arg", "unsat_cmd", "__E_cli_command_105", 
	"unsat_sym", "solve_cmd", "__E_cli_command_106", "solve_sym", "__E___E_cli_command_106_107", "solve_cmd_arg", "run_cmd", "__E_cli_command_108", "run_sym", "__E___E_cli_command_108_109", 
	"normalize_cmd", "__E_cli_command_110", "normalize_sym", "__E___E_cli_command_110_111", "subst_cmd", "__E_cli_command_112", "subst_sym", "__E___E_cli_command_112_113", "nf_cmd_arg", "inst_cmd", 
	"__E_cli_command_114", "inst_sym", "__E___E_cli_command_114_115", "inst_args", "dnf_cmd", "__E_cli_command_116", "dnf_sym", "cnf_cmd", "__E_cli_command_117", "cnf_sym", 
	"anf_cmd", "__E_cli_command_118", "anf_sym", "nnf_cmd", "__E_cli_command_119", "nnf_sym", "pnf_cmd", "__E_cli_command_120", "pnf_sym", "mnf_cmd", 
	"__E_cli_command_121", "mnf_sym", "snf_cmd", "__E_cli_command_122", "snf_sym", "onf_cmd", "__E_cli_command_123", "onf_sym", "onf_cmd_arg", "def_list_cmd", 
	"__E_cli_command_124", "def_sym", "__E___E_cli_command_124_125", "def_print_cmd", "__E_cli_command_126", "number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_127", "qelim_sym", 
	"wff_cmd_arg", "get_cmd", "__E_cli_command_128", "get_sym", "__E___E_cli_command_128_129", "option", "set_cmd", "__E_cli_command_130", "set_sym", "__E___E_cli_command_130_131", 
	"option_value", "toggle_cmd", "__E_cli_command_132", "toggle_sym", "bool_option", "def_input_cmd", "__E_cli_command_133", "input_stream", "def_output_cmd", "__E_cli_command_134", 
	"output_stream", "history_list_cmd", "__E_cli_command_135", "history_sym", "__E___E_cli_command_135_136", "history_print_cmd", "__E_cli_command_137", "memory", "history_store_cmd", "__E_cli_command_138", 
	"__E_input_stream_139", "q_file_name", "keyboard_sym", "__E_input_stream_140", "__E_output_stream_141", "console_sym", "__E_output_stream_142", "file_name", "__E_file_name_143", "bf_cmd_arg", 
	"abs_memory_sym", "rel_memory_sym", "selection_sym", "execute_sym", "examples_sym", "__E_help_arg_144", "__E___E_help_arg_144_145", "rel_memory", "__E_memory_146", "__E___E_memory_146_147", 
	"memory_id", "abs_memory", "__E_memory_148", "enum_option", "severity_opt", "__E_option_149", "status_opt", "__E_bool_option_150", "colors_opt", "__E_bool_option_151", 
	"debug_repl_opt", "__E_bool_option_152", "option_value_true", "option_value_false", "severity", "error_sym", "__E_severity_153", "info_sym", "__E_severity_154", "debug_sym", 
	"__E_severity_155", "trace_sym", "__E_severity_156", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', ',', '(', ')', '<', '{', 
	'}', 's', 'o', 'm', 'e', 't', 'i', '>', 'a', 'l', 
	'w', 'y', '[', ']', '?', 'x', '-', '|', '^', '&', 
	'!', 'T', 'F', 'S', '+', '\'', '1', '0', '$', '\t', 
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
			11, 58
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			20, 22, 144, 150, 155, 168, 169, 175, 177, 178,
			347
		},
		.to_inline = {
			{ 14, 64, 14 },
			{ 21, 108, 21 },
			{ 49 },
			{ 53 },
			{ 159 },
			{ 172 },
			{ 179 },
			{ 194 },
			{ 196 },
			{ 197 },
			{ 198 },
			{ 199 },
			{ 201 },
			{ 202 },
			{ 203 },
			{ 247 },
			{ 255 },
			{ 268 },
			{ 273 },
			{ 298 },
			{ 310 },
			{ 349 }
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
//G126: __E_bf_41(109)       => '(' _(11) bf(21) _(11) ')'.
	p(NT(109), (T(5)+NT(11)+NT(21)+NT(11)+T(6)));
//G127: bf_parenthesis(108)  => __E_bf_41(109).
	p(NT(108), (NT(109)));
//G128: bf(21)               => bf_parenthesis(108).
	p(NT(21), (NT(108)));
//G129: bf(21)               => variable(110).
	p(NT(21), (NT(110)));
//G130: bf(21)               => flag(111).
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
//G158: bf(21)               => uninterpreted_constant(129).
	p(NT(21), (NT(129)));
//G159: __E_flag_49(131)     => flagvar(132) _(11) '!' '=' _(11) num(133).
	p(NT(131), (NT(132)+NT(11)+T(29)+T(3)+NT(11)+NT(133)));
//G160: __E_flag_49(131)     => num(133) _(11) '!' '=' _(11) flagvar(132).
	p(NT(131), (NT(133)+NT(11)+T(29)+T(3)+NT(11)+NT(132)));
//G161: flag_neq(130)        => __E_flag_49(131).
	p(NT(130), (NT(131)));
//G162: flag(111)            => flag_neq(130).
	p(NT(111), (NT(130)));
//G163: __E_flag_50(135)     => flagvar(132) _(11) '=' _(11) num(133).
	p(NT(135), (NT(132)+NT(11)+T(3)+NT(11)+NT(133)));
//G164: __E_flag_50(135)     => num(133) _(11) '=' _(11) flagvar(132).
	p(NT(135), (NT(133)+NT(11)+T(3)+NT(11)+NT(132)));
//G165: flag_eq(134)         => __E_flag_50(135).
	p(NT(134), (NT(135)));
//G166: flag(111)            => flag_eq(134).
	p(NT(111), (NT(134)));
//G167: __E_flag_51(137)     => flagvar(132) _(11) '>' '=' _(11) num(133).
	p(NT(137), (NT(132)+NT(11)+T(16)+T(3)+NT(11)+NT(133)));
//G168: __E_flag_51(137)     => num(133) _(11) '>' '=' _(11) flagvar(132).
	p(NT(137), (NT(133)+NT(11)+T(16)+T(3)+NT(11)+NT(132)));
//G169: flag_greater_equal(136) => __E_flag_51(137).
	p(NT(136), (NT(137)));
//G170: flag(111)            => flag_greater_equal(136).
	p(NT(111), (NT(136)));
//G171: __E_flag_52(139)     => flagvar(132) _(11) '>' _(11) num(133).
	p(NT(139), (NT(132)+NT(11)+T(16)+NT(11)+NT(133)));
//G172: __E_flag_52(139)     => num(133) _(11) '>' _(11) flagvar(132).
	p(NT(139), (NT(133)+NT(11)+T(16)+NT(11)+NT(132)));
//G173: flag_greater(138)    => __E_flag_52(139).
	p(NT(138), (NT(139)));
//G174: flag(111)            => flag_greater(138).
	p(NT(111), (NT(138)));
//G175: __E_flag_53(141)     => flagvar(132) _(11) '<' '=' _(11) num(133).
	p(NT(141), (NT(132)+NT(11)+T(7)+T(3)+NT(11)+NT(133)));
//G176: __E_flag_53(141)     => num(133) _(11) '<' '=' _(11) flagvar(132).
	p(NT(141), (NT(133)+NT(11)+T(7)+T(3)+NT(11)+NT(132)));
//G177: flag_less_equal(140) => __E_flag_53(141).
	p(NT(140), (NT(141)));
//G178: flag(111)            => flag_less_equal(140).
	p(NT(111), (NT(140)));
//G179: __E_flag_54(143)     => flagvar(132) _(11) '<' _(11) num(133).
	p(NT(143), (NT(132)+NT(11)+T(7)+NT(11)+NT(133)));
//G180: __E_flag_54(143)     => num(133) _(11) '<' _(11) flagvar(132).
	p(NT(143), (NT(133)+NT(11)+T(7)+NT(11)+NT(132)));
//G181: flag_less(142)       => __E_flag_54(143).
	p(NT(142), (NT(143)));
//G182: flag(111)            => flag_less(142).
	p(NT(111), (NT(142)));
//G183: __N_0(383)           => 'F'.
	p(NT(383), (T(31)));
//G184: __N_1(384)           => 'T'.
	p(NT(384), (T(30)));
//G185: __E_charvar_55(145)  => ~( __N_0(383) ) & ~( __N_1(384) ) & alpha(5).	 # conjunctive
	p(NT(145), ~(NT(383)) & ~(NT(384)) & (NT(5)));
//G186: __E_charvar_56(146)  => null.
	p(NT(146), (nul));
//G187: __E_charvar_56(146)  => digit(3) __E_charvar_56(146).
	p(NT(146), (NT(3)+NT(146)));
//G188: charvar(144)         => __E_charvar_55(145) __E_charvar_56(146).
	p(NT(144), (NT(145)+NT(146)));
//G189: flagvar(132)         => charvar(144).
	p(NT(132), (NT(144)));
//G190: variable(110)        => charvar(144).
	p(NT(110), (NT(144)));
//G191: __E_variable_57(148) => in(35).
	p(NT(148), (NT(35)));
//G192: __E_variable_57(148) => out(149).
	p(NT(148), (NT(149)));
//G193: io_var(147)          => __E_variable_57(148).
	p(NT(147), (NT(148)));
//G194: variable(110)        => io_var(147).
	p(NT(110), (NT(147)));
//G195: __E___E___E_in_58_59_60(153) => digit(3).
	p(NT(153), (NT(3)));
//G196: __E___E___E_in_58_59_60(153) => digit(3) __E___E___E_in_58_59_60(153).
	p(NT(153), (NT(3)+NT(153)));
//G197: __E___E_in_58_59(152) => __E___E___E_in_58_59_60(153).
	p(NT(152), (NT(153)));
//G198: __E_in_58(151)       => 'i' __E___E_in_58_59(152).
	p(NT(151), (T(15)+NT(152)));
//G199: in_var_name(150)     => __E_in_58(151).
	p(NT(150), (NT(151)));
//G200: in(35)               => in_var_name(150) '[' offset(154) ']'.
	p(NT(35), (NT(150)+T(21)+NT(154)+T(22)));
//G201: __E___E___E_out_61_62_63(158) => digit(3).
	p(NT(158), (NT(3)));
//G202: __E___E___E_out_61_62_63(158) => digit(3) __E___E___E_out_61_62_63(158).
	p(NT(158), (NT(3)+NT(158)));
//G203: __E___E_out_61_62(157) => __E___E___E_out_61_62_63(158).
	p(NT(157), (NT(158)));
//G204: __E_out_61(156)      => 'o' __E___E_out_61_62(157).
	p(NT(156), (T(11)+NT(157)));
//G205: out_var_name(155)    => __E_out_61(156).
	p(NT(155), (NT(156)));
//G206: out(149)             => out_var_name(155) '[' offset(154) ']'.
	p(NT(149), (NT(155)+T(21)+NT(154)+T(22)));
//G207: capture(20)          => '$' charvar(144).
	p(NT(20), (T(37)+NT(144)));
//G208: __E_q_vars_64(160)   => _(11) ',' _(11) q_var(159).
	p(NT(160), (NT(11)+T(4)+NT(11)+NT(159)));
//G209: __E_q_vars_65(161)   => null.
	p(NT(161), (nul));
//G210: __E_q_vars_65(161)   => __E_q_vars_64(160) __E_q_vars_65(161).
	p(NT(161), (NT(160)+NT(161)));
//G211: q_vars(76)           => q_var(159) __E_q_vars_65(161).
	p(NT(76), (NT(159)+NT(161)));
//G212: q_var(159)           => capture(20).
	p(NT(159), (NT(20)));
//G213: q_var(159)           => variable(110).
	p(NT(159), (NT(110)));
//G214: __E_offsets_66(162)  => _(11) ',' _(11) offset(154).
	p(NT(162), (NT(11)+T(4)+NT(11)+NT(154)));
//G215: __E_offsets_67(163)  => null.
	p(NT(163), (nul));
//G216: __E_offsets_67(163)  => __E_offsets_66(162) __E_offsets_67(163).
	p(NT(163), (NT(162)+NT(163)));
//G217: offsets(24)          => '[' _(11) offset(154) __E_offsets_67(163) _(11) ']'.
	p(NT(24), (T(21)+NT(11)+NT(154)+NT(163)+NT(11)+T(22)));
//G218: offset(154)          => num(133).
	p(NT(154), (NT(133)));
//G219: offset(154)          => capture(20).
	p(NT(154), (NT(20)));
//G220: offset(154)          => shift(164).
	p(NT(154), (NT(164)));
//G221: __N_2(385)           => io_var(147).
	p(NT(385), (NT(147)));
//G222: __E_offset_68(165)   => variable(110) & ~( __N_2(385) ).	 # conjunctive
	p(NT(165), (NT(110)) & ~(NT(385)));
//G223: offset(154)          => __E_offset_68(165).
	p(NT(154), (NT(165)));
//G224: __E_shift_69(166)    => capture(20).
	p(NT(166), (NT(20)));
//G225: __N_3(386)           => io_var(147).
	p(NT(386), (NT(147)));
//G226: __E___E_shift_69_70(167) => variable(110) & ~( __N_3(386) ).	 # conjunctive
	p(NT(167), (NT(110)) & ~(NT(386)));
//G227: __E_shift_69(166)    => __E___E_shift_69_70(167).
	p(NT(166), (NT(167)));
//G228: shift(164)           => __E_shift_69(166) _(11) '-' _(11) num(133).
	p(NT(164), (NT(166)+NT(11)+T(25)+NT(11)+NT(133)));
//G229: num(133)             => digits(168).
	p(NT(133), (NT(168)));
//G230: __E___E_uninterpreted_constant_71_72(171) => chars(172).
	p(NT(171), (NT(172)));
//G231: __E___E_uninterpreted_constant_71_72(171) => _(11).
	p(NT(171), (NT(11)));
//G232: __E_uninterpreted_constant_71(170) => __E___E_uninterpreted_constant_71_72(171) ':' chars(172).
	p(NT(170), (NT(171)+T(2)+NT(172)));
//G233: uninter_const_name(169) => __E_uninterpreted_constant_71(170).
	p(NT(169), (NT(170)));
//G234: uninterpreted_constant(129) => '<' uninter_const_name(169) _(11) '>'.
	p(NT(129), (T(7)+NT(169)+NT(11)+T(16)));
//G235: constant(126)        => binding(173).
	p(NT(126), (NT(173)));
//G236: constant(126)        => capture(20).
	p(NT(126), (NT(20)));
//G237: __E___E_binding_73_74(176) => chars(172) _(11).
	p(NT(176), (NT(172)+NT(11)));
//G238: __E___E_binding_73_74(176) => _(11).
	p(NT(176), (NT(11)));
//G239: type(175)            => __E___E_binding_73_74(176).
	p(NT(175), (NT(176)));
//G240: __E_binding_73(174)  => type(175) ':' _(11) source(177).
	p(NT(174), (NT(175)+T(2)+NT(11)+NT(177)));
//G241: source_binding(36)   => __E_binding_73(174).
	p(NT(36), (NT(174)));
//G242: binding(173)         => source_binding(36).
	p(NT(173), (NT(36)));
//G243: named_binding(178)   => chars(172).
	p(NT(178), (NT(172)));
//G244: binding(173)         => named_binding(178).
	p(NT(173), (NT(178)));
//G245: __E___E_source_75_76(181) => src_c(179).
	p(NT(181), (NT(179)));
//G246: __E___E_source_75_76(181) => space(2).
	p(NT(181), (NT(2)));
//G247: __E___E_source_75_77(182) => null.
	p(NT(182), (nul));
//G248: __E___E_source_75_77(182) => __E___E_source_75_76(181) __E___E_source_75_77(182).
	p(NT(182), (NT(181)+NT(182)));
//G249: __E_source_75(180)   => __E___E_source_75_77(182) src_c(179).
	p(NT(180), (NT(182)+NT(179)));
//G250: __E_source_75(180)   => null.
	p(NT(180), (nul));
//G251: source(177)          => src_c(179) __E_source_75(180).
	p(NT(177), (NT(179)+NT(180)));
//G252: src_c(179)           => alnum(6).
	p(NT(179), (NT(6)));
//G253: __N_4(387)           => '{'.
	p(NT(387), (T(8)));
//G254: __N_5(388)           => '}'.
	p(NT(388), (T(9)));
//G255: src_c(179)           => ~( __N_4(387) ) & ~( __N_5(388) ) & punct(7).	 # conjunctive
	p(NT(179), ~(NT(387)) & ~(NT(388)) & (NT(7)));
//G256: __E_src_c_78(183)    => src_c(179).
	p(NT(183), (NT(179)));
//G257: __E_src_c_78(183)    => space(2).
	p(NT(183), (NT(2)));
//G258: __E_src_c_79(184)    => null.
	p(NT(184), (nul));
//G259: __E_src_c_79(184)    => __E_src_c_78(183) __E_src_c_79(184).
	p(NT(184), (NT(183)+NT(184)));
//G260: src_c(179)           => '{' __E_src_c_79(184) '}'.
	p(NT(179), (T(8)+NT(184)+T(9)));
//G261: __E_chars_80(185)    => null.
	p(NT(185), (nul));
//G262: __E_chars_80(185)    => alnum(6) __E_chars_80(185).
	p(NT(185), (NT(6)+NT(185)));
//G263: chars(172)           => alpha(5) __E_chars_80(185).
	p(NT(172), (NT(5)+NT(185)));
//G264: __E_digits_81(186)   => digit(3).
	p(NT(186), (NT(3)));
//G265: __E_digits_81(186)   => digit(3) __E_digits_81(186).
	p(NT(186), (NT(3)+NT(186)));
//G266: digits(168)          => __E_digits_81(186).
	p(NT(168), (NT(186)));
//G267: sym(22)              => chars(172).
	p(NT(22), (NT(172)));
//G268: extra(187)           => chars(172).
	p(NT(187), (NT(172)));
//G269: __E_comment_82(189)  => printable(8).
	p(NT(189), (NT(8)));
//G270: __E_comment_82(189)  => '\t'.
	p(NT(189), (T(38)));
//G271: __E_comment_83(190)  => null.
	p(NT(190), (nul));
//G272: __E_comment_83(190)  => __E_comment_82(189) __E_comment_83(190).
	p(NT(190), (NT(189)+NT(190)));
//G273: __E_comment_84(191)  => '\n'.
	p(NT(191), (T(39)));
//G274: __E_comment_84(191)  => '\r'.
	p(NT(191), (T(40)));
//G275: __E_comment_84(191)  => eof(1).
	p(NT(191), (NT(1)));
//G276: comment(188)         => '#' __E_comment_83(190) __E_comment_84(191).
	p(NT(188), (T(41)+NT(190)+NT(191)));
//G277: __E____85(192)       => space(2).
	p(NT(192), (NT(2)));
//G278: __E____85(192)       => comment(188).
	p(NT(192), (NT(188)));
//G279: __(58)               => __E____85(192) _(11).
	p(NT(58), (NT(192)+NT(11)));
//G280: __E___86(193)        => __(58).
	p(NT(193), (NT(58)));
//G281: __E___86(193)        => null.
	p(NT(193), (nul));
//G282: _(11)                => __E___86(193).
	p(NT(11), (NT(193)));
//G283: bf_cb_arg(195)       => bf(21).
	p(NT(195), (NT(21)));
//G284: bf_cb_args1(194)     => __(58) bf_cb_arg(195).
	p(NT(194), (NT(58)+NT(195)));
//G285: bf_cb_args2(196)     => bf_cb_args1(194) bf_cb_args1(194).
	p(NT(196), (NT(194)+NT(194)));
//G286: bf_cb_args3(197)     => bf_cb_args2(196) bf_cb_args1(194).
	p(NT(197), (NT(196)+NT(194)));
//G287: bf_cb_args4(198)     => bf_cb_args3(197) bf_cb_args1(194).
	p(NT(198), (NT(197)+NT(194)));
//G288: wff_cb_arg(200)      => wff(14).
	p(NT(200), (NT(14)));
//G289: wff_cb_args1(199)    => __(58) wff_cb_arg(200).
	p(NT(199), (NT(58)+NT(200)));
//G290: wff_cb_args2(201)    => wff_cb_args1(199) wff_cb_args1(199).
	p(NT(201), (NT(199)+NT(199)));
//G291: wff_cb_args3(202)    => wff_cb_args2(201) wff_cb_args1(199).
	p(NT(202), (NT(201)+NT(199)));
//G292: wff_cb_args4(203)    => wff_cb_args3(202) wff_cb_args1(199).
	p(NT(203), (NT(202)+NT(199)));
//G293: __E_wff_cb_87(205)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(201).
	p(NT(205), (T(19)+T(42)+T(42)+T(43)+T(44)+T(17)+T(10)+T(43)+T(45)+T(18)+T(17)+T(10)+T(44)+T(15)+T(46)+T(47)+T(43)+T(10)+T(48)+T(49)+T(42)+T(11)+T(50)+T(12)+T(48)+T(18)+T(17)+T(10)+T(43)+T(45)+T(49)+NT(201)));
//G294: wff_has_clashing_subformulas_cb(204) => __E_wff_cb_87(205).
	p(NT(204), (NT(205)));
//G295: wff_cb(49)           => wff_has_clashing_subformulas_cb(204).
	p(NT(49), (NT(204)));
//G296: __E_wff_cb_88(207)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(202).
	p(NT(207), (T(19)+T(42)+T(42)+T(43)+T(44)+T(17)+T(10)+T(43)+T(10)+T(48)+T(49)+T(42)+T(11)+T(50)+T(12)+T(48)+T(18)+T(17)+T(43)+T(45)+T(49)+NT(202)));
//G297: wff_has_subformula_cb(206) => __E_wff_cb_88(207).
	p(NT(206), (NT(207)));
//G298: wff_cb(49)           => wff_has_subformula_cb(206).
	p(NT(49), (NT(206)));
//G299: __E_wff_cb_89(209)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(201).
	p(NT(209), (T(19)+T(42)+T(42)+T(43)+T(50)+T(13)+T(12)+T(11)+T(51)+T(13)+T(43)+T(13)+T(24)+T(15)+T(10)+T(14)+T(13)+T(46)+T(14)+T(15)+T(17)+T(18)+T(43)+T(45)+T(49)+NT(201)));
//G300: wff_remove_existential_cb(208) => __E_wff_cb_89(209).
	p(NT(208), (NT(209)));
//G301: wff_cb(49)           => wff_remove_existential_cb(208).
	p(NT(49), (NT(208)));
//G302: __E_bf_cb_90(211)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(197).
	p(NT(211), (T(49)+T(42)+T(43)+T(44)+T(17)+T(10)+T(43)+T(10)+T(48)+T(49)+T(42)+T(11)+T(50)+T(12)+T(48)+T(18)+T(17)+T(43)+T(45)+T(49)+NT(197)));
//G303: bf_has_subformula_cb(210) => __E_bf_cb_90(211).
	p(NT(210), (NT(211)));
//G304: bf_cb(53)            => bf_has_subformula_cb(210).
	p(NT(53), (NT(210)));
//G305: __E_bf_cb_91(213)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(198).
	p(NT(213), (T(49)+T(42)+T(43)+T(50)+T(13)+T(12)+T(11)+T(51)+T(13)+T(43)+T(42)+T(48)+T(46)+T(15)+T(51)+T(13)+T(50)+T(10)+T(17)+T(18)+T(43)+T(45)+T(49)+NT(198)));
//G306: bf_remove_funiversal_cb(212) => __E_bf_cb_91(213).
	p(NT(212), (NT(213)));
//G307: bf_cb(53)            => bf_remove_funiversal_cb(212).
	p(NT(53), (NT(212)));
//G308: __E_bf_cb_92(215)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(198).
	p(NT(215), (T(49)+T(42)+T(43)+T(50)+T(13)+T(12)+T(11)+T(51)+T(13)+T(43)+T(42)+T(13)+T(24)+T(15)+T(10)+T(14)+T(13)+T(46)+T(14)+T(15)+T(17)+T(18)+T(43)+T(45)+T(49)+NT(198)));
//G309: bf_remove_fexistential_cb(214) => __E_bf_cb_92(215).
	p(NT(214), (NT(215)));
//G310: bf_cb(53)            => bf_remove_fexistential_cb(214).
	p(NT(53), (NT(214)));
//G311: __E_bf_cb_93(217)    => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(194).
	p(NT(217), (T(49)+T(42)+T(43)+T(46)+T(11)+T(50)+T(12)+T(17)+T(18)+T(15)+T(52)+T(13)+T(43)+T(45)+T(49)+NT(194)));
//G312: bf_normalize_cb(216) => __E_bf_cb_93(217).
	p(NT(216), (NT(217)));
//G313: bf_cb(53)            => bf_normalize_cb(216).
	p(NT(53), (NT(216)));
//G314: cli(218)             => _(11).
	p(NT(218), (NT(11)));
//G315: __E_cli_94(220)      => '.' _(11) cli_command(219) _(11).
	p(NT(220), (T(1)+NT(11)+NT(219)+NT(11)));
//G316: __E_cli_95(221)      => null.
	p(NT(221), (nul));
//G317: __E_cli_95(221)      => __E_cli_94(220) __E_cli_95(221).
	p(NT(221), (NT(220)+NT(221)));
//G318: cli(218)             => _(11) cli_command(219) _(11) __E_cli_95(221).
	p(NT(218), (NT(11)+NT(219)+NT(11)+NT(221)));
//G319: __E_cli_command_96(224) => 'q'.
	p(NT(224), (T(53)));
//G320: __E_cli_command_96(224) => 'q' 'u' 'i' 't'.
	p(NT(224), (T(53)+T(48)+T(15)+T(14)));
//G321: quit_sym(223)        => __E_cli_command_96(224).
	p(NT(223), (NT(224)));
//G322: quit_cmd(222)        => quit_sym(223).
	p(NT(222), (NT(223)));
//G323: cli_command(219)     => quit_cmd(222).
	p(NT(219), (NT(222)));
//G324: __E_cli_command_97(227) => 'v'.
	p(NT(227), (T(51)));
//G325: __E_cli_command_97(227) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(227), (T(51)+T(13)+T(50)+T(10)+T(15)+T(11)+T(46)));
//G326: version_sym(226)     => __E_cli_command_97(227).
	p(NT(226), (NT(227)));
//G327: version_cmd(225)     => version_sym(226).
	p(NT(225), (NT(226)));
//G328: cli_command(219)     => version_cmd(225).
	p(NT(219), (NT(225)));
//G329: __E_cli_command_98(230) => 'c'.
	p(NT(230), (T(45)));
//G330: __E_cli_command_98(230) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(230), (T(45)+T(18)+T(13)+T(17)+T(50)));
//G331: clear_sym(229)       => __E_cli_command_98(230).
	p(NT(229), (NT(230)));
//G332: clear_cmd(228)       => clear_sym(229).
	p(NT(228), (NT(229)));
//G333: cli_command(219)     => clear_cmd(228).
	p(NT(219), (NT(228)));
//G334: __E___E_cli_command_99_100(234) => 'h'.
	p(NT(234), (T(44)));
//G335: __E___E_cli_command_99_100(234) => 'h' 'e' 'l' 'p'.
	p(NT(234), (T(44)+T(13)+T(18)+T(54)));
//G336: help_sym(233)        => __E___E_cli_command_99_100(234).
	p(NT(233), (NT(234)));
//G337: __E___E_cli_command_99_101(235) => __(58) help_arg(236).
	p(NT(235), (NT(58)+NT(236)));
//G338: __E___E_cli_command_99_101(235) => null.
	p(NT(235), (nul));
//G339: __E_cli_command_99(232) => help_sym(233) __E___E_cli_command_99_101(235).
	p(NT(232), (NT(233)+NT(235)));
//G340: help_cmd(231)        => __E_cli_command_99(232).
	p(NT(231), (NT(232)));
//G341: cli_command(219)     => help_cmd(231).
	p(NT(219), (NT(231)));
//G342: file_sym(239)        => 'f' 'i' 'l' 'e'.
	p(NT(239), (T(42)+T(15)+T(18)+T(13)));
//G343: __E_cli_command_102(238) => file_sym(239) __(58) q_string(240).
	p(NT(238), (NT(239)+NT(58)+NT(240)));
//G344: file_cmd(237)        => __E_cli_command_102(238).
	p(NT(237), (NT(238)));
//G345: cli_command(219)     => file_cmd(237).
	p(NT(219), (NT(237)));
//G346: valid_sym(243)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(243), (T(51)+T(17)+T(18)+T(15)+T(55)));
//G347: __E_cli_command_103(242) => valid_sym(243) __(58) rr(10).
	p(NT(242), (NT(243)+NT(58)+NT(10)));
//G348: valid_cmd(241)       => __E_cli_command_103(242).
	p(NT(241), (NT(242)));
//G349: cli_command(219)     => valid_cmd(241).
	p(NT(219), (NT(241)));
//G350: sat_sym(246)         => 's' 'a' 't'.
	p(NT(246), (T(10)+T(17)+T(14)));
//G351: __E_cli_command_104(245) => sat_sym(246) __(58) normalize_cmd_arg(247).
	p(NT(245), (NT(246)+NT(58)+NT(247)));
//G352: sat_cmd(244)         => __E_cli_command_104(245).
	p(NT(244), (NT(245)));
//G353: cli_command(219)     => sat_cmd(244).
	p(NT(219), (NT(244)));
//G354: unsat_sym(250)       => 'u' 'n' 's' 'a' 't'.
	p(NT(250), (T(48)+T(46)+T(10)+T(17)+T(14)));
//G355: __E_cli_command_105(249) => unsat_sym(250) __(58) rr(10).
	p(NT(249), (NT(250)+NT(58)+NT(10)));
//G356: unsat_cmd(248)       => __E_cli_command_105(249).
	p(NT(248), (NT(249)));
//G357: cli_command(219)     => unsat_cmd(248).
	p(NT(219), (NT(248)));
//G358: solve_sym(253)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(253), (T(10)+T(11)+T(18)+T(51)+T(13)));
//G359: __E___E_cli_command_106_107(254) => __(58) type(175).
	p(NT(254), (NT(58)+NT(175)));
//G360: __E___E_cli_command_106_107(254) => null.
	p(NT(254), (nul));
//G361: __E_cli_command_106(252) => solve_sym(253) __E___E_cli_command_106_107(254) __(58) solve_cmd_arg(255).
	p(NT(252), (NT(253)+NT(254)+NT(58)+NT(255)));
//G362: solve_cmd(251)       => __E_cli_command_106(252).
	p(NT(251), (NT(252)));
//G363: cli_command(219)     => solve_cmd(251).
	p(NT(219), (NT(251)));
//G364: __E___E_cli_command_108_109(259) => 'r'.
	p(NT(259), (T(50)));
//G365: __E___E_cli_command_108_109(259) => 'r' 'u' 'n'.
	p(NT(259), (T(50)+T(48)+T(46)));
//G366: run_sym(258)         => __E___E_cli_command_108_109(259).
	p(NT(258), (NT(259)));
//G367: __E_cli_command_108(257) => run_sym(258) __(58) wff(14).
	p(NT(257), (NT(258)+NT(58)+NT(14)));
//G368: run_cmd(256)         => __E_cli_command_108(257).
	p(NT(256), (NT(257)));
//G369: cli_command(219)     => run_cmd(256).
	p(NT(219), (NT(256)));
//G370: __E___E_cli_command_110_111(263) => 'n'.
	p(NT(263), (T(46)));
//G371: __E___E_cli_command_110_111(263) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(263), (T(46)+T(11)+T(50)+T(12)+T(17)+T(18)+T(15)+T(52)+T(13)));
//G372: normalize_sym(262)   => __E___E_cli_command_110_111(263).
	p(NT(262), (NT(263)));
//G373: __E_cli_command_110(261) => normalize_sym(262) __(58) normalize_cmd_arg(247).
	p(NT(261), (NT(262)+NT(58)+NT(247)));
//G374: normalize_cmd(260)   => __E_cli_command_110(261).
	p(NT(260), (NT(261)));
//G375: cli_command(219)     => normalize_cmd(260).
	p(NT(219), (NT(260)));
//G376: __E___E_cli_command_112_113(267) => 's'.
	p(NT(267), (T(10)));
//G377: __E___E_cli_command_112_113(267) => 's' 'u' 'b' 's' 't'.
	p(NT(267), (T(10)+T(48)+T(49)+T(10)+T(14)));
//G378: __E___E_cli_command_112_113(267) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(267), (T(10)+T(48)+T(49)+T(10)+T(14)+T(15)+T(14)+T(48)+T(14)+T(13)));
//G379: subst_sym(266)       => __E___E_cli_command_112_113(267).
	p(NT(266), (NT(267)));
//G380: __E_cli_command_112(265) => subst_sym(266) __(58) nf_cmd_arg(268) _(11) '[' _(11) nf_cmd_arg(268) _(11) '/' _(11) nf_cmd_arg(268) _(11) ']'.
	p(NT(265), (NT(266)+NT(58)+NT(268)+NT(11)+T(21)+NT(11)+NT(268)+NT(11)+T(56)+NT(11)+NT(268)+NT(11)+T(22)));
//G381: subst_cmd(264)       => __E_cli_command_112(265).
	p(NT(264), (NT(265)));
//G382: cli_command(219)     => subst_cmd(264).
	p(NT(219), (NT(264)));
//G383: __E___E_cli_command_114_115(272) => 'i'.
	p(NT(272), (T(15)));
//G384: __E___E_cli_command_114_115(272) => 'i' 'n' 's' 't'.
	p(NT(272), (T(15)+T(46)+T(10)+T(14)));
//G385: __E___E_cli_command_114_115(272) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(272), (T(15)+T(46)+T(10)+T(14)+T(17)+T(46)+T(14)+T(15)+T(17)+T(14)+T(13)));
//G386: inst_sym(271)        => __E___E_cli_command_114_115(272).
	p(NT(271), (NT(272)));
//G387: __E_cli_command_114(270) => inst_sym(271) __(58) inst_args(273).
	p(NT(270), (NT(271)+NT(58)+NT(273)));
//G388: inst_cmd(269)        => __E_cli_command_114(270).
	p(NT(269), (NT(270)));
//G389: cli_command(219)     => inst_cmd(269).
	p(NT(219), (NT(269)));
//G390: dnf_sym(276)         => 'd' 'n' 'f'.
	p(NT(276), (T(55)+T(46)+T(42)));
//G391: __E_cli_command_116(275) => dnf_sym(276) __(58) nf_cmd_arg(268).
	p(NT(275), (NT(276)+NT(58)+NT(268)));
//G392: dnf_cmd(274)         => __E_cli_command_116(275).
	p(NT(274), (NT(275)));
//G393: cli_command(219)     => dnf_cmd(274).
	p(NT(219), (NT(274)));
//G394: cnf_sym(279)         => 'c' 'n' 'f'.
	p(NT(279), (T(45)+T(46)+T(42)));
//G395: __E_cli_command_117(278) => cnf_sym(279) __(58) nf_cmd_arg(268).
	p(NT(278), (NT(279)+NT(58)+NT(268)));
//G396: cnf_cmd(277)         => __E_cli_command_117(278).
	p(NT(277), (NT(278)));
//G397: cli_command(219)     => cnf_cmd(277).
	p(NT(219), (NT(277)));
//G398: anf_sym(282)         => 'a' 'n' 'f'.
	p(NT(282), (T(17)+T(46)+T(42)));
//G399: __E_cli_command_118(281) => anf_sym(282) __(58) nf_cmd_arg(268).
	p(NT(281), (NT(282)+NT(58)+NT(268)));
//G400: anf_cmd(280)         => __E_cli_command_118(281).
	p(NT(280), (NT(281)));
//G401: cli_command(219)     => anf_cmd(280).
	p(NT(219), (NT(280)));
//G402: nnf_sym(285)         => 'n' 'n' 'f'.
	p(NT(285), (T(46)+T(46)+T(42)));
//G403: __E_cli_command_119(284) => nnf_sym(285) __(58) nf_cmd_arg(268).
	p(NT(284), (NT(285)+NT(58)+NT(268)));
//G404: nnf_cmd(283)         => __E_cli_command_119(284).
	p(NT(283), (NT(284)));
//G405: cli_command(219)     => nnf_cmd(283).
	p(NT(219), (NT(283)));
//G406: pnf_sym(288)         => 'p' 'n' 'f'.
	p(NT(288), (T(54)+T(46)+T(42)));
//G407: __E_cli_command_120(287) => pnf_sym(288) __(58) nf_cmd_arg(268).
	p(NT(287), (NT(288)+NT(58)+NT(268)));
//G408: pnf_cmd(286)         => __E_cli_command_120(287).
	p(NT(286), (NT(287)));
//G409: cli_command(219)     => pnf_cmd(286).
	p(NT(219), (NT(286)));
//G410: mnf_sym(291)         => 'm' 'n' 'f'.
	p(NT(291), (T(12)+T(46)+T(42)));
//G411: __E_cli_command_121(290) => mnf_sym(291) __(58) nf_cmd_arg(268).
	p(NT(290), (NT(291)+NT(58)+NT(268)));
//G412: mnf_cmd(289)         => __E_cli_command_121(290).
	p(NT(289), (NT(290)));
//G413: cli_command(219)     => mnf_cmd(289).
	p(NT(219), (NT(289)));
//G414: snf_sym(294)         => 's' 'n' 'f'.
	p(NT(294), (T(10)+T(46)+T(42)));
//G415: __E_cli_command_122(293) => snf_sym(294) __(58) nf_cmd_arg(268).
	p(NT(293), (NT(294)+NT(58)+NT(268)));
//G416: snf_cmd(292)         => __E_cli_command_122(293).
	p(NT(292), (NT(293)));
//G417: cli_command(219)     => snf_cmd(292).
	p(NT(219), (NT(292)));
//G418: onf_sym(297)         => 'o' 'n' 'f'.
	p(NT(297), (T(11)+T(46)+T(42)));
//G419: __E_cli_command_123(296) => onf_sym(297) __(58) variable(110) __(58) onf_cmd_arg(298).
	p(NT(296), (NT(297)+NT(58)+NT(110)+NT(58)+NT(298)));
//G420: onf_cmd(295)         => __E_cli_command_123(296).
	p(NT(295), (NT(296)));
//G421: cli_command(219)     => onf_cmd(295).
	p(NT(219), (NT(295)));
//G422: __E___E_cli_command_124_125(302) => 'd' 'e' 'f' 's'.
	p(NT(302), (T(55)+T(13)+T(42)+T(10)));
//G423: __E___E_cli_command_124_125(302) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(302), (T(55)+T(13)+T(42)+T(15)+T(46)+T(15)+T(14)+T(15)+T(11)+T(46)+T(10)));
//G424: def_sym(301)         => __E___E_cli_command_124_125(302).
	p(NT(301), (NT(302)));
//G425: __E_cli_command_124(300) => def_sym(301).
	p(NT(300), (NT(301)));
//G426: def_list_cmd(299)    => __E_cli_command_124(300).
	p(NT(299), (NT(300)));
//G427: cli_command(219)     => def_list_cmd(299).
	p(NT(219), (NT(299)));
//G428: __E_cli_command_126(304) => def_sym(301) __(58) number(305).
	p(NT(304), (NT(301)+NT(58)+NT(305)));
//G429: def_print_cmd(303)   => __E_cli_command_126(304).
	p(NT(303), (NT(304)));
//G430: cli_command(219)     => def_print_cmd(303).
	p(NT(219), (NT(303)));
//G431: def_rr_cmd(306)      => rec_relation(16).
	p(NT(306), (NT(16)));
//G432: cli_command(219)     => def_rr_cmd(306).
	p(NT(219), (NT(306)));
//G433: qelim_sym(309)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(309), (T(53)+T(13)+T(18)+T(15)+T(12)));
//G434: __E_cli_command_127(308) => qelim_sym(309) __(58) wff_cmd_arg(310).
	p(NT(308), (NT(309)+NT(58)+NT(310)));
//G435: qelim_cmd(307)       => __E_cli_command_127(308).
	p(NT(307), (NT(308)));
//G436: cli_command(219)     => qelim_cmd(307).
	p(NT(219), (NT(307)));
//G437: get_sym(313)         => 'g' 'e' 't'.
	p(NT(313), (T(47)+T(13)+T(14)));
//G438: __E___E_cli_command_128_129(314) => __(58) option(315).
	p(NT(314), (NT(58)+NT(315)));
//G439: __E___E_cli_command_128_129(314) => null.
	p(NT(314), (nul));
//G440: __E_cli_command_128(312) => get_sym(313) __E___E_cli_command_128_129(314).
	p(NT(312), (NT(313)+NT(314)));
//G441: get_cmd(311)         => __E_cli_command_128(312).
	p(NT(311), (NT(312)));
//G442: cli_command(219)     => get_cmd(311).
	p(NT(219), (NT(311)));
//G443: set_sym(318)         => 's' 'e' 't'.
	p(NT(318), (T(10)+T(13)+T(14)));
//G444: __E___E_cli_command_130_131(319) => __(58).
	p(NT(319), (NT(58)));
//G445: __E___E_cli_command_130_131(319) => _(11) '=' _(11).
	p(NT(319), (NT(11)+T(3)+NT(11)));
//G446: __E_cli_command_130(317) => set_sym(318) __(58) option(315) __E___E_cli_command_130_131(319) option_value(320).
	p(NT(317), (NT(318)+NT(58)+NT(315)+NT(319)+NT(320)));
//G447: set_cmd(316)         => __E_cli_command_130(317).
	p(NT(316), (NT(317)));
//G448: cli_command(219)     => set_cmd(316).
	p(NT(219), (NT(316)));
//G449: toggle_sym(323)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(323), (T(14)+T(11)+T(47)+T(47)+T(18)+T(13)));
//G450: __E_cli_command_132(322) => toggle_sym(323) __(58) bool_option(324).
	p(NT(322), (NT(323)+NT(58)+NT(324)));
//G451: toggle_cmd(321)      => __E_cli_command_132(322).
	p(NT(321), (NT(322)));
//G452: cli_command(219)     => toggle_cmd(321).
	p(NT(219), (NT(321)));
//G453: __E_cli_command_133(326) => type(175) __(58) in_var_name(150) _(11) '=' _(11) input_stream(327).
	p(NT(326), (NT(175)+NT(58)+NT(150)+NT(11)+T(3)+NT(11)+NT(327)));
//G454: def_input_cmd(325)   => __E_cli_command_133(326).
	p(NT(325), (NT(326)));
//G455: cli_command(219)     => def_input_cmd(325).
	p(NT(219), (NT(325)));
//G456: __E_cli_command_134(329) => type(175) __(58) out_var_name(155) _(11) '=' _(11) output_stream(330).
	p(NT(329), (NT(175)+NT(58)+NT(155)+NT(11)+T(3)+NT(11)+NT(330)));
//G457: def_output_cmd(328)  => __E_cli_command_134(329).
	p(NT(328), (NT(329)));
//G458: cli_command(219)     => def_output_cmd(328).
	p(NT(219), (NT(328)));
//G459: __E___E_cli_command_135_136(334) => 'h' 'i' 's' 't'.
	p(NT(334), (T(44)+T(15)+T(10)+T(14)));
//G460: __E___E_cli_command_135_136(334) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(334), (T(44)+T(15)+T(10)+T(14)+T(11)+T(50)+T(20)));
//G461: history_sym(333)     => __E___E_cli_command_135_136(334).
	p(NT(333), (NT(334)));
//G462: __E_cli_command_135(332) => history_sym(333).
	p(NT(332), (NT(333)));
//G463: history_list_cmd(331) => __E_cli_command_135(332).
	p(NT(331), (NT(332)));
//G464: cli_command(219)     => history_list_cmd(331).
	p(NT(219), (NT(331)));
//G465: __E_cli_command_137(336) => history_sym(333) __(58) memory(337).
	p(NT(336), (NT(333)+NT(58)+NT(337)));
//G466: history_print_cmd(335) => __E_cli_command_137(336).
	p(NT(335), (NT(336)));
//G467: cli_command(219)     => history_print_cmd(335).
	p(NT(219), (NT(335)));
//G468: __E_cli_command_138(339) => wff(14).
	p(NT(339), (NT(14)));
//G469: __E_cli_command_138(339) => bf(21).
	p(NT(339), (NT(21)));
//G470: history_store_cmd(338) => __E_cli_command_138(339).
	p(NT(338), (NT(339)));
//G471: cli_command(219)     => history_store_cmd(338).
	p(NT(219), (NT(338)));
//G472: __E_input_stream_139(340) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(341) _(11) ')'.
	p(NT(340), (T(15)+T(42)+T(15)+T(18)+T(13)+NT(11)+T(5)+NT(11)+NT(341)+NT(11)+T(6)));
//G473: input_stream(327)    => __E_input_stream_139(340).
	p(NT(327), (NT(340)));
//G474: __E_input_stream_140(343) => 'k' 'e' 'y' 'b' 'o' 'a' 'r' 'd'.
	p(NT(343), (T(57)+T(13)+T(20)+T(49)+T(11)+T(17)+T(50)+T(55)));
//G475: keyboard_sym(342)    => __E_input_stream_140(343).
	p(NT(342), (NT(343)));
//G476: input_stream(327)    => keyboard_sym(342).
	p(NT(327), (NT(342)));
//G477: __E_output_stream_141(344) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(341) _(11) ')'.
	p(NT(344), (T(11)+T(42)+T(15)+T(18)+T(13)+NT(11)+T(5)+NT(11)+NT(341)+NT(11)+T(6)));
//G478: output_stream(330)   => __E_output_stream_141(344).
	p(NT(330), (NT(344)));
//G479: __E_output_stream_142(346) => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(346), (T(45)+T(11)+T(46)+T(10)+T(11)+T(18)+T(13)));
//G480: console_sym(345)     => __E_output_stream_142(346).
	p(NT(345), (NT(346)));
//G481: output_stream(330)   => console_sym(345).
	p(NT(330), (NT(345)));
//G482: q_file_name(341)     => '"' file_name(347) '"'.
	p(NT(341), (T(58)+NT(347)+T(58)));
//G483: __E_file_name_143(348) => printable(8).
	p(NT(348), (NT(8)));
//G484: __E_file_name_143(348) => printable(8) __E_file_name_143(348).
	p(NT(348), (NT(8)+NT(348)));
//G485: file_name(347)       => __E_file_name_143(348).
	p(NT(347), (NT(348)));
//G486: number(305)          => digits(168).
	p(NT(305), (NT(168)));
//G487: bf_cmd_arg(349)      => memory(337).
	p(NT(349), (NT(337)));
//G488: bf_cmd_arg(349)      => bf(21).
	p(NT(349), (NT(21)));
//G489: wff_cmd_arg(310)     => memory(337).
	p(NT(310), (NT(337)));
//G490: wff_cmd_arg(310)     => wff(14).
	p(NT(310), (NT(14)));
//G491: solve_cmd_arg(255)   => memory(337).
	p(NT(255), (NT(337)));
//G492: solve_cmd_arg(255)   => wff(14).
	p(NT(255), (NT(14)));
//G493: nf_cmd_arg(268)      => memory(337).
	p(NT(268), (NT(337)));
//G494: nf_cmd_arg(268)      => wff(14).
	p(NT(268), (NT(14)));
//G495: nf_cmd_arg(268)      => bf(21).
	p(NT(268), (NT(21)));
//G496: onf_cmd_arg(298)     => memory(337).
	p(NT(298), (NT(337)));
//G497: onf_cmd_arg(298)     => wff(14).
	p(NT(298), (NT(14)));
//G498: normalize_cmd_arg(247) => memory(337).
	p(NT(247), (NT(337)));
//G499: normalize_cmd_arg(247) => rr(10).
	p(NT(247), (NT(10)));
//G500: normalize_cmd_arg(247) => ref(18).
	p(NT(247), (NT(18)));
//G501: normalize_cmd_arg(247) => wff(14).
	p(NT(247), (NT(14)));
//G502: normalize_cmd_arg(247) => bf(21).
	p(NT(247), (NT(21)));
//G503: inst_args(273)       => wff_cmd_arg(310) _(11) '[' _(11) variable(110) _(11) '/' _(11) bf_cmd_arg(349) _(11) ']'.
	p(NT(273), (NT(310)+NT(11)+T(21)+NT(11)+NT(110)+NT(11)+T(56)+NT(11)+NT(349)+NT(11)+T(22)));
//G504: inst_args(273)       => bf_cmd_arg(349) _(11) '[' _(11) variable(110) _(11) '/' _(11) bf_cmd_arg(349) _(11) ']'.
	p(NT(273), (NT(349)+NT(11)+T(21)+NT(11)+NT(110)+NT(11)+T(56)+NT(11)+NT(349)+NT(11)+T(22)));
//G505: help_arg(236)        => help_sym(233).
	p(NT(236), (NT(233)));
//G506: help_arg(236)        => version_sym(226).
	p(NT(236), (NT(226)));
//G507: help_arg(236)        => quit_sym(223).
	p(NT(236), (NT(223)));
//G508: help_arg(236)        => clear_sym(229).
	p(NT(236), (NT(229)));
//G509: help_arg(236)        => get_sym(313).
	p(NT(236), (NT(313)));
//G510: help_arg(236)        => set_sym(318).
	p(NT(236), (NT(318)));
//G511: help_arg(236)        => toggle_sym(323).
	p(NT(236), (NT(323)));
//G512: help_arg(236)        => file_sym(239).
	p(NT(236), (NT(239)));
//G513: help_arg(236)        => history_sym(333).
	p(NT(236), (NT(333)));
//G514: help_arg(236)        => abs_memory_sym(350).
	p(NT(236), (NT(350)));
//G515: help_arg(236)        => rel_memory_sym(351).
	p(NT(236), (NT(351)));
//G516: help_arg(236)        => selection_sym(352).
	p(NT(236), (NT(352)));
//G517: help_arg(236)        => def_sym(301).
	p(NT(236), (NT(301)));
//G518: help_arg(236)        => inst_sym(271).
	p(NT(236), (NT(271)));
//G519: help_arg(236)        => subst_sym(266).
	p(NT(236), (NT(266)));
//G520: help_arg(236)        => normalize_sym(262).
	p(NT(236), (NT(262)));
//G521: help_arg(236)        => execute_sym(353).
	p(NT(236), (NT(353)));
//G522: help_arg(236)        => solve_sym(253).
	p(NT(236), (NT(253)));
//G523: help_arg(236)        => valid_sym(243).
	p(NT(236), (NT(243)));
//G524: help_arg(236)        => sat_sym(246).
	p(NT(236), (NT(246)));
//G525: help_arg(236)        => unsat_sym(250).
	p(NT(236), (NT(250)));
//G526: help_arg(236)        => dnf_sym(276).
	p(NT(236), (NT(276)));
//G527: help_arg(236)        => cnf_sym(279).
	p(NT(236), (NT(279)));
//G528: help_arg(236)        => anf_sym(282).
	p(NT(236), (NT(282)));
//G529: help_arg(236)        => snf_sym(294).
	p(NT(236), (NT(294)));
//G530: help_arg(236)        => nnf_sym(285).
	p(NT(236), (NT(285)));
//G531: help_arg(236)        => pnf_sym(288).
	p(NT(236), (NT(288)));
//G532: help_arg(236)        => mnf_sym(291).
	p(NT(236), (NT(291)));
//G533: help_arg(236)        => onf_sym(297).
	p(NT(236), (NT(297)));
//G534: help_arg(236)        => qelim_sym(309).
	p(NT(236), (NT(309)));
//G535: __E___E_help_arg_144_145(356) => 's'.
	p(NT(356), (T(10)));
//G536: __E___E_help_arg_144_145(356) => null.
	p(NT(356), (nul));
//G537: __E_help_arg_144(355) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_144_145(356).
	p(NT(355), (T(13)+T(24)+T(17)+T(12)+T(54)+T(18)+T(13)+NT(356)));
//G538: examples_sym(354)    => __E_help_arg_144(355).
	p(NT(354), (NT(355)));
//G539: help_arg(236)        => examples_sym(354).
	p(NT(236), (NT(354)));
//G540: __E_memory_146(358)  => '%'.
	p(NT(358), (T(59)));
//G541: rel_memory_sym(351)  => '%' '-'.
	p(NT(351), (T(59)+T(25)));
//G542: memory_id(360)       => digits(168).
	p(NT(360), (NT(168)));
//G543: __E___E_memory_146_147(359) => memory_id(360).
	p(NT(359), (NT(360)));
//G544: __E___E_memory_146_147(359) => null.
	p(NT(359), (nul));
//G545: __E_memory_146(358)  => rel_memory_sym(351) __E___E_memory_146_147(359).
	p(NT(358), (NT(351)+NT(359)));
//G546: rel_memory(357)      => __E_memory_146(358).
	p(NT(357), (NT(358)));
//G547: memory(337)          => rel_memory(357).
	p(NT(337), (NT(357)));
//G548: abs_memory_sym(350)  => '%'.
	p(NT(350), (T(59)));
//G549: __E_memory_148(362)  => abs_memory_sym(350) memory_id(360).
	p(NT(362), (NT(350)+NT(360)));
//G550: abs_memory(361)      => __E_memory_148(362).
	p(NT(361), (NT(362)));
//G551: memory(337)          => abs_memory(361).
	p(NT(337), (NT(361)));
//G552: option(315)          => bool_option(324).
	p(NT(315), (NT(324)));
//G553: __E_option_149(365)  => 's' 'e' 'v'.
	p(NT(365), (T(10)+T(13)+T(51)));
//G554: __E_option_149(365)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(365), (T(10)+T(13)+T(51)+T(13)+T(50)+T(15)+T(14)+T(20)));
//G555: severity_opt(364)    => __E_option_149(365).
	p(NT(364), (NT(365)));
//G556: enum_option(363)     => severity_opt(364).
	p(NT(363), (NT(364)));
//G557: option(315)          => enum_option(363).
	p(NT(315), (NT(363)));
//G558: __E_bool_option_150(367) => 's'.
	p(NT(367), (T(10)));
//G559: __E_bool_option_150(367) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(367), (T(10)+T(14)+T(17)+T(14)+T(48)+T(10)));
//G560: status_opt(366)      => __E_bool_option_150(367).
	p(NT(366), (NT(367)));
//G561: bool_option(324)     => status_opt(366).
	p(NT(324), (NT(366)));
//G562: __E_bool_option_151(369) => 'c'.
	p(NT(369), (T(45)));
//G563: __E_bool_option_151(369) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(369), (T(45)+T(11)+T(18)+T(11)+T(50)));
//G564: __E_bool_option_151(369) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(369), (T(45)+T(11)+T(18)+T(11)+T(50)+T(10)));
//G565: colors_opt(368)      => __E_bool_option_151(369).
	p(NT(368), (NT(369)));
//G566: bool_option(324)     => colors_opt(368).
	p(NT(324), (NT(368)));
//G567: __E_bool_option_152(371) => 'd'.
	p(NT(371), (T(55)));
//G568: __E_bool_option_152(371) => 'd' 'b' 'g'.
	p(NT(371), (T(55)+T(49)+T(47)));
//G569: __E_bool_option_152(371) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(371), (T(55)+T(13)+T(49)+T(48)+T(47)));
//G570: debug_repl_opt(370)  => __E_bool_option_152(371).
	p(NT(370), (NT(371)));
//G571: bool_option(324)     => debug_repl_opt(370).
	p(NT(324), (NT(370)));
//G572: option_value(320)    => option_value_true(372).
	p(NT(320), (NT(372)));
//G573: option_value(320)    => option_value_false(373).
	p(NT(320), (NT(373)));
//G574: option_value(320)    => severity(374).
	p(NT(320), (NT(374)));
//G575: option_value_true(372) => 't'.
	p(NT(372), (T(14)));
//G576: option_value_true(372) => 't' 'r' 'u' 'e'.
	p(NT(372), (T(14)+T(50)+T(48)+T(13)));
//G577: option_value_true(372) => 'o' 'n'.
	p(NT(372), (T(11)+T(46)));
//G578: option_value_true(372) => '1'.
	p(NT(372), (T(35)));
//G579: option_value_true(372) => 'y'.
	p(NT(372), (T(20)));
//G580: option_value_true(372) => 'y' 'e' 's'.
	p(NT(372), (T(20)+T(13)+T(10)));
//G581: option_value_false(373) => 'f'.
	p(NT(373), (T(42)));
//G582: option_value_false(373) => 'f' 'a' 'l' 's' 'e'.
	p(NT(373), (T(42)+T(17)+T(18)+T(10)+T(13)));
//G583: option_value_false(373) => 'o' 'f' 'f'.
	p(NT(373), (T(11)+T(42)+T(42)));
//G584: option_value_false(373) => '0'.
	p(NT(373), (T(36)));
//G585: option_value_false(373) => 'n'.
	p(NT(373), (T(46)));
//G586: option_value_false(373) => 'n' 'o'.
	p(NT(373), (T(46)+T(11)));
//G587: __E_severity_153(376) => 'e'.
	p(NT(376), (T(13)));
//G588: __E_severity_153(376) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(376), (T(13)+T(50)+T(50)+T(11)+T(50)));
//G589: error_sym(375)       => __E_severity_153(376).
	p(NT(375), (NT(376)));
//G590: severity(374)        => error_sym(375).
	p(NT(374), (NT(375)));
//G591: __E_severity_154(378) => 'i'.
	p(NT(378), (T(15)));
//G592: __E_severity_154(378) => 'i' 'n' 'f' 'o'.
	p(NT(378), (T(15)+T(46)+T(42)+T(11)));
//G593: info_sym(377)        => __E_severity_154(378).
	p(NT(377), (NT(378)));
//G594: severity(374)        => info_sym(377).
	p(NT(374), (NT(377)));
//G595: __E_severity_155(380) => 'd'.
	p(NT(380), (T(55)));
//G596: __E_severity_155(380) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(380), (T(55)+T(13)+T(49)+T(48)+T(47)));
//G597: debug_sym(379)       => __E_severity_155(380).
	p(NT(379), (NT(380)));
//G598: severity(374)        => debug_sym(379).
	p(NT(374), (NT(379)));
//G599: __E_severity_156(382) => 't'.
	p(NT(382), (T(14)));
//G600: __E_severity_156(382) => 't' 'r' 'a' 'c' 'e'.
	p(NT(382), (T(14)+T(50)+T(17)+T(45)+T(13)));
//G601: trace_sym(381)       => __E_severity_156(382).
	p(NT(381), (NT(382)));
//G602: severity(374)        => trace_sym(381).
	p(NT(374), (NT(381)));
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
		bf_nleq, __E_wff_37, bf_greater, __E_wff_38, bf_less_equal, __E_wff_39, bf_less, __E_wff_40, bf_parenthesis, __E_bf_41, 
		variable, flag, bf_splitter, __E_bf_42, bf_ref, bf_or, __E_bf_43, bf_xor, __E_bf_44, bf_and, 
		__E_bf_45, __E___E_bf_45_46, bf_neg, __E_bf_47, bf_constant, __E_bf_48, constant, bf_t, bf_f, uninterpreted_constant, 
		flag_neq, __E_flag_49, flagvar, num, flag_eq, __E_flag_50, flag_greater_equal, __E_flag_51, flag_greater, __E_flag_52, 
		flag_less_equal, __E_flag_53, flag_less, __E_flag_54, charvar, __E_charvar_55, __E_charvar_56, io_var, __E_variable_57, out, 
		in_var_name, __E_in_58, __E___E_in_58_59, __E___E___E_in_58_59_60, offset, out_var_name, __E_out_61, __E___E_out_61_62, __E___E___E_out_61_62_63, q_var, 
		__E_q_vars_64, __E_q_vars_65, __E_offsets_66, __E_offsets_67, shift, __E_offset_68, __E_shift_69, __E___E_shift_69_70, digits, uninter_const_name, 
		__E_uninterpreted_constant_71, __E___E_uninterpreted_constant_71_72, chars, binding, __E_binding_73, type, __E___E_binding_73_74, source, named_binding, src_c, 
		__E_source_75, __E___E_source_75_76, __E___E_source_75_77, __E_src_c_78, __E_src_c_79, __E_chars_80, __E_digits_81, extra, comment, __E_comment_82, 
		__E_comment_83, __E_comment_84, __E____85, __E___86, bf_cb_args1, bf_cb_arg, bf_cb_args2, bf_cb_args3, bf_cb_args4, wff_cb_args1, 
		wff_cb_arg, wff_cb_args2, wff_cb_args3, wff_cb_args4, wff_has_clashing_subformulas_cb, __E_wff_cb_87, wff_has_subformula_cb, __E_wff_cb_88, wff_remove_existential_cb, __E_wff_cb_89, 
		bf_has_subformula_cb, __E_bf_cb_90, bf_remove_funiversal_cb, __E_bf_cb_91, bf_remove_fexistential_cb, __E_bf_cb_92, bf_normalize_cb, __E_bf_cb_93, cli, cli_command, 
		__E_cli_94, __E_cli_95, quit_cmd, quit_sym, __E_cli_command_96, version_cmd, version_sym, __E_cli_command_97, clear_cmd, clear_sym, 
		__E_cli_command_98, help_cmd, __E_cli_command_99, help_sym, __E___E_cli_command_99_100, __E___E_cli_command_99_101, help_arg, file_cmd, __E_cli_command_102, file_sym, 
		q_string, valid_cmd, __E_cli_command_103, valid_sym, sat_cmd, __E_cli_command_104, sat_sym, normalize_cmd_arg, unsat_cmd, __E_cli_command_105, 
		unsat_sym, solve_cmd, __E_cli_command_106, solve_sym, __E___E_cli_command_106_107, solve_cmd_arg, run_cmd, __E_cli_command_108, run_sym, __E___E_cli_command_108_109, 
		normalize_cmd, __E_cli_command_110, normalize_sym, __E___E_cli_command_110_111, subst_cmd, __E_cli_command_112, subst_sym, __E___E_cli_command_112_113, nf_cmd_arg, inst_cmd, 
		__E_cli_command_114, inst_sym, __E___E_cli_command_114_115, inst_args, dnf_cmd, __E_cli_command_116, dnf_sym, cnf_cmd, __E_cli_command_117, cnf_sym, 
		anf_cmd, __E_cli_command_118, anf_sym, nnf_cmd, __E_cli_command_119, nnf_sym, pnf_cmd, __E_cli_command_120, pnf_sym, mnf_cmd, 
		__E_cli_command_121, mnf_sym, snf_cmd, __E_cli_command_122, snf_sym, onf_cmd, __E_cli_command_123, onf_sym, onf_cmd_arg, def_list_cmd, 
		__E_cli_command_124, def_sym, __E___E_cli_command_124_125, def_print_cmd, __E_cli_command_126, number, def_rr_cmd, qelim_cmd, __E_cli_command_127, qelim_sym, 
		wff_cmd_arg, get_cmd, __E_cli_command_128, get_sym, __E___E_cli_command_128_129, option, set_cmd, __E_cli_command_130, set_sym, __E___E_cli_command_130_131, 
		option_value, toggle_cmd, __E_cli_command_132, toggle_sym, bool_option, def_input_cmd, __E_cli_command_133, input_stream, def_output_cmd, __E_cli_command_134, 
		output_stream, history_list_cmd, __E_cli_command_135, history_sym, __E___E_cli_command_135_136, history_print_cmd, __E_cli_command_137, memory, history_store_cmd, __E_cli_command_138, 
		__E_input_stream_139, q_file_name, keyboard_sym, __E_input_stream_140, __E_output_stream_141, console_sym, __E_output_stream_142, file_name, __E_file_name_143, bf_cmd_arg, 
		abs_memory_sym, rel_memory_sym, selection_sym, execute_sym, examples_sym, __E_help_arg_144, __E___E_help_arg_144_145, rel_memory, __E_memory_146, __E___E_memory_146_147, 
		memory_id, abs_memory, __E_memory_148, enum_option, severity_opt, __E_option_149, status_opt, __E_bool_option_150, colors_opt, __E_bool_option_151, 
		debug_repl_opt, __E_bool_option_152, option_value_true, option_value_false, severity, error_sym, __E_severity_153, info_sym, __E_severity_154, debug_sym, 
		__E_severity_155, trace_sym, __E_severity_156, __N_0, __N_1, __N_2, __N_3, __N_4, __N_5, 
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
