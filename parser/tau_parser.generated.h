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
	"rule", "__E___E___E_library_9_10_11", "wff_rule", "bf_rule", "__E___E_library_9_12", "wff_matcher", "wff_body", "bf_matcher", "bf_body", "builder", 
	"builder_head", "builder_body", "__E_builder_head_13", "__E_builder_head_14", "bf_builder_body", "__E_builder_body_15", "wff_builder_body", "__E_builder_body_16", "tau_constant_source", "__E_tau_constant_source_17", 
	"wff_parenthesis", "__E_wff_18", "wff_sometimes", "__E_wff_19", "__E___E_wff_19_20", "wff_always", "__E_wff_21", "__E___E_wff_21_22", "wff_conditional", "__E_wff_23", 
	"wff_all", "__E_wff_24", "q_vars", "wff_ex", "__E_wff_25", "wff_ref", "constraint", "wff_imply", "__E_wff_26", "wff_rimply", 
	"__E_wff_27", "wff_equiv", "__E_wff_28", "wff_or", "__E_wff_29", "wff_xor", "__E_wff_30", "wff_and", "__E_wff_31", "wff_neg", 
	"__E_wff_32", "wff_t", "wff_f", "bf_interval", "__E_wff_33", "bf_eq", "__E_wff_34", "bf_neq", "__E_wff_35", "bf_less_equal", 
	"__E_wff_36", "bf_nleq", "__E_wff_37", "bf_greater", "__E_wff_38", "bf_ngreater", "__E_wff_39", "bf_greater_equal", "__E_wff_40", "bf_ngeq", 
	"__E_wff_41", "bf_less", "__E_wff_42", "bf_nless", "__E_wff_43", "bf_parenthesis", "__E_bf_44", "bf_constant", "variable", "bf_splitter", 
	"__E_bf_45", "bf_ref", "bf_or", "__E_bf_46", "bf_xor", "__E_bf_47", "bf_t", "__E_bf_48", "__E___E_bf_48_49", "type", 
	"bf_f", "__E_bf_50", "__E___E_bf_50_51", "bf_and", "__E_bf_52", "__E___E_bf_52_53", "bf_neg", "__E_bf_54", "bf_neg_oprnd", "__E___E_bf_54_55", 
	"bf_and_nosep", "__E_bf_56", "bf_and_nosep_1st_oprnd", "__E___E_bf_56_57", "bf_and_nosep_2nd_oprnd", "__E___E_bf_56_58", "ctn_neq", "__E_constraint_59", "ctnvar", "num", 
	"ctn_eq", "__E_constraint_60", "ctn_greater_equal", "__E_constraint_61", "ctn_greater", "__E_constraint_62", "ctn_less_equal", "__E_constraint_63", "ctn_less", "__E_constraint_64", 
	"__E_variable_65", "__E_variable_66", "__E_variable_67", "__E_variable_68", "__E_variable_69", "io_var", "__E_variable_70", "in", "out", "uninterpreted_constant", 
	"in_var_name", "__E_in_71", "__E___E_in_71_72", "offset", "out_var_name", "__E_out_73", "__E___E_out_73_74", "chars", "q_var", "__E_q_vars_75", 
	"__E_q_vars_76", "__E_offsets_77", "__E_offsets_78", "shift", "__E_offset_79", "__E_shift_80", "__E___E_shift_80_81", "digits", "uninter_const_name", "__E_uninterpreted_constant_82", 
	"__E___E_uninterpreted_constant_82_83", "constant", "__E_bf_constant_84", "binding", "__E_bf_constant_85", "source", "__E_binding_86", "src_c", "__E___E_binding_86_87", "__E___E___E_binding_86_87_88", 
	"__E___E___E_binding_86_87_89", "__E_src_c_90", "__E_src_c_91", "__E_chars_92", "__E_chars_93", "__E_digits_94", "comment", "__E_comment_95", "__E_comment_96", "__E_comment_97", 
	"__E____98", "__E___99", "cli", "cli_command", "__E_cli_100", "__E_cli_101", "quit_cmd", "quit_sym", "__E_cli_command_102", "version_cmd", 
	"version_sym", "__E_cli_command_103", "clear_cmd", "clear_sym", "__E_cli_command_104", "help_cmd", "__E_cli_command_105", "help_sym", "__E___E_cli_command_105_106", "__E___E_cli_command_105_107", 
	"help_arg", "file_cmd", "__E_cli_command_108", "file_sym", "q_string", "valid_cmd", "__E_cli_command_109", "valid_sym", "normalize_cmd_arg", "sat_cmd", 
	"__E_cli_command_110", "sat_sym", "unsat_cmd", "__E_cli_command_111", "unsat_sym", "solve_cmd", "__E_cli_command_112", "solve_sym", "__E___E_cli_command_112_113", "solve_options", 
	"wff_cmd_arg", "lgrs_cmd", "__E_cli_command_114", "lgrs_sym", "run_cmd", "__E_cli_command_115", "run_sym", "__E___E_cli_command_115_116", "__E___E_cli_command_115_117", "memory", 
	"normalize_cmd", "__E_cli_command_118", "normalize_sym", "__E___E_cli_command_118_119", "subst_cmd", "__E_cli_command_120", "subst_sym", "__E___E_cli_command_120_121", "nf_cmd_arg", "inst_cmd", 
	"__E_cli_command_122", "inst_sym", "__E___E_cli_command_122_123", "inst_args", "dnf_cmd", "__E_cli_command_124", "dnf_sym", "cnf_cmd", "__E_cli_command_125", "cnf_sym", 
	"anf_cmd", "__E_cli_command_126", "anf_sym", "nnf_cmd", "__E_cli_command_127", "nnf_sym", "pnf_cmd", "__E_cli_command_128", "pnf_sym", "mnf_cmd", 
	"__E_cli_command_129", "mnf_sym", "snf_cmd", "__E_cli_command_130", "snf_sym", "onf_cmd", "__E_cli_command_131", "onf_sym", "onf_cmd_arg", "def_list_cmd", 
	"__E_cli_command_132", "def_sym", "__E___E_cli_command_132_133", "def_print_cmd", "__E_cli_command_134", "number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_135", "qelim_sym", 
	"get_cmd", "__E_cli_command_136", "get_sym", "__E___E_cli_command_136_137", "option_name", "set_cmd", "__E_cli_command_138", "set_sym", "__E___E_cli_command_138_139", "option_value", 
	"enable_cmd", "__E_cli_command_140", "enable_sym", "disable_cmd", "__E_cli_command_141", "disable_sym", "toggle_cmd", "__E_cli_command_142", "toggle_sym", "def_input_cmd", 
	"__E_cli_command_143", "input_stream", "def_output_cmd", "__E_cli_command_144", "output_stream", "history_list_cmd", "__E_cli_command_145", "history_sym", "__E___E_cli_command_145_146", "history_print_cmd", 
	"__E_cli_command_147", "history_store_cmd", "__E_cli_command_148", "__E_solve_options_149", "solve_option", "__E_solve_options_150", "__E_solve_option_151", "solver_mode", "__E_solve_option_152", "solver_mode_minimum", 
	"__E_solver_mode_153", "solver_mode_maximum", "__E_solver_mode_154", "__E_input_stream_155", "q_file_name", "console_sym", "__E_output_stream_156", "file_name", "__E_file_name_157", "__E_option_name_158", 
	"__E_option_value_159", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "execute_sym", "examples_sym", "__E_help_arg_160", "__E___E_help_arg_160_161", "rel_memory", 
	"__E_memory_162", "__E___E_memory_162_163", "memory_id", "abs_memory", "__E_memory_164", "named_binding", "__N_0", "__N_1", "__N_2", "__N_3", 
	"__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'f', 'a', 'l', 'b', 'c', 
	'k', 'i', 'r', 's', 't', ',', '(', ')', 'o', 'm', 
	'e', '<', '>', 'w', 'y', '[', ']', '?', 'x', '-', 
	'|', '^', '&', '!', 'T', 'F', 'S', '+', '1', '0', 
	'\'', '_', '$', '{', '}', '\t', '\n', '\r', '#', 'q', 
	'u', 'v', 'n', 'h', 'p', 'd', 'g', 'z', '/', '"', 
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
			11, 27
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			20, 22, 118, 129, 148, 170, 174, 187, 188, 195,
			314, 319, 357, 375
		},
		.to_inline = {
			{ 14, 60, 14 },
			{ 21, 115, 21 },
			{ 138, 115, 21 },
			{ 140, 115, 133 },
			{ 142, 115, 21 },
			{ 144, 115, 21 },
			{ 177 },
			{ 178 },
			{ 197 },
			{ 238 },
			{ 250 },
			{ 268 },
			{ 273 },
			{ 298 },
			{ 361 }
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
//G12:  __E_ref_3(23)        => offsets(24).
	p(NT(23), (NT(24)));
//G13:  __E_ref_3(23)        => null.
	p(NT(23), (nul));
//G14:  __E_ref_4(26)        => _(11) 'f' 'a' 'l' 'l' 'b' 'a' 'c' 'k' __(27) fp_fallback(28).
	p(NT(26), (NT(11)+T(4)+T(5)+T(6)+T(6)+T(7)+T(5)+T(8)+T(9)+NT(27)+NT(28)));
//G15:  __E_ref_4(26)        => null.
	p(NT(26), (nul));
//G16:  ref(18)              => sym(22) __E_ref_3(23) ref_args(25) __E_ref_4(26).
	p(NT(18), (NT(22)+NT(23)+NT(25)+NT(26)));
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
//G43:  wff_body(46)         => wff(14).
	p(NT(46), (NT(14)));
//G44:  wff_rule(42)         => wff_matcher(45) _(11) ':' ':' '=' _(11) wff_body(46) _(11) '.'.
	p(NT(42), (NT(45)+NT(11)+T(2)+T(2)+T(3)+NT(11)+NT(46)+NT(11)+T(1)));
//G45:  bf_matcher(47)       => bf(21).
	p(NT(47), (NT(21)));
//G46:  bf_body(48)          => bf(21).
	p(NT(48), (NT(21)));
//G47:  bf_rule(43)          => bf_matcher(47) _(11) ':' '=' _(11) bf_body(48) _(11) '.'.
	p(NT(43), (NT(47)+NT(11)+T(2)+T(3)+NT(11)+NT(48)+NT(11)+T(1)));
//G48:  builder(49)          => _(11) builder_head(50) _(11) builder_body(51) _(11) '.'.
	p(NT(49), (NT(11)+NT(50)+NT(11)+NT(51)+NT(11)+T(1)));
//G49:  __E_builder_head_13(52) => __(27) capture(20).
	p(NT(52), (NT(27)+NT(20)));
//G50:  __E_builder_head_14(53) => null.
	p(NT(53), (nul));
//G51:  __E_builder_head_14(53) => __E_builder_head_13(52) __E_builder_head_14(53).
	p(NT(53), (NT(52)+NT(53)));
//G52:  builder_head(50)     => '(' _(11) capture(20) __E_builder_head_14(53) _(11) ')'.
	p(NT(50), (T(15)+NT(11)+NT(20)+NT(53)+NT(11)+T(16)));
//G53:  __E_builder_body_15(55) => '=' ':' _(11) bf(21).
	p(NT(55), (T(3)+T(2)+NT(11)+NT(21)));
//G54:  bf_builder_body(54)  => __E_builder_body_15(55).
	p(NT(54), (NT(55)));
//G55:  builder_body(51)     => bf_builder_body(54).
	p(NT(51), (NT(54)));
//G56:  __E_builder_body_16(57) => '=' ':' ':' _(11) wff(14).
	p(NT(57), (T(3)+T(2)+T(2)+NT(11)+NT(14)));
//G57:  wff_builder_body(56) => __E_builder_body_16(57).
	p(NT(56), (NT(57)));
//G58:  builder_body(51)     => wff_builder_body(56).
	p(NT(51), (NT(56)));
//G59:  __E_tau_constant_source_17(59) => '.' _(11).
	p(NT(59), (T(1)+NT(11)));
//G60:  __E_tau_constant_source_17(59) => null.
	p(NT(59), (nul));
//G61:  tau_constant_source(58) => rec_relations(12) _(11) main(13) _(11) __E_tau_constant_source_17(59).
	p(NT(58), (NT(12)+NT(11)+NT(13)+NT(11)+NT(59)));
//G62:  __E_wff_18(61)       => '(' _(11) wff(14) _(11) ')'.
	p(NT(61), (T(15)+NT(11)+NT(14)+NT(11)+T(16)));
//G63:  wff_parenthesis(60)  => __E_wff_18(61).
	p(NT(60), (NT(61)));
//G64:  wff(14)              => wff_parenthesis(60).
	p(NT(14), (NT(60)));
//G65:  __E___E_wff_19_20(64) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(64), (T(12)+T(17)+T(18)+T(19)+T(13)+T(10)+T(18)+T(19)+T(12)));
//G66:  __E___E_wff_19_20(64) => '<' '>'.
	p(NT(64), (T(20)+T(21)));
//G67:  __E_wff_19(63)       => __E___E_wff_19_20(64) _(11) wff(14).
	p(NT(63), (NT(64)+NT(11)+NT(14)));
//G68:  wff_sometimes(62)    => __E_wff_19(63).
	p(NT(62), (NT(63)));
//G69:  wff(14)              => wff_sometimes(62).
	p(NT(14), (NT(62)));
//G70:  __E___E_wff_21_22(67) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(67), (T(5)+T(6)+T(22)+T(5)+T(23)+T(12)));
//G71:  __E___E_wff_21_22(67) => '[' ']'.
	p(NT(67), (T(24)+T(25)));
//G72:  __E_wff_21(66)       => __E___E_wff_21_22(67) _(11) wff(14).
	p(NT(66), (NT(67)+NT(11)+NT(14)));
//G73:  wff_always(65)       => __E_wff_21(66).
	p(NT(65), (NT(66)));
//G74:  wff(14)              => wff_always(65).
	p(NT(14), (NT(65)));
//G75:  __E_wff_23(69)       => wff(14) _(11) '?' _(11) wff(14) _(11) ':' _(11) wff(14).
	p(NT(69), (NT(14)+NT(11)+T(26)+NT(11)+NT(14)+NT(11)+T(2)+NT(11)+NT(14)));
//G76:  wff_conditional(68)  => __E_wff_23(69).
	p(NT(68), (NT(69)));
//G77:  wff(14)              => wff_conditional(68).
	p(NT(14), (NT(68)));
//G78:  __E_wff_24(71)       => 'a' 'l' 'l' __(27) q_vars(72) __(27) wff(14).
	p(NT(71), (T(5)+T(6)+T(6)+NT(27)+NT(72)+NT(27)+NT(14)));
//G79:  wff_all(70)          => __E_wff_24(71).
	p(NT(70), (NT(71)));
//G80:  wff(14)              => wff_all(70).
	p(NT(14), (NT(70)));
//G81:  __E_wff_25(74)       => 'e' 'x' __(27) q_vars(72) __(27) wff(14).
	p(NT(74), (T(19)+T(27)+NT(27)+NT(72)+NT(27)+NT(14)));
//G82:  wff_ex(73)           => __E_wff_25(74).
	p(NT(73), (NT(74)));
//G83:  wff(14)              => wff_ex(73).
	p(NT(14), (NT(73)));
//G84:  wff_ref(75)          => ref(18).
	p(NT(75), (NT(18)));
//G85:  wff(14)              => wff_ref(75).
	p(NT(14), (NT(75)));
//G86:  wff(14)              => constraint(76).
	p(NT(14), (NT(76)));
//G87:  __E_wff_26(78)       => wff(14) _(11) '-' '>' _(11) wff(14).
	p(NT(78), (NT(14)+NT(11)+T(28)+T(21)+NT(11)+NT(14)));
//G88:  wff_imply(77)        => __E_wff_26(78).
	p(NT(77), (NT(78)));
//G89:  wff(14)              => wff_imply(77).
	p(NT(14), (NT(77)));
//G90:  __E_wff_27(80)       => wff(14) _(11) '<' '-' _(11) wff(14).
	p(NT(80), (NT(14)+NT(11)+T(20)+T(28)+NT(11)+NT(14)));
//G91:  wff_rimply(79)       => __E_wff_27(80).
	p(NT(79), (NT(80)));
//G92:  wff(14)              => wff_rimply(79).
	p(NT(14), (NT(79)));
//G93:  __E_wff_28(82)       => wff(14) _(11) '<' '-' '>' _(11) wff(14).
	p(NT(82), (NT(14)+NT(11)+T(20)+T(28)+T(21)+NT(11)+NT(14)));
//G94:  wff_equiv(81)        => __E_wff_28(82).
	p(NT(81), (NT(82)));
//G95:  wff(14)              => wff_equiv(81).
	p(NT(14), (NT(81)));
//G96:  __E_wff_29(84)       => wff(14) _(11) '|' '|' _(11) wff(14).
	p(NT(84), (NT(14)+NT(11)+T(29)+T(29)+NT(11)+NT(14)));
//G97:  wff_or(83)           => __E_wff_29(84).
	p(NT(83), (NT(84)));
//G98:  wff(14)              => wff_or(83).
	p(NT(14), (NT(83)));
//G99:  __E_wff_30(86)       => wff(14) _(11) '^' _(11) wff(14).
	p(NT(86), (NT(14)+NT(11)+T(30)+NT(11)+NT(14)));
//G100: wff_xor(85)          => __E_wff_30(86).
	p(NT(85), (NT(86)));
//G101: wff(14)              => wff_xor(85).
	p(NT(14), (NT(85)));
//G102: __E_wff_31(88)       => wff(14) _(11) '&' '&' _(11) wff(14).
	p(NT(88), (NT(14)+NT(11)+T(31)+T(31)+NT(11)+NT(14)));
//G103: wff_and(87)          => __E_wff_31(88).
	p(NT(87), (NT(88)));
//G104: wff(14)              => wff_and(87).
	p(NT(14), (NT(87)));
//G105: __E_wff_32(90)       => '!' _(11) wff(14).
	p(NT(90), (T(32)+NT(11)+NT(14)));
//G106: wff_neg(89)          => __E_wff_32(90).
	p(NT(89), (NT(90)));
//G107: wff(14)              => wff_neg(89).
	p(NT(14), (NT(89)));
//G108: wff_t(91)            => 'T'.
	p(NT(91), (T(33)));
//G109: wff(14)              => wff_t(91).
	p(NT(14), (NT(91)));
//G110: wff_f(92)            => 'F'.
	p(NT(92), (T(34)));
//G111: wff(14)              => wff_f(92).
	p(NT(14), (NT(92)));
//G112: __E_wff_33(94)       => bf(21) _(11) '<' '=' _(11) bf(21) _(11) '<' '=' _(11) bf(21).
	p(NT(94), (NT(21)+NT(11)+T(20)+T(3)+NT(11)+NT(21)+NT(11)+T(20)+T(3)+NT(11)+NT(21)));
//G113: bf_interval(93)      => __E_wff_33(94).
	p(NT(93), (NT(94)));
//G114: wff(14)              => bf_interval(93).
	p(NT(14), (NT(93)));
//G115: __E_wff_34(96)       => bf(21) _(11) '=' _(11) bf(21).
	p(NT(96), (NT(21)+NT(11)+T(3)+NT(11)+NT(21)));
//G116: bf_eq(95)            => __E_wff_34(96).
	p(NT(95), (NT(96)));
//G117: wff(14)              => bf_eq(95).
	p(NT(14), (NT(95)));
//G118: __E_wff_35(98)       => bf(21) _(11) '!' '=' _(11) bf(21).
	p(NT(98), (NT(21)+NT(11)+T(32)+T(3)+NT(11)+NT(21)));
//G119: bf_neq(97)           => __E_wff_35(98).
	p(NT(97), (NT(98)));
//G120: wff(14)              => bf_neq(97).
	p(NT(14), (NT(97)));
//G121: __E_wff_36(100)      => bf(21) _(11) '<' '=' _(11) bf(21).
	p(NT(100), (NT(21)+NT(11)+T(20)+T(3)+NT(11)+NT(21)));
//G122: bf_less_equal(99)    => __E_wff_36(100).
	p(NT(99), (NT(100)));
//G123: wff(14)              => bf_less_equal(99).
	p(NT(14), (NT(99)));
//G124: __E_wff_37(102)      => bf(21) _(11) '!' '<' '=' _(11) bf(21).
	p(NT(102), (NT(21)+NT(11)+T(32)+T(20)+T(3)+NT(11)+NT(21)));
//G125: bf_nleq(101)         => __E_wff_37(102).
	p(NT(101), (NT(102)));
//G126: wff(14)              => bf_nleq(101).
	p(NT(14), (NT(101)));
//G127: __E_wff_38(104)      => bf(21) _(11) '>' _(11) bf(21).
	p(NT(104), (NT(21)+NT(11)+T(21)+NT(11)+NT(21)));
//G128: bf_greater(103)      => __E_wff_38(104).
	p(NT(103), (NT(104)));
//G129: wff(14)              => bf_greater(103).
	p(NT(14), (NT(103)));
//G130: __E_wff_39(106)      => bf(21) _(11) '!' '>' _(11) bf(21).
	p(NT(106), (NT(21)+NT(11)+T(32)+T(21)+NT(11)+NT(21)));
//G131: bf_ngreater(105)     => __E_wff_39(106).
	p(NT(105), (NT(106)));
//G132: wff(14)              => bf_ngreater(105).
	p(NT(14), (NT(105)));
//G133: __E_wff_40(108)      => bf(21) _(11) '>' '=' _(11) bf(21).
	p(NT(108), (NT(21)+NT(11)+T(21)+T(3)+NT(11)+NT(21)));
//G134: bf_greater_equal(107) => __E_wff_40(108).
	p(NT(107), (NT(108)));
//G135: wff(14)              => bf_greater_equal(107).
	p(NT(14), (NT(107)));
//G136: __E_wff_41(110)      => bf(21) _(11) '!' '>' '=' _(11) bf(21).
	p(NT(110), (NT(21)+NT(11)+T(32)+T(21)+T(3)+NT(11)+NT(21)));
//G137: bf_ngeq(109)         => __E_wff_41(110).
	p(NT(109), (NT(110)));
//G138: wff(14)              => bf_ngeq(109).
	p(NT(14), (NT(109)));
//G139: __E_wff_42(112)      => bf(21) _(11) '<' _(11) bf(21).
	p(NT(112), (NT(21)+NT(11)+T(20)+NT(11)+NT(21)));
//G140: bf_less(111)         => __E_wff_42(112).
	p(NT(111), (NT(112)));
//G141: wff(14)              => bf_less(111).
	p(NT(14), (NT(111)));
//G142: __E_wff_43(114)      => bf(21) _(11) '!' '<' _(11) bf(21).
	p(NT(114), (NT(21)+NT(11)+T(32)+T(20)+NT(11)+NT(21)));
//G143: bf_nless(113)        => __E_wff_43(114).
	p(NT(113), (NT(114)));
//G144: wff(14)              => bf_nless(113).
	p(NT(14), (NT(113)));
//G145: wff(14)              => capture(20).
	p(NT(14), (NT(20)));
//G146: __E_bf_44(116)       => '(' _(11) bf(21) _(11) ')'.
	p(NT(116), (T(15)+NT(11)+NT(21)+NT(11)+T(16)));
//G147: bf_parenthesis(115)  => __E_bf_44(116).
	p(NT(115), (NT(116)));
//G148: bf(21)               => bf_parenthesis(115).
	p(NT(21), (NT(115)));
//G149: bf(21)               => bf_constant(117).
	p(NT(21), (NT(117)));
//G150: bf(21)               => variable(118).
	p(NT(21), (NT(118)));
//G151: __E_bf_45(120)       => 'S' _(11) '(' _(11) bf(21) _(11) ')'.
	p(NT(120), (T(35)+NT(11)+T(15)+NT(11)+NT(21)+NT(11)+T(16)));
//G152: bf_splitter(119)     => __E_bf_45(120).
	p(NT(119), (NT(120)));
//G153: bf(21)               => bf_splitter(119).
	p(NT(21), (NT(119)));
//G154: bf_ref(121)          => ref(18).
	p(NT(121), (NT(18)));
//G155: bf(21)               => bf_ref(121).
	p(NT(21), (NT(121)));
//G156: __E_bf_46(123)       => bf(21) _(11) '|' _(11) bf(21).
	p(NT(123), (NT(21)+NT(11)+T(29)+NT(11)+NT(21)));
//G157: bf_or(122)           => __E_bf_46(123).
	p(NT(122), (NT(123)));
//G158: bf(21)               => bf_or(122).
	p(NT(21), (NT(122)));
//G159: __E_bf_47(125)       => bf(21) _(11) '+' _(11) bf(21).
	p(NT(125), (NT(21)+NT(11)+T(36)+NT(11)+NT(21)));
//G160: bf_xor(124)          => __E_bf_47(125).
	p(NT(124), (NT(125)));
//G161: bf(21)               => bf_xor(124).
	p(NT(21), (NT(124)));
//G162: __E___E_bf_48_49(128) => _(11) ':' _(11) type(129).
	p(NT(128), (NT(11)+T(2)+NT(11)+NT(129)));
//G163: __E___E_bf_48_49(128) => null.
	p(NT(128), (nul));
//G164: __E_bf_48(127)       => '1' __E___E_bf_48_49(128).
	p(NT(127), (T(37)+NT(128)));
//G165: bf_t(126)            => __E_bf_48(127).
	p(NT(126), (NT(127)));
//G166: bf(21)               => bf_t(126).
	p(NT(21), (NT(126)));
//G167: __E___E_bf_50_51(132) => _(11) ':' _(11) type(129).
	p(NT(132), (NT(11)+T(2)+NT(11)+NT(129)));
//G168: __E___E_bf_50_51(132) => null.
	p(NT(132), (nul));
//G169: __E_bf_50(131)       => '0' __E___E_bf_50_51(132).
	p(NT(131), (T(38)+NT(132)));
//G170: bf_f(130)            => __E_bf_50(131).
	p(NT(130), (NT(131)));
//G171: bf(21)               => bf_f(130).
	p(NT(21), (NT(130)));
//G172: __E___E_bf_52_53(135) => _(11) '&' _(11).
	p(NT(135), (NT(11)+T(31)+NT(11)));
//G173: __E___E_bf_52_53(135) => __(27).
	p(NT(135), (NT(27)));
//G174: __E_bf_52(134)       => bf(21) __E___E_bf_52_53(135) bf(21).
	p(NT(134), (NT(21)+NT(135)+NT(21)));
//G175: bf_and(133)          => __E_bf_52(134).
	p(NT(133), (NT(134)));
//G176: bf(21)               => bf_and(133).
	p(NT(21), (NT(133)));
//G177: __E___E_bf_54_55(139) => bf_parenthesis(115).
	p(NT(139), (NT(115)));
//G178: __E___E_bf_54_55(139) => bf_constant(117).
	p(NT(139), (NT(117)));
//G179: __E___E_bf_54_55(139) => variable(118).
	p(NT(139), (NT(118)));
//G180: __E___E_bf_54_55(139) => bf_splitter(119).
	p(NT(139), (NT(119)));
//G181: __E___E_bf_54_55(139) => bf_ref(121).
	p(NT(139), (NT(121)));
//G182: __E___E_bf_54_55(139) => bf_t(126).
	p(NT(139), (NT(126)));
//G183: __E___E_bf_54_55(139) => bf_f(130).
	p(NT(139), (NT(130)));
//G184: __E___E_bf_54_55(139) => bf_neg(136).
	p(NT(139), (NT(136)));
//G185: __E___E_bf_54_55(139) => capture(20).
	p(NT(139), (NT(20)));
//G186: bf_neg_oprnd(138)    => __E___E_bf_54_55(139).
	p(NT(138), (NT(139)));
//G187: __E_bf_54(137)       => bf_neg_oprnd(138) _(11) '\''.
	p(NT(137), (NT(138)+NT(11)+T(39)));
//G188: bf_neg(136)          => __E_bf_54(137).
	p(NT(136), (NT(137)));
//G189: bf(21)               => bf_neg(136).
	p(NT(21), (NT(136)));
//G190: __E___E_bf_56_57(143) => bf_parenthesis(115).
	p(NT(143), (NT(115)));
//G191: __E___E_bf_56_57(143) => variable(118).
	p(NT(143), (NT(118)));
//G192: __E___E_bf_56_57(143) => bf_splitter(119).
	p(NT(143), (NT(119)));
//G193: __E___E_bf_56_57(143) => bf_ref(121).
	p(NT(143), (NT(121)));
//G194: __E___E_bf_56_57(143) => bf_neg(136).
	p(NT(143), (NT(136)));
//G195: __E___E_bf_56_57(143) => bf_and_nosep(140).
	p(NT(143), (NT(140)));
//G196: __E___E_bf_56_57(143) => capture(20).
	p(NT(143), (NT(20)));
//G197: bf_and_nosep_1st_oprnd(142) => __E___E_bf_56_57(143).
	p(NT(142), (NT(143)));
//G198: __E___E_bf_56_58(145) => bf_parenthesis(115).
	p(NT(145), (NT(115)));
//G199: __E___E_bf_56_58(145) => bf_constant(117).
	p(NT(145), (NT(117)));
//G200: __E___E_bf_56_58(145) => variable(118).
	p(NT(145), (NT(118)));
//G201: __E___E_bf_56_58(145) => bf_splitter(119).
	p(NT(145), (NT(119)));
//G202: __E___E_bf_56_58(145) => bf_ref(121).
	p(NT(145), (NT(121)));
//G203: __E___E_bf_56_58(145) => bf_neg(136).
	p(NT(145), (NT(136)));
//G204: __E___E_bf_56_58(145) => capture(20).
	p(NT(145), (NT(20)));
//G205: bf_and_nosep_2nd_oprnd(144) => __E___E_bf_56_58(145).
	p(NT(144), (NT(145)));
//G206: __E_bf_56(141)       => bf_and_nosep_1st_oprnd(142) bf_and_nosep_2nd_oprnd(144).
	p(NT(141), (NT(142)+NT(144)));
//G207: bf_and_nosep(140)    => __E_bf_56(141).
	p(NT(140), (NT(141)));
//G208: bf(21)               => bf_and_nosep(140).
	p(NT(21), (NT(140)));
//G209: bf(21)               => capture(20).
	p(NT(21), (NT(20)));
//G210: __E_constraint_59(147) => '[' ctnvar(148) _(11) '!' '=' _(11) num(149) ']'.
	p(NT(147), (T(24)+NT(148)+NT(11)+T(32)+T(3)+NT(11)+NT(149)+T(25)));
//G211: __E_constraint_59(147) => '[' num(149) _(11) '!' '=' _(11) ctnvar(148) ']'.
	p(NT(147), (T(24)+NT(149)+NT(11)+T(32)+T(3)+NT(11)+NT(148)+T(25)));
//G212: ctn_neq(146)         => __E_constraint_59(147).
	p(NT(146), (NT(147)));
//G213: constraint(76)       => ctn_neq(146).
	p(NT(76), (NT(146)));
//G214: __E_constraint_60(151) => '[' ctnvar(148) _(11) '=' _(11) num(149) ']'.
	p(NT(151), (T(24)+NT(148)+NT(11)+T(3)+NT(11)+NT(149)+T(25)));
//G215: __E_constraint_60(151) => '[' num(149) _(11) '=' _(11) ctnvar(148) ']'.
	p(NT(151), (T(24)+NT(149)+NT(11)+T(3)+NT(11)+NT(148)+T(25)));
//G216: ctn_eq(150)          => __E_constraint_60(151).
	p(NT(150), (NT(151)));
//G217: constraint(76)       => ctn_eq(150).
	p(NT(76), (NT(150)));
//G218: __E_constraint_61(153) => '[' ctnvar(148) _(11) '>' '=' _(11) num(149) ']'.
	p(NT(153), (T(24)+NT(148)+NT(11)+T(21)+T(3)+NT(11)+NT(149)+T(25)));
//G219: __E_constraint_61(153) => '[' num(149) _(11) '>' '=' _(11) ctnvar(148) ']'.
	p(NT(153), (T(24)+NT(149)+NT(11)+T(21)+T(3)+NT(11)+NT(148)+T(25)));
//G220: ctn_greater_equal(152) => __E_constraint_61(153).
	p(NT(152), (NT(153)));
//G221: constraint(76)       => ctn_greater_equal(152).
	p(NT(76), (NT(152)));
//G222: __E_constraint_62(155) => '[' ctnvar(148) _(11) '>' _(11) num(149) ']'.
	p(NT(155), (T(24)+NT(148)+NT(11)+T(21)+NT(11)+NT(149)+T(25)));
//G223: __E_constraint_62(155) => '[' num(149) _(11) '>' _(11) ctnvar(148) ']'.
	p(NT(155), (T(24)+NT(149)+NT(11)+T(21)+NT(11)+NT(148)+T(25)));
//G224: ctn_greater(154)     => __E_constraint_62(155).
	p(NT(154), (NT(155)));
//G225: constraint(76)       => ctn_greater(154).
	p(NT(76), (NT(154)));
//G226: __E_constraint_63(157) => '[' ctnvar(148) _(11) '<' '=' _(11) num(149) ']'.
	p(NT(157), (T(24)+NT(148)+NT(11)+T(20)+T(3)+NT(11)+NT(149)+T(25)));
//G227: __E_constraint_63(157) => '[' num(149) _(11) '<' '=' _(11) ctnvar(148) ']'.
	p(NT(157), (T(24)+NT(149)+NT(11)+T(20)+T(3)+NT(11)+NT(148)+T(25)));
//G228: ctn_less_equal(156)  => __E_constraint_63(157).
	p(NT(156), (NT(157)));
//G229: constraint(76)       => ctn_less_equal(156).
	p(NT(76), (NT(156)));
//G230: __E_constraint_64(159) => '[' ctnvar(148) _(11) '<' _(11) num(149) ']'.
	p(NT(159), (T(24)+NT(148)+NT(11)+T(20)+NT(11)+NT(149)+T(25)));
//G231: __E_constraint_64(159) => '[' num(149) _(11) '<' _(11) ctnvar(148) ']'.
	p(NT(159), (T(24)+NT(149)+NT(11)+T(20)+NT(11)+NT(148)+T(25)));
//G232: ctn_less(158)        => __E_constraint_64(159).
	p(NT(158), (NT(159)));
//G233: constraint(76)       => ctn_less(158).
	p(NT(76), (NT(158)));
//G234: __N_0(376)           => 'F'.
	p(NT(376), (T(34)));
//G235: __N_1(377)           => 'T'.
	p(NT(377), (T(33)));
//G236: __E_variable_65(160) => ~( __N_0(376) ) & ~( __N_1(377) ) & alpha(5).	 # conjunctive
	p(NT(160), ~(NT(376)) & ~(NT(377)) & (NT(5)));
//G237: __E_variable_66(161) => null.
	p(NT(161), (nul));
//G238: __E_variable_66(161) => digit(3) __E_variable_66(161).
	p(NT(161), (NT(3)+NT(161)));
//G239: variable(118)        => __E_variable_65(160) __E_variable_66(161).	 # guarded: charvar
	p(NT(118), (NT(160)+NT(161)));
	p.back().guard = "charvar";
//G240: __N_2(378)           => 'F'.
	p(NT(378), (T(34)));
//G241: __N_3(379)           => 'T'.
	p(NT(379), (T(33)));
//G242: __E_variable_67(162) => ~( __N_2(378) ) & ~( __N_3(379) ) & alpha(5).	 # conjunctive
	p(NT(162), ~(NT(378)) & ~(NT(379)) & (NT(5)));
//G243: __E_variable_68(163) => alnum(6).
	p(NT(163), (NT(6)));
//G244: __E_variable_68(163) => '_'.
	p(NT(163), (T(40)));
//G245: __E_variable_69(164) => null.
	p(NT(164), (nul));
//G246: __E_variable_69(164) => __E_variable_68(163) __E_variable_69(164).
	p(NT(164), (NT(163)+NT(164)));
//G247: variable(118)        => __E_variable_67(162) __E_variable_69(164).	 # guarded: var
	p(NT(118), (NT(162)+NT(164)));
	p.back().guard = "var";
//G248: __E_variable_70(166) => in(167).
	p(NT(166), (NT(167)));
//G249: __E_variable_70(166) => out(168).
	p(NT(166), (NT(168)));
//G250: io_var(165)          => __E_variable_70(166).
	p(NT(165), (NT(166)));
//G251: variable(118)        => io_var(165).
	p(NT(118), (NT(165)));
//G252: variable(118)        => uninterpreted_constant(169).
	p(NT(118), (NT(169)));
//G253: __E___E_in_71_72(172) => digit(3).
	p(NT(172), (NT(3)));
//G254: __E___E_in_71_72(172) => digit(3) __E___E_in_71_72(172).
	p(NT(172), (NT(3)+NT(172)));
//G255: __E_in_71(171)       => 'i' __E___E_in_71_72(172).
	p(NT(171), (T(10)+NT(172)));
//G256: in_var_name(170)     => __E_in_71(171).
	p(NT(170), (NT(171)));
//G257: in(167)              => in_var_name(170) '[' offset(173) ']'.
	p(NT(167), (NT(170)+T(24)+NT(173)+T(25)));
//G258: __E___E_out_73_74(176) => digit(3).
	p(NT(176), (NT(3)));
//G259: __E___E_out_73_74(176) => digit(3) __E___E_out_73_74(176).
	p(NT(176), (NT(3)+NT(176)));
//G260: __E_out_73(175)      => 'o' __E___E_out_73_74(176).
	p(NT(175), (T(17)+NT(176)));
//G261: out_var_name(174)    => __E_out_73(175).
	p(NT(174), (NT(175)));
//G262: out(168)             => out_var_name(174) '[' offset(173) ']'.
	p(NT(168), (NT(174)+T(24)+NT(173)+T(25)));
//G263: ctnvar(148)          => chars(177).
	p(NT(148), (NT(177)));
//G264: capture(20)          => '$' chars(177).
	p(NT(20), (T(41)+NT(177)));
//G265: __E_q_vars_75(179)   => _(11) ',' _(11) q_var(178).
	p(NT(179), (NT(11)+T(14)+NT(11)+NT(178)));
//G266: __E_q_vars_76(180)   => null.
	p(NT(180), (nul));
//G267: __E_q_vars_76(180)   => __E_q_vars_75(179) __E_q_vars_76(180).
	p(NT(180), (NT(179)+NT(180)));
//G268: q_vars(72)           => q_var(178) __E_q_vars_76(180).
	p(NT(72), (NT(178)+NT(180)));
//G269: q_var(178)           => capture(20).
	p(NT(178), (NT(20)));
//G270: __N_4(380)           => uninterpreted_constant(169).
	p(NT(380), (NT(169)));
//G271: q_var(178)           => variable(118) & ~( __N_4(380) ).	 # conjunctive
	p(NT(178), (NT(118)) & ~(NT(380)));
//G272: __E_offsets_77(181)  => _(11) ',' _(11) offset(173).
	p(NT(181), (NT(11)+T(14)+NT(11)+NT(173)));
//G273: __E_offsets_78(182)  => null.
	p(NT(182), (nul));
//G274: __E_offsets_78(182)  => __E_offsets_77(181) __E_offsets_78(182).
	p(NT(182), (NT(181)+NT(182)));
//G275: offsets(24)          => '[' _(11) offset(173) __E_offsets_78(182) _(11) ']'.
	p(NT(24), (T(24)+NT(11)+NT(173)+NT(182)+NT(11)+T(25)));
//G276: offset(173)          => num(149).
	p(NT(173), (NT(149)));
//G277: offset(173)          => capture(20).
	p(NT(173), (NT(20)));
//G278: offset(173)          => shift(183).
	p(NT(173), (NT(183)));
//G279: __N_5(381)           => io_var(165).
	p(NT(381), (NT(165)));
//G280: __E_offset_79(184)   => variable(118) & ~( __N_5(381) ).	 # conjunctive
	p(NT(184), (NT(118)) & ~(NT(381)));
//G281: offset(173)          => __E_offset_79(184).
	p(NT(173), (NT(184)));
//G282: __E_shift_80(185)    => capture(20).
	p(NT(185), (NT(20)));
//G283: __N_6(382)           => io_var(165).
	p(NT(382), (NT(165)));
//G284: __E___E_shift_80_81(186) => variable(118) & ~( __N_6(382) ).	 # conjunctive
	p(NT(186), (NT(118)) & ~(NT(382)));
//G285: __E_shift_80(185)    => __E___E_shift_80_81(186).
	p(NT(185), (NT(186)));
//G286: shift(183)           => __E_shift_80(185) _(11) '-' _(11) num(149).
	p(NT(183), (NT(185)+NT(11)+T(28)+NT(11)+NT(149)));
//G287: num(149)             => digits(187).
	p(NT(149), (NT(187)));
//G288: __E___E_uninterpreted_constant_82_83(190) => chars(177).
	p(NT(190), (NT(177)));
//G289: __E___E_uninterpreted_constant_82_83(190) => _(11).
	p(NT(190), (NT(11)));
//G290: __E_uninterpreted_constant_82(189) => __E___E_uninterpreted_constant_82_83(190) ':' chars(177).
	p(NT(189), (NT(190)+T(2)+NT(177)));
//G291: uninter_const_name(188) => __E_uninterpreted_constant_82(189).
	p(NT(188), (NT(189)));
//G292: uninterpreted_constant(169) => '<' uninter_const_name(188) _(11) '>'.
	p(NT(169), (T(20)+NT(188)+NT(11)+T(21)));
//G293: __E_bf_constant_84(192) => capture(20).
	p(NT(192), (NT(20)));
//G294: __E_bf_constant_84(192) => binding(193).
	p(NT(192), (NT(193)));
//G295: constant(191)        => __E_bf_constant_84(192).
	p(NT(191), (NT(192)));
//G296: type(129)            => chars(177).
	p(NT(129), (NT(177)));
//G297: __E_bf_constant_85(194) => _(11) ':' _(11) type(129).
	p(NT(194), (NT(11)+T(2)+NT(11)+NT(129)));
//G298: __E_bf_constant_85(194) => null.
	p(NT(194), (nul));
//G299: bf_constant(117)     => '{' _(11) constant(191) _(11) '}' __E_bf_constant_85(194).
	p(NT(117), (T(42)+NT(11)+NT(191)+NT(11)+T(43)+NT(194)));
//G300: __E___E___E_binding_86_87_88(199) => src_c(197).
	p(NT(199), (NT(197)));
//G301: __E___E___E_binding_86_87_88(199) => space(2).
	p(NT(199), (NT(2)));
//G302: __E___E___E_binding_86_87_89(200) => null.
	p(NT(200), (nul));
//G303: __E___E___E_binding_86_87_89(200) => __E___E___E_binding_86_87_88(199) __E___E___E_binding_86_87_89(200).
	p(NT(200), (NT(199)+NT(200)));
//G304: __E___E_binding_86_87(198) => __E___E___E_binding_86_87_89(200) src_c(197).
	p(NT(198), (NT(200)+NT(197)));
//G305: __E___E_binding_86_87(198) => null.
	p(NT(198), (nul));
//G306: __E_binding_86(196)  => src_c(197) __E___E_binding_86_87(198).
	p(NT(196), (NT(197)+NT(198)));
//G307: source(195)          => __E_binding_86(196).
	p(NT(195), (NT(196)));
//G308: binding(193)         => source(195).
	p(NT(193), (NT(195)));
//G309: src_c(197)           => alnum(6).
	p(NT(197), (NT(6)));
//G310: __N_7(383)           => '{'.
	p(NT(383), (T(42)));
//G311: __N_8(384)           => '}'.
	p(NT(384), (T(43)));
//G312: src_c(197)           => ~( __N_7(383) ) & ~( __N_8(384) ) & punct(7).	 # conjunctive
	p(NT(197), ~(NT(383)) & ~(NT(384)) & (NT(7)));
//G313: __E_src_c_90(201)    => src_c(197).
	p(NT(201), (NT(197)));
//G314: __E_src_c_90(201)    => space(2).
	p(NT(201), (NT(2)));
//G315: __E_src_c_91(202)    => null.
	p(NT(202), (nul));
//G316: __E_src_c_91(202)    => __E_src_c_90(201) __E_src_c_91(202).
	p(NT(202), (NT(201)+NT(202)));
//G317: src_c(197)           => '{' __E_src_c_91(202) '}'.
	p(NT(197), (T(42)+NT(202)+T(43)));
//G318: __E_chars_92(203)    => alnum(6).
	p(NT(203), (NT(6)));
//G319: __E_chars_92(203)    => '_'.
	p(NT(203), (T(40)));
//G320: __E_chars_93(204)    => null.
	p(NT(204), (nul));
//G321: __E_chars_93(204)    => __E_chars_92(203) __E_chars_93(204).
	p(NT(204), (NT(203)+NT(204)));
//G322: chars(177)           => alpha(5) __E_chars_93(204).
	p(NT(177), (NT(5)+NT(204)));
//G323: __E_digits_94(205)   => digit(3).
	p(NT(205), (NT(3)));
//G324: __E_digits_94(205)   => digit(3) __E_digits_94(205).
	p(NT(205), (NT(3)+NT(205)));
//G325: digits(187)          => __E_digits_94(205).
	p(NT(187), (NT(205)));
//G326: sym(22)              => chars(177).
	p(NT(22), (NT(177)));
//G327: __E_comment_95(207)  => printable(8).
	p(NT(207), (NT(8)));
//G328: __E_comment_95(207)  => '\t'.
	p(NT(207), (T(44)));
//G329: __E_comment_96(208)  => null.
	p(NT(208), (nul));
//G330: __E_comment_96(208)  => __E_comment_95(207) __E_comment_96(208).
	p(NT(208), (NT(207)+NT(208)));
//G331: __E_comment_97(209)  => '\n'.
	p(NT(209), (T(45)));
//G332: __E_comment_97(209)  => '\r'.
	p(NT(209), (T(46)));
//G333: __E_comment_97(209)  => eof(1).
	p(NT(209), (NT(1)));
//G334: comment(206)         => '#' __E_comment_96(208) __E_comment_97(209).
	p(NT(206), (T(47)+NT(208)+NT(209)));
//G335: __E____98(210)       => space(2).
	p(NT(210), (NT(2)));
//G336: __E____98(210)       => comment(206).
	p(NT(210), (NT(206)));
//G337: __(27)               => __E____98(210) _(11).
	p(NT(27), (NT(210)+NT(11)));
//G338: __E___99(211)        => __(27).
	p(NT(211), (NT(27)));
//G339: __E___99(211)        => null.
	p(NT(211), (nul));
//G340: _(11)                => __E___99(211).
	p(NT(11), (NT(211)));
//G341: cli(212)             => _(11).
	p(NT(212), (NT(11)));
//G342: __E_cli_100(214)     => '.' _(11) cli_command(213) _(11).
	p(NT(214), (T(1)+NT(11)+NT(213)+NT(11)));
//G343: __E_cli_101(215)     => null.
	p(NT(215), (nul));
//G344: __E_cli_101(215)     => __E_cli_100(214) __E_cli_101(215).
	p(NT(215), (NT(214)+NT(215)));
//G345: cli(212)             => _(11) cli_command(213) _(11) __E_cli_101(215).
	p(NT(212), (NT(11)+NT(213)+NT(11)+NT(215)));
//G346: __E_cli_command_102(218) => 'q'.
	p(NT(218), (T(48)));
//G347: __E_cli_command_102(218) => 'q' 'u' 'i' 't'.
	p(NT(218), (T(48)+T(49)+T(10)+T(13)));
//G348: quit_sym(217)        => __E_cli_command_102(218).
	p(NT(217), (NT(218)));
//G349: quit_cmd(216)        => quit_sym(217).
	p(NT(216), (NT(217)));
//G350: cli_command(213)     => quit_cmd(216).
	p(NT(213), (NT(216)));
//G351: __E_cli_command_103(221) => 'v'.
	p(NT(221), (T(50)));
//G352: __E_cli_command_103(221) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(221), (T(50)+T(19)+T(11)+T(12)+T(10)+T(17)+T(51)));
//G353: version_sym(220)     => __E_cli_command_103(221).
	p(NT(220), (NT(221)));
//G354: version_cmd(219)     => version_sym(220).
	p(NT(219), (NT(220)));
//G355: cli_command(213)     => version_cmd(219).
	p(NT(213), (NT(219)));
//G356: __E_cli_command_104(224) => 'c'.
	p(NT(224), (T(8)));
//G357: __E_cli_command_104(224) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(224), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G358: clear_sym(223)       => __E_cli_command_104(224).
	p(NT(223), (NT(224)));
//G359: clear_cmd(222)       => clear_sym(223).
	p(NT(222), (NT(223)));
//G360: cli_command(213)     => clear_cmd(222).
	p(NT(213), (NT(222)));
//G361: __E___E_cli_command_105_106(228) => 'h'.
	p(NT(228), (T(52)));
//G362: __E___E_cli_command_105_106(228) => 'h' 'e' 'l' 'p'.
	p(NT(228), (T(52)+T(19)+T(6)+T(53)));
//G363: help_sym(227)        => __E___E_cli_command_105_106(228).
	p(NT(227), (NT(228)));
//G364: __E___E_cli_command_105_107(229) => __(27) help_arg(230).
	p(NT(229), (NT(27)+NT(230)));
//G365: __E___E_cli_command_105_107(229) => null.
	p(NT(229), (nul));
//G366: __E_cli_command_105(226) => help_sym(227) __E___E_cli_command_105_107(229).
	p(NT(226), (NT(227)+NT(229)));
//G367: help_cmd(225)        => __E_cli_command_105(226).
	p(NT(225), (NT(226)));
//G368: cli_command(213)     => help_cmd(225).
	p(NT(213), (NT(225)));
//G369: file_sym(233)        => 'f' 'i' 'l' 'e'.
	p(NT(233), (T(4)+T(10)+T(6)+T(19)));
//G370: __E_cli_command_108(232) => file_sym(233) __(27) q_string(234).
	p(NT(232), (NT(233)+NT(27)+NT(234)));
//G371: file_cmd(231)        => __E_cli_command_108(232).
	p(NT(231), (NT(232)));
//G372: cli_command(213)     => file_cmd(231).
	p(NT(213), (NT(231)));
//G373: valid_sym(237)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(237), (T(50)+T(5)+T(6)+T(10)+T(54)));
//G374: __E_cli_command_109(236) => valid_sym(237) __(27) normalize_cmd_arg(238).
	p(NT(236), (NT(237)+NT(27)+NT(238)));
//G375: valid_cmd(235)       => __E_cli_command_109(236).
	p(NT(235), (NT(236)));
//G376: cli_command(213)     => valid_cmd(235).
	p(NT(213), (NT(235)));
//G377: sat_sym(241)         => 's' 'a' 't'.
	p(NT(241), (T(12)+T(5)+T(13)));
//G378: __E_cli_command_110(240) => sat_sym(241) __(27) normalize_cmd_arg(238).
	p(NT(240), (NT(241)+NT(27)+NT(238)));
//G379: sat_cmd(239)         => __E_cli_command_110(240).
	p(NT(239), (NT(240)));
//G380: cli_command(213)     => sat_cmd(239).
	p(NT(213), (NT(239)));
//G381: unsat_sym(244)       => 'u' 'n' 's' 'a' 't'.
	p(NT(244), (T(49)+T(51)+T(12)+T(5)+T(13)));
//G382: __E_cli_command_111(243) => unsat_sym(244) __(27) normalize_cmd_arg(238).
	p(NT(243), (NT(244)+NT(27)+NT(238)));
//G383: unsat_cmd(242)       => __E_cli_command_111(243).
	p(NT(242), (NT(243)));
//G384: cli_command(213)     => unsat_cmd(242).
	p(NT(213), (NT(242)));
//G385: solve_sym(247)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(247), (T(12)+T(17)+T(6)+T(50)+T(19)));
//G386: __E___E_cli_command_112_113(248) => solve_options(249).
	p(NT(248), (NT(249)));
//G387: __E___E_cli_command_112_113(248) => null.
	p(NT(248), (nul));
//G388: __E_cli_command_112(246) => solve_sym(247) __E___E_cli_command_112_113(248) __(27) wff_cmd_arg(250).
	p(NT(246), (NT(247)+NT(248)+NT(27)+NT(250)));
//G389: solve_cmd(245)       => __E_cli_command_112(246).
	p(NT(245), (NT(246)));
//G390: cli_command(213)     => solve_cmd(245).
	p(NT(213), (NT(245)));
//G391: lgrs_sym(253)        => 'l' 'g' 'r' 's'.
	p(NT(253), (T(6)+T(55)+T(11)+T(12)));
//G392: __E_cli_command_114(252) => lgrs_sym(253) __(27) wff_cmd_arg(250).
	p(NT(252), (NT(253)+NT(27)+NT(250)));
//G393: lgrs_cmd(251)        => __E_cli_command_114(252).
	p(NT(251), (NT(252)));
//G394: cli_command(213)     => lgrs_cmd(251).
	p(NT(213), (NT(251)));
//G395: __E___E_cli_command_115_116(257) => 'r'.
	p(NT(257), (T(11)));
//G396: __E___E_cli_command_115_116(257) => 'r' 'u' 'n'.
	p(NT(257), (T(11)+T(49)+T(51)));
//G397: run_sym(256)         => __E___E_cli_command_115_116(257).
	p(NT(256), (NT(257)));
//G398: __E___E_cli_command_115_117(258) => memory(259).
	p(NT(258), (NT(259)));
//G399: __E___E_cli_command_115_117(258) => wff(14).
	p(NT(258), (NT(14)));
//G400: __E_cli_command_115(255) => run_sym(256) __(27) __E___E_cli_command_115_117(258).
	p(NT(255), (NT(256)+NT(27)+NT(258)));
//G401: run_cmd(254)         => __E_cli_command_115(255).
	p(NT(254), (NT(255)));
//G402: cli_command(213)     => run_cmd(254).
	p(NT(213), (NT(254)));
//G403: __E___E_cli_command_118_119(263) => 'n'.
	p(NT(263), (T(51)));
//G404: __E___E_cli_command_118_119(263) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(263), (T(51)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(56)+T(19)));
//G405: normalize_sym(262)   => __E___E_cli_command_118_119(263).
	p(NT(262), (NT(263)));
//G406: __E_cli_command_118(261) => normalize_sym(262) __(27) normalize_cmd_arg(238).
	p(NT(261), (NT(262)+NT(27)+NT(238)));
//G407: normalize_cmd(260)   => __E_cli_command_118(261).
	p(NT(260), (NT(261)));
//G408: cli_command(213)     => normalize_cmd(260).
	p(NT(213), (NT(260)));
//G409: __E___E_cli_command_120_121(267) => 's'.
	p(NT(267), (T(12)));
//G410: __E___E_cli_command_120_121(267) => 's' 'u' 'b' 's' 't'.
	p(NT(267), (T(12)+T(49)+T(7)+T(12)+T(13)));
//G411: __E___E_cli_command_120_121(267) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(267), (T(12)+T(49)+T(7)+T(12)+T(13)+T(10)+T(13)+T(49)+T(13)+T(19)));
//G412: subst_sym(266)       => __E___E_cli_command_120_121(267).
	p(NT(266), (NT(267)));
//G413: __E_cli_command_120(265) => subst_sym(266) __(27) nf_cmd_arg(268) _(11) '[' _(11) nf_cmd_arg(268) _(11) '/' _(11) nf_cmd_arg(268) _(11) ']'.
	p(NT(265), (NT(266)+NT(27)+NT(268)+NT(11)+T(24)+NT(11)+NT(268)+NT(11)+T(57)+NT(11)+NT(268)+NT(11)+T(25)));
//G414: subst_cmd(264)       => __E_cli_command_120(265).
	p(NT(264), (NT(265)));
//G415: cli_command(213)     => subst_cmd(264).
	p(NT(213), (NT(264)));
//G416: __E___E_cli_command_122_123(272) => 'i'.
	p(NT(272), (T(10)));
//G417: __E___E_cli_command_122_123(272) => 'i' 'n' 's' 't'.
	p(NT(272), (T(10)+T(51)+T(12)+T(13)));
//G418: __E___E_cli_command_122_123(272) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(272), (T(10)+T(51)+T(12)+T(13)+T(5)+T(51)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G419: inst_sym(271)        => __E___E_cli_command_122_123(272).
	p(NT(271), (NT(272)));
//G420: __E_cli_command_122(270) => inst_sym(271) __(27) inst_args(273).
	p(NT(270), (NT(271)+NT(27)+NT(273)));
//G421: inst_cmd(269)        => __E_cli_command_122(270).
	p(NT(269), (NT(270)));
//G422: cli_command(213)     => inst_cmd(269).
	p(NT(213), (NT(269)));
//G423: dnf_sym(276)         => 'd' 'n' 'f'.
	p(NT(276), (T(54)+T(51)+T(4)));
//G424: __E_cli_command_124(275) => dnf_sym(276) __(27) nf_cmd_arg(268).
	p(NT(275), (NT(276)+NT(27)+NT(268)));
//G425: dnf_cmd(274)         => __E_cli_command_124(275).
	p(NT(274), (NT(275)));
//G426: cli_command(213)     => dnf_cmd(274).
	p(NT(213), (NT(274)));
//G427: cnf_sym(279)         => 'c' 'n' 'f'.
	p(NT(279), (T(8)+T(51)+T(4)));
//G428: __E_cli_command_125(278) => cnf_sym(279) __(27) nf_cmd_arg(268).
	p(NT(278), (NT(279)+NT(27)+NT(268)));
//G429: cnf_cmd(277)         => __E_cli_command_125(278).
	p(NT(277), (NT(278)));
//G430: cli_command(213)     => cnf_cmd(277).
	p(NT(213), (NT(277)));
//G431: anf_sym(282)         => 'a' 'n' 'f'.
	p(NT(282), (T(5)+T(51)+T(4)));
//G432: __E_cli_command_126(281) => anf_sym(282) __(27) nf_cmd_arg(268).
	p(NT(281), (NT(282)+NT(27)+NT(268)));
//G433: anf_cmd(280)         => __E_cli_command_126(281).
	p(NT(280), (NT(281)));
//G434: cli_command(213)     => anf_cmd(280).
	p(NT(213), (NT(280)));
//G435: nnf_sym(285)         => 'n' 'n' 'f'.
	p(NT(285), (T(51)+T(51)+T(4)));
//G436: __E_cli_command_127(284) => nnf_sym(285) __(27) nf_cmd_arg(268).
	p(NT(284), (NT(285)+NT(27)+NT(268)));
//G437: nnf_cmd(283)         => __E_cli_command_127(284).
	p(NT(283), (NT(284)));
//G438: cli_command(213)     => nnf_cmd(283).
	p(NT(213), (NT(283)));
//G439: pnf_sym(288)         => 'p' 'n' 'f'.
	p(NT(288), (T(53)+T(51)+T(4)));
//G440: __E_cli_command_128(287) => pnf_sym(288) __(27) nf_cmd_arg(268).
	p(NT(287), (NT(288)+NT(27)+NT(268)));
//G441: pnf_cmd(286)         => __E_cli_command_128(287).
	p(NT(286), (NT(287)));
//G442: cli_command(213)     => pnf_cmd(286).
	p(NT(213), (NT(286)));
//G443: mnf_sym(291)         => 'm' 'n' 'f'.
	p(NT(291), (T(18)+T(51)+T(4)));
//G444: __E_cli_command_129(290) => mnf_sym(291) __(27) nf_cmd_arg(268).
	p(NT(290), (NT(291)+NT(27)+NT(268)));
//G445: mnf_cmd(289)         => __E_cli_command_129(290).
	p(NT(289), (NT(290)));
//G446: cli_command(213)     => mnf_cmd(289).
	p(NT(213), (NT(289)));
//G447: snf_sym(294)         => 's' 'n' 'f'.
	p(NT(294), (T(12)+T(51)+T(4)));
//G448: __E_cli_command_130(293) => snf_sym(294) __(27) nf_cmd_arg(268).
	p(NT(293), (NT(294)+NT(27)+NT(268)));
//G449: snf_cmd(292)         => __E_cli_command_130(293).
	p(NT(292), (NT(293)));
//G450: cli_command(213)     => snf_cmd(292).
	p(NT(213), (NT(292)));
//G451: onf_sym(297)         => 'o' 'n' 'f'.
	p(NT(297), (T(17)+T(51)+T(4)));
//G452: __E_cli_command_131(296) => onf_sym(297) __(27) variable(118) __(27) onf_cmd_arg(298).
	p(NT(296), (NT(297)+NT(27)+NT(118)+NT(27)+NT(298)));
//G453: onf_cmd(295)         => __E_cli_command_131(296).
	p(NT(295), (NT(296)));
//G454: cli_command(213)     => onf_cmd(295).
	p(NT(213), (NT(295)));
//G455: __E___E_cli_command_132_133(302) => 'd' 'e' 'f' 's'.
	p(NT(302), (T(54)+T(19)+T(4)+T(12)));
//G456: __E___E_cli_command_132_133(302) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(302), (T(54)+T(19)+T(4)+T(10)+T(51)+T(10)+T(13)+T(10)+T(17)+T(51)+T(12)));
//G457: def_sym(301)         => __E___E_cli_command_132_133(302).
	p(NT(301), (NT(302)));
//G458: __E_cli_command_132(300) => def_sym(301).
	p(NT(300), (NT(301)));
//G459: def_list_cmd(299)    => __E_cli_command_132(300).
	p(NT(299), (NT(300)));
//G460: cli_command(213)     => def_list_cmd(299).
	p(NT(213), (NT(299)));
//G461: __E_cli_command_134(304) => def_sym(301) __(27) number(305).
	p(NT(304), (NT(301)+NT(27)+NT(305)));
//G462: def_print_cmd(303)   => __E_cli_command_134(304).
	p(NT(303), (NT(304)));
//G463: cli_command(213)     => def_print_cmd(303).
	p(NT(213), (NT(303)));
//G464: def_rr_cmd(306)      => rec_relation(16).
	p(NT(306), (NT(16)));
//G465: cli_command(213)     => def_rr_cmd(306).
	p(NT(213), (NT(306)));
//G466: qelim_sym(309)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(309), (T(48)+T(19)+T(6)+T(10)+T(18)));
//G467: __E_cli_command_135(308) => qelim_sym(309) __(27) wff_cmd_arg(250).
	p(NT(308), (NT(309)+NT(27)+NT(250)));
//G468: qelim_cmd(307)       => __E_cli_command_135(308).
	p(NT(307), (NT(308)));
//G469: cli_command(213)     => qelim_cmd(307).
	p(NT(213), (NT(307)));
//G470: get_sym(312)         => 'g' 'e' 't'.
	p(NT(312), (T(55)+T(19)+T(13)));
//G471: __E___E_cli_command_136_137(313) => __(27) option_name(314).
	p(NT(313), (NT(27)+NT(314)));
//G472: __E___E_cli_command_136_137(313) => null.
	p(NT(313), (nul));
//G473: __E_cli_command_136(311) => get_sym(312) __E___E_cli_command_136_137(313).
	p(NT(311), (NT(312)+NT(313)));
//G474: get_cmd(310)         => __E_cli_command_136(311).
	p(NT(310), (NT(311)));
//G475: cli_command(213)     => get_cmd(310).
	p(NT(213), (NT(310)));
//G476: set_sym(317)         => 's' 'e' 't'.
	p(NT(317), (T(12)+T(19)+T(13)));
//G477: __E___E_cli_command_138_139(318) => __(27).
	p(NT(318), (NT(27)));
//G478: __E___E_cli_command_138_139(318) => _(11) '=' _(11).
	p(NT(318), (NT(11)+T(3)+NT(11)));
//G479: __E_cli_command_138(316) => set_sym(317) __(27) option_name(314) __E___E_cli_command_138_139(318) option_value(319).
	p(NT(316), (NT(317)+NT(27)+NT(314)+NT(318)+NT(319)));
//G480: set_cmd(315)         => __E_cli_command_138(316).
	p(NT(315), (NT(316)));
//G481: cli_command(213)     => set_cmd(315).
	p(NT(213), (NT(315)));
//G482: enable_sym(322)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(322), (T(19)+T(51)+T(5)+T(7)+T(6)+T(19)));
//G483: __E_cli_command_140(321) => enable_sym(322) __(27) option_name(314).
	p(NT(321), (NT(322)+NT(27)+NT(314)));
//G484: enable_cmd(320)      => __E_cli_command_140(321).
	p(NT(320), (NT(321)));
//G485: cli_command(213)     => enable_cmd(320).
	p(NT(213), (NT(320)));
//G486: disable_sym(325)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(325), (T(54)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G487: __E_cli_command_141(324) => disable_sym(325) __(27) option_name(314).
	p(NT(324), (NT(325)+NT(27)+NT(314)));
//G488: disable_cmd(323)     => __E_cli_command_141(324).
	p(NT(323), (NT(324)));
//G489: cli_command(213)     => disable_cmd(323).
	p(NT(213), (NT(323)));
//G490: toggle_sym(328)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(328), (T(13)+T(17)+T(55)+T(55)+T(6)+T(19)));
//G491: __E_cli_command_142(327) => toggle_sym(328) __(27) option_name(314).
	p(NT(327), (NT(328)+NT(27)+NT(314)));
//G492: toggle_cmd(326)      => __E_cli_command_142(327).
	p(NT(326), (NT(327)));
//G493: cli_command(213)     => toggle_cmd(326).
	p(NT(213), (NT(326)));
//G494: __E_cli_command_143(330) => type(129) __(27) in_var_name(170) _(11) '=' _(11) input_stream(331).
	p(NT(330), (NT(129)+NT(27)+NT(170)+NT(11)+T(3)+NT(11)+NT(331)));
//G495: def_input_cmd(329)   => __E_cli_command_143(330).
	p(NT(329), (NT(330)));
//G496: cli_command(213)     => def_input_cmd(329).
	p(NT(213), (NT(329)));
//G497: __E_cli_command_144(333) => type(129) __(27) out_var_name(174) _(11) '=' _(11) output_stream(334).
	p(NT(333), (NT(129)+NT(27)+NT(174)+NT(11)+T(3)+NT(11)+NT(334)));
//G498: def_output_cmd(332)  => __E_cli_command_144(333).
	p(NT(332), (NT(333)));
//G499: cli_command(213)     => def_output_cmd(332).
	p(NT(213), (NT(332)));
//G500: __E___E_cli_command_145_146(338) => 'h' 'i' 's' 't'.
	p(NT(338), (T(52)+T(10)+T(12)+T(13)));
//G501: __E___E_cli_command_145_146(338) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(338), (T(52)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G502: history_sym(337)     => __E___E_cli_command_145_146(338).
	p(NT(337), (NT(338)));
//G503: __E_cli_command_145(336) => history_sym(337).
	p(NT(336), (NT(337)));
//G504: history_list_cmd(335) => __E_cli_command_145(336).
	p(NT(335), (NT(336)));
//G505: cli_command(213)     => history_list_cmd(335).
	p(NT(213), (NT(335)));
//G506: __E_cli_command_147(340) => history_sym(337) __(27) memory(259).
	p(NT(340), (NT(337)+NT(27)+NT(259)));
//G507: history_print_cmd(339) => __E_cli_command_147(340).
	p(NT(339), (NT(340)));
//G508: cli_command(213)     => history_print_cmd(339).
	p(NT(213), (NT(339)));
//G509: __E_cli_command_148(342) => wff(14).
	p(NT(342), (NT(14)));
//G510: __E_cli_command_148(342) => bf(21).
	p(NT(342), (NT(21)));
//G511: history_store_cmd(341) => __E_cli_command_148(342).
	p(NT(341), (NT(342)));
//G512: cli_command(213)     => history_store_cmd(341).
	p(NT(213), (NT(341)));
//G513: __E_solve_options_149(343) => __(27) solve_option(344).
	p(NT(343), (NT(27)+NT(344)));
//G514: __E_solve_options_150(345) => null.
	p(NT(345), (nul));
//G515: __E_solve_options_150(345) => __E_solve_options_149(343) __E_solve_options_150(345).
	p(NT(345), (NT(343)+NT(345)));
//G516: solve_options(249)   => __E_solve_options_150(345).
	p(NT(249), (NT(345)));
//G517: __E_solve_option_151(346) => '-' '-' solver_mode(347).
	p(NT(346), (T(28)+T(28)+NT(347)));
//G518: solve_option(344)    => __E_solve_option_151(346).
	p(NT(344), (NT(346)));
//G519: __E_solve_option_152(348) => '-' '-' type(129).
	p(NT(348), (T(28)+T(28)+NT(129)));
//G520: solve_option(344)    => __E_solve_option_152(348).
	p(NT(344), (NT(348)));
//G521: __E_solver_mode_153(350) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(350), (T(18)+T(10)+T(51)+T(10)+T(18)+T(49)+T(18)));
//G522: __E_solver_mode_153(350) => 'm' 'i' 'n'.
	p(NT(350), (T(18)+T(10)+T(51)));
//G523: solver_mode_minimum(349) => __E_solver_mode_153(350).
	p(NT(349), (NT(350)));
//G524: solver_mode(347)     => solver_mode_minimum(349).
	p(NT(347), (NT(349)));
//G525: __E_solver_mode_154(352) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(352), (T(18)+T(5)+T(27)+T(10)+T(18)+T(49)+T(18)));
//G526: __E_solver_mode_154(352) => 'm' 'a' 'x'.
	p(NT(352), (T(18)+T(5)+T(27)));
//G527: solver_mode_maximum(351) => __E_solver_mode_154(352).
	p(NT(351), (NT(352)));
//G528: solver_mode(347)     => solver_mode_maximum(351).
	p(NT(347), (NT(351)));
//G529: __E_input_stream_155(353) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(354) _(11) ')'.
	p(NT(353), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(354)+NT(11)+T(16)));
