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
	"__E___E_bf_64_65", "bf_and_nosep", "__E_bf_66", "bf_and_nosep_1st_oprnd", "__E___E_bf_66_67", "bf_and_nosep_2nd_oprnd", "__E___E_bf_66_68", "z3_parenthesis", "__E_z3_69", "z3_constant", 
	"z3_not", "z3_add", "__E_z3_70", "__E___E_z3_70_71", "z3_sub", "__E_z3_72", "__E___E_z3_72_73", "z3_mul", "__E_z3_74", "__E___E_z3_74_75", 
	"z3_div", "__E_z3_76", "__E___E_z3_76_77", "z3_mod", "__E_z3_78", "__E___E_z3_78_79", "z3_and", "__E_z3_80", "__E___E_z3_80_81", "z3_or", 
	"__E_z3_82", "__E___E_z3_82_83", "z3_xo", "__E_z3_84", "__E___E_z3_84_85", "bits", "_uint", "_int", "_real", "bit", 
	"__E_bits_86", "__E_bits_87", "integer", "__E__int_88", "num", "_long", "_ulong", "__E__real_89", "__E___E__real_89_90", "__E___E___E__real_89_90_91", 
	"ctn_neq", "__E_constraint_92", "ctnvar", "ctn_eq", "__E_constraint_93", "ctn_greater_equal", "__E_constraint_94", "ctn_greater", "__E_constraint_95", "ctn_less_equal", 
	"__E_constraint_96", "ctn_less", "__E_constraint_97", "__E_variable_98", "__E_variable_99", "__E_variable_100", "chars", "__E_variable_101", "__E_variable_102", "__E_variable_103", 
	"__E_variable_104", "io_var", "__E_variable_105", "in", "out", "__E_variable_106", "uninterpreted_constant", "in_var_name", "__E_in_107", "__E___E_in_107_108", 
	"offset", "out_var_name", "__E_out_109", "__E___E_out_109_110", "__E___E___E_out_109_110_111", "q_var", "__E_q_vars_112", "__E_q_vars_113", "__E_q_var_114", "__E_offsets_115", 
	"__E_offsets_116", "shift", "__E_offset_117", "__E_shift_118", "__E___E_shift_118_119", "digits", "__E_integer_120", "uninter_const_name", "__E_uninterpreted_constant_121", "__E___E_uninterpreted_constant_121_122", 
	"constant", "__E_bf_constant_123", "binding", "__E_bf_constant_124", "source", "__E_binding_125", "src_c", "__E___E_binding_125_126", "__E___E___E_binding_125_126_127", "__E___E___E_binding_125_126_128", 
	"__E_src_c_129", "__E_src_c_130", "__E_chars_131", "__E_chars_132", "__E_digits_133", "comment", "__E_comment_134", "__E_comment_135", "__E_comment_136", "__E____137", 
	"__E___138", "cli", "cli_command", "__E_cli_139", "__E_cli_140", "quit_cmd", "quit_sym", "__E_cli_command_141", "version_cmd", "version_sym", 
	"__E_cli_command_142", "clear_cmd", "clear_sym", "__E_cli_command_143", "help_cmd", "__E_cli_command_144", "help_sym", "__E___E_cli_command_144_145", "__E___E_cli_command_144_146", "help_arg", 
	"file_cmd", "__E_cli_command_147", "file_sym", "q_string", "valid_cmd", "__E_cli_command_148", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_149", 
	"sat_sym", "unsat_cmd", "__E_cli_command_150", "unsat_sym", "solve_cmd", "__E_cli_command_151", "solve_sym", "__E___E_cli_command_151_152", "solve_options", "wff_cmd_arg", 
	"lgrs_cmd", "__E_cli_command_153", "lgrs_sym", "run_cmd", "__E_cli_command_154", "run_sym", "__E___E_cli_command_154_155", "__E___E_cli_command_154_156", "memory", "normalize_cmd", 
	"__E_cli_command_157", "normalize_sym", "__E___E_cli_command_157_158", "subst_cmd", "__E_cli_command_159", "subst_sym", "__E___E_cli_command_159_160", "nf_cmd_arg", "inst_cmd", "__E_cli_command_161", 
	"inst_sym", "__E___E_cli_command_161_162", "inst_args", "dnf_cmd", "__E_cli_command_163", "dnf_sym", "cnf_cmd", "__E_cli_command_164", "cnf_sym", "anf_cmd", 
	"__E_cli_command_165", "anf_sym", "nnf_cmd", "__E_cli_command_166", "nnf_sym", "pnf_cmd", "__E_cli_command_167", "pnf_sym", "mnf_cmd", "__E_cli_command_168", 
	"mnf_sym", "snf_cmd", "__E_cli_command_169", "snf_sym", "onf_cmd", "__E_cli_command_170", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_171", 
	"def_sym", "__E___E_cli_command_171_172", "def_print_cmd", "__E_cli_command_173", "number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_174", "qelim_sym", "get_cmd", 
	"__E_cli_command_175", "get_sym", "__E___E_cli_command_175_176", "option_name", "set_cmd", "__E_cli_command_177", "set_sym", "__E___E_cli_command_177_178", "option_value", "enable_cmd", 
	"__E_cli_command_179", "enable_sym", "disable_cmd", "__E_cli_command_180", "disable_sym", "toggle_cmd", "__E_cli_command_181", "toggle_sym", "def_input_cmd", "__E_cli_command_182", 
	"input_stream", "def_output_cmd", "__E_cli_command_183", "output_stream", "history_list_cmd", "__E_cli_command_184", "history_sym", "__E___E_cli_command_184_185", "history_print_cmd", "__E_cli_command_186", 
	"history_store_cmd", "__E_cli_command_187", "__E_solve_options_188", "solve_option", "__E_solve_options_189", "__E_solve_option_190", "solver_mode", "__E_solve_option_191", "solver_mode_minimum", "__E_solver_mode_192", 
	"solver_mode_maximum", "__E_solver_mode_193", "__E_input_stream_194", "q_file_name", "console_sym", "__E_output_stream_195", "file_name", "__E_file_name_196", "__E_option_name_197", "__E_option_value_198", 
	"bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "execute_sym", "examples_sym", "__E_help_arg_199", "__E___E_help_arg_199_200", "rel_memory", "__E_memory_201", 
	"__E___E_memory_201_202", "memory_id", "abs_memory", "__E_memory_203", "named_binding", "z3_bv", "z3_bv_parenthesis", "__N_0", "__N_1", "__N_2", 
	"__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'f', 'a', 'l', 'b', 'c', 
	'k', 'i', 'r', 's', 't', ',', '(', ')', 'o', 'm', 
	'e', '<', '>', 'w', 'y', '[', ']', '?', 'x', '-', 
	'|', '^', '&', '!', 'T', 'F', '_', 'S', '+', '1', 
	'0', '\'', '~', '*', '/', '%', 'u', '$', '{', '}', 
	'\t', '\n', '\r', '#', 'q', 'v', 'n', 'h', 'p', 'd', 
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
			20, 22, 139, 150, 202, 212, 237, 241, 255, 257,
			264, 383, 388, 426, 444
		},
		.to_inline = {
			{ 14, 60, 14 },
			{ 21, 136, 21 },
			{ 159, 136, 21 },
			{ 161, 136, 154 },
			{ 163, 136, 21 },
			{ 165, 136, 21 },
			{ 226 },
			{ 245 },
			{ 266 },
			{ 307 },
			{ 319 },
			{ 337 },
			{ 342 },
			{ 367 },
			{ 430 },
			{ 445, 446, 445 }
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
//G243: z3(117)              => z3_constant(169).
	p(NT(117), (NT(169)));
//G244: z3(117)              => variable(139).
	p(NT(117), (NT(139)));
//G245: z3_not(170)          => z3(117).
	p(NT(170), (NT(117)));
//G246: z3(117)              => '~' _(11) z3_not(170).
	p(NT(117), (T(41)+NT(11)+NT(170)));
//G247: __E___E_z3_70_71(173) => _(11) '+' _(11).
	p(NT(173), (NT(11)+T(37)+NT(11)));
//G248: __E___E_z3_70_71(173) => __(27).
	p(NT(173), (NT(27)));
//G249: __E_z3_70(172)       => z3(117) __E___E_z3_70_71(173) z3(117).
	p(NT(172), (NT(117)+NT(173)+NT(117)));
//G250: z3_add(171)          => __E_z3_70(172).
	p(NT(171), (NT(172)));
//G251: z3(117)              => z3_add(171).
	p(NT(117), (NT(171)));
//G252: __E___E_z3_72_73(176) => _(11) '-' _(11).
	p(NT(176), (NT(11)+T(28)+NT(11)));
//G253: __E___E_z3_72_73(176) => __(27).
	p(NT(176), (NT(27)));
//G254: __E_z3_72(175)       => z3(117) __E___E_z3_72_73(176) z3(117).
	p(NT(175), (NT(117)+NT(176)+NT(117)));
//G255: z3_sub(174)          => __E_z3_72(175).
	p(NT(174), (NT(175)));
//G256: z3(117)              => z3_sub(174).
	p(NT(117), (NT(174)));
//G257: __E___E_z3_74_75(179) => _(11) '*' _(11).
	p(NT(179), (NT(11)+T(42)+NT(11)));
//G258: __E___E_z3_74_75(179) => __(27).
	p(NT(179), (NT(27)));
//G259: __E_z3_74(178)       => z3(117) __E___E_z3_74_75(179) z3(117).
	p(NT(178), (NT(117)+NT(179)+NT(117)));
//G260: z3_mul(177)          => __E_z3_74(178).
	p(NT(177), (NT(178)));
//G261: z3(117)              => z3_mul(177).
	p(NT(117), (NT(177)));
//G262: __E___E_z3_76_77(182) => _(11) '/' _(11).
	p(NT(182), (NT(11)+T(43)+NT(11)));
//G263: __E___E_z3_76_77(182) => __(27).
	p(NT(182), (NT(27)));
//G264: __E_z3_76(181)       => z3(117) __E___E_z3_76_77(182) z3(117).
	p(NT(181), (NT(117)+NT(182)+NT(117)));
//G265: z3_div(180)          => __E_z3_76(181).
	p(NT(180), (NT(181)));
//G266: z3(117)              => z3_div(180).
	p(NT(117), (NT(180)));
//G267: __E___E_z3_78_79(185) => _(11) '%' _(11).
	p(NT(185), (NT(11)+T(44)+NT(11)));
//G268: __E___E_z3_78_79(185) => __(27).
	p(NT(185), (NT(27)));
//G269: __E_z3_78(184)       => z3(117) __E___E_z3_78_79(185) z3(117).
	p(NT(184), (NT(117)+NT(185)+NT(117)));
//G270: z3_mod(183)          => __E_z3_78(184).
	p(NT(183), (NT(184)));
//G271: z3(117)              => z3_mod(183).
	p(NT(117), (NT(183)));
//G272: __E___E_z3_80_81(188) => _(11) '&' _(11).
	p(NT(188), (NT(11)+T(31)+NT(11)));
//G273: __E___E_z3_80_81(188) => __(27).
	p(NT(188), (NT(27)));
//G274: __E_z3_80(187)       => z3(117) __E___E_z3_80_81(188) z3(117).
	p(NT(187), (NT(117)+NT(188)+NT(117)));
//G275: z3_and(186)          => __E_z3_80(187).
	p(NT(186), (NT(187)));
//G276: z3(117)              => z3_and(186).
	p(NT(117), (NT(186)));
//G277: __E___E_z3_82_83(191) => _(11) '|' _(11).
	p(NT(191), (NT(11)+T(29)+NT(11)));
//G278: __E___E_z3_82_83(191) => __(27).
	p(NT(191), (NT(27)));
//G279: __E_z3_82(190)       => z3(117) __E___E_z3_82_83(191) z3(117).
	p(NT(190), (NT(117)+NT(191)+NT(117)));
//G280: z3_or(189)           => __E_z3_82(190).
	p(NT(189), (NT(190)));
//G281: z3(117)              => z3_or(189).
	p(NT(117), (NT(189)));
//G282: __E___E_z3_84_85(194) => _(11) '^' _(11).
	p(NT(194), (NT(11)+T(30)+NT(11)));
//G283: __E___E_z3_84_85(194) => __(27).
	p(NT(194), (NT(27)));
//G284: __E_z3_84(193)       => z3(117) __E___E_z3_84_85(194) z3(117).
	p(NT(193), (NT(117)+NT(194)+NT(117)));
//G285: z3_xo(192)           => __E_z3_84(193).
	p(NT(192), (NT(193)));
//G286: z3(117)              => z3_xo(192).
	p(NT(117), (NT(192)));
//G287: z3(117)              => bits(195).
	p(NT(117), (NT(195)));
//G288: z3(117)              => _uint(196).
	p(NT(117), (NT(196)));
//G289: z3(117)              => _int(197).
	p(NT(117), (NT(197)));
//G290: z3(117)              => _real(198).
	p(NT(117), (NT(198)));
//G291: __E_bits_86(200)     => bit(199).
	p(NT(200), (NT(199)));
//G292: __E_bits_87(201)     => null.
	p(NT(201), (nul));
//G293: __E_bits_87(201)     => __E_bits_86(200) __E_bits_87(201).
	p(NT(201), (NT(200)+NT(201)));
//G294: bits(195)            => bit(199) __E_bits_87(201).
	p(NT(195), (NT(199)+NT(201)));
//G295: bit(199)             => '0'.
	p(NT(199), (T(39)));
//G296: bit(199)             => '1'.
	p(NT(199), (T(38)));
//G297: __E__int_88(203)     => _(11) 'i'.
	p(NT(203), (NT(11)+T(10)));
//G298: __E__int_88(203)     => null.
	p(NT(203), (nul));
//G299: _int(197)            => integer(202) __E__int_88(203).
	p(NT(197), (NT(202)+NT(203)));
//G300: _uint(196)           => num(204) _(11) 'u' 'i'.
	p(NT(196), (NT(204)+NT(11)+T(45)+T(10)));
//G301: _long(205)           => integer(202) _(11) 'l'.
	p(NT(205), (NT(202)+NT(11)+T(6)));
//G302: _ulong(206)          => num(204) _(11) 'u' 'l'.
	p(NT(206), (NT(204)+NT(11)+T(45)+T(6)));
//G303: __E___E___E__real_89_90_91(209) => num(204).
	p(NT(209), (NT(204)));
//G304: __E___E___E__real_89_90_91(209) => null.
	p(NT(209), (nul));
//G305: __E___E__real_89_90(208) => '.' _(11) __E___E___E__real_89_90_91(209).
	p(NT(208), (T(1)+NT(11)+NT(209)));
//G306: __E___E__real_89_90(208) => null.
	p(NT(208), (nul));
//G307: __E__real_89(207)    => __E___E__real_89_90(208).
	p(NT(207), (NT(208)));
//G308: __E__real_89(207)    => _(11) 'r'.
	p(NT(207), (NT(11)+T(11)));
//G309: __E__real_89(207)    => null.
	p(NT(207), (nul));
//G310: _real(198)           => integer(202) __E__real_89(207).
	p(NT(198), (NT(202)+NT(207)));
//G311: __E_constraint_92(211) => '[' ctnvar(212) _(11) '!' '=' _(11) num(204) ']'.
	p(NT(211), (T(24)+NT(212)+NT(11)+T(32)+T(3)+NT(11)+NT(204)+T(25)));
//G312: __E_constraint_92(211) => '[' num(204) _(11) '!' '=' _(11) ctnvar(212) ']'.
	p(NT(211), (T(24)+NT(204)+NT(11)+T(32)+T(3)+NT(11)+NT(212)+T(25)));
//G313: ctn_neq(210)         => __E_constraint_92(211).
	p(NT(210), (NT(211)));
//G314: constraint(76)       => ctn_neq(210).
	p(NT(76), (NT(210)));
//G315: __E_constraint_93(214) => '[' ctnvar(212) _(11) '=' _(11) num(204) ']'.
	p(NT(214), (T(24)+NT(212)+NT(11)+T(3)+NT(11)+NT(204)+T(25)));
//G316: __E_constraint_93(214) => '[' num(204) _(11) '=' _(11) ctnvar(212) ']'.
	p(NT(214), (T(24)+NT(204)+NT(11)+T(3)+NT(11)+NT(212)+T(25)));
//G317: ctn_eq(213)          => __E_constraint_93(214).
	p(NT(213), (NT(214)));
//G318: constraint(76)       => ctn_eq(213).
	p(NT(76), (NT(213)));
//G319: __E_constraint_94(216) => '[' ctnvar(212) _(11) '>' '=' _(11) num(204) ']'.
	p(NT(216), (T(24)+NT(212)+NT(11)+T(21)+T(3)+NT(11)+NT(204)+T(25)));
//G320: __E_constraint_94(216) => '[' num(204) _(11) '>' '=' _(11) ctnvar(212) ']'.
	p(NT(216), (T(24)+NT(204)+NT(11)+T(21)+T(3)+NT(11)+NT(212)+T(25)));
//G321: ctn_greater_equal(215) => __E_constraint_94(216).
	p(NT(215), (NT(216)));
//G322: constraint(76)       => ctn_greater_equal(215).
	p(NT(76), (NT(215)));
//G323: __E_constraint_95(218) => '[' ctnvar(212) _(11) '>' _(11) num(204) ']'.
	p(NT(218), (T(24)+NT(212)+NT(11)+T(21)+NT(11)+NT(204)+T(25)));
//G324: __E_constraint_95(218) => '[' num(204) _(11) '>' _(11) ctnvar(212) ']'.
	p(NT(218), (T(24)+NT(204)+NT(11)+T(21)+NT(11)+NT(212)+T(25)));
//G325: ctn_greater(217)     => __E_constraint_95(218).
	p(NT(217), (NT(218)));
//G326: constraint(76)       => ctn_greater(217).
	p(NT(76), (NT(217)));
//G327: __E_constraint_96(220) => '[' ctnvar(212) _(11) '<' '=' _(11) num(204) ']'.
	p(NT(220), (T(24)+NT(212)+NT(11)+T(20)+T(3)+NT(11)+NT(204)+T(25)));
//G328: __E_constraint_96(220) => '[' num(204) _(11) '<' '=' _(11) ctnvar(212) ']'.
	p(NT(220), (T(24)+NT(204)+NT(11)+T(20)+T(3)+NT(11)+NT(212)+T(25)));
//G329: ctn_less_equal(219)  => __E_constraint_96(220).
	p(NT(219), (NT(220)));
//G330: constraint(76)       => ctn_less_equal(219).
	p(NT(76), (NT(219)));
//G331: __E_constraint_97(222) => '[' ctnvar(212) _(11) '<' _(11) num(204) ']'.
	p(NT(222), (T(24)+NT(212)+NT(11)+T(20)+NT(11)+NT(204)+T(25)));
//G332: __E_constraint_97(222) => '[' num(204) _(11) '<' _(11) ctnvar(212) ']'.
	p(NT(222), (T(24)+NT(204)+NT(11)+T(20)+NT(11)+NT(212)+T(25)));
//G333: ctn_less(221)        => __E_constraint_97(222).
	p(NT(221), (NT(222)));
//G334: constraint(76)       => ctn_less(221).
	p(NT(76), (NT(221)));
//G335: __N_0(447)           => 'F'.
	p(NT(447), (T(34)));
//G336: __N_1(448)           => 'T'.
	p(NT(448), (T(33)));
//G337: __E_variable_98(223) => ~( __N_0(447) ) & ~( __N_1(448) ) & alpha(5).	 # conjunctive
	p(NT(223), ~(NT(447)) & ~(NT(448)) & (NT(5)));
//G338: __E_variable_99(224) => null.
	p(NT(224), (nul));
//G339: __E_variable_99(224) => digit(3) __E_variable_99(224).
	p(NT(224), (NT(3)+NT(224)));
//G340: __E_variable_100(225) => _(11) ':' _(11) chars(226).
	p(NT(225), (NT(11)+T(2)+NT(11)+NT(226)));
//G341: __E_variable_100(225) => null.
	p(NT(225), (nul));
//G342: variable(139)        => __E_variable_98(223) __E_variable_99(224) __E_variable_100(225).	 # guarded: charvar
	p(NT(139), (NT(223)+NT(224)+NT(225)));
	p.back().guard = "charvar";
//G343: __N_2(449)           => 'F'.
	p(NT(449), (T(34)));
//G344: __N_3(450)           => 'T'.
	p(NT(450), (T(33)));
//G345: __E_variable_101(227) => ~( __N_2(449) ) & ~( __N_3(450) ) & alpha(5).	 # conjunctive
	p(NT(227), ~(NT(449)) & ~(NT(450)) & (NT(5)));
//G346: __E_variable_102(228) => alnum(6).
	p(NT(228), (NT(6)));
//G347: __E_variable_102(228) => '_'.
	p(NT(228), (T(35)));
//G348: __E_variable_103(229) => null.
	p(NT(229), (nul));
//G349: __E_variable_103(229) => __E_variable_102(228) __E_variable_103(229).
	p(NT(229), (NT(228)+NT(229)));
//G350: __E_variable_104(230) => _(11) ':' _(11) chars(226).
	p(NT(230), (NT(11)+T(2)+NT(11)+NT(226)));
//G351: __E_variable_104(230) => null.
	p(NT(230), (nul));
//G352: variable(139)        => __E_variable_101(227) __E_variable_103(229) __E_variable_104(230).	 # guarded: var
	p(NT(139), (NT(227)+NT(229)+NT(230)));
	p.back().guard = "var";
//G353: __E_variable_105(232) => in(233).
	p(NT(232), (NT(233)));
//G354: __E_variable_105(232) => out(234).
	p(NT(232), (NT(234)));
//G355: io_var(231)          => __E_variable_105(232).
	p(NT(231), (NT(232)));
//G356: __E_variable_106(235) => _(11) ':' _(11) chars(226).
	p(NT(235), (NT(11)+T(2)+NT(11)+NT(226)));
//G357: __E_variable_106(235) => null.
	p(NT(235), (nul));
//G358: variable(139)        => io_var(231) __E_variable_106(235).
	p(NT(139), (NT(231)+NT(235)));
//G359: variable(139)        => uninterpreted_constant(236).
	p(NT(139), (NT(236)));
//G360: __E___E_in_107_108(239) => digit(3).
	p(NT(239), (NT(3)));
//G361: __E___E_in_107_108(239) => digit(3) __E___E_in_107_108(239).
	p(NT(239), (NT(3)+NT(239)));
//G362: __E_in_107(238)      => 'i' __E___E_in_107_108(239).
	p(NT(238), (T(10)+NT(239)));
//G363: in_var_name(237)     => __E_in_107(238).
	p(NT(237), (NT(238)));
//G364: in(233)              => in_var_name(237) '[' offset(240) ']'.
	p(NT(233), (NT(237)+T(24)+NT(240)+T(25)));
//G365: __E___E___E_out_109_110_111(244) => digit(3).
	p(NT(244), (NT(3)));
//G366: __E___E___E_out_109_110_111(244) => digit(3) __E___E___E_out_109_110_111(244).
	p(NT(244), (NT(3)+NT(244)));
//G367: __E___E_out_109_110(243) => 'o' __E___E___E_out_109_110_111(244).
	p(NT(243), (T(17)+NT(244)));
//G368: __E_out_109(242)     => __E___E_out_109_110(243).
	p(NT(242), (NT(243)));
//G369: __E_out_109(242)     => 'u'.
	p(NT(242), (T(45)));
//G370: out_var_name(241)    => __E_out_109(242).
	p(NT(241), (NT(242)));
//G371: out(234)             => out_var_name(241) '[' offset(240) ']'.
	p(NT(234), (NT(241)+T(24)+NT(240)+T(25)));
//G372: ctnvar(212)          => chars(226).
	p(NT(212), (NT(226)));
//G373: capture(20)          => '$' chars(226).
	p(NT(20), (T(46)+NT(226)));
//G374: __E_q_vars_112(246)  => _(11) ',' _(11) q_var(245).
	p(NT(246), (NT(11)+T(14)+NT(11)+NT(245)));
//G375: __E_q_vars_113(247)  => null.
	p(NT(247), (nul));
//G376: __E_q_vars_113(247)  => __E_q_vars_112(246) __E_q_vars_113(247).
	p(NT(247), (NT(246)+NT(247)));
//G377: q_vars(72)           => q_var(245) __E_q_vars_113(247).
	p(NT(72), (NT(245)+NT(247)));
//G378: q_var(245)           => capture(20).
	p(NT(245), (NT(20)));
//G379: __E_q_var_114(248)   => _(11) ':' _(11) chars(226).
	p(NT(248), (NT(11)+T(2)+NT(11)+NT(226)));
//G380: __E_q_var_114(248)   => null.
	p(NT(248), (nul));
//G381: __N_4(451)           => uninterpreted_constant(236) __E_q_var_114(248).
	p(NT(451), (NT(236)+NT(248)));
//G382: q_var(245)           => variable(139) & ~( __N_4(451) ).	 # conjunctive
	p(NT(245), (NT(139)) & ~(NT(451)));
//G383: __E_offsets_115(249) => _(11) ',' _(11) offset(240).
	p(NT(249), (NT(11)+T(14)+NT(11)+NT(240)));
//G384: __E_offsets_116(250) => null.
	p(NT(250), (nul));
//G385: __E_offsets_116(250) => __E_offsets_115(249) __E_offsets_116(250).
	p(NT(250), (NT(249)+NT(250)));
//G386: offsets(24)          => '[' _(11) offset(240) __E_offsets_116(250) _(11) ']'.
	p(NT(24), (T(24)+NT(11)+NT(240)+NT(250)+NT(11)+T(25)));
//G387: offset(240)          => integer(202).
	p(NT(240), (NT(202)));
//G388: offset(240)          => capture(20).
	p(NT(240), (NT(20)));
//G389: offset(240)          => shift(251).
	p(NT(240), (NT(251)));
//G390: __N_5(452)           => io_var(231).
	p(NT(452), (NT(231)));
//G391: __E_offset_117(252)  => variable(139) & ~( __N_5(452) ).	 # conjunctive
	p(NT(252), (NT(139)) & ~(NT(452)));
//G392: offset(240)          => __E_offset_117(252).
	p(NT(240), (NT(252)));
//G393: __E_shift_118(253)   => capture(20).
	p(NT(253), (NT(20)));
//G394: __N_6(453)           => io_var(231).
	p(NT(453), (NT(231)));
//G395: __E___E_shift_118_119(254) => variable(139) & ~( __N_6(453) ).	 # conjunctive
	p(NT(254), (NT(139)) & ~(NT(453)));
//G396: __E_shift_118(253)   => __E___E_shift_118_119(254).
	p(NT(253), (NT(254)));
//G397: shift(251)           => __E_shift_118(253) _(11) '-' _(11) num(204).
	p(NT(251), (NT(253)+NT(11)+T(28)+NT(11)+NT(204)));
//G398: num(204)             => digits(255).
	p(NT(204), (NT(255)));
//G399: __E_integer_120(256) => '-'.
	p(NT(256), (T(28)));
//G400: __E_integer_120(256) => null.
	p(NT(256), (nul));
//G401: integer(202)         => __E_integer_120(256) _(11) digits(255).
	p(NT(202), (NT(256)+NT(11)+NT(255)));
//G402: __E___E_uninterpreted_constant_121_122(259) => chars(226).
	p(NT(259), (NT(226)));
//G403: __E___E_uninterpreted_constant_121_122(259) => _(11).
	p(NT(259), (NT(11)));
//G404: __E_uninterpreted_constant_121(258) => __E___E_uninterpreted_constant_121_122(259) ':' chars(226).
	p(NT(258), (NT(259)+T(2)+NT(226)));
//G405: uninter_const_name(257) => __E_uninterpreted_constant_121(258).
	p(NT(257), (NT(258)));
//G406: uninterpreted_constant(236) => '<' uninter_const_name(257) _(11) '>'.
	p(NT(236), (T(20)+NT(257)+NT(11)+T(21)));
//G407: __E_bf_constant_123(261) => capture(20).
	p(NT(261), (NT(20)));
//G408: __E_bf_constant_123(261) => binding(262).
	p(NT(261), (NT(262)));
//G409: constant(260)        => __E_bf_constant_123(261).
	p(NT(260), (NT(261)));
//G410: __E_bf_constant_124(263) => _(11) ':' _(11) type(150).
	p(NT(263), (NT(11)+T(2)+NT(11)+NT(150)));
//G411: __E_bf_constant_124(263) => null.
	p(NT(263), (nul));
//G412: bf_constant(138)     => '{' _(11) constant(260) _(11) '}' __E_bf_constant_124(263).
	p(NT(138), (T(47)+NT(11)+NT(260)+NT(11)+T(48)+NT(263)));
//G413: __E___E___E_binding_125_126_127(268) => src_c(266).
	p(NT(268), (NT(266)));
//G414: __E___E___E_binding_125_126_127(268) => space(2).
	p(NT(268), (NT(2)));
//G415: __E___E___E_binding_125_126_128(269) => null.
	p(NT(269), (nul));
//G416: __E___E___E_binding_125_126_128(269) => __E___E___E_binding_125_126_127(268) __E___E___E_binding_125_126_128(269).
	p(NT(269), (NT(268)+NT(269)));
//G417: __E___E_binding_125_126(267) => __E___E___E_binding_125_126_128(269) src_c(266).
	p(NT(267), (NT(269)+NT(266)));
//G418: __E___E_binding_125_126(267) => null.
	p(NT(267), (nul));
//G419: __E_binding_125(265) => src_c(266) __E___E_binding_125_126(267).
	p(NT(265), (NT(266)+NT(267)));
//G420: source(264)          => __E_binding_125(265).
	p(NT(264), (NT(265)));
//G421: binding(262)         => source(264).
	p(NT(262), (NT(264)));
//G422: src_c(266)           => alnum(6).
	p(NT(266), (NT(6)));
//G423: __N_7(454)           => '{'.
	p(NT(454), (T(47)));
//G424: __N_8(455)           => '}'.
	p(NT(455), (T(48)));
//G425: src_c(266)           => ~( __N_7(454) ) & ~( __N_8(455) ) & punct(7).	 # conjunctive
	p(NT(266), ~(NT(454)) & ~(NT(455)) & (NT(7)));
//G426: __E_src_c_129(270)   => src_c(266).
	p(NT(270), (NT(266)));
//G427: __E_src_c_129(270)   => space(2).
	p(NT(270), (NT(2)));
//G428: __E_src_c_130(271)   => null.
	p(NT(271), (nul));
//G429: __E_src_c_130(271)   => __E_src_c_129(270) __E_src_c_130(271).
	p(NT(271), (NT(270)+NT(271)));
//G430: src_c(266)           => '{' __E_src_c_130(271) '}'.
	p(NT(266), (T(47)+NT(271)+T(48)));
//G431: type(150)            => chars(226).
	p(NT(150), (NT(226)));
//G432: type(150)            => chars(226) _(11) '[' _(11) num(204) _(11) ']'.
	p(NT(150), (NT(226)+NT(11)+T(24)+NT(11)+NT(204)+NT(11)+T(25)));
//G433: __E_chars_131(272)   => alnum(6).
	p(NT(272), (NT(6)));
//G434: __E_chars_131(272)   => '_'.
	p(NT(272), (T(35)));
//G435: __E_chars_132(273)   => null.
	p(NT(273), (nul));
//G436: __E_chars_132(273)   => __E_chars_131(272) __E_chars_132(273).
	p(NT(273), (NT(272)+NT(273)));
//G437: chars(226)           => alpha(5) __E_chars_132(273).
	p(NT(226), (NT(5)+NT(273)));
//G438: __E_digits_133(274)  => digit(3).
	p(NT(274), (NT(3)));
//G439: __E_digits_133(274)  => digit(3) __E_digits_133(274).
	p(NT(274), (NT(3)+NT(274)));
//G440: digits(255)          => __E_digits_133(274).
	p(NT(255), (NT(274)));
//G441: sym(22)              => chars(226).
	p(NT(22), (NT(226)));
//G442: __E_comment_134(276) => printable(8).
	p(NT(276), (NT(8)));
//G443: __E_comment_134(276) => '\t'.
	p(NT(276), (T(49)));
//G444: __E_comment_135(277) => null.
	p(NT(277), (nul));
//G445: __E_comment_135(277) => __E_comment_134(276) __E_comment_135(277).
	p(NT(277), (NT(276)+NT(277)));
//G446: __E_comment_136(278) => '\n'.
	p(NT(278), (T(50)));
//G447: __E_comment_136(278) => '\r'.
	p(NT(278), (T(51)));
//G448: __E_comment_136(278) => eof(1).
	p(NT(278), (NT(1)));
//G449: comment(275)         => '#' __E_comment_135(277) __E_comment_136(278).
	p(NT(275), (T(52)+NT(277)+NT(278)));
//G450: __E____137(279)      => space(2).
	p(NT(279), (NT(2)));
//G451: __E____137(279)      => comment(275).
	p(NT(279), (NT(275)));
//G452: __(27)               => __E____137(279) _(11).
	p(NT(27), (NT(279)+NT(11)));
//G453: __E___138(280)       => __(27).
	p(NT(280), (NT(27)));
//G454: __E___138(280)       => null.
	p(NT(280), (nul));
//G455: _(11)                => __E___138(280).
	p(NT(11), (NT(280)));
//G456: cli(281)             => _(11).
	p(NT(281), (NT(11)));
//G457: __E_cli_139(283)     => '.' _(11) cli_command(282) _(11).
	p(NT(283), (T(1)+NT(11)+NT(282)+NT(11)));
//G458: __E_cli_140(284)     => null.
	p(NT(284), (nul));
//G459: __E_cli_140(284)     => __E_cli_139(283) __E_cli_140(284).
	p(NT(284), (NT(283)+NT(284)));
//G460: cli(281)             => _(11) cli_command(282) _(11) __E_cli_140(284).
	p(NT(281), (NT(11)+NT(282)+NT(11)+NT(284)));
//G461: __E_cli_command_141(287) => 'q'.
	p(NT(287), (T(53)));
//G462: __E_cli_command_141(287) => 'q' 'u' 'i' 't'.
	p(NT(287), (T(53)+T(45)+T(10)+T(13)));
//G463: quit_sym(286)        => __E_cli_command_141(287).
	p(NT(286), (NT(287)));
//G464: quit_cmd(285)        => quit_sym(286).
	p(NT(285), (NT(286)));
//G465: cli_command(282)     => quit_cmd(285).
	p(NT(282), (NT(285)));
//G466: __E_cli_command_142(290) => 'v'.
	p(NT(290), (T(54)));
//G467: __E_cli_command_142(290) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(290), (T(54)+T(19)+T(11)+T(12)+T(10)+T(17)+T(55)));
//G468: version_sym(289)     => __E_cli_command_142(290).
	p(NT(289), (NT(290)));
//G469: version_cmd(288)     => version_sym(289).
	p(NT(288), (NT(289)));
//G470: cli_command(282)     => version_cmd(288).
	p(NT(282), (NT(288)));
//G471: __E_cli_command_143(293) => 'c'.
	p(NT(293), (T(8)));
//G472: __E_cli_command_143(293) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(293), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G473: clear_sym(292)       => __E_cli_command_143(293).
	p(NT(292), (NT(293)));
//G474: clear_cmd(291)       => clear_sym(292).
	p(NT(291), (NT(292)));
//G475: cli_command(282)     => clear_cmd(291).
	p(NT(282), (NT(291)));
//G476: __E___E_cli_command_144_145(297) => 'h'.
	p(NT(297), (T(56)));
//G477: __E___E_cli_command_144_145(297) => 'h' 'e' 'l' 'p'.
	p(NT(297), (T(56)+T(19)+T(6)+T(57)));
//G478: help_sym(296)        => __E___E_cli_command_144_145(297).
	p(NT(296), (NT(297)));
//G479: __E___E_cli_command_144_146(298) => __(27) help_arg(299).
	p(NT(298), (NT(27)+NT(299)));
//G480: __E___E_cli_command_144_146(298) => null.
	p(NT(298), (nul));
//G481: __E_cli_command_144(295) => help_sym(296) __E___E_cli_command_144_146(298).
	p(NT(295), (NT(296)+NT(298)));
//G482: help_cmd(294)        => __E_cli_command_144(295).
	p(NT(294), (NT(295)));
//G483: cli_command(282)     => help_cmd(294).
	p(NT(282), (NT(294)));
//G484: file_sym(302)        => 'f' 'i' 'l' 'e'.
	p(NT(302), (T(4)+T(10)+T(6)+T(19)));
//G485: __E_cli_command_147(301) => file_sym(302) __(27) q_string(303).
	p(NT(301), (NT(302)+NT(27)+NT(303)));
//G486: file_cmd(300)        => __E_cli_command_147(301).
	p(NT(300), (NT(301)));
//G487: cli_command(282)     => file_cmd(300).
	p(NT(282), (NT(300)));
//G488: valid_sym(306)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(306), (T(54)+T(5)+T(6)+T(10)+T(58)));
//G489: __E_cli_command_148(305) => valid_sym(306) __(27) normalize_cmd_arg(307).
	p(NT(305), (NT(306)+NT(27)+NT(307)));
//G490: valid_cmd(304)       => __E_cli_command_148(305).
	p(NT(304), (NT(305)));
//G491: cli_command(282)     => valid_cmd(304).
	p(NT(282), (NT(304)));
//G492: sat_sym(310)         => 's' 'a' 't'.
	p(NT(310), (T(12)+T(5)+T(13)));
//G493: __E_cli_command_149(309) => sat_sym(310) __(27) normalize_cmd_arg(307).
	p(NT(309), (NT(310)+NT(27)+NT(307)));
//G494: sat_cmd(308)         => __E_cli_command_149(309).
	p(NT(308), (NT(309)));
//G495: cli_command(282)     => sat_cmd(308).
	p(NT(282), (NT(308)));
//G496: unsat_sym(313)       => 'u' 'n' 's' 'a' 't'.
	p(NT(313), (T(45)+T(55)+T(12)+T(5)+T(13)));
//G497: __E_cli_command_150(312) => unsat_sym(313) __(27) normalize_cmd_arg(307).
	p(NT(312), (NT(313)+NT(27)+NT(307)));
//G498: unsat_cmd(311)       => __E_cli_command_150(312).
	p(NT(311), (NT(312)));
//G499: cli_command(282)     => unsat_cmd(311).
	p(NT(282), (NT(311)));
//G500: solve_sym(316)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(316), (T(12)+T(17)+T(6)+T(54)+T(19)));
//G501: __E___E_cli_command_151_152(317) => solve_options(318).
	p(NT(317), (NT(318)));
//G502: __E___E_cli_command_151_152(317) => null.
	p(NT(317), (nul));
//G503: __E_cli_command_151(315) => solve_sym(316) __E___E_cli_command_151_152(317) __(27) wff_cmd_arg(319).
	p(NT(315), (NT(316)+NT(317)+NT(27)+NT(319)));
//G504: solve_cmd(314)       => __E_cli_command_151(315).
	p(NT(314), (NT(315)));
//G505: cli_command(282)     => solve_cmd(314).
	p(NT(282), (NT(314)));
//G506: lgrs_sym(322)        => 'l' 'g' 'r' 's'.
	p(NT(322), (T(6)+T(59)+T(11)+T(12)));
//G507: __E_cli_command_153(321) => lgrs_sym(322) __(27) wff_cmd_arg(319).
	p(NT(321), (NT(322)+NT(27)+NT(319)));
//G508: lgrs_cmd(320)        => __E_cli_command_153(321).
	p(NT(320), (NT(321)));
//G509: cli_command(282)     => lgrs_cmd(320).
	p(NT(282), (NT(320)));
//G510: __E___E_cli_command_154_155(326) => 'r'.
	p(NT(326), (T(11)));
//G511: __E___E_cli_command_154_155(326) => 'r' 'u' 'n'.
	p(NT(326), (T(11)+T(45)+T(55)));
//G512: run_sym(325)         => __E___E_cli_command_154_155(326).
	p(NT(325), (NT(326)));
//G513: __E___E_cli_command_154_156(327) => memory(328).
	p(NT(327), (NT(328)));
//G514: __E___E_cli_command_154_156(327) => wff(14).
	p(NT(327), (NT(14)));
//G515: __E_cli_command_154(324) => run_sym(325) __(27) __E___E_cli_command_154_156(327).
	p(NT(324), (NT(325)+NT(27)+NT(327)));
//G516: run_cmd(323)         => __E_cli_command_154(324).
	p(NT(323), (NT(324)));
//G517: cli_command(282)     => run_cmd(323).
	p(NT(282), (NT(323)));
//G518: __E___E_cli_command_157_158(332) => 'n'.
	p(NT(332), (T(55)));
//G519: __E___E_cli_command_157_158(332) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(332), (T(55)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(60)+T(19)));
//G520: normalize_sym(331)   => __E___E_cli_command_157_158(332).
	p(NT(331), (NT(332)));
//G521: __E_cli_command_157(330) => normalize_sym(331) __(27) normalize_cmd_arg(307).
	p(NT(330), (NT(331)+NT(27)+NT(307)));
//G522: normalize_cmd(329)   => __E_cli_command_157(330).
	p(NT(329), (NT(330)));
//G523: cli_command(282)     => normalize_cmd(329).
	p(NT(282), (NT(329)));
//G524: __E___E_cli_command_159_160(336) => 's'.
	p(NT(336), (T(12)));
//G525: __E___E_cli_command_159_160(336) => 's' 'u' 'b' 's' 't'.
	p(NT(336), (T(12)+T(45)+T(7)+T(12)+T(13)));
//G526: __E___E_cli_command_159_160(336) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(336), (T(12)+T(45)+T(7)+T(12)+T(13)+T(10)+T(13)+T(45)+T(13)+T(19)));
//G527: subst_sym(335)       => __E___E_cli_command_159_160(336).
	p(NT(335), (NT(336)));
//G528: __E_cli_command_159(334) => subst_sym(335) __(27) nf_cmd_arg(337) _(11) '[' _(11) nf_cmd_arg(337) _(11) '/' _(11) nf_cmd_arg(337) _(11) ']'.
	p(NT(334), (NT(335)+NT(27)+NT(337)+NT(11)+T(24)+NT(11)+NT(337)+NT(11)+T(43)+NT(11)+NT(337)+NT(11)+T(25)));
//G529: subst_cmd(333)       => __E_cli_command_159(334).
	p(NT(333), (NT(334)));
//G530: cli_command(282)     => subst_cmd(333).
	p(NT(282), (NT(333)));
//G531: __E___E_cli_command_161_162(341) => 'i'.
	p(NT(341), (T(10)));
//G532: __E___E_cli_command_161_162(341) => 'i' 'n' 's' 't'.
	p(NT(341), (T(10)+T(55)+T(12)+T(13)));
//G533: __E___E_cli_command_161_162(341) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(341), (T(10)+T(55)+T(12)+T(13)+T(5)+T(55)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G534: inst_sym(340)        => __E___E_cli_command_161_162(341).
	p(NT(340), (NT(341)));
//G535: __E_cli_command_161(339) => inst_sym(340) __(27) inst_args(342).
	p(NT(339), (NT(340)+NT(27)+NT(342)));
//G536: inst_cmd(338)        => __E_cli_command_161(339).
	p(NT(338), (NT(339)));
//G537: cli_command(282)     => inst_cmd(338).
	p(NT(282), (NT(338)));
//G538: dnf_sym(345)         => 'd' 'n' 'f'.
	p(NT(345), (T(58)+T(55)+T(4)));
//G539: __E_cli_command_163(344) => dnf_sym(345) __(27) nf_cmd_arg(337).
	p(NT(344), (NT(345)+NT(27)+NT(337)));
//G540: dnf_cmd(343)         => __E_cli_command_163(344).
	p(NT(343), (NT(344)));
//G541: cli_command(282)     => dnf_cmd(343).
	p(NT(282), (NT(343)));
//G542: cnf_sym(348)         => 'c' 'n' 'f'.
	p(NT(348), (T(8)+T(55)+T(4)));
//G543: __E_cli_command_164(347) => cnf_sym(348) __(27) nf_cmd_arg(337).
	p(NT(347), (NT(348)+NT(27)+NT(337)));
//G544: cnf_cmd(346)         => __E_cli_command_164(347).
	p(NT(346), (NT(347)));
//G545: cli_command(282)     => cnf_cmd(346).
	p(NT(282), (NT(346)));
//G546: anf_sym(351)         => 'a' 'n' 'f'.
	p(NT(351), (T(5)+T(55)+T(4)));
//G547: __E_cli_command_165(350) => anf_sym(351) __(27) nf_cmd_arg(337).
	p(NT(350), (NT(351)+NT(27)+NT(337)));
//G548: anf_cmd(349)         => __E_cli_command_165(350).
	p(NT(349), (NT(350)));
//G549: cli_command(282)     => anf_cmd(349).
	p(NT(282), (NT(349)));
//G550: nnf_sym(354)         => 'n' 'n' 'f'.
	p(NT(354), (T(55)+T(55)+T(4)));
//G551: __E_cli_command_166(353) => nnf_sym(354) __(27) nf_cmd_arg(337).
	p(NT(353), (NT(354)+NT(27)+NT(337)));
//G552: nnf_cmd(352)         => __E_cli_command_166(353).
	p(NT(352), (NT(353)));
//G553: cli_command(282)     => nnf_cmd(352).
	p(NT(282), (NT(352)));
//G554: pnf_sym(357)         => 'p' 'n' 'f'.
	p(NT(357), (T(57)+T(55)+T(4)));
//G555: __E_cli_command_167(356) => pnf_sym(357) __(27) nf_cmd_arg(337).
	p(NT(356), (NT(357)+NT(27)+NT(337)));
//G556: pnf_cmd(355)         => __E_cli_command_167(356).
	p(NT(355), (NT(356)));
//G557: cli_command(282)     => pnf_cmd(355).
	p(NT(282), (NT(355)));
//G558: mnf_sym(360)         => 'm' 'n' 'f'.
	p(NT(360), (T(18)+T(55)+T(4)));
//G559: __E_cli_command_168(359) => mnf_sym(360) __(27) nf_cmd_arg(337).
	p(NT(359), (NT(360)+NT(27)+NT(337)));
//G560: mnf_cmd(358)         => __E_cli_command_168(359).
	p(NT(358), (NT(359)));
//G561: cli_command(282)     => mnf_cmd(358).
	p(NT(282), (NT(358)));
//G562: snf_sym(363)         => 's' 'n' 'f'.
	p(NT(363), (T(12)+T(55)+T(4)));
//G563: __E_cli_command_169(362) => snf_sym(363) __(27) nf_cmd_arg(337).
	p(NT(362), (NT(363)+NT(27)+NT(337)));
//G564: snf_cmd(361)         => __E_cli_command_169(362).
	p(NT(361), (NT(362)));
//G565: cli_command(282)     => snf_cmd(361).
	p(NT(282), (NT(361)));
//G566: onf_sym(366)         => 'o' 'n' 'f'.
	p(NT(366), (T(17)+T(55)+T(4)));
//G567: __E_cli_command_170(365) => onf_sym(366) __(27) variable(139) __(27) onf_cmd_arg(367).
	p(NT(365), (NT(366)+NT(27)+NT(139)+NT(27)+NT(367)));
//G568: onf_cmd(364)         => __E_cli_command_170(365).
	p(NT(364), (NT(365)));
//G569: cli_command(282)     => onf_cmd(364).
	p(NT(282), (NT(364)));
//G570: __E___E_cli_command_171_172(371) => 'd' 'e' 'f' 's'.
	p(NT(371), (T(58)+T(19)+T(4)+T(12)));
//G571: __E___E_cli_command_171_172(371) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(371), (T(58)+T(19)+T(4)+T(10)+T(55)+T(10)+T(13)+T(10)+T(17)+T(55)+T(12)));
//G572: def_sym(370)         => __E___E_cli_command_171_172(371).
	p(NT(370), (NT(371)));
//G573: __E_cli_command_171(369) => def_sym(370).
	p(NT(369), (NT(370)));
//G574: def_list_cmd(368)    => __E_cli_command_171(369).
	p(NT(368), (NT(369)));
//G575: cli_command(282)     => def_list_cmd(368).
	p(NT(282), (NT(368)));
//G576: __E_cli_command_173(373) => def_sym(370) __(27) number(374).
	p(NT(373), (NT(370)+NT(27)+NT(374)));
//G577: def_print_cmd(372)   => __E_cli_command_173(373).
	p(NT(372), (NT(373)));
//G578: cli_command(282)     => def_print_cmd(372).
	p(NT(282), (NT(372)));
//G579: def_rr_cmd(375)      => rec_relation(16).
	p(NT(375), (NT(16)));
//G580: cli_command(282)     => def_rr_cmd(375).
	p(NT(282), (NT(375)));
//G581: qelim_sym(378)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(378), (T(53)+T(19)+T(6)+T(10)+T(18)));
//G582: __E_cli_command_174(377) => qelim_sym(378) __(27) wff_cmd_arg(319).
	p(NT(377), (NT(378)+NT(27)+NT(319)));
//G583: qelim_cmd(376)       => __E_cli_command_174(377).
	p(NT(376), (NT(377)));
//G584: cli_command(282)     => qelim_cmd(376).
	p(NT(282), (NT(376)));
//G585: get_sym(381)         => 'g' 'e' 't'.
	p(NT(381), (T(59)+T(19)+T(13)));
//G586: __E___E_cli_command_175_176(382) => __(27) option_name(383).
	p(NT(382), (NT(27)+NT(383)));
//G587: __E___E_cli_command_175_176(382) => null.
	p(NT(382), (nul));
//G588: __E_cli_command_175(380) => get_sym(381) __E___E_cli_command_175_176(382).
	p(NT(380), (NT(381)+NT(382)));
//G589: get_cmd(379)         => __E_cli_command_175(380).
	p(NT(379), (NT(380)));
//G590: cli_command(282)     => get_cmd(379).
	p(NT(282), (NT(379)));
//G591: set_sym(386)         => 's' 'e' 't'.
	p(NT(386), (T(12)+T(19)+T(13)));
//G592: __E___E_cli_command_177_178(387) => __(27).
	p(NT(387), (NT(27)));
//G593: __E___E_cli_command_177_178(387) => _(11) '=' _(11).
	p(NT(387), (NT(11)+T(3)+NT(11)));
//G594: __E_cli_command_177(385) => set_sym(386) __(27) option_name(383) __E___E_cli_command_177_178(387) option_value(388).
	p(NT(385), (NT(386)+NT(27)+NT(383)+NT(387)+NT(388)));
//G595: set_cmd(384)         => __E_cli_command_177(385).
	p(NT(384), (NT(385)));
//G596: cli_command(282)     => set_cmd(384).
	p(NT(282), (NT(384)));
//G597: enable_sym(391)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(391), (T(19)+T(55)+T(5)+T(7)+T(6)+T(19)));
//G598: __E_cli_command_179(390) => enable_sym(391) __(27) option_name(383).
	p(NT(390), (NT(391)+NT(27)+NT(383)));
//G599: enable_cmd(389)      => __E_cli_command_179(390).
	p(NT(389), (NT(390)));
//G600: cli_command(282)     => enable_cmd(389).
	p(NT(282), (NT(389)));
//G601: disable_sym(394)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(394), (T(58)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G602: __E_cli_command_180(393) => disable_sym(394) __(27) option_name(383).
	p(NT(393), (NT(394)+NT(27)+NT(383)));
//G603: disable_cmd(392)     => __E_cli_command_180(393).
	p(NT(392), (NT(393)));
//G604: cli_command(282)     => disable_cmd(392).
	p(NT(282), (NT(392)));
//G605: toggle_sym(397)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(397), (T(13)+T(17)+T(59)+T(59)+T(6)+T(19)));
//G606: __E_cli_command_181(396) => toggle_sym(397) __(27) option_name(383).
	p(NT(396), (NT(397)+NT(27)+NT(383)));
//G607: toggle_cmd(395)      => __E_cli_command_181(396).
	p(NT(395), (NT(396)));
//G608: cli_command(282)     => toggle_cmd(395).
	p(NT(282), (NT(395)));
//G609: __E_cli_command_182(399) => type(150) __(27) in_var_name(237) _(11) '=' _(11) input_stream(400).
	p(NT(399), (NT(150)+NT(27)+NT(237)+NT(11)+T(3)+NT(11)+NT(400)));
//G610: def_input_cmd(398)   => __E_cli_command_182(399).
	p(NT(398), (NT(399)));
//G611: cli_command(282)     => def_input_cmd(398).
	p(NT(282), (NT(398)));
//G612: __E_cli_command_183(402) => type(150) __(27) out_var_name(241) _(11) '=' _(11) output_stream(403).
	p(NT(402), (NT(150)+NT(27)+NT(241)+NT(11)+T(3)+NT(11)+NT(403)));
//G613: def_output_cmd(401)  => __E_cli_command_183(402).
	p(NT(401), (NT(402)));
//G614: cli_command(282)     => def_output_cmd(401).
	p(NT(282), (NT(401)));
//G615: __E___E_cli_command_184_185(407) => 'h' 'i' 's' 't'.
	p(NT(407), (T(56)+T(10)+T(12)+T(13)));
//G616: __E___E_cli_command_184_185(407) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(407), (T(56)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G617: history_sym(406)     => __E___E_cli_command_184_185(407).
	p(NT(406), (NT(407)));
//G618: __E_cli_command_184(405) => history_sym(406).
	p(NT(405), (NT(406)));
//G619: history_list_cmd(404) => __E_cli_command_184(405).
	p(NT(404), (NT(405)));
//G620: cli_command(282)     => history_list_cmd(404).
	p(NT(282), (NT(404)));
//G621: __E_cli_command_186(409) => history_sym(406) __(27) memory(328).
	p(NT(409), (NT(406)+NT(27)+NT(328)));
//G622: history_print_cmd(408) => __E_cli_command_186(409).
	p(NT(408), (NT(409)));
//G623: cli_command(282)     => history_print_cmd(408).
	p(NT(282), (NT(408)));
//G624: __E_cli_command_187(411) => wff(14).
	p(NT(411), (NT(14)));
//G625: __E_cli_command_187(411) => bf(21).
	p(NT(411), (NT(21)));
//G626: history_store_cmd(410) => __E_cli_command_187(411).
	p(NT(410), (NT(411)));
//G627: cli_command(282)     => history_store_cmd(410).
	p(NT(282), (NT(410)));
//G628: __E_solve_options_188(412) => __(27) solve_option(413).
	p(NT(412), (NT(27)+NT(413)));
//G629: __E_solve_options_189(414) => null.
	p(NT(414), (nul));
//G630: __E_solve_options_189(414) => __E_solve_options_188(412) __E_solve_options_189(414).
	p(NT(414), (NT(412)+NT(414)));
//G631: solve_options(318)   => __E_solve_options_189(414).
	p(NT(318), (NT(414)));
//G632: __E_solve_option_190(415) => '-' '-' solver_mode(416).
	p(NT(415), (T(28)+T(28)+NT(416)));
//G633: solve_option(413)    => __E_solve_option_190(415).
	p(NT(413), (NT(415)));
//G634: __E_solve_option_191(417) => '-' '-' type(150).
	p(NT(417), (T(28)+T(28)+NT(150)));
//G635: solve_option(413)    => __E_solve_option_191(417).
	p(NT(413), (NT(417)));
//G636: __E_solver_mode_192(419) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(419), (T(18)+T(10)+T(55)+T(10)+T(18)+T(45)+T(18)));
//G637: __E_solver_mode_192(419) => 'm' 'i' 'n'.
	p(NT(419), (T(18)+T(10)+T(55)));
//G638: solver_mode_minimum(418) => __E_solver_mode_192(419).
	p(NT(418), (NT(419)));
//G639: solver_mode(416)     => solver_mode_minimum(418).
	p(NT(416), (NT(418)));
//G640: __E_solver_mode_193(421) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(421), (T(18)+T(5)+T(27)+T(10)+T(18)+T(45)+T(18)));
//G641: __E_solver_mode_193(421) => 'm' 'a' 'x'.
	p(NT(421), (T(18)+T(5)+T(27)));
//G642: solver_mode_maximum(420) => __E_solver_mode_193(421).
	p(NT(420), (NT(421)));
//G643: solver_mode(416)     => solver_mode_maximum(420).
	p(NT(416), (NT(420)));
//G644: __E_input_stream_194(422) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(423) _(11) ')'.
	p(NT(422), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(423)+NT(11)+T(16)));
