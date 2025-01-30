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
	"__E_q_vars_76", "__E_q_vars_77", "__E_offsets_78", "__E_offsets_79", "shift", "__E_offset_80", "__E_shift_81", "__E___E_shift_81_82", "digits", "uninter_const_name", 
	"__E_uninterpreted_constant_83", "__E___E_uninterpreted_constant_83_84", "constant", "__E_bf_constant_85", "binding", "__E_bf_constant_86", "source", "__E_binding_87", "src_c", "__E___E_binding_87_88", 
	"__E___E___E_binding_87_88_89", "__E___E___E_binding_87_88_90", "__E_src_c_91", "__E_src_c_92", "__E_chars_93", "__E_chars_94", "__E_digits_95", "comment", "__E_comment_96", "__E_comment_97", 
	"__E_comment_98", "__E____99", "__E___100", "cli", "cli_command", "__E_cli_101", "__E_cli_102", "quit_cmd", "quit_sym", "__E_cli_command_103", 
	"version_cmd", "version_sym", "__E_cli_command_104", "clear_cmd", "clear_sym", "__E_cli_command_105", "help_cmd", "__E_cli_command_106", "help_sym", "__E___E_cli_command_106_107", 
	"__E___E_cli_command_106_108", "help_arg", "file_cmd", "__E_cli_command_109", "file_sym", "q_string", "valid_cmd", "__E_cli_command_110", "valid_sym", "normalize_cmd_arg", 
	"sat_cmd", "__E_cli_command_111", "sat_sym", "unsat_cmd", "__E_cli_command_112", "unsat_sym", "solve_cmd", "__E_cli_command_113", "solve_sym", "__E___E_cli_command_113_114", 
	"solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_115", "lgrs_sym", "run_cmd", "__E_cli_command_116", "run_sym", "__E___E_cli_command_116_117", "__E___E_cli_command_116_118", 
	"memory", "normalize_cmd", "__E_cli_command_119", "normalize_sym", "__E___E_cli_command_119_120", "subst_cmd", "__E_cli_command_121", "subst_sym", "__E___E_cli_command_121_122", "nf_cmd_arg", 
	"inst_cmd", "__E_cli_command_123", "inst_sym", "__E___E_cli_command_123_124", "inst_args", "dnf_cmd", "__E_cli_command_125", "dnf_sym", "cnf_cmd", "__E_cli_command_126", 
	"cnf_sym", "anf_cmd", "__E_cli_command_127", "anf_sym", "nnf_cmd", "__E_cli_command_128", "nnf_sym", "pnf_cmd", "__E_cli_command_129", "pnf_sym", 
	"mnf_cmd", "__E_cli_command_130", "mnf_sym", "snf_cmd", "__E_cli_command_131", "snf_sym", "onf_cmd", "__E_cli_command_132", "onf_sym", "onf_cmd_arg", 
	"def_list_cmd", "__E_cli_command_133", "def_sym", "__E___E_cli_command_133_134", "def_print_cmd", "__E_cli_command_135", "number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_136", 
	"qelim_sym", "get_cmd", "__E_cli_command_137", "get_sym", "__E___E_cli_command_137_138", "option_name", "set_cmd", "__E_cli_command_139", "set_sym", "__E___E_cli_command_139_140", 
	"option_value", "enable_cmd", "__E_cli_command_141", "enable_sym", "disable_cmd", "__E_cli_command_142", "disable_sym", "toggle_cmd", "__E_cli_command_143", "toggle_sym", 
	"def_input_cmd", "__E_cli_command_144", "input_stream", "def_output_cmd", "__E_cli_command_145", "output_stream", "history_list_cmd", "__E_cli_command_146", "history_sym", "__E___E_cli_command_146_147", 
	"history_print_cmd", "__E_cli_command_148", "history_store_cmd", "__E_cli_command_149", "__E_solve_options_150", "solve_option", "__E_solve_options_151", "__E_solve_option_152", "solver_mode", "__E_solve_option_153", 
	"solver_mode_minimum", "__E_solver_mode_154", "solver_mode_maximum", "__E_solver_mode_155", "__E_input_stream_156", "q_file_name", "console_sym", "__E_output_stream_157", "file_name", "__E_file_name_158", 
	"__E_option_name_159", "__E_option_value_160", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "execute_sym", "examples_sym", "__E_help_arg_161", "__E___E_help_arg_161_162", 
	"rel_memory", "__E_memory_163", "__E___E_memory_163_164", "memory_id", "abs_memory", "__E_memory_165", "named_binding", "__N_0", "__N_1", "__N_2", 
	"__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
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
			20, 22, 118, 129, 148, 170, 174, 188, 189, 196,
			315, 320, 358, 376
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
			{ 198 },
			{ 239 },
			{ 251 },
			{ 269 },
			{ 274 },
			{ 299 },
			{ 362 }
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
//G234: __N_0(377)           => 'F'.
	p(NT(377), (T(34)));
//G235: __N_1(378)           => 'T'.
	p(NT(378), (T(33)));
//G236: __E_variable_65(160) => ~( __N_0(377) ) & ~( __N_1(378) ) & alpha(5).	 # conjunctive
	p(NT(160), ~(NT(377)) & ~(NT(378)) & (NT(5)));
//G237: __E_variable_66(161) => null.
	p(NT(161), (nul));
//G238: __E_variable_66(161) => digit(3) __E_variable_66(161).
	p(NT(161), (NT(3)+NT(161)));
//G239: variable(118)        => __E_variable_65(160) __E_variable_66(161).	 # guarded: charvar
	p(NT(118), (NT(160)+NT(161)));
	p.back().guard = "charvar";
//G240: __N_2(379)           => 'F'.
	p(NT(379), (T(34)));
//G241: __N_3(380)           => 'T'.
	p(NT(380), (T(33)));
//G242: __E_variable_67(162) => ~( __N_2(379) ) & ~( __N_3(380) ) & alpha(5).	 # conjunctive
	p(NT(162), ~(NT(379)) & ~(NT(380)) & (NT(5)));
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
//G272: __N_4(381)           => uninterpreted_constant(169).
	p(NT(381), (NT(169)));
//G273: q_var(179)           => variable(118) & ~( __N_4(381) ).	 # conjunctive
	p(NT(179), (NT(118)) & ~(NT(381)));
//G274: __E_offsets_78(182)  => _(11) ',' _(11) offset(173).
	p(NT(182), (NT(11)+T(14)+NT(11)+NT(173)));
