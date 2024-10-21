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
	"__E_wff_37", "bf_less", "__E_wff_38", "bf_parenthesis", "__E_bf_39", "variable", "bf_splitter", "__E_bf_40", "bf_ref", "bf_or", 
	"__E_bf_41", "bf_xor", "__E_bf_42", "bf_and", "__E_bf_43", "__E___E_bf_43_44", "bf_neg", "__E_bf_45", "bf_constant", "__E_bf_46", 
	"constant", "bf_t", "bf_f", "uninterpreted_constant", "ctn_neq", "__E_constraint_47", "ctnvar", "num", "ctn_eq", "__E_constraint_48", 
	"ctn_greater_equal", "__E_constraint_49", "ctn_greater", "__E_constraint_50", "ctn_less_equal", "__E_constraint_51", "ctn_less", "__E_constraint_52", "charvar", "__E_charvar_53", 
	"__E_charvar_54", "io_var", "__E_variable_55", "in", "out", "in_var_name", "__E_in_56", "__E___E_in_56_57", "__E___E___E_in_56_57_58", "offset", 
	"out_var_name", "__E_out_59", "__E___E_out_59_60", "__E___E___E_out_59_60_61", "q_var", "__E_q_vars_62", "__E_q_vars_63", "__E_offsets_64", "__E_offsets_65", "shift", 
	"__E_offset_66", "__E_shift_67", "__E___E_shift_67_68", "digits", "uninter_const_name", "__E_uninterpreted_constant_69", "__E___E_uninterpreted_constant_69_70", "chars", "binding", "source_binding", 
	"__E_binding_71", "type", "__E___E_binding_71_72", "source", "named_binding", "src_c", "__E_source_73", "__E___E_source_73_74", "__E___E_source_73_75", "__E_src_c_76", 
	"__E_src_c_77", "__E_chars_78", "__E_digits_79", "extra", "comment", "__E_comment_80", "__E_comment_81", "__E_comment_82", "__E____83", "__E___84", 
	"bf_cb_args1", "bf_cb_arg", "bf_cb_args2", "bf_cb_args3", "bf_cb_args4", "wff_cb_args1", "wff_cb_arg", "wff_cb_args2", "wff_cb_args3", "wff_cb_args4", 
	"wff_has_clashing_subformulas_cb", "__E_wff_cb_85", "wff_has_subformula_cb", "__E_wff_cb_86", "wff_remove_existential_cb", "__E_wff_cb_87", "bf_has_subformula_cb", "__E_bf_cb_88", "bf_remove_funiversal_cb", "__E_bf_cb_89", 
	"bf_remove_fexistential_cb", "__E_bf_cb_90", "bf_normalize_cb", "__E_bf_cb_91", "cli", "cli_command", "__E_cli_92", "__E_cli_93", "quit_cmd", "quit_sym", 
	"__E_cli_command_94", "version_cmd", "version_sym", "__E_cli_command_95", "clear_cmd", "clear_sym", "__E_cli_command_96", "help_cmd", "__E_cli_command_97", "help_sym", 
	"__E___E_cli_command_97_98", "__E___E_cli_command_97_99", "help_arg", "file_cmd", "__E_cli_command_100", "file_sym", "q_string", "valid_cmd", "__E_cli_command_101", "valid_sym", 
	"sat_cmd", "__E_cli_command_102", "sat_sym", "normalize_cmd_arg", "unsat_cmd", "__E_cli_command_103", "unsat_sym", "solve_cmd", "__E_cli_command_104", "solve_sym", 
	"__E___E_cli_command_104_105", "solve_cmd_arg", "run_cmd", "__E_cli_command_106", "run_sym", "__E___E_cli_command_106_107", "__E___E_cli_command_106_108", "normalize_cmd", "__E_cli_command_109", "normalize_sym", 
	"__E___E_cli_command_109_110", "subst_cmd", "__E_cli_command_111", "subst_sym", "__E___E_cli_command_111_112", "nf_cmd_arg", "inst_cmd", "__E_cli_command_113", "inst_sym", "__E___E_cli_command_113_114", 
	"inst_args", "dnf_cmd", "__E_cli_command_115", "dnf_sym", "cnf_cmd", "__E_cli_command_116", "cnf_sym", "anf_cmd", "__E_cli_command_117", "anf_sym", 
	"nnf_cmd", "__E_cli_command_118", "nnf_sym", "pnf_cmd", "__E_cli_command_119", "pnf_sym", "mnf_cmd", "__E_cli_command_120", "mnf_sym", "snf_cmd", 
	"__E_cli_command_121", "snf_sym", "onf_cmd", "__E_cli_command_122", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_123", "def_sym", "__E___E_cli_command_123_124", 
	"def_print_cmd", "__E_cli_command_125", "number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_126", "qelim_sym", "wff_cmd_arg", "get_cmd", "__E_cli_command_127", 
	"get_sym", "__E___E_cli_command_127_128", "option", "set_cmd", "__E_cli_command_129", "set_sym", "__E___E_cli_command_129_130", "option_value", "toggle_cmd", "__E_cli_command_131", 
	"toggle_sym", "bool_option", "def_input_cmd", "__E_cli_command_132", "input_stream", "def_output_cmd", "__E_cli_command_133", "output_stream", "history_list_cmd", "__E_cli_command_134", 
	"history_sym", "__E___E_cli_command_134_135", "history_print_cmd", "__E_cli_command_136", "memory", "history_store_cmd", "__E_cli_command_137", "__E_input_stream_138", "q_file_name", "console_sym", 
	"__E_input_stream_139", "__E_output_stream_140", "__E_output_stream_141", "file_name", "__E_file_name_142", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "execute_sym", 
	"examples_sym", "__E_help_arg_143", "__E___E_help_arg_143_144", "rel_memory", "__E_memory_145", "__E___E_memory_145_146", "memory_id", "abs_memory", "__E_memory_147", "enum_option", 
	"severity_opt", "__E_option_148", "status_opt", "__E_bool_option_149", "colors_opt", "__E_bool_option_150", "debug_repl_opt", "__E_bool_option_151", "option_value_true", "option_value_false", 
	"severity", "error_sym", "__E_severity_152", "info_sym", "__E_severity_153", "debug_sym", "__E_severity_154", "trace_sym", "__E_severity_155", "__N_0", 
	"__N_1", "__N_2", "__N_3", "__N_4", "__N_5", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', ',', '(', ')', 's', 'o', 
	'm', 'e', 't', 'i', '<', '>', 'a', 'l', 'w', 'y', 
	'[', ']', '?', 'x', '-', '|', '^', '&', '!', 'T', 
	'F', 'S', '+', '\'', '{', '}', '1', '0', '$', '\t', 
	'\n', '\r', '#', 'f', '_', 'h', 'c', 'n', 'g', 'u', 
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
			20, 22, 138, 145, 150, 163, 164, 171, 173, 174,
			343
		},
		.to_inline = {
			{ 14, 58, 14 },
			{ 21, 103, 21 },
			{ 43 },
			{ 47 },
			{ 154 },
			{ 167 },
			{ 175 },
			{ 190 },
			{ 192 },
			{ 193 },
			{ 194 },
			{ 195 },
			{ 197 },
			{ 198 },
			{ 199 },
			{ 243 },
			{ 251 },
			{ 265 },
			{ 270 },
			{ 295 },
			{ 307 },
			{ 345 }
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
//G125: bf(21)               => variable(105).
	p(NT(21), (NT(105)));
//G126: __E_bf_40(107)       => 'S' _(11) '(' _(11) bf(21) _(11) ')'.
	p(NT(107), (T(30)+NT(11)+T(5)+NT(11)+NT(21)+NT(11)+T(6)));
//G127: bf_splitter(106)     => __E_bf_40(107).
	p(NT(106), (NT(107)));
//G128: bf(21)               => bf_splitter(106).
	p(NT(21), (NT(106)));
//G129: bf_ref(108)          => ref(18).
	p(NT(108), (NT(18)));
//G130: bf(21)               => bf_ref(108).
	p(NT(21), (NT(108)));
//G131: __E_bf_41(110)       => bf(21) _(11) '|' _(11) bf(21).
	p(NT(110), (NT(21)+NT(11)+T(24)+NT(11)+NT(21)));
//G132: bf_or(109)           => __E_bf_41(110).
	p(NT(109), (NT(110)));
//G133: bf(21)               => bf_or(109).
	p(NT(21), (NT(109)));
//G134: __E_bf_42(112)       => bf(21) _(11) '+' _(11) bf(21).
	p(NT(112), (NT(21)+NT(11)+T(31)+NT(11)+NT(21)));
//G135: bf_xor(111)          => __E_bf_42(112).
	p(NT(111), (NT(112)));
//G136: bf(21)               => bf_xor(111).
	p(NT(21), (NT(111)));
//G137: __E___E_bf_43_44(115) => _(11).
	p(NT(115), (NT(11)));
//G138: __E___E_bf_43_44(115) => _(11) '&' _(11).
	p(NT(115), (NT(11)+T(26)+NT(11)));
//G139: __E_bf_43(114)       => bf(21) __E___E_bf_43_44(115) bf(21).
	p(NT(114), (NT(21)+NT(115)+NT(21)));
//G140: bf_and(113)          => __E_bf_43(114).
	p(NT(113), (NT(114)));
//G141: bf(21)               => bf_and(113).
	p(NT(21), (NT(113)));
//G142: __E_bf_45(117)       => bf(21) _(11) '\''.
	p(NT(117), (NT(21)+NT(11)+T(32)));
//G143: bf_neg(116)          => __E_bf_45(117).
	p(NT(116), (NT(117)));
//G144: bf(21)               => bf_neg(116).
	p(NT(21), (NT(116)));
//G145: __E_bf_46(119)       => '{' _(11) constant(120) _(11) '}'.
	p(NT(119), (T(33)+NT(11)+NT(120)+NT(11)+T(34)));
//G146: bf_constant(118)     => __E_bf_46(119).
	p(NT(118), (NT(119)));
//G147: bf(21)               => bf_constant(118).
	p(NT(21), (NT(118)));
//G148: bf_t(121)            => '1'.
	p(NT(121), (T(35)));
//G149: bf(21)               => bf_t(121).
	p(NT(21), (NT(121)));
//G150: bf_f(122)            => '0'.
	p(NT(122), (T(36)));
//G151: bf(21)               => bf_f(122).
	p(NT(21), (NT(122)));
//G152: bf(21)               => capture(20).
	p(NT(21), (NT(20)));
//G153: bf(21)               => uninterpreted_constant(123).
	p(NT(21), (NT(123)));
//G154: __E_constraint_47(125) => '[' ctnvar(126) _(11) '!' '=' _(11) num(127) ']'.
	p(NT(125), (T(19)+NT(126)+NT(11)+T(27)+T(3)+NT(11)+NT(127)+T(20)));
//G155: __E_constraint_47(125) => '[' num(127) _(11) '!' '=' _(11) ctnvar(126) ']'.
	p(NT(125), (T(19)+NT(127)+NT(11)+T(27)+T(3)+NT(11)+NT(126)+T(20)));
//G156: ctn_neq(124)         => __E_constraint_47(125).
	p(NT(124), (NT(125)));
//G157: constraint(74)       => ctn_neq(124).
	p(NT(74), (NT(124)));
//G158: __E_constraint_48(129) => '[' ctnvar(126) _(11) '=' _(11) num(127) ']'.
	p(NT(129), (T(19)+NT(126)+NT(11)+T(3)+NT(11)+NT(127)+T(20)));
//G159: __E_constraint_48(129) => '[' num(127) _(11) '=' _(11) ctnvar(126) ']'.
	p(NT(129), (T(19)+NT(127)+NT(11)+T(3)+NT(11)+NT(126)+T(20)));
//G160: ctn_eq(128)          => __E_constraint_48(129).
	p(NT(128), (NT(129)));
//G161: constraint(74)       => ctn_eq(128).
	p(NT(74), (NT(128)));
//G162: __E_constraint_49(131) => '[' ctnvar(126) _(11) '>' '=' _(11) num(127) ']'.
	p(NT(131), (T(19)+NT(126)+NT(11)+T(14)+T(3)+NT(11)+NT(127)+T(20)));
//G163: __E_constraint_49(131) => '[' num(127) _(11) '>' '=' _(11) ctnvar(126) ']'.
	p(NT(131), (T(19)+NT(127)+NT(11)+T(14)+T(3)+NT(11)+NT(126)+T(20)));
//G164: ctn_greater_equal(130) => __E_constraint_49(131).
	p(NT(130), (NT(131)));
//G165: constraint(74)       => ctn_greater_equal(130).
	p(NT(74), (NT(130)));
//G166: __E_constraint_50(133) => '[' ctnvar(126) _(11) '>' _(11) num(127) ']'.
	p(NT(133), (T(19)+NT(126)+NT(11)+T(14)+NT(11)+NT(127)+T(20)));
//G167: __E_constraint_50(133) => '[' num(127) _(11) '>' _(11) ctnvar(126) ']'.
	p(NT(133), (T(19)+NT(127)+NT(11)+T(14)+NT(11)+NT(126)+T(20)));
//G168: ctn_greater(132)     => __E_constraint_50(133).
	p(NT(132), (NT(133)));
//G169: constraint(74)       => ctn_greater(132).
	p(NT(74), (NT(132)));
//G170: __E_constraint_51(135) => '[' ctnvar(126) _(11) '<' '=' _(11) num(127) ']'.
	p(NT(135), (T(19)+NT(126)+NT(11)+T(13)+T(3)+NT(11)+NT(127)+T(20)));
//G171: __E_constraint_51(135) => '[' num(127) _(11) '<' '=' _(11) ctnvar(126) ']'.
	p(NT(135), (T(19)+NT(127)+NT(11)+T(13)+T(3)+NT(11)+NT(126)+T(20)));
//G172: ctn_less_equal(134)  => __E_constraint_51(135).
	p(NT(134), (NT(135)));
//G173: constraint(74)       => ctn_less_equal(134).
	p(NT(74), (NT(134)));
//G174: __E_constraint_52(137) => '[' ctnvar(126) _(11) '<' _(11) num(127) ']'.
	p(NT(137), (T(19)+NT(126)+NT(11)+T(13)+NT(11)+NT(127)+T(20)));
//G175: __E_constraint_52(137) => '[' num(127) _(11) '<' _(11) ctnvar(126) ']'.
	p(NT(137), (T(19)+NT(127)+NT(11)+T(13)+NT(11)+NT(126)+T(20)));
//G176: ctn_less(136)        => __E_constraint_52(137).
	p(NT(136), (NT(137)));
//G177: constraint(74)       => ctn_less(136).
	p(NT(74), (NT(136)));
//G178: __N_0(379)           => 'F'.
	p(NT(379), (T(29)));
//G179: __N_1(380)           => 'T'.
	p(NT(380), (T(28)));
//G180: __E_charvar_53(139)  => ~( __N_0(379) ) & ~( __N_1(380) ) & alpha(5).	 # conjunctive
	p(NT(139), ~(NT(379)) & ~(NT(380)) & (NT(5)));
//G181: __E_charvar_54(140)  => null.
	p(NT(140), (nul));
//G182: __E_charvar_54(140)  => digit(3) __E_charvar_54(140).
	p(NT(140), (NT(3)+NT(140)));
//G183: charvar(138)         => __E_charvar_53(139) __E_charvar_54(140).
	p(NT(138), (NT(139)+NT(140)));
//G184: ctnvar(126)          => charvar(138).
	p(NT(126), (NT(138)));
//G185: variable(105)        => charvar(138).
	p(NT(105), (NT(138)));
//G186: __E_variable_55(142) => in(143).
	p(NT(142), (NT(143)));
//G187: __E_variable_55(142) => out(144).
	p(NT(142), (NT(144)));
//G188: io_var(141)          => __E_variable_55(142).
	p(NT(141), (NT(142)));
//G189: variable(105)        => io_var(141).
	p(NT(105), (NT(141)));
//G190: __E___E___E_in_56_57_58(148) => digit(3).
	p(NT(148), (NT(3)));
//G191: __E___E___E_in_56_57_58(148) => digit(3) __E___E___E_in_56_57_58(148).
	p(NT(148), (NT(3)+NT(148)));
//G192: __E___E_in_56_57(147) => __E___E___E_in_56_57_58(148).
	p(NT(147), (NT(148)));
//G193: __E_in_56(146)       => 'i' __E___E_in_56_57(147).
	p(NT(146), (T(12)+NT(147)));
//G194: in_var_name(145)     => __E_in_56(146).
	p(NT(145), (NT(146)));
//G195: in(143)              => in_var_name(145) '[' offset(149) ']'.
	p(NT(143), (NT(145)+T(19)+NT(149)+T(20)));
//G196: __E___E___E_out_59_60_61(153) => digit(3).
	p(NT(153), (NT(3)));
//G197: __E___E___E_out_59_60_61(153) => digit(3) __E___E___E_out_59_60_61(153).
	p(NT(153), (NT(3)+NT(153)));
//G198: __E___E_out_59_60(152) => __E___E___E_out_59_60_61(153).
	p(NT(152), (NT(153)));
//G199: __E_out_59(151)      => 'o' __E___E_out_59_60(152).
	p(NT(151), (T(8)+NT(152)));
//G200: out_var_name(150)    => __E_out_59(151).
	p(NT(150), (NT(151)));
//G201: out(144)             => out_var_name(150) '[' offset(149) ']'.
	p(NT(144), (NT(150)+T(19)+NT(149)+T(20)));
//G202: capture(20)          => '$' charvar(138).
	p(NT(20), (T(37)+NT(138)));
//G203: __E_q_vars_62(155)   => _(11) ',' _(11) q_var(154).
	p(NT(155), (NT(11)+T(4)+NT(11)+NT(154)));
//G204: __E_q_vars_63(156)   => null.
	p(NT(156), (nul));
//G205: __E_q_vars_63(156)   => __E_q_vars_62(155) __E_q_vars_63(156).
	p(NT(156), (NT(155)+NT(156)));
//G206: q_vars(70)           => q_var(154) __E_q_vars_63(156).
	p(NT(70), (NT(154)+NT(156)));
//G207: q_var(154)           => capture(20).
	p(NT(154), (NT(20)));
//G208: q_var(154)           => variable(105).
	p(NT(154), (NT(105)));
//G209: __E_offsets_64(157)  => _(11) ',' _(11) offset(149).
	p(NT(157), (NT(11)+T(4)+NT(11)+NT(149)));
//G210: __E_offsets_65(158)  => null.
	p(NT(158), (nul));
//G211: __E_offsets_65(158)  => __E_offsets_64(157) __E_offsets_65(158).
	p(NT(158), (NT(157)+NT(158)));
//G212: offsets(24)          => '[' _(11) offset(149) __E_offsets_65(158) _(11) ']'.
	p(NT(24), (T(19)+NT(11)+NT(149)+NT(158)+NT(11)+T(20)));
//G213: offset(149)          => num(127).
	p(NT(149), (NT(127)));
//G214: offset(149)          => capture(20).
	p(NT(149), (NT(20)));
//G215: offset(149)          => shift(159).
	p(NT(149), (NT(159)));
//G216: __N_2(381)           => io_var(141).
	p(NT(381), (NT(141)));
//G217: __E_offset_66(160)   => variable(105) & ~( __N_2(381) ).	 # conjunctive
	p(NT(160), (NT(105)) & ~(NT(381)));
//G218: offset(149)          => __E_offset_66(160).
	p(NT(149), (NT(160)));
//G219: __E_shift_67(161)    => capture(20).
	p(NT(161), (NT(20)));
//G220: __N_3(382)           => io_var(141).
	p(NT(382), (NT(141)));
//G221: __E___E_shift_67_68(162) => variable(105) & ~( __N_3(382) ).	 # conjunctive
	p(NT(162), (NT(105)) & ~(NT(382)));
//G222: __E_shift_67(161)    => __E___E_shift_67_68(162).
	p(NT(161), (NT(162)));
//G223: shift(159)           => __E_shift_67(161) _(11) '-' _(11) num(127).
	p(NT(159), (NT(161)+NT(11)+T(23)+NT(11)+NT(127)));
//G224: num(127)             => digits(163).
	p(NT(127), (NT(163)));
//G225: __E___E_uninterpreted_constant_69_70(166) => chars(167).
	p(NT(166), (NT(167)));
//G226: __E___E_uninterpreted_constant_69_70(166) => _(11).
	p(NT(166), (NT(11)));
//G227: __E_uninterpreted_constant_69(165) => __E___E_uninterpreted_constant_69_70(166) ':' chars(167).
	p(NT(165), (NT(166)+T(2)+NT(167)));
//G228: uninter_const_name(164) => __E_uninterpreted_constant_69(165).
	p(NT(164), (NT(165)));
//G229: uninterpreted_constant(123) => '<' uninter_const_name(164) _(11) '>'.
	p(NT(123), (T(13)+NT(164)+NT(11)+T(14)));
//G230: constant(120)        => binding(168).
	p(NT(120), (NT(168)));
//G231: constant(120)        => capture(20).
	p(NT(120), (NT(20)));
//G232: __E___E_binding_71_72(172) => chars(167) _(11).
	p(NT(172), (NT(167)+NT(11)));
//G233: __E___E_binding_71_72(172) => _(11).
	p(NT(172), (NT(11)));
//G234: type(171)            => __E___E_binding_71_72(172).
	p(NT(171), (NT(172)));
//G235: __E_binding_71(170)  => type(171) ':' _(11) source(173).
	p(NT(170), (NT(171)+T(2)+NT(11)+NT(173)));
//G236: source_binding(169)  => __E_binding_71(170).
	p(NT(169), (NT(170)));
//G237: binding(168)         => source_binding(169).
	p(NT(168), (NT(169)));
//G238: named_binding(174)   => chars(167).
	p(NT(174), (NT(167)));
//G239: binding(168)         => named_binding(174).
	p(NT(168), (NT(174)));
//G240: __E___E_source_73_74(177) => src_c(175).
	p(NT(177), (NT(175)));
//G241: __E___E_source_73_74(177) => space(2).
	p(NT(177), (NT(2)));
//G242: __E___E_source_73_75(178) => null.
	p(NT(178), (nul));
//G243: __E___E_source_73_75(178) => __E___E_source_73_74(177) __E___E_source_73_75(178).
	p(NT(178), (NT(177)+NT(178)));
//G244: __E_source_73(176)   => __E___E_source_73_75(178) src_c(175).
	p(NT(176), (NT(178)+NT(175)));
//G245: __E_source_73(176)   => null.
	p(NT(176), (nul));
//G246: source(173)          => src_c(175) __E_source_73(176).
	p(NT(173), (NT(175)+NT(176)));
//G247: src_c(175)           => alnum(6).
	p(NT(175), (NT(6)));
//G248: __N_4(383)           => '{'.
	p(NT(383), (T(33)));
//G249: __N_5(384)           => '}'.
	p(NT(384), (T(34)));
//G250: src_c(175)           => ~( __N_4(383) ) & ~( __N_5(384) ) & punct(7).	 # conjunctive
	p(NT(175), ~(NT(383)) & ~(NT(384)) & (NT(7)));
//G251: __E_src_c_76(179)    => src_c(175).
	p(NT(179), (NT(175)));
//G252: __E_src_c_76(179)    => space(2).
	p(NT(179), (NT(2)));
//G253: __E_src_c_77(180)    => null.
	p(NT(180), (nul));
//G254: __E_src_c_77(180)    => __E_src_c_76(179) __E_src_c_77(180).
	p(NT(180), (NT(179)+NT(180)));
//G255: src_c(175)           => '{' __E_src_c_77(180) '}'.
	p(NT(175), (T(33)+NT(180)+T(34)));
//G256: __E_chars_78(181)    => null.
	p(NT(181), (nul));
//G257: __E_chars_78(181)    => alnum(6) __E_chars_78(181).
	p(NT(181), (NT(6)+NT(181)));
//G258: chars(167)           => alpha(5) __E_chars_78(181).
	p(NT(167), (NT(5)+NT(181)));
//G259: __E_digits_79(182)   => digit(3).
	p(NT(182), (NT(3)));
//G260: __E_digits_79(182)   => digit(3) __E_digits_79(182).
	p(NT(182), (NT(3)+NT(182)));
//G261: digits(163)          => __E_digits_79(182).
	p(NT(163), (NT(182)));
//G262: sym(22)              => chars(167).
	p(NT(22), (NT(167)));
//G263: extra(183)           => chars(167).
	p(NT(183), (NT(167)));
//G264: __E_comment_80(185)  => printable(8).
	p(NT(185), (NT(8)));
//G265: __E_comment_80(185)  => '\t'.
	p(NT(185), (T(38)));
//G266: __E_comment_81(186)  => null.
	p(NT(186), (nul));
//G267: __E_comment_81(186)  => __E_comment_80(185) __E_comment_81(186).
	p(NT(186), (NT(185)+NT(186)));
//G268: __E_comment_82(187)  => '\n'.
	p(NT(187), (T(39)));
//G269: __E_comment_82(187)  => '\r'.
	p(NT(187), (T(40)));
//G270: __E_comment_82(187)  => eof(1).
	p(NT(187), (NT(1)));
//G271: comment(184)         => '#' __E_comment_81(186) __E_comment_82(187).
	p(NT(184), (T(41)+NT(186)+NT(187)));
//G272: __E____83(188)       => space(2).
	p(NT(188), (NT(2)));
//G273: __E____83(188)       => comment(184).
	p(NT(188), (NT(184)));
//G274: __(52)               => __E____83(188) _(11).
	p(NT(52), (NT(188)+NT(11)));
//G275: __E___84(189)        => __(52).
	p(NT(189), (NT(52)));
//G276: __E___84(189)        => null.
	p(NT(189), (nul));
//G277: _(11)                => __E___84(189).
	p(NT(11), (NT(189)));
//G278: bf_cb_arg(191)       => bf(21).
	p(NT(191), (NT(21)));
//G279: bf_cb_args1(190)     => __(52) bf_cb_arg(191).
	p(NT(190), (NT(52)+NT(191)));
//G280: bf_cb_args2(192)     => bf_cb_args1(190) bf_cb_args1(190).
	p(NT(192), (NT(190)+NT(190)));
//G281: bf_cb_args3(193)     => bf_cb_args2(192) bf_cb_args1(190).
	p(NT(193), (NT(192)+NT(190)));
//G282: bf_cb_args4(194)     => bf_cb_args3(193) bf_cb_args1(190).
	p(NT(194), (NT(193)+NT(190)));
//G283: wff_cb_arg(196)      => wff(14).
	p(NT(196), (NT(14)));
//G284: wff_cb_args1(195)    => __(52) wff_cb_arg(196).
	p(NT(195), (NT(52)+NT(196)));
//G285: wff_cb_args2(197)    => wff_cb_args1(195) wff_cb_args1(195).
	p(NT(197), (NT(195)+NT(195)));
//G286: wff_cb_args3(198)    => wff_cb_args2(197) wff_cb_args1(195).
	p(NT(198), (NT(197)+NT(195)));
//G287: wff_cb_args4(199)    => wff_cb_args3(198) wff_cb_args1(195).
	p(NT(199), (NT(198)+NT(195)));
//G288: __E_wff_cb_85(201)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(197).
	p(NT(201), (T(17)+T(42)+T(42)+T(43)+T(44)+T(15)+T(7)+T(43)+T(45)+T(16)+T(15)+T(7)+T(44)+T(12)+T(46)+T(47)+T(43)+T(7)+T(48)+T(49)+T(42)+T(8)+T(50)+T(9)+T(48)+T(16)+T(15)+T(7)+T(43)+T(45)+T(49)+NT(197)));
//G289: wff_has_clashing_subformulas_cb(200) => __E_wff_cb_85(201).
	p(NT(200), (NT(201)));
//G290: wff_cb(43)           => wff_has_clashing_subformulas_cb(200).
	p(NT(43), (NT(200)));
//G291: __E_wff_cb_86(203)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(198).
	p(NT(203), (T(17)+T(42)+T(42)+T(43)+T(44)+T(15)+T(7)+T(43)+T(7)+T(48)+T(49)+T(42)+T(8)+T(50)+T(9)+T(48)+T(16)+T(15)+T(43)+T(45)+T(49)+NT(198)));
//G292: wff_has_subformula_cb(202) => __E_wff_cb_86(203).
	p(NT(202), (NT(203)));
//G293: wff_cb(43)           => wff_has_subformula_cb(202).
	p(NT(43), (NT(202)));
//G294: __E_wff_cb_87(205)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(197).
	p(NT(205), (T(17)+T(42)+T(42)+T(43)+T(50)+T(10)+T(9)+T(8)+T(51)+T(10)+T(43)+T(10)+T(22)+T(12)+T(7)+T(11)+T(10)+T(46)+T(11)+T(12)+T(15)+T(16)+T(43)+T(45)+T(49)+NT(197)));
//G295: wff_remove_existential_cb(204) => __E_wff_cb_87(205).
	p(NT(204), (NT(205)));
//G296: wff_cb(43)           => wff_remove_existential_cb(204).
	p(NT(43), (NT(204)));
//G297: __E_bf_cb_88(207)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(193).
	p(NT(207), (T(49)+T(42)+T(43)+T(44)+T(15)+T(7)+T(43)+T(7)+T(48)+T(49)+T(42)+T(8)+T(50)+T(9)+T(48)+T(16)+T(15)+T(43)+T(45)+T(49)+NT(193)));
//G298: bf_has_subformula_cb(206) => __E_bf_cb_88(207).
	p(NT(206), (NT(207)));
//G299: bf_cb(47)            => bf_has_subformula_cb(206).
	p(NT(47), (NT(206)));
//G300: __E_bf_cb_89(209)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(194).
	p(NT(209), (T(49)+T(42)+T(43)+T(50)+T(10)+T(9)+T(8)+T(51)+T(10)+T(43)+T(42)+T(48)+T(46)+T(12)+T(51)+T(10)+T(50)+T(7)+T(15)+T(16)+T(43)+T(45)+T(49)+NT(194)));
//G301: bf_remove_funiversal_cb(208) => __E_bf_cb_89(209).
	p(NT(208), (NT(209)));
//G302: bf_cb(47)            => bf_remove_funiversal_cb(208).
	p(NT(47), (NT(208)));
//G303: __E_bf_cb_90(211)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(194).
	p(NT(211), (T(49)+T(42)+T(43)+T(50)+T(10)+T(9)+T(8)+T(51)+T(10)+T(43)+T(42)+T(10)+T(22)+T(12)+T(7)+T(11)+T(10)+T(46)+T(11)+T(12)+T(15)+T(16)+T(43)+T(45)+T(49)+NT(194)));
//G304: bf_remove_fexistential_cb(210) => __E_bf_cb_90(211).
	p(NT(210), (NT(211)));
//G305: bf_cb(47)            => bf_remove_fexistential_cb(210).
	p(NT(47), (NT(210)));
//G306: __E_bf_cb_91(213)    => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(190).
	p(NT(213), (T(49)+T(42)+T(43)+T(46)+T(8)+T(50)+T(9)+T(15)+T(16)+T(12)+T(52)+T(10)+T(43)+T(45)+T(49)+NT(190)));
//G307: bf_normalize_cb(212) => __E_bf_cb_91(213).
	p(NT(212), (NT(213)));
//G308: bf_cb(47)            => bf_normalize_cb(212).
	p(NT(47), (NT(212)));
//G309: cli(214)             => _(11).
	p(NT(214), (NT(11)));
//G310: __E_cli_92(216)      => '.' _(11) cli_command(215) _(11).
	p(NT(216), (T(1)+NT(11)+NT(215)+NT(11)));
//G311: __E_cli_93(217)      => null.
	p(NT(217), (nul));
//G312: __E_cli_93(217)      => __E_cli_92(216) __E_cli_93(217).
	p(NT(217), (NT(216)+NT(217)));
//G313: cli(214)             => _(11) cli_command(215) _(11) __E_cli_93(217).
	p(NT(214), (NT(11)+NT(215)+NT(11)+NT(217)));
//G314: __E_cli_command_94(220) => 'q'.
	p(NT(220), (T(53)));
//G315: __E_cli_command_94(220) => 'q' 'u' 'i' 't'.
	p(NT(220), (T(53)+T(48)+T(12)+T(11)));
//G316: quit_sym(219)        => __E_cli_command_94(220).
	p(NT(219), (NT(220)));
//G317: quit_cmd(218)        => quit_sym(219).
	p(NT(218), (NT(219)));
//G318: cli_command(215)     => quit_cmd(218).
	p(NT(215), (NT(218)));
//G319: __E_cli_command_95(223) => 'v'.
	p(NT(223), (T(51)));
//G320: __E_cli_command_95(223) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(223), (T(51)+T(10)+T(50)+T(7)+T(12)+T(8)+T(46)));
//G321: version_sym(222)     => __E_cli_command_95(223).
	p(NT(222), (NT(223)));
//G322: version_cmd(221)     => version_sym(222).
	p(NT(221), (NT(222)));
//G323: cli_command(215)     => version_cmd(221).
	p(NT(215), (NT(221)));
//G324: __E_cli_command_96(226) => 'c'.
	p(NT(226), (T(45)));
//G325: __E_cli_command_96(226) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(226), (T(45)+T(16)+T(10)+T(15)+T(50)));
//G326: clear_sym(225)       => __E_cli_command_96(226).
	p(NT(225), (NT(226)));
//G327: clear_cmd(224)       => clear_sym(225).
	p(NT(224), (NT(225)));
//G328: cli_command(215)     => clear_cmd(224).
	p(NT(215), (NT(224)));
//G329: __E___E_cli_command_97_98(230) => 'h'.
	p(NT(230), (T(44)));
//G330: __E___E_cli_command_97_98(230) => 'h' 'e' 'l' 'p'.
	p(NT(230), (T(44)+T(10)+T(16)+T(54)));
//G331: help_sym(229)        => __E___E_cli_command_97_98(230).
	p(NT(229), (NT(230)));
//G332: __E___E_cli_command_97_99(231) => __(52) help_arg(232).
	p(NT(231), (NT(52)+NT(232)));
//G333: __E___E_cli_command_97_99(231) => null.
	p(NT(231), (nul));
//G334: __E_cli_command_97(228) => help_sym(229) __E___E_cli_command_97_99(231).
	p(NT(228), (NT(229)+NT(231)));
//G335: help_cmd(227)        => __E_cli_command_97(228).
	p(NT(227), (NT(228)));
//G336: cli_command(215)     => help_cmd(227).
	p(NT(215), (NT(227)));
//G337: file_sym(235)        => 'f' 'i' 'l' 'e'.
	p(NT(235), (T(42)+T(12)+T(16)+T(10)));
//G338: __E_cli_command_100(234) => file_sym(235) __(52) q_string(236).
	p(NT(234), (NT(235)+NT(52)+NT(236)));
//G339: file_cmd(233)        => __E_cli_command_100(234).
	p(NT(233), (NT(234)));
//G340: cli_command(215)     => file_cmd(233).
	p(NT(215), (NT(233)));
//G341: valid_sym(239)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(239), (T(51)+T(15)+T(16)+T(12)+T(55)));
//G342: __E_cli_command_101(238) => valid_sym(239) __(52) rr(10).
	p(NT(238), (NT(239)+NT(52)+NT(10)));
//G343: valid_cmd(237)       => __E_cli_command_101(238).
	p(NT(237), (NT(238)));
//G344: cli_command(215)     => valid_cmd(237).
	p(NT(215), (NT(237)));
//G345: sat_sym(242)         => 's' 'a' 't'.
	p(NT(242), (T(7)+T(15)+T(11)));
//G346: __E_cli_command_102(241) => sat_sym(242) __(52) normalize_cmd_arg(243).
	p(NT(241), (NT(242)+NT(52)+NT(243)));
//G347: sat_cmd(240)         => __E_cli_command_102(241).
	p(NT(240), (NT(241)));
//G348: cli_command(215)     => sat_cmd(240).
	p(NT(215), (NT(240)));
//G349: unsat_sym(246)       => 'u' 'n' 's' 'a' 't'.
	p(NT(246), (T(48)+T(46)+T(7)+T(15)+T(11)));
//G350: __E_cli_command_103(245) => unsat_sym(246) __(52) rr(10).
	p(NT(245), (NT(246)+NT(52)+NT(10)));
//G351: unsat_cmd(244)       => __E_cli_command_103(245).
	p(NT(244), (NT(245)));
//G352: cli_command(215)     => unsat_cmd(244).
	p(NT(215), (NT(244)));
//G353: solve_sym(249)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(249), (T(7)+T(8)+T(16)+T(51)+T(10)));
//G354: __E___E_cli_command_104_105(250) => __(52) type(171).
	p(NT(250), (NT(52)+NT(171)));
//G355: __E___E_cli_command_104_105(250) => null.
	p(NT(250), (nul));
//G356: __E_cli_command_104(248) => solve_sym(249) __E___E_cli_command_104_105(250) __(52) solve_cmd_arg(251).
	p(NT(248), (NT(249)+NT(250)+NT(52)+NT(251)));
//G357: solve_cmd(247)       => __E_cli_command_104(248).
	p(NT(247), (NT(248)));
//G358: cli_command(215)     => solve_cmd(247).
	p(NT(215), (NT(247)));
//G359: __E___E_cli_command_106_107(255) => 'r'.
	p(NT(255), (T(50)));
//G360: __E___E_cli_command_106_107(255) => 'r' 'u' 'n'.
	p(NT(255), (T(50)+T(48)+T(46)));
//G361: run_sym(254)         => __E___E_cli_command_106_107(255).
	p(NT(254), (NT(255)));
//G362: __E___E_cli_command_106_108(256) => __(52) num(127).
	p(NT(256), (NT(52)+NT(127)));
//G363: __E___E_cli_command_106_108(256) => null.
	p(NT(256), (nul));
//G364: __E_cli_command_106(253) => run_sym(254) __(52) wff(14) __E___E_cli_command_106_108(256).
	p(NT(253), (NT(254)+NT(52)+NT(14)+NT(256)));
//G365: run_cmd(252)         => __E_cli_command_106(253).
	p(NT(252), (NT(253)));
//G366: cli_command(215)     => run_cmd(252).
	p(NT(215), (NT(252)));
//G367: __E___E_cli_command_109_110(260) => 'n'.
	p(NT(260), (T(46)));
//G368: __E___E_cli_command_109_110(260) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(260), (T(46)+T(8)+T(50)+T(9)+T(15)+T(16)+T(12)+T(52)+T(10)));
//G369: normalize_sym(259)   => __E___E_cli_command_109_110(260).
	p(NT(259), (NT(260)));
//G370: __E_cli_command_109(258) => normalize_sym(259) __(52) normalize_cmd_arg(243).
	p(NT(258), (NT(259)+NT(52)+NT(243)));
//G371: normalize_cmd(257)   => __E_cli_command_109(258).
	p(NT(257), (NT(258)));
//G372: cli_command(215)     => normalize_cmd(257).
	p(NT(215), (NT(257)));
//G373: __E___E_cli_command_111_112(264) => 's'.
	p(NT(264), (T(7)));
//G374: __E___E_cli_command_111_112(264) => 's' 'u' 'b' 's' 't'.
	p(NT(264), (T(7)+T(48)+T(49)+T(7)+T(11)));
//G375: __E___E_cli_command_111_112(264) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(264), (T(7)+T(48)+T(49)+T(7)+T(11)+T(12)+T(11)+T(48)+T(11)+T(10)));
//G376: subst_sym(263)       => __E___E_cli_command_111_112(264).
	p(NT(263), (NT(264)));
//G377: __E_cli_command_111(262) => subst_sym(263) __(52) nf_cmd_arg(265) _(11) '[' _(11) nf_cmd_arg(265) _(11) '/' _(11) nf_cmd_arg(265) _(11) ']'.
	p(NT(262), (NT(263)+NT(52)+NT(265)+NT(11)+T(19)+NT(11)+NT(265)+NT(11)+T(56)+NT(11)+NT(265)+NT(11)+T(20)));
//G378: subst_cmd(261)       => __E_cli_command_111(262).
	p(NT(261), (NT(262)));
//G379: cli_command(215)     => subst_cmd(261).
	p(NT(215), (NT(261)));
//G380: __E___E_cli_command_113_114(269) => 'i'.
	p(NT(269), (T(12)));
//G381: __E___E_cli_command_113_114(269) => 'i' 'n' 's' 't'.
	p(NT(269), (T(12)+T(46)+T(7)+T(11)));
//G382: __E___E_cli_command_113_114(269) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(269), (T(12)+T(46)+T(7)+T(11)+T(15)+T(46)+T(11)+T(12)+T(15)+T(11)+T(10)));
//G383: inst_sym(268)        => __E___E_cli_command_113_114(269).
	p(NT(268), (NT(269)));
//G384: __E_cli_command_113(267) => inst_sym(268) __(52) inst_args(270).
	p(NT(267), (NT(268)+NT(52)+NT(270)));
//G385: inst_cmd(266)        => __E_cli_command_113(267).
	p(NT(266), (NT(267)));
//G386: cli_command(215)     => inst_cmd(266).
	p(NT(215), (NT(266)));
//G387: dnf_sym(273)         => 'd' 'n' 'f'.
	p(NT(273), (T(55)+T(46)+T(42)));
//G388: __E_cli_command_115(272) => dnf_sym(273) __(52) nf_cmd_arg(265).
	p(NT(272), (NT(273)+NT(52)+NT(265)));
//G389: dnf_cmd(271)         => __E_cli_command_115(272).
	p(NT(271), (NT(272)));
//G390: cli_command(215)     => dnf_cmd(271).
	p(NT(215), (NT(271)));
//G391: cnf_sym(276)         => 'c' 'n' 'f'.
	p(NT(276), (T(45)+T(46)+T(42)));
//G392: __E_cli_command_116(275) => cnf_sym(276) __(52) nf_cmd_arg(265).
	p(NT(275), (NT(276)+NT(52)+NT(265)));
//G393: cnf_cmd(274)         => __E_cli_command_116(275).
	p(NT(274), (NT(275)));
//G394: cli_command(215)     => cnf_cmd(274).
	p(NT(215), (NT(274)));
//G395: anf_sym(279)         => 'a' 'n' 'f'.
	p(NT(279), (T(15)+T(46)+T(42)));
//G396: __E_cli_command_117(278) => anf_sym(279) __(52) nf_cmd_arg(265).
	p(NT(278), (NT(279)+NT(52)+NT(265)));
//G397: anf_cmd(277)         => __E_cli_command_117(278).
	p(NT(277), (NT(278)));
//G398: cli_command(215)     => anf_cmd(277).
	p(NT(215), (NT(277)));
//G399: nnf_sym(282)         => 'n' 'n' 'f'.
	p(NT(282), (T(46)+T(46)+T(42)));
//G400: __E_cli_command_118(281) => nnf_sym(282) __(52) nf_cmd_arg(265).
	p(NT(281), (NT(282)+NT(52)+NT(265)));
//G401: nnf_cmd(280)         => __E_cli_command_118(281).
	p(NT(280), (NT(281)));
//G402: cli_command(215)     => nnf_cmd(280).
	p(NT(215), (NT(280)));
//G403: pnf_sym(285)         => 'p' 'n' 'f'.
	p(NT(285), (T(54)+T(46)+T(42)));
//G404: __E_cli_command_119(284) => pnf_sym(285) __(52) nf_cmd_arg(265).
	p(NT(284), (NT(285)+NT(52)+NT(265)));
//G405: pnf_cmd(283)         => __E_cli_command_119(284).
	p(NT(283), (NT(284)));
//G406: cli_command(215)     => pnf_cmd(283).
	p(NT(215), (NT(283)));
//G407: mnf_sym(288)         => 'm' 'n' 'f'.
	p(NT(288), (T(9)+T(46)+T(42)));
//G408: __E_cli_command_120(287) => mnf_sym(288) __(52) nf_cmd_arg(265).
	p(NT(287), (NT(288)+NT(52)+NT(265)));
//G409: mnf_cmd(286)         => __E_cli_command_120(287).
	p(NT(286), (NT(287)));
//G410: cli_command(215)     => mnf_cmd(286).
	p(NT(215), (NT(286)));
//G411: snf_sym(291)         => 's' 'n' 'f'.
	p(NT(291), (T(7)+T(46)+T(42)));
//G412: __E_cli_command_121(290) => snf_sym(291) __(52) nf_cmd_arg(265).
	p(NT(290), (NT(291)+NT(52)+NT(265)));
//G413: snf_cmd(289)         => __E_cli_command_121(290).
	p(NT(289), (NT(290)));
//G414: cli_command(215)     => snf_cmd(289).
	p(NT(215), (NT(289)));
//G415: onf_sym(294)         => 'o' 'n' 'f'.
	p(NT(294), (T(8)+T(46)+T(42)));
//G416: __E_cli_command_122(293) => onf_sym(294) __(52) variable(105) __(52) onf_cmd_arg(295).
	p(NT(293), (NT(294)+NT(52)+NT(105)+NT(52)+NT(295)));
//G417: onf_cmd(292)         => __E_cli_command_122(293).
	p(NT(292), (NT(293)));
//G418: cli_command(215)     => onf_cmd(292).
	p(NT(215), (NT(292)));
//G419: __E___E_cli_command_123_124(299) => 'd' 'e' 'f' 's'.
	p(NT(299), (T(55)+T(10)+T(42)+T(7)));
//G420: __E___E_cli_command_123_124(299) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(299), (T(55)+T(10)+T(42)+T(12)+T(46)+T(12)+T(11)+T(12)+T(8)+T(46)+T(7)));
//G421: def_sym(298)         => __E___E_cli_command_123_124(299).
	p(NT(298), (NT(299)));
//G422: __E_cli_command_123(297) => def_sym(298).
	p(NT(297), (NT(298)));
//G423: def_list_cmd(296)    => __E_cli_command_123(297).
	p(NT(296), (NT(297)));
//G424: cli_command(215)     => def_list_cmd(296).
	p(NT(215), (NT(296)));
//G425: __E_cli_command_125(301) => def_sym(298) __(52) number(302).
	p(NT(301), (NT(298)+NT(52)+NT(302)));
//G426: def_print_cmd(300)   => __E_cli_command_125(301).
	p(NT(300), (NT(301)));
//G427: cli_command(215)     => def_print_cmd(300).
	p(NT(215), (NT(300)));
//G428: def_rr_cmd(303)      => rec_relation(16).
	p(NT(303), (NT(16)));
//G429: cli_command(215)     => def_rr_cmd(303).
	p(NT(215), (NT(303)));
//G430: qelim_sym(306)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(306), (T(53)+T(10)+T(16)+T(12)+T(9)));
//G431: __E_cli_command_126(305) => qelim_sym(306) __(52) wff_cmd_arg(307).
	p(NT(305), (NT(306)+NT(52)+NT(307)));
//G432: qelim_cmd(304)       => __E_cli_command_126(305).
	p(NT(304), (NT(305)));
//G433: cli_command(215)     => qelim_cmd(304).
	p(NT(215), (NT(304)));
//G434: get_sym(310)         => 'g' 'e' 't'.
	p(NT(310), (T(47)+T(10)+T(11)));
//G435: __E___E_cli_command_127_128(311) => __(52) option(312).
	p(NT(311), (NT(52)+NT(312)));
//G436: __E___E_cli_command_127_128(311) => null.
	p(NT(311), (nul));
//G437: __E_cli_command_127(309) => get_sym(310) __E___E_cli_command_127_128(311).
	p(NT(309), (NT(310)+NT(311)));
//G438: get_cmd(308)         => __E_cli_command_127(309).
	p(NT(308), (NT(309)));
//G439: cli_command(215)     => get_cmd(308).
	p(NT(215), (NT(308)));
//G440: set_sym(315)         => 's' 'e' 't'.
	p(NT(315), (T(7)+T(10)+T(11)));
//G441: __E___E_cli_command_129_130(316) => __(52).
	p(NT(316), (NT(52)));
//G442: __E___E_cli_command_129_130(316) => _(11) '=' _(11).
	p(NT(316), (NT(11)+T(3)+NT(11)));
//G443: __E_cli_command_129(314) => set_sym(315) __(52) option(312) __E___E_cli_command_129_130(316) option_value(317).
	p(NT(314), (NT(315)+NT(52)+NT(312)+NT(316)+NT(317)));
//G444: set_cmd(313)         => __E_cli_command_129(314).
	p(NT(313), (NT(314)));
//G445: cli_command(215)     => set_cmd(313).
	p(NT(215), (NT(313)));
//G446: toggle_sym(320)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(320), (T(11)+T(8)+T(47)+T(47)+T(16)+T(10)));
//G447: __E_cli_command_131(319) => toggle_sym(320) __(52) bool_option(321).
	p(NT(319), (NT(320)+NT(52)+NT(321)));
//G448: toggle_cmd(318)      => __E_cli_command_131(319).
	p(NT(318), (NT(319)));
//G449: cli_command(215)     => toggle_cmd(318).
	p(NT(215), (NT(318)));
//G450: __E_cli_command_132(323) => type(171) __(52) in_var_name(145) _(11) '=' _(11) input_stream(324).
	p(NT(323), (NT(171)+NT(52)+NT(145)+NT(11)+T(3)+NT(11)+NT(324)));
//G451: def_input_cmd(322)   => __E_cli_command_132(323).
	p(NT(322), (NT(323)));
//G452: cli_command(215)     => def_input_cmd(322).
	p(NT(215), (NT(322)));
//G453: __E_cli_command_133(326) => type(171) __(52) out_var_name(150) _(11) '=' _(11) output_stream(327).
	p(NT(326), (NT(171)+NT(52)+NT(150)+NT(11)+T(3)+NT(11)+NT(327)));
//G454: def_output_cmd(325)  => __E_cli_command_133(326).
	p(NT(325), (NT(326)));
//G455: cli_command(215)     => def_output_cmd(325).
	p(NT(215), (NT(325)));
//G456: __E___E_cli_command_134_135(331) => 'h' 'i' 's' 't'.
	p(NT(331), (T(44)+T(12)+T(7)+T(11)));
//G457: __E___E_cli_command_134_135(331) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(331), (T(44)+T(12)+T(7)+T(11)+T(8)+T(50)+T(18)));
//G458: history_sym(330)     => __E___E_cli_command_134_135(331).
	p(NT(330), (NT(331)));
//G459: __E_cli_command_134(329) => history_sym(330).
	p(NT(329), (NT(330)));
//G460: history_list_cmd(328) => __E_cli_command_134(329).
	p(NT(328), (NT(329)));
//G461: cli_command(215)     => history_list_cmd(328).
	p(NT(215), (NT(328)));
//G462: __E_cli_command_136(333) => history_sym(330) __(52) memory(334).
	p(NT(333), (NT(330)+NT(52)+NT(334)));
//G463: history_print_cmd(332) => __E_cli_command_136(333).
	p(NT(332), (NT(333)));
//G464: cli_command(215)     => history_print_cmd(332).
	p(NT(215), (NT(332)));
//G465: __E_cli_command_137(336) => wff(14).
	p(NT(336), (NT(14)));
//G466: __E_cli_command_137(336) => bf(21).
	p(NT(336), (NT(21)));
//G467: history_store_cmd(335) => __E_cli_command_137(336).
	p(NT(335), (NT(336)));
//G468: cli_command(215)     => history_store_cmd(335).
	p(NT(215), (NT(335)));
//G469: __E_input_stream_138(337) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(338) _(11) ')'.
	p(NT(337), (T(12)+T(42)+T(12)+T(16)+T(10)+NT(11)+T(5)+NT(11)+NT(338)+NT(11)+T(6)));
//G470: input_stream(324)    => __E_input_stream_138(337).
	p(NT(324), (NT(337)));
//G471: __E_input_stream_139(340) => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(340), (T(45)+T(8)+T(46)+T(7)+T(8)+T(16)+T(10)));
//G472: console_sym(339)     => __E_input_stream_139(340).
	p(NT(339), (NT(340)));
//G473: input_stream(324)    => console_sym(339).
	p(NT(324), (NT(339)));
//G474: __E_output_stream_140(341) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(338) _(11) ')'.
	p(NT(341), (T(8)+T(42)+T(12)+T(16)+T(10)+NT(11)+T(5)+NT(11)+NT(338)+NT(11)+T(6)));
//G475: output_stream(327)   => __E_output_stream_140(341).
	p(NT(327), (NT(341)));
//G476: __E_output_stream_141(342) => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(342), (T(45)+T(8)+T(46)+T(7)+T(8)+T(16)+T(10)));
//G477: console_sym(339)     => __E_output_stream_141(342).
	p(NT(339), (NT(342)));
//G478: output_stream(327)   => console_sym(339).
	p(NT(327), (NT(339)));
//G479: q_file_name(338)     => '"' file_name(343) '"'.
	p(NT(338), (T(57)+NT(343)+T(57)));
//G480: __E_file_name_142(344) => printable(8).
	p(NT(344), (NT(8)));
//G481: __E_file_name_142(344) => printable(8) __E_file_name_142(344).
	p(NT(344), (NT(8)+NT(344)));
//G482: file_name(343)       => __E_file_name_142(344).
	p(NT(343), (NT(344)));
//G483: number(302)          => digits(163).
	p(NT(302), (NT(163)));
//G484: bf_cmd_arg(345)      => memory(334).
	p(NT(345), (NT(334)));
//G485: bf_cmd_arg(345)      => bf(21).
	p(NT(345), (NT(21)));
//G486: wff_cmd_arg(307)     => memory(334).
	p(NT(307), (NT(334)));
//G487: wff_cmd_arg(307)     => wff(14).
	p(NT(307), (NT(14)));
//G488: solve_cmd_arg(251)   => memory(334).
	p(NT(251), (NT(334)));
//G489: solve_cmd_arg(251)   => wff(14).
	p(NT(251), (NT(14)));
//G490: nf_cmd_arg(265)      => memory(334).
	p(NT(265), (NT(334)));
//G491: nf_cmd_arg(265)      => wff(14).
	p(NT(265), (NT(14)));
//G492: nf_cmd_arg(265)      => bf(21).
	p(NT(265), (NT(21)));
//G493: onf_cmd_arg(295)     => memory(334).
	p(NT(295), (NT(334)));
//G494: onf_cmd_arg(295)     => wff(14).
	p(NT(295), (NT(14)));
//G495: normalize_cmd_arg(243) => memory(334).
	p(NT(243), (NT(334)));
//G496: normalize_cmd_arg(243) => rr(10).
	p(NT(243), (NT(10)));
//G497: normalize_cmd_arg(243) => ref(18).
	p(NT(243), (NT(18)));
//G498: normalize_cmd_arg(243) => wff(14).
	p(NT(243), (NT(14)));
//G499: normalize_cmd_arg(243) => bf(21).
	p(NT(243), (NT(21)));
//G500: inst_args(270)       => wff_cmd_arg(307) _(11) '[' _(11) variable(105) _(11) '/' _(11) bf_cmd_arg(345) _(11) ']'.
	p(NT(270), (NT(307)+NT(11)+T(19)+NT(11)+NT(105)+NT(11)+T(56)+NT(11)+NT(345)+NT(11)+T(20)));
//G501: inst_args(270)       => bf_cmd_arg(345) _(11) '[' _(11) variable(105) _(11) '/' _(11) bf_cmd_arg(345) _(11) ']'.
	p(NT(270), (NT(345)+NT(11)+T(19)+NT(11)+NT(105)+NT(11)+T(56)+NT(11)+NT(345)+NT(11)+T(20)));
//G502: help_arg(232)        => help_sym(229).
	p(NT(232), (NT(229)));
//G503: help_arg(232)        => version_sym(222).
	p(NT(232), (NT(222)));
//G504: help_arg(232)        => quit_sym(219).
	p(NT(232), (NT(219)));
//G505: help_arg(232)        => clear_sym(225).
	p(NT(232), (NT(225)));
//G506: help_arg(232)        => get_sym(310).
	p(NT(232), (NT(310)));
//G507: help_arg(232)        => set_sym(315).
	p(NT(232), (NT(315)));
//G508: help_arg(232)        => toggle_sym(320).
	p(NT(232), (NT(320)));
//G509: help_arg(232)        => file_sym(235).
	p(NT(232), (NT(235)));
//G510: help_arg(232)        => history_sym(330).
	p(NT(232), (NT(330)));
//G511: help_arg(232)        => abs_memory_sym(346).
	p(NT(232), (NT(346)));
//G512: help_arg(232)        => rel_memory_sym(347).
	p(NT(232), (NT(347)));
//G513: help_arg(232)        => selection_sym(348).
	p(NT(232), (NT(348)));
//G514: help_arg(232)        => def_sym(298).
	p(NT(232), (NT(298)));
//G515: help_arg(232)        => inst_sym(268).
	p(NT(232), (NT(268)));
//G516: help_arg(232)        => subst_sym(263).
	p(NT(232), (NT(263)));
//G517: help_arg(232)        => normalize_sym(259).
	p(NT(232), (NT(259)));
//G518: help_arg(232)        => execute_sym(349).
	p(NT(232), (NT(349)));
//G519: help_arg(232)        => solve_sym(249).
	p(NT(232), (NT(249)));
//G520: help_arg(232)        => valid_sym(239).
	p(NT(232), (NT(239)));
//G521: help_arg(232)        => sat_sym(242).
	p(NT(232), (NT(242)));
//G522: help_arg(232)        => unsat_sym(246).
	p(NT(232), (NT(246)));
//G523: help_arg(232)        => dnf_sym(273).
	p(NT(232), (NT(273)));
//G524: help_arg(232)        => cnf_sym(276).
	p(NT(232), (NT(276)));
//G525: help_arg(232)        => anf_sym(279).
	p(NT(232), (NT(279)));
//G526: help_arg(232)        => snf_sym(291).
	p(NT(232), (NT(291)));
//G527: help_arg(232)        => nnf_sym(282).
	p(NT(232), (NT(282)));
//G528: help_arg(232)        => pnf_sym(285).
	p(NT(232), (NT(285)));
//G529: help_arg(232)        => mnf_sym(288).
	p(NT(232), (NT(288)));
//G530: help_arg(232)        => onf_sym(294).
	p(NT(232), (NT(294)));
//G531: help_arg(232)        => qelim_sym(306).
	p(NT(232), (NT(306)));
//G532: __E___E_help_arg_143_144(352) => 's'.
	p(NT(352), (T(7)));
//G533: __E___E_help_arg_143_144(352) => null.
	p(NT(352), (nul));
//G534: __E_help_arg_143(351) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_143_144(352).
	p(NT(351), (T(10)+T(22)+T(15)+T(9)+T(54)+T(16)+T(10)+NT(352)));
//G535: examples_sym(350)    => __E_help_arg_143(351).
	p(NT(350), (NT(351)));
//G536: help_arg(232)        => examples_sym(350).
	p(NT(232), (NT(350)));
//G537: __E_memory_145(354)  => '%'.
	p(NT(354), (T(58)));
//G538: rel_memory_sym(347)  => '%' '-'.
	p(NT(347), (T(58)+T(23)));
//G539: memory_id(356)       => digits(163).
	p(NT(356), (NT(163)));
//G540: __E___E_memory_145_146(355) => memory_id(356).
	p(NT(355), (NT(356)));
//G541: __E___E_memory_145_146(355) => null.
	p(NT(355), (nul));
//G542: __E_memory_145(354)  => rel_memory_sym(347) __E___E_memory_145_146(355).
	p(NT(354), (NT(347)+NT(355)));
//G543: rel_memory(353)      => __E_memory_145(354).
	p(NT(353), (NT(354)));
//G544: memory(334)          => rel_memory(353).
	p(NT(334), (NT(353)));
//G545: abs_memory_sym(346)  => '%'.
	p(NT(346), (T(58)));
//G546: __E_memory_147(358)  => abs_memory_sym(346) memory_id(356).
	p(NT(358), (NT(346)+NT(356)));
//G547: abs_memory(357)      => __E_memory_147(358).
	p(NT(357), (NT(358)));
//G548: memory(334)          => abs_memory(357).
	p(NT(334), (NT(357)));
//G549: option(312)          => bool_option(321).
	p(NT(312), (NT(321)));
//G550: __E_option_148(361)  => 's' 'e' 'v'.
	p(NT(361), (T(7)+T(10)+T(51)));
//G551: __E_option_148(361)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(361), (T(7)+T(10)+T(51)+T(10)+T(50)+T(12)+T(11)+T(18)));
//G552: severity_opt(360)    => __E_option_148(361).
	p(NT(360), (NT(361)));
//G553: enum_option(359)     => severity_opt(360).
	p(NT(359), (NT(360)));
//G554: option(312)          => enum_option(359).
	p(NT(312), (NT(359)));
//G555: __E_bool_option_149(363) => 's'.
	p(NT(363), (T(7)));
//G556: __E_bool_option_149(363) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(363), (T(7)+T(11)+T(15)+T(11)+T(48)+T(7)));
//G557: status_opt(362)      => __E_bool_option_149(363).
	p(NT(362), (NT(363)));
//G558: bool_option(321)     => status_opt(362).
	p(NT(321), (NT(362)));
//G559: __E_bool_option_150(365) => 'c'.
	p(NT(365), (T(45)));
//G560: __E_bool_option_150(365) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(365), (T(45)+T(8)+T(16)+T(8)+T(50)));
//G561: __E_bool_option_150(365) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(365), (T(45)+T(8)+T(16)+T(8)+T(50)+T(7)));
//G562: colors_opt(364)      => __E_bool_option_150(365).
	p(NT(364), (NT(365)));
//G563: bool_option(321)     => colors_opt(364).
	p(NT(321), (NT(364)));
//G564: __E_bool_option_151(367) => 'd'.
	p(NT(367), (T(55)));
//G565: __E_bool_option_151(367) => 'd' 'b' 'g'.
	p(NT(367), (T(55)+T(49)+T(47)));
//G566: __E_bool_option_151(367) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(367), (T(55)+T(10)+T(49)+T(48)+T(47)));
//G567: debug_repl_opt(366)  => __E_bool_option_151(367).
	p(NT(366), (NT(367)));
//G568: bool_option(321)     => debug_repl_opt(366).
	p(NT(321), (NT(366)));
//G569: option_value(317)    => option_value_true(368).
	p(NT(317), (NT(368)));
//G570: option_value(317)    => option_value_false(369).
	p(NT(317), (NT(369)));
//G571: option_value(317)    => severity(370).
	p(NT(317), (NT(370)));
//G572: option_value_true(368) => 't'.
	p(NT(368), (T(11)));
//G573: option_value_true(368) => 't' 'r' 'u' 'e'.
	p(NT(368), (T(11)+T(50)+T(48)+T(10)));
//G574: option_value_true(368) => 'o' 'n'.
	p(NT(368), (T(8)+T(46)));
//G575: option_value_true(368) => '1'.
	p(NT(368), (T(35)));
//G576: option_value_true(368) => 'y'.
	p(NT(368), (T(18)));
//G577: option_value_true(368) => 'y' 'e' 's'.
	p(NT(368), (T(18)+T(10)+T(7)));
//G578: option_value_false(369) => 'f'.
	p(NT(369), (T(42)));
//G579: option_value_false(369) => 'f' 'a' 'l' 's' 'e'.
	p(NT(369), (T(42)+T(15)+T(16)+T(7)+T(10)));
//G580: option_value_false(369) => 'o' 'f' 'f'.
	p(NT(369), (T(8)+T(42)+T(42)));
//G581: option_value_false(369) => '0'.
	p(NT(369), (T(36)));
//G582: option_value_false(369) => 'n'.
	p(NT(369), (T(46)));
//G583: option_value_false(369) => 'n' 'o'.
	p(NT(369), (T(46)+T(8)));
//G584: __E_severity_152(372) => 'e'.
	p(NT(372), (T(10)));
//G585: __E_severity_152(372) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(372), (T(10)+T(50)+T(50)+T(8)+T(50)));
//G586: error_sym(371)       => __E_severity_152(372).
	p(NT(371), (NT(372)));
//G587: severity(370)        => error_sym(371).
	p(NT(370), (NT(371)));
//G588: __E_severity_153(374) => 'i'.
	p(NT(374), (T(12)));
//G589: __E_severity_153(374) => 'i' 'n' 'f' 'o'.
	p(NT(374), (T(12)+T(46)+T(42)+T(8)));
//G590: info_sym(373)        => __E_severity_153(374).
	p(NT(373), (NT(374)));
//G591: severity(370)        => info_sym(373).
	p(NT(370), (NT(373)));
//G592: __E_severity_154(376) => 'd'.
	p(NT(376), (T(55)));
//G593: __E_severity_154(376) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(376), (T(55)+T(10)+T(49)+T(48)+T(47)));
//G594: debug_sym(375)       => __E_severity_154(376).
	p(NT(375), (NT(376)));
//G595: severity(370)        => debug_sym(375).
	p(NT(370), (NT(375)));
//G596: __E_severity_155(378) => 't'.
	p(NT(378), (T(11)));
//G597: __E_severity_155(378) => 't' 'r' 'a' 'c' 'e'.
	p(NT(378), (T(11)+T(50)+T(15)+T(45)+T(10)));
//G598: trace_sym(377)       => __E_severity_155(378).
	p(NT(377), (NT(378)));
//G599: severity(370)        => trace_sym(377).
	p(NT(370), (NT(377)));
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
		__E_wff_37, bf_less, __E_wff_38, bf_parenthesis, __E_bf_39, variable, bf_splitter, __E_bf_40, bf_ref, bf_or, 
		__E_bf_41, bf_xor, __E_bf_42, bf_and, __E_bf_43, __E___E_bf_43_44, bf_neg, __E_bf_45, bf_constant, __E_bf_46, 
		constant, bf_t, bf_f, uninterpreted_constant, ctn_neq, __E_constraint_47, ctnvar, num, ctn_eq, __E_constraint_48, 
		ctn_greater_equal, __E_constraint_49, ctn_greater, __E_constraint_50, ctn_less_equal, __E_constraint_51, ctn_less, __E_constraint_52, charvar, __E_charvar_53, 
		__E_charvar_54, io_var, __E_variable_55, in, out, in_var_name, __E_in_56, __E___E_in_56_57, __E___E___E_in_56_57_58, offset, 
		out_var_name, __E_out_59, __E___E_out_59_60, __E___E___E_out_59_60_61, q_var, __E_q_vars_62, __E_q_vars_63, __E_offsets_64, __E_offsets_65, shift, 
		__E_offset_66, __E_shift_67, __E___E_shift_67_68, digits, uninter_const_name, __E_uninterpreted_constant_69, __E___E_uninterpreted_constant_69_70, chars, binding, source_binding, 
		__E_binding_71, type, __E___E_binding_71_72, source, named_binding, src_c, __E_source_73, __E___E_source_73_74, __E___E_source_73_75, __E_src_c_76, 
		__E_src_c_77, __E_chars_78, __E_digits_79, extra, comment, __E_comment_80, __E_comment_81, __E_comment_82, __E____83, __E___84, 
		bf_cb_args1, bf_cb_arg, bf_cb_args2, bf_cb_args3, bf_cb_args4, wff_cb_args1, wff_cb_arg, wff_cb_args2, wff_cb_args3, wff_cb_args4, 
		wff_has_clashing_subformulas_cb, __E_wff_cb_85, wff_has_subformula_cb, __E_wff_cb_86, wff_remove_existential_cb, __E_wff_cb_87, bf_has_subformula_cb, __E_bf_cb_88, bf_remove_funiversal_cb, __E_bf_cb_89, 
		bf_remove_fexistential_cb, __E_bf_cb_90, bf_normalize_cb, __E_bf_cb_91, cli, cli_command, __E_cli_92, __E_cli_93, quit_cmd, quit_sym, 
		__E_cli_command_94, version_cmd, version_sym, __E_cli_command_95, clear_cmd, clear_sym, __E_cli_command_96, help_cmd, __E_cli_command_97, help_sym, 
		__E___E_cli_command_97_98, __E___E_cli_command_97_99, help_arg, file_cmd, __E_cli_command_100, file_sym, q_string, valid_cmd, __E_cli_command_101, valid_sym, 
		sat_cmd, __E_cli_command_102, sat_sym, normalize_cmd_arg, unsat_cmd, __E_cli_command_103, unsat_sym, solve_cmd, __E_cli_command_104, solve_sym, 
		__E___E_cli_command_104_105, solve_cmd_arg, run_cmd, __E_cli_command_106, run_sym, __E___E_cli_command_106_107, __E___E_cli_command_106_108, normalize_cmd, __E_cli_command_109, normalize_sym, 
		__E___E_cli_command_109_110, subst_cmd, __E_cli_command_111, subst_sym, __E___E_cli_command_111_112, nf_cmd_arg, inst_cmd, __E_cli_command_113, inst_sym, __E___E_cli_command_113_114, 
		inst_args, dnf_cmd, __E_cli_command_115, dnf_sym, cnf_cmd, __E_cli_command_116, cnf_sym, anf_cmd, __E_cli_command_117, anf_sym, 
		nnf_cmd, __E_cli_command_118, nnf_sym, pnf_cmd, __E_cli_command_119, pnf_sym, mnf_cmd, __E_cli_command_120, mnf_sym, snf_cmd, 
		__E_cli_command_121, snf_sym, onf_cmd, __E_cli_command_122, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_123, def_sym, __E___E_cli_command_123_124, 
		def_print_cmd, __E_cli_command_125, number, def_rr_cmd, qelim_cmd, __E_cli_command_126, qelim_sym, wff_cmd_arg, get_cmd, __E_cli_command_127, 
		get_sym, __E___E_cli_command_127_128, option, set_cmd, __E_cli_command_129, set_sym, __E___E_cli_command_129_130, option_value, toggle_cmd, __E_cli_command_131, 
		toggle_sym, bool_option, def_input_cmd, __E_cli_command_132, input_stream, def_output_cmd, __E_cli_command_133, output_stream, history_list_cmd, __E_cli_command_134, 
		history_sym, __E___E_cli_command_134_135, history_print_cmd, __E_cli_command_136, memory, history_store_cmd, __E_cli_command_137, __E_input_stream_138, q_file_name, console_sym, 
		__E_input_stream_139, __E_output_stream_140, __E_output_stream_141, file_name, __E_file_name_142, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, execute_sym, 
		examples_sym, __E_help_arg_143, __E___E_help_arg_143_144, rel_memory, __E_memory_145, __E___E_memory_145_146, memory_id, abs_memory, __E_memory_147, enum_option, 
		severity_opt, __E_option_148, status_opt, __E_bool_option_149, colors_opt, __E_bool_option_150, debug_repl_opt, __E_bool_option_151, option_value_true, option_value_false, 
		severity, error_sym, __E_severity_152, info_sym, __E_severity_153, debug_sym, __E_severity_154, trace_sym, __E_severity_155, __N_0, 
		__N_1, __N_2, __N_3, __N_4, __N_5, 
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