//G645: input_stream(400)    => __E_input_stream_194(422).
	p(NT(400), (NT(422)));
//G646: console_sym(424)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(424), (T(8)+T(17)+T(55)+T(12)+T(17)+T(6)+T(19)));
//G647: input_stream(400)    => console_sym(424).
	p(NT(400), (NT(424)));
//G648: __E_output_stream_195(425) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(423) _(11) ')'.
	p(NT(425), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(423)+NT(11)+T(16)));
//G649: output_stream(403)   => __E_output_stream_195(425).
	p(NT(403), (NT(425)));
//G650: output_stream(403)   => console_sym(424).
	p(NT(403), (NT(424)));
//G651: q_file_name(423)     => '"' file_name(426) '"'.
	p(NT(423), (T(61)+NT(426)+T(61)));
//G652: __E_file_name_196(427) => printable(8).
	p(NT(427), (NT(8)));
//G653: __E_file_name_196(427) => printable(8) __E_file_name_196(427).
	p(NT(427), (NT(8)+NT(427)));
//G654: file_name(426)       => __E_file_name_196(427).
	p(NT(426), (NT(427)));
//G655: __E_option_name_197(428) => alnum(6).
	p(NT(428), (NT(6)));
//G656: __E_option_name_197(428) => alnum(6) __E_option_name_197(428).
	p(NT(428), (NT(6)+NT(428)));
