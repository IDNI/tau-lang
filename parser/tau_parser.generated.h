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
	"__E_constraint_96", "ctn_less", "__E_constraint_97", "__E_variable_98", "__E_variable_99", "__E_variable_100", "__E_variable_101", "__E_variable_102", "io_var", "__E_variable_103", 
	"in", "out", "uninterpreted_constant", "in_var_name", "__E_in_104", "__E___E_in_104_105", "offset", "out_var_name", "__E_out_106", "__E___E_out_106_107", 
	"__E___E___E_out_106_107_108", "chars", "q_var", "__E_q_vars_109", "__E_q_vars_110", "__E_offsets_111", "__E_offsets_112", "shift", "__E_offset_113", "__E_shift_114", 
	"__E___E_shift_114_115", "digits", "__E_integer_116", "uninter_const_name", "__E_uninterpreted_constant_117", "__E___E_uninterpreted_constant_117_118", "constant", "__E_bf_constant_119", "binding", "__E_bf_constant_120", 
	"source", "__E_binding_121", "src_c", "__E___E_binding_121_122", "__E___E___E_binding_121_122_123", "__E___E___E_binding_121_122_124", "__E_src_c_125", "__E_src_c_126", "__E_chars_127", "__E_chars_128", 
	"__E_digits_129", "comment", "__E_comment_130", "__E_comment_131", "__E_comment_132", "__E____133", "__E___134", "cli", "cli_command", "__E_cli_135", 
	"__E_cli_136", "quit_cmd", "quit_sym", "__E_cli_command_137", "version_cmd", "version_sym", "__E_cli_command_138", "clear_cmd", "clear_sym", "__E_cli_command_139", 
	"help_cmd", "__E_cli_command_140", "help_sym", "__E___E_cli_command_140_141", "__E___E_cli_command_140_142", "help_arg", "file_cmd", "__E_cli_command_143", "file_sym", "q_string", 
	"valid_cmd", "__E_cli_command_144", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_145", "sat_sym", "unsat_cmd", "__E_cli_command_146", "unsat_sym", 
	"solve_cmd", "__E_cli_command_147", "solve_sym", "__E___E_cli_command_147_148", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_149", "lgrs_sym", "run_cmd", 
	"__E_cli_command_150", "run_sym", "__E___E_cli_command_150_151", "__E___E_cli_command_150_152", "memory", "normalize_cmd", "__E_cli_command_153", "normalize_sym", "__E___E_cli_command_153_154", "subst_cmd", 
	"__E_cli_command_155", "subst_sym", "__E___E_cli_command_155_156", "nf_cmd_arg", "inst_cmd", "__E_cli_command_157", "inst_sym", "__E___E_cli_command_157_158", "inst_args", "dnf_cmd", 
	"__E_cli_command_159", "dnf_sym", "cnf_cmd", "__E_cli_command_160", "cnf_sym", "anf_cmd", "__E_cli_command_161", "anf_sym", "nnf_cmd", "__E_cli_command_162", 
	"nnf_sym", "pnf_cmd", "__E_cli_command_163", "pnf_sym", "mnf_cmd", "__E_cli_command_164", "mnf_sym", "snf_cmd", "__E_cli_command_165", "snf_sym", 
	"onf_cmd", "__E_cli_command_166", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_167", "def_sym", "__E___E_cli_command_167_168", "def_print_cmd", "__E_cli_command_169", 
	"number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_170", "qelim_sym", "get_cmd", "__E_cli_command_171", "get_sym", "__E___E_cli_command_171_172", "option_name", 
	"set_cmd", "__E_cli_command_173", "set_sym", "__E___E_cli_command_173_174", "option_value", "enable_cmd", "__E_cli_command_175", "enable_sym", "disable_cmd", "__E_cli_command_176", 
	"disable_sym", "toggle_cmd", "__E_cli_command_177", "toggle_sym", "def_input_cmd", "__E_cli_command_178", "input_stream", "def_output_cmd", "__E_cli_command_179", "output_stream", 
	"history_list_cmd", "__E_cli_command_180", "history_sym", "__E___E_cli_command_180_181", "history_print_cmd", "__E_cli_command_182", "history_store_cmd", "__E_cli_command_183", "__E_solve_options_184", "solve_option", 
	"__E_solve_options_185", "__E_solve_option_186", "solver_mode", "__E_solve_option_187", "solver_mode_minimum", "__E_solver_mode_188", "solver_mode_maximum", "__E_solver_mode_189", "__E_input_stream_190", "q_file_name", 
	"console_sym", "__E_output_stream_191", "file_name", "__E_file_name_192", "__E_option_name_193", "__E_option_value_194", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", 
	"execute_sym", "examples_sym", "__E_help_arg_195", "__E___E_help_arg_195_196", "rel_memory", "__E_memory_197", "__E___E_memory_197_198", "memory_id", "abs_memory", "__E_memory_199", 
	"named_binding", "z3_bv", "z3_bv_parenthesis", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", 
	"__N_7", "__N_8", 
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
			20, 22, 139, 150, 202, 212, 233, 237, 251, 253,
			260, 379, 384, 422, 440
		},
		.to_inline = {
			{ 14, 60, 14 },
			{ 21, 136, 21 },
			{ 159, 136, 21 },
			{ 161, 136, 154 },
			{ 163, 136, 21 },
			{ 165, 136, 21 },
			{ 241 },
			{ 242 },
			{ 262 },
			{ 303 },
			{ 315 },
			{ 333 },
			{ 338 },
			{ 363 },
			{ 426 },
			{ 441, 442, 441 }
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
//G335: __N_0(443)           => 'F'.
	p(NT(443), (T(34)));
//G336: __N_1(444)           => 'T'.
	p(NT(444), (T(33)));
//G337: __E_variable_98(223) => ~( __N_0(443) ) & ~( __N_1(444) ) & alpha(5).	 # conjunctive
	p(NT(223), ~(NT(443)) & ~(NT(444)) & (NT(5)));
//G338: __E_variable_99(224) => null.
	p(NT(224), (nul));
//G339: __E_variable_99(224) => digit(3) __E_variable_99(224).
	p(NT(224), (NT(3)+NT(224)));
//G340: variable(139)        => __E_variable_98(223) __E_variable_99(224).	 # guarded: charvar
	p(NT(139), (NT(223)+NT(224)));
	p.back().guard = "charvar";
//G341: __N_2(445)           => 'F'.
	p(NT(445), (T(34)));
//G342: __N_3(446)           => 'T'.
	p(NT(446), (T(33)));
//G343: __E_variable_100(225) => ~( __N_2(445) ) & ~( __N_3(446) ) & alpha(5).	 # conjunctive
	p(NT(225), ~(NT(445)) & ~(NT(446)) & (NT(5)));
//G344: __E_variable_101(226) => alnum(6).
	p(NT(226), (NT(6)));
//G345: __E_variable_101(226) => '_'.
	p(NT(226), (T(35)));
//G346: __E_variable_102(227) => null.
	p(NT(227), (nul));
//G347: __E_variable_102(227) => __E_variable_101(226) __E_variable_102(227).
	p(NT(227), (NT(226)+NT(227)));
//G348: variable(139)        => __E_variable_100(225) __E_variable_102(227).	 # guarded: var
	p(NT(139), (NT(225)+NT(227)));
	p.back().guard = "var";
//G349: __E_variable_103(229) => in(230).
	p(NT(229), (NT(230)));
//G350: __E_variable_103(229) => out(231).
	p(NT(229), (NT(231)));
//G351: io_var(228)          => __E_variable_103(229).
	p(NT(228), (NT(229)));
//G352: variable(139)        => io_var(228).
	p(NT(139), (NT(228)));
//G353: variable(139)        => uninterpreted_constant(232).
	p(NT(139), (NT(232)));
//G354: __E___E_in_104_105(235) => digit(3).
	p(NT(235), (NT(3)));
//G355: __E___E_in_104_105(235) => digit(3) __E___E_in_104_105(235).
	p(NT(235), (NT(3)+NT(235)));
//G356: __E_in_104(234)      => 'i' __E___E_in_104_105(235).
	p(NT(234), (T(10)+NT(235)));
//G357: in_var_name(233)     => __E_in_104(234).
	p(NT(233), (NT(234)));
//G358: in(230)              => in_var_name(233) '[' offset(236) ']'.
	p(NT(230), (NT(233)+T(24)+NT(236)+T(25)));
//G359: __E___E___E_out_106_107_108(240) => digit(3).
	p(NT(240), (NT(3)));
//G360: __E___E___E_out_106_107_108(240) => digit(3) __E___E___E_out_106_107_108(240).
	p(NT(240), (NT(3)+NT(240)));
//G361: __E___E_out_106_107(239) => 'o' __E___E___E_out_106_107_108(240).
	p(NT(239), (T(17)+NT(240)));
//G362: __E_out_106(238)     => __E___E_out_106_107(239).
	p(NT(238), (NT(239)));
//G363: __E_out_106(238)     => 'u'.
	p(NT(238), (T(45)));
//G364: out_var_name(237)    => __E_out_106(238).
	p(NT(237), (NT(238)));
//G365: out(231)             => out_var_name(237) '[' offset(236) ']'.
	p(NT(231), (NT(237)+T(24)+NT(236)+T(25)));
//G366: ctnvar(212)          => chars(241).
	p(NT(212), (NT(241)));
//G367: capture(20)          => '$' chars(241).
	p(NT(20), (T(46)+NT(241)));
//G368: __E_q_vars_109(243)  => _(11) ',' _(11) q_var(242).
	p(NT(243), (NT(11)+T(14)+NT(11)+NT(242)));
//G369: __E_q_vars_110(244)  => null.
	p(NT(244), (nul));
//G370: __E_q_vars_110(244)  => __E_q_vars_109(243) __E_q_vars_110(244).
	p(NT(244), (NT(243)+NT(244)));
//G371: q_vars(72)           => q_var(242) __E_q_vars_110(244).
	p(NT(72), (NT(242)+NT(244)));
//G372: q_var(242)           => capture(20).
	p(NT(242), (NT(20)));
//G373: __N_4(447)           => uninterpreted_constant(232).
	p(NT(447), (NT(232)));
//G374: q_var(242)           => variable(139) & ~( __N_4(447) ).	 # conjunctive
	p(NT(242), (NT(139)) & ~(NT(447)));
//G375: __E_offsets_111(245) => _(11) ',' _(11) offset(236).
	p(NT(245), (NT(11)+T(14)+NT(11)+NT(236)));
//G376: __E_offsets_112(246) => null.
	p(NT(246), (nul));
//G377: __E_offsets_112(246) => __E_offsets_111(245) __E_offsets_112(246).
	p(NT(246), (NT(245)+NT(246)));
//G378: offsets(24)          => '[' _(11) offset(236) __E_offsets_112(246) _(11) ']'.
	p(NT(24), (T(24)+NT(11)+NT(236)+NT(246)+NT(11)+T(25)));
//G379: offset(236)          => integer(202).
	p(NT(236), (NT(202)));
//G380: offset(236)          => capture(20).
	p(NT(236), (NT(20)));
//G381: offset(236)          => shift(247).
	p(NT(236), (NT(247)));
//G382: __N_5(448)           => io_var(228).
	p(NT(448), (NT(228)));
//G383: __E_offset_113(248)  => variable(139) & ~( __N_5(448) ).	 # conjunctive
	p(NT(248), (NT(139)) & ~(NT(448)));
//G384: offset(236)          => __E_offset_113(248).
	p(NT(236), (NT(248)));
//G385: __E_shift_114(249)   => capture(20).
	p(NT(249), (NT(20)));
//G386: __N_6(449)           => io_var(228).
	p(NT(449), (NT(228)));
//G387: __E___E_shift_114_115(250) => variable(139) & ~( __N_6(449) ).	 # conjunctive
	p(NT(250), (NT(139)) & ~(NT(449)));
//G388: __E_shift_114(249)   => __E___E_shift_114_115(250).
	p(NT(249), (NT(250)));
//G389: shift(247)           => __E_shift_114(249) _(11) '-' _(11) num(204).
	p(NT(247), (NT(249)+NT(11)+T(28)+NT(11)+NT(204)));
//G390: num(204)             => digits(251).
	p(NT(204), (NT(251)));
//G391: __E_integer_116(252) => '-'.
	p(NT(252), (T(28)));
//G392: __E_integer_116(252) => null.
	p(NT(252), (nul));
//G393: integer(202)         => __E_integer_116(252) _(11) digits(251).
	p(NT(202), (NT(252)+NT(11)+NT(251)));
//G394: __E___E_uninterpreted_constant_117_118(255) => chars(241).
	p(NT(255), (NT(241)));
//G395: __E___E_uninterpreted_constant_117_118(255) => _(11).
	p(NT(255), (NT(11)));
//G396: __E_uninterpreted_constant_117(254) => __E___E_uninterpreted_constant_117_118(255) ':' chars(241).
	p(NT(254), (NT(255)+T(2)+NT(241)));
//G397: uninter_const_name(253) => __E_uninterpreted_constant_117(254).
	p(NT(253), (NT(254)));
//G398: uninterpreted_constant(232) => '<' uninter_const_name(253) _(11) '>'.
	p(NT(232), (T(20)+NT(253)+NT(11)+T(21)));
//G399: __E_bf_constant_119(257) => capture(20).
	p(NT(257), (NT(20)));
//G400: __E_bf_constant_119(257) => binding(258).
	p(NT(257), (NT(258)));
//G401: constant(256)        => __E_bf_constant_119(257).
	p(NT(256), (NT(257)));
//G402: __E_bf_constant_120(259) => _(11) ':' _(11) type(150).
	p(NT(259), (NT(11)+T(2)+NT(11)+NT(150)));
//G403: __E_bf_constant_120(259) => null.
	p(NT(259), (nul));
//G404: bf_constant(138)     => '{' _(11) constant(256) _(11) '}' __E_bf_constant_120(259).
	p(NT(138), (T(47)+NT(11)+NT(256)+NT(11)+T(48)+NT(259)));
//G405: __E___E___E_binding_121_122_123(264) => src_c(262).
	p(NT(264), (NT(262)));
//G406: __E___E___E_binding_121_122_123(264) => space(2).
	p(NT(264), (NT(2)));
//G407: __E___E___E_binding_121_122_124(265) => null.
	p(NT(265), (nul));
//G408: __E___E___E_binding_121_122_124(265) => __E___E___E_binding_121_122_123(264) __E___E___E_binding_121_122_124(265).
	p(NT(265), (NT(264)+NT(265)));
//G409: __E___E_binding_121_122(263) => __E___E___E_binding_121_122_124(265) src_c(262).
	p(NT(263), (NT(265)+NT(262)));
//G410: __E___E_binding_121_122(263) => null.
	p(NT(263), (nul));
//G411: __E_binding_121(261) => src_c(262) __E___E_binding_121_122(263).
	p(NT(261), (NT(262)+NT(263)));
//G412: source(260)          => __E_binding_121(261).
	p(NT(260), (NT(261)));
//G413: binding(258)         => source(260).
	p(NT(258), (NT(260)));
//G414: src_c(262)           => alnum(6).
	p(NT(262), (NT(6)));
//G415: __N_7(450)           => '{'.
	p(NT(450), (T(47)));
//G416: __N_8(451)           => '}'.
	p(NT(451), (T(48)));
//G417: src_c(262)           => ~( __N_7(450) ) & ~( __N_8(451) ) & punct(7).	 # conjunctive
	p(NT(262), ~(NT(450)) & ~(NT(451)) & (NT(7)));
//G418: __E_src_c_125(266)   => src_c(262).
	p(NT(266), (NT(262)));
//G419: __E_src_c_125(266)   => space(2).
	p(NT(266), (NT(2)));
//G420: __E_src_c_126(267)   => null.
	p(NT(267), (nul));
//G421: __E_src_c_126(267)   => __E_src_c_125(266) __E_src_c_126(267).
	p(NT(267), (NT(266)+NT(267)));
//G422: src_c(262)           => '{' __E_src_c_126(267) '}'.
	p(NT(262), (T(47)+NT(267)+T(48)));
//G423: type(150)            => chars(241).
	p(NT(150), (NT(241)));
//G424: type(150)            => chars(241) _(11) '[' _(11) num(204) _(11) ']'.
	p(NT(150), (NT(241)+NT(11)+T(24)+NT(11)+NT(204)+NT(11)+T(25)));
//G425: __E_chars_127(268)   => alnum(6).
	p(NT(268), (NT(6)));
//G426: __E_chars_127(268)   => '_'.
	p(NT(268), (T(35)));
//G427: __E_chars_128(269)   => null.
	p(NT(269), (nul));
//G428: __E_chars_128(269)   => __E_chars_127(268) __E_chars_128(269).
	p(NT(269), (NT(268)+NT(269)));
//G429: chars(241)           => alpha(5) __E_chars_128(269).
	p(NT(241), (NT(5)+NT(269)));
//G430: __E_digits_129(270)  => digit(3).
	p(NT(270), (NT(3)));
//G431: __E_digits_129(270)  => digit(3) __E_digits_129(270).
	p(NT(270), (NT(3)+NT(270)));
//G432: digits(251)          => __E_digits_129(270).
	p(NT(251), (NT(270)));
//G433: sym(22)              => chars(241).
	p(NT(22), (NT(241)));
//G434: __E_comment_130(272) => printable(8).
	p(NT(272), (NT(8)));
//G435: __E_comment_130(272) => '\t'.
	p(NT(272), (T(49)));
//G436: __E_comment_131(273) => null.
	p(NT(273), (nul));
//G437: __E_comment_131(273) => __E_comment_130(272) __E_comment_131(273).
	p(NT(273), (NT(272)+NT(273)));
//G438: __E_comment_132(274) => '\n'.
	p(NT(274), (T(50)));
//G439: __E_comment_132(274) => '\r'.
	p(NT(274), (T(51)));
//G440: __E_comment_132(274) => eof(1).
	p(NT(274), (NT(1)));
//G441: comment(271)         => '#' __E_comment_131(273) __E_comment_132(274).
	p(NT(271), (T(52)+NT(273)+NT(274)));
//G442: __E____133(275)      => space(2).
	p(NT(275), (NT(2)));
//G443: __E____133(275)      => comment(271).
	p(NT(275), (NT(271)));
//G444: __(27)               => __E____133(275) _(11).
	p(NT(27), (NT(275)+NT(11)));
//G445: __E___134(276)       => __(27).
	p(NT(276), (NT(27)));
//G446: __E___134(276)       => null.
	p(NT(276), (nul));
//G447: _(11)                => __E___134(276).
	p(NT(11), (NT(276)));
//G448: cli(277)             => _(11).
	p(NT(277), (NT(11)));
//G449: __E_cli_135(279)     => '.' _(11) cli_command(278) _(11).
	p(NT(279), (T(1)+NT(11)+NT(278)+NT(11)));
//G450: __E_cli_136(280)     => null.
	p(NT(280), (nul));
//G451: __E_cli_136(280)     => __E_cli_135(279) __E_cli_136(280).
	p(NT(280), (NT(279)+NT(280)));
//G452: cli(277)             => _(11) cli_command(278) _(11) __E_cli_136(280).
	p(NT(277), (NT(11)+NT(278)+NT(11)+NT(280)));
//G453: __E_cli_command_137(283) => 'q'.
	p(NT(283), (T(53)));
//G454: __E_cli_command_137(283) => 'q' 'u' 'i' 't'.
	p(NT(283), (T(53)+T(45)+T(10)+T(13)));
//G455: quit_sym(282)        => __E_cli_command_137(283).
	p(NT(282), (NT(283)));
//G456: quit_cmd(281)        => quit_sym(282).
	p(NT(281), (NT(282)));
//G457: cli_command(278)     => quit_cmd(281).
	p(NT(278), (NT(281)));
//G458: __E_cli_command_138(286) => 'v'.
	p(NT(286), (T(54)));
//G459: __E_cli_command_138(286) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(286), (T(54)+T(19)+T(11)+T(12)+T(10)+T(17)+T(55)));
//G460: version_sym(285)     => __E_cli_command_138(286).
	p(NT(285), (NT(286)));
//G461: version_cmd(284)     => version_sym(285).
	p(NT(284), (NT(285)));
//G462: cli_command(278)     => version_cmd(284).
	p(NT(278), (NT(284)));
//G463: __E_cli_command_139(289) => 'c'.
	p(NT(289), (T(8)));
//G464: __E_cli_command_139(289) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(289), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G465: clear_sym(288)       => __E_cli_command_139(289).
	p(NT(288), (NT(289)));
//G466: clear_cmd(287)       => clear_sym(288).
	p(NT(287), (NT(288)));
//G467: cli_command(278)     => clear_cmd(287).
	p(NT(278), (NT(287)));
//G468: __E___E_cli_command_140_141(293) => 'h'.
	p(NT(293), (T(56)));
//G469: __E___E_cli_command_140_141(293) => 'h' 'e' 'l' 'p'.
	p(NT(293), (T(56)+T(19)+T(6)+T(57)));
//G470: help_sym(292)        => __E___E_cli_command_140_141(293).
	p(NT(292), (NT(293)));
//G471: __E___E_cli_command_140_142(294) => __(27) help_arg(295).
	p(NT(294), (NT(27)+NT(295)));
//G472: __E___E_cli_command_140_142(294) => null.
	p(NT(294), (nul));
//G473: __E_cli_command_140(291) => help_sym(292) __E___E_cli_command_140_142(294).
	p(NT(291), (NT(292)+NT(294)));
//G474: help_cmd(290)        => __E_cli_command_140(291).
	p(NT(290), (NT(291)));
//G475: cli_command(278)     => help_cmd(290).
	p(NT(278), (NT(290)));
//G476: file_sym(298)        => 'f' 'i' 'l' 'e'.
	p(NT(298), (T(4)+T(10)+T(6)+T(19)));
//G477: __E_cli_command_143(297) => file_sym(298) __(27) q_string(299).
	p(NT(297), (NT(298)+NT(27)+NT(299)));
//G478: file_cmd(296)        => __E_cli_command_143(297).
	p(NT(296), (NT(297)));
//G479: cli_command(278)     => file_cmd(296).
	p(NT(278), (NT(296)));
//G480: valid_sym(302)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(302), (T(54)+T(5)+T(6)+T(10)+T(58)));
//G481: __E_cli_command_144(301) => valid_sym(302) __(27) normalize_cmd_arg(303).
	p(NT(301), (NT(302)+NT(27)+NT(303)));
//G482: valid_cmd(300)       => __E_cli_command_144(301).
	p(NT(300), (NT(301)));
//G483: cli_command(278)     => valid_cmd(300).
	p(NT(278), (NT(300)));
//G484: sat_sym(306)         => 's' 'a' 't'.
	p(NT(306), (T(12)+T(5)+T(13)));
//G485: __E_cli_command_145(305) => sat_sym(306) __(27) normalize_cmd_arg(303).
	p(NT(305), (NT(306)+NT(27)+NT(303)));
//G486: sat_cmd(304)         => __E_cli_command_145(305).
	p(NT(304), (NT(305)));
//G487: cli_command(278)     => sat_cmd(304).
	p(NT(278), (NT(304)));
//G488: unsat_sym(309)       => 'u' 'n' 's' 'a' 't'.
	p(NT(309), (T(45)+T(55)+T(12)+T(5)+T(13)));
//G489: __E_cli_command_146(308) => unsat_sym(309) __(27) normalize_cmd_arg(303).
	p(NT(308), (NT(309)+NT(27)+NT(303)));
//G490: unsat_cmd(307)       => __E_cli_command_146(308).
	p(NT(307), (NT(308)));
//G491: cli_command(278)     => unsat_cmd(307).
	p(NT(278), (NT(307)));
//G492: solve_sym(312)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(312), (T(12)+T(17)+T(6)+T(54)+T(19)));
//G493: __E___E_cli_command_147_148(313) => solve_options(314).
	p(NT(313), (NT(314)));
//G494: __E___E_cli_command_147_148(313) => null.
	p(NT(313), (nul));
//G495: __E_cli_command_147(311) => solve_sym(312) __E___E_cli_command_147_148(313) __(27) wff_cmd_arg(315).
	p(NT(311), (NT(312)+NT(313)+NT(27)+NT(315)));
//G496: solve_cmd(310)       => __E_cli_command_147(311).
	p(NT(310), (NT(311)));
//G497: cli_command(278)     => solve_cmd(310).
	p(NT(278), (NT(310)));
//G498: lgrs_sym(318)        => 'l' 'g' 'r' 's'.
	p(NT(318), (T(6)+T(59)+T(11)+T(12)));
//G499: __E_cli_command_149(317) => lgrs_sym(318) __(27) wff_cmd_arg(315).
	p(NT(317), (NT(318)+NT(27)+NT(315)));
//G500: lgrs_cmd(316)        => __E_cli_command_149(317).
	p(NT(316), (NT(317)));
//G501: cli_command(278)     => lgrs_cmd(316).
	p(NT(278), (NT(316)));
//G502: __E___E_cli_command_150_151(322) => 'r'.
	p(NT(322), (T(11)));
//G503: __E___E_cli_command_150_151(322) => 'r' 'u' 'n'.
	p(NT(322), (T(11)+T(45)+T(55)));
//G504: run_sym(321)         => __E___E_cli_command_150_151(322).
	p(NT(321), (NT(322)));
//G505: __E___E_cli_command_150_152(323) => memory(324).
	p(NT(323), (NT(324)));
//G506: __E___E_cli_command_150_152(323) => wff(14).
	p(NT(323), (NT(14)));
//G507: __E_cli_command_150(320) => run_sym(321) __(27) __E___E_cli_command_150_152(323).
	p(NT(320), (NT(321)+NT(27)+NT(323)));
//G508: run_cmd(319)         => __E_cli_command_150(320).
	p(NT(319), (NT(320)));
//G509: cli_command(278)     => run_cmd(319).
	p(NT(278), (NT(319)));
//G510: __E___E_cli_command_153_154(328) => 'n'.
	p(NT(328), (T(55)));
//G511: __E___E_cli_command_153_154(328) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(328), (T(55)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(60)+T(19)));
//G512: normalize_sym(327)   => __E___E_cli_command_153_154(328).
	p(NT(327), (NT(328)));
//G513: __E_cli_command_153(326) => normalize_sym(327) __(27) normalize_cmd_arg(303).
	p(NT(326), (NT(327)+NT(27)+NT(303)));
//G514: normalize_cmd(325)   => __E_cli_command_153(326).
	p(NT(325), (NT(326)));
//G515: cli_command(278)     => normalize_cmd(325).
	p(NT(278), (NT(325)));
//G516: __E___E_cli_command_155_156(332) => 's'.
	p(NT(332), (T(12)));
//G517: __E___E_cli_command_155_156(332) => 's' 'u' 'b' 's' 't'.
	p(NT(332), (T(12)+T(45)+T(7)+T(12)+T(13)));
//G518: __E___E_cli_command_155_156(332) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(332), (T(12)+T(45)+T(7)+T(12)+T(13)+T(10)+T(13)+T(45)+T(13)+T(19)));
//G519: subst_sym(331)       => __E___E_cli_command_155_156(332).
	p(NT(331), (NT(332)));
//G520: __E_cli_command_155(330) => subst_sym(331) __(27) nf_cmd_arg(333) _(11) '[' _(11) nf_cmd_arg(333) _(11) '/' _(11) nf_cmd_arg(333) _(11) ']'.
	p(NT(330), (NT(331)+NT(27)+NT(333)+NT(11)+T(24)+NT(11)+NT(333)+NT(11)+T(43)+NT(11)+NT(333)+NT(11)+T(25)));
//G521: subst_cmd(329)       => __E_cli_command_155(330).
	p(NT(329), (NT(330)));
//G522: cli_command(278)     => subst_cmd(329).
	p(NT(278), (NT(329)));
//G523: __E___E_cli_command_157_158(337) => 'i'.
	p(NT(337), (T(10)));
//G524: __E___E_cli_command_157_158(337) => 'i' 'n' 's' 't'.
	p(NT(337), (T(10)+T(55)+T(12)+T(13)));
//G525: __E___E_cli_command_157_158(337) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(337), (T(10)+T(55)+T(12)+T(13)+T(5)+T(55)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G526: inst_sym(336)        => __E___E_cli_command_157_158(337).
	p(NT(336), (NT(337)));
//G527: __E_cli_command_157(335) => inst_sym(336) __(27) inst_args(338).
	p(NT(335), (NT(336)+NT(27)+NT(338)));
//G528: inst_cmd(334)        => __E_cli_command_157(335).
	p(NT(334), (NT(335)));
//G529: cli_command(278)     => inst_cmd(334).
	p(NT(278), (NT(334)));
//G530: dnf_sym(341)         => 'd' 'n' 'f'.
	p(NT(341), (T(58)+T(55)+T(4)));
//G531: __E_cli_command_159(340) => dnf_sym(341) __(27) nf_cmd_arg(333).
	p(NT(340), (NT(341)+NT(27)+NT(333)));
//G532: dnf_cmd(339)         => __E_cli_command_159(340).
	p(NT(339), (NT(340)));
//G533: cli_command(278)     => dnf_cmd(339).
	p(NT(278), (NT(339)));
//G534: cnf_sym(344)         => 'c' 'n' 'f'.
	p(NT(344), (T(8)+T(55)+T(4)));
//G535: __E_cli_command_160(343) => cnf_sym(344) __(27) nf_cmd_arg(333).
	p(NT(343), (NT(344)+NT(27)+NT(333)));
//G536: cnf_cmd(342)         => __E_cli_command_160(343).
	p(NT(342), (NT(343)));
//G537: cli_command(278)     => cnf_cmd(342).
	p(NT(278), (NT(342)));
//G538: anf_sym(347)         => 'a' 'n' 'f'.
	p(NT(347), (T(5)+T(55)+T(4)));
//G539: __E_cli_command_161(346) => anf_sym(347) __(27) nf_cmd_arg(333).
	p(NT(346), (NT(347)+NT(27)+NT(333)));
//G540: anf_cmd(345)         => __E_cli_command_161(346).
	p(NT(345), (NT(346)));
//G541: cli_command(278)     => anf_cmd(345).
	p(NT(278), (NT(345)));
//G542: nnf_sym(350)         => 'n' 'n' 'f'.
	p(NT(350), (T(55)+T(55)+T(4)));
//G543: __E_cli_command_162(349) => nnf_sym(350) __(27) nf_cmd_arg(333).
	p(NT(349), (NT(350)+NT(27)+NT(333)));
//G544: nnf_cmd(348)         => __E_cli_command_162(349).
	p(NT(348), (NT(349)));
//G545: cli_command(278)     => nnf_cmd(348).
	p(NT(278), (NT(348)));
//G546: pnf_sym(353)         => 'p' 'n' 'f'.
	p(NT(353), (T(57)+T(55)+T(4)));
//G547: __E_cli_command_163(352) => pnf_sym(353) __(27) nf_cmd_arg(333).
	p(NT(352), (NT(353)+NT(27)+NT(333)));
//G548: pnf_cmd(351)         => __E_cli_command_163(352).
	p(NT(351), (NT(352)));
//G549: cli_command(278)     => pnf_cmd(351).
	p(NT(278), (NT(351)));
//G550: mnf_sym(356)         => 'm' 'n' 'f'.
	p(NT(356), (T(18)+T(55)+T(4)));
//G551: __E_cli_command_164(355) => mnf_sym(356) __(27) nf_cmd_arg(333).
	p(NT(355), (NT(356)+NT(27)+NT(333)));
//G552: mnf_cmd(354)         => __E_cli_command_164(355).
	p(NT(354), (NT(355)));
//G553: cli_command(278)     => mnf_cmd(354).
	p(NT(278), (NT(354)));
//G554: snf_sym(359)         => 's' 'n' 'f'.
	p(NT(359), (T(12)+T(55)+T(4)));
//G555: __E_cli_command_165(358) => snf_sym(359) __(27) nf_cmd_arg(333).
	p(NT(358), (NT(359)+NT(27)+NT(333)));
//G556: snf_cmd(357)         => __E_cli_command_165(358).
	p(NT(357), (NT(358)));
//G557: cli_command(278)     => snf_cmd(357).
	p(NT(278), (NT(357)));
//G558: onf_sym(362)         => 'o' 'n' 'f'.
	p(NT(362), (T(17)+T(55)+T(4)));
//G559: __E_cli_command_166(361) => onf_sym(362) __(27) variable(139) __(27) onf_cmd_arg(363).
	p(NT(361), (NT(362)+NT(27)+NT(139)+NT(27)+NT(363)));
//G560: onf_cmd(360)         => __E_cli_command_166(361).
	p(NT(360), (NT(361)));
//G561: cli_command(278)     => onf_cmd(360).
	p(NT(278), (NT(360)));
//G562: __E___E_cli_command_167_168(367) => 'd' 'e' 'f' 's'.
	p(NT(367), (T(58)+T(19)+T(4)+T(12)));
//G563: __E___E_cli_command_167_168(367) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(367), (T(58)+T(19)+T(4)+T(10)+T(55)+T(10)+T(13)+T(10)+T(17)+T(55)+T(12)));
//G564: def_sym(366)         => __E___E_cli_command_167_168(367).
	p(NT(366), (NT(367)));
//G565: __E_cli_command_167(365) => def_sym(366).
	p(NT(365), (NT(366)));
//G566: def_list_cmd(364)    => __E_cli_command_167(365).
	p(NT(364), (NT(365)));
//G567: cli_command(278)     => def_list_cmd(364).
	p(NT(278), (NT(364)));
//G568: __E_cli_command_169(369) => def_sym(366) __(27) number(370).
	p(NT(369), (NT(366)+NT(27)+NT(370)));
//G569: def_print_cmd(368)   => __E_cli_command_169(369).
	p(NT(368), (NT(369)));
//G570: cli_command(278)     => def_print_cmd(368).
	p(NT(278), (NT(368)));
//G571: def_rr_cmd(371)      => rec_relation(16).
	p(NT(371), (NT(16)));
//G572: cli_command(278)     => def_rr_cmd(371).
	p(NT(278), (NT(371)));
//G573: qelim_sym(374)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(374), (T(53)+T(19)+T(6)+T(10)+T(18)));
//G574: __E_cli_command_170(373) => qelim_sym(374) __(27) wff_cmd_arg(315).
	p(NT(373), (NT(374)+NT(27)+NT(315)));
//G575: qelim_cmd(372)       => __E_cli_command_170(373).
	p(NT(372), (NT(373)));
//G576: cli_command(278)     => qelim_cmd(372).
	p(NT(278), (NT(372)));
//G577: get_sym(377)         => 'g' 'e' 't'.
	p(NT(377), (T(59)+T(19)+T(13)));
//G578: __E___E_cli_command_171_172(378) => __(27) option_name(379).
	p(NT(378), (NT(27)+NT(379)));
//G579: __E___E_cli_command_171_172(378) => null.
	p(NT(378), (nul));
//G580: __E_cli_command_171(376) => get_sym(377) __E___E_cli_command_171_172(378).
	p(NT(376), (NT(377)+NT(378)));
//G581: get_cmd(375)         => __E_cli_command_171(376).
	p(NT(375), (NT(376)));
//G582: cli_command(278)     => get_cmd(375).
	p(NT(278), (NT(375)));
//G583: set_sym(382)         => 's' 'e' 't'.
	p(NT(382), (T(12)+T(19)+T(13)));
//G584: __E___E_cli_command_173_174(383) => __(27).
	p(NT(383), (NT(27)));
//G585: __E___E_cli_command_173_174(383) => _(11) '=' _(11).
	p(NT(383), (NT(11)+T(3)+NT(11)));
//G586: __E_cli_command_173(381) => set_sym(382) __(27) option_name(379) __E___E_cli_command_173_174(383) option_value(384).
	p(NT(381), (NT(382)+NT(27)+NT(379)+NT(383)+NT(384)));
//G587: set_cmd(380)         => __E_cli_command_173(381).
	p(NT(380), (NT(381)));
//G588: cli_command(278)     => set_cmd(380).
	p(NT(278), (NT(380)));
//G589: enable_sym(387)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(387), (T(19)+T(55)+T(5)+T(7)+T(6)+T(19)));
//G590: __E_cli_command_175(386) => enable_sym(387) __(27) option_name(379).
	p(NT(386), (NT(387)+NT(27)+NT(379)));
//G591: enable_cmd(385)      => __E_cli_command_175(386).
	p(NT(385), (NT(386)));
//G592: cli_command(278)     => enable_cmd(385).
	p(NT(278), (NT(385)));
//G593: disable_sym(390)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(390), (T(58)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G594: __E_cli_command_176(389) => disable_sym(390) __(27) option_name(379).
	p(NT(389), (NT(390)+NT(27)+NT(379)));
//G595: disable_cmd(388)     => __E_cli_command_176(389).
	p(NT(388), (NT(389)));
//G596: cli_command(278)     => disable_cmd(388).
	p(NT(278), (NT(388)));
//G597: toggle_sym(393)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(393), (T(13)+T(17)+T(59)+T(59)+T(6)+T(19)));
//G598: __E_cli_command_177(392) => toggle_sym(393) __(27) option_name(379).
	p(NT(392), (NT(393)+NT(27)+NT(379)));
//G599: toggle_cmd(391)      => __E_cli_command_177(392).
	p(NT(391), (NT(392)));
//G600: cli_command(278)     => toggle_cmd(391).
	p(NT(278), (NT(391)));
//G601: __E_cli_command_178(395) => type(150) __(27) in_var_name(233) _(11) '=' _(11) input_stream(396).
	p(NT(395), (NT(150)+NT(27)+NT(233)+NT(11)+T(3)+NT(11)+NT(396)));
//G602: def_input_cmd(394)   => __E_cli_command_178(395).
	p(NT(394), (NT(395)));
//G603: cli_command(278)     => def_input_cmd(394).
	p(NT(278), (NT(394)));
//G604: __E_cli_command_179(398) => type(150) __(27) out_var_name(237) _(11) '=' _(11) output_stream(399).
	p(NT(398), (NT(150)+NT(27)+NT(237)+NT(11)+T(3)+NT(11)+NT(399)));
//G605: def_output_cmd(397)  => __E_cli_command_179(398).
	p(NT(397), (NT(398)));
//G606: cli_command(278)     => def_output_cmd(397).
	p(NT(278), (NT(397)));
//G607: __E___E_cli_command_180_181(403) => 'h' 'i' 's' 't'.
	p(NT(403), (T(56)+T(10)+T(12)+T(13)));
//G608: __E___E_cli_command_180_181(403) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(403), (T(56)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G609: history_sym(402)     => __E___E_cli_command_180_181(403).
	p(NT(402), (NT(403)));
//G610: __E_cli_command_180(401) => history_sym(402).
	p(NT(401), (NT(402)));
//G611: history_list_cmd(400) => __E_cli_command_180(401).
	p(NT(400), (NT(401)));
//G612: cli_command(278)     => history_list_cmd(400).
	p(NT(278), (NT(400)));
//G613: __E_cli_command_182(405) => history_sym(402) __(27) memory(324).
	p(NT(405), (NT(402)+NT(27)+NT(324)));
//G614: history_print_cmd(404) => __E_cli_command_182(405).
	p(NT(404), (NT(405)));
//G615: cli_command(278)     => history_print_cmd(404).
	p(NT(278), (NT(404)));
//G616: __E_cli_command_183(407) => wff(14).
	p(NT(407), (NT(14)));
//G617: __E_cli_command_183(407) => bf(21).
	p(NT(407), (NT(21)));
//G618: history_store_cmd(406) => __E_cli_command_183(407).
	p(NT(406), (NT(407)));
//G619: cli_command(278)     => history_store_cmd(406).
	p(NT(278), (NT(406)));
//G620: __E_solve_options_184(408) => __(27) solve_option(409).
	p(NT(408), (NT(27)+NT(409)));
//G621: __E_solve_options_185(410) => null.
	p(NT(410), (nul));
//G622: __E_solve_options_185(410) => __E_solve_options_184(408) __E_solve_options_185(410).
	p(NT(410), (NT(408)+NT(410)));
//G623: solve_options(314)   => __E_solve_options_185(410).
	p(NT(314), (NT(410)));
//G624: __E_solve_option_186(411) => '-' '-' solver_mode(412).
	p(NT(411), (T(28)+T(28)+NT(412)));
//G625: solve_option(409)    => __E_solve_option_186(411).
	p(NT(409), (NT(411)));
//G626: __E_solve_option_187(413) => '-' '-' type(150).
	p(NT(413), (T(28)+T(28)+NT(150)));
//G627: solve_option(409)    => __E_solve_option_187(413).
	p(NT(409), (NT(413)));
//G628: __E_solver_mode_188(415) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(415), (T(18)+T(10)+T(55)+T(10)+T(18)+T(45)+T(18)));
//G629: __E_solver_mode_188(415) => 'm' 'i' 'n'.
	p(NT(415), (T(18)+T(10)+T(55)));
//G630: solver_mode_minimum(414) => __E_solver_mode_188(415).
	p(NT(414), (NT(415)));
//G631: solver_mode(412)     => solver_mode_minimum(414).
	p(NT(412), (NT(414)));
//G632: __E_solver_mode_189(417) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(417), (T(18)+T(5)+T(27)+T(10)+T(18)+T(45)+T(18)));
//G633: __E_solver_mode_189(417) => 'm' 'a' 'x'.
	p(NT(417), (T(18)+T(5)+T(27)));
//G634: solver_mode_maximum(416) => __E_solver_mode_189(417).
	p(NT(416), (NT(417)));
//G635: solver_mode(412)     => solver_mode_maximum(416).
	p(NT(412), (NT(416)));
//G636: __E_input_stream_190(418) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(419) _(11) ')'.
	p(NT(418), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(419)+NT(11)+T(16)));
