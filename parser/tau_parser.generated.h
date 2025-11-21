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
	"spec", "_", "definitions", "main", "wff", "spec_multiline", "__E_spec_multiline_0", "rec_relation", "input_def", "output_def", 
	"__E_spec_multiline_1", "__E_definitions_2", "__E___E_definitions_2_3", "__E_definitions_4", "rec_relations", "__E_rec_relations_5", "__E_rec_relations_6", "ref", "__E_rec_relation_7", "capture", 
	"bf", "io_var_name", "__E_input_def_8", "typed", "__", "stream", "__E_output_def_9", "__E_output_def_10", "q_file_name", "console_sym", 
	"sym", "__E_ref_11", "offsets", "ref_args", "__E_ref_12", "__E_ref_13", "fp_fallback", "first_sym", "last_sym", "__E_ref_args_14", 
	"__E___E_ref_args_14_15", "ref_arg", "__E___E_ref_args_14_16", "__E___E_ref_args_14_17", "tau_constant_source", "__E_tau_constant_source_18", "wff_parenthesis", "__E_wff_19", "wff_sometimes", "__E_wff_20", 
	"__E___E_wff_20_21", "wff_always", "__E_wff_22", "__E___E_wff_22_23", "wff_conditional", "__E_wff_24", "wff_all", "__E_wff_25", "q_vars", "wff_ex", 
	"__E_wff_26", "wff_ref", "constraint", "wff_imply", "__E_wff_27", "wff_rimply", "__E_wff_28", "wff_equiv", "__E_wff_29", "wff_or", 
	"__E_wff_30", "wff_xor", "__E_wff_31", "wff_and", "__E_wff_32", "wff_neg", "__E_wff_33", "wff_t", "wff_f", "bf_interval", 
	"__E_wff_34", "bf_eq", "__E_wff_35", "bf_neq", "__E_wff_36", "bf_lteq", "__E_wff_37", "bf_nlteq", "__E_wff_38", "bf_gt", 
	"__E_wff_39", "bf_ngt", "__E_wff_40", "bf_gteq", "__E_wff_41", "bf_ngteq", "__E_wff_42", "bf_lt", "__E_wff_43", "bf_nlt", 
	"__E_wff_44", "bf_parenthesis", "__E_bf_45", "ba_constant", "variable", "bf_fall", "__E_bf_46", "bf_fex", "__E_bf_47", "bf_ref", 
	"bf_shr", "__E_bf_48", "bf_shl", "__E_bf_49", "bf_xnor", "__E_bf_50", "bf_nor", "__E_bf_51", "bf_nand", "__E_bf_52", 
	"bf_sub", "__E_bf_53", "bf_add", "__E_bf_54", "bf_mod", "__E_bf_55", "bf_div", "__E_bf_56", "bf_mul", "__E_bf_57", 
	"bf_or", "__E_bf_58", "bf_xor", "__E_bf_59", "bf_t", "__E_bf_60", "__E___E_bf_60_61", "bf_f", "__E_bf_62", "__E___E_bf_62_63", 
	"bf_and", "__E_bf_64", "__E___E_bf_64_65", "bf_neg", "__E_bf_66", "bf_neg_oprnd", "__E___E_bf_66_67", "bf_and_nosep", "__E_bf_68", "bf_and_nosep_1st_oprnd", 
	"__E___E_bf_68_69", "bf_and_nosep_2nd_oprnd", "__E___E_bf_68_70", "ctn_neq", "__E_constraint_71", "ctnvar", "num", "ctn_eq", "__E_constraint_72", "ctn_gteq", 
	"__E_constraint_73", "ctn_gt", "__E_constraint_74", "ctn_lteq", "__E_constraint_75", "ctn_lt", "__E_constraint_76", "__E_ba_constant_77", "source", "__E_ba_constant_78", 
	"src_c", "__E_source_79", "__E___E_source_79_80", "__E___E_source_79_81", "__E_src_c_82", "__E_src_c_83", "__E_variable_84", "uconst", "io_var", "var_name", 
	"__E_variable_85", "__E_var_name_86", "__E_var_name_87", "__E_var_name_88", "__E_var_name_89", "__E_var_name_90", "offset", "chars", "uconst_name", "__E_uconst_91", 
	"__E___E_uconst_91_92", "q_var", "__E_q_vars_93", "__E_q_vars_94", "__E_offsets_95", "__E_offsets_96", "integer", "shift", "__E_offset_97", "__E_shift_98", 
	"__E___E_shift_98_99", "__E_chars_100", "__E_chars_101", "digits", "__E_digits_102", "__E_integer_103", "type", "__E_typed_104", "subtype", "comment", 
	"__E_comment_105", "__E_comment_106", "__E_comment_107", "cli", "cli_command", "__E_cli_108", "__E_cli_109", "quit_cmd", "quit_sym", "__E_cli_command_110", 
	"version_cmd", "version_sym", "__E_cli_command_111", "clear_cmd", "clear_sym", "__E_cli_command_112", "help_cmd", "__E_cli_command_113", "help_sym", "__E___E_cli_command_113_114", 
	"__E___E_cli_command_113_115", "help_arg", "file_cmd", "__E_cli_command_116", "file_sym", "q_string", "valid_cmd", "__E_cli_command_117", "valid_sym", "normalize_cmd_arg", 
	"sat_cmd", "__E_cli_command_118", "sat_sym", "unsat_cmd", "__E_cli_command_119", "unsat_sym", "solve_cmd", "__E_cli_command_120", "solve_sym", "__E___E_cli_command_120_121", 
	"solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_122", "lgrs_sym", "run_cmd", "__E_cli_command_123", "run_sym", "__E___E_cli_command_123_124", "__E___E_cli_command_123_125", 
	"history", "normalize_cmd", "__E_cli_command_126", "normalize_sym", "__E___E_cli_command_126_127", "subst_cmd", "__E_cli_command_128", "subst_sym", "__E___E_cli_command_128_129", "nf_cmd_arg", 
	"inst_cmd", "__E_cli_command_130", "inst_sym", "__E___E_cli_command_130_131", "inst_args", "dnf_cmd", "__E_cli_command_132", "dnf_sym", "cnf_cmd", "__E_cli_command_133", 
	"cnf_sym", "anf_cmd", "__E_cli_command_134", "anf_sym", "nnf_cmd", "__E_cli_command_135", "nnf_sym", "pnf_cmd", "__E_cli_command_136", "pnf_sym", 
	"mnf_cmd", "__E_cli_command_137", "mnf_sym", "onf_cmd", "__E_cli_command_138", "onf_sym", "onf_cmd_arg", "qelim_cmd", "__E_cli_command_139", "qelim_sym", 
	"get_cmd", "__E_cli_command_140", "get_sym", "__E___E_cli_command_140_141", "option_name", "set_cmd", "__E_cli_command_142", "set_sym", "__E___E_cli_command_142_143", "option_value", 
	"enable_cmd", "__E_cli_command_144", "enable_sym", "disable_cmd", "__E_cli_command_145", "disable_sym", "toggle_cmd", "__E_cli_command_146", "toggle_sym", "def_list_cmd", 
	"__E_cli_command_147", "def_sym", "__E___E_cli_command_147_148", "def_print_cmd", "__E_cli_command_149", "def_rr_cmd", "def_input_cmd", "def_output_cmd", "history_list_cmd", "__E_cli_command_150", 
	"history_sym", "__E___E_cli_command_150_151", "history_print_cmd", "__E_cli_command_152", "history_store_cmd", "__E_cli_command_153", "__E_solve_options_154", "solve_option", "__E_solve_options_155", "__E_solve_option_156", 
	"solver_mode", "solver_mode_minimum", "__E_solver_mode_157", "solver_mode_maximum", "__E_solver_mode_158", "file_name", "__E_file_name_159", "__E_option_name_160", "__E_option_value_161", "bf_cmd_arg", 
	"abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", "snf_sym", "examples_sym", "__E_help_arg_162", "__E___E_help_arg_162_163", "rel_history", "__E_history_164", 
	"__E___E_history_164_165", "history_id", "abs_history", "__E_history_166", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", 
	"__N_6", "__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', 'a', 'b', 'k', 
	'r', ',', 'm', '<', '>', 'w', 'y', '[', ']', '?', 
	'x', '-', '|', '^', '&', '!', 'T', 'F', '+', '%', 
	'/', '*', '1', '0', '\'', '{', '}', '_', '$', '\t', 
	'\n', '\r', '#', 'q', 'v', 'h', 'p', 'd', 'g', 'z', 
	'"', 
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
			11, 34
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			29, 31, 40, 165, 178, 189, 198, 206, 213, 216,
			218, 314, 319, 355
		},
		.to_inline = {
			{ 14, 56, 14 },
			{ 30, 111, 30 },
			{ 155, 111, 30 },
			{ 157, 111, 150 },
			{ 159, 111, 30 },
			{ 161, 111, 30 },
			{ 180 },
			{ 187 },
			{ 187, 198 },
			{ 197 },
			{ 201 },
			{ 249 },
			{ 261 },
			{ 279 },
			{ 284 },
			{ 306 },
			{ 359 }
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
//G3:   __E_spec_multiline_0(16) => rec_relation(17).
	p(NT(16), (NT(17)));
//G4:   __E_spec_multiline_0(16) => input_def(18).
	p(NT(16), (NT(18)));
//G5:   __E_spec_multiline_0(16) => output_def(19).
	p(NT(16), (NT(19)));
//G6:   __E_spec_multiline_0(16) => main(13).
	p(NT(16), (NT(13)));
//G7:   __E_spec_multiline_1(20) => _(11) '.'.
	p(NT(20), (NT(11)+T(1)));
//G8:   __E_spec_multiline_1(20) => null.
	p(NT(20), (nul));
//G9:   spec_multiline(15)   => _(11) __E_spec_multiline_0(16) __E_spec_multiline_1(20) _(11).
	p(NT(15), (NT(11)+NT(16)+NT(20)+NT(11)));
//G10:  spec_multiline(15)   => _(11).
	p(NT(15), (NT(11)));
//G11:  __E___E_definitions_2_3(22) => rec_relation(17).
	p(NT(22), (NT(17)));
//G12:  __E___E_definitions_2_3(22) => input_def(18).
	p(NT(22), (NT(18)));
//G13:  __E___E_definitions_2_3(22) => output_def(19).
	p(NT(22), (NT(19)));
//G14:  __E_definitions_2(21) => _(11) __E___E_definitions_2_3(22) _(11) '.'.
	p(NT(21), (NT(11)+NT(22)+NT(11)+T(1)));
//G15:  __E_definitions_4(23) => null.
	p(NT(23), (nul));
//G16:  __E_definitions_4(23) => __E_definitions_2(21) __E_definitions_4(23).
	p(NT(23), (NT(21)+NT(23)));
//G17:  definitions(12)      => __E_definitions_4(23).
	p(NT(12), (NT(23)));
//G18:  __E_rec_relations_5(25) => _(11) rec_relation(17) _(11) '.'.
	p(NT(25), (NT(11)+NT(17)+NT(11)+T(1)));
//G19:  __E_rec_relations_6(26) => null.
	p(NT(26), (nul));
//G20:  __E_rec_relations_6(26) => __E_rec_relations_5(25) __E_rec_relations_6(26).
	p(NT(26), (NT(25)+NT(26)));
//G21:  rec_relations(24)    => __E_rec_relations_6(26).
	p(NT(24), (NT(26)));
//G22:  __E_rec_relation_7(28) => capture(29).
	p(NT(28), (NT(29)));
//G23:  __E_rec_relation_7(28) => ref(27).
	p(NT(28), (NT(27)));
//G24:  __E_rec_relation_7(28) => wff(14).
	p(NT(28), (NT(14)));
//G25:  __E_rec_relation_7(28) => bf(30).
	p(NT(28), (NT(30)));
//G26:  rec_relation(17)     => ref(27) _(11) ':' '=' _(11) __E_rec_relation_7(28).
	p(NT(17), (NT(27)+NT(11)+T(2)+T(3)+NT(11)+NT(28)));
//G27:  __E_input_def_8(32)  => typed(33).
	p(NT(32), (NT(33)));
//G28:  __E_input_def_8(32)  => null.
	p(NT(32), (nul));
//G29:  input_def(18)        => io_var_name(31) __E_input_def_8(32) _(11) '=' _(11) 'i' 'n' __(34) stream(35).
	p(NT(18), (NT(31)+NT(32)+NT(11)+T(3)+NT(11)+T(4)+T(5)+NT(34)+NT(35)));
//G30:  __E_output_def_9(36) => typed(33).
	p(NT(36), (NT(33)));
//G31:  __E_output_def_9(36) => null.
	p(NT(36), (nul));
//G32:  __E_output_def_10(37) => __(34) stream(35).
	p(NT(37), (NT(34)+NT(35)));
//G33:  __E_output_def_10(37) => null.
	p(NT(37), (nul));
//G34:  output_def(19)       => io_var_name(31) __E_output_def_9(36) _(11) '=' _(11) 'o' 'u' 't' __E_output_def_10(37).
	p(NT(19), (NT(31)+NT(36)+NT(11)+T(3)+NT(11)+T(6)+T(7)+T(8)+NT(37)));
//G35:  stream(35)           => 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(38) _(11) ')'.
	p(NT(35), (T(9)+T(4)+T(10)+T(11)+NT(11)+T(12)+NT(11)+NT(38)+NT(11)+T(13)));
//G36:  console_sym(39)      => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(39), (T(14)+T(6)+T(5)+T(15)+T(6)+T(10)+T(11)));
//G37:  stream(35)           => console_sym(39).
	p(NT(35), (NT(39)));
//G38:  __E_ref_11(41)       => offsets(42).
	p(NT(41), (NT(42)));
//G39:  __E_ref_11(41)       => null.
	p(NT(41), (nul));
//G40:  __E_ref_12(44)       => typed(33).
	p(NT(44), (NT(33)));
//G41:  __E_ref_12(44)       => null.
	p(NT(44), (nul));
//G42:  __E_ref_13(45)       => _(11) 'f' 'a' 'l' 'l' 'b' 'a' 'c' 'k' __(34) fp_fallback(46).
	p(NT(45), (NT(11)+T(9)+T(16)+T(10)+T(10)+T(17)+T(16)+T(14)+T(18)+NT(34)+NT(46)));
//G43:  __E_ref_13(45)       => null.
	p(NT(45), (nul));
//G44:  ref(27)              => sym(40) __E_ref_11(41) ref_args(43) __E_ref_12(44) __E_ref_13(45).
	p(NT(27), (NT(40)+NT(41)+NT(43)+NT(44)+NT(45)));
//G45:  first_sym(47)        => 'f' 'i' 'r' 's' 't'.
	p(NT(47), (T(9)+T(4)+T(19)+T(15)+T(8)));
//G46:  fp_fallback(46)      => first_sym(47).
	p(NT(46), (NT(47)));
//G47:  last_sym(48)         => 'l' 'a' 's' 't'.
	p(NT(48), (T(10)+T(16)+T(15)+T(8)));
//G48:  fp_fallback(46)      => last_sym(48).
	p(NT(46), (NT(48)));
//G49:  fp_fallback(46)      => capture(29).
	p(NT(46), (NT(29)));
//G50:  fp_fallback(46)      => ref(27).
	p(NT(46), (NT(27)));
//G51:  fp_fallback(46)      => wff(14).
	p(NT(46), (NT(14)));
//G52:  fp_fallback(46)      => bf(30).
	p(NT(46), (NT(30)));
//G53:  ref_arg(51)          => bf(30).
	p(NT(51), (NT(30)));
//G54:  __E___E_ref_args_14_15(50) => _(11) ref_arg(51).
	p(NT(50), (NT(11)+NT(51)));
//G55:  __E___E_ref_args_14_16(52) => _(11) ',' _(11) ref_arg(51).
	p(NT(52), (NT(11)+T(20)+NT(11)+NT(51)));
//G56:  __E___E_ref_args_14_17(53) => null.
	p(NT(53), (nul));
//G57:  __E___E_ref_args_14_17(53) => __E___E_ref_args_14_16(52) __E___E_ref_args_14_17(53).
	p(NT(53), (NT(52)+NT(53)));
//G58:  __E_ref_args_14(49)  => __E___E_ref_args_14_15(50) __E___E_ref_args_14_17(53).
	p(NT(49), (NT(50)+NT(53)));
//G59:  __E_ref_args_14(49)  => null.
	p(NT(49), (nul));
//G60:  ref_args(43)         => '(' __E_ref_args_14(49) _(11) ')'.
	p(NT(43), (T(12)+NT(49)+NT(11)+T(13)));
//G61:  __E_tau_constant_source_18(55) => '.' _(11).
	p(NT(55), (T(1)+NT(11)));
//G62:  __E_tau_constant_source_18(55) => null.
	p(NT(55), (nul));
//G63:  tau_constant_source(54) => rec_relations(24) _(11) main(13) _(11) __E_tau_constant_source_18(55).
	p(NT(54), (NT(24)+NT(11)+NT(13)+NT(11)+NT(55)));
//G64:  __E_wff_19(57)       => '(' _(11) wff(14) _(11) ')'.
	p(NT(57), (T(12)+NT(11)+NT(14)+NT(11)+T(13)));
//G65:  wff_parenthesis(56)  => __E_wff_19(57).
	p(NT(56), (NT(57)));
//G66:  wff(14)              => wff_parenthesis(56).
	p(NT(14), (NT(56)));
//G67:  __E___E_wff_20_21(60) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(60), (T(15)+T(6)+T(21)+T(11)+T(8)+T(4)+T(21)+T(11)+T(15)));
//G68:  __E___E_wff_20_21(60) => '<' '>'.
	p(NT(60), (T(22)+T(23)));
//G69:  __E_wff_20(59)       => __E___E_wff_20_21(60) _(11) wff(14).
	p(NT(59), (NT(60)+NT(11)+NT(14)));
//G70:  wff_sometimes(58)    => __E_wff_20(59).
	p(NT(58), (NT(59)));
//G71:  wff(14)              => wff_sometimes(58).
	p(NT(14), (NT(58)));
//G72:  __E___E_wff_22_23(63) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(63), (T(16)+T(10)+T(24)+T(16)+T(25)+T(15)));
//G73:  __E___E_wff_22_23(63) => '[' ']'.
	p(NT(63), (T(26)+T(27)));
//G74:  __E_wff_22(62)       => __E___E_wff_22_23(63) _(11) wff(14).
	p(NT(62), (NT(63)+NT(11)+NT(14)));
//G75:  wff_always(61)       => __E_wff_22(62).
	p(NT(61), (NT(62)));
//G76:  wff(14)              => wff_always(61).
	p(NT(14), (NT(61)));
//G77:  __E_wff_24(65)       => wff(14) _(11) '?' _(11) wff(14) _(11) ':' _(11) wff(14).
	p(NT(65), (NT(14)+NT(11)+T(28)+NT(11)+NT(14)+NT(11)+T(2)+NT(11)+NT(14)));
//G78:  wff_conditional(64)  => __E_wff_24(65).
	p(NT(64), (NT(65)));
//G79:  wff(14)              => wff_conditional(64).
	p(NT(14), (NT(64)));
//G80:  __E_wff_25(67)       => 'a' 'l' 'l' __(34) q_vars(68) __(34) wff(14).
	p(NT(67), (T(16)+T(10)+T(10)+NT(34)+NT(68)+NT(34)+NT(14)));
//G81:  wff_all(66)          => __E_wff_25(67).
	p(NT(66), (NT(67)));
//G82:  wff(14)              => wff_all(66).
	p(NT(14), (NT(66)));
//G83:  __E_wff_26(70)       => 'e' 'x' __(34) q_vars(68) __(34) wff(14).
	p(NT(70), (T(11)+T(29)+NT(34)+NT(68)+NT(34)+NT(14)));
//G84:  wff_ex(69)           => __E_wff_26(70).
	p(NT(69), (NT(70)));
//G85:  wff(14)              => wff_ex(69).
	p(NT(14), (NT(69)));
//G86:  wff_ref(71)          => ref(27).
	p(NT(71), (NT(27)));
//G87:  wff(14)              => wff_ref(71).
	p(NT(14), (NT(71)));
//G88:  wff(14)              => constraint(72).
	p(NT(14), (NT(72)));
//G89:  __E_wff_27(74)       => wff(14) _(11) '-' '>' _(11) wff(14).
	p(NT(74), (NT(14)+NT(11)+T(30)+T(23)+NT(11)+NT(14)));
//G90:  wff_imply(73)        => __E_wff_27(74).
	p(NT(73), (NT(74)));
//G91:  wff(14)              => wff_imply(73).
	p(NT(14), (NT(73)));
//G92:  __E_wff_28(76)       => wff(14) _(11) '<' '-' _(11) wff(14).
	p(NT(76), (NT(14)+NT(11)+T(22)+T(30)+NT(11)+NT(14)));
//G93:  wff_rimply(75)       => __E_wff_28(76).
	p(NT(75), (NT(76)));
//G94:  wff(14)              => wff_rimply(75).
	p(NT(14), (NT(75)));
//G95:  __E_wff_29(78)       => wff(14) _(11) '<' '-' '>' _(11) wff(14).
	p(NT(78), (NT(14)+NT(11)+T(22)+T(30)+T(23)+NT(11)+NT(14)));
//G96:  wff_equiv(77)        => __E_wff_29(78).
	p(NT(77), (NT(78)));
//G97:  wff(14)              => wff_equiv(77).
	p(NT(14), (NT(77)));
//G98:  __E_wff_30(80)       => wff(14) _(11) '|' '|' _(11) wff(14).
	p(NT(80), (NT(14)+NT(11)+T(31)+T(31)+NT(11)+NT(14)));
//G99:  wff_or(79)           => __E_wff_30(80).
	p(NT(79), (NT(80)));
//G100: wff(14)              => wff_or(79).
	p(NT(14), (NT(79)));
//G101: __E_wff_31(82)       => wff(14) _(11) '^' '^' _(11) wff(14).
	p(NT(82), (NT(14)+NT(11)+T(32)+T(32)+NT(11)+NT(14)));
//G102: wff_xor(81)          => __E_wff_31(82).
	p(NT(81), (NT(82)));
//G103: wff(14)              => wff_xor(81).
	p(NT(14), (NT(81)));
//G104: __E_wff_32(84)       => wff(14) _(11) '&' '&' _(11) wff(14).
	p(NT(84), (NT(14)+NT(11)+T(33)+T(33)+NT(11)+NT(14)));
//G105: wff_and(83)          => __E_wff_32(84).
	p(NT(83), (NT(84)));
//G106: wff(14)              => wff_and(83).
	p(NT(14), (NT(83)));
//G107: __E_wff_33(86)       => '!' _(11) wff(14).
	p(NT(86), (T(34)+NT(11)+NT(14)));
//G108: wff_neg(85)          => __E_wff_33(86).
	p(NT(85), (NT(86)));
//G109: wff(14)              => wff_neg(85).
	p(NT(14), (NT(85)));
//G110: wff_t(87)            => 'T'.
	p(NT(87), (T(35)));
//G111: wff(14)              => wff_t(87).
	p(NT(14), (NT(87)));
//G112: wff_f(88)            => 'F'.
	p(NT(88), (T(36)));
//G113: wff(14)              => wff_f(88).
	p(NT(14), (NT(88)));
//G114: __E_wff_34(90)       => bf(30) _(11) '<' '=' _(11) bf(30) _(11) '<' '=' _(11) bf(30).
	p(NT(90), (NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)));