//G275: __E_offsets_79(183)  => null.
	p(NT(183), (nul));
//G276: __E_offsets_79(183)  => __E_offsets_78(182) __E_offsets_79(183).
	p(NT(183), (NT(182)+NT(183)));
//G277: offsets(24)          => '[' _(11) offset(173) __E_offsets_79(183) _(11) ']'.
	p(NT(24), (T(24)+NT(11)+NT(173)+NT(183)+NT(11)+T(25)));
//G278: offset(173)          => num(149).
	p(NT(173), (NT(149)));
//G279: offset(173)          => capture(20).
	p(NT(173), (NT(20)));
//G280: offset(173)          => shift(184).
	p(NT(173), (NT(184)));
//G281: __N_5(382)           => io_var(165).
	p(NT(382), (NT(165)));
//G282: __E_offset_80(185)   => variable(118) & ~( __N_5(382) ).	 # conjunctive
	p(NT(185), (NT(118)) & ~(NT(382)));
//G283: offset(173)          => __E_offset_80(185).
	p(NT(173), (NT(185)));
//G284: __E_shift_81(186)    => capture(20).
	p(NT(186), (NT(20)));
//G285: __N_6(383)           => io_var(165).
	p(NT(383), (NT(165)));
//G286: __E___E_shift_81_82(187) => variable(118) & ~( __N_6(383) ).	 # conjunctive
	p(NT(187), (NT(118)) & ~(NT(383)));
//G287: __E_shift_81(186)    => __E___E_shift_81_82(187).
	p(NT(186), (NT(187)));
//G288: shift(184)           => __E_shift_81(186) _(11) '-' _(11) num(149).
	p(NT(184), (NT(186)+NT(11)+T(28)+NT(11)+NT(149)));
//G289: num(149)             => digits(188).
	p(NT(149), (NT(188)));
//G290: __E___E_uninterpreted_constant_83_84(191) => chars(178).
	p(NT(191), (NT(178)));
//G291: __E___E_uninterpreted_constant_83_84(191) => _(11).
	p(NT(191), (NT(11)));
//G292: __E_uninterpreted_constant_83(190) => __E___E_uninterpreted_constant_83_84(191) ':' chars(178).
	p(NT(190), (NT(191)+T(2)+NT(178)));
//G293: uninter_const_name(189) => __E_uninterpreted_constant_83(190).
	p(NT(189), (NT(190)));
//G294: uninterpreted_constant(169) => '<' uninter_const_name(189) _(11) '>'.
	p(NT(169), (T(20)+NT(189)+NT(11)+T(21)));
//G295: __E_bf_constant_85(193) => capture(20).
	p(NT(193), (NT(20)));
//G296: __E_bf_constant_85(193) => binding(194).
	p(NT(193), (NT(194)));
//G297: constant(192)        => __E_bf_constant_85(193).
	p(NT(192), (NT(193)));
//G298: type(129)            => chars(178).
	p(NT(129), (NT(178)));
//G299: __E_bf_constant_86(195) => _(11) ':' _(11) type(129).
	p(NT(195), (NT(11)+T(2)+NT(11)+NT(129)));
//G300: __E_bf_constant_86(195) => null.
	p(NT(195), (nul));
//G301: bf_constant(117)     => '{' _(11) constant(192) _(11) '}' __E_bf_constant_86(195).
	p(NT(117), (T(43)+NT(11)+NT(192)+NT(11)+T(44)+NT(195)));
//G302: __E___E___E_binding_87_88_89(200) => src_c(198).
	p(NT(200), (NT(198)));
//G303: __E___E___E_binding_87_88_89(200) => space(2).
	p(NT(200), (NT(2)));
//G304: __E___E___E_binding_87_88_90(201) => null.
	p(NT(201), (nul));
//G305: __E___E___E_binding_87_88_90(201) => __E___E___E_binding_87_88_89(200) __E___E___E_binding_87_88_90(201).
	p(NT(201), (NT(200)+NT(201)));
//G306: __E___E_binding_87_88(199) => __E___E___E_binding_87_88_90(201) src_c(198).
	p(NT(199), (NT(201)+NT(198)));
//G307: __E___E_binding_87_88(199) => null.
	p(NT(199), (nul));
//G308: __E_binding_87(197)  => src_c(198) __E___E_binding_87_88(199).
	p(NT(197), (NT(198)+NT(199)));
//G309: source(196)          => __E_binding_87(197).
	p(NT(196), (NT(197)));
//G310: binding(194)         => source(196).
	p(NT(194), (NT(196)));
//G311: src_c(198)           => alnum(6).
	p(NT(198), (NT(6)));
//G312: __N_7(384)           => '{'.
	p(NT(384), (T(43)));
//G313: __N_8(385)           => '}'.
	p(NT(385), (T(44)));
//G314: src_c(198)           => ~( __N_7(384) ) & ~( __N_8(385) ) & punct(7).	 # conjunctive
	p(NT(198), ~(NT(384)) & ~(NT(385)) & (NT(7)));
//G315: __E_src_c_91(202)    => src_c(198).
	p(NT(202), (NT(198)));
//G316: __E_src_c_91(202)    => space(2).
	p(NT(202), (NT(2)));
//G317: __E_src_c_92(203)    => null.
	p(NT(203), (nul));
//G318: __E_src_c_92(203)    => __E_src_c_91(202) __E_src_c_92(203).
	p(NT(203), (NT(202)+NT(203)));
//G319: src_c(198)           => '{' __E_src_c_92(203) '}'.
	p(NT(198), (T(43)+NT(203)+T(44)));
//G320: __E_chars_93(204)    => alnum(6).
	p(NT(204), (NT(6)));
//G321: __E_chars_93(204)    => '_'.
	p(NT(204), (T(40)));
//G322: __E_chars_94(205)    => null.
	p(NT(205), (nul));
//G323: __E_chars_94(205)    => __E_chars_93(204) __E_chars_94(205).
	p(NT(205), (NT(204)+NT(205)));
//G324: chars(178)           => alpha(5) __E_chars_94(205).
	p(NT(178), (NT(5)+NT(205)));
//G325: __E_digits_95(206)   => digit(3).
	p(NT(206), (NT(3)));
//G326: __E_digits_95(206)   => digit(3) __E_digits_95(206).
	p(NT(206), (NT(3)+NT(206)));
//G327: digits(188)          => __E_digits_95(206).
	p(NT(188), (NT(206)));
