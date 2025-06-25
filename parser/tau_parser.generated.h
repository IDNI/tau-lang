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
	"__E_wff_41", "bf_less", "__E_wff_42", "bf_nless", "__E_wff_43", "bv_eq", "__E_wff_44", "bv", "bv_neq", "__E_wff_45", 
	"bv_less_equal", "__E_wff_46", "bv_nleq", "__E_wff_47", "bv_greater", "__E_wff_48", "bv_ngreater", "__E_wff_49", "bv_greater_equal", "__E_wff_50", 
	"bv_ngeq", "__E_wff_51", "bv_less", "__E_wff_52", "bv_nless", "__E_wff_53", "bf_parenthesis", "__E_bf_54", "bf_constant", "variable", 
	"bf_splitter", "__E_bf_55", "bf_ref", "bf_or", "__E_bf_56", "bf_xor", "__E_bf_57", "bf_t", "__E_bf_58", "__E___E_bf_58_59", 
	"type", "bf_f", "__E_bf_60", "__E___E_bf_60_61", "bf_and", "__E_bf_62", "__E___E_bf_62_63", "bf_neg", "__E_bf_64", "bf_neg_oprnd", 
	"__E___E_bf_64_65", "bf_and_nosep", "__E_bf_66", "bf_and_nosep_1st_oprnd", "__E___E_bf_66_67", "bf_and_nosep_2nd_oprnd", "__E___E_bf_66_68", "bv_parenthesis", "__E_bv_69", "bv_checked", 
	"__E_bv_70", "bv_neg", "bv_add", "__E_bv_71", "bv_sub", "__E_bv_72", "bv_mul", "__E_bv_73", "bv_div", "__E_bv_74", 
	"bv_mod", "__E_bv_75", "bv_nand", "__E_bv_76", "bv_and", "__E_bv_77", "bv_nor", "__E_bv_78", "bv_or", "__E_bv_79", 
	"bv_xnor", "__E_bv_80", "bv_xor", "__E_bv_81", "bv_rotate_left", "__E_bv_82", "num", "bv_rotate_right", "__E_bv_83", "bv_min", 
	"__E_bv_84", "bv_max", "__E_bv_85", "bitvector", "bits", "__E_bitvector_86", "bv_type", "__E_bitvector_87", "hexnum", "__E_bitvector_88", 
	"__E_bits_89", "__E_bits_90", "__E_hexnum_91", "ctn_neq", "__E_constraint_92", "ctnvar", "ctn_eq", "__E_constraint_93", "ctn_greater_equal", "__E_constraint_94", 
	"ctn_greater", "__E_constraint_95", "ctn_less_equal", "__E_constraint_96", "ctn_less", "__E_constraint_97", "__E_variable_98", "__E_variable_99", "__E_variable_100", "__E_variable_101", 
	"__E_variable_102", "__E_variable_103", "__E_variable_104", "io_var", "__E_variable_105", "in", "out", "__E_variable_106", "uninterpreted_constant", "in_var_name", 
	"__E_in_107", "__E___E_in_107_108", "offset", "out_var_name", "__E_out_109", "__E___E_out_109_110", "__E___E___E_out_109_110_111", "chars", "q_var", "__E_q_vars_112", 
	"__E_q_vars_113", "__E_q_var_114", "__E_offsets_115", "__E_offsets_116", "integer", "shift", "__E_offset_117", "__E_shift_118", "__E___E_shift_118_119", "digits", 
	"__E_integer_120", "uninter_const_name", "__E_uninterpreted_constant_121", "__E___E_uninterpreted_constant_121_122", "constant", "__E_bf_constant_123", "binding", "__E_bf_constant_124", "source", "__E_binding_125", 
	"src_c", "__E___E_binding_125_126", "__E___E___E_binding_125_126_127", "__E___E___E_binding_125_126_128", "__E_src_c_129", "__E_src_c_130", "__E_bv_type_131", "__E_chars_132", "__E_chars_133", "__E_digits_134", 
	"comment", "__E_comment_135", "__E_comment_136", "__E_comment_137", "__E____138", "__E___139", "cli", "cli_command", "__E_cli_140", "__E_cli_141", 
	"quit_cmd", "quit_sym", "__E_cli_command_142", "version_cmd", "version_sym", "__E_cli_command_143", "clear_cmd", "clear_sym", "__E_cli_command_144", "help_cmd", 
	"__E_cli_command_145", "help_sym", "__E___E_cli_command_145_146", "__E___E_cli_command_145_147", "help_arg", "file_cmd", "__E_cli_command_148", "file_sym", "q_string", "valid_cmd", 
	"__E_cli_command_149", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_150", "sat_sym", "unsat_cmd", "__E_cli_command_151", "unsat_sym", "solve_cmd", 
	"__E_cli_command_152", "solve_sym", "__E___E_cli_command_152_153", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_154", "lgrs_sym", "run_cmd", "__E_cli_command_155", 
	"run_sym", "__E___E_cli_command_155_156", "__E___E_cli_command_155_157", "memory", "normalize_cmd", "__E_cli_command_158", "normalize_sym", "__E___E_cli_command_158_159", "subst_cmd", "__E_cli_command_160", 
	"subst_sym", "__E___E_cli_command_160_161", "nf_cmd_arg", "inst_cmd", "__E_cli_command_162", "inst_sym", "__E___E_cli_command_162_163", "inst_args", "dnf_cmd", "__E_cli_command_164", 
	"dnf_sym", "cnf_cmd", "__E_cli_command_165", "cnf_sym", "anf_cmd", "__E_cli_command_166", "anf_sym", "nnf_cmd", "__E_cli_command_167", "nnf_sym", 
	"pnf_cmd", "__E_cli_command_168", "pnf_sym", "mnf_cmd", "__E_cli_command_169", "mnf_sym", "snf_cmd", "__E_cli_command_170", "snf_sym", "onf_cmd", 
	"__E_cli_command_171", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_172", "def_sym", "__E___E_cli_command_172_173", "def_print_cmd", "__E_cli_command_174", "number", 
	"def_rr_cmd", "qelim_cmd", "__E_cli_command_175", "qelim_sym", "get_cmd", "__E_cli_command_176", "get_sym", "__E___E_cli_command_176_177", "option_name", "set_cmd", 
	"__E_cli_command_178", "set_sym", "__E___E_cli_command_178_179", "option_value", "enable_cmd", "__E_cli_command_180", "enable_sym", "disable_cmd", "__E_cli_command_181", "disable_sym", 
	"toggle_cmd", "__E_cli_command_182", "toggle_sym", "def_input_cmd", "__E_cli_command_183", "input_stream", "def_output_cmd", "__E_cli_command_184", "output_stream", "history_list_cmd", 
	"__E_cli_command_185", "history_sym", "__E___E_cli_command_185_186", "history_print_cmd", "__E_cli_command_187", "history_store_cmd", "__E_cli_command_188", "__E_solve_options_189", "solve_option", "__E_solve_options_190", 
	"__E_solve_option_191", "solver_mode", "__E_solve_option_192", "solver_mode_minimum", "__E_solver_mode_193", "solver_mode_maximum", "__E_solver_mode_194", "__E_input_stream_195", "q_file_name", "console_sym", 
	"__E_output_stream_196", "file_name", "__E_file_name_197", "__E_option_name_198", "__E_option_value_199", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "execute_sym", 
	"examples_sym", "__E_help_arg_200", "__E___E_help_arg_200_201", "rel_memory", "__E_memory_202", "__E___E_memory_202_203", "memory_id", "abs_memory", "__E_memory_204", "named_binding", 
	"__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'f', 'a', 'l', 'b', 'c', 
	'k', 'i', 'r', 's', 't', ',', '(', ')', 'o', 'm', 
	'e', '<', '>', 'w', 'y', '[', ']', '?', 'x', '-', 
	'|', '^', '&', '!', 'T', 'F', '_', 'S', '+', '1', 
	'0', '\'', '~', '*', '/', '%', 'n', '#', 'u', '$', 
	'{', '}', 'v', '\t', '\n', '\r', 'q', 'h', 'p', 'd', 
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
			20, 22, 139, 150, 204, 206, 208, 215, 235, 236,
			239, 243, 254, 259, 261, 268, 388, 393, 431, 449
		},
		.to_inline = {
			{ 14, 60, 14 },
			{ 21, 136, 21 },
			{ 117, 167, 117 },
			{ 159, 136, 21 },
			{ 161, 136, 154 },
			{ 163, 136, 21 },
			{ 165, 136, 21 },
			{ 247 },
			{ 248 },
			{ 270 },
			{ 312 },
			{ 324 },
			{ 342 },
			{ 347 },
			{ 372 },
			{ 435 }
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
//G145: __E_wff_44(116)      => bv(117) _(11) '=' '_' _(11) bv(117).
	p(NT(116), (NT(117)+NT(11)+T(3)+T(35)+NT(11)+NT(117)));
//G146: bv_eq(115)           => __E_wff_44(116).
	p(NT(115), (NT(116)));
//G147: wff(14)              => bv_eq(115).
	p(NT(14), (NT(115)));
//G148: __E_wff_45(119)      => bv(117) _(11) '!' '=' '_' _(11) bv(117).
	p(NT(119), (NT(117)+NT(11)+T(32)+T(3)+T(35)+NT(11)+NT(117)));
//G149: bv_neq(118)          => __E_wff_45(119).
	p(NT(118), (NT(119)));
//G150: wff(14)              => bv_neq(118).
	p(NT(14), (NT(118)));
//G151: __E_wff_46(121)      => bv(117) _(11) '<' '=' '_' _(11) bv(117).
	p(NT(121), (NT(117)+NT(11)+T(20)+T(3)+T(35)+NT(11)+NT(117)));
//G152: bv_less_equal(120)   => __E_wff_46(121).
	p(NT(120), (NT(121)));
//G153: wff(14)              => bv_less_equal(120).
	p(NT(14), (NT(120)));
//G154: __E_wff_47(123)      => bv(117) _(11) '!' '<' '=' '_' _(11) bv(117).
	p(NT(123), (NT(117)+NT(11)+T(32)+T(20)+T(3)+T(35)+NT(11)+NT(117)));
//G155: bv_nleq(122)         => __E_wff_47(123).
	p(NT(122), (NT(123)));
//G156: wff(14)              => bv_nleq(122).
	p(NT(14), (NT(122)));
//G157: __E_wff_48(125)      => bv(117) _(11) '>' '_' _(11) bv(117).
	p(NT(125), (NT(117)+NT(11)+T(21)+T(35)+NT(11)+NT(117)));
//G158: bv_greater(124)      => __E_wff_48(125).
	p(NT(124), (NT(125)));
//G159: wff(14)              => bv_greater(124).
	p(NT(14), (NT(124)));
//G160: __E_wff_49(127)      => bv(117) _(11) '!' '>' '_' _(11) bv(117).
	p(NT(127), (NT(117)+NT(11)+T(32)+T(21)+T(35)+NT(11)+NT(117)));
//G161: bv_ngreater(126)     => __E_wff_49(127).
	p(NT(126), (NT(127)));
//G162: wff(14)              => bv_ngreater(126).
	p(NT(14), (NT(126)));
//G163: __E_wff_50(129)      => bv(117) _(11) '>' '=' '_' _(11) bv(117).
	p(NT(129), (NT(117)+NT(11)+T(21)+T(3)+T(35)+NT(11)+NT(117)));
//G164: bv_greater_equal(128) => __E_wff_50(129).
	p(NT(128), (NT(129)));
//G165: wff(14)              => bv_greater_equal(128).
	p(NT(14), (NT(128)));
//G166: __E_wff_51(131)      => bv(117) _(11) '!' '>' '=' '_' _(11) bv(117).
	p(NT(131), (NT(117)+NT(11)+T(32)+T(21)+T(3)+T(35)+NT(11)+NT(117)));
//G167: bv_ngeq(130)         => __E_wff_51(131).
	p(NT(130), (NT(131)));
//G168: wff(14)              => bv_ngeq(130).
	p(NT(14), (NT(130)));
//G169: __E_wff_52(133)      => bv(117) _(11) '<' '_' _(11) bv(117).
	p(NT(133), (NT(117)+NT(11)+T(20)+T(35)+NT(11)+NT(117)));
//G170: bv_less(132)         => __E_wff_52(133).
	p(NT(132), (NT(133)));
//G171: wff(14)              => bv_less(132).
	p(NT(14), (NT(132)));
//G172: __E_wff_53(135)      => bv(117) _(11) '!' '<' '_' _(11) bv(117).
	p(NT(135), (NT(117)+NT(11)+T(32)+T(20)+T(35)+NT(11)+NT(117)));
//G173: bv_nless(134)        => __E_wff_53(135).
	p(NT(134), (NT(135)));
//G174: wff(14)              => bv_nless(134).
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
//G240: __E_bv_69(168)       => '(' _(11) bv(117) _(11) ')'.
	p(NT(168), (T(15)+NT(11)+NT(117)+NT(11)+T(16)));
//G241: bv_parenthesis(167)  => __E_bv_69(168).
	p(NT(167), (NT(168)));
//G242: bv(117)              => bv_parenthesis(167).
	p(NT(117), (NT(167)));
//G243: __E_bv_70(170)       => '[' bv(117) ']'.
	p(NT(170), (T(24)+NT(117)+T(25)));
//G244: bv_checked(169)      => __E_bv_70(170).
	p(NT(169), (NT(170)));
//G245: bv(117)              => bv_checked(169).
	p(NT(117), (NT(169)));
//G246: bv(117)              => variable(139).
	p(NT(117), (NT(139)));
//G247: bv_neg(171)          => bv(117).
	p(NT(171), (NT(117)));
//G248: bv(117)              => '~' _(11) bv_neg(171).
	p(NT(117), (T(41)+NT(11)+NT(171)));
//G249: __E_bv_71(173)       => bv(117) _(11) '+' _(11) bv(117).
	p(NT(173), (NT(117)+NT(11)+T(37)+NT(11)+NT(117)));
//G250: bv_add(172)          => __E_bv_71(173).
	p(NT(172), (NT(173)));
//G251: bv(117)              => bv_add(172).
	p(NT(117), (NT(172)));
//G252: __E_bv_72(175)       => bv(117) _(11) '-' _(11) bv(117).
	p(NT(175), (NT(117)+NT(11)+T(28)+NT(11)+NT(117)));
//G253: bv_sub(174)          => __E_bv_72(175).
	p(NT(174), (NT(175)));
//G254: bv(117)              => bv_sub(174).
	p(NT(117), (NT(174)));
//G255: __E_bv_73(177)       => bv(117) _(11) '*' _(11) bv(117).
	p(NT(177), (NT(117)+NT(11)+T(42)+NT(11)+NT(117)));
//G256: bv_mul(176)          => __E_bv_73(177).
	p(NT(176), (NT(177)));
//G257: bv(117)              => bv_mul(176).
	p(NT(117), (NT(176)));
//G258: __E_bv_74(179)       => bv(117) _(11) '/' _(11) bv(117).
	p(NT(179), (NT(117)+NT(11)+T(43)+NT(11)+NT(117)));
//G259: bv_div(178)          => __E_bv_74(179).
	p(NT(178), (NT(179)));
//G260: bv(117)              => bv_div(178).
	p(NT(117), (NT(178)));
//G261: __E_bv_75(181)       => bv(117) _(11) '%' _(11) bv(117).
	p(NT(181), (NT(117)+NT(11)+T(44)+NT(11)+NT(117)));
//G262: bv_mod(180)          => __E_bv_75(181).
	p(NT(180), (NT(181)));
//G263: bv(117)              => bv_mod(180).
	p(NT(117), (NT(180)));
//G264: __E_bv_76(183)       => bv(117) _(11) '!' '&' _(11) bv(117).
	p(NT(183), (NT(117)+NT(11)+T(32)+T(31)+NT(11)+NT(117)));
//G265: bv_nand(182)         => __E_bv_76(183).
	p(NT(182), (NT(183)));
//G266: bv(117)              => bv_nand(182).
	p(NT(117), (NT(182)));
//G267: __E_bv_77(185)       => bv(117) _(11) '&' _(11) bv(117).
	p(NT(185), (NT(117)+NT(11)+T(31)+NT(11)+NT(117)));
//G268: bv_and(184)          => __E_bv_77(185).
	p(NT(184), (NT(185)));
//G269: bv(117)              => bv_and(184).
	p(NT(117), (NT(184)));
//G270: __E_bv_78(187)       => bv(117) _(11) '!' '|' _(11) bv(117).
	p(NT(187), (NT(117)+NT(11)+T(32)+T(29)+NT(11)+NT(117)));
//G271: bv_nor(186)          => __E_bv_78(187).
	p(NT(186), (NT(187)));
//G272: bv(117)              => bv_nor(186).
	p(NT(117), (NT(186)));
//G273: __E_bv_79(189)       => bv(117) _(11) '|' _(11) bv(117).
	p(NT(189), (NT(117)+NT(11)+T(29)+NT(11)+NT(117)));
//G274: bv_or(188)           => __E_bv_79(189).
	p(NT(188), (NT(189)));
//G275: bv(117)              => bv_or(188).
	p(NT(117), (NT(188)));
//G276: __E_bv_80(191)       => bv(117) _(11) '!' '(' '+' ')' _(11) bv(117).
	p(NT(191), (NT(117)+NT(11)+T(32)+T(15)+T(37)+T(16)+NT(11)+NT(117)));
//G277: bv_xnor(190)         => __E_bv_80(191).
	p(NT(190), (NT(191)));
//G278: bv(117)              => bv_xnor(190).
	p(NT(117), (NT(190)));
//G279: __E_bv_81(193)       => bv(117) _(11) '(' '+' ')' _(11) bv(117).
	p(NT(193), (NT(117)+NT(11)+T(15)+T(37)+T(16)+NT(11)+NT(117)));
//G280: bv_xor(192)          => __E_bv_81(193).
	p(NT(192), (NT(193)));
//G281: bv(117)              => bv_xor(192).
	p(NT(117), (NT(192)));
//G282: __E_bv_82(195)       => bv(117) _(11) '<' '<' _(11) num(196).
	p(NT(195), (NT(117)+NT(11)+T(20)+T(20)+NT(11)+NT(196)));
//G283: bv_rotate_left(194)  => __E_bv_82(195).
	p(NT(194), (NT(195)));
//G284: bv(117)              => bv_rotate_left(194).
	p(NT(117), (NT(194)));
//G285: __E_bv_83(198)       => bv(117) _(11) '>' '>' _(11) num(196).
	p(NT(198), (NT(117)+NT(11)+T(21)+T(21)+NT(11)+NT(196)));
//G286: bv_rotate_right(197) => __E_bv_83(198).
	p(NT(197), (NT(198)));
//G287: bv(117)              => bv_rotate_right(197).
	p(NT(117), (NT(197)));
//G288: __E_bv_84(200)       => 'm' 'i' 'n' __(27) bv(117) __(27) bv(117).
	p(NT(200), (T(18)+T(10)+T(45)+NT(27)+NT(117)+NT(27)+NT(117)));
//G289: bv_min(199)          => __E_bv_84(200).
	p(NT(199), (NT(200)));
//G290: bv(117)              => bv_min(199).
	p(NT(117), (NT(199)));
//G291: __E_bv_85(202)       => 'm' 'a' 'x' __(27) bv(117) __(27) bv(117).
	p(NT(202), (T(18)+T(5)+T(27)+NT(27)+NT(117)+NT(27)+NT(117)));
//G292: bv_max(201)          => __E_bv_85(202).
	p(NT(201), (NT(202)));
//G293: bv(117)              => bv_max(201).
	p(NT(117), (NT(201)));
//G294: bv(117)              => bitvector(203).
	p(NT(117), (NT(203)));
//G295: __E_bitvector_86(205) => _(11) ':' _(11) bv_type(206) _(11).
	p(NT(205), (NT(11)+T(2)+NT(11)+NT(206)+NT(11)));
//G296: __E_bitvector_86(205) => null.
	p(NT(205), (nul));
//G297: bitvector(203)       => '#' 'b' bits(204) __E_bitvector_86(205).
	p(NT(203), (T(46)+T(7)+NT(204)+NT(205)));
//G298: __E_bitvector_87(207) => _(11) ':' _(11) bv_type(206) _(11).
	p(NT(207), (NT(11)+T(2)+NT(11)+NT(206)+NT(11)));
//G299: __E_bitvector_87(207) => null.
	p(NT(207), (nul));
//G300: bitvector(203)       => num(196) __E_bitvector_87(207).
	p(NT(203), (NT(196)+NT(207)));
//G301: __E_bitvector_88(209) => _(11) ':' _(11) bv_type(206) _(11).
	p(NT(209), (NT(11)+T(2)+NT(11)+NT(206)+NT(11)));
//G302: __E_bitvector_88(209) => null.
	p(NT(209), (nul));
//G303: bitvector(203)       => '#' 'x' hexnum(208) __E_bitvector_88(209).
	p(NT(203), (T(46)+T(27)+NT(208)+NT(209)));
//G304: __E_bits_89(210)     => '0'.
	p(NT(210), (T(39)));
//G305: __E_bits_89(210)     => '1'.
	p(NT(210), (T(38)));
//G306: __E_bits_90(211)     => __E_bits_89(210).
	p(NT(211), (NT(210)));
//G307: __E_bits_90(211)     => __E_bits_89(210) __E_bits_90(211).
	p(NT(211), (NT(210)+NT(211)));
//G308: bits(204)            => __E_bits_90(211).
	p(NT(204), (NT(211)));
//G309: __E_hexnum_91(212)   => xdigit(4).
	p(NT(212), (NT(4)));
//G310: __E_hexnum_91(212)   => xdigit(4) __E_hexnum_91(212).
	p(NT(212), (NT(4)+NT(212)));
//G311: hexnum(208)          => __E_hexnum_91(212).
	p(NT(208), (NT(212)));
//G312: __E_constraint_92(214) => '[' ctnvar(215) _(11) '!' '=' _(11) num(196) ']'.
	p(NT(214), (T(24)+NT(215)+NT(11)+T(32)+T(3)+NT(11)+NT(196)+T(25)));
//G313: __E_constraint_92(214) => '[' num(196) _(11) '!' '=' _(11) ctnvar(215) ']'.
	p(NT(214), (T(24)+NT(196)+NT(11)+T(32)+T(3)+NT(11)+NT(215)+T(25)));
//G314: ctn_neq(213)         => __E_constraint_92(214).
	p(NT(213), (NT(214)));
//G315: constraint(76)       => ctn_neq(213).
	p(NT(76), (NT(213)));
//G316: __E_constraint_93(217) => '[' ctnvar(215) _(11) '=' _(11) num(196) ']'.
	p(NT(217), (T(24)+NT(215)+NT(11)+T(3)+NT(11)+NT(196)+T(25)));
//G317: __E_constraint_93(217) => '[' num(196) _(11) '=' _(11) ctnvar(215) ']'.
	p(NT(217), (T(24)+NT(196)+NT(11)+T(3)+NT(11)+NT(215)+T(25)));
//G318: ctn_eq(216)          => __E_constraint_93(217).
	p(NT(216), (NT(217)));
//G319: constraint(76)       => ctn_eq(216).
	p(NT(76), (NT(216)));
//G320: __E_constraint_94(219) => '[' ctnvar(215) _(11) '>' '=' _(11) num(196) ']'.
	p(NT(219), (T(24)+NT(215)+NT(11)+T(21)+T(3)+NT(11)+NT(196)+T(25)));
//G321: __E_constraint_94(219) => '[' num(196) _(11) '>' '=' _(11) ctnvar(215) ']'.
	p(NT(219), (T(24)+NT(196)+NT(11)+T(21)+T(3)+NT(11)+NT(215)+T(25)));
//G322: ctn_greater_equal(218) => __E_constraint_94(219).
	p(NT(218), (NT(219)));
//G323: constraint(76)       => ctn_greater_equal(218).
	p(NT(76), (NT(218)));
//G324: __E_constraint_95(221) => '[' ctnvar(215) _(11) '>' _(11) num(196) ']'.
	p(NT(221), (T(24)+NT(215)+NT(11)+T(21)+NT(11)+NT(196)+T(25)));
//G325: __E_constraint_95(221) => '[' num(196) _(11) '>' _(11) ctnvar(215) ']'.
	p(NT(221), (T(24)+NT(196)+NT(11)+T(21)+NT(11)+NT(215)+T(25)));
//G326: ctn_greater(220)     => __E_constraint_95(221).
	p(NT(220), (NT(221)));
//G327: constraint(76)       => ctn_greater(220).
	p(NT(76), (NT(220)));
//G328: __E_constraint_96(223) => '[' ctnvar(215) _(11) '<' '=' _(11) num(196) ']'.
	p(NT(223), (T(24)+NT(215)+NT(11)+T(20)+T(3)+NT(11)+NT(196)+T(25)));
//G329: __E_constraint_96(223) => '[' num(196) _(11) '<' '=' _(11) ctnvar(215) ']'.
	p(NT(223), (T(24)+NT(196)+NT(11)+T(20)+T(3)+NT(11)+NT(215)+T(25)));
//G330: ctn_less_equal(222)  => __E_constraint_96(223).
	p(NT(222), (NT(223)));
//G331: constraint(76)       => ctn_less_equal(222).
	p(NT(76), (NT(222)));
//G332: __E_constraint_97(225) => '[' ctnvar(215) _(11) '<' _(11) num(196) ']'.
	p(NT(225), (T(24)+NT(215)+NT(11)+T(20)+NT(11)+NT(196)+T(25)));
//G333: __E_constraint_97(225) => '[' num(196) _(11) '<' _(11) ctnvar(215) ']'.
	p(NT(225), (T(24)+NT(196)+NT(11)+T(20)+NT(11)+NT(215)+T(25)));
//G334: ctn_less(224)        => __E_constraint_97(225).
	p(NT(224), (NT(225)));
//G335: constraint(76)       => ctn_less(224).
	p(NT(76), (NT(224)));
//G336: __N_0(450)           => 'F'.
	p(NT(450), (T(34)));
//G337: __N_1(451)           => 'T'.
	p(NT(451), (T(33)));
//G338: __E_variable_98(226) => ~( __N_0(450) ) & ~( __N_1(451) ) & alpha(5).	 # conjunctive
	p(NT(226), ~(NT(450)) & ~(NT(451)) & (NT(5)));
//G339: __E_variable_99(227) => null.
	p(NT(227), (nul));
//G340: __E_variable_99(227) => digit(3) __E_variable_99(227).
	p(NT(227), (NT(3)+NT(227)));
//G341: __E_variable_100(228) => _(11) ':' _(11) type(150).
	p(NT(228), (NT(11)+T(2)+NT(11)+NT(150)));
//G342: __E_variable_100(228) => null.
	p(NT(228), (nul));
//G343: variable(139)        => __E_variable_98(226) __E_variable_99(227) __E_variable_100(228).	 # guarded: charvar
	p(NT(139), (NT(226)+NT(227)+NT(228)));
	p.back().guard = "charvar";
//G344: __N_2(452)           => 'F'.
	p(NT(452), (T(34)));
//G345: __N_3(453)           => 'T'.
	p(NT(453), (T(33)));
//G346: __E_variable_101(229) => ~( __N_2(452) ) & ~( __N_3(453) ) & alpha(5).	 # conjunctive
	p(NT(229), ~(NT(452)) & ~(NT(453)) & (NT(5)));
//G347: __E_variable_102(230) => alnum(6).
	p(NT(230), (NT(6)));
//G348: __E_variable_102(230) => '_'.
	p(NT(230), (T(35)));
//G349: __E_variable_103(231) => null.
	p(NT(231), (nul));
//G350: __E_variable_103(231) => __E_variable_102(230) __E_variable_103(231).
	p(NT(231), (NT(230)+NT(231)));
//G351: __E_variable_104(232) => _(11) ':' _(11) type(150).
	p(NT(232), (NT(11)+T(2)+NT(11)+NT(150)));
//G352: __E_variable_104(232) => null.
	p(NT(232), (nul));
//G353: variable(139)        => __E_variable_101(229) __E_variable_103(231) __E_variable_104(232).	 # guarded: var
	p(NT(139), (NT(229)+NT(231)+NT(232)));
	p.back().guard = "var";
//G354: __E_variable_105(234) => in(235).
	p(NT(234), (NT(235)));
//G355: __E_variable_105(234) => out(236).
	p(NT(234), (NT(236)));
//G356: io_var(233)          => __E_variable_105(234).
	p(NT(233), (NT(234)));
//G357: __E_variable_106(237) => _(11) ':' _(11) type(150).
	p(NT(237), (NT(11)+T(2)+NT(11)+NT(150)));
//G358: __E_variable_106(237) => null.
	p(NT(237), (nul));
//G359: variable(139)        => io_var(233) __E_variable_106(237).
	p(NT(139), (NT(233)+NT(237)));
//G360: variable(139)        => uninterpreted_constant(238).
	p(NT(139), (NT(238)));
//G361: __E___E_in_107_108(241) => digit(3).
	p(NT(241), (NT(3)));
//G362: __E___E_in_107_108(241) => digit(3) __E___E_in_107_108(241).
	p(NT(241), (NT(3)+NT(241)));
//G363: __E_in_107(240)      => 'i' __E___E_in_107_108(241).
	p(NT(240), (T(10)+NT(241)));
//G364: in_var_name(239)     => __E_in_107(240).
	p(NT(239), (NT(240)));
//G365: in(235)              => in_var_name(239) '[' offset(242) ']'.
	p(NT(235), (NT(239)+T(24)+NT(242)+T(25)));
//G366: __E___E___E_out_109_110_111(246) => digit(3).
	p(NT(246), (NT(3)));
//G367: __E___E___E_out_109_110_111(246) => digit(3) __E___E___E_out_109_110_111(246).
	p(NT(246), (NT(3)+NT(246)));
//G368: __E___E_out_109_110(245) => 'o' __E___E___E_out_109_110_111(246).
	p(NT(245), (T(17)+NT(246)));
//G369: __E_out_109(244)     => __E___E_out_109_110(245).
	p(NT(244), (NT(245)));
//G370: __E_out_109(244)     => 'u'.
	p(NT(244), (T(47)));
//G371: out_var_name(243)    => __E_out_109(244).
	p(NT(243), (NT(244)));
//G372: out(236)             => out_var_name(243) '[' offset(242) ']'.
	p(NT(236), (NT(243)+T(24)+NT(242)+T(25)));
//G373: ctnvar(215)          => chars(247).
	p(NT(215), (NT(247)));
//G374: capture(20)          => '$' chars(247).
	p(NT(20), (T(48)+NT(247)));
//G375: __E_q_vars_112(249)  => _(11) ',' _(11) q_var(248).
	p(NT(249), (NT(11)+T(14)+NT(11)+NT(248)));
//G376: __E_q_vars_113(250)  => null.
	p(NT(250), (nul));
//G377: __E_q_vars_113(250)  => __E_q_vars_112(249) __E_q_vars_113(250).
	p(NT(250), (NT(249)+NT(250)));
//G378: q_vars(72)           => q_var(248) __E_q_vars_113(250).
	p(NT(72), (NT(248)+NT(250)));
//G379: q_var(248)           => capture(20).
	p(NT(248), (NT(20)));
//G380: __N_4(454)           => uninterpreted_constant(238).
	p(NT(454), (NT(238)));
//G381: __E_q_var_114(251)   => variable(139) & ~( __N_4(454) ).	 # conjunctive
	p(NT(251), (NT(139)) & ~(NT(454)));
//G382: q_var(248)           => __E_q_var_114(251).
	p(NT(248), (NT(251)));
//G383: __E_offsets_115(252) => _(11) ',' _(11) offset(242).
	p(NT(252), (NT(11)+T(14)+NT(11)+NT(242)));
//G384: __E_offsets_116(253) => null.
	p(NT(253), (nul));
//G385: __E_offsets_116(253) => __E_offsets_115(252) __E_offsets_116(253).
	p(NT(253), (NT(252)+NT(253)));
//G386: offsets(24)          => '[' _(11) offset(242) __E_offsets_116(253) _(11) ']'.
	p(NT(24), (T(24)+NT(11)+NT(242)+NT(253)+NT(11)+T(25)));
//G387: offset(242)          => integer(254).
	p(NT(242), (NT(254)));
//G388: offset(242)          => capture(20).
	p(NT(242), (NT(20)));
//G389: offset(242)          => shift(255).
	p(NT(242), (NT(255)));
//G390: __N_5(455)           => io_var(233).
	p(NT(455), (NT(233)));
//G391: __E_offset_117(256)  => variable(139) & ~( __N_5(455) ).	 # conjunctive
	p(NT(256), (NT(139)) & ~(NT(455)));
//G392: offset(242)          => __E_offset_117(256).
	p(NT(242), (NT(256)));
//G393: __E_shift_118(257)   => capture(20).
	p(NT(257), (NT(20)));
//G394: __N_6(456)           => io_var(233).
	p(NT(456), (NT(233)));
//G395: __E___E_shift_118_119(258) => variable(139) & ~( __N_6(456) ).	 # conjunctive
	p(NT(258), (NT(139)) & ~(NT(456)));
//G396: __E_shift_118(257)   => __E___E_shift_118_119(258).
	p(NT(257), (NT(258)));
//G397: shift(255)           => __E_shift_118(257) _(11) '-' _(11) num(196).
	p(NT(255), (NT(257)+NT(11)+T(28)+NT(11)+NT(196)));
//G398: num(196)             => digits(259).
	p(NT(196), (NT(259)));
//G399: __E_integer_120(260) => '-'.
	p(NT(260), (T(28)));
//G400: __E_integer_120(260) => null.
	p(NT(260), (nul));
//G401: integer(254)         => __E_integer_120(260) _(11) digits(259).
	p(NT(254), (NT(260)+NT(11)+NT(259)));
//G402: __E___E_uninterpreted_constant_121_122(263) => chars(247).
	p(NT(263), (NT(247)));
//G403: __E___E_uninterpreted_constant_121_122(263) => _(11).
	p(NT(263), (NT(11)));
//G404: __E_uninterpreted_constant_121(262) => __E___E_uninterpreted_constant_121_122(263) ':' chars(247).
	p(NT(262), (NT(263)+T(2)+NT(247)));
//G405: uninter_const_name(261) => __E_uninterpreted_constant_121(262).
	p(NT(261), (NT(262)));
//G406: uninterpreted_constant(238) => '<' uninter_const_name(261) _(11) '>'.
	p(NT(238), (T(20)+NT(261)+NT(11)+T(21)));
//G407: __E_bf_constant_123(265) => capture(20).
	p(NT(265), (NT(20)));
//G408: __E_bf_constant_123(265) => binding(266).
	p(NT(265), (NT(266)));
//G409: constant(264)        => __E_bf_constant_123(265).
	p(NT(264), (NT(265)));
//G410: __E_bf_constant_124(267) => _(11) ':' _(11) type(150).
	p(NT(267), (NT(11)+T(2)+NT(11)+NT(150)));
//G411: __E_bf_constant_124(267) => null.
	p(NT(267), (nul));
//G412: bf_constant(138)     => '{' _(11) constant(264) _(11) '}' __E_bf_constant_124(267).
	p(NT(138), (T(49)+NT(11)+NT(264)+NT(11)+T(50)+NT(267)));
//G413: __E___E___E_binding_125_126_127(272) => src_c(270).
	p(NT(272), (NT(270)));
//G414: __E___E___E_binding_125_126_127(272) => space(2).
	p(NT(272), (NT(2)));
//G415: __E___E___E_binding_125_126_128(273) => null.
	p(NT(273), (nul));
//G416: __E___E___E_binding_125_126_128(273) => __E___E___E_binding_125_126_127(272) __E___E___E_binding_125_126_128(273).
	p(NT(273), (NT(272)+NT(273)));
//G417: __E___E_binding_125_126(271) => __E___E___E_binding_125_126_128(273) src_c(270).
	p(NT(271), (NT(273)+NT(270)));
//G418: __E___E_binding_125_126(271) => null.
	p(NT(271), (nul));
//G419: __E_binding_125(269) => src_c(270) __E___E_binding_125_126(271).
	p(NT(269), (NT(270)+NT(271)));
//G420: source(268)          => __E_binding_125(269).
	p(NT(268), (NT(269)));
//G421: binding(266)         => source(268).
	p(NT(266), (NT(268)));
//G422: src_c(270)           => alnum(6).
	p(NT(270), (NT(6)));
//G423: __N_7(457)           => '{'.
	p(NT(457), (T(49)));
//G424: __N_8(458)           => '}'.
	p(NT(458), (T(50)));
//G425: src_c(270)           => ~( __N_7(457) ) & ~( __N_8(458) ) & punct(7).	 # conjunctive
	p(NT(270), ~(NT(457)) & ~(NT(458)) & (NT(7)));
//G426: __E_src_c_129(274)   => src_c(270).
	p(NT(274), (NT(270)));
//G427: __E_src_c_129(274)   => space(2).
	p(NT(274), (NT(2)));
//G428: __E_src_c_130(275)   => null.
	p(NT(275), (nul));
//G429: __E_src_c_130(275)   => __E_src_c_129(274) __E_src_c_130(275).
	p(NT(275), (NT(274)+NT(275)));
//G430: src_c(270)           => '{' __E_src_c_130(275) '}'.
	p(NT(270), (T(49)+NT(275)+T(50)));
//G431: type(150)            => bv_type(206).
	p(NT(150), (NT(206)));
//G432: type(150)            => chars(247).
	p(NT(150), (NT(247)));
//G433: __E_bv_type_131(276) => _(11) '[' _(11) num(196) _(11) ']'.
	p(NT(276), (NT(11)+T(24)+NT(11)+NT(196)+NT(11)+T(25)));
//G434: __E_bv_type_131(276) => null.
	p(NT(276), (nul));
//G435: bv_type(206)         => 'b' 'v' __E_bv_type_131(276).
	p(NT(206), (T(7)+T(51)+NT(276)));
//G436: __E_chars_132(277)   => alnum(6).
	p(NT(277), (NT(6)));
//G437: __E_chars_132(277)   => '_'.
	p(NT(277), (T(35)));
//G438: __E_chars_133(278)   => null.
	p(NT(278), (nul));
//G439: __E_chars_133(278)   => __E_chars_132(277) __E_chars_133(278).
	p(NT(278), (NT(277)+NT(278)));
//G440: chars(247)           => alpha(5) __E_chars_133(278).
	p(NT(247), (NT(5)+NT(278)));
//G441: __E_digits_134(279)  => digit(3).
	p(NT(279), (NT(3)));
//G442: __E_digits_134(279)  => digit(3) __E_digits_134(279).
	p(NT(279), (NT(3)+NT(279)));
//G443: digits(259)          => __E_digits_134(279).
	p(NT(259), (NT(279)));
//G444: sym(22)              => chars(247).
	p(NT(22), (NT(247)));
//G445: __E_comment_135(281) => printable(8).
	p(NT(281), (NT(8)));
//G446: __E_comment_135(281) => '\t'.
	p(NT(281), (T(52)));
//G447: __E_comment_136(282) => null.
	p(NT(282), (nul));
//G448: __E_comment_136(282) => __E_comment_135(281) __E_comment_136(282).
	p(NT(282), (NT(281)+NT(282)));
//G449: __E_comment_137(283) => '\n'.
	p(NT(283), (T(53)));
//G450: __E_comment_137(283) => '\r'.
	p(NT(283), (T(54)));
//G451: __E_comment_137(283) => eof(1).
	p(NT(283), (NT(1)));
//G452: comment(280)         => '#' __E_comment_136(282) __E_comment_137(283).
	p(NT(280), (T(46)+NT(282)+NT(283)));
//G453: __E____138(284)      => space(2).
	p(NT(284), (NT(2)));
//G454: __E____138(284)      => comment(280).
	p(NT(284), (NT(280)));
//G455: __(27)               => __E____138(284) _(11).
	p(NT(27), (NT(284)+NT(11)));
//G456: __E___139(285)       => __(27).
	p(NT(285), (NT(27)));
//G457: __E___139(285)       => null.
	p(NT(285), (nul));
//G458: _(11)                => __E___139(285).
	p(NT(11), (NT(285)));
//G459: cli(286)             => _(11).
	p(NT(286), (NT(11)));
//G460: __E_cli_140(288)     => '.' _(11) cli_command(287) _(11).
	p(NT(288), (T(1)+NT(11)+NT(287)+NT(11)));
//G461: __E_cli_141(289)     => null.
	p(NT(289), (nul));
//G462: __E_cli_141(289)     => __E_cli_140(288) __E_cli_141(289).
	p(NT(289), (NT(288)+NT(289)));
//G463: cli(286)             => _(11) cli_command(287) _(11) __E_cli_141(289).
	p(NT(286), (NT(11)+NT(287)+NT(11)+NT(289)));
//G464: __E_cli_command_142(292) => 'q'.
	p(NT(292), (T(55)));
//G465: __E_cli_command_142(292) => 'q' 'u' 'i' 't'.
	p(NT(292), (T(55)+T(47)+T(10)+T(13)));
//G466: quit_sym(291)        => __E_cli_command_142(292).
	p(NT(291), (NT(292)));
//G467: quit_cmd(290)        => quit_sym(291).
	p(NT(290), (NT(291)));
//G468: cli_command(287)     => quit_cmd(290).
	p(NT(287), (NT(290)));
//G469: __E_cli_command_143(295) => 'v'.
	p(NT(295), (T(51)));
//G470: __E_cli_command_143(295) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(295), (T(51)+T(19)+T(11)+T(12)+T(10)+T(17)+T(45)));
//G471: version_sym(294)     => __E_cli_command_143(295).
	p(NT(294), (NT(295)));
//G472: version_cmd(293)     => version_sym(294).
	p(NT(293), (NT(294)));
//G473: cli_command(287)     => version_cmd(293).
	p(NT(287), (NT(293)));
//G474: __E_cli_command_144(298) => 'c'.
	p(NT(298), (T(8)));
//G475: __E_cli_command_144(298) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(298), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G476: clear_sym(297)       => __E_cli_command_144(298).
	p(NT(297), (NT(298)));
//G477: clear_cmd(296)       => clear_sym(297).
	p(NT(296), (NT(297)));
//G478: cli_command(287)     => clear_cmd(296).
	p(NT(287), (NT(296)));
//G479: __E___E_cli_command_145_146(302) => 'h'.
	p(NT(302), (T(56)));
//G480: __E___E_cli_command_145_146(302) => 'h' 'e' 'l' 'p'.
	p(NT(302), (T(56)+T(19)+T(6)+T(57)));
//G481: help_sym(301)        => __E___E_cli_command_145_146(302).
	p(NT(301), (NT(302)));
//G482: __E___E_cli_command_145_147(303) => __(27) help_arg(304).
	p(NT(303), (NT(27)+NT(304)));
//G483: __E___E_cli_command_145_147(303) => null.
	p(NT(303), (nul));
//G484: __E_cli_command_145(300) => help_sym(301) __E___E_cli_command_145_147(303).
	p(NT(300), (NT(301)+NT(303)));
//G485: help_cmd(299)        => __E_cli_command_145(300).
	p(NT(299), (NT(300)));
//G486: cli_command(287)     => help_cmd(299).
	p(NT(287), (NT(299)));
//G487: file_sym(307)        => 'f' 'i' 'l' 'e'.
	p(NT(307), (T(4)+T(10)+T(6)+T(19)));
//G488: __E_cli_command_148(306) => file_sym(307) __(27) q_string(308).
	p(NT(306), (NT(307)+NT(27)+NT(308)));
//G489: file_cmd(305)        => __E_cli_command_148(306).
	p(NT(305), (NT(306)));
//G490: cli_command(287)     => file_cmd(305).
	p(NT(287), (NT(305)));
//G491: valid_sym(311)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(311), (T(51)+T(5)+T(6)+T(10)+T(58)));
//G492: __E_cli_command_149(310) => valid_sym(311) __(27) normalize_cmd_arg(312).
	p(NT(310), (NT(311)+NT(27)+NT(312)));
//G493: valid_cmd(309)       => __E_cli_command_149(310).
	p(NT(309), (NT(310)));
//G494: cli_command(287)     => valid_cmd(309).
	p(NT(287), (NT(309)));
//G495: sat_sym(315)         => 's' 'a' 't'.
	p(NT(315), (T(12)+T(5)+T(13)));
//G496: __E_cli_command_150(314) => sat_sym(315) __(27) normalize_cmd_arg(312).
	p(NT(314), (NT(315)+NT(27)+NT(312)));
//G497: sat_cmd(313)         => __E_cli_command_150(314).
	p(NT(313), (NT(314)));
//G498: cli_command(287)     => sat_cmd(313).
	p(NT(287), (NT(313)));
//G499: unsat_sym(318)       => 'u' 'n' 's' 'a' 't'.
	p(NT(318), (T(47)+T(45)+T(12)+T(5)+T(13)));
//G500: __E_cli_command_151(317) => unsat_sym(318) __(27) normalize_cmd_arg(312).
	p(NT(317), (NT(318)+NT(27)+NT(312)));
//G501: unsat_cmd(316)       => __E_cli_command_151(317).
	p(NT(316), (NT(317)));
//G502: cli_command(287)     => unsat_cmd(316).
	p(NT(287), (NT(316)));
//G503: solve_sym(321)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(321), (T(12)+T(17)+T(6)+T(51)+T(19)));
//G504: __E___E_cli_command_152_153(322) => solve_options(323).
	p(NT(322), (NT(323)));
//G505: __E___E_cli_command_152_153(322) => null.
	p(NT(322), (nul));
//G506: __E_cli_command_152(320) => solve_sym(321) __E___E_cli_command_152_153(322) __(27) wff_cmd_arg(324).
	p(NT(320), (NT(321)+NT(322)+NT(27)+NT(324)));
//G507: solve_cmd(319)       => __E_cli_command_152(320).
	p(NT(319), (NT(320)));
//G508: cli_command(287)     => solve_cmd(319).
	p(NT(287), (NT(319)));
//G509: lgrs_sym(327)        => 'l' 'g' 'r' 's'.
	p(NT(327), (T(6)+T(59)+T(11)+T(12)));
//G510: __E_cli_command_154(326) => lgrs_sym(327) __(27) wff_cmd_arg(324).
	p(NT(326), (NT(327)+NT(27)+NT(324)));
//G511: lgrs_cmd(325)        => __E_cli_command_154(326).
	p(NT(325), (NT(326)));
//G512: cli_command(287)     => lgrs_cmd(325).
	p(NT(287), (NT(325)));
//G513: __E___E_cli_command_155_156(331) => 'r'.
	p(NT(331), (T(11)));
//G514: __E___E_cli_command_155_156(331) => 'r' 'u' 'n'.
	p(NT(331), (T(11)+T(47)+T(45)));
//G515: run_sym(330)         => __E___E_cli_command_155_156(331).
	p(NT(330), (NT(331)));
//G516: __E___E_cli_command_155_157(332) => memory(333).
	p(NT(332), (NT(333)));
//G517: __E___E_cli_command_155_157(332) => wff(14).
	p(NT(332), (NT(14)));
//G518: __E_cli_command_155(329) => run_sym(330) __(27) __E___E_cli_command_155_157(332).
	p(NT(329), (NT(330)+NT(27)+NT(332)));
//G519: run_cmd(328)         => __E_cli_command_155(329).
	p(NT(328), (NT(329)));
//G520: cli_command(287)     => run_cmd(328).
	p(NT(287), (NT(328)));
//G521: __E___E_cli_command_158_159(337) => 'n'.
	p(NT(337), (T(45)));
//G522: __E___E_cli_command_158_159(337) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(337), (T(45)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(60)+T(19)));
//G523: normalize_sym(336)   => __E___E_cli_command_158_159(337).
	p(NT(336), (NT(337)));
//G524: __E_cli_command_158(335) => normalize_sym(336) __(27) normalize_cmd_arg(312).
	p(NT(335), (NT(336)+NT(27)+NT(312)));
//G525: normalize_cmd(334)   => __E_cli_command_158(335).
	p(NT(334), (NT(335)));
//G526: cli_command(287)     => normalize_cmd(334).
	p(NT(287), (NT(334)));
//G527: __E___E_cli_command_160_161(341) => 's'.
	p(NT(341), (T(12)));
//G528: __E___E_cli_command_160_161(341) => 's' 'u' 'b' 's' 't'.
	p(NT(341), (T(12)+T(47)+T(7)+T(12)+T(13)));
//G529: __E___E_cli_command_160_161(341) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(341), (T(12)+T(47)+T(7)+T(12)+T(13)+T(10)+T(13)+T(47)+T(13)+T(19)));
//G530: subst_sym(340)       => __E___E_cli_command_160_161(341).
	p(NT(340), (NT(341)));
//G531: __E_cli_command_160(339) => subst_sym(340) __(27) nf_cmd_arg(342) _(11) '[' _(11) nf_cmd_arg(342) _(11) '/' _(11) nf_cmd_arg(342) _(11) ']'.
	p(NT(339), (NT(340)+NT(27)+NT(342)+NT(11)+T(24)+NT(11)+NT(342)+NT(11)+T(43)+NT(11)+NT(342)+NT(11)+T(25)));
//G532: subst_cmd(338)       => __E_cli_command_160(339).
	p(NT(338), (NT(339)));
//G533: cli_command(287)     => subst_cmd(338).
	p(NT(287), (NT(338)));
//G534: __E___E_cli_command_162_163(346) => 'i'.
	p(NT(346), (T(10)));
//G535: __E___E_cli_command_162_163(346) => 'i' 'n' 's' 't'.
	p(NT(346), (T(10)+T(45)+T(12)+T(13)));
//G536: __E___E_cli_command_162_163(346) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(346), (T(10)+T(45)+T(12)+T(13)+T(5)+T(45)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G537: inst_sym(345)        => __E___E_cli_command_162_163(346).
	p(NT(345), (NT(346)));
//G538: __E_cli_command_162(344) => inst_sym(345) __(27) inst_args(347).
	p(NT(344), (NT(345)+NT(27)+NT(347)));
//G539: inst_cmd(343)        => __E_cli_command_162(344).
	p(NT(343), (NT(344)));
//G540: cli_command(287)     => inst_cmd(343).
	p(NT(287), (NT(343)));
//G541: dnf_sym(350)         => 'd' 'n' 'f'.
	p(NT(350), (T(58)+T(45)+T(4)));
//G542: __E_cli_command_164(349) => dnf_sym(350) __(27) nf_cmd_arg(342).
	p(NT(349), (NT(350)+NT(27)+NT(342)));
//G543: dnf_cmd(348)         => __E_cli_command_164(349).
	p(NT(348), (NT(349)));
//G544: cli_command(287)     => dnf_cmd(348).
	p(NT(287), (NT(348)));
//G545: cnf_sym(353)         => 'c' 'n' 'f'.
	p(NT(353), (T(8)+T(45)+T(4)));
//G546: __E_cli_command_165(352) => cnf_sym(353) __(27) nf_cmd_arg(342).
	p(NT(352), (NT(353)+NT(27)+NT(342)));
//G547: cnf_cmd(351)         => __E_cli_command_165(352).
	p(NT(351), (NT(352)));
//G548: cli_command(287)     => cnf_cmd(351).
	p(NT(287), (NT(351)));
//G549: anf_sym(356)         => 'a' 'n' 'f'.
	p(NT(356), (T(5)+T(45)+T(4)));
//G550: __E_cli_command_166(355) => anf_sym(356) __(27) nf_cmd_arg(342).
	p(NT(355), (NT(356)+NT(27)+NT(342)));
//G551: anf_cmd(354)         => __E_cli_command_166(355).
	p(NT(354), (NT(355)));
//G552: cli_command(287)     => anf_cmd(354).
	p(NT(287), (NT(354)));
//G553: nnf_sym(359)         => 'n' 'n' 'f'.
	p(NT(359), (T(45)+T(45)+T(4)));
//G554: __E_cli_command_167(358) => nnf_sym(359) __(27) nf_cmd_arg(342).
	p(NT(358), (NT(359)+NT(27)+NT(342)));
//G555: nnf_cmd(357)         => __E_cli_command_167(358).
	p(NT(357), (NT(358)));
//G556: cli_command(287)     => nnf_cmd(357).
	p(NT(287), (NT(357)));
//G557: pnf_sym(362)         => 'p' 'n' 'f'.
	p(NT(362), (T(57)+T(45)+T(4)));
//G558: __E_cli_command_168(361) => pnf_sym(362) __(27) nf_cmd_arg(342).
	p(NT(361), (NT(362)+NT(27)+NT(342)));
//G559: pnf_cmd(360)         => __E_cli_command_168(361).
	p(NT(360), (NT(361)));
//G560: cli_command(287)     => pnf_cmd(360).
	p(NT(287), (NT(360)));
//G561: mnf_sym(365)         => 'm' 'n' 'f'.
	p(NT(365), (T(18)+T(45)+T(4)));
//G562: __E_cli_command_169(364) => mnf_sym(365) __(27) nf_cmd_arg(342).
	p(NT(364), (NT(365)+NT(27)+NT(342)));
//G563: mnf_cmd(363)         => __E_cli_command_169(364).
	p(NT(363), (NT(364)));
//G564: cli_command(287)     => mnf_cmd(363).
	p(NT(287), (NT(363)));
//G565: snf_sym(368)         => 's' 'n' 'f'.
	p(NT(368), (T(12)+T(45)+T(4)));
//G566: __E_cli_command_170(367) => snf_sym(368) __(27) nf_cmd_arg(342).
	p(NT(367), (NT(368)+NT(27)+NT(342)));
//G567: snf_cmd(366)         => __E_cli_command_170(367).
	p(NT(366), (NT(367)));
//G568: cli_command(287)     => snf_cmd(366).
	p(NT(287), (NT(366)));
//G569: onf_sym(371)         => 'o' 'n' 'f'.
	p(NT(371), (T(17)+T(45)+T(4)));
//G570: __E_cli_command_171(370) => onf_sym(371) __(27) variable(139) __(27) onf_cmd_arg(372).
	p(NT(370), (NT(371)+NT(27)+NT(139)+NT(27)+NT(372)));
//G571: onf_cmd(369)         => __E_cli_command_171(370).
	p(NT(369), (NT(370)));
//G572: cli_command(287)     => onf_cmd(369).
	p(NT(287), (NT(369)));
//G573: __E___E_cli_command_172_173(376) => 'd' 'e' 'f' 's'.
	p(NT(376), (T(58)+T(19)+T(4)+T(12)));
//G574: __E___E_cli_command_172_173(376) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(376), (T(58)+T(19)+T(4)+T(10)+T(45)+T(10)+T(13)+T(10)+T(17)+T(45)+T(12)));
//G575: def_sym(375)         => __E___E_cli_command_172_173(376).
	p(NT(375), (NT(376)));
//G576: __E_cli_command_172(374) => def_sym(375).
	p(NT(374), (NT(375)));
//G577: def_list_cmd(373)    => __E_cli_command_172(374).
	p(NT(373), (NT(374)));
//G578: cli_command(287)     => def_list_cmd(373).
	p(NT(287), (NT(373)));
//G579: __E_cli_command_174(378) => def_sym(375) __(27) number(379).
	p(NT(378), (NT(375)+NT(27)+NT(379)));
//G580: def_print_cmd(377)   => __E_cli_command_174(378).
	p(NT(377), (NT(378)));
//G581: cli_command(287)     => def_print_cmd(377).
	p(NT(287), (NT(377)));
//G582: def_rr_cmd(380)      => rec_relation(16).
	p(NT(380), (NT(16)));
//G583: cli_command(287)     => def_rr_cmd(380).
	p(NT(287), (NT(380)));
//G584: qelim_sym(383)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(383), (T(55)+T(19)+T(6)+T(10)+T(18)));
//G585: __E_cli_command_175(382) => qelim_sym(383) __(27) wff_cmd_arg(324).
	p(NT(382), (NT(383)+NT(27)+NT(324)));
//G586: qelim_cmd(381)       => __E_cli_command_175(382).
	p(NT(381), (NT(382)));
//G587: cli_command(287)     => qelim_cmd(381).
	p(NT(287), (NT(381)));
//G588: get_sym(386)         => 'g' 'e' 't'.
	p(NT(386), (T(59)+T(19)+T(13)));
//G589: __E___E_cli_command_176_177(387) => __(27) option_name(388).
	p(NT(387), (NT(27)+NT(388)));
//G590: __E___E_cli_command_176_177(387) => null.
	p(NT(387), (nul));
//G591: __E_cli_command_176(385) => get_sym(386) __E___E_cli_command_176_177(387).
	p(NT(385), (NT(386)+NT(387)));
//G592: get_cmd(384)         => __E_cli_command_176(385).
	p(NT(384), (NT(385)));
//G593: cli_command(287)     => get_cmd(384).
	p(NT(287), (NT(384)));
//G594: set_sym(391)         => 's' 'e' 't'.
	p(NT(391), (T(12)+T(19)+T(13)));
//G595: __E___E_cli_command_178_179(392) => __(27).
	p(NT(392), (NT(27)));
//G596: __E___E_cli_command_178_179(392) => _(11) '=' _(11).
	p(NT(392), (NT(11)+T(3)+NT(11)));
//G597: __E_cli_command_178(390) => set_sym(391) __(27) option_name(388) __E___E_cli_command_178_179(392) option_value(393).
	p(NT(390), (NT(391)+NT(27)+NT(388)+NT(392)+NT(393)));
//G598: set_cmd(389)         => __E_cli_command_178(390).
	p(NT(389), (NT(390)));
//G599: cli_command(287)     => set_cmd(389).
	p(NT(287), (NT(389)));
//G600: enable_sym(396)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(396), (T(19)+T(45)+T(5)+T(7)+T(6)+T(19)));
//G601: __E_cli_command_180(395) => enable_sym(396) __(27) option_name(388).
	p(NT(395), (NT(396)+NT(27)+NT(388)));
//G602: enable_cmd(394)      => __E_cli_command_180(395).
	p(NT(394), (NT(395)));
//G603: cli_command(287)     => enable_cmd(394).
	p(NT(287), (NT(394)));
//G604: disable_sym(399)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(399), (T(58)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G605: __E_cli_command_181(398) => disable_sym(399) __(27) option_name(388).
	p(NT(398), (NT(399)+NT(27)+NT(388)));
//G606: disable_cmd(397)     => __E_cli_command_181(398).
	p(NT(397), (NT(398)));
//G607: cli_command(287)     => disable_cmd(397).
	p(NT(287), (NT(397)));
//G608: toggle_sym(402)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(402), (T(13)+T(17)+T(59)+T(59)+T(6)+T(19)));
//G609: __E_cli_command_182(401) => toggle_sym(402) __(27) option_name(388).
	p(NT(401), (NT(402)+NT(27)+NT(388)));
//G610: toggle_cmd(400)      => __E_cli_command_182(401).
	p(NT(400), (NT(401)));
//G611: cli_command(287)     => toggle_cmd(400).
	p(NT(287), (NT(400)));
//G612: __E_cli_command_183(404) => type(150) __(27) in_var_name(239) _(11) '=' _(11) input_stream(405).
	p(NT(404), (NT(150)+NT(27)+NT(239)+NT(11)+T(3)+NT(11)+NT(405)));
//G613: def_input_cmd(403)   => __E_cli_command_183(404).
	p(NT(403), (NT(404)));
//G614: cli_command(287)     => def_input_cmd(403).
	p(NT(287), (NT(403)));
//G615: __E_cli_command_184(407) => type(150) __(27) out_var_name(243) _(11) '=' _(11) output_stream(408).
	p(NT(407), (NT(150)+NT(27)+NT(243)+NT(11)+T(3)+NT(11)+NT(408)));
//G616: def_output_cmd(406)  => __E_cli_command_184(407).
	p(NT(406), (NT(407)));
//G617: cli_command(287)     => def_output_cmd(406).
	p(NT(287), (NT(406)));
//G618: __E___E_cli_command_185_186(412) => 'h' 'i' 's' 't'.
	p(NT(412), (T(56)+T(10)+T(12)+T(13)));
//G619: __E___E_cli_command_185_186(412) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(412), (T(56)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G620: history_sym(411)     => __E___E_cli_command_185_186(412).
	p(NT(411), (NT(412)));
//G621: __E_cli_command_185(410) => history_sym(411).
	p(NT(410), (NT(411)));
//G622: history_list_cmd(409) => __E_cli_command_185(410).
	p(NT(409), (NT(410)));
//G623: cli_command(287)     => history_list_cmd(409).
	p(NT(287), (NT(409)));
//G624: __E_cli_command_187(414) => history_sym(411) __(27) memory(333).
	p(NT(414), (NT(411)+NT(27)+NT(333)));
//G625: history_print_cmd(413) => __E_cli_command_187(414).
	p(NT(413), (NT(414)));
//G626: cli_command(287)     => history_print_cmd(413).
	p(NT(287), (NT(413)));
//G627: __E_cli_command_188(416) => wff(14).
	p(NT(416), (NT(14)));
//G628: __E_cli_command_188(416) => bf(21).
	p(NT(416), (NT(21)));
//G629: history_store_cmd(415) => __E_cli_command_188(416).
	p(NT(415), (NT(416)));
//G630: cli_command(287)     => history_store_cmd(415).
	p(NT(287), (NT(415)));
//G631: __E_solve_options_189(417) => __(27) solve_option(418).
	p(NT(417), (NT(27)+NT(418)));
//G632: __E_solve_options_190(419) => null.
	p(NT(419), (nul));
//G633: __E_solve_options_190(419) => __E_solve_options_189(417) __E_solve_options_190(419).
	p(NT(419), (NT(417)+NT(419)));
//G634: solve_options(323)   => __E_solve_options_190(419).
	p(NT(323), (NT(419)));
//G635: __E_solve_option_191(420) => '-' '-' solver_mode(421).
	p(NT(420), (T(28)+T(28)+NT(421)));
//G636: solve_option(418)    => __E_solve_option_191(420).
	p(NT(418), (NT(420)));
//G637: __E_solve_option_192(422) => '-' '-' type(150).
	p(NT(422), (T(28)+T(28)+NT(150)));
//G638: solve_option(418)    => __E_solve_option_192(422).
	p(NT(418), (NT(422)));
//G639: __E_solver_mode_193(424) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(424), (T(18)+T(10)+T(45)+T(10)+T(18)+T(47)+T(18)));
//G640: __E_solver_mode_193(424) => 'm' 'i' 'n'.
	p(NT(424), (T(18)+T(10)+T(45)));
//G641: solver_mode_minimum(423) => __E_solver_mode_193(424).
	p(NT(423), (NT(424)));
//G642: solver_mode(421)     => solver_mode_minimum(423).
	p(NT(421), (NT(423)));
//G643: __E_solver_mode_194(426) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(426), (T(18)+T(5)+T(27)+T(10)+T(18)+T(47)+T(18)));
//G644: __E_solver_mode_194(426) => 'm' 'a' 'x'.
	p(NT(426), (T(18)+T(5)+T(27)));
//G645: solver_mode_maximum(425) => __E_solver_mode_194(426).
	p(NT(425), (NT(426)));
//G646: solver_mode(421)     => solver_mode_maximum(425).
	p(NT(421), (NT(425)));
//G647: __E_input_stream_195(427) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(428) _(11) ')'.
	p(NT(427), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(428)+NT(11)+T(16)));
