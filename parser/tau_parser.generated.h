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
	"bf", "io_var_name", "__E_input_def_8", "typed", "__", "stream", "__E_output_def_9", "q_file_name", "console_sym", "sym", 
	"__E_ref_10", "offsets", "ref_args", "__E_ref_11", "__E_ref_12", "fp_fallback", "first_sym", "last_sym", "__E_ref_args_13", "__E___E_ref_args_13_14", 
	"ref_arg", "__E___E_ref_args_13_15", "__E___E_ref_args_13_16", "tau_constant_source", "__E_tau_constant_source_17", "wff_parenthesis", "__E_wff_18", "wff_sometimes", "__E_wff_19", "__E___E_wff_19_20", 
	"wff_always", "__E_wff_21", "__E___E_wff_21_22", "wff_conditional", "__E_wff_23", "wff_all", "__E_wff_24", "q_vars", "wff_ex", "__E_wff_25", 
	"wff_ref", "constraint", "wff_imply", "__E_wff_26", "wff_rimply", "__E_wff_27", "wff_equiv", "__E_wff_28", "wff_or", "__E_wff_29", 
	"wff_xor", "__E_wff_30", "wff_and", "__E_wff_31", "wff_neg", "__E_wff_32", "wff_t", "wff_f", "bf_interval", "__E_wff_33", 
	"bf_eq", "__E_wff_34", "bf_neq", "__E_wff_35", "bf_lt", "__E_wff_36", "bf_nlt", "__E_wff_37", "bf_lteq", "__E_wff_38", 
	"bf_nlteq", "__E_wff_39", "bf_gt", "__E_wff_40", "bf_ngt", "__E_wff_41", "bf_gteq", "__E_wff_42", "bf_ngteq", "__E_wff_43", 
	"bf_parenthesis", "__E_bf_44", "ba_constant", "variable", "bf_fall", "__E_bf_45", "bf_fex", "__E_bf_46", "bf_ref", "bf_shr", 
	"__E_bf_47", "__E___E_bf_47_48", "bf_shl", "__E_bf_49", "__E___E_bf_49_50", "bf_add", "__E_bf_51", "bf_sub", "__E_bf_52", "__E___E_bf_52_53", 
	"bf_mod", "__E_bf_54", "__E___E_bf_54_55", "bf_mul", "__E_bf_56", "bf_div", "__E_bf_57", "__E___E_bf_57_58", "bf_nor", "__E_bf_59", 
	"__E___E_bf_59_60", "bf_xnor", "__E_bf_61", "__E___E_bf_61_62", "bf_nand", "__E_bf_63", "__E___E_bf_63_64", "bf_or", "__E_bf_65", "bf_xor", 
	"__E_bf_66", "bf_t", "__E_bf_67", "__E___E_bf_67_68", "bf_f", "__E_bf_69", "__E___E_bf_69_70", "bf_and", "__E_bf_71", "__E___E_bf_71_72", 
	"bf_neg", "__E_bf_73", "bf_neg_oprnd", "__E___E_bf_73_74", "bf_and_nosep", "__E_bf_75", "bf_and_nosep_1st_oprnd", "__E___E_bf_75_76", "bf_and_nosep_2nd_oprnd", "__E___E_bf_75_77", 
	"ctn_neq", "__E_constraint_78", "ctnvar", "num", "ctn_eq", "__E_constraint_79", "ctn_gteq", "__E_constraint_80", "ctn_gt", "__E_constraint_81", 
	"ctn_lteq", "__E_constraint_82", "ctn_lt", "__E_constraint_83", "__E_ba_constant_84", "source", "__E_ba_constant_85", "src_c", "__E_source_86", "__E___E_source_86_87", 
	"__E___E_source_86_88", "__E_src_c_89", "__E_src_c_90", "__E_variable_91", "uconst", "io_var", "var_name", "__E_variable_92", "__E_var_name_93", "__E_var_name_94", 
	"__E_var_name_95", "__E_var_name_96", "__E_var_name_97", "offset", "chars", "uconst_name", "__E_uconst_98", "__E___E_uconst_98_99", "q_var", "__E_q_vars_100", 
	"__E_q_vars_101", "__E_offsets_102", "__E_offsets_103", "integer", "shift", "__E_offset_104", "__E_shift_105", "__E___E_shift_105_106", "__E_chars_107", "__E_chars_108", 
	"digits", "__E_digits_109", "__E_integer_110", "type", "__E_typed_111", "subtype", "comment", "__E_comment_112", "__E_comment_113", "__E_comment_114", 
	"cli", "cli_command", "__E_cli_115", "__E_cli_116", "quit_cmd", "quit_sym", "__E_cli_command_117", "version_cmd", "version_sym", "__E_cli_command_118", 
	"clear_cmd", "clear_sym", "__E_cli_command_119", "help_cmd", "__E_cli_command_120", "help_sym", "__E___E_cli_command_120_121", "__E___E_cli_command_120_122", "help_arg", "file_cmd", 
	"__E_cli_command_123", "file_sym", "q_string", "valid_cmd", "__E_cli_command_124", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_125", "sat_sym", 
	"unsat_cmd", "__E_cli_command_126", "unsat_sym", "solve_cmd", "__E_cli_command_127", "solve_sym", "__E___E_cli_command_127_128", "solve_options", "wff_cmd_arg", "lgrs_cmd", 
	"__E_cli_command_129", "lgrs_sym", "run_cmd", "__E_cli_command_130", "run_sym", "__E___E_cli_command_130_131", "__E___E_cli_command_130_132", "history", "normalize_cmd", "__E_cli_command_133", 
	"normalize_sym", "__E___E_cli_command_133_134", "subst_cmd", "__E_cli_command_135", "subst_sym", "__E___E_cli_command_135_136", "nf_cmd_arg", "inst_cmd", "__E_cli_command_137", "inst_sym", 
	"__E___E_cli_command_137_138", "inst_args", "dnf_cmd", "__E_cli_command_139", "dnf_sym", "cnf_cmd", "__E_cli_command_140", "cnf_sym", "anf_cmd", "__E_cli_command_141", 
	"anf_sym", "nnf_cmd", "__E_cli_command_142", "nnf_sym", "pnf_cmd", "__E_cli_command_143", "pnf_sym", "mnf_cmd", "__E_cli_command_144", "mnf_sym", 
	"onf_cmd", "__E_cli_command_145", "onf_sym", "onf_cmd_arg", "qelim_cmd", "__E_cli_command_146", "qelim_sym", "get_cmd", "__E_cli_command_147", "get_sym", 
	"__E___E_cli_command_147_148", "option_name", "set_cmd", "__E_cli_command_149", "set_sym", "__E___E_cli_command_149_150", "option_value", "enable_cmd", "__E_cli_command_151", "enable_sym", 
	"disable_cmd", "__E_cli_command_152", "disable_sym", "toggle_cmd", "__E_cli_command_153", "toggle_sym", "def_list_cmd", "__E_cli_command_154", "def_sym", "__E___E_cli_command_154_155", 
	"def_print_cmd", "__E_cli_command_156", "def_rr_cmd", "def_input_cmd", "def_output_cmd", "history_list_cmd", "__E_cli_command_157", "history_sym", "__E___E_cli_command_157_158", "history_print_cmd", 
	"__E_cli_command_159", "history_store_cmd", "__E_cli_command_160", "__E_solve_options_161", "solve_option", "__E_solve_options_162", "__E_solve_option_163", "solver_mode", "solver_mode_minimum", "__E_solver_mode_164", 
	"solver_mode_maximum", "__E_solver_mode_165", "file_name", "__E_file_name_166", "__E_option_name_167", "__E_option_value_168", "bf_cmd_arg", "abs_history_sym", "rel_history_sym", "selection_sym", 
	"execute_sym", "snf_sym", "examples_sym", "__E_help_arg_169", "__E___E_help_arg_169_170", "rel_history", "__E_history_171", "__E___E_history_171_172", "history_id", "abs_history", 
	"__E_history_173", "unreachable", "processed", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", 
	"__N_7", "__N_8", "__N_9", "__N_10", "__N_11", "__N_12", "__N_13", "__N_14", "__N_15", "__N_16", 
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
			29, 31, 39, 172, 185, 196, 205, 213, 220, 223,
			225, 321, 326, 362
		},
		.to_inline = {
			{ 14, 55, 14 },
			{ 30, 110, 30 },
			{ 162, 110, 30 },
			{ 164, 110, 157 },
			{ 166, 110, 30 },
			{ 168, 110, 30 },
			{ 187 },
			{ 194 },
			{ 194, 205 },
			{ 204 },
			{ 208 },
			{ 256 },
			{ 268 },
			{ 286 },
			{ 291 },
			{ 313 },
			{ 366 }
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
//G32:  output_def(19)       => io_var_name(31) __E_output_def_9(36) _(11) ':' '=' _(11) 'o' 'u' 't' __(34) stream(35).
	p(NT(19), (NT(31)+NT(36)+NT(11)+T(2)+T(3)+NT(11)+T(6)+T(7)+T(8)+NT(34)+NT(35)));
//G33:  stream(35)           => 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(37) _(11) ')'.
	p(NT(35), (T(9)+T(4)+T(10)+T(11)+NT(11)+T(12)+NT(11)+NT(37)+NT(11)+T(13)));
//G34:  console_sym(38)      => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(38), (T(14)+T(6)+T(5)+T(15)+T(6)+T(10)+T(11)));
//G35:  stream(35)           => console_sym(38).
	p(NT(35), (NT(38)));
//G36:  __E_ref_10(40)       => offsets(41).
	p(NT(40), (NT(41)));
//G37:  __E_ref_10(40)       => null.
	p(NT(40), (nul));
//G38:  __E_ref_11(43)       => typed(33).
	p(NT(43), (NT(33)));
//G39:  __E_ref_11(43)       => null.
	p(NT(43), (nul));
//G40:  __E_ref_12(44)       => _(11) 'f' 'a' 'l' 'l' 'b' 'a' 'c' 'k' __(34) fp_fallback(45).
	p(NT(44), (NT(11)+T(9)+T(16)+T(10)+T(10)+T(17)+T(16)+T(14)+T(18)+NT(34)+NT(45)));
//G41:  __E_ref_12(44)       => null.
	p(NT(44), (nul));
//G42:  ref(27)              => sym(39) __E_ref_10(40) ref_args(42) __E_ref_11(43) __E_ref_12(44).
	p(NT(27), (NT(39)+NT(40)+NT(42)+NT(43)+NT(44)));
//G43:  first_sym(46)        => 'f' 'i' 'r' 's' 't'.
	p(NT(46), (T(9)+T(4)+T(19)+T(15)+T(8)));
//G44:  fp_fallback(45)      => first_sym(46).
	p(NT(45), (NT(46)));
//G45:  last_sym(47)         => 'l' 'a' 's' 't'.
	p(NT(47), (T(10)+T(16)+T(15)+T(8)));
//G46:  fp_fallback(45)      => last_sym(47).
	p(NT(45), (NT(47)));
//G47:  fp_fallback(45)      => capture(29).
	p(NT(45), (NT(29)));
//G48:  fp_fallback(45)      => ref(27).
	p(NT(45), (NT(27)));
//G49:  fp_fallback(45)      => wff(14).
	p(NT(45), (NT(14)));
//G50:  fp_fallback(45)      => bf(30).
	p(NT(45), (NT(30)));
//G51:  ref_arg(50)          => bf(30).
	p(NT(50), (NT(30)));
//G52:  __E___E_ref_args_13_14(49) => _(11) ref_arg(50).
	p(NT(49), (NT(11)+NT(50)));
//G53:  __E___E_ref_args_13_15(51) => _(11) ',' _(11) ref_arg(50).
	p(NT(51), (NT(11)+T(20)+NT(11)+NT(50)));
//G54:  __E___E_ref_args_13_16(52) => null.
	p(NT(52), (nul));
//G55:  __E___E_ref_args_13_16(52) => __E___E_ref_args_13_15(51) __E___E_ref_args_13_16(52).
	p(NT(52), (NT(51)+NT(52)));
//G56:  __E_ref_args_13(48)  => __E___E_ref_args_13_14(49) __E___E_ref_args_13_16(52).
	p(NT(48), (NT(49)+NT(52)));
//G57:  __E_ref_args_13(48)  => null.
	p(NT(48), (nul));
//G58:  ref_args(42)         => '(' __E_ref_args_13(48) _(11) ')'.
	p(NT(42), (T(12)+NT(48)+NT(11)+T(13)));
//G59:  __E_tau_constant_source_17(54) => '.' _(11).
	p(NT(54), (T(1)+NT(11)));
//G60:  __E_tau_constant_source_17(54) => null.
	p(NT(54), (nul));
//G61:  tau_constant_source(53) => rec_relations(24) _(11) main(13) _(11) __E_tau_constant_source_17(54).
	p(NT(53), (NT(24)+NT(11)+NT(13)+NT(11)+NT(54)));
//G62:  __E_wff_18(56)       => '(' _(11) wff(14) _(11) ')'.
	p(NT(56), (T(12)+NT(11)+NT(14)+NT(11)+T(13)));
//G63:  wff_parenthesis(55)  => __E_wff_18(56).
	p(NT(55), (NT(56)));
//G64:  wff(14)              => wff_parenthesis(55).
	p(NT(14), (NT(55)));
//G65:  __E___E_wff_19_20(59) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(59), (T(15)+T(6)+T(21)+T(11)+T(8)+T(4)+T(21)+T(11)+T(15)));
//G66:  __E___E_wff_19_20(59) => '<' '>'.
	p(NT(59), (T(22)+T(23)));
//G67:  __E_wff_19(58)       => __E___E_wff_19_20(59) _(11) wff(14).
	p(NT(58), (NT(59)+NT(11)+NT(14)));
//G68:  wff_sometimes(57)    => __E_wff_19(58).
	p(NT(57), (NT(58)));
//G69:  wff(14)              => wff_sometimes(57).
	p(NT(14), (NT(57)));
//G70:  __E___E_wff_21_22(62) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(62), (T(16)+T(10)+T(24)+T(16)+T(25)+T(15)));
//G71:  __E___E_wff_21_22(62) => '[' ']'.
	p(NT(62), (T(26)+T(27)));
//G72:  __E_wff_21(61)       => __E___E_wff_21_22(62) _(11) wff(14).
	p(NT(61), (NT(62)+NT(11)+NT(14)));
//G73:  wff_always(60)       => __E_wff_21(61).
	p(NT(60), (NT(61)));
//G74:  wff(14)              => wff_always(60).
	p(NT(14), (NT(60)));
//G75:  __E_wff_23(64)       => wff(14) _(11) '?' _(11) wff(14) _(11) ':' _(11) wff(14).
	p(NT(64), (NT(14)+NT(11)+T(28)+NT(11)+NT(14)+NT(11)+T(2)+NT(11)+NT(14)));
//G76:  wff_conditional(63)  => __E_wff_23(64).
	p(NT(63), (NT(64)));
//G77:  wff(14)              => wff_conditional(63).
	p(NT(14), (NT(63)));
//G78:  __E_wff_24(66)       => 'a' 'l' 'l' __(34) q_vars(67) __(34) wff(14).
	p(NT(66), (T(16)+T(10)+T(10)+NT(34)+NT(67)+NT(34)+NT(14)));
//G79:  wff_all(65)          => __E_wff_24(66).
	p(NT(65), (NT(66)));
//G80:  wff(14)              => wff_all(65).
	p(NT(14), (NT(65)));
//G81:  __E_wff_25(69)       => 'e' 'x' __(34) q_vars(67) __(34) wff(14).
	p(NT(69), (T(11)+T(29)+NT(34)+NT(67)+NT(34)+NT(14)));
//G82:  wff_ex(68)           => __E_wff_25(69).
	p(NT(68), (NT(69)));
//G83:  wff(14)              => wff_ex(68).
	p(NT(14), (NT(68)));
//G84:  wff_ref(70)          => ref(27).
	p(NT(70), (NT(27)));
//G85:  wff(14)              => wff_ref(70).
	p(NT(14), (NT(70)));
//G86:  wff(14)              => constraint(71).
	p(NT(14), (NT(71)));
//G87:  __E_wff_26(73)       => wff(14) _(11) '-' '>' _(11) wff(14).
	p(NT(73), (NT(14)+NT(11)+T(30)+T(23)+NT(11)+NT(14)));
//G88:  wff_imply(72)        => __E_wff_26(73).
	p(NT(72), (NT(73)));
//G89:  wff(14)              => wff_imply(72).
	p(NT(14), (NT(72)));
//G90:  __E_wff_27(75)       => wff(14) _(11) '<' '-' _(11) wff(14).
	p(NT(75), (NT(14)+NT(11)+T(22)+T(30)+NT(11)+NT(14)));
//G91:  wff_rimply(74)       => __E_wff_27(75).
	p(NT(74), (NT(75)));
//G92:  wff(14)              => wff_rimply(74).
	p(NT(14), (NT(74)));
//G93:  __E_wff_28(77)       => wff(14) _(11) '<' '-' '>' _(11) wff(14).
	p(NT(77), (NT(14)+NT(11)+T(22)+T(30)+T(23)+NT(11)+NT(14)));
//G94:  wff_equiv(76)        => __E_wff_28(77).
	p(NT(76), (NT(77)));
//G95:  wff(14)              => wff_equiv(76).
	p(NT(14), (NT(76)));
//G96:  __E_wff_29(79)       => wff(14) _(11) '|' '|' _(11) wff(14).
	p(NT(79), (NT(14)+NT(11)+T(31)+T(31)+NT(11)+NT(14)));
//G97:  wff_or(78)           => __E_wff_29(79).
	p(NT(78), (NT(79)));
//G98:  wff(14)              => wff_or(78).
	p(NT(14), (NT(78)));
//G99:  __E_wff_30(81)       => wff(14) _(11) '^' '^' _(11) wff(14).
	p(NT(81), (NT(14)+NT(11)+T(32)+T(32)+NT(11)+NT(14)));
//G100: wff_xor(80)          => __E_wff_30(81).
	p(NT(80), (NT(81)));
//G101: wff(14)              => wff_xor(80).
	p(NT(14), (NT(80)));
//G102: __E_wff_31(83)       => wff(14) _(11) '&' '&' _(11) wff(14).
	p(NT(83), (NT(14)+NT(11)+T(33)+T(33)+NT(11)+NT(14)));
//G103: wff_and(82)          => __E_wff_31(83).
	p(NT(82), (NT(83)));
//G104: wff(14)              => wff_and(82).
	p(NT(14), (NT(82)));
//G105: __E_wff_32(85)       => '!' _(11) wff(14).
	p(NT(85), (T(34)+NT(11)+NT(14)));
//G106: wff_neg(84)          => __E_wff_32(85).
	p(NT(84), (NT(85)));
//G107: wff(14)              => wff_neg(84).
	p(NT(14), (NT(84)));
//G108: wff_t(86)            => 'T'.
	p(NT(86), (T(35)));
//G109: wff(14)              => wff_t(86).
	p(NT(14), (NT(86)));
//G110: wff_f(87)            => 'F'.
	p(NT(87), (T(36)));
//G111: wff(14)              => wff_f(87).
	p(NT(14), (NT(87)));
//G112: __E_wff_33(89)       => bf(30) _(11) '<' '=' _(11) bf(30) _(11) '<' '=' _(11) bf(30).
	p(NT(89), (NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)));
