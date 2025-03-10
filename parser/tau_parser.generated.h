// This file is generated from a file parser/tau.tgf by
//       https://github.com/IDNI/parser/tools/tgf
//
#ifndef __TAU_PARSER_H__
#define __TAU_PARSER_H__

#include "parser.h"

namespace tau_parser_data {

using char_type     = char;
using terminal_type = char;

inline std::vector<std::string> symbol_names{
	"", "eof", "space", "digit", "xdigit", "alpha", "alnum", "punct", "printable", "start", 
	"rr", "_", "rec_relations", "main", "wff", "__E_rec_relations_0", "rec_relation", "__E_rec_relations_1", "ref", "__E_rec_relation_2", 
	"capture", "bf", "sym", "__E_ref_3", "offsets", "ref_args", "__E_ref_4", "__", "fp_fallback", "first_sym", 
	"last_sym", "__E_ref_args_5", "__E___E_ref_args_5_6", "ref_arg", "__E___E_ref_args_5_7", "__E___E_ref_args_5_8", "library", "rules", "__E_library_9", "__E___E_library_9_10", 
	"rule", "__E___E___E_library_9_10_11", "wff_rule", "bf_rule", "__E___E_library_9_12", "wff_matcher", "wff_body", "bf_matcher", "bf_body", "builder", 
	"builder_head", "builder_body", "__E_builder_head_13", "__E_builder_head_14", "bf_builder_body", "__E_builder_body_15", "wff_builder_body", "__E_builder_body_16", "tau_constant_source", "__E_tau_constant_source_17", 
	"wff_parenthesis", "__E_wff_18", "wff_sometimes", "__E_wff_19", "__E___E_wff_19_20", "wff_always", "__E_wff_21", "__E___E_wff_21_22", "wff_conditional", "__E_wff_23", 
	"wff_all", "__E_wff_24", "q_vars", "wff_ex", "__E_wff_25", "wff_ref", "constraint", "wff_imply", "__E_wff_26", "wff_rimply", 
	"__E_wff_27", "wff_equiv", "__E_wff_28", "wff_or", "__E_wff_29", "wff_xor", "__E_wff_30", "wff_and", "__E_wff_31", "wff_neg", 
	"__E_wff_32", "wff_t", "wff_f", "bf_interval", "__E_wff_33", "bf_eq", "__E_wff_34", "bf_neq", "__E_wff_35", "bf_less_equal", 
	"__E_wff_36", "bf_nleq", "__E_wff_37", "bf_greater", "__E_wff_38", "bf_ngreater", "__E_wff_39", "bf_greater_equal", "__E_wff_40", "bf_ngeq", 
	"__E_wff_41", "bf_less", "__E_wff_42", "bf_nless", "__E_wff_43", "bf_parenthesis", "__E_bf_44", "bf_constant", "variable", "bf_splitter", 
	"__E_bf_45", "bf_ref", "bf_or", "__E_bf_46", "bf_xor", "__E_bf_47", "bf_t", "__E_bf_48", "__E___E_bf_48_49", "type", 
	"bf_f", "__E_bf_50", "__E___E_bf_50_51", "z3", "bf_and", "__E_bf_52", "__E___E_bf_52_53", "bf_neg", "__E_bf_54", "bf_neg_oprnd", 
	"__E___E_bf_54_55", "bf_and_nosep", "__E_bf_56", "bf_and_nosep_1st_oprnd", "__E___E_bf_56_57", "bf_and_nosep_2nd_oprnd", "__E___E_bf_56_58", "z3_parenthesis", "__E_z3_59", "bitvector_constant", 
	"z3_add", "__E_z3_60", "z3_sub", "__E_z3_61", "z3_mul", "__E_z3_62", "z3_div", "__E_z3_63", "z3_mod", "__E_z3_64", 
	"z3_shl", "__E_z3_65", "digits", "z3_shr", "__E_z3_66", "z3_lt", "__E_z3_67", "z3_le", "__E_z3_68", "z3_gt", 
	"__E_z3_69", "z3_ge", "__E_z3_70", "z3_eq", "__E_z3_71", "z3_ne", "__E_z3_72", "constant", "__E_bitvector_constant_73", "binding", 
	"__E_bitvector_constant_74", "bv_type", "__E_bv_type_75", "ctn_neq", "__E_constraint_76", "ctnvar", "num", "ctn_eq", "__E_constraint_77", "ctn_greater_equal", 
	"__E_constraint_78", "ctn_greater", "__E_constraint_79", "ctn_less_equal", "__E_constraint_80", "ctn_less", "__E_constraint_81", "__E_variable_82", "__E_variable_83", "__E_variable_84", 
	"__E_variable_85", "__E_variable_86", "io_var", "__E_variable_87", "in", "out", "uninterpreted_constant", "in_var_name", "__E_in_88", "__E___E_in_88_89", 
	"offset", "out_var_name", "__E_out_90", "__E___E_out_90_91", "__E___E___E_out_90_91_92", "chars", "q_var", "__E_q_vars_93", "__E_q_vars_94", "__E_offsets_95", 
	"__E_offsets_96", "integer", "shift", "__E_offset_97", "__E_shift_98", "__E___E_shift_98_99", "__E_integer_100", "uninter_const_name", "__E_uninterpreted_constant_101", "__E___E_uninterpreted_constant_101_102", 
	"__E_bf_constant_103", "__E_bf_constant_104", "source", "__E_binding_105", "src_c", "__E___E_binding_105_106", "__E___E___E_binding_105_106_107", "__E___E___E_binding_105_106_108", "__E_src_c_109", "__E_src_c_110", 
	"__E_chars_111", "__E_chars_112", "__E_digits_113", "comment", "__E_comment_114", "__E_comment_115", "__E_comment_116", "__E____117", "__E___118", "cli", 
	"cli_command", "__E_cli_119", "__E_cli_120", "quit_cmd", "quit_sym", "__E_cli_command_121", "version_cmd", "version_sym", "__E_cli_command_122", "clear_cmd", 
	"clear_sym", "__E_cli_command_123", "help_cmd", "__E_cli_command_124", "help_sym", "__E___E_cli_command_124_125", "__E___E_cli_command_124_126", "help_arg", "file_cmd", "__E_cli_command_127", 
	"file_sym", "q_string", "valid_cmd", "__E_cli_command_128", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_129", "sat_sym", "unsat_cmd", 
	"__E_cli_command_130", "unsat_sym", "solve_cmd", "__E_cli_command_131", "solve_sym", "__E___E_cli_command_131_132", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_133", 
	"lgrs_sym", "run_cmd", "__E_cli_command_134", "run_sym", "__E___E_cli_command_134_135", "__E___E_cli_command_134_136", "memory", "normalize_cmd", "__E_cli_command_137", "normalize_sym", 
	"__E___E_cli_command_137_138", "subst_cmd", "__E_cli_command_139", "subst_sym", "__E___E_cli_command_139_140", "nf_cmd_arg", "inst_cmd", "__E_cli_command_141", "inst_sym", "__E___E_cli_command_141_142", 
	"inst_args", "dnf_cmd", "__E_cli_command_143", "dnf_sym", "cnf_cmd", "__E_cli_command_144", "cnf_sym", "anf_cmd", "__E_cli_command_145", "anf_sym", 
	"nnf_cmd", "__E_cli_command_146", "nnf_sym", "pnf_cmd", "__E_cli_command_147", "pnf_sym", "mnf_cmd", "__E_cli_command_148", "mnf_sym", "snf_cmd", 
	"__E_cli_command_149", "snf_sym", "onf_cmd", "__E_cli_command_150", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_151", "def_sym", "__E___E_cli_command_151_152", 
	"def_print_cmd", "__E_cli_command_153", "number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_154", "qelim_sym", "get_cmd", "__E_cli_command_155", "get_sym", 
	"__E___E_cli_command_155_156", "option_name", "set_cmd", "__E_cli_command_157", "set_sym", "__E___E_cli_command_157_158", "option_value", "enable_cmd", "__E_cli_command_159", "enable_sym", 
	"disable_cmd", "__E_cli_command_160", "disable_sym", "toggle_cmd", "__E_cli_command_161", "toggle_sym", "def_input_cmd", "__E_cli_command_162", "input_stream", "def_output_cmd", 
	"__E_cli_command_163", "output_stream", "history_list_cmd", "__E_cli_command_164", "history_sym", "__E___E_cli_command_164_165", "history_print_cmd", "__E_cli_command_166", "history_store_cmd", "__E_cli_command_167", 
	"__E_solve_options_168", "solve_option", "__E_solve_options_169", "__E_solve_option_170", "solver_mode", "__E_solve_option_171", "solver_mode_minimum", "__E_solver_mode_172", "solver_mode_maximum", "__E_solver_mode_173", 
	"__E_input_stream_174", "q_file_name", "console_sym", "__E_output_stream_175", "file_name", "__E_file_name_176", "__E_option_name_177", "__E_option_value_178", "bf_cmd_arg", "abs_memory_sym", 
	"rel_memory_sym", "selection_sym", "execute_sym", "examples_sym", "__E_help_arg_179", "__E___E_help_arg_179_180", "rel_memory", "__E_memory_181", "__E___E_memory_181_182", "memory_id", 
	"abs_memory", "__E_memory_183", "named_binding", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", "__N_6", 
	"__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'f', 'a', 'l', 'b', 'c', 
	'k', 'i', 'r', 's', 't', ',', '(', ')', 'o', 'm', 
	'e', '<', '>', 'w', 'y', '[', ']', '?', 'x', '-', 
	'|', '^', '&', '!', 'T', 'F', 'S', '+', '1', '0', 
	'\'', '*', '/', '%', '{', '}', '_', 'u', '$', '\t', 
	'\n', '\r', '#', 'q', 'v', 'n', 'h', 'p', 'd', 'g', 
	'z', '"', 
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
			20, 22, 118, 129, 162, 185, 207, 211, 221, 227,
			232, 351, 356, 394, 412
		},
		.to_inline = {
			{ 14, 60, 14 },
			{ 21, 115, 21 },
			{ 133, 147, 133 },
			{ 139, 115, 21 },
			{ 141, 115, 134 },
			{ 143, 115, 21 },
			{ 145, 115, 21 },
			{ 215 },
			{ 216 },
			{ 234 },
			{ 275 },
			{ 287 },
			{ 305 },
			{ 310 },
			{ 335 },
			{ 398 }
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
//G0:   start(9)             => rr(10) _(11).
	p(NT(9), (NT(10)+NT(11)));
//G1:   main(13)             => wff(14).
	p(NT(13), (NT(14)));
//G2:   rr(10)               => rec_relations(12) _(11) main(13) _(11) '.'.
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
//G122: bf_less_equal(99)    => __E_wff_36(100).
	p(NT(99), (NT(100)));
//G123: wff(14)              => bf_less_equal(99).
	p(NT(14), (NT(99)));
//G124: __E_wff_37(102)      => bf(21) _(11) '!' '<' '=' _(11) bf(21).
	p(NT(102), (NT(21)+NT(11)+T(32)+T(20)+T(3)+NT(11)+NT(21)));
//G125: bf_nleq(101)         => __E_wff_37(102).
	p(NT(101), (NT(102)));
//G126: wff(14)              => bf_nleq(101).
	p(NT(14), (NT(101)));
//G127: __E_wff_38(104)      => bf(21) _(11) '>' _(11) bf(21).
	p(NT(104), (NT(21)+NT(11)+T(21)+NT(11)+NT(21)));
//G128: bf_greater(103)      => __E_wff_38(104).
	p(NT(103), (NT(104)));
//G129: wff(14)              => bf_greater(103).
	p(NT(14), (NT(103)));
//G130: __E_wff_39(106)      => bf(21) _(11) '!' '>' _(11) bf(21).
	p(NT(106), (NT(21)+NT(11)+T(32)+T(21)+NT(11)+NT(21)));
//G131: bf_ngreater(105)     => __E_wff_39(106).
	p(NT(105), (NT(106)));
//G132: wff(14)              => bf_ngreater(105).
	p(NT(14), (NT(105)));
//G133: __E_wff_40(108)      => bf(21) _(11) '>' '=' _(11) bf(21).
	p(NT(108), (NT(21)+NT(11)+T(21)+T(3)+NT(11)+NT(21)));
//G134: bf_greater_equal(107) => __E_wff_40(108).
	p(NT(107), (NT(108)));
//G135: wff(14)              => bf_greater_equal(107).
	p(NT(14), (NT(107)));
//G136: __E_wff_41(110)      => bf(21) _(11) '!' '>' '=' _(11) bf(21).
	p(NT(110), (NT(21)+NT(11)+T(32)+T(21)+T(3)+NT(11)+NT(21)));
//G137: bf_ngeq(109)         => __E_wff_41(110).
	p(NT(109), (NT(110)));
//G138: wff(14)              => bf_ngeq(109).
	p(NT(14), (NT(109)));
//G139: __E_wff_42(112)      => bf(21) _(11) '<' _(11) bf(21).
	p(NT(112), (NT(21)+NT(11)+T(20)+NT(11)+NT(21)));
//G140: bf_less(111)         => __E_wff_42(112).
	p(NT(111), (NT(112)));
//G141: wff(14)              => bf_less(111).
	p(NT(14), (NT(111)));
//G142: __E_wff_43(114)      => bf(21) _(11) '!' '<' _(11) bf(21).
	p(NT(114), (NT(21)+NT(11)+T(32)+T(20)+NT(11)+NT(21)));
//G143: bf_nless(113)        => __E_wff_43(114).
	p(NT(113), (NT(114)));
//G144: wff(14)              => bf_nless(113).
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
//G162: __E___E_bf_48_49(128) => _(11) ':' _(11) type(129).
	p(NT(128), (NT(11)+T(2)+NT(11)+NT(129)));
//G163: __E___E_bf_48_49(128) => null.
	p(NT(128), (nul));
//G164: __E_bf_48(127)       => '1' __E___E_bf_48_49(128).
	p(NT(127), (T(37)+NT(128)));
//G165: bf_t(126)            => __E_bf_48(127).
	p(NT(126), (NT(127)));
//G166: bf(21)               => bf_t(126).
	p(NT(21), (NT(126)));
//G167: __E___E_bf_50_51(132) => _(11) ':' _(11) type(129).
	p(NT(132), (NT(11)+T(2)+NT(11)+NT(129)));
//G168: __E___E_bf_50_51(132) => null.
	p(NT(132), (nul));
//G169: __E_bf_50(131)       => '0' __E___E_bf_50_51(132).
	p(NT(131), (T(38)+NT(132)));
//G170: bf_f(130)            => __E_bf_50(131).
	p(NT(130), (NT(131)));
//G171: bf(21)               => bf_f(130).
	p(NT(21), (NT(130)));
//G172: bf(21)               => z3(133).
	p(NT(21), (NT(133)));
//G173: __E___E_bf_52_53(136) => _(11) '&' _(11).
	p(NT(136), (NT(11)+T(31)+NT(11)));
//G174: __E___E_bf_52_53(136) => __(27).
	p(NT(136), (NT(27)));
//G175: __E_bf_52(135)       => bf(21) __E___E_bf_52_53(136) bf(21).
	p(NT(135), (NT(21)+NT(136)+NT(21)));
//G176: bf_and(134)          => __E_bf_52(135).
	p(NT(134), (NT(135)));
//G177: bf(21)               => bf_and(134).
	p(NT(21), (NT(134)));
//G178: __E___E_bf_54_55(140) => bf_parenthesis(115).
	p(NT(140), (NT(115)));
//G179: __E___E_bf_54_55(140) => bf_constant(117).
	p(NT(140), (NT(117)));
//G180: __E___E_bf_54_55(140) => variable(118).
	p(NT(140), (NT(118)));
//G181: __E___E_bf_54_55(140) => bf_splitter(119).
	p(NT(140), (NT(119)));
//G182: __E___E_bf_54_55(140) => bf_ref(121).
	p(NT(140), (NT(121)));
//G183: __E___E_bf_54_55(140) => bf_t(126).
	p(NT(140), (NT(126)));
//G184: __E___E_bf_54_55(140) => bf_f(130).
	p(NT(140), (NT(130)));
//G185: __E___E_bf_54_55(140) => bf_neg(137).
	p(NT(140), (NT(137)));
//G186: __E___E_bf_54_55(140) => capture(20).
	p(NT(140), (NT(20)));
//G187: bf_neg_oprnd(139)    => __E___E_bf_54_55(140).
	p(NT(139), (NT(140)));
//G188: __E_bf_54(138)       => bf_neg_oprnd(139) _(11) '\''.
	p(NT(138), (NT(139)+NT(11)+T(39)));
//G189: bf_neg(137)          => __E_bf_54(138).
	p(NT(137), (NT(138)));
//G190: bf(21)               => bf_neg(137).
	p(NT(21), (NT(137)));
//G191: __E___E_bf_56_57(144) => bf_parenthesis(115).
	p(NT(144), (NT(115)));
//G192: __E___E_bf_56_57(144) => variable(118).
	p(NT(144), (NT(118)));
//G193: __E___E_bf_56_57(144) => bf_splitter(119).
	p(NT(144), (NT(119)));
//G194: __E___E_bf_56_57(144) => bf_ref(121).
	p(NT(144), (NT(121)));
//G195: __E___E_bf_56_57(144) => bf_neg(137).
	p(NT(144), (NT(137)));
//G196: __E___E_bf_56_57(144) => bf_and_nosep(141).
	p(NT(144), (NT(141)));
//G197: __E___E_bf_56_57(144) => capture(20).
	p(NT(144), (NT(20)));
//G198: bf_and_nosep_1st_oprnd(143) => __E___E_bf_56_57(144).
	p(NT(143), (NT(144)));
//G199: __E___E_bf_56_58(146) => bf_parenthesis(115).
	p(NT(146), (NT(115)));
//G200: __E___E_bf_56_58(146) => bf_constant(117).
	p(NT(146), (NT(117)));
//G201: __E___E_bf_56_58(146) => variable(118).
	p(NT(146), (NT(118)));
//G202: __E___E_bf_56_58(146) => bf_splitter(119).
	p(NT(146), (NT(119)));
//G203: __E___E_bf_56_58(146) => bf_ref(121).
	p(NT(146), (NT(121)));
//G204: __E___E_bf_56_58(146) => bf_neg(137).
	p(NT(146), (NT(137)));
//G205: __E___E_bf_56_58(146) => capture(20).
	p(NT(146), (NT(20)));
//G206: bf_and_nosep_2nd_oprnd(145) => __E___E_bf_56_58(146).
	p(NT(145), (NT(146)));
//G207: __E_bf_56(142)       => bf_and_nosep_1st_oprnd(143) bf_and_nosep_2nd_oprnd(145).
	p(NT(142), (NT(143)+NT(145)));
//G208: bf_and_nosep(141)    => __E_bf_56(142).
	p(NT(141), (NT(142)));
//G209: bf(21)               => bf_and_nosep(141).
	p(NT(21), (NT(141)));
//G210: bf(21)               => capture(20).
	p(NT(21), (NT(20)));
//G211: __E_z3_59(148)       => '(' _(11) z3(133) _(11) ')'.
	p(NT(148), (T(15)+NT(11)+NT(133)+NT(11)+T(16)));
//G212: z3_parenthesis(147)  => __E_z3_59(148).
	p(NT(147), (NT(148)));
//G213: z3(133)              => z3_parenthesis(147).
	p(NT(133), (NT(147)));
//G214: z3(133)              => bitvector_constant(149).
	p(NT(133), (NT(149)));
//G215: __E_z3_60(151)       => z3(133) _(11) ':' '+' _(11) z3(133).
	p(NT(151), (NT(133)+NT(11)+T(2)+T(36)+NT(11)+NT(133)));
//G216: z3_add(150)          => __E_z3_60(151).
	p(NT(150), (NT(151)));
//G217: z3(133)              => z3_add(150).
	p(NT(133), (NT(150)));
//G218: __E_z3_61(153)       => z3(133) _(11) ':' '-' _(11) z3(133).
	p(NT(153), (NT(133)+NT(11)+T(2)+T(28)+NT(11)+NT(133)));
//G219: z3_sub(152)          => __E_z3_61(153).
	p(NT(152), (NT(153)));
//G220: z3(133)              => z3_sub(152).
	p(NT(133), (NT(152)));
//G221: __E_z3_62(155)       => z3(133) _(11) ':' '*' _(11) z3(133).
	p(NT(155), (NT(133)+NT(11)+T(2)+T(40)+NT(11)+NT(133)));
//G222: z3_mul(154)          => __E_z3_62(155).
	p(NT(154), (NT(155)));
//G223: z3(133)              => z3_mul(154).
	p(NT(133), (NT(154)));
//G224: __E_z3_63(157)       => z3(133) _(11) ':' '/' _(11) z3(133).
	p(NT(157), (NT(133)+NT(11)+T(2)+T(41)+NT(11)+NT(133)));
//G225: z3_div(156)          => __E_z3_63(157).
	p(NT(156), (NT(157)));
//G226: z3(133)              => z3_div(156).
	p(NT(133), (NT(156)));
//G227: __E_z3_64(159)       => z3(133) _(11) ':' '%' _(11) z3(133).
	p(NT(159), (NT(133)+NT(11)+T(2)+T(42)+NT(11)+NT(133)));
//G228: z3_mod(158)          => __E_z3_64(159).
	p(NT(158), (NT(159)));
//G229: z3(133)              => z3_mod(158).
	p(NT(133), (NT(158)));
//G230: __E_z3_65(161)       => z3(133) _(11) ':' '<' '<' _(11) digits(162).
	p(NT(161), (NT(133)+NT(11)+T(2)+T(20)+T(20)+NT(11)+NT(162)));
//G231: z3_shl(160)          => __E_z3_65(161).
	p(NT(160), (NT(161)));
//G232: z3(133)              => z3_shl(160).
	p(NT(133), (NT(160)));
//G233: __E_z3_66(164)       => z3(133) _(11) ':' '>' '>' _(11) digits(162).
	p(NT(164), (NT(133)+NT(11)+T(2)+T(21)+T(21)+NT(11)+NT(162)));
//G234: z3_shr(163)          => __E_z3_66(164).
	p(NT(163), (NT(164)));
//G235: z3(133)              => z3_shr(163).
	p(NT(133), (NT(163)));
//G236: __E_z3_67(166)       => z3(133) _(11) ':' '<' _(11) z3(133).
	p(NT(166), (NT(133)+NT(11)+T(2)+T(20)+NT(11)+NT(133)));
//G237: z3_lt(165)           => __E_z3_67(166).
	p(NT(165), (NT(166)));
//G238: z3(133)              => z3_lt(165).
	p(NT(133), (NT(165)));
//G239: __E_z3_68(168)       => z3(133) _(11) ':' '<' '=' _(11) z3(133).
	p(NT(168), (NT(133)+NT(11)+T(2)+T(20)+T(3)+NT(11)+NT(133)));
//G240: z3_le(167)           => __E_z3_68(168).
	p(NT(167), (NT(168)));
//G241: z3(133)              => z3_le(167).
	p(NT(133), (NT(167)));
//G242: __E_z3_69(170)       => z3(133) _(11) ':' '>' _(11) z3(133).
	p(NT(170), (NT(133)+NT(11)+T(2)+T(21)+NT(11)+NT(133)));
//G243: z3_gt(169)           => __E_z3_69(170).
	p(NT(169), (NT(170)));
//G244: z3(133)              => z3_gt(169).
	p(NT(133), (NT(169)));
//G245: __E_z3_70(172)       => z3(133) _(11) ':' '>' '=' _(11) z3(133).
	p(NT(172), (NT(133)+NT(11)+T(2)+T(21)+T(3)+NT(11)+NT(133)));
//G246: z3_ge(171)           => __E_z3_70(172).
	p(NT(171), (NT(172)));
//G247: z3(133)              => z3_ge(171).
	p(NT(133), (NT(171)));
//G248: __E_z3_71(174)       => z3(133) _(11) ':' '=' _(11) z3(133).
	p(NT(174), (NT(133)+NT(11)+T(2)+T(3)+NT(11)+NT(133)));
//G249: z3_eq(173)           => __E_z3_71(174).
	p(NT(173), (NT(174)));
//G250: z3(133)              => z3_eq(173).
	p(NT(133), (NT(173)));
//G251: __E_z3_72(176)       => z3(133) _(11) ':' '!' '=' _(11) z3(133).
	p(NT(176), (NT(133)+NT(11)+T(2)+T(32)+T(3)+NT(11)+NT(133)));
//G252: z3_ne(175)           => __E_z3_72(176).
	p(NT(175), (NT(176)));
//G253: z3(133)              => z3_ne(175).
	p(NT(133), (NT(175)));
//G254: __E_bitvector_constant_73(178) => binding(179).
	p(NT(178), (NT(179)));
//G255: constant(177)        => __E_bitvector_constant_73(178).
	p(NT(177), (NT(178)));
//G256: __E_bitvector_constant_74(180) => bv_type(181).
	p(NT(180), (NT(181)));
//G257: type(129)            => __E_bitvector_constant_74(180).
	p(NT(129), (NT(180)));
//G258: bitvector_constant(149) => '{' _(11) constant(177) _(11) '}' _(11) ':' _(11) type(129).
	p(NT(149), (T(43)+NT(11)+NT(177)+NT(11)+T(44)+NT(11)+T(2)+NT(11)+NT(129)));
//G259: __E_bv_type_75(182)  => digits(162).
	p(NT(182), (NT(162)));
//G260: __E_bv_type_75(182)  => null.
	p(NT(182), (nul));
//G261: bv_type(181)         => 'b' 'i' 't' _(11) '[' _(11) __E_bv_type_75(182) _(11) ']'.
	p(NT(181), (T(7)+T(10)+T(13)+NT(11)+T(24)+NT(11)+NT(182)+NT(11)+T(25)));
//G262: __E_constraint_76(184) => '[' ctnvar(185) _(11) '!' '=' _(11) num(186) ']'.
	p(NT(184), (T(24)+NT(185)+NT(11)+T(32)+T(3)+NT(11)+NT(186)+T(25)));
//G263: __E_constraint_76(184) => '[' num(186) _(11) '!' '=' _(11) ctnvar(185) ']'.
	p(NT(184), (T(24)+NT(186)+NT(11)+T(32)+T(3)+NT(11)+NT(185)+T(25)));
//G264: ctn_neq(183)         => __E_constraint_76(184).
	p(NT(183), (NT(184)));
//G265: constraint(76)       => ctn_neq(183).
	p(NT(76), (NT(183)));
//G266: __E_constraint_77(188) => '[' ctnvar(185) _(11) '=' _(11) num(186) ']'.
	p(NT(188), (T(24)+NT(185)+NT(11)+T(3)+NT(11)+NT(186)+T(25)));
//G267: __E_constraint_77(188) => '[' num(186) _(11) '=' _(11) ctnvar(185) ']'.
	p(NT(188), (T(24)+NT(186)+NT(11)+T(3)+NT(11)+NT(185)+T(25)));
//G268: ctn_eq(187)          => __E_constraint_77(188).
	p(NT(187), (NT(188)));
//G269: constraint(76)       => ctn_eq(187).
	p(NT(76), (NT(187)));
//G270: __E_constraint_78(190) => '[' ctnvar(185) _(11) '>' '=' _(11) num(186) ']'.
	p(NT(190), (T(24)+NT(185)+NT(11)+T(21)+T(3)+NT(11)+NT(186)+T(25)));
//G271: __E_constraint_78(190) => '[' num(186) _(11) '>' '=' _(11) ctnvar(185) ']'.
	p(NT(190), (T(24)+NT(186)+NT(11)+T(21)+T(3)+NT(11)+NT(185)+T(25)));
//G272: ctn_greater_equal(189) => __E_constraint_78(190).
	p(NT(189), (NT(190)));
//G273: constraint(76)       => ctn_greater_equal(189).
	p(NT(76), (NT(189)));
//G274: __E_constraint_79(192) => '[' ctnvar(185) _(11) '>' _(11) num(186) ']'.
	p(NT(192), (T(24)+NT(185)+NT(11)+T(21)+NT(11)+NT(186)+T(25)));
//G275: __E_constraint_79(192) => '[' num(186) _(11) '>' _(11) ctnvar(185) ']'.
	p(NT(192), (T(24)+NT(186)+NT(11)+T(21)+NT(11)+NT(185)+T(25)));
//G276: ctn_greater(191)     => __E_constraint_79(192).
	p(NT(191), (NT(192)));
//G277: constraint(76)       => ctn_greater(191).
	p(NT(76), (NT(191)));
//G278: __E_constraint_80(194) => '[' ctnvar(185) _(11) '<' '=' _(11) num(186) ']'.
	p(NT(194), (T(24)+NT(185)+NT(11)+T(20)+T(3)+NT(11)+NT(186)+T(25)));
//G279: __E_constraint_80(194) => '[' num(186) _(11) '<' '=' _(11) ctnvar(185) ']'.
	p(NT(194), (T(24)+NT(186)+NT(11)+T(20)+T(3)+NT(11)+NT(185)+T(25)));
//G280: ctn_less_equal(193)  => __E_constraint_80(194).
	p(NT(193), (NT(194)));
//G281: constraint(76)       => ctn_less_equal(193).
	p(NT(76), (NT(193)));
//G282: __E_constraint_81(196) => '[' ctnvar(185) _(11) '<' _(11) num(186) ']'.
	p(NT(196), (T(24)+NT(185)+NT(11)+T(20)+NT(11)+NT(186)+T(25)));
//G283: __E_constraint_81(196) => '[' num(186) _(11) '<' _(11) ctnvar(185) ']'.
	p(NT(196), (T(24)+NT(186)+NT(11)+T(20)+NT(11)+NT(185)+T(25)));
//G284: ctn_less(195)        => __E_constraint_81(196).
	p(NT(195), (NT(196)));
//G285: constraint(76)       => ctn_less(195).
	p(NT(76), (NT(195)));
//G286: __N_0(413)           => 'F'.
	p(NT(413), (T(34)));
//G287: __N_1(414)           => 'T'.
	p(NT(414), (T(33)));
//G288: __E_variable_82(197) => ~( __N_0(413) ) & ~( __N_1(414) ) & alpha(5).	 # conjunctive
	p(NT(197), ~(NT(413)) & ~(NT(414)) & (NT(5)));
//G289: __E_variable_83(198) => null.
	p(NT(198), (nul));
//G290: __E_variable_83(198) => digit(3) __E_variable_83(198).
	p(NT(198), (NT(3)+NT(198)));
//G291: variable(118)        => __E_variable_82(197) __E_variable_83(198).	 # guarded: charvar
	p(NT(118), (NT(197)+NT(198)));
	p.back().guard = "charvar";
//G292: __N_2(415)           => 'F'.
	p(NT(415), (T(34)));
//G293: __N_3(416)           => 'T'.
	p(NT(416), (T(33)));
//G294: __E_variable_84(199) => ~( __N_2(415) ) & ~( __N_3(416) ) & alpha(5).	 # conjunctive
	p(NT(199), ~(NT(415)) & ~(NT(416)) & (NT(5)));
//G295: __E_variable_85(200) => alnum(6).
	p(NT(200), (NT(6)));
//G296: __E_variable_85(200) => '_'.
	p(NT(200), (T(45)));
//G297: __E_variable_86(201) => null.
	p(NT(201), (nul));
//G298: __E_variable_86(201) => __E_variable_85(200) __E_variable_86(201).
	p(NT(201), (NT(200)+NT(201)));
//G299: variable(118)        => __E_variable_84(199) __E_variable_86(201).	 # guarded: var
	p(NT(118), (NT(199)+NT(201)));
	p.back().guard = "var";
//G300: __E_variable_87(203) => in(204).
	p(NT(203), (NT(204)));
//G301: __E_variable_87(203) => out(205).
	p(NT(203), (NT(205)));
//G302: io_var(202)          => __E_variable_87(203).
	p(NT(202), (NT(203)));
//G303: variable(118)        => io_var(202).
	p(NT(118), (NT(202)));
//G304: variable(118)        => uninterpreted_constant(206).
	p(NT(118), (NT(206)));
//G305: __E___E_in_88_89(209) => digit(3).
	p(NT(209), (NT(3)));
//G306: __E___E_in_88_89(209) => digit(3) __E___E_in_88_89(209).
	p(NT(209), (NT(3)+NT(209)));
//G307: __E_in_88(208)       => 'i' __E___E_in_88_89(209).
	p(NT(208), (T(10)+NT(209)));
//G308: in_var_name(207)     => __E_in_88(208).
	p(NT(207), (NT(208)));
//G309: in(204)              => in_var_name(207) '[' offset(210) ']'.
	p(NT(204), (NT(207)+T(24)+NT(210)+T(25)));
//G310: __E___E___E_out_90_91_92(214) => digit(3).
	p(NT(214), (NT(3)));
//G311: __E___E___E_out_90_91_92(214) => digit(3) __E___E___E_out_90_91_92(214).
	p(NT(214), (NT(3)+NT(214)));
//G312: __E___E_out_90_91(213) => 'o' __E___E___E_out_90_91_92(214).
	p(NT(213), (T(17)+NT(214)));
//G313: __E_out_90(212)      => __E___E_out_90_91(213).
	p(NT(212), (NT(213)));
//G314: __E_out_90(212)      => 'u'.
	p(NT(212), (T(46)));
//G315: out_var_name(211)    => __E_out_90(212).
	p(NT(211), (NT(212)));
//G316: out(205)             => out_var_name(211) '[' offset(210) ']'.
	p(NT(205), (NT(211)+T(24)+NT(210)+T(25)));
//G317: ctnvar(185)          => chars(215).
	p(NT(185), (NT(215)));
//G318: capture(20)          => '$' chars(215).
	p(NT(20), (T(47)+NT(215)));
//G319: __E_q_vars_93(217)   => _(11) ',' _(11) q_var(216).
	p(NT(217), (NT(11)+T(14)+NT(11)+NT(216)));
//G320: __E_q_vars_94(218)   => null.
	p(NT(218), (nul));
//G321: __E_q_vars_94(218)   => __E_q_vars_93(217) __E_q_vars_94(218).
	p(NT(218), (NT(217)+NT(218)));
//G322: q_vars(72)           => q_var(216) __E_q_vars_94(218).
	p(NT(72), (NT(216)+NT(218)));
//G323: q_var(216)           => capture(20).
	p(NT(216), (NT(20)));
//G324: __N_4(417)           => uninterpreted_constant(206).
	p(NT(417), (NT(206)));
//G325: q_var(216)           => variable(118) & ~( __N_4(417) ).	 # conjunctive
	p(NT(216), (NT(118)) & ~(NT(417)));
//G326: __E_offsets_95(219)  => _(11) ',' _(11) offset(210).
	p(NT(219), (NT(11)+T(14)+NT(11)+NT(210)));
//G327: __E_offsets_96(220)  => null.
	p(NT(220), (nul));
//G328: __E_offsets_96(220)  => __E_offsets_95(219) __E_offsets_96(220).
	p(NT(220), (NT(219)+NT(220)));
//G329: offsets(24)          => '[' _(11) offset(210) __E_offsets_96(220) _(11) ']'.
	p(NT(24), (T(24)+NT(11)+NT(210)+NT(220)+NT(11)+T(25)));
//G330: offset(210)          => integer(221).
	p(NT(210), (NT(221)));
//G331: offset(210)          => capture(20).
	p(NT(210), (NT(20)));
//G332: offset(210)          => shift(222).
	p(NT(210), (NT(222)));
//G333: __N_5(418)           => io_var(202).
	p(NT(418), (NT(202)));
//G334: __E_offset_97(223)   => variable(118) & ~( __N_5(418) ).	 # conjunctive
	p(NT(223), (NT(118)) & ~(NT(418)));
//G335: offset(210)          => __E_offset_97(223).
	p(NT(210), (NT(223)));
//G336: __E_shift_98(224)    => capture(20).
	p(NT(224), (NT(20)));
//G337: __N_6(419)           => io_var(202).
	p(NT(419), (NT(202)));
//G338: __E___E_shift_98_99(225) => variable(118) & ~( __N_6(419) ).	 # conjunctive
	p(NT(225), (NT(118)) & ~(NT(419)));
//G339: __E_shift_98(224)    => __E___E_shift_98_99(225).
	p(NT(224), (NT(225)));
//G340: shift(222)           => __E_shift_98(224) _(11) '-' _(11) num(186).
	p(NT(222), (NT(224)+NT(11)+T(28)+NT(11)+NT(186)));
//G341: num(186)             => digits(162).
	p(NT(186), (NT(162)));
//G342: __E_integer_100(226) => '-'.
	p(NT(226), (T(28)));
//G343: __E_integer_100(226) => null.
	p(NT(226), (nul));
//G344: integer(221)         => __E_integer_100(226) _(11) digits(162).
	p(NT(221), (NT(226)+NT(11)+NT(162)));
//G345: __E___E_uninterpreted_constant_101_102(229) => chars(215).
	p(NT(229), (NT(215)));
//G346: __E___E_uninterpreted_constant_101_102(229) => _(11).
	p(NT(229), (NT(11)));
//G347: __E_uninterpreted_constant_101(228) => __E___E_uninterpreted_constant_101_102(229) ':' chars(215).
	p(NT(228), (NT(229)+T(2)+NT(215)));
//G348: uninter_const_name(227) => __E_uninterpreted_constant_101(228).
	p(NT(227), (NT(228)));
//G349: uninterpreted_constant(206) => '<' uninter_const_name(227) _(11) '>'.
	p(NT(206), (T(20)+NT(227)+NT(11)+T(21)));
//G350: __E_bf_constant_103(230) => capture(20).
	p(NT(230), (NT(20)));
//G351: __E_bf_constant_103(230) => binding(179).
	p(NT(230), (NT(179)));
//G352: constant(177)        => __E_bf_constant_103(230).
	p(NT(177), (NT(230)));
//G353: __E_bf_constant_104(231) => _(11) ':' _(11) type(129).
	p(NT(231), (NT(11)+T(2)+NT(11)+NT(129)));
//G354: __E_bf_constant_104(231) => null.
	p(NT(231), (nul));
//G355: bf_constant(117)     => '{' _(11) constant(177) _(11) '}' __E_bf_constant_104(231).
	p(NT(117), (T(43)+NT(11)+NT(177)+NT(11)+T(44)+NT(231)));
//G356: __E___E___E_binding_105_106_107(236) => src_c(234).
	p(NT(236), (NT(234)));
//G357: __E___E___E_binding_105_106_107(236) => space(2).
	p(NT(236), (NT(2)));
//G358: __E___E___E_binding_105_106_108(237) => null.
	p(NT(237), (nul));
//G359: __E___E___E_binding_105_106_108(237) => __E___E___E_binding_105_106_107(236) __E___E___E_binding_105_106_108(237).
	p(NT(237), (NT(236)+NT(237)));
//G360: __E___E_binding_105_106(235) => __E___E___E_binding_105_106_108(237) src_c(234).
	p(NT(235), (NT(237)+NT(234)));
//G361: __E___E_binding_105_106(235) => null.
	p(NT(235), (nul));
//G362: __E_binding_105(233) => src_c(234) __E___E_binding_105_106(235).
	p(NT(233), (NT(234)+NT(235)));
//G363: source(232)          => __E_binding_105(233).
	p(NT(232), (NT(233)));
//G364: binding(179)         => source(232).
	p(NT(179), (NT(232)));
//G365: src_c(234)           => alnum(6).
	p(NT(234), (NT(6)));
//G366: __N_7(420)           => '{'.
	p(NT(420), (T(43)));
//G367: __N_8(421)           => '}'.
	p(NT(421), (T(44)));
//G368: src_c(234)           => ~( __N_7(420) ) & ~( __N_8(421) ) & punct(7).	 # conjunctive
	p(NT(234), ~(NT(420)) & ~(NT(421)) & (NT(7)));
//G369: __E_src_c_109(238)   => src_c(234).
	p(NT(238), (NT(234)));
//G370: __E_src_c_109(238)   => space(2).
	p(NT(238), (NT(2)));
//G371: __E_src_c_110(239)   => null.
	p(NT(239), (nul));
//G372: __E_src_c_110(239)   => __E_src_c_109(238) __E_src_c_110(239).
	p(NT(239), (NT(238)+NT(239)));
//G373: src_c(234)           => '{' __E_src_c_110(239) '}'.
	p(NT(234), (T(43)+NT(239)+T(44)));
//G374: type(129)            => chars(215).
	p(NT(129), (NT(215)));
//G375: type(129)            => chars(215) _(11) '[' _(11) num(186) _(11) ']'.
	p(NT(129), (NT(215)+NT(11)+T(24)+NT(11)+NT(186)+NT(11)+T(25)));
//G376: __E_chars_111(240)   => alnum(6).
	p(NT(240), (NT(6)));
//G377: __E_chars_111(240)   => '_'.
	p(NT(240), (T(45)));
//G378: __E_chars_112(241)   => null.
	p(NT(241), (nul));
//G379: __E_chars_112(241)   => __E_chars_111(240) __E_chars_112(241).
	p(NT(241), (NT(240)+NT(241)));
//G380: chars(215)           => alpha(5) __E_chars_112(241).
	p(NT(215), (NT(5)+NT(241)));
//G381: __E_digits_113(242)  => digit(3).
	p(NT(242), (NT(3)));
//G382: __E_digits_113(242)  => digit(3) __E_digits_113(242).
	p(NT(242), (NT(3)+NT(242)));
//G383: digits(162)          => __E_digits_113(242).
	p(NT(162), (NT(242)));
//G384: sym(22)              => chars(215).
	p(NT(22), (NT(215)));
//G385: __E_comment_114(244) => printable(8).
	p(NT(244), (NT(8)));
//G386: __E_comment_114(244) => '\t'.
	p(NT(244), (T(48)));
//G387: __E_comment_115(245) => null.
	p(NT(245), (nul));
//G388: __E_comment_115(245) => __E_comment_114(244) __E_comment_115(245).
	p(NT(245), (NT(244)+NT(245)));
//G389: __E_comment_116(246) => '\n'.
	p(NT(246), (T(49)));
//G390: __E_comment_116(246) => '\r'.
	p(NT(246), (T(50)));
//G391: __E_comment_116(246) => eof(1).
	p(NT(246), (NT(1)));
//G392: comment(243)         => '#' __E_comment_115(245) __E_comment_116(246).
	p(NT(243), (T(51)+NT(245)+NT(246)));
//G393: __E____117(247)      => space(2).
	p(NT(247), (NT(2)));
//G394: __E____117(247)      => comment(243).
	p(NT(247), (NT(243)));
//G395: __(27)               => __E____117(247) _(11).
	p(NT(27), (NT(247)+NT(11)));
//G396: __E___118(248)       => __(27).
	p(NT(248), (NT(27)));
//G397: __E___118(248)       => null.
	p(NT(248), (nul));
//G398: _(11)                => __E___118(248).
	p(NT(11), (NT(248)));
//G399: cli(249)             => _(11).
	p(NT(249), (NT(11)));
//G400: __E_cli_119(251)     => '.' _(11) cli_command(250) _(11).
	p(NT(251), (T(1)+NT(11)+NT(250)+NT(11)));
//G401: __E_cli_120(252)     => null.
	p(NT(252), (nul));
//G402: __E_cli_120(252)     => __E_cli_119(251) __E_cli_120(252).
	p(NT(252), (NT(251)+NT(252)));
//G403: cli(249)             => _(11) cli_command(250) _(11) __E_cli_120(252).
	p(NT(249), (NT(11)+NT(250)+NT(11)+NT(252)));
//G404: __E_cli_command_121(255) => 'q'.
	p(NT(255), (T(52)));
//G405: __E_cli_command_121(255) => 'q' 'u' 'i' 't'.
	p(NT(255), (T(52)+T(46)+T(10)+T(13)));
//G406: quit_sym(254)        => __E_cli_command_121(255).
	p(NT(254), (NT(255)));
//G407: quit_cmd(253)        => quit_sym(254).
	p(NT(253), (NT(254)));
//G408: cli_command(250)     => quit_cmd(253).
	p(NT(250), (NT(253)));
//G409: __E_cli_command_122(258) => 'v'.
	p(NT(258), (T(53)));
//G410: __E_cli_command_122(258) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(258), (T(53)+T(19)+T(11)+T(12)+T(10)+T(17)+T(54)));
//G411: version_sym(257)     => __E_cli_command_122(258).
	p(NT(257), (NT(258)));
//G412: version_cmd(256)     => version_sym(257).
	p(NT(256), (NT(257)));
//G413: cli_command(250)     => version_cmd(256).
	p(NT(250), (NT(256)));
//G414: __E_cli_command_123(261) => 'c'.
	p(NT(261), (T(8)));
//G415: __E_cli_command_123(261) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(261), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G416: clear_sym(260)       => __E_cli_command_123(261).
	p(NT(260), (NT(261)));
//G417: clear_cmd(259)       => clear_sym(260).
	p(NT(259), (NT(260)));
//G418: cli_command(250)     => clear_cmd(259).
	p(NT(250), (NT(259)));
//G419: __E___E_cli_command_124_125(265) => 'h'.
	p(NT(265), (T(55)));
//G420: __E___E_cli_command_124_125(265) => 'h' 'e' 'l' 'p'.
	p(NT(265), (T(55)+T(19)+T(6)+T(56)));
//G421: help_sym(264)        => __E___E_cli_command_124_125(265).
	p(NT(264), (NT(265)));
//G422: __E___E_cli_command_124_126(266) => __(27) help_arg(267).
	p(NT(266), (NT(27)+NT(267)));
//G423: __E___E_cli_command_124_126(266) => null.
	p(NT(266), (nul));
//G424: __E_cli_command_124(263) => help_sym(264) __E___E_cli_command_124_126(266).
	p(NT(263), (NT(264)+NT(266)));
//G425: help_cmd(262)        => __E_cli_command_124(263).
	p(NT(262), (NT(263)));
//G426: cli_command(250)     => help_cmd(262).
	p(NT(250), (NT(262)));
//G427: file_sym(270)        => 'f' 'i' 'l' 'e'.
	p(NT(270), (T(4)+T(10)+T(6)+T(19)));
//G428: __E_cli_command_127(269) => file_sym(270) __(27) q_string(271).
	p(NT(269), (NT(270)+NT(27)+NT(271)));
//G429: file_cmd(268)        => __E_cli_command_127(269).
	p(NT(268), (NT(269)));
//G430: cli_command(250)     => file_cmd(268).
	p(NT(250), (NT(268)));
//G431: valid_sym(274)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(274), (T(53)+T(5)+T(6)+T(10)+T(57)));
//G432: __E_cli_command_128(273) => valid_sym(274) __(27) normalize_cmd_arg(275).
	p(NT(273), (NT(274)+NT(27)+NT(275)));
//G433: valid_cmd(272)       => __E_cli_command_128(273).
	p(NT(272), (NT(273)));
//G434: cli_command(250)     => valid_cmd(272).
	p(NT(250), (NT(272)));
//G435: sat_sym(278)         => 's' 'a' 't'.
	p(NT(278), (T(12)+T(5)+T(13)));
//G436: __E_cli_command_129(277) => sat_sym(278) __(27) normalize_cmd_arg(275).
	p(NT(277), (NT(278)+NT(27)+NT(275)));
//G437: sat_cmd(276)         => __E_cli_command_129(277).
	p(NT(276), (NT(277)));
//G438: cli_command(250)     => sat_cmd(276).
	p(NT(250), (NT(276)));
//G439: unsat_sym(281)       => 'u' 'n' 's' 'a' 't'.
	p(NT(281), (T(46)+T(54)+T(12)+T(5)+T(13)));
//G440: __E_cli_command_130(280) => unsat_sym(281) __(27) normalize_cmd_arg(275).
	p(NT(280), (NT(281)+NT(27)+NT(275)));
//G441: unsat_cmd(279)       => __E_cli_command_130(280).
	p(NT(279), (NT(280)));
//G442: cli_command(250)     => unsat_cmd(279).
	p(NT(250), (NT(279)));
//G443: solve_sym(284)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(284), (T(12)+T(17)+T(6)+T(53)+T(19)));
//G444: __E___E_cli_command_131_132(285) => solve_options(286).
	p(NT(285), (NT(286)));
//G445: __E___E_cli_command_131_132(285) => null.
	p(NT(285), (nul));
//G446: __E_cli_command_131(283) => solve_sym(284) __E___E_cli_command_131_132(285) __(27) wff_cmd_arg(287).
	p(NT(283), (NT(284)+NT(285)+NT(27)+NT(287)));
//G447: solve_cmd(282)       => __E_cli_command_131(283).
	p(NT(282), (NT(283)));
//G448: cli_command(250)     => solve_cmd(282).
	p(NT(250), (NT(282)));
//G449: lgrs_sym(290)        => 'l' 'g' 'r' 's'.
	p(NT(290), (T(6)+T(58)+T(11)+T(12)));
//G450: __E_cli_command_133(289) => lgrs_sym(290) __(27) wff_cmd_arg(287).
	p(NT(289), (NT(290)+NT(27)+NT(287)));
//G451: lgrs_cmd(288)        => __E_cli_command_133(289).
	p(NT(288), (NT(289)));
//G452: cli_command(250)     => lgrs_cmd(288).
	p(NT(250), (NT(288)));
//G453: __E___E_cli_command_134_135(294) => 'r'.
	p(NT(294), (T(11)));
//G454: __E___E_cli_command_134_135(294) => 'r' 'u' 'n'.
	p(NT(294), (T(11)+T(46)+T(54)));
//G455: run_sym(293)         => __E___E_cli_command_134_135(294).
	p(NT(293), (NT(294)));
//G456: __E___E_cli_command_134_136(295) => memory(296).
	p(NT(295), (NT(296)));
//G457: __E___E_cli_command_134_136(295) => wff(14).
	p(NT(295), (NT(14)));
//G458: __E_cli_command_134(292) => run_sym(293) __(27) __E___E_cli_command_134_136(295).
	p(NT(292), (NT(293)+NT(27)+NT(295)));
//G459: run_cmd(291)         => __E_cli_command_134(292).
	p(NT(291), (NT(292)));
//G460: cli_command(250)     => run_cmd(291).
	p(NT(250), (NT(291)));
//G461: __E___E_cli_command_137_138(300) => 'n'.
	p(NT(300), (T(54)));
//G462: __E___E_cli_command_137_138(300) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(300), (T(54)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(59)+T(19)));
//G463: normalize_sym(299)   => __E___E_cli_command_137_138(300).
	p(NT(299), (NT(300)));
//G464: __E_cli_command_137(298) => normalize_sym(299) __(27) normalize_cmd_arg(275).
	p(NT(298), (NT(299)+NT(27)+NT(275)));
//G465: normalize_cmd(297)   => __E_cli_command_137(298).
	p(NT(297), (NT(298)));
//G466: cli_command(250)     => normalize_cmd(297).
	p(NT(250), (NT(297)));
//G467: __E___E_cli_command_139_140(304) => 's'.
	p(NT(304), (T(12)));
//G468: __E___E_cli_command_139_140(304) => 's' 'u' 'b' 's' 't'.
	p(NT(304), (T(12)+T(46)+T(7)+T(12)+T(13)));
//G469: __E___E_cli_command_139_140(304) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(304), (T(12)+T(46)+T(7)+T(12)+T(13)+T(10)+T(13)+T(46)+T(13)+T(19)));
//G470: subst_sym(303)       => __E___E_cli_command_139_140(304).
	p(NT(303), (NT(304)));
//G471: __E_cli_command_139(302) => subst_sym(303) __(27) nf_cmd_arg(305) _(11) '[' _(11) nf_cmd_arg(305) _(11) '/' _(11) nf_cmd_arg(305) _(11) ']'.
	p(NT(302), (NT(303)+NT(27)+NT(305)+NT(11)+T(24)+NT(11)+NT(305)+NT(11)+T(41)+NT(11)+NT(305)+NT(11)+T(25)));
//G472: subst_cmd(301)       => __E_cli_command_139(302).
	p(NT(301), (NT(302)));
//G473: cli_command(250)     => subst_cmd(301).
	p(NT(250), (NT(301)));
//G474: __E___E_cli_command_141_142(309) => 'i'.
	p(NT(309), (T(10)));
//G475: __E___E_cli_command_141_142(309) => 'i' 'n' 's' 't'.
	p(NT(309), (T(10)+T(54)+T(12)+T(13)));
//G476: __E___E_cli_command_141_142(309) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(309), (T(10)+T(54)+T(12)+T(13)+T(5)+T(54)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G477: inst_sym(308)        => __E___E_cli_command_141_142(309).
	p(NT(308), (NT(309)));
//G478: __E_cli_command_141(307) => inst_sym(308) __(27) inst_args(310).
	p(NT(307), (NT(308)+NT(27)+NT(310)));
//G479: inst_cmd(306)        => __E_cli_command_141(307).
	p(NT(306), (NT(307)));
//G480: cli_command(250)     => inst_cmd(306).
	p(NT(250), (NT(306)));
//G481: dnf_sym(313)         => 'd' 'n' 'f'.
	p(NT(313), (T(57)+T(54)+T(4)));
//G482: __E_cli_command_143(312) => dnf_sym(313) __(27) nf_cmd_arg(305).
	p(NT(312), (NT(313)+NT(27)+NT(305)));
//G483: dnf_cmd(311)         => __E_cli_command_143(312).
	p(NT(311), (NT(312)));
//G484: cli_command(250)     => dnf_cmd(311).
	p(NT(250), (NT(311)));
//G485: cnf_sym(316)         => 'c' 'n' 'f'.
	p(NT(316), (T(8)+T(54)+T(4)));
//G486: __E_cli_command_144(315) => cnf_sym(316) __(27) nf_cmd_arg(305).
	p(NT(315), (NT(316)+NT(27)+NT(305)));
//G487: cnf_cmd(314)         => __E_cli_command_144(315).
	p(NT(314), (NT(315)));
//G488: cli_command(250)     => cnf_cmd(314).
	p(NT(250), (NT(314)));
//G489: anf_sym(319)         => 'a' 'n' 'f'.
	p(NT(319), (T(5)+T(54)+T(4)));
//G490: __E_cli_command_145(318) => anf_sym(319) __(27) nf_cmd_arg(305).
	p(NT(318), (NT(319)+NT(27)+NT(305)));
//G491: anf_cmd(317)         => __E_cli_command_145(318).
	p(NT(317), (NT(318)));
//G492: cli_command(250)     => anf_cmd(317).
	p(NT(250), (NT(317)));
//G493: nnf_sym(322)         => 'n' 'n' 'f'.
	p(NT(322), (T(54)+T(54)+T(4)));
//G494: __E_cli_command_146(321) => nnf_sym(322) __(27) nf_cmd_arg(305).
	p(NT(321), (NT(322)+NT(27)+NT(305)));
//G495: nnf_cmd(320)         => __E_cli_command_146(321).
	p(NT(320), (NT(321)));
//G496: cli_command(250)     => nnf_cmd(320).
	p(NT(250), (NT(320)));
//G497: pnf_sym(325)         => 'p' 'n' 'f'.
	p(NT(325), (T(56)+T(54)+T(4)));
//G498: __E_cli_command_147(324) => pnf_sym(325) __(27) nf_cmd_arg(305).
	p(NT(324), (NT(325)+NT(27)+NT(305)));
//G499: pnf_cmd(323)         => __E_cli_command_147(324).
	p(NT(323), (NT(324)));
//G500: cli_command(250)     => pnf_cmd(323).
	p(NT(250), (NT(323)));
//G501: mnf_sym(328)         => 'm' 'n' 'f'.
	p(NT(328), (T(18)+T(54)+T(4)));
//G502: __E_cli_command_148(327) => mnf_sym(328) __(27) nf_cmd_arg(305).
	p(NT(327), (NT(328)+NT(27)+NT(305)));
//G503: mnf_cmd(326)         => __E_cli_command_148(327).
	p(NT(326), (NT(327)));
//G504: cli_command(250)     => mnf_cmd(326).
	p(NT(250), (NT(326)));
//G505: snf_sym(331)         => 's' 'n' 'f'.
	p(NT(331), (T(12)+T(54)+T(4)));
//G506: __E_cli_command_149(330) => snf_sym(331) __(27) nf_cmd_arg(305).
	p(NT(330), (NT(331)+NT(27)+NT(305)));
//G507: snf_cmd(329)         => __E_cli_command_149(330).
	p(NT(329), (NT(330)));
//G508: cli_command(250)     => snf_cmd(329).
	p(NT(250), (NT(329)));
//G509: onf_sym(334)         => 'o' 'n' 'f'.
	p(NT(334), (T(17)+T(54)+T(4)));
//G510: __E_cli_command_150(333) => onf_sym(334) __(27) variable(118) __(27) onf_cmd_arg(335).
	p(NT(333), (NT(334)+NT(27)+NT(118)+NT(27)+NT(335)));
//G511: onf_cmd(332)         => __E_cli_command_150(333).
	p(NT(332), (NT(333)));
//G512: cli_command(250)     => onf_cmd(332).
	p(NT(250), (NT(332)));
//G513: __E___E_cli_command_151_152(339) => 'd' 'e' 'f' 's'.
	p(NT(339), (T(57)+T(19)+T(4)+T(12)));
//G514: __E___E_cli_command_151_152(339) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(339), (T(57)+T(19)+T(4)+T(10)+T(54)+T(10)+T(13)+T(10)+T(17)+T(54)+T(12)));
//G515: def_sym(338)         => __E___E_cli_command_151_152(339).
	p(NT(338), (NT(339)));
//G516: __E_cli_command_151(337) => def_sym(338).
	p(NT(337), (NT(338)));
//G517: def_list_cmd(336)    => __E_cli_command_151(337).
	p(NT(336), (NT(337)));
//G518: cli_command(250)     => def_list_cmd(336).
	p(NT(250), (NT(336)));
//G519: __E_cli_command_153(341) => def_sym(338) __(27) number(342).
	p(NT(341), (NT(338)+NT(27)+NT(342)));
//G520: def_print_cmd(340)   => __E_cli_command_153(341).
	p(NT(340), (NT(341)));
//G521: cli_command(250)     => def_print_cmd(340).
	p(NT(250), (NT(340)));
//G522: def_rr_cmd(343)      => rec_relation(16).
	p(NT(343), (NT(16)));
//G523: cli_command(250)     => def_rr_cmd(343).
	p(NT(250), (NT(343)));
//G524: qelim_sym(346)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(346), (T(52)+T(19)+T(6)+T(10)+T(18)));
//G525: __E_cli_command_154(345) => qelim_sym(346) __(27) wff_cmd_arg(287).
	p(NT(345), (NT(346)+NT(27)+NT(287)));
//G526: qelim_cmd(344)       => __E_cli_command_154(345).
	p(NT(344), (NT(345)));
//G527: cli_command(250)     => qelim_cmd(344).
	p(NT(250), (NT(344)));
//G528: get_sym(349)         => 'g' 'e' 't'.
	p(NT(349), (T(58)+T(19)+T(13)));
//G529: __E___E_cli_command_155_156(350) => __(27) option_name(351).
	p(NT(350), (NT(27)+NT(351)));
//G530: __E___E_cli_command_155_156(350) => null.
	p(NT(350), (nul));
//G531: __E_cli_command_155(348) => get_sym(349) __E___E_cli_command_155_156(350).
	p(NT(348), (NT(349)+NT(350)));
//G532: get_cmd(347)         => __E_cli_command_155(348).
	p(NT(347), (NT(348)));
//G533: cli_command(250)     => get_cmd(347).
	p(NT(250), (NT(347)));
//G534: set_sym(354)         => 's' 'e' 't'.
	p(NT(354), (T(12)+T(19)+T(13)));
//G535: __E___E_cli_command_157_158(355) => __(27).
	p(NT(355), (NT(27)));
//G536: __E___E_cli_command_157_158(355) => _(11) '=' _(11).
	p(NT(355), (NT(11)+T(3)+NT(11)));
//G537: __E_cli_command_157(353) => set_sym(354) __(27) option_name(351) __E___E_cli_command_157_158(355) option_value(356).
	p(NT(353), (NT(354)+NT(27)+NT(351)+NT(355)+NT(356)));
//G538: set_cmd(352)         => __E_cli_command_157(353).
	p(NT(352), (NT(353)));
//G539: cli_command(250)     => set_cmd(352).
	p(NT(250), (NT(352)));
//G540: enable_sym(359)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(359), (T(19)+T(54)+T(5)+T(7)+T(6)+T(19)));
//G541: __E_cli_command_159(358) => enable_sym(359) __(27) option_name(351).
	p(NT(358), (NT(359)+NT(27)+NT(351)));
//G542: enable_cmd(357)      => __E_cli_command_159(358).
	p(NT(357), (NT(358)));
//G543: cli_command(250)     => enable_cmd(357).
	p(NT(250), (NT(357)));
//G544: disable_sym(362)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(362), (T(57)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G545: __E_cli_command_160(361) => disable_sym(362) __(27) option_name(351).
	p(NT(361), (NT(362)+NT(27)+NT(351)));
//G546: disable_cmd(360)     => __E_cli_command_160(361).
	p(NT(360), (NT(361)));
//G547: cli_command(250)     => disable_cmd(360).
	p(NT(250), (NT(360)));
//G548: toggle_sym(365)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(365), (T(13)+T(17)+T(58)+T(58)+T(6)+T(19)));
//G549: __E_cli_command_161(364) => toggle_sym(365) __(27) option_name(351).
	p(NT(364), (NT(365)+NT(27)+NT(351)));
//G550: toggle_cmd(363)      => __E_cli_command_161(364).
	p(NT(363), (NT(364)));
//G551: cli_command(250)     => toggle_cmd(363).
	p(NT(250), (NT(363)));
//G552: __E_cli_command_162(367) => type(129) __(27) in_var_name(207) _(11) '=' _(11) input_stream(368).
	p(NT(367), (NT(129)+NT(27)+NT(207)+NT(11)+T(3)+NT(11)+NT(368)));
//G553: def_input_cmd(366)   => __E_cli_command_162(367).
	p(NT(366), (NT(367)));
//G554: cli_command(250)     => def_input_cmd(366).
	p(NT(250), (NT(366)));
//G555: __E_cli_command_163(370) => type(129) __(27) out_var_name(211) _(11) '=' _(11) output_stream(371).
	p(NT(370), (NT(129)+NT(27)+NT(211)+NT(11)+T(3)+NT(11)+NT(371)));
//G556: def_output_cmd(369)  => __E_cli_command_163(370).
	p(NT(369), (NT(370)));
//G557: cli_command(250)     => def_output_cmd(369).
	p(NT(250), (NT(369)));
//G558: __E___E_cli_command_164_165(375) => 'h' 'i' 's' 't'.
	p(NT(375), (T(55)+T(10)+T(12)+T(13)));
//G559: __E___E_cli_command_164_165(375) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(375), (T(55)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G560: history_sym(374)     => __E___E_cli_command_164_165(375).
	p(NT(374), (NT(375)));
//G561: __E_cli_command_164(373) => history_sym(374).
	p(NT(373), (NT(374)));
//G562: history_list_cmd(372) => __E_cli_command_164(373).
	p(NT(372), (NT(373)));
//G563: cli_command(250)     => history_list_cmd(372).
	p(NT(250), (NT(372)));
//G564: __E_cli_command_166(377) => history_sym(374) __(27) memory(296).
	p(NT(377), (NT(374)+NT(27)+NT(296)));
//G565: history_print_cmd(376) => __E_cli_command_166(377).
	p(NT(376), (NT(377)));
//G566: cli_command(250)     => history_print_cmd(376).
	p(NT(250), (NT(376)));
//G567: __E_cli_command_167(379) => wff(14).
	p(NT(379), (NT(14)));
//G568: __E_cli_command_167(379) => bf(21).
	p(NT(379), (NT(21)));
//G569: history_store_cmd(378) => __E_cli_command_167(379).
	p(NT(378), (NT(379)));
//G570: cli_command(250)     => history_store_cmd(378).
	p(NT(250), (NT(378)));
//G571: __E_solve_options_168(380) => __(27) solve_option(381).
	p(NT(380), (NT(27)+NT(381)));
//G572: __E_solve_options_169(382) => null.
	p(NT(382), (nul));
//G573: __E_solve_options_169(382) => __E_solve_options_168(380) __E_solve_options_169(382).
	p(NT(382), (NT(380)+NT(382)));
//G574: solve_options(286)   => __E_solve_options_169(382).
	p(NT(286), (NT(382)));
//G575: __E_solve_option_170(383) => '-' '-' solver_mode(384).
	p(NT(383), (T(28)+T(28)+NT(384)));
//G576: solve_option(381)    => __E_solve_option_170(383).
	p(NT(381), (NT(383)));
//G577: __E_solve_option_171(385) => '-' '-' type(129).
	p(NT(385), (T(28)+T(28)+NT(129)));
//G578: solve_option(381)    => __E_solve_option_171(385).
	p(NT(381), (NT(385)));
//G579: __E_solver_mode_172(387) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(387), (T(18)+T(10)+T(54)+T(10)+T(18)+T(46)+T(18)));
//G580: __E_solver_mode_172(387) => 'm' 'i' 'n'.
	p(NT(387), (T(18)+T(10)+T(54)));
//G581: solver_mode_minimum(386) => __E_solver_mode_172(387).
	p(NT(386), (NT(387)));
//G582: solver_mode(384)     => solver_mode_minimum(386).
	p(NT(384), (NT(386)));
//G583: __E_solver_mode_173(389) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(389), (T(18)+T(5)+T(27)+T(10)+T(18)+T(46)+T(18)));
//G584: __E_solver_mode_173(389) => 'm' 'a' 'x'.
	p(NT(389), (T(18)+T(5)+T(27)));
//G585: solver_mode_maximum(388) => __E_solver_mode_173(389).
	p(NT(388), (NT(389)));
//G586: solver_mode(384)     => solver_mode_maximum(388).
	p(NT(384), (NT(388)));
//G587: __E_input_stream_174(390) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(391) _(11) ')'.
	p(NT(390), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(391)+NT(11)+T(16)));
//G588: input_stream(368)    => __E_input_stream_174(390).
	p(NT(368), (NT(390)));
//G589: console_sym(392)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(392), (T(8)+T(17)+T(54)+T(12)+T(17)+T(6)+T(19)));
//G590: input_stream(368)    => console_sym(392).
	p(NT(368), (NT(392)));
//G591: __E_output_stream_175(393) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(391) _(11) ')'.
	p(NT(393), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(391)+NT(11)+T(16)));