//G648: input_stream(405)    => __E_input_stream_195(427).
	p(NT(405), (NT(427)));
//G649: console_sym(429)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(429), (T(8)+T(17)+T(45)+T(12)+T(17)+T(6)+T(19)));
//G650: input_stream(405)    => console_sym(429).
	p(NT(405), (NT(429)));
//G651: __E_output_stream_196(430) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(428) _(11) ')'.
	p(NT(430), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(428)+NT(11)+T(16)));
//G652: output_stream(408)   => __E_output_stream_196(430).
	p(NT(408), (NT(430)));
//G653: output_stream(408)   => console_sym(429).
	p(NT(408), (NT(429)));
//G654: q_file_name(428)     => '"' file_name(431) '"'.
	p(NT(428), (T(61)+NT(431)+T(61)));
//G655: __E_file_name_197(432) => printable(8).
	p(NT(432), (NT(8)));
//G656: __E_file_name_197(432) => printable(8) __E_file_name_197(432).
	p(NT(432), (NT(8)+NT(432)));
//G657: file_name(431)       => __E_file_name_197(432).
	p(NT(431), (NT(432)));
//G658: __E_option_name_198(433) => alnum(6).
	p(NT(433), (NT(6)));
//G659: __E_option_name_198(433) => alnum(6) __E_option_name_198(433).
	p(NT(433), (NT(6)+NT(433)));
