// This file is generated from a file parser/tau.tgf by
//       https://github.com/IDNI/parser/src/tgf
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
	"in_var_name", "__E_in_71", "__E___E_in_71_72", "__E___E___E_in_71_72_73", "offset", "out_var_name", "__E_out_74", "__E___E_out_74_75", "__E___E___E_out_74_75_76", "chars", 
	"q_var", "__E_q_vars_77", "__E_q_vars_78", "__E_offsets_79", "__E_offsets_80", "integer", "shift", "__E_offset_81", "__E_shift_82", "__E___E_shift_82_83", 
	"digits", "__E_integer_84", "uninter_const_name", "__E_uninterpreted_constant_85", "__E___E_uninterpreted_constant_85_86", "constant", "__E_bf_constant_87", "binding", "__E_bf_constant_88", "source", 
	"__E_binding_89", "src_c", "__E___E_binding_89_90", "__E___E___E_binding_89_90_91", "__E___E___E_binding_89_90_92", "__E_src_c_93", "__E_src_c_94", "__E_chars_95", "__E_chars_96", "__E_digits_97", 
	"comment", "__E_comment_98", "__E_comment_99", "__E_comment_100", "__E____101", "__E___102", "cli", "cli_command", "__E_cli_103", "__E_cli_104", 
	"quit_cmd", "quit_sym", "__E_cli_command_105", "version_cmd", "version_sym", "__E_cli_command_106", "clear_cmd", "clear_sym", "__E_cli_command_107", "help_cmd", 
	"__E_cli_command_108", "help_sym", "__E___E_cli_command_108_109", "__E___E_cli_command_108_110", "help_arg", "file_cmd", "__E_cli_command_111", "file_sym", "q_string", "valid_cmd", 
	"__E_cli_command_112", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_113", "sat_sym", "unsat_cmd", "__E_cli_command_114", "unsat_sym", "solve_cmd", 
	"__E_cli_command_115", "solve_sym", "__E___E_cli_command_115_116", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_117", "lgrs_sym", "run_cmd", "__E_cli_command_118", 
	"run_sym", "__E___E_cli_command_118_119", "__E___E_cli_command_118_120", "memory", "normalize_cmd", "__E_cli_command_121", "normalize_sym", "__E___E_cli_command_121_122", "subst_cmd", "__E_cli_command_123", 
	"subst_sym", "__E___E_cli_command_123_124", "nf_cmd_arg", "inst_cmd", "__E_cli_command_125", "inst_sym", "__E___E_cli_command_125_126", "inst_args", "dnf_cmd", "__E_cli_command_127", 
	"dnf_sym", "cnf_cmd", "__E_cli_command_128", "cnf_sym", "anf_cmd", "__E_cli_command_129", "anf_sym", "nnf_cmd", "__E_cli_command_130", "nnf_sym", 
	"pnf_cmd", "__E_cli_command_131", "pnf_sym", "mnf_cmd", "__E_cli_command_132", "mnf_sym", "snf_cmd", "__E_cli_command_133", "snf_sym", "onf_cmd", 
	"__E_cli_command_134", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_135", "def_sym", "__E___E_cli_command_135_136", "def_print_cmd", "__E_cli_command_137", "number", 
	"def_rr_cmd", "qelim_cmd", "__E_cli_command_138", "qelim_sym", "get_cmd", "__E_cli_command_139", "get_sym", "__E___E_cli_command_139_140", "option_name", "set_cmd", 
	"__E_cli_command_141", "set_sym", "__E___E_cli_command_141_142", "option_value", "enable_cmd", "__E_cli_command_143", "enable_sym", "disable_cmd", "__E_cli_command_144", "disable_sym", 
	"toggle_cmd", "__E_cli_command_145", "toggle_sym", "def_input_cmd", "__E_cli_command_146", "input_stream", "def_output_cmd", "__E_cli_command_147", "output_stream", "history_list_cmd", 
	"__E_cli_command_148", "history_sym", "__E___E_cli_command_148_149", "history_print_cmd", "__E_cli_command_150", "history_store_cmd", "__E_cli_command_151", "__E_solve_options_152", "solve_option", "__E_solve_options_153", 
	"__E_solve_option_154", "solver_mode", "__E_solve_option_155", "solver_mode_minimum", "__E_solver_mode_156", "solver_mode_maximum", "__E_solver_mode_157", "__E_input_stream_158", "q_file_name", "console_sym", 
	"__E_output_stream_159", "file_name", "__E_file_name_160", "__E_option_name_161", "__E_option_value_162", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "execute_sym", 
	"examples_sym", "__E_help_arg_163", "__E___E_help_arg_163_164", "rel_memory", "__E_memory_165", "__E___E_memory_165_166", "memory_id", "abs_memory", "__E_memory_167", "named_binding", 
	"__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'f', 'a', 'l', 'b', 'c', 
	'k', 'i', 'r', 's', 't', ',', '(', ')', 'o', 'm', 
	'e', '<', '>', 'w', 'y', '[', ']', '?', 'x', '-', 
	'|', '^', '&', '!', 'T', 'F', 'S', '+', '1', '0', 
	'\'', '_', 'h', 'u', '$', '{', '}', '\t', '\n', '\r', 
	'#', 'q', 'v', 'n', 'p', 'd', 'g', 'z', '/', '"', 
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
			20, 22, 118, 129, 148, 170, 175, 185, 190, 192,
			199, 318, 323, 361, 379
		},
		.to_inline = {
			{ 14, 60, 14 },
			{ 21, 115, 21 },
			{ 138, 115, 21 },
			{ 140, 115, 133 },
			{ 142, 115, 21 },
			{ 144, 115, 21 },
			{ 179 },
			{ 180 },
			{ 201 },
			{ 242 },
			{ 254 },
			{ 272 },
			{ 277 },
			{ 302 },
			{ 365 }
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
//G234: __N_0(380)           => 'F'.
	p(NT(380), (T(34)));
//G235: __N_1(381)           => 'T'.
	p(NT(381), (T(33)));
//G236: __E_variable_65(160) => ~( __N_0(380) ) & ~( __N_1(381) ) & alpha(5).	 # conjunctive
	p(NT(160), ~(NT(380)) & ~(NT(381)) & (NT(5)));
//G237: __E_variable_66(161) => null.
	p(NT(161), (nul));
//G238: __E_variable_66(161) => digit(3) __E_variable_66(161).
	p(NT(161), (NT(3)+NT(161)));
//G239: variable(118)        => __E_variable_65(160) __E_variable_66(161).	 # guarded: charvar
	p(NT(118), (NT(160)+NT(161)));
	p.back().guard = "charvar";
//G240: __N_2(382)           => 'F'.
	p(NT(382), (T(34)));
//G241: __N_3(383)           => 'T'.
	p(NT(383), (T(33)));
//G242: __E_variable_67(162) => ~( __N_2(382) ) & ~( __N_3(383) ) & alpha(5).	 # conjunctive
	p(NT(162), ~(NT(382)) & ~(NT(383)) & (NT(5)));
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
//G253: __E___E___E_in_71_72_73(173) => digit(3).
	p(NT(173), (NT(3)));
//G254: __E___E___E_in_71_72_73(173) => digit(3) __E___E___E_in_71_72_73(173).
	p(NT(173), (NT(3)+NT(173)));
//G255: __E___E_in_71_72(172) => 'i' __E___E___E_in_71_72_73(173).
	p(NT(172), (T(10)+NT(173)));
//G256: __E_in_71(171)       => __E___E_in_71_72(172).
	p(NT(171), (NT(172)));
//G257: __E_in_71(171)       => 't' 'h' 'i' 's'.
	p(NT(171), (T(13)+T(41)+T(10)+T(12)));
//G258: in_var_name(170)     => __E_in_71(171).
	p(NT(170), (NT(171)));
//G259: in(167)              => in_var_name(170) '[' offset(174) ']'.
	p(NT(167), (NT(170)+T(24)+NT(174)+T(25)));
//G260: __E___E___E_out_74_75_76(178) => digit(3).
	p(NT(178), (NT(3)));
//G261: __E___E___E_out_74_75_76(178) => digit(3) __E___E___E_out_74_75_76(178).
	p(NT(178), (NT(3)+NT(178)));
//G262: __E___E_out_74_75(177) => 'o' __E___E___E_out_74_75_76(178).
	p(NT(177), (T(17)+NT(178)));
//G263: __E_out_74(176)      => __E___E_out_74_75(177).
	p(NT(176), (NT(177)));
//G264: __E_out_74(176)      => 'u'.
	p(NT(176), (T(42)));
//G265: out_var_name(175)    => __E_out_74(176).
	p(NT(175), (NT(176)));
//G266: out(168)             => out_var_name(175) '[' offset(174) ']'.
	p(NT(168), (NT(175)+T(24)+NT(174)+T(25)));
//G267: ctnvar(148)          => chars(179).
	p(NT(148), (NT(179)));
//G268: capture(20)          => '$' chars(179).
	p(NT(20), (T(43)+NT(179)));
//G269: __E_q_vars_77(181)   => _(11) ',' _(11) q_var(180).
	p(NT(181), (NT(11)+T(14)+NT(11)+NT(180)));
//G270: __E_q_vars_78(182)   => null.
	p(NT(182), (nul));
//G271: __E_q_vars_78(182)   => __E_q_vars_77(181) __E_q_vars_78(182).
	p(NT(182), (NT(181)+NT(182)));
//G272: q_vars(72)           => q_var(180) __E_q_vars_78(182).
	p(NT(72), (NT(180)+NT(182)));
//G273: q_var(180)           => capture(20).
	p(NT(180), (NT(20)));
//G274: __N_4(384)           => uninterpreted_constant(169).
	p(NT(384), (NT(169)));
//G275: q_var(180)           => variable(118) & ~( __N_4(384) ).	 # conjunctive
	p(NT(180), (NT(118)) & ~(NT(384)));
//G276: __E_offsets_79(183)  => _(11) ',' _(11) offset(174).
	p(NT(183), (NT(11)+T(14)+NT(11)+NT(174)));
//G277: __E_offsets_80(184)  => null.
	p(NT(184), (nul));
//G278: __E_offsets_80(184)  => __E_offsets_79(183) __E_offsets_80(184).
	p(NT(184), (NT(183)+NT(184)));
//G279: offsets(24)          => '[' _(11) offset(174) __E_offsets_80(184) _(11) ']'.
	p(NT(24), (T(24)+NT(11)+NT(174)+NT(184)+NT(11)+T(25)));
//G280: offset(174)          => integer(185).
	p(NT(174), (NT(185)));
//G281: offset(174)          => capture(20).
	p(NT(174), (NT(20)));
//G282: offset(174)          => shift(186).
	p(NT(174), (NT(186)));
//G283: __N_5(385)           => io_var(165).
	p(NT(385), (NT(165)));
//G284: __E_offset_81(187)   => variable(118) & ~( __N_5(385) ).	 # conjunctive
	p(NT(187), (NT(118)) & ~(NT(385)));
//G285: offset(174)          => __E_offset_81(187).
	p(NT(174), (NT(187)));
//G286: __E_shift_82(188)    => capture(20).
	p(NT(188), (NT(20)));
//G287: __N_6(386)           => io_var(165).
	p(NT(386), (NT(165)));
//G288: __E___E_shift_82_83(189) => variable(118) & ~( __N_6(386) ).	 # conjunctive
	p(NT(189), (NT(118)) & ~(NT(386)));
//G289: __E_shift_82(188)    => __E___E_shift_82_83(189).
	p(NT(188), (NT(189)));
//G290: shift(186)           => __E_shift_82(188) _(11) '-' _(11) num(149).
	p(NT(186), (NT(188)+NT(11)+T(28)+NT(11)+NT(149)));
//G291: num(149)             => digits(190).
	p(NT(149), (NT(190)));
//G292: __E_integer_84(191)  => '-'.
	p(NT(191), (T(28)));
//G293: __E_integer_84(191)  => null.
	p(NT(191), (nul));
//G294: integer(185)         => __E_integer_84(191) _(11) digits(190).
	p(NT(185), (NT(191)+NT(11)+NT(190)));
//G295: __E___E_uninterpreted_constant_85_86(194) => chars(179).
	p(NT(194), (NT(179)));
//G296: __E___E_uninterpreted_constant_85_86(194) => _(11).
	p(NT(194), (NT(11)));
//G297: __E_uninterpreted_constant_85(193) => __E___E_uninterpreted_constant_85_86(194) ':' chars(179).
	p(NT(193), (NT(194)+T(2)+NT(179)));
//G298: uninter_const_name(192) => __E_uninterpreted_constant_85(193).
	p(NT(192), (NT(193)));
//G299: uninterpreted_constant(169) => '<' uninter_const_name(192) _(11) '>'.
	p(NT(169), (T(20)+NT(192)+NT(11)+T(21)));
//G300: __E_bf_constant_87(196) => capture(20).
	p(NT(196), (NT(20)));
//G301: __E_bf_constant_87(196) => binding(197).
	p(NT(196), (NT(197)));
//G302: constant(195)        => __E_bf_constant_87(196).
	p(NT(195), (NT(196)));
//G303: type(129)            => chars(179).
	p(NT(129), (NT(179)));
//G304: __E_bf_constant_88(198) => _(11) ':' _(11) type(129).
	p(NT(198), (NT(11)+T(2)+NT(11)+NT(129)));
//G305: __E_bf_constant_88(198) => null.
	p(NT(198), (nul));
//G306: bf_constant(117)     => '{' _(11) constant(195) _(11) '}' __E_bf_constant_88(198).
	p(NT(117), (T(44)+NT(11)+NT(195)+NT(11)+T(45)+NT(198)));
//G307: __E___E___E_binding_89_90_91(203) => src_c(201).
	p(NT(203), (NT(201)));
//G308: __E___E___E_binding_89_90_91(203) => space(2).
	p(NT(203), (NT(2)));
//G309: __E___E___E_binding_89_90_92(204) => null.
	p(NT(204), (nul));
//G310: __E___E___E_binding_89_90_92(204) => __E___E___E_binding_89_90_91(203) __E___E___E_binding_89_90_92(204).
	p(NT(204), (NT(203)+NT(204)));
//G311: __E___E_binding_89_90(202) => __E___E___E_binding_89_90_92(204) src_c(201).
	p(NT(202), (NT(204)+NT(201)));
//G312: __E___E_binding_89_90(202) => null.
	p(NT(202), (nul));
//G313: __E_binding_89(200)  => src_c(201) __E___E_binding_89_90(202).
	p(NT(200), (NT(201)+NT(202)));
//G314: source(199)          => __E_binding_89(200).
	p(NT(199), (NT(200)));
//G315: binding(197)         => source(199).
	p(NT(197), (NT(199)));
//G316: src_c(201)           => alnum(6).
	p(NT(201), (NT(6)));
//G317: __N_7(387)           => '{'.
	p(NT(387), (T(44)));
//G318: __N_8(388)           => '}'.
	p(NT(388), (T(45)));
//G319: src_c(201)           => ~( __N_7(387) ) & ~( __N_8(388) ) & punct(7).	 # conjunctive
	p(NT(201), ~(NT(387)) & ~(NT(388)) & (NT(7)));
//G320: __E_src_c_93(205)    => src_c(201).
	p(NT(205), (NT(201)));
//G321: __E_src_c_93(205)    => space(2).
	p(NT(205), (NT(2)));
//G322: __E_src_c_94(206)    => null.
	p(NT(206), (nul));
//G323: __E_src_c_94(206)    => __E_src_c_93(205) __E_src_c_94(206).
	p(NT(206), (NT(205)+NT(206)));
//G324: src_c(201)           => '{' __E_src_c_94(206) '}'.
	p(NT(201), (T(44)+NT(206)+T(45)));
//G325: __E_chars_95(207)    => alnum(6).
	p(NT(207), (NT(6)));
//G326: __E_chars_95(207)    => '_'.
	p(NT(207), (T(40)));
//G327: __E_chars_96(208)    => null.
	p(NT(208), (nul));
//G328: __E_chars_96(208)    => __E_chars_95(207) __E_chars_96(208).
	p(NT(208), (NT(207)+NT(208)));
//G329: chars(179)           => alpha(5) __E_chars_96(208).
	p(NT(179), (NT(5)+NT(208)));
//G330: __E_digits_97(209)   => digit(3).
	p(NT(209), (NT(3)));
//G331: __E_digits_97(209)   => digit(3) __E_digits_97(209).
	p(NT(209), (NT(3)+NT(209)));
//G332: digits(190)          => __E_digits_97(209).
	p(NT(190), (NT(209)));
//G333: sym(22)              => chars(179).
	p(NT(22), (NT(179)));
//G334: __E_comment_98(211)  => printable(8).
	p(NT(211), (NT(8)));
//G335: __E_comment_98(211)  => '\t'.
	p(NT(211), (T(46)));
//G336: __E_comment_99(212)  => null.
	p(NT(212), (nul));
//G337: __E_comment_99(212)  => __E_comment_98(211) __E_comment_99(212).
	p(NT(212), (NT(211)+NT(212)));
//G338: __E_comment_100(213) => '\n'.
	p(NT(213), (T(47)));
//G339: __E_comment_100(213) => '\r'.
	p(NT(213), (T(48)));
//G340: __E_comment_100(213) => eof(1).
	p(NT(213), (NT(1)));
//G341: comment(210)         => '#' __E_comment_99(212) __E_comment_100(213).
	p(NT(210), (T(49)+NT(212)+NT(213)));
//G342: __E____101(214)      => space(2).
	p(NT(214), (NT(2)));
//G343: __E____101(214)      => comment(210).
	p(NT(214), (NT(210)));
//G344: __(27)               => __E____101(214) _(11).
	p(NT(27), (NT(214)+NT(11)));
//G345: __E___102(215)       => __(27).
	p(NT(215), (NT(27)));
//G346: __E___102(215)       => null.
	p(NT(215), (nul));
//G347: _(11)                => __E___102(215).
	p(NT(11), (NT(215)));
//G348: cli(216)             => _(11).
	p(NT(216), (NT(11)));
//G349: __E_cli_103(218)     => '.' _(11) cli_command(217) _(11).
	p(NT(218), (T(1)+NT(11)+NT(217)+NT(11)));
//G350: __E_cli_104(219)     => null.
	p(NT(219), (nul));
//G351: __E_cli_104(219)     => __E_cli_103(218) __E_cli_104(219).
	p(NT(219), (NT(218)+NT(219)));
//G352: cli(216)             => _(11) cli_command(217) _(11) __E_cli_104(219).
	p(NT(216), (NT(11)+NT(217)+NT(11)+NT(219)));
//G353: __E_cli_command_105(222) => 'q'.
	p(NT(222), (T(50)));
//G354: __E_cli_command_105(222) => 'q' 'u' 'i' 't'.
	p(NT(222), (T(50)+T(42)+T(10)+T(13)));
//G355: quit_sym(221)        => __E_cli_command_105(222).
	p(NT(221), (NT(222)));
//G356: quit_cmd(220)        => quit_sym(221).
	p(NT(220), (NT(221)));
//G357: cli_command(217)     => quit_cmd(220).
	p(NT(217), (NT(220)));
//G358: __E_cli_command_106(225) => 'v'.
	p(NT(225), (T(51)));
//G359: __E_cli_command_106(225) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(225), (T(51)+T(19)+T(11)+T(12)+T(10)+T(17)+T(52)));
//G360: version_sym(224)     => __E_cli_command_106(225).
	p(NT(224), (NT(225)));
//G361: version_cmd(223)     => version_sym(224).
	p(NT(223), (NT(224)));
//G362: cli_command(217)     => version_cmd(223).
	p(NT(217), (NT(223)));
//G363: __E_cli_command_107(228) => 'c'.
	p(NT(228), (T(8)));
//G364: __E_cli_command_107(228) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(228), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G365: clear_sym(227)       => __E_cli_command_107(228).
	p(NT(227), (NT(228)));
//G366: clear_cmd(226)       => clear_sym(227).
	p(NT(226), (NT(227)));
//G367: cli_command(217)     => clear_cmd(226).
	p(NT(217), (NT(226)));
//G368: __E___E_cli_command_108_109(232) => 'h'.
	p(NT(232), (T(41)));
//G369: __E___E_cli_command_108_109(232) => 'h' 'e' 'l' 'p'.
	p(NT(232), (T(41)+T(19)+T(6)+T(53)));
//G370: help_sym(231)        => __E___E_cli_command_108_109(232).
	p(NT(231), (NT(232)));
//G371: __E___E_cli_command_108_110(233) => __(27) help_arg(234).
	p(NT(233), (NT(27)+NT(234)));
//G372: __E___E_cli_command_108_110(233) => null.
	p(NT(233), (nul));
//G373: __E_cli_command_108(230) => help_sym(231) __E___E_cli_command_108_110(233).
	p(NT(230), (NT(231)+NT(233)));
//G374: help_cmd(229)        => __E_cli_command_108(230).
	p(NT(229), (NT(230)));
//G375: cli_command(217)     => help_cmd(229).
	p(NT(217), (NT(229)));
//G376: file_sym(237)        => 'f' 'i' 'l' 'e'.
	p(NT(237), (T(4)+T(10)+T(6)+T(19)));
//G377: __E_cli_command_111(236) => file_sym(237) __(27) q_string(238).
	p(NT(236), (NT(237)+NT(27)+NT(238)));
//G378: file_cmd(235)        => __E_cli_command_111(236).
	p(NT(235), (NT(236)));
//G379: cli_command(217)     => file_cmd(235).
	p(NT(217), (NT(235)));
//G380: valid_sym(241)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(241), (T(51)+T(5)+T(6)+T(10)+T(54)));
//G381: __E_cli_command_112(240) => valid_sym(241) __(27) normalize_cmd_arg(242).
	p(NT(240), (NT(241)+NT(27)+NT(242)));
//G382: valid_cmd(239)       => __E_cli_command_112(240).
	p(NT(239), (NT(240)));
//G383: cli_command(217)     => valid_cmd(239).
	p(NT(217), (NT(239)));
//G384: sat_sym(245)         => 's' 'a' 't'.
	p(NT(245), (T(12)+T(5)+T(13)));
//G385: __E_cli_command_113(244) => sat_sym(245) __(27) normalize_cmd_arg(242).
	p(NT(244), (NT(245)+NT(27)+NT(242)));
//G386: sat_cmd(243)         => __E_cli_command_113(244).
	p(NT(243), (NT(244)));
//G387: cli_command(217)     => sat_cmd(243).
	p(NT(217), (NT(243)));
//G388: unsat_sym(248)       => 'u' 'n' 's' 'a' 't'.
	p(NT(248), (T(42)+T(52)+T(12)+T(5)+T(13)));
//G389: __E_cli_command_114(247) => unsat_sym(248) __(27) normalize_cmd_arg(242).
	p(NT(247), (NT(248)+NT(27)+NT(242)));
//G390: unsat_cmd(246)       => __E_cli_command_114(247).
	p(NT(246), (NT(247)));
//G391: cli_command(217)     => unsat_cmd(246).
	p(NT(217), (NT(246)));
//G392: solve_sym(251)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(251), (T(12)+T(17)+T(6)+T(51)+T(19)));
//G393: __E___E_cli_command_115_116(252) => solve_options(253).
	p(NT(252), (NT(253)));
//G394: __E___E_cli_command_115_116(252) => null.
	p(NT(252), (nul));
//G395: __E_cli_command_115(250) => solve_sym(251) __E___E_cli_command_115_116(252) __(27) wff_cmd_arg(254).
	p(NT(250), (NT(251)+NT(252)+NT(27)+NT(254)));
//G396: solve_cmd(249)       => __E_cli_command_115(250).
	p(NT(249), (NT(250)));
//G397: cli_command(217)     => solve_cmd(249).
	p(NT(217), (NT(249)));
//G398: lgrs_sym(257)        => 'l' 'g' 'r' 's'.
	p(NT(257), (T(6)+T(55)+T(11)+T(12)));
//G399: __E_cli_command_117(256) => lgrs_sym(257) __(27) wff_cmd_arg(254).
	p(NT(256), (NT(257)+NT(27)+NT(254)));
//G400: lgrs_cmd(255)        => __E_cli_command_117(256).
	p(NT(255), (NT(256)));
//G401: cli_command(217)     => lgrs_cmd(255).
	p(NT(217), (NT(255)));
//G402: __E___E_cli_command_118_119(261) => 'r'.
	p(NT(261), (T(11)));
//G403: __E___E_cli_command_118_119(261) => 'r' 'u' 'n'.
	p(NT(261), (T(11)+T(42)+T(52)));
//G404: run_sym(260)         => __E___E_cli_command_118_119(261).
	p(NT(260), (NT(261)));
//G405: __E___E_cli_command_118_120(262) => memory(263).
	p(NT(262), (NT(263)));
//G406: __E___E_cli_command_118_120(262) => wff(14).
	p(NT(262), (NT(14)));
//G407: __E_cli_command_118(259) => run_sym(260) __(27) __E___E_cli_command_118_120(262).
	p(NT(259), (NT(260)+NT(27)+NT(262)));
//G408: run_cmd(258)         => __E_cli_command_118(259).
	p(NT(258), (NT(259)));
//G409: cli_command(217)     => run_cmd(258).
	p(NT(217), (NT(258)));
//G410: __E___E_cli_command_121_122(267) => 'n'.
	p(NT(267), (T(52)));
//G411: __E___E_cli_command_121_122(267) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(267), (T(52)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(56)+T(19)));
//G412: normalize_sym(266)   => __E___E_cli_command_121_122(267).
	p(NT(266), (NT(267)));
//G413: __E_cli_command_121(265) => normalize_sym(266) __(27) normalize_cmd_arg(242).
	p(NT(265), (NT(266)+NT(27)+NT(242)));
//G414: normalize_cmd(264)   => __E_cli_command_121(265).
	p(NT(264), (NT(265)));
//G415: cli_command(217)     => normalize_cmd(264).
	p(NT(217), (NT(264)));
//G416: __E___E_cli_command_123_124(271) => 's'.
	p(NT(271), (T(12)));
//G417: __E___E_cli_command_123_124(271) => 's' 'u' 'b' 's' 't'.
	p(NT(271), (T(12)+T(42)+T(7)+T(12)+T(13)));
//G418: __E___E_cli_command_123_124(271) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(271), (T(12)+T(42)+T(7)+T(12)+T(13)+T(10)+T(13)+T(42)+T(13)+T(19)));
//G419: subst_sym(270)       => __E___E_cli_command_123_124(271).
	p(NT(270), (NT(271)));
//G420: __E_cli_command_123(269) => subst_sym(270) __(27) nf_cmd_arg(272) _(11) '[' _(11) nf_cmd_arg(272) _(11) '/' _(11) nf_cmd_arg(272) _(11) ']'.
	p(NT(269), (NT(270)+NT(27)+NT(272)+NT(11)+T(24)+NT(11)+NT(272)+NT(11)+T(57)+NT(11)+NT(272)+NT(11)+T(25)));
//G421: subst_cmd(268)       => __E_cli_command_123(269).
	p(NT(268), (NT(269)));
//G422: cli_command(217)     => subst_cmd(268).
	p(NT(217), (NT(268)));
//G423: __E___E_cli_command_125_126(276) => 'i'.
	p(NT(276), (T(10)));
//G424: __E___E_cli_command_125_126(276) => 'i' 'n' 's' 't'.
	p(NT(276), (T(10)+T(52)+T(12)+T(13)));
//G425: __E___E_cli_command_125_126(276) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(276), (T(10)+T(52)+T(12)+T(13)+T(5)+T(52)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G426: inst_sym(275)        => __E___E_cli_command_125_126(276).
	p(NT(275), (NT(276)));
//G427: __E_cli_command_125(274) => inst_sym(275) __(27) inst_args(277).
	p(NT(274), (NT(275)+NT(27)+NT(277)));
//G428: inst_cmd(273)        => __E_cli_command_125(274).
	p(NT(273), (NT(274)));
//G429: cli_command(217)     => inst_cmd(273).
	p(NT(217), (NT(273)));
//G430: dnf_sym(280)         => 'd' 'n' 'f'.
	p(NT(280), (T(54)+T(52)+T(4)));
//G431: __E_cli_command_127(279) => dnf_sym(280) __(27) nf_cmd_arg(272).
	p(NT(279), (NT(280)+NT(27)+NT(272)));
//G432: dnf_cmd(278)         => __E_cli_command_127(279).
	p(NT(278), (NT(279)));
//G433: cli_command(217)     => dnf_cmd(278).
	p(NT(217), (NT(278)));
//G434: cnf_sym(283)         => 'c' 'n' 'f'.
	p(NT(283), (T(8)+T(52)+T(4)));
//G435: __E_cli_command_128(282) => cnf_sym(283) __(27) nf_cmd_arg(272).
	p(NT(282), (NT(283)+NT(27)+NT(272)));
//G436: cnf_cmd(281)         => __E_cli_command_128(282).
	p(NT(281), (NT(282)));
//G437: cli_command(217)     => cnf_cmd(281).
	p(NT(217), (NT(281)));
//G438: anf_sym(286)         => 'a' 'n' 'f'.
	p(NT(286), (T(5)+T(52)+T(4)));
//G439: __E_cli_command_129(285) => anf_sym(286) __(27) nf_cmd_arg(272).
	p(NT(285), (NT(286)+NT(27)+NT(272)));
//G440: anf_cmd(284)         => __E_cli_command_129(285).
	p(NT(284), (NT(285)));
//G441: cli_command(217)     => anf_cmd(284).
	p(NT(217), (NT(284)));
//G442: nnf_sym(289)         => 'n' 'n' 'f'.
	p(NT(289), (T(52)+T(52)+T(4)));
//G443: __E_cli_command_130(288) => nnf_sym(289) __(27) nf_cmd_arg(272).
	p(NT(288), (NT(289)+NT(27)+NT(272)));
//G444: nnf_cmd(287)         => __E_cli_command_130(288).
	p(NT(287), (NT(288)));
//G445: cli_command(217)     => nnf_cmd(287).
	p(NT(217), (NT(287)));
//G446: pnf_sym(292)         => 'p' 'n' 'f'.
	p(NT(292), (T(53)+T(52)+T(4)));
//G447: __E_cli_command_131(291) => pnf_sym(292) __(27) nf_cmd_arg(272).
	p(NT(291), (NT(292)+NT(27)+NT(272)));
//G448: pnf_cmd(290)         => __E_cli_command_131(291).
	p(NT(290), (NT(291)));
//G449: cli_command(217)     => pnf_cmd(290).
	p(NT(217), (NT(290)));
//G450: mnf_sym(295)         => 'm' 'n' 'f'.
	p(NT(295), (T(18)+T(52)+T(4)));
//G451: __E_cli_command_132(294) => mnf_sym(295) __(27) nf_cmd_arg(272).
	p(NT(294), (NT(295)+NT(27)+NT(272)));
//G452: mnf_cmd(293)         => __E_cli_command_132(294).
	p(NT(293), (NT(294)));
//G453: cli_command(217)     => mnf_cmd(293).
	p(NT(217), (NT(293)));
//G454: snf_sym(298)         => 's' 'n' 'f'.
	p(NT(298), (T(12)+T(52)+T(4)));
//G455: __E_cli_command_133(297) => snf_sym(298) __(27) nf_cmd_arg(272).
	p(NT(297), (NT(298)+NT(27)+NT(272)));
//G456: snf_cmd(296)         => __E_cli_command_133(297).
	p(NT(296), (NT(297)));
//G457: cli_command(217)     => snf_cmd(296).
	p(NT(217), (NT(296)));
//G458: onf_sym(301)         => 'o' 'n' 'f'.
	p(NT(301), (T(17)+T(52)+T(4)));
//G459: __E_cli_command_134(300) => onf_sym(301) __(27) variable(118) __(27) onf_cmd_arg(302).
	p(NT(300), (NT(301)+NT(27)+NT(118)+NT(27)+NT(302)));
//G460: onf_cmd(299)         => __E_cli_command_134(300).
	p(NT(299), (NT(300)));
//G461: cli_command(217)     => onf_cmd(299).
	p(NT(217), (NT(299)));
//G462: __E___E_cli_command_135_136(306) => 'd' 'e' 'f' 's'.
	p(NT(306), (T(54)+T(19)+T(4)+T(12)));
//G463: __E___E_cli_command_135_136(306) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(306), (T(54)+T(19)+T(4)+T(10)+T(52)+T(10)+T(13)+T(10)+T(17)+T(52)+T(12)));
//G464: def_sym(305)         => __E___E_cli_command_135_136(306).
	p(NT(305), (NT(306)));
//G465: __E_cli_command_135(304) => def_sym(305).
	p(NT(304), (NT(305)));
//G466: def_list_cmd(303)    => __E_cli_command_135(304).
	p(NT(303), (NT(304)));
//G467: cli_command(217)     => def_list_cmd(303).
	p(NT(217), (NT(303)));
//G468: __E_cli_command_137(308) => def_sym(305) __(27) number(309).
	p(NT(308), (NT(305)+NT(27)+NT(309)));
//G469: def_print_cmd(307)   => __E_cli_command_137(308).
	p(NT(307), (NT(308)));
//G470: cli_command(217)     => def_print_cmd(307).
	p(NT(217), (NT(307)));
//G471: def_rr_cmd(310)      => rec_relation(16).
	p(NT(310), (NT(16)));
//G472: cli_command(217)     => def_rr_cmd(310).
	p(NT(217), (NT(310)));
//G473: qelim_sym(313)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(313), (T(50)+T(19)+T(6)+T(10)+T(18)));
//G474: __E_cli_command_138(312) => qelim_sym(313) __(27) wff_cmd_arg(254).
	p(NT(312), (NT(313)+NT(27)+NT(254)));
//G475: qelim_cmd(311)       => __E_cli_command_138(312).
	p(NT(311), (NT(312)));
//G476: cli_command(217)     => qelim_cmd(311).
	p(NT(217), (NT(311)));
//G477: get_sym(316)         => 'g' 'e' 't'.
	p(NT(316), (T(55)+T(19)+T(13)));
//G478: __E___E_cli_command_139_140(317) => __(27) option_name(318).
	p(NT(317), (NT(27)+NT(318)));
//G479: __E___E_cli_command_139_140(317) => null.
	p(NT(317), (nul));
//G480: __E_cli_command_139(315) => get_sym(316) __E___E_cli_command_139_140(317).
	p(NT(315), (NT(316)+NT(317)));
//G481: get_cmd(314)         => __E_cli_command_139(315).
	p(NT(314), (NT(315)));
//G482: cli_command(217)     => get_cmd(314).
	p(NT(217), (NT(314)));
//G483: set_sym(321)         => 's' 'e' 't'.
	p(NT(321), (T(12)+T(19)+T(13)));
//G484: __E___E_cli_command_141_142(322) => __(27).
	p(NT(322), (NT(27)));
//G485: __E___E_cli_command_141_142(322) => _(11) '=' _(11).
	p(NT(322), (NT(11)+T(3)+NT(11)));
//G486: __E_cli_command_141(320) => set_sym(321) __(27) option_name(318) __E___E_cli_command_141_142(322) option_value(323).
	p(NT(320), (NT(321)+NT(27)+NT(318)+NT(322)+NT(323)));
//G487: set_cmd(319)         => __E_cli_command_141(320).
	p(NT(319), (NT(320)));
//G488: cli_command(217)     => set_cmd(319).
	p(NT(217), (NT(319)));
//G489: enable_sym(326)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(326), (T(19)+T(52)+T(5)+T(7)+T(6)+T(19)));
//G490: __E_cli_command_143(325) => enable_sym(326) __(27) option_name(318).
	p(NT(325), (NT(326)+NT(27)+NT(318)));
//G491: enable_cmd(324)      => __E_cli_command_143(325).
	p(NT(324), (NT(325)));
//G492: cli_command(217)     => enable_cmd(324).
	p(NT(217), (NT(324)));
//G493: disable_sym(329)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(329), (T(54)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G494: __E_cli_command_144(328) => disable_sym(329) __(27) option_name(318).
	p(NT(328), (NT(329)+NT(27)+NT(318)));
//G495: disable_cmd(327)     => __E_cli_command_144(328).
	p(NT(327), (NT(328)));
//G496: cli_command(217)     => disable_cmd(327).
	p(NT(217), (NT(327)));
//G497: toggle_sym(332)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(332), (T(13)+T(17)+T(55)+T(55)+T(6)+T(19)));
//G498: __E_cli_command_145(331) => toggle_sym(332) __(27) option_name(318).
	p(NT(331), (NT(332)+NT(27)+NT(318)));
//G499: toggle_cmd(330)      => __E_cli_command_145(331).
	p(NT(330), (NT(331)));
//G500: cli_command(217)     => toggle_cmd(330).
	p(NT(217), (NT(330)));
//G501: __E_cli_command_146(334) => type(129) __(27) in_var_name(170) _(11) '=' _(11) input_stream(335).
	p(NT(334), (NT(129)+NT(27)+NT(170)+NT(11)+T(3)+NT(11)+NT(335)));
//G502: def_input_cmd(333)   => __E_cli_command_146(334).
	p(NT(333), (NT(334)));
//G503: cli_command(217)     => def_input_cmd(333).
	p(NT(217), (NT(333)));
//G504: __E_cli_command_147(337) => type(129) __(27) out_var_name(175) _(11) '=' _(11) output_stream(338).
	p(NT(337), (NT(129)+NT(27)+NT(175)+NT(11)+T(3)+NT(11)+NT(338)));
//G505: def_output_cmd(336)  => __E_cli_command_147(337).
	p(NT(336), (NT(337)));
//G506: cli_command(217)     => def_output_cmd(336).
	p(NT(217), (NT(336)));
//G507: __E___E_cli_command_148_149(342) => 'h' 'i' 's' 't'.
	p(NT(342), (T(41)+T(10)+T(12)+T(13)));
//G508: __E___E_cli_command_148_149(342) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(342), (T(41)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G509: history_sym(341)     => __E___E_cli_command_148_149(342).
	p(NT(341), (NT(342)));
//G510: __E_cli_command_148(340) => history_sym(341).
	p(NT(340), (NT(341)));
//G511: history_list_cmd(339) => __E_cli_command_148(340).
	p(NT(339), (NT(340)));
//G512: cli_command(217)     => history_list_cmd(339).
	p(NT(217), (NT(339)));
//G513: __E_cli_command_150(344) => history_sym(341) __(27) memory(263).
	p(NT(344), (NT(341)+NT(27)+NT(263)));
//G514: history_print_cmd(343) => __E_cli_command_150(344).
	p(NT(343), (NT(344)));
//G515: cli_command(217)     => history_print_cmd(343).
	p(NT(217), (NT(343)));
//G516: __E_cli_command_151(346) => wff(14).
	p(NT(346), (NT(14)));
//G517: __E_cli_command_151(346) => bf(21).
	p(NT(346), (NT(21)));
//G518: history_store_cmd(345) => __E_cli_command_151(346).
	p(NT(345), (NT(346)));
//G519: cli_command(217)     => history_store_cmd(345).
	p(NT(217), (NT(345)));
//G520: __E_solve_options_152(347) => __(27) solve_option(348).
	p(NT(347), (NT(27)+NT(348)));
//G521: __E_solve_options_153(349) => null.
	p(NT(349), (nul));
//G522: __E_solve_options_153(349) => __E_solve_options_152(347) __E_solve_options_153(349).
	p(NT(349), (NT(347)+NT(349)));
//G523: solve_options(253)   => __E_solve_options_153(349).
	p(NT(253), (NT(349)));
//G524: __E_solve_option_154(350) => '-' '-' solver_mode(351).
	p(NT(350), (T(28)+T(28)+NT(351)));
//G525: solve_option(348)    => __E_solve_option_154(350).
	p(NT(348), (NT(350)));
//G526: __E_solve_option_155(352) => '-' '-' type(129).
	p(NT(352), (T(28)+T(28)+NT(129)));
//G527: solve_option(348)    => __E_solve_option_155(352).
	p(NT(348), (NT(352)));
//G528: __E_solver_mode_156(354) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(354), (T(18)+T(10)+T(52)+T(10)+T(18)+T(42)+T(18)));
//G529: __E_solver_mode_156(354) => 'm' 'i' 'n'.
	p(NT(354), (T(18)+T(10)+T(52)));
//G530: solver_mode_minimum(353) => __E_solver_mode_156(354).
	p(NT(353), (NT(354)));
//G531: solver_mode(351)     => solver_mode_minimum(353).
	p(NT(351), (NT(353)));
//G532: __E_solver_mode_157(356) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(356), (T(18)+T(5)+T(27)+T(10)+T(18)+T(42)+T(18)));
//G533: __E_solver_mode_157(356) => 'm' 'a' 'x'.
	p(NT(356), (T(18)+T(5)+T(27)));
//G534: solver_mode_maximum(355) => __E_solver_mode_157(356).
	p(NT(355), (NT(356)));
//G535: solver_mode(351)     => solver_mode_maximum(355).
	p(NT(351), (NT(355)));
//G536: __E_input_stream_158(357) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(358) _(11) ')'.
	p(NT(357), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(358)+NT(11)+T(16)));
