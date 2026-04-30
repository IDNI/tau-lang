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
	"num", "bf_shr", "__E_bf_48", "__E___E_bf_48_49", "bf_shl", "__E_bf_50", "__E___E_bf_50_51", "bf_add", "__E_bf_52", "bf_sub", 
	"__E_bf_53", "__E___E_bf_53_54", "bf_mod", "__E_bf_55", "__E___E_bf_55_56", "bf_mul", "__E_bf_57", "bf_div", "__E_bf_58", "__E___E_bf_58_59", 
	"bf_nor", "__E_bf_60", "__E___E_bf_60_61", "bf_xnor", "__E_bf_62", "__E___E_bf_62_63", "bf_nand", "__E_bf_64", "__E___E_bf_64_65", "bf_or", 
	"__E_bf_66", "bf_xor", "__E_bf_67", "bf_t", "__E_bf_68", "__E___E_bf_68_69", "bf_f", "__E_bf_70", "__E___E_bf_70_71", "bf_and", 
	"__E_bf_72", "__E___E_bf_72_73", "bf_neg", "__E_bf_74", "bf_neg_oprnd", "__E___E_bf_74_75", "bf_and_nosep", "__E_bf_76", "bf_and_nosep_1st_oprnd", "__E___E_bf_76_77", 
	"bf_and_nosep_2nd_oprnd", "__E___E_bf_76_78", "ctn_neq", "__E_constraint_79", "ctnvar", "ctn_eq", "__E_constraint_80", "ctn_gteq", "__E_constraint_81", "ctn_gt", 
	"__E_constraint_82", "ctn_lteq", "__E_constraint_83", "ctn_lt", "__E_constraint_84", "__E_ba_constant_85", "source", "__E_ba_constant_86", "src_c", "__E_source_87", 
	"__E___E_source_87_88", "__E___E_source_87_89", "__E_src_c_90", "__E_src_c_91", "__E_variable_92", "uconst", "io_var", "var_name", "__E_variable_93", "__E_var_name_94", 
	"__E_var_name_95", "__E_var_name_96", "__E_var_name_97", "__E_var_name_98", "offset", "chars", "uconst_name", "__E_uconst_99", "__E___E_uconst_99_100", "q_var", 
	"__E_q_vars_101", "__E_q_vars_102", "__E_offsets_103", "__E_offsets_104", "integer", "shift", "__E_offset_105", "__E_shift_106", "__E___E_shift_106_107", "__E_chars_108", 
	"__E_chars_109", "digits", "__E_digits_110", "__E_integer_111", "type", "subtype", "__E_typed_112", "comment", "__E_comment_113", "__E_comment_114", 
	"__E_comment_115", "cli", "cli_command", "__E_cli_116", "__E_cli_117", "__E_cli_118", "quit_cmd", "quit_sym", "__E_cli_command_119", "version_cmd", 
	"version_sym", "__E_cli_command_120", "clear_cmd", "clear_sym", "__E_cli_command_121", "help_cmd", "__E_cli_command_122", "help_sym", "__E___E_cli_command_122_123", "__E___E_cli_command_122_124", 
	"help_arg", "file_cmd", "__E_cli_command_125", "file_sym", "q_string", "valid_cmd", "__E_cli_command_126", "valid_sym", "spec_arg", "sat_cmd", 
	"__E_cli_command_127", "sat_sym", "unsat_cmd", "__E_cli_command_128", "unsat_sym", "solve_cmd", "__E_cli_command_129", "solve_sym", "__E___E_cli_command_129_130", "solve_options", 
	"wff_cmd_arg", "lgrs_cmd", "__E_cli_command_131", "lgrs_sym", "run_cmd", "__E_cli_command_132", "run_sym", "__E___E_cli_command_132_133", "normalize_cmd", "__E_cli_command_134", 
	"normalize_sym", "__E___E_cli_command_134_135", "subst_cmd", "__E_cli_command_136", "subst_sym", "__E___E_cli_command_136_137", "nf_cmd_arg", "inst_cmd", "__E_cli_command_138", "inst_sym", 
	"__E___E_cli_command_138_139", "inst_args", "dnf_cmd", "__E_cli_command_140", "dnf_sym", "cnf_cmd", "__E_cli_command_141", "cnf_sym", "anf_cmd", "__E_cli_command_142", 
	"anf_sym", "nnf_cmd", "__E_cli_command_143", "nnf_sym", "pnf_cmd", "__E_cli_command_144", "pnf_sym", "mnf_cmd", "__E_cli_command_145", "mnf_sym", 
	"onf_cmd", "__E_cli_command_146", "onf_sym", "onf_cmd_arg", "qelim_cmd", "__E_cli_command_147", "qelim_sym", "get_cmd", "__E_cli_command_148", "get_sym", 
	"__E___E_cli_command_148_149", "option_name", "set_cmd", "__E_cli_command_150", "set_sym", "__E___E_cli_command_150_151", "option_value", "enable_cmd", "__E_cli_command_152", "enable_sym", 
	"disable_cmd", "__E_cli_command_153", "disable_sym", "toggle_cmd", "__E_cli_command_154", "toggle_sym", "def_list_cmd", "__E_cli_command_155", "def_sym", "__E___E_cli_command_155_156", 
	"def_print_cmd", "__E_cli_command_157", "def_rr_cmd", "def_input_cmd", "def_output_cmd", "history_list_cmd", "__E_cli_command_158", "history_sym", "__E___E_cli_command_158_159", "history_print_cmd", 
	"__E_cli_command_160", "history", "history_store_cmd", "__E_cli_command_161", "__E_solve_options_162", "solve_option", "__E_solve_options_163", "__E_solve_option_164", "solver_mode", "solver_mode_minimum", 
	"__E_solver_mode_165", "solver_mode_maximum", "__E_solver_mode_166", "file_name", "__E_file_name_167", "__E_option_name_168", "__E_option_value_169", "bf_cmd_arg", "abs_history_sym", "rel_history_sym", 
	"selection_sym", "execute_sym", "snf_sym", "examples_sym", "__E_help_arg_170", "__E___E_help_arg_170_171", "rel_history", "__E_history_172", "__E___E_history_172_173", "history_id", 
	"abs_history", "__E_history_174", "stream_def", "fm_or_term", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", 
	"__N_6", "__N_7", "__N_8", "__N_9", "__N_10", "__N_11", "__N_12", "__N_13", "__N_14", "__N_15", 
	"__N_16", "__N_17", 
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
			30, 32, 40, 174, 186, 197, 206, 214, 221, 224,
			225, 321, 326, 363
		},
		.to_inline = {
			{ 15, 54, 15 },
			{ 25 },
			{ 31, 109, 31 },
			{ 164, 109, 31 },
			{ 166, 109, 159 },
			{ 168, 109, 31 },
			{ 170, 109, 31 },
			{ 188 },
			{ 195 },
			{ 195, 206 },
			{ 205 },
			{ 209 },
			{ 258 },
			{ 270 },
			{ 286 },
			{ 291 },
			{ 313 },
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
//G159: __E_bf_47(119)       => '(' _(13) 'b' 'v' _(13) '[' _(13) num(120) _(13) ']' _(13) ')' _(13) bf(31).
	p(NT(119), (T(12)+NT(13)+T(17)+T(37)+NT(13)+T(26)+NT(13)+NT(120)+NT(13)+T(27)+NT(13)+T(13)+NT(13)+NT(31)));
//G160: bf_cast(118)         => __E_bf_47(119).
	p(NT(118), (NT(119)));
//G161: bf(31)               => bf_cast(118).
	p(NT(31), (NT(118)));
//G162: __N_0(384)           => bf_shr(121).
	p(NT(384), (NT(121)));
//G163: __E___E_bf_48_49(123) => bf(31) & ~( __N_0(384) ).	 # conjunctive
	p(NT(123), (NT(31)) & ~(NT(384)));
//G164: __E_bf_48(122)       => bf(31) _(13) '>' '>' _(13) __E___E_bf_48_49(123).
	p(NT(122), (NT(31)+NT(13)+T(23)+T(23)+NT(13)+NT(123)));
//G165: bf_shr(121)          => __E_bf_48(122).
	p(NT(121), (NT(122)));
//G166: bf(31)               => bf_shr(121).
	p(NT(31), (NT(121)));
//G167: __N_1(385)           => bf_shl(124).
	p(NT(385), (NT(124)));
//G168: __E___E_bf_50_51(126) => bf(31) & ~( __N_1(385) ).	 # conjunctive
	p(NT(126), (NT(31)) & ~(NT(385)));
//G169: __E_bf_50(125)       => bf(31) _(13) '<' '<' _(13) __E___E_bf_50_51(126).
	p(NT(125), (NT(31)+NT(13)+T(22)+T(22)+NT(13)+NT(126)));
//G170: bf_shl(124)          => __E_bf_50(125).
	p(NT(124), (NT(125)));
//G171: bf(31)               => bf_shl(124).
	p(NT(31), (NT(124)));
//G172: __E_bf_52(128)       => bf(31) _(13) '+' _(13) bf(31).
	p(NT(128), (NT(31)+NT(13)+T(38)+NT(13)+NT(31)));
//G173: bf_add(127)          => __E_bf_52(128).
	p(NT(127), (NT(128)));
//G174: bf(31)               => bf_add(127).
	p(NT(31), (NT(127)));
//G175: __N_2(386)           => bf_sub(129).
	p(NT(386), (NT(129)));
//G176: __E___E_bf_53_54(131) => bf(31) & ~( __N_2(386) ).	 # conjunctive
	p(NT(131), (NT(31)) & ~(NT(386)));
//G177: __E_bf_53(130)       => bf(31) _(13) '-' _(13) __E___E_bf_53_54(131).
	p(NT(130), (NT(31)+NT(13)+T(30)+NT(13)+NT(131)));
//G178: bf_sub(129)          => __E_bf_53(130).
	p(NT(129), (NT(130)));
//G179: bf(31)               => bf_sub(129).
	p(NT(31), (NT(129)));
//G180: __N_3(387)           => bf_mod(132).
	p(NT(387), (NT(132)));
//G181: __E___E_bf_55_56(134) => bf(31) & ~( __N_3(387) ).	 # conjunctive
	p(NT(134), (NT(31)) & ~(NT(387)));
//G182: __E_bf_55(133)       => bf(31) _(13) '%' _(13) __E___E_bf_55_56(134).
	p(NT(133), (NT(31)+NT(13)+T(39)+NT(13)+NT(134)));
//G183: bf_mod(132)          => __E_bf_55(133).
	p(NT(132), (NT(133)));
//G184: bf(31)               => bf_mod(132).
	p(NT(31), (NT(132)));
//G185: __E_bf_57(136)       => bf(31) _(13) '*' _(13) bf(31).
	p(NT(136), (NT(31)+NT(13)+T(40)+NT(13)+NT(31)));
//G186: bf_mul(135)          => __E_bf_57(136).
	p(NT(135), (NT(136)));
//G187: bf(31)               => bf_mul(135).
	p(NT(31), (NT(135)));
//G188: __N_4(388)           => bf_div(137).
	p(NT(388), (NT(137)));
//G189: __E___E_bf_58_59(139) => bf(31) & ~( __N_4(388) ).	 # conjunctive
	p(NT(139), (NT(31)) & ~(NT(388)));
//G190: __E_bf_58(138)       => bf(31) _(13) '/' _(13) __E___E_bf_58_59(139).
	p(NT(138), (NT(31)+NT(13)+T(41)+NT(13)+NT(139)));
//G191: bf_div(137)          => __E_bf_58(138).
	p(NT(137), (NT(138)));
//G192: bf(31)               => bf_div(137).
	p(NT(31), (NT(137)));
//G193: __N_5(389)           => bf_nor(140).
	p(NT(389), (NT(140)));
//G194: __E___E_bf_60_61(142) => bf(31) & ~( __N_5(389) ).	 # conjunctive
	p(NT(142), (NT(31)) & ~(NT(389)));
//G195: __E_bf_60(141)       => bf(31) _(13) '!' '|' _(13) __E___E_bf_60_61(142).
	p(NT(141), (NT(31)+NT(13)+T(34)+T(31)+NT(13)+NT(142)));
//G196: bf_nor(140)          => __E_bf_60(141).
	p(NT(140), (NT(141)));
//G197: bf(31)               => bf_nor(140).
	p(NT(31), (NT(140)));
//G198: __N_6(390)           => bf_xnor(143).
	p(NT(390), (NT(143)));
//G199: __E___E_bf_62_63(145) => bf(31) & ~( __N_6(390) ).	 # conjunctive
	p(NT(145), (NT(31)) & ~(NT(390)));
//G200: __E_bf_62(144)       => bf(31) _(13) '!' '^' _(13) __E___E_bf_62_63(145).
	p(NT(144), (NT(31)+NT(13)+T(34)+T(32)+NT(13)+NT(145)));
//G201: bf_xnor(143)         => __E_bf_62(144).
	p(NT(143), (NT(144)));
//G202: bf(31)               => bf_xnor(143).
	p(NT(31), (NT(143)));
//G203: __N_7(391)           => bf_nand(146).
	p(NT(391), (NT(146)));
//G204: __E___E_bf_64_65(148) => bf(31) & ~( __N_7(391) ).	 # conjunctive
	p(NT(148), (NT(31)) & ~(NT(391)));
//G205: __E_bf_64(147)       => bf(31) _(13) '!' '&' _(13) __E___E_bf_64_65(148).
	p(NT(147), (NT(31)+NT(13)+T(34)+T(33)+NT(13)+NT(148)));
//G206: bf_nand(146)         => __E_bf_64(147).
	p(NT(146), (NT(147)));
//G207: bf(31)               => bf_nand(146).
	p(NT(31), (NT(146)));
//G208: __E_bf_66(150)       => bf(31) _(13) '|' _(13) bf(31).
	p(NT(150), (NT(31)+NT(13)+T(31)+NT(13)+NT(31)));
//G209: bf_or(149)           => __E_bf_66(150).
	p(NT(149), (NT(150)));
//G210: bf(31)               => bf_or(149).
	p(NT(31), (NT(149)));
//G211: __E_bf_67(152)       => bf(31) _(13) '^' _(13) bf(31).
	p(NT(152), (NT(31)+NT(13)+T(32)+NT(13)+NT(31)));
//G212: bf_xor(151)          => __E_bf_67(152).
	p(NT(151), (NT(152)));
//G213: bf(31)               => bf_xor(151).
	p(NT(31), (NT(151)));
//G214: __E___E_bf_68_69(155) => typed(34).
	p(NT(155), (NT(34)));
//G215: __E___E_bf_68_69(155) => null.
	p(NT(155), (nul));
//G216: __E_bf_68(154)       => '1' __E___E_bf_68_69(155).
	p(NT(154), (T(42)+NT(155)));
//G217: bf_t(153)            => __E_bf_68(154).
	p(NT(153), (NT(154)));
//G218: bf(31)               => bf_t(153).
	p(NT(31), (NT(153)));
//G219: __E___E_bf_70_71(158) => typed(34).
	p(NT(158), (NT(34)));
//G220: __E___E_bf_70_71(158) => null.
	p(NT(158), (nul));
//G221: __E_bf_70(157)       => '0' __E___E_bf_70_71(158).
	p(NT(157), (T(43)+NT(158)));
//G222: bf_f(156)            => __E_bf_70(157).
	p(NT(156), (NT(157)));
//G223: bf(31)               => bf_f(156).
	p(NT(31), (NT(156)));
//G224: __E___E_bf_72_73(161) => _(13) '&' _(13).
	p(NT(161), (NT(13)+T(33)+NT(13)));
//G225: __E___E_bf_72_73(161) => __(35).
	p(NT(161), (NT(35)));
//G226: __E_bf_72(160)       => bf(31) __E___E_bf_72_73(161) bf(31).
	p(NT(160), (NT(31)+NT(161)+NT(31)));
//G227: bf_and(159)          => __E_bf_72(160).
	p(NT(159), (NT(160)));
//G228: bf(31)               => bf_and(159).
	p(NT(31), (NT(159)));
//G229: __E___E_bf_74_75(165) => bf_parenthesis(109).
	p(NT(165), (NT(109)));
//G230: __E___E_bf_74_75(165) => ba_constant(111).
	p(NT(165), (NT(111)));
//G231: __E___E_bf_74_75(165) => variable(112).
	p(NT(165), (NT(112)));
//G232: __E___E_bf_74_75(165) => bf_ref(117).
	p(NT(165), (NT(117)));
//G233: __E___E_bf_74_75(165) => bf_t(153).
	p(NT(165), (NT(153)));
//G234: __E___E_bf_74_75(165) => bf_f(156).
	p(NT(165), (NT(156)));
//G235: __E___E_bf_74_75(165) => bf_neg(162).
	p(NT(165), (NT(162)));
//G236: __E___E_bf_74_75(165) => capture(30).
	p(NT(165), (NT(30)));
//G237: bf_neg_oprnd(164)    => __E___E_bf_74_75(165).
	p(NT(164), (NT(165)));
//G238: __E_bf_74(163)       => bf_neg_oprnd(164) _(13) '\''.
	p(NT(163), (NT(164)+NT(13)+T(44)));
//G239: bf_neg(162)          => __E_bf_74(163).
	p(NT(162), (NT(163)));
//G240: bf(31)               => bf_neg(162).
	p(NT(31), (NT(162)));
//G241: __E___E_bf_76_77(169) => bf_parenthesis(109).
	p(NT(169), (NT(109)));
//G242: __E___E_bf_76_77(169) => variable(112).
	p(NT(169), (NT(112)));
//G243: __E___E_bf_76_77(169) => bf_ref(117).
	p(NT(169), (NT(117)));
//G244: __E___E_bf_76_77(169) => bf_neg(162).
	p(NT(169), (NT(162)));
//G245: __E___E_bf_76_77(169) => bf_and_nosep(166).
	p(NT(169), (NT(166)));
//G246: __E___E_bf_76_77(169) => capture(30).
	p(NT(169), (NT(30)));
//G247: bf_and_nosep_1st_oprnd(168) => __E___E_bf_76_77(169).
	p(NT(168), (NT(169)));
//G248: __E___E_bf_76_78(171) => bf_parenthesis(109).
	p(NT(171), (NT(109)));
//G249: __E___E_bf_76_78(171) => ba_constant(111).
	p(NT(171), (NT(111)));
//G250: __E___E_bf_76_78(171) => variable(112).
	p(NT(171), (NT(112)));
//G251: __E___E_bf_76_78(171) => bf_ref(117).
	p(NT(171), (NT(117)));
//G252: __E___E_bf_76_78(171) => bf_neg(162).
	p(NT(171), (NT(162)));
//G253: __E___E_bf_76_78(171) => capture(30).
	p(NT(171), (NT(30)));
//G254: bf_and_nosep_2nd_oprnd(170) => __E___E_bf_76_78(171).
	p(NT(170), (NT(171)));
//G255: __E_bf_76(167)       => bf_and_nosep_1st_oprnd(168) bf_and_nosep_2nd_oprnd(170).
	p(NT(167), (NT(168)+NT(170)));
//G256: bf_and_nosep(166)    => __E_bf_76(167).
	p(NT(166), (NT(167)));
//G257: bf(31)               => bf_and_nosep(166).
	p(NT(31), (NT(166)));
//G258: bf(31)               => capture(30).
	p(NT(31), (NT(30)));
//G259: __E_constraint_79(173) => '[' ctnvar(174) _(13) '!' '=' _(13) num(120) ']'.
	p(NT(173), (T(26)+NT(174)+NT(13)+T(34)+T(3)+NT(13)+NT(120)+T(27)));
//G260: __E_constraint_79(173) => '[' num(120) _(13) '!' '=' _(13) ctnvar(174) ']'.
	p(NT(173), (T(26)+NT(120)+NT(13)+T(34)+T(3)+NT(13)+NT(174)+T(27)));
//G261: ctn_neq(172)         => __E_constraint_79(173).
	p(NT(172), (NT(173)));
//G262: constraint(70)       => ctn_neq(172).
	p(NT(70), (NT(172)));
//G263: __E_constraint_80(176) => '[' ctnvar(174) _(13) '=' _(13) num(120) ']'.
	p(NT(176), (T(26)+NT(174)+NT(13)+T(3)+NT(13)+NT(120)+T(27)));
//G264: __E_constraint_80(176) => '[' num(120) _(13) '=' _(13) ctnvar(174) ']'.
	p(NT(176), (T(26)+NT(120)+NT(13)+T(3)+NT(13)+NT(174)+T(27)));
//G265: ctn_eq(175)          => __E_constraint_80(176).
	p(NT(175), (NT(176)));
//G266: constraint(70)       => ctn_eq(175).
	p(NT(70), (NT(175)));
//G267: __E_constraint_81(178) => '[' ctnvar(174) _(13) '>' '=' _(13) num(120) ']'.
	p(NT(178), (T(26)+NT(174)+NT(13)+T(23)+T(3)+NT(13)+NT(120)+T(27)));
//G268: __E_constraint_81(178) => '[' num(120) _(13) '>' '=' _(13) ctnvar(174) ']'.
	p(NT(178), (T(26)+NT(120)+NT(13)+T(23)+T(3)+NT(13)+NT(174)+T(27)));
//G269: ctn_gteq(177)        => __E_constraint_81(178).
	p(NT(177), (NT(178)));
//G270: constraint(70)       => ctn_gteq(177).
	p(NT(70), (NT(177)));
//G271: __E_constraint_82(180) => '[' ctnvar(174) _(13) '>' _(13) num(120) ']'.
	p(NT(180), (T(26)+NT(174)+NT(13)+T(23)+NT(13)+NT(120)+T(27)));
//G272: __E_constraint_82(180) => '[' num(120) _(13) '>' _(13) ctnvar(174) ']'.
	p(NT(180), (T(26)+NT(120)+NT(13)+T(23)+NT(13)+NT(174)+T(27)));
//G273: ctn_gt(179)          => __E_constraint_82(180).
	p(NT(179), (NT(180)));
//G274: constraint(70)       => ctn_gt(179).
	p(NT(70), (NT(179)));
//G275: __E_constraint_83(182) => '[' ctnvar(174) _(13) '<' '=' _(13) num(120) ']'.
	p(NT(182), (T(26)+NT(174)+NT(13)+T(22)+T(3)+NT(13)+NT(120)+T(27)));
//G276: __E_constraint_83(182) => '[' num(120) _(13) '<' '=' _(13) ctnvar(174) ']'.
	p(NT(182), (T(26)+NT(120)+NT(13)+T(22)+T(3)+NT(13)+NT(174)+T(27)));
//G277: ctn_lteq(181)        => __E_constraint_83(182).
	p(NT(181), (NT(182)));
//G278: constraint(70)       => ctn_lteq(181).
	p(NT(70), (NT(181)));
//G279: __E_constraint_84(184) => '[' ctnvar(174) _(13) '<' _(13) num(120) ']'.
	p(NT(184), (T(26)+NT(174)+NT(13)+T(22)+NT(13)+NT(120)+T(27)));
//G280: __E_constraint_84(184) => '[' num(120) _(13) '<' _(13) ctnvar(174) ']'.
	p(NT(184), (T(26)+NT(120)+NT(13)+T(22)+NT(13)+NT(174)+T(27)));
//G281: ctn_lt(183)          => __E_constraint_84(184).
	p(NT(183), (NT(184)));
//G282: constraint(70)       => ctn_lt(183).
	p(NT(70), (NT(183)));
//G283: __E_ba_constant_85(185) => capture(30).
	p(NT(185), (NT(30)));
//G284: __E_ba_constant_85(185) => source(186).
	p(NT(185), (NT(186)));
//G285: __E_ba_constant_86(187) => typed(34).
	p(NT(187), (NT(34)));
//G286: __E_ba_constant_86(187) => null.
	p(NT(187), (nul));
//G287: ba_constant(111)     => '{' _(13) __E_ba_constant_85(185) _(13) '}' __E_ba_constant_86(187).
	p(NT(111), (T(45)+NT(13)+NT(185)+NT(13)+T(46)+NT(187)));
//G288: __E___E_source_87_88(190) => src_c(188).
	p(NT(190), (NT(188)));
//G289: __E___E_source_87_88(190) => space(2).
	p(NT(190), (NT(2)));
//G290: __E___E_source_87_89(191) => null.
	p(NT(191), (nul));
//G291: __E___E_source_87_89(191) => __E___E_source_87_88(190) __E___E_source_87_89(191).
	p(NT(191), (NT(190)+NT(191)));
//G292: __E_source_87(189)   => __E___E_source_87_89(191) src_c(188).
	p(NT(189), (NT(191)+NT(188)));
//G293: __E_source_87(189)   => null.
	p(NT(189), (nul));
//G294: source(186)          => src_c(188) __E_source_87(189).
	p(NT(186), (NT(188)+NT(189)));
//G295: src_c(188)           => alnum(6).
	p(NT(188), (NT(6)));
//G296: __N_8(392)           => '{'.
	p(NT(392), (T(45)));
//G297: __N_9(393)           => '}'.
	p(NT(393), (T(46)));
//G298: src_c(188)           => ~( __N_8(392) ) & ~( __N_9(393) ) & punct(7).	 # conjunctive
	p(NT(188), ~(NT(392)) & ~(NT(393)) & (NT(7)));
//G299: __E_src_c_90(192)    => src_c(188).
	p(NT(192), (NT(188)));
//G300: __E_src_c_90(192)    => space(2).
	p(NT(192), (NT(2)));
//G301: __E_src_c_91(193)    => null.
	p(NT(193), (nul));
//G302: __E_src_c_91(193)    => __E_src_c_90(192) __E_src_c_91(193).
	p(NT(193), (NT(192)+NT(193)));
//G303: src_c(188)           => '{' __E_src_c_91(193) '}'.
	p(NT(188), (T(45)+NT(193)+T(46)));
//G304: __E_variable_92(194) => uconst(195).
	p(NT(194), (NT(195)));
//G305: __E_variable_92(194) => io_var(196).
	p(NT(194), (NT(196)));
//G306: __E_variable_92(194) => var_name(197).
	p(NT(194), (NT(197)));
//G307: __E_variable_93(198) => typed(34).
	p(NT(198), (NT(34)));
//G308: __E_variable_93(198) => null.
	p(NT(198), (nul));
//G309: variable(112)        => __E_variable_92(194) __E_variable_93(198).
	p(NT(112), (NT(194)+NT(198)));
//G310: __N_10(394)          => 'F'.
	p(NT(394), (T(36)));
//G311: __N_11(395)          => 'T'.
	p(NT(395), (T(35)));
//G312: __E_var_name_94(199) => ~( __N_10(394) ) & ~( __N_11(395) ) & alpha(5).	 # conjunctive
	p(NT(199), ~(NT(394)) & ~(NT(395)) & (NT(5)));
//G313: __E_var_name_95(200) => null.
	p(NT(200), (nul));
//G314: __E_var_name_95(200) => digit(3) __E_var_name_95(200).
	p(NT(200), (NT(3)+NT(200)));
//G315: var_name(197)        => __E_var_name_94(199) __E_var_name_95(200).	 # guarded: charvar
	p(NT(197), (NT(199)+NT(200)));
	p.back().guard = "charvar";
//G316: __N_12(396)          => 'F'.
	p(NT(396), (T(36)));
//G317: __N_13(397)          => 'T'.
	p(NT(397), (T(35)));
//G318: __E_var_name_96(201) => ~( __N_12(396) ) & ~( __N_13(397) ) & alpha(5).	 # conjunctive
	p(NT(201), ~(NT(396)) & ~(NT(397)) & (NT(5)));
//G319: __E_var_name_97(202) => alnum(6).
	p(NT(202), (NT(6)));
//G320: __E_var_name_97(202) => '_'.
	p(NT(202), (T(47)));
//G321: __E_var_name_98(203) => null.
	p(NT(203), (nul));
//G322: __E_var_name_98(203) => __E_var_name_97(202) __E_var_name_98(203).
	p(NT(203), (NT(202)+NT(203)));
//G323: var_name(197)        => __E_var_name_96(201) __E_var_name_98(203).	 # guarded: var
	p(NT(197), (NT(201)+NT(203)));
	p.back().guard = "var";
//G324: io_var(196)          => io_var_name(32) '[' offset(204) ']'.
	p(NT(196), (NT(32)+T(26)+NT(204)+T(27)));
//G325: io_var_name(32)      => chars(205).
	p(NT(32), (NT(205)));
//G326: __E___E_uconst_99_100(208) => chars(205) _(13).
	p(NT(208), (NT(205)+NT(13)));
//G327: __E___E_uconst_99_100(208) => null.
	p(NT(208), (nul));
//G328: __E_uconst_99(207)   => __E___E_uconst_99_100(208) ':' _(13) chars(205).
	p(NT(207), (NT(208)+T(2)+NT(13)+NT(205)));
//G329: uconst_name(206)     => __E_uconst_99(207).
	p(NT(206), (NT(207)));
//G330: uconst(195)          => '<' _(13) uconst_name(206) _(13) '>'.
	p(NT(195), (T(22)+NT(13)+NT(206)+NT(13)+T(23)));
//G331: __E_q_vars_101(210)  => _(13) ',' _(13) q_var(209).
	p(NT(210), (NT(13)+T(20)+NT(13)+NT(209)));
//G332: __E_q_vars_102(211)  => null.
	p(NT(211), (nul));
//G333: __E_q_vars_102(211)  => __E_q_vars_101(210) __E_q_vars_102(211).
	p(NT(211), (NT(210)+NT(211)));
//G334: q_vars(66)           => q_var(209) __E_q_vars_102(211).
	p(NT(66), (NT(209)+NT(211)));
//G335: q_var(209)           => capture(30).
	p(NT(209), (NT(30)));
//G336: __N_14(398)          => uconst(195).
	p(NT(398), (NT(195)));
//G337: q_var(209)           => variable(112) & ~( __N_14(398) ).	 # conjunctive
	p(NT(209), (NT(112)) & ~(NT(398)));
//G338: ctnvar(174)          => chars(205).
	p(NT(174), (NT(205)));
//G339: __E_offsets_103(212) => _(13) ',' _(13) offset(204).
	p(NT(212), (NT(13)+T(20)+NT(13)+NT(204)));
//G340: __E_offsets_104(213) => null.
	p(NT(213), (nul));
//G341: __E_offsets_104(213) => __E_offsets_103(212) __E_offsets_104(213).
	p(NT(213), (NT(212)+NT(213)));
//G342: offsets(42)          => '[' _(13) offset(204) __E_offsets_104(213) _(13) ']'.
	p(NT(42), (T(26)+NT(13)+NT(204)+NT(213)+NT(13)+T(27)));
//G343: offset(204)          => integer(214).
	p(NT(204), (NT(214)));
//G344: offset(204)          => capture(30).
	p(NT(204), (NT(30)));
//G345: offset(204)          => shift(215).
	p(NT(204), (NT(215)));
//G346: __N_15(399)          => io_var(196).
	p(NT(399), (NT(196)));
//G347: __E_offset_105(216)  => variable(112) & ~( __N_15(399) ).	 # conjunctive
	p(NT(216), (NT(112)) & ~(NT(399)));
//G348: offset(204)          => __E_offset_105(216).
	p(NT(204), (NT(216)));
//G349: __E_shift_106(217)   => capture(30).
	p(NT(217), (NT(30)));
//G350: __N_16(400)          => io_var(196).
	p(NT(400), (NT(196)));
//G351: __E___E_shift_106_107(218) => variable(112) & ~( __N_16(400) ).	 # conjunctive
	p(NT(218), (NT(112)) & ~(NT(400)));
//G352: __E_shift_106(217)   => __E___E_shift_106_107(218).
	p(NT(217), (NT(218)));
//G353: shift(215)           => __E_shift_106(217) _(13) '-' _(13) num(120).
	p(NT(215), (NT(217)+NT(13)+T(30)+NT(13)+NT(120)));
//G354: __E_chars_108(219)   => alnum(6).
	p(NT(219), (NT(6)));
//G355: __E_chars_108(219)   => '_'.
	p(NT(219), (T(47)));
//G356: __E_chars_109(220)   => null.
	p(NT(220), (nul));
//G357: __E_chars_109(220)   => __E_chars_108(219) __E_chars_109(220).
	p(NT(220), (NT(219)+NT(220)));
//G358: chars(205)           => alpha(5) __E_chars_109(220).
	p(NT(205), (NT(5)+NT(220)));
//G359: __E_digits_110(222)  => digit(3).
	p(NT(222), (NT(3)));
//G360: __E_digits_110(222)  => digit(3) __E_digits_110(222).
	p(NT(222), (NT(3)+NT(222)));
//G361: digits(221)          => __E_digits_110(222).
	p(NT(221), (NT(222)));
//G362: num(120)             => digits(221).
	p(NT(120), (NT(221)));
//G363: __E_integer_111(223) => '-'.
	p(NT(223), (T(30)));
//G364: __E_integer_111(223) => null.
	p(NT(223), (nul));
//G365: integer(214)         => __E_integer_111(223) _(13) digits(221).
	p(NT(214), (NT(223)+NT(13)+NT(221)));
//G366: sym(40)              => chars(205).
	p(NT(40), (NT(205)));
//G367: capture(30)          => '$' chars(205).
	p(NT(30), (T(48)+NT(205)));
//G368: typed(34)            => _(13) ':' _(13) type(224) _(13) '[' _(13) subtype(225) _(13) ']'.
	p(NT(34), (NT(13)+T(2)+NT(13)+NT(224)+NT(13)+T(26)+NT(13)+NT(225)+NT(13)+T(27)));
//G369: __N_17(401)          => 'b' 'v'.
	p(NT(401), (T(17)+T(37)));
//G370: __E_typed_112(226)   => ~( __N_17(401) ) & type(224).	 # conjunctive
	p(NT(226), ~(NT(401)) & (NT(224)));
//G371: typed(34)            => _(13) ':' _(13) __E_typed_112(226).
	p(NT(34), (NT(13)+T(2)+NT(13)+NT(226)));
//G372: type(224)            => chars(205).
	p(NT(224), (NT(205)));
//G373: subtype(225)         => num(120).
	p(NT(225), (NT(120)));
//G374: __E_comment_113(228) => printable(8).
	p(NT(228), (NT(8)));
//G375: __E_comment_113(228) => '\t'.
	p(NT(228), (T(49)));
//G376: __E_comment_114(229) => null.
	p(NT(229), (nul));
//G377: __E_comment_114(229) => __E_comment_113(228) __E_comment_114(229).
	p(NT(229), (NT(228)+NT(229)));
//G378: __E_comment_115(230) => '\n'.
	p(NT(230), (T(50)));
//G379: __E_comment_115(230) => '\r'.
	p(NT(230), (T(51)));
//G380: __E_comment_115(230) => eof(1).
	p(NT(230), (NT(1)));
//G381: comment(227)         => '#' __E_comment_114(229) __E_comment_115(230).
	p(NT(227), (T(52)+NT(229)+NT(230)));
//G382: __(35)               => space(2).
	p(NT(35), (NT(2)));
//G383: __(35)               => comment(227).
	p(NT(35), (NT(227)));
//G384: __(35)               => __(35) space(2).
	p(NT(35), (NT(35)+NT(2)));
//G385: __(35)               => __(35) comment(227).
	p(NT(35), (NT(35)+NT(227)));
//G386: _(13)                => __(35).
	p(NT(13), (NT(35)));
//G387: _(13)                => null.
	p(NT(13), (nul));
//G388: cli(231)             => _(13).
	p(NT(231), (NT(13)));
//G389: __E_cli_116(233)     => '.' _(13) cli_command(232) _(13).
	p(NT(233), (T(1)+NT(13)+NT(232)+NT(13)));
//G390: __E_cli_117(234)     => null.
	p(NT(234), (nul));
//G391: __E_cli_117(234)     => __E_cli_116(233) __E_cli_117(234).
	p(NT(234), (NT(233)+NT(234)));
//G392: __E_cli_118(235)     => '.' _(13).
	p(NT(235), (T(1)+NT(13)));
//G393: __E_cli_118(235)     => null.
	p(NT(235), (nul));
//G394: cli(231)             => _(13) cli_command(232) _(13) __E_cli_117(234) __E_cli_118(235).
	p(NT(231), (NT(13)+NT(232)+NT(13)+NT(234)+NT(235)));
//G395: __E_cli_command_119(238) => 'q'.
	p(NT(238), (T(53)));
//G396: __E_cli_command_119(238) => 'q' 'u' 'i' 't'.
	p(NT(238), (T(53)+T(7)+T(4)+T(8)));
//G397: quit_sym(237)        => __E_cli_command_119(238).
	p(NT(237), (NT(238)));
//G398: quit_cmd(236)        => quit_sym(237).
	p(NT(236), (NT(237)));
//G399: cli_command(232)     => quit_cmd(236).
	p(NT(232), (NT(236)));
//G400: __E_cli_command_120(241) => 'v'.
	p(NT(241), (T(37)));
//G401: __E_cli_command_120(241) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(241), (T(37)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G402: version_sym(240)     => __E_cli_command_120(241).
	p(NT(240), (NT(241)));
//G403: version_cmd(239)     => version_sym(240).
	p(NT(239), (NT(240)));
//G404: cli_command(232)     => version_cmd(239).
	p(NT(232), (NT(239)));
//G405: __E_cli_command_121(244) => 'c'.
	p(NT(244), (T(14)));
//G406: __E_cli_command_121(244) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(244), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G407: clear_sym(243)       => __E_cli_command_121(244).
	p(NT(243), (NT(244)));
//G408: clear_cmd(242)       => clear_sym(243).
	p(NT(242), (NT(243)));
//G409: cli_command(232)     => clear_cmd(242).
	p(NT(232), (NT(242)));
//G410: __E___E_cli_command_122_123(248) => 'h'.
	p(NT(248), (T(54)));
//G411: __E___E_cli_command_122_123(248) => 'h' 'e' 'l' 'p'.
	p(NT(248), (T(54)+T(11)+T(10)+T(55)));
//G412: help_sym(247)        => __E___E_cli_command_122_123(248).
	p(NT(247), (NT(248)));
//G413: __E___E_cli_command_122_124(249) => __(35) help_arg(250).
	p(NT(249), (NT(35)+NT(250)));
//G414: __E___E_cli_command_122_124(249) => null.
	p(NT(249), (nul));
//G415: __E_cli_command_122(246) => help_sym(247) __E___E_cli_command_122_124(249).
	p(NT(246), (NT(247)+NT(249)));
//G416: help_cmd(245)        => __E_cli_command_122(246).
	p(NT(245), (NT(246)));
//G417: cli_command(232)     => help_cmd(245).
	p(NT(232), (NT(245)));
//G418: file_sym(253)        => 'f' 'i' 'l' 'e'.
	p(NT(253), (T(9)+T(4)+T(10)+T(11)));
//G419: __E_cli_command_125(252) => file_sym(253) __(35) q_string(254).
	p(NT(252), (NT(253)+NT(35)+NT(254)));
//G420: file_cmd(251)        => __E_cli_command_125(252).
	p(NT(251), (NT(252)));
//G421: cli_command(232)     => file_cmd(251).
	p(NT(232), (NT(251)));
//G422: valid_sym(257)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(257), (T(37)+T(16)+T(10)+T(4)+T(56)));
//G423: __E_cli_command_126(256) => valid_sym(257) __(35) spec_arg(258).
	p(NT(256), (NT(257)+NT(35)+NT(258)));
//G424: valid_cmd(255)       => __E_cli_command_126(256).
	p(NT(255), (NT(256)));
//G425: cli_command(232)     => valid_cmd(255).
	p(NT(232), (NT(255)));
//G426: sat_sym(261)         => 's' 'a' 't'.
	p(NT(261), (T(15)+T(16)+T(8)));
//G427: __E_cli_command_127(260) => sat_sym(261) __(35) spec_arg(258).
	p(NT(260), (NT(261)+NT(35)+NT(258)));
//G428: sat_cmd(259)         => __E_cli_command_127(260).
	p(NT(259), (NT(260)));
//G429: cli_command(232)     => sat_cmd(259).
	p(NT(232), (NT(259)));
//G430: unsat_sym(264)       => 'u' 'n' 's' 'a' 't'.
	p(NT(264), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G431: __E_cli_command_128(263) => unsat_sym(264) __(35) spec_arg(258).
	p(NT(263), (NT(264)+NT(35)+NT(258)));
//G432: unsat_cmd(262)       => __E_cli_command_128(263).
	p(NT(262), (NT(263)));
//G433: cli_command(232)     => unsat_cmd(262).
	p(NT(232), (NT(262)));
//G434: solve_sym(267)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(267), (T(15)+T(6)+T(10)+T(37)+T(11)));
//G435: __E___E_cli_command_129_130(268) => solve_options(269).
	p(NT(268), (NT(269)));
//G436: __E___E_cli_command_129_130(268) => null.
	p(NT(268), (nul));
//G437: __E_cli_command_129(266) => solve_sym(267) __E___E_cli_command_129_130(268) __(35) wff_cmd_arg(270).
	p(NT(266), (NT(267)+NT(268)+NT(35)+NT(270)));
//G438: solve_cmd(265)       => __E_cli_command_129(266).
	p(NT(265), (NT(266)));
//G439: cli_command(232)     => solve_cmd(265).
	p(NT(232), (NT(265)));
//G440: lgrs_sym(273)        => 'l' 'g' 'r' 's'.
	p(NT(273), (T(10)+T(57)+T(19)+T(15)));
//G441: __E_cli_command_131(272) => lgrs_sym(273) __(35) wff_cmd_arg(270).
	p(NT(272), (NT(273)+NT(35)+NT(270)));
//G442: lgrs_cmd(271)        => __E_cli_command_131(272).
	p(NT(271), (NT(272)));
//G443: cli_command(232)     => lgrs_cmd(271).
	p(NT(232), (NT(271)));
//G444: __E___E_cli_command_132_133(277) => 'r'.
	p(NT(277), (T(19)));
//G445: __E___E_cli_command_132_133(277) => 'r' 'u' 'n'.
	p(NT(277), (T(19)+T(7)+T(5)));
//G446: run_sym(276)         => __E___E_cli_command_132_133(277).
	p(NT(276), (NT(277)));
//G447: __E_cli_command_132(275) => run_sym(276) __(35) spec_arg(258).
	p(NT(275), (NT(276)+NT(35)+NT(258)));
//G448: run_cmd(274)         => __E_cli_command_132(275).
	p(NT(274), (NT(275)));
//G449: cli_command(232)     => run_cmd(274).
	p(NT(232), (NT(274)));
//G450: __E___E_cli_command_134_135(281) => 'n'.
	p(NT(281), (T(5)));
//G451: __E___E_cli_command_134_135(281) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(281), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(58)+T(11)));
//G452: normalize_sym(280)   => __E___E_cli_command_134_135(281).
	p(NT(280), (NT(281)));
//G453: __E_cli_command_134(279) => normalize_sym(280) __(35) spec_arg(258).
	p(NT(279), (NT(280)+NT(35)+NT(258)));
//G454: normalize_cmd(278)   => __E_cli_command_134(279).
	p(NT(278), (NT(279)));
//G455: cli_command(232)     => normalize_cmd(278).
	p(NT(232), (NT(278)));
//G456: __E___E_cli_command_136_137(285) => 's'.
	p(NT(285), (T(15)));
//G457: __E___E_cli_command_136_137(285) => 's' 'u' 'b' 's' 't'.
	p(NT(285), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G458: __E___E_cli_command_136_137(285) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(285), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G459: subst_sym(284)       => __E___E_cli_command_136_137(285).
	p(NT(284), (NT(285)));
//G460: __E_cli_command_136(283) => subst_sym(284) __(35) nf_cmd_arg(286) _(13) '[' _(13) nf_cmd_arg(286) _(13) '/' _(13) nf_cmd_arg(286) _(13) ']'.
	p(NT(283), (NT(284)+NT(35)+NT(286)+NT(13)+T(26)+NT(13)+NT(286)+NT(13)+T(41)+NT(13)+NT(286)+NT(13)+T(27)));
//G461: subst_cmd(282)       => __E_cli_command_136(283).
	p(NT(282), (NT(283)));
//G462: cli_command(232)     => subst_cmd(282).
	p(NT(232), (NT(282)));
//G463: __E___E_cli_command_138_139(290) => 'i'.
	p(NT(290), (T(4)));
//G464: __E___E_cli_command_138_139(290) => 'i' 'n' 's' 't'.
	p(NT(290), (T(4)+T(5)+T(15)+T(8)));
//G465: __E___E_cli_command_138_139(290) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(290), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G466: inst_sym(289)        => __E___E_cli_command_138_139(290).
	p(NT(289), (NT(290)));
//G467: __E_cli_command_138(288) => inst_sym(289) __(35) inst_args(291).
	p(NT(288), (NT(289)+NT(35)+NT(291)));
//G468: inst_cmd(287)        => __E_cli_command_138(288).
	p(NT(287), (NT(288)));
//G469: cli_command(232)     => inst_cmd(287).
	p(NT(232), (NT(287)));
//G470: dnf_sym(294)         => 'd' 'n' 'f'.
	p(NT(294), (T(56)+T(5)+T(9)));
//G471: __E_cli_command_140(293) => dnf_sym(294) __(35) nf_cmd_arg(286).
	p(NT(293), (NT(294)+NT(35)+NT(286)));
//G472: dnf_cmd(292)         => __E_cli_command_140(293).
	p(NT(292), (NT(293)));
//G473: cli_command(232)     => dnf_cmd(292).
	p(NT(232), (NT(292)));
//G474: cnf_sym(297)         => 'c' 'n' 'f'.
	p(NT(297), (T(14)+T(5)+T(9)));
//G475: __E_cli_command_141(296) => cnf_sym(297) __(35) nf_cmd_arg(286).
	p(NT(296), (NT(297)+NT(35)+NT(286)));
//G476: cnf_cmd(295)         => __E_cli_command_141(296).
	p(NT(295), (NT(296)));
//G477: cli_command(232)     => cnf_cmd(295).
	p(NT(232), (NT(295)));
//G478: anf_sym(300)         => 'a' 'n' 'f'.
	p(NT(300), (T(16)+T(5)+T(9)));
//G479: __E_cli_command_142(299) => anf_sym(300) __(35) nf_cmd_arg(286).
	p(NT(299), (NT(300)+NT(35)+NT(286)));
//G480: anf_cmd(298)         => __E_cli_command_142(299).
	p(NT(298), (NT(299)));
//G481: cli_command(232)     => anf_cmd(298).
	p(NT(232), (NT(298)));
//G482: nnf_sym(303)         => 'n' 'n' 'f'.
	p(NT(303), (T(5)+T(5)+T(9)));
//G483: __E_cli_command_143(302) => nnf_sym(303) __(35) nf_cmd_arg(286).
	p(NT(302), (NT(303)+NT(35)+NT(286)));
//G484: nnf_cmd(301)         => __E_cli_command_143(302).
	p(NT(301), (NT(302)));
//G485: cli_command(232)     => nnf_cmd(301).
	p(NT(232), (NT(301)));
//G486: pnf_sym(306)         => 'p' 'n' 'f'.
	p(NT(306), (T(55)+T(5)+T(9)));
//G487: __E_cli_command_144(305) => pnf_sym(306) __(35) nf_cmd_arg(286).
	p(NT(305), (NT(306)+NT(35)+NT(286)));
//G488: pnf_cmd(304)         => __E_cli_command_144(305).
	p(NT(304), (NT(305)));
//G489: cli_command(232)     => pnf_cmd(304).
	p(NT(232), (NT(304)));
//G490: mnf_sym(309)         => 'm' 'n' 'f'.
	p(NT(309), (T(21)+T(5)+T(9)));
//G491: __E_cli_command_145(308) => mnf_sym(309) __(35) nf_cmd_arg(286).
	p(NT(308), (NT(309)+NT(35)+NT(286)));
//G492: mnf_cmd(307)         => __E_cli_command_145(308).
	p(NT(307), (NT(308)));
//G493: cli_command(232)     => mnf_cmd(307).
	p(NT(232), (NT(307)));
//G494: onf_sym(312)         => 'o' 'n' 'f'.
	p(NT(312), (T(6)+T(5)+T(9)));
//G495: __E_cli_command_146(311) => onf_sym(312) __(35) variable(112) __(35) onf_cmd_arg(313).
	p(NT(311), (NT(312)+NT(35)+NT(112)+NT(35)+NT(313)));
//G496: onf_cmd(310)         => __E_cli_command_146(311).
	p(NT(310), (NT(311)));
//G497: cli_command(232)     => onf_cmd(310).
	p(NT(232), (NT(310)));
//G498: qelim_sym(316)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(316), (T(53)+T(11)+T(10)+T(4)+T(21)));
//G499: __E_cli_command_147(315) => qelim_sym(316) __(35) wff_cmd_arg(270).
	p(NT(315), (NT(316)+NT(35)+NT(270)));
//G500: qelim_cmd(314)       => __E_cli_command_147(315).
	p(NT(314), (NT(315)));
//G501: cli_command(232)     => qelim_cmd(314).
	p(NT(232), (NT(314)));
//G502: get_sym(319)         => 'g' 'e' 't'.
	p(NT(319), (T(57)+T(11)+T(8)));
//G503: __E___E_cli_command_148_149(320) => __(35) option_name(321).
	p(NT(320), (NT(35)+NT(321)));
//G504: __E___E_cli_command_148_149(320) => null.
	p(NT(320), (nul));
//G505: __E_cli_command_148(318) => get_sym(319) __E___E_cli_command_148_149(320).
	p(NT(318), (NT(319)+NT(320)));
//G506: get_cmd(317)         => __E_cli_command_148(318).
	p(NT(317), (NT(318)));
//G507: cli_command(232)     => get_cmd(317).
	p(NT(232), (NT(317)));
//G508: set_sym(324)         => 's' 'e' 't'.
	p(NT(324), (T(15)+T(11)+T(8)));
//G509: __E___E_cli_command_150_151(325) => __(35).
	p(NT(325), (NT(35)));
//G510: __E___E_cli_command_150_151(325) => _(13) '=' _(13).
	p(NT(325), (NT(13)+T(3)+NT(13)));
//G511: __E_cli_command_150(323) => set_sym(324) __(35) option_name(321) __E___E_cli_command_150_151(325) option_value(326).
	p(NT(323), (NT(324)+NT(35)+NT(321)+NT(325)+NT(326)));
//G512: set_cmd(322)         => __E_cli_command_150(323).
	p(NT(322), (NT(323)));
//G513: cli_command(232)     => set_cmd(322).
	p(NT(232), (NT(322)));
//G514: enable_sym(329)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(329), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G515: __E_cli_command_152(328) => enable_sym(329) __(35) option_name(321).
	p(NT(328), (NT(329)+NT(35)+NT(321)));
//G516: enable_cmd(327)      => __E_cli_command_152(328).
	p(NT(327), (NT(328)));
//G517: cli_command(232)     => enable_cmd(327).
	p(NT(232), (NT(327)));
//G518: disable_sym(332)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(332), (T(56)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G519: __E_cli_command_153(331) => disable_sym(332) __(35) option_name(321).
	p(NT(331), (NT(332)+NT(35)+NT(321)));
//G520: disable_cmd(330)     => __E_cli_command_153(331).
	p(NT(330), (NT(331)));
//G521: cli_command(232)     => disable_cmd(330).
	p(NT(232), (NT(330)));
//G522: toggle_sym(335)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(335), (T(8)+T(6)+T(57)+T(57)+T(10)+T(11)));
//G523: __E_cli_command_154(334) => toggle_sym(335) __(35) option_name(321).
	p(NT(334), (NT(335)+NT(35)+NT(321)));
//G524: toggle_cmd(333)      => __E_cli_command_154(334).
	p(NT(333), (NT(334)));
//G525: cli_command(232)     => toggle_cmd(333).
	p(NT(232), (NT(333)));
//G526: __E___E_cli_command_155_156(339) => 'd' 'e' 'f' 's'.
	p(NT(339), (T(56)+T(11)+T(9)+T(15)));
//G527: __E___E_cli_command_155_156(339) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(339), (T(56)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G528: def_sym(338)         => __E___E_cli_command_155_156(339).
	p(NT(338), (NT(339)));
//G529: __E_cli_command_155(337) => def_sym(338).
	p(NT(337), (NT(338)));
//G530: def_list_cmd(336)    => __E_cli_command_155(337).
	p(NT(336), (NT(337)));
//G531: cli_command(232)     => def_list_cmd(336).
	p(NT(232), (NT(336)));
//G532: __E_cli_command_157(341) => def_sym(338) __(35) num(120).
	p(NT(341), (NT(338)+NT(35)+NT(120)));
//G533: def_print_cmd(340)   => __E_cli_command_157(341).
	p(NT(340), (NT(341)));
//G534: cli_command(232)     => def_print_cmd(340).
	p(NT(232), (NT(340)));
//G535: def_rr_cmd(342)      => rec_relation(19).
	p(NT(342), (NT(19)));
//G536: cli_command(232)     => def_rr_cmd(342).
	p(NT(232), (NT(342)));
//G537: def_input_cmd(343)   => input_def(20).
	p(NT(343), (NT(20)));
//G538: cli_command(232)     => def_input_cmd(343).
	p(NT(232), (NT(343)));
//G539: def_output_cmd(344)  => output_def(21).
	p(NT(344), (NT(21)));
//G540: cli_command(232)     => def_output_cmd(344).
	p(NT(232), (NT(344)));
//G541: __E___E_cli_command_158_159(348) => 'h' 'i' 's' 't'.
	p(NT(348), (T(54)+T(4)+T(15)+T(8)));
//G542: __E___E_cli_command_158_159(348) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(348), (T(54)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G543: history_sym(347)     => __E___E_cli_command_158_159(348).
	p(NT(347), (NT(348)));
//G544: __E_cli_command_158(346) => history_sym(347).
	p(NT(346), (NT(347)));
//G545: history_list_cmd(345) => __E_cli_command_158(346).
	p(NT(345), (NT(346)));
//G546: cli_command(232)     => history_list_cmd(345).
	p(NT(232), (NT(345)));
//G547: __E_cli_command_160(350) => history_sym(347) __(35) history(351).
	p(NT(350), (NT(347)+NT(35)+NT(351)));
//G548: history_print_cmd(349) => __E_cli_command_160(350).
	p(NT(349), (NT(350)));
//G549: cli_command(232)     => history_print_cmd(349).
	p(NT(232), (NT(349)));
//G550: __E_cli_command_161(353) => ref(28).
	p(NT(353), (NT(28)));
//G551: __E_cli_command_161(353) => wff(15).
	p(NT(353), (NT(15)));
//G552: __E_cli_command_161(353) => bf(31).
	p(NT(353), (NT(31)));
//G553: history_store_cmd(352) => __E_cli_command_161(353).
	p(NT(352), (NT(353)));
//G554: cli_command(232)     => history_store_cmd(352).
	p(NT(232), (NT(352)));
//G555: __E_solve_options_162(354) => __(35) solve_option(355).
	p(NT(354), (NT(35)+NT(355)));
//G556: __E_solve_options_163(356) => null.
	p(NT(356), (nul));
//G557: __E_solve_options_163(356) => __E_solve_options_162(354) __E_solve_options_163(356).
	p(NT(356), (NT(354)+NT(356)));
//G558: solve_options(269)   => __E_solve_options_163(356).
	p(NT(269), (NT(356)));
//G559: __E_solve_option_164(357) => solver_mode(358).
	p(NT(357), (NT(358)));
//G560: __E_solve_option_164(357) => type(224).
	p(NT(357), (NT(224)));
//G561: solve_option(355)    => '-' '-' __E_solve_option_164(357).
	p(NT(355), (T(30)+T(30)+NT(357)));
//G562: __E_solver_mode_165(360) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(360), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G563: __E_solver_mode_165(360) => 'm' 'i' 'n'.
	p(NT(360), (T(21)+T(4)+T(5)));
//G564: solver_mode_minimum(359) => __E_solver_mode_165(360).
	p(NT(359), (NT(360)));
//G565: solver_mode(358)     => solver_mode_minimum(359).
	p(NT(358), (NT(359)));
//G566: __E_solver_mode_166(362) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(362), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G567: __E_solver_mode_166(362) => 'm' 'a' 'x'.
	p(NT(362), (T(21)+T(16)+T(29)));
//G568: solver_mode_maximum(361) => __E_solver_mode_166(362).
	p(NT(361), (NT(362)));
//G569: solver_mode(358)     => solver_mode_maximum(361).
	p(NT(358), (NT(361)));
//G570: q_file_name(38)      => '"' file_name(363) '"'.
	p(NT(38), (T(59)+NT(363)+T(59)));
//G571: __E_file_name_167(364) => printable(8).
	p(NT(364), (NT(8)));
//G572: __E_file_name_167(364) => printable(8) __E_file_name_167(364).
	p(NT(364), (NT(8)+NT(364)));
//G573: file_name(363)       => __E_file_name_167(364).
	p(NT(363), (NT(364)));
//G574: __E_option_name_168(365) => alnum(6).
	p(NT(365), (NT(6)));
//G575: __E_option_name_168(365) => alnum(6) __E_option_name_168(365).
	p(NT(365), (NT(6)+NT(365)));
//G576: option_name(321)     => __E_option_name_168(365).
	p(NT(321), (NT(365)));
//G577: __E_option_value_169(366) => alnum(6).
	p(NT(366), (NT(6)));
//G578: __E_option_value_169(366) => alnum(6) __E_option_value_169(366).
	p(NT(366), (NT(6)+NT(366)));
//G579: option_value(326)    => __E_option_value_169(366).
	p(NT(326), (NT(366)));
//G580: bf_cmd_arg(367)      => history(351).
	p(NT(367), (NT(351)));
//G581: bf_cmd_arg(367)      => bf(31).
	p(NT(367), (NT(31)));
//G582: wff_cmd_arg(270)     => history(351).
	p(NT(270), (NT(351)));
//G583: wff_cmd_arg(270)     => wff(15).
	p(NT(270), (NT(15)));
//G584: nf_cmd_arg(286)      => history(351).
	p(NT(286), (NT(351)));
//G585: nf_cmd_arg(286)      => ref(28).
	p(NT(286), (NT(28)));
//G586: nf_cmd_arg(286)      => wff(15).
	p(NT(286), (NT(15)));
//G587: nf_cmd_arg(286)      => bf(31).
	p(NT(286), (NT(31)));
//G588: onf_cmd_arg(313)     => history(351).
	p(NT(313), (NT(351)));
//G589: onf_cmd_arg(313)     => wff(15).
	p(NT(313), (NT(15)));
//G590: spec_arg(258)        => history(351).
	p(NT(258), (NT(351)));
//G591: spec_arg(258)        => spec(10).
	p(NT(258), (NT(10)));
//G592: spec_arg(258)        => ref(28).
	p(NT(258), (NT(28)));
//G593: spec_arg(258)        => wff(15).
	p(NT(258), (NT(15)));
//G594: spec_arg(258)        => bf(31).
	p(NT(258), (NT(31)));
//G595: inst_args(291)       => wff_cmd_arg(270) _(13) '[' _(13) bf(31) _(13) '/' _(13) bf_cmd_arg(367) _(13) ']'.
	p(NT(291), (NT(270)+NT(13)+T(26)+NT(13)+NT(31)+NT(13)+T(41)+NT(13)+NT(367)+NT(13)+T(27)));
//G596: inst_args(291)       => bf_cmd_arg(367) _(13) '[' _(13) bf(31) _(13) '/' _(13) bf_cmd_arg(367) _(13) ']'.
	p(NT(291), (NT(367)+NT(13)+T(26)+NT(13)+NT(31)+NT(13)+T(41)+NT(13)+NT(367)+NT(13)+T(27)));
//G597: help_arg(250)        => help_sym(247).
	p(NT(250), (NT(247)));
//G598: help_arg(250)        => version_sym(240).
	p(NT(250), (NT(240)));
//G599: help_arg(250)        => quit_sym(237).
	p(NT(250), (NT(237)));
//G600: help_arg(250)        => clear_sym(243).
	p(NT(250), (NT(243)));
//G601: help_arg(250)        => get_sym(319).
	p(NT(250), (NT(319)));
//G602: help_arg(250)        => set_sym(324).
	p(NT(250), (NT(324)));
//G603: help_arg(250)        => enable_sym(329).
	p(NT(250), (NT(329)));
//G604: help_arg(250)        => disable_sym(332).
	p(NT(250), (NT(332)));
//G605: help_arg(250)        => toggle_sym(335).
	p(NT(250), (NT(335)));
//G606: help_arg(250)        => file_sym(253).
	p(NT(250), (NT(253)));
//G607: help_arg(250)        => history_sym(347).
	p(NT(250), (NT(347)));
//G608: help_arg(250)        => abs_history_sym(368).
	p(NT(250), (NT(368)));
//G609: help_arg(250)        => rel_history_sym(369).
	p(NT(250), (NT(369)));
//G610: help_arg(250)        => selection_sym(370).
	p(NT(250), (NT(370)));
//G611: help_arg(250)        => def_sym(338).
	p(NT(250), (NT(338)));
//G612: help_arg(250)        => inst_sym(289).
	p(NT(250), (NT(289)));
//G613: help_arg(250)        => subst_sym(284).
	p(NT(250), (NT(284)));
//G614: help_arg(250)        => normalize_sym(280).
	p(NT(250), (NT(280)));
//G615: help_arg(250)        => execute_sym(371).
	p(NT(250), (NT(371)));
//G616: help_arg(250)        => solve_sym(267).
	p(NT(250), (NT(267)));
//G617: help_arg(250)        => valid_sym(257).
	p(NT(250), (NT(257)));
//G618: help_arg(250)        => sat_sym(261).
	p(NT(250), (NT(261)));
//G619: help_arg(250)        => unsat_sym(264).
	p(NT(250), (NT(264)));
//G620: help_arg(250)        => run_sym(276).
	p(NT(250), (NT(276)));
//G621: help_arg(250)        => dnf_sym(294).
	p(NT(250), (NT(294)));
//G622: help_arg(250)        => cnf_sym(297).
	p(NT(250), (NT(297)));
//G623: help_arg(250)        => anf_sym(300).
	p(NT(250), (NT(300)));
//G624: help_arg(250)        => snf_sym(372).
	p(NT(250), (NT(372)));
//G625: help_arg(250)        => nnf_sym(303).
	p(NT(250), (NT(303)));
//G626: help_arg(250)        => pnf_sym(306).
	p(NT(250), (NT(306)));
//G627: help_arg(250)        => mnf_sym(309).
	p(NT(250), (NT(309)));
//G628: help_arg(250)        => onf_sym(312).
	p(NT(250), (NT(312)));
//G629: help_arg(250)        => qelim_sym(316).
	p(NT(250), (NT(316)));
//G630: __E___E_help_arg_170_171(375) => 's'.
	p(NT(375), (T(15)));
//G631: __E___E_help_arg_170_171(375) => null.
	p(NT(375), (nul));
//G632: __E_help_arg_170(374) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_170_171(375).
	p(NT(374), (T(11)+T(29)+T(16)+T(21)+T(55)+T(10)+T(11)+NT(375)));
//G633: examples_sym(373)    => __E_help_arg_170(374).
	p(NT(373), (NT(374)));
//G634: help_arg(250)        => examples_sym(373).
	p(NT(250), (NT(373)));
//G635: __E_history_172(377) => '%'.
	p(NT(377), (T(39)));
//G636: rel_history_sym(369) => '%' '-'.
	p(NT(369), (T(39)+T(30)));
//G637: history_id(379)      => digits(221).
	p(NT(379), (NT(221)));
//G638: __E___E_history_172_173(378) => history_id(379).
	p(NT(378), (NT(379)));
//G639: __E___E_history_172_173(378) => null.
	p(NT(378), (nul));
//G640: __E_history_172(377) => rel_history_sym(369) __E___E_history_172_173(378).
	p(NT(377), (NT(369)+NT(378)));
//G641: rel_history(376)     => __E_history_172(377).
	p(NT(376), (NT(377)));
//G642: history(351)         => rel_history(376).
	p(NT(351), (NT(376)));
//G643: abs_history_sym(368) => '%'.
	p(NT(368), (T(39)));
//G644: __E_history_174(381) => abs_history_sym(368) history_id(379).
	p(NT(381), (NT(368)+NT(379)));
//G645: abs_history(380)     => __E_history_174(381).
	p(NT(380), (NT(381)));
//G646: history(351)         => abs_history(380).
	p(NT(351), (NT(380)));
//G647: stream_def(382)      => input_def(20).
	p(NT(382), (NT(20)));
//G648: stream_def(382)      => output_def(21).
	p(NT(382), (NT(21)));
//G649: fm_or_term(383)      => wff(15).
	p(NT(383), (NT(15)));
//G650: fm_or_term(383)      => bf(31).
	p(NT(383), (NT(31)));
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
		num, bf_shr, __E_bf_48, __E___E_bf_48_49, bf_shl, __E_bf_50, __E___E_bf_50_51, bf_add, __E_bf_52, bf_sub, 
		__E_bf_53, __E___E_bf_53_54, bf_mod, __E_bf_55, __E___E_bf_55_56, bf_mul, __E_bf_57, bf_div, __E_bf_58, __E___E_bf_58_59, 
		bf_nor, __E_bf_60, __E___E_bf_60_61, bf_xnor, __E_bf_62, __E___E_bf_62_63, bf_nand, __E_bf_64, __E___E_bf_64_65, bf_or, 
		__E_bf_66, bf_xor, __E_bf_67, bf_t, __E_bf_68, __E___E_bf_68_69, bf_f, __E_bf_70, __E___E_bf_70_71, bf_and, 
		__E_bf_72, __E___E_bf_72_73, bf_neg, __E_bf_74, bf_neg_oprnd, __E___E_bf_74_75, bf_and_nosep, __E_bf_76, bf_and_nosep_1st_oprnd, __E___E_bf_76_77, 
		bf_and_nosep_2nd_oprnd, __E___E_bf_76_78, ctn_neq, __E_constraint_79, ctnvar, ctn_eq, __E_constraint_80, ctn_gteq, __E_constraint_81, ctn_gt, 
		__E_constraint_82, ctn_lteq, __E_constraint_83, ctn_lt, __E_constraint_84, __E_ba_constant_85, source, __E_ba_constant_86, src_c, __E_source_87, 
		__E___E_source_87_88, __E___E_source_87_89, __E_src_c_90, __E_src_c_91, __E_variable_92, uconst, io_var, var_name, __E_variable_93, __E_var_name_94, 
		__E_var_name_95, __E_var_name_96, __E_var_name_97, __E_var_name_98, offset, chars, uconst_name, __E_uconst_99, __E___E_uconst_99_100, q_var, 
		__E_q_vars_101, __E_q_vars_102, __E_offsets_103, __E_offsets_104, integer, shift, __E_offset_105, __E_shift_106, __E___E_shift_106_107, __E_chars_108, 
		__E_chars_109, digits, __E_digits_110, __E_integer_111, type, subtype, __E_typed_112, comment, __E_comment_113, __E_comment_114, 
		__E_comment_115, cli, cli_command, __E_cli_116, __E_cli_117, __E_cli_118, quit_cmd, quit_sym, __E_cli_command_119, version_cmd, 
		version_sym, __E_cli_command_120, clear_cmd, clear_sym, __E_cli_command_121, help_cmd, __E_cli_command_122, help_sym, __E___E_cli_command_122_123, __E___E_cli_command_122_124, 
		help_arg, file_cmd, __E_cli_command_125, file_sym, q_string, valid_cmd, __E_cli_command_126, valid_sym, spec_arg, sat_cmd, 
		__E_cli_command_127, sat_sym, unsat_cmd, __E_cli_command_128, unsat_sym, solve_cmd, __E_cli_command_129, solve_sym, __E___E_cli_command_129_130, solve_options, 
		wff_cmd_arg, lgrs_cmd, __E_cli_command_131, lgrs_sym, run_cmd, __E_cli_command_132, run_sym, __E___E_cli_command_132_133, normalize_cmd, __E_cli_command_134, 
		normalize_sym, __E___E_cli_command_134_135, subst_cmd, __E_cli_command_136, subst_sym, __E___E_cli_command_136_137, nf_cmd_arg, inst_cmd, __E_cli_command_138, inst_sym, 
		__E___E_cli_command_138_139, inst_args, dnf_cmd, __E_cli_command_140, dnf_sym, cnf_cmd, __E_cli_command_141, cnf_sym, anf_cmd, __E_cli_command_142, 
		anf_sym, nnf_cmd, __E_cli_command_143, nnf_sym, pnf_cmd, __E_cli_command_144, pnf_sym, mnf_cmd, __E_cli_command_145, mnf_sym, 
		onf_cmd, __E_cli_command_146, onf_sym, onf_cmd_arg, qelim_cmd, __E_cli_command_147, qelim_sym, get_cmd, __E_cli_command_148, get_sym, 
		__E___E_cli_command_148_149, option_name, set_cmd, __E_cli_command_150, set_sym, __E___E_cli_command_150_151, option_value, enable_cmd, __E_cli_command_152, enable_sym, 
		disable_cmd, __E_cli_command_153, disable_sym, toggle_cmd, __E_cli_command_154, toggle_sym, def_list_cmd, __E_cli_command_155, def_sym, __E___E_cli_command_155_156, 
		def_print_cmd, __E_cli_command_157, def_rr_cmd, def_input_cmd, def_output_cmd, history_list_cmd, __E_cli_command_158, history_sym, __E___E_cli_command_158_159, history_print_cmd, 
		__E_cli_command_160, history, history_store_cmd, __E_cli_command_161, __E_solve_options_162, solve_option, __E_solve_options_163, __E_solve_option_164, solver_mode, solver_mode_minimum, 
		__E_solver_mode_165, solver_mode_maximum, __E_solver_mode_166, file_name, __E_file_name_167, __E_option_name_168, __E_option_value_169, bf_cmd_arg, abs_history_sym, rel_history_sym, 
		selection_sym, execute_sym, snf_sym, examples_sym, __E_help_arg_170, __E___E_help_arg_170_171, rel_history, __E_history_172, __E___E_history_172_173, history_id, 
		abs_history, __E_history_174, stream_def, fm_or_term, __N_0, __N_1, __N_2, __N_3, __N_4, __N_5, 
		__N_6, __N_7, __N_8, __N_9, __N_10, __N_11, __N_12, __N_13, __N_14, __N_15, 
		__N_16, __N_17, 
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