//G660: option_name(388)     => __E_option_name_198(433).
	p(NT(388), (NT(433)));
//G661: __E_option_value_199(434) => alnum(6).
	p(NT(434), (NT(6)));
//G662: __E_option_value_199(434) => alnum(6) __E_option_value_199(434).
	p(NT(434), (NT(6)+NT(434)));
//G663: option_value(393)    => __E_option_value_199(434).
	p(NT(393), (NT(434)));
//G664: number(379)          => digits(259).
	p(NT(379), (NT(259)));
//G665: bf_cmd_arg(435)      => memory(333).
	p(NT(435), (NT(333)));
//G666: bf_cmd_arg(435)      => bf(21).
	p(NT(435), (NT(21)));
//G667: wff_cmd_arg(324)     => memory(333).
	p(NT(324), (NT(333)));
//G668: wff_cmd_arg(324)     => wff(14).
	p(NT(324), (NT(14)));
//G669: nf_cmd_arg(342)      => memory(333).
	p(NT(342), (NT(333)));
//G670: nf_cmd_arg(342)      => ref(18).
	p(NT(342), (NT(18)));
//G671: nf_cmd_arg(342)      => wff(14).
	p(NT(342), (NT(14)));
//G672: nf_cmd_arg(342)      => bf(21).
	p(NT(342), (NT(21)));
