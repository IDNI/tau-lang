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
	"__E_bv_89", "bv_min", "__E_bv_90", "bv_max", "__E_bv_91", "bv_constant", "binary", "__E_bv_constant_92", "bv_type", "decimal", 
	"__E_bv_constant_93", "hexadecimal", "__E_bv_constant_94", "__E_binary_95", "__E_binary_96", "__E_hexadecimal_97", "digits", "__E_decimal_98", "ctn_neq", "__E_constraint_99", 
	"ctnvar", "num", "ctn_eq", "__E_constraint_100", "ctn_gteq", "__E_constraint_101", "ctn_gt", "__E_constraint_102", "ctn_lteq", "__E_constraint_103", 
	"ctn_lt", "__E_constraint_104", "__E_bf_constant_105", "source", "__E_bf_constant_106", "src_c", "__E_source_107", "__E___E_source_107_108", "__E___E_source_107_109", "__E_src_c_110", 
	"__E_src_c_111", "__E_variable_112", "uconst", "io_var", "var_name", "__E_variable_113", "__E_var_name_114", "__E_var_name_115", "__E_var_name_116", "__E_var_name_117", 
	"__E_var_name_118", "offset", "chars", "uconst_name", "__E_uconst_119", "__E___E_uconst_119_120", "q_var", "__E_q_vars_121", "__E_q_vars_122", "__E_offsets_123", 
	"__E_offsets_124", "integer", "shift", "__E_offset_125", "__E_shift_126", "__E___E_shift_126_127", "__E_chars_128", "__E_chars_129", "__E_digits_130", "__E_integer_131", 
	"type", "__E_bv_type_132", "scope_id", "comment", "__E_comment_133", "__E_comment_134", "__E_comment_135", "cli", "cli_command", "__E_cli_136", 
	"__E_cli_137", "quit_cmd", "quit_sym", "__E_cli_command_138", "version_cmd", "version_sym", "__E_cli_command_139", "clear_cmd", "clear_sym", "__E_cli_command_140", 
	"help_cmd", "__E_cli_command_141", "help_sym", "__E___E_cli_command_141_142", "__E___E_cli_command_141_143", "help_arg", "file_cmd", "__E_cli_command_144", "file_sym", "q_string", 
	"valid_cmd", "__E_cli_command_145", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_146", "sat_sym", "unsat_cmd", "__E_cli_command_147", "unsat_sym", 
	"solve_cmd", "__E_cli_command_148", "solve_sym", "__E___E_cli_command_148_149", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_150", "lgrs_sym", "run_cmd", 
	"__E_cli_command_151", "run_sym", "__E___E_cli_command_151_152", "__E___E_cli_command_151_153", "history", "normalize_cmd", "__E_cli_command_154", "normalize_sym", "__E___E_cli_command_154_155", "subst_cmd", 
	"__E_cli_command_156", "subst_sym", "__E___E_cli_command_156_157", "nf_cmd_arg", "inst_cmd", "__E_cli_command_158", "inst_sym", "__E___E_cli_command_158_159", "inst_args", "dnf_cmd", 
	"__E_cli_command_160", "dnf_sym", "cnf_cmd", "__E_cli_command_161", "cnf_sym", "anf_cmd", "__E_cli_command_162", "anf_sym", "nnf_cmd", "__E_cli_command_163", 
	"nnf_sym", "pnf_cmd", "__E_cli_command_164", "pnf_sym", "mnf_cmd", "__E_cli_command_165", "mnf_sym", "snf_cmd", "__E_cli_command_166", "snf_sym", 
	"onf_cmd", "__E_cli_command_167", "onf_sym", "onf_cmd_arg", "qelim_cmd", "__E_cli_command_168", "qelim_sym", "get_cmd", "__E_cli_command_169", "get_sym", 
	"__E___E_cli_command_169_170", "option_name", "set_cmd", "__E_cli_command_171", "set_sym", "__E___E_cli_command_171_172", "option_value", "enable_cmd", "__E_cli_command_173", "enable_sym", 
	"disable_cmd", "__E_cli_command_174", "disable_sym", "toggle_cmd", "__E_cli_command_175", "toggle_sym", "def_list_cmd", "__E_cli_command_176", "def_sym", "__E___E_cli_command_176_177", 
	"def_print_cmd", "__E_cli_command_178", "def_rr_cmd", "def_input_cmd", "def_output_cmd", "history_list_cmd", "__E_cli_command_179", "history_sym", "__E___E_cli_command_179_180", "history_print_cmd", 
	"__E_cli_command_181", "history_store_cmd", "__E_cli_command_182", "__E_solve_options_183", "solve_option", "__E_solve_options_184", "__E_solve_option_185", "solver_mode", "solver_mode_minimum", "__E_solver_mode_186", 
	"solver_mode_maximum", "__E_solver_mode_187", "file_name", "__E_file_name_188", "__E_option_name_189", "__E_option_value_190", "bf_cmd_arg", "abs_history_sym", "rel_history_sym", "selection_sym", 
	"execute_sym", "examples_sym", "__E_help_arg_191", "__E___E_help_arg_191_192", "rel_history", "__E_history_193", "__E___E_history_193_194", "history_id", "abs_history", "__E_history_195", 
	"__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
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
			26, 28, 37, 216, 218, 219, 221, 226, 230, 243,
			254, 263, 271, 280, 381, 386, 422
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
			{ 245 },
			{ 252 },
			{ 252, 263 },
			{ 262 },
			{ 266 },
			{ 313 },
			{ 325 },
			{ 343 },
			{ 348 },
			{ 373 },
			{ 426 }
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
//G312: bv(131)              => bv_constant(215).
	p(NT(131), (NT(215)));
//G313: __E_bv_constant_92(217) => _(11) ':' _(11) bv_type(218) _(11).
	p(NT(217), (NT(11)+T(2)+NT(11)+NT(218)+NT(11)));
//G314: __E_bv_constant_92(217) => null.
	p(NT(217), (nul));
//G315: bv_constant(215)     => '#' 'b' binary(216) __E_bv_constant_92(217).
	p(NT(215), (T(47)+T(17)+NT(216)+NT(217)));
//G316: __E_bv_constant_93(220) => _(11) ':' _(11) bv_type(218) _(11).
	p(NT(220), (NT(11)+T(2)+NT(11)+NT(218)+NT(11)));
//G317: __E_bv_constant_93(220) => null.
	p(NT(220), (nul));
//G318: bv_constant(215)     => decimal(219) __E_bv_constant_93(220).
	p(NT(215), (NT(219)+NT(220)));
//G319: __E_bv_constant_94(222) => _(11) ':' _(11) bv_type(218) _(11).
	p(NT(222), (NT(11)+T(2)+NT(11)+NT(218)+NT(11)));
//G320: __E_bv_constant_94(222) => null.
	p(NT(222), (nul));
//G321: bv_constant(215)     => '#' 'x' hexadecimal(221) __E_bv_constant_94(222).
	p(NT(215), (T(47)+T(29)+NT(221)+NT(222)));
//G322: __E_binary_95(223)   => '0'.
	p(NT(223), (T(41)));
//G323: __E_binary_95(223)   => '1'.
	p(NT(223), (T(40)));
//G324: __E_binary_96(224)   => __E_binary_95(223).
	p(NT(224), (NT(223)));
//G325: __E_binary_96(224)   => __E_binary_95(223) __E_binary_96(224).
	p(NT(224), (NT(223)+NT(224)));