//G113: bf_interval(88)      => __E_wff_33(89).
	p(NT(88), (NT(89)));
//G114: wff(14)              => bf_interval(88).
	p(NT(14), (NT(88)));
//G115: __E_wff_34(91)       => bf(30) _(11) '=' _(11) bf(30).
	p(NT(91), (NT(30)+NT(11)+T(3)+NT(11)+NT(30)));
//G116: bf_eq(90)            => __E_wff_34(91).
	p(NT(90), (NT(91)));
//G117: wff(14)              => bf_eq(90).
	p(NT(14), (NT(90)));
//G118: __E_wff_35(93)       => bf(30) _(11) '!' '=' _(11) bf(30).
	p(NT(93), (NT(30)+NT(11)+T(34)+T(3)+NT(11)+NT(30)));
//G119: bf_neq(92)           => __E_wff_35(93).
	p(NT(92), (NT(93)));
//G120: wff(14)              => bf_neq(92).
	p(NT(14), (NT(92)));
//G121: __E_wff_36(95)       => bf(30) _(11) '<' _(11) bf(30).
	p(NT(95), (NT(30)+NT(11)+T(22)+NT(11)+NT(30)));
//G122: bf_lt(94)            => __E_wff_36(95).
	p(NT(94), (NT(95)));
//G123: wff(14)              => bf_lt(94).
	p(NT(14), (NT(94)));
//G124: __E_wff_37(97)       => bf(30) _(11) '!' '<' _(11) bf(30).
	p(NT(97), (NT(30)+NT(11)+T(34)+T(22)+NT(11)+NT(30)));
//G125: bf_nlt(96)           => __E_wff_37(97).
	p(NT(96), (NT(97)));
//G126: wff(14)              => bf_nlt(96).
	p(NT(14), (NT(96)));
//G127: __E_wff_38(99)       => bf(30) _(11) '<' '=' _(11) bf(30).
	p(NT(99), (NT(30)+NT(11)+T(22)+T(3)+NT(11)+NT(30)));
//G128: bf_lteq(98)          => __E_wff_38(99).
	p(NT(98), (NT(99)));
//G129: wff(14)              => bf_lteq(98).
	p(NT(14), (NT(98)));
//G130: __E_wff_39(101)      => bf(30) _(11) '!' '<' '=' _(11) bf(30).
	p(NT(101), (NT(30)+NT(11)+T(34)+T(22)+T(3)+NT(11)+NT(30)));
//G131: bf_nlteq(100)        => __E_wff_39(101).
	p(NT(100), (NT(101)));
//G132: wff(14)              => bf_nlteq(100).
	p(NT(14), (NT(100)));
//G133: __E_wff_40(103)      => bf(30) _(11) '>' _(11) bf(30).
	p(NT(103), (NT(30)+NT(11)+T(23)+NT(11)+NT(30)));
//G134: bf_gt(102)           => __E_wff_40(103).
	p(NT(102), (NT(103)));
//G135: wff(14)              => bf_gt(102).
	p(NT(14), (NT(102)));
//G136: __E_wff_41(105)      => bf(30) _(11) '!' '>' _(11) bf(30).
	p(NT(105), (NT(30)+NT(11)+T(34)+T(23)+NT(11)+NT(30)));
