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
	"__E_wff_41", "bf_less", "__E_wff_42", "bf_nless", "__E_wff_43", "z3_eq", "__E_wff_44", "z3", "z3_neq", "__E_wff_45", 
	"z3_less_equal", "__E_wff_46", "z3_nleq", "__E_wff_47", "z3_greater", "__E_wff_48", "z3_ngreater", "__E_wff_49", "z3_greater_equal", "__E_wff_50", 
	"z3_ngeq", "__E_wff_51", "z3_less", "__E_wff_52", "z3_nless", "__E_wff_53", "bf_parenthesis", "__E_bf_54", "bf_constant", "variable", 
	"bf_splitter", "__E_bf_55", "bf_ref", "bf_or", "__E_bf_56", "bf_xor", "__E_bf_57", "bf_t", "__E_bf_58", "__E___E_bf_58_59", 
	"type", "bf_f", "__E_bf_60", "__E___E_bf_60_61", "bf_and", "__E_bf_62", "__E___E_bf_62_63", "bf_neg", "__E_bf_64", "bf_neg_oprnd", 
	"__E___E_bf_64_65", "bf_and_nosep", "__E_bf_66", "bf_and_nosep_1st_oprnd", "__E___E_bf_66_67", "bf_and_nosep_2nd_oprnd", "__E___E_bf_66_68", "z3_parenthesis", "__E_z3_69", "z3_neg", 
	"z3_add", "__E_z3_70", "z3_sub", "__E_z3_71", "z3_mul", "__E_z3_72", "z3_div", "__E_z3_73", "z3_mod", "__E_z3_74", 
	"z3_nand", "__E_z3_75", "z3_and", "__E_z3_76", "z3_nor", "__E_z3_77", "z3_or", "__E_z3_78", "z3_xnor", "__E_z3_79", 
	"z3_xor", "__E_z3_80", "z3_rotate_left", "__E_z3_81", "num", "z3_rotate_right", "__E_z3_82", "z3_min", "__E_z3_83", "z3_max", 
	"__E_z3_84", "bitvector", "bits", "__E_bitvector_85", "__E_bitvector_86", "hexnum", "__E_bitvector_87", "__E_bits_88", "__E_bits_89", "__E_hexnum_90", 
	"ctn_neq", "__E_constraint_91", "ctnvar", "ctn_eq", "__E_constraint_92", "ctn_greater_equal", "__E_constraint_93", "ctn_greater", "__E_constraint_94", "ctn_less_equal", 
	"__E_constraint_95", "ctn_less", "__E_constraint_96", "__E_variable_97", "__E_variable_98", "__E_variable_99", "chars", "__E_variable_100", "__E_variable_101", "__E_variable_102", 
	"__E_variable_103", "io_var", "__E_variable_104", "in", "out", "__E_variable_105", "uninterpreted_constant", "in_var_name", "__E_in_106", "__E___E_in_106_107", 
	"offset", "out_var_name", "__E_out_108", "__E___E_out_108_109", "__E___E___E_out_108_109_110", "q_var", "__E_q_vars_111", "__E_q_vars_112", "__E_q_var_113", "__E_offsets_114", 
	"__E_offsets_115", "integer", "shift", "__E_offset_116", "__E_shift_117", "__E___E_shift_117_118", "digits", "__E_integer_119", "uninter_const_name", "__E_uninterpreted_constant_120", 
	"__E___E_uninterpreted_constant_120_121", "constant", "__E_bf_constant_122", "binding", "__E_bf_constant_123", "source", "__E_binding_124", "src_c", "__E___E_binding_124_125", "__E___E___E_binding_124_125_126", 
	"__E___E___E_binding_124_125_127", "__E_src_c_128", "__E_src_c_129", "__E_chars_130", "__E_chars_131", "__E_digits_132", "comment", "__E_comment_133", "__E_comment_134", "__E_comment_135", 
	"__E____136", "__E___137", "cli", "cli_command", "__E_cli_138", "__E_cli_139", "quit_cmd", "quit_sym", "__E_cli_command_140", "version_cmd", 
	"version_sym", "__E_cli_command_141", "clear_cmd", "clear_sym", "__E_cli_command_142", "help_cmd", "__E_cli_command_143", "help_sym", "__E___E_cli_command_143_144", "__E___E_cli_command_143_145", 
	"help_arg", "file_cmd", "__E_cli_command_146", "file_sym", "q_string", "valid_cmd", "__E_cli_command_147", "valid_sym", "normalize_cmd_arg", "sat_cmd", 
	"__E_cli_command_148", "sat_sym", "unsat_cmd", "__E_cli_command_149", "unsat_sym", "solve_cmd", "__E_cli_command_150", "solve_sym", "__E___E_cli_command_150_151", "solve_options", 
	"wff_cmd_arg", "lgrs_cmd", "__E_cli_command_152", "lgrs_sym", "run_cmd", "__E_cli_command_153", "run_sym", "__E___E_cli_command_153_154", "__E___E_cli_command_153_155", "memory", 
	"normalize_cmd", "__E_cli_command_156", "normalize_sym", "__E___E_cli_command_156_157", "subst_cmd", "__E_cli_command_158", "subst_sym", "__E___E_cli_command_158_159", "nf_cmd_arg", "inst_cmd", 
	"__E_cli_command_160", "inst_sym", "__E___E_cli_command_160_161", "inst_args", "dnf_cmd", "__E_cli_command_162", "dnf_sym", "cnf_cmd", "__E_cli_command_163", "cnf_sym", 
	"anf_cmd", "__E_cli_command_164", "anf_sym", "nnf_cmd", "__E_cli_command_165", "nnf_sym", "pnf_cmd", "__E_cli_command_166", "pnf_sym", "mnf_cmd", 
	"__E_cli_command_167", "mnf_sym", "snf_cmd", "__E_cli_command_168", "snf_sym", "onf_cmd", "__E_cli_command_169", "onf_sym", "onf_cmd_arg", "def_list_cmd", 
	"__E_cli_command_170", "def_sym", "__E___E_cli_command_170_171", "def_print_cmd", "__E_cli_command_172", "number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_173", "qelim_sym", 
	"get_cmd", "__E_cli_command_174", "get_sym", "__E___E_cli_command_174_175", "option_name", "set_cmd", "__E_cli_command_176", "set_sym", "__E___E_cli_command_176_177", "option_value", 
	"enable_cmd", "__E_cli_command_178", "enable_sym", "disable_cmd", "__E_cli_command_179", "disable_sym", "toggle_cmd", "__E_cli_command_180", "toggle_sym", "def_input_cmd", 
	"__E_cli_command_181", "input_stream", "def_output_cmd", "__E_cli_command_182", "output_stream", "history_list_cmd", "__E_cli_command_183", "history_sym", "__E___E_cli_command_183_184", "history_print_cmd", 
	"__E_cli_command_185", "history_store_cmd", "__E_cli_command_186", "__E_solve_options_187", "solve_option", "__E_solve_options_188", "__E_solve_option_189", "solver_mode", "__E_solve_option_190", "solver_mode_minimum", 
	"__E_solver_mode_191", "solver_mode_maximum", "__E_solver_mode_192", "__E_input_stream_193", "q_file_name", "console_sym", "__E_output_stream_194", "file_name", "__E_file_name_195", "__E_option_name_196", 
	"__E_option_value_197", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "execute_sym", "examples_sym", "__E_help_arg_198", "__E___E_help_arg_198_199", "rel_memory", 
	"__E_memory_200", "__E___E_memory_200_201", "memory_id", "abs_memory", "__E_memory_202", "named_binding", "__N_0", "__N_1", "__N_2", "__N_3", 
	"__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'f', 'a', 'l', 'b', 'c', 
	'k', 'i', 'r', 's', 't', ',', '(', ')', 'o', 'm', 
	'e', '<', '>', 'w', 'y', '[', ']', '?', 'x', '-', 
	'|', '^', '&', '!', 'T', 'F', '_', 'S', '+', '1', 
	'0', '\'', '~', '*', '/', '%', 'n', '#', 'u', '$', 
	'{', '}', '\t', '\n', '\r', 'q', 'v', 'h', 'p', 'd', 
	'g', 'z', '"', 
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
			20, 22, 139, 150, 202, 205, 212, 237, 241, 251,
			256, 258, 265, 384, 389, 427, 445
		},
		.to_inline = {
			{ 14, 60, 14 },
			{ 21, 136, 21 },
			{ 117, 167, 117 },
			{ 159, 136, 21 },
			{ 161, 136, 154 },
			{ 163, 136, 21 },
			{ 165, 136, 21 },
			{ 226 },
			{ 245 },
			{ 267 },
			{ 308 },
			{ 320 },
			{ 338 },
			{ 343 },
			{ 368 },
			{ 431 }
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
//G145: __E_wff_44(116)      => z3(117) _(11) '=' '_' _(11) z3(117).
	p(NT(116), (NT(117)+NT(11)+T(3)+T(35)+NT(11)+NT(117)));
//G146: z3_eq(115)           => __E_wff_44(116).
	p(NT(115), (NT(116)));
//G147: wff(14)              => z3_eq(115).
	p(NT(14), (NT(115)));
//G148: __E_wff_45(119)      => z3(117) _(11) '!' '=' '_' _(11) z3(117).
	p(NT(119), (NT(117)+NT(11)+T(32)+T(3)+T(35)+NT(11)+NT(117)));
//G149: z3_neq(118)          => __E_wff_45(119).
	p(NT(118), (NT(119)));
//G150: wff(14)              => z3_neq(118).
	p(NT(14), (NT(118)));
//G151: __E_wff_46(121)      => z3(117) _(11) '<' '=' '_' _(11) z3(117).
	p(NT(121), (NT(117)+NT(11)+T(20)+T(3)+T(35)+NT(11)+NT(117)));
//G152: z3_less_equal(120)   => __E_wff_46(121).
	p(NT(120), (NT(121)));
//G153: wff(14)              => z3_less_equal(120).
	p(NT(14), (NT(120)));
//G154: __E_wff_47(123)      => z3(117) _(11) '!' '<' '=' '_' _(11) z3(117).
	p(NT(123), (NT(117)+NT(11)+T(32)+T(20)+T(3)+T(35)+NT(11)+NT(117)));
//G155: z3_nleq(122)         => __E_wff_47(123).
	p(NT(122), (NT(123)));
//G156: wff(14)              => z3_nleq(122).
	p(NT(14), (NT(122)));
//G157: __E_wff_48(125)      => z3(117) _(11) '>' '_' _(11) z3(117).
	p(NT(125), (NT(117)+NT(11)+T(21)+T(35)+NT(11)+NT(117)));
//G158: z3_greater(124)      => __E_wff_48(125).
	p(NT(124), (NT(125)));
//G159: wff(14)              => z3_greater(124).
	p(NT(14), (NT(124)));
//G160: __E_wff_49(127)      => z3(117) _(11) '!' '>' '_' _(11) z3(117).
	p(NT(127), (NT(117)+NT(11)+T(32)+T(21)+T(35)+NT(11)+NT(117)));
//G161: z3_ngreater(126)     => __E_wff_49(127).
	p(NT(126), (NT(127)));
//G162: wff(14)              => z3_ngreater(126).
	p(NT(14), (NT(126)));
//G163: __E_wff_50(129)      => z3(117) _(11) '>' '=' '_' _(11) z3(117).
	p(NT(129), (NT(117)+NT(11)+T(21)+T(3)+T(35)+NT(11)+NT(117)));
//G164: z3_greater_equal(128) => __E_wff_50(129).
	p(NT(128), (NT(129)));
//G165: wff(14)              => z3_greater_equal(128).
	p(NT(14), (NT(128)));
//G166: __E_wff_51(131)      => z3(117) _(11) '!' '>' '=' '_' _(11) z3(117).
	p(NT(131), (NT(117)+NT(11)+T(32)+T(21)+T(3)+T(35)+NT(11)+NT(117)));
//G167: z3_ngeq(130)         => __E_wff_51(131).
	p(NT(130), (NT(131)));
//G168: wff(14)              => z3_ngeq(130).
	p(NT(14), (NT(130)));
//G169: __E_wff_52(133)      => z3(117) _(11) '<' '_' _(11) z3(117).
	p(NT(133), (NT(117)+NT(11)+T(20)+T(35)+NT(11)+NT(117)));
//G170: z3_less(132)         => __E_wff_52(133).
	p(NT(132), (NT(133)));
//G171: wff(14)              => z3_less(132).
	p(NT(14), (NT(132)));
//G172: __E_wff_53(135)      => z3(117) _(11) '!' '<' '_' _(11) z3(117).
	p(NT(135), (NT(117)+NT(11)+T(32)+T(20)+T(35)+NT(11)+NT(117)));
//G173: z3_nless(134)        => __E_wff_53(135).
	p(NT(134), (NT(135)));
//G174: wff(14)              => z3_nless(134).
	p(NT(14), (NT(134)));
//G175: wff(14)              => capture(20).
	p(NT(14), (NT(20)));
//G176: __E_bf_54(137)       => '(' _(11) bf(21) _(11) ')'.
	p(NT(137), (T(15)+NT(11)+NT(21)+NT(11)+T(16)));
//G177: bf_parenthesis(136)  => __E_bf_54(137).
	p(NT(136), (NT(137)));
//G178: bf(21)               => bf_parenthesis(136).
	p(NT(21), (NT(136)));
//G179: bf(21)               => bf_constant(138).
	p(NT(21), (NT(138)));
//G180: bf(21)               => variable(139).
	p(NT(21), (NT(139)));
//G181: __E_bf_55(141)       => 'S' _(11) '(' _(11) bf(21) _(11) ')'.
	p(NT(141), (T(36)+NT(11)+T(15)+NT(11)+NT(21)+NT(11)+T(16)));
//G182: bf_splitter(140)     => __E_bf_55(141).
	p(NT(140), (NT(141)));
//G183: bf(21)               => bf_splitter(140).
	p(NT(21), (NT(140)));
//G184: bf_ref(142)          => ref(18).
	p(NT(142), (NT(18)));
//G185: bf(21)               => bf_ref(142).
	p(NT(21), (NT(142)));
//G186: __E_bf_56(144)       => bf(21) _(11) '|' _(11) bf(21).
	p(NT(144), (NT(21)+NT(11)+T(29)+NT(11)+NT(21)));
//G187: bf_or(143)           => __E_bf_56(144).
	p(NT(143), (NT(144)));
//G188: bf(21)               => bf_or(143).
	p(NT(21), (NT(143)));
//G189: __E_bf_57(146)       => bf(21) _(11) '+' _(11) bf(21).
	p(NT(146), (NT(21)+NT(11)+T(37)+NT(11)+NT(21)));
//G190: bf_xor(145)          => __E_bf_57(146).
	p(NT(145), (NT(146)));
//G191: bf(21)               => bf_xor(145).
	p(NT(21), (NT(145)));
//G192: __E___E_bf_58_59(149) => _(11) ':' _(11) type(150).
	p(NT(149), (NT(11)+T(2)+NT(11)+NT(150)));
//G193: __E___E_bf_58_59(149) => null.
	p(NT(149), (nul));
//G194: __E_bf_58(148)       => '1' __E___E_bf_58_59(149).
	p(NT(148), (T(38)+NT(149)));
//G195: bf_t(147)            => __E_bf_58(148).
	p(NT(147), (NT(148)));
//G196: bf(21)               => bf_t(147).
	p(NT(21), (NT(147)));
//G197: __E___E_bf_60_61(153) => _(11) ':' _(11) type(150).
	p(NT(153), (NT(11)+T(2)+NT(11)+NT(150)));
//G198: __E___E_bf_60_61(153) => null.
	p(NT(153), (nul));
//G199: __E_bf_60(152)       => '0' __E___E_bf_60_61(153).
	p(NT(152), (T(39)+NT(153)));
//G200: bf_f(151)            => __E_bf_60(152).
	p(NT(151), (NT(152)));
//G201: bf(21)               => bf_f(151).
	p(NT(21), (NT(151)));
//G202: __E___E_bf_62_63(156) => _(11) '&' _(11).
	p(NT(156), (NT(11)+T(31)+NT(11)));
//G203: __E___E_bf_62_63(156) => __(27).
	p(NT(156), (NT(27)));
//G204: __E_bf_62(155)       => bf(21) __E___E_bf_62_63(156) bf(21).
	p(NT(155), (NT(21)+NT(156)+NT(21)));
//G205: bf_and(154)          => __E_bf_62(155).
	p(NT(154), (NT(155)));
//G206: bf(21)               => bf_and(154).
	p(NT(21), (NT(154)));
//G207: __E___E_bf_64_65(160) => bf_parenthesis(136).
	p(NT(160), (NT(136)));
//G208: __E___E_bf_64_65(160) => bf_constant(138).
	p(NT(160), (NT(138)));
//G209: __E___E_bf_64_65(160) => variable(139).
	p(NT(160), (NT(139)));
//G210: __E___E_bf_64_65(160) => bf_splitter(140).
	p(NT(160), (NT(140)));
//G211: __E___E_bf_64_65(160) => bf_ref(142).
	p(NT(160), (NT(142)));
//G212: __E___E_bf_64_65(160) => bf_t(147).
	p(NT(160), (NT(147)));
//G213: __E___E_bf_64_65(160) => bf_f(151).
	p(NT(160), (NT(151)));
//G214: __E___E_bf_64_65(160) => bf_neg(157).
	p(NT(160), (NT(157)));
//G215: __E___E_bf_64_65(160) => capture(20).
	p(NT(160), (NT(20)));
//G216: bf_neg_oprnd(159)    => __E___E_bf_64_65(160).
	p(NT(159), (NT(160)));
//G217: __E_bf_64(158)       => bf_neg_oprnd(159) _(11) '\''.
	p(NT(158), (NT(159)+NT(11)+T(40)));
//G218: bf_neg(157)          => __E_bf_64(158).
	p(NT(157), (NT(158)));
//G219: bf(21)               => bf_neg(157).
	p(NT(21), (NT(157)));
//G220: __E___E_bf_66_67(164) => bf_parenthesis(136).
	p(NT(164), (NT(136)));
//G221: __E___E_bf_66_67(164) => variable(139).
	p(NT(164), (NT(139)));
//G222: __E___E_bf_66_67(164) => bf_splitter(140).
	p(NT(164), (NT(140)));
//G223: __E___E_bf_66_67(164) => bf_ref(142).
	p(NT(164), (NT(142)));
//G224: __E___E_bf_66_67(164) => bf_neg(157).
	p(NT(164), (NT(157)));
//G225: __E___E_bf_66_67(164) => bf_and_nosep(161).
	p(NT(164), (NT(161)));
//G226: __E___E_bf_66_67(164) => capture(20).
	p(NT(164), (NT(20)));
//G227: bf_and_nosep_1st_oprnd(163) => __E___E_bf_66_67(164).
	p(NT(163), (NT(164)));
//G228: __E___E_bf_66_68(166) => bf_parenthesis(136).
	p(NT(166), (NT(136)));
//G229: __E___E_bf_66_68(166) => bf_constant(138).
	p(NT(166), (NT(138)));
//G230: __E___E_bf_66_68(166) => variable(139).
	p(NT(166), (NT(139)));
//G231: __E___E_bf_66_68(166) => bf_splitter(140).
	p(NT(166), (NT(140)));
//G232: __E___E_bf_66_68(166) => bf_ref(142).
	p(NT(166), (NT(142)));
//G233: __E___E_bf_66_68(166) => bf_neg(157).
	p(NT(166), (NT(157)));
//G234: __E___E_bf_66_68(166) => capture(20).
	p(NT(166), (NT(20)));
//G235: bf_and_nosep_2nd_oprnd(165) => __E___E_bf_66_68(166).
	p(NT(165), (NT(166)));
//G236: __E_bf_66(162)       => bf_and_nosep_1st_oprnd(163) bf_and_nosep_2nd_oprnd(165).
	p(NT(162), (NT(163)+NT(165)));
//G237: bf_and_nosep(161)    => __E_bf_66(162).
	p(NT(161), (NT(162)));
//G238: bf(21)               => bf_and_nosep(161).
	p(NT(21), (NT(161)));
//G239: bf(21)               => capture(20).
	p(NT(21), (NT(20)));
//G240: __E_z3_69(168)       => '(' _(11) z3(117) _(11) ')'.
	p(NT(168), (T(15)+NT(11)+NT(117)+NT(11)+T(16)));
//G241: z3_parenthesis(167)  => __E_z3_69(168).
	p(NT(167), (NT(168)));
//G242: z3(117)              => z3_parenthesis(167).
	p(NT(117), (NT(167)));
//G243: z3(117)              => variable(139).
	p(NT(117), (NT(139)));
//G244: z3_neg(169)          => z3(117).
	p(NT(169), (NT(117)));
//G245: z3(117)              => '~' _(11) z3_neg(169).
	p(NT(117), (T(41)+NT(11)+NT(169)));
//G246: __E_z3_70(171)       => z3(117) _(11) '+' _(11) z3(117).
	p(NT(171), (NT(117)+NT(11)+T(37)+NT(11)+NT(117)));
//G247: z3_add(170)          => __E_z3_70(171).
	p(NT(170), (NT(171)));
//G248: z3(117)              => z3_add(170).
	p(NT(117), (NT(170)));
//G249: __E_z3_71(173)       => z3(117) _(11) '-' _(11) z3(117).
	p(NT(173), (NT(117)+NT(11)+T(28)+NT(11)+NT(117)));
//G250: z3_sub(172)          => __E_z3_71(173).
	p(NT(172), (NT(173)));
//G251: z3(117)              => z3_sub(172).
	p(NT(117), (NT(172)));
//G252: __E_z3_72(175)       => z3(117) _(11) '*' _(11) z3(117).
	p(NT(175), (NT(117)+NT(11)+T(42)+NT(11)+NT(117)));
//G253: z3_mul(174)          => __E_z3_72(175).
	p(NT(174), (NT(175)));
//G254: z3(117)              => z3_mul(174).
	p(NT(117), (NT(174)));
//G255: __E_z3_73(177)       => z3(117) _(11) '/' _(11) z3(117).
	p(NT(177), (NT(117)+NT(11)+T(43)+NT(11)+NT(117)));
//G256: z3_div(176)          => __E_z3_73(177).
	p(NT(176), (NT(177)));
//G257: z3(117)              => z3_div(176).
	p(NT(117), (NT(176)));
//G258: __E_z3_74(179)       => z3(117) _(11) '%' _(11) z3(117).
	p(NT(179), (NT(117)+NT(11)+T(44)+NT(11)+NT(117)));
//G259: z3_mod(178)          => __E_z3_74(179).
	p(NT(178), (NT(179)));
//G260: z3(117)              => z3_mod(178).
	p(NT(117), (NT(178)));
//G261: __E_z3_75(181)       => z3(117) _(11) '!' '&' _(11) z3(117).
	p(NT(181), (NT(117)+NT(11)+T(32)+T(31)+NT(11)+NT(117)));
//G262: z3_nand(180)         => __E_z3_75(181).
	p(NT(180), (NT(181)));
//G263: z3(117)              => z3_nand(180).
	p(NT(117), (NT(180)));
//G264: __E_z3_76(183)       => z3(117) _(11) '&' _(11) z3(117).
	p(NT(183), (NT(117)+NT(11)+T(31)+NT(11)+NT(117)));
//G265: z3_and(182)          => __E_z3_76(183).
	p(NT(182), (NT(183)));
//G266: z3(117)              => z3_and(182).
	p(NT(117), (NT(182)));
//G267: __E_z3_77(185)       => z3(117) _(11) '!' '|' _(11) z3(117).
	p(NT(185), (NT(117)+NT(11)+T(32)+T(29)+NT(11)+NT(117)));
//G268: z3_nor(184)          => __E_z3_77(185).
	p(NT(184), (NT(185)));
//G269: z3(117)              => z3_nor(184).
	p(NT(117), (NT(184)));
//G270: __E_z3_78(187)       => z3(117) _(11) '|' _(11) z3(117).
	p(NT(187), (NT(117)+NT(11)+T(29)+NT(11)+NT(117)));
//G271: z3_or(186)           => __E_z3_78(187).
	p(NT(186), (NT(187)));
//G272: z3(117)              => z3_or(186).
	p(NT(117), (NT(186)));
//G273: __E_z3_79(189)       => z3(117) _(11) '!' '(' '+' ')' _(11) z3(117).
	p(NT(189), (NT(117)+NT(11)+T(32)+T(15)+T(37)+T(16)+NT(11)+NT(117)));
//G274: z3_xnor(188)         => __E_z3_79(189).
	p(NT(188), (NT(189)));
//G275: z3(117)              => z3_xnor(188).
	p(NT(117), (NT(188)));
//G276: __E_z3_80(191)       => z3(117) _(11) '(' '+' ')' _(11) z3(117).
	p(NT(191), (NT(117)+NT(11)+T(15)+T(37)+T(16)+NT(11)+NT(117)));
//G277: z3_xor(190)          => __E_z3_80(191).
	p(NT(190), (NT(191)));
//G278: z3(117)              => z3_xor(190).
	p(NT(117), (NT(190)));
//G279: __E_z3_81(193)       => z3(117) _(11) '<' '<' _(11) num(194).
	p(NT(193), (NT(117)+NT(11)+T(20)+T(20)+NT(11)+NT(194)));
//G280: z3_rotate_left(192)  => __E_z3_81(193).
	p(NT(192), (NT(193)));
//G281: z3(117)              => z3_rotate_left(192).
	p(NT(117), (NT(192)));
//G282: __E_z3_82(196)       => z3(117) _(11) '>' '>' _(11) num(194).
	p(NT(196), (NT(117)+NT(11)+T(21)+T(21)+NT(11)+NT(194)));
//G283: z3_rotate_right(195) => __E_z3_82(196).
	p(NT(195), (NT(196)));
//G284: z3(117)              => z3_rotate_right(195).
	p(NT(117), (NT(195)));
//G285: __E_z3_83(198)       => 'm' 'i' 'n' __(27) z3(117) __(27) z3(117).
	p(NT(198), (T(18)+T(10)+T(45)+NT(27)+NT(117)+NT(27)+NT(117)));
//G286: z3_min(197)          => __E_z3_83(198).
	p(NT(197), (NT(198)));
//G287: z3(117)              => z3_min(197).
	p(NT(117), (NT(197)));
//G288: __E_z3_84(200)       => 'm' 'a' 'x' __(27) z3(117) __(27) z3(117).
	p(NT(200), (T(18)+T(5)+T(27)+NT(27)+NT(117)+NT(27)+NT(117)));
//G289: z3_max(199)          => __E_z3_84(200).
	p(NT(199), (NT(200)));
//G290: z3(117)              => z3_max(199).
	p(NT(117), (NT(199)));
//G291: z3(117)              => bitvector(201).
	p(NT(117), (NT(201)));
//G292: __E_bitvector_85(203) => _(11) ':' _(11) '[' _(11) type(150) _(11) ']'.
	p(NT(203), (NT(11)+T(2)+NT(11)+T(24)+NT(11)+NT(150)+NT(11)+T(25)));
//G293: __E_bitvector_85(203) => null.
	p(NT(203), (nul));
//G294: bitvector(201)       => '#' 'b' bits(202) __E_bitvector_85(203).
	p(NT(201), (T(46)+T(7)+NT(202)+NT(203)));
//G295: __E_bitvector_86(204) => _(11) ':' _(11) '[' _(11) type(150) _(11) ']'.
	p(NT(204), (NT(11)+T(2)+NT(11)+T(24)+NT(11)+NT(150)+NT(11)+T(25)));
//G296: __E_bitvector_86(204) => null.
	p(NT(204), (nul));
//G297: bitvector(201)       => num(194) __E_bitvector_86(204).
	p(NT(201), (NT(194)+NT(204)));
//G298: __E_bitvector_87(206) => _(11) ':' _(11) '[' _(11) type(150) _(11) ']'.
	p(NT(206), (NT(11)+T(2)+NT(11)+T(24)+NT(11)+NT(150)+NT(11)+T(25)));
//G299: __E_bitvector_87(206) => null.
	p(NT(206), (nul));
//G300: bitvector(201)       => '#' 'x' hexnum(205) __E_bitvector_87(206).
	p(NT(201), (T(46)+T(27)+NT(205)+NT(206)));
//G301: __E_bits_88(207)     => '0'.
	p(NT(207), (T(39)));
//G302: __E_bits_88(207)     => '1'.
	p(NT(207), (T(38)));
//G303: __E_bits_89(208)     => __E_bits_88(207).
	p(NT(208), (NT(207)));
//G304: __E_bits_89(208)     => __E_bits_88(207) __E_bits_89(208).
	p(NT(208), (NT(207)+NT(208)));
//G305: bits(202)            => __E_bits_89(208).
	p(NT(202), (NT(208)));
//G306: __E_hexnum_90(209)   => xdigit(4).
	p(NT(209), (NT(4)));
//G307: __E_hexnum_90(209)   => xdigit(4) __E_hexnum_90(209).
	p(NT(209), (NT(4)+NT(209)));
//G308: hexnum(205)          => __E_hexnum_90(209).
	p(NT(205), (NT(209)));
//G309: __E_constraint_91(211) => '[' ctnvar(212) _(11) '!' '=' _(11) num(194) ']'.
	p(NT(211), (T(24)+NT(212)+NT(11)+T(32)+T(3)+NT(11)+NT(194)+T(25)));
//G310: __E_constraint_91(211) => '[' num(194) _(11) '!' '=' _(11) ctnvar(212) ']'.
	p(NT(211), (T(24)+NT(194)+NT(11)+T(32)+T(3)+NT(11)+NT(212)+T(25)));
//G311: ctn_neq(210)         => __E_constraint_91(211).
	p(NT(210), (NT(211)));
//G312: constraint(76)       => ctn_neq(210).
	p(NT(76), (NT(210)));
//G313: __E_constraint_92(214) => '[' ctnvar(212) _(11) '=' _(11) num(194) ']'.
	p(NT(214), (T(24)+NT(212)+NT(11)+T(3)+NT(11)+NT(194)+T(25)));
//G314: __E_constraint_92(214) => '[' num(194) _(11) '=' _(11) ctnvar(212) ']'.
	p(NT(214), (T(24)+NT(194)+NT(11)+T(3)+NT(11)+NT(212)+T(25)));
//G315: ctn_eq(213)          => __E_constraint_92(214).
	p(NT(213), (NT(214)));
//G316: constraint(76)       => ctn_eq(213).
	p(NT(76), (NT(213)));
//G317: __E_constraint_93(216) => '[' ctnvar(212) _(11) '>' '=' _(11) num(194) ']'.
	p(NT(216), (T(24)+NT(212)+NT(11)+T(21)+T(3)+NT(11)+NT(194)+T(25)));
//G318: __E_constraint_93(216) => '[' num(194) _(11) '>' '=' _(11) ctnvar(212) ']'.
	p(NT(216), (T(24)+NT(194)+NT(11)+T(21)+T(3)+NT(11)+NT(212)+T(25)));
//G319: ctn_greater_equal(215) => __E_constraint_93(216).
	p(NT(215), (NT(216)));
//G320: constraint(76)       => ctn_greater_equal(215).
	p(NT(76), (NT(215)));
//G321: __E_constraint_94(218) => '[' ctnvar(212) _(11) '>' _(11) num(194) ']'.
	p(NT(218), (T(24)+NT(212)+NT(11)+T(21)+NT(11)+NT(194)+T(25)));
//G322: __E_constraint_94(218) => '[' num(194) _(11) '>' _(11) ctnvar(212) ']'.
	p(NT(218), (T(24)+NT(194)+NT(11)+T(21)+NT(11)+NT(212)+T(25)));
//G323: ctn_greater(217)     => __E_constraint_94(218).
	p(NT(217), (NT(218)));
//G324: constraint(76)       => ctn_greater(217).
	p(NT(76), (NT(217)));
//G325: __E_constraint_95(220) => '[' ctnvar(212) _(11) '<' '=' _(11) num(194) ']'.
	p(NT(220), (T(24)+NT(212)+NT(11)+T(20)+T(3)+NT(11)+NT(194)+T(25)));
//G326: __E_constraint_95(220) => '[' num(194) _(11) '<' '=' _(11) ctnvar(212) ']'.
	p(NT(220), (T(24)+NT(194)+NT(11)+T(20)+T(3)+NT(11)+NT(212)+T(25)));
//G327: ctn_less_equal(219)  => __E_constraint_95(220).
	p(NT(219), (NT(220)));
//G328: constraint(76)       => ctn_less_equal(219).
	p(NT(76), (NT(219)));
//G329: __E_constraint_96(222) => '[' ctnvar(212) _(11) '<' _(11) num(194) ']'.
	p(NT(222), (T(24)+NT(212)+NT(11)+T(20)+NT(11)+NT(194)+T(25)));
//G330: __E_constraint_96(222) => '[' num(194) _(11) '<' _(11) ctnvar(212) ']'.
	p(NT(222), (T(24)+NT(194)+NT(11)+T(20)+NT(11)+NT(212)+T(25)));
//G331: ctn_less(221)        => __E_constraint_96(222).
	p(NT(221), (NT(222)));
//G332: constraint(76)       => ctn_less(221).
	p(NT(76), (NT(221)));
//G333: __N_0(446)           => 'F'.
	p(NT(446), (T(34)));
//G334: __N_1(447)           => 'T'.
	p(NT(447), (T(33)));
//G335: __E_variable_97(223) => ~( __N_0(446) ) & ~( __N_1(447) ) & alpha(5).	 # conjunctive
	p(NT(223), ~(NT(446)) & ~(NT(447)) & (NT(5)));
//G336: __E_variable_98(224) => null.
	p(NT(224), (nul));
//G337: __E_variable_98(224) => digit(3) __E_variable_98(224).
	p(NT(224), (NT(3)+NT(224)));
//G338: __E_variable_99(225) => _(11) ':' _(11) chars(226).
	p(NT(225), (NT(11)+T(2)+NT(11)+NT(226)));
//G339: __E_variable_99(225) => null.
	p(NT(225), (nul));
//G340: variable(139)        => __E_variable_97(223) __E_variable_98(224) __E_variable_99(225).	 # guarded: charvar
	p(NT(139), (NT(223)+NT(224)+NT(225)));
	p.back().guard = "charvar";
//G341: __N_2(448)           => 'F'.
	p(NT(448), (T(34)));
//G342: __N_3(449)           => 'T'.
	p(NT(449), (T(33)));
//G343: __E_variable_100(227) => ~( __N_2(448) ) & ~( __N_3(449) ) & alpha(5).	 # conjunctive
	p(NT(227), ~(NT(448)) & ~(NT(449)) & (NT(5)));
//G344: __E_variable_101(228) => alnum(6).
	p(NT(228), (NT(6)));
//G345: __E_variable_101(228) => '_'.
	p(NT(228), (T(35)));
//G346: __E_variable_102(229) => null.
	p(NT(229), (nul));
//G347: __E_variable_102(229) => __E_variable_101(228) __E_variable_102(229).
	p(NT(229), (NT(228)+NT(229)));
//G348: __E_variable_103(230) => _(11) ':' _(11) chars(226).
	p(NT(230), (NT(11)+T(2)+NT(11)+NT(226)));
//G349: __E_variable_103(230) => null.
	p(NT(230), (nul));
//G350: variable(139)        => __E_variable_100(227) __E_variable_102(229) __E_variable_103(230).	 # guarded: var
	p(NT(139), (NT(227)+NT(229)+NT(230)));
	p.back().guard = "var";
//G351: __E_variable_104(232) => in(233).
	p(NT(232), (NT(233)));
//G352: __E_variable_104(232) => out(234).
	p(NT(232), (NT(234)));
//G353: io_var(231)          => __E_variable_104(232).
	p(NT(231), (NT(232)));
//G354: __E_variable_105(235) => _(11) ':' _(11) chars(226).
	p(NT(235), (NT(11)+T(2)+NT(11)+NT(226)));
//G355: __E_variable_105(235) => null.
	p(NT(235), (nul));
//G356: variable(139)        => io_var(231) __E_variable_105(235).
	p(NT(139), (NT(231)+NT(235)));
//G357: variable(139)        => uninterpreted_constant(236).
	p(NT(139), (NT(236)));
//G358: __E___E_in_106_107(239) => digit(3).
	p(NT(239), (NT(3)));
//G359: __E___E_in_106_107(239) => digit(3) __E___E_in_106_107(239).
	p(NT(239), (NT(3)+NT(239)));
//G360: __E_in_106(238)      => 'i' __E___E_in_106_107(239).
	p(NT(238), (T(10)+NT(239)));
//G361: in_var_name(237)     => __E_in_106(238).
	p(NT(237), (NT(238)));
//G362: in(233)              => in_var_name(237) '[' offset(240) ']'.
	p(NT(233), (NT(237)+T(24)+NT(240)+T(25)));
//G363: __E___E___E_out_108_109_110(244) => digit(3).
	p(NT(244), (NT(3)));
//G364: __E___E___E_out_108_109_110(244) => digit(3) __E___E___E_out_108_109_110(244).
	p(NT(244), (NT(3)+NT(244)));
//G365: __E___E_out_108_109(243) => 'o' __E___E___E_out_108_109_110(244).
	p(NT(243), (T(17)+NT(244)));
//G366: __E_out_108(242)     => __E___E_out_108_109(243).
	p(NT(242), (NT(243)));
//G367: __E_out_108(242)     => 'u'.
	p(NT(242), (T(47)));
//G368: out_var_name(241)    => __E_out_108(242).
	p(NT(241), (NT(242)));
//G369: out(234)             => out_var_name(241) '[' offset(240) ']'.
	p(NT(234), (NT(241)+T(24)+NT(240)+T(25)));
//G370: ctnvar(212)          => chars(226).
	p(NT(212), (NT(226)));
//G371: capture(20)          => '$' chars(226).
	p(NT(20), (T(48)+NT(226)));
//G372: __E_q_vars_111(246)  => _(11) ',' _(11) q_var(245).
	p(NT(246), (NT(11)+T(14)+NT(11)+NT(245)));
//G373: __E_q_vars_112(247)  => null.
	p(NT(247), (nul));
//G374: __E_q_vars_112(247)  => __E_q_vars_111(246) __E_q_vars_112(247).
	p(NT(247), (NT(246)+NT(247)));
//G375: q_vars(72)           => q_var(245) __E_q_vars_112(247).
	p(NT(72), (NT(245)+NT(247)));
//G376: q_var(245)           => capture(20).
	p(NT(245), (NT(20)));
//G377: __E_q_var_113(248)   => _(11) ':' _(11) chars(226).
	p(NT(248), (NT(11)+T(2)+NT(11)+NT(226)));
//G378: __E_q_var_113(248)   => null.
	p(NT(248), (nul));
//G379: __N_4(450)           => uninterpreted_constant(236) __E_q_var_113(248).
	p(NT(450), (NT(236)+NT(248)));
//G380: q_var(245)           => variable(139) & ~( __N_4(450) ).	 # conjunctive
	p(NT(245), (NT(139)) & ~(NT(450)));
//G381: __E_offsets_114(249) => _(11) ',' _(11) offset(240).
	p(NT(249), (NT(11)+T(14)+NT(11)+NT(240)));
//G382: __E_offsets_115(250) => null.
	p(NT(250), (nul));
//G383: __E_offsets_115(250) => __E_offsets_114(249) __E_offsets_115(250).
	p(NT(250), (NT(249)+NT(250)));
//G384: offsets(24)          => '[' _(11) offset(240) __E_offsets_115(250) _(11) ']'.
	p(NT(24), (T(24)+NT(11)+NT(240)+NT(250)+NT(11)+T(25)));
//G385: offset(240)          => integer(251).
	p(NT(240), (NT(251)));
//G386: offset(240)          => capture(20).
	p(NT(240), (NT(20)));
//G387: offset(240)          => shift(252).
	p(NT(240), (NT(252)));
//G388: __N_5(451)           => io_var(231).
	p(NT(451), (NT(231)));
//G389: __E_offset_116(253)  => variable(139) & ~( __N_5(451) ).	 # conjunctive
	p(NT(253), (NT(139)) & ~(NT(451)));
//G390: offset(240)          => __E_offset_116(253).
	p(NT(240), (NT(253)));
//G391: __E_shift_117(254)   => capture(20).
	p(NT(254), (NT(20)));
//G392: __N_6(452)           => io_var(231).
	p(NT(452), (NT(231)));
//G393: __E___E_shift_117_118(255) => variable(139) & ~( __N_6(452) ).	 # conjunctive
	p(NT(255), (NT(139)) & ~(NT(452)));
//G394: __E_shift_117(254)   => __E___E_shift_117_118(255).
	p(NT(254), (NT(255)));
//G395: shift(252)           => __E_shift_117(254) _(11) '-' _(11) num(194).
	p(NT(252), (NT(254)+NT(11)+T(28)+NT(11)+NT(194)));
//G396: num(194)             => digits(256).
	p(NT(194), (NT(256)));
//G397: __E_integer_119(257) => '-'.
	p(NT(257), (T(28)));
//G398: __E_integer_119(257) => null.
	p(NT(257), (nul));
//G399: integer(251)         => __E_integer_119(257) _(11) digits(256).
	p(NT(251), (NT(257)+NT(11)+NT(256)));
//G400: __E___E_uninterpreted_constant_120_121(260) => chars(226).
	p(NT(260), (NT(226)));
//G401: __E___E_uninterpreted_constant_120_121(260) => _(11).
	p(NT(260), (NT(11)));
//G402: __E_uninterpreted_constant_120(259) => __E___E_uninterpreted_constant_120_121(260) ':' chars(226).
	p(NT(259), (NT(260)+T(2)+NT(226)));
//G403: uninter_const_name(258) => __E_uninterpreted_constant_120(259).
	p(NT(258), (NT(259)));
//G404: uninterpreted_constant(236) => '<' uninter_const_name(258) _(11) '>'.
	p(NT(236), (T(20)+NT(258)+NT(11)+T(21)));
//G405: __E_bf_constant_122(262) => capture(20).
	p(NT(262), (NT(20)));
//G406: __E_bf_constant_122(262) => binding(263).
	p(NT(262), (NT(263)));
//G407: constant(261)        => __E_bf_constant_122(262).
	p(NT(261), (NT(262)));
//G408: __E_bf_constant_123(264) => _(11) ':' _(11) type(150).
	p(NT(264), (NT(11)+T(2)+NT(11)+NT(150)));
//G409: __E_bf_constant_123(264) => null.
	p(NT(264), (nul));
//G410: bf_constant(138)     => '{' _(11) constant(261) _(11) '}' __E_bf_constant_123(264).
	p(NT(138), (T(49)+NT(11)+NT(261)+NT(11)+T(50)+NT(264)));
//G411: __E___E___E_binding_124_125_126(269) => src_c(267).
	p(NT(269), (NT(267)));
//G412: __E___E___E_binding_124_125_126(269) => space(2).
	p(NT(269), (NT(2)));
//G413: __E___E___E_binding_124_125_127(270) => null.
	p(NT(270), (nul));
//G414: __E___E___E_binding_124_125_127(270) => __E___E___E_binding_124_125_126(269) __E___E___E_binding_124_125_127(270).
	p(NT(270), (NT(269)+NT(270)));
//G415: __E___E_binding_124_125(268) => __E___E___E_binding_124_125_127(270) src_c(267).
	p(NT(268), (NT(270)+NT(267)));
//G416: __E___E_binding_124_125(268) => null.
	p(NT(268), (nul));
//G417: __E_binding_124(266) => src_c(267) __E___E_binding_124_125(268).
	p(NT(266), (NT(267)+NT(268)));
//G418: source(265)          => __E_binding_124(266).
	p(NT(265), (NT(266)));
//G419: binding(263)         => source(265).
	p(NT(263), (NT(265)));
//G420: src_c(267)           => alnum(6).
	p(NT(267), (NT(6)));
//G421: __N_7(453)           => '{'.
	p(NT(453), (T(49)));
//G422: __N_8(454)           => '}'.
	p(NT(454), (T(50)));
//G423: src_c(267)           => ~( __N_7(453) ) & ~( __N_8(454) ) & punct(7).	 # conjunctive
	p(NT(267), ~(NT(453)) & ~(NT(454)) & (NT(7)));
//G424: __E_src_c_128(271)   => src_c(267).
	p(NT(271), (NT(267)));
//G425: __E_src_c_128(271)   => space(2).
	p(NT(271), (NT(2)));
//G426: __E_src_c_129(272)   => null.
	p(NT(272), (nul));
//G427: __E_src_c_129(272)   => __E_src_c_128(271) __E_src_c_129(272).
	p(NT(272), (NT(271)+NT(272)));
//G428: src_c(267)           => '{' __E_src_c_129(272) '}'.
	p(NT(267), (T(49)+NT(272)+T(50)));
//G429: type(150)            => chars(226).
	p(NT(150), (NT(226)));
//G430: type(150)            => chars(226) _(11) '[' _(11) num(194) _(11) ']'.
	p(NT(150), (NT(226)+NT(11)+T(24)+NT(11)+NT(194)+NT(11)+T(25)));
//G431: __E_chars_130(273)   => alnum(6).
	p(NT(273), (NT(6)));
//G432: __E_chars_130(273)   => '_'.
	p(NT(273), (T(35)));
//G433: __E_chars_131(274)   => null.
	p(NT(274), (nul));
//G434: __E_chars_131(274)   => __E_chars_130(273) __E_chars_131(274).
	p(NT(274), (NT(273)+NT(274)));
//G435: chars(226)           => alpha(5) __E_chars_131(274).
	p(NT(226), (NT(5)+NT(274)));
//G436: __E_digits_132(275)  => digit(3).
	p(NT(275), (NT(3)));
//G437: __E_digits_132(275)  => digit(3) __E_digits_132(275).
	p(NT(275), (NT(3)+NT(275)));
//G438: digits(256)          => __E_digits_132(275).
	p(NT(256), (NT(275)));
//G439: sym(22)              => chars(226).
	p(NT(22), (NT(226)));
//G440: __E_comment_133(277) => printable(8).
	p(NT(277), (NT(8)));
//G441: __E_comment_133(277) => '\t'.
	p(NT(277), (T(51)));
//G442: __E_comment_134(278) => null.
	p(NT(278), (nul));
//G443: __E_comment_134(278) => __E_comment_133(277) __E_comment_134(278).
	p(NT(278), (NT(277)+NT(278)));
//G444: __E_comment_135(279) => '\n'.
	p(NT(279), (T(52)));
//G445: __E_comment_135(279) => '\r'.
	p(NT(279), (T(53)));
//G446: __E_comment_135(279) => eof(1).
	p(NT(279), (NT(1)));
//G447: comment(276)         => '#' __E_comment_134(278) __E_comment_135(279).
	p(NT(276), (T(46)+NT(278)+NT(279)));
//G448: __E____136(280)      => space(2).
	p(NT(280), (NT(2)));
//G449: __E____136(280)      => comment(276).
	p(NT(280), (NT(276)));
//G450: __(27)               => __E____136(280) _(11).
	p(NT(27), (NT(280)+NT(11)));
//G451: __E___137(281)       => __(27).
	p(NT(281), (NT(27)));
//G452: __E___137(281)       => null.
	p(NT(281), (nul));
//G453: _(11)                => __E___137(281).
	p(NT(11), (NT(281)));
//G454: cli(282)             => _(11).
	p(NT(282), (NT(11)));
//G455: __E_cli_138(284)     => '.' _(11) cli_command(283) _(11).
	p(NT(284), (T(1)+NT(11)+NT(283)+NT(11)));
//G456: __E_cli_139(285)     => null.
	p(NT(285), (nul));
//G457: __E_cli_139(285)     => __E_cli_138(284) __E_cli_139(285).
	p(NT(285), (NT(284)+NT(285)));
//G458: cli(282)             => _(11) cli_command(283) _(11) __E_cli_139(285).
	p(NT(282), (NT(11)+NT(283)+NT(11)+NT(285)));
//G459: __E_cli_command_140(288) => 'q'.
	p(NT(288), (T(54)));
//G460: __E_cli_command_140(288) => 'q' 'u' 'i' 't'.
	p(NT(288), (T(54)+T(47)+T(10)+T(13)));
//G461: quit_sym(287)        => __E_cli_command_140(288).
	p(NT(287), (NT(288)));
//G462: quit_cmd(286)        => quit_sym(287).
	p(NT(286), (NT(287)));
//G463: cli_command(283)     => quit_cmd(286).
	p(NT(283), (NT(286)));
//G464: __E_cli_command_141(291) => 'v'.
	p(NT(291), (T(55)));
//G465: __E_cli_command_141(291) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(291), (T(55)+T(19)+T(11)+T(12)+T(10)+T(17)+T(45)));
//G466: version_sym(290)     => __E_cli_command_141(291).
	p(NT(290), (NT(291)));
//G467: version_cmd(289)     => version_sym(290).
	p(NT(289), (NT(290)));
//G468: cli_command(283)     => version_cmd(289).
	p(NT(283), (NT(289)));
//G469: __E_cli_command_142(294) => 'c'.
	p(NT(294), (T(8)));
//G470: __E_cli_command_142(294) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(294), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G471: clear_sym(293)       => __E_cli_command_142(294).
	p(NT(293), (NT(294)));
//G472: clear_cmd(292)       => clear_sym(293).
	p(NT(292), (NT(293)));
//G473: cli_command(283)     => clear_cmd(292).
	p(NT(283), (NT(292)));
//G474: __E___E_cli_command_143_144(298) => 'h'.
	p(NT(298), (T(56)));
//G475: __E___E_cli_command_143_144(298) => 'h' 'e' 'l' 'p'.
	p(NT(298), (T(56)+T(19)+T(6)+T(57)));
//G476: help_sym(297)        => __E___E_cli_command_143_144(298).
	p(NT(297), (NT(298)));
//G477: __E___E_cli_command_143_145(299) => __(27) help_arg(300).
	p(NT(299), (NT(27)+NT(300)));
//G478: __E___E_cli_command_143_145(299) => null.
	p(NT(299), (nul));
//G479: __E_cli_command_143(296) => help_sym(297) __E___E_cli_command_143_145(299).
	p(NT(296), (NT(297)+NT(299)));
//G480: help_cmd(295)        => __E_cli_command_143(296).
	p(NT(295), (NT(296)));
//G481: cli_command(283)     => help_cmd(295).
	p(NT(283), (NT(295)));
//G482: file_sym(303)        => 'f' 'i' 'l' 'e'.
	p(NT(303), (T(4)+T(10)+T(6)+T(19)));
//G483: __E_cli_command_146(302) => file_sym(303) __(27) q_string(304).
	p(NT(302), (NT(303)+NT(27)+NT(304)));
//G484: file_cmd(301)        => __E_cli_command_146(302).
	p(NT(301), (NT(302)));
//G485: cli_command(283)     => file_cmd(301).
	p(NT(283), (NT(301)));
//G486: valid_sym(307)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(307), (T(55)+T(5)+T(6)+T(10)+T(58)));
//G487: __E_cli_command_147(306) => valid_sym(307) __(27) normalize_cmd_arg(308).
	p(NT(306), (NT(307)+NT(27)+NT(308)));
//G488: valid_cmd(305)       => __E_cli_command_147(306).
	p(NT(305), (NT(306)));
//G489: cli_command(283)     => valid_cmd(305).
	p(NT(283), (NT(305)));
//G490: sat_sym(311)         => 's' 'a' 't'.
	p(NT(311), (T(12)+T(5)+T(13)));
//G491: __E_cli_command_148(310) => sat_sym(311) __(27) normalize_cmd_arg(308).
	p(NT(310), (NT(311)+NT(27)+NT(308)));
//G492: sat_cmd(309)         => __E_cli_command_148(310).
	p(NT(309), (NT(310)));
//G493: cli_command(283)     => sat_cmd(309).
	p(NT(283), (NT(309)));
//G494: unsat_sym(314)       => 'u' 'n' 's' 'a' 't'.
	p(NT(314), (T(47)+T(45)+T(12)+T(5)+T(13)));
//G495: __E_cli_command_149(313) => unsat_sym(314) __(27) normalize_cmd_arg(308).
	p(NT(313), (NT(314)+NT(27)+NT(308)));
//G496: unsat_cmd(312)       => __E_cli_command_149(313).
	p(NT(312), (NT(313)));
//G497: cli_command(283)     => unsat_cmd(312).
	p(NT(283), (NT(312)));
//G498: solve_sym(317)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(317), (T(12)+T(17)+T(6)+T(55)+T(19)));
//G499: __E___E_cli_command_150_151(318) => solve_options(319).
	p(NT(318), (NT(319)));
//G500: __E___E_cli_command_150_151(318) => null.
	p(NT(318), (nul));
//G501: __E_cli_command_150(316) => solve_sym(317) __E___E_cli_command_150_151(318) __(27) wff_cmd_arg(320).
	p(NT(316), (NT(317)+NT(318)+NT(27)+NT(320)));
//G502: solve_cmd(315)       => __E_cli_command_150(316).
	p(NT(315), (NT(316)));
//G503: cli_command(283)     => solve_cmd(315).
	p(NT(283), (NT(315)));
//G504: lgrs_sym(323)        => 'l' 'g' 'r' 's'.
	p(NT(323), (T(6)+T(59)+T(11)+T(12)));
//G505: __E_cli_command_152(322) => lgrs_sym(323) __(27) wff_cmd_arg(320).
	p(NT(322), (NT(323)+NT(27)+NT(320)));
//G506: lgrs_cmd(321)        => __E_cli_command_152(322).
	p(NT(321), (NT(322)));
//G507: cli_command(283)     => lgrs_cmd(321).
	p(NT(283), (NT(321)));
//G508: __E___E_cli_command_153_154(327) => 'r'.
	p(NT(327), (T(11)));
//G509: __E___E_cli_command_153_154(327) => 'r' 'u' 'n'.
	p(NT(327), (T(11)+T(47)+T(45)));
//G510: run_sym(326)         => __E___E_cli_command_153_154(327).
	p(NT(326), (NT(327)));
//G511: __E___E_cli_command_153_155(328) => memory(329).
	p(NT(328), (NT(329)));
//G512: __E___E_cli_command_153_155(328) => wff(14).
	p(NT(328), (NT(14)));
//G513: __E_cli_command_153(325) => run_sym(326) __(27) __E___E_cli_command_153_155(328).
	p(NT(325), (NT(326)+NT(27)+NT(328)));
//G514: run_cmd(324)         => __E_cli_command_153(325).
	p(NT(324), (NT(325)));
//G515: cli_command(283)     => run_cmd(324).
	p(NT(283), (NT(324)));
//G516: __E___E_cli_command_156_157(333) => 'n'.
	p(NT(333), (T(45)));
//G517: __E___E_cli_command_156_157(333) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(333), (T(45)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(60)+T(19)));
//G518: normalize_sym(332)   => __E___E_cli_command_156_157(333).
	p(NT(332), (NT(333)));
//G519: __E_cli_command_156(331) => normalize_sym(332) __(27) normalize_cmd_arg(308).
	p(NT(331), (NT(332)+NT(27)+NT(308)));
//G520: normalize_cmd(330)   => __E_cli_command_156(331).
	p(NT(330), (NT(331)));
//G521: cli_command(283)     => normalize_cmd(330).
	p(NT(283), (NT(330)));
//G522: __E___E_cli_command_158_159(337) => 's'.
	p(NT(337), (T(12)));
//G523: __E___E_cli_command_158_159(337) => 's' 'u' 'b' 's' 't'.
	p(NT(337), (T(12)+T(47)+T(7)+T(12)+T(13)));
//G524: __E___E_cli_command_158_159(337) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(337), (T(12)+T(47)+T(7)+T(12)+T(13)+T(10)+T(13)+T(47)+T(13)+T(19)));
//G525: subst_sym(336)       => __E___E_cli_command_158_159(337).
	p(NT(336), (NT(337)));
//G526: __E_cli_command_158(335) => subst_sym(336) __(27) nf_cmd_arg(338) _(11) '[' _(11) nf_cmd_arg(338) _(11) '/' _(11) nf_cmd_arg(338) _(11) ']'.
	p(NT(335), (NT(336)+NT(27)+NT(338)+NT(11)+T(24)+NT(11)+NT(338)+NT(11)+T(43)+NT(11)+NT(338)+NT(11)+T(25)));
//G527: subst_cmd(334)       => __E_cli_command_158(335).
	p(NT(334), (NT(335)));
//G528: cli_command(283)     => subst_cmd(334).
	p(NT(283), (NT(334)));
//G529: __E___E_cli_command_160_161(342) => 'i'.
	p(NT(342), (T(10)));
//G530: __E___E_cli_command_160_161(342) => 'i' 'n' 's' 't'.
	p(NT(342), (T(10)+T(45)+T(12)+T(13)));
//G531: __E___E_cli_command_160_161(342) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(342), (T(10)+T(45)+T(12)+T(13)+T(5)+T(45)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G532: inst_sym(341)        => __E___E_cli_command_160_161(342).
	p(NT(341), (NT(342)));
//G533: __E_cli_command_160(340) => inst_sym(341) __(27) inst_args(343).
	p(NT(340), (NT(341)+NT(27)+NT(343)));
//G534: inst_cmd(339)        => __E_cli_command_160(340).
	p(NT(339), (NT(340)));
//G535: cli_command(283)     => inst_cmd(339).
	p(NT(283), (NT(339)));
//G536: dnf_sym(346)         => 'd' 'n' 'f'.
	p(NT(346), (T(58)+T(45)+T(4)));
//G537: __E_cli_command_162(345) => dnf_sym(346) __(27) nf_cmd_arg(338).
	p(NT(345), (NT(346)+NT(27)+NT(338)));
//G538: dnf_cmd(344)         => __E_cli_command_162(345).
	p(NT(344), (NT(345)));
//G539: cli_command(283)     => dnf_cmd(344).
	p(NT(283), (NT(344)));
//G540: cnf_sym(349)         => 'c' 'n' 'f'.
	p(NT(349), (T(8)+T(45)+T(4)));
//G541: __E_cli_command_163(348) => cnf_sym(349) __(27) nf_cmd_arg(338).
	p(NT(348), (NT(349)+NT(27)+NT(338)));
//G542: cnf_cmd(347)         => __E_cli_command_163(348).
	p(NT(347), (NT(348)));
//G543: cli_command(283)     => cnf_cmd(347).
	p(NT(283), (NT(347)));
//G544: anf_sym(352)         => 'a' 'n' 'f'.
	p(NT(352), (T(5)+T(45)+T(4)));
//G545: __E_cli_command_164(351) => anf_sym(352) __(27) nf_cmd_arg(338).
	p(NT(351), (NT(352)+NT(27)+NT(338)));
//G546: anf_cmd(350)         => __E_cli_command_164(351).
	p(NT(350), (NT(351)));
//G547: cli_command(283)     => anf_cmd(350).
	p(NT(283), (NT(350)));
//G548: nnf_sym(355)         => 'n' 'n' 'f'.
	p(NT(355), (T(45)+T(45)+T(4)));
//G549: __E_cli_command_165(354) => nnf_sym(355) __(27) nf_cmd_arg(338).
	p(NT(354), (NT(355)+NT(27)+NT(338)));
//G550: nnf_cmd(353)         => __E_cli_command_165(354).
	p(NT(353), (NT(354)));
//G551: cli_command(283)     => nnf_cmd(353).
	p(NT(283), (NT(353)));
//G552: pnf_sym(358)         => 'p' 'n' 'f'.
	p(NT(358), (T(57)+T(45)+T(4)));
//G553: __E_cli_command_166(357) => pnf_sym(358) __(27) nf_cmd_arg(338).
	p(NT(357), (NT(358)+NT(27)+NT(338)));
//G554: pnf_cmd(356)         => __E_cli_command_166(357).
	p(NT(356), (NT(357)));
//G555: cli_command(283)     => pnf_cmd(356).
	p(NT(283), (NT(356)));
//G556: mnf_sym(361)         => 'm' 'n' 'f'.
	p(NT(361), (T(18)+T(45)+T(4)));
//G557: __E_cli_command_167(360) => mnf_sym(361) __(27) nf_cmd_arg(338).
	p(NT(360), (NT(361)+NT(27)+NT(338)));
//G558: mnf_cmd(359)         => __E_cli_command_167(360).
	p(NT(359), (NT(360)));
//G559: cli_command(283)     => mnf_cmd(359).
	p(NT(283), (NT(359)));
//G560: snf_sym(364)         => 's' 'n' 'f'.
	p(NT(364), (T(12)+T(45)+T(4)));
//G561: __E_cli_command_168(363) => snf_sym(364) __(27) nf_cmd_arg(338).
	p(NT(363), (NT(364)+NT(27)+NT(338)));
//G562: snf_cmd(362)         => __E_cli_command_168(363).
	p(NT(362), (NT(363)));
//G563: cli_command(283)     => snf_cmd(362).
	p(NT(283), (NT(362)));
//G564: onf_sym(367)         => 'o' 'n' 'f'.
	p(NT(367), (T(17)+T(45)+T(4)));
//G565: __E_cli_command_169(366) => onf_sym(367) __(27) variable(139) __(27) onf_cmd_arg(368).
	p(NT(366), (NT(367)+NT(27)+NT(139)+NT(27)+NT(368)));
//G566: onf_cmd(365)         => __E_cli_command_169(366).
	p(NT(365), (NT(366)));
//G567: cli_command(283)     => onf_cmd(365).
	p(NT(283), (NT(365)));
//G568: __E___E_cli_command_170_171(372) => 'd' 'e' 'f' 's'.
	p(NT(372), (T(58)+T(19)+T(4)+T(12)));
//G569: __E___E_cli_command_170_171(372) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(372), (T(58)+T(19)+T(4)+T(10)+T(45)+T(10)+T(13)+T(10)+T(17)+T(45)+T(12)));
//G570: def_sym(371)         => __E___E_cli_command_170_171(372).
	p(NT(371), (NT(372)));
//G571: __E_cli_command_170(370) => def_sym(371).
	p(NT(370), (NT(371)));
//G572: def_list_cmd(369)    => __E_cli_command_170(370).
	p(NT(369), (NT(370)));
//G573: cli_command(283)     => def_list_cmd(369).
	p(NT(283), (NT(369)));
//G574: __E_cli_command_172(374) => def_sym(371) __(27) number(375).
	p(NT(374), (NT(371)+NT(27)+NT(375)));
//G575: def_print_cmd(373)   => __E_cli_command_172(374).
	p(NT(373), (NT(374)));
//G576: cli_command(283)     => def_print_cmd(373).
	p(NT(283), (NT(373)));
//G577: def_rr_cmd(376)      => rec_relation(16).
	p(NT(376), (NT(16)));
//G578: cli_command(283)     => def_rr_cmd(376).
	p(NT(283), (NT(376)));
//G579: qelim_sym(379)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(379), (T(54)+T(19)+T(6)+T(10)+T(18)));
//G580: __E_cli_command_173(378) => qelim_sym(379) __(27) wff_cmd_arg(320).
	p(NT(378), (NT(379)+NT(27)+NT(320)));
//G581: qelim_cmd(377)       => __E_cli_command_173(378).
	p(NT(377), (NT(378)));
//G582: cli_command(283)     => qelim_cmd(377).
	p(NT(283), (NT(377)));
//G583: get_sym(382)         => 'g' 'e' 't'.
	p(NT(382), (T(59)+T(19)+T(13)));
//G584: __E___E_cli_command_174_175(383) => __(27) option_name(384).
	p(NT(383), (NT(27)+NT(384)));
//G585: __E___E_cli_command_174_175(383) => null.
	p(NT(383), (nul));
//G586: __E_cli_command_174(381) => get_sym(382) __E___E_cli_command_174_175(383).
	p(NT(381), (NT(382)+NT(383)));
//G587: get_cmd(380)         => __E_cli_command_174(381).
	p(NT(380), (NT(381)));
//G588: cli_command(283)     => get_cmd(380).
	p(NT(283), (NT(380)));
//G589: set_sym(387)         => 's' 'e' 't'.
	p(NT(387), (T(12)+T(19)+T(13)));
//G590: __E___E_cli_command_176_177(388) => __(27).
	p(NT(388), (NT(27)));
//G591: __E___E_cli_command_176_177(388) => _(11) '=' _(11).
	p(NT(388), (NT(11)+T(3)+NT(11)));
//G592: __E_cli_command_176(386) => set_sym(387) __(27) option_name(384) __E___E_cli_command_176_177(388) option_value(389).
	p(NT(386), (NT(387)+NT(27)+NT(384)+NT(388)+NT(389)));
//G593: set_cmd(385)         => __E_cli_command_176(386).
	p(NT(385), (NT(386)));
//G594: cli_command(283)     => set_cmd(385).
	p(NT(283), (NT(385)));
//G595: enable_sym(392)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(392), (T(19)+T(45)+T(5)+T(7)+T(6)+T(19)));
//G596: __E_cli_command_178(391) => enable_sym(392) __(27) option_name(384).
	p(NT(391), (NT(392)+NT(27)+NT(384)));
//G597: enable_cmd(390)      => __E_cli_command_178(391).
	p(NT(390), (NT(391)));
//G598: cli_command(283)     => enable_cmd(390).
	p(NT(283), (NT(390)));
//G599: disable_sym(395)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(395), (T(58)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G600: __E_cli_command_179(394) => disable_sym(395) __(27) option_name(384).
	p(NT(394), (NT(395)+NT(27)+NT(384)));
//G601: disable_cmd(393)     => __E_cli_command_179(394).
	p(NT(393), (NT(394)));
//G602: cli_command(283)     => disable_cmd(393).
	p(NT(283), (NT(393)));
//G603: toggle_sym(398)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(398), (T(13)+T(17)+T(59)+T(59)+T(6)+T(19)));
//G604: __E_cli_command_180(397) => toggle_sym(398) __(27) option_name(384).
	p(NT(397), (NT(398)+NT(27)+NT(384)));
//G605: toggle_cmd(396)      => __E_cli_command_180(397).
	p(NT(396), (NT(397)));
//G606: cli_command(283)     => toggle_cmd(396).
	p(NT(283), (NT(396)));
//G607: __E_cli_command_181(400) => type(150) __(27) in_var_name(237) _(11) '=' _(11) input_stream(401).
	p(NT(400), (NT(150)+NT(27)+NT(237)+NT(11)+T(3)+NT(11)+NT(401)));
//G608: def_input_cmd(399)   => __E_cli_command_181(400).
	p(NT(399), (NT(400)));
//G609: cli_command(283)     => def_input_cmd(399).
	p(NT(283), (NT(399)));
//G610: __E_cli_command_182(403) => type(150) __(27) out_var_name(241) _(11) '=' _(11) output_stream(404).
	p(NT(403), (NT(150)+NT(27)+NT(241)+NT(11)+T(3)+NT(11)+NT(404)));
//G611: def_output_cmd(402)  => __E_cli_command_182(403).
	p(NT(402), (NT(403)));
//G612: cli_command(283)     => def_output_cmd(402).
	p(NT(283), (NT(402)));
//G613: __E___E_cli_command_183_184(408) => 'h' 'i' 's' 't'.
	p(NT(408), (T(56)+T(10)+T(12)+T(13)));
//G614: __E___E_cli_command_183_184(408) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(408), (T(56)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G615: history_sym(407)     => __E___E_cli_command_183_184(408).
	p(NT(407), (NT(408)));
//G616: __E_cli_command_183(406) => history_sym(407).
	p(NT(406), (NT(407)));
//G617: history_list_cmd(405) => __E_cli_command_183(406).
	p(NT(405), (NT(406)));
//G618: cli_command(283)     => history_list_cmd(405).
	p(NT(283), (NT(405)));
//G619: __E_cli_command_185(410) => history_sym(407) __(27) memory(329).
	p(NT(410), (NT(407)+NT(27)+NT(329)));
//G620: history_print_cmd(409) => __E_cli_command_185(410).
	p(NT(409), (NT(410)));
//G621: cli_command(283)     => history_print_cmd(409).
	p(NT(283), (NT(409)));
//G622: __E_cli_command_186(412) => wff(14).
	p(NT(412), (NT(14)));
//G623: __E_cli_command_186(412) => bf(21).
	p(NT(412), (NT(21)));
//G624: history_store_cmd(411) => __E_cli_command_186(412).
	p(NT(411), (NT(412)));
//G625: cli_command(283)     => history_store_cmd(411).
	p(NT(283), (NT(411)));
//G626: __E_solve_options_187(413) => __(27) solve_option(414).
	p(NT(413), (NT(27)+NT(414)));
//G627: __E_solve_options_188(415) => null.
	p(NT(415), (nul));
//G628: __E_solve_options_188(415) => __E_solve_options_187(413) __E_solve_options_188(415).
	p(NT(415), (NT(413)+NT(415)));
//G629: solve_options(319)   => __E_solve_options_188(415).
	p(NT(319), (NT(415)));
//G630: __E_solve_option_189(416) => '-' '-' solver_mode(417).
	p(NT(416), (T(28)+T(28)+NT(417)));
//G631: solve_option(414)    => __E_solve_option_189(416).
	p(NT(414), (NT(416)));
//G632: __E_solve_option_190(418) => '-' '-' type(150).
	p(NT(418), (T(28)+T(28)+NT(150)));
//G633: solve_option(414)    => __E_solve_option_190(418).
	p(NT(414), (NT(418)));
//G634: __E_solver_mode_191(420) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(420), (T(18)+T(10)+T(45)+T(10)+T(18)+T(47)+T(18)));
//G635: __E_solver_mode_191(420) => 'm' 'i' 'n'.
	p(NT(420), (T(18)+T(10)+T(45)));
//G636: solver_mode_minimum(419) => __E_solver_mode_191(420).
	p(NT(419), (NT(420)));
//G637: solver_mode(417)     => solver_mode_minimum(419).
	p(NT(417), (NT(419)));
//G638: __E_solver_mode_192(422) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(422), (T(18)+T(5)+T(27)+T(10)+T(18)+T(47)+T(18)));
//G639: __E_solver_mode_192(422) => 'm' 'a' 'x'.
	p(NT(422), (T(18)+T(5)+T(27)));
//G640: solver_mode_maximum(421) => __E_solver_mode_192(422).
	p(NT(421), (NT(422)));
//G641: solver_mode(417)     => solver_mode_maximum(421).
	p(NT(417), (NT(421)));
//G642: __E_input_stream_193(423) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(424) _(11) ')'.
	p(NT(423), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(424)+NT(11)+T(16)));
