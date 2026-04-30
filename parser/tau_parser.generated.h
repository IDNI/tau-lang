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
	"__E_bf_44", "ba_constant", "variable", "bf_fall", "__E_bf_45", "bf_fex", "__E_bf_46", "bf_ref", "bf_shr", "__E_bf_47", 
	"__E___E_bf_47_48", "bf_shl", "__E_bf_49", "__E___E_bf_49_50", "bf_add", "__E_bf_51", "bf_sub", "__E_bf_52", "__E___E_bf_52_53", "bf_mod", 
	"__E_bf_54", "__E___E_bf_54_55", "bf_mul", "__E_bf_56", "bf_div", "__E_bf_57", "__E___E_bf_57_58", "bf_nor", "__E_bf_59", "__E___E_bf_59_60", 
	"bf_xnor", "__E_bf_61", "__E___E_bf_61_62", "bf_nand", "__E_bf_63", "__E___E_bf_63_64", "bf_or", "__E_bf_65", "bf_xor", "__E_bf_66", 
	"bf_t", "__E_bf_67", "__E___E_bf_67_68", "bf_f", "__E_bf_69", "__E___E_bf_69_70", "bf_and", "__E_bf_71", "__E___E_bf_71_72", "bf_neg", 
	"__E_bf_73", "bf_neg_oprnd", "__E___E_bf_73_74", "bf_and_nosep", "__E_bf_75", "bf_and_nosep_1st_oprnd", "__E___E_bf_75_76", "bf_and_nosep_2nd_oprnd", "__E___E_bf_75_77", "ctn_neq", 
	"__E_constraint_78", "ctnvar", "num", "ctn_eq", "__E_constraint_79", "ctn_gteq", "__E_constraint_80", "ctn_gt", "__E_constraint_81", "ctn_lteq", 
	"__E_constraint_82", "ctn_lt", "__E_constraint_83", "__E_ba_constant_84", "source", "__E_ba_constant_85", "src_c", "__E_source_86", "__E___E_source_86_87", "__E___E_source_86_88", 
	"__E_src_c_89", "__E_src_c_90", "__E_variable_91", "uconst", "io_var", "var_name", "__E_variable_92", "__E_var_name_93", "__E_var_name_94", "__E_var_name_95", 
	"__E_var_name_96", "__E_var_name_97", "offset", "chars", "uconst_name", "__E_uconst_98", "__E___E_uconst_98_99", "q_var", "__E_q_vars_100", "__E_q_vars_101", 
	"__E_offsets_102", "__E_offsets_103", "integer", "shift", "__E_offset_104", "__E_shift_105", "__E___E_shift_105_106", "__E_chars_107", "__E_chars_108", "digits", 
	"__E_digits_109", "__E_integer_110", "type", "subtype", "__E_typed_111", "comment", "__E_comment_112", "__E_comment_113", "__E_comment_114", "cli", 
	"cli_command", "__E_cli_115", "__E_cli_116", "__E_cli_117", "quit_cmd", "quit_sym", "__E_cli_command_118", "version_cmd", "version_sym", "__E_cli_command_119", 
	"clear_cmd", "clear_sym", "__E_cli_command_120", "help_cmd", "__E_cli_command_121", "help_sym", "__E___E_cli_command_121_122", "__E___E_cli_command_121_123", "help_arg", "file_cmd", 
	"__E_cli_command_124", "file_sym", "q_string", "valid_cmd", "__E_cli_command_125", "valid_sym", "spec_arg", "sat_cmd", "__E_cli_command_126", "sat_sym", 
	"unsat_cmd", "__E_cli_command_127", "unsat_sym", "solve_cmd", "__E_cli_command_128", "solve_sym", "__E___E_cli_command_128_129", "solve_options", "wff_cmd_arg", "lgrs_cmd", 
	"__E_cli_command_130", "lgrs_sym", "run_cmd", "__E_cli_command_131", "run_sym", "__E___E_cli_command_131_132", "normalize_cmd", "__E_cli_command_133", "normalize_sym", "__E___E_cli_command_133_134", 
	"subst_cmd", "__E_cli_command_135", "subst_sym", "__E___E_cli_command_135_136", "nf_cmd_arg", "inst_cmd", "__E_cli_command_137", "inst_sym", "__E___E_cli_command_137_138", "inst_args", 
	"dnf_cmd", "__E_cli_command_139", "dnf_sym", "cnf_cmd", "__E_cli_command_140", "cnf_sym", "anf_cmd", "__E_cli_command_141", "anf_sym", "nnf_cmd", 
	"__E_cli_command_142", "nnf_sym", "pnf_cmd", "__E_cli_command_143", "pnf_sym", "mnf_cmd", "__E_cli_command_144", "mnf_sym", "onf_cmd", "__E_cli_command_145", 
	"onf_sym", "onf_cmd_arg", "qelim_cmd", "__E_cli_command_146", "qelim_sym", "get_cmd", "__E_cli_command_147", "get_sym", "__E___E_cli_command_147_148", "option_name", 
	"set_cmd", "__E_cli_command_149", "set_sym", "__E___E_cli_command_149_150", "option_value", "enable_cmd", "__E_cli_command_151", "enable_sym", "disable_cmd", "__E_cli_command_152", 
	"disable_sym", "toggle_cmd", "__E_cli_command_153", "toggle_sym", "def_list_cmd", "__E_cli_command_154", "def_sym", "__E___E_cli_command_154_155", "def_print_cmd", "__E_cli_command_156", 
	"def_rr_cmd", "def_input_cmd", "def_output_cmd", "history_list_cmd", "__E_cli_command_157", "history_sym", "__E___E_cli_command_157_158", "history_print_cmd", "__E_cli_command_159", "history", 
	"history_store_cmd", "__E_cli_command_160", "__E_solve_options_161", "solve_option", "__E_solve_options_162", "__E_solve_option_163", "solver_mode", "solver_mode_minimum", "__E_solver_mode_164", "solver_mode_maximum", 
	"__E_solver_mode_165", "file_name", "__E_file_name_166", "__E_option_name_167", "__E_option_value_168", "bf_cmd_arg", "abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", 
	"snf_sym", "examples_sym", "__E_help_arg_169", "__E___E_help_arg_169_170", "rel_history", "__E_history_171", "__E___E_history_171_172", "history_id", "abs_history", "__E_history_173", 
	"stream_def", "fm_or_term", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", "__N_7", 
	"__N_8", "__N_9", "__N_10", "__N_11", "__N_12", "__N_13", "__N_14", "__N_15", "__N_16", "__N_17", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', 'a', 'b', 'k', 
	'r', ',', 'm', '<', '>', 'w', 'y', '[', ']', '?', 
	'x', '-', '|', '^', '&', '!', 'T', 'F', '+', '%', 
	'*', '/', '1', '0', '\'', '{', '}', '_', '$', 'v', 
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
			30, 32, 40, 171, 184, 195, 204, 212, 219, 222,
			223, 319, 324, 361
		},
		.to_inline = {
			{ 15, 54, 15 },
			{ 25 },
			{ 31, 109, 31 },
			{ 161, 109, 31 },
			{ 163, 109, 156 },
			{ 165, 109, 31 },
			{ 167, 109, 31 },
			{ 186 },
			{ 193 },
			{ 193, 204 },
			{ 203 },
			{ 207 },
			{ 256 },
			{ 268 },
			{ 284 },
			{ 289 },
			{ 311 },
			{ 365 }
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
//G159: __N_0(382)           => bf_shr(118).
	p(NT(382), (NT(118)));
//G160: __E___E_bf_47_48(120) => bf(31) & ~( __N_0(382) ).	 # conjunctive
	p(NT(120), (NT(31)) & ~(NT(382)));
//G161: __E_bf_47(119)       => bf(31) _(13) '>' '>' _(13) __E___E_bf_47_48(120).
	p(NT(119), (NT(31)+NT(13)+T(23)+T(23)+NT(13)+NT(120)));
//G162: bf_shr(118)          => __E_bf_47(119).
	p(NT(118), (NT(119)));
//G163: bf(31)               => bf_shr(118).
	p(NT(31), (NT(118)));
//G164: __N_1(383)           => bf_shl(121).
	p(NT(383), (NT(121)));
//G165: __E___E_bf_49_50(123) => bf(31) & ~( __N_1(383) ).	 # conjunctive
	p(NT(123), (NT(31)) & ~(NT(383)));
//G166: __E_bf_49(122)       => bf(31) _(13) '<' '<' _(13) __E___E_bf_49_50(123).
	p(NT(122), (NT(31)+NT(13)+T(22)+T(22)+NT(13)+NT(123)));
//G167: bf_shl(121)          => __E_bf_49(122).
	p(NT(121), (NT(122)));
//G168: bf(31)               => bf_shl(121).
	p(NT(31), (NT(121)));
//G169: __E_bf_51(125)       => bf(31) _(13) '+' _(13) bf(31).
	p(NT(125), (NT(31)+NT(13)+T(37)+NT(13)+NT(31)));
//G170: bf_add(124)          => __E_bf_51(125).
	p(NT(124), (NT(125)));
//G171: bf(31)               => bf_add(124).
	p(NT(31), (NT(124)));
//G172: __N_2(384)           => bf_sub(126).
	p(NT(384), (NT(126)));
//G173: __E___E_bf_52_53(128) => bf(31) & ~( __N_2(384) ).	 # conjunctive
	p(NT(128), (NT(31)) & ~(NT(384)));
//G174: __E_bf_52(127)       => bf(31) _(13) '-' _(13) __E___E_bf_52_53(128).
	p(NT(127), (NT(31)+NT(13)+T(30)+NT(13)+NT(128)));
//G175: bf_sub(126)          => __E_bf_52(127).
	p(NT(126), (NT(127)));
//G176: bf(31)               => bf_sub(126).
	p(NT(31), (NT(126)));
//G177: __N_3(385)           => bf_mod(129).
	p(NT(385), (NT(129)));
//G178: __E___E_bf_54_55(131) => bf(31) & ~( __N_3(385) ).	 # conjunctive
	p(NT(131), (NT(31)) & ~(NT(385)));
//G179: __E_bf_54(130)       => bf(31) _(13) '%' _(13) __E___E_bf_54_55(131).
	p(NT(130), (NT(31)+NT(13)+T(38)+NT(13)+NT(131)));
//G180: bf_mod(129)          => __E_bf_54(130).
	p(NT(129), (NT(130)));
//G181: bf(31)               => bf_mod(129).
	p(NT(31), (NT(129)));
//G182: __E_bf_56(133)       => bf(31) _(13) '*' _(13) bf(31).
	p(NT(133), (NT(31)+NT(13)+T(39)+NT(13)+NT(31)));
//G183: bf_mul(132)          => __E_bf_56(133).
	p(NT(132), (NT(133)));
//G184: bf(31)               => bf_mul(132).
	p(NT(31), (NT(132)));
//G185: __N_4(386)           => bf_div(134).
	p(NT(386), (NT(134)));
//G186: __E___E_bf_57_58(136) => bf(31) & ~( __N_4(386) ).	 # conjunctive
	p(NT(136), (NT(31)) & ~(NT(386)));
//G187: __E_bf_57(135)       => bf(31) _(13) '/' _(13) __E___E_bf_57_58(136).
	p(NT(135), (NT(31)+NT(13)+T(40)+NT(13)+NT(136)));
//G188: bf_div(134)          => __E_bf_57(135).
	p(NT(134), (NT(135)));
//G189: bf(31)               => bf_div(134).
	p(NT(31), (NT(134)));
//G190: __N_5(387)           => bf_nor(137).
	p(NT(387), (NT(137)));
//G191: __E___E_bf_59_60(139) => bf(31) & ~( __N_5(387) ).	 # conjunctive
	p(NT(139), (NT(31)) & ~(NT(387)));
//G192: __E_bf_59(138)       => bf(31) _(13) '!' '|' _(13) __E___E_bf_59_60(139).
	p(NT(138), (NT(31)+NT(13)+T(34)+T(31)+NT(13)+NT(139)));
//G193: bf_nor(137)          => __E_bf_59(138).
	p(NT(137), (NT(138)));
//G194: bf(31)               => bf_nor(137).
	p(NT(31), (NT(137)));
//G195: __N_6(388)           => bf_xnor(140).
	p(NT(388), (NT(140)));
//G196: __E___E_bf_61_62(142) => bf(31) & ~( __N_6(388) ).	 # conjunctive
	p(NT(142), (NT(31)) & ~(NT(388)));
//G197: __E_bf_61(141)       => bf(31) _(13) '!' '^' _(13) __E___E_bf_61_62(142).
	p(NT(141), (NT(31)+NT(13)+T(34)+T(32)+NT(13)+NT(142)));
//G198: bf_xnor(140)         => __E_bf_61(141).
	p(NT(140), (NT(141)));
//G199: bf(31)               => bf_xnor(140).
	p(NT(31), (NT(140)));
//G200: __N_7(389)           => bf_nand(143).
	p(NT(389), (NT(143)));
//G201: __E___E_bf_63_64(145) => bf(31) & ~( __N_7(389) ).	 # conjunctive
	p(NT(145), (NT(31)) & ~(NT(389)));
//G202: __E_bf_63(144)       => bf(31) _(13) '!' '&' _(13) __E___E_bf_63_64(145).
	p(NT(144), (NT(31)+NT(13)+T(34)+T(33)+NT(13)+NT(145)));
//G203: bf_nand(143)         => __E_bf_63(144).
	p(NT(143), (NT(144)));
//G204: bf(31)               => bf_nand(143).
	p(NT(31), (NT(143)));
//G205: __E_bf_65(147)       => bf(31) _(13) '|' _(13) bf(31).
	p(NT(147), (NT(31)+NT(13)+T(31)+NT(13)+NT(31)));
//G206: bf_or(146)           => __E_bf_65(147).
	p(NT(146), (NT(147)));
//G207: bf(31)               => bf_or(146).
	p(NT(31), (NT(146)));
//G208: __E_bf_66(149)       => bf(31) _(13) '^' _(13) bf(31).
	p(NT(149), (NT(31)+NT(13)+T(32)+NT(13)+NT(31)));
//G209: bf_xor(148)          => __E_bf_66(149).
	p(NT(148), (NT(149)));
//G210: bf(31)               => bf_xor(148).
	p(NT(31), (NT(148)));
//G211: __E___E_bf_67_68(152) => typed(34).
	p(NT(152), (NT(34)));
//G212: __E___E_bf_67_68(152) => null.
	p(NT(152), (nul));
//G213: __E_bf_67(151)       => '1' __E___E_bf_67_68(152).
	p(NT(151), (T(41)+NT(152)));
//G214: bf_t(150)            => __E_bf_67(151).
	p(NT(150), (NT(151)));
//G215: bf(31)               => bf_t(150).
	p(NT(31), (NT(150)));
//G216: __E___E_bf_69_70(155) => typed(34).
	p(NT(155), (NT(34)));
//G217: __E___E_bf_69_70(155) => null.
	p(NT(155), (nul));
//G218: __E_bf_69(154)       => '0' __E___E_bf_69_70(155).
	p(NT(154), (T(42)+NT(155)));
//G219: bf_f(153)            => __E_bf_69(154).
	p(NT(153), (NT(154)));
//G220: bf(31)               => bf_f(153).
	p(NT(31), (NT(153)));
//G221: __E___E_bf_71_72(158) => _(13) '&' _(13).
	p(NT(158), (NT(13)+T(33)+NT(13)));
//G222: __E___E_bf_71_72(158) => __(35).
	p(NT(158), (NT(35)));
//G223: __E_bf_71(157)       => bf(31) __E___E_bf_71_72(158) bf(31).
	p(NT(157), (NT(31)+NT(158)+NT(31)));
//G224: bf_and(156)          => __E_bf_71(157).
	p(NT(156), (NT(157)));
//G225: bf(31)               => bf_and(156).
	p(NT(31), (NT(156)));
//G226: __E___E_bf_73_74(162) => bf_parenthesis(109).
	p(NT(162), (NT(109)));
//G227: __E___E_bf_73_74(162) => ba_constant(111).
	p(NT(162), (NT(111)));
//G228: __E___E_bf_73_74(162) => variable(112).
	p(NT(162), (NT(112)));
//G229: __E___E_bf_73_74(162) => bf_ref(117).
	p(NT(162), (NT(117)));
//G230: __E___E_bf_73_74(162) => bf_t(150).
	p(NT(162), (NT(150)));
//G231: __E___E_bf_73_74(162) => bf_f(153).
	p(NT(162), (NT(153)));
//G232: __E___E_bf_73_74(162) => bf_neg(159).
	p(NT(162), (NT(159)));
//G233: __E___E_bf_73_74(162) => capture(30).
	p(NT(162), (NT(30)));
//G234: bf_neg_oprnd(161)    => __E___E_bf_73_74(162).
	p(NT(161), (NT(162)));
//G235: __E_bf_73(160)       => bf_neg_oprnd(161) _(13) '\''.
	p(NT(160), (NT(161)+NT(13)+T(43)));
//G236: bf_neg(159)          => __E_bf_73(160).
	p(NT(159), (NT(160)));
//G237: bf(31)               => bf_neg(159).
	p(NT(31), (NT(159)));
//G238: __E___E_bf_75_76(166) => bf_parenthesis(109).
	p(NT(166), (NT(109)));
//G239: __E___E_bf_75_76(166) => variable(112).
	p(NT(166), (NT(112)));
//G240: __E___E_bf_75_76(166) => bf_ref(117).
	p(NT(166), (NT(117)));
//G241: __E___E_bf_75_76(166) => bf_neg(159).
	p(NT(166), (NT(159)));
//G242: __E___E_bf_75_76(166) => bf_and_nosep(163).
	p(NT(166), (NT(163)));
//G243: __E___E_bf_75_76(166) => capture(30).
	p(NT(166), (NT(30)));
//G244: bf_and_nosep_1st_oprnd(165) => __E___E_bf_75_76(166).
	p(NT(165), (NT(166)));
//G245: __E___E_bf_75_77(168) => bf_parenthesis(109).
	p(NT(168), (NT(109)));
//G246: __E___E_bf_75_77(168) => ba_constant(111).
	p(NT(168), (NT(111)));
//G247: __E___E_bf_75_77(168) => variable(112).
	p(NT(168), (NT(112)));
//G248: __E___E_bf_75_77(168) => bf_ref(117).
	p(NT(168), (NT(117)));
//G249: __E___E_bf_75_77(168) => bf_neg(159).
	p(NT(168), (NT(159)));
//G250: __E___E_bf_75_77(168) => capture(30).
	p(NT(168), (NT(30)));
//G251: bf_and_nosep_2nd_oprnd(167) => __E___E_bf_75_77(168).
	p(NT(167), (NT(168)));
//G252: __E_bf_75(164)       => bf_and_nosep_1st_oprnd(165) bf_and_nosep_2nd_oprnd(167).
	p(NT(164), (NT(165)+NT(167)));
//G253: bf_and_nosep(163)    => __E_bf_75(164).
	p(NT(163), (NT(164)));
//G254: bf(31)               => bf_and_nosep(163).
	p(NT(31), (NT(163)));
//G255: bf(31)               => capture(30).
	p(NT(31), (NT(30)));
//G256: __E_constraint_78(170) => '[' ctnvar(171) _(13) '!' '=' _(13) num(172) ']'.
	p(NT(170), (T(26)+NT(171)+NT(13)+T(34)+T(3)+NT(13)+NT(172)+T(27)));
//G257: __E_constraint_78(170) => '[' num(172) _(13) '!' '=' _(13) ctnvar(171) ']'.
	p(NT(170), (T(26)+NT(172)+NT(13)+T(34)+T(3)+NT(13)+NT(171)+T(27)));
//G258: ctn_neq(169)         => __E_constraint_78(170).
	p(NT(169), (NT(170)));
//G259: constraint(70)       => ctn_neq(169).
	p(NT(70), (NT(169)));
//G260: __E_constraint_79(174) => '[' ctnvar(171) _(13) '=' _(13) num(172) ']'.
	p(NT(174), (T(26)+NT(171)+NT(13)+T(3)+NT(13)+NT(172)+T(27)));
//G261: __E_constraint_79(174) => '[' num(172) _(13) '=' _(13) ctnvar(171) ']'.
	p(NT(174), (T(26)+NT(172)+NT(13)+T(3)+NT(13)+NT(171)+T(27)));
//G262: ctn_eq(173)          => __E_constraint_79(174).
	p(NT(173), (NT(174)));
//G263: constraint(70)       => ctn_eq(173).
	p(NT(70), (NT(173)));
//G264: __E_constraint_80(176) => '[' ctnvar(171) _(13) '>' '=' _(13) num(172) ']'.
	p(NT(176), (T(26)+NT(171)+NT(13)+T(23)+T(3)+NT(13)+NT(172)+T(27)));
//G265: __E_constraint_80(176) => '[' num(172) _(13) '>' '=' _(13) ctnvar(171) ']'.
	p(NT(176), (T(26)+NT(172)+NT(13)+T(23)+T(3)+NT(13)+NT(171)+T(27)));
//G266: ctn_gteq(175)        => __E_constraint_80(176).
	p(NT(175), (NT(176)));
//G267: constraint(70)       => ctn_gteq(175).
	p(NT(70), (NT(175)));
//G268: __E_constraint_81(178) => '[' ctnvar(171) _(13) '>' _(13) num(172) ']'.
	p(NT(178), (T(26)+NT(171)+NT(13)+T(23)+NT(13)+NT(172)+T(27)));
//G269: __E_constraint_81(178) => '[' num(172) _(13) '>' _(13) ctnvar(171) ']'.
	p(NT(178), (T(26)+NT(172)+NT(13)+T(23)+NT(13)+NT(171)+T(27)));
//G270: ctn_gt(177)          => __E_constraint_81(178).
	p(NT(177), (NT(178)));
//G271: constraint(70)       => ctn_gt(177).
	p(NT(70), (NT(177)));
//G272: __E_constraint_82(180) => '[' ctnvar(171) _(13) '<' '=' _(13) num(172) ']'.
	p(NT(180), (T(26)+NT(171)+NT(13)+T(22)+T(3)+NT(13)+NT(172)+T(27)));
//G273: __E_constraint_82(180) => '[' num(172) _(13) '<' '=' _(13) ctnvar(171) ']'.
	p(NT(180), (T(26)+NT(172)+NT(13)+T(22)+T(3)+NT(13)+NT(171)+T(27)));
//G274: ctn_lteq(179)        => __E_constraint_82(180).
	p(NT(179), (NT(180)));
//G275: constraint(70)       => ctn_lteq(179).
	p(NT(70), (NT(179)));
//G276: __E_constraint_83(182) => '[' ctnvar(171) _(13) '<' _(13) num(172) ']'.
	p(NT(182), (T(26)+NT(171)+NT(13)+T(22)+NT(13)+NT(172)+T(27)));
//G277: __E_constraint_83(182) => '[' num(172) _(13) '<' _(13) ctnvar(171) ']'.
	p(NT(182), (T(26)+NT(172)+NT(13)+T(22)+NT(13)+NT(171)+T(27)));
//G278: ctn_lt(181)          => __E_constraint_83(182).
	p(NT(181), (NT(182)));
//G279: constraint(70)       => ctn_lt(181).
	p(NT(70), (NT(181)));
//G280: __E_ba_constant_84(183) => capture(30).
	p(NT(183), (NT(30)));
//G281: __E_ba_constant_84(183) => source(184).
	p(NT(183), (NT(184)));
//G282: __E_ba_constant_85(185) => typed(34).
	p(NT(185), (NT(34)));
//G283: __E_ba_constant_85(185) => null.
	p(NT(185), (nul));
//G284: ba_constant(111)     => '{' _(13) __E_ba_constant_84(183) _(13) '}' __E_ba_constant_85(185).
	p(NT(111), (T(44)+NT(13)+NT(183)+NT(13)+T(45)+NT(185)));
//G285: __E___E_source_86_87(188) => src_c(186).
	p(NT(188), (NT(186)));
//G286: __E___E_source_86_87(188) => space(2).
	p(NT(188), (NT(2)));
//G287: __E___E_source_86_88(189) => null.
	p(NT(189), (nul));
//G288: __E___E_source_86_88(189) => __E___E_source_86_87(188) __E___E_source_86_88(189).
	p(NT(189), (NT(188)+NT(189)));
//G289: __E_source_86(187)   => __E___E_source_86_88(189) src_c(186).
	p(NT(187), (NT(189)+NT(186)));
//G290: __E_source_86(187)   => null.
	p(NT(187), (nul));
//G291: source(184)          => src_c(186) __E_source_86(187).
	p(NT(184), (NT(186)+NT(187)));
//G292: src_c(186)           => alnum(6).
	p(NT(186), (NT(6)));
//G293: __N_8(390)           => '{'.
	p(NT(390), (T(44)));
//G294: __N_9(391)           => '}'.
	p(NT(391), (T(45)));
//G295: src_c(186)           => ~( __N_8(390) ) & ~( __N_9(391) ) & punct(7).	 # conjunctive
	p(NT(186), ~(NT(390)) & ~(NT(391)) & (NT(7)));
//G296: __E_src_c_89(190)    => src_c(186).
	p(NT(190), (NT(186)));
//G297: __E_src_c_89(190)    => space(2).
	p(NT(190), (NT(2)));
//G298: __E_src_c_90(191)    => null.
	p(NT(191), (nul));
//G299: __E_src_c_90(191)    => __E_src_c_89(190) __E_src_c_90(191).
	p(NT(191), (NT(190)+NT(191)));
//G300: src_c(186)           => '{' __E_src_c_90(191) '}'.
	p(NT(186), (T(44)+NT(191)+T(45)));
//G301: __E_variable_91(192) => uconst(193).
	p(NT(192), (NT(193)));
//G302: __E_variable_91(192) => io_var(194).
	p(NT(192), (NT(194)));
//G303: __E_variable_91(192) => var_name(195).
	p(NT(192), (NT(195)));
//G304: __E_variable_92(196) => typed(34).
	p(NT(196), (NT(34)));
//G305: __E_variable_92(196) => null.
	p(NT(196), (nul));
//G306: variable(112)        => __E_variable_91(192) __E_variable_92(196).
	p(NT(112), (NT(192)+NT(196)));
//G307: __N_10(392)          => 'F'.
	p(NT(392), (T(36)));
//G308: __N_11(393)          => 'T'.
	p(NT(393), (T(35)));
//G309: __E_var_name_93(197) => ~( __N_10(392) ) & ~( __N_11(393) ) & alpha(5).	 # conjunctive
	p(NT(197), ~(NT(392)) & ~(NT(393)) & (NT(5)));
//G310: __E_var_name_94(198) => null.
	p(NT(198), (nul));
//G311: __E_var_name_94(198) => digit(3) __E_var_name_94(198).
	p(NT(198), (NT(3)+NT(198)));
//G312: var_name(195)        => __E_var_name_93(197) __E_var_name_94(198).	 # guarded: charvar
	p(NT(195), (NT(197)+NT(198)));
	p.back().guard = "charvar";
//G313: __N_12(394)          => 'F'.
	p(NT(394), (T(36)));
//G314: __N_13(395)          => 'T'.
	p(NT(395), (T(35)));
//G315: __E_var_name_95(199) => ~( __N_12(394) ) & ~( __N_13(395) ) & alpha(5).	 # conjunctive
	p(NT(199), ~(NT(394)) & ~(NT(395)) & (NT(5)));
//G316: __E_var_name_96(200) => alnum(6).
	p(NT(200), (NT(6)));
//G317: __E_var_name_96(200) => '_'.
	p(NT(200), (T(46)));
//G318: __E_var_name_97(201) => null.
	p(NT(201), (nul));
//G319: __E_var_name_97(201) => __E_var_name_96(200) __E_var_name_97(201).
	p(NT(201), (NT(200)+NT(201)));
//G320: var_name(195)        => __E_var_name_95(199) __E_var_name_97(201).	 # guarded: var
	p(NT(195), (NT(199)+NT(201)));
	p.back().guard = "var";
//G321: io_var(194)          => io_var_name(32) '[' offset(202) ']'.
	p(NT(194), (NT(32)+T(26)+NT(202)+T(27)));
//G322: io_var_name(32)      => chars(203).
	p(NT(32), (NT(203)));
//G323: __E___E_uconst_98_99(206) => chars(203) _(13).
	p(NT(206), (NT(203)+NT(13)));
//G324: __E___E_uconst_98_99(206) => null.
	p(NT(206), (nul));
//G325: __E_uconst_98(205)   => __E___E_uconst_98_99(206) ':' _(13) chars(203).
	p(NT(205), (NT(206)+T(2)+NT(13)+NT(203)));
//G326: uconst_name(204)     => __E_uconst_98(205).
	p(NT(204), (NT(205)));
//G327: uconst(193)          => '<' _(13) uconst_name(204) _(13) '>'.
	p(NT(193), (T(22)+NT(13)+NT(204)+NT(13)+T(23)));
//G328: __E_q_vars_100(208)  => _(13) ',' _(13) q_var(207).
	p(NT(208), (NT(13)+T(20)+NT(13)+NT(207)));
//G329: __E_q_vars_101(209)  => null.
	p(NT(209), (nul));
//G330: __E_q_vars_101(209)  => __E_q_vars_100(208) __E_q_vars_101(209).
	p(NT(209), (NT(208)+NT(209)));
//G331: q_vars(66)           => q_var(207) __E_q_vars_101(209).
	p(NT(66), (NT(207)+NT(209)));
//G332: q_var(207)           => capture(30).
	p(NT(207), (NT(30)));
//G333: __N_14(396)          => uconst(193).
	p(NT(396), (NT(193)));
//G334: q_var(207)           => variable(112) & ~( __N_14(396) ).	 # conjunctive
	p(NT(207), (NT(112)) & ~(NT(396)));
//G335: ctnvar(171)          => chars(203).
	p(NT(171), (NT(203)));
//G336: __E_offsets_102(210) => _(13) ',' _(13) offset(202).
	p(NT(210), (NT(13)+T(20)+NT(13)+NT(202)));
//G337: __E_offsets_103(211) => null.
	p(NT(211), (nul));
//G338: __E_offsets_103(211) => __E_offsets_102(210) __E_offsets_103(211).
	p(NT(211), (NT(210)+NT(211)));
//G339: offsets(42)          => '[' _(13) offset(202) __E_offsets_103(211) _(13) ']'.
	p(NT(42), (T(26)+NT(13)+NT(202)+NT(211)+NT(13)+T(27)));
//G340: offset(202)          => integer(212).
	p(NT(202), (NT(212)));
//G341: offset(202)          => capture(30).
	p(NT(202), (NT(30)));
//G342: offset(202)          => shift(213).
	p(NT(202), (NT(213)));
//G343: __N_15(397)          => io_var(194).
	p(NT(397), (NT(194)));
//G344: __E_offset_104(214)  => variable(112) & ~( __N_15(397) ).	 # conjunctive
	p(NT(214), (NT(112)) & ~(NT(397)));
//G345: offset(202)          => __E_offset_104(214).
	p(NT(202), (NT(214)));
//G346: __E_shift_105(215)   => capture(30).
	p(NT(215), (NT(30)));
//G347: __N_16(398)          => io_var(194).
	p(NT(398), (NT(194)));
//G348: __E___E_shift_105_106(216) => variable(112) & ~( __N_16(398) ).	 # conjunctive
	p(NT(216), (NT(112)) & ~(NT(398)));
//G349: __E_shift_105(215)   => __E___E_shift_105_106(216).
	p(NT(215), (NT(216)));
//G350: shift(213)           => __E_shift_105(215) _(13) '-' _(13) num(172).
	p(NT(213), (NT(215)+NT(13)+T(30)+NT(13)+NT(172)));
//G351: __E_chars_107(217)   => alnum(6).
	p(NT(217), (NT(6)));
//G352: __E_chars_107(217)   => '_'.
	p(NT(217), (T(46)));
//G353: __E_chars_108(218)   => null.
	p(NT(218), (nul));
//G354: __E_chars_108(218)   => __E_chars_107(217) __E_chars_108(218).
	p(NT(218), (NT(217)+NT(218)));
//G355: chars(203)           => alpha(5) __E_chars_108(218).
	p(NT(203), (NT(5)+NT(218)));
//G356: __E_digits_109(220)  => digit(3).
	p(NT(220), (NT(3)));
//G357: __E_digits_109(220)  => digit(3) __E_digits_109(220).
	p(NT(220), (NT(3)+NT(220)));
//G358: digits(219)          => __E_digits_109(220).
	p(NT(219), (NT(220)));
//G359: num(172)             => digits(219).
	p(NT(172), (NT(219)));
//G360: __E_integer_110(221) => '-'.
	p(NT(221), (T(30)));
//G361: __E_integer_110(221) => null.
	p(NT(221), (nul));
//G362: integer(212)         => __E_integer_110(221) _(13) digits(219).
	p(NT(212), (NT(221)+NT(13)+NT(219)));
//G363: sym(40)              => chars(203).
	p(NT(40), (NT(203)));
//G364: capture(30)          => '$' chars(203).
	p(NT(30), (T(47)+NT(203)));
//G365: typed(34)            => _(13) ':' _(13) type(222) _(13) '[' _(13) subtype(223) _(13) ']'.
	p(NT(34), (NT(13)+T(2)+NT(13)+NT(222)+NT(13)+T(26)+NT(13)+NT(223)+NT(13)+T(27)));
//G366: __N_17(399)          => 'b' 'v'.
	p(NT(399), (T(17)+T(48)));
//G367: __E_typed_111(224)   => ~( __N_17(399) ) & type(222).	 # conjunctive
	p(NT(224), ~(NT(399)) & (NT(222)));
//G368: typed(34)            => _(13) ':' _(13) __E_typed_111(224).
	p(NT(34), (NT(13)+T(2)+NT(13)+NT(224)));
//G369: type(222)            => chars(203).
	p(NT(222), (NT(203)));
//G370: subtype(223)         => num(172).
	p(NT(223), (NT(172)));
//G371: __E_comment_112(226) => printable(8).
	p(NT(226), (NT(8)));
//G372: __E_comment_112(226) => '\t'.
	p(NT(226), (T(49)));
//G373: __E_comment_113(227) => null.
	p(NT(227), (nul));
//G374: __E_comment_113(227) => __E_comment_112(226) __E_comment_113(227).
	p(NT(227), (NT(226)+NT(227)));
//G375: __E_comment_114(228) => '\n'.
	p(NT(228), (T(50)));
//G376: __E_comment_114(228) => '\r'.
	p(NT(228), (T(51)));
//G377: __E_comment_114(228) => eof(1).
	p(NT(228), (NT(1)));
//G378: comment(225)         => '#' __E_comment_113(227) __E_comment_114(228).
	p(NT(225), (T(52)+NT(227)+NT(228)));
//G379: __(35)               => space(2).
	p(NT(35), (NT(2)));
//G380: __(35)               => comment(225).
	p(NT(35), (NT(225)));
//G381: __(35)               => __(35) space(2).
	p(NT(35), (NT(35)+NT(2)));
//G382: __(35)               => __(35) comment(225).
	p(NT(35), (NT(35)+NT(225)));
//G383: _(13)                => __(35).
	p(NT(13), (NT(35)));
//G384: _(13)                => null.
	p(NT(13), (nul));
//G385: cli(229)             => _(13).
	p(NT(229), (NT(13)));
//G386: __E_cli_115(231)     => '.' _(13) cli_command(230) _(13).
	p(NT(231), (T(1)+NT(13)+NT(230)+NT(13)));
//G387: __E_cli_116(232)     => null.
	p(NT(232), (nul));
//G388: __E_cli_116(232)     => __E_cli_115(231) __E_cli_116(232).
	p(NT(232), (NT(231)+NT(232)));
//G389: __E_cli_117(233)     => '.' _(13).
	p(NT(233), (T(1)+NT(13)));
//G390: __E_cli_117(233)     => null.
	p(NT(233), (nul));
//G391: cli(229)             => _(13) cli_command(230) _(13) __E_cli_116(232) __E_cli_117(233).
	p(NT(229), (NT(13)+NT(230)+NT(13)+NT(232)+NT(233)));
//G392: __E_cli_command_118(236) => 'q'.
	p(NT(236), (T(53)));
//G393: __E_cli_command_118(236) => 'q' 'u' 'i' 't'.
	p(NT(236), (T(53)+T(7)+T(4)+T(8)));
//G394: quit_sym(235)        => __E_cli_command_118(236).
	p(NT(235), (NT(236)));
//G395: quit_cmd(234)        => quit_sym(235).
	p(NT(234), (NT(235)));
//G396: cli_command(230)     => quit_cmd(234).
	p(NT(230), (NT(234)));
//G397: __E_cli_command_119(239) => 'v'.
	p(NT(239), (T(48)));
//G398: __E_cli_command_119(239) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(239), (T(48)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G399: version_sym(238)     => __E_cli_command_119(239).
	p(NT(238), (NT(239)));
//G400: version_cmd(237)     => version_sym(238).
	p(NT(237), (NT(238)));
//G401: cli_command(230)     => version_cmd(237).
	p(NT(230), (NT(237)));
//G402: __E_cli_command_120(242) => 'c'.
	p(NT(242), (T(14)));
//G403: __E_cli_command_120(242) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(242), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G404: clear_sym(241)       => __E_cli_command_120(242).
	p(NT(241), (NT(242)));
//G405: clear_cmd(240)       => clear_sym(241).
	p(NT(240), (NT(241)));
//G406: cli_command(230)     => clear_cmd(240).
	p(NT(230), (NT(240)));
//G407: __E___E_cli_command_121_122(246) => 'h'.
	p(NT(246), (T(54)));
//G408: __E___E_cli_command_121_122(246) => 'h' 'e' 'l' 'p'.
	p(NT(246), (T(54)+T(11)+T(10)+T(55)));
//G409: help_sym(245)        => __E___E_cli_command_121_122(246).
	p(NT(245), (NT(246)));
//G410: __E___E_cli_command_121_123(247) => __(35) help_arg(248).
	p(NT(247), (NT(35)+NT(248)));
//G411: __E___E_cli_command_121_123(247) => null.
	p(NT(247), (nul));
//G412: __E_cli_command_121(244) => help_sym(245) __E___E_cli_command_121_123(247).
	p(NT(244), (NT(245)+NT(247)));
//G413: help_cmd(243)        => __E_cli_command_121(244).
	p(NT(243), (NT(244)));
//G414: cli_command(230)     => help_cmd(243).
	p(NT(230), (NT(243)));
//G415: file_sym(251)        => 'f' 'i' 'l' 'e'.
	p(NT(251), (T(9)+T(4)+T(10)+T(11)));
//G416: __E_cli_command_124(250) => file_sym(251) __(35) q_string(252).
	p(NT(250), (NT(251)+NT(35)+NT(252)));
//G417: file_cmd(249)        => __E_cli_command_124(250).
	p(NT(249), (NT(250)));
//G418: cli_command(230)     => file_cmd(249).
	p(NT(230), (NT(249)));
//G419: valid_sym(255)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(255), (T(48)+T(16)+T(10)+T(4)+T(56)));
//G420: __E_cli_command_125(254) => valid_sym(255) __(35) spec_arg(256).
	p(NT(254), (NT(255)+NT(35)+NT(256)));
//G421: valid_cmd(253)       => __E_cli_command_125(254).
	p(NT(253), (NT(254)));
//G422: cli_command(230)     => valid_cmd(253).
	p(NT(230), (NT(253)));
//G423: sat_sym(259)         => 's' 'a' 't'.
	p(NT(259), (T(15)+T(16)+T(8)));
//G424: __E_cli_command_126(258) => sat_sym(259) __(35) spec_arg(256).
	p(NT(258), (NT(259)+NT(35)+NT(256)));
//G425: sat_cmd(257)         => __E_cli_command_126(258).
	p(NT(257), (NT(258)));
//G426: cli_command(230)     => sat_cmd(257).
	p(NT(230), (NT(257)));
//G427: unsat_sym(262)       => 'u' 'n' 's' 'a' 't'.
	p(NT(262), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G428: __E_cli_command_127(261) => unsat_sym(262) __(35) spec_arg(256).
	p(NT(261), (NT(262)+NT(35)+NT(256)));
//G429: unsat_cmd(260)       => __E_cli_command_127(261).
	p(NT(260), (NT(261)));
//G430: cli_command(230)     => unsat_cmd(260).
	p(NT(230), (NT(260)));
//G431: solve_sym(265)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(265), (T(15)+T(6)+T(10)+T(48)+T(11)));
//G432: __E___E_cli_command_128_129(266) => solve_options(267).
	p(NT(266), (NT(267)));
//G433: __E___E_cli_command_128_129(266) => null.
	p(NT(266), (nul));
//G434: __E_cli_command_128(264) => solve_sym(265) __E___E_cli_command_128_129(266) __(35) wff_cmd_arg(268).
	p(NT(264), (NT(265)+NT(266)+NT(35)+NT(268)));
//G435: solve_cmd(263)       => __E_cli_command_128(264).
	p(NT(263), (NT(264)));
//G436: cli_command(230)     => solve_cmd(263).
	p(NT(230), (NT(263)));
//G437: lgrs_sym(271)        => 'l' 'g' 'r' 's'.
	p(NT(271), (T(10)+T(57)+T(19)+T(15)));
//G438: __E_cli_command_130(270) => lgrs_sym(271) __(35) wff_cmd_arg(268).
	p(NT(270), (NT(271)+NT(35)+NT(268)));
//G439: lgrs_cmd(269)        => __E_cli_command_130(270).
	p(NT(269), (NT(270)));
//G440: cli_command(230)     => lgrs_cmd(269).
	p(NT(230), (NT(269)));
//G441: __E___E_cli_command_131_132(275) => 'r'.
	p(NT(275), (T(19)));
//G442: __E___E_cli_command_131_132(275) => 'r' 'u' 'n'.
	p(NT(275), (T(19)+T(7)+T(5)));
//G443: run_sym(274)         => __E___E_cli_command_131_132(275).
	p(NT(274), (NT(275)));
//G444: __E_cli_command_131(273) => run_sym(274) __(35) spec_arg(256).
	p(NT(273), (NT(274)+NT(35)+NT(256)));
//G445: run_cmd(272)         => __E_cli_command_131(273).
	p(NT(272), (NT(273)));
//G446: cli_command(230)     => run_cmd(272).
	p(NT(230), (NT(272)));
//G447: __E___E_cli_command_133_134(279) => 'n'.
	p(NT(279), (T(5)));
//G448: __E___E_cli_command_133_134(279) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(279), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(58)+T(11)));
//G449: normalize_sym(278)   => __E___E_cli_command_133_134(279).
	p(NT(278), (NT(279)));
//G450: __E_cli_command_133(277) => normalize_sym(278) __(35) spec_arg(256).
	p(NT(277), (NT(278)+NT(35)+NT(256)));
//G451: normalize_cmd(276)   => __E_cli_command_133(277).
	p(NT(276), (NT(277)));
//G452: cli_command(230)     => normalize_cmd(276).
	p(NT(230), (NT(276)));
//G453: __E___E_cli_command_135_136(283) => 's'.
	p(NT(283), (T(15)));
//G454: __E___E_cli_command_135_136(283) => 's' 'u' 'b' 's' 't'.
	p(NT(283), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G455: __E___E_cli_command_135_136(283) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(283), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G456: subst_sym(282)       => __E___E_cli_command_135_136(283).
	p(NT(282), (NT(283)));
//G457: __E_cli_command_135(281) => subst_sym(282) __(35) nf_cmd_arg(284) _(13) '[' _(13) nf_cmd_arg(284) _(13) '/' _(13) nf_cmd_arg(284) _(13) ']'.
	p(NT(281), (NT(282)+NT(35)+NT(284)+NT(13)+T(26)+NT(13)+NT(284)+NT(13)+T(40)+NT(13)+NT(284)+NT(13)+T(27)));
//G458: subst_cmd(280)       => __E_cli_command_135(281).
	p(NT(280), (NT(281)));
//G459: cli_command(230)     => subst_cmd(280).
	p(NT(230), (NT(280)));
//G460: __E___E_cli_command_137_138(288) => 'i'.
	p(NT(288), (T(4)));
//G461: __E___E_cli_command_137_138(288) => 'i' 'n' 's' 't'.
	p(NT(288), (T(4)+T(5)+T(15)+T(8)));
//G462: __E___E_cli_command_137_138(288) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(288), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G463: inst_sym(287)        => __E___E_cli_command_137_138(288).
	p(NT(287), (NT(288)));
//G464: __E_cli_command_137(286) => inst_sym(287) __(35) inst_args(289).
	p(NT(286), (NT(287)+NT(35)+NT(289)));
//G465: inst_cmd(285)        => __E_cli_command_137(286).
	p(NT(285), (NT(286)));
//G466: cli_command(230)     => inst_cmd(285).
	p(NT(230), (NT(285)));
//G467: dnf_sym(292)         => 'd' 'n' 'f'.
	p(NT(292), (T(56)+T(5)+T(9)));
//G468: __E_cli_command_139(291) => dnf_sym(292) __(35) nf_cmd_arg(284).
	p(NT(291), (NT(292)+NT(35)+NT(284)));
//G469: dnf_cmd(290)         => __E_cli_command_139(291).
	p(NT(290), (NT(291)));
//G470: cli_command(230)     => dnf_cmd(290).
	p(NT(230), (NT(290)));
//G471: cnf_sym(295)         => 'c' 'n' 'f'.
	p(NT(295), (T(14)+T(5)+T(9)));
//G472: __E_cli_command_140(294) => cnf_sym(295) __(35) nf_cmd_arg(284).
	p(NT(294), (NT(295)+NT(35)+NT(284)));
//G473: cnf_cmd(293)         => __E_cli_command_140(294).
	p(NT(293), (NT(294)));
//G474: cli_command(230)     => cnf_cmd(293).
	p(NT(230), (NT(293)));
//G475: anf_sym(298)         => 'a' 'n' 'f'.
	p(NT(298), (T(16)+T(5)+T(9)));
//G476: __E_cli_command_141(297) => anf_sym(298) __(35) nf_cmd_arg(284).
	p(NT(297), (NT(298)+NT(35)+NT(284)));
//G477: anf_cmd(296)         => __E_cli_command_141(297).
	p(NT(296), (NT(297)));
//G478: cli_command(230)     => anf_cmd(296).
	p(NT(230), (NT(296)));
//G479: nnf_sym(301)         => 'n' 'n' 'f'.
	p(NT(301), (T(5)+T(5)+T(9)));
//G480: __E_cli_command_142(300) => nnf_sym(301) __(35) nf_cmd_arg(284).
	p(NT(300), (NT(301)+NT(35)+NT(284)));
//G481: nnf_cmd(299)         => __E_cli_command_142(300).
	p(NT(299), (NT(300)));
//G482: cli_command(230)     => nnf_cmd(299).
	p(NT(230), (NT(299)));
//G483: pnf_sym(304)         => 'p' 'n' 'f'.
	p(NT(304), (T(55)+T(5)+T(9)));
//G484: __E_cli_command_143(303) => pnf_sym(304) __(35) nf_cmd_arg(284).
	p(NT(303), (NT(304)+NT(35)+NT(284)));
//G485: pnf_cmd(302)         => __E_cli_command_143(303).
	p(NT(302), (NT(303)));
//G486: cli_command(230)     => pnf_cmd(302).
	p(NT(230), (NT(302)));
//G487: mnf_sym(307)         => 'm' 'n' 'f'.
	p(NT(307), (T(21)+T(5)+T(9)));
//G488: __E_cli_command_144(306) => mnf_sym(307) __(35) nf_cmd_arg(284).
	p(NT(306), (NT(307)+NT(35)+NT(284)));
//G489: mnf_cmd(305)         => __E_cli_command_144(306).
	p(NT(305), (NT(306)));
//G490: cli_command(230)     => mnf_cmd(305).
	p(NT(230), (NT(305)));
//G491: onf_sym(310)         => 'o' 'n' 'f'.
	p(NT(310), (T(6)+T(5)+T(9)));
//G492: __E_cli_command_145(309) => onf_sym(310) __(35) variable(112) __(35) onf_cmd_arg(311).
	p(NT(309), (NT(310)+NT(35)+NT(112)+NT(35)+NT(311)));
//G493: onf_cmd(308)         => __E_cli_command_145(309).
	p(NT(308), (NT(309)));
//G494: cli_command(230)     => onf_cmd(308).
	p(NT(230), (NT(308)));
//G495: qelim_sym(314)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(314), (T(53)+T(11)+T(10)+T(4)+T(21)));
//G496: __E_cli_command_146(313) => qelim_sym(314) __(35) wff_cmd_arg(268).
	p(NT(313), (NT(314)+NT(35)+NT(268)));
//G497: qelim_cmd(312)       => __E_cli_command_146(313).
	p(NT(312), (NT(313)));
//G498: cli_command(230)     => qelim_cmd(312).
	p(NT(230), (NT(312)));
//G499: get_sym(317)         => 'g' 'e' 't'.
	p(NT(317), (T(57)+T(11)+T(8)));
//G500: __E___E_cli_command_147_148(318) => __(35) option_name(319).
	p(NT(318), (NT(35)+NT(319)));
//G501: __E___E_cli_command_147_148(318) => null.
	p(NT(318), (nul));
//G502: __E_cli_command_147(316) => get_sym(317) __E___E_cli_command_147_148(318).
	p(NT(316), (NT(317)+NT(318)));
//G503: get_cmd(315)         => __E_cli_command_147(316).
	p(NT(315), (NT(316)));
//G504: cli_command(230)     => get_cmd(315).
	p(NT(230), (NT(315)));
//G505: set_sym(322)         => 's' 'e' 't'.
	p(NT(322), (T(15)+T(11)+T(8)));
//G506: __E___E_cli_command_149_150(323) => __(35).
	p(NT(323), (NT(35)));
//G507: __E___E_cli_command_149_150(323) => _(13) '=' _(13).
	p(NT(323), (NT(13)+T(3)+NT(13)));
//G508: __E_cli_command_149(321) => set_sym(322) __(35) option_name(319) __E___E_cli_command_149_150(323) option_value(324).
	p(NT(321), (NT(322)+NT(35)+NT(319)+NT(323)+NT(324)));
//G509: set_cmd(320)         => __E_cli_command_149(321).
	p(NT(320), (NT(321)));
//G510: cli_command(230)     => set_cmd(320).
	p(NT(230), (NT(320)));
//G511: enable_sym(327)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(327), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G512: __E_cli_command_151(326) => enable_sym(327) __(35) option_name(319).
	p(NT(326), (NT(327)+NT(35)+NT(319)));
//G513: enable_cmd(325)      => __E_cli_command_151(326).
	p(NT(325), (NT(326)));
//G514: cli_command(230)     => enable_cmd(325).
	p(NT(230), (NT(325)));
//G515: disable_sym(330)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(330), (T(56)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G516: __E_cli_command_152(329) => disable_sym(330) __(35) option_name(319).
	p(NT(329), (NT(330)+NT(35)+NT(319)));
//G517: disable_cmd(328)     => __E_cli_command_152(329).
	p(NT(328), (NT(329)));
//G518: cli_command(230)     => disable_cmd(328).
	p(NT(230), (NT(328)));
//G519: toggle_sym(333)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(333), (T(8)+T(6)+T(57)+T(57)+T(10)+T(11)));
//G520: __E_cli_command_153(332) => toggle_sym(333) __(35) option_name(319).
	p(NT(332), (NT(333)+NT(35)+NT(319)));
//G521: toggle_cmd(331)      => __E_cli_command_153(332).
	p(NT(331), (NT(332)));
//G522: cli_command(230)     => toggle_cmd(331).
	p(NT(230), (NT(331)));
//G523: __E___E_cli_command_154_155(337) => 'd' 'e' 'f' 's'.
	p(NT(337), (T(56)+T(11)+T(9)+T(15)));
//G524: __E___E_cli_command_154_155(337) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(337), (T(56)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G525: def_sym(336)         => __E___E_cli_command_154_155(337).
	p(NT(336), (NT(337)));
//G526: __E_cli_command_154(335) => def_sym(336).
	p(NT(335), (NT(336)));
//G527: def_list_cmd(334)    => __E_cli_command_154(335).
	p(NT(334), (NT(335)));
//G528: cli_command(230)     => def_list_cmd(334).
	p(NT(230), (NT(334)));
//G529: __E_cli_command_156(339) => def_sym(336) __(35) num(172).
	p(NT(339), (NT(336)+NT(35)+NT(172)));
//G530: def_print_cmd(338)   => __E_cli_command_156(339).
	p(NT(338), (NT(339)));
//G531: cli_command(230)     => def_print_cmd(338).
	p(NT(230), (NT(338)));
//G532: def_rr_cmd(340)      => rec_relation(19).
	p(NT(340), (NT(19)));
//G533: cli_command(230)     => def_rr_cmd(340).
	p(NT(230), (NT(340)));
//G534: def_input_cmd(341)   => input_def(20).
	p(NT(341), (NT(20)));
//G535: cli_command(230)     => def_input_cmd(341).
	p(NT(230), (NT(341)));
//G536: def_output_cmd(342)  => output_def(21).
	p(NT(342), (NT(21)));
//G537: cli_command(230)     => def_output_cmd(342).
	p(NT(230), (NT(342)));
//G538: __E___E_cli_command_157_158(346) => 'h' 'i' 's' 't'.
	p(NT(346), (T(54)+T(4)+T(15)+T(8)));
//G539: __E___E_cli_command_157_158(346) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(346), (T(54)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G540: history_sym(345)     => __E___E_cli_command_157_158(346).
	p(NT(345), (NT(346)));
//G541: __E_cli_command_157(344) => history_sym(345).
	p(NT(344), (NT(345)));
//G542: history_list_cmd(343) => __E_cli_command_157(344).
	p(NT(343), (NT(344)));
//G543: cli_command(230)     => history_list_cmd(343).
	p(NT(230), (NT(343)));
//G544: __E_cli_command_159(348) => history_sym(345) __(35) history(349).
	p(NT(348), (NT(345)+NT(35)+NT(349)));
//G545: history_print_cmd(347) => __E_cli_command_159(348).
	p(NT(347), (NT(348)));
//G546: cli_command(230)     => history_print_cmd(347).
	p(NT(230), (NT(347)));
//G547: __E_cli_command_160(351) => ref(28).
	p(NT(351), (NT(28)));
//G548: __E_cli_command_160(351) => wff(15).
	p(NT(351), (NT(15)));
//G549: __E_cli_command_160(351) => bf(31).
	p(NT(351), (NT(31)));
//G550: history_store_cmd(350) => __E_cli_command_160(351).
	p(NT(350), (NT(351)));
//G551: cli_command(230)     => history_store_cmd(350).
	p(NT(230), (NT(350)));
//G552: __E_solve_options_161(352) => __(35) solve_option(353).
	p(NT(352), (NT(35)+NT(353)));
//G553: __E_solve_options_162(354) => null.
	p(NT(354), (nul));
//G554: __E_solve_options_162(354) => __E_solve_options_161(352) __E_solve_options_162(354).
	p(NT(354), (NT(352)+NT(354)));
//G555: solve_options(267)   => __E_solve_options_162(354).
	p(NT(267), (NT(354)));
//G556: __E_solve_option_163(355) => solver_mode(356).
	p(NT(355), (NT(356)));
//G557: __E_solve_option_163(355) => type(222).
	p(NT(355), (NT(222)));
//G558: solve_option(353)    => '-' '-' __E_solve_option_163(355).
	p(NT(353), (T(30)+T(30)+NT(355)));
//G559: __E_solver_mode_164(358) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(358), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G560: __E_solver_mode_164(358) => 'm' 'i' 'n'.
	p(NT(358), (T(21)+T(4)+T(5)));
//G561: solver_mode_minimum(357) => __E_solver_mode_164(358).
	p(NT(357), (NT(358)));
//G562: solver_mode(356)     => solver_mode_minimum(357).
	p(NT(356), (NT(357)));
//G563: __E_solver_mode_165(360) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(360), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G564: __E_solver_mode_165(360) => 'm' 'a' 'x'.
	p(NT(360), (T(21)+T(16)+T(29)));
//G565: solver_mode_maximum(359) => __E_solver_mode_165(360).
	p(NT(359), (NT(360)));
//G566: solver_mode(356)     => solver_mode_maximum(359).
	p(NT(356), (NT(359)));
//G567: q_file_name(38)      => '"' file_name(361) '"'.
	p(NT(38), (T(59)+NT(361)+T(59)));
//G568: __E_file_name_166(362) => printable(8).
	p(NT(362), (NT(8)));
//G569: __E_file_name_166(362) => printable(8) __E_file_name_166(362).
	p(NT(362), (NT(8)+NT(362)));
//G570: file_name(361)       => __E_file_name_166(362).
	p(NT(361), (NT(362)));
//G571: __E_option_name_167(363) => alnum(6).
	p(NT(363), (NT(6)));
//G572: __E_option_name_167(363) => alnum(6) __E_option_name_167(363).
	p(NT(363), (NT(6)+NT(363)));
//G573: option_name(319)     => __E_option_name_167(363).
	p(NT(319), (NT(363)));
//G574: __E_option_value_168(364) => alnum(6).
	p(NT(364), (NT(6)));
//G575: __E_option_value_168(364) => alnum(6) __E_option_value_168(364).
	p(NT(364), (NT(6)+NT(364)));
//G576: option_value(324)    => __E_option_value_168(364).
	p(NT(324), (NT(364)));
//G577: bf_cmd_arg(365)      => history(349).
	p(NT(365), (NT(349)));
//G578: bf_cmd_arg(365)      => bf(31).
	p(NT(365), (NT(31)));
//G579: wff_cmd_arg(268)     => history(349).
	p(NT(268), (NT(349)));
//G580: wff_cmd_arg(268)     => wff(15).
	p(NT(268), (NT(15)));
//G581: nf_cmd_arg(284)      => history(349).
	p(NT(284), (NT(349)));
//G582: nf_cmd_arg(284)      => ref(28).
	p(NT(284), (NT(28)));
//G583: nf_cmd_arg(284)      => wff(15).
	p(NT(284), (NT(15)));
//G584: nf_cmd_arg(284)      => bf(31).
	p(NT(284), (NT(31)));
//G585: onf_cmd_arg(311)     => history(349).
	p(NT(311), (NT(349)));
//G586: onf_cmd_arg(311)     => wff(15).
	p(NT(311), (NT(15)));
//G587: spec_arg(256)        => history(349).
	p(NT(256), (NT(349)));
//G588: spec_arg(256)        => spec(10).
	p(NT(256), (NT(10)));
//G589: spec_arg(256)        => ref(28).
	p(NT(256), (NT(28)));
//G590: spec_arg(256)        => wff(15).
	p(NT(256), (NT(15)));
//G591: spec_arg(256)        => bf(31).
	p(NT(256), (NT(31)));
//G592: inst_args(289)       => wff_cmd_arg(268) _(13) '[' _(13) bf(31) _(13) '/' _(13) bf_cmd_arg(365) _(13) ']'.
	p(NT(289), (NT(268)+NT(13)+T(26)+NT(13)+NT(31)+NT(13)+T(40)+NT(13)+NT(365)+NT(13)+T(27)));
//G593: inst_args(289)       => bf_cmd_arg(365) _(13) '[' _(13) bf(31) _(13) '/' _(13) bf_cmd_arg(365) _(13) ']'.
	p(NT(289), (NT(365)+NT(13)+T(26)+NT(13)+NT(31)+NT(13)+T(40)+NT(13)+NT(365)+NT(13)+T(27)));
//G594: help_arg(248)        => help_sym(245).
	p(NT(248), (NT(245)));
//G595: help_arg(248)        => version_sym(238).
	p(NT(248), (NT(238)));
//G596: help_arg(248)        => quit_sym(235).
	p(NT(248), (NT(235)));
//G597: help_arg(248)        => clear_sym(241).
	p(NT(248), (NT(241)));
//G598: help_arg(248)        => get_sym(317).
	p(NT(248), (NT(317)));
//G599: help_arg(248)        => set_sym(322).
	p(NT(248), (NT(322)));
//G600: help_arg(248)        => enable_sym(327).
	p(NT(248), (NT(327)));
//G601: help_arg(248)        => disable_sym(330).
	p(NT(248), (NT(330)));
//G602: help_arg(248)        => toggle_sym(333).
	p(NT(248), (NT(333)));
//G603: help_arg(248)        => file_sym(251).
	p(NT(248), (NT(251)));
//G604: help_arg(248)        => history_sym(345).
	p(NT(248), (NT(345)));
//G605: help_arg(248)        => abs_history_sym(366).
	p(NT(248), (NT(366)));
//G606: help_arg(248)        => rel_history_sym(367).
	p(NT(248), (NT(367)));
//G607: help_arg(248)        => selection_sym(368).
	p(NT(248), (NT(368)));
//G608: help_arg(248)        => def_sym(336).
	p(NT(248), (NT(336)));
//G609: help_arg(248)        => inst_sym(287).
	p(NT(248), (NT(287)));
//G610: help_arg(248)        => subst_sym(282).
	p(NT(248), (NT(282)));
//G611: help_arg(248)        => normalize_sym(278).
	p(NT(248), (NT(278)));
//G612: help_arg(248)        => execute_sym(369).
	p(NT(248), (NT(369)));
//G613: help_arg(248)        => solve_sym(265).
	p(NT(248), (NT(265)));
//G614: help_arg(248)        => valid_sym(255).
	p(NT(248), (NT(255)));
//G615: help_arg(248)        => sat_sym(259).
	p(NT(248), (NT(259)));
//G616: help_arg(248)        => unsat_sym(262).
	p(NT(248), (NT(262)));
//G617: help_arg(248)        => run_sym(274).
	p(NT(248), (NT(274)));
//G618: help_arg(248)        => dnf_sym(292).
	p(NT(248), (NT(292)));
//G619: help_arg(248)        => cnf_sym(295).
	p(NT(248), (NT(295)));
//G620: help_arg(248)        => anf_sym(298).
	p(NT(248), (NT(298)));
//G621: help_arg(248)        => snf_sym(370).
	p(NT(248), (NT(370)));
//G622: help_arg(248)        => nnf_sym(301).
	p(NT(248), (NT(301)));
//G623: help_arg(248)        => pnf_sym(304).
	p(NT(248), (NT(304)));
//G624: help_arg(248)        => mnf_sym(307).
	p(NT(248), (NT(307)));
//G625: help_arg(248)        => onf_sym(310).
	p(NT(248), (NT(310)));
//G626: help_arg(248)        => qelim_sym(314).
	p(NT(248), (NT(314)));
//G627: __E___E_help_arg_169_170(373) => 's'.
	p(NT(373), (T(15)));
//G628: __E___E_help_arg_169_170(373) => null.
	p(NT(373), (nul));
//G629: __E_help_arg_169(372) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_169_170(373).
	p(NT(372), (T(11)+T(29)+T(16)+T(21)+T(55)+T(10)+T(11)+NT(373)));
//G630: examples_sym(371)    => __E_help_arg_169(372).
	p(NT(371), (NT(372)));
//G631: help_arg(248)        => examples_sym(371).
	p(NT(248), (NT(371)));
//G632: __E_history_171(375) => '%'.
	p(NT(375), (T(38)));
//G633: rel_history_sym(367) => '%' '-'.
	p(NT(367), (T(38)+T(30)));
//G634: history_id(377)      => digits(219).
	p(NT(377), (NT(219)));
//G635: __E___E_history_171_172(376) => history_id(377).
	p(NT(376), (NT(377)));
//G636: __E___E_history_171_172(376) => null.
	p(NT(376), (nul));
//G637: __E_history_171(375) => rel_history_sym(367) __E___E_history_171_172(376).
	p(NT(375), (NT(367)+NT(376)));
//G638: rel_history(374)     => __E_history_171(375).
	p(NT(374), (NT(375)));
//G639: history(349)         => rel_history(374).
	p(NT(349), (NT(374)));
//G640: abs_history_sym(366) => '%'.
	p(NT(366), (T(38)));
//G641: __E_history_173(379) => abs_history_sym(366) history_id(377).
	p(NT(379), (NT(366)+NT(377)));
//G642: abs_history(378)     => __E_history_173(379).
	p(NT(378), (NT(379)));
//G643: history(349)         => abs_history(378).
	p(NT(349), (NT(378)));
//G644: stream_def(380)      => input_def(20).
	p(NT(380), (NT(20)));
//G645: stream_def(380)      => output_def(21).
	p(NT(380), (NT(21)));
//G646: fm_or_term(381)      => wff(15).
	p(NT(381), (NT(15)));
//G647: fm_or_term(381)      => bf(31).
	p(NT(381), (NT(31)));
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
		__E_bf_44, ba_constant, variable, bf_fall, __E_bf_45, bf_fex, __E_bf_46, bf_ref, bf_shr, __E_bf_47, 
		__E___E_bf_47_48, bf_shl, __E_bf_49, __E___E_bf_49_50, bf_add, __E_bf_51, bf_sub, __E_bf_52, __E___E_bf_52_53, bf_mod, 
		__E_bf_54, __E___E_bf_54_55, bf_mul, __E_bf_56, bf_div, __E_bf_57, __E___E_bf_57_58, bf_nor, __E_bf_59, __E___E_bf_59_60, 
		bf_xnor, __E_bf_61, __E___E_bf_61_62, bf_nand, __E_bf_63, __E___E_bf_63_64, bf_or, __E_bf_65, bf_xor, __E_bf_66, 
		bf_t, __E_bf_67, __E___E_bf_67_68, bf_f, __E_bf_69, __E___E_bf_69_70, bf_and, __E_bf_71, __E___E_bf_71_72, bf_neg, 
		__E_bf_73, bf_neg_oprnd, __E___E_bf_73_74, bf_and_nosep, __E_bf_75, bf_and_nosep_1st_oprnd, __E___E_bf_75_76, bf_and_nosep_2nd_oprnd, __E___E_bf_75_77, ctn_neq, 
		__E_constraint_78, ctnvar, num, ctn_eq, __E_constraint_79, ctn_gteq, __E_constraint_80, ctn_gt, __E_constraint_81, ctn_lteq, 
		__E_constraint_82, ctn_lt, __E_constraint_83, __E_ba_constant_84, source, __E_ba_constant_85, src_c, __E_source_86, __E___E_source_86_87, __E___E_source_86_88, 
		__E_src_c_89, __E_src_c_90, __E_variable_91, uconst, io_var, var_name, __E_variable_92, __E_var_name_93, __E_var_name_94, __E_var_name_95, 
		__E_var_name_96, __E_var_name_97, offset, chars, uconst_name, __E_uconst_98, __E___E_uconst_98_99, q_var, __E_q_vars_100, __E_q_vars_101, 
		__E_offsets_102, __E_offsets_103, integer, shift, __E_offset_104, __E_shift_105, __E___E_shift_105_106, __E_chars_107, __E_chars_108, digits, 
		__E_digits_109, __E_integer_110, type, subtype, __E_typed_111, comment, __E_comment_112, __E_comment_113, __E_comment_114, cli, 
		cli_command, __E_cli_115, __E_cli_116, __E_cli_117, quit_cmd, quit_sym, __E_cli_command_118, version_cmd, version_sym, __E_cli_command_119, 
		clear_cmd, clear_sym, __E_cli_command_120, help_cmd, __E_cli_command_121, help_sym, __E___E_cli_command_121_122, __E___E_cli_command_121_123, help_arg, file_cmd, 
		__E_cli_command_124, file_sym, q_string, valid_cmd, __E_cli_command_125, valid_sym, spec_arg, sat_cmd, __E_cli_command_126, sat_sym, 
		unsat_cmd, __E_cli_command_127, unsat_sym, solve_cmd, __E_cli_command_128, solve_sym, __E___E_cli_command_128_129, solve_options, wff_cmd_arg, lgrs_cmd, 
		__E_cli_command_130, lgrs_sym, run_cmd, __E_cli_command_131, run_sym, __E___E_cli_command_131_132, normalize_cmd, __E_cli_command_133, normalize_sym, __E___E_cli_command_133_134, 
		subst_cmd, __E_cli_command_135, subst_sym, __E___E_cli_command_135_136, nf_cmd_arg, inst_cmd, __E_cli_command_137, inst_sym, __E___E_cli_command_137_138, inst_args, 
		dnf_cmd, __E_cli_command_139, dnf_sym, cnf_cmd, __E_cli_command_140, cnf_sym, anf_cmd, __E_cli_command_141, anf_sym, nnf_cmd, 
		__E_cli_command_142, nnf_sym, pnf_cmd, __E_cli_command_143, pnf_sym, mnf_cmd, __E_cli_command_144, mnf_sym, onf_cmd, __E_cli_command_145, 
		onf_sym, onf_cmd_arg, qelim_cmd, __E_cli_command_146, qelim_sym, get_cmd, __E_cli_command_147, get_sym, __E___E_cli_command_147_148, option_name, 
		set_cmd, __E_cli_command_149, set_sym, __E___E_cli_command_149_150, option_value, enable_cmd, __E_cli_command_151, enable_sym, disable_cmd, __E_cli_command_152, 
		disable_sym, toggle_cmd, __E_cli_command_153, toggle_sym, def_list_cmd, __E_cli_command_154, def_sym, __E___E_cli_command_154_155, def_print_cmd, __E_cli_command_156, 
		def_rr_cmd, def_input_cmd, def_output_cmd, history_list_cmd, __E_cli_command_157, history_sym, __E___E_cli_command_157_158, history_print_cmd, __E_cli_command_159, history, 
		history_store_cmd, __E_cli_command_160, __E_solve_options_161, solve_option, __E_solve_options_162, __E_solve_option_163, solver_mode, solver_mode_minimum, __E_solver_mode_164, solver_mode_maximum, 
		__E_solver_mode_165, file_name, __E_file_name_166, __E_option_name_167, __E_option_value_168, bf_cmd_arg, abs_history_sym, rel_history_sym, selection_sym, execute_sym, 
		snf_sym, examples_sym, __E_help_arg_169, __E___E_help_arg_169_170, rel_history, __E_history_171, __E___E_history_171_172, history_id, abs_history, __E_history_173, 
		stream_def, fm_or_term, __N_0, __N_1, __N_2, __N_3, __N_4, __N_5, __N_6, __N_7, 
		__N_8, __N_9, __N_10, __N_11, __N_12, __N_13, __N_14, __N_15, __N_16, __N_17, 
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