//G137: bf_ngt(104)          => __E_wff_41(105).
	p(NT(104), (NT(105)));
//G138: wff(14)              => bf_ngt(104).
	p(NT(14), (NT(104)));
//G139: __E_wff_42(107)      => bf(30) _(11) '>' '=' _(11) bf(30).
	p(NT(107), (NT(30)+NT(11)+T(23)+T(3)+NT(11)+NT(30)));
//G140: bf_gteq(106)         => __E_wff_42(107).
	p(NT(106), (NT(107)));
//G141: wff(14)              => bf_gteq(106).
	p(NT(14), (NT(106)));
//G142: __E_wff_43(109)      => bf(30) _(11) '!' '>' '=' _(11) bf(30).
	p(NT(109), (NT(30)+NT(11)+T(34)+T(23)+T(3)+NT(11)+NT(30)));
//G143: bf_ngteq(108)        => __E_wff_43(109).
	p(NT(108), (NT(109)));
//G144: wff(14)              => bf_ngteq(108).
	p(NT(14), (NT(108)));
//G145: wff(14)              => capture(29).
	p(NT(14), (NT(29)));
//G146: __E_bf_44(111)       => '(' _(11) bf(30) _(11) ')'.
	p(NT(111), (T(12)+NT(11)+NT(30)+NT(11)+T(13)));
//G147: bf_parenthesis(110)  => __E_bf_44(111).
	p(NT(110), (NT(111)));
//G148: bf(30)               => bf_parenthesis(110).
	p(NT(30), (NT(110)));
//G149: bf(30)               => ba_constant(112).
	p(NT(30), (NT(112)));
//G150: bf(30)               => variable(113).
	p(NT(30), (NT(113)));
//G151: __E_bf_45(115)       => 'f' 'a' 'l' 'l' __(34) q_vars(67) __(34) bf(30).
	p(NT(115), (T(9)+T(16)+T(10)+T(10)+NT(34)+NT(67)+NT(34)+NT(30)));
//G152: bf_fall(114)         => __E_bf_45(115).
	p(NT(114), (NT(115)));
//G153: bf(30)               => bf_fall(114).
	p(NT(30), (NT(114)));
//G154: __E_bf_46(117)       => 'f' 'e' 'x' __(34) q_vars(67) __(34) bf(30).
	p(NT(117), (T(9)+T(11)+T(29)+NT(34)+NT(67)+NT(34)+NT(30)));
//G155: bf_fex(116)          => __E_bf_46(117).
	p(NT(116), (NT(117)));
//G156: bf(30)               => bf_fex(116).
	p(NT(30), (NT(116)));
//G157: bf_ref(118)          => ref(27).
	p(NT(118), (NT(27)));
//G158: bf(30)               => bf_ref(118).
	p(NT(30), (NT(118)));
//G159: __N_0(383)           => bf_shr(119).
	p(NT(383), (NT(119)));
//G160: __E___E_bf_47_48(121) => bf(30) & ~( __N_0(383) ).	 # conjunctive
	p(NT(121), (NT(30)) & ~(NT(383)));
//G161: __E_bf_47(120)       => bf(30) _(11) '>' '>' _(11) __E___E_bf_47_48(121).
	p(NT(120), (NT(30)+NT(11)+T(23)+T(23)+NT(11)+NT(121)));
//G162: bf_shr(119)          => __E_bf_47(120).
	p(NT(119), (NT(120)));
//G163: bf(30)               => bf_shr(119).
	p(NT(30), (NT(119)));
//G164: __N_1(384)           => bf_shl(122).
	p(NT(384), (NT(122)));
//G165: __E___E_bf_49_50(124) => bf(30) & ~( __N_1(384) ).	 # conjunctive
	p(NT(124), (NT(30)) & ~(NT(384)));
//G166: __E_bf_49(123)       => bf(30) _(11) '<' '<' _(11) __E___E_bf_49_50(124).
	p(NT(123), (NT(30)+NT(11)+T(22)+T(22)+NT(11)+NT(124)));
//G167: bf_shl(122)          => __E_bf_49(123).
	p(NT(122), (NT(123)));
//G168: bf(30)               => bf_shl(122).
	p(NT(30), (NT(122)));
//G169: __E_bf_51(126)       => bf(30) _(11) '+' _(11) bf(30).
	p(NT(126), (NT(30)+NT(11)+T(37)+NT(11)+NT(30)));
//G170: bf_add(125)          => __E_bf_51(126).
	p(NT(125), (NT(126)));
//G171: bf(30)               => bf_add(125).
	p(NT(30), (NT(125)));
//G172: __N_2(385)           => bf_sub(127).
	p(NT(385), (NT(127)));
//G173: __E___E_bf_52_53(129) => bf(30) & ~( __N_2(385) ).	 # conjunctive
	p(NT(129), (NT(30)) & ~(NT(385)));
//G174: __E_bf_52(128)       => bf(30) _(11) '-' _(11) __E___E_bf_52_53(129).
	p(NT(128), (NT(30)+NT(11)+T(30)+NT(11)+NT(129)));
//G175: bf_sub(127)          => __E_bf_52(128).
	p(NT(127), (NT(128)));
//G176: bf(30)               => bf_sub(127).
	p(NT(30), (NT(127)));
//G177: __N_3(386)           => bf_mod(130).
	p(NT(386), (NT(130)));
//G178: __E___E_bf_54_55(132) => bf(30) & ~( __N_3(386) ).	 # conjunctive
	p(NT(132), (NT(30)) & ~(NT(386)));
//G179: __E_bf_54(131)       => bf(30) _(11) '%' _(11) __E___E_bf_54_55(132).
	p(NT(131), (NT(30)+NT(11)+T(38)+NT(11)+NT(132)));
//G180: bf_mod(130)          => __E_bf_54(131).
	p(NT(130), (NT(131)));
//G181: bf(30)               => bf_mod(130).
	p(NT(30), (NT(130)));
//G182: __E_bf_56(134)       => bf(30) _(11) '*' _(11) bf(30).
	p(NT(134), (NT(30)+NT(11)+T(39)+NT(11)+NT(30)));
//G183: bf_mul(133)          => __E_bf_56(134).
	p(NT(133), (NT(134)));
//G184: bf(30)               => bf_mul(133).
	p(NT(30), (NT(133)));
//G185: __N_4(387)           => bf_div(135).
	p(NT(387), (NT(135)));
//G186: __E___E_bf_57_58(137) => bf(30) & ~( __N_4(387) ).	 # conjunctive
	p(NT(137), (NT(30)) & ~(NT(387)));
//G187: __E_bf_57(136)       => bf(30) _(11) '/' _(11) __E___E_bf_57_58(137).
	p(NT(136), (NT(30)+NT(11)+T(40)+NT(11)+NT(137)));
//G188: bf_div(135)          => __E_bf_57(136).
	p(NT(135), (NT(136)));
//G189: bf(30)               => bf_div(135).
	p(NT(30), (NT(135)));
//G190: __N_5(388)           => bf_nor(138).
	p(NT(388), (NT(138)));
//G191: __E___E_bf_59_60(140) => bf(30) & ~( __N_5(388) ).	 # conjunctive
	p(NT(140), (NT(30)) & ~(NT(388)));
//G192: __E_bf_59(139)       => bf(30) _(11) '!' '|' _(11) __E___E_bf_59_60(140).
	p(NT(139), (NT(30)+NT(11)+T(34)+T(31)+NT(11)+NT(140)));
//G193: bf_nor(138)          => __E_bf_59(139).
	p(NT(138), (NT(139)));
//G194: bf(30)               => bf_nor(138).
	p(NT(30), (NT(138)));
//G195: __N_6(389)           => bf_xnor(141).
	p(NT(389), (NT(141)));
//G196: __E___E_bf_61_62(143) => bf(30) & ~( __N_6(389) ).	 # conjunctive
	p(NT(143), (NT(30)) & ~(NT(389)));
//G197: __E_bf_61(142)       => bf(30) _(11) '!' '^' _(11) __E___E_bf_61_62(143).
	p(NT(142), (NT(30)+NT(11)+T(34)+T(32)+NT(11)+NT(143)));
//G198: bf_xnor(141)         => __E_bf_61(142).
	p(NT(141), (NT(142)));
//G199: bf(30)               => bf_xnor(141).
	p(NT(30), (NT(141)));
//G200: __N_7(390)           => bf_nand(144).
	p(NT(390), (NT(144)));
//G201: __E___E_bf_63_64(146) => bf(30) & ~( __N_7(390) ).	 # conjunctive
	p(NT(146), (NT(30)) & ~(NT(390)));
//G202: __E_bf_63(145)       => bf(30) _(11) '!' '&' _(11) __E___E_bf_63_64(146).
	p(NT(145), (NT(30)+NT(11)+T(34)+T(33)+NT(11)+NT(146)));
//G203: bf_nand(144)         => __E_bf_63(145).
	p(NT(144), (NT(145)));
//G204: bf(30)               => bf_nand(144).
	p(NT(30), (NT(144)));
//G205: __E_bf_65(148)       => bf(30) _(11) '|' _(11) bf(30).
	p(NT(148), (NT(30)+NT(11)+T(31)+NT(11)+NT(30)));
//G206: bf_or(147)           => __E_bf_65(148).
	p(NT(147), (NT(148)));
//G207: bf(30)               => bf_or(147).
	p(NT(30), (NT(147)));
//G208: __E_bf_66(150)       => bf(30) _(11) '^' _(11) bf(30).
	p(NT(150), (NT(30)+NT(11)+T(32)+NT(11)+NT(30)));
//G209: bf_xor(149)          => __E_bf_66(150).
	p(NT(149), (NT(150)));
//G210: bf(30)               => bf_xor(149).
	p(NT(30), (NT(149)));
//G211: __E___E_bf_67_68(153) => typed(33).
	p(NT(153), (NT(33)));
//G212: __E___E_bf_67_68(153) => null.
	p(NT(153), (nul));
//G213: __E_bf_67(152)       => '1' __E___E_bf_67_68(153).
	p(NT(152), (T(41)+NT(153)));
//G214: bf_t(151)            => __E_bf_67(152).
	p(NT(151), (NT(152)));
//G215: bf(30)               => bf_t(151).
	p(NT(30), (NT(151)));
//G216: __E___E_bf_69_70(156) => typed(33).
	p(NT(156), (NT(33)));
//G217: __E___E_bf_69_70(156) => null.
	p(NT(156), (nul));
//G218: __E_bf_69(155)       => '0' __E___E_bf_69_70(156).
	p(NT(155), (T(42)+NT(156)));
//G219: bf_f(154)            => __E_bf_69(155).
	p(NT(154), (NT(155)));
//G220: bf(30)               => bf_f(154).
	p(NT(30), (NT(154)));
//G221: __E___E_bf_71_72(159) => _(11) '&' _(11).
	p(NT(159), (NT(11)+T(33)+NT(11)));
//G222: __E___E_bf_71_72(159) => __(34).
	p(NT(159), (NT(34)));
//G223: __E_bf_71(158)       => bf(30) __E___E_bf_71_72(159) bf(30).
	p(NT(158), (NT(30)+NT(159)+NT(30)));
//G224: bf_and(157)          => __E_bf_71(158).
	p(NT(157), (NT(158)));
//G225: bf(30)               => bf_and(157).
	p(NT(30), (NT(157)));