//G643: input_stream(401)    => __E_input_stream_193(423).
	p(NT(401), (NT(423)));
//G644: console_sym(425)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(425), (T(8)+T(17)+T(45)+T(12)+T(17)+T(6)+T(19)));
//G645: input_stream(401)    => console_sym(425).
	p(NT(401), (NT(425)));
//G646: __E_output_stream_194(426) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(424) _(11) ')'.
	p(NT(426), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(424)+NT(11)+T(16)));
//G647: output_stream(404)   => __E_output_stream_194(426).
	p(NT(404), (NT(426)));
//G648: output_stream(404)   => console_sym(425).
	p(NT(404), (NT(425)));
//G649: q_file_name(424)     => '"' file_name(427) '"'.
	p(NT(424), (T(61)+NT(427)+T(61)));
//G650: __E_file_name_195(428) => printable(8).
	p(NT(428), (NT(8)));
//G651: __E_file_name_195(428) => printable(8) __E_file_name_195(428).
	p(NT(428), (NT(8)+NT(428)));
//G652: file_name(427)       => __E_file_name_195(428).
	p(NT(427), (NT(428)));
//G653: __E_option_name_196(429) => alnum(6).
	p(NT(429), (NT(6)));
//G654: __E_option_name_196(429) => alnum(6) __E_option_name_196(429).
	p(NT(429), (NT(6)+NT(429)));
