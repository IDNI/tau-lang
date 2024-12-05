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
	"rule", "__E___E___E_library_9_10_11", "wff_rule", "bf_rule", "__E___E_library_9_12", "wff_matcher", "wff_body", "__E_wff_rule_13", "bf_matcher", "bf_body", 
	"__E_bf_rule_14", "builder", "builder_head", "builder_body", "__E_builder_head_15", "__E_builder_head_16", "bf_builder_body", "__E_builder_body_17", "wff_builder_body", "__E_builder_body_18", 
	"tau_constant_source", "__E_tau_constant_source_19", "wff_parenthesis", "__E_wff_20", "wff_sometimes", "__E_wff_21", "__E___E_wff_21_22", "wff_always", "__E_wff_23", "__E___E_wff_23_24", 
	"wff_conditional", "__E_wff_25", "wff_all", "__E_wff_26", "q_vars", "wff_ex", "__E_wff_27", "wff_ref", "constraint", "wff_imply", 
	"__E_wff_28", "wff_equiv", "__E_wff_29", "wff_or", "__E_wff_30", "wff_xor", "__E_wff_31", "wff_and", "__E_wff_32", "wff_neg", 
	"__E_wff_33", "wff_t", "wff_f", "bf_interval", "__E_wff_34", "bf_eq", "__E_wff_35", "bf_neq", "__E_wff_36", "bf_less_equal", 
	"__E_wff_37", "bf_nleq", "__E_wff_38", "bf_greater", "__E_wff_39", "bf_ngreater", "__E_wff_40", "bf_greater_equal", "__E_wff_41", "bf_ngeq", 
	"__E_wff_42", "bf_less", "__E_wff_43", "bf_nless", "__E_wff_44", "bf_parenthesis", "__E_bf_45", "bf_constant", "variable", "bf_splitter", 
	"__E_bf_46", "bf_ref", "bf_or", "__E_bf_47", "bf_xor", "__E_bf_48", "bf_t", "__E_bf_49", "__E___E_bf_49_50", "type", 
	"bf_f", "__E_bf_51", "__E___E_bf_51_52", "bf_and", "__E_bf_53", "__E___E_bf_53_54", "bf_and_nosep", "bf_neg", "__E_bf_55", "__E_bf_and_nosep_56", 
	"ctn_neq", "__E_constraint_57", "ctnvar", "num", "ctn_eq", "__E_constraint_58", "ctn_greater_equal", "__E_constraint_59", "ctn_greater", "__E_constraint_60", 
	"ctn_less_equal", "__E_constraint_61", "ctn_less", "__E_constraint_62", "__E_variable_63", "chars", "io_var", "__E_variable_64", "in", "out", 
	"uninterpreted_constant", "in_var_name", "__E_in_65", "__E___E_in_65_66", "offset", "out_var_name", "__E_out_67", "__E___E_out_67_68", "q_var", "__E_q_vars_69", 
	"__E_q_vars_70", "__E_offsets_71", "__E_offsets_72", "shift", "__E_offset_73", "__E_shift_74", "__E___E_shift_74_75", "digits", "uninter_const_name", "__E_uninterpreted_constant_76", 
	"__E___E_uninterpreted_constant_76_77", "constant", "__E_bf_constant_78", "binding", "__E_bf_constant_79", "source", "__E_binding_80", "src_c", "__E___E_binding_80_81", "__E___E___E_binding_80_81_82", 
	"__E___E___E_binding_80_81_83", "__E_src_c_84", "__E_src_c_85", "__E_chars_86", "__E_chars_87", "__E_digits_88", "extra", "comment", "__E_comment_89", "__E_comment_90", 
	"__E_comment_91", "__E____92", "__E___93", "cli", "cli_command", "__E_cli_94", "__E_cli_95", "quit_cmd", "quit_sym", "__E_cli_command_96", 
	"version_cmd", "version_sym", "__E_cli_command_97", "clear_cmd", "clear_sym", "__E_cli_command_98", "help_cmd", "__E_cli_command_99", "help_sym", "__E___E_cli_command_99_100", 
	"__E___E_cli_command_99_101", "help_arg", "file_cmd", "__E_cli_command_102", "file_sym", "q_string", "valid_cmd", "__E_cli_command_103", "valid_sym", "normalize_cmd_arg", 
	"sat_cmd", "__E_cli_command_104", "sat_sym", "unsat_cmd", "__E_cli_command_105", "unsat_sym", "solve_cmd", "__E_cli_command_106", "solve_sym", "__E___E_cli_command_106_107", 
	"solve_cmd_arg", "run_cmd", "__E_cli_command_108", "run_sym", "__E___E_cli_command_108_109", "__E___E_cli_command_108_110", "memory", "normalize_cmd", "__E_cli_command_111", "normalize_sym", 
	"__E___E_cli_command_111_112", "subst_cmd", "__E_cli_command_113", "subst_sym", "__E___E_cli_command_113_114", "nf_cmd_arg", "inst_cmd", "__E_cli_command_115", "inst_sym", "__E___E_cli_command_115_116", 
	"inst_args", "dnf_cmd", "__E_cli_command_117", "dnf_sym", "cnf_cmd", "__E_cli_command_118", "cnf_sym", "anf_cmd", "__E_cli_command_119", "anf_sym", 
	"nnf_cmd", "__E_cli_command_120", "nnf_sym", "pnf_cmd", "__E_cli_command_121", "pnf_sym", "mnf_cmd", "__E_cli_command_122", "mnf_sym", "snf_cmd", 
	"__E_cli_command_123", "snf_sym", "onf_cmd", "__E_cli_command_124", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_125", "def_sym", "__E___E_cli_command_125_126", 
	"def_print_cmd", "__E_cli_command_127", "number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_128", "qelim_sym", "wff_cmd_arg", "get_cmd", "__E_cli_command_129", 
	"get_sym", "__E___E_cli_command_129_130", "option", "set_cmd", "__E_cli_command_131", "set_sym", "__E___E_cli_command_131_132", "option_value", "enable_cmd", "__E_cli_command_133", 
	"enable_sym", "bool_option", "disable_cmd", "__E_cli_command_134", "disable_sym", "toggle_cmd", "__E_cli_command_135", "toggle_sym", "def_input_cmd", "__E_cli_command_136", 
	"input_stream", "def_output_cmd", "__E_cli_command_137", "output_stream", "history_list_cmd", "__E_cli_command_138", "history_sym", "__E___E_cli_command_138_139", "history_print_cmd", "__E_cli_command_140", 
	"history_store_cmd", "__E_cli_command_141", "__E_input_stream_142", "q_file_name", "console_sym", "__E_output_stream_143", "file_name", "__E_file_name_144", "bf_cmd_arg", "abs_memory_sym", 
	"rel_memory_sym", "selection_sym", "execute_sym", "examples_sym", "__E_help_arg_145", "__E___E_help_arg_145_146", "rel_memory", "__E_memory_147", "__E___E_memory_147_148", "memory_id", 
	"abs_memory", "__E_memory_149", "enum_option", "severity_opt", "__E_option_150", "status_opt", "__E_bool_option_151", "colors_opt", "__E_bool_option_152", "charvar_opt", 
	"__E_bool_option_153", "highlighting_opt", "__E_bool_option_154", "indenting_opt", "__E_bool_option_155", "debug_repl_opt", "__E_bool_option_156", "option_value_true", "option_value_false", "severity", 
	"error_sym", "__E_severity_157", "info_sym", "__E_severity_158", "debug_sym", "__E_severity_159", "trace_sym", "__E_severity_160", "named_binding", "__N_0", 
	"__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'f', 'a', 'l', 'b', 'c', 
	'k', 'i', 'r', 's', 't', ',', '(', ')', 'o', 'm', 
	'e', '<', '>', 'w', 'y', '[', ']', '?', 'x', '-', 
	'|', '^', '&', '!', 'T', 'F', 'S', '+', '1', '0', 
	'\'', '$', '{', '}', '_', '\t', '\n', '\r', '#', 'q', 
	'u', 'v', 'n', 'h', 'p', 'd', 'z', '/', 'g', '"', 
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
			20, 22, 118, 129, 142, 161, 165, 177, 178, 185,
			336, 378
		},
		.to_inline = {
			{ 14, 62, 14 },
			{ 21, 115, 21 },
			{ 136 },
			{ 155 },
			{ 168 },
			{ 187 },
			{ 229 },
			{ 240 },
			{ 255 },
			{ 260 },
			{ 285 },
			{ 297 },
			{ 338 }
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
//G44:  wff_body(46)         => __E_wff_rule_13(47).
	p(NT(46), (NT(47)));
//G45:  wff_rule(42)         => wff_matcher(45) _(11) ':' ':' '=' _(11) wff_body(46) _(11) '.'.
	p(NT(42), (NT(45)+NT(11)+T(2)+T(2)+T(3)+NT(11)+NT(46)+NT(11)+T(1)));
//G46:  bf_matcher(48)       => bf(21).
	p(NT(48), (NT(21)));
//G47:  __E_bf_rule_14(50)   => bf(21).
	p(NT(50), (NT(21)));
//G48:  bf_body(49)          => __E_bf_rule_14(50).
	p(NT(49), (NT(50)));
//G49:  bf_rule(43)          => bf_matcher(48) _(11) ':' '=' _(11) bf_body(49) _(11) '.'.
	p(NT(43), (NT(48)+NT(11)+T(2)+T(3)+NT(11)+NT(49)+NT(11)+T(1)));
//G50:  builder(51)          => _(11) builder_head(52) _(11) builder_body(53) _(11) '.'.
	p(NT(51), (NT(11)+NT(52)+NT(11)+NT(53)+NT(11)+T(1)));
//G51:  __E_builder_head_15(54) => __(27) capture(20).
	p(NT(54), (NT(27)+NT(20)));
//G52:  __E_builder_head_16(55) => null.
	p(NT(55), (nul));
//G53:  __E_builder_head_16(55) => __E_builder_head_15(54) __E_builder_head_16(55).
	p(NT(55), (NT(54)+NT(55)));
//G54:  builder_head(52)     => '(' _(11) capture(20) __E_builder_head_16(55) _(11) ')'.
	p(NT(52), (T(15)+NT(11)+NT(20)+NT(55)+NT(11)+T(16)));
//G55:  __E_builder_body_17(57) => '=' ':' _(11) bf(21).
	p(NT(57), (T(3)+T(2)+NT(11)+NT(21)));
//G56:  bf_builder_body(56)  => __E_builder_body_17(57).
	p(NT(56), (NT(57)));
//G57:  builder_body(53)     => bf_builder_body(56).
	p(NT(53), (NT(56)));
//G58:  __E_builder_body_18(59) => '=' ':' ':' _(11) wff(14).
	p(NT(59), (T(3)+T(2)+T(2)+NT(11)+NT(14)));
//G59:  wff_builder_body(58) => __E_builder_body_18(59).
	p(NT(58), (NT(59)));
//G60:  builder_body(53)     => wff_builder_body(58).
	p(NT(53), (NT(58)));
//G61:  __E_tau_constant_source_19(61) => '.' _(11).
	p(NT(61), (T(1)+NT(11)));
//G62:  __E_tau_constant_source_19(61) => null.
	p(NT(61), (nul));
//G63:  tau_constant_source(60) => rec_relations(12) _(11) main(13) _(11) __E_tau_constant_source_19(61).
	p(NT(60), (NT(12)+NT(11)+NT(13)+NT(11)+NT(61)));
//G64:  __E_wff_20(63)       => '(' _(11) wff(14) _(11) ')'.
	p(NT(63), (T(15)+NT(11)+NT(14)+NT(11)+T(16)));
//G65:  wff_parenthesis(62)  => __E_wff_20(63).
	p(NT(62), (NT(63)));
//G66:  wff(14)              => wff_parenthesis(62).
	p(NT(14), (NT(62)));
//G67:  __E___E_wff_21_22(66) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(66), (T(12)+T(17)+T(18)+T(19)+T(13)+T(10)+T(18)+T(19)+T(12)));
//G68:  __E___E_wff_21_22(66) => '<' '>'.
	p(NT(66), (T(20)+T(21)));
//G69:  __E_wff_21(65)       => __E___E_wff_21_22(66) _(11) wff(14).
	p(NT(65), (NT(66)+NT(11)+NT(14)));
//G70:  wff_sometimes(64)    => __E_wff_21(65).
	p(NT(64), (NT(65)));
//G71:  wff(14)              => wff_sometimes(64).
	p(NT(14), (NT(64)));
//G72:  __E___E_wff_23_24(69) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(69), (T(5)+T(6)+T(22)+T(5)+T(23)+T(12)));
//G73:  __E___E_wff_23_24(69) => '[' ']'.
	p(NT(69), (T(24)+T(25)));
//G74:  __E_wff_23(68)       => __E___E_wff_23_24(69) _(11) wff(14).
	p(NT(68), (NT(69)+NT(11)+NT(14)));
//G75:  wff_always(67)       => __E_wff_23(68).
	p(NT(67), (NT(68)));
//G76:  wff(14)              => wff_always(67).
	p(NT(14), (NT(67)));
//G77:  __E_wff_25(71)       => wff(14) _(11) '?' _(11) wff(14) _(11) ':' _(11) wff(14).
	p(NT(71), (NT(14)+NT(11)+T(26)+NT(11)+NT(14)+NT(11)+T(2)+NT(11)+NT(14)));
//G78:  wff_conditional(70)  => __E_wff_25(71).
	p(NT(70), (NT(71)));
//G79:  wff(14)              => wff_conditional(70).
	p(NT(14), (NT(70)));
//G80:  __E_wff_26(73)       => 'a' 'l' 'l' __(27) q_vars(74) __(27) wff(14).
	p(NT(73), (T(5)+T(6)+T(6)+NT(27)+NT(74)+NT(27)+NT(14)));
//G81:  wff_all(72)          => __E_wff_26(73).
	p(NT(72), (NT(73)));
//G82:  wff(14)              => wff_all(72).
	p(NT(14), (NT(72)));
//G83:  __E_wff_27(76)       => 'e' 'x' __(27) q_vars(74) __(27) wff(14).
	p(NT(76), (T(19)+T(27)+NT(27)+NT(74)+NT(27)+NT(14)));
//G84:  wff_ex(75)           => __E_wff_27(76).
	p(NT(75), (NT(76)));
//G85:  wff(14)              => wff_ex(75).
	p(NT(14), (NT(75)));
//G86:  wff_ref(77)          => ref(18).
	p(NT(77), (NT(18)));
//G87:  wff(14)              => wff_ref(77).
	p(NT(14), (NT(77)));
//G88:  wff(14)              => constraint(78).
	p(NT(14), (NT(78)));
//G89:  __E_wff_28(80)       => wff(14) _(11) '-' '>' _(11) wff(14).
	p(NT(80), (NT(14)+NT(11)+T(28)+T(21)+NT(11)+NT(14)));
//G90:  wff_imply(79)        => __E_wff_28(80).
	p(NT(79), (NT(80)));
//G91:  wff(14)              => wff_imply(79).
	p(NT(14), (NT(79)));
//G92:  __E_wff_29(82)       => wff(14) _(11) '<' '-' '>' _(11) wff(14).
	p(NT(82), (NT(14)+NT(11)+T(20)+T(28)+T(21)+NT(11)+NT(14)));
//G93:  wff_equiv(81)        => __E_wff_29(82).
	p(NT(81), (NT(82)));
//G94:  wff(14)              => wff_equiv(81).
	p(NT(14), (NT(81)));
//G95:  __E_wff_30(84)       => wff(14) _(11) '|' '|' _(11) wff(14).
	p(NT(84), (NT(14)+NT(11)+T(29)+T(29)+NT(11)+NT(14)));
//G96:  wff_or(83)           => __E_wff_30(84).
	p(NT(83), (NT(84)));
//G97:  wff(14)              => wff_or(83).
	p(NT(14), (NT(83)));
//G98:  __E_wff_31(86)       => wff(14) _(11) '^' _(11) wff(14).
	p(NT(86), (NT(14)+NT(11)+T(30)+NT(11)+NT(14)));
//G99:  wff_xor(85)          => __E_wff_31(86).
	p(NT(85), (NT(86)));
//G100: wff(14)              => wff_xor(85).
	p(NT(14), (NT(85)));
//G101: __E_wff_32(88)       => wff(14) _(11) '&' '&' _(11) wff(14).
	p(NT(88), (NT(14)+NT(11)+T(31)+T(31)+NT(11)+NT(14)));
//G102: wff_and(87)          => __E_wff_32(88).
	p(NT(87), (NT(88)));
//G103: wff(14)              => wff_and(87).
	p(NT(14), (NT(87)));
//G104: __E_wff_33(90)       => '!' _(11) wff(14).
	p(NT(90), (T(32)+NT(11)+NT(14)));
//G105: wff_neg(89)          => __E_wff_33(90).
	p(NT(89), (NT(90)));
//G106: wff(14)              => wff_neg(89).
	p(NT(14), (NT(89)));
//G107: wff_t(91)            => 'T'.
	p(NT(91), (T(33)));
//G108: wff(14)              => wff_t(91).
	p(NT(14), (NT(91)));
//G109: wff_f(92)            => 'F'.
	p(NT(92), (T(34)));
//G110: wff(14)              => wff_f(92).
	p(NT(14), (NT(92)));
//G111: __E_wff_34(94)       => bf(21) _(11) '<' '=' _(11) bf(21) _(11) '<' '=' _(11) bf(21).
	p(NT(94), (NT(21)+NT(11)+T(20)+T(3)+NT(11)+NT(21)+NT(11)+T(20)+T(3)+NT(11)+NT(21)));
//G112: bf_interval(93)      => __E_wff_34(94).
	p(NT(93), (NT(94)));
//G113: wff(14)              => bf_interval(93).
	p(NT(14), (NT(93)));
//G114: __E_wff_35(96)       => bf(21) _(11) '=' _(11) bf(21).
	p(NT(96), (NT(21)+NT(11)+T(3)+NT(11)+NT(21)));
//G115: bf_eq(95)            => __E_wff_35(96).
	p(NT(95), (NT(96)));
//G116: wff(14)              => bf_eq(95).
	p(NT(14), (NT(95)));
//G117: __E_wff_36(98)       => bf(21) _(11) '!' '=' _(11) bf(21).
	p(NT(98), (NT(21)+NT(11)+T(32)+T(3)+NT(11)+NT(21)));
//G118: bf_neq(97)           => __E_wff_36(98).
	p(NT(97), (NT(98)));
//G119: wff(14)              => bf_neq(97).
	p(NT(14), (NT(97)));
//G120: __E_wff_37(100)      => bf(21) _(11) '<' '=' _(11) bf(21).
	p(NT(100), (NT(21)+NT(11)+T(20)+T(3)+NT(11)+NT(21)));
//G121: bf_less_equal(99)    => __E_wff_37(100).
	p(NT(99), (NT(100)));
//G122: wff(14)              => bf_less_equal(99).
	p(NT(14), (NT(99)));
//G123: __E_wff_38(102)      => bf(21) _(11) '!' '<' '=' _(11) bf(21).
	p(NT(102), (NT(21)+NT(11)+T(32)+T(20)+T(3)+NT(11)+NT(21)));
//G124: bf_nleq(101)         => __E_wff_38(102).
	p(NT(101), (NT(102)));
//G125: wff(14)              => bf_nleq(101).
	p(NT(14), (NT(101)));
//G126: __E_wff_39(104)      => bf(21) _(11) '>' _(11) bf(21).
	p(NT(104), (NT(21)+NT(11)+T(21)+NT(11)+NT(21)));
//G127: bf_greater(103)      => __E_wff_39(104).
	p(NT(103), (NT(104)));
//G128: wff(14)              => bf_greater(103).
	p(NT(14), (NT(103)));
//G129: __E_wff_40(106)      => bf(21) _(11) '!' '>' _(11) bf(21).
	p(NT(106), (NT(21)+NT(11)+T(32)+T(21)+NT(11)+NT(21)));
//G130: bf_ngreater(105)     => __E_wff_40(106).
	p(NT(105), (NT(106)));
//G131: wff(14)              => bf_ngreater(105).
	p(NT(14), (NT(105)));
//G132: __E_wff_41(108)      => bf(21) _(11) '>' '=' _(11) bf(21).
	p(NT(108), (NT(21)+NT(11)+T(21)+T(3)+NT(11)+NT(21)));
//G133: bf_greater_equal(107) => __E_wff_41(108).
	p(NT(107), (NT(108)));
//G134: wff(14)              => bf_greater_equal(107).
	p(NT(14), (NT(107)));
//G135: __E_wff_42(110)      => bf(21) _(11) '!' '>' '=' _(11) bf(21).
	p(NT(110), (NT(21)+NT(11)+T(32)+T(21)+T(3)+NT(11)+NT(21)));
//G136: bf_ngeq(109)         => __E_wff_42(110).
	p(NT(109), (NT(110)));
//G137: wff(14)              => bf_ngeq(109).
	p(NT(14), (NT(109)));
//G138: __E_wff_43(112)      => bf(21) _(11) '<' _(11) bf(21).
	p(NT(112), (NT(21)+NT(11)+T(20)+NT(11)+NT(21)));
//G139: bf_less(111)         => __E_wff_43(112).
	p(NT(111), (NT(112)));
//G140: wff(14)              => bf_less(111).
	p(NT(14), (NT(111)));
//G141: __E_wff_44(114)      => bf(21) _(11) '!' '<' _(11) bf(21).
	p(NT(114), (NT(21)+NT(11)+T(32)+T(20)+NT(11)+NT(21)));
//G142: bf_nless(113)        => __E_wff_44(114).
	p(NT(113), (NT(114)));
//G143: wff(14)              => bf_nless(113).
	p(NT(14), (NT(113)));
//G144: wff(14)              => capture(20).
	p(NT(14), (NT(20)));
//G145: __E_bf_45(116)       => '(' _(11) bf(21) _(11) ')'.
	p(NT(116), (T(15)+NT(11)+NT(21)+NT(11)+T(16)));
//G146: bf_parenthesis(115)  => __E_bf_45(116).
	p(NT(115), (NT(116)));
//G147: bf(21)               => bf_parenthesis(115).
	p(NT(21), (NT(115)));
//G148: bf(21)               => bf_constant(117).
	p(NT(21), (NT(117)));
//G149: bf(21)               => variable(118).
	p(NT(21), (NT(118)));
//G150: __E_bf_46(120)       => 'S' _(11) '(' _(11) bf(21) _(11) ')'.
	p(NT(120), (T(35)+NT(11)+T(15)+NT(11)+NT(21)+NT(11)+T(16)));
//G151: bf_splitter(119)     => __E_bf_46(120).
	p(NT(119), (NT(120)));
//G152: bf(21)               => bf_splitter(119).
	p(NT(21), (NT(119)));
//G153: bf_ref(121)          => ref(18).
	p(NT(121), (NT(18)));
//G154: bf(21)               => bf_ref(121).
	p(NT(21), (NT(121)));
//G155: __E_bf_47(123)       => bf(21) _(11) '|' _(11) bf(21).
	p(NT(123), (NT(21)+NT(11)+T(29)+NT(11)+NT(21)));
//G156: bf_or(122)           => __E_bf_47(123).
	p(NT(122), (NT(123)));
//G157: bf(21)               => bf_or(122).
	p(NT(21), (NT(122)));
//G158: __E_bf_48(125)       => bf(21) _(11) '+' _(11) bf(21).
	p(NT(125), (NT(21)+NT(11)+T(36)+NT(11)+NT(21)));
//G159: bf_xor(124)          => __E_bf_48(125).
	p(NT(124), (NT(125)));
//G160: bf(21)               => bf_xor(124).
	p(NT(21), (NT(124)));
//G161: __E___E_bf_49_50(128) => _(11) ':' _(11) type(129).
	p(NT(128), (NT(11)+T(2)+NT(11)+NT(129)));
//G162: __E___E_bf_49_50(128) => null.
	p(NT(128), (nul));
//G163: __E_bf_49(127)       => '1' __E___E_bf_49_50(128).
	p(NT(127), (T(37)+NT(128)));
//G164: bf_t(126)            => __E_bf_49(127).
	p(NT(126), (NT(127)));
//G165: bf(21)               => bf_t(126).
	p(NT(21), (NT(126)));
//G166: __E___E_bf_51_52(132) => _(11) ':' _(11) type(129).
	p(NT(132), (NT(11)+T(2)+NT(11)+NT(129)));
//G167: __E___E_bf_51_52(132) => null.
	p(NT(132), (nul));
//G168: __E_bf_51(131)       => '0' __E___E_bf_51_52(132).
	p(NT(131), (T(38)+NT(132)));
//G169: bf_f(130)            => __E_bf_51(131).
	p(NT(130), (NT(131)));
//G170: bf(21)               => bf_f(130).
	p(NT(21), (NT(130)));
//G171: __E___E_bf_53_54(135) => _(11) '&' _(11).
	p(NT(135), (NT(11)+T(31)+NT(11)));
//G172: __E___E_bf_53_54(135) => __(27).
	p(NT(135), (NT(27)));
//G173: __E_bf_53(134)       => bf(21) __E___E_bf_53_54(135) bf(21).
	p(NT(134), (NT(21)+NT(135)+NT(21)));
//G174: bf_and(133)          => __E_bf_53(134).
	p(NT(133), (NT(134)));
//G175: bf(21)               => bf_and(133).
	p(NT(21), (NT(133)));
//G176: bf_and(133)          => bf_and_nosep(136).
	p(NT(133), (NT(136)));
//G177: bf(21)               => bf_and(133).
	p(NT(21), (NT(133)));
//G178: __E_bf_55(138)       => bf(21) _(11) '\''.
	p(NT(138), (NT(21)+NT(11)+T(39)));
//G179: bf_neg(137)          => __E_bf_55(138).
	p(NT(137), (NT(138)));
//G180: bf(21)               => bf_neg(137).
	p(NT(21), (NT(137)));
//G181: bf(21)               => capture(20).
	p(NT(21), (NT(20)));
//G182: __N_0(379)           => bf_constant(117).
	p(NT(379), (NT(117)));
//G183: __N_1(380)           => bf_t(126).
	p(NT(380), (NT(126)));
//G184: __N_2(381)           => bf_f(130).
	p(NT(381), (NT(130)));
//G185: __N_3(382)           => bf_and(133).
	p(NT(382), (NT(133)));
//G186: __E_bf_and_nosep_56(139) => bf(21) & ~( __N_0(379) ) & ~( __N_1(380) ) & ~( __N_2(381) ) & ~( __N_3(382) ).	 # conjunctive
	p(NT(139), (NT(21)) & ~(NT(379)) & ~(NT(380)) & ~(NT(381)) & ~(NT(382)));
//G187: bf_and_nosep(136)    => __E_bf_and_nosep_56(139) bf(21).
	p(NT(136), (NT(139)+NT(21)));
//G188: __E_constraint_57(141) => '[' ctnvar(142) _(11) '!' '=' _(11) num(143) ']'.
	p(NT(141), (T(24)+NT(142)+NT(11)+T(32)+T(3)+NT(11)+NT(143)+T(25)));
//G189: __E_constraint_57(141) => '[' num(143) _(11) '!' '=' _(11) ctnvar(142) ']'.
	p(NT(141), (T(24)+NT(143)+NT(11)+T(32)+T(3)+NT(11)+NT(142)+T(25)));
//G190: ctn_neq(140)         => __E_constraint_57(141).
	p(NT(140), (NT(141)));
//G191: constraint(78)       => ctn_neq(140).
	p(NT(78), (NT(140)));
//G192: __E_constraint_58(145) => '[' ctnvar(142) _(11) '=' _(11) num(143) ']'.
	p(NT(145), (T(24)+NT(142)+NT(11)+T(3)+NT(11)+NT(143)+T(25)));
//G193: __E_constraint_58(145) => '[' num(143) _(11) '=' _(11) ctnvar(142) ']'.
	p(NT(145), (T(24)+NT(143)+NT(11)+T(3)+NT(11)+NT(142)+T(25)));
//G194: ctn_eq(144)          => __E_constraint_58(145).
	p(NT(144), (NT(145)));
//G195: constraint(78)       => ctn_eq(144).
	p(NT(78), (NT(144)));
//G196: __E_constraint_59(147) => '[' ctnvar(142) _(11) '>' '=' _(11) num(143) ']'.
	p(NT(147), (T(24)+NT(142)+NT(11)+T(21)+T(3)+NT(11)+NT(143)+T(25)));
//G197: __E_constraint_59(147) => '[' num(143) _(11) '>' '=' _(11) ctnvar(142) ']'.
	p(NT(147), (T(24)+NT(143)+NT(11)+T(21)+T(3)+NT(11)+NT(142)+T(25)));
//G198: ctn_greater_equal(146) => __E_constraint_59(147).
	p(NT(146), (NT(147)));
//G199: constraint(78)       => ctn_greater_equal(146).
	p(NT(78), (NT(146)));
//G200: __E_constraint_60(149) => '[' ctnvar(142) _(11) '>' _(11) num(143) ']'.
	p(NT(149), (T(24)+NT(142)+NT(11)+T(21)+NT(11)+NT(143)+T(25)));
//G201: __E_constraint_60(149) => '[' num(143) _(11) '>' _(11) ctnvar(142) ']'.
	p(NT(149), (T(24)+NT(143)+NT(11)+T(21)+NT(11)+NT(142)+T(25)));
//G202: ctn_greater(148)     => __E_constraint_60(149).
	p(NT(148), (NT(149)));
//G203: constraint(78)       => ctn_greater(148).
	p(NT(78), (NT(148)));
//G204: __E_constraint_61(151) => '[' ctnvar(142) _(11) '<' '=' _(11) num(143) ']'.
	p(NT(151), (T(24)+NT(142)+NT(11)+T(20)+T(3)+NT(11)+NT(143)+T(25)));
//G205: __E_constraint_61(151) => '[' num(143) _(11) '<' '=' _(11) ctnvar(142) ']'.
	p(NT(151), (T(24)+NT(143)+NT(11)+T(20)+T(3)+NT(11)+NT(142)+T(25)));
//G206: ctn_less_equal(150)  => __E_constraint_61(151).
	p(NT(150), (NT(151)));
//G207: constraint(78)       => ctn_less_equal(150).
	p(NT(78), (NT(150)));
//G208: __E_constraint_62(153) => '[' ctnvar(142) _(11) '<' _(11) num(143) ']'.
	p(NT(153), (T(24)+NT(142)+NT(11)+T(20)+NT(11)+NT(143)+T(25)));
//G209: __E_constraint_62(153) => '[' num(143) _(11) '<' _(11) ctnvar(142) ']'.
	p(NT(153), (T(24)+NT(143)+NT(11)+T(20)+NT(11)+NT(142)+T(25)));
//G210: ctn_less(152)        => __E_constraint_62(153).
	p(NT(152), (NT(153)));
//G211: constraint(78)       => ctn_less(152).
	p(NT(78), (NT(152)));
//G212: __E_variable_63(154) => null.
	p(NT(154), (nul));
//G213: __E_variable_63(154) => digit(3) __E_variable_63(154).
	p(NT(154), (NT(3)+NT(154)));
//G214: variable(118)        => alpha(5) __E_variable_63(154).	 # guarded: charvar
	p(NT(118), (NT(5)+NT(154)));
	p.back().guard = "charvar";
//G215: variable(118)        => chars(155).	 # guarded: var
	p(NT(118), (NT(155)));
	p.back().guard = "var";
//G216: __E_variable_64(157) => in(158).
	p(NT(157), (NT(158)));
//G217: __E_variable_64(157) => out(159).
	p(NT(157), (NT(159)));
//G218: io_var(156)          => __E_variable_64(157).
	p(NT(156), (NT(157)));
//G219: variable(118)        => io_var(156).
	p(NT(118), (NT(156)));
//G220: variable(118)        => uninterpreted_constant(160).
	p(NT(118), (NT(160)));
//G221: __E___E_in_65_66(163) => digit(3).
	p(NT(163), (NT(3)));
//G222: __E___E_in_65_66(163) => digit(3) __E___E_in_65_66(163).
	p(NT(163), (NT(3)+NT(163)));
//G223: __E_in_65(162)       => 'i' __E___E_in_65_66(163).
	p(NT(162), (T(10)+NT(163)));
//G224: in_var_name(161)     => __E_in_65(162).
	p(NT(161), (NT(162)));
//G225: in(158)              => in_var_name(161) '[' offset(164) ']'.
	p(NT(158), (NT(161)+T(24)+NT(164)+T(25)));
//G226: __E___E_out_67_68(167) => digit(3).
	p(NT(167), (NT(3)));
//G227: __E___E_out_67_68(167) => digit(3) __E___E_out_67_68(167).
	p(NT(167), (NT(3)+NT(167)));
//G228: __E_out_67(166)      => 'o' __E___E_out_67_68(167).
	p(NT(166), (T(17)+NT(167)));
//G229: out_var_name(165)    => __E_out_67(166).
	p(NT(165), (NT(166)));
//G230: out(159)             => out_var_name(165) '[' offset(164) ']'.
	p(NT(159), (NT(165)+T(24)+NT(164)+T(25)));
//G231: ctnvar(142)          => chars(155).
	p(NT(142), (NT(155)));
//G232: capture(20)          => '$' chars(155).
	p(NT(20), (T(40)+NT(155)));
//G233: __E_q_vars_69(169)   => _(11) ',' _(11) q_var(168).
	p(NT(169), (NT(11)+T(14)+NT(11)+NT(168)));
//G234: __E_q_vars_70(170)   => null.
	p(NT(170), (nul));
//G235: __E_q_vars_70(170)   => __E_q_vars_69(169) __E_q_vars_70(170).
	p(NT(170), (NT(169)+NT(170)));
//G236: q_vars(74)           => q_var(168) __E_q_vars_70(170).
	p(NT(74), (NT(168)+NT(170)));
//G237: q_var(168)           => capture(20).
	p(NT(168), (NT(20)));
//G238: __N_4(383)           => uninterpreted_constant(160).
	p(NT(383), (NT(160)));
//G239: q_var(168)           => variable(118) & ~( __N_4(383) ).	 # conjunctive
	p(NT(168), (NT(118)) & ~(NT(383)));
//G240: __E_offsets_71(171)  => _(11) ',' _(11) offset(164).
	p(NT(171), (NT(11)+T(14)+NT(11)+NT(164)));
//G241: __E_offsets_72(172)  => null.
	p(NT(172), (nul));
//G242: __E_offsets_72(172)  => __E_offsets_71(171) __E_offsets_72(172).
	p(NT(172), (NT(171)+NT(172)));
//G243: offsets(24)          => '[' _(11) offset(164) __E_offsets_72(172) _(11) ']'.
	p(NT(24), (T(24)+NT(11)+NT(164)+NT(172)+NT(11)+T(25)));
//G244: offset(164)          => num(143).
	p(NT(164), (NT(143)));
//G245: offset(164)          => capture(20).
	p(NT(164), (NT(20)));
//G246: offset(164)          => shift(173).
	p(NT(164), (NT(173)));
//G247: __N_5(384)           => io_var(156).
	p(NT(384), (NT(156)));
//G248: __E_offset_73(174)   => variable(118) & ~( __N_5(384) ).	 # conjunctive
	p(NT(174), (NT(118)) & ~(NT(384)));
//G249: offset(164)          => __E_offset_73(174).
	p(NT(164), (NT(174)));
//G250: __E_shift_74(175)    => capture(20).
	p(NT(175), (NT(20)));
//G251: __N_6(385)           => io_var(156).
	p(NT(385), (NT(156)));
//G252: __E___E_shift_74_75(176) => variable(118) & ~( __N_6(385) ).	 # conjunctive
	p(NT(176), (NT(118)) & ~(NT(385)));
//G253: __E_shift_74(175)    => __E___E_shift_74_75(176).
	p(NT(175), (NT(176)));
//G254: shift(173)           => __E_shift_74(175) _(11) '-' _(11) num(143).
	p(NT(173), (NT(175)+NT(11)+T(28)+NT(11)+NT(143)));
//G255: num(143)             => digits(177).
	p(NT(143), (NT(177)));
//G256: __E___E_uninterpreted_constant_76_77(180) => chars(155).
	p(NT(180), (NT(155)));
//G257: __E___E_uninterpreted_constant_76_77(180) => _(11).
	p(NT(180), (NT(11)));
//G258: __E_uninterpreted_constant_76(179) => __E___E_uninterpreted_constant_76_77(180) ':' chars(155).
	p(NT(179), (NT(180)+T(2)+NT(155)));
//G259: uninter_const_name(178) => __E_uninterpreted_constant_76(179).
	p(NT(178), (NT(179)));
//G260: uninterpreted_constant(160) => '<' uninter_const_name(178) _(11) '>'.
	p(NT(160), (T(20)+NT(178)+NT(11)+T(21)));
//G261: __E_bf_constant_78(182) => capture(20).
	p(NT(182), (NT(20)));
//G262: __E_bf_constant_78(182) => binding(183).
	p(NT(182), (NT(183)));
//G263: constant(181)        => __E_bf_constant_78(182).
	p(NT(181), (NT(182)));
//G264: type(129)            => chars(155).
	p(NT(129), (NT(155)));
//G265: __E_bf_constant_79(184) => _(11) ':' _(11) type(129).
	p(NT(184), (NT(11)+T(2)+NT(11)+NT(129)));
//G266: __E_bf_constant_79(184) => null.
	p(NT(184), (nul));
//G267: bf_constant(117)     => '{' _(11) constant(181) _(11) '}' __E_bf_constant_79(184).
	p(NT(117), (T(41)+NT(11)+NT(181)+NT(11)+T(42)+NT(184)));
//G268: __E___E___E_binding_80_81_82(189) => src_c(187).
	p(NT(189), (NT(187)));
//G269: __E___E___E_binding_80_81_82(189) => space(2).
	p(NT(189), (NT(2)));
//G270: __E___E___E_binding_80_81_83(190) => null.
	p(NT(190), (nul));
//G271: __E___E___E_binding_80_81_83(190) => __E___E___E_binding_80_81_82(189) __E___E___E_binding_80_81_83(190).
	p(NT(190), (NT(189)+NT(190)));
//G272: __E___E_binding_80_81(188) => __E___E___E_binding_80_81_83(190) src_c(187).
	p(NT(188), (NT(190)+NT(187)));
//G273: __E___E_binding_80_81(188) => null.
	p(NT(188), (nul));
//G274: __E_binding_80(186)  => src_c(187) __E___E_binding_80_81(188).
	p(NT(186), (NT(187)+NT(188)));
//G275: source(185)          => __E_binding_80(186).
	p(NT(185), (NT(186)));
//G276: binding(183)         => source(185).
	p(NT(183), (NT(185)));
//G277: src_c(187)           => alnum(6).
	p(NT(187), (NT(6)));
//G278: __N_7(386)           => '{'.
	p(NT(386), (T(41)));
//G279: __N_8(387)           => '}'.
	p(NT(387), (T(42)));
//G280: src_c(187)           => ~( __N_7(386) ) & ~( __N_8(387) ) & punct(7).	 # conjunctive
	p(NT(187), ~(NT(386)) & ~(NT(387)) & (NT(7)));
//G281: __E_src_c_84(191)    => src_c(187).
	p(NT(191), (NT(187)));
//G282: __E_src_c_84(191)    => space(2).
	p(NT(191), (NT(2)));
//G283: __E_src_c_85(192)    => null.
	p(NT(192), (nul));
//G284: __E_src_c_85(192)    => __E_src_c_84(191) __E_src_c_85(192).
	p(NT(192), (NT(191)+NT(192)));
//G285: src_c(187)           => '{' __E_src_c_85(192) '}'.
	p(NT(187), (T(41)+NT(192)+T(42)));
//G286: __E_chars_86(193)    => alnum(6).
	p(NT(193), (NT(6)));
//G287: __E_chars_86(193)    => '_'.
	p(NT(193), (T(43)));
//G288: __E_chars_87(194)    => null.
	p(NT(194), (nul));
//G289: __E_chars_87(194)    => __E_chars_86(193) __E_chars_87(194).
	p(NT(194), (NT(193)+NT(194)));
//G290: chars(155)           => alpha(5) __E_chars_87(194).
	p(NT(155), (NT(5)+NT(194)));
//G291: __E_digits_88(195)   => digit(3).
	p(NT(195), (NT(3)));
//G292: __E_digits_88(195)   => digit(3) __E_digits_88(195).
	p(NT(195), (NT(3)+NT(195)));
//G293: digits(177)          => __E_digits_88(195).
	p(NT(177), (NT(195)));
//G294: sym(22)              => chars(155).
	p(NT(22), (NT(155)));
//G295: extra(196)           => chars(155).
	p(NT(196), (NT(155)));
//G296: __E_comment_89(198)  => printable(8).
	p(NT(198), (NT(8)));
//G297: __E_comment_89(198)  => '\t'.
	p(NT(198), (T(44)));
//G298: __E_comment_90(199)  => null.
	p(NT(199), (nul));
//G299: __E_comment_90(199)  => __E_comment_89(198) __E_comment_90(199).
	p(NT(199), (NT(198)+NT(199)));
//G300: __E_comment_91(200)  => '\n'.
	p(NT(200), (T(45)));
//G301: __E_comment_91(200)  => '\r'.
	p(NT(200), (T(46)));
//G302: __E_comment_91(200)  => eof(1).
	p(NT(200), (NT(1)));
//G303: comment(197)         => '#' __E_comment_90(199) __E_comment_91(200).
	p(NT(197), (T(47)+NT(199)+NT(200)));
//G304: __E____92(201)       => space(2).
	p(NT(201), (NT(2)));
//G305: __E____92(201)       => comment(197).
	p(NT(201), (NT(197)));
//G306: __(27)               => __E____92(201) _(11).
	p(NT(27), (NT(201)+NT(11)));
//G307: __E___93(202)        => __(27).
	p(NT(202), (NT(27)));
//G308: __E___93(202)        => null.
	p(NT(202), (nul));
//G309: _(11)                => __E___93(202).
	p(NT(11), (NT(202)));
//G310: cli(203)             => _(11).
	p(NT(203), (NT(11)));
//G311: __E_cli_94(205)      => '.' _(11) cli_command(204) _(11).
	p(NT(205), (T(1)+NT(11)+NT(204)+NT(11)));
//G312: __E_cli_95(206)      => null.
	p(NT(206), (nul));
//G313: __E_cli_95(206)      => __E_cli_94(205) __E_cli_95(206).
	p(NT(206), (NT(205)+NT(206)));
//G314: cli(203)             => _(11) cli_command(204) _(11) __E_cli_95(206).
	p(NT(203), (NT(11)+NT(204)+NT(11)+NT(206)));
//G315: __E_cli_command_96(209) => 'q'.
	p(NT(209), (T(48)));
//G316: __E_cli_command_96(209) => 'q' 'u' 'i' 't'.
	p(NT(209), (T(48)+T(49)+T(10)+T(13)));
//G317: quit_sym(208)        => __E_cli_command_96(209).
	p(NT(208), (NT(209)));
//G318: quit_cmd(207)        => quit_sym(208).
	p(NT(207), (NT(208)));
//G319: cli_command(204)     => quit_cmd(207).
	p(NT(204), (NT(207)));
//G320: __E_cli_command_97(212) => 'v'.
	p(NT(212), (T(50)));
//G321: __E_cli_command_97(212) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(212), (T(50)+T(19)+T(11)+T(12)+T(10)+T(17)+T(51)));
//G322: version_sym(211)     => __E_cli_command_97(212).
	p(NT(211), (NT(212)));
//G323: version_cmd(210)     => version_sym(211).
	p(NT(210), (NT(211)));
//G324: cli_command(204)     => version_cmd(210).
	p(NT(204), (NT(210)));
//G325: __E_cli_command_98(215) => 'c'.
	p(NT(215), (T(8)));
//G326: __E_cli_command_98(215) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(215), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G327: clear_sym(214)       => __E_cli_command_98(215).
	p(NT(214), (NT(215)));
//G328: clear_cmd(213)       => clear_sym(214).
	p(NT(213), (NT(214)));
//G329: cli_command(204)     => clear_cmd(213).
	p(NT(204), (NT(213)));
//G330: __E___E_cli_command_99_100(219) => 'h'.
	p(NT(219), (T(52)));
//G331: __E___E_cli_command_99_100(219) => 'h' 'e' 'l' 'p'.
	p(NT(219), (T(52)+T(19)+T(6)+T(53)));
//G332: help_sym(218)        => __E___E_cli_command_99_100(219).
	p(NT(218), (NT(219)));
//G333: __E___E_cli_command_99_101(220) => __(27) help_arg(221).
	p(NT(220), (NT(27)+NT(221)));
//G334: __E___E_cli_command_99_101(220) => null.
	p(NT(220), (nul));
//G335: __E_cli_command_99(217) => help_sym(218) __E___E_cli_command_99_101(220).
	p(NT(217), (NT(218)+NT(220)));
//G336: help_cmd(216)        => __E_cli_command_99(217).
	p(NT(216), (NT(217)));
//G337: cli_command(204)     => help_cmd(216).
	p(NT(204), (NT(216)));
//G338: file_sym(224)        => 'f' 'i' 'l' 'e'.
	p(NT(224), (T(4)+T(10)+T(6)+T(19)));
//G339: __E_cli_command_102(223) => file_sym(224) __(27) q_string(225).
	p(NT(223), (NT(224)+NT(27)+NT(225)));
//G340: file_cmd(222)        => __E_cli_command_102(223).
	p(NT(222), (NT(223)));
//G341: cli_command(204)     => file_cmd(222).
	p(NT(204), (NT(222)));
//G342: valid_sym(228)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(228), (T(50)+T(5)+T(6)+T(10)+T(54)));
//G343: __E_cli_command_103(227) => valid_sym(228) __(27) normalize_cmd_arg(229).
	p(NT(227), (NT(228)+NT(27)+NT(229)));
//G344: valid_cmd(226)       => __E_cli_command_103(227).
	p(NT(226), (NT(227)));
//G345: cli_command(204)     => valid_cmd(226).
	p(NT(204), (NT(226)));
//G346: sat_sym(232)         => 's' 'a' 't'.
	p(NT(232), (T(12)+T(5)+T(13)));
//G347: __E_cli_command_104(231) => sat_sym(232) __(27) normalize_cmd_arg(229).
	p(NT(231), (NT(232)+NT(27)+NT(229)));
//G348: sat_cmd(230)         => __E_cli_command_104(231).
	p(NT(230), (NT(231)));
//G349: cli_command(204)     => sat_cmd(230).
	p(NT(204), (NT(230)));
//G350: unsat_sym(235)       => 'u' 'n' 's' 'a' 't'.
	p(NT(235), (T(49)+T(51)+T(12)+T(5)+T(13)));
//G351: __E_cli_command_105(234) => unsat_sym(235) __(27) normalize_cmd_arg(229).
	p(NT(234), (NT(235)+NT(27)+NT(229)));
//G352: unsat_cmd(233)       => __E_cli_command_105(234).
	p(NT(233), (NT(234)));
//G353: cli_command(204)     => unsat_cmd(233).
	p(NT(204), (NT(233)));
//G354: solve_sym(238)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(238), (T(12)+T(17)+T(6)+T(50)+T(19)));
//G355: __E___E_cli_command_106_107(239) => __(27) type(129).
	p(NT(239), (NT(27)+NT(129)));
//G356: __E___E_cli_command_106_107(239) => null.
	p(NT(239), (nul));
//G357: __E_cli_command_106(237) => solve_sym(238) __E___E_cli_command_106_107(239) __(27) solve_cmd_arg(240).
	p(NT(237), (NT(238)+NT(239)+NT(27)+NT(240)));
//G358: solve_cmd(236)       => __E_cli_command_106(237).
	p(NT(236), (NT(237)));
//G359: cli_command(204)     => solve_cmd(236).
	p(NT(204), (NT(236)));
//G360: __E___E_cli_command_108_109(244) => 'r'.
	p(NT(244), (T(11)));
//G361: __E___E_cli_command_108_109(244) => 'r' 'u' 'n'.
	p(NT(244), (T(11)+T(49)+T(51)));
//G362: run_sym(243)         => __E___E_cli_command_108_109(244).
	p(NT(243), (NT(244)));
//G363: __E___E_cli_command_108_110(245) => memory(246).
	p(NT(245), (NT(246)));
//G364: __E___E_cli_command_108_110(245) => wff(14).
	p(NT(245), (NT(14)));
//G365: __E_cli_command_108(242) => run_sym(243) __(27) __E___E_cli_command_108_110(245).
	p(NT(242), (NT(243)+NT(27)+NT(245)));
//G366: run_cmd(241)         => __E_cli_command_108(242).
	p(NT(241), (NT(242)));
//G367: cli_command(204)     => run_cmd(241).
	p(NT(204), (NT(241)));
//G368: __E___E_cli_command_111_112(250) => 'n'.
	p(NT(250), (T(51)));
//G369: __E___E_cli_command_111_112(250) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(250), (T(51)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(55)+T(19)));
//G370: normalize_sym(249)   => __E___E_cli_command_111_112(250).
	p(NT(249), (NT(250)));
//G371: __E_cli_command_111(248) => normalize_sym(249) __(27) normalize_cmd_arg(229).
	p(NT(248), (NT(249)+NT(27)+NT(229)));
//G372: normalize_cmd(247)   => __E_cli_command_111(248).
	p(NT(247), (NT(248)));
//G373: cli_command(204)     => normalize_cmd(247).
	p(NT(204), (NT(247)));
//G374: __E___E_cli_command_113_114(254) => 's'.
	p(NT(254), (T(12)));
//G375: __E___E_cli_command_113_114(254) => 's' 'u' 'b' 's' 't'.
	p(NT(254), (T(12)+T(49)+T(7)+T(12)+T(13)));
//G376: __E___E_cli_command_113_114(254) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(254), (T(12)+T(49)+T(7)+T(12)+T(13)+T(10)+T(13)+T(49)+T(13)+T(19)));
//G377: subst_sym(253)       => __E___E_cli_command_113_114(254).
	p(NT(253), (NT(254)));
//G378: __E_cli_command_113(252) => subst_sym(253) __(27) nf_cmd_arg(255) _(11) '[' _(11) nf_cmd_arg(255) _(11) '/' _(11) nf_cmd_arg(255) _(11) ']'.
	p(NT(252), (NT(253)+NT(27)+NT(255)+NT(11)+T(24)+NT(11)+NT(255)+NT(11)+T(56)+NT(11)+NT(255)+NT(11)+T(25)));
//G379: subst_cmd(251)       => __E_cli_command_113(252).
	p(NT(251), (NT(252)));
//G380: cli_command(204)     => subst_cmd(251).
	p(NT(204), (NT(251)));
//G381: __E___E_cli_command_115_116(259) => 'i'.
	p(NT(259), (T(10)));
//G382: __E___E_cli_command_115_116(259) => 'i' 'n' 's' 't'.
	p(NT(259), (T(10)+T(51)+T(12)+T(13)));
//G383: __E___E_cli_command_115_116(259) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(259), (T(10)+T(51)+T(12)+T(13)+T(5)+T(51)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G384: inst_sym(258)        => __E___E_cli_command_115_116(259).
	p(NT(258), (NT(259)));
//G385: __E_cli_command_115(257) => inst_sym(258) __(27) inst_args(260).
	p(NT(257), (NT(258)+NT(27)+NT(260)));
//G386: inst_cmd(256)        => __E_cli_command_115(257).
	p(NT(256), (NT(257)));
//G387: cli_command(204)     => inst_cmd(256).
	p(NT(204), (NT(256)));
//G388: dnf_sym(263)         => 'd' 'n' 'f'.
	p(NT(263), (T(54)+T(51)+T(4)));
//G389: __E_cli_command_117(262) => dnf_sym(263) __(27) nf_cmd_arg(255).
	p(NT(262), (NT(263)+NT(27)+NT(255)));
//G390: dnf_cmd(261)         => __E_cli_command_117(262).
	p(NT(261), (NT(262)));
//G391: cli_command(204)     => dnf_cmd(261).
	p(NT(204), (NT(261)));
//G392: cnf_sym(266)         => 'c' 'n' 'f'.
	p(NT(266), (T(8)+T(51)+T(4)));
//G393: __E_cli_command_118(265) => cnf_sym(266) __(27) nf_cmd_arg(255).
	p(NT(265), (NT(266)+NT(27)+NT(255)));
//G394: cnf_cmd(264)         => __E_cli_command_118(265).
	p(NT(264), (NT(265)));
//G395: cli_command(204)     => cnf_cmd(264).
	p(NT(204), (NT(264)));
//G396: anf_sym(269)         => 'a' 'n' 'f'.
	p(NT(269), (T(5)+T(51)+T(4)));
//G397: __E_cli_command_119(268) => anf_sym(269) __(27) nf_cmd_arg(255).
	p(NT(268), (NT(269)+NT(27)+NT(255)));
//G398: anf_cmd(267)         => __E_cli_command_119(268).
	p(NT(267), (NT(268)));
//G399: cli_command(204)     => anf_cmd(267).
	p(NT(204), (NT(267)));
//G400: nnf_sym(272)         => 'n' 'n' 'f'.
	p(NT(272), (T(51)+T(51)+T(4)));
//G401: __E_cli_command_120(271) => nnf_sym(272) __(27) nf_cmd_arg(255).
	p(NT(271), (NT(272)+NT(27)+NT(255)));
//G402: nnf_cmd(270)         => __E_cli_command_120(271).
	p(NT(270), (NT(271)));
//G403: cli_command(204)     => nnf_cmd(270).
	p(NT(204), (NT(270)));
//G404: pnf_sym(275)         => 'p' 'n' 'f'.
	p(NT(275), (T(53)+T(51)+T(4)));
//G405: __E_cli_command_121(274) => pnf_sym(275) __(27) nf_cmd_arg(255).
	p(NT(274), (NT(275)+NT(27)+NT(255)));
//G406: pnf_cmd(273)         => __E_cli_command_121(274).
	p(NT(273), (NT(274)));
//G407: cli_command(204)     => pnf_cmd(273).
	p(NT(204), (NT(273)));
//G408: mnf_sym(278)         => 'm' 'n' 'f'.
	p(NT(278), (T(18)+T(51)+T(4)));
//G409: __E_cli_command_122(277) => mnf_sym(278) __(27) nf_cmd_arg(255).
	p(NT(277), (NT(278)+NT(27)+NT(255)));
//G410: mnf_cmd(276)         => __E_cli_command_122(277).
	p(NT(276), (NT(277)));
//G411: cli_command(204)     => mnf_cmd(276).
	p(NT(204), (NT(276)));
//G412: snf_sym(281)         => 's' 'n' 'f'.
	p(NT(281), (T(12)+T(51)+T(4)));
//G413: __E_cli_command_123(280) => snf_sym(281) __(27) nf_cmd_arg(255).
	p(NT(280), (NT(281)+NT(27)+NT(255)));
//G414: snf_cmd(279)         => __E_cli_command_123(280).
	p(NT(279), (NT(280)));
//G415: cli_command(204)     => snf_cmd(279).
	p(NT(204), (NT(279)));
//G416: onf_sym(284)         => 'o' 'n' 'f'.
	p(NT(284), (T(17)+T(51)+T(4)));
//G417: __E_cli_command_124(283) => onf_sym(284) __(27) variable(118) __(27) onf_cmd_arg(285).
	p(NT(283), (NT(284)+NT(27)+NT(118)+NT(27)+NT(285)));
//G418: onf_cmd(282)         => __E_cli_command_124(283).
	p(NT(282), (NT(283)));
//G419: cli_command(204)     => onf_cmd(282).
	p(NT(204), (NT(282)));
//G420: __E___E_cli_command_125_126(289) => 'd' 'e' 'f' 's'.
	p(NT(289), (T(54)+T(19)+T(4)+T(12)));
//G421: __E___E_cli_command_125_126(289) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(289), (T(54)+T(19)+T(4)+T(10)+T(51)+T(10)+T(13)+T(10)+T(17)+T(51)+T(12)));
//G422: def_sym(288)         => __E___E_cli_command_125_126(289).
	p(NT(288), (NT(289)));
//G423: __E_cli_command_125(287) => def_sym(288).
	p(NT(287), (NT(288)));
//G424: def_list_cmd(286)    => __E_cli_command_125(287).
	p(NT(286), (NT(287)));
//G425: cli_command(204)     => def_list_cmd(286).
	p(NT(204), (NT(286)));
//G426: __E_cli_command_127(291) => def_sym(288) __(27) number(292).
	p(NT(291), (NT(288)+NT(27)+NT(292)));
//G427: def_print_cmd(290)   => __E_cli_command_127(291).
	p(NT(290), (NT(291)));
//G428: cli_command(204)     => def_print_cmd(290).
	p(NT(204), (NT(290)));
//G429: def_rr_cmd(293)      => rec_relation(16).
	p(NT(293), (NT(16)));
//G430: cli_command(204)     => def_rr_cmd(293).
	p(NT(204), (NT(293)));
//G431: qelim_sym(296)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(296), (T(48)+T(19)+T(6)+T(10)+T(18)));
//G432: __E_cli_command_128(295) => qelim_sym(296) __(27) wff_cmd_arg(297).
	p(NT(295), (NT(296)+NT(27)+NT(297)));
//G433: qelim_cmd(294)       => __E_cli_command_128(295).
	p(NT(294), (NT(295)));
//G434: cli_command(204)     => qelim_cmd(294).
	p(NT(204), (NT(294)));
//G435: get_sym(300)         => 'g' 'e' 't'.
	p(NT(300), (T(57)+T(19)+T(13)));
//G436: __E___E_cli_command_129_130(301) => __(27) option(302).
	p(NT(301), (NT(27)+NT(302)));
//G437: __E___E_cli_command_129_130(301) => null.
	p(NT(301), (nul));
//G438: __E_cli_command_129(299) => get_sym(300) __E___E_cli_command_129_130(301).
	p(NT(299), (NT(300)+NT(301)));
//G439: get_cmd(298)         => __E_cli_command_129(299).
	p(NT(298), (NT(299)));
//G440: cli_command(204)     => get_cmd(298).
	p(NT(204), (NT(298)));
//G441: set_sym(305)         => 's' 'e' 't'.
	p(NT(305), (T(12)+T(19)+T(13)));
//G442: __E___E_cli_command_131_132(306) => __(27).
	p(NT(306), (NT(27)));
//G443: __E___E_cli_command_131_132(306) => _(11) '=' _(11).
	p(NT(306), (NT(11)+T(3)+NT(11)));
//G444: __E_cli_command_131(304) => set_sym(305) __(27) option(302) __E___E_cli_command_131_132(306) option_value(307).
	p(NT(304), (NT(305)+NT(27)+NT(302)+NT(306)+NT(307)));
//G445: set_cmd(303)         => __E_cli_command_131(304).
	p(NT(303), (NT(304)));
//G446: cli_command(204)     => set_cmd(303).
	p(NT(204), (NT(303)));
//G447: enable_sym(310)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(310), (T(19)+T(51)+T(5)+T(7)+T(6)+T(19)));
//G448: __E_cli_command_133(309) => enable_sym(310) __(27) bool_option(311).
	p(NT(309), (NT(310)+NT(27)+NT(311)));
//G449: enable_cmd(308)      => __E_cli_command_133(309).
	p(NT(308), (NT(309)));
//G450: cli_command(204)     => enable_cmd(308).
	p(NT(204), (NT(308)));
//G451: disable_sym(314)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(314), (T(54)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G452: __E_cli_command_134(313) => disable_sym(314) __(27) bool_option(311).
	p(NT(313), (NT(314)+NT(27)+NT(311)));
//G453: disable_cmd(312)     => __E_cli_command_134(313).
	p(NT(312), (NT(313)));
//G454: cli_command(204)     => disable_cmd(312).
	p(NT(204), (NT(312)));
//G455: toggle_sym(317)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(317), (T(13)+T(17)+T(57)+T(57)+T(6)+T(19)));
//G456: __E_cli_command_135(316) => toggle_sym(317) __(27) bool_option(311).
	p(NT(316), (NT(317)+NT(27)+NT(311)));
//G457: toggle_cmd(315)      => __E_cli_command_135(316).
	p(NT(315), (NT(316)));
//G458: cli_command(204)     => toggle_cmd(315).
	p(NT(204), (NT(315)));
//G459: __E_cli_command_136(319) => type(129) __(27) in_var_name(161) _(11) '=' _(11) input_stream(320).
	p(NT(319), (NT(129)+NT(27)+NT(161)+NT(11)+T(3)+NT(11)+NT(320)));
//G460: def_input_cmd(318)   => __E_cli_command_136(319).
	p(NT(318), (NT(319)));
//G461: cli_command(204)     => def_input_cmd(318).
	p(NT(204), (NT(318)));
//G462: __E_cli_command_137(322) => type(129) __(27) out_var_name(165) _(11) '=' _(11) output_stream(323).
	p(NT(322), (NT(129)+NT(27)+NT(165)+NT(11)+T(3)+NT(11)+NT(323)));
//G463: def_output_cmd(321)  => __E_cli_command_137(322).
	p(NT(321), (NT(322)));
//G464: cli_command(204)     => def_output_cmd(321).
	p(NT(204), (NT(321)));
//G465: __E___E_cli_command_138_139(327) => 'h' 'i' 's' 't'.
	p(NT(327), (T(52)+T(10)+T(12)+T(13)));
//G466: __E___E_cli_command_138_139(327) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(327), (T(52)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G467: history_sym(326)     => __E___E_cli_command_138_139(327).
	p(NT(326), (NT(327)));
//G468: __E_cli_command_138(325) => history_sym(326).
	p(NT(325), (NT(326)));
//G469: history_list_cmd(324) => __E_cli_command_138(325).
	p(NT(324), (NT(325)));
//G470: cli_command(204)     => history_list_cmd(324).
	p(NT(204), (NT(324)));
//G471: __E_cli_command_140(329) => history_sym(326) __(27) memory(246).
	p(NT(329), (NT(326)+NT(27)+NT(246)));
//G472: history_print_cmd(328) => __E_cli_command_140(329).
	p(NT(328), (NT(329)));
//G473: cli_command(204)     => history_print_cmd(328).
	p(NT(204), (NT(328)));
//G474: __E_cli_command_141(331) => wff(14).
	p(NT(331), (NT(14)));
//G475: __E_cli_command_141(331) => bf(21).
	p(NT(331), (NT(21)));
//G476: history_store_cmd(330) => __E_cli_command_141(331).
	p(NT(330), (NT(331)));
//G477: cli_command(204)     => history_store_cmd(330).
	p(NT(204), (NT(330)));
//G478: __E_input_stream_142(332) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(333) _(11) ')'.
	p(NT(332), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(333)+NT(11)+T(16)));