//G657: option_name(383)     => __E_option_name_197(428).
	p(NT(383), (NT(428)));
//G658: __E_option_value_198(429) => alnum(6).
	p(NT(429), (NT(6)));
//G659: __E_option_value_198(429) => alnum(6) __E_option_value_198(429).
	p(NT(429), (NT(6)+NT(429)));
//G660: option_value(388)    => __E_option_value_198(429).
	p(NT(388), (NT(429)));
//G661: number(374)          => digits(255).
	p(NT(374), (NT(255)));
//G662: bf_cmd_arg(430)      => memory(328).
	p(NT(430), (NT(328)));
//G663: bf_cmd_arg(430)      => bf(21).
	p(NT(430), (NT(21)));
//G664: wff_cmd_arg(319)     => memory(328).
	p(NT(319), (NT(328)));
//G665: wff_cmd_arg(319)     => wff(14).
	p(NT(319), (NT(14)));
//G666: nf_cmd_arg(337)      => memory(328).
	p(NT(337), (NT(328)));
//G667: nf_cmd_arg(337)      => ref(18).
	p(NT(337), (NT(18)));
//G668: nf_cmd_arg(337)      => wff(14).
	p(NT(337), (NT(14)));
//G669: nf_cmd_arg(337)      => bf(21).
	p(NT(337), (NT(21)));