//G115: bf_interval(89)      => __E_wff_34(90).
	p(NT(89), (NT(90)));
//G116: wff(14)              => bf_interval(89).
	p(NT(14), (NT(89)));
//G117: __E_wff_35(92)       => bf(30) _(11) '=' _(11) bf(30).
	p(NT(92), (NT(30)+NT(11)+T(3)+NT(11)+NT(30)));
//G118: bf_eq(91)            => __E_wff_35(92).
	p(NT(91), (NT(92)));
//G119: wff(14)              => bf_eq(91).
	p(NT(14), (NT(91)));
//G120: __E_wff_36(94)       => bf(30) _(11) '!' '=' _(11) bf(30).
	p(NT(94), (NT(30)+NT(11)+T(34)+T(3)+NT(11)+NT(30)));
//G121: bf_neq(93)           => __E_wff_36(94).
	p(NT(93), (NT(94)));
//G122: wff(14)              => bf_neq(93).
	p(NT(14), (NT(93)));
//G123: __E_wff_37(96)       => bf(30) _(11) '<' '=' _(11) bf(30).
	p(NT(96), (NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)));
//G124: bf_lteq(95)          => __E_wff_37(96).
	p(NT(95), (NT(96)));
//G125: wff(14)              => bf_lteq(95).
	p(NT(14), (NT(95)));
