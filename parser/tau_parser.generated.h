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
	"__E_wff_41", "bf_less", "__E_wff_42", "bf_nless", "__E_wff_43", "bf_parenthesis", "__E_bf_44", "bf_constant", "bf_variable", "bf_splitter", 
	"__E_bf_45", "bf_ref", "bf_or", "__E_bf_46", "bf_xor", "__E_bf_47", "bf_t", "__E_bf_48", "__E___E_bf_48_49", "type", 
	"bf_f", "__E_bf_50", "__E___E_bf_50_51", "bf_and", "__E_bf_52", "__E___E_bf_52_53", "bf_neg", "__E_bf_54", "bf_neg_oprnd", "__E___E_bf_54_55", 
	"bf_and_nosep", "__E_bf_56", "bf_and_nosep_1st_oprnd", "__E___E_bf_56_57", "bf_and_nosep_2nd_oprnd", "__E___E_bf_56_58", "ctn_neq", "__E_constraint_59", "ctnvar", "num", 
	"ctn_eq", "__E_constraint_60", "ctn_greater_equal", "__E_constraint_61", "ctn_greater", "__E_constraint_62", "ctn_less_equal", "__E_constraint_63", "ctn_less", "__E_constraint_64", 
	"variable", "__E_bf_variable_65", "chars", "__E_variable_66", "__E_variable_67", "__E_variable_68", "__E_variable_69", "__E_variable_70", "io_var", "__E_variable_71", 
	"in", "out", "uninterpreted_constant", "in_var_name", "__E_in_72", "__E___E_in_72_73", "offset", "out_var_name", "__E_out_74", "__E___E_out_74_75", 
	"__E___E___E_out_74_75_76", "q_var", "__E_q_vars_77", "__E_q_vars_78", "scope_id", "digits", "__E_offsets_79", "__E_offsets_80", "integer", "shift", 
	"__E_offset_81", "__E_shift_82", "__E___E_shift_82_83", "__E_integer_84", "uninter_const_name", "__E_uninterpreted_constant_85", "__E___E_uninterpreted_constant_85_86", "constant", "__E_bf_constant_87", "binding", 
	"__E_bf_constant_88", "source", "__E_binding_89", "src_c", "__E___E_binding_89_90", "__E___E___E_binding_89_90_91", "__E___E___E_binding_89_90_92", "__E_src_c_93", "__E_src_c_94", "__E_chars_95", 
	"__E_chars_96", "__E_digits_97", "comment", "__E_comment_98", "__E_comment_99", "__E_comment_100", "__E____101", "__E___102", "cli", "cli_command", 
	"__E_cli_103", "__E_cli_104", "quit_cmd", "quit_sym", "__E_cli_command_105", "version_cmd", "version_sym", "__E_cli_command_106", "clear_cmd", "clear_sym", 
	"__E_cli_command_107", "help_cmd", "__E_cli_command_108", "help_sym", "__E___E_cli_command_108_109", "__E___E_cli_command_108_110", "help_arg", "file_cmd", "__E_cli_command_111", "file_sym", 
	"q_string", "valid_cmd", "__E_cli_command_112", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_113", "sat_sym", "unsat_cmd", "__E_cli_command_114", 
	"unsat_sym", "solve_cmd", "__E_cli_command_115", "solve_sym", "__E___E_cli_command_115_116", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_117", "lgrs_sym", 
	"run_cmd", "__E_cli_command_118", "run_sym", "__E___E_cli_command_118_119", "__E___E_cli_command_118_120", "memory", "normalize_cmd", "__E_cli_command_121", "normalize_sym", "__E___E_cli_command_121_122", 
	"subst_cmd", "__E_cli_command_123", "subst_sym", "__E___E_cli_command_123_124", "nf_cmd_arg", "inst_cmd", "__E_cli_command_125", "inst_sym", "__E___E_cli_command_125_126", "inst_args", 
	"dnf_cmd", "__E_cli_command_127", "dnf_sym", "cnf_cmd", "__E_cli_command_128", "cnf_sym", "anf_cmd", "__E_cli_command_129", "anf_sym", "nnf_cmd", 
	"__E_cli_command_130", "nnf_sym", "pnf_cmd", "__E_cli_command_131", "pnf_sym", "mnf_cmd", "__E_cli_command_132", "mnf_sym", "snf_cmd", "__E_cli_command_133", 
	"snf_sym", "onf_cmd", "__E_cli_command_134", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_135", "def_sym", "__E___E_cli_command_135_136", "def_print_cmd", 
	"__E_cli_command_137", "number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_138", "qelim_sym", "get_cmd", "__E_cli_command_139", "get_sym", "__E___E_cli_command_139_140", 
	"option_name", "set_cmd", "__E_cli_command_141", "set_sym", "__E___E_cli_command_141_142", "option_value", "enable_cmd", "__E_cli_command_143", "enable_sym", "disable_cmd", 
	"__E_cli_command_144", "disable_sym", "toggle_cmd", "__E_cli_command_145", "toggle_sym", "def_input_cmd", "__E_cli_command_146", "input_stream", "def_output_cmd", "__E_cli_command_147", 
	"output_stream", "history_list_cmd", "__E_cli_command_148", "history_sym", "__E___E_cli_command_148_149", "history_print_cmd", "__E_cli_command_150", "history_store_cmd", "__E_cli_command_151", "__E_solve_options_152", 
	"solve_option", "__E_solve_options_153", "__E_solve_option_154", "solver_mode", "__E_solve_option_155", "solver_mode_minimum", "__E_solver_mode_156", "solver_mode_maximum", "__E_solver_mode_157", "__E_input_stream_158", 
	"q_file_name", "console_sym", "__E_output_stream_159", "file_name", "__E_file_name_160", "__E_option_name_161", "__E_option_value_162", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", 
	"selection_sym", "execute_sym", "examples_sym", "__E_help_arg_163", "__E___E_help_arg_163_164", "rel_memory", "__E_memory_165", "__E___E_memory_165_166", "memory_id", "abs_memory", 
	"__E_memory_167", "named_binding", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", "__N_7", 
	"__N_8", 
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
			20, 22, 129, 148, 160, 173, 177, 185, 188, 194,
			201, 320, 325, 363, 381
		},
		.to_inline = {
			{ 14, 60, 14 },
			{ 21, 115, 21 },
			{ 138, 115, 21 },
			{ 140, 115, 133 },
			{ 142, 115, 21 },
			{ 144, 115, 21 },
			{ 162 },
			{ 181 },
			{ 203 },
			{ 244 },
			{ 256 },
			{ 274 },
			{ 279 },
			{ 304 },
			{ 367 }
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
//G150: bf(21)               => bf_variable(118).
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
//G179: __E___E_bf_54_55(139) => bf_variable(118).
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
//G191: __E___E_bf_56_57(143) => bf_variable(118).
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
//G200: __E___E_bf_56_58(145) => bf_variable(118).
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
//G234: type(129)            => chars(162).
	p(NT(129), (NT(162)));
//G235: __E_bf_variable_65(161) => _(11) ':' _(11) type(129).
	p(NT(161), (NT(11)+T(2)+NT(11)+NT(129)));
//G236: __E_bf_variable_65(161) => null.
	p(NT(161), (nul));
//G237: bf_variable(118)     => variable(160) __E_bf_variable_65(161).
	p(NT(118), (NT(160)+NT(161)));
//G238: __N_0(382)           => 'F'.
	p(NT(382), (T(34)));
//G239: __N_1(383)           => 'T'.
	p(NT(383), (T(33)));
//G240: __E_variable_66(163) => ~( __N_0(382) ) & ~( __N_1(383) ) & alpha(5).	 # conjunctive
	p(NT(163), ~(NT(382)) & ~(NT(383)) & (NT(5)));
//G241: __E_variable_67(164) => null.
	p(NT(164), (nul));
//G242: __E_variable_67(164) => digit(3) __E_variable_67(164).
	p(NT(164), (NT(3)+NT(164)));
//G243: variable(160)        => __E_variable_66(163) __E_variable_67(164).	 # guarded: charvar
	p(NT(160), (NT(163)+NT(164)));
	p.back().guard = "charvar";
//G244: __N_2(384)           => 'F'.
	p(NT(384), (T(34)));
//G245: __N_3(385)           => 'T'.
	p(NT(385), (T(33)));
//G246: __E_variable_68(165) => ~( __N_2(384) ) & ~( __N_3(385) ) & alpha(5).	 # conjunctive
	p(NT(165), ~(NT(384)) & ~(NT(385)) & (NT(5)));
//G247: __E_variable_69(166) => alnum(6).
	p(NT(166), (NT(6)));
//G248: __E_variable_69(166) => '_'.
	p(NT(166), (T(40)));
//G249: __E_variable_70(167) => null.
	p(NT(167), (nul));
//G250: __E_variable_70(167) => __E_variable_69(166) __E_variable_70(167).
	p(NT(167), (NT(166)+NT(167)));
//G251: variable(160)        => __E_variable_68(165) __E_variable_70(167).	 # guarded: var
	p(NT(160), (NT(165)+NT(167)));
	p.back().guard = "var";
//G252: __E_variable_71(169) => in(170).
	p(NT(169), (NT(170)));
//G253: __E_variable_71(169) => out(171).
	p(NT(169), (NT(171)));
//G254: io_var(168)          => __E_variable_71(169).
	p(NT(168), (NT(169)));
//G255: variable(160)        => io_var(168).
	p(NT(160), (NT(168)));
//G256: variable(160)        => uninterpreted_constant(172).
	p(NT(160), (NT(172)));
//G257: __E___E_in_72_73(175) => digit(3).
	p(NT(175), (NT(3)));
//G258: __E___E_in_72_73(175) => digit(3) __E___E_in_72_73(175).
	p(NT(175), (NT(3)+NT(175)));
//G259: __E_in_72(174)       => 'i' __E___E_in_72_73(175).
	p(NT(174), (T(10)+NT(175)));
//G260: in_var_name(173)     => __E_in_72(174).
	p(NT(173), (NT(174)));
//G261: in(170)              => in_var_name(173) '[' offset(176) ']'.
	p(NT(170), (NT(173)+T(24)+NT(176)+T(25)));
//G262: __E___E___E_out_74_75_76(180) => digit(3).
	p(NT(180), (NT(3)));
//G263: __E___E___E_out_74_75_76(180) => digit(3) __E___E___E_out_74_75_76(180).
	p(NT(180), (NT(3)+NT(180)));
//G264: __E___E_out_74_75(179) => 'o' __E___E___E_out_74_75_76(180).
	p(NT(179), (T(17)+NT(180)));
//G265: __E_out_74(178)      => __E___E_out_74_75(179).
	p(NT(178), (NT(179)));
//G266: __E_out_74(178)      => 'u'.
	p(NT(178), (T(41)));
//G267: out_var_name(177)    => __E_out_74(178).
	p(NT(177), (NT(178)));
//G268: out(171)             => out_var_name(177) '[' offset(176) ']'.
	p(NT(171), (NT(177)+T(24)+NT(176)+T(25)));
//G269: ctnvar(148)          => chars(162).
	p(NT(148), (NT(162)));
//G270: capture(20)          => '$' chars(162).
	p(NT(20), (T(42)+NT(162)));
//G271: q_var(181)           => capture(20).
	p(NT(181), (NT(20)));
//G272: __N_4(386)           => uninterpreted_constant(172).
	p(NT(386), (NT(172)));
//G273: q_var(181)           => bf_variable(118) & ~( __N_4(386) ).	 # conjunctive
	p(NT(181), (NT(118)) & ~(NT(386)));
//G274: __E_q_vars_77(182)   => _(11) ',' _(11) q_var(181).
	p(NT(182), (NT(11)+T(14)+NT(11)+NT(181)));
//G275: __E_q_vars_78(183)   => null.
	p(NT(183), (nul));
//G276: __E_q_vars_78(183)   => __E_q_vars_77(182) __E_q_vars_78(183).
	p(NT(183), (NT(182)+NT(183)));
//G277: q_vars(72)           => q_var(181) __E_q_vars_78(183).
	p(NT(72), (NT(181)+NT(183)));
//G278: scope_id(184)        => digits(185).
	p(NT(184), (NT(185)));
//G279: __E_offsets_79(186)  => _(11) ',' _(11) offset(176).
	p(NT(186), (NT(11)+T(14)+NT(11)+NT(176)));
//G280: __E_offsets_80(187)  => null.
	p(NT(187), (nul));
//G281: __E_offsets_80(187)  => __E_offsets_79(186) __E_offsets_80(187).
	p(NT(187), (NT(186)+NT(187)));
//G282: offsets(24)          => '[' _(11) offset(176) __E_offsets_80(187) _(11) ']'.
	p(NT(24), (T(24)+NT(11)+NT(176)+NT(187)+NT(11)+T(25)));
//G283: offset(176)          => integer(188).
	p(NT(176), (NT(188)));
//G284: offset(176)          => capture(20).
	p(NT(176), (NT(20)));
//G285: offset(176)          => shift(189).
	p(NT(176), (NT(189)));
//G286: __N_5(387)           => io_var(168).
	p(NT(387), (NT(168)));
//G287: __E_offset_81(190)   => bf_variable(118) & ~( __N_5(387) ).	 # conjunctive
	p(NT(190), (NT(118)) & ~(NT(387)));
//G288: offset(176)          => __E_offset_81(190).
	p(NT(176), (NT(190)));
//G289: __E_shift_82(191)    => capture(20).
	p(NT(191), (NT(20)));
//G290: __N_6(388)           => io_var(168).
	p(NT(388), (NT(168)));
//G291: __E___E_shift_82_83(192) => bf_variable(118) & ~( __N_6(388) ).	 # conjunctive
	p(NT(192), (NT(118)) & ~(NT(388)));
//G292: __E_shift_82(191)    => __E___E_shift_82_83(192).
	p(NT(191), (NT(192)));
//G293: shift(189)           => __E_shift_82(191) _(11) '-' _(11) num(149).
	p(NT(189), (NT(191)+NT(11)+T(28)+NT(11)+NT(149)));
//G294: num(149)             => digits(185).
	p(NT(149), (NT(185)));
//G295: __E_integer_84(193)  => '-'.
	p(NT(193), (T(28)));
//G296: __E_integer_84(193)  => null.
	p(NT(193), (nul));
//G297: integer(188)         => __E_integer_84(193) _(11) digits(185).
	p(NT(188), (NT(193)+NT(11)+NT(185)));
//G298: __E___E_uninterpreted_constant_85_86(196) => chars(162).
	p(NT(196), (NT(162)));
//G299: __E___E_uninterpreted_constant_85_86(196) => _(11).
	p(NT(196), (NT(11)));
//G300: __E_uninterpreted_constant_85(195) => __E___E_uninterpreted_constant_85_86(196) ':' chars(162).
	p(NT(195), (NT(196)+T(2)+NT(162)));
//G301: uninter_const_name(194) => __E_uninterpreted_constant_85(195).
	p(NT(194), (NT(195)));
//G302: uninterpreted_constant(172) => '<' uninter_const_name(194) _(11) '>'.
	p(NT(172), (T(20)+NT(194)+NT(11)+T(21)));
//G303: __E_bf_constant_87(198) => capture(20).
	p(NT(198), (NT(20)));
//G304: __E_bf_constant_87(198) => binding(199).
	p(NT(198), (NT(199)));
//G305: constant(197)        => __E_bf_constant_87(198).
	p(NT(197), (NT(198)));
//G306: __E_bf_constant_88(200) => _(11) ':' _(11) type(129).
	p(NT(200), (NT(11)+T(2)+NT(11)+NT(129)));
//G307: __E_bf_constant_88(200) => null.
	p(NT(200), (nul));
//G308: bf_constant(117)     => '{' _(11) constant(197) _(11) '}' __E_bf_constant_88(200).
	p(NT(117), (T(43)+NT(11)+NT(197)+NT(11)+T(44)+NT(200)));
//G309: __E___E___E_binding_89_90_91(205) => src_c(203).
	p(NT(205), (NT(203)));
//G310: __E___E___E_binding_89_90_91(205) => space(2).
	p(NT(205), (NT(2)));
//G311: __E___E___E_binding_89_90_92(206) => null.
	p(NT(206), (nul));
//G312: __E___E___E_binding_89_90_92(206) => __E___E___E_binding_89_90_91(205) __E___E___E_binding_89_90_92(206).
	p(NT(206), (NT(205)+NT(206)));
//G313: __E___E_binding_89_90(204) => __E___E___E_binding_89_90_92(206) src_c(203).
	p(NT(204), (NT(206)+NT(203)));
//G314: __E___E_binding_89_90(204) => null.
	p(NT(204), (nul));
//G315: __E_binding_89(202)  => src_c(203) __E___E_binding_89_90(204).
	p(NT(202), (NT(203)+NT(204)));
//G316: source(201)          => __E_binding_89(202).
	p(NT(201), (NT(202)));
//G317: binding(199)         => source(201).
	p(NT(199), (NT(201)));
//G318: src_c(203)           => alnum(6).
	p(NT(203), (NT(6)));
//G319: __N_7(389)           => '{'.
	p(NT(389), (T(43)));
//G320: __N_8(390)           => '}'.
	p(NT(390), (T(44)));
//G321: src_c(203)           => ~( __N_7(389) ) & ~( __N_8(390) ) & punct(7).	 # conjunctive
	p(NT(203), ~(NT(389)) & ~(NT(390)) & (NT(7)));
//G322: __E_src_c_93(207)    => src_c(203).
	p(NT(207), (NT(203)));
//G323: __E_src_c_93(207)    => space(2).
	p(NT(207), (NT(2)));
//G324: __E_src_c_94(208)    => null.
	p(NT(208), (nul));
//G325: __E_src_c_94(208)    => __E_src_c_93(207) __E_src_c_94(208).
	p(NT(208), (NT(207)+NT(208)));
//G326: src_c(203)           => '{' __E_src_c_94(208) '}'.
	p(NT(203), (T(43)+NT(208)+T(44)));
//G327: __E_chars_95(209)    => alnum(6).
	p(NT(209), (NT(6)));
//G328: __E_chars_95(209)    => '_'.
	p(NT(209), (T(40)));
//G329: __E_chars_96(210)    => null.
	p(NT(210), (nul));
//G330: __E_chars_96(210)    => __E_chars_95(209) __E_chars_96(210).
	p(NT(210), (NT(209)+NT(210)));
//G331: chars(162)           => alpha(5) __E_chars_96(210).
	p(NT(162), (NT(5)+NT(210)));
//G332: __E_digits_97(211)   => digit(3).
	p(NT(211), (NT(3)));
//G333: __E_digits_97(211)   => digit(3) __E_digits_97(211).
	p(NT(211), (NT(3)+NT(211)));
//G334: digits(185)          => __E_digits_97(211).
	p(NT(185), (NT(211)));
//G335: sym(22)              => chars(162).
	p(NT(22), (NT(162)));
//G336: __E_comment_98(213)  => printable(8).
	p(NT(213), (NT(8)));
//G337: __E_comment_98(213)  => '\t'.
	p(NT(213), (T(45)));
//G338: __E_comment_99(214)  => null.
	p(NT(214), (nul));
//G339: __E_comment_99(214)  => __E_comment_98(213) __E_comment_99(214).
	p(NT(214), (NT(213)+NT(214)));
//G340: __E_comment_100(215) => '\n'.
	p(NT(215), (T(46)));
//G341: __E_comment_100(215) => '\r'.
	p(NT(215), (T(47)));
//G342: __E_comment_100(215) => eof(1).
	p(NT(215), (NT(1)));
//G343: comment(212)         => '#' __E_comment_99(214) __E_comment_100(215).
	p(NT(212), (T(48)+NT(214)+NT(215)));
//G344: __E____101(216)      => space(2).
	p(NT(216), (NT(2)));
//G345: __E____101(216)      => comment(212).
	p(NT(216), (NT(212)));
//G346: __(27)               => __E____101(216) _(11).
	p(NT(27), (NT(216)+NT(11)));
//G347: __E___102(217)       => __(27).
	p(NT(217), (NT(27)));
//G348: __E___102(217)       => null.
	p(NT(217), (nul));
//G349: _(11)                => __E___102(217).
	p(NT(11), (NT(217)));
//G350: cli(218)             => _(11).
	p(NT(218), (NT(11)));
//G351: __E_cli_103(220)     => '.' _(11) cli_command(219) _(11).
	p(NT(220), (T(1)+NT(11)+NT(219)+NT(11)));
//G352: __E_cli_104(221)     => null.
	p(NT(221), (nul));
//G353: __E_cli_104(221)     => __E_cli_103(220) __E_cli_104(221).
	p(NT(221), (NT(220)+NT(221)));
//G354: cli(218)             => _(11) cli_command(219) _(11) __E_cli_104(221).
	p(NT(218), (NT(11)+NT(219)+NT(11)+NT(221)));
//G355: __E_cli_command_105(224) => 'q'.
	p(NT(224), (T(49)));
//G356: __E_cli_command_105(224) => 'q' 'u' 'i' 't'.
	p(NT(224), (T(49)+T(41)+T(10)+T(13)));
//G357: quit_sym(223)        => __E_cli_command_105(224).
	p(NT(223), (NT(224)));
//G358: quit_cmd(222)        => quit_sym(223).
	p(NT(222), (NT(223)));
//G359: cli_command(219)     => quit_cmd(222).
	p(NT(219), (NT(222)));
//G360: __E_cli_command_106(227) => 'v'.
	p(NT(227), (T(50)));
//G361: __E_cli_command_106(227) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(227), (T(50)+T(19)+T(11)+T(12)+T(10)+T(17)+T(51)));
//G362: version_sym(226)     => __E_cli_command_106(227).
	p(NT(226), (NT(227)));
//G363: version_cmd(225)     => version_sym(226).
	p(NT(225), (NT(226)));
//G364: cli_command(219)     => version_cmd(225).
	p(NT(219), (NT(225)));
//G365: __E_cli_command_107(230) => 'c'.
	p(NT(230), (T(8)));
//G366: __E_cli_command_107(230) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(230), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G367: clear_sym(229)       => __E_cli_command_107(230).
	p(NT(229), (NT(230)));
//G368: clear_cmd(228)       => clear_sym(229).
	p(NT(228), (NT(229)));
//G369: cli_command(219)     => clear_cmd(228).
	p(NT(219), (NT(228)));
//G370: __E___E_cli_command_108_109(234) => 'h'.
	p(NT(234), (T(52)));
//G371: __E___E_cli_command_108_109(234) => 'h' 'e' 'l' 'p'.
	p(NT(234), (T(52)+T(19)+T(6)+T(53)));
//G372: help_sym(233)        => __E___E_cli_command_108_109(234).
	p(NT(233), (NT(234)));
//G373: __E___E_cli_command_108_110(235) => __(27) help_arg(236).
	p(NT(235), (NT(27)+NT(236)));
//G374: __E___E_cli_command_108_110(235) => null.
	p(NT(235), (nul));
//G375: __E_cli_command_108(232) => help_sym(233) __E___E_cli_command_108_110(235).
	p(NT(232), (NT(233)+NT(235)));
//G376: help_cmd(231)        => __E_cli_command_108(232).
	p(NT(231), (NT(232)));
//G377: cli_command(219)     => help_cmd(231).
	p(NT(219), (NT(231)));
//G378: file_sym(239)        => 'f' 'i' 'l' 'e'.
	p(NT(239), (T(4)+T(10)+T(6)+T(19)));
//G379: __E_cli_command_111(238) => file_sym(239) __(27) q_string(240).
	p(NT(238), (NT(239)+NT(27)+NT(240)));
//G380: file_cmd(237)        => __E_cli_command_111(238).
	p(NT(237), (NT(238)));
//G381: cli_command(219)     => file_cmd(237).
	p(NT(219), (NT(237)));
//G382: valid_sym(243)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(243), (T(50)+T(5)+T(6)+T(10)+T(54)));
//G383: __E_cli_command_112(242) => valid_sym(243) __(27) normalize_cmd_arg(244).
	p(NT(242), (NT(243)+NT(27)+NT(244)));
//G384: valid_cmd(241)       => __E_cli_command_112(242).
	p(NT(241), (NT(242)));
//G385: cli_command(219)     => valid_cmd(241).
	p(NT(219), (NT(241)));
//G386: sat_sym(247)         => 's' 'a' 't'.
	p(NT(247), (T(12)+T(5)+T(13)));
//G387: __E_cli_command_113(246) => sat_sym(247) __(27) normalize_cmd_arg(244).
	p(NT(246), (NT(247)+NT(27)+NT(244)));
//G388: sat_cmd(245)         => __E_cli_command_113(246).
	p(NT(245), (NT(246)));
//G389: cli_command(219)     => sat_cmd(245).
	p(NT(219), (NT(245)));
//G390: unsat_sym(250)       => 'u' 'n' 's' 'a' 't'.
	p(NT(250), (T(41)+T(51)+T(12)+T(5)+T(13)));
//G391: __E_cli_command_114(249) => unsat_sym(250) __(27) normalize_cmd_arg(244).
	p(NT(249), (NT(250)+NT(27)+NT(244)));
//G392: unsat_cmd(248)       => __E_cli_command_114(249).
	p(NT(248), (NT(249)));
//G393: cli_command(219)     => unsat_cmd(248).
	p(NT(219), (NT(248)));
//G394: solve_sym(253)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(253), (T(12)+T(17)+T(6)+T(50)+T(19)));
//G395: __E___E_cli_command_115_116(254) => solve_options(255).
	p(NT(254), (NT(255)));
//G396: __E___E_cli_command_115_116(254) => null.
	p(NT(254), (nul));
//G397: __E_cli_command_115(252) => solve_sym(253) __E___E_cli_command_115_116(254) __(27) wff_cmd_arg(256).
	p(NT(252), (NT(253)+NT(254)+NT(27)+NT(256)));
//G398: solve_cmd(251)       => __E_cli_command_115(252).
	p(NT(251), (NT(252)));
//G399: cli_command(219)     => solve_cmd(251).
	p(NT(219), (NT(251)));
//G400: lgrs_sym(259)        => 'l' 'g' 'r' 's'.
	p(NT(259), (T(6)+T(55)+T(11)+T(12)));
//G401: __E_cli_command_117(258) => lgrs_sym(259) __(27) wff_cmd_arg(256).
	p(NT(258), (NT(259)+NT(27)+NT(256)));
//G402: lgrs_cmd(257)        => __E_cli_command_117(258).
	p(NT(257), (NT(258)));
//G403: cli_command(219)     => lgrs_cmd(257).
	p(NT(219), (NT(257)));
//G404: __E___E_cli_command_118_119(263) => 'r'.
	p(NT(263), (T(11)));
//G405: __E___E_cli_command_118_119(263) => 'r' 'u' 'n'.
	p(NT(263), (T(11)+T(41)+T(51)));
//G406: run_sym(262)         => __E___E_cli_command_118_119(263).
	p(NT(262), (NT(263)));
//G407: __E___E_cli_command_118_120(264) => memory(265).
	p(NT(264), (NT(265)));
//G408: __E___E_cli_command_118_120(264) => wff(14).
	p(NT(264), (NT(14)));
//G409: __E_cli_command_118(261) => run_sym(262) __(27) __E___E_cli_command_118_120(264).
	p(NT(261), (NT(262)+NT(27)+NT(264)));
//G410: run_cmd(260)         => __E_cli_command_118(261).
	p(NT(260), (NT(261)));
//G411: cli_command(219)     => run_cmd(260).
	p(NT(219), (NT(260)));
//G412: __E___E_cli_command_121_122(269) => 'n'.
	p(NT(269), (T(51)));
//G413: __E___E_cli_command_121_122(269) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(269), (T(51)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(56)+T(19)));
//G414: normalize_sym(268)   => __E___E_cli_command_121_122(269).
	p(NT(268), (NT(269)));
//G415: __E_cli_command_121(267) => normalize_sym(268) __(27) normalize_cmd_arg(244).
	p(NT(267), (NT(268)+NT(27)+NT(244)));
//G416: normalize_cmd(266)   => __E_cli_command_121(267).
	p(NT(266), (NT(267)));
//G417: cli_command(219)     => normalize_cmd(266).
	p(NT(219), (NT(266)));
//G418: __E___E_cli_command_123_124(273) => 's'.
	p(NT(273), (T(12)));
//G419: __E___E_cli_command_123_124(273) => 's' 'u' 'b' 's' 't'.
	p(NT(273), (T(12)+T(41)+T(7)+T(12)+T(13)));
//G420: __E___E_cli_command_123_124(273) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(273), (T(12)+T(41)+T(7)+T(12)+T(13)+T(10)+T(13)+T(41)+T(13)+T(19)));
//G421: subst_sym(272)       => __E___E_cli_command_123_124(273).
	p(NT(272), (NT(273)));
//G422: __E_cli_command_123(271) => subst_sym(272) __(27) nf_cmd_arg(274) _(11) '[' _(11) nf_cmd_arg(274) _(11) '/' _(11) nf_cmd_arg(274) _(11) ']'.
	p(NT(271), (NT(272)+NT(27)+NT(274)+NT(11)+T(24)+NT(11)+NT(274)+NT(11)+T(57)+NT(11)+NT(274)+NT(11)+T(25)));
//G423: subst_cmd(270)       => __E_cli_command_123(271).
	p(NT(270), (NT(271)));
//G424: cli_command(219)     => subst_cmd(270).
	p(NT(219), (NT(270)));
//G425: __E___E_cli_command_125_126(278) => 'i'.
	p(NT(278), (T(10)));
//G426: __E___E_cli_command_125_126(278) => 'i' 'n' 's' 't'.
	p(NT(278), (T(10)+T(51)+T(12)+T(13)));
//G427: __E___E_cli_command_125_126(278) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(278), (T(10)+T(51)+T(12)+T(13)+T(5)+T(51)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G428: inst_sym(277)        => __E___E_cli_command_125_126(278).
	p(NT(277), (NT(278)));
//G429: __E_cli_command_125(276) => inst_sym(277) __(27) inst_args(279).
	p(NT(276), (NT(277)+NT(27)+NT(279)));
//G430: inst_cmd(275)        => __E_cli_command_125(276).
	p(NT(275), (NT(276)));
//G431: cli_command(219)     => inst_cmd(275).
	p(NT(219), (NT(275)));
//G432: dnf_sym(282)         => 'd' 'n' 'f'.
	p(NT(282), (T(54)+T(51)+T(4)));
//G433: __E_cli_command_127(281) => dnf_sym(282) __(27) nf_cmd_arg(274).
	p(NT(281), (NT(282)+NT(27)+NT(274)));
//G434: dnf_cmd(280)         => __E_cli_command_127(281).
	p(NT(280), (NT(281)));
//G435: cli_command(219)     => dnf_cmd(280).
	p(NT(219), (NT(280)));
//G436: cnf_sym(285)         => 'c' 'n' 'f'.
	p(NT(285), (T(8)+T(51)+T(4)));
//G437: __E_cli_command_128(284) => cnf_sym(285) __(27) nf_cmd_arg(274).
	p(NT(284), (NT(285)+NT(27)+NT(274)));
//G438: cnf_cmd(283)         => __E_cli_command_128(284).
	p(NT(283), (NT(284)));
//G439: cli_command(219)     => cnf_cmd(283).
	p(NT(219), (NT(283)));
//G440: anf_sym(288)         => 'a' 'n' 'f'.
	p(NT(288), (T(5)+T(51)+T(4)));
//G441: __E_cli_command_129(287) => anf_sym(288) __(27) nf_cmd_arg(274).
	p(NT(287), (NT(288)+NT(27)+NT(274)));
//G442: anf_cmd(286)         => __E_cli_command_129(287).
	p(NT(286), (NT(287)));
//G443: cli_command(219)     => anf_cmd(286).
	p(NT(219), (NT(286)));
//G444: nnf_sym(291)         => 'n' 'n' 'f'.
	p(NT(291), (T(51)+T(51)+T(4)));
//G445: __E_cli_command_130(290) => nnf_sym(291) __(27) nf_cmd_arg(274).
	p(NT(290), (NT(291)+NT(27)+NT(274)));
//G446: nnf_cmd(289)         => __E_cli_command_130(290).
	p(NT(289), (NT(290)));
//G447: cli_command(219)     => nnf_cmd(289).
	p(NT(219), (NT(289)));
//G448: pnf_sym(294)         => 'p' 'n' 'f'.
	p(NT(294), (T(53)+T(51)+T(4)));
//G449: __E_cli_command_131(293) => pnf_sym(294) __(27) nf_cmd_arg(274).
	p(NT(293), (NT(294)+NT(27)+NT(274)));
//G450: pnf_cmd(292)         => __E_cli_command_131(293).
	p(NT(292), (NT(293)));
//G451: cli_command(219)     => pnf_cmd(292).
	p(NT(219), (NT(292)));
//G452: mnf_sym(297)         => 'm' 'n' 'f'.
	p(NT(297), (T(18)+T(51)+T(4)));
//G453: __E_cli_command_132(296) => mnf_sym(297) __(27) nf_cmd_arg(274).
	p(NT(296), (NT(297)+NT(27)+NT(274)));
//G454: mnf_cmd(295)         => __E_cli_command_132(296).
	p(NT(295), (NT(296)));
//G455: cli_command(219)     => mnf_cmd(295).
	p(NT(219), (NT(295)));
//G456: snf_sym(300)         => 's' 'n' 'f'.
	p(NT(300), (T(12)+T(51)+T(4)));
//G457: __E_cli_command_133(299) => snf_sym(300) __(27) nf_cmd_arg(274).
	p(NT(299), (NT(300)+NT(27)+NT(274)));
//G458: snf_cmd(298)         => __E_cli_command_133(299).
	p(NT(298), (NT(299)));
//G459: cli_command(219)     => snf_cmd(298).
	p(NT(219), (NT(298)));
//G460: onf_sym(303)         => 'o' 'n' 'f'.
	p(NT(303), (T(17)+T(51)+T(4)));
//G461: __E_cli_command_134(302) => onf_sym(303) __(27) bf_variable(118) __(27) onf_cmd_arg(304).
	p(NT(302), (NT(303)+NT(27)+NT(118)+NT(27)+NT(304)));
//G462: onf_cmd(301)         => __E_cli_command_134(302).
	p(NT(301), (NT(302)));
//G463: cli_command(219)     => onf_cmd(301).
	p(NT(219), (NT(301)));
//G464: __E___E_cli_command_135_136(308) => 'd' 'e' 'f' 's'.
	p(NT(308), (T(54)+T(19)+T(4)+T(12)));
//G465: __E___E_cli_command_135_136(308) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(308), (T(54)+T(19)+T(4)+T(10)+T(51)+T(10)+T(13)+T(10)+T(17)+T(51)+T(12)));
//G466: def_sym(307)         => __E___E_cli_command_135_136(308).
	p(NT(307), (NT(308)));
//G467: __E_cli_command_135(306) => def_sym(307).
	p(NT(306), (NT(307)));
//G468: def_list_cmd(305)    => __E_cli_command_135(306).
	p(NT(305), (NT(306)));
//G469: cli_command(219)     => def_list_cmd(305).
	p(NT(219), (NT(305)));
//G470: __E_cli_command_137(310) => def_sym(307) __(27) number(311).
	p(NT(310), (NT(307)+NT(27)+NT(311)));
//G471: def_print_cmd(309)   => __E_cli_command_137(310).
	p(NT(309), (NT(310)));
//G472: cli_command(219)     => def_print_cmd(309).
	p(NT(219), (NT(309)));
//G473: def_rr_cmd(312)      => rec_relation(16).
	p(NT(312), (NT(16)));
//G474: cli_command(219)     => def_rr_cmd(312).
	p(NT(219), (NT(312)));
//G475: qelim_sym(315)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(315), (T(49)+T(19)+T(6)+T(10)+T(18)));
//G476: __E_cli_command_138(314) => qelim_sym(315) __(27) wff_cmd_arg(256).
	p(NT(314), (NT(315)+NT(27)+NT(256)));
//G477: qelim_cmd(313)       => __E_cli_command_138(314).
	p(NT(313), (NT(314)));
//G478: cli_command(219)     => qelim_cmd(313).
	p(NT(219), (NT(313)));
//G479: get_sym(318)         => 'g' 'e' 't'.
	p(NT(318), (T(55)+T(19)+T(13)));
//G480: __E___E_cli_command_139_140(319) => __(27) option_name(320).
	p(NT(319), (NT(27)+NT(320)));
//G481: __E___E_cli_command_139_140(319) => null.
	p(NT(319), (nul));
//G482: __E_cli_command_139(317) => get_sym(318) __E___E_cli_command_139_140(319).
	p(NT(317), (NT(318)+NT(319)));
//G483: get_cmd(316)         => __E_cli_command_139(317).
	p(NT(316), (NT(317)));
//G484: cli_command(219)     => get_cmd(316).
	p(NT(219), (NT(316)));
//G485: set_sym(323)         => 's' 'e' 't'.
	p(NT(323), (T(12)+T(19)+T(13)));
//G486: __E___E_cli_command_141_142(324) => __(27).
	p(NT(324), (NT(27)));
//G487: __E___E_cli_command_141_142(324) => _(11) '=' _(11).
	p(NT(324), (NT(11)+T(3)+NT(11)));
//G488: __E_cli_command_141(322) => set_sym(323) __(27) option_name(320) __E___E_cli_command_141_142(324) option_value(325).
	p(NT(322), (NT(323)+NT(27)+NT(320)+NT(324)+NT(325)));
//G489: set_cmd(321)         => __E_cli_command_141(322).
	p(NT(321), (NT(322)));
//G490: cli_command(219)     => set_cmd(321).
	p(NT(219), (NT(321)));
//G491: enable_sym(328)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(328), (T(19)+T(51)+T(5)+T(7)+T(6)+T(19)));
//G492: __E_cli_command_143(327) => enable_sym(328) __(27) option_name(320).
	p(NT(327), (NT(328)+NT(27)+NT(320)));
//G493: enable_cmd(326)      => __E_cli_command_143(327).
	p(NT(326), (NT(327)));
//G494: cli_command(219)     => enable_cmd(326).
	p(NT(219), (NT(326)));
//G495: disable_sym(331)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(331), (T(54)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G496: __E_cli_command_144(330) => disable_sym(331) __(27) option_name(320).
	p(NT(330), (NT(331)+NT(27)+NT(320)));
//G497: disable_cmd(329)     => __E_cli_command_144(330).
	p(NT(329), (NT(330)));
//G498: cli_command(219)     => disable_cmd(329).
	p(NT(219), (NT(329)));
//G499: toggle_sym(334)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(334), (T(13)+T(17)+T(55)+T(55)+T(6)+T(19)));
//G500: __E_cli_command_145(333) => toggle_sym(334) __(27) option_name(320).
	p(NT(333), (NT(334)+NT(27)+NT(320)));
//G501: toggle_cmd(332)      => __E_cli_command_145(333).
	p(NT(332), (NT(333)));
//G502: cli_command(219)     => toggle_cmd(332).
	p(NT(219), (NT(332)));
//G503: __E_cli_command_146(336) => type(129) __(27) in_var_name(173) _(11) '=' _(11) input_stream(337).
	p(NT(336), (NT(129)+NT(27)+NT(173)+NT(11)+T(3)+NT(11)+NT(337)));
//G504: def_input_cmd(335)   => __E_cli_command_146(336).
	p(NT(335), (NT(336)));
//G505: cli_command(219)     => def_input_cmd(335).
	p(NT(219), (NT(335)));
//G506: __E_cli_command_147(339) => type(129) __(27) out_var_name(177) _(11) '=' _(11) output_stream(340).
	p(NT(339), (NT(129)+NT(27)+NT(177)+NT(11)+T(3)+NT(11)+NT(340)));
//G507: def_output_cmd(338)  => __E_cli_command_147(339).
	p(NT(338), (NT(339)));
//G508: cli_command(219)     => def_output_cmd(338).
	p(NT(219), (NT(338)));
//G509: __E___E_cli_command_148_149(344) => 'h' 'i' 's' 't'.
	p(NT(344), (T(52)+T(10)+T(12)+T(13)));
//G510: __E___E_cli_command_148_149(344) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(344), (T(52)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G511: history_sym(343)     => __E___E_cli_command_148_149(344).
	p(NT(343), (NT(344)));
//G512: __E_cli_command_148(342) => history_sym(343).
	p(NT(342), (NT(343)));
//G513: history_list_cmd(341) => __E_cli_command_148(342).
	p(NT(341), (NT(342)));
//G514: cli_command(219)     => history_list_cmd(341).
	p(NT(219), (NT(341)));
//G515: __E_cli_command_150(346) => history_sym(343) __(27) memory(265).
	p(NT(346), (NT(343)+NT(27)+NT(265)));
//G516: history_print_cmd(345) => __E_cli_command_150(346).
	p(NT(345), (NT(346)));
//G517: cli_command(219)     => history_print_cmd(345).
	p(NT(219), (NT(345)));
//G518: __E_cli_command_151(348) => wff(14).
	p(NT(348), (NT(14)));
//G519: __E_cli_command_151(348) => bf(21).
	p(NT(348), (NT(21)));
//G520: history_store_cmd(347) => __E_cli_command_151(348).
	p(NT(347), (NT(348)));
//G521: cli_command(219)     => history_store_cmd(347).
	p(NT(219), (NT(347)));
//G522: __E_solve_options_152(349) => __(27) solve_option(350).
	p(NT(349), (NT(27)+NT(350)));
//G523: __E_solve_options_153(351) => null.
	p(NT(351), (nul));
//G524: __E_solve_options_153(351) => __E_solve_options_152(349) __E_solve_options_153(351).
	p(NT(351), (NT(349)+NT(351)));
//G525: solve_options(255)   => __E_solve_options_153(351).
	p(NT(255), (NT(351)));
//G526: __E_solve_option_154(352) => '-' '-' solver_mode(353).
	p(NT(352), (T(28)+T(28)+NT(353)));
//G527: solve_option(350)    => __E_solve_option_154(352).
	p(NT(350), (NT(352)));
//G528: __E_solve_option_155(354) => '-' '-' type(129).
	p(NT(354), (T(28)+T(28)+NT(129)));
//G529: solve_option(350)    => __E_solve_option_155(354).
	p(NT(350), (NT(354)));
//G530: __E_solver_mode_156(356) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(356), (T(18)+T(10)+T(51)+T(10)+T(18)+T(41)+T(18)));
//G531: __E_solver_mode_156(356) => 'm' 'i' 'n'.
	p(NT(356), (T(18)+T(10)+T(51)));
//G532: solver_mode_minimum(355) => __E_solver_mode_156(356).
	p(NT(355), (NT(356)));
//G533: solver_mode(353)     => solver_mode_minimum(355).
	p(NT(353), (NT(355)));
//G534: __E_solver_mode_157(358) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(358), (T(18)+T(5)+T(27)+T(10)+T(18)+T(41)+T(18)));
//G535: __E_solver_mode_157(358) => 'm' 'a' 'x'.
	p(NT(358), (T(18)+T(5)+T(27)));
//G536: solver_mode_maximum(357) => __E_solver_mode_157(358).
	p(NT(357), (NT(358)));
//G537: solver_mode(353)     => solver_mode_maximum(357).
	p(NT(353), (NT(357)));
//G538: __E_input_stream_158(359) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(360) _(11) ')'.
	p(NT(359), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(360)+NT(11)+T(16)));