//G530: input_stream(331)    => __E_input_stream_155(353).
	p(NT(331), (NT(353)));
//G531: console_sym(355)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(355), (T(8)+T(17)+T(51)+T(12)+T(17)+T(6)+T(19)));
//G532: input_stream(331)    => console_sym(355).
	p(NT(331), (NT(355)));
//G533: __E_output_stream_156(356) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(354) _(11) ')'.
	p(NT(356), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(354)+NT(11)+T(16)));
//G534: output_stream(334)   => __E_output_stream_156(356).
	p(NT(334), (NT(356)));
//G535: output_stream(334)   => console_sym(355).
	p(NT(334), (NT(355)));
//G536: q_file_name(354)     => '"' file_name(357) '"'.
	p(NT(354), (T(58)+NT(357)+T(58)));
//G537: __E_file_name_157(358) => printable(8).
	p(NT(358), (NT(8)));
//G538: __E_file_name_157(358) => printable(8) __E_file_name_157(358).
	p(NT(358), (NT(8)+NT(358)));
//G539: file_name(357)       => __E_file_name_157(358).
	p(NT(357), (NT(358)));
//G540: __E_option_name_158(359) => alnum(6).
	p(NT(359), (NT(6)));
//G541: __E_option_name_158(359) => alnum(6) __E_option_name_158(359).
	p(NT(359), (NT(6)+NT(359)));
