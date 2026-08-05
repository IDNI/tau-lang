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
	"spec", "__E_spec_0", "definitions", "_", "main", "wff", "__E_spec_1", "__E_definitions_2", "__E___E_definitions_2_3", "rec_relation", 
	"input_def", "output_def", "type_def", "__E_definitions_4", "spec_multiline", "__E_spec_multiline_5", "spec_part", "__E_spec_multiline_6", "__E_spec_multiline_7", "ref", 
	"__E_rec_relation_8", "capture", "bf", "io_var_name", "__E_input_def_9", "member_path", "__E_input_def_10", "typed", "__", "stream", 
	"__E_output_def_11", "__E_output_def_12", "q_file_name", "console_sym", "type_name", "chars", "__E_type_def_13", "__E___E_type_def_13_14", "type_parents", "__E_type_def_15", 
	"type_body", "__E_type_parents_16", "__E_type_parents_17", "tuple", "type", "__E_type_body_18", "subtype", "member", "__E_tuple_19", "member_name", 
	"__E_tuple_20", "__E_tuple_21", "__E_member_path_22", "__E_member_path_23", "sym", "__E_ref_24", "offsets", "ref_args", "__E_ref_25", "__E_ref_26", 
	"fp_fallback", "first_sym", "last_sym", "__E_ref_args_27", "__E___E_ref_args_27_28", "ref_arg", "__E___E_ref_args_27_29", "__E___E_ref_args_27_30", "wff_parenthesis", "__E_wff_31", 
	"wff_sometimes", "__E_wff_32", "__E___E_wff_32_33", "wff_always", "__E_wff_34", "__E___E_wff_34_35", "wff_conditional", "__E_wff_36", "wff_all", "__E_wff_37", 
	"q_vars", "wff_ex", "__E_wff_38", "wff_ref", "constraint", "wff_imply", "__E_wff_39", "wff_rimply", "__E_wff_40", "wff_equiv", 
	"__E_wff_41", "wff_or", "__E_wff_42", "wff_xor", "__E_wff_43", "wff_and", "__E_wff_44", "wff_neg", "__E_wff_45", "wff_t", 
	"wff_f", "bf_interval", "__E_wff_46", "bf_eq", "__E_wff_47", "bf_neq", "__E_wff_48", "bf_lt", "__E_wff_49", "bf_nlt", 
	"__E_wff_50", "bf_lteq", "__E_wff_51", "bf_nlteq", "__E_wff_52", "bf_gt", "__E_wff_53", "bf_ngt", "__E_wff_54", "bf_gteq", 
	"__E_wff_55", "bf_ngteq", "__E_wff_56", "bf_parenthesis", "__E_bf_57", "ba_constant", "variable", "bf_fall", "__E_bf_58", "bf_fex", 
	"__E_bf_59", "bf_ref", "bf_cast", "__E_bf_60", "num", "bf_cast_oprnd", "__E___E_bf_60_61", "bf_t", "bf_f", "bf_neg", 
	"bf_shr", "__E_bf_62", "__E___E_bf_62_63", "bf_shl", "__E_bf_64", "__E___E_bf_64_65", "bf_add", "__E_bf_66", "bf_sub", "__E_bf_67", 
	"__E___E_bf_67_68", "bf_mod", "__E_bf_69", "__E___E_bf_69_70", "bf_mul", "__E_bf_71", "bf_div", "__E_bf_72", "__E___E_bf_72_73", "bf_nor", 
	"__E_bf_74", "__E___E_bf_74_75", "bf_xnor", "__E_bf_76", "__E___E_bf_76_77", "bf_nand", "__E_bf_78", "__E___E_bf_78_79", "bf_or", "__E_bf_80", 
	"bf_xor", "__E_bf_81", "__E_bf_82", "__E___E_bf_82_83", "__E_bf_84", "__E___E_bf_84_85", "bf_and", "__E_bf_86", "__E___E_bf_86_87", "__E_bf_88", 
	"bf_neg_oprnd", "__E___E_bf_88_89", "bf_and_nosep", "__E_bf_90", "bf_and_nosep_1st_oprnd", "__E___E_bf_90_91", "bf_and_nosep_2nd_oprnd", "__E___E_bf_90_92", "ctn_neq", "__E_constraint_93", 
	"ctnvar", "ctn_eq", "__E_constraint_94", "ctn_gteq", "__E_constraint_95", "ctn_gt", "__E_constraint_96", "ctn_lteq", "__E_constraint_97", "ctn_lt", 
	"__E_constraint_98", "__E_ba_constant_99", "source", "__E_ba_constant_100", "src_c", "__E_source_101", "__E___E_source_101_102", "__E___E_source_101_103", "__E_src_c_104", "__E_src_c_105", 
	"__E_variable_106", "uconst", "io_var", "var_name", "__E_variable_107", "__E_variable_108", "__E_var_name_109", "__E_var_name_110", "__E_var_name_111", "__E_var_name_112", 
	"__E_var_name_113", "offset", "uconst_name", "__E_uconst_114", "__E___E_uconst_114_115", "q_var", "__E_q_vars_116", "__E_q_vars_117", "__E_offsets_118", "__E_offsets_119", 
	"integer", "shift", "__E_offset_120", "__E_shift_121", "__E___E_shift_121_122", "__E_chars_123", "__E_chars_124", "digits", "__E_digits_125", "__E_integer_126", 
	"__E_typed_127", "comment", "__E_comment_128", "__E_comment_129", "__E_comment_130", "cli", "cli_command", "__E_cli_131", "__E_cli_132", "__E_cli_133", 
	"quit_cmd", "quit_sym", "__E_cli_command_134", "version_cmd", "version_sym", "__E_cli_command_135", "clear_cmd", "clear_sym", "__E_cli_command_136", "help_cmd", 
	"__E_cli_command_137", "help_sym", "__E___E_cli_command_137_138", "__E___E_cli_command_137_139", "help_arg", "file_cmd", "__E_cli_command_140", "file_sym", "q_string", "valid_cmd", 
	"__E_cli_command_141", "valid_sym", "spec_arg", "sat_cmd", "__E_cli_command_142", "sat_sym", "unsat_cmd", "__E_cli_command_143", "unsat_sym", "solve_cmd", 
	"__E_cli_command_144", "solve_sym", "__E___E_cli_command_144_145", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_146", "lgrs_sym", "run_cmd", "__E_cli_command_147", 
	"run_sym", "__E___E_cli_command_147_148", "normalize_cmd", "__E_cli_command_149", "normalize_sym", "__E___E_cli_command_149_150", "subst_cmd", "__E_cli_command_151", "subst_sym", "__E___E_cli_command_151_152", 
	"nf_cmd_arg", "inst_cmd", "__E_cli_command_153", "inst_sym", "__E___E_cli_command_153_154", "inst_args", "dnf_cmd", "__E_cli_command_155", "dnf_sym", "cnf_cmd", 
	"__E_cli_command_156", "cnf_sym", "nnf_cmd", "__E_cli_command_157", "nnf_sym", "mnf_cmd", "__E_cli_command_158", "mnf_sym", "onf_cmd", "__E_cli_command_159", 
	"onf_sym", "onf_cmd_arg", "qelim_cmd", "__E_cli_command_160", "qelim_sym", "get_cmd", "__E_cli_command_161", "get_sym", "__E___E_cli_command_161_162", "option_name", 
	"set_cmd", "__E_cli_command_163", "set_sym", "__E___E_cli_command_163_164", "option_value", "enable_cmd", "__E_cli_command_165", "enable_sym", "disable_cmd", "__E_cli_command_166", 
	"disable_sym", "toggle_cmd", "__E_cli_command_167", "toggle_sym", "def_list_cmd", "__E_cli_command_168", "def_sym", "__E___E_cli_command_168_169", "def_print_cmd", "__E_cli_command_170", 
	"def_rr_cmd", "def_input_cmd", "def_output_cmd", "def_type_cmd", "history_list_cmd", "__E_cli_command_171", "history_sym", "__E___E_cli_command_171_172", "history_print_cmd", "__E_cli_command_173", 
	"history", "history_store_cmd", "__E_cli_command_174", "__E_solve_options_175", "solve_option", "__E_solve_options_176", "__E_solve_option_177", "solver_mode", "solver_mode_minimum", "__E_solver_mode_178", 
	"solver_mode_maximum", "__E_solver_mode_179", "file_name", "__E_file_name_180", "__E_option_name_181", "__E_option_value_182", "bf_cmd_arg", "abs_history_sym", "rel_history_sym", "selection_sym", 
	"execute_sym", "snf_sym", "examples_sym", "__E_help_arg_183", "__E___E_help_arg_183_184", "rel_history", "__E_history_185", "__E___E_history_185_186", "history_id", "abs_history", 
	"__E_history_187", "unreachable", "BDD_ID", "stream_def", "fm_or_term", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", 
	"__N_5", "__N_6", "__N_7", "__N_8", "__N_9", "__N_10", "__N_11", "__N_12", "__N_13", "__N_14", 
	"__N_15", "__N_16", "__N_17", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', '/', 'y', 'p', 
	',', '[', ']', '{', '}', 'a', 'b', 'k', 'r', 'm', 
	'<', '>', 'w', '?', 'x', '-', '|', '^', '&', '!', 
	'T', 'F', 'v', '+', '%', '*', '1', '0', '\'', '_', 
	'$', '\t', '\n', '\r', '#', 'q', 'h', 'd', 'g', 'z', 
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
			13, 38
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			31, 33, 44, 54, 56, 59, 64, 200, 212, 223,
			232, 240, 247, 339, 344, 382
		},
		.to_inline = {
			{ 15, 78, 15 },
			{ 26 },
			{ 32, 133, 32 },
			{ 45 },
			{ 145, 133, 32 },
			{ 190, 133, 32 },
			{ 192, 133, 186 },
			{ 194, 133, 32 },
			{ 196, 133, 32 },
			{ 214 },
			{ 221 },
			{ 221, 232 },
			{ 235 },
			{ 282 },
			{ 294 },
			{ 310 },
			{ 315 },
			{ 331 },
			{ 386 }
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
//G0:   start(9)             => spec(10).
	p(NT(9), (NT(10)));
//G1:   __E_spec_0(11)       => definitions(12).
	p(NT(11), (NT(12)));
//G2:   __E_spec_0(11)       => null.
	p(NT(11), (nul));
//G3:   main(14)             => wff(15).
	p(NT(14), (NT(15)));
//G4:   __E_spec_1(16)       => '.' _(13).
	p(NT(16), (T(1)+NT(13)));
//G5:   __E_spec_1(16)       => null.
	p(NT(16), (nul));
//G6:   spec(10)             => __E_spec_0(11) _(13) main(14) _(13) __E_spec_1(16).
	p(NT(10), (NT(11)+NT(13)+NT(14)+NT(13)+NT(16)));
//G7:   __E___E_definitions_2_3(18) => rec_relation(19).
	p(NT(18), (NT(19)));
//G8:   __E___E_definitions_2_3(18) => input_def(20).
	p(NT(18), (NT(20)));
//G9:   __E___E_definitions_2_3(18) => output_def(21).
	p(NT(18), (NT(21)));
//G10:  __E___E_definitions_2_3(18) => type_def(22).
	p(NT(18), (NT(22)));
//G11:  __E_definitions_2(17) => _(13) __E___E_definitions_2_3(18) _(13) '.'.
	p(NT(17), (NT(13)+NT(18)+NT(13)+T(1)));
//G12:  __E_definitions_4(23) => __E_definitions_2(17).
	p(NT(23), (NT(17)));
//G13:  __E_definitions_4(23) => __E_definitions_2(17) __E_definitions_4(23).
	p(NT(23), (NT(17)+NT(23)));
//G14:  definitions(12)      => __E_definitions_4(23).
	p(NT(12), (NT(23)));
//G15:  __E_spec_multiline_5(25) => _(13) spec_part(26) _(13) '.'.
	p(NT(25), (NT(13)+NT(26)+NT(13)+T(1)));
//G16:  __E_spec_multiline_6(27) => null.
	p(NT(27), (nul));
//G17:  __E_spec_multiline_6(27) => __E_spec_multiline_5(25) __E_spec_multiline_6(27).
	p(NT(27), (NT(25)+NT(27)));
//G18:  __E_spec_multiline_7(28) => _(13) '.'.
	p(NT(28), (NT(13)+T(1)));
//G19:  __E_spec_multiline_7(28) => null.
	p(NT(28), (nul));
//G20:  spec_multiline(24)   => __E_spec_multiline_6(27) _(13) spec_part(26) __E_spec_multiline_7(28) _(13).
	p(NT(24), (NT(27)+NT(13)+NT(26)+NT(28)+NT(13)));
//G21:  spec_multiline(24)   => _(13).
	p(NT(24), (NT(13)));
//G22:  spec_part(26)        => rec_relation(19).
	p(NT(26), (NT(19)));
//G23:  spec_part(26)        => input_def(20).
	p(NT(26), (NT(20)));
//G24:  spec_part(26)        => output_def(21).
	p(NT(26), (NT(21)));
//G25:  spec_part(26)        => type_def(22).
	p(NT(26), (NT(22)));
//G26:  spec_part(26)        => main(14).
	p(NT(26), (NT(14)));
//G27:  __E_rec_relation_8(30) => capture(31).
	p(NT(30), (NT(31)));
//G28:  __E_rec_relation_8(30) => ref(29).
	p(NT(30), (NT(29)));
//G29:  __E_rec_relation_8(30) => wff(15).
	p(NT(30), (NT(15)));
//G30:  __E_rec_relation_8(30) => bf(32).
	p(NT(30), (NT(32)));
//G31:  rec_relation(19)     => ref(29) _(13) ':' '=' _(13) __E_rec_relation_8(30).
	p(NT(19), (NT(29)+NT(13)+T(2)+T(3)+NT(13)+NT(30)));
//G32:  __E_input_def_9(34)  => member_path(35).
	p(NT(34), (NT(35)));
//G33:  __E_input_def_9(34)  => null.
	p(NT(34), (nul));
//G34:  __E_input_def_10(36) => typed(37).
	p(NT(36), (NT(37)));
//G35:  __E_input_def_10(36) => null.
	p(NT(36), (nul));
//G36:  input_def(20)        => io_var_name(33) __E_input_def_9(34) __E_input_def_10(36) _(13) ':' '=' _(13) 'i' 'n' __(38) stream(39).
	p(NT(20), (NT(33)+NT(34)+NT(36)+NT(13)+T(2)+T(3)+NT(13)+T(4)+T(5)+NT(38)+NT(39)));
//G37:  __E_output_def_11(40) => member_path(35).
	p(NT(40), (NT(35)));
//G38:  __E_output_def_11(40) => null.
	p(NT(40), (nul));
//G39:  __E_output_def_12(41) => typed(37).
	p(NT(41), (NT(37)));
//G40:  __E_output_def_12(41) => null.
	p(NT(41), (nul));
//G41:  output_def(21)       => io_var_name(33) __E_output_def_11(40) __E_output_def_12(41) _(13) ':' '=' _(13) 'o' 'u' 't' __(38) stream(39).
	p(NT(21), (NT(33)+NT(40)+NT(41)+NT(13)+T(2)+T(3)+NT(13)+T(6)+T(7)+T(8)+NT(38)+NT(39)));
//G42:  stream(39)           => 'f' 'i' 'l' 'e' _(13) '(' _(13) q_file_name(42) _(13) ')'.
	p(NT(39), (T(9)+T(4)+T(10)+T(11)+NT(13)+T(12)+NT(13)+NT(42)+NT(13)+T(13)));
//G43:  console_sym(43)      => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(43), (T(14)+T(6)+T(5)+T(15)+T(6)+T(10)+T(11)));
//G44:  stream(39)           => console_sym(43).
	p(NT(39), (NT(43)));
//G45:  type_name(44)        => chars(45).
	p(NT(44), (NT(45)));
//G46:  __E___E_type_def_13_14(47) => __(38) 'o' 'f' __(38).
	p(NT(47), (NT(38)+T(6)+T(9)+NT(38)));
//G47:  __E___E_type_def_13_14(47) => _(13) '/' _(13).
	p(NT(47), (NT(13)+T(16)+NT(13)));
//G48:  __E_type_def_13(46)  => __E___E_type_def_13_14(47) type_parents(48).
	p(NT(46), (NT(47)+NT(48)));
//G49:  __E_type_def_13(46)  => null.
	p(NT(46), (nul));
//G50:  __E_type_def_15(49)  => __(38) 'i' 's' __(38).
	p(NT(49), (NT(38)+T(4)+T(15)+NT(38)));
//G51:  __E_type_def_15(49)  => _(13) '=' _(13).
	p(NT(49), (NT(13)+T(3)+NT(13)));
//G52:  type_def(22)         => 't' 'y' 'p' 'e' __(38) type_name(44) __E_type_def_13(46) __E_type_def_15(49) type_body(50).
	p(NT(22), (T(8)+T(17)+T(18)+T(11)+NT(38)+NT(44)+NT(46)+NT(49)+NT(50)));
//G53:  __E_type_parents_16(51) => _(13) ',' _(13) type_name(44).
	p(NT(51), (NT(13)+T(19)+NT(13)+NT(44)));
//G54:  __E_type_parents_17(52) => null.
	p(NT(52), (nul));
//G55:  __E_type_parents_17(52) => __E_type_parents_16(51) __E_type_parents_17(52).
	p(NT(52), (NT(51)+NT(52)));
//G56:  type_parents(48)     => '(' _(13) type_name(44) __E_type_parents_17(52) _(13) ')'.
	p(NT(48), (T(12)+NT(13)+NT(44)+NT(52)+NT(13)+T(13)));
//G57:  type_body(50)        => tuple(53).
	p(NT(50), (NT(53)));
//G58:  __E_type_body_18(55) => _(13) '[' _(13) subtype(56) _(13) ']'.
	p(NT(55), (NT(13)+T(20)+NT(13)+NT(56)+NT(13)+T(21)));
//G59:  __E_type_body_18(55) => null.
	p(NT(55), (nul));
//G60:  type_body(50)        => type(54) __E_type_body_18(55).
	p(NT(50), (NT(54)+NT(55)));
//G61:  __E_tuple_19(58)     => member_name(59) typed(37).
	p(NT(58), (NT(59)+NT(37)));
//G62:  member(57)           => __E_tuple_19(58).
	p(NT(57), (NT(58)));
//G63:  __E_tuple_20(60)     => _(13) ',' _(13) member(57).
	p(NT(60), (NT(13)+T(19)+NT(13)+NT(57)));
//G64:  __E_tuple_21(61)     => null.
	p(NT(61), (nul));
//G65:  __E_tuple_21(61)     => __E_tuple_20(60) __E_tuple_21(61).
	p(NT(61), (NT(60)+NT(61)));
//G66:  tuple(53)            => '{' _(13) member(57) __E_tuple_21(61) _(13) '}'.
	p(NT(53), (T(22)+NT(13)+NT(57)+NT(61)+NT(13)+T(23)));
//G67:  member_name(59)      => chars(45).
	p(NT(59), (NT(45)));
//G68:  __E_member_path_22(62) => '.' member_name(59).
	p(NT(62), (T(1)+NT(59)));
//G69:  __E_member_path_23(63) => null.
	p(NT(63), (nul));
//G70:  __E_member_path_23(63) => __E_member_path_22(62) __E_member_path_23(63).
	p(NT(63), (NT(62)+NT(63)));
//G71:  member_path(35)      => '.' member_name(59) __E_member_path_23(63).
	p(NT(35), (T(1)+NT(59)+NT(63)));
//G72:  __E_ref_24(65)       => offsets(66).
	p(NT(65), (NT(66)));
//G73:  __E_ref_24(65)       => null.
	p(NT(65), (nul));
//G74:  __E_ref_25(68)       => typed(37).
	p(NT(68), (NT(37)));
//G75:  __E_ref_25(68)       => null.
	p(NT(68), (nul));
//G76:  __E_ref_26(69)       => _(13) 'f' 'a' 'l' 'l' 'b' 'a' 'c' 'k' __(38) fp_fallback(70).
	p(NT(69), (NT(13)+T(9)+T(24)+T(10)+T(10)+T(25)+T(24)+T(14)+T(26)+NT(38)+NT(70)));
//G77:  __E_ref_26(69)       => null.
	p(NT(69), (nul));
//G78:  ref(29)              => sym(64) __E_ref_24(65) ref_args(67) __E_ref_25(68) __E_ref_26(69).
	p(NT(29), (NT(64)+NT(65)+NT(67)+NT(68)+NT(69)));
//G79:  first_sym(71)        => 'f' 'i' 'r' 's' 't'.
	p(NT(71), (T(9)+T(4)+T(27)+T(15)+T(8)));
//G80:  fp_fallback(70)      => first_sym(71).
	p(NT(70), (NT(71)));
//G81:  last_sym(72)         => 'l' 'a' 's' 't'.
	p(NT(72), (T(10)+T(24)+T(15)+T(8)));
//G82:  fp_fallback(70)      => last_sym(72).
	p(NT(70), (NT(72)));
//G83:  fp_fallback(70)      => capture(31).
	p(NT(70), (NT(31)));
//G84:  fp_fallback(70)      => ref(29).
	p(NT(70), (NT(29)));
//G85:  fp_fallback(70)      => wff(15).
	p(NT(70), (NT(15)));
//G86:  fp_fallback(70)      => bf(32).
	p(NT(70), (NT(32)));
//G87:  ref_arg(75)          => bf(32).
	p(NT(75), (NT(32)));
//G88:  __E___E_ref_args_27_28(74) => _(13) ref_arg(75).
	p(NT(74), (NT(13)+NT(75)));
//G89:  __E___E_ref_args_27_29(76) => _(13) ',' _(13) ref_arg(75).
	p(NT(76), (NT(13)+T(19)+NT(13)+NT(75)));
//G90:  __E___E_ref_args_27_30(77) => null.
	p(NT(77), (nul));
//G91:  __E___E_ref_args_27_30(77) => __E___E_ref_args_27_29(76) __E___E_ref_args_27_30(77).
	p(NT(77), (NT(76)+NT(77)));
//G92:  __E_ref_args_27(73)  => __E___E_ref_args_27_28(74) __E___E_ref_args_27_30(77).
	p(NT(73), (NT(74)+NT(77)));
//G93:  __E_ref_args_27(73)  => null.
	p(NT(73), (nul));
//G94:  ref_args(67)         => '(' __E_ref_args_27(73) _(13) ')'.
	p(NT(67), (T(12)+NT(73)+NT(13)+T(13)));
//G95:  __E_wff_31(79)       => '(' _(13) wff(15) _(13) ')'.
	p(NT(79), (T(12)+NT(13)+NT(15)+NT(13)+T(13)));
//G96:  wff_parenthesis(78)  => __E_wff_31(79).
	p(NT(78), (NT(79)));
//G97:  wff(15)              => wff_parenthesis(78).
	p(NT(15), (NT(78)));
//G98:  __E___E_wff_32_33(82) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(82), (T(15)+T(6)+T(28)+T(11)+T(8)+T(4)+T(28)+T(11)+T(15)));
//G99:  __E___E_wff_32_33(82) => '<' '>'.
	p(NT(82), (T(29)+T(30)));
//G100: __E_wff_32(81)       => __E___E_wff_32_33(82) _(13) wff(15).
	p(NT(81), (NT(82)+NT(13)+NT(15)));
//G101: wff_sometimes(80)    => __E_wff_32(81).
	p(NT(80), (NT(81)));
//G102: wff(15)              => wff_sometimes(80).
	p(NT(15), (NT(80)));
//G103: __E___E_wff_34_35(85) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(85), (T(24)+T(10)+T(31)+T(24)+T(17)+T(15)));
//G104: __E___E_wff_34_35(85) => '[' ']'.
	p(NT(85), (T(20)+T(21)));
//G105: __E_wff_34(84)       => __E___E_wff_34_35(85) _(13) wff(15).
	p(NT(84), (NT(85)+NT(13)+NT(15)));
//G106: wff_always(83)       => __E_wff_34(84).
	p(NT(83), (NT(84)));
//G107: wff(15)              => wff_always(83).
	p(NT(15), (NT(83)));
//G108: __E_wff_36(87)       => wff(15) _(13) '?' _(13) wff(15) _(13) ':' _(13) wff(15).
	p(NT(87), (NT(15)+NT(13)+T(32)+NT(13)+NT(15)+NT(13)+T(2)+NT(13)+NT(15)));
//G109: wff_conditional(86)  => __E_wff_36(87).
	p(NT(86), (NT(87)));
//G110: wff(15)              => wff_conditional(86).
	p(NT(15), (NT(86)));
//G111: __E_wff_37(89)       => 'a' 'l' 'l' __(38) q_vars(90) __(38) wff(15).
	p(NT(89), (T(24)+T(10)+T(10)+NT(38)+NT(90)+NT(38)+NT(15)));
//G112: wff_all(88)          => __E_wff_37(89).
	p(NT(88), (NT(89)));
//G113: wff(15)              => wff_all(88).
	p(NT(15), (NT(88)));
//G114: __E_wff_38(92)       => 'e' 'x' __(38) q_vars(90) __(38) wff(15).
	p(NT(92), (T(11)+T(33)+NT(38)+NT(90)+NT(38)+NT(15)));
//G115: wff_ex(91)           => __E_wff_38(92).
	p(NT(91), (NT(92)));
//G116: wff(15)              => wff_ex(91).
	p(NT(15), (NT(91)));
//G117: wff_ref(93)          => ref(29).
	p(NT(93), (NT(29)));
//G118: wff(15)              => wff_ref(93).
	p(NT(15), (NT(93)));
//G119: wff(15)              => constraint(94).
	p(NT(15), (NT(94)));
//G120: __E_wff_39(96)       => wff(15) _(13) '-' '>' _(13) wff(15).
	p(NT(96), (NT(15)+NT(13)+T(34)+T(30)+NT(13)+NT(15)));
//G121: wff_imply(95)        => __E_wff_39(96).
	p(NT(95), (NT(96)));
//G122: wff(15)              => wff_imply(95).
	p(NT(15), (NT(95)));
//G123: __E_wff_40(98)       => wff(15) _(13) '<' '-' _(13) wff(15).
	p(NT(98), (NT(15)+NT(13)+T(29)+T(34)+NT(13)+NT(15)));
//G124: wff_rimply(97)       => __E_wff_40(98).
	p(NT(97), (NT(98)));
//G125: wff(15)              => wff_rimply(97).
	p(NT(15), (NT(97)));
//G126: __E_wff_41(100)      => wff(15) _(13) '<' '-' '>' _(13) wff(15).
	p(NT(100), (NT(15)+NT(13)+T(29)+T(34)+T(30)+NT(13)+NT(15)));
//G127: wff_equiv(99)        => __E_wff_41(100).
	p(NT(99), (NT(100)));
//G128: wff(15)              => wff_equiv(99).
	p(NT(15), (NT(99)));
//G129: __E_wff_42(102)      => wff(15) _(13) '|' '|' _(13) wff(15).
	p(NT(102), (NT(15)+NT(13)+T(35)+T(35)+NT(13)+NT(15)));
//G130: wff_or(101)          => __E_wff_42(102).
	p(NT(101), (NT(102)));
//G131: wff(15)              => wff_or(101).
	p(NT(15), (NT(101)));
//G132: __E_wff_43(104)      => wff(15) _(13) '^' '^' _(13) wff(15).
	p(NT(104), (NT(15)+NT(13)+T(36)+T(36)+NT(13)+NT(15)));
//G133: wff_xor(103)         => __E_wff_43(104).
	p(NT(103), (NT(104)));
//G134: wff(15)              => wff_xor(103).
	p(NT(15), (NT(103)));
//G135: __E_wff_44(106)      => wff(15) _(13) '&' '&' _(13) wff(15).
	p(NT(106), (NT(15)+NT(13)+T(37)+T(37)+NT(13)+NT(15)));
//G136: wff_and(105)         => __E_wff_44(106).
	p(NT(105), (NT(106)));
//G137: wff(15)              => wff_and(105).
	p(NT(15), (NT(105)));
//G138: __E_wff_45(108)      => '!' _(13) wff(15).
	p(NT(108), (T(38)+NT(13)+NT(15)));
//G139: wff_neg(107)         => __E_wff_45(108).
	p(NT(107), (NT(108)));
//G140: wff(15)              => wff_neg(107).
	p(NT(15), (NT(107)));
//G141: wff_t(109)           => 'T'.
	p(NT(109), (T(39)));
//G142: wff(15)              => wff_t(109).
	p(NT(15), (NT(109)));
//G143: wff_f(110)           => 'F'.
	p(NT(110), (T(40)));
//G144: wff(15)              => wff_f(110).
	p(NT(15), (NT(110)));
//G145: __E_wff_46(112)      => bf(32) _(13) '<' '=' _(13) bf(32) _(13) '<' '=' _(13) bf(32).
	p(NT(112), (NT(32)+NT(13)+T(29)+T(3)+NT(13)+NT(32)+NT(13)+T(29)+T(3)+NT(13)+NT(32)));
//G146: bf_interval(111)     => __E_wff_46(112).
	p(NT(111), (NT(112)));
//G147: wff(15)              => bf_interval(111).
	p(NT(15), (NT(111)));
//G148: __E_wff_47(114)      => bf(32) _(13) '=' _(13) bf(32).
	p(NT(114), (NT(32)+NT(13)+T(3)+NT(13)+NT(32)));
//G149: bf_eq(113)           => __E_wff_47(114).
	p(NT(113), (NT(114)));
//G150: wff(15)              => bf_eq(113).
	p(NT(15), (NT(113)));
//G151: __E_wff_48(116)      => bf(32) _(13) '!' '=' _(13) bf(32).
	p(NT(116), (NT(32)+NT(13)+T(38)+T(3)+NT(13)+NT(32)));
//G152: bf_neq(115)          => __E_wff_48(116).
	p(NT(115), (NT(116)));
//G153: wff(15)              => bf_neq(115).
	p(NT(15), (NT(115)));
//G154: __E_wff_49(118)      => bf(32) _(13) '<' _(13) bf(32).
	p(NT(118), (NT(32)+NT(13)+T(29)+NT(13)+NT(32)));
//G155: bf_lt(117)           => __E_wff_49(118).
	p(NT(117), (NT(118)));
//G156: wff(15)              => bf_lt(117).
	p(NT(15), (NT(117)));
//G157: __E_wff_50(120)      => bf(32) _(13) '!' '<' _(13) bf(32).
	p(NT(120), (NT(32)+NT(13)+T(38)+T(29)+NT(13)+NT(32)));
//G158: bf_nlt(119)          => __E_wff_50(120).
	p(NT(119), (NT(120)));
//G159: wff(15)              => bf_nlt(119).
	p(NT(15), (NT(119)));
//G160: __E_wff_51(122)      => bf(32) _(13) '<' '=' _(13) bf(32).
	p(NT(122), (NT(32)+NT(13)+T(29)+T(3)+NT(13)+NT(32)));
//G161: bf_lteq(121)         => __E_wff_51(122).
	p(NT(121), (NT(122)));
//G162: wff(15)              => bf_lteq(121).
	p(NT(15), (NT(121)));
//G163: __E_wff_52(124)      => bf(32) _(13) '!' '<' '=' _(13) bf(32).
	p(NT(124), (NT(32)+NT(13)+T(38)+T(29)+T(3)+NT(13)+NT(32)));
//G164: bf_nlteq(123)        => __E_wff_52(124).
	p(NT(123), (NT(124)));
//G165: wff(15)              => bf_nlteq(123).
	p(NT(15), (NT(123)));
//G166: __E_wff_53(126)      => bf(32) _(13) '>' _(13) bf(32).
	p(NT(126), (NT(32)+NT(13)+T(30)+NT(13)+NT(32)));
//G167: bf_gt(125)           => __E_wff_53(126).
	p(NT(125), (NT(126)));
//G168: wff(15)              => bf_gt(125).
	p(NT(15), (NT(125)));
//G169: __E_wff_54(128)      => bf(32) _(13) '!' '>' _(13) bf(32).
	p(NT(128), (NT(32)+NT(13)+T(38)+T(30)+NT(13)+NT(32)));
//G170: bf_ngt(127)          => __E_wff_54(128).
	p(NT(127), (NT(128)));
//G171: wff(15)              => bf_ngt(127).
	p(NT(15), (NT(127)));
//G172: __E_wff_55(130)      => bf(32) _(13) '>' '=' _(13) bf(32).
	p(NT(130), (NT(32)+NT(13)+T(30)+T(3)+NT(13)+NT(32)));
//G173: bf_gteq(129)         => __E_wff_55(130).
	p(NT(129), (NT(130)));
//G174: wff(15)              => bf_gteq(129).
	p(NT(15), (NT(129)));
//G175: __E_wff_56(132)      => bf(32) _(13) '!' '>' '=' _(13) bf(32).
	p(NT(132), (NT(32)+NT(13)+T(38)+T(30)+T(3)+NT(13)+NT(32)));
//G176: bf_ngteq(131)        => __E_wff_56(132).
	p(NT(131), (NT(132)));
//G177: wff(15)              => bf_ngteq(131).
	p(NT(15), (NT(131)));
//G178: wff(15)              => capture(31).
	p(NT(15), (NT(31)));
//G179: __E_bf_57(134)       => '(' _(13) bf(32) _(13) ')'.
	p(NT(134), (T(12)+NT(13)+NT(32)+NT(13)+T(13)));
//G180: bf_parenthesis(133)  => __E_bf_57(134).
	p(NT(133), (NT(134)));
//G181: bf(32)               => bf_parenthesis(133).
	p(NT(32), (NT(133)));
//G182: bf(32)               => ba_constant(135).
	p(NT(32), (NT(135)));
//G183: bf(32)               => variable(136).
	p(NT(32), (NT(136)));
//G184: __E_bf_58(138)       => 'f' 'a' 'l' 'l' __(38) q_vars(90) __(38) bf(32).
	p(NT(138), (T(9)+T(24)+T(10)+T(10)+NT(38)+NT(90)+NT(38)+NT(32)));
//G185: bf_fall(137)         => __E_bf_58(138).
	p(NT(137), (NT(138)));
//G186: bf(32)               => bf_fall(137).
	p(NT(32), (NT(137)));
//G187: __E_bf_59(140)       => 'f' 'e' 'x' __(38) q_vars(90) __(38) bf(32).
	p(NT(140), (T(9)+T(11)+T(33)+NT(38)+NT(90)+NT(38)+NT(32)));
//G188: bf_fex(139)          => __E_bf_59(140).
	p(NT(139), (NT(140)));
//G189: bf(32)               => bf_fex(139).
	p(NT(32), (NT(139)));
//G190: bf_ref(141)          => ref(29).
	p(NT(141), (NT(29)));
//G191: bf(32)               => bf_ref(141).
	p(NT(32), (NT(141)));
//G192: __E___E_bf_60_61(146) => bf_parenthesis(133).
	p(NT(146), (NT(133)));
//G193: __E___E_bf_60_61(146) => ba_constant(135).
	p(NT(146), (NT(135)));
//G194: __E___E_bf_60_61(146) => variable(136).
	p(NT(146), (NT(136)));
//G195: __E___E_bf_60_61(146) => bf_ref(141).
	p(NT(146), (NT(141)));
//G196: __E___E_bf_60_61(146) => bf_t(147).
	p(NT(146), (NT(147)));
//G197: __E___E_bf_60_61(146) => bf_f(148).
	p(NT(146), (NT(148)));
//G198: __E___E_bf_60_61(146) => bf_neg(149).
	p(NT(146), (NT(149)));
//G199: __E___E_bf_60_61(146) => bf_cast(142).
	p(NT(146), (NT(142)));
//G200: __E___E_bf_60_61(146) => capture(31).
	p(NT(146), (NT(31)));
//G201: bf_cast_oprnd(145)   => __E___E_bf_60_61(146).
	p(NT(145), (NT(146)));
//G202: __E_bf_60(143)       => '(' _(13) 'b' 'v' _(13) '[' _(13) num(144) _(13) ']' _(13) ')' _(13) bf_cast_oprnd(145).
	p(NT(143), (T(12)+NT(13)+T(25)+T(41)+NT(13)+T(20)+NT(13)+NT(144)+NT(13)+T(21)+NT(13)+T(13)+NT(13)+NT(145)));
//G203: bf_cast(142)         => __E_bf_60(143).
	p(NT(142), (NT(143)));
//G204: bf(32)               => bf_cast(142).
	p(NT(32), (NT(142)));
//G205: __N_0(405)           => bf_shr(150).
	p(NT(405), (NT(150)));
//G206: __E___E_bf_62_63(152) => bf(32) & ~( __N_0(405) ).	 # conjunctive
	p(NT(152), (NT(32)) & ~(NT(405)));
//G207: __E_bf_62(151)       => bf(32) _(13) '>' '>' _(13) __E___E_bf_62_63(152).
	p(NT(151), (NT(32)+NT(13)+T(30)+T(30)+NT(13)+NT(152)));
//G208: bf_shr(150)          => __E_bf_62(151).
	p(NT(150), (NT(151)));
//G209: bf(32)               => bf_shr(150).
	p(NT(32), (NT(150)));
//G210: __N_1(406)           => bf_shl(153).
	p(NT(406), (NT(153)));
//G211: __E___E_bf_64_65(155) => bf(32) & ~( __N_1(406) ).	 # conjunctive
	p(NT(155), (NT(32)) & ~(NT(406)));
//G212: __E_bf_64(154)       => bf(32) _(13) '<' '<' _(13) __E___E_bf_64_65(155).
	p(NT(154), (NT(32)+NT(13)+T(29)+T(29)+NT(13)+NT(155)));
//G213: bf_shl(153)          => __E_bf_64(154).
	p(NT(153), (NT(154)));
//G214: bf(32)               => bf_shl(153).
	p(NT(32), (NT(153)));
//G215: __E_bf_66(157)       => bf(32) _(13) '+' _(13) bf(32).
	p(NT(157), (NT(32)+NT(13)+T(42)+NT(13)+NT(32)));
//G216: bf_add(156)          => __E_bf_66(157).
	p(NT(156), (NT(157)));
//G217: bf(32)               => bf_add(156).
	p(NT(32), (NT(156)));
//G218: __N_2(407)           => bf_sub(158).
	p(NT(407), (NT(158)));
//G219: __E___E_bf_67_68(160) => bf(32) & ~( __N_2(407) ).	 # conjunctive
	p(NT(160), (NT(32)) & ~(NT(407)));
//G220: __E_bf_67(159)       => bf(32) _(13) '-' _(13) __E___E_bf_67_68(160).
	p(NT(159), (NT(32)+NT(13)+T(34)+NT(13)+NT(160)));
//G221: bf_sub(158)          => __E_bf_67(159).
	p(NT(158), (NT(159)));
//G222: bf(32)               => bf_sub(158).
	p(NT(32), (NT(158)));
//G223: __N_3(408)           => bf_mod(161).
	p(NT(408), (NT(161)));
//G224: __E___E_bf_69_70(163) => bf(32) & ~( __N_3(408) ).	 # conjunctive
	p(NT(163), (NT(32)) & ~(NT(408)));
//G225: __E_bf_69(162)       => bf(32) _(13) '%' _(13) __E___E_bf_69_70(163).
	p(NT(162), (NT(32)+NT(13)+T(43)+NT(13)+NT(163)));
//G226: bf_mod(161)          => __E_bf_69(162).
	p(NT(161), (NT(162)));
//G227: bf(32)               => bf_mod(161).
	p(NT(32), (NT(161)));
//G228: __E_bf_71(165)       => bf(32) _(13) '*' _(13) bf(32).
	p(NT(165), (NT(32)+NT(13)+T(44)+NT(13)+NT(32)));
//G229: bf_mul(164)          => __E_bf_71(165).
	p(NT(164), (NT(165)));
//G230: bf(32)               => bf_mul(164).
	p(NT(32), (NT(164)));
//G231: __N_4(409)           => bf_div(166).
	p(NT(409), (NT(166)));
//G232: __E___E_bf_72_73(168) => bf(32) & ~( __N_4(409) ).	 # conjunctive
	p(NT(168), (NT(32)) & ~(NT(409)));
//G233: __E_bf_72(167)       => bf(32) _(13) '/' _(13) __E___E_bf_72_73(168).
	p(NT(167), (NT(32)+NT(13)+T(16)+NT(13)+NT(168)));
//G234: bf_div(166)          => __E_bf_72(167).
	p(NT(166), (NT(167)));
//G235: bf(32)               => bf_div(166).
	p(NT(32), (NT(166)));
//G236: __N_5(410)           => bf_nor(169).
	p(NT(410), (NT(169)));
//G237: __E___E_bf_74_75(171) => bf(32) & ~( __N_5(410) ).	 # conjunctive
	p(NT(171), (NT(32)) & ~(NT(410)));
//G238: __E_bf_74(170)       => bf(32) _(13) '!' '|' _(13) __E___E_bf_74_75(171).
	p(NT(170), (NT(32)+NT(13)+T(38)+T(35)+NT(13)+NT(171)));
//G239: bf_nor(169)          => __E_bf_74(170).
	p(NT(169), (NT(170)));
//G240: bf(32)               => bf_nor(169).
	p(NT(32), (NT(169)));
//G241: __N_6(411)           => bf_xnor(172).
	p(NT(411), (NT(172)));
//G242: __E___E_bf_76_77(174) => bf(32) & ~( __N_6(411) ).	 # conjunctive
	p(NT(174), (NT(32)) & ~(NT(411)));
//G243: __E_bf_76(173)       => bf(32) _(13) '!' '^' _(13) __E___E_bf_76_77(174).
	p(NT(173), (NT(32)+NT(13)+T(38)+T(36)+NT(13)+NT(174)));
//G244: bf_xnor(172)         => __E_bf_76(173).
	p(NT(172), (NT(173)));
//G245: bf(32)               => bf_xnor(172).
	p(NT(32), (NT(172)));
//G246: __N_7(412)           => bf_nand(175).
	p(NT(412), (NT(175)));
//G247: __E___E_bf_78_79(177) => bf(32) & ~( __N_7(412) ).	 # conjunctive
	p(NT(177), (NT(32)) & ~(NT(412)));
//G248: __E_bf_78(176)       => bf(32) _(13) '!' '&' _(13) __E___E_bf_78_79(177).
	p(NT(176), (NT(32)+NT(13)+T(38)+T(37)+NT(13)+NT(177)));
//G249: bf_nand(175)         => __E_bf_78(176).
	p(NT(175), (NT(176)));
//G250: bf(32)               => bf_nand(175).
	p(NT(32), (NT(175)));
//G251: __E_bf_80(179)       => bf(32) _(13) '|' _(13) bf(32).
	p(NT(179), (NT(32)+NT(13)+T(35)+NT(13)+NT(32)));
//G252: bf_or(178)           => __E_bf_80(179).
	p(NT(178), (NT(179)));
//G253: bf(32)               => bf_or(178).
	p(NT(32), (NT(178)));
//G254: __E_bf_81(181)       => bf(32) _(13) '^' _(13) bf(32).
	p(NT(181), (NT(32)+NT(13)+T(36)+NT(13)+NT(32)));
//G255: bf_xor(180)          => __E_bf_81(181).
	p(NT(180), (NT(181)));
//G256: bf(32)               => bf_xor(180).
	p(NT(32), (NT(180)));
//G257: __E___E_bf_82_83(183) => typed(37).
	p(NT(183), (NT(37)));
//G258: __E___E_bf_82_83(183) => null.
	p(NT(183), (nul));
//G259: __E_bf_82(182)       => '1' __E___E_bf_82_83(183).
	p(NT(182), (T(45)+NT(183)));
//G260: bf_t(147)            => __E_bf_82(182).
	p(NT(147), (NT(182)));
//G261: bf(32)               => bf_t(147).
	p(NT(32), (NT(147)));
//G262: __E___E_bf_84_85(185) => typed(37).
	p(NT(185), (NT(37)));
//G263: __E___E_bf_84_85(185) => null.
	p(NT(185), (nul));
//G264: __E_bf_84(184)       => '0' __E___E_bf_84_85(185).
	p(NT(184), (T(46)+NT(185)));
//G265: bf_f(148)            => __E_bf_84(184).
	p(NT(148), (NT(184)));
//G266: bf(32)               => bf_f(148).
	p(NT(32), (NT(148)));
//G267: __E___E_bf_86_87(188) => _(13) '&' _(13).
	p(NT(188), (NT(13)+T(37)+NT(13)));
//G268: __E___E_bf_86_87(188) => __(38).
	p(NT(188), (NT(38)));
//G269: __E_bf_86(187)       => bf(32) __E___E_bf_86_87(188) bf(32).
	p(NT(187), (NT(32)+NT(188)+NT(32)));
//G270: bf_and(186)          => __E_bf_86(187).
	p(NT(186), (NT(187)));
//G271: bf(32)               => bf_and(186).
	p(NT(32), (NT(186)));
//G272: __E___E_bf_88_89(191) => bf_parenthesis(133).
	p(NT(191), (NT(133)));
//G273: __E___E_bf_88_89(191) => ba_constant(135).
	p(NT(191), (NT(135)));
//G274: __E___E_bf_88_89(191) => variable(136).
	p(NT(191), (NT(136)));
//G275: __E___E_bf_88_89(191) => bf_ref(141).
	p(NT(191), (NT(141)));
//G276: __E___E_bf_88_89(191) => bf_t(147).
	p(NT(191), (NT(147)));
//G277: __E___E_bf_88_89(191) => bf_f(148).
	p(NT(191), (NT(148)));
//G278: __E___E_bf_88_89(191) => bf_neg(149).
	p(NT(191), (NT(149)));
//G279: __E___E_bf_88_89(191) => capture(31).
	p(NT(191), (NT(31)));
//G280: bf_neg_oprnd(190)    => __E___E_bf_88_89(191).
	p(NT(190), (NT(191)));
//G281: __E_bf_88(189)       => bf_neg_oprnd(190) _(13) '\''.
	p(NT(189), (NT(190)+NT(13)+T(47)));
//G282: bf_neg(149)          => __E_bf_88(189).
	p(NT(149), (NT(189)));
//G283: bf(32)               => bf_neg(149).
	p(NT(32), (NT(149)));
//G284: __E___E_bf_90_91(195) => bf_parenthesis(133).
	p(NT(195), (NT(133)));
//G285: __E___E_bf_90_91(195) => variable(136).
	p(NT(195), (NT(136)));
//G286: __E___E_bf_90_91(195) => bf_ref(141).
	p(NT(195), (NT(141)));
//G287: __E___E_bf_90_91(195) => bf_neg(149).
	p(NT(195), (NT(149)));
//G288: __E___E_bf_90_91(195) => bf_and_nosep(192).
	p(NT(195), (NT(192)));
//G289: __E___E_bf_90_91(195) => capture(31).
	p(NT(195), (NT(31)));
//G290: bf_and_nosep_1st_oprnd(194) => __E___E_bf_90_91(195).
	p(NT(194), (NT(195)));
//G291: __E___E_bf_90_92(197) => bf_parenthesis(133).
	p(NT(197), (NT(133)));
//G292: __E___E_bf_90_92(197) => ba_constant(135).
	p(NT(197), (NT(135)));
//G293: __E___E_bf_90_92(197) => variable(136).
	p(NT(197), (NT(136)));
//G294: __E___E_bf_90_92(197) => bf_ref(141).
	p(NT(197), (NT(141)));
//G295: __E___E_bf_90_92(197) => bf_neg(149).
	p(NT(197), (NT(149)));
//G296: __E___E_bf_90_92(197) => capture(31).
	p(NT(197), (NT(31)));
//G297: bf_and_nosep_2nd_oprnd(196) => __E___E_bf_90_92(197).
	p(NT(196), (NT(197)));
//G298: __E_bf_90(193)       => bf_and_nosep_1st_oprnd(194) bf_and_nosep_2nd_oprnd(196).
	p(NT(193), (NT(194)+NT(196)));
//G299: bf_and_nosep(192)    => __E_bf_90(193).
	p(NT(192), (NT(193)));
//G300: bf(32)               => bf_and_nosep(192).
	p(NT(32), (NT(192)));
//G301: bf(32)               => capture(31).
	p(NT(32), (NT(31)));
//G302: __E_constraint_93(199) => '[' ctnvar(200) _(13) '!' '=' _(13) num(144) ']'.
	p(NT(199), (T(20)+NT(200)+NT(13)+T(38)+T(3)+NT(13)+NT(144)+T(21)));
//G303: __E_constraint_93(199) => '[' num(144) _(13) '!' '=' _(13) ctnvar(200) ']'.
	p(NT(199), (T(20)+NT(144)+NT(13)+T(38)+T(3)+NT(13)+NT(200)+T(21)));
//G304: ctn_neq(198)         => __E_constraint_93(199).
	p(NT(198), (NT(199)));
//G305: constraint(94)       => ctn_neq(198).
	p(NT(94), (NT(198)));
//G306: __E_constraint_94(202) => '[' ctnvar(200) _(13) '=' _(13) num(144) ']'.
	p(NT(202), (T(20)+NT(200)+NT(13)+T(3)+NT(13)+NT(144)+T(21)));
//G307: __E_constraint_94(202) => '[' num(144) _(13) '=' _(13) ctnvar(200) ']'.
	p(NT(202), (T(20)+NT(144)+NT(13)+T(3)+NT(13)+NT(200)+T(21)));
//G308: ctn_eq(201)          => __E_constraint_94(202).
	p(NT(201), (NT(202)));
//G309: constraint(94)       => ctn_eq(201).
	p(NT(94), (NT(201)));
//G310: __E_constraint_95(204) => '[' ctnvar(200) _(13) '>' '=' _(13) num(144) ']'.
	p(NT(204), (T(20)+NT(200)+NT(13)+T(30)+T(3)+NT(13)+NT(144)+T(21)));
//G311: __E_constraint_95(204) => '[' num(144) _(13) '>' '=' _(13) ctnvar(200) ']'.
	p(NT(204), (T(20)+NT(144)+NT(13)+T(30)+T(3)+NT(13)+NT(200)+T(21)));
//G312: ctn_gteq(203)        => __E_constraint_95(204).
	p(NT(203), (NT(204)));
//G313: constraint(94)       => ctn_gteq(203).
	p(NT(94), (NT(203)));
//G314: __E_constraint_96(206) => '[' ctnvar(200) _(13) '>' _(13) num(144) ']'.
	p(NT(206), (T(20)+NT(200)+NT(13)+T(30)+NT(13)+NT(144)+T(21)));
//G315: __E_constraint_96(206) => '[' num(144) _(13) '>' _(13) ctnvar(200) ']'.
	p(NT(206), (T(20)+NT(144)+NT(13)+T(30)+NT(13)+NT(200)+T(21)));
//G316: ctn_gt(205)          => __E_constraint_96(206).
	p(NT(205), (NT(206)));
//G317: constraint(94)       => ctn_gt(205).
	p(NT(94), (NT(205)));
//G318: __E_constraint_97(208) => '[' ctnvar(200) _(13) '<' '=' _(13) num(144) ']'.
	p(NT(208), (T(20)+NT(200)+NT(13)+T(29)+T(3)+NT(13)+NT(144)+T(21)));
//G319: __E_constraint_97(208) => '[' num(144) _(13) '<' '=' _(13) ctnvar(200) ']'.
	p(NT(208), (T(20)+NT(144)+NT(13)+T(29)+T(3)+NT(13)+NT(200)+T(21)));
//G320: ctn_lteq(207)        => __E_constraint_97(208).
	p(NT(207), (NT(208)));
//G321: constraint(94)       => ctn_lteq(207).
	p(NT(94), (NT(207)));
//G322: __E_constraint_98(210) => '[' ctnvar(200) _(13) '<' _(13) num(144) ']'.
	p(NT(210), (T(20)+NT(200)+NT(13)+T(29)+NT(13)+NT(144)+T(21)));
//G323: __E_constraint_98(210) => '[' num(144) _(13) '<' _(13) ctnvar(200) ']'.
	p(NT(210), (T(20)+NT(144)+NT(13)+T(29)+NT(13)+NT(200)+T(21)));
//G324: ctn_lt(209)          => __E_constraint_98(210).
	p(NT(209), (NT(210)));
//G325: constraint(94)       => ctn_lt(209).
	p(NT(94), (NT(209)));
//G326: __E_ba_constant_99(211) => capture(31).
	p(NT(211), (NT(31)));
//G327: __E_ba_constant_99(211) => source(212).
	p(NT(211), (NT(212)));
//G328: __E_ba_constant_100(213) => typed(37).
	p(NT(213), (NT(37)));
//G329: __E_ba_constant_100(213) => null.
	p(NT(213), (nul));
//G330: ba_constant(135)     => '{' _(13) __E_ba_constant_99(211) _(13) '}' __E_ba_constant_100(213).
	p(NT(135), (T(22)+NT(13)+NT(211)+NT(13)+T(23)+NT(213)));
//G331: __E___E_source_101_102(216) => src_c(214).
	p(NT(216), (NT(214)));
//G332: __E___E_source_101_102(216) => space(2).
	p(NT(216), (NT(2)));
//G333: __E___E_source_101_103(217) => null.
	p(NT(217), (nul));
//G334: __E___E_source_101_103(217) => __E___E_source_101_102(216) __E___E_source_101_103(217).
	p(NT(217), (NT(216)+NT(217)));
//G335: __E_source_101(215)  => __E___E_source_101_103(217) src_c(214).
	p(NT(215), (NT(217)+NT(214)));
//G336: __E_source_101(215)  => null.
	p(NT(215), (nul));
//G337: source(212)          => src_c(214) __E_source_101(215).
	p(NT(212), (NT(214)+NT(215)));
//G338: src_c(214)           => alnum(6).
	p(NT(214), (NT(6)));
//G339: __N_8(413)           => '{'.
	p(NT(413), (T(22)));
//G340: __N_9(414)           => '}'.
	p(NT(414), (T(23)));
//G341: src_c(214)           => ~( __N_8(413) ) & ~( __N_9(414) ) & punct(7).	 # conjunctive
	p(NT(214), ~(NT(413)) & ~(NT(414)) & (NT(7)));
//G342: __E_src_c_104(218)   => src_c(214).
	p(NT(218), (NT(214)));
//G343: __E_src_c_104(218)   => space(2).
	p(NT(218), (NT(2)));
//G344: __E_src_c_105(219)   => null.
	p(NT(219), (nul));
//G345: __E_src_c_105(219)   => __E_src_c_104(218) __E_src_c_105(219).
	p(NT(219), (NT(218)+NT(219)));
//G346: src_c(214)           => '{' __E_src_c_105(219) '}'.
	p(NT(214), (T(22)+NT(219)+T(23)));
//G347: __E_variable_106(220) => uconst(221).
	p(NT(220), (NT(221)));
//G348: __E_variable_106(220) => io_var(222).
	p(NT(220), (NT(222)));
//G349: __E_variable_106(220) => var_name(223).
	p(NT(220), (NT(223)));
//G350: __E_variable_107(224) => member_path(35).
	p(NT(224), (NT(35)));
//G351: __E_variable_107(224) => null.
	p(NT(224), (nul));
//G352: __E_variable_108(225) => typed(37).
	p(NT(225), (NT(37)));
//G353: __E_variable_108(225) => null.
	p(NT(225), (nul));
//G354: variable(136)        => __E_variable_106(220) __E_variable_107(224) __E_variable_108(225).
	p(NT(136), (NT(220)+NT(224)+NT(225)));
//G355: __N_10(415)          => 'F'.
	p(NT(415), (T(40)));
//G356: __N_11(416)          => 'T'.
	p(NT(416), (T(39)));
//G357: __E_var_name_109(226) => ~( __N_10(415) ) & ~( __N_11(416) ) & alpha(5).	 # conjunctive
	p(NT(226), ~(NT(415)) & ~(NT(416)) & (NT(5)));
//G358: __E_var_name_110(227) => null.
	p(NT(227), (nul));
//G359: __E_var_name_110(227) => digit(3) __E_var_name_110(227).
	p(NT(227), (NT(3)+NT(227)));
//G360: var_name(223)        => __E_var_name_109(226) __E_var_name_110(227).	 # guarded: charvar
	p(NT(223), (NT(226)+NT(227)));
	p.back().guard = "charvar";
//G361: __N_12(417)          => 'F'.
	p(NT(417), (T(40)));
//G362: __N_13(418)          => 'T'.
	p(NT(418), (T(39)));
//G363: __E_var_name_111(228) => ~( __N_12(417) ) & ~( __N_13(418) ) & alpha(5).	 # conjunctive
	p(NT(228), ~(NT(417)) & ~(NT(418)) & (NT(5)));
//G364: __E_var_name_112(229) => alnum(6).
	p(NT(229), (NT(6)));
//G365: __E_var_name_112(229) => '_'.
	p(NT(229), (T(48)));
//G366: __E_var_name_113(230) => null.
	p(NT(230), (nul));
//G367: __E_var_name_113(230) => __E_var_name_112(229) __E_var_name_113(230).
	p(NT(230), (NT(229)+NT(230)));
//G368: var_name(223)        => __E_var_name_111(228) __E_var_name_113(230).	 # guarded: var
	p(NT(223), (NT(228)+NT(230)));
	p.back().guard = "var";
//G369: io_var(222)          => io_var_name(33) '[' offset(231) ']'.
	p(NT(222), (NT(33)+T(20)+NT(231)+T(21)));
//G370: io_var_name(33)      => chars(45).
	p(NT(33), (NT(45)));
//G371: __E___E_uconst_114_115(234) => chars(45) _(13).
	p(NT(234), (NT(45)+NT(13)));
//G372: __E___E_uconst_114_115(234) => null.
	p(NT(234), (nul));
//G373: __E_uconst_114(233)  => __E___E_uconst_114_115(234) ':' _(13) chars(45).
	p(NT(233), (NT(234)+T(2)+NT(13)+NT(45)));
//G374: uconst_name(232)     => __E_uconst_114(233).
	p(NT(232), (NT(233)));
//G375: uconst(221)          => '<' _(13) uconst_name(232) _(13) '>'.
	p(NT(221), (T(29)+NT(13)+NT(232)+NT(13)+T(30)));
//G376: __E_q_vars_116(236)  => _(13) ',' _(13) q_var(235).
	p(NT(236), (NT(13)+T(19)+NT(13)+NT(235)));
//G377: __E_q_vars_117(237)  => null.
	p(NT(237), (nul));
//G378: __E_q_vars_117(237)  => __E_q_vars_116(236) __E_q_vars_117(237).
	p(NT(237), (NT(236)+NT(237)));
//G379: q_vars(90)           => q_var(235) __E_q_vars_117(237).
	p(NT(90), (NT(235)+NT(237)));
//G380: q_var(235)           => capture(31).
	p(NT(235), (NT(31)));
//G381: __N_14(419)          => uconst(221).
	p(NT(419), (NT(221)));
//G382: q_var(235)           => variable(136) & ~( __N_14(419) ).	 # conjunctive
	p(NT(235), (NT(136)) & ~(NT(419)));
//G383: ctnvar(200)          => chars(45).
	p(NT(200), (NT(45)));
//G384: __E_offsets_118(238) => _(13) ',' _(13) offset(231).
	p(NT(238), (NT(13)+T(19)+NT(13)+NT(231)));
//G385: __E_offsets_119(239) => null.
	p(NT(239), (nul));
//G386: __E_offsets_119(239) => __E_offsets_118(238) __E_offsets_119(239).
	p(NT(239), (NT(238)+NT(239)));
//G387: offsets(66)          => '[' _(13) offset(231) __E_offsets_119(239) _(13) ']'.
	p(NT(66), (T(20)+NT(13)+NT(231)+NT(239)+NT(13)+T(21)));
//G388: offset(231)          => integer(240).
	p(NT(231), (NT(240)));
//G389: offset(231)          => capture(31).
	p(NT(231), (NT(31)));
//G390: offset(231)          => shift(241).
	p(NT(231), (NT(241)));
//G391: __N_15(420)          => io_var(222).
	p(NT(420), (NT(222)));
//G392: __E_offset_120(242)  => variable(136) & ~( __N_15(420) ).	 # conjunctive
	p(NT(242), (NT(136)) & ~(NT(420)));
//G393: offset(231)          => __E_offset_120(242).
	p(NT(231), (NT(242)));
//G394: __E_shift_121(243)   => capture(31).
	p(NT(243), (NT(31)));
//G395: __N_16(421)          => io_var(222).
	p(NT(421), (NT(222)));
//G396: __E___E_shift_121_122(244) => variable(136) & ~( __N_16(421) ).	 # conjunctive
	p(NT(244), (NT(136)) & ~(NT(421)));
//G397: __E_shift_121(243)   => __E___E_shift_121_122(244).
	p(NT(243), (NT(244)));
//G398: shift(241)           => __E_shift_121(243) _(13) '-' _(13) num(144).
	p(NT(241), (NT(243)+NT(13)+T(34)+NT(13)+NT(144)));
//G399: __E_chars_123(245)   => alnum(6).
	p(NT(245), (NT(6)));
//G400: __E_chars_123(245)   => '_'.
	p(NT(245), (T(48)));
//G401: __E_chars_124(246)   => null.
	p(NT(246), (nul));
//G402: __E_chars_124(246)   => __E_chars_123(245) __E_chars_124(246).
	p(NT(246), (NT(245)+NT(246)));
//G403: chars(45)            => alpha(5) __E_chars_124(246).
	p(NT(45), (NT(5)+NT(246)));
//G404: __E_digits_125(248)  => digit(3).
	p(NT(248), (NT(3)));
//G405: __E_digits_125(248)  => digit(3) __E_digits_125(248).
	p(NT(248), (NT(3)+NT(248)));
//G406: digits(247)          => __E_digits_125(248).
	p(NT(247), (NT(248)));
//G407: num(144)             => digits(247).
	p(NT(144), (NT(247)));
//G408: __E_integer_126(249) => '-'.
	p(NT(249), (T(34)));
//G409: __E_integer_126(249) => null.
	p(NT(249), (nul));
//G410: integer(240)         => __E_integer_126(249) _(13) digits(247).
	p(NT(240), (NT(249)+NT(13)+NT(247)));
//G411: sym(64)              => chars(45).
	p(NT(64), (NT(45)));
//G412: capture(31)          => '$' chars(45).
	p(NT(31), (T(49)+NT(45)));
//G413: typed(37)            => _(13) ':' _(13) type(54) _(13) '[' _(13) subtype(56) _(13) ']'.
	p(NT(37), (NT(13)+T(2)+NT(13)+NT(54)+NT(13)+T(20)+NT(13)+NT(56)+NT(13)+T(21)));
//G414: __N_17(422)          => 'b' 'v'.
	p(NT(422), (T(25)+T(41)));
//G415: __E_typed_127(250)   => ~( __N_17(422) ) & type(54).	 # conjunctive
	p(NT(250), ~(NT(422)) & (NT(54)));
//G416: typed(37)            => _(13) ':' _(13) __E_typed_127(250).
	p(NT(37), (NT(13)+T(2)+NT(13)+NT(250)));
//G417: type(54)             => chars(45).
	p(NT(54), (NT(45)));
//G418: subtype(56)          => num(144).
	p(NT(56), (NT(144)));
//G419: __E_comment_128(252) => printable(8).
	p(NT(252), (NT(8)));
//G420: __E_comment_128(252) => '\t'.
	p(NT(252), (T(50)));
//G421: __E_comment_129(253) => null.
	p(NT(253), (nul));
//G422: __E_comment_129(253) => __E_comment_128(252) __E_comment_129(253).
	p(NT(253), (NT(252)+NT(253)));
//G423: __E_comment_130(254) => '\n'.
	p(NT(254), (T(51)));
//G424: __E_comment_130(254) => '\r'.
	p(NT(254), (T(52)));
//G425: __E_comment_130(254) => eof(1).
	p(NT(254), (NT(1)));
//G426: comment(251)         => '#' __E_comment_129(253) __E_comment_130(254).
	p(NT(251), (T(53)+NT(253)+NT(254)));
//G427: __(38)               => space(2).
	p(NT(38), (NT(2)));
//G428: __(38)               => comment(251).
	p(NT(38), (NT(251)));
//G429: __(38)               => __(38) space(2).
	p(NT(38), (NT(38)+NT(2)));
//G430: __(38)               => __(38) comment(251).
	p(NT(38), (NT(38)+NT(251)));
//G431: _(13)                => __(38).
	p(NT(13), (NT(38)));
//G432: _(13)                => null.
	p(NT(13), (nul));
//G433: cli(255)             => _(13).
	p(NT(255), (NT(13)));
//G434: __E_cli_131(257)     => '.' _(13) cli_command(256) _(13).
	p(NT(257), (T(1)+NT(13)+NT(256)+NT(13)));
//G435: __E_cli_132(258)     => null.
	p(NT(258), (nul));
//G436: __E_cli_132(258)     => __E_cli_131(257) __E_cli_132(258).
	p(NT(258), (NT(257)+NT(258)));
//G437: __E_cli_133(259)     => '.' _(13).
	p(NT(259), (T(1)+NT(13)));
//G438: __E_cli_133(259)     => null.
	p(NT(259), (nul));
//G439: cli(255)             => _(13) cli_command(256) _(13) __E_cli_132(258) __E_cli_133(259).
	p(NT(255), (NT(13)+NT(256)+NT(13)+NT(258)+NT(259)));
//G440: __E_cli_command_134(262) => 'q'.
	p(NT(262), (T(54)));
//G441: __E_cli_command_134(262) => 'q' 'u' 'i' 't'.
	p(NT(262), (T(54)+T(7)+T(4)+T(8)));
//G442: quit_sym(261)        => __E_cli_command_134(262).
	p(NT(261), (NT(262)));
//G443: quit_cmd(260)        => quit_sym(261).
	p(NT(260), (NT(261)));
//G444: cli_command(256)     => quit_cmd(260).
	p(NT(256), (NT(260)));
//G445: __E_cli_command_135(265) => 'v'.
	p(NT(265), (T(41)));
//G446: __E_cli_command_135(265) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(265), (T(41)+T(11)+T(27)+T(15)+T(4)+T(6)+T(5)));
//G447: version_sym(264)     => __E_cli_command_135(265).
	p(NT(264), (NT(265)));
//G448: version_cmd(263)     => version_sym(264).
	p(NT(263), (NT(264)));
//G449: cli_command(256)     => version_cmd(263).
	p(NT(256), (NT(263)));
//G450: __E_cli_command_136(268) => 'c'.
	p(NT(268), (T(14)));
//G451: __E_cli_command_136(268) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(268), (T(14)+T(10)+T(11)+T(24)+T(27)));
//G452: clear_sym(267)       => __E_cli_command_136(268).
	p(NT(267), (NT(268)));
//G453: clear_cmd(266)       => clear_sym(267).
	p(NT(266), (NT(267)));
//G454: cli_command(256)     => clear_cmd(266).
	p(NT(256), (NT(266)));
//G455: __E___E_cli_command_137_138(272) => 'h'.
	p(NT(272), (T(55)));
//G456: __E___E_cli_command_137_138(272) => 'h' 'e' 'l' 'p'.
	p(NT(272), (T(55)+T(11)+T(10)+T(18)));
//G457: help_sym(271)        => __E___E_cli_command_137_138(272).
	p(NT(271), (NT(272)));
//G458: __E___E_cli_command_137_139(273) => __(38) help_arg(274).
	p(NT(273), (NT(38)+NT(274)));
//G459: __E___E_cli_command_137_139(273) => null.
	p(NT(273), (nul));
//G460: __E_cli_command_137(270) => help_sym(271) __E___E_cli_command_137_139(273).
	p(NT(270), (NT(271)+NT(273)));
//G461: help_cmd(269)        => __E_cli_command_137(270).
	p(NT(269), (NT(270)));
//G462: cli_command(256)     => help_cmd(269).
	p(NT(256), (NT(269)));
//G463: file_sym(277)        => 'f' 'i' 'l' 'e'.
	p(NT(277), (T(9)+T(4)+T(10)+T(11)));
//G464: __E_cli_command_140(276) => file_sym(277) __(38) q_string(278).
	p(NT(276), (NT(277)+NT(38)+NT(278)));
//G465: file_cmd(275)        => __E_cli_command_140(276).
	p(NT(275), (NT(276)));
//G466: cli_command(256)     => file_cmd(275).
	p(NT(256), (NT(275)));
//G467: valid_sym(281)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(281), (T(41)+T(24)+T(10)+T(4)+T(56)));
//G468: __E_cli_command_141(280) => valid_sym(281) __(38) spec_arg(282).
	p(NT(280), (NT(281)+NT(38)+NT(282)));
//G469: valid_cmd(279)       => __E_cli_command_141(280).
	p(NT(279), (NT(280)));
//G470: cli_command(256)     => valid_cmd(279).
	p(NT(256), (NT(279)));
//G471: sat_sym(285)         => 's' 'a' 't'.
	p(NT(285), (T(15)+T(24)+T(8)));
//G472: __E_cli_command_142(284) => sat_sym(285) __(38) spec_arg(282).
	p(NT(284), (NT(285)+NT(38)+NT(282)));
//G473: sat_cmd(283)         => __E_cli_command_142(284).
	p(NT(283), (NT(284)));
//G474: cli_command(256)     => sat_cmd(283).
	p(NT(256), (NT(283)));
//G475: unsat_sym(288)       => 'u' 'n' 's' 'a' 't'.
	p(NT(288), (T(7)+T(5)+T(15)+T(24)+T(8)));
//G476: __E_cli_command_143(287) => unsat_sym(288) __(38) spec_arg(282).
	p(NT(287), (NT(288)+NT(38)+NT(282)));
//G477: unsat_cmd(286)       => __E_cli_command_143(287).
	p(NT(286), (NT(287)));
//G478: cli_command(256)     => unsat_cmd(286).
	p(NT(256), (NT(286)));
//G479: solve_sym(291)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(291), (T(15)+T(6)+T(10)+T(41)+T(11)));
//G480: __E___E_cli_command_144_145(292) => solve_options(293).
	p(NT(292), (NT(293)));
//G481: __E___E_cli_command_144_145(292) => null.
	p(NT(292), (nul));
//G482: __E_cli_command_144(290) => solve_sym(291) __E___E_cli_command_144_145(292) __(38) wff_cmd_arg(294).
	p(NT(290), (NT(291)+NT(292)+NT(38)+NT(294)));
//G483: solve_cmd(289)       => __E_cli_command_144(290).
	p(NT(289), (NT(290)));
//G484: cli_command(256)     => solve_cmd(289).
	p(NT(256), (NT(289)));
//G485: lgrs_sym(297)        => 'l' 'g' 'r' 's'.
	p(NT(297), (T(10)+T(57)+T(27)+T(15)));
//G486: __E_cli_command_146(296) => lgrs_sym(297) __(38) wff_cmd_arg(294).
	p(NT(296), (NT(297)+NT(38)+NT(294)));
//G487: lgrs_cmd(295)        => __E_cli_command_146(296).
	p(NT(295), (NT(296)));
//G488: cli_command(256)     => lgrs_cmd(295).
	p(NT(256), (NT(295)));
//G489: __E___E_cli_command_147_148(301) => 'r'.
	p(NT(301), (T(27)));
//G490: __E___E_cli_command_147_148(301) => 'r' 'u' 'n'.
	p(NT(301), (T(27)+T(7)+T(5)));
//G491: run_sym(300)         => __E___E_cli_command_147_148(301).
	p(NT(300), (NT(301)));
//G492: __E_cli_command_147(299) => run_sym(300) __(38) spec_arg(282).
	p(NT(299), (NT(300)+NT(38)+NT(282)));
//G493: run_cmd(298)         => __E_cli_command_147(299).
	p(NT(298), (NT(299)));
//G494: cli_command(256)     => run_cmd(298).
	p(NT(256), (NT(298)));
//G495: __E___E_cli_command_149_150(305) => 'n'.
	p(NT(305), (T(5)));
//G496: __E___E_cli_command_149_150(305) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(305), (T(5)+T(6)+T(27)+T(28)+T(24)+T(10)+T(4)+T(58)+T(11)));
//G497: normalize_sym(304)   => __E___E_cli_command_149_150(305).
	p(NT(304), (NT(305)));
//G498: __E_cli_command_149(303) => normalize_sym(304) __(38) spec_arg(282).
	p(NT(303), (NT(304)+NT(38)+NT(282)));
//G499: normalize_cmd(302)   => __E_cli_command_149(303).
	p(NT(302), (NT(303)));
//G500: cli_command(256)     => normalize_cmd(302).
	p(NT(256), (NT(302)));
//G501: __E___E_cli_command_151_152(309) => 's'.
	p(NT(309), (T(15)));
//G502: __E___E_cli_command_151_152(309) => 's' 'u' 'b' 's' 't'.
	p(NT(309), (T(15)+T(7)+T(25)+T(15)+T(8)));
//G503: __E___E_cli_command_151_152(309) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(309), (T(15)+T(7)+T(25)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G504: subst_sym(308)       => __E___E_cli_command_151_152(309).
	p(NT(308), (NT(309)));
//G505: __E_cli_command_151(307) => subst_sym(308) __(38) nf_cmd_arg(310) _(13) '[' _(13) nf_cmd_arg(310) _(13) '/' _(13) nf_cmd_arg(310) _(13) ']'.
	p(NT(307), (NT(308)+NT(38)+NT(310)+NT(13)+T(20)+NT(13)+NT(310)+NT(13)+T(16)+NT(13)+NT(310)+NT(13)+T(21)));
//G506: subst_cmd(306)       => __E_cli_command_151(307).
	p(NT(306), (NT(307)));
//G507: cli_command(256)     => subst_cmd(306).
	p(NT(256), (NT(306)));
//G508: __E___E_cli_command_153_154(314) => 'i'.
	p(NT(314), (T(4)));
//G509: __E___E_cli_command_153_154(314) => 'i' 'n' 's' 't'.
	p(NT(314), (T(4)+T(5)+T(15)+T(8)));
//G510: __E___E_cli_command_153_154(314) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(314), (T(4)+T(5)+T(15)+T(8)+T(24)+T(5)+T(8)+T(4)+T(24)+T(8)+T(11)));
//G511: inst_sym(313)        => __E___E_cli_command_153_154(314).
	p(NT(313), (NT(314)));
//G512: __E_cli_command_153(312) => inst_sym(313) __(38) inst_args(315).
	p(NT(312), (NT(313)+NT(38)+NT(315)));
//G513: inst_cmd(311)        => __E_cli_command_153(312).
	p(NT(311), (NT(312)));
//G514: cli_command(256)     => inst_cmd(311).
	p(NT(256), (NT(311)));
//G515: dnf_sym(318)         => 'd' 'n' 'f'.
	p(NT(318), (T(56)+T(5)+T(9)));
//G516: __E_cli_command_155(317) => dnf_sym(318) __(38) nf_cmd_arg(310).
	p(NT(317), (NT(318)+NT(38)+NT(310)));
//G517: dnf_cmd(316)         => __E_cli_command_155(317).
	p(NT(316), (NT(317)));
//G518: cli_command(256)     => dnf_cmd(316).
	p(NT(256), (NT(316)));
//G519: cnf_sym(321)         => 'c' 'n' 'f'.
	p(NT(321), (T(14)+T(5)+T(9)));
//G520: __E_cli_command_156(320) => cnf_sym(321) __(38) nf_cmd_arg(310).
	p(NT(320), (NT(321)+NT(38)+NT(310)));
//G521: cnf_cmd(319)         => __E_cli_command_156(320).
	p(NT(319), (NT(320)));
//G522: cli_command(256)     => cnf_cmd(319).
	p(NT(256), (NT(319)));
//G523: nnf_sym(324)         => 'n' 'n' 'f'.
	p(NT(324), (T(5)+T(5)+T(9)));
//G524: __E_cli_command_157(323) => nnf_sym(324) __(38) nf_cmd_arg(310).
	p(NT(323), (NT(324)+NT(38)+NT(310)));
//G525: nnf_cmd(322)         => __E_cli_command_157(323).
	p(NT(322), (NT(323)));
//G526: cli_command(256)     => nnf_cmd(322).
	p(NT(256), (NT(322)));
//G527: mnf_sym(327)         => 'm' 'n' 'f'.
	p(NT(327), (T(28)+T(5)+T(9)));
//G528: __E_cli_command_158(326) => mnf_sym(327) __(38) nf_cmd_arg(310).
	p(NT(326), (NT(327)+NT(38)+NT(310)));
//G529: mnf_cmd(325)         => __E_cli_command_158(326).
	p(NT(325), (NT(326)));
//G530: cli_command(256)     => mnf_cmd(325).
	p(NT(256), (NT(325)));
//G531: onf_sym(330)         => 'o' 'n' 'f'.
	p(NT(330), (T(6)+T(5)+T(9)));
//G532: __E_cli_command_159(329) => onf_sym(330) __(38) variable(136) __(38) onf_cmd_arg(331).
	p(NT(329), (NT(330)+NT(38)+NT(136)+NT(38)+NT(331)));
//G533: onf_cmd(328)         => __E_cli_command_159(329).
	p(NT(328), (NT(329)));
//G534: cli_command(256)     => onf_cmd(328).
	p(NT(256), (NT(328)));
//G535: qelim_sym(334)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(334), (T(54)+T(11)+T(10)+T(4)+T(28)));
//G536: __E_cli_command_160(333) => qelim_sym(334) __(38) wff_cmd_arg(294).
	p(NT(333), (NT(334)+NT(38)+NT(294)));
//G537: qelim_cmd(332)       => __E_cli_command_160(333).
	p(NT(332), (NT(333)));
//G538: cli_command(256)     => qelim_cmd(332).
	p(NT(256), (NT(332)));
//G539: get_sym(337)         => 'g' 'e' 't'.
	p(NT(337), (T(57)+T(11)+T(8)));
//G540: __E___E_cli_command_161_162(338) => __(38) option_name(339).
	p(NT(338), (NT(38)+NT(339)));
//G541: __E___E_cli_command_161_162(338) => null.
	p(NT(338), (nul));
//G542: __E_cli_command_161(336) => get_sym(337) __E___E_cli_command_161_162(338).
	p(NT(336), (NT(337)+NT(338)));
//G543: get_cmd(335)         => __E_cli_command_161(336).
	p(NT(335), (NT(336)));
//G544: cli_command(256)     => get_cmd(335).
	p(NT(256), (NT(335)));
//G545: set_sym(342)         => 's' 'e' 't'.
	p(NT(342), (T(15)+T(11)+T(8)));
//G546: __E___E_cli_command_163_164(343) => __(38).
	p(NT(343), (NT(38)));
//G547: __E___E_cli_command_163_164(343) => _(13) '=' _(13).
	p(NT(343), (NT(13)+T(3)+NT(13)));
//G548: __E_cli_command_163(341) => set_sym(342) __(38) option_name(339) __E___E_cli_command_163_164(343) option_value(344).
	p(NT(341), (NT(342)+NT(38)+NT(339)+NT(343)+NT(344)));
//G549: set_cmd(340)         => __E_cli_command_163(341).
	p(NT(340), (NT(341)));
//G550: cli_command(256)     => set_cmd(340).
	p(NT(256), (NT(340)));
//G551: enable_sym(347)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(347), (T(11)+T(5)+T(24)+T(25)+T(10)+T(11)));
//G552: __E_cli_command_165(346) => enable_sym(347) __(38) option_name(339).
	p(NT(346), (NT(347)+NT(38)+NT(339)));
//G553: enable_cmd(345)      => __E_cli_command_165(346).
	p(NT(345), (NT(346)));
//G554: cli_command(256)     => enable_cmd(345).
	p(NT(256), (NT(345)));
//G555: disable_sym(350)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(350), (T(56)+T(4)+T(15)+T(24)+T(25)+T(10)+T(11)));
//G556: __E_cli_command_166(349) => disable_sym(350) __(38) option_name(339).
	p(NT(349), (NT(350)+NT(38)+NT(339)));
//G557: disable_cmd(348)     => __E_cli_command_166(349).
	p(NT(348), (NT(349)));
//G558: cli_command(256)     => disable_cmd(348).
	p(NT(256), (NT(348)));
//G559: toggle_sym(353)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(353), (T(8)+T(6)+T(57)+T(57)+T(10)+T(11)));
//G560: __E_cli_command_167(352) => toggle_sym(353) __(38) option_name(339).
	p(NT(352), (NT(353)+NT(38)+NT(339)));
//G561: toggle_cmd(351)      => __E_cli_command_167(352).
	p(NT(351), (NT(352)));
//G562: cli_command(256)     => toggle_cmd(351).
	p(NT(256), (NT(351)));
//G563: __E___E_cli_command_168_169(357) => 'd' 'e' 'f' 's'.
	p(NT(357), (T(56)+T(11)+T(9)+T(15)));
//G564: __E___E_cli_command_168_169(357) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(357), (T(56)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G565: def_sym(356)         => __E___E_cli_command_168_169(357).
	p(NT(356), (NT(357)));
//G566: __E_cli_command_168(355) => def_sym(356).
	p(NT(355), (NT(356)));
//G567: def_list_cmd(354)    => __E_cli_command_168(355).
	p(NT(354), (NT(355)));
//G568: cli_command(256)     => def_list_cmd(354).
	p(NT(256), (NT(354)));
//G569: __E_cli_command_170(359) => def_sym(356) __(38) num(144).
	p(NT(359), (NT(356)+NT(38)+NT(144)));
//G570: def_print_cmd(358)   => __E_cli_command_170(359).
	p(NT(358), (NT(359)));
//G571: cli_command(256)     => def_print_cmd(358).
	p(NT(256), (NT(358)));
//G572: def_rr_cmd(360)      => rec_relation(19).
	p(NT(360), (NT(19)));
//G573: cli_command(256)     => def_rr_cmd(360).
	p(NT(256), (NT(360)));
//G574: def_input_cmd(361)   => input_def(20).
	p(NT(361), (NT(20)));
//G575: cli_command(256)     => def_input_cmd(361).
	p(NT(256), (NT(361)));
//G576: def_output_cmd(362)  => output_def(21).
	p(NT(362), (NT(21)));
//G577: cli_command(256)     => def_output_cmd(362).
	p(NT(256), (NT(362)));
//G578: def_type_cmd(363)    => type_def(22).
	p(NT(363), (NT(22)));
//G579: cli_command(256)     => def_type_cmd(363).
	p(NT(256), (NT(363)));
//G580: __E___E_cli_command_171_172(367) => 'h' 'i' 's' 't'.
	p(NT(367), (T(55)+T(4)+T(15)+T(8)));
//G581: __E___E_cli_command_171_172(367) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(367), (T(55)+T(4)+T(15)+T(8)+T(6)+T(27)+T(17)));
//G582: history_sym(366)     => __E___E_cli_command_171_172(367).
	p(NT(366), (NT(367)));
//G583: __E_cli_command_171(365) => history_sym(366).
	p(NT(365), (NT(366)));
//G584: history_list_cmd(364) => __E_cli_command_171(365).
	p(NT(364), (NT(365)));
//G585: cli_command(256)     => history_list_cmd(364).
	p(NT(256), (NT(364)));
//G586: __E_cli_command_173(369) => history_sym(366) __(38) history(370).
	p(NT(369), (NT(366)+NT(38)+NT(370)));
//G587: history_print_cmd(368) => __E_cli_command_173(369).
	p(NT(368), (NT(369)));
//G588: cli_command(256)     => history_print_cmd(368).
	p(NT(256), (NT(368)));
//G589: __E_cli_command_174(372) => ref(29).
	p(NT(372), (NT(29)));
//G590: __E_cli_command_174(372) => wff(15).
	p(NT(372), (NT(15)));
//G591: __E_cli_command_174(372) => bf(32).
	p(NT(372), (NT(32)));
//G592: history_store_cmd(371) => __E_cli_command_174(372).
	p(NT(371), (NT(372)));
//G593: cli_command(256)     => history_store_cmd(371).
	p(NT(256), (NT(371)));
//G594: __E_solve_options_175(373) => __(38) solve_option(374).
	p(NT(373), (NT(38)+NT(374)));
//G595: __E_solve_options_176(375) => null.
	p(NT(375), (nul));
//G596: __E_solve_options_176(375) => __E_solve_options_175(373) __E_solve_options_176(375).
	p(NT(375), (NT(373)+NT(375)));
//G597: solve_options(293)   => __E_solve_options_176(375).
	p(NT(293), (NT(375)));
//G598: __E_solve_option_177(376) => solver_mode(377).
	p(NT(376), (NT(377)));
//G599: __E_solve_option_177(376) => type(54).
	p(NT(376), (NT(54)));
//G600: solve_option(374)    => '-' '-' __E_solve_option_177(376).
	p(NT(374), (T(34)+T(34)+NT(376)));
//G601: __E_solver_mode_178(379) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(379), (T(28)+T(4)+T(5)+T(4)+T(28)+T(7)+T(28)));
//G602: __E_solver_mode_178(379) => 'm' 'i' 'n'.
	p(NT(379), (T(28)+T(4)+T(5)));
//G603: solver_mode_minimum(378) => __E_solver_mode_178(379).
	p(NT(378), (NT(379)));
//G604: solver_mode(377)     => solver_mode_minimum(378).
	p(NT(377), (NT(378)));
//G605: __E_solver_mode_179(381) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(381), (T(28)+T(24)+T(33)+T(4)+T(28)+T(7)+T(28)));
//G606: __E_solver_mode_179(381) => 'm' 'a' 'x'.
	p(NT(381), (T(28)+T(24)+T(33)));
//G607: solver_mode_maximum(380) => __E_solver_mode_179(381).
	p(NT(380), (NT(381)));
//G608: solver_mode(377)     => solver_mode_maximum(380).
	p(NT(377), (NT(380)));
//G609: q_file_name(42)      => '"' file_name(382) '"'.
	p(NT(42), (T(59)+NT(382)+T(59)));
//G610: __E_file_name_180(383) => printable(8).
	p(NT(383), (NT(8)));
//G611: __E_file_name_180(383) => printable(8) __E_file_name_180(383).
	p(NT(383), (NT(8)+NT(383)));
//G612: file_name(382)       => __E_file_name_180(383).
	p(NT(382), (NT(383)));
//G613: __E_option_name_181(384) => alnum(6).
	p(NT(384), (NT(6)));
//G614: __E_option_name_181(384) => alnum(6) __E_option_name_181(384).
	p(NT(384), (NT(6)+NT(384)));
//G615: option_name(339)     => __E_option_name_181(384).
	p(NT(339), (NT(384)));
//G616: __E_option_value_182(385) => alnum(6).
	p(NT(385), (NT(6)));
//G617: __E_option_value_182(385) => alnum(6) __E_option_value_182(385).
	p(NT(385), (NT(6)+NT(385)));
//G618: option_value(344)    => __E_option_value_182(385).
	p(NT(344), (NT(385)));
//G619: bf_cmd_arg(386)      => history(370).
	p(NT(386), (NT(370)));
//G620: bf_cmd_arg(386)      => bf(32).
	p(NT(386), (NT(32)));
//G621: wff_cmd_arg(294)     => history(370).
	p(NT(294), (NT(370)));
//G622: wff_cmd_arg(294)     => wff(15).
	p(NT(294), (NT(15)));
//G623: nf_cmd_arg(310)      => history(370).
	p(NT(310), (NT(370)));
//G624: nf_cmd_arg(310)      => ref(29).
	p(NT(310), (NT(29)));
//G625: nf_cmd_arg(310)      => wff(15).
	p(NT(310), (NT(15)));
//G626: nf_cmd_arg(310)      => bf(32).
	p(NT(310), (NT(32)));
//G627: onf_cmd_arg(331)     => history(370).
	p(NT(331), (NT(370)));
//G628: onf_cmd_arg(331)     => wff(15).
	p(NT(331), (NT(15)));
//G629: spec_arg(282)        => history(370).
	p(NT(282), (NT(370)));
//G630: spec_arg(282)        => spec(10).
	p(NT(282), (NT(10)));
//G631: spec_arg(282)        => ref(29).
	p(NT(282), (NT(29)));
//G632: spec_arg(282)        => wff(15).
	p(NT(282), (NT(15)));
//G633: spec_arg(282)        => bf(32).
	p(NT(282), (NT(32)));
//G634: inst_args(315)       => wff_cmd_arg(294) _(13) '[' _(13) bf(32) _(13) '/' _(13) bf_cmd_arg(386) _(13) ']'.
	p(NT(315), (NT(294)+NT(13)+T(20)+NT(13)+NT(32)+NT(13)+T(16)+NT(13)+NT(386)+NT(13)+T(21)));
//G635: inst_args(315)       => bf_cmd_arg(386) _(13) '[' _(13) bf(32) _(13) '/' _(13) bf_cmd_arg(386) _(13) ']'.
	p(NT(315), (NT(386)+NT(13)+T(20)+NT(13)+NT(32)+NT(13)+T(16)+NT(13)+NT(386)+NT(13)+T(21)));
//G636: help_arg(274)        => help_sym(271).
	p(NT(274), (NT(271)));
//G637: help_arg(274)        => version_sym(264).
	p(NT(274), (NT(264)));
//G638: help_arg(274)        => quit_sym(261).
	p(NT(274), (NT(261)));
//G639: help_arg(274)        => clear_sym(267).
	p(NT(274), (NT(267)));
//G640: help_arg(274)        => get_sym(337).
	p(NT(274), (NT(337)));
//G641: help_arg(274)        => set_sym(342).
	p(NT(274), (NT(342)));
//G642: help_arg(274)        => enable_sym(347).
	p(NT(274), (NT(347)));
//G643: help_arg(274)        => disable_sym(350).
	p(NT(274), (NT(350)));
//G644: help_arg(274)        => toggle_sym(353).
	p(NT(274), (NT(353)));
//G645: help_arg(274)        => file_sym(277).
	p(NT(274), (NT(277)));
//G646: help_arg(274)        => history_sym(366).
	p(NT(274), (NT(366)));
//G647: help_arg(274)        => abs_history_sym(387).
	p(NT(274), (NT(387)));
//G648: help_arg(274)        => rel_history_sym(388).
	p(NT(274), (NT(388)));
//G649: help_arg(274)        => selection_sym(389).
	p(NT(274), (NT(389)));
//G650: help_arg(274)        => def_sym(356).
	p(NT(274), (NT(356)));
//G651: help_arg(274)        => inst_sym(313).
	p(NT(274), (NT(313)));
//G652: help_arg(274)        => subst_sym(308).
	p(NT(274), (NT(308)));
//G653: help_arg(274)        => normalize_sym(304).
	p(NT(274), (NT(304)));
//G654: help_arg(274)        => execute_sym(390).
	p(NT(274), (NT(390)));
//G655: help_arg(274)        => solve_sym(291).
	p(NT(274), (NT(291)));
//G656: help_arg(274)        => valid_sym(281).
	p(NT(274), (NT(281)));
//G657: help_arg(274)        => sat_sym(285).
	p(NT(274), (NT(285)));
//G658: help_arg(274)        => unsat_sym(288).
	p(NT(274), (NT(288)));
//G659: help_arg(274)        => run_sym(300).
	p(NT(274), (NT(300)));
//G660: help_arg(274)        => dnf_sym(318).
	p(NT(274), (NT(318)));
//G661: help_arg(274)        => cnf_sym(321).
	p(NT(274), (NT(321)));
//G662: help_arg(274)        => snf_sym(391).
	p(NT(274), (NT(391)));
//G663: help_arg(274)        => nnf_sym(324).
	p(NT(274), (NT(324)));
//G664: help_arg(274)        => mnf_sym(327).
	p(NT(274), (NT(327)));
//G665: help_arg(274)        => onf_sym(330).
	p(NT(274), (NT(330)));
//G666: help_arg(274)        => qelim_sym(334).
	p(NT(274), (NT(334)));
//G667: __E___E_help_arg_183_184(394) => 's'.
	p(NT(394), (T(15)));
//G668: __E___E_help_arg_183_184(394) => null.
	p(NT(394), (nul));
//G669: __E_help_arg_183(393) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_183_184(394).
	p(NT(393), (T(11)+T(33)+T(24)+T(28)+T(18)+T(10)+T(11)+NT(394)));
//G670: examples_sym(392)    => __E_help_arg_183(393).
	p(NT(392), (NT(393)));
//G671: help_arg(274)        => examples_sym(392).
	p(NT(274), (NT(392)));
//G672: __E_history_185(396) => '%'.
	p(NT(396), (T(43)));
//G673: rel_history_sym(388) => '%' '-'.
	p(NT(388), (T(43)+T(34)));
//G674: history_id(398)      => digits(247).
	p(NT(398), (NT(247)));
//G675: __E___E_history_185_186(397) => history_id(398).
	p(NT(397), (NT(398)));
//G676: __E___E_history_185_186(397) => null.
	p(NT(397), (nul));
//G677: __E_history_185(396) => rel_history_sym(388) __E___E_history_185_186(397).
	p(NT(396), (NT(388)+NT(397)));
//G678: rel_history(395)     => __E_history_185(396).
	p(NT(395), (NT(396)));
//G679: history(370)         => rel_history(395).
	p(NT(370), (NT(395)));
//G680: abs_history_sym(387) => '%'.
	p(NT(387), (T(43)));
//G681: __E_history_187(400) => abs_history_sym(387) history_id(398).
	p(NT(400), (NT(387)+NT(398)));
//G682: abs_history(399)     => __E_history_187(400).
	p(NT(399), (NT(400)));
//G683: history(370)         => abs_history(399).
	p(NT(370), (NT(399)));
//G684: unreachable(401)     => BDD_ID(402).
	p(NT(401), (NT(402)));
//G685: stream_def(403)      => input_def(20).
	p(NT(403), (NT(20)));
//G686: stream_def(403)      => output_def(21).
	p(NT(403), (NT(21)));
//G687: fm_or_term(404)      => wff(15).
	p(NT(404), (NT(15)));
//G688: fm_or_term(404)      => bf(32).
	p(NT(404), (NT(32)));
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
		spec, __E_spec_0, definitions, _, main, wff, __E_spec_1, __E_definitions_2, __E___E_definitions_2_3, rec_relation, 
		input_def, output_def, type_def, __E_definitions_4, spec_multiline, __E_spec_multiline_5, spec_part, __E_spec_multiline_6, __E_spec_multiline_7, ref, 
		__E_rec_relation_8, capture, bf, io_var_name, __E_input_def_9, member_path, __E_input_def_10, typed, __, stream, 
		__E_output_def_11, __E_output_def_12, q_file_name, console_sym, type_name, chars, __E_type_def_13, __E___E_type_def_13_14, type_parents, __E_type_def_15, 
		type_body, __E_type_parents_16, __E_type_parents_17, tuple, type, __E_type_body_18, subtype, member, __E_tuple_19, member_name, 
		__E_tuple_20, __E_tuple_21, __E_member_path_22, __E_member_path_23, sym, __E_ref_24, offsets, ref_args, __E_ref_25, __E_ref_26, 
		fp_fallback, first_sym, last_sym, __E_ref_args_27, __E___E_ref_args_27_28, ref_arg, __E___E_ref_args_27_29, __E___E_ref_args_27_30, wff_parenthesis, __E_wff_31, 
		wff_sometimes, __E_wff_32, __E___E_wff_32_33, wff_always, __E_wff_34, __E___E_wff_34_35, wff_conditional, __E_wff_36, wff_all, __E_wff_37, 
		q_vars, wff_ex, __E_wff_38, wff_ref, constraint, wff_imply, __E_wff_39, wff_rimply, __E_wff_40, wff_equiv, 
		__E_wff_41, wff_or, __E_wff_42, wff_xor, __E_wff_43, wff_and, __E_wff_44, wff_neg, __E_wff_45, wff_t, 
		wff_f, bf_interval, __E_wff_46, bf_eq, __E_wff_47, bf_neq, __E_wff_48, bf_lt, __E_wff_49, bf_nlt, 
		__E_wff_50, bf_lteq, __E_wff_51, bf_nlteq, __E_wff_52, bf_gt, __E_wff_53, bf_ngt, __E_wff_54, bf_gteq, 
		__E_wff_55, bf_ngteq, __E_wff_56, bf_parenthesis, __E_bf_57, ba_constant, variable, bf_fall, __E_bf_58, bf_fex, 
		__E_bf_59, bf_ref, bf_cast, __E_bf_60, num, bf_cast_oprnd, __E___E_bf_60_61, bf_t, bf_f, bf_neg, 
		bf_shr, __E_bf_62, __E___E_bf_62_63, bf_shl, __E_bf_64, __E___E_bf_64_65, bf_add, __E_bf_66, bf_sub, __E_bf_67, 
		__E___E_bf_67_68, bf_mod, __E_bf_69, __E___E_bf_69_70, bf_mul, __E_bf_71, bf_div, __E_bf_72, __E___E_bf_72_73, bf_nor, 
		__E_bf_74, __E___E_bf_74_75, bf_xnor, __E_bf_76, __E___E_bf_76_77, bf_nand, __E_bf_78, __E___E_bf_78_79, bf_or, __E_bf_80, 
		bf_xor, __E_bf_81, __E_bf_82, __E___E_bf_82_83, __E_bf_84, __E___E_bf_84_85, bf_and, __E_bf_86, __E___E_bf_86_87, __E_bf_88, 
		bf_neg_oprnd, __E___E_bf_88_89, bf_and_nosep, __E_bf_90, bf_and_nosep_1st_oprnd, __E___E_bf_90_91, bf_and_nosep_2nd_oprnd, __E___E_bf_90_92, ctn_neq, __E_constraint_93, 
		ctnvar, ctn_eq, __E_constraint_94, ctn_gteq, __E_constraint_95, ctn_gt, __E_constraint_96, ctn_lteq, __E_constraint_97, ctn_lt, 
		__E_constraint_98, __E_ba_constant_99, source, __E_ba_constant_100, src_c, __E_source_101, __E___E_source_101_102, __E___E_source_101_103, __E_src_c_104, __E_src_c_105, 
		__E_variable_106, uconst, io_var, var_name, __E_variable_107, __E_variable_108, __E_var_name_109, __E_var_name_110, __E_var_name_111, __E_var_name_112, 
		__E_var_name_113, offset, uconst_name, __E_uconst_114, __E___E_uconst_114_115, q_var, __E_q_vars_116, __E_q_vars_117, __E_offsets_118, __E_offsets_119, 
		integer, shift, __E_offset_120, __E_shift_121, __E___E_shift_121_122, __E_chars_123, __E_chars_124, digits, __E_digits_125, __E_integer_126, 
		__E_typed_127, comment, __E_comment_128, __E_comment_129, __E_comment_130, cli, cli_command, __E_cli_131, __E_cli_132, __E_cli_133, 
		quit_cmd, quit_sym, __E_cli_command_134, version_cmd, version_sym, __E_cli_command_135, clear_cmd, clear_sym, __E_cli_command_136, help_cmd, 
		__E_cli_command_137, help_sym, __E___E_cli_command_137_138, __E___E_cli_command_137_139, help_arg, file_cmd, __E_cli_command_140, file_sym, q_string, valid_cmd, 
		__E_cli_command_141, valid_sym, spec_arg, sat_cmd, __E_cli_command_142, sat_sym, unsat_cmd, __E_cli_command_143, unsat_sym, solve_cmd, 
		__E_cli_command_144, solve_sym, __E___E_cli_command_144_145, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_146, lgrs_sym, run_cmd, __E_cli_command_147, 
		run_sym, __E___E_cli_command_147_148, normalize_cmd, __E_cli_command_149, normalize_sym, __E___E_cli_command_149_150, subst_cmd, __E_cli_command_151, subst_sym, __E___E_cli_command_151_152, 
		nf_cmd_arg, inst_cmd, __E_cli_command_153, inst_sym, __E___E_cli_command_153_154, inst_args, dnf_cmd, __E_cli_command_155, dnf_sym, cnf_cmd, 
		__E_cli_command_156, cnf_sym, nnf_cmd, __E_cli_command_157, nnf_sym, mnf_cmd, __E_cli_command_158, mnf_sym, onf_cmd, __E_cli_command_159, 
		onf_sym, onf_cmd_arg, qelim_cmd, __E_cli_command_160, qelim_sym, get_cmd, __E_cli_command_161, get_sym, __E___E_cli_command_161_162, option_name, 
		set_cmd, __E_cli_command_163, set_sym, __E___E_cli_command_163_164, option_value, enable_cmd, __E_cli_command_165, enable_sym, disable_cmd, __E_cli_command_166, 
		disable_sym, toggle_cmd, __E_cli_command_167, toggle_sym, def_list_cmd, __E_cli_command_168, def_sym, __E___E_cli_command_168_169, def_print_cmd, __E_cli_command_170, 
		def_rr_cmd, def_input_cmd, def_output_cmd, def_type_cmd, history_list_cmd, __E_cli_command_171, history_sym, __E___E_cli_command_171_172, history_print_cmd, __E_cli_command_173, 
		history, history_store_cmd, __E_cli_command_174, __E_solve_options_175, solve_option, __E_solve_options_176, __E_solve_option_177, solver_mode, solver_mode_minimum, __E_solver_mode_178, 
		solver_mode_maximum, __E_solver_mode_179, file_name, __E_file_name_180, __E_option_name_181, __E_option_value_182, bf_cmd_arg, abs_history_sym, rel_history_sym, selection_sym, 
		execute_sym, snf_sym, examples_sym, __E_help_arg_183, __E___E_help_arg_183_184, rel_history, __E_history_185, __E___E_history_185_186, history_id, abs_history, 
		__E_history_187, unreachable, BDD_ID, stream_def, fm_or_term, __N_0, __N_1, __N_2, __N_3, __N_4, 
		__N_5, __N_6, __N_7, __N_8, __N_9, __N_10, __N_11, __N_12, __N_13, __N_14, 
		__N_15, __N_16, __N_17, 
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
