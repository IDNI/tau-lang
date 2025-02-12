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
	"in_var_name", "__E_in_71", "__E___E_in_71_72", "offset", "out_var_name", "__E_out_73", "__E___E_out_73_74", "__E___E___E_out_73_74_75", "chars", "q_var", 
	"__E_q_vars_76", "__E_q_vars_77", "__E_offsets_78", "__E_offsets_79", "integer", "shift", "__E_offset_80", "__E_shift_81", "__E___E_shift_81_82", "digits", 
	"__E_integer_83", "uninter_const_name", "__E_uninterpreted_constant_84", "__E___E_uninterpreted_constant_84_85", "constant", "__E_bf_constant_86", "binding", "__E_bf_constant_87", "source", "__E_binding_88", 
	"src_c", "__E___E_binding_88_89", "__E___E___E_binding_88_89_90", "__E___E___E_binding_88_89_91", "__E_src_c_92", "__E_src_c_93", "__E_chars_94", "__E_chars_95", "__E_digits_96", "comment", 
	"__E_comment_97", "__E_comment_98", "__E_comment_99", "__E____100", "__E___101", "cli", "cli_command", "__E_cli_102", "__E_cli_103", "quit_cmd", 
	"quit_sym", "__E_cli_command_104", "version_cmd", "version_sym", "__E_cli_command_105", "clear_cmd", "clear_sym", "__E_cli_command_106", "help_cmd", "__E_cli_command_107", 
	"help_sym", "__E___E_cli_command_107_108", "__E___E_cli_command_107_109", "help_arg", "file_cmd", "__E_cli_command_110", "file_sym", "q_string", "valid_cmd", "__E_cli_command_111", 
	"valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_112", "sat_sym", "unsat_cmd", "__E_cli_command_113", "unsat_sym", "solve_cmd", "__E_cli_command_114", 
	"solve_sym", "__E___E_cli_command_114_115", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_116", "lgrs_sym", "run_cmd", "__E_cli_command_117", "run_sym", 
	"__E___E_cli_command_117_118", "__E___E_cli_command_117_119", "memory", "normalize_cmd", "__E_cli_command_120", "normalize_sym", "__E___E_cli_command_120_121", "subst_cmd", "__E_cli_command_122", "subst_sym", 
	"__E___E_cli_command_122_123", "nf_cmd_arg", "inst_cmd", "__E_cli_command_124", "inst_sym", "__E___E_cli_command_124_125", "inst_args", "dnf_cmd", "__E_cli_command_126", "dnf_sym", 
	"cnf_cmd", "__E_cli_command_127", "cnf_sym", "anf_cmd", "__E_cli_command_128", "anf_sym", "nnf_cmd", "__E_cli_command_129", "nnf_sym", "pnf_cmd", 
	"__E_cli_command_130", "pnf_sym", "mnf_cmd", "__E_cli_command_131", "mnf_sym", "snf_cmd", "__E_cli_command_132", "snf_sym", "onf_cmd", "__E_cli_command_133", 
	"onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_134", "def_sym", "__E___E_cli_command_134_135", "def_print_cmd", "__E_cli_command_136", "number", "def_rr_cmd", 
	"qelim_cmd", "__E_cli_command_137", "qelim_sym", "get_cmd", "__E_cli_command_138", "get_sym", "__E___E_cli_command_138_139", "option_name", "set_cmd", "__E_cli_command_140", 
	"set_sym", "__E___E_cli_command_140_141", "option_value", "enable_cmd", "__E_cli_command_142", "enable_sym", "disable_cmd", "__E_cli_command_143", "disable_sym", "toggle_cmd", 
	"__E_cli_command_144", "toggle_sym", "def_input_cmd", "__E_cli_command_145", "input_stream", "def_output_cmd", "__E_cli_command_146", "output_stream", "history_list_cmd", "__E_cli_command_147", 
	"history_sym", "__E___E_cli_command_147_148", "history_print_cmd", "__E_cli_command_149", "history_store_cmd", "__E_cli_command_150", "__E_solve_options_151", "solve_option", "__E_solve_options_152", "__E_solve_option_153", 
	"solver_mode", "__E_solve_option_154", "solver_mode_minimum", "__E_solver_mode_155", "solver_mode_maximum", "__E_solver_mode_156", "__E_input_stream_157", "q_file_name", "console_sym", "__E_output_stream_158", 
	"file_name", "__E_file_name_159", "__E_option_name_160", "__E_option_value_161", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "execute_sym", "examples_sym", 
	"__E_help_arg_162", "__E___E_help_arg_162_163", "rel_memory", "__E_memory_164", "__E___E_memory_164_165", "memory_id", "abs_memory", "__E_memory_166", "named_binding", "__N_0", 
	"__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'f', 'a', 'l', 'b', 'c', 
	'k', 'i', 'r', 's', 't', ',', '(', ')', 'o', 'm', 
	'e', '<', '>', 'w', 'y', '[', ']', '?', 'x', '-', 
	'|', '^', '&', '!', 'T', 'F', 'S', '+', '1', '0', 
	'\'', '_', 'u', '$', '{', '}', '\t', '\n', '\r', '#', 
	'q', 'v', 'n', 'h', 'p', 'd', 'g', 'z', '/', '"', 
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
			20, 22, 118, 129, 148, 170, 174, 184, 189, 191,
			198, 317, 322, 360, 378
		},
		.to_inline = {
			{ 14, 60, 14 },
			{ 21, 115, 21 },
			{ 138, 115, 21 },
			{ 140, 115, 133 },
			{ 142, 115, 21 },
			{ 144, 115, 21 },
			{ 178 },
			{ 179 },
			{ 200 },
			{ 241 },
			{ 253 },
			{ 271 },
			{ 276 },
			{ 301 },
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
//G234: __N_0(379)           => 'F'.
	p(NT(379), (T(34)));
//G235: __N_1(380)           => 'T'.
	p(NT(380), (T(33)));
//G236: __E_variable_65(160) => ~( __N_0(379) ) & ~( __N_1(380) ) & alpha(5).	 # conjunctive
	p(NT(160), ~(NT(379)) & ~(NT(380)) & (NT(5)));
//G237: __E_variable_66(161) => null.
	p(NT(161), (nul));
//G238: __E_variable_66(161) => digit(3) __E_variable_66(161).
	p(NT(161), (NT(3)+NT(161)));
//G239: variable(118)        => __E_variable_65(160) __E_variable_66(161).	 # guarded: charvar
	p(NT(118), (NT(160)+NT(161)));
	p.back().guard = "charvar";
//G240: __N_2(381)           => 'F'.
	p(NT(381), (T(34)));
//G241: __N_3(382)           => 'T'.
	p(NT(382), (T(33)));
//G242: __E_variable_67(162) => ~( __N_2(381) ) & ~( __N_3(382) ) & alpha(5).	 # conjunctive
	p(NT(162), ~(NT(381)) & ~(NT(382)) & (NT(5)));
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
//G258: __E___E___E_out_73_74_75(177) => digit(3).
	p(NT(177), (NT(3)));
//G259: __E___E___E_out_73_74_75(177) => digit(3) __E___E___E_out_73_74_75(177).
	p(NT(177), (NT(3)+NT(177)));
//G260: __E___E_out_73_74(176) => 'o' __E___E___E_out_73_74_75(177).
	p(NT(176), (T(17)+NT(177)));
//G261: __E_out_73(175)      => __E___E_out_73_74(176).
	p(NT(175), (NT(176)));
//G262: __E_out_73(175)      => 'u'.
	p(NT(175), (T(41)));
//G263: out_var_name(174)    => __E_out_73(175).
	p(NT(174), (NT(175)));
//G264: out(168)             => out_var_name(174) '[' offset(173) ']'.
	p(NT(168), (NT(174)+T(24)+NT(173)+T(25)));
//G265: ctnvar(148)          => chars(178).
	p(NT(148), (NT(178)));
//G266: capture(20)          => '$' chars(178).
	p(NT(20), (T(42)+NT(178)));
//G267: __E_q_vars_76(180)   => _(11) ',' _(11) q_var(179).
	p(NT(180), (NT(11)+T(14)+NT(11)+NT(179)));
//G268: __E_q_vars_77(181)   => null.
	p(NT(181), (nul));
//G269: __E_q_vars_77(181)   => __E_q_vars_76(180) __E_q_vars_77(181).
	p(NT(181), (NT(180)+NT(181)));
//G270: q_vars(72)           => q_var(179) __E_q_vars_77(181).
	p(NT(72), (NT(179)+NT(181)));
//G271: q_var(179)           => capture(20).
	p(NT(179), (NT(20)));
//G272: __N_4(383)           => uninterpreted_constant(169).
	p(NT(383), (NT(169)));
//G273: q_var(179)           => variable(118) & ~( __N_4(383) ).	 # conjunctive
	p(NT(179), (NT(118)) & ~(NT(383)));
//G274: __E_offsets_78(182)  => _(11) ',' _(11) offset(173).
	p(NT(182), (NT(11)+T(14)+NT(11)+NT(173)));
//G275: __E_offsets_79(183)  => null.
	p(NT(183), (nul));
//G276: __E_offsets_79(183)  => __E_offsets_78(182) __E_offsets_79(183).
	p(NT(183), (NT(182)+NT(183)));
//G277: offsets(24)          => '[' _(11) offset(173) __E_offsets_79(183) _(11) ']'.
	p(NT(24), (T(24)+NT(11)+NT(173)+NT(183)+NT(11)+T(25)));
//G278: offset(173)          => integer(184).
	p(NT(173), (NT(184)));
//G279: offset(173)          => capture(20).
	p(NT(173), (NT(20)));
//G280: offset(173)          => shift(185).
	p(NT(173), (NT(185)));
//G281: __N_5(384)           => io_var(165).
	p(NT(384), (NT(165)));
//G282: __E_offset_80(186)   => variable(118) & ~( __N_5(384) ).	 # conjunctive
	p(NT(186), (NT(118)) & ~(NT(384)));
//G283: offset(173)          => __E_offset_80(186).
	p(NT(173), (NT(186)));
//G284: __E_shift_81(187)    => capture(20).
	p(NT(187), (NT(20)));
//G285: __N_6(385)           => io_var(165).
	p(NT(385), (NT(165)));
//G286: __E___E_shift_81_82(188) => variable(118) & ~( __N_6(385) ).	 # conjunctive
	p(NT(188), (NT(118)) & ~(NT(385)));
//G287: __E_shift_81(187)    => __E___E_shift_81_82(188).
	p(NT(187), (NT(188)));
//G288: shift(185)           => __E_shift_81(187) _(11) '-' _(11) num(149).
	p(NT(185), (NT(187)+NT(11)+T(28)+NT(11)+NT(149)));
//G289: num(149)             => digits(189).
	p(NT(149), (NT(189)));
//G290: __E_integer_83(190)  => '-'.
	p(NT(190), (T(28)));
//G291: __E_integer_83(190)  => null.
	p(NT(190), (nul));
//G292: integer(184)         => __E_integer_83(190) _(11) digits(189).
	p(NT(184), (NT(190)+NT(11)+NT(189)));
//G293: __E___E_uninterpreted_constant_84_85(193) => chars(178).
	p(NT(193), (NT(178)));
//G294: __E___E_uninterpreted_constant_84_85(193) => _(11).
	p(NT(193), (NT(11)));
//G295: __E_uninterpreted_constant_84(192) => __E___E_uninterpreted_constant_84_85(193) ':' chars(178).
	p(NT(192), (NT(193)+T(2)+NT(178)));
//G296: uninter_const_name(191) => __E_uninterpreted_constant_84(192).
	p(NT(191), (NT(192)));
//G297: uninterpreted_constant(169) => '<' uninter_const_name(191) _(11) '>'.
	p(NT(169), (T(20)+NT(191)+NT(11)+T(21)));
//G298: __E_bf_constant_86(195) => capture(20).
	p(NT(195), (NT(20)));
//G299: __E_bf_constant_86(195) => binding(196).
	p(NT(195), (NT(196)));
//G300: constant(194)        => __E_bf_constant_86(195).
	p(NT(194), (NT(195)));
//G301: type(129)            => chars(178).
	p(NT(129), (NT(178)));
//G302: __E_bf_constant_87(197) => _(11) ':' _(11) type(129).
	p(NT(197), (NT(11)+T(2)+NT(11)+NT(129)));
//G303: __E_bf_constant_87(197) => null.
	p(NT(197), (nul));
//G304: bf_constant(117)     => '{' _(11) constant(194) _(11) '}' __E_bf_constant_87(197).
	p(NT(117), (T(43)+NT(11)+NT(194)+NT(11)+T(44)+NT(197)));
//G305: __E___E___E_binding_88_89_90(202) => src_c(200).
	p(NT(202), (NT(200)));
//G306: __E___E___E_binding_88_89_90(202) => space(2).
	p(NT(202), (NT(2)));
//G307: __E___E___E_binding_88_89_91(203) => null.
	p(NT(203), (nul));
//G308: __E___E___E_binding_88_89_91(203) => __E___E___E_binding_88_89_90(202) __E___E___E_binding_88_89_91(203).
	p(NT(203), (NT(202)+NT(203)));
//G309: __E___E_binding_88_89(201) => __E___E___E_binding_88_89_91(203) src_c(200).
	p(NT(201), (NT(203)+NT(200)));
//G310: __E___E_binding_88_89(201) => null.
	p(NT(201), (nul));
//G311: __E_binding_88(199)  => src_c(200) __E___E_binding_88_89(201).
	p(NT(199), (NT(200)+NT(201)));
//G312: source(198)          => __E_binding_88(199).
	p(NT(198), (NT(199)));
//G313: binding(196)         => source(198).
	p(NT(196), (NT(198)));
//G314: src_c(200)           => alnum(6).
	p(NT(200), (NT(6)));
//G315: __N_7(386)           => '{'.
	p(NT(386), (T(43)));
//G316: __N_8(387)           => '}'.
	p(NT(387), (T(44)));
//G317: src_c(200)           => ~( __N_7(386) ) & ~( __N_8(387) ) & punct(7).	 # conjunctive
	p(NT(200), ~(NT(386)) & ~(NT(387)) & (NT(7)));
//G318: __E_src_c_92(204)    => src_c(200).
	p(NT(204), (NT(200)));
//G319: __E_src_c_92(204)    => space(2).
	p(NT(204), (NT(2)));
//G320: __E_src_c_93(205)    => null.
	p(NT(205), (nul));
//G321: __E_src_c_93(205)    => __E_src_c_92(204) __E_src_c_93(205).
	p(NT(205), (NT(204)+NT(205)));
//G322: src_c(200)           => '{' __E_src_c_93(205) '}'.
	p(NT(200), (T(43)+NT(205)+T(44)));
//G323: __E_chars_94(206)    => alnum(6).
	p(NT(206), (NT(6)));
//G324: __E_chars_94(206)    => '_'.
	p(NT(206), (T(40)));
//G325: __E_chars_95(207)    => null.
	p(NT(207), (nul));
//G326: __E_chars_95(207)    => __E_chars_94(206) __E_chars_95(207).
	p(NT(207), (NT(206)+NT(207)));
//G327: chars(178)           => alpha(5) __E_chars_95(207).
	p(NT(178), (NT(5)+NT(207)));
//G328: __E_digits_96(208)   => digit(3).
	p(NT(208), (NT(3)));
//G329: __E_digits_96(208)   => digit(3) __E_digits_96(208).
	p(NT(208), (NT(3)+NT(208)));
//G330: digits(189)          => __E_digits_96(208).
	p(NT(189), (NT(208)));
//G331: sym(22)              => chars(178).
	p(NT(22), (NT(178)));
//G332: __E_comment_97(210)  => printable(8).
	p(NT(210), (NT(8)));
//G333: __E_comment_97(210)  => '\t'.
	p(NT(210), (T(45)));
//G334: __E_comment_98(211)  => null.
	p(NT(211), (nul));
//G335: __E_comment_98(211)  => __E_comment_97(210) __E_comment_98(211).
	p(NT(211), (NT(210)+NT(211)));
//G336: __E_comment_99(212)  => '\n'.
	p(NT(212), (T(46)));
//G337: __E_comment_99(212)  => '\r'.
	p(NT(212), (T(47)));
//G338: __E_comment_99(212)  => eof(1).
	p(NT(212), (NT(1)));
//G339: comment(209)         => '#' __E_comment_98(211) __E_comment_99(212).
	p(NT(209), (T(48)+NT(211)+NT(212)));
//G340: __E____100(213)      => space(2).
	p(NT(213), (NT(2)));
//G341: __E____100(213)      => comment(209).
	p(NT(213), (NT(209)));
//G342: __(27)               => __E____100(213) _(11).
	p(NT(27), (NT(213)+NT(11)));
//G343: __E___101(214)       => __(27).
	p(NT(214), (NT(27)));
//G344: __E___101(214)       => null.
	p(NT(214), (nul));
//G345: _(11)                => __E___101(214).
	p(NT(11), (NT(214)));
//G346: cli(215)             => _(11).
	p(NT(215), (NT(11)));
//G347: __E_cli_102(217)     => '.' _(11) cli_command(216) _(11).
	p(NT(217), (T(1)+NT(11)+NT(216)+NT(11)));
//G348: __E_cli_103(218)     => null.
	p(NT(218), (nul));
//G349: __E_cli_103(218)     => __E_cli_102(217) __E_cli_103(218).
	p(NT(218), (NT(217)+NT(218)));
//G350: cli(215)             => _(11) cli_command(216) _(11) __E_cli_103(218).
	p(NT(215), (NT(11)+NT(216)+NT(11)+NT(218)));
//G351: __E_cli_command_104(221) => 'q'.
	p(NT(221), (T(49)));
//G352: __E_cli_command_104(221) => 'q' 'u' 'i' 't'.
	p(NT(221), (T(49)+T(41)+T(10)+T(13)));
//G353: quit_sym(220)        => __E_cli_command_104(221).
	p(NT(220), (NT(221)));
//G354: quit_cmd(219)        => quit_sym(220).
	p(NT(219), (NT(220)));
//G355: cli_command(216)     => quit_cmd(219).
	p(NT(216), (NT(219)));
//G356: __E_cli_command_105(224) => 'v'.
	p(NT(224), (T(50)));
//G357: __E_cli_command_105(224) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(224), (T(50)+T(19)+T(11)+T(12)+T(10)+T(17)+T(51)));
//G358: version_sym(223)     => __E_cli_command_105(224).
	p(NT(223), (NT(224)));
//G359: version_cmd(222)     => version_sym(223).
	p(NT(222), (NT(223)));
//G360: cli_command(216)     => version_cmd(222).
	p(NT(216), (NT(222)));
//G361: __E_cli_command_106(227) => 'c'.
	p(NT(227), (T(8)));
//G362: __E_cli_command_106(227) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(227), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G363: clear_sym(226)       => __E_cli_command_106(227).
	p(NT(226), (NT(227)));
//G364: clear_cmd(225)       => clear_sym(226).
	p(NT(225), (NT(226)));
//G365: cli_command(216)     => clear_cmd(225).
	p(NT(216), (NT(225)));
//G366: __E___E_cli_command_107_108(231) => 'h'.
	p(NT(231), (T(52)));
//G367: __E___E_cli_command_107_108(231) => 'h' 'e' 'l' 'p'.
	p(NT(231), (T(52)+T(19)+T(6)+T(53)));
//G368: help_sym(230)        => __E___E_cli_command_107_108(231).
	p(NT(230), (NT(231)));
//G369: __E___E_cli_command_107_109(232) => __(27) help_arg(233).
	p(NT(232), (NT(27)+NT(233)));
//G370: __E___E_cli_command_107_109(232) => null.
	p(NT(232), (nul));
//G371: __E_cli_command_107(229) => help_sym(230) __E___E_cli_command_107_109(232).
	p(NT(229), (NT(230)+NT(232)));
//G372: help_cmd(228)        => __E_cli_command_107(229).
	p(NT(228), (NT(229)));
//G373: cli_command(216)     => help_cmd(228).
	p(NT(216), (NT(228)));
//G374: file_sym(236)        => 'f' 'i' 'l' 'e'.
	p(NT(236), (T(4)+T(10)+T(6)+T(19)));
//G375: __E_cli_command_110(235) => file_sym(236) __(27) q_string(237).
	p(NT(235), (NT(236)+NT(27)+NT(237)));
//G376: file_cmd(234)        => __E_cli_command_110(235).
	p(NT(234), (NT(235)));
//G377: cli_command(216)     => file_cmd(234).
	p(NT(216), (NT(234)));
//G378: valid_sym(240)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(240), (T(50)+T(5)+T(6)+T(10)+T(54)));
//G379: __E_cli_command_111(239) => valid_sym(240) __(27) normalize_cmd_arg(241).
	p(NT(239), (NT(240)+NT(27)+NT(241)));
//G380: valid_cmd(238)       => __E_cli_command_111(239).
	p(NT(238), (NT(239)));
//G381: cli_command(216)     => valid_cmd(238).
	p(NT(216), (NT(238)));
//G382: sat_sym(244)         => 's' 'a' 't'.
	p(NT(244), (T(12)+T(5)+T(13)));
//G383: __E_cli_command_112(243) => sat_sym(244) __(27) normalize_cmd_arg(241).
	p(NT(243), (NT(244)+NT(27)+NT(241)));
//G384: sat_cmd(242)         => __E_cli_command_112(243).
	p(NT(242), (NT(243)));
//G385: cli_command(216)     => sat_cmd(242).
	p(NT(216), (NT(242)));
//G386: unsat_sym(247)       => 'u' 'n' 's' 'a' 't'.
	p(NT(247), (T(41)+T(51)+T(12)+T(5)+T(13)));
//G387: __E_cli_command_113(246) => unsat_sym(247) __(27) normalize_cmd_arg(241).
	p(NT(246), (NT(247)+NT(27)+NT(241)));
//G388: unsat_cmd(245)       => __E_cli_command_113(246).
	p(NT(245), (NT(246)));
//G389: cli_command(216)     => unsat_cmd(245).
	p(NT(216), (NT(245)));
//G390: solve_sym(250)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(250), (T(12)+T(17)+T(6)+T(50)+T(19)));
//G391: __E___E_cli_command_114_115(251) => solve_options(252).
	p(NT(251), (NT(252)));
//G392: __E___E_cli_command_114_115(251) => null.
	p(NT(251), (nul));
//G393: __E_cli_command_114(249) => solve_sym(250) __E___E_cli_command_114_115(251) __(27) wff_cmd_arg(253).
	p(NT(249), (NT(250)+NT(251)+NT(27)+NT(253)));
//G394: solve_cmd(248)       => __E_cli_command_114(249).
	p(NT(248), (NT(249)));
//G395: cli_command(216)     => solve_cmd(248).
	p(NT(216), (NT(248)));
//G396: lgrs_sym(256)        => 'l' 'g' 'r' 's'.
	p(NT(256), (T(6)+T(55)+T(11)+T(12)));
//G397: __E_cli_command_116(255) => lgrs_sym(256) __(27) wff_cmd_arg(253).
	p(NT(255), (NT(256)+NT(27)+NT(253)));
//G398: lgrs_cmd(254)        => __E_cli_command_116(255).
	p(NT(254), (NT(255)));
//G399: cli_command(216)     => lgrs_cmd(254).
	p(NT(216), (NT(254)));
//G400: __E___E_cli_command_117_118(260) => 'r'.
	p(NT(260), (T(11)));
//G401: __E___E_cli_command_117_118(260) => 'r' 'u' 'n'.
	p(NT(260), (T(11)+T(41)+T(51)));
//G402: run_sym(259)         => __E___E_cli_command_117_118(260).
	p(NT(259), (NT(260)));
//G403: __E___E_cli_command_117_119(261) => memory(262).
	p(NT(261), (NT(262)));
//G404: __E___E_cli_command_117_119(261) => wff(14).
	p(NT(261), (NT(14)));
//G405: __E_cli_command_117(258) => run_sym(259) __(27) __E___E_cli_command_117_119(261).
	p(NT(258), (NT(259)+NT(27)+NT(261)));
//G406: run_cmd(257)         => __E_cli_command_117(258).
	p(NT(257), (NT(258)));
//G407: cli_command(216)     => run_cmd(257).
	p(NT(216), (NT(257)));
//G408: __E___E_cli_command_120_121(266) => 'n'.
	p(NT(266), (T(51)));
//G409: __E___E_cli_command_120_121(266) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(266), (T(51)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(56)+T(19)));
//G410: normalize_sym(265)   => __E___E_cli_command_120_121(266).
	p(NT(265), (NT(266)));
//G411: __E_cli_command_120(264) => normalize_sym(265) __(27) normalize_cmd_arg(241).
	p(NT(264), (NT(265)+NT(27)+NT(241)));
//G412: normalize_cmd(263)   => __E_cli_command_120(264).
	p(NT(263), (NT(264)));
//G413: cli_command(216)     => normalize_cmd(263).
	p(NT(216), (NT(263)));
//G414: __E___E_cli_command_122_123(270) => 's'.
	p(NT(270), (T(12)));
//G415: __E___E_cli_command_122_123(270) => 's' 'u' 'b' 's' 't'.
	p(NT(270), (T(12)+T(41)+T(7)+T(12)+T(13)));
//G416: __E___E_cli_command_122_123(270) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(270), (T(12)+T(41)+T(7)+T(12)+T(13)+T(10)+T(13)+T(41)+T(13)+T(19)));
//G417: subst_sym(269)       => __E___E_cli_command_122_123(270).
	p(NT(269), (NT(270)));
//G418: __E_cli_command_122(268) => subst_sym(269) __(27) nf_cmd_arg(271) _(11) '[' _(11) nf_cmd_arg(271) _(11) '/' _(11) nf_cmd_arg(271) _(11) ']'.
	p(NT(268), (NT(269)+NT(27)+NT(271)+NT(11)+T(24)+NT(11)+NT(271)+NT(11)+T(57)+NT(11)+NT(271)+NT(11)+T(25)));
//G419: subst_cmd(267)       => __E_cli_command_122(268).
	p(NT(267), (NT(268)));
//G420: cli_command(216)     => subst_cmd(267).
	p(NT(216), (NT(267)));
//G421: __E___E_cli_command_124_125(275) => 'i'.
	p(NT(275), (T(10)));
//G422: __E___E_cli_command_124_125(275) => 'i' 'n' 's' 't'.
	p(NT(275), (T(10)+T(51)+T(12)+T(13)));
//G423: __E___E_cli_command_124_125(275) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(275), (T(10)+T(51)+T(12)+T(13)+T(5)+T(51)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G424: inst_sym(274)        => __E___E_cli_command_124_125(275).
	p(NT(274), (NT(275)));
//G425: __E_cli_command_124(273) => inst_sym(274) __(27) inst_args(276).
	p(NT(273), (NT(274)+NT(27)+NT(276)));
//G426: inst_cmd(272)        => __E_cli_command_124(273).
	p(NT(272), (NT(273)));
//G427: cli_command(216)     => inst_cmd(272).
	p(NT(216), (NT(272)));
//G428: dnf_sym(279)         => 'd' 'n' 'f'.
	p(NT(279), (T(54)+T(51)+T(4)));
//G429: __E_cli_command_126(278) => dnf_sym(279) __(27) nf_cmd_arg(271).
	p(NT(278), (NT(279)+NT(27)+NT(271)));
//G430: dnf_cmd(277)         => __E_cli_command_126(278).
	p(NT(277), (NT(278)));
//G431: cli_command(216)     => dnf_cmd(277).
	p(NT(216), (NT(277)));
//G432: cnf_sym(282)         => 'c' 'n' 'f'.
	p(NT(282), (T(8)+T(51)+T(4)));
//G433: __E_cli_command_127(281) => cnf_sym(282) __(27) nf_cmd_arg(271).
	p(NT(281), (NT(282)+NT(27)+NT(271)));
//G434: cnf_cmd(280)         => __E_cli_command_127(281).
	p(NT(280), (NT(281)));
//G435: cli_command(216)     => cnf_cmd(280).
	p(NT(216), (NT(280)));
//G436: anf_sym(285)         => 'a' 'n' 'f'.
	p(NT(285), (T(5)+T(51)+T(4)));
//G437: __E_cli_command_128(284) => anf_sym(285) __(27) nf_cmd_arg(271).
	p(NT(284), (NT(285)+NT(27)+NT(271)));
//G438: anf_cmd(283)         => __E_cli_command_128(284).
	p(NT(283), (NT(284)));
//G439: cli_command(216)     => anf_cmd(283).
	p(NT(216), (NT(283)));
//G440: nnf_sym(288)         => 'n' 'n' 'f'.
	p(NT(288), (T(51)+T(51)+T(4)));
//G441: __E_cli_command_129(287) => nnf_sym(288) __(27) nf_cmd_arg(271).
	p(NT(287), (NT(288)+NT(27)+NT(271)));
//G442: nnf_cmd(286)         => __E_cli_command_129(287).
	p(NT(286), (NT(287)));
//G443: cli_command(216)     => nnf_cmd(286).
	p(NT(216), (NT(286)));
//G444: pnf_sym(291)         => 'p' 'n' 'f'.
	p(NT(291), (T(53)+T(51)+T(4)));
//G445: __E_cli_command_130(290) => pnf_sym(291) __(27) nf_cmd_arg(271).
	p(NT(290), (NT(291)+NT(27)+NT(271)));
//G446: pnf_cmd(289)         => __E_cli_command_130(290).
	p(NT(289), (NT(290)));
//G447: cli_command(216)     => pnf_cmd(289).
	p(NT(216), (NT(289)));
//G448: mnf_sym(294)         => 'm' 'n' 'f'.
	p(NT(294), (T(18)+T(51)+T(4)));
//G449: __E_cli_command_131(293) => mnf_sym(294) __(27) nf_cmd_arg(271).
	p(NT(293), (NT(294)+NT(27)+NT(271)));
//G450: mnf_cmd(292)         => __E_cli_command_131(293).
	p(NT(292), (NT(293)));
//G451: cli_command(216)     => mnf_cmd(292).
	p(NT(216), (NT(292)));
//G452: snf_sym(297)         => 's' 'n' 'f'.
	p(NT(297), (T(12)+T(51)+T(4)));
//G453: __E_cli_command_132(296) => snf_sym(297) __(27) nf_cmd_arg(271).
	p(NT(296), (NT(297)+NT(27)+NT(271)));
//G454: snf_cmd(295)         => __E_cli_command_132(296).
	p(NT(295), (NT(296)));
//G455: cli_command(216)     => snf_cmd(295).
	p(NT(216), (NT(295)));
//G456: onf_sym(300)         => 'o' 'n' 'f'.
	p(NT(300), (T(17)+T(51)+T(4)));
//G457: __E_cli_command_133(299) => onf_sym(300) __(27) variable(118) __(27) onf_cmd_arg(301).
	p(NT(299), (NT(300)+NT(27)+NT(118)+NT(27)+NT(301)));
//G458: onf_cmd(298)         => __E_cli_command_133(299).
	p(NT(298), (NT(299)));
//G459: cli_command(216)     => onf_cmd(298).
	p(NT(216), (NT(298)));
//G460: __E___E_cli_command_134_135(305) => 'd' 'e' 'f' 's'.
	p(NT(305), (T(54)+T(19)+T(4)+T(12)));
//G461: __E___E_cli_command_134_135(305) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(305), (T(54)+T(19)+T(4)+T(10)+T(51)+T(10)+T(13)+T(10)+T(17)+T(51)+T(12)));
//G462: def_sym(304)         => __E___E_cli_command_134_135(305).
	p(NT(304), (NT(305)));
//G463: __E_cli_command_134(303) => def_sym(304).
	p(NT(303), (NT(304)));
//G464: def_list_cmd(302)    => __E_cli_command_134(303).
	p(NT(302), (NT(303)));
//G465: cli_command(216)     => def_list_cmd(302).
	p(NT(216), (NT(302)));
//G466: __E_cli_command_136(307) => def_sym(304) __(27) number(308).
	p(NT(307), (NT(304)+NT(27)+NT(308)));
//G467: def_print_cmd(306)   => __E_cli_command_136(307).
	p(NT(306), (NT(307)));
//G468: cli_command(216)     => def_print_cmd(306).
	p(NT(216), (NT(306)));
//G469: def_rr_cmd(309)      => rec_relation(16).
	p(NT(309), (NT(16)));
//G470: cli_command(216)     => def_rr_cmd(309).
	p(NT(216), (NT(309)));
//G471: qelim_sym(312)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(312), (T(49)+T(19)+T(6)+T(10)+T(18)));
//G472: __E_cli_command_137(311) => qelim_sym(312) __(27) wff_cmd_arg(253).
	p(NT(311), (NT(312)+NT(27)+NT(253)));
//G473: qelim_cmd(310)       => __E_cli_command_137(311).
	p(NT(310), (NT(311)));
//G474: cli_command(216)     => qelim_cmd(310).
	p(NT(216), (NT(310)));
//G475: get_sym(315)         => 'g' 'e' 't'.
	p(NT(315), (T(55)+T(19)+T(13)));
//G476: __E___E_cli_command_138_139(316) => __(27) option_name(317).
	p(NT(316), (NT(27)+NT(317)));
//G477: __E___E_cli_command_138_139(316) => null.
	p(NT(316), (nul));
//G478: __E_cli_command_138(314) => get_sym(315) __E___E_cli_command_138_139(316).
	p(NT(314), (NT(315)+NT(316)));
//G479: get_cmd(313)         => __E_cli_command_138(314).
	p(NT(313), (NT(314)));
//G480: cli_command(216)     => get_cmd(313).
	p(NT(216), (NT(313)));
//G481: set_sym(320)         => 's' 'e' 't'.
	p(NT(320), (T(12)+T(19)+T(13)));
//G482: __E___E_cli_command_140_141(321) => __(27).
	p(NT(321), (NT(27)));
//G483: __E___E_cli_command_140_141(321) => _(11) '=' _(11).
	p(NT(321), (NT(11)+T(3)+NT(11)));
//G484: __E_cli_command_140(319) => set_sym(320) __(27) option_name(317) __E___E_cli_command_140_141(321) option_value(322).
	p(NT(319), (NT(320)+NT(27)+NT(317)+NT(321)+NT(322)));
//G485: set_cmd(318)         => __E_cli_command_140(319).
	p(NT(318), (NT(319)));
//G486: cli_command(216)     => set_cmd(318).
	p(NT(216), (NT(318)));
//G487: enable_sym(325)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(325), (T(19)+T(51)+T(5)+T(7)+T(6)+T(19)));
//G488: __E_cli_command_142(324) => enable_sym(325) __(27) option_name(317).
	p(NT(324), (NT(325)+NT(27)+NT(317)));
//G489: enable_cmd(323)      => __E_cli_command_142(324).
	p(NT(323), (NT(324)));
//G490: cli_command(216)     => enable_cmd(323).
	p(NT(216), (NT(323)));
//G491: disable_sym(328)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(328), (T(54)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G492: __E_cli_command_143(327) => disable_sym(328) __(27) option_name(317).
	p(NT(327), (NT(328)+NT(27)+NT(317)));
//G493: disable_cmd(326)     => __E_cli_command_143(327).
	p(NT(326), (NT(327)));
//G494: cli_command(216)     => disable_cmd(326).
	p(NT(216), (NT(326)));
//G495: toggle_sym(331)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(331), (T(13)+T(17)+T(55)+T(55)+T(6)+T(19)));
//G496: __E_cli_command_144(330) => toggle_sym(331) __(27) option_name(317).
	p(NT(330), (NT(331)+NT(27)+NT(317)));
//G497: toggle_cmd(329)      => __E_cli_command_144(330).
	p(NT(329), (NT(330)));
//G498: cli_command(216)     => toggle_cmd(329).
	p(NT(216), (NT(329)));
//G499: __E_cli_command_145(333) => type(129) __(27) in_var_name(170) _(11) '=' _(11) input_stream(334).
	p(NT(333), (NT(129)+NT(27)+NT(170)+NT(11)+T(3)+NT(11)+NT(334)));
//G500: def_input_cmd(332)   => __E_cli_command_145(333).
	p(NT(332), (NT(333)));
//G501: cli_command(216)     => def_input_cmd(332).
	p(NT(216), (NT(332)));
//G502: __E_cli_command_146(336) => type(129) __(27) out_var_name(174) _(11) '=' _(11) output_stream(337).
	p(NT(336), (NT(129)+NT(27)+NT(174)+NT(11)+T(3)+NT(11)+NT(337)));
//G503: def_output_cmd(335)  => __E_cli_command_146(336).
	p(NT(335), (NT(336)));
//G504: cli_command(216)     => def_output_cmd(335).
	p(NT(216), (NT(335)));
//G505: __E___E_cli_command_147_148(341) => 'h' 'i' 's' 't'.
	p(NT(341), (T(52)+T(10)+T(12)+T(13)));
//G506: __E___E_cli_command_147_148(341) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(341), (T(52)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G507: history_sym(340)     => __E___E_cli_command_147_148(341).
	p(NT(340), (NT(341)));
//G508: __E_cli_command_147(339) => history_sym(340).
	p(NT(339), (NT(340)));
//G509: history_list_cmd(338) => __E_cli_command_147(339).
	p(NT(338), (NT(339)));
//G510: cli_command(216)     => history_list_cmd(338).
	p(NT(216), (NT(338)));
//G511: __E_cli_command_149(343) => history_sym(340) __(27) memory(262).
	p(NT(343), (NT(340)+NT(27)+NT(262)));
//G512: history_print_cmd(342) => __E_cli_command_149(343).
	p(NT(342), (NT(343)));
//G513: cli_command(216)     => history_print_cmd(342).
	p(NT(216), (NT(342)));
//G514: __E_cli_command_150(345) => wff(14).
	p(NT(345), (NT(14)));
//G515: __E_cli_command_150(345) => bf(21).
	p(NT(345), (NT(21)));
//G516: history_store_cmd(344) => __E_cli_command_150(345).
	p(NT(344), (NT(345)));
//G517: cli_command(216)     => history_store_cmd(344).
	p(NT(216), (NT(344)));
//G518: __E_solve_options_151(346) => __(27) solve_option(347).
	p(NT(346), (NT(27)+NT(347)));
//G519: __E_solve_options_152(348) => null.
	p(NT(348), (nul));
//G520: __E_solve_options_152(348) => __E_solve_options_151(346) __E_solve_options_152(348).
	p(NT(348), (NT(346)+NT(348)));
//G521: solve_options(252)   => __E_solve_options_152(348).
	p(NT(252), (NT(348)));
//G522: __E_solve_option_153(349) => '-' '-' solver_mode(350).
	p(NT(349), (T(28)+T(28)+NT(350)));
//G523: solve_option(347)    => __E_solve_option_153(349).
	p(NT(347), (NT(349)));
//G524: __E_solve_option_154(351) => '-' '-' type(129).
	p(NT(351), (T(28)+T(28)+NT(129)));
//G525: solve_option(347)    => __E_solve_option_154(351).
	p(NT(347), (NT(351)));
//G526: __E_solver_mode_155(353) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(353), (T(18)+T(10)+T(51)+T(10)+T(18)+T(41)+T(18)));
//G527: __E_solver_mode_155(353) => 'm' 'i' 'n'.
	p(NT(353), (T(18)+T(10)+T(51)));
//G528: solver_mode_minimum(352) => __E_solver_mode_155(353).
	p(NT(352), (NT(353)));
//G529: solver_mode(350)     => solver_mode_minimum(352).
	p(NT(350), (NT(352)));
//G530: __E_solver_mode_156(355) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(355), (T(18)+T(5)+T(27)+T(10)+T(18)+T(41)+T(18)));
//G531: __E_solver_mode_156(355) => 'm' 'a' 'x'.
	p(NT(355), (T(18)+T(5)+T(27)));
//G532: solver_mode_maximum(354) => __E_solver_mode_156(355).
	p(NT(354), (NT(355)));
//G533: solver_mode(350)     => solver_mode_maximum(354).
	p(NT(350), (NT(354)));
//G534: __E_input_stream_157(356) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(357) _(11) ')'.
	p(NT(356), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(357)+NT(11)+T(16)));
