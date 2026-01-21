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
	"__E_wff_34", "bf_eq", "__E_wff_35", "bf_neq", "__E_wff_36", "bf_lt", "__E_wff_37", "bf_nlt", "__E_wff_38", "bf_lteq", 
	"__E_wff_39", "bf_nlteq", "__E_wff_40", "bf_gt", "__E_wff_41", "bf_ngt", "__E_wff_42", "bf_gteq", "__E_wff_43", "bf_ngteq", 
	"__E_wff_44", "bf_parenthesis", "__E_bf_45", "ba_constant", "variable", "bf_fall", "__E_bf_46", "bf_fex", "__E_bf_47", "bf_ref", 
	"bf_shr", "__E_bf_48", "__E___E_bf_48_49", "bf_shl", "__E_bf_50", "__E___E_bf_50_51", "bf_add", "__E_bf_52", "bf_sub", "__E_bf_53", 
	"__E___E_bf_53_54", "bf_mod", "__E_bf_55", "__E___E_bf_55_56", "bf_mul", "__E_bf_57", "bf_div", "__E_bf_58", "__E___E_bf_58_59", "bf_nor", 
	"__E_bf_60", "__E___E_bf_60_61", "bf_xnor", "__E_bf_62", "__E___E_bf_62_63", "bf_nand", "__E_bf_64", "__E___E_bf_64_65", "bf_or", "__E_bf_66", 
	"bf_xor", "__E_bf_67", "bf_t", "__E_bf_68", "__E___E_bf_68_69", "bf_f", "__E_bf_70", "__E___E_bf_70_71", "bf_and", "__E_bf_72", 
	"__E___E_bf_72_73", "bf_neg", "__E_bf_74", "bf_neg_oprnd", "__E___E_bf_74_75", "bf_and_nosep", "__E_bf_76", "bf_and_nosep_1st_oprnd", "__E___E_bf_76_77", "bf_and_nosep_2nd_oprnd", 
	"__E___E_bf_76_78", "ctn_neq", "__E_constraint_79", "ctnvar", "num", "ctn_eq", "__E_constraint_80", "ctn_gteq", "__E_constraint_81", "ctn_gt", 
	"__E_constraint_82", "ctn_lteq", "__E_constraint_83", "ctn_lt", "__E_constraint_84", "__E_ba_constant_85", "source", "__E_ba_constant_86", "src_c", "__E_source_87", 
	"__E___E_source_87_88", "__E___E_source_87_89", "__E_src_c_90", "__E_src_c_91", "__E_variable_92", "uconst", "io_var", "var_name", "__E_variable_93", "__E_var_name_94", 
	"__E_var_name_95", "__E_var_name_96", "__E_var_name_97", "__E_var_name_98", "offset", "chars", "uconst_name", "__E_uconst_99", "__E___E_uconst_99_100", "q_var", 
	"__E_q_vars_101", "__E_q_vars_102", "__E_offsets_103", "__E_offsets_104", "integer", "shift", "__E_offset_105", "__E_shift_106", "__E___E_shift_106_107", "__E_chars_108", 
	"__E_chars_109", "digits", "__E_digits_110", "__E_integer_111", "type", "__E_typed_112", "subtype", "comment", "__E_comment_113", "__E_comment_114", 
	"__E_comment_115", "cli", "cli_command", "__E_cli_116", "__E_cli_117", "quit_cmd", "quit_sym", "__E_cli_command_118", "version_cmd", "version_sym", 
	"__E_cli_command_119", "clear_cmd", "clear_sym", "__E_cli_command_120", "help_cmd", "__E_cli_command_121", "help_sym", "__E___E_cli_command_121_122", "__E___E_cli_command_121_123", "help_arg", 
	"file_cmd", "__E_cli_command_124", "file_sym", "q_string", "valid_cmd", "__E_cli_command_125", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_126", 
	"sat_sym", "unsat_cmd", "__E_cli_command_127", "unsat_sym", "solve_cmd", "__E_cli_command_128", "solve_sym", "__E___E_cli_command_128_129", "solve_options", "wff_cmd_arg", 
	"lgrs_cmd", "__E_cli_command_130", "lgrs_sym", "run_cmd", "__E_cli_command_131", "run_sym", "__E___E_cli_command_131_132", "__E___E_cli_command_131_133", "history", "normalize_cmd", 
	"__E_cli_command_134", "normalize_sym", "__E___E_cli_command_134_135", "subst_cmd", "__E_cli_command_136", "subst_sym", "__E___E_cli_command_136_137", "nf_cmd_arg", "inst_cmd", "__E_cli_command_138", 
	"inst_sym", "__E___E_cli_command_138_139", "inst_args", "dnf_cmd", "__E_cli_command_140", "dnf_sym", "cnf_cmd", "__E_cli_command_141", "cnf_sym", "anf_cmd", 
	"__E_cli_command_142", "anf_sym", "nnf_cmd", "__E_cli_command_143", "nnf_sym", "pnf_cmd", "__E_cli_command_144", "pnf_sym", "mnf_cmd", "__E_cli_command_145", 
	"mnf_sym", "onf_cmd", "__E_cli_command_146", "onf_sym", "onf_cmd_arg", "qelim_cmd", "__E_cli_command_147", "qelim_sym", "get_cmd", "__E_cli_command_148", 
	"get_sym", "__E___E_cli_command_148_149", "option_name", "set_cmd", "__E_cli_command_150", "set_sym", "__E___E_cli_command_150_151", "option_value", "enable_cmd", "__E_cli_command_152", 
	"enable_sym", "disable_cmd", "__E_cli_command_153", "disable_sym", "toggle_cmd", "__E_cli_command_154", "toggle_sym", "def_list_cmd", "__E_cli_command_155", "def_sym", 
	"__E___E_cli_command_155_156", "def_print_cmd", "__E_cli_command_157", "def_rr_cmd", "def_input_cmd", "def_output_cmd", "history_list_cmd", "__E_cli_command_158", "history_sym", "__E___E_cli_command_158_159", 
	"history_print_cmd", "__E_cli_command_160", "history_store_cmd", "__E_cli_command_161", "__E_solve_options_162", "solve_option", "__E_solve_options_163", "__E_solve_option_164", "solver_mode", "solver_mode_minimum", 
	"__E_solver_mode_165", "solver_mode_maximum", "__E_solver_mode_166", "file_name", "__E_file_name_167", "__E_option_name_168", "__E_option_value_169", "bf_cmd_arg", "abs_history_sym", "rel_history_sym", 
	"selection_sym", "execute_sym", "snf_sym", "examples_sym", "__E_help_arg_170", "__E___E_help_arg_170_171", "rel_history", "__E_history_172", "__E___E_history_172_173", "history_id", 
	"abs_history", "__E_history_174", "unreachable", "processed", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", 
	"__N_6", "__N_7", "__N_8", "__N_9", "__N_10", "__N_11", "__N_12", "__N_13", "__N_14", "__N_15", 
	"__N_16", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', 'a', 'b', 'k', 
	'r', ',', 'm', '<', '>', 'w', 'y', '[', ']', '?', 
	'x', '-', '|', '^', '&', '!', 'T', 'F', '+', '%', 
	'*', '/', '1', '0', '\'', '{', '}', '_', '$', '\t', 
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
			29, 31, 40, 173, 186, 197, 206, 214, 221, 224,
			226, 322, 327, 363
		},
		.to_inline = {
			{ 14, 56, 14 },
			{ 30, 111, 30 },
			{ 163, 111, 30 },
			{ 165, 111, 158 },
			{ 167, 111, 30 },
			{ 169, 111, 30 },
			{ 188 },
			{ 195 },
			{ 195, 206 },
			{ 205 },
			{ 209 },
			{ 257 },
			{ 269 },
			{ 287 },
			{ 292 },
			{ 314 },
			{ 367 }
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
//G29:  input_def(18)        => io_var_name(31) __E_input_def_8(32) _(11) ':' '=' _(11) 'i' 'n' __(34) stream(35).
	p(NT(18), (NT(31)+NT(32)+NT(11)+T(2)+T(3)+NT(11)+T(4)+T(5)+NT(34)+NT(35)));
//G30:  __E_output_def_9(36) => typed(33).
	p(NT(36), (NT(33)));
//G31:  __E_output_def_9(36) => null.
	p(NT(36), (nul));
//G32:  __E_output_def_10(37) => __(34) stream(35).
	p(NT(37), (NT(34)+NT(35)));
//G33:  __E_output_def_10(37) => null.
	p(NT(37), (nul));
//G34:  output_def(19)       => io_var_name(31) __E_output_def_9(36) _(11) ':' '=' _(11) 'o' 'u' 't' __E_output_def_10(37).
	p(NT(19), (NT(31)+NT(36)+NT(11)+T(2)+T(3)+NT(11)+T(6)+T(7)+T(8)+NT(37)));
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
//G123: __E_wff_37(96)       => bf(30) _(11) '<' _(11) bf(30).
	p(NT(96), (NT(30)+NT(11)+T(22)+NT(11)+NT(30)));
//G124: bf_lt(95)            => __E_wff_37(96).
	p(NT(95), (NT(96)));
//G125: wff(14)              => bf_lt(95).
	p(NT(14), (NT(95)));
//G126: __E_wff_38(98)       => bf(30) _(11) '!' '<' _(11) bf(30).
	p(NT(98), (NT(30)+NT(11)+T(34)+T(22)+NT(11)+NT(30)));
//G127: bf_nlt(97)           => __E_wff_38(98).
	p(NT(97), (NT(98)));
//G128: wff(14)              => bf_nlt(97).
	p(NT(14), (NT(97)));
//G129: __E_wff_39(100)      => bf(30) _(11) '<' '=' _(11) bf(30).
	p(NT(100), (NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)));