//G126: __E_wff_38(98)       => bf(30) _(11) '!' '<' '=' _(11) bf(30).
	p(NT(98), (NT(30)+NT(11)+T(34)+T(22)+T(3)+NT(11)+NT(30)));
//G127: bf_nlteq(97)         => __E_wff_38(98).
	p(NT(97), (NT(98)));
//G128: wff(14)              => bf_nlteq(97).
	p(NT(14), (NT(97)));
//G129: __E_wff_39(100)      => bf(30) _(11) '>' _(11) bf(30).
	p(NT(100), (NT(30)+NT(11)+T(23)+NT(11)+NT(30)));
//G130: bf_gt(99)            => __E_wff_39(100).
	p(NT(99), (NT(100)));
//G131: wff(14)              => bf_gt(99).
	p(NT(14), (NT(99)));
//G132: __E_wff_40(102)      => bf(30) _(11) '!' '>' _(11) bf(30).
	p(NT(102), (NT(30)+NT(11)+T(34)+T(23)+NT(11)+NT(30)));
//G133: bf_ngt(101)          => __E_wff_40(102).
	p(NT(101), (NT(102)));
//G134: wff(14)              => bf_ngt(101).
	p(NT(14), (NT(101)));
//G135: __E_wff_41(104)      => bf(30) _(11) '>' '=' _(11) bf(30).
	p(NT(104), (NT(30)+NT(11)+T(23)+T(3)+NT(11)+NT(30)));
//G136: bf_gteq(103)         => __E_wff_41(104).
	p(NT(103), (NT(104)));
//G137: wff(14)              => bf_gteq(103).
	p(NT(14), (NT(103)));
//G138: __E_wff_42(106)      => bf(30) _(11) '!' '>' '=' _(11) bf(30).
	p(NT(106), (NT(30)+NT(11)+T(34)+T(23)+T(3)+NT(11)+NT(30)));
//G139: bf_ngteq(105)        => __E_wff_42(106).
	p(NT(105), (NT(106)));
//G140: wff(14)              => bf_ngteq(105).
	p(NT(14), (NT(105)));
//G141: __E_wff_43(108)      => bf(30) _(11) '<' _(11) bf(30).
	p(NT(108), (NT(30)+NT(11)+T(22)+NT(11)+NT(30)));
//G142: bf_lt(107)           => __E_wff_43(108).
	p(NT(107), (NT(108)));
//G143: wff(14)              => bf_lt(107).
	p(NT(14), (NT(107)));
//G144: __E_wff_44(110)      => bf(30) _(11) '!' '<' _(11) bf(30).
	p(NT(110), (NT(30)+NT(11)+T(34)+T(22)+NT(11)+NT(30)));
//G145: bf_nlt(109)          => __E_wff_44(110).
	p(NT(109), (NT(110)));
//G146: wff(14)              => bf_nlt(109).
	p(NT(14), (NT(109)));
//G147: wff(14)              => capture(29).
	p(NT(14), (NT(29)));
//G148: __E_bf_45(112)       => '(' _(11) bf(30) _(11) ')'.
	p(NT(112), (T(12)+NT(11)+NT(30)+NT(11)+T(13)));
//G149: bf_parenthesis(111)  => __E_bf_45(112).
	p(NT(111), (NT(112)));
//G150: bf(30)               => bf_parenthesis(111).
	p(NT(30), (NT(111)));
//G151: bf(30)               => ba_constant(113).
	p(NT(30), (NT(113)));
//G152: bf(30)               => variable(114).
	p(NT(30), (NT(114)));
//G153: __E_bf_46(116)       => 'f' 'a' 'l' 'l' __(34) q_vars(68) __(34) bf(30).
	p(NT(116), (T(9)+T(16)+T(10)+T(10)+NT(34)+NT(68)+NT(34)+NT(30)));
//G154: bf_fall(115)         => __E_bf_46(116).
	p(NT(115), (NT(116)));
//G155: bf(30)               => bf_fall(115).
	p(NT(30), (NT(115)));
//G156: __E_bf_47(118)       => 'f' 'e' 'x' __(34) q_vars(68) __(34) bf(30).
	p(NT(118), (T(9)+T(11)+T(29)+NT(34)+NT(68)+NT(34)+NT(30)));
//G157: bf_fex(117)          => __E_bf_47(118).
	p(NT(117), (NT(118)));
//G158: bf(30)               => bf_fex(117).
	p(NT(30), (NT(117)));
//G159: bf_ref(119)          => ref(27).
	p(NT(119), (NT(27)));
//G160: bf(30)               => bf_ref(119).
	p(NT(30), (NT(119)));
//G161: __E_bf_48(121)       => bf(30) _(11) '>' '>' _(11) bf(30).
	p(NT(121), (NT(30)+NT(11)+T(23)+T(23)+NT(11)+NT(30)));
//G162: bf_shr(120)          => __E_bf_48(121).
	p(NT(120), (NT(121)));
//G163: bf(30)               => bf_shr(120).
	p(NT(30), (NT(120)));
//G164: __E_bf_49(123)       => bf(30) _(11) '<' '<' _(11) bf(30).
	p(NT(123), (NT(30)+NT(11)+T(22)+T(22)+NT(11)+NT(30)));
//G165: bf_shl(122)          => __E_bf_49(123).
	p(NT(122), (NT(123)));
//G166: bf(30)               => bf_shl(122).
	p(NT(30), (NT(122)));
//G167: __E_bf_50(125)       => bf(30) _(11) '!' '^' _(11) bf(30).
	p(NT(125), (NT(30)+NT(11)+T(34)+T(32)+NT(11)+NT(30)));
//G168: bf_xnor(124)         => __E_bf_50(125).
	p(NT(124), (NT(125)));
//G169: bf(30)               => bf_xnor(124).
	p(NT(30), (NT(124)));
//G170: __E_bf_51(127)       => bf(30) _(11) '!' '|' _(11) bf(30).
	p(NT(127), (NT(30)+NT(11)+T(34)+T(31)+NT(11)+NT(30)));
//G171: bf_nor(126)          => __E_bf_51(127).
	p(NT(126), (NT(127)));
//G172: bf(30)               => bf_nor(126).
	p(NT(30), (NT(126)));
//G173: __E_bf_52(129)       => bf(30) _(11) '!' '&' _(11) bf(30).
	p(NT(129), (NT(30)+NT(11)+T(34)+T(33)+NT(11)+NT(30)));
//G174: bf_nand(128)         => __E_bf_52(129).
	p(NT(128), (NT(129)));
//G175: bf(30)               => bf_nand(128).
	p(NT(30), (NT(128)));
//G176: __E_bf_53(131)       => bf(30) _(11) '-' _(11) bf(30).
	p(NT(131), (NT(30)+NT(11)+T(30)+NT(11)+NT(30)));
//G177: bf_sub(130)          => __E_bf_53(131).
	p(NT(130), (NT(131)));
//G178: bf(30)               => bf_sub(130).
	p(NT(30), (NT(130)));
//G179: __E_bf_54(133)       => bf(30) _(11) '+' _(11) bf(30).
	p(NT(133), (NT(30)+NT(11)+T(37)+NT(11)+NT(30)));
//G180: bf_add(132)          => __E_bf_54(133).
	p(NT(132), (NT(133)));
//G181: bf(30)               => bf_add(132).
	p(NT(30), (NT(132)));
//G182: __E_bf_55(135)       => bf(30) _(11) '%' _(11) bf(30).
	p(NT(135), (NT(30)+NT(11)+T(38)+NT(11)+NT(30)));
//G183: bf_mod(134)          => __E_bf_55(135).
	p(NT(134), (NT(135)));
//G184: bf(30)               => bf_mod(134).
	p(NT(30), (NT(134)));
//G185: __E_bf_56(137)       => bf(30) _(11) '/' _(11) bf(30).
	p(NT(137), (NT(30)+NT(11)+T(39)+NT(11)+NT(30)));
//G186: bf_div(136)          => __E_bf_56(137).
	p(NT(136), (NT(137)));
//G187: bf(30)               => bf_div(136).
	p(NT(30), (NT(136)));
//G188: __E_bf_57(139)       => bf(30) _(11) '*' _(11) bf(30).
	p(NT(139), (NT(30)+NT(11)+T(40)+NT(11)+NT(30)));
//G189: bf_mul(138)          => __E_bf_57(139).
	p(NT(138), (NT(139)));
//G190: bf(30)               => bf_mul(138).
	p(NT(30), (NT(138)));
//G191: __E_bf_58(141)       => bf(30) _(11) '|' _(11) bf(30).
	p(NT(141), (NT(30)+NT(11)+T(31)+NT(11)+NT(30)));
//G192: bf_or(140)           => __E_bf_58(141).
	p(NT(140), (NT(141)));
//G193: bf(30)               => bf_or(140).
	p(NT(30), (NT(140)));
//G194: __E_bf_59(143)       => bf(30) _(11) '^' _(11) bf(30).
	p(NT(143), (NT(30)+NT(11)+T(32)+NT(11)+NT(30)));
//G195: bf_xor(142)          => __E_bf_59(143).
	p(NT(142), (NT(143)));
//G196: bf(30)               => bf_xor(142).
	p(NT(30), (NT(142)));
//G197: __E___E_bf_60_61(146) => typed(33).
	p(NT(146), (NT(33)));
//G198: __E___E_bf_60_61(146) => null.
	p(NT(146), (nul));
//G199: __E_bf_60(145)       => '1' __E___E_bf_60_61(146).
	p(NT(145), (T(41)+NT(146)));
//G200: bf_t(144)            => __E_bf_60(145).
	p(NT(144), (NT(145)));