//G479: input_stream(320)    => __E_input_stream_142(332).
	p(NT(320), (NT(332)));
//G480: console_sym(334)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(334), (T(8)+T(17)+T(51)+T(12)+T(17)+T(6)+T(19)));
//G481: input_stream(320)    => console_sym(334).
	p(NT(320), (NT(334)));
//G482: __E_output_stream_143(335) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(333) _(11) ')'.
	p(NT(335), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(333)+NT(11)+T(16)));
//G483: output_stream(323)   => __E_output_stream_143(335).
	p(NT(323), (NT(335)));
//G484: output_stream(323)   => console_sym(334).
	p(NT(323), (NT(334)));
//G485: q_file_name(333)     => '"' file_name(336) '"'.
	p(NT(333), (T(58)+NT(336)+T(58)));
//G486: __E_file_name_144(337) => printable(8).
	p(NT(337), (NT(8)));
//G487: __E_file_name_144(337) => printable(8) __E_file_name_144(337).
	p(NT(337), (NT(8)+NT(337)));
//G488: file_name(336)       => __E_file_name_144(337).
	p(NT(336), (NT(337)));
//G489: number(292)          => digits(177).
	p(NT(292), (NT(177)));
//G490: bf_cmd_arg(338)      => memory(246).
	p(NT(338), (NT(246)));
