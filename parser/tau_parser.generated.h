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
	"__E_start_0", "inputs", "builder", "library", "rr", "_", "rec_relations", "main", "wff", "__E_rec_relations_1", 
	"rec_relation", "__E_rec_relations_2", "ref", "__E_rec_relation_3", "capture", "bf", "sym", "__E_ref_4", "offsets", "ref_args", 
	"__E_ref_args_5", "__E___E_ref_args_5_6", "ref_arg", "__E___E_ref_args_5_7", "__E___E_ref_args_5_8", "__E_inputs_9", "__E_inputs_10", "input", "in", "source_binding", 
	"rules", "__E_library_11", "__E___E_library_11_12", "rule", "__E___E___E_library_11_12_13", "wff_rule", "bf_rule", "__E___E_library_11_14", "wff_matcher", "wff_body", 
	"__E_wff_rule_15", "wff_cb", "bf_matcher", "bf_body", "__E_bf_rule_16", "bf_cb", "builder_head", "builder_body", "__E_builder_head_17", "__", 
	"__E_builder_head_18", "bf_builder_body", "__E_builder_body_19", "wff_builder_body", "__E_builder_body_20", "wff_parenthesis", "__E_wff_21", "wff_sometimes", "__E_wff_22", "__E___E_wff_22_23", 
	"wff_always", "__E_wff_24", "__E___E_wff_24_25", "wff_conditional", "__E_wff_26", "wff_all", "__E_wff_27", "q_vars", "wff_ex", "__E_wff_28", 
	"wff_ref", "wff_imply", "__E_wff_29", "wff_equiv", "__E_wff_30", "wff_or", "__E_wff_31", "wff_xor", "__E_wff_32", "wff_and", 
	"__E_wff_33", "wff_neg", "__E_wff_34", "wff_t", "wff_f", "bf_interval", "__E_wff_35", "bf_neq", "__E_wff_36", "bf_eq", 
	"__E_wff_37", "bf_nleq", "__E_wff_38", "bf_greater", "__E_wff_39", "bf_less_equal", "__E_wff_40", "bf_less", "__E_wff_41", "uninterpreted_constant", 
	"bf_parenthesis", "__E_bf_42", "variable", "bf_splitter", "__E_bf_43", "bf_ref", "bf_or", "__E_bf_44", "bf_xor", "__E_bf_45", 
	"bf_and", "__E_bf_46", "__E___E_bf_46_47", "bf_neg", "__E_bf_48", "bf_constant", "__E_bf_49", "constant", "bf_t", "bf_f", 
	"charvar", "__E_charvar_50", "__E_charvar_51", "io_var", "__E_variable_52", "out", "in_var_name", "__E_in_53", "digits", "offset", 
	"out_var_name", "__E_out_54", "q_var", "__E_q_vars_55", "__E_q_vars_56", "__E_offsets_57", "__E_offsets_58", "num", "shift", "__E_offset_59", 
	"__E_shift_60", "__E___E_shift_60_61", "uninter_const_name", "__E_uninterpreted_constant_62", "__E___E_uninterpreted_constant_62_63", "chars", "binding", "__E_binding_64", "type", "__E___E_binding_64_65", 
	"source", "named_binding", "src_c", "__E_source_66", "__E_source_67", "__E___E_source_67_68", "__E___E_source_67_69", "__E_chars_70", "__E_digits_71", "comment", 
	"__E_comment_72", "__E_comment_73", "__E_comment_74", "__E____75", "__E___76", "bf_cb_args1", "bf_cb_arg", "bf_cb_args2", "bf_cb_args3", "bf_cb_args4", 
	"wff_cb_args1", "wff_cb_arg", "wff_cb_args2", "wff_cb_args3", "wff_cb_args4", "bf_eq_cb", "__E_wff_cb_77", "bf_neq_cb", "__E_wff_cb_78", "wff_has_clashing_subformulas_cb", 
	"__E_wff_cb_79", "wff_has_subformula_cb", "__E_wff_cb_80", "wff_remove_existential_cb", "__E_wff_cb_81", "bf_is_zero_cb", "__E_bf_cb_82", "bf_is_one_cb", "__E_bf_cb_83", "bf_has_subformula_cb", 
	"__E_bf_cb_84", "bf_remove_funiversal_cb", "__E_bf_cb_85", "bf_remove_fexistential_cb", "__E_bf_cb_86", "bf_and_cb", "__E_bf_cb_87", "bf_or_cb", "__E_bf_cb_88", "bf_xor_cb", 
	"__E_bf_cb_89", "bf_neg_cb", "__E_bf_cb_90", "bf_normalize_cb", "__E_bf_cb_91", "cli", "cli_command", "__E_cli_92", "__E_cli_93", "quit_cmd", 
	"quit_sym", "__E_cli_command_94", "version_cmd", "version_sym", "__E_cli_command_95", "clear_cmd", "clear_sym", "__E_cli_command_96", "help_cmd", "__E_cli_command_97", 
	"help_sym", "__E___E_cli_command_97_98", "__E___E_cli_command_97_99", "help_arg", "file_cmd", "__E_cli_command_100", "file_sym", "q_string", "valid_cmd", "__E_cli_command_101", 
	"valid_sym", "sat_cmd", "__E_cli_command_102", "sat_sym", "unsat_cmd", "__E_cli_command_103", "unsat_sym", "solve_cmd", "__E_cli_command_104", "solve_sym", 
	"wff_cmd_arg", "execute_cmd", "__E_cli_command_105", "execute_sym", "__E___E_cli_command_105_106", "__E___E_cli_command_105_107", "wff_typed", "normalize_cmd", "__E_cli_command_108", "normalize_sym", 
	"__E___E_cli_command_108_109", "normalize_cmd_arg", "subst_cmd", "__E_cli_command_110", "subst_sym", "__E___E_cli_command_110_111", "nf_cmd_arg", "inst_cmd", "__E_cli_command_112", "inst_sym", 
	"__E___E_cli_command_112_113", "inst_args", "dnf_cmd", "__E_cli_command_114", "dnf_sym", "cnf_cmd", "__E_cli_command_115", "cnf_sym", "anf_cmd", "__E_cli_command_116", 
	"anf_sym", "nnf_cmd", "__E_cli_command_117", "nnf_sym", "pnf_cmd", "__E_cli_command_118", "pnf_sym", "mnf_cmd", "__E_cli_command_119", "mnf_sym", 
	"snf_cmd", "__E_cli_command_120", "snf_sym", "onf_cmd", "__E_cli_command_121", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_122", "def_sym", 
	"__E___E_cli_command_122_123", "def_print_cmd", "__E_cli_command_124", "number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_125", "qelim_sym", "get_cmd", "__E_cli_command_126", 
	"get_sym", "__E___E_cli_command_126_127", "option", "set_cmd", "__E_cli_command_128", "set_sym", "__E___E_cli_command_128_129", "option_value", "toggle_cmd", "__E_cli_command_130", 
	"toggle_sym", "bool_option", "history_list_cmd", "__E_cli_command_131", "history_sym", "__E___E_cli_command_131_132", "history_print_cmd", "__E_cli_command_133", "memory", "history_store_cmd", 
	"__E_cli_command_134", "bf_typed", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "examples_sym", "__E_help_arg_135", "__E___E_help_arg_135_136", "rel_memory", 
	"__E_memory_137", "__E___E_memory_137_138", "memory_id", "abs_memory", "__E_memory_139", "enum_option", "severity_opt", "__E_option_140", "status_opt", "__E_bool_option_141", 
	"colors_opt", "__E_bool_option_142", "debug_repl_opt", "__E_bool_option_143", "option_value_true", "option_value_false", "severity", "error_sym", "__E_severity_144", "info_sym", 
	"__E_severity_145", "debug_sym", "__E_severity_146", "trace_sym", "__E_severity_147", "__N_0", "__N_1", "__N_2", "__N_3", 
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
			15, 59
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			24, 26, 130, 136, 138, 140, 152, 158, 160, 161
		},
		.to_inline = {
			{ 18, 65, 18 },
			{ 25, 110, 25 },
			{ 51 },
			{ 55 },
			{ 142 },
			{ 155 },
			{ 162 },
			{ 175 },
			{ 177 },
			{ 178 },
			{ 179 },
			{ 180 },
			{ 182 },
			{ 183 },
			{ 184 },
			{ 250 },
			{ 256 },
			{ 261 },
			{ 266 },
			{ 271 },
			{ 296 },
			{ 331 },
			{ 332 }
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
//G0:   __E_start_0(10)      => inputs(11).
	p(NT(10), (NT(11)));
//G1:   __E_start_0(10)      => builder(12).
	p(NT(10), (NT(12)));
//G2:   __E_start_0(10)      => library(13).
	p(NT(10), (NT(13)));
//G3:   __E_start_0(10)      => rr(14).
	p(NT(10), (NT(14)));
//G4:   start(9)             => __E_start_0(10) _(15).
	p(NT(9), (NT(10)+NT(15)));
//G5:   main(17)             => wff(18).
	p(NT(17), (NT(18)));
//G6:   rr(14)               => rec_relations(16) _(15) main(17) _(15) '.'.
	p(NT(14), (NT(16)+NT(15)+NT(17)+NT(15)+T(1)));
//G7:   __E_rec_relations_1(19) => _(15) rec_relation(20) _(15) '.'.
	p(NT(19), (NT(15)+NT(20)+NT(15)+T(1)));
//G8:   __E_rec_relations_2(21) => null.
	p(NT(21), (nul));
//G9:   __E_rec_relations_2(21) => __E_rec_relations_1(19) __E_rec_relations_2(21).
	p(NT(21), (NT(19)+NT(21)));
//G10:  rec_relations(16)    => __E_rec_relations_2(21).
	p(NT(16), (NT(21)));
//G11:  __E_rec_relation_3(23) => capture(24).
	p(NT(23), (NT(24)));
//G12:  __E_rec_relation_3(23) => ref(22).
	p(NT(23), (NT(22)));
//G13:  __E_rec_relation_3(23) => wff(18).
	p(NT(23), (NT(18)));
//G14:  __E_rec_relation_3(23) => bf(25).
	p(NT(23), (NT(25)));
//G15:  rec_relation(20)     => ref(22) _(15) ':' '=' _(15) __E_rec_relation_3(23).
	p(NT(20), (NT(22)+NT(15)+T(2)+T(3)+NT(15)+NT(23)));
//G16:  __E_ref_4(27)        => _(15) offsets(28).
	p(NT(27), (NT(15)+NT(28)));
//G17:  __E_ref_4(27)        => null.
	p(NT(27), (nul));
//G18:  ref(22)              => sym(26) __E_ref_4(27) _(15) ref_args(29).
	p(NT(22), (NT(26)+NT(27)+NT(15)+NT(29)));
//G19:  ref_arg(32)          => bf(25).
	p(NT(32), (NT(25)));
//G20:  __E___E_ref_args_5_6(31) => _(15) ref_arg(32).
	p(NT(31), (NT(15)+NT(32)));
//G21:  __E___E_ref_args_5_7(33) => _(15) ',' _(15) ref_arg(32).
	p(NT(33), (NT(15)+T(4)+NT(15)+NT(32)));
//G22:  __E___E_ref_args_5_8(34) => null.
	p(NT(34), (nul));
//G23:  __E___E_ref_args_5_8(34) => __E___E_ref_args_5_7(33) __E___E_ref_args_5_8(34).
	p(NT(34), (NT(33)+NT(34)));
//G24:  __E_ref_args_5(30)   => __E___E_ref_args_5_6(31) __E___E_ref_args_5_8(34).
	p(NT(30), (NT(31)+NT(34)));
//G25:  __E_ref_args_5(30)   => null.
	p(NT(30), (nul));
//G26:  ref_args(29)         => '(' __E_ref_args_5(30) _(15) ')'.
	p(NT(29), (T(5)+NT(30)+NT(15)+T(6)));
//G27:  __E_inputs_10(36)    => _(15) input(37).
	p(NT(36), (NT(15)+NT(37)));
//G28:  __E_inputs_9(35)     => __E_inputs_10(36).
	p(NT(35), (NT(36)));
//G29:  __E_inputs_9(35)     => __E_inputs_10(36) __E_inputs_9(35).
	p(NT(35), (NT(36)+NT(35)));
//G30:  inputs(11)           => _(15) '<' __E_inputs_9(35) _(15) '.'.
	p(NT(11), (NT(15)+T(7)+NT(35)+NT(15)+T(1)));
//G31:  input(37)            => in(38) _(15) ':' _(15) '{' _(15) source_binding(39) _(15) '}'.
	p(NT(37), (NT(38)+NT(15)+T(2)+NT(15)+T(8)+NT(15)+NT(39)+NT(15)+T(9)));
//G32:  __E___E___E_library_11_12_13(44) => wff_rule(45).
	p(NT(44), (NT(45)));
//G33:  __E___E___E_library_11_12_13(44) => bf_rule(46).
	p(NT(44), (NT(46)));
//G34:  rule(43)             => __E___E___E_library_11_12_13(44).
	p(NT(43), (NT(44)));
//G35:  __E___E_library_11_12(42) => _(15) rule(43).
	p(NT(42), (NT(15)+NT(43)));
//G36:  __E___E_library_11_14(47) => null.
	p(NT(47), (nul));
//G37:  __E___E_library_11_14(47) => __E___E_library_11_12(42) __E___E_library_11_14(47).
	p(NT(47), (NT(42)+NT(47)));
//G38:  __E_library_11(41)   => __E___E_library_11_14(47).
	p(NT(41), (NT(47)));
//G39:  rules(40)            => __E_library_11(41).
	p(NT(40), (NT(41)));
//G40:  library(13)          => rules(40).
	p(NT(13), (NT(40)));
//G41:  wff_matcher(48)      => wff(18).
	p(NT(48), (NT(18)));
//G42:  __E_wff_rule_15(50)  => wff(18).
	p(NT(50), (NT(18)));
//G43:  __E_wff_rule_15(50)  => wff_cb(51).
	p(NT(50), (NT(51)));
//G44:  wff_body(49)         => __E_wff_rule_15(50).
	p(NT(49), (NT(50)));
//G45:  wff_rule(45)         => wff_matcher(48) _(15) ':' ':' '=' _(15) wff_body(49) _(15) '.'.
	p(NT(45), (NT(48)+NT(15)+T(2)+T(2)+T(3)+NT(15)+NT(49)+NT(15)+T(1)));
//G46:  bf_matcher(52)       => bf(25).
	p(NT(52), (NT(25)));
//G47:  __E_bf_rule_16(54)   => bf(25).
	p(NT(54), (NT(25)));
//G48:  __E_bf_rule_16(54)   => bf_cb(55).
	p(NT(54), (NT(55)));
//G49:  bf_body(53)          => __E_bf_rule_16(54).
	p(NT(53), (NT(54)));
//G50:  bf_rule(46)          => bf_matcher(52) _(15) ':' '=' _(15) bf_body(53) _(15) '.'.
	p(NT(46), (NT(52)+NT(15)+T(2)+T(3)+NT(15)+NT(53)+NT(15)+T(1)));
//G51:  builder(12)          => _(15) builder_head(56) _(15) builder_body(57) _(15) '.'.
	p(NT(12), (NT(15)+NT(56)+NT(15)+NT(57)+NT(15)+T(1)));
//G52:  __E_builder_head_17(58) => __(59) capture(24).
	p(NT(58), (NT(59)+NT(24)));
//G53:  __E_builder_head_18(60) => null.
	p(NT(60), (nul));
//G54:  __E_builder_head_18(60) => __E_builder_head_17(58) __E_builder_head_18(60).
	p(NT(60), (NT(58)+NT(60)));
//G55:  builder_head(56)     => '(' _(15) capture(24) __E_builder_head_18(60) _(15) ')'.
	p(NT(56), (T(5)+NT(15)+NT(24)+NT(60)+NT(15)+T(6)));
//G56:  __E_builder_body_19(62) => '=' ':' _(15) bf(25).
	p(NT(62), (T(3)+T(2)+NT(15)+NT(25)));
//G57:  bf_builder_body(61)  => __E_builder_body_19(62).
	p(NT(61), (NT(62)));
//G58:  builder_body(57)     => bf_builder_body(61).
	p(NT(57), (NT(61)));
//G59:  __E_builder_body_20(64) => '=' ':' ':' _(15) wff(18).
	p(NT(64), (T(3)+T(2)+T(2)+NT(15)+NT(18)));
//G60:  wff_builder_body(63) => __E_builder_body_20(64).
	p(NT(63), (NT(64)));
//G61:  builder_body(57)     => wff_builder_body(63).
	p(NT(57), (NT(63)));
//G62:  __E_wff_21(66)       => '(' _(15) wff(18) _(15) ')'.
	p(NT(66), (T(5)+NT(15)+NT(18)+NT(15)+T(6)));
//G63:  wff_parenthesis(65)  => __E_wff_21(66).
	p(NT(65), (NT(66)));
//G64:  wff(18)              => wff_parenthesis(65).
	p(NT(18), (NT(65)));
//G65:  __E___E_wff_22_23(69) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(69), (T(10)+T(11)+T(12)+T(13)+T(14)+T(15)+T(12)+T(13)+T(10)));
//G66:  __E___E_wff_22_23(69) => '<' '>'.
	p(NT(69), (T(7)+T(16)));
//G67:  __E_wff_22(68)       => __E___E_wff_22_23(69) _(15) wff(18).
	p(NT(68), (NT(69)+NT(15)+NT(18)));
//G68:  wff_sometimes(67)    => __E_wff_22(68).
	p(NT(67), (NT(68)));
//G69:  wff(18)              => wff_sometimes(67).
	p(NT(18), (NT(67)));
//G70:  __E___E_wff_24_25(72) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(72), (T(17)+T(18)+T(19)+T(17)+T(20)+T(10)));
//G71:  __E___E_wff_24_25(72) => '[' ']'.
	p(NT(72), (T(21)+T(22)));
//G72:  __E_wff_24(71)       => __E___E_wff_24_25(72) _(15) wff(18).
	p(NT(71), (NT(72)+NT(15)+NT(18)));
//G73:  wff_always(70)       => __E_wff_24(71).
	p(NT(70), (NT(71)));
//G74:  wff(18)              => wff_always(70).
	p(NT(18), (NT(70)));
//G75:  __E_wff_26(74)       => wff(18) _(15) '?' _(15) wff(18) _(15) ':' _(15) wff(18).
	p(NT(74), (NT(18)+NT(15)+T(23)+NT(15)+NT(18)+NT(15)+T(2)+NT(15)+NT(18)));
//G76:  wff_conditional(73)  => __E_wff_26(74).
	p(NT(73), (NT(74)));
//G77:  wff(18)              => wff_conditional(73).
	p(NT(18), (NT(73)));
//G78:  __E_wff_27(76)       => 'a' 'l' 'l' __(59) q_vars(77) __(59) wff(18).
	p(NT(76), (T(17)+T(18)+T(18)+NT(59)+NT(77)+NT(59)+NT(18)));
//G79:  wff_all(75)          => __E_wff_27(76).
	p(NT(75), (NT(76)));
//G80:  wff(18)              => wff_all(75).
	p(NT(18), (NT(75)));
//G81:  __E_wff_28(79)       => 'e' 'x' __(59) q_vars(77) __(59) wff(18).
	p(NT(79), (T(13)+T(24)+NT(59)+NT(77)+NT(59)+NT(18)));
//G82:  wff_ex(78)           => __E_wff_28(79).
	p(NT(78), (NT(79)));
//G83:  wff(18)              => wff_ex(78).
	p(NT(18), (NT(78)));
//G84:  wff_ref(80)          => ref(22).
	p(NT(80), (NT(22)));
//G85:  wff(18)              => wff_ref(80).
	p(NT(18), (NT(80)));
//G86:  __E_wff_29(82)       => wff(18) _(15) '-' '>' _(15) wff(18).
	p(NT(82), (NT(18)+NT(15)+T(25)+T(16)+NT(15)+NT(18)));
//G87:  wff_imply(81)        => __E_wff_29(82).
	p(NT(81), (NT(82)));
//G88:  wff(18)              => wff_imply(81).
	p(NT(18), (NT(81)));
//G89:  __E_wff_30(84)       => wff(18) _(15) '<' '-' '>' _(15) wff(18).
	p(NT(84), (NT(18)+NT(15)+T(7)+T(25)+T(16)+NT(15)+NT(18)));
//G90:  wff_equiv(83)        => __E_wff_30(84).
	p(NT(83), (NT(84)));
//G91:  wff(18)              => wff_equiv(83).
	p(NT(18), (NT(83)));
//G92:  __E_wff_31(86)       => wff(18) _(15) '|' '|' _(15) wff(18).
	p(NT(86), (NT(18)+NT(15)+T(26)+T(26)+NT(15)+NT(18)));
//G93:  wff_or(85)           => __E_wff_31(86).
	p(NT(85), (NT(86)));
//G94:  wff(18)              => wff_or(85).
	p(NT(18), (NT(85)));
//G95:  __E_wff_32(88)       => wff(18) _(15) '^' _(15) wff(18).
	p(NT(88), (NT(18)+NT(15)+T(27)+NT(15)+NT(18)));
//G96:  wff_xor(87)          => __E_wff_32(88).
	p(NT(87), (NT(88)));
//G97:  wff(18)              => wff_xor(87).
	p(NT(18), (NT(87)));
//G98:  __E_wff_33(90)       => wff(18) _(15) '&' '&' _(15) wff(18).
	p(NT(90), (NT(18)+NT(15)+T(28)+T(28)+NT(15)+NT(18)));
//G99:  wff_and(89)          => __E_wff_33(90).
	p(NT(89), (NT(90)));
//G100: wff(18)              => wff_and(89).
	p(NT(18), (NT(89)));
//G101: __E_wff_34(92)       => '!' _(15) wff(18).
	p(NT(92), (T(29)+NT(15)+NT(18)));
//G102: wff_neg(91)          => __E_wff_34(92).
	p(NT(91), (NT(92)));
//G103: wff(18)              => wff_neg(91).
	p(NT(18), (NT(91)));
//G104: wff_t(93)            => 'T'.
	p(NT(93), (T(30)));
//G105: wff(18)              => wff_t(93).
	p(NT(18), (NT(93)));
//G106: wff_f(94)            => 'F'.
	p(NT(94), (T(31)));
//G107: wff(18)              => wff_f(94).
	p(NT(18), (NT(94)));
//G108: __E_wff_35(96)       => bf(25) _(15) '<' '=' _(15) bf(25) _(15) '<' '=' _(15) bf(25).
	p(NT(96), (NT(25)+NT(15)+T(7)+T(3)+NT(15)+NT(25)+NT(15)+T(7)+T(3)+NT(15)+NT(25)));
//G109: bf_interval(95)      => __E_wff_35(96).
	p(NT(95), (NT(96)));
//G110: wff(18)              => bf_interval(95).
	p(NT(18), (NT(95)));
//G111: __E_wff_36(98)       => bf(25) _(15) '!' '=' _(15) bf(25).
	p(NT(98), (NT(25)+NT(15)+T(29)+T(3)+NT(15)+NT(25)));
//G112: bf_neq(97)           => __E_wff_36(98).
	p(NT(97), (NT(98)));
//G113: wff(18)              => bf_neq(97).
	p(NT(18), (NT(97)));
//G114: __E_wff_37(100)      => bf(25) _(15) '=' _(15) bf(25).
	p(NT(100), (NT(25)+NT(15)+T(3)+NT(15)+NT(25)));
//G115: bf_eq(99)            => __E_wff_37(100).
	p(NT(99), (NT(100)));
//G116: wff(18)              => bf_eq(99).
	p(NT(18), (NT(99)));
//G117: __E_wff_38(102)      => bf(25) _(15) '!' '<' '=' _(15) bf(25).
	p(NT(102), (NT(25)+NT(15)+T(29)+T(7)+T(3)+NT(15)+NT(25)));
//G118: bf_nleq(101)         => __E_wff_38(102).
	p(NT(101), (NT(102)));
//G119: wff(18)              => bf_nleq(101).
	p(NT(18), (NT(101)));
//G120: __E_wff_39(104)      => bf(25) _(15) '>' _(15) bf(25).
	p(NT(104), (NT(25)+NT(15)+T(16)+NT(15)+NT(25)));
//G121: bf_greater(103)      => __E_wff_39(104).
	p(NT(103), (NT(104)));
//G122: wff(18)              => bf_greater(103).
	p(NT(18), (NT(103)));
//G123: __E_wff_40(106)      => bf(25) _(15) '<' '=' _(15) bf(25).
	p(NT(106), (NT(25)+NT(15)+T(7)+T(3)+NT(15)+NT(25)));
//G124: bf_less_equal(105)   => __E_wff_40(106).
	p(NT(105), (NT(106)));
//G125: wff(18)              => bf_less_equal(105).
	p(NT(18), (NT(105)));
//G126: __E_wff_41(108)      => bf(25) _(15) '<' _(15) bf(25).
	p(NT(108), (NT(25)+NT(15)+T(7)+NT(15)+NT(25)));
//G127: bf_less(107)         => __E_wff_41(108).
	p(NT(107), (NT(108)));
//G128: wff(18)              => bf_less(107).
	p(NT(18), (NT(107)));
//G129: wff(18)              => capture(24).
	p(NT(18), (NT(24)));
//G130: wff(18)              => uninterpreted_constant(109).
	p(NT(18), (NT(109)));
//G131: __E_bf_42(111)       => '(' _(15) bf(25) _(15) ')'.
	p(NT(111), (T(5)+NT(15)+NT(25)+NT(15)+T(6)));
//G132: bf_parenthesis(110)  => __E_bf_42(111).
	p(NT(110), (NT(111)));
//G133: bf(25)               => bf_parenthesis(110).
	p(NT(25), (NT(110)));
//G134: bf(25)               => variable(112).
	p(NT(25), (NT(112)));
//G135: __E_bf_43(114)       => 'S' _(15) '(' _(15) bf(25) _(15) ')'.
	p(NT(114), (T(32)+NT(15)+T(5)+NT(15)+NT(25)+NT(15)+T(6)));
//G136: bf_splitter(113)     => __E_bf_43(114).
	p(NT(113), (NT(114)));
//G137: bf(25)               => bf_splitter(113).
	p(NT(25), (NT(113)));
//G138: bf_ref(115)          => ref(22).
	p(NT(115), (NT(22)));
//G139: bf(25)               => bf_ref(115).
	p(NT(25), (NT(115)));
//G140: __E_bf_44(117)       => bf(25) _(15) '|' _(15) bf(25).
	p(NT(117), (NT(25)+NT(15)+T(26)+NT(15)+NT(25)));
//G141: bf_or(116)           => __E_bf_44(117).
	p(NT(116), (NT(117)));
//G142: bf(25)               => bf_or(116).
	p(NT(25), (NT(116)));
//G143: __E_bf_45(119)       => bf(25) _(15) '+' _(15) bf(25).
	p(NT(119), (NT(25)+NT(15)+T(33)+NT(15)+NT(25)));
//G144: bf_xor(118)          => __E_bf_45(119).
	p(NT(118), (NT(119)));
//G145: bf(25)               => bf_xor(118).
	p(NT(25), (NT(118)));
//G146: __E___E_bf_46_47(122) => _(15).
	p(NT(122), (NT(15)));
//G147: __E___E_bf_46_47(122) => _(15) '&' _(15).
	p(NT(122), (NT(15)+T(28)+NT(15)));
//G148: __E_bf_46(121)       => bf(25) __E___E_bf_46_47(122) bf(25).
	p(NT(121), (NT(25)+NT(122)+NT(25)));
//G149: bf_and(120)          => __E_bf_46(121).
	p(NT(120), (NT(121)));
//G150: bf(25)               => bf_and(120).
	p(NT(25), (NT(120)));
//G151: __E_bf_48(124)       => bf(25) _(15) '\''.
	p(NT(124), (NT(25)+NT(15)+T(34)));
//G152: bf_neg(123)          => __E_bf_48(124).
	p(NT(123), (NT(124)));
//G153: bf(25)               => bf_neg(123).
	p(NT(25), (NT(123)));
//G154: __E_bf_49(126)       => '{' _(15) constant(127) _(15) '}'.
	p(NT(126), (T(8)+NT(15)+NT(127)+NT(15)+T(9)));
//G155: bf_constant(125)     => __E_bf_49(126).
	p(NT(125), (NT(126)));
//G156: bf(25)               => bf_constant(125).
	p(NT(25), (NT(125)));
//G157: bf_t(128)            => '1'.
	p(NT(128), (T(35)));
//G158: bf(25)               => bf_t(128).
	p(NT(25), (NT(128)));
//G159: bf_f(129)            => '0'.
	p(NT(129), (T(36)));
//G160: bf(25)               => bf_f(129).
	p(NT(25), (NT(129)));
//G161: bf(25)               => capture(24).
	p(NT(25), (NT(24)));
//G162: __N_0(365)           => 'F'.
	p(NT(365), (T(31)));
//G163: __N_1(366)           => 'T'.
	p(NT(366), (T(30)));
//G164: __E_charvar_50(131)  => ~( __N_0(365) ) & ~( __N_1(366) ) & alpha(5).	 # conjunctive
	p(NT(131), ~(NT(365)) & ~(NT(366)) & (NT(5)));
//G165: __E_charvar_51(132)  => null.
	p(NT(132), (nul));
//G166: __E_charvar_51(132)  => digit(3) __E_charvar_51(132).
	p(NT(132), (NT(3)+NT(132)));
//G167: charvar(130)         => __E_charvar_50(131) __E_charvar_51(132).
	p(NT(130), (NT(131)+NT(132)));
//G168: variable(112)        => charvar(130).
	p(NT(112), (NT(130)));
//G169: __E_variable_52(134) => in(38).
	p(NT(134), (NT(38)));
//G170: __E_variable_52(134) => out(135).
	p(NT(134), (NT(135)));
//G171: io_var(133)          => __E_variable_52(134).
	p(NT(133), (NT(134)));
//G172: variable(112)        => io_var(133).
	p(NT(112), (NT(133)));
//G173: __E_in_53(137)       => 'i' digits(138).
	p(NT(137), (T(15)+NT(138)));
//G174: in_var_name(136)     => __E_in_53(137).
	p(NT(136), (NT(137)));
//G175: in(38)               => in_var_name(136) '[' offset(139) ']'.
	p(NT(38), (NT(136)+T(21)+NT(139)+T(22)));
//G176: __E_out_54(141)      => 'o' digits(138).
	p(NT(141), (T(11)+NT(138)));
//G177: out_var_name(140)    => __E_out_54(141).
	p(NT(140), (NT(141)));
//G178: out(135)             => out_var_name(140) '[' offset(139) ']'.
	p(NT(135), (NT(140)+T(21)+NT(139)+T(22)));
//G179: capture(24)          => '$' charvar(130).
	p(NT(24), (T(37)+NT(130)));
//G180: __E_q_vars_55(143)   => _(15) ',' _(15) q_var(142).
	p(NT(143), (NT(15)+T(4)+NT(15)+NT(142)));
//G181: __E_q_vars_56(144)   => null.
	p(NT(144), (nul));
//G182: __E_q_vars_56(144)   => __E_q_vars_55(143) __E_q_vars_56(144).
	p(NT(144), (NT(143)+NT(144)));
//G183: q_vars(77)           => q_var(142) __E_q_vars_56(144).
	p(NT(77), (NT(142)+NT(144)));
//G184: q_var(142)           => capture(24).
	p(NT(142), (NT(24)));
//G185: q_var(142)           => variable(112).
	p(NT(142), (NT(112)));
//G186: __E_offsets_57(145)  => _(15) ',' _(15) offset(139).
	p(NT(145), (NT(15)+T(4)+NT(15)+NT(139)));
//G187: __E_offsets_58(146)  => null.
	p(NT(146), (nul));
//G188: __E_offsets_58(146)  => __E_offsets_57(145) __E_offsets_58(146).
	p(NT(146), (NT(145)+NT(146)));
//G189: offsets(28)          => '[' _(15) offset(139) __E_offsets_58(146) _(15) ']'.
	p(NT(28), (T(21)+NT(15)+NT(139)+NT(146)+NT(15)+T(22)));
//G190: offset(139)          => num(147).
	p(NT(139), (NT(147)));
//G191: offset(139)          => capture(24).
	p(NT(139), (NT(24)));
//G192: offset(139)          => shift(148).
	p(NT(139), (NT(148)));
//G193: __N_2(367)           => io_var(133).
	p(NT(367), (NT(133)));
//G194: __E_offset_59(149)   => variable(112) & ~( __N_2(367) ).	 # conjunctive
	p(NT(149), (NT(112)) & ~(NT(367)));
//G195: offset(139)          => __E_offset_59(149).
	p(NT(139), (NT(149)));
//G196: __E_shift_60(150)    => capture(24).
	p(NT(150), (NT(24)));
//G197: __N_3(368)           => io_var(133).
	p(NT(368), (NT(133)));
//G198: __E___E_shift_60_61(151) => variable(112) & ~( __N_3(368) ).	 # conjunctive
	p(NT(151), (NT(112)) & ~(NT(368)));
//G199: __E_shift_60(150)    => __E___E_shift_60_61(151).
	p(NT(150), (NT(151)));
//G200: shift(148)           => __E_shift_60(150) _(15) '-' _(15) num(147).
	p(NT(148), (NT(150)+NT(15)+T(25)+NT(15)+NT(147)));
//G201: num(147)             => digits(138).
	p(NT(147), (NT(138)));
//G202: __E___E_uninterpreted_constant_62_63(154) => chars(155).
	p(NT(154), (NT(155)));
//G203: __E___E_uninterpreted_constant_62_63(154) => _(15).
	p(NT(154), (NT(15)));
//G204: __E_uninterpreted_constant_62(153) => __E___E_uninterpreted_constant_62_63(154) ':' chars(155).
	p(NT(153), (NT(154)+T(2)+NT(155)));
//G205: uninter_const_name(152) => __E_uninterpreted_constant_62(153).
	p(NT(152), (NT(153)));
//G206: uninterpreted_constant(109) => '<' uninter_const_name(152) _(15) '>'.
	p(NT(109), (T(7)+NT(152)+NT(15)+T(16)));
//G207: constant(127)        => binding(156).
	p(NT(127), (NT(156)));
//G208: constant(127)        => capture(24).
	p(NT(127), (NT(24)));
//G209: __E___E_binding_64_65(159) => chars(155) _(15).
	p(NT(159), (NT(155)+NT(15)));
//G210: __E___E_binding_64_65(159) => _(15).
	p(NT(159), (NT(15)));
//G211: type(158)            => __E___E_binding_64_65(159).
	p(NT(158), (NT(159)));
//G212: __E_binding_64(157)  => type(158) ':' _(15) source(160).
	p(NT(157), (NT(158)+T(2)+NT(15)+NT(160)));
//G213: source_binding(39)   => __E_binding_64(157).
	p(NT(39), (NT(157)));
//G214: binding(156)         => source_binding(39).
	p(NT(156), (NT(39)));
//G215: named_binding(161)   => chars(155).
	p(NT(161), (NT(155)));
//G216: binding(156)         => named_binding(161).
	p(NT(156), (NT(161)));
//G217: __E_source_66(163)   => alnum(6).
	p(NT(163), (NT(6)));
//G218: __E_source_66(163)   => punct(7).
	p(NT(163), (NT(7)));
//G219: src_c(162)           => __E_source_66(163).
	p(NT(162), (NT(163)));
//G220: __E___E_source_67_68(165) => src_c(162).
	p(NT(165), (NT(162)));
//G221: __E___E_source_67_68(165) => space(2).
	p(NT(165), (NT(2)));
//G222: __E___E_source_67_69(166) => null.
	p(NT(166), (nul));
//G223: __E___E_source_67_69(166) => __E___E_source_67_68(165) __E___E_source_67_69(166).
	p(NT(166), (NT(165)+NT(166)));
//G224: __E_source_67(164)   => __E___E_source_67_69(166) src_c(162).
	p(NT(164), (NT(166)+NT(162)));
//G225: __E_source_67(164)   => null.
	p(NT(164), (nul));
//G226: source(160)          => src_c(162) __E_source_67(164).
	p(NT(160), (NT(162)+NT(164)));
//G227: __E_chars_70(167)    => null.
	p(NT(167), (nul));
//G228: __E_chars_70(167)    => alnum(6) __E_chars_70(167).
	p(NT(167), (NT(6)+NT(167)));
//G229: chars(155)           => alpha(5) __E_chars_70(167).
	p(NT(155), (NT(5)+NT(167)));
//G230: __E_digits_71(168)   => digit(3).
	p(NT(168), (NT(3)));
//G231: __E_digits_71(168)   => digit(3) __E_digits_71(168).
	p(NT(168), (NT(3)+NT(168)));
//G232: digits(138)          => __E_digits_71(168).
	p(NT(138), (NT(168)));
//G233: sym(26)              => chars(155).
	p(NT(26), (NT(155)));
//G234: __E_comment_72(170)  => printable(8).
	p(NT(170), (NT(8)));
//G235: __E_comment_72(170)  => '\t'.
	p(NT(170), (T(38)));
//G236: __E_comment_73(171)  => null.
	p(NT(171), (nul));
//G237: __E_comment_73(171)  => __E_comment_72(170) __E_comment_73(171).
	p(NT(171), (NT(170)+NT(171)));
//G238: __E_comment_74(172)  => '\n'.
	p(NT(172), (T(39)));
//G239: __E_comment_74(172)  => '\r'.
	p(NT(172), (T(40)));
//G240: __E_comment_74(172)  => eof(1).
	p(NT(172), (NT(1)));
//G241: comment(169)         => '#' __E_comment_73(171) __E_comment_74(172).
	p(NT(169), (T(41)+NT(171)+NT(172)));
//G242: __E____75(173)       => space(2).
	p(NT(173), (NT(2)));
//G243: __E____75(173)       => comment(169).
	p(NT(173), (NT(169)));
//G244: __(59)               => __E____75(173) _(15).
	p(NT(59), (NT(173)+NT(15)));
//G245: __E___76(174)        => __(59).
	p(NT(174), (NT(59)));
//G246: __E___76(174)        => null.
	p(NT(174), (nul));
//G247: _(15)                => __E___76(174).
	p(NT(15), (NT(174)));
//G248: bf_cb_arg(176)       => bf(25).
	p(NT(176), (NT(25)));
//G249: bf_cb_args1(175)     => __(59) bf_cb_arg(176).
	p(NT(175), (NT(59)+NT(176)));
//G250: bf_cb_args2(177)     => bf_cb_args1(175) bf_cb_args1(175).
	p(NT(177), (NT(175)+NT(175)));
//G251: bf_cb_args3(178)     => bf_cb_args2(177) bf_cb_args1(175).
	p(NT(178), (NT(177)+NT(175)));
//G252: bf_cb_args4(179)     => bf_cb_args3(178) bf_cb_args1(175).
	p(NT(179), (NT(178)+NT(175)));
//G253: wff_cb_arg(181)      => wff(18).
	p(NT(181), (NT(18)));
//G254: wff_cb_args1(180)    => __(59) wff_cb_arg(181).
	p(NT(180), (NT(59)+NT(181)));
//G255: wff_cb_args2(182)    => wff_cb_args1(180) wff_cb_args1(180).
	p(NT(182), (NT(180)+NT(180)));
//G256: wff_cb_args3(183)    => wff_cb_args2(182) wff_cb_args1(180).
	p(NT(183), (NT(182)+NT(180)));
//G257: wff_cb_args4(184)    => wff_cb_args3(183) wff_cb_args1(180).
	p(NT(184), (NT(183)+NT(180)));
//G258: __E_wff_cb_77(186)   => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b' bf_cb_args1(175) wff_cb_args2(182).
	p(NT(186), (T(42)+T(43)+T(44)+T(13)+T(45)+T(44)+T(46)+T(42)+NT(175)+NT(182)));
//G259: bf_eq_cb(185)        => __E_wff_cb_77(186).
	p(NT(185), (NT(186)));
//G260: wff_cb(51)           => bf_eq_cb(185).
	p(NT(51), (NT(185)));
//G261: __E_wff_cb_78(188)   => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b' bf_cb_args1(175) wff_cb_args2(182).
	p(NT(188), (T(42)+T(43)+T(44)+T(47)+T(13)+T(45)+T(44)+T(46)+T(42)+NT(175)+NT(182)));
//G262: bf_neq_cb(187)       => __E_wff_cb_78(188).
	p(NT(187), (NT(188)));
//G263: wff_cb(51)           => bf_neq_cb(187).
	p(NT(51), (NT(187)));
//G264: __E_wff_cb_79(190)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' wff_cb_args2(182).
	p(NT(190), (T(19)+T(43)+T(43)+T(44)+T(48)+T(17)+T(10)+T(44)+T(46)+T(18)+T(17)+T(10)+T(48)+T(15)+T(47)+T(49)+T(44)+T(10)+T(50)+T(42)+T(43)+T(11)+T(51)+T(12)+T(50)+T(18)+T(17)+T(10)+T(44)+T(46)+T(42)+NT(182)));
//G265: wff_has_clashing_subformulas_cb(189) => __E_wff_cb_79(190).
	p(NT(189), (NT(190)));
//G266: wff_cb(51)           => wff_has_clashing_subformulas_cb(189).
	p(NT(51), (NT(189)));
//G267: __E_wff_cb_80(192)   => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' wff_cb_args3(183).
	p(NT(192), (T(19)+T(43)+T(43)+T(44)+T(48)+T(17)+T(10)+T(44)+T(10)+T(50)+T(42)+T(43)+T(11)+T(51)+T(12)+T(50)+T(18)+T(17)+T(44)+T(46)+T(42)+NT(183)));
//G268: wff_has_subformula_cb(191) => __E_wff_cb_80(192).
	p(NT(191), (NT(192)));
//G269: wff_cb(51)           => wff_has_subformula_cb(191).
	p(NT(51), (NT(191)));
//G270: __E_wff_cb_81(194)   => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' wff_cb_args2(182).
	p(NT(194), (T(19)+T(43)+T(43)+T(44)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(44)+T(13)+T(24)+T(15)+T(10)+T(14)+T(13)+T(47)+T(14)+T(15)+T(17)+T(18)+T(44)+T(46)+T(42)+NT(182)));
//G271: wff_remove_existential_cb(193) => __E_wff_cb_81(194).
	p(NT(193), (NT(194)));
//G272: wff_cb(51)           => wff_remove_existential_cb(193).
	p(NT(51), (NT(193)));
//G273: __E_bf_cb_82(196)    => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b' bf_cb_args2(177).
	p(NT(196), (T(42)+T(43)+T(44)+T(15)+T(10)+T(44)+T(53)+T(13)+T(51)+T(11)+T(44)+T(46)+T(42)+NT(177)));
//G274: bf_is_zero_cb(195)   => __E_bf_cb_82(196).
	p(NT(195), (NT(196)));
//G275: bf_cb(55)            => bf_is_zero_cb(195).
	p(NT(55), (NT(195)));
//G276: __E_bf_cb_83(198)    => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b' bf_cb_args2(177).
	p(NT(198), (T(42)+T(43)+T(44)+T(15)+T(10)+T(44)+T(11)+T(47)+T(13)+T(44)+T(46)+T(42)+NT(177)));
//G277: bf_is_one_cb(197)    => __E_bf_cb_83(198).
	p(NT(197), (NT(198)));
//G278: bf_cb(55)            => bf_is_one_cb(197).
	p(NT(55), (NT(197)));
//G279: __E_bf_cb_84(200)    => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' bf_cb_args3(178).
	p(NT(200), (T(42)+T(43)+T(44)+T(48)+T(17)+T(10)+T(44)+T(10)+T(50)+T(42)+T(43)+T(11)+T(51)+T(12)+T(50)+T(18)+T(17)+T(44)+T(46)+T(42)+NT(178)));
//G280: bf_has_subformula_cb(199) => __E_bf_cb_84(200).
	p(NT(199), (NT(200)));
//G281: bf_cb(55)            => bf_has_subformula_cb(199).
	p(NT(55), (NT(199)));
//G282: __E_bf_cb_85(202)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b' bf_cb_args4(179).
	p(NT(202), (T(42)+T(43)+T(44)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(44)+T(43)+T(50)+T(47)+T(15)+T(52)+T(13)+T(51)+T(10)+T(17)+T(18)+T(44)+T(46)+T(42)+NT(179)));
//G283: bf_remove_funiversal_cb(201) => __E_bf_cb_85(202).
	p(NT(201), (NT(202)));
//G284: bf_cb(55)            => bf_remove_funiversal_cb(201).
	p(NT(55), (NT(201)));
//G285: __E_bf_cb_86(204)    => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' bf_cb_args4(179).
	p(NT(204), (T(42)+T(43)+T(44)+T(51)+T(13)+T(12)+T(11)+T(52)+T(13)+T(44)+T(43)+T(13)+T(24)+T(15)+T(10)+T(14)+T(13)+T(47)+T(14)+T(15)+T(17)+T(18)+T(44)+T(46)+T(42)+NT(179)));
//G286: bf_remove_fexistential_cb(203) => __E_bf_cb_86(204).
	p(NT(203), (NT(204)));
//G287: bf_cb(55)            => bf_remove_fexistential_cb(203).
	p(NT(55), (NT(203)));
//G288: __E_bf_cb_87(206)    => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b' bf_cb_args2(177).
	p(NT(206), (T(42)+T(43)+T(44)+T(17)+T(47)+T(54)+T(44)+T(46)+T(42)+NT(177)));
//G289: bf_and_cb(205)       => __E_bf_cb_87(206).
	p(NT(205), (NT(206)));
//G290: bf_cb(55)            => bf_and_cb(205).
	p(NT(55), (NT(205)));
//G291: __E_bf_cb_88(208)    => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b' bf_cb_args2(177).
	p(NT(208), (T(42)+T(43)+T(44)+T(11)+T(51)+T(44)+T(46)+T(42)+NT(177)));
//G292: bf_or_cb(207)        => __E_bf_cb_88(208).
	p(NT(207), (NT(208)));
//G293: bf_cb(55)            => bf_or_cb(207).
	p(NT(55), (NT(207)));
//G294: __E_bf_cb_89(210)    => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b' bf_cb_args2(177).
	p(NT(210), (T(42)+T(43)+T(44)+T(24)+T(11)+T(51)+T(44)+T(46)+T(42)+NT(177)));
//G295: bf_xor_cb(209)       => __E_bf_cb_89(210).
	p(NT(209), (NT(210)));
//G296: bf_cb(55)            => bf_xor_cb(209).
	p(NT(55), (NT(209)));
//G297: __E_bf_cb_90(212)    => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b' bf_cb_args1(175).
	p(NT(212), (T(42)+T(43)+T(44)+T(47)+T(13)+T(49)+T(44)+T(46)+T(42)+NT(175)));
//G298: bf_neg_cb(211)       => __E_bf_cb_90(212).
	p(NT(211), (NT(212)));
//G299: bf_cb(55)            => bf_neg_cb(211).
	p(NT(55), (NT(211)));
//G300: __E_bf_cb_91(214)    => 'b' 'f' '_' 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e' '_' 'c' 'b' bf_cb_args1(175).
	p(NT(214), (T(42)+T(43)+T(44)+T(47)+T(11)+T(51)+T(12)+T(17)+T(18)+T(15)+T(53)+T(13)+T(44)+T(46)+T(42)+NT(175)));
//G301: bf_normalize_cb(213) => __E_bf_cb_91(214).
	p(NT(213), (NT(214)));
//G302: bf_cb(55)            => bf_normalize_cb(213).
	p(NT(55), (NT(213)));
//G303: cli(215)             => _(15).
	p(NT(215), (NT(15)));
//G304: __E_cli_92(217)      => '.' _(15) cli_command(216) _(15).
	p(NT(217), (T(1)+NT(15)+NT(216)+NT(15)));
//G305: __E_cli_93(218)      => null.
	p(NT(218), (nul));
//G306: __E_cli_93(218)      => __E_cli_92(217) __E_cli_93(218).
	p(NT(218), (NT(217)+NT(218)));
//G307: cli(215)             => _(15) cli_command(216) _(15) __E_cli_93(218).
	p(NT(215), (NT(15)+NT(216)+NT(15)+NT(218)));
//G308: __E_cli_command_94(221) => 'q'.
	p(NT(221), (T(45)));
//G309: __E_cli_command_94(221) => 'q' 'u' 'i' 't'.
	p(NT(221), (T(45)+T(50)+T(15)+T(14)));
//G310: quit_sym(220)        => __E_cli_command_94(221).
	p(NT(220), (NT(221)));
//G311: quit_cmd(219)        => quit_sym(220).
	p(NT(219), (NT(220)));
//G312: cli_command(216)     => quit_cmd(219).
	p(NT(216), (NT(219)));
//G313: __E_cli_command_95(224) => 'v'.
	p(NT(224), (T(52)));
//G314: __E_cli_command_95(224) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(224), (T(52)+T(13)+T(51)+T(10)+T(15)+T(11)+T(47)));
//G315: version_sym(223)     => __E_cli_command_95(224).
	p(NT(223), (NT(224)));
//G316: version_cmd(222)     => version_sym(223).
	p(NT(222), (NT(223)));
//G317: cli_command(216)     => version_cmd(222).
	p(NT(216), (NT(222)));
//G318: __E_cli_command_96(227) => 'c'.
	p(NT(227), (T(46)));
//G319: __E_cli_command_96(227) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(227), (T(46)+T(18)+T(13)+T(17)+T(51)));
//G320: clear_sym(226)       => __E_cli_command_96(227).
	p(NT(226), (NT(227)));
//G321: clear_cmd(225)       => clear_sym(226).
	p(NT(225), (NT(226)));
//G322: cli_command(216)     => clear_cmd(225).
	p(NT(216), (NT(225)));
//G323: __E___E_cli_command_97_98(231) => 'h'.
	p(NT(231), (T(48)));
//G324: __E___E_cli_command_97_98(231) => 'h' 'e' 'l' 'p'.
	p(NT(231), (T(48)+T(13)+T(18)+T(55)));
//G325: help_sym(230)        => __E___E_cli_command_97_98(231).
	p(NT(230), (NT(231)));
//G326: __E___E_cli_command_97_99(232) => __(59) help_arg(233).
	p(NT(232), (NT(59)+NT(233)));
//G327: __E___E_cli_command_97_99(232) => null.
	p(NT(232), (nul));
//G328: __E_cli_command_97(229) => help_sym(230) __E___E_cli_command_97_99(232).
	p(NT(229), (NT(230)+NT(232)));
//G329: help_cmd(228)        => __E_cli_command_97(229).
	p(NT(228), (NT(229)));
//G330: cli_command(216)     => help_cmd(228).
	p(NT(216), (NT(228)));
//G331: file_sym(236)        => 'f' 'i' 'l' 'e'.
	p(NT(236), (T(43)+T(15)+T(18)+T(13)));
//G332: __E_cli_command_100(235) => file_sym(236) __(59) q_string(237).
	p(NT(235), (NT(236)+NT(59)+NT(237)));
//G333: file_cmd(234)        => __E_cli_command_100(235).
	p(NT(234), (NT(235)));
//G334: cli_command(216)     => file_cmd(234).
	p(NT(216), (NT(234)));
//G335: valid_sym(240)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(240), (T(52)+T(17)+T(18)+T(15)+T(54)));
//G336: __E_cli_command_101(239) => valid_sym(240) __(59) rr(14).
	p(NT(239), (NT(240)+NT(59)+NT(14)));
//G337: valid_cmd(238)       => __E_cli_command_101(239).
	p(NT(238), (NT(239)));
//G338: cli_command(216)     => valid_cmd(238).
	p(NT(216), (NT(238)));
//G339: sat_sym(243)         => 's' 'a' 't'.
	p(NT(243), (T(10)+T(17)+T(14)));
//G340: __E_cli_command_102(242) => sat_sym(243) __(59) rr(14).
	p(NT(242), (NT(243)+NT(59)+NT(14)));
//G341: sat_cmd(241)         => __E_cli_command_102(242).
	p(NT(241), (NT(242)));
//G342: cli_command(216)     => sat_cmd(241).
	p(NT(216), (NT(241)));
//G343: unsat_sym(246)       => 'u' 'n' 's' 'a' 't'.
	p(NT(246), (T(50)+T(47)+T(10)+T(17)+T(14)));
//G344: __E_cli_command_103(245) => unsat_sym(246) __(59) rr(14).
	p(NT(245), (NT(246)+NT(59)+NT(14)));
//G345: unsat_cmd(244)       => __E_cli_command_103(245).
	p(NT(244), (NT(245)));
//G346: cli_command(216)     => unsat_cmd(244).
	p(NT(216), (NT(244)));
//G347: solve_sym(249)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(249), (T(10)+T(11)+T(18)+T(52)+T(13)));
//G348: __E_cli_command_104(248) => solve_sym(249) __(59) wff_cmd_arg(250).
	p(NT(248), (NT(249)+NT(59)+NT(250)));
//G349: solve_cmd(247)       => __E_cli_command_104(248).
	p(NT(247), (NT(248)));
//G350: cli_command(216)     => solve_cmd(247).
	p(NT(216), (NT(247)));
//G351: __E___E_cli_command_105_106(254) => 'e'.
	p(NT(254), (T(13)));
//G352: __E___E_cli_command_105_106(254) => 'e' 'x' 'e' 'c' 'u' 't' 'e'.
	p(NT(254), (T(13)+T(24)+T(13)+T(46)+T(50)+T(14)+T(13)));
//G353: execute_sym(253)     => __E___E_cli_command_105_106(254).
	p(NT(253), (NT(254)));
//G354: __E___E_cli_command_105_107(255) => wff_typed(256).
	p(NT(255), (NT(256)));
//G355: __E___E_cli_command_105_107(255) => rr(14).
	p(NT(255), (NT(14)));
//G356: __E_cli_command_105(252) => execute_sym(253) __(59) __E___E_cli_command_105_107(255).
	p(NT(252), (NT(253)+NT(59)+NT(255)));
//G357: execute_cmd(251)     => __E_cli_command_105(252).
	p(NT(251), (NT(252)));
//G358: cli_command(216)     => execute_cmd(251).
	p(NT(216), (NT(251)));
//G359: __E___E_cli_command_108_109(260) => 'n'.
	p(NT(260), (T(47)));
//G360: __E___E_cli_command_108_109(260) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(260), (T(47)+T(11)+T(51)+T(12)+T(17)+T(18)+T(15)+T(53)+T(13)));
//G361: normalize_sym(259)   => __E___E_cli_command_108_109(260).
	p(NT(259), (NT(260)));
//G362: __E_cli_command_108(258) => normalize_sym(259) __(59) normalize_cmd_arg(261).
	p(NT(258), (NT(259)+NT(59)+NT(261)));
//G363: normalize_cmd(257)   => __E_cli_command_108(258).
	p(NT(257), (NT(258)));
//G364: cli_command(216)     => normalize_cmd(257).
	p(NT(216), (NT(257)));
//G365: __E___E_cli_command_110_111(265) => 's'.
	p(NT(265), (T(10)));
//G366: __E___E_cli_command_110_111(265) => 's' 'u' 'b' 's' 't'.
	p(NT(265), (T(10)+T(50)+T(42)+T(10)+T(14)));
//G367: __E___E_cli_command_110_111(265) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(265), (T(10)+T(50)+T(42)+T(10)+T(14)+T(15)+T(14)+T(50)+T(14)+T(13)));
//G368: subst_sym(264)       => __E___E_cli_command_110_111(265).
	p(NT(264), (NT(265)));
//G369: __E_cli_command_110(263) => subst_sym(264) __(59) nf_cmd_arg(266) _(15) '[' _(15) nf_cmd_arg(266) _(15) '/' _(15) nf_cmd_arg(266) _(15) ']'.
	p(NT(263), (NT(264)+NT(59)+NT(266)+NT(15)+T(21)+NT(15)+NT(266)+NT(15)+T(56)+NT(15)+NT(266)+NT(15)+T(22)));
//G370: subst_cmd(262)       => __E_cli_command_110(263).
	p(NT(262), (NT(263)));
//G371: cli_command(216)     => subst_cmd(262).
	p(NT(216), (NT(262)));
//G372: __E___E_cli_command_112_113(270) => 'i'.
	p(NT(270), (T(15)));
//G373: __E___E_cli_command_112_113(270) => 'i' 'n' 's' 't'.
	p(NT(270), (T(15)+T(47)+T(10)+T(14)));
//G374: __E___E_cli_command_112_113(270) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(270), (T(15)+T(47)+T(10)+T(14)+T(17)+T(47)+T(14)+T(15)+T(17)+T(14)+T(13)));
//G375: inst_sym(269)        => __E___E_cli_command_112_113(270).
	p(NT(269), (NT(270)));
//G376: __E_cli_command_112(268) => inst_sym(269) __(59) inst_args(271).
	p(NT(268), (NT(269)+NT(59)+NT(271)));
//G377: inst_cmd(267)        => __E_cli_command_112(268).
	p(NT(267), (NT(268)));
//G378: cli_command(216)     => inst_cmd(267).
	p(NT(216), (NT(267)));
//G379: dnf_sym(274)         => 'd' 'n' 'f'.
	p(NT(274), (T(54)+T(47)+T(43)));
//G380: __E_cli_command_114(273) => dnf_sym(274) __(59) nf_cmd_arg(266).
	p(NT(273), (NT(274)+NT(59)+NT(266)));
//G381: dnf_cmd(272)         => __E_cli_command_114(273).
	p(NT(272), (NT(273)));
//G382: cli_command(216)     => dnf_cmd(272).
	p(NT(216), (NT(272)));
//G383: cnf_sym(277)         => 'c' 'n' 'f'.
	p(NT(277), (T(46)+T(47)+T(43)));
//G384: __E_cli_command_115(276) => cnf_sym(277) __(59) nf_cmd_arg(266).
	p(NT(276), (NT(277)+NT(59)+NT(266)));
//G385: cnf_cmd(275)         => __E_cli_command_115(276).
	p(NT(275), (NT(276)));
//G386: cli_command(216)     => cnf_cmd(275).
	p(NT(216), (NT(275)));
//G387: anf_sym(280)         => 'a' 'n' 'f'.
	p(NT(280), (T(17)+T(47)+T(43)));
//G388: __E_cli_command_116(279) => anf_sym(280) __(59) nf_cmd_arg(266).
	p(NT(279), (NT(280)+NT(59)+NT(266)));
//G389: anf_cmd(278)         => __E_cli_command_116(279).
	p(NT(278), (NT(279)));
//G390: cli_command(216)     => anf_cmd(278).
	p(NT(216), (NT(278)));
//G391: nnf_sym(283)         => 'n' 'n' 'f'.
	p(NT(283), (T(47)+T(47)+T(43)));
//G392: __E_cli_command_117(282) => nnf_sym(283) __(59) nf_cmd_arg(266).
	p(NT(282), (NT(283)+NT(59)+NT(266)));
//G393: nnf_cmd(281)         => __E_cli_command_117(282).
	p(NT(281), (NT(282)));
//G394: cli_command(216)     => nnf_cmd(281).
	p(NT(216), (NT(281)));
//G395: pnf_sym(286)         => 'p' 'n' 'f'.
	p(NT(286), (T(55)+T(47)+T(43)));
//G396: __E_cli_command_118(285) => pnf_sym(286) __(59) nf_cmd_arg(266).
	p(NT(285), (NT(286)+NT(59)+NT(266)));
//G397: pnf_cmd(284)         => __E_cli_command_118(285).
	p(NT(284), (NT(285)));
//G398: cli_command(216)     => pnf_cmd(284).
	p(NT(216), (NT(284)));
//G399: mnf_sym(289)         => 'm' 'n' 'f'.
	p(NT(289), (T(12)+T(47)+T(43)));
//G400: __E_cli_command_119(288) => mnf_sym(289) __(59) nf_cmd_arg(266).
	p(NT(288), (NT(289)+NT(59)+NT(266)));
//G401: mnf_cmd(287)         => __E_cli_command_119(288).
	p(NT(287), (NT(288)));
//G402: cli_command(216)     => mnf_cmd(287).
	p(NT(216), (NT(287)));
//G403: snf_sym(292)         => 's' 'n' 'f'.
	p(NT(292), (T(10)+T(47)+T(43)));
//G404: __E_cli_command_120(291) => snf_sym(292) __(59) nf_cmd_arg(266).
	p(NT(291), (NT(292)+NT(59)+NT(266)));
//G405: snf_cmd(290)         => __E_cli_command_120(291).
	p(NT(290), (NT(291)));
//G406: cli_command(216)     => snf_cmd(290).
	p(NT(216), (NT(290)));
//G407: onf_sym(295)         => 'o' 'n' 'f'.
	p(NT(295), (T(11)+T(47)+T(43)));
//G408: __E_cli_command_121(294) => onf_sym(295) __(59) variable(112) __(59) onf_cmd_arg(296).
	p(NT(294), (NT(295)+NT(59)+NT(112)+NT(59)+NT(296)));
//G409: onf_cmd(293)         => __E_cli_command_121(294).
	p(NT(293), (NT(294)));
//G410: cli_command(216)     => onf_cmd(293).
	p(NT(216), (NT(293)));
//G411: __E___E_cli_command_122_123(300) => 'd' 'e' 'f' 's'.
	p(NT(300), (T(54)+T(13)+T(43)+T(10)));
//G412: __E___E_cli_command_122_123(300) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(300), (T(54)+T(13)+T(43)+T(15)+T(47)+T(15)+T(14)+T(15)+T(11)+T(47)+T(10)));
//G413: def_sym(299)         => __E___E_cli_command_122_123(300).
	p(NT(299), (NT(300)));
//G414: __E_cli_command_122(298) => def_sym(299).
	p(NT(298), (NT(299)));
//G415: def_list_cmd(297)    => __E_cli_command_122(298).
	p(NT(297), (NT(298)));
//G416: cli_command(216)     => def_list_cmd(297).
	p(NT(216), (NT(297)));
//G417: __E_cli_command_124(302) => def_sym(299) __(59) number(303).
	p(NT(302), (NT(299)+NT(59)+NT(303)));
//G418: def_print_cmd(301)   => __E_cli_command_124(302).
	p(NT(301), (NT(302)));
//G419: cli_command(216)     => def_print_cmd(301).
	p(NT(216), (NT(301)));
//G420: def_rr_cmd(304)      => rec_relation(20).
	p(NT(304), (NT(20)));
//G421: cli_command(216)     => def_rr_cmd(304).
	p(NT(216), (NT(304)));
//G422: qelim_sym(307)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(307), (T(45)+T(13)+T(18)+T(15)+T(12)));
//G423: __E_cli_command_125(306) => qelim_sym(307) __(59) wff_cmd_arg(250).
	p(NT(306), (NT(307)+NT(59)+NT(250)));
//G424: qelim_cmd(305)       => __E_cli_command_125(306).
	p(NT(305), (NT(306)));
//G425: cli_command(216)     => qelim_cmd(305).
	p(NT(216), (NT(305)));
//G426: get_sym(310)         => 'g' 'e' 't'.
	p(NT(310), (T(49)+T(13)+T(14)));
//G427: __E___E_cli_command_126_127(311) => __(59) option(312).
	p(NT(311), (NT(59)+NT(312)));
//G428: __E___E_cli_command_126_127(311) => null.
	p(NT(311), (nul));
//G429: __E_cli_command_126(309) => get_sym(310) __E___E_cli_command_126_127(311).
	p(NT(309), (NT(310)+NT(311)));
//G430: get_cmd(308)         => __E_cli_command_126(309).
	p(NT(308), (NT(309)));
//G431: cli_command(216)     => get_cmd(308).
	p(NT(216), (NT(308)));
//G432: set_sym(315)         => 's' 'e' 't'.
	p(NT(315), (T(10)+T(13)+T(14)));
//G433: __E___E_cli_command_128_129(316) => __(59).
	p(NT(316), (NT(59)));
//G434: __E___E_cli_command_128_129(316) => _(15) '=' _(15).
	p(NT(316), (NT(15)+T(3)+NT(15)));
//G435: __E_cli_command_128(314) => set_sym(315) __(59) option(312) __E___E_cli_command_128_129(316) option_value(317).
	p(NT(314), (NT(315)+NT(59)+NT(312)+NT(316)+NT(317)));
//G436: set_cmd(313)         => __E_cli_command_128(314).
	p(NT(313), (NT(314)));
//G437: cli_command(216)     => set_cmd(313).
	p(NT(216), (NT(313)));
//G438: toggle_sym(320)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(320), (T(14)+T(11)+T(49)+T(49)+T(18)+T(13)));
//G439: __E_cli_command_130(319) => toggle_sym(320) __(59) bool_option(321).
	p(NT(319), (NT(320)+NT(59)+NT(321)));
//G440: toggle_cmd(318)      => __E_cli_command_130(319).
	p(NT(318), (NT(319)));
//G441: cli_command(216)     => toggle_cmd(318).
	p(NT(216), (NT(318)));
//G442: __E___E_cli_command_131_132(325) => 'h' 'i' 's' 't'.
	p(NT(325), (T(48)+T(15)+T(10)+T(14)));
//G443: __E___E_cli_command_131_132(325) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(325), (T(48)+T(15)+T(10)+T(14)+T(11)+T(51)+T(20)));
//G444: history_sym(324)     => __E___E_cli_command_131_132(325).
	p(NT(324), (NT(325)));
//G445: __E_cli_command_131(323) => history_sym(324).
	p(NT(323), (NT(324)));
//G446: history_list_cmd(322) => __E_cli_command_131(323).
	p(NT(322), (NT(323)));
//G447: cli_command(216)     => history_list_cmd(322).
	p(NT(216), (NT(322)));
//G448: __E_cli_command_133(327) => history_sym(324) __(59) memory(328).
	p(NT(327), (NT(324)+NT(59)+NT(328)));
//G449: history_print_cmd(326) => __E_cli_command_133(327).
	p(NT(326), (NT(327)));
//G450: cli_command(216)     => history_print_cmd(326).
	p(NT(216), (NT(326)));
//G451: __E_cli_command_134(330) => bf_typed(331).
	p(NT(330), (NT(331)));
//G452: __E_cli_command_134(330) => wff_typed(256).
	p(NT(330), (NT(256)));
//G453: history_store_cmd(329) => __E_cli_command_134(330).
	p(NT(329), (NT(330)));
//G454: cli_command(216)     => history_store_cmd(329).
	p(NT(216), (NT(329)));
//G455: number(303)          => digits(138).
	p(NT(303), (NT(138)));
//G456: bf_typed(331)        => bf(25).
	p(NT(331), (NT(25)));
//G457: bf_typed(331)        => bf(25) _(15) ':' _(15) 'b' 'f'.
	p(NT(331), (NT(25)+NT(15)+T(2)+NT(15)+T(42)+T(43)));
//G458: wff_typed(256)       => wff(18).
	p(NT(256), (NT(18)));
//G459: wff_typed(256)       => wff(18) _(15) ':' _(15) 'w' 'f' 'f'.
	p(NT(256), (NT(18)+NT(15)+T(2)+NT(15)+T(19)+T(43)+T(43)));
//G460: bf_cmd_arg(332)      => memory(328).
	p(NT(332), (NT(328)));
//G461: bf_cmd_arg(332)      => bf_typed(331).
	p(NT(332), (NT(331)));
//G462: wff_cmd_arg(250)     => memory(328).
	p(NT(250), (NT(328)));
//G463: wff_cmd_arg(250)     => wff_typed(256).
	p(NT(250), (NT(256)));
//G464: nf_cmd_arg(266)      => memory(328).
	p(NT(266), (NT(328)));
//G465: nf_cmd_arg(266)      => bf_typed(331).
	p(NT(266), (NT(331)));
//G466: nf_cmd_arg(266)      => wff_typed(256).
	p(NT(266), (NT(256)));
//G467: onf_cmd_arg(296)     => memory(328).
	p(NT(296), (NT(328)));
//G468: onf_cmd_arg(296)     => wff_typed(256).
	p(NT(296), (NT(256)));
//G469: normalize_cmd_arg(261) => memory(328).
	p(NT(261), (NT(328)));
//G470: normalize_cmd_arg(261) => rr(14).
	p(NT(261), (NT(14)));
//G471: normalize_cmd_arg(261) => ref(22).
	p(NT(261), (NT(22)));
//G472: normalize_cmd_arg(261) => bf_typed(331).
	p(NT(261), (NT(331)));
//G473: normalize_cmd_arg(261) => wff_typed(256).
	p(NT(261), (NT(256)));
//G474: inst_args(271)       => bf_cmd_arg(332) _(15) '[' _(15) variable(112) _(15) '/' _(15) bf_cmd_arg(332) _(15) ']'.
	p(NT(271), (NT(332)+NT(15)+T(21)+NT(15)+NT(112)+NT(15)+T(56)+NT(15)+NT(332)+NT(15)+T(22)));
//G475: inst_args(271)       => wff_cmd_arg(250) _(15) '[' _(15) variable(112) _(15) '/' _(15) bf_cmd_arg(332) _(15) ']'.
	p(NT(271), (NT(250)+NT(15)+T(21)+NT(15)+NT(112)+NT(15)+T(56)+NT(15)+NT(332)+NT(15)+T(22)));
//G476: help_arg(233)        => help_sym(230).
	p(NT(233), (NT(230)));
//G477: help_arg(233)        => version_sym(223).
	p(NT(233), (NT(223)));
//G478: help_arg(233)        => quit_sym(220).
	p(NT(233), (NT(220)));
//G479: help_arg(233)        => clear_sym(226).
	p(NT(233), (NT(226)));
//G480: help_arg(233)        => get_sym(310).
	p(NT(233), (NT(310)));
//G481: help_arg(233)        => set_sym(315).
	p(NT(233), (NT(315)));
//G482: help_arg(233)        => toggle_sym(320).
	p(NT(233), (NT(320)));
//G483: help_arg(233)        => file_sym(236).
	p(NT(233), (NT(236)));
//G484: help_arg(233)        => history_sym(324).
	p(NT(233), (NT(324)));
//G485: help_arg(233)        => abs_memory_sym(333).
	p(NT(233), (NT(333)));
//G486: help_arg(233)        => rel_memory_sym(334).
	p(NT(233), (NT(334)));
//G487: help_arg(233)        => selection_sym(335).
	p(NT(233), (NT(335)));
//G488: help_arg(233)        => def_sym(299).
	p(NT(233), (NT(299)));
//G489: help_arg(233)        => inst_sym(269).
	p(NT(233), (NT(269)));
//G490: help_arg(233)        => subst_sym(264).
	p(NT(233), (NT(264)));
//G491: help_arg(233)        => normalize_sym(259).
	p(NT(233), (NT(259)));
//G492: help_arg(233)        => execute_sym(253).
	p(NT(233), (NT(253)));
//G493: help_arg(233)        => solve_sym(249).
	p(NT(233), (NT(249)));
//G494: help_arg(233)        => valid_sym(240).
	p(NT(233), (NT(240)));
//G495: help_arg(233)        => sat_sym(243).
	p(NT(233), (NT(243)));
//G496: help_arg(233)        => unsat_sym(246).
	p(NT(233), (NT(246)));
//G497: help_arg(233)        => dnf_sym(274).
	p(NT(233), (NT(274)));
//G498: help_arg(233)        => cnf_sym(277).
	p(NT(233), (NT(277)));
//G499: help_arg(233)        => anf_sym(280).
	p(NT(233), (NT(280)));
//G500: help_arg(233)        => snf_sym(292).
	p(NT(233), (NT(292)));
//G501: help_arg(233)        => nnf_sym(283).
	p(NT(233), (NT(283)));
//G502: help_arg(233)        => pnf_sym(286).
	p(NT(233), (NT(286)));
//G503: help_arg(233)        => mnf_sym(289).
	p(NT(233), (NT(289)));
//G504: help_arg(233)        => onf_sym(295).
	p(NT(233), (NT(295)));
//G505: help_arg(233)        => qelim_sym(307).
	p(NT(233), (NT(307)));
//G506: __E___E_help_arg_135_136(338) => 's'.
	p(NT(338), (T(10)));
//G507: __E___E_help_arg_135_136(338) => null.
	p(NT(338), (nul));
//G508: __E_help_arg_135(337) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_135_136(338).
	p(NT(337), (T(13)+T(24)+T(17)+T(12)+T(55)+T(18)+T(13)+NT(338)));
//G509: examples_sym(336)    => __E_help_arg_135(337).
	p(NT(336), (NT(337)));
//G510: help_arg(233)        => examples_sym(336).
	p(NT(233), (NT(336)));
//G511: __E_memory_137(340)  => '%'.
	p(NT(340), (T(57)));
//G512: rel_memory_sym(334)  => '%' '-'.
	p(NT(334), (T(57)+T(25)));
//G513: memory_id(342)       => digits(138).
	p(NT(342), (NT(138)));
//G514: __E___E_memory_137_138(341) => memory_id(342).
	p(NT(341), (NT(342)));
//G515: __E___E_memory_137_138(341) => null.
	p(NT(341), (nul));
//G516: __E_memory_137(340)  => rel_memory_sym(334) __E___E_memory_137_138(341).
	p(NT(340), (NT(334)+NT(341)));
//G517: rel_memory(339)      => __E_memory_137(340).
	p(NT(339), (NT(340)));
//G518: rel_memory(339)      => rel_memory(339).
	p(NT(339), (NT(339)));
//G519: memory(328)          => rel_memory(339).
	p(NT(328), (NT(339)));
//G520: abs_memory_sym(333)  => '%'.
	p(NT(333), (T(57)));
//G521: __E_memory_139(344)  => abs_memory_sym(333) memory_id(342).
	p(NT(344), (NT(333)+NT(342)));
//G522: abs_memory(343)      => __E_memory_139(344).
	p(NT(343), (NT(344)));
//G523: memory(328)          => abs_memory(343).
	p(NT(328), (NT(343)));
//G524: option(312)          => bool_option(321).
	p(NT(312), (NT(321)));
//G525: __E_option_140(347)  => 's' 'e' 'v'.
	p(NT(347), (T(10)+T(13)+T(52)));
//G526: __E_option_140(347)  => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
	p(NT(347), (T(10)+T(13)+T(52)+T(13)+T(51)+T(15)+T(14)+T(20)));
//G527: severity_opt(346)    => __E_option_140(347).
	p(NT(346), (NT(347)));
//G528: enum_option(345)     => severity_opt(346).
	p(NT(345), (NT(346)));
//G529: option(312)          => enum_option(345).
	p(NT(312), (NT(345)));
//G530: __E_bool_option_141(349) => 's'.
	p(NT(349), (T(10)));
//G531: __E_bool_option_141(349) => 's' 't' 'a' 't' 'u' 's'.
	p(NT(349), (T(10)+T(14)+T(17)+T(14)+T(50)+T(10)));
//G532: status_opt(348)      => __E_bool_option_141(349).
	p(NT(348), (NT(349)));
//G533: bool_option(321)     => status_opt(348).
	p(NT(321), (NT(348)));
//G534: __E_bool_option_142(351) => 'c'.
	p(NT(351), (T(46)));
//G535: __E_bool_option_142(351) => 'c' 'o' 'l' 'o' 'r'.
	p(NT(351), (T(46)+T(11)+T(18)+T(11)+T(51)));
//G536: __E_bool_option_142(351) => 'c' 'o' 'l' 'o' 'r' 's'.
	p(NT(351), (T(46)+T(11)+T(18)+T(11)+T(51)+T(10)));
//G537: colors_opt(350)      => __E_bool_option_142(351).
	p(NT(350), (NT(351)));
//G538: bool_option(321)     => colors_opt(350).
	p(NT(321), (NT(350)));
//G539: __E_bool_option_143(353) => 'd'.
	p(NT(353), (T(54)));
//G540: __E_bool_option_143(353) => 'd' 'b' 'g'.
	p(NT(353), (T(54)+T(42)+T(49)));
//G541: __E_bool_option_143(353) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(353), (T(54)+T(13)+T(42)+T(50)+T(49)));
//G542: debug_repl_opt(352)  => __E_bool_option_143(353).
	p(NT(352), (NT(353)));
//G543: bool_option(321)     => debug_repl_opt(352).
	p(NT(321), (NT(352)));
//G544: option_value(317)    => option_value_true(354).
	p(NT(317), (NT(354)));
//G545: option_value(317)    => option_value_false(355).
	p(NT(317), (NT(355)));
//G546: option_value(317)    => severity(356).
	p(NT(317), (NT(356)));
//G547: option_value_true(354) => 't'.
	p(NT(354), (T(14)));
//G548: option_value_true(354) => 't' 'r' 'u' 'e'.
	p(NT(354), (T(14)+T(51)+T(50)+T(13)));
//G549: option_value_true(354) => 'o' 'n'.
	p(NT(354), (T(11)+T(47)));
//G550: option_value_true(354) => '1'.
	p(NT(354), (T(35)));
//G551: option_value_true(354) => 'y'.
	p(NT(354), (T(20)));
//G552: option_value_true(354) => 'y' 'e' 's'.
	p(NT(354), (T(20)+T(13)+T(10)));
//G553: option_value_false(355) => 'f'.
	p(NT(355), (T(43)));
//G554: option_value_false(355) => 'f' 'a' 'l' 's' 'e'.
	p(NT(355), (T(43)+T(17)+T(18)+T(10)+T(13)));
//G555: option_value_false(355) => 'o' 'f' 'f'.
	p(NT(355), (T(11)+T(43)+T(43)));
//G556: option_value_false(355) => '0'.
	p(NT(355), (T(36)));
//G557: option_value_false(355) => 'n'.
	p(NT(355), (T(47)));
//G558: option_value_false(355) => 'n' 'o'.
	p(NT(355), (T(47)+T(11)));
//G559: __E_severity_144(358) => 'e'.
	p(NT(358), (T(13)));
//G560: __E_severity_144(358) => 'e' 'r' 'r' 'o' 'r'.
	p(NT(358), (T(13)+T(51)+T(51)+T(11)+T(51)));
//G561: error_sym(357)       => __E_severity_144(358).
	p(NT(357), (NT(358)));
//G562: severity(356)        => error_sym(357).
	p(NT(356), (NT(357)));
//G563: __E_severity_145(360) => 'i'.
	p(NT(360), (T(15)));
//G564: __E_severity_145(360) => 'i' 'n' 'f' 'o'.
	p(NT(360), (T(15)+T(47)+T(43)+T(11)));
//G565: info_sym(359)        => __E_severity_145(360).
	p(NT(359), (NT(360)));
//G566: severity(356)        => info_sym(359).
	p(NT(356), (NT(359)));
//G567: __E_severity_146(362) => 'd'.
	p(NT(362), (T(54)));
//G568: __E_severity_146(362) => 'd' 'e' 'b' 'u' 'g'.
	p(NT(362), (T(54)+T(13)+T(42)+T(50)+T(49)));
//G569: debug_sym(361)       => __E_severity_146(362).
	p(NT(361), (NT(362)));
//G570: severity(356)        => debug_sym(361).
	p(NT(356), (NT(361)));
//G571: __E_severity_147(364) => 't'.
	p(NT(364), (T(14)));
//G572: __E_severity_147(364) => 't' 'r' 'a' 'c' 'e'.
	p(NT(364), (T(14)+T(51)+T(17)+T(46)+T(13)));
//G573: trace_sym(363)       => __E_severity_147(364).
	p(NT(363), (NT(364)));
//G574: severity(356)        => trace_sym(363).
	p(NT(356), (NT(363)));
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
		__E_start_0, inputs, builder, library, rr, _, rec_relations, main, wff, __E_rec_relations_1, 
		rec_relation, __E_rec_relations_2, ref, __E_rec_relation_3, capture, bf, sym, __E_ref_4, offsets, ref_args, 
		__E_ref_args_5, __E___E_ref_args_5_6, ref_arg, __E___E_ref_args_5_7, __E___E_ref_args_5_8, __E_inputs_9, __E_inputs_10, input, in, source_binding, 
		rules, __E_library_11, __E___E_library_11_12, rule, __E___E___E_library_11_12_13, wff_rule, bf_rule, __E___E_library_11_14, wff_matcher, wff_body, 
		__E_wff_rule_15, wff_cb, bf_matcher, bf_body, __E_bf_rule_16, bf_cb, builder_head, builder_body, __E_builder_head_17, __, 
		__E_builder_head_18, bf_builder_body, __E_builder_body_19, wff_builder_body, __E_builder_body_20, wff_parenthesis, __E_wff_21, wff_sometimes, __E_wff_22, __E___E_wff_22_23, 
		wff_always, __E_wff_24, __E___E_wff_24_25, wff_conditional, __E_wff_26, wff_all, __E_wff_27, q_vars, wff_ex, __E_wff_28, 
		wff_ref, wff_imply, __E_wff_29, wff_equiv, __E_wff_30, wff_or, __E_wff_31, wff_xor, __E_wff_32, wff_and, 
		__E_wff_33, wff_neg, __E_wff_34, wff_t, wff_f, bf_interval, __E_wff_35, bf_neq, __E_wff_36, bf_eq, 
		__E_wff_37, bf_nleq, __E_wff_38, bf_greater, __E_wff_39, bf_less_equal, __E_wff_40, bf_less, __E_wff_41, uninterpreted_constant, 
		bf_parenthesis, __E_bf_42, variable, bf_splitter, __E_bf_43, bf_ref, bf_or, __E_bf_44, bf_xor, __E_bf_45, 
		bf_and, __E_bf_46, __E___E_bf_46_47, bf_neg, __E_bf_48, bf_constant, __E_bf_49, constant, bf_t, bf_f, 
		charvar, __E_charvar_50, __E_charvar_51, io_var, __E_variable_52, out, in_var_name, __E_in_53, digits, offset, 
		out_var_name, __E_out_54, q_var, __E_q_vars_55, __E_q_vars_56, __E_offsets_57, __E_offsets_58, num, shift, __E_offset_59, 
		__E_shift_60, __E___E_shift_60_61, uninter_const_name, __E_uninterpreted_constant_62, __E___E_uninterpreted_constant_62_63, chars, binding, __E_binding_64, type, __E___E_binding_64_65, 
		source, named_binding, src_c, __E_source_66, __E_source_67, __E___E_source_67_68, __E___E_source_67_69, __E_chars_70, __E_digits_71, comment, 
		__E_comment_72, __E_comment_73, __E_comment_74, __E____75, __E___76, bf_cb_args1, bf_cb_arg, bf_cb_args2, bf_cb_args3, bf_cb_args4, 
		wff_cb_args1, wff_cb_arg, wff_cb_args2, wff_cb_args3, wff_cb_args4, bf_eq_cb, __E_wff_cb_77, bf_neq_cb, __E_wff_cb_78, wff_has_clashing_subformulas_cb, 
		__E_wff_cb_79, wff_has_subformula_cb, __E_wff_cb_80, wff_remove_existential_cb, __E_wff_cb_81, bf_is_zero_cb, __E_bf_cb_82, bf_is_one_cb, __E_bf_cb_83, bf_has_subformula_cb, 
		__E_bf_cb_84, bf_remove_funiversal_cb, __E_bf_cb_85, bf_remove_fexistential_cb, __E_bf_cb_86, bf_and_cb, __E_bf_cb_87, bf_or_cb, __E_bf_cb_88, bf_xor_cb, 
		__E_bf_cb_89, bf_neg_cb, __E_bf_cb_90, bf_normalize_cb, __E_bf_cb_91, cli, cli_command, __E_cli_92, __E_cli_93, quit_cmd, 
		quit_sym, __E_cli_command_94, version_cmd, version_sym, __E_cli_command_95, clear_cmd, clear_sym, __E_cli_command_96, help_cmd, __E_cli_command_97, 
		help_sym, __E___E_cli_command_97_98, __E___E_cli_command_97_99, help_arg, file_cmd, __E_cli_command_100, file_sym, q_string, valid_cmd, __E_cli_command_101, 
		valid_sym, sat_cmd, __E_cli_command_102, sat_sym, unsat_cmd, __E_cli_command_103, unsat_sym, solve_cmd, __E_cli_command_104, solve_sym, 
		wff_cmd_arg, execute_cmd, __E_cli_command_105, execute_sym, __E___E_cli_command_105_106, __E___E_cli_command_105_107, wff_typed, normalize_cmd, __E_cli_command_108, normalize_sym, 
		__E___E_cli_command_108_109, normalize_cmd_arg, subst_cmd, __E_cli_command_110, subst_sym, __E___E_cli_command_110_111, nf_cmd_arg, inst_cmd, __E_cli_command_112, inst_sym, 
		__E___E_cli_command_112_113, inst_args, dnf_cmd, __E_cli_command_114, dnf_sym, cnf_cmd, __E_cli_command_115, cnf_sym, anf_cmd, __E_cli_command_116, 
		anf_sym, nnf_cmd, __E_cli_command_117, nnf_sym, pnf_cmd, __E_cli_command_118, pnf_sym, mnf_cmd, __E_cli_command_119, mnf_sym, 
		snf_cmd, __E_cli_command_120, snf_sym, onf_cmd, __E_cli_command_121, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_122, def_sym, 
		__E___E_cli_command_122_123, def_print_cmd, __E_cli_command_124, number, def_rr_cmd, qelim_cmd, __E_cli_command_125, qelim_sym, get_cmd, __E_cli_command_126, 
		get_sym, __E___E_cli_command_126_127, option, set_cmd, __E_cli_command_128, set_sym, __E___E_cli_command_128_129, option_value, toggle_cmd, __E_cli_command_130, 
		toggle_sym, bool_option, history_list_cmd, __E_cli_command_131, history_sym, __E___E_cli_command_131_132, history_print_cmd, __E_cli_command_133, memory, history_store_cmd, 
		__E_cli_command_134, bf_typed, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, examples_sym, __E_help_arg_135, __E___E_help_arg_135_136, rel_memory, 
		__E_memory_137, __E___E_memory_137_138, memory_id, abs_memory, __E_memory_139, enum_option, severity_opt, __E_option_140, status_opt, __E_bool_option_141, 
		colors_opt, __E_bool_option_142, debug_repl_opt, __E_bool_option_143, option_value_true, option_value_false, severity, error_sym, __E_severity_144, info_sym, 
		__E_severity_145, debug_sym, __E_severity_146, trace_sym, __E_severity_147, __N_0, __N_1, __N_2, __N_3, 
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
