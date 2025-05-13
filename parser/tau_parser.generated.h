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
	"spec", "_", "rec_relations", "main", "wff", "__E_rec_relations_0", "rec_relation", "__E_rec_relations_1", "ref", "__E_rec_relation_2", 
	"capture", "bf", "sym", "__E_ref_3", "offsets", "ref_args", "__E_ref_4", "__", "fp_fallback", "first_sym", 
	"last_sym", "__E_ref_args_5", "__E___E_ref_args_5_6", "ref_arg", "__E___E_ref_args_5_7", "__E___E_ref_args_5_8", "library", "rules", "__E_library_9", "__E___E_library_9_10", 
	"rule", "__E___E___E_library_9_10_11", "wff_rule", "bf_rule", "__E___E_library_9_12", "wff_matcher", "wff_body", "bf_matcher", "bf_body", "builder", 
	"builder_head", "builder_body", "__E_builder_head_13", "__E_builder_head_14", "bf_builder_body", "__E_builder_body_15", "wff_builder_body", "__E_builder_body_16", "tau_constant_source", "__E_tau_constant_source_17", 
	"wff_parenthesis", "__E_wff_18", "wff_sometimes", "__E_wff_19", "__E___E_wff_19_20", "wff_always", "__E_wff_21", "__E___E_wff_21_22", "wff_conditional", "__E_wff_23", 
	"wff_all", "__E_wff_24", "q_vars", "wff_ex", "__E_wff_25", "wff_ref", "constraint", "wff_imply", "__E_wff_26", "wff_rimply", 
	"__E_wff_27", "wff_equiv", "__E_wff_28", "wff_or", "__E_wff_29", "wff_xor", "__E_wff_30", "wff_and", "__E_wff_31", "wff_neg", 
	"__E_wff_32", "wff_t", "wff_f", "bf_interval", "__E_wff_33", "bf_eq", "__E_wff_34", "bf_neq", "__E_wff_35", "bf_lteq", 
	"__E_wff_36", "bf_nlteq", "__E_wff_37", "bf_gt", "__E_wff_38", "bf_ngt", "__E_wff_39", "bf_gteq", "__E_wff_40", "bf_ngteq", 
	"__E_wff_41", "bf_lt", "__E_wff_42", "bf_nlt", "__E_wff_43", "bf_parenthesis", "__E_bf_44", "bf_constant", "variable", "bf_splitter", 
	"__E_bf_45", "bf_ref", "bf_or", "__E_bf_46", "bf_xor", "__E_bf_47", "bf_t", "__E_bf_48", "__E___E_bf_48_49", "typed", 
	"bf_f", "__E_bf_50", "__E___E_bf_50_51", "bf_and", "__E_bf_52", "__E___E_bf_52_53", "bf_neg", "__E_bf_54", "bf_neg_oprnd", "__E___E_bf_54_55", 
	"bf_and_nosep", "__E_bf_56", "bf_and_nosep_1st_oprnd", "__E___E_bf_56_57", "bf_and_nosep_2nd_oprnd", "__E___E_bf_56_58", "ctn_neq", "__E_constraint_59", "ctnvar", "num", 
	"ctn_eq", "__E_constraint_60", "ctn_gteq", "__E_constraint_61", "ctn_gt", "__E_constraint_62", "ctn_lteq", "__E_constraint_63", "ctn_lt", "__E_constraint_64", 
	"__E_bf_constant_65", "source", "__E_bf_constant_66", "src_c", "__E_source_67", "__E___E_source_67_68", "__E___E_source_67_69", "__E_src_c_70", "__E_src_c_71", "__E_variable_72", 
	"uconst", "io_var", "var_name", "__E_variable_73", "__E_var_name_74", "__E_var_name_75", "__E_var_name_76", "__E_var_name_77", "__E_var_name_78", "offset", 
	"uconst_name", "__E_uconst_79", "__E___E_uconst_79_80", "chars", "q_var", "__E_q_vars_81", "__E_q_vars_82", "__E_offsets_83", "__E_offsets_84", "integer", 
	"shift", "__E_offset_85", "__E_shift_86", "__E___E_shift_86_87", "__E_chars_88", "__E_chars_89", "digits", "__E_digits_90", "__E_integer_91", "type", 
	"scope_id", "comment", "__E_comment_92", "__E_comment_93", "__E_comment_94", "__E____95", "__E___96", "cli", "cli_command", "__E_cli_97", 
	"__E_cli_98", "quit_cmd", "quit_sym", "__E_cli_command_99", "version_cmd", "version_sym", "__E_cli_command_100", "clear_cmd", "clear_sym", "__E_cli_command_101", 
	"help_cmd", "__E_cli_command_102", "help_sym", "__E___E_cli_command_102_103", "__E___E_cli_command_102_104", "help_arg", "file_cmd", "__E_cli_command_105", "file_sym", "q_string", 
	"valid_cmd", "__E_cli_command_106", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_107", "sat_sym", "unsat_cmd", "__E_cli_command_108", "unsat_sym", 
	"solve_cmd", "__E_cli_command_109", "solve_sym", "__E___E_cli_command_109_110", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_111", "lgrs_sym", "run_cmd", 
	"__E_cli_command_112", "run_sym", "__E___E_cli_command_112_113", "__E___E_cli_command_112_114", "history", "normalize_cmd", "__E_cli_command_115", "normalize_sym", "__E___E_cli_command_115_116", "subst_cmd", 
	"__E_cli_command_117", "subst_sym", "__E___E_cli_command_117_118", "nf_cmd_arg", "inst_cmd", "__E_cli_command_119", "inst_sym", "__E___E_cli_command_119_120", "inst_args", "dnf_cmd", 
	"__E_cli_command_121", "dnf_sym", "cnf_cmd", "__E_cli_command_122", "cnf_sym", "anf_cmd", "__E_cli_command_123", "anf_sym", "nnf_cmd", "__E_cli_command_124", 
	"nnf_sym", "pnf_cmd", "__E_cli_command_125", "pnf_sym", "mnf_cmd", "__E_cli_command_126", "mnf_sym", "snf_cmd", "__E_cli_command_127", "snf_sym", 
	"onf_cmd", "__E_cli_command_128", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_129", "def_sym", "__E___E_cli_command_129_130", "def_print_cmd", "__E_cli_command_131", 
	"def_rr_cmd", "qelim_cmd", "__E_cli_command_132", "qelim_sym", "get_cmd", "__E_cli_command_133", "get_sym", "__E___E_cli_command_133_134", "option_name", "set_cmd", 
	"__E_cli_command_135", "set_sym", "__E___E_cli_command_135_136", "option_value", "enable_cmd", "__E_cli_command_137", "enable_sym", "disable_cmd", "__E_cli_command_138", "disable_sym", 
	"toggle_cmd", "__E_cli_command_139", "toggle_sym", "def_input_cmd", "__E_cli_command_140", "input_stream", "def_output_cmd", "__E_cli_command_141", "output_stream", "history_list_cmd", 
	"__E_cli_command_142", "history_sym", "__E___E_cli_command_142_143", "history_print_cmd", "__E_cli_command_144", "history_store_cmd", "__E_cli_command_145", "__E_solve_options_146", "solve_option", "__E_solve_options_147", 
	"__E_solve_option_148", "solver_mode", "solver_mode_minimum", "__E_solver_mode_149", "solver_mode_maximum", "__E_solver_mode_150", "__E_input_stream_151", "q_file_name", "console_sym", "__E_output_stream_152", 
	"file_name", "__E_file_name_153", "__E_option_name_154", "__E_option_value_155", "bf_cmd_arg", "abs_history_sym", "rel_history_sym", "selection_sym", "execute_sym", "examples_sym", 
	"__E_help_arg_156", "__E___E_help_arg_156_157", "rel_history", "__E_history_158", "__E___E_history_158_159", "history_id", "abs_history", "__E_history_160", "__N_0", "__N_1", 
	"__N_2", "__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'f', 'a', 'l', 'b', 'c', 
	'k', 'i', 'r', 's', 't', ',', '(', ')', 'o', 'm', 
	'e', '<', '>', 'w', 'y', '[', ']', '?', 'x', '-', 
	'|', '^', '&', '!', 'T', 'F', 'S', '+', '1', '0', 
	'\'', '{', '}', '_', '$', '\t', '\n', '\r', '#', 'q', 
	'u', 'v', 'n', 'h', 'p', 'd', 'g', 'z', '/', '"', 
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
			11, 27
		},
		.trim_terminals = true,
		.dont_trim_terminals_of = {
			20, 22, 148, 161, 172, 180, 189, 196, 199, 308,
			313, 350
		},
		.to_inline = {
			{ 14, 60, 14 },
			{ 21, 115, 21 },
			{ 129 },
			{ 138, 115, 21 },
			{ 140, 115, 133 },
			{ 142, 115, 21 },
			{ 144, 115, 21 },
			{ 163 },
			{ 170 },
			{ 170, 180 },
			{ 183 },
			{ 184 },
			{ 233 },
			{ 245 },
			{ 263 },
			{ 268 },
			{ 293 },
			{ 354 }
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
//G2:   spec(10)             => rec_relations(12) _(11) main(13) _(11) '.'.
	p(NT(10), (NT(12)+NT(11)+NT(13)+NT(11)+T(1)));
//G3:   __E_rec_relations_0(15) => _(11) rec_relation(16) _(11) '.'.
	p(NT(15), (NT(11)+NT(16)+NT(11)+T(1)));
//G4:   __E_rec_relations_1(17) => null.
	p(NT(17), (nul));
//G5:   __E_rec_relations_1(17) => __E_rec_relations_0(15) __E_rec_relations_1(17).
	p(NT(17), (NT(15)+NT(17)));
//G6:   rec_relations(12)    => __E_rec_relations_1(17).
	p(NT(12), (NT(17)));
//G7:   __E_rec_relation_2(19) => capture(20).
	p(NT(19), (NT(20)));
//G8:   __E_rec_relation_2(19) => ref(18).
	p(NT(19), (NT(18)));
//G9:   __E_rec_relation_2(19) => wff(14).
	p(NT(19), (NT(14)));
//G10:  __E_rec_relation_2(19) => bf(21).
	p(NT(19), (NT(21)));
//G11:  rec_relation(16)     => ref(18) _(11) ':' '=' _(11) __E_rec_relation_2(19).
	p(NT(16), (NT(18)+NT(11)+T(2)+T(3)+NT(11)+NT(19)));
//G12:  __E_ref_3(23)        => offsets(24).
	p(NT(23), (NT(24)));
//G13:  __E_ref_3(23)        => null.
	p(NT(23), (nul));
//G14:  __E_ref_4(26)        => _(11) 'f' 'a' 'l' 'l' 'b' 'a' 'c' 'k' __(27) fp_fallback(28).
	p(NT(26), (NT(11)+T(4)+T(5)+T(6)+T(6)+T(7)+T(5)+T(8)+T(9)+NT(27)+NT(28)));
//G15:  __E_ref_4(26)        => null.
	p(NT(26), (nul));
//G16:  ref(18)              => sym(22) __E_ref_3(23) ref_args(25) __E_ref_4(26).
	p(NT(18), (NT(22)+NT(23)+NT(25)+NT(26)));
//G17:  first_sym(29)        => 'f' 'i' 'r' 's' 't'.
	p(NT(29), (T(4)+T(10)+T(11)+T(12)+T(13)));
//G18:  fp_fallback(28)      => first_sym(29).
	p(NT(28), (NT(29)));
//G19:  last_sym(30)         => 'l' 'a' 's' 't'.
	p(NT(30), (T(6)+T(5)+T(12)+T(13)));
//G20:  fp_fallback(28)      => last_sym(30).
	p(NT(28), (NT(30)));
//G21:  fp_fallback(28)      => capture(20).
	p(NT(28), (NT(20)));
//G22:  fp_fallback(28)      => ref(18).
	p(NT(28), (NT(18)));
//G23:  fp_fallback(28)      => wff(14).
	p(NT(28), (NT(14)));
//G24:  fp_fallback(28)      => bf(21).
	p(NT(28), (NT(21)));
//G25:  ref_arg(33)          => bf(21).
	p(NT(33), (NT(21)));
//G26:  __E___E_ref_args_5_6(32) => _(11) ref_arg(33).
	p(NT(32), (NT(11)+NT(33)));
//G27:  __E___E_ref_args_5_7(34) => _(11) ',' _(11) ref_arg(33).
	p(NT(34), (NT(11)+T(14)+NT(11)+NT(33)));
//G28:  __E___E_ref_args_5_8(35) => null.
	p(NT(35), (nul));
//G29:  __E___E_ref_args_5_8(35) => __E___E_ref_args_5_7(34) __E___E_ref_args_5_8(35).
	p(NT(35), (NT(34)+NT(35)));
//G30:  __E_ref_args_5(31)   => __E___E_ref_args_5_6(32) __E___E_ref_args_5_8(35).
	p(NT(31), (NT(32)+NT(35)));
//G31:  __E_ref_args_5(31)   => null.
	p(NT(31), (nul));
//G32:  ref_args(25)         => '(' __E_ref_args_5(31) _(11) ')'.
	p(NT(25), (T(15)+NT(31)+NT(11)+T(16)));
//G33:  __E___E___E_library_9_10_11(41) => wff_rule(42).
	p(NT(41), (NT(42)));
//G34:  __E___E___E_library_9_10_11(41) => bf_rule(43).
	p(NT(41), (NT(43)));
//G35:  rule(40)             => __E___E___E_library_9_10_11(41).
	p(NT(40), (NT(41)));
//G36:  __E___E_library_9_10(39) => _(11) rule(40).
	p(NT(39), (NT(11)+NT(40)));
//G37:  __E___E_library_9_12(44) => null.
	p(NT(44), (nul));
//G38:  __E___E_library_9_12(44) => __E___E_library_9_10(39) __E___E_library_9_12(44).
	p(NT(44), (NT(39)+NT(44)));
//G39:  __E_library_9(38)    => __E___E_library_9_12(44).
	p(NT(38), (NT(44)));
//G40:  rules(37)            => __E_library_9(38).
	p(NT(37), (NT(38)));
//G41:  library(36)          => rules(37).
	p(NT(36), (NT(37)));
//G42:  wff_matcher(45)      => wff(14).
	p(NT(45), (NT(14)));
//G43:  wff_body(46)         => wff(14).
	p(NT(46), (NT(14)));
//G44:  wff_rule(42)         => wff_matcher(45) _(11) ':' ':' '=' _(11) wff_body(46) _(11) '.'.
	p(NT(42), (NT(45)+NT(11)+T(2)+T(2)+T(3)+NT(11)+NT(46)+NT(11)+T(1)));
//G45:  bf_matcher(47)       => bf(21).
	p(NT(47), (NT(21)));
//G46:  bf_body(48)          => bf(21).
	p(NT(48), (NT(21)));
//G47:  bf_rule(43)          => bf_matcher(47) _(11) ':' '=' _(11) bf_body(48) _(11) '.'.
	p(NT(43), (NT(47)+NT(11)+T(2)+T(3)+NT(11)+NT(48)+NT(11)+T(1)));
//G48:  builder(49)          => _(11) builder_head(50) _(11) builder_body(51) _(11) '.'.
	p(NT(49), (NT(11)+NT(50)+NT(11)+NT(51)+NT(11)+T(1)));
//G49:  __E_builder_head_13(52) => __(27) capture(20).
	p(NT(52), (NT(27)+NT(20)));
//G50:  __E_builder_head_14(53) => null.
	p(NT(53), (nul));
//G51:  __E_builder_head_14(53) => __E_builder_head_13(52) __E_builder_head_14(53).
	p(NT(53), (NT(52)+NT(53)));
//G52:  builder_head(50)     => '(' _(11) capture(20) __E_builder_head_14(53) _(11) ')'.
	p(NT(50), (T(15)+NT(11)+NT(20)+NT(53)+NT(11)+T(16)));
//G53:  __E_builder_body_15(55) => '=' ':' _(11) bf(21).
	p(NT(55), (T(3)+T(2)+NT(11)+NT(21)));
//G54:  bf_builder_body(54)  => __E_builder_body_15(55).
	p(NT(54), (NT(55)));
//G55:  builder_body(51)     => bf_builder_body(54).
	p(NT(51), (NT(54)));
//G56:  __E_builder_body_16(57) => '=' ':' ':' _(11) wff(14).
	p(NT(57), (T(3)+T(2)+T(2)+NT(11)+NT(14)));
//G57:  wff_builder_body(56) => __E_builder_body_16(57).
	p(NT(56), (NT(57)));
//G58:  builder_body(51)     => wff_builder_body(56).
	p(NT(51), (NT(56)));
//G59:  __E_tau_constant_source_17(59) => '.' _(11).
	p(NT(59), (T(1)+NT(11)));
//G60:  __E_tau_constant_source_17(59) => null.
	p(NT(59), (nul));
//G61:  tau_constant_source(58) => rec_relations(12) _(11) main(13) _(11) __E_tau_constant_source_17(59).
	p(NT(58), (NT(12)+NT(11)+NT(13)+NT(11)+NT(59)));
//G62:  __E_wff_18(61)       => '(' _(11) wff(14) _(11) ')'.
	p(NT(61), (T(15)+NT(11)+NT(14)+NT(11)+T(16)));
//G63:  wff_parenthesis(60)  => __E_wff_18(61).
	p(NT(60), (NT(61)));
//G64:  wff(14)              => wff_parenthesis(60).
	p(NT(14), (NT(60)));
//G65:  __E___E_wff_19_20(64) => 's' 'o' 'm' 'e' 't' 'i' 'm' 'e' 's'.
	p(NT(64), (T(12)+T(17)+T(18)+T(19)+T(13)+T(10)+T(18)+T(19)+T(12)));
//G66:  __E___E_wff_19_20(64) => '<' '>'.
	p(NT(64), (T(20)+T(21)));
//G67:  __E_wff_19(63)       => __E___E_wff_19_20(64) _(11) wff(14).
	p(NT(63), (NT(64)+NT(11)+NT(14)));
//G68:  wff_sometimes(62)    => __E_wff_19(63).
	p(NT(62), (NT(63)));
//G69:  wff(14)              => wff_sometimes(62).
	p(NT(14), (NT(62)));
//G70:  __E___E_wff_21_22(67) => 'a' 'l' 'w' 'a' 'y' 's'.
	p(NT(67), (T(5)+T(6)+T(22)+T(5)+T(23)+T(12)));
//G71:  __E___E_wff_21_22(67) => '[' ']'.
	p(NT(67), (T(24)+T(25)));
//G72:  __E_wff_21(66)       => __E___E_wff_21_22(67) _(11) wff(14).
	p(NT(66), (NT(67)+NT(11)+NT(14)));
//G73:  wff_always(65)       => __E_wff_21(66).
	p(NT(65), (NT(66)));
//G74:  wff(14)              => wff_always(65).
	p(NT(14), (NT(65)));
//G75:  __E_wff_23(69)       => wff(14) _(11) '?' _(11) wff(14) _(11) ':' _(11) wff(14).
	p(NT(69), (NT(14)+NT(11)+T(26)+NT(11)+NT(14)+NT(11)+T(2)+NT(11)+NT(14)));
//G76:  wff_conditional(68)  => __E_wff_23(69).
	p(NT(68), (NT(69)));
//G77:  wff(14)              => wff_conditional(68).
	p(NT(14), (NT(68)));
//G78:  __E_wff_24(71)       => 'a' 'l' 'l' __(27) q_vars(72) __(27) wff(14).
	p(NT(71), (T(5)+T(6)+T(6)+NT(27)+NT(72)+NT(27)+NT(14)));
//G79:  wff_all(70)          => __E_wff_24(71).
	p(NT(70), (NT(71)));
//G80:  wff(14)              => wff_all(70).
	p(NT(14), (NT(70)));
//G81:  __E_wff_25(74)       => 'e' 'x' __(27) q_vars(72) __(27) wff(14).
	p(NT(74), (T(19)+T(27)+NT(27)+NT(72)+NT(27)+NT(14)));
//G82:  wff_ex(73)           => __E_wff_25(74).
	p(NT(73), (NT(74)));
//G83:  wff(14)              => wff_ex(73).
	p(NT(14), (NT(73)));
//G84:  wff_ref(75)          => ref(18).
	p(NT(75), (NT(18)));
//G85:  wff(14)              => wff_ref(75).
	p(NT(14), (NT(75)));
//G86:  wff(14)              => constraint(76).
	p(NT(14), (NT(76)));
//G87:  __E_wff_26(78)       => wff(14) _(11) '-' '>' _(11) wff(14).
	p(NT(78), (NT(14)+NT(11)+T(28)+T(21)+NT(11)+NT(14)));
//G88:  wff_imply(77)        => __E_wff_26(78).
	p(NT(77), (NT(78)));
//G89:  wff(14)              => wff_imply(77).
	p(NT(14), (NT(77)));
//G90:  __E_wff_27(80)       => wff(14) _(11) '<' '-' _(11) wff(14).
	p(NT(80), (NT(14)+NT(11)+T(20)+T(28)+NT(11)+NT(14)));
//G91:  wff_rimply(79)       => __E_wff_27(80).
	p(NT(79), (NT(80)));
//G92:  wff(14)              => wff_rimply(79).
	p(NT(14), (NT(79)));
//G93:  __E_wff_28(82)       => wff(14) _(11) '<' '-' '>' _(11) wff(14).
	p(NT(82), (NT(14)+NT(11)+T(20)+T(28)+T(21)+NT(11)+NT(14)));
//G94:  wff_equiv(81)        => __E_wff_28(82).
	p(NT(81), (NT(82)));
//G95:  wff(14)              => wff_equiv(81).
	p(NT(14), (NT(81)));
//G96:  __E_wff_29(84)       => wff(14) _(11) '|' '|' _(11) wff(14).
	p(NT(84), (NT(14)+NT(11)+T(29)+T(29)+NT(11)+NT(14)));
//G97:  wff_or(83)           => __E_wff_29(84).
	p(NT(83), (NT(84)));
//G98:  wff(14)              => wff_or(83).
	p(NT(14), (NT(83)));
//G99:  __E_wff_30(86)       => wff(14) _(11) '^' _(11) wff(14).
	p(NT(86), (NT(14)+NT(11)+T(30)+NT(11)+NT(14)));
//G100: wff_xor(85)          => __E_wff_30(86).
	p(NT(85), (NT(86)));
//G101: wff(14)              => wff_xor(85).
	p(NT(14), (NT(85)));
//G102: __E_wff_31(88)       => wff(14) _(11) '&' '&' _(11) wff(14).
	p(NT(88), (NT(14)+NT(11)+T(31)+T(31)+NT(11)+NT(14)));
//G103: wff_and(87)          => __E_wff_31(88).
	p(NT(87), (NT(88)));
//G104: wff(14)              => wff_and(87).
	p(NT(14), (NT(87)));
//G105: __E_wff_32(90)       => '!' _(11) wff(14).
	p(NT(90), (T(32)+NT(11)+NT(14)));
//G106: wff_neg(89)          => __E_wff_32(90).
	p(NT(89), (NT(90)));
//G107: wff(14)              => wff_neg(89).
	p(NT(14), (NT(89)));
//G108: wff_t(91)            => 'T'.
	p(NT(91), (T(33)));
//G109: wff(14)              => wff_t(91).
	p(NT(14), (NT(91)));
//G110: wff_f(92)            => 'F'.
	p(NT(92), (T(34)));
//G111: wff(14)              => wff_f(92).
	p(NT(14), (NT(92)));
//G112: __E_wff_33(94)       => bf(21) _(11) '<' '=' _(11) bf(21) _(11) '<' '=' _(11) bf(21).
	p(NT(94), (NT(21)+NT(11)+T(20)+T(3)+NT(11)+NT(21)+NT(11)+T(20)+T(3)+NT(11)+NT(21)));
//G113: bf_interval(93)      => __E_wff_33(94).
	p(NT(93), (NT(94)));
//G114: wff(14)              => bf_interval(93).
	p(NT(14), (NT(93)));
//G115: __E_wff_34(96)       => bf(21) _(11) '=' _(11) bf(21).
	p(NT(96), (NT(21)+NT(11)+T(3)+NT(11)+NT(21)));
//G116: bf_eq(95)            => __E_wff_34(96).
	p(NT(95), (NT(96)));
//G117: wff(14)              => bf_eq(95).
	p(NT(14), (NT(95)));
//G118: __E_wff_35(98)       => bf(21) _(11) '!' '=' _(11) bf(21).
	p(NT(98), (NT(21)+NT(11)+T(32)+T(3)+NT(11)+NT(21)));
//G119: bf_neq(97)           => __E_wff_35(98).
	p(NT(97), (NT(98)));
//G120: wff(14)              => bf_neq(97).
	p(NT(14), (NT(97)));
//G121: __E_wff_36(100)      => bf(21) _(11) '<' '=' _(11) bf(21).
	p(NT(100), (NT(21)+NT(11)+T(20)+T(3)+NT(11)+NT(21)));
//G122: bf_lteq(99)          => __E_wff_36(100).
	p(NT(99), (NT(100)));
//G123: wff(14)              => bf_lteq(99).
	p(NT(14), (NT(99)));
//G124: __E_wff_37(102)      => bf(21) _(11) '!' '<' '=' _(11) bf(21).
	p(NT(102), (NT(21)+NT(11)+T(32)+T(20)+T(3)+NT(11)+NT(21)));
//G125: bf_nlteq(101)        => __E_wff_37(102).
	p(NT(101), (NT(102)));
//G126: wff(14)              => bf_nlteq(101).
	p(NT(14), (NT(101)));
//G127: __E_wff_38(104)      => bf(21) _(11) '>' _(11) bf(21).
	p(NT(104), (NT(21)+NT(11)+T(21)+NT(11)+NT(21)));
//G128: bf_gt(103)           => __E_wff_38(104).
	p(NT(103), (NT(104)));
//G129: wff(14)              => bf_gt(103).
	p(NT(14), (NT(103)));
//G130: __E_wff_39(106)      => bf(21) _(11) '!' '>' _(11) bf(21).
	p(NT(106), (NT(21)+NT(11)+T(32)+T(21)+NT(11)+NT(21)));
//G131: bf_ngt(105)          => __E_wff_39(106).
	p(NT(105), (NT(106)));
//G132: wff(14)              => bf_ngt(105).
	p(NT(14), (NT(105)));
//G133: __E_wff_40(108)      => bf(21) _(11) '>' '=' _(11) bf(21).
	p(NT(108), (NT(21)+NT(11)+T(21)+T(3)+NT(11)+NT(21)));
//G134: bf_gteq(107)         => __E_wff_40(108).
	p(NT(107), (NT(108)));
//G135: wff(14)              => bf_gteq(107).
	p(NT(14), (NT(107)));
//G136: __E_wff_41(110)      => bf(21) _(11) '!' '>' '=' _(11) bf(21).
	p(NT(110), (NT(21)+NT(11)+T(32)+T(21)+T(3)+NT(11)+NT(21)));
//G137: bf_ngteq(109)        => __E_wff_41(110).
	p(NT(109), (NT(110)));
//G138: wff(14)              => bf_ngteq(109).
	p(NT(14), (NT(109)));
//G139: __E_wff_42(112)      => bf(21) _(11) '<' _(11) bf(21).
	p(NT(112), (NT(21)+NT(11)+T(20)+NT(11)+NT(21)));
//G140: bf_lt(111)           => __E_wff_42(112).
	p(NT(111), (NT(112)));
//G141: wff(14)              => bf_lt(111).
	p(NT(14), (NT(111)));
//G142: __E_wff_43(114)      => bf(21) _(11) '!' '<' _(11) bf(21).
	p(NT(114), (NT(21)+NT(11)+T(32)+T(20)+NT(11)+NT(21)));
//G143: bf_nlt(113)          => __E_wff_43(114).
	p(NT(113), (NT(114)));
//G144: wff(14)              => bf_nlt(113).
	p(NT(14), (NT(113)));
//G145: wff(14)              => capture(20).
	p(NT(14), (NT(20)));
//G146: __E_bf_44(116)       => '(' _(11) bf(21) _(11) ')'.
	p(NT(116), (T(15)+NT(11)+NT(21)+NT(11)+T(16)));
//G147: bf_parenthesis(115)  => __E_bf_44(116).
	p(NT(115), (NT(116)));
//G148: bf(21)               => bf_parenthesis(115).
	p(NT(21), (NT(115)));
//G149: bf(21)               => bf_constant(117).
	p(NT(21), (NT(117)));
//G150: bf(21)               => variable(118).
	p(NT(21), (NT(118)));
//G151: __E_bf_45(120)       => 'S' _(11) '(' _(11) bf(21) _(11) ')'.
	p(NT(120), (T(35)+NT(11)+T(15)+NT(11)+NT(21)+NT(11)+T(16)));
//G152: bf_splitter(119)     => __E_bf_45(120).
	p(NT(119), (NT(120)));
//G153: bf(21)               => bf_splitter(119).
	p(NT(21), (NT(119)));
//G154: bf_ref(121)          => ref(18).
	p(NT(121), (NT(18)));
//G155: bf(21)               => bf_ref(121).
	p(NT(21), (NT(121)));
//G156: __E_bf_46(123)       => bf(21) _(11) '|' _(11) bf(21).
	p(NT(123), (NT(21)+NT(11)+T(29)+NT(11)+NT(21)));
//G157: bf_or(122)           => __E_bf_46(123).
	p(NT(122), (NT(123)));
//G158: bf(21)               => bf_or(122).
	p(NT(21), (NT(122)));
//G159: __E_bf_47(125)       => bf(21) _(11) '+' _(11) bf(21).
	p(NT(125), (NT(21)+NT(11)+T(36)+NT(11)+NT(21)));
//G160: bf_xor(124)          => __E_bf_47(125).
	p(NT(124), (NT(125)));
//G161: bf(21)               => bf_xor(124).
	p(NT(21), (NT(124)));
//G162: __E___E_bf_48_49(128) => typed(129).
	p(NT(128), (NT(129)));
//G163: __E___E_bf_48_49(128) => null.
	p(NT(128), (nul));
//G164: __E_bf_48(127)       => '1' __E___E_bf_48_49(128).
	p(NT(127), (T(37)+NT(128)));
//G165: bf_t(126)            => __E_bf_48(127).
	p(NT(126), (NT(127)));
//G166: bf(21)               => bf_t(126).
	p(NT(21), (NT(126)));
//G167: __E___E_bf_50_51(132) => typed(129).
	p(NT(132), (NT(129)));
//G168: __E___E_bf_50_51(132) => null.
	p(NT(132), (nul));
//G169: __E_bf_50(131)       => '0' __E___E_bf_50_51(132).
	p(NT(131), (T(38)+NT(132)));
//G170: bf_f(130)            => __E_bf_50(131).
	p(NT(130), (NT(131)));
//G171: bf(21)               => bf_f(130).
	p(NT(21), (NT(130)));
//G172: __E___E_bf_52_53(135) => _(11) '&' _(11).
	p(NT(135), (NT(11)+T(31)+NT(11)));
//G173: __E___E_bf_52_53(135) => __(27).
	p(NT(135), (NT(27)));
//G174: __E_bf_52(134)       => bf(21) __E___E_bf_52_53(135) bf(21).
	p(NT(134), (NT(21)+NT(135)+NT(21)));
//G175: bf_and(133)          => __E_bf_52(134).
	p(NT(133), (NT(134)));
//G176: bf(21)               => bf_and(133).
	p(NT(21), (NT(133)));
//G177: __E___E_bf_54_55(139) => bf_parenthesis(115).
	p(NT(139), (NT(115)));
//G178: __E___E_bf_54_55(139) => bf_constant(117).
	p(NT(139), (NT(117)));
//G179: __E___E_bf_54_55(139) => variable(118).
	p(NT(139), (NT(118)));
//G180: __E___E_bf_54_55(139) => bf_splitter(119).
	p(NT(139), (NT(119)));
//G181: __E___E_bf_54_55(139) => bf_ref(121).
	p(NT(139), (NT(121)));
//G182: __E___E_bf_54_55(139) => bf_t(126).
	p(NT(139), (NT(126)));
//G183: __E___E_bf_54_55(139) => bf_f(130).
	p(NT(139), (NT(130)));
//G184: __E___E_bf_54_55(139) => bf_neg(136).
	p(NT(139), (NT(136)));
//G185: __E___E_bf_54_55(139) => capture(20).
	p(NT(139), (NT(20)));
//G186: bf_neg_oprnd(138)    => __E___E_bf_54_55(139).
	p(NT(138), (NT(139)));
//G187: __E_bf_54(137)       => bf_neg_oprnd(138) _(11) '\''.
	p(NT(137), (NT(138)+NT(11)+T(39)));
//G188: bf_neg(136)          => __E_bf_54(137).
	p(NT(136), (NT(137)));
//G189: bf(21)               => bf_neg(136).
	p(NT(21), (NT(136)));
//G190: __E___E_bf_56_57(143) => bf_parenthesis(115).
	p(NT(143), (NT(115)));
//G191: __E___E_bf_56_57(143) => variable(118).
	p(NT(143), (NT(118)));
//G192: __E___E_bf_56_57(143) => bf_splitter(119).
	p(NT(143), (NT(119)));
//G193: __E___E_bf_56_57(143) => bf_ref(121).
	p(NT(143), (NT(121)));
//G194: __E___E_bf_56_57(143) => bf_neg(136).
	p(NT(143), (NT(136)));
//G195: __E___E_bf_56_57(143) => bf_and_nosep(140).
	p(NT(143), (NT(140)));
//G196: __E___E_bf_56_57(143) => capture(20).
	p(NT(143), (NT(20)));
//G197: bf_and_nosep_1st_oprnd(142) => __E___E_bf_56_57(143).
	p(NT(142), (NT(143)));
//G198: __E___E_bf_56_58(145) => bf_parenthesis(115).
	p(NT(145), (NT(115)));
//G199: __E___E_bf_56_58(145) => bf_constant(117).
	p(NT(145), (NT(117)));
//G200: __E___E_bf_56_58(145) => variable(118).
	p(NT(145), (NT(118)));
//G201: __E___E_bf_56_58(145) => bf_splitter(119).
	p(NT(145), (NT(119)));
//G202: __E___E_bf_56_58(145) => bf_ref(121).
	p(NT(145), (NT(121)));
//G203: __E___E_bf_56_58(145) => bf_neg(136).
	p(NT(145), (NT(136)));
//G204: __E___E_bf_56_58(145) => capture(20).
	p(NT(145), (NT(20)));
//G205: bf_and_nosep_2nd_oprnd(144) => __E___E_bf_56_58(145).
	p(NT(144), (NT(145)));
//G206: __E_bf_56(141)       => bf_and_nosep_1st_oprnd(142) bf_and_nosep_2nd_oprnd(144).
	p(NT(141), (NT(142)+NT(144)));
//G207: bf_and_nosep(140)    => __E_bf_56(141).
	p(NT(140), (NT(141)));
//G208: bf(21)               => bf_and_nosep(140).
	p(NT(21), (NT(140)));
//G209: bf(21)               => capture(20).
	p(NT(21), (NT(20)));
//G210: __E_constraint_59(147) => '[' ctnvar(148) _(11) '!' '=' _(11) num(149) ']'.
	p(NT(147), (T(24)+NT(148)+NT(11)+T(32)+T(3)+NT(11)+NT(149)+T(25)));
//G211: __E_constraint_59(147) => '[' num(149) _(11) '!' '=' _(11) ctnvar(148) ']'.
	p(NT(147), (T(24)+NT(149)+NT(11)+T(32)+T(3)+NT(11)+NT(148)+T(25)));
//G212: ctn_neq(146)         => __E_constraint_59(147).
	p(NT(146), (NT(147)));
//G213: constraint(76)       => ctn_neq(146).
	p(NT(76), (NT(146)));
//G214: __E_constraint_60(151) => '[' ctnvar(148) _(11) '=' _(11) num(149) ']'.
	p(NT(151), (T(24)+NT(148)+NT(11)+T(3)+NT(11)+NT(149)+T(25)));
//G215: __E_constraint_60(151) => '[' num(149) _(11) '=' _(11) ctnvar(148) ']'.
	p(NT(151), (T(24)+NT(149)+NT(11)+T(3)+NT(11)+NT(148)+T(25)));
//G216: ctn_eq(150)          => __E_constraint_60(151).
	p(NT(150), (NT(151)));
//G217: constraint(76)       => ctn_eq(150).
	p(NT(76), (NT(150)));
//G218: __E_constraint_61(153) => '[' ctnvar(148) _(11) '>' '=' _(11) num(149) ']'.
	p(NT(153), (T(24)+NT(148)+NT(11)+T(21)+T(3)+NT(11)+NT(149)+T(25)));
//G219: __E_constraint_61(153) => '[' num(149) _(11) '>' '=' _(11) ctnvar(148) ']'.
	p(NT(153), (T(24)+NT(149)+NT(11)+T(21)+T(3)+NT(11)+NT(148)+T(25)));
//G220: ctn_gteq(152)        => __E_constraint_61(153).
	p(NT(152), (NT(153)));
//G221: constraint(76)       => ctn_gteq(152).
	p(NT(76), (NT(152)));
//G222: __E_constraint_62(155) => '[' ctnvar(148) _(11) '>' _(11) num(149) ']'.
	p(NT(155), (T(24)+NT(148)+NT(11)+T(21)+NT(11)+NT(149)+T(25)));
//G223: __E_constraint_62(155) => '[' num(149) _(11) '>' _(11) ctnvar(148) ']'.
	p(NT(155), (T(24)+NT(149)+NT(11)+T(21)+NT(11)+NT(148)+T(25)));
//G224: ctn_gt(154)          => __E_constraint_62(155).
	p(NT(154), (NT(155)));
//G225: constraint(76)       => ctn_gt(154).
	p(NT(76), (NT(154)));
//G226: __E_constraint_63(157) => '[' ctnvar(148) _(11) '<' '=' _(11) num(149) ']'.
	p(NT(157), (T(24)+NT(148)+NT(11)+T(20)+T(3)+NT(11)+NT(149)+T(25)));
//G227: __E_constraint_63(157) => '[' num(149) _(11) '<' '=' _(11) ctnvar(148) ']'.
	p(NT(157), (T(24)+NT(149)+NT(11)+T(20)+T(3)+NT(11)+NT(148)+T(25)));
//G228: ctn_lteq(156)        => __E_constraint_63(157).
	p(NT(156), (NT(157)));
//G229: constraint(76)       => ctn_lteq(156).
	p(NT(76), (NT(156)));
//G230: __E_constraint_64(159) => '[' ctnvar(148) _(11) '<' _(11) num(149) ']'.
	p(NT(159), (T(24)+NT(148)+NT(11)+T(20)+NT(11)+NT(149)+T(25)));
//G231: __E_constraint_64(159) => '[' num(149) _(11) '<' _(11) ctnvar(148) ']'.
	p(NT(159), (T(24)+NT(149)+NT(11)+T(20)+NT(11)+NT(148)+T(25)));
//G232: ctn_lt(158)          => __E_constraint_64(159).
	p(NT(158), (NT(159)));
//G233: constraint(76)       => ctn_lt(158).
	p(NT(76), (NT(158)));
//G234: __E_bf_constant_65(160) => capture(20).
	p(NT(160), (NT(20)));
//G235: __E_bf_constant_65(160) => source(161).
	p(NT(160), (NT(161)));
//G236: __E_bf_constant_66(162) => typed(129).
	p(NT(162), (NT(129)));
//G237: __E_bf_constant_66(162) => null.
	p(NT(162), (nul));
//G238: bf_constant(117)     => '{' _(11) __E_bf_constant_65(160) _(11) '}' __E_bf_constant_66(162).
	p(NT(117), (T(40)+NT(11)+NT(160)+NT(11)+T(41)+NT(162)));
//G239: __E___E_source_67_68(165) => src_c(163).
	p(NT(165), (NT(163)));
//G240: __E___E_source_67_68(165) => space(2).
	p(NT(165), (NT(2)));
//G241: __E___E_source_67_69(166) => null.
	p(NT(166), (nul));
//G242: __E___E_source_67_69(166) => __E___E_source_67_68(165) __E___E_source_67_69(166).
	p(NT(166), (NT(165)+NT(166)));
//G243: __E_source_67(164)   => __E___E_source_67_69(166) src_c(163).
	p(NT(164), (NT(166)+NT(163)));
//G244: __E_source_67(164)   => null.
	p(NT(164), (nul));
//G245: source(161)          => src_c(163) __E_source_67(164).
	p(NT(161), (NT(163)+NT(164)));
//G246: src_c(163)           => alnum(6).
	p(NT(163), (NT(6)));
//G247: __N_0(368)           => '{'.
	p(NT(368), (T(40)));
//G248: __N_1(369)           => '}'.
	p(NT(369), (T(41)));
//G249: src_c(163)           => ~( __N_0(368) ) & ~( __N_1(369) ) & punct(7).	 # conjunctive
	p(NT(163), ~(NT(368)) & ~(NT(369)) & (NT(7)));
//G250: __E_src_c_70(167)    => src_c(163).
	p(NT(167), (NT(163)));
//G251: __E_src_c_70(167)    => space(2).
	p(NT(167), (NT(2)));
//G252: __E_src_c_71(168)    => null.
	p(NT(168), (nul));
//G253: __E_src_c_71(168)    => __E_src_c_70(167) __E_src_c_71(168).
	p(NT(168), (NT(167)+NT(168)));
//G254: src_c(163)           => '{' __E_src_c_71(168) '}'.
	p(NT(163), (T(40)+NT(168)+T(41)));
//G255: __E_variable_72(169) => uconst(170).
	p(NT(169), (NT(170)));
//G256: __E_variable_72(169) => io_var(171).
	p(NT(169), (NT(171)));
//G257: __E_variable_72(169) => var_name(172).
	p(NT(169), (NT(172)));
//G258: __E_variable_73(173) => typed(129).
	p(NT(173), (NT(129)));
//G259: __E_variable_73(173) => null.
	p(NT(173), (nul));
//G260: variable(118)        => __E_variable_72(169) __E_variable_73(173).
	p(NT(118), (NT(169)+NT(173)));
//G261: __N_2(370)           => 'F'.
	p(NT(370), (T(34)));
//G262: __N_3(371)           => 'T'.
	p(NT(371), (T(33)));
//G263: __E_var_name_74(174) => ~( __N_2(370) ) & ~( __N_3(371) ) & alpha(5).	 # conjunctive
	p(NT(174), ~(NT(370)) & ~(NT(371)) & (NT(5)));
//G264: __E_var_name_75(175) => null.
	p(NT(175), (nul));
//G265: __E_var_name_75(175) => digit(3) __E_var_name_75(175).
	p(NT(175), (NT(3)+NT(175)));
//G266: var_name(172)        => __E_var_name_74(174) __E_var_name_75(175).	 # guarded: charvar
	p(NT(172), (NT(174)+NT(175)));
	p.back().guard = "charvar";
//G267: __N_4(372)           => 'F'.
	p(NT(372), (T(34)));
//G268: __N_5(373)           => 'T'.
	p(NT(373), (T(33)));
//G269: __E_var_name_76(176) => ~( __N_4(372) ) & ~( __N_5(373) ) & alpha(5).	 # conjunctive
	p(NT(176), ~(NT(372)) & ~(NT(373)) & (NT(5)));
//G270: __E_var_name_77(177) => alnum(6).
	p(NT(177), (NT(6)));
//G271: __E_var_name_77(177) => '_'.
	p(NT(177), (T(42)));
//G272: __E_var_name_78(178) => null.
	p(NT(178), (nul));
//G273: __E_var_name_78(178) => __E_var_name_77(177) __E_var_name_78(178).
	p(NT(178), (NT(177)+NT(178)));
//G274: var_name(172)        => __E_var_name_76(176) __E_var_name_78(178).	 # guarded: var
	p(NT(172), (NT(176)+NT(178)));
	p.back().guard = "var";
//G275: io_var(171)          => var_name(172) '[' offset(179) ']'.
	p(NT(171), (NT(172)+T(24)+NT(179)+T(25)));
//G276: __E___E_uconst_79_80(182) => chars(183) _(11).
	p(NT(182), (NT(183)+NT(11)));
//G277: __E___E_uconst_79_80(182) => null.
	p(NT(182), (nul));
//G278: __E_uconst_79(181)   => __E___E_uconst_79_80(182) ':' _(11) chars(183).
	p(NT(181), (NT(182)+T(2)+NT(11)+NT(183)));
//G279: uconst_name(180)     => __E_uconst_79(181).
	p(NT(180), (NT(181)));
//G280: uconst(170)          => '<' _(11) uconst_name(180) _(11) '>'.
	p(NT(170), (T(20)+NT(11)+NT(180)+NT(11)+T(21)));
//G281: __E_q_vars_81(185)   => _(11) ',' _(11) q_var(184).
	p(NT(185), (NT(11)+T(14)+NT(11)+NT(184)));
//G282: __E_q_vars_82(186)   => null.
	p(NT(186), (nul));
//G283: __E_q_vars_82(186)   => __E_q_vars_81(185) __E_q_vars_82(186).
	p(NT(186), (NT(185)+NT(186)));
//G284: q_vars(72)           => q_var(184) __E_q_vars_82(186).
	p(NT(72), (NT(184)+NT(186)));
//G285: q_var(184)           => capture(20).
	p(NT(184), (NT(20)));
//G286: __N_6(374)           => uconst(170).
	p(NT(374), (NT(170)));
//G287: q_var(184)           => variable(118) & ~( __N_6(374) ).	 # conjunctive
	p(NT(184), (NT(118)) & ~(NT(374)));
//G288: ctnvar(148)          => chars(183).
	p(NT(148), (NT(183)));
//G289: __E_offsets_83(187)  => _(11) ',' _(11) offset(179).
	p(NT(187), (NT(11)+T(14)+NT(11)+NT(179)));
//G290: __E_offsets_84(188)  => null.
	p(NT(188), (nul));
//G291: __E_offsets_84(188)  => __E_offsets_83(187) __E_offsets_84(188).
	p(NT(188), (NT(187)+NT(188)));
//G292: offsets(24)          => '[' _(11) offset(179) __E_offsets_84(188) _(11) ']'.
	p(NT(24), (T(24)+NT(11)+NT(179)+NT(188)+NT(11)+T(25)));
//G293: offset(179)          => integer(189).
	p(NT(179), (NT(189)));
//G294: offset(179)          => capture(20).
	p(NT(179), (NT(20)));
//G295: offset(179)          => shift(190).
	p(NT(179), (NT(190)));
//G296: __N_7(375)           => io_var(171).
	p(NT(375), (NT(171)));
//G297: __E_offset_85(191)   => variable(118) & ~( __N_7(375) ).	 # conjunctive
	p(NT(191), (NT(118)) & ~(NT(375)));
//G298: offset(179)          => __E_offset_85(191).
	p(NT(179), (NT(191)));
//G299: __E_shift_86(192)    => capture(20).
	p(NT(192), (NT(20)));
//G300: __N_8(376)           => io_var(171).
	p(NT(376), (NT(171)));
//G301: __E___E_shift_86_87(193) => variable(118) & ~( __N_8(376) ).	 # conjunctive
	p(NT(193), (NT(118)) & ~(NT(376)));
//G302: __E_shift_86(192)    => __E___E_shift_86_87(193).
	p(NT(192), (NT(193)));
//G303: shift(190)           => __E_shift_86(192) _(11) '-' _(11) num(149).
	p(NT(190), (NT(192)+NT(11)+T(28)+NT(11)+NT(149)));
//G304: __E_chars_88(194)    => alnum(6).
	p(NT(194), (NT(6)));
//G305: __E_chars_88(194)    => '_'.
	p(NT(194), (T(42)));
//G306: __E_chars_89(195)    => null.
	p(NT(195), (nul));
//G307: __E_chars_89(195)    => __E_chars_88(194) __E_chars_89(195).
	p(NT(195), (NT(194)+NT(195)));
//G308: chars(183)           => alpha(5) __E_chars_89(195).
	p(NT(183), (NT(5)+NT(195)));
//G309: __E_digits_90(197)   => digit(3).
	p(NT(197), (NT(3)));
//G310: __E_digits_90(197)   => digit(3) __E_digits_90(197).
	p(NT(197), (NT(3)+NT(197)));
//G311: digits(196)          => __E_digits_90(197).
	p(NT(196), (NT(197)));
//G312: num(149)             => digits(196).
	p(NT(149), (NT(196)));
//G313: __E_integer_91(198)  => '-'.
	p(NT(198), (T(28)));
//G314: __E_integer_91(198)  => null.
	p(NT(198), (nul));
//G315: integer(189)         => __E_integer_91(198) _(11) digits(196).
	p(NT(189), (NT(198)+NT(11)+NT(196)));
//G316: sym(22)              => chars(183).
	p(NT(22), (NT(183)));
//G317: capture(20)          => '$' chars(183).
	p(NT(20), (T(43)+NT(183)));
//G318: type(199)            => chars(183).
	p(NT(199), (NT(183)));
//G319: typed(129)           => _(11) ':' _(11) type(199).
	p(NT(129), (NT(11)+T(2)+NT(11)+NT(199)));
//G320: scope_id(200)        => digits(196).
	p(NT(200), (NT(196)));
//G321: __E_comment_92(202)  => printable(8).
	p(NT(202), (NT(8)));
//G322: __E_comment_92(202)  => '\t'.
	p(NT(202), (T(44)));
//G323: __E_comment_93(203)  => null.
	p(NT(203), (nul));
//G324: __E_comment_93(203)  => __E_comment_92(202) __E_comment_93(203).
	p(NT(203), (NT(202)+NT(203)));
//G325: __E_comment_94(204)  => '\n'.
	p(NT(204), (T(45)));
//G326: __E_comment_94(204)  => '\r'.
	p(NT(204), (T(46)));
//G327: __E_comment_94(204)  => eof(1).
	p(NT(204), (NT(1)));
//G328: comment(201)         => '#' __E_comment_93(203) __E_comment_94(204).
	p(NT(201), (T(47)+NT(203)+NT(204)));
//G329: __E____95(205)       => space(2).
	p(NT(205), (NT(2)));
//G330: __E____95(205)       => comment(201).
	p(NT(205), (NT(201)));
//G331: __(27)               => __E____95(205) _(11).
	p(NT(27), (NT(205)+NT(11)));
//G332: __E___96(206)        => __(27).
	p(NT(206), (NT(27)));
//G333: __E___96(206)        => null.
	p(NT(206), (nul));
//G334: _(11)                => __E___96(206).
	p(NT(11), (NT(206)));
//G335: cli(207)             => _(11).
	p(NT(207), (NT(11)));
//G336: __E_cli_97(209)      => '.' _(11) cli_command(208) _(11).
	p(NT(209), (T(1)+NT(11)+NT(208)+NT(11)));
//G337: __E_cli_98(210)      => null.
	p(NT(210), (nul));
//G338: __E_cli_98(210)      => __E_cli_97(209) __E_cli_98(210).
	p(NT(210), (NT(209)+NT(210)));
//G339: cli(207)             => _(11) cli_command(208) _(11) __E_cli_98(210).
	p(NT(207), (NT(11)+NT(208)+NT(11)+NT(210)));
//G340: __E_cli_command_99(213) => 'q'.
	p(NT(213), (T(48)));
//G341: __E_cli_command_99(213) => 'q' 'u' 'i' 't'.
	p(NT(213), (T(48)+T(49)+T(10)+T(13)));
//G342: quit_sym(212)        => __E_cli_command_99(213).
	p(NT(212), (NT(213)));
//G343: quit_cmd(211)        => quit_sym(212).
	p(NT(211), (NT(212)));
//G344: cli_command(208)     => quit_cmd(211).
	p(NT(208), (NT(211)));
//G345: __E_cli_command_100(216) => 'v'.
	p(NT(216), (T(50)));
//G346: __E_cli_command_100(216) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(216), (T(50)+T(19)+T(11)+T(12)+T(10)+T(17)+T(51)));
//G347: version_sym(215)     => __E_cli_command_100(216).
	p(NT(215), (NT(216)));
//G348: version_cmd(214)     => version_sym(215).
	p(NT(214), (NT(215)));
//G349: cli_command(208)     => version_cmd(214).
	p(NT(208), (NT(214)));
//G350: __E_cli_command_101(219) => 'c'.
	p(NT(219), (T(8)));
//G351: __E_cli_command_101(219) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(219), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G352: clear_sym(218)       => __E_cli_command_101(219).
	p(NT(218), (NT(219)));
//G353: clear_cmd(217)       => clear_sym(218).
	p(NT(217), (NT(218)));
//G354: cli_command(208)     => clear_cmd(217).
	p(NT(208), (NT(217)));
//G355: __E___E_cli_command_102_103(223) => 'h'.
	p(NT(223), (T(52)));
//G356: __E___E_cli_command_102_103(223) => 'h' 'e' 'l' 'p'.
	p(NT(223), (T(52)+T(19)+T(6)+T(53)));
//G357: help_sym(222)        => __E___E_cli_command_102_103(223).
	p(NT(222), (NT(223)));
//G358: __E___E_cli_command_102_104(224) => __(27) help_arg(225).
	p(NT(224), (NT(27)+NT(225)));
//G359: __E___E_cli_command_102_104(224) => null.
	p(NT(224), (nul));
//G360: __E_cli_command_102(221) => help_sym(222) __E___E_cli_command_102_104(224).
	p(NT(221), (NT(222)+NT(224)));
//G361: help_cmd(220)        => __E_cli_command_102(221).
	p(NT(220), (NT(221)));
//G362: cli_command(208)     => help_cmd(220).
	p(NT(208), (NT(220)));
//G363: file_sym(228)        => 'f' 'i' 'l' 'e'.
	p(NT(228), (T(4)+T(10)+T(6)+T(19)));
//G364: __E_cli_command_105(227) => file_sym(228) __(27) q_string(229).
	p(NT(227), (NT(228)+NT(27)+NT(229)));
//G365: file_cmd(226)        => __E_cli_command_105(227).
	p(NT(226), (NT(227)));
//G366: cli_command(208)     => file_cmd(226).
	p(NT(208), (NT(226)));
//G367: valid_sym(232)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(232), (T(50)+T(5)+T(6)+T(10)+T(54)));
//G368: __E_cli_command_106(231) => valid_sym(232) __(27) normalize_cmd_arg(233).
	p(NT(231), (NT(232)+NT(27)+NT(233)));
//G369: valid_cmd(230)       => __E_cli_command_106(231).
	p(NT(230), (NT(231)));
//G370: cli_command(208)     => valid_cmd(230).
	p(NT(208), (NT(230)));
//G371: sat_sym(236)         => 's' 'a' 't'.
	p(NT(236), (T(12)+T(5)+T(13)));
//G372: __E_cli_command_107(235) => sat_sym(236) __(27) normalize_cmd_arg(233).
	p(NT(235), (NT(236)+NT(27)+NT(233)));
//G373: sat_cmd(234)         => __E_cli_command_107(235).
	p(NT(234), (NT(235)));
//G374: cli_command(208)     => sat_cmd(234).
	p(NT(208), (NT(234)));
//G375: unsat_sym(239)       => 'u' 'n' 's' 'a' 't'.
	p(NT(239), (T(49)+T(51)+T(12)+T(5)+T(13)));
//G376: __E_cli_command_108(238) => unsat_sym(239) __(27) normalize_cmd_arg(233).
	p(NT(238), (NT(239)+NT(27)+NT(233)));
//G377: unsat_cmd(237)       => __E_cli_command_108(238).
	p(NT(237), (NT(238)));
//G378: cli_command(208)     => unsat_cmd(237).
	p(NT(208), (NT(237)));
//G379: solve_sym(242)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(242), (T(12)+T(17)+T(6)+T(50)+T(19)));
//G380: __E___E_cli_command_109_110(243) => solve_options(244).
	p(NT(243), (NT(244)));
//G381: __E___E_cli_command_109_110(243) => null.
	p(NT(243), (nul));
//G382: __E_cli_command_109(241) => solve_sym(242) __E___E_cli_command_109_110(243) __(27) wff_cmd_arg(245).
	p(NT(241), (NT(242)+NT(243)+NT(27)+NT(245)));
//G383: solve_cmd(240)       => __E_cli_command_109(241).
	p(NT(240), (NT(241)));
//G384: cli_command(208)     => solve_cmd(240).
	p(NT(208), (NT(240)));
//G385: lgrs_sym(248)        => 'l' 'g' 'r' 's'.
	p(NT(248), (T(6)+T(55)+T(11)+T(12)));
//G386: __E_cli_command_111(247) => lgrs_sym(248) __(27) wff_cmd_arg(245).
	p(NT(247), (NT(248)+NT(27)+NT(245)));
//G387: lgrs_cmd(246)        => __E_cli_command_111(247).
	p(NT(246), (NT(247)));
//G388: cli_command(208)     => lgrs_cmd(246).
	p(NT(208), (NT(246)));
//G389: __E___E_cli_command_112_113(252) => 'r'.
	p(NT(252), (T(11)));
//G390: __E___E_cli_command_112_113(252) => 'r' 'u' 'n'.
	p(NT(252), (T(11)+T(49)+T(51)));
//G391: run_sym(251)         => __E___E_cli_command_112_113(252).
	p(NT(251), (NT(252)));
//G392: __E___E_cli_command_112_114(253) => history(254).
	p(NT(253), (NT(254)));
//G393: __E___E_cli_command_112_114(253) => wff(14).
	p(NT(253), (NT(14)));
//G394: __E_cli_command_112(250) => run_sym(251) __(27) __E___E_cli_command_112_114(253).
	p(NT(250), (NT(251)+NT(27)+NT(253)));
//G395: run_cmd(249)         => __E_cli_command_112(250).
	p(NT(249), (NT(250)));
//G396: cli_command(208)     => run_cmd(249).
	p(NT(208), (NT(249)));
//G397: __E___E_cli_command_115_116(258) => 'n'.
	p(NT(258), (T(51)));
//G398: __E___E_cli_command_115_116(258) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(258), (T(51)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(56)+T(19)));
//G399: normalize_sym(257)   => __E___E_cli_command_115_116(258).
	p(NT(257), (NT(258)));
//G400: __E_cli_command_115(256) => normalize_sym(257) __(27) normalize_cmd_arg(233).
	p(NT(256), (NT(257)+NT(27)+NT(233)));
//G401: normalize_cmd(255)   => __E_cli_command_115(256).
	p(NT(255), (NT(256)));
//G402: cli_command(208)     => normalize_cmd(255).
	p(NT(208), (NT(255)));
//G403: __E___E_cli_command_117_118(262) => 's'.
	p(NT(262), (T(12)));
//G404: __E___E_cli_command_117_118(262) => 's' 'u' 'b' 's' 't'.
	p(NT(262), (T(12)+T(49)+T(7)+T(12)+T(13)));
//G405: __E___E_cli_command_117_118(262) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(262), (T(12)+T(49)+T(7)+T(12)+T(13)+T(10)+T(13)+T(49)+T(13)+T(19)));
//G406: subst_sym(261)       => __E___E_cli_command_117_118(262).
	p(NT(261), (NT(262)));
//G407: __E_cli_command_117(260) => subst_sym(261) __(27) nf_cmd_arg(263) _(11) '[' _(11) nf_cmd_arg(263) _(11) '/' _(11) nf_cmd_arg(263) _(11) ']'.
	p(NT(260), (NT(261)+NT(27)+NT(263)+NT(11)+T(24)+NT(11)+NT(263)+NT(11)+T(57)+NT(11)+NT(263)+NT(11)+T(25)));
//G408: subst_cmd(259)       => __E_cli_command_117(260).
	p(NT(259), (NT(260)));
//G409: cli_command(208)     => subst_cmd(259).
	p(NT(208), (NT(259)));
//G410: __E___E_cli_command_119_120(267) => 'i'.
	p(NT(267), (T(10)));
//G411: __E___E_cli_command_119_120(267) => 'i' 'n' 's' 't'.
	p(NT(267), (T(10)+T(51)+T(12)+T(13)));
//G412: __E___E_cli_command_119_120(267) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(267), (T(10)+T(51)+T(12)+T(13)+T(5)+T(51)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G413: inst_sym(266)        => __E___E_cli_command_119_120(267).
	p(NT(266), (NT(267)));
//G414: __E_cli_command_119(265) => inst_sym(266) __(27) inst_args(268).
	p(NT(265), (NT(266)+NT(27)+NT(268)));
//G415: inst_cmd(264)        => __E_cli_command_119(265).
	p(NT(264), (NT(265)));
//G416: cli_command(208)     => inst_cmd(264).
	p(NT(208), (NT(264)));
//G417: dnf_sym(271)         => 'd' 'n' 'f'.
	p(NT(271), (T(54)+T(51)+T(4)));
//G418: __E_cli_command_121(270) => dnf_sym(271) __(27) nf_cmd_arg(263).
	p(NT(270), (NT(271)+NT(27)+NT(263)));
//G419: dnf_cmd(269)         => __E_cli_command_121(270).
	p(NT(269), (NT(270)));
//G420: cli_command(208)     => dnf_cmd(269).
	p(NT(208), (NT(269)));
//G421: cnf_sym(274)         => 'c' 'n' 'f'.
	p(NT(274), (T(8)+T(51)+T(4)));
//G422: __E_cli_command_122(273) => cnf_sym(274) __(27) nf_cmd_arg(263).
	p(NT(273), (NT(274)+NT(27)+NT(263)));
//G423: cnf_cmd(272)         => __E_cli_command_122(273).
	p(NT(272), (NT(273)));
//G424: cli_command(208)     => cnf_cmd(272).
	p(NT(208), (NT(272)));
//G425: anf_sym(277)         => 'a' 'n' 'f'.
	p(NT(277), (T(5)+T(51)+T(4)));
//G426: __E_cli_command_123(276) => anf_sym(277) __(27) nf_cmd_arg(263).
	p(NT(276), (NT(277)+NT(27)+NT(263)));
//G427: anf_cmd(275)         => __E_cli_command_123(276).
	p(NT(275), (NT(276)));
//G428: cli_command(208)     => anf_cmd(275).
	p(NT(208), (NT(275)));
//G429: nnf_sym(280)         => 'n' 'n' 'f'.
	p(NT(280), (T(51)+T(51)+T(4)));
//G430: __E_cli_command_124(279) => nnf_sym(280) __(27) nf_cmd_arg(263).
	p(NT(279), (NT(280)+NT(27)+NT(263)));
//G431: nnf_cmd(278)         => __E_cli_command_124(279).
	p(NT(278), (NT(279)));
//G432: cli_command(208)     => nnf_cmd(278).
	p(NT(208), (NT(278)));
//G433: pnf_sym(283)         => 'p' 'n' 'f'.
	p(NT(283), (T(53)+T(51)+T(4)));
//G434: __E_cli_command_125(282) => pnf_sym(283) __(27) nf_cmd_arg(263).
	p(NT(282), (NT(283)+NT(27)+NT(263)));
//G435: pnf_cmd(281)         => __E_cli_command_125(282).
	p(NT(281), (NT(282)));
//G436: cli_command(208)     => pnf_cmd(281).
	p(NT(208), (NT(281)));
//G437: mnf_sym(286)         => 'm' 'n' 'f'.
	p(NT(286), (T(18)+T(51)+T(4)));
//G438: __E_cli_command_126(285) => mnf_sym(286) __(27) nf_cmd_arg(263).
	p(NT(285), (NT(286)+NT(27)+NT(263)));
//G439: mnf_cmd(284)         => __E_cli_command_126(285).
	p(NT(284), (NT(285)));
//G440: cli_command(208)     => mnf_cmd(284).
	p(NT(208), (NT(284)));
//G441: snf_sym(289)         => 's' 'n' 'f'.
	p(NT(289), (T(12)+T(51)+T(4)));
//G442: __E_cli_command_127(288) => snf_sym(289) __(27) nf_cmd_arg(263).
	p(NT(288), (NT(289)+NT(27)+NT(263)));
//G443: snf_cmd(287)         => __E_cli_command_127(288).
	p(NT(287), (NT(288)));
//G444: cli_command(208)     => snf_cmd(287).
	p(NT(208), (NT(287)));
//G445: onf_sym(292)         => 'o' 'n' 'f'.
	p(NT(292), (T(17)+T(51)+T(4)));
//G446: __E_cli_command_128(291) => onf_sym(292) __(27) variable(118) __(27) onf_cmd_arg(293).
	p(NT(291), (NT(292)+NT(27)+NT(118)+NT(27)+NT(293)));
//G447: onf_cmd(290)         => __E_cli_command_128(291).
	p(NT(290), (NT(291)));
//G448: cli_command(208)     => onf_cmd(290).
	p(NT(208), (NT(290)));
//G449: __E___E_cli_command_129_130(297) => 'd' 'e' 'f' 's'.
	p(NT(297), (T(54)+T(19)+T(4)+T(12)));
//G450: __E___E_cli_command_129_130(297) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(297), (T(54)+T(19)+T(4)+T(10)+T(51)+T(10)+T(13)+T(10)+T(17)+T(51)+T(12)));
//G451: def_sym(296)         => __E___E_cli_command_129_130(297).
	p(NT(296), (NT(297)));
//G452: __E_cli_command_129(295) => def_sym(296).
	p(NT(295), (NT(296)));
//G453: def_list_cmd(294)    => __E_cli_command_129(295).
	p(NT(294), (NT(295)));
//G454: cli_command(208)     => def_list_cmd(294).
	p(NT(208), (NT(294)));
//G455: __E_cli_command_131(299) => def_sym(296) __(27) num(149).
	p(NT(299), (NT(296)+NT(27)+NT(149)));
//G456: def_print_cmd(298)   => __E_cli_command_131(299).
	p(NT(298), (NT(299)));
//G457: cli_command(208)     => def_print_cmd(298).
	p(NT(208), (NT(298)));
//G458: def_rr_cmd(300)      => rec_relation(16).
	p(NT(300), (NT(16)));
//G459: cli_command(208)     => def_rr_cmd(300).
	p(NT(208), (NT(300)));
//G460: qelim_sym(303)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(303), (T(48)+T(19)+T(6)+T(10)+T(18)));
//G461: __E_cli_command_132(302) => qelim_sym(303) __(27) wff_cmd_arg(245).
	p(NT(302), (NT(303)+NT(27)+NT(245)));
//G462: qelim_cmd(301)       => __E_cli_command_132(302).
	p(NT(301), (NT(302)));
//G463: cli_command(208)     => qelim_cmd(301).
	p(NT(208), (NT(301)));
//G464: get_sym(306)         => 'g' 'e' 't'.
	p(NT(306), (T(55)+T(19)+T(13)));
//G465: __E___E_cli_command_133_134(307) => __(27) option_name(308).
	p(NT(307), (NT(27)+NT(308)));
//G466: __E___E_cli_command_133_134(307) => null.
	p(NT(307), (nul));
//G467: __E_cli_command_133(305) => get_sym(306) __E___E_cli_command_133_134(307).
	p(NT(305), (NT(306)+NT(307)));
//G468: get_cmd(304)         => __E_cli_command_133(305).
	p(NT(304), (NT(305)));
//G469: cli_command(208)     => get_cmd(304).
	p(NT(208), (NT(304)));
//G470: set_sym(311)         => 's' 'e' 't'.
	p(NT(311), (T(12)+T(19)+T(13)));
//G471: __E___E_cli_command_135_136(312) => __(27).
	p(NT(312), (NT(27)));
//G472: __E___E_cli_command_135_136(312) => _(11) '=' _(11).
	p(NT(312), (NT(11)+T(3)+NT(11)));
//G473: __E_cli_command_135(310) => set_sym(311) __(27) option_name(308) __E___E_cli_command_135_136(312) option_value(313).
	p(NT(310), (NT(311)+NT(27)+NT(308)+NT(312)+NT(313)));
//G474: set_cmd(309)         => __E_cli_command_135(310).
	p(NT(309), (NT(310)));
//G475: cli_command(208)     => set_cmd(309).
	p(NT(208), (NT(309)));
//G476: enable_sym(316)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(316), (T(19)+T(51)+T(5)+T(7)+T(6)+T(19)));
//G477: __E_cli_command_137(315) => enable_sym(316) __(27) option_name(308).
	p(NT(315), (NT(316)+NT(27)+NT(308)));
//G478: enable_cmd(314)      => __E_cli_command_137(315).
	p(NT(314), (NT(315)));
//G479: cli_command(208)     => enable_cmd(314).
	p(NT(208), (NT(314)));
//G480: disable_sym(319)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(319), (T(54)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G481: __E_cli_command_138(318) => disable_sym(319) __(27) option_name(308).
	p(NT(318), (NT(319)+NT(27)+NT(308)));
//G482: disable_cmd(317)     => __E_cli_command_138(318).
	p(NT(317), (NT(318)));
//G483: cli_command(208)     => disable_cmd(317).
	p(NT(208), (NT(317)));
//G484: toggle_sym(322)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(322), (T(13)+T(17)+T(55)+T(55)+T(6)+T(19)));
//G485: __E_cli_command_139(321) => toggle_sym(322) __(27) option_name(308).
	p(NT(321), (NT(322)+NT(27)+NT(308)));
//G486: toggle_cmd(320)      => __E_cli_command_139(321).
	p(NT(320), (NT(321)));
//G487: cli_command(208)     => toggle_cmd(320).
	p(NT(208), (NT(320)));
//G488: __E_cli_command_140(324) => type(199) __(27) var_name(172) _(11) '=' _(11) input_stream(325).
	p(NT(324), (NT(199)+NT(27)+NT(172)+NT(11)+T(3)+NT(11)+NT(325)));
//G489: def_input_cmd(323)   => __E_cli_command_140(324).
	p(NT(323), (NT(324)));
//G490: cli_command(208)     => def_input_cmd(323).
	p(NT(208), (NT(323)));
//G491: __E_cli_command_141(327) => type(199) __(27) var_name(172) _(11) '=' _(11) output_stream(328).
	p(NT(327), (NT(199)+NT(27)+NT(172)+NT(11)+T(3)+NT(11)+NT(328)));
//G492: def_output_cmd(326)  => __E_cli_command_141(327).
	p(NT(326), (NT(327)));
//G493: cli_command(208)     => def_output_cmd(326).
	p(NT(208), (NT(326)));
//G494: __E___E_cli_command_142_143(332) => 'h' 'i' 's' 't'.
	p(NT(332), (T(52)+T(10)+T(12)+T(13)));
//G495: __E___E_cli_command_142_143(332) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(332), (T(52)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G496: history_sym(331)     => __E___E_cli_command_142_143(332).
	p(NT(331), (NT(332)));
//G497: __E_cli_command_142(330) => history_sym(331).
	p(NT(330), (NT(331)));
//G498: history_list_cmd(329) => __E_cli_command_142(330).
	p(NT(329), (NT(330)));
//G499: cli_command(208)     => history_list_cmd(329).
	p(NT(208), (NT(329)));
//G500: __E_cli_command_144(334) => history_sym(331) __(27) history(254).
	p(NT(334), (NT(331)+NT(27)+NT(254)));
//G501: history_print_cmd(333) => __E_cli_command_144(334).
	p(NT(333), (NT(334)));
//G502: cli_command(208)     => history_print_cmd(333).
	p(NT(208), (NT(333)));
//G503: __E_cli_command_145(336) => wff(14).
	p(NT(336), (NT(14)));
//G504: __E_cli_command_145(336) => bf(21).
	p(NT(336), (NT(21)));
//G505: history_store_cmd(335) => __E_cli_command_145(336).
	p(NT(335), (NT(336)));
//G506: cli_command(208)     => history_store_cmd(335).
	p(NT(208), (NT(335)));
//G507: __E_solve_options_146(337) => __(27) solve_option(338).
	p(NT(337), (NT(27)+NT(338)));
//G508: __E_solve_options_147(339) => null.
	p(NT(339), (nul));
//G509: __E_solve_options_147(339) => __E_solve_options_146(337) __E_solve_options_147(339).
	p(NT(339), (NT(337)+NT(339)));
//G510: solve_options(244)   => __E_solve_options_147(339).
	p(NT(244), (NT(339)));
//G511: __E_solve_option_148(340) => solver_mode(341).
	p(NT(340), (NT(341)));
//G512: __E_solve_option_148(340) => type(199).
	p(NT(340), (NT(199)));
//G513: solve_option(338)    => '-' '-' __E_solve_option_148(340).
	p(NT(338), (T(28)+T(28)+NT(340)));
//G514: __E_solver_mode_149(343) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(343), (T(18)+T(10)+T(51)+T(10)+T(18)+T(49)+T(18)));
//G515: __E_solver_mode_149(343) => 'm' 'i' 'n'.
	p(NT(343), (T(18)+T(10)+T(51)));
//G516: solver_mode_minimum(342) => __E_solver_mode_149(343).
	p(NT(342), (NT(343)));
//G517: solver_mode(341)     => solver_mode_minimum(342).
	p(NT(341), (NT(342)));
//G518: __E_solver_mode_150(345) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(345), (T(18)+T(5)+T(27)+T(10)+T(18)+T(49)+T(18)));
//G519: __E_solver_mode_150(345) => 'm' 'a' 'x'.
	p(NT(345), (T(18)+T(5)+T(27)));
//G520: solver_mode_maximum(344) => __E_solver_mode_150(345).
	p(NT(344), (NT(345)));
//G521: solver_mode(341)     => solver_mode_maximum(344).
	p(NT(341), (NT(344)));
//G522: __E_input_stream_151(346) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(347) _(11) ')'.
	p(NT(346), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(347)+NT(11)+T(16)));