//G491: bf_cmd_arg(338)      => bf(21).
	p(NT(338), (NT(21)));
//G492: wff_cmd_arg(297)     => memory(246).
	p(NT(297), (NT(246)));
//G493: wff_cmd_arg(297)     => wff(14).
	p(NT(297), (NT(14)));
//G494: solve_cmd_arg(240)   => memory(246).
	p(NT(240), (NT(246)));
//G495: solve_cmd_arg(240)   => wff(14).
	p(NT(240), (NT(14)));
//G496: nf_cmd_arg(255)      => memory(246).
	p(NT(255), (NT(246)));
//G497: nf_cmd_arg(255)      => ref(18).
	p(NT(255), (NT(18)));
//G498: nf_cmd_arg(255)      => wff(14).
	p(NT(255), (NT(14)));
//G499: nf_cmd_arg(255)      => bf(21).
	p(NT(255), (NT(21)));
//G500: onf_cmd_arg(285)     => memory(246).
	p(NT(285), (NT(246)));
//G501: onf_cmd_arg(285)     => wff(14).
	p(NT(285), (NT(14)));
//G502: normalize_cmd_arg(229) => memory(246).
	p(NT(229), (NT(246)));
//G503: normalize_cmd_arg(229) => rr(10).
	p(NT(229), (NT(10)));