//G655: option_name(384)     => __E_option_name_196(429).
	p(NT(384), (NT(429)));
//G656: __E_option_value_197(430) => alnum(6).
	p(NT(430), (NT(6)));
//G657: __E_option_value_197(430) => alnum(6) __E_option_value_197(430).
	p(NT(430), (NT(6)+NT(430)));
//G658: option_value(389)    => __E_option_value_197(430).
	p(NT(389), (NT(430)));
//G659: number(375)          => digits(256).
	p(NT(375), (NT(256)));
//G660: bf_cmd_arg(431)      => memory(329).
	p(NT(431), (NT(329)));
//G661: bf_cmd_arg(431)      => bf(21).
	p(NT(431), (NT(21)));
//G662: wff_cmd_arg(320)     => memory(329).
	p(NT(320), (NT(329)));
//G663: wff_cmd_arg(320)     => wff(14).
	p(NT(320), (NT(14)));
//G664: nf_cmd_arg(338)      => memory(329).
	p(NT(338), (NT(329)));
//G665: nf_cmd_arg(338)      => ref(18).
	p(NT(338), (NT(18)));
//G666: nf_cmd_arg(338)      => wff(14).
	p(NT(338), (NT(14)));
//G667: nf_cmd_arg(338)      => bf(21).
	p(NT(338), (NT(21)));