//G326: binary(216)          => __E_binary_96(224).
	p(NT(216), (NT(224)));
//G327: __E_hexadecimal_97(225) => xdigit(4).
	p(NT(225), (NT(4)));
//G328: __E_hexadecimal_97(225) => xdigit(4) __E_hexadecimal_97(225).
	p(NT(225), (NT(4)+NT(225)));
//G329: hexadecimal(221)     => __E_hexadecimal_97(225).
	p(NT(221), (NT(225)));
//G330: __E_decimal_98(227)  => digits(226).
	p(NT(227), (NT(226)));
//G331: __E_decimal_98(227)  => digits(226) __E_decimal_98(227).
	p(NT(227), (NT(226)+NT(227)));
//G332: decimal(219)         => __E_decimal_98(227).
	p(NT(219), (NT(227)));
//G333: __E_constraint_99(229) => '[' ctnvar(230) _(11) '!' '=' _(11) num(231) ']'.
	p(NT(229), (T(26)+NT(230)+NT(11)+T(34)+T(3)+NT(11)+NT(231)+T(27)));
//G334: __E_constraint_99(229) => '[' num(231) _(11) '!' '=' _(11) ctnvar(230) ']'.
	p(NT(229), (T(26)+NT(231)+NT(11)+T(34)+T(3)+NT(11)+NT(230)+T(27)));
//G335: ctn_neq(228)         => __E_constraint_99(229).
	p(NT(228), (NT(229)));
//G336: constraint(90)       => ctn_neq(228).
	p(NT(90), (NT(228)));
//G337: __E_constraint_100(233) => '[' ctnvar(230) _(11) '=' _(11) num(231) ']'.
	p(NT(233), (T(26)+NT(230)+NT(11)+T(3)+NT(11)+NT(231)+T(27)));
//G338: __E_constraint_100(233) => '[' num(231) _(11) '=' _(11) ctnvar(230) ']'.
	p(NT(233), (T(26)+NT(231)+NT(11)+T(3)+NT(11)+NT(230)+T(27)));
//G339: ctn_eq(232)          => __E_constraint_100(233).
	p(NT(232), (NT(233)));
//G340: constraint(90)       => ctn_eq(232).
	p(NT(90), (NT(232)));
//G341: __E_constraint_101(235) => '[' ctnvar(230) _(11) '>' '=' _(11) num(231) ']'.
	p(NT(235), (T(26)+NT(230)+NT(11)+T(23)+T(3)+NT(11)+NT(231)+T(27)));
//G342: __E_constraint_101(235) => '[' num(231) _(11) '>' '=' _(11) ctnvar(230) ']'.
	p(NT(235), (T(26)+NT(231)+NT(11)+T(23)+T(3)+NT(11)+NT(230)+T(27)));
//G343: ctn_gteq(234)        => __E_constraint_101(235).
	p(NT(234), (NT(235)));
//G344: constraint(90)       => ctn_gteq(234).
	p(NT(90), (NT(234)));
//G345: __E_constraint_102(237) => '[' ctnvar(230) _(11) '>' _(11) num(231) ']'.
	p(NT(237), (T(26)+NT(230)+NT(11)+T(23)+NT(11)+NT(231)+T(27)));
//G346: __E_constraint_102(237) => '[' num(231) _(11) '>' _(11) ctnvar(230) ']'.
	p(NT(237), (T(26)+NT(231)+NT(11)+T(23)+NT(11)+NT(230)+T(27)));
//G347: ctn_gt(236)          => __E_constraint_102(237).
	p(NT(236), (NT(237)));
//G348: constraint(90)       => ctn_gt(236).
	p(NT(90), (NT(236)));
//G349: __E_constraint_103(239) => '[' ctnvar(230) _(11) '<' '=' _(11) num(231) ']'.
	p(NT(239), (T(26)+NT(230)+NT(11)+T(22)+T(3)+NT(11)+NT(231)+T(27)));
//G350: __E_constraint_103(239) => '[' num(231) _(11) '<' '=' _(11) ctnvar(230) ']'.
	p(NT(239), (T(26)+NT(231)+NT(11)+T(22)+T(3)+NT(11)+NT(230)+T(27)));
//G351: ctn_lteq(238)        => __E_constraint_103(239).
	p(NT(238), (NT(239)));
//G352: constraint(90)       => ctn_lteq(238).
	p(NT(90), (NT(238)));
//G353: __E_constraint_104(241) => '[' ctnvar(230) _(11) '<' _(11) num(231) ']'.
	p(NT(241), (T(26)+NT(230)+NT(11)+T(22)+NT(11)+NT(231)+T(27)));
//G354: __E_constraint_104(241) => '[' num(231) _(11) '<' _(11) ctnvar(230) ']'.
	p(NT(241), (T(26)+NT(231)+NT(11)+T(22)+NT(11)+NT(230)+T(27)));
//G355: ctn_lt(240)          => __E_constraint_104(241).
	p(NT(240), (NT(241)));
//G356: constraint(90)       => ctn_lt(240).
	p(NT(90), (NT(240)));
//G357: __E_bf_constant_105(242) => capture(26).
	p(NT(242), (NT(26)));
//G358: __E_bf_constant_105(242) => source(243).
	p(NT(242), (NT(243)));
//G359: __E_bf_constant_106(244) => typed(30).
	p(NT(244), (NT(30)));
//G360: __E_bf_constant_106(244) => null.
	p(NT(244), (nul));
//G361: bf_constant(152)     => '{' _(11) __E_bf_constant_105(242) _(11) '}' __E_bf_constant_106(244).
	p(NT(152), (T(48)+NT(11)+NT(242)+NT(11)+T(49)+NT(244)));
//G362: __E___E_source_107_108(247) => src_c(245).
	p(NT(247), (NT(245)));
//G363: __E___E_source_107_108(247) => space(2).
	p(NT(247), (NT(2)));
//G364: __E___E_source_107_109(248) => null.
	p(NT(248), (nul));
//G365: __E___E_source_107_109(248) => __E___E_source_107_108(247) __E___E_source_107_109(248).
	p(NT(248), (NT(247)+NT(248)));
//G366: __E_source_107(246)  => __E___E_source_107_109(248) src_c(245).
	p(NT(246), (NT(248)+NT(245)));
//G367: __E_source_107(246)  => null.
	p(NT(246), (nul));
//G368: source(243)          => src_c(245) __E_source_107(246).
	p(NT(243), (NT(245)+NT(246)));
//G369: src_c(245)           => alnum(6).
	p(NT(245), (NT(6)));
//G370: __N_0(440)           => '{'.
	p(NT(440), (T(48)));
//G371: __N_1(441)           => '}'.
	p(NT(441), (T(49)));
//G372: src_c(245)           => ~( __N_0(440) ) & ~( __N_1(441) ) & punct(7).	 # conjunctive
	p(NT(245), ~(NT(440)) & ~(NT(441)) & (NT(7)));
//G373: __E_src_c_110(249)   => src_c(245).
	p(NT(249), (NT(245)));
//G374: __E_src_c_110(249)   => space(2).
	p(NT(249), (NT(2)));
//G375: __E_src_c_111(250)   => null.
	p(NT(250), (nul));
//G376: __E_src_c_111(250)   => __E_src_c_110(249) __E_src_c_111(250).
	p(NT(250), (NT(249)+NT(250)));
//G377: src_c(245)           => '{' __E_src_c_111(250) '}'.
	p(NT(245), (T(48)+NT(250)+T(49)));