//G504: normalize_cmd_arg(229) => ref(18).
	p(NT(229), (NT(18)));
//G505: normalize_cmd_arg(229) => wff(14).
	p(NT(229), (NT(14)));
//G506: normalize_cmd_arg(229) => bf(21).
	p(NT(229), (NT(21)));
//G507: inst_args(260)       => wff_cmd_arg(297) _(11) '[' _(11) variable(118) _(11) '/' _(11) bf_cmd_arg(338) _(11) ']'.
	p(NT(260), (NT(297)+NT(11)+T(24)+NT(11)+NT(118)+NT(11)+T(56)+NT(11)+NT(338)+NT(11)+T(25)));
//G508: inst_args(260)       => bf_cmd_arg(338) _(11) '[' _(11) variable(118) _(11) '/' _(11) bf_cmd_arg(338) _(11) ']'.
	p(NT(260), (NT(338)+NT(11)+T(24)+NT(11)+NT(118)+NT(11)+T(56)+NT(11)+NT(338)+NT(11)+T(25)));
//G509: help_arg(221)        => help_sym(218).
	p(NT(221), (NT(218)));
//G510: help_arg(221)        => version_sym(211).
	p(NT(221), (NT(211)));
//G511: help_arg(221)        => quit_sym(208).
	p(NT(221), (NT(208)));