//G668: onf_cmd_arg(368)     => memory(329).
	p(NT(368), (NT(329)));
//G669: onf_cmd_arg(368)     => wff(14).
	p(NT(368), (NT(14)));
//G670: normalize_cmd_arg(308) => memory(329).
	p(NT(308), (NT(329)));
//G671: normalize_cmd_arg(308) => rr(10).
	p(NT(308), (NT(10)));
//G672: normalize_cmd_arg(308) => ref(18).
	p(NT(308), (NT(18)));
//G673: normalize_cmd_arg(308) => wff(14).
	p(NT(308), (NT(14)));
//G674: normalize_cmd_arg(308) => bf(21).
	p(NT(308), (NT(21)));
//G675: inst_args(343)       => wff_cmd_arg(320) _(11) '[' _(11) variable(139) _(11) '/' _(11) bf_cmd_arg(431) _(11) ']'.
	p(NT(343), (NT(320)+NT(11)+T(24)+NT(11)+NT(139)+NT(11)+T(43)+NT(11)+NT(431)+NT(11)+T(25)));
//G676: inst_args(343)       => bf_cmd_arg(431) _(11) '[' _(11) variable(139) _(11) '/' _(11) bf_cmd_arg(431) _(11) ']'.
	p(NT(343), (NT(431)+NT(11)+T(24)+NT(11)+NT(139)+NT(11)+T(43)+NT(11)+NT(431)+NT(11)+T(25)));
