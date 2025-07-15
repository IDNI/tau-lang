// This file is generated from a file parser/tau.tgf by
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
	"__E_bv_70", "bv_add", "__E_bv_71", "bv_mul", "__E_bv_72", "bv_div", "__E_bv_73", "bv_mod", "__E_bv_74", "bv_sub", 
	"__E_bv_75", "bv_neg", "bv_and", "__E_bv_76", "bv_nand", "__E_bv_77", "bv_or", "__E_bv_78", "bv_nor", "__E_bv_79", 
	"bv_xor", "__E_bv_80", "bv_xnor", "__E_bv_81", "bv_rotate_left", "__E_bv_82", "bv_rotate_right", "__E_bv_83", "bv_min", "__E_bv_84", 
	"bv_max", "__E_bv_85", "bitvector", "bits", "__E_bitvector_86", "bv_type", "num", "__E_bitvector_87", "hexnum", "__E_bitvector_88", 
	"__E_bits_89", "__E_bits_90", "__E_hexnum_91", "ctn_neq", "__E_constraint_92", "ctnvar", "ctn_eq", "__E_constraint_93", "ctn_greater_equal", "__E_constraint_94", 
	"ctn_greater", "__E_constraint_95", "ctn_less_equal", "__E_constraint_96", "ctn_less", "__E_constraint_97", "__E_variable_98", "__E_variable_99", "__E_variable_100", "__E_variable_101", 
	"__E_variable_102", "__E_variable_103", "__E_variable_104", "io_var", "__E_variable_105", "in", "out", "__E_variable_106", "uninterpreted_constant", "__E_variable_107", 
	"in_var_name", "__E_in_108", "__E___E_in_108_109", "__E___E___E_in_108_109_110", "offset", "out_var_name", "__E_out_111", "__E___E_out_111_112", "__E___E___E_out_111_112_113", "chars", 
	"q_var", "__E_q_vars_114", "__E_q_vars_115", "__E_q_var_116", "__E_offsets_117", "__E_offsets_118", "integer", "shift", "__E_offset_119", "__E_shift_120", 
	"__E___E_shift_120_121", "digits", "__E_integer_122", "uninter_const_name", "__E_uninterpreted_constant_123", "__E___E_uninterpreted_constant_123_124", "constant", "__E_bf_constant_125", "binding", "__E_bf_constant_126", 
	"source", "__E_binding_127", "src_c", "__E___E_binding_127_128", "__E___E___E_binding_127_128_129", "__E___E___E_binding_127_128_130", "__E_src_c_131", "__E_src_c_132", "__E_bv_type_133", "__E_chars_134", 
	"__E_chars_135", "__E_digits_136", "comment", "__E_comment_137", "__E_comment_138", "__E_comment_139", "__E____140", "__E___141", "cli", "cli_command", 
	"__E_cli_142", "__E_cli_143", "quit_cmd", "quit_sym", "__E_cli_command_144", "version_cmd", "version_sym", "__E_cli_command_145", "clear_cmd", "clear_sym", 
	"__E_cli_command_146", "help_cmd", "__E_cli_command_147", "help_sym", "__E___E_cli_command_147_148", "__E___E_cli_command_147_149", "help_arg", "file_cmd", "__E_cli_command_150", "file_sym", 
	"q_string", "valid_cmd", "__E_cli_command_151", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_152", "sat_sym", "unsat_cmd", "__E_cli_command_153", 
	"unsat_sym", "solve_cmd", "__E_cli_command_154", "solve_sym", "__E___E_cli_command_154_155", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_156", "lgrs_sym", 
	"run_cmd", "__E_cli_command_157", "run_sym", "__E___E_cli_command_157_158", "__E___E_cli_command_157_159", "memory", "normalize_cmd", "__E_cli_command_160", "normalize_sym", "__E___E_cli_command_160_161", 
	"subst_cmd", "__E_cli_command_162", "subst_sym", "__E___E_cli_command_162_163", "nf_cmd_arg", "inst_cmd", "__E_cli_command_164", "inst_sym", "__E___E_cli_command_164_165", "inst_args", 
	"dnf_cmd", "__E_cli_command_166", "dnf_sym", "cnf_cmd", "__E_cli_command_167", "cnf_sym", "anf_cmd", "__E_cli_command_168", "anf_sym", "nnf_cmd", 
	"__E_cli_command_169", "nnf_sym", "pnf_cmd", "__E_cli_command_170", "pnf_sym", "mnf_cmd", "__E_cli_command_171", "mnf_sym", "snf_cmd", "__E_cli_command_172", 
	"snf_sym", "onf_cmd", "__E_cli_command_173", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_174", "def_sym", "__E___E_cli_command_174_175", "def_print_cmd", 
	"__E_cli_command_176", "number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_177", "qelim_sym", "get_cmd", "__E_cli_command_178", "get_sym", "__E___E_cli_command_178_179", 
	"option_name", "set_cmd", "__E_cli_command_180", "set_sym", "__E___E_cli_command_180_181", "option_value", "enable_cmd", "__E_cli_command_182", "enable_sym", "disable_cmd", 
	"__E_cli_command_183", "disable_sym", "toggle_cmd", "__E_cli_command_184", "toggle_sym", "def_input_cmd", "__E_cli_command_185", "input_stream", "def_output_cmd", "__E_cli_command_186", 
	"output_stream", "history_list_cmd", "__E_cli_command_187", "history_sym", "__E___E_cli_command_187_188", "history_print_cmd", "__E_cli_command_189", "history_store_cmd", "__E_cli_command_190", "__E_solve_options_191", 
	"solve_option", "__E_solve_options_192", "__E_solve_option_193", "solver_mode", "__E_solve_option_194", "solver_mode_minimum", "__E_solver_mode_195", "solver_mode_maximum", "__E_solver_mode_196", "__E_input_stream_197", 
	"q_file_name", "console_sym", "__E_output_stream_198", "file_name", "__E_file_name_199", "__E_option_name_200", "__E_option_value_201", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", 
	"selection_sym", "execute_sym", "examples_sym", "__E_help_arg_202", "__E___E_help_arg_202_203", "rel_memory", "__E_memory_204", "__E___E_memory_204_205", "memory_id", "abs_memory", 
	"__E_memory_206", "named_binding", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", "__N_7", 
	"__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'f', 'a', 'l', 'b', 'c', 
	'k', 'i', 'r', 's', 't', ',', '(', ')', 'o', 'm', 
	'e', '<', '>', 'w', 'y', '[', ']', '?', 'x', '-', 
	'|', '^', '&', '!', 'T', 'F', '_', 'S', '+', '1', 
	'0', '\'', '*', '/', '%', '~', 'n', '#', 'h', 'u', 
	'$', '{', '}', 'v', '\t', '\n', '\r', 'q', 'p', 'd', 
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
			20, 22, 139, 150, 203, 205, 208, 215, 240, 245,
			256, 261, 263, 270, 390, 395, 433, 451
		},
		.to_inline = {
			{ 14, 60, 14 },
			{ 21, 136, 21 },
			{ 117, 167, 117 },
			{ 159, 136, 21 },
			{ 161, 136, 154 },
			{ 163, 136, 21 },
			{ 165, 136, 21 },
			{ 249 },
			{ 250 },
			{ 272 },
			{ 314 },
			{ 326 },
			{ 344 },
			{ 349 },
			{ 374 },
			{ 437 }
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
//G247: __E_bv_71(172)       => bv(117) _(11) '+' _(11) bv(117).
	p(NT(172), (NT(117)+NT(11)+T(37)+NT(11)+NT(117)));
//G248: bv_add(171)          => __E_bv_71(172).
	p(NT(171), (NT(172)));
//G249: bv(117)              => bv_add(171).
	p(NT(117), (NT(171)));
//G250: __E_bv_72(174)       => bv(117) _(11) '*' _(11) bv(117).
	p(NT(174), (NT(117)+NT(11)+T(41)+NT(11)+NT(117)));
//G251: bv_mul(173)          => __E_bv_72(174).
	p(NT(173), (NT(174)));
//G252: bv(117)              => bv_mul(173).
	p(NT(117), (NT(173)));
//G253: __E_bv_73(176)       => bv(117) _(11) '/' _(11) bv(117).
	p(NT(176), (NT(117)+NT(11)+T(42)+NT(11)+NT(117)));
//G254: bv_div(175)          => __E_bv_73(176).
	p(NT(175), (NT(176)));
//G255: bv(117)              => bv_div(175).
	p(NT(117), (NT(175)));
//G256: __E_bv_74(178)       => bv(117) _(11) '%' _(11) bv(117).
	p(NT(178), (NT(117)+NT(11)+T(43)+NT(11)+NT(117)));
//G257: bv_mod(177)          => __E_bv_74(178).
	p(NT(177), (NT(178)));
//G258: bv(117)              => bv_mod(177).
	p(NT(117), (NT(177)));
//G259: __E_bv_75(180)       => bv(117) _(11) '-' _(11) bv(117).
	p(NT(180), (NT(117)+NT(11)+T(28)+NT(11)+NT(117)));
//G260: bv_sub(179)          => __E_bv_75(180).
	p(NT(179), (NT(180)));
//G261: bv(117)              => bv_sub(179).
	p(NT(117), (NT(179)));
//G262: bv_neg(181)          => bv(117).
	p(NT(181), (NT(117)));
//G263: bv(117)              => '~' _(11) bv_neg(181).
	p(NT(117), (T(44)+NT(11)+NT(181)));
//G264: __E_bv_76(183)       => bv(117) _(11) '&' _(11) bv(117).
	p(NT(183), (NT(117)+NT(11)+T(31)+NT(11)+NT(117)));
//G265: bv_and(182)          => __E_bv_76(183).
	p(NT(182), (NT(183)));
//G266: bv(117)              => bv_and(182).
	p(NT(117), (NT(182)));
//G267: __E_bv_77(185)       => bv(117) _(11) '!' '&' _(11) bv(117).
	p(NT(185), (NT(117)+NT(11)+T(32)+T(31)+NT(11)+NT(117)));
//G268: bv_nand(184)         => __E_bv_77(185).
	p(NT(184), (NT(185)));
//G269: bv(117)              => bv_nand(184).
	p(NT(117), (NT(184)));
//G270: __E_bv_78(187)       => bv(117) _(11) '|' _(11) bv(117).
	p(NT(187), (NT(117)+NT(11)+T(29)+NT(11)+NT(117)));
//G271: bv_or(186)           => __E_bv_78(187).
	p(NT(186), (NT(187)));
//G272: bv(117)              => bv_or(186).
	p(NT(117), (NT(186)));
//G273: __E_bv_79(189)       => bv(117) _(11) '!' '|' _(11) bv(117).
	p(NT(189), (NT(117)+NT(11)+T(32)+T(29)+NT(11)+NT(117)));
//G274: bv_nor(188)          => __E_bv_79(189).
	p(NT(188), (NT(189)));
//G275: bv(117)              => bv_nor(188).
	p(NT(117), (NT(188)));
//G276: __E_bv_80(191)       => bv(117) _(11) '(' '+' ')' _(11) bv(117).
	p(NT(191), (NT(117)+NT(11)+T(15)+T(37)+T(16)+NT(11)+NT(117)));
//G277: bv_xor(190)          => __E_bv_80(191).
	p(NT(190), (NT(191)));
//G278: bv(117)              => bv_xor(190).
	p(NT(117), (NT(190)));
//G279: __E_bv_81(193)       => bv(117) _(11) '!' '(' '+' ')' _(11) bv(117).
	p(NT(193), (NT(117)+NT(11)+T(32)+T(15)+T(37)+T(16)+NT(11)+NT(117)));
//G280: bv_xnor(192)         => __E_bv_81(193).
	p(NT(192), (NT(193)));
//G281: bv(117)              => bv_xnor(192).
	p(NT(117), (NT(192)));
//G282: __E_bv_82(195)       => bv(117) _(11) '<' '<' _(11) bv(117).
	p(NT(195), (NT(117)+NT(11)+T(20)+T(20)+NT(11)+NT(117)));
//G283: bv_rotate_left(194)  => __E_bv_82(195).
	p(NT(194), (NT(195)));
//G284: bv(117)              => bv_rotate_left(194).
	p(NT(117), (NT(194)));
//G285: __E_bv_83(197)       => bv(117) _(11) '>' '>' _(11) bv(117).
	p(NT(197), (NT(117)+NT(11)+T(21)+T(21)+NT(11)+NT(117)));
//G286: bv_rotate_right(196) => __E_bv_83(197).
	p(NT(196), (NT(197)));
//G287: bv(117)              => bv_rotate_right(196).
	p(NT(117), (NT(196)));
//G288: __E_bv_84(199)       => 'm' 'i' 'n' __(27) bv(117) __(27) bv(117).
	p(NT(199), (T(18)+T(10)+T(45)+NT(27)+NT(117)+NT(27)+NT(117)));
//G289: bv_min(198)          => __E_bv_84(199).
	p(NT(198), (NT(199)));
//G290: bv(117)              => bv_min(198).
	p(NT(117), (NT(198)));
//G291: __E_bv_85(201)       => 'm' 'a' 'x' __(27) bv(117) __(27) bv(117).
	p(NT(201), (T(18)+T(5)+T(27)+NT(27)+NT(117)+NT(27)+NT(117)));
//G292: bv_max(200)          => __E_bv_85(201).
	p(NT(200), (NT(201)));
//G293: bv(117)              => bv_max(200).
	p(NT(117), (NT(200)));
//G294: bv(117)              => bitvector(202).
	p(NT(117), (NT(202)));
//G295: __E_bitvector_86(204) => _(11) ':' _(11) bv_type(205) _(11).
	p(NT(204), (NT(11)+T(2)+NT(11)+NT(205)+NT(11)));
//G296: __E_bitvector_86(204) => null.
	p(NT(204), (nul));
//G297: bitvector(202)       => '#' 'b' bits(203) __E_bitvector_86(204).
	p(NT(202), (T(46)+T(7)+NT(203)+NT(204)));
//G298: __E_bitvector_87(207) => _(11) ':' _(11) bv_type(205) _(11).
	p(NT(207), (NT(11)+T(2)+NT(11)+NT(205)+NT(11)));
//G299: __E_bitvector_87(207) => null.
	p(NT(207), (nul));
//G300: bitvector(202)       => num(206) __E_bitvector_87(207).
	p(NT(202), (NT(206)+NT(207)));
//G301: __E_bitvector_88(209) => _(11) ':' _(11) bv_type(205) _(11).
	p(NT(209), (NT(11)+T(2)+NT(11)+NT(205)+NT(11)));
//G302: __E_bitvector_88(209) => null.
	p(NT(209), (nul));
//G303: bitvector(202)       => '#' 'x' hexnum(208) __E_bitvector_88(209).
	p(NT(202), (T(46)+T(27)+NT(208)+NT(209)));
//G304: __E_bits_89(210)     => '0'.
	p(NT(210), (T(39)));
//G305: __E_bits_89(210)     => '1'.
	p(NT(210), (T(38)));
//G306: __E_bits_90(211)     => __E_bits_89(210).
	p(NT(211), (NT(210)));
//G307: __E_bits_90(211)     => __E_bits_89(210) __E_bits_90(211).
	p(NT(211), (NT(210)+NT(211)));
//G308: bits(203)            => __E_bits_90(211).
	p(NT(203), (NT(211)));
//G309: __E_hexnum_91(212)   => xdigit(4).
	p(NT(212), (NT(4)));
//G310: __E_hexnum_91(212)   => xdigit(4) __E_hexnum_91(212).
	p(NT(212), (NT(4)+NT(212)));
//G311: hexnum(208)          => __E_hexnum_91(212).
	p(NT(208), (NT(212)));
//G312: __E_constraint_92(214) => '[' ctnvar(215) _(11) '!' '=' _(11) num(206) ']'.
	p(NT(214), (T(24)+NT(215)+NT(11)+T(32)+T(3)+NT(11)+NT(206)+T(25)));
//G313: __E_constraint_92(214) => '[' num(206) _(11) '!' '=' _(11) ctnvar(215) ']'.
	p(NT(214), (T(24)+NT(206)+NT(11)+T(32)+T(3)+NT(11)+NT(215)+T(25)));
//G314: ctn_neq(213)         => __E_constraint_92(214).
	p(NT(213), (NT(214)));
//G315: constraint(76)       => ctn_neq(213).
	p(NT(76), (NT(213)));
//G316: __E_constraint_93(217) => '[' ctnvar(215) _(11) '=' _(11) num(206) ']'.
	p(NT(217), (T(24)+NT(215)+NT(11)+T(3)+NT(11)+NT(206)+T(25)));
//G317: __E_constraint_93(217) => '[' num(206) _(11) '=' _(11) ctnvar(215) ']'.
	p(NT(217), (T(24)+NT(206)+NT(11)+T(3)+NT(11)+NT(215)+T(25)));
//G318: ctn_eq(216)          => __E_constraint_93(217).
	p(NT(216), (NT(217)));
//G319: constraint(76)       => ctn_eq(216).
	p(NT(76), (NT(216)));
//G320: __E_constraint_94(219) => '[' ctnvar(215) _(11) '>' '=' _(11) num(206) ']'.
	p(NT(219), (T(24)+NT(215)+NT(11)+T(21)+T(3)+NT(11)+NT(206)+T(25)));
//G321: __E_constraint_94(219) => '[' num(206) _(11) '>' '=' _(11) ctnvar(215) ']'.
	p(NT(219), (T(24)+NT(206)+NT(11)+T(21)+T(3)+NT(11)+NT(215)+T(25)));
//G322: ctn_greater_equal(218) => __E_constraint_94(219).
	p(NT(218), (NT(219)));
//G323: constraint(76)       => ctn_greater_equal(218).
	p(NT(76), (NT(218)));
//G324: __E_constraint_95(221) => '[' ctnvar(215) _(11) '>' _(11) num(206) ']'.
	p(NT(221), (T(24)+NT(215)+NT(11)+T(21)+NT(11)+NT(206)+T(25)));
//G325: __E_constraint_95(221) => '[' num(206) _(11) '>' _(11) ctnvar(215) ']'.
	p(NT(221), (T(24)+NT(206)+NT(11)+T(21)+NT(11)+NT(215)+T(25)));
//G326: ctn_greater(220)     => __E_constraint_95(221).
	p(NT(220), (NT(221)));
//G327: constraint(76)       => ctn_greater(220).
	p(NT(76), (NT(220)));
//G328: __E_constraint_96(223) => '[' ctnvar(215) _(11) '<' '=' _(11) num(206) ']'.
	p(NT(223), (T(24)+NT(215)+NT(11)+T(20)+T(3)+NT(11)+NT(206)+T(25)));
//G329: __E_constraint_96(223) => '[' num(206) _(11) '<' '=' _(11) ctnvar(215) ']'.
	p(NT(223), (T(24)+NT(206)+NT(11)+T(20)+T(3)+NT(11)+NT(215)+T(25)));
//G330: ctn_less_equal(222)  => __E_constraint_96(223).
	p(NT(222), (NT(223)));
//G331: constraint(76)       => ctn_less_equal(222).
	p(NT(76), (NT(222)));
//G332: __E_constraint_97(225) => '[' ctnvar(215) _(11) '<' _(11) num(206) ']'.
	p(NT(225), (T(24)+NT(215)+NT(11)+T(20)+NT(11)+NT(206)+T(25)));
//G333: __E_constraint_97(225) => '[' num(206) _(11) '<' _(11) ctnvar(215) ']'.
	p(NT(225), (T(24)+NT(206)+NT(11)+T(20)+NT(11)+NT(215)+T(25)));
//G334: ctn_less(224)        => __E_constraint_97(225).
	p(NT(224), (NT(225)));
//G335: constraint(76)       => ctn_less(224).
	p(NT(76), (NT(224)));
//G336: __N_0(452)           => 'F'.
	p(NT(452), (T(34)));
//G337: __N_1(453)           => 'T'.
	p(NT(453), (T(33)));
//G338: __E_variable_98(226) => ~( __N_0(452) ) & ~( __N_1(453) ) & alpha(5).	 # conjunctive
	p(NT(226), ~(NT(452)) & ~(NT(453)) & (NT(5)));
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
//G344: __N_2(454)           => 'F'.
	p(NT(454), (T(34)));
//G345: __N_3(455)           => 'T'.
	p(NT(455), (T(33)));
//G346: __E_variable_101(229) => ~( __N_2(454) ) & ~( __N_3(455) ) & alpha(5).	 # conjunctive
	p(NT(229), ~(NT(454)) & ~(NT(455)) & (NT(5)));
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
//G360: __E_variable_107(239) => _(11) ':' _(11) type(150).
	p(NT(239), (NT(11)+T(2)+NT(11)+NT(150)));
//G361: __E_variable_107(239) => null.
	p(NT(239), (nul));
//G362: variable(139)        => uninterpreted_constant(238) __E_variable_107(239).
	p(NT(139), (NT(238)+NT(239)));
//G363: __E___E___E_in_108_109_110(243) => digit(3).
	p(NT(243), (NT(3)));
//G364: __E___E___E_in_108_109_110(243) => digit(3) __E___E___E_in_108_109_110(243).
	p(NT(243), (NT(3)+NT(243)));
//G365: __E___E_in_108_109(242) => 'i' __E___E___E_in_108_109_110(243).
	p(NT(242), (T(10)+NT(243)));
//G366: __E_in_108(241)      => __E___E_in_108_109(242).
	p(NT(241), (NT(242)));
//G367: __E_in_108(241)      => 't' 'h' 'i' 's'.
	p(NT(241), (T(13)+T(47)+T(10)+T(12)));
//G368: in_var_name(240)     => __E_in_108(241).
	p(NT(240), (NT(241)));
//G369: in(235)              => in_var_name(240) '[' offset(244) ']'.
	p(NT(235), (NT(240)+T(24)+NT(244)+T(25)));
//G370: __E___E___E_out_111_112_113(248) => digit(3).
	p(NT(248), (NT(3)));
//G371: __E___E___E_out_111_112_113(248) => digit(3) __E___E___E_out_111_112_113(248).
	p(NT(248), (NT(3)+NT(248)));
//G372: __E___E_out_111_112(247) => 'o' __E___E___E_out_111_112_113(248).
	p(NT(247), (T(17)+NT(248)));
//G373: __E_out_111(246)     => __E___E_out_111_112(247).
	p(NT(246), (NT(247)));
//G374: __E_out_111(246)     => 'u'.
	p(NT(246), (T(48)));
//G375: out_var_name(245)    => __E_out_111(246).
	p(NT(245), (NT(246)));
//G376: out(236)             => out_var_name(245) '[' offset(244) ']'.
	p(NT(236), (NT(245)+T(24)+NT(244)+T(25)));
//G377: ctnvar(215)          => chars(249).
	p(NT(215), (NT(249)));
//G378: capture(20)          => '$' chars(249).
	p(NT(20), (T(49)+NT(249)));
//G379: __E_q_vars_114(251)  => _(11) ',' _(11) q_var(250).
	p(NT(251), (NT(11)+T(14)+NT(11)+NT(250)));
//G380: __E_q_vars_115(252)  => null.
	p(NT(252), (nul));
//G381: __E_q_vars_115(252)  => __E_q_vars_114(251) __E_q_vars_115(252).
	p(NT(252), (NT(251)+NT(252)));
//G382: q_vars(72)           => q_var(250) __E_q_vars_115(252).
	p(NT(72), (NT(250)+NT(252)));
//G383: q_var(250)           => capture(20).
	p(NT(250), (NT(20)));
//G384: __N_4(456)           => uninterpreted_constant(238).
	p(NT(456), (NT(238)));
//G385: __E_q_var_116(253)   => variable(139) & ~( __N_4(456) ).	 # conjunctive
	p(NT(253), (NT(139)) & ~(NT(456)));
//G386: q_var(250)           => __E_q_var_116(253).
	p(NT(250), (NT(253)));
//G387: __E_offsets_117(254) => _(11) ',' _(11) offset(244).
	p(NT(254), (NT(11)+T(14)+NT(11)+NT(244)));
//G388: __E_offsets_118(255) => null.
	p(NT(255), (nul));
//G389: __E_offsets_118(255) => __E_offsets_117(254) __E_offsets_118(255).
	p(NT(255), (NT(254)+NT(255)));
//G390: offsets(24)          => '[' _(11) offset(244) __E_offsets_118(255) _(11) ']'.
	p(NT(24), (T(24)+NT(11)+NT(244)+NT(255)+NT(11)+T(25)));
//G391: offset(244)          => integer(256).
	p(NT(244), (NT(256)));
//G392: offset(244)          => capture(20).
	p(NT(244), (NT(20)));
//G393: offset(244)          => shift(257).
	p(NT(244), (NT(257)));
//G394: __N_5(457)           => io_var(233).
	p(NT(457), (NT(233)));
//G395: __E_offset_119(258)  => variable(139) & ~( __N_5(457) ).	 # conjunctive
	p(NT(258), (NT(139)) & ~(NT(457)));
//G396: offset(244)          => __E_offset_119(258).
	p(NT(244), (NT(258)));
//G397: __E_shift_120(259)   => capture(20).
	p(NT(259), (NT(20)));
//G398: __N_6(458)           => io_var(233).
	p(NT(458), (NT(233)));
//G399: __E___E_shift_120_121(260) => variable(139) & ~( __N_6(458) ).	 # conjunctive
	p(NT(260), (NT(139)) & ~(NT(458)));
//G400: __E_shift_120(259)   => __E___E_shift_120_121(260).
	p(NT(259), (NT(260)));
//G401: shift(257)           => __E_shift_120(259) _(11) '-' _(11) num(206).
	p(NT(257), (NT(259)+NT(11)+T(28)+NT(11)+NT(206)));
//G402: num(206)             => digits(261).
	p(NT(206), (NT(261)));
//G403: __E_integer_122(262) => '-'.
	p(NT(262), (T(28)));
//G404: __E_integer_122(262) => null.
	p(NT(262), (nul));
//G405: integer(256)         => __E_integer_122(262) _(11) digits(261).
	p(NT(256), (NT(262)+NT(11)+NT(261)));
//G406: __E___E_uninterpreted_constant_123_124(265) => chars(249).
	p(NT(265), (NT(249)));
//G407: __E___E_uninterpreted_constant_123_124(265) => _(11).
	p(NT(265), (NT(11)));
//G408: __E_uninterpreted_constant_123(264) => __E___E_uninterpreted_constant_123_124(265) ':' chars(249).
	p(NT(264), (NT(265)+T(2)+NT(249)));
//G409: uninter_const_name(263) => __E_uninterpreted_constant_123(264).
	p(NT(263), (NT(264)));
//G410: uninterpreted_constant(238) => '<' uninter_const_name(263) _(11) '>'.
	p(NT(238), (T(20)+NT(263)+NT(11)+T(21)));
//G411: __E_bf_constant_125(267) => capture(20).
	p(NT(267), (NT(20)));
//G412: __E_bf_constant_125(267) => binding(268).
	p(NT(267), (NT(268)));
//G413: constant(266)        => __E_bf_constant_125(267).
	p(NT(266), (NT(267)));
//G414: __E_bf_constant_126(269) => _(11) ':' _(11) type(150).
	p(NT(269), (NT(11)+T(2)+NT(11)+NT(150)));
//G415: __E_bf_constant_126(269) => null.
	p(NT(269), (nul));
//G416: bf_constant(138)     => '{' _(11) constant(266) _(11) '}' __E_bf_constant_126(269).
	p(NT(138), (T(50)+NT(11)+NT(266)+NT(11)+T(51)+NT(269)));
//G417: __E___E___E_binding_127_128_129(274) => src_c(272).
	p(NT(274), (NT(272)));
//G418: __E___E___E_binding_127_128_129(274) => space(2).
	p(NT(274), (NT(2)));
//G419: __E___E___E_binding_127_128_130(275) => null.
	p(NT(275), (nul));
//G420: __E___E___E_binding_127_128_130(275) => __E___E___E_binding_127_128_129(274) __E___E___E_binding_127_128_130(275).
	p(NT(275), (NT(274)+NT(275)));
//G421: __E___E_binding_127_128(273) => __E___E___E_binding_127_128_130(275) src_c(272).
	p(NT(273), (NT(275)+NT(272)));
//G422: __E___E_binding_127_128(273) => null.
	p(NT(273), (nul));
//G423: __E_binding_127(271) => src_c(272) __E___E_binding_127_128(273).
	p(NT(271), (NT(272)+NT(273)));
//G424: source(270)          => __E_binding_127(271).
	p(NT(270), (NT(271)));
//G425: binding(268)         => source(270).
	p(NT(268), (NT(270)));
//G426: src_c(272)           => alnum(6).
	p(NT(272), (NT(6)));
//G427: __N_7(459)           => '{'.
	p(NT(459), (T(50)));
//G428: __N_8(460)           => '}'.
	p(NT(460), (T(51)));
//G429: src_c(272)           => ~( __N_7(459) ) & ~( __N_8(460) ) & punct(7).	 # conjunctive
	p(NT(272), ~(NT(459)) & ~(NT(460)) & (NT(7)));
//G430: __E_src_c_131(276)   => src_c(272).
	p(NT(276), (NT(272)));
//G431: __E_src_c_131(276)   => space(2).
	p(NT(276), (NT(2)));
//G432: __E_src_c_132(277)   => null.
	p(NT(277), (nul));
//G433: __E_src_c_132(277)   => __E_src_c_131(276) __E_src_c_132(277).
	p(NT(277), (NT(276)+NT(277)));
//G434: src_c(272)           => '{' __E_src_c_132(277) '}'.
	p(NT(272), (T(50)+NT(277)+T(51)));
//G435: type(150)            => bv_type(205).
	p(NT(150), (NT(205)));
//G436: type(150)            => chars(249).
	p(NT(150), (NT(249)));
//G437: __E_bv_type_133(278) => num(206).
	p(NT(278), (NT(206)));
//G438: __E_bv_type_133(278) => null.
	p(NT(278), (nul));
//G439: bv_type(205)         => 'b' 'v' __E_bv_type_133(278).
	p(NT(205), (T(7)+T(52)+NT(278)));
//G440: __E_chars_134(279)   => alnum(6).
	p(NT(279), (NT(6)));
//G441: __E_chars_134(279)   => '_'.
	p(NT(279), (T(35)));
//G442: __E_chars_135(280)   => null.
	p(NT(280), (nul));
//G443: __E_chars_135(280)   => __E_chars_134(279) __E_chars_135(280).
	p(NT(280), (NT(279)+NT(280)));
//G444: chars(249)           => alpha(5) __E_chars_135(280).
	p(NT(249), (NT(5)+NT(280)));
//G445: __E_digits_136(281)  => digit(3).
	p(NT(281), (NT(3)));
//G446: __E_digits_136(281)  => digit(3) __E_digits_136(281).
	p(NT(281), (NT(3)+NT(281)));
//G447: digits(261)          => __E_digits_136(281).
	p(NT(261), (NT(281)));
//G448: sym(22)              => chars(249).
	p(NT(22), (NT(249)));
//G449: __E_comment_137(283) => printable(8).
	p(NT(283), (NT(8)));
//G450: __E_comment_137(283) => '\t'.
	p(NT(283), (T(53)));
//G451: __E_comment_138(284) => null.
	p(NT(284), (nul));
//G452: __E_comment_138(284) => __E_comment_137(283) __E_comment_138(284).
	p(NT(284), (NT(283)+NT(284)));
//G453: __E_comment_139(285) => '\n'.
	p(NT(285), (T(54)));
//G454: __E_comment_139(285) => '\r'.
	p(NT(285), (T(55)));
//G455: __E_comment_139(285) => eof(1).
	p(NT(285), (NT(1)));
//G456: comment(282)         => '#' __E_comment_138(284) __E_comment_139(285).
	p(NT(282), (T(46)+NT(284)+NT(285)));
//G457: __E____140(286)      => space(2).
	p(NT(286), (NT(2)));
//G458: __E____140(286)      => comment(282).
	p(NT(286), (NT(282)));
//G459: __(27)               => __E____140(286) _(11).
	p(NT(27), (NT(286)+NT(11)));
//G460: __E___141(287)       => __(27).
	p(NT(287), (NT(27)));
//G461: __E___141(287)       => null.
	p(NT(287), (nul));
//G462: _(11)                => __E___141(287).
	p(NT(11), (NT(287)));
//G463: cli(288)             => _(11).
	p(NT(288), (NT(11)));
//G464: __E_cli_142(290)     => '.' _(11) cli_command(289) _(11).
	p(NT(290), (T(1)+NT(11)+NT(289)+NT(11)));
//G465: __E_cli_143(291)     => null.
	p(NT(291), (nul));
//G466: __E_cli_143(291)     => __E_cli_142(290) __E_cli_143(291).
	p(NT(291), (NT(290)+NT(291)));
//G467: cli(288)             => _(11) cli_command(289) _(11) __E_cli_143(291).
	p(NT(288), (NT(11)+NT(289)+NT(11)+NT(291)));
//G468: __E_cli_command_144(294) => 'q'.
	p(NT(294), (T(56)));
//G469: __E_cli_command_144(294) => 'q' 'u' 'i' 't'.
	p(NT(294), (T(56)+T(48)+T(10)+T(13)));
//G470: quit_sym(293)        => __E_cli_command_144(294).
	p(NT(293), (NT(294)));
//G471: quit_cmd(292)        => quit_sym(293).
	p(NT(292), (NT(293)));
//G472: cli_command(289)     => quit_cmd(292).
	p(NT(289), (NT(292)));
//G473: __E_cli_command_145(297) => 'v'.
	p(NT(297), (T(52)));
//G474: __E_cli_command_145(297) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(297), (T(52)+T(19)+T(11)+T(12)+T(10)+T(17)+T(45)));
//G475: version_sym(296)     => __E_cli_command_145(297).
	p(NT(296), (NT(297)));
//G476: version_cmd(295)     => version_sym(296).
	p(NT(295), (NT(296)));
//G477: cli_command(289)     => version_cmd(295).
	p(NT(289), (NT(295)));
//G478: __E_cli_command_146(300) => 'c'.
	p(NT(300), (T(8)));
//G479: __E_cli_command_146(300) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(300), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G480: clear_sym(299)       => __E_cli_command_146(300).
	p(NT(299), (NT(300)));
//G481: clear_cmd(298)       => clear_sym(299).
	p(NT(298), (NT(299)));
//G482: cli_command(289)     => clear_cmd(298).
	p(NT(289), (NT(298)));
//G483: __E___E_cli_command_147_148(304) => 'h'.
	p(NT(304), (T(47)));
//G484: __E___E_cli_command_147_148(304) => 'h' 'e' 'l' 'p'.
	p(NT(304), (T(47)+T(19)+T(6)+T(57)));
//G485: help_sym(303)        => __E___E_cli_command_147_148(304).
	p(NT(303), (NT(304)));
//G486: __E___E_cli_command_147_149(305) => __(27) help_arg(306).
	p(NT(305), (NT(27)+NT(306)));
//G487: __E___E_cli_command_147_149(305) => null.
	p(NT(305), (nul));
//G488: __E_cli_command_147(302) => help_sym(303) __E___E_cli_command_147_149(305).
	p(NT(302), (NT(303)+NT(305)));
//G489: help_cmd(301)        => __E_cli_command_147(302).
	p(NT(301), (NT(302)));
//G490: cli_command(289)     => help_cmd(301).
	p(NT(289), (NT(301)));
//G491: file_sym(309)        => 'f' 'i' 'l' 'e'.
	p(NT(309), (T(4)+T(10)+T(6)+T(19)));
//G492: __E_cli_command_150(308) => file_sym(309) __(27) q_string(310).
	p(NT(308), (NT(309)+NT(27)+NT(310)));
//G493: file_cmd(307)        => __E_cli_command_150(308).
	p(NT(307), (NT(308)));
//G494: cli_command(289)     => file_cmd(307).
	p(NT(289), (NT(307)));
//G495: valid_sym(313)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(313), (T(52)+T(5)+T(6)+T(10)+T(58)));
//G496: __E_cli_command_151(312) => valid_sym(313) __(27) normalize_cmd_arg(314).
	p(NT(312), (NT(313)+NT(27)+NT(314)));
//G497: valid_cmd(311)       => __E_cli_command_151(312).
	p(NT(311), (NT(312)));
//G498: cli_command(289)     => valid_cmd(311).
	p(NT(289), (NT(311)));
//G499: sat_sym(317)         => 's' 'a' 't'.
	p(NT(317), (T(12)+T(5)+T(13)));
//G500: __E_cli_command_152(316) => sat_sym(317) __(27) normalize_cmd_arg(314).
	p(NT(316), (NT(317)+NT(27)+NT(314)));
//G501: sat_cmd(315)         => __E_cli_command_152(316).
	p(NT(315), (NT(316)));
//G502: cli_command(289)     => sat_cmd(315).
	p(NT(289), (NT(315)));
//G503: unsat_sym(320)       => 'u' 'n' 's' 'a' 't'.
	p(NT(320), (T(48)+T(45)+T(12)+T(5)+T(13)));
//G504: __E_cli_command_153(319) => unsat_sym(320) __(27) normalize_cmd_arg(314).
	p(NT(319), (NT(320)+NT(27)+NT(314)));
//G505: unsat_cmd(318)       => __E_cli_command_153(319).
	p(NT(318), (NT(319)));
//G506: cli_command(289)     => unsat_cmd(318).
	p(NT(289), (NT(318)));
//G507: solve_sym(323)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(323), (T(12)+T(17)+T(6)+T(52)+T(19)));
//G508: __E___E_cli_command_154_155(324) => solve_options(325).
	p(NT(324), (NT(325)));
//G509: __E___E_cli_command_154_155(324) => null.
	p(NT(324), (nul));
//G510: __E_cli_command_154(322) => solve_sym(323) __E___E_cli_command_154_155(324) __(27) wff_cmd_arg(326).
	p(NT(322), (NT(323)+NT(324)+NT(27)+NT(326)));
//G511: solve_cmd(321)       => __E_cli_command_154(322).
	p(NT(321), (NT(322)));
//G512: cli_command(289)     => solve_cmd(321).
	p(NT(289), (NT(321)));
//G513: lgrs_sym(329)        => 'l' 'g' 'r' 's'.
	p(NT(329), (T(6)+T(59)+T(11)+T(12)));
//G514: __E_cli_command_156(328) => lgrs_sym(329) __(27) wff_cmd_arg(326).
	p(NT(328), (NT(329)+NT(27)+NT(326)));
//G515: lgrs_cmd(327)        => __E_cli_command_156(328).
	p(NT(327), (NT(328)));
//G516: cli_command(289)     => lgrs_cmd(327).
	p(NT(289), (NT(327)));
//G517: __E___E_cli_command_157_158(333) => 'r'.
	p(NT(333), (T(11)));
//G518: __E___E_cli_command_157_158(333) => 'r' 'u' 'n'.
	p(NT(333), (T(11)+T(48)+T(45)));
//G519: run_sym(332)         => __E___E_cli_command_157_158(333).
	p(NT(332), (NT(333)));
//G520: __E___E_cli_command_157_159(334) => memory(335).
	p(NT(334), (NT(335)));
//G521: __E___E_cli_command_157_159(334) => wff(14).
	p(NT(334), (NT(14)));
//G522: __E_cli_command_157(331) => run_sym(332) __(27) __E___E_cli_command_157_159(334).
	p(NT(331), (NT(332)+NT(27)+NT(334)));
//G523: run_cmd(330)         => __E_cli_command_157(331).
	p(NT(330), (NT(331)));
//G524: cli_command(289)     => run_cmd(330).
	p(NT(289), (NT(330)));
//G525: __E___E_cli_command_160_161(339) => 'n'.
	p(NT(339), (T(45)));
//G526: __E___E_cli_command_160_161(339) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(339), (T(45)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(60)+T(19)));
//G527: normalize_sym(338)   => __E___E_cli_command_160_161(339).
	p(NT(338), (NT(339)));
//G528: __E_cli_command_160(337) => normalize_sym(338) __(27) normalize_cmd_arg(314).
	p(NT(337), (NT(338)+NT(27)+NT(314)));
//G529: normalize_cmd(336)   => __E_cli_command_160(337).
	p(NT(336), (NT(337)));
//G530: cli_command(289)     => normalize_cmd(336).
	p(NT(289), (NT(336)));
//G531: __E___E_cli_command_162_163(343) => 's'.
	p(NT(343), (T(12)));
//G532: __E___E_cli_command_162_163(343) => 's' 'u' 'b' 's' 't'.
	p(NT(343), (T(12)+T(48)+T(7)+T(12)+T(13)));
//G533: __E___E_cli_command_162_163(343) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(343), (T(12)+T(48)+T(7)+T(12)+T(13)+T(10)+T(13)+T(48)+T(13)+T(19)));
//G534: subst_sym(342)       => __E___E_cli_command_162_163(343).
	p(NT(342), (NT(343)));
//G535: __E_cli_command_162(341) => subst_sym(342) __(27) nf_cmd_arg(344) _(11) '[' _(11) nf_cmd_arg(344) _(11) '/' _(11) nf_cmd_arg(344) _(11) ']'.
	p(NT(341), (NT(342)+NT(27)+NT(344)+NT(11)+T(24)+NT(11)+NT(344)+NT(11)+T(42)+NT(11)+NT(344)+NT(11)+T(25)));
//G536: subst_cmd(340)       => __E_cli_command_162(341).
	p(NT(340), (NT(341)));
//G537: cli_command(289)     => subst_cmd(340).
	p(NT(289), (NT(340)));
//G538: __E___E_cli_command_164_165(348) => 'i'.
	p(NT(348), (T(10)));
//G539: __E___E_cli_command_164_165(348) => 'i' 'n' 's' 't'.
	p(NT(348), (T(10)+T(45)+T(12)+T(13)));
//G540: __E___E_cli_command_164_165(348) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(348), (T(10)+T(45)+T(12)+T(13)+T(5)+T(45)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G541: inst_sym(347)        => __E___E_cli_command_164_165(348).
	p(NT(347), (NT(348)));
//G542: __E_cli_command_164(346) => inst_sym(347) __(27) inst_args(349).
	p(NT(346), (NT(347)+NT(27)+NT(349)));
//G543: inst_cmd(345)        => __E_cli_command_164(346).
	p(NT(345), (NT(346)));
//G544: cli_command(289)     => inst_cmd(345).
	p(NT(289), (NT(345)));
//G545: dnf_sym(352)         => 'd' 'n' 'f'.
	p(NT(352), (T(58)+T(45)+T(4)));
//G546: __E_cli_command_166(351) => dnf_sym(352) __(27) nf_cmd_arg(344).
	p(NT(351), (NT(352)+NT(27)+NT(344)));
//G547: dnf_cmd(350)         => __E_cli_command_166(351).
	p(NT(350), (NT(351)));
//G548: cli_command(289)     => dnf_cmd(350).
	p(NT(289), (NT(350)));
//G549: cnf_sym(355)         => 'c' 'n' 'f'.
	p(NT(355), (T(8)+T(45)+T(4)));
//G550: __E_cli_command_167(354) => cnf_sym(355) __(27) nf_cmd_arg(344).
	p(NT(354), (NT(355)+NT(27)+NT(344)));
//G551: cnf_cmd(353)         => __E_cli_command_167(354).
	p(NT(353), (NT(354)));
//G552: cli_command(289)     => cnf_cmd(353).
	p(NT(289), (NT(353)));
//G553: anf_sym(358)         => 'a' 'n' 'f'.
	p(NT(358), (T(5)+T(45)+T(4)));
//G554: __E_cli_command_168(357) => anf_sym(358) __(27) nf_cmd_arg(344).
	p(NT(357), (NT(358)+NT(27)+NT(344)));
//G555: anf_cmd(356)         => __E_cli_command_168(357).
	p(NT(356), (NT(357)));
//G556: cli_command(289)     => anf_cmd(356).
	p(NT(289), (NT(356)));
//G557: nnf_sym(361)         => 'n' 'n' 'f'.
	p(NT(361), (T(45)+T(45)+T(4)));
//G558: __E_cli_command_169(360) => nnf_sym(361) __(27) nf_cmd_arg(344).
	p(NT(360), (NT(361)+NT(27)+NT(344)));
//G559: nnf_cmd(359)         => __E_cli_command_169(360).
	p(NT(359), (NT(360)));
//G560: cli_command(289)     => nnf_cmd(359).
	p(NT(289), (NT(359)));
//G561: pnf_sym(364)         => 'p' 'n' 'f'.
	p(NT(364), (T(57)+T(45)+T(4)));
//G562: __E_cli_command_170(363) => pnf_sym(364) __(27) nf_cmd_arg(344).
	p(NT(363), (NT(364)+NT(27)+NT(344)));
//G563: pnf_cmd(362)         => __E_cli_command_170(363).
	p(NT(362), (NT(363)));
//G564: cli_command(289)     => pnf_cmd(362).
	p(NT(289), (NT(362)));
//G565: mnf_sym(367)         => 'm' 'n' 'f'.
	p(NT(367), (T(18)+T(45)+T(4)));
//G566: __E_cli_command_171(366) => mnf_sym(367) __(27) nf_cmd_arg(344).
	p(NT(366), (NT(367)+NT(27)+NT(344)));
//G567: mnf_cmd(365)         => __E_cli_command_171(366).
	p(NT(365), (NT(366)));
//G568: cli_command(289)     => mnf_cmd(365).
	p(NT(289), (NT(365)));
//G569: snf_sym(370)         => 's' 'n' 'f'.
	p(NT(370), (T(12)+T(45)+T(4)));
//G570: __E_cli_command_172(369) => snf_sym(370) __(27) nf_cmd_arg(344).
	p(NT(369), (NT(370)+NT(27)+NT(344)));
//G571: snf_cmd(368)         => __E_cli_command_172(369).
	p(NT(368), (NT(369)));
//G572: cli_command(289)     => snf_cmd(368).
	p(NT(289), (NT(368)));
//G573: onf_sym(373)         => 'o' 'n' 'f'.
	p(NT(373), (T(17)+T(45)+T(4)));
//G574: __E_cli_command_173(372) => onf_sym(373) __(27) variable(139) __(27) onf_cmd_arg(374).
	p(NT(372), (NT(373)+NT(27)+NT(139)+NT(27)+NT(374)));
//G575: onf_cmd(371)         => __E_cli_command_173(372).
	p(NT(371), (NT(372)));
//G576: cli_command(289)     => onf_cmd(371).
	p(NT(289), (NT(371)));
//G577: __E___E_cli_command_174_175(378) => 'd' 'e' 'f' 's'.
	p(NT(378), (T(58)+T(19)+T(4)+T(12)));
//G578: __E___E_cli_command_174_175(378) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(378), (T(58)+T(19)+T(4)+T(10)+T(45)+T(10)+T(13)+T(10)+T(17)+T(45)+T(12)));
//G579: def_sym(377)         => __E___E_cli_command_174_175(378).
	p(NT(377), (NT(378)));
//G580: __E_cli_command_174(376) => def_sym(377).
	p(NT(376), (NT(377)));
//G581: def_list_cmd(375)    => __E_cli_command_174(376).
	p(NT(375), (NT(376)));
//G582: cli_command(289)     => def_list_cmd(375).
	p(NT(289), (NT(375)));
//G583: __E_cli_command_176(380) => def_sym(377) __(27) number(381).
	p(NT(380), (NT(377)+NT(27)+NT(381)));
//G584: def_print_cmd(379)   => __E_cli_command_176(380).
	p(NT(379), (NT(380)));
//G585: cli_command(289)     => def_print_cmd(379).
	p(NT(289), (NT(379)));
//G586: def_rr_cmd(382)      => rec_relation(16).
	p(NT(382), (NT(16)));
//G587: cli_command(289)     => def_rr_cmd(382).
	p(NT(289), (NT(382)));
//G588: qelim_sym(385)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(385), (T(56)+T(19)+T(6)+T(10)+T(18)));
//G589: __E_cli_command_177(384) => qelim_sym(385) __(27) wff_cmd_arg(326).
	p(NT(384), (NT(385)+NT(27)+NT(326)));
//G590: qelim_cmd(383)       => __E_cli_command_177(384).
	p(NT(383), (NT(384)));
//G591: cli_command(289)     => qelim_cmd(383).
	p(NT(289), (NT(383)));
//G592: get_sym(388)         => 'g' 'e' 't'.
	p(NT(388), (T(59)+T(19)+T(13)));
//G593: __E___E_cli_command_178_179(389) => __(27) option_name(390).
	p(NT(389), (NT(27)+NT(390)));
//G594: __E___E_cli_command_178_179(389) => null.
	p(NT(389), (nul));
//G595: __E_cli_command_178(387) => get_sym(388) __E___E_cli_command_178_179(389).
	p(NT(387), (NT(388)+NT(389)));
//G596: get_cmd(386)         => __E_cli_command_178(387).
	p(NT(386), (NT(387)));
//G597: cli_command(289)     => get_cmd(386).
	p(NT(289), (NT(386)));
//G598: set_sym(393)         => 's' 'e' 't'.
	p(NT(393), (T(12)+T(19)+T(13)));
//G599: __E___E_cli_command_180_181(394) => __(27).
	p(NT(394), (NT(27)));
//G600: __E___E_cli_command_180_181(394) => _(11) '=' _(11).
	p(NT(394), (NT(11)+T(3)+NT(11)));
//G601: __E_cli_command_180(392) => set_sym(393) __(27) option_name(390) __E___E_cli_command_180_181(394) option_value(395).
	p(NT(392), (NT(393)+NT(27)+NT(390)+NT(394)+NT(395)));
//G602: set_cmd(391)         => __E_cli_command_180(392).
	p(NT(391), (NT(392)));
//G603: cli_command(289)     => set_cmd(391).
	p(NT(289), (NT(391)));
//G604: enable_sym(398)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(398), (T(19)+T(45)+T(5)+T(7)+T(6)+T(19)));
//G605: __E_cli_command_182(397) => enable_sym(398) __(27) option_name(390).
	p(NT(397), (NT(398)+NT(27)+NT(390)));
//G606: enable_cmd(396)      => __E_cli_command_182(397).
	p(NT(396), (NT(397)));
//G607: cli_command(289)     => enable_cmd(396).
	p(NT(289), (NT(396)));
//G608: disable_sym(401)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(401), (T(58)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G609: __E_cli_command_183(400) => disable_sym(401) __(27) option_name(390).
	p(NT(400), (NT(401)+NT(27)+NT(390)));
//G610: disable_cmd(399)     => __E_cli_command_183(400).
	p(NT(399), (NT(400)));
//G611: cli_command(289)     => disable_cmd(399).
	p(NT(289), (NT(399)));
//G612: toggle_sym(404)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(404), (T(13)+T(17)+T(59)+T(59)+T(6)+T(19)));
//G613: __E_cli_command_184(403) => toggle_sym(404) __(27) option_name(390).
	p(NT(403), (NT(404)+NT(27)+NT(390)));
//G614: toggle_cmd(402)      => __E_cli_command_184(403).
	p(NT(402), (NT(403)));
//G615: cli_command(289)     => toggle_cmd(402).
	p(NT(289), (NT(402)));
//G616: __E_cli_command_185(406) => type(150) __(27) in_var_name(240) _(11) '=' _(11) input_stream(407).
	p(NT(406), (NT(150)+NT(27)+NT(240)+NT(11)+T(3)+NT(11)+NT(407)));
//G617: def_input_cmd(405)   => __E_cli_command_185(406).
	p(NT(405), (NT(406)));
//G618: cli_command(289)     => def_input_cmd(405).
	p(NT(289), (NT(405)));
//G619: __E_cli_command_186(409) => type(150) __(27) out_var_name(245) _(11) '=' _(11) output_stream(410).
	p(NT(409), (NT(150)+NT(27)+NT(245)+NT(11)+T(3)+NT(11)+NT(410)));
//G620: def_output_cmd(408)  => __E_cli_command_186(409).
	p(NT(408), (NT(409)));
//G621: cli_command(289)     => def_output_cmd(408).
	p(NT(289), (NT(408)));
//G622: __E___E_cli_command_187_188(414) => 'h' 'i' 's' 't'.
	p(NT(414), (T(47)+T(10)+T(12)+T(13)));
//G623: __E___E_cli_command_187_188(414) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(414), (T(47)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G624: history_sym(413)     => __E___E_cli_command_187_188(414).
	p(NT(413), (NT(414)));
//G625: __E_cli_command_187(412) => history_sym(413).
	p(NT(412), (NT(413)));
//G626: history_list_cmd(411) => __E_cli_command_187(412).
	p(NT(411), (NT(412)));
//G627: cli_command(289)     => history_list_cmd(411).
	p(NT(289), (NT(411)));
//G628: __E_cli_command_189(416) => history_sym(413) __(27) memory(335).
	p(NT(416), (NT(413)+NT(27)+NT(335)));
//G629: history_print_cmd(415) => __E_cli_command_189(416).
	p(NT(415), (NT(416)));
//G630: cli_command(289)     => history_print_cmd(415).
	p(NT(289), (NT(415)));
//G631: __E_cli_command_190(418) => wff(14).
	p(NT(418), (NT(14)));
//G632: __E_cli_command_190(418) => bf(21).
	p(NT(418), (NT(21)));
//G633: history_store_cmd(417) => __E_cli_command_190(418).
	p(NT(417), (NT(418)));
//G634: cli_command(289)     => history_store_cmd(417).
	p(NT(289), (NT(417)));
//G635: __E_solve_options_191(419) => __(27) solve_option(420).
	p(NT(419), (NT(27)+NT(420)));
//G636: __E_solve_options_192(421) => null.
	p(NT(421), (nul));
//G637: __E_solve_options_192(421) => __E_solve_options_191(419) __E_solve_options_192(421).
	p(NT(421), (NT(419)+NT(421)));
//G638: solve_options(325)   => __E_solve_options_192(421).
	p(NT(325), (NT(421)));
//G639: __E_solve_option_193(422) => '-' '-' solver_mode(423).
	p(NT(422), (T(28)+T(28)+NT(423)));
//G640: solve_option(420)    => __E_solve_option_193(422).
	p(NT(420), (NT(422)));
//G641: __E_solve_option_194(424) => '-' '-' type(150).
	p(NT(424), (T(28)+T(28)+NT(150)));
//G642: solve_option(420)    => __E_solve_option_194(424).
	p(NT(420), (NT(424)));
//G643: __E_solver_mode_195(426) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(426), (T(18)+T(10)+T(45)+T(10)+T(18)+T(48)+T(18)));
//G644: __E_solver_mode_195(426) => 'm' 'i' 'n'.
	p(NT(426), (T(18)+T(10)+T(45)));
//G645: solver_mode_minimum(425) => __E_solver_mode_195(426).
	p(NT(425), (NT(426)));
//G646: solver_mode(423)     => solver_mode_minimum(425).
	p(NT(423), (NT(425)));
//G647: __E_solver_mode_196(428) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(428), (T(18)+T(5)+T(27)+T(10)+T(18)+T(48)+T(18)));
//G648: __E_solver_mode_196(428) => 'm' 'a' 'x'.
	p(NT(428), (T(18)+T(5)+T(27)));
//G649: solver_mode_maximum(427) => __E_solver_mode_196(428).
	p(NT(427), (NT(428)));
//G650: solver_mode(423)     => solver_mode_maximum(427).
	p(NT(423), (NT(427)));
//G651: __E_input_stream_197(429) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(430) _(11) ')'.
	p(NT(429), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(430)+NT(11)+T(16)));