//G592: output_stream(371)   => __E_output_stream_175(393).
	p(NT(371), (NT(393)));
//G593: output_stream(371)   => console_sym(392).
	p(NT(371), (NT(392)));
//G594: q_file_name(391)     => '"' file_name(394) '"'.
	p(NT(391), (T(60)+NT(394)+T(60)));
//G595: __E_file_name_176(395) => printable(8).
	p(NT(395), (NT(8)));
//G596: __E_file_name_176(395) => printable(8) __E_file_name_176(395).
	p(NT(395), (NT(8)+NT(395)));
//G597: file_name(394)       => __E_file_name_176(395).
	p(NT(394), (NT(395)));
//G598: __E_option_name_177(396) => alnum(6).
	p(NT(396), (NT(6)));
//G599: __E_option_name_177(396) => alnum(6) __E_option_name_177(396).
	p(NT(396), (NT(6)+NT(396)));
//G600: option_name(351)     => __E_option_name_177(396).
	p(NT(351), (NT(396)));
//G601: __E_option_value_178(397) => alnum(6).
	p(NT(397), (NT(6)));
//G602: __E_option_value_178(397) => alnum(6) __E_option_value_178(397).
	p(NT(397), (NT(6)+NT(397)));
//G603: option_value(356)    => __E_option_value_178(397).
	p(NT(356), (NT(397)));