//G535: input_stream(334)    => __E_input_stream_157(356).
	p(NT(334), (NT(356)));
//G536: console_sym(358)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(358), (T(8)+T(17)+T(51)+T(12)+T(17)+T(6)+T(19)));
//G537: input_stream(334)    => console_sym(358).
	p(NT(334), (NT(358)));
//G538: __E_output_stream_158(359) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(357) _(11) ')'.
	p(NT(359), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(357)+NT(11)+T(16)));
//G539: output_stream(337)   => __E_output_stream_158(359).
	p(NT(337), (NT(359)));
//G540: output_stream(337)   => console_sym(358).
	p(NT(337), (NT(358)));
//G541: q_file_name(357)     => '"' file_name(360) '"'.
	p(NT(357), (T(58)+NT(360)+T(58)));
//G542: __E_file_name_159(361) => printable(8).
	p(NT(361), (NT(8)));
//G543: __E_file_name_159(361) => printable(8) __E_file_name_159(361).
	p(NT(361), (NT(8)+NT(361)));
//G544: file_name(360)       => __E_file_name_159(361).
	p(NT(360), (NT(361)));
//G545: __E_option_name_160(362) => alnum(6).
	p(NT(362), (NT(6)));
//G546: __E_option_name_160(362) => alnum(6) __E_option_name_160(362).
	p(NT(362), (NT(6)+NT(362)));