//G652: input_stream(407)    => __E_input_stream_197(429).
	p(NT(407), (NT(429)));
//G653: console_sym(431)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(431), (T(8)+T(17)+T(45)+T(12)+T(17)+T(6)+T(19)));
//G654: input_stream(407)    => console_sym(431).
	p(NT(407), (NT(431)));
//G655: __E_output_stream_198(432) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(430) _(11) ')'.
	p(NT(432), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(430)+NT(11)+T(16)));
//G656: output_stream(410)   => __E_output_stream_198(432).
	p(NT(410), (NT(432)));
//G657: output_stream(410)   => console_sym(431).
	p(NT(410), (NT(431)));
//G658: q_file_name(430)     => '"' file_name(433) '"'.
	p(NT(430), (T(61)+NT(433)+T(61)));
//G659: __E_file_name_199(434) => printable(8).
	p(NT(434), (NT(8)));
//G660: __E_file_name_199(434) => printable(8) __E_file_name_199(434).
	p(NT(434), (NT(8)+NT(434)));
//G661: file_name(433)       => __E_file_name_199(434).
	p(NT(433), (NT(434)));
//G662: __E_option_name_200(435) => alnum(6).
	p(NT(435), (NT(6)));
//G663: __E_option_name_200(435) => alnum(6) __E_option_name_200(435).
	p(NT(435), (NT(6)+NT(435)));
