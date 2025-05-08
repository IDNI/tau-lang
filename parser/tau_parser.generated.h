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
	"comment", "__E_comment_92", "__E_comment_93", "__E_comment_94", "__E____95", "__E___96", "cli", "cli_command", "__E_cli_97", "__E_cli_98", 
	"quit_cmd", "quit_sym", "__E_cli_command_99", "version_cmd", "version_sym", "__E_cli_command_100", "clear_cmd", "clear_sym", "__E_cli_command_101", "help_cmd", 
	"__E_cli_command_102", "help_sym", "__E___E_cli_command_102_103", "__E___E_cli_command_102_104", "help_arg", "file_cmd", "__E_cli_command_105", "file_sym", "q_string", "valid_cmd", 
	"__E_cli_command_106", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_107", "sat_sym", "unsat_cmd", "__E_cli_command_108", "unsat_sym", "solve_cmd", 
	"__E_cli_command_109", "solve_sym", "__E___E_cli_command_109_110", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_111", "lgrs_sym", "run_cmd", "__E_cli_command_112", 
	"run_sym", "__E___E_cli_command_112_113", "__E___E_cli_command_112_114", "memory", "normalize_cmd", "__E_cli_command_115", "normalize_sym", "__E___E_cli_command_115_116", "subst_cmd", "__E_cli_command_117", 
	"subst_sym", "__E___E_cli_command_117_118", "nf_cmd_arg", "inst_cmd", "__E_cli_command_119", "inst_sym", "__E___E_cli_command_119_120", "inst_args", "dnf_cmd", "__E_cli_command_121", 
	"dnf_sym", "cnf_cmd", "__E_cli_command_122", "cnf_sym", "anf_cmd", "__E_cli_command_123", "anf_sym", "nnf_cmd", "__E_cli_command_124", "nnf_sym", 
	"pnf_cmd", "__E_cli_command_125", "pnf_sym", "mnf_cmd", "__E_cli_command_126", "mnf_sym", "snf_cmd", "__E_cli_command_127", "snf_sym", "onf_cmd", 
	"__E_cli_command_128", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_129", "def_sym", "__E___E_cli_command_129_130", "def_print_cmd", "__E_cli_command_131", "number", 
	"def_rr_cmd", "qelim_cmd", "__E_cli_command_132", "qelim_sym", "get_cmd", "__E_cli_command_133", "get_sym", "__E___E_cli_command_133_134", "option_name", "set_cmd", 
	"__E_cli_command_135", "set_sym", "__E___E_cli_command_135_136", "option_value", "enable_cmd", "__E_cli_command_137", "enable_sym", "disable_cmd", "__E_cli_command_138", "disable_sym", 
	"toggle_cmd", "__E_cli_command_139", "toggle_sym", "def_input_cmd", "__E_cli_command_140", "input_stream", "def_output_cmd", "__E_cli_command_141", "output_stream", "history_list_cmd", 
	"__E_cli_command_142", "history_sym", "__E___E_cli_command_142_143", "history_print_cmd", "__E_cli_command_144", "history_store_cmd", "__E_cli_command_145", "__E_solve_options_146", "solve_option", "__E_solve_options_147", 
	"__E_solve_option_148", "solver_mode", "__E_solve_option_149", "solver_mode_minimum", "__E_solver_mode_150", "solver_mode_maximum", "__E_solver_mode_151", "__E_input_stream_152", "q_file_name", "console_sym", 
	"__E_output_stream_153", "file_name", "__E_file_name_154", "__E_option_name_155", "__E_option_value_156", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "execute_sym", 
	"examples_sym", "__E_help_arg_157", "__E___E_help_arg_157_158", "rel_memory", "__E_memory_159", "__E___E_memory_159_160", "memory_id", "abs_memory", "__E_memory_161", "__N_0", 
	"__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
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
			313, 351
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
			{ 232 },
			{ 244 },
			{ 262 },
			{ 267 },
			{ 292 },
			{ 355 }
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
//G247: __N_0(369)           => '{'.
	p(NT(369), (T(40)));
//G248: __N_1(370)           => '}'.
	p(NT(370), (T(41)));
//G249: src_c(163)           => ~( __N_0(369) ) & ~( __N_1(370) ) & punct(7).	 # conjunctive
	p(NT(163), ~(NT(369)) & ~(NT(370)) & (NT(7)));
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
//G261: __N_2(371)           => 'F'.
	p(NT(371), (T(34)));
//G262: __N_3(372)           => 'T'.
	p(NT(372), (T(33)));
//G263: __E_var_name_74(174) => ~( __N_2(371) ) & ~( __N_3(372) ) & alpha(5).	 # conjunctive
	p(NT(174), ~(NT(371)) & ~(NT(372)) & (NT(5)));
//G264: __E_var_name_75(175) => null.
	p(NT(175), (nul));
//G265: __E_var_name_75(175) => digit(3) __E_var_name_75(175).
	p(NT(175), (NT(3)+NT(175)));
//G266: var_name(172)        => __E_var_name_74(174) __E_var_name_75(175).	 # guarded: charvar
	p(NT(172), (NT(174)+NT(175)));
	p.back().guard = "charvar";
//G267: __N_4(373)           => 'F'.
	p(NT(373), (T(34)));
//G268: __N_5(374)           => 'T'.
	p(NT(374), (T(33)));
//G269: __E_var_name_76(176) => ~( __N_4(373) ) & ~( __N_5(374) ) & alpha(5).	 # conjunctive
	p(NT(176), ~(NT(373)) & ~(NT(374)) & (NT(5)));
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
//G286: __N_6(375)           => uconst(170).
	p(NT(375), (NT(170)));