//G604: number(342)          => digits(162).
	p(NT(342), (NT(162)));
//G605: bf_cmd_arg(398)      => memory(296).
	p(NT(398), (NT(296)));
//G606: bf_cmd_arg(398)      => bf(21).
	p(NT(398), (NT(21)));
//G607: wff_cmd_arg(287)     => memory(296).
	p(NT(287), (NT(296)));
//G608: wff_cmd_arg(287)     => wff(14).
	p(NT(287), (NT(14)));
//G609: nf_cmd_arg(305)      => memory(296).
	p(NT(305), (NT(296)));
//G610: nf_cmd_arg(305)      => ref(18).
	p(NT(305), (NT(18)));
//G611: nf_cmd_arg(305)      => wff(14).
	p(NT(305), (NT(14)));
//G612: nf_cmd_arg(305)      => bf(21).
	p(NT(305), (NT(21)));
//G613: onf_cmd_arg(335)     => memory(296).
	p(NT(335), (NT(296)));
//G614: onf_cmd_arg(335)     => wff(14).
	p(NT(335), (NT(14)));
//G615: normalize_cmd_arg(275) => memory(296).
	p(NT(275), (NT(296)));
//G616: normalize_cmd_arg(275) => rr(10).
	p(NT(275), (NT(10)));