//G542: option_name(314)     => __E_option_name_158(359).
	p(NT(314), (NT(359)));
//G543: __E_option_value_159(360) => alnum(6).
	p(NT(360), (NT(6)));
//G544: __E_option_value_159(360) => alnum(6) __E_option_value_159(360).
	p(NT(360), (NT(6)+NT(360)));
//G545: option_value(319)    => __E_option_value_159(360).
	p(NT(319), (NT(360)));
//G546: number(305)          => digits(187).
	p(NT(305), (NT(187)));
//G547: bf_cmd_arg(361)      => memory(259).
	p(NT(361), (NT(259)));
//G548: bf_cmd_arg(361)      => bf(21).
	p(NT(361), (NT(21)));
//G549: wff_cmd_arg(250)     => memory(259).
	p(NT(250), (NT(259)));
//G550: wff_cmd_arg(250)     => wff(14).
	p(NT(250), (NT(14)));
//G551: nf_cmd_arg(268)      => memory(259).
	p(NT(268), (NT(259)));
//G552: nf_cmd_arg(268)      => ref(18).
	p(NT(268), (NT(18)));
//G553: nf_cmd_arg(268)      => wff(14).
	p(NT(268), (NT(14)));
//G554: nf_cmd_arg(268)      => bf(21).
	p(NT(268), (NT(21)));
