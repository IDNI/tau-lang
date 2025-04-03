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
	"__E___E_bf_64_65", "bf_and_nosep", "__E_bf_66", "bf_and_nosep_1st_oprnd", "__E___E_bf_66_67", "bf_and_nosep_2nd_oprnd", "__E___E_bf_66_68", "z3_parenthesis", "__E_z3_69", "z3_neg", 
	"z3_add", "__E_z3_70", "z3_sub", "__E_z3_71", "z3_mul", "__E_z3_72", "z3_div", "__E_z3_73", "z3_mod", "__E_z3_74", 
	"z3_and", "__E_z3_75", "z3_or", "__E_z3_76", "z3_xor", "__E_z3_77", "z3_shift_left", "__E_z3_78", "num", "z3_shift_right", 
	"__E_z3_79", "bitvector", "bits", "__E_bitvector_80", "bitvector_type", "__E_bitvector_81", "__E_bitvector_type_82", "bit", "__E_bits_83", "__E_bits_84", 
	"ctn_neq", "__E_constraint_85", "ctnvar", "ctn_eq", "__E_constraint_86", "ctn_greater_equal", "__E_constraint_87", "ctn_greater", "__E_constraint_88", "ctn_less_equal", 
	"__E_constraint_89", "ctn_less", "__E_constraint_90", "__E_variable_91", "__E_variable_92", "__E_variable_93", "chars", "__E_variable_94", "__E_variable_95", "__E_variable_96", 
	"__E_variable_97", "io_var", "__E_variable_98", "in", "out", "__E_variable_99", "uninterpreted_constant", "in_var_name", "__E_in_100", "__E___E_in_100_101", 
	"offset", "out_var_name", "__E_out_102", "__E___E_out_102_103", "__E___E___E_out_102_103_104", "q_var", "__E_q_vars_105", "__E_q_vars_106", "__E_q_var_107", "__E_offsets_108", 
	"__E_offsets_109", "integer", "shift", "__E_offset_110", "__E_shift_111", "__E___E_shift_111_112", "digits", "__E_integer_113", "uninter_const_name", "__E_uninterpreted_constant_114", 
	"__E___E_uninterpreted_constant_114_115", "constant", "__E_bf_constant_116", "binding", "__E_bf_constant_117", "source", "__E_binding_118", "src_c", "__E___E_binding_118_119", "__E___E___E_binding_118_119_120", 
	"__E___E___E_binding_118_119_121", "__E_src_c_122", "__E_src_c_123", "__E_chars_124", "__E_chars_125", "__E_digits_126", "comment", "__E_comment_127", "__E_comment_128", "__E_comment_129", 
	"__E____130", "__E___131", "cli", "cli_command", "__E_cli_132", "__E_cli_133", "quit_cmd", "quit_sym", "__E_cli_command_134", "version_cmd", 
	"version_sym", "__E_cli_command_135", "clear_cmd", "clear_sym", "__E_cli_command_136", "help_cmd", "__E_cli_command_137", "help_sym", "__E___E_cli_command_137_138", "__E___E_cli_command_137_139", 
	"help_arg", "file_cmd", "__E_cli_command_140", "file_sym", "q_string", "valid_cmd", "__E_cli_command_141", "valid_sym", "normalize_cmd_arg", "sat_cmd", 
	"__E_cli_command_142", "sat_sym", "unsat_cmd", "__E_cli_command_143", "unsat_sym", "solve_cmd", "__E_cli_command_144", "solve_sym", "__E___E_cli_command_144_145", "solve_options", 
	"wff_cmd_arg", "lgrs_cmd", "__E_cli_command_146", "lgrs_sym", "run_cmd", "__E_cli_command_147", "run_sym", "__E___E_cli_command_147_148", "__E___E_cli_command_147_149", "memory", 
	"normalize_cmd", "__E_cli_command_150", "normalize_sym", "__E___E_cli_command_150_151", "subst_cmd", "__E_cli_command_152", "subst_sym", "__E___E_cli_command_152_153", "nf_cmd_arg", "inst_cmd", 
	"__E_cli_command_154", "inst_sym", "__E___E_cli_command_154_155", "inst_args", "dnf_cmd", "__E_cli_command_156", "dnf_sym", "cnf_cmd", "__E_cli_command_157", "cnf_sym", 
	"anf_cmd", "__E_cli_command_158", "anf_sym", "nnf_cmd", "__E_cli_command_159", "nnf_sym", "pnf_cmd", "__E_cli_command_160", "pnf_sym", "mnf_cmd", 
	"__E_cli_command_161", "mnf_sym", "snf_cmd", "__E_cli_command_162", "snf_sym", "onf_cmd", "__E_cli_command_163", "onf_sym", "onf_cmd_arg", "def_list_cmd", 
	"__E_cli_command_164", "def_sym", "__E___E_cli_command_164_165", "def_print_cmd", "__E_cli_command_166", "number", "def_rr_cmd", "qelim_cmd", "__E_cli_command_167", "qelim_sym", 
	"get_cmd", "__E_cli_command_168", "get_sym", "__E___E_cli_command_168_169", "option_name", "set_cmd", "__E_cli_command_170", "set_sym", "__E___E_cli_command_170_171", "option_value", 
	"enable_cmd", "__E_cli_command_172", "enable_sym", "disable_cmd", "__E_cli_command_173", "disable_sym", "toggle_cmd", "__E_cli_command_174", "toggle_sym", "def_input_cmd", 
	"__E_cli_command_175", "input_stream", "def_output_cmd", "__E_cli_command_176", "output_stream", "history_list_cmd", "__E_cli_command_177", "history_sym", "__E___E_cli_command_177_178", "history_print_cmd", 
	"__E_cli_command_179", "history_store_cmd", "__E_cli_command_180", "__E_solve_options_181", "solve_option", "__E_solve_options_182", "__E_solve_option_183", "solver_mode", "__E_solve_option_184", "solver_mode_minimum", 
	"__E_solver_mode_185", "solver_mode_maximum", "__E_solver_mode_186", "__E_input_stream_187", "q_file_name", "console_sym", "__E_output_stream_188", "file_name", "__E_file_name_189", "__E_option_name_190", 
	"__E_option_value_191", "bf_cmd_arg", "abs_memory_sym", "rel_memory_sym", "selection_sym", "execute_sym", "examples_sym", "__E_help_arg_192", "__E___E_help_arg_192_193", "rel_memory", 
	"__E_memory_194", "__E___E_memory_194_195", "memory_id", "abs_memory", "__E_memory_196", "named_binding", "__N_0", "__N_1", "__N_2", "__N_3", 
	"__N_4", "__N_5", "__N_6", "__N_7", "__N_8", 
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
			20, 22, 139, 150, 202, 227, 231, 241, 246, 248,
			255, 374, 379, 417, 435
		},
		.to_inline = {
			{ 14, 60, 14 },
			{ 21, 136, 21 },
			{ 117, 167, 117 },
			{ 159, 136, 21 },
			{ 161, 136, 154 },
			{ 163, 136, 21 },
			{ 165, 136, 21 },
			{ 194 },
			{ 216 },
			{ 235 },
			{ 257 },
			{ 298 },
			{ 310 },
			{ 328 },
			{ 333 },
			{ 358 },
			{ 421 }
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
//G243: z3(117)              => variable(139).
	p(NT(117), (NT(139)));
//G244: z3_neg(169)          => z3(117).
	p(NT(169), (NT(117)));
//G245: z3(117)              => '~' _(11) z3_neg(169).
	p(NT(117), (T(41)+NT(11)+NT(169)));
//G246: __E_z3_70(171)       => z3(117) _(11) '+' _(11) z3(117).
	p(NT(171), (NT(117)+NT(11)+T(37)+NT(11)+NT(117)));
//G247: z3_add(170)          => __E_z3_70(171).
	p(NT(170), (NT(171)));
//G248: z3(117)              => z3_add(170).
	p(NT(117), (NT(170)));
//G249: __E_z3_71(173)       => z3(117) _(11) '-' _(11) z3(117).
	p(NT(173), (NT(117)+NT(11)+T(28)+NT(11)+NT(117)));
//G250: z3_sub(172)          => __E_z3_71(173).
	p(NT(172), (NT(173)));
//G251: z3(117)              => z3_sub(172).
	p(NT(117), (NT(172)));
//G252: __E_z3_72(175)       => z3(117) _(11) '*' _(11) z3(117).
	p(NT(175), (NT(117)+NT(11)+T(42)+NT(11)+NT(117)));
//G253: z3_mul(174)          => __E_z3_72(175).
	p(NT(174), (NT(175)));
//G254: z3(117)              => z3_mul(174).
	p(NT(117), (NT(174)));
//G255: __E_z3_73(177)       => z3(117) _(11) '/' _(11) z3(117).
	p(NT(177), (NT(117)+NT(11)+T(43)+NT(11)+NT(117)));
//G256: z3_div(176)          => __E_z3_73(177).
	p(NT(176), (NT(177)));
//G257: z3(117)              => z3_div(176).
	p(NT(117), (NT(176)));
//G258: __E_z3_74(179)       => z3(117) _(11) '%' _(11) z3(117).
	p(NT(179), (NT(117)+NT(11)+T(44)+NT(11)+NT(117)));
//G259: z3_mod(178)          => __E_z3_74(179).
	p(NT(178), (NT(179)));
//G260: z3(117)              => z3_mod(178).
	p(NT(117), (NT(178)));
//G261: __E_z3_75(181)       => z3(117) _(11) '&' _(11) z3(117).
	p(NT(181), (NT(117)+NT(11)+T(31)+NT(11)+NT(117)));
//G262: z3_and(180)          => __E_z3_75(181).
	p(NT(180), (NT(181)));
//G263: z3(117)              => z3_and(180).
	p(NT(117), (NT(180)));
//G264: __E_z3_76(183)       => z3(117) _(11) '|' _(11) z3(117).
	p(NT(183), (NT(117)+NT(11)+T(29)+NT(11)+NT(117)));
//G265: z3_or(182)           => __E_z3_76(183).
	p(NT(182), (NT(183)));
//G266: z3(117)              => z3_or(182).
	p(NT(117), (NT(182)));
//G267: __E_z3_77(185)       => z3(117) _(11) '(' '+' ')' _(11) z3(117).
	p(NT(185), (NT(117)+NT(11)+T(15)+T(37)+T(16)+NT(11)+NT(117)));
//G268: z3_xor(184)          => __E_z3_77(185).
	p(NT(184), (NT(185)));
//G269: z3(117)              => z3_xor(184).
	p(NT(117), (NT(184)));
//G270: __E_z3_78(187)       => z3(117) _(11) '<' '<' _(11) num(188).
	p(NT(187), (NT(117)+NT(11)+T(20)+T(20)+NT(11)+NT(188)));
//G271: z3_shift_left(186)   => __E_z3_78(187).
	p(NT(186), (NT(187)));
//G272: z3(117)              => z3_shift_left(186).
	p(NT(117), (NT(186)));
//G273: __E_z3_79(190)       => z3(117) _(11) '>' '>' _(11) num(188).
	p(NT(190), (NT(117)+NT(11)+T(21)+T(21)+NT(11)+NT(188)));
//G274: z3_shift_right(189)  => __E_z3_79(190).
	p(NT(189), (NT(190)));
//G275: z3(117)              => z3_shift_right(189).
	p(NT(117), (NT(189)));
//G276: z3(117)              => bitvector(191).
	p(NT(117), (NT(191)));
//G277: __E_bitvector_80(193) => _(11) bitvector_type(194).
	p(NT(193), (NT(11)+NT(194)));
//G278: __E_bitvector_80(193) => null.
	p(NT(193), (nul));
//G279: bitvector(191)       => bits(192) __E_bitvector_80(193).
	p(NT(191), (NT(192)+NT(193)));
//G280: __E_bitvector_81(195) => _(11) bitvector_type(194).
	p(NT(195), (NT(11)+NT(194)));
//G281: __E_bitvector_81(195) => null.
	p(NT(195), (nul));
//G282: bitvector(191)       => num(188) __E_bitvector_81(195).
	p(NT(191), (NT(188)+NT(195)));
//G283: __E_bitvector_type_82(196) => _(11) ':' _(11) '[' _(11) num(188) _(11) ']'.
	p(NT(196), (NT(11)+T(2)+NT(11)+T(24)+NT(11)+NT(188)+NT(11)+T(25)));
//G284: type(150)            => __E_bitvector_type_82(196).
	p(NT(150), (NT(196)));
//G285: bitvector_type(194)  => type(150).
	p(NT(194), (NT(150)));
//G286: __E_bits_83(198)     => bit(197).
	p(NT(198), (NT(197)));
//G287: __E_bits_84(199)     => null.
	p(NT(199), (nul));
//G288: __E_bits_84(199)     => __E_bits_83(198) __E_bits_84(199).
	p(NT(199), (NT(198)+NT(199)));
//G289: bits(192)            => bit(197) __E_bits_84(199) 'b'.
	p(NT(192), (NT(197)+NT(199)+T(7)));
//G290: __E_constraint_85(201) => '[' ctnvar(202) _(11) '!' '=' _(11) num(188) ']'.
	p(NT(201), (T(24)+NT(202)+NT(11)+T(32)+T(3)+NT(11)+NT(188)+T(25)));
//G291: __E_constraint_85(201) => '[' num(188) _(11) '!' '=' _(11) ctnvar(202) ']'.
	p(NT(201), (T(24)+NT(188)+NT(11)+T(32)+T(3)+NT(11)+NT(202)+T(25)));
//G292: ctn_neq(200)         => __E_constraint_85(201).
	p(NT(200), (NT(201)));
//G293: constraint(76)       => ctn_neq(200).
	p(NT(76), (NT(200)));
//G294: __E_constraint_86(204) => '[' ctnvar(202) _(11) '=' _(11) num(188) ']'.
	p(NT(204), (T(24)+NT(202)+NT(11)+T(3)+NT(11)+NT(188)+T(25)));
//G295: __E_constraint_86(204) => '[' num(188) _(11) '=' _(11) ctnvar(202) ']'.
	p(NT(204), (T(24)+NT(188)+NT(11)+T(3)+NT(11)+NT(202)+T(25)));
//G296: ctn_eq(203)          => __E_constraint_86(204).
	p(NT(203), (NT(204)));
//G297: constraint(76)       => ctn_eq(203).
	p(NT(76), (NT(203)));
//G298: __E_constraint_87(206) => '[' ctnvar(202) _(11) '>' '=' _(11) num(188) ']'.
	p(NT(206), (T(24)+NT(202)+NT(11)+T(21)+T(3)+NT(11)+NT(188)+T(25)));
//G299: __E_constraint_87(206) => '[' num(188) _(11) '>' '=' _(11) ctnvar(202) ']'.
	p(NT(206), (T(24)+NT(188)+NT(11)+T(21)+T(3)+NT(11)+NT(202)+T(25)));
//G300: ctn_greater_equal(205) => __E_constraint_87(206).
	p(NT(205), (NT(206)));
//G301: constraint(76)       => ctn_greater_equal(205).
	p(NT(76), (NT(205)));
//G302: __E_constraint_88(208) => '[' ctnvar(202) _(11) '>' _(11) num(188) ']'.
	p(NT(208), (T(24)+NT(202)+NT(11)+T(21)+NT(11)+NT(188)+T(25)));
//G303: __E_constraint_88(208) => '[' num(188) _(11) '>' _(11) ctnvar(202) ']'.
	p(NT(208), (T(24)+NT(188)+NT(11)+T(21)+NT(11)+NT(202)+T(25)));
//G304: ctn_greater(207)     => __E_constraint_88(208).
	p(NT(207), (NT(208)));
//G305: constraint(76)       => ctn_greater(207).
	p(NT(76), (NT(207)));
//G306: __E_constraint_89(210) => '[' ctnvar(202) _(11) '<' '=' _(11) num(188) ']'.
	p(NT(210), (T(24)+NT(202)+NT(11)+T(20)+T(3)+NT(11)+NT(188)+T(25)));
//G307: __E_constraint_89(210) => '[' num(188) _(11) '<' '=' _(11) ctnvar(202) ']'.
	p(NT(210), (T(24)+NT(188)+NT(11)+T(20)+T(3)+NT(11)+NT(202)+T(25)));
//G308: ctn_less_equal(209)  => __E_constraint_89(210).
	p(NT(209), (NT(210)));
//G309: constraint(76)       => ctn_less_equal(209).
	p(NT(76), (NT(209)));
//G310: __E_constraint_90(212) => '[' ctnvar(202) _(11) '<' _(11) num(188) ']'.
	p(NT(212), (T(24)+NT(202)+NT(11)+T(20)+NT(11)+NT(188)+T(25)));
//G311: __E_constraint_90(212) => '[' num(188) _(11) '<' _(11) ctnvar(202) ']'.
	p(NT(212), (T(24)+NT(188)+NT(11)+T(20)+NT(11)+NT(202)+T(25)));
//G312: ctn_less(211)        => __E_constraint_90(212).
	p(NT(211), (NT(212)));
//G313: constraint(76)       => ctn_less(211).
	p(NT(76), (NT(211)));
//G314: __N_0(436)           => 'F'.
	p(NT(436), (T(34)));
//G315: __N_1(437)           => 'T'.
	p(NT(437), (T(33)));
//G316: __E_variable_91(213) => ~( __N_0(436) ) & ~( __N_1(437) ) & alpha(5).	 # conjunctive
	p(NT(213), ~(NT(436)) & ~(NT(437)) & (NT(5)));
//G317: __E_variable_92(214) => null.
	p(NT(214), (nul));
//G318: __E_variable_92(214) => digit(3) __E_variable_92(214).
	p(NT(214), (NT(3)+NT(214)));
//G319: __E_variable_93(215) => _(11) ':' _(11) chars(216).
	p(NT(215), (NT(11)+T(2)+NT(11)+NT(216)));
//G320: __E_variable_93(215) => null.
	p(NT(215), (nul));
//G321: variable(139)        => __E_variable_91(213) __E_variable_92(214) __E_variable_93(215).	 # guarded: charvar
	p(NT(139), (NT(213)+NT(214)+NT(215)));
	p.back().guard = "charvar";
//G322: __N_2(438)           => 'F'.
	p(NT(438), (T(34)));
//G323: __N_3(439)           => 'T'.
	p(NT(439), (T(33)));
//G324: __E_variable_94(217) => ~( __N_2(438) ) & ~( __N_3(439) ) & alpha(5).	 # conjunctive
	p(NT(217), ~(NT(438)) & ~(NT(439)) & (NT(5)));
//G325: __E_variable_95(218) => alnum(6).
	p(NT(218), (NT(6)));
//G326: __E_variable_95(218) => '_'.
	p(NT(218), (T(35)));
//G327: __E_variable_96(219) => null.
	p(NT(219), (nul));
//G328: __E_variable_96(219) => __E_variable_95(218) __E_variable_96(219).
	p(NT(219), (NT(218)+NT(219)));
//G329: __E_variable_97(220) => _(11) ':' _(11) chars(216).
	p(NT(220), (NT(11)+T(2)+NT(11)+NT(216)));
//G330: __E_variable_97(220) => null.
	p(NT(220), (nul));
//G331: variable(139)        => __E_variable_94(217) __E_variable_96(219) __E_variable_97(220).	 # guarded: var
	p(NT(139), (NT(217)+NT(219)+NT(220)));
	p.back().guard = "var";
//G332: __E_variable_98(222) => in(223).
	p(NT(222), (NT(223)));
//G333: __E_variable_98(222) => out(224).
	p(NT(222), (NT(224)));
//G334: io_var(221)          => __E_variable_98(222).
	p(NT(221), (NT(222)));
//G335: __E_variable_99(225) => _(11) ':' _(11) chars(216).
	p(NT(225), (NT(11)+T(2)+NT(11)+NT(216)));
//G336: __E_variable_99(225) => null.
	p(NT(225), (nul));
//G337: variable(139)        => io_var(221) __E_variable_99(225).
	p(NT(139), (NT(221)+NT(225)));
//G338: variable(139)        => uninterpreted_constant(226).
	p(NT(139), (NT(226)));
//G339: __E___E_in_100_101(229) => digit(3).
	p(NT(229), (NT(3)));
//G340: __E___E_in_100_101(229) => digit(3) __E___E_in_100_101(229).
	p(NT(229), (NT(3)+NT(229)));
//G341: __E_in_100(228)      => 'i' __E___E_in_100_101(229).
	p(NT(228), (T(10)+NT(229)));
//G342: in_var_name(227)     => __E_in_100(228).
	p(NT(227), (NT(228)));
//G343: in(223)              => in_var_name(227) '[' offset(230) ']'.
	p(NT(223), (NT(227)+T(24)+NT(230)+T(25)));
//G344: __E___E___E_out_102_103_104(234) => digit(3).
	p(NT(234), (NT(3)));
//G345: __E___E___E_out_102_103_104(234) => digit(3) __E___E___E_out_102_103_104(234).
	p(NT(234), (NT(3)+NT(234)));
//G346: __E___E_out_102_103(233) => 'o' __E___E___E_out_102_103_104(234).
	p(NT(233), (T(17)+NT(234)));
//G347: __E_out_102(232)     => __E___E_out_102_103(233).
	p(NT(232), (NT(233)));
//G348: __E_out_102(232)     => 'u'.
	p(NT(232), (T(45)));
//G349: out_var_name(231)    => __E_out_102(232).
	p(NT(231), (NT(232)));
//G350: out(224)             => out_var_name(231) '[' offset(230) ']'.
	p(NT(224), (NT(231)+T(24)+NT(230)+T(25)));
//G351: ctnvar(202)          => chars(216).
	p(NT(202), (NT(216)));
//G352: capture(20)          => '$' chars(216).
	p(NT(20), (T(46)+NT(216)));
//G353: __E_q_vars_105(236)  => _(11) ',' _(11) q_var(235).
	p(NT(236), (NT(11)+T(14)+NT(11)+NT(235)));
//G354: __E_q_vars_106(237)  => null.
	p(NT(237), (nul));
//G355: __E_q_vars_106(237)  => __E_q_vars_105(236) __E_q_vars_106(237).
	p(NT(237), (NT(236)+NT(237)));
//G356: q_vars(72)           => q_var(235) __E_q_vars_106(237).
	p(NT(72), (NT(235)+NT(237)));
//G357: q_var(235)           => capture(20).
	p(NT(235), (NT(20)));
//G358: __E_q_var_107(238)   => _(11) ':' _(11) chars(216).
	p(NT(238), (NT(11)+T(2)+NT(11)+NT(216)));
//G359: __E_q_var_107(238)   => null.
	p(NT(238), (nul));
//G360: __N_4(440)           => uninterpreted_constant(226) __E_q_var_107(238).
	p(NT(440), (NT(226)+NT(238)));
//G361: q_var(235)           => variable(139) & ~( __N_4(440) ).	 # conjunctive
	p(NT(235), (NT(139)) & ~(NT(440)));
//G362: __E_offsets_108(239) => _(11) ',' _(11) offset(230).
	p(NT(239), (NT(11)+T(14)+NT(11)+NT(230)));
//G363: __E_offsets_109(240) => null.
	p(NT(240), (nul));
//G364: __E_offsets_109(240) => __E_offsets_108(239) __E_offsets_109(240).
	p(NT(240), (NT(239)+NT(240)));
//G365: offsets(24)          => '[' _(11) offset(230) __E_offsets_109(240) _(11) ']'.
	p(NT(24), (T(24)+NT(11)+NT(230)+NT(240)+NT(11)+T(25)));
//G366: offset(230)          => integer(241).
	p(NT(230), (NT(241)));
//G367: offset(230)          => capture(20).
	p(NT(230), (NT(20)));
//G368: offset(230)          => shift(242).
	p(NT(230), (NT(242)));
//G369: __N_5(441)           => io_var(221).
	p(NT(441), (NT(221)));
//G370: __E_offset_110(243)  => variable(139) & ~( __N_5(441) ).	 # conjunctive
	p(NT(243), (NT(139)) & ~(NT(441)));
//G371: offset(230)          => __E_offset_110(243).
	p(NT(230), (NT(243)));
//G372: __E_shift_111(244)   => capture(20).
	p(NT(244), (NT(20)));
//G373: __N_6(442)           => io_var(221).
	p(NT(442), (NT(221)));
//G374: __E___E_shift_111_112(245) => variable(139) & ~( __N_6(442) ).	 # conjunctive
	p(NT(245), (NT(139)) & ~(NT(442)));
//G375: __E_shift_111(244)   => __E___E_shift_111_112(245).
	p(NT(244), (NT(245)));
//G376: shift(242)           => __E_shift_111(244) _(11) '-' _(11) num(188).
	p(NT(242), (NT(244)+NT(11)+T(28)+NT(11)+NT(188)));
//G377: num(188)             => digits(246).
	p(NT(188), (NT(246)));
//G378: __E_integer_113(247) => '-'.
	p(NT(247), (T(28)));
//G379: __E_integer_113(247) => null.
	p(NT(247), (nul));
//G380: integer(241)         => __E_integer_113(247) _(11) digits(246).
	p(NT(241), (NT(247)+NT(11)+NT(246)));
//G381: __E___E_uninterpreted_constant_114_115(250) => chars(216).
	p(NT(250), (NT(216)));
//G382: __E___E_uninterpreted_constant_114_115(250) => _(11).
	p(NT(250), (NT(11)));
//G383: __E_uninterpreted_constant_114(249) => __E___E_uninterpreted_constant_114_115(250) ':' chars(216).
	p(NT(249), (NT(250)+T(2)+NT(216)));
//G384: uninter_const_name(248) => __E_uninterpreted_constant_114(249).
	p(NT(248), (NT(249)));
//G385: uninterpreted_constant(226) => '<' uninter_const_name(248) _(11) '>'.
	p(NT(226), (T(20)+NT(248)+NT(11)+T(21)));
//G386: __E_bf_constant_116(252) => capture(20).
	p(NT(252), (NT(20)));
//G387: __E_bf_constant_116(252) => binding(253).
	p(NT(252), (NT(253)));
//G388: constant(251)        => __E_bf_constant_116(252).
	p(NT(251), (NT(252)));
//G389: __E_bf_constant_117(254) => _(11) ':' _(11) type(150).
	p(NT(254), (NT(11)+T(2)+NT(11)+NT(150)));
//G390: __E_bf_constant_117(254) => null.
	p(NT(254), (nul));
//G391: bf_constant(138)     => '{' _(11) constant(251) _(11) '}' __E_bf_constant_117(254).
	p(NT(138), (T(47)+NT(11)+NT(251)+NT(11)+T(48)+NT(254)));
//G392: __E___E___E_binding_118_119_120(259) => src_c(257).
	p(NT(259), (NT(257)));
//G393: __E___E___E_binding_118_119_120(259) => space(2).
	p(NT(259), (NT(2)));
//G394: __E___E___E_binding_118_119_121(260) => null.
	p(NT(260), (nul));
//G395: __E___E___E_binding_118_119_121(260) => __E___E___E_binding_118_119_120(259) __E___E___E_binding_118_119_121(260).
	p(NT(260), (NT(259)+NT(260)));
//G396: __E___E_binding_118_119(258) => __E___E___E_binding_118_119_121(260) src_c(257).
	p(NT(258), (NT(260)+NT(257)));
//G397: __E___E_binding_118_119(258) => null.
	p(NT(258), (nul));
//G398: __E_binding_118(256) => src_c(257) __E___E_binding_118_119(258).
	p(NT(256), (NT(257)+NT(258)));
//G399: source(255)          => __E_binding_118(256).
	p(NT(255), (NT(256)));
//G400: binding(253)         => source(255).
	p(NT(253), (NT(255)));
//G401: src_c(257)           => alnum(6).
	p(NT(257), (NT(6)));
//G402: __N_7(443)           => '{'.
	p(NT(443), (T(47)));
//G403: __N_8(444)           => '}'.
	p(NT(444), (T(48)));
//G404: src_c(257)           => ~( __N_7(443) ) & ~( __N_8(444) ) & punct(7).	 # conjunctive
	p(NT(257), ~(NT(443)) & ~(NT(444)) & (NT(7)));
//G405: __E_src_c_122(261)   => src_c(257).
	p(NT(261), (NT(257)));
//G406: __E_src_c_122(261)   => space(2).
	p(NT(261), (NT(2)));
//G407: __E_src_c_123(262)   => null.
	p(NT(262), (nul));
//G408: __E_src_c_123(262)   => __E_src_c_122(261) __E_src_c_123(262).
	p(NT(262), (NT(261)+NT(262)));
//G409: src_c(257)           => '{' __E_src_c_123(262) '}'.
	p(NT(257), (T(47)+NT(262)+T(48)));
//G410: type(150)            => chars(216).
	p(NT(150), (NT(216)));
//G411: type(150)            => chars(216) _(11) '[' _(11) num(188) _(11) ']'.
	p(NT(150), (NT(216)+NT(11)+T(24)+NT(11)+NT(188)+NT(11)+T(25)));
//G412: __E_chars_124(263)   => alnum(6).
	p(NT(263), (NT(6)));
//G413: __E_chars_124(263)   => '_'.
	p(NT(263), (T(35)));
//G414: __E_chars_125(264)   => null.
	p(NT(264), (nul));
//G415: __E_chars_125(264)   => __E_chars_124(263) __E_chars_125(264).
	p(NT(264), (NT(263)+NT(264)));
//G416: chars(216)           => alpha(5) __E_chars_125(264).
	p(NT(216), (NT(5)+NT(264)));
//G417: __E_digits_126(265)  => digit(3).
	p(NT(265), (NT(3)));
//G418: __E_digits_126(265)  => digit(3) __E_digits_126(265).
	p(NT(265), (NT(3)+NT(265)));
//G419: digits(246)          => __E_digits_126(265).
	p(NT(246), (NT(265)));
//G420: sym(22)              => chars(216).
	p(NT(22), (NT(216)));
//G421: __E_comment_127(267) => printable(8).
	p(NT(267), (NT(8)));
//G422: __E_comment_127(267) => '\t'.
	p(NT(267), (T(49)));
//G423: __E_comment_128(268) => null.
	p(NT(268), (nul));
//G424: __E_comment_128(268) => __E_comment_127(267) __E_comment_128(268).
	p(NT(268), (NT(267)+NT(268)));
//G425: __E_comment_129(269) => '\n'.
	p(NT(269), (T(50)));
//G426: __E_comment_129(269) => '\r'.
	p(NT(269), (T(51)));
//G427: __E_comment_129(269) => eof(1).
	p(NT(269), (NT(1)));
//G428: comment(266)         => '#' __E_comment_128(268) __E_comment_129(269).
	p(NT(266), (T(52)+NT(268)+NT(269)));
//G429: __E____130(270)      => space(2).
	p(NT(270), (NT(2)));
//G430: __E____130(270)      => comment(266).
	p(NT(270), (NT(266)));
//G431: __(27)               => __E____130(270) _(11).
	p(NT(27), (NT(270)+NT(11)));
//G432: __E___131(271)       => __(27).
	p(NT(271), (NT(27)));
//G433: __E___131(271)       => null.
	p(NT(271), (nul));
//G434: _(11)                => __E___131(271).
	p(NT(11), (NT(271)));
//G435: cli(272)             => _(11).
	p(NT(272), (NT(11)));
//G436: __E_cli_132(274)     => '.' _(11) cli_command(273) _(11).
	p(NT(274), (T(1)+NT(11)+NT(273)+NT(11)));
//G437: __E_cli_133(275)     => null.
	p(NT(275), (nul));
//G438: __E_cli_133(275)     => __E_cli_132(274) __E_cli_133(275).
	p(NT(275), (NT(274)+NT(275)));
//G439: cli(272)             => _(11) cli_command(273) _(11) __E_cli_133(275).
	p(NT(272), (NT(11)+NT(273)+NT(11)+NT(275)));
//G440: __E_cli_command_134(278) => 'q'.
	p(NT(278), (T(53)));
//G441: __E_cli_command_134(278) => 'q' 'u' 'i' 't'.
	p(NT(278), (T(53)+T(45)+T(10)+T(13)));
//G442: quit_sym(277)        => __E_cli_command_134(278).
	p(NT(277), (NT(278)));
//G443: quit_cmd(276)        => quit_sym(277).
	p(NT(276), (NT(277)));
//G444: cli_command(273)     => quit_cmd(276).
	p(NT(273), (NT(276)));
//G445: __E_cli_command_135(281) => 'v'.
	p(NT(281), (T(54)));
//G446: __E_cli_command_135(281) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
	p(NT(281), (T(54)+T(19)+T(11)+T(12)+T(10)+T(17)+T(55)));
//G447: version_sym(280)     => __E_cli_command_135(281).
	p(NT(280), (NT(281)));
//G448: version_cmd(279)     => version_sym(280).
	p(NT(279), (NT(280)));
//G449: cli_command(273)     => version_cmd(279).
	p(NT(273), (NT(279)));
//G450: __E_cli_command_136(284) => 'c'.
	p(NT(284), (T(8)));
//G451: __E_cli_command_136(284) => 'c' 'l' 'e' 'a' 'r'.
	p(NT(284), (T(8)+T(6)+T(19)+T(5)+T(11)));
//G452: clear_sym(283)       => __E_cli_command_136(284).
	p(NT(283), (NT(284)));
//G453: clear_cmd(282)       => clear_sym(283).
	p(NT(282), (NT(283)));
//G454: cli_command(273)     => clear_cmd(282).
	p(NT(273), (NT(282)));
//G455: __E___E_cli_command_137_138(288) => 'h'.
	p(NT(288), (T(56)));
//G456: __E___E_cli_command_137_138(288) => 'h' 'e' 'l' 'p'.
	p(NT(288), (T(56)+T(19)+T(6)+T(57)));
//G457: help_sym(287)        => __E___E_cli_command_137_138(288).
	p(NT(287), (NT(288)));
//G458: __E___E_cli_command_137_139(289) => __(27) help_arg(290).
	p(NT(289), (NT(27)+NT(290)));
//G459: __E___E_cli_command_137_139(289) => null.
	p(NT(289), (nul));
//G460: __E_cli_command_137(286) => help_sym(287) __E___E_cli_command_137_139(289).
	p(NT(286), (NT(287)+NT(289)));
//G461: help_cmd(285)        => __E_cli_command_137(286).
	p(NT(285), (NT(286)));
//G462: cli_command(273)     => help_cmd(285).
	p(NT(273), (NT(285)));
//G463: file_sym(293)        => 'f' 'i' 'l' 'e'.
	p(NT(293), (T(4)+T(10)+T(6)+T(19)));
//G464: __E_cli_command_140(292) => file_sym(293) __(27) q_string(294).
	p(NT(292), (NT(293)+NT(27)+NT(294)));
//G465: file_cmd(291)        => __E_cli_command_140(292).
	p(NT(291), (NT(292)));
//G466: cli_command(273)     => file_cmd(291).
	p(NT(273), (NT(291)));
//G467: valid_sym(297)       => 'v' 'a' 'l' 'i' 'd'.
	p(NT(297), (T(54)+T(5)+T(6)+T(10)+T(58)));
//G468: __E_cli_command_141(296) => valid_sym(297) __(27) normalize_cmd_arg(298).
	p(NT(296), (NT(297)+NT(27)+NT(298)));
//G469: valid_cmd(295)       => __E_cli_command_141(296).
	p(NT(295), (NT(296)));
//G470: cli_command(273)     => valid_cmd(295).
	p(NT(273), (NT(295)));
//G471: sat_sym(301)         => 's' 'a' 't'.
	p(NT(301), (T(12)+T(5)+T(13)));
//G472: __E_cli_command_142(300) => sat_sym(301) __(27) normalize_cmd_arg(298).
	p(NT(300), (NT(301)+NT(27)+NT(298)));
//G473: sat_cmd(299)         => __E_cli_command_142(300).
	p(NT(299), (NT(300)));
//G474: cli_command(273)     => sat_cmd(299).
	p(NT(273), (NT(299)));
//G475: unsat_sym(304)       => 'u' 'n' 's' 'a' 't'.
	p(NT(304), (T(45)+T(55)+T(12)+T(5)+T(13)));
//G476: __E_cli_command_143(303) => unsat_sym(304) __(27) normalize_cmd_arg(298).
	p(NT(303), (NT(304)+NT(27)+NT(298)));
//G477: unsat_cmd(302)       => __E_cli_command_143(303).
	p(NT(302), (NT(303)));
//G478: cli_command(273)     => unsat_cmd(302).
	p(NT(273), (NT(302)));
//G479: solve_sym(307)       => 's' 'o' 'l' 'v' 'e'.
	p(NT(307), (T(12)+T(17)+T(6)+T(54)+T(19)));
//G480: __E___E_cli_command_144_145(308) => solve_options(309).
	p(NT(308), (NT(309)));
//G481: __E___E_cli_command_144_145(308) => null.
	p(NT(308), (nul));
//G482: __E_cli_command_144(306) => solve_sym(307) __E___E_cli_command_144_145(308) __(27) wff_cmd_arg(310).
	p(NT(306), (NT(307)+NT(308)+NT(27)+NT(310)));
//G483: solve_cmd(305)       => __E_cli_command_144(306).
	p(NT(305), (NT(306)));
//G484: cli_command(273)     => solve_cmd(305).
	p(NT(273), (NT(305)));
//G485: lgrs_sym(313)        => 'l' 'g' 'r' 's'.
	p(NT(313), (T(6)+T(59)+T(11)+T(12)));
//G486: __E_cli_command_146(312) => lgrs_sym(313) __(27) wff_cmd_arg(310).
	p(NT(312), (NT(313)+NT(27)+NT(310)));
//G487: lgrs_cmd(311)        => __E_cli_command_146(312).
	p(NT(311), (NT(312)));
//G488: cli_command(273)     => lgrs_cmd(311).
	p(NT(273), (NT(311)));
//G489: __E___E_cli_command_147_148(317) => 'r'.
	p(NT(317), (T(11)));
//G490: __E___E_cli_command_147_148(317) => 'r' 'u' 'n'.
	p(NT(317), (T(11)+T(45)+T(55)));
//G491: run_sym(316)         => __E___E_cli_command_147_148(317).
	p(NT(316), (NT(317)));
//G492: __E___E_cli_command_147_149(318) => memory(319).
	p(NT(318), (NT(319)));
//G493: __E___E_cli_command_147_149(318) => wff(14).
	p(NT(318), (NT(14)));
//G494: __E_cli_command_147(315) => run_sym(316) __(27) __E___E_cli_command_147_149(318).
	p(NT(315), (NT(316)+NT(27)+NT(318)));
//G495: run_cmd(314)         => __E_cli_command_147(315).
	p(NT(314), (NT(315)));
//G496: cli_command(273)     => run_cmd(314).
	p(NT(273), (NT(314)));
//G497: __E___E_cli_command_150_151(323) => 'n'.
	p(NT(323), (T(55)));
//G498: __E___E_cli_command_150_151(323) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
	p(NT(323), (T(55)+T(17)+T(11)+T(18)+T(5)+T(6)+T(10)+T(60)+T(19)));
//G499: normalize_sym(322)   => __E___E_cli_command_150_151(323).
	p(NT(322), (NT(323)));
//G500: __E_cli_command_150(321) => normalize_sym(322) __(27) normalize_cmd_arg(298).
	p(NT(321), (NT(322)+NT(27)+NT(298)));
//G501: normalize_cmd(320)   => __E_cli_command_150(321).
	p(NT(320), (NT(321)));
//G502: cli_command(273)     => normalize_cmd(320).
	p(NT(273), (NT(320)));
//G503: __E___E_cli_command_152_153(327) => 's'.
	p(NT(327), (T(12)));
//G504: __E___E_cli_command_152_153(327) => 's' 'u' 'b' 's' 't'.
	p(NT(327), (T(12)+T(45)+T(7)+T(12)+T(13)));
//G505: __E___E_cli_command_152_153(327) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
	p(NT(327), (T(12)+T(45)+T(7)+T(12)+T(13)+T(10)+T(13)+T(45)+T(13)+T(19)));
//G506: subst_sym(326)       => __E___E_cli_command_152_153(327).
	p(NT(326), (NT(327)));
//G507: __E_cli_command_152(325) => subst_sym(326) __(27) nf_cmd_arg(328) _(11) '[' _(11) nf_cmd_arg(328) _(11) '/' _(11) nf_cmd_arg(328) _(11) ']'.
	p(NT(325), (NT(326)+NT(27)+NT(328)+NT(11)+T(24)+NT(11)+NT(328)+NT(11)+T(43)+NT(11)+NT(328)+NT(11)+T(25)));
//G508: subst_cmd(324)       => __E_cli_command_152(325).
	p(NT(324), (NT(325)));
//G509: cli_command(273)     => subst_cmd(324).
	p(NT(273), (NT(324)));
//G510: __E___E_cli_command_154_155(332) => 'i'.
	p(NT(332), (T(10)));
//G511: __E___E_cli_command_154_155(332) => 'i' 'n' 's' 't'.
	p(NT(332), (T(10)+T(55)+T(12)+T(13)));
//G512: __E___E_cli_command_154_155(332) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
	p(NT(332), (T(10)+T(55)+T(12)+T(13)+T(5)+T(55)+T(13)+T(10)+T(5)+T(13)+T(19)));
//G513: inst_sym(331)        => __E___E_cli_command_154_155(332).
	p(NT(331), (NT(332)));
//G514: __E_cli_command_154(330) => inst_sym(331) __(27) inst_args(333).
	p(NT(330), (NT(331)+NT(27)+NT(333)));
//G515: inst_cmd(329)        => __E_cli_command_154(330).
	p(NT(329), (NT(330)));
//G516: cli_command(273)     => inst_cmd(329).
	p(NT(273), (NT(329)));
//G517: dnf_sym(336)         => 'd' 'n' 'f'.
	p(NT(336), (T(58)+T(55)+T(4)));
//G518: __E_cli_command_156(335) => dnf_sym(336) __(27) nf_cmd_arg(328).
	p(NT(335), (NT(336)+NT(27)+NT(328)));
//G519: dnf_cmd(334)         => __E_cli_command_156(335).
	p(NT(334), (NT(335)));
//G520: cli_command(273)     => dnf_cmd(334).
	p(NT(273), (NT(334)));
//G521: cnf_sym(339)         => 'c' 'n' 'f'.
	p(NT(339), (T(8)+T(55)+T(4)));
//G522: __E_cli_command_157(338) => cnf_sym(339) __(27) nf_cmd_arg(328).
	p(NT(338), (NT(339)+NT(27)+NT(328)));
//G523: cnf_cmd(337)         => __E_cli_command_157(338).
	p(NT(337), (NT(338)));
//G524: cli_command(273)     => cnf_cmd(337).
	p(NT(273), (NT(337)));
//G525: anf_sym(342)         => 'a' 'n' 'f'.
	p(NT(342), (T(5)+T(55)+T(4)));
//G526: __E_cli_command_158(341) => anf_sym(342) __(27) nf_cmd_arg(328).
	p(NT(341), (NT(342)+NT(27)+NT(328)));
//G527: anf_cmd(340)         => __E_cli_command_158(341).
	p(NT(340), (NT(341)));
//G528: cli_command(273)     => anf_cmd(340).
	p(NT(273), (NT(340)));
//G529: nnf_sym(345)         => 'n' 'n' 'f'.
	p(NT(345), (T(55)+T(55)+T(4)));
//G530: __E_cli_command_159(344) => nnf_sym(345) __(27) nf_cmd_arg(328).
	p(NT(344), (NT(345)+NT(27)+NT(328)));
//G531: nnf_cmd(343)         => __E_cli_command_159(344).
	p(NT(343), (NT(344)));
//G532: cli_command(273)     => nnf_cmd(343).
	p(NT(273), (NT(343)));
//G533: pnf_sym(348)         => 'p' 'n' 'f'.
	p(NT(348), (T(57)+T(55)+T(4)));
//G534: __E_cli_command_160(347) => pnf_sym(348) __(27) nf_cmd_arg(328).
	p(NT(347), (NT(348)+NT(27)+NT(328)));
//G535: pnf_cmd(346)         => __E_cli_command_160(347).
	p(NT(346), (NT(347)));
//G536: cli_command(273)     => pnf_cmd(346).
	p(NT(273), (NT(346)));
//G537: mnf_sym(351)         => 'm' 'n' 'f'.
	p(NT(351), (T(18)+T(55)+T(4)));
//G538: __E_cli_command_161(350) => mnf_sym(351) __(27) nf_cmd_arg(328).
	p(NT(350), (NT(351)+NT(27)+NT(328)));
//G539: mnf_cmd(349)         => __E_cli_command_161(350).
	p(NT(349), (NT(350)));
//G540: cli_command(273)     => mnf_cmd(349).
	p(NT(273), (NT(349)));
//G541: snf_sym(354)         => 's' 'n' 'f'.
	p(NT(354), (T(12)+T(55)+T(4)));
//G542: __E_cli_command_162(353) => snf_sym(354) __(27) nf_cmd_arg(328).
	p(NT(353), (NT(354)+NT(27)+NT(328)));
//G543: snf_cmd(352)         => __E_cli_command_162(353).
	p(NT(352), (NT(353)));
//G544: cli_command(273)     => snf_cmd(352).
	p(NT(273), (NT(352)));
//G545: onf_sym(357)         => 'o' 'n' 'f'.
	p(NT(357), (T(17)+T(55)+T(4)));
//G546: __E_cli_command_163(356) => onf_sym(357) __(27) variable(139) __(27) onf_cmd_arg(358).
	p(NT(356), (NT(357)+NT(27)+NT(139)+NT(27)+NT(358)));
//G547: onf_cmd(355)         => __E_cli_command_163(356).
	p(NT(355), (NT(356)));
//G548: cli_command(273)     => onf_cmd(355).
	p(NT(273), (NT(355)));
//G549: __E___E_cli_command_164_165(362) => 'd' 'e' 'f' 's'.
	p(NT(362), (T(58)+T(19)+T(4)+T(12)));
//G550: __E___E_cli_command_164_165(362) => 'd' 'e' 'f' 'i' 'n' 'i' 't' 'i' 'o' 'n' 's'.
	p(NT(362), (T(58)+T(19)+T(4)+T(10)+T(55)+T(10)+T(13)+T(10)+T(17)+T(55)+T(12)));
//G551: def_sym(361)         => __E___E_cli_command_164_165(362).
	p(NT(361), (NT(362)));
//G552: __E_cli_command_164(360) => def_sym(361).
	p(NT(360), (NT(361)));
//G553: def_list_cmd(359)    => __E_cli_command_164(360).
	p(NT(359), (NT(360)));
//G554: cli_command(273)     => def_list_cmd(359).
	p(NT(273), (NT(359)));
//G555: __E_cli_command_166(364) => def_sym(361) __(27) number(365).
	p(NT(364), (NT(361)+NT(27)+NT(365)));
//G556: def_print_cmd(363)   => __E_cli_command_166(364).
	p(NT(363), (NT(364)));
//G557: cli_command(273)     => def_print_cmd(363).
	p(NT(273), (NT(363)));
//G558: def_rr_cmd(366)      => rec_relation(16).
	p(NT(366), (NT(16)));
//G559: cli_command(273)     => def_rr_cmd(366).
	p(NT(273), (NT(366)));
//G560: qelim_sym(369)       => 'q' 'e' 'l' 'i' 'm'.
	p(NT(369), (T(53)+T(19)+T(6)+T(10)+T(18)));
//G561: __E_cli_command_167(368) => qelim_sym(369) __(27) wff_cmd_arg(310).
	p(NT(368), (NT(369)+NT(27)+NT(310)));
//G562: qelim_cmd(367)       => __E_cli_command_167(368).
	p(NT(367), (NT(368)));
//G563: cli_command(273)     => qelim_cmd(367).
	p(NT(273), (NT(367)));
//G564: get_sym(372)         => 'g' 'e' 't'.
	p(NT(372), (T(59)+T(19)+T(13)));
//G565: __E___E_cli_command_168_169(373) => __(27) option_name(374).
	p(NT(373), (NT(27)+NT(374)));
//G566: __E___E_cli_command_168_169(373) => null.
	p(NT(373), (nul));
//G567: __E_cli_command_168(371) => get_sym(372) __E___E_cli_command_168_169(373).
	p(NT(371), (NT(372)+NT(373)));
//G568: get_cmd(370)         => __E_cli_command_168(371).
	p(NT(370), (NT(371)));
//G569: cli_command(273)     => get_cmd(370).
	p(NT(273), (NT(370)));
//G570: set_sym(377)         => 's' 'e' 't'.
	p(NT(377), (T(12)+T(19)+T(13)));
//G571: __E___E_cli_command_170_171(378) => __(27).
	p(NT(378), (NT(27)));
//G572: __E___E_cli_command_170_171(378) => _(11) '=' _(11).
	p(NT(378), (NT(11)+T(3)+NT(11)));
//G573: __E_cli_command_170(376) => set_sym(377) __(27) option_name(374) __E___E_cli_command_170_171(378) option_value(379).
	p(NT(376), (NT(377)+NT(27)+NT(374)+NT(378)+NT(379)));
//G574: set_cmd(375)         => __E_cli_command_170(376).
	p(NT(375), (NT(376)));
//G575: cli_command(273)     => set_cmd(375).
	p(NT(273), (NT(375)));
//G576: enable_sym(382)      => 'e' 'n' 'a' 'b' 'l' 'e'.
	p(NT(382), (T(19)+T(55)+T(5)+T(7)+T(6)+T(19)));
//G577: __E_cli_command_172(381) => enable_sym(382) __(27) option_name(374).
	p(NT(381), (NT(382)+NT(27)+NT(374)));
//G578: enable_cmd(380)      => __E_cli_command_172(381).
	p(NT(380), (NT(381)));
//G579: cli_command(273)     => enable_cmd(380).
	p(NT(273), (NT(380)));
//G580: disable_sym(385)     => 'd' 'i' 's' 'a' 'b' 'l' 'e'.
	p(NT(385), (T(58)+T(10)+T(12)+T(5)+T(7)+T(6)+T(19)));
//G581: __E_cli_command_173(384) => disable_sym(385) __(27) option_name(374).
	p(NT(384), (NT(385)+NT(27)+NT(374)));
//G582: disable_cmd(383)     => __E_cli_command_173(384).
	p(NT(383), (NT(384)));
//G583: cli_command(273)     => disable_cmd(383).
	p(NT(273), (NT(383)));
//G584: toggle_sym(388)      => 't' 'o' 'g' 'g' 'l' 'e'.
	p(NT(388), (T(13)+T(17)+T(59)+T(59)+T(6)+T(19)));
//G585: __E_cli_command_174(387) => toggle_sym(388) __(27) option_name(374).
	p(NT(387), (NT(388)+NT(27)+NT(374)));
//G586: toggle_cmd(386)      => __E_cli_command_174(387).
	p(NT(386), (NT(387)));
//G587: cli_command(273)     => toggle_cmd(386).
	p(NT(273), (NT(386)));
//G588: __E_cli_command_175(390) => type(150) __(27) in_var_name(227) _(11) '=' _(11) input_stream(391).
	p(NT(390), (NT(150)+NT(27)+NT(227)+NT(11)+T(3)+NT(11)+NT(391)));
//G589: def_input_cmd(389)   => __E_cli_command_175(390).
	p(NT(389), (NT(390)));
//G590: cli_command(273)     => def_input_cmd(389).
	p(NT(273), (NT(389)));
//G591: __E_cli_command_176(393) => type(150) __(27) out_var_name(231) _(11) '=' _(11) output_stream(394).
	p(NT(393), (NT(150)+NT(27)+NT(231)+NT(11)+T(3)+NT(11)+NT(394)));
//G592: def_output_cmd(392)  => __E_cli_command_176(393).
	p(NT(392), (NT(393)));
//G593: cli_command(273)     => def_output_cmd(392).
	p(NT(273), (NT(392)));
//G594: __E___E_cli_command_177_178(398) => 'h' 'i' 's' 't'.
	p(NT(398), (T(56)+T(10)+T(12)+T(13)));
//G595: __E___E_cli_command_177_178(398) => 'h' 'i' 's' 't' 'o' 'r' 'y'.
	p(NT(398), (T(56)+T(10)+T(12)+T(13)+T(17)+T(11)+T(23)));
//G596: history_sym(397)     => __E___E_cli_command_177_178(398).
	p(NT(397), (NT(398)));
//G597: __E_cli_command_177(396) => history_sym(397).
	p(NT(396), (NT(397)));
//G598: history_list_cmd(395) => __E_cli_command_177(396).
	p(NT(395), (NT(396)));
//G599: cli_command(273)     => history_list_cmd(395).
	p(NT(273), (NT(395)));
//G600: __E_cli_command_179(400) => history_sym(397) __(27) memory(319).
	p(NT(400), (NT(397)+NT(27)+NT(319)));
//G601: history_print_cmd(399) => __E_cli_command_179(400).
	p(NT(399), (NT(400)));
//G602: cli_command(273)     => history_print_cmd(399).
	p(NT(273), (NT(399)));
//G603: __E_cli_command_180(402) => wff(14).
	p(NT(402), (NT(14)));
//G604: __E_cli_command_180(402) => bf(21).
	p(NT(402), (NT(21)));
//G605: history_store_cmd(401) => __E_cli_command_180(402).
	p(NT(401), (NT(402)));
//G606: cli_command(273)     => history_store_cmd(401).
	p(NT(273), (NT(401)));
//G607: __E_solve_options_181(403) => __(27) solve_option(404).
	p(NT(403), (NT(27)+NT(404)));
//G608: __E_solve_options_182(405) => null.
	p(NT(405), (nul));
//G609: __E_solve_options_182(405) => __E_solve_options_181(403) __E_solve_options_182(405).
	p(NT(405), (NT(403)+NT(405)));
//G610: solve_options(309)   => __E_solve_options_182(405).
	p(NT(309), (NT(405)));
//G611: __E_solve_option_183(406) => '-' '-' solver_mode(407).
	p(NT(406), (T(28)+T(28)+NT(407)));
//G612: solve_option(404)    => __E_solve_option_183(406).
	p(NT(404), (NT(406)));
//G613: __E_solve_option_184(408) => '-' '-' type(150).
	p(NT(408), (T(28)+T(28)+NT(150)));
//G614: solve_option(404)    => __E_solve_option_184(408).
	p(NT(404), (NT(408)));
//G615: __E_solver_mode_185(410) => 'm' 'i' 'n' 'i' 'm' 'u' 'm'.
	p(NT(410), (T(18)+T(10)+T(55)+T(10)+T(18)+T(45)+T(18)));
//G616: __E_solver_mode_185(410) => 'm' 'i' 'n'.
	p(NT(410), (T(18)+T(10)+T(55)));
//G617: solver_mode_minimum(409) => __E_solver_mode_185(410).
	p(NT(409), (NT(410)));
//G618: solver_mode(407)     => solver_mode_minimum(409).
	p(NT(407), (NT(409)));
//G619: __E_solver_mode_186(412) => 'm' 'a' 'x' 'i' 'm' 'u' 'm'.
	p(NT(412), (T(18)+T(5)+T(27)+T(10)+T(18)+T(45)+T(18)));
//G620: __E_solver_mode_186(412) => 'm' 'a' 'x'.
	p(NT(412), (T(18)+T(5)+T(27)));
//G621: solver_mode_maximum(411) => __E_solver_mode_186(412).
	p(NT(411), (NT(412)));
//G622: solver_mode(407)     => solver_mode_maximum(411).
	p(NT(407), (NT(411)));
//G623: __E_input_stream_187(413) => 'i' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(414) _(11) ')'.
	p(NT(413), (T(10)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(414)+NT(11)+T(16)));
//G624: input_stream(391)    => __E_input_stream_187(413).
	p(NT(391), (NT(413)));
//G625: console_sym(415)     => 'c' 'o' 'n' 's' 'o' 'l' 'e'.
	p(NT(415), (T(8)+T(17)+T(55)+T(12)+T(17)+T(6)+T(19)));
//G626: input_stream(391)    => console_sym(415).
	p(NT(391), (NT(415)));
//G627: __E_output_stream_188(416) => 'o' 'f' 'i' 'l' 'e' _(11) '(' _(11) q_file_name(414) _(11) ')'.
	p(NT(416), (T(17)+T(4)+T(10)+T(6)+T(19)+NT(11)+T(15)+NT(11)+NT(414)+NT(11)+T(16)));
//G628: output_stream(394)   => __E_output_stream_188(416).
	p(NT(394), (NT(416)));
//G629: output_stream(394)   => console_sym(415).
	p(NT(394), (NT(415)));
//G630: q_file_name(414)     => '"' file_name(417) '"'.
	p(NT(414), (T(61)+NT(417)+T(61)));
//G631: __E_file_name_189(418) => printable(8).
	p(NT(418), (NT(8)));
//G632: __E_file_name_189(418) => printable(8) __E_file_name_189(418).
	p(NT(418), (NT(8)+NT(418)));
//G633: file_name(417)       => __E_file_name_189(418).
	p(NT(417), (NT(418)));
//G634: __E_option_name_190(419) => alnum(6).
	p(NT(419), (NT(6)));
//G635: __E_option_name_190(419) => alnum(6) __E_option_name_190(419).
	p(NT(419), (NT(6)+NT(419)));
//G636: option_name(374)     => __E_option_name_190(419).
	p(NT(374), (NT(419)));
//G637: __E_option_value_191(420) => alnum(6).
	p(NT(420), (NT(6)));
//G638: __E_option_value_191(420) => alnum(6) __E_option_value_191(420).
	p(NT(420), (NT(6)+NT(420)));
//G639: option_value(379)    => __E_option_value_191(420).
	p(NT(379), (NT(420)));
//G640: number(365)          => digits(246).
	p(NT(365), (NT(246)));
//G641: bf_cmd_arg(421)      => memory(319).
	p(NT(421), (NT(319)));
//G642: bf_cmd_arg(421)      => bf(21).
	p(NT(421), (NT(21)));
//G643: wff_cmd_arg(310)     => memory(319).
	p(NT(310), (NT(319)));
//G644: wff_cmd_arg(310)     => wff(14).
	p(NT(310), (NT(14)));
//G645: nf_cmd_arg(328)      => memory(319).
	p(NT(328), (NT(319)));
//G646: nf_cmd_arg(328)      => ref(18).
	p(NT(328), (NT(18)));
//G647: nf_cmd_arg(328)      => wff(14).
	p(NT(328), (NT(14)));
//G648: nf_cmd_arg(328)      => bf(21).
	p(NT(328), (NT(21)));
//G649: onf_cmd_arg(358)     => memory(319).
	p(NT(358), (NT(319)));
//G650: onf_cmd_arg(358)     => wff(14).
	p(NT(358), (NT(14)));
//G651: normalize_cmd_arg(298) => memory(319).
	p(NT(298), (NT(319)));
//G652: normalize_cmd_arg(298) => rr(10).
	p(NT(298), (NT(10)));
//G653: normalize_cmd_arg(298) => ref(18).
	p(NT(298), (NT(18)));
//G654: normalize_cmd_arg(298) => wff(14).
	p(NT(298), (NT(14)));
//G655: normalize_cmd_arg(298) => bf(21).
	p(NT(298), (NT(21)));
//G656: inst_args(333)       => wff_cmd_arg(310) _(11) '[' _(11) variable(139) _(11) '/' _(11) bf_cmd_arg(421) _(11) ']'.
	p(NT(333), (NT(310)+NT(11)+T(24)+NT(11)+NT(139)+NT(11)+T(43)+NT(11)+NT(421)+NT(11)+T(25)));
//G657: inst_args(333)       => bf_cmd_arg(421) _(11) '[' _(11) variable(139) _(11) '/' _(11) bf_cmd_arg(421) _(11) ']'.
	p(NT(333), (NT(421)+NT(11)+T(24)+NT(11)+NT(139)+NT(11)+T(43)+NT(11)+NT(421)+NT(11)+T(25)));
//G658: help_arg(290)        => help_sym(287).
	p(NT(290), (NT(287)));
//G659: help_arg(290)        => version_sym(280).
	p(NT(290), (NT(280)));
//G660: help_arg(290)        => quit_sym(277).
	p(NT(290), (NT(277)));
//G661: help_arg(290)        => clear_sym(283).
	p(NT(290), (NT(283)));
//G662: help_arg(290)        => get_sym(372).
	p(NT(290), (NT(372)));
//G663: help_arg(290)        => set_sym(377).
	p(NT(290), (NT(377)));
//G664: help_arg(290)        => enable_sym(382).
	p(NT(290), (NT(382)));
//G665: help_arg(290)        => disable_sym(385).
	p(NT(290), (NT(385)));
//G666: help_arg(290)        => toggle_sym(388).
	p(NT(290), (NT(388)));
//G667: help_arg(290)        => file_sym(293).
	p(NT(290), (NT(293)));
//G668: help_arg(290)        => history_sym(397).
	p(NT(290), (NT(397)));
//G669: help_arg(290)        => abs_memory_sym(422).
	p(NT(290), (NT(422)));
//G670: help_arg(290)        => rel_memory_sym(423).
	p(NT(290), (NT(423)));
//G671: help_arg(290)        => selection_sym(424).
	p(NT(290), (NT(424)));
//G672: help_arg(290)        => def_sym(361).
	p(NT(290), (NT(361)));
//G673: help_arg(290)        => inst_sym(331).
	p(NT(290), (NT(331)));
//G674: help_arg(290)        => subst_sym(326).
	p(NT(290), (NT(326)));
//G675: help_arg(290)        => normalize_sym(322).
	p(NT(290), (NT(322)));
//G676: help_arg(290)        => execute_sym(425).
	p(NT(290), (NT(425)));
//G677: help_arg(290)        => solve_sym(307).
	p(NT(290), (NT(307)));
//G678: help_arg(290)        => valid_sym(297).
	p(NT(290), (NT(297)));
//G679: help_arg(290)        => sat_sym(301).
	p(NT(290), (NT(301)));
//G680: help_arg(290)        => unsat_sym(304).
	p(NT(290), (NT(304)));
//G681: help_arg(290)        => run_sym(316).
	p(NT(290), (NT(316)));
//G682: help_arg(290)        => dnf_sym(336).
	p(NT(290), (NT(336)));
//G683: help_arg(290)        => cnf_sym(339).
	p(NT(290), (NT(339)));
//G684: help_arg(290)        => anf_sym(342).
	p(NT(290), (NT(342)));
//G685: help_arg(290)        => snf_sym(354).
	p(NT(290), (NT(354)));
//G686: help_arg(290)        => nnf_sym(345).
	p(NT(290), (NT(345)));
//G687: help_arg(290)        => pnf_sym(348).
	p(NT(290), (NT(348)));
//G688: help_arg(290)        => mnf_sym(351).
	p(NT(290), (NT(351)));
//G689: help_arg(290)        => onf_sym(357).
	p(NT(290), (NT(357)));
//G690: help_arg(290)        => qelim_sym(369).
	p(NT(290), (NT(369)));
//G691: __E___E_help_arg_192_193(428) => 's'.
	p(NT(428), (T(12)));
//G692: __E___E_help_arg_192_193(428) => null.
	p(NT(428), (nul));
//G693: __E_help_arg_192(427) => 'e' 'x' 'a' 'm' 'p' 'l' 'e' __E___E_help_arg_192_193(428).
	p(NT(427), (T(19)+T(27)+T(5)+T(18)+T(57)+T(6)+T(19)+NT(428)));
//G694: examples_sym(426)    => __E_help_arg_192(427).
	p(NT(426), (NT(427)));
//G695: help_arg(290)        => examples_sym(426).
	p(NT(290), (NT(426)));
//G696: __E_memory_194(430)  => '%'.
	p(NT(430), (T(44)));
//G697: rel_memory_sym(423)  => '%' '-'.
	p(NT(423), (T(44)+T(28)));
//G698: memory_id(432)       => digits(246).
	p(NT(432), (NT(246)));
//G699: __E___E_memory_194_195(431) => memory_id(432).
	p(NT(431), (NT(432)));
//G700: __E___E_memory_194_195(431) => null.
	p(NT(431), (nul));
//G701: __E_memory_194(430)  => rel_memory_sym(423) __E___E_memory_194_195(431).
	p(NT(430), (NT(423)+NT(431)));
//G702: rel_memory(429)      => __E_memory_194(430).
	p(NT(429), (NT(430)));
//G703: memory(319)          => rel_memory(429).
	p(NT(319), (NT(429)));
//G704: abs_memory_sym(422)  => '%'.
	p(NT(422), (T(44)));
//G705: __E_memory_196(434)  => abs_memory_sym(422) memory_id(432).
	p(NT(434), (NT(422)+NT(432)));
//G706: abs_memory(433)      => __E_memory_196(434).
	p(NT(433), (NT(434)));
//G707: memory(319)          => abs_memory(433).
	p(NT(319), (NT(433)));
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
		__E___E_bf_64_65, bf_and_nosep, __E_bf_66, bf_and_nosep_1st_oprnd, __E___E_bf_66_67, bf_and_nosep_2nd_oprnd, __E___E_bf_66_68, z3_parenthesis, __E_z3_69, z3_neg, 
		z3_add, __E_z3_70, z3_sub, __E_z3_71, z3_mul, __E_z3_72, z3_div, __E_z3_73, z3_mod, __E_z3_74, 
		z3_and, __E_z3_75, z3_or, __E_z3_76, z3_xor, __E_z3_77, z3_shift_left, __E_z3_78, num, z3_shift_right, 
		__E_z3_79, bitvector, bits, __E_bitvector_80, bitvector_type, __E_bitvector_81, __E_bitvector_type_82, bit, __E_bits_83, __E_bits_84, 
		ctn_neq, __E_constraint_85, ctnvar, ctn_eq, __E_constraint_86, ctn_greater_equal, __E_constraint_87, ctn_greater, __E_constraint_88, ctn_less_equal, 
		__E_constraint_89, ctn_less, __E_constraint_90, __E_variable_91, __E_variable_92, __E_variable_93, chars, __E_variable_94, __E_variable_95, __E_variable_96, 
		__E_variable_97, io_var, __E_variable_98, in, out, __E_variable_99, uninterpreted_constant, in_var_name, __E_in_100, __E___E_in_100_101, 
		offset, out_var_name, __E_out_102, __E___E_out_102_103, __E___E___E_out_102_103_104, q_var, __E_q_vars_105, __E_q_vars_106, __E_q_var_107, __E_offsets_108, 
		__E_offsets_109, integer, shift, __E_offset_110, __E_shift_111, __E___E_shift_111_112, digits, __E_integer_113, uninter_const_name, __E_uninterpreted_constant_114, 
		__E___E_uninterpreted_constant_114_115, constant, __E_bf_constant_116, binding, __E_bf_constant_117, source, __E_binding_118, src_c, __E___E_binding_118_119, __E___E___E_binding_118_119_120, 
		__E___E___E_binding_118_119_121, __E_src_c_122, __E_src_c_123, __E_chars_124, __E_chars_125, __E_digits_126, comment, __E_comment_127, __E_comment_128, __E_comment_129, 
		__E____130, __E___131, cli, cli_command, __E_cli_132, __E_cli_133, quit_cmd, quit_sym, __E_cli_command_134, version_cmd, 
		version_sym, __E_cli_command_135, clear_cmd, clear_sym, __E_cli_command_136, help_cmd, __E_cli_command_137, help_sym, __E___E_cli_command_137_138, __E___E_cli_command_137_139, 
		help_arg, file_cmd, __E_cli_command_140, file_sym, q_string, valid_cmd, __E_cli_command_141, valid_sym, normalize_cmd_arg, sat_cmd, 
		__E_cli_command_142, sat_sym, unsat_cmd, __E_cli_command_143, unsat_sym, solve_cmd, __E_cli_command_144, solve_sym, __E___E_cli_command_144_145, solve_options, 
		wff_cmd_arg, lgrs_cmd, __E_cli_command_146, lgrs_sym, run_cmd, __E_cli_command_147, run_sym, __E___E_cli_command_147_148, __E___E_cli_command_147_149, memory, 
		normalize_cmd, __E_cli_command_150, normalize_sym, __E___E_cli_command_150_151, subst_cmd, __E_cli_command_152, subst_sym, __E___E_cli_command_152_153, nf_cmd_arg, inst_cmd, 
		__E_cli_command_154, inst_sym, __E___E_cli_command_154_155, inst_args, dnf_cmd, __E_cli_command_156, dnf_sym, cnf_cmd, __E_cli_command_157, cnf_sym, 
		anf_cmd, __E_cli_command_158, anf_sym, nnf_cmd, __E_cli_command_159, nnf_sym, pnf_cmd, __E_cli_command_160, pnf_sym, mnf_cmd, 
		__E_cli_command_161, mnf_sym, snf_cmd, __E_cli_command_162, snf_sym, onf_cmd, __E_cli_command_163, onf_sym, onf_cmd_arg, def_list_cmd, 
		__E_cli_command_164, def_sym, __E___E_cli_command_164_165, def_print_cmd, __E_cli_command_166, number, def_rr_cmd, qelim_cmd, __E_cli_command_167, qelim_sym, 
		get_cmd, __E_cli_command_168, get_sym, __E___E_cli_command_168_169, option_name, set_cmd, __E_cli_command_170, set_sym, __E___E_cli_command_170_171, option_value, 
		enable_cmd, __E_cli_command_172, enable_sym, disable_cmd, __E_cli_command_173, disable_sym, toggle_cmd, __E_cli_command_174, toggle_sym, def_input_cmd, 
		__E_cli_command_175, input_stream, def_output_cmd, __E_cli_command_176, output_stream, history_list_cmd, __E_cli_command_177, history_sym, __E___E_cli_command_177_178, history_print_cmd, 
		__E_cli_command_179, history_store_cmd, __E_cli_command_180, __E_solve_options_181, solve_option, __E_solve_options_182, __E_solve_option_183, solver_mode, __E_solve_option_184, solver_mode_minimum, 
		__E_solver_mode_185, solver_mode_maximum, __E_solver_mode_186, __E_input_stream_187, q_file_name, console_sym, __E_output_stream_188, file_name, __E_file_name_189, __E_option_name_190, 
		__E_option_value_191, bf_cmd_arg, abs_memory_sym, rel_memory_sym, selection_sym, execute_sym, examples_sym, __E_help_arg_192, __E___E_help_arg_192_193, rel_memory, 
		__E_memory_194, __E___E_memory_194_195, memory_id, abs_memory, __E_memory_196, named_binding, __N_0, __N_1, __N_2, __N_3, 
		__N_4, __N_5, __N_6, __N_7, __N_8, 
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
