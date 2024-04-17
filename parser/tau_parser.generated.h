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
		g(nts, load_prods(), nt(247), cc, load_grammar_opts()),
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
		__, comment, __E_comment_0, __E_comment_1, __E_comment_2, __E____3, __E___4, dot, open_parenthesis, close_parenthesis, 
		open_bracket, close_bracket, open_brace, close_brace, minus, colon, semicolon, less, comma, apostrophe, 
		quote, tau_def, tau_builder_def, tau_and_sym, tau_or_sym, tau_neg_sym, wff_def, wff_builder_def, wff_and_sym, wff_or_sym, 
		wff_xor_sym, wff_conditional_sym, wff_neg_sym, wff_imply_sym, wff_equiv_sym, wff_all_sym, wff_ex_sym, wff_ball_sym, wff_bex_sym, wff_t, 
		wff_f, bf_def, bf_builder_def, bf_and_sym, bf_or_sym, bf_xor_sym, bf_neg_sym, bf_equality_sym, bf_nequality_sym, bf_less_sym, 
		bf_less_equal_sym, bf_not_less_equal_sym, bf_greater_sym, bf_all_sym, bf_ex_sym, bf_splitter_sym, bf_t, bf_f, chars, __E_chars_5, 
		__E_chars_6, digits, __E_digits_7, sym, offsets, offset, __E_offsets_8, __E_offsets_9, num, capture, 
		shift, __E_offset_10, variable, io_var, __E_shift_11, __E___E_shift_11_12, charvar, in, out, in_var_name, 
		out_var_name, bool_variable, __E_charvar_13, __E_charvar_14, __E_charvar_15, capture_var, var, tau_rule, tau_matcher, tau_body, 
		tau, tau_collapse_positives_cb, tau_positives_upwards_cb, tau_rec_relation, tau_ref, __E_tau_ref_16, tau_ref_args, __E_tau_ref_args_17, tau_ref_arg, __E_tau_ref_args_18, 
		bf, tau_or, tau_and, tau_neg, tau_wff, wff, wff_rule, wff_matcher, wff_body, bf_eq_cb, 
		bf_neq_cb, wff_has_clashing_subformulas_cb, wff_has_subformula_cb, wff_remove_existential_cb, wff_remove_bexistential_cb, wff_remove_buniversal_cb, wff_rec_relation, wff_ref, wff_conditional, wff_ball, 
		wff_bex, wff_all, wff_ex, wff_imply, wff_equiv, wff_or, wff_and, wff_xor, wff_neg, bf_interval, 
		bf_neq, bf_eq, bf_not_less_equal, bf_greater, bf_less_equal, bf_less, __E_wff_ref_19, wff_ref_args, __E_wff_ref_args_20, wff_ref_arg, 
		__E_wff_ref_args_21, __E_wff_and_22, __E_wff_all_23, __E_wff_ex_24, __E_wff_ball_25, __E_wff_bex_26, bf_rule, bf_matcher, bf_body, bf_is_zero_cb, 
		bf_is_one_cb, bf_has_subformula_cb, bf_remove_funiversal_cb, bf_remove_fexistential_cb, bf_and_cb, bf_or_cb, bf_xor_cb, bf_neg_cb, bf_splitter, bf_ref, 
		bf_all, bf_ex, bf_or, bf_and, bf_xor, bf_neg, bf_constant, bf_rec_relation, __E_bf_ref_27, bf_ref_args, 
		__E_bf_ref_args_28, bf_ref_arg, __E_bf_ref_args_29, __E_bf_and_30, __E_bf_all_31, __E_bf_ex_32, constant, binding, source_binding, named_binding, 
		type, source, source0, __E_source_33, __E_source_34, bf_and_cb_sym, bf_cb_arg, bf_or_cb_sym, bf_xor_cb_sym, bf_neg_cb_sym, 
		bf_eq_cb_sym, wff_cb_arg, bf_neq_cb_sym, bf_is_zero_cb_sym, bf_is_one_cb_sym, bf_remove_funiversal_cb_sym, bf_remove_fexistential_cb_sym, wff_remove_existential_cb_sym, wff_remove_bexistential_cb_sym, wff_remove_buniversal_cb_sym, 
		wff_has_clashing_subformulas_cb_sym, bf_has_subformula_cb_sym, wff_has_subformula_cb_sym, __E_tau_collapse_positives_cb_35, tau_collapse_positives_cb_sym, tau_cb_arg, __E_tau_collapse_positives_cb_36, tau_positives_upwards_cb_sym, inputs, input, 
		__E_inputs_37, __E_inputs_38, builder, builder_head, builder_body, __E_builder_head_39, __E_builder_head_40, bf_builder_body, wff_builder_body, tau_builder_body, 
		library, rules, __E_rules_41, rule, __E_rules_42, nso_rr, nso_rec_relations, nso_main, __E_nso_rec_relations_43, nso_rec_relation, 
		__E_nso_rec_relations_44, gssotc_rr, gssotc_rec_relations, gssotc_main, __E_gssotc_rec_relations_45, gssotc_rec_relation, __E_gssotc_rec_relations_46, start, __E_start_47, cli, 
		cli_command, __E_cli_48, __E_cli_49, help_cmd, version_cmd, quit_cmd, clear_cmd, get_cmd, set_cmd, toggle_cmd, 
		list_outputs_cmd, clear_outputs_cmd, print_output_cmd, file, normalize_cmd, bf_dnf_cmd, bf_cnf_cmd, bf_anf_cmd, bf_nnf_cmd, bf_pnf_cmd, 
		bf_mnf_cmd, bf_substitute, wff_substitute, bf_instantiate, wff_instantiate, wff_onf_cmd, wff_dnf_cmd, wff_cnf_cmd, wff_anf_cmd, wff_nnf_cmd, 
		wff_pnf_cmd, wff_mnf_cmd, def_wff_cmd, def_bf_cmd, def_tau_cmd, def_list_cmd, def_del_cmd, file_sym, file_path, normalize_cmd_sym, 
		normalize_cmd_arg, output, wff_selection, bf_instantiate_cmd, instantiate_cmd_sym, __E_bf_instantiate_cmd_50, bf_selection, __E_bf_instantiate_cmd_51, wff_instantiate_cmd, __E_wff_instantiate_cmd_52, 
		__E_wff_instantiate_cmd_53, bf_substitute_cmd, substitute_cmd_sym, __E_bf_substitute_cmd_54, __E_bf_substitute_cmd_55, __E_bf_substitute_cmd_56, wff_substitute_cmd, substitutecmd__sym, __E_wff_substitute_cmd_57, __E_wff_substitute_cmd_58, 
		__E_wff_substitute_cmd_59, dnf_cmd_sym, __E_bf_dnf_cmd_60, cnf_cmd_sym, __E_bf_cnf_cmd_61, anf_cmd_sym, __E_bf_anf_cmd_62, nnf_cmd_sym, __E_bf_nnf_cmd_63, pnf_cmd_sym, 
		__E_bf_pnf_cmd_64, mnf_cmd_sym, __E_bf_mnf_cmd_65, onf_cmd_sym, __E_wff_onf_cmd_66, __E_wff_dnf_cmd_67, __E_wff_cnf_cmd_68, __E_wff_anf_cmd_69, __E_wff_nnf_cmd_70, __E_wff_pnf_cmd_71, 
		__E_wff_mnf_cmd_72, def_cmd_sym, def_clear_cmd, help_cmd_sym, __E_help_cmd_73, cli_cmd_sym, version_cmd_sym, quit_cmd_sym, clear_cmd_sym, selection_sym, 
		bf_var_selection, wff_var_selection, absolute_output, relative_output, __E_absolute_output_74, absolute_output_sym, output_sym, output_id, relative_output_sym, __E_clear_outputs_cmd_75, 
		clear_sym, file_cmd_sym, get_cmd_sym, set_cmd_sym, toggle_cmd_sym, instantiate_sym, substitute_sym, __E_get_cmd_76, option, __E_set_cmd_77, 
		option_value, bool_option, severity_opt, status_opt, colors_opt, debug_repl_opt, option_value_true, option_value_false, severity, error_sym, 
		debug_sym, trace_sym, info_sym, __N_0, __N_1, __N_2, __N_3, 
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
			"__", "comment", "__E_comment_0", "__E_comment_1", "__E_comment_2", "__E____3", "__E___4", "dot", "open_parenthesis", "close_parenthesis", 
			"open_bracket", "close_bracket", "open_brace", "close_brace", "minus", "colon", "semicolon", "less", "comma", "apostrophe", 
			"quote", "tau_def", "tau_builder_def", "tau_and_sym", "tau_or_sym", "tau_neg_sym", "wff_def", "wff_builder_def", "wff_and_sym", "wff_or_sym", 
			"wff_xor_sym", "wff_conditional_sym", "wff_neg_sym", "wff_imply_sym", "wff_equiv_sym", "wff_all_sym", "wff_ex_sym", "wff_ball_sym", "wff_bex_sym", "wff_t", 
			"wff_f", "bf_def", "bf_builder_def", "bf_and_sym", "bf_or_sym", "bf_xor_sym", "bf_neg_sym", "bf_equality_sym", "bf_nequality_sym", "bf_less_sym", 
			"bf_less_equal_sym", "bf_not_less_equal_sym", "bf_greater_sym", "bf_all_sym", "bf_ex_sym", "bf_splitter_sym", "bf_t", "bf_f", "chars", "__E_chars_5", 
			"__E_chars_6", "digits", "__E_digits_7", "sym", "offsets", "offset", "__E_offsets_8", "__E_offsets_9", "num", "capture", 
			"shift", "__E_offset_10", "variable", "io_var", "__E_shift_11", "__E___E_shift_11_12", "charvar", "in", "out", "in_var_name", 
			"out_var_name", "bool_variable", "__E_charvar_13", "__E_charvar_14", "__E_charvar_15", "capture_var", "var", "tau_rule", "tau_matcher", "tau_body", 
			"tau", "tau_collapse_positives_cb", "tau_positives_upwards_cb", "tau_rec_relation", "tau_ref", "__E_tau_ref_16", "tau_ref_args", "__E_tau_ref_args_17", "tau_ref_arg", "__E_tau_ref_args_18", 
			"bf", "tau_or", "tau_and", "tau_neg", "tau_wff", "wff", "wff_rule", "wff_matcher", "wff_body", "bf_eq_cb", 
			"bf_neq_cb", "wff_has_clashing_subformulas_cb", "wff_has_subformula_cb", "wff_remove_existential_cb", "wff_remove_bexistential_cb", "wff_remove_buniversal_cb", "wff_rec_relation", "wff_ref", "wff_conditional", "wff_ball", 
			"wff_bex", "wff_all", "wff_ex", "wff_imply", "wff_equiv", "wff_or", "wff_and", "wff_xor", "wff_neg", "bf_interval", 
			"bf_neq", "bf_eq", "bf_not_less_equal", "bf_greater", "bf_less_equal", "bf_less", "__E_wff_ref_19", "wff_ref_args", "__E_wff_ref_args_20", "wff_ref_arg", 
			"__E_wff_ref_args_21", "__E_wff_and_22", "__E_wff_all_23", "__E_wff_ex_24", "__E_wff_ball_25", "__E_wff_bex_26", "bf_rule", "bf_matcher", "bf_body", "bf_is_zero_cb", 
			"bf_is_one_cb", "bf_has_subformula_cb", "bf_remove_funiversal_cb", "bf_remove_fexistential_cb", "bf_and_cb", "bf_or_cb", "bf_xor_cb", "bf_neg_cb", "bf_splitter", "bf_ref", 
			"bf_all", "bf_ex", "bf_or", "bf_and", "bf_xor", "bf_neg", "bf_constant", "bf_rec_relation", "__E_bf_ref_27", "bf_ref_args", 
			"__E_bf_ref_args_28", "bf_ref_arg", "__E_bf_ref_args_29", "__E_bf_and_30", "__E_bf_all_31", "__E_bf_ex_32", "constant", "binding", "source_binding", "named_binding", 
			"type", "source", "source0", "__E_source_33", "__E_source_34", "bf_and_cb_sym", "bf_cb_arg", "bf_or_cb_sym", "bf_xor_cb_sym", "bf_neg_cb_sym", 
			"bf_eq_cb_sym", "wff_cb_arg", "bf_neq_cb_sym", "bf_is_zero_cb_sym", "bf_is_one_cb_sym", "bf_remove_funiversal_cb_sym", "bf_remove_fexistential_cb_sym", "wff_remove_existential_cb_sym", "wff_remove_bexistential_cb_sym", "wff_remove_buniversal_cb_sym", 
			"wff_has_clashing_subformulas_cb_sym", "bf_has_subformula_cb_sym", "wff_has_subformula_cb_sym", "__E_tau_collapse_positives_cb_35", "tau_collapse_positives_cb_sym", "tau_cb_arg", "__E_tau_collapse_positives_cb_36", "tau_positives_upwards_cb_sym", "inputs", "input", 
			"__E_inputs_37", "__E_inputs_38", "builder", "builder_head", "builder_body", "__E_builder_head_39", "__E_builder_head_40", "bf_builder_body", "wff_builder_body", "tau_builder_body", 
			"library", "rules", "__E_rules_41", "rule", "__E_rules_42", "nso_rr", "nso_rec_relations", "nso_main", "__E_nso_rec_relations_43", "nso_rec_relation", 
			"__E_nso_rec_relations_44", "gssotc_rr", "gssotc_rec_relations", "gssotc_main", "__E_gssotc_rec_relations_45", "gssotc_rec_relation", "__E_gssotc_rec_relations_46", "start", "__E_start_47", "cli", 
			"cli_command", "__E_cli_48", "__E_cli_49", "help_cmd", "version_cmd", "quit_cmd", "clear_cmd", "get_cmd", "set_cmd", "toggle_cmd", 
			"list_outputs_cmd", "clear_outputs_cmd", "print_output_cmd", "file", "normalize_cmd", "bf_dnf_cmd", "bf_cnf_cmd", "bf_anf_cmd", "bf_nnf_cmd", "bf_pnf_cmd", 
			"bf_mnf_cmd", "bf_substitute", "wff_substitute", "bf_instantiate", "wff_instantiate", "wff_onf_cmd", "wff_dnf_cmd", "wff_cnf_cmd", "wff_anf_cmd", "wff_nnf_cmd", 
			"wff_pnf_cmd", "wff_mnf_cmd", "def_wff_cmd", "def_bf_cmd", "def_tau_cmd", "def_list_cmd", "def_del_cmd", "file_sym", "file_path", "normalize_cmd_sym", 
			"normalize_cmd_arg", "output", "wff_selection", "bf_instantiate_cmd", "instantiate_cmd_sym", "__E_bf_instantiate_cmd_50", "bf_selection", "__E_bf_instantiate_cmd_51", "wff_instantiate_cmd", "__E_wff_instantiate_cmd_52", 
			"__E_wff_instantiate_cmd_53", "bf_substitute_cmd", "substitute_cmd_sym", "__E_bf_substitute_cmd_54", "__E_bf_substitute_cmd_55", "__E_bf_substitute_cmd_56", "wff_substitute_cmd", "substitutecmd__sym", "__E_wff_substitute_cmd_57", "__E_wff_substitute_cmd_58", 
			"__E_wff_substitute_cmd_59", "dnf_cmd_sym", "__E_bf_dnf_cmd_60", "cnf_cmd_sym", "__E_bf_cnf_cmd_61", "anf_cmd_sym", "__E_bf_anf_cmd_62", "nnf_cmd_sym", "__E_bf_nnf_cmd_63", "pnf_cmd_sym", 
			"__E_bf_pnf_cmd_64", "mnf_cmd_sym", "__E_bf_mnf_cmd_65", "onf_cmd_sym", "__E_wff_onf_cmd_66", "__E_wff_dnf_cmd_67", "__E_wff_cnf_cmd_68", "__E_wff_anf_cmd_69", "__E_wff_nnf_cmd_70", "__E_wff_pnf_cmd_71", 
			"__E_wff_mnf_cmd_72", "def_cmd_sym", "def_clear_cmd", "help_cmd_sym", "__E_help_cmd_73", "cli_cmd_sym", "version_cmd_sym", "quit_cmd_sym", "clear_cmd_sym", "selection_sym", 
			"bf_var_selection", "wff_var_selection", "absolute_output", "relative_output", "__E_absolute_output_74", "absolute_output_sym", "output_sym", "output_id", "relative_output_sym", "__E_clear_outputs_cmd_75", 
			"clear_sym", "file_cmd_sym", "get_cmd_sym", "set_cmd_sym", "toggle_cmd_sym", "instantiate_sym", "substitute_sym", "__E_get_cmd_76", "option", "__E_set_cmd_77", 
			"option_value", "bool_option", "severity_opt", "status_opt", "colors_opt", "debug_repl_opt", "option_value_true", "option_value_false", "severity", "error_sym", 
			"debug_sym", "trace_sym", "info_sym", "__N_0", "__N_1", "__N_2", "__N_3", 
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
		o.to_trim = { 9, 10 };
		return o;
	}
	options load_opts() {
		options o;
		return o;
	}
	idni::prods<char_type, terminal_type> load_prods() {
		idni::prods<char_type, terminal_type> q,
			nul(symbol_type{});
		//       __E_comment_0(12)    => printable(8).
		q(nt(12), (nt(8)));
		//       __E_comment_0(12)    => '\t'.
		q(nt(12), (t(1)));
		//       __E_comment_1(13)    => null.
		q(nt(13), (nul));
		//       __E_comment_1(13)    => __E_comment_0(12) __E_comment_1(13).
		q(nt(13), (nt(12)+nt(13)));
		//       __E_comment_2(14)    => '\n'.
		q(nt(14), (t(2)));
		//       __E_comment_2(14)    => '\r'.
		q(nt(14), (t(3)));
		//       __E_comment_2(14)    => eof(1).
		q(nt(14), (nt(1)));
		//       comment(11)          => '#' __E_comment_1(13) __E_comment_2(14).
		q(nt(11), (t(4)+nt(13)+nt(14)));
		//       __E____3(15)         => space(2).
		q(nt(15), (nt(2)));
		//       __E____3(15)         => comment(11).
		q(nt(15), (nt(11)));
		//       __(10)               => __E____3(15) _(9).
		q(nt(10), (nt(15)+nt(9)));
		//       __E___4(16)          => __(10).
		q(nt(16), (nt(10)));
		//       __E___4(16)          => null.
		q(nt(16), (nul));
		//       _(9)                 => __E___4(16).
		q(nt(9), (nt(16)));
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
		//       tau_builder_def(32)  => '=' ':' ':' ':'.
		q(nt(32), (t(19)+t(13)+t(13)+t(13)));
		//       tau_and_sym(33)      => '&' '&' '&'.
		q(nt(33), (t(20)+t(20)+t(20)));
		//       tau_or_sym(34)       => '|' '|' '|'.
		q(nt(34), (t(21)+t(21)+t(21)));
		//       tau_neg_sym(35)      => '!' '!' '!'.
		q(nt(35), (t(22)+t(22)+t(22)));
		//       wff_def(36)          => ':' ':' '='.
		q(nt(36), (t(13)+t(13)+t(19)));
		//       wff_builder_def(37)  => '=' ':' ':'.
		q(nt(37), (t(19)+t(13)+t(13)));
		//       wff_and_sym(38)      => '&' '&'.
		q(nt(38), (t(20)+t(20)));
		//       wff_or_sym(39)       => '|' '|'.
		q(nt(39), (t(21)+t(21)));
		//       wff_xor_sym(40)      => '^'.
		q(nt(40), (t(23)));
		//       wff_conditional_sym(41) => '?'.
		q(nt(41), (t(24)));
		//       wff_neg_sym(42)      => '!'.
		q(nt(42), (t(22)));
		//       wff_imply_sym(43)    => '-' '>'.
		q(nt(43), (t(12)+t(25)));
		//       wff_equiv_sym(44)    => '<' '-' '>'.
		q(nt(44), (t(15)+t(12)+t(25)));
		//       wff_all_sym(45)      => 'a' 'l' 'l'.
		q(nt(45), (t(26)+t(27)+t(27)));
		//       wff_ex_sym(46)       => 'e' 'x'.
		q(nt(46), (t(28)+t(29)));
		//       wff_ball_sym(47)     => 'b' 'o' 'o' 'l' '_' 'a' 'l' 'l'.
		q(nt(47), (t(30)+t(31)+t(31)+t(27)+t(32)+t(26)+t(27)+t(27)));
		//       wff_bex_sym(48)      => 'b' 'o' 'o' 'l' '_' 'e' 'x'.
		q(nt(48), (t(30)+t(31)+t(31)+t(27)+t(32)+t(28)+t(29)));
		//       wff_t(49)            => 'T'.
		q(nt(49), (t(33)));
		//       wff_f(50)            => 'F'.
		q(nt(50), (t(34)));
		//       bf_def(51)           => ':' '='.
		q(nt(51), (t(13)+t(19)));
		//       bf_builder_def(52)   => '=' ':'.
		q(nt(52), (t(19)+t(13)));
		//       bf_and_sym(53)       => '&'.
		q(nt(53), (t(20)));
		//       bf_or_sym(54)        => '|'.
		q(nt(54), (t(21)));
		//       bf_xor_sym(55)       => '+'.
		q(nt(55), (t(35)));
		//       bf_neg_sym(56)       => apostrophe(29).
		q(nt(56), (nt(29)));
		//       bf_equality_sym(57)  => '='.
		q(nt(57), (t(19)));
		//       bf_nequality_sym(58) => '!' '='.
		q(nt(58), (t(22)+t(19)));
		//       bf_less_sym(59)      => '<'.
		q(nt(59), (t(15)));
		//       bf_less_equal_sym(60) => '<' '='.
		q(nt(60), (t(15)+t(19)));
		//       bf_not_less_equal_sym(61) => '!' '<' '='.
		q(nt(61), (t(22)+t(15)+t(19)));
		//       bf_greater_sym(62)   => '>'.
		q(nt(62), (t(25)));
		//       bf_all_sym(63)       => 'f' 'a' 'l' 'l'.
		q(nt(63), (t(36)+t(26)+t(27)+t(27)));
		//       bf_ex_sym(64)        => 'f' 'e' 'x'.
		q(nt(64), (t(36)+t(28)+t(29)));
		//       bf_splitter_sym(65)  => 'S'.
		q(nt(65), (t(37)));
		//       bf_t(66)             => '1'.
		q(nt(66), (t(38)));
		//       bf_f(67)             => '0'.
		q(nt(67), (t(39)));
		//       __E_chars_5(69)      => alnum(6).
		q(nt(69), (nt(6)));
		//       __E_chars_6(70)      => null.
		q(nt(70), (nul));
		//       __E_chars_6(70)      => __E_chars_5(69) __E_chars_6(70).
		q(nt(70), (nt(69)+nt(70)));
		//       chars(68)            => alpha(5) __E_chars_6(70).
		q(nt(68), (nt(5)+nt(70)));
		//       __E_digits_7(72)     => digit(3).
		q(nt(72), (nt(3)));
		//       __E_digits_7(72)     => digit(3) __E_digits_7(72).
		q(nt(72), (nt(3)+nt(72)));
		//       digits(71)           => __E_digits_7(72).
		q(nt(71), (nt(72)));
		//       sym(73)              => chars(68).
		q(nt(73), (nt(68)));
		//       __E_offsets_8(76)    => _(9) comma(28) _(9) offset(75).
		q(nt(76), (nt(9)+nt(28)+nt(9)+nt(75)));
		//       __E_offsets_9(77)    => null.
		q(nt(77), (nul));
		//       __E_offsets_9(77)    => __E_offsets_8(76) __E_offsets_9(77).
		q(nt(77), (nt(76)+nt(77)));
		//       offsets(74)          => open_bracket(20) _(9) offset(75) __E_offsets_9(77) _(9) close_bracket(21).
		q(nt(74), (nt(20)+nt(9)+nt(75)+nt(77)+nt(9)+nt(21)));
		//       offset(75)           => num(78).
		q(nt(75), (nt(78)));
		//       offset(75)           => capture(79).
		q(nt(75), (nt(79)));
		//       offset(75)           => shift(80).
		q(nt(75), (nt(80)));
		//       __N_0(373)           => io_var(83).
		q(nt(373), (nt(83)));
		//       __E_offset_10(81)    => variable(82) & ~( __N_0(373) ).	 # conjunctive
		q(nt(81), (nt(82)) & ~(nt(373)));
		//       offset(75)           => __E_offset_10(81).
		q(nt(75), (nt(81)));
		//       __E_shift_11(84)     => capture(79).
		q(nt(84), (nt(79)));
		//       __N_1(374)           => io_var(83).
		q(nt(374), (nt(83)));
		//       __E___E_shift_11_12(85) => variable(82) & ~( __N_1(374) ).	 # conjunctive
		q(nt(85), (nt(82)) & ~(nt(374)));
		//       __E_shift_11(84)     => __E___E_shift_11_12(85).
		q(nt(84), (nt(85)));
		//       shift(80)            => __E_shift_11(84) _(9) minus(24) _(9) num(78).
		q(nt(80), (nt(84)+nt(9)+nt(24)+nt(9)+nt(78)));
		//       num(78)              => digits(71).
		q(nt(78), (nt(71)));
		//       variable(82)         => charvar(86).
		q(nt(82), (nt(86)));
		//       variable(82)         => io_var(83).
		q(nt(82), (nt(83)));
		//       io_var(83)           => in(87).
		q(nt(83), (nt(87)));
		//       io_var(83)           => out(88).
		q(nt(83), (nt(88)));
		//       in(87)               => in_var_name(89) open_bracket(20) offset(75) close_bracket(21).
		q(nt(87), (nt(89)+nt(20)+nt(75)+nt(21)));
		//       out(88)              => out_var_name(90) open_bracket(20) offset(75) close_bracket(21).
		q(nt(88), (nt(90)+nt(20)+nt(75)+nt(21)));
		//       bool_variable(91)    => '?' charvar(86).
		q(nt(91), (t(24)+nt(86)));
		//       __N_2(375)           => 'F'.
		q(nt(375), (t(34)));
		//       __N_3(376)           => 'T'.
		q(nt(376), (t(33)));
		//       __E_charvar_13(92)   => ~( __N_2(375) ) & ~( __N_3(376) ) & alpha(5).	 # conjunctive
		q(nt(92), ~(nt(375)) & ~(nt(376)) & (nt(5)));
		//       __E_charvar_14(93)   => digit(3).
		q(nt(93), (nt(3)));
		//       __E_charvar_15(94)   => null.
		q(nt(94), (nul));
		//       __E_charvar_15(94)   => __E_charvar_14(93) __E_charvar_15(94).
		q(nt(94), (nt(93)+nt(94)));
		//       charvar(86)          => __E_charvar_13(92) __E_charvar_15(94).
		q(nt(86), (nt(92)+nt(94)));
		//       capture(79)          => capture_var(95).
		q(nt(79), (nt(95)));
		//       capture_var(95)      => '$' chars(68).
		q(nt(95), (t(40)+nt(68)));
		//       var(96)              => chars(68).
		q(nt(96), (nt(68)));
		//       in_var_name(89)      => 'i' '_' chars(68).
		q(nt(89), (t(41)+t(32)+nt(68)));
		//       out_var_name(90)     => 'o' '_' chars(68).
		q(nt(90), (t(31)+t(32)+nt(68)));
		//       tau_rule(97)         => tau_matcher(98) _(9) tau_def(31) _(9) tau_body(99) _(9) dot(17).
		q(nt(97), (nt(98)+nt(9)+nt(31)+nt(9)+nt(99)+nt(9)+nt(17)));
		//       tau_matcher(98)      => tau(100).
		q(nt(98), (nt(100)));
		//       tau_body(99)         => tau(100).
		q(nt(99), (nt(100)));
		//       tau_body(99)         => tau_collapse_positives_cb(101).
		q(nt(99), (nt(101)));
		//       tau_body(99)         => tau_positives_upwards_cb(102).
		q(nt(99), (nt(102)));
		//       tau_rec_relation(103) => tau_ref(104) _(9) tau_def(31) _(9) tau(100) _(9) dot(17).
		q(nt(103), (nt(104)+nt(9)+nt(31)+nt(9)+nt(100)+nt(9)+nt(17)));
		//       __E_tau_ref_16(105)  => offsets(74).
		q(nt(105), (nt(74)));
		//       __E_tau_ref_16(105)  => null.
		q(nt(105), (nul));
		//       tau_ref(104)         => sym(73) _(9) __E_tau_ref_16(105) _(9) tau_ref_args(106).
		q(nt(104), (nt(73)+nt(9)+nt(105)+nt(9)+nt(106)));
		//       __E_tau_ref_args_17(107) => _(9) tau_ref_arg(108).
		q(nt(107), (nt(9)+nt(108)));
		//       __E_tau_ref_args_18(109) => null.
		q(nt(109), (nul));
		//       __E_tau_ref_args_18(109) => __E_tau_ref_args_17(107) __E_tau_ref_args_18(109).
		q(nt(109), (nt(107)+nt(109)));
		//       tau_ref_args(106)    => open_parenthesis(18) __E_tau_ref_args_18(109) _(9) close_parenthesis(19).
		q(nt(106), (nt(18)+nt(109)+nt(9)+nt(19)));
		//       tau_ref_arg(108)     => bf(110).
		q(nt(108), (nt(110)));
		//       tau_ref_arg(108)     => capture(79).
		q(nt(108), (nt(79)));
		//       tau_ref_arg(108)     => variable(82).
		q(nt(108), (nt(82)));
		//       tau(100)             => tau_ref(104).
		q(nt(100), (nt(104)));
		//       tau(100)             => tau_or(111).
		q(nt(100), (nt(111)));
		//       tau(100)             => tau_and(112).
		q(nt(100), (nt(112)));
		//       tau(100)             => tau_neg(113).
		q(nt(100), (nt(113)));
		//       tau(100)             => tau_wff(114).
		q(nt(100), (nt(114)));
		//       tau(100)             => capture(79).
		q(nt(100), (nt(79)));
		//       tau(100)             => open_parenthesis(18) _(9) tau(100) _(9) close_parenthesis(19).
		q(nt(100), (nt(18)+nt(9)+nt(100)+nt(9)+nt(19)));
		//       tau_and(112)         => tau(100) _(9) tau_and_sym(33) _(9) tau(100).
		q(nt(112), (nt(100)+nt(9)+nt(33)+nt(9)+nt(100)));
		//       tau_or(111)          => tau(100) _(9) tau_or_sym(34) _(9) tau(100).
		q(nt(111), (nt(100)+nt(9)+nt(34)+nt(9)+nt(100)));
		//       tau_neg(113)         => tau_neg_sym(35) _(9) tau(100).
		q(nt(113), (nt(35)+nt(9)+nt(100)));
		//       tau_wff(114)         => open_brace(22) _(9) wff(115) _(9) close_brace(23).
		q(nt(114), (nt(22)+nt(9)+nt(115)+nt(9)+nt(23)));
		//       wff_rule(116)        => wff_matcher(117) _(9) wff_def(36) _(9) wff_body(118) _(9) dot(17).
		q(nt(116), (nt(117)+nt(9)+nt(36)+nt(9)+nt(118)+nt(9)+nt(17)));
		//       wff_matcher(117)     => wff(115).
		q(nt(117), (nt(115)));
		//       wff_body(118)        => bf_eq_cb(119).
		q(nt(118), (nt(119)));
		//       wff_body(118)        => bf_neq_cb(120).
		q(nt(118), (nt(120)));
		//       wff_body(118)        => wff_has_clashing_subformulas_cb(121).
		q(nt(118), (nt(121)));
		//       wff_body(118)        => wff_has_subformula_cb(122).
		q(nt(118), (nt(122)));
		//       wff_body(118)        => wff_remove_existential_cb(123).
		q(nt(118), (nt(123)));
		//       wff_body(118)        => wff_remove_bexistential_cb(124).
		q(nt(118), (nt(124)));
		//       wff_body(118)        => wff_remove_buniversal_cb(125).
		q(nt(118), (nt(125)));
		//       wff_body(118)        => wff(115).
		q(nt(118), (nt(115)));
		//       wff_rec_relation(126) => wff_ref(127) _(9) wff_def(36) _(9) wff(115) _(9) dot(17).
		q(nt(126), (nt(127)+nt(9)+nt(36)+nt(9)+nt(115)+nt(9)+nt(17)));
		//       wff(115)             => wff_ref(127).
		q(nt(115), (nt(127)));
		//       wff(115)             => wff_conditional(128).
		q(nt(115), (nt(128)));
		//       wff(115)             => wff_ball(129).
		q(nt(115), (nt(129)));
		//       wff(115)             => wff_bex(130).
		q(nt(115), (nt(130)));
		//       wff(115)             => wff_all(131).
		q(nt(115), (nt(131)));
		//       wff(115)             => wff_ex(132).
		q(nt(115), (nt(132)));
		//       wff(115)             => wff_imply(133).
		q(nt(115), (nt(133)));
		//       wff(115)             => wff_equiv(134).
		q(nt(115), (nt(134)));
		//       wff(115)             => wff_or(135).
		q(nt(115), (nt(135)));
		//       wff(115)             => wff_and(136).
		q(nt(115), (nt(136)));
		//       wff(115)             => wff_xor(137).
		q(nt(115), (nt(137)));
		//       wff(115)             => wff_neg(138).
		q(nt(115), (nt(138)));
		//       wff(115)             => wff_t(49).
		q(nt(115), (nt(49)));
		//       wff(115)             => wff_f(50).
		q(nt(115), (nt(50)));
		//       wff(115)             => bf_interval(139).
		q(nt(115), (nt(139)));
		//       wff(115)             => bf_neq(140).
		q(nt(115), (nt(140)));
		//       wff(115)             => bf_eq(141).
		q(nt(115), (nt(141)));
		//       wff(115)             => bf_not_less_equal(142).
		q(nt(115), (nt(142)));
		//       wff(115)             => bf_greater(143).
		q(nt(115), (nt(143)));
		//       wff(115)             => bf_less_equal(144).
		q(nt(115), (nt(144)));
		//       wff(115)             => bf_less(145).
		q(nt(115), (nt(145)));
		//       wff(115)             => capture(79).
		q(nt(115), (nt(79)));
		//       wff(115)             => bool_variable(91).
		q(nt(115), (nt(91)));
		//       wff(115)             => open_parenthesis(18) _(9) wff(115) _(9) close_parenthesis(19).
		q(nt(115), (nt(18)+nt(9)+nt(115)+nt(9)+nt(19)));
		//       __E_wff_ref_19(146)  => offsets(74).
		q(nt(146), (nt(74)));
		//       __E_wff_ref_19(146)  => null.
		q(nt(146), (nul));
		//       wff_ref(127)         => sym(73) _(9) __E_wff_ref_19(146) _(9) wff_ref_args(147).
		q(nt(127), (nt(73)+nt(9)+nt(146)+nt(9)+nt(147)));
		//       __E_wff_ref_args_20(148) => _(9) wff_ref_arg(149).
		q(nt(148), (nt(9)+nt(149)));
		//       __E_wff_ref_args_21(150) => null.
		q(nt(150), (nul));
		//       __E_wff_ref_args_21(150) => __E_wff_ref_args_20(148) __E_wff_ref_args_21(150).
		q(nt(150), (nt(148)+nt(150)));
		//       wff_ref_args(147)    => open_parenthesis(18) __E_wff_ref_args_21(150) _(9) close_parenthesis(19).
		q(nt(147), (nt(18)+nt(150)+nt(9)+nt(19)));
		//       wff_ref_arg(149)     => bf(110).
		q(nt(149), (nt(110)));
		//       wff_ref_arg(149)     => capture(79).
		q(nt(149), (nt(79)));
		//       wff_ref_arg(149)     => variable(82).
		q(nt(149), (nt(82)));
		//       __E_wff_and_22(151)  => wff_and_sym(38) _(9).
		q(nt(151), (nt(38)+nt(9)));
		//       __E_wff_and_22(151)  => null.
		q(nt(151), (nul));
		//       wff_and(136)         => wff(115) _(9) __E_wff_and_22(151) wff(115).
		q(nt(136), (nt(115)+nt(9)+nt(151)+nt(115)));
		//       wff_or(135)          => wff(115) _(9) wff_or_sym(39) _(9) wff(115).
		q(nt(135), (nt(115)+nt(9)+nt(39)+nt(9)+nt(115)));
		//       wff_xor(137)         => wff(115) _(9) wff_xor_sym(40) _(9) wff(115).
		q(nt(137), (nt(115)+nt(9)+nt(40)+nt(9)+nt(115)));
		//       wff_conditional(128) => wff(115) _(9) wff_conditional_sym(41) _(9) wff(115) _(9) colon(25) _(9) wff(115).
		q(nt(128), (nt(115)+nt(9)+nt(41)+nt(9)+nt(115)+nt(9)+nt(25)+nt(9)+nt(115)));
		//       wff_neg(138)         => wff_neg_sym(42) _(9) wff(115).
		q(nt(138), (nt(42)+nt(9)+nt(115)));
		//       wff_imply(133)       => wff(115) _(9) wff_imply_sym(43) _(9) wff(115).
		q(nt(133), (nt(115)+nt(9)+nt(43)+nt(9)+nt(115)));
		//       wff_equiv(134)       => wff(115) _(9) wff_equiv_sym(44) _(9) wff(115).
		q(nt(134), (nt(115)+nt(9)+nt(44)+nt(9)+nt(115)));
		//       __E_wff_all_23(152)  => variable(82).
		q(nt(152), (nt(82)));
		//       __E_wff_all_23(152)  => capture(79).
		q(nt(152), (nt(79)));
		//       wff_all(131)         => wff_all_sym(45) __(10) __E_wff_all_23(152) __(10) wff(115).
		q(nt(131), (nt(45)+nt(10)+nt(152)+nt(10)+nt(115)));
		//       __E_wff_ex_24(153)   => variable(82).
		q(nt(153), (nt(82)));
		//       __E_wff_ex_24(153)   => capture(79).
		q(nt(153), (nt(79)));
		//       wff_ex(132)          => wff_ex_sym(46) __(10) __E_wff_ex_24(153) __(10) wff(115).
		q(nt(132), (nt(46)+nt(10)+nt(153)+nt(10)+nt(115)));
		//       __E_wff_ball_25(154) => bool_variable(91).
		q(nt(154), (nt(91)));
		//       __E_wff_ball_25(154) => capture(79).
		q(nt(154), (nt(79)));
		//       wff_ball(129)        => wff_ball_sym(47) __(10) __E_wff_ball_25(154) __(10) wff(115).
		q(nt(129), (nt(47)+nt(10)+nt(154)+nt(10)+nt(115)));
		//       __E_wff_bex_26(155)  => bool_variable(91).
		q(nt(155), (nt(91)));
		//       __E_wff_bex_26(155)  => capture(79).
		q(nt(155), (nt(79)));
		//       wff_bex(130)         => wff_bex_sym(48) __(10) __E_wff_bex_26(155) __(10) wff(115).
		q(nt(130), (nt(48)+nt(10)+nt(155)+nt(10)+nt(115)));
		//       bf_eq(141)           => bf(110) _(9) bf_equality_sym(57) _(9) bf(110).
		q(nt(141), (nt(110)+nt(9)+nt(57)+nt(9)+nt(110)));
		//       bf_neq(140)          => bf(110) _(9) bf_nequality_sym(58) _(9) bf(110).
		q(nt(140), (nt(110)+nt(9)+nt(58)+nt(9)+nt(110)));
		//       bf_less(145)         => bf(110) _(9) bf_less_sym(59) _(9) bf(110).
		q(nt(145), (nt(110)+nt(9)+nt(59)+nt(9)+nt(110)));
		//       bf_less_equal(144)   => bf(110) _(9) bf_less_equal_sym(60) _(9) bf(110).
		q(nt(144), (nt(110)+nt(9)+nt(60)+nt(9)+nt(110)));
		//       bf_not_less_equal(142) => bf(110) _(9) bf_not_less_equal_sym(61) _(9) bf(110).
		q(nt(142), (nt(110)+nt(9)+nt(61)+nt(9)+nt(110)));
		//       bf_greater(143)      => bf(110) _(9) bf_greater_sym(62) _(9) bf(110).
		q(nt(143), (nt(110)+nt(9)+nt(62)+nt(9)+nt(110)));
		//       bf_interval(139)     => bf(110) _(9) bf_less_equal_sym(60) _(9) bf(110) _(9) bf_less_equal_sym(60) _(9) bf(110).
		q(nt(139), (nt(110)+nt(9)+nt(60)+nt(9)+nt(110)+nt(9)+nt(60)+nt(9)+nt(110)));
		//       bf_rule(156)         => bf_matcher(157) _(9) bf_def(51) _(9) bf_body(158) _(9) dot(17).
		q(nt(156), (nt(157)+nt(9)+nt(51)+nt(9)+nt(158)+nt(9)+nt(17)));
		//       bf_matcher(157)      => bf(110).
		q(nt(157), (nt(110)));
		//       bf_body(158)         => bf(110).
		q(nt(158), (nt(110)));
		//       bf_body(158)         => bf_is_zero_cb(159).
		q(nt(158), (nt(159)));
		//       bf_body(158)         => bf_is_one_cb(160).
		q(nt(158), (nt(160)));
		//       bf_body(158)         => bf_has_subformula_cb(161).
		q(nt(158), (nt(161)));
		//       bf_body(158)         => bf_remove_funiversal_cb(162).
		q(nt(158), (nt(162)));
		//       bf_body(158)         => bf_remove_fexistential_cb(163).
		q(nt(158), (nt(163)));
		//       bf_body(158)         => bf_and_cb(164).
		q(nt(158), (nt(164)));
		//       bf_body(158)         => bf_or_cb(165).
		q(nt(158), (nt(165)));
		//       bf_body(158)         => bf_xor_cb(166).
		q(nt(158), (nt(166)));
		//       bf_body(158)         => bf_neg_cb(167).
		q(nt(158), (nt(167)));
		//       bf_body(158)         => bf_not_less_equal(142).
		q(nt(158), (nt(142)));
		//       bf(110)              => bf_splitter(168).
		q(nt(110), (nt(168)));
		//       bf(110)              => bf_ref(169).
		q(nt(110), (nt(169)));
		//       bf(110)              => bf_all(170).
		q(nt(110), (nt(170)));
		//       bf(110)              => bf_ex(171).
		q(nt(110), (nt(171)));
		//       bf(110)              => bf_or(172).
		q(nt(110), (nt(172)));
		//       bf(110)              => bf_and(173).
		q(nt(110), (nt(173)));
		//       bf(110)              => bf_xor(174).
		q(nt(110), (nt(174)));
		//       bf(110)              => bf_neg(175).
		q(nt(110), (nt(175)));
		//       bf(110)              => bf_constant(176).
		q(nt(110), (nt(176)));
		//       bf(110)              => bf_t(66).
		q(nt(110), (nt(66)));
		//       bf(110)              => bf_f(67).
		q(nt(110), (nt(67)));
		//       bf(110)              => capture(79).
		q(nt(110), (nt(79)));
		//       bf(110)              => variable(82).
		q(nt(110), (nt(82)));
		//       bf(110)              => open_parenthesis(18) _(9) bf(110) _(9) close_parenthesis(19).
		q(nt(110), (nt(18)+nt(9)+nt(110)+nt(9)+nt(19)));
		//       bf_rec_relation(177) => bf_ref(169) _(9) bf_def(51) _(9) bf(110) _(9) dot(17).
		q(nt(177), (nt(169)+nt(9)+nt(51)+nt(9)+nt(110)+nt(9)+nt(17)));
		//       __E_bf_ref_27(178)   => offsets(74).
		q(nt(178), (nt(74)));
		//       __E_bf_ref_27(178)   => null.
		q(nt(178), (nul));
		//       bf_ref(169)          => sym(73) __E_bf_ref_27(178) bf_ref_args(179).
		q(nt(169), (nt(73)+nt(178)+nt(179)));
		//       __E_bf_ref_args_28(180) => _(9) bf_ref_arg(181).
		q(nt(180), (nt(9)+nt(181)));
		//       __E_bf_ref_args_29(182) => null.
		q(nt(182), (nul));
		//       __E_bf_ref_args_29(182) => __E_bf_ref_args_28(180) __E_bf_ref_args_29(182).
		q(nt(182), (nt(180)+nt(182)));
		//       bf_ref_args(179)     => open_parenthesis(18) __E_bf_ref_args_29(182) _(9) close_parenthesis(19).
		q(nt(179), (nt(18)+nt(182)+nt(9)+nt(19)));
		//       bf_ref_arg(181)      => bf(110).
		q(nt(181), (nt(110)));
		//       bf_ref_arg(181)      => capture(79).
		q(nt(181), (nt(79)));
		//       bf_ref_arg(181)      => variable(82).
		q(nt(181), (nt(82)));
		//       __E_bf_and_30(183)   => bf_and_sym(53) _(9).
		q(nt(183), (nt(53)+nt(9)));
		//       __E_bf_and_30(183)   => null.
		q(nt(183), (nul));
		//       bf_and(173)          => bf(110) _(9) __E_bf_and_30(183) bf(110).
		q(nt(173), (nt(110)+nt(9)+nt(183)+nt(110)));
		//       bf_or(172)           => bf(110) _(9) bf_or_sym(54) _(9) bf(110).
		q(nt(172), (nt(110)+nt(9)+nt(54)+nt(9)+nt(110)));
		//       bf_xor(174)          => bf(110) _(9) bf_xor_sym(55) _(9) bf(110).
		q(nt(174), (nt(110)+nt(9)+nt(55)+nt(9)+nt(110)));
		//       bf_neg(175)          => bf(110) _(9) bf_neg_sym(56).
		q(nt(175), (nt(110)+nt(9)+nt(56)));
		//       __E_bf_all_31(184)   => variable(82).
		q(nt(184), (nt(82)));
		//       __E_bf_all_31(184)   => capture(79).
		q(nt(184), (nt(79)));
		//       bf_all(170)          => bf_all_sym(63) __(10) __E_bf_all_31(184) __(10) bf(110).
		q(nt(170), (nt(63)+nt(10)+nt(184)+nt(10)+nt(110)));
		//       __E_bf_ex_32(185)    => variable(82).
		q(nt(185), (nt(82)));
		//       __E_bf_ex_32(185)    => capture(79).
		q(nt(185), (nt(79)));
		//       bf_ex(171)           => bf_ex_sym(64) __(10) __E_bf_ex_32(185) __(10) bf(110).
		q(nt(171), (nt(64)+nt(10)+nt(185)+nt(10)+nt(110)));
		//       bf_splitter(168)     => bf_splitter_sym(65) _(9) open_parenthesis(18) _(9) bf(110) _(9) close_parenthesis(19).
		q(nt(168), (nt(65)+nt(9)+nt(18)+nt(9)+nt(110)+nt(9)+nt(19)));
		//       bf_constant(176)     => open_brace(22) _(9) constant(186) _(9) close_brace(23).
		q(nt(176), (nt(22)+nt(9)+nt(186)+nt(9)+nt(23)));
		//       constant(186)        => binding(187).
		q(nt(186), (nt(187)));
		//       constant(186)        => capture(79).
		q(nt(186), (nt(79)));
		//       binding(187)         => source_binding(188).
		q(nt(187), (nt(188)));
		//       binding(187)         => named_binding(189).
		q(nt(187), (nt(189)));
		//       named_binding(189)   => chars(68).
		q(nt(189), (nt(68)));
		//       source_binding(188)  => type(190) _(9) colon(25) _(9) source(191).
		q(nt(188), (nt(190)+nt(9)+nt(25)+nt(9)+nt(191)));
		//       type(190)            => chars(68).
		q(nt(190), (nt(68)));
		//       type(190)            => null.
		q(nt(190), (nul));
		//       source0(192)         => alnum(6).
		q(nt(192), (nt(6)));
		//       source0(192)         => space(2).
		q(nt(192), (nt(2)));
		//       source0(192)         => punct(7).
		q(nt(192), (nt(7)));
		//       __E_source_33(193)   => source0(192).
		q(nt(193), (nt(192)));
		//       __E_source_34(194)   => __E_source_33(193).
		q(nt(194), (nt(193)));
		//       __E_source_34(194)   => __E_source_33(193) __E_source_34(194).
		q(nt(194), (nt(193)+nt(194)));
		//       source(191)          => __E_source_34(194).
		q(nt(191), (nt(194)));
		//       bf_and_cb(164)       => bf_and_cb_sym(195) __(10) bf_cb_arg(196) __(10) bf_cb_arg(196).
		q(nt(164), (nt(195)+nt(10)+nt(196)+nt(10)+nt(196)));
		//       bf_or_cb(165)        => bf_or_cb_sym(197) __(10) bf_cb_arg(196) __(10) bf_cb_arg(196).
		q(nt(165), (nt(197)+nt(10)+nt(196)+nt(10)+nt(196)));
		//       bf_xor_cb(166)       => bf_xor_cb_sym(198) __(10) bf_cb_arg(196) __(10) bf_cb_arg(196).
		q(nt(166), (nt(198)+nt(10)+nt(196)+nt(10)+nt(196)));
		//       bf_neg_cb(167)       => bf_neg_cb_sym(199) __(10) bf_cb_arg(196).
		q(nt(167), (nt(199)+nt(10)+nt(196)));
		//       bf_eq_cb(119)        => bf_eq_cb_sym(200) __(10) bf_cb_arg(196) __(10) wff_cb_arg(201) __(10) wff_cb_arg(201).
		q(nt(119), (nt(200)+nt(10)+nt(196)+nt(10)+nt(201)+nt(10)+nt(201)));
		//       bf_neq_cb(120)       => bf_neq_cb_sym(202) __(10) bf_cb_arg(196) __(10) wff_cb_arg(201) __(10) wff_cb_arg(201).
		q(nt(120), (nt(202)+nt(10)+nt(196)+nt(10)+nt(201)+nt(10)+nt(201)));
		//       bf_is_zero_cb(159)   => bf_is_zero_cb_sym(203) __(10) bf_cb_arg(196) __(10) bf_cb_arg(196).
		q(nt(159), (nt(203)+nt(10)+nt(196)+nt(10)+nt(196)));
		//       bf_is_one_cb(160)    => bf_is_one_cb_sym(204) __(10) bf_cb_arg(196) __(10) bf_cb_arg(196).
		q(nt(160), (nt(204)+nt(10)+nt(196)+nt(10)+nt(196)));
		//       bf_remove_funiversal_cb(162) => bf_remove_funiversal_cb_sym(205) __(10) bf_cb_arg(196) __(10) bf_cb_arg(196) __(10) bf_cb_arg(196) __(10) bf_cb_arg(196).
		q(nt(162), (nt(205)+nt(10)+nt(196)+nt(10)+nt(196)+nt(10)+nt(196)+nt(10)+nt(196)));
		//       bf_remove_fexistential_cb(163) => bf_remove_fexistential_cb_sym(206) __(10) bf_cb_arg(196) __(10) bf_cb_arg(196) __(10) bf_cb_arg(196) __(10) bf_cb_arg(196).
		q(nt(163), (nt(206)+nt(10)+nt(196)+nt(10)+nt(196)+nt(10)+nt(196)+nt(10)+nt(196)));
		//       wff_remove_existential_cb(123) => wff_remove_existential_cb_sym(207) __(10) wff_cb_arg(201) __(10) wff_cb_arg(201).
		q(nt(123), (nt(207)+nt(10)+nt(201)+nt(10)+nt(201)));
		//       wff_remove_bexistential_cb(124) => wff_remove_bexistential_cb_sym(208) __(10) wff_cb_arg(201) __(10) wff_cb_arg(201) __(10) wff_cb_arg(201) __(10) wff_cb_arg(201).
		q(nt(124), (nt(208)+nt(10)+nt(201)+nt(10)+nt(201)+nt(10)+nt(201)+nt(10)+nt(201)));
		//       wff_remove_buniversal_cb(125) => wff_remove_buniversal_cb_sym(209) __(10) wff_cb_arg(201) __(10) wff_cb_arg(201) __(10) wff_cb_arg(201) __(10) wff_cb_arg(201).
		q(nt(125), (nt(209)+nt(10)+nt(201)+nt(10)+nt(201)+nt(10)+nt(201)+nt(10)+nt(201)));
		//       wff_has_clashing_subformulas_cb(121) => wff_has_clashing_subformulas_cb_sym(210) __(10) wff_cb_arg(201) __(10) wff_cb_arg(201).
		q(nt(121), (nt(210)+nt(10)+nt(201)+nt(10)+nt(201)));
		//       bf_has_subformula_cb(161) => bf_has_subformula_cb_sym(211) __(10) bf_cb_arg(196) __(10) bf_cb_arg(196) __(10) bf_cb_arg(196).
		q(nt(161), (nt(211)+nt(10)+nt(196)+nt(10)+nt(196)+nt(10)+nt(196)));
		//       wff_has_subformula_cb(122) => wff_has_subformula_cb_sym(212) __(10) wff_cb_arg(201) __(10) wff_cb_arg(201) __(10) wff_cb_arg(201).
		q(nt(122), (nt(212)+nt(10)+nt(201)+nt(10)+nt(201)+nt(10)+nt(201)));
		//       bf_has_subformula_cb_sym(211) => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b'.
		q(nt(211), (t(30)+t(36)+t(32)+t(42)+t(26)+t(43)+t(32)+t(43)+t(44)+t(30)+t(36)+t(31)+t(45)+t(46)+t(44)+t(27)+t(26)+t(32)+t(47)+t(30)));
		//       wff_has_clashing_subformulas_cb_sym(210) => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b'.
		q(nt(210), (t(48)+t(36)+t(36)+t(32)+t(42)+t(26)+t(43)+t(32)+t(47)+t(27)+t(26)+t(43)+t(42)+t(41)+t(49)+t(50)+t(32)+t(43)+t(44)+t(30)+t(36)+t(31)+t(45)+t(46)+t(44)+t(27)+t(26)+t(43)+t(32)+t(47)+t(30)));
		//       wff_has_subformula_cb_sym(212) => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b'.
		q(nt(212), (t(48)+t(36)+t(36)+t(32)+t(42)+t(26)+t(43)+t(32)+t(43)+t(44)+t(30)+t(36)+t(31)+t(45)+t(46)+t(44)+t(27)+t(26)+t(32)+t(47)+t(30)));
		//       wff_remove_existential_cb_sym(207) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(207), (t(48)+t(36)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(28)+t(29)+t(41)+t(43)+t(52)+t(28)+t(49)+t(52)+t(41)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       wff_remove_bexistential_cb_sym(208) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(208), (t(48)+t(36)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(30)+t(28)+t(29)+t(41)+t(43)+t(52)+t(28)+t(49)+t(52)+t(41)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       wff_remove_buniversal_cb_sym(209) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b'.
		q(nt(209), (t(48)+t(36)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(30)+t(44)+t(49)+t(41)+t(51)+t(28)+t(45)+t(43)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       bf_remove_fexistential_cb_sym(206) => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(206), (t(30)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(36)+t(28)+t(29)+t(41)+t(43)+t(52)+t(28)+t(49)+t(52)+t(41)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       bf_remove_funiversal_cb_sym(205) => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b'.
		q(nt(205), (t(30)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(36)+t(44)+t(49)+t(41)+t(51)+t(28)+t(45)+t(43)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       bf_cb_arg(196)       => bf(110).
		q(nt(196), (nt(110)));
		//       wff_cb_arg(201)      => wff(115).
		q(nt(201), (nt(115)));
		//       bf_and_cb_sym(195)   => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b'.
		q(nt(195), (t(30)+t(36)+t(32)+t(26)+t(49)+t(53)+t(32)+t(47)+t(30)));
		//       bf_or_cb_sym(197)    => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b'.
		q(nt(197), (t(30)+t(36)+t(32)+t(31)+t(45)+t(32)+t(47)+t(30)));
		//       bf_xor_cb_sym(198)   => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b'.
		q(nt(198), (t(30)+t(36)+t(32)+t(29)+t(31)+t(45)+t(32)+t(47)+t(30)));
		//       bf_neg_cb_sym(199)   => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b'.
		q(nt(199), (t(30)+t(36)+t(32)+t(49)+t(28)+t(50)+t(32)+t(47)+t(30)));
		//       bf_eq_cb_sym(200)    => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b'.
		q(nt(200), (t(30)+t(36)+t(32)+t(28)+t(54)+t(32)+t(47)+t(30)));
		//       bf_neq_cb_sym(202)   => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b'.
		q(nt(202), (t(30)+t(36)+t(32)+t(49)+t(28)+t(54)+t(32)+t(47)+t(30)));
		//       bf_is_zero_cb_sym(203) => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b'.
		q(nt(203), (t(30)+t(36)+t(32)+t(41)+t(43)+t(32)+t(55)+t(28)+t(45)+t(31)+t(32)+t(47)+t(30)));
		//       bf_is_one_cb_sym(204) => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b'.
		q(nt(204), (t(30)+t(36)+t(32)+t(41)+t(43)+t(32)+t(31)+t(49)+t(28)+t(32)+t(47)+t(30)));
		//       __E_tau_collapse_positives_cb_35(213) => tau_collapse_positives_cb_sym(214) __(10) tau_cb_arg(215) __(10) tau_cb_arg(215) __(10) tau_cb_arg(215).
		q(nt(213), (nt(214)+nt(10)+nt(215)+nt(10)+nt(215)+nt(10)+nt(215)));
		//       tau_collapse_positives_cb(101) => __E_tau_collapse_positives_cb_35(213).
		q(nt(101), (nt(213)));
		//       __E_tau_collapse_positives_cb_36(216) => tau_collapse_positives_cb_sym(214) __(10) tau_cb_arg(215) __(10) tau_cb_arg(215).
		q(nt(216), (nt(214)+nt(10)+nt(215)+nt(10)+nt(215)));
		//       tau_collapse_positives_cb(101) => __E_tau_collapse_positives_cb_36(216).
		q(nt(101), (nt(216)));
		//       tau_positives_upwards_cb(102) => tau_positives_upwards_cb_sym(217) __(10) tau_cb_arg(215) __(10) tau_cb_arg(215).
		q(nt(102), (nt(217)+nt(10)+nt(215)+nt(10)+nt(215)));
		//       tau_cb_arg(215)      => capture(79).
		q(nt(215), (nt(79)));
		//       tau_cb_arg(215)      => tau(100).
		q(nt(215), (nt(100)));
		//       tau_collapse_positives_cb_sym(214) => 't' 'a' 'u' '_' 'c' 'o' 'l' 'l' 'a' 'p' 's' 'e' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'c' 'b'.
		q(nt(214), (t(52)+t(26)+t(44)+t(32)+t(47)+t(31)+t(27)+t(27)+t(26)+t(56)+t(43)+t(28)+t(32)+t(56)+t(31)+t(43)+t(41)+t(52)+t(41)+t(51)+t(28)+t(43)+t(32)+t(47)+t(30)));
		//       tau_positives_upwards_cb_sym(217) => 't' 'a' 'u' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'u' 'p' 'w' 'a' 'r' 'd' 's' '_' 'c' 'b'.
		q(nt(217), (t(52)+t(26)+t(44)+t(32)+t(56)+t(31)+t(43)+t(41)+t(52)+t(41)+t(51)+t(28)+t(43)+t(32)+t(44)+t(56)+t(48)+t(26)+t(45)+t(53)+t(43)+t(32)+t(47)+t(30)));
		//       __E_inputs_37(220)   => _(9) input(219).
		q(nt(220), (nt(9)+nt(219)));
		//       __E_inputs_38(221)   => null.
		q(nt(221), (nul));
		//       __E_inputs_38(221)   => __E_inputs_37(220) __E_inputs_38(221).
		q(nt(221), (nt(220)+nt(221)));
		//       inputs(218)          => _(9) less(27) _(9) input(219) __E_inputs_38(221) _(9) dot(17).
		q(nt(218), (nt(9)+nt(27)+nt(9)+nt(219)+nt(221)+nt(9)+nt(17)));
		//       input(219)           => in(87) _(9) colon(25) _(9) open_brace(22) _(9) source_binding(188) _(9) close_brace(23).
		q(nt(219), (nt(87)+nt(9)+nt(25)+nt(9)+nt(22)+nt(9)+nt(188)+nt(9)+nt(23)));
		//       builder(222)         => _(9) builder_head(223) _(9) builder_body(224) _(9) dot(17).
		q(nt(222), (nt(9)+nt(223)+nt(9)+nt(224)+nt(9)+nt(17)));
		//       __E_builder_head_39(225) => __(10) capture(79).
		q(nt(225), (nt(10)+nt(79)));
		//       __E_builder_head_40(226) => null.
		q(nt(226), (nul));
		//       __E_builder_head_40(226) => __E_builder_head_39(225) __E_builder_head_40(226).
		q(nt(226), (nt(225)+nt(226)));
		//       builder_head(223)    => open_parenthesis(18) _(9) capture(79) __E_builder_head_40(226) _(9) close_parenthesis(19).
		q(nt(223), (nt(18)+nt(9)+nt(79)+nt(226)+nt(9)+nt(19)));
		//       builder_body(224)    => bf_builder_body(227).
		q(nt(224), (nt(227)));
		//       builder_body(224)    => wff_builder_body(228).
		q(nt(224), (nt(228)));
		//       builder_body(224)    => tau_builder_body(229).
		q(nt(224), (nt(229)));
		//       bf_builder_body(227) => bf_builder_def(52) _(9) bf(110).
		q(nt(227), (nt(52)+nt(9)+nt(110)));
		//       wff_builder_body(228) => wff_builder_def(37) _(9) wff(115).
		q(nt(228), (nt(37)+nt(9)+nt(115)));
		//       tau_builder_body(229) => tau_builder_def(32) _(9) tau(100).
		q(nt(229), (nt(32)+nt(9)+nt(100)));
		//       library(230)         => rules(231).
		q(nt(230), (nt(231)));
		//       __E_rules_41(232)    => _(9) rule(233).
		q(nt(232), (nt(9)+nt(233)));
		//       __E_rules_42(234)    => null.
		q(nt(234), (nul));
		//       __E_rules_42(234)    => __E_rules_41(232) __E_rules_42(234).
		q(nt(234), (nt(232)+nt(234)));
		//       rules(231)           => __E_rules_42(234).
		q(nt(231), (nt(234)));
		//       rule(233)            => wff_rule(116).
		q(nt(233), (nt(116)));
		//       rule(233)            => bf_rule(156).
		q(nt(233), (nt(156)));
		//       rule(233)            => tau_rule(97).
		q(nt(233), (nt(97)));
		//       nso_rr(235)          => nso_rec_relations(236) nso_main(237).
		q(nt(235), (nt(236)+nt(237)));
		//       __E_nso_rec_relations_43(238) => _(9) nso_rec_relation(239).
		q(nt(238), (nt(9)+nt(239)));
		//       __E_nso_rec_relations_44(240) => null.
		q(nt(240), (nul));
		//       __E_nso_rec_relations_44(240) => __E_nso_rec_relations_43(238) __E_nso_rec_relations_44(240).
		q(nt(240), (nt(238)+nt(240)));
		//       nso_rec_relations(236) => __E_nso_rec_relations_44(240).
		q(nt(236), (nt(240)));
		//       nso_rec_relation(239) => bf_rec_relation(177).
		q(nt(239), (nt(177)));
		//       nso_rec_relation(239) => wff_rec_relation(126).
		q(nt(239), (nt(126)));
		//       nso_main(237)        => _(9) wff(115) _(9) dot(17).
		q(nt(237), (nt(9)+nt(115)+nt(9)+nt(17)));
		//       gssotc_rr(241)       => gssotc_rec_relations(242) gssotc_main(243).
		q(nt(241), (nt(242)+nt(243)));
		//       __E_gssotc_rec_relations_45(244) => _(9) gssotc_rec_relation(245).
		q(nt(244), (nt(9)+nt(245)));
		//       __E_gssotc_rec_relations_46(246) => null.
		q(nt(246), (nul));
		//       __E_gssotc_rec_relations_46(246) => __E_gssotc_rec_relations_45(244) __E_gssotc_rec_relations_46(246).
		q(nt(246), (nt(244)+nt(246)));
		//       gssotc_rec_relations(242) => __E_gssotc_rec_relations_46(246).
		q(nt(242), (nt(246)));
		//       gssotc_rec_relation(245) => tau_rec_relation(103).
		q(nt(245), (nt(103)));
		//       gssotc_rec_relation(245) => wff_rec_relation(126).
		q(nt(245), (nt(126)));
		//       gssotc_rec_relation(245) => bf_rec_relation(177).
		q(nt(245), (nt(177)));
		//       gssotc_main(243)     => _(9) tau(100) _(9) semicolon(26).
		q(nt(243), (nt(9)+nt(100)+nt(9)+nt(26)));
		//       __E_start_47(248)    => inputs(218).
		q(nt(248), (nt(218)));
		//       __E_start_47(248)    => builder(222).
		q(nt(248), (nt(222)));
		//       __E_start_47(248)    => library(230).
		q(nt(248), (nt(230)));
		//       __E_start_47(248)    => nso_rr(235).
		q(nt(248), (nt(235)));
		//       __E_start_47(248)    => gssotc_rr(241).
		q(nt(248), (nt(241)));
		//       start(247)           => __E_start_47(248) _(9).
		q(nt(247), (nt(248)+nt(9)));
		//       __E_cli_48(251)      => _(9) dot(17) _(9) cli_command(250).
		q(nt(251), (nt(9)+nt(17)+nt(9)+nt(250)));
		//       __E_cli_49(252)      => null.
		q(nt(252), (nul));
		//       __E_cli_49(252)      => __E_cli_48(251) __E_cli_49(252).
		q(nt(252), (nt(251)+nt(252)));
		//       cli(249)             => _(9) cli_command(250) __E_cli_49(252).
		q(nt(249), (nt(9)+nt(250)+nt(252)));
		//       cli_command(250)     => help_cmd(253).
		q(nt(250), (nt(253)));
		//       cli_command(250)     => version_cmd(254).
		q(nt(250), (nt(254)));
		//       cli_command(250)     => quit_cmd(255).
		q(nt(250), (nt(255)));
		//       cli_command(250)     => clear_cmd(256).
		q(nt(250), (nt(256)));
		//       cli_command(250)     => get_cmd(257).
		q(nt(250), (nt(257)));
		//       cli_command(250)     => set_cmd(258).
		q(nt(250), (nt(258)));
		//       cli_command(250)     => toggle_cmd(259).
		q(nt(250), (nt(259)));
		//       cli_command(250)     => list_outputs_cmd(260).
		q(nt(250), (nt(260)));
		//       cli_command(250)     => clear_outputs_cmd(261).
		q(nt(250), (nt(261)));
		//       cli_command(250)     => print_output_cmd(262).
		q(nt(250), (nt(262)));
		//       cli_command(250)     => file(263).
		q(nt(250), (nt(263)));
		//       cli_command(250)     => normalize_cmd(264).
		q(nt(250), (nt(264)));
		//       cli_command(250)     => bf_dnf_cmd(265).
		q(nt(250), (nt(265)));
		//       cli_command(250)     => bf_cnf_cmd(266).
		q(nt(250), (nt(266)));
		//       cli_command(250)     => bf_anf_cmd(267).
		q(nt(250), (nt(267)));
		//       cli_command(250)     => bf_nnf_cmd(268).
		q(nt(250), (nt(268)));
		//       cli_command(250)     => bf_pnf_cmd(269).
		q(nt(250), (nt(269)));
		//       cli_command(250)     => bf_mnf_cmd(270).
		q(nt(250), (nt(270)));
		//       cli_command(250)     => bf_substitute(271).
		q(nt(250), (nt(271)));
		//       cli_command(250)     => wff_substitute(272).
		q(nt(250), (nt(272)));
		//       cli_command(250)     => bf_instantiate(273).
		q(nt(250), (nt(273)));
		//       cli_command(250)     => wff_instantiate(274).
		q(nt(250), (nt(274)));
		//       cli_command(250)     => wff_onf_cmd(275).
		q(nt(250), (nt(275)));
		//       cli_command(250)     => wff_dnf_cmd(276).
		q(nt(250), (nt(276)));
		//       cli_command(250)     => wff_cnf_cmd(277).
		q(nt(250), (nt(277)));
		//       cli_command(250)     => wff_anf_cmd(278).
		q(nt(250), (nt(278)));
		//       cli_command(250)     => wff_nnf_cmd(279).
		q(nt(250), (nt(279)));
		//       cli_command(250)     => wff_pnf_cmd(280).
		q(nt(250), (nt(280)));
		//       cli_command(250)     => wff_mnf_cmd(281).
		q(nt(250), (nt(281)));
		//       cli_command(250)     => def_wff_cmd(282).
		q(nt(250), (nt(282)));
		//       cli_command(250)     => def_bf_cmd(283).
		q(nt(250), (nt(283)));
		//       cli_command(250)     => def_tau_cmd(284).
		q(nt(250), (nt(284)));
		//       cli_command(250)     => def_list_cmd(285).
		q(nt(250), (nt(285)));
		//       cli_command(250)     => def_del_cmd(286).
		q(nt(250), (nt(286)));
		//       cli_command(250)     => bf(110).
		q(nt(250), (nt(110)));
		//       cli_command(250)     => wff(115).
		q(nt(250), (nt(115)));
		//       cli_command(250)     => nso_rr(235).
		q(nt(250), (nt(235)));
		//       file(263)            => file_sym(287) quote(30) file_path(288) quote(30).
		q(nt(263), (nt(287)+nt(30)+nt(288)+nt(30)));
		//       normalize_cmd(264)   => normalize_cmd_sym(289) __(10) normalize_cmd_arg(290).
		q(nt(264), (nt(289)+nt(10)+nt(290)));
		//       normalize_cmd_arg(290) => wff(115).
		q(nt(290), (nt(115)));
		//       normalize_cmd_arg(290) => nso_rr(235).
		q(nt(290), (nt(235)));
		//       normalize_cmd_arg(290) => output(291).
		q(nt(290), (nt(291)));
		//       normalize_cmd_arg(290) => wff_selection(292).
		q(nt(290), (nt(292)));
		//       __E_bf_instantiate_cmd_50(295) => bf(110).
		q(nt(295), (nt(110)));
		//       __E_bf_instantiate_cmd_50(295) => bf_selection(296).
		q(nt(295), (nt(296)));
		//       __E_bf_instantiate_cmd_50(295) => output(291).
		q(nt(295), (nt(291)));
		//       __E_bf_instantiate_cmd_51(297) => bf_selection(296).
		q(nt(297), (nt(296)));
		//       __E_bf_instantiate_cmd_51(297) => output(291).
		q(nt(297), (nt(291)));
		//       bf_instantiate_cmd(293) => instantiate_cmd_sym(294) __(10) variable(82) __(10) __E_bf_instantiate_cmd_50(295) __(10) __E_bf_instantiate_cmd_51(297).
		q(nt(293), (nt(294)+nt(10)+nt(82)+nt(10)+nt(295)+nt(10)+nt(297)));
		//       __E_wff_instantiate_cmd_52(299) => wff(115).
		q(nt(299), (nt(115)));
		//       __E_wff_instantiate_cmd_52(299) => wff_selection(292).
		q(nt(299), (nt(292)));
		//       __E_wff_instantiate_cmd_52(299) => output(291).
		q(nt(299), (nt(291)));
		//       __E_wff_instantiate_cmd_53(300) => wff_selection(292).
		q(nt(300), (nt(292)));
		//       __E_wff_instantiate_cmd_53(300) => output(291).
		q(nt(300), (nt(291)));
		//       wff_instantiate_cmd(298) => instantiate_cmd_sym(294) __(10) variable(82) __(10) __E_wff_instantiate_cmd_52(299) __(10) __E_wff_instantiate_cmd_53(300).
		q(nt(298), (nt(294)+nt(10)+nt(82)+nt(10)+nt(299)+nt(10)+nt(300)));
		//       __E_bf_substitute_cmd_54(303) => bf(110).
		q(nt(303), (nt(110)));
		//       __E_bf_substitute_cmd_54(303) => output(291).
		q(nt(303), (nt(291)));
		//       __E_bf_substitute_cmd_54(303) => bf_selection(296).
		q(nt(303), (nt(296)));
		//       __E_bf_substitute_cmd_55(304) => output(291).
		q(nt(304), (nt(291)));
		//       __E_bf_substitute_cmd_55(304) => bf_selection(296).
		q(nt(304), (nt(296)));
		//       __E_bf_substitute_cmd_55(304) => bf_selection(296).
		q(nt(304), (nt(296)));
		//       __E_bf_substitute_cmd_56(305) => bf_selection(296).
		q(nt(305), (nt(296)));
		//       __E_bf_substitute_cmd_56(305) => output(291).
		q(nt(305), (nt(291)));
		//       bf_substitute_cmd(301) => substitute_cmd_sym(302) __(10) __E_bf_substitute_cmd_54(303) __(10) __E_bf_substitute_cmd_55(304) __E_bf_substitute_cmd_56(305).
		q(nt(301), (nt(302)+nt(10)+nt(303)+nt(10)+nt(304)+nt(305)));
		//       __E_wff_substitute_cmd_57(308) => wff(115).
		q(nt(308), (nt(115)));
		//       __E_wff_substitute_cmd_57(308) => output(291).
		q(nt(308), (nt(291)));
		//       __E_wff_substitute_cmd_57(308) => wff_selection(292).
		q(nt(308), (nt(292)));
		//       __E_wff_substitute_cmd_58(309) => output(291).
		q(nt(309), (nt(291)));
		//       __E_wff_substitute_cmd_58(309) => wff_selection(292).
		q(nt(309), (nt(292)));
		//       __E_wff_substitute_cmd_59(310) => wff_selection(292).
		q(nt(310), (nt(292)));
		//       __E_wff_substitute_cmd_59(310) => output(291).
		q(nt(310), (nt(291)));
		//       wff_substitute_cmd(306) => substitutecmd__sym(307) __(10) __E_wff_substitute_cmd_57(308) __(10) __E_wff_substitute_cmd_58(309) __E_wff_substitute_cmd_59(310).
		q(nt(306), (nt(307)+nt(10)+nt(308)+nt(10)+nt(309)+nt(310)));
		//       __E_bf_dnf_cmd_60(312) => bf(110).
		q(nt(312), (nt(110)));
		//       __E_bf_dnf_cmd_60(312) => output(291).
		q(nt(312), (nt(291)));
		//       __E_bf_dnf_cmd_60(312) => bf_selection(296).
		q(nt(312), (nt(296)));
		//       bf_dnf_cmd(265)      => dnf_cmd_sym(311) __(10) __E_bf_dnf_cmd_60(312).
		q(nt(265), (nt(311)+nt(10)+nt(312)));
		//       __E_bf_cnf_cmd_61(314) => bf(110).
		q(nt(314), (nt(110)));
		//       __E_bf_cnf_cmd_61(314) => output(291).
		q(nt(314), (nt(291)));
		//       __E_bf_cnf_cmd_61(314) => bf_selection(296).
		q(nt(314), (nt(296)));
		//       bf_cnf_cmd(266)      => cnf_cmd_sym(313) __(10) __E_bf_cnf_cmd_61(314).
		q(nt(266), (nt(313)+nt(10)+nt(314)));
		//       __E_bf_anf_cmd_62(316) => bf(110).
		q(nt(316), (nt(110)));
		//       __E_bf_anf_cmd_62(316) => output(291).
		q(nt(316), (nt(291)));
		//       __E_bf_anf_cmd_62(316) => bf_selection(296).
		q(nt(316), (nt(296)));
		//       bf_anf_cmd(267)      => anf_cmd_sym(315) __(10) __E_bf_anf_cmd_62(316).
		q(nt(267), (nt(315)+nt(10)+nt(316)));
		//       __E_bf_nnf_cmd_63(318) => bf(110).
		q(nt(318), (nt(110)));
		//       __E_bf_nnf_cmd_63(318) => output(291).
		q(nt(318), (nt(291)));
		//       __E_bf_nnf_cmd_63(318) => bf_selection(296).
		q(nt(318), (nt(296)));
		//       bf_nnf_cmd(268)      => nnf_cmd_sym(317) __(10) __E_bf_nnf_cmd_63(318).
		q(nt(268), (nt(317)+nt(10)+nt(318)));
		//       __E_bf_pnf_cmd_64(320) => bf(110).
		q(nt(320), (nt(110)));
		//       __E_bf_pnf_cmd_64(320) => output(291).
		q(nt(320), (nt(291)));
		//       __E_bf_pnf_cmd_64(320) => bf_selection(296).
		q(nt(320), (nt(296)));
		//       bf_pnf_cmd(269)      => pnf_cmd_sym(319) __(10) __E_bf_pnf_cmd_64(320).
		q(nt(269), (nt(319)+nt(10)+nt(320)));
		//       __E_bf_mnf_cmd_65(322) => bf(110).
		q(nt(322), (nt(110)));
		//       __E_bf_mnf_cmd_65(322) => output(291).
		q(nt(322), (nt(291)));
		//       __E_bf_mnf_cmd_65(322) => bf_selection(296).
		q(nt(322), (nt(296)));
		//       bf_mnf_cmd(270)      => mnf_cmd_sym(321) __(10) __E_bf_mnf_cmd_65(322).
		q(nt(270), (nt(321)+nt(10)+nt(322)));
		//       __E_wff_onf_cmd_66(324) => wff(115).
		q(nt(324), (nt(115)));
		//       __E_wff_onf_cmd_66(324) => output(291).
		q(nt(324), (nt(291)));
		//       __E_wff_onf_cmd_66(324) => wff_selection(292).
		q(nt(324), (nt(292)));
		//       wff_onf_cmd(275)     => onf_cmd_sym(323) __(10) variable(82) __(10) __E_wff_onf_cmd_66(324).
		q(nt(275), (nt(323)+nt(10)+nt(82)+nt(10)+nt(324)));
		//       __E_wff_dnf_cmd_67(325) => wff(115).
		q(nt(325), (nt(115)));
		//       __E_wff_dnf_cmd_67(325) => output(291).
		q(nt(325), (nt(291)));
		//       __E_wff_dnf_cmd_67(325) => wff_selection(292).
		q(nt(325), (nt(292)));
		//       wff_dnf_cmd(276)     => dnf_cmd_sym(311) __(10) __E_wff_dnf_cmd_67(325).
		q(nt(276), (nt(311)+nt(10)+nt(325)));
		//       __E_wff_cnf_cmd_68(326) => wff(115).
		q(nt(326), (nt(115)));
		//       __E_wff_cnf_cmd_68(326) => output(291).
		q(nt(326), (nt(291)));
		//       __E_wff_cnf_cmd_68(326) => wff_selection(292).
		q(nt(326), (nt(292)));
		//       wff_cnf_cmd(277)     => cnf_cmd_sym(313) __(10) __E_wff_cnf_cmd_68(326).
		q(nt(277), (nt(313)+nt(10)+nt(326)));
		//       __E_wff_anf_cmd_69(327) => wff(115).
		q(nt(327), (nt(115)));
		//       __E_wff_anf_cmd_69(327) => output(291).
		q(nt(327), (nt(291)));
		//       __E_wff_anf_cmd_69(327) => wff_selection(292).
		q(nt(327), (nt(292)));
		//       wff_anf_cmd(278)     => anf_cmd_sym(315) __(10) __E_wff_anf_cmd_69(327).
		q(nt(278), (nt(315)+nt(10)+nt(327)));
		//       __E_wff_nnf_cmd_70(328) => wff(115).
		q(nt(328), (nt(115)));
		//       __E_wff_nnf_cmd_70(328) => output(291).
		q(nt(328), (nt(291)));
		//       __E_wff_nnf_cmd_70(328) => wff_selection(292).
		q(nt(328), (nt(292)));
		//       wff_nnf_cmd(279)     => nnf_cmd_sym(317) __(10) __E_wff_nnf_cmd_70(328).
		q(nt(279), (nt(317)+nt(10)+nt(328)));
		//       __E_wff_pnf_cmd_71(329) => wff(115).
		q(nt(329), (nt(115)));
		//       __E_wff_pnf_cmd_71(329) => output(291).
		q(nt(329), (nt(291)));
		//       __E_wff_pnf_cmd_71(329) => wff_selection(292).
		q(nt(329), (nt(292)));
		//       wff_pnf_cmd(280)     => pnf_cmd_sym(319) __(10) __E_wff_pnf_cmd_71(329).
		q(nt(280), (nt(319)+nt(10)+nt(329)));
		//       __E_wff_mnf_cmd_72(330) => wff(115).
		q(nt(330), (nt(115)));
		//       __E_wff_mnf_cmd_72(330) => output(291).
		q(nt(330), (nt(291)));
		//       __E_wff_mnf_cmd_72(330) => wff_selection(292).
		q(nt(330), (nt(292)));
		//       wff_mnf_cmd(281)     => mnf_cmd_sym(321) __(10) __E_wff_mnf_cmd_72(330).
		q(nt(281), (nt(321)+nt(10)+nt(330)));
		//       def_wff_cmd(282)     => def_cmd_sym(331) __(10) wff_rec_relation(126).
		q(nt(282), (nt(331)+nt(10)+nt(126)));
		//       def_bf_cmd(283)      => def_cmd_sym(331) __(10) bf_rec_relation(177).
		q(nt(283), (nt(331)+nt(10)+nt(177)));
		//       def_tau_cmd(284)     => def_cmd_sym(331) __(10) tau_rec_relation(103).
		q(nt(284), (nt(331)+nt(10)+nt(103)));
		//       def_list_cmd(285)    => def_cmd_sym(331) __(10) 'l' 'i' 's' 't'.
		q(nt(285), (nt(331)+nt(10)+t(27)+t(41)+t(43)+t(52)));
		//       def_del_cmd(286)     => def_cmd_sym(331) __(10) 'd' 'e' 'l' __(10) digits(71).
		q(nt(286), (nt(331)+nt(10)+t(53)+t(28)+t(27)+nt(10)+nt(71)));
		//       def_clear_cmd(332)   => def_cmd_sym(331) __(10) 'c' 'l' 'e' 'a' 'r'.
		q(nt(332), (nt(331)+nt(10)+t(47)+t(27)+t(28)+t(26)+t(45)));
		//       __E_help_cmd_73(334) => __(10) cli_cmd_sym(335).
		q(nt(334), (nt(10)+nt(335)));
		//       __E_help_cmd_73(334) => null.
		q(nt(334), (nul));
		//       help_cmd(253)        => help_cmd_sym(333) __E_help_cmd_73(334).
		q(nt(253), (nt(333)+nt(334)));
		//       version_cmd(254)     => version_cmd_sym(336).
		q(nt(254), (nt(336)));
		//       quit_cmd(255)        => quit_cmd_sym(337).
		q(nt(255), (nt(337)));
		//       clear_cmd(256)       => clear_cmd_sym(338).
		q(nt(256), (nt(338)));
		//       wff_selection(292)   => selection_sym(339) __(10) digits(71).
		q(nt(292), (nt(339)+nt(10)+nt(71)));
		//       bf_selection(296)    => selection_sym(339) __(10) digits(71).
		q(nt(296), (nt(339)+nt(10)+nt(71)));
		//       bf_var_selection(340) => selection_sym(339) __(10) digits(71).
		q(nt(340), (nt(339)+nt(10)+nt(71)));
		//       wff_var_selection(341) => selection_sym(339) __(10) digits(71).
		q(nt(341), (nt(339)+nt(10)+nt(71)));
		//       file_path(288)       => chars(68).
		q(nt(288), (nt(68)));
		//       output(291)          => absolute_output(342).
		q(nt(291), (nt(342)));
		//       output(291)          => relative_output(343).
		q(nt(291), (nt(343)));
		//       __E_absolute_output_74(344) => absolute_output_sym(345).
		q(nt(344), (nt(345)));
		//       __E_absolute_output_74(344) => output_sym(346) __(10).
		q(nt(344), (nt(346)+nt(10)));
		//       absolute_output(342) => __E_absolute_output_74(344) output_id(347).
		q(nt(342), (nt(344)+nt(347)));
		//       relative_output(343) => relative_output_sym(348) output_id(347).
		q(nt(343), (nt(348)+nt(347)));
		//       output_id(347)       => digits(71).
		q(nt(347), (nt(71)));
		//       list_outputs_cmd(260) => output_sym(346).
		q(nt(260), (nt(346)));
		//       list_outputs_cmd(260) => absolute_output_sym(345).
		q(nt(260), (nt(345)));
		//       list_outputs_cmd(260) => relative_output_sym(348).
		q(nt(260), (nt(348)));
		//       __E_clear_outputs_cmd_75(349) => output_sym(346) __(10).
		q(nt(349), (nt(346)+nt(10)));
		//       __E_clear_outputs_cmd_75(349) => absolute_output_sym(345).
		q(nt(349), (nt(345)));
		//       __E_clear_outputs_cmd_75(349) => relative_output_sym(348).
		q(nt(349), (nt(348)));
		//       clear_outputs_cmd(261) => __E_clear_outputs_cmd_75(349) clear_sym(350).
		q(nt(261), (nt(349)+nt(350)));
		//       print_output_cmd(262) => output(291).
		q(nt(262), (nt(291)));
		//       cli_cmd_sym(335)     => help_cmd_sym(333).
		q(nt(335), (nt(333)));
		//       cli_cmd_sym(335)     => version_cmd_sym(336).
		q(nt(335), (nt(336)));
		//       cli_cmd_sym(335)     => quit_cmd_sym(337).
		q(nt(335), (nt(337)));
		//       cli_cmd_sym(335)     => clear_cmd_sym(338).
		q(nt(335), (nt(338)));
		//       cli_cmd_sym(335)     => output_sym(346).
		q(nt(335), (nt(346)));
		//       cli_cmd_sym(335)     => absolute_output_sym(345).
		q(nt(335), (nt(345)));
		//       cli_cmd_sym(335)     => relative_output_sym(348).
		q(nt(335), (nt(348)));
		//       cli_cmd_sym(335)     => selection_sym(339).
		q(nt(335), (nt(339)));
		//       cli_cmd_sym(335)     => instantiate_cmd_sym(294).
		q(nt(335), (nt(294)));
		//       cli_cmd_sym(335)     => substitute_cmd_sym(302).
		q(nt(335), (nt(302)));
		//       cli_cmd_sym(335)     => normalize_cmd_sym(289).
		q(nt(335), (nt(289)));
		//       cli_cmd_sym(335)     => file_cmd_sym(351).
		q(nt(335), (nt(351)));
		//       cli_cmd_sym(335)     => get_cmd_sym(352).
		q(nt(335), (nt(352)));
		//       cli_cmd_sym(335)     => set_cmd_sym(353).
		q(nt(335), (nt(353)));
		//       cli_cmd_sym(335)     => toggle_cmd_sym(354).
		q(nt(335), (nt(354)));
		//       cli_cmd_sym(335)     => cnf_cmd_sym(313).
		q(nt(335), (nt(313)));
		//       cli_cmd_sym(335)     => dnf_cmd_sym(311).
		q(nt(335), (nt(311)));
		//       cli_cmd_sym(335)     => nnf_cmd_sym(317).
		q(nt(335), (nt(317)));
		//       cli_cmd_sym(335)     => mnf_cmd_sym(321).
		q(nt(335), (nt(321)));
		//       cli_cmd_sym(335)     => onf_cmd_sym(323).
		q(nt(335), (nt(323)));
		//       help_cmd_sym(333)    => 'h'.
		q(nt(333), (t(42)));
		//       help_cmd_sym(333)    => 'h' 'e' 'l' 'p'.
		q(nt(333), (t(42)+t(28)+t(27)+t(56)));
		//       version_cmd_sym(336) => 'v'.
		q(nt(336), (t(51)));
		//       version_cmd_sym(336) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
		q(nt(336), (t(51)+t(28)+t(45)+t(43)+t(41)+t(31)+t(49)));
		//       quit_cmd_sym(337)    => 'q'.
		q(nt(337), (t(54)));
		//       quit_cmd_sym(337)    => 'e'.
		q(nt(337), (t(28)));
		//       quit_cmd_sym(337)    => 'q' 'u' 'i' 't'.
		q(nt(337), (t(54)+t(44)+t(41)+t(52)));
		//       quit_cmd_sym(337)    => 'e' 'x' 'i' 't'.
		q(nt(337), (t(28)+t(29)+t(41)+t(52)));
		//       instantiate_sym(355) => 'i'.
		q(nt(355), (t(41)));
		//       instantiate_sym(355) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
		q(nt(355), (t(41)+t(49)+t(43)+t(52)+t(26)+t(49)+t(52)+t(41)+t(26)+t(52)+t(28)));
		//       substitute_sym(356)  => 's'.
		q(nt(356), (t(43)));
		//       substitute_sym(356)  => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
		q(nt(356), (t(43)+t(44)+t(30)+t(43)+t(52)+t(41)+t(52)+t(44)+t(52)+t(28)));
		//       normalize_cmd_sym(289) => 'n'.
		q(nt(289), (t(49)));
		//       normalize_cmd_sym(289) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
		q(nt(289), (t(49)+t(31)+t(45)+t(46)+t(26)+t(27)+t(41)+t(55)+t(28)));
		//       file_cmd_sym(351)    => 'r'.
		q(nt(351), (t(45)));
		//       file_cmd_sym(351)    => 'r' 'e' 'a' 'd'.
		q(nt(351), (t(45)+t(28)+t(26)+t(53)));
		//       clear_cmd_sym(338)   => 'c'.
		q(nt(338), (t(47)));
		//       clear_cmd_sym(338)   => 'c' 'l' 'e' 'a' 'r'.
		q(nt(338), (t(47)+t(27)+t(28)+t(26)+t(45)));
		//       onf_cmd_sym(323)     => 'o' 'n' 'f'.
		q(nt(323), (t(31)+t(49)+t(36)));
		//       dnf_cmd_sym(311)     => 'd' 'n' 'f'.
		q(nt(311), (t(53)+t(49)+t(36)));
		//       cnf_cmd_sym(313)     => 'c' 'n' 'f'.
		q(nt(313), (t(47)+t(49)+t(36)));
		//       anf_cmd_sym(315)     => 'a' 'n' 'f'.
		q(nt(315), (t(26)+t(49)+t(36)));
		//       nnf_cmd_sym(317)     => 'n' 'n' 'f'.
		q(nt(317), (t(49)+t(49)+t(36)));
		//       pnf_cmd_sym(319)     => 'p' 'n' 'f'.
		q(nt(319), (t(56)+t(49)+t(36)));
		//       mnf_cmd_sym(321)     => 'm' 'n' 'f'.
		q(nt(321), (t(46)+t(49)+t(36)));
		//       def_cmd_sym(331)     => 'd' 'e' 'f'.
		q(nt(331), (t(53)+t(28)+t(36)));
		//       selection_sym(339)   => 's'.
		q(nt(339), (t(43)));
		//       selection_sym(339)   => 's' 'e' 'l' 'e' 'c' 't' 'i' 'o' 'n'.
		q(nt(339), (t(43)+t(28)+t(27)+t(28)+t(47)+t(52)+t(41)+t(31)+t(49)));
		//       output_sym(346)      => 'o'.
		q(nt(346), (t(31)));
		//       output_sym(346)      => 'o' 'u' 't' 'p' 'u' 't'.
		q(nt(346), (t(31)+t(44)+t(52)+t(56)+t(44)+t(52)));
		//       absolute_output_sym(345) => '&'.
		q(nt(345), (t(20)));
		//       relative_output_sym(348) => '%'.
		q(nt(348), (t(57)));
		//       __E_get_cmd_76(357)  => __(10) option(358).
		q(nt(357), (nt(10)+nt(358)));
		//       __E_get_cmd_76(357)  => null.
		q(nt(357), (nul));
		//       get_cmd(257)         => get_cmd_sym(352) __E_get_cmd_76(357).
		q(nt(257), (nt(352)+nt(357)));
		//       __E_set_cmd_77(359)  => __(10).
		q(nt(359), (nt(10)));
		//       __E_set_cmd_77(359)  => _(9) '=' _(9).
		q(nt(359), (nt(9)+t(19)+nt(9)));
		//       set_cmd(258)         => set_cmd_sym(353) __(10) option(358) __E_set_cmd_77(359) option_value(360).
		q(nt(258), (nt(353)+nt(10)+nt(358)+nt(359)+nt(360)));
		//       toggle_cmd(259)      => toggle_cmd_sym(354) __(10) bool_option(361).
		q(nt(259), (nt(354)+nt(10)+nt(361)));
		//       get_cmd_sym(352)     => 'g' 'e' 't'.
		q(nt(352), (t(50)+t(28)+t(52)));
		//       set_cmd_sym(353)     => 's' 'e' 't'.
		q(nt(353), (t(43)+t(28)+t(52)));
		//       toggle_cmd_sym(354)  => 't' 'o' 'g' 'g' 'l' 'e'.
		q(nt(354), (t(52)+t(31)+t(50)+t(50)+t(27)+t(28)));
		//       option(358)          => bool_option(361).
		q(nt(358), (nt(361)));
		//       option(358)          => severity_opt(362).
		q(nt(358), (nt(362)));
		//       bool_option(361)     => status_opt(363).
		q(nt(361), (nt(363)));
		//       bool_option(361)     => colors_opt(364).
		q(nt(361), (nt(364)));
		//       bool_option(361)     => debug_repl_opt(365).
		q(nt(361), (nt(365)));
		//       status_opt(363)      => 's'.
		q(nt(363), (t(43)));
		//       status_opt(363)      => 's' 't' 'a' 't' 'u' 's'.
		q(nt(363), (t(43)+t(52)+t(26)+t(52)+t(44)+t(43)));
		//       colors_opt(364)      => 'c'.
		q(nt(364), (t(47)));
		//       colors_opt(364)      => 'c' 'o' 'l' 'o' 'r'.
		q(nt(364), (t(47)+t(31)+t(27)+t(31)+t(45)));
		//       colors_opt(364)      => 'c' 'o' 'l' 'o' 'r' 's'.
		q(nt(364), (t(47)+t(31)+t(27)+t(31)+t(45)+t(43)));
		//       severity_opt(362)    => 's' 'e' 'v'.
		q(nt(362), (t(43)+t(28)+t(51)));
		//       severity_opt(362)    => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
		q(nt(362), (t(43)+t(28)+t(51)+t(28)+t(45)+t(41)+t(52)+t(58)));
		//       debug_repl_opt(365)  => 'd'.
		q(nt(365), (t(53)));
		//       debug_repl_opt(365)  => 'd' 'r'.
		q(nt(365), (t(53)+t(45)));
		//       debug_repl_opt(365)  => 'd' 'b' 'g'.
		q(nt(365), (t(53)+t(30)+t(50)));
		//       debug_repl_opt(365)  => 'd' 'e' 'b' 'u' 'g'.
		q(nt(365), (t(53)+t(28)+t(30)+t(44)+t(50)));
		//       debug_repl_opt(365)  => 'd' 'e' 'b' 'u' 'g' '-' 'r' 'e' 'p' 'l'.
		q(nt(365), (t(53)+t(28)+t(30)+t(44)+t(50)+t(12)+t(45)+t(28)+t(56)+t(27)));
		//       option_value(360)    => option_value_true(366).
		q(nt(360), (nt(366)));
		//       option_value(360)    => option_value_false(367).
		q(nt(360), (nt(367)));
		//       option_value(360)    => severity(368).
		q(nt(360), (nt(368)));
		//       option_value_true(366) => 't'.
		q(nt(366), (t(52)));
		//       option_value_true(366) => 't' 'r' 'u' 'e'.
		q(nt(366), (t(52)+t(45)+t(44)+t(28)));
		//       option_value_true(366) => 'o' 'n'.
		q(nt(366), (t(31)+t(49)));
		//       option_value_true(366) => '1'.
		q(nt(366), (t(38)));
		//       option_value_true(366) => 'y'.
		q(nt(366), (t(58)));
		//       option_value_true(366) => 'y' 'e' 's'.
		q(nt(366), (t(58)+t(28)+t(43)));
		//       option_value_false(367) => 'f'.
		q(nt(367), (t(36)));
		//       option_value_false(367) => 'f' 'a' 'l' 's' 'e'.
		q(nt(367), (t(36)+t(26)+t(27)+t(43)+t(28)));
		//       option_value_false(367) => 'o' 'f' 'f'.
		q(nt(367), (t(31)+t(36)+t(36)));
		//       option_value_false(367) => '0'.
		q(nt(367), (t(39)));
		//       option_value_false(367) => 'n'.
		q(nt(367), (t(49)));
		//       option_value_false(367) => 'n' 'o'.
		q(nt(367), (t(49)+t(31)));
		//       severity(368)        => error_sym(369).
		q(nt(368), (nt(369)));
		//       severity(368)        => debug_sym(370).
		q(nt(368), (nt(370)));
		//       severity(368)        => trace_sym(371).
		q(nt(368), (nt(371)));
		//       severity(368)        => info_sym(372).
		q(nt(368), (nt(372)));
		//       error_sym(369)       => 'e'.
		q(nt(369), (t(28)));
		//       error_sym(369)       => 'e' 'r' 'r' 'o' 'r'.
		q(nt(369), (t(28)+t(45)+t(45)+t(31)+t(45)));
		//       info_sym(372)        => 'i'.
		q(nt(372), (t(41)));
		//       info_sym(372)        => 'i' 'n' 'f' 'o'.
		q(nt(372), (t(41)+t(49)+t(36)+t(31)));
		//       debug_sym(370)       => 'd'.
		q(nt(370), (t(53)));
		//       debug_sym(370)       => 'd' 'e' 'b' 'u' 'g'.
		q(nt(370), (t(53)+t(28)+t(30)+t(44)+t(50)));
		//       trace_sym(371)       => 't'.
		q(nt(371), (t(52)));
		//       trace_sym(371)       => 't' 'r' 'a' 'c' 'e'.
		q(nt(371), (t(52)+t(45)+t(26)+t(47)+t(28)));
		return q;
	}
};

#endif // __TAU_PARSER_H__