//G537: input_stream(335)    => __E_input_stream_158(357).
	p(NT(335), (NT(357)));
//G538: console_sym(359)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(359), (T(8)+T(17)+T(52)+T(12)+T(17)+T(6)+T(19)));
//G539: input_stream(335)    => console_sym(359).
	p(NT(335), (NT(359)));
//G540: __E_output_stream_159(360) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(358) _(11) ')'.
	p(NT(360), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(358)+NT(11)+T(16)));
//G541: output_stream(338)   => __E_output_stream_159(360).
	p(NT(338), (NT(360)));
//G542: output_stream(338)   => console_sym(359).
	p(NT(338), (NT(359)));
//G543: q_file_name(358)     => '"' file_name(361) '"'.
	p(NT(358), (T(58)+NT(361)+T(58)));
//G544: __E_file_name_160(362) => printable(8).
	p(NT(362), (NT(8)));
//G545: __E_file_name_160(362) => printable(8) __E_file_name_160(362).
	p(NT(362), (NT(8)+NT(362)));
//G546: file_name(361)       => __E_file_name_160(362).
	p(NT(361), (NT(362)));
//G547: __E_option_name_161(363) => alnum(6).
	p(NT(363), (NT(6)));
//G548: __E_option_name_161(363) => alnum(6) __E_option_name_161(363).
	p(NT(363), (NT(6)+NT(363)));