//G555: onf_cmd_arg(298)     => memory(259).
	p(NT(298), (NT(259)));
//G556: onf_cmd_arg(298)     => wff(14).
	p(NT(298), (NT(14)));
//G557: normalize_cmd_arg(238) => memory(259).
	p(NT(238), (NT(259)));
//G558: normalize_cmd_arg(238) => rr(10).
	p(NT(238), (NT(10)));
//G559: normalize_cmd_arg(238) => ref(18).
	p(NT(238), (NT(18)));
//G560: normalize_cmd_arg(238) => wff(14).
	p(NT(238), (NT(14)));
//G561: normalize_cmd_arg(238) => bf(21).
	p(NT(238), (NT(21)));
//G562: inst_args(273)       => wff_cmd_arg(250) _(11) '[' _(11) variable(118) _(11) '/' _(11) bf_cmd_arg(361) _(11) ']'.
	p(NT(273), (NT(250)+NT(11)+T(24)+NT(11)+NT(118)+NT(11)+T(57)+NT(11)+NT(361)+NT(11)+T(25)));
//G563: inst_args(273)       => bf_cmd_arg(361) _(11) '[' _(11) variable(118) _(11) '/' _(11) bf_cmd_arg(361) _(11) ']'.
	p(NT(273), (NT(361)+NT(11)+T(24)+NT(11)+NT(118)+NT(11)+T(57)+NT(11)+NT(361)+NT(11)+T(25)));