//G547: option_name(317)     => __E_option_name_160(362).
	p(NT(317), (NT(362)));
//G548: __E_option_value_161(363) => alnum(6).
	p(NT(363), (NT(6)));
//G549: __E_option_value_161(363) => alnum(6) __E_option_value_161(363).
	p(NT(363), (NT(6)+NT(363)));
//G550: option_value(322)    => __E_option_value_161(363).
	p(NT(322), (NT(363)));
//G551: number(308)          => digits(189).
	p(NT(308), (NT(189)));
//G552: bf_cmd_arg(364)      => memory(262).
	p(NT(364), (NT(262)));
//G553: bf_cmd_arg(364)      => bf(21).
	p(NT(364), (NT(21)));
//G554: wff_cmd_arg(253)     => memory(262).
	p(NT(253), (NT(262)));
//G555: wff_cmd_arg(253)     => wff(14).
	p(NT(253), (NT(14)));
//G556: nf_cmd_arg(271)      => memory(262).
	p(NT(271), (NT(262)));
//G557: nf_cmd_arg(271)      => ref(18).
	p(NT(271), (NT(18)));
//G558: nf_cmd_arg(271)      => wff(14).
	p(NT(271), (NT(14)));
//G559: nf_cmd_arg(271)      => bf(21).
	p(NT(271), (NT(21)));