//G523: input_stream(325)    => __E_input_stream_151(346).
	p(NT(325), (NT(346)));
//G524: console_sym(348)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(348), (T(8)+T(17)+T(51)+T(12)+T(17)+T(6)+T(19)));
//G525: input_stream(325)    => console_sym(348).
	p(NT(325), (NT(348)));
//G526: __E_output_stream_152(349) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(347) _(11) ')'.
	p(NT(349), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(347)+NT(11)+T(16)));
//G527: output_stream(328)   => __E_output_stream_152(349).
	p(NT(328), (NT(349)));
//G528: output_stream(328)   => console_sym(348).
	p(NT(328), (NT(348)));
//G529: q_file_name(347)     => '"' file_name(350) '"'.
	p(NT(347), (T(58)+NT(350)+T(58)));
//G530: __E_file_name_153(351) => printable(8).
	p(NT(351), (NT(8)));
//G531: __E_file_name_153(351) => printable(8) __E_file_name_153(351).
	p(NT(351), (NT(8)+NT(351)));
//G532: file_name(350)       => __E_file_name_153(351).
	p(NT(350), (NT(351)));
//G533: __E_option_name_154(352) => alnum(6).
	p(NT(352), (NT(6)));
//G534: __E_option_name_154(352) => alnum(6) __E_option_name_154(352).
	p(NT(352), (NT(6)+NT(352)));