//G637: input_stream(396)    => __E_input_stream_190(418).
	p(NT(396), (NT(418)));
//G638: console_sym(420)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(420), (T(8)+T(17)+T(55)+T(12)+T(17)+T(6)+T(19)));
//G639: input_stream(396)    => console_sym(420).
	p(NT(396), (NT(420)));
//G640: __E_output_stream_191(421) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(419) _(11) ')'.
	p(NT(421), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(419)+NT(11)+T(16)));
//G641: output_stream(399)   => __E_output_stream_191(421).
	p(NT(399), (NT(421)));
//G642: output_stream(399)   => console_sym(420).
	p(NT(399), (NT(420)));
//G643: q_file_name(419)     => '"' file_name(422) '"'.
	p(NT(419), (T(61)+NT(422)+T(61)));
//G644: __E_file_name_192(423) => printable(8).
	p(NT(423), (NT(8)));
//G645: __E_file_name_192(423) => printable(8) __E_file_name_192(423).
	p(NT(423), (NT(8)+NT(423)));
//G646: file_name(422)       => __E_file_name_192(423).
	p(NT(422), (NT(423)));
//G647: __E_option_name_193(424) => alnum(6).
	p(NT(424), (NT(6)));
//G648: __E_option_name_193(424) => alnum(6) __E_option_name_193(424).
	p(NT(424), (NT(6)+NT(424)));