//G201: bf(30)               => bf_t(144).
	p(NT(30), (NT(144)));
//G202: __E___E_bf_62_63(149) => typed(33).
	p(NT(149), (NT(33)));
//G203: __E___E_bf_62_63(149) => null.
	p(NT(149), (nul));
//G204: __E_bf_62(148)       => '0' __E___E_bf_62_63(149).
	p(NT(148), (T(42)+NT(149)));
//G205: bf_f(147)            => __E_bf_62(148).
	p(NT(147), (NT(148)));
//G206: bf(30)               => bf_f(147).
	p(NT(30), (NT(147)));
//G207: __E___E_bf_64_65(152) => _(11) '&' _(11).
	p(NT(152), (NT(11)+T(33)+NT(11)));
//G208: __E___E_bf_64_65(152) => __(34).
	p(NT(152), (NT(34)));
//G209: __E_bf_64(151)       => bf(30) __E___E_bf_64_65(152) bf(30).
	p(NT(151), (NT(30)+NT(152)+NT(30)));
//G210: bf_and(150)          => __E_bf_64(151).
	p(NT(150), (NT(151)));
//G211: bf(30)               => bf_and(150).
	p(NT(30), (NT(150)));
//G212: __E___E_bf_66_67(156) => bf_parenthesis(111).
	p(NT(156), (NT(111)));
//G213: __E___E_bf_66_67(156) => ba_constant(113).
	p(NT(156), (NT(113)));
//G214: __E___E_bf_66_67(156) => variable(114).
	p(NT(156), (NT(114)));
//G215: __E___E_bf_66_67(156) => bf_ref(119).
	p(NT(156), (NT(119)));
//G216: __E___E_bf_66_67(156) => bf_t(144).
	p(NT(156), (NT(144)));
//G217: __E___E_bf_66_67(156) => bf_f(147).
	p(NT(156), (NT(147)));
//G218: __E___E_bf_66_67(156) => bf_neg(153).
	p(NT(156), (NT(153)));
//G219: __E___E_bf_66_67(156) => capture(29).
	p(NT(156), (NT(29)));
//G220: bf_neg_oprnd(155)    => __E___E_bf_66_67(156).
	p(NT(155), (NT(156)));
//G221: __E_bf_66(154)       => bf_neg_oprnd(155) _(11) '\''.
	p(NT(154), (NT(155)+NT(11)+T(43)));
//G222: bf_neg(153)          => __E_bf_66(154).
	p(NT(153), (NT(154)));
//G223: bf(30)               => bf_neg(153).
	p(NT(30), (NT(153)));
//G224: __E___E_bf_68_69(160) => bf_parenthesis(111).
	p(NT(160), (NT(111)));
//G225: __E___E_bf_68_69(160) => variable(114).
	p(NT(160), (NT(114)));
//G226: __E___E_bf_68_69(160) => bf_ref(119).
	p(NT(160), (NT(119)));
//G227: __E___E_bf_68_69(160) => bf_neg(153).
	p(NT(160), (NT(153)));
//G228: __E___E_bf_68_69(160) => bf_and_nosep(157).
	p(NT(160), (NT(157)));
//G229: __E___E_bf_68_69(160) => capture(29).
	p(NT(160), (NT(29)));
//G230: bf_and_nosep_1st_oprnd(159) => __E___E_bf_68_69(160).
	p(NT(159), (NT(160)));
//G231: __E___E_bf_68_70(162) => bf_parenthesis(111).
	p(NT(162), (NT(111)));
//G232: __E___E_bf_68_70(162) => ba_constant(113).
	p(NT(162), (NT(113)));
//G233: __E___E_bf_68_70(162) => variable(114).
	p(NT(162), (NT(114)));
//G234: __E___E_bf_68_70(162) => bf_ref(119).
	p(NT(162), (NT(119)));
//G235: __E___E_bf_68_70(162) => bf_neg(153).
	p(NT(162), (NT(153)));
//G236: __E___E_bf_68_70(162) => capture(29).
	p(NT(162), (NT(29)));
//G237: bf_and_nosep_2nd_oprnd(161) => __E___E_bf_68_70(162).
	p(NT(161), (NT(162)));
//G238: __E_bf_68(158)       => bf_and_nosep_1st_oprnd(159) bf_and_nosep_2nd_oprnd(161).
	p(NT(158), (NT(159)+NT(161)));
//G239: bf_and_nosep(157)    => __E_bf_68(158).
	p(NT(157), (NT(158)));
//G240: bf(30)               => bf_and_nosep(157).
	p(NT(30), (NT(157)));
//G241: bf(30)               => capture(29).
	p(NT(30), (NT(29)));
//G242: __E_constraint_71(164) => '[' ctnvar(165) _(11) '!' '=' _(11) num(166) ']'.
	p(NT(164), (T(26)+NT(165)+NT(11)+T(34)+T(3)+NT(11)+NT(166)+T(27)));
//G243: __E_constraint_71(164) => '[' num(166) _(11) '!' '=' _(11) ctnvar(165) ']'.
	p(NT(164), (T(26)+NT(166)+NT(11)+T(34)+T(3)+NT(11)+NT(165)+T(27)));
//G244: ctn_neq(163)         => __E_constraint_71(164).
	p(NT(163), (NT(164)));
//G245: constraint(72)       => ctn_neq(163).
	p(NT(72), (NT(163)));
//G246: __E_constraint_72(168) => '[' ctnvar(165) _(11) '=' _(11) num(166) ']'.
	p(NT(168), (T(26)+NT(165)+NT(11)+T(3)+NT(11)+NT(166)+T(27)));
//G247: __E_constraint_72(168) => '[' num(166) _(11) '=' _(11) ctnvar(165) ']'.
	p(NT(168), (T(26)+NT(166)+NT(11)+T(3)+NT(11)+NT(165)+T(27)));
//G248: ctn_eq(167)          => __E_constraint_72(168).
	p(NT(167), (NT(168)));
//G249: constraint(72)       => ctn_eq(167).
	p(NT(72), (NT(167)));
//G250: __E_constraint_73(170) => '[' ctnvar(165) _(11) '>' '=' _(11) num(166) ']'.
	p(NT(170), (T(26)+NT(165)+NT(11)+T(23)+T(3)+NT(11)+NT(166)+T(27)));
//G251: __E_constraint_73(170) => '[' num(166) _(11) '>' '=' _(11) ctnvar(165) ']'.
	p(NT(170), (T(26)+NT(166)+NT(11)+T(23)+T(3)+NT(11)+NT(165)+T(27)));
//G252: ctn_gteq(169)        => __E_constraint_73(170).
	p(NT(169), (NT(170)));
//G253: constraint(72)       => ctn_gteq(169).
	p(NT(72), (NT(169)));
//G254: __E_constraint_74(172) => '[' ctnvar(165) _(11) '>' _(11) num(166) ']'.
	p(NT(172), (T(26)+NT(165)+NT(11)+T(23)+NT(11)+NT(166)+T(27)));
//G255: __E_constraint_74(172) => '[' num(166) _(11) '>' _(11) ctnvar(165) ']'.
	p(NT(172), (T(26)+NT(166)+NT(11)+T(23)+NT(11)+NT(165)+T(27)));
//G256: ctn_gt(171)          => __E_constraint_74(172).
	p(NT(171), (NT(172)));
//G257: constraint(72)       => ctn_gt(171).
	p(NT(72), (NT(171)));
//G258: __E_constraint_75(174) => '[' ctnvar(165) _(11) '<' '=' _(11) num(166) ']'.
	p(NT(174), (T(26)+NT(165)+NT(11)+T(22)+T(3)+NT(11)+NT(166)+T(27)));
//G259: __E_constraint_75(174) => '[' num(166) _(11) '<' '=' _(11) ctnvar(165) ']'.
	p(NT(174), (T(26)+NT(166)+NT(11)+T(22)+T(3)+NT(11)+NT(165)+T(27)));
//G260: ctn_lteq(173)        => __E_constraint_75(174).
	p(NT(173), (NT(174)));
//G261: constraint(72)       => ctn_lteq(173).
	p(NT(72), (NT(173)));
//G262: __E_constraint_76(176) => '[' ctnvar(165) _(11) '<' _(11) num(166) ']'.
	p(NT(176), (T(26)+NT(165)+NT(11)+T(22)+NT(11)+NT(166)+T(27)));
//G263: __E_constraint_76(176) => '[' num(166) _(11) '<' _(11) ctnvar(165) ']'.
	p(NT(176), (T(26)+NT(166)+NT(11)+T(22)+NT(11)+NT(165)+T(27)));
//G264: ctn_lt(175)          => __E_constraint_76(176).
	p(NT(175), (NT(176)));
//G265: constraint(72)       => ctn_lt(175).
	p(NT(72), (NT(175)));
//G266: __E_ba_constant_77(177) => capture(29).
	p(NT(177), (NT(29)));
//G267: __E_ba_constant_77(177) => source(178).
	p(NT(177), (NT(178)));
//G268: __E_ba_constant_78(179) => typed(33).
	p(NT(179), (NT(33)));
//G269: __E_ba_constant_78(179) => null.
	p(NT(179), (nul));
//G270: ba_constant(113)     => '{' _(11) __E_ba_constant_77(177) _(11) '}' __E_ba_constant_78(179).
	p(NT(113), (T(44)+NT(11)+NT(177)+NT(11)+T(45)+NT(179)));
//G271: __E___E_source_79_80(182) => src_c(180).
	p(NT(182), (NT(180)));
//G272: __E___E_source_79_80(182) => space(2).
	p(NT(182), (NT(2)));
//G273: __E___E_source_79_81(183) => null.
	p(NT(183), (nul));
//G274: __E___E_source_79_81(183) => __E___E_source_79_80(182) __E___E_source_79_81(183).
	p(NT(183), (NT(182)+NT(183)));
//G275: __E_source_79(181)   => __E___E_source_79_81(183) src_c(180).
	p(NT(181), (NT(183)+NT(180)));
//G276: __E_source_79(181)   => null.
	p(NT(181), (nul));
//G277: source(178)          => src_c(180) __E_source_79(181).
	p(NT(178), (NT(180)+NT(181)));
//G278: src_c(180)           => alnum(6).
	p(NT(180), (NT(6)));
//G279: __N_0(374)           => '{'.
	p(NT(374), (T(44)));
//G280: __N_1(375)           => '}'.
	p(NT(375), (T(45)));
//G281: src_c(180)           => ~( __N_0(374) ) & ~( __N_1(375) ) & punct(7).	 # conjunctive
	p(NT(180), ~(NT(374)) & ~(NT(375)) & (NT(7)));
//G282: __E_src_c_82(184)    => src_c(180).
	p(NT(184), (NT(180)));
//G283: __E_src_c_82(184)    => space(2).
	p(NT(184), (NT(2)));
//G284: __E_src_c_83(185)    => null.
	p(NT(185), (nul));
//G285: __E_src_c_83(185)    => __E_src_c_82(184) __E_src_c_83(185).
	p(NT(185), (NT(184)+NT(185)));
//G286: src_c(180)           => '{' __E_src_c_83(185) '}'.
	p(NT(180), (T(44)+NT(185)+T(45)));
//G287: __E_variable_84(186) => uconst(187).
	p(NT(186), (NT(187)));
//G288: __E_variable_84(186) => io_var(188).
	p(NT(186), (NT(188)));
//G289: __E_variable_84(186) => var_name(189).
	p(NT(186), (NT(189)));
//G290: __E_variable_85(190) => typed(33).
	p(NT(190), (NT(33)));