//G673: onf_cmd_arg(372)     => memory(333).
	p(NT(372), (NT(333)));
//G674: onf_cmd_arg(372)     => wff(14).
	p(NT(372), (NT(14)));
//G675: normalize_cmd_arg(312) => memory(333).
	p(NT(312), (NT(333)));
//G676: normalize_cmd_arg(312) => rr(10).
	p(NT(312), (NT(10)));
//G677: normalize_cmd_arg(312) => ref(18).
	p(NT(312), (NT(18)));
//G678: normalize_cmd_arg(312) => wff(14).
	p(NT(312), (NT(14)));
//G679: normalize_cmd_arg(312) => bf(21).
	p(NT(312), (NT(21)));
//G680: inst_args(347)       => wff_cmd_arg(324) _(11) '[' _(11) variable(139) _(11) '/' _(11) bf_cmd_arg(435) _(11) ']'.
	p(NT(347), (NT(324)+NT(11)+T(24)+NT(11)+NT(139)+NT(11)+T(43)+NT(11)+NT(435)+NT(11)+T(25)));
//G681: inst_args(347)       => bf_cmd_arg(435) _(11) '[' _(11) variable(139) _(11) '/' _(11) bf_cmd_arg(435) _(11) ']'.
	p(NT(347), (NT(435)+NT(11)+T(24)+NT(11)+NT(139)+NT(11)+T(43)+NT(11)+NT(435)+NT(11)+T(25)));