//G130: bf_lteq(99)          => __E_wff_39(100).
	p(NT(99), (NT(100)));
//G131: wff(14)              => bf_lteq(99).
	p(NT(14), (NT(99)));
//G132: __E_wff_40(102)      => bf(30) _(11) '!' '<' '=' _(11) bf(30).
	p(NT(102), (NT(30)+NT(11)+T(34)+T(22)+T(3)+NT(11)+NT(30)));
//G133: bf_nlteq(101)        => __E_wff_40(102).
	p(NT(101), (NT(102)));
//G134: wff(14)              => bf_nlteq(101).
	p(NT(14), (NT(101)));
//G135: __E_wff_41(104)      => bf(30) _(11) '>' _(11) bf(30).
	p(NT(104), (NT(30)+NT(11)+T(23)+NT(11)+NT(30)));
//G136: bf_gt(103)           => __E_wff_41(104).
	p(NT(103), (NT(104)));
//G137: wff(14)              => bf_gt(103).
	p(NT(14), (NT(103)));
//G138: __E_wff_42(106)      => bf(30) _(11) '!' '>' _(11) bf(30).
	p(NT(106), (NT(30)+NT(11)+T(34)+T(23)+NT(11)+NT(30)));
//G139: bf_ngt(105)          => __E_wff_42(106).
	p(NT(105), (NT(106)));
//G140: wff(14)              => bf_ngt(105).
	p(NT(14), (NT(105)));
//G141: __E_wff_43(108)      => bf(30) _(11) '>' '=' _(11) bf(30).
	p(NT(108), (NT(30)+NT(11)+T(23)+T(3)+NT(11)+NT(30)));
//G142: bf_gteq(107)         => __E_wff_43(108).
	p(NT(107), (NT(108)));
//G143: wff(14)              => bf_gteq(107).
	p(NT(14), (NT(107)));
//G144: __E_wff_44(110)      => bf(30) _(11) '!' '>' '=' _(11) bf(30).
	p(NT(110), (NT(30)+NT(11)+T(34)+T(23)+T(3)+NT(11)+NT(30)));
//G145: bf_ngteq(109)        => __E_wff_44(110).
	p(NT(109), (NT(110)));
//G146: wff(14)              => bf_ngteq(109).
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
//G161: __N_0(384)           => bf_shr(120).
	p(NT(384), (NT(120)));
//G162: __E___E_bf_48_49(122) => bf(30) & ~( __N_0(384) ).	 # conjunctive
	p(NT(122), (NT(30)) & ~(NT(384)));
//G163: __E_bf_48(121)       => bf(30) _(11) '>' '>' _(11) __E___E_bf_48_49(122).
	p(NT(121), (NT(30)+NT(11)+T(23)+T(23)+NT(11)+NT(122)));
//G164: bf_shr(120)          => __E_bf_48(121).
	p(NT(120), (NT(121)));
//G165: bf(30)               => bf_shr(120).
	p(NT(30), (NT(120)));
//G166: __N_1(385)           => bf_shl(123).
	p(NT(385), (NT(123)));
//G167: __E___E_bf_50_51(125) => bf(30) & ~( __N_1(385) ).	 # conjunctive
	p(NT(125), (NT(30)) & ~(NT(385)));
//G168: __E_bf_50(124)       => bf(30) _(11) '<' '<' _(11) __E___E_bf_50_51(125).
	p(NT(124), (NT(30)+NT(11)+T(22)+T(22)+NT(11)+NT(125)));
//G169: bf_shl(123)          => __E_bf_50(124).
	p(NT(123), (NT(124)));
//G170: bf(30)               => bf_shl(123).
	p(NT(30), (NT(123)));
//G171: __E_bf_52(127)       => bf(30) _(11) '+' _(11) bf(30).
	p(NT(127), (NT(30)+NT(11)+T(37)+NT(11)+NT(30)));
//G172: bf_add(126)          => __E_bf_52(127).
	p(NT(126), (NT(127)));
//G173: bf(30)               => bf_add(126).
	p(NT(30), (NT(126)));
//G174: __N_2(386)           => bf_sub(128).
	p(NT(386), (NT(128)));
//G175: __E___E_bf_53_54(130) => bf(30) & ~( __N_2(386) ).	 # conjunctive
	p(NT(130), (NT(30)) & ~(NT(386)));
//G176: __E_bf_53(129)       => bf(30) _(11) '-' _(11) __E___E_bf_53_54(130).
	p(NT(129), (NT(30)+NT(11)+T(30)+NT(11)+NT(130)));
//G177: bf_sub(128)          => __E_bf_53(129).
	p(NT(128), (NT(129)));
//G178: bf(30)               => bf_sub(128).
	p(NT(30), (NT(128)));
//G179: __N_3(387)           => bf_mod(131).
	p(NT(387), (NT(131)));
//G180: __E___E_bf_55_56(133) => bf(30) & ~( __N_3(387) ).	 # conjunctive
	p(NT(133), (NT(30)) & ~(NT(387)));
//G181: __E_bf_55(132)       => bf(30) _(11) '%' _(11) __E___E_bf_55_56(133).
	p(NT(132), (NT(30)+NT(11)+T(38)+NT(11)+NT(133)));
//G182: bf_mod(131)          => __E_bf_55(132).
	p(NT(131), (NT(132)));
//G183: bf(30)               => bf_mod(131).
	p(NT(30), (NT(131)));
//G184: __E_bf_57(135)       => bf(30) _(11) '*' _(11) bf(30).
	p(NT(135), (NT(30)+NT(11)+T(39)+NT(11)+NT(30)));
//G185: bf_mul(134)          => __E_bf_57(135).
	p(NT(134), (NT(135)));
//G186: bf(30)               => bf_mul(134).
	p(NT(30), (NT(134)));
//G187: __N_4(388)           => bf_div(136).
	p(NT(388), (NT(136)));
//G188: __E___E_bf_58_59(138) => bf(30) & ~( __N_4(388) ).	 # conjunctive
	p(NT(138), (NT(30)) & ~(NT(388)));
//G189: __E_bf_58(137)       => bf(30) _(11) '/' _(11) __E___E_bf_58_59(138).
	p(NT(137), (NT(30)+NT(11)+T(40)+NT(11)+NT(138)));
//G190: bf_div(136)          => __E_bf_58(137).
	p(NT(136), (NT(137)));
//G191: bf(30)               => bf_div(136).
	p(NT(30), (NT(136)));
//G192: __N_5(389)           => bf_nor(139).
	p(NT(389), (NT(139)));
//G193: __E___E_bf_60_61(141) => bf(30) & ~( __N_5(389) ).	 # conjunctive
	p(NT(141), (NT(30)) & ~(NT(389)));
//G194: __E_bf_60(140)       => bf(30) _(11) '!' '|' _(11) __E___E_bf_60_61(141).
	p(NT(140), (NT(30)+NT(11)+T(34)+T(31)+NT(11)+NT(141)));
//G195: bf_nor(139)          => __E_bf_60(140).
	p(NT(139), (NT(140)));
//G196: bf(30)               => bf_nor(139).
	p(NT(30), (NT(139)));
//G197: __N_6(390)           => bf_xnor(142).
	p(NT(390), (NT(142)));
//G198: __E___E_bf_62_63(144) => bf(30) & ~( __N_6(390) ).	 # conjunctive
	p(NT(144), (NT(30)) & ~(NT(390)));
//G199: __E_bf_62(143)       => bf(30) _(11) '!' '^' _(11) __E___E_bf_62_63(144).
	p(NT(143), (NT(30)+NT(11)+T(34)+T(32)+NT(11)+NT(144)));
//G200: bf_xnor(142)         => __E_bf_62(143).
	p(NT(142), (NT(143)));
//G201: bf(30)               => bf_xnor(142).
	p(NT(30), (NT(142)));
//G202: __N_7(391)           => bf_nand(145).
	p(NT(391), (NT(145)));
//G203: __E___E_bf_64_65(147) => bf(30) & ~( __N_7(391) ).	 # conjunctive
	p(NT(147), (NT(30)) & ~(NT(391)));
//G204: __E_bf_64(146)       => bf(30) _(11) '!' '&' _(11) __E___E_bf_64_65(147).
	p(NT(146), (NT(30)+NT(11)+T(34)+T(33)+NT(11)+NT(147)));
//G205: bf_nand(145)         => __E_bf_64(146).
	p(NT(145), (NT(146)));
//G206: bf(30)               => bf_nand(145).
	p(NT(30), (NT(145)));
//G207: __E_bf_66(149)       => bf(30) _(11) '|' _(11) bf(30).
	p(NT(149), (NT(30)+NT(11)+T(31)+NT(11)+NT(30)));
//G208: bf_or(148)           => __E_bf_66(149).
	p(NT(148), (NT(149)));
//G209: bf(30)               => bf_or(148).
	p(NT(30), (NT(148)));
//G210: __E_bf_67(151)       => bf(30) _(11) '^' _(11) bf(30).
	p(NT(151), (NT(30)+NT(11)+T(32)+NT(11)+NT(30)));
//G211: bf_xor(150)          => __E_bf_67(151).
	p(NT(150), (NT(151)));
//G212: bf(30)               => bf_xor(150).
	p(NT(30), (NT(150)));
//G213: __E___E_bf_68_69(154) => typed(33).
	p(NT(154), (NT(33)));