//G649: option_name(379)     => __E_option_name_193(424).
	p(NT(379), (NT(424)));
//G650: __E_option_value_194(425) => alnum(6).
	p(NT(425), (NT(6)));
//G651: __E_option_value_194(425) => alnum(6) __E_option_value_194(425).
	p(NT(425), (NT(6)+NT(425)));
//G652: option_value(384)    => __E_option_value_194(425).
	p(NT(384), (NT(425)));
//G653: number(370)          => digits(251).
	p(NT(370), (NT(251)));
//G654: bf_cmd_arg(426)      => memory(324).
	p(NT(426), (NT(324)));
//G655: bf_cmd_arg(426)      => bf(21).
	p(NT(426), (NT(21)));
//G656: wff_cmd_arg(315)     => memory(324).
	p(NT(315), (NT(324)));
//G657: wff_cmd_arg(315)     => wff(14).
	p(NT(315), (NT(14)));
//G658: nf_cmd_arg(333)      => memory(324).
	p(NT(333), (NT(324)));
//G659: nf_cmd_arg(333)      => ref(18).
	p(NT(333), (NT(18)));
//G660: nf_cmd_arg(333)      => wff(14).
	p(NT(333), (NT(14)));
//G661: nf_cmd_arg(333)      => bf(21).
	p(NT(333), (NT(21)));