//G670: onf_cmd_arg(367)     => memory(328).
	p(NT(367), (NT(328)));
//G671: onf_cmd_arg(367)     => wff(14).
	p(NT(367), (NT(14)));
//G672: normalize_cmd_arg(307) => memory(328).
	p(NT(307), (NT(328)));
//G673: normalize_cmd_arg(307) => rr(10).
	p(NT(307), (NT(10)));
//G674: normalize_cmd_arg(307) => ref(18).
	p(NT(307), (NT(18)));
//G675: normalize_cmd_arg(307) => wff(14).
	p(NT(307), (NT(14)));
//G676: normalize_cmd_arg(307) => bf(21).
	p(NT(307), (NT(21)));
//G677: inst_args(342)       => wff_cmd_arg(319) _(11) '[' _(11) variable(139) _(11) '/' _(11) bf_cmd_arg(430) _(11) ']'.
	p(NT(342), (NT(319)+NT(11)+T(24)+NT(11)+NT(139)+NT(11)+T(43)+NT(11)+NT(430)+NT(11)+T(25)));
//G678: inst_args(342)       => bf_cmd_arg(430) _(11) '[' _(11) variable(139) _(11) '/' _(11) bf_cmd_arg(430) _(11) ']'.
	p(NT(342), (NT(430)+NT(11)+T(24)+NT(11)+NT(139)+NT(11)+T(43)+NT(11)+NT(430)+NT(11)+T(25)));