//G664: option_name(390)     => __E_option_name_200(435).
	p(NT(390), (NT(435)));
//G665: __E_option_value_201(436) => alnum(6).
	p(NT(436), (NT(6)));
//G666: __E_option_value_201(436) => alnum(6) __E_option_value_201(436).
	p(NT(436), (NT(6)+NT(436)));
//G667: option_value(395)    => __E_option_value_201(436).
	p(NT(395), (NT(436)));
//G668: number(381)          => digits(261).
	p(NT(381), (NT(261)));
//G669: bf_cmd_arg(437)      => memory(335).
	p(NT(437), (NT(335)));
//G670: bf_cmd_arg(437)      => bf(21).
	p(NT(437), (NT(21)));
//G671: wff_cmd_arg(326)     => memory(335).
	p(NT(326), (NT(335)));
//G672: wff_cmd_arg(326)     => wff(14).
	p(NT(326), (NT(14)));
//G673: nf_cmd_arg(344)      => memory(335).
	p(NT(344), (NT(335)));
//G674: nf_cmd_arg(344)      => ref(18).
	p(NT(344), (NT(18)));
//G675: nf_cmd_arg(344)      => wff(14).
	p(NT(344), (NT(14)));
//G676: nf_cmd_arg(344)      => bf(21).
	p(NT(344), (NT(21)));