//G287: q_var(184)           => variable(118) & ~( __N_6(375) ).	 # conjunctive
	p(NT(184), (NT(118)) & ~(NT(375)));
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
//G296: __N_7(376)           => io_var(171).
	p(NT(376), (NT(171)));
//G297: __E_offset_85(191)   => variable(118) & ~( __N_7(376) ).	 # conjunctive
	p(NT(191), (NT(118)) & ~(NT(376)));
//G298: offset(179)          => __E_offset_85(191).
	p(NT(179), (NT(191)));
//G299: __E_shift_86(192)    => capture(20).
	p(NT(192), (NT(20)));
//G300: __N_8(377)           => io_var(171).
	p(NT(377), (NT(171)));
//G301: __E___E_shift_86_87(193) => variable(118) & ~( __N_8(377) ).	 # conjunctive
	p(NT(193), (NT(118)) & ~(NT(377)));
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
//G320: __E_comment_92(201)  => printable(8).
	p(NT(201), (NT(8)));
//G321: __E_comment_92(201)  => '\t'.
	p(NT(201), (T(44)));
//G322: __E_comment_93(202)  => null.
	p(NT(202), (nul));
//G323: __E_comment_93(202)  => __E_comment_92(201) __E_comment_93(202).
	p(NT(202), (NT(201)+NT(202)));
//G324: __E_comment_94(203)  => '\n'.
	p(NT(203), (T(45)));
//G325: __E_comment_94(203)  => '\r'.
	p(NT(203), (T(46)));
//G326: __E_comment_94(203)  => eof(1).
	p(NT(203), (NT(1)));
//G327: comment(200)         => '#' __E_comment_93(202) __E_comment_94(203).
	p(NT(200), (T(47)+NT(202)+NT(203)));
//G328: __E____95(204)       => space(2).
	p(NT(204), (NT(2)));
//G329: __E____95(204)       => comment(200).
	p(NT(204), (NT(200)));
//G330: __(27)               => __E____95(204) _(11).
	p(NT(27), (NT(204)+NT(11)));
//G331: __E___96(205)        => __(27).
	p(NT(205), (NT(27)));
//G332: __E___96(205)        => null.
	p(NT(205), (nul));
//G333: _(11)                => __E___96(205).
	p(NT(11), (NT(205)));
//G334: cli(206)             => _(11).
	p(NT(206), (NT(11)));
//G335: __E_cli_97(208)      => '.' _(11) cli_command(207) _(11).
	p(NT(208), (T(1)+NT(11)+NT(207)+NT(11)));
//G336: __E_cli_98(209)      => null.
	p(NT(209), (nul));
//G337: __E_cli_98(209)      => __E_cli_97(208) __E_cli_98(209).
	p(NT(209), (NT(208)+NT(209)));
//G338: cli(206)             => _(11) cli_command(207) _(11) __E_cli_98(209).
	p(NT(206), (NT(11)+NT(207)+NT(11)+NT(209)));
//G339: __E_cli_command_99(212) => 'q'.
	p(NT(212), (T(48)));
//G340: __E_cli_command_99(212) => 'q' 'u' 'i' 't'.
	p(NT(212), (T(48)+T(49)+T(10)+T(13)));
//G341: quit_sym(211)        => __E_cli_command_99(212).
	p(NT(211), (NT(212)));
//G342: quit_cmd(210)        => quit_sym(211).
	p(NT(210), (NT(211)));
//G343: cli_command(207)     => quit_cmd(210).
	p(NT(207), (NT(210)));
//G344: __E_cli_command_100(215) => 'v'.
	p(NT(215), (T(50)));
//G345: __E_cli_command_100(215) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(215), (T(50)+T(19)+T(11)+T(12)+T(10)+T(17)+T(51)));
//G346: version_sym(214)     => __E_cli_command_100(215).
	p(NT(214), (NT(215)));
//G347: version_cmd(213)     => version_sym(214).
	p(NT(213), (NT(214)));
//G348: cli_command(207)     => version_cmd(213).
	p(NT(207), (NT(213)));
//G349: __E_cli_command_101(218) => 'c'.
	p(NT(218), (T(8)));
//G350: __E_cli_command_101(218) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(218), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G351: clear_sym(217)       => __E_cli_command_101(218).
	p(NT(217), (NT(218)));
//G352: clear_cmd(216)       => clear_sym(217).
	p(NT(216), (NT(217)));
//G353: cli_command(207)     => clear_cmd(216).
	p(NT(207), (NT(216)));
//G354: __E___E_cli_command_102_103(222) => 'h'.
	p(NT(222), (T(52)));
//G355: __E___E_cli_command_102_103(222) => 'h' 'e' 'l' 'p'.
	p(NT(222), (T(52)+T(19)+T(6)+T(53)));
//G356: help_sym(221)        => __E___E_cli_command_102_103(222).
	p(NT(221), (NT(222)));
//G357: __E___E_cli_command_102_104(223) => __(27) help_arg(224).
	p(NT(223), (NT(27)+NT(224)));
//G358: __E___E_cli_command_102_104(223) => null.
	p(NT(223), (nul));
//G359: __E_cli_command_102(220) => help_sym(221) __E___E_cli_command_102_104(223).
	p(NT(220), (NT(221)+NT(223)));
//G360: help_cmd(219)        => __E_cli_command_102(220).
	p(NT(219), (NT(220)));
//G361: cli_command(207)     => help_cmd(219).
	p(NT(207), (NT(219)));