//G226: __E___E_bf_73_74(163) => bf_parenthesis(110).
	p(NT(163), (NT(110)));
//G227: __E___E_bf_73_74(163) => ba_constant(112).
	p(NT(163), (NT(112)));
//G228: __E___E_bf_73_74(163) => variable(113).
	p(NT(163), (NT(113)));
//G229: __E___E_bf_73_74(163) => bf_ref(118).
	p(NT(163), (NT(118)));
//G230: __E___E_bf_73_74(163) => bf_t(151).
	p(NT(163), (NT(151)));
//G231: __E___E_bf_73_74(163) => bf_f(154).
	p(NT(163), (NT(154)));
//G232: __E___E_bf_73_74(163) => bf_neg(160).
	p(NT(163), (NT(160)));
//G233: __E___E_bf_73_74(163) => capture(29).
	p(NT(163), (NT(29)));
//G234: bf_neg_oprnd(162)    => __E___E_bf_73_74(163).
	p(NT(162), (NT(163)));
//G235: __E_bf_73(161)       => bf_neg_oprnd(162) _(11) '\''.
	p(NT(161), (NT(162)+NT(11)+T(43)));
//G236: bf_neg(160)          => __E_bf_73(161).
	p(NT(160), (NT(161)));
//G237: bf(30)               => bf_neg(160).
	p(NT(30), (NT(160)));
//G238: __E___E_bf_75_76(167) => bf_parenthesis(110).
	p(NT(167), (NT(110)));
//G239: __E___E_bf_75_76(167) => variable(113).
	p(NT(167), (NT(113)));
//G240: __E___E_bf_75_76(167) => bf_ref(118).
	p(NT(167), (NT(118)));
//G241: __E___E_bf_75_76(167) => bf_neg(160).
	p(NT(167), (NT(160)));
//G242: __E___E_bf_75_76(167) => bf_and_nosep(164).
	p(NT(167), (NT(164)));
//G243: __E___E_bf_75_76(167) => capture(29).
	p(NT(167), (NT(29)));
//G244: bf_and_nosep_1st_oprnd(166) => __E___E_bf_75_76(167).
	p(NT(166), (NT(167)));
//G245: __E___E_bf_75_77(169) => bf_parenthesis(110).
	p(NT(169), (NT(110)));
//G246: __E___E_bf_75_77(169) => ba_constant(112).
	p(NT(169), (NT(112)));
//G247: __E___E_bf_75_77(169) => variable(113).
	p(NT(169), (NT(113)));
//G248: __E___E_bf_75_77(169) => bf_ref(118).
	p(NT(169), (NT(118)));
//G249: __E___E_bf_75_77(169) => bf_neg(160).
	p(NT(169), (NT(160)));
//G250: __E___E_bf_75_77(169) => capture(29).
	p(NT(169), (NT(29)));
//G251: bf_and_nosep_2nd_oprnd(168) => __E___E_bf_75_77(169).
	p(NT(168), (NT(169)));
//G252: __E_bf_75(165)       => bf_and_nosep_1st_oprnd(166) bf_and_nosep_2nd_oprnd(168).
	p(NT(165), (NT(166)+NT(168)));
//G253: bf_and_nosep(164)    => __E_bf_75(165).
	p(NT(164), (NT(165)));
//G254: bf(30)               => bf_and_nosep(164).
	p(NT(30), (NT(164)));
//G255: bf(30)               => capture(29).
	p(NT(30), (NT(29)));
//G256: __E_constraint_78(171) => '[' ctnvar(172) _(11) '!' '=' _(11) num(173) ']'.
	p(NT(171), (T(26)+NT(172)+NT(11)+T(34)+T(3)+NT(11)+NT(173)+T(27)));
//G257: __E_constraint_78(171) => '[' num(173) _(11) '!' '=' _(11) ctnvar(172) ']'.
	p(NT(171), (T(26)+NT(173)+NT(11)+T(34)+T(3)+NT(11)+NT(172)+T(27)));
//G258: ctn_neq(170)         => __E_constraint_78(171).
	p(NT(170), (NT(171)));
//G259: constraint(71)       => ctn_neq(170).
	p(NT(71), (NT(170)));
//G260: __E_constraint_79(175) => '[' ctnvar(172) _(11) '=' _(11) num(173) ']'.
	p(NT(175), (T(26)+NT(172)+NT(11)+T(3)+NT(11)+NT(173)+T(27)));
//G261: __E_constraint_79(175) => '[' num(173) _(11) '=' _(11) ctnvar(172) ']'.
	p(NT(175), (T(26)+NT(173)+NT(11)+T(3)+NT(11)+NT(172)+T(27)));
//G262: ctn_eq(174)          => __E_constraint_79(175).
	p(NT(174), (NT(175)));
//G263: constraint(71)       => ctn_eq(174).
	p(NT(71), (NT(174)));
//G264: __E_constraint_80(177) => '[' ctnvar(172) _(11) '>' '=' _(11) num(173) ']'.
	p(NT(177), (T(26)+NT(172)+NT(11)+T(23)+T(3)+NT(11)+NT(173)+T(27)));
//G265: __E_constraint_80(177) => '[' num(173) _(11) '>' '=' _(11) ctnvar(172) ']'.
	p(NT(177), (T(26)+NT(173)+NT(11)+T(23)+T(3)+NT(11)+NT(172)+T(27)));
//G266: ctn_gteq(176)        => __E_constraint_80(177).
	p(NT(176), (NT(177)));
//G267: constraint(71)       => ctn_gteq(176).
	p(NT(71), (NT(176)));
//G268: __E_constraint_81(179) => '[' ctnvar(172) _(11) '>' _(11) num(173) ']'.
	p(NT(179), (T(26)+NT(172)+NT(11)+T(23)+NT(11)+NT(173)+T(27)));
//G269: __E_constraint_81(179) => '[' num(173) _(11) '>' _(11) ctnvar(172) ']'.
	p(NT(179), (T(26)+NT(173)+NT(11)+T(23)+NT(11)+NT(172)+T(27)));
//G270: ctn_gt(178)          => __E_constraint_81(179).
	p(NT(178), (NT(179)));
//G271: constraint(71)       => ctn_gt(178).
	p(NT(71), (NT(178)));
//G272: __E_constraint_82(181) => '[' ctnvar(172) _(11) '<' '=' _(11) num(173) ']'.
	p(NT(181), (T(26)+NT(172)+NT(11)+T(22)+T(3)+NT(11)+NT(173)+T(27)));
//G273: __E_constraint_82(181) => '[' num(173) _(11) '<' '=' _(11) ctnvar(172) ']'.
	p(NT(181), (T(26)+NT(173)+NT(11)+T(22)+T(3)+NT(11)+NT(172)+T(27)));
//G274: ctn_lteq(180)        => __E_constraint_82(181).
	p(NT(180), (NT(181)));
//G275: constraint(71)       => ctn_lteq(180).
	p(NT(71), (NT(180)));
//G276: __E_constraint_83(183) => '[' ctnvar(172) _(11) '<' _(11) num(173) ']'.
	p(NT(183), (T(26)+NT(172)+NT(11)+T(22)+NT(11)+NT(173)+T(27)));
//G277: __E_constraint_83(183) => '[' num(173) _(11) '<' _(11) ctnvar(172) ']'.
	p(NT(183), (T(26)+NT(173)+NT(11)+T(22)+NT(11)+NT(172)+T(27)));
//G278: ctn_lt(182)          => __E_constraint_83(183).
	p(NT(182), (NT(183)));
//G279: constraint(71)       => ctn_lt(182).
	p(NT(71), (NT(182)));
//G280: __E_ba_constant_84(184) => capture(29).
	p(NT(184), (NT(29)));
//G281: __E_ba_constant_84(184) => source(185).
	p(NT(184), (NT(185)));
//G282: __E_ba_constant_85(186) => typed(33).
	p(NT(186), (NT(33)));
//G283: __E_ba_constant_85(186) => null.
	p(NT(186), (nul));
//G284: ba_constant(112)     => '{' _(11) __E_ba_constant_84(184) _(11) '}' __E_ba_constant_85(186).
	p(NT(112), (T(44)+NT(11)+NT(184)+NT(11)+T(45)+NT(186)));
//G285: __E___E_source_86_87(189) => src_c(187).
	p(NT(189), (NT(187)));
//G286: __E___E_source_86_87(189) => space(2).
	p(NT(189), (NT(2)));
//G287: __E___E_source_86_88(190) => null.
	p(NT(190), (nul));
//G288: __E___E_source_86_88(190) => __E___E_source_86_87(189) __E___E_source_86_88(190).
	p(NT(190), (NT(189)+NT(190)));
//G289: __E_source_86(188)   => __E___E_source_86_88(190) src_c(187).
	p(NT(188), (NT(190)+NT(187)));
//G290: __E_source_86(188)   => null.
	p(NT(188), (nul));
//G291: source(185)          => src_c(187) __E_source_86(188).
	p(NT(185), (NT(187)+NT(188)));
//G292: src_c(187)           => alnum(6).
	p(NT(187), (NT(6)));
//G293: __N_8(391)           => '{'.
	p(NT(391), (T(44)));
//G294: __N_9(392)           => '}'.
	p(NT(392), (T(45)));
//G295: src_c(187)           => ~( __N_8(391) ) & ~( __N_9(392) ) & punct(7).	 # conjunctive
	p(NT(187), ~(NT(391)) & ~(NT(392)) & (NT(7)));
//G296: __E_src_c_89(191)    => src_c(187).
	p(NT(191), (NT(187)));
//G297: __E_src_c_89(191)    => space(2).
	p(NT(191), (NT(2)));
//G298: __E_src_c_90(192)    => null.
	p(NT(192), (nul));
//G299: __E_src_c_90(192)    => __E_src_c_89(191) __E_src_c_90(192).
	p(NT(192), (NT(191)+NT(192)));
//G300: src_c(187)           => '{' __E_src_c_90(192) '}'.
	p(NT(187), (T(44)+NT(192)+T(45)));
//G301: __E_variable_91(193) => uconst(194).
	p(NT(193), (NT(194)));
//G302: __E_variable_91(193) => io_var(195).
	p(NT(193), (NT(195)));
//G303: __E_variable_91(193) => var_name(196).
	p(NT(193), (NT(196)));
//G304: __E_variable_92(197) => typed(33).
	p(NT(197), (NT(33)));
//G305: __E_variable_92(197) => null.
	p(NT(197), (nul));
//G306: variable(113)        => __E_variable_91(193) __E_variable_92(197).
	p(NT(113), (NT(193)+NT(197)));
//G307: __N_10(393)          => 'F'.
	p(NT(393), (T(36)));
//G308: __N_11(394)          => 'T'.
	p(NT(394), (T(35)));
//G309: __E_var_name_93(198) => ~( __N_10(393) ) & ~( __N_11(394) ) & alpha(5).	 # conjunctive
	p(NT(198), ~(NT(393)) & ~(NT(394)) & (NT(5)));
//G310: __E_var_name_94(199) => null.
	p(NT(199), (nul));
//G311: __E_var_name_94(199) => digit(3) __E_var_name_94(199).
	p(NT(199), (NT(3)+NT(199)));
//G312: var_name(196)        => __E_var_name_93(198) __E_var_name_94(199).	 # guarded: charvar
	p(NT(196), (NT(198)+NT(199)));
	p.back().guard = "charvar";