//G679: help_arg(299)        => help_sym(296).
	p(NT(299), (NT(296)));
//G680: help_arg(299)        => version_sym(289).
	p(NT(299), (NT(289)));
//G681: help_arg(299)        => quit_sym(286).
	p(NT(299), (NT(286)));
//G682: help_arg(299)        => clear_sym(292).
	p(NT(299), (NT(292)));
//G683: help_arg(299)        => get_sym(381).
	p(NT(299), (NT(381)));
//G684: help_arg(299)        => set_sym(386).
	p(NT(299), (NT(386)));
//G685: help_arg(299)        => enable_sym(391).
	p(NT(299), (NT(391)));
//G686: help_arg(299)        => disable_sym(394).
	p(NT(299), (NT(394)));
//G687: help_arg(299)        => toggle_sym(397).
	p(NT(299), (NT(397)));
//G688: help_arg(299)        => file_sym(302).
	p(NT(299), (NT(302)));
//G689: help_arg(299)        => history_sym(406).
	p(NT(299), (NT(406)));
//G690: help_arg(299)        => abs_memory_sym(431).
	p(NT(299), (NT(431)));
//G691: help_arg(299)        => rel_memory_sym(432).
	p(NT(299), (NT(432)));
//G692: help_arg(299)        => selection_sym(433).
	p(NT(299), (NT(433)));
