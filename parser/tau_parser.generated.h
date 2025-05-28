// This file is generated from a file parser/tau.tgf by
//       https://github.com/IDNI/parser/tools/tgf
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
	"spec", "_", "definitions", "main", "wff", "__E_definitions_0", "__E___E_definitions_0_1", "ref_relation", "input_def", "output_def", 
	"__E_definitions_2", "ref_relations", "__E_ref_relations_3", "rec_relation", "__E_ref_relations_4", "ref", "__E_rec_relation_5", "capture", "bf", "io_var_name", 
	"__E_input_def_6", "typed", "__", "stream", "__E_output_def_7", "__E_output_def_8", "q_file_name", "console_sym", "sym", "__E_ref_9", 
	"offsets", "ref_args", "__E_ref_10", "fp_fallback", "first_sym", "last_sym", "__E_ref_args_11", "__E___E_ref_args_11_12", "ref_arg", "__E___E_ref_args_11_13", 
	"__E___E_ref_args_11_14", "library", "rules", "__E_library_15", "__E___E_library_15_16", "rule", "__E___E___E_library_15_16_17", "wff_rule", "bf_rule", "__E___E_library_15_18", 
	"wff_matcher", "wff_body", "bf_matcher", "bf_body", "builder", "builder_head", "builder_body", "__E_builder_head_19", "__E_builder_head_20", "bf_builder_body", 
	"__E_builder_body_21", "wff_builder_body", "__E_builder_body_22", "tau_constant_source", "rec_relations", "__E_tau_constant_source_23", "wff_parenthesis", "__E_wff_24", "wff_sometimes", "__E_wff_25", 
	"__E___E_wff_25_26", "wff_always", "__E_wff_27", "__E___E_wff_27_28", "wff_conditional", "__E_wff_29", "wff_all", "__E_wff_30", "q_vars", "wff_ex", 
	"__E_wff_31", "wff_ref", "constraint", "wff_imply", "__E_wff_32", "wff_rimply", "__E_wff_33", "wff_equiv", "__E_wff_34", "wff_or", 
	"__E_wff_35", "wff_xor", "__E_wff_36", "wff_and", "__E_wff_37", "wff_neg", "__E_wff_38", "wff_t", "wff_f", "bf_interval", 
	"__E_wff_39", "bf_eq", "__E_wff_40", "bf_neq", "__E_wff_41", "bf_lteq", "__E_wff_42", "bf_nlteq", "__E_wff_43", "bf_gt", 
	"__E_wff_44", "bf_ngt", "__E_wff_45", "bf_gteq", "__E_wff_46", "bf_ngteq", "__E_wff_47", "bf_lt", "__E_wff_48", "bf_nlt", 
	"__E_wff_49", "bf_parenthesis", "__E_bf_50", "bf_constant", "variable", "bf_splitter", "__E_bf_51", "bf_ref", "bf_or", "__E_bf_52", 
	"bf_xor", "__E_bf_53", "bf_t", "__E_bf_54", "__E___E_bf_54_55", "bf_f", "__E_bf_56", "__E___E_bf_56_57", "bf_and", "__E_bf_58", 
	"__E___E_bf_58_59", "bf_neg", "__E_bf_60", "bf_neg_oprnd", "__E___E_bf_60_61", "bf_and_nosep", "__E_bf_62", "bf_and_nosep_1st_oprnd", "__E___E_bf_62_63", "bf_and_nosep_2nd_oprnd", 
	"__E___E_bf_62_64", "ctn_neq", "__E_constraint_65", "ctnvar", "num", "ctn_eq", "__E_constraint_66", "ctn_gteq", "__E_constraint_67", "ctn_gt", 
	"__E_constraint_68", "ctn_lteq", "__E_constraint_69", "ctn_lt", "__E_constraint_70", "__E_bf_constant_71", "source", "__E_bf_constant_72", "src_c", "__E_source_73", 
	"__E___E_source_73_74", "__E___E_source_73_75", "__E_src_c_76", "__E_src_c_77", "__E_variable_78", "uconst", "io_var", "var_name", "__E_variable_79", "__E_var_name_80", 
	"__E_var_name_81", "__E_var_name_82", "__E_var_name_83", "__E_var_name_84", "offset", "chars", "uconst_name", "__E_uconst_85", "__E___E_uconst_85_86", "q_var", 
	"__E_q_vars_87", "__E_q_vars_88", "__E_offsets_89", "__E_offsets_90", "integer", "shift", "__E_offset_91", "__E_shift_92", "__E___E_shift_92_93", "__E_chars_94", 
	"__E_chars_95", "digits", "__E_digits_96", "__E_integer_97", "type", "scope_id", "comment", "__E_comment_98", "__E_comment_99", "__E_comment_100", 
	"__E____101", "__E___102", "cli", "cli_command", "__E_cli_103", "__E_cli_104", "quit_cmd", "quit_sym", "__E_cli_command_105", "version_cmd", 
	"version_sym", "__E_cli_command_106", "clear_cmd", "clear_sym", "__E_cli_command_107", "help_cmd", "__E_cli_command_108", "help_sym", "__E___E_cli_command_108_109", "__E___E_cli_command_108_110", 
	"help_arg", "file_cmd", "__E_cli_command_111", "file_sym", "q_string", "valid_cmd", "__E_cli_command_112", "valid_sym", "normalize_cmd_arg", "sat_cmd", 
	"__E_cli_command_113", "sat_sym", "unsat_cmd", "__E_cli_command_114", "unsat_sym", "solve_cmd", "__E_cli_command_115", "solve_sym", "__E___E_cli_command_115_116", "solve_options", 
	"wff_cmd_arg", "lgrs_cmd", "__E_cli_command_117", "lgrs_sym", "run_cmd", "__E_cli_command_118", "run_sym", "__E___E_cli_command_118_119", "__E___E_cli_command_118_120", "history", 
	"normalize_cmd", "__E_cli_command_121", "normalize_sym", "__E___E_cli_command_121_122", "subst_cmd", "__E_cli_command_123", "subst_sym", "__E___E_cli_command_123_124", "nf_cmd_arg", "inst_cmd", 
	"__E_cli_command_125", "inst_sym", "__E___E_cli_command_125_126", "inst_args", "dnf_cmd", "__E_cli_command_127", "dnf_sym", "cnf_cmd", "__E_cli_command_128", "cnf_sym", 
	"anf_cmd", "__E_cli_command_129", "anf_sym", "nnf_cmd", "__E_cli_command_130", "nnf_sym", "pnf_cmd", "__E_cli_command_131", "pnf_sym", "mnf_cmd", 
	"__E_cli_command_132", "mnf_sym", "snf_cmd", "__E_cli_command_133", "snf_sym", "onf_cmd", "__E_cli_command_134", "onf_sym", "onf_cmd_arg", "qelim_cmd", 
	"__E_cli_command_135", "qelim_sym", "get_cmd", "__E_cli_command_136", "get_sym", "__E___E_cli_command_136_137", "option_name", "set_cmd", "__E_cli_command_138", "set_sym", 
	"__E___E_cli_command_138_139", "option_value", "enable_cmd", "__E_cli_command_140", "enable_sym", "disable_cmd", "__E_cli_command_141", "disable_sym", "toggle_cmd", "__E_cli_command_142", 
	"toggle_sym", "def_list_cmd", "__E_cli_command_143", "def_sym", "__E___E_cli_command_143_144", "def_print_cmd", "__E_cli_command_145", "def_rr_cmd", "def_input_cmd", "def_output_cmd", 
	"history_list_cmd", "__E_cli_command_146", "history_sym", "__E___E_cli_command_146_147", "history_print_cmd", "__E_cli_command_148", "history_store_cmd", "__E_cli_command_149", "__E_solve_options_150", "solve_option", 
	"__E_solve_options_151", "__E_solve_option_152", "solver_mode", "solver_mode_minimum", "__E_solver_mode_153", "solver_mode_maximum", "__E_solver_mode_154", "file_name", "__E_file_name_155", "__E_option_name_156", 
	"__E_option_value_157", "bf_cmd_arg", "abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", "examples_sym", "__E_help_arg_158", "__E___E_help_arg_158_159", "rel_history", 
	"__E_history_160", "__E___E_history_160_161", "history_id", "abs_history", "__E_history_162", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", 
	"__N_5", "__N_6", "__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', 'a', 'b', 'k', 
	'r', ',', 'm', '<', '>', 'w', 'y', '[', ']', '?', 
	'x', '-', '|', '^', '&', '!', 'T', 'F', 'S', '+', 
	'1', '0', '\'', '{', '}', '_', '$', '\t', '\n', '\r', 
	'#', 'q', 'v', 'h', 'p', 'd', 'g', 'z', '/', '"', 
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
			11, 32
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			27, 29, 38, 163, 176, 187, 196, 204, 211, 214,
			316, 321, 357
		},
		.to_inline = {
			{ 14, 76, 14 },
			{ 28, 131, 28 },
			{ 31 },
			{ 153, 131, 28 },
			{ 155, 131, 148 },
			{ 157, 131, 28 },
			{ 159, 131, 28 },
			{ 178 },
			{ 185 },
			{ 185, 196 },
			{ 195 },
			{ 199 },
			{ 248 },
			{ 260 },
			{ 278 },
			{ 283 },
			{ 308 },
			{ 361 }
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
//G3:   __E___E_definitions_0_1(16) => ref_relation(17).
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
//G10:  __E_ref_relations_3(22) => _(11) rec_relation(23) _(11) '.'.
	p(NT(22), (NT(11)+NT(23)+NT(11)+T(1)));
//G11:  __E_ref_relations_4(24) => null.
	p(NT(24), (nul));
//G12:  __E_ref_relations_4(24) => __E_ref_relations_3(22) __E_ref_relations_4(24).
	p(NT(24), (NT(22)+NT(24)));
//G13:  ref_relations(21)    => __E_ref_relations_4(24).
	p(NT(21), (NT(24)));
//G14:  __E_rec_relation_5(26) => capture(27).
	p(NT(26), (NT(27)));
//G15:  __E_rec_relation_5(26) => ref(25).
	p(NT(26), (NT(25)));
//G16:  __E_rec_relation_5(26) => wff(14).
	p(NT(26), (NT(14)));
//G17:  __E_rec_relation_5(26) => bf(28).
	p(NT(26), (NT(28)));
//G18:  rec_relation(23)     => ref(25) _(11) ':' '=' _(11) __E_rec_relation_5(26).
	p(NT(23), (NT(25)+NT(11)+T(2)+T(3)+NT(11)+NT(26)));
//G19:  __E_input_def_6(30)  => typed(31).
	p(NT(30), (NT(31)));
//G20:  __E_input_def_6(30)  => null.
	p(NT(30), (nul));
//G21:  input_def(18)        => io_var_name(29) __E_input_def_6(30) _(11) '=' _(11) 'i' 'n' __(32) stream(33).
	p(NT(18), (NT(29)+NT(30)+NT(11)+T(3)+NT(11)+T(4)+T(5)+NT(32)+NT(33)));
//G22:  __E_output_def_7(34) => typed(31).
	p(NT(34), (NT(31)));
//G23:  __E_output_def_7(34) => null.
	p(NT(34), (nul));
//G24:  __E_output_def_8(35) => __(32) stream(33).
	p(NT(35), (NT(32)+NT(33)));
//G25:  __E_output_def_8(35) => null.
	p(NT(35), (nul));
//G26:  output_def(19)       => io_var_name(29) __E_output_def_7(34) _(11) '=' _(11) 'o' 'u' 't' __E_output_def_8(35).
	p(NT(19), (NT(29)+NT(34)+NT(11)+T(3)+NT(11)+T(6)+T(7)+T(8)+NT(35)));
//G27:  stream(33)           => 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(36) _(11) ')'.
	p(NT(33), (T(9)+T(4)+T(10)+T(11)+NT(11)+T(12)+NT(11)+NT(36)+NT(11)+T(13)));
//G28:  console_sym(37)      => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(37), (T(14)+T(6)+T(5)+T(15)+T(6)+T(10)+T(11)));
//G29:  stream(33)           => console_sym(37).
	p(NT(33), (NT(37)));
//G30:  __E_ref_9(39)        => offsets(40).
	p(NT(39), (NT(40)));
//G31:  __E_ref_9(39)        => null.
	p(NT(39), (nul));
//G32:  __E_ref_10(42)       => _(11) 'f' 'a' 'l' 'l' 'b' 'a' 'c' 'k' __(32) fp_fallback(43).
	p(NT(42), (NT(11)+T(9)+T(16)+T(10)+T(10)+T(17)+T(16)+T(14)+T(18)+NT(32)+NT(43)));
//G33:  __E_ref_10(42)       => null.
	p(NT(42), (nul));
//G34:  ref(25)              => sym(38) __E_ref_9(39) ref_args(41) __E_ref_10(42).
	p(NT(25), (NT(38)+NT(39)+NT(41)+NT(42)));
//G35:  first_sym(44)        => 'f' 'i' 'r' 's' 't'.
	p(NT(44), (T(9)+T(4)+T(19)+T(15)+T(8)));
//G36:  fp_fallback(43)      => first_sym(44).
	p(NT(43), (NT(44)));
//G37:  last_sym(45)         => 'l' 'a' 's' 't'.
	p(NT(45), (T(10)+T(16)+T(15)+T(8)));
//G38:  fp_fallback(43)      => last_sym(45).
	p(NT(43), (NT(45)));
//G39:  fp_fallback(43)      => capture(27).
	p(NT(43), (NT(27)));
//G40:  fp_fallback(43)      => ref(25).
	p(NT(43), (NT(25)));
//G41:  fp_fallback(43)      => wff(14).
	p(NT(43), (NT(14)));
//G42:  fp_fallback(43)      => bf(28).
	p(NT(43), (NT(28)));
//G43:  ref_arg(48)          => bf(28).
	p(NT(48), (NT(28)));
//G44:  __E___E_ref_args_11_12(47) => _(11) ref_arg(48).
	p(NT(47), (NT(11)+NT(48)));
//G45:  __E___E_ref_args_11_13(49) => _(11) ',' _(11) ref_arg(48).
	p(NT(49), (NT(11)+T(20)+NT(11)+NT(48)));
//G46:  __E___E_ref_args_11_14(50) => null.
	p(NT(50), (nul));
//G47:  __E___E_ref_args_11_14(50) => __E___E_ref_args_11_13(49) __E___E_ref_args_11_14(50).
	p(NT(50), (NT(49)+NT(50)));
//G48:  __E_ref_args_11(46)  => __E___E_ref_args_11_12(47) __E___E_ref_args_11_14(50).
	p(NT(46), (NT(47)+NT(50)));
//G49:  __E_ref_args_11(46)  => null.
	p(NT(46), (nul));
//G50:  ref_args(41)         => '(' __E_ref_args_11(46) _(11) ')'.
	p(NT(41), (T(12)+NT(46)+NT(11)+T(13)));
//G51:  __E___E___E_library_15_16_17(56) => wff_rule(57).
	p(NT(56), (NT(57)));
//G52:  __E___E___E_library_15_16_17(56) => bf_rule(58).
	p(NT(56), (NT(58)));
//G53:  rule(55)             => __E___E___E_library_15_16_17(56).
	p(NT(55), (NT(56)));
//G54:  __E___E_library_15_16(54) => _(11) rule(55).
	p(NT(54), (NT(11)+NT(55)));
//G55:  __E___E_library_15_18(59) => null.
	p(NT(59), (nul));
//G56:  __E___E_library_15_18(59) => __E___E_library_15_16(54) __E___E_library_15_18(59).
	p(NT(59), (NT(54)+NT(59)));
//G57:  __E_library_15(53)   => __E___E_library_15_18(59).
	p(NT(53), (NT(59)));
//G58:  rules(52)            => __E_library_15(53).
	p(NT(52), (NT(53)));
//G59:  library(51)          => rules(52).
	p(NT(51), (NT(52)));
//G60:  wff_matcher(60)      => wff(14).
	p(NT(60), (NT(14)));
//G61:  wff_body(61)         => wff(14).
	p(NT(61), (NT(14)));
//G62:  wff_rule(57)         => wff_matcher(60) _(11) ':' ':' '=' _(11) wff_body(61) _(11) '.'.
	p(NT(57), (NT(60)+NT(11)+T(2)+T(2)+T(3)+NT(11)+NT(61)+NT(11)+T(1)));
//G63:  bf_matcher(62)       => bf(28).
	p(NT(62), (NT(28)));
//G64:  bf_body(63)          => bf(28).
	p(NT(63), (NT(28)));
//G65:  bf_rule(58)          => bf_matcher(62) _(11) ':' '=' _(11) bf_body(63) _(11) '.'.
	p(NT(58), (NT(62)+NT(11)+T(2)+T(3)+NT(11)+NT(63)+NT(11)+T(1)));
//G66:  builder(64)          => _(11) builder_head(65) _(11) builder_body(66) _(11) '.'.
	p(NT(64), (NT(11)+NT(65)+NT(11)+NT(66)+NT(11)+T(1)));
//G67:  __E_builder_head_19(67) => __(32) capture(27).
	p(NT(67), (NT(32)+NT(27)));
//G68:  __E_builder_head_20(68) => null.
	p(NT(68), (nul));
//G69:  __E_builder_head_20(68) => __E_builder_head_19(67) __E_builder_head_20(68).
	p(NT(68), (NT(67)+NT(68)));
//G70:  builder_head(65)     => '(' _(11) capture(27) __E_builder_head_20(68) _(11) ')'.
	p(NT(65), (T(12)+NT(11)+NT(27)+NT(68)+NT(11)+T(13)));
//G71:  __E_builder_body_21(70) => '=' ':' _(11) bf(28).
	p(NT(70), (T(3)+T(2)+NT(11)+NT(28)));
//G72:  bf_builder_body(69)  => __E_builder_body_21(70).
	p(NT(69), (NT(70)));
//G73:  builder_body(66)     => bf_builder_body(69).
	p(NT(66), (NT(69)));
//G74:  __E_builder_body_22(72) => '=' ':' ':' _(11) wff(14).
	p(NT(72), (T(3)+T(2)+T(2)+NT(11)+NT(14)));
//G75:  wff_builder_body(71) => __E_builder_body_22(72).
	p(NT(71), (NT(72)));
//G76:  builder_body(66)     => wff_builder_body(71).
	p(NT(66), (NT(71)));
//G77:  __E_tau_constant_source_23(75) => '.' _(11).
	p(NT(75), (T(1)+NT(11)));
//G78:  __E_tau_constant_source_23(75) => null.
	p(NT(75), (nul));
//G79:  tau_constant_source(73) => rec_relations(74) _(11) main(13) _(11) __E_tau_constant_source_23(75).
	p(NT(73), (NT(74)+NT(11)+NT(13)+NT(11)+NT(75)));
//G80:  __E_wff_24(77)       => '(' _(11) wff(14) _(11) ')'.
	p(NT(77), (T(12)+NT(11)+NT(14)+NT(11)+T(13)));
//G81:  wff_parenthesis(76)  => __E_wff_24(77).
	p(NT(76), (NT(77)));
//G82:  wff(14)              => wff_parenthesis(76).
	p(NT(14), (NT(76)));
//G83:  __E___E_wff_25_26(80) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(80), (T(15)+T(6)+T(21)+T(11)+T(8)+T(4)+T(21)+T(11)+T(15)));
//G84:  __E___E_wff_25_26(80) => '<' '>'.
	p(NT(80), (T(22)+T(23)));
//G85:  __E_wff_25(79)       => __E___E_wff_25_26(80) _(11) wff(14).
	p(NT(79), (NT(80)+NT(11)+NT(14)));
//G86:  wff_sometimes(78)    => __E_wff_25(79).
	p(NT(78), (NT(79)));
//G87:  wff(14)              => wff_sometimes(78).
	p(NT(14), (NT(78)));
//G88:  __E___E_wff_27_28(83) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(83), (T(16)+T(10)+T(24)+T(16)+T(25)+T(15)));
//G89:  __E___E_wff_27_28(83) => '[' ']'.
	p(NT(83), (T(26)+T(27)));
//G90:  __E_wff_27(82)       => __E___E_wff_27_28(83) _(11) wff(14).
	p(NT(82), (NT(83)+NT(11)+NT(14)));
//G91:  wff_always(81)       => __E_wff_27(82).
	p(NT(81), (NT(82)));
//G92:  wff(14)              => wff_always(81).
	p(NT(14), (NT(81)));
//G93:  __E_wff_29(85)       => wff(14) _(11) '?' _(11) wff(14) _(11) ':' _(11) wff(14).
	p(NT(85), (NT(14)+NT(11)+T(28)+NT(11)+NT(14)+NT(11)+T(2)+NT(11)+NT(14)));
//G94:  wff_conditional(84)  => __E_wff_29(85).
	p(NT(84), (NT(85)));
//G95:  wff(14)              => wff_conditional(84).
	p(NT(14), (NT(84)));
//G96:  __E_wff_30(87)       => 'a' 'l' 'l' __(32) q_vars(88) __(32) wff(14).
	p(NT(87), (T(16)+T(10)+T(10)+NT(32)+NT(88)+NT(32)+NT(14)));
//G97:  wff_all(86)          => __E_wff_30(87).
	p(NT(86), (NT(87)));
//G98:  wff(14)              => wff_all(86).
	p(NT(14), (NT(86)));
//G99:  __E_wff_31(90)       => 'e' 'x' __(32) q_vars(88) __(32) wff(14).
	p(NT(90), (T(11)+T(29)+NT(32)+NT(88)+NT(32)+NT(14)));
//G100: wff_ex(89)           => __E_wff_31(90).
	p(NT(89), (NT(90)));
//G101: wff(14)              => wff_ex(89).
	p(NT(14), (NT(89)));
//G102: wff_ref(91)          => ref(25).
	p(NT(91), (NT(25)));
//G103: wff(14)              => wff_ref(91).
	p(NT(14), (NT(91)));
//G104: wff(14)              => constraint(92).
	p(NT(14), (NT(92)));
//G105: __E_wff_32(94)       => wff(14) _(11) '-' '>' _(11) wff(14).
	p(NT(94), (NT(14)+NT(11)+T(30)+T(23)+NT(11)+NT(14)));
//G106: wff_imply(93)        => __E_wff_32(94).
	p(NT(93), (NT(94)));
//G107: wff(14)              => wff_imply(93).
	p(NT(14), (NT(93)));
//G108: __E_wff_33(96)       => wff(14) _(11) '<' '-' _(11) wff(14).
	p(NT(96), (NT(14)+NT(11)+T(22)+T(30)+NT(11)+NT(14)));
//G109: wff_rimply(95)       => __E_wff_33(96).
	p(NT(95), (NT(96)));
//G110: wff(14)              => wff_rimply(95).
	p(NT(14), (NT(95)));
//G111: __E_wff_34(98)       => wff(14) _(11) '<' '-' '>' _(11) wff(14).
	p(NT(98), (NT(14)+NT(11)+T(22)+T(30)+T(23)+NT(11)+NT(14)));
//G112: wff_equiv(97)        => __E_wff_34(98).
	p(NT(97), (NT(98)));
//G113: wff(14)              => wff_equiv(97).
	p(NT(14), (NT(97)));
//G114: __E_wff_35(100)      => wff(14) _(11) '|' '|' _(11) wff(14).
	p(NT(100), (NT(14)+NT(11)+T(31)+T(31)+NT(11)+NT(14)));
//G115: wff_or(99)           => __E_wff_35(100).
	p(NT(99), (NT(100)));
//G116: wff(14)              => wff_or(99).
	p(NT(14), (NT(99)));
//G117: __E_wff_36(102)      => wff(14) _(11) '^' _(11) wff(14).
	p(NT(102), (NT(14)+NT(11)+T(32)+NT(11)+NT(14)));
//G118: wff_xor(101)         => __E_wff_36(102).
	p(NT(101), (NT(102)));
//G119: wff(14)              => wff_xor(101).
	p(NT(14), (NT(101)));
//G120: __E_wff_37(104)      => wff(14) _(11) '&' '&' _(11) wff(14).
	p(NT(104), (NT(14)+NT(11)+T(33)+T(33)+NT(11)+NT(14)));
//G121: wff_and(103)         => __E_wff_37(104).
	p(NT(103), (NT(104)));
//G122: wff(14)              => wff_and(103).
	p(NT(14), (NT(103)));
//G123: __E_wff_38(106)      => '!' _(11) wff(14).
	p(NT(106), (T(34)+NT(11)+NT(14)));
//G124: wff_neg(105)         => __E_wff_38(106).
	p(NT(105), (NT(106)));
//G125: wff(14)              => wff_neg(105).
	p(NT(14), (NT(105)));
//G126: wff_t(107)           => 'T'.
	p(NT(107), (T(35)));
//G127: wff(14)              => wff_t(107).
	p(NT(14), (NT(107)));
//G128: wff_f(108)           => 'F'.
	p(NT(108), (T(36)));
//G129: wff(14)              => wff_f(108).
	p(NT(14), (NT(108)));
//G130: __E_wff_39(110)      => bf(28) _(11) '<' '=' _(11) bf(28) _(11) '<' '=' _(11) bf(28).
	p(NT(110), (NT(28)+NT(11)+T(22)+T(3)+NT(11)+NT(28)+NT(11)+T(22)+T(3)+NT(11)+NT(28)));
//G131: bf_interval(109)     => __E_wff_39(110).
	p(NT(109), (NT(110)));
//G132: wff(14)              => bf_interval(109).
	p(NT(14), (NT(109)));
//G133: __E_wff_40(112)      => bf(28) _(11) '=' _(11) bf(28).
	p(NT(112), (NT(28)+NT(11)+T(3)+NT(11)+NT(28)));
//G134: bf_eq(111)           => __E_wff_40(112).
	p(NT(111), (NT(112)));
//G135: wff(14)              => bf_eq(111).
	p(NT(14), (NT(111)));
//G136: __E_wff_41(114)      => bf(28) _(11) '!' '=' _(11) bf(28).
	p(NT(114), (NT(28)+NT(11)+T(34)+T(3)+NT(11)+NT(28)));
//G137: bf_neq(113)          => __E_wff_41(114).
	p(NT(113), (NT(114)));
//G138: wff(14)              => bf_neq(113).
	p(NT(14), (NT(113)));
//G139: __E_wff_42(116)      => bf(28) _(11) '<' '=' _(11) bf(28).
	p(NT(116), (NT(28)+NT(11)+T(22)+T(3)+NT(11)+NT(28)));
//G140: bf_lteq(115)         => __E_wff_42(116).
	p(NT(115), (NT(116)));
//G141: wff(14)              => bf_lteq(115).
	p(NT(14), (NT(115)));
//G142: __E_wff_43(118)      => bf(28) _(11) '!' '<' '=' _(11) bf(28).
	p(NT(118), (NT(28)+NT(11)+T(34)+T(22)+T(3)+NT(11)+NT(28)));
//G143: bf_nlteq(117)        => __E_wff_43(118).
	p(NT(117), (NT(118)));
//G144: wff(14)              => bf_nlteq(117).
	p(NT(14), (NT(117)));
//G145: __E_wff_44(120)      => bf(28) _(11) '>' _(11) bf(28).
	p(NT(120), (NT(28)+NT(11)+T(23)+NT(11)+NT(28)));
//G146: bf_gt(119)           => __E_wff_44(120).
	p(NT(119), (NT(120)));
//G147: wff(14)              => bf_gt(119).
	p(NT(14), (NT(119)));
//G148: __E_wff_45(122)      => bf(28) _(11) '!' '>' _(11) bf(28).
	p(NT(122), (NT(28)+NT(11)+T(34)+T(23)+NT(11)+NT(28)));
//G149: bf_ngt(121)          => __E_wff_45(122).
	p(NT(121), (NT(122)));
//G150: wff(14)              => bf_ngt(121).
	p(NT(14), (NT(121)));
//G151: __E_wff_46(124)      => bf(28) _(11) '>' '=' _(11) bf(28).
	p(NT(124), (NT(28)+NT(11)+T(23)+T(3)+NT(11)+NT(28)));
//G152: bf_gteq(123)         => __E_wff_46(124).
	p(NT(123), (NT(124)));
//G153: wff(14)              => bf_gteq(123).
	p(NT(14), (NT(123)));
//G154: __E_wff_47(126)      => bf(28) _(11) '!' '>' '=' _(11) bf(28).
	p(NT(126), (NT(28)+NT(11)+T(34)+T(23)+T(3)+NT(11)+NT(28)));
//G155: bf_ngteq(125)        => __E_wff_47(126).
	p(NT(125), (NT(126)));
//G156: wff(14)              => bf_ngteq(125).
	p(NT(14), (NT(125)));
//G157: __E_wff_48(128)      => bf(28) _(11) '<' _(11) bf(28).
	p(NT(128), (NT(28)+NT(11)+T(22)+NT(11)+NT(28)));
//G158: bf_lt(127)           => __E_wff_48(128).
	p(NT(127), (NT(128)));
//G159: wff(14)              => bf_lt(127).
	p(NT(14), (NT(127)));
//G160: __E_wff_49(130)      => bf(28) _(11) '!' '<' _(11) bf(28).
	p(NT(130), (NT(28)+NT(11)+T(34)+T(22)+NT(11)+NT(28)));
//G161: bf_nlt(129)          => __E_wff_49(130).
	p(NT(129), (NT(130)));
//G162: wff(14)              => bf_nlt(129).
	p(NT(14), (NT(129)));
//G163: wff(14)              => capture(27).
	p(NT(14), (NT(27)));
//G164: __E_bf_50(132)       => '(' _(11) bf(28) _(11) ')'.
	p(NT(132), (T(12)+NT(11)+NT(28)+NT(11)+T(13)));
//G165: bf_parenthesis(131)  => __E_bf_50(132).
	p(NT(131), (NT(132)));
//G166: bf(28)               => bf_parenthesis(131).
	p(NT(28), (NT(131)));
//G167: bf(28)               => bf_constant(133).
	p(NT(28), (NT(133)));
//G168: bf(28)               => variable(134).
	p(NT(28), (NT(134)));
//G169: __E_bf_51(136)       => 'S' _(11) '(' _(11) bf(28) _(11) ')'.
	p(NT(136), (T(37)+NT(11)+T(12)+NT(11)+NT(28)+NT(11)+T(13)));
//G170: bf_splitter(135)     => __E_bf_51(136).
	p(NT(135), (NT(136)));
//G171: bf(28)               => bf_splitter(135).
	p(NT(28), (NT(135)));
//G172: bf_ref(137)          => ref(25).
	p(NT(137), (NT(25)));
//G173: bf(28)               => bf_ref(137).
	p(NT(28), (NT(137)));
//G174: __E_bf_52(139)       => bf(28) _(11) '|' _(11) bf(28).
	p(NT(139), (NT(28)+NT(11)+T(31)+NT(11)+NT(28)));
//G175: bf_or(138)           => __E_bf_52(139).
	p(NT(138), (NT(139)));
//G176: bf(28)               => bf_or(138).
	p(NT(28), (NT(138)));
//G177: __E_bf_53(141)       => bf(28) _(11) '+' _(11) bf(28).
	p(NT(141), (NT(28)+NT(11)+T(38)+NT(11)+NT(28)));
//G178: bf_xor(140)          => __E_bf_53(141).
	p(NT(140), (NT(141)));
//G179: bf(28)               => bf_xor(140).
	p(NT(28), (NT(140)));
//G180: __E___E_bf_54_55(144) => typed(31).
	p(NT(144), (NT(31)));
//G181: __E___E_bf_54_55(144) => null.
	p(NT(144), (nul));
//G182: __E_bf_54(143)       => '1' __E___E_bf_54_55(144).
	p(NT(143), (T(39)+NT(144)));
//G183: bf_t(142)            => __E_bf_54(143).
	p(NT(142), (NT(143)));
//G184: bf(28)               => bf_t(142).
	p(NT(28), (NT(142)));
//G185: __E___E_bf_56_57(147) => typed(31).
	p(NT(147), (NT(31)));
//G186: __E___E_bf_56_57(147) => null.
	p(NT(147), (nul));
//G187: __E_bf_56(146)       => '0' __E___E_bf_56_57(147).
	p(NT(146), (T(40)+NT(147)));
//G188: bf_f(145)            => __E_bf_56(146).
	p(NT(145), (NT(146)));
//G189: bf(28)               => bf_f(145).
	p(NT(28), (NT(145)));
//G190: __E___E_bf_58_59(150) => _(11) '&' _(11).
	p(NT(150), (NT(11)+T(33)+NT(11)));
//G191: __E___E_bf_58_59(150) => __(32).
	p(NT(150), (NT(32)));
//G192: __E_bf_58(149)       => bf(28) __E___E_bf_58_59(150) bf(28).
	p(NT(149), (NT(28)+NT(150)+NT(28)));
//G193: bf_and(148)          => __E_bf_58(149).
	p(NT(148), (NT(149)));
//G194: bf(28)               => bf_and(148).
	p(NT(28), (NT(148)));
//G195: __E___E_bf_60_61(154) => bf_parenthesis(131).
	p(NT(154), (NT(131)));
//G196: __E___E_bf_60_61(154) => bf_constant(133).
	p(NT(154), (NT(133)));
//G197: __E___E_bf_60_61(154) => variable(134).
	p(NT(154), (NT(134)));
//G198: __E___E_bf_60_61(154) => bf_splitter(135).
	p(NT(154), (NT(135)));
//G199: __E___E_bf_60_61(154) => bf_ref(137).
	p(NT(154), (NT(137)));
//G200: __E___E_bf_60_61(154) => bf_t(142).
	p(NT(154), (NT(142)));
//G201: __E___E_bf_60_61(154) => bf_f(145).
	p(NT(154), (NT(145)));
//G202: __E___E_bf_60_61(154) => bf_neg(151).
	p(NT(154), (NT(151)));
//G203: __E___E_bf_60_61(154) => capture(27).
	p(NT(154), (NT(27)));
//G204: bf_neg_oprnd(153)    => __E___E_bf_60_61(154).
	p(NT(153), (NT(154)));
//G205: __E_bf_60(152)       => bf_neg_oprnd(153) _(11) '\''.
	p(NT(152), (NT(153)+NT(11)+T(41)));
//G206: bf_neg(151)          => __E_bf_60(152).
	p(NT(151), (NT(152)));
//G207: bf(28)               => bf_neg(151).
	p(NT(28), (NT(151)));
//G208: __E___E_bf_62_63(158) => bf_parenthesis(131).
	p(NT(158), (NT(131)));
//G209: __E___E_bf_62_63(158) => variable(134).
	p(NT(158), (NT(134)));
//G210: __E___E_bf_62_63(158) => bf_splitter(135).
	p(NT(158), (NT(135)));
//G211: __E___E_bf_62_63(158) => bf_ref(137).
	p(NT(158), (NT(137)));
//G212: __E___E_bf_62_63(158) => bf_neg(151).
	p(NT(158), (NT(151)));
//G213: __E___E_bf_62_63(158) => bf_and_nosep(155).
	p(NT(158), (NT(155)));
//G214: __E___E_bf_62_63(158) => capture(27).
	p(NT(158), (NT(27)));
//G215: bf_and_nosep_1st_oprnd(157) => __E___E_bf_62_63(158).
	p(NT(157), (NT(158)));
//G216: __E___E_bf_62_64(160) => bf_parenthesis(131).
	p(NT(160), (NT(131)));
//G217: __E___E_bf_62_64(160) => bf_constant(133).
	p(NT(160), (NT(133)));
//G218: __E___E_bf_62_64(160) => variable(134).
	p(NT(160), (NT(134)));
//G219: __E___E_bf_62_64(160) => bf_splitter(135).
	p(NT(160), (NT(135)));
//G220: __E___E_bf_62_64(160) => bf_ref(137).
	p(NT(160), (NT(137)));
//G221: __E___E_bf_62_64(160) => bf_neg(151).
	p(NT(160), (NT(151)));
//G222: __E___E_bf_62_64(160) => capture(27).
	p(NT(160), (NT(27)));
//G223: bf_and_nosep_2nd_oprnd(159) => __E___E_bf_62_64(160).
	p(NT(159), (NT(160)));
//G224: __E_bf_62(156)       => bf_and_nosep_1st_oprnd(157) bf_and_nosep_2nd_oprnd(159).
	p(NT(156), (NT(157)+NT(159)));
//G225: bf_and_nosep(155)    => __E_bf_62(156).
	p(NT(155), (NT(156)));
//G226: bf(28)               => bf_and_nosep(155).
	p(NT(28), (NT(155)));
//G227: bf(28)               => capture(27).
	p(NT(28), (NT(27)));
//G228: __E_constraint_65(162) => '[' ctnvar(163) _(11) '!' '=' _(11) num(164) ']'.
	p(NT(162), (T(26)+NT(163)+NT(11)+T(34)+T(3)+NT(11)+NT(164)+T(27)));
//G229: __E_constraint_65(162) => '[' num(164) _(11) '!' '=' _(11) ctnvar(163) ']'.
	p(NT(162), (T(26)+NT(164)+NT(11)+T(34)+T(3)+NT(11)+NT(163)+T(27)));
//G230: ctn_neq(161)         => __E_constraint_65(162).
	p(NT(161), (NT(162)));
//G231: constraint(92)       => ctn_neq(161).
	p(NT(92), (NT(161)));
//G232: __E_constraint_66(166) => '[' ctnvar(163) _(11) '=' _(11) num(164) ']'.
	p(NT(166), (T(26)+NT(163)+NT(11)+T(3)+NT(11)+NT(164)+T(27)));
//G233: __E_constraint_66(166) => '[' num(164) _(11) '=' _(11) ctnvar(163) ']'.
	p(NT(166), (T(26)+NT(164)+NT(11)+T(3)+NT(11)+NT(163)+T(27)));
//G234: ctn_eq(165)          => __E_constraint_66(166).
	p(NT(165), (NT(166)));
//G235: constraint(92)       => ctn_eq(165).
	p(NT(92), (NT(165)));
//G236: __E_constraint_67(168) => '[' ctnvar(163) _(11) '>' '=' _(11) num(164) ']'.
	p(NT(168), (T(26)+NT(163)+NT(11)+T(23)+T(3)+NT(11)+NT(164)+T(27)));
//G237: __E_constraint_67(168) => '[' num(164) _(11) '>' '=' _(11) ctnvar(163) ']'.
	p(NT(168), (T(26)+NT(164)+NT(11)+T(23)+T(3)+NT(11)+NT(163)+T(27)));
//G238: ctn_gteq(167)        => __E_constraint_67(168).
	p(NT(167), (NT(168)));
//G239: constraint(92)       => ctn_gteq(167).
	p(NT(92), (NT(167)));
//G240: __E_constraint_68(170) => '[' ctnvar(163) _(11) '>' _(11) num(164) ']'.
	p(NT(170), (T(26)+NT(163)+NT(11)+T(23)+NT(11)+NT(164)+T(27)));
//G241: __E_constraint_68(170) => '[' num(164) _(11) '>' _(11) ctnvar(163) ']'.
	p(NT(170), (T(26)+NT(164)+NT(11)+T(23)+NT(11)+NT(163)+T(27)));
//G242: ctn_gt(169)          => __E_constraint_68(170).
	p(NT(169), (NT(170)));
//G243: constraint(92)       => ctn_gt(169).
	p(NT(92), (NT(169)));
//G244: __E_constraint_69(172) => '[' ctnvar(163) _(11) '<' '=' _(11) num(164) ']'.
	p(NT(172), (T(26)+NT(163)+NT(11)+T(22)+T(3)+NT(11)+NT(164)+T(27)));
//G245: __E_constraint_69(172) => '[' num(164) _(11) '<' '=' _(11) ctnvar(163) ']'.
	p(NT(172), (T(26)+NT(164)+NT(11)+T(22)+T(3)+NT(11)+NT(163)+T(27)));
//G246: ctn_lteq(171)        => __E_constraint_69(172).
	p(NT(171), (NT(172)));
//G247: constraint(92)       => ctn_lteq(171).
	p(NT(92), (NT(171)));
//G248: __E_constraint_70(174) => '[' ctnvar(163) _(11) '<' _(11) num(164) ']'.
	p(NT(174), (T(26)+NT(163)+NT(11)+T(22)+NT(11)+NT(164)+T(27)));
//G249: __E_constraint_70(174) => '[' num(164) _(11) '<' _(11) ctnvar(163) ']'.
	p(NT(174), (T(26)+NT(164)+NT(11)+T(22)+NT(11)+NT(163)+T(27)));
//G250: ctn_lt(173)          => __E_constraint_70(174).
	p(NT(173), (NT(174)));
//G251: constraint(92)       => ctn_lt(173).
	p(NT(92), (NT(173)));
//G252: __E_bf_constant_71(175) => capture(27).
	p(NT(175), (NT(27)));
//G253: __E_bf_constant_71(175) => source(176).
	p(NT(175), (NT(176)));
//G254: __E_bf_constant_72(177) => typed(31).
	p(NT(177), (NT(31)));
//G255: __E_bf_constant_72(177) => null.
	p(NT(177), (nul));
//G256: bf_constant(133)     => '{' _(11) __E_bf_constant_71(175) _(11) '}' __E_bf_constant_72(177).
	p(NT(133), (T(42)+NT(11)+NT(175)+NT(11)+T(43)+NT(177)));
//G257: __E___E_source_73_74(180) => src_c(178).
	p(NT(180), (NT(178)));
//G258: __E___E_source_73_74(180) => space(2).
	p(NT(180), (NT(2)));
//G259: __E___E_source_73_75(181) => null.
	p(NT(181), (nul));
//G260: __E___E_source_73_75(181) => __E___E_source_73_74(180) __E___E_source_73_75(181).
	p(NT(181), (NT(180)+NT(181)));
//G261: __E_source_73(179)   => __E___E_source_73_75(181) src_c(178).
	p(NT(179), (NT(181)+NT(178)));
//G262: __E_source_73(179)   => null.
	p(NT(179), (nul));
//G263: source(176)          => src_c(178) __E_source_73(179).
	p(NT(176), (NT(178)+NT(179)));
//G264: src_c(178)           => alnum(6).
	p(NT(178), (NT(6)));
//G265: __N_0(375)           => '{'.
	p(NT(375), (T(42)));
//G266: __N_1(376)           => '}'.
	p(NT(376), (T(43)));
//G267: src_c(178)           => ~( __N_0(375) ) & ~( __N_1(376) ) & punct(7).	 # conjunctive
	p(NT(178), ~(NT(375)) & ~(NT(376)) & (NT(7)));
//G268: __E_src_c_76(182)    => src_c(178).
	p(NT(182), (NT(178)));
//G269: __E_src_c_76(182)    => space(2).
	p(NT(182), (NT(2)));
//G270: __E_src_c_77(183)    => null.
	p(NT(183), (nul));
//G271: __E_src_c_77(183)    => __E_src_c_76(182) __E_src_c_77(183).
	p(NT(183), (NT(182)+NT(183)));
//G272: src_c(178)           => '{' __E_src_c_77(183) '}'.
	p(NT(178), (T(42)+NT(183)+T(43)));
//G273: __E_variable_78(184) => uconst(185).
	p(NT(184), (NT(185)));
//G274: __E_variable_78(184) => io_var(186).
	p(NT(184), (NT(186)));
//G275: __E_variable_78(184) => var_name(187).
	p(NT(184), (NT(187)));
//G276: __E_variable_79(188) => typed(31).
	p(NT(188), (NT(31)));
//G277: __E_variable_79(188) => null.
	p(NT(188), (nul));
//G278: variable(134)        => __E_variable_78(184) __E_variable_79(188).
	p(NT(134), (NT(184)+NT(188)));
//G279: __N_2(377)           => 'F'.
	p(NT(377), (T(36)));
//G280: __N_3(378)           => 'T'.
	p(NT(378), (T(35)));
//G281: __E_var_name_80(189) => ~( __N_2(377) ) & ~( __N_3(378) ) & alpha(5).	 # conjunctive
	p(NT(189), ~(NT(377)) & ~(NT(378)) & (NT(5)));
//G282: __E_var_name_81(190) => null.
	p(NT(190), (nul));
//G283: __E_var_name_81(190) => digit(3) __E_var_name_81(190).
	p(NT(190), (NT(3)+NT(190)));
//G284: var_name(187)        => __E_var_name_80(189) __E_var_name_81(190).	 # guarded: charvar
	p(NT(187), (NT(189)+NT(190)));
	p.back().guard = "charvar";
//G285: __N_4(379)           => 'F'.
	p(NT(379), (T(36)));
//G286: __N_5(380)           => 'T'.
	p(NT(380), (T(35)));
//G287: __E_var_name_82(191) => ~( __N_4(379) ) & ~( __N_5(380) ) & alpha(5).	 # conjunctive
	p(NT(191), ~(NT(379)) & ~(NT(380)) & (NT(5)));
//G288: __E_var_name_83(192) => alnum(6).
	p(NT(192), (NT(6)));
//G289: __E_var_name_83(192) => '_'.
	p(NT(192), (T(44)));
//G290: __E_var_name_84(193) => null.
	p(NT(193), (nul));
//G291: __E_var_name_84(193) => __E_var_name_83(192) __E_var_name_84(193).
	p(NT(193), (NT(192)+NT(193)));
//G292: var_name(187)        => __E_var_name_82(191) __E_var_name_84(193).	 # guarded: var
	p(NT(187), (NT(191)+NT(193)));
	p.back().guard = "var";
//G293: io_var(186)          => io_var_name(29) '[' offset(194) ']'.
	p(NT(186), (NT(29)+T(26)+NT(194)+T(27)));
//G294: io_var_name(29)      => chars(195).
	p(NT(29), (NT(195)));
//G295: __E___E_uconst_85_86(198) => chars(195) _(11).
	p(NT(198), (NT(195)+NT(11)));
//G296: __E___E_uconst_85_86(198) => null.
	p(NT(198), (nul));
//G297: __E_uconst_85(197)   => __E___E_uconst_85_86(198) ':' _(11) chars(195).
	p(NT(197), (NT(198)+T(2)+NT(11)+NT(195)));
//G298: uconst_name(196)     => __E_uconst_85(197).
	p(NT(196), (NT(197)));
//G299: uconst(185)          => '<' _(11) uconst_name(196) _(11) '>'.
	p(NT(185), (T(22)+NT(11)+NT(196)+NT(11)+T(23)));
//G300: __E_q_vars_87(200)   => _(11) ',' _(11) q_var(199).
	p(NT(200), (NT(11)+T(20)+NT(11)+NT(199)));
//G301: __E_q_vars_88(201)   => null.
	p(NT(201), (nul));
//G302: __E_q_vars_88(201)   => __E_q_vars_87(200) __E_q_vars_88(201).
	p(NT(201), (NT(200)+NT(201)));
//G303: q_vars(88)           => q_var(199) __E_q_vars_88(201).
	p(NT(88), (NT(199)+NT(201)));
//G304: q_var(199)           => capture(27).
	p(NT(199), (NT(27)));
//G305: __N_6(381)           => uconst(185).
	p(NT(381), (NT(185)));
//G306: q_var(199)           => variable(134) & ~( __N_6(381) ).	 # conjunctive
	p(NT(199), (NT(134)) & ~(NT(381)));
//G307: ctnvar(163)          => chars(195).
	p(NT(163), (NT(195)));
//G308: __E_offsets_89(202)  => _(11) ',' _(11) offset(194).
	p(NT(202), (NT(11)+T(20)+NT(11)+NT(194)));
//G309: __E_offsets_90(203)  => null.
	p(NT(203), (nul));
//G310: __E_offsets_90(203)  => __E_offsets_89(202) __E_offsets_90(203).
	p(NT(203), (NT(202)+NT(203)));
//G311: offsets(40)          => '[' _(11) offset(194) __E_offsets_90(203) _(11) ']'.
	p(NT(40), (T(26)+NT(11)+NT(194)+NT(203)+NT(11)+T(27)));
//G312: offset(194)          => integer(204).
	p(NT(194), (NT(204)));
//G313: offset(194)          => capture(27).
	p(NT(194), (NT(27)));
//G314: offset(194)          => shift(205).
	p(NT(194), (NT(205)));
//G315: __N_7(382)           => io_var(186).
	p(NT(382), (NT(186)));
//G316: __E_offset_91(206)   => variable(134) & ~( __N_7(382) ).	 # conjunctive
	p(NT(206), (NT(134)) & ~(NT(382)));
//G317: offset(194)          => __E_offset_91(206).
	p(NT(194), (NT(206)));
//G318: __E_shift_92(207)    => capture(27).
	p(NT(207), (NT(27)));
//G319: __N_8(383)           => io_var(186).
	p(NT(383), (NT(186)));
//G320: __E___E_shift_92_93(208) => variable(134) & ~( __N_8(383) ).	 # conjunctive
	p(NT(208), (NT(134)) & ~(NT(383)));
//G321: __E_shift_92(207)    => __E___E_shift_92_93(208).
	p(NT(207), (NT(208)));
//G322: shift(205)           => __E_shift_92(207) _(11) '-' _(11) num(164).
	p(NT(205), (NT(207)+NT(11)+T(30)+NT(11)+NT(164)));
//G323: __E_chars_94(209)    => alnum(6).
	p(NT(209), (NT(6)));
//G324: __E_chars_94(209)    => '_'.
	p(NT(209), (T(44)));
//G325: __E_chars_95(210)    => null.
	p(NT(210), (nul));
//G326: __E_chars_95(210)    => __E_chars_94(209) __E_chars_95(210).
	p(NT(210), (NT(209)+NT(210)));
//G327: chars(195)           => alpha(5) __E_chars_95(210).
	p(NT(195), (NT(5)+NT(210)));
//G328: __E_digits_96(212)   => digit(3).
	p(NT(212), (NT(3)));
//G329: __E_digits_96(212)   => digit(3) __E_digits_96(212).
	p(NT(212), (NT(3)+NT(212)));
//G330: digits(211)          => __E_digits_96(212).
	p(NT(211), (NT(212)));
//G331: num(164)             => digits(211).
	p(NT(164), (NT(211)));
//G332: __E_integer_97(213)  => '-'.
	p(NT(213), (T(30)));
//G333: __E_integer_97(213)  => null.
	p(NT(213), (nul));
//G334: integer(204)         => __E_integer_97(213) _(11) digits(211).
	p(NT(204), (NT(213)+NT(11)+NT(211)));
//G335: sym(38)              => chars(195).
	p(NT(38), (NT(195)));
//G336: capture(27)          => '$' chars(195).
	p(NT(27), (T(45)+NT(195)));
//G337: type(214)            => chars(195).
	p(NT(214), (NT(195)));
//G338: typed(31)            => _(11) ':' _(11) type(214).
	p(NT(31), (NT(11)+T(2)+NT(11)+NT(214)));
//G339: scope_id(215)        => digits(211).
	p(NT(215), (NT(211)));
//G340: __E_comment_98(217)  => printable(8).
	p(NT(217), (NT(8)));
//G341: __E_comment_98(217)  => '\t'.
	p(NT(217), (T(46)));
//G342: __E_comment_99(218)  => null.
	p(NT(218), (nul));
//G343: __E_comment_99(218)  => __E_comment_98(217) __E_comment_99(218).
	p(NT(218), (NT(217)+NT(218)));
//G344: __E_comment_100(219) => '\n'.
	p(NT(219), (T(47)));
//G345: __E_comment_100(219) => '\r'.
	p(NT(219), (T(48)));
//G346: __E_comment_100(219) => eof(1).
	p(NT(219), (NT(1)));
//G347: comment(216)         => '#' __E_comment_99(218) __E_comment_100(219).
	p(NT(216), (T(49)+NT(218)+NT(219)));
//G348: __E____101(220)      => space(2).
	p(NT(220), (NT(2)));
//G349: __E____101(220)      => comment(216).
	p(NT(220), (NT(216)));
//G350: __(32)               => __E____101(220) _(11).
	p(NT(32), (NT(220)+NT(11)));
//G351: __E___102(221)       => __(32).
	p(NT(221), (NT(32)));
//G352: __E___102(221)       => null.
	p(NT(221), (nul));
//G353: _(11)                => __E___102(221).
	p(NT(11), (NT(221)));
//G354: cli(222)             => _(11).
	p(NT(222), (NT(11)));
//G355: __E_cli_103(224)     => '.' _(11) cli_command(223) _(11).
	p(NT(224), (T(1)+NT(11)+NT(223)+NT(11)));
//G356: __E_cli_104(225)     => null.
	p(NT(225), (nul));
//G357: __E_cli_104(225)     => __E_cli_103(224) __E_cli_104(225).
	p(NT(225), (NT(224)+NT(225)));
//G358: cli(222)             => _(11) cli_command(223) _(11) __E_cli_104(225).
	p(NT(222), (NT(11)+NT(223)+NT(11)+NT(225)));
//G359: __E_cli_command_105(228) => 'q'.
	p(NT(228), (T(50)));
//G360: __E_cli_command_105(228) => 'q' 'u' 'i' 't'.
	p(NT(228), (T(50)+T(7)+T(4)+T(8)));
//G361: quit_sym(227)        => __E_cli_command_105(228).
	p(NT(227), (NT(228)));
//G362: quit_cmd(226)        => quit_sym(227).
	p(NT(226), (NT(227)));
//G363: cli_command(223)     => quit_cmd(226).
	p(NT(223), (NT(226)));
//G364: __E_cli_command_106(231) => 'v'.
	p(NT(231), (T(51)));
//G365: __E_cli_command_106(231) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(231), (T(51)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G366: version_sym(230)     => __E_cli_command_106(231).
	p(NT(230), (NT(231)));
//G367: version_cmd(229)     => version_sym(230).
	p(NT(229), (NT(230)));
//G368: cli_command(223)     => version_cmd(229).
	p(NT(223), (NT(229)));
//G369: __E_cli_command_107(234) => 'c'.
	p(NT(234), (T(14)));
//G370: __E_cli_command_107(234) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(234), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G371: clear_sym(233)       => __E_cli_command_107(234).
	p(NT(233), (NT(234)));
//G372: clear_cmd(232)       => clear_sym(233).
	p(NT(232), (NT(233)));
//G373: cli_command(223)     => clear_cmd(232).
	p(NT(223), (NT(232)));
//G374: __E___E_cli_command_108_109(238) => 'h'.
	p(NT(238), (T(52)));
//G375: __E___E_cli_command_108_109(238) => 'h' 'e' 'l' 'p'.
	p(NT(238), (T(52)+T(11)+T(10)+T(53)));
//G376: help_sym(237)        => __E___E_cli_command_108_109(238).
	p(NT(237), (NT(238)));
//G377: __E___E_cli_command_108_110(239) => __(32) help_arg(240).
	p(NT(239), (NT(32)+NT(240)));
//G378: __E___E_cli_command_108_110(239) => null.
	p(NT(239), (nul));
//G379: __E_cli_command_108(236) => help_sym(237) __E___E_cli_command_108_110(239).
	p(NT(236), (NT(237)+NT(239)));
//G380: help_cmd(235)        => __E_cli_command_108(236).
	p(NT(235), (NT(236)));
//G381: cli_command(223)     => help_cmd(235).
	p(NT(223), (NT(235)));
//G382: file_sym(243)        => 'f' 'i' 'l' 'e'.
	p(NT(243), (T(9)+T(4)+T(10)+T(11)));
//G383: __E_cli_command_111(242) => file_sym(243) __(32) q_string(244).
	p(NT(242), (NT(243)+NT(32)+NT(244)));
//G384: file_cmd(241)        => __E_cli_command_111(242).
	p(NT(241), (NT(242)));
//G385: cli_command(223)     => file_cmd(241).
	p(NT(223), (NT(241)));
//G386: valid_sym(247)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(247), (T(51)+T(16)+T(10)+T(4)+T(54)));
//G387: __E_cli_command_112(246) => valid_sym(247) __(32) normalize_cmd_arg(248).
	p(NT(246), (NT(247)+NT(32)+NT(248)));
//G388: valid_cmd(245)       => __E_cli_command_112(246).
	p(NT(245), (NT(246)));
//G389: cli_command(223)     => valid_cmd(245).
	p(NT(223), (NT(245)));
//G390: sat_sym(251)         => 's' 'a' 't'.
	p(NT(251), (T(15)+T(16)+T(8)));
//G391: __E_cli_command_113(250) => sat_sym(251) __(32) normalize_cmd_arg(248).
	p(NT(250), (NT(251)+NT(32)+NT(248)));
//G392: sat_cmd(249)         => __E_cli_command_113(250).
	p(NT(249), (NT(250)));
//G393: cli_command(223)     => sat_cmd(249).
	p(NT(223), (NT(249)));
//G394: unsat_sym(254)       => 'u' 'n' 's' 'a' 't'.
	p(NT(254), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G395: __E_cli_command_114(253) => unsat_sym(254) __(32) normalize_cmd_arg(248).
	p(NT(253), (NT(254)+NT(32)+NT(248)));
//G396: unsat_cmd(252)       => __E_cli_command_114(253).
	p(NT(252), (NT(253)));
//G397: cli_command(223)     => unsat_cmd(252).
	p(NT(223), (NT(252)));
//G398: solve_sym(257)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(257), (T(15)+T(6)+T(10)+T(51)+T(11)));
//G399: __E___E_cli_command_115_116(258) => solve_options(259).
	p(NT(258), (NT(259)));
//G400: __E___E_cli_command_115_116(258) => null.
	p(NT(258), (nul));
//G401: __E_cli_command_115(256) => solve_sym(257) __E___E_cli_command_115_116(258) __(32) wff_cmd_arg(260).
	p(NT(256), (NT(257)+NT(258)+NT(32)+NT(260)));
//G402: solve_cmd(255)       => __E_cli_command_115(256).
	p(NT(255), (NT(256)));
//G403: cli_command(223)     => solve_cmd(255).
	p(NT(223), (NT(255)));
//G404: lgrs_sym(263)        => 'l' 'g' 'r' 's'.
	p(NT(263), (T(10)+T(55)+T(19)+T(15)));
//G405: __E_cli_command_117(262) => lgrs_sym(263) __(32) wff_cmd_arg(260).
	p(NT(262), (NT(263)+NT(32)+NT(260)));
//G406: lgrs_cmd(261)        => __E_cli_command_117(262).
	p(NT(261), (NT(262)));
//G407: cli_command(223)     => lgrs_cmd(261).
	p(NT(223), (NT(261)));
//G408: __E___E_cli_command_118_119(267) => 'r'.
	p(NT(267), (T(19)));
//G409: __E___E_cli_command_118_119(267) => 'r' 'u' 'n'.
	p(NT(267), (T(19)+T(7)+T(5)));
//G410: run_sym(266)         => __E___E_cli_command_118_119(267).
	p(NT(266), (NT(267)));
//G411: __E___E_cli_command_118_120(268) => history(269).
	p(NT(268), (NT(269)));
//G412: __E___E_cli_command_118_120(268) => wff(14).
	p(NT(268), (NT(14)));
//G413: __E_cli_command_118(265) => run_sym(266) __(32) __E___E_cli_command_118_120(268).
	p(NT(265), (NT(266)+NT(32)+NT(268)));
//G414: run_cmd(264)         => __E_cli_command_118(265).
	p(NT(264), (NT(265)));
//G415: cli_command(223)     => run_cmd(264).
	p(NT(223), (NT(264)));
//G416: __E___E_cli_command_121_122(273) => 'n'.
	p(NT(273), (T(5)));
//G417: __E___E_cli_command_121_122(273) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(273), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(56)+T(11)));
//G418: normalize_sym(272)   => __E___E_cli_command_121_122(273).
	p(NT(272), (NT(273)));
//G419: __E_cli_command_121(271) => normalize_sym(272) __(32) normalize_cmd_arg(248).
	p(NT(271), (NT(272)+NT(32)+NT(248)));
//G420: normalize_cmd(270)   => __E_cli_command_121(271).
	p(NT(270), (NT(271)));
//G421: cli_command(223)     => normalize_cmd(270).
	p(NT(223), (NT(270)));
//G422: __E___E_cli_command_123_124(277) => 's'.
	p(NT(277), (T(15)));
//G423: __E___E_cli_command_123_124(277) => 's' 'u' 'b' 's' 't'.
	p(NT(277), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G424: __E___E_cli_command_123_124(277) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(277), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G425: subst_sym(276)       => __E___E_cli_command_123_124(277).
	p(NT(276), (NT(277)));
//G426: __E_cli_command_123(275) => subst_sym(276) __(32) nf_cmd_arg(278) _(11) '[' _(11) nf_cmd_arg(278) _(11) '/' _(11) nf_cmd_arg(278) _(11) ']'.
	p(NT(275), (NT(276)+NT(32)+NT(278)+NT(11)+T(26)+NT(11)+NT(278)+NT(11)+T(57)+NT(11)+NT(278)+NT(11)+T(27)));
//G427: subst_cmd(274)       => __E_cli_command_123(275).
	p(NT(274), (NT(275)));
//G428: cli_command(223)     => subst_cmd(274).
	p(NT(223), (NT(274)));
//G429: __E___E_cli_command_125_126(282) => 'i'.
	p(NT(282), (T(4)));
//G430: __E___E_cli_command_125_126(282) => 'i' 'n' 's' 't'.
	p(NT(282), (T(4)+T(5)+T(15)+T(8)));
//G431: __E___E_cli_command_125_126(282) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(282), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G432: inst_sym(281)        => __E___E_cli_command_125_126(282).
	p(NT(281), (NT(282)));
//G433: __E_cli_command_125(280) => inst_sym(281) __(32) inst_args(283).
	p(NT(280), (NT(281)+NT(32)+NT(283)));
//G434: inst_cmd(279)        => __E_cli_command_125(280).
	p(NT(279), (NT(280)));
//G435: cli_command(223)     => inst_cmd(279).
	p(NT(223), (NT(279)));
//G436: dnf_sym(286)         => 'd' 'n' 'f'.
	p(NT(286), (T(54)+T(5)+T(9)));
//G437: __E_cli_command_127(285) => dnf_sym(286) __(32) nf_cmd_arg(278).
	p(NT(285), (NT(286)+NT(32)+NT(278)));
//G438: dnf_cmd(284)         => __E_cli_command_127(285).
	p(NT(284), (NT(285)));
//G439: cli_command(223)     => dnf_cmd(284).
	p(NT(223), (NT(284)));
//G440: cnf_sym(289)         => 'c' 'n' 'f'.
	p(NT(289), (T(14)+T(5)+T(9)));
//G441: __E_cli_command_128(288) => cnf_sym(289) __(32) nf_cmd_arg(278).
	p(NT(288), (NT(289)+NT(32)+NT(278)));
//G442: cnf_cmd(287)         => __E_cli_command_128(288).
	p(NT(287), (NT(288)));
//G443: cli_command(223)     => cnf_cmd(287).
	p(NT(223), (NT(287)));
//G444: anf_sym(292)         => 'a' 'n' 'f'.
	p(NT(292), (T(16)+T(5)+T(9)));
//G445: __E_cli_command_129(291) => anf_sym(292) __(32) nf_cmd_arg(278).
	p(NT(291), (NT(292)+NT(32)+NT(278)));
//G446: anf_cmd(290)         => __E_cli_command_129(291).
	p(NT(290), (NT(291)));
//G447: cli_command(223)     => anf_cmd(290).
	p(NT(223), (NT(290)));
//G448: nnf_sym(295)         => 'n' 'n' 'f'.
	p(NT(295), (T(5)+T(5)+T(9)));
//G449: __E_cli_command_130(294) => nnf_sym(295) __(32) nf_cmd_arg(278).
	p(NT(294), (NT(295)+NT(32)+NT(278)));
//G450: nnf_cmd(293)         => __E_cli_command_130(294).
	p(NT(293), (NT(294)));
//G451: cli_command(223)     => nnf_cmd(293).
	p(NT(223), (NT(293)));
//G452: pnf_sym(298)         => 'p' 'n' 'f'.
	p(NT(298), (T(53)+T(5)+T(9)));
//G453: __E_cli_command_131(297) => pnf_sym(298) __(32) nf_cmd_arg(278).
	p(NT(297), (NT(298)+NT(32)+NT(278)));
//G454: pnf_cmd(296)         => __E_cli_command_131(297).
	p(NT(296), (NT(297)));
//G455: cli_command(223)     => pnf_cmd(296).
	p(NT(223), (NT(296)));
//G456: mnf_sym(301)         => 'm' 'n' 'f'.
	p(NT(301), (T(21)+T(5)+T(9)));
//G457: __E_cli_command_132(300) => mnf_sym(301) __(32) nf_cmd_arg(278).
	p(NT(300), (NT(301)+NT(32)+NT(278)));
//G458: mnf_cmd(299)         => __E_cli_command_132(300).
	p(NT(299), (NT(300)));
//G459: cli_command(223)     => mnf_cmd(299).
	p(NT(223), (NT(299)));
//G460: snf_sym(304)         => 's' 'n' 'f'.
	p(NT(304), (T(15)+T(5)+T(9)));
//G461: __E_cli_command_133(303) => snf_sym(304) __(32) nf_cmd_arg(278).
	p(NT(303), (NT(304)+NT(32)+NT(278)));
//G462: snf_cmd(302)         => __E_cli_command_133(303).
	p(NT(302), (NT(303)));
//G463: cli_command(223)     => snf_cmd(302).
	p(NT(223), (NT(302)));
//G464: onf_sym(307)         => 'o' 'n' 'f'.
	p(NT(307), (T(6)+T(5)+T(9)));
//G465: __E_cli_command_134(306) => onf_sym(307) __(32) variable(134) __(32) onf_cmd_arg(308).
	p(NT(306), (NT(307)+NT(32)+NT(134)+NT(32)+NT(308)));
//G466: onf_cmd(305)         => __E_cli_command_134(306).
	p(NT(305), (NT(306)));
//G467: cli_command(223)     => onf_cmd(305).
	p(NT(223), (NT(305)));
//G468: qelim_sym(311)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(311), (T(50)+T(11)+T(10)+T(4)+T(21)));
//G469: __E_cli_command_135(310) => qelim_sym(311) __(32) wff_cmd_arg(260).
	p(NT(310), (NT(311)+NT(32)+NT(260)));
//G470: qelim_cmd(309)       => __E_cli_command_135(310).
	p(NT(309), (NT(310)));
//G471: cli_command(223)     => qelim_cmd(309).
	p(NT(223), (NT(309)));
//G472: get_sym(314)         => 'g' 'e' 't'.
	p(NT(314), (T(55)+T(11)+T(8)));
//G473: __E___E_cli_command_136_137(315) => __(32) option_name(316).
	p(NT(315), (NT(32)+NT(316)));
//G474: __E___E_cli_command_136_137(315) => null.
	p(NT(315), (nul));
//G475: __E_cli_command_136(313) => get_sym(314) __E___E_cli_command_136_137(315).
	p(NT(313), (NT(314)+NT(315)));
//G476: get_cmd(312)         => __E_cli_command_136(313).
	p(NT(312), (NT(313)));
//G477: cli_command(223)     => get_cmd(312).
	p(NT(223), (NT(312)));
//G478: set_sym(319)         => 's' 'e' 't'.
	p(NT(319), (T(15)+T(11)+T(8)));
//G479: __E___E_cli_command_138_139(320) => __(32).
	p(NT(320), (NT(32)));
//G480: __E___E_cli_command_138_139(320) => _(11) '=' _(11).
	p(NT(320), (NT(11)+T(3)+NT(11)));
//G481: __E_cli_command_138(318) => set_sym(319) __(32) option_name(316) __E___E_cli_command_138_139(320) option_value(321).
	p(NT(318), (NT(319)+NT(32)+NT(316)+NT(320)+NT(321)));
//G482: set_cmd(317)         => __E_cli_command_138(318).
	p(NT(317), (NT(318)));
//G483: cli_command(223)     => set_cmd(317).
	p(NT(223), (NT(317)));
//G484: enable_sym(324)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(324), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G485: __E_cli_command_140(323) => enable_sym(324) __(32) option_name(316).
	p(NT(323), (NT(324)+NT(32)+NT(316)));
//G486: enable_cmd(322)      => __E_cli_command_140(323).
	p(NT(322), (NT(323)));
//G487: cli_command(223)     => enable_cmd(322).
	p(NT(223), (NT(322)));
//G488: disable_sym(327)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(327), (T(54)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G489: __E_cli_command_141(326) => disable_sym(327) __(32) option_name(316).
	p(NT(326), (NT(327)+NT(32)+NT(316)));
//G490: disable_cmd(325)     => __E_cli_command_141(326).
	p(NT(325), (NT(326)));
//G491: cli_command(223)     => disable_cmd(325).
	p(NT(223), (NT(325)));
//G492: toggle_sym(330)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(330), (T(8)+T(6)+T(55)+T(55)+T(10)+T(11)));
//G493: __E_cli_command_142(329) => toggle_sym(330) __(32) option_name(316).
	p(NT(329), (NT(330)+NT(32)+NT(316)));
//G494: toggle_cmd(328)      => __E_cli_command_142(329).
	p(NT(328), (NT(329)));
//G495: cli_command(223)     => toggle_cmd(328).
	p(NT(223), (NT(328)));
//G496: __E___E_cli_command_143_144(334) => 'd' 'e' 'f' 's'.
	p(NT(334), (T(54)+T(11)+T(9)+T(15)));
//G497: __E___E_cli_command_143_144(334) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(334), (T(54)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G498: def_sym(333)         => __E___E_cli_command_143_144(334).
	p(NT(333), (NT(334)));
//G499: __E_cli_command_143(332) => def_sym(333).
	p(NT(332), (NT(333)));
//G500: def_list_cmd(331)    => __E_cli_command_143(332).
	p(NT(331), (NT(332)));
//G501: cli_command(223)     => def_list_cmd(331).
	p(NT(223), (NT(331)));
//G502: __E_cli_command_145(336) => def_sym(333) __(32) num(164).
	p(NT(336), (NT(333)+NT(32)+NT(164)));
//G503: def_print_cmd(335)   => __E_cli_command_145(336).
	p(NT(335), (NT(336)));
//G504: cli_command(223)     => def_print_cmd(335).
	p(NT(223), (NT(335)));
//G505: def_rr_cmd(337)      => rec_relation(23).
	p(NT(337), (NT(23)));
//G506: cli_command(223)     => def_rr_cmd(337).
	p(NT(223), (NT(337)));
//G507: def_input_cmd(338)   => input_def(18).
	p(NT(338), (NT(18)));
//G508: cli_command(223)     => def_input_cmd(338).
	p(NT(223), (NT(338)));
//G509: def_output_cmd(339)  => output_def(19).
	p(NT(339), (NT(19)));
//G510: cli_command(223)     => def_output_cmd(339).
	p(NT(223), (NT(339)));
//G511: __E___E_cli_command_146_147(343) => 'h' 'i' 's' 't'.
	p(NT(343), (T(52)+T(4)+T(15)+T(8)));
//G512: __E___E_cli_command_146_147(343) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(343), (T(52)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G513: history_sym(342)     => __E___E_cli_command_146_147(343).
	p(NT(342), (NT(343)));
//G514: __E_cli_command_146(341) => history_sym(342).
	p(NT(341), (NT(342)));
//G515: history_list_cmd(340) => __E_cli_command_146(341).
	p(NT(340), (NT(341)));
//G516: cli_command(223)     => history_list_cmd(340).
	p(NT(223), (NT(340)));
//G517: __E_cli_command_148(345) => history_sym(342) __(32) history(269).
	p(NT(345), (NT(342)+NT(32)+NT(269)));
//G518: history_print_cmd(344) => __E_cli_command_148(345).
	p(NT(344), (NT(345)));
//G519: cli_command(223)     => history_print_cmd(344).
	p(NT(223), (NT(344)));
//G520: __E_cli_command_149(347) => wff(14).
	p(NT(347), (NT(14)));
//G521: __E_cli_command_149(347) => bf(28).
	p(NT(347), (NT(28)));
//G522: history_store_cmd(346) => __E_cli_command_149(347).
	p(NT(346), (NT(347)));
//G523: cli_command(223)     => history_store_cmd(346).
	p(NT(223), (NT(346)));
//G524: __E_solve_options_150(348) => __(32) solve_option(349).
	p(NT(348), (NT(32)+NT(349)));
//G525: __E_solve_options_151(350) => null.
	p(NT(350), (nul));
//G526: __E_solve_options_151(350) => __E_solve_options_150(348) __E_solve_options_151(350).
	p(NT(350), (NT(348)+NT(350)));
//G527: solve_options(259)   => __E_solve_options_151(350).
	p(NT(259), (NT(350)));
//G528: __E_solve_option_152(351) => solver_mode(352).
	p(NT(351), (NT(352)));
//G529: __E_solve_option_152(351) => type(214).
	p(NT(351), (NT(214)));
//G530: solve_option(349)    => '-' '-' __E_solve_option_152(351).
	p(NT(349), (T(30)+T(30)+NT(351)));
//G531: __E_solver_mode_153(354) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(354), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G532: __E_solver_mode_153(354) => 'm' 'i' 'n'.
	p(NT(354), (T(21)+T(4)+T(5)));
//G533: solver_mode_minimum(353) => __E_solver_mode_153(354).
	p(NT(353), (NT(354)));
//G534: solver_mode(352)     => solver_mode_minimum(353).
	p(NT(352), (NT(353)));
//G535: __E_solver_mode_154(356) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(356), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G536: __E_solver_mode_154(356) => 'm' 'a' 'x'.
	p(NT(356), (T(21)+T(16)+T(29)));
//G537: solver_mode_maximum(355) => __E_solver_mode_154(356).
	p(NT(355), (NT(356)));
//G538: solver_mode(352)     => solver_mode_maximum(355).
	p(NT(352), (NT(355)));
//G539: q_file_name(36)      => '"' file_name(357) '"'.
	p(NT(36), (T(58)+NT(357)+T(58)));
//G540: __E_file_name_155(358) => printable(8).
	p(NT(358), (NT(8)));
//G541: __E_file_name_155(358) => printable(8) __E_file_name_155(358).
	p(NT(358), (NT(8)+NT(358)));
//G542: file_name(357)       => __E_file_name_155(358).
	p(NT(357), (NT(358)));
//G543: __E_option_name_156(359) => alnum(6).
	p(NT(359), (NT(6)));
//G544: __E_option_name_156(359) => alnum(6) __E_option_name_156(359).
	p(NT(359), (NT(6)+NT(359)));
//G545: option_name(316)     => __E_option_name_156(359).
	p(NT(316), (NT(359)));
//G546: __E_option_value_157(360) => alnum(6).
	p(NT(360), (NT(6)));
//G547: __E_option_value_157(360) => alnum(6) __E_option_value_157(360).
	p(NT(360), (NT(6)+NT(360)));
//G548: option_value(321)    => __E_option_value_157(360).
	p(NT(321), (NT(360)));
//G549: bf_cmd_arg(361)      => history(269).
	p(NT(361), (NT(269)));
//G550: bf_cmd_arg(361)      => bf(28).
	p(NT(361), (NT(28)));
//G551: wff_cmd_arg(260)     => history(269).
	p(NT(260), (NT(269)));
//G552: wff_cmd_arg(260)     => wff(14).
	p(NT(260), (NT(14)));
//G553: nf_cmd_arg(278)      => history(269).
	p(NT(278), (NT(269)));
//G554: nf_cmd_arg(278)      => ref(25).
	p(NT(278), (NT(25)));
//G555: nf_cmd_arg(278)      => wff(14).
	p(NT(278), (NT(14)));
//G556: nf_cmd_arg(278)      => bf(28).
	p(NT(278), (NT(28)));
//G557: onf_cmd_arg(308)     => history(269).
	p(NT(308), (NT(269)));
//G558: onf_cmd_arg(308)     => wff(14).
	p(NT(308), (NT(14)));
//G559: normalize_cmd_arg(248) => history(269).
	p(NT(248), (NT(269)));
//G560: normalize_cmd_arg(248) => spec(10).
	p(NT(248), (NT(10)));
//G561: normalize_cmd_arg(248) => ref(25).
	p(NT(248), (NT(25)));
//G562: normalize_cmd_arg(248) => wff(14).
	p(NT(248), (NT(14)));
//G563: normalize_cmd_arg(248) => bf(28).
	p(NT(248), (NT(28)));
//G564: inst_args(283)       => wff_cmd_arg(260) _(11) '[' _(11) variable(134) _(11) '/' _(11) bf_cmd_arg(361) _(11) ']'.
	p(NT(283), (NT(260)+NT(11)+T(26)+NT(11)+NT(134)+NT(11)+T(57)+NT(11)+NT(361)+NT(11)+T(27)));
//G565: inst_args(283)       => bf_cmd_arg(361) _(11) '[' _(11) variable(134) _(11) '/' _(11) bf_cmd_arg(361) _(11) ']'.
	p(NT(283), (NT(361)+NT(11)+T(26)+NT(11)+NT(134)+NT(11)+T(57)+NT(11)+NT(361)+NT(11)+T(27)));
//G566: help_arg(240)        => help_sym(237).
	p(NT(240), (NT(237)));
//G567: help_arg(240)        => version_sym(230).
	p(NT(240), (NT(230)));
//G568: help_arg(240)        => quit_sym(227).
	p(NT(240), (NT(227)));
//G569: help_arg(240)        => clear_sym(233).
	p(NT(240), (NT(233)));
//G570: help_arg(240)        => get_sym(314).
	p(NT(240), (NT(314)));
//G571: help_arg(240)        => set_sym(319).
	p(NT(240), (NT(319)));
//G572: help_arg(240)        => enable_sym(324).
	p(NT(240), (NT(324)));
//G573: help_arg(240)        => disable_sym(327).
	p(NT(240), (NT(327)));
//G574: help_arg(240)        => toggle_sym(330).
	p(NT(240), (NT(330)));
//G575: help_arg(240)        => file_sym(243).
	p(NT(240), (NT(243)));
//G576: help_arg(240)        => history_sym(342).
	p(NT(240), (NT(342)));
//G577: help_arg(240)        => abs_history_sym(362).
	p(NT(240), (NT(362)));
//G578: help_arg(240)        => rel_history_sym(363).
	p(NT(240), (NT(363)));
//G579: help_arg(240)        => selection_sym(364).
	p(NT(240), (NT(364)));
//G580: help_arg(240)        => def_sym(333).
	p(NT(240), (NT(333)));
//G581: help_arg(240)        => inst_sym(281).
	p(NT(240), (NT(281)));
//G582: help_arg(240)        => subst_sym(276).
	p(NT(240), (NT(276)));
//G583: help_arg(240)        => normalize_sym(272).
	p(NT(240), (NT(272)));
//G584: help_arg(240)        => execute_sym(365).
	p(NT(240), (NT(365)));
//G585: help_arg(240)        => solve_sym(257).
	p(NT(240), (NT(257)));
//G586: help_arg(240)        => valid_sym(247).
	p(NT(240), (NT(247)));
//G587: help_arg(240)        => sat_sym(251).
	p(NT(240), (NT(251)));
//G588: help_arg(240)        => unsat_sym(254).
	p(NT(240), (NT(254)));
//G589: help_arg(240)        => run_sym(266).
	p(NT(240), (NT(266)));
//G590: help_arg(240)        => dnf_sym(286).
	p(NT(240), (NT(286)));
//G591: help_arg(240)        => cnf_sym(289).
	p(NT(240), (NT(289)));
//G592: help_arg(240)        => anf_sym(292).
	p(NT(240), (NT(292)));
//G593: help_arg(240)        => snf_sym(304).
	p(NT(240), (NT(304)));
//G594: help_arg(240)        => nnf_sym(295).
	p(NT(240), (NT(295)));
//G595: help_arg(240)        => pnf_sym(298).
	p(NT(240), (NT(298)));
//G596: help_arg(240)        => mnf_sym(301).
	p(NT(240), (NT(301)));
//G597: help_arg(240)        => onf_sym(307).
	p(NT(240), (NT(307)));
//G598: help_arg(240)        => qelim_sym(311).
	p(NT(240), (NT(311)));
//G599: __E___E_help_arg_158_159(368) => 's'.
	p(NT(368), (T(15)));
//G600: __E___E_help_arg_158_159(368) => null.
	p(NT(368), (nul));
//G601: __E_help_arg_158(367) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_158_159(368).
	p(NT(367), (T(11)+T(29)+T(16)+T(21)+T(53)+T(10)+T(11)+NT(368)));
//G602: examples_sym(366)    => __E_help_arg_158(367).
	p(NT(366), (NT(367)));
//G603: help_arg(240)        => examples_sym(366).
	p(NT(240), (NT(366)));
//G604: __E_history_160(370) => '%'.
	p(NT(370), (T(59)));
//G605: rel_history_sym(363) => '%' '-'.
	p(NT(363), (T(59)+T(30)));
//G606: history_id(372)      => digits(211).
	p(NT(372), (NT(211)));
//G607: __E___E_history_160_161(371) => history_id(372).
	p(NT(371), (NT(372)));
//G608: __E___E_history_160_161(371) => null.
	p(NT(371), (nul));
//G609: __E_history_160(370) => rel_history_sym(363) __E___E_history_160_161(371).
	p(NT(370), (NT(363)+NT(371)));
//G610: rel_history(369)     => __E_history_160(370).
	p(NT(369), (NT(370)));
//G611: history(269)         => rel_history(369).
	p(NT(269), (NT(369)));
//G612: abs_history_sym(362) => '%'.
	p(NT(362), (T(59)));
//G613: __E_history_162(374) => abs_history_sym(362) history_id(372).
	p(NT(374), (NT(362)+NT(372)));
//G614: abs_history(373)     => __E_history_162(374).
	p(NT(373), (NT(374)));
//G615: history(269)         => abs_history(373).
	p(NT(269), (NT(373)));
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
		spec, _, definitions, main, wff, __E_definitions_0, __E___E_definitions_0_1, ref_relation, input_def, output_def, 
		__E_definitions_2, ref_relations, __E_ref_relations_3, rec_relation, __E_ref_relations_4, ref, __E_rec_relation_5, capture, bf, io_var_name, 
		__E_input_def_6, typed, __, stream, __E_output_def_7, __E_output_def_8, q_file_name, console_sym, sym, __E_ref_9, 
		offsets, ref_args, __E_ref_10, fp_fallback, first_sym, last_sym, __E_ref_args_11, __E___E_ref_args_11_12, ref_arg, __E___E_ref_args_11_13, 
		__E___E_ref_args_11_14, library, rules, __E_library_15, __E___E_library_15_16, rule, __E___E___E_library_15_16_17, wff_rule, bf_rule, __E___E_library_15_18, 
		wff_matcher, wff_body, bf_matcher, bf_body, builder, builder_head, builder_body, __E_builder_head_19, __E_builder_head_20, bf_builder_body, 
		__E_builder_body_21, wff_builder_body, __E_builder_body_22, tau_constant_source, rec_relations, __E_tau_constant_source_23, wff_parenthesis, __E_wff_24, wff_sometimes, __E_wff_25, 
		__E___E_wff_25_26, wff_always, __E_wff_27, __E___E_wff_27_28, wff_conditional, __E_wff_29, wff_all, __E_wff_30, q_vars, wff_ex, 
		__E_wff_31, wff_ref, constraint, wff_imply, __E_wff_32, wff_rimply, __E_wff_33, wff_equiv, __E_wff_34, wff_or, 
		__E_wff_35, wff_xor, __E_wff_36, wff_and, __E_wff_37, wff_neg, __E_wff_38, wff_t, wff_f, bf_interval, 
		__E_wff_39, bf_eq, __E_wff_40, bf_neq, __E_wff_41, bf_lteq, __E_wff_42, bf_nlteq, __E_wff_43, bf_gt, 
		__E_wff_44, bf_ngt, __E_wff_45, bf_gteq, __E_wff_46, bf_ngteq, __E_wff_47, bf_lt, __E_wff_48, bf_nlt, 
		__E_wff_49, bf_parenthesis, __E_bf_50, bf_constant, variable, bf_splitter, __E_bf_51, bf_ref, bf_or, __E_bf_52, 
		bf_xor, __E_bf_53, bf_t, __E_bf_54, __E___E_bf_54_55, bf_f, __E_bf_56, __E___E_bf_56_57, bf_and, __E_bf_58, 
		__E___E_bf_58_59, bf_neg, __E_bf_60, bf_neg_oprnd, __E___E_bf_60_61, bf_and_nosep, __E_bf_62, bf_and_nosep_1st_oprnd, __E___E_bf_62_63, bf_and_nosep_2nd_oprnd, 
		__E___E_bf_62_64, ctn_neq, __E_constraint_65, ctnvar, num, ctn_eq, __E_constraint_66, ctn_gteq, __E_constraint_67, ctn_gt, 
		__E_constraint_68, ctn_lteq, __E_constraint_69, ctn_lt, __E_constraint_70, __E_bf_constant_71, source, __E_bf_constant_72, src_c, __E_source_73, 
		__E___E_source_73_74, __E___E_source_73_75, __E_src_c_76, __E_src_c_77, __E_variable_78, uconst, io_var, var_name, __E_variable_79, __E_var_name_80, 
		__E_var_name_81, __E_var_name_82, __E_var_name_83, __E_var_name_84, offset, chars, uconst_name, __E_uconst_85, __E___E_uconst_85_86, q_var, 
		__E_q_vars_87, __E_q_vars_88, __E_offsets_89, __E_offsets_90, integer, shift, __E_offset_91, __E_shift_92, __E___E_shift_92_93, __E_chars_94, 
		__E_chars_95, digits, __E_digits_96, __E_integer_97, type, scope_id, comment, __E_comment_98, __E_comment_99, __E_comment_100, 
		__E____101, __E___102, cli, cli_command, __E_cli_103, __E_cli_104, quit_cmd, quit_sym, __E_cli_command_105, version_cmd, 
		version_sym, __E_cli_command_106, clear_cmd, clear_sym, __E_cli_command_107, help_cmd, __E_cli_command_108, help_sym, __E___E_cli_command_108_109, __E___E_cli_command_108_110, 
		help_arg, file_cmd, __E_cli_command_111, file_sym, q_string, valid_cmd, __E_cli_command_112, valid_sym, normalize_cmd_arg, sat_cmd, 
		__E_cli_command_113, sat_sym, unsat_cmd, __E_cli_command_114, unsat_sym, solve_cmd, __E_cli_command_115, solve_sym, __E___E_cli_command_115_116, solve_options, 
		wff_cmd_arg, lgrs_cmd, __E_cli_command_117, lgrs_sym, run_cmd, __E_cli_command_118, run_sym, __E___E_cli_command_118_119, __E___E_cli_command_118_120, history, 
		normalize_cmd, __E_cli_command_121, normalize_sym, __E___E_cli_command_121_122, subst_cmd, __E_cli_command_123, subst_sym, __E___E_cli_command_123_124, nf_cmd_arg, inst_cmd, 
		__E_cli_command_125, inst_sym, __E___E_cli_command_125_126, inst_args, dnf_cmd, __E_cli_command_127, dnf_sym, cnf_cmd, __E_cli_command_128, cnf_sym, 
		anf_cmd, __E_cli_command_129, anf_sym, nnf_cmd, __E_cli_command_130, nnf_sym, pnf_cmd, __E_cli_command_131, pnf_sym, mnf_cmd, 
		__E_cli_command_132, mnf_sym, snf_cmd, __E_cli_command_133, snf_sym, onf_cmd, __E_cli_command_134, onf_sym, onf_cmd_arg, qelim_cmd, 
		__E_cli_command_135, qelim_sym, get_cmd, __E_cli_command_136, get_sym, __E___E_cli_command_136_137, option_name, set_cmd, __E_cli_command_138, set_sym, 
		__E___E_cli_command_138_139, option_value, enable_cmd, __E_cli_command_140, enable_sym, disable_cmd, __E_cli_command_141, disable_sym, toggle_cmd, __E_cli_command_142, 
		toggle_sym, def_list_cmd, __E_cli_command_143, def_sym, __E___E_cli_command_143_144, def_print_cmd, __E_cli_command_145, def_rr_cmd, def_input_cmd, def_output_cmd, 
		history_list_cmd, __E_cli_command_146, history_sym, __E___E_cli_command_146_147, history_print_cmd, __E_cli_command_148, history_store_cmd, __E_cli_command_149, __E_solve_options_150, solve_option, 
		__E_solve_options_151, __E_solve_option_152, solver_mode, solver_mode_minimum, __E_solver_mode_153, solver_mode_maximum, __E_solver_mode_154, file_name, __E_file_name_155, __E_option_name_156, 
		__E_option_value_157, bf_cmd_arg, abs_history_sym, rel_history_sym, selection_sym, execute_sym, examples_sym, __E_help_arg_158, __E___E_help_arg_158_159, rel_history, 
		__E_history_160, __E___E_history_160_161, history_id, abs_history, __E_history_162, __N_0, __N_1, __N_2, __N_3, __N_4, 
		__N_5, __N_6, __N_7, __N_8, 
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