//G362: file_sym(227)        => 'f' 'i' 'l' 'e'.
	p(NT(227), (T(4)+T(10)+T(6)+T(19)));
//G363: __E_cli_command_105(226) => file_sym(227) __(27) q_string(228).
	p(NT(226), (NT(227)+NT(27)+NT(228)));
//G364: file_cmd(225)        => __E_cli_command_105(226).
	p(NT(225), (NT(226)));
//G365: cli_command(207)     => file_cmd(225).
	p(NT(207), (NT(225)));
//G366: valid_sym(231)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(231), (T(50)+T(5)+T(6)+T(10)+T(54)));
//G367: __E_cli_command_106(230) => valid_sym(231) __(27) normalize_cmd_arg(232).
	p(NT(230), (NT(231)+NT(27)+NT(232)));
//G368: valid_cmd(229)       => __E_cli_command_106(230).
	p(NT(229), (NT(230)));
//G369: cli_command(207)     => valid_cmd(229).
	p(NT(207), (NT(229)));
//G370: sat_sym(235)         => 's' 'a' 't'.
	p(NT(235), (T(12)+T(5)+T(13)));
//G371: __E_cli_command_107(234) => sat_sym(235) __(27) normalize_cmd_arg(232).
	p(NT(234), (NT(235)+NT(27)+NT(232)));
//G372: sat_cmd(233)         => __E_cli_command_107(234).
	p(NT(233), (NT(234)));
//G373: cli_command(207)     => sat_cmd(233).
	p(NT(207), (NT(233)));
//G374: unsat_sym(238)       => 'u' 'n' 's' 'a' 't'.
	p(NT(238), (T(49)+T(51)+T(12)+T(5)+T(13)));
//G375: __E_cli_command_108(237) => unsat_sym(238) __(27) normalize_cmd_arg(232).
	p(NT(237), (NT(238)+NT(27)+NT(232)));
//G376: unsat_cmd(236)       => __E_cli_command_108(237).
	p(NT(236), (NT(237)));
//G377: cli_command(207)     => unsat_cmd(236).
	p(NT(207), (NT(236)));
//G378: solve_sym(241)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(241), (T(12)+T(17)+T(6)+T(50)+T(19)));
//G379: __E___E_cli_command_109_110(242) => solve_options(243).
	p(NT(242), (NT(243)));
//G380: __E___E_cli_command_109_110(242) => null.
	p(NT(242), (nul));
//G381: __E_cli_command_109(240) => solve_sym(241) __E___E_cli_command_109_110(242) __(27) wff_cmd_arg(244).
	p(NT(240), (NT(241)+NT(242)+NT(27)+NT(244)));
//G382: solve_cmd(239)       => __E_cli_command_109(240).
	p(NT(239), (NT(240)));
//G383: cli_command(207)     => solve_cmd(239).
	p(NT(207), (NT(239)));
//G384: lgrs_sym(247)        => 'l' 'g' 'r' 's'.
	p(NT(247), (T(6)+T(55)+T(11)+T(12)));
//G385: __E_cli_command_111(246) => lgrs_sym(247) __(27) wff_cmd_arg(244).
	p(NT(246), (NT(247)+NT(27)+NT(244)));
//G386: lgrs_cmd(245)        => __E_cli_command_111(246).
	p(NT(245), (NT(246)));
//G387: cli_command(207)     => lgrs_cmd(245).
	p(NT(207), (NT(245)));
//G388: __E___E_cli_command_112_113(251) => 'r'.
	p(NT(251), (T(11)));
//G389: __E___E_cli_command_112_113(251) => 'r' 'u' 'n'.
	p(NT(251), (T(11)+T(49)+T(51)));
//G390: run_sym(250)         => __E___E_cli_command_112_113(251).
	p(NT(250), (NT(251)));
//G391: __E___E_cli_command_112_114(252) => memory(253).
	p(NT(252), (NT(253)));
//G392: __E___E_cli_command_112_114(252) => wff(14).
	p(NT(252), (NT(14)));
//G393: __E_cli_command_112(249) => run_sym(250) __(27) __E___E_cli_command_112_114(252).
	p(NT(249), (NT(250)+NT(27)+NT(252)));
//G394: run_cmd(248)         => __E_cli_command_112(249).
	p(NT(248), (NT(249)));
//G395: cli_command(207)     => run_cmd(248).
	p(NT(207), (NT(248)));
//G396: __E___E_cli_command_115_116(257) => 'n'.
	p(NT(257), (T(51)));
//G397: __E___E_cli_command_115_116(257) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(257), (T(51)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(56)+T(19)));
//G398: normalize_sym(256)   => __E___E_cli_command_115_116(257).
	p(NT(256), (NT(257)));
//G399: __E_cli_command_115(255) => normalize_sym(256) __(27) normalize_cmd_arg(232).
	p(NT(255), (NT(256)+NT(27)+NT(232)));
//G400: normalize_cmd(254)   => __E_cli_command_115(255).
	p(NT(254), (NT(255)));
//G401: cli_command(207)     => normalize_cmd(254).
	p(NT(207), (NT(254)));
//G402: __E___E_cli_command_117_118(261) => 's'.
	p(NT(261), (T(12)));
//G403: __E___E_cli_command_117_118(261) => 's' 'u' 'b' 's' 't'.
	p(NT(261), (T(12)+T(49)+T(7)+T(12)+T(13)));
//G404: __E___E_cli_command_117_118(261) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(261), (T(12)+T(49)+T(7)+T(12)+T(13)+T(10)+T(13)+T(49)+T(13)+T(19)));
//G405: subst_sym(260)       => __E___E_cli_command_117_118(261).
	p(NT(260), (NT(261)));
