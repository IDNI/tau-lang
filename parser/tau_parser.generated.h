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
		g(nts, load_prods(), nt(250), cc, load_grammar_opts()),
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
	sptree_type shape(forest_type* f, const node_type& n) {
		idni::tree_shaping_options opt;
		opt.to_trim = g.opt.to_trim;
		opt.to_trim_children = g.opt.to_trim_children;
		opt.trim_terminals = g.opt.trim_terminals;
		opt.to_inline = g.opt.to_inline;
		opt.inline_char_classes = g.opt.inline_char_classes;
		return f->get_shaped_tree(n, opt);
	}
	sptree_type parse_and_shape(const char_type* data, size_t size,
		const node_type& n, parse_options po = {})
	{
		return shape(p.parse(data, size, po).get(), n);
	}
	sptree_type parse_and_shape(const char_type* data, size_t size,
		parse_options po = {})
	{
		auto f = p.parse(data, size, po);
		return shape(f.get(), f->root());
	}
	sptree_type parse_and_shape(std::basic_istream<char_type>& is,
		const node_type& n, parse_options po = {})
	{
		return shape(p.parse(is, po).get(), n);
	}
	sptree_type parse_and_shape(std::basic_istream<char_type>& is,
		parse_options po = {})
	{
		auto f = p.parse(is, po);
		return shape(f.get(), f->root());
	}
	sptree_type parse_and_shape(const std::string& fn,
		const node_type& n, parse_options po = {})
	{
		return shape(p.parse(fn, po).get(), n);
	}
	sptree_type parse_and_shape(const std::string& fn,
		parse_options po = {})
	{
		auto f = p.parse(fn, po);
		return shape(f.get(), f->root());
	}
#ifndef WIN32
	sptree_type parse_and_shape(int fd, const node_type& n, parse_options po = {})
	{
		return shape(p.parse(fd, po).get(), n);
	}
	sptree_type parse_and_shape(int fd, parse_options po = {})
	{
		auto f = p.parse(fd, po);
		return shape(f.get(), f->root());
	}
