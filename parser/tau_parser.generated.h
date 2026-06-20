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
	"input_def", "output_def", "__E_definitions_4", "spec_multiline", "__E_spec_multiline_5", "spec_part", "__E_spec_multiline_6", "__E_spec_multiline_7", "ref", "__E_rec_relation_8", 
	"capture", "bf", "io_var_name", "__E_input_def_9", "typed", "__", "stream", "__E_output_def_10", "q_file_name", "console_sym", 
	"sym", "__E_ref_11", "offsets", "ref_args", "__E_ref_12", "__E_ref_13", "fp_fallback", "first_sym", "last_sym", "__E_ref_args_14", 
	"__E___E_ref_args_14_15", "ref_arg", "__E___E_ref_args_14_16", "__E___E_ref_args_14_17", "wff_parenthesis", "__E_wff_18", "wff_sometimes", "__E_wff_19", "__E___E_wff_19_20", "wff_always", 
	"__E_wff_21", "__E___E_wff_21_22", "wff_conditional", "__E_wff_23", "wff_all", "__E_wff_24", "q_vars", "wff_ex", "__E_wff_25", "wff_ref", 
	"constraint", "wff_imply", "__E_wff_26", "wff_rimply", "__E_wff_27", "wff_equiv", "__E_wff_28", "wff_or", "__E_wff_29", "wff_xor", 
	"__E_wff_30", "wff_and", "__E_wff_31", "wff_neg", "__E_wff_32", "wff_t", "wff_f", "bf_interval", "__E_wff_33", "bf_eq", 
	"__E_wff_34", "bf_neq", "__E_wff_35", "bf_lt", "__E_wff_36", "bf_nlt", "__E_wff_37", "bf_lteq", "__E_wff_38", "bf_nlteq", 
	"__E_wff_39", "bf_gt", "__E_wff_40", "bf_ngt", "__E_wff_41", "bf_gteq", "__E_wff_42", "bf_ngteq", "__E_wff_43", "bf_parenthesis", 
	"__E_bf_44", "ba_constant", "variable", "bf_fall", "__E_bf_45", "bf_fex", "__E_bf_46", "bf_ref", "bf_cast", "__E_bf_47", 
	"num", "bf_cast_oprnd", "__E___E_bf_47_48", "bf_t", "bf_f", "bf_neg", "bf_shr", "__E_bf_49", "__E___E_bf_49_50", "bf_shl", 
	"__E_bf_51", "__E___E_bf_51_52", "bf_add", "__E_bf_53", "bf_sub", "__E_bf_54", "__E___E_bf_54_55", "bf_mod", "__E_bf_56", "__E___E_bf_56_57", 
	"bf_mul", "__E_bf_58", "bf_div", "__E_bf_59", "__E___E_bf_59_60", "bf_nor", "__E_bf_61", "__E___E_bf_61_62", "bf_xnor", "__E_bf_63", 
	"__E___E_bf_63_64", "bf_nand", "__E_bf_65", "__E___E_bf_65_66", "bf_or", "__E_bf_67", "bf_xor", "__E_bf_68", "__E_bf_69", "__E___E_bf_69_70", 
	"__E_bf_71", "__E___E_bf_71_72", "bf_and", "__E_bf_73", "__E___E_bf_73_74", "__E_bf_75", "bf_neg_oprnd", "__E___E_bf_75_76", "bf_and_nosep", "__E_bf_77", 
	"bf_and_nosep_1st_oprnd", "__E___E_bf_77_78", "bf_and_nosep_2nd_oprnd", "__E___E_bf_77_79", "ctn_neq", "__E_constraint_80", "ctnvar", "ctn_eq", "__E_constraint_81", "ctn_gteq", 
	"__E_constraint_82", "ctn_gt", "__E_constraint_83", "ctn_lteq", "__E_constraint_84", "ctn_lt", "__E_constraint_85", "__E_ba_constant_86", "source", "__E_ba_constant_87", 
	"src_c", "__E_source_88", "__E___E_source_88_89", "__E___E_source_88_90", "__E_src_c_91", "__E_src_c_92", "__E_variable_93", "uconst", "io_var", "var_name", 
	"__E_variable_94", "__E_var_name_95", "__E_var_name_96", "__E_var_name_97", "__E_var_name_98", "__E_var_name_99", "offset", "chars", "uconst_name", "__E_uconst_100", 
	"__E___E_uconst_100_101", "q_var", "__E_q_vars_102", "__E_q_vars_103", "__E_offsets_104", "__E_offsets_105", "integer", "shift", "__E_offset_106", "__E_shift_107", 
	"__E___E_shift_107_108", "__E_chars_109", "__E_chars_110", "digits", "__E_digits_111", "__E_integer_112", "type", "subtype", "__E_typed_113", "comment", 
	"__E_comment_114", "__E_comment_115", "__E_comment_116", "cli", "cli_command", "__E_cli_117", "__E_cli_118", "__E_cli_119", "quit_cmd", "quit_sym", 
	"__E_cli_command_120", "version_cmd", "version_sym", "__E_cli_command_121", "clear_cmd", "clear_sym", "__E_cli_command_122", "help_cmd", "__E_cli_command_123", "help_sym", 
	"__E___E_cli_command_123_124", "__E___E_cli_command_123_125", "help_arg", "file_cmd", "__E_cli_command_126", "file_sym", "q_string", "valid_cmd", "__E_cli_command_127", "valid_sym", 
	"spec_arg", "sat_cmd", "__E_cli_command_128", "sat_sym", "unsat_cmd", "__E_cli_command_129", "unsat_sym", "solve_cmd", "__E_cli_command_130", "solve_sym", 
	"__E___E_cli_command_130_131", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_132", "lgrs_sym", "run_cmd", "__E_cli_command_133", "run_sym", "__E___E_cli_command_133_134", 
	"normalize_cmd", "__E_cli_command_135", "normalize_sym", "__E___E_cli_command_135_136", "subst_cmd", "__E_cli_command_137", "subst_sym", "__E___E_cli_command_137_138", "nf_cmd_arg", "inst_cmd", 
	"__E_cli_command_139", "inst_sym", "__E___E_cli_command_139_140", "inst_args", "dnf_cmd", "__E_cli_command_141", "dnf_sym", "cnf_cmd", "__E_cli_command_142", "cnf_sym", 
	"anf_cmd", "__E_cli_command_143", "anf_sym", "nnf_cmd", "__E_cli_command_144", "nnf_sym", "pnf_cmd", "__E_cli_command_145", "pnf_sym", "mnf_cmd", 
	"__E_cli_command_146", "mnf_sym", "onf_cmd", "__E_cli_command_147", "onf_sym", "onf_cmd_arg", "qelim_cmd", "__E_cli_command_148", "qelim_sym", "get_cmd", 
	"__E_cli_command_149", "get_sym", "__E___E_cli_command_149_150", "option_name", "set_cmd", "__E_cli_command_151", "set_sym", "__E___E_cli_command_151_152", "option_value", "enable_cmd", 
	"__E_cli_command_153", "enable_sym", "disable_cmd", "__E_cli_command_154", "disable_sym", "toggle_cmd", "__E_cli_command_155", "toggle_sym", "def_list_cmd", "__E_cli_command_156", 
	"def_sym", "__E___E_cli_command_156_157", "def_print_cmd", "__E_cli_command_158", "def_rr_cmd", "def_input_cmd", "def_output_cmd", "history_list_cmd", "__E_cli_command_159", "history_sym", 
	"__E___E_cli_command_159_160", "history_print_cmd", "__E_cli_command_161", "history", "history_store_cmd", "__E_cli_command_162", "__E_solve_options_163", "solve_option", "__E_solve_options_164", "__E_solve_option_165", 
	"solver_mode", "solver_mode_minimum", "__E_solver_mode_166", "solver_mode_maximum", "__E_solver_mode_167", "file_name", "__E_file_name_168", "__E_option_name_169", "__E_option_value_170", "bf_cmd_arg", 
	"abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", "snf_sym", "examples_sym", "__E_help_arg_171", "__E___E_help_arg_171_172", "rel_history", "__E_history_173", 
	"__E___E_history_173_174", "history_id", "abs_history", "__E_history_175", "unreachable", "BDD_ID", "stream_def", "fm_or_term", "__N_0", "__N_1", 
	"__N_2", "__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", "__N_9", "__N_10", "__N_11", 
	"__N_12", "__N_13", "__N_14", "__N_15", "__N_16", "__N_17", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', 'a', 'b', 'k', 
	'r', ',', 'm', '<', '>', 'w', 'y', '[', ']', '?', 
	'x', '-', '|', '^', '&', '!', 'T', 'F', 'v', '+', 
	'%', '*', '/', '1', '0', '\'', '{', '}', '_', '$', 
	'\t', '\n', '\r', '#', 'q', 'h', 'p', 'd', 'g', 'z', 
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
			13, 35
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			30, 32, 40, 176, 188, 199, 208, 216, 223, 226,
			227, 323, 328, 365
		},
		.to_inline = {
			{ 15, 54, 15 },
			{ 25 },
			{ 31, 109, 31 },
			{ 121, 109, 31 },
			{ 166, 109, 31 },
			{ 168, 109, 162 },
			{ 170, 109, 31 },
			{ 172, 109, 31 },
			{ 190 },
			{ 197 },
			{ 197, 208 },
			{ 207 },
			{ 211 },
			{ 260 },
			{ 272 },
			{ 288 },
			{ 293 },
			{ 315 },
			{ 369 }
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
//G10:  __E_definitions_2(17) => _(13) __E___E_definitions_2_3(18) _(13) '.'.
	p(NT(17), (NT(13)+NT(18)+NT(13)+T(1)));
//G11:  __E_definitions_4(22) => __E_definitions_2(17).
	p(NT(22), (NT(17)));
//G12:  __E_definitions_4(22) => __E_definitions_2(17) __E_definitions_4(22).
	p(NT(22), (NT(17)+NT(22)));
//G13:  definitions(12)      => __E_definitions_4(22).
	p(NT(12), (NT(22)));
//G14:  __E_spec_multiline_5(24) => _(13) spec_part(25) _(13) '.'.
	p(NT(24), (NT(13)+NT(25)+NT(13)+T(1)));
//G15:  __E_spec_multiline_6(26) => null.
	p(NT(26), (nul));
//G16:  __E_spec_multiline_6(26) => __E_spec_multiline_5(24) __E_spec_multiline_6(26).
	p(NT(26), (NT(24)+NT(26)));
//G17:  __E_spec_multiline_7(27) => _(13) '.'.
	p(NT(27), (NT(13)+T(1)));
//G18:  __E_spec_multiline_7(27) => null.
	p(NT(27), (nul));
//G19:  spec_multiline(23)   => __E_spec_multiline_6(26) _(13) spec_part(25) __E_spec_multiline_7(27) _(13).
	p(NT(23), (NT(26)+NT(13)+NT(25)+NT(27)+NT(13)));
//G20:  spec_multiline(23)   => _(13).
	p(NT(23), (NT(13)));
//G21:  spec_part(25)        => rec_relation(19).
	p(NT(25), (NT(19)));
//G22:  spec_part(25)        => input_def(20).
	p(NT(25), (NT(20)));
//G23:  spec_part(25)        => output_def(21).
	p(NT(25), (NT(21)));
//G24:  spec_part(25)        => main(14).
	p(NT(25), (NT(14)));
//G25:  __E_rec_relation_8(29) => capture(30).
	p(NT(29), (NT(30)));
//G26:  __E_rec_relation_8(29) => ref(28).
	p(NT(29), (NT(28)));
//G27:  __E_rec_relation_8(29) => wff(15).
	p(NT(29), (NT(15)));
//G28:  __E_rec_relation_8(29) => bf(31).
	p(NT(29), (NT(31)));
//G29:  rec_relation(19)     => ref(28) _(13) ':' '=' _(13) __E_rec_relation_8(29).
	p(NT(19), (NT(28)+NT(13)+T(2)+T(3)+NT(13)+NT(29)));
//G30:  __E_input_def_9(33)  => typed(34).
	p(NT(33), (NT(34)));
//G31:  __E_input_def_9(33)  => null.
	p(NT(33), (nul));
//G32:  input_def(20)        => io_var_name(32) __E_input_def_9(33) _(13) ':' '=' _(13) 'i' 'n' __(35) stream(36).
	p(NT(20), (NT(32)+NT(33)+NT(13)+T(2)+T(3)+NT(13)+T(4)+T(5)+NT(35)+NT(36)));
//G33:  __E_output_def_10(37) => typed(34).
	p(NT(37), (NT(34)));
//G34:  __E_output_def_10(37) => null.
	p(NT(37), (nul));
//G35:  output_def(21)       => io_var_name(32) __E_output_def_10(37) _(13) ':' '=' _(13) 'o' 'u' 't' __(35) stream(36).
	p(NT(21), (NT(32)+NT(37)+NT(13)+T(2)+T(3)+NT(13)+T(6)+T(7)+T(8)+NT(35)+NT(36)));
//G36:  stream(36)           => 'f' 'i' 'l' 'e' _(13) '(' _(13) q_file_name(38) _(13) ')'.
	p(NT(36), (T(9)+T(4)+T(10)+T(11)+NT(13)+T(12)+NT(13)+NT(38)+NT(13)+T(13)));
//G37:  console_sym(39)      => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(39), (T(14)+T(6)+T(5)+T(15)+T(6)+T(10)+T(11)));
//G38:  stream(36)           => console_sym(39).
	p(NT(36), (NT(39)));
//G39:  __E_ref_11(41)       => offsets(42).
	p(NT(41), (NT(42)));
//G40:  __E_ref_11(41)       => null.
	p(NT(41), (nul));
//G41:  __E_ref_12(44)       => typed(34).
	p(NT(44), (NT(34)));
//G42:  __E_ref_12(44)       => null.
	p(NT(44), (nul));
//G43:  __E_ref_13(45)       => _(13) 'f' 'a' 'l' 'l' 'b' 'a' 'c' 'k' __(35) fp_fallback(46).
	p(NT(45), (NT(13)+T(9)+T(16)+T(10)+T(10)+T(17)+T(16)+T(14)+T(18)+NT(35)+NT(46)));
//G44:  __E_ref_13(45)       => null.
	p(NT(45), (nul));
//G45:  ref(28)              => sym(40) __E_ref_11(41) ref_args(43) __E_ref_12(44) __E_ref_13(45).
	p(NT(28), (NT(40)+NT(41)+NT(43)+NT(44)+NT(45)));
//G46:  first_sym(47)        => 'f' 'i' 'r' 's' 't'.
	p(NT(47), (T(9)+T(4)+T(19)+T(15)+T(8)));
//G47:  fp_fallback(46)      => first_sym(47).
	p(NT(46), (NT(47)));
//G48:  last_sym(48)         => 'l' 'a' 's' 't'.
	p(NT(48), (T(10)+T(16)+T(15)+T(8)));
//G49:  fp_fallback(46)      => last_sym(48).
	p(NT(46), (NT(48)));
//G50:  fp_fallback(46)      => capture(30).
	p(NT(46), (NT(30)));
//G51:  fp_fallback(46)      => ref(28).
	p(NT(46), (NT(28)));
//G52:  fp_fallback(46)      => wff(15).
	p(NT(46), (NT(15)));
//G53:  fp_fallback(46)      => bf(31).
	p(NT(46), (NT(31)));
//G54:  ref_arg(51)          => bf(31).
	p(NT(51), (NT(31)));
//G55:  __E___E_ref_args_14_15(50) => _(13) ref_arg(51).
	p(NT(50), (NT(13)+NT(51)));
//G56:  __E___E_ref_args_14_16(52) => _(13) ',' _(13) ref_arg(51).
	p(NT(52), (NT(13)+T(20)+NT(13)+NT(51)));
//G57:  __E___E_ref_args_14_17(53) => null.
	p(NT(53), (nul));
//G58:  __E___E_ref_args_14_17(53) => __E___E_ref_args_14_16(52) __E___E_ref_args_14_17(53).
	p(NT(53), (NT(52)+NT(53)));
//G59:  __E_ref_args_14(49)  => __E___E_ref_args_14_15(50) __E___E_ref_args_14_17(53).
	p(NT(49), (NT(50)+NT(53)));
//G60:  __E_ref_args_14(49)  => null.
	p(NT(49), (nul));
//G61:  ref_args(43)         => '(' __E_ref_args_14(49) _(13) ')'.
	p(NT(43), (T(12)+NT(49)+NT(13)+T(13)));
//G62:  __E_wff_18(55)       => '(' _(13) wff(15) _(13) ')'.
	p(NT(55), (T(12)+NT(13)+NT(15)+NT(13)+T(13)));
//G63:  wff_parenthesis(54)  => __E_wff_18(55).
	p(NT(54), (NT(55)));
//G64:  wff(15)              => wff_parenthesis(54).
	p(NT(15), (NT(54)));
//G65:  __E___E_wff_19_20(58) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(58), (T(15)+T(6)+T(21)+T(11)+T(8)+T(4)+T(21)+T(11)+T(15)));
//G66:  __E___E_wff_19_20(58) => '<' '>'.
	p(NT(58), (T(22)+T(23)));
//G67:  __E_wff_19(57)       => __E___E_wff_19_20(58) _(13) wff(15).
	p(NT(57), (NT(58)+NT(13)+NT(15)));
//G68:  wff_sometimes(56)    => __E_wff_19(57).
	p(NT(56), (NT(57)));
//G69:  wff(15)              => wff_sometimes(56).
	p(NT(15), (NT(56)));
//G70:  __E___E_wff_21_22(61) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(61), (T(16)+T(10)+T(24)+T(16)+T(25)+T(15)));
//G71:  __E___E_wff_21_22(61) => '[' ']'.
	p(NT(61), (T(26)+T(27)));
//G72:  __E_wff_21(60)       => __E___E_wff_21_22(61) _(13) wff(15).
	p(NT(60), (NT(61)+NT(13)+NT(15)));
//G73:  wff_always(59)       => __E_wff_21(60).
	p(NT(59), (NT(60)));
//G74:  wff(15)              => wff_always(59).
	p(NT(15), (NT(59)));
//G75:  __E_wff_23(63)       => wff(15) _(13) '?' _(13) wff(15) _(13) ':' _(13) wff(15).
	p(NT(63), (NT(15)+NT(13)+T(28)+NT(13)+NT(15)+NT(13)+T(2)+NT(13)+NT(15)));
//G76:  wff_conditional(62)  => __E_wff_23(63).
	p(NT(62), (NT(63)));
//G77:  wff(15)              => wff_conditional(62).
	p(NT(15), (NT(62)));
//G78:  __E_wff_24(65)       => 'a' 'l' 'l' __(35) q_vars(66) __(35) wff(15).
	p(NT(65), (T(16)+T(10)+T(10)+NT(35)+NT(66)+NT(35)+NT(15)));
//G79:  wff_all(64)          => __E_wff_24(65).
	p(NT(64), (NT(65)));
//G80:  wff(15)              => wff_all(64).
	p(NT(15), (NT(64)));
//G81:  __E_wff_25(68)       => 'e' 'x' __(35) q_vars(66) __(35) wff(15).
	p(NT(68), (T(11)+T(29)+NT(35)+NT(66)+NT(35)+NT(15)));
//G82:  wff_ex(67)           => __E_wff_25(68).
	p(NT(67), (NT(68)));
//G83:  wff(15)              => wff_ex(67).
	p(NT(15), (NT(67)));
//G84:  wff_ref(69)          => ref(28).
	p(NT(69), (NT(28)));
//G85:  wff(15)              => wff_ref(69).
	p(NT(15), (NT(69)));
//G86:  wff(15)              => constraint(70).
	p(NT(15), (NT(70)));
//G87:  __E_wff_26(72)       => wff(15) _(13) '-' '>' _(13) wff(15).
	p(NT(72), (NT(15)+NT(13)+T(30)+T(23)+NT(13)+NT(15)));
//G88:  wff_imply(71)        => __E_wff_26(72).
	p(NT(71), (NT(72)));
//G89:  wff(15)              => wff_imply(71).
	p(NT(15), (NT(71)));
//G90:  __E_wff_27(74)       => wff(15) _(13) '<' '-' _(13) wff(15).
	p(NT(74), (NT(15)+NT(13)+T(22)+T(30)+NT(13)+NT(15)));
//G91:  wff_rimply(73)       => __E_wff_27(74).
	p(NT(73), (NT(74)));
//G92:  wff(15)              => wff_rimply(73).
	p(NT(15), (NT(73)));
//G93:  __E_wff_28(76)       => wff(15) _(13) '<' '-' '>' _(13) wff(15).
	p(NT(76), (NT(15)+NT(13)+T(22)+T(30)+T(23)+NT(13)+NT(15)));
//G94:  wff_equiv(75)        => __E_wff_28(76).
	p(NT(75), (NT(76)));
//G95:  wff(15)              => wff_equiv(75).
	p(NT(15), (NT(75)));
//G96:  __E_wff_29(78)       => wff(15) _(13) '|' '|' _(13) wff(15).
	p(NT(78), (NT(15)+NT(13)+T(31)+T(31)+NT(13)+NT(15)));
//G97:  wff_or(77)           => __E_wff_29(78).
	p(NT(77), (NT(78)));
//G98:  wff(15)              => wff_or(77).
	p(NT(15), (NT(77)));
//G99:  __E_wff_30(80)       => wff(15) _(13) '^' '^' _(13) wff(15).
	p(NT(80), (NT(15)+NT(13)+T(32)+T(32)+NT(13)+NT(15)));
//G100: wff_xor(79)          => __E_wff_30(80).
	p(NT(79), (NT(80)));
//G101: wff(15)              => wff_xor(79).
	p(NT(15), (NT(79)));
//G102: __E_wff_31(82)       => wff(15) _(13) '&' '&' _(13) wff(15).
	p(NT(82), (NT(15)+NT(13)+T(33)+T(33)+NT(13)+NT(15)));
//G103: wff_and(81)          => __E_wff_31(82).
	p(NT(81), (NT(82)));
//G104: wff(15)              => wff_and(81).
	p(NT(15), (NT(81)));
//G105: __E_wff_32(84)       => '!' _(13) wff(15).
	p(NT(84), (T(34)+NT(13)+NT(15)));
//G106: wff_neg(83)          => __E_wff_32(84).
	p(NT(83), (NT(84)));
//G107: wff(15)              => wff_neg(83).
	p(NT(15), (NT(83)));
//G108: wff_t(85)            => 'T'.
	p(NT(85), (T(35)));
//G109: wff(15)              => wff_t(85).
	p(NT(15), (NT(85)));
//G110: wff_f(86)            => 'F'.
	p(NT(86), (T(36)));
//G111: wff(15)              => wff_f(86).
	p(NT(15), (NT(86)));
//G112: __E_wff_33(88)       => bf(31) _(13) '<' '=' _(13) bf(31) _(13) '<' '=' _(13) bf(31).
	p(NT(88), (NT(31)+NT(13)+T(22)+T(3)+NT(13)+NT(31)+NT(13)+T(22)+T(3)+NT(13)+NT(31)));
//G113: bf_interval(87)      => __E_wff_33(88).
	p(NT(87), (NT(88)));
//G114: wff(15)              => bf_interval(87).
	p(NT(15), (NT(87)));
//G115: __E_wff_34(90)       => bf(31) _(13) '=' _(13) bf(31).
	p(NT(90), (NT(31)+NT(13)+T(3)+NT(13)+NT(31)));
//G116: bf_eq(89)            => __E_wff_34(90).
	p(NT(89), (NT(90)));
//G117: wff(15)              => bf_eq(89).
	p(NT(15), (NT(89)));
//G118: __E_wff_35(92)       => bf(31) _(13) '!' '=' _(13) bf(31).
	p(NT(92), (NT(31)+NT(13)+T(34)+T(3)+NT(13)+NT(31)));
//G119: bf_neq(91)           => __E_wff_35(92).
	p(NT(91), (NT(92)));
//G120: wff(15)              => bf_neq(91).
	p(NT(15), (NT(91)));
//G121: __E_wff_36(94)       => bf(31) _(13) '<' _(13) bf(31).
	p(NT(94), (NT(31)+NT(13)+T(22)+NT(13)+NT(31)));
//G122: bf_lt(93)            => __E_wff_36(94).
	p(NT(93), (NT(94)));
//G123: wff(15)              => bf_lt(93).
	p(NT(15), (NT(93)));
//G124: __E_wff_37(96)       => bf(31) _(13) '!' '<' _(13) bf(31).
	p(NT(96), (NT(31)+NT(13)+T(34)+T(22)+NT(13)+NT(31)));
//G125: bf_nlt(95)           => __E_wff_37(96).
	p(NT(95), (NT(96)));
//G126: wff(15)              => bf_nlt(95).
	p(NT(15), (NT(95)));
//G127: __E_wff_38(98)       => bf(31) _(13) '<' '=' _(13) bf(31).
	p(NT(98), (NT(31)+NT(13)+T(22)+T(3)+NT(13)+NT(31)));
//G128: bf_lteq(97)          => __E_wff_38(98).
	p(NT(97), (NT(98)));
//G129: wff(15)              => bf_lteq(97).
	p(NT(15), (NT(97)));
//G130: __E_wff_39(100)      => bf(31) _(13) '!' '<' '=' _(13) bf(31).
	p(NT(100), (NT(31)+NT(13)+T(34)+T(22)+T(3)+NT(13)+NT(31)));
//G131: bf_nlteq(99)         => __E_wff_39(100).
	p(NT(99), (NT(100)));
//G132: wff(15)              => bf_nlteq(99).
	p(NT(15), (NT(99)));
//G133: __E_wff_40(102)      => bf(31) _(13) '>' _(13) bf(31).
	p(NT(102), (NT(31)+NT(13)+T(23)+NT(13)+NT(31)));
//G134: bf_gt(101)           => __E_wff_40(102).
	p(NT(101), (NT(102)));
//G135: wff(15)              => bf_gt(101).
	p(NT(15), (NT(101)));
//G136: __E_wff_41(104)      => bf(31) _(13) '!' '>' _(13) bf(31).
	p(NT(104), (NT(31)+NT(13)+T(34)+T(23)+NT(13)+NT(31)));
//G137: bf_ngt(103)          => __E_wff_41(104).
	p(NT(103), (NT(104)));
//G138: wff(15)              => bf_ngt(103).
	p(NT(15), (NT(103)));
//G139: __E_wff_42(106)      => bf(31) _(13) '>' '=' _(13) bf(31).
	p(NT(106), (NT(31)+NT(13)+T(23)+T(3)+NT(13)+NT(31)));
//G140: bf_gteq(105)         => __E_wff_42(106).
	p(NT(105), (NT(106)));
//G141: wff(15)              => bf_gteq(105).
	p(NT(15), (NT(105)));
//G142: __E_wff_43(108)      => bf(31) _(13) '!' '>' '=' _(13) bf(31).
	p(NT(108), (NT(31)+NT(13)+T(34)+T(23)+T(3)+NT(13)+NT(31)));
//G143: bf_ngteq(107)        => __E_wff_43(108).
	p(NT(107), (NT(108)));
//G144: wff(15)              => bf_ngteq(107).
	p(NT(15), (NT(107)));
//G145: wff(15)              => capture(30).
	p(NT(15), (NT(30)));
//G146: __E_bf_44(110)       => '(' _(13) bf(31) _(13) ')'.
	p(NT(110), (T(12)+NT(13)+NT(31)+NT(13)+T(13)));
//G147: bf_parenthesis(109)  => __E_bf_44(110).
	p(NT(109), (NT(110)));
//G148: bf(31)               => bf_parenthesis(109).
	p(NT(31), (NT(109)));
//G149: bf(31)               => ba_constant(111).
	p(NT(31), (NT(111)));
//G150: bf(31)               => variable(112).
	p(NT(31), (NT(112)));
//G151: __E_bf_45(114)       => 'f' 'a' 'l' 'l' __(35) q_vars(66) __(35) bf(31).
	p(NT(114), (T(9)+T(16)+T(10)+T(10)+NT(35)+NT(66)+NT(35)+NT(31)));
//G152: bf_fall(113)         => __E_bf_45(114).
	p(NT(113), (NT(114)));
//G153: bf(31)               => bf_fall(113).
	p(NT(31), (NT(113)));
//G154: __E_bf_46(116)       => 'f' 'e' 'x' __(35) q_vars(66) __(35) bf(31).
	p(NT(116), (T(9)+T(11)+T(29)+NT(35)+NT(66)+NT(35)+NT(31)));
//G155: bf_fex(115)          => __E_bf_46(116).
	p(NT(115), (NT(116)));
//G156: bf(31)               => bf_fex(115).
	p(NT(31), (NT(115)));
//G157: bf_ref(117)          => ref(28).
	p(NT(117), (NT(28)));
//G158: bf(31)               => bf_ref(117).
	p(NT(31), (NT(117)));
//G159: __E___E_bf_47_48(122) => bf_parenthesis(109).
	p(NT(122), (NT(109)));
//G160: __E___E_bf_47_48(122) => ba_constant(111).
	p(NT(122), (NT(111)));
//G161: __E___E_bf_47_48(122) => variable(112).
	p(NT(122), (NT(112)));
//G162: __E___E_bf_47_48(122) => bf_ref(117).
	p(NT(122), (NT(117)));
//G163: __E___E_bf_47_48(122) => bf_t(123).
	p(NT(122), (NT(123)));
//G164: __E___E_bf_47_48(122) => bf_f(124).
	p(NT(122), (NT(124)));
//G165: __E___E_bf_47_48(122) => bf_neg(125).
	p(NT(122), (NT(125)));
//G166: __E___E_bf_47_48(122) => bf_cast(118).
	p(NT(122), (NT(118)));
//G167: __E___E_bf_47_48(122) => capture(30).
	p(NT(122), (NT(30)));
//G168: bf_cast_oprnd(121)   => __E___E_bf_47_48(122).
	p(NT(121), (NT(122)));
//G169: __E_bf_47(119)       => '(' _(13) 'b' 'v' _(13) '[' _(13) num(120) _(13) ']' _(13) ')' _(13) bf_cast_oprnd(121).
	p(NT(119), (T(12)+NT(13)+T(17)+T(37)+NT(13)+T(26)+NT(13)+NT(120)+NT(13)+T(27)+NT(13)+T(13)+NT(13)+NT(121)));
//G170: bf_cast(118)         => __E_bf_47(119).
	p(NT(118), (NT(119)));
//G171: bf(31)               => bf_cast(118).
	p(NT(31), (NT(118)));
//G172: __N_0(388)           => bf_shr(126).
	p(NT(388), (NT(126)));
//G173: __E___E_bf_49_50(128) => bf(31) & ~( __N_0(388) ).	 # conjunctive
	p(NT(128), (NT(31)) & ~(NT(388)));
//G174: __E_bf_49(127)       => bf(31) _(13) '>' '>' _(13) __E___E_bf_49_50(128).
	p(NT(127), (NT(31)+NT(13)+T(23)+T(23)+NT(13)+NT(128)));
//G175: bf_shr(126)          => __E_bf_49(127).
	p(NT(126), (NT(127)));
//G176: bf(31)               => bf_shr(126).
	p(NT(31), (NT(126)));
//G177: __N_1(389)           => bf_shl(129).
	p(NT(389), (NT(129)));
//G178: __E___E_bf_51_52(131) => bf(31) & ~( __N_1(389) ).	 # conjunctive
	p(NT(131), (NT(31)) & ~(NT(389)));
//G179: __E_bf_51(130)       => bf(31) _(13) '<' '<' _(13) __E___E_bf_51_52(131).
	p(NT(130), (NT(31)+NT(13)+T(22)+T(22)+NT(13)+NT(131)));
//G180: bf_shl(129)          => __E_bf_51(130).
	p(NT(129), (NT(130)));
//G181: bf(31)               => bf_shl(129).
	p(NT(31), (NT(129)));
//G182: __E_bf_53(133)       => bf(31) _(13) '+' _(13) bf(31).
	p(NT(133), (NT(31)+NT(13)+T(38)+NT(13)+NT(31)));
//G183: bf_add(132)          => __E_bf_53(133).
	p(NT(132), (NT(133)));
//G184: bf(31)               => bf_add(132).
	p(NT(31), (NT(132)));
//G185: __N_2(390)           => bf_sub(134).
	p(NT(390), (NT(134)));
//G186: __E___E_bf_54_55(136) => bf(31) & ~( __N_2(390) ).	 # conjunctive
	p(NT(136), (NT(31)) & ~(NT(390)));
//G187: __E_bf_54(135)       => bf(31) _(13) '-' _(13) __E___E_bf_54_55(136).
	p(NT(135), (NT(31)+NT(13)+T(30)+NT(13)+NT(136)));
//G188: bf_sub(134)          => __E_bf_54(135).
	p(NT(134), (NT(135)));
//G189: bf(31)               => bf_sub(134).
	p(NT(31), (NT(134)));
//G190: __N_3(391)           => bf_mod(137).
	p(NT(391), (NT(137)));
//G191: __E___E_bf_56_57(139) => bf(31) & ~( __N_3(391) ).	 # conjunctive
	p(NT(139), (NT(31)) & ~(NT(391)));
//G192: __E_bf_56(138)       => bf(31) _(13) '%' _(13) __E___E_bf_56_57(139).
	p(NT(138), (NT(31)+NT(13)+T(39)+NT(13)+NT(139)));
//G193: bf_mod(137)          => __E_bf_56(138).
	p(NT(137), (NT(138)));
//G194: bf(31)               => bf_mod(137).
	p(NT(31), (NT(137)));
//G195: __E_bf_58(141)       => bf(31) _(13) '*' _(13) bf(31).
	p(NT(141), (NT(31)+NT(13)+T(40)+NT(13)+NT(31)));
//G196: bf_mul(140)          => __E_bf_58(141).
	p(NT(140), (NT(141)));
//G197: bf(31)               => bf_mul(140).
	p(NT(31), (NT(140)));
//G198: __N_4(392)           => bf_div(142).
	p(NT(392), (NT(142)));
//G199: __E___E_bf_59_60(144) => bf(31) & ~( __N_4(392) ).	 # conjunctive
	p(NT(144), (NT(31)) & ~(NT(392)));
//G200: __E_bf_59(143)       => bf(31) _(13) '/' _(13) __E___E_bf_59_60(144).
	p(NT(143), (NT(31)+NT(13)+T(41)+NT(13)+NT(144)));
//G201: bf_div(142)          => __E_bf_59(143).
	p(NT(142), (NT(143)));
//G202: bf(31)               => bf_div(142).
	p(NT(31), (NT(142)));
//G203: __N_5(393)           => bf_nor(145).
	p(NT(393), (NT(145)));
//G204: __E___E_bf_61_62(147) => bf(31) & ~( __N_5(393) ).	 # conjunctive
	p(NT(147), (NT(31)) & ~(NT(393)));
//G205: __E_bf_61(146)       => bf(31) _(13) '!' '|' _(13) __E___E_bf_61_62(147).
	p(NT(146), (NT(31)+NT(13)+T(34)+T(31)+NT(13)+NT(147)));
//G206: bf_nor(145)          => __E_bf_61(146).
	p(NT(145), (NT(146)));
//G207: bf(31)               => bf_nor(145).
	p(NT(31), (NT(145)));
//G208: __N_6(394)           => bf_xnor(148).
	p(NT(394), (NT(148)));
//G209: __E___E_bf_63_64(150) => bf(31) & ~( __N_6(394) ).	 # conjunctive
	p(NT(150), (NT(31)) & ~(NT(394)));
//G210: __E_bf_63(149)       => bf(31) _(13) '!' '^' _(13) __E___E_bf_63_64(150).
	p(NT(149), (NT(31)+NT(13)+T(34)+T(32)+NT(13)+NT(150)));
//G211: bf_xnor(148)         => __E_bf_63(149).
	p(NT(148), (NT(149)));
//G212: bf(31)               => bf_xnor(148).
	p(NT(31), (NT(148)));
//G213: __N_7(395)           => bf_nand(151).
	p(NT(395), (NT(151)));
//G214: __E___E_bf_65_66(153) => bf(31) & ~( __N_7(395) ).	 # conjunctive
	p(NT(153), (NT(31)) & ~(NT(395)));
//G215: __E_bf_65(152)       => bf(31) _(13) '!' '&' _(13) __E___E_bf_65_66(153).
	p(NT(152), (NT(31)+NT(13)+T(34)+T(33)+NT(13)+NT(153)));
//G216: bf_nand(151)         => __E_bf_65(152).
	p(NT(151), (NT(152)));
//G217: bf(31)               => bf_nand(151).
	p(NT(31), (NT(151)));
//G218: __E_bf_67(155)       => bf(31) _(13) '|' _(13) bf(31).
	p(NT(155), (NT(31)+NT(13)+T(31)+NT(13)+NT(31)));
//G219: bf_or(154)           => __E_bf_67(155).
	p(NT(154), (NT(155)));
//G220: bf(31)               => bf_or(154).
	p(NT(31), (NT(154)));
//G221: __E_bf_68(157)       => bf(31) _(13) '^' _(13) bf(31).
	p(NT(157), (NT(31)+NT(13)+T(32)+NT(13)+NT(31)));
//G222: bf_xor(156)          => __E_bf_68(157).
	p(NT(156), (NT(157)));
//G223: bf(31)               => bf_xor(156).
	p(NT(31), (NT(156)));
//G224: __E___E_bf_69_70(159) => typed(34).
	p(NT(159), (NT(34)));
//G225: __E___E_bf_69_70(159) => null.
	p(NT(159), (nul));
//G226: __E_bf_69(158)       => '1' __E___E_bf_69_70(159).
	p(NT(158), (T(42)+NT(159)));
//G227: bf_t(123)            => __E_bf_69(158).
	p(NT(123), (NT(158)));
//G228: bf(31)               => bf_t(123).
	p(NT(31), (NT(123)));
//G229: __E___E_bf_71_72(161) => typed(34).
	p(NT(161), (NT(34)));
//G230: __E___E_bf_71_72(161) => null.
	p(NT(161), (nul));
//G231: __E_bf_71(160)       => '0' __E___E_bf_71_72(161).
	p(NT(160), (T(43)+NT(161)));
//G232: bf_f(124)            => __E_bf_71(160).
	p(NT(124), (NT(160)));
//G233: bf(31)               => bf_f(124).
	p(NT(31), (NT(124)));
//G234: __E___E_bf_73_74(164) => _(13) '&' _(13).
	p(NT(164), (NT(13)+T(33)+NT(13)));
//G235: __E___E_bf_73_74(164) => __(35).
	p(NT(164), (NT(35)));
//G236: __E_bf_73(163)       => bf(31) __E___E_bf_73_74(164) bf(31).
	p(NT(163), (NT(31)+NT(164)+NT(31)));
//G237: bf_and(162)          => __E_bf_73(163).
	p(NT(162), (NT(163)));
//G238: bf(31)               => bf_and(162).
	p(NT(31), (NT(162)));
//G239: __E___E_bf_75_76(167) => bf_parenthesis(109).
	p(NT(167), (NT(109)));
//G240: __E___E_bf_75_76(167) => ba_constant(111).
	p(NT(167), (NT(111)));
//G241: __E___E_bf_75_76(167) => variable(112).
	p(NT(167), (NT(112)));
//G242: __E___E_bf_75_76(167) => bf_ref(117).
	p(NT(167), (NT(117)));
//G243: __E___E_bf_75_76(167) => bf_t(123).
	p(NT(167), (NT(123)));
//G244: __E___E_bf_75_76(167) => bf_f(124).
	p(NT(167), (NT(124)));
//G245: __E___E_bf_75_76(167) => bf_neg(125).
	p(NT(167), (NT(125)));
//G246: __E___E_bf_75_76(167) => capture(30).
	p(NT(167), (NT(30)));
//G247: bf_neg_oprnd(166)    => __E___E_bf_75_76(167).
	p(NT(166), (NT(167)));
//G248: __E_bf_75(165)       => bf_neg_oprnd(166) _(13) '\''.
	p(NT(165), (NT(166)+NT(13)+T(44)));
//G249: bf_neg(125)          => __E_bf_75(165).
	p(NT(125), (NT(165)));
//G250: bf(31)               => bf_neg(125).
	p(NT(31), (NT(125)));
//G251: __E___E_bf_77_78(171) => bf_parenthesis(109).
	p(NT(171), (NT(109)));
//G252: __E___E_bf_77_78(171) => variable(112).
	p(NT(171), (NT(112)));
//G253: __E___E_bf_77_78(171) => bf_ref(117).
	p(NT(171), (NT(117)));
//G254: __E___E_bf_77_78(171) => bf_neg(125).
	p(NT(171), (NT(125)));
//G255: __E___E_bf_77_78(171) => bf_and_nosep(168).
	p(NT(171), (NT(168)));
//G256: __E___E_bf_77_78(171) => capture(30).
	p(NT(171), (NT(30)));
//G257: bf_and_nosep_1st_oprnd(170) => __E___E_bf_77_78(171).
	p(NT(170), (NT(171)));
//G258: __E___E_bf_77_79(173) => bf_parenthesis(109).
	p(NT(173), (NT(109)));
//G259: __E___E_bf_77_79(173) => ba_constant(111).
	p(NT(173), (NT(111)));
//G260: __E___E_bf_77_79(173) => variable(112).
	p(NT(173), (NT(112)));
//G261: __E___E_bf_77_79(173) => bf_ref(117).
	p(NT(173), (NT(117)));
//G262: __E___E_bf_77_79(173) => bf_neg(125).
	p(NT(173), (NT(125)));
//G263: __E___E_bf_77_79(173) => capture(30).
	p(NT(173), (NT(30)));
//G264: bf_and_nosep_2nd_oprnd(172) => __E___E_bf_77_79(173).
	p(NT(172), (NT(173)));
//G265: __E_bf_77(169)       => bf_and_nosep_1st_oprnd(170) bf_and_nosep_2nd_oprnd(172).
	p(NT(169), (NT(170)+NT(172)));
//G266: bf_and_nosep(168)    => __E_bf_77(169).
	p(NT(168), (NT(169)));
//G267: bf(31)               => bf_and_nosep(168).
	p(NT(31), (NT(168)));
//G268: bf(31)               => capture(30).
	p(NT(31), (NT(30)));
//G269: __E_constraint_80(175) => '[' ctnvar(176) _(13) '!' '=' _(13) num(120) ']'.
	p(NT(175), (T(26)+NT(176)+NT(13)+T(34)+T(3)+NT(13)+NT(120)+T(27)));
//G270: __E_constraint_80(175) => '[' num(120) _(13) '!' '=' _(13) ctnvar(176) ']'.
	p(NT(175), (T(26)+NT(120)+NT(13)+T(34)+T(3)+NT(13)+NT(176)+T(27)));
//G271: ctn_neq(174)         => __E_constraint_80(175).
	p(NT(174), (NT(175)));
//G272: constraint(70)       => ctn_neq(174).
	p(NT(70), (NT(174)));
//G273: __E_constraint_81(178) => '[' ctnvar(176) _(13) '=' _(13) num(120) ']'.
	p(NT(178), (T(26)+NT(176)+NT(13)+T(3)+NT(13)+NT(120)+T(27)));
//G274: __E_constraint_81(178) => '[' num(120) _(13) '=' _(13) ctnvar(176) ']'.
	p(NT(178), (T(26)+NT(120)+NT(13)+T(3)+NT(13)+NT(176)+T(27)));
//G275: ctn_eq(177)          => __E_constraint_81(178).
	p(NT(177), (NT(178)));
//G276: constraint(70)       => ctn_eq(177).
	p(NT(70), (NT(177)));
//G277: __E_constraint_82(180) => '[' ctnvar(176) _(13) '>' '=' _(13) num(120) ']'.
	p(NT(180), (T(26)+NT(176)+NT(13)+T(23)+T(3)+NT(13)+NT(120)+T(27)));
//G278: __E_constraint_82(180) => '[' num(120) _(13) '>' '=' _(13) ctnvar(176) ']'.
	p(NT(180), (T(26)+NT(120)+NT(13)+T(23)+T(3)+NT(13)+NT(176)+T(27)));
//G279: ctn_gteq(179)        => __E_constraint_82(180).
	p(NT(179), (NT(180)));
//G280: constraint(70)       => ctn_gteq(179).
	p(NT(70), (NT(179)));
//G281: __E_constraint_83(182) => '[' ctnvar(176) _(13) '>' _(13) num(120) ']'.
	p(NT(182), (T(26)+NT(176)+NT(13)+T(23)+NT(13)+NT(120)+T(27)));
//G282: __E_constraint_83(182) => '[' num(120) _(13) '>' _(13) ctnvar(176) ']'.
	p(NT(182), (T(26)+NT(120)+NT(13)+T(23)+NT(13)+NT(176)+T(27)));
//G283: ctn_gt(181)          => __E_constraint_83(182).
	p(NT(181), (NT(182)));
//G284: constraint(70)       => ctn_gt(181).
	p(NT(70), (NT(181)));
//G285: __E_constraint_84(184) => '[' ctnvar(176) _(13) '<' '=' _(13) num(120) ']'.
	p(NT(184), (T(26)+NT(176)+NT(13)+T(22)+T(3)+NT(13)+NT(120)+T(27)));
//G286: __E_constraint_84(184) => '[' num(120) _(13) '<' '=' _(13) ctnvar(176) ']'.
	p(NT(184), (T(26)+NT(120)+NT(13)+T(22)+T(3)+NT(13)+NT(176)+T(27)));
//G287: ctn_lteq(183)        => __E_constraint_84(184).
	p(NT(183), (NT(184)));
//G288: constraint(70)       => ctn_lteq(183).
	p(NT(70), (NT(183)));
//G289: __E_constraint_85(186) => '[' ctnvar(176) _(13) '<' _(13) num(120) ']'.
	p(NT(186), (T(26)+NT(176)+NT(13)+T(22)+NT(13)+NT(120)+T(27)));
//G290: __E_constraint_85(186) => '[' num(120) _(13) '<' _(13) ctnvar(176) ']'.
	p(NT(186), (T(26)+NT(120)+NT(13)+T(22)+NT(13)+NT(176)+T(27)));
//G291: ctn_lt(185)          => __E_constraint_85(186).
	p(NT(185), (NT(186)));
//G292: constraint(70)       => ctn_lt(185).
	p(NT(70), (NT(185)));
//G293: __E_ba_constant_86(187) => capture(30).
	p(NT(187), (NT(30)));
//G294: __E_ba_constant_86(187) => source(188).
	p(NT(187), (NT(188)));
//G295: __E_ba_constant_87(189) => typed(34).
	p(NT(189), (NT(34)));
//G296: __E_ba_constant_87(189) => null.
	p(NT(189), (nul));
//G297: ba_constant(111)     => '{' _(13) __E_ba_constant_86(187) _(13) '}' __E_ba_constant_87(189).
	p(NT(111), (T(45)+NT(13)+NT(187)+NT(13)+T(46)+NT(189)));
//G298: __E___E_source_88_89(192) => src_c(190).
	p(NT(192), (NT(190)));
//G299: __E___E_source_88_89(192) => space(2).
	p(NT(192), (NT(2)));
//G300: __E___E_source_88_90(193) => null.
	p(NT(193), (nul));
//G301: __E___E_source_88_90(193) => __E___E_source_88_89(192) __E___E_source_88_90(193).
	p(NT(193), (NT(192)+NT(193)));
//G302: __E_source_88(191)   => __E___E_source_88_90(193) src_c(190).
	p(NT(191), (NT(193)+NT(190)));
//G303: __E_source_88(191)   => null.
	p(NT(191), (nul));
//G304: source(188)          => src_c(190) __E_source_88(191).
	p(NT(188), (NT(190)+NT(191)));
//G305: src_c(190)           => alnum(6).
	p(NT(190), (NT(6)));
//G306: __N_8(396)           => '{'.
	p(NT(396), (T(45)));
//G307: __N_9(397)           => '}'.
	p(NT(397), (T(46)));
//G308: src_c(190)           => ~( __N_8(396) ) & ~( __N_9(397) ) & punct(7).	 # conjunctive
	p(NT(190), ~(NT(396)) & ~(NT(397)) & (NT(7)));
//G309: __E_src_c_91(194)    => src_c(190).
	p(NT(194), (NT(190)));
//G310: __E_src_c_91(194)    => space(2).
	p(NT(194), (NT(2)));
//G311: __E_src_c_92(195)    => null.
	p(NT(195), (nul));
//G312: __E_src_c_92(195)    => __E_src_c_91(194) __E_src_c_92(195).
	p(NT(195), (NT(194)+NT(195)));
//G313: src_c(190)           => '{' __E_src_c_92(195) '}'.
	p(NT(190), (T(45)+NT(195)+T(46)));
//G314: __E_variable_93(196) => uconst(197).
	p(NT(196), (NT(197)));
//G315: __E_variable_93(196) => io_var(198).
	p(NT(196), (NT(198)));
//G316: __E_variable_93(196) => var_name(199).
	p(NT(196), (NT(199)));
//G317: __E_variable_94(200) => typed(34).
	p(NT(200), (NT(34)));
//G318: __E_variable_94(200) => null.
	p(NT(200), (nul));
//G319: variable(112)        => __E_variable_93(196) __E_variable_94(200).
	p(NT(112), (NT(196)+NT(200)));
//G320: __N_10(398)          => 'F'.
	p(NT(398), (T(36)));
//G321: __N_11(399)          => 'T'.
	p(NT(399), (T(35)));
//G322: __E_var_name_95(201) => ~( __N_10(398) ) & ~( __N_11(399) ) & alpha(5).	 # conjunctive
	p(NT(201), ~(NT(398)) & ~(NT(399)) & (NT(5)));
//G323: __E_var_name_96(202) => null.
	p(NT(202), (nul));
//G324: __E_var_name_96(202) => digit(3) __E_var_name_96(202).
	p(NT(202), (NT(3)+NT(202)));
//G325: var_name(199)        => __E_var_name_95(201) __E_var_name_96(202).	 # guarded: charvar
	p(NT(199), (NT(201)+NT(202)));
	p.back().guard = "charvar";
//G326: __N_12(400)          => 'F'.
	p(NT(400), (T(36)));
//G327: __N_13(401)          => 'T'.
	p(NT(401), (T(35)));
//G328: __E_var_name_97(203) => ~( __N_12(400) ) & ~( __N_13(401) ) & alpha(5).	 # conjunctive
	p(NT(203), ~(NT(400)) & ~(NT(401)) & (NT(5)));
//G329: __E_var_name_98(204) => alnum(6).
	p(NT(204), (NT(6)));
//G330: __E_var_name_98(204) => '_'.
	p(NT(204), (T(47)));
//G331: __E_var_name_99(205) => null.
	p(NT(205), (nul));
//G332: __E_var_name_99(205) => __E_var_name_98(204) __E_var_name_99(205).
	p(NT(205), (NT(204)+NT(205)));
//G333: var_name(199)        => __E_var_name_97(203) __E_var_name_99(205).	 # guarded: var
	p(NT(199), (NT(203)+NT(205)));
	p.back().guard = "var";
//G334: io_var(198)          => io_var_name(32) '[' offset(206) ']'.
	p(NT(198), (NT(32)+T(26)+NT(206)+T(27)));
//G335: io_var_name(32)      => chars(207).
	p(NT(32), (NT(207)));
//G336: __E___E_uconst_100_101(210) => chars(207) _(13).
	p(NT(210), (NT(207)+NT(13)));
//G337: __E___E_uconst_100_101(210) => null.
	p(NT(210), (nul));
//G338: __E_uconst_100(209)  => __E___E_uconst_100_101(210) ':' _(13) chars(207).
	p(NT(209), (NT(210)+T(2)+NT(13)+NT(207)));
//G339: uconst_name(208)     => __E_uconst_100(209).
	p(NT(208), (NT(209)));
//G340: uconst(197)          => '<' _(13) uconst_name(208) _(13) '>'.
	p(NT(197), (T(22)+NT(13)+NT(208)+NT(13)+T(23)));
//G341: __E_q_vars_102(212)  => _(13) ',' _(13) q_var(211).
	p(NT(212), (NT(13)+T(20)+NT(13)+NT(211)));
//G342: __E_q_vars_103(213)  => null.
	p(NT(213), (nul));
//G343: __E_q_vars_103(213)  => __E_q_vars_102(212) __E_q_vars_103(213).
	p(NT(213), (NT(212)+NT(213)));
//G344: q_vars(66)           => q_var(211) __E_q_vars_103(213).
	p(NT(66), (NT(211)+NT(213)));
//G345: q_var(211)           => capture(30).
	p(NT(211), (NT(30)));
//G346: __N_14(402)          => uconst(197).
	p(NT(402), (NT(197)));
//G347: q_var(211)           => variable(112) & ~( __N_14(402) ).	 # conjunctive
	p(NT(211), (NT(112)) & ~(NT(402)));
//G348: ctnvar(176)          => chars(207).
	p(NT(176), (NT(207)));
//G349: __E_offsets_104(214) => _(13) ',' _(13) offset(206).
	p(NT(214), (NT(13)+T(20)+NT(13)+NT(206)));
//G350: __E_offsets_105(215) => null.
	p(NT(215), (nul));
//G351: __E_offsets_105(215) => __E_offsets_104(214) __E_offsets_105(215).
	p(NT(215), (NT(214)+NT(215)));
//G352: offsets(42)          => '[' _(13) offset(206) __E_offsets_105(215) _(13) ']'.
	p(NT(42), (T(26)+NT(13)+NT(206)+NT(215)+NT(13)+T(27)));
//G353: offset(206)          => integer(216).
	p(NT(206), (NT(216)));
//G354: offset(206)          => capture(30).
	p(NT(206), (NT(30)));
//G355: offset(206)          => shift(217).
	p(NT(206), (NT(217)));
//G356: __N_15(403)          => io_var(198).
	p(NT(403), (NT(198)));
//G357: __E_offset_106(218)  => variable(112) & ~( __N_15(403) ).	 # conjunctive
	p(NT(218), (NT(112)) & ~(NT(403)));
//G358: offset(206)          => __E_offset_106(218).
	p(NT(206), (NT(218)));
//G359: __E_shift_107(219)   => capture(30).
	p(NT(219), (NT(30)));
//G360: __N_16(404)          => io_var(198).
	p(NT(404), (NT(198)));
//G361: __E___E_shift_107_108(220) => variable(112) & ~( __N_16(404) ).	 # conjunctive
	p(NT(220), (NT(112)) & ~(NT(404)));
//G362: __E_shift_107(219)   => __E___E_shift_107_108(220).
	p(NT(219), (NT(220)));
//G363: shift(217)           => __E_shift_107(219) _(13) '-' _(13) num(120).
	p(NT(217), (NT(219)+NT(13)+T(30)+NT(13)+NT(120)));
//G364: __E_chars_109(221)   => alnum(6).
	p(NT(221), (NT(6)));
//G365: __E_chars_109(221)   => '_'.
	p(NT(221), (T(47)));
//G366: __E_chars_110(222)   => null.
	p(NT(222), (nul));
//G367: __E_chars_110(222)   => __E_chars_109(221) __E_chars_110(222).
	p(NT(222), (NT(221)+NT(222)));
//G368: chars(207)           => alpha(5) __E_chars_110(222).
	p(NT(207), (NT(5)+NT(222)));
//G369: __E_digits_111(224)  => digit(3).
	p(NT(224), (NT(3)));
//G370: __E_digits_111(224)  => digit(3) __E_digits_111(224).
	p(NT(224), (NT(3)+NT(224)));
//G371: digits(223)          => __E_digits_111(224).
	p(NT(223), (NT(224)));
//G372: num(120)             => digits(223).
	p(NT(120), (NT(223)));
//G373: __E_integer_112(225) => '-'.
	p(NT(225), (T(30)));
//G374: __E_integer_112(225) => null.
	p(NT(225), (nul));
//G375: integer(216)         => __E_integer_112(225) _(13) digits(223).
	p(NT(216), (NT(225)+NT(13)+NT(223)));
//G376: sym(40)              => chars(207).
	p(NT(40), (NT(207)));
//G377: capture(30)          => '$' chars(207).
	p(NT(30), (T(48)+NT(207)));
//G378: typed(34)            => _(13) ':' _(13) type(226) _(13) '[' _(13) subtype(227) _(13) ']'.
	p(NT(34), (NT(13)+T(2)+NT(13)+NT(226)+NT(13)+T(26)+NT(13)+NT(227)+NT(13)+T(27)));
//G379: __N_17(405)          => 'b' 'v'.
	p(NT(405), (T(17)+T(37)));
//G380: __E_typed_113(228)   => ~( __N_17(405) ) & type(226).	 # conjunctive
	p(NT(228), ~(NT(405)) & (NT(226)));
//G381: typed(34)            => _(13) ':' _(13) __E_typed_113(228).
	p(NT(34), (NT(13)+T(2)+NT(13)+NT(228)));
//G382: type(226)            => chars(207).
	p(NT(226), (NT(207)));
//G383: subtype(227)         => num(120).
	p(NT(227), (NT(120)));
//G384: __E_comment_114(230) => printable(8).
	p(NT(230), (NT(8)));
//G385: __E_comment_114(230) => '\t'.
	p(NT(230), (T(49)));
//G386: __E_comment_115(231) => null.
	p(NT(231), (nul));
//G387: __E_comment_115(231) => __E_comment_114(230) __E_comment_115(231).
	p(NT(231), (NT(230)+NT(231)));
//G388: __E_comment_116(232) => '\n'.
	p(NT(232), (T(50)));
//G389: __E_comment_116(232) => '\r'.
	p(NT(232), (T(51)));
//G390: __E_comment_116(232) => eof(1).
	p(NT(232), (NT(1)));
//G391: comment(229)         => '#' __E_comment_115(231) __E_comment_116(232).
	p(NT(229), (T(52)+NT(231)+NT(232)));
//G392: __(35)               => space(2).
	p(NT(35), (NT(2)));
//G393: __(35)               => comment(229).
	p(NT(35), (NT(229)));
//G394: __(35)               => __(35) space(2).
	p(NT(35), (NT(35)+NT(2)));
//G395: __(35)               => __(35) comment(229).
	p(NT(35), (NT(35)+NT(229)));
//G396: _(13)                => __(35).
	p(NT(13), (NT(35)));
//G397: _(13)                => null.
	p(NT(13), (nul));
//G398: cli(233)             => _(13).
	p(NT(233), (NT(13)));
//G399: __E_cli_117(235)     => '.' _(13) cli_command(234) _(13).
	p(NT(235), (T(1)+NT(13)+NT(234)+NT(13)));
//G400: __E_cli_118(236)     => null.
	p(NT(236), (nul));
//G401: __E_cli_118(236)     => __E_cli_117(235) __E_cli_118(236).
	p(NT(236), (NT(235)+NT(236)));
//G402: __E_cli_119(237)     => '.' _(13).
	p(NT(237), (T(1)+NT(13)));
//G403: __E_cli_119(237)     => null.
	p(NT(237), (nul));
//G404: cli(233)             => _(13) cli_command(234) _(13) __E_cli_118(236) __E_cli_119(237).
	p(NT(233), (NT(13)+NT(234)+NT(13)+NT(236)+NT(237)));
//G405: __E_cli_command_120(240) => 'q'.
	p(NT(240), (T(53)));
//G406: __E_cli_command_120(240) => 'q' 'u' 'i' 't'.
	p(NT(240), (T(53)+T(7)+T(4)+T(8)));
//G407: quit_sym(239)        => __E_cli_command_120(240).
	p(NT(239), (NT(240)));
//G408: quit_cmd(238)        => quit_sym(239).
	p(NT(238), (NT(239)));
//G409: cli_command(234)     => quit_cmd(238).
	p(NT(234), (NT(238)));
//G410: __E_cli_command_121(243) => 'v'.
	p(NT(243), (T(37)));
//G411: __E_cli_command_121(243) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(243), (T(37)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G412: version_sym(242)     => __E_cli_command_121(243).
	p(NT(242), (NT(243)));
//G413: version_cmd(241)     => version_sym(242).
	p(NT(241), (NT(242)));
//G414: cli_command(234)     => version_cmd(241).
	p(NT(234), (NT(241)));
//G415: __E_cli_command_122(246) => 'c'.
	p(NT(246), (T(14)));
//G416: __E_cli_command_122(246) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(246), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G417: clear_sym(245)       => __E_cli_command_122(246).
	p(NT(245), (NT(246)));
//G418: clear_cmd(244)       => clear_sym(245).
	p(NT(244), (NT(245)));
//G419: cli_command(234)     => clear_cmd(244).
	p(NT(234), (NT(244)));
//G420: __E___E_cli_command_123_124(250) => 'h'.
	p(NT(250), (T(54)));
//G421: __E___E_cli_command_123_124(250) => 'h' 'e' 'l' 'p'.
	p(NT(250), (T(54)+T(11)+T(10)+T(55)));
//G422: help_sym(249)        => __E___E_cli_command_123_124(250).
	p(NT(249), (NT(250)));
//G423: __E___E_cli_command_123_125(251) => __(35) help_arg(252).
	p(NT(251), (NT(35)+NT(252)));
//G424: __E___E_cli_command_123_125(251) => null.
	p(NT(251), (nul));
//G425: __E_cli_command_123(248) => help_sym(249) __E___E_cli_command_123_125(251).
	p(NT(248), (NT(249)+NT(251)));
//G426: help_cmd(247)        => __E_cli_command_123(248).
	p(NT(247), (NT(248)));
//G427: cli_command(234)     => help_cmd(247).
	p(NT(234), (NT(247)));
//G428: file_sym(255)        => 'f' 'i' 'l' 'e'.
	p(NT(255), (T(9)+T(4)+T(10)+T(11)));
//G429: __E_cli_command_126(254) => file_sym(255) __(35) q_string(256).
	p(NT(254), (NT(255)+NT(35)+NT(256)));
//G430: file_cmd(253)        => __E_cli_command_126(254).
	p(NT(253), (NT(254)));
//G431: cli_command(234)     => file_cmd(253).
	p(NT(234), (NT(253)));
//G432: valid_sym(259)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(259), (T(37)+T(16)+T(10)+T(4)+T(56)));
//G433: __E_cli_command_127(258) => valid_sym(259) __(35) spec_arg(260).
	p(NT(258), (NT(259)+NT(35)+NT(260)));
//G434: valid_cmd(257)       => __E_cli_command_127(258).
	p(NT(257), (NT(258)));
//G435: cli_command(234)     => valid_cmd(257).
	p(NT(234), (NT(257)));
//G436: sat_sym(263)         => 's' 'a' 't'.
	p(NT(263), (T(15)+T(16)+T(8)));
//G437: __E_cli_command_128(262) => sat_sym(263) __(35) spec_arg(260).
	p(NT(262), (NT(263)+NT(35)+NT(260)));
//G438: sat_cmd(261)         => __E_cli_command_128(262).
	p(NT(261), (NT(262)));
//G439: cli_command(234)     => sat_cmd(261).
	p(NT(234), (NT(261)));
//G440: unsat_sym(266)       => 'u' 'n' 's' 'a' 't'.
	p(NT(266), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G441: __E_cli_command_129(265) => unsat_sym(266) __(35) spec_arg(260).
	p(NT(265), (NT(266)+NT(35)+NT(260)));
//G442: unsat_cmd(264)       => __E_cli_command_129(265).
	p(NT(264), (NT(265)));
//G443: cli_command(234)     => unsat_cmd(264).
	p(NT(234), (NT(264)));
//G444: solve_sym(269)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(269), (T(15)+T(6)+T(10)+T(37)+T(11)));
//G445: __E___E_cli_command_130_131(270) => solve_options(271).
	p(NT(270), (NT(271)));
//G446: __E___E_cli_command_130_131(270) => null.
	p(NT(270), (nul));
//G447: __E_cli_command_130(268) => solve_sym(269) __E___E_cli_command_130_131(270) __(35) wff_cmd_arg(272).
	p(NT(268), (NT(269)+NT(270)+NT(35)+NT(272)));
//G448: solve_cmd(267)       => __E_cli_command_130(268).
	p(NT(267), (NT(268)));
//G449: cli_command(234)     => solve_cmd(267).
	p(NT(234), (NT(267)));
//G450: lgrs_sym(275)        => 'l' 'g' 'r' 's'.
	p(NT(275), (T(10)+T(57)+T(19)+T(15)));
//G451: __E_cli_command_132(274) => lgrs_sym(275) __(35) wff_cmd_arg(272).
	p(NT(274), (NT(275)+NT(35)+NT(272)));
//G452: lgrs_cmd(273)        => __E_cli_command_132(274).
	p(NT(273), (NT(274)));
//G453: cli_command(234)     => lgrs_cmd(273).
	p(NT(234), (NT(273)));
//G454: __E___E_cli_command_133_134(279) => 'r'.
	p(NT(279), (T(19)));
//G455: __E___E_cli_command_133_134(279) => 'r' 'u' 'n'.
	p(NT(279), (T(19)+T(7)+T(5)));
//G456: run_sym(278)         => __E___E_cli_command_133_134(279).
	p(NT(278), (NT(279)));
//G457: __E_cli_command_133(277) => run_sym(278) __(35) spec_arg(260).
	p(NT(277), (NT(278)+NT(35)+NT(260)));
//G458: run_cmd(276)         => __E_cli_command_133(277).
	p(NT(276), (NT(277)));
//G459: cli_command(234)     => run_cmd(276).
	p(NT(234), (NT(276)));
//G460: __E___E_cli_command_135_136(283) => 'n'.
	p(NT(283), (T(5)));
//G461: __E___E_cli_command_135_136(283) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(283), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(58)+T(11)));
//G462: normalize_sym(282)   => __E___E_cli_command_135_136(283).
	p(NT(282), (NT(283)));
//G463: __E_cli_command_135(281) => normalize_sym(282) __(35) spec_arg(260).
	p(NT(281), (NT(282)+NT(35)+NT(260)));
//G464: normalize_cmd(280)   => __E_cli_command_135(281).
	p(NT(280), (NT(281)));
//G465: cli_command(234)     => normalize_cmd(280).
	p(NT(234), (NT(280)));
//G466: __E___E_cli_command_137_138(287) => 's'.
	p(NT(287), (T(15)));
//G467: __E___E_cli_command_137_138(287) => 's' 'u' 'b' 's' 't'.
	p(NT(287), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G468: __E___E_cli_command_137_138(287) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(287), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G469: subst_sym(286)       => __E___E_cli_command_137_138(287).
	p(NT(286), (NT(287)));
//G470: __E_cli_command_137(285) => subst_sym(286) __(35) nf_cmd_arg(288) _(13) '[' _(13) nf_cmd_arg(288) _(13) '/' _(13) nf_cmd_arg(288) _(13) ']'.
	p(NT(285), (NT(286)+NT(35)+NT(288)+NT(13)+T(26)+NT(13)+NT(288)+NT(13)+T(41)+NT(13)+NT(288)+NT(13)+T(27)));
//G471: subst_cmd(284)       => __E_cli_command_137(285).
	p(NT(284), (NT(285)));
//G472: cli_command(234)     => subst_cmd(284).
	p(NT(234), (NT(284)));
//G473: __E___E_cli_command_139_140(292) => 'i'.
	p(NT(292), (T(4)));
//G474: __E___E_cli_command_139_140(292) => 'i' 'n' 's' 't'.
	p(NT(292), (T(4)+T(5)+T(15)+T(8)));
//G475: __E___E_cli_command_139_140(292) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(292), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G476: inst_sym(291)        => __E___E_cli_command_139_140(292).
	p(NT(291), (NT(292)));
//G477: __E_cli_command_139(290) => inst_sym(291) __(35) inst_args(293).
	p(NT(290), (NT(291)+NT(35)+NT(293)));
//G478: inst_cmd(289)        => __E_cli_command_139(290).
	p(NT(289), (NT(290)));
//G479: cli_command(234)     => inst_cmd(289).
	p(NT(234), (NT(289)));
//G480: dnf_sym(296)         => 'd' 'n' 'f'.
	p(NT(296), (T(56)+T(5)+T(9)));
//G481: __E_cli_command_141(295) => dnf_sym(296) __(35) nf_cmd_arg(288).
	p(NT(295), (NT(296)+NT(35)+NT(288)));
//G482: dnf_cmd(294)         => __E_cli_command_141(295).
	p(NT(294), (NT(295)));
//G483: cli_command(234)     => dnf_cmd(294).
	p(NT(234), (NT(294)));
//G484: cnf_sym(299)         => 'c' 'n' 'f'.
	p(NT(299), (T(14)+T(5)+T(9)));
//G485: __E_cli_command_142(298) => cnf_sym(299) __(35) nf_cmd_arg(288).
	p(NT(298), (NT(299)+NT(35)+NT(288)));
//G486: cnf_cmd(297)         => __E_cli_command_142(298).
	p(NT(297), (NT(298)));
//G487: cli_command(234)     => cnf_cmd(297).
	p(NT(234), (NT(297)));
//G488: anf_sym(302)         => 'a' 'n' 'f'.
	p(NT(302), (T(16)+T(5)+T(9)));
//G489: __E_cli_command_143(301) => anf_sym(302) __(35) nf_cmd_arg(288).
	p(NT(301), (NT(302)+NT(35)+NT(288)));
//G490: anf_cmd(300)         => __E_cli_command_143(301).
	p(NT(300), (NT(301)));
//G491: cli_command(234)     => anf_cmd(300).
	p(NT(234), (NT(300)));
//G492: nnf_sym(305)         => 'n' 'n' 'f'.
	p(NT(305), (T(5)+T(5)+T(9)));
//G493: __E_cli_command_144(304) => nnf_sym(305) __(35) nf_cmd_arg(288).
	p(NT(304), (NT(305)+NT(35)+NT(288)));
//G494: nnf_cmd(303)         => __E_cli_command_144(304).
	p(NT(303), (NT(304)));
//G495: cli_command(234)     => nnf_cmd(303).
	p(NT(234), (NT(303)));
//G496: pnf_sym(308)         => 'p' 'n' 'f'.
	p(NT(308), (T(55)+T(5)+T(9)));
//G497: __E_cli_command_145(307) => pnf_sym(308) __(35) nf_cmd_arg(288).
	p(NT(307), (NT(308)+NT(35)+NT(288)));
//G498: pnf_cmd(306)         => __E_cli_command_145(307).
	p(NT(306), (NT(307)));
//G499: cli_command(234)     => pnf_cmd(306).
	p(NT(234), (NT(306)));
//G500: mnf_sym(311)         => 'm' 'n' 'f'.
	p(NT(311), (T(21)+T(5)+T(9)));
//G501: __E_cli_command_146(310) => mnf_sym(311) __(35) nf_cmd_arg(288).
	p(NT(310), (NT(311)+NT(35)+NT(288)));
//G502: mnf_cmd(309)         => __E_cli_command_146(310).
	p(NT(309), (NT(310)));
//G503: cli_command(234)     => mnf_cmd(309).
	p(NT(234), (NT(309)));
//G504: onf_sym(314)         => 'o' 'n' 'f'.
	p(NT(314), (T(6)+T(5)+T(9)));
//G505: __E_cli_command_147(313) => onf_sym(314) __(35) variable(112) __(35) onf_cmd_arg(315).
	p(NT(313), (NT(314)+NT(35)+NT(112)+NT(35)+NT(315)));
//G506: onf_cmd(312)         => __E_cli_command_147(313).
	p(NT(312), (NT(313)));
//G507: cli_command(234)     => onf_cmd(312).
	p(NT(234), (NT(312)));
//G508: qelim_sym(318)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(318), (T(53)+T(11)+T(10)+T(4)+T(21)));
//G509: __E_cli_command_148(317) => qelim_sym(318) __(35) wff_cmd_arg(272).
	p(NT(317), (NT(318)+NT(35)+NT(272)));
//G510: qelim_cmd(316)       => __E_cli_command_148(317).
	p(NT(316), (NT(317)));
//G511: cli_command(234)     => qelim_cmd(316).
	p(NT(234), (NT(316)));
//G512: get_sym(321)         => 'g' 'e' 't'.
	p(NT(321), (T(57)+T(11)+T(8)));
//G513: __E___E_cli_command_149_150(322) => __(35) option_name(323).
	p(NT(322), (NT(35)+NT(323)));
//G514: __E___E_cli_command_149_150(322) => null.
	p(NT(322), (nul));
//G515: __E_cli_command_149(320) => get_sym(321) __E___E_cli_command_149_150(322).
	p(NT(320), (NT(321)+NT(322)));
//G516: get_cmd(319)         => __E_cli_command_149(320).
	p(NT(319), (NT(320)));
//G517: cli_command(234)     => get_cmd(319).
	p(NT(234), (NT(319)));
//G518: set_sym(326)         => 's' 'e' 't'.
	p(NT(326), (T(15)+T(11)+T(8)));
//G519: __E___E_cli_command_151_152(327) => __(35).
	p(NT(327), (NT(35)));
//G520: __E___E_cli_command_151_152(327) => _(13) '=' _(13).
	p(NT(327), (NT(13)+T(3)+NT(13)));
//G521: __E_cli_command_151(325) => set_sym(326) __(35) option_name(323) __E___E_cli_command_151_152(327) option_value(328).
	p(NT(325), (NT(326)+NT(35)+NT(323)+NT(327)+NT(328)));
//G522: set_cmd(324)         => __E_cli_command_151(325).
	p(NT(324), (NT(325)));
//G523: cli_command(234)     => set_cmd(324).
	p(NT(234), (NT(324)));
//G524: enable_sym(331)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(331), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G525: __E_cli_command_153(330) => enable_sym(331) __(35) option_name(323).
	p(NT(330), (NT(331)+NT(35)+NT(323)));
//G526: enable_cmd(329)      => __E_cli_command_153(330).
	p(NT(329), (NT(330)));
//G527: cli_command(234)     => enable_cmd(329).
	p(NT(234), (NT(329)));
//G528: disable_sym(334)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(334), (T(56)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G529: __E_cli_command_154(333) => disable_sym(334) __(35) option_name(323).
	p(NT(333), (NT(334)+NT(35)+NT(323)));
//G530: disable_cmd(332)     => __E_cli_command_154(333).
	p(NT(332), (NT(333)));
//G531: cli_command(234)     => disable_cmd(332).
	p(NT(234), (NT(332)));
//G532: toggle_sym(337)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(337), (T(8)+T(6)+T(57)+T(57)+T(10)+T(11)));
//G533: __E_cli_command_155(336) => toggle_sym(337) __(35) option_name(323).
	p(NT(336), (NT(337)+NT(35)+NT(323)));
//G534: toggle_cmd(335)      => __E_cli_command_155(336).
	p(NT(335), (NT(336)));
//G535: cli_command(234)     => toggle_cmd(335).
	p(NT(234), (NT(335)));
//G536: __E___E_cli_command_156_157(341) => 'd' 'e' 'f' 's'.
	p(NT(341), (T(56)+T(11)+T(9)+T(15)));
//G537: __E___E_cli_command_156_157(341) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(341), (T(56)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G538: def_sym(340)         => __E___E_cli_command_156_157(341).
	p(NT(340), (NT(341)));
//G539: __E_cli_command_156(339) => def_sym(340).
	p(NT(339), (NT(340)));
//G540: def_list_cmd(338)    => __E_cli_command_156(339).
	p(NT(338), (NT(339)));
//G541: cli_command(234)     => def_list_cmd(338).
	p(NT(234), (NT(338)));
//G542: __E_cli_command_158(343) => def_sym(340) __(35) num(120).
	p(NT(343), (NT(340)+NT(35)+NT(120)));
//G543: def_print_cmd(342)   => __E_cli_command_158(343).
	p(NT(342), (NT(343)));
//G544: cli_command(234)     => def_print_cmd(342).
	p(NT(234), (NT(342)));
//G545: def_rr_cmd(344)      => rec_relation(19).
	p(NT(344), (NT(19)));
//G546: cli_command(234)     => def_rr_cmd(344).
	p(NT(234), (NT(344)));
//G547: def_input_cmd(345)   => input_def(20).
	p(NT(345), (NT(20)));
//G548: cli_command(234)     => def_input_cmd(345).
	p(NT(234), (NT(345)));
//G549: def_output_cmd(346)  => output_def(21).
	p(NT(346), (NT(21)));
//G550: cli_command(234)     => def_output_cmd(346).
	p(NT(234), (NT(346)));
//G551: __E___E_cli_command_159_160(350) => 'h' 'i' 's' 't'.
	p(NT(350), (T(54)+T(4)+T(15)+T(8)));
//G552: __E___E_cli_command_159_160(350) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(350), (T(54)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G553: history_sym(349)     => __E___E_cli_command_159_160(350).
	p(NT(349), (NT(350)));
//G554: __E_cli_command_159(348) => history_sym(349).
	p(NT(348), (NT(349)));
//G555: history_list_cmd(347) => __E_cli_command_159(348).
	p(NT(347), (NT(348)));
//G556: cli_command(234)     => history_list_cmd(347).
	p(NT(234), (NT(347)));
//G557: __E_cli_command_161(352) => history_sym(349) __(35) history(353).
	p(NT(352), (NT(349)+NT(35)+NT(353)));
//G558: history_print_cmd(351) => __E_cli_command_161(352).
	p(NT(351), (NT(352)));
//G559: cli_command(234)     => history_print_cmd(351).
	p(NT(234), (NT(351)));
//G560: __E_cli_command_162(355) => ref(28).
	p(NT(355), (NT(28)));
//G561: __E_cli_command_162(355) => wff(15).
	p(NT(355), (NT(15)));
//G562: __E_cli_command_162(355) => bf(31).
	p(NT(355), (NT(31)));
//G563: history_store_cmd(354) => __E_cli_command_162(355).
	p(NT(354), (NT(355)));
//G564: cli_command(234)     => history_store_cmd(354).
	p(NT(234), (NT(354)));
//G565: __E_solve_options_163(356) => __(35) solve_option(357).
	p(NT(356), (NT(35)+NT(357)));
//G566: __E_solve_options_164(358) => null.
	p(NT(358), (nul));
//G567: __E_solve_options_164(358) => __E_solve_options_163(356) __E_solve_options_164(358).
	p(NT(358), (NT(356)+NT(358)));
//G568: solve_options(271)   => __E_solve_options_164(358).
	p(NT(271), (NT(358)));
//G569: __E_solve_option_165(359) => solver_mode(360).
	p(NT(359), (NT(360)));
//G570: __E_solve_option_165(359) => type(226).
	p(NT(359), (NT(226)));
//G571: solve_option(357)    => '-' '-' __E_solve_option_165(359).
	p(NT(357), (T(30)+T(30)+NT(359)));
//G572: __E_solver_mode_166(362) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(362), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G573: __E_solver_mode_166(362) => 'm' 'i' 'n'.
	p(NT(362), (T(21)+T(4)+T(5)));
//G574: solver_mode_minimum(361) => __E_solver_mode_166(362).
	p(NT(361), (NT(362)));
//G575: solver_mode(360)     => solver_mode_minimum(361).
	p(NT(360), (NT(361)));
//G576: __E_solver_mode_167(364) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(364), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G577: __E_solver_mode_167(364) => 'm' 'a' 'x'.
	p(NT(364), (T(21)+T(16)+T(29)));
//G578: solver_mode_maximum(363) => __E_solver_mode_167(364).
	p(NT(363), (NT(364)));
//G579: solver_mode(360)     => solver_mode_maximum(363).
	p(NT(360), (NT(363)));
//G580: q_file_name(38)      => '"' file_name(365) '"'.
	p(NT(38), (T(59)+NT(365)+T(59)));
//G581: __E_file_name_168(366) => printable(8).
	p(NT(366), (NT(8)));
//G582: __E_file_name_168(366) => printable(8) __E_file_name_168(366).
	p(NT(366), (NT(8)+NT(366)));
//G583: file_name(365)       => __E_file_name_168(366).
	p(NT(365), (NT(366)));
//G584: __E_option_name_169(367) => alnum(6).
	p(NT(367), (NT(6)));
//G585: __E_option_name_169(367) => alnum(6) __E_option_name_169(367).
	p(NT(367), (NT(6)+NT(367)));
//G586: option_name(323)     => __E_option_name_169(367).
	p(NT(323), (NT(367)));
//G587: __E_option_value_170(368) => alnum(6).
	p(NT(368), (NT(6)));
//G588: __E_option_value_170(368) => alnum(6) __E_option_value_170(368).
	p(NT(368), (NT(6)+NT(368)));
//G589: option_value(328)    => __E_option_value_170(368).
	p(NT(328), (NT(368)));
//G590: bf_cmd_arg(369)      => history(353).
	p(NT(369), (NT(353)));
//G591: bf_cmd_arg(369)      => bf(31).
	p(NT(369), (NT(31)));
//G592: wff_cmd_arg(272)     => history(353).
	p(NT(272), (NT(353)));
//G593: wff_cmd_arg(272)     => wff(15).
	p(NT(272), (NT(15)));
//G594: nf_cmd_arg(288)      => history(353).
	p(NT(288), (NT(353)));
//G595: nf_cmd_arg(288)      => ref(28).
	p(NT(288), (NT(28)));
//G596: nf_cmd_arg(288)      => wff(15).
	p(NT(288), (NT(15)));
//G597: nf_cmd_arg(288)      => bf(31).
	p(NT(288), (NT(31)));
//G598: onf_cmd_arg(315)     => history(353).
	p(NT(315), (NT(353)));
//G599: onf_cmd_arg(315)     => wff(15).
	p(NT(315), (NT(15)));
//G600: spec_arg(260)        => history(353).
	p(NT(260), (NT(353)));
//G601: spec_arg(260)        => spec(10).
	p(NT(260), (NT(10)));
//G602: spec_arg(260)        => ref(28).
	p(NT(260), (NT(28)));
//G603: spec_arg(260)        => wff(15).
	p(NT(260), (NT(15)));
//G604: spec_arg(260)        => bf(31).
	p(NT(260), (NT(31)));
//G605: inst_args(293)       => wff_cmd_arg(272) _(13) '[' _(13) bf(31) _(13) '/' _(13) bf_cmd_arg(369) _(13) ']'.
	p(NT(293), (NT(272)+NT(13)+T(26)+NT(13)+NT(31)+NT(13)+T(41)+NT(13)+NT(369)+NT(13)+T(27)));
//G606: inst_args(293)       => bf_cmd_arg(369) _(13) '[' _(13) bf(31) _(13) '/' _(13) bf_cmd_arg(369) _(13) ']'.
	p(NT(293), (NT(369)+NT(13)+T(26)+NT(13)+NT(31)+NT(13)+T(41)+NT(13)+NT(369)+NT(13)+T(27)));
//G607: help_arg(252)        => help_sym(249).
	p(NT(252), (NT(249)));
//G608: help_arg(252)        => version_sym(242).
	p(NT(252), (NT(242)));
//G609: help_arg(252)        => quit_sym(239).
	p(NT(252), (NT(239)));
//G610: help_arg(252)        => clear_sym(245).
	p(NT(252), (NT(245)));
//G611: help_arg(252)        => get_sym(321).
	p(NT(252), (NT(321)));
//G612: help_arg(252)        => set_sym(326).
	p(NT(252), (NT(326)));
//G613: help_arg(252)        => enable_sym(331).
	p(NT(252), (NT(331)));
//G614: help_arg(252)        => disable_sym(334).
	p(NT(252), (NT(334)));
//G615: help_arg(252)        => toggle_sym(337).
	p(NT(252), (NT(337)));
//G616: help_arg(252)        => file_sym(255).
	p(NT(252), (NT(255)));
//G617: help_arg(252)        => history_sym(349).
	p(NT(252), (NT(349)));
//G618: help_arg(252)        => abs_history_sym(370).
	p(NT(252), (NT(370)));
//G619: help_arg(252)        => rel_history_sym(371).
	p(NT(252), (NT(371)));
//G620: help_arg(252)        => selection_sym(372).
	p(NT(252), (NT(372)));
//G621: help_arg(252)        => def_sym(340).
	p(NT(252), (NT(340)));
//G622: help_arg(252)        => inst_sym(291).
	p(NT(252), (NT(291)));
//G623: help_arg(252)        => subst_sym(286).
	p(NT(252), (NT(286)));
//G624: help_arg(252)        => normalize_sym(282).
	p(NT(252), (NT(282)));
//G625: help_arg(252)        => execute_sym(373).
	p(NT(252), (NT(373)));
//G626: help_arg(252)        => solve_sym(269).
	p(NT(252), (NT(269)));
//G627: help_arg(252)        => valid_sym(259).
	p(NT(252), (NT(259)));
//G628: help_arg(252)        => sat_sym(263).
	p(NT(252), (NT(263)));
//G629: help_arg(252)        => unsat_sym(266).
	p(NT(252), (NT(266)));
//G630: help_arg(252)        => run_sym(278).
	p(NT(252), (NT(278)));
//G631: help_arg(252)        => dnf_sym(296).
	p(NT(252), (NT(296)));
//G632: help_arg(252)        => cnf_sym(299).
	p(NT(252), (NT(299)));
//G633: help_arg(252)        => anf_sym(302).
	p(NT(252), (NT(302)));
//G634: help_arg(252)        => snf_sym(374).
	p(NT(252), (NT(374)));
//G635: help_arg(252)        => nnf_sym(305).
	p(NT(252), (NT(305)));
//G636: help_arg(252)        => pnf_sym(308).
	p(NT(252), (NT(308)));
//G637: help_arg(252)        => mnf_sym(311).
	p(NT(252), (NT(311)));
//G638: help_arg(252)        => onf_sym(314).
	p(NT(252), (NT(314)));
//G639: help_arg(252)        => qelim_sym(318).
	p(NT(252), (NT(318)));
//G640: __E___E_help_arg_171_172(377) => 's'.
	p(NT(377), (T(15)));
//G641: __E___E_help_arg_171_172(377) => null.
	p(NT(377), (nul));
//G642: __E_help_arg_171(376) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_171_172(377).
	p(NT(376), (T(11)+T(29)+T(16)+T(21)+T(55)+T(10)+T(11)+NT(377)));
//G643: examples_sym(375)    => __E_help_arg_171(376).
	p(NT(375), (NT(376)));
//G644: help_arg(252)        => examples_sym(375).
	p(NT(252), (NT(375)));
//G645: __E_history_173(379) => '%'.
	p(NT(379), (T(39)));
//G646: rel_history_sym(371) => '%' '-'.
	p(NT(371), (T(39)+T(30)));
//G647: history_id(381)      => digits(223).
	p(NT(381), (NT(223)));
//G648: __E___E_history_173_174(380) => history_id(381).
	p(NT(380), (NT(381)));
//G649: __E___E_history_173_174(380) => null.
	p(NT(380), (nul));
//G650: __E_history_173(379) => rel_history_sym(371) __E___E_history_173_174(380).
	p(NT(379), (NT(371)+NT(380)));
//G651: rel_history(378)     => __E_history_173(379).
	p(NT(378), (NT(379)));
//G652: history(353)         => rel_history(378).
	p(NT(353), (NT(378)));
//G653: abs_history_sym(370) => '%'.
	p(NT(370), (T(39)));
//G654: __E_history_175(383) => abs_history_sym(370) history_id(381).
	p(NT(383), (NT(370)+NT(381)));
//G655: abs_history(382)     => __E_history_175(383).
	p(NT(382), (NT(383)));
//G656: history(353)         => abs_history(382).
	p(NT(353), (NT(382)));
//G657: unreachable(384)     => BDD_ID(385).
	p(NT(384), (NT(385)));
//G658: stream_def(386)      => input_def(20).
	p(NT(386), (NT(20)));
//G659: stream_def(386)      => output_def(21).
	p(NT(386), (NT(21)));
//G660: fm_or_term(387)      => wff(15).
	p(NT(387), (NT(15)));
//G661: fm_or_term(387)      => bf(31).
	p(NT(387), (NT(31)));
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
		input_def, output_def, __E_definitions_4, spec_multiline, __E_spec_multiline_5, spec_part, __E_spec_multiline_6, __E_spec_multiline_7, ref, __E_rec_relation_8, 
		capture, bf, io_var_name, __E_input_def_9, typed, __, stream, __E_output_def_10, q_file_name, console_sym, 
		sym, __E_ref_11, offsets, ref_args, __E_ref_12, __E_ref_13, fp_fallback, first_sym, last_sym, __E_ref_args_14, 
		__E___E_ref_args_14_15, ref_arg, __E___E_ref_args_14_16, __E___E_ref_args_14_17, wff_parenthesis, __E_wff_18, wff_sometimes, __E_wff_19, __E___E_wff_19_20, wff_always, 
		__E_wff_21, __E___E_wff_21_22, wff_conditional, __E_wff_23, wff_all, __E_wff_24, q_vars, wff_ex, __E_wff_25, wff_ref, 
		constraint, wff_imply, __E_wff_26, wff_rimply, __E_wff_27, wff_equiv, __E_wff_28, wff_or, __E_wff_29, wff_xor, 
		__E_wff_30, wff_and, __E_wff_31, wff_neg, __E_wff_32, wff_t, wff_f, bf_interval, __E_wff_33, bf_eq, 
		__E_wff_34, bf_neq, __E_wff_35, bf_lt, __E_wff_36, bf_nlt, __E_wff_37, bf_lteq, __E_wff_38, bf_nlteq, 
		__E_wff_39, bf_gt, __E_wff_40, bf_ngt, __E_wff_41, bf_gteq, __E_wff_42, bf_ngteq, __E_wff_43, bf_parenthesis, 
		__E_bf_44, ba_constant, variable, bf_fall, __E_bf_45, bf_fex, __E_bf_46, bf_ref, bf_cast, __E_bf_47, 
		num, bf_cast_oprnd, __E___E_bf_47_48, bf_t, bf_f, bf_neg, bf_shr, __E_bf_49, __E___E_bf_49_50, bf_shl, 
		__E_bf_51, __E___E_bf_51_52, bf_add, __E_bf_53, bf_sub, __E_bf_54, __E___E_bf_54_55, bf_mod, __E_bf_56, __E___E_bf_56_57, 
		bf_mul, __E_bf_58, bf_div, __E_bf_59, __E___E_bf_59_60, bf_nor, __E_bf_61, __E___E_bf_61_62, bf_xnor, __E_bf_63, 
		__E___E_bf_63_64, bf_nand, __E_bf_65, __E___E_bf_65_66, bf_or, __E_bf_67, bf_xor, __E_bf_68, __E_bf_69, __E___E_bf_69_70, 
		__E_bf_71, __E___E_bf_71_72, bf_and, __E_bf_73, __E___E_bf_73_74, __E_bf_75, bf_neg_oprnd, __E___E_bf_75_76, bf_and_nosep, __E_bf_77, 
		bf_and_nosep_1st_oprnd, __E___E_bf_77_78, bf_and_nosep_2nd_oprnd, __E___E_bf_77_79, ctn_neq, __E_constraint_80, ctnvar, ctn_eq, __E_constraint_81, ctn_gteq, 
		__E_constraint_82, ctn_gt, __E_constraint_83, ctn_lteq, __E_constraint_84, ctn_lt, __E_constraint_85, __E_ba_constant_86, source, __E_ba_constant_87, 
		src_c, __E_source_88, __E___E_source_88_89, __E___E_source_88_90, __E_src_c_91, __E_src_c_92, __E_variable_93, uconst, io_var, var_name, 
		__E_variable_94, __E_var_name_95, __E_var_name_96, __E_var_name_97, __E_var_name_98, __E_var_name_99, offset, chars, uconst_name, __E_uconst_100, 
		__E___E_uconst_100_101, q_var, __E_q_vars_102, __E_q_vars_103, __E_offsets_104, __E_offsets_105, integer, shift, __E_offset_106, __E_shift_107, 
		__E___E_shift_107_108, __E_chars_109, __E_chars_110, digits, __E_digits_111, __E_integer_112, type, subtype, __E_typed_113, comment, 
		__E_comment_114, __E_comment_115, __E_comment_116, cli, cli_command, __E_cli_117, __E_cli_118, __E_cli_119, quit_cmd, quit_sym, 
		__E_cli_command_120, version_cmd, version_sym, __E_cli_command_121, clear_cmd, clear_sym, __E_cli_command_122, help_cmd, __E_cli_command_123, help_sym, 
		__E___E_cli_command_123_124, __E___E_cli_command_123_125, help_arg, file_cmd, __E_cli_command_126, file_sym, q_string, valid_cmd, __E_cli_command_127, valid_sym, 
		spec_arg, sat_cmd, __E_cli_command_128, sat_sym, unsat_cmd, __E_cli_command_129, unsat_sym, solve_cmd, __E_cli_command_130, solve_sym, 
		__E___E_cli_command_130_131, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_132, lgrs_sym, run_cmd, __E_cli_command_133, run_sym, __E___E_cli_command_133_134, 
		normalize_cmd, __E_cli_command_135, normalize_sym, __E___E_cli_command_135_136, subst_cmd, __E_cli_command_137, subst_sym, __E___E_cli_command_137_138, nf_cmd_arg, inst_cmd, 
		__E_cli_command_139, inst_sym, __E___E_cli_command_139_140, inst_args, dnf_cmd, __E_cli_command_141, dnf_sym, cnf_cmd, __E_cli_command_142, cnf_sym, 
		anf_cmd, __E_cli_command_143, anf_sym, nnf_cmd, __E_cli_command_144, nnf_sym, pnf_cmd, __E_cli_command_145, pnf_sym, mnf_cmd, 
		__E_cli_command_146, mnf_sym, onf_cmd, __E_cli_command_147, onf_sym, onf_cmd_arg, qelim_cmd, __E_cli_command_148, qelim_sym, get_cmd, 
		__E_cli_command_149, get_sym, __E___E_cli_command_149_150, option_name, set_cmd, __E_cli_command_151, set_sym, __E___E_cli_command_151_152, option_value, enable_cmd, 
		__E_cli_command_153, enable_sym, disable_cmd, __E_cli_command_154, disable_sym, toggle_cmd, __E_cli_command_155, toggle_sym, def_list_cmd, __E_cli_command_156, 
		def_sym, __E___E_cli_command_156_157, def_print_cmd, __E_cli_command_158, def_rr_cmd, def_input_cmd, def_output_cmd, history_list_cmd, __E_cli_command_159, history_sym, 
		__E___E_cli_command_159_160, history_print_cmd, __E_cli_command_161, history, history_store_cmd, __E_cli_command_162, __E_solve_options_163, solve_option, __E_solve_options_164, __E_solve_option_165, 
		solver_mode, solver_mode_minimum, __E_solver_mode_166, solver_mode_maximum, __E_solver_mode_167, file_name, __E_file_name_168, __E_option_name_169, __E_option_value_170, bf_cmd_arg, 
		abs_history_sym, rel_history_sym, selection_sym, execute_sym, snf_sym, examples_sym, __E_help_arg_171, __E___E_help_arg_171_172, rel_history, __E_history_173, 
		__E___E_history_173_174, history_id, abs_history, __E_history_175, unreachable, BDD_ID, stream_def, fm_or_term, __N_0, __N_1, 
		__N_2, __N_3, __N_4, __N_5, __N_6, __N_7, __N_8, __N_9, __N_10, __N_11, 
		__N_12, __N_13, __N_14, __N_15, __N_16, __N_17, 
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
