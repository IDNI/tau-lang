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
		g(nts, load_prods(), nt(253), cc, load_grammar_opts()),
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
		wff_ref_args, __E_wff_ref_args_20, wff_ref_arg, __E_wff_ref_args_21, __E_wff_and_22, __E_wff_all_23, __E_wff_ex_24, __E_wff_ball_25, __E_wff_bex_26, bf_nleq_lower, 
		bf_nleq_sym, bf_nleq_upper, bf_rule, bf_matcher, bf_body, bf_is_zero_cb, bf_is_one_cb, bf_has_subformula_cb, bf_remove_funiversal_cb, bf_remove_fexistential_cb, 
		bf_and_cb, bf_or_cb, bf_xor_cb, bf_neg_cb, bf_splitter, bf_ref, bf_all, bf_ex, bf_or, bf_and, 
		bf_xor, bf_neg, bf_constant, bf_rec_relation, __E_bf_ref_27, bf_ref_args, __E_bf_ref_args_28, bf_ref_arg, __E_bf_ref_args_29, __E_bf_and_30, 
		__E_bf_all_31, __E_bf_ex_32, constant, binding, source_binding, named_binding, type, source, source0, __E_source_33, 
		__E_source_34, bf_and_cb_sym, bf_cb_arg, bf_or_cb_sym, bf_xor_cb_sym, bf_neg_cb_sym, bf_eq_cb_sym, wff_cb_arg, bf_neq_cb_sym, bf_is_zero_cb_sym, 
		bf_is_one_cb_sym, bf_remove_funiversal_cb_sym, bf_remove_fexistential_cb_sym, wff_remove_existential_cb_sym, wff_remove_bexistential_cb_sym, wff_remove_buniversal_cb_sym, wff_has_clashing_subformulas_cb_sym, bf_has_subformula_cb_sym, wff_has_subformula_cb_sym, __E_tau_collapse_positives_cb_35, 
		tau_collapse_positives_cb_sym, tau_cb_arg, __E_tau_collapse_positives_cb_36, tau_positives_upwards_cb_sym, inputs, input, __E_inputs_37, __E_inputs_38, builder, builder_head, 
		builder_body, __E_builder_head_39, __E_builder_head_40, bf_builder_body, wff_builder_body, tau_builder_body, library, rules, __E_rules_41, rule, 
		__E_rules_42, nso_rr, nso_rec_relations, nso_main, __E_nso_rec_relations_43, nso_rec_relation, __E_nso_rec_relations_44, gssotc_rr, gssotc_rec_relations, gssotc_main, 
		__E_gssotc_rec_relations_45, gssotc_rec_relation, __E_gssotc_rec_relations_46, start, __E_start_47, cli, cli_command, __E_cli_48, __E_cli_49, help_cmd, 
		version_cmd, quit_cmd, clear_cmd, get_cmd, set_cmd, toggle_cmd, list_outputs_cmd, clear_outputs_cmd, print_output_cmd, file, 
		normalize_cmd, execute_cmd, solve_cmd, bf_dnf_cmd, bf_cnf_cmd, bf_anf_cmd, bf_nnf_cmd, bf_pnf_cmd, bf_mnf_cmd, bf_substitute_cmd, 
		wff_substitute_cmd, bf_instantiate_cmd, wff_instantiate_cmd, wff_onf_cmd, wff_dnf_cmd, wff_cnf_cmd, wff_anf_cmd, wff_nnf_cmd, wff_pnf_cmd, wff_mnf_cmd, 
		def_rule_cmd, def_list_cmd, def_del_cmd, def_clear_cmd, normalize_cmd_sym, normalize_cmd_arg, output, wff_selection, solve_cmd_sym, solve_cmd_arg, 
		execute_cmd_sym, execute_cmd_arg, file_sym, file_path, instantiate_cmd_sym, __E_bf_instantiate_cmd_50, bf_selection, __E_bf_instantiate_cmd_51, __E_wff_instantiate_cmd_52, __E_wff_instantiate_cmd_53, 
		substitute_cmd_sym, __E_bf_substitute_cmd_54, __E_bf_substitute_cmd_55, __E_bf_substitute_cmd_56, substitutecmd__sym, __E_wff_substitute_cmd_57, __E_wff_substitute_cmd_58, __E_wff_substitute_cmd_59, dnf_cmd_sym, __E_bf_dnf_cmd_60, 
		cnf_cmd_sym, __E_bf_cnf_cmd_61, anf_cmd_sym, __E_bf_anf_cmd_62, nnf_cmd_sym, __E_bf_nnf_cmd_63, pnf_cmd_sym, __E_bf_pnf_cmd_64, mnf_cmd_sym, __E_bf_mnf_cmd_65, 
		onf_cmd_sym, __E_wff_onf_cmd_66, __E_wff_dnf_cmd_67, __E_wff_cnf_cmd_68, __E_wff_anf_cmd_69, __E_wff_nnf_cmd_70, __E_wff_pnf_cmd_71, __E_wff_mnf_cmd_72, def_cmd_sym, def_rule_cmd_arg, 
		__E_def_rule_cmd_arg_73, def_cmd_list, def_cmd_del, def_cmf_clear, help_cmd_sym, __E_help_cmd_74, cli_cmd_sym, version_cmd_sym, quit_cmd_sym, clear_cmd_sym, 
		selection_sym, bf_var_selection, wff_var_selection, absolute_output, relative_output, __E_absolute_output_75, absolute_output_sym, output_sym, output_id, relative_output_sym, 
		__E_clear_outputs_cmd_76, clear_sym, file_cmd_sym, get_cmd_sym, set_cmd_sym, toggle_cmd_sym, __E_get_cmd_77, option, __E_set_cmd_78, option_value, 
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
			"wff_ref_args", "__E_wff_ref_args_20", "wff_ref_arg", "__E_wff_ref_args_21", "__E_wff_and_22", "__E_wff_all_23", "__E_wff_ex_24", "__E_wff_ball_25", "__E_wff_bex_26", "bf_nleq_lower", 
			"bf_nleq_sym", "bf_nleq_upper", "bf_rule", "bf_matcher", "bf_body", "bf_is_zero_cb", "bf_is_one_cb", "bf_has_subformula_cb", "bf_remove_funiversal_cb", "bf_remove_fexistential_cb", 
			"bf_and_cb", "bf_or_cb", "bf_xor_cb", "bf_neg_cb", "bf_splitter", "bf_ref", "bf_all", "bf_ex", "bf_or", "bf_and", 
			"bf_xor", "bf_neg", "bf_constant", "bf_rec_relation", "__E_bf_ref_27", "bf_ref_args", "__E_bf_ref_args_28", "bf_ref_arg", "__E_bf_ref_args_29", "__E_bf_and_30", 
			"__E_bf_all_31", "__E_bf_ex_32", "constant", "binding", "source_binding", "named_binding", "type", "source", "source0", "__E_source_33", 
			"__E_source_34", "bf_and_cb_sym", "bf_cb_arg", "bf_or_cb_sym", "bf_xor_cb_sym", "bf_neg_cb_sym", "bf_eq_cb_sym", "wff_cb_arg", "bf_neq_cb_sym", "bf_is_zero_cb_sym", 
			"bf_is_one_cb_sym", "bf_remove_funiversal_cb_sym", "bf_remove_fexistential_cb_sym", "wff_remove_existential_cb_sym", "wff_remove_bexistential_cb_sym", "wff_remove_buniversal_cb_sym", "wff_has_clashing_subformulas_cb_sym", "bf_has_subformula_cb_sym", "wff_has_subformula_cb_sym", "__E_tau_collapse_positives_cb_35", 
			"tau_collapse_positives_cb_sym", "tau_cb_arg", "__E_tau_collapse_positives_cb_36", "tau_positives_upwards_cb_sym", "inputs", "input", "__E_inputs_37", "__E_inputs_38", "builder", "builder_head", 
			"builder_body", "__E_builder_head_39", "__E_builder_head_40", "bf_builder_body", "wff_builder_body", "tau_builder_body", "library", "rules", "__E_rules_41", "rule", 
			"__E_rules_42", "nso_rr", "nso_rec_relations", "nso_main", "__E_nso_rec_relations_43", "nso_rec_relation", "__E_nso_rec_relations_44", "gssotc_rr", "gssotc_rec_relations", "gssotc_main", 
			"__E_gssotc_rec_relations_45", "gssotc_rec_relation", "__E_gssotc_rec_relations_46", "start", "__E_start_47", "cli", "cli_command", "__E_cli_48", "__E_cli_49", "help_cmd", 
			"version_cmd", "quit_cmd", "clear_cmd", "get_cmd", "set_cmd", "toggle_cmd", "list_outputs_cmd", "clear_outputs_cmd", "print_output_cmd", "file", 
			"normalize_cmd", "execute_cmd", "solve_cmd", "bf_dnf_cmd", "bf_cnf_cmd", "bf_anf_cmd", "bf_nnf_cmd", "bf_pnf_cmd", "bf_mnf_cmd", "bf_substitute_cmd", 
			"wff_substitute_cmd", "bf_instantiate_cmd", "wff_instantiate_cmd", "wff_onf_cmd", "wff_dnf_cmd", "wff_cnf_cmd", "wff_anf_cmd", "wff_nnf_cmd", "wff_pnf_cmd", "wff_mnf_cmd", 
			"def_rule_cmd", "def_list_cmd", "def_del_cmd", "def_clear_cmd", "normalize_cmd_sym", "normalize_cmd_arg", "output", "wff_selection", "solve_cmd_sym", "solve_cmd_arg", 
			"execute_cmd_sym", "execute_cmd_arg", "file_sym", "file_path", "instantiate_cmd_sym", "__E_bf_instantiate_cmd_50", "bf_selection", "__E_bf_instantiate_cmd_51", "__E_wff_instantiate_cmd_52", "__E_wff_instantiate_cmd_53", 
			"substitute_cmd_sym", "__E_bf_substitute_cmd_54", "__E_bf_substitute_cmd_55", "__E_bf_substitute_cmd_56", "substitutecmd__sym", "__E_wff_substitute_cmd_57", "__E_wff_substitute_cmd_58", "__E_wff_substitute_cmd_59", "dnf_cmd_sym", "__E_bf_dnf_cmd_60", 
			"cnf_cmd_sym", "__E_bf_cnf_cmd_61", "anf_cmd_sym", "__E_bf_anf_cmd_62", "nnf_cmd_sym", "__E_bf_nnf_cmd_63", "pnf_cmd_sym", "__E_bf_pnf_cmd_64", "mnf_cmd_sym", "__E_bf_mnf_cmd_65", 
			"onf_cmd_sym", "__E_wff_onf_cmd_66", "__E_wff_dnf_cmd_67", "__E_wff_cnf_cmd_68", "__E_wff_anf_cmd_69", "__E_wff_nnf_cmd_70", "__E_wff_pnf_cmd_71", "__E_wff_mnf_cmd_72", "def_cmd_sym", "def_rule_cmd_arg", 
			"__E_def_rule_cmd_arg_73", "def_cmd_list", "def_cmd_del", "def_cmf_clear", "help_cmd_sym", "__E_help_cmd_74", "cli_cmd_sym", "version_cmd_sym", "quit_cmd_sym", "clear_cmd_sym", 
			"selection_sym", "bf_var_selection", "wff_var_selection", "absolute_output", "relative_output", "__E_absolute_output_75", "absolute_output_sym", "output_sym", "output_id", "relative_output_sym", 
			"__E_clear_outputs_cmd_76", "clear_sym", "file_cmd_sym", "get_cmd_sym", "set_cmd_sym", "toggle_cmd_sym", "__E_get_cmd_77", "option", "__E_set_cmd_78", "option_value", 
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
		//       bf_nleq_lower(159)   => bf(11) _(9) bf_nleq_sym(160) _(9) var(102).
		q(nt(159), (nt(11)+nt(9)+nt(160)+nt(9)+nt(102)));
		//       bf_nleq_upper(161)   => var(102) _(9) bf_nleq_sym(160) _(9) bf(11).
		q(nt(161), (nt(102)+nt(9)+nt(160)+nt(9)+nt(11)));
		//       bf_greater(146)      => bf(11) _(9) bf_greater_sym(68) _(9) bf(11).
		q(nt(146), (nt(11)+nt(9)+nt(68)+nt(9)+nt(11)));
		//       bf_interval(142)     => bf(11) _(9) bf_less_equal_sym(66) _(9) var(102) _(9) bf_less_equal_sym(66) _(9) bf(11).
		q(nt(142), (nt(11)+nt(9)+nt(66)+nt(9)+nt(102)+nt(9)+nt(66)+nt(9)+nt(11)));
		//       bf_rule(162)         => bf_matcher(163) _(9) bf_def(57) _(9) bf_body(164) _(9) dot(23).
		q(nt(162), (nt(163)+nt(9)+nt(57)+nt(9)+nt(164)+nt(9)+nt(23)));
		//       bf_matcher(163)      => bf(11).
		q(nt(163), (nt(11)));
		//       bf_body(164)         => bf(11).
		q(nt(164), (nt(11)));
		//       bf_body(164)         => bf_is_zero_cb(165).
		q(nt(164), (nt(165)));
		//       bf_body(164)         => bf_is_one_cb(166).
		q(nt(164), (nt(166)));
		//       bf_body(164)         => bf_has_subformula_cb(167).
		q(nt(164), (nt(167)));
		//       bf_body(164)         => bf_remove_funiversal_cb(168).
		q(nt(164), (nt(168)));
		//       bf_body(164)         => bf_remove_fexistential_cb(169).
		q(nt(164), (nt(169)));
		//       bf_body(164)         => bf_and_cb(170).
		q(nt(164), (nt(170)));
		//       bf_body(164)         => bf_or_cb(171).
		q(nt(164), (nt(171)));
		//       bf_body(164)         => bf_xor_cb(172).
		q(nt(164), (nt(172)));
		//       bf_body(164)         => bf_neg_cb(173).
		q(nt(164), (nt(173)));
		//       bf_body(164)         => bf_not_less_equal(145).
		q(nt(164), (nt(145)));
		//       bf(11)               => bf_splitter(174).
		q(nt(11), (nt(174)));
		//       bf(11)               => bf_ref(175).
		q(nt(11), (nt(175)));
		//       bf(11)               => bf_all(176).
		q(nt(11), (nt(176)));
		//       bf(11)               => bf_ex(177).
		q(nt(11), (nt(177)));
		//       bf(11)               => bf_or(178).
		q(nt(11), (nt(178)));
		//       bf(11)               => bf_and(179).
		q(nt(11), (nt(179)));
		//       bf(11)               => bf_xor(180).
		q(nt(11), (nt(180)));
		//       bf(11)               => bf_neg(181).
		q(nt(11), (nt(181)));
		//       bf(11)               => bf_constant(182).
		q(nt(11), (nt(182)));
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
		//       bf_rec_relation(183) => bf_ref(175) _(9) bf_def(57) _(9) bf(11) _(9) dot(23).
		q(nt(183), (nt(175)+nt(9)+nt(57)+nt(9)+nt(11)+nt(9)+nt(23)));
		//       __E_bf_ref_27(184)   => offsets(80).
		q(nt(184), (nt(80)));
		//       __E_bf_ref_27(184)   => null.
		q(nt(184), (nul));
		//       bf_ref(175)          => sym(79) __E_bf_ref_27(184) bf_ref_args(185).
		q(nt(175), (nt(79)+nt(184)+nt(185)));
		//       __E_bf_ref_args_28(186) => _(9) bf_ref_arg(187).
		q(nt(186), (nt(9)+nt(187)));
		//       __E_bf_ref_args_29(188) => null.
		q(nt(188), (nul));
		//       __E_bf_ref_args_29(188) => __E_bf_ref_args_28(186) __E_bf_ref_args_29(188).
		q(nt(188), (nt(186)+nt(188)));
		//       bf_ref_args(185)     => open_parenthesis(24) __E_bf_ref_args_29(188) _(9) close_parenthesis(25).
		q(nt(185), (nt(24)+nt(188)+nt(9)+nt(25)));
		//       bf_ref_arg(187)      => bf(11).
		q(nt(187), (nt(11)));
		//       bf_ref_arg(187)      => capture(85).
		q(nt(187), (nt(85)));
		//       bf_ref_arg(187)      => variable(88).
		q(nt(187), (nt(88)));
		//       __E_bf_and_30(189)   => bf_and_sym(59) _(9).
		q(nt(189), (nt(59)+nt(9)));
		//       __E_bf_and_30(189)   => null.
		q(nt(189), (nul));
		//       bf_and(179)          => bf(11) _(9) __E_bf_and_30(189) bf(11).
		q(nt(179), (nt(11)+nt(9)+nt(189)+nt(11)));
		//       bf_or(178)           => bf(11) _(9) bf_or_sym(60) _(9) bf(11).
		q(nt(178), (nt(11)+nt(9)+nt(60)+nt(9)+nt(11)));
		//       bf_xor(180)          => bf(11) _(9) bf_xor_sym(61) _(9) bf(11).
		q(nt(180), (nt(11)+nt(9)+nt(61)+nt(9)+nt(11)));
		//       bf_neg(181)          => bf(11) _(9) bf_neg_sym(62).
		q(nt(181), (nt(11)+nt(9)+nt(62)));
		//       __E_bf_all_31(190)   => variable(88).
		q(nt(190), (nt(88)));
		//       __E_bf_all_31(190)   => capture(85).
		q(nt(190), (nt(85)));
		//       bf_all(176)          => bf_all_sym(69) __(10) __E_bf_all_31(190) __(10) bf(11).
		q(nt(176), (nt(69)+nt(10)+nt(190)+nt(10)+nt(11)));
		//       __E_bf_ex_32(191)    => variable(88).
		q(nt(191), (nt(88)));
		//       __E_bf_ex_32(191)    => capture(85).
		q(nt(191), (nt(85)));
		//       bf_ex(177)           => bf_ex_sym(70) __(10) __E_bf_ex_32(191) __(10) bf(11).
		q(nt(177), (nt(70)+nt(10)+nt(191)+nt(10)+nt(11)));
		//       bf_splitter(174)     => bf_splitter_sym(71) _(9) open_parenthesis(24) _(9) bf(11) _(9) close_parenthesis(25).
		q(nt(174), (nt(71)+nt(9)+nt(24)+nt(9)+nt(11)+nt(9)+nt(25)));
		//       bf_constant(182)     => open_brace(28) _(9) constant(192) _(9) close_brace(29).
		q(nt(182), (nt(28)+nt(9)+nt(192)+nt(9)+nt(29)));
		//       constant(192)        => binding(193).
		q(nt(192), (nt(193)));
		//       constant(192)        => capture(85).
		q(nt(192), (nt(85)));
		//       binding(193)         => source_binding(194).
		q(nt(193), (nt(194)));
		//       binding(193)         => named_binding(195).
		q(nt(193), (nt(195)));
		//       named_binding(195)   => chars(74).
		q(nt(195), (nt(74)));
		//       source_binding(194)  => type(196) _(9) colon(31) _(9) source(197).
		q(nt(194), (nt(196)+nt(9)+nt(31)+nt(9)+nt(197)));
		//       type(196)            => chars(74).
		q(nt(196), (nt(74)));
		//       type(196)            => null.
		q(nt(196), (nul));
		//       source0(198)         => alnum(6).
		q(nt(198), (nt(6)));
		//       source0(198)         => space(2).
		q(nt(198), (nt(2)));
		//       source0(198)         => punct(7).
		q(nt(198), (nt(7)));
		//       __E_source_33(199)   => source0(198).
		q(nt(199), (nt(198)));
		//       __E_source_34(200)   => __E_source_33(199).
		q(nt(200), (nt(199)));
		//       __E_source_34(200)   => __E_source_33(199) __E_source_34(200).
		q(nt(200), (nt(199)+nt(200)));
		//       source(197)          => __E_source_34(200).
		q(nt(197), (nt(200)));
		//       bf_and_cb(170)       => bf_and_cb_sym(201) __(10) bf_cb_arg(202) __(10) bf_cb_arg(202).
		q(nt(170), (nt(201)+nt(10)+nt(202)+nt(10)+nt(202)));
		//       bf_or_cb(171)        => bf_or_cb_sym(203) __(10) bf_cb_arg(202) __(10) bf_cb_arg(202).
		q(nt(171), (nt(203)+nt(10)+nt(202)+nt(10)+nt(202)));
		//       bf_xor_cb(172)       => bf_xor_cb_sym(204) __(10) bf_cb_arg(202) __(10) bf_cb_arg(202).
		q(nt(172), (nt(204)+nt(10)+nt(202)+nt(10)+nt(202)));
		//       bf_neg_cb(173)       => bf_neg_cb_sym(205) __(10) bf_cb_arg(202).
		q(nt(173), (nt(205)+nt(10)+nt(202)));
		//       bf_eq_cb(122)        => bf_eq_cb_sym(206) __(10) bf_cb_arg(202) __(10) wff_cb_arg(207) __(10) wff_cb_arg(207).
		q(nt(122), (nt(206)+nt(10)+nt(202)+nt(10)+nt(207)+nt(10)+nt(207)));
		//       bf_neq_cb(123)       => bf_neq_cb_sym(208) __(10) bf_cb_arg(202) __(10) wff_cb_arg(207) __(10) wff_cb_arg(207).
		q(nt(123), (nt(208)+nt(10)+nt(202)+nt(10)+nt(207)+nt(10)+nt(207)));
		//       bf_is_zero_cb(165)   => bf_is_zero_cb_sym(209) __(10) bf_cb_arg(202) __(10) bf_cb_arg(202).
		q(nt(165), (nt(209)+nt(10)+nt(202)+nt(10)+nt(202)));
		//       bf_is_one_cb(166)    => bf_is_one_cb_sym(210) __(10) bf_cb_arg(202) __(10) bf_cb_arg(202).
		q(nt(166), (nt(210)+nt(10)+nt(202)+nt(10)+nt(202)));
		//       bf_remove_funiversal_cb(168) => bf_remove_funiversal_cb_sym(211) __(10) bf_cb_arg(202) __(10) bf_cb_arg(202) __(10) bf_cb_arg(202) __(10) bf_cb_arg(202).
		q(nt(168), (nt(211)+nt(10)+nt(202)+nt(10)+nt(202)+nt(10)+nt(202)+nt(10)+nt(202)));
		//       bf_remove_fexistential_cb(169) => bf_remove_fexistential_cb_sym(212) __(10) bf_cb_arg(202) __(10) bf_cb_arg(202) __(10) bf_cb_arg(202) __(10) bf_cb_arg(202).
		q(nt(169), (nt(212)+nt(10)+nt(202)+nt(10)+nt(202)+nt(10)+nt(202)+nt(10)+nt(202)));
		//       wff_remove_existential_cb(126) => wff_remove_existential_cb_sym(213) __(10) wff_cb_arg(207) __(10) wff_cb_arg(207).
		q(nt(126), (nt(213)+nt(10)+nt(207)+nt(10)+nt(207)));
		//       wff_remove_bexistential_cb(127) => wff_remove_bexistential_cb_sym(214) __(10) wff_cb_arg(207) __(10) wff_cb_arg(207) __(10) wff_cb_arg(207) __(10) wff_cb_arg(207).
		q(nt(127), (nt(214)+nt(10)+nt(207)+nt(10)+nt(207)+nt(10)+nt(207)+nt(10)+nt(207)));
		//       wff_remove_buniversal_cb(128) => wff_remove_buniversal_cb_sym(215) __(10) wff_cb_arg(207) __(10) wff_cb_arg(207) __(10) wff_cb_arg(207) __(10) wff_cb_arg(207).
		q(nt(128), (nt(215)+nt(10)+nt(207)+nt(10)+nt(207)+nt(10)+nt(207)+nt(10)+nt(207)));
		//       wff_has_clashing_subformulas_cb(124) => wff_has_clashing_subformulas_cb_sym(216) __(10) wff_cb_arg(207) __(10) wff_cb_arg(207).
		q(nt(124), (nt(216)+nt(10)+nt(207)+nt(10)+nt(207)));
		//       bf_has_subformula_cb(167) => bf_has_subformula_cb_sym(217) __(10) bf_cb_arg(202) __(10) bf_cb_arg(202) __(10) bf_cb_arg(202).
		q(nt(167), (nt(217)+nt(10)+nt(202)+nt(10)+nt(202)+nt(10)+nt(202)));
		//       wff_has_subformula_cb(125) => wff_has_subformula_cb_sym(218) __(10) wff_cb_arg(207) __(10) wff_cb_arg(207) __(10) wff_cb_arg(207).
		q(nt(125), (nt(218)+nt(10)+nt(207)+nt(10)+nt(207)+nt(10)+nt(207)));
		//       bf_has_subformula_cb_sym(217) => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b'.
		q(nt(217), (t(30)+t(36)+t(32)+t(42)+t(26)+t(43)+t(32)+t(43)+t(44)+t(30)+t(36)+t(31)+t(45)+t(46)+t(44)+t(27)+t(26)+t(32)+t(47)+t(30)));
		//       wff_has_clashing_subformulas_cb_sym(216) => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b'.
		q(nt(216), (t(48)+t(36)+t(36)+t(32)+t(42)+t(26)+t(43)+t(32)+t(47)+t(27)+t(26)+t(43)+t(42)+t(41)+t(49)+t(50)+t(32)+t(43)+t(44)+t(30)+t(36)+t(31)+t(45)+t(46)+t(44)+t(27)+t(26)+t(43)+t(32)+t(47)+t(30)));
		//       wff_has_subformula_cb_sym(218) => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b'.
		q(nt(218), (t(48)+t(36)+t(36)+t(32)+t(42)+t(26)+t(43)+t(32)+t(43)+t(44)+t(30)+t(36)+t(31)+t(45)+t(46)+t(44)+t(27)+t(26)+t(32)+t(47)+t(30)));
		//       wff_remove_existential_cb_sym(213) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(213), (t(48)+t(36)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(28)+t(29)+t(41)+t(43)+t(52)+t(28)+t(49)+t(52)+t(41)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       wff_remove_bexistential_cb_sym(214) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(214), (t(48)+t(36)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(30)+t(28)+t(29)+t(41)+t(43)+t(52)+t(28)+t(49)+t(52)+t(41)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       wff_remove_buniversal_cb_sym(215) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b'.
		q(nt(215), (t(48)+t(36)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(30)+t(44)+t(49)+t(41)+t(51)+t(28)+t(45)+t(43)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       bf_remove_fexistential_cb_sym(212) => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(212), (t(30)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(36)+t(28)+t(29)+t(41)+t(43)+t(52)+t(28)+t(49)+t(52)+t(41)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       bf_remove_funiversal_cb_sym(211) => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b'.
		q(nt(211), (t(30)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(36)+t(44)+t(49)+t(41)+t(51)+t(28)+t(45)+t(43)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       bf_cb_arg(202)       => bf(11).
		q(nt(202), (nt(11)));
		//       wff_cb_arg(207)      => wff(13).
		q(nt(207), (nt(13)));
		//       bf_and_cb_sym(201)   => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b'.
		q(nt(201), (t(30)+t(36)+t(32)+t(26)+t(49)+t(53)+t(32)+t(47)+t(30)));
		//       bf_or_cb_sym(203)    => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b'.
		q(nt(203), (t(30)+t(36)+t(32)+t(31)+t(45)+t(32)+t(47)+t(30)));
		//       bf_xor_cb_sym(204)   => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b'.
		q(nt(204), (t(30)+t(36)+t(32)+t(29)+t(31)+t(45)+t(32)+t(47)+t(30)));
		//       bf_neg_cb_sym(205)   => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b'.
		q(nt(205), (t(30)+t(36)+t(32)+t(49)+t(28)+t(50)+t(32)+t(47)+t(30)));
		//       bf_eq_cb_sym(206)    => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b'.
		q(nt(206), (t(30)+t(36)+t(32)+t(28)+t(54)+t(32)+t(47)+t(30)));
		//       bf_neq_cb_sym(208)   => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b'.
		q(nt(208), (t(30)+t(36)+t(32)+t(49)+t(28)+t(54)+t(32)+t(47)+t(30)));
		//       bf_is_zero_cb_sym(209) => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b'.
		q(nt(209), (t(30)+t(36)+t(32)+t(41)+t(43)+t(32)+t(55)+t(28)+t(45)+t(31)+t(32)+t(47)+t(30)));
		//       bf_is_one_cb_sym(210) => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b'.
		q(nt(210), (t(30)+t(36)+t(32)+t(41)+t(43)+t(32)+t(31)+t(49)+t(28)+t(32)+t(47)+t(30)));
		//       __E_tau_collapse_positives_cb_35(219) => tau_collapse_positives_cb_sym(220) __(10) tau_cb_arg(221) __(10) tau_cb_arg(221) __(10) tau_cb_arg(221).
		q(nt(219), (nt(220)+nt(10)+nt(221)+nt(10)+nt(221)+nt(10)+nt(221)));
		//       tau_collapse_positives_cb(106) => __E_tau_collapse_positives_cb_35(219).
		q(nt(106), (nt(219)));
		//       __E_tau_collapse_positives_cb_36(222) => tau_collapse_positives_cb_sym(220) __(10) tau_cb_arg(221) __(10) tau_cb_arg(221).
		q(nt(222), (nt(220)+nt(10)+nt(221)+nt(10)+nt(221)));
		//       tau_collapse_positives_cb(106) => __E_tau_collapse_positives_cb_36(222).
		q(nt(106), (nt(222)));
		//       tau_positives_upwards_cb(107) => tau_positives_upwards_cb_sym(223) __(10) tau_cb_arg(221) __(10) tau_cb_arg(221).
		q(nt(107), (nt(223)+nt(10)+nt(221)+nt(10)+nt(221)));
		//       tau_cb_arg(221)      => capture(85).
		q(nt(221), (nt(85)));
		//       tau_cb_arg(221)      => tau(15).
		q(nt(221), (nt(15)));
		//       tau_collapse_positives_cb_sym(220) => 't' 'a' 'u' '_' 'c' 'o' 'l' 'l' 'a' 'p' 's' 'e' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'c' 'b'.
		q(nt(220), (t(52)+t(26)+t(44)+t(32)+t(47)+t(31)+t(27)+t(27)+t(26)+t(56)+t(43)+t(28)+t(32)+t(56)+t(31)+t(43)+t(41)+t(52)+t(41)+t(51)+t(28)+t(43)+t(32)+t(47)+t(30)));
		//       tau_positives_upwards_cb_sym(223) => 't' 'a' 'u' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'u' 'p' 'w' 'a' 'r' 'd' 's' '_' 'c' 'b'.
		q(nt(223), (t(52)+t(26)+t(44)+t(32)+t(56)+t(31)+t(43)+t(41)+t(52)+t(41)+t(51)+t(28)+t(43)+t(32)+t(44)+t(56)+t(48)+t(26)+t(45)+t(53)+t(43)+t(32)+t(47)+t(30)));
		//       __E_inputs_37(226)   => _(9) input(225).
		q(nt(226), (nt(9)+nt(225)));
		//       __E_inputs_38(227)   => null.
		q(nt(227), (nul));
		//       __E_inputs_38(227)   => __E_inputs_37(226) __E_inputs_38(227).
		q(nt(227), (nt(226)+nt(227)));
		//       inputs(224)          => _(9) less(33) _(9) input(225) __E_inputs_38(227) _(9) dot(23).
		q(nt(224), (nt(9)+nt(33)+nt(9)+nt(225)+nt(227)+nt(9)+nt(23)));
		//       input(225)           => in(93) _(9) colon(31) _(9) open_brace(28) _(9) source_binding(194) _(9) close_brace(29).
		q(nt(225), (nt(93)+nt(9)+nt(31)+nt(9)+nt(28)+nt(9)+nt(194)+nt(9)+nt(29)));
		//       builder(228)         => _(9) builder_head(229) _(9) builder_body(230) _(9) dot(23).
		q(nt(228), (nt(9)+nt(229)+nt(9)+nt(230)+nt(9)+nt(23)));
		//       __E_builder_head_39(231) => __(10) capture(85).
		q(nt(231), (nt(10)+nt(85)));
		//       __E_builder_head_40(232) => null.
		q(nt(232), (nul));
		//       __E_builder_head_40(232) => __E_builder_head_39(231) __E_builder_head_40(232).
		q(nt(232), (nt(231)+nt(232)));
		//       builder_head(229)    => open_parenthesis(24) _(9) capture(85) __E_builder_head_40(232) _(9) close_parenthesis(25).
		q(nt(229), (nt(24)+nt(9)+nt(85)+nt(232)+nt(9)+nt(25)));
		//       builder_body(230)    => bf_builder_body(233).
		q(nt(230), (nt(233)));
		//       builder_body(230)    => wff_builder_body(234).
		q(nt(230), (nt(234)));
		//       builder_body(230)    => tau_builder_body(235).
		q(nt(230), (nt(235)));
		//       bf_builder_body(233) => bf_builder_def(58) _(9) bf(11).
		q(nt(233), (nt(58)+nt(9)+nt(11)));
		//       wff_builder_body(234) => wff_builder_def(43) _(9) wff(13).
		q(nt(234), (nt(43)+nt(9)+nt(13)));
		//       tau_builder_body(235) => tau_builder_def(38) _(9) tau(15).
		q(nt(235), (nt(38)+nt(9)+nt(15)));
		//       library(236)         => rules(237).
		q(nt(236), (nt(237)));
		//       __E_rules_41(238)    => _(9) rule(239).
		q(nt(238), (nt(9)+nt(239)));
		//       __E_rules_42(240)    => null.
		q(nt(240), (nul));
		//       __E_rules_42(240)    => __E_rules_41(238) __E_rules_42(240).
		q(nt(240), (nt(238)+nt(240)));
		//       rules(237)           => __E_rules_42(240).
		q(nt(237), (nt(240)));
		//       rule(239)            => wff_rule(119).
		q(nt(239), (nt(119)));
		//       rule(239)            => bf_rule(162).
		q(nt(239), (nt(162)));
		//       rule(239)            => tau_rule(103).
		q(nt(239), (nt(103)));
		//       nso_rr(241)          => nso_rec_relations(242) nso_main(243).
		q(nt(241), (nt(242)+nt(243)));
		//       __E_nso_rec_relations_43(244) => _(9) nso_rec_relation(245).
		q(nt(244), (nt(9)+nt(245)));
		//       __E_nso_rec_relations_44(246) => null.
		q(nt(246), (nul));
		//       __E_nso_rec_relations_44(246) => __E_nso_rec_relations_43(244) __E_nso_rec_relations_44(246).
		q(nt(246), (nt(244)+nt(246)));
		//       nso_rec_relations(242) => __E_nso_rec_relations_44(246).
		q(nt(242), (nt(246)));
		//       nso_rec_relation(245) => bf_rec_relation(183).
		q(nt(245), (nt(183)));
		//       nso_rec_relation(245) => wff_rec_relation(129).
		q(nt(245), (nt(129)));
		//       nso_main(243)        => _(9) wff(13) _(9) dot(23).
		q(nt(243), (nt(9)+nt(13)+nt(9)+nt(23)));
		//       gssotc_rr(247)       => gssotc_rec_relations(248) gssotc_main(249).
		q(nt(247), (nt(248)+nt(249)));
		//       __E_gssotc_rec_relations_45(250) => _(9) gssotc_rec_relation(251).
		q(nt(250), (nt(9)+nt(251)));
		//       __E_gssotc_rec_relations_46(252) => null.
		q(nt(252), (nul));
		//       __E_gssotc_rec_relations_46(252) => __E_gssotc_rec_relations_45(250) __E_gssotc_rec_relations_46(252).
		q(nt(252), (nt(250)+nt(252)));
		//       gssotc_rec_relations(248) => __E_gssotc_rec_relations_46(252).
		q(nt(248), (nt(252)));
		//       gssotc_rec_relation(251) => tau_rec_relation(108).
		q(nt(251), (nt(108)));
		//       gssotc_rec_relation(251) => wff_rec_relation(129).
		q(nt(251), (nt(129)));
		//       gssotc_rec_relation(251) => bf_rec_relation(183).
		q(nt(251), (nt(183)));
		//       gssotc_main(249)     => _(9) tau(15) _(9) semicolon(32).
		q(nt(249), (nt(9)+nt(15)+nt(9)+nt(32)));
		//       __E_start_47(254)    => inputs(224).
		q(nt(254), (nt(224)));
		//       __E_start_47(254)    => builder(228).
		q(nt(254), (nt(228)));
		//       __E_start_47(254)    => library(236).
		q(nt(254), (nt(236)));
		//       __E_start_47(254)    => nso_rr(241).
		q(nt(254), (nt(241)));
		//       __E_start_47(254)    => gssotc_rr(247).
		q(nt(254), (nt(247)));
		//       start(253)           => __E_start_47(254) _(9).
		q(nt(253), (nt(254)+nt(9)));
		//       __E_cli_48(257)      => _(9) dot(23) _(9) cli_command(256).
		q(nt(257), (nt(9)+nt(23)+nt(9)+nt(256)));
		//       __E_cli_49(258)      => null.
		q(nt(258), (nul));
		//       __E_cli_49(258)      => __E_cli_48(257) __E_cli_49(258).
		q(nt(258), (nt(257)+nt(258)));
		//       cli(255)             => _(9) cli_command(256) __E_cli_49(258).
		q(nt(255), (nt(9)+nt(256)+nt(258)));
		//       cli_command(256)     => help_cmd(259).
		q(nt(256), (nt(259)));
		//       cli_command(256)     => version_cmd(260).
		q(nt(256), (nt(260)));
		//       cli_command(256)     => quit_cmd(261).
		q(nt(256), (nt(261)));
		//       cli_command(256)     => clear_cmd(262).
		q(nt(256), (nt(262)));
		//       cli_command(256)     => get_cmd(263).
		q(nt(256), (nt(263)));
		//       cli_command(256)     => set_cmd(264).
		q(nt(256), (nt(264)));
		//       cli_command(256)     => toggle_cmd(265).
		q(nt(256), (nt(265)));
		//       cli_command(256)     => list_outputs_cmd(266).
		q(nt(256), (nt(266)));
		//       cli_command(256)     => clear_outputs_cmd(267).
		q(nt(256), (nt(267)));
		//       cli_command(256)     => print_output_cmd(268).
		q(nt(256), (nt(268)));
		//       cli_command(256)     => file(269).
		q(nt(256), (nt(269)));
		//       cli_command(256)     => normalize_cmd(270).
		q(nt(256), (nt(270)));
		//       cli_command(256)     => execute_cmd(271).
		q(nt(256), (nt(271)));
		//       cli_command(256)     => solve_cmd(272).
		q(nt(256), (nt(272)));
		//       cli_command(256)     => bf_dnf_cmd(273).
		q(nt(256), (nt(273)));
		//       cli_command(256)     => bf_cnf_cmd(274).
		q(nt(256), (nt(274)));
		//       cli_command(256)     => bf_anf_cmd(275).
		q(nt(256), (nt(275)));
		//       cli_command(256)     => bf_nnf_cmd(276).
		q(nt(256), (nt(276)));
		//       cli_command(256)     => bf_pnf_cmd(277).
		q(nt(256), (nt(277)));
		//       cli_command(256)     => bf_mnf_cmd(278).
		q(nt(256), (nt(278)));
		//       cli_command(256)     => bf_substitute_cmd(279).
		q(nt(256), (nt(279)));
		//       cli_command(256)     => wff_substitute_cmd(280).
		q(nt(256), (nt(280)));
		//       cli_command(256)     => bf_instantiate_cmd(281).
		q(nt(256), (nt(281)));
		//       cli_command(256)     => wff_instantiate_cmd(282).
		q(nt(256), (nt(282)));
		//       cli_command(256)     => wff_onf_cmd(283).
		q(nt(256), (nt(283)));
		//       cli_command(256)     => wff_dnf_cmd(284).
		q(nt(256), (nt(284)));
		//       cli_command(256)     => wff_cnf_cmd(285).
		q(nt(256), (nt(285)));
		//       cli_command(256)     => wff_anf_cmd(286).
		q(nt(256), (nt(286)));
		//       cli_command(256)     => wff_nnf_cmd(287).
		q(nt(256), (nt(287)));
		//       cli_command(256)     => wff_pnf_cmd(288).
		q(nt(256), (nt(288)));
		//       cli_command(256)     => wff_mnf_cmd(289).
		q(nt(256), (nt(289)));
		//       cli_command(256)     => def_rule_cmd(290).
		q(nt(256), (nt(290)));
		//       cli_command(256)     => def_list_cmd(291).
		q(nt(256), (nt(291)));
		//       cli_command(256)     => def_del_cmd(292).
		q(nt(256), (nt(292)));
		//       cli_command(256)     => def_clear_cmd(293).
		q(nt(256), (nt(293)));
		//       cli_command(256)     => bf(11).
		q(nt(256), (nt(11)));
		//       cli_command(256)     => wff(13).
		q(nt(256), (nt(13)));
		//       cli_command(256)     => nso_rr(241).
		q(nt(256), (nt(241)));
		//       normalize_cmd(270)   => normalize_cmd_sym(294) __(10) normalize_cmd_arg(295).
		q(nt(270), (nt(294)+nt(10)+nt(295)));
		//       normalize_cmd_arg(295) => wff(13).
		q(nt(295), (nt(13)));
		//       normalize_cmd_arg(295) => nso_rr(241).
		q(nt(295), (nt(241)));
		//       normalize_cmd_arg(295) => output(296).
		q(nt(295), (nt(296)));
		//       normalize_cmd_arg(295) => wff_selection(297).
		q(nt(295), (nt(297)));
		//       solve_cmd(272)       => solve_cmd_sym(298) __(10) solve_cmd_arg(299).
		q(nt(272), (nt(298)+nt(10)+nt(299)));
		//       solve_cmd_arg(299)   => wff(13).
		q(nt(299), (nt(13)));
		//       solve_cmd_arg(299)   => output(296).
		q(nt(299), (nt(296)));
		//       solve_cmd_arg(299)   => wff_selection(297).
		q(nt(299), (nt(297)));
		//       execute_cmd(271)     => execute_cmd_sym(300) __(10) execute_cmd_arg(301).
		q(nt(271), (nt(300)+nt(10)+nt(301)));
		//       execute_cmd_arg(301) => tau(15).
		q(nt(301), (nt(15)));
		//       execute_cmd_arg(301) => gssotc_rr(247).
		q(nt(301), (nt(247)));
		//       file(269)            => file_sym(302) quote(36) file_path(303) quote(36).
		q(nt(269), (nt(302)+nt(36)+nt(303)+nt(36)));
		//       __E_bf_instantiate_cmd_50(305) => bf(11).
		q(nt(305), (nt(11)));
		//       __E_bf_instantiate_cmd_50(305) => bf_selection(306).
		q(nt(305), (nt(306)));
		//       __E_bf_instantiate_cmd_50(305) => output(296).
		q(nt(305), (nt(296)));
		//       __E_bf_instantiate_cmd_51(307) => bf_selection(306).
		q(nt(307), (nt(306)));
		//       __E_bf_instantiate_cmd_51(307) => output(296).
		q(nt(307), (nt(296)));
		//       bf_instantiate_cmd(281) => instantiate_cmd_sym(304) __(10) variable(88) __(10) __E_bf_instantiate_cmd_50(305) __(10) __E_bf_instantiate_cmd_51(307).
		q(nt(281), (nt(304)+nt(10)+nt(88)+nt(10)+nt(305)+nt(10)+nt(307)));
		//       __E_wff_instantiate_cmd_52(308) => wff(13).
		q(nt(308), (nt(13)));
		//       __E_wff_instantiate_cmd_52(308) => wff_selection(297).
		q(nt(308), (nt(297)));
		//       __E_wff_instantiate_cmd_52(308) => output(296).
		q(nt(308), (nt(296)));
		//       __E_wff_instantiate_cmd_53(309) => wff_selection(297).
		q(nt(309), (nt(297)));
		//       __E_wff_instantiate_cmd_53(309) => output(296).
		q(nt(309), (nt(296)));
		//       wff_instantiate_cmd(282) => instantiate_cmd_sym(304) __(10) variable(88) __(10) __E_wff_instantiate_cmd_52(308) __(10) __E_wff_instantiate_cmd_53(309).
		q(nt(282), (nt(304)+nt(10)+nt(88)+nt(10)+nt(308)+nt(10)+nt(309)));
		//       __E_bf_substitute_cmd_54(311) => bf(11).
		q(nt(311), (nt(11)));
		//       __E_bf_substitute_cmd_54(311) => output(296).
		q(nt(311), (nt(296)));
		//       __E_bf_substitute_cmd_54(311) => bf_selection(306).
		q(nt(311), (nt(306)));
		//       __E_bf_substitute_cmd_55(312) => output(296).
		q(nt(312), (nt(296)));
		//       __E_bf_substitute_cmd_55(312) => bf_selection(306).
		q(nt(312), (nt(306)));
		//       __E_bf_substitute_cmd_55(312) => bf_selection(306).
		q(nt(312), (nt(306)));
		//       __E_bf_substitute_cmd_56(313) => bf_selection(306).
		q(nt(313), (nt(306)));
		//       __E_bf_substitute_cmd_56(313) => output(296).
		q(nt(313), (nt(296)));
		//       bf_substitute_cmd(279) => substitute_cmd_sym(310) __(10) __E_bf_substitute_cmd_54(311) __(10) __E_bf_substitute_cmd_55(312) __E_bf_substitute_cmd_56(313).
		q(nt(279), (nt(310)+nt(10)+nt(311)+nt(10)+nt(312)+nt(313)));
		//       __E_wff_substitute_cmd_57(315) => wff(13).
		q(nt(315), (nt(13)));
		//       __E_wff_substitute_cmd_57(315) => output(296).
		q(nt(315), (nt(296)));
		//       __E_wff_substitute_cmd_57(315) => wff_selection(297).
		q(nt(315), (nt(297)));
		//       __E_wff_substitute_cmd_58(316) => output(296).
		q(nt(316), (nt(296)));
		//       __E_wff_substitute_cmd_58(316) => wff_selection(297).
		q(nt(316), (nt(297)));
		//       __E_wff_substitute_cmd_59(317) => wff_selection(297).
		q(nt(317), (nt(297)));
		//       __E_wff_substitute_cmd_59(317) => output(296).
		q(nt(317), (nt(296)));
		//       wff_substitute_cmd(280) => substitutecmd__sym(314) __(10) __E_wff_substitute_cmd_57(315) __(10) __E_wff_substitute_cmd_58(316) __E_wff_substitute_cmd_59(317).
		q(nt(280), (nt(314)+nt(10)+nt(315)+nt(10)+nt(316)+nt(317)));
		//       __E_bf_dnf_cmd_60(319) => bf(11).
		q(nt(319), (nt(11)));
		//       __E_bf_dnf_cmd_60(319) => output(296).
		q(nt(319), (nt(296)));
		//       __E_bf_dnf_cmd_60(319) => bf_selection(306).
		q(nt(319), (nt(306)));
		//       bf_dnf_cmd(273)      => dnf_cmd_sym(318) __(10) __E_bf_dnf_cmd_60(319).
		q(nt(273), (nt(318)+nt(10)+nt(319)));
		//       __E_bf_cnf_cmd_61(321) => bf(11).
		q(nt(321), (nt(11)));
		//       __E_bf_cnf_cmd_61(321) => output(296).
		q(nt(321), (nt(296)));
		//       __E_bf_cnf_cmd_61(321) => bf_selection(306).
		q(nt(321), (nt(306)));
		//       bf_cnf_cmd(274)      => cnf_cmd_sym(320) __(10) __E_bf_cnf_cmd_61(321).
		q(nt(274), (nt(320)+nt(10)+nt(321)));
		//       __E_bf_anf_cmd_62(323) => bf(11).
		q(nt(323), (nt(11)));
		//       __E_bf_anf_cmd_62(323) => output(296).
		q(nt(323), (nt(296)));
		//       __E_bf_anf_cmd_62(323) => bf_selection(306).
		q(nt(323), (nt(306)));
		//       bf_anf_cmd(275)      => anf_cmd_sym(322) __(10) __E_bf_anf_cmd_62(323).
		q(nt(275), (nt(322)+nt(10)+nt(323)));
		//       __E_bf_nnf_cmd_63(325) => bf(11).
		q(nt(325), (nt(11)));
		//       __E_bf_nnf_cmd_63(325) => output(296).
		q(nt(325), (nt(296)));
		//       __E_bf_nnf_cmd_63(325) => bf_selection(306).
		q(nt(325), (nt(306)));
		//       bf_nnf_cmd(276)      => nnf_cmd_sym(324) __(10) __E_bf_nnf_cmd_63(325).
		q(nt(276), (nt(324)+nt(10)+nt(325)));
		//       __E_bf_pnf_cmd_64(327) => bf(11).
		q(nt(327), (nt(11)));
		//       __E_bf_pnf_cmd_64(327) => output(296).
		q(nt(327), (nt(296)));
		//       __E_bf_pnf_cmd_64(327) => bf_selection(306).
		q(nt(327), (nt(306)));
		//       bf_pnf_cmd(277)      => pnf_cmd_sym(326) __(10) __E_bf_pnf_cmd_64(327).
		q(nt(277), (nt(326)+nt(10)+nt(327)));
		//       __E_bf_mnf_cmd_65(329) => bf(11).
		q(nt(329), (nt(11)));
		//       __E_bf_mnf_cmd_65(329) => output(296).
		q(nt(329), (nt(296)));
		//       __E_bf_mnf_cmd_65(329) => bf_selection(306).
		q(nt(329), (nt(306)));
		//       bf_mnf_cmd(278)      => mnf_cmd_sym(328) __(10) __E_bf_mnf_cmd_65(329).
		q(nt(278), (nt(328)+nt(10)+nt(329)));
		//       __E_wff_onf_cmd_66(331) => wff(13).
		q(nt(331), (nt(13)));
		//       __E_wff_onf_cmd_66(331) => output(296).
		q(nt(331), (nt(296)));
		//       __E_wff_onf_cmd_66(331) => wff_selection(297).
		q(nt(331), (nt(297)));
		//       wff_onf_cmd(283)     => onf_cmd_sym(330) __(10) variable(88) __(10) __E_wff_onf_cmd_66(331).
		q(nt(283), (nt(330)+nt(10)+nt(88)+nt(10)+nt(331)));
		//       __E_wff_dnf_cmd_67(332) => wff(13).
		q(nt(332), (nt(13)));
		//       __E_wff_dnf_cmd_67(332) => output(296).
		q(nt(332), (nt(296)));
		//       __E_wff_dnf_cmd_67(332) => wff_selection(297).
		q(nt(332), (nt(297)));
		//       wff_dnf_cmd(284)     => dnf_cmd_sym(318) __(10) __E_wff_dnf_cmd_67(332).
		q(nt(284), (nt(318)+nt(10)+nt(332)));
		//       __E_wff_cnf_cmd_68(333) => wff(13).
		q(nt(333), (nt(13)));
		//       __E_wff_cnf_cmd_68(333) => output(296).
		q(nt(333), (nt(296)));
		//       __E_wff_cnf_cmd_68(333) => wff_selection(297).
		q(nt(333), (nt(297)));
		//       wff_cnf_cmd(285)     => cnf_cmd_sym(320) __(10) __E_wff_cnf_cmd_68(333).
		q(nt(285), (nt(320)+nt(10)+nt(333)));
		//       __E_wff_anf_cmd_69(334) => wff(13).
		q(nt(334), (nt(13)));
		//       __E_wff_anf_cmd_69(334) => output(296).
		q(nt(334), (nt(296)));
		//       __E_wff_anf_cmd_69(334) => wff_selection(297).
		q(nt(334), (nt(297)));
		//       wff_anf_cmd(286)     => anf_cmd_sym(322) __(10) __E_wff_anf_cmd_69(334).
		q(nt(286), (nt(322)+nt(10)+nt(334)));
		//       __E_wff_nnf_cmd_70(335) => wff(13).
		q(nt(335), (nt(13)));
		//       __E_wff_nnf_cmd_70(335) => output(296).
		q(nt(335), (nt(296)));
		//       __E_wff_nnf_cmd_70(335) => wff_selection(297).
		q(nt(335), (nt(297)));
		//       wff_nnf_cmd(287)     => nnf_cmd_sym(324) __(10) __E_wff_nnf_cmd_70(335).
		q(nt(287), (nt(324)+nt(10)+nt(335)));
		//       __E_wff_pnf_cmd_71(336) => wff(13).
		q(nt(336), (nt(13)));
		//       __E_wff_pnf_cmd_71(336) => output(296).
		q(nt(336), (nt(296)));
		//       __E_wff_pnf_cmd_71(336) => wff_selection(297).
		q(nt(336), (nt(297)));
		//       wff_pnf_cmd(288)     => pnf_cmd_sym(326) __(10) __E_wff_pnf_cmd_71(336).
		q(nt(288), (nt(326)+nt(10)+nt(336)));
		//       __E_wff_mnf_cmd_72(337) => wff(13).
		q(nt(337), (nt(13)));
		//       __E_wff_mnf_cmd_72(337) => output(296).
		q(nt(337), (nt(296)));
		//       __E_wff_mnf_cmd_72(337) => wff_selection(297).
		q(nt(337), (nt(297)));
		//       wff_mnf_cmd(289)     => mnf_cmd_sym(328) __(10) __E_wff_mnf_cmd_72(337).
		q(nt(289), (nt(328)+nt(10)+nt(337)));
		//       def_rule_cmd(290)    => def_cmd_sym(338) __(10) def_rule_cmd_arg(339).
		q(nt(290), (nt(338)+nt(10)+nt(339)));
		//       __E_def_rule_cmd_arg_73(340) => bf_rec_relation(183).
		q(nt(340), (nt(183)));
		//       __E_def_rule_cmd_arg_73(340) => wff_rec_relation(129).
		q(nt(340), (nt(129)));
		//       __E_def_rule_cmd_arg_73(340) => tau_rec_relation(108).
		q(nt(340), (nt(108)));
		//       def_rule_cmd_arg(339) => __E_def_rule_cmd_arg_73(340).
		q(nt(339), (nt(340)));
		//       def_list_cmd(291)    => def_cmd_sym(338) __(10) def_cmd_list(341).
		q(nt(291), (nt(338)+nt(10)+nt(341)));
		//       def_del_cmd(292)     => def_cmd_sym(338) __(10) def_cmd_del(342) __(10) digits(77).
		q(nt(292), (nt(338)+nt(10)+nt(342)+nt(10)+nt(77)));
		//       def_clear_cmd(293)   => def_cmd_sym(338) __(10) def_cmf_clear(343).
		q(nt(293), (nt(338)+nt(10)+nt(343)));
		//       __E_help_cmd_74(345) => __(10) cli_cmd_sym(346).
		q(nt(345), (nt(10)+nt(346)));
		//       __E_help_cmd_74(345) => null.
		q(nt(345), (nul));
		//       help_cmd(259)        => help_cmd_sym(344) __E_help_cmd_74(345).
		q(nt(259), (nt(344)+nt(345)));
		//       version_cmd(260)     => version_cmd_sym(347).
		q(nt(260), (nt(347)));
		//       quit_cmd(261)        => quit_cmd_sym(348).
		q(nt(261), (nt(348)));
		//       clear_cmd(262)       => clear_cmd_sym(349).
		q(nt(262), (nt(349)));
		//       wff_selection(297)   => selection_sym(350) __(10) digits(77).
		q(nt(297), (nt(350)+nt(10)+nt(77)));
		//       bf_selection(306)    => selection_sym(350) __(10) digits(77).
		q(nt(306), (nt(350)+nt(10)+nt(77)));
		//       bf_var_selection(351) => selection_sym(350) __(10) digits(77).
		q(nt(351), (nt(350)+nt(10)+nt(77)));
		//       wff_var_selection(352) => selection_sym(350) __(10) digits(77).
		q(nt(352), (nt(350)+nt(10)+nt(77)));
		//       file_path(303)       => chars(74).
		q(nt(303), (nt(74)));
		//       output(296)          => absolute_output(353).
		q(nt(296), (nt(353)));
		//       output(296)          => relative_output(354).
		q(nt(296), (nt(354)));
		//       __E_absolute_output_75(355) => absolute_output_sym(356).
		q(nt(355), (nt(356)));
		//       __E_absolute_output_75(355) => output_sym(357) __(10).
		q(nt(355), (nt(357)+nt(10)));
		//       absolute_output(353) => __E_absolute_output_75(355) output_id(358).
		q(nt(353), (nt(355)+nt(358)));
		//       relative_output(354) => relative_output_sym(359) output_id(358).
		q(nt(354), (nt(359)+nt(358)));
		//       output_id(358)       => digits(77).
		q(nt(358), (nt(77)));
		//       list_outputs_cmd(266) => output_sym(357).
		q(nt(266), (nt(357)));
		//       list_outputs_cmd(266) => absolute_output_sym(356).
		q(nt(266), (nt(356)));
		//       list_outputs_cmd(266) => relative_output_sym(359).
		q(nt(266), (nt(359)));
		//       __E_clear_outputs_cmd_76(360) => output_sym(357) __(10).
		q(nt(360), (nt(357)+nt(10)));
		//       __E_clear_outputs_cmd_76(360) => absolute_output_sym(356).
		q(nt(360), (nt(356)));
		//       __E_clear_outputs_cmd_76(360) => relative_output_sym(359).
		q(nt(360), (nt(359)));
		//       clear_outputs_cmd(267) => __E_clear_outputs_cmd_76(360) clear_sym(361).
		q(nt(267), (nt(360)+nt(361)));
		//       print_output_cmd(268) => output(296).
		q(nt(268), (nt(296)));
		//       cli_cmd_sym(346)     => help_cmd_sym(344).
		q(nt(346), (nt(344)));
		//       cli_cmd_sym(346)     => version_cmd_sym(347).
		q(nt(346), (nt(347)));
		//       cli_cmd_sym(346)     => quit_cmd_sym(348).
		q(nt(346), (nt(348)));
		//       cli_cmd_sym(346)     => clear_cmd_sym(349).
		q(nt(346), (nt(349)));
		//       cli_cmd_sym(346)     => def_cmd_sym(338).
		q(nt(346), (nt(338)));
		//       cli_cmd_sym(346)     => output_sym(357).
		q(nt(346), (nt(357)));
		//       cli_cmd_sym(346)     => absolute_output_sym(356).
		q(nt(346), (nt(356)));
		//       cli_cmd_sym(346)     => relative_output_sym(359).
		q(nt(346), (nt(359)));
		//       cli_cmd_sym(346)     => selection_sym(350).
		q(nt(346), (nt(350)));
		//       cli_cmd_sym(346)     => instantiate_cmd_sym(304).
		q(nt(346), (nt(304)));
		//       cli_cmd_sym(346)     => substitute_cmd_sym(310).
		q(nt(346), (nt(310)));
		//       cli_cmd_sym(346)     => normalize_cmd_sym(294).
		q(nt(346), (nt(294)));
		//       cli_cmd_sym(346)     => execute_cmd_sym(300).
		q(nt(346), (nt(300)));
		//       cli_cmd_sym(346)     => file_cmd_sym(362).
		q(nt(346), (nt(362)));
		//       cli_cmd_sym(346)     => get_cmd_sym(363).
		q(nt(346), (nt(363)));
		//       cli_cmd_sym(346)     => set_cmd_sym(364).
		q(nt(346), (nt(364)));
		//       cli_cmd_sym(346)     => toggle_cmd_sym(365).
		q(nt(346), (nt(365)));
		//       cli_cmd_sym(346)     => cnf_cmd_sym(320).
		q(nt(346), (nt(320)));
		//       cli_cmd_sym(346)     => dnf_cmd_sym(318).
		q(nt(346), (nt(318)));
		//       cli_cmd_sym(346)     => nnf_cmd_sym(324).
		q(nt(346), (nt(324)));
		//       cli_cmd_sym(346)     => mnf_cmd_sym(328).
		q(nt(346), (nt(328)));
		//       cli_cmd_sym(346)     => onf_cmd_sym(330).
		q(nt(346), (nt(330)));
		//       help_cmd_sym(344)    => 'h'.
		q(nt(344), (t(42)));
		//       help_cmd_sym(344)    => 'h' 'e' 'l' 'p'.
		q(nt(344), (t(42)+t(28)+t(27)+t(56)));
		//       version_cmd_sym(347) => 'v'.
		q(nt(347), (t(51)));
		//       version_cmd_sym(347) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
		q(nt(347), (t(51)+t(28)+t(45)+t(43)+t(41)+t(31)+t(49)));
		//       quit_cmd_sym(348)    => 'q'.
		q(nt(348), (t(54)));
		//       quit_cmd_sym(348)    => 'e'.
		q(nt(348), (t(28)));
		//       quit_cmd_sym(348)    => 'q' 'u' 'i' 't'.
		q(nt(348), (t(54)+t(44)+t(41)+t(52)));
		//       quit_cmd_sym(348)    => 'e' 'x' 'i' 't'.
		q(nt(348), (t(28)+t(29)+t(41)+t(52)));
		//       instantiate_cmd_sym(304) => 'i'.
		q(nt(304), (t(41)));
		//       instantiate_cmd_sym(304) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
		q(nt(304), (t(41)+t(49)+t(43)+t(52)+t(26)+t(49)+t(52)+t(41)+t(26)+t(52)+t(28)));
		//       substitute_cmd_sym(310) => 's'.
		q(nt(310), (t(43)));
		//       substitute_cmd_sym(310) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
		q(nt(310), (t(43)+t(44)+t(30)+t(43)+t(52)+t(41)+t(52)+t(44)+t(52)+t(28)));
		//       file_cmd_sym(362)    => 'r'.
		q(nt(362), (t(45)));
		//       file_cmd_sym(362)    => 'r' 'e' 'a' 'd'.
		q(nt(362), (t(45)+t(28)+t(26)+t(53)));
		//       clear_cmd_sym(349)   => 'c'.
		q(nt(349), (t(47)));
		//       clear_cmd_sym(349)   => 'c' 'l' 'e' 'a' 'r'.
		q(nt(349), (t(47)+t(27)+t(28)+t(26)+t(45)));
		//       normalize_cmd_sym(294) => 'n'.
		q(nt(294), (t(49)));
		//       normalize_cmd_sym(294) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
		q(nt(294), (t(49)+t(31)+t(45)+t(46)+t(26)+t(27)+t(41)+t(55)+t(28)));
		//       execute_cmd_sym(300) => 'e' 'x' 'e' 'c' 'u' 't' 'e'.
		q(nt(300), (t(28)+t(29)+t(28)+t(47)+t(44)+t(52)+t(28)));
		//       solve_cmd_sym(298)   => 's' 'o' 'l' 'v' 'e'.
		q(nt(298), (t(43)+t(31)+t(27)+t(51)+t(28)));
		//       onf_cmd_sym(330)     => 'o' 'n' 'f'.
		q(nt(330), (t(31)+t(49)+t(36)));
		//       dnf_cmd_sym(318)     => 'd' 'n' 'f'.
		q(nt(318), (t(53)+t(49)+t(36)));
		//       cnf_cmd_sym(320)     => 'c' 'n' 'f'.
		q(nt(320), (t(47)+t(49)+t(36)));
		//       anf_cmd_sym(322)     => 'a' 'n' 'f'.
		q(nt(322), (t(26)+t(49)+t(36)));
		//       nnf_cmd_sym(324)     => 'n' 'n' 'f'.
		q(nt(324), (t(49)+t(49)+t(36)));
		//       pnf_cmd_sym(326)     => 'p' 'n' 'f'.
		q(nt(326), (t(56)+t(49)+t(36)));
		//       mnf_cmd_sym(328)     => 'm' 'n' 'f'.
		q(nt(328), (t(46)+t(49)+t(36)));
		//       def_cmd_sym(338)     => 'd' 'e' 'f'.
		q(nt(338), (t(53)+t(28)+t(36)));
		//       def_cmd_list(341)    => 'l' 'i' 's' 't'.
		q(nt(341), (t(27)+t(41)+t(43)+t(52)));
		//       def_cmd_del(342)     => 'd' 'e' 'l'.
		q(nt(342), (t(53)+t(28)+t(27)));
		//       def_cmf_clear(343)   => 'c' 'l' 'e' 'a' 'r'.
		q(nt(343), (t(47)+t(27)+t(28)+t(26)+t(45)));
		//       selection_sym(350)   => 's'.
		q(nt(350), (t(43)));
		//       selection_sym(350)   => 's' 'e' 'l' 'e' 'c' 't' 'i' 'o' 'n'.
		q(nt(350), (t(43)+t(28)+t(27)+t(28)+t(47)+t(52)+t(41)+t(31)+t(49)));
		//       output_sym(357)      => 'o'.
		q(nt(357), (t(31)));
		//       output_sym(357)      => 'o' 'u' 't' 'p' 'u' 't'.
		q(nt(357), (t(31)+t(44)+t(52)+t(56)+t(44)+t(52)));
		//       absolute_output_sym(356) => '&'.
		q(nt(356), (t(20)));
		//       relative_output_sym(359) => '%'.
		q(nt(359), (t(57)));
		//       __E_get_cmd_77(366)  => __(10) option(367).
		q(nt(366), (nt(10)+nt(367)));
		//       __E_get_cmd_77(366)  => null.
		q(nt(366), (nul));
		//       get_cmd(263)         => get_cmd_sym(363) __E_get_cmd_77(366).
		q(nt(263), (nt(363)+nt(366)));
		//       __E_set_cmd_78(368)  => __(10).
		q(nt(368), (nt(10)));
		//       __E_set_cmd_78(368)  => _(9) '=' _(9).
		q(nt(368), (nt(9)+t(19)+nt(9)));
		//       set_cmd(264)         => set_cmd_sym(364) __(10) option(367) __E_set_cmd_78(368) option_value(369).
		q(nt(264), (nt(364)+nt(10)+nt(367)+nt(368)+nt(369)));
		//       toggle_cmd(265)      => toggle_cmd_sym(365) __(10) bool_option(370).
		q(nt(265), (nt(365)+nt(10)+nt(370)));
		//       get_cmd_sym(363)     => 'g' 'e' 't'.
		q(nt(363), (t(50)+t(28)+t(52)));
		//       set_cmd_sym(364)     => 's' 'e' 't'.
		q(nt(364), (t(43)+t(28)+t(52)));
		//       toggle_cmd_sym(365)  => 't' 'o' 'g' 'g' 'l' 'e'.
		q(nt(365), (t(52)+t(31)+t(50)+t(50)+t(27)+t(28)));
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