//G328: sym(22)              => chars(178).
	p(NT(22), (NT(178)));
//G329: __E_comment_96(208)  => printable(8).
	p(NT(208), (NT(8)));
//G330: __E_comment_96(208)  => '\t'.
	p(NT(208), (T(45)));
//G331: __E_comment_97(209)  => null.
	p(NT(209), (nul));
//G332: __E_comment_97(209)  => __E_comment_96(208) __E_comment_97(209).
	p(NT(209), (NT(208)+NT(209)));
//G333: __E_comment_98(210)  => '\n'.
	p(NT(210), (T(46)));
//G334: __E_comment_98(210)  => '\r'.
	p(NT(210), (T(47)));
//G335: __E_comment_98(210)  => eof(1).
	p(NT(210), (NT(1)));
//G336: comment(207)         => '#' __E_comment_97(209) __E_comment_98(210).
	p(NT(207), (T(48)+NT(209)+NT(210)));
//G337: __E____99(211)       => space(2).
	p(NT(211), (NT(2)));
//G338: __E____99(211)       => comment(207).
	p(NT(211), (NT(207)));
//G339: __(27)               => __E____99(211) _(11).
	p(NT(27), (NT(211)+NT(11)));
//G340: __E___100(212)       => __(27).
	p(NT(212), (NT(27)));
//G341: __E___100(212)       => null.
	p(NT(212), (nul));
//G342: _(11)                => __E___100(212).
	p(NT(11), (NT(212)));
//G343: cli(213)             => _(11).
	p(NT(213), (NT(11)));
//G344: __E_cli_101(215)     => '.' _(11) cli_command(214) _(11).
	p(NT(215), (T(1)+NT(11)+NT(214)+NT(11)));
//G345: __E_cli_102(216)     => null.
	p(NT(216), (nul));
//G346: __E_cli_102(216)     => __E_cli_101(215) __E_cli_102(216).
	p(NT(216), (NT(215)+NT(216)));
//G347: cli(213)             => _(11) cli_command(214) _(11) __E_cli_102(216).
	p(NT(213), (NT(11)+NT(214)+NT(11)+NT(216)));
//G348: __E_cli_command_103(219) => 'q'.
	p(NT(219), (T(49)));
//G349: __E_cli_command_103(219) => 'q' 'u' 'i' 't'.
	p(NT(219), (T(49)+T(41)+T(10)+T(13)));
//G350: quit_sym(218)        => __E_cli_command_103(219).
	p(NT(218), (NT(219)));
//G351: quit_cmd(217)        => quit_sym(218).
	p(NT(217), (NT(218)));
//G352: cli_command(214)     => quit_cmd(217).
	p(NT(214), (NT(217)));
//G353: __E_cli_command_104(222) => 'v'.
	p(NT(222), (T(50)));
//G354: __E_cli_command_104(222) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(222), (T(50)+T(19)+T(11)+T(12)+T(10)+T(17)+T(51)));
//G355: version_sym(221)     => __E_cli_command_104(222).
	p(NT(221), (NT(222)));
//G356: version_cmd(220)     => version_sym(221).
	p(NT(220), (NT(221)));
//G357: cli_command(214)     => version_cmd(220).
	p(NT(214), (NT(220)));
//G358: __E_cli_command_105(225) => 'c'.
	p(NT(225), (T(8)));
//G359: __E_cli_command_105(225) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(225), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G360: clear_sym(224)       => __E_cli_command_105(225).
	p(NT(224), (NT(225)));
//G361: clear_cmd(223)       => clear_sym(224).
	p(NT(223), (NT(224)));
//G362: cli_command(214)     => clear_cmd(223).
	p(NT(214), (NT(223)));
//G363: __E___E_cli_command_106_107(229) => 'h'.
	p(NT(229), (T(52)));
//G364: __E___E_cli_command_106_107(229) => 'h' 'e' 'l' 'p'.
	p(NT(229), (T(52)+T(19)+T(6)+T(53)));
//G365: help_sym(228)        => __E___E_cli_command_106_107(229).
	p(NT(228), (NT(229)));
//G366: __E___E_cli_command_106_108(230) => __(27) help_arg(231).
	p(NT(230), (NT(27)+NT(231)));
//G367: __E___E_cli_command_106_108(230) => null.
	p(NT(230), (nul));
//G368: __E_cli_command_106(227) => help_sym(228) __E___E_cli_command_106_108(230).
	p(NT(227), (NT(228)+NT(230)));
//G369: help_cmd(226)        => __E_cli_command_106(227).
	p(NT(226), (NT(227)));
//G370: cli_command(214)     => help_cmd(226).
	p(NT(214), (NT(226)));
//G371: file_sym(234)        => 'f' 'i' 'l' 'e'.
	p(NT(234), (T(4)+T(10)+T(6)+T(19)));
//G372: __E_cli_command_109(233) => file_sym(234) __(27) q_string(235).
	p(NT(233), (NT(234)+NT(27)+NT(235)));
//G373: file_cmd(232)        => __E_cli_command_109(233).
	p(NT(232), (NT(233)));
//G374: cli_command(214)     => file_cmd(232).
	p(NT(214), (NT(232)));
//G375: valid_sym(238)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(238), (T(50)+T(5)+T(6)+T(10)+T(54)));
//G376: __E_cli_command_110(237) => valid_sym(238) __(27) normalize_cmd_arg(239).
	p(NT(237), (NT(238)+NT(27)+NT(239)));
//G377: valid_cmd(236)       => __E_cli_command_110(237).
	p(NT(236), (NT(237)));
//G378: cli_command(214)     => valid_cmd(236).
	p(NT(214), (NT(236)));
//G379: sat_sym(242)         => 's' 'a' 't'.
	p(NT(242), (T(12)+T(5)+T(13)));
//G380: __E_cli_command_111(241) => sat_sym(242) __(27) normalize_cmd_arg(239).
	p(NT(241), (NT(242)+NT(27)+NT(239)));
//G381: sat_cmd(240)         => __E_cli_command_111(241).
	p(NT(240), (NT(241)));
//G382: cli_command(214)     => sat_cmd(240).
	p(NT(214), (NT(240)));
//G383: unsat_sym(245)       => 'u' 'n' 's' 'a' 't'.
	p(NT(245), (T(41)+T(51)+T(12)+T(5)+T(13)));
//G384: __E_cli_command_112(244) => unsat_sym(245) __(27) normalize_cmd_arg(239).
	p(NT(244), (NT(245)+NT(27)+NT(239)));