//G535: option_name(308)     => __E_option_name_154(352).
	p(NT(308), (NT(352)));
//G536: __E_option_value_155(353) => alnum(6).
	p(NT(353), (NT(6)));
//G537: __E_option_value_155(353) => alnum(6) __E_option_value_155(353).
	p(NT(353), (NT(6)+NT(353)));
//G538: option_value(313)    => __E_option_value_155(353).
	p(NT(313), (NT(353)));
//G539: bf_cmd_arg(354)      => history(254).
	p(NT(354), (NT(254)));
//G540: bf_cmd_arg(354)      => bf(21).
	p(NT(354), (NT(21)));
//G541: wff_cmd_arg(245)     => history(254).
	p(NT(245), (NT(254)));
//G542: wff_cmd_arg(245)     => wff(14).
	p(NT(245), (NT(14)));
//G543: nf_cmd_arg(263)      => history(254).
	p(NT(263), (NT(254)));
//G544: nf_cmd_arg(263)      => ref(18).
	p(NT(263), (NT(18)));
//G545: nf_cmd_arg(263)      => wff(14).
	p(NT(263), (NT(14)));
//G546: nf_cmd_arg(263)      => bf(21).
	p(NT(263), (NT(21)));
//G547: onf_cmd_arg(293)     => history(254).
	p(NT(293), (NT(254)));