//G564: help_arg(230)        => help_sym(227).
	p(NT(230), (NT(227)));
//G565: help_arg(230)        => version_sym(220).
	p(NT(230), (NT(220)));
//G566: help_arg(230)        => quit_sym(217).
	p(NT(230), (NT(217)));
//G567: help_arg(230)        => clear_sym(223).
	p(NT(230), (NT(223)));
//G568: help_arg(230)        => get_sym(312).
	p(NT(230), (NT(312)));
//G569: help_arg(230)        => set_sym(317).
	p(NT(230), (NT(317)));
//G570: help_arg(230)        => enable_sym(322).
	p(NT(230), (NT(322)));
//G571: help_arg(230)        => disable_sym(325).
	p(NT(230), (NT(325)));
//G572: help_arg(230)        => toggle_sym(328).
	p(NT(230), (NT(328)));
//G573: help_arg(230)        => file_sym(233).
	p(NT(230), (NT(233)));
//G574: help_arg(230)        => history_sym(337).
	p(NT(230), (NT(337)));
//G575: help_arg(230)        => abs_memory_sym(362).
	p(NT(230), (NT(362)));
//G576: help_arg(230)        => rel_memory_sym(363).
	p(NT(230), (NT(363)));
//G577: help_arg(230)        => selection_sym(364).
	p(NT(230), (NT(364)));
