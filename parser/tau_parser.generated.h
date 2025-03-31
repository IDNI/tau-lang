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
	"__E_wff_41", "bf_less", "__E_wff_42", "bf_nless", "__E_wff_43", "z3_eq", "__E_wff_44", "z3", "z3_neq", "__E_wff_45", 
	"z3_less_equal", "__E_wff_46", "z3_nleq", "__E_wff_47", "z3_greater", "__E_wff_48", "z3_ngreater", "__E_wff_49", "z3_greater_equal", "__E_wff_50", 
	"z3_ngeq", "__E_wff_51", "z3_less", "__E_wff_52", "z3_nless", "__E_wff_53", "bf_parenthesis", "__E_bf_54", "bf_constant", "variable", 
	"bf_splitter", "__E_bf_55", "bf_ref", "bf_or", "__E_bf_56", "bf_xor", "__E_bf_57", "bf_t", "__E_bf_58", "__E___E_bf_58_59", 
	"type", "bf_f", "__E_bf_60", "__E___E_bf_60_61", "bf_and", "__E_bf_62", "__E___E_bf_62_63", "bf_neg", "__E_bf_64", "bf_neg_oprnd", 
	"__E___E_bf_64_65", "bf_and_nosep", "__E_bf_66", "bf_and_nosep_1st_oprnd", "__E___E_bf_66_67", "bf_and_nosep_2nd_oprnd", "__E___E_bf_66_68", "z3_parenthesis", "__E_z3_69", "bitvector", 
	"z3_not", "z3_add", "__E_z3_70", "z3_sub", "__E_z3_71", "z3_mul", "__E_z3_72", "z3_div", "__E_z3_73", "z3_mod", 
	"__E_z3_74", "z3_and", "__E_z3_75", "z3_or", "__E_z3_76", "z3_xor", "__E_z3_77", "bits", "__E_bitvector_78", "bitvector_type", 
	"num", "__E_bitvector_79", "__E_bitvector_type_80", "bit", "__E_bits_81", "__E_bits_82", "ctn_neq", "__E_constraint_83", "ctnvar", "ctn_eq", 
	"__E_constraint_84", "ctn_greater_equal", "__E_constraint_85", "ctn_greater", "__E_constraint_86", "ctn_less_equal", "__E_constraint_87", "ctn_less", "__E_constraint_88", "__E_variable_89", 
	"__E_variable_90", "__E_variable_91", "chars", "__E_variable_92", "__E_variable_93", "__E_variable_94", "__E_variable_95", "io_var", "__E_variable_96", "in", 
	"out", "__E_variable_97", "uninterpreted_constant", "in_var_name", "__E_in_98", "__E___E_in_98_99", "offset", "out_var_name", "__E_out_100", "__E___E_out_100_101", 
	"__E___E___E_out_100_101_102", "q_var", "__E_q_vars_103", "__E_q_vars_104", "__E_q_var_105", "__E_offsets_106", "__E_offsets_107", "integer", "shift", "__E_offset_108", 
	"__E_shift_109", "__E___E_shift_109_110", "digits", "__E_integer_111", "uninter_const_name", "__E_uninterpreted_constant_112", "__E___E_uninterpreted_constant_112_113", "constant", "__E_bf_constant_114", "binding", 
	"__E_bf_constant_115", "source", "__E_binding_116", "src_c", "__E___E_binding_116_117", "__E___E___E_binding_116_117_118", "__E___E___E_binding_116_117_119", "__E_src_c_120", "__E_src_c_121", "__E_chars_122", 
	"__E_chars_123", "__E_digits_124", "comment", "__E_comment_125", "__E_comment_126", "__E_comment_127", "__E____128", "__E___129", "cli", "cli_command", 
	"__E_cli_130", "__E_cli_131", "quit_cmd", "quit_sym", "__E_cli_command_132", "version_cmd", "version_sym", "__E_cli_command_133", "clear_cmd", "clear_sym", 
	"__E_cli_command_134", "help_cmd", "__E_cli_command_135", "help_sym", "__E___E_cli_command_135_136", "__E___E_cli_command_135_137", "help_arg", "file_cmd", "__E_cli_command_138", "file_sym", 
	"q_string", "valid_cmd", "__E_cli_command_139", "valid_sym", "normalize_cmd_arg", "sat_cmd", "__E_cli_command_140", "sat_sym", "unsat_cmd", "__E_cli_command_141", 
	"unsat_sym", "solve_cmd", "__E_cli_command_142", "solve_sym", "__E___E_cli_command_142_143", "solve_options", "wff_cmd_arg", "lgrs_cmd", "__E_cli_command_144", "lgrs_sym", 
	"run_cmd", "__E_cli_command_145", "run_sym", "__E___E_cli_command_145_146", "__E___E_cli_command_145_147", "memory", "normalize_cmd", "__E_cli_command_148", "normalize_sym", "__E___E_cli_command_148_149", 
	"subst_cmd", "__E_cli_command_150", "subst_sym", "__E___E_cli_command_150_151", "nf_cmd_arg", "inst_cmd", "__E_cli_command_152", "inst_sym", "__E___E_cli_command_152_153", "inst_args", 
	"dnf_cmd", "__E_cli_command_154", "dnf_sym", "cnf_cmd", "__E_cli_command_155", "cnf_sym", "anf_cmd", "__E_cli_command_156", "anf_sym", "nnf_cmd", 
	"__E_cli_command_157", "nnf_sym", "pnf_cmd", "__E_cli_command_158", "pnf_sym", "mnf_cmd", "__E_cli_command_159", "mnf_sym", "snf_cmd", "__E_cli_command_160", 
	"snf_sym", "onf_cmd", "__E_cli_command_161", "onf_sym", "onf_cmd_arg", "def_list_cmd", "__E_cli_command_162", "def_sym", "__E___E_cli_command_162_163", "def_print_cmd", 
	"__E_cli_command_164", "number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_165", "qelim_sym", "get_cmd", "__E_cli_command_166", "get_sym", "__E___E_cli_command_166_167", 
	"option_name", "set_cmd", "__E_cli_command_168", "set_sym", "__E___E_cli_command_168_169", "option_value", "enable_cmd", "__E_cli_command_170", "enable_sym", "disable_cmd", 
	"__E_cli_command_171", "disable_sym", "toggle_cmd", "__E_cli_command_172", "toggle_sym", "def_input_cmd", "__E_cli_command_173", "input_stream", "def_output_cmd", "__E_cli_command_174", 
	"output_stream", "history_list_cmd", "__E_cli_command_175", "history_sym", "__E___E_cli_command_175_176", "history_print_cmd", "__E_cli_command_177", "history_store_cmd", "__E_cli_command_178", "__E_solve_options_179", 
	"solve_option", "__E_solve_options_180", "__E_solve_option_181", "solver_mode", "__E_solve_option_182", "solver_mode_minimum", "__E_solver_mode_183", "solver_mode_maximum", "__E_solver_mode_184", "__E_input_stream_185", 
	"q_file_name", "console_sym", "__E_output_stream_186", "file_name", "__E_file_name_187", "__E_option_name_188", "__E_option_value_189", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", 
	"selection_sym", "execute_sym", "examples_sym", "__E_help_arg_190", "__E___E_help_arg_190_191", "rel_memory", "__E_memory_192", "__E___E_memory_192_193", "memory_id", "abs_memory", 
	"__E_memory_194", "named_binding", "z3_bv", "z3_bv_parenthesis", "__N_0", "__N_1", "__N_2", "__N_3", "__N_4", "__N_5", 
	"__N_6", "__N_7", "__N_8", 
};

inline ::idni::nonterminals<char_type, terminal_type> nts{symbol_names};