//G291: __E_variable_85(190) => null.
	p(NT(190), (nul));
//G292: variable(114)        => __E_variable_84(186) __E_variable_85(190).
	p(NT(114), (NT(186)+NT(190)));
//G293: __N_2(376)           => 'F'.
	p(NT(376), (T(36)));
//G294: __N_3(377)           => 'T'.
	p(NT(377), (T(35)));
//G295: __E_var_name_86(191) => ~( __N_2(376) ) & ~( __N_3(377) ) & alpha(5).	 # conjunctive
	p(NT(191), ~(NT(376)) & ~(NT(377)) & (NT(5)));
//G296: __E_var_name_87(192) => null.
	p(NT(192), (nul));
//G297: __E_var_name_87(192) => digit(3) __E_var_name_87(192).
	p(NT(192), (NT(3)+NT(192)));
//G298: var_name(189)        => __E_var_name_86(191) __E_var_name_87(192).	 # guarded: charvar
	p(NT(189), (NT(191)+NT(192)));
	p.back().guard = "charvar";
//G299: __N_4(378)           => 'F'.
	p(NT(378), (T(36)));
//G300: __N_5(379)           => 'T'.
	p(NT(379), (T(35)));
//G301: __E_var_name_88(193) => ~( __N_4(378) ) & ~( __N_5(379) ) & alpha(5).	 # conjunctive
	p(NT(193), ~(NT(378)) & ~(NT(379)) & (NT(5)));
//G302: __E_var_name_89(194) => alnum(6).
	p(NT(194), (NT(6)));
//G303: __E_var_name_89(194) => '_'.
	p(NT(194), (T(46)));
//G304: __E_var_name_90(195) => null.
	p(NT(195), (nul));
//G305: __E_var_name_90(195) => __E_var_name_89(194) __E_var_name_90(195).
	p(NT(195), (NT(194)+NT(195)));
//G306: var_name(189)        => __E_var_name_88(193) __E_var_name_90(195).	 # guarded: var
	p(NT(189), (NT(193)+NT(195)));
	p.back().guard = "var";
//G307: io_var(188)          => io_var_name(31) '[' offset(196) ']'.
	p(NT(188), (NT(31)+T(26)+NT(196)+T(27)));
//G308: io_var_name(31)      => chars(197).
	p(NT(31), (NT(197)));
//G309: __E___E_uconst_91_92(200) => chars(197) _(11).
	p(NT(200), (NT(197)+NT(11)));
//G310: __E___E_uconst_91_92(200) => null.
	p(NT(200), (nul));
//G311: __E_uconst_91(199)   => __E___E_uconst_91_92(200) ':' _(11) chars(197).
	p(NT(199), (NT(200)+T(2)+NT(11)+NT(197)));
//G312: uconst_name(198)     => __E_uconst_91(199).
	p(NT(198), (NT(199)));
//G313: uconst(187)          => '<' _(11) uconst_name(198) _(11) '>'.
	p(NT(187), (T(22)+NT(11)+NT(198)+NT(11)+T(23)));
//G314: __E_q_vars_93(202)   => _(11) ',' _(11) q_var(201).
	p(NT(202), (NT(11)+T(20)+NT(11)+NT(201)));
//G315: __E_q_vars_94(203)   => null.
	p(NT(203), (nul));
//G316: __E_q_vars_94(203)   => __E_q_vars_93(202) __E_q_vars_94(203).
	p(NT(203), (NT(202)+NT(203)));
//G317: q_vars(68)           => q_var(201) __E_q_vars_94(203).
	p(NT(68), (NT(201)+NT(203)));
//G318: q_var(201)           => capture(29).
	p(NT(201), (NT(29)));
//G319: __N_6(380)           => uconst(187).
	p(NT(380), (NT(187)));
//G320: q_var(201)           => variable(114) & ~( __N_6(380) ).	 # conjunctive
	p(NT(201), (NT(114)) & ~(NT(380)));
//G321: ctnvar(165)          => chars(197).
	p(NT(165), (NT(197)));
//G322: __E_offsets_95(204)  => _(11) ',' _(11) offset(196).
	p(NT(204), (NT(11)+T(20)+NT(11)+NT(196)));
//G323: __E_offsets_96(205)  => null.
	p(NT(205), (nul));
//G324: __E_offsets_96(205)  => __E_offsets_95(204) __E_offsets_96(205).
	p(NT(205), (NT(204)+NT(205)));
//G325: offsets(42)          => '[' _(11) offset(196) __E_offsets_96(205) _(11) ']'.
	p(NT(42), (T(26)+NT(11)+NT(196)+NT(205)+NT(11)+T(27)));
//G326: offset(196)          => integer(206).
	p(NT(196), (NT(206)));
//G327: offset(196)          => capture(29).
	p(NT(196), (NT(29)));
//G328: offset(196)          => shift(207).
	p(NT(196), (NT(207)));
//G329: __N_7(381)           => io_var(188).
	p(NT(381), (NT(188)));
//G330: __E_offset_97(208)   => variable(114) & ~( __N_7(381) ).	 # conjunctive
	p(NT(208), (NT(114)) & ~(NT(381)));
//G331: offset(196)          => __E_offset_97(208).
	p(NT(196), (NT(208)));
//G332: __E_shift_98(209)    => capture(29).
	p(NT(209), (NT(29)));
//G333: __N_8(382)           => io_var(188).
	p(NT(382), (NT(188)));
//G334: __E___E_shift_98_99(210) => variable(114) & ~( __N_8(382) ).	 # conjunctive
	p(NT(210), (NT(114)) & ~(NT(382)));
//G335: __E_shift_98(209)    => __E___E_shift_98_99(210).
	p(NT(209), (NT(210)));
//G336: shift(207)           => __E_shift_98(209) _(11) '-' _(11) num(166).
	p(NT(207), (NT(209)+NT(11)+T(30)+NT(11)+NT(166)));
//G337: __E_chars_100(211)   => alnum(6).
	p(NT(211), (NT(6)));
//G338: __E_chars_100(211)   => '_'.
	p(NT(211), (T(46)));
//G339: __E_chars_101(212)   => null.
	p(NT(212), (nul));
//G340: __E_chars_101(212)   => __E_chars_100(211) __E_chars_101(212).
	p(NT(212), (NT(211)+NT(212)));
//G341: chars(197)           => alpha(5) __E_chars_101(212).
	p(NT(197), (NT(5)+NT(212)));
//G342: __E_digits_102(214)  => digit(3).
	p(NT(214), (NT(3)));
//G343: __E_digits_102(214)  => digit(3) __E_digits_102(214).
	p(NT(214), (NT(3)+NT(214)));
//G344: digits(213)          => __E_digits_102(214).
	p(NT(213), (NT(214)));
//G345: num(166)             => digits(213).
	p(NT(166), (NT(213)));
//G346: __E_integer_103(215) => '-'.
	p(NT(215), (T(30)));
//G347: __E_integer_103(215) => null.
	p(NT(215), (nul));
//G348: integer(206)         => __E_integer_103(215) _(11) digits(213).
	p(NT(206), (NT(215)+NT(11)+NT(213)));
//G349: sym(40)              => chars(197).
	p(NT(40), (NT(197)));
//G350: capture(29)          => '$' chars(197).
	p(NT(29), (T(47)+NT(197)));
//G351: __E_typed_104(217)   => _(11) '[' _(11) subtype(218) _(11) ']' _(11).
	p(NT(217), (NT(11)+T(26)+NT(11)+NT(218)+NT(11)+T(27)+NT(11)));
//G352: __E_typed_104(217)   => null.
	p(NT(217), (nul));
//G353: typed(33)            => _(11) ':' _(11) type(216) __E_typed_104(217).
	p(NT(33), (NT(11)+T(2)+NT(11)+NT(216)+NT(217)));
//G354: type(216)            => chars(197).
	p(NT(216), (NT(197)));
//G355: subtype(218)         => num(166).
	p(NT(218), (NT(166)));
//G356: __E_comment_105(220) => printable(8).
	p(NT(220), (NT(8)));
//G357: __E_comment_105(220) => '\t'.
	p(NT(220), (T(48)));
//G358: __E_comment_106(221) => null.
	p(NT(221), (nul));
//G359: __E_comment_106(221) => __E_comment_105(220) __E_comment_106(221).
	p(NT(221), (NT(220)+NT(221)));
//G360: __E_comment_107(222) => '\n'.
	p(NT(222), (T(49)));
//G361: __E_comment_107(222) => '\r'.
	p(NT(222), (T(50)));
//G362: __E_comment_107(222) => eof(1).
	p(NT(222), (NT(1)));
//G363: comment(219)         => '#' __E_comment_106(221) __E_comment_107(222).
	p(NT(219), (T(51)+NT(221)+NT(222)));
//G364: __(34)               => space(2).
	p(NT(34), (NT(2)));
//G365: __(34)               => comment(219).
	p(NT(34), (NT(219)));
//G366: __(34)               => __(34) space(2).
	p(NT(34), (NT(34)+NT(2)));
//G367: __(34)               => __(34) comment(219).
	p(NT(34), (NT(34)+NT(219)));
//G368: _(11)                => __(34).
	p(NT(11), (NT(34)));
//G369: _(11)                => null.
	p(NT(11), (nul));
//G370: cli(223)             => _(11).
	p(NT(223), (NT(11)));
//G371: __E_cli_108(225)     => '.' _(11) cli_command(224) _(11).
	p(NT(225), (T(1)+NT(11)+NT(224)+NT(11)));
//G372: __E_cli_109(226)     => null.
	p(NT(226), (nul));
//G373: __E_cli_109(226)     => __E_cli_108(225) __E_cli_109(226).
	p(NT(226), (NT(225)+NT(226)));
//G374: cli(223)             => _(11) cli_command(224) _(11) __E_cli_109(226).
	p(NT(223), (NT(11)+NT(224)+NT(11)+NT(226)));
//G375: __E_cli_command_110(229) => 'q'.
	p(NT(229), (T(52)));
//G376: __E_cli_command_110(229) => 'q' 'u' 'i' 't'.
	p(NT(229), (T(52)+T(7)+T(4)+T(8)));
//G377: quit_sym(228)        => __E_cli_command_110(229).
	p(NT(228), (NT(229)));
//G378: quit_cmd(227)        => quit_sym(228).
	p(NT(227), (NT(228)));
//G379: cli_command(224)     => quit_cmd(227).
	p(NT(224), (NT(227)));
//G380: __E_cli_command_111(232) => 'v'.
	p(NT(232), (T(53)));
//G381: __E_cli_command_111(232) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(232), (T(53)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G382: version_sym(231)     => __E_cli_command_111(232).
	p(NT(231), (NT(232)));
//G383: version_cmd(230)     => version_sym(231).
	p(NT(230), (NT(231)));
//G384: cli_command(224)     => version_cmd(230).
	p(NT(224), (NT(230)));
//G385: __E_cli_command_112(235) => 'c'.
	p(NT(235), (T(14)));
//G386: __E_cli_command_112(235) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(235), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G387: clear_sym(234)       => __E_cli_command_112(235).
	p(NT(234), (NT(235)));
//G388: clear_cmd(233)       => clear_sym(234).
	p(NT(233), (NT(234)));
//G389: cli_command(224)     => clear_cmd(233).
	p(NT(224), (NT(233)));
//G390: __E___E_cli_command_113_114(239) => 'h'.
	p(NT(239), (T(54)));
//G391: __E___E_cli_command_113_114(239) => 'h' 'e' 'l' 'p'.
	p(NT(239), (T(54)+T(11)+T(10)+T(55)));