//G693: help_arg(299)        => def_sym(370).
	p(NT(299), (NT(370)));
//G694: help_arg(299)        => inst_sym(340).
	p(NT(299), (NT(340)));
//G695: help_arg(299)        => subst_sym(335).
	p(NT(299), (NT(335)));
//G696: help_arg(299)        => normalize_sym(331).
	p(NT(299), (NT(331)));
//G697: help_arg(299)        => execute_sym(434).
	p(NT(299), (NT(434)));
//G698: help_arg(299)        => solve_sym(316).
	p(NT(299), (NT(316)));
//G699: help_arg(299)        => valid_sym(306).
	p(NT(299), (NT(306)));
//G700: help_arg(299)        => sat_sym(310).
	p(NT(299), (NT(310)));
//G701: help_arg(299)        => unsat_sym(313).
	p(NT(299), (NT(313)));
//G702: help_arg(299)        => run_sym(325).
	p(NT(299), (NT(325)));
//G703: help_arg(299)        => dnf_sym(345).
	p(NT(299), (NT(345)));
//G704: help_arg(299)        => cnf_sym(348).
	p(NT(299), (NT(348)));
//G705: help_arg(299)        => anf_sym(351).
	p(NT(299), (NT(351)));
//G706: help_arg(299)        => snf_sym(363).
	p(NT(299), (NT(363)));