//G682: help_arg(304)        => help_sym(301).
	p(NT(304), (NT(301)));
//G683: help_arg(304)        => version_sym(294).
	p(NT(304), (NT(294)));
//G684: help_arg(304)        => quit_sym(291).
	p(NT(304), (NT(291)));
//G685: help_arg(304)        => clear_sym(297).
	p(NT(304), (NT(297)));
//G686: help_arg(304)        => get_sym(386).
	p(NT(304), (NT(386)));
//G687: help_arg(304)        => set_sym(391).
	p(NT(304), (NT(391)));
//G688: help_arg(304)        => enable_sym(396).
	p(NT(304), (NT(396)));
//G689: help_arg(304)        => disable_sym(399).
	p(NT(304), (NT(399)));
//G690: help_arg(304)        => toggle_sym(402).
	p(NT(304), (NT(402)));
//G691: help_arg(304)        => file_sym(307).
	p(NT(304), (NT(307)));
//G692: help_arg(304)        => history_sym(411).
	p(NT(304), (NT(411)));
//G693: help_arg(304)        => abs_memory_sym(436).
	p(NT(304), (NT(436)));
//G694: help_arg(304)        => rel_memory_sym(437).
	p(NT(304), (NT(437)));
//G695: help_arg(304)        => selection_sym(438).
	p(NT(304), (NT(438)));