//G617: normalize_cmd_arg(275) => ref(18).
	p(NT(275), (NT(18)));
//G618: normalize_cmd_arg(275) => wff(14).
	p(NT(275), (NT(14)));
//G619: normalize_cmd_arg(275) => bf(21).
	p(NT(275), (NT(21)));
//G620: inst_args(310)       => wff_cmd_arg(287) _(11) '[' _(11) variable(118) _(11) '/' _(11) bf_cmd_arg(398) _(11) ']'.
	p(NT(310), (NT(287)+NT(11)+T(24)+NT(11)+NT(118)+NT(11)+T(41)+NT(11)+NT(398)+NT(11)+T(25)));
//G621: inst_args(310)       => bf_cmd_arg(398) _(11) '[' _(11) variable(118) _(11) '/' _(11) bf_cmd_arg(398) _(11) ']'.
	p(NT(310), (NT(398)+NT(11)+T(24)+NT(11)+NT(118)+NT(11)+T(41)+NT(11)+NT(398)+NT(11)+T(25)));
//G622: help_arg(267)        => help_sym(264).
	p(NT(267), (NT(264)));
//G623: help_arg(267)        => version_sym(257).
	p(NT(267), (NT(257)));
//G624: help_arg(267)        => quit_sym(254).
	p(NT(267), (NT(254)));