//G548: onf_cmd_arg(293)     => wff(14).
	p(NT(293), (NT(14)));
//G549: normalize_cmd_arg(233) => history(254).
	p(NT(233), (NT(254)));
//G550: normalize_cmd_arg(233) => spec(10).
	p(NT(233), (NT(10)));
//G551: normalize_cmd_arg(233) => ref(18).
	p(NT(233), (NT(18)));
//G552: normalize_cmd_arg(233) => wff(14).
	p(NT(233), (NT(14)));
//G553: normalize_cmd_arg(233) => bf(21).
	p(NT(233), (NT(21)));
//G554: inst_args(268)       => wff_cmd_arg(245) _(11) '[' _(11) variable(118) _(11) '/' _(11) bf_cmd_arg(354) _(11) ']'.
	p(NT(268), (NT(245)+NT(11)+T(24)+NT(11)+NT(118)+NT(11)+T(57)+NT(11)+NT(354)+NT(11)+T(25)));
//G555: inst_args(268)       => bf_cmd_arg(354) _(11) '[' _(11) variable(118) _(11) '/' _(11) bf_cmd_arg(354) _(11) ']'.
	p(NT(268), (NT(354)+NT(11)+T(24)+NT(11)+NT(118)+NT(11)+T(57)+NT(11)+NT(354)+NT(11)+T(25)));
