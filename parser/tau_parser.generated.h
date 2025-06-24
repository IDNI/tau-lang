// This file is generated from a file /home/castrod/git/tau-lang/parser/tau.tgf by
//       https://github.com/IDNI/parser/src/tgf
//
#ifndef __TAU_PARSER_H__
#define __TAU_PARSER_H__

#include "parser.h"

namespace tau_parser_data {

using char_type     = char;
using terminal_type = char;

inline static constexpr size_t nt_bits = 9;
inline const std::vector<std::string> symbol_names{
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
	"src_c", "__E___E_binding_125_126", "__E___E___E_binding_125_126_127", "__E___E___E_binding_125_126_128", "__E_src_c_129", "__E_src_c_130", "__E_chars_131", "__E_chars_132", "__E_digits_133", "comment", 
	"__E_comment_134", "__E_comment_135", "__E_comment_136", "__E____137", "__E___138", "cli", "cli_command", "__E_cli_139", "__E_cli_140", "quit_cmd", 
	"quit_sym", "__E_cli_command_141", "version_cmd", "version_sym", "__E_cli_command_142", "clear_cmd", "clear_sym", "__E_cli_command_143", "help_cmd", "__E_cli_command_144", 
	"help_sym", "__E___E_cli_command_144_145", "__E___E_cli_command_144_146", "help_arg", "file_cmd", "__E_cli_command_147", "file_sym", "q_string", "valid_cmd", "__E_cli_command_148", 
	"valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_149", "sat_sym", "unsat_cmd", "__E_cli_command_150", "unsat_sym", "solve_cmd", "__E_cli_command_151", 
	"solve_sym", "__E___E_cli_command_151_152", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_153", "lgrs_sym", "run_cmd", "__E_cli_command_154", "run_sym", 
	"__E___E_cli_command_154_155", "__E___E_cli_command_154_156", "memory", "normalize_cmd", "__E_cli_command_157", "normalize_sym", "__E___E_cli_command_157_158", "subst_cmd", "__E_cli_command_159", "subst_sym", 
	"__E___E_cli_command_159_160", "nf_cmd_arg", "inst_cmd", "__E_cli_command_161", "inst_sym", "__E___E_cli_command_161_162", "inst_args", "dnf_cmd", "__E_cli_command_163", "dnf_sym", 
	"cnf_cmd", "__E_cli_command_164", "cnf_sym", "anf_cmd", "__E_cli_command_165", "anf_sym", "nnf_cmd", "__E_cli_command_166", "nnf_sym", "pnf_cmd", 
	"__E_cli_command_167", "pnf_sym", "mnf_cmd", "__E_cli_command_168", "mnf_sym", "snf_cmd", "__E_cli_command_169", "snf_sym", "onf_cmd", "__E_cli_command_170", 
	"onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_171", "def_sym", "__E___E_cli_command_171_172", "def_print_cmd", "__E_cli_command_173", "number", "def_rr_cmd", 
	"qelim_cmd", "__E_cli_command_174", "qelim_sym", "get_cmd", "__E_cli_command_175", "get_sym", "__E___E_cli_command_175_176", "option_name", "set_cmd", "__E_cli_command_177", 
	"set_sym", "__E___E_cli_command_177_178", "option_value", "enable_cmd", "__E_cli_command_179", "enable_sym", "disable_cmd", "__E_cli_command_180", "disable_sym", "toggle_cmd", 
	"__E_cli_command_181", "toggle_sym", "def_input_cmd", "__E_cli_command_182", "input_stream", "def_output_cmd", "__E_cli_command_183", "output_stream", "history_list_cmd", "__E_cli_command_184", 
	"history_sym", "__E___E_cli_command_184_185", "history_print_cmd", "__E_cli_command_186", "history_store_cmd", "__E_cli_command_187", "__E_solve_options_188", "solve_option", "__E_solve_options_189", "__E_solve_option_190", 
	"solver_mode", "__E_solve_option_191", "solver_mode_minimum", "__E_solver_mode_192", "solver_mode_maximum", "__E_solver_mode_193", "__E_input_stream_194", "q_file_name", "console_sym", "__E_output_stream_195", 
	"file_name", "__E_file_name_196", "__E_option_name_197", "__E_option_value_198", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "execute_sym", "examples_sym", 
	"__E_help_arg_199", "__E___E_help_arg_199_200", "rel_memory", "__E_memory_201", "__E___E_memory_201_202", "memory_id", "abs_memory", "__E_memory_203", "named_binding", "__N_0", 
	"__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
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
			239, 243, 254, 259, 261, 268, 387, 392, 430, 448
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
			{ 311 },
			{ 323 },
			{ 341 },
			{ 346 },
			{ 371 },
			{ 434 }
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
//G336: __N_0(449)           => 'F'.
	p(NT(449), (T(34)));
//G337: __N_1(450)           => 'T'.
	p(NT(450), (T(33)));
//G338: __E_variable_98(226) => ~( __N_0(449) ) & ~( __N_1(450) ) & alpha(5).	 # conjunctive
	p(NT(226), ~(NT(449)) & ~(NT(450)) & (NT(5)));
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
//G344: __N_2(451)           => 'F'.
	p(NT(451), (T(34)));
//G345: __N_3(452)           => 'T'.
	p(NT(452), (T(33)));
//G346: __E_variable_101(229) => ~( __N_2(451) ) & ~( __N_3(452) ) & alpha(5).	 # conjunctive
	p(NT(229), ~(NT(451)) & ~(NT(452)) & (NT(5)));
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
//G380: __N_4(453)           => uninterpreted_constant(238).
	p(NT(453), (NT(238)));
//G381: __E_q_var_114(251)   => variable(139) & ~( __N_4(453) ).	 # conjunctive
	p(NT(251), (NT(139)) & ~(NT(453)));
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
//G390: __N_5(454)           => io_var(233).
	p(NT(454), (NT(233)));
//G391: __E_offset_117(256)  => variable(139) & ~( __N_5(454) ).	 # conjunctive
	p(NT(256), (NT(139)) & ~(NT(454)));
//G392: offset(242)          => __E_offset_117(256).
	p(NT(242), (NT(256)));
//G393: __E_shift_118(257)   => capture(20).
	p(NT(257), (NT(20)));
//G394: __N_6(455)           => io_var(233).
	p(NT(455), (NT(233)));
//G395: __E___E_shift_118_119(258) => variable(139) & ~( __N_6(455) ).	 # conjunctive
	p(NT(258), (NT(139)) & ~(NT(455)));
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
//G423: __N_7(456)           => '{'.
	p(NT(456), (T(49)));
//G424: __N_8(457)           => '}'.
	p(NT(457), (T(50)));
//G425: src_c(270)           => ~( __N_7(456) ) & ~( __N_8(457) ) & punct(7).	 # conjunctive
	p(NT(270), ~(NT(456)) & ~(NT(457)) & (NT(7)));
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
//G433: bv_type(206)         => 'b' 'v' _(11) '[' _(11) num(196) _(11) ']'.
	p(NT(206), (T(7)+T(51)+NT(11)+T(24)+NT(11)+NT(196)+NT(11)+T(25)));
//G434: __E_chars_131(276)   => alnum(6).
	p(NT(276), (NT(6)));
//G435: __E_chars_131(276)   => '_'.
	p(NT(276), (T(35)));
//G436: __E_chars_132(277)   => null.
	p(NT(277), (nul));
//G437: __E_chars_132(277)   => __E_chars_131(276) __E_chars_132(277).
	p(NT(277), (NT(276)+NT(277)));
//G438: chars(247)           => alpha(5) __E_chars_132(277).
	p(NT(247), (NT(5)+NT(277)));
//G439: __E_digits_133(278)  => digit(3).
	p(NT(278), (NT(3)));
//G440: __E_digits_133(278)  => digit(3) __E_digits_133(278).
	p(NT(278), (NT(3)+NT(278)));
//G441: digits(259)          => __E_digits_133(278).
	p(NT(259), (NT(278)));
//G442: sym(22)              => chars(247).
	p(NT(22), (NT(247)));
//G443: __E_comment_134(280) => printable(8).
	p(NT(280), (NT(8)));
//G444: __E_comment_134(280) => '\t'.
	p(NT(280), (T(52)));
//G445: __E_comment_135(281) => null.
	p(NT(281), (nul));
//G446: __E_comment_135(281) => __E_comment_134(280) __E_comment_135(281).
	p(NT(281), (NT(280)+NT(281)));
//G447: __E_comment_136(282) => '\n'.
	p(NT(282), (T(53)));
//G448: __E_comment_136(282) => '\r'.
	p(NT(282), (T(54)));
//G449: __E_comment_136(282) => eof(1).
	p(NT(282), (NT(1)));
//G450: comment(279)         => '#' __E_comment_135(281) __E_comment_136(282).
	p(NT(279), (T(46)+NT(281)+NT(282)));
//G451: __E____137(283)      => space(2).
	p(NT(283), (NT(2)));
//G452: __E____137(283)      => comment(279).
	p(NT(283), (NT(279)));
//G453: __(27)               => __E____137(283) _(11).
	p(NT(27), (NT(283)+NT(11)));
//G454: __E___138(284)       => __(27).
	p(NT(284), (NT(27)));
//G455: __E___138(284)       => null.
	p(NT(284), (nul));
//G456: _(11)                => __E___138(284).
	p(NT(11), (NT(284)));
//G457: cli(285)             => _(11).
	p(NT(285), (NT(11)));
//G458: __E_cli_139(287)     => '.' _(11) cli_command(286) _(11).
	p(NT(287), (T(1)+NT(11)+NT(286)+NT(11)));
//G459: __E_cli_140(288)     => null.
	p(NT(288), (nul));
//G460: __E_cli_140(288)     => __E_cli_139(287) __E_cli_140(288).
	p(NT(288), (NT(287)+NT(288)));
//G461: cli(285)             => _(11) cli_command(286) _(11) __E_cli_140(288).
	p(NT(285), (NT(11)+NT(286)+NT(11)+NT(288)));
//G462: __E_cli_command_141(291) => 'q'.
	p(NT(291), (T(55)));
//G463: __E_cli_command_141(291) => 'q' 'u' 'i' 't'.
	p(NT(291), (T(55)+T(47)+T(10)+T(13)));
//G464: quit_sym(290)        => __E_cli_command_141(291).
	p(NT(290), (NT(291)));
//G465: quit_cmd(289)        => quit_sym(290).
	p(NT(289), (NT(290)));
//G466: cli_command(286)     => quit_cmd(289).
	p(NT(286), (NT(289)));
//G467: __E_cli_command_142(294) => 'v'.
	p(NT(294), (T(51)));
//G468: __E_cli_command_142(294) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(294), (T(51)+T(19)+T(11)+T(12)+T(10)+T(17)+T(45)));
//G469: version_sym(293)     => __E_cli_command_142(294).
	p(NT(293), (NT(294)));
//G470: version_cmd(292)     => version_sym(293).
	p(NT(292), (NT(293)));
//G471: cli_command(286)     => version_cmd(292).
	p(NT(286), (NT(292)));
//G472: __E_cli_command_143(297) => 'c'.
	p(NT(297), (T(8)));
//G473: __E_cli_command_143(297) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(297), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G474: clear_sym(296)       => __E_cli_command_143(297).
	p(NT(296), (NT(297)));
//G475: clear_cmd(295)       => clear_sym(296).
	p(NT(295), (NT(296)));
//G476: cli_command(286)     => clear_cmd(295).
	p(NT(286), (NT(295)));
//G477: __E___E_cli_command_144_145(301) => 'h'.
	p(NT(301), (T(56)));
//G478: __E___E_cli_command_144_145(301) => 'h' 'e' 'l' 'p'.
	p(NT(301), (T(56)+T(19)+T(6)+T(57)));
//G479: help_sym(300)        => __E___E_cli_command_144_145(301).
	p(NT(300), (NT(301)));
//G480: __E___E_cli_command_144_146(302) => __(27) help_arg(303).
	p(NT(302), (NT(27)+NT(303)));
//G481: __E___E_cli_command_144_146(302) => null.
	p(NT(302), (nul));
//G482: __E_cli_command_144(299) => help_sym(300) __E___E_cli_command_144_146(302).
	p(NT(299), (NT(300)+NT(302)));
//G483: help_cmd(298)        => __E_cli_command_144(299).
	p(NT(298), (NT(299)));
//G484: cli_command(286)     => help_cmd(298).
	p(NT(286), (NT(298)));
//G485: file_sym(306)        => 'f' 'i' 'l' 'e'.
	p(NT(306), (T(4)+T(10)+T(6)+T(19)));
//G486: __E_cli_command_147(305) => file_sym(306) __(27) q_string(307).
	p(NT(305), (NT(306)+NT(27)+NT(307)));
//G487: file_cmd(304)        => __E_cli_command_147(305).
	p(NT(304), (NT(305)));
//G488: cli_command(286)     => file_cmd(304).
	p(NT(286), (NT(304)));
//G489: valid_sym(310)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(310), (T(51)+T(5)+T(6)+T(10)+T(58)));
//G490: __E_cli_command_148(309) => valid_sym(310) __(27) normalize_cmd_arg(311).
	p(NT(309), (NT(310)+NT(27)+NT(311)));
//G491: valid_cmd(308)       => __E_cli_command_148(309).
	p(NT(308), (NT(309)));
//G492: cli_command(286)     => valid_cmd(308).
	p(NT(286), (NT(308)));
//G493: sat_sym(314)         => 's' 'a' 't'.
	p(NT(314), (T(12)+T(5)+T(13)));
//G494: __E_cli_command_149(313) => sat_sym(314) __(27) normalize_cmd_arg(311).
	p(NT(313), (NT(314)+NT(27)+NT(311)));
//G495: sat_cmd(312)         => __E_cli_command_149(313).
	p(NT(312), (NT(313)));
//G496: cli_command(286)     => sat_cmd(312).
	p(NT(286), (NT(312)));
//G497: unsat_sym(317)       => 'u' 'n' 's' 'a' 't'.
	p(NT(317), (T(47)+T(45)+T(12)+T(5)+T(13)));
//G498: __E_cli_command_150(316) => unsat_sym(317) __(27) normalize_cmd_arg(311).
	p(NT(316), (NT(317)+NT(27)+NT(311)));
//G499: unsat_cmd(315)       => __E_cli_command_150(316).
	p(NT(315), (NT(316)));
//G500: cli_command(286)     => unsat_cmd(315).
	p(NT(286), (NT(315)));
//G501: solve_sym(320)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(320), (T(12)+T(17)+T(6)+T(51)+T(19)));
//G502: __E___E_cli_command_151_152(321) => solve_options(322).
	p(NT(321), (NT(322)));
//G503: __E___E_cli_command_151_152(321) => null.
	p(NT(321), (nul));
//G504: __E_cli_command_151(319) => solve_sym(320) __E___E_cli_command_151_152(321) __(27) wff_cmd_arg(323).
	p(NT(319), (NT(320)+NT(321)+NT(27)+NT(323)));
//G505: solve_cmd(318)       => __E_cli_command_151(319).
	p(NT(318), (NT(319)));
//G506: cli_command(286)     => solve_cmd(318).
	p(NT(286), (NT(318)));
//G507: lgrs_sym(326)        => 'l' 'g' 'r' 's'.
	p(NT(326), (T(6)+T(59)+T(11)+T(12)));
//G508: __E_cli_command_153(325) => lgrs_sym(326) __(27) wff_cmd_arg(323).
	p(NT(325), (NT(326)+NT(27)+NT(323)));
//G509: lgrs_cmd(324)        => __E_cli_command_153(325).
	p(NT(324), (NT(325)));
//G510: cli_command(286)     => lgrs_cmd(324).
	p(NT(286), (NT(324)));
//G511: __E___E_cli_command_154_155(330) => 'r'.
	p(NT(330), (T(11)));
//G512: __E___E_cli_command_154_155(330) => 'r' 'u' 'n'.
	p(NT(330), (T(11)+T(47)+T(45)));
//G513: run_sym(329)         => __E___E_cli_command_154_155(330).
	p(NT(329), (NT(330)));
//G514: __E___E_cli_command_154_156(331) => memory(332).
	p(NT(331), (NT(332)));
//G515: __E___E_cli_command_154_156(331) => wff(14).
	p(NT(331), (NT(14)));
//G516: __E_cli_command_154(328) => run_sym(329) __(27) __E___E_cli_command_154_156(331).
	p(NT(328), (NT(329)+NT(27)+NT(331)));
//G517: run_cmd(327)         => __E_cli_command_154(328).
	p(NT(327), (NT(328)));
//G518: cli_command(286)     => run_cmd(327).
	p(NT(286), (NT(327)));
//G519: __E___E_cli_command_157_158(336) => 'n'.
	p(NT(336), (T(45)));
//G520: __E___E_cli_command_157_158(336) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(336), (T(45)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(60)+T(19)));
//G521: normalize_sym(335)   => __E___E_cli_command_157_158(336).
	p(NT(335), (NT(336)));
//G522: __E_cli_command_157(334) => normalize_sym(335) __(27) normalize_cmd_arg(311).
	p(NT(334), (NT(335)+NT(27)+NT(311)));
//G523: normalize_cmd(333)   => __E_cli_command_157(334).
	p(NT(333), (NT(334)));
//G524: cli_command(286)     => normalize_cmd(333).
	p(NT(286), (NT(333)));
//G525: __E___E_cli_command_159_160(340) => 's'.
	p(NT(340), (T(12)));
//G526: __E___E_cli_command_159_160(340) => 's' 'u' 'b' 's' 't'.
	p(NT(340), (T(12)+T(47)+T(7)+T(12)+T(13)));
//G527: __E___E_cli_command_159_160(340) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(340), (T(12)+T(47)+T(7)+T(12)+T(13)+T(10)+T(13)+T(47)+T(13)+T(19)));
//G528: subst_sym(339)       => __E___E_cli_command_159_160(340).
	p(NT(339), (NT(340)));
//G529: __E_cli_command_159(338) => subst_sym(339) __(27) nf_cmd_arg(341) _(11) '[' _(11) nf_cmd_arg(341) _(11) '/' _(11) nf_cmd_arg(341) _(11) ']'.
	p(NT(338), (NT(339)+NT(27)+NT(341)+NT(11)+T(24)+NT(11)+NT(341)+NT(11)+T(43)+NT(11)+NT(341)+NT(11)+T(25)));
//G530: subst_cmd(337)       => __E_cli_command_159(338).
	p(NT(337), (NT(338)));
//G531: cli_command(286)     => subst_cmd(337).
	p(NT(286), (NT(337)));
//G532: __E___E_cli_command_161_162(345) => 'i'.
	p(NT(345), (T(10)));
//G533: __E___E_cli_command_161_162(345) => 'i' 'n' 's' 't'.
	p(NT(345), (T(10)+T(45)+T(12)+T(13)));
//G534: __E___E_cli_command_161_162(345) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(345), (T(10)+T(45)+T(12)+T(13)+T(5)+T(45)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G535: inst_sym(344)        => __E___E_cli_command_161_162(345).
	p(NT(344), (NT(345)));
//G536: __E_cli_command_161(343) => inst_sym(344) __(27) inst_args(346).
	p(NT(343), (NT(344)+NT(27)+NT(346)));
//G537: inst_cmd(342)        => __E_cli_command_161(343).
	p(NT(342), (NT(343)));
//G538: cli_command(286)     => inst_cmd(342).
	p(NT(286), (NT(342)));
//G539: dnf_sym(349)         => 'd' 'n' 'f'.
	p(NT(349), (T(58)+T(45)+T(4)));
//G540: __E_cli_command_163(348) => dnf_sym(349) __(27) nf_cmd_arg(341).
	p(NT(348), (NT(349)+NT(27)+NT(341)));
//G541: dnf_cmd(347)         => __E_cli_command_163(348).
	p(NT(347), (NT(348)));
//G542: cli_command(286)     => dnf_cmd(347).
	p(NT(286), (NT(347)));
//G543: cnf_sym(352)         => 'c' 'n' 'f'.
	p(NT(352), (T(8)+T(45)+T(4)));
//G544: __E_cli_command_164(351) => cnf_sym(352) __(27) nf_cmd_arg(341).
	p(NT(351), (NT(352)+NT(27)+NT(341)));
//G545: cnf_cmd(350)         => __E_cli_command_164(351).
	p(NT(350), (NT(351)));
//G546: cli_command(286)     => cnf_cmd(350).
	p(NT(286), (NT(350)));
//G547: anf_sym(355)         => 'a' 'n' 'f'.
	p(NT(355), (T(5)+T(45)+T(4)));
//G548: __E_cli_command_165(354) => anf_sym(355) __(27) nf_cmd_arg(341).
	p(NT(354), (NT(355)+NT(27)+NT(341)));
//G549: anf_cmd(353)         => __E_cli_command_165(354).
	p(NT(353), (NT(354)));
//G550: cli_command(286)     => anf_cmd(353).
	p(NT(286), (NT(353)));
//G551: nnf_sym(358)         => 'n' 'n' 'f'.
	p(NT(358), (T(45)+T(45)+T(4)));
//G552: __E_cli_command_166(357) => nnf_sym(358) __(27) nf_cmd_arg(341).
	p(NT(357), (NT(358)+NT(27)+NT(341)));
//G553: nnf_cmd(356)         => __E_cli_command_166(357).
	p(NT(356), (NT(357)));
//G554: cli_command(286)     => nnf_cmd(356).
	p(NT(286), (NT(356)));
//G555: pnf_sym(361)         => 'p' 'n' 'f'.
	p(NT(361), (T(57)+T(45)+T(4)));
//G556: __E_cli_command_167(360) => pnf_sym(361) __(27) nf_cmd_arg(341).
	p(NT(360), (NT(361)+NT(27)+NT(341)));
//G557: pnf_cmd(359)         => __E_cli_command_167(360).
	p(NT(359), (NT(360)));
//G558: cli_command(286)     => pnf_cmd(359).
	p(NT(286), (NT(359)));
//G559: mnf_sym(364)         => 'm' 'n' 'f'.
	p(NT(364), (T(18)+T(45)+T(4)));
//G560: __E_cli_command_168(363) => mnf_sym(364) __(27) nf_cmd_arg(341).
	p(NT(363), (NT(364)+NT(27)+NT(341)));
//G561: mnf_cmd(362)         => __E_cli_command_168(363).
	p(NT(362), (NT(363)));
//G562: cli_command(286)     => mnf_cmd(362).
	p(NT(286), (NT(362)));
//G563: snf_sym(367)         => 's' 'n' 'f'.
	p(NT(367), (T(12)+T(45)+T(4)));
//G564: __E_cli_command_169(366) => snf_sym(367) __(27) nf_cmd_arg(341).
	p(NT(366), (NT(367)+NT(27)+NT(341)));
//G565: snf_cmd(365)         => __E_cli_command_169(366).
	p(NT(365), (NT(366)));
//G566: cli_command(286)     => snf_cmd(365).
	p(NT(286), (NT(365)));
//G567: onf_sym(370)         => 'o' 'n' 'f'.
	p(NT(370), (T(17)+T(45)+T(4)));
//G568: __E_cli_command_170(369) => onf_sym(370) __(27) variable(139) __(27) onf_cmd_arg(371).
	p(NT(369), (NT(370)+NT(27)+NT(139)+NT(27)+NT(371)));
//G569: onf_cmd(368)         => __E_cli_command_170(369).
	p(NT(368), (NT(369)));
//G570: cli_command(286)     => onf_cmd(368).
	p(NT(286), (NT(368)));
//G571: __E___E_cli_command_171_172(375) => 'd' 'e' 'f' 's'.
	p(NT(375), (T(58)+T(19)+T(4)+T(12)));
//G572: __E___E_cli_command_171_172(375) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(375), (T(58)+T(19)+T(4)+T(10)+T(45)+T(10)+T(13)+T(10)+T(17)+T(45)+T(12)));
//G573: def_sym(374)         => __E___E_cli_command_171_172(375).
	p(NT(374), (NT(375)));
//G574: __E_cli_command_171(373) => def_sym(374).
	p(NT(373), (NT(374)));
//G575: def_list_cmd(372)    => __E_cli_command_171(373).
	p(NT(372), (NT(373)));
//G576: cli_command(286)     => def_list_cmd(372).
	p(NT(286), (NT(372)));
//G577: __E_cli_command_173(377) => def_sym(374) __(27) number(378).
	p(NT(377), (NT(374)+NT(27)+NT(378)));
//G578: def_print_cmd(376)   => __E_cli_command_173(377).
	p(NT(376), (NT(377)));
//G579: cli_command(286)     => def_print_cmd(376).
	p(NT(286), (NT(376)));
//G580: def_rr_cmd(379)      => rec_relation(16).
	p(NT(379), (NT(16)));
//G581: cli_command(286)     => def_rr_cmd(379).
	p(NT(286), (NT(379)));
//G582: qelim_sym(382)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(382), (T(55)+T(19)+T(6)+T(10)+T(18)));
//G583: __E_cli_command_174(381) => qelim_sym(382) __(27) wff_cmd_arg(323).
	p(NT(381), (NT(382)+NT(27)+NT(323)));
//G584: qelim_cmd(380)       => __E_cli_command_174(381).
	p(NT(380), (NT(381)));
//G585: cli_command(286)     => qelim_cmd(380).
	p(NT(286), (NT(380)));
//G586: get_sym(385)         => 'g' 'e' 't'.
	p(NT(385), (T(59)+T(19)+T(13)));
//G587: __E___E_cli_command_175_176(386) => __(27) option_name(387).
	p(NT(386), (NT(27)+NT(387)));
//G588: __E___E_cli_command_175_176(386) => null.
	p(NT(386), (nul));
//G589: __E_cli_command_175(384) => get_sym(385) __E___E_cli_command_175_176(386).
	p(NT(384), (NT(385)+NT(386)));
//G590: get_cmd(383)         => __E_cli_command_175(384).
	p(NT(383), (NT(384)));
//G591: cli_command(286)     => get_cmd(383).
	p(NT(286), (NT(383)));
//G592: set_sym(390)         => 's' 'e' 't'.
	p(NT(390), (T(12)+T(19)+T(13)));
//G593: __E___E_cli_command_177_178(391) => __(27).
	p(NT(391), (NT(27)));
//G594: __E___E_cli_command_177_178(391) => _(11) '=' _(11).
	p(NT(391), (NT(11)+T(3)+NT(11)));
//G595: __E_cli_command_177(389) => set_sym(390) __(27) option_name(387) __E___E_cli_command_177_178(391) option_value(392).
	p(NT(389), (NT(390)+NT(27)+NT(387)+NT(391)+NT(392)));
//G596: set_cmd(388)         => __E_cli_command_177(389).
	p(NT(388), (NT(389)));
//G597: cli_command(286)     => set_cmd(388).
	p(NT(286), (NT(388)));
//G598: enable_sym(395)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(395), (T(19)+T(45)+T(5)+T(7)+T(6)+T(19)));
//G599: __E_cli_command_179(394) => enable_sym(395) __(27) option_name(387).
	p(NT(394), (NT(395)+NT(27)+NT(387)));
//G600: enable_cmd(393)      => __E_cli_command_179(394).
	p(NT(393), (NT(394)));
//G601: cli_command(286)     => enable_cmd(393).
	p(NT(286), (NT(393)));
//G602: disable_sym(398)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(398), (T(58)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G603: __E_cli_command_180(397) => disable_sym(398) __(27) option_name(387).
	p(NT(397), (NT(398)+NT(27)+NT(387)));
//G604: disable_cmd(396)     => __E_cli_command_180(397).
	p(NT(396), (NT(397)));
//G605: cli_command(286)     => disable_cmd(396).
	p(NT(286), (NT(396)));
//G606: toggle_sym(401)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(401), (T(13)+T(17)+T(59)+T(59)+T(6)+T(19)));
//G607: __E_cli_command_181(400) => toggle_sym(401) __(27) option_name(387).
	p(NT(400), (NT(401)+NT(27)+NT(387)));
//G608: toggle_cmd(399)      => __E_cli_command_181(400).
	p(NT(399), (NT(400)));
//G609: cli_command(286)     => toggle_cmd(399).
	p(NT(286), (NT(399)));
//G610: __E_cli_command_182(403) => type(150) __(27) in_var_name(239) _(11) '=' _(11) input_stream(404).
	p(NT(403), (NT(150)+NT(27)+NT(239)+NT(11)+T(3)+NT(11)+NT(404)));
//G611: def_input_cmd(402)   => __E_cli_command_182(403).
	p(NT(402), (NT(403)));
//G612: cli_command(286)     => def_input_cmd(402).
	p(NT(286), (NT(402)));
//G613: __E_cli_command_183(406) => type(150) __(27) out_var_name(243) _(11) '=' _(11) output_stream(407).
	p(NT(406), (NT(150)+NT(27)+NT(243)+NT(11)+T(3)+NT(11)+NT(407)));
//G614: def_output_cmd(405)  => __E_cli_command_183(406).
	p(NT(405), (NT(406)));
//G615: cli_command(286)     => def_output_cmd(405).
	p(NT(286), (NT(405)));
//G616: __E___E_cli_command_184_185(411) => 'h' 'i' 's' 't'.
	p(NT(411), (T(56)+T(10)+T(12)+T(13)));
//G617: __E___E_cli_command_184_185(411) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(411), (T(56)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G618: history_sym(410)     => __E___E_cli_command_184_185(411).
	p(NT(410), (NT(411)));
//G619: __E_cli_command_184(409) => history_sym(410).
	p(NT(409), (NT(410)));
//G620: history_list_cmd(408) => __E_cli_command_184(409).
	p(NT(408), (NT(409)));
//G621: cli_command(286)     => history_list_cmd(408).
	p(NT(286), (NT(408)));
//G622: __E_cli_command_186(413) => history_sym(410) __(27) memory(332).
	p(NT(413), (NT(410)+NT(27)+NT(332)));
//G623: history_print_cmd(412) => __E_cli_command_186(413).
	p(NT(412), (NT(413)));
//G624: cli_command(286)     => history_print_cmd(412).
	p(NT(286), (NT(412)));
//G625: __E_cli_command_187(415) => wff(14).
	p(NT(415), (NT(14)));
//G626: __E_cli_command_187(415) => bf(21).
	p(NT(415), (NT(21)));
//G627: history_store_cmd(414) => __E_cli_command_187(415).
	p(NT(414), (NT(415)));
//G628: cli_command(286)     => history_store_cmd(414).
	p(NT(286), (NT(414)));
//G629: __E_solve_options_188(416) => __(27) solve_option(417).
	p(NT(416), (NT(27)+NT(417)));
//G630: __E_solve_options_189(418) => null.
	p(NT(418), (nul));
//G631: __E_solve_options_189(418) => __E_solve_options_188(416) __E_solve_options_189(418).
	p(NT(418), (NT(416)+NT(418)));
//G632: solve_options(322)   => __E_solve_options_189(418).
	p(NT(322), (NT(418)));
//G633: __E_solve_option_190(419) => '-' '-' solver_mode(420).
	p(NT(419), (T(28)+T(28)+NT(420)));
//G634: solve_option(417)    => __E_solve_option_190(419).
	p(NT(417), (NT(419)));
//G635: __E_solve_option_191(421) => '-' '-' type(150).
	p(NT(421), (T(28)+T(28)+NT(150)));
//G636: solve_option(417)    => __E_solve_option_191(421).
	p(NT(417), (NT(421)));
//G637: __E_solver_mode_192(423) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(423), (T(18)+T(10)+T(45)+T(10)+T(18)+T(47)+T(18)));
//G638: __E_solver_mode_192(423) => 'm' 'i' 'n'.
	p(NT(423), (T(18)+T(10)+T(45)));
//G639: solver_mode_minimum(422) => __E_solver_mode_192(423).
	p(NT(422), (NT(423)));
//G640: solver_mode(420)     => solver_mode_minimum(422).
	p(NT(420), (NT(422)));
//G641: __E_solver_mode_193(425) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(425), (T(18)+T(5)+T(27)+T(10)+T(18)+T(47)+T(18)));
//G642: __E_solver_mode_193(425) => 'm' 'a' 'x'.
	p(NT(425), (T(18)+T(5)+T(27)));
//G643: solver_mode_maximum(424) => __E_solver_mode_193(425).
	p(NT(424), (NT(425)));
//G644: solver_mode(420)     => solver_mode_maximum(424).
	p(NT(420), (NT(424)));
//G645: __E_input_stream_194(426) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(427) _(11) ')'.
	p(NT(426), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(427)+NT(11)+T(16)));
//G646: input_stream(404)    => __E_input_stream_194(426).
	p(NT(404), (NT(426)));
//G647: console_sym(428)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(428), (T(8)+T(17)+T(45)+T(12)+T(17)+T(6)+T(19)));
//G648: input_stream(404)    => console_sym(428).
	p(NT(404), (NT(428)));
//G649: __E_output_stream_195(429) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(427) _(11) ')'.
	p(NT(429), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(427)+NT(11)+T(16)));