//G406: __E_cli_command_117(259) => subst_sym(260) __(27) nf_cmd_arg(262) _(11) '[' _(11) nf_cmd_arg(262) _(11) '/' _(11) nf_cmd_arg(262) _(11) ']'.
	p(NT(259), (NT(260)+NT(27)+NT(262)+NT(11)+T(24)+NT(11)+NT(262)+NT(11)+T(57)+NT(11)+NT(262)+NT(11)+T(25)));
//G407: subst_cmd(258)       => __E_cli_command_117(259).
	p(NT(258), (NT(259)));
//G408: cli_command(207)     => subst_cmd(258).
	p(NT(207), (NT(258)));
//G409: __E___E_cli_command_119_120(266) => 'i'.
	p(NT(266), (T(10)));
//G410: __E___E_cli_command_119_120(266) => 'i' 'n' 's' 't'.
	p(NT(266), (T(10)+T(51)+T(12)+T(13)));
//G411: __E___E_cli_command_119_120(266) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(266), (T(10)+T(51)+T(12)+T(13)+T(5)+T(51)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G412: inst_sym(265)        => __E___E_cli_command_119_120(266).
	p(NT(265), (NT(266)));
//G413: __E_cli_command_119(264) => inst_sym(265) __(27) inst_args(267).
	p(NT(264), (NT(265)+NT(27)+NT(267)));
//G414: inst_cmd(263)        => __E_cli_command_119(264).
	p(NT(263), (NT(264)));
//G415: cli_command(207)     => inst_cmd(263).
	p(NT(207), (NT(263)));
//G416: dnf_sym(270)         => 'd' 'n' 'f'.
	p(NT(270), (T(54)+T(51)+T(4)));
//G417: __E_cli_command_121(269) => dnf_sym(270) __(27) nf_cmd_arg(262).
	p(NT(269), (NT(270)+NT(27)+NT(262)));
//G418: dnf_cmd(268)         => __E_cli_command_121(269).
	p(NT(268), (NT(269)));
//G419: cli_command(207)     => dnf_cmd(268).
	p(NT(207), (NT(268)));
//G420: cnf_sym(273)         => 'c' 'n' 'f'.
	p(NT(273), (T(8)+T(51)+T(4)));
//G421: __E_cli_command_122(272) => cnf_sym(273) __(27) nf_cmd_arg(262).
	p(NT(272), (NT(273)+NT(27)+NT(262)));
//G422: cnf_cmd(271)         => __E_cli_command_122(272).
	p(NT(271), (NT(272)));
//G423: cli_command(207)     => cnf_cmd(271).
	p(NT(207), (NT(271)));
//G424: anf_sym(276)         => 'a' 'n' 'f'.
	p(NT(276), (T(5)+T(51)+T(4)));
//G425: __E_cli_command_123(275) => anf_sym(276) __(27) nf_cmd_arg(262).
	p(NT(275), (NT(276)+NT(27)+NT(262)));
//G426: anf_cmd(274)         => __E_cli_command_123(275).
	p(NT(274), (NT(275)));
//G427: cli_command(207)     => anf_cmd(274).
	p(NT(207), (NT(274)));
//G428: nnf_sym(279)         => 'n' 'n' 'f'.
	p(NT(279), (T(51)+T(51)+T(4)));
//G429: __E_cli_command_124(278) => nnf_sym(279) __(27) nf_cmd_arg(262).
	p(NT(278), (NT(279)+NT(27)+NT(262)));
//G430: nnf_cmd(277)         => __E_cli_command_124(278).
	p(NT(277), (NT(278)));
//G431: cli_command(207)     => nnf_cmd(277).
	p(NT(207), (NT(277)));
//G432: pnf_sym(282)         => 'p' 'n' 'f'.
	p(NT(282), (T(53)+T(51)+T(4)));
//G433: __E_cli_command_125(281) => pnf_sym(282) __(27) nf_cmd_arg(262).
	p(NT(281), (NT(282)+NT(27)+NT(262)));
//G434: pnf_cmd(280)         => __E_cli_command_125(281).
	p(NT(280), (NT(281)));
//G435: cli_command(207)     => pnf_cmd(280).
	p(NT(207), (NT(280)));
//G436: mnf_sym(285)         => 'm' 'n' 'f'.
	p(NT(285), (T(18)+T(51)+T(4)));
//G437: __E_cli_command_126(284) => mnf_sym(285) __(27) nf_cmd_arg(262).
	p(NT(284), (NT(285)+NT(27)+NT(262)));
//G438: mnf_cmd(283)         => __E_cli_command_126(284).
	p(NT(283), (NT(284)));
//G439: cli_command(207)     => mnf_cmd(283).
	p(NT(207), (NT(283)));
//G440: snf_sym(288)         => 's' 'n' 'f'.
	p(NT(288), (T(12)+T(51)+T(4)));
//G441: __E_cli_command_127(287) => snf_sym(288) __(27) nf_cmd_arg(262).
	p(NT(287), (NT(288)+NT(27)+NT(262)));
//G442: snf_cmd(286)         => __E_cli_command_127(287).
	p(NT(286), (NT(287)));
//G443: cli_command(207)     => snf_cmd(286).
	p(NT(207), (NT(286)));
//G444: onf_sym(291)         => 'o' 'n' 'f'.
	p(NT(291), (T(17)+T(51)+T(4)));