//G625: help_arg(267)        => clear_sym(260).
	p(NT(267), (NT(260)));
//G626: help_arg(267)        => get_sym(349).
	p(NT(267), (NT(349)));
//G627: help_arg(267)        => set_sym(354).
	p(NT(267), (NT(354)));
//G628: help_arg(267)        => enable_sym(359).
	p(NT(267), (NT(359)));
//G629: help_arg(267)        => disable_sym(362).
	p(NT(267), (NT(362)));
//G630: help_arg(267)        => toggle_sym(365).
	p(NT(267), (NT(365)));
//G631: help_arg(267)        => file_sym(270).
	p(NT(267), (NT(270)));
//G632: help_arg(267)        => history_sym(374).
	p(NT(267), (NT(374)));
//G633: help_arg(267)        => abs_memory_sym(399).
	p(NT(267), (NT(399)));
//G634: help_arg(267)        => rel_memory_sym(400).
	p(NT(267), (NT(400)));
//G635: help_arg(267)        => selection_sym(401).
	p(NT(267), (NT(401)));
//G636: help_arg(267)        => def_sym(338).
	p(NT(267), (NT(338)));
//G637: help_arg(267)        => inst_sym(308).
	p(NT(267), (NT(308)));
//G638: help_arg(267)        => subst_sym(303).
	p(NT(267), (NT(303)));
