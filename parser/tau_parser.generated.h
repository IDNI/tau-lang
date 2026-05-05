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
	"__E_wff_21", "__E___E_wff_21_22", "wff_F", "__E_wff_23", "wff_A", "__E_wff_24", "wff_E", "__E_wff_25", "wff_semantic_neg", "__E_wff_26", 
	"wff_conditional", "__E_wff_27", "wff_all", "__E_wff_28", "q_vars", "wff_ex", "__E_wff_29", "wff_ref", "constraint", "wff_imply", 
	"__E_wff_30", "wff_rimply", "__E_wff_31", "wff_equiv", "__E_wff_32", "wff_or", "__E_wff_33", "wff_xor", "__E_wff_34", "wff_and", 
	"__E_wff_35", "wff_neg", "__E_wff_36", "wff_t", "wff_f", "bf_interval", "__E_wff_37", "bf_eq", "__E_wff_38", "bf_neq", 
	"__E_wff_39", "bf_lt", "__E_wff_40", "bf_nlt", "__E_wff_41", "bf_lteq", "__E_wff_42", "bf_nlteq", "__E_wff_43", "bf_gt", 
	"__E_wff_44", "bf_ngt", "__E_wff_45", "bf_gteq", "__E_wff_46", "bf_ngteq", "__E_wff_47", "wff_U", "__E_wff_48", "wff_R", 
	"__E_wff_49", "wff_W", "__E_wff_50", "wff_S", "__E_wff_51", "wff_T", "__E_wff_52", "bf_parenthesis", "__E_bf_53", "ba_constant", 
	"variable", "bf_fall", "__E_bf_54", "bf_fex", "__E_bf_55", "bf_ref", "bf_cast", "__E_bf_56", "num", "bf_cast_oprnd", 
	"__E___E_bf_56_57", "bf_t", "bf_f", "bf_neg", "bf_func_app", "__E_bf_58", "func_sym", "func_args", "bf_shr", "__E_bf_59", 
	"__E___E_bf_59_60", "bf_shl", "__E_bf_61", "__E___E_bf_61_62", "bf_add", "__E_bf_63", "bf_sub", "__E_bf_64", "__E___E_bf_64_65", "bf_mod", 
	"__E_bf_66", "__E___E_bf_66_67", "bf_mul", "__E_bf_68", "bf_div", "__E_bf_69", "__E___E_bf_69_70", "bf_nor", "__E_bf_71", "__E___E_bf_71_72", 
	"bf_xnor", "__E_bf_73", "__E___E_bf_73_74", "bf_nand", "__E_bf_75", "__E___E_bf_75_76", "bf_or", "__E_bf_77", "bf_xor", "__E_bf_78", 
	"__E_bf_79", "__E___E_bf_79_80", "__E_bf_81", "__E___E_bf_81_82", "bf_and", "__E_bf_83", "__E___E_bf_83_84", "__E_bf_85", "bf_neg_oprnd", "__E___E_bf_85_86", 
	"bf_and_nosep", "__E_bf_87", "bf_and_nosep_1st_oprnd", "__E___E_bf_87_88", "bf_and_nosep_2nd_oprnd", "__E___E_bf_87_89", "chars", "__E_func_args_90", "__E_func_args_91", "func_mode", 
	"func_dynamic", "func_static", "func_sort_list", "type", "__E_func_sort_list_92", "__E_func_sort_list_93", "func_decl", "ctn_neq", "__E_constraint_94", "ctnvar", 
	"ctn_eq", "__E_constraint_95", "ctn_gteq", "__E_constraint_96", "ctn_gt", "__E_constraint_97", "ctn_lteq", "__E_constraint_98", "ctn_lt", "__E_constraint_99", 
	"__E_ba_constant_100", "source", "__E_ba_constant_101", "src_c", "__E_source_102", "__E___E_source_102_103", "__E___E_source_102_104", "__E_src_c_105", "__E_src_c_106", "__E_variable_107", 
	"uconst", "io_var", "var_name", "__E_variable_108", "__E_var_name_109", "__E_var_name_110", "__E_var_name_111", "__E_var_name_112", "__E_var_name_113", "offset", 
	"uconst_name", "__E_uconst_114", "__E___E_uconst_114_115", "q_var", "__E_q_vars_116", "__E_q_vars_117", "__E_offsets_118", "__E_offsets_119", "integer", "shift", 
	"__E_offset_120", "__E_shift_121", "__E___E_shift_121_122", "__E_chars_123", "__E_chars_124", "digits", "__E_digits_125", "__E_integer_126", "subtype", "__E_typed_127", 
	"comment", "__E_comment_128", "__E_comment_129", "__E_comment_130", "cli", "cli_command", "__E_cli_131", "__E_cli_132", "__E_cli_133", "quit_cmd", 
	"quit_sym", "__E_cli_command_134", "version_cmd", "version_sym", "__E_cli_command_135", "clear_cmd", "clear_sym", "__E_cli_command_136", "help_cmd", "__E_cli_command_137", 
	"help_sym", "__E___E_cli_command_137_138", "__E___E_cli_command_137_139", "help_arg", "file_cmd", "__E_cli_command_140", "file_sym", "q_string", "valid_cmd", "__E_cli_command_141", 
	"valid_sym", "spec_arg", "sat_cmd", "__E_cli_command_142", "sat_sym", "unsat_cmd", "__E_cli_command_143", "unsat_sym", "solve_cmd", "__E_cli_command_144", 
	"solve_sym", "__E___E_cli_command_144_145", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_146", "lgrs_sym", "run_cmd", "__E_cli_command_147", "run_sym", 
	"__E___E_cli_command_147_148", "ltl_cmd", "__E_cli_command_149", "ltl_sym", "func_cmd", "__E_cli_command_150", "func_sym_cmd", "normalize_cmd", "__E_cli_command_151", "normalize_sym", 
	"__E___E_cli_command_151_152", "subst_cmd", "__E_cli_command_153", "subst_sym", "__E___E_cli_command_153_154", "nf_cmd_arg", "inst_cmd", "__E_cli_command_155", "inst_sym", "__E___E_cli_command_155_156", 
	"inst_args", "dnf_cmd", "__E_cli_command_157", "dnf_sym", "cnf_cmd", "__E_cli_command_158", "cnf_sym", "nnf_cmd", "__E_cli_command_159", "nnf_sym", 
	"mnf_cmd", "__E_cli_command_160", "mnf_sym", "onf_cmd", "__E_cli_command_161", "onf_sym", "onf_cmd_arg", "qelim_cmd", "__E_cli_command_162", "qelim_sym", 
	"whatis_cmd", "__E_cli_command_163", "whatis_sym", "reset_cmd", "__E_cli_command_164", "reset_sym", "fragment_cmd", "__E_cli_command_165", "fragment_sym", "fragment_name", 
	"get_cmd", "__E_cli_command_166", "get_sym", "__E___E_cli_command_166_167", "option_name", "set_cmd", "__E_cli_command_168", "set_sym", "__E___E_cli_command_168_169", "option_value", 
	"enable_cmd", "__E_cli_command_170", "enable_sym", "disable_cmd", "__E_cli_command_171", "disable_sym", "toggle_cmd", "__E_cli_command_172", "toggle_sym", "def_list_cmd", 
	"__E_cli_command_173", "def_sym", "__E___E_cli_command_173_174", "def_print_cmd", "__E_cli_command_175", "def_rr_cmd", "def_input_cmd", "def_output_cmd", "history_list_cmd", "__E_cli_command_176", 
	"history_sym", "__E___E_cli_command_176_177", "history_print_cmd", "__E_cli_command_178", "history", "history_store_cmd", "__E_cli_command_179", "__E_solve_options_180", "solve_option", "__E_solve_options_181", 
	"__E_solve_option_182", "solver_mode", "solver_mode_minimum", "__E_solver_mode_183", "solver_mode_maximum", "__E_solver_mode_184", "file_name", "__E_file_name_185", "__E_option_name_186", "__E_option_value_187", 
	"__E_fragment_name_188", "fragment_ltl", "fragment_ctl_star", "bf_cmd_arg", "abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", "snf_sym", "examples_sym", 
	"__E_help_arg_189", "__E___E_help_arg_189_190", "rel_history", "__E_history_191", "__E___E_history_191_192", "history_id", "abs_history", "__E_history_193", "unreachable", "BDD_ID", 
	"stream_def", "fm_or_term", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", "__N_7", 
	"__N_8", "__N_9", "__N_10", "__N_11", "__N_12", "__N_13", "__N_14", "__N_15", "__N_16", "__N_17", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'i', 'n', 'o', 'u', 't', 
	'f', 'l', 'e', '(', ')', 'c', 's', 'a', 'b', 'k', 
	'r', ',', 'm', '<', '>', 'w', 'y', '[', ']', 'G', 
	'F', 'A', 'E', '-', '?', 'x', '|', '^', '&', '!', 
	'T', 'U', 'R', 'W', 'S', 'v', '+', '%', '*', '/', 
	'1', '0', '\'', 'd', '{', '}', '_', '$', '\t', '\n', 
	'\r', '#', 'q', 'h', 'p', 'g', 'z', '"', 
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
			30, 32, 40, 203, 209, 221, 232, 240, 248, 255,
			258, 364, 369, 406
		},
		.to_inline = {
			{ 15, 54, 15 },
			{ 25 },
			{ 31, 127, 31 },
			{ 139, 127, 31 },
			{ 188, 127, 31 },
			{ 190, 127, 184 },
			{ 192, 127, 31 },
			{ 194, 127, 31 },
			{ 196 },
			{ 223 },
			{ 230 },
			{ 230, 240 },
			{ 243 },
			{ 291 },
			{ 303 },
			{ 325 },
			{ 330 },
			{ 346 },
			{ 413 }
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
//G72:  __E___E_wff_21_22(61) => 'G'.
	p(NT(61), (T(28)));
//G73:  __E_wff_21(60)       => __E___E_wff_21_22(61) _(13) wff(15).
	p(NT(60), (NT(61)+NT(13)+NT(15)));
//G74:  wff_always(59)       => __E_wff_21(60).
	p(NT(59), (NT(60)));
//G75:  wff(15)              => wff_always(59).
	p(NT(15), (NT(59)));
//G76:  __E_wff_23(63)       => 'F' _(13) wff(15).
	p(NT(63), (T(29)+NT(13)+NT(15)));
//G77:  wff_F(62)            => __E_wff_23(63).
	p(NT(62), (NT(63)));
//G78:  wff(15)              => wff_F(62).
	p(NT(15), (NT(62)));
//G79:  __E_wff_24(65)       => 'A' _(13) wff(15).
	p(NT(65), (T(30)+NT(13)+NT(15)));
//G80:  wff_A(64)            => __E_wff_24(65).
	p(NT(64), (NT(65)));
//G81:  wff(15)              => wff_A(64).
	p(NT(15), (NT(64)));
//G82:  __E_wff_25(67)       => 'E' _(13) wff(15).
	p(NT(67), (T(31)+NT(13)+NT(15)));
//G83:  wff_E(66)            => __E_wff_25(67).
	p(NT(66), (NT(67)));
//G84:  wff(15)              => wff_E(66).
	p(NT(15), (NT(66)));
//G85:  __E_wff_26(69)       => '-' _(13) wff(15).
	p(NT(69), (T(32)+NT(13)+NT(15)));
//G86:  wff_semantic_neg(68) => __E_wff_26(69).
	p(NT(68), (NT(69)));
//G87:  wff(15)              => wff_semantic_neg(68).
	p(NT(15), (NT(68)));
//G88:  __E_wff_27(71)       => wff(15) _(13) '?' _(13) wff(15) _(13) ':' _(13) wff(15).
	p(NT(71), (NT(15)+NT(13)+T(33)+NT(13)+NT(15)+NT(13)+T(2)+NT(13)+NT(15)));
//G89:  wff_conditional(70)  => __E_wff_27(71).
	p(NT(70), (NT(71)));
//G90:  wff(15)              => wff_conditional(70).
	p(NT(15), (NT(70)));
//G91:  __E_wff_28(73)       => 'a' 'l' 'l' __(35) q_vars(74) __(35) wff(15).
	p(NT(73), (T(16)+T(10)+T(10)+NT(35)+NT(74)+NT(35)+NT(15)));
//G92:  wff_all(72)          => __E_wff_28(73).
	p(NT(72), (NT(73)));
//G93:  wff(15)              => wff_all(72).
	p(NT(15), (NT(72)));
//G94:  __E_wff_29(76)       => 'e' 'x' __(35) q_vars(74) __(35) wff(15).
	p(NT(76), (T(11)+T(34)+NT(35)+NT(74)+NT(35)+NT(15)));
//G95:  wff_ex(75)           => __E_wff_29(76).
	p(NT(75), (NT(76)));
//G96:  wff(15)              => wff_ex(75).
	p(NT(15), (NT(75)));
//G97:  wff_ref(77)          => ref(28).
	p(NT(77), (NT(28)));
//G98:  wff(15)              => wff_ref(77).
	p(NT(15), (NT(77)));
//G99:  wff(15)              => constraint(78).
	p(NT(15), (NT(78)));
//G100: __E_wff_30(80)       => wff(15) _(13) '-' '>' _(13) wff(15).
	p(NT(80), (NT(15)+NT(13)+T(32)+T(23)+NT(13)+NT(15)));
//G101: wff_imply(79)        => __E_wff_30(80).
	p(NT(79), (NT(80)));
//G102: wff(15)              => wff_imply(79).
	p(NT(15), (NT(79)));
//G103: __E_wff_31(82)       => wff(15) _(13) '<' '-' _(13) wff(15).
	p(NT(82), (NT(15)+NT(13)+T(22)+T(32)+NT(13)+NT(15)));
//G104: wff_rimply(81)       => __E_wff_31(82).
	p(NT(81), (NT(82)));
//G105: wff(15)              => wff_rimply(81).
	p(NT(15), (NT(81)));
//G106: __E_wff_32(84)       => wff(15) _(13) '<' '-' '>' _(13) wff(15).
	p(NT(84), (NT(15)+NT(13)+T(22)+T(32)+T(23)+NT(13)+NT(15)));
//G107: wff_equiv(83)        => __E_wff_32(84).
	p(NT(83), (NT(84)));
//G108: wff(15)              => wff_equiv(83).
	p(NT(15), (NT(83)));
//G109: __E_wff_33(86)       => wff(15) _(13) '|' '|' _(13) wff(15).
	p(NT(86), (NT(15)+NT(13)+T(35)+T(35)+NT(13)+NT(15)));
//G110: wff_or(85)           => __E_wff_33(86).
	p(NT(85), (NT(86)));
//G111: wff(15)              => wff_or(85).
	p(NT(15), (NT(85)));
//G112: __E_wff_34(88)       => wff(15) _(13) '^' '^' _(13) wff(15).
	p(NT(88), (NT(15)+NT(13)+T(36)+T(36)+NT(13)+NT(15)));
//G113: wff_xor(87)          => __E_wff_34(88).
	p(NT(87), (NT(88)));
//G114: wff(15)              => wff_xor(87).
	p(NT(15), (NT(87)));
//G115: __E_wff_35(90)       => wff(15) _(13) '&' '&' _(13) wff(15).
	p(NT(90), (NT(15)+NT(13)+T(37)+T(37)+NT(13)+NT(15)));
//G116: wff_and(89)          => __E_wff_35(90).
	p(NT(89), (NT(90)));
//G117: wff(15)              => wff_and(89).
	p(NT(15), (NT(89)));
//G118: __E_wff_36(92)       => '!' _(13) wff(15).
	p(NT(92), (T(38)+NT(13)+NT(15)));
//G119: wff_neg(91)          => __E_wff_36(92).
	p(NT(91), (NT(92)));
//G120: wff(15)              => wff_neg(91).
	p(NT(15), (NT(91)));
//G121: wff_t(93)            => 'T'.
	p(NT(93), (T(39)));
//G122: wff(15)              => wff_t(93).
	p(NT(15), (NT(93)));
//G123: wff_f(94)            => 'F'.
	p(NT(94), (T(29)));
//G124: wff(15)              => wff_f(94).
	p(NT(15), (NT(94)));
//G125: __E_wff_37(96)       => bf(31) _(13) '<' '=' _(13) bf(31) _(13) '<' '=' _(13) bf(31).
	p(NT(96), (NT(31)+NT(13)+T(22)+T(3)+NT(13)+NT(31)+NT(13)+T(22)+T(3)+NT(13)+NT(31)));
//G126: bf_interval(95)      => __E_wff_37(96).
	p(NT(95), (NT(96)));
//G127: wff(15)              => bf_interval(95).
	p(NT(15), (NT(95)));
//G128: __E_wff_38(98)       => bf(31) _(13) '=' _(13) bf(31).
	p(NT(98), (NT(31)+NT(13)+T(3)+NT(13)+NT(31)));
//G129: bf_eq(97)            => __E_wff_38(98).
	p(NT(97), (NT(98)));
//G130: wff(15)              => bf_eq(97).
	p(NT(15), (NT(97)));
//G131: __E_wff_39(100)      => bf(31) _(13) '!' '=' _(13) bf(31).
	p(NT(100), (NT(31)+NT(13)+T(38)+T(3)+NT(13)+NT(31)));
//G132: bf_neq(99)           => __E_wff_39(100).
	p(NT(99), (NT(100)));
//G133: wff(15)              => bf_neq(99).
	p(NT(15), (NT(99)));
//G134: __E_wff_40(102)      => bf(31) _(13) '<' _(13) bf(31).
	p(NT(102), (NT(31)+NT(13)+T(22)+NT(13)+NT(31)));
//G135: bf_lt(101)           => __E_wff_40(102).
	p(NT(101), (NT(102)));
//G136: wff(15)              => bf_lt(101).
	p(NT(15), (NT(101)));
//G137: __E_wff_41(104)      => bf(31) _(13) '!' '<' _(13) bf(31).
	p(NT(104), (NT(31)+NT(13)+T(38)+T(22)+NT(13)+NT(31)));
//G138: bf_nlt(103)          => __E_wff_41(104).
	p(NT(103), (NT(104)));
//G139: wff(15)              => bf_nlt(103).
	p(NT(15), (NT(103)));
//G140: __E_wff_42(106)      => bf(31) _(13) '<' '=' _(13) bf(31).
	p(NT(106), (NT(31)+NT(13)+T(22)+T(3)+NT(13)+NT(31)));
//G141: bf_lteq(105)         => __E_wff_42(106).
	p(NT(105), (NT(106)));
//G142: wff(15)              => bf_lteq(105).
	p(NT(15), (NT(105)));
//G143: __E_wff_43(108)      => bf(31) _(13) '!' '<' '=' _(13) bf(31).
	p(NT(108), (NT(31)+NT(13)+T(38)+T(22)+T(3)+NT(13)+NT(31)));
//G144: bf_nlteq(107)        => __E_wff_43(108).
	p(NT(107), (NT(108)));
//G145: wff(15)              => bf_nlteq(107).
	p(NT(15), (NT(107)));
//G146: __E_wff_44(110)      => bf(31) _(13) '>' _(13) bf(31).
	p(NT(110), (NT(31)+NT(13)+T(23)+NT(13)+NT(31)));
//G147: bf_gt(109)           => __E_wff_44(110).
	p(NT(109), (NT(110)));
//G148: wff(15)              => bf_gt(109).
	p(NT(15), (NT(109)));
//G149: __E_wff_45(112)      => bf(31) _(13) '!' '>' _(13) bf(31).
	p(NT(112), (NT(31)+NT(13)+T(38)+T(23)+NT(13)+NT(31)));
//G150: bf_ngt(111)          => __E_wff_45(112).
	p(NT(111), (NT(112)));
//G151: wff(15)              => bf_ngt(111).
	p(NT(15), (NT(111)));
//G152: __E_wff_46(114)      => bf(31) _(13) '>' '=' _(13) bf(31).
	p(NT(114), (NT(31)+NT(13)+T(23)+T(3)+NT(13)+NT(31)));
//G153: bf_gteq(113)         => __E_wff_46(114).
	p(NT(113), (NT(114)));
//G154: wff(15)              => bf_gteq(113).
	p(NT(15), (NT(113)));
//G155: __E_wff_47(116)      => bf(31) _(13) '!' '>' '=' _(13) bf(31).
	p(NT(116), (NT(31)+NT(13)+T(38)+T(23)+T(3)+NT(13)+NT(31)));
//G156: bf_ngteq(115)        => __E_wff_47(116).
	p(NT(115), (NT(116)));
//G157: wff(15)              => bf_ngteq(115).
	p(NT(15), (NT(115)));
//G158: __E_wff_48(118)      => wff(15) _(13) 'U' _(13) wff(15).
	p(NT(118), (NT(15)+NT(13)+T(40)+NT(13)+NT(15)));
//G159: wff_U(117)           => __E_wff_48(118).
	p(NT(117), (NT(118)));
//G160: wff(15)              => wff_U(117).
	p(NT(15), (NT(117)));
//G161: __E_wff_49(120)      => wff(15) _(13) 'R' _(13) wff(15).
	p(NT(120), (NT(15)+NT(13)+T(41)+NT(13)+NT(15)));
//G162: wff_R(119)           => __E_wff_49(120).
	p(NT(119), (NT(120)));
//G163: wff(15)              => wff_R(119).
	p(NT(15), (NT(119)));
//G164: __E_wff_50(122)      => wff(15) _(13) 'W' _(13) wff(15).
	p(NT(122), (NT(15)+NT(13)+T(42)+NT(13)+NT(15)));
//G165: wff_W(121)           => __E_wff_50(122).
	p(NT(121), (NT(122)));
//G166: wff(15)              => wff_W(121).
	p(NT(15), (NT(121)));
//G167: __E_wff_51(124)      => wff(15) _(13) 'S' _(13) wff(15).
	p(NT(124), (NT(15)+NT(13)+T(43)+NT(13)+NT(15)));
//G168: wff_S(123)           => __E_wff_51(124).
	p(NT(123), (NT(124)));
//G169: wff(15)              => wff_S(123).
	p(NT(15), (NT(123)));
//G170: __E_wff_52(126)      => wff(15) _(13) 'T' _(13) wff(15).
	p(NT(126), (NT(15)+NT(13)+T(39)+NT(13)+NT(15)));
//G171: wff_T(125)           => __E_wff_52(126).
	p(NT(125), (NT(126)));
//G172: wff(15)              => wff_T(125).
	p(NT(15), (NT(125)));
//G173: wff(15)              => capture(30).
	p(NT(15), (NT(30)));
//G174: __E_bf_53(128)       => '(' _(13) bf(31) _(13) ')'.
	p(NT(128), (T(12)+NT(13)+NT(31)+NT(13)+T(13)));
//G175: bf_parenthesis(127)  => __E_bf_53(128).
	p(NT(127), (NT(128)));
//G176: bf(31)               => bf_parenthesis(127).
	p(NT(31), (NT(127)));
//G177: bf(31)               => ba_constant(129).
	p(NT(31), (NT(129)));
//G178: bf(31)               => variable(130).
	p(NT(31), (NT(130)));
//G179: __E_bf_54(132)       => 'f' 'a' 'l' 'l' __(35) q_vars(74) __(35) bf(31).
	p(NT(132), (T(9)+T(16)+T(10)+T(10)+NT(35)+NT(74)+NT(35)+NT(31)));
//G180: bf_fall(131)         => __E_bf_54(132).
	p(NT(131), (NT(132)));
//G181: bf(31)               => bf_fall(131).
	p(NT(31), (NT(131)));
//G182: __E_bf_55(134)       => 'f' 'e' 'x' __(35) q_vars(74) __(35) bf(31).
	p(NT(134), (T(9)+T(11)+T(34)+NT(35)+NT(74)+NT(35)+NT(31)));
//G183: bf_fex(133)          => __E_bf_55(134).
	p(NT(133), (NT(134)));
//G184: bf(31)               => bf_fex(133).
	p(NT(31), (NT(133)));
//G185: bf_ref(135)          => ref(28).
	p(NT(135), (NT(28)));
//G186: bf(31)               => bf_ref(135).
	p(NT(31), (NT(135)));
//G187: __E___E_bf_56_57(140) => bf_parenthesis(127).
	p(NT(140), (NT(127)));
//G188: __E___E_bf_56_57(140) => ba_constant(129).
	p(NT(140), (NT(129)));
//G189: __E___E_bf_56_57(140) => variable(130).
	p(NT(140), (NT(130)));
//G190: __E___E_bf_56_57(140) => bf_ref(135).
	p(NT(140), (NT(135)));
//G191: __E___E_bf_56_57(140) => bf_t(141).
	p(NT(140), (NT(141)));
//G192: __E___E_bf_56_57(140) => bf_f(142).
	p(NT(140), (NT(142)));
//G193: __E___E_bf_56_57(140) => bf_neg(143).
	p(NT(140), (NT(143)));
//G194: __E___E_bf_56_57(140) => bf_cast(136).
	p(NT(140), (NT(136)));
//G195: __E___E_bf_56_57(140) => capture(30).
	p(NT(140), (NT(30)));
//G196: bf_cast_oprnd(139)   => __E___E_bf_56_57(140).
	p(NT(139), (NT(140)));
//G197: __E_bf_56(137)       => '(' _(13) 'b' 'v' _(13) '[' _(13) num(138) _(13) ']' _(13) ')' _(13) bf_cast_oprnd(139).
	p(NT(137), (T(12)+NT(13)+T(17)+T(44)+NT(13)+T(26)+NT(13)+NT(138)+NT(13)+T(27)+NT(13)+T(13)+NT(13)+NT(139)));
//G198: bf_cast(136)         => __E_bf_56(137).
	p(NT(136), (NT(137)));
//G199: bf(31)               => bf_cast(136).
	p(NT(31), (NT(136)));
//G200: __E_bf_58(145)       => func_sym(146) '(' _(13) func_args(147) _(13) ')'.
	p(NT(145), (NT(146)+T(12)+NT(13)+NT(147)+NT(13)+T(13)));
//G201: bf_func_app(144)     => __E_bf_58(145).
	p(NT(144), (NT(145)));
//G202: bf(31)               => bf_func_app(144).
	p(NT(31), (NT(144)));
//G203: __N_0(432)           => bf_shr(148).
	p(NT(432), (NT(148)));
//G204: __E___E_bf_59_60(150) => bf(31) & ~( __N_0(432) ).	 # conjunctive
	p(NT(150), (NT(31)) & ~(NT(432)));
//G205: __E_bf_59(149)       => bf(31) _(13) '>' '>' _(13) __E___E_bf_59_60(150).
	p(NT(149), (NT(31)+NT(13)+T(23)+T(23)+NT(13)+NT(150)));
//G206: bf_shr(148)          => __E_bf_59(149).
	p(NT(148), (NT(149)));
//G207: bf(31)               => bf_shr(148).
	p(NT(31), (NT(148)));
//G208: __N_1(433)           => bf_shl(151).
	p(NT(433), (NT(151)));
//G209: __E___E_bf_61_62(153) => bf(31) & ~( __N_1(433) ).	 # conjunctive
	p(NT(153), (NT(31)) & ~(NT(433)));
//G210: __E_bf_61(152)       => bf(31) _(13) '<' '<' _(13) __E___E_bf_61_62(153).
	p(NT(152), (NT(31)+NT(13)+T(22)+T(22)+NT(13)+NT(153)));
//G211: bf_shl(151)          => __E_bf_61(152).
	p(NT(151), (NT(152)));
//G212: bf(31)               => bf_shl(151).
	p(NT(31), (NT(151)));
//G213: __E_bf_63(155)       => bf(31) _(13) '+' _(13) bf(31).
	p(NT(155), (NT(31)+NT(13)+T(45)+NT(13)+NT(31)));
//G214: bf_add(154)          => __E_bf_63(155).
	p(NT(154), (NT(155)));
//G215: bf(31)               => bf_add(154).
	p(NT(31), (NT(154)));
//G216: __N_2(434)           => bf_sub(156).
	p(NT(434), (NT(156)));
//G217: __E___E_bf_64_65(158) => bf(31) & ~( __N_2(434) ).	 # conjunctive
	p(NT(158), (NT(31)) & ~(NT(434)));
//G218: __E_bf_64(157)       => bf(31) _(13) '-' _(13) __E___E_bf_64_65(158).
	p(NT(157), (NT(31)+NT(13)+T(32)+NT(13)+NT(158)));
//G219: bf_sub(156)          => __E_bf_64(157).
	p(NT(156), (NT(157)));
//G220: bf(31)               => bf_sub(156).
	p(NT(31), (NT(156)));
//G221: __N_3(435)           => bf_mod(159).
	p(NT(435), (NT(159)));
//G222: __E___E_bf_66_67(161) => bf(31) & ~( __N_3(435) ).	 # conjunctive
	p(NT(161), (NT(31)) & ~(NT(435)));
//G223: __E_bf_66(160)       => bf(31) _(13) '%' _(13) __E___E_bf_66_67(161).
	p(NT(160), (NT(31)+NT(13)+T(46)+NT(13)+NT(161)));
//G224: bf_mod(159)          => __E_bf_66(160).
	p(NT(159), (NT(160)));
//G225: bf(31)               => bf_mod(159).
	p(NT(31), (NT(159)));
//G226: __E_bf_68(163)       => bf(31) _(13) '*' _(13) bf(31).
	p(NT(163), (NT(31)+NT(13)+T(47)+NT(13)+NT(31)));
//G227: bf_mul(162)          => __E_bf_68(163).
	p(NT(162), (NT(163)));
//G228: bf(31)               => bf_mul(162).
	p(NT(31), (NT(162)));
//G229: __N_4(436)           => bf_div(164).
	p(NT(436), (NT(164)));
//G230: __E___E_bf_69_70(166) => bf(31) & ~( __N_4(436) ).	 # conjunctive
	p(NT(166), (NT(31)) & ~(NT(436)));
//G231: __E_bf_69(165)       => bf(31) _(13) '/' _(13) __E___E_bf_69_70(166).
	p(NT(165), (NT(31)+NT(13)+T(48)+NT(13)+NT(166)));
//G232: bf_div(164)          => __E_bf_69(165).
	p(NT(164), (NT(165)));
//G233: bf(31)               => bf_div(164).
	p(NT(31), (NT(164)));
//G234: __N_5(437)           => bf_nor(167).
	p(NT(437), (NT(167)));
//G235: __E___E_bf_71_72(169) => bf(31) & ~( __N_5(437) ).	 # conjunctive
	p(NT(169), (NT(31)) & ~(NT(437)));
//G236: __E_bf_71(168)       => bf(31) _(13) '!' '|' _(13) __E___E_bf_71_72(169).
	p(NT(168), (NT(31)+NT(13)+T(38)+T(35)+NT(13)+NT(169)));
//G237: bf_nor(167)          => __E_bf_71(168).
	p(NT(167), (NT(168)));
//G238: bf(31)               => bf_nor(167).
	p(NT(31), (NT(167)));
//G239: __N_6(438)           => bf_xnor(170).
	p(NT(438), (NT(170)));
//G240: __E___E_bf_73_74(172) => bf(31) & ~( __N_6(438) ).	 # conjunctive
	p(NT(172), (NT(31)) & ~(NT(438)));
//G241: __E_bf_73(171)       => bf(31) _(13) '!' '^' _(13) __E___E_bf_73_74(172).
	p(NT(171), (NT(31)+NT(13)+T(38)+T(36)+NT(13)+NT(172)));
//G242: bf_xnor(170)         => __E_bf_73(171).
	p(NT(170), (NT(171)));
//G243: bf(31)               => bf_xnor(170).
	p(NT(31), (NT(170)));
//G244: __N_7(439)           => bf_nand(173).
	p(NT(439), (NT(173)));
//G245: __E___E_bf_75_76(175) => bf(31) & ~( __N_7(439) ).	 # conjunctive
	p(NT(175), (NT(31)) & ~(NT(439)));
//G246: __E_bf_75(174)       => bf(31) _(13) '!' '&' _(13) __E___E_bf_75_76(175).
	p(NT(174), (NT(31)+NT(13)+T(38)+T(37)+NT(13)+NT(175)));
//G247: bf_nand(173)         => __E_bf_75(174).
	p(NT(173), (NT(174)));
//G248: bf(31)               => bf_nand(173).
	p(NT(31), (NT(173)));
//G249: __E_bf_77(177)       => bf(31) _(13) '|' _(13) bf(31).
	p(NT(177), (NT(31)+NT(13)+T(35)+NT(13)+NT(31)));
//G250: bf_or(176)           => __E_bf_77(177).
	p(NT(176), (NT(177)));
//G251: bf(31)               => bf_or(176).
	p(NT(31), (NT(176)));
//G252: __E_bf_78(179)       => bf(31) _(13) '^' _(13) bf(31).
	p(NT(179), (NT(31)+NT(13)+T(36)+NT(13)+NT(31)));
//G253: bf_xor(178)          => __E_bf_78(179).
	p(NT(178), (NT(179)));
//G254: bf(31)               => bf_xor(178).
	p(NT(31), (NT(178)));
//G255: __E___E_bf_79_80(181) => typed(34).
	p(NT(181), (NT(34)));
//G256: __E___E_bf_79_80(181) => null.
	p(NT(181), (nul));
//G257: __E_bf_79(180)       => '1' __E___E_bf_79_80(181).
	p(NT(180), (T(49)+NT(181)));
//G258: bf_t(141)            => __E_bf_79(180).
	p(NT(141), (NT(180)));
//G259: bf(31)               => bf_t(141).
	p(NT(31), (NT(141)));
//G260: __E___E_bf_81_82(183) => typed(34).
	p(NT(183), (NT(34)));
//G261: __E___E_bf_81_82(183) => null.
	p(NT(183), (nul));
//G262: __E_bf_81(182)       => '0' __E___E_bf_81_82(183).
	p(NT(182), (T(50)+NT(183)));
//G263: bf_f(142)            => __E_bf_81(182).
	p(NT(142), (NT(182)));
//G264: bf(31)               => bf_f(142).
	p(NT(31), (NT(142)));
//G265: __E___E_bf_83_84(186) => _(13) '&' _(13).
	p(NT(186), (NT(13)+T(37)+NT(13)));
//G266: __E___E_bf_83_84(186) => __(35).
	p(NT(186), (NT(35)));
//G267: __E_bf_83(185)       => bf(31) __E___E_bf_83_84(186) bf(31).
	p(NT(185), (NT(31)+NT(186)+NT(31)));
//G268: bf_and(184)          => __E_bf_83(185).
	p(NT(184), (NT(185)));
//G269: bf(31)               => bf_and(184).
	p(NT(31), (NT(184)));
//G270: __E___E_bf_85_86(189) => bf_parenthesis(127).
	p(NT(189), (NT(127)));
//G271: __E___E_bf_85_86(189) => ba_constant(129).
	p(NT(189), (NT(129)));
//G272: __E___E_bf_85_86(189) => variable(130).
	p(NT(189), (NT(130)));
//G273: __E___E_bf_85_86(189) => bf_ref(135).
	p(NT(189), (NT(135)));
//G274: __E___E_bf_85_86(189) => bf_t(141).
	p(NT(189), (NT(141)));
//G275: __E___E_bf_85_86(189) => bf_f(142).
	p(NT(189), (NT(142)));
//G276: __E___E_bf_85_86(189) => bf_neg(143).
	p(NT(189), (NT(143)));
//G277: __E___E_bf_85_86(189) => capture(30).
	p(NT(189), (NT(30)));
//G278: bf_neg_oprnd(188)    => __E___E_bf_85_86(189).
	p(NT(188), (NT(189)));
//G279: __E_bf_85(187)       => bf_neg_oprnd(188) _(13) '\''.
	p(NT(187), (NT(188)+NT(13)+T(51)));
//G280: bf_neg(143)          => __E_bf_85(187).
	p(NT(143), (NT(187)));
//G281: bf(31)               => bf_neg(143).
	p(NT(31), (NT(143)));
//G282: __E___E_bf_87_88(193) => bf_parenthesis(127).
	p(NT(193), (NT(127)));
//G283: __E___E_bf_87_88(193) => variable(130).
	p(NT(193), (NT(130)));
//G284: __E___E_bf_87_88(193) => bf_ref(135).
	p(NT(193), (NT(135)));
//G285: __E___E_bf_87_88(193) => bf_neg(143).
	p(NT(193), (NT(143)));
//G286: __E___E_bf_87_88(193) => bf_and_nosep(190).
	p(NT(193), (NT(190)));
//G287: __E___E_bf_87_88(193) => capture(30).
	p(NT(193), (NT(30)));
//G288: bf_and_nosep_1st_oprnd(192) => __E___E_bf_87_88(193).
	p(NT(192), (NT(193)));
//G289: __E___E_bf_87_89(195) => bf_parenthesis(127).
	p(NT(195), (NT(127)));
//G290: __E___E_bf_87_89(195) => ba_constant(129).
	p(NT(195), (NT(129)));
//G291: __E___E_bf_87_89(195) => variable(130).
	p(NT(195), (NT(130)));
//G292: __E___E_bf_87_89(195) => bf_ref(135).
	p(NT(195), (NT(135)));
//G293: __E___E_bf_87_89(195) => bf_neg(143).
	p(NT(195), (NT(143)));
//G294: __E___E_bf_87_89(195) => capture(30).
	p(NT(195), (NT(30)));
//G295: bf_and_nosep_2nd_oprnd(194) => __E___E_bf_87_89(195).
	p(NT(194), (NT(195)));
//G296: __E_bf_87(191)       => bf_and_nosep_1st_oprnd(192) bf_and_nosep_2nd_oprnd(194).
	p(NT(191), (NT(192)+NT(194)));
//G297: bf_and_nosep(190)    => __E_bf_87(191).
	p(NT(190), (NT(191)));
//G298: bf(31)               => bf_and_nosep(190).
	p(NT(31), (NT(190)));
//G299: bf(31)               => capture(30).
	p(NT(31), (NT(30)));
//G300: func_sym(146)        => chars(196).
	p(NT(146), (NT(196)));
//G301: __E_func_args_90(197) => _(13) ',' _(13) bf(31).
	p(NT(197), (NT(13)+T(20)+NT(13)+NT(31)));
//G302: __E_func_args_91(198) => null.
	p(NT(198), (nul));
//G303: __E_func_args_91(198) => __E_func_args_90(197) __E_func_args_91(198).
	p(NT(198), (NT(197)+NT(198)));
//G304: func_args(147)       => bf(31) __E_func_args_91(198).
	p(NT(147), (NT(31)+NT(198)));
//G305: func_dynamic(200)    => 'd' 'y' 'n' 'a' 'm' 'i' 'c'.
	p(NT(200), (T(52)+T(25)+T(5)+T(16)+T(21)+T(4)+T(14)));
//G306: func_mode(199)       => func_dynamic(200).
	p(NT(199), (NT(200)));
//G307: func_static(201)     => 's' 't' 'a' 't' 'i' 'c'.
	p(NT(201), (T(15)+T(8)+T(16)+T(8)+T(4)+T(14)));
//G308: func_mode(199)       => func_static(201).
	p(NT(199), (NT(201)));
//G309: __E_func_sort_list_92(204) => _(13) ',' _(13) type(203).
	p(NT(204), (NT(13)+T(20)+NT(13)+NT(203)));
//G310: __E_func_sort_list_93(205) => null.
	p(NT(205), (nul));
//G311: __E_func_sort_list_93(205) => __E_func_sort_list_92(204) __E_func_sort_list_93(205).
	p(NT(205), (NT(204)+NT(205)));
//G312: func_sort_list(202)  => type(203) __E_func_sort_list_93(205).
	p(NT(202), (NT(203)+NT(205)));
//G313: func_decl(206)       => func_mode(199) __(35) func_sym(146) _(13) ':' _(13) func_sort_list(202) _(13) '-' '>' _(13) type(203).
	p(NT(206), (NT(199)+NT(35)+NT(146)+NT(13)+T(2)+NT(13)+NT(202)+NT(13)+T(32)+T(23)+NT(13)+NT(203)));
//G314: __E_constraint_94(208) => '[' ctnvar(209) _(13) '!' '=' _(13) num(138) ']'.
	p(NT(208), (T(26)+NT(209)+NT(13)+T(38)+T(3)+NT(13)+NT(138)+T(27)));
//G315: __E_constraint_94(208) => '[' num(138) _(13) '!' '=' _(13) ctnvar(209) ']'.
	p(NT(208), (T(26)+NT(138)+NT(13)+T(38)+T(3)+NT(13)+NT(209)+T(27)));
//G316: ctn_neq(207)         => __E_constraint_94(208).
	p(NT(207), (NT(208)));
//G317: constraint(78)       => ctn_neq(207).
	p(NT(78), (NT(207)));
//G318: __E_constraint_95(211) => '[' ctnvar(209) _(13) '=' _(13) num(138) ']'.
	p(NT(211), (T(26)+NT(209)+NT(13)+T(3)+NT(13)+NT(138)+T(27)));
//G319: __E_constraint_95(211) => '[' num(138) _(13) '=' _(13) ctnvar(209) ']'.
	p(NT(211), (T(26)+NT(138)+NT(13)+T(3)+NT(13)+NT(209)+T(27)));
//G320: ctn_eq(210)          => __E_constraint_95(211).
	p(NT(210), (NT(211)));
//G321: constraint(78)       => ctn_eq(210).
	p(NT(78), (NT(210)));
//G322: __E_constraint_96(213) => '[' ctnvar(209) _(13) '>' '=' _(13) num(138) ']'.
	p(NT(213), (T(26)+NT(209)+NT(13)+T(23)+T(3)+NT(13)+NT(138)+T(27)));
//G323: __E_constraint_96(213) => '[' num(138) _(13) '>' '=' _(13) ctnvar(209) ']'.
	p(NT(213), (T(26)+NT(138)+NT(13)+T(23)+T(3)+NT(13)+NT(209)+T(27)));
//G324: ctn_gteq(212)        => __E_constraint_96(213).
	p(NT(212), (NT(213)));
//G325: constraint(78)       => ctn_gteq(212).
	p(NT(78), (NT(212)));
//G326: __E_constraint_97(215) => '[' ctnvar(209) _(13) '>' _(13) num(138) ']'.
	p(NT(215), (T(26)+NT(209)+NT(13)+T(23)+NT(13)+NT(138)+T(27)));
//G327: __E_constraint_97(215) => '[' num(138) _(13) '>' _(13) ctnvar(209) ']'.
	p(NT(215), (T(26)+NT(138)+NT(13)+T(23)+NT(13)+NT(209)+T(27)));
//G328: ctn_gt(214)          => __E_constraint_97(215).
	p(NT(214), (NT(215)));
//G329: constraint(78)       => ctn_gt(214).
	p(NT(78), (NT(214)));
//G330: __E_constraint_98(217) => '[' ctnvar(209) _(13) '<' '=' _(13) num(138) ']'.
	p(NT(217), (T(26)+NT(209)+NT(13)+T(22)+T(3)+NT(13)+NT(138)+T(27)));
//G331: __E_constraint_98(217) => '[' num(138) _(13) '<' '=' _(13) ctnvar(209) ']'.
	p(NT(217), (T(26)+NT(138)+NT(13)+T(22)+T(3)+NT(13)+NT(209)+T(27)));
//G332: ctn_lteq(216)        => __E_constraint_98(217).
	p(NT(216), (NT(217)));
//G333: constraint(78)       => ctn_lteq(216).
	p(NT(78), (NT(216)));
//G334: __E_constraint_99(219) => '[' ctnvar(209) _(13) '<' _(13) num(138) ']'.
	p(NT(219), (T(26)+NT(209)+NT(13)+T(22)+NT(13)+NT(138)+T(27)));
//G335: __E_constraint_99(219) => '[' num(138) _(13) '<' _(13) ctnvar(209) ']'.
	p(NT(219), (T(26)+NT(138)+NT(13)+T(22)+NT(13)+NT(209)+T(27)));
//G336: ctn_lt(218)          => __E_constraint_99(219).
	p(NT(218), (NT(219)));
//G337: constraint(78)       => ctn_lt(218).
	p(NT(78), (NT(218)));
//G338: __E_ba_constant_100(220) => capture(30).
	p(NT(220), (NT(30)));
//G339: __E_ba_constant_100(220) => source(221).
	p(NT(220), (NT(221)));
//G340: __E_ba_constant_101(222) => typed(34).
	p(NT(222), (NT(34)));
//G341: __E_ba_constant_101(222) => null.
	p(NT(222), (nul));
//G342: ba_constant(129)     => '{' _(13) __E_ba_constant_100(220) _(13) '}' __E_ba_constant_101(222).
	p(NT(129), (T(53)+NT(13)+NT(220)+NT(13)+T(54)+NT(222)));
//G343: __E___E_source_102_103(225) => src_c(223).
	p(NT(225), (NT(223)));
//G344: __E___E_source_102_103(225) => space(2).
	p(NT(225), (NT(2)));
//G345: __E___E_source_102_104(226) => null.
	p(NT(226), (nul));
//G346: __E___E_source_102_104(226) => __E___E_source_102_103(225) __E___E_source_102_104(226).
	p(NT(226), (NT(225)+NT(226)));
//G347: __E_source_102(224)  => __E___E_source_102_104(226) src_c(223).
	p(NT(224), (NT(226)+NT(223)));
//G348: __E_source_102(224)  => null.
	p(NT(224), (nul));
//G349: source(221)          => src_c(223) __E_source_102(224).
	p(NT(221), (NT(223)+NT(224)));
//G350: src_c(223)           => alnum(6).
	p(NT(223), (NT(6)));
//G351: __N_8(440)           => '{'.
	p(NT(440), (T(53)));
//G352: __N_9(441)           => '}'.
	p(NT(441), (T(54)));
//G353: src_c(223)           => ~( __N_8(440) ) & ~( __N_9(441) ) & punct(7).	 # conjunctive
	p(NT(223), ~(NT(440)) & ~(NT(441)) & (NT(7)));
//G354: __E_src_c_105(227)   => src_c(223).
	p(NT(227), (NT(223)));
//G355: __E_src_c_105(227)   => space(2).
	p(NT(227), (NT(2)));
//G356: __E_src_c_106(228)   => null.
	p(NT(228), (nul));
//G357: __E_src_c_106(228)   => __E_src_c_105(227) __E_src_c_106(228).
	p(NT(228), (NT(227)+NT(228)));
//G358: src_c(223)           => '{' __E_src_c_106(228) '}'.
	p(NT(223), (T(53)+NT(228)+T(54)));
//G359: __E_variable_107(229) => uconst(230).
	p(NT(229), (NT(230)));
//G360: __E_variable_107(229) => io_var(231).
	p(NT(229), (NT(231)));
//G361: __E_variable_107(229) => var_name(232).
	p(NT(229), (NT(232)));
//G362: __E_variable_108(233) => typed(34).
	p(NT(233), (NT(34)));
//G363: __E_variable_108(233) => null.
	p(NT(233), (nul));
//G364: variable(130)        => __E_variable_107(229) __E_variable_108(233).
	p(NT(130), (NT(229)+NT(233)));
//G365: __N_10(442)          => 'F'.
	p(NT(442), (T(29)));
//G366: __N_11(443)          => 'T'.
	p(NT(443), (T(39)));
//G367: __E_var_name_109(234) => ~( __N_10(442) ) & ~( __N_11(443) ) & alpha(5).	 # conjunctive
	p(NT(234), ~(NT(442)) & ~(NT(443)) & (NT(5)));
//G368: __E_var_name_110(235) => null.
	p(NT(235), (nul));
//G369: __E_var_name_110(235) => digit(3) __E_var_name_110(235).
	p(NT(235), (NT(3)+NT(235)));
//G370: var_name(232)        => __E_var_name_109(234) __E_var_name_110(235).	 # guarded: charvar
	p(NT(232), (NT(234)+NT(235)));
	p.back().guard = "charvar";
//G371: __N_12(444)          => 'F'.
	p(NT(444), (T(29)));
//G372: __N_13(445)          => 'T'.
	p(NT(445), (T(39)));
//G373: __E_var_name_111(236) => ~( __N_12(444) ) & ~( __N_13(445) ) & alpha(5).	 # conjunctive
	p(NT(236), ~(NT(444)) & ~(NT(445)) & (NT(5)));
//G374: __E_var_name_112(237) => alnum(6).
	p(NT(237), (NT(6)));
//G375: __E_var_name_112(237) => '_'.
	p(NT(237), (T(55)));
//G376: __E_var_name_113(238) => null.
	p(NT(238), (nul));
//G377: __E_var_name_113(238) => __E_var_name_112(237) __E_var_name_113(238).
	p(NT(238), (NT(237)+NT(238)));
//G378: var_name(232)        => __E_var_name_111(236) __E_var_name_113(238).	 # guarded: var
	p(NT(232), (NT(236)+NT(238)));
	p.back().guard = "var";
//G379: io_var(231)          => io_var_name(32) '[' offset(239) ']'.
	p(NT(231), (NT(32)+T(26)+NT(239)+T(27)));
//G380: io_var_name(32)      => chars(196).
	p(NT(32), (NT(196)));
//G381: __E___E_uconst_114_115(242) => chars(196) _(13).
	p(NT(242), (NT(196)+NT(13)));
//G382: __E___E_uconst_114_115(242) => null.
	p(NT(242), (nul));
//G383: __E_uconst_114(241)  => __E___E_uconst_114_115(242) ':' _(13) chars(196).
	p(NT(241), (NT(242)+T(2)+NT(13)+NT(196)));
//G384: uconst_name(240)     => __E_uconst_114(241).
	p(NT(240), (NT(241)));
//G385: uconst(230)          => '<' _(13) uconst_name(240) _(13) '>'.
	p(NT(230), (T(22)+NT(13)+NT(240)+NT(13)+T(23)));
//G386: __E_q_vars_116(244)  => _(13) ',' _(13) q_var(243).
	p(NT(244), (NT(13)+T(20)+NT(13)+NT(243)));
//G387: __E_q_vars_117(245)  => null.
	p(NT(245), (nul));
//G388: __E_q_vars_117(245)  => __E_q_vars_116(244) __E_q_vars_117(245).
	p(NT(245), (NT(244)+NT(245)));
//G389: q_vars(74)           => q_var(243) __E_q_vars_117(245).
	p(NT(74), (NT(243)+NT(245)));
//G390: q_var(243)           => capture(30).
	p(NT(243), (NT(30)));
//G391: __N_14(446)          => uconst(230).
	p(NT(446), (NT(230)));
//G392: q_var(243)           => variable(130) & ~( __N_14(446) ).	 # conjunctive
	p(NT(243), (NT(130)) & ~(NT(446)));
//G393: ctnvar(209)          => chars(196).
	p(NT(209), (NT(196)));
//G394: __E_offsets_118(246) => _(13) ',' _(13) offset(239).
	p(NT(246), (NT(13)+T(20)+NT(13)+NT(239)));
//G395: __E_offsets_119(247) => null.
	p(NT(247), (nul));
//G396: __E_offsets_119(247) => __E_offsets_118(246) __E_offsets_119(247).
	p(NT(247), (NT(246)+NT(247)));
//G397: offsets(42)          => '[' _(13) offset(239) __E_offsets_119(247) _(13) ']'.
	p(NT(42), (T(26)+NT(13)+NT(239)+NT(247)+NT(13)+T(27)));
//G398: offset(239)          => integer(248).
	p(NT(239), (NT(248)));
//G399: offset(239)          => capture(30).
	p(NT(239), (NT(30)));
//G400: offset(239)          => shift(249).
	p(NT(239), (NT(249)));
//G401: __N_15(447)          => io_var(231).
	p(NT(447), (NT(231)));
//G402: __E_offset_120(250)  => variable(130) & ~( __N_15(447) ).	 # conjunctive
	p(NT(250), (NT(130)) & ~(NT(447)));
//G403: offset(239)          => __E_offset_120(250).
	p(NT(239), (NT(250)));
//G404: __E_shift_121(251)   => capture(30).
	p(NT(251), (NT(30)));
//G405: __N_16(448)          => io_var(231).
	p(NT(448), (NT(231)));
//G406: __E___E_shift_121_122(252) => variable(130) & ~( __N_16(448) ).	 # conjunctive
	p(NT(252), (NT(130)) & ~(NT(448)));
//G407: __E_shift_121(251)   => __E___E_shift_121_122(252).
	p(NT(251), (NT(252)));
//G408: shift(249)           => __E_shift_121(251) _(13) '-' _(13) num(138).
	p(NT(249), (NT(251)+NT(13)+T(32)+NT(13)+NT(138)));
//G409: __E_chars_123(253)   => alnum(6).
	p(NT(253), (NT(6)));
//G410: __E_chars_123(253)   => '_'.
	p(NT(253), (T(55)));
//G411: __E_chars_124(254)   => null.
	p(NT(254), (nul));
//G412: __E_chars_124(254)   => __E_chars_123(253) __E_chars_124(254).
	p(NT(254), (NT(253)+NT(254)));
//G413: chars(196)           => alpha(5) __E_chars_124(254).
	p(NT(196), (NT(5)+NT(254)));
//G414: __E_digits_125(256)  => digit(3).
	p(NT(256), (NT(3)));
//G415: __E_digits_125(256)  => digit(3) __E_digits_125(256).
	p(NT(256), (NT(3)+NT(256)));
//G416: digits(255)          => __E_digits_125(256).
	p(NT(255), (NT(256)));
//G417: num(138)             => digits(255).
	p(NT(138), (NT(255)));
//G418: __E_integer_126(257) => '-'.
	p(NT(257), (T(32)));
//G419: __E_integer_126(257) => null.
	p(NT(257), (nul));
//G420: integer(248)         => __E_integer_126(257) _(13) digits(255).
	p(NT(248), (NT(257)+NT(13)+NT(255)));
//G421: sym(40)              => chars(196).
	p(NT(40), (NT(196)));
//G422: capture(30)          => '$' chars(196).
	p(NT(30), (T(56)+NT(196)));
//G423: typed(34)            => _(13) ':' _(13) type(203) _(13) '[' _(13) subtype(258) _(13) ']'.
	p(NT(34), (NT(13)+T(2)+NT(13)+NT(203)+NT(13)+T(26)+NT(13)+NT(258)+NT(13)+T(27)));
//G424: __N_17(449)          => 'b' 'v'.
	p(NT(449), (T(17)+T(44)));
//G425: __E_typed_127(259)   => ~( __N_17(449) ) & type(203).	 # conjunctive
	p(NT(259), ~(NT(449)) & (NT(203)));
//G426: typed(34)            => _(13) ':' _(13) __E_typed_127(259).
	p(NT(34), (NT(13)+T(2)+NT(13)+NT(259)));
//G427: type(203)            => chars(196).
	p(NT(203), (NT(196)));
//G428: subtype(258)         => num(138).
	p(NT(258), (NT(138)));
//G429: __E_comment_128(261) => printable(8).
	p(NT(261), (NT(8)));
//G430: __E_comment_128(261) => '\t'.
	p(NT(261), (T(57)));
//G431: __E_comment_129(262) => null.
	p(NT(262), (nul));
//G432: __E_comment_129(262) => __E_comment_128(261) __E_comment_129(262).
	p(NT(262), (NT(261)+NT(262)));
//G433: __E_comment_130(263) => '\n'.
	p(NT(263), (T(58)));
//G434: __E_comment_130(263) => '\r'.
	p(NT(263), (T(59)));
//G435: __E_comment_130(263) => eof(1).
	p(NT(263), (NT(1)));
//G436: comment(260)         => '#' __E_comment_129(262) __E_comment_130(263).
	p(NT(260), (T(60)+NT(262)+NT(263)));
//G437: __(35)               => space(2).
	p(NT(35), (NT(2)));
//G438: __(35)               => comment(260).
	p(NT(35), (NT(260)));
//G439: __(35)               => __(35) space(2).
	p(NT(35), (NT(35)+NT(2)));
//G440: __(35)               => __(35) comment(260).
	p(NT(35), (NT(35)+NT(260)));
//G441: _(13)                => __(35).
	p(NT(13), (NT(35)));
//G442: _(13)                => null.
	p(NT(13), (nul));
//G443: cli(264)             => _(13).
	p(NT(264), (NT(13)));
//G444: __E_cli_131(266)     => '.' _(13) cli_command(265) _(13).
	p(NT(266), (T(1)+NT(13)+NT(265)+NT(13)));
//G445: __E_cli_132(267)     => null.
	p(NT(267), (nul));
//G446: __E_cli_132(267)     => __E_cli_131(266) __E_cli_132(267).
	p(NT(267), (NT(266)+NT(267)));
//G447: __E_cli_133(268)     => '.' _(13).
	p(NT(268), (T(1)+NT(13)));
//G448: __E_cli_133(268)     => null.
	p(NT(268), (nul));
//G449: cli(264)             => _(13) cli_command(265) _(13) __E_cli_132(267) __E_cli_133(268).
	p(NT(264), (NT(13)+NT(265)+NT(13)+NT(267)+NT(268)));
//G450: __E_cli_command_134(271) => 'q'.
	p(NT(271), (T(61)));
//G451: __E_cli_command_134(271) => 'q' 'u' 'i' 't'.
	p(NT(271), (T(61)+T(7)+T(4)+T(8)));
//G452: quit_sym(270)        => __E_cli_command_134(271).
	p(NT(270), (NT(271)));
//G453: quit_cmd(269)        => quit_sym(270).
	p(NT(269), (NT(270)));
//G454: cli_command(265)     => quit_cmd(269).
	p(NT(265), (NT(269)));
//G455: __E_cli_command_135(274) => 'v'.
	p(NT(274), (T(44)));
//G456: __E_cli_command_135(274) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(274), (T(44)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G457: version_sym(273)     => __E_cli_command_135(274).
	p(NT(273), (NT(274)));
//G458: version_cmd(272)     => version_sym(273).
	p(NT(272), (NT(273)));
//G459: cli_command(265)     => version_cmd(272).
	p(NT(265), (NT(272)));
//G460: __E_cli_command_136(277) => 'c'.
	p(NT(277), (T(14)));
//G461: __E_cli_command_136(277) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(277), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G462: clear_sym(276)       => __E_cli_command_136(277).
	p(NT(276), (NT(277)));
//G463: clear_cmd(275)       => clear_sym(276).
	p(NT(275), (NT(276)));
//G464: cli_command(265)     => clear_cmd(275).
	p(NT(265), (NT(275)));
//G465: __E___E_cli_command_137_138(281) => 'h'.
	p(NT(281), (T(62)));
//G466: __E___E_cli_command_137_138(281) => 'h' 'e' 'l' 'p'.
	p(NT(281), (T(62)+T(11)+T(10)+T(63)));
//G467: help_sym(280)        => __E___E_cli_command_137_138(281).
	p(NT(280), (NT(281)));
//G468: __E___E_cli_command_137_139(282) => __(35) help_arg(283).
	p(NT(282), (NT(35)+NT(283)));
//G469: __E___E_cli_command_137_139(282) => null.
	p(NT(282), (nul));
//G470: __E_cli_command_137(279) => help_sym(280) __E___E_cli_command_137_139(282).
	p(NT(279), (NT(280)+NT(282)));
//G471: help_cmd(278)        => __E_cli_command_137(279).
	p(NT(278), (NT(279)));
//G472: cli_command(265)     => help_cmd(278).
	p(NT(265), (NT(278)));
//G473: file_sym(286)        => 'f' 'i' 'l' 'e'.
	p(NT(286), (T(9)+T(4)+T(10)+T(11)));
//G474: __E_cli_command_140(285) => file_sym(286) __(35) q_string(287).
	p(NT(285), (NT(286)+NT(35)+NT(287)));
//G475: file_cmd(284)        => __E_cli_command_140(285).
	p(NT(284), (NT(285)));
//G476: cli_command(265)     => file_cmd(284).
	p(NT(265), (NT(284)));
//G477: valid_sym(290)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(290), (T(44)+T(16)+T(10)+T(4)+T(52)));
//G478: __E_cli_command_141(289) => valid_sym(290) __(35) spec_arg(291).
	p(NT(289), (NT(290)+NT(35)+NT(291)));
//G479: valid_cmd(288)       => __E_cli_command_141(289).
	p(NT(288), (NT(289)));
//G480: cli_command(265)     => valid_cmd(288).
	p(NT(265), (NT(288)));
//G481: sat_sym(294)         => 's' 'a' 't'.
	p(NT(294), (T(15)+T(16)+T(8)));
//G482: __E_cli_command_142(293) => sat_sym(294) __(35) spec_arg(291).
	p(NT(293), (NT(294)+NT(35)+NT(291)));
//G483: sat_cmd(292)         => __E_cli_command_142(293).
	p(NT(292), (NT(293)));
//G484: cli_command(265)     => sat_cmd(292).
	p(NT(265), (NT(292)));
//G485: unsat_sym(297)       => 'u' 'n' 's' 'a' 't'.
	p(NT(297), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G486: __E_cli_command_143(296) => unsat_sym(297) __(35) spec_arg(291).
	p(NT(296), (NT(297)+NT(35)+NT(291)));
//G487: unsat_cmd(295)       => __E_cli_command_143(296).
	p(NT(295), (NT(296)));
//G488: cli_command(265)     => unsat_cmd(295).
	p(NT(265), (NT(295)));
//G489: solve_sym(300)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(300), (T(15)+T(6)+T(10)+T(44)+T(11)));
//G490: __E___E_cli_command_144_145(301) => solve_options(302).
	p(NT(301), (NT(302)));
//G491: __E___E_cli_command_144_145(301) => null.
	p(NT(301), (nul));
//G492: __E_cli_command_144(299) => solve_sym(300) __E___E_cli_command_144_145(301) __(35) wff_cmd_arg(303).
	p(NT(299), (NT(300)+NT(301)+NT(35)+NT(303)));
//G493: solve_cmd(298)       => __E_cli_command_144(299).
	p(NT(298), (NT(299)));
//G494: cli_command(265)     => solve_cmd(298).
	p(NT(265), (NT(298)));
//G495: lgrs_sym(306)        => 'l' 'g' 'r' 's'.
	p(NT(306), (T(10)+T(64)+T(19)+T(15)));
//G496: __E_cli_command_146(305) => lgrs_sym(306) __(35) wff_cmd_arg(303).
	p(NT(305), (NT(306)+NT(35)+NT(303)));
//G497: lgrs_cmd(304)        => __E_cli_command_146(305).
	p(NT(304), (NT(305)));
//G498: cli_command(265)     => lgrs_cmd(304).
	p(NT(265), (NT(304)));
//G499: __E___E_cli_command_147_148(310) => 'r'.
	p(NT(310), (T(19)));
//G500: __E___E_cli_command_147_148(310) => 'r' 'u' 'n'.
	p(NT(310), (T(19)+T(7)+T(5)));
//G501: run_sym(309)         => __E___E_cli_command_147_148(310).
	p(NT(309), (NT(310)));
//G502: __E_cli_command_147(308) => run_sym(309) __(35) spec_arg(291).
	p(NT(308), (NT(309)+NT(35)+NT(291)));
//G503: run_cmd(307)         => __E_cli_command_147(308).
	p(NT(307), (NT(308)));
//G504: cli_command(265)     => run_cmd(307).
	p(NT(265), (NT(307)));
//G505: ltl_sym(313)         => 'l' 't' 'l'.
	p(NT(313), (T(10)+T(8)+T(10)));
//G506: __E_cli_command_149(312) => ltl_sym(313) __(35) spec_arg(291).
	p(NT(312), (NT(313)+NT(35)+NT(291)));
//G507: ltl_cmd(311)         => __E_cli_command_149(312).
	p(NT(311), (NT(312)));
//G508: cli_command(265)     => ltl_cmd(311).
	p(NT(265), (NT(311)));
//G509: func_sym_cmd(316)    => 'f' 'u' 'n' 'c'.
	p(NT(316), (T(9)+T(7)+T(5)+T(14)));
//G510: __E_cli_command_150(315) => func_sym_cmd(316) __(35) func_decl(206).
	p(NT(315), (NT(316)+NT(35)+NT(206)));
//G511: func_cmd(314)        => __E_cli_command_150(315).
	p(NT(314), (NT(315)));
//G512: cli_command(265)     => func_cmd(314).
	p(NT(265), (NT(314)));
//G513: __E___E_cli_command_151_152(320) => 'n'.
	p(NT(320), (T(5)));
//G514: __E___E_cli_command_151_152(320) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(320), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(65)+T(11)));
//G515: normalize_sym(319)   => __E___E_cli_command_151_152(320).
	p(NT(319), (NT(320)));
//G516: __E_cli_command_151(318) => normalize_sym(319) __(35) spec_arg(291).
	p(NT(318), (NT(319)+NT(35)+NT(291)));
//G517: normalize_cmd(317)   => __E_cli_command_151(318).
	p(NT(317), (NT(318)));
//G518: cli_command(265)     => normalize_cmd(317).
	p(NT(265), (NT(317)));
//G519: __E___E_cli_command_153_154(324) => 's'.
	p(NT(324), (T(15)));
//G520: __E___E_cli_command_153_154(324) => 's' 'u' 'b' 's' 't'.
	p(NT(324), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G521: __E___E_cli_command_153_154(324) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(324), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G522: subst_sym(323)       => __E___E_cli_command_153_154(324).
	p(NT(323), (NT(324)));
//G523: __E_cli_command_153(322) => subst_sym(323) __(35) nf_cmd_arg(325) _(13) '[' _(13) nf_cmd_arg(325) _(13) '/' _(13) nf_cmd_arg(325) _(13) ']'.
	p(NT(322), (NT(323)+NT(35)+NT(325)+NT(13)+T(26)+NT(13)+NT(325)+NT(13)+T(48)+NT(13)+NT(325)+NT(13)+T(27)));
//G524: subst_cmd(321)       => __E_cli_command_153(322).
	p(NT(321), (NT(322)));
//G525: cli_command(265)     => subst_cmd(321).
	p(NT(265), (NT(321)));
//G526: __E___E_cli_command_155_156(329) => 'i'.
	p(NT(329), (T(4)));
//G527: __E___E_cli_command_155_156(329) => 'i' 'n' 's' 't'.
	p(NT(329), (T(4)+T(5)+T(15)+T(8)));
//G528: __E___E_cli_command_155_156(329) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(329), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G529: inst_sym(328)        => __E___E_cli_command_155_156(329).
	p(NT(328), (NT(329)));
//G530: __E_cli_command_155(327) => inst_sym(328) __(35) inst_args(330).
	p(NT(327), (NT(328)+NT(35)+NT(330)));
//G531: inst_cmd(326)        => __E_cli_command_155(327).
	p(NT(326), (NT(327)));
//G532: cli_command(265)     => inst_cmd(326).
	p(NT(265), (NT(326)));
//G533: dnf_sym(333)         => 'd' 'n' 'f'.
	p(NT(333), (T(52)+T(5)+T(9)));
//G534: __E_cli_command_157(332) => dnf_sym(333) __(35) nf_cmd_arg(325).
	p(NT(332), (NT(333)+NT(35)+NT(325)));
//G535: dnf_cmd(331)         => __E_cli_command_157(332).
	p(NT(331), (NT(332)));
//G536: cli_command(265)     => dnf_cmd(331).
	p(NT(265), (NT(331)));
//G537: cnf_sym(336)         => 'c' 'n' 'f'.
	p(NT(336), (T(14)+T(5)+T(9)));
//G538: __E_cli_command_158(335) => cnf_sym(336) __(35) nf_cmd_arg(325).
	p(NT(335), (NT(336)+NT(35)+NT(325)));
//G539: cnf_cmd(334)         => __E_cli_command_158(335).
	p(NT(334), (NT(335)));
//G540: cli_command(265)     => cnf_cmd(334).
	p(NT(265), (NT(334)));
//G541: nnf_sym(339)         => 'n' 'n' 'f'.
	p(NT(339), (T(5)+T(5)+T(9)));
//G542: __E_cli_command_159(338) => nnf_sym(339) __(35) nf_cmd_arg(325).
	p(NT(338), (NT(339)+NT(35)+NT(325)));
//G543: nnf_cmd(337)         => __E_cli_command_159(338).
	p(NT(337), (NT(338)));
//G544: cli_command(265)     => nnf_cmd(337).
	p(NT(265), (NT(337)));
//G545: mnf_sym(342)         => 'm' 'n' 'f'.
	p(NT(342), (T(21)+T(5)+T(9)));
//G546: __E_cli_command_160(341) => mnf_sym(342) __(35) nf_cmd_arg(325).
	p(NT(341), (NT(342)+NT(35)+NT(325)));
//G547: mnf_cmd(340)         => __E_cli_command_160(341).
	p(NT(340), (NT(341)));
//G548: cli_command(265)     => mnf_cmd(340).
	p(NT(265), (NT(340)));
//G549: onf_sym(345)         => 'o' 'n' 'f'.
	p(NT(345), (T(6)+T(5)+T(9)));
//G550: __E_cli_command_161(344) => onf_sym(345) __(35) variable(130) __(35) onf_cmd_arg(346).
	p(NT(344), (NT(345)+NT(35)+NT(130)+NT(35)+NT(346)));
//G551: onf_cmd(343)         => __E_cli_command_161(344).
	p(NT(343), (NT(344)));
//G552: cli_command(265)     => onf_cmd(343).
	p(NT(265), (NT(343)));
//G553: qelim_sym(349)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(349), (T(61)+T(11)+T(10)+T(4)+T(21)));
//G554: __E_cli_command_162(348) => qelim_sym(349) __(35) wff_cmd_arg(303).
	p(NT(348), (NT(349)+NT(35)+NT(303)));
//G555: qelim_cmd(347)       => __E_cli_command_162(348).
	p(NT(347), (NT(348)));
//G556: cli_command(265)     => qelim_cmd(347).
	p(NT(265), (NT(347)));
//G557: whatis_sym(352)      => 'w' 'h' 'a' 't' 'i' 's'.
	p(NT(352), (T(24)+T(62)+T(16)+T(8)+T(4)+T(15)));
//G558: __E_cli_command_163(351) => whatis_sym(352) __(35) spec_arg(291).
	p(NT(351), (NT(352)+NT(35)+NT(291)));
//G559: whatis_cmd(350)      => __E_cli_command_163(351).
	p(NT(350), (NT(351)));
//G560: cli_command(265)     => whatis_cmd(350).
	p(NT(265), (NT(350)));
//G561: reset_sym(355)       => 'r' 'e' 's' 'e' 't'.
	p(NT(355), (T(19)+T(11)+T(15)+T(11)+T(8)));
//G562: __E_cli_command_164(354) => reset_sym(355).
	p(NT(354), (NT(355)));
//G563: reset_cmd(353)       => __E_cli_command_164(354).
	p(NT(353), (NT(354)));
//G564: cli_command(265)     => reset_cmd(353).
	p(NT(265), (NT(353)));
//G565: fragment_sym(358)    => 'f' 'r' 'a' 'g' 'm' 'e' 'n' 't'.
	p(NT(358), (T(9)+T(19)+T(16)+T(64)+T(21)+T(11)+T(5)+T(8)));
//G566: __E_cli_command_165(357) => fragment_sym(358) __(35) fragment_name(359).
	p(NT(357), (NT(358)+NT(35)+NT(359)));
//G567: fragment_cmd(356)    => __E_cli_command_165(357).
	p(NT(356), (NT(357)));
//G568: cli_command(265)     => fragment_cmd(356).
	p(NT(265), (NT(356)));
//G569: get_sym(362)         => 'g' 'e' 't'.
	p(NT(362), (T(64)+T(11)+T(8)));
//G570: __E___E_cli_command_166_167(363) => __(35) option_name(364).
	p(NT(363), (NT(35)+NT(364)));
//G571: __E___E_cli_command_166_167(363) => null.
	p(NT(363), (nul));
//G572: __E_cli_command_166(361) => get_sym(362) __E___E_cli_command_166_167(363).
	p(NT(361), (NT(362)+NT(363)));
//G573: get_cmd(360)         => __E_cli_command_166(361).
	p(NT(360), (NT(361)));
//G574: cli_command(265)     => get_cmd(360).
	p(NT(265), (NT(360)));
//G575: set_sym(367)         => 's' 'e' 't'.
	p(NT(367), (T(15)+T(11)+T(8)));
//G576: __E___E_cli_command_168_169(368) => __(35).
	p(NT(368), (NT(35)));
//G577: __E___E_cli_command_168_169(368) => _(13) '=' _(13).
	p(NT(368), (NT(13)+T(3)+NT(13)));
//G578: __E_cli_command_168(366) => set_sym(367) __(35) option_name(364) __E___E_cli_command_168_169(368) option_value(369).
	p(NT(366), (NT(367)+NT(35)+NT(364)+NT(368)+NT(369)));
//G579: set_cmd(365)         => __E_cli_command_168(366).
	p(NT(365), (NT(366)));
//G580: cli_command(265)     => set_cmd(365).
	p(NT(265), (NT(365)));
//G581: enable_sym(372)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(372), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G582: __E_cli_command_170(371) => enable_sym(372) __(35) option_name(364).
	p(NT(371), (NT(372)+NT(35)+NT(364)));
//G583: enable_cmd(370)      => __E_cli_command_170(371).
	p(NT(370), (NT(371)));
//G584: cli_command(265)     => enable_cmd(370).
	p(NT(265), (NT(370)));
//G585: disable_sym(375)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(375), (T(52)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G586: __E_cli_command_171(374) => disable_sym(375) __(35) option_name(364).
	p(NT(374), (NT(375)+NT(35)+NT(364)));
//G587: disable_cmd(373)     => __E_cli_command_171(374).
	p(NT(373), (NT(374)));
//G588: cli_command(265)     => disable_cmd(373).
	p(NT(265), (NT(373)));
//G589: toggle_sym(378)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(378), (T(8)+T(6)+T(64)+T(64)+T(10)+T(11)));
//G590: __E_cli_command_172(377) => toggle_sym(378) __(35) option_name(364).
	p(NT(377), (NT(378)+NT(35)+NT(364)));
//G591: toggle_cmd(376)      => __E_cli_command_172(377).
	p(NT(376), (NT(377)));
//G592: cli_command(265)     => toggle_cmd(376).
	p(NT(265), (NT(376)));
//G593: __E___E_cli_command_173_174(382) => 'd' 'e' 'f' 's'.
	p(NT(382), (T(52)+T(11)+T(9)+T(15)));
//G594: __E___E_cli_command_173_174(382) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(382), (T(52)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G595: def_sym(381)         => __E___E_cli_command_173_174(382).
	p(NT(381), (NT(382)));
//G596: __E_cli_command_173(380) => def_sym(381).
	p(NT(380), (NT(381)));
//G597: def_list_cmd(379)    => __E_cli_command_173(380).
	p(NT(379), (NT(380)));
//G598: cli_command(265)     => def_list_cmd(379).
	p(NT(265), (NT(379)));
//G599: __E_cli_command_175(384) => def_sym(381) __(35) num(138).
	p(NT(384), (NT(381)+NT(35)+NT(138)));
//G600: def_print_cmd(383)   => __E_cli_command_175(384).
	p(NT(383), (NT(384)));
//G601: cli_command(265)     => def_print_cmd(383).
	p(NT(265), (NT(383)));
//G602: def_rr_cmd(385)      => rec_relation(19).
	p(NT(385), (NT(19)));
//G603: cli_command(265)     => def_rr_cmd(385).
	p(NT(265), (NT(385)));
//G604: def_input_cmd(386)   => input_def(20).
	p(NT(386), (NT(20)));
//G605: cli_command(265)     => def_input_cmd(386).
	p(NT(265), (NT(386)));
//G606: def_output_cmd(387)  => output_def(21).
	p(NT(387), (NT(21)));
//G607: cli_command(265)     => def_output_cmd(387).
	p(NT(265), (NT(387)));
//G608: __E___E_cli_command_176_177(391) => 'h' 'i' 's' 't'.
	p(NT(391), (T(62)+T(4)+T(15)+T(8)));
//G609: __E___E_cli_command_176_177(391) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(391), (T(62)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G610: history_sym(390)     => __E___E_cli_command_176_177(391).
	p(NT(390), (NT(391)));
//G611: __E_cli_command_176(389) => history_sym(390).
	p(NT(389), (NT(390)));
//G612: history_list_cmd(388) => __E_cli_command_176(389).
	p(NT(388), (NT(389)));
//G613: cli_command(265)     => history_list_cmd(388).
	p(NT(265), (NT(388)));
//G614: __E_cli_command_178(393) => history_sym(390) __(35) history(394).
	p(NT(393), (NT(390)+NT(35)+NT(394)));
//G615: history_print_cmd(392) => __E_cli_command_178(393).
	p(NT(392), (NT(393)));
//G616: cli_command(265)     => history_print_cmd(392).
	p(NT(265), (NT(392)));
//G617: __E_cli_command_179(396) => ref(28).
	p(NT(396), (NT(28)));
//G618: __E_cli_command_179(396) => wff(15).
	p(NT(396), (NT(15)));
//G619: __E_cli_command_179(396) => bf(31).
	p(NT(396), (NT(31)));
//G620: history_store_cmd(395) => __E_cli_command_179(396).
	p(NT(395), (NT(396)));
//G621: cli_command(265)     => history_store_cmd(395).
	p(NT(265), (NT(395)));
//G622: __E_solve_options_180(397) => __(35) solve_option(398).
	p(NT(397), (NT(35)+NT(398)));
//G623: __E_solve_options_181(399) => null.
	p(NT(399), (nul));
//G624: __E_solve_options_181(399) => __E_solve_options_180(397) __E_solve_options_181(399).
	p(NT(399), (NT(397)+NT(399)));
//G625: solve_options(302)   => __E_solve_options_181(399).
	p(NT(302), (NT(399)));
//G626: __E_solve_option_182(400) => solver_mode(401).
	p(NT(400), (NT(401)));
//G627: __E_solve_option_182(400) => type(203).
	p(NT(400), (NT(203)));
//G628: solve_option(398)    => '-' '-' __E_solve_option_182(400).
	p(NT(398), (T(32)+T(32)+NT(400)));
//G629: __E_solver_mode_183(403) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(403), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G630: __E_solver_mode_183(403) => 'm' 'i' 'n'.
	p(NT(403), (T(21)+T(4)+T(5)));
//G631: solver_mode_minimum(402) => __E_solver_mode_183(403).
	p(NT(402), (NT(403)));
//G632: solver_mode(401)     => solver_mode_minimum(402).
	p(NT(401), (NT(402)));
//G633: __E_solver_mode_184(405) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(405), (T(21)+T(16)+T(34)+T(4)+T(21)+T(7)+T(21)));
//G634: __E_solver_mode_184(405) => 'm' 'a' 'x'.
	p(NT(405), (T(21)+T(16)+T(34)));
//G635: solver_mode_maximum(404) => __E_solver_mode_184(405).
	p(NT(404), (NT(405)));
//G636: solver_mode(401)     => solver_mode_maximum(404).
	p(NT(401), (NT(404)));
//G637: q_file_name(38)      => '"' file_name(406) '"'.
	p(NT(38), (T(66)+NT(406)+T(66)));
//G638: __E_file_name_185(407) => printable(8).
	p(NT(407), (NT(8)));
//G639: __E_file_name_185(407) => printable(8) __E_file_name_185(407).
	p(NT(407), (NT(8)+NT(407)));
//G640: file_name(406)       => __E_file_name_185(407).
	p(NT(406), (NT(407)));
//G641: __E_option_name_186(408) => alnum(6).
	p(NT(408), (NT(6)));
//G642: __E_option_name_186(408) => alnum(6) __E_option_name_186(408).
	p(NT(408), (NT(6)+NT(408)));
//G643: option_name(364)     => __E_option_name_186(408).
	p(NT(364), (NT(408)));
//G644: __E_option_value_187(409) => alnum(6).
	p(NT(409), (NT(6)));
//G645: __E_option_value_187(409) => alnum(6) __E_option_value_187(409).
	p(NT(409), (NT(6)+NT(409)));
//G646: option_value(369)    => __E_option_value_187(409).
	p(NT(369), (NT(409)));
//G647: fragment_ltl(411)    => 'l' 't' 'l'.
	p(NT(411), (T(10)+T(8)+T(10)));
//G648: __E_fragment_name_188(410) => fragment_ltl(411).
	p(NT(410), (NT(411)));
//G649: fragment_ctl_star(412) => 'c' 't' 'l' '_' 's' 't' 'a' 'r'.
	p(NT(412), (T(14)+T(8)+T(10)+T(55)+T(15)+T(8)+T(16)+T(19)));
//G650: __E_fragment_name_188(410) => fragment_ctl_star(412).
	p(NT(410), (NT(412)));
//G651: fragment_ctl_star(412) => 'c' 't' 'l' '*'.
	p(NT(412), (T(14)+T(8)+T(10)+T(47)));
//G652: __E_fragment_name_188(410) => fragment_ctl_star(412).
	p(NT(410), (NT(412)));
//G653: fragment_name(359)   => __E_fragment_name_188(410).
	p(NT(359), (NT(410)));
//G654: bf_cmd_arg(413)      => history(394).
	p(NT(413), (NT(394)));
//G655: bf_cmd_arg(413)      => bf(31).
	p(NT(413), (NT(31)));
//G656: wff_cmd_arg(303)     => history(394).
	p(NT(303), (NT(394)));
//G657: wff_cmd_arg(303)     => wff(15).
	p(NT(303), (NT(15)));
//G658: nf_cmd_arg(325)      => history(394).
	p(NT(325), (NT(394)));
//G659: nf_cmd_arg(325)      => ref(28).
	p(NT(325), (NT(28)));
//G660: nf_cmd_arg(325)      => wff(15).
	p(NT(325), (NT(15)));
//G661: nf_cmd_arg(325)      => bf(31).
	p(NT(325), (NT(31)));
//G662: onf_cmd_arg(346)     => history(394).
	p(NT(346), (NT(394)));
//G663: onf_cmd_arg(346)     => wff(15).
	p(NT(346), (NT(15)));
//G664: spec_arg(291)        => history(394).
	p(NT(291), (NT(394)));
//G665: spec_arg(291)        => spec(10).
	p(NT(291), (NT(10)));
//G666: spec_arg(291)        => ref(28).
	p(NT(291), (NT(28)));
//G667: spec_arg(291)        => wff(15).
	p(NT(291), (NT(15)));
//G668: spec_arg(291)        => bf(31).
	p(NT(291), (NT(31)));
//G669: inst_args(330)       => wff_cmd_arg(303) _(13) '[' _(13) bf(31) _(13) '/' _(13) bf_cmd_arg(413) _(13) ']'.
	p(NT(330), (NT(303)+NT(13)+T(26)+NT(13)+NT(31)+NT(13)+T(48)+NT(13)+NT(413)+NT(13)+T(27)));
//G670: inst_args(330)       => bf_cmd_arg(413) _(13) '[' _(13) bf(31) _(13) '/' _(13) bf_cmd_arg(413) _(13) ']'.
	p(NT(330), (NT(413)+NT(13)+T(26)+NT(13)+NT(31)+NT(13)+T(48)+NT(13)+NT(413)+NT(13)+T(27)));
//G671: help_arg(283)        => help_sym(280).
	p(NT(283), (NT(280)));
//G672: help_arg(283)        => version_sym(273).
	p(NT(283), (NT(273)));
//G673: help_arg(283)        => quit_sym(270).
	p(NT(283), (NT(270)));
//G674: help_arg(283)        => clear_sym(276).
	p(NT(283), (NT(276)));
//G675: help_arg(283)        => get_sym(362).
	p(NT(283), (NT(362)));
//G676: help_arg(283)        => set_sym(367).
	p(NT(283), (NT(367)));
//G677: help_arg(283)        => enable_sym(372).
	p(NT(283), (NT(372)));
//G678: help_arg(283)        => disable_sym(375).
	p(NT(283), (NT(375)));
//G679: help_arg(283)        => toggle_sym(378).
	p(NT(283), (NT(378)));
//G680: help_arg(283)        => file_sym(286).
	p(NT(283), (NT(286)));
//G681: help_arg(283)        => history_sym(390).
	p(NT(283), (NT(390)));
//G682: help_arg(283)        => abs_history_sym(414).
	p(NT(283), (NT(414)));
//G683: help_arg(283)        => rel_history_sym(415).
	p(NT(283), (NT(415)));
//G684: help_arg(283)        => selection_sym(416).
	p(NT(283), (NT(416)));
//G685: help_arg(283)        => def_sym(381).
	p(NT(283), (NT(381)));
//G686: help_arg(283)        => inst_sym(328).
	p(NT(283), (NT(328)));
//G687: help_arg(283)        => subst_sym(323).
	p(NT(283), (NT(323)));
//G688: help_arg(283)        => normalize_sym(319).
	p(NT(283), (NT(319)));
//G689: help_arg(283)        => execute_sym(417).
	p(NT(283), (NT(417)));
//G690: help_arg(283)        => solve_sym(300).
	p(NT(283), (NT(300)));
//G691: help_arg(283)        => lgrs_sym(306).
	p(NT(283), (NT(306)));
//G692: help_arg(283)        => valid_sym(290).
	p(NT(283), (NT(290)));
//G693: help_arg(283)        => sat_sym(294).
	p(NT(283), (NT(294)));
//G694: help_arg(283)        => unsat_sym(297).
	p(NT(283), (NT(297)));
//G695: help_arg(283)        => run_sym(309).
	p(NT(283), (NT(309)));
//G696: help_arg(283)        => ltl_sym(313).
	p(NT(283), (NT(313)));
//G697: help_arg(283)        => dnf_sym(333).
	p(NT(283), (NT(333)));
//G698: help_arg(283)        => cnf_sym(336).
	p(NT(283), (NT(336)));
//G699: help_arg(283)        => snf_sym(418).
	p(NT(283), (NT(418)));
//G700: help_arg(283)        => nnf_sym(339).
	p(NT(283), (NT(339)));
//G701: help_arg(283)        => mnf_sym(342).
	p(NT(283), (NT(342)));
//G702: help_arg(283)        => onf_sym(345).
	p(NT(283), (NT(345)));
//G703: help_arg(283)        => qelim_sym(349).
	p(NT(283), (NT(349)));
//G704: help_arg(283)        => whatis_sym(352).
	p(NT(283), (NT(352)));
//G705: help_arg(283)        => reset_sym(355).
	p(NT(283), (NT(355)));
//G706: help_arg(283)        => fragment_sym(358).
	p(NT(283), (NT(358)));
//G707: __E___E_help_arg_189_190(421) => 's'.
	p(NT(421), (T(15)));
//G708: __E___E_help_arg_189_190(421) => null.
	p(NT(421), (nul));
//G709: __E_help_arg_189(420) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_189_190(421).
	p(NT(420), (T(11)+T(34)+T(16)+T(21)+T(63)+T(10)+T(11)+NT(421)));
//G710: examples_sym(419)    => __E_help_arg_189(420).
	p(NT(419), (NT(420)));
//G711: help_arg(283)        => examples_sym(419).
	p(NT(283), (NT(419)));
//G712: __E_history_191(423) => '%'.
	p(NT(423), (T(46)));
//G713: rel_history_sym(415) => '%' '-'.
	p(NT(415), (T(46)+T(32)));
//G714: history_id(425)      => digits(255).
	p(NT(425), (NT(255)));
//G715: __E___E_history_191_192(424) => history_id(425).
	p(NT(424), (NT(425)));
//G716: __E___E_history_191_192(424) => null.
	p(NT(424), (nul));
//G717: __E_history_191(423) => rel_history_sym(415) __E___E_history_191_192(424).
	p(NT(423), (NT(415)+NT(424)));
//G718: rel_history(422)     => __E_history_191(423).
	p(NT(422), (NT(423)));
//G719: history(394)         => rel_history(422).
	p(NT(394), (NT(422)));
//G720: abs_history_sym(414) => '%'.
	p(NT(414), (T(46)));
//G721: __E_history_193(427) => abs_history_sym(414) history_id(425).
	p(NT(427), (NT(414)+NT(425)));
//G722: abs_history(426)     => __E_history_193(427).
	p(NT(426), (NT(427)));
//G723: history(394)         => abs_history(426).
	p(NT(394), (NT(426)));
//G724: unreachable(428)     => BDD_ID(429).
	p(NT(428), (NT(429)));
//G725: stream_def(430)      => input_def(20).
	p(NT(430), (NT(20)));
//G726: stream_def(430)      => output_def(21).
	p(NT(430), (NT(21)));
//G727: fm_or_term(431)      => wff(15).
	p(NT(431), (NT(15)));
//G728: fm_or_term(431)      => bf(31).
	p(NT(431), (NT(31)));
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
		__E_wff_21, __E___E_wff_21_22, wff_F, __E_wff_23, wff_A, __E_wff_24, wff_E, __E_wff_25, wff_semantic_neg, __E_wff_26, 
		wff_conditional, __E_wff_27, wff_all, __E_wff_28, q_vars, wff_ex, __E_wff_29, wff_ref, constraint, wff_imply, 
		__E_wff_30, wff_rimply, __E_wff_31, wff_equiv, __E_wff_32, wff_or, __E_wff_33, wff_xor, __E_wff_34, wff_and, 
		__E_wff_35, wff_neg, __E_wff_36, wff_t, wff_f, bf_interval, __E_wff_37, bf_eq, __E_wff_38, bf_neq, 
		__E_wff_39, bf_lt, __E_wff_40, bf_nlt, __E_wff_41, bf_lteq, __E_wff_42, bf_nlteq, __E_wff_43, bf_gt, 
		__E_wff_44, bf_ngt, __E_wff_45, bf_gteq, __E_wff_46, bf_ngteq, __E_wff_47, wff_U, __E_wff_48, wff_R, 
		__E_wff_49, wff_W, __E_wff_50, wff_S, __E_wff_51, wff_T, __E_wff_52, bf_parenthesis, __E_bf_53, ba_constant, 
		variable, bf_fall, __E_bf_54, bf_fex, __E_bf_55, bf_ref, bf_cast, __E_bf_56, num, bf_cast_oprnd, 
		__E___E_bf_56_57, bf_t, bf_f, bf_neg, bf_func_app, __E_bf_58, func_sym, func_args, bf_shr, __E_bf_59, 
		__E___E_bf_59_60, bf_shl, __E_bf_61, __E___E_bf_61_62, bf_add, __E_bf_63, bf_sub, __E_bf_64, __E___E_bf_64_65, bf_mod, 
		__E_bf_66, __E___E_bf_66_67, bf_mul, __E_bf_68, bf_div, __E_bf_69, __E___E_bf_69_70, bf_nor, __E_bf_71, __E___E_bf_71_72, 
		bf_xnor, __E_bf_73, __E___E_bf_73_74, bf_nand, __E_bf_75, __E___E_bf_75_76, bf_or, __E_bf_77, bf_xor, __E_bf_78, 
		__E_bf_79, __E___E_bf_79_80, __E_bf_81, __E___E_bf_81_82, bf_and, __E_bf_83, __E___E_bf_83_84, __E_bf_85, bf_neg_oprnd, __E___E_bf_85_86, 
		bf_and_nosep, __E_bf_87, bf_and_nosep_1st_oprnd, __E___E_bf_87_88, bf_and_nosep_2nd_oprnd, __E___E_bf_87_89, chars, __E_func_args_90, __E_func_args_91, func_mode, 
		func_dynamic, func_static, func_sort_list, type, __E_func_sort_list_92, __E_func_sort_list_93, func_decl, ctn_neq, __E_constraint_94, ctnvar, 
		ctn_eq, __E_constraint_95, ctn_gteq, __E_constraint_96, ctn_gt, __E_constraint_97, ctn_lteq, __E_constraint_98, ctn_lt, __E_constraint_99, 
		__E_ba_constant_100, source, __E_ba_constant_101, src_c, __E_source_102, __E___E_source_102_103, __E___E_source_102_104, __E_src_c_105, __E_src_c_106, __E_variable_107, 
		uconst, io_var, var_name, __E_variable_108, __E_var_name_109, __E_var_name_110, __E_var_name_111, __E_var_name_112, __E_var_name_113, offset, 
		uconst_name, __E_uconst_114, __E___E_uconst_114_115, q_var, __E_q_vars_116, __E_q_vars_117, __E_offsets_118, __E_offsets_119, integer, shift, 
		__E_offset_120, __E_shift_121, __E___E_shift_121_122, __E_chars_123, __E_chars_124, digits, __E_digits_125, __E_integer_126, subtype, __E_typed_127, 
		comment, __E_comment_128, __E_comment_129, __E_comment_130, cli, cli_command, __E_cli_131, __E_cli_132, __E_cli_133, quit_cmd, 
		quit_sym, __E_cli_command_134, version_cmd, version_sym, __E_cli_command_135, clear_cmd, clear_sym, __E_cli_command_136, help_cmd, __E_cli_command_137, 
		help_sym, __E___E_cli_command_137_138, __E___E_cli_command_137_139, help_arg, file_cmd, __E_cli_command_140, file_sym, q_string, valid_cmd, __E_cli_command_141, 
		valid_sym, spec_arg, sat_cmd, __E_cli_command_142, sat_sym, unsat_cmd, __E_cli_command_143, unsat_sym, solve_cmd, __E_cli_command_144, 
		solve_sym, __E___E_cli_command_144_145, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_146, lgrs_sym, run_cmd, __E_cli_command_147, run_sym, 
		__E___E_cli_command_147_148, ltl_cmd, __E_cli_command_149, ltl_sym, func_cmd, __E_cli_command_150, func_sym_cmd, normalize_cmd, __E_cli_command_151, normalize_sym, 
		__E___E_cli_command_151_152, subst_cmd, __E_cli_command_153, subst_sym, __E___E_cli_command_153_154, nf_cmd_arg, inst_cmd, __E_cli_command_155, inst_sym, __E___E_cli_command_155_156, 
		inst_args, dnf_cmd, __E_cli_command_157, dnf_sym, cnf_cmd, __E_cli_command_158, cnf_sym, nnf_cmd, __E_cli_command_159, nnf_sym, 
		mnf_cmd, __E_cli_command_160, mnf_sym, onf_cmd, __E_cli_command_161, onf_sym, onf_cmd_arg, qelim_cmd, __E_cli_command_162, qelim_sym, 
		whatis_cmd, __E_cli_command_163, whatis_sym, reset_cmd, __E_cli_command_164, reset_sym, fragment_cmd, __E_cli_command_165, fragment_sym, fragment_name, 
		get_cmd, __E_cli_command_166, get_sym, __E___E_cli_command_166_167, option_name, set_cmd, __E_cli_command_168, set_sym, __E___E_cli_command_168_169, option_value, 
		enable_cmd, __E_cli_command_170, enable_sym, disable_cmd, __E_cli_command_171, disable_sym, toggle_cmd, __E_cli_command_172, toggle_sym, def_list_cmd, 
		__E_cli_command_173, def_sym, __E___E_cli_command_173_174, def_print_cmd, __E_cli_command_175, def_rr_cmd, def_input_cmd, def_output_cmd, history_list_cmd, __E_cli_command_176, 
		history_sym, __E___E_cli_command_176_177, history_print_cmd, __E_cli_command_178, history, history_store_cmd, __E_cli_command_179, __E_solve_options_180, solve_option, __E_solve_options_181, 
		__E_solve_option_182, solver_mode, solver_mode_minimum, __E_solver_mode_183, solver_mode_maximum, __E_solver_mode_184, file_name, __E_file_name_185, __E_option_name_186, __E_option_value_187, 
		__E_fragment_name_188, fragment_ltl, fragment_ctl_star, bf_cmd_arg, abs_history_sym, rel_history_sym, selection_sym, execute_sym, snf_sym, examples_sym, 
		__E_help_arg_189, __E___E_help_arg_189_190, rel_history, __E_history_191, __E___E_history_191_192, history_id, abs_history, __E_history_193, unreachable, BDD_ID, 
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