//G662: onf_cmd_arg(363)     => memory(324).
	p(NT(363), (NT(324)));
//G663: onf_cmd_arg(363)     => wff(14).
	p(NT(363), (NT(14)));
//G664: normalize_cmd_arg(303) => memory(324).
	p(NT(303), (NT(324)));
//G665: normalize_cmd_arg(303) => rr(10).
	p(NT(303), (NT(10)));
//G666: normalize_cmd_arg(303) => ref(18).
	p(NT(303), (NT(18)));
//G667: normalize_cmd_arg(303) => wff(14).
	p(NT(303), (NT(14)));
//G668: normalize_cmd_arg(303) => bf(21).
	p(NT(303), (NT(21)));
//G669: inst_args(338)       => wff_cmd_arg(315) _(11) '[' _(11) variable(139) _(11) '/' _(11) bf_cmd_arg(426) _(11) ']'.
	p(NT(338), (NT(315)+NT(11)+T(24)+NT(11)+NT(139)+NT(11)+T(43)+NT(11)+NT(426)+NT(11)+T(25)));
//G670: inst_args(338)       => bf_cmd_arg(426) _(11) '[' _(11) variable(139) _(11) '/' _(11) bf_cmd_arg(426) _(11) ']'.
	p(NT(338), (NT(426)+NT(11)+T(24)+NT(11)+NT(139)+NT(11)+T(43)+NT(11)+NT(426)+NT(11)+T(25)));