//G639: help_arg(267)        => normalize_sym(299).
	p(NT(267), (NT(299)));
//G640: help_arg(267)        => execute_sym(402).
	p(NT(267), (NT(402)));
//G641: help_arg(267)        => solve_sym(284).
	p(NT(267), (NT(284)));
//G642: help_arg(267)        => valid_sym(274).
	p(NT(267), (NT(274)));
//G643: help_arg(267)        => sat_sym(278).
	p(NT(267), (NT(278)));
//G644: help_arg(267)        => unsat_sym(281).
	p(NT(267), (NT(281)));
//G645: help_arg(267)        => run_sym(293).
	p(NT(267), (NT(293)));
//G646: help_arg(267)        => dnf_sym(313).
	p(NT(267), (NT(313)));
//G647: help_arg(267)        => cnf_sym(316).
	p(NT(267), (NT(316)));
//G648: help_arg(267)        => anf_sym(319).
	p(NT(267), (NT(319)));
//G649: help_arg(267)        => snf_sym(331).
	p(NT(267), (NT(331)));
//G650: help_arg(267)        => nnf_sym(322).
	p(NT(267), (NT(322)));
//G651: help_arg(267)        => pnf_sym(325).
	p(NT(267), (NT(325)));
//G652: help_arg(267)        => mnf_sym(328).
	p(NT(267), (NT(328)));