//G385: unsat_cmd(243)       => __E_cli_command_112(244).
	p(NT(243), (NT(244)));
//G386: cli_command(214)     => unsat_cmd(243).
	p(NT(214), (NT(243)));
//G387: solve_sym(248)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(248), (T(12)+T(17)+T(6)+T(50)+T(19)));
//G388: __E___E_cli_command_113_114(249) => solve_options(250).
	p(NT(249), (NT(250)));
//G389: __E___E_cli_command_113_114(249) => null.
	p(NT(249), (nul));
//G390: __E_cli_command_113(247) => solve_sym(248) __E___E_cli_command_113_114(249) __(27) wff_cmd_arg(251).
	p(NT(247), (NT(248)+NT(249)+NT(27)+NT(251)));
//G391: solve_cmd(246)       => __E_cli_command_113(247).
	p(NT(246), (NT(247)));
//G392: cli_command(214)     => solve_cmd(246).
	p(NT(214), (NT(246)));
//G393: lgrs_sym(254)        => 'l' 'g' 'r' 's'.
	p(NT(254), (T(6)+T(55)+T(11)+T(12)));
//G394: __E_cli_command_115(253) => lgrs_sym(254) __(27) wff_cmd_arg(251).
	p(NT(253), (NT(254)+NT(27)+NT(251)));
//G395: lgrs_cmd(252)        => __E_cli_command_115(253).
	p(NT(252), (NT(253)));
//G396: cli_command(214)     => lgrs_cmd(252).
	p(NT(214), (NT(252)));
//G397: __E___E_cli_command_116_117(258) => 'r'.
	p(NT(258), (T(11)));
//G398: __E___E_cli_command_116_117(258) => 'r' 'u' 'n'.
	p(NT(258), (T(11)+T(41)+T(51)));
//G399: run_sym(257)         => __E___E_cli_command_116_117(258).
	p(NT(257), (NT(258)));
//G400: __E___E_cli_command_116_118(259) => memory(260).
	p(NT(259), (NT(260)));
//G401: __E___E_cli_command_116_118(259) => wff(14).
	p(NT(259), (NT(14)));
//G402: __E_cli_command_116(256) => run_sym(257) __(27) __E___E_cli_command_116_118(259).
	p(NT(256), (NT(257)+NT(27)+NT(259)));
//G403: run_cmd(255)         => __E_cli_command_116(256).
	p(NT(255), (NT(256)));
//G404: cli_command(214)     => run_cmd(255).
	p(NT(214), (NT(255)));
//G405: __E___E_cli_command_119_120(264) => 'n'.
	p(NT(264), (T(51)));
//G406: __E___E_cli_command_119_120(264) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(264), (T(51)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(56)+T(19)));
//G407: normalize_sym(263)   => __E___E_cli_command_119_120(264).
	p(NT(263), (NT(264)));
//G408: __E_cli_command_119(262) => normalize_sym(263) __(27) normalize_cmd_arg(239).
	p(NT(262), (NT(263)+NT(27)+NT(239)));
//G409: normalize_cmd(261)   => __E_cli_command_119(262).
	p(NT(261), (NT(262)));
//G410: cli_command(214)     => normalize_cmd(261).
	p(NT(214), (NT(261)));
//G411: __E___E_cli_command_121_122(268) => 's'.
	p(NT(268), (T(12)));
//G412: __E___E_cli_command_121_122(268) => 's' 'u' 'b' 's' 't'.
	p(NT(268), (T(12)+T(41)+T(7)+T(12)+T(13)));
//G413: __E___E_cli_command_121_122(268) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(268), (T(12)+T(41)+T(7)+T(12)+T(13)+T(10)+T(13)+T(41)+T(13)+T(19)));
//G414: subst_sym(267)       => __E___E_cli_command_121_122(268).
	p(NT(267), (NT(268)));
//G415: __E_cli_command_121(266) => subst_sym(267) __(27) nf_cmd_arg(269) _(11) '[' _(11) nf_cmd_arg(269) _(11) '/' _(11) nf_cmd_arg(269) _(11) ']'.
	p(NT(266), (NT(267)+NT(27)+NT(269)+NT(11)+T(24)+NT(11)+NT(269)+NT(11)+T(57)+NT(11)+NT(269)+NT(11)+T(25)));
//G416: subst_cmd(265)       => __E_cli_command_121(266).
	p(NT(265), (NT(266)));
//G417: cli_command(214)     => subst_cmd(265).
	p(NT(214), (NT(265)));
//G418: __E___E_cli_command_123_124(273) => 'i'.
	p(NT(273), (T(10)));
//G419: __E___E_cli_command_123_124(273) => 'i' 'n' 's' 't'.
	p(NT(273), (T(10)+T(51)+T(12)+T(13)));
//G420: __E___E_cli_command_123_124(273) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(273), (T(10)+T(51)+T(12)+T(13)+T(5)+T(51)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G421: inst_sym(272)        => __E___E_cli_command_123_124(273).
	p(NT(272), (NT(273)));
//G422: __E_cli_command_123(271) => inst_sym(272) __(27) inst_args(274).
	p(NT(271), (NT(272)+NT(27)+NT(274)));
//G423: inst_cmd(270)        => __E_cli_command_123(271).
	p(NT(270), (NT(271)));
//G424: cli_command(214)     => inst_cmd(270).
	p(NT(214), (NT(270)));
//G425: dnf_sym(277)         => 'd' 'n' 'f'.
	p(NT(277), (T(54)+T(51)+T(4)));
//G426: __E_cli_command_125(276) => dnf_sym(277) __(27) nf_cmd_arg(269).
	p(NT(276), (NT(277)+NT(27)+NT(269)));
//G427: dnf_cmd(275)         => __E_cli_command_125(276).
	p(NT(275), (NT(276)));
//G428: cli_command(214)     => dnf_cmd(275).
	p(NT(214), (NT(275)));
//G429: cnf_sym(280)         => 'c' 'n' 'f'.
	p(NT(280), (T(8)+T(51)+T(4)));
//G430: __E_cli_command_126(279) => cnf_sym(280) __(27) nf_cmd_arg(269).
	p(NT(279), (NT(280)+NT(27)+NT(269)));
//G431: cnf_cmd(278)         => __E_cli_command_126(279).
	p(NT(278), (NT(279)));
//G432: cli_command(214)     => cnf_cmd(278).
	p(NT(214), (NT(278)));