//G671: help_arg(295)        => help_sym(292).
	p(NT(295), (NT(292)));
//G672: help_arg(295)        => version_sym(285).
	p(NT(295), (NT(285)));
//G673: help_arg(295)        => quit_sym(282).
	p(NT(295), (NT(282)));
//G674: help_arg(295)        => clear_sym(288).
	p(NT(295), (NT(288)));
//G675: help_arg(295)        => get_sym(377).
	p(NT(295), (NT(377)));
//G676: help_arg(295)        => set_sym(382).
	p(NT(295), (NT(382)));
//G677: help_arg(295)        => enable_sym(387).
	p(NT(295), (NT(387)));
//G678: help_arg(295)        => disable_sym(390).
	p(NT(295), (NT(390)));
//G679: help_arg(295)        => toggle_sym(393).
	p(NT(295), (NT(393)));
//G680: help_arg(295)        => file_sym(298).
	p(NT(295), (NT(298)));
//G681: help_arg(295)        => history_sym(402).
	p(NT(295), (NT(402)));
//G682: help_arg(295)        => abs_memory_sym(427).
	p(NT(295), (NT(427)));
//G683: help_arg(295)        => rel_memory_sym(428).
	p(NT(295), (NT(428)));
//G684: help_arg(295)        => selection_sym(429).
	p(NT(295), (NT(429)));