//G378: __E_variable_112(251) => uconst(252).
	p(NT(251), (NT(252)));
//G379: __E_variable_112(251) => io_var(253).
	p(NT(251), (NT(253)));
//G380: __E_variable_112(251) => var_name(254).
	p(NT(251), (NT(254)));
//G381: __E_variable_113(255) => typed(30).
	p(NT(255), (NT(30)));
//G382: __E_variable_113(255) => null.
	p(NT(255), (nul));
//G383: variable(153)        => __E_variable_112(251) __E_variable_113(255).
	p(NT(153), (NT(251)+NT(255)));
//G384: __N_2(442)           => 'F'.
	p(NT(442), (T(36)));
//G385: __N_3(443)           => 'T'.
	p(NT(443), (T(35)));
//G386: __E_var_name_114(256) => ~( __N_2(442) ) & ~( __N_3(443) ) & alpha(5).	 # conjunctive
	p(NT(256), ~(NT(442)) & ~(NT(443)) & (NT(5)));
//G387: __E_var_name_115(257) => null.
	p(NT(257), (nul));
//G388: __E_var_name_115(257) => digit(3) __E_var_name_115(257).
	p(NT(257), (NT(3)+NT(257)));
//G389: var_name(254)        => __E_var_name_114(256) __E_var_name_115(257).	 # guarded: charvar
	p(NT(254), (NT(256)+NT(257)));
	p.back().guard = "charvar";
//G390: __N_4(444)           => 'F'.
	p(NT(444), (T(36)));
//G391: __N_5(445)           => 'T'.
	p(NT(445), (T(35)));
//G392: __E_var_name_116(258) => ~( __N_4(444) ) & ~( __N_5(445) ) & alpha(5).	 # conjunctive
	p(NT(258), ~(NT(444)) & ~(NT(445)) & (NT(5)));
//G393: __E_var_name_117(259) => alnum(6).
	p(NT(259), (NT(6)));
//G394: __E_var_name_117(259) => '_'.
	p(NT(259), (T(37)));
//G395: __E_var_name_118(260) => null.
	p(NT(260), (nul));
//G396: __E_var_name_118(260) => __E_var_name_117(259) __E_var_name_118(260).
	p(NT(260), (NT(259)+NT(260)));
//G397: var_name(254)        => __E_var_name_116(258) __E_var_name_118(260).	 # guarded: var
	p(NT(254), (NT(258)+NT(260)));
	p.back().guard = "var";
//G398: io_var(253)          => io_var_name(28) '[' offset(261) ']'.
	p(NT(253), (NT(28)+T(26)+NT(261)+T(27)));
//G399: io_var_name(28)      => chars(262).
	p(NT(28), (NT(262)));
//G400: __E___E_uconst_119_120(265) => chars(262) _(11).
	p(NT(265), (NT(262)+NT(11)));
//G401: __E___E_uconst_119_120(265) => null.
	p(NT(265), (nul));
//G402: __E_uconst_119(264)  => __E___E_uconst_119_120(265) ':' _(11) chars(262).
	p(NT(264), (NT(265)+T(2)+NT(11)+NT(262)));
//G403: uconst_name(263)     => __E_uconst_119(264).
	p(NT(263), (NT(264)));
//G404: uconst(252)          => '<' _(11) uconst_name(263) _(11) '>'.
	p(NT(252), (T(22)+NT(11)+NT(263)+NT(11)+T(23)));
//G405: __E_q_vars_121(267)  => _(11) ',' _(11) q_var(266).
	p(NT(267), (NT(11)+T(20)+NT(11)+NT(266)));
//G406: __E_q_vars_122(268)  => null.
	p(NT(268), (nul));
//G407: __E_q_vars_122(268)  => __E_q_vars_121(267) __E_q_vars_122(268).
	p(NT(268), (NT(267)+NT(268)));
//G408: q_vars(86)           => q_var(266) __E_q_vars_122(268).
	p(NT(86), (NT(266)+NT(268)));
//G409: q_var(266)           => capture(26).
	p(NT(266), (NT(26)));
//G410: __N_6(446)           => uconst(252).
	p(NT(446), (NT(252)));
//G411: q_var(266)           => variable(153) & ~( __N_6(446) ).	 # conjunctive
	p(NT(266), (NT(153)) & ~(NT(446)));
//G412: ctnvar(230)          => chars(262).
	p(NT(230), (NT(262)));
//G413: __E_offsets_123(269) => _(11) ',' _(11) offset(261).
	p(NT(269), (NT(11)+T(20)+NT(11)+NT(261)));
//G414: __E_offsets_124(270) => null.
	p(NT(270), (nul));
//G415: __E_offsets_124(270) => __E_offsets_123(269) __E_offsets_124(270).
	p(NT(270), (NT(269)+NT(270)));
//G416: offsets(39)          => '[' _(11) offset(261) __E_offsets_124(270) _(11) ']'.
	p(NT(39), (T(26)+NT(11)+NT(261)+NT(270)+NT(11)+T(27)));
//G417: offset(261)          => integer(271).
	p(NT(261), (NT(271)));
//G418: offset(261)          => capture(26).
	p(NT(261), (NT(26)));
//G419: offset(261)          => shift(272).
	p(NT(261), (NT(272)));
//G420: __N_7(447)           => io_var(253).
	p(NT(447), (NT(253)));
//G421: __E_offset_125(273)  => variable(153) & ~( __N_7(447) ).	 # conjunctive
	p(NT(273), (NT(153)) & ~(NT(447)));
//G422: offset(261)          => __E_offset_125(273).
	p(NT(261), (NT(273)));
//G423: __E_shift_126(274)   => capture(26).
	p(NT(274), (NT(26)));
//G424: __N_8(448)           => io_var(253).
	p(NT(448), (NT(253)));
//G425: __E___E_shift_126_127(275) => variable(153) & ~( __N_8(448) ).	 # conjunctive
	p(NT(275), (NT(153)) & ~(NT(448)));
//G426: __E_shift_126(274)   => __E___E_shift_126_127(275).
	p(NT(274), (NT(275)));
//G427: shift(272)           => __E_shift_126(274) _(11) '-' _(11) num(231).
	p(NT(272), (NT(274)+NT(11)+T(30)+NT(11)+NT(231)));
//G428: __E_chars_128(276)   => alnum(6).
	p(NT(276), (NT(6)));
//G429: __E_chars_128(276)   => '_'.
	p(NT(276), (T(37)));
//G430: __E_chars_129(277)   => null.
	p(NT(277), (nul));
//G431: __E_chars_129(277)   => __E_chars_128(276) __E_chars_129(277).
	p(NT(277), (NT(276)+NT(277)));
//G432: chars(262)           => alpha(5) __E_chars_129(277).
	p(NT(262), (NT(5)+NT(277)));
//G433: __E_digits_130(278)  => digit(3).
	p(NT(278), (NT(3)));
//G434: __E_digits_130(278)  => digit(3) __E_digits_130(278).
	p(NT(278), (NT(3)+NT(278)));
//G435: digits(226)          => __E_digits_130(278).
	p(NT(226), (NT(278)));
//G436: num(231)             => digits(226).
	p(NT(231), (NT(226)));
//G437: __E_integer_131(279) => '-'.
	p(NT(279), (T(30)));
//G438: __E_integer_131(279) => null.
	p(NT(279), (nul));
//G439: integer(271)         => __E_integer_131(279) _(11) digits(226).
	p(NT(271), (NT(279)+NT(11)+NT(226)));