//G650: output_stream(407)   => __E_output_stream_195(429).
	p(NT(407), (NT(429)));
//G651: output_stream(407)   => console_sym(428).
	p(NT(407), (NT(428)));
//G652: q_file_name(427)     => '"' file_name(430) '"'.
	p(NT(427), (T(61)+NT(430)+T(61)));
//G653: __E_file_name_196(431) => printable(8).
	p(NT(431), (NT(8)));
//G654: __E_file_name_196(431) => printable(8) __E_file_name_196(431).
	p(NT(431), (NT(8)+NT(431)));
//G655: file_name(430)       => __E_file_name_196(431).
	p(NT(430), (NT(431)));
//G656: __E_option_name_197(432) => alnum(6).
	p(NT(432), (NT(6)));
//G657: __E_option_name_197(432) => alnum(6) __E_option_name_197(432).
	p(NT(432), (NT(6)+NT(432)));
//G658: option_name(387)     => __E_option_name_197(432).
	p(NT(387), (NT(432)));
//G659: __E_option_value_198(433) => alnum(6).
	p(NT(433), (NT(6)));
//G660: __E_option_value_198(433) => alnum(6) __E_option_value_198(433).
	p(NT(433), (NT(6)+NT(433)));
//G661: option_value(392)    => __E_option_value_198(433).
	p(NT(392), (NT(433)));