//G685: help_arg(295)        => def_sym(366).
	p(NT(295), (NT(366)));
//G686: help_arg(295)        => inst_sym(336).
	p(NT(295), (NT(336)));
//G687: help_arg(295)        => subst_sym(331).
	p(NT(295), (NT(331)));
//G688: help_arg(295)        => normalize_sym(327).
	p(NT(295), (NT(327)));
//G689: help_arg(295)        => execute_sym(430).
	p(NT(295), (NT(430)));
//G690: help_arg(295)        => solve_sym(312).
	p(NT(295), (NT(312)));
//G691: help_arg(295)        => valid_sym(302).
	p(NT(295), (NT(302)));
//G692: help_arg(295)        => sat_sym(306).
	p(NT(295), (NT(306)));
//G693: help_arg(295)        => unsat_sym(309).
	p(NT(295), (NT(309)));
//G694: help_arg(295)        => run_sym(321).
	p(NT(295), (NT(321)));
//G695: help_arg(295)        => dnf_sym(341).
	p(NT(295), (NT(341)));
//G696: help_arg(295)        => cnf_sym(344).
	p(NT(295), (NT(344)));
//G697: help_arg(295)        => anf_sym(347).
	p(NT(295), (NT(347)));
//G698: help_arg(295)        => snf_sym(359).
	p(NT(295), (NT(359)));