//G677: onf_cmd_arg(374)     => memory(335).
	p(NT(374), (NT(335)));
//G678: onf_cmd_arg(374)     => wff(14).
	p(NT(374), (NT(14)));
//G679: normalize_cmd_arg(314) => memory(335).
	p(NT(314), (NT(335)));
//G680: normalize_cmd_arg(314) => rr(10).
	p(NT(314), (NT(10)));
//G681: normalize_cmd_arg(314) => ref(18).
	p(NT(314), (NT(18)));
//G682: normalize_cmd_arg(314) => wff(14).
	p(NT(314), (NT(14)));
//G683: normalize_cmd_arg(314) => bf(21).
	p(NT(314), (NT(21)));
//G684: inst_args(349)       => wff_cmd_arg(326) _(11) '[' _(11) variable(139) _(11) '/' _(11) bf_cmd_arg(437) _(11) ']'.
	p(NT(349), (NT(326)+NT(11)+T(24)+NT(11)+NT(139)+NT(11)+T(42)+NT(11)+NT(437)+NT(11)+T(25)));
//G685: inst_args(349)       => bf_cmd_arg(437) _(11) '[' _(11) variable(139) _(11) '/' _(11) bf_cmd_arg(437) _(11) ']'.
	p(NT(349), (NT(437)+NT(11)+T(24)+NT(11)+NT(139)+NT(11)+T(42)+NT(11)+NT(437)+NT(11)+T(25)));