//G539: input_stream(337)    => __E_input_stream_158(359).
	p(NT(337), (NT(359)));
//G540: console_sym(361)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(361), (T(8)+T(17)+T(51)+T(12)+T(17)+T(6)+T(19)));
//G541: input_stream(337)    => console_sym(361).
	p(NT(337), (NT(361)));
//G542: __E_output_stream_159(362) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(360) _(11) ')'.
	p(NT(362), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(360)+NT(11)+T(16)));
//G543: output_stream(340)   => __E_output_stream_159(362).
	p(NT(340), (NT(362)));
//G544: output_stream(340)   => console_sym(361).
	p(NT(340), (NT(361)));
//G545: q_file_name(360)     => '"' file_name(363) '"'.
	p(NT(360), (T(58)+NT(363)+T(58)));
//G546: __E_file_name_160(364) => printable(8).
	p(NT(364), (NT(8)));
//G547: __E_file_name_160(364) => printable(8) __E_file_name_160(364).
	p(NT(364), (NT(8)+NT(364)));
//G548: file_name(363)       => __E_file_name_160(364).
	p(NT(363), (NT(364)));
//G549: __E_option_name_161(365) => alnum(6).
	p(NT(365), (NT(6)));
//G550: __E_option_name_161(365) => alnum(6) __E_option_name_161(365).
	p(NT(365), (NT(6)+NT(365)));
