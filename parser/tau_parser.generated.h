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
	"spec", "_", "definitions", "main", "wff", "__E_definitions_0", "__E___E_definitions_0_1", "rec_relation", "input_def", "output_def", 
	"__E_definitions_2", "rec_relations", "__E_rec_relations_3", "__E_rec_relations_4", "ref", "__E_rec_relation_5", "capture", "bf", "io_var_name", "__E_input_def_6", 
	"typed", "__", "stream", "__E_output_def_7", "__E_output_def_8", "q_file_name", "console_sym", "sym", "__E_ref_9", "offsets", 
	"ref_args", "__E_ref_10", "fp_fallback", "first_sym", "last_sym", "__E_ref_args_11", "__E___E_ref_args_11_12", "ref_arg", "__E___E_ref_args_11_13", "__E___E_ref_args_11_14", 
	"library", "rules", "__E_library_15", "__E___E_library_15_16", "rule", "__E___E___E_library_15_16_17", "wff_rule", "bf_rule", "__E___E_library_15_18", "wff_matcher", 
	"wff_body", "bf_matcher", "bf_body", "builder", "builder_head", "builder_body", "__E_builder_head_19", "__E_builder_head_20", "bf_builder_body", "__E_builder_body_21", 
	"wff_builder_body", "__E_builder_body_22", "tau_constant_source", "__E_tau_constant_source_23", "wff_parenthesis", "__E_wff_24", "wff_sometimes", "__E_wff_25", "__E___E_wff_25_26", "wff_always", 
	"__E_wff_27", "__E___E_wff_27_28", "wff_conditional", "__E_wff_29", "wff_all", "__E_wff_30", "q_vars", "wff_ex", "__E_wff_31", "wff_ref", 
	"constraint", "wff_imply", "__E_wff_32", "wff_rimply", "__E_wff_33", "wff_equiv", "__E_wff_34", "wff_or", "__E_wff_35", "wff_xor", 
	"__E_wff_36", "wff_and", "__E_wff_37", "wff_neg", "__E_wff_38", "wff_t", "wff_f", "bf_interval", "__E_wff_39", "bf_eq", 
	"__E_wff_40", "bf_neq", "__E_wff_41", "bf_lteq", "__E_wff_42", "bf_nlteq", "__E_wff_43", "bf_gt", "__E_wff_44", "bf_ngt", 
	"__E_wff_45", "bf_gteq", "__E_wff_46", "bf_ngteq", "__E_wff_47", "bf_lt", "__E_wff_48", "bf_nlt", "__E_wff_49", "bv_eq", 
	"__E_wff_50", "bv", "bv_neq", "__E_wff_51", "bv_less_equal", "__E_wff_52", "bv_nleq", "__E_wff_53", "bv_greater", "__E_wff_54", 
	"bv_ngreater", "__E_wff_55", "bv_greater_equal", "__E_wff_56", "bv_ngeq", "__E_wff_57", "bv_less", "__E_wff_58", "bv_nless", "__E_wff_59", 
	"bf_parenthesis", "__E_bf_60", "bf_constant", "variable", "bf_splitter", "__E_bf_61", "bf_ref", "bf_or", "__E_bf_62", "bf_xor", 
	"__E_bf_63", "bf_t", "__E_bf_64", "__E___E_bf_64_65", "bf_f", "__E_bf_66", "__E___E_bf_66_67", "bf_and", "__E_bf_68", "__E___E_bf_68_69", 
	"bf_neg", "__E_bf_70", "bf_neg_oprnd", "__E___E_bf_70_71", "bf_and_nosep", "__E_bf_72", "bf_and_nosep_1st_oprnd", "__E___E_bf_72_73", "bf_and_nosep_2nd_oprnd", "__E___E_bf_72_74", 
	"bv_parenthesis", "__E_bv_75", "bv_checked", "__E_bv_76", "bv_add", "__E_bv_77", "bv_mul", "__E_bv_78", "bv_div", "__E_bv_79", 
	"bv_mod", "__E_bv_80", "bv_sub", "__E_bv_81", "bv_neg", "bv_and", "__E_bv_82", "bv_nand", "__E_bv_83", "bv_or", 
	"__E_bv_84", "bv_nor", "__E_bv_85", "bv_xor", "__E_bv_86", "bv_xnor", "__E_bv_87", "bv_rotate_left", "__E_bv_88", "bv_rotate_right", 
	"__E_bv_89", "bv_min", "__E_bv_90", "bv_max", "__E_bv_91", "bitvector", "bits", "__E_bitvector_92", "bv_type", "num", 
	"__E_bitvector_93", "hexnum", "__E_bitvector_94", "__E_bits_95", "__E_bits_96", "__E_hexnum_97", "ctn_neq", "__E_constraint_98", "ctnvar", "ctn_eq", 
	"__E_constraint_99", "ctn_gteq", "__E_constraint_100", "ctn_gt", "__E_constraint_101", "ctn_lteq", "__E_constraint_102", "ctn_lt", "__E_constraint_103", "__E_bf_constant_104", 
	"source", "__E_bf_constant_105", "src_c", "__E_source_106", "__E___E_source_106_107", "__E___E_source_106_108", "__E_src_c_109", "__E_src_c_110", "__E_variable_111", "uconst", 
	"io_var", "var_name", "__E_variable_112", "__E_var_name_113", "__E_var_name_114", "__E_var_name_115", "__E_var_name_116", "__E_var_name_117", "offset", "chars", 
	"uconst_name", "__E_uconst_118", "__E___E_uconst_118_119", "q_var", "__E_q_vars_120", "__E_q_vars_121", "__E_offsets_122", "__E_offsets_123", "integer", "shift", 
	"__E_offset_124", "__E_shift_125", "__E___E_shift_125_126", "__E_chars_127", "__E_chars_128", "digits", "__E_digits_129", "__E_integer_130", "type", "__E_bv_type_131", 
	"scope_id", "comment", "__E_comment_132", "__E_comment_133", "__E_comment_134", "cli", "cli_command", "__E_cli_135", "__E_cli_136", "quit_cmd", 
	"quit_sym", "__E_cli_command_137", "version_cmd", "version_sym", "__E_cli_command_138", "clear_cmd", "clear_sym", "__E_cli_command_139", "help_cmd", "__E_cli_command_140", 
	"help_sym", "__E___E_cli_command_140_141", "__E___E_cli_command_140_142", "help_arg", "file_cmd", "__E_cli_command_143", "file_sym", "q_string", "valid_cmd", "__E_cli_command_144", 
	"valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_145", "sat_sym", "unsat_cmd", "__E_cli_command_146", "unsat_sym", "solve_cmd", "__E_cli_command_147", 
	"solve_sym", "__E___E_cli_command_147_148", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_149", "lgrs_sym", "run_cmd", "__E_cli_command_150", "run_sym", 
	"__E___E_cli_command_150_151", "__E___E_cli_command_150_152", "history", "normalize_cmd", "__E_cli_command_153", "normalize_sym", "__E___E_cli_command_153_154", "subst_cmd", "__E_cli_command_155", "subst_sym", 
	"__E___E_cli_command_155_156", "nf_cmd_arg", "inst_cmd", "__E_cli_command_157", "inst_sym", "__E___E_cli_command_157_158", "inst_args", "dnf_cmd", "__E_cli_command_159", "dnf_sym", 
	"cnf_cmd", "__E_cli_command_160", "cnf_sym", "anf_cmd", "__E_cli_command_161", "anf_sym", "nnf_cmd", "__E_cli_command_162", "nnf_sym", "pnf_cmd", 
	"__E_cli_command_163", "pnf_sym", "mnf_cmd", "__E_cli_command_164", "mnf_sym", "snf_cmd", "__E_cli_command_165", "snf_sym", "onf_cmd", "__E_cli_command_166", 
	"onf_sym", "onf_cmd_arg", "qelim_cmd", "__E_cli_command_167", "qelim_sym", "get_cmd", "__E_cli_command_168", "get_sym", "__E___E_cli_command_168_169", "option_name", 
	"set_cmd", "__E_cli_command_170", "set_sym", "__E___E_cli_command_170_171", "option_value", "enable_cmd", "__E_cli_command_172", "enable_sym", "disable_cmd", "__E_cli_command_173", 
	"disable_sym", "toggle_cmd", "__E_cli_command_174", "toggle_sym", "def_list_cmd", "__E_cli_command_175", "def_sym", "__E___E_cli_command_175_176", "def_print_cmd", "__E_cli_command_177", 
	"def_rr_cmd", "def_input_cmd", "def_output_cmd", "history_list_cmd", "__E_cli_command_178", "history_sym", "__E___E_cli_command_178_179", "history_print_cmd", "__E_cli_command_180", "history_store_cmd", 
	"__E_cli_command_181", "__E_solve_options_182", "solve_option", "__E_solve_options_183", "__E_solve_option_184", "solver_mode", "solver_mode_minimum", "__E_solver_mode_185", "solver_mode_maximum", "__E_solver_mode_186", 
	"file_name", "__E_file_name_187", "__E_option_name_188", "__E_option_value_189", "bf_cmd_arg", "abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", "examples_sym", 
	"__E_help_arg_190", "__E___E_help_arg_190_191", "rel_history", "__E_history_192", "__E___E_history_192_193", "history_id", "abs_history", "__E_history_194", "__N_0", "__N_1", 
	"__N_2", "__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', 'a', 'b', 'k', 
	'r', ',', 'm', '<', '>', 'w', 'y', '[', ']', '?', 
	'x', '-', '|', '^', '&', '!', 'T', 'F', '_', 'S', 
	'+', '1', '0', '\'', '*', '/', '%', '~', '#', '{', 
	'}', '$', 'v', '\t', '\n', '\r', 'q', 'h', 'p', 'd', 
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
			11, 31
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			26, 28, 37, 216, 218, 221, 228, 240, 251, 260,
			268, 275, 278, 379, 384, 420
		},
		.to_inline = {
			{ 14, 74, 14 },
			{ 27, 150, 27 },
			{ 30 },
			{ 131, 180, 131 },
			{ 172, 150, 27 },
			{ 174, 150, 167 },
			{ 176, 150, 27 },
			{ 178, 150, 27 },
			{ 242 },
			{ 249 },
			{ 249, 260 },
			{ 259 },
			{ 263 },
			{ 311 },
			{ 323 },
			{ 341 },
			{ 346 },
			{ 371 },
			{ 424 }
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
//G0:   start(9)             => spec(10) _(11).
	p(NT(9), (NT(10)+NT(11)));
//G1:   main(13)             => wff(14).
	p(NT(13), (NT(14)));
//G2:   spec(10)             => definitions(12) _(11) main(13) _(11) '.'.
	p(NT(10), (NT(12)+NT(11)+NT(13)+NT(11)+T(1)));
//G3:   __E___E_definitions_0_1(16) => rec_relation(17).
	p(NT(16), (NT(17)));
//G4:   __E___E_definitions_0_1(16) => input_def(18).
	p(NT(16), (NT(18)));
//G5:   __E___E_definitions_0_1(16) => output_def(19).
	p(NT(16), (NT(19)));
//G6:   __E_definitions_0(15) => _(11) __E___E_definitions_0_1(16) _(11) '.'.
	p(NT(15), (NT(11)+NT(16)+NT(11)+T(1)));
//G7:   __E_definitions_2(20) => null.
	p(NT(20), (nul));
//G8:   __E_definitions_2(20) => __E_definitions_0(15) __E_definitions_2(20).
	p(NT(20), (NT(15)+NT(20)));
//G9:   definitions(12)      => __E_definitions_2(20).
	p(NT(12), (NT(20)));
//G10:  __E_rec_relations_3(22) => _(11) rec_relation(17) _(11) '.'.
	p(NT(22), (NT(11)+NT(17)+NT(11)+T(1)));
//G11:  __E_rec_relations_4(23) => null.
	p(NT(23), (nul));
//G12:  __E_rec_relations_4(23) => __E_rec_relations_3(22) __E_rec_relations_4(23).
	p(NT(23), (NT(22)+NT(23)));
//G13:  rec_relations(21)    => __E_rec_relations_4(23).
	p(NT(21), (NT(23)));
//G14:  __E_rec_relation_5(25) => capture(26).
	p(NT(25), (NT(26)));
//G15:  __E_rec_relation_5(25) => ref(24).
	p(NT(25), (NT(24)));
//G16:  __E_rec_relation_5(25) => wff(14).
	p(NT(25), (NT(14)));
//G17:  __E_rec_relation_5(25) => bf(27).
	p(NT(25), (NT(27)));
//G18:  rec_relation(17)     => ref(24) _(11) ':' '=' _(11) __E_rec_relation_5(25).
	p(NT(17), (NT(24)+NT(11)+T(2)+T(3)+NT(11)+NT(25)));
//G19:  __E_input_def_6(29)  => typed(30).
	p(NT(29), (NT(30)));
//G20:  __E_input_def_6(29)  => null.
	p(NT(29), (nul));
//G21:  input_def(18)        => io_var_name(28) __E_input_def_6(29) _(11) '=' _(11) 'i' 'n' __(31) stream(32).
	p(NT(18), (NT(28)+NT(29)+NT(11)+T(3)+NT(11)+T(4)+T(5)+NT(31)+NT(32)));
//G22:  __E_output_def_7(33) => typed(30).
	p(NT(33), (NT(30)));
//G23:  __E_output_def_7(33) => null.
	p(NT(33), (nul));
//G24:  __E_output_def_8(34) => __(31) stream(32).
	p(NT(34), (NT(31)+NT(32)));
//G25:  __E_output_def_8(34) => null.
	p(NT(34), (nul));
//G26:  output_def(19)       => io_var_name(28) __E_output_def_7(33) _(11) '=' _(11) 'o' 'u' 't' __E_output_def_8(34).
	p(NT(19), (NT(28)+NT(33)+NT(11)+T(3)+NT(11)+T(6)+T(7)+T(8)+NT(34)));
//G27:  stream(32)           => 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(35) _(11) ')'.
	p(NT(32), (T(9)+T(4)+T(10)+T(11)+NT(11)+T(12)+NT(11)+NT(35)+NT(11)+T(13)));
//G28:  console_sym(36)      => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(36), (T(14)+T(6)+T(5)+T(15)+T(6)+T(10)+T(11)));
//G29:  stream(32)           => console_sym(36).
	p(NT(32), (NT(36)));
//G30:  __E_ref_9(38)        => offsets(39).
	p(NT(38), (NT(39)));
//G31:  __E_ref_9(38)        => null.
	p(NT(38), (nul));
//G32:  __E_ref_10(41)       => _(11) 'f' 'a' 'l' 'l' 'b' 'a' 'c' 'k' __(31) fp_fallback(42).
	p(NT(41), (NT(11)+T(9)+T(16)+T(10)+T(10)+T(17)+T(16)+T(14)+T(18)+NT(31)+NT(42)));
//G33:  __E_ref_10(41)       => null.
	p(NT(41), (nul));
//G34:  ref(24)              => sym(37) __E_ref_9(38) ref_args(40) __E_ref_10(41).
	p(NT(24), (NT(37)+NT(38)+NT(40)+NT(41)));
//G35:  first_sym(43)        => 'f' 'i' 'r' 's' 't'.
	p(NT(43), (T(9)+T(4)+T(19)+T(15)+T(8)));
//G36:  fp_fallback(42)      => first_sym(43).
	p(NT(42), (NT(43)));
//G37:  last_sym(44)         => 'l' 'a' 's' 't'.
	p(NT(44), (T(10)+T(16)+T(15)+T(8)));
//G38:  fp_fallback(42)      => last_sym(44).
	p(NT(42), (NT(44)));
//G39:  fp_fallback(42)      => capture(26).
	p(NT(42), (NT(26)));
//G40:  fp_fallback(42)      => ref(24).
	p(NT(42), (NT(24)));
//G41:  fp_fallback(42)      => wff(14).
	p(NT(42), (NT(14)));
//G42:  fp_fallback(42)      => bf(27).
	p(NT(42), (NT(27)));
//G43:  ref_arg(47)          => bf(27).
	p(NT(47), (NT(27)));
//G44:  __E___E_ref_args_11_12(46) => _(11) ref_arg(47).
	p(NT(46), (NT(11)+NT(47)));
//G45:  __E___E_ref_args_11_13(48) => _(11) ',' _(11) ref_arg(47).
	p(NT(48), (NT(11)+T(20)+NT(11)+NT(47)));
//G46:  __E___E_ref_args_11_14(49) => null.
	p(NT(49), (nul));
//G47:  __E___E_ref_args_11_14(49) => __E___E_ref_args_11_13(48) __E___E_ref_args_11_14(49).
	p(NT(49), (NT(48)+NT(49)));
//G48:  __E_ref_args_11(45)  => __E___E_ref_args_11_12(46) __E___E_ref_args_11_14(49).
	p(NT(45), (NT(46)+NT(49)));
//G49:  __E_ref_args_11(45)  => null.
	p(NT(45), (nul));
//G50:  ref_args(40)         => '(' __E_ref_args_11(45) _(11) ')'.
	p(NT(40), (T(12)+NT(45)+NT(11)+T(13)));
//G51:  __E___E___E_library_15_16_17(55) => wff_rule(56).
	p(NT(55), (NT(56)));
//G52:  __E___E___E_library_15_16_17(55) => bf_rule(57).
	p(NT(55), (NT(57)));
//G53:  rule(54)             => __E___E___E_library_15_16_17(55).
	p(NT(54), (NT(55)));
//G54:  __E___E_library_15_16(53) => _(11) rule(54).
	p(NT(53), (NT(11)+NT(54)));
//G55:  __E___E_library_15_18(58) => null.
	p(NT(58), (nul));
//G56:  __E___E_library_15_18(58) => __E___E_library_15_16(53) __E___E_library_15_18(58).
	p(NT(58), (NT(53)+NT(58)));
//G57:  __E_library_15(52)   => __E___E_library_15_18(58).
	p(NT(52), (NT(58)));
//G58:  rules(51)            => __E_library_15(52).
	p(NT(51), (NT(52)));
//G59:  library(50)          => rules(51).
	p(NT(50), (NT(51)));
//G60:  wff_matcher(59)      => wff(14).
	p(NT(59), (NT(14)));
//G61:  wff_body(60)         => wff(14).
	p(NT(60), (NT(14)));
//G62:  wff_rule(56)         => wff_matcher(59) _(11) ':' ':' '=' _(11) wff_body(60) _(11) '.'.
	p(NT(56), (NT(59)+NT(11)+T(2)+T(2)+T(3)+NT(11)+NT(60)+NT(11)+T(1)));
//G63:  bf_matcher(61)       => bf(27).
	p(NT(61), (NT(27)));
//G64:  bf_body(62)          => bf(27).
	p(NT(62), (NT(27)));
//G65:  bf_rule(57)          => bf_matcher(61) _(11) ':' '=' _(11) bf_body(62) _(11) '.'.
	p(NT(57), (NT(61)+NT(11)+T(2)+T(3)+NT(11)+NT(62)+NT(11)+T(1)));
//G66:  builder(63)          => _(11) builder_head(64) _(11) builder_body(65) _(11) '.'.
	p(NT(63), (NT(11)+NT(64)+NT(11)+NT(65)+NT(11)+T(1)));
//G67:  __E_builder_head_19(66) => __(31) capture(26).
	p(NT(66), (NT(31)+NT(26)));
//G68:  __E_builder_head_20(67) => null.
	p(NT(67), (nul));
//G69:  __E_builder_head_20(67) => __E_builder_head_19(66) __E_builder_head_20(67).
	p(NT(67), (NT(66)+NT(67)));
//G70:  builder_head(64)     => '(' _(11) capture(26) __E_builder_head_20(67) _(11) ')'.
	p(NT(64), (T(12)+NT(11)+NT(26)+NT(67)+NT(11)+T(13)));
//G71:  __E_builder_body_21(69) => '=' ':' _(11) bf(27).
	p(NT(69), (T(3)+T(2)+NT(11)+NT(27)));
//G72:  bf_builder_body(68)  => __E_builder_body_21(69).
	p(NT(68), (NT(69)));
//G73:  builder_body(65)     => bf_builder_body(68).
	p(NT(65), (NT(68)));
//G74:  __E_builder_body_22(71) => '=' ':' ':' _(11) wff(14).
	p(NT(71), (T(3)+T(2)+T(2)+NT(11)+NT(14)));
//G75:  wff_builder_body(70) => __E_builder_body_22(71).
	p(NT(70), (NT(71)));
//G76:  builder_body(65)     => wff_builder_body(70).
	p(NT(65), (NT(70)));
//G77:  __E_tau_constant_source_23(73) => '.' _(11).
	p(NT(73), (T(1)+NT(11)));
//G78:  __E_tau_constant_source_23(73) => null.
	p(NT(73), (nul));
//G79:  tau_constant_source(72) => rec_relations(21) _(11) main(13) _(11) __E_tau_constant_source_23(73).
	p(NT(72), (NT(21)+NT(11)+NT(13)+NT(11)+NT(73)));
//G80:  __E_wff_24(75)       => '(' _(11) wff(14) _(11) ')'.
	p(NT(75), (T(12)+NT(11)+NT(14)+NT(11)+T(13)));
//G81:  wff_parenthesis(74)  => __E_wff_24(75).
	p(NT(74), (NT(75)));
//G82:  wff(14)              => wff_parenthesis(74).
	p(NT(14), (NT(74)));
//G83:  __E___E_wff_25_26(78) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(78), (T(15)+T(6)+T(21)+T(11)+T(8)+T(4)+T(21)+T(11)+T(15)));
//G84:  __E___E_wff_25_26(78) => '<' '>'.
	p(NT(78), (T(22)+T(23)));
//G85:  __E_wff_25(77)       => __E___E_wff_25_26(78) _(11) wff(14).
	p(NT(77), (NT(78)+NT(11)+NT(14)));
//G86:  wff_sometimes(76)    => __E_wff_25(77).
	p(NT(76), (NT(77)));
//G87:  wff(14)              => wff_sometimes(76).
	p(NT(14), (NT(76)));
//G88:  __E___E_wff_27_28(81) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(81), (T(16)+T(10)+T(24)+T(16)+T(25)+T(15)));
//G89:  __E___E_wff_27_28(81) => '[' ']'.
	p(NT(81), (T(26)+T(27)));
//G90:  __E_wff_27(80)       => __E___E_wff_27_28(81) _(11) wff(14).
	p(NT(80), (NT(81)+NT(11)+NT(14)));
//G91:  wff_always(79)       => __E_wff_27(80).
	p(NT(79), (NT(80)));
//G92:  wff(14)              => wff_always(79).
	p(NT(14), (NT(79)));
//G93:  __E_wff_29(83)       => wff(14) _(11) '?' _(11) wff(14) _(11) ':' _(11) wff(14).
	p(NT(83), (NT(14)+NT(11)+T(28)+NT(11)+NT(14)+NT(11)+T(2)+NT(11)+NT(14)));
//G94:  wff_conditional(82)  => __E_wff_29(83).
	p(NT(82), (NT(83)));
//G95:  wff(14)              => wff_conditional(82).
	p(NT(14), (NT(82)));
//G96:  __E_wff_30(85)       => 'a' 'l' 'l' __(31) q_vars(86) __(31) wff(14).
	p(NT(85), (T(16)+T(10)+T(10)+NT(31)+NT(86)+NT(31)+NT(14)));
//G97:  wff_all(84)          => __E_wff_30(85).
	p(NT(84), (NT(85)));
//G98:  wff(14)              => wff_all(84).
	p(NT(14), (NT(84)));
//G99:  __E_wff_31(88)       => 'e' 'x' __(31) q_vars(86) __(31) wff(14).
	p(NT(88), (T(11)+T(29)+NT(31)+NT(86)+NT(31)+NT(14)));
//G100: wff_ex(87)           => __E_wff_31(88).
	p(NT(87), (NT(88)));
//G101: wff(14)              => wff_ex(87).
	p(NT(14), (NT(87)));
//G102: wff_ref(89)          => ref(24).
	p(NT(89), (NT(24)));
//G103: wff(14)              => wff_ref(89).
	p(NT(14), (NT(89)));
//G104: wff(14)              => constraint(90).
	p(NT(14), (NT(90)));
//G105: __E_wff_32(92)       => wff(14) _(11) '-' '>' _(11) wff(14).
	p(NT(92), (NT(14)+NT(11)+T(30)+T(23)+NT(11)+NT(14)));
//G106: wff_imply(91)        => __E_wff_32(92).
	p(NT(91), (NT(92)));
//G107: wff(14)              => wff_imply(91).
	p(NT(14), (NT(91)));
//G108: __E_wff_33(94)       => wff(14) _(11) '<' '-' _(11) wff(14).
	p(NT(94), (NT(14)+NT(11)+T(22)+T(30)+NT(11)+NT(14)));
//G109: wff_rimply(93)       => __E_wff_33(94).
	p(NT(93), (NT(94)));
//G110: wff(14)              => wff_rimply(93).
	p(NT(14), (NT(93)));
//G111: __E_wff_34(96)       => wff(14) _(11) '<' '-' '>' _(11) wff(14).
	p(NT(96), (NT(14)+NT(11)+T(22)+T(30)+T(23)+NT(11)+NT(14)));
//G112: wff_equiv(95)        => __E_wff_34(96).
	p(NT(95), (NT(96)));
//G113: wff(14)              => wff_equiv(95).
	p(NT(14), (NT(95)));
//G114: __E_wff_35(98)       => wff(14) _(11) '|' '|' _(11) wff(14).
	p(NT(98), (NT(14)+NT(11)+T(31)+T(31)+NT(11)+NT(14)));
//G115: wff_or(97)           => __E_wff_35(98).
	p(NT(97), (NT(98)));
//G116: wff(14)              => wff_or(97).
	p(NT(14), (NT(97)));
//G117: __E_wff_36(100)      => wff(14) _(11) '^' _(11) wff(14).
	p(NT(100), (NT(14)+NT(11)+T(32)+NT(11)+NT(14)));
//G118: wff_xor(99)          => __E_wff_36(100).
	p(NT(99), (NT(100)));
//G119: wff(14)              => wff_xor(99).
	p(NT(14), (NT(99)));
//G120: __E_wff_37(102)      => wff(14) _(11) '&' '&' _(11) wff(14).
	p(NT(102), (NT(14)+NT(11)+T(33)+T(33)+NT(11)+NT(14)));
//G121: wff_and(101)         => __E_wff_37(102).
	p(NT(101), (NT(102)));
//G122: wff(14)              => wff_and(101).
	p(NT(14), (NT(101)));
//G123: __E_wff_38(104)      => '!' _(11) wff(14).
	p(NT(104), (T(34)+NT(11)+NT(14)));
//G124: wff_neg(103)         => __E_wff_38(104).
	p(NT(103), (NT(104)));
//G125: wff(14)              => wff_neg(103).
	p(NT(14), (NT(103)));
//G126: wff_t(105)           => 'T'.
	p(NT(105), (T(35)));
//G127: wff(14)              => wff_t(105).
	p(NT(14), (NT(105)));
//G128: wff_f(106)           => 'F'.
	p(NT(106), (T(36)));
//G129: wff(14)              => wff_f(106).
	p(NT(14), (NT(106)));
//G130: __E_wff_39(108)      => bf(27) _(11) '<' '=' _(11) bf(27) _(11) '<' '=' _(11) bf(27).
	p(NT(108), (NT(27)+NT(11)+T(22)+T(3)+NT(11)+NT(27)+NT(11)+T(22)+T(3)+NT(11)+NT(27)));
//G131: bf_interval(107)     => __E_wff_39(108).
	p(NT(107), (NT(108)));
//G132: wff(14)              => bf_interval(107).
	p(NT(14), (NT(107)));
//G133: __E_wff_40(110)      => bf(27) _(11) '=' _(11) bf(27).
	p(NT(110), (NT(27)+NT(11)+T(3)+NT(11)+NT(27)));
//G134: bf_eq(109)           => __E_wff_40(110).
	p(NT(109), (NT(110)));
//G135: wff(14)              => bf_eq(109).
	p(NT(14), (NT(109)));
//G136: __E_wff_41(112)      => bf(27) _(11) '!' '=' _(11) bf(27).
	p(NT(112), (NT(27)+NT(11)+T(34)+T(3)+NT(11)+NT(27)));
//G137: bf_neq(111)          => __E_wff_41(112).
	p(NT(111), (NT(112)));
//G138: wff(14)              => bf_neq(111).
	p(NT(14), (NT(111)));
//G139: __E_wff_42(114)      => bf(27) _(11) '<' '=' _(11) bf(27).
	p(NT(114), (NT(27)+NT(11)+T(22)+T(3)+NT(11)+NT(27)));
//G140: bf_lteq(113)         => __E_wff_42(114).
	p(NT(113), (NT(114)));
//G141: wff(14)              => bf_lteq(113).
	p(NT(14), (NT(113)));
//G142: __E_wff_43(116)      => bf(27) _(11) '!' '<' '=' _(11) bf(27).
	p(NT(116), (NT(27)+NT(11)+T(34)+T(22)+T(3)+NT(11)+NT(27)));
//G143: bf_nlteq(115)        => __E_wff_43(116).
	p(NT(115), (NT(116)));
//G144: wff(14)              => bf_nlteq(115).
	p(NT(14), (NT(115)));
//G145: __E_wff_44(118)      => bf(27) _(11) '>' _(11) bf(27).
	p(NT(118), (NT(27)+NT(11)+T(23)+NT(11)+NT(27)));
//G146: bf_gt(117)           => __E_wff_44(118).
	p(NT(117), (NT(118)));
//G147: wff(14)              => bf_gt(117).
	p(NT(14), (NT(117)));
//G148: __E_wff_45(120)      => bf(27) _(11) '!' '>' _(11) bf(27).
	p(NT(120), (NT(27)+NT(11)+T(34)+T(23)+NT(11)+NT(27)));
//G149: bf_ngt(119)          => __E_wff_45(120).
	p(NT(119), (NT(120)));
//G150: wff(14)              => bf_ngt(119).
	p(NT(14), (NT(119)));
//G151: __E_wff_46(122)      => bf(27) _(11) '>' '=' _(11) bf(27).
	p(NT(122), (NT(27)+NT(11)+T(23)+T(3)+NT(11)+NT(27)));
//G152: bf_gteq(121)         => __E_wff_46(122).
	p(NT(121), (NT(122)));
//G153: wff(14)              => bf_gteq(121).
	p(NT(14), (NT(121)));
//G154: __E_wff_47(124)      => bf(27) _(11) '!' '>' '=' _(11) bf(27).
	p(NT(124), (NT(27)+NT(11)+T(34)+T(23)+T(3)+NT(11)+NT(27)));
//G155: bf_ngteq(123)        => __E_wff_47(124).
	p(NT(123), (NT(124)));
//G156: wff(14)              => bf_ngteq(123).
	p(NT(14), (NT(123)));
//G157: __E_wff_48(126)      => bf(27) _(11) '<' _(11) bf(27).
	p(NT(126), (NT(27)+NT(11)+T(22)+NT(11)+NT(27)));
//G158: bf_lt(125)           => __E_wff_48(126).
	p(NT(125), (NT(126)));
//G159: wff(14)              => bf_lt(125).
	p(NT(14), (NT(125)));
//G160: __E_wff_49(128)      => bf(27) _(11) '!' '<' _(11) bf(27).
	p(NT(128), (NT(27)+NT(11)+T(34)+T(22)+NT(11)+NT(27)));
//G161: bf_nlt(127)          => __E_wff_49(128).
	p(NT(127), (NT(128)));
//G162: wff(14)              => bf_nlt(127).
	p(NT(14), (NT(127)));
//G163: __E_wff_50(130)      => bv(131) _(11) '=' '_' _(11) bv(131).
	p(NT(130), (NT(131)+NT(11)+T(3)+T(37)+NT(11)+NT(131)));
//G164: bv_eq(129)           => __E_wff_50(130).
	p(NT(129), (NT(130)));
//G165: wff(14)              => bv_eq(129).
	p(NT(14), (NT(129)));
//G166: __E_wff_51(133)      => bv(131) _(11) '!' '=' '_' _(11) bv(131).
	p(NT(133), (NT(131)+NT(11)+T(34)+T(3)+T(37)+NT(11)+NT(131)));
//G167: bv_neq(132)          => __E_wff_51(133).
	p(NT(132), (NT(133)));
//G168: wff(14)              => bv_neq(132).
	p(NT(14), (NT(132)));
//G169: __E_wff_52(135)      => bv(131) _(11) '<' '=' '_' _(11) bv(131).
	p(NT(135), (NT(131)+NT(11)+T(22)+T(3)+T(37)+NT(11)+NT(131)));
//G170: bv_less_equal(134)   => __E_wff_52(135).
	p(NT(134), (NT(135)));
//G171: wff(14)              => bv_less_equal(134).
	p(NT(14), (NT(134)));
//G172: __E_wff_53(137)      => bv(131) _(11) '!' '<' '=' '_' _(11) bv(131).
	p(NT(137), (NT(131)+NT(11)+T(34)+T(22)+T(3)+T(37)+NT(11)+NT(131)));
//G173: bv_nleq(136)         => __E_wff_53(137).
	p(NT(136), (NT(137)));
//G174: wff(14)              => bv_nleq(136).
	p(NT(14), (NT(136)));
//G175: __E_wff_54(139)      => bv(131) _(11) '>' '_' _(11) bv(131).
	p(NT(139), (NT(131)+NT(11)+T(23)+T(37)+NT(11)+NT(131)));
//G176: bv_greater(138)      => __E_wff_54(139).
	p(NT(138), (NT(139)));
//G177: wff(14)              => bv_greater(138).
	p(NT(14), (NT(138)));
//G178: __E_wff_55(141)      => bv(131) _(11) '!' '>' '_' _(11) bv(131).
	p(NT(141), (NT(131)+NT(11)+T(34)+T(23)+T(37)+NT(11)+NT(131)));
//G179: bv_ngreater(140)     => __E_wff_55(141).
	p(NT(140), (NT(141)));
//G180: wff(14)              => bv_ngreater(140).
	p(NT(14), (NT(140)));
//G181: __E_wff_56(143)      => bv(131) _(11) '>' '=' '_' _(11) bv(131).
	p(NT(143), (NT(131)+NT(11)+T(23)+T(3)+T(37)+NT(11)+NT(131)));
//G182: bv_greater_equal(142) => __E_wff_56(143).
	p(NT(142), (NT(143)));
//G183: wff(14)              => bv_greater_equal(142).
	p(NT(14), (NT(142)));
//G184: __E_wff_57(145)      => bv(131) _(11) '!' '>' '=' '_' _(11) bv(131).
	p(NT(145), (NT(131)+NT(11)+T(34)+T(23)+T(3)+T(37)+NT(11)+NT(131)));
//G185: bv_ngeq(144)         => __E_wff_57(145).
	p(NT(144), (NT(145)));
//G186: wff(14)              => bv_ngeq(144).
	p(NT(14), (NT(144)));
//G187: __E_wff_58(147)      => bv(131) _(11) '<' '_' _(11) bv(131).
	p(NT(147), (NT(131)+NT(11)+T(22)+T(37)+NT(11)+NT(131)));
//G188: bv_less(146)         => __E_wff_58(147).
	p(NT(146), (NT(147)));
//G189: wff(14)              => bv_less(146).
	p(NT(14), (NT(146)));
//G190: __E_wff_59(149)      => bv(131) _(11) '!' '<' '_' _(11) bv(131).
	p(NT(149), (NT(131)+NT(11)+T(34)+T(22)+T(37)+NT(11)+NT(131)));
//G191: bv_nless(148)        => __E_wff_59(149).
	p(NT(148), (NT(149)));
//G192: wff(14)              => bv_nless(148).
	p(NT(14), (NT(148)));
//G193: wff(14)              => capture(26).
	p(NT(14), (NT(26)));
//G194: __E_bf_60(151)       => '(' _(11) bf(27) _(11) ')'.
	p(NT(151), (T(12)+NT(11)+NT(27)+NT(11)+T(13)));
//G195: bf_parenthesis(150)  => __E_bf_60(151).
	p(NT(150), (NT(151)));
//G196: bf(27)               => bf_parenthesis(150).
	p(NT(27), (NT(150)));
//G197: bf(27)               => bf_constant(152).
	p(NT(27), (NT(152)));
//G198: bf(27)               => variable(153).
	p(NT(27), (NT(153)));
//G199: __E_bf_61(155)       => 'S' _(11) '(' _(11) bf(27) _(11) ')'.
	p(NT(155), (T(38)+NT(11)+T(12)+NT(11)+NT(27)+NT(11)+T(13)));
//G200: bf_splitter(154)     => __E_bf_61(155).
	p(NT(154), (NT(155)));
//G201: bf(27)               => bf_splitter(154).
	p(NT(27), (NT(154)));
//G202: bf_ref(156)          => ref(24).
	p(NT(156), (NT(24)));
//G203: bf(27)               => bf_ref(156).
	p(NT(27), (NT(156)));
//G204: __E_bf_62(158)       => bf(27) _(11) '|' _(11) bf(27).
	p(NT(158), (NT(27)+NT(11)+T(31)+NT(11)+NT(27)));
//G205: bf_or(157)           => __E_bf_62(158).
	p(NT(157), (NT(158)));
//G206: bf(27)               => bf_or(157).
	p(NT(27), (NT(157)));
//G207: __E_bf_63(160)       => bf(27) _(11) '+' _(11) bf(27).
	p(NT(160), (NT(27)+NT(11)+T(39)+NT(11)+NT(27)));
//G208: bf_xor(159)          => __E_bf_63(160).
	p(NT(159), (NT(160)));
//G209: bf(27)               => bf_xor(159).
	p(NT(27), (NT(159)));
//G210: __E___E_bf_64_65(163) => typed(30).
	p(NT(163), (NT(30)));
//G211: __E___E_bf_64_65(163) => null.
	p(NT(163), (nul));
//G212: __E_bf_64(162)       => '1' __E___E_bf_64_65(163).
	p(NT(162), (T(40)+NT(163)));
//G213: bf_t(161)            => __E_bf_64(162).
	p(NT(161), (NT(162)));
//G214: bf(27)               => bf_t(161).
	p(NT(27), (NT(161)));
//G215: __E___E_bf_66_67(166) => typed(30).
	p(NT(166), (NT(30)));
//G216: __E___E_bf_66_67(166) => null.
	p(NT(166), (nul));
//G217: __E_bf_66(165)       => '0' __E___E_bf_66_67(166).
	p(NT(165), (T(41)+NT(166)));
//G218: bf_f(164)            => __E_bf_66(165).
	p(NT(164), (NT(165)));
//G219: bf(27)               => bf_f(164).
	p(NT(27), (NT(164)));
//G220: __E___E_bf_68_69(169) => _(11) '&' _(11).
	p(NT(169), (NT(11)+T(33)+NT(11)));
//G221: __E___E_bf_68_69(169) => __(31).
	p(NT(169), (NT(31)));
//G222: __E_bf_68(168)       => bf(27) __E___E_bf_68_69(169) bf(27).
	p(NT(168), (NT(27)+NT(169)+NT(27)));
//G223: bf_and(167)          => __E_bf_68(168).
	p(NT(167), (NT(168)));
//G224: bf(27)               => bf_and(167).
	p(NT(27), (NT(167)));
//G225: __E___E_bf_70_71(173) => bf_parenthesis(150).
	p(NT(173), (NT(150)));
//G226: __E___E_bf_70_71(173) => bf_constant(152).
	p(NT(173), (NT(152)));
//G227: __E___E_bf_70_71(173) => variable(153).
	p(NT(173), (NT(153)));
//G228: __E___E_bf_70_71(173) => bf_splitter(154).
	p(NT(173), (NT(154)));
//G229: __E___E_bf_70_71(173) => bf_ref(156).
	p(NT(173), (NT(156)));
//G230: __E___E_bf_70_71(173) => bf_t(161).
	p(NT(173), (NT(161)));
//G231: __E___E_bf_70_71(173) => bf_f(164).
	p(NT(173), (NT(164)));
//G232: __E___E_bf_70_71(173) => bf_neg(170).
	p(NT(173), (NT(170)));
//G233: __E___E_bf_70_71(173) => capture(26).
	p(NT(173), (NT(26)));
//G234: bf_neg_oprnd(172)    => __E___E_bf_70_71(173).
	p(NT(172), (NT(173)));
//G235: __E_bf_70(171)       => bf_neg_oprnd(172) _(11) '\''.
	p(NT(171), (NT(172)+NT(11)+T(42)));
//G236: bf_neg(170)          => __E_bf_70(171).
	p(NT(170), (NT(171)));
//G237: bf(27)               => bf_neg(170).
	p(NT(27), (NT(170)));
//G238: __E___E_bf_72_73(177) => bf_parenthesis(150).
	p(NT(177), (NT(150)));
//G239: __E___E_bf_72_73(177) => variable(153).
	p(NT(177), (NT(153)));
//G240: __E___E_bf_72_73(177) => bf_splitter(154).
	p(NT(177), (NT(154)));
//G241: __E___E_bf_72_73(177) => bf_ref(156).
	p(NT(177), (NT(156)));
//G242: __E___E_bf_72_73(177) => bf_neg(170).
	p(NT(177), (NT(170)));
//G243: __E___E_bf_72_73(177) => bf_and_nosep(174).
	p(NT(177), (NT(174)));
//G244: __E___E_bf_72_73(177) => capture(26).
	p(NT(177), (NT(26)));
//G245: bf_and_nosep_1st_oprnd(176) => __E___E_bf_72_73(177).
	p(NT(176), (NT(177)));
//G246: __E___E_bf_72_74(179) => bf_parenthesis(150).
	p(NT(179), (NT(150)));
//G247: __E___E_bf_72_74(179) => bf_constant(152).
	p(NT(179), (NT(152)));
//G248: __E___E_bf_72_74(179) => variable(153).
	p(NT(179), (NT(153)));
//G249: __E___E_bf_72_74(179) => bf_splitter(154).
	p(NT(179), (NT(154)));
//G250: __E___E_bf_72_74(179) => bf_ref(156).
	p(NT(179), (NT(156)));
//G251: __E___E_bf_72_74(179) => bf_neg(170).
	p(NT(179), (NT(170)));
//G252: __E___E_bf_72_74(179) => capture(26).
	p(NT(179), (NT(26)));
//G253: bf_and_nosep_2nd_oprnd(178) => __E___E_bf_72_74(179).
	p(NT(178), (NT(179)));
//G254: __E_bf_72(175)       => bf_and_nosep_1st_oprnd(176) bf_and_nosep_2nd_oprnd(178).
	p(NT(175), (NT(176)+NT(178)));
//G255: bf_and_nosep(174)    => __E_bf_72(175).
	p(NT(174), (NT(175)));
//G256: bf(27)               => bf_and_nosep(174).
	p(NT(27), (NT(174)));
//G257: bf(27)               => capture(26).
	p(NT(27), (NT(26)));
//G258: __E_bv_75(181)       => '(' _(11) bv(131) _(11) ')'.
	p(NT(181), (T(12)+NT(11)+NT(131)+NT(11)+T(13)));
//G259: bv_parenthesis(180)  => __E_bv_75(181).
	p(NT(180), (NT(181)));
//G260: bv(131)              => bv_parenthesis(180).
	p(NT(131), (NT(180)));
//G261: __E_bv_76(183)       => '[' bv(131) ']'.
	p(NT(183), (T(26)+NT(131)+T(27)));
//G262: bv_checked(182)      => __E_bv_76(183).
	p(NT(182), (NT(183)));
//G263: bv(131)              => bv_checked(182).
	p(NT(131), (NT(182)));
//G264: bv(131)              => variable(153).
	p(NT(131), (NT(153)));
//G265: __E_bv_77(185)       => bv(131) _(11) '+' _(11) bv(131).
	p(NT(185), (NT(131)+NT(11)+T(39)+NT(11)+NT(131)));
//G266: bv_add(184)          => __E_bv_77(185).
	p(NT(184), (NT(185)));
//G267: bv(131)              => bv_add(184).
	p(NT(131), (NT(184)));
//G268: __E_bv_78(187)       => bv(131) _(11) '*' _(11) bv(131).
	p(NT(187), (NT(131)+NT(11)+T(43)+NT(11)+NT(131)));
//G269: bv_mul(186)          => __E_bv_78(187).
	p(NT(186), (NT(187)));
//G270: bv(131)              => bv_mul(186).
	p(NT(131), (NT(186)));
//G271: __E_bv_79(189)       => bv(131) _(11) '/' _(11) bv(131).
	p(NT(189), (NT(131)+NT(11)+T(44)+NT(11)+NT(131)));
//G272: bv_div(188)          => __E_bv_79(189).
	p(NT(188), (NT(189)));
//G273: bv(131)              => bv_div(188).
	p(NT(131), (NT(188)));
//G274: __E_bv_80(191)       => bv(131) _(11) '%' _(11) bv(131).
	p(NT(191), (NT(131)+NT(11)+T(45)+NT(11)+NT(131)));
//G275: bv_mod(190)          => __E_bv_80(191).
	p(NT(190), (NT(191)));
//G276: bv(131)              => bv_mod(190).
	p(NT(131), (NT(190)));
//G277: __E_bv_81(193)       => bv(131) _(11) '-' _(11) bv(131).
	p(NT(193), (NT(131)+NT(11)+T(30)+NT(11)+NT(131)));
//G278: bv_sub(192)          => __E_bv_81(193).
	p(NT(192), (NT(193)));
//G279: bv(131)              => bv_sub(192).
	p(NT(131), (NT(192)));
//G280: bv_neg(194)          => bv(131).
	p(NT(194), (NT(131)));
//G281: bv(131)              => '~' _(11) bv_neg(194).
	p(NT(131), (T(46)+NT(11)+NT(194)));
//G282: __E_bv_82(196)       => bv(131) _(11) '&' _(11) bv(131).
	p(NT(196), (NT(131)+NT(11)+T(33)+NT(11)+NT(131)));
//G283: bv_and(195)          => __E_bv_82(196).
	p(NT(195), (NT(196)));
//G284: bv(131)              => bv_and(195).
	p(NT(131), (NT(195)));
//G285: __E_bv_83(198)       => bv(131) _(11) '!' '&' _(11) bv(131).
	p(NT(198), (NT(131)+NT(11)+T(34)+T(33)+NT(11)+NT(131)));
//G286: bv_nand(197)         => __E_bv_83(198).
	p(NT(197), (NT(198)));
//G287: bv(131)              => bv_nand(197).
	p(NT(131), (NT(197)));
//G288: __E_bv_84(200)       => bv(131) _(11) '|' _(11) bv(131).
	p(NT(200), (NT(131)+NT(11)+T(31)+NT(11)+NT(131)));
//G289: bv_or(199)           => __E_bv_84(200).
	p(NT(199), (NT(200)));
//G290: bv(131)              => bv_or(199).
	p(NT(131), (NT(199)));
//G291: __E_bv_85(202)       => bv(131) _(11) '!' '|' _(11) bv(131).
	p(NT(202), (NT(131)+NT(11)+T(34)+T(31)+NT(11)+NT(131)));
//G292: bv_nor(201)          => __E_bv_85(202).
	p(NT(201), (NT(202)));
//G293: bv(131)              => bv_nor(201).
	p(NT(131), (NT(201)));
//G294: __E_bv_86(204)       => bv(131) _(11) '(' '+' ')' _(11) bv(131).
	p(NT(204), (NT(131)+NT(11)+T(12)+T(39)+T(13)+NT(11)+NT(131)));
//G295: bv_xor(203)          => __E_bv_86(204).
	p(NT(203), (NT(204)));
//G296: bv(131)              => bv_xor(203).
	p(NT(131), (NT(203)));
//G297: __E_bv_87(206)       => bv(131) _(11) '!' '(' '+' ')' _(11) bv(131).
	p(NT(206), (NT(131)+NT(11)+T(34)+T(12)+T(39)+T(13)+NT(11)+NT(131)));
//G298: bv_xnor(205)         => __E_bv_87(206).
	p(NT(205), (NT(206)));
//G299: bv(131)              => bv_xnor(205).
	p(NT(131), (NT(205)));
//G300: __E_bv_88(208)       => bv(131) _(11) '<' '<' _(11) bv(131).
	p(NT(208), (NT(131)+NT(11)+T(22)+T(22)+NT(11)+NT(131)));
//G301: bv_rotate_left(207)  => __E_bv_88(208).
	p(NT(207), (NT(208)));
//G302: bv(131)              => bv_rotate_left(207).
	p(NT(131), (NT(207)));
//G303: __E_bv_89(210)       => bv(131) _(11) '>' '>' _(11) bv(131).
	p(NT(210), (NT(131)+NT(11)+T(23)+T(23)+NT(11)+NT(131)));
//G304: bv_rotate_right(209) => __E_bv_89(210).
	p(NT(209), (NT(210)));
//G305: bv(131)              => bv_rotate_right(209).
	p(NT(131), (NT(209)));
//G306: __E_bv_90(212)       => 'm' 'i' 'n' __(31) bv(131) __(31) bv(131).
	p(NT(212), (T(21)+T(4)+T(5)+NT(31)+NT(131)+NT(31)+NT(131)));
//G307: bv_min(211)          => __E_bv_90(212).
	p(NT(211), (NT(212)));
//G308: bv(131)              => bv_min(211).
	p(NT(131), (NT(211)));
//G309: __E_bv_91(214)       => 'm' 'a' 'x' __(31) bv(131) __(31) bv(131).
	p(NT(214), (T(21)+T(16)+T(29)+NT(31)+NT(131)+NT(31)+NT(131)));
//G310: bv_max(213)          => __E_bv_91(214).
	p(NT(213), (NT(214)));
//G311: bv(131)              => bv_max(213).
	p(NT(131), (NT(213)));
//G312: bv(131)              => bitvector(215).
	p(NT(131), (NT(215)));
//G313: __E_bitvector_92(217) => _(11) ':' _(11) bv_type(218) _(11).
	p(NT(217), (NT(11)+T(2)+NT(11)+NT(218)+NT(11)));
//G314: __E_bitvector_92(217) => null.
	p(NT(217), (nul));
//G315: bitvector(215)       => '#' 'b' bits(216) __E_bitvector_92(217).
	p(NT(215), (T(47)+T(17)+NT(216)+NT(217)));
//G316: __E_bitvector_93(220) => _(11) ':' _(11) bv_type(218) _(11).
	p(NT(220), (NT(11)+T(2)+NT(11)+NT(218)+NT(11)));
//G317: __E_bitvector_93(220) => null.
	p(NT(220), (nul));
//G318: bitvector(215)       => num(219) __E_bitvector_93(220).
	p(NT(215), (NT(219)+NT(220)));
//G319: __E_bitvector_94(222) => _(11) ':' _(11) bv_type(218) _(11).
	p(NT(222), (NT(11)+T(2)+NT(11)+NT(218)+NT(11)));
//G320: __E_bitvector_94(222) => null.
	p(NT(222), (nul));
//G321: bitvector(215)       => '#' 'x' hexnum(221) __E_bitvector_94(222).
	p(NT(215), (T(47)+T(29)+NT(221)+NT(222)));
//G322: __E_bits_95(223)     => '0'.
	p(NT(223), (T(41)));
//G323: __E_bits_95(223)     => '1'.
	p(NT(223), (T(40)));
//G324: __E_bits_96(224)     => __E_bits_95(223).
	p(NT(224), (NT(223)));
//G325: __E_bits_96(224)     => __E_bits_95(223) __E_bits_96(224).
	p(NT(224), (NT(223)+NT(224)));
//G326: bits(216)            => __E_bits_96(224).
	p(NT(216), (NT(224)));
//G327: __E_hexnum_97(225)   => xdigit(4).
	p(NT(225), (NT(4)));
//G328: __E_hexnum_97(225)   => xdigit(4) __E_hexnum_97(225).
	p(NT(225), (NT(4)+NT(225)));
//G329: hexnum(221)          => __E_hexnum_97(225).
	p(NT(221), (NT(225)));
//G330: __E_constraint_98(227) => '[' ctnvar(228) _(11) '!' '=' _(11) num(219) ']'.
	p(NT(227), (T(26)+NT(228)+NT(11)+T(34)+T(3)+NT(11)+NT(219)+T(27)));
//G331: __E_constraint_98(227) => '[' num(219) _(11) '!' '=' _(11) ctnvar(228) ']'.
	p(NT(227), (T(26)+NT(219)+NT(11)+T(34)+T(3)+NT(11)+NT(228)+T(27)));
//G332: ctn_neq(226)         => __E_constraint_98(227).
	p(NT(226), (NT(227)));
//G333: constraint(90)       => ctn_neq(226).
	p(NT(90), (NT(226)));
//G334: __E_constraint_99(230) => '[' ctnvar(228) _(11) '=' _(11) num(219) ']'.
	p(NT(230), (T(26)+NT(228)+NT(11)+T(3)+NT(11)+NT(219)+T(27)));
//G335: __E_constraint_99(230) => '[' num(219) _(11) '=' _(11) ctnvar(228) ']'.
	p(NT(230), (T(26)+NT(219)+NT(11)+T(3)+NT(11)+NT(228)+T(27)));
//G336: ctn_eq(229)          => __E_constraint_99(230).
	p(NT(229), (NT(230)));
//G337: constraint(90)       => ctn_eq(229).
	p(NT(90), (NT(229)));
//G338: __E_constraint_100(232) => '[' ctnvar(228) _(11) '>' '=' _(11) num(219) ']'.
	p(NT(232), (T(26)+NT(228)+NT(11)+T(23)+T(3)+NT(11)+NT(219)+T(27)));
//G339: __E_constraint_100(232) => '[' num(219) _(11) '>' '=' _(11) ctnvar(228) ']'.
	p(NT(232), (T(26)+NT(219)+NT(11)+T(23)+T(3)+NT(11)+NT(228)+T(27)));
//G340: ctn_gteq(231)        => __E_constraint_100(232).
	p(NT(231), (NT(232)));
//G341: constraint(90)       => ctn_gteq(231).
	p(NT(90), (NT(231)));
//G342: __E_constraint_101(234) => '[' ctnvar(228) _(11) '>' _(11) num(219) ']'.
	p(NT(234), (T(26)+NT(228)+NT(11)+T(23)+NT(11)+NT(219)+T(27)));
//G343: __E_constraint_101(234) => '[' num(219) _(11) '>' _(11) ctnvar(228) ']'.
	p(NT(234), (T(26)+NT(219)+NT(11)+T(23)+NT(11)+NT(228)+T(27)));
//G344: ctn_gt(233)          => __E_constraint_101(234).
	p(NT(233), (NT(234)));
//G345: constraint(90)       => ctn_gt(233).
	p(NT(90), (NT(233)));
//G346: __E_constraint_102(236) => '[' ctnvar(228) _(11) '<' '=' _(11) num(219) ']'.
	p(NT(236), (T(26)+NT(228)+NT(11)+T(22)+T(3)+NT(11)+NT(219)+T(27)));
//G347: __E_constraint_102(236) => '[' num(219) _(11) '<' '=' _(11) ctnvar(228) ']'.
	p(NT(236), (T(26)+NT(219)+NT(11)+T(22)+T(3)+NT(11)+NT(228)+T(27)));
//G348: ctn_lteq(235)        => __E_constraint_102(236).
	p(NT(235), (NT(236)));
//G349: constraint(90)       => ctn_lteq(235).
	p(NT(90), (NT(235)));
//G350: __E_constraint_103(238) => '[' ctnvar(228) _(11) '<' _(11) num(219) ']'.
	p(NT(238), (T(26)+NT(228)+NT(11)+T(22)+NT(11)+NT(219)+T(27)));
//G351: __E_constraint_103(238) => '[' num(219) _(11) '<' _(11) ctnvar(228) ']'.
	p(NT(238), (T(26)+NT(219)+NT(11)+T(22)+NT(11)+NT(228)+T(27)));
//G352: ctn_lt(237)          => __E_constraint_103(238).
	p(NT(237), (NT(238)));
//G353: constraint(90)       => ctn_lt(237).
	p(NT(90), (NT(237)));
//G354: __E_bf_constant_104(239) => capture(26).
	p(NT(239), (NT(26)));
//G355: __E_bf_constant_104(239) => source(240).
	p(NT(239), (NT(240)));
//G356: __E_bf_constant_105(241) => typed(30).
	p(NT(241), (NT(30)));
//G357: __E_bf_constant_105(241) => null.
	p(NT(241), (nul));
//G358: bf_constant(152)     => '{' _(11) __E_bf_constant_104(239) _(11) '}' __E_bf_constant_105(241).
	p(NT(152), (T(48)+NT(11)+NT(239)+NT(11)+T(49)+NT(241)));
//G359: __E___E_source_106_107(244) => src_c(242).
	p(NT(244), (NT(242)));
//G360: __E___E_source_106_107(244) => space(2).
	p(NT(244), (NT(2)));
//G361: __E___E_source_106_108(245) => null.
	p(NT(245), (nul));
//G362: __E___E_source_106_108(245) => __E___E_source_106_107(244) __E___E_source_106_108(245).
	p(NT(245), (NT(244)+NT(245)));
//G363: __E_source_106(243)  => __E___E_source_106_108(245) src_c(242).
	p(NT(243), (NT(245)+NT(242)));
//G364: __E_source_106(243)  => null.
	p(NT(243), (nul));
//G365: source(240)          => src_c(242) __E_source_106(243).
	p(NT(240), (NT(242)+NT(243)));
//G366: src_c(242)           => alnum(6).
	p(NT(242), (NT(6)));
//G367: __N_0(438)           => '{'.
	p(NT(438), (T(48)));
//G368: __N_1(439)           => '}'.
	p(NT(439), (T(49)));
//G369: src_c(242)           => ~( __N_0(438) ) & ~( __N_1(439) ) & punct(7).	 # conjunctive
	p(NT(242), ~(NT(438)) & ~(NT(439)) & (NT(7)));
//G370: __E_src_c_109(246)   => src_c(242).
	p(NT(246), (NT(242)));
//G371: __E_src_c_109(246)   => space(2).
	p(NT(246), (NT(2)));
//G372: __E_src_c_110(247)   => null.
	p(NT(247), (nul));
//G373: __E_src_c_110(247)   => __E_src_c_109(246) __E_src_c_110(247).
	p(NT(247), (NT(246)+NT(247)));
//G374: src_c(242)           => '{' __E_src_c_110(247) '}'.
	p(NT(242), (T(48)+NT(247)+T(49)));
//G375: __E_variable_111(248) => uconst(249).
	p(NT(248), (NT(249)));
//G376: __E_variable_111(248) => io_var(250).
	p(NT(248), (NT(250)));
//G377: __E_variable_111(248) => var_name(251).
	p(NT(248), (NT(251)));
//G378: __E_variable_112(252) => typed(30).
	p(NT(252), (NT(30)));
//G379: __E_variable_112(252) => null.
	p(NT(252), (nul));
//G380: variable(153)        => __E_variable_111(248) __E_variable_112(252).
	p(NT(153), (NT(248)+NT(252)));
//G381: __N_2(440)           => 'F'.
	p(NT(440), (T(36)));
//G382: __N_3(441)           => 'T'.
	p(NT(441), (T(35)));
//G383: __E_var_name_113(253) => ~( __N_2(440) ) & ~( __N_3(441) ) & alpha(5).	 # conjunctive
	p(NT(253), ~(NT(440)) & ~(NT(441)) & (NT(5)));
//G384: __E_var_name_114(254) => null.
	p(NT(254), (nul));
//G385: __E_var_name_114(254) => digit(3) __E_var_name_114(254).
	p(NT(254), (NT(3)+NT(254)));
//G386: var_name(251)        => __E_var_name_113(253) __E_var_name_114(254).	 # guarded: charvar
	p(NT(251), (NT(253)+NT(254)));
	p.back().guard = "charvar";
//G387: __N_4(442)           => 'F'.
	p(NT(442), (T(36)));
//G388: __N_5(443)           => 'T'.
	p(NT(443), (T(35)));
//G389: __E_var_name_115(255) => ~( __N_4(442) ) & ~( __N_5(443) ) & alpha(5).	 # conjunctive
	p(NT(255), ~(NT(442)) & ~(NT(443)) & (NT(5)));
//G390: __E_var_name_116(256) => alnum(6).
	p(NT(256), (NT(6)));
//G391: __E_var_name_116(256) => '_'.
	p(NT(256), (T(37)));
//G392: __E_var_name_117(257) => null.
	p(NT(257), (nul));
//G393: __E_var_name_117(257) => __E_var_name_116(256) __E_var_name_117(257).
	p(NT(257), (NT(256)+NT(257)));
//G394: var_name(251)        => __E_var_name_115(255) __E_var_name_117(257).	 # guarded: var
	p(NT(251), (NT(255)+NT(257)));
	p.back().guard = "var";
//G395: io_var(250)          => io_var_name(28) '[' offset(258) ']'.
	p(NT(250), (NT(28)+T(26)+NT(258)+T(27)));
//G396: io_var_name(28)      => chars(259).
	p(NT(28), (NT(259)));
//G397: __E___E_uconst_118_119(262) => chars(259) _(11).
	p(NT(262), (NT(259)+NT(11)));
//G398: __E___E_uconst_118_119(262) => null.
	p(NT(262), (nul));
//G399: __E_uconst_118(261)  => __E___E_uconst_118_119(262) ':' _(11) chars(259).
	p(NT(261), (NT(262)+T(2)+NT(11)+NT(259)));
//G400: uconst_name(260)     => __E_uconst_118(261).
	p(NT(260), (NT(261)));
//G401: uconst(249)          => '<' _(11) uconst_name(260) _(11) '>'.
	p(NT(249), (T(22)+NT(11)+NT(260)+NT(11)+T(23)));
//G402: __E_q_vars_120(264)  => _(11) ',' _(11) q_var(263).
	p(NT(264), (NT(11)+T(20)+NT(11)+NT(263)));
//G403: __E_q_vars_121(265)  => null.
	p(NT(265), (nul));
//G404: __E_q_vars_121(265)  => __E_q_vars_120(264) __E_q_vars_121(265).
	p(NT(265), (NT(264)+NT(265)));
//G405: q_vars(86)           => q_var(263) __E_q_vars_121(265).
	p(NT(86), (NT(263)+NT(265)));
//G406: q_var(263)           => capture(26).
	p(NT(263), (NT(26)));
//G407: __N_6(444)           => uconst(249).
	p(NT(444), (NT(249)));
//G408: q_var(263)           => variable(153) & ~( __N_6(444) ).	 # conjunctive
	p(NT(263), (NT(153)) & ~(NT(444)));
//G409: ctnvar(228)          => chars(259).
	p(NT(228), (NT(259)));
//G410: __E_offsets_122(266) => _(11) ',' _(11) offset(258).
	p(NT(266), (NT(11)+T(20)+NT(11)+NT(258)));
//G411: __E_offsets_123(267) => null.
	p(NT(267), (nul));
//G412: __E_offsets_123(267) => __E_offsets_122(266) __E_offsets_123(267).
	p(NT(267), (NT(266)+NT(267)));
//G413: offsets(39)          => '[' _(11) offset(258) __E_offsets_123(267) _(11) ']'.
	p(NT(39), (T(26)+NT(11)+NT(258)+NT(267)+NT(11)+T(27)));
//G414: offset(258)          => integer(268).
	p(NT(258), (NT(268)));
//G415: offset(258)          => capture(26).
	p(NT(258), (NT(26)));
//G416: offset(258)          => shift(269).
	p(NT(258), (NT(269)));
//G417: __N_7(445)           => io_var(250).
	p(NT(445), (NT(250)));
//G418: __E_offset_124(270)  => variable(153) & ~( __N_7(445) ).	 # conjunctive
	p(NT(270), (NT(153)) & ~(NT(445)));
//G419: offset(258)          => __E_offset_124(270).
	p(NT(258), (NT(270)));
//G420: __E_shift_125(271)   => capture(26).
	p(NT(271), (NT(26)));
//G421: __N_8(446)           => io_var(250).
	p(NT(446), (NT(250)));
//G422: __E___E_shift_125_126(272) => variable(153) & ~( __N_8(446) ).	 # conjunctive
	p(NT(272), (NT(153)) & ~(NT(446)));
//G423: __E_shift_125(271)   => __E___E_shift_125_126(272).
	p(NT(271), (NT(272)));
//G424: shift(269)           => __E_shift_125(271) _(11) '-' _(11) num(219).
	p(NT(269), (NT(271)+NT(11)+T(30)+NT(11)+NT(219)));
//G425: __E_chars_127(273)   => alnum(6).
	p(NT(273), (NT(6)));
//G426: __E_chars_127(273)   => '_'.
	p(NT(273), (T(37)));
//G427: __E_chars_128(274)   => null.
	p(NT(274), (nul));
//G428: __E_chars_128(274)   => __E_chars_127(273) __E_chars_128(274).
	p(NT(274), (NT(273)+NT(274)));
//G429: chars(259)           => alpha(5) __E_chars_128(274).
	p(NT(259), (NT(5)+NT(274)));
//G430: __E_digits_129(276)  => digit(3).
	p(NT(276), (NT(3)));
//G431: __E_digits_129(276)  => digit(3) __E_digits_129(276).
	p(NT(276), (NT(3)+NT(276)));
//G432: digits(275)          => __E_digits_129(276).
	p(NT(275), (NT(276)));
//G433: num(219)             => digits(275).
	p(NT(219), (NT(275)));
//G434: __E_integer_130(277) => '-'.
	p(NT(277), (T(30)));
//G435: __E_integer_130(277) => null.
	p(NT(277), (nul));
//G436: integer(268)         => __E_integer_130(277) _(11) digits(275).
	p(NT(268), (NT(277)+NT(11)+NT(275)));
//G437: sym(37)              => chars(259).
	p(NT(37), (NT(259)));
//G438: capture(26)          => '$' chars(259).
	p(NT(26), (T(50)+NT(259)));
//G439: typed(30)            => _(11) ':' _(11) type(278).
	p(NT(30), (NT(11)+T(2)+NT(11)+NT(278)));
//G440: type(278)            => bv_type(218).
	p(NT(278), (NT(218)));
//G441: type(278)            => chars(259).
	p(NT(278), (NT(259)));
//G442: __E_bv_type_131(279) => num(219).
	p(NT(279), (NT(219)));
//G443: __E_bv_type_131(279) => null.
	p(NT(279), (nul));
//G444: bv_type(218)         => 'b' 'v' __E_bv_type_131(279).
	p(NT(218), (T(17)+T(51)+NT(279)));
//G445: scope_id(280)        => digits(275).
	p(NT(280), (NT(275)));
//G446: __E_comment_132(282) => printable(8).
	p(NT(282), (NT(8)));
//G447: __E_comment_132(282) => '\t'.
	p(NT(282), (T(52)));
//G448: __E_comment_133(283) => null.
	p(NT(283), (nul));
//G449: __E_comment_133(283) => __E_comment_132(282) __E_comment_133(283).
	p(NT(283), (NT(282)+NT(283)));
//G450: __E_comment_134(284) => '\n'.
	p(NT(284), (T(53)));
//G451: __E_comment_134(284) => '\r'.
	p(NT(284), (T(54)));
//G452: __E_comment_134(284) => eof(1).
	p(NT(284), (NT(1)));
//G453: comment(281)         => '#' __E_comment_133(283) __E_comment_134(284).
	p(NT(281), (T(47)+NT(283)+NT(284)));
//G454: __(31)               => space(2).
	p(NT(31), (NT(2)));
//G455: __(31)               => comment(281).
	p(NT(31), (NT(281)));
//G456: __(31)               => __(31) space(2).
	p(NT(31), (NT(31)+NT(2)));
//G457: __(31)               => __(31) comment(281).
	p(NT(31), (NT(31)+NT(281)));
//G458: _(11)                => __(31).
	p(NT(11), (NT(31)));
//G459: _(11)                => null.
	p(NT(11), (nul));
//G460: cli(285)             => _(11).
	p(NT(285), (NT(11)));
//G461: __E_cli_135(287)     => '.' _(11) cli_command(286) _(11).
	p(NT(287), (T(1)+NT(11)+NT(286)+NT(11)));
//G462: __E_cli_136(288)     => null.
	p(NT(288), (nul));
//G463: __E_cli_136(288)     => __E_cli_135(287) __E_cli_136(288).
	p(NT(288), (NT(287)+NT(288)));
//G464: cli(285)             => _(11) cli_command(286) _(11) __E_cli_136(288).
	p(NT(285), (NT(11)+NT(286)+NT(11)+NT(288)));
//G465: __E_cli_command_137(291) => 'q'.
	p(NT(291), (T(55)));
//G466: __E_cli_command_137(291) => 'q' 'u' 'i' 't'.
	p(NT(291), (T(55)+T(7)+T(4)+T(8)));
//G467: quit_sym(290)        => __E_cli_command_137(291).
	p(NT(290), (NT(291)));
//G468: quit_cmd(289)        => quit_sym(290).
	p(NT(289), (NT(290)));
//G469: cli_command(286)     => quit_cmd(289).
	p(NT(286), (NT(289)));
//G470: __E_cli_command_138(294) => 'v'.
	p(NT(294), (T(51)));
//G471: __E_cli_command_138(294) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(294), (T(51)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G472: version_sym(293)     => __E_cli_command_138(294).
	p(NT(293), (NT(294)));
//G473: version_cmd(292)     => version_sym(293).
	p(NT(292), (NT(293)));
//G474: cli_command(286)     => version_cmd(292).
	p(NT(286), (NT(292)));
//G475: __E_cli_command_139(297) => 'c'.
	p(NT(297), (T(14)));
//G476: __E_cli_command_139(297) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(297), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G477: clear_sym(296)       => __E_cli_command_139(297).
	p(NT(296), (NT(297)));
//G478: clear_cmd(295)       => clear_sym(296).
	p(NT(295), (NT(296)));
//G479: cli_command(286)     => clear_cmd(295).
	p(NT(286), (NT(295)));
//G480: __E___E_cli_command_140_141(301) => 'h'.
	p(NT(301), (T(56)));
//G481: __E___E_cli_command_140_141(301) => 'h' 'e' 'l' 'p'.
	p(NT(301), (T(56)+T(11)+T(10)+T(57)));
//G482: help_sym(300)        => __E___E_cli_command_140_141(301).
	p(NT(300), (NT(301)));
//G483: __E___E_cli_command_140_142(302) => __(31) help_arg(303).
	p(NT(302), (NT(31)+NT(303)));
//G484: __E___E_cli_command_140_142(302) => null.
	p(NT(302), (nul));
//G485: __E_cli_command_140(299) => help_sym(300) __E___E_cli_command_140_142(302).
	p(NT(299), (NT(300)+NT(302)));
//G486: help_cmd(298)        => __E_cli_command_140(299).
	p(NT(298), (NT(299)));
//G487: cli_command(286)     => help_cmd(298).
	p(NT(286), (NT(298)));
//G488: file_sym(306)        => 'f' 'i' 'l' 'e'.
	p(NT(306), (T(9)+T(4)+T(10)+T(11)));
//G489: __E_cli_command_143(305) => file_sym(306) __(31) q_string(307).
	p(NT(305), (NT(306)+NT(31)+NT(307)));
//G490: file_cmd(304)        => __E_cli_command_143(305).
	p(NT(304), (NT(305)));
//G491: cli_command(286)     => file_cmd(304).
	p(NT(286), (NT(304)));
//G492: valid_sym(310)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(310), (T(51)+T(16)+T(10)+T(4)+T(58)));
//G493: __E_cli_command_144(309) => valid_sym(310) __(31) normalize_cmd_arg(311).
	p(NT(309), (NT(310)+NT(31)+NT(311)));
//G494: valid_cmd(308)       => __E_cli_command_144(309).
	p(NT(308), (NT(309)));
//G495: cli_command(286)     => valid_cmd(308).
	p(NT(286), (NT(308)));
//G496: sat_sym(314)         => 's' 'a' 't'.
	p(NT(314), (T(15)+T(16)+T(8)));
//G497: __E_cli_command_145(313) => sat_sym(314) __(31) normalize_cmd_arg(311).
	p(NT(313), (NT(314)+NT(31)+NT(311)));
//G498: sat_cmd(312)         => __E_cli_command_145(313).
	p(NT(312), (NT(313)));
//G499: cli_command(286)     => sat_cmd(312).
	p(NT(286), (NT(312)));
//G500: unsat_sym(317)       => 'u' 'n' 's' 'a' 't'.
	p(NT(317), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G501: __E_cli_command_146(316) => unsat_sym(317) __(31) normalize_cmd_arg(311).
	p(NT(316), (NT(317)+NT(31)+NT(311)));
//G502: unsat_cmd(315)       => __E_cli_command_146(316).
	p(NT(315), (NT(316)));
//G503: cli_command(286)     => unsat_cmd(315).
	p(NT(286), (NT(315)));
//G504: solve_sym(320)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(320), (T(15)+T(6)+T(10)+T(51)+T(11)));
//G505: __E___E_cli_command_147_148(321) => solve_options(322).
	p(NT(321), (NT(322)));
//G506: __E___E_cli_command_147_148(321) => null.
	p(NT(321), (nul));
//G507: __E_cli_command_147(319) => solve_sym(320) __E___E_cli_command_147_148(321) __(31) wff_cmd_arg(323).
	p(NT(319), (NT(320)+NT(321)+NT(31)+NT(323)));
//G508: solve_cmd(318)       => __E_cli_command_147(319).
	p(NT(318), (NT(319)));
//G509: cli_command(286)     => solve_cmd(318).
	p(NT(286), (NT(318)));
//G510: lgrs_sym(326)        => 'l' 'g' 'r' 's'.
	p(NT(326), (T(10)+T(59)+T(19)+T(15)));
//G511: __E_cli_command_149(325) => lgrs_sym(326) __(31) wff_cmd_arg(323).
	p(NT(325), (NT(326)+NT(31)+NT(323)));
//G512: lgrs_cmd(324)        => __E_cli_command_149(325).
	p(NT(324), (NT(325)));
//G513: cli_command(286)     => lgrs_cmd(324).
	p(NT(286), (NT(324)));
//G514: __E___E_cli_command_150_151(330) => 'r'.
	p(NT(330), (T(19)));
//G515: __E___E_cli_command_150_151(330) => 'r' 'u' 'n'.
	p(NT(330), (T(19)+T(7)+T(5)));
//G516: run_sym(329)         => __E___E_cli_command_150_151(330).
	p(NT(329), (NT(330)));
//G517: __E___E_cli_command_150_152(331) => history(332).
	p(NT(331), (NT(332)));
//G518: __E___E_cli_command_150_152(331) => wff(14).
	p(NT(331), (NT(14)));
//G519: __E_cli_command_150(328) => run_sym(329) __(31) __E___E_cli_command_150_152(331).
	p(NT(328), (NT(329)+NT(31)+NT(331)));
//G520: run_cmd(327)         => __E_cli_command_150(328).
	p(NT(327), (NT(328)));
//G521: cli_command(286)     => run_cmd(327).
	p(NT(286), (NT(327)));
//G522: __E___E_cli_command_153_154(336) => 'n'.
	p(NT(336), (T(5)));
//G523: __E___E_cli_command_153_154(336) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(336), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(60)+T(11)));
//G524: normalize_sym(335)   => __E___E_cli_command_153_154(336).
	p(NT(335), (NT(336)));
//G525: __E_cli_command_153(334) => normalize_sym(335) __(31) normalize_cmd_arg(311).
	p(NT(334), (NT(335)+NT(31)+NT(311)));
//G526: normalize_cmd(333)   => __E_cli_command_153(334).
	p(NT(333), (NT(334)));
//G527: cli_command(286)     => normalize_cmd(333).
	p(NT(286), (NT(333)));
//G528: __E___E_cli_command_155_156(340) => 's'.
	p(NT(340), (T(15)));
//G529: __E___E_cli_command_155_156(340) => 's' 'u' 'b' 's' 't'.
	p(NT(340), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G530: __E___E_cli_command_155_156(340) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(340), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G531: subst_sym(339)       => __E___E_cli_command_155_156(340).
	p(NT(339), (NT(340)));
//G532: __E_cli_command_155(338) => subst_sym(339) __(31) nf_cmd_arg(341) _(11) '[' _(11) nf_cmd_arg(341) _(11) '/' _(11) nf_cmd_arg(341) _(11) ']'.
	p(NT(338), (NT(339)+NT(31)+NT(341)+NT(11)+T(26)+NT(11)+NT(341)+NT(11)+T(44)+NT(11)+NT(341)+NT(11)+T(27)));
//G533: subst_cmd(337)       => __E_cli_command_155(338).
	p(NT(337), (NT(338)));
//G534: cli_command(286)     => subst_cmd(337).
	p(NT(286), (NT(337)));
//G535: __E___E_cli_command_157_158(345) => 'i'.
	p(NT(345), (T(4)));
//G536: __E___E_cli_command_157_158(345) => 'i' 'n' 's' 't'.
	p(NT(345), (T(4)+T(5)+T(15)+T(8)));
//G537: __E___E_cli_command_157_158(345) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(345), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G538: inst_sym(344)        => __E___E_cli_command_157_158(345).
	p(NT(344), (NT(345)));
//G539: __E_cli_command_157(343) => inst_sym(344) __(31) inst_args(346).
	p(NT(343), (NT(344)+NT(31)+NT(346)));
//G540: inst_cmd(342)        => __E_cli_command_157(343).
	p(NT(342), (NT(343)));
//G541: cli_command(286)     => inst_cmd(342).
	p(NT(286), (NT(342)));
//G542: dnf_sym(349)         => 'd' 'n' 'f'.
	p(NT(349), (T(58)+T(5)+T(9)));
//G543: __E_cli_command_159(348) => dnf_sym(349) __(31) nf_cmd_arg(341).
	p(NT(348), (NT(349)+NT(31)+NT(341)));
//G544: dnf_cmd(347)         => __E_cli_command_159(348).
	p(NT(347), (NT(348)));
//G545: cli_command(286)     => dnf_cmd(347).
	p(NT(286), (NT(347)));
//G546: cnf_sym(352)         => 'c' 'n' 'f'.
	p(NT(352), (T(14)+T(5)+T(9)));
//G547: __E_cli_command_160(351) => cnf_sym(352) __(31) nf_cmd_arg(341).
	p(NT(351), (NT(352)+NT(31)+NT(341)));
//G548: cnf_cmd(350)         => __E_cli_command_160(351).
	p(NT(350), (NT(351)));
//G549: cli_command(286)     => cnf_cmd(350).
	p(NT(286), (NT(350)));
//G550: anf_sym(355)         => 'a' 'n' 'f'.
	p(NT(355), (T(16)+T(5)+T(9)));
//G551: __E_cli_command_161(354) => anf_sym(355) __(31) nf_cmd_arg(341).
	p(NT(354), (NT(355)+NT(31)+NT(341)));
//G552: anf_cmd(353)         => __E_cli_command_161(354).
	p(NT(353), (NT(354)));
//G553: cli_command(286)     => anf_cmd(353).
	p(NT(286), (NT(353)));
//G554: nnf_sym(358)         => 'n' 'n' 'f'.
	p(NT(358), (T(5)+T(5)+T(9)));
//G555: __E_cli_command_162(357) => nnf_sym(358) __(31) nf_cmd_arg(341).
	p(NT(357), (NT(358)+NT(31)+NT(341)));
//G556: nnf_cmd(356)         => __E_cli_command_162(357).
	p(NT(356), (NT(357)));
//G557: cli_command(286)     => nnf_cmd(356).
	p(NT(286), (NT(356)));
//G558: pnf_sym(361)         => 'p' 'n' 'f'.
	p(NT(361), (T(57)+T(5)+T(9)));
//G559: __E_cli_command_163(360) => pnf_sym(361) __(31) nf_cmd_arg(341).
	p(NT(360), (NT(361)+NT(31)+NT(341)));
//G560: pnf_cmd(359)         => __E_cli_command_163(360).
	p(NT(359), (NT(360)));
//G561: cli_command(286)     => pnf_cmd(359).
	p(NT(286), (NT(359)));
//G562: mnf_sym(364)         => 'm' 'n' 'f'.
	p(NT(364), (T(21)+T(5)+T(9)));
//G563: __E_cli_command_164(363) => mnf_sym(364) __(31) nf_cmd_arg(341).
	p(NT(363), (NT(364)+NT(31)+NT(341)));
//G564: mnf_cmd(362)         => __E_cli_command_164(363).
	p(NT(362), (NT(363)));
//G565: cli_command(286)     => mnf_cmd(362).
	p(NT(286), (NT(362)));
//G566: snf_sym(367)         => 's' 'n' 'f'.
	p(NT(367), (T(15)+T(5)+T(9)));
//G567: __E_cli_command_165(366) => snf_sym(367) __(31) nf_cmd_arg(341).
	p(NT(366), (NT(367)+NT(31)+NT(341)));
//G568: snf_cmd(365)         => __E_cli_command_165(366).
	p(NT(365), (NT(366)));
//G569: cli_command(286)     => snf_cmd(365).
	p(NT(286), (NT(365)));
//G570: onf_sym(370)         => 'o' 'n' 'f'.
	p(NT(370), (T(6)+T(5)+T(9)));
//G571: __E_cli_command_166(369) => onf_sym(370) __(31) variable(153) __(31) onf_cmd_arg(371).
	p(NT(369), (NT(370)+NT(31)+NT(153)+NT(31)+NT(371)));
//G572: onf_cmd(368)         => __E_cli_command_166(369).
	p(NT(368), (NT(369)));
//G573: cli_command(286)     => onf_cmd(368).
	p(NT(286), (NT(368)));
//G574: qelim_sym(374)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(374), (T(55)+T(11)+T(10)+T(4)+T(21)));
//G575: __E_cli_command_167(373) => qelim_sym(374) __(31) wff_cmd_arg(323).
	p(NT(373), (NT(374)+NT(31)+NT(323)));
//G576: qelim_cmd(372)       => __E_cli_command_167(373).
	p(NT(372), (NT(373)));
//G577: cli_command(286)     => qelim_cmd(372).
	p(NT(286), (NT(372)));
//G578: get_sym(377)         => 'g' 'e' 't'.
	p(NT(377), (T(59)+T(11)+T(8)));
//G579: __E___E_cli_command_168_169(378) => __(31) option_name(379).
	p(NT(378), (NT(31)+NT(379)));
//G580: __E___E_cli_command_168_169(378) => null.
	p(NT(378), (nul));
//G581: __E_cli_command_168(376) => get_sym(377) __E___E_cli_command_168_169(378).
	p(NT(376), (NT(377)+NT(378)));
//G582: get_cmd(375)         => __E_cli_command_168(376).
	p(NT(375), (NT(376)));
//G583: cli_command(286)     => get_cmd(375).
	p(NT(286), (NT(375)));
//G584: set_sym(382)         => 's' 'e' 't'.
	p(NT(382), (T(15)+T(11)+T(8)));
//G585: __E___E_cli_command_170_171(383) => __(31).
	p(NT(383), (NT(31)));
//G586: __E___E_cli_command_170_171(383) => _(11) '=' _(11).
	p(NT(383), (NT(11)+T(3)+NT(11)));
//G587: __E_cli_command_170(381) => set_sym(382) __(31) option_name(379) __E___E_cli_command_170_171(383) option_value(384).
	p(NT(381), (NT(382)+NT(31)+NT(379)+NT(383)+NT(384)));
//G588: set_cmd(380)         => __E_cli_command_170(381).
	p(NT(380), (NT(381)));
//G589: cli_command(286)     => set_cmd(380).
	p(NT(286), (NT(380)));
//G590: enable_sym(387)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(387), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G591: __E_cli_command_172(386) => enable_sym(387) __(31) option_name(379).
	p(NT(386), (NT(387)+NT(31)+NT(379)));
//G592: enable_cmd(385)      => __E_cli_command_172(386).
	p(NT(385), (NT(386)));
//G593: cli_command(286)     => enable_cmd(385).
	p(NT(286), (NT(385)));
//G594: disable_sym(390)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(390), (T(58)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G595: __E_cli_command_173(389) => disable_sym(390) __(31) option_name(379).
	p(NT(389), (NT(390)+NT(31)+NT(379)));
//G596: disable_cmd(388)     => __E_cli_command_173(389).
	p(NT(388), (NT(389)));
//G597: cli_command(286)     => disable_cmd(388).
	p(NT(286), (NT(388)));
//G598: toggle_sym(393)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(393), (T(8)+T(6)+T(59)+T(59)+T(10)+T(11)));
//G599: __E_cli_command_174(392) => toggle_sym(393) __(31) option_name(379).
	p(NT(392), (NT(393)+NT(31)+NT(379)));
//G600: toggle_cmd(391)      => __E_cli_command_174(392).
	p(NT(391), (NT(392)));
//G601: cli_command(286)     => toggle_cmd(391).
	p(NT(286), (NT(391)));
//G602: __E___E_cli_command_175_176(397) => 'd' 'e' 'f' 's'.
	p(NT(397), (T(58)+T(11)+T(9)+T(15)));
//G603: __E___E_cli_command_175_176(397) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(397), (T(58)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G604: def_sym(396)         => __E___E_cli_command_175_176(397).
	p(NT(396), (NT(397)));
//G605: __E_cli_command_175(395) => def_sym(396).
	p(NT(395), (NT(396)));
//G606: def_list_cmd(394)    => __E_cli_command_175(395).
	p(NT(394), (NT(395)));
//G607: cli_command(286)     => def_list_cmd(394).
	p(NT(286), (NT(394)));
//G608: __E_cli_command_177(399) => def_sym(396) __(31) num(219).
	p(NT(399), (NT(396)+NT(31)+NT(219)));
//G609: def_print_cmd(398)   => __E_cli_command_177(399).
	p(NT(398), (NT(399)));
//G610: cli_command(286)     => def_print_cmd(398).
	p(NT(286), (NT(398)));
//G611: def_rr_cmd(400)      => rec_relation(17).
	p(NT(400), (NT(17)));
//G612: cli_command(286)     => def_rr_cmd(400).
	p(NT(286), (NT(400)));
//G613: def_input_cmd(401)   => input_def(18).
	p(NT(401), (NT(18)));
//G614: cli_command(286)     => def_input_cmd(401).
	p(NT(286), (NT(401)));
//G615: def_output_cmd(402)  => output_def(19).
	p(NT(402), (NT(19)));
//G616: cli_command(286)     => def_output_cmd(402).
	p(NT(286), (NT(402)));
//G617: __E___E_cli_command_178_179(406) => 'h' 'i' 's' 't'.
	p(NT(406), (T(56)+T(4)+T(15)+T(8)));
//G618: __E___E_cli_command_178_179(406) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(406), (T(56)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G619: history_sym(405)     => __E___E_cli_command_178_179(406).
	p(NT(405), (NT(406)));
//G620: __E_cli_command_178(404) => history_sym(405).
	p(NT(404), (NT(405)));
//G621: history_list_cmd(403) => __E_cli_command_178(404).
	p(NT(403), (NT(404)));
//G622: cli_command(286)     => history_list_cmd(403).
	p(NT(286), (NT(403)));
//G623: __E_cli_command_180(408) => history_sym(405) __(31) history(332).
	p(NT(408), (NT(405)+NT(31)+NT(332)));
//G624: history_print_cmd(407) => __E_cli_command_180(408).
	p(NT(407), (NT(408)));
//G625: cli_command(286)     => history_print_cmd(407).
	p(NT(286), (NT(407)));
//G626: __E_cli_command_181(410) => wff(14).
	p(NT(410), (NT(14)));
//G627: __E_cli_command_181(410) => bf(27).
	p(NT(410), (NT(27)));
//G628: history_store_cmd(409) => __E_cli_command_181(410).
	p(NT(409), (NT(410)));
//G629: cli_command(286)     => history_store_cmd(409).
	p(NT(286), (NT(409)));
//G630: __E_solve_options_182(411) => __(31) solve_option(412).
	p(NT(411), (NT(31)+NT(412)));
//G631: __E_solve_options_183(413) => null.
	p(NT(413), (nul));
//G632: __E_solve_options_183(413) => __E_solve_options_182(411) __E_solve_options_183(413).
	p(NT(413), (NT(411)+NT(413)));
//G633: solve_options(322)   => __E_solve_options_183(413).
	p(NT(322), (NT(413)));
//G634: __E_solve_option_184(414) => solver_mode(415).
	p(NT(414), (NT(415)));
//G635: __E_solve_option_184(414) => type(278).
	p(NT(414), (NT(278)));
//G636: solve_option(412)    => '-' '-' __E_solve_option_184(414).
	p(NT(412), (T(30)+T(30)+NT(414)));
//G637: __E_solver_mode_185(417) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(417), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G638: __E_solver_mode_185(417) => 'm' 'i' 'n'.
	p(NT(417), (T(21)+T(4)+T(5)));
//G639: solver_mode_minimum(416) => __E_solver_mode_185(417).
	p(NT(416), (NT(417)));
//G640: solver_mode(415)     => solver_mode_minimum(416).
	p(NT(415), (NT(416)));
//G641: __E_solver_mode_186(419) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(419), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G642: __E_solver_mode_186(419) => 'm' 'a' 'x'.
	p(NT(419), (T(21)+T(16)+T(29)));
//G643: solver_mode_maximum(418) => __E_solver_mode_186(419).
	p(NT(418), (NT(419)));
//G644: solver_mode(415)     => solver_mode_maximum(418).
	p(NT(415), (NT(418)));
//G645: q_file_name(35)      => '"' file_name(420) '"'.
	p(NT(35), (T(61)+NT(420)+T(61)));
//G646: __E_file_name_187(421) => printable(8).
	p(NT(421), (NT(8)));
//G647: __E_file_name_187(421) => printable(8) __E_file_name_187(421).
	p(NT(421), (NT(8)+NT(421)));
//G648: file_name(420)       => __E_file_name_187(421).
	p(NT(420), (NT(421)));
//G649: __E_option_name_188(422) => alnum(6).
	p(NT(422), (NT(6)));
//G650: __E_option_name_188(422) => alnum(6) __E_option_name_188(422).
	p(NT(422), (NT(6)+NT(422)));
//G651: option_name(379)     => __E_option_name_188(422).
	p(NT(379), (NT(422)));
//G652: __E_option_value_189(423) => alnum(6).
	p(NT(423), (NT(6)));
//G653: __E_option_value_189(423) => alnum(6) __E_option_value_189(423).
	p(NT(423), (NT(6)+NT(423)));
//G654: option_value(384)    => __E_option_value_189(423).
	p(NT(384), (NT(423)));
//G655: bf_cmd_arg(424)      => history(332).
	p(NT(424), (NT(332)));
//G656: bf_cmd_arg(424)      => bf(27).
	p(NT(424), (NT(27)));
//G657: wff_cmd_arg(323)     => history(332).
	p(NT(323), (NT(332)));
//G658: wff_cmd_arg(323)     => wff(14).
	p(NT(323), (NT(14)));
//G659: nf_cmd_arg(341)      => history(332).
	p(NT(341), (NT(332)));
//G660: nf_cmd_arg(341)      => ref(24).
	p(NT(341), (NT(24)));
//G661: nf_cmd_arg(341)      => wff(14).
	p(NT(341), (NT(14)));
//G662: nf_cmd_arg(341)      => bf(27).
	p(NT(341), (NT(27)));
//G663: onf_cmd_arg(371)     => history(332).
	p(NT(371), (NT(332)));
//G664: onf_cmd_arg(371)     => wff(14).
	p(NT(371), (NT(14)));
//G665: normalize_cmd_arg(311) => history(332).
	p(NT(311), (NT(332)));
//G666: normalize_cmd_arg(311) => spec(10).
	p(NT(311), (NT(10)));
//G667: normalize_cmd_arg(311) => ref(24).
	p(NT(311), (NT(24)));
//G668: normalize_cmd_arg(311) => wff(14).
	p(NT(311), (NT(14)));
//G669: normalize_cmd_arg(311) => bf(27).
	p(NT(311), (NT(27)));
//G670: inst_args(346)       => wff_cmd_arg(323) _(11) '[' _(11) variable(153) _(11) '/' _(11) bf_cmd_arg(424) _(11) ']'.
	p(NT(346), (NT(323)+NT(11)+T(26)+NT(11)+NT(153)+NT(11)+T(44)+NT(11)+NT(424)+NT(11)+T(27)));
//G671: inst_args(346)       => bf_cmd_arg(424) _(11) '[' _(11) variable(153) _(11) '/' _(11) bf_cmd_arg(424) _(11) ']'.
	p(NT(346), (NT(424)+NT(11)+T(26)+NT(11)+NT(153)+NT(11)+T(44)+NT(11)+NT(424)+NT(11)+T(27)));
//G672: help_arg(303)        => help_sym(300).
	p(NT(303), (NT(300)));
//G673: help_arg(303)        => version_sym(293).
	p(NT(303), (NT(293)));
//G674: help_arg(303)        => quit_sym(290).
	p(NT(303), (NT(290)));
//G675: help_arg(303)        => clear_sym(296).
	p(NT(303), (NT(296)));
//G676: help_arg(303)        => get_sym(377).
	p(NT(303), (NT(377)));
//G677: help_arg(303)        => set_sym(382).
	p(NT(303), (NT(382)));
//G678: help_arg(303)        => enable_sym(387).
	p(NT(303), (NT(387)));
//G679: help_arg(303)        => disable_sym(390).
	p(NT(303), (NT(390)));
//G680: help_arg(303)        => toggle_sym(393).
	p(NT(303), (NT(393)));
//G681: help_arg(303)        => file_sym(306).
	p(NT(303), (NT(306)));
//G682: help_arg(303)        => history_sym(405).
	p(NT(303), (NT(405)));
//G683: help_arg(303)        => abs_history_sym(425).
	p(NT(303), (NT(425)));
//G684: help_arg(303)        => rel_history_sym(426).
	p(NT(303), (NT(426)));
//G685: help_arg(303)        => selection_sym(427).
	p(NT(303), (NT(427)));
//G686: help_arg(303)        => def_sym(396).
	p(NT(303), (NT(396)));
//G687: help_arg(303)        => inst_sym(344).
	p(NT(303), (NT(344)));
//G688: help_arg(303)        => subst_sym(339).
	p(NT(303), (NT(339)));
//G689: help_arg(303)        => normalize_sym(335).
	p(NT(303), (NT(335)));
//G690: help_arg(303)        => execute_sym(428).
	p(NT(303), (NT(428)));
//G691: help_arg(303)        => solve_sym(320).
	p(NT(303), (NT(320)));
//G692: help_arg(303)        => valid_sym(310).
	p(NT(303), (NT(310)));
//G693: help_arg(303)        => sat_sym(314).
	p(NT(303), (NT(314)));
//G694: help_arg(303)        => unsat_sym(317).
	p(NT(303), (NT(317)));
//G695: help_arg(303)        => run_sym(329).
	p(NT(303), (NT(329)));
//G696: help_arg(303)        => dnf_sym(349).
	p(NT(303), (NT(349)));
//G697: help_arg(303)        => cnf_sym(352).
	p(NT(303), (NT(352)));
//G698: help_arg(303)        => anf_sym(355).
	p(NT(303), (NT(355)));
//G699: help_arg(303)        => snf_sym(367).
	p(NT(303), (NT(367)));
//G700: help_arg(303)        => nnf_sym(358).
	p(NT(303), (NT(358)));
//G701: help_arg(303)        => pnf_sym(361).
	p(NT(303), (NT(361)));
//G702: help_arg(303)        => mnf_sym(364).
	p(NT(303), (NT(364)));
//G703: help_arg(303)        => onf_sym(370).
	p(NT(303), (NT(370)));
//G704: help_arg(303)        => qelim_sym(374).
	p(NT(303), (NT(374)));
//G705: __E___E_help_arg_190_191(431) => 's'.
	p(NT(431), (T(15)));
//G706: __E___E_help_arg_190_191(431) => null.
	p(NT(431), (nul));
//G707: __E_help_arg_190(430) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_190_191(431).
	p(NT(430), (T(11)+T(29)+T(16)+T(21)+T(57)+T(10)+T(11)+NT(431)));
//G708: examples_sym(429)    => __E_help_arg_190(430).
	p(NT(429), (NT(430)));
//G709: help_arg(303)        => examples_sym(429).
	p(NT(303), (NT(429)));
//G710: __E_history_192(433) => '%'.
	p(NT(433), (T(45)));
//G711: rel_history_sym(426) => '%' '-'.
	p(NT(426), (T(45)+T(30)));
//G712: history_id(435)      => digits(275).
	p(NT(435), (NT(275)));
//G713: __E___E_history_192_193(434) => history_id(435).
	p(NT(434), (NT(435)));
//G714: __E___E_history_192_193(434) => null.
	p(NT(434), (nul));
//G715: __E_history_192(433) => rel_history_sym(426) __E___E_history_192_193(434).
	p(NT(433), (NT(426)+NT(434)));
//G716: rel_history(432)     => __E_history_192(433).
	p(NT(432), (NT(433)));
//G717: history(332)         => rel_history(432).
	p(NT(332), (NT(432)));
//G718: abs_history_sym(425) => '%'.
	p(NT(425), (T(45)));
//G719: __E_history_194(437) => abs_history_sym(425) history_id(435).
	p(NT(437), (NT(425)+NT(435)));
//G720: abs_history(436)     => __E_history_194(437).
	p(NT(436), (NT(437)));
//G721: history(332)         => abs_history(436).
	p(NT(332), (NT(436)));
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
		spec, _, definitions, main, wff, __E_definitions_0, __E___E_definitions_0_1, rec_relation, input_def, output_def, 
		__E_definitions_2, rec_relations, __E_rec_relations_3, __E_rec_relations_4, ref, __E_rec_relation_5, capture, bf, io_var_name, __E_input_def_6, 
		typed, __, stream, __E_output_def_7, __E_output_def_8, q_file_name, console_sym, sym, __E_ref_9, offsets, 
		ref_args, __E_ref_10, fp_fallback, first_sym, last_sym, __E_ref_args_11, __E___E_ref_args_11_12, ref_arg, __E___E_ref_args_11_13, __E___E_ref_args_11_14, 
		library, rules, __E_library_15, __E___E_library_15_16, rule, __E___E___E_library_15_16_17, wff_rule, bf_rule, __E___E_library_15_18, wff_matcher, 
		wff_body, bf_matcher, bf_body, builder, builder_head, builder_body, __E_builder_head_19, __E_builder_head_20, bf_builder_body, __E_builder_body_21, 
		wff_builder_body, __E_builder_body_22, tau_constant_source, __E_tau_constant_source_23, wff_parenthesis, __E_wff_24, wff_sometimes, __E_wff_25, __E___E_wff_25_26, wff_always, 
		__E_wff_27, __E___E_wff_27_28, wff_conditional, __E_wff_29, wff_all, __E_wff_30, q_vars, wff_ex, __E_wff_31, wff_ref, 
		constraint, wff_imply, __E_wff_32, wff_rimply, __E_wff_33, wff_equiv, __E_wff_34, wff_or, __E_wff_35, wff_xor, 
		__E_wff_36, wff_and, __E_wff_37, wff_neg, __E_wff_38, wff_t, wff_f, bf_interval, __E_wff_39, bf_eq, 
		__E_wff_40, bf_neq, __E_wff_41, bf_lteq, __E_wff_42, bf_nlteq, __E_wff_43, bf_gt, __E_wff_44, bf_ngt, 
		__E_wff_45, bf_gteq, __E_wff_46, bf_ngteq, __E_wff_47, bf_lt, __E_wff_48, bf_nlt, __E_wff_49, bv_eq, 
		__E_wff_50, bv, bv_neq, __E_wff_51, bv_less_equal, __E_wff_52, bv_nleq, __E_wff_53, bv_greater, __E_wff_54, 
		bv_ngreater, __E_wff_55, bv_greater_equal, __E_wff_56, bv_ngeq, __E_wff_57, bv_less, __E_wff_58, bv_nless, __E_wff_59, 
		bf_parenthesis, __E_bf_60, bf_constant, variable, bf_splitter, __E_bf_61, bf_ref, bf_or, __E_bf_62, bf_xor, 
		__E_bf_63, bf_t, __E_bf_64, __E___E_bf_64_65, bf_f, __E_bf_66, __E___E_bf_66_67, bf_and, __E_bf_68, __E___E_bf_68_69, 
		bf_neg, __E_bf_70, bf_neg_oprnd, __E___E_bf_70_71, bf_and_nosep, __E_bf_72, bf_and_nosep_1st_oprnd, __E___E_bf_72_73, bf_and_nosep_2nd_oprnd, __E___E_bf_72_74, 
		bv_parenthesis, __E_bv_75, bv_checked, __E_bv_76, bv_add, __E_bv_77, bv_mul, __E_bv_78, bv_div, __E_bv_79, 
		bv_mod, __E_bv_80, bv_sub, __E_bv_81, bv_neg, bv_and, __E_bv_82, bv_nand, __E_bv_83, bv_or, 
		__E_bv_84, bv_nor, __E_bv_85, bv_xor, __E_bv_86, bv_xnor, __E_bv_87, bv_rotate_left, __E_bv_88, bv_rotate_right, 
		__E_bv_89, bv_min, __E_bv_90, bv_max, __E_bv_91, bitvector, bits, __E_bitvector_92, bv_type, num, 
		__E_bitvector_93, hexnum, __E_bitvector_94, __E_bits_95, __E_bits_96, __E_hexnum_97, ctn_neq, __E_constraint_98, ctnvar, ctn_eq, 
		__E_constraint_99, ctn_gteq, __E_constraint_100, ctn_gt, __E_constraint_101, ctn_lteq, __E_constraint_102, ctn_lt, __E_constraint_103, __E_bf_constant_104, 
		source, __E_bf_constant_105, src_c, __E_source_106, __E___E_source_106_107, __E___E_source_106_108, __E_src_c_109, __E_src_c_110, __E_variable_111, uconst, 
		io_var, var_name, __E_variable_112, __E_var_name_113, __E_var_name_114, __E_var_name_115, __E_var_name_116, __E_var_name_117, offset, chars, 
		uconst_name, __E_uconst_118, __E___E_uconst_118_119, q_var, __E_q_vars_120, __E_q_vars_121, __E_offsets_122, __E_offsets_123, integer, shift, 
		__E_offset_124, __E_shift_125, __E___E_shift_125_126, __E_chars_127, __E_chars_128, digits, __E_digits_129, __E_integer_130, type, __E_bv_type_131, 
		scope_id, comment, __E_comment_132, __E_comment_133, __E_comment_134, cli, cli_command, __E_cli_135, __E_cli_136, quit_cmd, 
		quit_sym, __E_cli_command_137, version_cmd, version_sym, __E_cli_command_138, clear_cmd, clear_sym, __E_cli_command_139, help_cmd, __E_cli_command_140, 
		help_sym, __E___E_cli_command_140_141, __E___E_cli_command_140_142, help_arg, file_cmd, __E_cli_command_143, file_sym, q_string, valid_cmd, __E_cli_command_144, 
		valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_145, sat_sym, unsat_cmd, __E_cli_command_146, unsat_sym, solve_cmd, __E_cli_command_147, 
		solve_sym, __E___E_cli_command_147_148, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_149, lgrs_sym, run_cmd, __E_cli_command_150, run_sym, 
		__E___E_cli_command_150_151, __E___E_cli_command_150_152, history, normalize_cmd, __E_cli_command_153, normalize_sym, __E___E_cli_command_153_154, subst_cmd, __E_cli_command_155, subst_sym, 
		__E___E_cli_command_155_156, nf_cmd_arg, inst_cmd, __E_cli_command_157, inst_sym, __E___E_cli_command_157_158, inst_args, dnf_cmd, __E_cli_command_159, dnf_sym, 
		cnf_cmd, __E_cli_command_160, cnf_sym, anf_cmd, __E_cli_command_161, anf_sym, nnf_cmd, __E_cli_command_162, nnf_sym, pnf_cmd, 
		__E_cli_command_163, pnf_sym, mnf_cmd, __E_cli_command_164, mnf_sym, snf_cmd, __E_cli_command_165, snf_sym, onf_cmd, __E_cli_command_166, 
		onf_sym, onf_cmd_arg, qelim_cmd, __E_cli_command_167, qelim_sym, get_cmd, __E_cli_command_168, get_sym, __E___E_cli_command_168_169, option_name, 
		set_cmd, __E_cli_command_170, set_sym, __E___E_cli_command_170_171, option_value, enable_cmd, __E_cli_command_172, enable_sym, disable_cmd, __E_cli_command_173, 
		disable_sym, toggle_cmd, __E_cli_command_174, toggle_sym, def_list_cmd, __E_cli_command_175, def_sym, __E___E_cli_command_175_176, def_print_cmd, __E_cli_command_177, 
		def_rr_cmd, def_input_cmd, def_output_cmd, history_list_cmd, __E_cli_command_178, history_sym, __E___E_cli_command_178_179, history_print_cmd, __E_cli_command_180, history_store_cmd, 
		__E_cli_command_181, __E_solve_options_182, solve_option, __E_solve_options_183, __E_solve_option_184, solver_mode, solver_mode_minimum, __E_solver_mode_185, solver_mode_maximum, __E_solver_mode_186, 
		file_name, __E_file_name_187, __E_option_name_188, __E_option_value_189, bf_cmd_arg, abs_history_sym, rel_history_sym, selection_sym, execute_sym, examples_sym, 
		__E_help_arg_190, __E___E_help_arg_190_191, rel_history, __E_history_192, __E___E_history_192_193, history_id, abs_history, __E_history_194, __N_0, __N_1, 
		__N_2, __N_3, __N_4, __N_5, __N_6, __N_7, __N_8, 
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