//G549: option_name(318)     => __E_option_name_161(363).
	p(NT(318), (NT(363)));
//G550: __E_option_value_162(364) => alnum(6).
	p(NT(364), (NT(6)));
//G551: __E_option_value_162(364) => alnum(6) __E_option_value_162(364).
	p(NT(364), (NT(6)+NT(364)));
//G552: option_value(323)    => __E_option_value_162(364).
	p(NT(323), (NT(364)));
//G553: number(309)          => digits(190).
	p(NT(309), (NT(190)));
//G554: bf_cmd_arg(365)      => memory(263).
	p(NT(365), (NT(263)));
//G555: bf_cmd_arg(365)      => bf(21).
	p(NT(365), (NT(21)));
//G556: wff_cmd_arg(254)     => memory(263).
	p(NT(254), (NT(263)));
//G557: wff_cmd_arg(254)     => wff(14).
	p(NT(254), (NT(14)));
//G558: nf_cmd_arg(272)      => memory(263).
	p(NT(272), (NT(263)));
//G559: nf_cmd_arg(272)      => ref(18).
	p(NT(272), (NT(18)));
//G560: nf_cmd_arg(272)      => wff(14).
	p(NT(272), (NT(14)));
//G561: nf_cmd_arg(272)      => bf(21).
	p(NT(272), (NT(21)));