//G551: option_name(320)     => __E_option_name_161(365).
	p(NT(320), (NT(365)));
//G552: __E_option_value_162(366) => alnum(6).
	p(NT(366), (NT(6)));
//G553: __E_option_value_162(366) => alnum(6) __E_option_value_162(366).
	p(NT(366), (NT(6)+NT(366)));
//G554: option_value(325)    => __E_option_value_162(366).
	p(NT(325), (NT(366)));
//G555: number(311)          => digits(185).
	p(NT(311), (NT(185)));
//G556: bf_cmd_arg(367)      => memory(265).
	p(NT(367), (NT(265)));
//G557: bf_cmd_arg(367)      => bf(21).
	p(NT(367), (NT(21)));
//G558: wff_cmd_arg(256)     => memory(265).
	p(NT(256), (NT(265)));
//G559: wff_cmd_arg(256)     => wff(14).
	p(NT(256), (NT(14)));
//G560: nf_cmd_arg(274)      => memory(265).
	p(NT(274), (NT(265)));
//G561: nf_cmd_arg(274)      => ref(18).
	p(NT(274), (NT(18)));
//G562: nf_cmd_arg(274)      => wff(14).
	p(NT(274), (NT(14)));
//G563: nf_cmd_arg(274)      => bf(21).
	p(NT(274), (NT(21)));