//G696: help_arg(304)        => def_sym(375).
	p(NT(304), (NT(375)));
//G697: help_arg(304)        => inst_sym(345).
	p(NT(304), (NT(345)));
//G698: help_arg(304)        => subst_sym(340).
	p(NT(304), (NT(340)));
//G699: help_arg(304)        => normalize_sym(336).
	p(NT(304), (NT(336)));
//G700: help_arg(304)        => execute_sym(439).
	p(NT(304), (NT(439)));
//G701: help_arg(304)        => solve_sym(321).
	p(NT(304), (NT(321)));
//G702: help_arg(304)        => valid_sym(311).
	p(NT(304), (NT(311)));
//G703: help_arg(304)        => sat_sym(315).
	p(NT(304), (NT(315)));
//G704: help_arg(304)        => unsat_sym(318).
	p(NT(304), (NT(318)));
//G705: help_arg(304)        => run_sym(330).
	p(NT(304), (NT(330)));
//G706: help_arg(304)        => dnf_sym(350).
	p(NT(304), (NT(350)));
//G707: help_arg(304)        => cnf_sym(353).
	p(NT(304), (NT(353)));
//G708: help_arg(304)        => anf_sym(356).
	p(NT(304), (NT(356)));
//G709: help_arg(304)        => snf_sym(368).
	p(NT(304), (NT(368)));