//G662: number(378)          => digits(259).
	p(NT(378), (NT(259)));
//G663: bf_cmd_arg(434)      => memory(332).
	p(NT(434), (NT(332)));
//G664: bf_cmd_arg(434)      => bf(21).
	p(NT(434), (NT(21)));
//G665: wff_cmd_arg(323)     => memory(332).
	p(NT(323), (NT(332)));
//G666: wff_cmd_arg(323)     => wff(14).
	p(NT(323), (NT(14)));
//G667: nf_cmd_arg(341)      => memory(332).
	p(NT(341), (NT(332)));
//G668: nf_cmd_arg(341)      => ref(18).
	p(NT(341), (NT(18)));
//G669: nf_cmd_arg(341)      => wff(14).
	p(NT(341), (NT(14)));
//G670: nf_cmd_arg(341)      => bf(21).
	p(NT(341), (NT(21)));
//G671: onf_cmd_arg(371)     => memory(332).
	p(NT(371), (NT(332)));
//G672: onf_cmd_arg(371)     => wff(14).
	p(NT(371), (NT(14)));
//G673: normalize_cmd_arg(311) => memory(332).
	p(NT(311), (NT(332)));
//G674: normalize_cmd_arg(311) => rr(10).
	p(NT(311), (NT(10)));