//G677: help_arg(300)        => help_sym(297).
	p(NT(300), (NT(297)));
//G678: help_arg(300)        => version_sym(290).
	p(NT(300), (NT(290)));
//G679: help_arg(300)        => quit_sym(287).
	p(NT(300), (NT(287)));
//G680: help_arg(300)        => clear_sym(293).
	p(NT(300), (NT(293)));
//G681: help_arg(300)        => get_sym(382).
	p(NT(300), (NT(382)));
//G682: help_arg(300)        => set_sym(387).
	p(NT(300), (NT(387)));
//G683: help_arg(300)        => enable_sym(392).
	p(NT(300), (NT(392)));
//G684: help_arg(300)        => disable_sym(395).
	p(NT(300), (NT(395)));
//G685: help_arg(300)        => toggle_sym(398).
	p(NT(300), (NT(398)));
//G686: help_arg(300)        => file_sym(303).
	p(NT(300), (NT(303)));
//G687: help_arg(300)        => history_sym(407).
	p(NT(300), (NT(407)));
//G688: help_arg(300)        => abs_memory_sym(432).
	p(NT(300), (NT(432)));
//G689: help_arg(300)        => rel_memory_sym(433).
	p(NT(300), (NT(433)));
//G690: help_arg(300)        => selection_sym(434).
	p(NT(300), (NT(434)));