inline std::vector<terminal_type> terminals{
	'\0', '.', ':', '=', 'f', 'a', 'l', 'b', 'c', 
	'k', 'i', 'r', 's', 't', ',', '(', ')', 'o', 'm', 
	'e', '<', '>', 'w', 'y', '[', ']', '?', 'x', '-', 
	'|', '^', '&', '!', 'T', 'F', '_', 'S', '+', '1', 
	'0', '\'', '~', '*', '/', '%', 'u', '$', '{', '}', 
	'\t', '\n', '\r', '#', 'q', 'v', 'n', 'h', 'p', 'd', 
	'g', 'z', '"', 
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
			20, 22, 139, 150, 198, 223, 227, 237, 242, 244,
			251, 370, 375, 413, 431
		},
		.to_inline = {
			{ 14, 60, 14 },
			{ 21, 136, 21 },
			{ 159, 136, 21 },
			{ 161, 136, 154 },
			{ 163, 136, 21 },
			{ 165, 136, 21 },
			{ 189 },
			{ 212 },
			{ 231 },
			{ 253 },
			{ 294 },
			{ 306 },
			{ 324 },
			{ 329 },
			{ 354 },
			{ 417 },
			{ 432, 433, 432 }
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
//G145: __E_wff_44(116)      => z3(117) _(11) '=' '_' _(11) z3(117).
	p(NT(116), (NT(117)+NT(11)+T(3)+T(35)+NT(11)+NT(117)));
//G146: z3_eq(115)           => __E_wff_44(116).
	p(NT(115), (NT(116)));
//G147: wff(14)              => z3_eq(115).
	p(NT(14), (NT(115)));
//G148: __E_wff_45(119)      => z3(117) _(11) '!' '=' '_' _(11) z3(117).
	p(NT(119), (NT(117)+NT(11)+T(32)+T(3)+T(35)+NT(11)+NT(117)));
//G149: z3_neq(118)          => __E_wff_45(119).
	p(NT(118), (NT(119)));
//G150: wff(14)              => z3_neq(118).
	p(NT(14), (NT(118)));
//G151: __E_wff_46(121)      => z3(117) _(11) '<' '=' '_' _(11) z3(117).
	p(NT(121), (NT(117)+NT(11)+T(20)+T(3)+T(35)+NT(11)+NT(117)));
//G152: z3_less_equal(120)   => __E_wff_46(121).
	p(NT(120), (NT(121)));
//G153: wff(14)              => z3_less_equal(120).
	p(NT(14), (NT(120)));
//G154: __E_wff_47(123)      => z3(117) _(11) '!' '<' '=' '_' _(11) z3(117).
	p(NT(123), (NT(117)+NT(11)+T(32)+T(20)+T(3)+T(35)+NT(11)+NT(117)));
//G155: z3_nleq(122)         => __E_wff_47(123).
	p(NT(122), (NT(123)));
//G156: wff(14)              => z3_nleq(122).
	p(NT(14), (NT(122)));
//G157: __E_wff_48(125)      => z3(117) _(11) '>' '_' _(11) z3(117).
	p(NT(125), (NT(117)+NT(11)+T(21)+T(35)+NT(11)+NT(117)));
//G158: z3_greater(124)      => __E_wff_48(125).
	p(NT(124), (NT(125)));
//G159: wff(14)              => z3_greater(124).
	p(NT(14), (NT(124)));
//G160: __E_wff_49(127)      => z3(117) _(11) '!' '>' '_' _(11) z3(117).
	p(NT(127), (NT(117)+NT(11)+T(32)+T(21)+T(35)+NT(11)+NT(117)));
//G161: z3_ngreater(126)     => __E_wff_49(127).
	p(NT(126), (NT(127)));
//G162: wff(14)              => z3_ngreater(126).
	p(NT(14), (NT(126)));
//G163: __E_wff_50(129)      => z3(117) _(11) '>' '=' '_' _(11) z3(117).
	p(NT(129), (NT(117)+NT(11)+T(21)+T(3)+T(35)+NT(11)+NT(117)));
//G164: z3_greater_equal(128) => __E_wff_50(129).
	p(NT(128), (NT(129)));
//G165: wff(14)              => z3_greater_equal(128).
	p(NT(14), (NT(128)));
//G166: __E_wff_51(131)      => z3(117) _(11) '!' '>' '=' '_' _(11) z3(117).
	p(NT(131), (NT(117)+NT(11)+T(32)+T(21)+T(3)+T(35)+NT(11)+NT(117)));
//G167: z3_ngeq(130)         => __E_wff_51(131).
	p(NT(130), (NT(131)));
//G168: wff(14)              => z3_ngeq(130).
	p(NT(14), (NT(130)));
//G169: __E_wff_52(133)      => z3(117) _(11) '<' '_' _(11) z3(117).
	p(NT(133), (NT(117)+NT(11)+T(20)+T(35)+NT(11)+NT(117)));
//G170: z3_less(132)         => __E_wff_52(133).
	p(NT(132), (NT(133)));
//G171: wff(14)              => z3_less(132).
	p(NT(14), (NT(132)));
//G172: __E_wff_53(135)      => z3(117) _(11) '!' '<' '_' _(11) z3(117).
	p(NT(135), (NT(117)+NT(11)+T(32)+T(20)+T(35)+NT(11)+NT(117)));
//G173: z3_nless(134)        => __E_wff_53(135).
	p(NT(134), (NT(135)));
//G174: wff(14)              => z3_nless(134).
	p(NT(14), (NT(134)));
//G175: wff(14)              => capture(20).
	p(NT(14), (NT(20)));
//G176: __E_bf_54(137)       => '(' _(11) bf(21) _(11) ')'.
	p(NT(137), (T(15)+NT(11)+NT(21)+NT(11)+T(16)));
//G177: bf_parenthesis(136)  => __E_bf_54(137).
	p(NT(136), (NT(137)));
//G178: bf(21)               => bf_parenthesis(136).
	p(NT(21), (NT(136)));
//G179: bf(21)               => bf_constant(138).
	p(NT(21), (NT(138)));
//G180: bf(21)               => variable(139).
	p(NT(21), (NT(139)));
//G181: __E_bf_55(141)       => 'S' _(11) '(' _(11) bf(21) _(11) ')'.
	p(NT(141), (T(36)+NT(11)+T(15)+NT(11)+NT(21)+NT(11)+T(16)));
//G182: bf_splitter(140)     => __E_bf_55(141).
	p(NT(140), (NT(141)));
//G183: bf(21)               => bf_splitter(140).
	p(NT(21), (NT(140)));
//G184: bf_ref(142)          => ref(18).
	p(NT(142), (NT(18)));
//G185: bf(21)               => bf_ref(142).
	p(NT(21), (NT(142)));
//G186: __E_bf_56(144)       => bf(21) _(11) '|' _(11) bf(21).
	p(NT(144), (NT(21)+NT(11)+T(29)+NT(11)+NT(21)));
//G187: bf_or(143)           => __E_bf_56(144).
	p(NT(143), (NT(144)));
//G188: bf(21)               => bf_or(143).
	p(NT(21), (NT(143)));
//G189: __E_bf_57(146)       => bf(21) _(11) '+' _(11) bf(21).
	p(NT(146), (NT(21)+NT(11)+T(37)+NT(11)+NT(21)));
//G190: bf_xor(145)          => __E_bf_57(146).
	p(NT(145), (NT(146)));
//G191: bf(21)               => bf_xor(145).
	p(NT(21), (NT(145)));
//G192: __E___E_bf_58_59(149) => _(11) ':' _(11) type(150).
	p(NT(149), (NT(11)+T(2)+NT(11)+NT(150)));
//G193: __E___E_bf_58_59(149) => null.
	p(NT(149), (nul));
//G194: __E_bf_58(148)       => '1' __E___E_bf_58_59(149).
	p(NT(148), (T(38)+NT(149)));
//G195: bf_t(147)            => __E_bf_58(148).
	p(NT(147), (NT(148)));
//G196: bf(21)               => bf_t(147).
	p(NT(21), (NT(147)));
//G197: __E___E_bf_60_61(153) => _(11) ':' _(11) type(150).
	p(NT(153), (NT(11)+T(2)+NT(11)+NT(150)));
//G198: __E___E_bf_60_61(153) => null.
	p(NT(153), (nul));
//G199: __E_bf_60(152)       => '0' __E___E_bf_60_61(153).
	p(NT(152), (T(39)+NT(153)));
//G200: bf_f(151)            => __E_bf_60(152).
	p(NT(151), (NT(152)));
//G201: bf(21)               => bf_f(151).
	p(NT(21), (NT(151)));
//G202: __E___E_bf_62_63(156) => _(11) '&' _(11).
	p(NT(156), (NT(11)+T(31)+NT(11)));
//G203: __E___E_bf_62_63(156) => __(27).
	p(NT(156), (NT(27)));
//G204: __E_bf_62(155)       => bf(21) __E___E_bf_62_63(156) bf(21).
	p(NT(155), (NT(21)+NT(156)+NT(21)));
//G205: bf_and(154)          => __E_bf_62(155).
	p(NT(154), (NT(155)));
//G206: bf(21)               => bf_and(154).
	p(NT(21), (NT(154)));
//G207: __E___E_bf_64_65(160) => bf_parenthesis(136).
	p(NT(160), (NT(136)));
//G208: __E___E_bf_64_65(160) => bf_constant(138).
	p(NT(160), (NT(138)));
//G209: __E___E_bf_64_65(160) => variable(139).
	p(NT(160), (NT(139)));
//G210: __E___E_bf_64_65(160) => bf_splitter(140).
	p(NT(160), (NT(140)));
//G211: __E___E_bf_64_65(160) => bf_ref(142).
	p(NT(160), (NT(142)));
//G212: __E___E_bf_64_65(160) => bf_t(147).
	p(NT(160), (NT(147)));
//G213: __E___E_bf_64_65(160) => bf_f(151).
	p(NT(160), (NT(151)));
//G214: __E___E_bf_64_65(160) => bf_neg(157).
	p(NT(160), (NT(157)));
//G215: __E___E_bf_64_65(160) => capture(20).
	p(NT(160), (NT(20)));
//G216: bf_neg_oprnd(159)    => __E___E_bf_64_65(160).
	p(NT(159), (NT(160)));
//G217: __E_bf_64(158)       => bf_neg_oprnd(159) _(11) '\''.
	p(NT(158), (NT(159)+NT(11)+T(40)));
//G218: bf_neg(157)          => __E_bf_64(158).
	p(NT(157), (NT(158)));
//G219: bf(21)               => bf_neg(157).
	p(NT(21), (NT(157)));
//G220: __E___E_bf_66_67(164) => bf_parenthesis(136).
	p(NT(164), (NT(136)));
//G221: __E___E_bf_66_67(164) => variable(139).
	p(NT(164), (NT(139)));
//G222: __E___E_bf_66_67(164) => bf_splitter(140).
	p(NT(164), (NT(140)));
//G223: __E___E_bf_66_67(164) => bf_ref(142).
	p(NT(164), (NT(142)));
//G224: __E___E_bf_66_67(164) => bf_neg(157).
	p(NT(164), (NT(157)));
//G225: __E___E_bf_66_67(164) => bf_and_nosep(161).
	p(NT(164), (NT(161)));
//G226: __E___E_bf_66_67(164) => capture(20).
	p(NT(164), (NT(20)));
//G227: bf_and_nosep_1st_oprnd(163) => __E___E_bf_66_67(164).
	p(NT(163), (NT(164)));
//G228: __E___E_bf_66_68(166) => bf_parenthesis(136).
	p(NT(166), (NT(136)));
//G229: __E___E_bf_66_68(166) => bf_constant(138).
	p(NT(166), (NT(138)));
//G230: __E___E_bf_66_68(166) => variable(139).
	p(NT(166), (NT(139)));
//G231: __E___E_bf_66_68(166) => bf_splitter(140).
	p(NT(166), (NT(140)));
//G232: __E___E_bf_66_68(166) => bf_ref(142).
	p(NT(166), (NT(142)));
//G233: __E___E_bf_66_68(166) => bf_neg(157).
	p(NT(166), (NT(157)));
//G234: __E___E_bf_66_68(166) => capture(20).
	p(NT(166), (NT(20)));
//G235: bf_and_nosep_2nd_oprnd(165) => __E___E_bf_66_68(166).
	p(NT(165), (NT(166)));
//G236: __E_bf_66(162)       => bf_and_nosep_1st_oprnd(163) bf_and_nosep_2nd_oprnd(165).
	p(NT(162), (NT(163)+NT(165)));
//G237: bf_and_nosep(161)    => __E_bf_66(162).
	p(NT(161), (NT(162)));
//G238: bf(21)               => bf_and_nosep(161).
	p(NT(21), (NT(161)));
//G239: bf(21)               => capture(20).
	p(NT(21), (NT(20)));
//G240: __E_z3_69(168)       => '(' _(11) z3(117) _(11) ')'.
	p(NT(168), (T(15)+NT(11)+NT(117)+NT(11)+T(16)));
//G241: z3_parenthesis(167)  => __E_z3_69(168).
	p(NT(167), (NT(168)));
//G242: z3(117)              => z3_parenthesis(167).
	p(NT(117), (NT(167)));
//G243: z3(117)              => bitvector(169).
	p(NT(117), (NT(169)));
//G244: z3(117)              => variable(139).
	p(NT(117), (NT(139)));
//G245: z3_not(170)          => z3(117).
	p(NT(170), (NT(117)));
//G246: z3(117)              => '~' _(11) z3_not(170).
	p(NT(117), (T(41)+NT(11)+NT(170)));
//G247: __E_z3_70(172)       => z3(117) _(11) '+' _(11) z3(117).
	p(NT(172), (NT(117)+NT(11)+T(37)+NT(11)+NT(117)));
//G248: z3_add(171)          => __E_z3_70(172).
	p(NT(171), (NT(172)));
//G249: z3(117)              => z3_add(171).
	p(NT(117), (NT(171)));
//G250: __E_z3_71(174)       => z3(117) _(11) '-' _(11) z3(117).
	p(NT(174), (NT(117)+NT(11)+T(28)+NT(11)+NT(117)));
//G251: z3_sub(173)          => __E_z3_71(174).
	p(NT(173), (NT(174)));
//G252: z3(117)              => z3_sub(173).
	p(NT(117), (NT(173)));
//G253: __E_z3_72(176)       => z3(117) _(11) '*' _(11) z3(117).
	p(NT(176), (NT(117)+NT(11)+T(42)+NT(11)+NT(117)));
//G254: z3_mul(175)          => __E_z3_72(176).
	p(NT(175), (NT(176)));
//G255: z3(117)              => z3_mul(175).
	p(NT(117), (NT(175)));
//G256: __E_z3_73(178)       => z3(117) _(11) '/' _(11) z3(117).
	p(NT(178), (NT(117)+NT(11)+T(43)+NT(11)+NT(117)));
//G257: z3_div(177)          => __E_z3_73(178).
	p(NT(177), (NT(178)));
//G258: z3(117)              => z3_div(177).
	p(NT(117), (NT(177)));
//G259: __E_z3_74(180)       => z3(117) _(11) '%' _(11) z3(117).
	p(NT(180), (NT(117)+NT(11)+T(44)+NT(11)+NT(117)));
//G260: z3_mod(179)          => __E_z3_74(180).
	p(NT(179), (NT(180)));
//G261: z3(117)              => z3_mod(179).
	p(NT(117), (NT(179)));
//G262: __E_z3_75(182)       => z3(117) _(11) '&' _(11) z3(117).
	p(NT(182), (NT(117)+NT(11)+T(31)+NT(11)+NT(117)));
//G263: z3_and(181)          => __E_z3_75(182).
	p(NT(181), (NT(182)));
//G264: z3(117)              => z3_and(181).
	p(NT(117), (NT(181)));
//G265: __E_z3_76(184)       => z3(117) _(11) '|' _(11) z3(117).
	p(NT(184), (NT(117)+NT(11)+T(29)+NT(11)+NT(117)));
//G266: z3_or(183)           => __E_z3_76(184).
	p(NT(183), (NT(184)));
//G267: z3(117)              => z3_or(183).
	p(NT(117), (NT(183)));
//G268: __E_z3_77(186)       => z3(117) _(11) '^' _(11) z3(117).
	p(NT(186), (NT(117)+NT(11)+T(30)+NT(11)+NT(117)));
//G269: z3_xor(185)          => __E_z3_77(186).
	p(NT(185), (NT(186)));
//G270: z3(117)              => z3_xor(185).
	p(NT(117), (NT(185)));
//G271: z3(117)              => bitvector(169).
	p(NT(117), (NT(169)));
//G272: __E_bitvector_78(188) => _(11) bitvector_type(189).
	p(NT(188), (NT(11)+NT(189)));
//G273: __E_bitvector_78(188) => null.
	p(NT(188), (nul));
//G274: bitvector(169)       => bits(187) __E_bitvector_78(188).
	p(NT(169), (NT(187)+NT(188)));
//G275: __E_bitvector_79(191) => _(11) bitvector_type(189).
	p(NT(191), (NT(11)+NT(189)));
//G276: __E_bitvector_79(191) => null.
	p(NT(191), (nul));
//G277: bitvector(169)       => num(190) __E_bitvector_79(191).
	p(NT(169), (NT(190)+NT(191)));
//G278: __E_bitvector_type_80(192) => _(11) ':' _(11) '[' _(11) num(190) _(11) ']'.
	p(NT(192), (NT(11)+T(2)+NT(11)+T(24)+NT(11)+NT(190)+NT(11)+T(25)));
//G279: type(150)            => __E_bitvector_type_80(192).
	p(NT(150), (NT(192)));
//G280: bitvector_type(189)  => type(150).
	p(NT(189), (NT(150)));
//G281: __E_bits_81(194)     => bit(193).
	p(NT(194), (NT(193)));
//G282: __E_bits_82(195)     => null.
	p(NT(195), (nul));
//G283: __E_bits_82(195)     => __E_bits_81(194) __E_bits_82(195).
	p(NT(195), (NT(194)+NT(195)));
//G284: bits(187)            => bit(193) __E_bits_82(195) 'b'.
	p(NT(187), (NT(193)+NT(195)+T(7)));
//G285: __E_constraint_83(197) => '[' ctnvar(198) _(11) '!' '=' _(11) num(190) ']'.
	p(NT(197), (T(24)+NT(198)+NT(11)+T(32)+T(3)+NT(11)+NT(190)+T(25)));
//G286: __E_constraint_83(197) => '[' num(190) _(11) '!' '=' _(11) ctnvar(198) ']'.
	p(NT(197), (T(24)+NT(190)+NT(11)+T(32)+T(3)+NT(11)+NT(198)+T(25)));
//G287: ctn_neq(196)         => __E_constraint_83(197).
	p(NT(196), (NT(197)));
//G288: constraint(76)       => ctn_neq(196).
	p(NT(76), (NT(196)));
//G289: __E_constraint_84(200) => '[' ctnvar(198) _(11) '=' _(11) num(190) ']'.
	p(NT(200), (T(24)+NT(198)+NT(11)+T(3)+NT(11)+NT(190)+T(25)));
//G290: __E_constraint_84(200) => '[' num(190) _(11) '=' _(11) ctnvar(198) ']'.
	p(NT(200), (T(24)+NT(190)+NT(11)+T(3)+NT(11)+NT(198)+T(25)));
//G291: ctn_eq(199)          => __E_constraint_84(200).
	p(NT(199), (NT(200)));
//G292: constraint(76)       => ctn_eq(199).
	p(NT(76), (NT(199)));
//G293: __E_constraint_85(202) => '[' ctnvar(198) _(11) '>' '=' _(11) num(190) ']'.
	p(NT(202), (T(24)+NT(198)+NT(11)+T(21)+T(3)+NT(11)+NT(190)+T(25)));
//G294: __E_constraint_85(202) => '[' num(190) _(11) '>' '=' _(11) ctnvar(198) ']'.
	p(NT(202), (T(24)+NT(190)+NT(11)+T(21)+T(3)+NT(11)+NT(198)+T(25)));
//G295: ctn_greater_equal(201) => __E_constraint_85(202).
	p(NT(201), (NT(202)));
//G296: constraint(76)       => ctn_greater_equal(201).
	p(NT(76), (NT(201)));
//G297: __E_constraint_86(204) => '[' ctnvar(198) _(11) '>' _(11) num(190) ']'.
	p(NT(204), (T(24)+NT(198)+NT(11)+T(21)+NT(11)+NT(190)+T(25)));
//G298: __E_constraint_86(204) => '[' num(190) _(11) '>' _(11) ctnvar(198) ']'.
	p(NT(204), (T(24)+NT(190)+NT(11)+T(21)+NT(11)+NT(198)+T(25)));
//G299: ctn_greater(203)     => __E_constraint_86(204).
	p(NT(203), (NT(204)));
//G300: constraint(76)       => ctn_greater(203).
	p(NT(76), (NT(203)));
//G301: __E_constraint_87(206) => '[' ctnvar(198) _(11) '<' '=' _(11) num(190) ']'.
	p(NT(206), (T(24)+NT(198)+NT(11)+T(20)+T(3)+NT(11)+NT(190)+T(25)));
//G302: __E_constraint_87(206) => '[' num(190) _(11) '<' '=' _(11) ctnvar(198) ']'.
	p(NT(206), (T(24)+NT(190)+NT(11)+T(20)+T(3)+NT(11)+NT(198)+T(25)));
//G303: ctn_less_equal(205)  => __E_constraint_87(206).
	p(NT(205), (NT(206)));
//G304: constraint(76)       => ctn_less_equal(205).
	p(NT(76), (NT(205)));
//G305: __E_constraint_88(208) => '[' ctnvar(198) _(11) '<' _(11) num(190) ']'.
	p(NT(208), (T(24)+NT(198)+NT(11)+T(20)+NT(11)+NT(190)+T(25)));
//G306: __E_constraint_88(208) => '[' num(190) _(11) '<' _(11) ctnvar(198) ']'.
	p(NT(208), (T(24)+NT(190)+NT(11)+T(20)+NT(11)+NT(198)+T(25)));
//G307: ctn_less(207)        => __E_constraint_88(208).
	p(NT(207), (NT(208)));
//G308: constraint(76)       => ctn_less(207).
	p(NT(76), (NT(207)));
//G309: __N_0(434)           => 'F'.
	p(NT(434), (T(34)));
//G310: __N_1(435)           => 'T'.
	p(NT(435), (T(33)));
//G311: __E_variable_89(209) => ~( __N_0(434) ) & ~( __N_1(435) ) & alpha(5).	 # conjunctive
	p(NT(209), ~(NT(434)) & ~(NT(435)) & (NT(5)));
//G312: __E_variable_90(210) => null.
	p(NT(210), (nul));
//G313: __E_variable_90(210) => digit(3) __E_variable_90(210).
	p(NT(210), (NT(3)+NT(210)));
//G314: __E_variable_91(211) => _(11) ':' _(11) chars(212).
	p(NT(211), (NT(11)+T(2)+NT(11)+NT(212)));
//G315: __E_variable_91(211) => null.
	p(NT(211), (nul));
//G316: variable(139)        => __E_variable_89(209) __E_variable_90(210) __E_variable_91(211).	 # guarded: charvar
	p(NT(139), (NT(209)+NT(210)+NT(211)));
	p.back().guard = "charvar";
//G317: __N_2(436)           => 'F'.
	p(NT(436), (T(34)));
//G318: __N_3(437)           => 'T'.
	p(NT(437), (T(33)));
//G319: __E_variable_92(213) => ~( __N_2(436) ) & ~( __N_3(437) ) & alpha(5).	 # conjunctive
	p(NT(213), ~(NT(436)) & ~(NT(437)) & (NT(5)));
//G320: __E_variable_93(214) => alnum(6).
	p(NT(214), (NT(6)));
//G321: __E_variable_93(214) => '_'.
	p(NT(214), (T(35)));
//G322: __E_variable_94(215) => null.
	p(NT(215), (nul));
//G323: __E_variable_94(215) => __E_variable_93(214) __E_variable_94(215).
	p(NT(215), (NT(214)+NT(215)));
//G324: __E_variable_95(216) => _(11) ':' _(11) chars(212).
	p(NT(216), (NT(11)+T(2)+NT(11)+NT(212)));
//G325: __E_variable_95(216) => null.
	p(NT(216), (nul));
//G326: variable(139)        => __E_variable_92(213) __E_variable_94(215) __E_variable_95(216).	 # guarded: var
	p(NT(139), (NT(213)+NT(215)+NT(216)));
	p.back().guard = "var";
//G327: __E_variable_96(218) => in(219).
	p(NT(218), (NT(219)));
//G328: __E_variable_96(218) => out(220).
	p(NT(218), (NT(220)));
//G329: io_var(217)          => __E_variable_96(218).
	p(NT(217), (NT(218)));
//G330: __E_variable_97(221) => _(11) ':' _(11) chars(212).
	p(NT(221), (NT(11)+T(2)+NT(11)+NT(212)));
//G331: __E_variable_97(221) => null.
	p(NT(221), (nul));
//G332: variable(139)        => io_var(217) __E_variable_97(221).
	p(NT(139), (NT(217)+NT(221)));
//G333: variable(139)        => uninterpreted_constant(222).
	p(NT(139), (NT(222)));
//G334: __E___E_in_98_99(225) => digit(3).
	p(NT(225), (NT(3)));
//G335: __E___E_in_98_99(225) => digit(3) __E___E_in_98_99(225).
	p(NT(225), (NT(3)+NT(225)));
//G336: __E_in_98(224)       => 'i' __E___E_in_98_99(225).
	p(NT(224), (T(10)+NT(225)));
//G337: in_var_name(223)     => __E_in_98(224).
	p(NT(223), (NT(224)));
//G338: in(219)              => in_var_name(223) '[' offset(226) ']'.
	p(NT(219), (NT(223)+T(24)+NT(226)+T(25)));
//G339: __E___E___E_out_100_101_102(230) => digit(3).
	p(NT(230), (NT(3)));
//G340: __E___E___E_out_100_101_102(230) => digit(3) __E___E___E_out_100_101_102(230).
	p(NT(230), (NT(3)+NT(230)));
//G341: __E___E_out_100_101(229) => 'o' __E___E___E_out_100_101_102(230).
	p(NT(229), (T(17)+NT(230)));
//G342: __E_out_100(228)     => __E___E_out_100_101(229).
	p(NT(228), (NT(229)));
//G343: __E_out_100(228)     => 'u'.
	p(NT(228), (T(45)));
//G344: out_var_name(227)    => __E_out_100(228).
	p(NT(227), (NT(228)));
//G345: out(220)             => out_var_name(227) '[' offset(226) ']'.
	p(NT(220), (NT(227)+T(24)+NT(226)+T(25)));
//G346: ctnvar(198)          => chars(212).
	p(NT(198), (NT(212)));
//G347: capture(20)          => '$' chars(212).
	p(NT(20), (T(46)+NT(212)));
//G348: __E_q_vars_103(232)  => _(11) ',' _(11) q_var(231).
	p(NT(232), (NT(11)+T(14)+NT(11)+NT(231)));
//G349: __E_q_vars_104(233)  => null.
	p(NT(233), (nul));
//G350: __E_q_vars_104(233)  => __E_q_vars_103(232) __E_q_vars_104(233).
	p(NT(233), (NT(232)+NT(233)));
//G351: q_vars(72)           => q_var(231) __E_q_vars_104(233).
	p(NT(72), (NT(231)+NT(233)));
//G352: q_var(231)           => capture(20).
	p(NT(231), (NT(20)));
//G353: __E_q_var_105(234)   => _(11) ':' _(11) chars(212).
	p(NT(234), (NT(11)+T(2)+NT(11)+NT(212)));
//G354: __E_q_var_105(234)   => null.
	p(NT(234), (nul));
//G355: __N_4(438)           => uninterpreted_constant(222) __E_q_var_105(234).
	p(NT(438), (NT(222)+NT(234)));
//G356: q_var(231)           => variable(139) & ~( __N_4(438) ).	 # conjunctive
	p(NT(231), (NT(139)) & ~(NT(438)));
//G357: __E_offsets_106(235) => _(11) ',' _(11) offset(226).
	p(NT(235), (NT(11)+T(14)+NT(11)+NT(226)));
//G358: __E_offsets_107(236) => null.
	p(NT(236), (nul));
//G359: __E_offsets_107(236) => __E_offsets_106(235) __E_offsets_107(236).
	p(NT(236), (NT(235)+NT(236)));
//G360: offsets(24)          => '[' _(11) offset(226) __E_offsets_107(236) _(11) ']'.
	p(NT(24), (T(24)+NT(11)+NT(226)+NT(236)+NT(11)+T(25)));
//G361: offset(226)          => integer(237).
	p(NT(226), (NT(237)));
//G362: offset(226)          => capture(20).
	p(NT(226), (NT(20)));
//G363: offset(226)          => shift(238).
	p(NT(226), (NT(238)));
//G364: __N_5(439)           => io_var(217).
	p(NT(439), (NT(217)));
//G365: __E_offset_108(239)  => variable(139) & ~( __N_5(439) ).	 # conjunctive
	p(NT(239), (NT(139)) & ~(NT(439)));
//G366: offset(226)          => __E_offset_108(239).
	p(NT(226), (NT(239)));
//G367: __E_shift_109(240)   => capture(20).
	p(NT(240), (NT(20)));
//G368: __N_6(440)           => io_var(217).
	p(NT(440), (NT(217)));
//G369: __E___E_shift_109_110(241) => variable(139) & ~( __N_6(440) ).	 # conjunctive
	p(NT(241), (NT(139)) & ~(NT(440)));
//G370: __E_shift_109(240)   => __E___E_shift_109_110(241).
	p(NT(240), (NT(241)));
//G371: shift(238)           => __E_shift_109(240) _(11) '-' _(11) num(190).
	p(NT(238), (NT(240)+NT(11)+T(28)+NT(11)+NT(190)));
//G372: num(190)             => digits(242).
	p(NT(190), (NT(242)));
//G373: __E_integer_111(243) => '-'.
	p(NT(243), (T(28)));
//G374: __E_integer_111(243) => null.
	p(NT(243), (nul));
//G375: integer(237)         => __E_integer_111(243) _(11) digits(242).
	p(NT(237), (NT(243)+NT(11)+NT(242)));
//G376: __E___E_uninterpreted_constant_112_113(246) => chars(212).
	p(NT(246), (NT(212)));
//G377: __E___E_uninterpreted_constant_112_113(246) => _(11).
	p(NT(246), (NT(11)));
//G378: __E_uninterpreted_constant_112(245) => __E___E_uninterpreted_constant_112_113(246) ':' chars(212).
	p(NT(245), (NT(246)+T(2)+NT(212)));
//G379: uninter_const_name(244) => __E_uninterpreted_constant_112(245).
	p(NT(244), (NT(245)));
//G380: uninterpreted_constant(222) => '<' uninter_const_name(244) _(11) '>'.
	p(NT(222), (T(20)+NT(244)+NT(11)+T(21)));
//G381: __E_bf_constant_114(248) => capture(20).
	p(NT(248), (NT(20)));
//G382: __E_bf_constant_114(248) => binding(249).
	p(NT(248), (NT(249)));
//G383: constant(247)        => __E_bf_constant_114(248).
	p(NT(247), (NT(248)));
//G384: __E_bf_constant_115(250) => _(11) ':' _(11) type(150).
	p(NT(250), (NT(11)+T(2)+NT(11)+NT(150)));
//G385: __E_bf_constant_115(250) => null.
	p(NT(250), (nul));
//G386: bf_constant(138)     => '{' _(11) constant(247) _(11) '}' __E_bf_constant_115(250).
	p(NT(138), (T(47)+NT(11)+NT(247)+NT(11)+T(48)+NT(250)));
//G387: __E___E___E_binding_116_117_118(255) => src_c(253).
	p(NT(255), (NT(253)));
//G388: __E___E___E_binding_116_117_118(255) => space(2).
	p(NT(255), (NT(2)));
//G389: __E___E___E_binding_116_117_119(256) => null.
	p(NT(256), (nul));
//G390: __E___E___E_binding_116_117_119(256) => __E___E___E_binding_116_117_118(255) __E___E___E_binding_116_117_119(256).
	p(NT(256), (NT(255)+NT(256)));
//G391: __E___E_binding_116_117(254) => __E___E___E_binding_116_117_119(256) src_c(253).
	p(NT(254), (NT(256)+NT(253)));
//G392: __E___E_binding_116_117(254) => null.
	p(NT(254), (nul));
//G393: __E_binding_116(252) => src_c(253) __E___E_binding_116_117(254).
	p(NT(252), (NT(253)+NT(254)));
//G394: source(251)          => __E_binding_116(252).
	p(NT(251), (NT(252)));
//G395: binding(249)         => source(251).
	p(NT(249), (NT(251)));
//G396: src_c(253)           => alnum(6).
	p(NT(253), (NT(6)));
//G397: __N_7(441)           => '{'.
	p(NT(441), (T(47)));
//G398: __N_8(442)           => '}'.
	p(NT(442), (T(48)));
//G399: src_c(253)           => ~( __N_7(441) ) & ~( __N_8(442) ) & punct(7).	 # conjunctive
	p(NT(253), ~(NT(441)) & ~(NT(442)) & (NT(7)));
//G400: __E_src_c_120(257)   => src_c(253).
	p(NT(257), (NT(253)));
//G401: __E_src_c_120(257)   => space(2).
	p(NT(257), (NT(2)));
//G402: __E_src_c_121(258)   => null.
	p(NT(258), (nul));
//G403: __E_src_c_121(258)   => __E_src_c_120(257) __E_src_c_121(258).
	p(NT(258), (NT(257)+NT(258)));
//G404: src_c(253)           => '{' __E_src_c_121(258) '}'.
	p(NT(253), (T(47)+NT(258)+T(48)));
//G405: type(150)            => chars(212).
	p(NT(150), (NT(212)));
//G406: type(150)            => chars(212) _(11) '[' _(11) num(190) _(11) ']'.
	p(NT(150), (NT(212)+NT(11)+T(24)+NT(11)+NT(190)+NT(11)+T(25)));
//G407: __E_chars_122(259)   => alnum(6).
	p(NT(259), (NT(6)));
//G408: __E_chars_122(259)   => '_'.
	p(NT(259), (T(35)));
//G409: __E_chars_123(260)   => null.
	p(NT(260), (nul));
//G410: __E_chars_123(260)   => __E_chars_122(259) __E_chars_123(260).
	p(NT(260), (NT(259)+NT(260)));
//G411: chars(212)           => alpha(5) __E_chars_123(260).
	p(NT(212), (NT(5)+NT(260)));
//G412: __E_digits_124(261)  => digit(3).
	p(NT(261), (NT(3)));
//G413: __E_digits_124(261)  => digit(3) __E_digits_124(261).
	p(NT(261), (NT(3)+NT(261)));
//G414: digits(242)          => __E_digits_124(261).
	p(NT(242), (NT(261)));
//G415: sym(22)              => chars(212).
	p(NT(22), (NT(212)));
//G416: __E_comment_125(263) => printable(8).
	p(NT(263), (NT(8)));
//G417: __E_comment_125(263) => '\t'.
	p(NT(263), (T(49)));
//G418: __E_comment_126(264) => null.
	p(NT(264), (nul));
//G419: __E_comment_126(264) => __E_comment_125(263) __E_comment_126(264).
	p(NT(264), (NT(263)+NT(264)));
//G420: __E_comment_127(265) => '\n'.
	p(NT(265), (T(50)));
//G421: __E_comment_127(265) => '\r'.
	p(NT(265), (T(51)));
//G422: __E_comment_127(265) => eof(1).
	p(NT(265), (NT(1)));
//G423: comment(262)         => '#' __E_comment_126(264) __E_comment_127(265).
	p(NT(262), (T(52)+NT(264)+NT(265)));
//G424: __E____128(266)      => space(2).
	p(NT(266), (NT(2)));
//G425: __E____128(266)      => comment(262).
	p(NT(266), (NT(262)));
//G426: __(27)               => __E____128(266) _(11).
	p(NT(27), (NT(266)+NT(11)));
//G427: __E___129(267)       => __(27).
	p(NT(267), (NT(27)));
//G428: __E___129(267)       => null.
	p(NT(267), (nul));
//G429: _(11)                => __E___129(267).
	p(NT(11), (NT(267)));
//G430: cli(268)             => _(11).
	p(NT(268), (NT(11)));
//G431: __E_cli_130(270)     => '.' _(11) cli_command(269) _(11).
	p(NT(270), (T(1)+NT(11)+NT(269)+NT(11)));
//G432: __E_cli_131(271)     => null.
	p(NT(271), (nul));
//G433: __E_cli_131(271)     => __E_cli_130(270) __E_cli_131(271).
	p(NT(271), (NT(270)+NT(271)));
//G434: cli(268)             => _(11) cli_command(269) _(11) __E_cli_131(271).
	p(NT(268), (NT(11)+NT(269)+NT(11)+NT(271)));
//G435: __E_cli_command_132(274) => 'q'.
	p(NT(274), (T(53)));
//G436: __E_cli_command_132(274) => 'q' 'u' 'i' 't'.
	p(NT(274), (T(53)+T(45)+T(10)+T(13)));
//G437: quit_sym(273)        => __E_cli_command_132(274).
	p(NT(273), (NT(274)));
//G438: quit_cmd(272)        => quit_sym(273).
	p(NT(272), (NT(273)));
//G439: cli_command(269)     => quit_cmd(272).
	p(NT(269), (NT(272)));
//G440: __E_cli_command_133(277) => 'v'.
	p(NT(277), (T(54)));
//G441: __E_cli_command_133(277) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(277), (T(54)+T(19)+T(11)+T(12)+T(10)+T(17)+T(55)));
//G442: version_sym(276)     => __E_cli_command_133(277).
	p(NT(276), (NT(277)));
//G443: version_cmd(275)     => version_sym(276).
	p(NT(275), (NT(276)));
//G444: cli_command(269)     => version_cmd(275).
	p(NT(269), (NT(275)));
//G445: __E_cli_command_134(280) => 'c'.
	p(NT(280), (T(8)));
//G446: __E_cli_command_134(280) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(280), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G447: clear_sym(279)       => __E_cli_command_134(280).
	p(NT(279), (NT(280)));
//G448: clear_cmd(278)       => clear_sym(279).
	p(NT(278), (NT(279)));
//G449: cli_command(269)     => clear_cmd(278).
	p(NT(269), (NT(278)));
//G450: __E___E_cli_command_135_136(284) => 'h'.
	p(NT(284), (T(56)));
//G451: __E___E_cli_command_135_136(284) => 'h' 'e' 'l' 'p'.
	p(NT(284), (T(56)+T(19)+T(6)+T(57)));
//G452: help_sym(283)        => __E___E_cli_command_135_136(284).
	p(NT(283), (NT(284)));
//G453: __E___E_cli_command_135_137(285) => __(27) help_arg(286).
	p(NT(285), (NT(27)+NT(286)));
//G454: __E___E_cli_command_135_137(285) => null.
	p(NT(285), (nul));
//G455: __E_cli_command_135(282) => help_sym(283) __E___E_cli_command_135_137(285).
	p(NT(282), (NT(283)+NT(285)));
//G456: help_cmd(281)        => __E_cli_command_135(282).
	p(NT(281), (NT(282)));
//G457: cli_command(269)     => help_cmd(281).
	p(NT(269), (NT(281)));
//G458: file_sym(289)        => 'f' 'i' 'l' 'e'.
	p(NT(289), (T(4)+T(10)+T(6)+T(19)));
//G459: __E_cli_command_138(288) => file_sym(289) __(27) q_string(290).
	p(NT(288), (NT(289)+NT(27)+NT(290)));
//G460: file_cmd(287)        => __E_cli_command_138(288).
	p(NT(287), (NT(288)));
//G461: cli_command(269)     => file_cmd(287).
	p(NT(269), (NT(287)));
//G462: valid_sym(293)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(293), (T(54)+T(5)+T(6)+T(10)+T(58)));
//G463: __E_cli_command_139(292) => valid_sym(293) __(27) normalize_cmd_arg(294).
	p(NT(292), (NT(293)+NT(27)+NT(294)));
//G464: valid_cmd(291)       => __E_cli_command_139(292).
	p(NT(291), (NT(292)));
//G465: cli_command(269)     => valid_cmd(291).
	p(NT(269), (NT(291)));
//G466: sat_sym(297)         => 's' 'a' 't'.
	p(NT(297), (T(12)+T(5)+T(13)));
//G467: __E_cli_command_140(296) => sat_sym(297) __(27) normalize_cmd_arg(294).
	p(NT(296), (NT(297)+NT(27)+NT(294)));
//G468: sat_cmd(295)         => __E_cli_command_140(296).
	p(NT(295), (NT(296)));
//G469: cli_command(269)     => sat_cmd(295).
	p(NT(269), (NT(295)));
//G470: unsat_sym(300)       => 'u' 'n' 's' 'a' 't'.
	p(NT(300), (T(45)+T(55)+T(12)+T(5)+T(13)));
//G471: __E_cli_command_141(299) => unsat_sym(300) __(27) normalize_cmd_arg(294).
	p(NT(299), (NT(300)+NT(27)+NT(294)));
//G472: unsat_cmd(298)       => __E_cli_command_141(299).
	p(NT(298), (NT(299)));
//G473: cli_command(269)     => unsat_cmd(298).
	p(NT(269), (NT(298)));
//G474: solve_sym(303)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(303), (T(12)+T(17)+T(6)+T(54)+T(19)));
//G475: __E___E_cli_command_142_143(304) => solve_options(305).
	p(NT(304), (NT(305)));
//G476: __E___E_cli_command_142_143(304) => null.
	p(NT(304), (nul));
//G477: __E_cli_command_142(302) => solve_sym(303) __E___E_cli_command_142_143(304) __(27) wff_cmd_arg(306).
	p(NT(302), (NT(303)+NT(304)+NT(27)+NT(306)));
//G478: solve_cmd(301)       => __E_cli_command_142(302).
	p(NT(301), (NT(302)));
//G479: cli_command(269)     => solve_cmd(301).
	p(NT(269), (NT(301)));
//G480: lgrs_sym(309)        => 'l' 'g' 'r' 's'.
	p(NT(309), (T(6)+T(59)+T(11)+T(12)));
//G481: __E_cli_command_144(308) => lgrs_sym(309) __(27) wff_cmd_arg(306).
	p(NT(308), (NT(309)+NT(27)+NT(306)));
//G482: lgrs_cmd(307)        => __E_cli_command_144(308).
	p(NT(307), (NT(308)));
//G483: cli_command(269)     => lgrs_cmd(307).
	p(NT(269), (NT(307)));
//G484: __E___E_cli_command_145_146(313) => 'r'.
	p(NT(313), (T(11)));
//G485: __E___E_cli_command_145_146(313) => 'r' 'u' 'n'.
	p(NT(313), (T(11)+T(45)+T(55)));
//G486: run_sym(312)         => __E___E_cli_command_145_146(313).
	p(NT(312), (NT(313)));
//G487: __E___E_cli_command_145_147(314) => memory(315).
	p(NT(314), (NT(315)));
//G488: __E___E_cli_command_145_147(314) => wff(14).
	p(NT(314), (NT(14)));
//G489: __E_cli_command_145(311) => run_sym(312) __(27) __E___E_cli_command_145_147(314).
	p(NT(311), (NT(312)+NT(27)+NT(314)));
//G490: run_cmd(310)         => __E_cli_command_145(311).
	p(NT(310), (NT(311)));
//G491: cli_command(269)     => run_cmd(310).
	p(NT(269), (NT(310)));
//G492: __E___E_cli_command_148_149(319) => 'n'.
	p(NT(319), (T(55)));
//G493: __E___E_cli_command_148_149(319) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(319), (T(55)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(60)+T(19)));
//G494: normalize_sym(318)   => __E___E_cli_command_148_149(319).
	p(NT(318), (NT(319)));
//G495: __E_cli_command_148(317) => normalize_sym(318) __(27) normalize_cmd_arg(294).
	p(NT(317), (NT(318)+NT(27)+NT(294)));
//G496: normalize_cmd(316)   => __E_cli_command_148(317).
	p(NT(316), (NT(317)));
//G497: cli_command(269)     => normalize_cmd(316).
	p(NT(269), (NT(316)));
//G498: __E___E_cli_command_150_151(323) => 's'.
	p(NT(323), (T(12)));
//G499: __E___E_cli_command_150_151(323) => 's' 'u' 'b' 's' 't'.
	p(NT(323), (T(12)+T(45)+T(7)+T(12)+T(13)));
//G500: __E___E_cli_command_150_151(323) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(323), (T(12)+T(45)+T(7)+T(12)+T(13)+T(10)+T(13)+T(45)+T(13)+T(19)));
//G501: subst_sym(322)       => __E___E_cli_command_150_151(323).
	p(NT(322), (NT(323)));
//G502: __E_cli_command_150(321) => subst_sym(322) __(27) nf_cmd_arg(324) _(11) '[' _(11) nf_cmd_arg(324) _(11) '/' _(11) nf_cmd_arg(324) _(11) ']'.
	p(NT(321), (NT(322)+NT(27)+NT(324)+NT(11)+T(24)+NT(11)+NT(324)+NT(11)+T(43)+NT(11)+NT(324)+NT(11)+T(25)));
//G503: subst_cmd(320)       => __E_cli_command_150(321).
	p(NT(320), (NT(321)));
//G504: cli_command(269)     => subst_cmd(320).
	p(NT(269), (NT(320)));
//G505: __E___E_cli_command_152_153(328) => 'i'.
	p(NT(328), (T(10)));
//G506: __E___E_cli_command_152_153(328) => 'i' 'n' 's' 't'.
	p(NT(328), (T(10)+T(55)+T(12)+T(13)));
//G507: __E___E_cli_command_152_153(328) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(328), (T(10)+T(55)+T(12)+T(13)+T(5)+T(55)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G508: inst_sym(327)        => __E___E_cli_command_152_153(328).
	p(NT(327), (NT(328)));
//G509: __E_cli_command_152(326) => inst_sym(327) __(27) inst_args(329).
	p(NT(326), (NT(327)+NT(27)+NT(329)));
//G510: inst_cmd(325)        => __E_cli_command_152(326).
	p(NT(325), (NT(326)));
//G511: cli_command(269)     => inst_cmd(325).
	p(NT(269), (NT(325)));
//G512: dnf_sym(332)         => 'd' 'n' 'f'.
	p(NT(332), (T(58)+T(55)+T(4)));
//G513: __E_cli_command_154(331) => dnf_sym(332) __(27) nf_cmd_arg(324).
	p(NT(331), (NT(332)+NT(27)+NT(324)));
//G514: dnf_cmd(330)         => __E_cli_command_154(331).
	p(NT(330), (NT(331)));
//G515: cli_command(269)     => dnf_cmd(330).
	p(NT(269), (NT(330)));
//G516: cnf_sym(335)         => 'c' 'n' 'f'.
	p(NT(335), (T(8)+T(55)+T(4)));
//G517: __E_cli_command_155(334) => cnf_sym(335) __(27) nf_cmd_arg(324).
	p(NT(334), (NT(335)+NT(27)+NT(324)));
//G518: cnf_cmd(333)         => __E_cli_command_155(334).
	p(NT(333), (NT(334)));
//G519: cli_command(269)     => cnf_cmd(333).
	p(NT(269), (NT(333)));
//G520: anf_sym(338)         => 'a' 'n' 'f'.
	p(NT(338), (T(5)+T(55)+T(4)));
//G521: __E_cli_command_156(337) => anf_sym(338) __(27) nf_cmd_arg(324).
	p(NT(337), (NT(338)+NT(27)+NT(324)));
//G522: anf_cmd(336)         => __E_cli_command_156(337).
	p(NT(336), (NT(337)));
//G523: cli_command(269)     => anf_cmd(336).
	p(NT(269), (NT(336)));
//G524: nnf_sym(341)         => 'n' 'n' 'f'.
	p(NT(341), (T(55)+T(55)+T(4)));
//G525: __E_cli_command_157(340) => nnf_sym(341) __(27) nf_cmd_arg(324).
	p(NT(340), (NT(341)+NT(27)+NT(324)));
//G526: nnf_cmd(339)         => __E_cli_command_157(340).
	p(NT(339), (NT(340)));
//G527: cli_command(269)     => nnf_cmd(339).
	p(NT(269), (NT(339)));
//G528: pnf_sym(344)         => 'p' 'n' 'f'.
	p(NT(344), (T(57)+T(55)+T(4)));
//G529: __E_cli_command_158(343) => pnf_sym(344) __(27) nf_cmd_arg(324).
	p(NT(343), (NT(344)+NT(27)+NT(324)));
//G530: pnf_cmd(342)         => __E_cli_command_158(343).
	p(NT(342), (NT(343)));
//G531: cli_command(269)     => pnf_cmd(342).
	p(NT(269), (NT(342)));
//G532: mnf_sym(347)         => 'm' 'n' 'f'.
	p(NT(347), (T(18)+T(55)+T(4)));
//G533: __E_cli_command_159(346) => mnf_sym(347) __(27) nf_cmd_arg(324).
	p(NT(346), (NT(347)+NT(27)+NT(324)));
//G534: mnf_cmd(345)         => __E_cli_command_159(346).
	p(NT(345), (NT(346)));
//G535: cli_command(269)     => mnf_cmd(345).
	p(NT(269), (NT(345)));
//G536: snf_sym(350)         => 's' 'n' 'f'.
	p(NT(350), (T(12)+T(55)+T(4)));
//G537: __E_cli_command_160(349) => snf_sym(350) __(27) nf_cmd_arg(324).
	p(NT(349), (NT(350)+NT(27)+NT(324)));
//G538: snf_cmd(348)         => __E_cli_command_160(349).
	p(NT(348), (NT(349)));
//G539: cli_command(269)     => snf_cmd(348).
	p(NT(269), (NT(348)));
//G540: onf_sym(353)         => 'o' 'n' 'f'.
	p(NT(353), (T(17)+T(55)+T(4)));
//G541: __E_cli_command_161(352) => onf_sym(353) __(27) variable(139) __(27) onf_cmd_arg(354).
	p(NT(352), (NT(353)+NT(27)+NT(139)+NT(27)+NT(354)));
//G542: onf_cmd(351)         => __E_cli_command_161(352).
	p(NT(351), (NT(352)));
//G543: cli_command(269)     => onf_cmd(351).
	p(NT(269), (NT(351)));
//G544: __E___E_cli_command_162_163(358) => 'd' 'e' 'f' 's'.
	p(NT(358), (T(58)+T(19)+T(4)+T(12)));
//G545: __E___E_cli_command_162_163(358) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(358), (T(58)+T(19)+T(4)+T(10)+T(55)+T(10)+T(13)+T(10)+T(17)+T(55)+T(12)));
//G546: def_sym(357)         => __E___E_cli_command_162_163(358).
	p(NT(357), (NT(358)));
//G547: __E_cli_command_162(356) => def_sym(357).
	p(NT(356), (NT(357)));
//G548: def_list_cmd(355)    => __E_cli_command_162(356).
	p(NT(355), (NT(356)));
//G549: cli_command(269)     => def_list_cmd(355).
	p(NT(269), (NT(355)));
//G550: __E_cli_command_164(360) => def_sym(357) __(27) number(361).
	p(NT(360), (NT(357)+NT(27)+NT(361)));
//G551: def_print_cmd(359)   => __E_cli_command_164(360).
	p(NT(359), (NT(360)));
//G552: cli_command(269)     => def_print_cmd(359).
	p(NT(269), (NT(359)));
//G553: def_rr_cmd(362)      => rec_relation(16).
	p(NT(362), (NT(16)));
//G554: cli_command(269)     => def_rr_cmd(362).
	p(NT(269), (NT(362)));
//G555: qelim_sym(365)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(365), (T(53)+T(19)+T(6)+T(10)+T(18)));
//G556: __E_cli_command_165(364) => qelim_sym(365) __(27) wff_cmd_arg(306).
	p(NT(364), (NT(365)+NT(27)+NT(306)));
//G557: qelim_cmd(363)       => __E_cli_command_165(364).
	p(NT(363), (NT(364)));
//G558: cli_command(269)     => qelim_cmd(363).
	p(NT(269), (NT(363)));
//G559: get_sym(368)         => 'g' 'e' 't'.
	p(NT(368), (T(59)+T(19)+T(13)));
//G560: __E___E_cli_command_166_167(369) => __(27) option_name(370).
	p(NT(369), (NT(27)+NT(370)));
//G561: __E___E_cli_command_166_167(369) => null.
	p(NT(369), (nul));
//G562: __E_cli_command_166(367) => get_sym(368) __E___E_cli_command_166_167(369).
	p(NT(367), (NT(368)+NT(369)));
//G563: get_cmd(366)         => __E_cli_command_166(367).
	p(NT(366), (NT(367)));
//G564: cli_command(269)     => get_cmd(366).
	p(NT(269), (NT(366)));
//G565: set_sym(373)         => 's' 'e' 't'.
	p(NT(373), (T(12)+T(19)+T(13)));
//G566: __E___E_cli_command_168_169(374) => __(27).
	p(NT(374), (NT(27)));
//G567: __E___E_cli_command_168_169(374) => _(11) '=' _(11).
	p(NT(374), (NT(11)+T(3)+NT(11)));
//G568: __E_cli_command_168(372) => set_sym(373) __(27) option_name(370) __E___E_cli_command_168_169(374) option_value(375).
	p(NT(372), (NT(373)+NT(27)+NT(370)+NT(374)+NT(375)));
//G569: set_cmd(371)         => __E_cli_command_168(372).
	p(NT(371), (NT(372)));
//G570: cli_command(269)     => set_cmd(371).
	p(NT(269), (NT(371)));
//G571: enable_sym(378)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(378), (T(19)+T(55)+T(5)+T(7)+T(6)+T(19)));
//G572: __E_cli_command_170(377) => enable_sym(378) __(27) option_name(370).
	p(NT(377), (NT(378)+NT(27)+NT(370)));
//G573: enable_cmd(376)      => __E_cli_command_170(377).
	p(NT(376), (NT(377)));
//G574: cli_command(269)     => enable_cmd(376).
	p(NT(269), (NT(376)));
//G575: disable_sym(381)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(381), (T(58)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G576: __E_cli_command_171(380) => disable_sym(381) __(27) option_name(370).
	p(NT(380), (NT(381)+NT(27)+NT(370)));
//G577: disable_cmd(379)     => __E_cli_command_171(380).
	p(NT(379), (NT(380)));
//G578: cli_command(269)     => disable_cmd(379).
	p(NT(269), (NT(379)));
//G579: toggle_sym(384)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(384), (T(13)+T(17)+T(59)+T(59)+T(6)+T(19)));
//G580: __E_cli_command_172(383) => toggle_sym(384) __(27) option_name(370).
	p(NT(383), (NT(384)+NT(27)+NT(370)));
//G581: toggle_cmd(382)      => __E_cli_command_172(383).
	p(NT(382), (NT(383)));
//G582: cli_command(269)     => toggle_cmd(382).
	p(NT(269), (NT(382)));
//G583: __E_cli_command_173(386) => type(150) __(27) in_var_name(223) _(11) '=' _(11) input_stream(387).
	p(NT(386), (NT(150)+NT(27)+NT(223)+NT(11)+T(3)+NT(11)+NT(387)));
//G584: def_input_cmd(385)   => __E_cli_command_173(386).
	p(NT(385), (NT(386)));
//G585: cli_command(269)     => def_input_cmd(385).
	p(NT(269), (NT(385)));
//G586: __E_cli_command_174(389) => type(150) __(27) out_var_name(227) _(11) '=' _(11) output_stream(390).
	p(NT(389), (NT(150)+NT(27)+NT(227)+NT(11)+T(3)+NT(11)+NT(390)));
//G587: def_output_cmd(388)  => __E_cli_command_174(389).
	p(NT(388), (NT(389)));
//G588: cli_command(269)     => def_output_cmd(388).
	p(NT(269), (NT(388)));
//G589: __E___E_cli_command_175_176(394) => 'h' 'i' 's' 't'.
	p(NT(394), (T(56)+T(10)+T(12)+T(13)));
//G590: __E___E_cli_command_175_176(394) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(394), (T(56)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G591: history_sym(393)     => __E___E_cli_command_175_176(394).
	p(NT(393), (NT(394)));
//G592: __E_cli_command_175(392) => history_sym(393).
	p(NT(392), (NT(393)));
//G593: history_list_cmd(391) => __E_cli_command_175(392).
	p(NT(391), (NT(392)));
//G594: cli_command(269)     => history_list_cmd(391).
	p(NT(269), (NT(391)));
//G595: __E_cli_command_177(396) => history_sym(393) __(27) memory(315).
	p(NT(396), (NT(393)+NT(27)+NT(315)));
//G596: history_print_cmd(395) => __E_cli_command_177(396).
	p(NT(395), (NT(396)));
//G597: cli_command(269)     => history_print_cmd(395).
	p(NT(269), (NT(395)));
//G598: __E_cli_command_178(398) => wff(14).
	p(NT(398), (NT(14)));
//G599: __E_cli_command_178(398) => bf(21).
	p(NT(398), (NT(21)));
//G600: history_store_cmd(397) => __E_cli_command_178(398).
	p(NT(397), (NT(398)));
//G601: cli_command(269)     => history_store_cmd(397).
	p(NT(269), (NT(397)));
//G602: __E_solve_options_179(399) => __(27) solve_option(400).
	p(NT(399), (NT(27)+NT(400)));
//G603: __E_solve_options_180(401) => null.
	p(NT(401), (nul));
//G604: __E_solve_options_180(401) => __E_solve_options_179(399) __E_solve_options_180(401).
	p(NT(401), (NT(399)+NT(401)));
//G605: solve_options(305)   => __E_solve_options_180(401).
	p(NT(305), (NT(401)));
//G606: __E_solve_option_181(402) => '-' '-' solver_mode(403).
	p(NT(402), (T(28)+T(28)+NT(403)));
//G607: solve_option(400)    => __E_solve_option_181(402).
	p(NT(400), (NT(402)));
//G608: __E_solve_option_182(404) => '-' '-' type(150).
	p(NT(404), (T(28)+T(28)+NT(150)));
//G609: solve_option(400)    => __E_solve_option_182(404).
	p(NT(400), (NT(404)));
//G610: __E_solver_mode_183(406) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(406), (T(18)+T(10)+T(55)+T(10)+T(18)+T(45)+T(18)));
//G611: __E_solver_mode_183(406) => 'm' 'i' 'n'.
	p(NT(406), (T(18)+T(10)+T(55)));
//G612: solver_mode_minimum(405) => __E_solver_mode_183(406).
	p(NT(405), (NT(406)));
//G613: solver_mode(403)     => solver_mode_minimum(405).
	p(NT(403), (NT(405)));
//G614: __E_solver_mode_184(408) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(408), (T(18)+T(5)+T(27)+T(10)+T(18)+T(45)+T(18)));
//G615: __E_solver_mode_184(408) => 'm' 'a' 'x'.
	p(NT(408), (T(18)+T(5)+T(27)));
//G616: solver_mode_maximum(407) => __E_solver_mode_184(408).
	p(NT(407), (NT(408)));
//G617: solver_mode(403)     => solver_mode_maximum(407).
	p(NT(403), (NT(407)));
//G618: __E_input_stream_185(409) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(410) _(11) ')'.
	p(NT(409), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(410)+NT(11)+T(16)));
