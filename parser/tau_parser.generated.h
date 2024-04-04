// This file is generated from a file parser/tau.tgf by
//       https://github.com/IDNI/parser/tools/tgf
//
#ifndef __TAU_PARSER_H__
#define __TAU_PARSER_H__

#include <string.h>

#include "parser.h"

struct tau_parser {
	using char_type       = char;
	using terminal_type   = char;
	using traits_type     = std::char_traits<char_type>;
	using int_type        = typename traits_type::int_type;
	using grammar_type    = idni::grammar<char_type, terminal_type>;
	using grammar_options = grammar_type::options;
	using symbol_type     = idni::lit<char_type, terminal_type>;
	using location_type   = std::array<size_t, 2>;
	using node_type       = std::pair<symbol_type, location_type>;
	using parser_type     = idni::parser<char_type, terminal_type>;
	using options         = parser_type::options;
	using parse_options   = parser_type::parse_options;
	using forest_type     = parser_type::pforest;
	using input_type      = parser_type::input;
	using decoder_type    = parser_type::input::decoder_type;
	using encoder_type    = std::function<std::basic_string<char_type>(
			const std::vector<terminal_type>&)>;
	tau_parser() :
		nts(load_nonterminals()), cc(load_cc()),
		g(nts, load_prods(), nt(245), cc, load_grammar_opts()),
		p(g, load_opts()) {}
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
	bool found(size_t start = SIZE_MAX) { return p.found(start); }
	typename parser_type::error get_error() { return p.get_error(); }
	enum nonterminal {
		nul, alnum, alpha, digit, eof, printable, punct, space, xdigit, comment, 
		_Rcomment_0, _Rcomment_1, _Rcomment_2, __, _R___3, _, _R__4, dot, open_parenthesis, close_parenthesis, 
		open_bracket, close_bracket, open_brace, close_brace, minus, colon, semicolon, less, comma, apostrophe, 
		quote, tau_def, tau_and_sym, tau_or_sym, tau_neg_sym, wff_def, wff_and_sym, wff_or_sym, wff_xor_sym, wff_conditional_sym, 
		wff_neg_sym, wff_imply_sym, wff_equiv_sym, wff_all_sym, wff_ex_sym, wff_ball_sym, wff_bex_sym, wff_t, wff_f, bf_def, 
		bf_and_sym, bf_or_sym, bf_xor_sym, bf_neg_sym, bf_equality_sym, bf_nequality_sym, bf_less_sym, bf_less_equal_sym, bf_not_less_equal_sym, bf_greater_sym, 
		bf_all_sym, bf_ex_sym, bf_splitter_sym, bf_t, bf_f, chars, _Rchars_5, _Rchars_6, digits, _Rdigits_7, 
		sym, offsets, offset, _Roffsets_8, _Roffsets_9, num, capture, shift, variable, io_var, 
		_Roffset_10, _Rshift_11, _Rshift_12, charvar, in, out, in_var_name, out_var_name, bool_variable, _Rcharvar_13, 
		_Rcharvar_14, _Rcharvar_15, capture_var, var, tau_rule, tau_matcher, tau_body, tau, tau_collapse_positives_cb, tau_positives_upwards_cb, 
		tau_rec_relation, tau_and, tau_neg, tau_or, tau_wff, _Rtau_rec_relation_16, tau_ref, _Rtau_ref_17, tau_ref_args, tau_ref_arg, 
		_Rtau_ref_args_18, _Rtau_ref_args_19, bf, wff, wff_rule, wff_matcher, wff_body, bf_eq_cb, bf_neq_cb, wff_has_clashing_subformulas_cb, 
		wff_has_subformula_cb, wff_remove_existential_cb, wff_remove_bexistential_cb, wff_remove_buniversal_cb, wff_rec_relation, wff_and, wff_neg, wff_xor, wff_conditional, wff_or, 
		wff_all, wff_ex, wff_imply, wff_equiv, wff_ball, wff_bex, bf_eq, bf_neq, bf_less, bf_less_equal, 
		bf_greater, bf_interval, bf_not_less_equal, _Rwff_rec_relation_20, wff_ref, _Rwff_ref_21, wff_ref_args, wff_ref_arg, _Rwff_ref_args_22, _Rwff_ref_args_23, 
		_Rwff_all_24, _Rwff_ex_25, _Rwff_ball_26, _Rwff_bex_27, bf_rule, bf_matcher, bf_body, bf_is_zero_cb, bf_is_one_cb, bf_has_subformula_cb, 
		bf_remove_funiversal_cb, bf_remove_fexistential_cb, bf_and_cb, bf_or_cb, bf_xor_cb, bf_neg_cb, bf_splitter, bf_ref, bf_all, bf_ex, 
		bf_or, bf_and, bf_xor, bf_neg, bf_constant, bf_rec_relation, _Rbf_rec_relation_28, _Rbf_ref_29, bf_ref_args, bf_ref_arg, 
		_Rbf_ref_args_30, _Rbf_ref_args_31, _Rbf_all_32, _Rbf_ex_33, constant, binding, source_binding, named_binding, type, source, 
		source0, _Rsource_34, _Rsource_35, bf_and_cb_sym, bf_cb_arg, bf_or_cb_sym, bf_xor_cb_sym, bf_neg_cb_sym, bf_eq_cb_sym, wff_cb_arg, 
		bf_neq_cb_sym, bf_is_zero_cb_sym, bf_is_one_cb_sym, bf_remove_funiversal_cb_sym, bf_remove_fexistential_cb_sym, wff_remove_existential_cb_sym, wff_remove_bexistential_cb_sym, wff_remove_buniversal_cb_sym, wff_has_clashing_subformulas_cb_sym, bf_has_subformula_cb_sym, 
		wff_has_subformula_cb_sym, tau_collapse_positives_cb_sym, tau_cb_arg, _Rtau_collapse_positives_cb_36, _Rtau_collapse_positives_cb_37, tau_positives_upwards_cb_sym, inputs, input, _Rinputs_38, _Rinputs_39, 
		builder, builder_head, builder_body, _Rbuilder_head_40, _Rbuilder_head_41, bf_builder_body, wff_builder_body, tau_builder_body, library, rules, 
		rule, _Rrules_42, _Rrules_43, nso_rr, nso_rec_relations, nso_main, nso_rec_relation, _Rnso_rec_relations_44, _Rnso_rec_relations_45, gssotc_rr, 
		gssotc_rec_relations, gssotc_main, gssotc_rec_relation, _Rgssotc_rec_relations_46, _Rgssotc_rec_relations_47, start, _Rstart_48, cli, cli_command, _Rcli_49, 
		_Rcli_50, _Rcli_51, help, version, quit, get, set, toggle, list_outputs, clear_outputs, 
		print_output, file, normalize, wff_onf_cmd, wff_dnf_cmd, wff_cnf_cmd, wff_anf_cmd, wff_nnf_cmd, wff_pnf_cmd, wff_mnf_cmd, 
		bf_dnf_cmd, bf_cnf_cmd, bf_anf_cmd, bf_nnf_cmd, bf_pnf_cmd, bf_mnf_cmd, bf_substitute, wff_substitute, bf_instantiate, wff_instantiate, 
		rr_nso, normalize_sym, form_arg, onf, onf_sym, var_arg, dnf, dnf_sym, cnf, cnf_sym, 
		anf, anf_sym, nnf, nnf_sym, pnf, pnf_sym, minterm, minterm_sym, file_sym, file_path, 
		normalize_cmd, normalize_cmd_sym, output, wff_selection, _Rnormalize_cmd_52, bf_instantiate_cmd, instantiate_cmd_sym, bf_selection, _Rbf_instantiate_cmd_53, _Rbf_instantiate_cmd_54, 
		wff_instantiate_cmd, _Rwff_instantiate_cmd_55, _Rwff_instantiate_cmd_56, bf_substitute_cmd, substitute_cmd_sym, _Rbf_substitute_cmd_57, _Rbf_substitute_cmd_58, _Rbf_substitute_cmd_59, wff_substitute_cmd, substitutecmd__sym, 
		_Rwff_substitute_cmd_60, _Rwff_substitute_cmd_61, _Rwff_substitute_cmd_62, dnf_cmd_sym, _Rbf_dnf_cmd_63, cnf_cmd_sym, _Rbf_cnf_cmd_64, anf_cmd_sym, _Rbf_anf_cmd_65, nnf_cmd_sym, 
		_Rbf_nnf_cmd_66, pnf_cmd_sym, _Rbf_pnf_cmd_67, mnf_cmd_sym, _Rbf_mnf_cmd_68, onf_cmd_sym, _Rwff_onf_cmd_69, _Rwff_dnf_cmd_70, _Rwff_cnf_cmd_71, _Rwff_anf_cmd_72, 
		_Rwff_nnf_cmd_73, _Rwff_pnf_cmd_74, _Rwff_mnf_cmd_75, help_cmd, help_cmd_sym, cli_cmd_sym, _Rhelp_cmd_76, version_cmd, version_cmd_sym, quit_cmd, 
		quit_cmd_sym, selection_sym, bf_var_selection, wff_var_selection, absolute_output, relative_output, absolute_output_sym, output_sym, _Rabsolute_output_77, output_id, 
		relative_output_sym, list_outputs_cmd, clear_outputs_cmd, _Rclear_outputs_cmd_78, clear_sym, print_output_cmd, file_cmd_sym, get_cmd_sym, set_cmd_sym, toggle_cmd_sym, 
		instantiate_sym, substitute_sym, clear_cmd_sym, get_cmd, option, _Rget_cmd_79, set_cmd, _Rset_cmd_80, option_value, toggle_cmd, 
		bool_option, severity_opt, status_opt, colors_opt, option_value_true, option_value_false, severity, error_sym, debug_sym, trace_sym, 
		info_sym, __neg_0, __neg_1, __neg_2, __neg_3, __neg_4, __neg_5, __neg_6, __neg_7, __neg_8, 
		__neg_9, __neg_10, __neg_11, __neg_12, __neg_13, __neg_14, __neg_15, __neg_16, __neg_17, __neg_18, 
		__neg_19, __neg_20, __neg_21, __neg_22, __neg_23, __neg_24, __neg_25, __neg_26, __neg_27, __neg_28, 
		__neg_29, __neg_30, __neg_31, __neg_32, __neg_33, __neg_34, __neg_35, __neg_36, __neg_37, __neg_38, 
		__neg_39, __neg_40, __neg_41, __neg_42, __neg_43, __neg_44, 
	};
	size_t id(const std::basic_string<char_type>& name) {
		return nts.get(name);
	}
	const std::basic_string<char_type>& name(size_t id) {
		return nts.get(id);
	}
private:
	std::vector<terminal_type> ts{
		'\0', '\t', '\n', '\r', '#', '.', '(', ')', '[', 
		']', '{', '}', '-', ':', ';', '<', ',', '\'', '"', 
		'=', '&', '|', '!', '^', '?', '>', 'a', 'l', 'e', 
		'x', 'b', 'T', 'F', '+', 'f', 'S', '1', '0', '$', 
		'i', '_', 'o', 'h', 's', 'u', 'r', 'm', 'c', 'w', 
		'n', 'g', 'v', 't', 'd', 'q', 'z', 'p', '%', 'y', 
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
			"", "alnum", "alpha", "digit", "eof", "printable", "punct", "space", "xdigit", "comment", 
			"_Rcomment_0", "_Rcomment_1", "_Rcomment_2", "__", "_R___3", "_", "_R__4", "dot", "open_parenthesis", "close_parenthesis", 
			"open_bracket", "close_bracket", "open_brace", "close_brace", "minus", "colon", "semicolon", "less", "comma", "apostrophe", 
			"quote", "tau_def", "tau_and_sym", "tau_or_sym", "tau_neg_sym", "wff_def", "wff_and_sym", "wff_or_sym", "wff_xor_sym", "wff_conditional_sym", 
			"wff_neg_sym", "wff_imply_sym", "wff_equiv_sym", "wff_all_sym", "wff_ex_sym", "wff_ball_sym", "wff_bex_sym", "wff_t", "wff_f", "bf_def", 
			"bf_and_sym", "bf_or_sym", "bf_xor_sym", "bf_neg_sym", "bf_equality_sym", "bf_nequality_sym", "bf_less_sym", "bf_less_equal_sym", "bf_not_less_equal_sym", "bf_greater_sym", 
			"bf_all_sym", "bf_ex_sym", "bf_splitter_sym", "bf_t", "bf_f", "chars", "_Rchars_5", "_Rchars_6", "digits", "_Rdigits_7", 
			"sym", "offsets", "offset", "_Roffsets_8", "_Roffsets_9", "num", "capture", "shift", "variable", "io_var", 
			"_Roffset_10", "_Rshift_11", "_Rshift_12", "charvar", "in", "out", "in_var_name", "out_var_name", "bool_variable", "_Rcharvar_13", 
			"_Rcharvar_14", "_Rcharvar_15", "capture_var", "var", "tau_rule", "tau_matcher", "tau_body", "tau", "tau_collapse_positives_cb", "tau_positives_upwards_cb", 
			"tau_rec_relation", "tau_and", "tau_neg", "tau_or", "tau_wff", "_Rtau_rec_relation_16", "tau_ref", "_Rtau_ref_17", "tau_ref_args", "tau_ref_arg", 
			"_Rtau_ref_args_18", "_Rtau_ref_args_19", "bf", "wff", "wff_rule", "wff_matcher", "wff_body", "bf_eq_cb", "bf_neq_cb", "wff_has_clashing_subformulas_cb", 
			"wff_has_subformula_cb", "wff_remove_existential_cb", "wff_remove_bexistential_cb", "wff_remove_buniversal_cb", "wff_rec_relation", "wff_and", "wff_neg", "wff_xor", "wff_conditional", "wff_or", 
			"wff_all", "wff_ex", "wff_imply", "wff_equiv", "wff_ball", "wff_bex", "bf_eq", "bf_neq", "bf_less", "bf_less_equal", 
			"bf_greater", "bf_interval", "bf_not_less_equal", "_Rwff_rec_relation_20", "wff_ref", "_Rwff_ref_21", "wff_ref_args", "wff_ref_arg", "_Rwff_ref_args_22", "_Rwff_ref_args_23", 
			"_Rwff_all_24", "_Rwff_ex_25", "_Rwff_ball_26", "_Rwff_bex_27", "bf_rule", "bf_matcher", "bf_body", "bf_is_zero_cb", "bf_is_one_cb", "bf_has_subformula_cb", 
			"bf_remove_funiversal_cb", "bf_remove_fexistential_cb", "bf_and_cb", "bf_or_cb", "bf_xor_cb", "bf_neg_cb", "bf_splitter", "bf_ref", "bf_all", "bf_ex", 
			"bf_or", "bf_and", "bf_xor", "bf_neg", "bf_constant", "bf_rec_relation", "_Rbf_rec_relation_28", "_Rbf_ref_29", "bf_ref_args", "bf_ref_arg", 
			"_Rbf_ref_args_30", "_Rbf_ref_args_31", "_Rbf_all_32", "_Rbf_ex_33", "constant", "binding", "source_binding", "named_binding", "type", "source", 
			"source0", "_Rsource_34", "_Rsource_35", "bf_and_cb_sym", "bf_cb_arg", "bf_or_cb_sym", "bf_xor_cb_sym", "bf_neg_cb_sym", "bf_eq_cb_sym", "wff_cb_arg", 
			"bf_neq_cb_sym", "bf_is_zero_cb_sym", "bf_is_one_cb_sym", "bf_remove_funiversal_cb_sym", "bf_remove_fexistential_cb_sym", "wff_remove_existential_cb_sym", "wff_remove_bexistential_cb_sym", "wff_remove_buniversal_cb_sym", "wff_has_clashing_subformulas_cb_sym", "bf_has_subformula_cb_sym", 
			"wff_has_subformula_cb_sym", "tau_collapse_positives_cb_sym", "tau_cb_arg", "_Rtau_collapse_positives_cb_36", "_Rtau_collapse_positives_cb_37", "tau_positives_upwards_cb_sym", "inputs", "input", "_Rinputs_38", "_Rinputs_39", 
			"builder", "builder_head", "builder_body", "_Rbuilder_head_40", "_Rbuilder_head_41", "bf_builder_body", "wff_builder_body", "tau_builder_body", "library", "rules", 
			"rule", "_Rrules_42", "_Rrules_43", "nso_rr", "nso_rec_relations", "nso_main", "nso_rec_relation", "_Rnso_rec_relations_44", "_Rnso_rec_relations_45", "gssotc_rr", 
			"gssotc_rec_relations", "gssotc_main", "gssotc_rec_relation", "_Rgssotc_rec_relations_46", "_Rgssotc_rec_relations_47", "start", "_Rstart_48", "cli", "cli_command", "_Rcli_49", 
			"_Rcli_50", "_Rcli_51", "help", "version", "quit", "get", "set", "toggle", "list_outputs", "clear_outputs", 
			"print_output", "file", "normalize", "wff_onf_cmd", "wff_dnf_cmd", "wff_cnf_cmd", "wff_anf_cmd", "wff_nnf_cmd", "wff_pnf_cmd", "wff_mnf_cmd", 
			"bf_dnf_cmd", "bf_cnf_cmd", "bf_anf_cmd", "bf_nnf_cmd", "bf_pnf_cmd", "bf_mnf_cmd", "bf_substitute", "wff_substitute", "bf_instantiate", "wff_instantiate", 
			"rr_nso", "normalize_sym", "form_arg", "onf", "onf_sym", "var_arg", "dnf", "dnf_sym", "cnf", "cnf_sym", 
			"anf", "anf_sym", "nnf", "nnf_sym", "pnf", "pnf_sym", "minterm", "minterm_sym", "file_sym", "file_path", 
			"normalize_cmd", "normalize_cmd_sym", "output", "wff_selection", "_Rnormalize_cmd_52", "bf_instantiate_cmd", "instantiate_cmd_sym", "bf_selection", "_Rbf_instantiate_cmd_53", "_Rbf_instantiate_cmd_54", 
			"wff_instantiate_cmd", "_Rwff_instantiate_cmd_55", "_Rwff_instantiate_cmd_56", "bf_substitute_cmd", "substitute_cmd_sym", "_Rbf_substitute_cmd_57", "_Rbf_substitute_cmd_58", "_Rbf_substitute_cmd_59", "wff_substitute_cmd", "substitutecmd__sym", 
			"_Rwff_substitute_cmd_60", "_Rwff_substitute_cmd_61", "_Rwff_substitute_cmd_62", "dnf_cmd_sym", "_Rbf_dnf_cmd_63", "cnf_cmd_sym", "_Rbf_cnf_cmd_64", "anf_cmd_sym", "_Rbf_anf_cmd_65", "nnf_cmd_sym", 
			"_Rbf_nnf_cmd_66", "pnf_cmd_sym", "_Rbf_pnf_cmd_67", "mnf_cmd_sym", "_Rbf_mnf_cmd_68", "onf_cmd_sym", "_Rwff_onf_cmd_69", "_Rwff_dnf_cmd_70", "_Rwff_cnf_cmd_71", "_Rwff_anf_cmd_72", 
			"_Rwff_nnf_cmd_73", "_Rwff_pnf_cmd_74", "_Rwff_mnf_cmd_75", "help_cmd", "help_cmd_sym", "cli_cmd_sym", "_Rhelp_cmd_76", "version_cmd", "version_cmd_sym", "quit_cmd", 
			"quit_cmd_sym", "selection_sym", "bf_var_selection", "wff_var_selection", "absolute_output", "relative_output", "absolute_output_sym", "output_sym", "_Rabsolute_output_77", "output_id", 
			"relative_output_sym", "list_outputs_cmd", "clear_outputs_cmd", "_Rclear_outputs_cmd_78", "clear_sym", "print_output_cmd", "file_cmd_sym", "get_cmd_sym", "set_cmd_sym", "toggle_cmd_sym", 
			"instantiate_sym", "substitute_sym", "clear_cmd_sym", "get_cmd", "option", "_Rget_cmd_79", "set_cmd", "_Rset_cmd_80", "option_value", "toggle_cmd", 
			"bool_option", "severity_opt", "status_opt", "colors_opt", "option_value_true", "option_value_false", "severity", "error_sym", "debug_sym", "trace_sym", 
			"info_sym", "__neg_0", "__neg_1", "__neg_2", "__neg_3", "__neg_4", "__neg_5", "__neg_6", "__neg_7", "__neg_8", 
			"__neg_9", "__neg_10", "__neg_11", "__neg_12", "__neg_13", "__neg_14", "__neg_15", "__neg_16", "__neg_17", "__neg_18", 
			"__neg_19", "__neg_20", "__neg_21", "__neg_22", "__neg_23", "__neg_24", "__neg_25", "__neg_26", "__neg_27", "__neg_28", 
			"__neg_29", "__neg_30", "__neg_31", "__neg_32", "__neg_33", "__neg_34", "__neg_35", "__neg_36", "__neg_37", "__neg_38", 
			"__neg_39", "__neg_40", "__neg_41", "__neg_42", "__neg_43", "__neg_44", 
		}) nts.get(nt);
		return nts;
	}
	idni::char_class_fns<terminal_type> load_cc() {
		return idni::predefined_char_classes<char_type, terminal_type>({
			"alnum",
			"alpha",
			"digit",
			"eof",
			"printable",
			"punct",
			"space",
			"xdigit",
		}, nts);
	}
	grammar_type::options load_grammar_opts() {
		grammar_type::options o;
		o.auto_disambiguate = true;
		return o;
	}
	options load_opts() {
		options o;
		return o;
	}
	idni::prods<char_type, terminal_type> load_prods() {
		idni::prods<char_type, terminal_type> q,
			nul(symbol_type{});
		//       _Rcomment_0(10)      => '\t'.
		q(nt(10), (t(1)));
		//       _Rcomment_0(10)      => printable(5).
		q(nt(10), (nt(5)));
		//       _Rcomment_1(11)      => null.
		q(nt(11), (nul));
		//       _Rcomment_1(11)      => _Rcomment_0(10) _Rcomment_1(11).
		q(nt(11), (nt(10)+nt(11)));
		//       _Rcomment_2(12)      => '\n'.
		q(nt(12), (t(2)));
		//       _Rcomment_2(12)      => '\r'.
		q(nt(12), (t(3)));
		//       _Rcomment_2(12)      => eof(4).
		q(nt(12), (nt(4)));
		//       comment(9)           => '#' _Rcomment_1(11) _Rcomment_2(12).
		q(nt(9), (t(4)+nt(11)+nt(12)));
		//       _R___3(14)           => space(7).
		q(nt(14), (nt(7)));
		//       _R___3(14)           => comment(9).
		q(nt(14), (nt(9)));
		//       __(13)               => _R___3(14) _(15).
		q(nt(13), (nt(14)+nt(15)));
		//       _R__4(16)            => null.
		q(nt(16), (nul));
		//       _R__4(16)            => __(13).
		q(nt(16), (nt(13)));
		//       _(15)                => _R__4(16).
		q(nt(15), (nt(16)));
		//       dot(17)              => '.'.
		q(nt(17), (t(5)));
		//       open_parenthesis(18) => '('.
		q(nt(18), (t(6)));
		//       close_parenthesis(19) => ')'.
		q(nt(19), (t(7)));
		//       open_bracket(20)     => '['.
		q(nt(20), (t(8)));
		//       close_bracket(21)    => ']'.
		q(nt(21), (t(9)));
		//       open_brace(22)       => '{'.
		q(nt(22), (t(10)));
		//       close_brace(23)      => '}'.
		q(nt(23), (t(11)));
		//       minus(24)            => '-'.
		q(nt(24), (t(12)));
		//       colon(25)            => ':'.
		q(nt(25), (t(13)));
		//       semicolon(26)        => ';'.
		q(nt(26), (t(14)));
		//       less(27)             => '<'.
		q(nt(27), (t(15)));
		//       comma(28)            => ','.
		q(nt(28), (t(16)));
		//       apostrophe(29)       => '\''.
		q(nt(29), (t(17)));
		//       quote(30)            => '"'.
		q(nt(30), (t(18)));
		//       tau_def(31)          => ':' ':' ':' '='.
		q(nt(31), (t(13)+t(13)+t(13)+t(19)));
		//       tau_and_sym(32)      => '&' '&' '&'.
		q(nt(32), (t(20)+t(20)+t(20)));
		//       tau_or_sym(33)       => '|' '|' '|'.
		q(nt(33), (t(21)+t(21)+t(21)));
		//       tau_neg_sym(34)      => '!' '!' '!'.
		q(nt(34), (t(22)+t(22)+t(22)));
		//       wff_def(35)          => ':' ':' '='.
		q(nt(35), (t(13)+t(13)+t(19)));
		//       wff_and_sym(36)      => '&' '&'.
		q(nt(36), (t(20)+t(20)));
		//       wff_or_sym(37)       => '|' '|'.
		q(nt(37), (t(21)+t(21)));
		//       wff_xor_sym(38)      => '^'.
		q(nt(38), (t(23)));
		//       wff_conditional_sym(39) => '?'.
		q(nt(39), (t(24)));
		//       wff_neg_sym(40)      => '!'.
		q(nt(40), (t(22)));
		//       wff_imply_sym(41)    => '-' '>'.
		q(nt(41), (t(12)+t(25)));
		//       wff_equiv_sym(42)    => '<' '-' '>'.
		q(nt(42), (t(15)+t(12)+t(25)));
		//       wff_all_sym(43)      => 'a' 'l' 'l'.
		q(nt(43), (t(26)+t(27)+t(27)));
		//       wff_ex_sym(44)       => 'e' 'x'.
		q(nt(44), (t(28)+t(29)));
		//       wff_ball_sym(45)     => 'b' 'a' 'l' 'l'.
		q(nt(45), (t(30)+t(26)+t(27)+t(27)));
		//       wff_bex_sym(46)      => 'b' 'e' 'x'.
		q(nt(46), (t(30)+t(28)+t(29)));
		//       wff_t(47)            => 'T'.
		q(nt(47), (t(31)));
		//       wff_f(48)            => 'F'.
		q(nt(48), (t(32)));
		//       bf_def(49)           => ':' '='.
		q(nt(49), (t(13)+t(19)));
		//       bf_and_sym(50)       => '&'.
		q(nt(50), (t(20)));
		//       bf_or_sym(51)        => '|'.
		q(nt(51), (t(21)));
		//       bf_xor_sym(52)       => '+'.
		q(nt(52), (t(33)));
		//       bf_neg_sym(53)       => apostrophe(29).
		q(nt(53), (nt(29)));
		//       bf_equality_sym(54)  => '='.
		q(nt(54), (t(19)));
		//       bf_nequality_sym(55) => '!' '='.
		q(nt(55), (t(22)+t(19)));
		//       bf_less_sym(56)      => '<'.
		q(nt(56), (t(15)));
		//       bf_less_equal_sym(57) => '<' '='.
		q(nt(57), (t(15)+t(19)));
		//       bf_not_less_equal_sym(58) => '!' '<' '='.
		q(nt(58), (t(22)+t(15)+t(19)));
		//       bf_greater_sym(59)   => '>'.
		q(nt(59), (t(25)));
		//       bf_all_sym(60)       => 'f' 'a' 'l' 'l'.
		q(nt(60), (t(34)+t(26)+t(27)+t(27)));
		//       bf_ex_sym(61)        => 'f' 'e' 'x'.
		q(nt(61), (t(34)+t(28)+t(29)));
		//       bf_splitter_sym(62)  => 'S'.
		q(nt(62), (t(35)));
		//       bf_t(63)             => '1'.
		q(nt(63), (t(36)));
		//       bf_f(64)             => '0'.
		q(nt(64), (t(37)));
		//       _Rchars_5(66)        => alnum(1).
		q(nt(66), (nt(1)));
		//       _Rchars_6(67)        => null.
		q(nt(67), (nul));
		//       _Rchars_6(67)        => _Rchars_5(66) _Rchars_6(67).
		q(nt(67), (nt(66)+nt(67)));
		//       chars(65)            => alpha(2) _Rchars_6(67).
		q(nt(65), (nt(2)+nt(67)));
		//       _Rdigits_7(69)       => digit(3).
		q(nt(69), (nt(3)));
		//       _Rdigits_7(69)       => digit(3) _Rdigits_7(69).
		q(nt(69), (nt(3)+nt(69)));
		//       digits(68)           => _Rdigits_7(69).
		q(nt(68), (nt(69)));
		//       sym(70)              => chars(65).
		q(nt(70), (nt(65)));
		//       _Roffsets_8(73)      => _(15) comma(28) _(15) offset(72).
		q(nt(73), (nt(15)+nt(28)+nt(15)+nt(72)));
		//       _Roffsets_9(74)      => null.
		q(nt(74), (nul));
		//       _Roffsets_9(74)      => _Roffsets_8(73) _Roffsets_9(74).
		q(nt(74), (nt(73)+nt(74)));
		//       offsets(71)          => open_bracket(20) _(15) offset(72) _Roffsets_9(74) _(15) close_bracket(21).
		q(nt(71), (nt(20)+nt(15)+nt(72)+nt(74)+nt(15)+nt(21)));
		//       __neg_0(391)         => io_var(79).
		q(nt(391), (nt(79)));
		//       _Roffset_10(80)      => variable(78) & ~( __neg_0(391) ).	 # conjunctive
		q(nt(80), (nt(78)) & ~(nt(391)));
		//       offset(72)           => num(75).
		q(nt(72), (nt(75)));
		//       offset(72)           => capture(76).
		q(nt(72), (nt(76)));
		//       offset(72)           => shift(77).
		q(nt(72), (nt(77)));
		//       offset(72)           => _Roffset_10(80).
		q(nt(72), (nt(80)));
		//       __neg_1(392)         => io_var(79).
		q(nt(392), (nt(79)));
		//       _Rshift_11(81)       => variable(78) & ~( __neg_1(392) ).	 # conjunctive
		q(nt(81), (nt(78)) & ~(nt(392)));
		//       _Rshift_12(82)       => capture(76).
		q(nt(82), (nt(76)));
		//       _Rshift_12(82)       => _Rshift_11(81).
		q(nt(82), (nt(81)));
		//       shift(77)            => _Rshift_12(82) _(15) minus(24) _(15) num(75).
		q(nt(77), (nt(82)+nt(15)+nt(24)+nt(15)+nt(75)));
		//       num(75)              => digits(68).
		q(nt(75), (nt(68)));
		//       variable(78)         => io_var(79).
		q(nt(78), (nt(79)));
		//       variable(78)         => charvar(83).
		q(nt(78), (nt(83)));
		//       io_var(79)           => in(84).
		q(nt(79), (nt(84)));
		//       io_var(79)           => out(85).
		q(nt(79), (nt(85)));
		//       in(84)               => in_var_name(86) open_bracket(20) offset(72) close_bracket(21).
		q(nt(84), (nt(86)+nt(20)+nt(72)+nt(21)));
		//       out(85)              => out_var_name(87) open_bracket(20) offset(72) close_bracket(21).
		q(nt(85), (nt(87)+nt(20)+nt(72)+nt(21)));
		//       bool_variable(88)    => charvar(83).
		q(nt(88), (nt(83)));
		//       __neg_2(393)         => 'F'.
		q(nt(393), (t(32)));
		//       __neg_3(394)         => 'T'.
		q(nt(394), (t(31)));
		//       _Rcharvar_13(89)     => ~( __neg_2(393) ) & ~( __neg_3(394) ) & alpha(2).	 # conjunctive
		q(nt(89), ~(nt(393)) & ~(nt(394)) & (nt(2)));
		//       _Rcharvar_14(90)     => digit(3).
		q(nt(90), (nt(3)));
		//       _Rcharvar_15(91)     => null.
		q(nt(91), (nul));
		//       _Rcharvar_15(91)     => _Rcharvar_14(90) _Rcharvar_15(91).
		q(nt(91), (nt(90)+nt(91)));
		//       charvar(83)          => _Rcharvar_13(89) _Rcharvar_15(91).
		q(nt(83), (nt(89)+nt(91)));
		//       capture(76)          => capture_var(92).
		q(nt(76), (nt(92)));
		//       capture_var(92)      => '$' chars(65).
		q(nt(92), (t(38)+nt(65)));
		//       var(93)              => chars(65).
		q(nt(93), (nt(65)));
		//       in_var_name(86)      => 'i' '_' chars(65).
		q(nt(86), (t(39)+t(40)+nt(65)));
		//       out_var_name(87)     => 'o' '_' chars(65).
		q(nt(87), (t(41)+t(40)+nt(65)));
		//       tau_rule(94)         => tau_matcher(95) _(15) tau_def(31) _(15) tau_body(96) _(15) dot(17).
		q(nt(94), (nt(95)+nt(15)+nt(31)+nt(15)+nt(96)+nt(15)+nt(17)));
		//       tau_matcher(95)      => tau(97).
		q(nt(95), (nt(97)));
		//       tau_body(96)         => tau(97).
		q(nt(96), (nt(97)));
		//       tau_body(96)         => tau_collapse_positives_cb(98).
		q(nt(96), (nt(98)));
		//       tau_body(96)         => tau_positives_upwards_cb(99).
		q(nt(96), (nt(99)));
		//       __neg_4(395)         => capture(76).
		q(nt(395), (nt(76)));
		//       __neg_5(396)         => tau_and(101).
		q(nt(396), (nt(101)));
		//       __neg_6(397)         => tau_neg(102).
		q(nt(397), (nt(102)));
		//       __neg_7(398)         => tau_or(103).
		q(nt(398), (nt(103)));
		//       __neg_8(399)         => tau_wff(104).
		q(nt(399), (nt(104)));
		//       _Rtau_rec_relation_16(105) => ~( __neg_4(395) ) & tau(97) & ~( __neg_5(396) ) & ~( __neg_6(397) ) & ~( __neg_7(398) ) & ~( __neg_8(399) ).	 # conjunctive
		q(nt(105), ~(nt(395)) & (nt(97)) & ~(nt(396)) & ~(nt(397)) & ~(nt(398)) & ~(nt(399)));
		//       tau_rec_relation(100) => _Rtau_rec_relation_16(105) _(15) tau_def(31) _(15) tau(97) _(15) dot(17).
		q(nt(100), (nt(105)+nt(15)+nt(31)+nt(15)+nt(97)+nt(15)+nt(17)));
		//       _Rtau_ref_17(107)    => null.
		q(nt(107), (nul));
		//       _Rtau_ref_17(107)    => offsets(71).
		q(nt(107), (nt(71)));
		//       tau_ref(106)         => sym(70) _(15) _Rtau_ref_17(107) _(15) tau_ref_args(108).
		q(nt(106), (nt(70)+nt(15)+nt(107)+nt(15)+nt(108)));
		//       _Rtau_ref_args_18(110) => _(15) tau_ref_arg(109).
		q(nt(110), (nt(15)+nt(109)));
		//       _Rtau_ref_args_19(111) => null.
		q(nt(111), (nul));
		//       _Rtau_ref_args_19(111) => _Rtau_ref_args_18(110) _Rtau_ref_args_19(111).
		q(nt(111), (nt(110)+nt(111)));
		//       tau_ref_args(108)    => open_parenthesis(18) _Rtau_ref_args_19(111) _(15) close_parenthesis(19).
		q(nt(108), (nt(18)+nt(111)+nt(15)+nt(19)));
		//       tau_ref_arg(109)     => bf(112).
		q(nt(109), (nt(112)));
		//       tau_ref_arg(109)     => capture(76).
		q(nt(109), (nt(76)));
		//       tau_ref_arg(109)     => variable(78).
		q(nt(109), (nt(78)));
		//       tau(97)              => tau_ref(106).
		q(nt(97), (nt(106)));
		//       tau(97)              => tau_or(103).
		q(nt(97), (nt(103)));
		//       tau(97)              => tau_and(101).
		q(nt(97), (nt(101)));
		//       tau(97)              => tau_neg(102).
		q(nt(97), (nt(102)));
		//       tau(97)              => tau_wff(104).
		q(nt(97), (nt(104)));
		//       tau(97)              => capture(76).
		q(nt(97), (nt(76)));
		//       tau(97)              => open_parenthesis(18) _(15) tau(97) _(15) close_parenthesis(19).
		q(nt(97), (nt(18)+nt(15)+nt(97)+nt(15)+nt(19)));
		//       tau_and(101)         => tau(97) _(15) tau_and_sym(32) _(15) tau(97).
		q(nt(101), (nt(97)+nt(15)+nt(32)+nt(15)+nt(97)));
		//       tau_or(103)          => tau(97) _(15) tau_or_sym(33) _(15) tau(97).
		q(nt(103), (nt(97)+nt(15)+nt(33)+nt(15)+nt(97)));
		//       tau_neg(102)         => tau_neg_sym(34) _(15) tau(97).
		q(nt(102), (nt(34)+nt(15)+nt(97)));
		//       tau_wff(104)         => open_brace(22) _(15) wff(113) _(15) close_brace(23).
		q(nt(104), (nt(22)+nt(15)+nt(113)+nt(15)+nt(23)));
		//       wff_rule(114)        => wff_matcher(115) _(15) wff_def(35) _(15) wff_body(116) _(15) dot(17).
		q(nt(114), (nt(115)+nt(15)+nt(35)+nt(15)+nt(116)+nt(15)+nt(17)));
		//       wff_matcher(115)     => wff(113).
		q(nt(115), (nt(113)));
		//       wff_body(116)        => bf_eq_cb(117).
		q(nt(116), (nt(117)));
		//       wff_body(116)        => bf_neq_cb(118).
		q(nt(116), (nt(118)));
		//       wff_body(116)        => wff_has_clashing_subformulas_cb(119).
		q(nt(116), (nt(119)));
		//       wff_body(116)        => wff_has_subformula_cb(120).
		q(nt(116), (nt(120)));
		//       wff_body(116)        => wff_remove_existential_cb(121).
		q(nt(116), (nt(121)));
		//       wff_body(116)        => wff_remove_bexistential_cb(122).
		q(nt(116), (nt(122)));
		//       wff_body(116)        => wff_remove_buniversal_cb(123).
		q(nt(116), (nt(123)));
		//       wff_body(116)        => wff(113).
		q(nt(116), (nt(113)));
		//       __neg_9(400)         => wff_t(47).
		q(nt(400), (nt(47)));
		//       __neg_10(401)        => wff_f(48).
		q(nt(401), (nt(48)));
		//       __neg_11(402)        => capture(76).
		q(nt(402), (nt(76)));
		//       __neg_12(403)        => bool_variable(88).
		q(nt(403), (nt(88)));
		//       __neg_13(404)        => wff_and(125).
		q(nt(404), (nt(125)));
		//       __neg_14(405)        => wff_neg(126).
		q(nt(405), (nt(126)));
		//       __neg_15(406)        => wff_xor(127).
		q(nt(406), (nt(127)));
		//       __neg_16(407)        => wff_conditional(128).
		q(nt(407), (nt(128)));
		//       __neg_17(408)        => wff_or(129).
		q(nt(408), (nt(129)));
		//       __neg_18(409)        => wff_all(130).
		q(nt(409), (nt(130)));
		//       __neg_19(410)        => wff_ex(131).
		q(nt(410), (nt(131)));
		//       __neg_20(411)        => wff_imply(132).
		q(nt(411), (nt(132)));
		//       __neg_21(412)        => wff_equiv(133).
		q(nt(412), (nt(133)));
		//       __neg_22(413)        => wff_ball(134).
		q(nt(413), (nt(134)));
		//       __neg_23(414)        => wff_bex(135).
		q(nt(414), (nt(135)));
		//       __neg_24(415)        => bf_eq(136).
		q(nt(415), (nt(136)));
		//       __neg_25(416)        => bf_neq(137).
		q(nt(416), (nt(137)));
		//       __neg_26(417)        => bf_less(138).
		q(nt(417), (nt(138)));
		//       __neg_27(418)        => bf_less_equal(139).
		q(nt(418), (nt(139)));
		//       __neg_28(419)        => bf_greater(140).
		q(nt(419), (nt(140)));
		//       __neg_29(420)        => bf_interval(141).
		q(nt(420), (nt(141)));
		//       __neg_30(421)        => bf_not_less_equal(142).
		q(nt(421), (nt(142)));
		//       _Rwff_rec_relation_20(143) => ~( __neg_9(400) ) & ~( __neg_10(401) ) & ~( __neg_11(402) ) & ~( __neg_12(403) ) & wff(113) & ~( __neg_13(404) ) & ~( __neg_14(405) ) & ~( __neg_15(406) ) & ~( __neg_16(407) ) & ~( __neg_17(408) ) & ~( __neg_18(409) ) & ~( __neg_19(410) ) & ~( __neg_20(411) ) & ~( __neg_21(412) ) & ~( __neg_22(413) ) & ~( __neg_23(414) ) & ~( __neg_24(415) ) & ~( __neg_25(416) ) & ~( __neg_26(417) ) & ~( __neg_27(418) ) & ~( __neg_28(419) ) & ~( __neg_29(420) ) & ~( __neg_30(421) ).	 # conjunctive
		q(nt(143), ~(nt(400)) & ~(nt(401)) & ~(nt(402)) & ~(nt(403)) & (nt(113)) & ~(nt(404)) & ~(nt(405)) & ~(nt(406)) & ~(nt(407)) & ~(nt(408)) & ~(nt(409)) & ~(nt(410)) & ~(nt(411)) & ~(nt(412)) & ~(nt(413)) & ~(nt(414)) & ~(nt(415)) & ~(nt(416)) & ~(nt(417)) & ~(nt(418)) & ~(nt(419)) & ~(nt(420)) & ~(nt(421)));
		//       wff_rec_relation(124) => _Rwff_rec_relation_20(143) _(15) wff_def(35) _(15) wff(113) _(15) dot(17).
		q(nt(124), (nt(143)+nt(15)+nt(35)+nt(15)+nt(113)+nt(15)+nt(17)));
		//       wff(113)             => wff_ref(144).
		q(nt(113), (nt(144)));
		//       wff(113)             => wff_conditional(128).
		q(nt(113), (nt(128)));
		//       wff(113)             => wff_ball(134).
		q(nt(113), (nt(134)));
		//       wff(113)             => wff_bex(135).
		q(nt(113), (nt(135)));
		//       wff(113)             => wff_all(130).
		q(nt(113), (nt(130)));
		//       wff(113)             => wff_ex(131).
		q(nt(113), (nt(131)));
		//       wff(113)             => wff_imply(132).
		q(nt(113), (nt(132)));
		//       wff(113)             => wff_equiv(133).
		q(nt(113), (nt(133)));
		//       wff(113)             => bf_interval(141).
		q(nt(113), (nt(141)));
		//       wff(113)             => bf_eq(136).
		q(nt(113), (nt(136)));
		//       wff(113)             => bf_neq(137).
		q(nt(113), (nt(137)));
		//       wff(113)             => bf_less(138).
		q(nt(113), (nt(138)));
		//       wff(113)             => bf_less_equal(139).
		q(nt(113), (nt(139)));
		//       wff(113)             => bf_greater(140).
		q(nt(113), (nt(140)));
		//       wff(113)             => bf_not_less_equal(142).
		q(nt(113), (nt(142)));
		//       wff(113)             => wff_or(129).
		q(nt(113), (nt(129)));
		//       wff(113)             => wff_and(125).
		q(nt(113), (nt(125)));
		//       wff(113)             => wff_xor(127).
		q(nt(113), (nt(127)));
		//       wff(113)             => wff_neg(126).
		q(nt(113), (nt(126)));
		//       wff(113)             => wff_t(47).
		q(nt(113), (nt(47)));
		//       wff(113)             => wff_f(48).
		q(nt(113), (nt(48)));
		//       wff(113)             => capture(76).
		q(nt(113), (nt(76)));
		//       wff(113)             => bool_variable(88).
		q(nt(113), (nt(88)));
		//       wff(113)             => open_parenthesis(18) _(15) wff(113) _(15) close_parenthesis(19).
		q(nt(113), (nt(18)+nt(15)+nt(113)+nt(15)+nt(19)));
		//       _Rwff_ref_21(145)    => null.
		q(nt(145), (nul));
		//       _Rwff_ref_21(145)    => offsets(71).
		q(nt(145), (nt(71)));
		//       wff_ref(144)         => sym(70) _(15) _Rwff_ref_21(145) _(15) wff_ref_args(146).
		q(nt(144), (nt(70)+nt(15)+nt(145)+nt(15)+nt(146)));
		//       _Rwff_ref_args_22(148) => _(15) wff_ref_arg(147).
		q(nt(148), (nt(15)+nt(147)));
		//       _Rwff_ref_args_23(149) => null.
		q(nt(149), (nul));
		//       _Rwff_ref_args_23(149) => _Rwff_ref_args_22(148) _Rwff_ref_args_23(149).
		q(nt(149), (nt(148)+nt(149)));
		//       wff_ref_args(146)    => open_parenthesis(18) _Rwff_ref_args_23(149) _(15) close_parenthesis(19).
		q(nt(146), (nt(18)+nt(149)+nt(15)+nt(19)));
		//       wff_ref_arg(147)     => bf(112).
		q(nt(147), (nt(112)));
		//       wff_ref_arg(147)     => capture(76).
		q(nt(147), (nt(76)));
		//       wff_ref_arg(147)     => variable(78).
		q(nt(147), (nt(78)));
		//       wff_and(125)         => wff(113) _(15) wff_and_sym(36) _(15) wff(113).
		q(nt(125), (nt(113)+nt(15)+nt(36)+nt(15)+nt(113)));
		//       wff_or(129)          => wff(113) _(15) wff_or_sym(37) _(15) wff(113).
		q(nt(129), (nt(113)+nt(15)+nt(37)+nt(15)+nt(113)));
		//       wff_xor(127)         => wff(113) _(15) wff_xor_sym(38) _(15) wff(113).
		q(nt(127), (nt(113)+nt(15)+nt(38)+nt(15)+nt(113)));
		//       wff_conditional(128) => wff(113) _(15) wff_conditional_sym(39) _(15) wff(113) _(15) colon(25) _(15) wff(113).
		q(nt(128), (nt(113)+nt(15)+nt(39)+nt(15)+nt(113)+nt(15)+nt(25)+nt(15)+nt(113)));
		//       wff_neg(126)         => wff_neg_sym(40) _(15) wff(113).
		q(nt(126), (nt(40)+nt(15)+nt(113)));
		//       wff_imply(132)       => wff(113) _(15) wff_imply_sym(41) _(15) wff(113).
		q(nt(132), (nt(113)+nt(15)+nt(41)+nt(15)+nt(113)));
		//       wff_equiv(133)       => wff(113) _(15) wff_equiv_sym(42) _(15) wff(113).
		q(nt(133), (nt(113)+nt(15)+nt(42)+nt(15)+nt(113)));
		//       _Rwff_all_24(150)    => capture(76).
		q(nt(150), (nt(76)));
		//       _Rwff_all_24(150)    => variable(78).
		q(nt(150), (nt(78)));
		//       wff_all(130)         => wff_all_sym(43) __(13) _Rwff_all_24(150) __(13) wff(113).
		q(nt(130), (nt(43)+nt(13)+nt(150)+nt(13)+nt(113)));
		//       _Rwff_ex_25(151)     => capture(76).
		q(nt(151), (nt(76)));
		//       _Rwff_ex_25(151)     => variable(78).
		q(nt(151), (nt(78)));
		//       wff_ex(131)          => wff_ex_sym(44) __(13) _Rwff_ex_25(151) __(13) wff(113).
		q(nt(131), (nt(44)+nt(13)+nt(151)+nt(13)+nt(113)));
		//       _Rwff_ball_26(152)   => capture(76).
		q(nt(152), (nt(76)));
		//       _Rwff_ball_26(152)   => bool_variable(88).
		q(nt(152), (nt(88)));
		//       wff_ball(134)        => wff_ball_sym(45) __(13) _Rwff_ball_26(152) __(13) wff(113).
		q(nt(134), (nt(45)+nt(13)+nt(152)+nt(13)+nt(113)));
		//       _Rwff_bex_27(153)    => capture(76).
		q(nt(153), (nt(76)));
		//       _Rwff_bex_27(153)    => bool_variable(88).
		q(nt(153), (nt(88)));
		//       wff_bex(135)         => wff_bex_sym(46) __(13) _Rwff_bex_27(153) __(13) wff(113).
		q(nt(135), (nt(46)+nt(13)+nt(153)+nt(13)+nt(113)));
		//       bf_eq(136)           => bf(112) _(15) bf_equality_sym(54) _(15) bf(112).
		q(nt(136), (nt(112)+nt(15)+nt(54)+nt(15)+nt(112)));
		//       bf_neq(137)          => bf(112) _(15) bf_nequality_sym(55) _(15) bf(112).
		q(nt(137), (nt(112)+nt(15)+nt(55)+nt(15)+nt(112)));
		//       bf_less(138)         => bf(112) _(15) bf_less_sym(56) _(15) bf(112).
		q(nt(138), (nt(112)+nt(15)+nt(56)+nt(15)+nt(112)));
		//       bf_less_equal(139)   => bf(112) _(15) bf_less_equal_sym(57) _(15) bf(112).
		q(nt(139), (nt(112)+nt(15)+nt(57)+nt(15)+nt(112)));
		//       bf_not_less_equal(142) => bf(112) _(15) bf_not_less_equal_sym(58) _(15) bf(112).
		q(nt(142), (nt(112)+nt(15)+nt(58)+nt(15)+nt(112)));
		//       bf_greater(140)      => bf(112) _(15) bf_greater_sym(59) _(15) bf(112).
		q(nt(140), (nt(112)+nt(15)+nt(59)+nt(15)+nt(112)));
		//       bf_interval(141)     => bf(112) _(15) bf_less_equal_sym(57) _(15) bf(112) _(15) bf_less_equal_sym(57) _(15) bf(112).
		q(nt(141), (nt(112)+nt(15)+nt(57)+nt(15)+nt(112)+nt(15)+nt(57)+nt(15)+nt(112)));
		//       bf_rule(154)         => bf_matcher(155) _(15) bf_def(49) _(15) bf_body(156) _(15) dot(17).
		q(nt(154), (nt(155)+nt(15)+nt(49)+nt(15)+nt(156)+nt(15)+nt(17)));
		//       bf_matcher(155)      => bf(112).
		q(nt(155), (nt(112)));
		//       bf_body(156)         => bf(112).
		q(nt(156), (nt(112)));
		//       bf_body(156)         => bf_not_less_equal(142).
		q(nt(156), (nt(142)));
		//       bf_body(156)         => bf_is_zero_cb(157).
		q(nt(156), (nt(157)));
		//       bf_body(156)         => bf_is_one_cb(158).
		q(nt(156), (nt(158)));
		//       bf_body(156)         => bf_has_subformula_cb(159).
		q(nt(156), (nt(159)));
		//       bf_body(156)         => bf_remove_funiversal_cb(160).
		q(nt(156), (nt(160)));
		//       bf_body(156)         => bf_remove_fexistential_cb(161).
		q(nt(156), (nt(161)));
		//       bf_body(156)         => bf_and_cb(162).
		q(nt(156), (nt(162)));
		//       bf_body(156)         => bf_or_cb(163).
		q(nt(156), (nt(163)));
		//       bf_body(156)         => bf_xor_cb(164).
		q(nt(156), (nt(164)));
		//       bf_body(156)         => bf_neg_cb(165).
		q(nt(156), (nt(165)));
		//       bf(112)              => bf_splitter(166).
		q(nt(112), (nt(166)));
		//       bf(112)              => bf_ref(167).
		q(nt(112), (nt(167)));
		//       bf(112)              => bf_all(168).
		q(nt(112), (nt(168)));
		//       bf(112)              => bf_ex(169).
		q(nt(112), (nt(169)));
		//       bf(112)              => bf_or(170).
		q(nt(112), (nt(170)));
		//       bf(112)              => bf_and(171).
		q(nt(112), (nt(171)));
		//       bf(112)              => bf_xor(172).
		q(nt(112), (nt(172)));
		//       bf(112)              => bf_neg(173).
		q(nt(112), (nt(173)));
		//       bf(112)              => bf_constant(174).
		q(nt(112), (nt(174)));
		//       bf(112)              => bf_t(63).
		q(nt(112), (nt(63)));
		//       bf(112)              => bf_f(64).
		q(nt(112), (nt(64)));
		//       bf(112)              => capture(76).
		q(nt(112), (nt(76)));
		//       bf(112)              => variable(78).
		q(nt(112), (nt(78)));
		//       bf(112)              => open_parenthesis(18) _(15) bf(112) _(15) close_parenthesis(19).
		q(nt(112), (nt(18)+nt(15)+nt(112)+nt(15)+nt(19)));
		//       __neg_31(422)        => bf_t(63).
		q(nt(422), (nt(63)));
		//       __neg_32(423)        => bf_f(64).
		q(nt(423), (nt(64)));
		//       __neg_33(424)        => capture(76).
		q(nt(424), (nt(76)));
		//       __neg_34(425)        => variable(78).
		q(nt(425), (nt(78)));
		//       __neg_35(426)        => bf_eq(136).
		q(nt(426), (nt(136)));
		//       __neg_36(427)        => bf_neq(137).
		q(nt(427), (nt(137)));
		//       __neg_37(428)        => bf_splitter(166).
		q(nt(428), (nt(166)));
		//       __neg_38(429)        => bf_all(168).
		q(nt(429), (nt(168)));
		//       __neg_39(430)        => bf_ex(169).
		q(nt(430), (nt(169)));
		//       __neg_40(431)        => bf_or(170).
		q(nt(431), (nt(170)));
		//       __neg_41(432)        => bf_and(171).
		q(nt(432), (nt(171)));
		//       __neg_42(433)        => bf_xor(172).
		q(nt(433), (nt(172)));
		//       __neg_43(434)        => bf_neg(173).
		q(nt(434), (nt(173)));
		//       __neg_44(435)        => bf_constant(174).
		q(nt(435), (nt(174)));
		//       _Rbf_rec_relation_28(176) => ~( __neg_31(422) ) & ~( __neg_32(423) ) & ~( __neg_33(424) ) & ~( __neg_34(425) ) & bf(112) & ~( __neg_35(426) ) & ~( __neg_36(427) ) & ~( __neg_37(428) ) & ~( __neg_38(429) ) & ~( __neg_39(430) ) & ~( __neg_40(431) ) & ~( __neg_41(432) ) & ~( __neg_42(433) ) & ~( __neg_43(434) ) & ~( __neg_44(435) ).	 # conjunctive
		q(nt(176), ~(nt(422)) & ~(nt(423)) & ~(nt(424)) & ~(nt(425)) & (nt(112)) & ~(nt(426)) & ~(nt(427)) & ~(nt(428)) & ~(nt(429)) & ~(nt(430)) & ~(nt(431)) & ~(nt(432)) & ~(nt(433)) & ~(nt(434)) & ~(nt(435)));
		//       bf_rec_relation(175) => _Rbf_rec_relation_28(176) _(15) bf_def(49) _(15) bf(112) _(15) dot(17).
		q(nt(175), (nt(176)+nt(15)+nt(49)+nt(15)+nt(112)+nt(15)+nt(17)));
		//       _Rbf_ref_29(177)     => null.
		q(nt(177), (nul));
		//       _Rbf_ref_29(177)     => offsets(71).
		q(nt(177), (nt(71)));
		//       bf_ref(167)          => sym(70) _Rbf_ref_29(177) bf_ref_args(178).
		q(nt(167), (nt(70)+nt(177)+nt(178)));
		//       _Rbf_ref_args_30(180) => _(15) bf_ref_arg(179).
		q(nt(180), (nt(15)+nt(179)));
		//       _Rbf_ref_args_31(181) => null.
		q(nt(181), (nul));
		//       _Rbf_ref_args_31(181) => _Rbf_ref_args_30(180) _Rbf_ref_args_31(181).
		q(nt(181), (nt(180)+nt(181)));
		//       bf_ref_args(178)     => open_parenthesis(18) _Rbf_ref_args_31(181) _(15) close_parenthesis(19).
		q(nt(178), (nt(18)+nt(181)+nt(15)+nt(19)));
		//       bf_ref_arg(179)      => bf(112).
		q(nt(179), (nt(112)));
		//       bf_ref_arg(179)      => capture(76).
		q(nt(179), (nt(76)));
		//       bf_ref_arg(179)      => variable(78).
		q(nt(179), (nt(78)));
		//       bf_and(171)          => bf(112) _(15) bf_and_sym(50) _(15) bf(112).
		q(nt(171), (nt(112)+nt(15)+nt(50)+nt(15)+nt(112)));
		//       bf_or(170)           => bf(112) _(15) bf_or_sym(51) _(15) bf(112).
		q(nt(170), (nt(112)+nt(15)+nt(51)+nt(15)+nt(112)));
		//       bf_xor(172)          => bf(112) _(15) bf_xor_sym(52) _(15) bf(112).
		q(nt(172), (nt(112)+nt(15)+nt(52)+nt(15)+nt(112)));
		//       bf_neg(173)          => bf(112) _(15) bf_neg_sym(53).
		q(nt(173), (nt(112)+nt(15)+nt(53)));
		//       _Rbf_all_32(182)     => capture(76).
		q(nt(182), (nt(76)));
		//       _Rbf_all_32(182)     => variable(78).
		q(nt(182), (nt(78)));
		//       bf_all(168)          => bf_all_sym(60) __(13) _Rbf_all_32(182) __(13) bf(112).
		q(nt(168), (nt(60)+nt(13)+nt(182)+nt(13)+nt(112)));
		//       _Rbf_ex_33(183)      => capture(76).
		q(nt(183), (nt(76)));
		//       _Rbf_ex_33(183)      => variable(78).
		q(nt(183), (nt(78)));
		//       bf_ex(169)           => bf_ex_sym(61) __(13) _Rbf_ex_33(183) __(13) bf(112).
		q(nt(169), (nt(61)+nt(13)+nt(183)+nt(13)+nt(112)));
		//       bf_splitter(166)     => bf_splitter_sym(62) _(15) open_parenthesis(18) _(15) bf(112) _(15) close_parenthesis(19).
		q(nt(166), (nt(62)+nt(15)+nt(18)+nt(15)+nt(112)+nt(15)+nt(19)));
		//       bf_constant(174)     => open_brace(22) _(15) constant(184) _(15) close_brace(23).
		q(nt(174), (nt(22)+nt(15)+nt(184)+nt(15)+nt(23)));
		//       constant(184)        => capture(76).
		q(nt(184), (nt(76)));
		//       constant(184)        => binding(185).
		q(nt(184), (nt(185)));
		//       binding(185)         => source_binding(186).
		q(nt(185), (nt(186)));
		//       binding(185)         => named_binding(187).
		q(nt(185), (nt(187)));
		//       named_binding(187)   => chars(65).
		q(nt(187), (nt(65)));
		//       source_binding(186)  => type(188) _(15) colon(25) _(15) source(189).
		q(nt(186), (nt(188)+nt(15)+nt(25)+nt(15)+nt(189)));
		//       type(188)            => null.
		q(nt(188), (nul));
		//       type(188)            => chars(65).
		q(nt(188), (nt(65)));
		//       source0(190)         => alnum(1).
		q(nt(190), (nt(1)));
		//       source0(190)         => punct(6).
		q(nt(190), (nt(6)));
		//       source0(190)         => space(7).
		q(nt(190), (nt(7)));
		//       _Rsource_34(191)     => source0(190).
		q(nt(191), (nt(190)));
		//       _Rsource_35(192)     => _Rsource_34(191).
		q(nt(192), (nt(191)));
		//       _Rsource_35(192)     => _Rsource_34(191) _Rsource_35(192).
		q(nt(192), (nt(191)+nt(192)));
		//       source(189)          => _Rsource_35(192).
		q(nt(189), (nt(192)));
		//       bf_and_cb(162)       => bf_and_cb_sym(193) __(13) bf_cb_arg(194) __(13) bf_cb_arg(194).
		q(nt(162), (nt(193)+nt(13)+nt(194)+nt(13)+nt(194)));
		//       bf_or_cb(163)        => bf_or_cb_sym(195) __(13) bf_cb_arg(194) __(13) bf_cb_arg(194).
		q(nt(163), (nt(195)+nt(13)+nt(194)+nt(13)+nt(194)));
		//       bf_xor_cb(164)       => bf_xor_cb_sym(196) __(13) bf_cb_arg(194) __(13) bf_cb_arg(194).
		q(nt(164), (nt(196)+nt(13)+nt(194)+nt(13)+nt(194)));
		//       bf_neg_cb(165)       => bf_neg_cb_sym(197) __(13) bf_cb_arg(194).
		q(nt(165), (nt(197)+nt(13)+nt(194)));
		//       bf_eq_cb(117)        => bf_eq_cb_sym(198) __(13) bf_cb_arg(194) __(13) wff_cb_arg(199) __(13) wff_cb_arg(199).
		q(nt(117), (nt(198)+nt(13)+nt(194)+nt(13)+nt(199)+nt(13)+nt(199)));
		//       bf_neq_cb(118)       => bf_neq_cb_sym(200) __(13) bf_cb_arg(194) __(13) wff_cb_arg(199) __(13) wff_cb_arg(199).
		q(nt(118), (nt(200)+nt(13)+nt(194)+nt(13)+nt(199)+nt(13)+nt(199)));
		//       bf_is_zero_cb(157)   => bf_is_zero_cb_sym(201) __(13) bf_cb_arg(194) __(13) bf_cb_arg(194).
		q(nt(157), (nt(201)+nt(13)+nt(194)+nt(13)+nt(194)));
		//       bf_is_one_cb(158)    => bf_is_one_cb_sym(202) __(13) bf_cb_arg(194) __(13) bf_cb_arg(194).
		q(nt(158), (nt(202)+nt(13)+nt(194)+nt(13)+nt(194)));
		//       bf_remove_funiversal_cb(160) => bf_remove_funiversal_cb_sym(203) __(13) bf_cb_arg(194) __(13) bf_cb_arg(194) __(13) bf_cb_arg(194) __(13) bf_cb_arg(194).
		q(nt(160), (nt(203)+nt(13)+nt(194)+nt(13)+nt(194)+nt(13)+nt(194)+nt(13)+nt(194)));
		//       bf_remove_fexistential_cb(161) => bf_remove_fexistential_cb_sym(204) __(13) bf_cb_arg(194) __(13) bf_cb_arg(194) __(13) bf_cb_arg(194) __(13) bf_cb_arg(194).
		q(nt(161), (nt(204)+nt(13)+nt(194)+nt(13)+nt(194)+nt(13)+nt(194)+nt(13)+nt(194)));
		//       wff_remove_existential_cb(121) => wff_remove_existential_cb_sym(205) __(13) wff_cb_arg(199) __(13) wff_cb_arg(199).
		q(nt(121), (nt(205)+nt(13)+nt(199)+nt(13)+nt(199)));
		//       wff_remove_bexistential_cb(122) => wff_remove_bexistential_cb_sym(206) __(13) wff_cb_arg(199) __(13) wff_cb_arg(199) __(13) wff_cb_arg(199) __(13) wff_cb_arg(199).
		q(nt(122), (nt(206)+nt(13)+nt(199)+nt(13)+nt(199)+nt(13)+nt(199)+nt(13)+nt(199)));
		//       wff_remove_buniversal_cb(123) => wff_remove_buniversal_cb_sym(207) __(13) wff_cb_arg(199) __(13) wff_cb_arg(199) __(13) wff_cb_arg(199) __(13) wff_cb_arg(199).
		q(nt(123), (nt(207)+nt(13)+nt(199)+nt(13)+nt(199)+nt(13)+nt(199)+nt(13)+nt(199)));
		//       wff_has_clashing_subformulas_cb(119) => wff_has_clashing_subformulas_cb_sym(208) __(13) wff_cb_arg(199) __(13) wff_cb_arg(199).
		q(nt(119), (nt(208)+nt(13)+nt(199)+nt(13)+nt(199)));
		//       bf_has_subformula_cb(159) => bf_has_subformula_cb_sym(209) __(13) bf_cb_arg(194) __(13) bf_cb_arg(194) __(13) bf_cb_arg(194).
		q(nt(159), (nt(209)+nt(13)+nt(194)+nt(13)+nt(194)+nt(13)+nt(194)));
		//       wff_has_subformula_cb(120) => wff_has_subformula_cb_sym(210) __(13) wff_cb_arg(199) __(13) wff_cb_arg(199) __(13) wff_cb_arg(199).
		q(nt(120), (nt(210)+nt(13)+nt(199)+nt(13)+nt(199)+nt(13)+nt(199)));
		//       bf_has_subformula_cb_sym(209) => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b'.
		q(nt(209), (t(30)+t(34)+t(40)+t(42)+t(26)+t(43)+t(40)+t(43)+t(44)+t(30)+t(34)+t(41)+t(45)+t(46)+t(44)+t(27)+t(26)+t(40)+t(47)+t(30)));
		//       wff_has_clashing_subformulas_cb_sym(208) => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b'.
		q(nt(208), (t(48)+t(34)+t(34)+t(40)+t(42)+t(26)+t(43)+t(40)+t(47)+t(27)+t(26)+t(43)+t(42)+t(39)+t(49)+t(50)+t(40)+t(43)+t(44)+t(30)+t(34)+t(41)+t(45)+t(46)+t(44)+t(27)+t(26)+t(43)+t(40)+t(47)+t(30)));
		//       wff_has_subformula_cb_sym(210) => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b'.
		q(nt(210), (t(48)+t(34)+t(34)+t(40)+t(42)+t(26)+t(43)+t(40)+t(43)+t(44)+t(30)+t(34)+t(41)+t(45)+t(46)+t(44)+t(27)+t(26)+t(40)+t(47)+t(30)));
		//       wff_remove_existential_cb_sym(205) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(205), (t(48)+t(34)+t(34)+t(40)+t(45)+t(28)+t(46)+t(41)+t(51)+t(28)+t(40)+t(28)+t(29)+t(39)+t(43)+t(52)+t(28)+t(49)+t(52)+t(39)+t(26)+t(27)+t(40)+t(47)+t(30)));
		//       wff_remove_bexistential_cb_sym(206) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(206), (t(48)+t(34)+t(34)+t(40)+t(45)+t(28)+t(46)+t(41)+t(51)+t(28)+t(40)+t(30)+t(28)+t(29)+t(39)+t(43)+t(52)+t(28)+t(49)+t(52)+t(39)+t(26)+t(27)+t(40)+t(47)+t(30)));
		//       wff_remove_buniversal_cb_sym(207) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b'.
		q(nt(207), (t(48)+t(34)+t(34)+t(40)+t(45)+t(28)+t(46)+t(41)+t(51)+t(28)+t(40)+t(30)+t(44)+t(49)+t(39)+t(51)+t(28)+t(45)+t(43)+t(26)+t(27)+t(40)+t(47)+t(30)));
		//       bf_remove_fexistential_cb_sym(204) => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(204), (t(30)+t(34)+t(40)+t(45)+t(28)+t(46)+t(41)+t(51)+t(28)+t(40)+t(34)+t(28)+t(29)+t(39)+t(43)+t(52)+t(28)+t(49)+t(52)+t(39)+t(26)+t(27)+t(40)+t(47)+t(30)));
		//       bf_remove_funiversal_cb_sym(203) => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b'.
		q(nt(203), (t(30)+t(34)+t(40)+t(45)+t(28)+t(46)+t(41)+t(51)+t(28)+t(40)+t(34)+t(44)+t(49)+t(39)+t(51)+t(28)+t(45)+t(43)+t(26)+t(27)+t(40)+t(47)+t(30)));
		//       bf_cb_arg(194)       => bf(112).
		q(nt(194), (nt(112)));
		//       wff_cb_arg(199)      => wff(113).
		q(nt(199), (nt(113)));
		//       bf_and_cb_sym(193)   => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b'.
		q(nt(193), (t(30)+t(34)+t(40)+t(26)+t(49)+t(53)+t(40)+t(47)+t(30)));
		//       bf_or_cb_sym(195)    => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b'.
		q(nt(195), (t(30)+t(34)+t(40)+t(41)+t(45)+t(40)+t(47)+t(30)));
		//       bf_xor_cb_sym(196)   => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b'.
		q(nt(196), (t(30)+t(34)+t(40)+t(29)+t(41)+t(45)+t(40)+t(47)+t(30)));
		//       bf_neg_cb_sym(197)   => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b'.
		q(nt(197), (t(30)+t(34)+t(40)+t(49)+t(28)+t(50)+t(40)+t(47)+t(30)));
		//       bf_eq_cb_sym(198)    => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b'.
		q(nt(198), (t(30)+t(34)+t(40)+t(28)+t(54)+t(40)+t(47)+t(30)));
		//       bf_neq_cb_sym(200)   => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b'.
		q(nt(200), (t(30)+t(34)+t(40)+t(49)+t(28)+t(54)+t(40)+t(47)+t(30)));
		//       bf_is_zero_cb_sym(201) => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b'.
		q(nt(201), (t(30)+t(34)+t(40)+t(39)+t(43)+t(40)+t(55)+t(28)+t(45)+t(41)+t(40)+t(47)+t(30)));
		//       bf_is_one_cb_sym(202) => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b'.
		q(nt(202), (t(30)+t(34)+t(40)+t(39)+t(43)+t(40)+t(41)+t(49)+t(28)+t(40)+t(47)+t(30)));
		//       _Rtau_collapse_positives_cb_36(213) => tau_collapse_positives_cb_sym(211) __(13) tau_cb_arg(212) __(13) tau_cb_arg(212) __(13) tau_cb_arg(212).
		q(nt(213), (nt(211)+nt(13)+nt(212)+nt(13)+nt(212)+nt(13)+nt(212)));
		//       _Rtau_collapse_positives_cb_37(214) => tau_collapse_positives_cb_sym(211) __(13) tau_cb_arg(212) __(13) tau_cb_arg(212).
		q(nt(214), (nt(211)+nt(13)+nt(212)+nt(13)+nt(212)));
		//       tau_collapse_positives_cb(98) => _Rtau_collapse_positives_cb_36(213).
		q(nt(98), (nt(213)));
		//       tau_collapse_positives_cb(98) => _Rtau_collapse_positives_cb_37(214).
		q(nt(98), (nt(214)));
		//       tau_positives_upwards_cb(99) => tau_positives_upwards_cb_sym(215) __(13) tau_cb_arg(212) __(13) tau_cb_arg(212).
		q(nt(99), (nt(215)+nt(13)+nt(212)+nt(13)+nt(212)));
		//       tau_cb_arg(212)      => capture(76).
		q(nt(212), (nt(76)));
		//       tau_cb_arg(212)      => tau(97).
		q(nt(212), (nt(97)));
		//       tau_collapse_positives_cb_sym(211) => 't' 'a' 'u' '_' 'c' 'o' 'l' 'l' 'a' 'p' 's' 'e' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'c' 'b'.
		q(nt(211), (t(52)+t(26)+t(44)+t(40)+t(47)+t(41)+t(27)+t(27)+t(26)+t(56)+t(43)+t(28)+t(40)+t(56)+t(41)+t(43)+t(39)+t(52)+t(39)+t(51)+t(28)+t(43)+t(40)+t(47)+t(30)));
		//       tau_positives_upwards_cb_sym(215) => 't' 'a' 'u' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'u' 'p' 'w' 'a' 'r' 'd' 's' '_' 'c' 'b'.
		q(nt(215), (t(52)+t(26)+t(44)+t(40)+t(56)+t(41)+t(43)+t(39)+t(52)+t(39)+t(51)+t(28)+t(43)+t(40)+t(44)+t(56)+t(48)+t(26)+t(45)+t(53)+t(43)+t(40)+t(47)+t(30)));
		//       _Rinputs_38(218)     => _(15) input(217).
		q(nt(218), (nt(15)+nt(217)));
		//       _Rinputs_39(219)     => null.
		q(nt(219), (nul));
		//       _Rinputs_39(219)     => _Rinputs_38(218) _Rinputs_39(219).
		q(nt(219), (nt(218)+nt(219)));
		//       inputs(216)          => _(15) less(27) _(15) input(217) _Rinputs_39(219) _(15) dot(17).
		q(nt(216), (nt(15)+nt(27)+nt(15)+nt(217)+nt(219)+nt(15)+nt(17)));
		//       input(217)           => in(84) _(15) colon(25) _(15) open_brace(22) _(15) source_binding(186) _(15) close_brace(23).
		q(nt(217), (nt(84)+nt(15)+nt(25)+nt(15)+nt(22)+nt(15)+nt(186)+nt(15)+nt(23)));
		//       builder(220)         => _(15) builder_head(221) _(15) builder_body(222) _(15) dot(17).
		q(nt(220), (nt(15)+nt(221)+nt(15)+nt(222)+nt(15)+nt(17)));
		//       _Rbuilder_head_40(223) => __(13) capture(76).
		q(nt(223), (nt(13)+nt(76)));
		//       _Rbuilder_head_41(224) => null.
		q(nt(224), (nul));
		//       _Rbuilder_head_41(224) => _Rbuilder_head_40(223) _Rbuilder_head_41(224).
		q(nt(224), (nt(223)+nt(224)));
		//       builder_head(221)    => open_parenthesis(18) _(15) capture(76) _Rbuilder_head_41(224) _(15) close_parenthesis(19).
		q(nt(221), (nt(18)+nt(15)+nt(76)+nt(224)+nt(15)+nt(19)));
		//       builder_body(222)    => bf_builder_body(225).
		q(nt(222), (nt(225)));
		//       builder_body(222)    => wff_builder_body(226).
		q(nt(222), (nt(226)));
		//       builder_body(222)    => tau_builder_body(227).
		q(nt(222), (nt(227)));
		//       bf_builder_body(225) => bf_def(49) _(15) bf(112).
		q(nt(225), (nt(49)+nt(15)+nt(112)));
		//       wff_builder_body(226) => wff_def(35) _(15) wff(113).
		q(nt(226), (nt(35)+nt(15)+nt(113)));
		//       tau_builder_body(227) => tau_def(31) _(15) tau(97).
		q(nt(227), (nt(31)+nt(15)+nt(97)));
		//       library(228)         => rules(229).
		q(nt(228), (nt(229)));
		//       _Rrules_42(231)      => _(15) rule(230).
		q(nt(231), (nt(15)+nt(230)));
		//       _Rrules_43(232)      => null.
		q(nt(232), (nul));
		//       _Rrules_43(232)      => _Rrules_42(231) _Rrules_43(232).
		q(nt(232), (nt(231)+nt(232)));
		//       rules(229)           => _Rrules_43(232).
		q(nt(229), (nt(232)));
		//       rule(230)            => tau_rule(94).
		q(nt(230), (nt(94)));
		//       rule(230)            => wff_rule(114).
		q(nt(230), (nt(114)));
		//       rule(230)            => bf_rule(154).
		q(nt(230), (nt(154)));
		//       nso_rr(233)          => nso_rec_relations(234) nso_main(235).
		q(nt(233), (nt(234)+nt(235)));
		//       _Rnso_rec_relations_44(237) => _(15) nso_rec_relation(236).
		q(nt(237), (nt(15)+nt(236)));
		//       _Rnso_rec_relations_45(238) => null.
		q(nt(238), (nul));
		//       _Rnso_rec_relations_45(238) => _Rnso_rec_relations_44(237) _Rnso_rec_relations_45(238).
		q(nt(238), (nt(237)+nt(238)));
		//       nso_rec_relations(234) => _Rnso_rec_relations_45(238).
		q(nt(234), (nt(238)));
		//       nso_rec_relation(236) => wff_rec_relation(124).
		q(nt(236), (nt(124)));
		//       nso_rec_relation(236) => bf_rec_relation(175).
		q(nt(236), (nt(175)));
		//       nso_main(235)        => _(15) wff(113) _(15) dot(17).
		q(nt(235), (nt(15)+nt(113)+nt(15)+nt(17)));
		//       gssotc_rr(239)       => gssotc_rec_relations(240) gssotc_main(241).
		q(nt(239), (nt(240)+nt(241)));
		//       _Rgssotc_rec_relations_46(243) => _(15) gssotc_rec_relation(242).
		q(nt(243), (nt(15)+nt(242)));
		//       _Rgssotc_rec_relations_47(244) => null.
		q(nt(244), (nul));
		//       _Rgssotc_rec_relations_47(244) => _Rgssotc_rec_relations_46(243) _Rgssotc_rec_relations_47(244).
		q(nt(244), (nt(243)+nt(244)));
		//       gssotc_rec_relations(240) => _Rgssotc_rec_relations_47(244).
		q(nt(240), (nt(244)));
		//       gssotc_rec_relation(242) => tau_rec_relation(100).
		q(nt(242), (nt(100)));
		//       gssotc_rec_relation(242) => wff_rec_relation(124).
		q(nt(242), (nt(124)));
		//       gssotc_rec_relation(242) => bf_rec_relation(175).
		q(nt(242), (nt(175)));
		//       gssotc_main(241)     => _(15) tau(97) _(15) semicolon(26).
		q(nt(241), (nt(15)+nt(97)+nt(15)+nt(26)));
		//       _Rstart_48(246)      => inputs(216).
		q(nt(246), (nt(216)));
		//       _Rstart_48(246)      => builder(220).
		q(nt(246), (nt(220)));
		//       _Rstart_48(246)      => library(228).
		q(nt(246), (nt(228)));
		//       _Rstart_48(246)      => nso_rr(233).
		q(nt(246), (nt(233)));
		//       _Rstart_48(246)      => gssotc_rr(239).
		q(nt(246), (nt(239)));
		//       start(245)           => _Rstart_48(246) _(15).
		q(nt(245), (nt(246)+nt(15)));
		//       _Rcli_49(249)        => _(15) dot(17) _(15) cli_command(248).
		q(nt(249), (nt(15)+nt(17)+nt(15)+nt(248)));
		//       _Rcli_50(250)        => null.
		q(nt(250), (nul));
		//       _Rcli_50(250)        => _Rcli_49(249) _Rcli_50(250).
		q(nt(250), (nt(249)+nt(250)));
		//       _Rcli_51(251)        => null.
		q(nt(251), (nul));
		//       _Rcli_51(251)        => _(15) dot(17).
		q(nt(251), (nt(15)+nt(17)));
		//       cli(247)             => _(15) cli_command(248) _Rcli_50(250) _Rcli_51(251) _(15).
		q(nt(247), (nt(15)+nt(248)+nt(250)+nt(251)+nt(15)));
		//       cli_command(248)     => help(252).
		q(nt(248), (nt(252)));
		//       cli_command(248)     => version(253).
		q(nt(248), (nt(253)));
		//       cli_command(248)     => quit(254).
		q(nt(248), (nt(254)));
		//       cli_command(248)     => get(255).
		q(nt(248), (nt(255)));
		//       cli_command(248)     => set(256).
		q(nt(248), (nt(256)));
		//       cli_command(248)     => toggle(257).
		q(nt(248), (nt(257)));
		//       cli_command(248)     => list_outputs(258).
		q(nt(248), (nt(258)));
		//       cli_command(248)     => clear_outputs(259).
		q(nt(248), (nt(259)));
		//       cli_command(248)     => print_output(260).
		q(nt(248), (nt(260)));
		//       cli_command(248)     => file(261).
		q(nt(248), (nt(261)));
		//       cli_command(248)     => normalize(262).
		q(nt(248), (nt(262)));
		//       cli_command(248)     => wff_onf_cmd(263).
		q(nt(248), (nt(263)));
		//       cli_command(248)     => wff_dnf_cmd(264).
		q(nt(248), (nt(264)));
		//       cli_command(248)     => wff_cnf_cmd(265).
		q(nt(248), (nt(265)));
		//       cli_command(248)     => wff_anf_cmd(266).
		q(nt(248), (nt(266)));
		//       cli_command(248)     => wff_nnf_cmd(267).
		q(nt(248), (nt(267)));
		//       cli_command(248)     => wff_pnf_cmd(268).
		q(nt(248), (nt(268)));
		//       cli_command(248)     => wff_mnf_cmd(269).
		q(nt(248), (nt(269)));
		//       cli_command(248)     => bf_dnf_cmd(270).
		q(nt(248), (nt(270)));
		//       cli_command(248)     => bf_cnf_cmd(271).
		q(nt(248), (nt(271)));
		//       cli_command(248)     => bf_anf_cmd(272).
		q(nt(248), (nt(272)));
		//       cli_command(248)     => bf_nnf_cmd(273).
		q(nt(248), (nt(273)));
		//       cli_command(248)     => bf_pnf_cmd(274).
		q(nt(248), (nt(274)));
		//       cli_command(248)     => bf_mnf_cmd(275).
		q(nt(248), (nt(275)));
		//       cli_command(248)     => bf_substitute(276).
		q(nt(248), (nt(276)));
		//       cli_command(248)     => wff_substitute(277).
		q(nt(248), (nt(277)));
		//       cli_command(248)     => bf_instantiate(278).
		q(nt(248), (nt(278)));
		//       cli_command(248)     => wff_instantiate(279).
		q(nt(248), (nt(279)));
		//       cli_command(248)     => bf(112).
		q(nt(248), (nt(112)));
		//       cli_command(248)     => wff(113).
		q(nt(248), (nt(113)));
		//       cli_command(248)     => rr_nso(280).
		q(nt(248), (nt(280)));
		//       normalize(262)       => normalize_sym(281) __(13) form_arg(282).
		q(nt(262), (nt(281)+nt(13)+nt(282)));
		//       onf(283)             => onf_sym(284) __(13) var_arg(285) form_arg(282).
		q(nt(283), (nt(284)+nt(13)+nt(285)+nt(282)));
		//       dnf(286)             => dnf_sym(287) __(13) form_arg(282).
		q(nt(286), (nt(287)+nt(13)+nt(282)));
		//       cnf(288)             => cnf_sym(289) __(13) form_arg(282).
		q(nt(288), (nt(289)+nt(13)+nt(282)));
		//       anf(290)             => anf_sym(291) __(13) form_arg(282).
		q(nt(290), (nt(291)+nt(13)+nt(282)));
		//       nnf(292)             => nnf_sym(293) __(13) form_arg(282).
		q(nt(292), (nt(293)+nt(13)+nt(282)));
		//       pnf(294)             => pnf_sym(295) __(13) form_arg(282).
		q(nt(294), (nt(295)+nt(13)+nt(282)));
		//       minterm(296)         => minterm_sym(297) __(13) form_arg(282).
		q(nt(296), (nt(297)+nt(13)+nt(282)));
		//       file(261)            => file_sym(298) quote(30) file_path(299) quote(30).
		q(nt(261), (nt(298)+nt(30)+nt(299)+nt(30)));
		//       _Rnormalize_cmd_52(304) => wff(113).
		q(nt(304), (nt(113)));
		//       _Rnormalize_cmd_52(304) => nso_rr(233).
		q(nt(304), (nt(233)));
		//       _Rnormalize_cmd_52(304) => output(302).
		q(nt(304), (nt(302)));
		//       _Rnormalize_cmd_52(304) => wff_selection(303).
		q(nt(304), (nt(303)));
		//       normalize_cmd(300)   => normalize_cmd_sym(301) __(13) _Rnormalize_cmd_52(304).
		q(nt(300), (nt(301)+nt(13)+nt(304)));
		//       _Rbf_instantiate_cmd_53(308) => bf(112).
		q(nt(308), (nt(112)));
		//       _Rbf_instantiate_cmd_53(308) => output(302).
		q(nt(308), (nt(302)));
		//       _Rbf_instantiate_cmd_53(308) => bf_selection(307).
		q(nt(308), (nt(307)));
		//       _Rbf_instantiate_cmd_54(309) => output(302).
		q(nt(309), (nt(302)));
		//       _Rbf_instantiate_cmd_54(309) => bf_selection(307).
		q(nt(309), (nt(307)));
		//       bf_instantiate_cmd(305) => instantiate_cmd_sym(306) __(13) variable(78) __(13) _Rbf_instantiate_cmd_53(308) __(13) _Rbf_instantiate_cmd_54(309).
		q(nt(305), (nt(306)+nt(13)+nt(78)+nt(13)+nt(308)+nt(13)+nt(309)));
		//       _Rwff_instantiate_cmd_55(311) => wff(113).
		q(nt(311), (nt(113)));
		//       _Rwff_instantiate_cmd_55(311) => output(302).
		q(nt(311), (nt(302)));
		//       _Rwff_instantiate_cmd_55(311) => wff_selection(303).
		q(nt(311), (nt(303)));
		//       _Rwff_instantiate_cmd_56(312) => output(302).
		q(nt(312), (nt(302)));
		//       _Rwff_instantiate_cmd_56(312) => wff_selection(303).
		q(nt(312), (nt(303)));
		//       wff_instantiate_cmd(310) => instantiate_cmd_sym(306) __(13) variable(78) __(13) _Rwff_instantiate_cmd_55(311) __(13) _Rwff_instantiate_cmd_56(312).
		q(nt(310), (nt(306)+nt(13)+nt(78)+nt(13)+nt(311)+nt(13)+nt(312)));
		//       _Rbf_substitute_cmd_57(315) => bf(112).
		q(nt(315), (nt(112)));
		//       _Rbf_substitute_cmd_57(315) => output(302).
		q(nt(315), (nt(302)));
		//       _Rbf_substitute_cmd_57(315) => bf_selection(307).
		q(nt(315), (nt(307)));
		//       _Rbf_substitute_cmd_58(316) => output(302).
		q(nt(316), (nt(302)));
		//       _Rbf_substitute_cmd_58(316) => bf_selection(307).
		q(nt(316), (nt(307)));
		//       _Rbf_substitute_cmd_59(317) => output(302).
		q(nt(317), (nt(302)));
		//       _Rbf_substitute_cmd_59(317) => bf_selection(307).
		q(nt(317), (nt(307)));
		//       bf_substitute_cmd(313) => substitute_cmd_sym(314) __(13) _Rbf_substitute_cmd_57(315) __(13) _Rbf_substitute_cmd_58(316) _Rbf_substitute_cmd_59(317).
		q(nt(313), (nt(314)+nt(13)+nt(315)+nt(13)+nt(316)+nt(317)));
		//       _Rwff_substitute_cmd_60(320) => wff(113).
		q(nt(320), (nt(113)));
		//       _Rwff_substitute_cmd_60(320) => output(302).
		q(nt(320), (nt(302)));
		//       _Rwff_substitute_cmd_60(320) => wff_selection(303).
		q(nt(320), (nt(303)));
		//       _Rwff_substitute_cmd_61(321) => output(302).
		q(nt(321), (nt(302)));
		//       _Rwff_substitute_cmd_61(321) => wff_selection(303).
		q(nt(321), (nt(303)));
		//       _Rwff_substitute_cmd_62(322) => output(302).
		q(nt(322), (nt(302)));
		//       _Rwff_substitute_cmd_62(322) => wff_selection(303).
		q(nt(322), (nt(303)));
		//       wff_substitute_cmd(318) => substitutecmd__sym(319) __(13) _Rwff_substitute_cmd_60(320) __(13) _Rwff_substitute_cmd_61(321) _Rwff_substitute_cmd_62(322).
		q(nt(318), (nt(319)+nt(13)+nt(320)+nt(13)+nt(321)+nt(322)));
		//       _Rbf_dnf_cmd_63(324) => bf(112).
		q(nt(324), (nt(112)));
		//       _Rbf_dnf_cmd_63(324) => output(302).
		q(nt(324), (nt(302)));
		//       _Rbf_dnf_cmd_63(324) => bf_selection(307).
		q(nt(324), (nt(307)));
		//       bf_dnf_cmd(270)      => dnf_cmd_sym(323) __(13) _Rbf_dnf_cmd_63(324).
		q(nt(270), (nt(323)+nt(13)+nt(324)));
		//       _Rbf_cnf_cmd_64(326) => bf(112).
		q(nt(326), (nt(112)));
		//       _Rbf_cnf_cmd_64(326) => output(302).
		q(nt(326), (nt(302)));
		//       _Rbf_cnf_cmd_64(326) => bf_selection(307).
		q(nt(326), (nt(307)));
		//       bf_cnf_cmd(271)      => cnf_cmd_sym(325) __(13) _Rbf_cnf_cmd_64(326).
		q(nt(271), (nt(325)+nt(13)+nt(326)));
		//       _Rbf_anf_cmd_65(328) => bf(112).
		q(nt(328), (nt(112)));
		//       _Rbf_anf_cmd_65(328) => output(302).
		q(nt(328), (nt(302)));
		//       _Rbf_anf_cmd_65(328) => bf_selection(307).
		q(nt(328), (nt(307)));
		//       bf_anf_cmd(272)      => anf_cmd_sym(327) __(13) _Rbf_anf_cmd_65(328).
		q(nt(272), (nt(327)+nt(13)+nt(328)));
		//       _Rbf_nnf_cmd_66(330) => bf(112).
		q(nt(330), (nt(112)));
		//       _Rbf_nnf_cmd_66(330) => output(302).
		q(nt(330), (nt(302)));
		//       _Rbf_nnf_cmd_66(330) => bf_selection(307).
		q(nt(330), (nt(307)));
		//       bf_nnf_cmd(273)      => nnf_cmd_sym(329) __(13) _Rbf_nnf_cmd_66(330).
		q(nt(273), (nt(329)+nt(13)+nt(330)));
		//       _Rbf_pnf_cmd_67(332) => bf(112).
		q(nt(332), (nt(112)));
		//       _Rbf_pnf_cmd_67(332) => output(302).
		q(nt(332), (nt(302)));
		//       _Rbf_pnf_cmd_67(332) => bf_selection(307).
		q(nt(332), (nt(307)));
		//       bf_pnf_cmd(274)      => pnf_cmd_sym(331) __(13) _Rbf_pnf_cmd_67(332).
		q(nt(274), (nt(331)+nt(13)+nt(332)));
		//       _Rbf_mnf_cmd_68(334) => bf(112).
		q(nt(334), (nt(112)));
		//       _Rbf_mnf_cmd_68(334) => output(302).
		q(nt(334), (nt(302)));
		//       _Rbf_mnf_cmd_68(334) => bf_selection(307).
		q(nt(334), (nt(307)));
		//       bf_mnf_cmd(275)      => mnf_cmd_sym(333) __(13) _Rbf_mnf_cmd_68(334).
		q(nt(275), (nt(333)+nt(13)+nt(334)));
		//       _Rwff_onf_cmd_69(336) => wff(113).
		q(nt(336), (nt(113)));
		//       _Rwff_onf_cmd_69(336) => output(302).
		q(nt(336), (nt(302)));
		//       _Rwff_onf_cmd_69(336) => wff_selection(303).
		q(nt(336), (nt(303)));
		//       wff_onf_cmd(263)     => onf_cmd_sym(335) __(13) variable(78) _Rwff_onf_cmd_69(336).
		q(nt(263), (nt(335)+nt(13)+nt(78)+nt(336)));
		//       _Rwff_dnf_cmd_70(337) => wff(113).
		q(nt(337), (nt(113)));
		//       _Rwff_dnf_cmd_70(337) => output(302).
		q(nt(337), (nt(302)));
		//       _Rwff_dnf_cmd_70(337) => wff_selection(303).
		q(nt(337), (nt(303)));
		//       wff_dnf_cmd(264)     => dnf_cmd_sym(323) __(13) _Rwff_dnf_cmd_70(337).
		q(nt(264), (nt(323)+nt(13)+nt(337)));
		//       _Rwff_cnf_cmd_71(338) => wff(113).
		q(nt(338), (nt(113)));
		//       _Rwff_cnf_cmd_71(338) => output(302).
		q(nt(338), (nt(302)));
		//       _Rwff_cnf_cmd_71(338) => wff_selection(303).
		q(nt(338), (nt(303)));
		//       wff_cnf_cmd(265)     => cnf_cmd_sym(325) __(13) _Rwff_cnf_cmd_71(338).
		q(nt(265), (nt(325)+nt(13)+nt(338)));
		//       _Rwff_anf_cmd_72(339) => wff(113).
		q(nt(339), (nt(113)));
		//       _Rwff_anf_cmd_72(339) => output(302).
		q(nt(339), (nt(302)));
		//       _Rwff_anf_cmd_72(339) => wff_selection(303).
		q(nt(339), (nt(303)));
		//       wff_anf_cmd(266)     => anf_cmd_sym(327) __(13) _Rwff_anf_cmd_72(339).
		q(nt(266), (nt(327)+nt(13)+nt(339)));
		//       _Rwff_nnf_cmd_73(340) => wff(113).
		q(nt(340), (nt(113)));
		//       _Rwff_nnf_cmd_73(340) => output(302).
		q(nt(340), (nt(302)));
		//       _Rwff_nnf_cmd_73(340) => wff_selection(303).
		q(nt(340), (nt(303)));
		//       wff_nnf_cmd(267)     => nnf_cmd_sym(329) __(13) _Rwff_nnf_cmd_73(340).
		q(nt(267), (nt(329)+nt(13)+nt(340)));
		//       _Rwff_pnf_cmd_74(341) => wff(113).
		q(nt(341), (nt(113)));
		//       _Rwff_pnf_cmd_74(341) => output(302).
		q(nt(341), (nt(302)));
		//       _Rwff_pnf_cmd_74(341) => wff_selection(303).
		q(nt(341), (nt(303)));
		//       wff_pnf_cmd(268)     => pnf_cmd_sym(331) __(13) _Rwff_pnf_cmd_74(341).
		q(nt(268), (nt(331)+nt(13)+nt(341)));
		//       _Rwff_mnf_cmd_75(342) => wff(113).
		q(nt(342), (nt(113)));
		//       _Rwff_mnf_cmd_75(342) => output(302).
		q(nt(342), (nt(302)));
		//       _Rwff_mnf_cmd_75(342) => wff_selection(303).
		q(nt(342), (nt(303)));
		//       wff_mnf_cmd(269)     => mnf_cmd_sym(333) __(13) _Rwff_mnf_cmd_75(342).
		q(nt(269), (nt(333)+nt(13)+nt(342)));
		//       _Rhelp_cmd_76(346)   => null.
		q(nt(346), (nul));
		//       _Rhelp_cmd_76(346)   => __(13) cli_cmd_sym(345).
		q(nt(346), (nt(13)+nt(345)));
		//       help_cmd(343)        => help_cmd_sym(344) _Rhelp_cmd_76(346).
		q(nt(343), (nt(344)+nt(346)));
		//       version_cmd(347)     => version_cmd_sym(348).
		q(nt(347), (nt(348)));
		//       quit_cmd(349)        => quit_cmd_sym(350).
		q(nt(349), (nt(350)));
		//       wff_selection(303)   => selection_sym(351) __(13) digits(68).
		q(nt(303), (nt(351)+nt(13)+nt(68)));
		//       bf_selection(307)    => selection_sym(351) __(13) digits(68).
		q(nt(307), (nt(351)+nt(13)+nt(68)));
		//       bf_var_selection(352) => selection_sym(351) __(13) digits(68).
		q(nt(352), (nt(351)+nt(13)+nt(68)));
		//       wff_var_selection(353) => selection_sym(351) __(13) digits(68).
		q(nt(353), (nt(351)+nt(13)+nt(68)));
		//       file_path(299)       => chars(65).
		q(nt(299), (nt(65)));
		//       output(302)          => absolute_output(354).
		q(nt(302), (nt(354)));
		//       output(302)          => relative_output(355).
		q(nt(302), (nt(355)));
		//       _Rabsolute_output_77(358) => absolute_output_sym(356).
		q(nt(358), (nt(356)));
		//       _Rabsolute_output_77(358) => output_sym(357) __(13).
		q(nt(358), (nt(357)+nt(13)));
		//       absolute_output(354) => _Rabsolute_output_77(358) output_id(359).
		q(nt(354), (nt(358)+nt(359)));
		//       relative_output(355) => relative_output_sym(360) output_id(359).
		q(nt(355), (nt(360)+nt(359)));
		//       output_id(359)       => digits(68).
		q(nt(359), (nt(68)));
		//       list_outputs_cmd(361) => absolute_output_sym(356).
		q(nt(361), (nt(356)));
		//       list_outputs_cmd(361) => output_sym(357).
		q(nt(361), (nt(357)));
		//       list_outputs_cmd(361) => relative_output_sym(360).
		q(nt(361), (nt(360)));
		//       _Rclear_outputs_cmd_78(363) => absolute_output_sym(356).
		q(nt(363), (nt(356)));
		//       _Rclear_outputs_cmd_78(363) => output_sym(357) __(13).
		q(nt(363), (nt(357)+nt(13)));
		//       _Rclear_outputs_cmd_78(363) => relative_output_sym(360).
		q(nt(363), (nt(360)));
		//       clear_outputs_cmd(362) => _Rclear_outputs_cmd_78(363) clear_sym(364).
		q(nt(362), (nt(363)+nt(364)));
		//       print_output_cmd(365) => output(302).
		q(nt(365), (nt(302)));
		//       cli_cmd_sym(345)     => normalize_cmd_sym(301).
		q(nt(345), (nt(301)));
		//       cli_cmd_sym(345)     => instantiate_cmd_sym(306).
		q(nt(345), (nt(306)));
		//       cli_cmd_sym(345)     => substitute_cmd_sym(314).
		q(nt(345), (nt(314)));
		//       cli_cmd_sym(345)     => help_cmd_sym(344).
		q(nt(345), (nt(344)));
		//       cli_cmd_sym(345)     => version_cmd_sym(348).
		q(nt(345), (nt(348)));
		//       cli_cmd_sym(345)     => quit_cmd_sym(350).
		q(nt(345), (nt(350)));
		//       cli_cmd_sym(345)     => selection_sym(351).
		q(nt(345), (nt(351)));
		//       cli_cmd_sym(345)     => absolute_output_sym(356).
		q(nt(345), (nt(356)));
		//       cli_cmd_sym(345)     => output_sym(357).
		q(nt(345), (nt(357)));
		//       cli_cmd_sym(345)     => relative_output_sym(360).
		q(nt(345), (nt(360)));
		//       cli_cmd_sym(345)     => file_cmd_sym(366).
		q(nt(345), (nt(366)));
		//       cli_cmd_sym(345)     => get_cmd_sym(367).
		q(nt(345), (nt(367)));
		//       cli_cmd_sym(345)     => set_cmd_sym(368).
		q(nt(345), (nt(368)));
		//       cli_cmd_sym(345)     => toggle_cmd_sym(369).
		q(nt(345), (nt(369)));
		//       help_cmd_sym(344)    => 'h'.
		q(nt(344), (t(42)));
		//       help_cmd_sym(344)    => 'h' 'e' 'l' 'p'.
		q(nt(344), (t(42)+t(28)+t(27)+t(56)));
		//       version_cmd_sym(348) => 'v'.
		q(nt(348), (t(51)));
		//       version_cmd_sym(348) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
		q(nt(348), (t(51)+t(28)+t(45)+t(43)+t(39)+t(41)+t(49)));
		//       quit_cmd_sym(350)    => 'e'.
		q(nt(350), (t(28)));
		//       quit_cmd_sym(350)    => 'e' 'x' 'i' 't'.
		q(nt(350), (t(28)+t(29)+t(39)+t(52)));
		//       quit_cmd_sym(350)    => 'q'.
		q(nt(350), (t(54)));
		//       quit_cmd_sym(350)    => 'q' 'u' 'i' 't'.
		q(nt(350), (t(54)+t(44)+t(39)+t(52)));
		//       instantiate_sym(370) => 'i'.
		q(nt(370), (t(39)));
		//       instantiate_sym(370) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
		q(nt(370), (t(39)+t(49)+t(43)+t(52)+t(26)+t(49)+t(52)+t(39)+t(26)+t(52)+t(28)));
		//       substitute_sym(371)  => 's'.
		q(nt(371), (t(43)));
		//       substitute_sym(371)  => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
		q(nt(371), (t(43)+t(44)+t(30)+t(43)+t(52)+t(39)+t(52)+t(44)+t(52)+t(28)));
		//       normalize_cmd_sym(301) => 'n'.
		q(nt(301), (t(49)));
		//       normalize_cmd_sym(301) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
		q(nt(301), (t(49)+t(41)+t(45)+t(46)+t(26)+t(27)+t(39)+t(55)+t(28)));
		//       file_cmd_sym(366)    => 'r'.
		q(nt(366), (t(45)));
		//       file_cmd_sym(366)    => 'r' 'e' 'a' 'd'.
		q(nt(366), (t(45)+t(28)+t(26)+t(53)));
		//       clear_cmd_sym(372)   => 'c'.
		q(nt(372), (t(47)));
		//       clear_cmd_sym(372)   => 'c' 'l' 'e' 'a' 'r'.
		q(nt(372), (t(47)+t(27)+t(28)+t(26)+t(45)));
		//       onf_cmd_sym(335)     => 'o' 'n' 'f'.
		q(nt(335), (t(41)+t(49)+t(34)));
		//       dnf_cmd_sym(323)     => 'd' 'n' 'f'.
		q(nt(323), (t(53)+t(49)+t(34)));
		//       cnf_cmd_sym(325)     => 'c' 'n' 'f'.
		q(nt(325), (t(47)+t(49)+t(34)));
		//       anf_cmd_sym(327)     => 'a' 'n' 'f'.
		q(nt(327), (t(26)+t(49)+t(34)));
		//       nnf_cmd_sym(329)     => 'n' 'n' 'f'.
		q(nt(329), (t(49)+t(49)+t(34)));
		//       pnf_cmd_sym(331)     => 'p' 'n' 'f'.
		q(nt(331), (t(56)+t(49)+t(34)));
		//       mnf_cmd_sym(333)     => 'm' 'n' 'f'.
		q(nt(333), (t(46)+t(49)+t(34)));
		//       selection_sym(351)   => 's'.
		q(nt(351), (t(43)));
		//       selection_sym(351)   => 's' 'e' 'l' 'e' 'c' 't' 'i' 'o' 'n'.
		q(nt(351), (t(43)+t(28)+t(27)+t(28)+t(47)+t(52)+t(39)+t(41)+t(49)));
		//       output_sym(357)      => 'o'.
		q(nt(357), (t(41)));
		//       output_sym(357)      => 'o' 'u' 't' 'p' 'u' 't'.
		q(nt(357), (t(41)+t(44)+t(52)+t(56)+t(44)+t(52)));
		//       absolute_output_sym(356) => '&'.
		q(nt(356), (t(20)));
		//       relative_output_sym(360) => '%'.
		q(nt(360), (t(57)));
		//       _Rget_cmd_79(375)    => null.
		q(nt(375), (nul));
		//       _Rget_cmd_79(375)    => __(13) option(374).
		q(nt(375), (nt(13)+nt(374)));
		//       get_cmd(373)         => get_cmd_sym(367) _Rget_cmd_79(375).
		q(nt(373), (nt(367)+nt(375)));
		//       _Rset_cmd_80(377)    => __(13).
		q(nt(377), (nt(13)));
		//       _Rset_cmd_80(377)    => _(15) '=' _(15).
		q(nt(377), (nt(15)+t(19)+nt(15)));
		//       set_cmd(376)         => set_cmd_sym(368) __(13) option(374) _Rset_cmd_80(377) option_value(378).
		q(nt(376), (nt(368)+nt(13)+nt(374)+nt(377)+nt(378)));
		//       toggle_cmd(379)      => toggle_cmd_sym(369) __(13) bool_option(380).
		q(nt(379), (nt(369)+nt(13)+nt(380)));
		//       get_cmd_sym(367)     => 'g' 'e' 't'.
		q(nt(367), (t(50)+t(28)+t(52)));
		//       set_cmd_sym(368)     => 's' 'e' 't'.
		q(nt(368), (t(43)+t(28)+t(52)));
		//       toggle_cmd_sym(369)  => 't' 'o' 'g' 'g' 'l' 'e'.
		q(nt(369), (t(52)+t(41)+t(50)+t(50)+t(27)+t(28)));
		//       option(374)          => bool_option(380).
		q(nt(374), (nt(380)));
		//       option(374)          => severity_opt(381).
		q(nt(374), (nt(381)));
		//       bool_option(380)     => status_opt(382).
		q(nt(380), (nt(382)));
		//       bool_option(380)     => colors_opt(383).
		q(nt(380), (nt(383)));
		//       status_opt(382)      => 's'.
		q(nt(382), (t(43)));
		//       status_opt(382)      => 's' 't' 'a' 't' 'u' 's'.
		q(nt(382), (t(43)+t(52)+t(26)+t(52)+t(44)+t(43)));
		//       colors_opt(383)      => 'c'.
		q(nt(383), (t(47)));
		//       colors_opt(383)      => 'c' 'o' 'l' 'o' 'r'.
		q(nt(383), (t(47)+t(41)+t(27)+t(41)+t(45)));
		//       colors_opt(383)      => 'c' 'o' 'l' 'o' 'r' 's'.
		q(nt(383), (t(47)+t(41)+t(27)+t(41)+t(45)+t(43)));
		//       severity_opt(381)    => 's' 'e' 'v'.
		q(nt(381), (t(43)+t(28)+t(51)));
		//       severity_opt(381)    => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
		q(nt(381), (t(43)+t(28)+t(51)+t(28)+t(45)+t(39)+t(52)+t(58)));
		//       option_value(378)    => option_value_true(384).
		q(nt(378), (nt(384)));
		//       option_value(378)    => option_value_false(385).
		q(nt(378), (nt(385)));
		//       option_value(378)    => severity(386).
		q(nt(378), (nt(386)));
		//       option_value_true(384) => '1'.
		q(nt(384), (t(36)));
		//       option_value_true(384) => 'o' 'n'.
		q(nt(384), (t(41)+t(49)));
		//       option_value_true(384) => 't'.
		q(nt(384), (t(52)));
		//       option_value_true(384) => 't' 'r' 'u' 'e'.
		q(nt(384), (t(52)+t(45)+t(44)+t(28)));
		//       option_value_true(384) => 'y'.
		q(nt(384), (t(58)));
		//       option_value_true(384) => 'y' 'e' 's'.
		q(nt(384), (t(58)+t(28)+t(43)));
		//       option_value_false(385) => '0'.
		q(nt(385), (t(37)));
		//       option_value_false(385) => 'f'.
		q(nt(385), (t(34)));
		//       option_value_false(385) => 'f' 'a' 'l' 's' 'e'.
		q(nt(385), (t(34)+t(26)+t(27)+t(43)+t(28)));
		//       option_value_false(385) => 'n'.
		q(nt(385), (t(49)));
		//       option_value_false(385) => 'n' 'o'.
		q(nt(385), (t(49)+t(41)));
		//       option_value_false(385) => 'o' 'f' 'f'.
		q(nt(385), (t(41)+t(34)+t(34)));
		//       severity(386)        => error_sym(387).
		q(nt(386), (nt(387)));
		//       severity(386)        => debug_sym(388).
		q(nt(386), (nt(388)));
		//       severity(386)        => trace_sym(389).
		q(nt(386), (nt(389)));
		//       severity(386)        => info_sym(390).
		q(nt(386), (nt(390)));
		//       error_sym(387)       => 'e'.
		q(nt(387), (t(28)));
		//       error_sym(387)       => 'e' 'r' 'r' 'o' 'r'.
		q(nt(387), (t(28)+t(45)+t(45)+t(41)+t(45)));
		//       info_sym(390)        => 'i'.
		q(nt(390), (t(39)));
		//       info_sym(390)        => 'i' 'n' 'f' 'o'.
		q(nt(390), (t(39)+t(49)+t(34)+t(41)));
		//       debug_sym(388)       => 'd'.
		q(nt(388), (t(53)));
		//       debug_sym(388)       => 'd' 'e' 'b' 'u' 'g'.
		q(nt(388), (t(53)+t(28)+t(30)+t(44)+t(50)));
		//       trace_sym(389)       => 't'.
		q(nt(389), (t(52)));
		//       trace_sym(389)       => 't' 'r' 'a' 'c' 'e'.
		q(nt(389), (t(52)+t(45)+t(26)+t(47)+t(28)));
		return q;
	}
};

#endif // __TAU_PARSER_H__