//G392: help_sym(238)        => __E___E_cli_command_113_114(239).
	p(NT(238), (NT(239)));
//G393: __E___E_cli_command_113_115(240) => __(34) help_arg(241).
	p(NT(240), (NT(34)+NT(241)));
//G394: __E___E_cli_command_113_115(240) => null.
	p(NT(240), (nul));
//G395: __E_cli_command_113(237) => help_sym(238) __E___E_cli_command_113_115(240).
	p(NT(237), (NT(238)+NT(240)));
//G396: help_cmd(236)        => __E_cli_command_113(237).
	p(NT(236), (NT(237)));
//G397: cli_command(224)     => help_cmd(236).
	p(NT(224), (NT(236)));
//G398: file_sym(244)        => 'f' 'i' 'l' 'e'.
	p(NT(244), (T(9)+T(4)+T(10)+T(11)));
//G399: __E_cli_command_116(243) => file_sym(244) __(34) q_string(245).
	p(NT(243), (NT(244)+NT(34)+NT(245)));
//G400: file_cmd(242)        => __E_cli_command_116(243).
	p(NT(242), (NT(243)));
//G401: cli_command(224)     => file_cmd(242).
	p(NT(224), (NT(242)));
//G402: valid_sym(248)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(248), (T(53)+T(16)+T(10)+T(4)+T(56)));
//G403: __E_cli_command_117(247) => valid_sym(248) __(34) normalize_cmd_arg(249).
	p(NT(247), (NT(248)+NT(34)+NT(249)));
//G404: valid_cmd(246)       => __E_cli_command_117(247).
	p(NT(246), (NT(247)));
//G405: cli_command(224)     => valid_cmd(246).
	p(NT(224), (NT(246)));
//G406: sat_sym(252)         => 's' 'a' 't'.
	p(NT(252), (T(15)+T(16)+T(8)));
//G407: __E_cli_command_118(251) => sat_sym(252) __(34) normalize_cmd_arg(249).
	p(NT(251), (NT(252)+NT(34)+NT(249)));
//G408: sat_cmd(250)         => __E_cli_command_118(251).
	p(NT(250), (NT(251)));
//G409: cli_command(224)     => sat_cmd(250).
	p(NT(224), (NT(250)));