//G619: input_stream(387)    => __E_input_stream_185(409).
	p(NT(387), (NT(409)));
//G620: console_sym(411)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(411), (T(8)+T(17)+T(55)+T(12)+T(17)+T(6)+T(19)));
//G621: input_stream(387)    => console_sym(411).
	p(NT(387), (NT(411)));
//G622: __E_output_stream_186(412) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(410) _(11) ')'.
	p(NT(412), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(410)+NT(11)+T(16)));
//G623: output_stream(390)   => __E_output_stream_186(412).
	p(NT(390), (NT(412)));
//G624: output_stream(390)   => console_sym(411).
	p(NT(390), (NT(411)));
//G625: q_file_name(410)     => '"' file_name(413) '"'.
	p(NT(410), (T(61)+NT(413)+T(61)));
//G626: __E_file_name_187(414) => printable(8).
	p(NT(414), (NT(8)));
//G627: __E_file_name_187(414) => printable(8) __E_file_name_187(414).
	p(NT(414), (NT(8)+NT(414)));
//G628: file_name(413)       => __E_file_name_187(414).
	p(NT(413), (NT(414)));
//G629: __E_option_name_188(415) => alnum(6).
	p(NT(415), (NT(6)));
//G630: __E_option_name_188(415) => alnum(6) __E_option_name_188(415).
	p(NT(415), (NT(6)+NT(415)));