//G445: __E_cli_command_128(290) => onf_sym(291) __(27) variable(118) __(27) onf_cmd_arg(292).
	p(NT(290), (NT(291)+NT(27)+NT(118)+NT(27)+NT(292)));
//G446: onf_cmd(289)         => __E_cli_command_128(290).
	p(NT(289), (NT(290)));
//G447: cli_command(207)     => onf_cmd(289).
	p(NT(207), (NT(289)));
//G448: __E___E_cli_command_129_130(296) => 'd' 'e' 'f' 's'.
	p(NT(296), (T(54)+T(19)+T(4)+T(12)));
//G449: __E___E_cli_command_129_130(296) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(296), (T(54)+T(19)+T(4)+T(10)+T(51)+T(10)+T(13)+T(10)+T(17)+T(51)+T(12)));
//G450: def_sym(295)         => __E___E_cli_command_129_130(296).
	p(NT(295), (NT(296)));
//G451: __E_cli_command_129(294) => def_sym(295).
	p(NT(294), (NT(295)));
//G452: def_list_cmd(293)    => __E_cli_command_129(294).
	p(NT(293), (NT(294)));
//G453: cli_command(207)     => def_list_cmd(293).
	p(NT(207), (NT(293)));
//G454: __E_cli_command_131(298) => def_sym(295) __(27) number(299).
	p(NT(298), (NT(295)+NT(27)+NT(299)));
//G455: def_print_cmd(297)   => __E_cli_command_131(298).
	p(NT(297), (NT(298)));
//G456: cli_command(207)     => def_print_cmd(297).
	p(NT(207), (NT(297)));
//G457: def_rr_cmd(300)      => rec_relation(16).
	p(NT(300), (NT(16)));
//G458: cli_command(207)     => def_rr_cmd(300).
	p(NT(207), (NT(300)));
//G459: qelim_sym(303)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(303), (T(48)+T(19)+T(6)+T(10)+T(18)));
//G460: __E_cli_command_132(302) => qelim_sym(303) __(27) wff_cmd_arg(244).
	p(NT(302), (NT(303)+NT(27)+NT(244)));
//G461: qelim_cmd(301)       => __E_cli_command_132(302).
	p(NT(301), (NT(302)));
//G462: cli_command(207)     => qelim_cmd(301).
	p(NT(207), (NT(301)));
//G463: get_sym(306)         => 'g' 'e' 't'.
	p(NT(306), (T(55)+T(19)+T(13)));
//G464: __E___E_cli_command_133_134(307) => __(27) option_name(308).
	p(NT(307), (NT(27)+NT(308)));
//G465: __E___E_cli_command_133_134(307) => null.
	p(NT(307), (nul));
//G466: __E_cli_command_133(305) => get_sym(306) __E___E_cli_command_133_134(307).
	p(NT(305), (NT(306)+NT(307)));
//G467: get_cmd(304)         => __E_cli_command_133(305).
	p(NT(304), (NT(305)));
//G468: cli_command(207)     => get_cmd(304).
	p(NT(207), (NT(304)));
//G469: set_sym(311)         => 's' 'e' 't'.
	p(NT(311), (T(12)+T(19)+T(13)));
//G470: __E___E_cli_command_135_136(312) => __(27).
	p(NT(312), (NT(27)));
//G471: __E___E_cli_command_135_136(312) => _(11) '=' _(11).
	p(NT(312), (NT(11)+T(3)+NT(11)));
//G472: __E_cli_command_135(310) => set_sym(311) __(27) option_name(308) __E___E_cli_command_135_136(312) option_value(313).
	p(NT(310), (NT(311)+NT(27)+NT(308)+NT(312)+NT(313)));
//G473: set_cmd(309)         => __E_cli_command_135(310).
	p(NT(309), (NT(310)));
//G474: cli_command(207)     => set_cmd(309).
	p(NT(207), (NT(309)));
//G475: enable_sym(316)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(316), (T(19)+T(51)+T(5)+T(7)+T(6)+T(19)));
//G476: __E_cli_command_137(315) => enable_sym(316) __(27) option_name(308).
	p(NT(315), (NT(316)+NT(27)+NT(308)));
//G477: enable_cmd(314)      => __E_cli_command_137(315).
	p(NT(314), (NT(315)));
//G478: cli_command(207)     => enable_cmd(314).
	p(NT(207), (NT(314)));
//G479: disable_sym(319)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(319), (T(54)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G480: __E_cli_command_138(318) => disable_sym(319) __(27) option_name(308).
	p(NT(318), (NT(319)+NT(27)+NT(308)));
//G481: disable_cmd(317)     => __E_cli_command_138(318).
	p(NT(317), (NT(318)));
//G482: cli_command(207)     => disable_cmd(317).
	p(NT(207), (NT(317)));
//G483: toggle_sym(322)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(322), (T(13)+T(17)+T(55)+T(55)+T(6)+T(19)));
//G484: __E_cli_command_139(321) => toggle_sym(322) __(27) option_name(308).
	p(NT(321), (NT(322)+NT(27)+NT(308)));
//G485: toggle_cmd(320)      => __E_cli_command_139(321).
	p(NT(320), (NT(321)));
//G486: cli_command(207)     => toggle_cmd(320).
	p(NT(207), (NT(320)));
//G487: __E_cli_command_140(324) => type(199) __(27) var_name(172) _(11) '=' _(11) input_stream(325).
	p(NT(324), (NT(199)+NT(27)+NT(172)+NT(11)+T(3)+NT(11)+NT(325)));
//G488: def_input_cmd(323)   => __E_cli_command_140(324).
	p(NT(323), (NT(324)));