//G564: onf_cmd_arg(304)     => memory(265).
	p(NT(304), (NT(265)));
//G565: onf_cmd_arg(304)     => wff(14).
	p(NT(304), (NT(14)));
//G566: normalize_cmd_arg(244) => memory(265).
	p(NT(244), (NT(265)));
//G567: normalize_cmd_arg(244) => rr(10).
	p(NT(244), (NT(10)));
//G568: normalize_cmd_arg(244) => ref(18).
	p(NT(244), (NT(18)));
//G569: normalize_cmd_arg(244) => wff(14).
	p(NT(244), (NT(14)));
//G570: normalize_cmd_arg(244) => bf(21).
	p(NT(244), (NT(21)));
//G571: inst_args(279)       => wff_cmd_arg(256) _(11) '[' _(11) bf_variable(118) _(11) '/' _(11) bf_cmd_arg(367) _(11) ']'.
	p(NT(279), (NT(256)+NT(11)+T(24)+NT(11)+NT(118)+NT(11)+T(57)+NT(11)+NT(367)+NT(11)+T(25)));
//G572: inst_args(279)       => bf_cmd_arg(367) _(11) '[' _(11) bf_variable(118) _(11) '/' _(11) bf_cmd_arg(367) _(11) ']'.
	p(NT(279), (NT(367)+NT(11)+T(24)+NT(11)+NT(118)+NT(11)+T(57)+NT(11)+NT(367)+NT(11)+T(25)));