//G686: help_arg(306)        => help_sym(303).
	p(NT(306), (NT(303)));
//G687: help_arg(306)        => version_sym(296).
	p(NT(306), (NT(296)));
//G688: help_arg(306)        => quit_sym(293).
	p(NT(306), (NT(293)));
//G689: help_arg(306)        => clear_sym(299).
	p(NT(306), (NT(299)));
//G690: help_arg(306)        => get_sym(388).
	p(NT(306), (NT(388)));
//G691: help_arg(306)        => set_sym(393).
	p(NT(306), (NT(393)));
//G692: help_arg(306)        => enable_sym(398).
	p(NT(306), (NT(398)));
//G693: help_arg(306)        => disable_sym(401).
	p(NT(306), (NT(401)));
//G694: help_arg(306)        => toggle_sym(404).
	p(NT(306), (NT(404)));
//G695: help_arg(306)        => file_sym(309).
	p(NT(306), (NT(309)));
//G696: help_arg(306)        => history_sym(413).
	p(NT(306), (NT(413)));
//G697: help_arg(306)        => abs_memory_sym(438).
	p(NT(306), (NT(438)));
//G698: help_arg(306)        => rel_memory_sym(439).
	p(NT(306), (NT(439)));
//G699: help_arg(306)        => selection_sym(440).
	p(NT(306), (NT(440)));