//G433: anf_sym(283)         => 'a' 'n' 'f'.
	p(NT(283), (T(5)+T(51)+T(4)));
//G434: __E_cli_command_127(282) => anf_sym(283) __(27) nf_cmd_arg(269).
	p(NT(282), (NT(283)+NT(27)+NT(269)));
//G435: anf_cmd(281)         => __E_cli_command_127(282).
	p(NT(281), (NT(282)));
//G436: cli_command(214)     => anf_cmd(281).
	p(NT(214), (NT(281)));
//G437: nnf_sym(286)         => 'n' 'n' 'f'.
	p(NT(286), (T(51)+T(51)+T(4)));
//G438: __E_cli_command_128(285) => nnf_sym(286) __(27) nf_cmd_arg(269).
	p(NT(285), (NT(286)+NT(27)+NT(269)));
//G439: nnf_cmd(284)         => __E_cli_command_128(285).
	p(NT(284), (NT(285)));
//G440: cli_command(214)     => nnf_cmd(284).
	p(NT(214), (NT(284)));
//G441: pnf_sym(289)         => 'p' 'n' 'f'.
	p(NT(289), (T(53)+T(51)+T(4)));
//G442: __E_cli_command_129(288) => pnf_sym(289) __(27) nf_cmd_arg(269).
	p(NT(288), (NT(289)+NT(27)+NT(269)));
//G443: pnf_cmd(287)         => __E_cli_command_129(288).
	p(NT(287), (NT(288)));
//G444: cli_command(214)     => pnf_cmd(287).
	p(NT(214), (NT(287)));
//G445: mnf_sym(292)         => 'm' 'n' 'f'.
	p(NT(292), (T(18)+T(51)+T(4)));
//G446: __E_cli_command_130(291) => mnf_sym(292) __(27) nf_cmd_arg(269).
	p(NT(291), (NT(292)+NT(27)+NT(269)));
//G447: mnf_cmd(290)         => __E_cli_command_130(291).
	p(NT(290), (NT(291)));
//G448: cli_command(214)     => mnf_cmd(290).
	p(NT(214), (NT(290)));
//G449: snf_sym(295)         => 's' 'n' 'f'.
	p(NT(295), (T(12)+T(51)+T(4)));
//G450: __E_cli_command_131(294) => snf_sym(295) __(27) nf_cmd_arg(269).
	p(NT(294), (NT(295)+NT(27)+NT(269)));
//G451: snf_cmd(293)         => __E_cli_command_131(294).
	p(NT(293), (NT(294)));
//G452: cli_command(214)     => snf_cmd(293).
	p(NT(214), (NT(293)));
//G453: onf_sym(298)         => 'o' 'n' 'f'.
	p(NT(298), (T(17)+T(51)+T(4)));
//G454: __E_cli_command_132(297) => onf_sym(298) __(27) variable(118) __(27) onf_cmd_arg(299).
	p(NT(297), (NT(298)+NT(27)+NT(118)+NT(27)+NT(299)));
//G455: onf_cmd(296)         => __E_cli_command_132(297).
	p(NT(296), (NT(297)));
//G456: cli_command(214)     => onf_cmd(296).
	p(NT(214), (NT(296)));
//G457: __E___E_cli_command_133_134(303) => 'd' 'e' 'f' 's'.
	p(NT(303), (T(54)+T(19)+T(4)+T(12)));
//G458: __E___E_cli_command_133_134(303) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(303), (T(54)+T(19)+T(4)+T(10)+T(51)+T(10)+T(13)+T(10)+T(17)+T(51)+T(12)));
//G459: def_sym(302)         => __E___E_cli_command_133_134(303).
	p(NT(302), (NT(303)));
//G460: __E_cli_command_133(301) => def_sym(302).
	p(NT(301), (NT(302)));
//G461: def_list_cmd(300)    => __E_cli_command_133(301).
	p(NT(300), (NT(301)));
//G462: cli_command(214)     => def_list_cmd(300).
	p(NT(214), (NT(300)));
//G463: __E_cli_command_135(305) => def_sym(302) __(27) number(306).
	p(NT(305), (NT(302)+NT(27)+NT(306)));
//G464: def_print_cmd(304)   => __E_cli_command_135(305).
	p(NT(304), (NT(305)));
//G465: cli_command(214)     => def_print_cmd(304).
	p(NT(214), (NT(304)));
//G466: def_rr_cmd(307)      => rec_relation(16).
	p(NT(307), (NT(16)));
//G467: cli_command(214)     => def_rr_cmd(307).
	p(NT(214), (NT(307)));
//G468: qelim_sym(310)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(310), (T(49)+T(19)+T(6)+T(10)+T(18)));
//G469: __E_cli_command_136(309) => qelim_sym(310) __(27) wff_cmd_arg(251).
	p(NT(309), (NT(310)+NT(27)+NT(251)));
//G470: qelim_cmd(308)       => __E_cli_command_136(309).
	p(NT(308), (NT(309)));
//G471: cli_command(214)     => qelim_cmd(308).
	p(NT(214), (NT(308)));
//G472: get_sym(313)         => 'g' 'e' 't'.
	p(NT(313), (T(55)+T(19)+T(13)));
//G473: __E___E_cli_command_137_138(314) => __(27) option_name(315).
	p(NT(314), (NT(27)+NT(315)));
//G474: __E___E_cli_command_137_138(314) => null.
	p(NT(314), (nul));
//G475: __E_cli_command_137(312) => get_sym(313) __E___E_cli_command_137_138(314).
	p(NT(312), (NT(313)+NT(314)));
//G476: get_cmd(311)         => __E_cli_command_137(312).
	p(NT(311), (NT(312)));
//G477: cli_command(214)     => get_cmd(311).
	p(NT(214), (NT(311)));
//G478: set_sym(318)         => 's' 'e' 't'.
	p(NT(318), (T(12)+T(19)+T(13)));
//G479: __E___E_cli_command_139_140(319) => __(27).
	p(NT(319), (NT(27)));
//G480: __E___E_cli_command_139_140(319) => _(11) '=' _(11).
	p(NT(319), (NT(11)+T(3)+NT(11)));
//G481: __E_cli_command_139(317) => set_sym(318) __(27) option_name(315) __E___E_cli_command_139_140(319) option_value(320).
	p(NT(317), (NT(318)+NT(27)+NT(315)+NT(319)+NT(320)));
//G482: set_cmd(316)         => __E_cli_command_139(317).
	p(NT(316), (NT(317)));