//G489: cli_command(207)     => def_input_cmd(323).
	p(NT(207), (NT(323)));
//G490: __E_cli_command_141(327) => type(199) __(27) var_name(172) _(11) '=' _(11) output_stream(328).
	p(NT(327), (NT(199)+NT(27)+NT(172)+NT(11)+T(3)+NT(11)+NT(328)));
//G491: def_output_cmd(326)  => __E_cli_command_141(327).
	p(NT(326), (NT(327)));
//G492: cli_command(207)     => def_output_cmd(326).
	p(NT(207), (NT(326)));
//G493: __E___E_cli_command_142_143(332) => 'h' 'i' 's' 't'.
	p(NT(332), (T(52)+T(10)+T(12)+T(13)));
//G494: __E___E_cli_command_142_143(332) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(332), (T(52)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G495: history_sym(331)     => __E___E_cli_command_142_143(332).
	p(NT(331), (NT(332)));
//G496: __E_cli_command_142(330) => history_sym(331).
	p(NT(330), (NT(331)));
//G497: history_list_cmd(329) => __E_cli_command_142(330).
	p(NT(329), (NT(330)));
//G498: cli_command(207)     => history_list_cmd(329).
	p(NT(207), (NT(329)));
//G499: __E_cli_command_144(334) => history_sym(331) __(27) memory(253).
	p(NT(334), (NT(331)+NT(27)+NT(253)));
//G500: history_print_cmd(333) => __E_cli_command_144(334).
	p(NT(333), (NT(334)));
//G501: cli_command(207)     => history_print_cmd(333).
	p(NT(207), (NT(333)));
//G502: __E_cli_command_145(336) => wff(14).
	p(NT(336), (NT(14)));
//G503: __E_cli_command_145(336) => bf(21).
	p(NT(336), (NT(21)));
//G504: history_store_cmd(335) => __E_cli_command_145(336).
	p(NT(335), (NT(336)));
//G505: cli_command(207)     => history_store_cmd(335).
	p(NT(207), (NT(335)));
//G506: __E_solve_options_146(337) => __(27) solve_option(338).
	p(NT(337), (NT(27)+NT(338)));
//G507: __E_solve_options_147(339) => null.
	p(NT(339), (nul));
//G508: __E_solve_options_147(339) => __E_solve_options_146(337) __E_solve_options_147(339).
	p(NT(339), (NT(337)+NT(339)));
//G509: solve_options(243)   => __E_solve_options_147(339).
	p(NT(243), (NT(339)));
//G510: __E_solve_option_148(340) => '-' '-' solver_mode(341).
	p(NT(340), (T(28)+T(28)+NT(341)));
//G511: solve_option(338)    => __E_solve_option_148(340).
	p(NT(338), (NT(340)));
//G512: __E_solve_option_149(342) => '-' '-' type(199).
	p(NT(342), (T(28)+T(28)+NT(199)));
//G513: solve_option(338)    => __E_solve_option_149(342).
	p(NT(338), (NT(342)));
//G514: __E_solver_mode_150(344) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(344), (T(18)+T(10)+T(51)+T(10)+T(18)+T(49)+T(18)));
//G515: __E_solver_mode_150(344) => 'm' 'i' 'n'.
	p(NT(344), (T(18)+T(10)+T(51)));
//G516: solver_mode_minimum(343) => __E_solver_mode_150(344).
	p(NT(343), (NT(344)));
//G517: solver_mode(341)     => solver_mode_minimum(343).
	p(NT(341), (NT(343)));
//G518: __E_solver_mode_151(346) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(346), (T(18)+T(5)+T(27)+T(10)+T(18)+T(49)+T(18)));
//G519: __E_solver_mode_151(346) => 'm' 'a' 'x'.
	p(NT(346), (T(18)+T(5)+T(27)));
//G520: solver_mode_maximum(345) => __E_solver_mode_151(346).
	p(NT(345), (NT(346)));
//G521: solver_mode(341)     => solver_mode_maximum(345).
	p(NT(341), (NT(345)));
//G522: __E_input_stream_152(347) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(348) _(11) ')'.
	p(NT(347), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(348)+NT(11)+T(16)));
//G523: input_stream(325)    => __E_input_stream_152(347).
	p(NT(325), (NT(347)));
//G524: console_sym(349)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(349), (T(8)+T(17)+T(51)+T(12)+T(17)+T(6)+T(19)));
//G525: input_stream(325)    => console_sym(349).
	p(NT(325), (NT(349)));
//G526: __E_output_stream_153(350) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(348) _(11) ')'.
	p(NT(350), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(348)+NT(11)+T(16)));
//G527: output_stream(328)   => __E_output_stream_153(350).
	p(NT(328), (NT(350)));
//G528: output_stream(328)   => console_sym(349).
	p(NT(328), (NT(349)));
//G529: q_file_name(348)     => '"' file_name(351) '"'.
	p(NT(348), (T(58)+NT(351)+T(58)));
//G530: __E_file_name_154(352) => printable(8).
	p(NT(352), (NT(8)));
//G531: __E_file_name_154(352) => printable(8) __E_file_name_154(352).
	p(NT(352), (NT(8)+NT(352)));
//G532: file_name(351)       => __E_file_name_154(352).
	p(NT(351), (NT(352)));
//G533: __E_option_name_155(353) => alnum(6).
	p(NT(353), (NT(6)));
//G534: __E_option_name_155(353) => alnum(6) __E_option_name_155(353).
	p(NT(353), (NT(6)+NT(353)));
//G535: option_name(308)     => __E_option_name_155(353).
	p(NT(308), (NT(353)));