//G556: help_arg(225)        => help_sym(222).
	p(NT(225), (NT(222)));
//G557: help_arg(225)        => version_sym(215).
	p(NT(225), (NT(215)));
//G558: help_arg(225)        => quit_sym(212).
	p(NT(225), (NT(212)));
//G559: help_arg(225)        => clear_sym(218).
	p(NT(225), (NT(218)));
//G560: help_arg(225)        => get_sym(306).
	p(NT(225), (NT(306)));
//G561: help_arg(225)        => set_sym(311).
	p(NT(225), (NT(311)));
//G562: help_arg(225)        => enable_sym(316).
	p(NT(225), (NT(316)));
//G563: help_arg(225)        => disable_sym(319).
	p(NT(225), (NT(319)));
//G564: help_arg(225)        => toggle_sym(322).
	p(NT(225), (NT(322)));
//G565: help_arg(225)        => file_sym(228).
	p(NT(225), (NT(228)));
//G566: help_arg(225)        => history_sym(331).
	p(NT(225), (NT(331)));
//G567: help_arg(225)        => abs_history_sym(355).
	p(NT(225), (NT(355)));
//G568: help_arg(225)        => rel_history_sym(356).
	p(NT(225), (NT(356)));
//G569: help_arg(225)        => selection_sym(357).
	p(NT(225), (NT(357)));