//G214: __E___E_bf_68_69(154) => null.
	p(NT(154), (nul));
//G215: __E_bf_68(153)       => '1' __E___E_bf_68_69(154).
	p(NT(153), (T(41)+NT(154)));
//G216: bf_t(152)            => __E_bf_68(153).
	p(NT(152), (NT(153)));
//G217: bf(30)               => bf_t(152).
	p(NT(30), (NT(152)));
//G218: __E___E_bf_70_71(157) => typed(33).
	p(NT(157), (NT(33)));
//G219: __E___E_bf_70_71(157) => null.
	p(NT(157), (nul));
//G220: __E_bf_70(156)       => '0' __E___E_bf_70_71(157).
	p(NT(156), (T(42)+NT(157)));
//G221: bf_f(155)            => __E_bf_70(156).
	p(NT(155), (NT(156)));
//G222: bf(30)               => bf_f(155).
	p(NT(30), (NT(155)));
//G223: __E___E_bf_72_73(160) => _(11) '&' _(11).
	p(NT(160), (NT(11)+T(33)+NT(11)));
//G224: __E___E_bf_72_73(160) => __(34).
	p(NT(160), (NT(34)));
//G225: __E_bf_72(159)       => bf(30) __E___E_bf_72_73(160) bf(30).
	p(NT(159), (NT(30)+NT(160)+NT(30)));
//G226: bf_and(158)          => __E_bf_72(159).
	p(NT(158), (NT(159)));
//G227: bf(30)               => bf_and(158).
	p(NT(30), (NT(158)));
//G228: __E___E_bf_74_75(164) => bf_parenthesis(111).
	p(NT(164), (NT(111)));
//G229: __E___E_bf_74_75(164) => ba_constant(113).
	p(NT(164), (NT(113)));
//G230: __E___E_bf_74_75(164) => variable(114).
	p(NT(164), (NT(114)));
//G231: __E___E_bf_74_75(164) => bf_ref(119).
	p(NT(164), (NT(119)));
//G232: __E___E_bf_74_75(164) => bf_t(152).
	p(NT(164), (NT(152)));
//G233: __E___E_bf_74_75(164) => bf_f(155).
	p(NT(164), (NT(155)));
//G234: __E___E_bf_74_75(164) => bf_neg(161).
	p(NT(164), (NT(161)));
//G235: __E___E_bf_74_75(164) => capture(29).
	p(NT(164), (NT(29)));
//G236: bf_neg_oprnd(163)    => __E___E_bf_74_75(164).
	p(NT(163), (NT(164)));
//G237: __E_bf_74(162)       => bf_neg_oprnd(163) _(11) '\''.
	p(NT(162), (NT(163)+NT(11)+T(43)));
//G238: bf_neg(161)          => __E_bf_74(162).
	p(NT(161), (NT(162)));
//G239: bf(30)               => bf_neg(161).
	p(NT(30), (NT(161)));
//G240: __E___E_bf_76_77(168) => bf_parenthesis(111).
	p(NT(168), (NT(111)));
//G241: __E___E_bf_76_77(168) => variable(114).
	p(NT(168), (NT(114)));
//G242: __E___E_bf_76_77(168) => bf_ref(119).
	p(NT(168), (NT(119)));
//G243: __E___E_bf_76_77(168) => bf_neg(161).
	p(NT(168), (NT(161)));
//G244: __E___E_bf_76_77(168) => bf_and_nosep(165).
	p(NT(168), (NT(165)));
//G245: __E___E_bf_76_77(168) => capture(29).
	p(NT(168), (NT(29)));
//G246: bf_and_nosep_1st_oprnd(167) => __E___E_bf_76_77(168).
	p(NT(167), (NT(168)));
//G247: __E___E_bf_76_78(170) => bf_parenthesis(111).
	p(NT(170), (NT(111)));
//G248: __E___E_bf_76_78(170) => ba_constant(113).
	p(NT(170), (NT(113)));
//G249: __E___E_bf_76_78(170) => variable(114).
	p(NT(170), (NT(114)));
//G250: __E___E_bf_76_78(170) => bf_ref(119).
	p(NT(170), (NT(119)));
//G251: __E___E_bf_76_78(170) => bf_neg(161).
	p(NT(170), (NT(161)));
//G252: __E___E_bf_76_78(170) => capture(29).
	p(NT(170), (NT(29)));
//G253: bf_and_nosep_2nd_oprnd(169) => __E___E_bf_76_78(170).
	p(NT(169), (NT(170)));
//G254: __E_bf_76(166)       => bf_and_nosep_1st_oprnd(167) bf_and_nosep_2nd_oprnd(169).
	p(NT(166), (NT(167)+NT(169)));
//G255: bf_and_nosep(165)    => __E_bf_76(166).
	p(NT(165), (NT(166)));
//G256: bf(30)               => bf_and_nosep(165).
	p(NT(30), (NT(165)));
//G257: bf(30)               => capture(29).
	p(NT(30), (NT(29)));
//G258: __E_constraint_79(172) => '[' ctnvar(173) _(11) '!' '=' _(11) num(174) ']'.
	p(NT(172), (T(26)+NT(173)+NT(11)+T(34)+T(3)+NT(11)+NT(174)+T(27)));
//G259: __E_constraint_79(172) => '[' num(174) _(11) '!' '=' _(11) ctnvar(173) ']'.
	p(NT(172), (T(26)+NT(174)+NT(11)+T(34)+T(3)+NT(11)+NT(173)+T(27)));
//G260: ctn_neq(171)         => __E_constraint_79(172).
	p(NT(171), (NT(172)));
//G261: constraint(72)       => ctn_neq(171).
	p(NT(72), (NT(171)));
//G262: __E_constraint_80(176) => '[' ctnvar(173) _(11) '=' _(11) num(174) ']'.
	p(NT(176), (T(26)+NT(173)+NT(11)+T(3)+NT(11)+NT(174)+T(27)));
//G263: __E_constraint_80(176) => '[' num(174) _(11) '=' _(11) ctnvar(173) ']'.
	p(NT(176), (T(26)+NT(174)+NT(11)+T(3)+NT(11)+NT(173)+T(27)));
//G264: ctn_eq(175)          => __E_constraint_80(176).
	p(NT(175), (NT(176)));
//G265: constraint(72)       => ctn_eq(175).
	p(NT(72), (NT(175)));
//G266: __E_constraint_81(178) => '[' ctnvar(173) _(11) '>' '=' _(11) num(174) ']'.
	p(NT(178), (T(26)+NT(173)+NT(11)+T(23)+T(3)+NT(11)+NT(174)+T(27)));
//G267: __E_constraint_81(178) => '[' num(174) _(11) '>' '=' _(11) ctnvar(173) ']'.
	p(NT(178), (T(26)+NT(174)+NT(11)+T(23)+T(3)+NT(11)+NT(173)+T(27)));
//G268: ctn_gteq(177)        => __E_constraint_81(178).
	p(NT(177), (NT(178)));
//G269: constraint(72)       => ctn_gteq(177).
	p(NT(72), (NT(177)));
//G270: __E_constraint_82(180) => '[' ctnvar(173) _(11) '>' _(11) num(174) ']'.
	p(NT(180), (T(26)+NT(173)+NT(11)+T(23)+NT(11)+NT(174)+T(27)));
//G271: __E_constraint_82(180) => '[' num(174) _(11) '>' _(11) ctnvar(173) ']'.
	p(NT(180), (T(26)+NT(174)+NT(11)+T(23)+NT(11)+NT(173)+T(27)));
//G272: ctn_gt(179)          => __E_constraint_82(180).
	p(NT(179), (NT(180)));
//G273: constraint(72)       => ctn_gt(179).
	p(NT(72), (NT(179)));
//G274: __E_constraint_83(182) => '[' ctnvar(173) _(11) '<' '=' _(11) num(174) ']'.
	p(NT(182), (T(26)+NT(173)+NT(11)+T(22)+T(3)+NT(11)+NT(174)+T(27)));
//G275: __E_constraint_83(182) => '[' num(174) _(11) '<' '=' _(11) ctnvar(173) ']'.
	p(NT(182), (T(26)+NT(174)+NT(11)+T(22)+T(3)+NT(11)+NT(173)+T(27)));
//G276: ctn_lteq(181)        => __E_constraint_83(182).
	p(NT(181), (NT(182)));
//G277: constraint(72)       => ctn_lteq(181).
	p(NT(72), (NT(181)));
//G278: __E_constraint_84(184) => '[' ctnvar(173) _(11) '<' _(11) num(174) ']'.
	p(NT(184), (T(26)+NT(173)+NT(11)+T(22)+NT(11)+NT(174)+T(27)));
//G279: __E_constraint_84(184) => '[' num(174) _(11) '<' _(11) ctnvar(173) ']'.
	p(NT(184), (T(26)+NT(174)+NT(11)+T(22)+NT(11)+NT(173)+T(27)));
//G280: ctn_lt(183)          => __E_constraint_84(184).
	p(NT(183), (NT(184)));
//G281: constraint(72)       => ctn_lt(183).
	p(NT(72), (NT(183)));
//G282: __E_ba_constant_85(185) => capture(29).
	p(NT(185), (NT(29)));
//G283: __E_ba_constant_85(185) => source(186).
	p(NT(185), (NT(186)));
//G284: __E_ba_constant_86(187) => typed(33).
	p(NT(187), (NT(33)));
//G285: __E_ba_constant_86(187) => null.
	p(NT(187), (nul));