//G440: sym(37)              => chars(262).
	p(NT(37), (NT(262)));
//G441: capture(26)          => '$' chars(262).
	p(NT(26), (T(50)+NT(262)));
//G442: typed(30)            => _(11) ':' _(11) type(280).
	p(NT(30), (NT(11)+T(2)+NT(11)+NT(280)));
//G443: type(280)            => bv_type(218).
	p(NT(280), (NT(218)));
//G444: type(280)            => chars(262).
	p(NT(280), (NT(262)));
//G445: __E_bv_type_132(281) => num(231).
	p(NT(281), (NT(231)));
//G446: __E_bv_type_132(281) => null.
	p(NT(281), (nul));
//G447: bv_type(218)         => 'b' 'v' __E_bv_type_132(281).
	p(NT(218), (T(17)+T(51)+NT(281)));
//G448: scope_id(282)        => digits(226).
	p(NT(282), (NT(226)));
//G449: __E_comment_133(284) => printable(8).
	p(NT(284), (NT(8)));
//G450: __E_comment_133(284) => '\t'.
	p(NT(284), (T(52)));
//G451: __E_comment_134(285) => null.
	p(NT(285), (nul));
//G452: __E_comment_134(285) => __E_comment_133(284) __E_comment_134(285).
	p(NT(285), (NT(284)+NT(285)));
//G453: __E_comment_135(286) => '\n'.
	p(NT(286), (T(53)));
//G454: __E_comment_135(286) => '\r'.
	p(NT(286), (T(54)));
//G455: __E_comment_135(286) => eof(1).
	p(NT(286), (NT(1)));
//G456: comment(283)         => '#' __E_comment_134(285) __E_comment_135(286).
	p(NT(283), (T(47)+NT(285)+NT(286)));
//G457: __(31)               => space(2).
	p(NT(31), (NT(2)));
//G458: __(31)               => comment(283).
	p(NT(31), (NT(283)));
//G459: __(31)               => __(31) space(2).
	p(NT(31), (NT(31)+NT(2)));
//G460: __(31)               => __(31) comment(283).
	p(NT(31), (NT(31)+NT(283)));
//G461: _(11)                => __(31).
	p(NT(11), (NT(31)));
//G462: _(11)                => null.
	p(NT(11), (nul));
//G463: cli(287)             => _(11).
	p(NT(287), (NT(11)));
//G464: __E_cli_136(289)     => '.' _(11) cli_command(288) _(11).
	p(NT(289), (T(1)+NT(11)+NT(288)+NT(11)));
//G465: __E_cli_137(290)     => null.
	p(NT(290), (nul));
//G466: __E_cli_137(290)     => __E_cli_136(289) __E_cli_137(290).
	p(NT(290), (NT(289)+NT(290)));
//G467: cli(287)             => _(11) cli_command(288) _(11) __E_cli_137(290).
	p(NT(287), (NT(11)+NT(288)+NT(11)+NT(290)));
//G468: __E_cli_command_138(293) => 'q'.
	p(NT(293), (T(55)));
//G469: __E_cli_command_138(293) => 'q' 'u' 'i' 't'.
	p(NT(293), (T(55)+T(7)+T(4)+T(8)));
//G470: quit_sym(292)        => __E_cli_command_138(293).
	p(NT(292), (NT(293)));
//G471: quit_cmd(291)        => quit_sym(292).
	p(NT(291), (NT(292)));
//G472: cli_command(288)     => quit_cmd(291).
	p(NT(288), (NT(291)));
//G473: __E_cli_command_139(296) => 'v'.
	p(NT(296), (T(51)));
//G474: __E_cli_command_139(296) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(296), (T(51)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G475: version_sym(295)     => __E_cli_command_139(296).
	p(NT(295), (NT(296)));
//G476: version_cmd(294)     => version_sym(295).
	p(NT(294), (NT(295)));
//G477: cli_command(288)     => version_cmd(294).
	p(NT(288), (NT(294)));
//G478: __E_cli_command_140(299) => 'c'.
	p(NT(299), (T(14)));
//G479: __E_cli_command_140(299) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(299), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G480: clear_sym(298)       => __E_cli_command_140(299).
	p(NT(298), (NT(299)));
//G481: clear_cmd(297)       => clear_sym(298).
	p(NT(297), (NT(298)));
//G482: cli_command(288)     => clear_cmd(297).
	p(NT(288), (NT(297)));
//G483: __E___E_cli_command_141_142(303) => 'h'.
	p(NT(303), (T(56)));
//G484: __E___E_cli_command_141_142(303) => 'h' 'e' 'l' 'p'.
	p(NT(303), (T(56)+T(11)+T(10)+T(57)));
//G485: help_sym(302)        => __E___E_cli_command_141_142(303).
	p(NT(302), (NT(303)));
//G486: __E___E_cli_command_141_143(304) => __(31) help_arg(305).
	p(NT(304), (NT(31)+NT(305)));
//G487: __E___E_cli_command_141_143(304) => null.
	p(NT(304), (nul));
//G488: __E_cli_command_141(301) => help_sym(302) __E___E_cli_command_141_143(304).
	p(NT(301), (NT(302)+NT(304)));
//G489: help_cmd(300)        => __E_cli_command_141(301).
	p(NT(300), (NT(301)));
//G490: cli_command(288)     => help_cmd(300).
	p(NT(288), (NT(300)));
//G491: file_sym(308)        => 'f' 'i' 'l' 'e'.
	p(NT(308), (T(9)+T(4)+T(10)+T(11)));
//G492: __E_cli_command_144(307) => file_sym(308) __(31) q_string(309).
	p(NT(307), (NT(308)+NT(31)+NT(309)));
//G493: file_cmd(306)        => __E_cli_command_144(307).
	p(NT(306), (NT(307)));
//G494: cli_command(288)     => file_cmd(306).
	p(NT(288), (NT(306)));
//G495: valid_sym(312)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(312), (T(51)+T(16)+T(10)+T(4)+T(58)));
//G496: __E_cli_command_145(311) => valid_sym(312) __(31) normalize_cmd_arg(313).
	p(NT(311), (NT(312)+NT(31)+NT(313)));
//G497: valid_cmd(310)       => __E_cli_command_145(311).
	p(NT(310), (NT(311)));
//G498: cli_command(288)     => valid_cmd(310).
	p(NT(288), (NT(310)));
//G499: sat_sym(316)         => 's' 'a' 't'.
	p(NT(316), (T(15)+T(16)+T(8)));
//G500: __E_cli_command_146(315) => sat_sym(316) __(31) normalize_cmd_arg(313).
	p(NT(315), (NT(316)+NT(31)+NT(313)));
//G501: sat_cmd(314)         => __E_cli_command_146(315).
	p(NT(314), (NT(315)));
//G502: cli_command(288)     => sat_cmd(314).
	p(NT(288), (NT(314)));