//G562: onf_cmd_arg(302)     => memory(263).
	p(NT(302), (NT(263)));
//G563: onf_cmd_arg(302)     => wff(14).
	p(NT(302), (NT(14)));
//G564: normalize_cmd_arg(242) => memory(263).
	p(NT(242), (NT(263)));
//G565: normalize_cmd_arg(242) => rr(10).
	p(NT(242), (NT(10)));
//G566: normalize_cmd_arg(242) => ref(18).
	p(NT(242), (NT(18)));
//G567: normalize_cmd_arg(242) => wff(14).
	p(NT(242), (NT(14)));
//G568: normalize_cmd_arg(242) => bf(21).
	p(NT(242), (NT(21)));
//G569: inst_args(277)       => wff_cmd_arg(254) _(11) '[' _(11) variable(118) _(11) '/' _(11) bf_cmd_arg(365) _(11) ']'.
	p(NT(277), (NT(254)+NT(11)+T(24)+NT(11)+NT(118)+NT(11)+T(57)+NT(11)+NT(365)+NT(11)+T(25)));
//G570: inst_args(277)       => bf_cmd_arg(365) _(11) '[' _(11) variable(118) _(11) '/' _(11) bf_cmd_arg(365) _(11) ']'.
	p(NT(277), (NT(365)+NT(11)+T(24)+NT(11)+NT(118)+NT(11)+T(57)+NT(11)+NT(365)+NT(11)+T(25)));