//G570: help_arg(225)        => def_sym(296).
	p(NT(225), (NT(296)));
//G571: help_arg(225)        => inst_sym(266).
	p(NT(225), (NT(266)));
//G572: help_arg(225)        => subst_sym(261).
	p(NT(225), (NT(261)));
//G573: help_arg(225)        => normalize_sym(257).
	p(NT(225), (NT(257)));
//G574: help_arg(225)        => execute_sym(358).
	p(NT(225), (NT(358)));
//G575: help_arg(225)        => solve_sym(242).
	p(NT(225), (NT(242)));
//G576: help_arg(225)        => valid_sym(232).
	p(NT(225), (NT(232)));
//G577: help_arg(225)        => sat_sym(236).
	p(NT(225), (NT(236)));
//G578: help_arg(225)        => unsat_sym(239).
	p(NT(225), (NT(239)));
//G579: help_arg(225)        => run_sym(251).
	p(NT(225), (NT(251)));
//G580: help_arg(225)        => dnf_sym(271).
	p(NT(225), (NT(271)));
//G581: help_arg(225)        => cnf_sym(274).
	p(NT(225), (NT(274)));
//G582: help_arg(225)        => anf_sym(277).
	p(NT(225), (NT(277)));
//G583: help_arg(225)        => snf_sym(289).
	p(NT(225), (NT(289)));