//G675: normalize_cmd_arg(311) => ref(18).
	p(NT(311), (NT(18)));
//G676: normalize_cmd_arg(311) => wff(14).
	p(NT(311), (NT(14)));
//G677: normalize_cmd_arg(311) => bf(21).
	p(NT(311), (NT(21)));
//G678: inst_args(346)       => wff_cmd_arg(323) _(11) '[' _(11) variable(139) _(11) '/' _(11) bf_cmd_arg(434) _(11) ']'.
	p(NT(346), (NT(323)+NT(11)+T(24)+NT(11)+NT(139)+NT(11)+T(43)+NT(11)+NT(434)+NT(11)+T(25)));
//G679: inst_args(346)       => bf_cmd_arg(434) _(11) '[' _(11) variable(139) _(11) '/' _(11) bf_cmd_arg(434) _(11) ']'.
	p(NT(346), (NT(434)+NT(11)+T(24)+NT(11)+NT(139)+NT(11)+T(43)+NT(11)+NT(434)+NT(11)+T(25)));
//G680: help_arg(303)        => help_sym(300).
	p(NT(303), (NT(300)));
//G681: help_arg(303)        => version_sym(293).
	p(NT(303), (NT(293)));
//G682: help_arg(303)        => quit_sym(290).
	p(NT(303), (NT(290)));
