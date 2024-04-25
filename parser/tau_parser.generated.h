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
		g(nts, load_prods(), nt(254), cc, load_grammar_opts()),
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
		wff_xor, wff_neg, bf_interval, bf_neq, bf_eq, bf_nleq_lower, bf_nleq_upper, bf_greater, bf_less_equal, bf_less, 
		__E_wff_ref_19, wff_ref_args, __E_wff_ref_args_20, wff_ref_arg, __E_wff_ref_args_21, __E_wff_and_22, __E_wff_all_23, __E_wff_ex_24, __E_wff_ball_25, __E_wff_bex_26, 
		__E_bf_nleq_lower_27, __E_bf_nleq_upper_28, __E_bf_interval_29, bf_rule, bf_matcher, bf_body, bf_is_zero_cb, bf_is_one_cb, bf_has_subformula_cb, bf_remove_funiversal_cb, 
		bf_remove_fexistential_cb, bf_and_cb, bf_or_cb, bf_xor_cb, bf_neg_cb, bf_splitter, bf_ref, bf_all, bf_ex, bf_or, 
		bf_and, bf_xor, bf_neg, bf_constant, bf_rec_relation, __E_bf_ref_30, bf_ref_args, __E_bf_ref_args_31, bf_ref_arg, __E_bf_ref_args_32, 
		__E_bf_and_33, __E_bf_all_34, __E_bf_ex_35, constant, binding, source_binding, named_binding, type, source, source0, 
		__E_source_36, __E_source_37, bf_and_cb_sym, bf_cb_arg, bf_or_cb_sym, bf_xor_cb_sym, bf_neg_cb_sym, bf_eq_cb_sym, wff_cb_arg, bf_neq_cb_sym, 
		bf_is_zero_cb_sym, bf_is_one_cb_sym, bf_remove_funiversal_cb_sym, bf_remove_fexistential_cb_sym, wff_remove_existential_cb_sym, wff_remove_bexistential_cb_sym, wff_remove_buniversal_cb_sym, wff_has_clashing_subformulas_cb_sym, bf_has_subformula_cb_sym, wff_has_subformula_cb_sym, 
		__E_tau_collapse_positives_cb_38, tau_collapse_positives_cb_sym, tau_cb_arg, __E_tau_collapse_positives_cb_39, tau_positives_upwards_cb_sym, inputs, input, __E_inputs_40, __E_inputs_41, builder, 
		builder_head, builder_body, __E_builder_head_42, __E_builder_head_43, bf_builder_body, wff_builder_body, tau_builder_body, library, rules, __E_rules_44, 
		rule, __E_rules_45, nso_rr, nso_rec_relations, nso_main, __E_nso_rec_relations_46, nso_rec_relation, __E_nso_rec_relations_47, gssotc_rr, gssotc_rec_relations, 
		gssotc_main, __E_gssotc_rec_relations_48, gssotc_rec_relation, __E_gssotc_rec_relations_49, start, __E_start_50, cli, cli_command, __E_cli_51, __E_cli_52, 
		help_cmd, version_cmd, quit_cmd, clear_cmd, get_cmd, set_cmd, toggle_cmd, list_outputs_cmd, clear_outputs_cmd, print_output_cmd, 
		file, normalize_cmd, execute_cmd, solve_cmd, is_satisfiable_cmd, is_valid_cmd, is_unsatisfiable_cmd, bf_dnf_cmd, bf_cnf_cmd, bf_anf_cmd, 
		bf_nnf_cmd, bf_pnf_cmd, bf_mnf_cmd, wff_onf_cmd, wff_dnf_cmd, wff_cnf_cmd, wff_anf_cmd, wff_nnf_cmd, wff_pnf_cmd, wff_mnf_cmd, 
		bf_substitute_cmd, wff_substitute_cmd, bf_instantiate_cmd, wff_instantiate_cmd, def_rule_cmd, def_list_cmd, def_del_cmd, def_clear_cmd, normalize_cmd_sym, normalize_cmd_arg, 
		output, wff_selection, solve_cmd_sym, solve_cmd_arg, execute_cmd_sym, execute_cmd_arg, is_satisfiable_cmd_sym, is_satisfiable_cmd_arg, is_valid_cmd_sym, is_valid_cmd_arg, 
		is_unsatisfiable_cmd_sym, is_unsatisfiable_cmd_arg, file_sym, file_path, instantiate_cmd_sym, __E_bf_instantiate_cmd_53, bf_selection, __E_bf_instantiate_cmd_54, __E_wff_instantiate_cmd_55, __E_wff_instantiate_cmd_56, 
		substitute_cmd_sym, __E_bf_substitute_cmd_57, __E_bf_substitute_cmd_58, __E_bf_substitute_cmd_59, substitutecmd__sym, __E_wff_substitute_cmd_60, __E_wff_substitute_cmd_61, __E_wff_substitute_cmd_62, dnf_cmd_sym, __E_bf_dnf_cmd_63, 
		cnf_cmd_sym, __E_bf_cnf_cmd_64, anf_cmd_sym, __E_bf_anf_cmd_65, nnf_cmd_sym, __E_bf_nnf_cmd_66, pnf_cmd_sym, __E_bf_pnf_cmd_67, mnf_cmd_sym, __E_bf_mnf_cmd_68, 
		onf_cmd_sym, __E_wff_onf_cmd_69, __E_wff_dnf_cmd_70, __E_wff_cnf_cmd_71, __E_wff_anf_cmd_72, __E_wff_nnf_cmd_73, __E_wff_pnf_cmd_74, __E_wff_mnf_cmd_75, def_cmd_sym, def_rule_cmd_arg, 
		__E_def_rule_cmd_arg_76, def_cmd_list, def_cmd_del, def_cmf_clear, help_cmd_sym, __E_help_cmd_77, cli_cmd_sym, version_cmd_sym, quit_cmd_sym, clear_cmd_sym, 
		selection_sym, bf_var_selection, wff_var_selection, absolute_output, relative_output, __E_absolute_output_78, absolute_output_sym, output_sym, output_id, relative_output_sym, 
		__E_clear_outputs_cmd_79, clear_sym, file_cmd_sym, examples_sym, get_cmd_sym, set_cmd_sym, toggle_cmd_sym, __E_get_cmd_80, option, __E_set_cmd_81, 
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
			"wff_xor", "wff_neg", "bf_interval", "bf_neq", "bf_eq", "bf_nleq_lower", "bf_nleq_upper", "bf_greater", "bf_less_equal", "bf_less", 
			"__E_wff_ref_19", "wff_ref_args", "__E_wff_ref_args_20", "wff_ref_arg", "__E_wff_ref_args_21", "__E_wff_and_22", "__E_wff_all_23", "__E_wff_ex_24", "__E_wff_ball_25", "__E_wff_bex_26", 
			"__E_bf_nleq_lower_27", "__E_bf_nleq_upper_28", "__E_bf_interval_29", "bf_rule", "bf_matcher", "bf_body", "bf_is_zero_cb", "bf_is_one_cb", "bf_has_subformula_cb", "bf_remove_funiversal_cb", 
			"bf_remove_fexistential_cb", "bf_and_cb", "bf_or_cb", "bf_xor_cb", "bf_neg_cb", "bf_splitter", "bf_ref", "bf_all", "bf_ex", "bf_or", 
			"bf_and", "bf_xor", "bf_neg", "bf_constant", "bf_rec_relation", "__E_bf_ref_30", "bf_ref_args", "__E_bf_ref_args_31", "bf_ref_arg", "__E_bf_ref_args_32", 
			"__E_bf_and_33", "__E_bf_all_34", "__E_bf_ex_35", "constant", "binding", "source_binding", "named_binding", "type", "source", "source0", 
			"__E_source_36", "__E_source_37", "bf_and_cb_sym", "bf_cb_arg", "bf_or_cb_sym", "bf_xor_cb_sym", "bf_neg_cb_sym", "bf_eq_cb_sym", "wff_cb_arg", "bf_neq_cb_sym", 
			"bf_is_zero_cb_sym", "bf_is_one_cb_sym", "bf_remove_funiversal_cb_sym", "bf_remove_fexistential_cb_sym", "wff_remove_existential_cb_sym", "wff_remove_bexistential_cb_sym", "wff_remove_buniversal_cb_sym", "wff_has_clashing_subformulas_cb_sym", "bf_has_subformula_cb_sym", "wff_has_subformula_cb_sym", 
			"__E_tau_collapse_positives_cb_38", "tau_collapse_positives_cb_sym", "tau_cb_arg", "__E_tau_collapse_positives_cb_39", "tau_positives_upwards_cb_sym", "inputs", "input", "__E_inputs_40", "__E_inputs_41", "builder", 
			"builder_head", "builder_body", "__E_builder_head_42", "__E_builder_head_43", "bf_builder_body", "wff_builder_body", "tau_builder_body", "library", "rules", "__E_rules_44", 
			"rule", "__E_rules_45", "nso_rr", "nso_rec_relations", "nso_main", "__E_nso_rec_relations_46", "nso_rec_relation", "__E_nso_rec_relations_47", "gssotc_rr", "gssotc_rec_relations", 
			"gssotc_main", "__E_gssotc_rec_relations_48", "gssotc_rec_relation", "__E_gssotc_rec_relations_49", "start", "__E_start_50", "cli", "cli_command", "__E_cli_51", "__E_cli_52", 
			"help_cmd", "version_cmd", "quit_cmd", "clear_cmd", "get_cmd", "set_cmd", "toggle_cmd", "list_outputs_cmd", "clear_outputs_cmd", "print_output_cmd", 
			"file", "normalize_cmd", "execute_cmd", "solve_cmd", "is_satisfiable_cmd", "is_valid_cmd", "is_unsatisfiable_cmd", "bf_dnf_cmd", "bf_cnf_cmd", "bf_anf_cmd", 
			"bf_nnf_cmd", "bf_pnf_cmd", "bf_mnf_cmd", "wff_onf_cmd", "wff_dnf_cmd", "wff_cnf_cmd", "wff_anf_cmd", "wff_nnf_cmd", "wff_pnf_cmd", "wff_mnf_cmd", 
			"bf_substitute_cmd", "wff_substitute_cmd", "bf_instantiate_cmd", "wff_instantiate_cmd", "def_rule_cmd", "def_list_cmd", "def_del_cmd", "def_clear_cmd", "normalize_cmd_sym", "normalize_cmd_arg", 
			"output", "wff_selection", "solve_cmd_sym", "solve_cmd_arg", "execute_cmd_sym", "execute_cmd_arg", "is_satisfiable_cmd_sym", "is_satisfiable_cmd_arg", "is_valid_cmd_sym", "is_valid_cmd_arg", 
			"is_unsatisfiable_cmd_sym", "is_unsatisfiable_cmd_arg", "file_sym", "file_path", "instantiate_cmd_sym", "__E_bf_instantiate_cmd_53", "bf_selection", "__E_bf_instantiate_cmd_54", "__E_wff_instantiate_cmd_55", "__E_wff_instantiate_cmd_56", 
			"substitute_cmd_sym", "__E_bf_substitute_cmd_57", "__E_bf_substitute_cmd_58", "__E_bf_substitute_cmd_59", "substitutecmd__sym", "__E_wff_substitute_cmd_60", "__E_wff_substitute_cmd_61", "__E_wff_substitute_cmd_62", "dnf_cmd_sym", "__E_bf_dnf_cmd_63", 
			"cnf_cmd_sym", "__E_bf_cnf_cmd_64", "anf_cmd_sym", "__E_bf_anf_cmd_65", "nnf_cmd_sym", "__E_bf_nnf_cmd_66", "pnf_cmd_sym", "__E_bf_pnf_cmd_67", "mnf_cmd_sym", "__E_bf_mnf_cmd_68", 
			"onf_cmd_sym", "__E_wff_onf_cmd_69", "__E_wff_dnf_cmd_70", "__E_wff_cnf_cmd_71", "__E_wff_anf_cmd_72", "__E_wff_nnf_cmd_73", "__E_wff_pnf_cmd_74", "__E_wff_mnf_cmd_75", "def_cmd_sym", "def_rule_cmd_arg", 
			"__E_def_rule_cmd_arg_76", "def_cmd_list", "def_cmd_del", "def_cmf_clear", "help_cmd_sym", "__E_help_cmd_77", "cli_cmd_sym", "version_cmd_sym", "quit_cmd_sym", "clear_cmd_sym", 
			"selection_sym", "bf_var_selection", "wff_var_selection", "absolute_output", "relative_output", "__E_absolute_output_78", "absolute_output_sym", "output_sym", "output_id", "relative_output_sym", 
			"__E_clear_outputs_cmd_79", "clear_sym", "file_cmd_sym", "examples_sym", "get_cmd_sym", "set_cmd_sym", "toggle_cmd_sym", "__E_get_cmd_80", "option", "__E_set_cmd_81", 
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
		//       __N_0(393)           => io_var(89).
		q(nt(393), (nt(89)));
		//       __E_offset_10(87)    => variable(88) & ~( __N_0(393) ).	 # conjunctive
		q(nt(87), (nt(88)) & ~(nt(393)));
		//       offset(81)           => __E_offset_10(87).
		q(nt(81), (nt(87)));
		//       __E_shift_11(90)     => capture(85).
		q(nt(90), (nt(85)));
		//       __N_1(394)           => io_var(89).
		q(nt(394), (nt(89)));
		//       __E___E_shift_11_12(91) => variable(88) & ~( __N_1(394) ).	 # conjunctive
		q(nt(91), (nt(88)) & ~(nt(394)));
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
		//       __N_2(395)           => 'F'.
		q(nt(395), (t(34)));
		//       __N_3(396)           => 'T'.
		q(nt(396), (t(33)));
		//       __E_charvar_13(98)   => ~( __N_2(395) ) & ~( __N_3(396) ) & alpha(5).	 # conjunctive
		q(nt(98), ~(nt(395)) & ~(nt(396)) & (nt(5)));
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
		//       wff(13)              => bf_nleq_lower(145).
		q(nt(13), (nt(145)));
		//       wff(13)              => bf_nleq_upper(146).
		q(nt(13), (nt(146)));
		//       wff(13)              => bf_greater(147).
		q(nt(13), (nt(147)));
		//       wff(13)              => bf_less_equal(148).
		q(nt(13), (nt(148)));
		//       wff(13)              => bf_less(149).
		q(nt(13), (nt(149)));
		//       wff(13)              => capture(85).
		q(nt(13), (nt(85)));
		//       wff(13)              => bool_variable(97).
		q(nt(13), (nt(97)));
		//       wff(13)              => wff_parenthesis(14).
		q(nt(13), (nt(14)));
		//       wff_parenthesis(14)  => open_parenthesis(24) _(9) wff(13) _(9) close_parenthesis(25).
		q(nt(14), (nt(24)+nt(9)+nt(13)+nt(9)+nt(25)));
		//       __E_wff_ref_19(150)  => offsets(80).
		q(nt(150), (nt(80)));
		//       __E_wff_ref_19(150)  => null.
		q(nt(150), (nul));
		//       wff_ref(130)         => sym(79) _(9) __E_wff_ref_19(150) _(9) wff_ref_args(151).
		q(nt(130), (nt(79)+nt(9)+nt(150)+nt(9)+nt(151)));
		//       __E_wff_ref_args_20(152) => _(9) wff_ref_arg(153).
		q(nt(152), (nt(9)+nt(153)));
		//       __E_wff_ref_args_21(154) => null.
		q(nt(154), (nul));
		//       __E_wff_ref_args_21(154) => __E_wff_ref_args_20(152) __E_wff_ref_args_21(154).
		q(nt(154), (nt(152)+nt(154)));
		//       wff_ref_args(151)    => open_parenthesis(24) __E_wff_ref_args_21(154) _(9) close_parenthesis(25).
		q(nt(151), (nt(24)+nt(154)+nt(9)+nt(25)));
		//       wff_ref_arg(153)     => bf(11).
		q(nt(153), (nt(11)));
		//       wff_ref_arg(153)     => capture(85).
		q(nt(153), (nt(85)));
		//       wff_ref_arg(153)     => variable(88).
		q(nt(153), (nt(88)));
		//       __E_wff_and_22(155)  => wff_and_sym(44) _(9).
		q(nt(155), (nt(44)+nt(9)));
		//       __E_wff_and_22(155)  => null.
		q(nt(155), (nul));
		//       wff_and(139)         => wff(13) _(9) __E_wff_and_22(155) wff(13).
		q(nt(139), (nt(13)+nt(9)+nt(155)+nt(13)));
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
		//       __E_wff_all_23(156)  => variable(88).
		q(nt(156), (nt(88)));
		//       __E_wff_all_23(156)  => capture(85).
		q(nt(156), (nt(85)));
		//       wff_all(134)         => wff_all_sym(51) __(10) __E_wff_all_23(156) __(10) wff(13).
		q(nt(134), (nt(51)+nt(10)+nt(156)+nt(10)+nt(13)));
		//       __E_wff_ex_24(157)   => variable(88).
		q(nt(157), (nt(88)));
		//       __E_wff_ex_24(157)   => capture(85).
		q(nt(157), (nt(85)));
		//       wff_ex(135)          => wff_ex_sym(52) __(10) __E_wff_ex_24(157) __(10) wff(13).
		q(nt(135), (nt(52)+nt(10)+nt(157)+nt(10)+nt(13)));
		//       __E_wff_ball_25(158) => bool_variable(97).
		q(nt(158), (nt(97)));
		//       __E_wff_ball_25(158) => capture(85).
		q(nt(158), (nt(85)));
		//       wff_ball(132)        => wff_ball_sym(53) __(10) __E_wff_ball_25(158) __(10) wff(13).
		q(nt(132), (nt(53)+nt(10)+nt(158)+nt(10)+nt(13)));
		//       __E_wff_bex_26(159)  => bool_variable(97).
		q(nt(159), (nt(97)));
		//       __E_wff_bex_26(159)  => capture(85).
		q(nt(159), (nt(85)));
		//       wff_bex(133)         => wff_bex_sym(54) __(10) __E_wff_bex_26(159) __(10) wff(13).
		q(nt(133), (nt(54)+nt(10)+nt(159)+nt(10)+nt(13)));
		//       bf_eq(144)           => bf(11) _(9) bf_equality_sym(63) _(9) bf(11).
		q(nt(144), (nt(11)+nt(9)+nt(63)+nt(9)+nt(11)));
		//       bf_neq(143)          => bf(11) _(9) bf_nequality_sym(64) _(9) bf(11).
		q(nt(143), (nt(11)+nt(9)+nt(64)+nt(9)+nt(11)));
		//       bf_less(149)         => bf(11) _(9) bf_less_sym(65) _(9) bf(11).
		q(nt(149), (nt(11)+nt(9)+nt(65)+nt(9)+nt(11)));
		//       bf_less_equal(148)   => bf(11) _(9) bf_less_equal_sym(66) _(9) bf(11).
		q(nt(148), (nt(11)+nt(9)+nt(66)+nt(9)+nt(11)));
		//       __E_bf_nleq_lower_27(160) => variable(88).
		q(nt(160), (nt(88)));
		//       __E_bf_nleq_lower_27(160) => capture(85).
		q(nt(160), (nt(85)));
		//       bf_nleq_lower(145)   => bf(11) _(9) bf_not_less_equal_sym(67) _(9) __E_bf_nleq_lower_27(160).
		q(nt(145), (nt(11)+nt(9)+nt(67)+nt(9)+nt(160)));
		//       __E_bf_nleq_upper_28(161) => variable(88).
		q(nt(161), (nt(88)));
		//       __E_bf_nleq_upper_28(161) => capture(85).
		q(nt(161), (nt(85)));
		//       bf_nleq_upper(146)   => __E_bf_nleq_upper_28(161) _(9) bf_not_less_equal_sym(67) _(9) bf(11).
		q(nt(146), (nt(161)+nt(9)+nt(67)+nt(9)+nt(11)));
		//       bf_greater(147)      => bf(11) _(9) bf_greater_sym(68) _(9) bf(11).
		q(nt(147), (nt(11)+nt(9)+nt(68)+nt(9)+nt(11)));
		//       __E_bf_interval_29(162) => variable(88).
		q(nt(162), (nt(88)));
		//       __E_bf_interval_29(162) => capture(85).
		q(nt(162), (nt(85)));
		//       bf_interval(142)     => bf(11) _(9) bf_less_equal_sym(66) _(9) __E_bf_interval_29(162) _(9) bf_less_equal_sym(66) _(9) bf(11).
		q(nt(142), (nt(11)+nt(9)+nt(66)+nt(9)+nt(162)+nt(9)+nt(66)+nt(9)+nt(11)));
		//       bf_rule(163)         => bf_matcher(164) _(9) bf_def(57) _(9) bf_body(165) _(9) dot(23).
		q(nt(163), (nt(164)+nt(9)+nt(57)+nt(9)+nt(165)+nt(9)+nt(23)));
		//       bf_matcher(164)      => bf(11).
		q(nt(164), (nt(11)));
		//       bf_body(165)         => bf(11).
		q(nt(165), (nt(11)));
		//       bf_body(165)         => bf_is_zero_cb(166).
		q(nt(165), (nt(166)));
		//       bf_body(165)         => bf_is_one_cb(167).
		q(nt(165), (nt(167)));
		//       bf_body(165)         => bf_has_subformula_cb(168).
		q(nt(165), (nt(168)));
		//       bf_body(165)         => bf_remove_funiversal_cb(169).
		q(nt(165), (nt(169)));
		//       bf_body(165)         => bf_remove_fexistential_cb(170).
		q(nt(165), (nt(170)));
		//       bf_body(165)         => bf_and_cb(171).
		q(nt(165), (nt(171)));
		//       bf_body(165)         => bf_or_cb(172).
		q(nt(165), (nt(172)));
		//       bf_body(165)         => bf_xor_cb(173).
		q(nt(165), (nt(173)));
		//       bf_body(165)         => bf_neg_cb(174).
		q(nt(165), (nt(174)));
		//       bf_body(165)         => bf_nleq_lower(145).
		q(nt(165), (nt(145)));
		//       bf_body(165)         => bf_nleq_upper(146).
		q(nt(165), (nt(146)));
		//       bf(11)               => bf_splitter(175).
		q(nt(11), (nt(175)));
		//       bf(11)               => bf_ref(176).
		q(nt(11), (nt(176)));
		//       bf(11)               => bf_all(177).
		q(nt(11), (nt(177)));
		//       bf(11)               => bf_ex(178).
		q(nt(11), (nt(178)));
		//       bf(11)               => bf_or(179).
		q(nt(11), (nt(179)));
		//       bf(11)               => bf_and(180).
		q(nt(11), (nt(180)));
		//       bf(11)               => bf_xor(181).
		q(nt(11), (nt(181)));
		//       bf(11)               => bf_neg(182).
		q(nt(11), (nt(182)));
		//       bf(11)               => bf_constant(183).
		q(nt(11), (nt(183)));
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
		//       bf_rec_relation(184) => bf_ref(176) _(9) bf_def(57) _(9) bf(11) _(9) dot(23).
		q(nt(184), (nt(176)+nt(9)+nt(57)+nt(9)+nt(11)+nt(9)+nt(23)));
		//       __E_bf_ref_30(185)   => offsets(80).
		q(nt(185), (nt(80)));
		//       __E_bf_ref_30(185)   => null.
		q(nt(185), (nul));
		//       bf_ref(176)          => sym(79) __E_bf_ref_30(185) bf_ref_args(186).
		q(nt(176), (nt(79)+nt(185)+nt(186)));
		//       __E_bf_ref_args_31(187) => _(9) bf_ref_arg(188).
		q(nt(187), (nt(9)+nt(188)));
		//       __E_bf_ref_args_32(189) => null.
		q(nt(189), (nul));
		//       __E_bf_ref_args_32(189) => __E_bf_ref_args_31(187) __E_bf_ref_args_32(189).
		q(nt(189), (nt(187)+nt(189)));
		//       bf_ref_args(186)     => open_parenthesis(24) __E_bf_ref_args_32(189) _(9) close_parenthesis(25).
		q(nt(186), (nt(24)+nt(189)+nt(9)+nt(25)));
		//       bf_ref_arg(188)      => bf(11).
		q(nt(188), (nt(11)));
		//       bf_ref_arg(188)      => capture(85).
		q(nt(188), (nt(85)));
		//       bf_ref_arg(188)      => variable(88).
		q(nt(188), (nt(88)));
		//       __E_bf_and_33(190)   => bf_and_sym(59) _(9).
		q(nt(190), (nt(59)+nt(9)));
		//       __E_bf_and_33(190)   => null.
		q(nt(190), (nul));
		//       bf_and(180)          => bf(11) _(9) __E_bf_and_33(190) bf(11).
		q(nt(180), (nt(11)+nt(9)+nt(190)+nt(11)));
		//       bf_or(179)           => bf(11) _(9) bf_or_sym(60) _(9) bf(11).
		q(nt(179), (nt(11)+nt(9)+nt(60)+nt(9)+nt(11)));
		//       bf_xor(181)          => bf(11) _(9) bf_xor_sym(61) _(9) bf(11).
		q(nt(181), (nt(11)+nt(9)+nt(61)+nt(9)+nt(11)));
		//       bf_neg(182)          => bf(11) _(9) bf_neg_sym(62).
		q(nt(182), (nt(11)+nt(9)+nt(62)));
		//       __E_bf_all_34(191)   => variable(88).
		q(nt(191), (nt(88)));
		//       __E_bf_all_34(191)   => capture(85).
		q(nt(191), (nt(85)));
		//       bf_all(177)          => bf_all_sym(69) __(10) __E_bf_all_34(191) __(10) bf(11).
		q(nt(177), (nt(69)+nt(10)+nt(191)+nt(10)+nt(11)));
		//       __E_bf_ex_35(192)    => variable(88).
		q(nt(192), (nt(88)));
		//       __E_bf_ex_35(192)    => capture(85).
		q(nt(192), (nt(85)));
		//       bf_ex(178)           => bf_ex_sym(70) __(10) __E_bf_ex_35(192) __(10) bf(11).
		q(nt(178), (nt(70)+nt(10)+nt(192)+nt(10)+nt(11)));
		//       bf_splitter(175)     => bf_splitter_sym(71) _(9) open_parenthesis(24) _(9) bf(11) _(9) close_parenthesis(25).
		q(nt(175), (nt(71)+nt(9)+nt(24)+nt(9)+nt(11)+nt(9)+nt(25)));
		//       bf_constant(183)     => open_brace(28) _(9) constant(193) _(9) close_brace(29).
		q(nt(183), (nt(28)+nt(9)+nt(193)+nt(9)+nt(29)));
		//       constant(193)        => binding(194).
		q(nt(193), (nt(194)));
		//       constant(193)        => capture(85).
		q(nt(193), (nt(85)));
		//       binding(194)         => source_binding(195).
		q(nt(194), (nt(195)));
		//       binding(194)         => named_binding(196).
		q(nt(194), (nt(196)));
		//       named_binding(196)   => chars(74).
		q(nt(196), (nt(74)));
		//       source_binding(195)  => type(197) _(9) colon(31) _(9) source(198).
		q(nt(195), (nt(197)+nt(9)+nt(31)+nt(9)+nt(198)));
		//       type(197)            => chars(74).
		q(nt(197), (nt(74)));
		//       type(197)            => null.
		q(nt(197), (nul));
		//       source0(199)         => alnum(6).
		q(nt(199), (nt(6)));
		//       source0(199)         => space(2).
		q(nt(199), (nt(2)));
		//       source0(199)         => punct(7).
		q(nt(199), (nt(7)));
		//       __E_source_36(200)   => source0(199).
		q(nt(200), (nt(199)));
		//       __E_source_37(201)   => __E_source_36(200).
		q(nt(201), (nt(200)));
		//       __E_source_37(201)   => __E_source_36(200) __E_source_37(201).
		q(nt(201), (nt(200)+nt(201)));
		//       source(198)          => __E_source_37(201).
		q(nt(198), (nt(201)));
		//       bf_and_cb(171)       => bf_and_cb_sym(202) __(10) bf_cb_arg(203) __(10) bf_cb_arg(203).
		q(nt(171), (nt(202)+nt(10)+nt(203)+nt(10)+nt(203)));
		//       bf_or_cb(172)        => bf_or_cb_sym(204) __(10) bf_cb_arg(203) __(10) bf_cb_arg(203).
		q(nt(172), (nt(204)+nt(10)+nt(203)+nt(10)+nt(203)));
		//       bf_xor_cb(173)       => bf_xor_cb_sym(205) __(10) bf_cb_arg(203) __(10) bf_cb_arg(203).
		q(nt(173), (nt(205)+nt(10)+nt(203)+nt(10)+nt(203)));
		//       bf_neg_cb(174)       => bf_neg_cb_sym(206) __(10) bf_cb_arg(203).
		q(nt(174), (nt(206)+nt(10)+nt(203)));
		//       bf_eq_cb(122)        => bf_eq_cb_sym(207) __(10) bf_cb_arg(203) __(10) wff_cb_arg(208) __(10) wff_cb_arg(208).
		q(nt(122), (nt(207)+nt(10)+nt(203)+nt(10)+nt(208)+nt(10)+nt(208)));
		//       bf_neq_cb(123)       => bf_neq_cb_sym(209) __(10) bf_cb_arg(203) __(10) wff_cb_arg(208) __(10) wff_cb_arg(208).
		q(nt(123), (nt(209)+nt(10)+nt(203)+nt(10)+nt(208)+nt(10)+nt(208)));
		//       bf_is_zero_cb(166)   => bf_is_zero_cb_sym(210) __(10) bf_cb_arg(203) __(10) bf_cb_arg(203).
		q(nt(166), (nt(210)+nt(10)+nt(203)+nt(10)+nt(203)));
		//       bf_is_one_cb(167)    => bf_is_one_cb_sym(211) __(10) bf_cb_arg(203) __(10) bf_cb_arg(203).
		q(nt(167), (nt(211)+nt(10)+nt(203)+nt(10)+nt(203)));
		//       bf_remove_funiversal_cb(169) => bf_remove_funiversal_cb_sym(212) __(10) bf_cb_arg(203) __(10) bf_cb_arg(203) __(10) bf_cb_arg(203) __(10) bf_cb_arg(203).
		q(nt(169), (nt(212)+nt(10)+nt(203)+nt(10)+nt(203)+nt(10)+nt(203)+nt(10)+nt(203)));
		//       bf_remove_fexistential_cb(170) => bf_remove_fexistential_cb_sym(213) __(10) bf_cb_arg(203) __(10) bf_cb_arg(203) __(10) bf_cb_arg(203) __(10) bf_cb_arg(203).
		q(nt(170), (nt(213)+nt(10)+nt(203)+nt(10)+nt(203)+nt(10)+nt(203)+nt(10)+nt(203)));
		//       wff_remove_existential_cb(126) => wff_remove_existential_cb_sym(214) __(10) wff_cb_arg(208) __(10) wff_cb_arg(208).
		q(nt(126), (nt(214)+nt(10)+nt(208)+nt(10)+nt(208)));
		//       wff_remove_bexistential_cb(127) => wff_remove_bexistential_cb_sym(215) __(10) wff_cb_arg(208) __(10) wff_cb_arg(208) __(10) wff_cb_arg(208) __(10) wff_cb_arg(208).
		q(nt(127), (nt(215)+nt(10)+nt(208)+nt(10)+nt(208)+nt(10)+nt(208)+nt(10)+nt(208)));
		//       wff_remove_buniversal_cb(128) => wff_remove_buniversal_cb_sym(216) __(10) wff_cb_arg(208) __(10) wff_cb_arg(208) __(10) wff_cb_arg(208) __(10) wff_cb_arg(208).
		q(nt(128), (nt(216)+nt(10)+nt(208)+nt(10)+nt(208)+nt(10)+nt(208)+nt(10)+nt(208)));
		//       wff_has_clashing_subformulas_cb(124) => wff_has_clashing_subformulas_cb_sym(217) __(10) wff_cb_arg(208) __(10) wff_cb_arg(208).
		q(nt(124), (nt(217)+nt(10)+nt(208)+nt(10)+nt(208)));
		//       bf_has_subformula_cb(168) => bf_has_subformula_cb_sym(218) __(10) bf_cb_arg(203) __(10) bf_cb_arg(203) __(10) bf_cb_arg(203).
		q(nt(168), (nt(218)+nt(10)+nt(203)+nt(10)+nt(203)+nt(10)+nt(203)));
		//       wff_has_subformula_cb(125) => wff_has_subformula_cb_sym(219) __(10) wff_cb_arg(208) __(10) wff_cb_arg(208) __(10) wff_cb_arg(208).
		q(nt(125), (nt(219)+nt(10)+nt(208)+nt(10)+nt(208)+nt(10)+nt(208)));
		//       bf_has_subformula_cb_sym(218) => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b'.
		q(nt(218), (t(30)+t(36)+t(32)+t(42)+t(26)+t(43)+t(32)+t(43)+t(44)+t(30)+t(36)+t(31)+t(45)+t(46)+t(44)+t(27)+t(26)+t(32)+t(47)+t(30)));
		//       wff_has_clashing_subformulas_cb_sym(217) => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b'.
		q(nt(217), (t(48)+t(36)+t(36)+t(32)+t(42)+t(26)+t(43)+t(32)+t(47)+t(27)+t(26)+t(43)+t(42)+t(41)+t(49)+t(50)+t(32)+t(43)+t(44)+t(30)+t(36)+t(31)+t(45)+t(46)+t(44)+t(27)+t(26)+t(43)+t(32)+t(47)+t(30)));
		//       wff_has_subformula_cb_sym(219) => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b'.
		q(nt(219), (t(48)+t(36)+t(36)+t(32)+t(42)+t(26)+t(43)+t(32)+t(43)+t(44)+t(30)+t(36)+t(31)+t(45)+t(46)+t(44)+t(27)+t(26)+t(32)+t(47)+t(30)));
		//       wff_remove_existential_cb_sym(214) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(214), (t(48)+t(36)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(28)+t(29)+t(41)+t(43)+t(52)+t(28)+t(49)+t(52)+t(41)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       wff_remove_bexistential_cb_sym(215) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(215), (t(48)+t(36)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(30)+t(28)+t(29)+t(41)+t(43)+t(52)+t(28)+t(49)+t(52)+t(41)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       wff_remove_buniversal_cb_sym(216) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b'.
		q(nt(216), (t(48)+t(36)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(30)+t(44)+t(49)+t(41)+t(51)+t(28)+t(45)+t(43)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       bf_remove_fexistential_cb_sym(213) => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(213), (t(30)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(36)+t(28)+t(29)+t(41)+t(43)+t(52)+t(28)+t(49)+t(52)+t(41)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       bf_remove_funiversal_cb_sym(212) => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b'.
		q(nt(212), (t(30)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(36)+t(44)+t(49)+t(41)+t(51)+t(28)+t(45)+t(43)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       bf_cb_arg(203)       => bf(11).
		q(nt(203), (nt(11)));
		//       wff_cb_arg(208)      => wff(13).
		q(nt(208), (nt(13)));
		//       bf_and_cb_sym(202)   => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b'.
		q(nt(202), (t(30)+t(36)+t(32)+t(26)+t(49)+t(53)+t(32)+t(47)+t(30)));
		//       bf_or_cb_sym(204)    => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b'.
		q(nt(204), (t(30)+t(36)+t(32)+t(31)+t(45)+t(32)+t(47)+t(30)));
		//       bf_xor_cb_sym(205)   => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b'.
		q(nt(205), (t(30)+t(36)+t(32)+t(29)+t(31)+t(45)+t(32)+t(47)+t(30)));
		//       bf_neg_cb_sym(206)   => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b'.
		q(nt(206), (t(30)+t(36)+t(32)+t(49)+t(28)+t(50)+t(32)+t(47)+t(30)));
		//       bf_eq_cb_sym(207)    => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b'.
		q(nt(207), (t(30)+t(36)+t(32)+t(28)+t(54)+t(32)+t(47)+t(30)));
		//       bf_neq_cb_sym(209)   => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b'.
		q(nt(209), (t(30)+t(36)+t(32)+t(49)+t(28)+t(54)+t(32)+t(47)+t(30)));
		//       bf_is_zero_cb_sym(210) => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b'.
		q(nt(210), (t(30)+t(36)+t(32)+t(41)+t(43)+t(32)+t(55)+t(28)+t(45)+t(31)+t(32)+t(47)+t(30)));
		//       bf_is_one_cb_sym(211) => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b'.
		q(nt(211), (t(30)+t(36)+t(32)+t(41)+t(43)+t(32)+t(31)+t(49)+t(28)+t(32)+t(47)+t(30)));
		//       __E_tau_collapse_positives_cb_38(220) => tau_collapse_positives_cb_sym(221) __(10) tau_cb_arg(222) __(10) tau_cb_arg(222) __(10) tau_cb_arg(222).
		q(nt(220), (nt(221)+nt(10)+nt(222)+nt(10)+nt(222)+nt(10)+nt(222)));
		//       tau_collapse_positives_cb(106) => __E_tau_collapse_positives_cb_38(220).
		q(nt(106), (nt(220)));
		//       __E_tau_collapse_positives_cb_39(223) => tau_collapse_positives_cb_sym(221) __(10) tau_cb_arg(222) __(10) tau_cb_arg(222).
		q(nt(223), (nt(221)+nt(10)+nt(222)+nt(10)+nt(222)));
		//       tau_collapse_positives_cb(106) => __E_tau_collapse_positives_cb_39(223).
		q(nt(106), (nt(223)));
		//       tau_positives_upwards_cb(107) => tau_positives_upwards_cb_sym(224) __(10) tau_cb_arg(222) __(10) tau_cb_arg(222).
		q(nt(107), (nt(224)+nt(10)+nt(222)+nt(10)+nt(222)));
		//       tau_cb_arg(222)      => capture(85).
		q(nt(222), (nt(85)));
		//       tau_cb_arg(222)      => tau(15).
		q(nt(222), (nt(15)));
		//       tau_collapse_positives_cb_sym(221) => 't' 'a' 'u' '_' 'c' 'o' 'l' 'l' 'a' 'p' 's' 'e' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'c' 'b'.
		q(nt(221), (t(52)+t(26)+t(44)+t(32)+t(47)+t(31)+t(27)+t(27)+t(26)+t(56)+t(43)+t(28)+t(32)+t(56)+t(31)+t(43)+t(41)+t(52)+t(41)+t(51)+t(28)+t(43)+t(32)+t(47)+t(30)));
		//       tau_positives_upwards_cb_sym(224) => 't' 'a' 'u' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'u' 'p' 'w' 'a' 'r' 'd' 's' '_' 'c' 'b'.
		q(nt(224), (t(52)+t(26)+t(44)+t(32)+t(56)+t(31)+t(43)+t(41)+t(52)+t(41)+t(51)+t(28)+t(43)+t(32)+t(44)+t(56)+t(48)+t(26)+t(45)+t(53)+t(43)+t(32)+t(47)+t(30)));
		//       __E_inputs_40(227)   => _(9) input(226).
		q(nt(227), (nt(9)+nt(226)));
		//       __E_inputs_41(228)   => null.
		q(nt(228), (nul));
		//       __E_inputs_41(228)   => __E_inputs_40(227) __E_inputs_41(228).
		q(nt(228), (nt(227)+nt(228)));
		//       inputs(225)          => _(9) less(33) _(9) input(226) __E_inputs_41(228) _(9) dot(23).
		q(nt(225), (nt(9)+nt(33)+nt(9)+nt(226)+nt(228)+nt(9)+nt(23)));
		//       input(226)           => in(93) _(9) colon(31) _(9) open_brace(28) _(9) source_binding(195) _(9) close_brace(29).
		q(nt(226), (nt(93)+nt(9)+nt(31)+nt(9)+nt(28)+nt(9)+nt(195)+nt(9)+nt(29)));
		//       builder(229)         => _(9) builder_head(230) _(9) builder_body(231) _(9) dot(23).
		q(nt(229), (nt(9)+nt(230)+nt(9)+nt(231)+nt(9)+nt(23)));
		//       __E_builder_head_42(232) => __(10) capture(85).
		q(nt(232), (nt(10)+nt(85)));
		//       __E_builder_head_43(233) => null.
		q(nt(233), (nul));
		//       __E_builder_head_43(233) => __E_builder_head_42(232) __E_builder_head_43(233).
		q(nt(233), (nt(232)+nt(233)));
		//       builder_head(230)    => open_parenthesis(24) _(9) capture(85) __E_builder_head_43(233) _(9) close_parenthesis(25).
		q(nt(230), (nt(24)+nt(9)+nt(85)+nt(233)+nt(9)+nt(25)));
		//       builder_body(231)    => bf_builder_body(234).
		q(nt(231), (nt(234)));
		//       builder_body(231)    => wff_builder_body(235).
		q(nt(231), (nt(235)));
		//       builder_body(231)    => tau_builder_body(236).
		q(nt(231), (nt(236)));
		//       bf_builder_body(234) => bf_builder_def(58) _(9) bf(11).
		q(nt(234), (nt(58)+nt(9)+nt(11)));
		//       wff_builder_body(235) => wff_builder_def(43) _(9) wff(13).
		q(nt(235), (nt(43)+nt(9)+nt(13)));
		//       tau_builder_body(236) => tau_builder_def(38) _(9) tau(15).
		q(nt(236), (nt(38)+nt(9)+nt(15)));
		//       library(237)         => rules(238).
		q(nt(237), (nt(238)));
		//       __E_rules_44(239)    => _(9) rule(240).
		q(nt(239), (nt(9)+nt(240)));
		//       __E_rules_45(241)    => null.
		q(nt(241), (nul));
		//       __E_rules_45(241)    => __E_rules_44(239) __E_rules_45(241).
		q(nt(241), (nt(239)+nt(241)));
		//       rules(238)           => __E_rules_45(241).
		q(nt(238), (nt(241)));
		//       rule(240)            => wff_rule(119).
		q(nt(240), (nt(119)));
		//       rule(240)            => bf_rule(163).
		q(nt(240), (nt(163)));
		//       rule(240)            => tau_rule(103).
		q(nt(240), (nt(103)));
		//       nso_rr(242)          => nso_rec_relations(243) nso_main(244).
		q(nt(242), (nt(243)+nt(244)));
		//       __E_nso_rec_relations_46(245) => _(9) nso_rec_relation(246).
		q(nt(245), (nt(9)+nt(246)));
		//       __E_nso_rec_relations_47(247) => null.
		q(nt(247), (nul));
		//       __E_nso_rec_relations_47(247) => __E_nso_rec_relations_46(245) __E_nso_rec_relations_47(247).
		q(nt(247), (nt(245)+nt(247)));
		//       nso_rec_relations(243) => __E_nso_rec_relations_47(247).
		q(nt(243), (nt(247)));
		//       nso_rec_relation(246) => bf_rec_relation(184).
		q(nt(246), (nt(184)));
		//       nso_rec_relation(246) => wff_rec_relation(129).
		q(nt(246), (nt(129)));
		//       nso_main(244)        => _(9) wff(13) _(9) dot(23).
		q(nt(244), (nt(9)+nt(13)+nt(9)+nt(23)));
		//       gssotc_rr(248)       => gssotc_rec_relations(249) gssotc_main(250).
		q(nt(248), (nt(249)+nt(250)));
		//       __E_gssotc_rec_relations_48(251) => _(9) gssotc_rec_relation(252).
		q(nt(251), (nt(9)+nt(252)));
		//       __E_gssotc_rec_relations_49(253) => null.
		q(nt(253), (nul));
		//       __E_gssotc_rec_relations_49(253) => __E_gssotc_rec_relations_48(251) __E_gssotc_rec_relations_49(253).
		q(nt(253), (nt(251)+nt(253)));
		//       gssotc_rec_relations(249) => __E_gssotc_rec_relations_49(253).
		q(nt(249), (nt(253)));
		//       gssotc_rec_relation(252) => tau_rec_relation(108).
		q(nt(252), (nt(108)));
		//       gssotc_rec_relation(252) => wff_rec_relation(129).
		q(nt(252), (nt(129)));
		//       gssotc_rec_relation(252) => bf_rec_relation(184).
		q(nt(252), (nt(184)));
		//       gssotc_main(250)     => _(9) tau(15) _(9) semicolon(32).
		q(nt(250), (nt(9)+nt(15)+nt(9)+nt(32)));
		//       __E_start_50(255)    => inputs(225).
		q(nt(255), (nt(225)));
		//       __E_start_50(255)    => builder(229).
		q(nt(255), (nt(229)));
		//       __E_start_50(255)    => library(237).
		q(nt(255), (nt(237)));
		//       __E_start_50(255)    => nso_rr(242).
		q(nt(255), (nt(242)));
		//       __E_start_50(255)    => gssotc_rr(248).
		q(nt(255), (nt(248)));
		//       start(254)           => __E_start_50(255) _(9).
		q(nt(254), (nt(255)+nt(9)));
		//       __E_cli_51(258)      => _(9) dot(23) _(9) cli_command(257).
		q(nt(258), (nt(9)+nt(23)+nt(9)+nt(257)));
		//       __E_cli_52(259)      => null.
		q(nt(259), (nul));
		//       __E_cli_52(259)      => __E_cli_51(258) __E_cli_52(259).
		q(nt(259), (nt(258)+nt(259)));
		//       cli(256)             => _(9) cli_command(257) __E_cli_52(259).
		q(nt(256), (nt(9)+nt(257)+nt(259)));
		//       cli_command(257)     => help_cmd(260).
		q(nt(257), (nt(260)));
		//       cli_command(257)     => version_cmd(261).
		q(nt(257), (nt(261)));
		//       cli_command(257)     => quit_cmd(262).
		q(nt(257), (nt(262)));
		//       cli_command(257)     => clear_cmd(263).
		q(nt(257), (nt(263)));
		//       cli_command(257)     => get_cmd(264).
		q(nt(257), (nt(264)));
		//       cli_command(257)     => set_cmd(265).
		q(nt(257), (nt(265)));
		//       cli_command(257)     => toggle_cmd(266).
		q(nt(257), (nt(266)));
		//       cli_command(257)     => list_outputs_cmd(267).
		q(nt(257), (nt(267)));
		//       cli_command(257)     => clear_outputs_cmd(268).
		q(nt(257), (nt(268)));
		//       cli_command(257)     => print_output_cmd(269).
		q(nt(257), (nt(269)));
		//       cli_command(257)     => file(270).
		q(nt(257), (nt(270)));
		//       cli_command(257)     => normalize_cmd(271).
		q(nt(257), (nt(271)));
		//       cli_command(257)     => execute_cmd(272).
		q(nt(257), (nt(272)));
		//       cli_command(257)     => solve_cmd(273).
		q(nt(257), (nt(273)));
		//       cli_command(257)     => is_satisfiable_cmd(274).
		q(nt(257), (nt(274)));
		//       cli_command(257)     => is_valid_cmd(275).
		q(nt(257), (nt(275)));
		//       cli_command(257)     => is_unsatisfiable_cmd(276).
		q(nt(257), (nt(276)));
		//       cli_command(257)     => bf_dnf_cmd(277).
		q(nt(257), (nt(277)));
		//       cli_command(257)     => bf_cnf_cmd(278).
		q(nt(257), (nt(278)));
		//       cli_command(257)     => bf_anf_cmd(279).
		q(nt(257), (nt(279)));
		//       cli_command(257)     => bf_nnf_cmd(280).
		q(nt(257), (nt(280)));
		//       cli_command(257)     => bf_pnf_cmd(281).
		q(nt(257), (nt(281)));
		//       cli_command(257)     => bf_mnf_cmd(282).
		q(nt(257), (nt(282)));
		//       cli_command(257)     => wff_onf_cmd(283).
		q(nt(257), (nt(283)));
		//       cli_command(257)     => wff_dnf_cmd(284).
		q(nt(257), (nt(284)));
		//       cli_command(257)     => wff_cnf_cmd(285).
		q(nt(257), (nt(285)));
		//       cli_command(257)     => wff_anf_cmd(286).
		q(nt(257), (nt(286)));
		//       cli_command(257)     => wff_nnf_cmd(287).
		q(nt(257), (nt(287)));
		//       cli_command(257)     => wff_pnf_cmd(288).
		q(nt(257), (nt(288)));
		//       cli_command(257)     => wff_mnf_cmd(289).
		q(nt(257), (nt(289)));
		//       cli_command(257)     => bf_substitute_cmd(290).
		q(nt(257), (nt(290)));
		//       cli_command(257)     => wff_substitute_cmd(291).
		q(nt(257), (nt(291)));
		//       cli_command(257)     => bf_instantiate_cmd(292).
		q(nt(257), (nt(292)));
		//       cli_command(257)     => wff_instantiate_cmd(293).
		q(nt(257), (nt(293)));
		//       cli_command(257)     => def_rule_cmd(294).
		q(nt(257), (nt(294)));
		//       cli_command(257)     => def_list_cmd(295).
		q(nt(257), (nt(295)));
		//       cli_command(257)     => def_del_cmd(296).
		q(nt(257), (nt(296)));
		//       cli_command(257)     => def_clear_cmd(297).
		q(nt(257), (nt(297)));
		//       cli_command(257)     => bf(11).
		q(nt(257), (nt(11)));
		//       cli_command(257)     => wff(13).
		q(nt(257), (nt(13)));
		//       cli_command(257)     => nso_rr(242).
		q(nt(257), (nt(242)));
		//       normalize_cmd(271)   => normalize_cmd_sym(298) __(10) normalize_cmd_arg(299).
		q(nt(271), (nt(298)+nt(10)+nt(299)));
		//       normalize_cmd_arg(299) => wff(13).
		q(nt(299), (nt(13)));
		//       normalize_cmd_arg(299) => nso_rr(242).
		q(nt(299), (nt(242)));
		//       normalize_cmd_arg(299) => output(300).
		q(nt(299), (nt(300)));
		//       normalize_cmd_arg(299) => wff_selection(301).
		q(nt(299), (nt(301)));
		//       solve_cmd(273)       => solve_cmd_sym(302) __(10) solve_cmd_arg(303).
		q(nt(273), (nt(302)+nt(10)+nt(303)));
		//       solve_cmd_arg(303)   => wff(13).
		q(nt(303), (nt(13)));
		//       solve_cmd_arg(303)   => output(300).
		q(nt(303), (nt(300)));
		//       solve_cmd_arg(303)   => wff_selection(301).
		q(nt(303), (nt(301)));
		//       execute_cmd(272)     => execute_cmd_sym(304) __(10) execute_cmd_arg(305).
		q(nt(272), (nt(304)+nt(10)+nt(305)));
		//       execute_cmd_arg(305) => tau(15).
		q(nt(305), (nt(15)));
		//       execute_cmd_arg(305) => gssotc_rr(248).
		q(nt(305), (nt(248)));
		//       is_satisfiable_cmd(274) => is_satisfiable_cmd_sym(306) __(10) is_satisfiable_cmd_arg(307).
		q(nt(274), (nt(306)+nt(10)+nt(307)));
		//       is_satisfiable_cmd_arg(307) => gssotc_rr(248).
		q(nt(307), (nt(248)));
		//       is_valid_cmd(275)    => is_valid_cmd_sym(308) __(10) is_valid_cmd_arg(309).
		q(nt(275), (nt(308)+nt(10)+nt(309)));
		//       is_valid_cmd_arg(309) => gssotc_rr(248).
		q(nt(309), (nt(248)));
		//       is_unsatisfiable_cmd(276) => is_unsatisfiable_cmd_sym(310) __(10) is_unsatisfiable_cmd_arg(311).
		q(nt(276), (nt(310)+nt(10)+nt(311)));
		//       is_unsatisfiable_cmd_arg(311) => gssotc_rr(248).
		q(nt(311), (nt(248)));
		//       file(270)            => file_sym(312) quote(36) file_path(313) quote(36).
		q(nt(270), (nt(312)+nt(36)+nt(313)+nt(36)));
		//       __E_bf_instantiate_cmd_53(315) => bf(11).
		q(nt(315), (nt(11)));
		//       __E_bf_instantiate_cmd_53(315) => bf_selection(316).
		q(nt(315), (nt(316)));
		//       __E_bf_instantiate_cmd_53(315) => output(300).
		q(nt(315), (nt(300)));
		//       __E_bf_instantiate_cmd_54(317) => bf_selection(316).
		q(nt(317), (nt(316)));
		//       __E_bf_instantiate_cmd_54(317) => output(300).
		q(nt(317), (nt(300)));
		//       bf_instantiate_cmd(292) => instantiate_cmd_sym(314) __(10) variable(88) __(10) __E_bf_instantiate_cmd_53(315) __(10) __E_bf_instantiate_cmd_54(317).
		q(nt(292), (nt(314)+nt(10)+nt(88)+nt(10)+nt(315)+nt(10)+nt(317)));
		//       __E_wff_instantiate_cmd_55(318) => wff(13).
		q(nt(318), (nt(13)));
		//       __E_wff_instantiate_cmd_55(318) => wff_selection(301).
		q(nt(318), (nt(301)));
		//       __E_wff_instantiate_cmd_55(318) => output(300).
		q(nt(318), (nt(300)));
		//       __E_wff_instantiate_cmd_56(319) => wff_selection(301).
		q(nt(319), (nt(301)));
		//       __E_wff_instantiate_cmd_56(319) => output(300).
		q(nt(319), (nt(300)));
		//       wff_instantiate_cmd(293) => instantiate_cmd_sym(314) __(10) variable(88) __(10) __E_wff_instantiate_cmd_55(318) __(10) __E_wff_instantiate_cmd_56(319).
		q(nt(293), (nt(314)+nt(10)+nt(88)+nt(10)+nt(318)+nt(10)+nt(319)));
		//       __E_bf_substitute_cmd_57(321) => bf(11).
		q(nt(321), (nt(11)));
		//       __E_bf_substitute_cmd_57(321) => output(300).
		q(nt(321), (nt(300)));
		//       __E_bf_substitute_cmd_57(321) => bf_selection(316).
		q(nt(321), (nt(316)));
		//       __E_bf_substitute_cmd_58(322) => output(300).
		q(nt(322), (nt(300)));
		//       __E_bf_substitute_cmd_58(322) => bf_selection(316).
		q(nt(322), (nt(316)));
		//       __E_bf_substitute_cmd_58(322) => bf_selection(316).
		q(nt(322), (nt(316)));
		//       __E_bf_substitute_cmd_59(323) => bf_selection(316).
		q(nt(323), (nt(316)));
		//       __E_bf_substitute_cmd_59(323) => output(300).
		q(nt(323), (nt(300)));
		//       bf_substitute_cmd(290) => substitute_cmd_sym(320) __(10) __E_bf_substitute_cmd_57(321) __(10) __E_bf_substitute_cmd_58(322) __E_bf_substitute_cmd_59(323).
		q(nt(290), (nt(320)+nt(10)+nt(321)+nt(10)+nt(322)+nt(323)));
		//       __E_wff_substitute_cmd_60(325) => wff(13).
		q(nt(325), (nt(13)));
		//       __E_wff_substitute_cmd_60(325) => output(300).
		q(nt(325), (nt(300)));
		//       __E_wff_substitute_cmd_60(325) => wff_selection(301).
		q(nt(325), (nt(301)));
		//       __E_wff_substitute_cmd_61(326) => output(300).
		q(nt(326), (nt(300)));
		//       __E_wff_substitute_cmd_61(326) => wff_selection(301).
		q(nt(326), (nt(301)));
		//       __E_wff_substitute_cmd_62(327) => wff_selection(301).
		q(nt(327), (nt(301)));
		//       __E_wff_substitute_cmd_62(327) => output(300).
		q(nt(327), (nt(300)));
		//       wff_substitute_cmd(291) => substitutecmd__sym(324) __(10) __E_wff_substitute_cmd_60(325) __(10) __E_wff_substitute_cmd_61(326) __E_wff_substitute_cmd_62(327).
		q(nt(291), (nt(324)+nt(10)+nt(325)+nt(10)+nt(326)+nt(327)));
		//       __E_bf_dnf_cmd_63(329) => bf(11).
		q(nt(329), (nt(11)));
		//       __E_bf_dnf_cmd_63(329) => output(300).
		q(nt(329), (nt(300)));
		//       __E_bf_dnf_cmd_63(329) => bf_selection(316).
		q(nt(329), (nt(316)));
		//       bf_dnf_cmd(277)      => dnf_cmd_sym(328) __(10) __E_bf_dnf_cmd_63(329).
		q(nt(277), (nt(328)+nt(10)+nt(329)));
		//       __E_bf_cnf_cmd_64(331) => bf(11).
		q(nt(331), (nt(11)));
		//       __E_bf_cnf_cmd_64(331) => output(300).
		q(nt(331), (nt(300)));
		//       __E_bf_cnf_cmd_64(331) => bf_selection(316).
		q(nt(331), (nt(316)));
		//       bf_cnf_cmd(278)      => cnf_cmd_sym(330) __(10) __E_bf_cnf_cmd_64(331).
		q(nt(278), (nt(330)+nt(10)+nt(331)));
		//       __E_bf_anf_cmd_65(333) => bf(11).
		q(nt(333), (nt(11)));
		//       __E_bf_anf_cmd_65(333) => output(300).
		q(nt(333), (nt(300)));
		//       __E_bf_anf_cmd_65(333) => bf_selection(316).
		q(nt(333), (nt(316)));
		//       bf_anf_cmd(279)      => anf_cmd_sym(332) __(10) __E_bf_anf_cmd_65(333).
		q(nt(279), (nt(332)+nt(10)+nt(333)));
		//       __E_bf_nnf_cmd_66(335) => bf(11).
		q(nt(335), (nt(11)));
		//       __E_bf_nnf_cmd_66(335) => output(300).
		q(nt(335), (nt(300)));
		//       __E_bf_nnf_cmd_66(335) => bf_selection(316).
		q(nt(335), (nt(316)));
		//       bf_nnf_cmd(280)      => nnf_cmd_sym(334) __(10) __E_bf_nnf_cmd_66(335).
		q(nt(280), (nt(334)+nt(10)+nt(335)));
		//       __E_bf_pnf_cmd_67(337) => bf(11).
		q(nt(337), (nt(11)));
		//       __E_bf_pnf_cmd_67(337) => output(300).
		q(nt(337), (nt(300)));
		//       __E_bf_pnf_cmd_67(337) => bf_selection(316).
		q(nt(337), (nt(316)));
		//       bf_pnf_cmd(281)      => pnf_cmd_sym(336) __(10) __E_bf_pnf_cmd_67(337).
		q(nt(281), (nt(336)+nt(10)+nt(337)));
		//       __E_bf_mnf_cmd_68(339) => bf(11).
		q(nt(339), (nt(11)));
		//       __E_bf_mnf_cmd_68(339) => output(300).
		q(nt(339), (nt(300)));
		//       __E_bf_mnf_cmd_68(339) => bf_selection(316).
		q(nt(339), (nt(316)));
		//       bf_mnf_cmd(282)      => mnf_cmd_sym(338) __(10) __E_bf_mnf_cmd_68(339).
		q(nt(282), (nt(338)+nt(10)+nt(339)));
		//       __E_wff_onf_cmd_69(341) => wff(13).
		q(nt(341), (nt(13)));
		//       __E_wff_onf_cmd_69(341) => output(300).
		q(nt(341), (nt(300)));
		//       __E_wff_onf_cmd_69(341) => wff_selection(301).
		q(nt(341), (nt(301)));
		//       wff_onf_cmd(283)     => onf_cmd_sym(340) __(10) variable(88) __(10) __E_wff_onf_cmd_69(341).
		q(nt(283), (nt(340)+nt(10)+nt(88)+nt(10)+nt(341)));
		//       __E_wff_dnf_cmd_70(342) => wff(13).
		q(nt(342), (nt(13)));
		//       __E_wff_dnf_cmd_70(342) => output(300).
		q(nt(342), (nt(300)));
		//       __E_wff_dnf_cmd_70(342) => wff_selection(301).
		q(nt(342), (nt(301)));
		//       wff_dnf_cmd(284)     => dnf_cmd_sym(328) __(10) __E_wff_dnf_cmd_70(342).
		q(nt(284), (nt(328)+nt(10)+nt(342)));
		//       __E_wff_cnf_cmd_71(343) => wff(13).
		q(nt(343), (nt(13)));
		//       __E_wff_cnf_cmd_71(343) => output(300).
		q(nt(343), (nt(300)));
		//       __E_wff_cnf_cmd_71(343) => wff_selection(301).
		q(nt(343), (nt(301)));
		//       wff_cnf_cmd(285)     => cnf_cmd_sym(330) __(10) __E_wff_cnf_cmd_71(343).
		q(nt(285), (nt(330)+nt(10)+nt(343)));
		//       __E_wff_anf_cmd_72(344) => wff(13).
		q(nt(344), (nt(13)));
		//       __E_wff_anf_cmd_72(344) => output(300).
		q(nt(344), (nt(300)));
		//       __E_wff_anf_cmd_72(344) => wff_selection(301).
		q(nt(344), (nt(301)));
		//       wff_anf_cmd(286)     => anf_cmd_sym(332) __(10) __E_wff_anf_cmd_72(344).
		q(nt(286), (nt(332)+nt(10)+nt(344)));
		//       __E_wff_nnf_cmd_73(345) => wff(13).
		q(nt(345), (nt(13)));
		//       __E_wff_nnf_cmd_73(345) => output(300).
		q(nt(345), (nt(300)));
		//       __E_wff_nnf_cmd_73(345) => wff_selection(301).
		q(nt(345), (nt(301)));
		//       wff_nnf_cmd(287)     => nnf_cmd_sym(334) __(10) __E_wff_nnf_cmd_73(345).
		q(nt(287), (nt(334)+nt(10)+nt(345)));
		//       __E_wff_pnf_cmd_74(346) => wff(13).
		q(nt(346), (nt(13)));
		//       __E_wff_pnf_cmd_74(346) => output(300).
		q(nt(346), (nt(300)));
		//       __E_wff_pnf_cmd_74(346) => wff_selection(301).
		q(nt(346), (nt(301)));
		//       wff_pnf_cmd(288)     => pnf_cmd_sym(336) __(10) __E_wff_pnf_cmd_74(346).
		q(nt(288), (nt(336)+nt(10)+nt(346)));
		//       __E_wff_mnf_cmd_75(347) => wff(13).
		q(nt(347), (nt(13)));
		//       __E_wff_mnf_cmd_75(347) => output(300).
		q(nt(347), (nt(300)));
		//       __E_wff_mnf_cmd_75(347) => wff_selection(301).
		q(nt(347), (nt(301)));
		//       wff_mnf_cmd(289)     => mnf_cmd_sym(338) __(10) __E_wff_mnf_cmd_75(347).
		q(nt(289), (nt(338)+nt(10)+nt(347)));
		//       def_rule_cmd(294)    => def_cmd_sym(348) __(10) def_rule_cmd_arg(349).
		q(nt(294), (nt(348)+nt(10)+nt(349)));
		//       __E_def_rule_cmd_arg_76(350) => bf_rec_relation(184).
		q(nt(350), (nt(184)));
		//       __E_def_rule_cmd_arg_76(350) => wff_rec_relation(129).
		q(nt(350), (nt(129)));
		//       __E_def_rule_cmd_arg_76(350) => tau_rec_relation(108).
		q(nt(350), (nt(108)));
		//       def_rule_cmd_arg(349) => __E_def_rule_cmd_arg_76(350).
		q(nt(349), (nt(350)));
		//       def_list_cmd(295)    => def_cmd_sym(348) __(10) def_cmd_list(351).
		q(nt(295), (nt(348)+nt(10)+nt(351)));
		//       def_del_cmd(296)     => def_cmd_sym(348) __(10) def_cmd_del(352) __(10) digits(77).
		q(nt(296), (nt(348)+nt(10)+nt(352)+nt(10)+nt(77)));
		//       def_clear_cmd(297)   => def_cmd_sym(348) __(10) def_cmf_clear(353).
		q(nt(297), (nt(348)+nt(10)+nt(353)));
		//       __E_help_cmd_77(355) => __(10) cli_cmd_sym(356).
		q(nt(355), (nt(10)+nt(356)));
		//       __E_help_cmd_77(355) => null.
		q(nt(355), (nul));
		//       help_cmd(260)        => help_cmd_sym(354) __E_help_cmd_77(355).
		q(nt(260), (nt(354)+nt(355)));
		//       version_cmd(261)     => version_cmd_sym(357).
		q(nt(261), (nt(357)));
		//       quit_cmd(262)        => quit_cmd_sym(358).
		q(nt(262), (nt(358)));
		//       clear_cmd(263)       => clear_cmd_sym(359).
		q(nt(263), (nt(359)));
		//       wff_selection(301)   => selection_sym(360) __(10) digits(77).
		q(nt(301), (nt(360)+nt(10)+nt(77)));
		//       bf_selection(316)    => selection_sym(360) __(10) digits(77).
		q(nt(316), (nt(360)+nt(10)+nt(77)));
		//       bf_var_selection(361) => selection_sym(360) __(10) digits(77).
		q(nt(361), (nt(360)+nt(10)+nt(77)));
		//       wff_var_selection(362) => selection_sym(360) __(10) digits(77).
		q(nt(362), (nt(360)+nt(10)+nt(77)));
		//       file_path(313)       => chars(74).
		q(nt(313), (nt(74)));
		//       output(300)          => absolute_output(363).
		q(nt(300), (nt(363)));
		//       output(300)          => relative_output(364).
		q(nt(300), (nt(364)));
		//       __E_absolute_output_78(365) => absolute_output_sym(366).
		q(nt(365), (nt(366)));
		//       __E_absolute_output_78(365) => output_sym(367) __(10).
		q(nt(365), (nt(367)+nt(10)));
		//       absolute_output(363) => __E_absolute_output_78(365) output_id(368).
		q(nt(363), (nt(365)+nt(368)));
		//       relative_output(364) => relative_output_sym(369) output_id(368).
		q(nt(364), (nt(369)+nt(368)));
		//       output_id(368)       => digits(77).
		q(nt(368), (nt(77)));
		//       list_outputs_cmd(267) => output_sym(367).
		q(nt(267), (nt(367)));
		//       list_outputs_cmd(267) => absolute_output_sym(366).
		q(nt(267), (nt(366)));
		//       list_outputs_cmd(267) => relative_output_sym(369).
		q(nt(267), (nt(369)));
		//       __E_clear_outputs_cmd_79(370) => output_sym(367) __(10).
		q(nt(370), (nt(367)+nt(10)));
		//       __E_clear_outputs_cmd_79(370) => absolute_output_sym(366).
		q(nt(370), (nt(366)));
		//       __E_clear_outputs_cmd_79(370) => relative_output_sym(369).
		q(nt(370), (nt(369)));
		//       clear_outputs_cmd(268) => __E_clear_outputs_cmd_79(370) clear_sym(371).
		q(nt(268), (nt(370)+nt(371)));
		//       print_output_cmd(269) => output(300).
		q(nt(269), (nt(300)));
		//       cli_cmd_sym(356)     => help_cmd_sym(354).
		q(nt(356), (nt(354)));
		//       cli_cmd_sym(356)     => version_cmd_sym(357).
		q(nt(356), (nt(357)));
		//       cli_cmd_sym(356)     => quit_cmd_sym(358).
		q(nt(356), (nt(358)));
		//       cli_cmd_sym(356)     => clear_cmd_sym(359).
		q(nt(356), (nt(359)));
		//       cli_cmd_sym(356)     => output_sym(367).
		q(nt(356), (nt(367)));
		//       cli_cmd_sym(356)     => absolute_output_sym(366).
		q(nt(356), (nt(366)));
		//       cli_cmd_sym(356)     => relative_output_sym(369).
		q(nt(356), (nt(369)));
		//       cli_cmd_sym(356)     => selection_sym(360).
		q(nt(356), (nt(360)));
		//       cli_cmd_sym(356)     => instantiate_cmd_sym(314).
		q(nt(356), (nt(314)));
		//       cli_cmd_sym(356)     => substitute_cmd_sym(320).
		q(nt(356), (nt(320)));
		//       cli_cmd_sym(356)     => def_cmd_sym(348).
		q(nt(356), (nt(348)));
		//       cli_cmd_sym(356)     => normalize_cmd_sym(298).
		q(nt(356), (nt(298)));
		//       cli_cmd_sym(356)     => execute_cmd_sym(304).
		q(nt(356), (nt(304)));
		//       cli_cmd_sym(356)     => file_cmd_sym(372).
		q(nt(356), (nt(372)));
		//       cli_cmd_sym(356)     => solve_cmd_sym(302).
		q(nt(356), (nt(302)));
		//       cli_cmd_sym(356)     => is_satisfiable_cmd_sym(306).
		q(nt(356), (nt(306)));
		//       cli_cmd_sym(356)     => is_valid_cmd_sym(308).
		q(nt(356), (nt(308)));
		//       cli_cmd_sym(356)     => is_unsatisfiable_cmd_sym(310).
		q(nt(356), (nt(310)));
		//       cli_cmd_sym(356)     => examples_sym(373).
		q(nt(356), (nt(373)));
		//       cli_cmd_sym(356)     => get_cmd_sym(374).
		q(nt(356), (nt(374)));
		//       cli_cmd_sym(356)     => set_cmd_sym(375).
		q(nt(356), (nt(375)));
		//       cli_cmd_sym(356)     => toggle_cmd_sym(376).
		q(nt(356), (nt(376)));
		//       cli_cmd_sym(356)     => cnf_cmd_sym(330).
		q(nt(356), (nt(330)));
		//       cli_cmd_sym(356)     => dnf_cmd_sym(328).
		q(nt(356), (nt(328)));
		//       cli_cmd_sym(356)     => nnf_cmd_sym(334).
		q(nt(356), (nt(334)));
		//       cli_cmd_sym(356)     => mnf_cmd_sym(338).
		q(nt(356), (nt(338)));
		//       cli_cmd_sym(356)     => onf_cmd_sym(340).
		q(nt(356), (nt(340)));
		//       help_cmd_sym(354)    => 'h'.
		q(nt(354), (t(42)));
		//       help_cmd_sym(354)    => 'h' 'e' 'l' 'p'.
		q(nt(354), (t(42)+t(28)+t(27)+t(56)));
		//       version_cmd_sym(357) => 'v'.
		q(nt(357), (t(51)));
		//       version_cmd_sym(357) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
		q(nt(357), (t(51)+t(28)+t(45)+t(43)+t(41)+t(31)+t(49)));
		//       quit_cmd_sym(358)    => 'q'.
		q(nt(358), (t(54)));
		//       quit_cmd_sym(358)    => 'e'.
		q(nt(358), (t(28)));
		//       quit_cmd_sym(358)    => 'q' 'u' 'i' 't'.
		q(nt(358), (t(54)+t(44)+t(41)+t(52)));
		//       quit_cmd_sym(358)    => 'e' 'x' 'i' 't'.
		q(nt(358), (t(28)+t(29)+t(41)+t(52)));
		//       instantiate_cmd_sym(314) => 'i'.
		q(nt(314), (t(41)));
		//       instantiate_cmd_sym(314) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
		q(nt(314), (t(41)+t(49)+t(43)+t(52)+t(26)+t(49)+t(52)+t(41)+t(26)+t(52)+t(28)));
		//       substitute_cmd_sym(320) => 's'.
		q(nt(320), (t(43)));
		//       substitute_cmd_sym(320) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
		q(nt(320), (t(43)+t(44)+t(30)+t(43)+t(52)+t(41)+t(52)+t(44)+t(52)+t(28)));
		//       file_cmd_sym(372)    => 'r'.
		q(nt(372), (t(45)));
		//       file_cmd_sym(372)    => 'r' 'e' 'a' 'd'.
		q(nt(372), (t(45)+t(28)+t(26)+t(53)));
		//       clear_cmd_sym(359)   => 'c'.
		q(nt(359), (t(47)));
		//       clear_cmd_sym(359)   => 'c' 'l' 'e' 'a' 'r'.
		q(nt(359), (t(47)+t(27)+t(28)+t(26)+t(45)));
		//       normalize_cmd_sym(298) => 'n'.
		q(nt(298), (t(49)));
		//       normalize_cmd_sym(298) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
		q(nt(298), (t(49)+t(31)+t(45)+t(46)+t(26)+t(27)+t(41)+t(55)+t(28)));
		//       execute_cmd_sym(304) => 'e' 'x' 'e' 'c' 'u' 't' 'e'.
		q(nt(304), (t(28)+t(29)+t(28)+t(47)+t(44)+t(52)+t(28)));
		//       solve_cmd_sym(302)   => 's' 'o' 'l' 'v' 'e'.
		q(nt(302), (t(43)+t(31)+t(27)+t(51)+t(28)));
		//       is_satisfiable_cmd_sym(306) => 's' 'a' 't'.
		q(nt(306), (t(43)+t(26)+t(52)));
		//       is_valid_cmd_sym(308) => 'v' 'a' 'l' 'i' 'd'.
		q(nt(308), (t(51)+t(26)+t(27)+t(41)+t(53)));
		//       is_unsatisfiable_cmd_sym(310) => 'u' 'n' 's' 'a' 't'.
		q(nt(310), (t(44)+t(49)+t(43)+t(26)+t(52)));
		//       onf_cmd_sym(340)     => 'o' 'n' 'f'.
		q(nt(340), (t(31)+t(49)+t(36)));
		//       dnf_cmd_sym(328)     => 'd' 'n' 'f'.
		q(nt(328), (t(53)+t(49)+t(36)));
		//       cnf_cmd_sym(330)     => 'c' 'n' 'f'.
		q(nt(330), (t(47)+t(49)+t(36)));
		//       anf_cmd_sym(332)     => 'a' 'n' 'f'.
		q(nt(332), (t(26)+t(49)+t(36)));
		//       nnf_cmd_sym(334)     => 'n' 'n' 'f'.
		q(nt(334), (t(49)+t(49)+t(36)));
		//       pnf_cmd_sym(336)     => 'p' 'n' 'f'.
		q(nt(336), (t(56)+t(49)+t(36)));
		//       mnf_cmd_sym(338)     => 'm' 'n' 'f'.
		q(nt(338), (t(46)+t(49)+t(36)));
		//       def_cmd_sym(348)     => 'd' 'e' 'f'.
		q(nt(348), (t(53)+t(28)+t(36)));
		//       def_cmd_list(351)    => 'l' 'i' 's' 't'.
		q(nt(351), (t(27)+t(41)+t(43)+t(52)));
		//       def_cmd_del(352)     => 'd' 'e' 'l'.
		q(nt(352), (t(53)+t(28)+t(27)));
		//       def_cmf_clear(353)   => 'c' 'l' 'e' 'a' 'r'.
		q(nt(353), (t(47)+t(27)+t(28)+t(26)+t(45)));
		//       selection_sym(360)   => 's'.
		q(nt(360), (t(43)));
		//       selection_sym(360)   => 's' 'e' 'l' 'e' 'c' 't' 'i' 'o' 'n'.
		q(nt(360), (t(43)+t(28)+t(27)+t(28)+t(47)+t(52)+t(41)+t(31)+t(49)));
		//       output_sym(367)      => 'o'.
		q(nt(367), (t(31)));
		//       output_sym(367)      => 'o' 'u' 't' 'p' 'u' 't'.
		q(nt(367), (t(31)+t(44)+t(52)+t(56)+t(44)+t(52)));
		//       absolute_output_sym(366) => '&'.
		q(nt(366), (t(20)));
		//       relative_output_sym(369) => '%'.
		q(nt(369), (t(57)));
		//       examples_sym(373)    => 'e' 'x' 'a' 'm' 'p' 'l' 'e' 's'.
		q(nt(373), (t(28)+t(29)+t(26)+t(46)+t(56)+t(27)+t(28)+t(43)));
		//       __E_get_cmd_80(377)  => __(10) option(378).
		q(nt(377), (nt(10)+nt(378)));
		//       __E_get_cmd_80(377)  => null.
		q(nt(377), (nul));
		//       get_cmd(264)         => get_cmd_sym(374) __E_get_cmd_80(377).
		q(nt(264), (nt(374)+nt(377)));
		//       __E_set_cmd_81(379)  => __(10).
		q(nt(379), (nt(10)));
		//       __E_set_cmd_81(379)  => _(9) '=' _(9).
		q(nt(379), (nt(9)+t(19)+nt(9)));
		//       set_cmd(265)         => set_cmd_sym(375) __(10) option(378) __E_set_cmd_81(379) option_value(380).
		q(nt(265), (nt(375)+nt(10)+nt(378)+nt(379)+nt(380)));
		//       toggle_cmd(266)      => toggle_cmd_sym(376) __(10) bool_option(381).
		q(nt(266), (nt(376)+nt(10)+nt(381)));
		//       get_cmd_sym(374)     => 'g' 'e' 't'.
		q(nt(374), (t(50)+t(28)+t(52)));
		//       set_cmd_sym(375)     => 's' 'e' 't'.
		q(nt(375), (t(43)+t(28)+t(52)));
		//       toggle_cmd_sym(376)  => 't' 'o' 'g' 'g' 'l' 'e'.
		q(nt(376), (t(52)+t(31)+t(50)+t(50)+t(27)+t(28)));
		//       option(378)          => bool_option(381).
		q(nt(378), (nt(381)));
		//       option(378)          => severity_opt(382).
		q(nt(378), (nt(382)));
		//       bool_option(381)     => status_opt(383).
		q(nt(381), (nt(383)));
		//       bool_option(381)     => colors_opt(384).
		q(nt(381), (nt(384)));
		//       bool_option(381)     => debug_repl_opt(385).
		q(nt(381), (nt(385)));
		//       status_opt(383)      => 's'.
		q(nt(383), (t(43)));
		//       status_opt(383)      => 's' 't' 'a' 't' 'u' 's'.
		q(nt(383), (t(43)+t(52)+t(26)+t(52)+t(44)+t(43)));
		//       colors_opt(384)      => 'c'.
		q(nt(384), (t(47)));
		//       colors_opt(384)      => 'c' 'o' 'l' 'o' 'r'.
		q(nt(384), (t(47)+t(31)+t(27)+t(31)+t(45)));
		//       colors_opt(384)      => 'c' 'o' 'l' 'o' 'r' 's'.
		q(nt(384), (t(47)+t(31)+t(27)+t(31)+t(45)+t(43)));
		//       severity_opt(382)    => 's' 'e' 'v'.
		q(nt(382), (t(43)+t(28)+t(51)));
		//       severity_opt(382)    => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
		q(nt(382), (t(43)+t(28)+t(51)+t(28)+t(45)+t(41)+t(52)+t(58)));
		//       debug_repl_opt(385)  => 'd'.
		q(nt(385), (t(53)));
		//       debug_repl_opt(385)  => 'd' 'r'.
		q(nt(385), (t(53)+t(45)));
		//       debug_repl_opt(385)  => 'd' 'b' 'g'.
		q(nt(385), (t(53)+t(30)+t(50)));
		//       debug_repl_opt(385)  => 'd' 'e' 'b' 'u' 'g'.
		q(nt(385), (t(53)+t(28)+t(30)+t(44)+t(50)));
		//       debug_repl_opt(385)  => 'd' 'e' 'b' 'u' 'g' '-' 'r' 'e' 'p' 'l'.
		q(nt(385), (t(53)+t(28)+t(30)+t(44)+t(50)+t(12)+t(45)+t(28)+t(56)+t(27)));
		//       option_value(380)    => option_value_true(386).
		q(nt(380), (nt(386)));
		//       option_value(380)    => option_value_false(387).
		q(nt(380), (nt(387)));
		//       option_value(380)    => severity(388).
		q(nt(380), (nt(388)));
		//       option_value_true(386) => 't'.
		q(nt(386), (t(52)));
		//       option_value_true(386) => 't' 'r' 'u' 'e'.
		q(nt(386), (t(52)+t(45)+t(44)+t(28)));
		//       option_value_true(386) => 'o' 'n'.
		q(nt(386), (t(31)+t(49)));
		//       option_value_true(386) => '1'.
		q(nt(386), (t(38)));
		//       option_value_true(386) => 'y'.
		q(nt(386), (t(58)));
		//       option_value_true(386) => 'y' 'e' 's'.
		q(nt(386), (t(58)+t(28)+t(43)));
		//       option_value_false(387) => 'f'.
		q(nt(387), (t(36)));
		//       option_value_false(387) => 'f' 'a' 'l' 's' 'e'.
		q(nt(387), (t(36)+t(26)+t(27)+t(43)+t(28)));
		//       option_value_false(387) => 'o' 'f' 'f'.
		q(nt(387), (t(31)+t(36)+t(36)));
		//       option_value_false(387) => '0'.
		q(nt(387), (t(39)));
		//       option_value_false(387) => 'n'.
		q(nt(387), (t(49)));
		//       option_value_false(387) => 'n' 'o'.
		q(nt(387), (t(49)+t(31)));
		//       severity(388)        => error_sym(389).
		q(nt(388), (nt(389)));
		//       severity(388)        => debug_sym(390).
		q(nt(388), (nt(390)));
		//       severity(388)        => trace_sym(391).
		q(nt(388), (nt(391)));
		//       severity(388)        => info_sym(392).
		q(nt(388), (nt(392)));
		//       error_sym(389)       => 'e'.
		q(nt(389), (t(28)));
		//       error_sym(389)       => 'e' 'r' 'r' 'o' 'r'.
		q(nt(389), (t(28)+t(45)+t(45)+t(31)+t(45)));
		//       info_sym(392)        => 'i'.
		q(nt(392), (t(41)));
		//       info_sym(392)        => 'i' 'n' 'f' 'o'.
		q(nt(392), (t(41)+t(49)+t(36)+t(31)));
		//       debug_sym(390)       => 'd'.
		q(nt(390), (t(53)));
		//       debug_sym(390)       => 'd' 'e' 'b' 'u' 'g'.
		q(nt(390), (t(53)+t(28)+t(30)+t(44)+t(50)));
		//       trace_sym(391)       => 't'.
		q(nt(391), (t(52)));
		//       trace_sym(391)       => 't' 'r' 'a' 'c' 'e'.
		q(nt(391), (t(52)+t(45)+t(26)+t(47)+t(28)));
		return q;
	}
};

#endif // __TAU_PARSER_H__