//G286: ba_constant(113)     => '{' _(11) __E_ba_constant_85(185) _(11) '}' __E_ba_constant_86(187).
	p(NT(113), (T(44)+NT(11)+NT(185)+NT(11)+T(45)+NT(187)));
//G287: __E___E_source_87_88(190) => src_c(188).
	p(NT(190), (NT(188)));
//G288: __E___E_source_87_88(190) => space(2).
	p(NT(190), (NT(2)));
//G289: __E___E_source_87_89(191) => null.
	p(NT(191), (nul));
//G290: __E___E_source_87_89(191) => __E___E_source_87_88(190) __E___E_source_87_89(191).
	p(NT(191), (NT(190)+NT(191)));
//G291: __E_source_87(189)   => __E___E_source_87_89(191) src_c(188).
	p(NT(189), (NT(191)+NT(188)));
//G292: __E_source_87(189)   => null.
	p(NT(189), (nul));
//G293: source(186)          => src_c(188) __E_source_87(189).
	p(NT(186), (NT(188)+NT(189)));
//G294: src_c(188)           => alnum(6).
	p(NT(188), (NT(6)));
//G295: __N_8(392)           => '{'.
	p(NT(392), (T(44)));
//G296: __N_9(393)           => '}'.
	p(NT(393), (T(45)));
//G297: src_c(188)           => ~( __N_8(392) ) & ~( __N_9(393) ) & punct(7).	 # conjunctive
	p(NT(188), ~(NT(392)) & ~(NT(393)) & (NT(7)));
//G298: __E_src_c_90(192)    => src_c(188).
	p(NT(192), (NT(188)));
//G299: __E_src_c_90(192)    => space(2).
	p(NT(192), (NT(2)));
//G300: __E_src_c_91(193)    => null.
	p(NT(193), (nul));
//G301: __E_src_c_91(193)    => __E_src_c_90(192) __E_src_c_91(193).
	p(NT(193), (NT(192)+NT(193)));
//G302: src_c(188)           => '{' __E_src_c_91(193) '}'.
	p(NT(188), (T(44)+NT(193)+T(45)));
//G303: __E_variable_92(194) => uconst(195).
	p(NT(194), (NT(195)));
//G304: __E_variable_92(194) => io_var(196).
	p(NT(194), (NT(196)));
//G305: __E_variable_92(194) => var_name(197).
	p(NT(194), (NT(197)));
//G306: __E_variable_93(198) => typed(33).
	p(NT(198), (NT(33)));
//G307: __E_variable_93(198) => null.
	p(NT(198), (nul));
//G308: variable(114)        => __E_variable_92(194) __E_variable_93(198).
	p(NT(114), (NT(194)+NT(198)));
//G309: __N_10(394)          => 'F'.
	p(NT(394), (T(36)));
//G310: __N_11(395)          => 'T'.
	p(NT(395), (T(35)));
//G311: __E_var_name_94(199) => ~( __N_10(394) ) & ~( __N_11(395) ) & alpha(5).	 # conjunctive
	p(NT(199), ~(NT(394)) & ~(NT(395)) & (NT(5)));
//G312: __E_var_name_95(200) => null.
	p(NT(200), (nul));
//G313: __E_var_name_95(200) => digit(3) __E_var_name_95(200).
	p(NT(200), (NT(3)+NT(200)));
//G314: var_name(197)        => __E_var_name_94(199) __E_var_name_95(200).	 # guarded: charvar
	p(NT(197), (NT(199)+NT(200)));
	p.back().guard = "charvar";
//G315: __N_12(396)          => 'F'.
	p(NT(396), (T(36)));
//G316: __N_13(397)          => 'T'.
	p(NT(397), (T(35)));
//G317: __E_var_name_96(201) => ~( __N_12(396) ) & ~( __N_13(397) ) & alpha(5).	 # conjunctive
	p(NT(201), ~(NT(396)) & ~(NT(397)) & (NT(5)));
//G318: __E_var_name_97(202) => alnum(6).
	p(NT(202), (NT(6)));
//G319: __E_var_name_97(202) => '_'.
	p(NT(202), (T(46)));
//G320: __E_var_name_98(203) => null.
	p(NT(203), (nul));
//G321: __E_var_name_98(203) => __E_var_name_97(202) __E_var_name_98(203).
	p(NT(203), (NT(202)+NT(203)));
//G322: var_name(197)        => __E_var_name_96(201) __E_var_name_98(203).	 # guarded: var
	p(NT(197), (NT(201)+NT(203)));
	p.back().guard = "var";
//G323: io_var(196)          => io_var_name(31) '[' offset(204) ']'.
	p(NT(196), (NT(31)+T(26)+NT(204)+T(27)));
//G324: io_var_name(31)      => chars(205).
	p(NT(31), (NT(205)));
//G325: __E___E_uconst_99_100(208) => chars(205) _(11).
	p(NT(208), (NT(205)+NT(11)));
//G326: __E___E_uconst_99_100(208) => null.
	p(NT(208), (nul));
//G327: __E_uconst_99(207)   => __E___E_uconst_99_100(208) ':' _(11) chars(205).
	p(NT(207), (NT(208)+T(2)+NT(11)+NT(205)));
//G328: uconst_name(206)     => __E_uconst_99(207).
	p(NT(206), (NT(207)));
//G329: uconst(195)          => '<' _(11) uconst_name(206) _(11) '>'.
	p(NT(195), (T(22)+NT(11)+NT(206)+NT(11)+T(23)));
//G330: __E_q_vars_101(210)  => _(11) ',' _(11) q_var(209).
	p(NT(210), (NT(11)+T(20)+NT(11)+NT(209)));
//G331: __E_q_vars_102(211)  => null.
	p(NT(211), (nul));
//G332: __E_q_vars_102(211)  => __E_q_vars_101(210) __E_q_vars_102(211).
	p(NT(211), (NT(210)+NT(211)));
//G333: q_vars(68)           => q_var(209) __E_q_vars_102(211).
	p(NT(68), (NT(209)+NT(211)));
//G334: q_var(209)           => capture(29).
	p(NT(209), (NT(29)));
//G335: __N_14(398)          => uconst(195).
	p(NT(398), (NT(195)));
//G336: q_var(209)           => variable(114) & ~( __N_14(398) ).	 # conjunctive
	p(NT(209), (NT(114)) & ~(NT(398)));
//G337: ctnvar(173)          => chars(205).
	p(NT(173), (NT(205)));
//G338: __E_offsets_103(212) => _(11) ',' _(11) offset(204).
	p(NT(212), (NT(11)+T(20)+NT(11)+NT(204)));
//G339: __E_offsets_104(213) => null.
	p(NT(213), (nul));
//G340: __E_offsets_104(213) => __E_offsets_103(212) __E_offsets_104(213).
	p(NT(213), (NT(212)+NT(213)));
//G341: offsets(42)          => '[' _(11) offset(204) __E_offsets_104(213) _(11) ']'.
	p(NT(42), (T(26)+NT(11)+NT(204)+NT(213)+NT(11)+T(27)));
//G342: offset(204)          => integer(214).
	p(NT(204), (NT(214)));
//G343: offset(204)          => capture(29).
	p(NT(204), (NT(29)));
//G344: offset(204)          => shift(215).
	p(NT(204), (NT(215)));
//G345: __N_15(399)          => io_var(196).
	p(NT(399), (NT(196)));
//G346: __E_offset_105(216)  => variable(114) & ~( __N_15(399) ).	 # conjunctive
	p(NT(216), (NT(114)) & ~(NT(399)));
//G347: offset(204)          => __E_offset_105(216).
	p(NT(204), (NT(216)));
//G348: __E_shift_106(217)   => capture(29).
	p(NT(217), (NT(29)));
//G349: __N_16(400)          => io_var(196).
	p(NT(400), (NT(196)));
//G350: __E___E_shift_106_107(218) => variable(114) & ~( __N_16(400) ).	 # conjunctive
	p(NT(218), (NT(114)) & ~(NT(400)));
//G351: __E_shift_106(217)   => __E___E_shift_106_107(218).
	p(NT(217), (NT(218)));
//G352: shift(215)           => __E_shift_106(217) _(11) '-' _(11) num(174).
	p(NT(215), (NT(217)+NT(11)+T(30)+NT(11)+NT(174)));
//G353: __E_chars_108(219)   => alnum(6).
	p(NT(219), (NT(6)));
//G354: __E_chars_108(219)   => '_'.
	p(NT(219), (T(46)));
//G355: __E_chars_109(220)   => null.
	p(NT(220), (nul));
//G356: __E_chars_109(220)   => __E_chars_108(219) __E_chars_109(220).
	p(NT(220), (NT(219)+NT(220)));
//G357: chars(205)           => alpha(5) __E_chars_109(220).
	p(NT(205), (NT(5)+NT(220)));
//G358: __E_digits_110(222)  => digit(3).
	p(NT(222), (NT(3)));
//G359: __E_digits_110(222)  => digit(3) __E_digits_110(222).
	p(NT(222), (NT(3)+NT(222)));
//G360: digits(221)          => __E_digits_110(222).
	p(NT(221), (NT(222)));
//G361: num(174)             => digits(221).
	p(NT(174), (NT(221)));
//G362: __E_integer_111(223) => '-'.
	p(NT(223), (T(30)));
//G363: __E_integer_111(223) => null.
	p(NT(223), (nul));
//G364: integer(214)         => __E_integer_111(223) _(11) digits(221).
	p(NT(214), (NT(223)+NT(11)+NT(221)));
//G365: sym(40)              => chars(205).
	p(NT(40), (NT(205)));
//G366: capture(29)          => '$' chars(205).
	p(NT(29), (T(47)+NT(205)));