//G483: cli_command(214)     => set_cmd(316).
	p(NT(214), (NT(316)));
//G484: enable_sym(323)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(323), (T(19)+T(51)+T(5)+T(7)+T(6)+T(19)));
//G485: __E_cli_command_141(322) => enable_sym(323) __(27) option_name(315).
	p(NT(322), (NT(323)+NT(27)+NT(315)));
//G486: enable_cmd(321)      => __E_cli_command_141(322).
	p(NT(321), (NT(322)));
//G487: cli_command(214)     => enable_cmd(321).
	p(NT(214), (NT(321)));
//G488: disable_sym(326)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(326), (T(54)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G489: __E_cli_command_142(325) => disable_sym(326) __(27) option_name(315).
	p(NT(325), (NT(326)+NT(27)+NT(315)));
//G490: disable_cmd(324)     => __E_cli_command_142(325).
	p(NT(324), (NT(325)));
//G491: cli_command(214)     => disable_cmd(324).
	p(NT(214), (NT(324)));
//G492: toggle_sym(329)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(329), (T(13)+T(17)+T(55)+T(55)+T(6)+T(19)));
//G493: __E_cli_command_143(328) => toggle_sym(329) __(27) option_name(315).
	p(NT(328), (NT(329)+NT(27)+NT(315)));
//G494: toggle_cmd(327)      => __E_cli_command_143(328).
	p(NT(327), (NT(328)));
//G495: cli_command(214)     => toggle_cmd(327).
	p(NT(214), (NT(327)));
//G496: __E_cli_command_144(331) => type(129) __(27) in_var_name(170) _(11) '=' _(11) input_stream(332).
	p(NT(331), (NT(129)+NT(27)+NT(170)+NT(11)+T(3)+NT(11)+NT(332)));
//G497: def_input_cmd(330)   => __E_cli_command_144(331).
	p(NT(330), (NT(331)));
//G498: cli_command(214)     => def_input_cmd(330).
	p(NT(214), (NT(330)));
//G499: __E_cli_command_145(334) => type(129) __(27) out_var_name(174) _(11) '=' _(11) output_stream(335).
	p(NT(334), (NT(129)+NT(27)+NT(174)+NT(11)+T(3)+NT(11)+NT(335)));
//G500: def_output_cmd(333)  => __E_cli_command_145(334).
	p(NT(333), (NT(334)));
//G501: cli_command(214)     => def_output_cmd(333).
	p(NT(214), (NT(333)));
//G502: __E___E_cli_command_146_147(339) => 'h' 'i' 's' 't'.
	p(NT(339), (T(52)+T(10)+T(12)+T(13)));
//G503: __E___E_cli_command_146_147(339) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(339), (T(52)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G504: history_sym(338)     => __E___E_cli_command_146_147(339).
	p(NT(338), (NT(339)));
//G505: __E_cli_command_146(337) => history_sym(338).
	p(NT(337), (NT(338)));
//G506: history_list_cmd(336) => __E_cli_command_146(337).
	p(NT(336), (NT(337)));
//G507: cli_command(214)     => history_list_cmd(336).
	p(NT(214), (NT(336)));
//G508: __E_cli_command_148(341) => history_sym(338) __(27) memory(260).
	p(NT(341), (NT(338)+NT(27)+NT(260)));
//G509: history_print_cmd(340) => __E_cli_command_148(341).
	p(NT(340), (NT(341)));
//G510: cli_command(214)     => history_print_cmd(340).
	p(NT(214), (NT(340)));
//G511: __E_cli_command_149(343) => wff(14).
	p(NT(343), (NT(14)));
//G512: __E_cli_command_149(343) => bf(21).
	p(NT(343), (NT(21)));
//G513: history_store_cmd(342) => __E_cli_command_149(343).
	p(NT(342), (NT(343)));
//G514: cli_command(214)     => history_store_cmd(342).
	p(NT(214), (NT(342)));
//G515: __E_solve_options_150(344) => __(27) solve_option(345).
	p(NT(344), (NT(27)+NT(345)));
//G516: __E_solve_options_151(346) => null.
	p(NT(346), (nul));
//G517: __E_solve_options_151(346) => __E_solve_options_150(344) __E_solve_options_151(346).
	p(NT(346), (NT(344)+NT(346)));
//G518: solve_options(250)   => __E_solve_options_151(346).
	p(NT(250), (NT(346)));
//G519: __E_solve_option_152(347) => '-' '-' solver_mode(348).
	p(NT(347), (T(28)+T(28)+NT(348)));
//G520: solve_option(345)    => __E_solve_option_152(347).
	p(NT(345), (NT(347)));
//G521: __E_solve_option_153(349) => '-' '-' type(129).
	p(NT(349), (T(28)+T(28)+NT(129)));
//G522: solve_option(345)    => __E_solve_option_153(349).
	p(NT(345), (NT(349)));
//G523: __E_solver_mode_154(351) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(351), (T(18)+T(10)+T(51)+T(10)+T(18)+T(41)+T(18)));
//G524: __E_solver_mode_154(351) => 'm' 'i' 'n'.
	p(NT(351), (T(18)+T(10)+T(51)));
//G525: solver_mode_minimum(350) => __E_solver_mode_154(351).
	p(NT(350), (NT(351)));
//G526: solver_mode(348)     => solver_mode_minimum(350).
	p(NT(348), (NT(350)));
//G527: __E_solver_mode_155(353) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(353), (T(18)+T(5)+T(27)+T(10)+T(18)+T(41)+T(18)));
//G528: __E_solver_mode_155(353) => 'm' 'a' 'x'.
	p(NT(353), (T(18)+T(5)+T(27)));
//G529: solver_mode_maximum(352) => __E_solver_mode_155(353).
	p(NT(352), (NT(353)));
//G530: solver_mode(348)     => solver_mode_maximum(352).
	p(NT(348), (NT(352)));
//G531: __E_input_stream_156(354) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(355) _(11) ')'.
	p(NT(354), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(355)+NT(11)+T(16)));
//G532: input_stream(332)    => __E_input_stream_156(354).
	p(NT(332), (NT(354)));
//G533: console_sym(356)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(356), (T(8)+T(17)+T(51)+T(12)+T(17)+T(6)+T(19)));
//G534: input_stream(332)    => console_sym(356).
	p(NT(332), (NT(356)));
//G535: __E_output_stream_157(357) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(355) _(11) ')'.
	p(NT(357), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(355)+NT(11)+T(16)));
