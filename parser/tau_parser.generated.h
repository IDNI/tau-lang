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
	using sptree_type     = parser_type::psptree;
	using input_type      = parser_type::input;
	using decoder_type    = parser_type::input::decoder_type;
	using encoder_type    = std::function<std::basic_string<char_type>(
			const std::vector<terminal_type>&)>;
	tau_parser() :
		nts(load_nonterminals()), cc(load_cc()),
		g(nts, load_prods(), nt(243), cc, load_grammar_opts()),
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
	sptree_type parse_and_shape(forest_type* f) {
		idni::tree_shaping_options opt;
		opt.to_trim = g.opt.to_trim;
		opt.to_trim_children = g.opt.to_trim_children;
		opt.trim_terminals = g.opt.trim_terminals;
		opt.to_inline = g.opt.to_inline;
		opt.inline_char_classes = g.opt.inline_char_classes;
		return f->get_shaped_tree(opt);
	}
	sptree_type parse_and_shape(const char_type* data, size_t size,
		parse_options po = {}) {
			return parse_and_shape(p.parse(data, size, po).get()); }
	sptree_type parse_and_shape(std::basic_istream<char_type>& is,
		parse_options po = {}) {
				return parse_and_shape(p.parse(is, po).get()); }
	sptree_type parse_and_shape(const std::string& fn,
		parse_options po = {}) {
				return parse_and_shape(p.parse(fn, po).get()); }
#ifndef WIN32
	sptree_type parse_and_shape(int fd, parse_options po = {})
		{ return parse_and_shape(p.parse(fd, po).get()); }