//G560: onf_cmd_arg(301)     => memory(262).
	p(NT(301), (NT(262)));
//G561: onf_cmd_arg(301)     => wff(14).
	p(NT(301), (NT(14)));
//G562: normalize_cmd_arg(241) => memory(262).
	p(NT(241), (NT(262)));
//G563: normalize_cmd_arg(241) => rr(10).
	p(NT(241), (NT(10)));
//G564: normalize_cmd_arg(241) => ref(18).
	p(NT(241), (NT(18)));
//G565: normalize_cmd_arg(241) => wff(14).
	p(NT(241), (NT(14)));
//G566: normalize_cmd_arg(241) => bf(21).
	p(NT(241), (NT(21)));
//G567: inst_args(276)       => wff_cmd_arg(253) _(11) '[' _(11) variable(118) _(11) '/' _(11) bf_cmd_arg(364) _(11) ']'.
	p(NT(276), (NT(253)+NT(11)+T(24)+NT(11)+NT(118)+NT(11)+T(57)+NT(11)+NT(364)+NT(11)+T(25)));
//G568: inst_args(276)       => bf_cmd_arg(364) _(11) '[' _(11) variable(118) _(11) '/' _(11) bf_cmd_arg(364) _(11) ']'.
	p(NT(276), (NT(364)+NT(11)+T(24)+NT(11)+NT(118)+NT(11)+T(57)+NT(11)+NT(364)+NT(11)+T(25)));