//G313: __N_12(395)          => 'F'.
	p(NT(395), (T(36)));
//G314: __N_13(396)          => 'T'.
	p(NT(396), (T(35)));
//G315: __E_var_name_95(200) => ~( __N_12(395) ) & ~( __N_13(396) ) & alpha(5).	 # conjunctive
	p(NT(200), ~(NT(395)) & ~(NT(396)) & (NT(5)));
//G316: __E_var_name_96(201) => alnum(6).
	p(NT(201), (NT(6)));
//G317: __E_var_name_96(201) => '_'.
	p(NT(201), (T(46)));
//G318: __E_var_name_97(202) => null.
	p(NT(202), (nul));
//G319: __E_var_name_97(202) => __E_var_name_96(201) __E_var_name_97(202).
	p(NT(202), (NT(201)+NT(202)));
//G320: var_name(196)        => __E_var_name_95(200) __E_var_name_97(202).	 # guarded: var
	p(NT(196), (NT(200)+NT(202)));
	p.back().guard = "var";
//G321: io_var(195)          => io_var_name(31) '[' offset(203) ']'.
	p(NT(195), (NT(31)+T(26)+NT(203)+T(27)));
//G322: io_var_name(31)      => chars(204).
	p(NT(31), (NT(204)));
//G323: __E___E_uconst_98_99(207) => chars(204) _(11).
	p(NT(207), (NT(204)+NT(11)));
//G324: __E___E_uconst_98_99(207) => null.
	p(NT(207), (nul));
//G325: __E_uconst_98(206)   => __E___E_uconst_98_99(207) ':' _(11) chars(204).
	p(NT(206), (NT(207)+T(2)+NT(11)+NT(204)));
//G326: uconst_name(205)     => __E_uconst_98(206).
	p(NT(205), (NT(206)));
//G327: uconst(194)          => '<' _(11) uconst_name(205) _(11) '>'.
	p(NT(194), (T(22)+NT(11)+NT(205)+NT(11)+T(23)));
//G328: __E_q_vars_100(209)  => _(11) ',' _(11) q_var(208).
	p(NT(209), (NT(11)+T(20)+NT(11)+NT(208)));
//G329: __E_q_vars_101(210)  => null.
	p(NT(210), (nul));
//G330: __E_q_vars_101(210)  => __E_q_vars_100(209) __E_q_vars_101(210).
	p(NT(210), (NT(209)+NT(210)));
//G331: q_vars(67)           => q_var(208) __E_q_vars_101(210).
	p(NT(67), (NT(208)+NT(210)));
//G332: q_var(208)           => capture(29).
	p(NT(208), (NT(29)));
//G333: __N_14(397)          => uconst(194).
	p(NT(397), (NT(194)));
//G334: q_var(208)           => variable(113) & ~( __N_14(397) ).	 # conjunctive
	p(NT(208), (NT(113)) & ~(NT(397)));
//G335: ctnvar(172)          => chars(204).
	p(NT(172), (NT(204)));
//G336: __E_offsets_102(211) => _(11) ',' _(11) offset(203).
	p(NT(211), (NT(11)+T(20)+NT(11)+NT(203)));
//G337: __E_offsets_103(212) => null.
	p(NT(212), (nul));
//G338: __E_offsets_103(212) => __E_offsets_102(211) __E_offsets_103(212).
	p(NT(212), (NT(211)+NT(212)));
//G339: offsets(41)          => '[' _(11) offset(203) __E_offsets_103(212) _(11) ']'.
	p(NT(41), (T(26)+NT(11)+NT(203)+NT(212)+NT(11)+T(27)));
//G340: offset(203)          => integer(213).
	p(NT(203), (NT(213)));
//G341: offset(203)          => capture(29).
	p(NT(203), (NT(29)));
//G342: offset(203)          => shift(214).
	p(NT(203), (NT(214)));
//G343: __N_15(398)          => io_var(195).
	p(NT(398), (NT(195)));
//G344: __E_offset_104(215)  => variable(113) & ~( __N_15(398) ).	 # conjunctive
	p(NT(215), (NT(113)) & ~(NT(398)));
//G345: offset(203)          => __E_offset_104(215).
	p(NT(203), (NT(215)));
//G346: __E_shift_105(216)   => capture(29).
	p(NT(216), (NT(29)));
//G347: __N_16(399)          => io_var(195).
	p(NT(399), (NT(195)));
//G348: __E___E_shift_105_106(217) => variable(113) & ~( __N_16(399) ).	 # conjunctive
	p(NT(217), (NT(113)) & ~(NT(399)));
//G349: __E_shift_105(216)   => __E___E_shift_105_106(217).
	p(NT(216), (NT(217)));
//G350: shift(214)           => __E_shift_105(216) _(11) '-' _(11) num(173).
	p(NT(214), (NT(216)+NT(11)+T(30)+NT(11)+NT(173)));
//G351: __E_chars_107(218)   => alnum(6).
	p(NT(218), (NT(6)));
//G352: __E_chars_107(218)   => '_'.
	p(NT(218), (T(46)));
//G353: __E_chars_108(219)   => null.
	p(NT(219), (nul));
//G354: __E_chars_108(219)   => __E_chars_107(218) __E_chars_108(219).
	p(NT(219), (NT(218)+NT(219)));
//G355: chars(204)           => alpha(5) __E_chars_108(219).
	p(NT(204), (NT(5)+NT(219)));
//G356: __E_digits_109(221)  => digit(3).
	p(NT(221), (NT(3)));
//G357: __E_digits_109(221)  => digit(3) __E_digits_109(221).
	p(NT(221), (NT(3)+NT(221)));
//G358: digits(220)          => __E_digits_109(221).
	p(NT(220), (NT(221)));
//G359: num(173)             => digits(220).
	p(NT(173), (NT(220)));
//G360: __E_integer_110(222) => '-'.
	p(NT(222), (T(30)));
//G361: __E_integer_110(222) => null.
	p(NT(222), (nul));
//G362: integer(213)         => __E_integer_110(222) _(11) digits(220).
	p(NT(213), (NT(222)+NT(11)+NT(220)));
//G363: sym(39)              => chars(204).
	p(NT(39), (NT(204)));
//G364: capture(29)          => '$' chars(204).
	p(NT(29), (T(47)+NT(204)));
//G365: __E_typed_111(224)   => _(11) '[' _(11) subtype(225) _(11) ']' _(11).
	p(NT(224), (NT(11)+T(26)+NT(11)+NT(225)+NT(11)+T(27)+NT(11)));
//G366: __E_typed_111(224)   => null.
	p(NT(224), (nul));
//G367: typed(33)            => _(11) ':' _(11) type(223) __E_typed_111(224).
	p(NT(33), (NT(11)+T(2)+NT(11)+NT(223)+NT(224)));
//G368: type(223)            => chars(204).
	p(NT(223), (NT(204)));
//G369: subtype(225)         => num(173).
	p(NT(225), (NT(173)));
//G370: __E_comment_112(227) => printable(8).
	p(NT(227), (NT(8)));
//G371: __E_comment_112(227) => '\t'.
	p(NT(227), (T(48)));
//G372: __E_comment_113(228) => null.
	p(NT(228), (nul));
//G373: __E_comment_113(228) => __E_comment_112(227) __E_comment_113(228).
	p(NT(228), (NT(227)+NT(228)));
//G374: __E_comment_114(229) => '\n'.
	p(NT(229), (T(49)));
//G375: __E_comment_114(229) => '\r'.
	p(NT(229), (T(50)));
//G376: __E_comment_114(229) => eof(1).
	p(NT(229), (NT(1)));
//G377: comment(226)         => '#' __E_comment_113(228) __E_comment_114(229).
	p(NT(226), (T(51)+NT(228)+NT(229)));
//G378: __(34)               => space(2).
	p(NT(34), (NT(2)));
//G379: __(34)               => comment(226).
	p(NT(34), (NT(226)));
//G380: __(34)               => __(34) space(2).
	p(NT(34), (NT(34)+NT(2)));
//G381: __(34)               => __(34) comment(226).
	p(NT(34), (NT(34)+NT(226)));
//G382: _(11)                => __(34).
	p(NT(11), (NT(34)));
//G383: _(11)                => null.
	p(NT(11), (nul));
//G384: cli(230)             => _(11).
	p(NT(230), (NT(11)));
//G385: __E_cli_115(232)     => '.' _(11) cli_command(231) _(11).
	p(NT(232), (T(1)+NT(11)+NT(231)+NT(11)));
//G386: __E_cli_116(233)     => null.
	p(NT(233), (nul));
//G387: __E_cli_116(233)     => __E_cli_115(232) __E_cli_116(233).
	p(NT(233), (NT(232)+NT(233)));
//G388: cli(230)             => _(11) cli_command(231) _(11) __E_cli_116(233).
	p(NT(230), (NT(11)+NT(231)+NT(11)+NT(233)));
//G389: __E_cli_command_117(236) => 'q'.
	p(NT(236), (T(52)));
//G390: __E_cli_command_117(236) => 'q' 'u' 'i' 't'.
	p(NT(236), (T(52)+T(7)+T(4)+T(8)));
//G391: quit_sym(235)        => __E_cli_command_117(236).
	p(NT(235), (NT(236)));
//G392: quit_cmd(234)        => quit_sym(235).
	p(NT(234), (NT(235)));
//G393: cli_command(231)     => quit_cmd(234).
	p(NT(231), (NT(234)));
//G394: __E_cli_command_118(239) => 'v'.
	p(NT(239), (T(53)));
//G395: __E_cli_command_118(239) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(239), (T(53)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G396: version_sym(238)     => __E_cli_command_118(239).
	p(NT(238), (NT(239)));
//G397: version_cmd(237)     => version_sym(238).
	p(NT(237), (NT(238)));
//G398: cli_command(231)     => version_cmd(237).
	p(NT(231), (NT(237)));
//G399: __E_cli_command_119(242) => 'c'.
	p(NT(242), (T(14)));
//G400: __E_cli_command_119(242) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(242), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G401: clear_sym(241)       => __E_cli_command_119(242).
	p(NT(241), (NT(242)));
//G402: clear_cmd(240)       => clear_sym(241).
	p(NT(240), (NT(241)));
//G403: cli_command(231)     => clear_cmd(240).
	p(NT(231), (NT(240)));
//G404: __E___E_cli_command_120_121(246) => 'h'.
	p(NT(246), (T(54)));
//G405: __E___E_cli_command_120_121(246) => 'h' 'e' 'l' 'p'.
	p(NT(246), (T(54)+T(11)+T(10)+T(55)));
//G406: help_sym(245)        => __E___E_cli_command_120_121(246).
	p(NT(245), (NT(246)));
//G407: __E___E_cli_command_120_122(247) => __(34) help_arg(248).
	p(NT(247), (NT(34)+NT(248)));
//G408: __E___E_cli_command_120_122(247) => null.
	p(NT(247), (nul));
//G409: __E_cli_command_120(244) => help_sym(245) __E___E_cli_command_120_122(247).
	p(NT(244), (NT(245)+NT(247)));
//G410: help_cmd(243)        => __E_cli_command_120(244).
	p(NT(243), (NT(244)));
//G411: cli_command(231)     => help_cmd(243).
	p(NT(231), (NT(243)));