//G578: help_arg(230)        => def_sym(301).
	p(NT(230), (NT(301)));
//G579: help_arg(230)        => inst_sym(271).
	p(NT(230), (NT(271)));
//G580: help_arg(230)        => subst_sym(266).
	p(NT(230), (NT(266)));
//G581: help_arg(230)        => normalize_sym(262).
	p(NT(230), (NT(262)));
//G582: help_arg(230)        => execute_sym(365).
	p(NT(230), (NT(365)));
//G583: help_arg(230)        => solve_sym(247).
	p(NT(230), (NT(247)));
//G584: help_arg(230)        => valid_sym(237).
	p(NT(230), (NT(237)));
//G585: help_arg(230)        => sat_sym(241).
	p(NT(230), (NT(241)));
//G586: help_arg(230)        => unsat_sym(244).
	p(NT(230), (NT(244)));
//G587: help_arg(230)        => run_sym(256).
	p(NT(230), (NT(256)));
//G588: help_arg(230)        => dnf_sym(276).
	p(NT(230), (NT(276)));
//G589: help_arg(230)        => cnf_sym(279).
	p(NT(230), (NT(279)));
//G590: help_arg(230)        => anf_sym(282).
	p(NT(230), (NT(282)));
//G591: help_arg(230)        => snf_sym(294).
	p(NT(230), (NT(294)));