//G707: help_arg(299)        => nnf_sym(354).
	p(NT(299), (NT(354)));
//G708: help_arg(299)        => pnf_sym(357).
	p(NT(299), (NT(357)));
//G709: help_arg(299)        => mnf_sym(360).
	p(NT(299), (NT(360)));
//G710: help_arg(299)        => onf_sym(366).
	p(NT(299), (NT(366)));
//G711: help_arg(299)        => qelim_sym(378).
	p(NT(299), (NT(378)));
//G712: __E___E_help_arg_199_200(437) => 's'.
	p(NT(437), (T(12)));
//G713: __E___E_help_arg_199_200(437) => null.
	p(NT(437), (nul));
//G714: __E_help_arg_199(436) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_199_200(437).
	p(NT(436), (T(19)+T(27)+T(5)+T(18)+T(57)+T(6)+T(19)+NT(437)));
//G715: examples_sym(435)    => __E_help_arg_199(436).
	p(NT(435), (NT(436)));
//G716: help_arg(299)        => examples_sym(435).
	p(NT(299), (NT(435)));
//G717: __E_memory_201(439)  => '%'.
	p(NT(439), (T(44)));
//G718: rel_memory_sym(432)  => '%' '-'.
	p(NT(432), (T(44)+T(28)));
//G719: memory_id(441)       => digits(255).
	p(NT(441), (NT(255)));