//G573: help_arg(236)        => help_sym(233).
	p(NT(236), (NT(233)));
//G574: help_arg(236)        => version_sym(226).
	p(NT(236), (NT(226)));
//G575: help_arg(236)        => quit_sym(223).
	p(NT(236), (NT(223)));
//G576: help_arg(236)        => clear_sym(229).
	p(NT(236), (NT(229)));
//G577: help_arg(236)        => get_sym(318).
	p(NT(236), (NT(318)));
//G578: help_arg(236)        => set_sym(323).
	p(NT(236), (NT(323)));
//G579: help_arg(236)        => enable_sym(328).
	p(NT(236), (NT(328)));
//G580: help_arg(236)        => disable_sym(331).
	p(NT(236), (NT(331)));
//G581: help_arg(236)        => toggle_sym(334).
	p(NT(236), (NT(334)));
//G582: help_arg(236)        => file_sym(239).
	p(NT(236), (NT(239)));
//G583: help_arg(236)        => history_sym(343).
	p(NT(236), (NT(343)));
//G584: help_arg(236)        => abs_memory_sym(368).
	p(NT(236), (NT(368)));
//G585: help_arg(236)        => rel_memory_sym(369).
	p(NT(236), (NT(369)));
//G586: help_arg(236)        => selection_sym(370).
	p(NT(236), (NT(370)));