//G412: file_sym(251)        => 'f' 'i' 'l' 'e'.
	p(NT(251), (T(9)+T(4)+T(10)+T(11)));
//G413: __E_cli_command_123(250) => file_sym(251) __(34) q_string(252).
	p(NT(250), (NT(251)+NT(34)+NT(252)));
//G414: file_cmd(249)        => __E_cli_command_123(250).
	p(NT(249), (NT(250)));
//G415: cli_command(231)     => file_cmd(249).
	p(NT(231), (NT(249)));
//G416: valid_sym(255)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(255), (T(53)+T(16)+T(10)+T(4)+T(56)));
//G417: __E_cli_command_124(254) => valid_sym(255) __(34) normalize_cmd_arg(256).
	p(NT(254), (NT(255)+NT(34)+NT(256)));
//G418: valid_cmd(253)       => __E_cli_command_124(254).
	p(NT(253), (NT(254)));
//G419: cli_command(231)     => valid_cmd(253).
	p(NT(231), (NT(253)));
//G420: sat_sym(259)         => 's' 'a' 't'.
	p(NT(259), (T(15)+T(16)+T(8)));
//G421: __E_cli_command_125(258) => sat_sym(259) __(34) normalize_cmd_arg(256).
	p(NT(258), (NT(259)+NT(34)+NT(256)));
//G422: sat_cmd(257)         => __E_cli_command_125(258).
	p(NT(257), (NT(258)));
//G423: cli_command(231)     => sat_cmd(257).
	p(NT(231), (NT(257)));