//G691: help_arg(300)        => def_sym(371).
	p(NT(300), (NT(371)));
//G692: help_arg(300)        => inst_sym(341).
	p(NT(300), (NT(341)));
//G693: help_arg(300)        => subst_sym(336).
	p(NT(300), (NT(336)));
//G694: help_arg(300)        => normalize_sym(332).
	p(NT(300), (NT(332)));
//G695: help_arg(300)        => execute_sym(435).
	p(NT(300), (NT(435)));
//G696: help_arg(300)        => solve_sym(317).
	p(NT(300), (NT(317)));
//G697: help_arg(300)        => valid_sym(307).
	p(NT(300), (NT(307)));
//G698: help_arg(300)        => sat_sym(311).
	p(NT(300), (NT(311)));
//G699: help_arg(300)        => unsat_sym(314).
	p(NT(300), (NT(314)));
//G700: help_arg(300)        => run_sym(326).
	p(NT(300), (NT(326)));
//G701: help_arg(300)        => dnf_sym(346).
	p(NT(300), (NT(346)));
//G702: help_arg(300)        => cnf_sym(349).
	p(NT(300), (NT(349)));
//G703: help_arg(300)        => anf_sym(352).
	p(NT(300), (NT(352)));
//G704: help_arg(300)        => snf_sym(364).
	p(NT(300), (NT(364)));