//G587: help_arg(236)        => def_sym(307).
	p(NT(236), (NT(307)));
//G588: help_arg(236)        => inst_sym(277).
	p(NT(236), (NT(277)));
//G589: help_arg(236)        => subst_sym(272).
	p(NT(236), (NT(272)));
//G590: help_arg(236)        => normalize_sym(268).
	p(NT(236), (NT(268)));
//G591: help_arg(236)        => execute_sym(371).
	p(NT(236), (NT(371)));
//G592: help_arg(236)        => solve_sym(253).
	p(NT(236), (NT(253)));
//G593: help_arg(236)        => valid_sym(243).
	p(NT(236), (NT(243)));
//G594: help_arg(236)        => sat_sym(247).
	p(NT(236), (NT(247)));
//G595: help_arg(236)        => unsat_sym(250).
	p(NT(236), (NT(250)));
//G596: help_arg(236)        => run_sym(262).
	p(NT(236), (NT(262)));
//G597: help_arg(236)        => dnf_sym(282).
	p(NT(236), (NT(282)));
//G598: help_arg(236)        => cnf_sym(285).
	p(NT(236), (NT(285)));
//G599: help_arg(236)        => anf_sym(288).
	p(NT(236), (NT(288)));
//G600: help_arg(236)        => snf_sym(300).
	p(NT(236), (NT(300)));