//G683: help_arg(303)        => clear_sym(296).
	p(NT(303), (NT(296)));
//G684: help_arg(303)        => get_sym(385).
	p(NT(303), (NT(385)));
//G685: help_arg(303)        => set_sym(390).
	p(NT(303), (NT(390)));
//G686: help_arg(303)        => enable_sym(395).
	p(NT(303), (NT(395)));
//G687: help_arg(303)        => disable_sym(398).
	p(NT(303), (NT(398)));
//G688: help_arg(303)        => toggle_sym(401).
	p(NT(303), (NT(401)));
//G689: help_arg(303)        => file_sym(306).
	p(NT(303), (NT(306)));
//G690: help_arg(303)        => history_sym(410).
	p(NT(303), (NT(410)));
//G691: help_arg(303)        => abs_memory_sym(435).
	p(NT(303), (NT(435)));
//G692: help_arg(303)        => rel_memory_sym(436).
	p(NT(303), (NT(436)));
//G693: help_arg(303)        => selection_sym(437).
	p(NT(303), (NT(437)));
//G694: help_arg(303)        => def_sym(374).
	p(NT(303), (NT(374)));
//G695: help_arg(303)        => inst_sym(344).
	p(NT(303), (NT(344)));
//G696: help_arg(303)        => subst_sym(339).
	p(NT(303), (NT(339)));