#endif //WIN32
	bool found(size_t start = SIZE_MAX) { return p.found(start); }
	typename parser_type::error get_error() { return p.get_error(); }
	enum nonterminal {
		nul, eof, space, digit, xdigit, alpha, alnum, punct, printable, comment, 
		__E_comment_0, __E_comment_1, __E_comment_2, __, __E____3, _, __E___4, dot, open_parenthesis, close_parenthesis, 
		open_bracket, close_bracket, open_brace, close_brace, minus, colon, semicolon, less, comma, apostrophe, 
		quote, tau_def, tau_and_sym, tau_or_sym, tau_neg_sym, wff_def, wff_and_sym, wff_or_sym, wff_xor_sym, wff_conditional_sym, 
		wff_neg_sym, wff_imply_sym, wff_equiv_sym, wff_all_sym, wff_ex_sym, wff_ball_sym, wff_bex_sym, wff_t, wff_f, bf_def, 
		bf_and_sym, bf_or_sym, bf_xor_sym, bf_neg_sym, bf_equality_sym, bf_nequality_sym, bf_less_sym, bf_less_equal_sym, bf_not_less_equal_sym, bf_greater_sym, 
		bf_all_sym, bf_ex_sym, bf_splitter_sym, bf_t, bf_f, chars, __E_chars_5, __E_chars_6, digits, __E_digits_7, 
		sym, offsets, offset, __E_offsets_8, __E_offsets_9, num, capture, shift, __E_offset_10, variable, 
		io_var, __E_shift_11, __E___E_shift_11_12, charvar, in, out, in_var_name, out_var_name, bool_variable, __E_charvar_13, 
		__E_charvar_14, __E_charvar_15, capture_var, var, tau_rule, tau_matcher, tau_body, tau, tau_collapse_positives_cb, tau_positives_upwards_cb, 
		tau_rec_relation, tau_ref, __E_tau_ref_16, tau_ref_args, __E_tau_ref_args_17, tau_ref_arg, __E_tau_ref_args_18, bf, tau_or, tau_and, 
		tau_neg, tau_wff, wff, wff_rule, wff_matcher, wff_body, bf_eq_cb, bf_neq_cb, wff_has_clashing_subformulas_cb, wff_has_subformula_cb, 
		wff_remove_existential_cb, wff_remove_bexistential_cb, wff_remove_buniversal_cb, wff_rec_relation, wff_ref, wff_conditional, wff_ball, wff_bex, wff_all, wff_ex, 
		wff_imply, wff_equiv, bf_interval, bf_neq, bf_eq, bf_not_less_equal, bf_greater, bf_less_equal, bf_less, wff_or, 
		wff_and, wff_xor, wff_neg, __E_wff_ref_19, wff_ref_args, __E_wff_ref_args_20, wff_ref_arg, __E_wff_ref_args_21, __E_wff_all_22, __E_wff_ex_23, 
		__E_wff_ball_24, __E_wff_bex_25, bf_rule, bf_matcher, bf_body, bf_is_zero_cb, bf_is_one_cb, bf_has_subformula_cb, bf_remove_funiversal_cb, bf_remove_fexistential_cb, 
		bf_and_cb, bf_or_cb, bf_xor_cb, bf_neg_cb, bf_splitter, bf_ref, bf_all, bf_ex, bf_or, bf_and, 
		bf_xor, bf_neg, bf_constant, bf_rec_relation, __E_bf_ref_26, bf_ref_args, __E_bf_ref_args_27, bf_ref_arg, __E_bf_ref_args_28, __E_bf_and_29, 
		__E_bf_all_30, __E_bf_ex_31, constant, binding, source_binding, named_binding, type, source, source0, __E_source_32, 
		__E_source_33, bf_and_cb_sym, bf_cb_arg, bf_or_cb_sym, bf_xor_cb_sym, bf_neg_cb_sym, bf_eq_cb_sym, wff_cb_arg, bf_neq_cb_sym, bf_is_zero_cb_sym, 
		bf_is_one_cb_sym, bf_remove_funiversal_cb_sym, bf_remove_fexistential_cb_sym, wff_remove_existential_cb_sym, wff_remove_bexistential_cb_sym, wff_remove_buniversal_cb_sym, wff_has_clashing_subformulas_cb_sym, bf_has_subformula_cb_sym, wff_has_subformula_cb_sym, __E_tau_collapse_positives_cb_34, 
		tau_collapse_positives_cb_sym, tau_cb_arg, __E_tau_collapse_positives_cb_35, tau_positives_upwards_cb_sym, inputs, input, __E_inputs_36, __E_inputs_37, builder, builder_head, 
		builder_body, __E_builder_head_38, __E_builder_head_39, bf_builder_body, wff_builder_body, tau_builder_body, library, rules, __E_rules_40, rule, 
		__E_rules_41, nso_rr, nso_rec_relations, nso_main, __E_nso_rec_relations_42, nso_rec_relation, __E_nso_rec_relations_43, gssotc_rr, gssotc_rec_relations, gssotc_main, 
		__E_gssotc_rec_relations_44, gssotc_rec_relation, __E_gssotc_rec_relations_45, start, __E_start_46, cli, cli_command, __E_cli_47, __E_cli_48, help_cmd, 
		version_cmd, quit_cmd, clear_cmd, get_cmd, set_cmd, toggle_cmd, list_outputs_cmd, clear_outputs_cmd, print_output_cmd, file, 
		normalize_cmd, wff_onf_cmd, wff_dnf_cmd, wff_cnf_cmd, wff_anf_cmd, wff_nnf_cmd, wff_pnf_cmd, wff_mnf_cmd, bf_dnf_cmd, bf_cnf_cmd, 
		bf_anf_cmd, bf_nnf_cmd, bf_pnf_cmd, bf_mnf_cmd, bf_substitute, wff_substitute, bf_instantiate, wff_instantiate, file_sym, file_path, 
		normalize_cmd_sym, normalize_cmd_arg, output, wff_selection, bf_instantiate_cmd, instantiate_cmd_sym, __E_bf_instantiate_cmd_49, bf_selection, __E_bf_instantiate_cmd_50, wff_instantiate_cmd, 
		__E_wff_instantiate_cmd_51, __E_wff_instantiate_cmd_52, bf_substitute_cmd, substitute_cmd_sym, __E_bf_substitute_cmd_53, __E_bf_substitute_cmd_54, __E_bf_substitute_cmd_55, wff_substitute_cmd, substitutecmd__sym, __E_wff_substitute_cmd_56, 
		__E_wff_substitute_cmd_57, __E_wff_substitute_cmd_58, dnf_cmd_sym, __E_bf_dnf_cmd_59, cnf_cmd_sym, __E_bf_cnf_cmd_60, anf_cmd_sym, __E_bf_anf_cmd_61, nnf_cmd_sym, __E_bf_nnf_cmd_62, 
		pnf_cmd_sym, __E_bf_pnf_cmd_63, mnf_cmd_sym, __E_bf_mnf_cmd_64, onf_cmd_sym, __E_wff_onf_cmd_65, __E_wff_dnf_cmd_66, __E_wff_cnf_cmd_67, __E_wff_anf_cmd_68, __E_wff_nnf_cmd_69, 
		__E_wff_pnf_cmd_70, __E_wff_mnf_cmd_71, help_cmd_sym, __E_help_cmd_72, cli_cmd_sym, version_cmd_sym, quit_cmd_sym, clear_cmd_sym, selection_sym, bf_var_selection, 
		wff_var_selection, absolute_output, relative_output, __E_absolute_output_73, absolute_output_sym, output_sym, output_id, relative_output_sym, __E_clear_outputs_cmd_74, clear_sym, 
		file_cmd_sym, get_cmd_sym, set_cmd_sym, toggle_cmd_sym, instantiate_sym, substitute_sym, __E_get_cmd_75, option, __E_set_cmd_76, option_value, 
		bool_option, severity_opt, status_opt, colors_opt, debug_repl_opt, option_value_true, option_value_false, severity, error_sym, debug_sym, 
		trace_sym, info_sym, __N_0, __N_1, __N_2, __N_3, 
	};
	size_t id(const std::basic_string<char_type>& name) {
		return nts.get(name);
	}
	const std::basic_string<char_type>& name(size_t id) {
		return nts.get(id);
	}
	symbol_type literal(const nonterminal& nt) {
		return symbol_type(nt, &nts);
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
			"", "eof", "space", "digit", "xdigit", "alpha", "alnum", "punct", "printable", "comment", 
			"__E_comment_0", "__E_comment_1", "__E_comment_2", "__", "__E____3", "_", "__E___4", "dot", "open_parenthesis", "close_parenthesis", 
			"open_bracket", "close_bracket", "open_brace", "close_brace", "minus", "colon", "semicolon", "less", "comma", "apostrophe", 
			"quote", "tau_def", "tau_and_sym", "tau_or_sym", "tau_neg_sym", "wff_def", "wff_and_sym", "wff_or_sym", "wff_xor_sym", "wff_conditional_sym", 
			"wff_neg_sym", "wff_imply_sym", "wff_equiv_sym", "wff_all_sym", "wff_ex_sym", "wff_ball_sym", "wff_bex_sym", "wff_t", "wff_f", "bf_def", 
			"bf_and_sym", "bf_or_sym", "bf_xor_sym", "bf_neg_sym", "bf_equality_sym", "bf_nequality_sym", "bf_less_sym", "bf_less_equal_sym", "bf_not_less_equal_sym", "bf_greater_sym", 
			"bf_all_sym", "bf_ex_sym", "bf_splitter_sym", "bf_t", "bf_f", "chars", "__E_chars_5", "__E_chars_6", "digits", "__E_digits_7", 
			"sym", "offsets", "offset", "__E_offsets_8", "__E_offsets_9", "num", "capture", "shift", "__E_offset_10", "variable", 
			"io_var", "__E_shift_11", "__E___E_shift_11_12", "charvar", "in", "out", "in_var_name", "out_var_name", "bool_variable", "__E_charvar_13", 
			"__E_charvar_14", "__E_charvar_15", "capture_var", "var", "tau_rule", "tau_matcher", "tau_body", "tau", "tau_collapse_positives_cb", "tau_positives_upwards_cb", 
			"tau_rec_relation", "tau_ref", "__E_tau_ref_16", "tau_ref_args", "__E_tau_ref_args_17", "tau_ref_arg", "__E_tau_ref_args_18", "bf", "tau_or", "tau_and", 
			"tau_neg", "tau_wff", "wff", "wff_rule", "wff_matcher", "wff_body", "bf_eq_cb", "bf_neq_cb", "wff_has_clashing_subformulas_cb", "wff_has_subformula_cb", 
			"wff_remove_existential_cb", "wff_remove_bexistential_cb", "wff_remove_buniversal_cb", "wff_rec_relation", "wff_ref", "wff_conditional", "wff_ball", "wff_bex", "wff_all", "wff_ex", 
			"wff_imply", "wff_equiv", "bf_interval", "bf_neq", "bf_eq", "bf_not_less_equal", "bf_greater", "bf_less_equal", "bf_less", "wff_or", 
			"wff_and", "wff_xor", "wff_neg", "__E_wff_ref_19", "wff_ref_args", "__E_wff_ref_args_20", "wff_ref_arg", "__E_wff_ref_args_21", "__E_wff_all_22", "__E_wff_ex_23", 
			"__E_wff_ball_24", "__E_wff_bex_25", "bf_rule", "bf_matcher", "bf_body", "bf_is_zero_cb", "bf_is_one_cb", "bf_has_subformula_cb", "bf_remove_funiversal_cb", "bf_remove_fexistential_cb", 
			"bf_and_cb", "bf_or_cb", "bf_xor_cb", "bf_neg_cb", "bf_splitter", "bf_ref", "bf_all", "bf_ex", "bf_or", "bf_and", 
			"bf_xor", "bf_neg", "bf_constant", "bf_rec_relation", "__E_bf_ref_26", "bf_ref_args", "__E_bf_ref_args_27", "bf_ref_arg", "__E_bf_ref_args_28", "__E_bf_and_29", 
			"__E_bf_all_30", "__E_bf_ex_31", "constant", "binding", "source_binding", "named_binding", "type", "source", "source0", "__E_source_32", 
			"__E_source_33", "bf_and_cb_sym", "bf_cb_arg", "bf_or_cb_sym", "bf_xor_cb_sym", "bf_neg_cb_sym", "bf_eq_cb_sym", "wff_cb_arg", "bf_neq_cb_sym", "bf_is_zero_cb_sym", 
			"bf_is_one_cb_sym", "bf_remove_funiversal_cb_sym", "bf_remove_fexistential_cb_sym", "wff_remove_existential_cb_sym", "wff_remove_bexistential_cb_sym", "wff_remove_buniversal_cb_sym", "wff_has_clashing_subformulas_cb_sym", "bf_has_subformula_cb_sym", "wff_has_subformula_cb_sym", "__E_tau_collapse_positives_cb_34", 
			"tau_collapse_positives_cb_sym", "tau_cb_arg", "__E_tau_collapse_positives_cb_35", "tau_positives_upwards_cb_sym", "inputs", "input", "__E_inputs_36", "__E_inputs_37", "builder", "builder_head", 
			"builder_body", "__E_builder_head_38", "__E_builder_head_39", "bf_builder_body", "wff_builder_body", "tau_builder_body", "library", "rules", "__E_rules_40", "rule", 
			"__E_rules_41", "nso_rr", "nso_rec_relations", "nso_main", "__E_nso_rec_relations_42", "nso_rec_relation", "__E_nso_rec_relations_43", "gssotc_rr", "gssotc_rec_relations", "gssotc_main", 
			"__E_gssotc_rec_relations_44", "gssotc_rec_relation", "__E_gssotc_rec_relations_45", "start", "__E_start_46", "cli", "cli_command", "__E_cli_47", "__E_cli_48", "help_cmd", 
			"version_cmd", "quit_cmd", "clear_cmd", "get_cmd", "set_cmd", "toggle_cmd", "list_outputs_cmd", "clear_outputs_cmd", "print_output_cmd", "file", 
			"normalize_cmd", "wff_onf_cmd", "wff_dnf_cmd", "wff_cnf_cmd", "wff_anf_cmd", "wff_nnf_cmd", "wff_pnf_cmd", "wff_mnf_cmd", "bf_dnf_cmd", "bf_cnf_cmd", 
			"bf_anf_cmd", "bf_nnf_cmd", "bf_pnf_cmd", "bf_mnf_cmd", "bf_substitute", "wff_substitute", "bf_instantiate", "wff_instantiate", "file_sym", "file_path", 
			"normalize_cmd_sym", "normalize_cmd_arg", "output", "wff_selection", "bf_instantiate_cmd", "instantiate_cmd_sym", "__E_bf_instantiate_cmd_49", "bf_selection", "__E_bf_instantiate_cmd_50", "wff_instantiate_cmd", 
			"__E_wff_instantiate_cmd_51", "__E_wff_instantiate_cmd_52", "bf_substitute_cmd", "substitute_cmd_sym", "__E_bf_substitute_cmd_53", "__E_bf_substitute_cmd_54", "__E_bf_substitute_cmd_55", "wff_substitute_cmd", "substitutecmd__sym", "__E_wff_substitute_cmd_56", 
			"__E_wff_substitute_cmd_57", "__E_wff_substitute_cmd_58", "dnf_cmd_sym", "__E_bf_dnf_cmd_59", "cnf_cmd_sym", "__E_bf_cnf_cmd_60", "anf_cmd_sym", "__E_bf_anf_cmd_61", "nnf_cmd_sym", "__E_bf_nnf_cmd_62", 
			"pnf_cmd_sym", "__E_bf_pnf_cmd_63", "mnf_cmd_sym", "__E_bf_mnf_cmd_64", "onf_cmd_sym", "__E_wff_onf_cmd_65", "__E_wff_dnf_cmd_66", "__E_wff_cnf_cmd_67", "__E_wff_anf_cmd_68", "__E_wff_nnf_cmd_69", 
			"__E_wff_pnf_cmd_70", "__E_wff_mnf_cmd_71", "help_cmd_sym", "__E_help_cmd_72", "cli_cmd_sym", "version_cmd_sym", "quit_cmd_sym", "clear_cmd_sym", "selection_sym", "bf_var_selection", 
			"wff_var_selection", "absolute_output", "relative_output", "__E_absolute_output_73", "absolute_output_sym", "output_sym", "output_id", "relative_output_sym", "__E_clear_outputs_cmd_74", "clear_sym", 
			"file_cmd_sym", "get_cmd_sym", "set_cmd_sym", "toggle_cmd_sym", "instantiate_sym", "substitute_sym", "__E_get_cmd_75", "option", "__E_set_cmd_76", "option_value", 
			"bool_option", "severity_opt", "status_opt", "colors_opt", "debug_repl_opt", "option_value_true", "option_value_false", "severity", "error_sym", "debug_sym", 
			"trace_sym", "info_sym", "__N_0", "__N_1", "__N_2", "__N_3", 
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
	grammar_type::options load_grammar_opts() {
		grammar_type::options o;
		o.transform_negation = false;
		o.trim_terminals = false;
		o.inline_char_classes = false;
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
		//       __E_comment_0(10)    => printable(8).
		q(nt(10), (nt(8)));
		//       __E_comment_0(10)    => '\t'.
		q(nt(10), (t(1)));
		//       __E_comment_1(11)    => null.
		q(nt(11), (nul));
		//       __E_comment_1(11)    => __E_comment_0(10) __E_comment_1(11).
		q(nt(11), (nt(10)+nt(11)));
		//       __E_comment_2(12)    => '\n'.
		q(nt(12), (t(2)));
		//       __E_comment_2(12)    => '\r'.
		q(nt(12), (t(3)));
		//       __E_comment_2(12)    => eof(1).
		q(nt(12), (nt(1)));
		//       comment(9)           => '#' __E_comment_1(11) __E_comment_2(12).
		q(nt(9), (t(4)+nt(11)+nt(12)));
		//       __E____3(14)         => space(2).
		q(nt(14), (nt(2)));
		//       __E____3(14)         => comment(9).
		q(nt(14), (nt(9)));
		//       __(13)               => __E____3(14) _(15).
		q(nt(13), (nt(14)+nt(15)));
		//       __E___4(16)          => __(13).
		q(nt(16), (nt(13)));
		//       __E___4(16)          => null.
		q(nt(16), (nul));
		//       _(15)                => __E___4(16).
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
		//       __E_chars_5(66)      => alnum(6).
		q(nt(66), (nt(6)));
		//       __E_chars_6(67)      => null.
		q(nt(67), (nul));
		//       __E_chars_6(67)      => __E_chars_5(66) __E_chars_6(67).
		q(nt(67), (nt(66)+nt(67)));
		//       chars(65)            => alpha(5) __E_chars_6(67).
		q(nt(65), (nt(5)+nt(67)));
		//       __E_digits_7(69)     => digit(3).
		q(nt(69), (nt(3)));
		//       __E_digits_7(69)     => digit(3) __E_digits_7(69).
		q(nt(69), (nt(3)+nt(69)));
		//       digits(68)           => __E_digits_7(69).
		q(nt(68), (nt(69)));
		//       sym(70)              => chars(65).
		q(nt(70), (nt(65)));
		//       __E_offsets_8(73)    => _(15) comma(28) _(15) offset(72).
		q(nt(73), (nt(15)+nt(28)+nt(15)+nt(72)));
		//       __E_offsets_9(74)    => null.
		q(nt(74), (nul));
		//       __E_offsets_9(74)    => __E_offsets_8(73) __E_offsets_9(74).
		q(nt(74), (nt(73)+nt(74)));
		//       offsets(71)          => open_bracket(20) _(15) offset(72) __E_offsets_9(74) _(15) close_bracket(21).
		q(nt(71), (nt(20)+nt(15)+nt(72)+nt(74)+nt(15)+nt(21)));
		//       offset(72)           => num(75).
		q(nt(72), (nt(75)));
		//       offset(72)           => capture(76).
		q(nt(72), (nt(76)));
		//       offset(72)           => shift(77).
		q(nt(72), (nt(77)));
		//       __N_0(362)           => io_var(80).
		q(nt(362), (nt(80)));
		//       __E_offset_10(78)    => variable(79) & ~( __N_0(362) ).	 # conjunctive
		q(nt(78), (nt(79)) & ~(nt(362)));
		//       offset(72)           => __E_offset_10(78).
		q(nt(72), (nt(78)));
		//       __E_shift_11(81)     => capture(76).
		q(nt(81), (nt(76)));
		//       __N_1(363)           => io_var(80).
		q(nt(363), (nt(80)));
		//       __E___E_shift_11_12(82) => variable(79) & ~( __N_1(363) ).	 # conjunctive
		q(nt(82), (nt(79)) & ~(nt(363)));
		//       __E_shift_11(81)     => __E___E_shift_11_12(82).
		q(nt(81), (nt(82)));
		//       shift(77)            => __E_shift_11(81) _(15) minus(24) _(15) num(75).
		q(nt(77), (nt(81)+nt(15)+nt(24)+nt(15)+nt(75)));
		//       num(75)              => digits(68).
		q(nt(75), (nt(68)));
		//       variable(79)         => charvar(83).
		q(nt(79), (nt(83)));
		//       variable(79)         => io_var(80).
		q(nt(79), (nt(80)));
		//       io_var(80)           => in(84).
		q(nt(80), (nt(84)));
		//       io_var(80)           => out(85).
		q(nt(80), (nt(85)));
		//       in(84)               => in_var_name(86) open_bracket(20) offset(72) close_bracket(21).
		q(nt(84), (nt(86)+nt(20)+nt(72)+nt(21)));
		//       out(85)              => out_var_name(87) open_bracket(20) offset(72) close_bracket(21).
		q(nt(85), (nt(87)+nt(20)+nt(72)+nt(21)));
		//       bool_variable(88)    => charvar(83).
		q(nt(88), (nt(83)));
		//       __N_2(364)           => 'F'.
		q(nt(364), (t(32)));
		//       __N_3(365)           => 'T'.
		q(nt(365), (t(31)));
		//       __E_charvar_13(89)   => ~( __N_2(364) ) & ~( __N_3(365) ) & alpha(5).	 # conjunctive
		q(nt(89), ~(nt(364)) & ~(nt(365)) & (nt(5)));
		//       __E_charvar_14(90)   => digit(3).
		q(nt(90), (nt(3)));
		//       __E_charvar_15(91)   => null.
		q(nt(91), (nul));
		//       __E_charvar_15(91)   => __E_charvar_14(90) __E_charvar_15(91).
		q(nt(91), (nt(90)+nt(91)));
		//       charvar(83)          => __E_charvar_13(89) __E_charvar_15(91).
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
		//       tau_rec_relation(100) => tau_ref(101) _(15) tau_def(31) _(15) tau(97) _(15) dot(17).
		q(nt(100), (nt(101)+nt(15)+nt(31)+nt(15)+nt(97)+nt(15)+nt(17)));
		//       __E_tau_ref_16(102)  => offsets(71).
		q(nt(102), (nt(71)));
		//       __E_tau_ref_16(102)  => null.
		q(nt(102), (nul));
		//       tau_ref(101)         => sym(70) _(15) __E_tau_ref_16(102) _(15) tau_ref_args(103).
		q(nt(101), (nt(70)+nt(15)+nt(102)+nt(15)+nt(103)));
		//       __E_tau_ref_args_17(104) => _(15) tau_ref_arg(105).
		q(nt(104), (nt(15)+nt(105)));
		//       __E_tau_ref_args_18(106) => null.
		q(nt(106), (nul));
		//       __E_tau_ref_args_18(106) => __E_tau_ref_args_17(104) __E_tau_ref_args_18(106).
		q(nt(106), (nt(104)+nt(106)));
		//       tau_ref_args(103)    => open_parenthesis(18) __E_tau_ref_args_18(106) _(15) close_parenthesis(19).
		q(nt(103), (nt(18)+nt(106)+nt(15)+nt(19)));
		//       tau_ref_arg(105)     => bf(107).
		q(nt(105), (nt(107)));
		//       tau_ref_arg(105)     => capture(76).
		q(nt(105), (nt(76)));
		//       tau_ref_arg(105)     => variable(79).
		q(nt(105), (nt(79)));
		//       tau(97)              => tau_ref(101).
		q(nt(97), (nt(101)));
		//       tau(97)              => tau_or(108).
		q(nt(97), (nt(108)));
		//       tau(97)              => tau_and(109).
		q(nt(97), (nt(109)));
		//       tau(97)              => tau_neg(110).
		q(nt(97), (nt(110)));
		//       tau(97)              => tau_wff(111).
		q(nt(97), (nt(111)));
		//       tau(97)              => capture(76).
		q(nt(97), (nt(76)));
		//       tau(97)              => open_parenthesis(18) _(15) tau(97) _(15) close_parenthesis(19).
		q(nt(97), (nt(18)+nt(15)+nt(97)+nt(15)+nt(19)));
		//       tau_and(109)         => tau(97) _(15) tau_and_sym(32) _(15) tau(97).
		q(nt(109), (nt(97)+nt(15)+nt(32)+nt(15)+nt(97)));
		//       tau_or(108)          => tau(97) _(15) tau_or_sym(33) _(15) tau(97).
		q(nt(108), (nt(97)+nt(15)+nt(33)+nt(15)+nt(97)));
		//       tau_neg(110)         => tau_neg_sym(34) _(15) tau(97).
		q(nt(110), (nt(34)+nt(15)+nt(97)));
		//       tau_wff(111)         => open_brace(22) _(15) wff(112) _(15) close_brace(23).
		q(nt(111), (nt(22)+nt(15)+nt(112)+nt(15)+nt(23)));
		//       wff_rule(113)        => wff_matcher(114) _(15) wff_def(35) _(15) wff_body(115) _(15) dot(17).
		q(nt(113), (nt(114)+nt(15)+nt(35)+nt(15)+nt(115)+nt(15)+nt(17)));
		//       wff_matcher(114)     => wff(112).
		q(nt(114), (nt(112)));
		//       wff_body(115)        => bf_eq_cb(116).
		q(nt(115), (nt(116)));
		//       wff_body(115)        => bf_neq_cb(117).
		q(nt(115), (nt(117)));
		//       wff_body(115)        => wff_has_clashing_subformulas_cb(118).
		q(nt(115), (nt(118)));
		//       wff_body(115)        => wff_has_subformula_cb(119).
		q(nt(115), (nt(119)));
		//       wff_body(115)        => wff_remove_existential_cb(120).
		q(nt(115), (nt(120)));
		//       wff_body(115)        => wff_remove_bexistential_cb(121).
		q(nt(115), (nt(121)));
		//       wff_body(115)        => wff_remove_buniversal_cb(122).
		q(nt(115), (nt(122)));
		//       wff_body(115)        => wff(112).
		q(nt(115), (nt(112)));
		//       wff_rec_relation(123) => wff_ref(124) _(15) wff_def(35) _(15) wff(112) _(15) dot(17).
		q(nt(123), (nt(124)+nt(15)+nt(35)+nt(15)+nt(112)+nt(15)+nt(17)));
		//       wff(112)             => wff_ref(124).
		q(nt(112), (nt(124)));
		//       wff(112)             => wff_conditional(125).
		q(nt(112), (nt(125)));
		//       wff(112)             => wff_ball(126).
		q(nt(112), (nt(126)));
		//       wff(112)             => wff_bex(127).
		q(nt(112), (nt(127)));
		//       wff(112)             => wff_all(128).
		q(nt(112), (nt(128)));
		//       wff(112)             => wff_ex(129).
		q(nt(112), (nt(129)));
		//       wff(112)             => wff_imply(130).
		q(nt(112), (nt(130)));
		//       wff(112)             => wff_equiv(131).
		q(nt(112), (nt(131)));
		//       wff(112)             => bf_interval(132).
		q(nt(112), (nt(132)));
		//       wff(112)             => bf_neq(133).
		q(nt(112), (nt(133)));
		//       wff(112)             => bf_eq(134).
		q(nt(112), (nt(134)));
		//       wff(112)             => bf_not_less_equal(135).
		q(nt(112), (nt(135)));
		//       wff(112)             => bf_greater(136).
		q(nt(112), (nt(136)));
		//       wff(112)             => bf_less_equal(137).
		q(nt(112), (nt(137)));
		//       wff(112)             => bf_less(138).
		q(nt(112), (nt(138)));
		//       wff(112)             => wff_or(139).
		q(nt(112), (nt(139)));
		//       wff(112)             => wff_and(140).
		q(nt(112), (nt(140)));
		//       wff(112)             => wff_xor(141).
		q(nt(112), (nt(141)));
		//       wff(112)             => wff_neg(142).
		q(nt(112), (nt(142)));
		//       wff(112)             => wff_t(47).
		q(nt(112), (nt(47)));
		//       wff(112)             => wff_f(48).
		q(nt(112), (nt(48)));
		//       wff(112)             => capture(76).
		q(nt(112), (nt(76)));
		//       wff(112)             => bool_variable(88).
		q(nt(112), (nt(88)));
		//       wff(112)             => open_parenthesis(18) _(15) wff(112) _(15) close_parenthesis(19).
		q(nt(112), (nt(18)+nt(15)+nt(112)+nt(15)+nt(19)));
		//       __E_wff_ref_19(143)  => offsets(71).
		q(nt(143), (nt(71)));
		//       __E_wff_ref_19(143)  => null.
		q(nt(143), (nul));
		//       wff_ref(124)         => sym(70) _(15) __E_wff_ref_19(143) _(15) wff_ref_args(144).
		q(nt(124), (nt(70)+nt(15)+nt(143)+nt(15)+nt(144)));
		//       __E_wff_ref_args_20(145) => _(15) wff_ref_arg(146).
		q(nt(145), (nt(15)+nt(146)));
		//       __E_wff_ref_args_21(147) => null.
		q(nt(147), (nul));
		//       __E_wff_ref_args_21(147) => __E_wff_ref_args_20(145) __E_wff_ref_args_21(147).
		q(nt(147), (nt(145)+nt(147)));
		//       wff_ref_args(144)    => open_parenthesis(18) __E_wff_ref_args_21(147) _(15) close_parenthesis(19).
		q(nt(144), (nt(18)+nt(147)+nt(15)+nt(19)));
		//       wff_ref_arg(146)     => bf(107).
		q(nt(146), (nt(107)));
		//       wff_ref_arg(146)     => capture(76).
		q(nt(146), (nt(76)));
		//       wff_ref_arg(146)     => variable(79).
		q(nt(146), (nt(79)));
		//       wff_and(140)         => wff(112) _(15) wff_and_sym(36) _(15) wff(112).
		q(nt(140), (nt(112)+nt(15)+nt(36)+nt(15)+nt(112)));
		//       wff_or(139)          => wff(112) _(15) wff_or_sym(37) _(15) wff(112).
		q(nt(139), (nt(112)+nt(15)+nt(37)+nt(15)+nt(112)));
		//       wff_xor(141)         => wff(112) _(15) wff_xor_sym(38) _(15) wff(112).
		q(nt(141), (nt(112)+nt(15)+nt(38)+nt(15)+nt(112)));
		//       wff_conditional(125) => wff(112) _(15) wff_conditional_sym(39) _(15) wff(112) _(15) colon(25) _(15) wff(112).
		q(nt(125), (nt(112)+nt(15)+nt(39)+nt(15)+nt(112)+nt(15)+nt(25)+nt(15)+nt(112)));
		//       wff_neg(142)         => wff_neg_sym(40) _(15) wff(112).
		q(nt(142), (nt(40)+nt(15)+nt(112)));
		//       wff_imply(130)       => wff(112) _(15) wff_imply_sym(41) _(15) wff(112).
		q(nt(130), (nt(112)+nt(15)+nt(41)+nt(15)+nt(112)));
		//       wff_equiv(131)       => wff(112) _(15) wff_equiv_sym(42) _(15) wff(112).
		q(nt(131), (nt(112)+nt(15)+nt(42)+nt(15)+nt(112)));
		//       __E_wff_all_22(148)  => variable(79).
		q(nt(148), (nt(79)));
		//       __E_wff_all_22(148)  => capture(76).
		q(nt(148), (nt(76)));
		//       wff_all(128)         => wff_all_sym(43) __(13) __E_wff_all_22(148) __(13) wff(112).
		q(nt(128), (nt(43)+nt(13)+nt(148)+nt(13)+nt(112)));
		//       __E_wff_ex_23(149)   => variable(79).
		q(nt(149), (nt(79)));
		//       __E_wff_ex_23(149)   => capture(76).
		q(nt(149), (nt(76)));
		//       wff_ex(129)          => wff_ex_sym(44) __(13) __E_wff_ex_23(149) __(13) wff(112).
		q(nt(129), (nt(44)+nt(13)+nt(149)+nt(13)+nt(112)));
		//       __E_wff_ball_24(150) => bool_variable(88).
		q(nt(150), (nt(88)));
		//       __E_wff_ball_24(150) => capture(76).
		q(nt(150), (nt(76)));
		//       wff_ball(126)        => wff_ball_sym(45) __(13) __E_wff_ball_24(150) __(13) wff(112).
		q(nt(126), (nt(45)+nt(13)+nt(150)+nt(13)+nt(112)));
		//       __E_wff_bex_25(151)  => bool_variable(88).
		q(nt(151), (nt(88)));
		//       __E_wff_bex_25(151)  => capture(76).
		q(nt(151), (nt(76)));
		//       wff_bex(127)         => wff_bex_sym(46) __(13) __E_wff_bex_25(151) __(13) wff(112).
		q(nt(127), (nt(46)+nt(13)+nt(151)+nt(13)+nt(112)));
		//       bf_eq(134)           => bf(107) _(15) bf_equality_sym(54) _(15) bf(107).
		q(nt(134), (nt(107)+nt(15)+nt(54)+nt(15)+nt(107)));
		//       bf_neq(133)          => bf(107) _(15) bf_nequality_sym(55) _(15) bf(107).
		q(nt(133), (nt(107)+nt(15)+nt(55)+nt(15)+nt(107)));
		//       bf_less(138)         => bf(107) _(15) bf_less_sym(56) _(15) bf(107).
		q(nt(138), (nt(107)+nt(15)+nt(56)+nt(15)+nt(107)));
		//       bf_less_equal(137)   => bf(107) _(15) bf_less_equal_sym(57) _(15) bf(107).
		q(nt(137), (nt(107)+nt(15)+nt(57)+nt(15)+nt(107)));
		//       bf_not_less_equal(135) => bf(107) _(15) bf_not_less_equal_sym(58) _(15) bf(107).
		q(nt(135), (nt(107)+nt(15)+nt(58)+nt(15)+nt(107)));
		//       bf_greater(136)      => bf(107) _(15) bf_greater_sym(59) _(15) bf(107).
		q(nt(136), (nt(107)+nt(15)+nt(59)+nt(15)+nt(107)));
		//       bf_interval(132)     => bf(107) _(15) bf_less_equal_sym(57) _(15) bf(107) _(15) bf_less_equal_sym(57) _(15) bf(107).
		q(nt(132), (nt(107)+nt(15)+nt(57)+nt(15)+nt(107)+nt(15)+nt(57)+nt(15)+nt(107)));
		//       bf_rule(152)         => bf_matcher(153) _(15) bf_def(49) _(15) bf_body(154) _(15) dot(17).
		q(nt(152), (nt(153)+nt(15)+nt(49)+nt(15)+nt(154)+nt(15)+nt(17)));
		//       bf_matcher(153)      => bf(107).
		q(nt(153), (nt(107)));
		//       bf_body(154)         => bf(107).
		q(nt(154), (nt(107)));
		//       bf_body(154)         => bf_is_zero_cb(155).
		q(nt(154), (nt(155)));
		//       bf_body(154)         => bf_is_one_cb(156).
		q(nt(154), (nt(156)));
		//       bf_body(154)         => bf_has_subformula_cb(157).
		q(nt(154), (nt(157)));
		//       bf_body(154)         => bf_remove_funiversal_cb(158).
		q(nt(154), (nt(158)));
		//       bf_body(154)         => bf_remove_fexistential_cb(159).
		q(nt(154), (nt(159)));
		//       bf_body(154)         => bf_and_cb(160).
		q(nt(154), (nt(160)));
		//       bf_body(154)         => bf_or_cb(161).
		q(nt(154), (nt(161)));
		//       bf_body(154)         => bf_xor_cb(162).
		q(nt(154), (nt(162)));
		//       bf_body(154)         => bf_neg_cb(163).
		q(nt(154), (nt(163)));
		//       bf_body(154)         => bf_not_less_equal(135).
		q(nt(154), (nt(135)));
		//       bf(107)              => bf_splitter(164).
		q(nt(107), (nt(164)));
		//       bf(107)              => bf_ref(165).
		q(nt(107), (nt(165)));
		//       bf(107)              => bf_all(166).
		q(nt(107), (nt(166)));
		//       bf(107)              => bf_ex(167).
		q(nt(107), (nt(167)));
		//       bf(107)              => bf_or(168).
		q(nt(107), (nt(168)));
		//       bf(107)              => bf_and(169).
		q(nt(107), (nt(169)));
		//       bf(107)              => bf_xor(170).
		q(nt(107), (nt(170)));
		//       bf(107)              => bf_neg(171).
		q(nt(107), (nt(171)));
		//       bf(107)              => bf_constant(172).
		q(nt(107), (nt(172)));
		//       bf(107)              => bf_t(63).
		q(nt(107), (nt(63)));
		//       bf(107)              => bf_f(64).
		q(nt(107), (nt(64)));
		//       bf(107)              => capture(76).
		q(nt(107), (nt(76)));
		//       bf(107)              => variable(79).
		q(nt(107), (nt(79)));
		//       bf(107)              => open_parenthesis(18) _(15) bf(107) _(15) close_parenthesis(19).
		q(nt(107), (nt(18)+nt(15)+nt(107)+nt(15)+nt(19)));
		//       bf_rec_relation(173) => bf_ref(165) _(15) bf_def(49) _(15) bf(107) _(15) dot(17).
		q(nt(173), (nt(165)+nt(15)+nt(49)+nt(15)+nt(107)+nt(15)+nt(17)));
		//       __E_bf_ref_26(174)   => offsets(71).
		q(nt(174), (nt(71)));
		//       __E_bf_ref_26(174)   => null.
		q(nt(174), (nul));
		//       bf_ref(165)          => sym(70) __E_bf_ref_26(174) bf_ref_args(175).
		q(nt(165), (nt(70)+nt(174)+nt(175)));
		//       __E_bf_ref_args_27(176) => _(15) bf_ref_arg(177).
		q(nt(176), (nt(15)+nt(177)));
		//       __E_bf_ref_args_28(178) => null.
		q(nt(178), (nul));
		//       __E_bf_ref_args_28(178) => __E_bf_ref_args_27(176) __E_bf_ref_args_28(178).
		q(nt(178), (nt(176)+nt(178)));
		//       bf_ref_args(175)     => open_parenthesis(18) __E_bf_ref_args_28(178) _(15) close_parenthesis(19).
		q(nt(175), (nt(18)+nt(178)+nt(15)+nt(19)));
		//       bf_ref_arg(177)      => bf(107).
		q(nt(177), (nt(107)));
		//       bf_ref_arg(177)      => capture(76).
		q(nt(177), (nt(76)));
		//       bf_ref_arg(177)      => variable(79).
		q(nt(177), (nt(79)));
		//       __E_bf_and_29(179)   => bf_and_sym(50) _(15).
		q(nt(179), (nt(50)+nt(15)));
		//       __E_bf_and_29(179)   => null.
		q(nt(179), (nul));
		//       bf_and(169)          => bf(107) _(15) __E_bf_and_29(179) bf(107).
		q(nt(169), (nt(107)+nt(15)+nt(179)+nt(107)));
		//       bf_or(168)           => bf(107) _(15) bf_or_sym(51) _(15) bf(107).
		q(nt(168), (nt(107)+nt(15)+nt(51)+nt(15)+nt(107)));
		//       bf_xor(170)          => bf(107) _(15) bf_xor_sym(52) _(15) bf(107).
		q(nt(170), (nt(107)+nt(15)+nt(52)+nt(15)+nt(107)));
		//       bf_neg(171)          => bf(107) _(15) bf_neg_sym(53).
		q(nt(171), (nt(107)+nt(15)+nt(53)));
		//       __E_bf_all_30(180)   => variable(79).
		q(nt(180), (nt(79)));
		//       __E_bf_all_30(180)   => capture(76).
		q(nt(180), (nt(76)));
		//       bf_all(166)          => bf_all_sym(60) __(13) __E_bf_all_30(180) __(13) bf(107).
		q(nt(166), (nt(60)+nt(13)+nt(180)+nt(13)+nt(107)));
		//       __E_bf_ex_31(181)    => variable(79).
		q(nt(181), (nt(79)));
		//       __E_bf_ex_31(181)    => capture(76).
		q(nt(181), (nt(76)));
		//       bf_ex(167)           => bf_ex_sym(61) __(13) __E_bf_ex_31(181) __(13) bf(107).
		q(nt(167), (nt(61)+nt(13)+nt(181)+nt(13)+nt(107)));
		//       bf_splitter(164)     => bf_splitter_sym(62) _(15) open_parenthesis(18) _(15) bf(107) _(15) close_parenthesis(19).
		q(nt(164), (nt(62)+nt(15)+nt(18)+nt(15)+nt(107)+nt(15)+nt(19)));
		//       bf_constant(172)     => open_brace(22) _(15) constant(182) _(15) close_brace(23).
		q(nt(172), (nt(22)+nt(15)+nt(182)+nt(15)+nt(23)));
		//       constant(182)        => binding(183).
		q(nt(182), (nt(183)));
		//       constant(182)        => capture(76).
		q(nt(182), (nt(76)));
		//       binding(183)         => source_binding(184).
		q(nt(183), (nt(184)));
		//       binding(183)         => named_binding(185).
		q(nt(183), (nt(185)));
		//       named_binding(185)   => chars(65).
		q(nt(185), (nt(65)));
		//       source_binding(184)  => type(186) _(15) colon(25) _(15) source(187).
		q(nt(184), (nt(186)+nt(15)+nt(25)+nt(15)+nt(187)));
		//       type(186)            => chars(65).
		q(nt(186), (nt(65)));
		//       type(186)            => null.
		q(nt(186), (nul));
		//       source0(188)         => alnum(6).
		q(nt(188), (nt(6)));
		//       source0(188)         => space(2).
		q(nt(188), (nt(2)));
		//       source0(188)         => punct(7).
		q(nt(188), (nt(7)));
		//       __E_source_32(189)   => source0(188).
		q(nt(189), (nt(188)));
		//       __E_source_33(190)   => __E_source_32(189).
		q(nt(190), (nt(189)));
		//       __E_source_33(190)   => __E_source_32(189) __E_source_33(190).
		q(nt(190), (nt(189)+nt(190)));
		//       source(187)          => __E_source_33(190).
		q(nt(187), (nt(190)));
		//       bf_and_cb(160)       => bf_and_cb_sym(191) __(13) bf_cb_arg(192) __(13) bf_cb_arg(192).
		q(nt(160), (nt(191)+nt(13)+nt(192)+nt(13)+nt(192)));
		//       bf_or_cb(161)        => bf_or_cb_sym(193) __(13) bf_cb_arg(192) __(13) bf_cb_arg(192).
		q(nt(161), (nt(193)+nt(13)+nt(192)+nt(13)+nt(192)));
		//       bf_xor_cb(162)       => bf_xor_cb_sym(194) __(13) bf_cb_arg(192) __(13) bf_cb_arg(192).
		q(nt(162), (nt(194)+nt(13)+nt(192)+nt(13)+nt(192)));
		//       bf_neg_cb(163)       => bf_neg_cb_sym(195) __(13) bf_cb_arg(192).
		q(nt(163), (nt(195)+nt(13)+nt(192)));
		//       bf_eq_cb(116)        => bf_eq_cb_sym(196) __(13) bf_cb_arg(192) __(13) wff_cb_arg(197) __(13) wff_cb_arg(197).
		q(nt(116), (nt(196)+nt(13)+nt(192)+nt(13)+nt(197)+nt(13)+nt(197)));
		//       bf_neq_cb(117)       => bf_neq_cb_sym(198) __(13) bf_cb_arg(192) __(13) wff_cb_arg(197) __(13) wff_cb_arg(197).
		q(nt(117), (nt(198)+nt(13)+nt(192)+nt(13)+nt(197)+nt(13)+nt(197)));
		//       bf_is_zero_cb(155)   => bf_is_zero_cb_sym(199) __(13) bf_cb_arg(192) __(13) bf_cb_arg(192).
		q(nt(155), (nt(199)+nt(13)+nt(192)+nt(13)+nt(192)));
		//       bf_is_one_cb(156)    => bf_is_one_cb_sym(200) __(13) bf_cb_arg(192) __(13) bf_cb_arg(192).
		q(nt(156), (nt(200)+nt(13)+nt(192)+nt(13)+nt(192)));
		//       bf_remove_funiversal_cb(158) => bf_remove_funiversal_cb_sym(201) __(13) bf_cb_arg(192) __(13) bf_cb_arg(192) __(13) bf_cb_arg(192) __(13) bf_cb_arg(192).
		q(nt(158), (nt(201)+nt(13)+nt(192)+nt(13)+nt(192)+nt(13)+nt(192)+nt(13)+nt(192)));
		//       bf_remove_fexistential_cb(159) => bf_remove_fexistential_cb_sym(202) __(13) bf_cb_arg(192) __(13) bf_cb_arg(192) __(13) bf_cb_arg(192) __(13) bf_cb_arg(192).
		q(nt(159), (nt(202)+nt(13)+nt(192)+nt(13)+nt(192)+nt(13)+nt(192)+nt(13)+nt(192)));
		//       wff_remove_existential_cb(120) => wff_remove_existential_cb_sym(203) __(13) wff_cb_arg(197) __(13) wff_cb_arg(197).
		q(nt(120), (nt(203)+nt(13)+nt(197)+nt(13)+nt(197)));
		//       wff_remove_bexistential_cb(121) => wff_remove_bexistential_cb_sym(204) __(13) wff_cb_arg(197) __(13) wff_cb_arg(197) __(13) wff_cb_arg(197) __(13) wff_cb_arg(197).
		q(nt(121), (nt(204)+nt(13)+nt(197)+nt(13)+nt(197)+nt(13)+nt(197)+nt(13)+nt(197)));
		//       wff_remove_buniversal_cb(122) => wff_remove_buniversal_cb_sym(205) __(13) wff_cb_arg(197) __(13) wff_cb_arg(197) __(13) wff_cb_arg(197) __(13) wff_cb_arg(197).
		q(nt(122), (nt(205)+nt(13)+nt(197)+nt(13)+nt(197)+nt(13)+nt(197)+nt(13)+nt(197)));
		//       wff_has_clashing_subformulas_cb(118) => wff_has_clashing_subformulas_cb_sym(206) __(13) wff_cb_arg(197) __(13) wff_cb_arg(197).
		q(nt(118), (nt(206)+nt(13)+nt(197)+nt(13)+nt(197)));
		//       bf_has_subformula_cb(157) => bf_has_subformula_cb_sym(207) __(13) bf_cb_arg(192) __(13) bf_cb_arg(192) __(13) bf_cb_arg(192).
		q(nt(157), (nt(207)+nt(13)+nt(192)+nt(13)+nt(192)+nt(13)+nt(192)));
		//       wff_has_subformula_cb(119) => wff_has_subformula_cb_sym(208) __(13) wff_cb_arg(197) __(13) wff_cb_arg(197) __(13) wff_cb_arg(197).
		q(nt(119), (nt(208)+nt(13)+nt(197)+nt(13)+nt(197)+nt(13)+nt(197)));
		//       bf_has_subformula_cb_sym(207) => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b'.
		q(nt(207), (t(30)+t(34)+t(40)+t(42)+t(26)+t(43)+t(40)+t(43)+t(44)+t(30)+t(34)+t(41)+t(45)+t(46)+t(44)+t(27)+t(26)+t(40)+t(47)+t(30)));
		//       wff_has_clashing_subformulas_cb_sym(206) => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b'.
		q(nt(206), (t(48)+t(34)+t(34)+t(40)+t(42)+t(26)+t(43)+t(40)+t(47)+t(27)+t(26)+t(43)+t(42)+t(39)+t(49)+t(50)+t(40)+t(43)+t(44)+t(30)+t(34)+t(41)+t(45)+t(46)+t(44)+t(27)+t(26)+t(43)+t(40)+t(47)+t(30)));
		//       wff_has_subformula_cb_sym(208) => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b'.
		q(nt(208), (t(48)+t(34)+t(34)+t(40)+t(42)+t(26)+t(43)+t(40)+t(43)+t(44)+t(30)+t(34)+t(41)+t(45)+t(46)+t(44)+t(27)+t(26)+t(40)+t(47)+t(30)));
		//       wff_remove_existential_cb_sym(203) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(203), (t(48)+t(34)+t(34)+t(40)+t(45)+t(28)+t(46)+t(41)+t(51)+t(28)+t(40)+t(28)+t(29)+t(39)+t(43)+t(52)+t(28)+t(49)+t(52)+t(39)+t(26)+t(27)+t(40)+t(47)+t(30)));
		//       wff_remove_bexistential_cb_sym(204) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(204), (t(48)+t(34)+t(34)+t(40)+t(45)+t(28)+t(46)+t(41)+t(51)+t(28)+t(40)+t(30)+t(28)+t(29)+t(39)+t(43)+t(52)+t(28)+t(49)+t(52)+t(39)+t(26)+t(27)+t(40)+t(47)+t(30)));
		//       wff_remove_buniversal_cb_sym(205) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b'.
		q(nt(205), (t(48)+t(34)+t(34)+t(40)+t(45)+t(28)+t(46)+t(41)+t(51)+t(28)+t(40)+t(30)+t(44)+t(49)+t(39)+t(51)+t(28)+t(45)+t(43)+t(26)+t(27)+t(40)+t(47)+t(30)));
		//       bf_remove_fexistential_cb_sym(202) => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(202), (t(30)+t(34)+t(40)+t(45)+t(28)+t(46)+t(41)+t(51)+t(28)+t(40)+t(34)+t(28)+t(29)+t(39)+t(43)+t(52)+t(28)+t(49)+t(52)+t(39)+t(26)+t(27)+t(40)+t(47)+t(30)));
		//       bf_remove_funiversal_cb_sym(201) => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b'.
		q(nt(201), (t(30)+t(34)+t(40)+t(45)+t(28)+t(46)+t(41)+t(51)+t(28)+t(40)+t(34)+t(44)+t(49)+t(39)+t(51)+t(28)+t(45)+t(43)+t(26)+t(27)+t(40)+t(47)+t(30)));
		//       bf_cb_arg(192)       => bf(107).
		q(nt(192), (nt(107)));
		//       wff_cb_arg(197)      => wff(112).
		q(nt(197), (nt(112)));
		//       bf_and_cb_sym(191)   => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b'.
		q(nt(191), (t(30)+t(34)+t(40)+t(26)+t(49)+t(53)+t(40)+t(47)+t(30)));
		//       bf_or_cb_sym(193)    => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b'.
		q(nt(193), (t(30)+t(34)+t(40)+t(41)+t(45)+t(40)+t(47)+t(30)));
		//       bf_xor_cb_sym(194)   => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b'.
		q(nt(194), (t(30)+t(34)+t(40)+t(29)+t(41)+t(45)+t(40)+t(47)+t(30)));
		//       bf_neg_cb_sym(195)   => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b'.
		q(nt(195), (t(30)+t(34)+t(40)+t(49)+t(28)+t(50)+t(40)+t(47)+t(30)));
		//       bf_eq_cb_sym(196)    => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b'.
		q(nt(196), (t(30)+t(34)+t(40)+t(28)+t(54)+t(40)+t(47)+t(30)));
		//       bf_neq_cb_sym(198)   => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b'.
		q(nt(198), (t(30)+t(34)+t(40)+t(49)+t(28)+t(54)+t(40)+t(47)+t(30)));
		//       bf_is_zero_cb_sym(199) => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b'.
		q(nt(199), (t(30)+t(34)+t(40)+t(39)+t(43)+t(40)+t(55)+t(28)+t(45)+t(41)+t(40)+t(47)+t(30)));
		//       bf_is_one_cb_sym(200) => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b'.
		q(nt(200), (t(30)+t(34)+t(40)+t(39)+t(43)+t(40)+t(41)+t(49)+t(28)+t(40)+t(47)+t(30)));
		//       __E_tau_collapse_positives_cb_34(209) => tau_collapse_positives_cb_sym(210) __(13) tau_cb_arg(211) __(13) tau_cb_arg(211) __(13) tau_cb_arg(211).
		q(nt(209), (nt(210)+nt(13)+nt(211)+nt(13)+nt(211)+nt(13)+nt(211)));
		//       tau_collapse_positives_cb(98) => __E_tau_collapse_positives_cb_34(209).
		q(nt(98), (nt(209)));
		//       __E_tau_collapse_positives_cb_35(212) => tau_collapse_positives_cb_sym(210) __(13) tau_cb_arg(211) __(13) tau_cb_arg(211).
		q(nt(212), (nt(210)+nt(13)+nt(211)+nt(13)+nt(211)));
		//       tau_collapse_positives_cb(98) => __E_tau_collapse_positives_cb_35(212).
		q(nt(98), (nt(212)));
		//       tau_positives_upwards_cb(99) => tau_positives_upwards_cb_sym(213) __(13) tau_cb_arg(211) __(13) tau_cb_arg(211).
		q(nt(99), (nt(213)+nt(13)+nt(211)+nt(13)+nt(211)));
		//       tau_cb_arg(211)      => capture(76).
		q(nt(211), (nt(76)));
		//       tau_cb_arg(211)      => tau(97).
		q(nt(211), (nt(97)));
		//       tau_collapse_positives_cb_sym(210) => 't' 'a' 'u' '_' 'c' 'o' 'l' 'l' 'a' 'p' 's' 'e' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'c' 'b'.
		q(nt(210), (t(52)+t(26)+t(44)+t(40)+t(47)+t(41)+t(27)+t(27)+t(26)+t(56)+t(43)+t(28)+t(40)+t(56)+t(41)+t(43)+t(39)+t(52)+t(39)+t(51)+t(28)+t(43)+t(40)+t(47)+t(30)));
		//       tau_positives_upwards_cb_sym(213) => 't' 'a' 'u' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'u' 'p' 'w' 'a' 'r' 'd' 's' '_' 'c' 'b'.
		q(nt(213), (t(52)+t(26)+t(44)+t(40)+t(56)+t(41)+t(43)+t(39)+t(52)+t(39)+t(51)+t(28)+t(43)+t(40)+t(44)+t(56)+t(48)+t(26)+t(45)+t(53)+t(43)+t(40)+t(47)+t(30)));
		//       __E_inputs_36(216)   => _(15) input(215).
		q(nt(216), (nt(15)+nt(215)));
		//       __E_inputs_37(217)   => null.
		q(nt(217), (nul));
		//       __E_inputs_37(217)   => __E_inputs_36(216) __E_inputs_37(217).
		q(nt(217), (nt(216)+nt(217)));
		//       inputs(214)          => _(15) less(27) _(15) input(215) __E_inputs_37(217) _(15) dot(17).
		q(nt(214), (nt(15)+nt(27)+nt(15)+nt(215)+nt(217)+nt(15)+nt(17)));
		//       input(215)           => in(84) _(15) colon(25) _(15) open_brace(22) _(15) source_binding(184) _(15) close_brace(23).
		q(nt(215), (nt(84)+nt(15)+nt(25)+nt(15)+nt(22)+nt(15)+nt(184)+nt(15)+nt(23)));
		//       builder(218)         => _(15) builder_head(219) _(15) builder_body(220) _(15) dot(17).
		q(nt(218), (nt(15)+nt(219)+nt(15)+nt(220)+nt(15)+nt(17)));
		//       __E_builder_head_38(221) => __(13) capture(76).
		q(nt(221), (nt(13)+nt(76)));
		//       __E_builder_head_39(222) => null.
		q(nt(222), (nul));
		//       __E_builder_head_39(222) => __E_builder_head_38(221) __E_builder_head_39(222).
		q(nt(222), (nt(221)+nt(222)));
		//       builder_head(219)    => open_parenthesis(18) _(15) capture(76) __E_builder_head_39(222) _(15) close_parenthesis(19).
		q(nt(219), (nt(18)+nt(15)+nt(76)+nt(222)+nt(15)+nt(19)));
		//       builder_body(220)    => bf_builder_body(223).
		q(nt(220), (nt(223)));
		//       builder_body(220)    => wff_builder_body(224).
		q(nt(220), (nt(224)));
		//       builder_body(220)    => tau_builder_body(225).
		q(nt(220), (nt(225)));
		//       bf_builder_body(223) => bf_def(49) _(15) bf(107).
		q(nt(223), (nt(49)+nt(15)+nt(107)));
		//       wff_builder_body(224) => wff_def(35) _(15) wff(112).
		q(nt(224), (nt(35)+nt(15)+nt(112)));
		//       tau_builder_body(225) => tau_def(31) _(15) tau(97).
		q(nt(225), (nt(31)+nt(15)+nt(97)));
		//       library(226)         => rules(227).
		q(nt(226), (nt(227)));
		//       __E_rules_40(228)    => _(15) rule(229).
		q(nt(228), (nt(15)+nt(229)));
		//       __E_rules_41(230)    => null.
		q(nt(230), (nul));
		//       __E_rules_41(230)    => __E_rules_40(228) __E_rules_41(230).
		q(nt(230), (nt(228)+nt(230)));
		//       rules(227)           => __E_rules_41(230).
		q(nt(227), (nt(230)));
		//       rule(229)            => wff_rule(113).
		q(nt(229), (nt(113)));
		//       rule(229)            => bf_rule(152).
		q(nt(229), (nt(152)));
		//       rule(229)            => tau_rule(94).
		q(nt(229), (nt(94)));
		//       nso_rr(231)          => nso_rec_relations(232) nso_main(233).
		q(nt(231), (nt(232)+nt(233)));
		//       __E_nso_rec_relations_42(234) => _(15) nso_rec_relation(235).
		q(nt(234), (nt(15)+nt(235)));
		//       __E_nso_rec_relations_43(236) => null.
		q(nt(236), (nul));
		//       __E_nso_rec_relations_43(236) => __E_nso_rec_relations_42(234) __E_nso_rec_relations_43(236).
		q(nt(236), (nt(234)+nt(236)));
		//       nso_rec_relations(232) => __E_nso_rec_relations_43(236).
		q(nt(232), (nt(236)));
		//       nso_rec_relation(235) => bf_rec_relation(173).
		q(nt(235), (nt(173)));
		//       nso_rec_relation(235) => wff_rec_relation(123).
		q(nt(235), (nt(123)));
		//       nso_main(233)        => _(15) wff(112) _(15) dot(17).
		q(nt(233), (nt(15)+nt(112)+nt(15)+nt(17)));
		//       gssotc_rr(237)       => gssotc_rec_relations(238) gssotc_main(239).
		q(nt(237), (nt(238)+nt(239)));
		//       __E_gssotc_rec_relations_44(240) => _(15) gssotc_rec_relation(241).
		q(nt(240), (nt(15)+nt(241)));
		//       __E_gssotc_rec_relations_45(242) => null.
		q(nt(242), (nul));
		//       __E_gssotc_rec_relations_45(242) => __E_gssotc_rec_relations_44(240) __E_gssotc_rec_relations_45(242).
		q(nt(242), (nt(240)+nt(242)));
		//       gssotc_rec_relations(238) => __E_gssotc_rec_relations_45(242).
		q(nt(238), (nt(242)));
		//       gssotc_rec_relation(241) => tau_rec_relation(100).
		q(nt(241), (nt(100)));
		//       gssotc_rec_relation(241) => wff_rec_relation(123).
		q(nt(241), (nt(123)));
		//       gssotc_rec_relation(241) => bf_rec_relation(173).
		q(nt(241), (nt(173)));
		//       gssotc_main(239)     => _(15) tau(97) _(15) semicolon(26).
		q(nt(239), (nt(15)+nt(97)+nt(15)+nt(26)));
		//       __E_start_46(244)    => inputs(214).
		q(nt(244), (nt(214)));
		//       __E_start_46(244)    => builder(218).
		q(nt(244), (nt(218)));
		//       __E_start_46(244)    => library(226).
		q(nt(244), (nt(226)));
		//       __E_start_46(244)    => nso_rr(231).
		q(nt(244), (nt(231)));
		//       __E_start_46(244)    => gssotc_rr(237).
		q(nt(244), (nt(237)));
		//       start(243)           => __E_start_46(244) _(15).
		q(nt(243), (nt(244)+nt(15)));
		//       __E_cli_47(247)      => _(15) dot(17) _(15) cli_command(246).
		q(nt(247), (nt(15)+nt(17)+nt(15)+nt(246)));
		//       __E_cli_48(248)      => null.
		q(nt(248), (nul));
		//       __E_cli_48(248)      => __E_cli_47(247) __E_cli_48(248).
		q(nt(248), (nt(247)+nt(248)));
		//       cli(245)             => _(15) cli_command(246) __E_cli_48(248).
		q(nt(245), (nt(15)+nt(246)+nt(248)));
		//       cli_command(246)     => help_cmd(249).
		q(nt(246), (nt(249)));
		//       cli_command(246)     => version_cmd(250).
		q(nt(246), (nt(250)));
		//       cli_command(246)     => quit_cmd(251).
		q(nt(246), (nt(251)));
		//       cli_command(246)     => clear_cmd(252).
		q(nt(246), (nt(252)));
		//       cli_command(246)     => get_cmd(253).
		q(nt(246), (nt(253)));
		//       cli_command(246)     => set_cmd(254).
		q(nt(246), (nt(254)));
		//       cli_command(246)     => toggle_cmd(255).
		q(nt(246), (nt(255)));
		//       cli_command(246)     => list_outputs_cmd(256).
		q(nt(246), (nt(256)));
		//       cli_command(246)     => clear_outputs_cmd(257).
		q(nt(246), (nt(257)));
		//       cli_command(246)     => print_output_cmd(258).
		q(nt(246), (nt(258)));
		//       cli_command(246)     => file(259).
		q(nt(246), (nt(259)));
		//       cli_command(246)     => normalize_cmd(260).
		q(nt(246), (nt(260)));
		//       cli_command(246)     => wff_onf_cmd(261).
		q(nt(246), (nt(261)));
		//       cli_command(246)     => wff_dnf_cmd(262).
		q(nt(246), (nt(262)));
		//       cli_command(246)     => wff_cnf_cmd(263).
		q(nt(246), (nt(263)));
		//       cli_command(246)     => wff_anf_cmd(264).
		q(nt(246), (nt(264)));
		//       cli_command(246)     => wff_nnf_cmd(265).
		q(nt(246), (nt(265)));
		//       cli_command(246)     => wff_pnf_cmd(266).
		q(nt(246), (nt(266)));
		//       cli_command(246)     => wff_mnf_cmd(267).
		q(nt(246), (nt(267)));
		//       cli_command(246)     => bf_dnf_cmd(268).
		q(nt(246), (nt(268)));
		//       cli_command(246)     => bf_cnf_cmd(269).
		q(nt(246), (nt(269)));
		//       cli_command(246)     => bf_anf_cmd(270).
		q(nt(246), (nt(270)));
		//       cli_command(246)     => bf_nnf_cmd(271).
		q(nt(246), (nt(271)));
		//       cli_command(246)     => bf_pnf_cmd(272).
		q(nt(246), (nt(272)));
		//       cli_command(246)     => bf_mnf_cmd(273).
		q(nt(246), (nt(273)));
		//       cli_command(246)     => bf_substitute(274).
		q(nt(246), (nt(274)));
		//       cli_command(246)     => wff_substitute(275).
		q(nt(246), (nt(275)));
		//       cli_command(246)     => bf_instantiate(276).
		q(nt(246), (nt(276)));
		//       cli_command(246)     => wff_instantiate(277).
		q(nt(246), (nt(277)));
		//       cli_command(246)     => bf(107).
		q(nt(246), (nt(107)));
		//       cli_command(246)     => wff(112).
		q(nt(246), (nt(112)));
		//       cli_command(246)     => nso_rr(231).
		q(nt(246), (nt(231)));
		//       file(259)            => file_sym(278) quote(30) file_path(279) quote(30).
		q(nt(259), (nt(278)+nt(30)+nt(279)+nt(30)));
		//       normalize_cmd(260)   => normalize_cmd_sym(280) __(13) normalize_cmd_arg(281).
		q(nt(260), (nt(280)+nt(13)+nt(281)));
		//       normalize_cmd_arg(281) => wff(112).
		q(nt(281), (nt(112)));
		//       normalize_cmd_arg(281) => nso_rr(231).
		q(nt(281), (nt(231)));
		//       normalize_cmd_arg(281) => output(282).
		q(nt(281), (nt(282)));
		//       normalize_cmd_arg(281) => wff_selection(283).
		q(nt(281), (nt(283)));
		//       __E_bf_instantiate_cmd_49(286) => bf(107).
		q(nt(286), (nt(107)));
		//       __E_bf_instantiate_cmd_49(286) => bf_selection(287).
		q(nt(286), (nt(287)));
		//       __E_bf_instantiate_cmd_49(286) => output(282).
		q(nt(286), (nt(282)));
		//       __E_bf_instantiate_cmd_50(288) => bf_selection(287).
		q(nt(288), (nt(287)));
		//       __E_bf_instantiate_cmd_50(288) => output(282).
		q(nt(288), (nt(282)));
		//       bf_instantiate_cmd(284) => instantiate_cmd_sym(285) __(13) variable(79) __(13) __E_bf_instantiate_cmd_49(286) __(13) __E_bf_instantiate_cmd_50(288).
		q(nt(284), (nt(285)+nt(13)+nt(79)+nt(13)+nt(286)+nt(13)+nt(288)));
		//       __E_wff_instantiate_cmd_51(290) => wff(112).
		q(nt(290), (nt(112)));
		//       __E_wff_instantiate_cmd_51(290) => wff_selection(283).
		q(nt(290), (nt(283)));
		//       __E_wff_instantiate_cmd_51(290) => output(282).
		q(nt(290), (nt(282)));
		//       __E_wff_instantiate_cmd_52(291) => wff_selection(283).
		q(nt(291), (nt(283)));
		//       __E_wff_instantiate_cmd_52(291) => output(282).
		q(nt(291), (nt(282)));
		//       wff_instantiate_cmd(289) => instantiate_cmd_sym(285) __(13) variable(79) __(13) __E_wff_instantiate_cmd_51(290) __(13) __E_wff_instantiate_cmd_52(291).
		q(nt(289), (nt(285)+nt(13)+nt(79)+nt(13)+nt(290)+nt(13)+nt(291)));
		//       __E_bf_substitute_cmd_53(294) => bf(107).
		q(nt(294), (nt(107)));
		//       __E_bf_substitute_cmd_53(294) => output(282).
		q(nt(294), (nt(282)));
		//       __E_bf_substitute_cmd_53(294) => bf_selection(287).
		q(nt(294), (nt(287)));
		//       __E_bf_substitute_cmd_54(295) => output(282).
		q(nt(295), (nt(282)));
		//       __E_bf_substitute_cmd_54(295) => bf_selection(287).
		q(nt(295), (nt(287)));
		//       __E_bf_substitute_cmd_54(295) => bf_selection(287).
		q(nt(295), (nt(287)));
		//       __E_bf_substitute_cmd_55(296) => bf_selection(287).
		q(nt(296), (nt(287)));
		//       __E_bf_substitute_cmd_55(296) => output(282).
		q(nt(296), (nt(282)));
		//       bf_substitute_cmd(292) => substitute_cmd_sym(293) __(13) __E_bf_substitute_cmd_53(294) __(13) __E_bf_substitute_cmd_54(295) __E_bf_substitute_cmd_55(296).
		q(nt(292), (nt(293)+nt(13)+nt(294)+nt(13)+nt(295)+nt(296)));
		//       __E_wff_substitute_cmd_56(299) => wff(112).
		q(nt(299), (nt(112)));
		//       __E_wff_substitute_cmd_56(299) => output(282).
		q(nt(299), (nt(282)));
		//       __E_wff_substitute_cmd_56(299) => wff_selection(283).
		q(nt(299), (nt(283)));
		//       __E_wff_substitute_cmd_57(300) => output(282).
		q(nt(300), (nt(282)));
		//       __E_wff_substitute_cmd_57(300) => wff_selection(283).
		q(nt(300), (nt(283)));
		//       __E_wff_substitute_cmd_58(301) => wff_selection(283).
		q(nt(301), (nt(283)));
		//       __E_wff_substitute_cmd_58(301) => output(282).
		q(nt(301), (nt(282)));
		//       wff_substitute_cmd(297) => substitutecmd__sym(298) __(13) __E_wff_substitute_cmd_56(299) __(13) __E_wff_substitute_cmd_57(300) __E_wff_substitute_cmd_58(301).
		q(nt(297), (nt(298)+nt(13)+nt(299)+nt(13)+nt(300)+nt(301)));
		//       __E_bf_dnf_cmd_59(303) => bf(107).
		q(nt(303), (nt(107)));
		//       __E_bf_dnf_cmd_59(303) => output(282).
		q(nt(303), (nt(282)));
		//       __E_bf_dnf_cmd_59(303) => bf_selection(287).
		q(nt(303), (nt(287)));
		//       bf_dnf_cmd(268)      => dnf_cmd_sym(302) __(13) __E_bf_dnf_cmd_59(303).
		q(nt(268), (nt(302)+nt(13)+nt(303)));
		//       __E_bf_cnf_cmd_60(305) => bf(107).
		q(nt(305), (nt(107)));
		//       __E_bf_cnf_cmd_60(305) => output(282).
		q(nt(305), (nt(282)));
		//       __E_bf_cnf_cmd_60(305) => bf_selection(287).
		q(nt(305), (nt(287)));
		//       bf_cnf_cmd(269)      => cnf_cmd_sym(304) __(13) __E_bf_cnf_cmd_60(305).
		q(nt(269), (nt(304)+nt(13)+nt(305)));
		//       __E_bf_anf_cmd_61(307) => bf(107).
		q(nt(307), (nt(107)));
		//       __E_bf_anf_cmd_61(307) => output(282).
		q(nt(307), (nt(282)));
		//       __E_bf_anf_cmd_61(307) => bf_selection(287).
		q(nt(307), (nt(287)));
		//       bf_anf_cmd(270)      => anf_cmd_sym(306) __(13) __E_bf_anf_cmd_61(307).
		q(nt(270), (nt(306)+nt(13)+nt(307)));
		//       __E_bf_nnf_cmd_62(309) => bf(107).
		q(nt(309), (nt(107)));
		//       __E_bf_nnf_cmd_62(309) => output(282).
		q(nt(309), (nt(282)));
		//       __E_bf_nnf_cmd_62(309) => bf_selection(287).
		q(nt(309), (nt(287)));
		//       bf_nnf_cmd(271)      => nnf_cmd_sym(308) __(13) __E_bf_nnf_cmd_62(309).
		q(nt(271), (nt(308)+nt(13)+nt(309)));
		//       __E_bf_pnf_cmd_63(311) => bf(107).
		q(nt(311), (nt(107)));
		//       __E_bf_pnf_cmd_63(311) => output(282).
		q(nt(311), (nt(282)));
		//       __E_bf_pnf_cmd_63(311) => bf_selection(287).
		q(nt(311), (nt(287)));
		//       bf_pnf_cmd(272)      => pnf_cmd_sym(310) __(13) __E_bf_pnf_cmd_63(311).
		q(nt(272), (nt(310)+nt(13)+nt(311)));
		//       __E_bf_mnf_cmd_64(313) => bf(107).
		q(nt(313), (nt(107)));
		//       __E_bf_mnf_cmd_64(313) => output(282).
		q(nt(313), (nt(282)));
		//       __E_bf_mnf_cmd_64(313) => bf_selection(287).
		q(nt(313), (nt(287)));
		//       bf_mnf_cmd(273)      => mnf_cmd_sym(312) __(13) __E_bf_mnf_cmd_64(313).
		q(nt(273), (nt(312)+nt(13)+nt(313)));
		//       __E_wff_onf_cmd_65(315) => wff(112).
		q(nt(315), (nt(112)));
		//       __E_wff_onf_cmd_65(315) => output(282).
		q(nt(315), (nt(282)));
		//       __E_wff_onf_cmd_65(315) => wff_selection(283).
		q(nt(315), (nt(283)));
		//       wff_onf_cmd(261)     => onf_cmd_sym(314) __(13) variable(79) __E_wff_onf_cmd_65(315).
		q(nt(261), (nt(314)+nt(13)+nt(79)+nt(315)));
		//       __E_wff_dnf_cmd_66(316) => wff(112).
		q(nt(316), (nt(112)));
		//       __E_wff_dnf_cmd_66(316) => output(282).
		q(nt(316), (nt(282)));
		//       __E_wff_dnf_cmd_66(316) => wff_selection(283).
		q(nt(316), (nt(283)));
		//       wff_dnf_cmd(262)     => dnf_cmd_sym(302) __(13) __E_wff_dnf_cmd_66(316).
		q(nt(262), (nt(302)+nt(13)+nt(316)));
		//       __E_wff_cnf_cmd_67(317) => wff(112).
		q(nt(317), (nt(112)));
		//       __E_wff_cnf_cmd_67(317) => output(282).
		q(nt(317), (nt(282)));
		//       __E_wff_cnf_cmd_67(317) => wff_selection(283).
		q(nt(317), (nt(283)));
		//       wff_cnf_cmd(263)     => cnf_cmd_sym(304) __(13) __E_wff_cnf_cmd_67(317).
		q(nt(263), (nt(304)+nt(13)+nt(317)));
		//       __E_wff_anf_cmd_68(318) => wff(112).
		q(nt(318), (nt(112)));
		//       __E_wff_anf_cmd_68(318) => output(282).
		q(nt(318), (nt(282)));
		//       __E_wff_anf_cmd_68(318) => wff_selection(283).
		q(nt(318), (nt(283)));
		//       wff_anf_cmd(264)     => anf_cmd_sym(306) __(13) __E_wff_anf_cmd_68(318).
		q(nt(264), (nt(306)+nt(13)+nt(318)));
		//       __E_wff_nnf_cmd_69(319) => wff(112).
		q(nt(319), (nt(112)));
		//       __E_wff_nnf_cmd_69(319) => output(282).
		q(nt(319), (nt(282)));
		//       __E_wff_nnf_cmd_69(319) => wff_selection(283).
		q(nt(319), (nt(283)));
		//       wff_nnf_cmd(265)     => nnf_cmd_sym(308) __(13) __E_wff_nnf_cmd_69(319).
		q(nt(265), (nt(308)+nt(13)+nt(319)));
		//       __E_wff_pnf_cmd_70(320) => wff(112).
		q(nt(320), (nt(112)));
		//       __E_wff_pnf_cmd_70(320) => output(282).
		q(nt(320), (nt(282)));
		//       __E_wff_pnf_cmd_70(320) => wff_selection(283).
		q(nt(320), (nt(283)));
		//       wff_pnf_cmd(266)     => pnf_cmd_sym(310) __(13) __E_wff_pnf_cmd_70(320).
		q(nt(266), (nt(310)+nt(13)+nt(320)));
		//       __E_wff_mnf_cmd_71(321) => wff(112).
		q(nt(321), (nt(112)));
		//       __E_wff_mnf_cmd_71(321) => output(282).
		q(nt(321), (nt(282)));
		//       __E_wff_mnf_cmd_71(321) => wff_selection(283).
		q(nt(321), (nt(283)));
		//       wff_mnf_cmd(267)     => mnf_cmd_sym(312) __(13) __E_wff_mnf_cmd_71(321).
		q(nt(267), (nt(312)+nt(13)+nt(321)));
		//       __E_help_cmd_72(323) => __(13) cli_cmd_sym(324).
		q(nt(323), (nt(13)+nt(324)));
		//       __E_help_cmd_72(323) => null.
		q(nt(323), (nul));
		//       help_cmd(249)        => help_cmd_sym(322) __E_help_cmd_72(323).
		q(nt(249), (nt(322)+nt(323)));
		//       version_cmd(250)     => version_cmd_sym(325).
		q(nt(250), (nt(325)));
		//       quit_cmd(251)        => quit_cmd_sym(326).
		q(nt(251), (nt(326)));
		//       clear_cmd(252)       => clear_cmd_sym(327).
		q(nt(252), (nt(327)));
		//       wff_selection(283)   => selection_sym(328) __(13) digits(68).
		q(nt(283), (nt(328)+nt(13)+nt(68)));
		//       bf_selection(287)    => selection_sym(328) __(13) digits(68).
		q(nt(287), (nt(328)+nt(13)+nt(68)));
		//       bf_var_selection(329) => selection_sym(328) __(13) digits(68).
		q(nt(329), (nt(328)+nt(13)+nt(68)));
		//       wff_var_selection(330) => selection_sym(328) __(13) digits(68).
		q(nt(330), (nt(328)+nt(13)+nt(68)));
		//       file_path(279)       => chars(65).
		q(nt(279), (nt(65)));
		//       output(282)          => absolute_output(331).
		q(nt(282), (nt(331)));
		//       output(282)          => relative_output(332).
		q(nt(282), (nt(332)));
		//       __E_absolute_output_73(333) => absolute_output_sym(334).
		q(nt(333), (nt(334)));
		//       __E_absolute_output_73(333) => output_sym(335) __(13).
		q(nt(333), (nt(335)+nt(13)));
		//       absolute_output(331) => __E_absolute_output_73(333) output_id(336).
		q(nt(331), (nt(333)+nt(336)));
		//       relative_output(332) => relative_output_sym(337) output_id(336).
		q(nt(332), (nt(337)+nt(336)));
		//       output_id(336)       => digits(68).
		q(nt(336), (nt(68)));
		//       list_outputs_cmd(256) => output_sym(335).
		q(nt(256), (nt(335)));
		//       list_outputs_cmd(256) => absolute_output_sym(334).
		q(nt(256), (nt(334)));
		//       list_outputs_cmd(256) => relative_output_sym(337).
		q(nt(256), (nt(337)));
		//       __E_clear_outputs_cmd_74(338) => output_sym(335) __(13).
		q(nt(338), (nt(335)+nt(13)));
		//       __E_clear_outputs_cmd_74(338) => absolute_output_sym(334).
		q(nt(338), (nt(334)));
		//       __E_clear_outputs_cmd_74(338) => relative_output_sym(337).
		q(nt(338), (nt(337)));
		//       clear_outputs_cmd(257) => __E_clear_outputs_cmd_74(338) clear_sym(339).
		q(nt(257), (nt(338)+nt(339)));
		//       print_output_cmd(258) => output(282).
		q(nt(258), (nt(282)));
		//       cli_cmd_sym(324)     => help_cmd_sym(322).
		q(nt(324), (nt(322)));
		//       cli_cmd_sym(324)     => version_cmd_sym(325).
		q(nt(324), (nt(325)));
		//       cli_cmd_sym(324)     => quit_cmd_sym(326).
		q(nt(324), (nt(326)));
		//       cli_cmd_sym(324)     => clear_cmd_sym(327).
		q(nt(324), (nt(327)));
		//       cli_cmd_sym(324)     => output_sym(335).
		q(nt(324), (nt(335)));
		//       cli_cmd_sym(324)     => absolute_output_sym(334).
		q(nt(324), (nt(334)));
		//       cli_cmd_sym(324)     => relative_output_sym(337).
		q(nt(324), (nt(337)));
		//       cli_cmd_sym(324)     => selection_sym(328).
		q(nt(324), (nt(328)));
		//       cli_cmd_sym(324)     => instantiate_cmd_sym(285).
		q(nt(324), (nt(285)));
		//       cli_cmd_sym(324)     => substitute_cmd_sym(293).
		q(nt(324), (nt(293)));
		//       cli_cmd_sym(324)     => normalize_cmd_sym(280).
		q(nt(324), (nt(280)));
		//       cli_cmd_sym(324)     => file_cmd_sym(340).
		q(nt(324), (nt(340)));
		//       cli_cmd_sym(324)     => get_cmd_sym(341).
		q(nt(324), (nt(341)));
		//       cli_cmd_sym(324)     => set_cmd_sym(342).
		q(nt(324), (nt(342)));
		//       cli_cmd_sym(324)     => toggle_cmd_sym(343).
		q(nt(324), (nt(343)));
		//       help_cmd_sym(322)    => 'h'.
		q(nt(322), (t(42)));
		//       help_cmd_sym(322)    => 'h' 'e' 'l' 'p'.
		q(nt(322), (t(42)+t(28)+t(27)+t(56)));
		//       version_cmd_sym(325) => 'v'.
		q(nt(325), (t(51)));
		//       version_cmd_sym(325) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
		q(nt(325), (t(51)+t(28)+t(45)+t(43)+t(39)+t(41)+t(49)));
		//       quit_cmd_sym(326)    => 'q'.
		q(nt(326), (t(54)));
		//       quit_cmd_sym(326)    => 'e'.
		q(nt(326), (t(28)));
		//       quit_cmd_sym(326)    => 'q' 'u' 'i' 't'.
		q(nt(326), (t(54)+t(44)+t(39)+t(52)));
		//       quit_cmd_sym(326)    => 'e' 'x' 'i' 't'.
		q(nt(326), (t(28)+t(29)+t(39)+t(52)));
		//       instantiate_sym(344) => 'i'.
		q(nt(344), (t(39)));
		//       instantiate_sym(344) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
		q(nt(344), (t(39)+t(49)+t(43)+t(52)+t(26)+t(49)+t(52)+t(39)+t(26)+t(52)+t(28)));
		//       substitute_sym(345)  => 's'.
		q(nt(345), (t(43)));
		//       substitute_sym(345)  => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
		q(nt(345), (t(43)+t(44)+t(30)+t(43)+t(52)+t(39)+t(52)+t(44)+t(52)+t(28)));
		//       normalize_cmd_sym(280) => 'n'.
		q(nt(280), (t(49)));
		//       normalize_cmd_sym(280) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
		q(nt(280), (t(49)+t(41)+t(45)+t(46)+t(26)+t(27)+t(39)+t(55)+t(28)));
		//       file_cmd_sym(340)    => 'r'.
		q(nt(340), (t(45)));
		//       file_cmd_sym(340)    => 'r' 'e' 'a' 'd'.
		q(nt(340), (t(45)+t(28)+t(26)+t(53)));
		//       clear_cmd_sym(327)   => 'c'.
		q(nt(327), (t(47)));
		//       clear_cmd_sym(327)   => 'c' 'l' 'e' 'a' 'r'.
		q(nt(327), (t(47)+t(27)+t(28)+t(26)+t(45)));
		//       onf_cmd_sym(314)     => 'o' 'n' 'f'.
		q(nt(314), (t(41)+t(49)+t(34)));
		//       dnf_cmd_sym(302)     => 'd' 'n' 'f'.
		q(nt(302), (t(53)+t(49)+t(34)));
		//       cnf_cmd_sym(304)     => 'c' 'n' 'f'.
		q(nt(304), (t(47)+t(49)+t(34)));
		//       anf_cmd_sym(306)     => 'a' 'n' 'f'.
		q(nt(306), (t(26)+t(49)+t(34)));
		//       nnf_cmd_sym(308)     => 'n' 'n' 'f'.
		q(nt(308), (t(49)+t(49)+t(34)));
		//       pnf_cmd_sym(310)     => 'p' 'n' 'f'.
		q(nt(310), (t(56)+t(49)+t(34)));
		//       mnf_cmd_sym(312)     => 'm' 'n' 'f'.
		q(nt(312), (t(46)+t(49)+t(34)));
		//       selection_sym(328)   => 's'.
		q(nt(328), (t(43)));
		//       selection_sym(328)   => 's' 'e' 'l' 'e' 'c' 't' 'i' 'o' 'n'.
		q(nt(328), (t(43)+t(28)+t(27)+t(28)+t(47)+t(52)+t(39)+t(41)+t(49)));
		//       output_sym(335)      => 'o'.
		q(nt(335), (t(41)));
		//       output_sym(335)      => 'o' 'u' 't' 'p' 'u' 't'.
		q(nt(335), (t(41)+t(44)+t(52)+t(56)+t(44)+t(52)));
		//       absolute_output_sym(334) => '&'.
		q(nt(334), (t(20)));
		//       relative_output_sym(337) => '%'.
		q(nt(337), (t(57)));
		//       __E_get_cmd_75(346)  => __(13) option(347).
		q(nt(346), (nt(13)+nt(347)));
		//       __E_get_cmd_75(346)  => null.
		q(nt(346), (nul));
		//       get_cmd(253)         => get_cmd_sym(341) __E_get_cmd_75(346).
		q(nt(253), (nt(341)+nt(346)));
		//       __E_set_cmd_76(348)  => __(13).
		q(nt(348), (nt(13)));
		//       __E_set_cmd_76(348)  => _(15) '=' _(15).
		q(nt(348), (nt(15)+t(19)+nt(15)));
		//       set_cmd(254)         => set_cmd_sym(342) __(13) option(347) __E_set_cmd_76(348) option_value(349).
		q(nt(254), (nt(342)+nt(13)+nt(347)+nt(348)+nt(349)));
		//       toggle_cmd(255)      => toggle_cmd_sym(343) __(13) bool_option(350).
		q(nt(255), (nt(343)+nt(13)+nt(350)));
		//       get_cmd_sym(341)     => 'g' 'e' 't'.
		q(nt(341), (t(50)+t(28)+t(52)));
		//       set_cmd_sym(342)     => 's' 'e' 't'.
		q(nt(342), (t(43)+t(28)+t(52)));
		//       toggle_cmd_sym(343)  => 't' 'o' 'g' 'g' 'l' 'e'.
		q(nt(343), (t(52)+t(41)+t(50)+t(50)+t(27)+t(28)));
		//       option(347)          => bool_option(350).
		q(nt(347), (nt(350)));
		//       option(347)          => severity_opt(351).
		q(nt(347), (nt(351)));
		//       bool_option(350)     => status_opt(352).
		q(nt(350), (nt(352)));
		//       bool_option(350)     => colors_opt(353).
		q(nt(350), (nt(353)));
		//       bool_option(350)     => debug_repl_opt(354).
		q(nt(350), (nt(354)));
		//       status_opt(352)      => 's'.
		q(nt(352), (t(43)));
		//       status_opt(352)      => 's' 't' 'a' 't' 'u' 's'.
		q(nt(352), (t(43)+t(52)+t(26)+t(52)+t(44)+t(43)));
		//       colors_opt(353)      => 'c'.
		q(nt(353), (t(47)));
		//       colors_opt(353)      => 'c' 'o' 'l' 'o' 'r'.
		q(nt(353), (t(47)+t(41)+t(27)+t(41)+t(45)));
		//       colors_opt(353)      => 'c' 'o' 'l' 'o' 'r' 's'.
		q(nt(353), (t(47)+t(41)+t(27)+t(41)+t(45)+t(43)));
		//       severity_opt(351)    => 's' 'e' 'v'.
		q(nt(351), (t(43)+t(28)+t(51)));
		//       severity_opt(351)    => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
		q(nt(351), (t(43)+t(28)+t(51)+t(28)+t(45)+t(39)+t(52)+t(58)));
		//       debug_repl_opt(354)  => 'd'.
		q(nt(354), (t(53)));
		//       debug_repl_opt(354)  => 'd' 'r'.
		q(nt(354), (t(53)+t(45)));
		//       debug_repl_opt(354)  => 'd' 'b' 'g'.
		q(nt(354), (t(53)+t(30)+t(50)));
		//       debug_repl_opt(354)  => 'd' 'e' 'b' 'u' 'g'.
		q(nt(354), (t(53)+t(28)+t(30)+t(44)+t(50)));
		//       debug_repl_opt(354)  => 'd' 'e' 'b' 'u' 'g' '-' 'r' 'e' 'p' 'l'.
		q(nt(354), (t(53)+t(28)+t(30)+t(44)+t(50)+t(12)+t(45)+t(28)+t(56)+t(27)));
		//       option_value(349)    => option_value_true(355).
		q(nt(349), (nt(355)));
		//       option_value(349)    => option_value_false(356).
		q(nt(349), (nt(356)));
		//       option_value(349)    => severity(357).
		q(nt(349), (nt(357)));
		//       option_value_true(355) => 't'.
		q(nt(355), (t(52)));
		//       option_value_true(355) => 't' 'r' 'u' 'e'.
		q(nt(355), (t(52)+t(45)+t(44)+t(28)));
		//       option_value_true(355) => 'o' 'n'.
		q(nt(355), (t(41)+t(49)));
		//       option_value_true(355) => '1'.
		q(nt(355), (t(36)));
		//       option_value_true(355) => 'y'.
		q(nt(355), (t(58)));
		//       option_value_true(355) => 'y' 'e' 's'.
		q(nt(355), (t(58)+t(28)+t(43)));
		//       option_value_false(356) => 'f'.
		q(nt(356), (t(34)));
		//       option_value_false(356) => 'f' 'a' 'l' 's' 'e'.
		q(nt(356), (t(34)+t(26)+t(27)+t(43)+t(28)));
		//       option_value_false(356) => 'o' 'f' 'f'.
		q(nt(356), (t(41)+t(34)+t(34)));
		//       option_value_false(356) => '0'.
		q(nt(356), (t(37)));
		//       option_value_false(356) => 'n'.
		q(nt(356), (t(49)));
		//       option_value_false(356) => 'n' 'o'.
		q(nt(356), (t(49)+t(41)));
		//       severity(357)        => error_sym(358).
		q(nt(357), (nt(358)));
		//       severity(357)        => debug_sym(359).
		q(nt(357), (nt(359)));
		//       severity(357)        => trace_sym(360).
		q(nt(357), (nt(360)));
		//       severity(357)        => info_sym(361).
		q(nt(357), (nt(361)));
		//       error_sym(358)       => 'e'.
		q(nt(358), (t(28)));
		//       error_sym(358)       => 'e' 'r' 'r' 'o' 'r'.
		q(nt(358), (t(28)+t(45)+t(45)+t(41)+t(45)));
		//       info_sym(361)        => 'i'.
		q(nt(361), (t(39)));
		//       info_sym(361)        => 'i' 'n' 'f' 'o'.
		q(nt(361), (t(39)+t(49)+t(34)+t(41)));
		//       debug_sym(359)       => 'd'.
		q(nt(359), (t(53)));
		//       debug_sym(359)       => 'd' 'e' 'b' 'u' 'g'.
		q(nt(359), (t(53)+t(28)+t(30)+t(44)+t(50)));
		//       trace_sym(360)       => 't'.
		q(nt(360), (t(52)));
		//       trace_sym(360)       => 't' 'r' 'a' 'c' 'e'.
		q(nt(360), (t(52)+t(45)+t(26)+t(47)+t(28)));
		return q;
	}
};

#endif // __TAU_PARSER_H__