//G601: help_arg(236)        => nnf_sym(291).
	p(NT(236), (NT(291)));
//G602: help_arg(236)        => pnf_sym(294).
	p(NT(236), (NT(294)));
//G603: help_arg(236)        => mnf_sym(297).
	p(NT(236), (NT(297)));
//G604: help_arg(236)        => onf_sym(303).
	p(NT(236), (NT(303)));
//G605: help_arg(236)        => qelim_sym(315).
	p(NT(236), (NT(315)));
//G606: __E___E_help_arg_163_164(374) => 's'.
	p(NT(374), (T(12)));
//G607: __E___E_help_arg_163_164(374) => null.
	p(NT(374), (nul));
//G608: __E_help_arg_163(373) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_163_164(374).
	p(NT(373), (T(19)+T(27)+T(5)+T(18)+T(53)+T(6)+T(19)+NT(374)));
//G609: examples_sym(372)    => __E_help_arg_163(373).
	p(NT(372), (NT(373)));
//G610: help_arg(236)        => examples_sym(372).
	p(NT(236), (NT(372)));
//G611: __E_memory_165(376)  => '%'.
	p(NT(376), (T(59)));
//G612: rel_memory_sym(369)  => '%' '-'.
	p(NT(369), (T(59)+T(28)));
//G613: memory_id(378)       => digits(185).
	p(NT(378), (NT(185)));