//G631: option_name(370)     => __E_option_name_188(415).
	p(NT(370), (NT(415)));
//G632: __E_option_value_189(416) => alnum(6).
	p(NT(416), (NT(6)));
//G633: __E_option_value_189(416) => alnum(6) __E_option_value_189(416).
	p(NT(416), (NT(6)+NT(416)));
//G634: option_value(375)    => __E_option_value_189(416).
	p(NT(375), (NT(416)));
//G635: number(361)          => digits(242).
	p(NT(361), (NT(242)));
//G636: bf_cmd_arg(417)      => memory(315).
	p(NT(417), (NT(315)));
//G637: bf_cmd_arg(417)      => bf(21).
	p(NT(417), (NT(21)));
//G638: wff_cmd_arg(306)     => memory(315).
	p(NT(306), (NT(315)));
//G639: wff_cmd_arg(306)     => wff(14).
	p(NT(306), (NT(14)));
//G640: nf_cmd_arg(324)      => memory(315).
	p(NT(324), (NT(315)));
//G641: nf_cmd_arg(324)      => ref(18).
	p(NT(324), (NT(18)));
//G642: nf_cmd_arg(324)      => wff(14).
	p(NT(324), (NT(14)));
//G643: nf_cmd_arg(324)      => bf(21).
	p(NT(324), (NT(21)));