//G569: help_arg(233)        => help_sym(230).
	p(NT(233), (NT(230)));
//G570: help_arg(233)        => version_sym(223).
	p(NT(233), (NT(223)));
//G571: help_arg(233)        => quit_sym(220).
	p(NT(233), (NT(220)));
//G572: help_arg(233)        => clear_sym(226).
	p(NT(233), (NT(226)));
//G573: help_arg(233)        => get_sym(315).
	p(NT(233), (NT(315)));
//G574: help_arg(233)        => set_sym(320).
	p(NT(233), (NT(320)));
//G575: help_arg(233)        => enable_sym(325).
	p(NT(233), (NT(325)));
//G576: help_arg(233)        => disable_sym(328).
	p(NT(233), (NT(328)));
//G577: help_arg(233)        => toggle_sym(331).
	p(NT(233), (NT(331)));
//G578: help_arg(233)        => file_sym(236).
	p(NT(233), (NT(236)));
//G579: help_arg(233)        => history_sym(340).
	p(NT(233), (NT(340)));
//G580: help_arg(233)        => abs_memory_sym(365).
	p(NT(233), (NT(365)));
//G581: help_arg(233)        => rel_memory_sym(366).
	p(NT(233), (NT(366)));
//G582: help_arg(233)        => selection_sym(367).
	p(NT(233), (NT(367)));