//G653: help_arg(267)        => onf_sym(334).
	p(NT(267), (NT(334)));
//G654: help_arg(267)        => qelim_sym(346).
	p(NT(267), (NT(346)));
//G655: __E___E_help_arg_179_180(405) => 's'.
	p(NT(405), (T(12)));
//G656: __E___E_help_arg_179_180(405) => null.
	p(NT(405), (nul));
//G657: __E_help_arg_179(404) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_179_180(405).
	p(NT(404), (T(19)+T(27)+T(5)+T(18)+T(56)+T(6)+T(19)+NT(405)));
//G658: examples_sym(403)    => __E_help_arg_179(404).
	p(NT(403), (NT(404)));
//G659: help_arg(267)        => examples_sym(403).
	p(NT(267), (NT(403)));
//G660: __E_memory_181(407)  => '%'.
	p(NT(407), (T(42)));
//G661: rel_memory_sym(400)  => '%' '-'.
	p(NT(400), (T(42)+T(28)));
//G662: memory_id(409)       => digits(162).
	p(NT(409), (NT(162)));
//G663: __E___E_memory_181_182(408) => memory_id(409).
	p(NT(408), (NT(409)));
//G664: __E___E_memory_181_182(408) => null.
	p(NT(408), (nul));
//G665: __E_memory_181(407)  => rel_memory_sym(400) __E___E_memory_181_182(408).
	p(NT(407), (NT(400)+NT(408)));