//G424: unsat_sym(262)       => 'u' 'n' 's' 'a' 't'.
	p(NT(262), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G425: __E_cli_command_126(261) => unsat_sym(262) __(34) normalize_cmd_arg(256).
	p(NT(261), (NT(262)+NT(34)+NT(256)));
//G426: unsat_cmd(260)       => __E_cli_command_126(261).
	p(NT(260), (NT(261)));
//G427: cli_command(231)     => unsat_cmd(260).
	p(NT(231), (NT(260)));
//G428: solve_sym(265)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(265), (T(15)+T(6)+T(10)+T(53)+T(11)));
//G429: __E___E_cli_command_127_128(266) => solve_options(267).
	p(NT(266), (NT(267)));
//G430: __E___E_cli_command_127_128(266) => null.
	p(NT(266), (nul));
//G431: __E_cli_command_127(264) => solve_sym(265) __E___E_cli_command_127_128(266) __(34) wff_cmd_arg(268).
	p(NT(264), (NT(265)+NT(266)+NT(34)+NT(268)));
//G432: solve_cmd(263)       => __E_cli_command_127(264).
	p(NT(263), (NT(264)));
//G433: cli_command(231)     => solve_cmd(263).
	p(NT(231), (NT(263)));
//G434: lgrs_sym(271)        => 'l' 'g' 'r' 's'.
	p(NT(271), (T(10)+T(57)+T(19)+T(15)));
//G435: __E_cli_command_129(270) => lgrs_sym(271) __(34) wff_cmd_arg(268).
	p(NT(270), (NT(271)+NT(34)+NT(268)));
//G436: lgrs_cmd(269)        => __E_cli_command_129(270).
	p(NT(269), (NT(270)));
//G437: cli_command(231)     => lgrs_cmd(269).
	p(NT(231), (NT(269)));
//G438: __E___E_cli_command_130_131(275) => 'r'.
	p(NT(275), (T(19)));
//G439: __E___E_cli_command_130_131(275) => 'r' 'u' 'n'.
	p(NT(275), (T(19)+T(7)+T(5)));
//G440: run_sym(274)         => __E___E_cli_command_130_131(275).
	p(NT(274), (NT(275)));
//G441: __E___E_cli_command_130_132(276) => history(277).
	p(NT(276), (NT(277)));
//G442: __E___E_cli_command_130_132(276) => wff(14).
	p(NT(276), (NT(14)));
//G443: __E_cli_command_130(273) => run_sym(274) __(34) __E___E_cli_command_130_132(276).
	p(NT(273), (NT(274)+NT(34)+NT(276)));
//G444: run_cmd(272)         => __E_cli_command_130(273).
	p(NT(272), (NT(273)));
//G445: cli_command(231)     => run_cmd(272).
	p(NT(231), (NT(272)));
//G446: __E___E_cli_command_133_134(281) => 'n'.
	p(NT(281), (T(5)));
//G447: __E___E_cli_command_133_134(281) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(281), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(58)+T(11)));
//G448: normalize_sym(280)   => __E___E_cli_command_133_134(281).
	p(NT(280), (NT(281)));
//G449: __E_cli_command_133(279) => normalize_sym(280) __(34) normalize_cmd_arg(256).
	p(NT(279), (NT(280)+NT(34)+NT(256)));
//G450: normalize_cmd(278)   => __E_cli_command_133(279).
	p(NT(278), (NT(279)));
//G451: cli_command(231)     => normalize_cmd(278).
	p(NT(231), (NT(278)));
//G452: __E___E_cli_command_135_136(285) => 's'.
	p(NT(285), (T(15)));
//G453: __E___E_cli_command_135_136(285) => 's' 'u' 'b' 's' 't'.
	p(NT(285), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G454: __E___E_cli_command_135_136(285) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(285), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G455: subst_sym(284)       => __E___E_cli_command_135_136(285).
	p(NT(284), (NT(285)));
//G456: __E_cli_command_135(283) => subst_sym(284) __(34) nf_cmd_arg(286) _(11) '[' _(11) nf_cmd_arg(286) _(11) '/' _(11) nf_cmd_arg(286) _(11) ']'.
	p(NT(283), (NT(284)+NT(34)+NT(286)+NT(11)+T(26)+NT(11)+NT(286)+NT(11)+T(40)+NT(11)+NT(286)+NT(11)+T(27)));
//G457: subst_cmd(282)       => __E_cli_command_135(283).
	p(NT(282), (NT(283)));
//G458: cli_command(231)     => subst_cmd(282).
	p(NT(231), (NT(282)));
//G459: __E___E_cli_command_137_138(290) => 'i'.
	p(NT(290), (T(4)));
//G460: __E___E_cli_command_137_138(290) => 'i' 'n' 's' 't'.
	p(NT(290), (T(4)+T(5)+T(15)+T(8)));
//G461: __E___E_cli_command_137_138(290) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(290), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G462: inst_sym(289)        => __E___E_cli_command_137_138(290).
	p(NT(289), (NT(290)));
//G463: __E_cli_command_137(288) => inst_sym(289) __(34) inst_args(291).
	p(NT(288), (NT(289)+NT(34)+NT(291)));
//G464: inst_cmd(287)        => __E_cli_command_137(288).
	p(NT(287), (NT(288)));
//G465: cli_command(231)     => inst_cmd(287).
	p(NT(231), (NT(287)));
//G466: dnf_sym(294)         => 'd' 'n' 'f'.
	p(NT(294), (T(56)+T(5)+T(9)));
//G467: __E_cli_command_139(293) => dnf_sym(294) __(34) nf_cmd_arg(286).
	p(NT(293), (NT(294)+NT(34)+NT(286)));
//G468: dnf_cmd(292)         => __E_cli_command_139(293).
	p(NT(292), (NT(293)));
//G469: cli_command(231)     => dnf_cmd(292).
	p(NT(231), (NT(292)));
//G470: cnf_sym(297)         => 'c' 'n' 'f'.
	p(NT(297), (T(14)+T(5)+T(9)));
//G471: __E_cli_command_140(296) => cnf_sym(297) __(34) nf_cmd_arg(286).
	p(NT(296), (NT(297)+NT(34)+NT(286)));
//G472: cnf_cmd(295)         => __E_cli_command_140(296).
	p(NT(295), (NT(296)));
//G473: cli_command(231)     => cnf_cmd(295).
	p(NT(231), (NT(295)));
//G474: anf_sym(300)         => 'a' 'n' 'f'.
	p(NT(300), (T(16)+T(5)+T(9)));
//G475: __E_cli_command_141(299) => anf_sym(300) __(34) nf_cmd_arg(286).
	p(NT(299), (NT(300)+NT(34)+NT(286)));
//G476: anf_cmd(298)         => __E_cli_command_141(299).
	p(NT(298), (NT(299)));
//G477: cli_command(231)     => anf_cmd(298).
	p(NT(231), (NT(298)));
//G478: nnf_sym(303)         => 'n' 'n' 'f'.
	p(NT(303), (T(5)+T(5)+T(9)));
//G479: __E_cli_command_142(302) => nnf_sym(303) __(34) nf_cmd_arg(286).
	p(NT(302), (NT(303)+NT(34)+NT(286)));
//G480: nnf_cmd(301)         => __E_cli_command_142(302).
	p(NT(301), (NT(302)));
//G481: cli_command(231)     => nnf_cmd(301).
	p(NT(231), (NT(301)));
//G482: pnf_sym(306)         => 'p' 'n' 'f'.
	p(NT(306), (T(55)+T(5)+T(9)));
//G483: __E_cli_command_143(305) => pnf_sym(306) __(34) nf_cmd_arg(286).
	p(NT(305), (NT(306)+NT(34)+NT(286)));
//G484: pnf_cmd(304)         => __E_cli_command_143(305).
	p(NT(304), (NT(305)));
//G485: cli_command(231)     => pnf_cmd(304).
	p(NT(231), (NT(304)));
//G486: mnf_sym(309)         => 'm' 'n' 'f'.
	p(NT(309), (T(21)+T(5)+T(9)));
//G487: __E_cli_command_144(308) => mnf_sym(309) __(34) nf_cmd_arg(286).
	p(NT(308), (NT(309)+NT(34)+NT(286)));
//G488: mnf_cmd(307)         => __E_cli_command_144(308).
	p(NT(307), (NT(308)));
//G489: cli_command(231)     => mnf_cmd(307).
	p(NT(231), (NT(307)));
//G490: onf_sym(312)         => 'o' 'n' 'f'.
	p(NT(312), (T(6)+T(5)+T(9)));
//G491: __E_cli_command_145(311) => onf_sym(312) __(34) variable(113) __(34) onf_cmd_arg(313).
	p(NT(311), (NT(312)+NT(34)+NT(113)+NT(34)+NT(313)));
//G492: onf_cmd(310)         => __E_cli_command_145(311).
	p(NT(310), (NT(311)));
//G493: cli_command(231)     => onf_cmd(310).
	p(NT(231), (NT(310)));
//G494: qelim_sym(316)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(316), (T(52)+T(11)+T(10)+T(4)+T(21)));
//G495: __E_cli_command_146(315) => qelim_sym(316) __(34) wff_cmd_arg(268).
	p(NT(315), (NT(316)+NT(34)+NT(268)));
//G496: qelim_cmd(314)       => __E_cli_command_146(315).
	p(NT(314), (NT(315)));
//G497: cli_command(231)     => qelim_cmd(314).
	p(NT(231), (NT(314)));
//G498: get_sym(319)         => 'g' 'e' 't'.
	p(NT(319), (T(57)+T(11)+T(8)));
//G499: __E___E_cli_command_147_148(320) => __(34) option_name(321).
	p(NT(320), (NT(34)+NT(321)));
//G500: __E___E_cli_command_147_148(320) => null.
	p(NT(320), (nul));
//G501: __E_cli_command_147(318) => get_sym(319) __E___E_cli_command_147_148(320).
	p(NT(318), (NT(319)+NT(320)));
//G502: get_cmd(317)         => __E_cli_command_147(318).
	p(NT(317), (NT(318)));
//G503: cli_command(231)     => get_cmd(317).
	p(NT(231), (NT(317)));
//G504: set_sym(324)         => 's' 'e' 't'.
	p(NT(324), (T(15)+T(11)+T(8)));
//G505: __E___E_cli_command_149_150(325) => __(34).
	p(NT(325), (NT(34)));
//G506: __E___E_cli_command_149_150(325) => _(11) '=' _(11).
	p(NT(325), (NT(11)+T(3)+NT(11)));
//G507: __E_cli_command_149(323) => set_sym(324) __(34) option_name(321) __E___E_cli_command_149_150(325) option_value(326).
	p(NT(323), (NT(324)+NT(34)+NT(321)+NT(325)+NT(326)));
//G508: set_cmd(322)         => __E_cli_command_149(323).
	p(NT(322), (NT(323)));
//G509: cli_command(231)     => set_cmd(322).
	p(NT(231), (NT(322)));
//G510: enable_sym(329)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(329), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G511: __E_cli_command_151(328) => enable_sym(329) __(34) option_name(321).
	p(NT(328), (NT(329)+NT(34)+NT(321)));
//G512: enable_cmd(327)      => __E_cli_command_151(328).
	p(NT(327), (NT(328)));
//G513: cli_command(231)     => enable_cmd(327).
	p(NT(231), (NT(327)));
//G514: disable_sym(332)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(332), (T(56)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G515: __E_cli_command_152(331) => disable_sym(332) __(34) option_name(321).
	p(NT(331), (NT(332)+NT(34)+NT(321)));
//G516: disable_cmd(330)     => __E_cli_command_152(331).
	p(NT(330), (NT(331)));
//G517: cli_command(231)     => disable_cmd(330).
	p(NT(231), (NT(330)));
//G518: toggle_sym(335)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(335), (T(8)+T(6)+T(57)+T(57)+T(10)+T(11)));
//G519: __E_cli_command_153(334) => toggle_sym(335) __(34) option_name(321).
	p(NT(334), (NT(335)+NT(34)+NT(321)));
//G520: toggle_cmd(333)      => __E_cli_command_153(334).
	p(NT(333), (NT(334)));
//G521: cli_command(231)     => toggle_cmd(333).
	p(NT(231), (NT(333)));
//G522: __E___E_cli_command_154_155(339) => 'd' 'e' 'f' 's'.
	p(NT(339), (T(56)+T(11)+T(9)+T(15)));
//G523: __E___E_cli_command_154_155(339) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(339), (T(56)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G524: def_sym(338)         => __E___E_cli_command_154_155(339).
	p(NT(338), (NT(339)));
//G525: __E_cli_command_154(337) => def_sym(338).
	p(NT(337), (NT(338)));
//G526: def_list_cmd(336)    => __E_cli_command_154(337).
	p(NT(336), (NT(337)));
//G527: cli_command(231)     => def_list_cmd(336).
	p(NT(231), (NT(336)));
//G528: __E_cli_command_156(341) => def_sym(338) __(34) num(173).
	p(NT(341), (NT(338)+NT(34)+NT(173)));
//G529: def_print_cmd(340)   => __E_cli_command_156(341).
	p(NT(340), (NT(341)));
//G530: cli_command(231)     => def_print_cmd(340).
	p(NT(231), (NT(340)));
//G531: def_rr_cmd(342)      => rec_relation(17).
	p(NT(342), (NT(17)));
//G532: cli_command(231)     => def_rr_cmd(342).
	p(NT(231), (NT(342)));
//G533: def_input_cmd(343)   => input_def(18).
	p(NT(343), (NT(18)));
//G534: cli_command(231)     => def_input_cmd(343).
	p(NT(231), (NT(343)));
//G535: def_output_cmd(344)  => output_def(19).
	p(NT(344), (NT(19)));
//G536: cli_command(231)     => def_output_cmd(344).
	p(NT(231), (NT(344)));
//G537: __E___E_cli_command_157_158(348) => 'h' 'i' 's' 't'.
	p(NT(348), (T(54)+T(4)+T(15)+T(8)));
//G538: __E___E_cli_command_157_158(348) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(348), (T(54)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G539: history_sym(347)     => __E___E_cli_command_157_158(348).
	p(NT(347), (NT(348)));
//G540: __E_cli_command_157(346) => history_sym(347).
	p(NT(346), (NT(347)));
//G541: history_list_cmd(345) => __E_cli_command_157(346).
	p(NT(345), (NT(346)));
//G542: cli_command(231)     => history_list_cmd(345).
	p(NT(231), (NT(345)));
//G543: __E_cli_command_159(350) => history_sym(347) __(34) history(277).
	p(NT(350), (NT(347)+NT(34)+NT(277)));
//G544: history_print_cmd(349) => __E_cli_command_159(350).
	p(NT(349), (NT(350)));
//G545: cli_command(231)     => history_print_cmd(349).
	p(NT(231), (NT(349)));
//G546: __E_cli_command_160(352) => ref(27).
	p(NT(352), (NT(27)));
//G547: __E_cli_command_160(352) => wff(14).
	p(NT(352), (NT(14)));
//G548: __E_cli_command_160(352) => bf(30).
	p(NT(352), (NT(30)));
//G549: history_store_cmd(351) => __E_cli_command_160(352).
	p(NT(351), (NT(352)));
//G550: cli_command(231)     => history_store_cmd(351).
	p(NT(231), (NT(351)));
//G551: __E_solve_options_161(353) => __(34) solve_option(354).
	p(NT(353), (NT(34)+NT(354)));
//G552: __E_solve_options_162(355) => null.
	p(NT(355), (nul));
//G553: __E_solve_options_162(355) => __E_solve_options_161(353) __E_solve_options_162(355).
	p(NT(355), (NT(353)+NT(355)));
//G554: solve_options(267)   => __E_solve_options_162(355).
	p(NT(267), (NT(355)));
//G555: __E_solve_option_163(356) => solver_mode(357).
	p(NT(356), (NT(357)));
//G556: __E_solve_option_163(356) => type(223).
	p(NT(356), (NT(223)));
//G557: solve_option(354)    => '-' '-' __E_solve_option_163(356).
	p(NT(354), (T(30)+T(30)+NT(356)));
//G558: __E_solver_mode_164(359) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(359), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G559: __E_solver_mode_164(359) => 'm' 'i' 'n'.
	p(NT(359), (T(21)+T(4)+T(5)));
//G560: solver_mode_minimum(358) => __E_solver_mode_164(359).
	p(NT(358), (NT(359)));
//G561: solver_mode(357)     => solver_mode_minimum(358).
	p(NT(357), (NT(358)));
//G562: __E_solver_mode_165(361) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(361), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G563: __E_solver_mode_165(361) => 'm' 'a' 'x'.
	p(NT(361), (T(21)+T(16)+T(29)));
//G564: solver_mode_maximum(360) => __E_solver_mode_165(361).
	p(NT(360), (NT(361)));
//G565: solver_mode(357)     => solver_mode_maximum(360).
	p(NT(357), (NT(360)));
//G566: q_file_name(37)      => '"' file_name(362) '"'.
	p(NT(37), (T(59)+NT(362)+T(59)));
//G567: __E_file_name_166(363) => printable(8).
	p(NT(363), (NT(8)));
//G568: __E_file_name_166(363) => printable(8) __E_file_name_166(363).
	p(NT(363), (NT(8)+NT(363)));
//G569: file_name(362)       => __E_file_name_166(363).
	p(NT(362), (NT(363)));
//G570: __E_option_name_167(364) => alnum(6).
	p(NT(364), (NT(6)));
//G571: __E_option_name_167(364) => alnum(6) __E_option_name_167(364).
	p(NT(364), (NT(6)+NT(364)));
//G572: option_name(321)     => __E_option_name_167(364).
	p(NT(321), (NT(364)));
//G573: __E_option_value_168(365) => alnum(6).
	p(NT(365), (NT(6)));
//G574: __E_option_value_168(365) => alnum(6) __E_option_value_168(365).
	p(NT(365), (NT(6)+NT(365)));
//G575: option_value(326)    => __E_option_value_168(365).
	p(NT(326), (NT(365)));
//G576: bf_cmd_arg(366)      => history(277).
	p(NT(366), (NT(277)));
//G577: bf_cmd_arg(366)      => bf(30).
	p(NT(366), (NT(30)));
//G578: wff_cmd_arg(268)     => history(277).
	p(NT(268), (NT(277)));
//G579: wff_cmd_arg(268)     => wff(14).
	p(NT(268), (NT(14)));
//G580: nf_cmd_arg(286)      => history(277).
	p(NT(286), (NT(277)));
//G581: nf_cmd_arg(286)      => ref(27).
	p(NT(286), (NT(27)));
//G582: nf_cmd_arg(286)      => wff(14).
	p(NT(286), (NT(14)));
//G583: nf_cmd_arg(286)      => bf(30).
	p(NT(286), (NT(30)));
//G584: onf_cmd_arg(313)     => history(277).
	p(NT(313), (NT(277)));
//G585: onf_cmd_arg(313)     => wff(14).
	p(NT(313), (NT(14)));
//G586: normalize_cmd_arg(256) => history(277).
	p(NT(256), (NT(277)));
//G587: normalize_cmd_arg(256) => spec(10).
	p(NT(256), (NT(10)));
//G588: normalize_cmd_arg(256) => ref(27).
	p(NT(256), (NT(27)));
//G589: normalize_cmd_arg(256) => wff(14).
	p(NT(256), (NT(14)));
//G590: normalize_cmd_arg(256) => bf(30).
	p(NT(256), (NT(30)));
//G591: inst_args(291)       => wff_cmd_arg(268) _(11) '[' _(11) bf(30) _(11) '/' _(11) bf_cmd_arg(366) _(11) ']'.
	p(NT(291), (NT(268)+NT(11)+T(26)+NT(11)+NT(30)+NT(11)+T(40)+NT(11)+NT(366)+NT(11)+T(27)));
//G592: inst_args(291)       => bf_cmd_arg(366) _(11) '[' _(11) bf(30) _(11) '/' _(11) bf_cmd_arg(366) _(11) ']'.
	p(NT(291), (NT(366)+NT(11)+T(26)+NT(11)+NT(30)+NT(11)+T(40)+NT(11)+NT(366)+NT(11)+T(27)));
//G593: help_arg(248)        => help_sym(245).
	p(NT(248), (NT(245)));
//G594: help_arg(248)        => version_sym(238).
	p(NT(248), (NT(238)));
//G595: help_arg(248)        => quit_sym(235).
	p(NT(248), (NT(235)));
//G596: help_arg(248)        => clear_sym(241).
	p(NT(248), (NT(241)));
//G597: help_arg(248)        => get_sym(319).
	p(NT(248), (NT(319)));
//G598: help_arg(248)        => set_sym(324).
	p(NT(248), (NT(324)));
//G599: help_arg(248)        => enable_sym(329).
	p(NT(248), (NT(329)));
//G600: help_arg(248)        => disable_sym(332).
	p(NT(248), (NT(332)));
//G601: help_arg(248)        => toggle_sym(335).
	p(NT(248), (NT(335)));
//G602: help_arg(248)        => file_sym(251).
	p(NT(248), (NT(251)));
//G603: help_arg(248)        => history_sym(347).
	p(NT(248), (NT(347)));
//G604: help_arg(248)        => abs_history_sym(367).
	p(NT(248), (NT(367)));
//G605: help_arg(248)        => rel_history_sym(368).
	p(NT(248), (NT(368)));
//G606: help_arg(248)        => selection_sym(369).
	p(NT(248), (NT(369)));
//G607: help_arg(248)        => def_sym(338).
	p(NT(248), (NT(338)));
//G608: help_arg(248)        => inst_sym(289).
	p(NT(248), (NT(289)));
//G609: help_arg(248)        => subst_sym(284).
	p(NT(248), (NT(284)));
//G610: help_arg(248)        => normalize_sym(280).
	p(NT(248), (NT(280)));
//G611: help_arg(248)        => execute_sym(370).
	p(NT(248), (NT(370)));
//G612: help_arg(248)        => solve_sym(265).
	p(NT(248), (NT(265)));
//G613: help_arg(248)        => valid_sym(255).
	p(NT(248), (NT(255)));
//G614: help_arg(248)        => sat_sym(259).
	p(NT(248), (NT(259)));
//G615: help_arg(248)        => unsat_sym(262).
	p(NT(248), (NT(262)));
//G616: help_arg(248)        => run_sym(274).
	p(NT(248), (NT(274)));
//G617: help_arg(248)        => dnf_sym(294).
	p(NT(248), (NT(294)));
//G618: help_arg(248)        => cnf_sym(297).
	p(NT(248), (NT(297)));
//G619: help_arg(248)        => anf_sym(300).
	p(NT(248), (NT(300)));
//G620: help_arg(248)        => snf_sym(371).
	p(NT(248), (NT(371)));
//G621: help_arg(248)        => nnf_sym(303).
	p(NT(248), (NT(303)));
//G622: help_arg(248)        => pnf_sym(306).
	p(NT(248), (NT(306)));
//G623: help_arg(248)        => mnf_sym(309).
	p(NT(248), (NT(309)));
//G624: help_arg(248)        => onf_sym(312).
	p(NT(248), (NT(312)));
//G625: help_arg(248)        => qelim_sym(316).
	p(NT(248), (NT(316)));
//G626: __E___E_help_arg_169_170(374) => 's'.
	p(NT(374), (T(15)));
//G627: __E___E_help_arg_169_170(374) => null.
	p(NT(374), (nul));
//G628: __E_help_arg_169(373) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_169_170(374).
	p(NT(373), (T(11)+T(29)+T(16)+T(21)+T(55)+T(10)+T(11)+NT(374)));
//G629: examples_sym(372)    => __E_help_arg_169(373).
	p(NT(372), (NT(373)));
//G630: help_arg(248)        => examples_sym(372).
	p(NT(248), (NT(372)));
//G631: __E_history_171(376) => '%'.
	p(NT(376), (T(38)));
//G632: rel_history_sym(368) => '%' '-'.
	p(NT(368), (T(38)+T(30)));
//G633: history_id(378)      => digits(220).
	p(NT(378), (NT(220)));
//G634: __E___E_history_171_172(377) => history_id(378).
	p(NT(377), (NT(378)));
//G635: __E___E_history_171_172(377) => null.
	p(NT(377), (nul));
//G636: __E_history_171(376) => rel_history_sym(368) __E___E_history_171_172(377).
	p(NT(376), (NT(368)+NT(377)));
//G637: rel_history(375)     => __E_history_171(376).
	p(NT(375), (NT(376)));
//G638: history(277)         => rel_history(375).
	p(NT(277), (NT(375)));
//G639: abs_history_sym(367) => '%'.
	p(NT(367), (T(38)));
//G640: __E_history_173(380) => abs_history_sym(367) history_id(378).
	p(NT(380), (NT(367)+NT(378)));
//G641: abs_history(379)     => __E_history_173(380).
	p(NT(379), (NT(380)));
//G642: history(277)         => abs_history(379).
	p(NT(277), (NT(379)));
//G643: unreachable(381)     => processed(382).
	p(NT(381), (NT(382)));
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
		bf, io_var_name, __E_input_def_8, typed, __, stream, __E_output_def_9, q_file_name, console_sym, sym, 
		__E_ref_10, offsets, ref_args, __E_ref_11, __E_ref_12, fp_fallback, first_sym, last_sym, __E_ref_args_13, __E___E_ref_args_13_14, 
		ref_arg, __E___E_ref_args_13_15, __E___E_ref_args_13_16, tau_constant_source, __E_tau_constant_source_17, wff_parenthesis, __E_wff_18, wff_sometimes, __E_wff_19, __E___E_wff_19_20, 
		wff_always, __E_wff_21, __E___E_wff_21_22, wff_conditional, __E_wff_23, wff_all, __E_wff_24, q_vars, wff_ex, __E_wff_25, 
		wff_ref, constraint, wff_imply, __E_wff_26, wff_rimply, __E_wff_27, wff_equiv, __E_wff_28, wff_or, __E_wff_29, 
		wff_xor, __E_wff_30, wff_and, __E_wff_31, wff_neg, __E_wff_32, wff_t, wff_f, bf_interval, __E_wff_33, 
		bf_eq, __E_wff_34, bf_neq, __E_wff_35, bf_lt, __E_wff_36, bf_nlt, __E_wff_37, bf_lteq, __E_wff_38, 
		bf_nlteq, __E_wff_39, bf_gt, __E_wff_40, bf_ngt, __E_wff_41, bf_gteq, __E_wff_42, bf_ngteq, __E_wff_43, 
		bf_parenthesis, __E_bf_44, ba_constant, variable, bf_fall, __E_bf_45, bf_fex, __E_bf_46, bf_ref, bf_shr, 
		__E_bf_47, __E___E_bf_47_48, bf_shl, __E_bf_49, __E___E_bf_49_50, bf_add, __E_bf_51, bf_sub, __E_bf_52, __E___E_bf_52_53, 
		bf_mod, __E_bf_54, __E___E_bf_54_55, bf_mul, __E_bf_56, bf_div, __E_bf_57, __E___E_bf_57_58, bf_nor, __E_bf_59, 
		__E___E_bf_59_60, bf_xnor, __E_bf_61, __E___E_bf_61_62, bf_nand, __E_bf_63, __E___E_bf_63_64, bf_or, __E_bf_65, bf_xor, 
		__E_bf_66, bf_t, __E_bf_67, __E___E_bf_67_68, bf_f, __E_bf_69, __E___E_bf_69_70, bf_and, __E_bf_71, __E___E_bf_71_72, 
		bf_neg, __E_bf_73, bf_neg_oprnd, __E___E_bf_73_74, bf_and_nosep, __E_bf_75, bf_and_nosep_1st_oprnd, __E___E_bf_75_76, bf_and_nosep_2nd_oprnd, __E___E_bf_75_77, 
		ctn_neq, __E_constraint_78, ctnvar, num, ctn_eq, __E_constraint_79, ctn_gteq, __E_constraint_80, ctn_gt, __E_constraint_81, 
		ctn_lteq, __E_constraint_82, ctn_lt, __E_constraint_83, __E_ba_constant_84, source, __E_ba_constant_85, src_c, __E_source_86, __E___E_source_86_87, 
		__E___E_source_86_88, __E_src_c_89, __E_src_c_90, __E_variable_91, uconst, io_var, var_name, __E_variable_92, __E_var_name_93, __E_var_name_94, 
		__E_var_name_95, __E_var_name_96, __E_var_name_97, offset, chars, uconst_name, __E_uconst_98, __E___E_uconst_98_99, q_var, __E_q_vars_100, 
		__E_q_vars_101, __E_offsets_102, __E_offsets_103, integer, shift, __E_offset_104, __E_shift_105, __E___E_shift_105_106, __E_chars_107, __E_chars_108, 
		digits, __E_digits_109, __E_integer_110, type, __E_typed_111, subtype, comment, __E_comment_112, __E_comment_113, __E_comment_114, 
		cli, cli_command, __E_cli_115, __E_cli_116, quit_cmd, quit_sym, __E_cli_command_117, version_cmd, version_sym, __E_cli_command_118, 
		clear_cmd, clear_sym, __E_cli_command_119, help_cmd, __E_cli_command_120, help_sym, __E___E_cli_command_120_121, __E___E_cli_command_120_122, help_arg, file_cmd, 
		__E_cli_command_123, file_sym, q_string, valid_cmd, __E_cli_command_124, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_125, sat_sym, 
		unsat_cmd, __E_cli_command_126, unsat_sym, solve_cmd, __E_cli_command_127, solve_sym, __E___E_cli_command_127_128, solve_options, wff_cmd_arg, lgrs_cmd, 
		__E_cli_command_129, lgrs_sym, run_cmd, __E_cli_command_130, run_sym, __E___E_cli_command_130_131, __E___E_cli_command_130_132, history, normalize_cmd, __E_cli_command_133, 
		normalize_sym, __E___E_cli_command_133_134, subst_cmd, __E_cli_command_135, subst_sym, __E___E_cli_command_135_136, nf_cmd_arg, inst_cmd, __E_cli_command_137, inst_sym, 
		__E___E_cli_command_137_138, inst_args, dnf_cmd, __E_cli_command_139, dnf_sym, cnf_cmd, __E_cli_command_140, cnf_sym, anf_cmd, __E_cli_command_141, 
		anf_sym, nnf_cmd, __E_cli_command_142, nnf_sym, pnf_cmd, __E_cli_command_143, pnf_sym, mnf_cmd, __E_cli_command_144, mnf_sym, 
		onf_cmd, __E_cli_command_145, onf_sym, onf_cmd_arg, qelim_cmd, __E_cli_command_146, qelim_sym, get_cmd, __E_cli_command_147, get_sym, 
		__E___E_cli_command_147_148, option_name, set_cmd, __E_cli_command_149, set_sym, __E___E_cli_command_149_150, option_value, enable_cmd, __E_cli_command_151, enable_sym, 
		disable_cmd, __E_cli_command_152, disable_sym, toggle_cmd, __E_cli_command_153, toggle_sym, def_list_cmd, __E_cli_command_154, def_sym, __E___E_cli_command_154_155, 
		def_print_cmd, __E_cli_command_156, def_rr_cmd, def_input_cmd, def_output_cmd, history_list_cmd, __E_cli_command_157, history_sym, __E___E_cli_command_157_158, history_print_cmd, 
		__E_cli_command_159, history_store_cmd, __E_cli_command_160, __E_solve_options_161, solve_option, __E_solve_options_162, __E_solve_option_163, solver_mode, solver_mode_minimum, __E_solver_mode_164, 
		solver_mode_maximum, __E_solver_mode_165, file_name, __E_file_name_166, __E_option_name_167, __E_option_value_168, bf_cmd_arg, abs_history_sym, rel_history_sym, selection_sym, 
		execute_sym, snf_sym, examples_sym, __E_help_arg_169, __E___E_help_arg_169_170, rel_history, __E_history_171, __E___E_history_171_172, history_id, abs_history, 
		__E_history_173, unreachable, processed, __N_0, __N_1, __N_2, __N_3, __N_4, __N_5, __N_6, 
		__N_7, __N_8, __N_9, __N_10, __N_11, __N_12, __N_13, __N_14, __N_15, __N_16, 
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