//G700: help_arg(306)        => def_sym(377).
	p(NT(306), (NT(377)));
//G701: help_arg(306)        => inst_sym(347).
	p(NT(306), (NT(347)));
//G702: help_arg(306)        => subst_sym(342).
	p(NT(306), (NT(342)));
//G703: help_arg(306)        => normalize_sym(338).
	p(NT(306), (NT(338)));
//G704: help_arg(306)        => execute_sym(441).
	p(NT(306), (NT(441)));
//G705: help_arg(306)        => solve_sym(323).
	p(NT(306), (NT(323)));
//G706: help_arg(306)        => valid_sym(313).
	p(NT(306), (NT(313)));
//G707: help_arg(306)        => sat_sym(317).
	p(NT(306), (NT(317)));
//G708: help_arg(306)        => unsat_sym(320).
	p(NT(306), (NT(320)));
//G709: help_arg(306)        => run_sym(332).
	p(NT(306), (NT(332)));
//G710: help_arg(306)        => dnf_sym(352).
	p(NT(306), (NT(352)));
//G711: help_arg(306)        => cnf_sym(355).
	p(NT(306), (NT(355)));
//G712: help_arg(306)        => anf_sym(358).
	p(NT(306), (NT(358)));
//G713: help_arg(306)        => snf_sym(370).
	p(NT(306), (NT(370)));