//G512: help_arg(221)        => clear_sym(214).
	p(NT(221), (NT(214)));
//G513: help_arg(221)        => get_sym(300).
	p(NT(221), (NT(300)));
//G514: help_arg(221)        => set_sym(305).
	p(NT(221), (NT(305)));
//G515: help_arg(221)        => enable_sym(310).
	p(NT(221), (NT(310)));
//G516: help_arg(221)        => disable_sym(314).
	p(NT(221), (NT(314)));
//G517: help_arg(221)        => toggle_sym(317).
	p(NT(221), (NT(317)));
//G518: help_arg(221)        => file_sym(224).
	p(NT(221), (NT(224)));
//G519: help_arg(221)        => history_sym(326).
	p(NT(221), (NT(326)));
//G520: help_arg(221)        => abs_memory_sym(339).
	p(NT(221), (NT(339)));
//G521: help_arg(221)        => rel_memory_sym(340).
	p(NT(221), (NT(340)));
//G522: help_arg(221)        => selection_sym(341).
	p(NT(221), (NT(341)));
//G523: help_arg(221)        => def_sym(288).
	p(NT(221), (NT(288)));
//G524: help_arg(221)        => inst_sym(258).
	p(NT(221), (NT(258)));
//G525: help_arg(221)        => subst_sym(253).
	p(NT(221), (NT(253)));