//G699: help_arg(295)        => nnf_sym(350).
	p(NT(295), (NT(350)));
//G700: help_arg(295)        => pnf_sym(353).
	p(NT(295), (NT(353)));
//G701: help_arg(295)        => mnf_sym(356).
	p(NT(295), (NT(356)));
//G702: help_arg(295)        => onf_sym(362).
	p(NT(295), (NT(362)));
//G703: help_arg(295)        => qelim_sym(374).
	p(NT(295), (NT(374)));
//G704: __E___E_help_arg_195_196(433) => 's'.
	p(NT(433), (T(12)));
//G705: __E___E_help_arg_195_196(433) => null.
	p(NT(433), (nul));
//G706: __E_help_arg_195(432) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_195_196(433).
	p(NT(432), (T(19)+T(27)+T(5)+T(18)+T(57)+T(6)+T(19)+NT(433)));
//G707: examples_sym(431)    => __E_help_arg_195(432).
	p(NT(431), (NT(432)));
//G708: help_arg(295)        => examples_sym(431).
	p(NT(295), (NT(431)));
//G709: __E_memory_197(435)  => '%'.
	p(NT(435), (T(44)));
//G710: rel_memory_sym(428)  => '%' '-'.
	p(NT(428), (T(44)+T(28)));
//G711: memory_id(437)       => digits(251).
	p(NT(437), (NT(251)));