//G714: help_arg(306)        => nnf_sym(361).
	p(NT(306), (NT(361)));
//G715: help_arg(306)        => pnf_sym(364).
	p(NT(306), (NT(364)));
//G716: help_arg(306)        => mnf_sym(367).
	p(NT(306), (NT(367)));
//G717: help_arg(306)        => onf_sym(373).
	p(NT(306), (NT(373)));
//G718: help_arg(306)        => qelim_sym(385).
	p(NT(306), (NT(385)));
//G719: __E___E_help_arg_202_203(444) => 's'.
	p(NT(444), (T(12)));
//G720: __E___E_help_arg_202_203(444) => null.
	p(NT(444), (nul));
//G721: __E_help_arg_202(443) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_202_203(444).
	p(NT(443), (T(19)+T(27)+T(5)+T(18)+T(57)+T(6)+T(19)+NT(444)));
//G722: examples_sym(442)    => __E_help_arg_202(443).
	p(NT(442), (NT(443)));
//G723: help_arg(306)        => examples_sym(442).
	p(NT(306), (NT(442)));
//G724: __E_memory_204(446)  => '%'.
	p(NT(446), (T(43)));
//G725: rel_memory_sym(439)  => '%' '-'.
	p(NT(439), (T(43)+T(28)));
//G726: memory_id(448)       => digits(261).
	p(NT(448), (NT(261)));