//G697: help_arg(303)        => normalize_sym(335).
	p(NT(303), (NT(335)));
//G698: help_arg(303)        => execute_sym(438).
	p(NT(303), (NT(438)));
//G699: help_arg(303)        => solve_sym(320).
	p(NT(303), (NT(320)));
//G700: help_arg(303)        => valid_sym(310).
	p(NT(303), (NT(310)));
//G701: help_arg(303)        => sat_sym(314).
	p(NT(303), (NT(314)));
//G702: help_arg(303)        => unsat_sym(317).
	p(NT(303), (NT(317)));
//G703: help_arg(303)        => run_sym(329).
	p(NT(303), (NT(329)));
//G704: help_arg(303)        => dnf_sym(349).
	p(NT(303), (NT(349)));
//G705: help_arg(303)        => cnf_sym(352).
	p(NT(303), (NT(352)));
//G706: help_arg(303)        => anf_sym(355).
	p(NT(303), (NT(355)));
//G707: help_arg(303)        => snf_sym(367).
	p(NT(303), (NT(367)));
//G708: help_arg(303)        => nnf_sym(358).
	p(NT(303), (NT(358)));
//G709: help_arg(303)        => pnf_sym(361).
	p(NT(303), (NT(361)));
//G710: help_arg(303)        => mnf_sym(364).
	p(NT(303), (NT(364)));