//G644: onf_cmd_arg(354)     => memory(315).
	p(NT(354), (NT(315)));
//G645: onf_cmd_arg(354)     => wff(14).
	p(NT(354), (NT(14)));
//G646: normalize_cmd_arg(294) => memory(315).
	p(NT(294), (NT(315)));
//G647: normalize_cmd_arg(294) => rr(10).
	p(NT(294), (NT(10)));
//G648: normalize_cmd_arg(294) => ref(18).
	p(NT(294), (NT(18)));
//G649: normalize_cmd_arg(294) => wff(14).
	p(NT(294), (NT(14)));
//G650: normalize_cmd_arg(294) => bf(21).
	p(NT(294), (NT(21)));
//G651: inst_args(329)       => wff_cmd_arg(306) _(11) '[' _(11) variable(139) _(11) '/' _(11) bf_cmd_arg(417) _(11) ']'.
	p(NT(329), (NT(306)+NT(11)+T(24)+NT(11)+NT(139)+NT(11)+T(43)+NT(11)+NT(417)+NT(11)+T(25)));
//G652: inst_args(329)       => bf_cmd_arg(417) _(11) '[' _(11) variable(139) _(11) '/' _(11) bf_cmd_arg(417) _(11) ']'.
	p(NT(329), (NT(417)+NT(11)+T(24)+NT(11)+NT(139)+NT(11)+T(43)+NT(11)+NT(417)+NT(11)+T(25)));