//G727: __E___E_memory_204_205(447) => memory_id(448).
	p(NT(447), (NT(448)));
//G728: __E___E_memory_204_205(447) => null.
	p(NT(447), (nul));
//G729: __E_memory_204(446)  => rel_memory_sym(439) __E___E_memory_204_205(447).
	p(NT(446), (NT(439)+NT(447)));
//G730: rel_memory(445)      => __E_memory_204(446).
	p(NT(445), (NT(446)));
//G731: memory(335)          => rel_memory(445).
	p(NT(335), (NT(445)));
//G732: abs_memory_sym(438)  => '%'.
	p(NT(438), (T(43)));
//G733: __E_memory_206(450)  => abs_memory_sym(438) memory_id(448).
	p(NT(450), (NT(438)+NT(448)));
//G734: abs_memory(449)      => __E_memory_206(450).
	p(NT(449), (NT(450)));
//G735: memory(335)          => abs_memory(449).
	p(NT(335), (NT(449)));
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
		__E_bv_70, bv_add, __E_bv_71, bv_mul, __E_bv_72, bv_div, __E_bv_73, bv_mod, __E_bv_74, bv_sub, 
		__E_bv_75, bv_neg, bv_and, __E_bv_76, bv_nand, __E_bv_77, bv_or, __E_bv_78, bv_nor, __E_bv_79, 
		bv_xor, __E_bv_80, bv_xnor, __E_bv_81, bv_rotate_left, __E_bv_82, bv_rotate_right, __E_bv_83, bv_min, __E_bv_84, 
		bv_max, __E_bv_85, bitvector, bits, __E_bitvector_86, bv_type, num, __E_bitvector_87, hexnum, __E_bitvector_88, 
		__E_bits_89, __E_bits_90, __E_hexnum_91, ctn_neq, __E_constraint_92, ctnvar, ctn_eq, __E_constraint_93, ctn_greater_equal, __E_constraint_94, 
		ctn_greater, __E_constraint_95, ctn_less_equal, __E_constraint_96, ctn_less, __E_constraint_97, __E_variable_98, __E_variable_99, __E_variable_100, __E_variable_101, 
		__E_variable_102, __E_variable_103, __E_variable_104, io_var, __E_variable_105, in, out, __E_variable_106, uninterpreted_constant, __E_variable_107, 
		in_var_name, __E_in_108, __E___E_in_108_109, __E___E___E_in_108_109_110, offset, out_var_name, __E_out_111, __E___E_out_111_112, __E___E___E_out_111_112_113, chars, 
		q_var, __E_q_vars_114, __E_q_vars_115, __E_q_var_116, __E_offsets_117, __E_offsets_118, integer, shift, __E_offset_119, __E_shift_120, 
		__E___E_shift_120_121, digits, __E_integer_122, uninter_const_name, __E_uninterpreted_constant_123, __E___E_uninterpreted_constant_123_124, constant, __E_bf_constant_125, binding, __E_bf_constant_126, 
		source, __E_binding_127, src_c, __E___E_binding_127_128, __E___E___E_binding_127_128_129, __E___E___E_binding_127_128_130, __E_src_c_131, __E_src_c_132, __E_bv_type_133, __E_chars_134, 
		__E_chars_135, __E_digits_136, comment, __E_comment_137, __E_comment_138, __E_comment_139, __E____140, __E___141, cli, cli_command, 
		__E_cli_142, __E_cli_143, quit_cmd, quit_sym, __E_cli_command_144, version_cmd, version_sym, __E_cli_command_145, clear_cmd, clear_sym, 
		__E_cli_command_146, help_cmd, __E_cli_command_147, help_sym, __E___E_cli_command_147_148, __E___E_cli_command_147_149, help_arg, file_cmd, __E_cli_command_150, file_sym, 
		q_string, valid_cmd, __E_cli_command_151, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_152, sat_sym, unsat_cmd, __E_cli_command_153, 
		unsat_sym, solve_cmd, __E_cli_command_154, solve_sym, __E___E_cli_command_154_155, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_156, lgrs_sym, 
		run_cmd, __E_cli_command_157, run_sym, __E___E_cli_command_157_158, __E___E_cli_command_157_159, memory, normalize_cmd, __E_cli_command_160, normalize_sym, __E___E_cli_command_160_161, 
		subst_cmd, __E_cli_command_162, subst_sym, __E___E_cli_command_162_163, nf_cmd_arg, inst_cmd, __E_cli_command_164, inst_sym, __E___E_cli_command_164_165, inst_args, 
		dnf_cmd, __E_cli_command_166, dnf_sym, cnf_cmd, __E_cli_command_167, cnf_sym, anf_cmd, __E_cli_command_168, anf_sym, nnf_cmd, 
		__E_cli_command_169, nnf_sym, pnf_cmd, __E_cli_command_170, pnf_sym, mnf_cmd, __E_cli_command_171, mnf_sym, snf_cmd, __E_cli_command_172, 
		snf_sym, onf_cmd, __E_cli_command_173, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_174, def_sym, __E___E_cli_command_174_175, def_print_cmd, 
		__E_cli_command_176, number, def_rr_cmd, qelim_cmd, __E_cli_command_177, qelim_sym, get_cmd, __E_cli_command_178, get_sym, __E___E_cli_command_178_179, 
		option_name, set_cmd, __E_cli_command_180, set_sym, __E___E_cli_command_180_181, option_value, enable_cmd, __E_cli_command_182, enable_sym, disable_cmd, 
		__E_cli_command_183, disable_sym, toggle_cmd, __E_cli_command_184, toggle_sym, def_input_cmd, __E_cli_command_185, input_stream, def_output_cmd, __E_cli_command_186, 
		output_stream, history_list_cmd, __E_cli_command_187, history_sym, __E___E_cli_command_187_188, history_print_cmd, __E_cli_command_189, history_store_cmd, __E_cli_command_190, __E_solve_options_191, 
		solve_option, __E_solve_options_192, __E_solve_option_193, solver_mode, __E_solve_option_194, solver_mode_minimum, __E_solver_mode_195, solver_mode_maximum, __E_solver_mode_196, __E_input_stream_197, 
		q_file_name, console_sym, __E_output_stream_198, file_name, __E_file_name_199, __E_option_name_200, __E_option_value_201, bf_cmd_arg, abs_memory_sym, rel_memory_sym, 
		selection_sym, execute_sym, examples_sym, __E_help_arg_202, __E___E_help_arg_202_203, rel_memory, __E_memory_204, __E___E_memory_204_205, memory_id, abs_memory, 
		__E_memory_206, named_binding, __N_0, __N_1, __N_2, __N_3, __N_4, __N_5, __N_6, __N_7, 
		__N_8, 
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