//G711: help_arg(303)        => onf_sym(370).
	p(NT(303), (NT(370)));
//G712: help_arg(303)        => qelim_sym(382).
	p(NT(303), (NT(382)));
//G713: __E___E_help_arg_199_200(441) => 's'.
	p(NT(441), (T(12)));
//G714: __E___E_help_arg_199_200(441) => null.
	p(NT(441), (nul));
//G715: __E_help_arg_199(440) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_199_200(441).
	p(NT(440), (T(19)+T(27)+T(5)+T(18)+T(57)+T(6)+T(19)+NT(441)));
//G716: examples_sym(439)    => __E_help_arg_199(440).
	p(NT(439), (NT(440)));
//G717: help_arg(303)        => examples_sym(439).
	p(NT(303), (NT(439)));
//G718: __E_memory_201(443)  => '%'.
	p(NT(443), (T(44)));
//G719: rel_memory_sym(436)  => '%' '-'.
	p(NT(436), (T(44)+T(28)));
//G720: memory_id(445)       => digits(259).
	p(NT(445), (NT(259)));
//G721: __E___E_memory_201_202(444) => memory_id(445).
	p(NT(444), (NT(445)));
//G722: __E___E_memory_201_202(444) => null.
	p(NT(444), (nul));
//G723: __E_memory_201(443)  => rel_memory_sym(436) __E___E_memory_201_202(444).
	p(NT(443), (NT(436)+NT(444)));