//G592: help_arg(230)        => nnf_sym(285).
	p(NT(230), (NT(285)));
//G593: help_arg(230)        => pnf_sym(288).
	p(NT(230), (NT(288)));
//G594: help_arg(230)        => mnf_sym(291).
	p(NT(230), (NT(291)));
//G595: help_arg(230)        => onf_sym(297).
	p(NT(230), (NT(297)));
//G596: help_arg(230)        => qelim_sym(309).
	p(NT(230), (NT(309)));
//G597: __E___E_help_arg_160_161(368) => 's'.
	p(NT(368), (T(12)));
//G598: __E___E_help_arg_160_161(368) => null.
	p(NT(368), (nul));
//G599: __E_help_arg_160(367) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_160_161(368).
	p(NT(367), (T(19)+T(27)+T(5)+T(18)+T(53)+T(6)+T(19)+NT(368)));
//G600: examples_sym(366)    => __E_help_arg_160(367).
	p(NT(366), (NT(367)));
//G601: help_arg(230)        => examples_sym(366).
	p(NT(230), (NT(366)));
//G602: __E_memory_162(370)  => '%'.
	p(NT(370), (T(59)));
//G603: rel_memory_sym(363)  => '%' '-'.
	p(NT(363), (T(59)+T(28)));
//G604: memory_id(372)       => digits(187).
	p(NT(372), (NT(187)));