//G720: __E___E_memory_201_202(440) => memory_id(441).
	p(NT(440), (NT(441)));
//G721: __E___E_memory_201_202(440) => null.
	p(NT(440), (nul));
//G722: __E_memory_201(439)  => rel_memory_sym(432) __E___E_memory_201_202(440).
	p(NT(439), (NT(432)+NT(440)));
//G723: rel_memory(438)      => __E_memory_201(439).
	p(NT(438), (NT(439)));
//G724: memory(328)          => rel_memory(438).
	p(NT(328), (NT(438)));
//G725: abs_memory_sym(431)  => '%'.
	p(NT(431), (T(44)));
//G726: __E_memory_203(443)  => abs_memory_sym(431) memory_id(441).
	p(NT(443), (NT(431)+NT(441)));
//G727: abs_memory(442)      => __E_memory_203(443).
	p(NT(442), (NT(443)));
//G728: memory(328)          => abs_memory(442).
	p(NT(328), (NT(442)));
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
		__E___E_bf_64_65, bf_and_nosep, __E_bf_66, bf_and_nosep_1st_oprnd, __E___E_bf_66_67, bf_and_nosep_2nd_oprnd, __E___E_bf_66_68, z3_parenthesis, __E_z3_69, z3_constant, 
		z3_not, z3_add, __E_z3_70, __E___E_z3_70_71, z3_sub, __E_z3_72, __E___E_z3_72_73, z3_mul, __E_z3_74, __E___E_z3_74_75, 
		z3_div, __E_z3_76, __E___E_z3_76_77, z3_mod, __E_z3_78, __E___E_z3_78_79, z3_and, __E_z3_80, __E___E_z3_80_81, z3_or, 
		__E_z3_82, __E___E_z3_82_83, z3_xo, __E_z3_84, __E___E_z3_84_85, bits, _uint, _int, _real, bit, 
		__E_bits_86, __E_bits_87, integer, __E__int_88, num, _long, _ulong, __E__real_89, __E___E__real_89_90, __E___E___E__real_89_90_91, 
		ctn_neq, __E_constraint_92, ctnvar, ctn_eq, __E_constraint_93, ctn_greater_equal, __E_constraint_94, ctn_greater, __E_constraint_95, ctn_less_equal, 
		__E_constraint_96, ctn_less, __E_constraint_97, __E_variable_98, __E_variable_99, __E_variable_100, chars, __E_variable_101, __E_variable_102, __E_variable_103, 
		__E_variable_104, io_var, __E_variable_105, in, out, __E_variable_106, uninterpreted_constant, in_var_name, __E_in_107, __E___E_in_107_108, 
		offset, out_var_name, __E_out_109, __E___E_out_109_110, __E___E___E_out_109_110_111, q_var, __E_q_vars_112, __E_q_vars_113, __E_q_var_114, __E_offsets_115, 
		__E_offsets_116, shift, __E_offset_117, __E_shift_118, __E___E_shift_118_119, digits, __E_integer_120, uninter_const_name, __E_uninterpreted_constant_121, __E___E_uninterpreted_constant_121_122, 
		constant, __E_bf_constant_123, binding, __E_bf_constant_124, source, __E_binding_125, src_c, __E___E_binding_125_126, __E___E___E_binding_125_126_127, __E___E___E_binding_125_126_128, 
		__E_src_c_129, __E_src_c_130, __E_chars_131, __E_chars_132, __E_digits_133, comment, __E_comment_134, __E_comment_135, __E_comment_136, __E____137, 
		__E___138, cli, cli_command, __E_cli_139, __E_cli_140, quit_cmd, quit_sym, __E_cli_command_141, version_cmd, version_sym, 
		__E_cli_command_142, clear_cmd, clear_sym, __E_cli_command_143, help_cmd, __E_cli_command_144, help_sym, __E___E_cli_command_144_145, __E___E_cli_command_144_146, help_arg, 
		file_cmd, __E_cli_command_147, file_sym, q_string, valid_cmd, __E_cli_command_148, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_149, 
		sat_sym, unsat_cmd, __E_cli_command_150, unsat_sym, solve_cmd, __E_cli_command_151, solve_sym, __E___E_cli_command_151_152, solve_options, wff_cmd_arg, 
		lgrs_cmd, __E_cli_command_153, lgrs_sym, run_cmd, __E_cli_command_154, run_sym, __E___E_cli_command_154_155, __E___E_cli_command_154_156, memory, normalize_cmd, 
		__E_cli_command_157, normalize_sym, __E___E_cli_command_157_158, subst_cmd, __E_cli_command_159, subst_sym, __E___E_cli_command_159_160, nf_cmd_arg, inst_cmd, __E_cli_command_161, 
		inst_sym, __E___E_cli_command_161_162, inst_args, dnf_cmd, __E_cli_command_163, dnf_sym, cnf_cmd, __E_cli_command_164, cnf_sym, anf_cmd, 
		__E_cli_command_165, anf_sym, nnf_cmd, __E_cli_command_166, nnf_sym, pnf_cmd, __E_cli_command_167, pnf_sym, mnf_cmd, __E_cli_command_168, 
		mnf_sym, snf_cmd, __E_cli_command_169, snf_sym, onf_cmd, __E_cli_command_170, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_171, 
		def_sym, __E___E_cli_command_171_172, def_print_cmd, __E_cli_command_173, number, def_rr_cmd, qelim_cmd, __E_cli_command_174, qelim_sym, get_cmd, 
		__E_cli_command_175, get_sym, __E___E_cli_command_175_176, option_name, set_cmd, __E_cli_command_177, set_sym, __E___E_cli_command_177_178, option_value, enable_cmd, 
		__E_cli_command_179, enable_sym, disable_cmd, __E_cli_command_180, disable_sym, toggle_cmd, __E_cli_command_181, toggle_sym, def_input_cmd, __E_cli_command_182, 
		input_stream, def_output_cmd, __E_cli_command_183, output_stream, history_list_cmd, __E_cli_command_184, history_sym, __E___E_cli_command_184_185, history_print_cmd, __E_cli_command_186, 
		history_store_cmd, __E_cli_command_187, __E_solve_options_188, solve_option, __E_solve_options_189, __E_solve_option_190, solver_mode, __E_solve_option_191, solver_mode_minimum, __E_solver_mode_192, 
		solver_mode_maximum, __E_solver_mode_193, __E_input_stream_194, q_file_name, console_sym, __E_output_stream_195, file_name, __E_file_name_196, __E_option_name_197, __E_option_value_198, 
		bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, execute_sym, examples_sym, __E_help_arg_199, __E___E_help_arg_199_200, rel_memory, __E_memory_201, 
		__E___E_memory_201_202, memory_id, abs_memory, __E_memory_203, named_binding, z3_bv, z3_bv_parenthesis, __N_0, __N_1, __N_2, 
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