//G536: __E_option_value_156(354) => alnum(6).
	p(NT(354), (NT(6)));
//G537: __E_option_value_156(354) => alnum(6) __E_option_value_156(354).
	p(NT(354), (NT(6)+NT(354)));
//G538: option_value(313)    => __E_option_value_156(354).
	p(NT(313), (NT(354)));
//G539: number(299)          => digits(196).
	p(NT(299), (NT(196)));
//G540: bf_cmd_arg(355)      => memory(253).
	p(NT(355), (NT(253)));
//G541: bf_cmd_arg(355)      => bf(21).
	p(NT(355), (NT(21)));
//G542: wff_cmd_arg(244)     => memory(253).
	p(NT(244), (NT(253)));
//G543: wff_cmd_arg(244)     => wff(14).
	p(NT(244), (NT(14)));
//G544: nf_cmd_arg(262)      => memory(253).
	p(NT(262), (NT(253)));
//G545: nf_cmd_arg(262)      => ref(18).
	p(NT(262), (NT(18)));
//G546: nf_cmd_arg(262)      => wff(14).
	p(NT(262), (NT(14)));
//G547: nf_cmd_arg(262)      => bf(21).
	p(NT(262), (NT(21)));
//G548: onf_cmd_arg(292)     => memory(253).
	p(NT(292), (NT(253)));
//G549: onf_cmd_arg(292)     => wff(14).
	p(NT(292), (NT(14)));
//G550: normalize_cmd_arg(232) => memory(253).
	p(NT(232), (NT(253)));
//G551: normalize_cmd_arg(232) => spec(10).
	p(NT(232), (NT(10)));
//G552: normalize_cmd_arg(232) => ref(18).
	p(NT(232), (NT(18)));
//G553: normalize_cmd_arg(232) => wff(14).
	p(NT(232), (NT(14)));
//G554: normalize_cmd_arg(232) => bf(21).
	p(NT(232), (NT(21)));
//G555: inst_args(267)       => wff_cmd_arg(244) _(11) '[' _(11) variable(118) _(11) '/' _(11) bf_cmd_arg(355) _(11) ']'.
	p(NT(267), (NT(244)+NT(11)+T(24)+NT(11)+NT(118)+NT(11)+T(57)+NT(11)+NT(355)+NT(11)+T(25)));
//G556: inst_args(267)       => bf_cmd_arg(355) _(11) '[' _(11) variable(118) _(11) '/' _(11) bf_cmd_arg(355) _(11) ']'.
	p(NT(267), (NT(355)+NT(11)+T(24)+NT(11)+NT(118)+NT(11)+T(57)+NT(11)+NT(355)+NT(11)+T(25)));
//G557: help_arg(224)        => help_sym(221).
	p(NT(224), (NT(221)));
//G558: help_arg(224)        => version_sym(214).
	p(NT(224), (NT(214)));
//G559: help_arg(224)        => quit_sym(211).
	p(NT(224), (NT(211)));
//G560: help_arg(224)        => clear_sym(217).
	p(NT(224), (NT(217)));
//G561: help_arg(224)        => get_sym(306).
	p(NT(224), (NT(306)));
//G562: help_arg(224)        => set_sym(311).
	p(NT(224), (NT(311)));
//G563: help_arg(224)        => enable_sym(316).
	p(NT(224), (NT(316)));
//G564: help_arg(224)        => disable_sym(319).
	p(NT(224), (NT(319)));
//G565: help_arg(224)        => toggle_sym(322).
	p(NT(224), (NT(322)));
//G566: help_arg(224)        => file_sym(227).
	p(NT(224), (NT(227)));
//G567: help_arg(224)        => history_sym(331).
	p(NT(224), (NT(331)));
//G568: help_arg(224)        => abs_memory_sym(356).
	p(NT(224), (NT(356)));
//G569: help_arg(224)        => rel_memory_sym(357).
	p(NT(224), (NT(357)));
//G570: help_arg(224)        => selection_sym(358).
	p(NT(224), (NT(358)));
//G571: help_arg(224)        => def_sym(295).
	p(NT(224), (NT(295)));
//G572: help_arg(224)        => inst_sym(265).
	p(NT(224), (NT(265)));
//G573: help_arg(224)        => subst_sym(260).
	p(NT(224), (NT(260)));
//G574: help_arg(224)        => normalize_sym(256).
	p(NT(224), (NT(256)));
//G575: help_arg(224)        => execute_sym(359).
	p(NT(224), (NT(359)));
//G576: help_arg(224)        => solve_sym(241).
	p(NT(224), (NT(241)));
//G577: help_arg(224)        => valid_sym(231).
	p(NT(224), (NT(231)));
//G578: help_arg(224)        => sat_sym(235).
	p(NT(224), (NT(235)));
//G579: help_arg(224)        => unsat_sym(238).
	p(NT(224), (NT(238)));
//G580: help_arg(224)        => run_sym(250).
	p(NT(224), (NT(250)));
//G581: help_arg(224)        => dnf_sym(270).
	p(NT(224), (NT(270)));
//G582: help_arg(224)        => cnf_sym(273).
	p(NT(224), (NT(273)));
//G583: help_arg(224)        => anf_sym(276).
	p(NT(224), (NT(276)));
//G584: help_arg(224)        => snf_sym(288).
	p(NT(224), (NT(288)));
//G585: help_arg(224)        => nnf_sym(279).
	p(NT(224), (NT(279)));
//G586: help_arg(224)        => pnf_sym(282).
	p(NT(224), (NT(282)));