//G653: help_arg(286)        => help_sym(283).
	p(NT(286), (NT(283)));
//G654: help_arg(286)        => version_sym(276).
	p(NT(286), (NT(276)));
//G655: help_arg(286)        => quit_sym(273).
	p(NT(286), (NT(273)));
//G656: help_arg(286)        => clear_sym(279).
	p(NT(286), (NT(279)));
//G657: help_arg(286)        => get_sym(368).
	p(NT(286), (NT(368)));
//G658: help_arg(286)        => set_sym(373).
	p(NT(286), (NT(373)));
//G659: help_arg(286)        => enable_sym(378).
	p(NT(286), (NT(378)));
//G660: help_arg(286)        => disable_sym(381).
	p(NT(286), (NT(381)));
//G661: help_arg(286)        => toggle_sym(384).
	p(NT(286), (NT(384)));
//G662: help_arg(286)        => file_sym(289).
	p(NT(286), (NT(289)));
//G663: help_arg(286)        => history_sym(393).
	p(NT(286), (NT(393)));
//G664: help_arg(286)        => abs_memory_sym(418).
	p(NT(286), (NT(418)));
//G665: help_arg(286)        => rel_memory_sym(419).
	p(NT(286), (NT(419)));
//G666: help_arg(286)        => selection_sym(420).
	p(NT(286), (NT(420)));