//G503: unsat_sym(319)       => 'u' 'n' 's' 'a' 't'.
	p(NT(319), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G504: __E_cli_command_147(318) => unsat_sym(319) __(31) normalize_cmd_arg(313).
	p(NT(318), (NT(319)+NT(31)+NT(313)));
//G505: unsat_cmd(317)       => __E_cli_command_147(318).
	p(NT(317), (NT(318)));
//G506: cli_command(288)     => unsat_cmd(317).
	p(NT(288), (NT(317)));
//G507: solve_sym(322)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(322), (T(15)+T(6)+T(10)+T(51)+T(11)));
//G508: __E___E_cli_command_148_149(323) => solve_options(324).
	p(NT(323), (NT(324)));
//G509: __E___E_cli_command_148_149(323) => null.
	p(NT(323), (nul));
//G510: __E_cli_command_148(321) => solve_sym(322) __E___E_cli_command_148_149(323) __(31) wff_cmd_arg(325).
	p(NT(321), (NT(322)+NT(323)+NT(31)+NT(325)));
//G511: solve_cmd(320)       => __E_cli_command_148(321).
	p(NT(320), (NT(321)));
//G512: cli_command(288)     => solve_cmd(320).
	p(NT(288), (NT(320)));
//G513: lgrs_sym(328)        => 'l' 'g' 'r' 's'.
	p(NT(328), (T(10)+T(59)+T(19)+T(15)));
//G514: __E_cli_command_150(327) => lgrs_sym(328) __(31) wff_cmd_arg(325).
	p(NT(327), (NT(328)+NT(31)+NT(325)));
//G515: lgrs_cmd(326)        => __E_cli_command_150(327).
	p(NT(326), (NT(327)));
//G516: cli_command(288)     => lgrs_cmd(326).
	p(NT(288), (NT(326)));
//G517: __E___E_cli_command_151_152(332) => 'r'.
	p(NT(332), (T(19)));
//G518: __E___E_cli_command_151_152(332) => 'r' 'u' 'n'.
	p(NT(332), (T(19)+T(7)+T(5)));
//G519: run_sym(331)         => __E___E_cli_command_151_152(332).
	p(NT(331), (NT(332)));
//G520: __E___E_cli_command_151_153(333) => history(334).
	p(NT(333), (NT(334)));
//G521: __E___E_cli_command_151_153(333) => wff(14).
	p(NT(333), (NT(14)));
//G522: __E_cli_command_151(330) => run_sym(331) __(31) __E___E_cli_command_151_153(333).
	p(NT(330), (NT(331)+NT(31)+NT(333)));
//G523: run_cmd(329)         => __E_cli_command_151(330).
	p(NT(329), (NT(330)));
//G524: cli_command(288)     => run_cmd(329).
	p(NT(288), (NT(329)));
//G525: __E___E_cli_command_154_155(338) => 'n'.
	p(NT(338), (T(5)));
//G526: __E___E_cli_command_154_155(338) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(338), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(60)+T(11)));
//G527: normalize_sym(337)   => __E___E_cli_command_154_155(338).
	p(NT(337), (NT(338)));
//G528: __E_cli_command_154(336) => normalize_sym(337) __(31) normalize_cmd_arg(313).
	p(NT(336), (NT(337)+NT(31)+NT(313)));
//G529: normalize_cmd(335)   => __E_cli_command_154(336).
	p(NT(335), (NT(336)));
//G530: cli_command(288)     => normalize_cmd(335).
	p(NT(288), (NT(335)));
//G531: __E___E_cli_command_156_157(342) => 's'.
	p(NT(342), (T(15)));
//G532: __E___E_cli_command_156_157(342) => 's' 'u' 'b' 's' 't'.
	p(NT(342), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G533: __E___E_cli_command_156_157(342) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(342), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G534: subst_sym(341)       => __E___E_cli_command_156_157(342).
	p(NT(341), (NT(342)));
//G535: __E_cli_command_156(340) => subst_sym(341) __(31) nf_cmd_arg(343) _(11) '[' _(11) nf_cmd_arg(343) _(11) '/' _(11) nf_cmd_arg(343) _(11) ']'.
	p(NT(340), (NT(341)+NT(31)+NT(343)+NT(11)+T(26)+NT(11)+NT(343)+NT(11)+T(44)+NT(11)+NT(343)+NT(11)+T(27)));
//G536: subst_cmd(339)       => __E_cli_command_156(340).
	p(NT(339), (NT(340)));
//G537: cli_command(288)     => subst_cmd(339).
	p(NT(288), (NT(339)));
//G538: __E___E_cli_command_158_159(347) => 'i'.
	p(NT(347), (T(4)));
//G539: __E___E_cli_command_158_159(347) => 'i' 'n' 's' 't'.
	p(NT(347), (T(4)+T(5)+T(15)+T(8)));
//G540: __E___E_cli_command_158_159(347) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(347), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G541: inst_sym(346)        => __E___E_cli_command_158_159(347).
	p(NT(346), (NT(347)));
//G542: __E_cli_command_158(345) => inst_sym(346) __(31) inst_args(348).
	p(NT(345), (NT(346)+NT(31)+NT(348)));
//G543: inst_cmd(344)        => __E_cli_command_158(345).
	p(NT(344), (NT(345)));
//G544: cli_command(288)     => inst_cmd(344).
	p(NT(288), (NT(344)));
//G545: dnf_sym(351)         => 'd' 'n' 'f'.
	p(NT(351), (T(58)+T(5)+T(9)));
//G546: __E_cli_command_160(350) => dnf_sym(351) __(31) nf_cmd_arg(343).
	p(NT(350), (NT(351)+NT(31)+NT(343)));
//G547: dnf_cmd(349)         => __E_cli_command_160(350).
	p(NT(349), (NT(350)));
//G548: cli_command(288)     => dnf_cmd(349).
	p(NT(288), (NT(349)));
//G549: cnf_sym(354)         => 'c' 'n' 'f'.
	p(NT(354), (T(14)+T(5)+T(9)));
//G550: __E_cli_command_161(353) => cnf_sym(354) __(31) nf_cmd_arg(343).
	p(NT(353), (NT(354)+NT(31)+NT(343)));
//G551: cnf_cmd(352)         => __E_cli_command_161(353).
	p(NT(352), (NT(353)));
//G552: cli_command(288)     => cnf_cmd(352).
	p(NT(288), (NT(352)));
//G553: anf_sym(357)         => 'a' 'n' 'f'.
	p(NT(357), (T(16)+T(5)+T(9)));
//G554: __E_cli_command_162(356) => anf_sym(357) __(31) nf_cmd_arg(343).
	p(NT(356), (NT(357)+NT(31)+NT(343)));
//G555: anf_cmd(355)         => __E_cli_command_162(356).
	p(NT(355), (NT(356)));
//G556: cli_command(288)     => anf_cmd(355).
	p(NT(288), (NT(355)));
//G557: nnf_sym(360)         => 'n' 'n' 'f'.
	p(NT(360), (T(5)+T(5)+T(9)));
//G558: __E_cli_command_163(359) => nnf_sym(360) __(31) nf_cmd_arg(343).
	p(NT(359), (NT(360)+NT(31)+NT(343)));
//G559: nnf_cmd(358)         => __E_cli_command_163(359).
	p(NT(358), (NT(359)));
//G560: cli_command(288)     => nnf_cmd(358).
	p(NT(288), (NT(358)));
//G561: pnf_sym(363)         => 'p' 'n' 'f'.
	p(NT(363), (T(57)+T(5)+T(9)));
//G562: __E_cli_command_164(362) => pnf_sym(363) __(31) nf_cmd_arg(343).
	p(NT(362), (NT(363)+NT(31)+NT(343)));
//G563: pnf_cmd(361)         => __E_cli_command_164(362).
	p(NT(361), (NT(362)));
//G564: cli_command(288)     => pnf_cmd(361).
	p(NT(288), (NT(361)));
//G565: mnf_sym(366)         => 'm' 'n' 'f'.
	p(NT(366), (T(21)+T(5)+T(9)));
//G566: __E_cli_command_165(365) => mnf_sym(366) __(31) nf_cmd_arg(343).
	p(NT(365), (NT(366)+NT(31)+NT(343)));
//G567: mnf_cmd(364)         => __E_cli_command_165(365).
	p(NT(364), (NT(365)));
//G568: cli_command(288)     => mnf_cmd(364).
	p(NT(288), (NT(364)));
//G569: snf_sym(369)         => 's' 'n' 'f'.
	p(NT(369), (T(15)+T(5)+T(9)));
//G570: __E_cli_command_166(368) => snf_sym(369) __(31) nf_cmd_arg(343).
	p(NT(368), (NT(369)+NT(31)+NT(343)));
//G571: snf_cmd(367)         => __E_cli_command_166(368).
	p(NT(367), (NT(368)));
//G572: cli_command(288)     => snf_cmd(367).
	p(NT(288), (NT(367)));
//G573: onf_sym(372)         => 'o' 'n' 'f'.
	p(NT(372), (T(6)+T(5)+T(9)));
//G574: __E_cli_command_167(371) => onf_sym(372) __(31) variable(153) __(31) onf_cmd_arg(373).
	p(NT(371), (NT(372)+NT(31)+NT(153)+NT(31)+NT(373)));
//G575: onf_cmd(370)         => __E_cli_command_167(371).
	p(NT(370), (NT(371)));
//G576: cli_command(288)     => onf_cmd(370).
	p(NT(288), (NT(370)));
//G577: qelim_sym(376)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(376), (T(55)+T(11)+T(10)+T(4)+T(21)));
//G578: __E_cli_command_168(375) => qelim_sym(376) __(31) wff_cmd_arg(325).
	p(NT(375), (NT(376)+NT(31)+NT(325)));
//G579: qelim_cmd(374)       => __E_cli_command_168(375).
	p(NT(374), (NT(375)));
//G580: cli_command(288)     => qelim_cmd(374).
	p(NT(288), (NT(374)));
//G581: get_sym(379)         => 'g' 'e' 't'.
	p(NT(379), (T(59)+T(11)+T(8)));
//G582: __E___E_cli_command_169_170(380) => __(31) option_name(381).
	p(NT(380), (NT(31)+NT(381)));
//G583: __E___E_cli_command_169_170(380) => null.
	p(NT(380), (nul));
//G584: __E_cli_command_169(378) => get_sym(379) __E___E_cli_command_169_170(380).
	p(NT(378), (NT(379)+NT(380)));
//G585: get_cmd(377)         => __E_cli_command_169(378).
	p(NT(377), (NT(378)));
//G586: cli_command(288)     => get_cmd(377).
	p(NT(288), (NT(377)));
//G587: set_sym(384)         => 's' 'e' 't'.
	p(NT(384), (T(15)+T(11)+T(8)));
//G588: __E___E_cli_command_171_172(385) => __(31).
	p(NT(385), (NT(31)));
//G589: __E___E_cli_command_171_172(385) => _(11) '=' _(11).
	p(NT(385), (NT(11)+T(3)+NT(11)));
//G590: __E_cli_command_171(383) => set_sym(384) __(31) option_name(381) __E___E_cli_command_171_172(385) option_value(386).
	p(NT(383), (NT(384)+NT(31)+NT(381)+NT(385)+NT(386)));
//G591: set_cmd(382)         => __E_cli_command_171(383).
	p(NT(382), (NT(383)));
//G592: cli_command(288)     => set_cmd(382).
	p(NT(288), (NT(382)));
//G593: enable_sym(389)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(389), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G594: __E_cli_command_173(388) => enable_sym(389) __(31) option_name(381).
	p(NT(388), (NT(389)+NT(31)+NT(381)));
//G595: enable_cmd(387)      => __E_cli_command_173(388).
	p(NT(387), (NT(388)));
//G596: cli_command(288)     => enable_cmd(387).
	p(NT(288), (NT(387)));
//G597: disable_sym(392)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(392), (T(58)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G598: __E_cli_command_174(391) => disable_sym(392) __(31) option_name(381).
	p(NT(391), (NT(392)+NT(31)+NT(381)));
//G599: disable_cmd(390)     => __E_cli_command_174(391).
	p(NT(390), (NT(391)));
//G600: cli_command(288)     => disable_cmd(390).
	p(NT(288), (NT(390)));
//G601: toggle_sym(395)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(395), (T(8)+T(6)+T(59)+T(59)+T(10)+T(11)));
//G602: __E_cli_command_175(394) => toggle_sym(395) __(31) option_name(381).
	p(NT(394), (NT(395)+NT(31)+NT(381)));
//G603: toggle_cmd(393)      => __E_cli_command_175(394).
	p(NT(393), (NT(394)));
//G604: cli_command(288)     => toggle_cmd(393).
	p(NT(288), (NT(393)));
//G605: __E___E_cli_command_176_177(399) => 'd' 'e' 'f' 's'.
	p(NT(399), (T(58)+T(11)+T(9)+T(15)));
//G606: __E___E_cli_command_176_177(399) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(399), (T(58)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G607: def_sym(398)         => __E___E_cli_command_176_177(399).
	p(NT(398), (NT(399)));
//G608: __E_cli_command_176(397) => def_sym(398).
	p(NT(397), (NT(398)));
//G609: def_list_cmd(396)    => __E_cli_command_176(397).
	p(NT(396), (NT(397)));
//G610: cli_command(288)     => def_list_cmd(396).
	p(NT(288), (NT(396)));
//G611: __E_cli_command_178(401) => def_sym(398) __(31) num(231).
	p(NT(401), (NT(398)+NT(31)+NT(231)));
//G612: def_print_cmd(400)   => __E_cli_command_178(401).
	p(NT(400), (NT(401)));
//G613: cli_command(288)     => def_print_cmd(400).
	p(NT(288), (NT(400)));
//G614: def_rr_cmd(402)      => rec_relation(17).
	p(NT(402), (NT(17)));
//G615: cli_command(288)     => def_rr_cmd(402).
	p(NT(288), (NT(402)));
//G616: def_input_cmd(403)   => input_def(18).
	p(NT(403), (NT(18)));
//G617: cli_command(288)     => def_input_cmd(403).
	p(NT(288), (NT(403)));
//G618: def_output_cmd(404)  => output_def(19).
	p(NT(404), (NT(19)));
//G619: cli_command(288)     => def_output_cmd(404).
	p(NT(288), (NT(404)));
//G620: __E___E_cli_command_179_180(408) => 'h' 'i' 's' 't'.
	p(NT(408), (T(56)+T(4)+T(15)+T(8)));
//G621: __E___E_cli_command_179_180(408) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(408), (T(56)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G622: history_sym(407)     => __E___E_cli_command_179_180(408).
	p(NT(407), (NT(408)));
//G623: __E_cli_command_179(406) => history_sym(407).
	p(NT(406), (NT(407)));
//G624: history_list_cmd(405) => __E_cli_command_179(406).
	p(NT(405), (NT(406)));
//G625: cli_command(288)     => history_list_cmd(405).
	p(NT(288), (NT(405)));
//G626: __E_cli_command_181(410) => history_sym(407) __(31) history(334).
	p(NT(410), (NT(407)+NT(31)+NT(334)));
//G627: history_print_cmd(409) => __E_cli_command_181(410).
	p(NT(409), (NT(410)));
//G628: cli_command(288)     => history_print_cmd(409).
	p(NT(288), (NT(409)));
//G629: __E_cli_command_182(412) => wff(14).
	p(NT(412), (NT(14)));
//G630: __E_cli_command_182(412) => bf(27).
	p(NT(412), (NT(27)));
//G631: history_store_cmd(411) => __E_cli_command_182(412).
	p(NT(411), (NT(412)));
//G632: cli_command(288)     => history_store_cmd(411).
	p(NT(288), (NT(411)));
//G633: __E_solve_options_183(413) => __(31) solve_option(414).
	p(NT(413), (NT(31)+NT(414)));
//G634: __E_solve_options_184(415) => null.
	p(NT(415), (nul));
//G635: __E_solve_options_184(415) => __E_solve_options_183(413) __E_solve_options_184(415).
	p(NT(415), (NT(413)+NT(415)));
//G636: solve_options(324)   => __E_solve_options_184(415).
	p(NT(324), (NT(415)));
//G637: __E_solve_option_185(416) => solver_mode(417).
	p(NT(416), (NT(417)));
//G638: __E_solve_option_185(416) => type(280).
	p(NT(416), (NT(280)));
//G639: solve_option(414)    => '-' '-' __E_solve_option_185(416).
	p(NT(414), (T(30)+T(30)+NT(416)));
//G640: __E_solver_mode_186(419) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(419), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G641: __E_solver_mode_186(419) => 'm' 'i' 'n'.
	p(NT(419), (T(21)+T(4)+T(5)));
//G642: solver_mode_minimum(418) => __E_solver_mode_186(419).
	p(NT(418), (NT(419)));
//G643: solver_mode(417)     => solver_mode_minimum(418).
	p(NT(417), (NT(418)));
//G644: __E_solver_mode_187(421) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(421), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G645: __E_solver_mode_187(421) => 'm' 'a' 'x'.
	p(NT(421), (T(21)+T(16)+T(29)));
//G646: solver_mode_maximum(420) => __E_solver_mode_187(421).
	p(NT(420), (NT(421)));
//G647: solver_mode(417)     => solver_mode_maximum(420).
	p(NT(417), (NT(420)));
//G648: q_file_name(35)      => '"' file_name(422) '"'.
	p(NT(35), (T(61)+NT(422)+T(61)));
//G649: __E_file_name_188(423) => printable(8).
	p(NT(423), (NT(8)));
//G650: __E_file_name_188(423) => printable(8) __E_file_name_188(423).
	p(NT(423), (NT(8)+NT(423)));
//G651: file_name(422)       => __E_file_name_188(423).
	p(NT(422), (NT(423)));
//G652: __E_option_name_189(424) => alnum(6).
	p(NT(424), (NT(6)));
//G653: __E_option_name_189(424) => alnum(6) __E_option_name_189(424).
	p(NT(424), (NT(6)+NT(424)));
//G654: option_name(381)     => __E_option_name_189(424).
	p(NT(381), (NT(424)));
//G655: __E_option_value_190(425) => alnum(6).
	p(NT(425), (NT(6)));
//G656: __E_option_value_190(425) => alnum(6) __E_option_value_190(425).
	p(NT(425), (NT(6)+NT(425)));
//G657: option_value(386)    => __E_option_value_190(425).
	p(NT(386), (NT(425)));
//G658: bf_cmd_arg(426)      => history(334).
	p(NT(426), (NT(334)));
//G659: bf_cmd_arg(426)      => bf(27).
	p(NT(426), (NT(27)));
//G660: wff_cmd_arg(325)     => history(334).
	p(NT(325), (NT(334)));
//G661: wff_cmd_arg(325)     => wff(14).
	p(NT(325), (NT(14)));
//G662: nf_cmd_arg(343)      => history(334).
	p(NT(343), (NT(334)));
//G663: nf_cmd_arg(343)      => ref(24).
	p(NT(343), (NT(24)));
//G664: nf_cmd_arg(343)      => wff(14).
	p(NT(343), (NT(14)));
//G665: nf_cmd_arg(343)      => bf(27).
	p(NT(343), (NT(27)));
//G666: onf_cmd_arg(373)     => history(334).
	p(NT(373), (NT(334)));
//G667: onf_cmd_arg(373)     => wff(14).
	p(NT(373), (NT(14)));
//G668: normalize_cmd_arg(313) => history(334).
	p(NT(313), (NT(334)));
//G669: normalize_cmd_arg(313) => spec(10).
	p(NT(313), (NT(10)));
//G670: normalize_cmd_arg(313) => ref(24).
	p(NT(313), (NT(24)));
//G671: normalize_cmd_arg(313) => wff(14).
	p(NT(313), (NT(14)));
//G672: normalize_cmd_arg(313) => bf(27).
	p(NT(313), (NT(27)));
//G673: inst_args(348)       => wff_cmd_arg(325) _(11) '[' _(11) variable(153) _(11) '/' _(11) bf_cmd_arg(426) _(11) ']'.
	p(NT(348), (NT(325)+NT(11)+T(26)+NT(11)+NT(153)+NT(11)+T(44)+NT(11)+NT(426)+NT(11)+T(27)));
//G674: inst_args(348)       => bf_cmd_arg(426) _(11) '[' _(11) variable(153) _(11) '/' _(11) bf_cmd_arg(426) _(11) ']'.
	p(NT(348), (NT(426)+NT(11)+T(26)+NT(11)+NT(153)+NT(11)+T(44)+NT(11)+NT(426)+NT(11)+T(27)));
//G675: help_arg(305)        => help_sym(302).
	p(NT(305), (NT(302)));
//G676: help_arg(305)        => version_sym(295).
	p(NT(305), (NT(295)));
//G677: help_arg(305)        => quit_sym(292).
	p(NT(305), (NT(292)));
//G678: help_arg(305)        => clear_sym(298).
	p(NT(305), (NT(298)));
//G679: help_arg(305)        => get_sym(379).
	p(NT(305), (NT(379)));
//G680: help_arg(305)        => set_sym(384).
	p(NT(305), (NT(384)));
//G681: help_arg(305)        => enable_sym(389).
	p(NT(305), (NT(389)));
//G682: help_arg(305)        => disable_sym(392).
	p(NT(305), (NT(392)));
//G683: help_arg(305)        => toggle_sym(395).
	p(NT(305), (NT(395)));
//G684: help_arg(305)        => file_sym(308).
	p(NT(305), (NT(308)));
//G685: help_arg(305)        => history_sym(407).
	p(NT(305), (NT(407)));
//G686: help_arg(305)        => abs_history_sym(427).
	p(NT(305), (NT(427)));
//G687: help_arg(305)        => rel_history_sym(428).
	p(NT(305), (NT(428)));
//G688: help_arg(305)        => selection_sym(429).
	p(NT(305), (NT(429)));
//G689: help_arg(305)        => def_sym(398).
	p(NT(305), (NT(398)));
//G690: help_arg(305)        => inst_sym(346).
	p(NT(305), (NT(346)));
//G691: help_arg(305)        => subst_sym(341).
	p(NT(305), (NT(341)));
//G692: help_arg(305)        => normalize_sym(337).
	p(NT(305), (NT(337)));
//G693: help_arg(305)        => execute_sym(430).
	p(NT(305), (NT(430)));
//G694: help_arg(305)        => solve_sym(322).
	p(NT(305), (NT(322)));
//G695: help_arg(305)        => valid_sym(312).
	p(NT(305), (NT(312)));
//G696: help_arg(305)        => sat_sym(316).
	p(NT(305), (NT(316)));
//G697: help_arg(305)        => unsat_sym(319).
	p(NT(305), (NT(319)));
//G698: help_arg(305)        => run_sym(331).
	p(NT(305), (NT(331)));
//G699: help_arg(305)        => dnf_sym(351).
	p(NT(305), (NT(351)));
//G700: help_arg(305)        => cnf_sym(354).
	p(NT(305), (NT(354)));
//G701: help_arg(305)        => anf_sym(357).
	p(NT(305), (NT(357)));
//G702: help_arg(305)        => snf_sym(369).
	p(NT(305), (NT(369)));
//G703: help_arg(305)        => nnf_sym(360).
	p(NT(305), (NT(360)));
//G704: help_arg(305)        => pnf_sym(363).
	p(NT(305), (NT(363)));
//G705: help_arg(305)        => mnf_sym(366).
	p(NT(305), (NT(366)));
//G706: help_arg(305)        => onf_sym(372).
	p(NT(305), (NT(372)));
//G707: help_arg(305)        => qelim_sym(376).
	p(NT(305), (NT(376)));
//G708: __E___E_help_arg_191_192(433) => 's'.
	p(NT(433), (T(15)));
//G709: __E___E_help_arg_191_192(433) => null.
	p(NT(433), (nul));
//G710: __E_help_arg_191(432) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_191_192(433).
	p(NT(432), (T(11)+T(29)+T(16)+T(21)+T(57)+T(10)+T(11)+NT(433)));
//G711: examples_sym(431)    => __E_help_arg_191(432).
	p(NT(431), (NT(432)));
//G712: help_arg(305)        => examples_sym(431).
	p(NT(305), (NT(431)));
//G713: __E_history_193(435) => '%'.
	p(NT(435), (T(45)));
//G714: rel_history_sym(428) => '%' '-'.
	p(NT(428), (T(45)+T(30)));
//G715: history_id(437)      => digits(226).
	p(NT(437), (NT(226)));
//G716: __E___E_history_193_194(436) => history_id(437).
	p(NT(436), (NT(437)));
//G717: __E___E_history_193_194(436) => null.
	p(NT(436), (nul));
//G718: __E_history_193(435) => rel_history_sym(428) __E___E_history_193_194(436).
	p(NT(435), (NT(428)+NT(436)));
//G719: rel_history(434)     => __E_history_193(435).
	p(NT(434), (NT(435)));
//G720: history(334)         => rel_history(434).
	p(NT(334), (NT(434)));
//G721: abs_history_sym(427) => '%'.
	p(NT(427), (T(45)));
//G722: __E_history_195(439) => abs_history_sym(427) history_id(437).
	p(NT(439), (NT(427)+NT(437)));
//G723: abs_history(438)     => __E_history_195(439).
	p(NT(438), (NT(439)));
//G724: history(334)         => abs_history(438).
	p(NT(334), (NT(438)));
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
		__E_bv_89, bv_min, __E_bv_90, bv_max, __E_bv_91, bv_constant, binary, __E_bv_constant_92, bv_type, decimal, 
		__E_bv_constant_93, hexadecimal, __E_bv_constant_94, __E_binary_95, __E_binary_96, __E_hexadecimal_97, digits, __E_decimal_98, ctn_neq, __E_constraint_99, 
		ctnvar, num, ctn_eq, __E_constraint_100, ctn_gteq, __E_constraint_101, ctn_gt, __E_constraint_102, ctn_lteq, __E_constraint_103, 
		ctn_lt, __E_constraint_104, __E_bf_constant_105, source, __E_bf_constant_106, src_c, __E_source_107, __E___E_source_107_108, __E___E_source_107_109, __E_src_c_110, 
		__E_src_c_111, __E_variable_112, uconst, io_var, var_name, __E_variable_113, __E_var_name_114, __E_var_name_115, __E_var_name_116, __E_var_name_117, 
		__E_var_name_118, offset, chars, uconst_name, __E_uconst_119, __E___E_uconst_119_120, q_var, __E_q_vars_121, __E_q_vars_122, __E_offsets_123, 
		__E_offsets_124, integer, shift, __E_offset_125, __E_shift_126, __E___E_shift_126_127, __E_chars_128, __E_chars_129, __E_digits_130, __E_integer_131, 
		type, __E_bv_type_132, scope_id, comment, __E_comment_133, __E_comment_134, __E_comment_135, cli, cli_command, __E_cli_136, 
		__E_cli_137, quit_cmd, quit_sym, __E_cli_command_138, version_cmd, version_sym, __E_cli_command_139, clear_cmd, clear_sym, __E_cli_command_140, 
		help_cmd, __E_cli_command_141, help_sym, __E___E_cli_command_141_142, __E___E_cli_command_141_143, help_arg, file_cmd, __E_cli_command_144, file_sym, q_string, 
		valid_cmd, __E_cli_command_145, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_146, sat_sym, unsat_cmd, __E_cli_command_147, unsat_sym, 
		solve_cmd, __E_cli_command_148, solve_sym, __E___E_cli_command_148_149, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_150, lgrs_sym, run_cmd, 
		__E_cli_command_151, run_sym, __E___E_cli_command_151_152, __E___E_cli_command_151_153, history, normalize_cmd, __E_cli_command_154, normalize_sym, __E___E_cli_command_154_155, subst_cmd, 
		__E_cli_command_156, subst_sym, __E___E_cli_command_156_157, nf_cmd_arg, inst_cmd, __E_cli_command_158, inst_sym, __E___E_cli_command_158_159, inst_args, dnf_cmd, 
		__E_cli_command_160, dnf_sym, cnf_cmd, __E_cli_command_161, cnf_sym, anf_cmd, __E_cli_command_162, anf_sym, nnf_cmd, __E_cli_command_163, 
		nnf_sym, pnf_cmd, __E_cli_command_164, pnf_sym, mnf_cmd, __E_cli_command_165, mnf_sym, snf_cmd, __E_cli_command_166, snf_sym, 
		onf_cmd, __E_cli_command_167, onf_sym, onf_cmd_arg, qelim_cmd, __E_cli_command_168, qelim_sym, get_cmd, __E_cli_command_169, get_sym, 
		__E___E_cli_command_169_170, option_name, set_cmd, __E_cli_command_171, set_sym, __E___E_cli_command_171_172, option_value, enable_cmd, __E_cli_command_173, enable_sym, 
		disable_cmd, __E_cli_command_174, disable_sym, toggle_cmd, __E_cli_command_175, toggle_sym, def_list_cmd, __E_cli_command_176, def_sym, __E___E_cli_command_176_177, 
		def_print_cmd, __E_cli_command_178, def_rr_cmd, def_input_cmd, def_output_cmd, history_list_cmd, __E_cli_command_179, history_sym, __E___E_cli_command_179_180, history_print_cmd, 
		__E_cli_command_181, history_store_cmd, __E_cli_command_182, __E_solve_options_183, solve_option, __E_solve_options_184, __E_solve_option_185, solver_mode, solver_mode_minimum, __E_solver_mode_186, 
		solver_mode_maximum, __E_solver_mode_187, file_name, __E_file_name_188, __E_option_name_189, __E_option_value_190, bf_cmd_arg, abs_history_sym, rel_history_sym, selection_sym, 
		execute_sym, examples_sym, __E_help_arg_191, __E___E_help_arg_191_192, rel_history, __E_history_193, __E___E_history_193_194, history_id, abs_history, __E_history_195, 
		__N_0, __N_1, __N_2, __N_3, __N_4, __N_5, __N_6, __N_7, __N_8, 
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