//G710: help_arg(304)        => nnf_sym(359).
	p(NT(304), (NT(359)));
//G711: help_arg(304)        => pnf_sym(362).
	p(NT(304), (NT(362)));
//G712: help_arg(304)        => mnf_sym(365).
	p(NT(304), (NT(365)));
//G713: help_arg(304)        => onf_sym(371).
	p(NT(304), (NT(371)));
//G714: help_arg(304)        => qelim_sym(383).
	p(NT(304), (NT(383)));
//G715: __E___E_help_arg_200_201(442) => 's'.
	p(NT(442), (T(12)));
//G716: __E___E_help_arg_200_201(442) => null.
	p(NT(442), (nul));
//G717: __E_help_arg_200(441) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_200_201(442).
	p(NT(441), (T(19)+T(27)+T(5)+T(18)+T(57)+T(6)+T(19)+NT(442)));
//G718: examples_sym(440)    => __E_help_arg_200(441).
	p(NT(440), (NT(441)));
//G719: help_arg(304)        => examples_sym(440).
	p(NT(304), (NT(440)));
//G720: __E_memory_202(444)  => '%'.
	p(NT(444), (T(44)));
//G721: rel_memory_sym(437)  => '%' '-'.
	p(NT(437), (T(44)+T(28)));
//G722: memory_id(446)       => digits(259).
	p(NT(446), (NT(259)));