//G583: help_arg(233)        => def_sym(304).
	p(NT(233), (NT(304)));
//G584: help_arg(233)        => inst_sym(274).
	p(NT(233), (NT(274)));
//G585: help_arg(233)        => subst_sym(269).
	p(NT(233), (NT(269)));
//G586: help_arg(233)        => normalize_sym(265).
	p(NT(233), (NT(265)));
//G587: help_arg(233)        => execute_sym(368).
	p(NT(233), (NT(368)));
//G588: help_arg(233)        => solve_sym(250).
	p(NT(233), (NT(250)));
//G589: help_arg(233)        => valid_sym(240).
	p(NT(233), (NT(240)));
//G590: help_arg(233)        => sat_sym(244).
	p(NT(233), (NT(244)));
//G591: help_arg(233)        => unsat_sym(247).
	p(NT(233), (NT(247)));
//G592: help_arg(233)        => run_sym(259).
	p(NT(233), (NT(259)));
//G593: help_arg(233)        => dnf_sym(279).
	p(NT(233), (NT(279)));
//G594: help_arg(233)        => cnf_sym(282).
	p(NT(233), (NT(282)));
//G595: help_arg(233)        => anf_sym(285).
	p(NT(233), (NT(285)));
//G596: help_arg(233)        => snf_sym(297).
	p(NT(233), (NT(297)));