//G367: __E_typed_112(225)   => _(11) '[' _(11) subtype(226) _(11) ']' _(11).
	p(NT(225), (NT(11)+T(26)+NT(11)+NT(226)+NT(11)+T(27)+NT(11)));
//G368: __E_typed_112(225)   => null.
	p(NT(225), (nul));
//G369: typed(33)            => _(11) ':' _(11) type(224) __E_typed_112(225).
	p(NT(33), (NT(11)+T(2)+NT(11)+NT(224)+NT(225)));
//G370: type(224)            => chars(205).
	p(NT(224), (NT(205)));
//G371: subtype(226)         => num(174).
	p(NT(226), (NT(174)));
//G372: __E_comment_113(228) => printable(8).
	p(NT(228), (NT(8)));
//G373: __E_comment_113(228) => '\t'.
	p(NT(228), (T(48)));
//G374: __E_comment_114(229) => null.
	p(NT(229), (nul));
//G375: __E_comment_114(229) => __E_comment_113(228) __E_comment_114(229).
	p(NT(229), (NT(228)+NT(229)));
//G376: __E_comment_115(230) => '\n'.
	p(NT(230), (T(49)));
//G377: __E_comment_115(230) => '\r'.
	p(NT(230), (T(50)));
//G378: __E_comment_115(230) => eof(1).
	p(NT(230), (NT(1)));
//G379: comment(227)         => '#' __E_comment_114(229) __E_comment_115(230).
	p(NT(227), (T(51)+NT(229)+NT(230)));
//G380: __(34)               => space(2).
	p(NT(34), (NT(2)));
//G381: __(34)               => comment(227).
	p(NT(34), (NT(227)));
//G382: __(34)               => __(34) space(2).
	p(NT(34), (NT(34)+NT(2)));
//G383: __(34)               => __(34) comment(227).
	p(NT(34), (NT(34)+NT(227)));
//G384: _(11)                => __(34).
	p(NT(11), (NT(34)));
//G385: _(11)                => null.
	p(NT(11), (nul));
//G386: cli(231)             => _(11).
	p(NT(231), (NT(11)));
//G387: __E_cli_116(233)     => '.' _(11) cli_command(232) _(11).
	p(NT(233), (T(1)+NT(11)+NT(232)+NT(11)));
//G388: __E_cli_117(234)     => null.
	p(NT(234), (nul));
//G389: __E_cli_117(234)     => __E_cli_116(233) __E_cli_117(234).
	p(NT(234), (NT(233)+NT(234)));
//G390: cli(231)             => _(11) cli_command(232) _(11) __E_cli_117(234).
	p(NT(231), (NT(11)+NT(232)+NT(11)+NT(234)));
//G391: __E_cli_command_118(237) => 'q'.
	p(NT(237), (T(52)));
//G392: __E_cli_command_118(237) => 'q' 'u' 'i' 't'.
	p(NT(237), (T(52)+T(7)+T(4)+T(8)));
//G393: quit_sym(236)        => __E_cli_command_118(237).
	p(NT(236), (NT(237)));
//G394: quit_cmd(235)        => quit_sym(236).
	p(NT(235), (NT(236)));
//G395: cli_command(232)     => quit_cmd(235).
	p(NT(232), (NT(235)));
//G396: __E_cli_command_119(240) => 'v'.
	p(NT(240), (T(53)));
//G397: __E_cli_command_119(240) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(240), (T(53)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G398: version_sym(239)     => __E_cli_command_119(240).
	p(NT(239), (NT(240)));
//G399: version_cmd(238)     => version_sym(239).
	p(NT(238), (NT(239)));
//G400: cli_command(232)     => version_cmd(238).
	p(NT(232), (NT(238)));
//G401: __E_cli_command_120(243) => 'c'.
	p(NT(243), (T(14)));
//G402: __E_cli_command_120(243) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(243), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G403: clear_sym(242)       => __E_cli_command_120(243).
	p(NT(242), (NT(243)));
//G404: clear_cmd(241)       => clear_sym(242).
	p(NT(241), (NT(242)));
//G405: cli_command(232)     => clear_cmd(241).
	p(NT(232), (NT(241)));
//G406: __E___E_cli_command_121_122(247) => 'h'.
	p(NT(247), (T(54)));
//G407: __E___E_cli_command_121_122(247) => 'h' 'e' 'l' 'p'.
	p(NT(247), (T(54)+T(11)+T(10)+T(55)));
//G408: help_sym(246)        => __E___E_cli_command_121_122(247).
	p(NT(246), (NT(247)));
//G409: __E___E_cli_command_121_123(248) => __(34) help_arg(249).
	p(NT(248), (NT(34)+NT(249)));
//G410: __E___E_cli_command_121_123(248) => null.
	p(NT(248), (nul));
//G411: __E_cli_command_121(245) => help_sym(246) __E___E_cli_command_121_123(248).
	p(NT(245), (NT(246)+NT(248)));
//G412: help_cmd(244)        => __E_cli_command_121(245).
	p(NT(244), (NT(245)));
//G413: cli_command(232)     => help_cmd(244).
	p(NT(232), (NT(244)));
//G414: file_sym(252)        => 'f' 'i' 'l' 'e'.
	p(NT(252), (T(9)+T(4)+T(10)+T(11)));
//G415: __E_cli_command_124(251) => file_sym(252) __(34) q_string(253).
	p(NT(251), (NT(252)+NT(34)+NT(253)));
//G416: file_cmd(250)        => __E_cli_command_124(251).
	p(NT(250), (NT(251)));
//G417: cli_command(232)     => file_cmd(250).
	p(NT(232), (NT(250)));
//G418: valid_sym(256)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(256), (T(53)+T(16)+T(10)+T(4)+T(56)));
//G419: __E_cli_command_125(255) => valid_sym(256) __(34) normalize_cmd_arg(257).
	p(NT(255), (NT(256)+NT(34)+NT(257)));
//G420: valid_cmd(254)       => __E_cli_command_125(255).
	p(NT(254), (NT(255)));
//G421: cli_command(232)     => valid_cmd(254).
	p(NT(232), (NT(254)));
//G422: sat_sym(260)         => 's' 'a' 't'.
	p(NT(260), (T(15)+T(16)+T(8)));
//G423: __E_cli_command_126(259) => sat_sym(260) __(34) normalize_cmd_arg(257).
	p(NT(259), (NT(260)+NT(34)+NT(257)));
//G424: sat_cmd(258)         => __E_cli_command_126(259).
	p(NT(258), (NT(259)));
//G425: cli_command(232)     => sat_cmd(258).
	p(NT(232), (NT(258)));