//G705: help_arg(300)        => nnf_sym(355).
	p(NT(300), (NT(355)));
//G706: help_arg(300)        => pnf_sym(358).
	p(NT(300), (NT(358)));
//G707: help_arg(300)        => mnf_sym(361).
	p(NT(300), (NT(361)));
//G708: help_arg(300)        => onf_sym(367).
	p(NT(300), (NT(367)));
//G709: help_arg(300)        => qelim_sym(379).
	p(NT(300), (NT(379)));
//G710: __E___E_help_arg_198_199(438) => 's'.
	p(NT(438), (T(12)));
//G711: __E___E_help_arg_198_199(438) => null.
	p(NT(438), (nul));
//G712: __E_help_arg_198(437) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_198_199(438).
	p(NT(437), (T(19)+T(27)+T(5)+T(18)+T(57)+T(6)+T(19)+NT(438)));
//G713: examples_sym(436)    => __E_help_arg_198(437).
	p(NT(436), (NT(437)));
//G714: help_arg(300)        => examples_sym(436).
	p(NT(300), (NT(436)));
//G715: __E_memory_200(440)  => '%'.
	p(NT(440), (T(44)));
//G716: rel_memory_sym(433)  => '%' '-'.
	p(NT(433), (T(44)+T(28)));
//G717: memory_id(442)       => digits(256).
	p(NT(442), (NT(256)));
