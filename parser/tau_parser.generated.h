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
	"__E_wff_45", "bf_gteq", "__E_wff_46", "bf_ngteq", "__E_wff_47", "bf_lt", "__E_wff_48", "bf_nlt", "__E_wff_49", "bf_parenthesis", 
	"__E_bf_50", "bf_constant", "variable", "bf_splitter", "__E_bf_51", "bf_ref", "bf_or", "__E_bf_52", "bf_xor", "__E_bf_53", 
	"bf_t", "__E_bf_54", "__E___E_bf_54_55", "bf_f", "__E_bf_56", "__E___E_bf_56_57", "bf_and", "__E_bf_58", "__E___E_bf_58_59", "bf_neg", 
	"__E_bf_60", "bf_neg_oprnd", "__E___E_bf_60_61", "bf_and_nosep", "__E_bf_62", "bf_and_nosep_1st_oprnd", "__E___E_bf_62_63", "bf_and_nosep_2nd_oprnd", "__E___E_bf_62_64", "ctn_neq", 
	"__E_constraint_65", "ctnvar", "num", "ctn_eq", "__E_constraint_66", "ctn_gteq", "__E_constraint_67", "ctn_gt", "__E_constraint_68", "ctn_lteq", 
	"__E_constraint_69", "ctn_lt", "__E_constraint_70", "__E_bf_constant_71", "source", "__E_bf_constant_72", "src_c", "__E_source_73", "__E___E_source_73_74", "__E___E_source_73_75", 
	"__E_src_c_76", "__E_src_c_77", "__E_variable_78", "uconst", "io_var", "var_name", "__E_variable_79", "__E_var_name_80", "__E_var_name_81", "__E_var_name_82", 
	"__E_var_name_83", "__E_var_name_84", "offset", "chars", "uconst_name", "__E_uconst_85", "__E___E_uconst_85_86", "q_var", "__E_q_vars_87", "__E_q_vars_88", 
	"__E_offsets_89", "__E_offsets_90", "integer", "shift", "__E_offset_91", "__E_shift_92", "__E___E_shift_92_93", "__E_chars_94", "__E_chars_95", "digits", 
	"__E_digits_96", "__E_integer_97", "type", "scope_id", "comment", "__E_comment_98", "__E_comment_99", "__E_comment_100", "__E____101", "__E___102", 
	"cli", "cli_command", "__E_cli_103", "__E_cli_104", "quit_cmd", "quit_sym", "__E_cli_command_105", "version_cmd", "version_sym", "__E_cli_command_106", 
	"clear_cmd", "clear_sym", "__E_cli_command_107", "help_cmd", "__E_cli_command_108", "help_sym", "__E___E_cli_command_108_109", "__E___E_cli_command_108_110", "help_arg", "file_cmd", 
	"__E_cli_command_111", "file_sym", "q_string", "valid_cmd", "__E_cli_command_112", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_113", "sat_sym", 
	"unsat_cmd", "__E_cli_command_114", "unsat_sym", "solve_cmd", "__E_cli_command_115", "solve_sym", "__E___E_cli_command_115_116", "solve_options", "wff_cmd_arg", "lgrs_cmd", 
	"__E_cli_command_117", "lgrs_sym", "run_cmd", "__E_cli_command_118", "run_sym", "__E___E_cli_command_118_119", "__E___E_cli_command_118_120", "history", "normalize_cmd", "__E_cli_command_121", 
	"normalize_sym", "__E___E_cli_command_121_122", "subst_cmd", "__E_cli_command_123", "subst_sym", "__E___E_cli_command_123_124", "nf_cmd_arg", "inst_cmd", "__E_cli_command_125", "inst_sym", 
	"__E___E_cli_command_125_126", "inst_args", "dnf_cmd", "__E_cli_command_127", "dnf_sym", "cnf_cmd", "__E_cli_command_128", "cnf_sym", "anf_cmd", "__E_cli_command_129", 
	"anf_sym", "nnf_cmd", "__E_cli_command_130", "nnf_sym", "pnf_cmd", "__E_cli_command_131", "pnf_sym", "mnf_cmd", "__E_cli_command_132", "mnf_sym", 
	"snf_cmd", "__E_cli_command_133", "snf_sym", "onf_cmd", "__E_cli_command_134", "onf_sym", "onf_cmd_arg", "qelim_cmd", "__E_cli_command_135", "qelim_sym", 
	"get_cmd", "__E_cli_command_136", "get_sym", "__E___E_cli_command_136_137", "option_name", "set_cmd", "__E_cli_command_138", "set_sym", "__E___E_cli_command_138_139", "option_value", 
	"enable_cmd", "__E_cli_command_140", "enable_sym", "disable_cmd", "__E_cli_command_141", "disable_sym", "toggle_cmd", "__E_cli_command_142", "toggle_sym", "def_list_cmd", 
	"__E_cli_command_143", "def_sym", "__E___E_cli_command_143_144", "def_print_cmd", "__E_cli_command_145", "def_rr_cmd", "def_input_cmd", "def_output_cmd", "history_list_cmd", "__E_cli_command_146", 
	"history_sym", "__E___E_cli_command_146_147", "history_print_cmd", "__E_cli_command_148", "history_store_cmd", "__E_cli_command_149", "__E_solve_options_150", "solve_option", "__E_solve_options_151", "__E_solve_option_152", 
	"solver_mode", "solver_mode_minimum", "__E_solver_mode_153", "solver_mode_maximum", "__E_solver_mode_154", "file_name", "__E_file_name_155", "__E_option_name_156", "__E_option_value_157", "bf_cmd_arg", 
	"abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", "examples_sym", "__E_help_arg_158", "__E___E_help_arg_158_159", "rel_history", "__E_history_160", "__E___E_history_160_161", 
	"history_id", "abs_history", "__E_history_162", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", 
	"__N_7", "__N_8", 
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
			11, 31
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			26, 28, 37, 161, 174, 185, 194, 202, 209, 212,
			314, 319, 355
		},
		.to_inline = {
			{ 14, 74, 14 },
			{ 27, 129, 27 },
			{ 30 },
			{ 151, 129, 27 },
			{ 153, 129, 146 },
			{ 155, 129, 27 },
			{ 157, 129, 27 },
			{ 176 },
			{ 183 },
			{ 183, 194 },
			{ 193 },
			{ 197 },
			{ 246 },
			{ 258 },
			{ 276 },
			{ 281 },
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
//G163: wff(14)              => capture(26).
	p(NT(14), (NT(26)));
//G164: __E_bf_50(130)       => '(' _(11) bf(27) _(11) ')'.
	p(NT(130), (T(12)+NT(11)+NT(27)+NT(11)+T(13)));
//G165: bf_parenthesis(129)  => __E_bf_50(130).
	p(NT(129), (NT(130)));
//G166: bf(27)               => bf_parenthesis(129).
	p(NT(27), (NT(129)));
//G167: bf(27)               => bf_constant(131).
	p(NT(27), (NT(131)));
//G168: bf(27)               => variable(132).
	p(NT(27), (NT(132)));
//G169: __E_bf_51(134)       => 'S' _(11) '(' _(11) bf(27) _(11) ')'.
	p(NT(134), (T(37)+NT(11)+T(12)+NT(11)+NT(27)+NT(11)+T(13)));
//G170: bf_splitter(133)     => __E_bf_51(134).
	p(NT(133), (NT(134)));
//G171: bf(27)               => bf_splitter(133).
	p(NT(27), (NT(133)));
//G172: bf_ref(135)          => ref(24).
	p(NT(135), (NT(24)));
//G173: bf(27)               => bf_ref(135).
	p(NT(27), (NT(135)));
//G174: __E_bf_52(137)       => bf(27) _(11) '|' _(11) bf(27).
	p(NT(137), (NT(27)+NT(11)+T(31)+NT(11)+NT(27)));
//G175: bf_or(136)           => __E_bf_52(137).
	p(NT(136), (NT(137)));
//G176: bf(27)               => bf_or(136).
	p(NT(27), (NT(136)));
//G177: __E_bf_53(139)       => bf(27) _(11) '+' _(11) bf(27).
	p(NT(139), (NT(27)+NT(11)+T(38)+NT(11)+NT(27)));
//G178: bf_xor(138)          => __E_bf_53(139).
	p(NT(138), (NT(139)));
//G179: bf(27)               => bf_xor(138).
	p(NT(27), (NT(138)));
//G180: __E___E_bf_54_55(142) => typed(30).
	p(NT(142), (NT(30)));
//G181: __E___E_bf_54_55(142) => null.
	p(NT(142), (nul));
//G182: __E_bf_54(141)       => '1' __E___E_bf_54_55(142).
	p(NT(141), (T(39)+NT(142)));
//G183: bf_t(140)            => __E_bf_54(141).
	p(NT(140), (NT(141)));
//G184: bf(27)               => bf_t(140).
	p(NT(27), (NT(140)));
//G185: __E___E_bf_56_57(145) => typed(30).
	p(NT(145), (NT(30)));
//G186: __E___E_bf_56_57(145) => null.
	p(NT(145), (nul));
//G187: __E_bf_56(144)       => '0' __E___E_bf_56_57(145).
	p(NT(144), (T(40)+NT(145)));
//G188: bf_f(143)            => __E_bf_56(144).
	p(NT(143), (NT(144)));
//G189: bf(27)               => bf_f(143).
	p(NT(27), (NT(143)));
//G190: __E___E_bf_58_59(148) => _(11) '&' _(11).
	p(NT(148), (NT(11)+T(33)+NT(11)));
//G191: __E___E_bf_58_59(148) => __(31).
	p(NT(148), (NT(31)));
//G192: __E_bf_58(147)       => bf(27) __E___E_bf_58_59(148) bf(27).
	p(NT(147), (NT(27)+NT(148)+NT(27)));
//G193: bf_and(146)          => __E_bf_58(147).
	p(NT(146), (NT(147)));
//G194: bf(27)               => bf_and(146).
	p(NT(27), (NT(146)));
//G195: __E___E_bf_60_61(152) => bf_parenthesis(129).
	p(NT(152), (NT(129)));
//G196: __E___E_bf_60_61(152) => bf_constant(131).
	p(NT(152), (NT(131)));
//G197: __E___E_bf_60_61(152) => variable(132).
	p(NT(152), (NT(132)));
//G198: __E___E_bf_60_61(152) => bf_splitter(133).
	p(NT(152), (NT(133)));
//G199: __E___E_bf_60_61(152) => bf_ref(135).
	p(NT(152), (NT(135)));
//G200: __E___E_bf_60_61(152) => bf_t(140).
	p(NT(152), (NT(140)));
//G201: __E___E_bf_60_61(152) => bf_f(143).
	p(NT(152), (NT(143)));
//G202: __E___E_bf_60_61(152) => bf_neg(149).
	p(NT(152), (NT(149)));
//G203: __E___E_bf_60_61(152) => capture(26).
	p(NT(152), (NT(26)));
//G204: bf_neg_oprnd(151)    => __E___E_bf_60_61(152).
	p(NT(151), (NT(152)));
//G205: __E_bf_60(150)       => bf_neg_oprnd(151) _(11) '\''.
	p(NT(150), (NT(151)+NT(11)+T(41)));
//G206: bf_neg(149)          => __E_bf_60(150).
	p(NT(149), (NT(150)));
//G207: bf(27)               => bf_neg(149).
	p(NT(27), (NT(149)));
//G208: __E___E_bf_62_63(156) => bf_parenthesis(129).
	p(NT(156), (NT(129)));
//G209: __E___E_bf_62_63(156) => variable(132).
	p(NT(156), (NT(132)));
//G210: __E___E_bf_62_63(156) => bf_splitter(133).
	p(NT(156), (NT(133)));
//G211: __E___E_bf_62_63(156) => bf_ref(135).
	p(NT(156), (NT(135)));
//G212: __E___E_bf_62_63(156) => bf_neg(149).
	p(NT(156), (NT(149)));
//G213: __E___E_bf_62_63(156) => bf_and_nosep(153).
	p(NT(156), (NT(153)));
//G214: __E___E_bf_62_63(156) => capture(26).
	p(NT(156), (NT(26)));
//G215: bf_and_nosep_1st_oprnd(155) => __E___E_bf_62_63(156).
	p(NT(155), (NT(156)));
//G216: __E___E_bf_62_64(158) => bf_parenthesis(129).
	p(NT(158), (NT(129)));
//G217: __E___E_bf_62_64(158) => bf_constant(131).
	p(NT(158), (NT(131)));
//G218: __E___E_bf_62_64(158) => variable(132).
	p(NT(158), (NT(132)));
//G219: __E___E_bf_62_64(158) => bf_splitter(133).
	p(NT(158), (NT(133)));
//G220: __E___E_bf_62_64(158) => bf_ref(135).
	p(NT(158), (NT(135)));
//G221: __E___E_bf_62_64(158) => bf_neg(149).
	p(NT(158), (NT(149)));
//G222: __E___E_bf_62_64(158) => capture(26).
	p(NT(158), (NT(26)));
//G223: bf_and_nosep_2nd_oprnd(157) => __E___E_bf_62_64(158).
	p(NT(157), (NT(158)));
//G224: __E_bf_62(154)       => bf_and_nosep_1st_oprnd(155) bf_and_nosep_2nd_oprnd(157).
	p(NT(154), (NT(155)+NT(157)));
//G225: bf_and_nosep(153)    => __E_bf_62(154).
	p(NT(153), (NT(154)));
//G226: bf(27)               => bf_and_nosep(153).
	p(NT(27), (NT(153)));
//G227: bf(27)               => capture(26).
	p(NT(27), (NT(26)));
//G228: __E_constraint_65(160) => '[' ctnvar(161) _(11) '!' '=' _(11) num(162) ']'.
	p(NT(160), (T(26)+NT(161)+NT(11)+T(34)+T(3)+NT(11)+NT(162)+T(27)));
//G229: __E_constraint_65(160) => '[' num(162) _(11) '!' '=' _(11) ctnvar(161) ']'.
	p(NT(160), (T(26)+NT(162)+NT(11)+T(34)+T(3)+NT(11)+NT(161)+T(27)));
//G230: ctn_neq(159)         => __E_constraint_65(160).
	p(NT(159), (NT(160)));
//G231: constraint(90)       => ctn_neq(159).
	p(NT(90), (NT(159)));
//G232: __E_constraint_66(164) => '[' ctnvar(161) _(11) '=' _(11) num(162) ']'.
	p(NT(164), (T(26)+NT(161)+NT(11)+T(3)+NT(11)+NT(162)+T(27)));
//G233: __E_constraint_66(164) => '[' num(162) _(11) '=' _(11) ctnvar(161) ']'.
	p(NT(164), (T(26)+NT(162)+NT(11)+T(3)+NT(11)+NT(161)+T(27)));
//G234: ctn_eq(163)          => __E_constraint_66(164).
	p(NT(163), (NT(164)));
//G235: constraint(90)       => ctn_eq(163).
	p(NT(90), (NT(163)));
//G236: __E_constraint_67(166) => '[' ctnvar(161) _(11) '>' '=' _(11) num(162) ']'.
	p(NT(166), (T(26)+NT(161)+NT(11)+T(23)+T(3)+NT(11)+NT(162)+T(27)));
//G237: __E_constraint_67(166) => '[' num(162) _(11) '>' '=' _(11) ctnvar(161) ']'.
	p(NT(166), (T(26)+NT(162)+NT(11)+T(23)+T(3)+NT(11)+NT(161)+T(27)));
//G238: ctn_gteq(165)        => __E_constraint_67(166).
	p(NT(165), (NT(166)));
//G239: constraint(90)       => ctn_gteq(165).
	p(NT(90), (NT(165)));
//G240: __E_constraint_68(168) => '[' ctnvar(161) _(11) '>' _(11) num(162) ']'.
	p(NT(168), (T(26)+NT(161)+NT(11)+T(23)+NT(11)+NT(162)+T(27)));
//G241: __E_constraint_68(168) => '[' num(162) _(11) '>' _(11) ctnvar(161) ']'.
	p(NT(168), (T(26)+NT(162)+NT(11)+T(23)+NT(11)+NT(161)+T(27)));
//G242: ctn_gt(167)          => __E_constraint_68(168).
	p(NT(167), (NT(168)));
//G243: constraint(90)       => ctn_gt(167).
	p(NT(90), (NT(167)));
//G244: __E_constraint_69(170) => '[' ctnvar(161) _(11) '<' '=' _(11) num(162) ']'.
	p(NT(170), (T(26)+NT(161)+NT(11)+T(22)+T(3)+NT(11)+NT(162)+T(27)));
//G245: __E_constraint_69(170) => '[' num(162) _(11) '<' '=' _(11) ctnvar(161) ']'.
	p(NT(170), (T(26)+NT(162)+NT(11)+T(22)+T(3)+NT(11)+NT(161)+T(27)));
//G246: ctn_lteq(169)        => __E_constraint_69(170).
	p(NT(169), (NT(170)));
//G247: constraint(90)       => ctn_lteq(169).
	p(NT(90), (NT(169)));
//G248: __E_constraint_70(172) => '[' ctnvar(161) _(11) '<' _(11) num(162) ']'.
	p(NT(172), (T(26)+NT(161)+NT(11)+T(22)+NT(11)+NT(162)+T(27)));
//G249: __E_constraint_70(172) => '[' num(162) _(11) '<' _(11) ctnvar(161) ']'.
	p(NT(172), (T(26)+NT(162)+NT(11)+T(22)+NT(11)+NT(161)+T(27)));
//G250: ctn_lt(171)          => __E_constraint_70(172).
	p(NT(171), (NT(172)));
//G251: constraint(90)       => ctn_lt(171).
	p(NT(90), (NT(171)));
//G252: __E_bf_constant_71(173) => capture(26).
	p(NT(173), (NT(26)));
//G253: __E_bf_constant_71(173) => source(174).
	p(NT(173), (NT(174)));
//G254: __E_bf_constant_72(175) => typed(30).
	p(NT(175), (NT(30)));
//G255: __E_bf_constant_72(175) => null.
	p(NT(175), (nul));
//G256: bf_constant(131)     => '{' _(11) __E_bf_constant_71(173) _(11) '}' __E_bf_constant_72(175).
	p(NT(131), (T(42)+NT(11)+NT(173)+NT(11)+T(43)+NT(175)));
//G257: __E___E_source_73_74(178) => src_c(176).
	p(NT(178), (NT(176)));
//G258: __E___E_source_73_74(178) => space(2).
	p(NT(178), (NT(2)));
//G259: __E___E_source_73_75(179) => null.
	p(NT(179), (nul));
//G260: __E___E_source_73_75(179) => __E___E_source_73_74(178) __E___E_source_73_75(179).
	p(NT(179), (NT(178)+NT(179)));
//G261: __E_source_73(177)   => __E___E_source_73_75(179) src_c(176).
	p(NT(177), (NT(179)+NT(176)));
//G262: __E_source_73(177)   => null.
	p(NT(177), (nul));
//G263: source(174)          => src_c(176) __E_source_73(177).
	p(NT(174), (NT(176)+NT(177)));
//G264: src_c(176)           => alnum(6).
	p(NT(176), (NT(6)));
//G265: __N_0(373)           => '{'.
	p(NT(373), (T(42)));
//G266: __N_1(374)           => '}'.
	p(NT(374), (T(43)));
//G267: src_c(176)           => ~( __N_0(373) ) & ~( __N_1(374) ) & punct(7).	 # conjunctive
	p(NT(176), ~(NT(373)) & ~(NT(374)) & (NT(7)));
//G268: __E_src_c_76(180)    => src_c(176).
	p(NT(180), (NT(176)));
//G269: __E_src_c_76(180)    => space(2).
	p(NT(180), (NT(2)));
//G270: __E_src_c_77(181)    => null.
	p(NT(181), (nul));
//G271: __E_src_c_77(181)    => __E_src_c_76(180) __E_src_c_77(181).
	p(NT(181), (NT(180)+NT(181)));
//G272: src_c(176)           => '{' __E_src_c_77(181) '}'.
	p(NT(176), (T(42)+NT(181)+T(43)));
//G273: __E_variable_78(182) => uconst(183).
	p(NT(182), (NT(183)));
//G274: __E_variable_78(182) => io_var(184).
	p(NT(182), (NT(184)));
//G275: __E_variable_78(182) => var_name(185).
	p(NT(182), (NT(185)));
//G276: __E_variable_79(186) => typed(30).
	p(NT(186), (NT(30)));
//G277: __E_variable_79(186) => null.
	p(NT(186), (nul));
//G278: variable(132)        => __E_variable_78(182) __E_variable_79(186).
	p(NT(132), (NT(182)+NT(186)));
//G279: __N_2(375)           => 'F'.
	p(NT(375), (T(36)));
//G280: __N_3(376)           => 'T'.
	p(NT(376), (T(35)));
//G281: __E_var_name_80(187) => ~( __N_2(375) ) & ~( __N_3(376) ) & alpha(5).	 # conjunctive
	p(NT(187), ~(NT(375)) & ~(NT(376)) & (NT(5)));
//G282: __E_var_name_81(188) => null.
	p(NT(188), (nul));
//G283: __E_var_name_81(188) => digit(3) __E_var_name_81(188).
	p(NT(188), (NT(3)+NT(188)));
//G284: var_name(185)        => __E_var_name_80(187) __E_var_name_81(188).	 # guarded: charvar
	p(NT(185), (NT(187)+NT(188)));
	p.back().guard = "charvar";
//G285: __N_4(377)           => 'F'.
	p(NT(377), (T(36)));
//G286: __N_5(378)           => 'T'.
	p(NT(378), (T(35)));
//G287: __E_var_name_82(189) => ~( __N_4(377) ) & ~( __N_5(378) ) & alpha(5).	 # conjunctive
	p(NT(189), ~(NT(377)) & ~(NT(378)) & (NT(5)));
//G288: __E_var_name_83(190) => alnum(6).
	p(NT(190), (NT(6)));
//G289: __E_var_name_83(190) => '_'.
	p(NT(190), (T(44)));
//G290: __E_var_name_84(191) => null.
	p(NT(191), (nul));
//G291: __E_var_name_84(191) => __E_var_name_83(190) __E_var_name_84(191).
	p(NT(191), (NT(190)+NT(191)));
//G292: var_name(185)        => __E_var_name_82(189) __E_var_name_84(191).	 # guarded: var
	p(NT(185), (NT(189)+NT(191)));
	p.back().guard = "var";
//G293: io_var(184)          => io_var_name(28) '[' offset(192) ']'.
	p(NT(184), (NT(28)+T(26)+NT(192)+T(27)));
//G294: io_var_name(28)      => chars(193).
	p(NT(28), (NT(193)));
//G295: __E___E_uconst_85_86(196) => chars(193) _(11).
	p(NT(196), (NT(193)+NT(11)));
//G296: __E___E_uconst_85_86(196) => null.
	p(NT(196), (nul));
//G297: __E_uconst_85(195)   => __E___E_uconst_85_86(196) ':' _(11) chars(193).
	p(NT(195), (NT(196)+T(2)+NT(11)+NT(193)));
//G298: uconst_name(194)     => __E_uconst_85(195).
	p(NT(194), (NT(195)));
//G299: uconst(183)          => '<' _(11) uconst_name(194) _(11) '>'.
	p(NT(183), (T(22)+NT(11)+NT(194)+NT(11)+T(23)));
//G300: __E_q_vars_87(198)   => _(11) ',' _(11) q_var(197).
	p(NT(198), (NT(11)+T(20)+NT(11)+NT(197)));
//G301: __E_q_vars_88(199)   => null.
	p(NT(199), (nul));
//G302: __E_q_vars_88(199)   => __E_q_vars_87(198) __E_q_vars_88(199).
	p(NT(199), (NT(198)+NT(199)));
//G303: q_vars(86)           => q_var(197) __E_q_vars_88(199).
	p(NT(86), (NT(197)+NT(199)));
//G304: q_var(197)           => capture(26).
	p(NT(197), (NT(26)));
//G305: __N_6(379)           => uconst(183).
	p(NT(379), (NT(183)));
//G306: q_var(197)           => variable(132) & ~( __N_6(379) ).	 # conjunctive
	p(NT(197), (NT(132)) & ~(NT(379)));
//G307: ctnvar(161)          => chars(193).
	p(NT(161), (NT(193)));
//G308: __E_offsets_89(200)  => _(11) ',' _(11) offset(192).
	p(NT(200), (NT(11)+T(20)+NT(11)+NT(192)));
//G309: __E_offsets_90(201)  => null.
	p(NT(201), (nul));
//G310: __E_offsets_90(201)  => __E_offsets_89(200) __E_offsets_90(201).
	p(NT(201), (NT(200)+NT(201)));
//G311: offsets(39)          => '[' _(11) offset(192) __E_offsets_90(201) _(11) ']'.
	p(NT(39), (T(26)+NT(11)+NT(192)+NT(201)+NT(11)+T(27)));
//G312: offset(192)          => integer(202).
	p(NT(192), (NT(202)));
//G313: offset(192)          => capture(26).
	p(NT(192), (NT(26)));
//G314: offset(192)          => shift(203).
	p(NT(192), (NT(203)));
//G315: __N_7(380)           => io_var(184).
	p(NT(380), (NT(184)));
//G316: __E_offset_91(204)   => variable(132) & ~( __N_7(380) ).	 # conjunctive
	p(NT(204), (NT(132)) & ~(NT(380)));
//G317: offset(192)          => __E_offset_91(204).
	p(NT(192), (NT(204)));
//G318: __E_shift_92(205)    => capture(26).
	p(NT(205), (NT(26)));
//G319: __N_8(381)           => io_var(184).
	p(NT(381), (NT(184)));
//G320: __E___E_shift_92_93(206) => variable(132) & ~( __N_8(381) ).	 # conjunctive
	p(NT(206), (NT(132)) & ~(NT(381)));
//G321: __E_shift_92(205)    => __E___E_shift_92_93(206).
	p(NT(205), (NT(206)));
//G322: shift(203)           => __E_shift_92(205) _(11) '-' _(11) num(162).
	p(NT(203), (NT(205)+NT(11)+T(30)+NT(11)+NT(162)));
//G323: __E_chars_94(207)    => alnum(6).
	p(NT(207), (NT(6)));
//G324: __E_chars_94(207)    => '_'.
	p(NT(207), (T(44)));
//G325: __E_chars_95(208)    => null.
	p(NT(208), (nul));
//G326: __E_chars_95(208)    => __E_chars_94(207) __E_chars_95(208).
	p(NT(208), (NT(207)+NT(208)));
//G327: chars(193)           => alpha(5) __E_chars_95(208).
	p(NT(193), (NT(5)+NT(208)));
//G328: __E_digits_96(210)   => digit(3).
	p(NT(210), (NT(3)));
//G329: __E_digits_96(210)   => digit(3) __E_digits_96(210).
	p(NT(210), (NT(3)+NT(210)));
//G330: digits(209)          => __E_digits_96(210).
	p(NT(209), (NT(210)));
//G331: num(162)             => digits(209).
	p(NT(162), (NT(209)));
//G332: __E_integer_97(211)  => '-'.
	p(NT(211), (T(30)));
//G333: __E_integer_97(211)  => null.
	p(NT(211), (nul));
//G334: integer(202)         => __E_integer_97(211) _(11) digits(209).
	p(NT(202), (NT(211)+NT(11)+NT(209)));
//G335: sym(37)              => chars(193).
	p(NT(37), (NT(193)));
//G336: capture(26)          => '$' chars(193).
	p(NT(26), (T(45)+NT(193)));
//G337: type(212)            => chars(193).
	p(NT(212), (NT(193)));
//G338: typed(30)            => _(11) ':' _(11) type(212).
	p(NT(30), (NT(11)+T(2)+NT(11)+NT(212)));
//G339: scope_id(213)        => digits(209).
	p(NT(213), (NT(209)));
//G340: __E_comment_98(215)  => printable(8).
	p(NT(215), (NT(8)));
//G341: __E_comment_98(215)  => '\t'.
	p(NT(215), (T(46)));
//G342: __E_comment_99(216)  => null.
	p(NT(216), (nul));
//G343: __E_comment_99(216)  => __E_comment_98(215) __E_comment_99(216).
	p(NT(216), (NT(215)+NT(216)));
//G344: __E_comment_100(217) => '\n'.
	p(NT(217), (T(47)));
//G345: __E_comment_100(217) => '\r'.
	p(NT(217), (T(48)));
//G346: __E_comment_100(217) => eof(1).
	p(NT(217), (NT(1)));
//G347: comment(214)         => '#' __E_comment_99(216) __E_comment_100(217).
	p(NT(214), (T(49)+NT(216)+NT(217)));
//G348: __E____101(218)      => space(2).
	p(NT(218), (NT(2)));
//G349: __E____101(218)      => comment(214).
	p(NT(218), (NT(214)));
//G350: __(31)               => __E____101(218) _(11).
	p(NT(31), (NT(218)+NT(11)));
//G351: __E___102(219)       => __(31).
	p(NT(219), (NT(31)));
//G352: __E___102(219)       => null.
	p(NT(219), (nul));
//G353: _(11)                => __E___102(219).
	p(NT(11), (NT(219)));
//G354: cli(220)             => _(11).
	p(NT(220), (NT(11)));
//G355: __E_cli_103(222)     => '.' _(11) cli_command(221) _(11).
	p(NT(222), (T(1)+NT(11)+NT(221)+NT(11)));
//G356: __E_cli_104(223)     => null.
	p(NT(223), (nul));
//G357: __E_cli_104(223)     => __E_cli_103(222) __E_cli_104(223).
	p(NT(223), (NT(222)+NT(223)));
//G358: cli(220)             => _(11) cli_command(221) _(11) __E_cli_104(223).
	p(NT(220), (NT(11)+NT(221)+NT(11)+NT(223)));
//G359: __E_cli_command_105(226) => 'q'.
	p(NT(226), (T(50)));
//G360: __E_cli_command_105(226) => 'q' 'u' 'i' 't'.
	p(NT(226), (T(50)+T(7)+T(4)+T(8)));
//G361: quit_sym(225)        => __E_cli_command_105(226).
	p(NT(225), (NT(226)));
//G362: quit_cmd(224)        => quit_sym(225).
	p(NT(224), (NT(225)));
//G363: cli_command(221)     => quit_cmd(224).
	p(NT(221), (NT(224)));
//G364: __E_cli_command_106(229) => 'v'.
	p(NT(229), (T(51)));
//G365: __E_cli_command_106(229) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(229), (T(51)+T(11)+T(19)+T(15)+T(4)+T(6)+T(5)));
//G366: version_sym(228)     => __E_cli_command_106(229).
	p(NT(228), (NT(229)));
//G367: version_cmd(227)     => version_sym(228).
	p(NT(227), (NT(228)));
//G368: cli_command(221)     => version_cmd(227).
	p(NT(221), (NT(227)));
//G369: __E_cli_command_107(232) => 'c'.
	p(NT(232), (T(14)));
//G370: __E_cli_command_107(232) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(232), (T(14)+T(10)+T(11)+T(16)+T(19)));
//G371: clear_sym(231)       => __E_cli_command_107(232).
	p(NT(231), (NT(232)));
//G372: clear_cmd(230)       => clear_sym(231).
	p(NT(230), (NT(231)));
//G373: cli_command(221)     => clear_cmd(230).
	p(NT(221), (NT(230)));
//G374: __E___E_cli_command_108_109(236) => 'h'.
	p(NT(236), (T(52)));
//G375: __E___E_cli_command_108_109(236) => 'h' 'e' 'l' 'p'.
	p(NT(236), (T(52)+T(11)+T(10)+T(53)));
//G376: help_sym(235)        => __E___E_cli_command_108_109(236).
	p(NT(235), (NT(236)));
//G377: __E___E_cli_command_108_110(237) => __(31) help_arg(238).
	p(NT(237), (NT(31)+NT(238)));
//G378: __E___E_cli_command_108_110(237) => null.
	p(NT(237), (nul));
//G379: __E_cli_command_108(234) => help_sym(235) __E___E_cli_command_108_110(237).
	p(NT(234), (NT(235)+NT(237)));
//G380: help_cmd(233)        => __E_cli_command_108(234).
	p(NT(233), (NT(234)));
//G381: cli_command(221)     => help_cmd(233).
	p(NT(221), (NT(233)));
//G382: file_sym(241)        => 'f' 'i' 'l' 'e'.
	p(NT(241), (T(9)+T(4)+T(10)+T(11)));
//G383: __E_cli_command_111(240) => file_sym(241) __(31) q_string(242).
	p(NT(240), (NT(241)+NT(31)+NT(242)));
//G384: file_cmd(239)        => __E_cli_command_111(240).
	p(NT(239), (NT(240)));
//G385: cli_command(221)     => file_cmd(239).
	p(NT(221), (NT(239)));
//G386: valid_sym(245)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(245), (T(51)+T(16)+T(10)+T(4)+T(54)));
//G387: __E_cli_command_112(244) => valid_sym(245) __(31) normalize_cmd_arg(246).
	p(NT(244), (NT(245)+NT(31)+NT(246)));
//G388: valid_cmd(243)       => __E_cli_command_112(244).
	p(NT(243), (NT(244)));
//G389: cli_command(221)     => valid_cmd(243).
	p(NT(221), (NT(243)));
//G390: sat_sym(249)         => 's' 'a' 't'.
	p(NT(249), (T(15)+T(16)+T(8)));
//G391: __E_cli_command_113(248) => sat_sym(249) __(31) normalize_cmd_arg(246).
	p(NT(248), (NT(249)+NT(31)+NT(246)));
//G392: sat_cmd(247)         => __E_cli_command_113(248).
	p(NT(247), (NT(248)));
//G393: cli_command(221)     => sat_cmd(247).
	p(NT(221), (NT(247)));
//G394: unsat_sym(252)       => 'u' 'n' 's' 'a' 't'.
	p(NT(252), (T(7)+T(5)+T(15)+T(16)+T(8)));
//G395: __E_cli_command_114(251) => unsat_sym(252) __(31) normalize_cmd_arg(246).
	p(NT(251), (NT(252)+NT(31)+NT(246)));
//G396: unsat_cmd(250)       => __E_cli_command_114(251).
	p(NT(250), (NT(251)));
//G397: cli_command(221)     => unsat_cmd(250).
	p(NT(221), (NT(250)));
//G398: solve_sym(255)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(255), (T(15)+T(6)+T(10)+T(51)+T(11)));
//G399: __E___E_cli_command_115_116(256) => solve_options(257).
	p(NT(256), (NT(257)));
//G400: __E___E_cli_command_115_116(256) => null.
	p(NT(256), (nul));
//G401: __E_cli_command_115(254) => solve_sym(255) __E___E_cli_command_115_116(256) __(31) wff_cmd_arg(258).
	p(NT(254), (NT(255)+NT(256)+NT(31)+NT(258)));
//G402: solve_cmd(253)       => __E_cli_command_115(254).
	p(NT(253), (NT(254)));
//G403: cli_command(221)     => solve_cmd(253).
	p(NT(221), (NT(253)));
//G404: lgrs_sym(261)        => 'l' 'g' 'r' 's'.
	p(NT(261), (T(10)+T(55)+T(19)+T(15)));
//G405: __E_cli_command_117(260) => lgrs_sym(261) __(31) wff_cmd_arg(258).
	p(NT(260), (NT(261)+NT(31)+NT(258)));
//G406: lgrs_cmd(259)        => __E_cli_command_117(260).
	p(NT(259), (NT(260)));
//G407: cli_command(221)     => lgrs_cmd(259).
	p(NT(221), (NT(259)));
//G408: __E___E_cli_command_118_119(265) => 'r'.
	p(NT(265), (T(19)));
//G409: __E___E_cli_command_118_119(265) => 'r' 'u' 'n'.
	p(NT(265), (T(19)+T(7)+T(5)));
//G410: run_sym(264)         => __E___E_cli_command_118_119(265).
	p(NT(264), (NT(265)));
//G411: __E___E_cli_command_118_120(266) => history(267).
	p(NT(266), (NT(267)));
//G412: __E___E_cli_command_118_120(266) => wff(14).
	p(NT(266), (NT(14)));
//G413: __E_cli_command_118(263) => run_sym(264) __(31) __E___E_cli_command_118_120(266).
	p(NT(263), (NT(264)+NT(31)+NT(266)));
//G414: run_cmd(262)         => __E_cli_command_118(263).
	p(NT(262), (NT(263)));
//G415: cli_command(221)     => run_cmd(262).
	p(NT(221), (NT(262)));
//G416: __E___E_cli_command_121_122(271) => 'n'.
	p(NT(271), (T(5)));
//G417: __E___E_cli_command_121_122(271) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(271), (T(5)+T(6)+T(19)+T(21)+T(16)+T(10)+T(4)+T(56)+T(11)));
//G418: normalize_sym(270)   => __E___E_cli_command_121_122(271).
	p(NT(270), (NT(271)));
//G419: __E_cli_command_121(269) => normalize_sym(270) __(31) normalize_cmd_arg(246).
	p(NT(269), (NT(270)+NT(31)+NT(246)));
//G420: normalize_cmd(268)   => __E_cli_command_121(269).
	p(NT(268), (NT(269)));
//G421: cli_command(221)     => normalize_cmd(268).
	p(NT(221), (NT(268)));
//G422: __E___E_cli_command_123_124(275) => 's'.
	p(NT(275), (T(15)));
//G423: __E___E_cli_command_123_124(275) => 's' 'u' 'b' 's' 't'.
	p(NT(275), (T(15)+T(7)+T(17)+T(15)+T(8)));
//G424: __E___E_cli_command_123_124(275) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(275), (T(15)+T(7)+T(17)+T(15)+T(8)+T(4)+T(8)+T(7)+T(8)+T(11)));
//G425: subst_sym(274)       => __E___E_cli_command_123_124(275).
	p(NT(274), (NT(275)));
//G426: __E_cli_command_123(273) => subst_sym(274) __(31) nf_cmd_arg(276) _(11) '[' _(11) nf_cmd_arg(276) _(11) '/' _(11) nf_cmd_arg(276) _(11) ']'.
	p(NT(273), (NT(274)+NT(31)+NT(276)+NT(11)+T(26)+NT(11)+NT(276)+NT(11)+T(57)+NT(11)+NT(276)+NT(11)+T(27)));
//G427: subst_cmd(272)       => __E_cli_command_123(273).
	p(NT(272), (NT(273)));
//G428: cli_command(221)     => subst_cmd(272).
	p(NT(221), (NT(272)));
//G429: __E___E_cli_command_125_126(280) => 'i'.
	p(NT(280), (T(4)));
//G430: __E___E_cli_command_125_126(280) => 'i' 'n' 's' 't'.
	p(NT(280), (T(4)+T(5)+T(15)+T(8)));
//G431: __E___E_cli_command_125_126(280) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(280), (T(4)+T(5)+T(15)+T(8)+T(16)+T(5)+T(8)+T(4)+T(16)+T(8)+T(11)));
//G432: inst_sym(279)        => __E___E_cli_command_125_126(280).
	p(NT(279), (NT(280)));
//G433: __E_cli_command_125(278) => inst_sym(279) __(31) inst_args(281).
	p(NT(278), (NT(279)+NT(31)+NT(281)));
//G434: inst_cmd(277)        => __E_cli_command_125(278).
	p(NT(277), (NT(278)));
//G435: cli_command(221)     => inst_cmd(277).
	p(NT(221), (NT(277)));
//G436: dnf_sym(284)         => 'd' 'n' 'f'.
	p(NT(284), (T(54)+T(5)+T(9)));
//G437: __E_cli_command_127(283) => dnf_sym(284) __(31) nf_cmd_arg(276).
	p(NT(283), (NT(284)+NT(31)+NT(276)));
//G438: dnf_cmd(282)         => __E_cli_command_127(283).
	p(NT(282), (NT(283)));
//G439: cli_command(221)     => dnf_cmd(282).
	p(NT(221), (NT(282)));
//G440: cnf_sym(287)         => 'c' 'n' 'f'.
	p(NT(287), (T(14)+T(5)+T(9)));
//G441: __E_cli_command_128(286) => cnf_sym(287) __(31) nf_cmd_arg(276).
	p(NT(286), (NT(287)+NT(31)+NT(276)));
//G442: cnf_cmd(285)         => __E_cli_command_128(286).
	p(NT(285), (NT(286)));
//G443: cli_command(221)     => cnf_cmd(285).
	p(NT(221), (NT(285)));
//G444: anf_sym(290)         => 'a' 'n' 'f'.
	p(NT(290), (T(16)+T(5)+T(9)));
//G445: __E_cli_command_129(289) => anf_sym(290) __(31) nf_cmd_arg(276).
	p(NT(289), (NT(290)+NT(31)+NT(276)));
//G446: anf_cmd(288)         => __E_cli_command_129(289).
	p(NT(288), (NT(289)));
//G447: cli_command(221)     => anf_cmd(288).
	p(NT(221), (NT(288)));
//G448: nnf_sym(293)         => 'n' 'n' 'f'.
	p(NT(293), (T(5)+T(5)+T(9)));
//G449: __E_cli_command_130(292) => nnf_sym(293) __(31) nf_cmd_arg(276).
	p(NT(292), (NT(293)+NT(31)+NT(276)));
//G450: nnf_cmd(291)         => __E_cli_command_130(292).
	p(NT(291), (NT(292)));
//G451: cli_command(221)     => nnf_cmd(291).
	p(NT(221), (NT(291)));
//G452: pnf_sym(296)         => 'p' 'n' 'f'.
	p(NT(296), (T(53)+T(5)+T(9)));
//G453: __E_cli_command_131(295) => pnf_sym(296) __(31) nf_cmd_arg(276).
	p(NT(295), (NT(296)+NT(31)+NT(276)));
//G454: pnf_cmd(294)         => __E_cli_command_131(295).
	p(NT(294), (NT(295)));
//G455: cli_command(221)     => pnf_cmd(294).
	p(NT(221), (NT(294)));
//G456: mnf_sym(299)         => 'm' 'n' 'f'.
	p(NT(299), (T(21)+T(5)+T(9)));
//G457: __E_cli_command_132(298) => mnf_sym(299) __(31) nf_cmd_arg(276).
	p(NT(298), (NT(299)+NT(31)+NT(276)));
//G458: mnf_cmd(297)         => __E_cli_command_132(298).
	p(NT(297), (NT(298)));
//G459: cli_command(221)     => mnf_cmd(297).
	p(NT(221), (NT(297)));
//G460: snf_sym(302)         => 's' 'n' 'f'.
	p(NT(302), (T(15)+T(5)+T(9)));
//G461: __E_cli_command_133(301) => snf_sym(302) __(31) nf_cmd_arg(276).
	p(NT(301), (NT(302)+NT(31)+NT(276)));
//G462: snf_cmd(300)         => __E_cli_command_133(301).
	p(NT(300), (NT(301)));
//G463: cli_command(221)     => snf_cmd(300).
	p(NT(221), (NT(300)));
//G464: onf_sym(305)         => 'o' 'n' 'f'.
	p(NT(305), (T(6)+T(5)+T(9)));
//G465: __E_cli_command_134(304) => onf_sym(305) __(31) variable(132) __(31) onf_cmd_arg(306).
	p(NT(304), (NT(305)+NT(31)+NT(132)+NT(31)+NT(306)));
//G466: onf_cmd(303)         => __E_cli_command_134(304).
	p(NT(303), (NT(304)));
//G467: cli_command(221)     => onf_cmd(303).
	p(NT(221), (NT(303)));
//G468: qelim_sym(309)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(309), (T(50)+T(11)+T(10)+T(4)+T(21)));
//G469: __E_cli_command_135(308) => qelim_sym(309) __(31) wff_cmd_arg(258).
	p(NT(308), (NT(309)+NT(31)+NT(258)));
//G470: qelim_cmd(307)       => __E_cli_command_135(308).
	p(NT(307), (NT(308)));
//G471: cli_command(221)     => qelim_cmd(307).
	p(NT(221), (NT(307)));
//G472: get_sym(312)         => 'g' 'e' 't'.
	p(NT(312), (T(55)+T(11)+T(8)));
//G473: __E___E_cli_command_136_137(313) => __(31) option_name(314).
	p(NT(313), (NT(31)+NT(314)));
//G474: __E___E_cli_command_136_137(313) => null.
	p(NT(313), (nul));
//G475: __E_cli_command_136(311) => get_sym(312) __E___E_cli_command_136_137(313).
	p(NT(311), (NT(312)+NT(313)));
//G476: get_cmd(310)         => __E_cli_command_136(311).
	p(NT(310), (NT(311)));
//G477: cli_command(221)     => get_cmd(310).
	p(NT(221), (NT(310)));
//G478: set_sym(317)         => 's' 'e' 't'.
	p(NT(317), (T(15)+T(11)+T(8)));
//G479: __E___E_cli_command_138_139(318) => __(31).
	p(NT(318), (NT(31)));
//G480: __E___E_cli_command_138_139(318) => _(11) '=' _(11).
	p(NT(318), (NT(11)+T(3)+NT(11)));
//G481: __E_cli_command_138(316) => set_sym(317) __(31) option_name(314) __E___E_cli_command_138_139(318) option_value(319).
	p(NT(316), (NT(317)+NT(31)+NT(314)+NT(318)+NT(319)));
//G482: set_cmd(315)         => __E_cli_command_138(316).
	p(NT(315), (NT(316)));
//G483: cli_command(221)     => set_cmd(315).
	p(NT(221), (NT(315)));
//G484: enable_sym(322)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(322), (T(11)+T(5)+T(16)+T(17)+T(10)+T(11)));
//G485: __E_cli_command_140(321) => enable_sym(322) __(31) option_name(314).
	p(NT(321), (NT(322)+NT(31)+NT(314)));
//G486: enable_cmd(320)      => __E_cli_command_140(321).
	p(NT(320), (NT(321)));
//G487: cli_command(221)     => enable_cmd(320).
	p(NT(221), (NT(320)));
//G488: disable_sym(325)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(325), (T(54)+T(4)+T(15)+T(16)+T(17)+T(10)+T(11)));
//G489: __E_cli_command_141(324) => disable_sym(325) __(31) option_name(314).
	p(NT(324), (NT(325)+NT(31)+NT(314)));
//G490: disable_cmd(323)     => __E_cli_command_141(324).
	p(NT(323), (NT(324)));
//G491: cli_command(221)     => disable_cmd(323).
	p(NT(221), (NT(323)));
//G492: toggle_sym(328)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(328), (T(8)+T(6)+T(55)+T(55)+T(10)+T(11)));
//G493: __E_cli_command_142(327) => toggle_sym(328) __(31) option_name(314).
	p(NT(327), (NT(328)+NT(31)+NT(314)));
//G494: toggle_cmd(326)      => __E_cli_command_142(327).
	p(NT(326), (NT(327)));
//G495: cli_command(221)     => toggle_cmd(326).
	p(NT(221), (NT(326)));
//G496: __E___E_cli_command_143_144(332) => 'd' 'e' 'f' 's'.
	p(NT(332), (T(54)+T(11)+T(9)+T(15)));
//G497: __E___E_cli_command_143_144(332) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(332), (T(54)+T(11)+T(9)+T(4)+T(5)+T(4)+T(8)+T(4)+T(6)+T(5)+T(15)));
//G498: def_sym(331)         => __E___E_cli_command_143_144(332).
	p(NT(331), (NT(332)));
//G499: __E_cli_command_143(330) => def_sym(331).
	p(NT(330), (NT(331)));
//G500: def_list_cmd(329)    => __E_cli_command_143(330).
	p(NT(329), (NT(330)));
//G501: cli_command(221)     => def_list_cmd(329).
	p(NT(221), (NT(329)));
//G502: __E_cli_command_145(334) => def_sym(331) __(31) num(162).
	p(NT(334), (NT(331)+NT(31)+NT(162)));
//G503: def_print_cmd(333)   => __E_cli_command_145(334).
	p(NT(333), (NT(334)));
//G504: cli_command(221)     => def_print_cmd(333).
	p(NT(221), (NT(333)));
//G505: def_rr_cmd(335)      => rec_relation(17).
	p(NT(335), (NT(17)));
//G506: cli_command(221)     => def_rr_cmd(335).
	p(NT(221), (NT(335)));
//G507: def_input_cmd(336)   => input_def(18).
	p(NT(336), (NT(18)));
//G508: cli_command(221)     => def_input_cmd(336).
	p(NT(221), (NT(336)));
//G509: def_output_cmd(337)  => output_def(19).
	p(NT(337), (NT(19)));
//G510: cli_command(221)     => def_output_cmd(337).
	p(NT(221), (NT(337)));
//G511: __E___E_cli_command_146_147(341) => 'h' 'i' 's' 't'.
	p(NT(341), (T(52)+T(4)+T(15)+T(8)));
//G512: __E___E_cli_command_146_147(341) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(341), (T(52)+T(4)+T(15)+T(8)+T(6)+T(19)+T(25)));
//G513: history_sym(340)     => __E___E_cli_command_146_147(341).
	p(NT(340), (NT(341)));
//G514: __E_cli_command_146(339) => history_sym(340).
	p(NT(339), (NT(340)));
//G515: history_list_cmd(338) => __E_cli_command_146(339).
	p(NT(338), (NT(339)));
//G516: cli_command(221)     => history_list_cmd(338).
	p(NT(221), (NT(338)));
//G517: __E_cli_command_148(343) => history_sym(340) __(31) history(267).
	p(NT(343), (NT(340)+NT(31)+NT(267)));
//G518: history_print_cmd(342) => __E_cli_command_148(343).
	p(NT(342), (NT(343)));
//G519: cli_command(221)     => history_print_cmd(342).
	p(NT(221), (NT(342)));
//G520: __E_cli_command_149(345) => wff(14).
	p(NT(345), (NT(14)));
//G521: __E_cli_command_149(345) => bf(27).
	p(NT(345), (NT(27)));
//G522: history_store_cmd(344) => __E_cli_command_149(345).
	p(NT(344), (NT(345)));
//G523: cli_command(221)     => history_store_cmd(344).
	p(NT(221), (NT(344)));
//G524: __E_solve_options_150(346) => __(31) solve_option(347).
	p(NT(346), (NT(31)+NT(347)));
//G525: __E_solve_options_151(348) => null.
	p(NT(348), (nul));
//G526: __E_solve_options_151(348) => __E_solve_options_150(346) __E_solve_options_151(348).
	p(NT(348), (NT(346)+NT(348)));
//G527: solve_options(257)   => __E_solve_options_151(348).
	p(NT(257), (NT(348)));
//G528: __E_solve_option_152(349) => solver_mode(350).
	p(NT(349), (NT(350)));
//G529: __E_solve_option_152(349) => type(212).
	p(NT(349), (NT(212)));
//G530: solve_option(347)    => '-' '-' __E_solve_option_152(349).
	p(NT(347), (T(30)+T(30)+NT(349)));
//G531: __E_solver_mode_153(352) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(352), (T(21)+T(4)+T(5)+T(4)+T(21)+T(7)+T(21)));
//G532: __E_solver_mode_153(352) => 'm' 'i' 'n'.
	p(NT(352), (T(21)+T(4)+T(5)));
//G533: solver_mode_minimum(351) => __E_solver_mode_153(352).
	p(NT(351), (NT(352)));
//G534: solver_mode(350)     => solver_mode_minimum(351).
	p(NT(350), (NT(351)));
//G535: __E_solver_mode_154(354) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(354), (T(21)+T(16)+T(29)+T(4)+T(21)+T(7)+T(21)));
//G536: __E_solver_mode_154(354) => 'm' 'a' 'x'.
	p(NT(354), (T(21)+T(16)+T(29)));
//G537: solver_mode_maximum(353) => __E_solver_mode_154(354).
	p(NT(353), (NT(354)));
//G538: solver_mode(350)     => solver_mode_maximum(353).
	p(NT(350), (NT(353)));
//G539: q_file_name(35)      => '"' file_name(355) '"'.
	p(NT(35), (T(58)+NT(355)+T(58)));
//G540: __E_file_name_155(356) => printable(8).
	p(NT(356), (NT(8)));
//G541: __E_file_name_155(356) => printable(8) __E_file_name_155(356).
	p(NT(356), (NT(8)+NT(356)));
//G542: file_name(355)       => __E_file_name_155(356).
	p(NT(355), (NT(356)));
//G543: __E_option_name_156(357) => alnum(6).
	p(NT(357), (NT(6)));
//G544: __E_option_name_156(357) => alnum(6) __E_option_name_156(357).
	p(NT(357), (NT(6)+NT(357)));
//G545: option_name(314)     => __E_option_name_156(357).
	p(NT(314), (NT(357)));
//G546: __E_option_value_157(358) => alnum(6).
	p(NT(358), (NT(6)));
//G547: __E_option_value_157(358) => alnum(6) __E_option_value_157(358).
	p(NT(358), (NT(6)+NT(358)));
//G548: option_value(319)    => __E_option_value_157(358).
	p(NT(319), (NT(358)));
//G549: bf_cmd_arg(359)      => history(267).
	p(NT(359), (NT(267)));
//G550: bf_cmd_arg(359)      => bf(27).
	p(NT(359), (NT(27)));
//G551: wff_cmd_arg(258)     => history(267).
	p(NT(258), (NT(267)));
//G552: wff_cmd_arg(258)     => wff(14).
	p(NT(258), (NT(14)));
//G553: nf_cmd_arg(276)      => history(267).
	p(NT(276), (NT(267)));
//G554: nf_cmd_arg(276)      => ref(24).
	p(NT(276), (NT(24)));
//G555: nf_cmd_arg(276)      => wff(14).
	p(NT(276), (NT(14)));
//G556: nf_cmd_arg(276)      => bf(27).
	p(NT(276), (NT(27)));
//G557: onf_cmd_arg(306)     => history(267).
	p(NT(306), (NT(267)));
//G558: onf_cmd_arg(306)     => wff(14).
	p(NT(306), (NT(14)));
//G559: normalize_cmd_arg(246) => history(267).
	p(NT(246), (NT(267)));
//G560: normalize_cmd_arg(246) => spec(10).
	p(NT(246), (NT(10)));
//G561: normalize_cmd_arg(246) => ref(24).
	p(NT(246), (NT(24)));
//G562: normalize_cmd_arg(246) => wff(14).
	p(NT(246), (NT(14)));
//G563: normalize_cmd_arg(246) => bf(27).
	p(NT(246), (NT(27)));
//G564: inst_args(281)       => wff_cmd_arg(258) _(11) '[' _(11) variable(132) _(11) '/' _(11) bf_cmd_arg(359) _(11) ']'.
	p(NT(281), (NT(258)+NT(11)+T(26)+NT(11)+NT(132)+NT(11)+T(57)+NT(11)+NT(359)+NT(11)+T(27)));
//G565: inst_args(281)       => bf_cmd_arg(359) _(11) '[' _(11) variable(132) _(11) '/' _(11) bf_cmd_arg(359) _(11) ']'.
	p(NT(281), (NT(359)+NT(11)+T(26)+NT(11)+NT(132)+NT(11)+T(57)+NT(11)+NT(359)+NT(11)+T(27)));
//G566: help_arg(238)        => help_sym(235).
	p(NT(238), (NT(235)));
//G567: help_arg(238)        => version_sym(228).
	p(NT(238), (NT(228)));
//G568: help_arg(238)        => quit_sym(225).
	p(NT(238), (NT(225)));
//G569: help_arg(238)        => clear_sym(231).
	p(NT(238), (NT(231)));
//G570: help_arg(238)        => get_sym(312).
	p(NT(238), (NT(312)));
//G571: help_arg(238)        => set_sym(317).
	p(NT(238), (NT(317)));
//G572: help_arg(238)        => enable_sym(322).
	p(NT(238), (NT(322)));
//G573: help_arg(238)        => disable_sym(325).
	p(NT(238), (NT(325)));
//G574: help_arg(238)        => toggle_sym(328).
	p(NT(238), (NT(328)));
//G575: help_arg(238)        => file_sym(241).
	p(NT(238), (NT(241)));
//G576: help_arg(238)        => history_sym(340).
	p(NT(238), (NT(340)));
//G577: help_arg(238)        => abs_history_sym(360).
	p(NT(238), (NT(360)));
//G578: help_arg(238)        => rel_history_sym(361).
	p(NT(238), (NT(361)));
//G579: help_arg(238)        => selection_sym(362).
	p(NT(238), (NT(362)));
//G580: help_arg(238)        => def_sym(331).
	p(NT(238), (NT(331)));
//G581: help_arg(238)        => inst_sym(279).
	p(NT(238), (NT(279)));
//G582: help_arg(238)        => subst_sym(274).
	p(NT(238), (NT(274)));
//G583: help_arg(238)        => normalize_sym(270).
	p(NT(238), (NT(270)));
//G584: help_arg(238)        => execute_sym(363).
	p(NT(238), (NT(363)));
//G585: help_arg(238)        => solve_sym(255).
	p(NT(238), (NT(255)));
//G586: help_arg(238)        => valid_sym(245).
	p(NT(238), (NT(245)));
//G587: help_arg(238)        => sat_sym(249).
	p(NT(238), (NT(249)));
//G588: help_arg(238)        => unsat_sym(252).
	p(NT(238), (NT(252)));
//G589: help_arg(238)        => run_sym(264).
	p(NT(238), (NT(264)));
//G590: help_arg(238)        => dnf_sym(284).
	p(NT(238), (NT(284)));
//G591: help_arg(238)        => cnf_sym(287).
	p(NT(238), (NT(287)));
//G592: help_arg(238)        => anf_sym(290).
	p(NT(238), (NT(290)));
//G593: help_arg(238)        => snf_sym(302).
	p(NT(238), (NT(302)));
//G594: help_arg(238)        => nnf_sym(293).
	p(NT(238), (NT(293)));
//G595: help_arg(238)        => pnf_sym(296).
	p(NT(238), (NT(296)));
//G596: help_arg(238)        => mnf_sym(299).
	p(NT(238), (NT(299)));
//G597: help_arg(238)        => onf_sym(305).
	p(NT(238), (NT(305)));
//G598: help_arg(238)        => qelim_sym(309).
	p(NT(238), (NT(309)));
//G599: __E___E_help_arg_158_159(366) => 's'.
	p(NT(366), (T(15)));
//G600: __E___E_help_arg_158_159(366) => null.
	p(NT(366), (nul));
//G601: __E_help_arg_158(365) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_158_159(366).
	p(NT(365), (T(11)+T(29)+T(16)+T(21)+T(53)+T(10)+T(11)+NT(366)));
//G602: examples_sym(364)    => __E_help_arg_158(365).
	p(NT(364), (NT(365)));
//G603: help_arg(238)        => examples_sym(364).
	p(NT(238), (NT(364)));
//G604: __E_history_160(368) => '%'.
	p(NT(368), (T(59)));
//G605: rel_history_sym(361) => '%' '-'.
	p(NT(361), (T(59)+T(30)));
//G606: history_id(370)      => digits(209).
	p(NT(370), (NT(209)));
//G607: __E___E_history_160_161(369) => history_id(370).
	p(NT(369), (NT(370)));
//G608: __E___E_history_160_161(369) => null.
	p(NT(369), (nul));
//G609: __E_history_160(368) => rel_history_sym(361) __E___E_history_160_161(369).
	p(NT(368), (NT(361)+NT(369)));
//G610: rel_history(367)     => __E_history_160(368).
	p(NT(367), (NT(368)));
//G611: history(267)         => rel_history(367).
	p(NT(267), (NT(367)));
//G612: abs_history_sym(360) => '%'.
	p(NT(360), (T(59)));
//G613: __E_history_162(372) => abs_history_sym(360) history_id(370).
	p(NT(372), (NT(360)+NT(370)));
//G614: abs_history(371)     => __E_history_162(372).
	p(NT(371), (NT(372)));
//G615: history(267)         => abs_history(371).
	p(NT(267), (NT(371)));
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
		__E_wff_45, bf_gteq, __E_wff_46, bf_ngteq, __E_wff_47, bf_lt, __E_wff_48, bf_nlt, __E_wff_49, bf_parenthesis, 
		__E_bf_50, bf_constant, variable, bf_splitter, __E_bf_51, bf_ref, bf_or, __E_bf_52, bf_xor, __E_bf_53, 
		bf_t, __E_bf_54, __E___E_bf_54_55, bf_f, __E_bf_56, __E___E_bf_56_57, bf_and, __E_bf_58, __E___E_bf_58_59, bf_neg, 
		__E_bf_60, bf_neg_oprnd, __E___E_bf_60_61, bf_and_nosep, __E_bf_62, bf_and_nosep_1st_oprnd, __E___E_bf_62_63, bf_and_nosep_2nd_oprnd, __E___E_bf_62_64, ctn_neq, 
		__E_constraint_65, ctnvar, num, ctn_eq, __E_constraint_66, ctn_gteq, __E_constraint_67, ctn_gt, __E_constraint_68, ctn_lteq, 
		__E_constraint_69, ctn_lt, __E_constraint_70, __E_bf_constant_71, source, __E_bf_constant_72, src_c, __E_source_73, __E___E_source_73_74, __E___E_source_73_75, 
		__E_src_c_76, __E_src_c_77, __E_variable_78, uconst, io_var, var_name, __E_variable_79, __E_var_name_80, __E_var_name_81, __E_var_name_82, 
		__E_var_name_83, __E_var_name_84, offset, chars, uconst_name, __E_uconst_85, __E___E_uconst_85_86, q_var, __E_q_vars_87, __E_q_vars_88, 
		__E_offsets_89, __E_offsets_90, integer, shift, __E_offset_91, __E_shift_92, __E___E_shift_92_93, __E_chars_94, __E_chars_95, digits, 
		__E_digits_96, __E_integer_97, type, scope_id, comment, __E_comment_98, __E_comment_99, __E_comment_100, __E____101, __E___102, 
		cli, cli_command, __E_cli_103, __E_cli_104, quit_cmd, quit_sym, __E_cli_command_105, version_cmd, version_sym, __E_cli_command_106, 
		clear_cmd, clear_sym, __E_cli_command_107, help_cmd, __E_cli_command_108, help_sym, __E___E_cli_command_108_109, __E___E_cli_command_108_110, help_arg, file_cmd, 
		__E_cli_command_111, file_sym, q_string, valid_cmd, __E_cli_command_112, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_113, sat_sym, 
		unsat_cmd, __E_cli_command_114, unsat_sym, solve_cmd, __E_cli_command_115, solve_sym, __E___E_cli_command_115_116, solve_options, wff_cmd_arg, lgrs_cmd, 
		__E_cli_command_117, lgrs_sym, run_cmd, __E_cli_command_118, run_sym, __E___E_cli_command_118_119, __E___E_cli_command_118_120, history, normalize_cmd, __E_cli_command_121, 
		normalize_sym, __E___E_cli_command_121_122, subst_cmd, __E_cli_command_123, subst_sym, __E___E_cli_command_123_124, nf_cmd_arg, inst_cmd, __E_cli_command_125, inst_sym, 
		__E___E_cli_command_125_126, inst_args, dnf_cmd, __E_cli_command_127, dnf_sym, cnf_cmd, __E_cli_command_128, cnf_sym, anf_cmd, __E_cli_command_129, 
		anf_sym, nnf_cmd, __E_cli_command_130, nnf_sym, pnf_cmd, __E_cli_command_131, pnf_sym, mnf_cmd, __E_cli_command_132, mnf_sym, 
		snf_cmd, __E_cli_command_133, snf_sym, onf_cmd, __E_cli_command_134, onf_sym, onf_cmd_arg, qelim_cmd, __E_cli_command_135, qelim_sym, 
		get_cmd, __E_cli_command_136, get_sym, __E___E_cli_command_136_137, option_name, set_cmd, __E_cli_command_138, set_sym, __E___E_cli_command_138_139, option_value, 
		enable_cmd, __E_cli_command_140, enable_sym, disable_cmd, __E_cli_command_141, disable_sym, toggle_cmd, __E_cli_command_142, toggle_sym, def_list_cmd, 
		__E_cli_command_143, def_sym, __E___E_cli_command_143_144, def_print_cmd, __E_cli_command_145, def_rr_cmd, def_input_cmd, def_output_cmd, history_list_cmd, __E_cli_command_146, 
		history_sym, __E___E_cli_command_146_147, history_print_cmd, __E_cli_command_148, history_store_cmd, __E_cli_command_149, __E_solve_options_150, solve_option, __E_solve_options_151, __E_solve_option_152, 
		solver_mode, solver_mode_minimum, __E_solver_mode_153, solver_mode_maximum, __E_solver_mode_154, file_name, __E_file_name_155, __E_option_name_156, __E_option_value_157, bf_cmd_arg, 
		abs_history_sym, rel_history_sym, selection_sym, execute_sym, examples_sym, __E_help_arg_158, __E___E_help_arg_158_159, rel_history, __E_history_160, __E___E_history_160_161, 
		history_id, abs_history, __E_history_162, __N_0, __N_1, __N_2, __N_3, __N_4, __N_5, __N_6, 
		__N_7, __N_8, 
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