//G605: __E___E_memory_162_163(371) => memory_id(372).
	p(NT(371), (NT(372)));
//G606: __E___E_memory_162_163(371) => null.
	p(NT(371), (nul));
//G607: __E_memory_162(370)  => rel_memory_sym(363) __E___E_memory_162_163(371).
	p(NT(370), (NT(363)+NT(371)));
//G608: rel_memory(369)      => __E_memory_162(370).
	p(NT(369), (NT(370)));
//G609: memory(259)          => rel_memory(369).
	p(NT(259), (NT(369)));
//G610: abs_memory_sym(362)  => '%'.
	p(NT(362), (T(59)));
//G611: __E_memory_164(374)  => abs_memory_sym(362) memory_id(372).
	p(NT(374), (NT(362)+NT(372)));
//G612: abs_memory(373)      => __E_memory_164(374).
	p(NT(373), (NT(374)));
//G613: memory(259)          => abs_memory(373).
	p(NT(259), (NT(373)));
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
		rule, __E___E___E_library_9_10_11, wff_rule, bf_rule, __E___E_library_9_12, wff_matcher, wff_body, bf_matcher, bf_body, builder, 
		builder_head, builder_body, __E_builder_head_13, __E_builder_head_14, bf_builder_body, __E_builder_body_15, wff_builder_body, __E_builder_body_16, tau_constant_source, __E_tau_constant_source_17, 
		wff_parenthesis, __E_wff_18, wff_sometimes, __E_wff_19, __E___E_wff_19_20, wff_always, __E_wff_21, __E___E_wff_21_22, wff_conditional, __E_wff_23, 
		wff_all, __E_wff_24, q_vars, wff_ex, __E_wff_25, wff_ref, constraint, wff_imply, __E_wff_26, wff_rimply, 
		__E_wff_27, wff_equiv, __E_wff_28, wff_or, __E_wff_29, wff_xor, __E_wff_30, wff_and, __E_wff_31, wff_neg, 
		__E_wff_32, wff_t, wff_f, bf_interval, __E_wff_33, bf_eq, __E_wff_34, bf_neq, __E_wff_35, bf_less_equal, 
		__E_wff_36, bf_nleq, __E_wff_37, bf_greater, __E_wff_38, bf_ngreater, __E_wff_39, bf_greater_equal, __E_wff_40, bf_ngeq, 
		__E_wff_41, bf_less, __E_wff_42, bf_nless, __E_wff_43, bf_parenthesis, __E_bf_44, bf_constant, variable, bf_splitter, 
		__E_bf_45, bf_ref, bf_or, __E_bf_46, bf_xor, __E_bf_47, bf_t, __E_bf_48, __E___E_bf_48_49, type, 
		bf_f, __E_bf_50, __E___E_bf_50_51, bf_and, __E_bf_52, __E___E_bf_52_53, bf_neg, __E_bf_54, bf_neg_oprnd, __E___E_bf_54_55, 
		bf_and_nosep, __E_bf_56, bf_and_nosep_1st_oprnd, __E___E_bf_56_57, bf_and_nosep_2nd_oprnd, __E___E_bf_56_58, ctn_neq, __E_constraint_59, ctnvar, num, 
		ctn_eq, __E_constraint_60, ctn_greater_equal, __E_constraint_61, ctn_greater, __E_constraint_62, ctn_less_equal, __E_constraint_63, ctn_less, __E_constraint_64, 
		__E_variable_65, __E_variable_66, __E_variable_67, __E_variable_68, __E_variable_69, io_var, __E_variable_70, in, out, uninterpreted_constant, 
		in_var_name, __E_in_71, __E___E_in_71_72, offset, out_var_name, __E_out_73, __E___E_out_73_74, chars, q_var, __E_q_vars_75, 
		__E_q_vars_76, __E_offsets_77, __E_offsets_78, shift, __E_offset_79, __E_shift_80, __E___E_shift_80_81, digits, uninter_const_name, __E_uninterpreted_constant_82, 
		__E___E_uninterpreted_constant_82_83, constant, __E_bf_constant_84, binding, __E_bf_constant_85, source, __E_binding_86, src_c, __E___E_binding_86_87, __E___E___E_binding_86_87_88, 
		__E___E___E_binding_86_87_89, __E_src_c_90, __E_src_c_91, __E_chars_92, __E_chars_93, __E_digits_94, comment, __E_comment_95, __E_comment_96, __E_comment_97, 
		__E____98, __E___99, cli, cli_command, __E_cli_100, __E_cli_101, quit_cmd, quit_sym, __E_cli_command_102, version_cmd, 
		version_sym, __E_cli_command_103, clear_cmd, clear_sym, __E_cli_command_104, help_cmd, __E_cli_command_105, help_sym, __E___E_cli_command_105_106, __E___E_cli_command_105_107, 
		help_arg, file_cmd, __E_cli_command_108, file_sym, q_string, valid_cmd, __E_cli_command_109, valid_sym, normalize_cmd_arg, sat_cmd, 
		__E_cli_command_110, sat_sym, unsat_cmd, __E_cli_command_111, unsat_sym, solve_cmd, __E_cli_command_112, solve_sym, __E___E_cli_command_112_113, solve_options, 
		wff_cmd_arg, lgrs_cmd, __E_cli_command_114, lgrs_sym, run_cmd, __E_cli_command_115, run_sym, __E___E_cli_command_115_116, __E___E_cli_command_115_117, memory, 
		normalize_cmd, __E_cli_command_118, normalize_sym, __E___E_cli_command_118_119, subst_cmd, __E_cli_command_120, subst_sym, __E___E_cli_command_120_121, nf_cmd_arg, inst_cmd, 
		__E_cli_command_122, inst_sym, __E___E_cli_command_122_123, inst_args, dnf_cmd, __E_cli_command_124, dnf_sym, cnf_cmd, __E_cli_command_125, cnf_sym, 
		anf_cmd, __E_cli_command_126, anf_sym, nnf_cmd, __E_cli_command_127, nnf_sym, pnf_cmd, __E_cli_command_128, pnf_sym, mnf_cmd, 
		__E_cli_command_129, mnf_sym, snf_cmd, __E_cli_command_130, snf_sym, onf_cmd, __E_cli_command_131, onf_sym, onf_cmd_arg, def_list_cmd, 
		__E_cli_command_132, def_sym, __E___E_cli_command_132_133, def_print_cmd, __E_cli_command_134, number, def_rr_cmd, qelim_cmd, __E_cli_command_135, qelim_sym, 
		get_cmd, __E_cli_command_136, get_sym, __E___E_cli_command_136_137, option_name, set_cmd, __E_cli_command_138, set_sym, __E___E_cli_command_138_139, option_value, 
		enable_cmd, __E_cli_command_140, enable_sym, disable_cmd, __E_cli_command_141, disable_sym, toggle_cmd, __E_cli_command_142, toggle_sym, def_input_cmd, 
		__E_cli_command_143, input_stream, def_output_cmd, __E_cli_command_144, output_stream, history_list_cmd, __E_cli_command_145, history_sym, __E___E_cli_command_145_146, history_print_cmd, 
		__E_cli_command_147, history_store_cmd, __E_cli_command_148, __E_solve_options_149, solve_option, __E_solve_options_150, __E_solve_option_151, solver_mode, __E_solve_option_152, solver_mode_minimum, 
		__E_solver_mode_153, solver_mode_maximum, __E_solver_mode_154, __E_input_stream_155, q_file_name, console_sym, __E_output_stream_156, file_name, __E_file_name_157, __E_option_name_158, 
		__E_option_value_159, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, execute_sym, examples_sym, __E_help_arg_160, __E___E_help_arg_160_161, rel_memory, 
		__E_memory_162, __E___E_memory_162_163, memory_id, abs_memory, __E_memory_164, named_binding, __N_0, __N_1, __N_2, __N_3, 
		__N_4, __N_5, __N_6, __N_7, __N_8, 
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