//G718: __E___E_memory_200_201(441) => memory_id(442).
	p(NT(441), (NT(442)));
//G719: __E___E_memory_200_201(441) => null.
	p(NT(441), (nul));
//G720: __E_memory_200(440)  => rel_memory_sym(433) __E___E_memory_200_201(441).
	p(NT(440), (NT(433)+NT(441)));
//G721: rel_memory(439)      => __E_memory_200(440).
	p(NT(439), (NT(440)));
//G722: memory(329)          => rel_memory(439).
	p(NT(329), (NT(439)));
//G723: abs_memory_sym(432)  => '%'.
	p(NT(432), (T(44)));
//G724: __E_memory_202(444)  => abs_memory_sym(432) memory_id(442).
	p(NT(444), (NT(432)+NT(442)));
//G725: abs_memory(443)      => __E_memory_202(444).
	p(NT(443), (NT(444)));
//G726: memory(329)          => abs_memory(443).
	p(NT(329), (NT(443)));
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
		__E_wff_41, bf_less, __E_wff_42, bf_nless, __E_wff_43, z3_eq, __E_wff_44, z3, z3_neq, __E_wff_45, 
		z3_less_equal, __E_wff_46, z3_nleq, __E_wff_47, z3_greater, __E_wff_48, z3_ngreater, __E_wff_49, z3_greater_equal, __E_wff_50, 
		z3_ngeq, __E_wff_51, z3_less, __E_wff_52, z3_nless, __E_wff_53, bf_parenthesis, __E_bf_54, bf_constant, variable, 
		bf_splitter, __E_bf_55, bf_ref, bf_or, __E_bf_56, bf_xor, __E_bf_57, bf_t, __E_bf_58, __E___E_bf_58_59, 
		type, bf_f, __E_bf_60, __E___E_bf_60_61, bf_and, __E_bf_62, __E___E_bf_62_63, bf_neg, __E_bf_64, bf_neg_oprnd, 
		__E___E_bf_64_65, bf_and_nosep, __E_bf_66, bf_and_nosep_1st_oprnd, __E___E_bf_66_67, bf_and_nosep_2nd_oprnd, __E___E_bf_66_68, z3_parenthesis, __E_z3_69, z3_neg, 
		z3_add, __E_z3_70, z3_sub, __E_z3_71, z3_mul, __E_z3_72, z3_div, __E_z3_73, z3_mod, __E_z3_74, 
		z3_nand, __E_z3_75, z3_and, __E_z3_76, z3_nor, __E_z3_77, z3_or, __E_z3_78, z3_xnor, __E_z3_79, 
		z3_xor, __E_z3_80, z3_rotate_left, __E_z3_81, num, z3_rotate_right, __E_z3_82, z3_min, __E_z3_83, z3_max, 
		__E_z3_84, bitvector, bits, __E_bitvector_85, __E_bitvector_86, hexnum, __E_bitvector_87, __E_bits_88, __E_bits_89, __E_hexnum_90, 
		ctn_neq, __E_constraint_91, ctnvar, ctn_eq, __E_constraint_92, ctn_greater_equal, __E_constraint_93, ctn_greater, __E_constraint_94, ctn_less_equal, 
		__E_constraint_95, ctn_less, __E_constraint_96, __E_variable_97, __E_variable_98, __E_variable_99, chars, __E_variable_100, __E_variable_101, __E_variable_102, 
		__E_variable_103, io_var, __E_variable_104, in, out, __E_variable_105, uninterpreted_constant, in_var_name, __E_in_106, __E___E_in_106_107, 
		offset, out_var_name, __E_out_108, __E___E_out_108_109, __E___E___E_out_108_109_110, q_var, __E_q_vars_111, __E_q_vars_112, __E_q_var_113, __E_offsets_114, 
		__E_offsets_115, integer, shift, __E_offset_116, __E_shift_117, __E___E_shift_117_118, digits, __E_integer_119, uninter_const_name, __E_uninterpreted_constant_120, 
		__E___E_uninterpreted_constant_120_121, constant, __E_bf_constant_122, binding, __E_bf_constant_123, source, __E_binding_124, src_c, __E___E_binding_124_125, __E___E___E_binding_124_125_126, 
		__E___E___E_binding_124_125_127, __E_src_c_128, __E_src_c_129, __E_chars_130, __E_chars_131, __E_digits_132, comment, __E_comment_133, __E_comment_134, __E_comment_135, 
		__E____136, __E___137, cli, cli_command, __E_cli_138, __E_cli_139, quit_cmd, quit_sym, __E_cli_command_140, version_cmd, 
		version_sym, __E_cli_command_141, clear_cmd, clear_sym, __E_cli_command_142, help_cmd, __E_cli_command_143, help_sym, __E___E_cli_command_143_144, __E___E_cli_command_143_145, 
		help_arg, file_cmd, __E_cli_command_146, file_sym, q_string, valid_cmd, __E_cli_command_147, valid_sym, normalize_cmd_arg, sat_cmd, 
		__E_cli_command_148, sat_sym, unsat_cmd, __E_cli_command_149, unsat_sym, solve_cmd, __E_cli_command_150, solve_sym, __E___E_cli_command_150_151, solve_options, 
		wff_cmd_arg, lgrs_cmd, __E_cli_command_152, lgrs_sym, run_cmd, __E_cli_command_153, run_sym, __E___E_cli_command_153_154, __E___E_cli_command_153_155, memory, 
		normalize_cmd, __E_cli_command_156, normalize_sym, __E___E_cli_command_156_157, subst_cmd, __E_cli_command_158, subst_sym, __E___E_cli_command_158_159, nf_cmd_arg, inst_cmd, 
		__E_cli_command_160, inst_sym, __E___E_cli_command_160_161, inst_args, dnf_cmd, __E_cli_command_162, dnf_sym, cnf_cmd, __E_cli_command_163, cnf_sym, 
		anf_cmd, __E_cli_command_164, anf_sym, nnf_cmd, __E_cli_command_165, nnf_sym, pnf_cmd, __E_cli_command_166, pnf_sym, mnf_cmd, 
		__E_cli_command_167, mnf_sym, snf_cmd, __E_cli_command_168, snf_sym, onf_cmd, __E_cli_command_169, onf_sym, onf_cmd_arg, def_list_cmd, 
		__E_cli_command_170, def_sym, __E___E_cli_command_170_171, def_print_cmd, __E_cli_command_172, number, def_rr_cmd, qelim_cmd, __E_cli_command_173, qelim_sym, 
		get_cmd, __E_cli_command_174, get_sym, __E___E_cli_command_174_175, option_name, set_cmd, __E_cli_command_176, set_sym, __E___E_cli_command_176_177, option_value, 
		enable_cmd, __E_cli_command_178, enable_sym, disable_cmd, __E_cli_command_179, disable_sym, toggle_cmd, __E_cli_command_180, toggle_sym, def_input_cmd, 
		__E_cli_command_181, input_stream, def_output_cmd, __E_cli_command_182, output_stream, history_list_cmd, __E_cli_command_183, history_sym, __E___E_cli_command_183_184, history_print_cmd, 
		__E_cli_command_185, history_store_cmd, __E_cli_command_186, __E_solve_options_187, solve_option, __E_solve_options_188, __E_solve_option_189, solver_mode, __E_solve_option_190, solver_mode_minimum, 
		__E_solver_mode_191, solver_mode_maximum, __E_solver_mode_192, __E_input_stream_193, q_file_name, console_sym, __E_output_stream_194, file_name, __E_file_name_195, __E_option_name_196, 
		__E_option_value_197, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, execute_sym, examples_sym, __E_help_arg_198, __E___E_help_arg_198_199, rel_memory, 
		__E_memory_200, __E___E_memory_200_201, memory_id, abs_memory, __E_memory_202, named_binding, __N_0, __N_1, __N_2, __N_3, 
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