//G614: __E___E_memory_165_166(377) => memory_id(378).
	p(NT(377), (NT(378)));
//G615: __E___E_memory_165_166(377) => null.
	p(NT(377), (nul));
//G616: __E_memory_165(376)  => rel_memory_sym(369) __E___E_memory_165_166(377).
	p(NT(376), (NT(369)+NT(377)));
//G617: rel_memory(375)      => __E_memory_165(376).
	p(NT(375), (NT(376)));
//G618: memory(265)          => rel_memory(375).
	p(NT(265), (NT(375)));
//G619: abs_memory_sym(368)  => '%'.
	p(NT(368), (T(59)));
//G620: __E_memory_167(380)  => abs_memory_sym(368) memory_id(378).
	p(NT(380), (NT(368)+NT(378)));
//G621: abs_memory(379)      => __E_memory_167(380).
	p(NT(379), (NT(380)));
//G622: memory(265)          => abs_memory(379).
	p(NT(265), (NT(379)));
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
		__E_wff_41, bf_less, __E_wff_42, bf_nless, __E_wff_43, bf_parenthesis, __E_bf_44, bf_constant, bf_variable, bf_splitter, 
		__E_bf_45, bf_ref, bf_or, __E_bf_46, bf_xor, __E_bf_47, bf_t, __E_bf_48, __E___E_bf_48_49, type, 
		bf_f, __E_bf_50, __E___E_bf_50_51, bf_and, __E_bf_52, __E___E_bf_52_53, bf_neg, __E_bf_54, bf_neg_oprnd, __E___E_bf_54_55, 
		bf_and_nosep, __E_bf_56, bf_and_nosep_1st_oprnd, __E___E_bf_56_57, bf_and_nosep_2nd_oprnd, __E___E_bf_56_58, ctn_neq, __E_constraint_59, ctnvar, num, 
		ctn_eq, __E_constraint_60, ctn_greater_equal, __E_constraint_61, ctn_greater, __E_constraint_62, ctn_less_equal, __E_constraint_63, ctn_less, __E_constraint_64, 
		variable, __E_bf_variable_65, chars, __E_variable_66, __E_variable_67, __E_variable_68, __E_variable_69, __E_variable_70, io_var, __E_variable_71, 
		in, out, uninterpreted_constant, in_var_name, __E_in_72, __E___E_in_72_73, offset, out_var_name, __E_out_74, __E___E_out_74_75, 
		__E___E___E_out_74_75_76, q_var, __E_q_vars_77, __E_q_vars_78, scope_id, digits, __E_offsets_79, __E_offsets_80, integer, shift, 
		__E_offset_81, __E_shift_82, __E___E_shift_82_83, __E_integer_84, uninter_const_name, __E_uninterpreted_constant_85, __E___E_uninterpreted_constant_85_86, constant, __E_bf_constant_87, binding, 
		__E_bf_constant_88, source, __E_binding_89, src_c, __E___E_binding_89_90, __E___E___E_binding_89_90_91, __E___E___E_binding_89_90_92, __E_src_c_93, __E_src_c_94, __E_chars_95, 
		__E_chars_96, __E_digits_97, comment, __E_comment_98, __E_comment_99, __E_comment_100, __E____101, __E___102, cli, cli_command, 
		__E_cli_103, __E_cli_104, quit_cmd, quit_sym, __E_cli_command_105, version_cmd, version_sym, __E_cli_command_106, clear_cmd, clear_sym, 
		__E_cli_command_107, help_cmd, __E_cli_command_108, help_sym, __E___E_cli_command_108_109, __E___E_cli_command_108_110, help_arg, file_cmd, __E_cli_command_111, file_sym, 
		q_string, valid_cmd, __E_cli_command_112, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_113, sat_sym, unsat_cmd, __E_cli_command_114, 
		unsat_sym, solve_cmd, __E_cli_command_115, solve_sym, __E___E_cli_command_115_116, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_117, lgrs_sym, 
		run_cmd, __E_cli_command_118, run_sym, __E___E_cli_command_118_119, __E___E_cli_command_118_120, memory, normalize_cmd, __E_cli_command_121, normalize_sym, __E___E_cli_command_121_122, 
		subst_cmd, __E_cli_command_123, subst_sym, __E___E_cli_command_123_124, nf_cmd_arg, inst_cmd, __E_cli_command_125, inst_sym, __E___E_cli_command_125_126, inst_args, 
		dnf_cmd, __E_cli_command_127, dnf_sym, cnf_cmd, __E_cli_command_128, cnf_sym, anf_cmd, __E_cli_command_129, anf_sym, nnf_cmd, 
		__E_cli_command_130, nnf_sym, pnf_cmd, __E_cli_command_131, pnf_sym, mnf_cmd, __E_cli_command_132, mnf_sym, snf_cmd, __E_cli_command_133, 
		snf_sym, onf_cmd, __E_cli_command_134, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_135, def_sym, __E___E_cli_command_135_136, def_print_cmd, 
		__E_cli_command_137, number, def_rr_cmd, qelim_cmd, __E_cli_command_138, qelim_sym, get_cmd, __E_cli_command_139, get_sym, __E___E_cli_command_139_140, 
		option_name, set_cmd, __E_cli_command_141, set_sym, __E___E_cli_command_141_142, option_value, enable_cmd, __E_cli_command_143, enable_sym, disable_cmd, 
		__E_cli_command_144, disable_sym, toggle_cmd, __E_cli_command_145, toggle_sym, def_input_cmd, __E_cli_command_146, input_stream, def_output_cmd, __E_cli_command_147, 
		output_stream, history_list_cmd, __E_cli_command_148, history_sym, __E___E_cli_command_148_149, history_print_cmd, __E_cli_command_150, history_store_cmd, __E_cli_command_151, __E_solve_options_152, 
		solve_option, __E_solve_options_153, __E_solve_option_154, solver_mode, __E_solve_option_155, solver_mode_minimum, __E_solver_mode_156, solver_mode_maximum, __E_solver_mode_157, __E_input_stream_158, 
		q_file_name, console_sym, __E_output_stream_159, file_name, __E_file_name_160, __E_option_name_161, __E_option_value_162, bf_cmd_arg, abs_memory_sym, rel_memory_sym, 
		selection_sym, execute_sym, examples_sym, __E_help_arg_163, __E___E_help_arg_163_164, rel_memory, __E_memory_165, __E___E_memory_165_166, memory_id, abs_memory, 
		__E_memory_167, named_binding, __N_0, __N_1, __N_2, __N_3, __N_4, __N_5, __N_6, __N_7, 
		__N_8, 
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