//G571: help_arg(234)        => help_sym(231).
	p(NT(234), (NT(231)));
//G572: help_arg(234)        => version_sym(224).
	p(NT(234), (NT(224)));
//G573: help_arg(234)        => quit_sym(221).
	p(NT(234), (NT(221)));
//G574: help_arg(234)        => clear_sym(227).
	p(NT(234), (NT(227)));
//G575: help_arg(234)        => get_sym(316).
	p(NT(234), (NT(316)));
//G576: help_arg(234)        => set_sym(321).
	p(NT(234), (NT(321)));
//G577: help_arg(234)        => enable_sym(326).
	p(NT(234), (NT(326)));
//G578: help_arg(234)        => disable_sym(329).
	p(NT(234), (NT(329)));
//G579: help_arg(234)        => toggle_sym(332).
	p(NT(234), (NT(332)));
//G580: help_arg(234)        => file_sym(237).
	p(NT(234), (NT(237)));
//G581: help_arg(234)        => history_sym(341).
	p(NT(234), (NT(341)));
//G582: help_arg(234)        => abs_memory_sym(366).
	p(NT(234), (NT(366)));
//G583: help_arg(234)        => rel_memory_sym(367).
	p(NT(234), (NT(367)));
//G584: help_arg(234)        => selection_sym(368).
	p(NT(234), (NT(368)));