//G526: help_arg(221)        => normalize_sym(249).
	p(NT(221), (NT(249)));
//G527: help_arg(221)        => execute_sym(342).
	p(NT(221), (NT(342)));
//G528: help_arg(221)        => solve_sym(238).
	p(NT(221), (NT(238)));
//G529: help_arg(221)        => valid_sym(228).
	p(NT(221), (NT(228)));
//G530: help_arg(221)        => sat_sym(232).
	p(NT(221), (NT(232)));
//G531: help_arg(221)        => unsat_sym(235).
	p(NT(221), (NT(235)));
//G532: help_arg(221)        => run_sym(243).
	p(NT(221), (NT(243)));
//G533: help_arg(221)        => dnf_sym(263).
	p(NT(221), (NT(263)));
//G534: help_arg(221)        => cnf_sym(266).
	p(NT(221), (NT(266)));
//G535: help_arg(221)        => anf_sym(269).
	p(NT(221), (NT(269)));
//G536: help_arg(221)        => snf_sym(281).
	p(NT(221), (NT(281)));
//G537: help_arg(221)        => nnf_sym(272).
	p(NT(221), (NT(272)));
//G538: help_arg(221)        => pnf_sym(275).
	p(NT(221), (NT(275)));
//G539: help_arg(221)        => mnf_sym(278).
	p(NT(221), (NT(278)));