//G597: help_arg(233)        => nnf_sym(288).
	p(NT(233), (NT(288)));
//G598: help_arg(233)        => pnf_sym(291).
	p(NT(233), (NT(291)));
//G599: help_arg(233)        => mnf_sym(294).
	p(NT(233), (NT(294)));
//G600: help_arg(233)        => onf_sym(300).
	p(NT(233), (NT(300)));
//G601: help_arg(233)        => qelim_sym(312).
	p(NT(233), (NT(312)));
//G602: __E___E_help_arg_162_163(371) => 's'.
	p(NT(371), (T(12)));
//G603: __E___E_help_arg_162_163(371) => null.
	p(NT(371), (nul));
//G604: __E_help_arg_162(370) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_162_163(371).
	p(NT(370), (T(19)+T(27)+T(5)+T(18)+T(53)+T(6)+T(19)+NT(371)));
//G605: examples_sym(369)    => __E_help_arg_162(370).
	p(NT(369), (NT(370)));
//G606: help_arg(233)        => examples_sym(369).
	p(NT(233), (NT(369)));
//G607: __E_memory_164(373)  => '%'.
	p(NT(373), (T(59)));
//G608: rel_memory_sym(366)  => '%' '-'.
	p(NT(366), (T(59)+T(28)));
//G609: memory_id(375)       => digits(189).
	p(NT(375), (NT(189)));