//G585: help_arg(234)        => def_sym(305).
	p(NT(234), (NT(305)));
//G586: help_arg(234)        => inst_sym(275).
	p(NT(234), (NT(275)));
//G587: help_arg(234)        => subst_sym(270).
	p(NT(234), (NT(270)));
//G588: help_arg(234)        => normalize_sym(266).
	p(NT(234), (NT(266)));
//G589: help_arg(234)        => execute_sym(369).
	p(NT(234), (NT(369)));
//G590: help_arg(234)        => solve_sym(251).
	p(NT(234), (NT(251)));
//G591: help_arg(234)        => valid_sym(241).
	p(NT(234), (NT(241)));
//G592: help_arg(234)        => sat_sym(245).
	p(NT(234), (NT(245)));
//G593: help_arg(234)        => unsat_sym(248).
	p(NT(234), (NT(248)));
//G594: help_arg(234)        => run_sym(260).
	p(NT(234), (NT(260)));
//G595: help_arg(234)        => dnf_sym(280).
	p(NT(234), (NT(280)));
//G596: help_arg(234)        => cnf_sym(283).
	p(NT(234), (NT(283)));
//G597: help_arg(234)        => anf_sym(286).
	p(NT(234), (NT(286)));
//G598: help_arg(234)        => snf_sym(298).
	p(NT(234), (NT(298)));