//G712: __E___E_memory_197_198(436) => memory_id(437).
	p(NT(436), (NT(437)));
//G713: __E___E_memory_197_198(436) => null.
	p(NT(436), (nul));
//G714: __E_memory_197(435)  => rel_memory_sym(428) __E___E_memory_197_198(436).
	p(NT(435), (NT(428)+NT(436)));
//G715: rel_memory(434)      => __E_memory_197(435).
	p(NT(434), (NT(435)));
//G716: memory(324)          => rel_memory(434).
	p(NT(324), (NT(434)));
//G717: abs_memory_sym(427)  => '%'.
	p(NT(427), (T(44)));
//G718: __E_memory_199(439)  => abs_memory_sym(427) memory_id(437).
	p(NT(439), (NT(427)+NT(437)));
//G719: abs_memory(438)      => __E_memory_199(439).
	p(NT(438), (NT(439)));
//G720: memory(324)          => abs_memory(438).
	p(NT(324), (NT(438)));
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
		__E_constraint_96, ctn_less, __E_constraint_97, __E_variable_98, __E_variable_99, __E_variable_100, __E_variable_101, __E_variable_102, io_var, __E_variable_103, 
		in, out, uninterpreted_constant, in_var_name, __E_in_104, __E___E_in_104_105, offset, out_var_name, __E_out_106, __E___E_out_106_107, 
		__E___E___E_out_106_107_108, chars, q_var, __E_q_vars_109, __E_q_vars_110, __E_offsets_111, __E_offsets_112, shift, __E_offset_113, __E_shift_114, 
		__E___E_shift_114_115, digits, __E_integer_116, uninter_const_name, __E_uninterpreted_constant_117, __E___E_uninterpreted_constant_117_118, constant, __E_bf_constant_119, binding, __E_bf_constant_120, 
		source, __E_binding_121, src_c, __E___E_binding_121_122, __E___E___E_binding_121_122_123, __E___E___E_binding_121_122_124, __E_src_c_125, __E_src_c_126, __E_chars_127, __E_chars_128, 
		__E_digits_129, comment, __E_comment_130, __E_comment_131, __E_comment_132, __E____133, __E___134, cli, cli_command, __E_cli_135, 
		__E_cli_136, quit_cmd, quit_sym, __E_cli_command_137, version_cmd, version_sym, __E_cli_command_138, clear_cmd, clear_sym, __E_cli_command_139, 
		help_cmd, __E_cli_command_140, help_sym, __E___E_cli_command_140_141, __E___E_cli_command_140_142, help_arg, file_cmd, __E_cli_command_143, file_sym, q_string, 
		valid_cmd, __E_cli_command_144, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_145, sat_sym, unsat_cmd, __E_cli_command_146, unsat_sym, 
		solve_cmd, __E_cli_command_147, solve_sym, __E___E_cli_command_147_148, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_149, lgrs_sym, run_cmd, 
		__E_cli_command_150, run_sym, __E___E_cli_command_150_151, __E___E_cli_command_150_152, memory, normalize_cmd, __E_cli_command_153, normalize_sym, __E___E_cli_command_153_154, subst_cmd, 
		__E_cli_command_155, subst_sym, __E___E_cli_command_155_156, nf_cmd_arg, inst_cmd, __E_cli_command_157, inst_sym, __E___E_cli_command_157_158, inst_args, dnf_cmd, 
		__E_cli_command_159, dnf_sym, cnf_cmd, __E_cli_command_160, cnf_sym, anf_cmd, __E_cli_command_161, anf_sym, nnf_cmd, __E_cli_command_162, 
		nnf_sym, pnf_cmd, __E_cli_command_163, pnf_sym, mnf_cmd, __E_cli_command_164, mnf_sym, snf_cmd, __E_cli_command_165, snf_sym, 
		onf_cmd, __E_cli_command_166, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_167, def_sym, __E___E_cli_command_167_168, def_print_cmd, __E_cli_command_169, 
		number, def_rr_cmd, qelim_cmd, __E_cli_command_170, qelim_sym, get_cmd, __E_cli_command_171, get_sym, __E___E_cli_command_171_172, option_name, 
		set_cmd, __E_cli_command_173, set_sym, __E___E_cli_command_173_174, option_value, enable_cmd, __E_cli_command_175, enable_sym, disable_cmd, __E_cli_command_176, 
		disable_sym, toggle_cmd, __E_cli_command_177, toggle_sym, def_input_cmd, __E_cli_command_178, input_stream, def_output_cmd, __E_cli_command_179, output_stream, 
		history_list_cmd, __E_cli_command_180, history_sym, __E___E_cli_command_180_181, history_print_cmd, __E_cli_command_182, history_store_cmd, __E_cli_command_183, __E_solve_options_184, solve_option, 
		__E_solve_options_185, __E_solve_option_186, solver_mode, __E_solve_option_187, solver_mode_minimum, __E_solver_mode_188, solver_mode_maximum, __E_solver_mode_189, __E_input_stream_190, q_file_name, 
		console_sym, __E_output_stream_191, file_name, __E_file_name_192, __E_option_name_193, __E_option_value_194, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, 
		execute_sym, examples_sym, __E_help_arg_195, __E___E_help_arg_195_196, rel_memory, __E_memory_197, __E___E_memory_197_198, memory_id, abs_memory, __E_memory_199, 
		named_binding, z3_bv, z3_bv_parenthesis, __N_0, __N_1, __N_2, __N_3, __N_4, __N_5, __N_6, 
		__N_7, __N_8, 
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