//G410: unsat_sym(255)       => 'u' 'n' 's' 'a' 't'.
	p(NT(255), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G411: __E_cli_command_119(254) => unsat_sym(255) __(34) normalize_cmd_arg(249).
	p(NT(254), (NT(255)+NT(34)+NT(249)));
//G412: unsat_cmd(253)       => __E_cli_command_119(254).
	p(NT(253), (NT(254)));
//G413: cli_command(224)     => unsat_cmd(253).
	p(NT(224), (NT(253)));
//G414: solve_sym(258)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(258), (T(15)+T(6)+T(10)+T(53)+T(11)));
//G415: __E___E_cli_command_120_121(259) => solve_options(260).
	p(NT(259), (NT(260)));
//G416: __E___E_cli_command_120_121(259) => null.
	p(NT(259), (nul));
//G417: __E_cli_command_120(257) => solve_sym(258) __E___E_cli_command_120_121(259) __(34) wff_cmd_arg(261).
	p(NT(257), (NT(258)+NT(259)+NT(34)+NT(261)));
//G418: solve_cmd(256)       => __E_cli_command_120(257).
	p(NT(256), (NT(257)));
//G419: cli_command(224)     => solve_cmd(256).
	p(NT(224), (NT(256)));
//G420: lgrs_sym(264)        => 'l' 'g' 'r' 's'.
	p(NT(264), (T(10)+T(57)+T(19)+T(15)));
//G421: __E_cli_command_122(263) => lgrs_sym(264) __(34) wff_cmd_arg(261).
	p(NT(263), (NT(264)+NT(34)+NT(261)));
//G422: lgrs_cmd(262)        => __E_cli_command_122(263).
	p(NT(262), (NT(263)));
//G423: cli_command(224)     => lgrs_cmd(262).
	p(NT(224), (NT(262)));
//G424: __E___E_cli_command_123_124(268) => 'r'.
	p(NT(268), (T(19)));
//G425: __E___E_cli_command_123_124(268) => 'r' 'u' 'n'.
	p(NT(268), (T(19)+T(7)+T(5)));
//G426: run_sym(267)         => __E___E_cli_command_123_124(268).
	p(NT(267), (NT(268)));
//G427: __E___E_cli_command_123_125(269) => history(270).
	p(NT(269), (NT(270)));
//G428: __E___E_cli_command_123_125(269) => wff(14).
	p(NT(269), (NT(14)));
//G429: __E_cli_command_123(266) => run_sym(267) __(34) __E___E_cli_command_123_125(269).
	p(NT(266), (NT(267)+NT(34)+NT(269)));
//G430: run_cmd(265)         => __E_cli_command_123(266).
	p(NT(265), (NT(266)));
//G431: cli_command(224)     => run_cmd(265).
	p(NT(224), (NT(265)));
//G432: __E___E_cli_command_126_127(274) => 'n'.
	p(NT(274), (T(5)));
//G433: __E___E_cli_command_126_127(274) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(274), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(58)+T(11)));
//G434: normalize_sym(273)   => __E___E_cli_command_126_127(274).
	p(NT(273), (NT(274)));
//G435: __E_cli_command_126(272) => normalize_sym(273) __(34) normalize_cmd_arg(249).
	p(NT(272), (NT(273)+NT(34)+NT(249)));
//G436: normalize_cmd(271)   => __E_cli_command_126(272).
	p(NT(271), (NT(272)));
//G437: cli_command(224)     => normalize_cmd(271).
	p(NT(224), (NT(271)));
//G438: __E___E_cli_command_128_129(278) => 's'.
	p(NT(278), (T(15)));
//G439: __E___E_cli_command_128_129(278) => 's' 'u' 'b' 's' 't'.
	p(NT(278), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G440: __E___E_cli_command_128_129(278) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(278), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G441: subst_sym(277)       => __E___E_cli_command_128_129(278).
	p(NT(277), (NT(278)));
//G442: __E_cli_command_128(276) => subst_sym(277) __(34) nf_cmd_arg(279) _(11) '[' _(11) nf_cmd_arg(279) _(11) '/' _(11) nf_cmd_arg(279) _(11) ']'.
	p(NT(276), (NT(277)+NT(34)+NT(279)+NT(11)+T(26)+NT(11)+NT(279)+NT(11)+T(39)+NT(11)+NT(279)+NT(11)+T(27)));
//G443: subst_cmd(275)       => __E_cli_command_128(276).
	p(NT(275), (NT(276)));
//G444: cli_command(224)     => subst_cmd(275).
	p(NT(224), (NT(275)));
//G445: __E___E_cli_command_130_131(283) => 'i'.
	p(NT(283), (T(4)));
//G446: __E___E_cli_command_130_131(283) => 'i' 'n' 's' 't'.
	p(NT(283), (T(4)+T(5)+T(15)+T(8)));
//G447: __E___E_cli_command_130_131(283) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(283), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G448: inst_sym(282)        => __E___E_cli_command_130_131(283).
	p(NT(282), (NT(283)));
//G449: __E_cli_command_130(281) => inst_sym(282) __(34) inst_args(284).
	p(NT(281), (NT(282)+NT(34)+NT(284)));
//G450: inst_cmd(280)        => __E_cli_command_130(281).
	p(NT(280), (NT(281)));
//G451: cli_command(224)     => inst_cmd(280).
	p(NT(224), (NT(280)));
//G452: dnf_sym(287)         => 'd' 'n' 'f'.
	p(NT(287), (T(56)+T(5)+T(9)));
//G453: __E_cli_command_132(286) => dnf_sym(287) __(34) nf_cmd_arg(279).
	p(NT(286), (NT(287)+NT(34)+NT(279)));
//G454: dnf_cmd(285)         => __E_cli_command_132(286).
	p(NT(285), (NT(286)));
//G455: cli_command(224)     => dnf_cmd(285).
	p(NT(224), (NT(285)));
//G456: cnf_sym(290)         => 'c' 'n' 'f'.
	p(NT(290), (T(14)+T(5)+T(9)));
//G457: __E_cli_command_133(289) => cnf_sym(290) __(34) nf_cmd_arg(279).
	p(NT(289), (NT(290)+NT(34)+NT(279)));
//G458: cnf_cmd(288)         => __E_cli_command_133(289).
	p(NT(288), (NT(289)));
//G459: cli_command(224)     => cnf_cmd(288).
	p(NT(224), (NT(288)));
//G460: anf_sym(293)         => 'a' 'n' 'f'.
	p(NT(293), (T(16)+T(5)+T(9)));
//G461: __E_cli_command_134(292) => anf_sym(293) __(34) nf_cmd_arg(279).
	p(NT(292), (NT(293)+NT(34)+NT(279)));
//G462: anf_cmd(291)         => __E_cli_command_134(292).
	p(NT(291), (NT(292)));
//G463: cli_command(224)     => anf_cmd(291).
	p(NT(224), (NT(291)));
//G464: nnf_sym(296)         => 'n' 'n' 'f'.
	p(NT(296), (T(5)+T(5)+T(9)));
//G465: __E_cli_command_135(295) => nnf_sym(296) __(34) nf_cmd_arg(279).
	p(NT(295), (NT(296)+NT(34)+NT(279)));
//G466: nnf_cmd(294)         => __E_cli_command_135(295).
	p(NT(294), (NT(295)));
//G467: cli_command(224)     => nnf_cmd(294).
	p(NT(224), (NT(294)));
//G468: pnf_sym(299)         => 'p' 'n' 'f'.
	p(NT(299), (T(55)+T(5)+T(9)));
//G469: __E_cli_command_136(298) => pnf_sym(299) __(34) nf_cmd_arg(279).
	p(NT(298), (NT(299)+NT(34)+NT(279)));
//G470: pnf_cmd(297)         => __E_cli_command_136(298).
	p(NT(297), (NT(298)));
//G471: cli_command(224)     => pnf_cmd(297).
	p(NT(224), (NT(297)));
//G472: mnf_sym(302)         => 'm' 'n' 'f'.
	p(NT(302), (T(21)+T(5)+T(9)));
//G473: __E_cli_command_137(301) => mnf_sym(302) __(34) nf_cmd_arg(279).
	p(NT(301), (NT(302)+NT(34)+NT(279)));
//G474: mnf_cmd(300)         => __E_cli_command_137(301).
	p(NT(300), (NT(301)));
//G475: cli_command(224)     => mnf_cmd(300).
	p(NT(224), (NT(300)));
//G476: onf_sym(305)         => 'o' 'n' 'f'.
	p(NT(305), (T(6)+T(5)+T(9)));
//G477: __E_cli_command_138(304) => onf_sym(305) __(34) variable(114) __(34) onf_cmd_arg(306).
	p(NT(304), (NT(305)+NT(34)+NT(114)+NT(34)+NT(306)));
//G478: onf_cmd(303)         => __E_cli_command_138(304).
	p(NT(303), (NT(304)));
//G479: cli_command(224)     => onf_cmd(303).
	p(NT(224), (NT(303)));
//G480: qelim_sym(309)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(309), (T(52)+T(11)+T(10)+T(4)+T(21)));
//G481: __E_cli_command_139(308) => qelim_sym(309) __(34) wff_cmd_arg(261).
	p(NT(308), (NT(309)+NT(34)+NT(261)));
//G482: qelim_cmd(307)       => __E_cli_command_139(308).
	p(NT(307), (NT(308)));
//G483: cli_command(224)     => qelim_cmd(307).
	p(NT(224), (NT(307)));
//G484: get_sym(312)         => 'g' 'e' 't'.
	p(NT(312), (T(57)+T(11)+T(8)));
//G485: __E___E_cli_command_140_141(313) => __(34) option_name(314).
	p(NT(313), (NT(34)+NT(314)));
//G486: __E___E_cli_command_140_141(313) => null.
	p(NT(313), (nul));
//G487: __E_cli_command_140(311) => get_sym(312) __E___E_cli_command_140_141(313).
	p(NT(311), (NT(312)+NT(313)));
//G488: get_cmd(310)         => __E_cli_command_140(311).
	p(NT(310), (NT(311)));
//G489: cli_command(224)     => get_cmd(310).
	p(NT(224), (NT(310)));
//G490: set_sym(317)         => 's' 'e' 't'.
	p(NT(317), (T(15)+T(11)+T(8)));
//G491: __E___E_cli_command_142_143(318) => __(34).
	p(NT(318), (NT(34)));
//G492: __E___E_cli_command_142_143(318) => _(11) '=' _(11).
	p(NT(318), (NT(11)+T(3)+NT(11)));
//G493: __E_cli_command_142(316) => set_sym(317) __(34) option_name(314) __E___E_cli_command_142_143(318) option_value(319).
	p(NT(316), (NT(317)+NT(34)+NT(314)+NT(318)+NT(319)));
//G494: set_cmd(315)         => __E_cli_command_142(316).
	p(NT(315), (NT(316)));
//G495: cli_command(224)     => set_cmd(315).
	p(NT(224), (NT(315)));
//G496: enable_sym(322)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(322), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G497: __E_cli_command_144(321) => enable_sym(322) __(34) option_name(314).
	p(NT(321), (NT(322)+NT(34)+NT(314)));
//G498: enable_cmd(320)      => __E_cli_command_144(321).
	p(NT(320), (NT(321)));
//G499: cli_command(224)     => enable_cmd(320).
	p(NT(224), (NT(320)));
//G500: disable_sym(325)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(325), (T(56)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G501: __E_cli_command_145(324) => disable_sym(325) __(34) option_name(314).
	p(NT(324), (NT(325)+NT(34)+NT(314)));
//G502: disable_cmd(323)     => __E_cli_command_145(324).
	p(NT(323), (NT(324)));
//G503: cli_command(224)     => disable_cmd(323).
	p(NT(224), (NT(323)));
//G504: toggle_sym(328)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(328), (T(8)+T(6)+T(57)+T(57)+T(10)+T(11)));
//G505: __E_cli_command_146(327) => toggle_sym(328) __(34) option_name(314).
	p(NT(327), (NT(328)+NT(34)+NT(314)));
//G506: toggle_cmd(326)      => __E_cli_command_146(327).
	p(NT(326), (NT(327)));
//G507: cli_command(224)     => toggle_cmd(326).
	p(NT(224), (NT(326)));
//G508: __E___E_cli_command_147_148(332) => 'd' 'e' 'f' 's'.
	p(NT(332), (T(56)+T(11)+T(9)+T(15)));
//G509: __E___E_cli_command_147_148(332) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(332), (T(56)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G510: def_sym(331)         => __E___E_cli_command_147_148(332).
	p(NT(331), (NT(332)));
//G511: __E_cli_command_147(330) => def_sym(331).
	p(NT(330), (NT(331)));
//G512: def_list_cmd(329)    => __E_cli_command_147(330).
	p(NT(329), (NT(330)));
//G513: cli_command(224)     => def_list_cmd(329).
	p(NT(224), (NT(329)));
//G514: __E_cli_command_149(334) => def_sym(331) __(34) num(166).
	p(NT(334), (NT(331)+NT(34)+NT(166)));
//G515: def_print_cmd(333)   => __E_cli_command_149(334).
	p(NT(333), (NT(334)));
//G516: cli_command(224)     => def_print_cmd(333).
	p(NT(224), (NT(333)));
//G517: def_rr_cmd(335)      => rec_relation(17).
	p(NT(335), (NT(17)));
//G518: cli_command(224)     => def_rr_cmd(335).
	p(NT(224), (NT(335)));
//G519: def_input_cmd(336)   => input_def(18).
	p(NT(336), (NT(18)));
//G520: cli_command(224)     => def_input_cmd(336).
	p(NT(224), (NT(336)));
//G521: def_output_cmd(337)  => output_def(19).
	p(NT(337), (NT(19)));
//G522: cli_command(224)     => def_output_cmd(337).
	p(NT(224), (NT(337)));
//G523: __E___E_cli_command_150_151(341) => 'h' 'i' 's' 't'.
	p(NT(341), (T(54)+T(4)+T(15)+T(8)));
//G524: __E___E_cli_command_150_151(341) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(341), (T(54)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G525: history_sym(340)     => __E___E_cli_command_150_151(341).
	p(NT(340), (NT(341)));
//G526: __E_cli_command_150(339) => history_sym(340).
	p(NT(339), (NT(340)));
//G527: history_list_cmd(338) => __E_cli_command_150(339).
	p(NT(338), (NT(339)));
//G528: cli_command(224)     => history_list_cmd(338).
	p(NT(224), (NT(338)));
//G529: __E_cli_command_152(343) => history_sym(340) __(34) history(270).
	p(NT(343), (NT(340)+NT(34)+NT(270)));
//G530: history_print_cmd(342) => __E_cli_command_152(343).
	p(NT(342), (NT(343)));
//G531: cli_command(224)     => history_print_cmd(342).
	p(NT(224), (NT(342)));
//G532: __E_cli_command_153(345) => wff(14).
	p(NT(345), (NT(14)));
//G533: __E_cli_command_153(345) => bf(30).
	p(NT(345), (NT(30)));
//G534: history_store_cmd(344) => __E_cli_command_153(345).
	p(NT(344), (NT(345)));
//G535: cli_command(224)     => history_store_cmd(344).
	p(NT(224), (NT(344)));
//G536: __E_solve_options_154(346) => __(34) solve_option(347).
	p(NT(346), (NT(34)+NT(347)));
//G537: __E_solve_options_155(348) => null.
	p(NT(348), (nul));
//G538: __E_solve_options_155(348) => __E_solve_options_154(346) __E_solve_options_155(348).
	p(NT(348), (NT(346)+NT(348)));
//G539: solve_options(260)   => __E_solve_options_155(348).
	p(NT(260), (NT(348)));
//G540: __E_solve_option_156(349) => solver_mode(350).
	p(NT(349), (NT(350)));
//G541: __E_solve_option_156(349) => type(216).
	p(NT(349), (NT(216)));
//G542: solve_option(347)    => '-' '-' __E_solve_option_156(349).
	p(NT(347), (T(30)+T(30)+NT(349)));
//G543: __E_solver_mode_157(352) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(352), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G544: __E_solver_mode_157(352) => 'm' 'i' 'n'.
	p(NT(352), (T(21)+T(4)+T(5)));
//G545: solver_mode_minimum(351) => __E_solver_mode_157(352).
	p(NT(351), (NT(352)));
//G546: solver_mode(350)     => solver_mode_minimum(351).
	p(NT(350), (NT(351)));
//G547: __E_solver_mode_158(354) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(354), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G548: __E_solver_mode_158(354) => 'm' 'a' 'x'.
	p(NT(354), (T(21)+T(16)+T(29)));
//G549: solver_mode_maximum(353) => __E_solver_mode_158(354).
	p(NT(353), (NT(354)));
//G550: solver_mode(350)     => solver_mode_maximum(353).
	p(NT(350), (NT(353)));
//G551: q_file_name(38)      => '"' file_name(355) '"'.
	p(NT(38), (T(59)+NT(355)+T(59)));
//G552: __E_file_name_159(356) => printable(8).
	p(NT(356), (NT(8)));
//G553: __E_file_name_159(356) => printable(8) __E_file_name_159(356).
	p(NT(356), (NT(8)+NT(356)));
//G554: file_name(355)       => __E_file_name_159(356).
	p(NT(355), (NT(356)));
//G555: __E_option_name_160(357) => alnum(6).
	p(NT(357), (NT(6)));
//G556: __E_option_name_160(357) => alnum(6) __E_option_name_160(357).
	p(NT(357), (NT(6)+NT(357)));
//G557: option_name(314)     => __E_option_name_160(357).
	p(NT(314), (NT(357)));
//G558: __E_option_value_161(358) => alnum(6).
	p(NT(358), (NT(6)));
//G559: __E_option_value_161(358) => alnum(6) __E_option_value_161(358).
	p(NT(358), (NT(6)+NT(358)));
//G560: option_value(319)    => __E_option_value_161(358).
	p(NT(319), (NT(358)));
//G561: bf_cmd_arg(359)      => history(270).
	p(NT(359), (NT(270)));
//G562: bf_cmd_arg(359)      => bf(30).
	p(NT(359), (NT(30)));
//G563: wff_cmd_arg(261)     => history(270).
	p(NT(261), (NT(270)));
//G564: wff_cmd_arg(261)     => wff(14).
	p(NT(261), (NT(14)));
//G565: nf_cmd_arg(279)      => history(270).
	p(NT(279), (NT(270)));
//G566: nf_cmd_arg(279)      => ref(27).
	p(NT(279), (NT(27)));
//G567: nf_cmd_arg(279)      => wff(14).
	p(NT(279), (NT(14)));
//G568: nf_cmd_arg(279)      => bf(30).
	p(NT(279), (NT(30)));
//G569: onf_cmd_arg(306)     => history(270).
	p(NT(306), (NT(270)));
//G570: onf_cmd_arg(306)     => wff(14).
	p(NT(306), (NT(14)));
//G571: normalize_cmd_arg(249) => history(270).
	p(NT(249), (NT(270)));
//G572: normalize_cmd_arg(249) => spec(10).
	p(NT(249), (NT(10)));
//G573: normalize_cmd_arg(249) => ref(27).
	p(NT(249), (NT(27)));
//G574: normalize_cmd_arg(249) => wff(14).
	p(NT(249), (NT(14)));
//G575: normalize_cmd_arg(249) => bf(30).
	p(NT(249), (NT(30)));
//G576: inst_args(284)       => wff_cmd_arg(261) _(11) '[' _(11) bf(30) _(11) '/' _(11) bf_cmd_arg(359) _(11) ']'.
	p(NT(284), (NT(261)+NT(11)+T(26)+NT(11)+NT(30)+NT(11)+T(39)+NT(11)+NT(359)+NT(11)+T(27)));
//G577: inst_args(284)       => bf_cmd_arg(359) _(11) '[' _(11) bf(30) _(11) '/' _(11) bf_cmd_arg(359) _(11) ']'.
	p(NT(284), (NT(359)+NT(11)+T(26)+NT(11)+NT(30)+NT(11)+T(39)+NT(11)+NT(359)+NT(11)+T(27)));
//G578: help_arg(241)        => help_sym(238).
	p(NT(241), (NT(238)));
//G579: help_arg(241)        => version_sym(231).
	p(NT(241), (NT(231)));
//G580: help_arg(241)        => quit_sym(228).
	p(NT(241), (NT(228)));
//G581: help_arg(241)        => clear_sym(234).
	p(NT(241), (NT(234)));
//G582: help_arg(241)        => get_sym(312).
	p(NT(241), (NT(312)));
//G583: help_arg(241)        => set_sym(317).
	p(NT(241), (NT(317)));
//G584: help_arg(241)        => enable_sym(322).
	p(NT(241), (NT(322)));
//G585: help_arg(241)        => disable_sym(325).
	p(NT(241), (NT(325)));
//G586: help_arg(241)        => toggle_sym(328).
	p(NT(241), (NT(328)));
//G587: help_arg(241)        => file_sym(244).
	p(NT(241), (NT(244)));
//G588: help_arg(241)        => history_sym(340).
	p(NT(241), (NT(340)));
//G589: help_arg(241)        => abs_history_sym(360).
	p(NT(241), (NT(360)));
//G590: help_arg(241)        => rel_history_sym(361).
	p(NT(241), (NT(361)));
//G591: help_arg(241)        => selection_sym(362).
	p(NT(241), (NT(362)));
//G592: help_arg(241)        => def_sym(331).
	p(NT(241), (NT(331)));
//G593: help_arg(241)        => inst_sym(282).
	p(NT(241), (NT(282)));
//G594: help_arg(241)        => subst_sym(277).
	p(NT(241), (NT(277)));
//G595: help_arg(241)        => normalize_sym(273).
	p(NT(241), (NT(273)));
//G596: help_arg(241)        => execute_sym(363).
	p(NT(241), (NT(363)));
//G597: help_arg(241)        => solve_sym(258).
	p(NT(241), (NT(258)));
//G598: help_arg(241)        => valid_sym(248).
	p(NT(241), (NT(248)));
//G599: help_arg(241)        => sat_sym(252).
	p(NT(241), (NT(252)));
//G600: help_arg(241)        => unsat_sym(255).
	p(NT(241), (NT(255)));
//G601: help_arg(241)        => run_sym(267).
	p(NT(241), (NT(267)));
//G602: help_arg(241)        => dnf_sym(287).
	p(NT(241), (NT(287)));
//G603: help_arg(241)        => cnf_sym(290).
	p(NT(241), (NT(290)));
//G604: help_arg(241)        => anf_sym(293).
	p(NT(241), (NT(293)));
//G605: help_arg(241)        => snf_sym(364).
	p(NT(241), (NT(364)));
//G606: help_arg(241)        => nnf_sym(296).
	p(NT(241), (NT(296)));
//G607: help_arg(241)        => pnf_sym(299).
	p(NT(241), (NT(299)));
//G608: help_arg(241)        => mnf_sym(302).
	p(NT(241), (NT(302)));
//G609: help_arg(241)        => onf_sym(305).
	p(NT(241), (NT(305)));
//G610: help_arg(241)        => qelim_sym(309).
	p(NT(241), (NT(309)));
//G611: __E___E_help_arg_162_163(367) => 's'.
	p(NT(367), (T(15)));
//G612: __E___E_help_arg_162_163(367) => null.
	p(NT(367), (nul));
//G613: __E_help_arg_162(366) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_162_163(367).
	p(NT(366), (T(11)+T(29)+T(16)+T(21)+T(55)+T(10)+T(11)+NT(367)));
//G614: examples_sym(365)    => __E_help_arg_162(366).
	p(NT(365), (NT(366)));
//G615: help_arg(241)        => examples_sym(365).
	p(NT(241), (NT(365)));
//G616: __E_history_164(369) => '%'.
	p(NT(369), (T(38)));
//G617: rel_history_sym(361) => '%' '-'.
	p(NT(361), (T(38)+T(30)));
//G618: history_id(371)      => digits(213).
	p(NT(371), (NT(213)));
//G619: __E___E_history_164_165(370) => history_id(371).
	p(NT(370), (NT(371)));
//G620: __E___E_history_164_165(370) => null.
	p(NT(370), (nul));
//G621: __E_history_164(369) => rel_history_sym(361) __E___E_history_164_165(370).
	p(NT(369), (NT(361)+NT(370)));
//G622: rel_history(368)     => __E_history_164(369).
	p(NT(368), (NT(369)));
//G623: history(270)         => rel_history(368).
	p(NT(270), (NT(368)));
//G624: abs_history_sym(360) => '%'.
	p(NT(360), (T(38)));
//G625: __E_history_166(373) => abs_history_sym(360) history_id(371).
	p(NT(373), (NT(360)+NT(371)));
//G626: abs_history(372)     => __E_history_166(373).
	p(NT(372), (NT(373)));
//G627: history(270)         => abs_history(372).
	p(NT(270), (NT(372)));
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
		spec, _, definitions, main, wff, spec_multiline, __E_spec_multiline_0, rec_relation, input_def, output_def, 
		__E_spec_multiline_1, __E_definitions_2, __E___E_definitions_2_3, __E_definitions_4, rec_relations, __E_rec_relations_5, __E_rec_relations_6, ref, __E_rec_relation_7, capture, 
		bf, io_var_name, __E_input_def_8, typed, __, stream, __E_output_def_9, __E_output_def_10, q_file_name, console_sym, 
		sym, __E_ref_11, offsets, ref_args, __E_ref_12, __E_ref_13, fp_fallback, first_sym, last_sym, __E_ref_args_14, 
		__E___E_ref_args_14_15, ref_arg, __E___E_ref_args_14_16, __E___E_ref_args_14_17, tau_constant_source, __E_tau_constant_source_18, wff_parenthesis, __E_wff_19, wff_sometimes, __E_wff_20, 
		__E___E_wff_20_21, wff_always, __E_wff_22, __E___E_wff_22_23, wff_conditional, __E_wff_24, wff_all, __E_wff_25, q_vars, wff_ex, 
		__E_wff_26, wff_ref, constraint, wff_imply, __E_wff_27, wff_rimply, __E_wff_28, wff_equiv, __E_wff_29, wff_or, 
		__E_wff_30, wff_xor, __E_wff_31, wff_and, __E_wff_32, wff_neg, __E_wff_33, wff_t, wff_f, bf_interval, 
		__E_wff_34, bf_eq, __E_wff_35, bf_neq, __E_wff_36, bf_lteq, __E_wff_37, bf_nlteq, __E_wff_38, bf_gt, 
		__E_wff_39, bf_ngt, __E_wff_40, bf_gteq, __E_wff_41, bf_ngteq, __E_wff_42, bf_lt, __E_wff_43, bf_nlt, 
		__E_wff_44, bf_parenthesis, __E_bf_45, ba_constant, variable, bf_fall, __E_bf_46, bf_fex, __E_bf_47, bf_ref, 
		bf_shr, __E_bf_48, bf_shl, __E_bf_49, bf_xnor, __E_bf_50, bf_nor, __E_bf_51, bf_nand, __E_bf_52, 
		bf_sub, __E_bf_53, bf_add, __E_bf_54, bf_mod, __E_bf_55, bf_div, __E_bf_56, bf_mul, __E_bf_57, 
		bf_or, __E_bf_58, bf_xor, __E_bf_59, bf_t, __E_bf_60, __E___E_bf_60_61, bf_f, __E_bf_62, __E___E_bf_62_63, 
		bf_and, __E_bf_64, __E___E_bf_64_65, bf_neg, __E_bf_66, bf_neg_oprnd, __E___E_bf_66_67, bf_and_nosep, __E_bf_68, bf_and_nosep_1st_oprnd, 
		__E___E_bf_68_69, bf_and_nosep_2nd_oprnd, __E___E_bf_68_70, ctn_neq, __E_constraint_71, ctnvar, num, ctn_eq, __E_constraint_72, ctn_gteq, 
		__E_constraint_73, ctn_gt, __E_constraint_74, ctn_lteq, __E_constraint_75, ctn_lt, __E_constraint_76, __E_ba_constant_77, source, __E_ba_constant_78, 
		src_c, __E_source_79, __E___E_source_79_80, __E___E_source_79_81, __E_src_c_82, __E_src_c_83, __E_variable_84, uconst, io_var, var_name, 
		__E_variable_85, __E_var_name_86, __E_var_name_87, __E_var_name_88, __E_var_name_89, __E_var_name_90, offset, chars, uconst_name, __E_uconst_91, 
		__E___E_uconst_91_92, q_var, __E_q_vars_93, __E_q_vars_94, __E_offsets_95, __E_offsets_96, integer, shift, __E_offset_97, __E_shift_98, 
		__E___E_shift_98_99, __E_chars_100, __E_chars_101, digits, __E_digits_102, __E_integer_103, type, __E_typed_104, subtype, comment, 
		__E_comment_105, __E_comment_106, __E_comment_107, cli, cli_command, __E_cli_108, __E_cli_109, quit_cmd, quit_sym, __E_cli_command_110, 
		version_cmd, version_sym, __E_cli_command_111, clear_cmd, clear_sym, __E_cli_command_112, help_cmd, __E_cli_command_113, help_sym, __E___E_cli_command_113_114, 
		__E___E_cli_command_113_115, help_arg, file_cmd, __E_cli_command_116, file_sym, q_string, valid_cmd, __E_cli_command_117, valid_sym, normalize_cmd_arg, 
		sat_cmd, __E_cli_command_118, sat_sym, unsat_cmd, __E_cli_command_119, unsat_sym, solve_cmd, __E_cli_command_120, solve_sym, __E___E_cli_command_120_121, 
		solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_122, lgrs_sym, run_cmd, __E_cli_command_123, run_sym, __E___E_cli_command_123_124, __E___E_cli_command_123_125, 
		history, normalize_cmd, __E_cli_command_126, normalize_sym, __E___E_cli_command_126_127, subst_cmd, __E_cli_command_128, subst_sym, __E___E_cli_command_128_129, nf_cmd_arg, 
		inst_cmd, __E_cli_command_130, inst_sym, __E___E_cli_command_130_131, inst_args, dnf_cmd, __E_cli_command_132, dnf_sym, cnf_cmd, __E_cli_command_133, 
		cnf_sym, anf_cmd, __E_cli_command_134, anf_sym, nnf_cmd, __E_cli_command_135, nnf_sym, pnf_cmd, __E_cli_command_136, pnf_sym, 
		mnf_cmd, __E_cli_command_137, mnf_sym, onf_cmd, __E_cli_command_138, onf_sym, onf_cmd_arg, qelim_cmd, __E_cli_command_139, qelim_sym, 
		get_cmd, __E_cli_command_140, get_sym, __E___E_cli_command_140_141, option_name, set_cmd, __E_cli_command_142, set_sym, __E___E_cli_command_142_143, option_value, 
		enable_cmd, __E_cli_command_144, enable_sym, disable_cmd, __E_cli_command_145, disable_sym, toggle_cmd, __E_cli_command_146, toggle_sym, def_list_cmd, 
		__E_cli_command_147, def_sym, __E___E_cli_command_147_148, def_print_cmd, __E_cli_command_149, def_rr_cmd, def_input_cmd, def_output_cmd, history_list_cmd, __E_cli_command_150, 
		history_sym, __E___E_cli_command_150_151, history_print_cmd, __E_cli_command_152, history_store_cmd, __E_cli_command_153, __E_solve_options_154, solve_option, __E_solve_options_155, __E_solve_option_156, 
		solver_mode, solver_mode_minimum, __E_solver_mode_157, solver_mode_maximum, __E_solver_mode_158, file_name, __E_file_name_159, __E_option_name_160, __E_option_value_161, bf_cmd_arg, 
		abs_history_sym, rel_history_sym, selection_sym, execute_sym, snf_sym, examples_sym, __E_help_arg_162, __E___E_help_arg_162_163, rel_history, __E_history_164, 
		__E___E_history_164_165, history_id, abs_history, __E_history_166, __N_0, __N_1, __N_2, __N_3, __N_4, __N_5, 
		__N_6, __N_7, __N_8, 
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