//G723: __E___E_memory_202_203(445) => memory_id(446).
	p(NT(445), (NT(446)));
//G724: __E___E_memory_202_203(445) => null.
	p(NT(445), (nul));
//G725: __E_memory_202(444)  => rel_memory_sym(437) __E___E_memory_202_203(445).
	p(NT(444), (NT(437)+NT(445)));
//G726: rel_memory(443)      => __E_memory_202(444).
	p(NT(443), (NT(444)));
//G727: memory(333)          => rel_memory(443).
	p(NT(333), (NT(443)));
//G728: abs_memory_sym(436)  => '%'.
	p(NT(436), (T(44)));
//G729: __E_memory_204(448)  => abs_memory_sym(436) memory_id(446).
	p(NT(448), (NT(436)+NT(446)));
//G730: abs_memory(447)      => __E_memory_204(448).
	p(NT(447), (NT(448)));
//G731: memory(333)          => abs_memory(447).
	p(NT(333), (NT(447)));
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
		__E_wff_41, bf_less, __E_wff_42, bf_nless, __E_wff_43, bv_eq, __E_wff_44, bv, bv_neq, __E_wff_45, 
		bv_less_equal, __E_wff_46, bv_nleq, __E_wff_47, bv_greater, __E_wff_48, bv_ngreater, __E_wff_49, bv_greater_equal, __E_wff_50, 
		bv_ngeq, __E_wff_51, bv_less, __E_wff_52, bv_nless, __E_wff_53, bf_parenthesis, __E_bf_54, bf_constant, variable, 
		bf_splitter, __E_bf_55, bf_ref, bf_or, __E_bf_56, bf_xor, __E_bf_57, bf_t, __E_bf_58, __E___E_bf_58_59, 
		type, bf_f, __E_bf_60, __E___E_bf_60_61, bf_and, __E_bf_62, __E___E_bf_62_63, bf_neg, __E_bf_64, bf_neg_oprnd, 
		__E___E_bf_64_65, bf_and_nosep, __E_bf_66, bf_and_nosep_1st_oprnd, __E___E_bf_66_67, bf_and_nosep_2nd_oprnd, __E___E_bf_66_68, bv_parenthesis, __E_bv_69, bv_checked, 
		__E_bv_70, bv_neg, bv_add, __E_bv_71, bv_sub, __E_bv_72, bv_mul, __E_bv_73, bv_div, __E_bv_74, 
		bv_mod, __E_bv_75, bv_nand, __E_bv_76, bv_and, __E_bv_77, bv_nor, __E_bv_78, bv_or, __E_bv_79, 
		bv_xnor, __E_bv_80, bv_xor, __E_bv_81, bv_rotate_left, __E_bv_82, num, bv_rotate_right, __E_bv_83, bv_min, 
		__E_bv_84, bv_max, __E_bv_85, bitvector, bits, __E_bitvector_86, bv_type, __E_bitvector_87, hexnum, __E_bitvector_88, 
		__E_bits_89, __E_bits_90, __E_hexnum_91, ctn_neq, __E_constraint_92, ctnvar, ctn_eq, __E_constraint_93, ctn_greater_equal, __E_constraint_94, 
		ctn_greater, __E_constraint_95, ctn_less_equal, __E_constraint_96, ctn_less, __E_constraint_97, __E_variable_98, __E_variable_99, __E_variable_100, __E_variable_101, 
		__E_variable_102, __E_variable_103, __E_variable_104, io_var, __E_variable_105, in, out, __E_variable_106, uninterpreted_constant, in_var_name, 
		__E_in_107, __E___E_in_107_108, offset, out_var_name, __E_out_109, __E___E_out_109_110, __E___E___E_out_109_110_111, chars, q_var, __E_q_vars_112, 
		__E_q_vars_113, __E_q_var_114, __E_offsets_115, __E_offsets_116, integer, shift, __E_offset_117, __E_shift_118, __E___E_shift_118_119, digits, 
		__E_integer_120, uninter_const_name, __E_uninterpreted_constant_121, __E___E_uninterpreted_constant_121_122, constant, __E_bf_constant_123, binding, __E_bf_constant_124, source, __E_binding_125, 
		src_c, __E___E_binding_125_126, __E___E___E_binding_125_126_127, __E___E___E_binding_125_126_128, __E_src_c_129, __E_src_c_130, __E_bv_type_131, __E_chars_132, __E_chars_133, __E_digits_134, 
		comment, __E_comment_135, __E_comment_136, __E_comment_137, __E____138, __E___139, cli, cli_command, __E_cli_140, __E_cli_141, 
		quit_cmd, quit_sym, __E_cli_command_142, version_cmd, version_sym, __E_cli_command_143, clear_cmd, clear_sym, __E_cli_command_144, help_cmd, 
		__E_cli_command_145, help_sym, __E___E_cli_command_145_146, __E___E_cli_command_145_147, help_arg, file_cmd, __E_cli_command_148, file_sym, q_string, valid_cmd, 
		__E_cli_command_149, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_150, sat_sym, unsat_cmd, __E_cli_command_151, unsat_sym, solve_cmd, 
		__E_cli_command_152, solve_sym, __E___E_cli_command_152_153, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_154, lgrs_sym, run_cmd, __E_cli_command_155, 
		run_sym, __E___E_cli_command_155_156, __E___E_cli_command_155_157, memory, normalize_cmd, __E_cli_command_158, normalize_sym, __E___E_cli_command_158_159, subst_cmd, __E_cli_command_160, 
		subst_sym, __E___E_cli_command_160_161, nf_cmd_arg, inst_cmd, __E_cli_command_162, inst_sym, __E___E_cli_command_162_163, inst_args, dnf_cmd, __E_cli_command_164, 
		dnf_sym, cnf_cmd, __E_cli_command_165, cnf_sym, anf_cmd, __E_cli_command_166, anf_sym, nnf_cmd, __E_cli_command_167, nnf_sym, 
		pnf_cmd, __E_cli_command_168, pnf_sym, mnf_cmd, __E_cli_command_169, mnf_sym, snf_cmd, __E_cli_command_170, snf_sym, onf_cmd, 
		__E_cli_command_171, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_172, def_sym, __E___E_cli_command_172_173, def_print_cmd, __E_cli_command_174, number, 
		def_rr_cmd, qelim_cmd, __E_cli_command_175, qelim_sym, get_cmd, __E_cli_command_176, get_sym, __E___E_cli_command_176_177, option_name, set_cmd, 
		__E_cli_command_178, set_sym, __E___E_cli_command_178_179, option_value, enable_cmd, __E_cli_command_180, enable_sym, disable_cmd, __E_cli_command_181, disable_sym, 
		toggle_cmd, __E_cli_command_182, toggle_sym, def_input_cmd, __E_cli_command_183, input_stream, def_output_cmd, __E_cli_command_184, output_stream, history_list_cmd, 
		__E_cli_command_185, history_sym, __E___E_cli_command_185_186, history_print_cmd, __E_cli_command_187, history_store_cmd, __E_cli_command_188, __E_solve_options_189, solve_option, __E_solve_options_190, 
		__E_solve_option_191, solver_mode, __E_solve_option_192, solver_mode_minimum, __E_solver_mode_193, solver_mode_maximum, __E_solver_mode_194, __E_input_stream_195, q_file_name, console_sym, 
		__E_output_stream_196, file_name, __E_file_name_197, __E_option_name_198, __E_option_value_199, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, execute_sym, 
		examples_sym, __E_help_arg_200, __E___E_help_arg_200_201, rel_memory, __E_memory_202, __E___E_memory_202_203, memory_id, abs_memory, __E_memory_204, named_binding, 
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
