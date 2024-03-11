// This file is generated from a file parser/tau.tgf by
//       https://github.com/IDNI/parser/tools/tgf
//
#ifndef __TAU_PARSER_H__
#define __TAU_PARSER_H__
#include <string.h>
#include "parser.h"
struct tau_parser {
	using char_type     = char;
	using terminal_type = char;
	using traits_type   = std::char_traits<char_type>;
	using int_type      = typename traits_type::int_type;
	using symbol_type   = idni::lit<char_type, terminal_type>;
	using location_type = std::array<size_t, 2>;
	using node_type     = std::pair<symbol_type, location_type>;
	using parser_type   = idni::parser<char_type, terminal_type>;
	using options       = parser_type::options;
	using parse_options = parser_type::parse_options;
	using forest_type   = parser_type::pforest;
	using input_type    = parser_type::input;
	using decoder_type  = parser_type::input::decoder_type;
	using encoder_type  = std::function<std::basic_string<char_type>(
			const std::vector<terminal_type>&)>;
	tau_parser() :
		nts(load_nonterminals()), cc(load_cc()),
		g(nts, load_prods(), nt(240), cc), p(g, load_opts()) {}
	std::unique_ptr<forest_type> parse(const char_type* data, size_t size,
		parse_options po = {}) { return p.parse(data, size, po); }
	std::unique_ptr<forest_type> parse(std::basic_istream<char_type>& is,
		parse_options po = {}) { return p.parse(is, po); }
	std::unique_ptr<forest_type> parse(const std::string& fn,
		parse_options po = {}) { return p.parse(fn, po); }
#ifndef WIN32
	std::unique_ptr<forest_type> parse(int fd, parse_options po = {})
		{ return p.parse(fd, po); }
#endif //WIN32
	bool found(int start = -1) { return p.found(start); }
	typename parser_type::error get_error() { return p.get_error(); }
	enum nonterminal {
		nul, eof, space, digit, xdigit, alpha, alnum, punct, printable, comment, 
		_Rcomment_0, _Rcomment_1, _Rcomment_2, __, _R___3, _, _R__4, bf_def, wff_def, tau_def, 
		dot, open_parenthesis, close_parenthesis, open_bracket, close_bracket, open_brace, close_brace, minus, colon, semicolon, 
		less, comma, apostrophe, quote, chars, _Rchars_5, _Rchars_6, digits, _Rdigits_7, sym, 
		offsets, offset, _Roffsets_8, _Roffsets_9, num, capture, shift, variable, io_var, _Roffset_10, 
		_Rshift_11, _Rshift_12, var, in, out, in_var_name, out_var_name, bool_variable, wff_t, wff_f, 
		capture_var, tau_rule, tau_matcher, tau_body, tau, tau_collapse_positives_cb, tau_positives_upwards_cb, tau_rec_relation, tau_and, tau_neg, 
		tau_or, tau_wff, _Rtau_rec_relation_13, tau_ref, tau_ref_args, _Rtau_ref_args_14, _Rtau_ref_args_15, _Rtau_ref_args_16, tau_and_sym, tau_or_sym, 
		tau_neg_sym, wff, wff_rule, wff_matcher, wff_body, bf_eq_cb, bf_neq_cb, wff_has_clashing_subformulas_cb, wff_has_subformula_cb, wff_remove_existential_cb, 
		wff_remove_bexistential_cb, wff_remove_buniversal_cb, wff_rec_relation, wff_and, wff_neg, wff_xor, wff_conditional, wff_or, wff_all, wff_ex, 
		wff_imply, wff_equiv, wff_ball, wff_bex, bf_eq, bf_neq, bf_less, bf_less_equal, bf_greater, bf_interval, 
		bf_not_less_equal, _Rwff_rec_relation_17, wff_ref, wff_ref_args, _Rwff_ref_args_18, _Rwff_ref_args_19, _Rwff_ref_args_20, wff_and_sym, wff_or_sym, wff_xor_sym, 
		wff_conditional_sym, wff_neg_sym, wff_imply_sym, wff_equiv_sym, wff_all_sym, _Rwff_all_21, wff_ex_sym, _Rwff_ex_22, wff_ball_sym, _Rwff_ball_23, 
		wff_bex_sym, _Rwff_bex_24, bf, bf_equality_sym, bf_nequality_sym, bf_less_sym, bf_less_equal_sym, bf_not_less_equal_sym, bf_greater_sym, bf_rule, 
		bf_matcher, bf_body, bf_is_zero_cb, bf_is_one_cb, bf_has_clashing_subformulas_cb, bf_has_subformula_cb, bf_remove_funiversal_cb, bf_remove_fexistential_cb, bf_and_cb, bf_or_cb, 
		bf_xor_cb, bf_neg_cb, bf_ref, bf_constant, bf_and, bf_neg, bf_xor, bf_or, bf_all, bf_ex, 
		bf_t, bf_f, bf_splitter, bf_rec_relation, _Rbf_rec_relation_25, bf_ref_args, _Rbf_ref_args_26, _Rbf_ref_args_27, _Rbf_ref_args_28, bf_and_sym, 
		bf_or_sym, bf_xor_sym, bf_neg_sym, bf_all_sym, _Rbf_all_29, bf_ex_sym, _Rbf_ex_30, bf_splitter_sym, constant, binding, 
		source_binding, named_binding, type, source, source0, _Rsource_31, _Rsource_32, bf_and_cb_sym, bf_cb_arg, bf_or_cb_sym, 
		bf_xor_cb_sym, bf_neg_cb_sym, bf_eq_cb_sym, wff_cb_arg, bf_neq_cb_sym, bf_is_zero_cb_sym, bf_is_one_cb_sym, bf_remove_funiversal_cb_sym, bf_remove_fexistential_cb_sym, wff_remove_existential_cb_sym, 
		wff_remove_bexistential_cb_sym, wff_remove_buniversal_cb_sym, bf_has_clashing_subformulas_cb_sym, wff_has_clashing_subformulas_cb_sym, bf_has_subformula_cb_sym, wff_has_subformula_cb_sym, tau_collapse_positives_cb_sym, tau_cb_arg, _Rtau_collapse_positives_cb_33, _Rtau_collapse_positives_cb_34, 
		tau_positives_upwards_cb_sym, inputs, input, _Rinputs_35, _Rinputs_36, builder, builder_head, builder_body, _Rbuilder_head_37, _Rbuilder_head_38, 
		bf_builder_body, wff_builder_body, tau_builder_body, library, rules, rule, _Rrules_39, _Rrules_40, nso_rr, nso_rec_relations, 
		nso_main, nso_rec_relation, _Rnso_rec_relations_41, _Rnso_rec_relations_42, gssotc_rr, gssotc_rec_relations, gssotc_main, gssotc_rec_relation, _Rgssotc_rec_relations_43, _Rgssotc_rec_relations_44, 
		start, _Rstart_45, cli, cli_command, _Rcli_46, _Rcli_47, _Rcli_48, normalize, bf_substitute, wff_substitute, 
		bf_instantiate, wff_instantiate, rr_nso, help, version, quit, get, list_outputs, _Rcli_command_49, clear_outputs, 
		print_output, file, toggle, set, normalize_sym, q_nso_rr, q_wff, output, wff_selection, _Rnormalize_50, 
		file_sym, file_path, instantiate_sym, q_var, bf_var_selection, _Rbf_instantiate_51, q_bf, bf_selection, _Rbf_instantiate_52, _Rbf_instantiate_53, 
		wff_var_selection, _Rwff_instantiate_54, _Rwff_instantiate_55, _Rwff_instantiate_56, substitute_sym, _Rbf_substitute_57, _Rbf_substitute_58, _Rwff_substitute_59, _Rwff_substitute_60, help_sym, 
		cli_cmd_sym, _Rhelp_61, version_sym, quit_sym, selection_sym, absolute_output, relative_output, absolute_output_sym, output_sym, _Rabsolute_output_62, 
		output_id, relative_output_sym, _Rclear_outputs_63, clear_sym, get_sym, set_sym, toggle_sym, option, _Rget_64, _Rset_65, 
		option_value, bool_option, severity_opt, status_opt, colors_opt, option_value_true, option_value_false, severity, error_sym, debug_sym, 
		trace_sym, info_sym, __neg_0, __neg_1, __neg_2, __neg_3, __neg_4, __neg_5, __neg_6, __neg_7, 
		__neg_8, __neg_9, __neg_10, __neg_11, __neg_12, __neg_13, __neg_14, __neg_15, __neg_16, __neg_17, 
		__neg_18, __neg_19, __neg_20, __neg_21, __neg_22, __neg_23, __neg_24, __neg_25, __neg_26, __neg_27, 
		__neg_28, __neg_29, __neg_30, __neg_31, __neg_32, __neg_33, __neg_34, __neg_35, __neg_36, __neg_37, 
		__neg_38, __neg_39, __neg_40, __neg_41, __neg_42, __neg_43, __neg_44, __neg_45, __neg_46, __neg_47, 
		__neg_48, __neg_49, 
	};
	size_t id(const std::basic_string<char_type>& name) {
		return nts.get(name);
	}
	const std::basic_string<char_type>& name(size_t id) {
		return nts.get(id);
	}
private:
	std::vector<terminal_type> ts{
		'\0', '\t', '\n', '\r', '#', ':', '=', '.', '(', 
		')', '[', ']', '{', '}', '-', ';', '<', ',', '\'', 
		'"', '$', 'i', '_', 'o', '&', '|', '!', '^', '?', 
		'>', 'a', 'l', 'e', 'x', 'b', 'T', 'F', '+', 'f', 
		'S', '1', '0', 'h', 's', 'c', 'n', 'g', 'u', 'r', 
		'm', 'w', 'v', 't', 'd', 'q', 'z', 'p', '%', 'y', 
	};
	idni::nonterminals<char_type, terminal_type> nts{};
	idni::char_class_fns<terminal_type> cc;
	idni::grammar<char_type, terminal_type> g;
	parser_type p;
	idni::prods<char_type, terminal_type> t(size_t tid) {
		return idni::prods<char_type, terminal_type>(ts[tid]);
	}
	idni::prods<char_type, terminal_type> nt(size_t ntid) {
		return idni::prods<char_type, terminal_type>(
			symbol_type(ntid, &nts));
	}
	idni::nonterminals<char_type, terminal_type> load_nonterminals() const {
		idni::nonterminals<char_type, terminal_type> nts{};
		for (const auto& nt : {
			"", "eof", "space", "digit", "xdigit", "alpha", "alnum", "punct", "printable", "comment", 
			"_Rcomment_0", "_Rcomment_1", "_Rcomment_2", "__", "_R___3", "_", "_R__4", "bf_def", "wff_def", "tau_def", 
			"dot", "open_parenthesis", "close_parenthesis", "open_bracket", "close_bracket", "open_brace", "close_brace", "minus", "colon", "semicolon", 
			"less", "comma", "apostrophe", "quote", "chars", "_Rchars_5", "_Rchars_6", "digits", "_Rdigits_7", "sym", 
			"offsets", "offset", "_Roffsets_8", "_Roffsets_9", "num", "capture", "shift", "variable", "io_var", "_Roffset_10", 
			"_Rshift_11", "_Rshift_12", "var", "in", "out", "in_var_name", "out_var_name", "bool_variable", "wff_t", "wff_f", 
			"capture_var", "tau_rule", "tau_matcher", "tau_body", "tau", "tau_collapse_positives_cb", "tau_positives_upwards_cb", "tau_rec_relation", "tau_and", "tau_neg", 
			"tau_or", "tau_wff", "_Rtau_rec_relation_13", "tau_ref", "tau_ref_args", "_Rtau_ref_args_14", "_Rtau_ref_args_15", "_Rtau_ref_args_16", "tau_and_sym", "tau_or_sym", 
			"tau_neg_sym", "wff", "wff_rule", "wff_matcher", "wff_body", "bf_eq_cb", "bf_neq_cb", "wff_has_clashing_subformulas_cb", "wff_has_subformula_cb", "wff_remove_existential_cb", 
			"wff_remove_bexistential_cb", "wff_remove_buniversal_cb", "wff_rec_relation", "wff_and", "wff_neg", "wff_xor", "wff_conditional", "wff_or", "wff_all", "wff_ex", 
			"wff_imply", "wff_equiv", "wff_ball", "wff_bex", "bf_eq", "bf_neq", "bf_less", "bf_less_equal", "bf_greater", "bf_interval", 
			"bf_not_less_equal", "_Rwff_rec_relation_17", "wff_ref", "wff_ref_args", "_Rwff_ref_args_18", "_Rwff_ref_args_19", "_Rwff_ref_args_20", "wff_and_sym", "wff_or_sym", "wff_xor_sym", 
			"wff_conditional_sym", "wff_neg_sym", "wff_imply_sym", "wff_equiv_sym", "wff_all_sym", "_Rwff_all_21", "wff_ex_sym", "_Rwff_ex_22", "wff_ball_sym", "_Rwff_ball_23", 
			"wff_bex_sym", "_Rwff_bex_24", "bf", "bf_equality_sym", "bf_nequality_sym", "bf_less_sym", "bf_less_equal_sym", "bf_not_less_equal_sym", "bf_greater_sym", "bf_rule", 
			"bf_matcher", "bf_body", "bf_is_zero_cb", "bf_is_one_cb", "bf_has_clashing_subformulas_cb", "bf_has_subformula_cb", "bf_remove_funiversal_cb", "bf_remove_fexistential_cb", "bf_and_cb", "bf_or_cb", 
			"bf_xor_cb", "bf_neg_cb", "bf_ref", "bf_constant", "bf_and", "bf_neg", "bf_xor", "bf_or", "bf_all", "bf_ex", 
			"bf_t", "bf_f", "bf_splitter", "bf_rec_relation", "_Rbf_rec_relation_25", "bf_ref_args", "_Rbf_ref_args_26", "_Rbf_ref_args_27", "_Rbf_ref_args_28", "bf_and_sym", 
			"bf_or_sym", "bf_xor_sym", "bf_neg_sym", "bf_all_sym", "_Rbf_all_29", "bf_ex_sym", "_Rbf_ex_30", "bf_splitter_sym", "constant", "binding", 
			"source_binding", "named_binding", "type", "source", "source0", "_Rsource_31", "_Rsource_32", "bf_and_cb_sym", "bf_cb_arg", "bf_or_cb_sym", 
			"bf_xor_cb_sym", "bf_neg_cb_sym", "bf_eq_cb_sym", "wff_cb_arg", "bf_neq_cb_sym", "bf_is_zero_cb_sym", "bf_is_one_cb_sym", "bf_remove_funiversal_cb_sym", "bf_remove_fexistential_cb_sym", "wff_remove_existential_cb_sym", 
			"wff_remove_bexistential_cb_sym", "wff_remove_buniversal_cb_sym", "bf_has_clashing_subformulas_cb_sym", "wff_has_clashing_subformulas_cb_sym", "bf_has_subformula_cb_sym", "wff_has_subformula_cb_sym", "tau_collapse_positives_cb_sym", "tau_cb_arg", "_Rtau_collapse_positives_cb_33", "_Rtau_collapse_positives_cb_34", 
			"tau_positives_upwards_cb_sym", "inputs", "input", "_Rinputs_35", "_Rinputs_36", "builder", "builder_head", "builder_body", "_Rbuilder_head_37", "_Rbuilder_head_38", 
			"bf_builder_body", "wff_builder_body", "tau_builder_body", "library", "rules", "rule", "_Rrules_39", "_Rrules_40", "nso_rr", "nso_rec_relations", 
			"nso_main", "nso_rec_relation", "_Rnso_rec_relations_41", "_Rnso_rec_relations_42", "gssotc_rr", "gssotc_rec_relations", "gssotc_main", "gssotc_rec_relation", "_Rgssotc_rec_relations_43", "_Rgssotc_rec_relations_44", 
			"start", "_Rstart_45", "cli", "cli_command", "_Rcli_46", "_Rcli_47", "_Rcli_48", "normalize", "bf_substitute", "wff_substitute", 
			"bf_instantiate", "wff_instantiate", "rr_nso", "help", "version", "quit", "get", "list_outputs", "_Rcli_command_49", "clear_outputs", 
			"print_output", "file", "toggle", "set", "normalize_sym", "q_nso_rr", "q_wff", "output", "wff_selection", "_Rnormalize_50", 
			"file_sym", "file_path", "instantiate_sym", "q_var", "bf_var_selection", "_Rbf_instantiate_51", "q_bf", "bf_selection", "_Rbf_instantiate_52", "_Rbf_instantiate_53", 
			"wff_var_selection", "_Rwff_instantiate_54", "_Rwff_instantiate_55", "_Rwff_instantiate_56", "substitute_sym", "_Rbf_substitute_57", "_Rbf_substitute_58", "_Rwff_substitute_59", "_Rwff_substitute_60", "help_sym", 
			"cli_cmd_sym", "_Rhelp_61", "version_sym", "quit_sym", "selection_sym", "absolute_output", "relative_output", "absolute_output_sym", "output_sym", "_Rabsolute_output_62", 
			"output_id", "relative_output_sym", "_Rclear_outputs_63", "clear_sym", "get_sym", "set_sym", "toggle_sym", "option", "_Rget_64", "_Rset_65", 
			"option_value", "bool_option", "severity_opt", "status_opt", "colors_opt", "option_value_true", "option_value_false", "severity", "error_sym", "debug_sym", 
			"trace_sym", "info_sym", "__neg_0", "__neg_1", "__neg_2", "__neg_3", "__neg_4", "__neg_5", "__neg_6", "__neg_7", 
			"__neg_8", "__neg_9", "__neg_10", "__neg_11", "__neg_12", "__neg_13", "__neg_14", "__neg_15", "__neg_16", "__neg_17", 
			"__neg_18", "__neg_19", "__neg_20", "__neg_21", "__neg_22", "__neg_23", "__neg_24", "__neg_25", "__neg_26", "__neg_27", 
			"__neg_28", "__neg_29", "__neg_30", "__neg_31", "__neg_32", "__neg_33", "__neg_34", "__neg_35", "__neg_36", "__neg_37", 
			"__neg_38", "__neg_39", "__neg_40", "__neg_41", "__neg_42", "__neg_43", "__neg_44", "__neg_45", "__neg_46", "__neg_47", 
			"__neg_48", "__neg_49", 
		}) nts.get(nt);
		return nts;
	}
	idni::char_class_fns<terminal_type> load_cc() {
		return idni::predefined_char_classes<char_type, terminal_type>({
			"eof",
			"space",
			"digit",
			"xdigit",
			"alpha",
			"alnum",
			"punct",
			"printable",
		}, nts);
	}
	options load_opts() {
		options o;
		return o;
	}
	idni::prods<char_type, terminal_type> load_prods() {
		idni::prods<char_type, terminal_type> q,
			nul(symbol_type{});
		// _Rcomment_0 => '\t'.
		q(nt(10), (t(1)));
		// _Rcomment_0 => printable.
		q(nt(10), (nt(8)));
		// _Rcomment_1 => null.
		q(nt(11), (nul));
		// _Rcomment_1 => _Rcomment_0 _Rcomment_1.
		q(nt(11), (nt(10)+nt(11)));
		// _Rcomment_2 => '\n'.
		q(nt(12), (t(2)));
		// _Rcomment_2 => '\r'.
		q(nt(12), (t(3)));
		// _Rcomment_2 => eof.
		q(nt(12), (nt(1)));
		// comment => '#' _Rcomment_1 _Rcomment_2.
		q(nt(9), (t(4)+nt(11)+nt(12)));
		// _R___3 => space.
		q(nt(14), (nt(2)));
		// _R___3 => comment.
		q(nt(14), (nt(9)));
		// __ => _R___3 _.
		q(nt(13), (nt(14)+nt(15)));
		// _R__4 => null.
		q(nt(16), (nul));
		// _R__4 => __.
		q(nt(16), (nt(13)));
		// _ => _R__4.
		q(nt(15), (nt(16)));
		// bf_def => ':' '='.
		q(nt(17), (t(5)+t(6)));
		// wff_def => ':' ':' '='.
		q(nt(18), (t(5)+t(5)+t(6)));
		// tau_def => ':' ':' ':' '='.
		q(nt(19), (t(5)+t(5)+t(5)+t(6)));
		// dot => '.'.
		q(nt(20), (t(7)));
		// open_parenthesis => '('.
		q(nt(21), (t(8)));
		// close_parenthesis => ')'.
		q(nt(22), (t(9)));
		// open_bracket => '['.
		q(nt(23), (t(10)));
		// close_bracket => ']'.
		q(nt(24), (t(11)));
		// open_brace => '{'.
		q(nt(25), (t(12)));
		// close_brace => '}'.
		q(nt(26), (t(13)));
		// minus => '-'.
		q(nt(27), (t(14)));
		// colon => ':'.
		q(nt(28), (t(5)));
		// semicolon => ';'.
		q(nt(29), (t(15)));
		// less => '<'.
		q(nt(30), (t(16)));
		// comma => ','.
		q(nt(31), (t(17)));
		// apostrophe => '\''.
		q(nt(32), (t(18)));
		// quote => '"'.
		q(nt(33), (t(19)));
		// _Rchars_5 => alnum.
		q(nt(35), (nt(6)));
		// _Rchars_6 => null.
		q(nt(36), (nul));
		// _Rchars_6 => _Rchars_5 _Rchars_6.
		q(nt(36), (nt(35)+nt(36)));
		// chars => alpha _Rchars_6.
		q(nt(34), (nt(5)+nt(36)));
		// _Rdigits_7 => digit.
		q(nt(38), (nt(3)));
		// _Rdigits_7 => digit _Rdigits_7.
		q(nt(38), (nt(3)+nt(38)));
		// digits => _Rdigits_7.
		q(nt(37), (nt(38)));
		// sym => chars.
		q(nt(39), (nt(34)));
		// _Roffsets_8 => _ comma _ offset.
		q(nt(42), (nt(15)+nt(31)+nt(15)+nt(41)));
		// _Roffsets_9 => null.
		q(nt(43), (nul));
		// _Roffsets_9 => _Roffsets_8 _Roffsets_9.
		q(nt(43), (nt(42)+nt(43)));
		// offsets => open_bracket _ offset _Roffsets_9 _ close_bracket.
		q(nt(40), (nt(23)+nt(15)+nt(41)+nt(43)+nt(15)+nt(24)));
		// __neg_0 => io_var.
		q(nt(322), (nt(48)));
		// _Roffset_10 => variable & ~( __neg_0 ).
		q(nt(49), (nt(47)) & ~(nt(322)));
		// offset => num.
		q(nt(41), (nt(44)));
		// offset => capture.
		q(nt(41), (nt(45)));
		// offset => shift.
		q(nt(41), (nt(46)));
		// offset => _Roffset_10.
		q(nt(41), (nt(49)));
		// __neg_1 => io_var.
		q(nt(323), (nt(48)));
		// _Rshift_11 => variable & ~( __neg_1 ).
		q(nt(50), (nt(47)) & ~(nt(323)));
		// _Rshift_12 => capture.
		q(nt(51), (nt(45)));
		// _Rshift_12 => _Rshift_11.
		q(nt(51), (nt(50)));
		// shift => _Rshift_12 _ minus _ num.
		q(nt(46), (nt(51)+nt(15)+nt(27)+nt(15)+nt(44)));
		// num => digits.
		q(nt(44), (nt(37)));
		// variable => io_var.
		q(nt(47), (nt(48)));
		// variable => var.
		q(nt(47), (nt(52)));
		// io_var => in.
		q(nt(48), (nt(53)));
		// io_var => out.
		q(nt(48), (nt(54)));
		// in => in_var_name open_bracket offset close_bracket.
		q(nt(53), (nt(55)+nt(23)+nt(41)+nt(24)));
		// out => out_var_name open_bracket offset close_bracket.
		q(nt(54), (nt(56)+nt(23)+nt(41)+nt(24)));
		// __neg_2 => wff_t.
		q(nt(324), (nt(58)));
		// __neg_3 => wff_f.
		q(nt(325), (nt(59)));
		// bool_variable => chars & ~( __neg_2 ) & ~( __neg_3 ).
		q(nt(57), (nt(34)) & ~(nt(324)) & ~(nt(325)));
		// capture => capture_var.
		q(nt(45), (nt(60)));
		// capture_var => '$' chars.
		q(nt(60), (t(20)+nt(34)));
		// var => chars.
		q(nt(52), (nt(34)));
		// in_var_name => 'i' '_' chars.
		q(nt(55), (t(21)+t(22)+nt(34)));
		// out_var_name => 'o' '_' chars.
		q(nt(56), (t(23)+t(22)+nt(34)));
		// tau_rule => tau_matcher _ tau_def _ tau_body _ dot.
		q(nt(61), (nt(62)+nt(15)+nt(19)+nt(15)+nt(63)+nt(15)+nt(20)));
		// tau_matcher => tau.
		q(nt(62), (nt(64)));
		// tau_body => tau.
		q(nt(63), (nt(64)));
		// tau_body => tau_collapse_positives_cb.
		q(nt(63), (nt(65)));
		// tau_body => tau_positives_upwards_cb.
		q(nt(63), (nt(66)));
		// __neg_4 => capture.
		q(nt(326), (nt(45)));
		// __neg_5 => tau_and.
		q(nt(327), (nt(68)));
		// __neg_6 => tau_neg.
		q(nt(328), (nt(69)));
		// __neg_7 => tau_or.
		q(nt(329), (nt(70)));
		// __neg_8 => tau_wff.
		q(nt(330), (nt(71)));
		// _Rtau_rec_relation_13 => ~( __neg_4 ) & tau & ~( __neg_5 ) & ~( __neg_6 ) & ~( __neg_7 ) & ~( __neg_8 ).
		q(nt(72), ~(nt(326)) & (nt(64)) & ~(nt(327)) & ~(nt(328)) & ~(nt(329)) & ~(nt(330)));
		// tau_rec_relation => _Rtau_rec_relation_13 _ tau_def _ tau _ dot.
		q(nt(67), (nt(72)+nt(15)+nt(19)+nt(15)+nt(64)+nt(15)+nt(20)));
		// tau_ref => sym _ offsets _ tau_ref_args.
		q(nt(73), (nt(39)+nt(15)+nt(40)+nt(15)+nt(74)));
		// _Rtau_ref_args_14 => capture.
		q(nt(75), (nt(45)));
		// _Rtau_ref_args_14 => variable.
		q(nt(75), (nt(47)));
		// _Rtau_ref_args_15 => _Rtau_ref_args_14.
		q(nt(76), (nt(75)));
		// _Rtau_ref_args_16 => null.
		q(nt(77), (nul));
		// _Rtau_ref_args_16 => _Rtau_ref_args_15 _Rtau_ref_args_16.
		q(nt(77), (nt(76)+nt(77)));
		// tau_ref_args => open_parenthesis _ _Rtau_ref_args_16 _ close_parenthesis.
		q(nt(74), (nt(21)+nt(15)+nt(77)+nt(15)+nt(22)));
		// tau => capture.
		q(nt(64), (nt(45)));
		// tau => tau_and.
		q(nt(64), (nt(68)));
		// tau => tau_neg.
		q(nt(64), (nt(69)));
		// tau => tau_or.
		q(nt(64), (nt(70)));
		// tau => tau_wff.
		q(nt(64), (nt(71)));
		// tau => tau_ref.
		q(nt(64), (nt(73)));
		// tau_and => open_parenthesis _ tau _ tau_and_sym _ tau _ close_parenthesis.
		q(nt(68), (nt(21)+nt(15)+nt(64)+nt(15)+nt(78)+nt(15)+nt(64)+nt(15)+nt(22)));
		// tau_or => open_parenthesis _ tau _ tau_or_sym _ tau _ close_parenthesis.
		q(nt(70), (nt(21)+nt(15)+nt(64)+nt(15)+nt(79)+nt(15)+nt(64)+nt(15)+nt(22)));
		// tau_neg => tau_neg_sym _ tau.
		q(nt(69), (nt(80)+nt(15)+nt(64)));
		// tau_wff => open_brace _ wff _ close_brace.
		q(nt(71), (nt(25)+nt(15)+nt(81)+nt(15)+nt(26)));
		// tau_and_sym => '&' '&' '&'.
		q(nt(78), (t(24)+t(24)+t(24)));
		// tau_or_sym => '|' '|' '|'.
		q(nt(79), (t(25)+t(25)+t(25)));
		// tau_neg_sym => '!' '!' '!'.
		q(nt(80), (t(26)+t(26)+t(26)));
		// wff_rule => wff_matcher _ wff_def _ wff_body _ dot.
		q(nt(82), (nt(83)+nt(15)+nt(18)+nt(15)+nt(84)+nt(15)+nt(20)));
		// wff_matcher => wff.
		q(nt(83), (nt(81)));
		// wff_body => wff.
		q(nt(84), (nt(81)));
		// wff_body => bf_eq_cb.
		q(nt(84), (nt(85)));
		// wff_body => bf_neq_cb.
		q(nt(84), (nt(86)));
		// wff_body => wff_has_clashing_subformulas_cb.
		q(nt(84), (nt(87)));
		// wff_body => wff_has_subformula_cb.
		q(nt(84), (nt(88)));
		// wff_body => wff_remove_existential_cb.
		q(nt(84), (nt(89)));
		// wff_body => wff_remove_bexistential_cb.
		q(nt(84), (nt(90)));
		// wff_body => wff_remove_buniversal_cb.
		q(nt(84), (nt(91)));
		// __neg_9 => capture.
		q(nt(331), (nt(45)));
		// __neg_10 => bool_variable.
		q(nt(332), (nt(57)));
		// __neg_11 => wff_t.
		q(nt(333), (nt(58)));
		// __neg_12 => wff_f.
		q(nt(334), (nt(59)));
		// __neg_13 => wff_and.
		q(nt(335), (nt(93)));
		// __neg_14 => wff_neg.
		q(nt(336), (nt(94)));
		// __neg_15 => wff_xor.
		q(nt(337), (nt(95)));
		// __neg_16 => wff_conditional.
		q(nt(338), (nt(96)));
		// __neg_17 => wff_or.
		q(nt(339), (nt(97)));
		// __neg_18 => wff_all.
		q(nt(340), (nt(98)));
		// __neg_19 => wff_ex.
		q(nt(341), (nt(99)));
		// __neg_20 => wff_imply.
		q(nt(342), (nt(100)));
		// __neg_21 => wff_equiv.
		q(nt(343), (nt(101)));
		// __neg_22 => wff_ball.
		q(nt(344), (nt(102)));
		// __neg_23 => wff_bex.
		q(nt(345), (nt(103)));
		// __neg_24 => bf_eq.
		q(nt(346), (nt(104)));
		// __neg_25 => bf_neq.
		q(nt(347), (nt(105)));
		// __neg_26 => bf_less.
		q(nt(348), (nt(106)));
		// __neg_27 => bf_less_equal.
		q(nt(349), (nt(107)));
		// __neg_28 => bf_greater.
		q(nt(350), (nt(108)));
		// __neg_29 => bf_interval.
		q(nt(351), (nt(109)));
		// __neg_30 => bf_not_less_equal.
		q(nt(352), (nt(110)));
		// _Rwff_rec_relation_17 => ~( __neg_9 ) & ~( __neg_10 ) & ~( __neg_11 ) & ~( __neg_12 ) & wff & ~( __neg_13 ) & ~( __neg_14 ) & ~( __neg_15 ) & ~( __neg_16 ) & ~( __neg_17 ) & ~( __neg_18 ) & ~( __neg_19 ) & ~( __neg_20 ) & ~( __neg_21 ) & ~( __neg_22 ) & ~( __neg_23 ) & ~( __neg_24 ) & ~( __neg_25 ) & ~( __neg_26 ) & ~( __neg_27 ) & ~( __neg_28 ) & ~( __neg_29 ) & ~( __neg_30 ).
		q(nt(111), ~(nt(331)) & ~(nt(332)) & ~(nt(333)) & ~(nt(334)) & (nt(81)) & ~(nt(335)) & ~(nt(336)) & ~(nt(337)) & ~(nt(338)) & ~(nt(339)) & ~(nt(340)) & ~(nt(341)) & ~(nt(342)) & ~(nt(343)) & ~(nt(344)) & ~(nt(345)) & ~(nt(346)) & ~(nt(347)) & ~(nt(348)) & ~(nt(349)) & ~(nt(350)) & ~(nt(351)) & ~(nt(352)));
		// wff_rec_relation => _Rwff_rec_relation_17 _ wff_def _ wff _ dot.
		q(nt(92), (nt(111)+nt(15)+nt(18)+nt(15)+nt(81)+nt(15)+nt(20)));
		// wff => capture.
		q(nt(81), (nt(45)));
		// wff => bool_variable.
		q(nt(81), (nt(57)));
		// wff => wff_t.
		q(nt(81), (nt(58)));
		// wff => wff_f.
		q(nt(81), (nt(59)));
		// wff => wff_and.
		q(nt(81), (nt(93)));
		// wff => wff_neg.
		q(nt(81), (nt(94)));
		// wff => wff_xor.
		q(nt(81), (nt(95)));
		// wff => wff_conditional.
		q(nt(81), (nt(96)));
		// wff => wff_or.
		q(nt(81), (nt(97)));
		// wff => wff_all.
		q(nt(81), (nt(98)));
		// wff => wff_ex.
		q(nt(81), (nt(99)));
		// wff => wff_imply.
		q(nt(81), (nt(100)));
		// wff => wff_equiv.
		q(nt(81), (nt(101)));
		// wff => wff_ball.
		q(nt(81), (nt(102)));
		// wff => wff_bex.
		q(nt(81), (nt(103)));
		// wff => bf_eq.
		q(nt(81), (nt(104)));
		// wff => bf_neq.
		q(nt(81), (nt(105)));
		// wff => bf_less.
		q(nt(81), (nt(106)));
		// wff => bf_less_equal.
		q(nt(81), (nt(107)));
		// wff => bf_greater.
		q(nt(81), (nt(108)));
		// wff => bf_interval.
		q(nt(81), (nt(109)));
		// wff => bf_not_less_equal.
		q(nt(81), (nt(110)));
		// wff => wff_ref.
		q(nt(81), (nt(112)));
		// wff_ref => sym _ offsets _ wff_ref_args.
		q(nt(112), (nt(39)+nt(15)+nt(40)+nt(15)+nt(113)));
		// _Rwff_ref_args_18 => capture.
		q(nt(114), (nt(45)));
		// _Rwff_ref_args_18 => variable.
		q(nt(114), (nt(47)));
		// _Rwff_ref_args_19 => _Rwff_ref_args_18.
		q(nt(115), (nt(114)));
		// _Rwff_ref_args_20 => null.
		q(nt(116), (nul));
		// _Rwff_ref_args_20 => _Rwff_ref_args_19 _Rwff_ref_args_20.
		q(nt(116), (nt(115)+nt(116)));
		// wff_ref_args => open_parenthesis _ _Rwff_ref_args_20 _ close_parenthesis.
		q(nt(113), (nt(21)+nt(15)+nt(116)+nt(15)+nt(22)));
		// wff_and => open_parenthesis _ wff _ wff_and_sym _ wff _ close_parenthesis.
		q(nt(93), (nt(21)+nt(15)+nt(81)+nt(15)+nt(117)+nt(15)+nt(81)+nt(15)+nt(22)));
		// wff_or => open_parenthesis _ wff _ wff_or_sym _ wff _ close_parenthesis.
		q(nt(97), (nt(21)+nt(15)+nt(81)+nt(15)+nt(118)+nt(15)+nt(81)+nt(15)+nt(22)));
		// wff_xor => open_parenthesis _ wff _ wff_xor_sym _ wff _ close_parenthesis.
		q(nt(95), (nt(21)+nt(15)+nt(81)+nt(15)+nt(119)+nt(15)+nt(81)+nt(15)+nt(22)));
		// wff_conditional => open_parenthesis _ wff _ wff_conditional_sym _ wff _ colon _ wff _ close_parenthesis.
		q(nt(96), (nt(21)+nt(15)+nt(81)+nt(15)+nt(120)+nt(15)+nt(81)+nt(15)+nt(28)+nt(15)+nt(81)+nt(15)+nt(22)));
		// wff_neg => wff_neg_sym _ wff.
		q(nt(94), (nt(121)+nt(15)+nt(81)));
		// wff_imply => open_parenthesis _ wff _ wff_imply_sym _ wff _ close_parenthesis.
		q(nt(100), (nt(21)+nt(15)+nt(81)+nt(15)+nt(122)+nt(15)+nt(81)+nt(15)+nt(22)));
		// wff_equiv => open_parenthesis _ wff _ wff_equiv_sym _ wff _ close_parenthesis.
		q(nt(101), (nt(21)+nt(15)+nt(81)+nt(15)+nt(123)+nt(15)+nt(81)+nt(15)+nt(22)));
		// _Rwff_all_21 => capture.
		q(nt(125), (nt(45)));
		// _Rwff_all_21 => variable.
		q(nt(125), (nt(47)));
		// wff_all => wff_all_sym __ _Rwff_all_21 __ wff.
		q(nt(98), (nt(124)+nt(13)+nt(125)+nt(13)+nt(81)));
		// _Rwff_ex_22 => capture.
		q(nt(127), (nt(45)));
		// _Rwff_ex_22 => variable.
		q(nt(127), (nt(47)));
		// wff_ex => wff_ex_sym __ _Rwff_ex_22 __ wff.
		q(nt(99), (nt(126)+nt(13)+nt(127)+nt(13)+nt(81)));
		// _Rwff_ball_23 => capture.
		q(nt(129), (nt(45)));
		// _Rwff_ball_23 => bool_variable.
		q(nt(129), (nt(57)));
		// wff_ball => wff_ball_sym __ _Rwff_ball_23 __ wff.
		q(nt(102), (nt(128)+nt(13)+nt(129)+nt(13)+nt(81)));
		// _Rwff_bex_24 => capture.
		q(nt(131), (nt(45)));
		// _Rwff_bex_24 => bool_variable.
		q(nt(131), (nt(57)));
		// wff_bex => wff_bex_sym __ _Rwff_bex_24 __ wff.
		q(nt(103), (nt(130)+nt(13)+nt(131)+nt(13)+nt(81)));
		// bf_eq => open_parenthesis _ bf _ bf_equality_sym _ bf _ close_parenthesis.
		q(nt(104), (nt(21)+nt(15)+nt(132)+nt(15)+nt(133)+nt(15)+nt(132)+nt(15)+nt(22)));
		// bf_neq => open_parenthesis _ bf _ bf_nequality_sym _ bf _ close_parenthesis.
		q(nt(105), (nt(21)+nt(15)+nt(132)+nt(15)+nt(134)+nt(15)+nt(132)+nt(15)+nt(22)));
		// bf_less => open_parenthesis _ bf _ bf_less_sym _ bf _ close_parenthesis.
		q(nt(106), (nt(21)+nt(15)+nt(132)+nt(15)+nt(135)+nt(15)+nt(132)+nt(15)+nt(22)));
		// bf_less_equal => open_parenthesis _ bf _ bf_less_equal_sym _ bf _ close_parenthesis.
		q(nt(107), (nt(21)+nt(15)+nt(132)+nt(15)+nt(136)+nt(15)+nt(132)+nt(15)+nt(22)));
		// bf_not_less_equal => open_parenthesis _ bf _ bf_not_less_equal_sym _ bf _ close_parenthesis.
		q(nt(110), (nt(21)+nt(15)+nt(132)+nt(15)+nt(137)+nt(15)+nt(132)+nt(15)+nt(22)));
		// bf_greater => open_parenthesis _ bf _ bf_greater_sym _ bf _ close_parenthesis.
		q(nt(108), (nt(21)+nt(15)+nt(132)+nt(15)+nt(138)+nt(15)+nt(132)+nt(15)+nt(22)));
		// bf_interval => open_parenthesis _ bf _ bf_less_equal_sym _ bf _ bf_less_equal_sym _ bf _ close_parenthesis.
		q(nt(109), (nt(21)+nt(15)+nt(132)+nt(15)+nt(136)+nt(15)+nt(132)+nt(15)+nt(136)+nt(15)+nt(132)+nt(15)+nt(22)));
		// wff_and_sym => '&' '&'.
		q(nt(117), (t(24)+t(24)));
		// wff_or_sym => '|' '|'.
		q(nt(118), (t(25)+t(25)));
		// wff_xor_sym => '^'.
		q(nt(119), (t(27)));
		// wff_conditional_sym => '?'.
		q(nt(120), (t(28)));
		// wff_neg_sym => '!'.
		q(nt(121), (t(26)));
		// wff_imply_sym => '-' '>'.
		q(nt(122), (t(14)+t(29)));
		// wff_equiv_sym => '<' '-' '>'.
		q(nt(123), (t(16)+t(14)+t(29)));
		// wff_all_sym => 'a' 'l' 'l'.
		q(nt(124), (t(30)+t(31)+t(31)));
		// wff_ex_sym => 'e' 'x'.
		q(nt(126), (t(32)+t(33)));
		// wff_ball_sym => 'b' 'a' 'l' 'l'.
		q(nt(128), (t(34)+t(30)+t(31)+t(31)));
		// wff_bex_sym => 'b' 'e' 'x'.
		q(nt(130), (t(34)+t(32)+t(33)));
		// wff_t => 'T'.
		q(nt(58), (t(35)));
		// wff_f => 'F'.
		q(nt(59), (t(36)));
		// bf_rule => bf_matcher _ bf_def _ bf_body _ dot.
		q(nt(139), (nt(140)+nt(15)+nt(17)+nt(15)+nt(141)+nt(15)+nt(20)));
		// bf_matcher => bf.
		q(nt(140), (nt(132)));
		// bf_body => bf_not_less_equal.
		q(nt(141), (nt(110)));
		// bf_body => bf.
		q(nt(141), (nt(132)));
		// bf_body => bf_is_zero_cb.
		q(nt(141), (nt(142)));
		// bf_body => bf_is_one_cb.
		q(nt(141), (nt(143)));
		// bf_body => bf_has_clashing_subformulas_cb.
		q(nt(141), (nt(144)));
		// bf_body => bf_has_subformula_cb.
		q(nt(141), (nt(145)));
		// bf_body => bf_remove_funiversal_cb.
		q(nt(141), (nt(146)));
		// bf_body => bf_remove_fexistential_cb.
		q(nt(141), (nt(147)));
		// bf_body => bf_and_cb.
		q(nt(141), (nt(148)));
		// bf_body => bf_or_cb.
		q(nt(141), (nt(149)));
		// bf_body => bf_xor_cb.
		q(nt(141), (nt(150)));
		// bf_body => bf_neg_cb.
		q(nt(141), (nt(151)));
		// bf => capture.
		q(nt(132), (nt(45)));
		// bf => variable.
		q(nt(132), (nt(47)));
		// bf => bf_ref.
		q(nt(132), (nt(152)));
		// bf => bf_constant.
		q(nt(132), (nt(153)));
		// bf => bf_and.
		q(nt(132), (nt(154)));
		// bf => bf_neg.
		q(nt(132), (nt(155)));
		// bf => bf_xor.
		q(nt(132), (nt(156)));
		// bf => bf_or.
		q(nt(132), (nt(157)));
		// bf => bf_all.
		q(nt(132), (nt(158)));
		// bf => bf_ex.
		q(nt(132), (nt(159)));
		// bf => bf_t.
		q(nt(132), (nt(160)));
		// bf => bf_f.
		q(nt(132), (nt(161)));
		// bf => bf_splitter.
		q(nt(132), (nt(162)));
		// __neg_31 => capture.
		q(nt(353), (nt(45)));
		// __neg_32 => variable.
		q(nt(354), (nt(47)));
		// __neg_33 => bf_eq.
		q(nt(355), (nt(104)));
		// __neg_34 => bf_neq.
		q(nt(356), (nt(105)));
		// __neg_35 => bf_constant.
		q(nt(357), (nt(153)));
		// __neg_36 => bf_and.
		q(nt(358), (nt(154)));
		// __neg_37 => bf_neg.
		q(nt(359), (nt(155)));
		// __neg_38 => bf_xor.
		q(nt(360), (nt(156)));
		// __neg_39 => bf_or.
		q(nt(361), (nt(157)));
		// __neg_40 => bf_all.
		q(nt(362), (nt(158)));
		// __neg_41 => bf_ex.
		q(nt(363), (nt(159)));
		// __neg_42 => bf_t.
		q(nt(364), (nt(160)));
		// __neg_43 => bf_f.
		q(nt(365), (nt(161)));
		// __neg_44 => bf_splitter.
		q(nt(366), (nt(162)));
		// _Rbf_rec_relation_25 => ~( __neg_31 ) & ~( __neg_32 ) & ~( __neg_33 ) & ~( __neg_34 ) & bf & ~( __neg_35 ) & ~( __neg_36 ) & ~( __neg_37 ) & ~( __neg_38 ) & ~( __neg_39 ) & ~( __neg_40 ) & ~( __neg_41 ) & ~( __neg_42 ) & ~( __neg_43 ) & ~( __neg_44 ).
		q(nt(164), ~(nt(353)) & ~(nt(354)) & ~(nt(355)) & ~(nt(356)) & (nt(132)) & ~(nt(357)) & ~(nt(358)) & ~(nt(359)) & ~(nt(360)) & ~(nt(361)) & ~(nt(362)) & ~(nt(363)) & ~(nt(364)) & ~(nt(365)) & ~(nt(366)));
		// bf_rec_relation => _Rbf_rec_relation_25 _ bf_def _ bf _ dot.
		q(nt(163), (nt(164)+nt(15)+nt(17)+nt(15)+nt(132)+nt(15)+nt(20)));
		// bf_ref => sym offsets bf_ref_args.
		q(nt(152), (nt(39)+nt(40)+nt(165)));
		// _Rbf_ref_args_26 => capture.
		q(nt(166), (nt(45)));
		// _Rbf_ref_args_26 => variable.
		q(nt(166), (nt(47)));
		// _Rbf_ref_args_27 => _Rbf_ref_args_26.
		q(nt(167), (nt(166)));
		// _Rbf_ref_args_28 => null.
		q(nt(168), (nul));
		// _Rbf_ref_args_28 => _Rbf_ref_args_27 _Rbf_ref_args_28.
		q(nt(168), (nt(167)+nt(168)));
		// bf_ref_args => open_parenthesis _ _Rbf_ref_args_28 _ close_parenthesis.
		q(nt(165), (nt(21)+nt(15)+nt(168)+nt(15)+nt(22)));
		// bf_and => open_parenthesis _ bf _ bf_and_sym _ bf _ close_parenthesis.
		q(nt(154), (nt(21)+nt(15)+nt(132)+nt(15)+nt(169)+nt(15)+nt(132)+nt(15)+nt(22)));
		// bf_or => open_parenthesis _ bf _ bf_or_sym _ bf _ close_parenthesis.
		q(nt(157), (nt(21)+nt(15)+nt(132)+nt(15)+nt(170)+nt(15)+nt(132)+nt(15)+nt(22)));
		// bf_xor => open_parenthesis _ bf _ bf_xor_sym _ bf _ close_parenthesis.
		q(nt(156), (nt(21)+nt(15)+nt(132)+nt(15)+nt(171)+nt(15)+nt(132)+nt(15)+nt(22)));
		// bf_neg => bf _ bf_neg_sym.
		q(nt(155), (nt(132)+nt(15)+nt(172)));
		// _Rbf_all_29 => capture.
		q(nt(174), (nt(45)));
		// _Rbf_all_29 => variable.
		q(nt(174), (nt(47)));
		// bf_all => bf_all_sym __ _Rbf_all_29 __ bf.
		q(nt(158), (nt(173)+nt(13)+nt(174)+nt(13)+nt(132)));
		// _Rbf_ex_30 => capture.
		q(nt(176), (nt(45)));
		// _Rbf_ex_30 => variable.
		q(nt(176), (nt(47)));
		// bf_ex => bf_ex_sym __ _Rbf_ex_30 __ bf.
		q(nt(159), (nt(175)+nt(13)+nt(176)+nt(13)+nt(132)));
		// bf_splitter => bf_splitter_sym _ open_parenthesis _ bf _ close_parenthesis.
		q(nt(162), (nt(177)+nt(15)+nt(21)+nt(15)+nt(132)+nt(15)+nt(22)));
		// bf_and_sym => '&'.
		q(nt(169), (t(24)));
		// bf_or_sym => '|'.
		q(nt(170), (t(25)));
		// bf_xor_sym => '+'.
		q(nt(171), (t(37)));
		// bf_neg_sym => apostrophe.
		q(nt(172), (nt(32)));
		// bf_equality_sym => '='.
		q(nt(133), (t(6)));
		// bf_nequality_sym => '!' '='.
		q(nt(134), (t(26)+t(6)));
		// bf_less_sym => '<'.
		q(nt(135), (t(16)));
		// bf_less_equal_sym => '<' '='.
		q(nt(136), (t(16)+t(6)));
		// bf_not_less_equal_sym => '!' '<' '='.
		q(nt(137), (t(26)+t(16)+t(6)));
		// bf_greater_sym => '>'.
		q(nt(138), (t(29)));
		// bf_all_sym => 'f' 'a' 'l' 'l'.
		q(nt(173), (t(38)+t(30)+t(31)+t(31)));
		// bf_ex_sym => 'f' 'e' 'x'.
		q(nt(175), (t(38)+t(32)+t(33)));
		// bf_splitter_sym => 'S'.
		q(nt(177), (t(39)));
		// bf_t => '1'.
		q(nt(160), (t(40)));
		// bf_f => '0'.
		q(nt(161), (t(41)));
		// bf_constant => open_brace _ constant _ close_brace.
		q(nt(153), (nt(25)+nt(15)+nt(178)+nt(15)+nt(26)));
		// constant => capture.
		q(nt(178), (nt(45)));
		// constant => binding.
		q(nt(178), (nt(179)));
		// binding => source_binding.
		q(nt(179), (nt(180)));
		// binding => named_binding.
		q(nt(179), (nt(181)));
		// named_binding => chars.
		q(nt(181), (nt(34)));
		// source_binding => type _ colon _ source.
		q(nt(180), (nt(182)+nt(15)+nt(28)+nt(15)+nt(183)));
		// type => null.
		q(nt(182), (nul));
		// type => chars.
		q(nt(182), (nt(34)));
		// source0 => space.
		q(nt(184), (nt(2)));
		// source0 => alnum.
		q(nt(184), (nt(6)));
		// source0 => punct.
		q(nt(184), (nt(7)));
		// _Rsource_31 => source0.
		q(nt(185), (nt(184)));
		// _Rsource_32 => _Rsource_31.
		q(nt(186), (nt(185)));
		// _Rsource_32 => _Rsource_31 _Rsource_32.
		q(nt(186), (nt(185)+nt(186)));
		// source => _Rsource_32.
		q(nt(183), (nt(186)));
		// bf_and_cb => bf_and_cb_sym __ bf_cb_arg __ bf_cb_arg.
		q(nt(148), (nt(187)+nt(13)+nt(188)+nt(13)+nt(188)));
		// bf_or_cb => bf_or_cb_sym __ bf_cb_arg __ bf_cb_arg.
		q(nt(149), (nt(189)+nt(13)+nt(188)+nt(13)+nt(188)));
		// bf_xor_cb => bf_xor_cb_sym __ bf_cb_arg __ bf_cb_arg.
		q(nt(150), (nt(190)+nt(13)+nt(188)+nt(13)+nt(188)));
		// bf_neg_cb => bf_neg_cb_sym __ bf_cb_arg.
		q(nt(151), (nt(191)+nt(13)+nt(188)));
		// bf_eq_cb => bf_eq_cb_sym __ bf_cb_arg __ wff_cb_arg __ wff_cb_arg.
		q(nt(85), (nt(192)+nt(13)+nt(188)+nt(13)+nt(193)+nt(13)+nt(193)));
		// bf_neq_cb => bf_neq_cb_sym __ bf_cb_arg __ wff_cb_arg __ wff_cb_arg.
		q(nt(86), (nt(194)+nt(13)+nt(188)+nt(13)+nt(193)+nt(13)+nt(193)));
		// bf_is_zero_cb => bf_is_zero_cb_sym __ bf_cb_arg __ bf_cb_arg.
		q(nt(142), (nt(195)+nt(13)+nt(188)+nt(13)+nt(188)));
		// bf_is_one_cb => bf_is_one_cb_sym __ bf_cb_arg __ bf_cb_arg.
		q(nt(143), (nt(196)+nt(13)+nt(188)+nt(13)+nt(188)));
		// bf_remove_funiversal_cb => bf_remove_funiversal_cb_sym __ bf_cb_arg __ bf_cb_arg __ bf_cb_arg __ bf_cb_arg.
		q(nt(146), (nt(197)+nt(13)+nt(188)+nt(13)+nt(188)+nt(13)+nt(188)+nt(13)+nt(188)));
		// bf_remove_fexistential_cb => bf_remove_fexistential_cb_sym __ bf_cb_arg __ bf_cb_arg __ bf_cb_arg __ bf_cb_arg.
		q(nt(147), (nt(198)+nt(13)+nt(188)+nt(13)+nt(188)+nt(13)+nt(188)+nt(13)+nt(188)));
		// wff_remove_existential_cb => wff_remove_existential_cb_sym __ wff_cb_arg __ wff_cb_arg.
		q(nt(89), (nt(199)+nt(13)+nt(193)+nt(13)+nt(193)));
		// wff_remove_bexistential_cb => wff_remove_bexistential_cb_sym __ wff_cb_arg __ wff_cb_arg __ wff_cb_arg __ wff_cb_arg.
		q(nt(90), (nt(200)+nt(13)+nt(193)+nt(13)+nt(193)+nt(13)+nt(193)+nt(13)+nt(193)));
		// wff_remove_buniversal_cb => wff_remove_buniversal_cb_sym __ wff_cb_arg __ wff_cb_arg __ wff_cb_arg __ wff_cb_arg.
		q(nt(91), (nt(201)+nt(13)+nt(193)+nt(13)+nt(193)+nt(13)+nt(193)+nt(13)+nt(193)));
		// bf_has_clashing_subformulas_cb => bf_has_clashing_subformulas_cb_sym __ bf_cb_arg __ bf_cb_arg.
		q(nt(144), (nt(202)+nt(13)+nt(188)+nt(13)+nt(188)));
		// wff_has_clashing_subformulas_cb => wff_has_clashing_subformulas_cb_sym __ wff_cb_arg __ wff_cb_arg.
		q(nt(87), (nt(203)+nt(13)+nt(193)+nt(13)+nt(193)));
		// bf_has_subformula_cb => bf_has_subformula_cb_sym __ bf_cb_arg __ bf_cb_arg __ bf_cb_arg.
		q(nt(145), (nt(204)+nt(13)+nt(188)+nt(13)+nt(188)+nt(13)+nt(188)));
		// wff_has_subformula_cb => wff_has_subformula_cb_sym __ wff_cb_arg __ wff_cb_arg __ wff_cb_arg.
		q(nt(88), (nt(205)+nt(13)+nt(193)+nt(13)+nt(193)+nt(13)+nt(193)));
		// bf_has_clashing_subformulas_cb_sym => 'b' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b'.
		q(nt(202), (t(34)+t(38)+t(22)+t(42)+t(30)+t(43)+t(22)+t(44)+t(31)+t(30)+t(43)+t(42)+t(21)+t(45)+t(46)+t(22)+t(43)+t(47)+t(34)+t(38)+t(23)+t(48)+t(49)+t(47)+t(31)+t(30)+t(43)+t(22)+t(44)+t(34)));
		// bf_has_subformula_cb_sym => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b'.
		q(nt(204), (t(34)+t(38)+t(22)+t(42)+t(30)+t(43)+t(22)+t(43)+t(47)+t(34)+t(38)+t(23)+t(48)+t(49)+t(47)+t(31)+t(30)+t(22)+t(44)+t(34)));
		// wff_has_clashing_subformulas_cb_sym => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b'.
		q(nt(203), (t(50)+t(38)+t(38)+t(22)+t(42)+t(30)+t(43)+t(22)+t(44)+t(31)+t(30)+t(43)+t(42)+t(21)+t(45)+t(46)+t(22)+t(43)+t(47)+t(34)+t(38)+t(23)+t(48)+t(49)+t(47)+t(31)+t(30)+t(43)+t(22)+t(44)+t(34)));
		// wff_has_subformula_cb_sym => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b'.
		q(nt(205), (t(50)+t(38)+t(38)+t(22)+t(42)+t(30)+t(43)+t(22)+t(43)+t(47)+t(34)+t(38)+t(23)+t(48)+t(49)+t(47)+t(31)+t(30)+t(22)+t(44)+t(34)));
		// wff_remove_existential_cb_sym => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(199), (t(50)+t(38)+t(38)+t(22)+t(48)+t(32)+t(49)+t(23)+t(51)+t(32)+t(22)+t(32)+t(33)+t(21)+t(43)+t(52)+t(32)+t(45)+t(52)+t(21)+t(30)+t(31)+t(22)+t(44)+t(34)));
		// wff_remove_bexistential_cb_sym => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(200), (t(50)+t(38)+t(38)+t(22)+t(48)+t(32)+t(49)+t(23)+t(51)+t(32)+t(22)+t(34)+t(32)+t(33)+t(21)+t(43)+t(52)+t(32)+t(45)+t(52)+t(21)+t(30)+t(31)+t(22)+t(44)+t(34)));
		// wff_remove_buniversal_cb_sym => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b'.
		q(nt(201), (t(50)+t(38)+t(38)+t(22)+t(48)+t(32)+t(49)+t(23)+t(51)+t(32)+t(22)+t(34)+t(47)+t(45)+t(21)+t(51)+t(32)+t(48)+t(43)+t(30)+t(31)+t(22)+t(44)+t(34)));
		// bf_remove_fexistential_cb_sym => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(198), (t(34)+t(38)+t(22)+t(48)+t(32)+t(49)+t(23)+t(51)+t(32)+t(22)+t(38)+t(32)+t(33)+t(21)+t(43)+t(52)+t(32)+t(45)+t(52)+t(21)+t(30)+t(31)+t(22)+t(44)+t(34)));
		// bf_remove_funiversal_cb_sym => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b'.
		q(nt(197), (t(34)+t(38)+t(22)+t(48)+t(32)+t(49)+t(23)+t(51)+t(32)+t(22)+t(38)+t(47)+t(45)+t(21)+t(51)+t(32)+t(48)+t(43)+t(30)+t(31)+t(22)+t(44)+t(34)));
		// bf_cb_arg => bf.
		q(nt(188), (nt(132)));
		// wff_cb_arg => wff.
		q(nt(193), (nt(81)));
		// bf_and_cb_sym => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b'.
		q(nt(187), (t(34)+t(38)+t(22)+t(30)+t(45)+t(53)+t(22)+t(44)+t(34)));
		// bf_or_cb_sym => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b'.
		q(nt(189), (t(34)+t(38)+t(22)+t(23)+t(48)+t(22)+t(44)+t(34)));
		// bf_xor_cb_sym => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b'.
		q(nt(190), (t(34)+t(38)+t(22)+t(33)+t(23)+t(48)+t(22)+t(44)+t(34)));
		// bf_neg_cb_sym => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b'.
		q(nt(191), (t(34)+t(38)+t(22)+t(45)+t(32)+t(46)+t(22)+t(44)+t(34)));
		// bf_eq_cb_sym => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b'.
		q(nt(192), (t(34)+t(38)+t(22)+t(32)+t(54)+t(22)+t(44)+t(34)));
		// bf_neq_cb_sym => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b'.
		q(nt(194), (t(34)+t(38)+t(22)+t(45)+t(32)+t(54)+t(22)+t(44)+t(34)));
		// bf_is_zero_cb_sym => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b'.
		q(nt(195), (t(34)+t(38)+t(22)+t(21)+t(43)+t(22)+t(55)+t(32)+t(48)+t(23)+t(22)+t(44)+t(34)));
		// bf_is_one_cb_sym => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b'.
		q(nt(196), (t(34)+t(38)+t(22)+t(21)+t(43)+t(22)+t(23)+t(45)+t(32)+t(22)+t(44)+t(34)));
		// _Rtau_collapse_positives_cb_33 => tau_collapse_positives_cb_sym __ tau_cb_arg __ tau_cb_arg __ tau_cb_arg.
		q(nt(208), (nt(206)+nt(13)+nt(207)+nt(13)+nt(207)+nt(13)+nt(207)));
		// _Rtau_collapse_positives_cb_34 => tau_collapse_positives_cb_sym __ tau_cb_arg __ tau_cb_arg.
		q(nt(209), (nt(206)+nt(13)+nt(207)+nt(13)+nt(207)));
		// tau_collapse_positives_cb => _Rtau_collapse_positives_cb_33.
		q(nt(65), (nt(208)));
		// tau_collapse_positives_cb => _Rtau_collapse_positives_cb_34.
		q(nt(65), (nt(209)));
		// tau_positives_upwards_cb => tau_positives_upwards_cb_sym __ tau_cb_arg __ tau_cb_arg.
		q(nt(66), (nt(210)+nt(13)+nt(207)+nt(13)+nt(207)));
		// tau_cb_arg => capture.
		q(nt(207), (nt(45)));
		// tau_cb_arg => tau.
		q(nt(207), (nt(64)));
		// tau_collapse_positives_cb_sym => 't' 'a' 'u' '_' 'c' 'o' 'l' 'l' 'a' 'p' 's' 'e' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'c' 'b'.
		q(nt(206), (t(52)+t(30)+t(47)+t(22)+t(44)+t(23)+t(31)+t(31)+t(30)+t(56)+t(43)+t(32)+t(22)+t(56)+t(23)+t(43)+t(21)+t(52)+t(21)+t(51)+t(32)+t(43)+t(22)+t(44)+t(34)));
		// tau_positives_upwards_cb_sym => 't' 'a' 'u' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'u' 'p' 'w' 'a' 'r' 'd' 's' '_' 'c' 'b'.
		q(nt(210), (t(52)+t(30)+t(47)+t(22)+t(56)+t(23)+t(43)+t(21)+t(52)+t(21)+t(51)+t(32)+t(43)+t(22)+t(47)+t(56)+t(50)+t(30)+t(48)+t(53)+t(43)+t(22)+t(44)+t(34)));
		// _Rinputs_35 => _ input.
		q(nt(213), (nt(15)+nt(212)));
		// _Rinputs_36 => null.
		q(nt(214), (nul));
		// _Rinputs_36 => _Rinputs_35 _Rinputs_36.
		q(nt(214), (nt(213)+nt(214)));
		// inputs => _ less _ input _Rinputs_36 _ dot.
		q(nt(211), (nt(15)+nt(30)+nt(15)+nt(212)+nt(214)+nt(15)+nt(20)));
		// input => in _ colon _ open_brace _ source_binding _ close_brace.
		q(nt(212), (nt(53)+nt(15)+nt(28)+nt(15)+nt(25)+nt(15)+nt(180)+nt(15)+nt(26)));
		// builder => _ builder_head _ builder_body _ dot.
		q(nt(215), (nt(15)+nt(216)+nt(15)+nt(217)+nt(15)+nt(20)));
		// _Rbuilder_head_37 => __ capture.
		q(nt(218), (nt(13)+nt(45)));
		// _Rbuilder_head_38 => null.
		q(nt(219), (nul));
		// _Rbuilder_head_38 => _Rbuilder_head_37 _Rbuilder_head_38.
		q(nt(219), (nt(218)+nt(219)));
		// builder_head => open_parenthesis _ capture _Rbuilder_head_38 _ close_parenthesis.
		q(nt(216), (nt(21)+nt(15)+nt(45)+nt(219)+nt(15)+nt(22)));
		// builder_body => bf_builder_body.
		q(nt(217), (nt(220)));
		// builder_body => wff_builder_body.
		q(nt(217), (nt(221)));
		// builder_body => tau_builder_body.
		q(nt(217), (nt(222)));
		// bf_builder_body => bf_def _ bf.
		q(nt(220), (nt(17)+nt(15)+nt(132)));
		// wff_builder_body => wff_def _ wff.
		q(nt(221), (nt(18)+nt(15)+nt(81)));
		// tau_builder_body => tau_def _ tau.
		q(nt(222), (nt(19)+nt(15)+nt(64)));
		// library => rules.
		q(nt(223), (nt(224)));
		// _Rrules_39 => _ rule.
		q(nt(226), (nt(15)+nt(225)));
		// _Rrules_40 => null.
		q(nt(227), (nul));
		// _Rrules_40 => _Rrules_39 _Rrules_40.
		q(nt(227), (nt(226)+nt(227)));
		// rules => _Rrules_40.
		q(nt(224), (nt(227)));
		// rule => tau_rule.
		q(nt(225), (nt(61)));
		// rule => wff_rule.
		q(nt(225), (nt(82)));
		// rule => bf_rule.
		q(nt(225), (nt(139)));
		// nso_rr => nso_rec_relations nso_main.
		q(nt(228), (nt(229)+nt(230)));
		// _Rnso_rec_relations_41 => _ nso_rec_relation.
		q(nt(232), (nt(15)+nt(231)));
		// _Rnso_rec_relations_42 => null.
		q(nt(233), (nul));
		// _Rnso_rec_relations_42 => _Rnso_rec_relations_41 _Rnso_rec_relations_42.
		q(nt(233), (nt(232)+nt(233)));
		// nso_rec_relations => _Rnso_rec_relations_42.
		q(nt(229), (nt(233)));
		// nso_rec_relation => wff_rec_relation.
		q(nt(231), (nt(92)));
		// nso_rec_relation => bf_rec_relation.
		q(nt(231), (nt(163)));
		// nso_main => _ wff _ dot.
		q(nt(230), (nt(15)+nt(81)+nt(15)+nt(20)));
		// gssotc_rr => gssotc_rec_relations gssotc_main.
		q(nt(234), (nt(235)+nt(236)));
		// _Rgssotc_rec_relations_43 => _ gssotc_rec_relation.
		q(nt(238), (nt(15)+nt(237)));
		// _Rgssotc_rec_relations_44 => null.
		q(nt(239), (nul));
		// _Rgssotc_rec_relations_44 => _Rgssotc_rec_relations_43 _Rgssotc_rec_relations_44.
		q(nt(239), (nt(238)+nt(239)));
		// gssotc_rec_relations => _Rgssotc_rec_relations_44.
		q(nt(235), (nt(239)));
		// gssotc_rec_relation => tau_rec_relation.
		q(nt(237), (nt(67)));
		// gssotc_rec_relation => wff_rec_relation.
		q(nt(237), (nt(92)));
		// gssotc_rec_relation => bf_rec_relation.
		q(nt(237), (nt(163)));
		// gssotc_main => _ tau _ semicolon.
		q(nt(236), (nt(15)+nt(64)+nt(15)+nt(29)));
		// _Rstart_45 => inputs.
		q(nt(241), (nt(211)));
		// _Rstart_45 => builder.
		q(nt(241), (nt(215)));
		// _Rstart_45 => library.
		q(nt(241), (nt(223)));
		// _Rstart_45 => nso_rr.
		q(nt(241), (nt(228)));
		// _Rstart_45 => gssotc_rr.
		q(nt(241), (nt(234)));
		// start => _Rstart_45 _.
		q(nt(240), (nt(241)+nt(15)));
		// _Rcli_46 => _ dot _ cli_command.
		q(nt(244), (nt(15)+nt(20)+nt(15)+nt(243)));
		// _Rcli_47 => null.
		q(nt(245), (nul));
		// _Rcli_47 => _Rcli_46 _Rcli_47.
		q(nt(245), (nt(244)+nt(245)));
		// _Rcli_48 => null.
		q(nt(246), (nul));
		// _Rcli_48 => _ dot.
		q(nt(246), (nt(15)+nt(20)));
		// cli => _ cli_command _Rcli_47 _Rcli_48 _.
		q(nt(242), (nt(15)+nt(243)+nt(245)+nt(246)+nt(15)));
		// __neg_45 => help.
		q(nt(367), (nt(253)));
		// __neg_46 => version.
		q(nt(368), (nt(254)));
		// __neg_47 => quit.
		q(nt(369), (nt(255)));
		// __neg_48 => get.
		q(nt(370), (nt(256)));
		// __neg_49 => list_outputs.
		q(nt(371), (nt(257)));
		// _Rcli_command_49 => bf & ~( __neg_45 ) & ~( __neg_46 ) & ~( __neg_47 ) & ~( __neg_48 ) & ~( __neg_49 ).
		q(nt(258), (nt(132)) & ~(nt(367)) & ~(nt(368)) & ~(nt(369)) & ~(nt(370)) & ~(nt(371)));
		// cli_command => wff.
		q(nt(243), (nt(81)));
		// cli_command => normalize.
		q(nt(243), (nt(247)));
		// cli_command => bf_substitute.
		q(nt(243), (nt(248)));
		// cli_command => wff_substitute.
		q(nt(243), (nt(249)));
		// cli_command => bf_instantiate.
		q(nt(243), (nt(250)));
		// cli_command => wff_instantiate.
		q(nt(243), (nt(251)));
		// cli_command => rr_nso.
		q(nt(243), (nt(252)));
		// cli_command => help.
		q(nt(243), (nt(253)));
		// cli_command => version.
		q(nt(243), (nt(254)));
		// cli_command => quit.
		q(nt(243), (nt(255)));
		// cli_command => get.
		q(nt(243), (nt(256)));
		// cli_command => list_outputs.
		q(nt(243), (nt(257)));
		// cli_command => _Rcli_command_49.
		q(nt(243), (nt(258)));
		// cli_command => clear_outputs.
		q(nt(243), (nt(259)));
		// cli_command => print_output.
		q(nt(243), (nt(260)));
		// cli_command => file.
		q(nt(243), (nt(261)));
		// cli_command => toggle.
		q(nt(243), (nt(262)));
		// cli_command => set.
		q(nt(243), (nt(263)));
		// _Rnormalize_50 => q_nso_rr.
		q(nt(269), (nt(265)));
		// _Rnormalize_50 => q_wff.
		q(nt(269), (nt(266)));
		// _Rnormalize_50 => output.
		q(nt(269), (nt(267)));
		// _Rnormalize_50 => wff_selection.
		q(nt(269), (nt(268)));
		// normalize => normalize_sym __ _Rnormalize_50.
		q(nt(247), (nt(264)+nt(13)+nt(269)));
		// file => file_sym quote file_path quote.
		q(nt(261), (nt(270)+nt(33)+nt(271)+nt(33)));
		// _Rbf_instantiate_51 => output.
		q(nt(275), (nt(267)));
		// _Rbf_instantiate_51 => q_var.
		q(nt(275), (nt(273)));
		// _Rbf_instantiate_51 => bf_var_selection.
		q(nt(275), (nt(274)));
		// _Rbf_instantiate_52 => output.
		q(nt(278), (nt(267)));
		// _Rbf_instantiate_52 => q_bf.
		q(nt(278), (nt(276)));
		// _Rbf_instantiate_52 => bf_selection.
		q(nt(278), (nt(277)));
		// _Rbf_instantiate_53 => output.
		q(nt(279), (nt(267)));
		// _Rbf_instantiate_53 => bf_selection.
		q(nt(279), (nt(277)));
		// bf_instantiate => instantiate_sym __ _Rbf_instantiate_51 __ _Rbf_instantiate_52 __ _Rbf_instantiate_53.
		q(nt(250), (nt(272)+nt(13)+nt(275)+nt(13)+nt(278)+nt(13)+nt(279)));
		// _Rwff_instantiate_54 => output.
		q(nt(281), (nt(267)));
		// _Rwff_instantiate_54 => q_var.
		q(nt(281), (nt(273)));
		// _Rwff_instantiate_54 => wff_var_selection.
		q(nt(281), (nt(280)));
		// _Rwff_instantiate_55 => q_wff.
		q(nt(282), (nt(266)));
		// _Rwff_instantiate_55 => output.
		q(nt(282), (nt(267)));
		// _Rwff_instantiate_55 => wff_selection.
		q(nt(282), (nt(268)));
		// _Rwff_instantiate_56 => output.
		q(nt(283), (nt(267)));
		// _Rwff_instantiate_56 => wff_selection.
		q(nt(283), (nt(268)));
		// wff_instantiate => instantiate_sym __ _Rwff_instantiate_54 __ _Rwff_instantiate_55 __ _Rwff_instantiate_56.
		q(nt(251), (nt(272)+nt(13)+nt(281)+nt(13)+nt(282)+nt(13)+nt(283)));
		// _Rbf_substitute_57 => output.
		q(nt(285), (nt(267)));
		// _Rbf_substitute_57 => q_bf.
		q(nt(285), (nt(276)));
		// _Rbf_substitute_57 => bf_selection.
		q(nt(285), (nt(277)));
		// _Rbf_substitute_58 => output.
		q(nt(286), (nt(267)));
		// _Rbf_substitute_58 => wff_selection.
		q(nt(286), (nt(268)));
		// _Rbf_substitute_58 => bf_selection.
		q(nt(286), (nt(277)));
		// bf_substitute => substitute_sym __ _Rbf_substitute_57 __ _Rbf_substitute_58.
		q(nt(248), (nt(284)+nt(13)+nt(285)+nt(13)+nt(286)));
		// _Rwff_substitute_59 => q_wff.
		q(nt(287), (nt(266)));
		// _Rwff_substitute_59 => output.
		q(nt(287), (nt(267)));
		// _Rwff_substitute_59 => wff_selection.
		q(nt(287), (nt(268)));
		// _Rwff_substitute_60 => output.
		q(nt(288), (nt(267)));
		// _Rwff_substitute_60 => wff_selection.
		q(nt(288), (nt(268)));
		// wff_substitute => substitute_sym __ _Rwff_substitute_59 __ _Rwff_substitute_60.
		q(nt(249), (nt(284)+nt(13)+nt(287)+nt(13)+nt(288)));
		// _Rhelp_61 => null.
		q(nt(291), (nul));
		// _Rhelp_61 => __ cli_cmd_sym.
		q(nt(291), (nt(13)+nt(290)));
		// help => help_sym _Rhelp_61.
		q(nt(253), (nt(289)+nt(291)));
		// version => version_sym.
		q(nt(254), (nt(292)));
		// quit => quit_sym.
		q(nt(255), (nt(293)));
		// wff_selection => selection_sym __ digits.
		q(nt(268), (nt(294)+nt(13)+nt(37)));
		// bf_selection => selection_sym __ digits.
		q(nt(277), (nt(294)+nt(13)+nt(37)));
		// bf_var_selection => selection_sym __ digits.
		q(nt(274), (nt(294)+nt(13)+nt(37)));
		// wff_var_selection => selection_sym __ digits.
		q(nt(280), (nt(294)+nt(13)+nt(37)));
		// q_nso_rr => apostrophe _ nso_rr _ apostrophe.
		q(nt(265), (nt(32)+nt(15)+nt(228)+nt(15)+nt(32)));
		// q_wff => apostrophe _ wff _ apostrophe.
		q(nt(266), (nt(32)+nt(15)+nt(81)+nt(15)+nt(32)));
		// q_bf => apostrophe _ bf _ apostrophe.
		q(nt(276), (nt(32)+nt(15)+nt(132)+nt(15)+nt(32)));
		// q_var => apostrophe _ var _ apostrophe.
		q(nt(273), (nt(32)+nt(15)+nt(52)+nt(15)+nt(32)));
		// file_path => chars.
		q(nt(271), (nt(34)));
		// output => absolute_output.
		q(nt(267), (nt(295)));
		// output => relative_output.
		q(nt(267), (nt(296)));
		// _Rabsolute_output_62 => absolute_output_sym.
		q(nt(299), (nt(297)));
		// _Rabsolute_output_62 => output_sym __.
		q(nt(299), (nt(298)+nt(13)));
		// absolute_output => _Rabsolute_output_62 output_id.
		q(nt(295), (nt(299)+nt(300)));
		// relative_output => relative_output_sym output_id.
		q(nt(296), (nt(301)+nt(300)));
		// output_id => digits.
		q(nt(300), (nt(37)));
		// list_outputs => absolute_output_sym.
		q(nt(257), (nt(297)));
		// list_outputs => output_sym.
		q(nt(257), (nt(298)));
		// list_outputs => relative_output_sym.
		q(nt(257), (nt(301)));
		// _Rclear_outputs_63 => absolute_output_sym.
		q(nt(302), (nt(297)));
		// _Rclear_outputs_63 => output_sym __.
		q(nt(302), (nt(298)+nt(13)));
		// _Rclear_outputs_63 => relative_output_sym.
		q(nt(302), (nt(301)));
		// clear_outputs => _Rclear_outputs_63 clear_sym.
		q(nt(259), (nt(302)+nt(303)));
		// print_output => output.
		q(nt(260), (nt(267)));
		// cli_cmd_sym => normalize_sym.
		q(nt(290), (nt(264)));
		// cli_cmd_sym => file_sym.
		q(nt(290), (nt(270)));
		// cli_cmd_sym => instantiate_sym.
		q(nt(290), (nt(272)));
		// cli_cmd_sym => substitute_sym.
		q(nt(290), (nt(284)));
		// cli_cmd_sym => help_sym.
		q(nt(290), (nt(289)));
		// cli_cmd_sym => version_sym.
		q(nt(290), (nt(292)));
		// cli_cmd_sym => quit_sym.
		q(nt(290), (nt(293)));
		// cli_cmd_sym => selection_sym.
		q(nt(290), (nt(294)));
		// cli_cmd_sym => output_sym.
		q(nt(290), (nt(298)));
		// cli_cmd_sym => get_sym.
		q(nt(290), (nt(304)));
		// cli_cmd_sym => set_sym.
		q(nt(290), (nt(305)));
		// cli_cmd_sym => toggle_sym.
		q(nt(290), (nt(306)));
		// help_sym => 'h'.
		q(nt(289), (t(42)));
		// help_sym => 'h' 'e' 'l' 'p'.
		q(nt(289), (t(42)+t(32)+t(31)+t(56)));
		// version_sym => 'v'.
		q(nt(292), (t(51)));
		// version_sym => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
		q(nt(292), (t(51)+t(32)+t(48)+t(43)+t(21)+t(23)+t(45)));
		// quit_sym => 'e'.
		q(nt(293), (t(32)));
		// quit_sym => 'e' 'x' 'i' 't'.
		q(nt(293), (t(32)+t(33)+t(21)+t(52)));
		// quit_sym => 'q'.
		q(nt(293), (t(54)));
		// quit_sym => 'q' 'u' 'i' 't'.
		q(nt(293), (t(54)+t(47)+t(21)+t(52)));
		// output_sym => 'o'.
		q(nt(298), (t(23)));
		// output_sym => 'o' 'u' 't' 'p' 'u' 't'.
		q(nt(298), (t(23)+t(47)+t(52)+t(56)+t(47)+t(52)));
		// selection_sym => 's'.
		q(nt(294), (t(43)));
		// selection_sym => 's' 'e' 'l' 'e' 'c' 't' 'i' 'o' 'n'.
		q(nt(294), (t(43)+t(32)+t(31)+t(32)+t(44)+t(52)+t(21)+t(23)+t(45)));
		// instantiate_sym => 'i'.
		q(nt(272), (t(21)));
		// instantiate_sym => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
		q(nt(272), (t(21)+t(45)+t(43)+t(52)+t(30)+t(45)+t(52)+t(21)+t(30)+t(52)+t(32)));
		// substitute_sym => 's'.
		q(nt(284), (t(43)));
		// substitute_sym => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
		q(nt(284), (t(43)+t(47)+t(34)+t(43)+t(52)+t(21)+t(52)+t(47)+t(52)+t(32)));
		// normalize_sym => 'n'.
		q(nt(264), (t(45)));
		// normalize_sym => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
		q(nt(264), (t(45)+t(23)+t(48)+t(49)+t(30)+t(31)+t(21)+t(55)+t(32)));
		// file_sym => 'r'.
		q(nt(270), (t(48)));
		// file_sym => 'r' 'e' 'a' 'd'.
		q(nt(270), (t(48)+t(32)+t(30)+t(53)));
		// clear_sym => 'c'.
		q(nt(303), (t(44)));
		// clear_sym => 'c' 'l' 'e' 'a' 'r'.
		q(nt(303), (t(44)+t(31)+t(32)+t(30)+t(48)));
		// absolute_output_sym => '&'.
		q(nt(297), (t(24)));
		// relative_output_sym => '%'.
		q(nt(301), (t(57)));
		// _Rget_64 => null.
		q(nt(308), (nul));
		// _Rget_64 => __ option.
		q(nt(308), (nt(13)+nt(307)));
		// get => get_sym _Rget_64.
		q(nt(256), (nt(304)+nt(308)));
		// _Rset_65 => __.
		q(nt(309), (nt(13)));
		// _Rset_65 => _ '=' _.
		q(nt(309), (nt(15)+t(6)+nt(15)));
		// set => set_sym __ option _Rset_65 option_value.
		q(nt(263), (nt(305)+nt(13)+nt(307)+nt(309)+nt(310)));
		// toggle => toggle_sym __ bool_option.
		q(nt(262), (nt(306)+nt(13)+nt(311)));
		// get_sym => 'g' 'e' 't'.
		q(nt(304), (t(46)+t(32)+t(52)));
		// set_sym => 's' 'e' 't'.
		q(nt(305), (t(43)+t(32)+t(52)));
		// toggle_sym => 't' 'o' 'g' 'g' 'l' 'e'.
		q(nt(306), (t(52)+t(23)+t(46)+t(46)+t(31)+t(32)));
		// option => bool_option.
		q(nt(307), (nt(311)));
		// option => severity_opt.
		q(nt(307), (nt(312)));
		// bool_option => status_opt.
		q(nt(311), (nt(313)));
		// bool_option => colors_opt.
		q(nt(311), (nt(314)));
		// status_opt => 's'.
		q(nt(313), (t(43)));
		// status_opt => 's' 't' 'a' 't' 'u' 's'.
		q(nt(313), (t(43)+t(52)+t(30)+t(52)+t(47)+t(43)));
		// colors_opt => 'c'.
		q(nt(314), (t(44)));
		// colors_opt => 'c' 'o' 'l' 'o' 'r'.
		q(nt(314), (t(44)+t(23)+t(31)+t(23)+t(48)));
		// colors_opt => 'c' 'o' 'l' 'o' 'r' 's'.
		q(nt(314), (t(44)+t(23)+t(31)+t(23)+t(48)+t(43)));
		// severity_opt => 's' 'e' 'v'.
		q(nt(312), (t(43)+t(32)+t(51)));
		// severity_opt => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
		q(nt(312), (t(43)+t(32)+t(51)+t(32)+t(48)+t(21)+t(52)+t(58)));
		// option_value => option_value_true.
		q(nt(310), (nt(315)));
		// option_value => option_value_false.
		q(nt(310), (nt(316)));
		// option_value => severity.
		q(nt(310), (nt(317)));
		// option_value_true => '1'.
		q(nt(315), (t(40)));
		// option_value_true => 'o' 'n'.
		q(nt(315), (t(23)+t(45)));
		// option_value_true => 't'.
		q(nt(315), (t(52)));
		// option_value_true => 't' 'r' 'u' 'e'.
		q(nt(315), (t(52)+t(48)+t(47)+t(32)));
		// option_value_true => 'y'.
		q(nt(315), (t(58)));
		// option_value_true => 'y' 'e' 's'.
		q(nt(315), (t(58)+t(32)+t(43)));
		// option_value_false => '0'.
		q(nt(316), (t(41)));
		// option_value_false => 'f'.
		q(nt(316), (t(38)));
		// option_value_false => 'f' 'a' 'l' 's' 'e'.
		q(nt(316), (t(38)+t(30)+t(31)+t(43)+t(32)));
		// option_value_false => 'n'.
		q(nt(316), (t(45)));
		// option_value_false => 'n' 'o'.
		q(nt(316), (t(45)+t(23)));
		// option_value_false => 'o' 'f' 'f'.
		q(nt(316), (t(23)+t(38)+t(38)));
		// severity => error_sym.
		q(nt(317), (nt(318)));
		// severity => debug_sym.
		q(nt(317), (nt(319)));
		// severity => trace_sym.
		q(nt(317), (nt(320)));
		// severity => info_sym.
		q(nt(317), (nt(321)));
		// error_sym => 'e'.
		q(nt(318), (t(32)));
		// error_sym => 'e' 'r' 'r' 'o' 'r'.
		q(nt(318), (t(32)+t(48)+t(48)+t(23)+t(48)));
		// info_sym => 'i'.
		q(nt(321), (t(21)));
		// info_sym => 'i' 'n' 'f' 'o'.
		q(nt(321), (t(21)+t(45)+t(38)+t(23)));
		// debug_sym => 'd'.
		q(nt(319), (t(53)));
		// debug_sym => 'd' 'e' 'b' 'u' 'g'.
		q(nt(319), (t(53)+t(32)+t(34)+t(47)+t(46)));
		// trace_sym => 't'.
		q(nt(320), (t(52)));
		// trace_sym => 't' 'r' 'a' 'c' 'e'.
		q(nt(320), (t(52)+t(48)+t(30)+t(44)+t(32)));
		return q;
	}
};
#endif // __TAU_PARSER_H__