//G426: unsat_sym(263)       => 'u' 'n' 's' 'a' 't'.
	p(NT(263), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G427: __E_cli_command_127(262) => unsat_sym(263) __(34) normalize_cmd_arg(257).
	p(NT(262), (NT(263)+NT(34)+NT(257)));
//G428: unsat_cmd(261)       => __E_cli_command_127(262).
	p(NT(261), (NT(262)));
//G429: cli_command(232)     => unsat_cmd(261).
	p(NT(232), (NT(261)));
//G430: solve_sym(266)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(266), (T(15)+T(6)+T(10)+T(53)+T(11)));
//G431: __E___E_cli_command_128_129(267) => solve_options(268).
	p(NT(267), (NT(268)));
//G432: __E___E_cli_command_128_129(267) => null.
	p(NT(267), (nul));
//G433: __E_cli_command_128(265) => solve_sym(266) __E___E_cli_command_128_129(267) __(34) wff_cmd_arg(269).
	p(NT(265), (NT(266)+NT(267)+NT(34)+NT(269)));
//G434: solve_cmd(264)       => __E_cli_command_128(265).
	p(NT(264), (NT(265)));
//G435: cli_command(232)     => solve_cmd(264).
	p(NT(232), (NT(264)));
//G436: lgrs_sym(272)        => 'l' 'g' 'r' 's'.
	p(NT(272), (T(10)+T(57)+T(19)+T(15)));
//G437: __E_cli_command_130(271) => lgrs_sym(272) __(34) wff_cmd_arg(269).
	p(NT(271), (NT(272)+NT(34)+NT(269)));
//G438: lgrs_cmd(270)        => __E_cli_command_130(271).
	p(NT(270), (NT(271)));
//G439: cli_command(232)     => lgrs_cmd(270).
	p(NT(232), (NT(270)));
//G440: __E___E_cli_command_131_132(276) => 'r'.
	p(NT(276), (T(19)));
//G441: __E___E_cli_command_131_132(276) => 'r' 'u' 'n'.
	p(NT(276), (T(19)+T(7)+T(5)));
//G442: run_sym(275)         => __E___E_cli_command_131_132(276).
	p(NT(275), (NT(276)));
//G443: __E___E_cli_command_131_133(277) => history(278).
	p(NT(277), (NT(278)));
//G444: __E___E_cli_command_131_133(277) => wff(14).
	p(NT(277), (NT(14)));
//G445: __E_cli_command_131(274) => run_sym(275) __(34) __E___E_cli_command_131_133(277).
	p(NT(274), (NT(275)+NT(34)+NT(277)));
//G446: run_cmd(273)         => __E_cli_command_131(274).
	p(NT(273), (NT(274)));
//G447: cli_command(232)     => run_cmd(273).
	p(NT(232), (NT(273)));
//G448: __E___E_cli_command_134_135(282) => 'n'.
	p(NT(282), (T(5)));
//G449: __E___E_cli_command_134_135(282) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(282), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(58)+T(11)));
//G450: normalize_sym(281)   => __E___E_cli_command_134_135(282).
	p(NT(281), (NT(282)));
//G451: __E_cli_command_134(280) => normalize_sym(281) __(34) normalize_cmd_arg(257).
	p(NT(280), (NT(281)+NT(34)+NT(257)));
//G452: normalize_cmd(279)   => __E_cli_command_134(280).
	p(NT(279), (NT(280)));
//G453: cli_command(232)     => normalize_cmd(279).
	p(NT(232), (NT(279)));
//G454: __E___E_cli_command_136_137(286) => 's'.
	p(NT(286), (T(15)));
//G455: __E___E_cli_command_136_137(286) => 's' 'u' 'b' 's' 't'.
	p(NT(286), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G456: __E___E_cli_command_136_137(286) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(286), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G457: subst_sym(285)       => __E___E_cli_command_136_137(286).
	p(NT(285), (NT(286)));
//G458: __E_cli_command_136(284) => subst_sym(285) __(34) nf_cmd_arg(287) _(11) '[' _(11) nf_cmd_arg(287) _(11) '/' _(11) nf_cmd_arg(287) _(11) ']'.
	p(NT(284), (NT(285)+NT(34)+NT(287)+NT(11)+T(26)+NT(11)+NT(287)+NT(11)+T(40)+NT(11)+NT(287)+NT(11)+T(27)));
//G459: subst_cmd(283)       => __E_cli_command_136(284).
	p(NT(283), (NT(284)));
//G460: cli_command(232)     => subst_cmd(283).
	p(NT(232), (NT(283)));
//G461: __E___E_cli_command_138_139(291) => 'i'.
	p(NT(291), (T(4)));
//G462: __E___E_cli_command_138_139(291) => 'i' 'n' 's' 't'.
	p(NT(291), (T(4)+T(5)+T(15)+T(8)));
//G463: __E___E_cli_command_138_139(291) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(291), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G464: inst_sym(290)        => __E___E_cli_command_138_139(291).
	p(NT(290), (NT(291)));
//G465: __E_cli_command_138(289) => inst_sym(290) __(34) inst_args(292).
	p(NT(289), (NT(290)+NT(34)+NT(292)));
//G466: inst_cmd(288)        => __E_cli_command_138(289).
	p(NT(288), (NT(289)));
//G467: cli_command(232)     => inst_cmd(288).
	p(NT(232), (NT(288)));
//G468: dnf_sym(295)         => 'd' 'n' 'f'.
	p(NT(295), (T(56)+T(5)+T(9)));
//G469: __E_cli_command_140(294) => dnf_sym(295) __(34) nf_cmd_arg(287).
	p(NT(294), (NT(295)+NT(34)+NT(287)));
//G470: dnf_cmd(293)         => __E_cli_command_140(294).
	p(NT(293), (NT(294)));
//G471: cli_command(232)     => dnf_cmd(293).
	p(NT(232), (NT(293)));
//G472: cnf_sym(298)         => 'c' 'n' 'f'.
	p(NT(298), (T(14)+T(5)+T(9)));
//G473: __E_cli_command_141(297) => cnf_sym(298) __(34) nf_cmd_arg(287).
	p(NT(297), (NT(298)+NT(34)+NT(287)));
//G474: cnf_cmd(296)         => __E_cli_command_141(297).
	p(NT(296), (NT(297)));
//G475: cli_command(232)     => cnf_cmd(296).
	p(NT(232), (NT(296)));
//G476: anf_sym(301)         => 'a' 'n' 'f'.
	p(NT(301), (T(16)+T(5)+T(9)));
//G477: __E_cli_command_142(300) => anf_sym(301) __(34) nf_cmd_arg(287).
	p(NT(300), (NT(301)+NT(34)+NT(287)));
//G478: anf_cmd(299)         => __E_cli_command_142(300).
	p(NT(299), (NT(300)));
//G479: cli_command(232)     => anf_cmd(299).
	p(NT(232), (NT(299)));
//G480: nnf_sym(304)         => 'n' 'n' 'f'.
	p(NT(304), (T(5)+T(5)+T(9)));
//G481: __E_cli_command_143(303) => nnf_sym(304) __(34) nf_cmd_arg(287).
	p(NT(303), (NT(304)+NT(34)+NT(287)));
//G482: nnf_cmd(302)         => __E_cli_command_143(303).
	p(NT(302), (NT(303)));
//G483: cli_command(232)     => nnf_cmd(302).
	p(NT(232), (NT(302)));
//G484: pnf_sym(307)         => 'p' 'n' 'f'.
	p(NT(307), (T(55)+T(5)+T(9)));
//G485: __E_cli_command_144(306) => pnf_sym(307) __(34) nf_cmd_arg(287).
	p(NT(306), (NT(307)+NT(34)+NT(287)));
//G486: pnf_cmd(305)         => __E_cli_command_144(306).
	p(NT(305), (NT(306)));
//G487: cli_command(232)     => pnf_cmd(305).
	p(NT(232), (NT(305)));
//G488: mnf_sym(310)         => 'm' 'n' 'f'.
	p(NT(310), (T(21)+T(5)+T(9)));
//G489: __E_cli_command_145(309) => mnf_sym(310) __(34) nf_cmd_arg(287).
	p(NT(309), (NT(310)+NT(34)+NT(287)));
//G490: mnf_cmd(308)         => __E_cli_command_145(309).
	p(NT(308), (NT(309)));
//G491: cli_command(232)     => mnf_cmd(308).
	p(NT(232), (NT(308)));
//G492: onf_sym(313)         => 'o' 'n' 'f'.
	p(NT(313), (T(6)+T(5)+T(9)));
//G493: __E_cli_command_146(312) => onf_sym(313) __(34) variable(114) __(34) onf_cmd_arg(314).
	p(NT(312), (NT(313)+NT(34)+NT(114)+NT(34)+NT(314)));
//G494: onf_cmd(311)         => __E_cli_command_146(312).
	p(NT(311), (NT(312)));
//G495: cli_command(232)     => onf_cmd(311).
	p(NT(232), (NT(311)));
//G496: qelim_sym(317)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(317), (T(52)+T(11)+T(10)+T(4)+T(21)));
//G497: __E_cli_command_147(316) => qelim_sym(317) __(34) wff_cmd_arg(269).
	p(NT(316), (NT(317)+NT(34)+NT(269)));
//G498: qelim_cmd(315)       => __E_cli_command_147(316).
	p(NT(315), (NT(316)));
//G499: cli_command(232)     => qelim_cmd(315).
	p(NT(232), (NT(315)));
//G500: get_sym(320)         => 'g' 'e' 't'.
	p(NT(320), (T(57)+T(11)+T(8)));
//G501: __E___E_cli_command_148_149(321) => __(34) option_name(322).
	p(NT(321), (NT(34)+NT(322)));
//G502: __E___E_cli_command_148_149(321) => null.
	p(NT(321), (nul));
//G503: __E_cli_command_148(319) => get_sym(320) __E___E_cli_command_148_149(321).
	p(NT(319), (NT(320)+NT(321)));
//G504: get_cmd(318)         => __E_cli_command_148(319).
	p(NT(318), (NT(319)));
//G505: cli_command(232)     => get_cmd(318).
	p(NT(232), (NT(318)));
//G506: set_sym(325)         => 's' 'e' 't'.
	p(NT(325), (T(15)+T(11)+T(8)));
//G507: __E___E_cli_command_150_151(326) => __(34).
	p(NT(326), (NT(34)));
//G508: __E___E_cli_command_150_151(326) => _(11) '=' _(11).
	p(NT(326), (NT(11)+T(3)+NT(11)));
//G509: __E_cli_command_150(324) => set_sym(325) __(34) option_name(322) __E___E_cli_command_150_151(326) option_value(327).
	p(NT(324), (NT(325)+NT(34)+NT(322)+NT(326)+NT(327)));
//G510: set_cmd(323)         => __E_cli_command_150(324).
	p(NT(323), (NT(324)));
//G511: cli_command(232)     => set_cmd(323).
	p(NT(232), (NT(323)));
//G512: enable_sym(330)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(330), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G513: __E_cli_command_152(329) => enable_sym(330) __(34) option_name(322).
	p(NT(329), (NT(330)+NT(34)+NT(322)));
//G514: enable_cmd(328)      => __E_cli_command_152(329).
	p(NT(328), (NT(329)));
//G515: cli_command(232)     => enable_cmd(328).
	p(NT(232), (NT(328)));
//G516: disable_sym(333)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(333), (T(56)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G517: __E_cli_command_153(332) => disable_sym(333) __(34) option_name(322).
	p(NT(332), (NT(333)+NT(34)+NT(322)));
//G518: disable_cmd(331)     => __E_cli_command_153(332).
	p(NT(331), (NT(332)));
//G519: cli_command(232)     => disable_cmd(331).
	p(NT(232), (NT(331)));
//G520: toggle_sym(336)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(336), (T(8)+T(6)+T(57)+T(57)+T(10)+T(11)));
//G521: __E_cli_command_154(335) => toggle_sym(336) __(34) option_name(322).
	p(NT(335), (NT(336)+NT(34)+NT(322)));
//G522: toggle_cmd(334)      => __E_cli_command_154(335).
	p(NT(334), (NT(335)));
//G523: cli_command(232)     => toggle_cmd(334).
	p(NT(232), (NT(334)));
//G524: __E___E_cli_command_155_156(340) => 'd' 'e' 'f' 's'.
	p(NT(340), (T(56)+T(11)+T(9)+T(15)));
//G525: __E___E_cli_command_155_156(340) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(340), (T(56)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G526: def_sym(339)         => __E___E_cli_command_155_156(340).
	p(NT(339), (NT(340)));
//G527: __E_cli_command_155(338) => def_sym(339).
	p(NT(338), (NT(339)));
//G528: def_list_cmd(337)    => __E_cli_command_155(338).
	p(NT(337), (NT(338)));
//G529: cli_command(232)     => def_list_cmd(337).
	p(NT(232), (NT(337)));
//G530: __E_cli_command_157(342) => def_sym(339) __(34) num(174).
	p(NT(342), (NT(339)+NT(34)+NT(174)));
//G531: def_print_cmd(341)   => __E_cli_command_157(342).
	p(NT(341), (NT(342)));
//G532: cli_command(232)     => def_print_cmd(341).
	p(NT(232), (NT(341)));
//G533: def_rr_cmd(343)      => rec_relation(17).
	p(NT(343), (NT(17)));
//G534: cli_command(232)     => def_rr_cmd(343).
	p(NT(232), (NT(343)));
//G535: def_input_cmd(344)   => input_def(18).
	p(NT(344), (NT(18)));
//G536: cli_command(232)     => def_input_cmd(344).
	p(NT(232), (NT(344)));
//G537: def_output_cmd(345)  => output_def(19).
	p(NT(345), (NT(19)));
//G538: cli_command(232)     => def_output_cmd(345).
	p(NT(232), (NT(345)));
//G539: __E___E_cli_command_158_159(349) => 'h' 'i' 's' 't'.
	p(NT(349), (T(54)+T(4)+T(15)+T(8)));
//G540: __E___E_cli_command_158_159(349) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(349), (T(54)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G541: history_sym(348)     => __E___E_cli_command_158_159(349).
	p(NT(348), (NT(349)));
//G542: __E_cli_command_158(347) => history_sym(348).
	p(NT(347), (NT(348)));
//G543: history_list_cmd(346) => __E_cli_command_158(347).
	p(NT(346), (NT(347)));
//G544: cli_command(232)     => history_list_cmd(346).
	p(NT(232), (NT(346)));
//G545: __E_cli_command_160(351) => history_sym(348) __(34) history(278).
	p(NT(351), (NT(348)+NT(34)+NT(278)));
//G546: history_print_cmd(350) => __E_cli_command_160(351).
	p(NT(350), (NT(351)));
//G547: cli_command(232)     => history_print_cmd(350).
	p(NT(232), (NT(350)));
//G548: __E_cli_command_161(353) => ref(27).
	p(NT(353), (NT(27)));
//G549: __E_cli_command_161(353) => wff(14).
	p(NT(353), (NT(14)));
//G550: __E_cli_command_161(353) => bf(30).
	p(NT(353), (NT(30)));
//G551: history_store_cmd(352) => __E_cli_command_161(353).
	p(NT(352), (NT(353)));
//G552: cli_command(232)     => history_store_cmd(352).
	p(NT(232), (NT(352)));
//G553: __E_solve_options_162(354) => __(34) solve_option(355).
	p(NT(354), (NT(34)+NT(355)));
//G554: __E_solve_options_163(356) => null.
	p(NT(356), (nul));
//G555: __E_solve_options_163(356) => __E_solve_options_162(354) __E_solve_options_163(356).
	p(NT(356), (NT(354)+NT(356)));
//G556: solve_options(268)   => __E_solve_options_163(356).
	p(NT(268), (NT(356)));
//G557: __E_solve_option_164(357) => solver_mode(358).
	p(NT(357), (NT(358)));
//G558: __E_solve_option_164(357) => type(224).
	p(NT(357), (NT(224)));
//G559: solve_option(355)    => '-' '-' __E_solve_option_164(357).
	p(NT(355), (T(30)+T(30)+NT(357)));
//G560: __E_solver_mode_165(360) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(360), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G561: __E_solver_mode_165(360) => 'm' 'i' 'n'.
	p(NT(360), (T(21)+T(4)+T(5)));
//G562: solver_mode_minimum(359) => __E_solver_mode_165(360).
	p(NT(359), (NT(360)));
//G563: solver_mode(358)     => solver_mode_minimum(359).
	p(NT(358), (NT(359)));
//G564: __E_solver_mode_166(362) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(362), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G565: __E_solver_mode_166(362) => 'm' 'a' 'x'.
	p(NT(362), (T(21)+T(16)+T(29)));
//G566: solver_mode_maximum(361) => __E_solver_mode_166(362).
	p(NT(361), (NT(362)));
//G567: solver_mode(358)     => solver_mode_maximum(361).
	p(NT(358), (NT(361)));
//G568: q_file_name(38)      => '"' file_name(363) '"'.
	p(NT(38), (T(59)+NT(363)+T(59)));
//G569: __E_file_name_167(364) => printable(8).
	p(NT(364), (NT(8)));
//G570: __E_file_name_167(364) => printable(8) __E_file_name_167(364).
	p(NT(364), (NT(8)+NT(364)));
//G571: file_name(363)       => __E_file_name_167(364).
	p(NT(363), (NT(364)));
//G572: __E_option_name_168(365) => alnum(6).
	p(NT(365), (NT(6)));
//G573: __E_option_name_168(365) => alnum(6) __E_option_name_168(365).
	p(NT(365), (NT(6)+NT(365)));
//G574: option_name(322)     => __E_option_name_168(365).
	p(NT(322), (NT(365)));
//G575: __E_option_value_169(366) => alnum(6).
	p(NT(366), (NT(6)));
//G576: __E_option_value_169(366) => alnum(6) __E_option_value_169(366).
	p(NT(366), (NT(6)+NT(366)));
//G577: option_value(327)    => __E_option_value_169(366).
	p(NT(327), (NT(366)));
//G578: bf_cmd_arg(367)      => history(278).
	p(NT(367), (NT(278)));
//G579: bf_cmd_arg(367)      => bf(30).
	p(NT(367), (NT(30)));
//G580: wff_cmd_arg(269)     => history(278).
	p(NT(269), (NT(278)));
//G581: wff_cmd_arg(269)     => wff(14).
	p(NT(269), (NT(14)));
//G582: nf_cmd_arg(287)      => history(278).
	p(NT(287), (NT(278)));
//G583: nf_cmd_arg(287)      => ref(27).
	p(NT(287), (NT(27)));
//G584: nf_cmd_arg(287)      => wff(14).
	p(NT(287), (NT(14)));
//G585: nf_cmd_arg(287)      => bf(30).
	p(NT(287), (NT(30)));
//G586: onf_cmd_arg(314)     => history(278).
	p(NT(314), (NT(278)));
//G587: onf_cmd_arg(314)     => wff(14).
	p(NT(314), (NT(14)));
//G588: normalize_cmd_arg(257) => history(278).
	p(NT(257), (NT(278)));
//G589: normalize_cmd_arg(257) => spec(10).
	p(NT(257), (NT(10)));
//G590: normalize_cmd_arg(257) => ref(27).
	p(NT(257), (NT(27)));
//G591: normalize_cmd_arg(257) => wff(14).
	p(NT(257), (NT(14)));
//G592: normalize_cmd_arg(257) => bf(30).
	p(NT(257), (NT(30)));
//G593: inst_args(292)       => wff_cmd_arg(269) _(11) '[' _(11) bf(30) _(11) '/' _(11) bf_cmd_arg(367) _(11) ']'.
	p(NT(292), (NT(269)+NT(11)+T(26)+NT(11)+NT(30)+NT(11)+T(40)+NT(11)+NT(367)+NT(11)+T(27)));
//G594: inst_args(292)       => bf_cmd_arg(367) _(11) '[' _(11) bf(30) _(11) '/' _(11) bf_cmd_arg(367) _(11) ']'.
	p(NT(292), (NT(367)+NT(11)+T(26)+NT(11)+NT(30)+NT(11)+T(40)+NT(11)+NT(367)+NT(11)+T(27)));
//G595: help_arg(249)        => help_sym(246).
	p(NT(249), (NT(246)));
//G596: help_arg(249)        => version_sym(239).
	p(NT(249), (NT(239)));
//G597: help_arg(249)        => quit_sym(236).
	p(NT(249), (NT(236)));
//G598: help_arg(249)        => clear_sym(242).
	p(NT(249), (NT(242)));
//G599: help_arg(249)        => get_sym(320).
	p(NT(249), (NT(320)));
//G600: help_arg(249)        => set_sym(325).
	p(NT(249), (NT(325)));
//G601: help_arg(249)        => enable_sym(330).
	p(NT(249), (NT(330)));
//G602: help_arg(249)        => disable_sym(333).
	p(NT(249), (NT(333)));
//G603: help_arg(249)        => toggle_sym(336).
	p(NT(249), (NT(336)));
//G604: help_arg(249)        => file_sym(252).
	p(NT(249), (NT(252)));
//G605: help_arg(249)        => history_sym(348).
	p(NT(249), (NT(348)));
//G606: help_arg(249)        => abs_history_sym(368).
	p(NT(249), (NT(368)));
//G607: help_arg(249)        => rel_history_sym(369).
	p(NT(249), (NT(369)));
//G608: help_arg(249)        => selection_sym(370).
	p(NT(249), (NT(370)));
//G609: help_arg(249)        => def_sym(339).
	p(NT(249), (NT(339)));
//G610: help_arg(249)        => inst_sym(290).
	p(NT(249), (NT(290)));
//G611: help_arg(249)        => subst_sym(285).
	p(NT(249), (NT(285)));
//G612: help_arg(249)        => normalize_sym(281).
	p(NT(249), (NT(281)));
//G613: help_arg(249)        => execute_sym(371).
	p(NT(249), (NT(371)));
//G614: help_arg(249)        => solve_sym(266).
	p(NT(249), (NT(266)));
//G615: help_arg(249)        => valid_sym(256).
	p(NT(249), (NT(256)));
//G616: help_arg(249)        => sat_sym(260).
	p(NT(249), (NT(260)));
//G617: help_arg(249)        => unsat_sym(263).
	p(NT(249), (NT(263)));
//G618: help_arg(249)        => run_sym(275).
	p(NT(249), (NT(275)));
//G619: help_arg(249)        => dnf_sym(295).
	p(NT(249), (NT(295)));
//G620: help_arg(249)        => cnf_sym(298).
	p(NT(249), (NT(298)));
//G621: help_arg(249)        => anf_sym(301).
	p(NT(249), (NT(301)));
//G622: help_arg(249)        => snf_sym(372).
	p(NT(249), (NT(372)));
//G623: help_arg(249)        => nnf_sym(304).
	p(NT(249), (NT(304)));
//G624: help_arg(249)        => pnf_sym(307).
	p(NT(249), (NT(307)));
//G625: help_arg(249)        => mnf_sym(310).
	p(NT(249), (NT(310)));
//G626: help_arg(249)        => onf_sym(313).
	p(NT(249), (NT(313)));
//G627: help_arg(249)        => qelim_sym(317).
	p(NT(249), (NT(317)));
//G628: __E___E_help_arg_170_171(375) => 's'.
	p(NT(375), (T(15)));
//G629: __E___E_help_arg_170_171(375) => null.
	p(NT(375), (nul));
//G630: __E_help_arg_170(374) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_170_171(375).
	p(NT(374), (T(11)+T(29)+T(16)+T(21)+T(55)+T(10)+T(11)+NT(375)));
//G631: examples_sym(373)    => __E_help_arg_170(374).
	p(NT(373), (NT(374)));
//G632: help_arg(249)        => examples_sym(373).
	p(NT(249), (NT(373)));
//G633: __E_history_172(377) => '%'.
	p(NT(377), (T(38)));
//G634: rel_history_sym(369) => '%' '-'.
	p(NT(369), (T(38)+T(30)));
//G635: history_id(379)      => digits(221).
	p(NT(379), (NT(221)));
//G636: __E___E_history_172_173(378) => history_id(379).
	p(NT(378), (NT(379)));
//G637: __E___E_history_172_173(378) => null.
	p(NT(378), (nul));
//G638: __E_history_172(377) => rel_history_sym(369) __E___E_history_172_173(378).
	p(NT(377), (NT(369)+NT(378)));
//G639: rel_history(376)     => __E_history_172(377).
	p(NT(376), (NT(377)));
//G640: history(278)         => rel_history(376).
	p(NT(278), (NT(376)));
//G641: abs_history_sym(368) => '%'.
	p(NT(368), (T(38)));
//G642: __E_history_174(381) => abs_history_sym(368) history_id(379).
	p(NT(381), (NT(368)+NT(379)));
//G643: abs_history(380)     => __E_history_174(381).
	p(NT(380), (NT(381)));
//G644: history(278)         => abs_history(380).
	p(NT(278), (NT(380)));
//G645: unreachable(382)     => processed(383).
	p(NT(382), (NT(383)));
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
		__E_wff_34, bf_eq, __E_wff_35, bf_neq, __E_wff_36, bf_lt, __E_wff_37, bf_nlt, __E_wff_38, bf_lteq, 
		__E_wff_39, bf_nlteq, __E_wff_40, bf_gt, __E_wff_41, bf_ngt, __E_wff_42, bf_gteq, __E_wff_43, bf_ngteq, 
		__E_wff_44, bf_parenthesis, __E_bf_45, ba_constant, variable, bf_fall, __E_bf_46, bf_fex, __E_bf_47, bf_ref, 
		bf_shr, __E_bf_48, __E___E_bf_48_49, bf_shl, __E_bf_50, __E___E_bf_50_51, bf_add, __E_bf_52, bf_sub, __E_bf_53, 
		__E___E_bf_53_54, bf_mod, __E_bf_55, __E___E_bf_55_56, bf_mul, __E_bf_57, bf_div, __E_bf_58, __E___E_bf_58_59, bf_nor, 
		__E_bf_60, __E___E_bf_60_61, bf_xnor, __E_bf_62, __E___E_bf_62_63, bf_nand, __E_bf_64, __E___E_bf_64_65, bf_or, __E_bf_66, 
		bf_xor, __E_bf_67, bf_t, __E_bf_68, __E___E_bf_68_69, bf_f, __E_bf_70, __E___E_bf_70_71, bf_and, __E_bf_72, 
		__E___E_bf_72_73, bf_neg, __E_bf_74, bf_neg_oprnd, __E___E_bf_74_75, bf_and_nosep, __E_bf_76, bf_and_nosep_1st_oprnd, __E___E_bf_76_77, bf_and_nosep_2nd_oprnd, 
		__E___E_bf_76_78, ctn_neq, __E_constraint_79, ctnvar, num, ctn_eq, __E_constraint_80, ctn_gteq, __E_constraint_81, ctn_gt, 
		__E_constraint_82, ctn_lteq, __E_constraint_83, ctn_lt, __E_constraint_84, __E_ba_constant_85, source, __E_ba_constant_86, src_c, __E_source_87, 
		__E___E_source_87_88, __E___E_source_87_89, __E_src_c_90, __E_src_c_91, __E_variable_92, uconst, io_var, var_name, __E_variable_93, __E_var_name_94, 
		__E_var_name_95, __E_var_name_96, __E_var_name_97, __E_var_name_98, offset, chars, uconst_name, __E_uconst_99, __E___E_uconst_99_100, q_var, 
		__E_q_vars_101, __E_q_vars_102, __E_offsets_103, __E_offsets_104, integer, shift, __E_offset_105, __E_shift_106, __E___E_shift_106_107, __E_chars_108, 
		__E_chars_109, digits, __E_digits_110, __E_integer_111, type, __E_typed_112, subtype, comment, __E_comment_113, __E_comment_114, 
		__E_comment_115, cli, cli_command, __E_cli_116, __E_cli_117, quit_cmd, quit_sym, __E_cli_command_118, version_cmd, version_sym, 
		__E_cli_command_119, clear_cmd, clear_sym, __E_cli_command_120, help_cmd, __E_cli_command_121, help_sym, __E___E_cli_command_121_122, __E___E_cli_command_121_123, help_arg, 
		file_cmd, __E_cli_command_124, file_sym, q_string, valid_cmd, __E_cli_command_125, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_126, 
		sat_sym, unsat_cmd, __E_cli_command_127, unsat_sym, solve_cmd, __E_cli_command_128, solve_sym, __E___E_cli_command_128_129, solve_options, wff_cmd_arg, 
		lgrs_cmd, __E_cli_command_130, lgrs_sym, run_cmd, __E_cli_command_131, run_sym, __E___E_cli_command_131_132, __E___E_cli_command_131_133, history, normalize_cmd, 
		__E_cli_command_134, normalize_sym, __E___E_cli_command_134_135, subst_cmd, __E_cli_command_136, subst_sym, __E___E_cli_command_136_137, nf_cmd_arg, inst_cmd, __E_cli_command_138, 
		inst_sym, __E___E_cli_command_138_139, inst_args, dnf_cmd, __E_cli_command_140, dnf_sym, cnf_cmd, __E_cli_command_141, cnf_sym, anf_cmd, 
		__E_cli_command_142, anf_sym, nnf_cmd, __E_cli_command_143, nnf_sym, pnf_cmd, __E_cli_command_144, pnf_sym, mnf_cmd, __E_cli_command_145, 
		mnf_sym, onf_cmd, __E_cli_command_146, onf_sym, onf_cmd_arg, qelim_cmd, __E_cli_command_147, qelim_sym, get_cmd, __E_cli_command_148, 
		get_sym, __E___E_cli_command_148_149, option_name, set_cmd, __E_cli_command_150, set_sym, __E___E_cli_command_150_151, option_value, enable_cmd, __E_cli_command_152, 
		enable_sym, disable_cmd, __E_cli_command_153, disable_sym, toggle_cmd, __E_cli_command_154, toggle_sym, def_list_cmd, __E_cli_command_155, def_sym, 
		__E___E_cli_command_155_156, def_print_cmd, __E_cli_command_157, def_rr_cmd, def_input_cmd, def_output_cmd, history_list_cmd, __E_cli_command_158, history_sym, __E___E_cli_command_158_159, 
		history_print_cmd, __E_cli_command_160, history_store_cmd, __E_cli_command_161, __E_solve_options_162, solve_option, __E_solve_options_163, __E_solve_option_164, solver_mode, solver_mode_minimum, 
		__E_solver_mode_165, solver_mode_maximum, __E_solver_mode_166, file_name, __E_file_name_167, __E_option_name_168, __E_option_value_169, bf_cmd_arg, abs_history_sym, rel_history_sym, 
		selection_sym, execute_sym, snf_sym, examples_sym, __E_help_arg_170, __E___E_help_arg_170_171, rel_history, __E_history_172, __E___E_history_172_173, history_id, 
		abs_history, __E_history_174, unreachable, processed, __N_0, __N_1, __N_2, __N_3, __N_4, __N_5, 
		__N_6, __N_7, __N_8, __N_9, __N_10, __N_11, __N_12, __N_13, __N_14, __N_15, 
		__N_16, 
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