//G599: help_arg(234)        => nnf_sym(289).
	p(NT(234), (NT(289)));
//G600: help_arg(234)        => pnf_sym(292).
	p(NT(234), (NT(292)));
//G601: help_arg(234)        => mnf_sym(295).
	p(NT(234), (NT(295)));
//G602: help_arg(234)        => onf_sym(301).
	p(NT(234), (NT(301)));
//G603: help_arg(234)        => qelim_sym(313).
	p(NT(234), (NT(313)));
//G604: __E___E_help_arg_163_164(372) => 's'.
	p(NT(372), (T(12)));
//G605: __E___E_help_arg_163_164(372) => null.
	p(NT(372), (nul));
//G606: __E_help_arg_163(371) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_163_164(372).
	p(NT(371), (T(19)+T(27)+T(5)+T(18)+T(53)+T(6)+T(19)+NT(372)));
//G607: examples_sym(370)    => __E_help_arg_163(371).
	p(NT(370), (NT(371)));
//G608: help_arg(234)        => examples_sym(370).
	p(NT(234), (NT(370)));
//G609: __E_memory_165(374)  => '%'.
	p(NT(374), (T(59)));
//G610: rel_memory_sym(367)  => '%' '-'.
	p(NT(367), (T(59)+T(28)));
//G611: memory_id(376)       => digits(190).
	p(NT(376), (NT(190)));