//G536: output_stream(335)   => __E_output_stream_157(357).
	p(NT(335), (NT(357)));
//G537: output_stream(335)   => console_sym(356).
	p(NT(335), (NT(356)));
//G538: q_file_name(355)     => '"' file_name(358) '"'.
	p(NT(355), (T(58)+NT(358)+T(58)));
//G539: __E_file_name_158(359) => printable(8).
	p(NT(359), (NT(8)));
//G540: __E_file_name_158(359) => printable(8) __E_file_name_158(359).
	p(NT(359), (NT(8)+NT(359)));
//G541: file_name(358)       => __E_file_name_158(359).
	p(NT(358), (NT(359)));
//G542: __E_option_name_159(360) => alnum(6).
	p(NT(360), (NT(6)));
//G543: __E_option_name_159(360) => alnum(6) __E_option_name_159(360).
	p(NT(360), (NT(6)+NT(360)));
//G544: option_name(315)     => __E_option_name_159(360).
	p(NT(315), (NT(360)));
//G545: __E_option_value_160(361) => alnum(6).
	p(NT(361), (NT(6)));
//G546: __E_option_value_160(361) => alnum(6) __E_option_value_160(361).
	p(NT(361), (NT(6)+NT(361)));
//G547: option_value(320)    => __E_option_value_160(361).
	p(NT(320), (NT(361)));
//G548: number(306)          => digits(188).
	p(NT(306), (NT(188)));
//G549: bf_cmd_arg(362)      => memory(260).
	p(NT(362), (NT(260)));
//G550: bf_cmd_arg(362)      => bf(21).
	p(NT(362), (NT(21)));
//G551: wff_cmd_arg(251)     => memory(260).
	p(NT(251), (NT(260)));
//G552: wff_cmd_arg(251)     => wff(14).
	p(NT(251), (NT(14)));
//G553: nf_cmd_arg(269)      => memory(260).
	p(NT(269), (NT(260)));
//G554: nf_cmd_arg(269)      => ref(18).
	p(NT(269), (NT(18)));
//G555: nf_cmd_arg(269)      => wff(14).
	p(NT(269), (NT(14)));
//G556: nf_cmd_arg(269)      => bf(21).
	p(NT(269), (NT(21)));
//G557: onf_cmd_arg(299)     => memory(260).
	p(NT(299), (NT(260)));
//G558: onf_cmd_arg(299)     => wff(14).
	p(NT(299), (NT(14)));
//G559: normalize_cmd_arg(239) => memory(260).
	p(NT(239), (NT(260)));
//G560: normalize_cmd_arg(239) => rr(10).
	p(NT(239), (NT(10)));
//G561: normalize_cmd_arg(239) => ref(18).
	p(NT(239), (NT(18)));
//G562: normalize_cmd_arg(239) => wff(14).
	p(NT(239), (NT(14)));
//G563: normalize_cmd_arg(239) => bf(21).
	p(NT(239), (NT(21)));
//G564: inst_args(274)       => wff_cmd_arg(251) _(11) '[' _(11) variable(118) _(11) '/' _(11) bf_cmd_arg(362) _(11) ']'.
	p(NT(274), (NT(251)+NT(11)+T(24)+NT(11)+NT(118)+NT(11)+T(57)+NT(11)+NT(362)+NT(11)+T(25)));
//G565: inst_args(274)       => bf_cmd_arg(362) _(11) '[' _(11) variable(118) _(11) '/' _(11) bf_cmd_arg(362) _(11) ']'.
	p(NT(274), (NT(362)+NT(11)+T(24)+NT(11)+NT(118)+NT(11)+T(57)+NT(11)+NT(362)+NT(11)+T(25)));
//G566: help_arg(231)        => help_sym(228).
	p(NT(231), (NT(228)));
//G567: help_arg(231)        => version_sym(221).
	p(NT(231), (NT(221)));
//G568: help_arg(231)        => quit_sym(218).
	p(NT(231), (NT(218)));
//G569: help_arg(231)        => clear_sym(224).
	p(NT(231), (NT(224)));
//G570: help_arg(231)        => get_sym(313).
	p(NT(231), (NT(313)));
//G571: help_arg(231)        => set_sym(318).
	p(NT(231), (NT(318)));
//G572: help_arg(231)        => enable_sym(323).
	p(NT(231), (NT(323)));
//G573: help_arg(231)        => disable_sym(326).
	p(NT(231), (NT(326)));
//G574: help_arg(231)        => toggle_sym(329).
	p(NT(231), (NT(329)));
//G575: help_arg(231)        => file_sym(234).
	p(NT(231), (NT(234)));
//G576: help_arg(231)        => history_sym(338).
	p(NT(231), (NT(338)));
//G577: help_arg(231)        => abs_memory_sym(363).
	p(NT(231), (NT(363)));
//G578: help_arg(231)        => rel_memory_sym(364).
	p(NT(231), (NT(364)));
//G579: help_arg(231)        => selection_sym(365).
	p(NT(231), (NT(365)));
//G580: help_arg(231)        => def_sym(302).
	p(NT(231), (NT(302)));
//G581: help_arg(231)        => inst_sym(272).
	p(NT(231), (NT(272)));
//G582: help_arg(231)        => subst_sym(267).
	p(NT(231), (NT(267)));
//G583: help_arg(231)        => normalize_sym(263).
	p(NT(231), (NT(263)));
//G584: help_arg(231)        => execute_sym(366).
	p(NT(231), (NT(366)));
//G585: help_arg(231)        => solve_sym(248).
	p(NT(231), (NT(248)));
//G586: help_arg(231)        => valid_sym(238).
	p(NT(231), (NT(238)));
//G587: help_arg(231)        => sat_sym(242).
	p(NT(231), (NT(242)));
//G588: help_arg(231)        => unsat_sym(245).
	p(NT(231), (NT(245)));
//G589: help_arg(231)        => run_sym(257).
	p(NT(231), (NT(257)));
//G590: help_arg(231)        => dnf_sym(277).
	p(NT(231), (NT(277)));
//G591: help_arg(231)        => cnf_sym(280).
	p(NT(231), (NT(280)));
//G592: help_arg(231)        => anf_sym(283).
	p(NT(231), (NT(283)));
//G593: help_arg(231)        => snf_sym(295).
	p(NT(231), (NT(295)));
//G594: help_arg(231)        => nnf_sym(286).
	p(NT(231), (NT(286)));