//G540: help_arg(221)        => onf_sym(284).
	p(NT(221), (NT(284)));
//G541: help_arg(221)        => qelim_sym(296).
	p(NT(221), (NT(296)));
//G542: __E___E_help_arg_145_146(345) => 's'.
	p(NT(345), (T(12)));
//G543: __E___E_help_arg_145_146(345) => null.
	p(NT(345), (nul));
//G544: __E_help_arg_145(344) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_145_146(345).
	p(NT(344), (T(19)+T(27)+T(5)+T(18)+T(53)+T(6)+T(19)+NT(345)));
//G545: examples_sym(343)    => __E_help_arg_145(344).
	p(NT(343), (NT(344)));
//G546: help_arg(221)        => examples_sym(343).
	p(NT(221), (NT(343)));
//G547: __E_memory_147(347)  => '%'.
	p(NT(347), (T(59)));
//G548: rel_memory_sym(340)  => '%' '-'.
	p(NT(340), (T(59)+T(28)));
//G549: memory_id(349)       => digits(177).
	p(NT(349), (NT(177)));
//G550: __E___E_memory_147_148(348) => memory_id(349).
	p(NT(348), (NT(349)));
//G551: __E___E_memory_147_148(348) => null.
	p(NT(348), (nul));
//G552: __E_memory_147(347)  => rel_memory_sym(340) __E___E_memory_147_148(348).
	p(NT(347), (NT(340)+NT(348)));
//G553: rel_memory(346)      => __E_memory_147(347).
	p(NT(346), (NT(347)));
//G554: memory(246)          => rel_memory(346).
	p(NT(246), (NT(346)));
//G555: abs_memory_sym(339)  => '%'.
	p(NT(339), (T(59)));
//G556: __E_memory_149(351)  => abs_memory_sym(339) memory_id(349).
	p(NT(351), (NT(339)+NT(349)));
//G557: abs_memory(350)      => __E_memory_149(351).
	p(NT(350), (NT(351)));
//G558: memory(246)          => abs_memory(350).
	p(NT(246), (NT(350)));
//G559: option(302)          => bool_option(311).
	p(NT(302), (NT(311)));
//G560: __E_option_150(354)  => 'S'.
	p(NT(354), (T(35)));
//G561: __E_option_150(354)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(354), (T(12)+T(19)+T(50)+T(19)+T(11)+T(10)+T(13)+T(23)));
//G562: severity_opt(353)    => __E_option_150(354).
	p(NT(353), (NT(354)));
//G563: enum_option(352)     => severity_opt(353).
	p(NT(352), (NT(353)));
//G564: option(302)          => enum_option(352).
	p(NT(302), (NT(352)));
//G565: __E_bool_option_151(356) => 's'.
	p(NT(356), (T(12)));
//G566: __E_bool_option_151(356) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(356), (T(12)+T(13)+T(5)+T(13)+T(49)+T(12)));
//G567: status_opt(355)      => __E_bool_option_151(356).
	p(NT(355), (NT(356)));
//G568: bool_option(311)     => status_opt(355).
	p(NT(311), (NT(355)));
//G569: __E_bool_option_152(358) => 'c'.
	p(NT(358), (T(8)));
//G570: __E_bool_option_152(358) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(358), (T(8)+T(17)+T(6)+T(17)+T(11)));
//G571: __E_bool_option_152(358) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(358), (T(8)+T(17)+T(6)+T(17)+T(11)+T(12)));
//G572: colors_opt(357)      => __E_bool_option_152(358).
	p(NT(357), (NT(358)));
//G573: bool_option(311)     => colors_opt(357).
	p(NT(311), (NT(357)));
//G574: __E_bool_option_153(360) => 'V'.
	p(NT(360), (T(60)));
//G575: __E_bool_option_153(360) => 'c' 'h' 'a' 'r' 'v' 'a' 'r'.
	p(NT(360), (T(8)+T(52)+T(5)+T(11)+T(50)+T(5)+T(11)));
//G576: charvar_opt(359)     => __E_bool_option_153(360).
	p(NT(359), (NT(360)));
//G577: bool_option(311)     => charvar_opt(359).
	p(NT(311), (NT(359)));
//G578: __E_bool_option_154(362) => 'H'.
	p(NT(362), (T(61)));
//G579: __E_bool_option_154(362) => 'h' 'i' 'g' 'h' 'l' 'i' 'g' 'h' 't'.
	p(NT(362), (T(52)+T(10)+T(57)+T(52)+T(6)+T(10)+T(57)+T(52)+T(13)));
//G580: __E_bool_option_154(362) => 'h' 'i' 'g' 'h' 'l' 'i' 'g' 'h' 't' 'i' 'n' 'g'.
	p(NT(362), (T(52)+T(10)+T(57)+T(52)+T(6)+T(10)+T(57)+T(52)+T(13)+T(10)+T(51)+T(57)));
//G581: highlighting_opt(361) => __E_bool_option_154(362).
	p(NT(361), (NT(362)));
//G582: bool_option(311)     => highlighting_opt(361).
	p(NT(311), (NT(361)));
//G583: __E_bool_option_155(364) => 'I'.
	p(NT(364), (T(62)));
//G584: __E_bool_option_155(364) => 'i' 'n' 'd' 'e' 'n' 't'.
	p(NT(364), (T(10)+T(51)+T(54)+T(19)+T(51)+T(13)));
//G585: __E_bool_option_155(364) => 'i' 'n' 'd' 'e' 'n' 't' 'i' 'n' 'g'.
	p(NT(364), (T(10)+T(51)+T(54)+T(19)+T(51)+T(13)+T(10)+T(51)+T(57)));