//G587: help_arg(224)        => mnf_sym(285).
	p(NT(224), (NT(285)));
//G588: help_arg(224)        => onf_sym(291).
	p(NT(224), (NT(291)));
//G589: help_arg(224)        => qelim_sym(303).
	p(NT(224), (NT(303)));
//G590: __E___E_help_arg_157_158(362) => 's'.
	p(NT(362), (T(12)));
//G591: __E___E_help_arg_157_158(362) => null.
	p(NT(362), (nul));
//G592: __E_help_arg_157(361) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_157_158(362).
	p(NT(361), (T(19)+T(27)+T(5)+T(18)+T(53)+T(6)+T(19)+NT(362)));
//G593: examples_sym(360)    => __E_help_arg_157(361).
	p(NT(360), (NT(361)));
//G594: help_arg(224)        => examples_sym(360).
	p(NT(224), (NT(360)));
//G595: __E_memory_159(364)  => '%'.
	p(NT(364), (T(59)));
//G596: rel_memory_sym(357)  => '%' '-'.
	p(NT(357), (T(59)+T(28)));
//G597: memory_id(366)       => digits(196).
	p(NT(366), (NT(196)));
//G598: __E___E_memory_159_160(365) => memory_id(366).
	p(NT(365), (NT(366)));
//G599: __E___E_memory_159_160(365) => null.
	p(NT(365), (nul));
//G600: __E_memory_159(364)  => rel_memory_sym(357) __E___E_memory_159_160(365).
	p(NT(364), (NT(357)+NT(365)));
//G601: rel_memory(363)      => __E_memory_159(364).
	p(NT(363), (NT(364)));
//G602: memory(253)          => rel_memory(363).
	p(NT(253), (NT(363)));
//G603: abs_memory_sym(356)  => '%'.
	p(NT(356), (T(59)));
//G604: __E_memory_161(368)  => abs_memory_sym(356) memory_id(366).
	p(NT(368), (NT(356)+NT(366)));
//G605: abs_memory(367)      => __E_memory_161(368).
	p(NT(367), (NT(368)));
//G606: memory(253)          => abs_memory(367).
	p(NT(253), (NT(367)));
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
		comment, __E_comment_92, __E_comment_93, __E_comment_94, __E____95, __E___96, cli, cli_command, __E_cli_97, __E_cli_98, 
		quit_cmd, quit_sym, __E_cli_command_99, version_cmd, version_sym, __E_cli_command_100, clear_cmd, clear_sym, __E_cli_command_101, help_cmd, 
		__E_cli_command_102, help_sym, __E___E_cli_command_102_103, __E___E_cli_command_102_104, help_arg, file_cmd, __E_cli_command_105, file_sym, q_string, valid_cmd, 
		__E_cli_command_106, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_107, sat_sym, unsat_cmd, __E_cli_command_108, unsat_sym, solve_cmd, 
		__E_cli_command_109, solve_sym, __E___E_cli_command_109_110, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_111, lgrs_sym, run_cmd, __E_cli_command_112, 
		run_sym, __E___E_cli_command_112_113, __E___E_cli_command_112_114, memory, normalize_cmd, __E_cli_command_115, normalize_sym, __E___E_cli_command_115_116, subst_cmd, __E_cli_command_117, 
		subst_sym, __E___E_cli_command_117_118, nf_cmd_arg, inst_cmd, __E_cli_command_119, inst_sym, __E___E_cli_command_119_120, inst_args, dnf_cmd, __E_cli_command_121, 
		dnf_sym, cnf_cmd, __E_cli_command_122, cnf_sym, anf_cmd, __E_cli_command_123, anf_sym, nnf_cmd, __E_cli_command_124, nnf_sym, 
		pnf_cmd, __E_cli_command_125, pnf_sym, mnf_cmd, __E_cli_command_126, mnf_sym, snf_cmd, __E_cli_command_127, snf_sym, onf_cmd, 
		__E_cli_command_128, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_129, def_sym, __E___E_cli_command_129_130, def_print_cmd, __E_cli_command_131, number, 
		def_rr_cmd, qelim_cmd, __E_cli_command_132, qelim_sym, get_cmd, __E_cli_command_133, get_sym, __E___E_cli_command_133_134, option_name, set_cmd, 
		__E_cli_command_135, set_sym, __E___E_cli_command_135_136, option_value, enable_cmd, __E_cli_command_137, enable_sym, disable_cmd, __E_cli_command_138, disable_sym, 
		toggle_cmd, __E_cli_command_139, toggle_sym, def_input_cmd, __E_cli_command_140, input_stream, def_output_cmd, __E_cli_command_141, output_stream, history_list_cmd, 
		__E_cli_command_142, history_sym, __E___E_cli_command_142_143, history_print_cmd, __E_cli_command_144, history_store_cmd, __E_cli_command_145, __E_solve_options_146, solve_option, __E_solve_options_147, 
		__E_solve_option_148, solver_mode, __E_solve_option_149, solver_mode_minimum, __E_solver_mode_150, solver_mode_maximum, __E_solver_mode_151, __E_input_stream_152, q_file_name, console_sym, 
		__E_output_stream_153, file_name, __E_file_name_154, __E_option_name_155, __E_option_value_156, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, execute_sym, 
		examples_sym, __E_help_arg_157, __E___E_help_arg_157_158, rel_memory, __E_memory_159, __E___E_memory_159_160, memory_id, abs_memory, __E_memory_161, __N_0, 
		__N_1, __N_2, __N_3, __N_4, __N_5, __N_6, __N_7, __N_8, 
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