//G666: rel_memory(406)      => __E_memory_181(407).
	p(NT(406), (NT(407)));
//G667: memory(296)          => rel_memory(406).
	p(NT(296), (NT(406)));
//G668: abs_memory_sym(399)  => '%'.
	p(NT(399), (T(42)));
//G669: __E_memory_183(411)  => abs_memory_sym(399) memory_id(409).
	p(NT(411), (NT(399)+NT(409)));
//G670: abs_memory(410)      => __E_memory_183(411).
	p(NT(410), (NT(411)));
//G671: memory(296)          => abs_memory(410).
	p(NT(296), (NT(410)));
	#undef T
	#undef NT
	return loaded = true, p;
}

inline ::idni::grammar<char_type, terminal_type> grammar(
	nts, productions(), start_symbol, char_classes, grammar_options);

} // namespace tau_parser_data

struct tau_parser : public idni::parser<char, char> {
	enum nonterminal {
		nul, eof, space, digit, xdigit, alpha, alnum, punct, printable, start, 
		rr, _, rec_relations, main, wff, __E_rec_relations_0, rec_relation, __E_rec_relations_1, ref, __E_rec_relation_2, 
		capture, bf, sym, __E_ref_3, offsets, ref_args, __E_ref_4, __, fp_fallback, first_sym, 
		last_sym, __E_ref_args_5, __E___E_ref_args_5_6, ref_arg, __E___E_ref_args_5_7, __E___E_ref_args_5_8, library, rules, __E_library_9, __E___E_library_9_10, 
		rule, __E___E___E_library_9_10_11, wff_rule, bf_rule, __E___E_library_9_12, wff_matcher, wff_body, bf_matcher, bf_body, builder, 
		builder_head, builder_body, __E_builder_head_13, __E_builder_head_14, bf_builder_body, __E_builder_body_15, wff_builder_body, __E_builder_body_16, tau_constant_source, __E_tau_constant_source_17, 
		wff_parenthesis, __E_wff_18, wff_sometimes, __E_wff_19, __E___E_wff_19_20, wff_always, __E_wff_21, __E___E_wff_21_22, wff_conditional, __E_wff_23, 
		wff_all, __E_wff_24, q_vars, wff_ex, __E_wff_25, wff_ref, constraint, wff_imply, __E_wff_26, wff_rimply, 
		__E_wff_27, wff_equiv, __E_wff_28, wff_or, __E_wff_29, wff_xor, __E_wff_30, wff_and, __E_wff_31, wff_neg, 
		__E_wff_32, wff_t, wff_f, bf_interval, __E_wff_33, bf_eq, __E_wff_34, bf_neq, __E_wff_35, bf_less_equal, 
		__E_wff_36, bf_nleq, __E_wff_37, bf_greater, __E_wff_38, bf_ngreater, __E_wff_39, bf_greater_equal, __E_wff_40, bf_ngeq, 
		__E_wff_41, bf_less, __E_wff_42, bf_nless, __E_wff_43, bf_parenthesis, __E_bf_44, bf_constant, variable, bf_splitter, 
		__E_bf_45, bf_ref, bf_or, __E_bf_46, bf_xor, __E_bf_47, bf_t, __E_bf_48, __E___E_bf_48_49, type, 
		bf_f, __E_bf_50, __E___E_bf_50_51, z3, bf_and, __E_bf_52, __E___E_bf_52_53, bf_neg, __E_bf_54, bf_neg_oprnd, 
		__E___E_bf_54_55, bf_and_nosep, __E_bf_56, bf_and_nosep_1st_oprnd, __E___E_bf_56_57, bf_and_nosep_2nd_oprnd, __E___E_bf_56_58, z3_parenthesis, __E_z3_59, bitvector_constant, 
		z3_add, __E_z3_60, z3_sub, __E_z3_61, z3_mul, __E_z3_62, z3_div, __E_z3_63, z3_mod, __E_z3_64, 
		z3_shl, __E_z3_65, digits, z3_shr, __E_z3_66, z3_lt, __E_z3_67, z3_le, __E_z3_68, z3_gt, 
		__E_z3_69, z3_ge, __E_z3_70, z3_eq, __E_z3_71, z3_ne, __E_z3_72, constant, __E_bitvector_constant_73, binding, 
		__E_bitvector_constant_74, bv_type, __E_bv_type_75, ctn_neq, __E_constraint_76, ctnvar, num, ctn_eq, __E_constraint_77, ctn_greater_equal, 
		__E_constraint_78, ctn_greater, __E_constraint_79, ctn_less_equal, __E_constraint_80, ctn_less, __E_constraint_81, __E_variable_82, __E_variable_83, __E_variable_84, 
		__E_variable_85, __E_variable_86, io_var, __E_variable_87, in, out, uninterpreted_constant, in_var_name, __E_in_88, __E___E_in_88_89, 
		offset, out_var_name, __E_out_90, __E___E_out_90_91, __E___E___E_out_90_91_92, chars, q_var, __E_q_vars_93, __E_q_vars_94, __E_offsets_95, 
		__E_offsets_96, integer, shift, __E_offset_97, __E_shift_98, __E___E_shift_98_99, __E_integer_100, uninter_const_name, __E_uninterpreted_constant_101, __E___E_uninterpreted_constant_101_102, 
		__E_bf_constant_103, __E_bf_constant_104, source, __E_binding_105, src_c, __E___E_binding_105_106, __E___E___E_binding_105_106_107, __E___E___E_binding_105_106_108, __E_src_c_109, __E_src_c_110, 
		__E_chars_111, __E_chars_112, __E_digits_113, comment, __E_comment_114, __E_comment_115, __E_comment_116, __E____117, __E___118, cli, 
		cli_command, __E_cli_119, __E_cli_120, quit_cmd, quit_sym, __E_cli_command_121, version_cmd, version_sym, __E_cli_command_122, clear_cmd, 
		clear_sym, __E_cli_command_123, help_cmd, __E_cli_command_124, help_sym, __E___E_cli_command_124_125, __E___E_cli_command_124_126, help_arg, file_cmd, __E_cli_command_127, 
		file_sym, q_string, valid_cmd, __E_cli_command_128, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_129, sat_sym, unsat_cmd, 
		__E_cli_command_130, unsat_sym, solve_cmd, __E_cli_command_131, solve_sym, __E___E_cli_command_131_132, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_133, 
		lgrs_sym, run_cmd, __E_cli_command_134, run_sym, __E___E_cli_command_134_135, __E___E_cli_command_134_136, memory, normalize_cmd, __E_cli_command_137, normalize_sym, 
		__E___E_cli_command_137_138, subst_cmd, __E_cli_command_139, subst_sym, __E___E_cli_command_139_140, nf_cmd_arg, inst_cmd, __E_cli_command_141, inst_sym, __E___E_cli_command_141_142, 
		inst_args, dnf_cmd, __E_cli_command_143, dnf_sym, cnf_cmd, __E_cli_command_144, cnf_sym, anf_cmd, __E_cli_command_145, anf_sym, 
		nnf_cmd, __E_cli_command_146, nnf_sym, pnf_cmd, __E_cli_command_147, pnf_sym, mnf_cmd, __E_cli_command_148, mnf_sym, snf_cmd, 
		__E_cli_command_149, snf_sym, onf_cmd, __E_cli_command_150, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_151, def_sym, __E___E_cli_command_151_152, 
		def_print_cmd, __E_cli_command_153, number, def_rr_cmd, qelim_cmd, __E_cli_command_154, qelim_sym, get_cmd, __E_cli_command_155, get_sym, 
		__E___E_cli_command_155_156, option_name, set_cmd, __E_cli_command_157, set_sym, __E___E_cli_command_157_158, option_value, enable_cmd, __E_cli_command_159, enable_sym, 
		disable_cmd, __E_cli_command_160, disable_sym, toggle_cmd, __E_cli_command_161, toggle_sym, def_input_cmd, __E_cli_command_162, input_stream, def_output_cmd, 
		__E_cli_command_163, output_stream, history_list_cmd, __E_cli_command_164, history_sym, __E___E_cli_command_164_165, history_print_cmd, __E_cli_command_166, history_store_cmd, __E_cli_command_167, 
		__E_solve_options_168, solve_option, __E_solve_options_169, __E_solve_option_170, solver_mode, __E_solve_option_171, solver_mode_minimum, __E_solver_mode_172, solver_mode_maximum, __E_solver_mode_173, 
		__E_input_stream_174, q_file_name, console_sym, __E_output_stream_175, file_name, __E_file_name_176, __E_option_name_177, __E_option_value_178, bf_cmd_arg, abs_memory_sym, 
		rel_memory_sym, selection_sym, execute_sym, examples_sym, __E_help_arg_179, __E___E_help_arg_179_180, rel_memory, __E_memory_181, __E___E_memory_181_182, memory_id, 
		abs_memory, __E_memory_183, named_binding, __N_0, __N_1, __N_2, __N_3, __N_4, __N_5, __N_6, 
		__N_7, __N_8, 
	};
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