//G586: indenting_opt(363)   => __E_bool_option_155(364).
	p(NT(363), (NT(364)));
//G587: bool_option(311)     => indenting_opt(363).
	p(NT(311), (NT(363)));
//G588: __E_bool_option_156(366) => 'd'.
	p(NT(366), (T(54)));
//G589: __E_bool_option_156(366) => 'd' 'b' 'g'.
	p(NT(366), (T(54)+T(7)+T(57)));
//G590: __E_bool_option_156(366) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(366), (T(54)+T(19)+T(7)+T(49)+T(57)));
//G591: debug_repl_opt(365)  => __E_bool_option_156(366).
	p(NT(365), (NT(366)));
//G592: bool_option(311)     => debug_repl_opt(365).
	p(NT(311), (NT(365)));
//G593: option_value(307)    => option_value_true(367).
	p(NT(307), (NT(367)));
//G594: option_value(307)    => option_value_false(368).
	p(NT(307), (NT(368)));
//G595: option_value(307)    => severity(369).
	p(NT(307), (NT(369)));
//G596: option_value_true(367) => 't'.
	p(NT(367), (T(13)));
//G597: option_value_true(367) => 't' 'r' 'u' 'e'.
	p(NT(367), (T(13)+T(11)+T(49)+T(19)));
//G598: option_value_true(367) => 'o' 'n'.
	p(NT(367), (T(17)+T(51)));
//G599: option_value_true(367) => '1'.
	p(NT(367), (T(37)));
//G600: option_value_true(367) => 'y'.
	p(NT(367), (T(23)));
//G601: option_value_true(367) => 'y' 'e' 's'.
	p(NT(367), (T(23)+T(19)+T(12)));
//G602: option_value_false(368) => 'f'.
	p(NT(368), (T(4)));
//G603: option_value_false(368) => 'f' 'a' 'l' 's' 'e'.
	p(NT(368), (T(4)+T(5)+T(6)+T(12)+T(19)));
//G604: option_value_false(368) => 'o' 'f' 'f'.
	p(NT(368), (T(17)+T(4)+T(4)));
//G605: option_value_false(368) => '0'.
	p(NT(368), (T(38)));
//G606: option_value_false(368) => 'n'.
	p(NT(368), (T(51)));
//G607: option_value_false(368) => 'n' 'o'.
	p(NT(368), (T(51)+T(17)));
//G608: __E_severity_157(371) => 'e'.
	p(NT(371), (T(19)));
//G609: __E_severity_157(371) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(371), (T(19)+T(11)+T(11)+T(17)+T(11)));
//G610: error_sym(370)       => __E_severity_157(371).
	p(NT(370), (NT(371)));
//G611: severity(369)        => error_sym(370).
	p(NT(369), (NT(370)));
//G612: __E_severity_158(373) => 'i'.
	p(NT(373), (T(10)));
//G613: __E_severity_158(373) => 'i' 'n' 'f' 'o'.
	p(NT(373), (T(10)+T(51)+T(4)+T(17)));
//G614: info_sym(372)        => __E_severity_158(373).
	p(NT(372), (NT(373)));
//G615: severity(369)        => info_sym(372).
	p(NT(369), (NT(372)));
//G616: __E_severity_159(375) => 'd'.
	p(NT(375), (T(54)));
//G617: __E_severity_159(375) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(375), (T(54)+T(19)+T(7)+T(49)+T(57)));
//G618: debug_sym(374)       => __E_severity_159(375).
	p(NT(374), (NT(375)));
//G619: severity(369)        => debug_sym(374).
	p(NT(369), (NT(374)));
//G620: __E_severity_160(377) => 't'.
	p(NT(377), (T(13)));
//G621: __E_severity_160(377) => 't' 'r' 'a' 'c' 'e'.
	p(NT(377), (T(13)+T(11)+T(5)+T(8)+T(19)));
//G622: trace_sym(376)       => __E_severity_160(377).
	p(NT(376), (NT(377)));
//G623: severity(369)        => trace_sym(376).
	p(NT(369), (NT(376)));
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
		rule, __E___E___E_library_9_10_11, wff_rule, bf_rule, __E___E_library_9_12, wff_matcher, wff_body, __E_wff_rule_13, bf_matcher, bf_body, 
		__E_bf_rule_14, builder, builder_head, builder_body, __E_builder_head_15, __E_builder_head_16, bf_builder_body, __E_builder_body_17, wff_builder_body, __E_builder_body_18, 
		tau_constant_source, __E_tau_constant_source_19, wff_parenthesis, __E_wff_20, wff_sometimes, __E_wff_21, __E___E_wff_21_22, wff_always, __E_wff_23, __E___E_wff_23_24, 
		wff_conditional, __E_wff_25, wff_all, __E_wff_26, q_vars, wff_ex, __E_wff_27, wff_ref, constraint, wff_imply, 
		__E_wff_28, wff_equiv, __E_wff_29, wff_or, __E_wff_30, wff_xor, __E_wff_31, wff_and, __E_wff_32, wff_neg, 
		__E_wff_33, wff_t, wff_f, bf_interval, __E_wff_34, bf_eq, __E_wff_35, bf_neq, __E_wff_36, bf_less_equal, 
		__E_wff_37, bf_nleq, __E_wff_38, bf_greater, __E_wff_39, bf_ngreater, __E_wff_40, bf_greater_equal, __E_wff_41, bf_ngeq, 
		__E_wff_42, bf_less, __E_wff_43, bf_nless, __E_wff_44, bf_parenthesis, __E_bf_45, bf_constant, variable, bf_splitter, 
		__E_bf_46, bf_ref, bf_or, __E_bf_47, bf_xor, __E_bf_48, bf_t, __E_bf_49, __E___E_bf_49_50, type, 
		bf_f, __E_bf_51, __E___E_bf_51_52, bf_and, __E_bf_53, __E___E_bf_53_54, bf_and_nosep, bf_neg, __E_bf_55, __E_bf_and_nosep_56, 
		ctn_neq, __E_constraint_57, ctnvar, num, ctn_eq, __E_constraint_58, ctn_greater_equal, __E_constraint_59, ctn_greater, __E_constraint_60, 
		ctn_less_equal, __E_constraint_61, ctn_less, __E_constraint_62, __E_variable_63, chars, io_var, __E_variable_64, in, out, 
		uninterpreted_constant, in_var_name, __E_in_65, __E___E_in_65_66, offset, out_var_name, __E_out_67, __E___E_out_67_68, q_var, __E_q_vars_69, 
		__E_q_vars_70, __E_offsets_71, __E_offsets_72, shift, __E_offset_73, __E_shift_74, __E___E_shift_74_75, digits, uninter_const_name, __E_uninterpreted_constant_76, 
		__E___E_uninterpreted_constant_76_77, constant, __E_bf_constant_78, binding, __E_bf_constant_79, source, __E_binding_80, src_c, __E___E_binding_80_81, __E___E___E_binding_80_81_82, 
		__E___E___E_binding_80_81_83, __E_src_c_84, __E_src_c_85, __E_chars_86, __E_chars_87, __E_digits_88, extra, comment, __E_comment_89, __E_comment_90, 
		__E_comment_91, __E____92, __E___93, cli, cli_command, __E_cli_94, __E_cli_95, quit_cmd, quit_sym, __E_cli_command_96, 
		version_cmd, version_sym, __E_cli_command_97, clear_cmd, clear_sym, __E_cli_command_98, help_cmd, __E_cli_command_99, help_sym, __E___E_cli_command_99_100, 
		__E___E_cli_command_99_101, help_arg, file_cmd, __E_cli_command_102, file_sym, q_string, valid_cmd, __E_cli_command_103, valid_sym, normalize_cmd_arg, 
		sat_cmd, __E_cli_command_104, sat_sym, unsat_cmd, __E_cli_command_105, unsat_sym, solve_cmd, __E_cli_command_106, solve_sym, __E___E_cli_command_106_107, 
		solve_cmd_arg, run_cmd, __E_cli_command_108, run_sym, __E___E_cli_command_108_109, __E___E_cli_command_108_110, memory, normalize_cmd, __E_cli_command_111, normalize_sym, 
		__E___E_cli_command_111_112, subst_cmd, __E_cli_command_113, subst_sym, __E___E_cli_command_113_114, nf_cmd_arg, inst_cmd, __E_cli_command_115, inst_sym, __E___E_cli_command_115_116, 
		inst_args, dnf_cmd, __E_cli_command_117, dnf_sym, cnf_cmd, __E_cli_command_118, cnf_sym, anf_cmd, __E_cli_command_119, anf_sym, 
		nnf_cmd, __E_cli_command_120, nnf_sym, pnf_cmd, __E_cli_command_121, pnf_sym, mnf_cmd, __E_cli_command_122, mnf_sym, snf_cmd, 
		__E_cli_command_123, snf_sym, onf_cmd, __E_cli_command_124, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_125, def_sym, __E___E_cli_command_125_126, 
		def_print_cmd, __E_cli_command_127, number, def_rr_cmd, qelim_cmd, __E_cli_command_128, qelim_sym, wff_cmd_arg, get_cmd, __E_cli_command_129, 
		get_sym, __E___E_cli_command_129_130, option, set_cmd, __E_cli_command_131, set_sym, __E___E_cli_command_131_132, option_value, enable_cmd, __E_cli_command_133, 
		enable_sym, bool_option, disable_cmd, __E_cli_command_134, disable_sym, toggle_cmd, __E_cli_command_135, toggle_sym, def_input_cmd, __E_cli_command_136, 
		input_stream, def_output_cmd, __E_cli_command_137, output_stream, history_list_cmd, __E_cli_command_138, history_sym, __E___E_cli_command_138_139, history_print_cmd, __E_cli_command_140, 
		history_store_cmd, __E_cli_command_141, __E_input_stream_142, q_file_name, console_sym, __E_output_stream_143, file_name, __E_file_name_144, bf_cmd_arg, abs_memory_sym, 
		rel_memory_sym, selection_sym, execute_sym, examples_sym, __E_help_arg_145, __E___E_help_arg_145_146, rel_memory, __E_memory_147, __E___E_memory_147_148, memory_id, 
		abs_memory, __E_memory_149, enum_option, severity_opt, __E_option_150, status_opt, __E_bool_option_151, colors_opt, __E_bool_option_152, charvar_opt, 
		__E_bool_option_153, highlighting_opt, __E_bool_option_154, indenting_opt, __E_bool_option_155, debug_repl_opt, __E_bool_option_156, option_value_true, option_value_false, severity, 
		error_sym, __E_severity_157, info_sym, __E_severity_158, debug_sym, __E_severity_159, trace_sym, __E_severity_160, named_binding, __N_0, 
		__N_1, __N_2, __N_3, __N_4, __N_5, __N_6, __N_7, __N_8, 
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