//G667: help_arg(286)        => def_sym(357).
	p(NT(286), (NT(357)));
//G668: help_arg(286)        => inst_sym(327).
	p(NT(286), (NT(327)));
//G669: help_arg(286)        => subst_sym(322).
	p(NT(286), (NT(322)));
//G670: help_arg(286)        => normalize_sym(318).
	p(NT(286), (NT(318)));
//G671: help_arg(286)        => execute_sym(421).
	p(NT(286), (NT(421)));
//G672: help_arg(286)        => solve_sym(303).
	p(NT(286), (NT(303)));
//G673: help_arg(286)        => valid_sym(293).
	p(NT(286), (NT(293)));
//G674: help_arg(286)        => sat_sym(297).
	p(NT(286), (NT(297)));
//G675: help_arg(286)        => unsat_sym(300).
	p(NT(286), (NT(300)));
//G676: help_arg(286)        => run_sym(312).
	p(NT(286), (NT(312)));
//G677: help_arg(286)        => dnf_sym(332).
	p(NT(286), (NT(332)));
//G678: help_arg(286)        => cnf_sym(335).
	p(NT(286), (NT(335)));
//G679: help_arg(286)        => anf_sym(338).
	p(NT(286), (NT(338)));
//G680: help_arg(286)        => snf_sym(350).
	p(NT(286), (NT(350)));