//G584: help_arg(225)        => nnf_sym(280).
	p(NT(225), (NT(280)));
//G585: help_arg(225)        => pnf_sym(283).
	p(NT(225), (NT(283)));
//G586: help_arg(225)        => mnf_sym(286).
	p(NT(225), (NT(286)));
//G587: help_arg(225)        => onf_sym(292).
	p(NT(225), (NT(292)));
//G588: help_arg(225)        => qelim_sym(303).
	p(NT(225), (NT(303)));
//G589: __E___E_help_arg_156_157(361) => 's'.
	p(NT(361), (T(12)));
//G590: __E___E_help_arg_156_157(361) => null.
	p(NT(361), (nul));
//G591: __E_help_arg_156(360) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_156_157(361).
	p(NT(360), (T(19)+T(27)+T(5)+T(18)+T(53)+T(6)+T(19)+NT(361)));
//G592: examples_sym(359)    => __E_help_arg_156(360).
	p(NT(359), (NT(360)));
//G593: help_arg(225)        => examples_sym(359).
	p(NT(225), (NT(359)));
//G594: __E_history_158(363) => '%'.
	p(NT(363), (T(59)));
//G595: rel_history_sym(356) => '%' '-'.
	p(NT(356), (T(59)+T(28)));
//G596: history_id(365)      => digits(196).
	p(NT(365), (NT(196)));