//G612: __E___E_memory_165_166(375) => memory_id(376).
	p(NT(375), (NT(376)));
//G613: __E___E_memory_165_166(375) => null.
	p(NT(375), (nul));
//G614: __E_memory_165(374)  => rel_memory_sym(367) __E___E_memory_165_166(375).
	p(NT(374), (NT(367)+NT(375)));
//G615: rel_memory(373)      => __E_memory_165(374).
	p(NT(373), (NT(374)));
//G616: memory(263)          => rel_memory(373).
	p(NT(263), (NT(373)));
//G617: abs_memory_sym(366)  => '%'.
	p(NT(366), (T(59)));
//G618: __E_memory_167(378)  => abs_memory_sym(366) memory_id(376).
	p(NT(378), (NT(366)+NT(376)));
//G619: abs_memory(377)      => __E_memory_167(378).
	p(NT(377), (NT(378)));
//G620: memory(263)          => abs_memory(377).
	p(NT(263), (NT(377)));
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
		in_var_name, __E_in_71, __E___E_in_71_72, __E___E___E_in_71_72_73, offset, out_var_name, __E_out_74, __E___E_out_74_75, __E___E___E_out_74_75_76, chars, 
		q_var, __E_q_vars_77, __E_q_vars_78, __E_offsets_79, __E_offsets_80, integer, shift, __E_offset_81, __E_shift_82, __E___E_shift_82_83, 
		digits, __E_integer_84, uninter_const_name, __E_uninterpreted_constant_85, __E___E_uninterpreted_constant_85_86, constant, __E_bf_constant_87, binding, __E_bf_constant_88, source, 
		__E_binding_89, src_c, __E___E_binding_89_90, __E___E___E_binding_89_90_91, __E___E___E_binding_89_90_92, __E_src_c_93, __E_src_c_94, __E_chars_95, __E_chars_96, __E_digits_97, 
		comment, __E_comment_98, __E_comment_99, __E_comment_100, __E____101, __E___102, cli, cli_command, __E_cli_103, __E_cli_104, 
		quit_cmd, quit_sym, __E_cli_command_105, version_cmd, version_sym, __E_cli_command_106, clear_cmd, clear_sym, __E_cli_command_107, help_cmd, 
		__E_cli_command_108, help_sym, __E___E_cli_command_108_109, __E___E_cli_command_108_110, help_arg, file_cmd, __E_cli_command_111, file_sym, q_string, valid_cmd, 
		__E_cli_command_112, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_113, sat_sym, unsat_cmd, __E_cli_command_114, unsat_sym, solve_cmd, 
		__E_cli_command_115, solve_sym, __E___E_cli_command_115_116, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_117, lgrs_sym, run_cmd, __E_cli_command_118, 
		run_sym, __E___E_cli_command_118_119, __E___E_cli_command_118_120, memory, normalize_cmd, __E_cli_command_121, normalize_sym, __E___E_cli_command_121_122, subst_cmd, __E_cli_command_123, 
		subst_sym, __E___E_cli_command_123_124, nf_cmd_arg, inst_cmd, __E_cli_command_125, inst_sym, __E___E_cli_command_125_126, inst_args, dnf_cmd, __E_cli_command_127, 
		dnf_sym, cnf_cmd, __E_cli_command_128, cnf_sym, anf_cmd, __E_cli_command_129, anf_sym, nnf_cmd, __E_cli_command_130, nnf_sym, 
		pnf_cmd, __E_cli_command_131, pnf_sym, mnf_cmd, __E_cli_command_132, mnf_sym, snf_cmd, __E_cli_command_133, snf_sym, onf_cmd, 
		__E_cli_command_134, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_135, def_sym, __E___E_cli_command_135_136, def_print_cmd, __E_cli_command_137, number, 
		def_rr_cmd, qelim_cmd, __E_cli_command_138, qelim_sym, get_cmd, __E_cli_command_139, get_sym, __E___E_cli_command_139_140, option_name, set_cmd, 
		__E_cli_command_141, set_sym, __E___E_cli_command_141_142, option_value, enable_cmd, __E_cli_command_143, enable_sym, disable_cmd, __E_cli_command_144, disable_sym, 
		toggle_cmd, __E_cli_command_145, toggle_sym, def_input_cmd, __E_cli_command_146, input_stream, def_output_cmd, __E_cli_command_147, output_stream, history_list_cmd, 
		__E_cli_command_148, history_sym, __E___E_cli_command_148_149, history_print_cmd, __E_cli_command_150, history_store_cmd, __E_cli_command_151, __E_solve_options_152, solve_option, __E_solve_options_153, 
		__E_solve_option_154, solver_mode, __E_solve_option_155, solver_mode_minimum, __E_solver_mode_156, solver_mode_maximum, __E_solver_mode_157, __E_input_stream_158, q_file_name, console_sym, 
		__E_output_stream_159, file_name, __E_file_name_160, __E_option_name_161, __E_option_value_162, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, execute_sym, 
		examples_sym, __E_help_arg_163, __E___E_help_arg_163_164, rel_memory, __E_memory_165, __E___E_memory_165_166, memory_id, abs_memory, __E_memory_167, named_binding, 
		__N_0, __N_1, __N_2, __N_3, __N_4, __N_5, __N_6, __N_7, __N_8, 
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