//G681: help_arg(286)        => nnf_sym(341).
	p(NT(286), (NT(341)));
//G682: help_arg(286)        => pnf_sym(344).
	p(NT(286), (NT(344)));
//G683: help_arg(286)        => mnf_sym(347).
	p(NT(286), (NT(347)));
//G684: help_arg(286)        => onf_sym(353).
	p(NT(286), (NT(353)));
//G685: help_arg(286)        => qelim_sym(365).
	p(NT(286), (NT(365)));
//G686: __E___E_help_arg_190_191(424) => 's'.
	p(NT(424), (T(12)));
//G687: __E___E_help_arg_190_191(424) => null.
	p(NT(424), (nul));
//G688: __E_help_arg_190(423) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_190_191(424).
	p(NT(423), (T(19)+T(27)+T(5)+T(18)+T(57)+T(6)+T(19)+NT(424)));
//G689: examples_sym(422)    => __E_help_arg_190(423).
	p(NT(422), (NT(423)));
//G690: help_arg(286)        => examples_sym(422).
	p(NT(286), (NT(422)));
//G691: __E_memory_192(426)  => '%'.
	p(NT(426), (T(44)));
//G692: rel_memory_sym(419)  => '%' '-'.
	p(NT(419), (T(44)+T(28)));
//G693: memory_id(428)       => digits(242).
	p(NT(428), (NT(242)));
//G694: __E___E_memory_192_193(427) => memory_id(428).
	p(NT(427), (NT(428)));
//G695: __E___E_memory_192_193(427) => null.
	p(NT(427), (nul));
//G696: __E_memory_192(426)  => rel_memory_sym(419) __E___E_memory_192_193(427).
	p(NT(426), (NT(419)+NT(427)));
//G697: rel_memory(425)      => __E_memory_192(426).
	p(NT(425), (NT(426)));
//G698: memory(315)          => rel_memory(425).
	p(NT(315), (NT(425)));
//G699: abs_memory_sym(418)  => '%'.
	p(NT(418), (T(44)));
//G700: __E_memory_194(430)  => abs_memory_sym(418) memory_id(428).
	p(NT(430), (NT(418)+NT(428)));
//G701: abs_memory(429)      => __E_memory_194(430).
	p(NT(429), (NT(430)));
//G702: memory(315)          => abs_memory(429).
	p(NT(315), (NT(429)));
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
		__E_wff_41, bf_less, __E_wff_42, bf_nless, __E_wff_43, z3_eq, __E_wff_44, z3, z3_neq, __E_wff_45, 
		z3_less_equal, __E_wff_46, z3_nleq, __E_wff_47, z3_greater, __E_wff_48, z3_ngreater, __E_wff_49, z3_greater_equal, __E_wff_50, 
		z3_ngeq, __E_wff_51, z3_less, __E_wff_52, z3_nless, __E_wff_53, bf_parenthesis, __E_bf_54, bf_constant, variable, 
		bf_splitter, __E_bf_55, bf_ref, bf_or, __E_bf_56, bf_xor, __E_bf_57, bf_t, __E_bf_58, __E___E_bf_58_59, 
		type, bf_f, __E_bf_60, __E___E_bf_60_61, bf_and, __E_bf_62, __E___E_bf_62_63, bf_neg, __E_bf_64, bf_neg_oprnd, 
		__E___E_bf_64_65, bf_and_nosep, __E_bf_66, bf_and_nosep_1st_oprnd, __E___E_bf_66_67, bf_and_nosep_2nd_oprnd, __E___E_bf_66_68, z3_parenthesis, __E_z3_69, bitvector, 
		z3_not, z3_add, __E_z3_70, z3_sub, __E_z3_71, z3_mul, __E_z3_72, z3_div, __E_z3_73, z3_mod, 
		__E_z3_74, z3_and, __E_z3_75, z3_or, __E_z3_76, z3_xor, __E_z3_77, bits, __E_bitvector_78, bitvector_type, 
		num, __E_bitvector_79, __E_bitvector_type_80, bit, __E_bits_81, __E_bits_82, ctn_neq, __E_constraint_83, ctnvar, ctn_eq, 
		__E_constraint_84, ctn_greater_equal, __E_constraint_85, ctn_greater, __E_constraint_86, ctn_less_equal, __E_constraint_87, ctn_less, __E_constraint_88, __E_variable_89, 
		__E_variable_90, __E_variable_91, chars, __E_variable_92, __E_variable_93, __E_variable_94, __E_variable_95, io_var, __E_variable_96, in, 
		out, __E_variable_97, uninterpreted_constant, in_var_name, __E_in_98, __E___E_in_98_99, offset, out_var_name, __E_out_100, __E___E_out_100_101, 
		__E___E___E_out_100_101_102, q_var, __E_q_vars_103, __E_q_vars_104, __E_q_var_105, __E_offsets_106, __E_offsets_107, integer, shift, __E_offset_108, 
		__E_shift_109, __E___E_shift_109_110, digits, __E_integer_111, uninter_const_name, __E_uninterpreted_constant_112, __E___E_uninterpreted_constant_112_113, constant, __E_bf_constant_114, binding, 
		__E_bf_constant_115, source, __E_binding_116, src_c, __E___E_binding_116_117, __E___E___E_binding_116_117_118, __E___E___E_binding_116_117_119, __E_src_c_120, __E_src_c_121, __E_chars_122, 
		__E_chars_123, __E_digits_124, comment, __E_comment_125, __E_comment_126, __E_comment_127, __E____128, __E___129, cli, cli_command, 
		__E_cli_130, __E_cli_131, quit_cmd, quit_sym, __E_cli_command_132, version_cmd, version_sym, __E_cli_command_133, clear_cmd, clear_sym, 
		__E_cli_command_134, help_cmd, __E_cli_command_135, help_sym, __E___E_cli_command_135_136, __E___E_cli_command_135_137, help_arg, file_cmd, __E_cli_command_138, file_sym, 
		q_string, valid_cmd, __E_cli_command_139, valid_sym, normalize_cmd_arg, sat_cmd, __E_cli_command_140, sat_sym, unsat_cmd, __E_cli_command_141, 
		unsat_sym, solve_cmd, __E_cli_command_142, solve_sym, __E___E_cli_command_142_143, solve_options, wff_cmd_arg, lgrs_cmd, __E_cli_command_144, lgrs_sym, 
		run_cmd, __E_cli_command_145, run_sym, __E___E_cli_command_145_146, __E___E_cli_command_145_147, memory, normalize_cmd, __E_cli_command_148, normalize_sym, __E___E_cli_command_148_149, 
		subst_cmd, __E_cli_command_150, subst_sym, __E___E_cli_command_150_151, nf_cmd_arg, inst_cmd, __E_cli_command_152, inst_sym, __E___E_cli_command_152_153, inst_args, 
		dnf_cmd, __E_cli_command_154, dnf_sym, cnf_cmd, __E_cli_command_155, cnf_sym, anf_cmd, __E_cli_command_156, anf_sym, nnf_cmd, 
		__E_cli_command_157, nnf_sym, pnf_cmd, __E_cli_command_158, pnf_sym, mnf_cmd, __E_cli_command_159, mnf_sym, snf_cmd, __E_cli_command_160, 
		snf_sym, onf_cmd, __E_cli_command_161, onf_sym, onf_cmd_arg, def_list_cmd, __E_cli_command_162, def_sym, __E___E_cli_command_162_163, def_print_cmd, 
		__E_cli_command_164, number, def_rr_cmd, qelim_cmd, __E_cli_command_165, qelim_sym, get_cmd, __E_cli_command_166, get_sym, __E___E_cli_command_166_167, 
		option_name, set_cmd, __E_cli_command_168, set_sym, __E___E_cli_command_168_169, option_value, enable_cmd, __E_cli_command_170, enable_sym, disable_cmd, 
		__E_cli_command_171, disable_sym, toggle_cmd, __E_cli_command_172, toggle_sym, def_input_cmd, __E_cli_command_173, input_stream, def_output_cmd, __E_cli_command_174, 
		output_stream, history_list_cmd, __E_cli_command_175, history_sym, __E___E_cli_command_175_176, history_print_cmd, __E_cli_command_177, history_store_cmd, __E_cli_command_178, __E_solve_options_179, 
		solve_option, __E_solve_options_180, __E_solve_option_181, solver_mode, __E_solve_option_182, solver_mode_minimum, __E_solver_mode_183, solver_mode_maximum, __E_solver_mode_184, __E_input_stream_185, 
		q_file_name, console_sym, __E_output_stream_186, file_name, __E_file_name_187, __E_option_name_188, __E_option_value_189, bf_cmd_arg, abs_memory_sym, rel_memory_sym, 
		selection_sym, execute_sym, examples_sym, __E_help_arg_190, __E___E_help_arg_190_191, rel_memory, __E_memory_192, __E___E_memory_192_193, memory_id, abs_memory, 
		__E_memory_194, named_binding, z3_bv, z3_bv_parenthesis, __N_0, __N_1, __N_2, __N_3, __N_4, __N_5, 
		__N_6, __N_7, __N_8, 
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