//G595: help_arg(231)        => pnf_sym(289).
	p(NT(231), (NT(289)));
//G596: help_arg(231)        => mnf_sym(292).
	p(NT(231), (NT(292)));
//G597: help_arg(231)        => onf_sym(298).
	p(NT(231), (NT(298)));
//G598: help_arg(231)        => qelim_sym(310).
	p(NT(231), (NT(310)));
//G599: __E___E_help_arg_161_162(369) => 's'.
	p(NT(369), (T(12)));
//G600: __E___E_help_arg_161_162(369) => null.
	p(NT(369), (nul));
//G601: __E_help_arg_161(368) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_161_162(369).
	p(NT(368), (T(19)+T(27)+T(5)+T(18)+T(53)+T(6)+T(19)+NT(369)));
//G602: examples_sym(367)    => __E_help_arg_161(368).
	p(NT(367), (NT(368)));
//G603: help_arg(231)        => examples_sym(367).
	p(NT(231), (NT(367)));
//G604: __E_memory_163(371)  => '%'.
	p(NT(371), (T(59)));
//G605: rel_memory_sym(364)  => '%' '-'.
	p(NT(364), (T(59)+T(28)));
//G606: memory_id(373)       => digits(188).
	p(NT(373), (NT(188)));
//G607: __E___E_memory_163_164(372) => memory_id(373).
	p(NT(372), (NT(373)));
//G608: __E___E_memory_163_164(372) => null.
	p(NT(372), (nul));
//G609: __E_memory_163(371)  => rel_memory_sym(364) __E___E_memory_163_164(372).
	p(NT(371), (NT(364)+NT(372)));
//G610: rel_memory(370)      => __E_memory_163(371).
	p(NT(370), (NT(371)));
//G611: memory(260)          => rel_memory(370).
	p(NT(260), (NT(370)));
//G612: abs_memory_sym(363)  => '%'.
	p(NT(363), (T(59)));
//G613: __E_memory_165(375)  => abs_memory_sym(363) memory_id(373).
	p(NT(375), (NT(363)+NT(373)));
//G614: abs_memory(374)      => __E_memory_165(375).
	p(NT(374), (NT(375)));
//G615: memory(260)          => abs_memory(374).
	p(NT(260), (NT(374)));
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
		__E_q_vars_76, __E_q_vars_77, __E_offsets_78, __E_offsets_79, shift, __E_offset_80, __E_shift_81, __E___E_shift_81_82, digits, uninter_const_name, 
		__E_uninterpreted_constant_83, __E___E_uninterpreted_constant_83_84, constant, __E_bf_constant_85, binding, __E_bf_constant_86, source, __E_binding_87, src_c, __E___E_binding_87_88, 
		__E___E___E_binding_87_88_89, __E___E___E_binding_87_88_90, __E_src_c_91, __E_src_c_92, __E_chars_93, __E_chars_94, __E_digits_95, comment, __E_comment_96, __E_comment_97, 
		__E_comment_98, __E____99, __E___100, cli, cli_command, __E_cli_101, __E_cli_102, quit_cmd, quit_sym, __E_cli_command_103, 
		version_cmd, version_sym, __E_cli_command_104, clear_cmd, clear_sym, __E_cli_command_105, help_cmd, __E_cli_command_106, help_sym, __E___E_cli_command_106_107, 
		__E___E_cli_command_106_108, help_arg, file_cmd, __E_cli_command_109, file_sym, q_string, valid_cmd, __E_cli_command_110, valid_sym, normalize_cmd_arg, 
		sat_cmd, __E_cli_command_111, sat_sym, unsat_cmd, __E_cli_command_112, unsat_sym, solve_cmd, __E_cli_command_113, solve_sym, __E___E_cli_command_113_114, 
		solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_115, lgrs_sym, run_cmd, __E_cli_command_116, run_sym, __E___E_cli_command_116_117, __E___E_cli_command_116_118, 
		memory, normalize_cmd, __E_cli_command_119, normalize_sym, __E___E_cli_command_119_120, subst_cmd, __E_cli_command_121, subst_sym, __E___E_cli_command_121_122, nf_cmd_arg, 
		inst_cmd, __E_cli_command_123, inst_sym, __E___E_cli_command_123_124, inst_args, dnf_cmd, __E_cli_command_125, dnf_sym, cnf_cmd, __E_cli_command_126, 
		cnf_sym, anf_cmd, __E_cli_command_127, anf_sym, nnf_cmd, __E_cli_command_128, nnf_sym, pnf_cmd, __E_cli_command_129, pnf_sym, 
		mnf_cmd, __E_cli_command_130, mnf_sym, snf_cmd, __E_cli_command_131, snf_sym, onf_cmd, __E_cli_command_132, onf_sym, onf_cmd_arg, 
		def_list_cmd, __E_cli_command_133, def_sym, __E___E_cli_command_133_134, def_print_cmd, __E_cli_command_135, number, def_rr_cmd, qelim_cmd, __E_cli_command_136, 
		qelim_sym, get_cmd, __E_cli_command_137, get_sym, __E___E_cli_command_137_138, option_name, set_cmd, __E_cli_command_139, set_sym, __E___E_cli_command_139_140, 
		option_value, enable_cmd, __E_cli_command_141, enable_sym, disable_cmd, __E_cli_command_142, disable_sym, toggle_cmd, __E_cli_command_143, toggle_sym, 
		def_input_cmd, __E_cli_command_144, input_stream, def_output_cmd, __E_cli_command_145, output_stream, history_list_cmd, __E_cli_command_146, history_sym, __E___E_cli_command_146_147, 
		history_print_cmd, __E_cli_command_148, history_store_cmd, __E_cli_command_149, __E_solve_options_150, solve_option, __E_solve_options_151, __E_solve_option_152, solver_mode, __E_solve_option_153, 
		solver_mode_minimum, __E_solver_mode_154, solver_mode_maximum, __E_solver_mode_155, __E_input_stream_156, q_file_name, console_sym, __E_output_stream_157, file_name, __E_file_name_158, 
		__E_option_name_159, __E_option_value_160, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, execute_sym, examples_sym, __E_help_arg_161, __E___E_help_arg_161_162, 
		rel_memory, __E_memory_163, __E___E_memory_163_164, memory_id, abs_memory, __E_memory_165, named_binding, __N_0, __N_1, __N_2, 
		__N_3, __N_4, __N_5, __N_6, __N_7, __N_8, 
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