//G724: rel_memory(442)      => __E_memory_201(443).
	p(NT(442), (NT(443)));
//G725: memory(332)          => rel_memory(442).
	p(NT(332), (NT(442)));
//G726: abs_memory_sym(435)  => '%'.
	p(NT(435), (T(44)));
//G727: __E_memory_203(447)  => abs_memory_sym(435) memory_id(445).
	p(NT(447), (NT(435)+NT(445)));
//G728: abs_memory(446)      => __E_memory_203(447).
	p(NT(446), (NT(447)));
//G729: memory(332)          => abs_memory(446).
	p(NT(332), (NT(446)));
	#undef T
	#undef NT
	return loaded = true, p;
}

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace tau_parser_data

struct tau_parser_nonterminals {
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
		src_c, __E___E_binding_125_126, __E___E___E_binding_125_126_127, __E___E___E_binding_125_126_128, __E_src_c_129, __E_src_c_130, __E_chars_131, __E_chars_132, __E_digits_133, comment, 
		__E_comment_134, __E_comment_135, __E_comment_136, __E____137, __E___138, cli, cli_command, __E_cli_139, __E_cli_140, quit_cmd, 
		quit_sym, __E_cli_command_141, version_cmd, version_sym, __E_cli_command_142, clear_cmd, clear_sym, __E_cli_command_143, help_cmd, __E_cli_command_144, 
		help_sym, __E___E_cli_command_144_145, __E___E_cli_command_144_146, help_arg, file_cmd, __E_cli_command_147, file_sym, q_string, valid_cmd, __E_cli_command_148, 
		valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_149, sat_sym, unsat_cmd, __E_cli_command_150, unsat_sym, solve_cmd, __E_cli_command_151, 
		solve_sym, __E___E_cli_command_151_152, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_153, lgrs_sym, run_cmd, __E_cli_command_154, run_sym, 
		__E___E_cli_command_154_155, __E___E_cli_command_154_156, memory, normalize_cmd, __E_cli_command_157, normalize_sym, __E___E_cli_command_157_158, subst_cmd, __E_cli_command_159, subst_sym, 
		__E___E_cli_command_159_160, nf_cmd_arg, inst_cmd, __E_cli_command_161, inst_sym, __E___E_cli_command_161_162, inst_args, dnf_cmd, __E_cli_command_163, dnf_sym, 
		cnf_cmd, __E_cli_command_164, cnf_sym, anf_cmd, __E_cli_command_165, anf_sym, nnf_cmd, __E_cli_command_166, nnf_sym, pnf_cmd, 
		__E_cli_command_167, pnf_sym, mnf_cmd, __E_cli_command_168, mnf_sym, snf_cmd, __E_cli_command_169, snf_sym, onf_cmd, __E_cli_command_170, 
		onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_171, def_sym, __E___E_cli_command_171_172, def_print_cmd, __E_cli_command_173, number, def_rr_cmd, 
		qelim_cmd, __E_cli_command_174, qelim_sym, get_cmd, __E_cli_command_175, get_sym, __E___E_cli_command_175_176, option_name, set_cmd, __E_cli_command_177, 
		set_sym, __E___E_cli_command_177_178, option_value, enable_cmd, __E_cli_command_179, enable_sym, disable_cmd, __E_cli_command_180, disable_sym, toggle_cmd, 
		__E_cli_command_181, toggle_sym, def_input_cmd, __E_cli_command_182, input_stream, def_output_cmd, __E_cli_command_183, output_stream, history_list_cmd, __E_cli_command_184, 
		history_sym, __E___E_cli_command_184_185, history_print_cmd, __E_cli_command_186, history_store_cmd, __E_cli_command_187, __E_solve_options_188, solve_option, __E_solve_options_189, __E_solve_option_190, 
		solver_mode, __E_solve_option_191, solver_mode_minimum, __E_solver_mode_192, solver_mode_maximum, __E_solver_mode_193, __E_input_stream_194, q_file_name, console_sym, __E_output_stream_195, 
		file_name, __E_file_name_196, __E_option_name_197, __E_option_value_198, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, execute_sym, examples_sym, 
		__E_help_arg_199, __E___E_help_arg_199_200, rel_memory, __E_memory_201, __E___E_memory_201_202, memory_id, abs_memory, __E_memory_203, named_binding, __N_0, 
		__N_1, __N_2, __N_3, __N_4, __N_5, __N_6, __N_7, __N_8, 
	};
};

struct tau_parser : public idni::parser<char, char>, public tau_parser_nonterminals {
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