#endif //WIN32
	bool found(size_t start = SIZE_MAX) { return p.found(start); }
	typename parser_type::error get_error() { return p.get_error(); }
	enum nonterminal {
		nul, eof, space, digit, xdigit, alpha, alnum, punct, printable, _, 
		__, bf, bf_parenthesis, wff, wff_parenthesis, tau, tau_parenthesis, comment, __E_comment_0, __E_comment_1, 
		__E_comment_2, __E____3, __E___4, dot, open_parenthesis, close_parenthesis, open_bracket, close_bracket, open_brace, close_brace, 
		minus, colon, semicolon, less, comma, apostrophe, quote, tau_def, tau_builder_def, tau_and_sym, 
		tau_or_sym, tau_neg_sym, wff_def, wff_builder_def, wff_and_sym, wff_or_sym, wff_xor_sym, wff_conditional_sym, wff_neg_sym, wff_imply_sym, 
		wff_equiv_sym, wff_all_sym, wff_ex_sym, wff_ball_sym, wff_bex_sym, wff_t, wff_f, bf_def, bf_builder_def, bf_and_sym, 
		bf_or_sym, bf_xor_sym, bf_neg_sym, bf_equality_sym, bf_nequality_sym, bf_less_sym, bf_less_equal_sym, bf_not_less_equal_sym, bf_greater_sym, bf_all_sym, 
		bf_ex_sym, bf_splitter_sym, bf_t, bf_f, chars, __E_chars_5, __E_chars_6, digits, __E_digits_7, sym, 
		offsets, offset, __E_offsets_8, __E_offsets_9, num, capture, shift, __E_offset_10, variable, io_var, 
		__E_shift_11, __E___E_shift_11_12, charvar, in, out, in_var_name, out_var_name, bool_variable, __E_charvar_13, __E_charvar_14, 
		__E_charvar_15, capture_var, var, tau_rule, tau_matcher, tau_body, tau_collapse_positives_cb, tau_positives_upwards_cb, tau_rec_relation, tau_ref, 
		__E_tau_ref_16, tau_ref_args, __E_tau_ref_args_17, tau_ref_arg, __E_tau_ref_args_18, tau_or, tau_and, tau_neg, tau_wff, wff_rule, 
		wff_matcher, wff_body, bf_eq_cb, bf_neq_cb, wff_has_clashing_subformulas_cb, wff_has_subformula_cb, wff_remove_existential_cb, wff_remove_bexistential_cb, wff_remove_buniversal_cb, wff_rec_relation, 
		wff_ref, wff_conditional, wff_ball, wff_bex, wff_all, wff_ex, wff_imply, wff_equiv, wff_or, wff_and, 
		wff_xor, wff_neg, bf_interval, bf_neq, bf_eq, bf_not_less_equal, bf_greater, bf_less_equal, bf_less, __E_wff_ref_19, 
		wff_ref_args, __E_wff_ref_args_20, wff_ref_arg, __E_wff_ref_args_21, __E_wff_and_22, __E_wff_all_23, __E_wff_ex_24, __E_wff_ball_25, __E_wff_bex_26, bf_rule, 
		bf_matcher, bf_body, bf_is_zero_cb, bf_is_one_cb, bf_has_subformula_cb, bf_remove_funiversal_cb, bf_remove_fexistential_cb, bf_and_cb, bf_or_cb, bf_xor_cb, 
		bf_neg_cb, bf_splitter, bf_ref, bf_all, bf_ex, bf_or, bf_and, bf_xor, bf_neg, bf_constant, 
		bf_rec_relation, __E_bf_ref_27, bf_ref_args, __E_bf_ref_args_28, bf_ref_arg, __E_bf_ref_args_29, __E_bf_and_30, __E_bf_all_31, __E_bf_ex_32, constant, 
		binding, source_binding, named_binding, type, source, source0, __E_source_33, __E_source_34, bf_and_cb_sym, bf_cb_arg, 
		bf_or_cb_sym, bf_xor_cb_sym, bf_neg_cb_sym, bf_eq_cb_sym, wff_cb_arg, bf_neq_cb_sym, bf_is_zero_cb_sym, bf_is_one_cb_sym, bf_remove_funiversal_cb_sym, bf_remove_fexistential_cb_sym, 
		wff_remove_existential_cb_sym, wff_remove_bexistential_cb_sym, wff_remove_buniversal_cb_sym, wff_has_clashing_subformulas_cb_sym, bf_has_subformula_cb_sym, wff_has_subformula_cb_sym, __E_tau_collapse_positives_cb_35, tau_collapse_positives_cb_sym, tau_cb_arg, __E_tau_collapse_positives_cb_36, 
		tau_positives_upwards_cb_sym, inputs, input, __E_inputs_37, __E_inputs_38, builder, builder_head, builder_body, __E_builder_head_39, __E_builder_head_40, 
		bf_builder_body, wff_builder_body, tau_builder_body, library, rules, __E_rules_41, rule, __E_rules_42, nso_rr, nso_rec_relations, 
		nso_main, __E_nso_rec_relations_43, nso_rec_relation, __E_nso_rec_relations_44, gssotc_rr, gssotc_rec_relations, gssotc_main, __E_gssotc_rec_relations_45, gssotc_rec_relation, __E_gssotc_rec_relations_46, 
		start, __E_start_47, cli, cli_command, __E_cli_48, __E_cli_49, help_cmd, version_cmd, quit_cmd, clear_cmd, 
		get_cmd, set_cmd, toggle_cmd, list_outputs_cmd, clear_outputs_cmd, print_output_cmd, file, normalize_cmd, execute_cmd, bf_dnf_cmd, 
		bf_cnf_cmd, bf_anf_cmd, bf_nnf_cmd, bf_pnf_cmd, bf_mnf_cmd, bf_substitute, wff_substitute, bf_instantiate, wff_instantiate, wff_onf_cmd, 
		wff_dnf_cmd, wff_cnf_cmd, wff_anf_cmd, wff_nnf_cmd, wff_pnf_cmd, wff_mnf_cmd, def_rule_cmd, def_list_cmd, def_del_cmd, def_clear_cmd, 
		normalize_cmd_sym, normalize_cmd_arg, output, wff_selection, execute_cmd_sym, execute_cmd_arg, file_sym, file_path, bf_instantiate_cmd, instantiate_cmd_sym, 
		__E_bf_instantiate_cmd_50, bf_selection, __E_bf_instantiate_cmd_51, wff_instantiate_cmd, __E_wff_instantiate_cmd_52, __E_wff_instantiate_cmd_53, bf_substitute_cmd, substitute_cmd_sym, __E_bf_substitute_cmd_54, __E_bf_substitute_cmd_55, 
		__E_bf_substitute_cmd_56, wff_substitute_cmd, substitutecmd__sym, __E_wff_substitute_cmd_57, __E_wff_substitute_cmd_58, __E_wff_substitute_cmd_59, dnf_cmd_sym, __E_bf_dnf_cmd_60, cnf_cmd_sym, __E_bf_cnf_cmd_61, 
		anf_cmd_sym, __E_bf_anf_cmd_62, nnf_cmd_sym, __E_bf_nnf_cmd_63, pnf_cmd_sym, __E_bf_pnf_cmd_64, mnf_cmd_sym, __E_bf_mnf_cmd_65, onf_cmd_sym, __E_wff_onf_cmd_66, 
		__E_wff_dnf_cmd_67, __E_wff_cnf_cmd_68, __E_wff_anf_cmd_69, __E_wff_nnf_cmd_70, __E_wff_pnf_cmd_71, __E_wff_mnf_cmd_72, def_cmd_sym, def_rule_cmd_arg, __E_def_rule_cmd_arg_73, def_cmd_list, 
		def_cmd_del, def_cmf_clear, help_cmd_sym, __E_help_cmd_74, cli_cmd_sym, version_cmd_sym, quit_cmd_sym, clear_cmd_sym, selection_sym, bf_var_selection, 
		wff_var_selection, absolute_output, relative_output, __E_absolute_output_75, absolute_output_sym, output_sym, output_id, relative_output_sym, __E_clear_outputs_cmd_76, clear_sym, 
		file_cmd_sym, get_cmd_sym, set_cmd_sym, toggle_cmd_sym, instantiate_sym, substitute_sym, __E_get_cmd_77, option, __E_set_cmd_78, option_value, 
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
		'x', 'b', 'o', '_', 'T', 'F', '+', 'f', 'S', '1', 
		'0', '$', 'i', 'h', 's', 'u', 'r', 'm', 'c', 'w', 
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
			"", "eof", "space", "digit", "xdigit", "alpha", "alnum", "punct", "printable", "_", 
			"__", "bf", "bf_parenthesis", "wff", "wff_parenthesis", "tau", "tau_parenthesis", "comment", "__E_comment_0", "__E_comment_1", 
			"__E_comment_2", "__E____3", "__E___4", "dot", "open_parenthesis", "close_parenthesis", "open_bracket", "close_bracket", "open_brace", "close_brace", 
			"minus", "colon", "semicolon", "less", "comma", "apostrophe", "quote", "tau_def", "tau_builder_def", "tau_and_sym", 
			"tau_or_sym", "tau_neg_sym", "wff_def", "wff_builder_def", "wff_and_sym", "wff_or_sym", "wff_xor_sym", "wff_conditional_sym", "wff_neg_sym", "wff_imply_sym", 
			"wff_equiv_sym", "wff_all_sym", "wff_ex_sym", "wff_ball_sym", "wff_bex_sym", "wff_t", "wff_f", "bf_def", "bf_builder_def", "bf_and_sym", 
			"bf_or_sym", "bf_xor_sym", "bf_neg_sym", "bf_equality_sym", "bf_nequality_sym", "bf_less_sym", "bf_less_equal_sym", "bf_not_less_equal_sym", "bf_greater_sym", "bf_all_sym", 
			"bf_ex_sym", "bf_splitter_sym", "bf_t", "bf_f", "chars", "__E_chars_5", "__E_chars_6", "digits", "__E_digits_7", "sym", 
			"offsets", "offset", "__E_offsets_8", "__E_offsets_9", "num", "capture", "shift", "__E_offset_10", "variable", "io_var", 
			"__E_shift_11", "__E___E_shift_11_12", "charvar", "in", "out", "in_var_name", "out_var_name", "bool_variable", "__E_charvar_13", "__E_charvar_14", 
			"__E_charvar_15", "capture_var", "var", "tau_rule", "tau_matcher", "tau_body", "tau_collapse_positives_cb", "tau_positives_upwards_cb", "tau_rec_relation", "tau_ref", 
			"__E_tau_ref_16", "tau_ref_args", "__E_tau_ref_args_17", "tau_ref_arg", "__E_tau_ref_args_18", "tau_or", "tau_and", "tau_neg", "tau_wff", "wff_rule", 
			"wff_matcher", "wff_body", "bf_eq_cb", "bf_neq_cb", "wff_has_clashing_subformulas_cb", "wff_has_subformula_cb", "wff_remove_existential_cb", "wff_remove_bexistential_cb", "wff_remove_buniversal_cb", "wff_rec_relation", 
			"wff_ref", "wff_conditional", "wff_ball", "wff_bex", "wff_all", "wff_ex", "wff_imply", "wff_equiv", "wff_or", "wff_and", 
			"wff_xor", "wff_neg", "bf_interval", "bf_neq", "bf_eq", "bf_not_less_equal", "bf_greater", "bf_less_equal", "bf_less", "__E_wff_ref_19", 
			"wff_ref_args", "__E_wff_ref_args_20", "wff_ref_arg", "__E_wff_ref_args_21", "__E_wff_and_22", "__E_wff_all_23", "__E_wff_ex_24", "__E_wff_ball_25", "__E_wff_bex_26", "bf_rule", 
			"bf_matcher", "bf_body", "bf_is_zero_cb", "bf_is_one_cb", "bf_has_subformula_cb", "bf_remove_funiversal_cb", "bf_remove_fexistential_cb", "bf_and_cb", "bf_or_cb", "bf_xor_cb", 
			"bf_neg_cb", "bf_splitter", "bf_ref", "bf_all", "bf_ex", "bf_or", "bf_and", "bf_xor", "bf_neg", "bf_constant", 
			"bf_rec_relation", "__E_bf_ref_27", "bf_ref_args", "__E_bf_ref_args_28", "bf_ref_arg", "__E_bf_ref_args_29", "__E_bf_and_30", "__E_bf_all_31", "__E_bf_ex_32", "constant", 
			"binding", "source_binding", "named_binding", "type", "source", "source0", "__E_source_33", "__E_source_34", "bf_and_cb_sym", "bf_cb_arg", 
			"bf_or_cb_sym", "bf_xor_cb_sym", "bf_neg_cb_sym", "bf_eq_cb_sym", "wff_cb_arg", "bf_neq_cb_sym", "bf_is_zero_cb_sym", "bf_is_one_cb_sym", "bf_remove_funiversal_cb_sym", "bf_remove_fexistential_cb_sym", 
			"wff_remove_existential_cb_sym", "wff_remove_bexistential_cb_sym", "wff_remove_buniversal_cb_sym", "wff_has_clashing_subformulas_cb_sym", "bf_has_subformula_cb_sym", "wff_has_subformula_cb_sym", "__E_tau_collapse_positives_cb_35", "tau_collapse_positives_cb_sym", "tau_cb_arg", "__E_tau_collapse_positives_cb_36", 
			"tau_positives_upwards_cb_sym", "inputs", "input", "__E_inputs_37", "__E_inputs_38", "builder", "builder_head", "builder_body", "__E_builder_head_39", "__E_builder_head_40", 
			"bf_builder_body", "wff_builder_body", "tau_builder_body", "library", "rules", "__E_rules_41", "rule", "__E_rules_42", "nso_rr", "nso_rec_relations", 
			"nso_main", "__E_nso_rec_relations_43", "nso_rec_relation", "__E_nso_rec_relations_44", "gssotc_rr", "gssotc_rec_relations", "gssotc_main", "__E_gssotc_rec_relations_45", "gssotc_rec_relation", "__E_gssotc_rec_relations_46", 
			"start", "__E_start_47", "cli", "cli_command", "__E_cli_48", "__E_cli_49", "help_cmd", "version_cmd", "quit_cmd", "clear_cmd", 
			"get_cmd", "set_cmd", "toggle_cmd", "list_outputs_cmd", "clear_outputs_cmd", "print_output_cmd", "file", "normalize_cmd", "execute_cmd", "bf_dnf_cmd", 
			"bf_cnf_cmd", "bf_anf_cmd", "bf_nnf_cmd", "bf_pnf_cmd", "bf_mnf_cmd", "bf_substitute", "wff_substitute", "bf_instantiate", "wff_instantiate", "wff_onf_cmd", 
			"wff_dnf_cmd", "wff_cnf_cmd", "wff_anf_cmd", "wff_nnf_cmd", "wff_pnf_cmd", "wff_mnf_cmd", "def_rule_cmd", "def_list_cmd", "def_del_cmd", "def_clear_cmd", 
			"normalize_cmd_sym", "normalize_cmd_arg", "output", "wff_selection", "execute_cmd_sym", "execute_cmd_arg", "file_sym", "file_path", "bf_instantiate_cmd", "instantiate_cmd_sym", 
			"__E_bf_instantiate_cmd_50", "bf_selection", "__E_bf_instantiate_cmd_51", "wff_instantiate_cmd", "__E_wff_instantiate_cmd_52", "__E_wff_instantiate_cmd_53", "bf_substitute_cmd", "substitute_cmd_sym", "__E_bf_substitute_cmd_54", "__E_bf_substitute_cmd_55", 
			"__E_bf_substitute_cmd_56", "wff_substitute_cmd", "substitutecmd__sym", "__E_wff_substitute_cmd_57", "__E_wff_substitute_cmd_58", "__E_wff_substitute_cmd_59", "dnf_cmd_sym", "__E_bf_dnf_cmd_60", "cnf_cmd_sym", "__E_bf_cnf_cmd_61", 
			"anf_cmd_sym", "__E_bf_anf_cmd_62", "nnf_cmd_sym", "__E_bf_nnf_cmd_63", "pnf_cmd_sym", "__E_bf_pnf_cmd_64", "mnf_cmd_sym", "__E_bf_mnf_cmd_65", "onf_cmd_sym", "__E_wff_onf_cmd_66", 
			"__E_wff_dnf_cmd_67", "__E_wff_cnf_cmd_68", "__E_wff_anf_cmd_69", "__E_wff_nnf_cmd_70", "__E_wff_pnf_cmd_71", "__E_wff_mnf_cmd_72", "def_cmd_sym", "def_rule_cmd_arg", "__E_def_rule_cmd_arg_73", "def_cmd_list", 
			"def_cmd_del", "def_cmf_clear", "help_cmd_sym", "__E_help_cmd_74", "cli_cmd_sym", "version_cmd_sym", "quit_cmd_sym", "clear_cmd_sym", "selection_sym", "bf_var_selection", 
			"wff_var_selection", "absolute_output", "relative_output", "__E_absolute_output_75", "absolute_output_sym", "output_sym", "output_id", "relative_output_sym", "__E_clear_outputs_cmd_76", "clear_sym", 
			"file_cmd_sym", "get_cmd_sym", "set_cmd_sym", "toggle_cmd_sym", "instantiate_sym", "substitute_sym", "__E_get_cmd_77", "option", "__E_set_cmd_78", "option_value", 
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
		o.to_trim = {
			9, 10
		};
		o.to_inline = {
			{ 11, 12, 11 },
			{ 13, 14, 13 },
			{ 15, 16, 15 }
		};
		return o;
	}
	options load_opts() {
		options o;
		return o;
	}
	idni::prods<char_type, terminal_type> load_prods() {
		idni::prods<char_type, terminal_type> q,
			nul(symbol_type{});
		//       __E_comment_0(18)    => printable(8).
		q(nt(18), (nt(8)));
		//       __E_comment_0(18)    => '\t'.
		q(nt(18), (t(1)));
		//       __E_comment_1(19)    => null.
		q(nt(19), (nul));
		//       __E_comment_1(19)    => __E_comment_0(18) __E_comment_1(19).
		q(nt(19), (nt(18)+nt(19)));
		//       __E_comment_2(20)    => '\n'.
		q(nt(20), (t(2)));
		//       __E_comment_2(20)    => '\r'.
		q(nt(20), (t(3)));
		//       __E_comment_2(20)    => eof(1).
		q(nt(20), (nt(1)));
		//       comment(17)          => '#' __E_comment_1(19) __E_comment_2(20).
		q(nt(17), (t(4)+nt(19)+nt(20)));
		//       __E____3(21)         => space(2).
		q(nt(21), (nt(2)));
		//       __E____3(21)         => comment(17).
		q(nt(21), (nt(17)));
		//       __(10)               => __E____3(21) _(9).
		q(nt(10), (nt(21)+nt(9)));
		//       __E___4(22)          => __(10).
		q(nt(22), (nt(10)));
		//       __E___4(22)          => null.
		q(nt(22), (nul));
		//       _(9)                 => __E___4(22).
		q(nt(9), (nt(22)));
		//       dot(23)              => '.'.
		q(nt(23), (t(5)));
		//       open_parenthesis(24) => '('.
		q(nt(24), (t(6)));
		//       close_parenthesis(25) => ')'.
		q(nt(25), (t(7)));
		//       open_bracket(26)     => '['.
		q(nt(26), (t(8)));
		//       close_bracket(27)    => ']'.
		q(nt(27), (t(9)));
		//       open_brace(28)       => '{'.
		q(nt(28), (t(10)));
		//       close_brace(29)      => '}'.
		q(nt(29), (t(11)));
		//       minus(30)            => '-'.
		q(nt(30), (t(12)));
		//       colon(31)            => ':'.
		q(nt(31), (t(13)));
		//       semicolon(32)        => ';'.
		q(nt(32), (t(14)));
		//       less(33)             => '<'.
		q(nt(33), (t(15)));
		//       comma(34)            => ','.
		q(nt(34), (t(16)));
		//       apostrophe(35)       => '\''.
		q(nt(35), (t(17)));
		//       quote(36)            => '"'.
		q(nt(36), (t(18)));
		//       tau_def(37)          => ':' ':' ':' '='.
		q(nt(37), (t(13)+t(13)+t(13)+t(19)));
		//       tau_builder_def(38)  => '=' ':' ':' ':'.
		q(nt(38), (t(19)+t(13)+t(13)+t(13)));
		//       tau_and_sym(39)      => '&' '&' '&'.
		q(nt(39), (t(20)+t(20)+t(20)));
		//       tau_or_sym(40)       => '|' '|' '|'.
		q(nt(40), (t(21)+t(21)+t(21)));
		//       tau_neg_sym(41)      => '!' '!' '!'.
		q(nt(41), (t(22)+t(22)+t(22)));
		//       wff_def(42)          => ':' ':' '='.
		q(nt(42), (t(13)+t(13)+t(19)));
		//       wff_builder_def(43)  => '=' ':' ':'.
		q(nt(43), (t(19)+t(13)+t(13)));
		//       wff_and_sym(44)      => '&' '&'.
		q(nt(44), (t(20)+t(20)));
		//       wff_or_sym(45)       => '|' '|'.
		q(nt(45), (t(21)+t(21)));
		//       wff_xor_sym(46)      => '^'.
		q(nt(46), (t(23)));
		//       wff_conditional_sym(47) => '?'.
		q(nt(47), (t(24)));
		//       wff_neg_sym(48)      => '!'.
		q(nt(48), (t(22)));
		//       wff_imply_sym(49)    => '-' '>'.
		q(nt(49), (t(12)+t(25)));
		//       wff_equiv_sym(50)    => '<' '-' '>'.
		q(nt(50), (t(15)+t(12)+t(25)));
		//       wff_all_sym(51)      => 'a' 'l' 'l'.
		q(nt(51), (t(26)+t(27)+t(27)));
		//       wff_ex_sym(52)       => 'e' 'x'.
		q(nt(52), (t(28)+t(29)));
		//       wff_ball_sym(53)     => 'b' 'o' 'o' 'l' '_' 'a' 'l' 'l'.
		q(nt(53), (t(30)+t(31)+t(31)+t(27)+t(32)+t(26)+t(27)+t(27)));
		//       wff_bex_sym(54)      => 'b' 'o' 'o' 'l' '_' 'e' 'x'.
		q(nt(54), (t(30)+t(31)+t(31)+t(27)+t(32)+t(28)+t(29)));
		//       wff_t(55)            => 'T'.
		q(nt(55), (t(33)));
		//       wff_f(56)            => 'F'.
		q(nt(56), (t(34)));
		//       bf_def(57)           => ':' '='.
		q(nt(57), (t(13)+t(19)));
		//       bf_builder_def(58)   => '=' ':'.
		q(nt(58), (t(19)+t(13)));
		//       bf_and_sym(59)       => '&'.
		q(nt(59), (t(20)));
		//       bf_or_sym(60)        => '|'.
		q(nt(60), (t(21)));
		//       bf_xor_sym(61)       => '+'.
		q(nt(61), (t(35)));
		//       bf_neg_sym(62)       => apostrophe(35).
		q(nt(62), (nt(35)));
		//       bf_equality_sym(63)  => '='.
		q(nt(63), (t(19)));
		//       bf_nequality_sym(64) => '!' '='.
		q(nt(64), (t(22)+t(19)));
		//       bf_less_sym(65)      => '<'.
		q(nt(65), (t(15)));
		//       bf_less_equal_sym(66) => '<' '='.
		q(nt(66), (t(15)+t(19)));
		//       bf_not_less_equal_sym(67) => '!' '<' '='.
		q(nt(67), (t(22)+t(15)+t(19)));
		//       bf_greater_sym(68)   => '>'.
		q(nt(68), (t(25)));
		//       bf_all_sym(69)       => 'f' 'a' 'l' 'l'.
		q(nt(69), (t(36)+t(26)+t(27)+t(27)));
		//       bf_ex_sym(70)        => 'f' 'e' 'x'.
		q(nt(70), (t(36)+t(28)+t(29)));
		//       bf_splitter_sym(71)  => 'S'.
		q(nt(71), (t(37)));
		//       bf_t(72)             => '1'.
		q(nt(72), (t(38)));
		//       bf_f(73)             => '0'.
		q(nt(73), (t(39)));
		//       __E_chars_5(75)      => alnum(6).
		q(nt(75), (nt(6)));
		//       __E_chars_6(76)      => null.
		q(nt(76), (nul));
		//       __E_chars_6(76)      => __E_chars_5(75) __E_chars_6(76).
		q(nt(76), (nt(75)+nt(76)));
		//       chars(74)            => alpha(5) __E_chars_6(76).
		q(nt(74), (nt(5)+nt(76)));
		//       __E_digits_7(78)     => digit(3).
		q(nt(78), (nt(3)));
		//       __E_digits_7(78)     => digit(3) __E_digits_7(78).
		q(nt(78), (nt(3)+nt(78)));
		//       digits(77)           => __E_digits_7(78).
		q(nt(77), (nt(78)));
		//       sym(79)              => chars(74).
		q(nt(79), (nt(74)));
		//       __E_offsets_8(82)    => _(9) comma(34) _(9) offset(81).
		q(nt(82), (nt(9)+nt(34)+nt(9)+nt(81)));
		//       __E_offsets_9(83)    => null.
		q(nt(83), (nul));
		//       __E_offsets_9(83)    => __E_offsets_8(82) __E_offsets_9(83).
		q(nt(83), (nt(82)+nt(83)));
		//       offsets(80)          => open_bracket(26) _(9) offset(81) __E_offsets_9(83) _(9) close_bracket(27).
		q(nt(80), (nt(26)+nt(9)+nt(81)+nt(83)+nt(9)+nt(27)));
		//       offset(81)           => num(84).
		q(nt(81), (nt(84)));
		//       offset(81)           => capture(85).
		q(nt(81), (nt(85)));
		//       offset(81)           => shift(86).
		q(nt(81), (nt(86)));
		//       __N_0(382)           => io_var(89).
		q(nt(382), (nt(89)));
		//       __E_offset_10(87)    => variable(88) & ~( __N_0(382) ).	 # conjunctive
		q(nt(87), (nt(88)) & ~(nt(382)));
		//       offset(81)           => __E_offset_10(87).
		q(nt(81), (nt(87)));
		//       __E_shift_11(90)     => capture(85).
		q(nt(90), (nt(85)));
		//       __N_1(383)           => io_var(89).
		q(nt(383), (nt(89)));
		//       __E___E_shift_11_12(91) => variable(88) & ~( __N_1(383) ).	 # conjunctive
		q(nt(91), (nt(88)) & ~(nt(383)));
		//       __E_shift_11(90)     => __E___E_shift_11_12(91).
		q(nt(90), (nt(91)));
		//       shift(86)            => __E_shift_11(90) _(9) minus(30) _(9) num(84).
		q(nt(86), (nt(90)+nt(9)+nt(30)+nt(9)+nt(84)));
		//       num(84)              => digits(77).
		q(nt(84), (nt(77)));
		//       variable(88)         => charvar(92).
		q(nt(88), (nt(92)));
		//       variable(88)         => io_var(89).
		q(nt(88), (nt(89)));
		//       io_var(89)           => in(93).
		q(nt(89), (nt(93)));
		//       io_var(89)           => out(94).
		q(nt(89), (nt(94)));
		//       in(93)               => in_var_name(95) open_bracket(26) offset(81) close_bracket(27).
		q(nt(93), (nt(95)+nt(26)+nt(81)+nt(27)));
		//       out(94)              => out_var_name(96) open_bracket(26) offset(81) close_bracket(27).
		q(nt(94), (nt(96)+nt(26)+nt(81)+nt(27)));
		//       bool_variable(97)    => '?' charvar(92).
		q(nt(97), (t(24)+nt(92)));
		//       __N_2(384)           => 'F'.
		q(nt(384), (t(34)));
		//       __N_3(385)           => 'T'.
		q(nt(385), (t(33)));
		//       __E_charvar_13(98)   => ~( __N_2(384) ) & ~( __N_3(385) ) & alpha(5).	 # conjunctive
		q(nt(98), ~(nt(384)) & ~(nt(385)) & (nt(5)));
		//       __E_charvar_14(99)   => digit(3).
		q(nt(99), (nt(3)));
		//       __E_charvar_15(100)  => null.
		q(nt(100), (nul));
		//       __E_charvar_15(100)  => __E_charvar_14(99) __E_charvar_15(100).
		q(nt(100), (nt(99)+nt(100)));
		//       charvar(92)          => __E_charvar_13(98) __E_charvar_15(100).
		q(nt(92), (nt(98)+nt(100)));
		//       capture(85)          => capture_var(101).
		q(nt(85), (nt(101)));
		//       capture_var(101)     => '$' chars(74).
		q(nt(101), (t(40)+nt(74)));
		//       var(102)             => chars(74).
		q(nt(102), (nt(74)));
		//       in_var_name(95)      => 'i' '_' chars(74).
		q(nt(95), (t(41)+t(32)+nt(74)));
		//       out_var_name(96)     => 'o' '_' chars(74).
		q(nt(96), (t(31)+t(32)+nt(74)));
		//       tau_rule(103)        => tau_matcher(104) _(9) tau_def(37) _(9) tau_body(105) _(9) dot(23).
		q(nt(103), (nt(104)+nt(9)+nt(37)+nt(9)+nt(105)+nt(9)+nt(23)));
		//       tau_matcher(104)     => tau(15).
		q(nt(104), (nt(15)));
		//       tau_body(105)        => tau(15).
		q(nt(105), (nt(15)));
		//       tau_body(105)        => tau_collapse_positives_cb(106).
		q(nt(105), (nt(106)));
		//       tau_body(105)        => tau_positives_upwards_cb(107).
		q(nt(105), (nt(107)));
		//       tau_rec_relation(108) => tau_ref(109) _(9) tau_def(37) _(9) tau(15) _(9) dot(23).
		q(nt(108), (nt(109)+nt(9)+nt(37)+nt(9)+nt(15)+nt(9)+nt(23)));
		//       __E_tau_ref_16(110)  => offsets(80).
		q(nt(110), (nt(80)));
		//       __E_tau_ref_16(110)  => null.
		q(nt(110), (nul));
		//       tau_ref(109)         => sym(79) _(9) __E_tau_ref_16(110) _(9) tau_ref_args(111).
		q(nt(109), (nt(79)+nt(9)+nt(110)+nt(9)+nt(111)));
		//       __E_tau_ref_args_17(112) => _(9) tau_ref_arg(113).
		q(nt(112), (nt(9)+nt(113)));
		//       __E_tau_ref_args_18(114) => null.
		q(nt(114), (nul));
		//       __E_tau_ref_args_18(114) => __E_tau_ref_args_17(112) __E_tau_ref_args_18(114).
		q(nt(114), (nt(112)+nt(114)));
		//       tau_ref_args(111)    => open_parenthesis(24) __E_tau_ref_args_18(114) _(9) close_parenthesis(25).
		q(nt(111), (nt(24)+nt(114)+nt(9)+nt(25)));
		//       tau_ref_arg(113)     => bf(11).
		q(nt(113), (nt(11)));
		//       tau_ref_arg(113)     => capture(85).
		q(nt(113), (nt(85)));
		//       tau_ref_arg(113)     => variable(88).
		q(nt(113), (nt(88)));
		//       tau(15)              => tau_ref(109).
		q(nt(15), (nt(109)));
		//       tau(15)              => tau_or(115).
		q(nt(15), (nt(115)));
		//       tau(15)              => tau_and(116).
		q(nt(15), (nt(116)));
		//       tau(15)              => tau_neg(117).
		q(nt(15), (nt(117)));
		//       tau(15)              => tau_wff(118).
		q(nt(15), (nt(118)));
		//       tau(15)              => capture(85).
		q(nt(15), (nt(85)));
		//       tau(15)              => tau_parenthesis(16).
		q(nt(15), (nt(16)));
		//       tau_parenthesis(16)  => open_parenthesis(24) _(9) tau(15) _(9) close_parenthesis(25).
		q(nt(16), (nt(24)+nt(9)+nt(15)+nt(9)+nt(25)));
		//       tau_and(116)         => tau(15) _(9) tau_and_sym(39) _(9) tau(15).
		q(nt(116), (nt(15)+nt(9)+nt(39)+nt(9)+nt(15)));
		//       tau_or(115)          => tau(15) _(9) tau_or_sym(40) _(9) tau(15).
		q(nt(115), (nt(15)+nt(9)+nt(40)+nt(9)+nt(15)));
		//       tau_neg(117)         => tau_neg_sym(41) _(9) tau(15).
		q(nt(117), (nt(41)+nt(9)+nt(15)));
		//       tau_wff(118)         => open_brace(28) _(9) wff(13) _(9) close_brace(29).
		q(nt(118), (nt(28)+nt(9)+nt(13)+nt(9)+nt(29)));
		//       wff_rule(119)        => wff_matcher(120) _(9) wff_def(42) _(9) wff_body(121) _(9) dot(23).
		q(nt(119), (nt(120)+nt(9)+nt(42)+nt(9)+nt(121)+nt(9)+nt(23)));
		//       wff_matcher(120)     => wff(13).
		q(nt(120), (nt(13)));
		//       wff_body(121)        => bf_eq_cb(122).
		q(nt(121), (nt(122)));
		//       wff_body(121)        => bf_neq_cb(123).
		q(nt(121), (nt(123)));
		//       wff_body(121)        => wff_has_clashing_subformulas_cb(124).
		q(nt(121), (nt(124)));
		//       wff_body(121)        => wff_has_subformula_cb(125).
		q(nt(121), (nt(125)));
		//       wff_body(121)        => wff_remove_existential_cb(126).
		q(nt(121), (nt(126)));
		//       wff_body(121)        => wff_remove_bexistential_cb(127).
		q(nt(121), (nt(127)));
		//       wff_body(121)        => wff_remove_buniversal_cb(128).
		q(nt(121), (nt(128)));
		//       wff_body(121)        => wff(13).
		q(nt(121), (nt(13)));
		//       wff_rec_relation(129) => wff_ref(130) _(9) wff_def(42) _(9) wff(13) _(9) dot(23).
		q(nt(129), (nt(130)+nt(9)+nt(42)+nt(9)+nt(13)+nt(9)+nt(23)));
		//       wff(13)              => wff_ref(130).
		q(nt(13), (nt(130)));
		//       wff(13)              => wff_conditional(131).
		q(nt(13), (nt(131)));
		//       wff(13)              => wff_ball(132).
		q(nt(13), (nt(132)));
		//       wff(13)              => wff_bex(133).
		q(nt(13), (nt(133)));
		//       wff(13)              => wff_all(134).
		q(nt(13), (nt(134)));
		//       wff(13)              => wff_ex(135).
		q(nt(13), (nt(135)));
		//       wff(13)              => wff_imply(136).
		q(nt(13), (nt(136)));
		//       wff(13)              => wff_equiv(137).
		q(nt(13), (nt(137)));
		//       wff(13)              => wff_or(138).
		q(nt(13), (nt(138)));
		//       wff(13)              => wff_and(139).
		q(nt(13), (nt(139)));
		//       wff(13)              => wff_xor(140).
		q(nt(13), (nt(140)));
		//       wff(13)              => wff_neg(141).
		q(nt(13), (nt(141)));
		//       wff(13)              => wff_t(55).
		q(nt(13), (nt(55)));
		//       wff(13)              => wff_f(56).
		q(nt(13), (nt(56)));
		//       wff(13)              => bf_interval(142).
		q(nt(13), (nt(142)));
		//       wff(13)              => bf_neq(143).
		q(nt(13), (nt(143)));
		//       wff(13)              => bf_eq(144).
		q(nt(13), (nt(144)));
		//       wff(13)              => bf_not_less_equal(145).
		q(nt(13), (nt(145)));
		//       wff(13)              => bf_greater(146).
		q(nt(13), (nt(146)));
		//       wff(13)              => bf_less_equal(147).
		q(nt(13), (nt(147)));
		//       wff(13)              => bf_less(148).
		q(nt(13), (nt(148)));
		//       wff(13)              => capture(85).
		q(nt(13), (nt(85)));
		//       wff(13)              => bool_variable(97).
		q(nt(13), (nt(97)));
		//       wff(13)              => wff_parenthesis(14).
		q(nt(13), (nt(14)));
		//       wff_parenthesis(14)  => open_parenthesis(24) _(9) wff(13) _(9) close_parenthesis(25).
		q(nt(14), (nt(24)+nt(9)+nt(13)+nt(9)+nt(25)));
		//       __E_wff_ref_19(149)  => offsets(80).
		q(nt(149), (nt(80)));
		//       __E_wff_ref_19(149)  => null.
		q(nt(149), (nul));
		//       wff_ref(130)         => sym(79) _(9) __E_wff_ref_19(149) _(9) wff_ref_args(150).
		q(nt(130), (nt(79)+nt(9)+nt(149)+nt(9)+nt(150)));
		//       __E_wff_ref_args_20(151) => _(9) wff_ref_arg(152).
		q(nt(151), (nt(9)+nt(152)));
		//       __E_wff_ref_args_21(153) => null.
		q(nt(153), (nul));
		//       __E_wff_ref_args_21(153) => __E_wff_ref_args_20(151) __E_wff_ref_args_21(153).
		q(nt(153), (nt(151)+nt(153)));
		//       wff_ref_args(150)    => open_parenthesis(24) __E_wff_ref_args_21(153) _(9) close_parenthesis(25).
		q(nt(150), (nt(24)+nt(153)+nt(9)+nt(25)));
		//       wff_ref_arg(152)     => bf(11).
		q(nt(152), (nt(11)));
		//       wff_ref_arg(152)     => capture(85).
		q(nt(152), (nt(85)));
		//       wff_ref_arg(152)     => variable(88).
		q(nt(152), (nt(88)));
		//       __E_wff_and_22(154)  => wff_and_sym(44) _(9).
		q(nt(154), (nt(44)+nt(9)));
		//       __E_wff_and_22(154)  => null.
		q(nt(154), (nul));
		//       wff_and(139)         => wff(13) _(9) __E_wff_and_22(154) wff(13).
		q(nt(139), (nt(13)+nt(9)+nt(154)+nt(13)));
		//       wff_or(138)          => wff(13) _(9) wff_or_sym(45) _(9) wff(13).
		q(nt(138), (nt(13)+nt(9)+nt(45)+nt(9)+nt(13)));
		//       wff_xor(140)         => wff(13) _(9) wff_xor_sym(46) _(9) wff(13).
		q(nt(140), (nt(13)+nt(9)+nt(46)+nt(9)+nt(13)));
		//       wff_conditional(131) => wff(13) _(9) wff_conditional_sym(47) _(9) wff(13) _(9) colon(31) _(9) wff(13).
		q(nt(131), (nt(13)+nt(9)+nt(47)+nt(9)+nt(13)+nt(9)+nt(31)+nt(9)+nt(13)));
		//       wff_neg(141)         => wff_neg_sym(48) _(9) wff(13).
		q(nt(141), (nt(48)+nt(9)+nt(13)));
		//       wff_imply(136)       => wff(13) _(9) wff_imply_sym(49) _(9) wff(13).
		q(nt(136), (nt(13)+nt(9)+nt(49)+nt(9)+nt(13)));
		//       wff_equiv(137)       => wff(13) _(9) wff_equiv_sym(50) _(9) wff(13).
		q(nt(137), (nt(13)+nt(9)+nt(50)+nt(9)+nt(13)));
		//       __E_wff_all_23(155)  => variable(88).
		q(nt(155), (nt(88)));
		//       __E_wff_all_23(155)  => capture(85).
		q(nt(155), (nt(85)));
		//       wff_all(134)         => wff_all_sym(51) __(10) __E_wff_all_23(155) __(10) wff(13).
		q(nt(134), (nt(51)+nt(10)+nt(155)+nt(10)+nt(13)));
		//       __E_wff_ex_24(156)   => variable(88).
		q(nt(156), (nt(88)));
		//       __E_wff_ex_24(156)   => capture(85).
		q(nt(156), (nt(85)));
		//       wff_ex(135)          => wff_ex_sym(52) __(10) __E_wff_ex_24(156) __(10) wff(13).
		q(nt(135), (nt(52)+nt(10)+nt(156)+nt(10)+nt(13)));
		//       __E_wff_ball_25(157) => bool_variable(97).
		q(nt(157), (nt(97)));
		//       __E_wff_ball_25(157) => capture(85).
		q(nt(157), (nt(85)));
		//       wff_ball(132)        => wff_ball_sym(53) __(10) __E_wff_ball_25(157) __(10) wff(13).
		q(nt(132), (nt(53)+nt(10)+nt(157)+nt(10)+nt(13)));
		//       __E_wff_bex_26(158)  => bool_variable(97).
		q(nt(158), (nt(97)));
		//       __E_wff_bex_26(158)  => capture(85).
		q(nt(158), (nt(85)));
		//       wff_bex(133)         => wff_bex_sym(54) __(10) __E_wff_bex_26(158) __(10) wff(13).
		q(nt(133), (nt(54)+nt(10)+nt(158)+nt(10)+nt(13)));
		//       bf_eq(144)           => bf(11) _(9) bf_equality_sym(63) _(9) bf(11).
		q(nt(144), (nt(11)+nt(9)+nt(63)+nt(9)+nt(11)));
		//       bf_neq(143)          => bf(11) _(9) bf_nequality_sym(64) _(9) bf(11).
		q(nt(143), (nt(11)+nt(9)+nt(64)+nt(9)+nt(11)));
		//       bf_less(148)         => bf(11) _(9) bf_less_sym(65) _(9) bf(11).
		q(nt(148), (nt(11)+nt(9)+nt(65)+nt(9)+nt(11)));
		//       bf_less_equal(147)   => bf(11) _(9) bf_less_equal_sym(66) _(9) bf(11).
		q(nt(147), (nt(11)+nt(9)+nt(66)+nt(9)+nt(11)));
		//       bf_not_less_equal(145) => bf(11) _(9) bf_not_less_equal_sym(67) _(9) bf(11).
		q(nt(145), (nt(11)+nt(9)+nt(67)+nt(9)+nt(11)));
		//       bf_greater(146)      => bf(11) _(9) bf_greater_sym(68) _(9) bf(11).
		q(nt(146), (nt(11)+nt(9)+nt(68)+nt(9)+nt(11)));
		//       bf_interval(142)     => bf(11) _(9) bf_less_equal_sym(66) _(9) bf(11) _(9) bf_less_equal_sym(66) _(9) bf(11).
		q(nt(142), (nt(11)+nt(9)+nt(66)+nt(9)+nt(11)+nt(9)+nt(66)+nt(9)+nt(11)));
		//       bf_rule(159)         => bf_matcher(160) _(9) bf_def(57) _(9) bf_body(161) _(9) dot(23).
		q(nt(159), (nt(160)+nt(9)+nt(57)+nt(9)+nt(161)+nt(9)+nt(23)));
		//       bf_matcher(160)      => bf(11).
		q(nt(160), (nt(11)));
		//       bf_body(161)         => bf(11).
		q(nt(161), (nt(11)));
		//       bf_body(161)         => bf_is_zero_cb(162).
		q(nt(161), (nt(162)));
		//       bf_body(161)         => bf_is_one_cb(163).
		q(nt(161), (nt(163)));
		//       bf_body(161)         => bf_has_subformula_cb(164).
		q(nt(161), (nt(164)));
		//       bf_body(161)         => bf_remove_funiversal_cb(165).
		q(nt(161), (nt(165)));
		//       bf_body(161)         => bf_remove_fexistential_cb(166).
		q(nt(161), (nt(166)));
		//       bf_body(161)         => bf_and_cb(167).
		q(nt(161), (nt(167)));
		//       bf_body(161)         => bf_or_cb(168).
		q(nt(161), (nt(168)));
		//       bf_body(161)         => bf_xor_cb(169).
		q(nt(161), (nt(169)));
		//       bf_body(161)         => bf_neg_cb(170).
		q(nt(161), (nt(170)));
		//       bf_body(161)         => bf_not_less_equal(145).
		q(nt(161), (nt(145)));
		//       bf(11)               => bf_splitter(171).
		q(nt(11), (nt(171)));
		//       bf(11)               => bf_ref(172).
		q(nt(11), (nt(172)));
		//       bf(11)               => bf_all(173).
		q(nt(11), (nt(173)));
		//       bf(11)               => bf_ex(174).
		q(nt(11), (nt(174)));
		//       bf(11)               => bf_or(175).
		q(nt(11), (nt(175)));
		//       bf(11)               => bf_and(176).
		q(nt(11), (nt(176)));
		//       bf(11)               => bf_xor(177).
		q(nt(11), (nt(177)));
		//       bf(11)               => bf_neg(178).
		q(nt(11), (nt(178)));
		//       bf(11)               => bf_constant(179).
		q(nt(11), (nt(179)));
		//       bf(11)               => bf_t(72).
		q(nt(11), (nt(72)));
		//       bf(11)               => bf_f(73).
		q(nt(11), (nt(73)));
		//       bf(11)               => capture(85).
		q(nt(11), (nt(85)));
		//       bf(11)               => variable(88).
		q(nt(11), (nt(88)));
		//       bf(11)               => bf_parenthesis(12).
		q(nt(11), (nt(12)));
		//       bf_parenthesis(12)   => open_parenthesis(24) _(9) bf(11) _(9) close_parenthesis(25).
		q(nt(12), (nt(24)+nt(9)+nt(11)+nt(9)+nt(25)));
		//       bf_rec_relation(180) => bf_ref(172) _(9) bf_def(57) _(9) bf(11) _(9) dot(23).
		q(nt(180), (nt(172)+nt(9)+nt(57)+nt(9)+nt(11)+nt(9)+nt(23)));
		//       __E_bf_ref_27(181)   => offsets(80).
		q(nt(181), (nt(80)));
		//       __E_bf_ref_27(181)   => null.
		q(nt(181), (nul));
		//       bf_ref(172)          => sym(79) __E_bf_ref_27(181) bf_ref_args(182).
		q(nt(172), (nt(79)+nt(181)+nt(182)));
		//       __E_bf_ref_args_28(183) => _(9) bf_ref_arg(184).
		q(nt(183), (nt(9)+nt(184)));
		//       __E_bf_ref_args_29(185) => null.
		q(nt(185), (nul));
		//       __E_bf_ref_args_29(185) => __E_bf_ref_args_28(183) __E_bf_ref_args_29(185).
		q(nt(185), (nt(183)+nt(185)));
		//       bf_ref_args(182)     => open_parenthesis(24) __E_bf_ref_args_29(185) _(9) close_parenthesis(25).
		q(nt(182), (nt(24)+nt(185)+nt(9)+nt(25)));
		//       bf_ref_arg(184)      => bf(11).
		q(nt(184), (nt(11)));
		//       bf_ref_arg(184)      => capture(85).
		q(nt(184), (nt(85)));
		//       bf_ref_arg(184)      => variable(88).
		q(nt(184), (nt(88)));
		//       __E_bf_and_30(186)   => bf_and_sym(59) _(9).
		q(nt(186), (nt(59)+nt(9)));
		//       __E_bf_and_30(186)   => null.
		q(nt(186), (nul));
		//       bf_and(176)          => bf(11) _(9) __E_bf_and_30(186) bf(11).
		q(nt(176), (nt(11)+nt(9)+nt(186)+nt(11)));
		//       bf_or(175)           => bf(11) _(9) bf_or_sym(60) _(9) bf(11).
		q(nt(175), (nt(11)+nt(9)+nt(60)+nt(9)+nt(11)));
		//       bf_xor(177)          => bf(11) _(9) bf_xor_sym(61) _(9) bf(11).
		q(nt(177), (nt(11)+nt(9)+nt(61)+nt(9)+nt(11)));
		//       bf_neg(178)          => bf(11) _(9) bf_neg_sym(62).
		q(nt(178), (nt(11)+nt(9)+nt(62)));
		//       __E_bf_all_31(187)   => variable(88).
		q(nt(187), (nt(88)));
		//       __E_bf_all_31(187)   => capture(85).
		q(nt(187), (nt(85)));
		//       bf_all(173)          => bf_all_sym(69) __(10) __E_bf_all_31(187) __(10) bf(11).
		q(nt(173), (nt(69)+nt(10)+nt(187)+nt(10)+nt(11)));
		//       __E_bf_ex_32(188)    => variable(88).
		q(nt(188), (nt(88)));
		//       __E_bf_ex_32(188)    => capture(85).
		q(nt(188), (nt(85)));
		//       bf_ex(174)           => bf_ex_sym(70) __(10) __E_bf_ex_32(188) __(10) bf(11).
		q(nt(174), (nt(70)+nt(10)+nt(188)+nt(10)+nt(11)));
		//       bf_splitter(171)     => bf_splitter_sym(71) _(9) open_parenthesis(24) _(9) bf(11) _(9) close_parenthesis(25).
		q(nt(171), (nt(71)+nt(9)+nt(24)+nt(9)+nt(11)+nt(9)+nt(25)));
		//       bf_constant(179)     => open_brace(28) _(9) constant(189) _(9) close_brace(29).
		q(nt(179), (nt(28)+nt(9)+nt(189)+nt(9)+nt(29)));
		//       constant(189)        => binding(190).
		q(nt(189), (nt(190)));
		//       constant(189)        => capture(85).
		q(nt(189), (nt(85)));
		//       binding(190)         => source_binding(191).
		q(nt(190), (nt(191)));
		//       binding(190)         => named_binding(192).
		q(nt(190), (nt(192)));
		//       named_binding(192)   => chars(74).
		q(nt(192), (nt(74)));
		//       source_binding(191)  => type(193) _(9) colon(31) _(9) source(194).
		q(nt(191), (nt(193)+nt(9)+nt(31)+nt(9)+nt(194)));
		//       type(193)            => chars(74).
		q(nt(193), (nt(74)));
		//       type(193)            => null.
		q(nt(193), (nul));
		//       source0(195)         => alnum(6).
		q(nt(195), (nt(6)));
		//       source0(195)         => space(2).
		q(nt(195), (nt(2)));
		//       source0(195)         => punct(7).
		q(nt(195), (nt(7)));
		//       __E_source_33(196)   => source0(195).
		q(nt(196), (nt(195)));
		//       __E_source_34(197)   => __E_source_33(196).
		q(nt(197), (nt(196)));
		//       __E_source_34(197)   => __E_source_33(196) __E_source_34(197).
		q(nt(197), (nt(196)+nt(197)));
		//       source(194)          => __E_source_34(197).
		q(nt(194), (nt(197)));
		//       bf_and_cb(167)       => bf_and_cb_sym(198) __(10) bf_cb_arg(199) __(10) bf_cb_arg(199).
		q(nt(167), (nt(198)+nt(10)+nt(199)+nt(10)+nt(199)));
		//       bf_or_cb(168)        => bf_or_cb_sym(200) __(10) bf_cb_arg(199) __(10) bf_cb_arg(199).
		q(nt(168), (nt(200)+nt(10)+nt(199)+nt(10)+nt(199)));
		//       bf_xor_cb(169)       => bf_xor_cb_sym(201) __(10) bf_cb_arg(199) __(10) bf_cb_arg(199).
		q(nt(169), (nt(201)+nt(10)+nt(199)+nt(10)+nt(199)));
		//       bf_neg_cb(170)       => bf_neg_cb_sym(202) __(10) bf_cb_arg(199).
		q(nt(170), (nt(202)+nt(10)+nt(199)));
		//       bf_eq_cb(122)        => bf_eq_cb_sym(203) __(10) bf_cb_arg(199) __(10) wff_cb_arg(204) __(10) wff_cb_arg(204).
		q(nt(122), (nt(203)+nt(10)+nt(199)+nt(10)+nt(204)+nt(10)+nt(204)));
		//       bf_neq_cb(123)       => bf_neq_cb_sym(205) __(10) bf_cb_arg(199) __(10) wff_cb_arg(204) __(10) wff_cb_arg(204).
		q(nt(123), (nt(205)+nt(10)+nt(199)+nt(10)+nt(204)+nt(10)+nt(204)));
		//       bf_is_zero_cb(162)   => bf_is_zero_cb_sym(206) __(10) bf_cb_arg(199) __(10) bf_cb_arg(199).
		q(nt(162), (nt(206)+nt(10)+nt(199)+nt(10)+nt(199)));
		//       bf_is_one_cb(163)    => bf_is_one_cb_sym(207) __(10) bf_cb_arg(199) __(10) bf_cb_arg(199).
		q(nt(163), (nt(207)+nt(10)+nt(199)+nt(10)+nt(199)));
		//       bf_remove_funiversal_cb(165) => bf_remove_funiversal_cb_sym(208) __(10) bf_cb_arg(199) __(10) bf_cb_arg(199) __(10) bf_cb_arg(199) __(10) bf_cb_arg(199).
		q(nt(165), (nt(208)+nt(10)+nt(199)+nt(10)+nt(199)+nt(10)+nt(199)+nt(10)+nt(199)));
		//       bf_remove_fexistential_cb(166) => bf_remove_fexistential_cb_sym(209) __(10) bf_cb_arg(199) __(10) bf_cb_arg(199) __(10) bf_cb_arg(199) __(10) bf_cb_arg(199).
		q(nt(166), (nt(209)+nt(10)+nt(199)+nt(10)+nt(199)+nt(10)+nt(199)+nt(10)+nt(199)));
		//       wff_remove_existential_cb(126) => wff_remove_existential_cb_sym(210) __(10) wff_cb_arg(204) __(10) wff_cb_arg(204).
		q(nt(126), (nt(210)+nt(10)+nt(204)+nt(10)+nt(204)));
		//       wff_remove_bexistential_cb(127) => wff_remove_bexistential_cb_sym(211) __(10) wff_cb_arg(204) __(10) wff_cb_arg(204) __(10) wff_cb_arg(204) __(10) wff_cb_arg(204).
		q(nt(127), (nt(211)+nt(10)+nt(204)+nt(10)+nt(204)+nt(10)+nt(204)+nt(10)+nt(204)));
		//       wff_remove_buniversal_cb(128) => wff_remove_buniversal_cb_sym(212) __(10) wff_cb_arg(204) __(10) wff_cb_arg(204) __(10) wff_cb_arg(204) __(10) wff_cb_arg(204).
		q(nt(128), (nt(212)+nt(10)+nt(204)+nt(10)+nt(204)+nt(10)+nt(204)+nt(10)+nt(204)));
		//       wff_has_clashing_subformulas_cb(124) => wff_has_clashing_subformulas_cb_sym(213) __(10) wff_cb_arg(204) __(10) wff_cb_arg(204).
		q(nt(124), (nt(213)+nt(10)+nt(204)+nt(10)+nt(204)));
		//       bf_has_subformula_cb(164) => bf_has_subformula_cb_sym(214) __(10) bf_cb_arg(199) __(10) bf_cb_arg(199) __(10) bf_cb_arg(199).
		q(nt(164), (nt(214)+nt(10)+nt(199)+nt(10)+nt(199)+nt(10)+nt(199)));
		//       wff_has_subformula_cb(125) => wff_has_subformula_cb_sym(215) __(10) wff_cb_arg(204) __(10) wff_cb_arg(204) __(10) wff_cb_arg(204).
		q(nt(125), (nt(215)+nt(10)+nt(204)+nt(10)+nt(204)+nt(10)+nt(204)));
		//       bf_has_subformula_cb_sym(214) => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b'.
		q(nt(214), (t(30)+t(36)+t(32)+t(42)+t(26)+t(43)+t(32)+t(43)+t(44)+t(30)+t(36)+t(31)+t(45)+t(46)+t(44)+t(27)+t(26)+t(32)+t(47)+t(30)));
		//       wff_has_clashing_subformulas_cb_sym(213) => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b'.
		q(nt(213), (t(48)+t(36)+t(36)+t(32)+t(42)+t(26)+t(43)+t(32)+t(47)+t(27)+t(26)+t(43)+t(42)+t(41)+t(49)+t(50)+t(32)+t(43)+t(44)+t(30)+t(36)+t(31)+t(45)+t(46)+t(44)+t(27)+t(26)+t(43)+t(32)+t(47)+t(30)));
		//       wff_has_subformula_cb_sym(215) => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b'.
		q(nt(215), (t(48)+t(36)+t(36)+t(32)+t(42)+t(26)+t(43)+t(32)+t(43)+t(44)+t(30)+t(36)+t(31)+t(45)+t(46)+t(44)+t(27)+t(26)+t(32)+t(47)+t(30)));
		//       wff_remove_existential_cb_sym(210) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(210), (t(48)+t(36)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(28)+t(29)+t(41)+t(43)+t(52)+t(28)+t(49)+t(52)+t(41)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       wff_remove_bexistential_cb_sym(211) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(211), (t(48)+t(36)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(30)+t(28)+t(29)+t(41)+t(43)+t(52)+t(28)+t(49)+t(52)+t(41)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       wff_remove_buniversal_cb_sym(212) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b'.
		q(nt(212), (t(48)+t(36)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(30)+t(44)+t(49)+t(41)+t(51)+t(28)+t(45)+t(43)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       bf_remove_fexistential_cb_sym(209) => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(209), (t(30)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(36)+t(28)+t(29)+t(41)+t(43)+t(52)+t(28)+t(49)+t(52)+t(41)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       bf_remove_funiversal_cb_sym(208) => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b'.
		q(nt(208), (t(30)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(36)+t(44)+t(49)+t(41)+t(51)+t(28)+t(45)+t(43)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       bf_cb_arg(199)       => bf(11).
		q(nt(199), (nt(11)));
		//       wff_cb_arg(204)      => wff(13).
		q(nt(204), (nt(13)));
		//       bf_and_cb_sym(198)   => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b'.
		q(nt(198), (t(30)+t(36)+t(32)+t(26)+t(49)+t(53)+t(32)+t(47)+t(30)));
		//       bf_or_cb_sym(200)    => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b'.
		q(nt(200), (t(30)+t(36)+t(32)+t(31)+t(45)+t(32)+t(47)+t(30)));
		//       bf_xor_cb_sym(201)   => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b'.
		q(nt(201), (t(30)+t(36)+t(32)+t(29)+t(31)+t(45)+t(32)+t(47)+t(30)));
		//       bf_neg_cb_sym(202)   => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b'.
		q(nt(202), (t(30)+t(36)+t(32)+t(49)+t(28)+t(50)+t(32)+t(47)+t(30)));
		//       bf_eq_cb_sym(203)    => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b'.
		q(nt(203), (t(30)+t(36)+t(32)+t(28)+t(54)+t(32)+t(47)+t(30)));
		//       bf_neq_cb_sym(205)   => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b'.
		q(nt(205), (t(30)+t(36)+t(32)+t(49)+t(28)+t(54)+t(32)+t(47)+t(30)));
		//       bf_is_zero_cb_sym(206) => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b'.
		q(nt(206), (t(30)+t(36)+t(32)+t(41)+t(43)+t(32)+t(55)+t(28)+t(45)+t(31)+t(32)+t(47)+t(30)));
		//       bf_is_one_cb_sym(207) => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b'.
		q(nt(207), (t(30)+t(36)+t(32)+t(41)+t(43)+t(32)+t(31)+t(49)+t(28)+t(32)+t(47)+t(30)));
		//       __E_tau_collapse_positives_cb_35(216) => tau_collapse_positives_cb_sym(217) __(10) tau_cb_arg(218) __(10) tau_cb_arg(218) __(10) tau_cb_arg(218).
		q(nt(216), (nt(217)+nt(10)+nt(218)+nt(10)+nt(218)+nt(10)+nt(218)));
		//       tau_collapse_positives_cb(106) => __E_tau_collapse_positives_cb_35(216).
		q(nt(106), (nt(216)));
		//       __E_tau_collapse_positives_cb_36(219) => tau_collapse_positives_cb_sym(217) __(10) tau_cb_arg(218) __(10) tau_cb_arg(218).
		q(nt(219), (nt(217)+nt(10)+nt(218)+nt(10)+nt(218)));
		//       tau_collapse_positives_cb(106) => __E_tau_collapse_positives_cb_36(219).
		q(nt(106), (nt(219)));
		//       tau_positives_upwards_cb(107) => tau_positives_upwards_cb_sym(220) __(10) tau_cb_arg(218) __(10) tau_cb_arg(218).
		q(nt(107), (nt(220)+nt(10)+nt(218)+nt(10)+nt(218)));
		//       tau_cb_arg(218)      => capture(85).
		q(nt(218), (nt(85)));
		//       tau_cb_arg(218)      => tau(15).
		q(nt(218), (nt(15)));
		//       tau_collapse_positives_cb_sym(217) => 't' 'a' 'u' '_' 'c' 'o' 'l' 'l' 'a' 'p' 's' 'e' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'c' 'b'.
		q(nt(217), (t(52)+t(26)+t(44)+t(32)+t(47)+t(31)+t(27)+t(27)+t(26)+t(56)+t(43)+t(28)+t(32)+t(56)+t(31)+t(43)+t(41)+t(52)+t(41)+t(51)+t(28)+t(43)+t(32)+t(47)+t(30)));
		//       tau_positives_upwards_cb_sym(220) => 't' 'a' 'u' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'u' 'p' 'w' 'a' 'r' 'd' 's' '_' 'c' 'b'.
		q(nt(220), (t(52)+t(26)+t(44)+t(32)+t(56)+t(31)+t(43)+t(41)+t(52)+t(41)+t(51)+t(28)+t(43)+t(32)+t(44)+t(56)+t(48)+t(26)+t(45)+t(53)+t(43)+t(32)+t(47)+t(30)));
		//       __E_inputs_37(223)   => _(9) input(222).
		q(nt(223), (nt(9)+nt(222)));
		//       __E_inputs_38(224)   => null.
		q(nt(224), (nul));
		//       __E_inputs_38(224)   => __E_inputs_37(223) __E_inputs_38(224).
		q(nt(224), (nt(223)+nt(224)));
		//       inputs(221)          => _(9) less(33) _(9) input(222) __E_inputs_38(224) _(9) dot(23).
		q(nt(221), (nt(9)+nt(33)+nt(9)+nt(222)+nt(224)+nt(9)+nt(23)));
		//       input(222)           => in(93) _(9) colon(31) _(9) open_brace(28) _(9) source_binding(191) _(9) close_brace(29).
		q(nt(222), (nt(93)+nt(9)+nt(31)+nt(9)+nt(28)+nt(9)+nt(191)+nt(9)+nt(29)));
		//       builder(225)         => _(9) builder_head(226) _(9) builder_body(227) _(9) dot(23).
		q(nt(225), (nt(9)+nt(226)+nt(9)+nt(227)+nt(9)+nt(23)));
		//       __E_builder_head_39(228) => __(10) capture(85).
		q(nt(228), (nt(10)+nt(85)));
		//       __E_builder_head_40(229) => null.
		q(nt(229), (nul));
		//       __E_builder_head_40(229) => __E_builder_head_39(228) __E_builder_head_40(229).
		q(nt(229), (nt(228)+nt(229)));
		//       builder_head(226)    => open_parenthesis(24) _(9) capture(85) __E_builder_head_40(229) _(9) close_parenthesis(25).
		q(nt(226), (nt(24)+nt(9)+nt(85)+nt(229)+nt(9)+nt(25)));
		//       builder_body(227)    => bf_builder_body(230).
		q(nt(227), (nt(230)));
		//       builder_body(227)    => wff_builder_body(231).
		q(nt(227), (nt(231)));
		//       builder_body(227)    => tau_builder_body(232).
		q(nt(227), (nt(232)));
		//       bf_builder_body(230) => bf_builder_def(58) _(9) bf(11).
		q(nt(230), (nt(58)+nt(9)+nt(11)));
		//       wff_builder_body(231) => wff_builder_def(43) _(9) wff(13).
		q(nt(231), (nt(43)+nt(9)+nt(13)));
		//       tau_builder_body(232) => tau_builder_def(38) _(9) tau(15).
		q(nt(232), (nt(38)+nt(9)+nt(15)));
		//       library(233)         => rules(234).
		q(nt(233), (nt(234)));
		//       __E_rules_41(235)    => _(9) rule(236).
		q(nt(235), (nt(9)+nt(236)));
		//       __E_rules_42(237)    => null.
		q(nt(237), (nul));
		//       __E_rules_42(237)    => __E_rules_41(235) __E_rules_42(237).
		q(nt(237), (nt(235)+nt(237)));
		//       rules(234)           => __E_rules_42(237).
		q(nt(234), (nt(237)));
		//       rule(236)            => wff_rule(119).
		q(nt(236), (nt(119)));
		//       rule(236)            => bf_rule(159).
		q(nt(236), (nt(159)));
		//       rule(236)            => tau_rule(103).
		q(nt(236), (nt(103)));
		//       nso_rr(238)          => nso_rec_relations(239) nso_main(240).
		q(nt(238), (nt(239)+nt(240)));
		//       __E_nso_rec_relations_43(241) => _(9) nso_rec_relation(242).
		q(nt(241), (nt(9)+nt(242)));
		//       __E_nso_rec_relations_44(243) => null.
		q(nt(243), (nul));
		//       __E_nso_rec_relations_44(243) => __E_nso_rec_relations_43(241) __E_nso_rec_relations_44(243).
		q(nt(243), (nt(241)+nt(243)));
		//       nso_rec_relations(239) => __E_nso_rec_relations_44(243).
		q(nt(239), (nt(243)));
		//       nso_rec_relation(242) => bf_rec_relation(180).
		q(nt(242), (nt(180)));
		//       nso_rec_relation(242) => wff_rec_relation(129).
		q(nt(242), (nt(129)));
		//       nso_main(240)        => _(9) wff(13) _(9) dot(23).
		q(nt(240), (nt(9)+nt(13)+nt(9)+nt(23)));
		//       gssotc_rr(244)       => gssotc_rec_relations(245) gssotc_main(246).
		q(nt(244), (nt(245)+nt(246)));
		//       __E_gssotc_rec_relations_45(247) => _(9) gssotc_rec_relation(248).
		q(nt(247), (nt(9)+nt(248)));
		//       __E_gssotc_rec_relations_46(249) => null.
		q(nt(249), (nul));
		//       __E_gssotc_rec_relations_46(249) => __E_gssotc_rec_relations_45(247) __E_gssotc_rec_relations_46(249).
		q(nt(249), (nt(247)+nt(249)));
		//       gssotc_rec_relations(245) => __E_gssotc_rec_relations_46(249).
		q(nt(245), (nt(249)));
		//       gssotc_rec_relation(248) => tau_rec_relation(108).
		q(nt(248), (nt(108)));
		//       gssotc_rec_relation(248) => wff_rec_relation(129).
		q(nt(248), (nt(129)));
		//       gssotc_rec_relation(248) => bf_rec_relation(180).
		q(nt(248), (nt(180)));
		//       gssotc_main(246)     => _(9) tau(15) _(9) semicolon(32).
		q(nt(246), (nt(9)+nt(15)+nt(9)+nt(32)));
		//       __E_start_47(251)    => inputs(221).
		q(nt(251), (nt(221)));
		//       __E_start_47(251)    => builder(225).
		q(nt(251), (nt(225)));
		//       __E_start_47(251)    => library(233).
		q(nt(251), (nt(233)));
		//       __E_start_47(251)    => nso_rr(238).
		q(nt(251), (nt(238)));
		//       __E_start_47(251)    => gssotc_rr(244).
		q(nt(251), (nt(244)));
		//       start(250)           => __E_start_47(251) _(9).
		q(nt(250), (nt(251)+nt(9)));
		//       __E_cli_48(254)      => _(9) dot(23) _(9) cli_command(253).
		q(nt(254), (nt(9)+nt(23)+nt(9)+nt(253)));
		//       __E_cli_49(255)      => null.
		q(nt(255), (nul));
		//       __E_cli_49(255)      => __E_cli_48(254) __E_cli_49(255).
		q(nt(255), (nt(254)+nt(255)));
		//       cli(252)             => _(9) cli_command(253) __E_cli_49(255).
		q(nt(252), (nt(9)+nt(253)+nt(255)));
		//       cli_command(253)     => help_cmd(256).
		q(nt(253), (nt(256)));
		//       cli_command(253)     => version_cmd(257).
		q(nt(253), (nt(257)));
		//       cli_command(253)     => quit_cmd(258).
		q(nt(253), (nt(258)));
		//       cli_command(253)     => clear_cmd(259).
		q(nt(253), (nt(259)));
		//       cli_command(253)     => get_cmd(260).
		q(nt(253), (nt(260)));
		//       cli_command(253)     => set_cmd(261).
		q(nt(253), (nt(261)));
		//       cli_command(253)     => toggle_cmd(262).
		q(nt(253), (nt(262)));
		//       cli_command(253)     => list_outputs_cmd(263).
		q(nt(253), (nt(263)));
		//       cli_command(253)     => clear_outputs_cmd(264).
		q(nt(253), (nt(264)));
		//       cli_command(253)     => print_output_cmd(265).
		q(nt(253), (nt(265)));
		//       cli_command(253)     => file(266).
		q(nt(253), (nt(266)));
		//       cli_command(253)     => normalize_cmd(267).
		q(nt(253), (nt(267)));
		//       cli_command(253)     => execute_cmd(268).
		q(nt(253), (nt(268)));
		//       cli_command(253)     => bf_dnf_cmd(269).
		q(nt(253), (nt(269)));
		//       cli_command(253)     => bf_cnf_cmd(270).
		q(nt(253), (nt(270)));
		//       cli_command(253)     => bf_anf_cmd(271).
		q(nt(253), (nt(271)));
		//       cli_command(253)     => bf_nnf_cmd(272).
		q(nt(253), (nt(272)));
		//       cli_command(253)     => bf_pnf_cmd(273).
		q(nt(253), (nt(273)));
		//       cli_command(253)     => bf_mnf_cmd(274).
		q(nt(253), (nt(274)));
		//       cli_command(253)     => bf_substitute(275).
		q(nt(253), (nt(275)));
		//       cli_command(253)     => wff_substitute(276).
		q(nt(253), (nt(276)));
		//       cli_command(253)     => bf_instantiate(277).
		q(nt(253), (nt(277)));
		//       cli_command(253)     => wff_instantiate(278).
		q(nt(253), (nt(278)));
		//       cli_command(253)     => wff_onf_cmd(279).
		q(nt(253), (nt(279)));
		//       cli_command(253)     => wff_dnf_cmd(280).
		q(nt(253), (nt(280)));
		//       cli_command(253)     => wff_cnf_cmd(281).
		q(nt(253), (nt(281)));
		//       cli_command(253)     => wff_anf_cmd(282).
		q(nt(253), (nt(282)));
		//       cli_command(253)     => wff_nnf_cmd(283).
		q(nt(253), (nt(283)));
		//       cli_command(253)     => wff_pnf_cmd(284).
		q(nt(253), (nt(284)));
		//       cli_command(253)     => wff_mnf_cmd(285).
		q(nt(253), (nt(285)));
		//       cli_command(253)     => def_rule_cmd(286).
		q(nt(253), (nt(286)));
		//       cli_command(253)     => def_list_cmd(287).
		q(nt(253), (nt(287)));
		//       cli_command(253)     => def_del_cmd(288).
		q(nt(253), (nt(288)));
		//       cli_command(253)     => def_clear_cmd(289).
		q(nt(253), (nt(289)));
		//       cli_command(253)     => bf(11).
		q(nt(253), (nt(11)));
		//       cli_command(253)     => wff(13).
		q(nt(253), (nt(13)));
		//       cli_command(253)     => nso_rr(238).
		q(nt(253), (nt(238)));
		//       normalize_cmd(267)   => normalize_cmd_sym(290) __(10) normalize_cmd_arg(291).
		q(nt(267), (nt(290)+nt(10)+nt(291)));
		//       normalize_cmd_arg(291) => wff(13).
		q(nt(291), (nt(13)));
		//       normalize_cmd_arg(291) => nso_rr(238).
		q(nt(291), (nt(238)));
		//       normalize_cmd_arg(291) => output(292).
		q(nt(291), (nt(292)));
		//       normalize_cmd_arg(291) => wff_selection(293).
		q(nt(291), (nt(293)));
		//       execute_cmd(268)     => execute_cmd_sym(294) __(10) execute_cmd_arg(295).
		q(nt(268), (nt(294)+nt(10)+nt(295)));
		//       execute_cmd_arg(295) => gssotc_rr(244).
		q(nt(295), (nt(244)));
		//       file(266)            => file_sym(296) quote(36) file_path(297) quote(36).
		q(nt(266), (nt(296)+nt(36)+nt(297)+nt(36)));
		//       __E_bf_instantiate_cmd_50(300) => bf(11).
		q(nt(300), (nt(11)));
		//       __E_bf_instantiate_cmd_50(300) => bf_selection(301).
		q(nt(300), (nt(301)));
		//       __E_bf_instantiate_cmd_50(300) => output(292).
		q(nt(300), (nt(292)));
		//       __E_bf_instantiate_cmd_51(302) => bf_selection(301).
		q(nt(302), (nt(301)));
		//       __E_bf_instantiate_cmd_51(302) => output(292).
		q(nt(302), (nt(292)));
		//       bf_instantiate_cmd(298) => instantiate_cmd_sym(299) __(10) variable(88) __(10) __E_bf_instantiate_cmd_50(300) __(10) __E_bf_instantiate_cmd_51(302).
		q(nt(298), (nt(299)+nt(10)+nt(88)+nt(10)+nt(300)+nt(10)+nt(302)));
		//       __E_wff_instantiate_cmd_52(304) => wff(13).
		q(nt(304), (nt(13)));
		//       __E_wff_instantiate_cmd_52(304) => wff_selection(293).
		q(nt(304), (nt(293)));
		//       __E_wff_instantiate_cmd_52(304) => output(292).
		q(nt(304), (nt(292)));
		//       __E_wff_instantiate_cmd_53(305) => wff_selection(293).
		q(nt(305), (nt(293)));
		//       __E_wff_instantiate_cmd_53(305) => output(292).
		q(nt(305), (nt(292)));
		//       wff_instantiate_cmd(303) => instantiate_cmd_sym(299) __(10) variable(88) __(10) __E_wff_instantiate_cmd_52(304) __(10) __E_wff_instantiate_cmd_53(305).
		q(nt(303), (nt(299)+nt(10)+nt(88)+nt(10)+nt(304)+nt(10)+nt(305)));
		//       __E_bf_substitute_cmd_54(308) => bf(11).
		q(nt(308), (nt(11)));
		//       __E_bf_substitute_cmd_54(308) => output(292).
		q(nt(308), (nt(292)));
		//       __E_bf_substitute_cmd_54(308) => bf_selection(301).
		q(nt(308), (nt(301)));
		//       __E_bf_substitute_cmd_55(309) => output(292).
		q(nt(309), (nt(292)));
		//       __E_bf_substitute_cmd_55(309) => bf_selection(301).
		q(nt(309), (nt(301)));
		//       __E_bf_substitute_cmd_55(309) => bf_selection(301).
		q(nt(309), (nt(301)));
		//       __E_bf_substitute_cmd_56(310) => bf_selection(301).
		q(nt(310), (nt(301)));
		//       __E_bf_substitute_cmd_56(310) => output(292).
		q(nt(310), (nt(292)));
		//       bf_substitute_cmd(306) => substitute_cmd_sym(307) __(10) __E_bf_substitute_cmd_54(308) __(10) __E_bf_substitute_cmd_55(309) __E_bf_substitute_cmd_56(310).
		q(nt(306), (nt(307)+nt(10)+nt(308)+nt(10)+nt(309)+nt(310)));
		//       __E_wff_substitute_cmd_57(313) => wff(13).
		q(nt(313), (nt(13)));
		//       __E_wff_substitute_cmd_57(313) => output(292).
		q(nt(313), (nt(292)));
		//       __E_wff_substitute_cmd_57(313) => wff_selection(293).
		q(nt(313), (nt(293)));
		//       __E_wff_substitute_cmd_58(314) => output(292).
		q(nt(314), (nt(292)));
		//       __E_wff_substitute_cmd_58(314) => wff_selection(293).
		q(nt(314), (nt(293)));
		//       __E_wff_substitute_cmd_59(315) => wff_selection(293).
		q(nt(315), (nt(293)));
		//       __E_wff_substitute_cmd_59(315) => output(292).
		q(nt(315), (nt(292)));
		//       wff_substitute_cmd(311) => substitutecmd__sym(312) __(10) __E_wff_substitute_cmd_57(313) __(10) __E_wff_substitute_cmd_58(314) __E_wff_substitute_cmd_59(315).
		q(nt(311), (nt(312)+nt(10)+nt(313)+nt(10)+nt(314)+nt(315)));
		//       __E_bf_dnf_cmd_60(317) => bf(11).
		q(nt(317), (nt(11)));
		//       __E_bf_dnf_cmd_60(317) => output(292).
		q(nt(317), (nt(292)));
		//       __E_bf_dnf_cmd_60(317) => bf_selection(301).
		q(nt(317), (nt(301)));
		//       bf_dnf_cmd(269)      => dnf_cmd_sym(316) __(10) __E_bf_dnf_cmd_60(317).
		q(nt(269), (nt(316)+nt(10)+nt(317)));
		//       __E_bf_cnf_cmd_61(319) => bf(11).
		q(nt(319), (nt(11)));
		//       __E_bf_cnf_cmd_61(319) => output(292).
		q(nt(319), (nt(292)));
		//       __E_bf_cnf_cmd_61(319) => bf_selection(301).
		q(nt(319), (nt(301)));
		//       bf_cnf_cmd(270)      => cnf_cmd_sym(318) __(10) __E_bf_cnf_cmd_61(319).
		q(nt(270), (nt(318)+nt(10)+nt(319)));
		//       __E_bf_anf_cmd_62(321) => bf(11).
		q(nt(321), (nt(11)));
		//       __E_bf_anf_cmd_62(321) => output(292).
		q(nt(321), (nt(292)));
		//       __E_bf_anf_cmd_62(321) => bf_selection(301).
		q(nt(321), (nt(301)));
		//       bf_anf_cmd(271)      => anf_cmd_sym(320) __(10) __E_bf_anf_cmd_62(321).
		q(nt(271), (nt(320)+nt(10)+nt(321)));
		//       __E_bf_nnf_cmd_63(323) => bf(11).
		q(nt(323), (nt(11)));
		//       __E_bf_nnf_cmd_63(323) => output(292).
		q(nt(323), (nt(292)));
		//       __E_bf_nnf_cmd_63(323) => bf_selection(301).
		q(nt(323), (nt(301)));
		//       bf_nnf_cmd(272)      => nnf_cmd_sym(322) __(10) __E_bf_nnf_cmd_63(323).
		q(nt(272), (nt(322)+nt(10)+nt(323)));
		//       __E_bf_pnf_cmd_64(325) => bf(11).
		q(nt(325), (nt(11)));
		//       __E_bf_pnf_cmd_64(325) => output(292).
		q(nt(325), (nt(292)));
		//       __E_bf_pnf_cmd_64(325) => bf_selection(301).
		q(nt(325), (nt(301)));
		//       bf_pnf_cmd(273)      => pnf_cmd_sym(324) __(10) __E_bf_pnf_cmd_64(325).
		q(nt(273), (nt(324)+nt(10)+nt(325)));
		//       __E_bf_mnf_cmd_65(327) => bf(11).
		q(nt(327), (nt(11)));
		//       __E_bf_mnf_cmd_65(327) => output(292).
		q(nt(327), (nt(292)));
		//       __E_bf_mnf_cmd_65(327) => bf_selection(301).
		q(nt(327), (nt(301)));
		//       bf_mnf_cmd(274)      => mnf_cmd_sym(326) __(10) __E_bf_mnf_cmd_65(327).
		q(nt(274), (nt(326)+nt(10)+nt(327)));
		//       __E_wff_onf_cmd_66(329) => wff(13).
		q(nt(329), (nt(13)));
		//       __E_wff_onf_cmd_66(329) => output(292).
		q(nt(329), (nt(292)));
		//       __E_wff_onf_cmd_66(329) => wff_selection(293).
		q(nt(329), (nt(293)));
		//       wff_onf_cmd(279)     => onf_cmd_sym(328) __(10) variable(88) __(10) __E_wff_onf_cmd_66(329).
		q(nt(279), (nt(328)+nt(10)+nt(88)+nt(10)+nt(329)));
		//       __E_wff_dnf_cmd_67(330) => wff(13).
		q(nt(330), (nt(13)));
		//       __E_wff_dnf_cmd_67(330) => output(292).
		q(nt(330), (nt(292)));
		//       __E_wff_dnf_cmd_67(330) => wff_selection(293).
		q(nt(330), (nt(293)));
		//       wff_dnf_cmd(280)     => dnf_cmd_sym(316) __(10) __E_wff_dnf_cmd_67(330).
		q(nt(280), (nt(316)+nt(10)+nt(330)));
		//       __E_wff_cnf_cmd_68(331) => wff(13).
		q(nt(331), (nt(13)));
		//       __E_wff_cnf_cmd_68(331) => output(292).
		q(nt(331), (nt(292)));
		//       __E_wff_cnf_cmd_68(331) => wff_selection(293).
		q(nt(331), (nt(293)));
		//       wff_cnf_cmd(281)     => cnf_cmd_sym(318) __(10) __E_wff_cnf_cmd_68(331).
		q(nt(281), (nt(318)+nt(10)+nt(331)));
		//       __E_wff_anf_cmd_69(332) => wff(13).
		q(nt(332), (nt(13)));
		//       __E_wff_anf_cmd_69(332) => output(292).
		q(nt(332), (nt(292)));
		//       __E_wff_anf_cmd_69(332) => wff_selection(293).
		q(nt(332), (nt(293)));
		//       wff_anf_cmd(282)     => anf_cmd_sym(320) __(10) __E_wff_anf_cmd_69(332).
		q(nt(282), (nt(320)+nt(10)+nt(332)));
		//       __E_wff_nnf_cmd_70(333) => wff(13).
		q(nt(333), (nt(13)));
		//       __E_wff_nnf_cmd_70(333) => output(292).
		q(nt(333), (nt(292)));
		//       __E_wff_nnf_cmd_70(333) => wff_selection(293).
		q(nt(333), (nt(293)));
		//       wff_nnf_cmd(283)     => nnf_cmd_sym(322) __(10) __E_wff_nnf_cmd_70(333).
		q(nt(283), (nt(322)+nt(10)+nt(333)));
		//       __E_wff_pnf_cmd_71(334) => wff(13).
		q(nt(334), (nt(13)));
		//       __E_wff_pnf_cmd_71(334) => output(292).
		q(nt(334), (nt(292)));
		//       __E_wff_pnf_cmd_71(334) => wff_selection(293).
		q(nt(334), (nt(293)));
		//       wff_pnf_cmd(284)     => pnf_cmd_sym(324) __(10) __E_wff_pnf_cmd_71(334).
		q(nt(284), (nt(324)+nt(10)+nt(334)));
		//       __E_wff_mnf_cmd_72(335) => wff(13).
		q(nt(335), (nt(13)));
		//       __E_wff_mnf_cmd_72(335) => output(292).
		q(nt(335), (nt(292)));
		//       __E_wff_mnf_cmd_72(335) => wff_selection(293).
		q(nt(335), (nt(293)));
		//       wff_mnf_cmd(285)     => mnf_cmd_sym(326) __(10) __E_wff_mnf_cmd_72(335).
		q(nt(285), (nt(326)+nt(10)+nt(335)));
		//       def_rule_cmd(286)    => def_cmd_sym(336) __(10) def_rule_cmd_arg(337).
		q(nt(286), (nt(336)+nt(10)+nt(337)));
		//       __E_def_rule_cmd_arg_73(338) => bf_rec_relation(180).
		q(nt(338), (nt(180)));
		//       __E_def_rule_cmd_arg_73(338) => wff_rec_relation(129).
		q(nt(338), (nt(129)));
		//       __E_def_rule_cmd_arg_73(338) => tau_rec_relation(108).
		q(nt(338), (nt(108)));
		//       def_rule_cmd_arg(337) => __E_def_rule_cmd_arg_73(338).
		q(nt(337), (nt(338)));
		//       def_list_cmd(287)    => def_cmd_sym(336) __(10) def_cmd_list(339).
		q(nt(287), (nt(336)+nt(10)+nt(339)));
		//       def_del_cmd(288)     => def_cmd_sym(336) __(10) def_cmd_del(340) __(10) digits(77).
		q(nt(288), (nt(336)+nt(10)+nt(340)+nt(10)+nt(77)));
		//       def_clear_cmd(289)   => def_cmd_sym(336) __(10) def_cmf_clear(341).
		q(nt(289), (nt(336)+nt(10)+nt(341)));
		//       __E_help_cmd_74(343) => __(10) cli_cmd_sym(344).
		q(nt(343), (nt(10)+nt(344)));
		//       __E_help_cmd_74(343) => null.
		q(nt(343), (nul));
		//       help_cmd(256)        => help_cmd_sym(342) __E_help_cmd_74(343).
		q(nt(256), (nt(342)+nt(343)));
		//       version_cmd(257)     => version_cmd_sym(345).
		q(nt(257), (nt(345)));
		//       quit_cmd(258)        => quit_cmd_sym(346).
		q(nt(258), (nt(346)));
		//       clear_cmd(259)       => clear_cmd_sym(347).
		q(nt(259), (nt(347)));
		//       wff_selection(293)   => selection_sym(348) __(10) digits(77).
		q(nt(293), (nt(348)+nt(10)+nt(77)));
		//       bf_selection(301)    => selection_sym(348) __(10) digits(77).
		q(nt(301), (nt(348)+nt(10)+nt(77)));
		//       bf_var_selection(349) => selection_sym(348) __(10) digits(77).
		q(nt(349), (nt(348)+nt(10)+nt(77)));
		//       wff_var_selection(350) => selection_sym(348) __(10) digits(77).
		q(nt(350), (nt(348)+nt(10)+nt(77)));
		//       file_path(297)       => chars(74).
		q(nt(297), (nt(74)));
		//       output(292)          => absolute_output(351).
		q(nt(292), (nt(351)));
		//       output(292)          => relative_output(352).
		q(nt(292), (nt(352)));
		//       __E_absolute_output_75(353) => absolute_output_sym(354).
		q(nt(353), (nt(354)));
		//       __E_absolute_output_75(353) => output_sym(355) __(10).
		q(nt(353), (nt(355)+nt(10)));
		//       absolute_output(351) => __E_absolute_output_75(353) output_id(356).
		q(nt(351), (nt(353)+nt(356)));
		//       relative_output(352) => relative_output_sym(357) output_id(356).
		q(nt(352), (nt(357)+nt(356)));
		//       output_id(356)       => digits(77).
		q(nt(356), (nt(77)));
		//       list_outputs_cmd(263) => output_sym(355).
		q(nt(263), (nt(355)));
		//       list_outputs_cmd(263) => absolute_output_sym(354).
		q(nt(263), (nt(354)));
		//       list_outputs_cmd(263) => relative_output_sym(357).
		q(nt(263), (nt(357)));
		//       __E_clear_outputs_cmd_76(358) => output_sym(355) __(10).
		q(nt(358), (nt(355)+nt(10)));
		//       __E_clear_outputs_cmd_76(358) => absolute_output_sym(354).
		q(nt(358), (nt(354)));
		//       __E_clear_outputs_cmd_76(358) => relative_output_sym(357).
		q(nt(358), (nt(357)));
		//       clear_outputs_cmd(264) => __E_clear_outputs_cmd_76(358) clear_sym(359).
		q(nt(264), (nt(358)+nt(359)));
		//       print_output_cmd(265) => output(292).
		q(nt(265), (nt(292)));
		//       cli_cmd_sym(344)     => help_cmd_sym(342).
		q(nt(344), (nt(342)));
		//       cli_cmd_sym(344)     => version_cmd_sym(345).
		q(nt(344), (nt(345)));
		//       cli_cmd_sym(344)     => quit_cmd_sym(346).
		q(nt(344), (nt(346)));
		//       cli_cmd_sym(344)     => clear_cmd_sym(347).
		q(nt(344), (nt(347)));
		//       cli_cmd_sym(344)     => def_cmd_sym(336).
		q(nt(344), (nt(336)));
		//       cli_cmd_sym(344)     => output_sym(355).
		q(nt(344), (nt(355)));
		//       cli_cmd_sym(344)     => absolute_output_sym(354).
		q(nt(344), (nt(354)));
		//       cli_cmd_sym(344)     => relative_output_sym(357).
		q(nt(344), (nt(357)));
		//       cli_cmd_sym(344)     => selection_sym(348).
		q(nt(344), (nt(348)));
		//       cli_cmd_sym(344)     => instantiate_cmd_sym(299).
		q(nt(344), (nt(299)));
		//       cli_cmd_sym(344)     => substitute_cmd_sym(307).
		q(nt(344), (nt(307)));
		//       cli_cmd_sym(344)     => normalize_cmd_sym(290).
		q(nt(344), (nt(290)));
		//       cli_cmd_sym(344)     => execute_cmd_sym(294).
		q(nt(344), (nt(294)));
		//       cli_cmd_sym(344)     => file_cmd_sym(360).
		q(nt(344), (nt(360)));
		//       cli_cmd_sym(344)     => get_cmd_sym(361).
		q(nt(344), (nt(361)));
		//       cli_cmd_sym(344)     => set_cmd_sym(362).
		q(nt(344), (nt(362)));
		//       cli_cmd_sym(344)     => toggle_cmd_sym(363).
		q(nt(344), (nt(363)));
		//       cli_cmd_sym(344)     => cnf_cmd_sym(318).
		q(nt(344), (nt(318)));
		//       cli_cmd_sym(344)     => dnf_cmd_sym(316).
		q(nt(344), (nt(316)));
		//       cli_cmd_sym(344)     => nnf_cmd_sym(322).
		q(nt(344), (nt(322)));
		//       cli_cmd_sym(344)     => mnf_cmd_sym(326).
		q(nt(344), (nt(326)));
		//       cli_cmd_sym(344)     => onf_cmd_sym(328).
		q(nt(344), (nt(328)));
		//       help_cmd_sym(342)    => 'h'.
		q(nt(342), (t(42)));
		//       help_cmd_sym(342)    => 'h' 'e' 'l' 'p'.
		q(nt(342), (t(42)+t(28)+t(27)+t(56)));
		//       version_cmd_sym(345) => 'v'.
		q(nt(345), (t(51)));
		//       version_cmd_sym(345) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
		q(nt(345), (t(51)+t(28)+t(45)+t(43)+t(41)+t(31)+t(49)));
		//       quit_cmd_sym(346)    => 'q'.
		q(nt(346), (t(54)));
		//       quit_cmd_sym(346)    => 'e'.
		q(nt(346), (t(28)));
		//       quit_cmd_sym(346)    => 'q' 'u' 'i' 't'.
		q(nt(346), (t(54)+t(44)+t(41)+t(52)));
		//       quit_cmd_sym(346)    => 'e' 'x' 'i' 't'.
		q(nt(346), (t(28)+t(29)+t(41)+t(52)));
		//       instantiate_sym(364) => 'i'.
		q(nt(364), (t(41)));
		//       instantiate_sym(364) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
		q(nt(364), (t(41)+t(49)+t(43)+t(52)+t(26)+t(49)+t(52)+t(41)+t(26)+t(52)+t(28)));
		//       substitute_sym(365)  => 's'.
		q(nt(365), (t(43)));
		//       substitute_sym(365)  => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
		q(nt(365), (t(43)+t(44)+t(30)+t(43)+t(52)+t(41)+t(52)+t(44)+t(52)+t(28)));
		//       file_cmd_sym(360)    => 'r'.
		q(nt(360), (t(45)));
		//       file_cmd_sym(360)    => 'r' 'e' 'a' 'd'.
		q(nt(360), (t(45)+t(28)+t(26)+t(53)));
		//       clear_cmd_sym(347)   => 'c'.
		q(nt(347), (t(47)));
		//       clear_cmd_sym(347)   => 'c' 'l' 'e' 'a' 'r'.
		q(nt(347), (t(47)+t(27)+t(28)+t(26)+t(45)));
		//       normalize_cmd_sym(290) => 'n'.
		q(nt(290), (t(49)));
		//       normalize_cmd_sym(290) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
		q(nt(290), (t(49)+t(31)+t(45)+t(46)+t(26)+t(27)+t(41)+t(55)+t(28)));
		//       execute_cmd_sym(294) => 'e' 'x' 'e' 'c' 'u' 't' 'e'.
		q(nt(294), (t(28)+t(29)+t(28)+t(47)+t(44)+t(52)+t(28)));
		//       onf_cmd_sym(328)     => 'o' 'n' 'f'.
		q(nt(328), (t(31)+t(49)+t(36)));
		//       dnf_cmd_sym(316)     => 'd' 'n' 'f'.
		q(nt(316), (t(53)+t(49)+t(36)));
		//       cnf_cmd_sym(318)     => 'c' 'n' 'f'.
		q(nt(318), (t(47)+t(49)+t(36)));
		//       anf_cmd_sym(320)     => 'a' 'n' 'f'.
		q(nt(320), (t(26)+t(49)+t(36)));
		//       nnf_cmd_sym(322)     => 'n' 'n' 'f'.
		q(nt(322), (t(49)+t(49)+t(36)));
		//       pnf_cmd_sym(324)     => 'p' 'n' 'f'.
		q(nt(324), (t(56)+t(49)+t(36)));
		//       mnf_cmd_sym(326)     => 'm' 'n' 'f'.
		q(nt(326), (t(46)+t(49)+t(36)));
		//       def_cmd_sym(336)     => 'd' 'e' 'f'.
		q(nt(336), (t(53)+t(28)+t(36)));
		//       def_cmd_list(339)    => 'l' 'i' 's' 't'.
		q(nt(339), (t(27)+t(41)+t(43)+t(52)));
		//       def_cmd_del(340)     => 'd' 'e' 'l'.
		q(nt(340), (t(53)+t(28)+t(27)));
		//       def_cmf_clear(341)   => 'c' 'l' 'e' 'a' 'r'.
		q(nt(341), (t(47)+t(27)+t(28)+t(26)+t(45)));
		//       selection_sym(348)   => 's'.
		q(nt(348), (t(43)));
		//       selection_sym(348)   => 's' 'e' 'l' 'e' 'c' 't' 'i' 'o' 'n'.
		q(nt(348), (t(43)+t(28)+t(27)+t(28)+t(47)+t(52)+t(41)+t(31)+t(49)));
		//       output_sym(355)      => 'o'.
		q(nt(355), (t(31)));
		//       output_sym(355)      => 'o' 'u' 't' 'p' 'u' 't'.
		q(nt(355), (t(31)+t(44)+t(52)+t(56)+t(44)+t(52)));
		//       absolute_output_sym(354) => '&'.
		q(nt(354), (t(20)));
		//       relative_output_sym(357) => '%'.
		q(nt(357), (t(57)));
		//       __E_get_cmd_77(366)  => __(10) option(367).
		q(nt(366), (nt(10)+nt(367)));
		//       __E_get_cmd_77(366)  => null.
		q(nt(366), (nul));
		//       get_cmd(260)         => get_cmd_sym(361) __E_get_cmd_77(366).
		q(nt(260), (nt(361)+nt(366)));
		//       __E_set_cmd_78(368)  => __(10).
		q(nt(368), (nt(10)));
		//       __E_set_cmd_78(368)  => _(9) '=' _(9).
		q(nt(368), (nt(9)+t(19)+nt(9)));
		//       set_cmd(261)         => set_cmd_sym(362) __(10) option(367) __E_set_cmd_78(368) option_value(369).
		q(nt(261), (nt(362)+nt(10)+nt(367)+nt(368)+nt(369)));
		//       toggle_cmd(262)      => toggle_cmd_sym(363) __(10) bool_option(370).
		q(nt(262), (nt(363)+nt(10)+nt(370)));
		//       get_cmd_sym(361)     => 'g' 'e' 't'.
		q(nt(361), (t(50)+t(28)+t(52)));
		//       set_cmd_sym(362)     => 's' 'e' 't'.
		q(nt(362), (t(43)+t(28)+t(52)));
		//       toggle_cmd_sym(363)  => 't' 'o' 'g' 'g' 'l' 'e'.
		q(nt(363), (t(52)+t(31)+t(50)+t(50)+t(27)+t(28)));
		//       option(367)          => bool_option(370).
		q(nt(367), (nt(370)));
		//       option(367)          => severity_opt(371).
		q(nt(367), (nt(371)));
		//       bool_option(370)     => status_opt(372).
		q(nt(370), (nt(372)));
		//       bool_option(370)     => colors_opt(373).
		q(nt(370), (nt(373)));
		//       bool_option(370)     => debug_repl_opt(374).
		q(nt(370), (nt(374)));
		//       status_opt(372)      => 's'.
		q(nt(372), (t(43)));
		//       status_opt(372)      => 's' 't' 'a' 't' 'u' 's'.
		q(nt(372), (t(43)+t(52)+t(26)+t(52)+t(44)+t(43)));
		//       colors_opt(373)      => 'c'.
		q(nt(373), (t(47)));
		//       colors_opt(373)      => 'c' 'o' 'l' 'o' 'r'.
		q(nt(373), (t(47)+t(31)+t(27)+t(31)+t(45)));
		//       colors_opt(373)      => 'c' 'o' 'l' 'o' 'r' 's'.
		q(nt(373), (t(47)+t(31)+t(27)+t(31)+t(45)+t(43)));
		//       severity_opt(371)    => 's' 'e' 'v'.
		q(nt(371), (t(43)+t(28)+t(51)));
		//       severity_opt(371)    => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
		q(nt(371), (t(43)+t(28)+t(51)+t(28)+t(45)+t(41)+t(52)+t(58)));
		//       debug_repl_opt(374)  => 'd'.
		q(nt(374), (t(53)));
		//       debug_repl_opt(374)  => 'd' 'r'.
		q(nt(374), (t(53)+t(45)));
		//       debug_repl_opt(374)  => 'd' 'b' 'g'.
		q(nt(374), (t(53)+t(30)+t(50)));
		//       debug_repl_opt(374)  => 'd' 'e' 'b' 'u' 'g'.
		q(nt(374), (t(53)+t(28)+t(30)+t(44)+t(50)));
		//       debug_repl_opt(374)  => 'd' 'e' 'b' 'u' 'g' '-' 'r' 'e' 'p' 'l'.
		q(nt(374), (t(53)+t(28)+t(30)+t(44)+t(50)+t(12)+t(45)+t(28)+t(56)+t(27)));
		//       option_value(369)    => option_value_true(375).
		q(nt(369), (nt(375)));
		//       option_value(369)    => option_value_false(376).
		q(nt(369), (nt(376)));
		//       option_value(369)    => severity(377).
		q(nt(369), (nt(377)));
		//       option_value_true(375) => 't'.
		q(nt(375), (t(52)));
		//       option_value_true(375) => 't' 'r' 'u' 'e'.
		q(nt(375), (t(52)+t(45)+t(44)+t(28)));
		//       option_value_true(375) => 'o' 'n'.
		q(nt(375), (t(31)+t(49)));
		//       option_value_true(375) => '1'.
		q(nt(375), (t(38)));
		//       option_value_true(375) => 'y'.
		q(nt(375), (t(58)));
		//       option_value_true(375) => 'y' 'e' 's'.
		q(nt(375), (t(58)+t(28)+t(43)));
		//       option_value_false(376) => 'f'.
		q(nt(376), (t(36)));
		//       option_value_false(376) => 'f' 'a' 'l' 's' 'e'.
		q(nt(376), (t(36)+t(26)+t(27)+t(43)+t(28)));
		//       option_value_false(376) => 'o' 'f' 'f'.
		q(nt(376), (t(31)+t(36)+t(36)));
		//       option_value_false(376) => '0'.
		q(nt(376), (t(39)));
		//       option_value_false(376) => 'n'.
		q(nt(376), (t(49)));
		//       option_value_false(376) => 'n' 'o'.
		q(nt(376), (t(49)+t(31)));
		//       severity(377)        => error_sym(378).
		q(nt(377), (nt(378)));
		//       severity(377)        => debug_sym(379).
		q(nt(377), (nt(379)));
		//       severity(377)        => trace_sym(380).
		q(nt(377), (nt(380)));
		//       severity(377)        => info_sym(381).
		q(nt(377), (nt(381)));
		//       error_sym(378)       => 'e'.
		q(nt(378), (t(28)));
		//       error_sym(378)       => 'e' 'r' 'r' 'o' 'r'.
		q(nt(378), (t(28)+t(45)+t(45)+t(31)+t(45)));
		//       info_sym(381)        => 'i'.
		q(nt(381), (t(41)));
		//       info_sym(381)        => 'i' 'n' 'f' 'o'.
		q(nt(381), (t(41)+t(49)+t(36)+t(31)));
		//       debug_sym(379)       => 'd'.
		q(nt(379), (t(53)));
		//       debug_sym(379)       => 'd' 'e' 'b' 'u' 'g'.
		q(nt(379), (t(53)+t(28)+t(30)+t(44)+t(50)));
		//       trace_sym(380)       => 't'.
		q(nt(380), (t(52)));
		//       trace_sym(380)       => 't' 'r' 'a' 'c' 'e'.
		q(nt(380), (t(52)+t(45)+t(26)+t(47)+t(28)));
		return q;
	}
};

#endif // __TAU_PARSER_H__