//G610: __E___E_memory_164_165(374) => memory_id(375).
	p(NT(374), (NT(375)));
//G611: __E___E_memory_164_165(374) => null.
	p(NT(374), (nul));
//G612: __E_memory_164(373)  => rel_memory_sym(366) __E___E_memory_164_165(374).
	p(NT(373), (NT(366)+NT(374)));
//G613: rel_memory(372)      => __E_memory_164(373).
	p(NT(372), (NT(373)));
//G614: memory(262)          => rel_memory(372).
	p(NT(262), (NT(372)));
//G615: abs_memory_sym(365)  => '%'.
	p(NT(365), (T(59)));
//G616: __E_memory_166(377)  => abs_memory_sym(365) memory_id(375).
	p(NT(377), (NT(365)+NT(375)));
//G617: abs_memory(376)      => __E_memory_166(377).
	p(NT(376), (NT(377)));
//G618: memory(262)          => abs_memory(376).
	p(NT(262), (NT(376)));
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
		in_var_name, __E_in_71, __E___E_in_71_72, offset, out_var_name, __E_out_73, __E___E_out_73_74, __E___E___E_out_73_74_75, chars, q_var, 
		__E_q_vars_76, __E_q_vars_77, __E_offsets_78, __E_offsets_79, integer, shift, __E_offset_80, __E_shift_81, __E___E_shift_81_82, digits, 
		__E_integer_83, uninter_const_name, __E_uninterpreted_constant_84, __E___E_uninterpreted_constant_84_85, constant, __E_bf_constant_86, binding, __E_bf_constant_87, source, __E_binding_88, 
		src_c, __E___E_binding_88_89, __E___E___E_binding_88_89_90, __E___E___E_binding_88_89_91, __E_src_c_92, __E_src_c_93, __E_chars_94, __E_chars_95, __E_digits_96, comment, 
		__E_comment_97, __E_comment_98, __E_comment_99, __E____100, __E___101, cli, cli_command, __E_cli_102, __E_cli_103, quit_cmd, 
		quit_sym, __E_cli_command_104, version_cmd, version_sym, __E_cli_command_105, clear_cmd, clear_sym, __E_cli_command_106, help_cmd, __E_cli_command_107, 
		help_sym, __E___E_cli_command_107_108, __E___E_cli_command_107_109, help_arg, file_cmd, __E_cli_command_110, file_sym, q_string, valid_cmd, __E_cli_command_111, 
		valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_112, sat_sym, unsat_cmd, __E_cli_command_113, unsat_sym, solve_cmd, __E_cli_command_114, 
		solve_sym, __E___E_cli_command_114_115, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_116, lgrs_sym, run_cmd, __E_cli_command_117, run_sym, 
		__E___E_cli_command_117_118, __E___E_cli_command_117_119, memory, normalize_cmd, __E_cli_command_120, normalize_sym, __E___E_cli_command_120_121, subst_cmd, __E_cli_command_122, subst_sym, 
		__E___E_cli_command_122_123, nf_cmd_arg, inst_cmd, __E_cli_command_124, inst_sym, __E___E_cli_command_124_125, inst_args, dnf_cmd, __E_cli_command_126, dnf_sym, 
		cnf_cmd, __E_cli_command_127, cnf_sym, anf_cmd, __E_cli_command_128, anf_sym, nnf_cmd, __E_cli_command_129, nnf_sym, pnf_cmd, 
		__E_cli_command_130, pnf_sym, mnf_cmd, __E_cli_command_131, mnf_sym, snf_cmd, __E_cli_command_132, snf_sym, onf_cmd, __E_cli_command_133, 
		onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_134, def_sym, __E___E_cli_command_134_135, def_print_cmd, __E_cli_command_136, number, def_rr_cmd, 
		qelim_cmd, __E_cli_command_137, qelim_sym, get_cmd, __E_cli_command_138, get_sym, __E___E_cli_command_138_139, option_name, set_cmd, __E_cli_command_140, 
		set_sym, __E___E_cli_command_140_141, option_value, enable_cmd, __E_cli_command_142, enable_sym, disable_cmd, __E_cli_command_143, disable_sym, toggle_cmd, 
		__E_cli_command_144, toggle_sym, def_input_cmd, __E_cli_command_145, input_stream, def_output_cmd, __E_cli_command_146, output_stream, history_list_cmd, __E_cli_command_147, 
		history_sym, __E___E_cli_command_147_148, history_print_cmd, __E_cli_command_149, history_store_cmd, __E_cli_command_150, __E_solve_options_151, solve_option, __E_solve_options_152, __E_solve_option_153, 
		solver_mode, __E_solve_option_154, solver_mode_minimum, __E_solver_mode_155, solver_mode_maximum, __E_solver_mode_156, __E_input_stream_157, q_file_name, console_sym, __E_output_stream_158, 
		file_name, __E_file_name_159, __E_option_name_160, __E_option_value_161, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, execute_sym, examples_sym, 
		__E_help_arg_162, __E___E_help_arg_162_163, rel_memory, __E_memory_164, __E___E_memory_164_165, memory_id, abs_memory, __E_memory_166, named_binding, __N_0, 
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