//G597: __E___E_history_158_159(364) => history_id(365).
	p(NT(364), (NT(365)));
//G598: __E___E_history_158_159(364) => null.
	p(NT(364), (nul));
//G599: __E_history_158(363) => rel_history_sym(356) __E___E_history_158_159(364).
	p(NT(363), (NT(356)+NT(364)));
//G600: rel_history(362)     => __E_history_158(363).
	p(NT(362), (NT(363)));
//G601: history(254)         => rel_history(362).
	p(NT(254), (NT(362)));
//G602: abs_history_sym(355) => '%'.
	p(NT(355), (T(59)));
//G603: __E_history_160(367) => abs_history_sym(355) history_id(365).
	p(NT(367), (NT(355)+NT(365)));
//G604: abs_history(366)     => __E_history_160(367).
	p(NT(366), (NT(367)));
//G605: history(254)         => abs_history(366).
	p(NT(254), (NT(366)));
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
		spec, _, rec_relations, main, wff, __E_rec_relations_0, rec_relation, __E_rec_relations_1, ref, __E_rec_relation_2, 
		capture, bf, sym, __E_ref_3, offsets, ref_args, __E_ref_4, __, fp_fallback, first_sym, 
		last_sym, __E_ref_args_5, __E___E_ref_args_5_6, ref_arg, __E___E_ref_args_5_7, __E___E_ref_args_5_8, library, rules, __E_library_9, __E___E_library_9_10, 
		rule, __E___E___E_library_9_10_11, wff_rule, bf_rule, __E___E_library_9_12, wff_matcher, wff_body, bf_matcher, bf_body, builder, 
		builder_head, builder_body, __E_builder_head_13, __E_builder_head_14, bf_builder_body, __E_builder_body_15, wff_builder_body, __E_builder_body_16, tau_constant_source, __E_tau_constant_source_17, 
		wff_parenthesis, __E_wff_18, wff_sometimes, __E_wff_19, __E___E_wff_19_20, wff_always, __E_wff_21, __E___E_wff_21_22, wff_conditional, __E_wff_23, 
		wff_all, __E_wff_24, q_vars, wff_ex, __E_wff_25, wff_ref, constraint, wff_imply, __E_wff_26, wff_rimply, 
		__E_wff_27, wff_equiv, __E_wff_28, wff_or, __E_wff_29, wff_xor, __E_wff_30, wff_and, __E_wff_31, wff_neg, 
		__E_wff_32, wff_t, wff_f, bf_interval, __E_wff_33, bf_eq, __E_wff_34, bf_neq, __E_wff_35, bf_lteq, 
		__E_wff_36, bf_nlteq, __E_wff_37, bf_gt, __E_wff_38, bf_ngt, __E_wff_39, bf_gteq, __E_wff_40, bf_ngteq, 
		__E_wff_41, bf_lt, __E_wff_42, bf_nlt, __E_wff_43, bf_parenthesis, __E_bf_44, bf_constant, variable, bf_splitter, 
		__E_bf_45, bf_ref, bf_or, __E_bf_46, bf_xor, __E_bf_47, bf_t, __E_bf_48, __E___E_bf_48_49, typed, 
		bf_f, __E_bf_50, __E___E_bf_50_51, bf_and, __E_bf_52, __E___E_bf_52_53, bf_neg, __E_bf_54, bf_neg_oprnd, __E___E_bf_54_55, 
		bf_and_nosep, __E_bf_56, bf_and_nosep_1st_oprnd, __E___E_bf_56_57, bf_and_nosep_2nd_oprnd, __E___E_bf_56_58, ctn_neq, __E_constraint_59, ctnvar, num, 
		ctn_eq, __E_constraint_60, ctn_gteq, __E_constraint_61, ctn_gt, __E_constraint_62, ctn_lteq, __E_constraint_63, ctn_lt, __E_constraint_64, 
		__E_bf_constant_65, source, __E_bf_constant_66, src_c, __E_source_67, __E___E_source_67_68, __E___E_source_67_69, __E_src_c_70, __E_src_c_71, __E_variable_72, 
		uconst, io_var, var_name, __E_variable_73, __E_var_name_74, __E_var_name_75, __E_var_name_76, __E_var_name_77, __E_var_name_78, offset, 
		uconst_name, __E_uconst_79, __E___E_uconst_79_80, chars, q_var, __E_q_vars_81, __E_q_vars_82, __E_offsets_83, __E_offsets_84, integer, 
		shift, __E_offset_85, __E_shift_86, __E___E_shift_86_87, __E_chars_88, __E_chars_89, digits, __E_digits_90, __E_integer_91, type, 
		scope_id, comment, __E_comment_92, __E_comment_93, __E_comment_94, __E____95, __E___96, cli, cli_command, __E_cli_97, 
		__E_cli_98, quit_cmd, quit_sym, __E_cli_command_99, version_cmd, version_sym, __E_cli_command_100, clear_cmd, clear_sym, __E_cli_command_101, 
		help_cmd, __E_cli_command_102, help_sym, __E___E_cli_command_102_103, __E___E_cli_command_102_104, help_arg, file_cmd, __E_cli_command_105, file_sym, q_string, 
		valid_cmd, __E_cli_command_106, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_107, sat_sym, unsat_cmd, __E_cli_command_108, unsat_sym, 
		solve_cmd, __E_cli_command_109, solve_sym, __E___E_cli_command_109_110, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_111, lgrs_sym, run_cmd, 
		__E_cli_command_112, run_sym, __E___E_cli_command_112_113, __E___E_cli_command_112_114, history, normalize_cmd, __E_cli_command_115, normalize_sym, __E___E_cli_command_115_116, subst_cmd, 
		__E_cli_command_117, subst_sym, __E___E_cli_command_117_118, nf_cmd_arg, inst_cmd, __E_cli_command_119, inst_sym, __E___E_cli_command_119_120, inst_args, dnf_cmd, 
		__E_cli_command_121, dnf_sym, cnf_cmd, __E_cli_command_122, cnf_sym, anf_cmd, __E_cli_command_123, anf_sym, nnf_cmd, __E_cli_command_124, 
		nnf_sym, pnf_cmd, __E_cli_command_125, pnf_sym, mnf_cmd, __E_cli_command_126, mnf_sym, snf_cmd, __E_cli_command_127, snf_sym, 
		onf_cmd, __E_cli_command_128, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_129, def_sym, __E___E_cli_command_129_130, def_print_cmd, __E_cli_command_131, 
		def_rr_cmd, qelim_cmd, __E_cli_command_132, qelim_sym, get_cmd, __E_cli_command_133, get_sym, __E___E_cli_command_133_134, option_name, set_cmd, 
		__E_cli_command_135, set_sym, __E___E_cli_command_135_136, option_value, enable_cmd, __E_cli_command_137, enable_sym, disable_cmd, __E_cli_command_138, disable_sym, 
		toggle_cmd, __E_cli_command_139, toggle_sym, def_input_cmd, __E_cli_command_140, input_stream, def_output_cmd, __E_cli_command_141, output_stream, history_list_cmd, 
		__E_cli_command_142, history_sym, __E___E_cli_command_142_143, history_print_cmd, __E_cli_command_144, history_store_cmd, __E_cli_command_145, __E_solve_options_146, solve_option, __E_solve_options_147, 
		__E_solve_option_148, solver_mode, solver_mode_minimum, __E_solver_mode_149, solver_mode_maximum, __E_solver_mode_150, __E_input_stream_151, q_file_name, console_sym, __E_output_stream_152, 
		file_name, __E_file_name_153, __E_option_name_154, __E_option_value_155, bf_cmd_arg, abs_history_sym, rel_history_sym, selection_sym, execute_sym, examples_sym, 
		__E_help_arg_156, __E___E_help_arg_156_157, rel_history, __E_history_158, __E___E_history_158_159, history_id, abs_history, __E_history_160, __N_0, __N_1, 
		__N_2, __N_3, __N_4, __N_5, __N_6, __N_7, __N_8, 
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
