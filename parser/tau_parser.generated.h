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
		__, bf_and_sym, wff_and_sym, tau_and_sym, bf, bf_parenthesis, wff, wff_parenthesis, tau, tau_parenthesis, 
		comment, __E_comment_0, __E_comment_1, __E_comment_2, __E____3, __E___4, dot, open_parenthesis, close_parenthesis, open_bracket, 
		close_bracket, open_brace, close_brace, minus, colon, semicolon, less, comma, apostrophe, quote, 
		tau_def, tau_builder_def, tau_or_sym, tau_neg_sym, wff_def, wff_builder_def, wff_or_sym, wff_xor_sym, wff_conditional_sym, wff_neg_sym, 
		wff_imply_sym, wff_equiv_sym, wff_all_sym, wff_ex_sym, wff_ball_sym, wff_bex_sym, wff_t, wff_f, bf_def, bf_builder_def, 
		bf_or_sym, bf_xor_sym, bf_neg_sym, bf_equality_sym, bf_nequality_sym, bf_less_sym, bf_less_equal_sym, bf_not_less_equal_sym, bf_greater_sym, bf_all_sym, 
		bf_ex_sym, bf_splitter_sym, bf_t, bf_f, chars, __E_chars_5, __E_chars_6, digits, __E_digits_7, sym, 
		offsets, offset, __E_offsets_8, __E_offsets_9, num, capture, shift, __E_offset_10, variable, io_var, 
		__E_shift_11, __E___E_shift_11_12, charvar, in, out, in_var_name, out_var_name, bool_variable, __E_charvar_13, __E_charvar_14, 
		__E_charvar_15, capture_var, var, tau_rule, tau_matcher, tau_body, tau_collapse_positives_cb, tau_positives_upwards_cb, tau_rec_relation, tau_ref, 
		__E_tau_ref_16, tau_ref_args, __E_tau_ref_args_17, tau_ref_arg, __E_tau_ref_args_18, tau_or, tau_and, tau_neg, tau_wff, wff_rule, 
		wff_matcher, wff_body, bf_eq_cb, bf_neq_cb, wff_has_clashing_subformulas_cb, wff_has_subformula_cb, wff_remove_existential_cb, wff_remove_bexistential_cb, wff_remove_buniversal_cb, wff_rec_relation, 
		wff_ref, wff_conditional, wff_ball, wff_bex, wff_all, wff_ex, wff_imply, wff_equiv, wff_or, wff_xor, 
		wff_and, wff_neg, bf_interval, bf_neq, bf_eq, bf_nleq_lower, bf_nleq_upper, bf_greater, bf_less_equal, bf_less, 
		__E_wff_ref_19, wff_ref_args, __E_wff_ref_args_20, wff_ref_arg, __E_wff_ref_args_21, __E_wff_and_22, __E_wff_all_23, __E_wff_ex_24, __E_wff_ball_25, __E_wff_bex_26, 
		__E_bf_nleq_lower_27, __E_bf_nleq_upper_28, __E_bf_interval_29, bf_rule, bf_matcher, bf_body, bf_is_zero_cb, bf_is_one_cb, bf_has_subformula_cb, bf_remove_funiversal_cb, 
		bf_remove_fexistential_cb, bf_and_cb, bf_or_cb, bf_xor_cb, bf_neg_cb, bf_splitter, bf_ref, bf_all, bf_ex, bf_or, 
		bf_xor, bf_and, bf_neg, bf_constant, bf_rec_relation, __E_bf_ref_30, bf_ref_args, __E_bf_ref_args_31, bf_ref_arg, __E_bf_ref_args_32, 
		__E_bf_and_33, __E_bf_all_34, __E_bf_ex_35, constant, binding, source_binding, named_binding, type, source, source0, 
		__E_source_36, __E_source_37, bf_and_cb_sym, bf_cb_arg, bf_or_cb_sym, bf_xor_cb_sym, bf_neg_cb_sym, bf_eq_cb_sym, wff_cb_arg, bf_neq_cb_sym, 
		bf_is_zero_cb_sym, bf_is_one_cb_sym, bf_remove_funiversal_cb_sym, bf_remove_fexistential_cb_sym, wff_remove_existential_cb_sym, wff_remove_bexistential_cb_sym, wff_remove_buniversal_cb_sym, wff_has_clashing_subformulas_cb_sym, bf_has_subformula_cb_sym, wff_has_subformula_cb_sym, 
		__E_tau_collapse_positives_cb_38, tau_collapse_positives_cb_sym, tau_cb_arg, __E_tau_collapse_positives_cb_39, tau_positives_upwards_cb_sym, inputs, input, __E_inputs_40, __E_inputs_41, builder, 
		builder_head, builder_body, __E_builder_head_42, __E_builder_head_43, bf_builder_body, wff_builder_body, tau_builder_body, library, rules, __E_rules_44, 
		rule, __E_rules_45, nso_rr, nso_rec_relations, nso_main, __E_nso_rec_relations_46, nso_rec_relation, __E_nso_rec_relations_47, gssotc_rr, gssotc_rec_relations, 
		gssotc_main, __E_gssotc_rec_relations_48, gssotc_rec_relation, __E_gssotc_rec_relations_49, start, __E_start_50, cli, cli_command, __E_cli_51, __E_cli_52, 
		help_cmd, version_cmd, quit_cmd, clear_cmd, get_cmd, set_cmd, toggle_cmd, memory_list_cmd, memory_clear_cmd, memory_print_cmd, 
		memory_del_cmd, memory_store_cmd, file, normalize_cmd, execute_cmd, solve_cmd, is_satisfiable_cmd, is_valid_cmd, is_unsatisfiable_cmd, dnf_cmd, 
		cnf_cmd, anf_cmd, nnf_cmd, pnf_cmd, mnf_cmd, onf_cmd, bf_substitute_cmd, wff_substitute_cmd, bf_instantiate_cmd, wff_instantiate_cmd, 
		def_rule_cmd, def_list_cmd, def_del_cmd, def_clear_cmd, normalize_cmd_sym, normalize_cmd_arg, memory, wff_selection, solve_cmd_sym, solve_cmd_arg, 
		execute_cmd_sym, execute_cmd_arg, is_satisfiable_cmd_sym, is_satisfiable_cmd_arg, is_valid_cmd_sym, is_valid_cmd_arg, is_unsatisfiable_cmd_sym, is_unsatisfiable_cmd_arg, file_sym, file_path, 
		instantiate_cmd_sym, __E_bf_instantiate_cmd_53, bf_selection, __E_bf_instantiate_cmd_54, __E_wff_instantiate_cmd_55, __E_wff_instantiate_cmd_56, substitute_cmd_sym, __E_bf_substitute_cmd_57, __E_bf_substitute_cmd_58, __E_bf_substitute_cmd_59, 
		substitutecmd__sym, __E_wff_substitute_cmd_60, __E_wff_substitute_cmd_61, __E_wff_substitute_cmd_62, dnf_cmd_sym, nf_cmd_arg, cnf_cmd_sym, anf_cmd_sym, nnf_cmd_sym, pnf_cmd_sym, 
		mnf_cmd_sym, onf_cmd_sym, onf_cmd_arg, def_cmd_sym, def_rule_cmd_arg, __E_def_rule_cmd_arg_63, def_cmd_list, def_cmd_del, def_cmf_clear, help_cmd_sym, 
		__E_help_cmd_64, cli_cmd_sym, version_cmd_sym, quit_cmd_sym, clear_cmd_sym, selection_sym, bf_var_selection, wff_var_selection, absolute_memory, relative_memory, 
		__E_absolute_memory_65, absolute_memory_sym, memory_sym, memory_id, relative_memory_sym, memory_cmd_sym, memory_clear_cmd_sym, memory_del_cmd_sym, memory_store_cmd_arg, __E_memory_store_cmd_arg_66, 
		file_cmd_sym, examples_sym, get_cmd_sym, set_cmd_sym, toggle_cmd_sym, __E_get_cmd_67, option, __E_set_cmd_68, option_value, bool_option, 
		severity_opt, status_opt, colors_opt, debug_repl_opt, option_value_true, option_value_false, severity, error_sym, debug_sym, trace_sym, 
		info_sym, __N_0, __N_1, __N_2, __N_3, 
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
		'n', 'g', 'v', 't', 'd', 'q', 'z', 'p', 'y', '@', 
		'%', 
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
			"__", "bf_and_sym", "wff_and_sym", "tau_and_sym", "bf", "bf_parenthesis", "wff", "wff_parenthesis", "tau", "tau_parenthesis", 
			"comment", "__E_comment_0", "__E_comment_1", "__E_comment_2", "__E____3", "__E___4", "dot", "open_parenthesis", "close_parenthesis", "open_bracket", 
			"close_bracket", "open_brace", "close_brace", "minus", "colon", "semicolon", "less", "comma", "apostrophe", "quote", 
			"tau_def", "tau_builder_def", "tau_or_sym", "tau_neg_sym", "wff_def", "wff_builder_def", "wff_or_sym", "wff_xor_sym", "wff_conditional_sym", "wff_neg_sym", 
			"wff_imply_sym", "wff_equiv_sym", "wff_all_sym", "wff_ex_sym", "wff_ball_sym", "wff_bex_sym", "wff_t", "wff_f", "bf_def", "bf_builder_def", 
			"bf_or_sym", "bf_xor_sym", "bf_neg_sym", "bf_equality_sym", "bf_nequality_sym", "bf_less_sym", "bf_less_equal_sym", "bf_not_less_equal_sym", "bf_greater_sym", "bf_all_sym", 
			"bf_ex_sym", "bf_splitter_sym", "bf_t", "bf_f", "chars", "__E_chars_5", "__E_chars_6", "digits", "__E_digits_7", "sym", 
			"offsets", "offset", "__E_offsets_8", "__E_offsets_9", "num", "capture", "shift", "__E_offset_10", "variable", "io_var", 
			"__E_shift_11", "__E___E_shift_11_12", "charvar", "in", "out", "in_var_name", "out_var_name", "bool_variable", "__E_charvar_13", "__E_charvar_14", 
			"__E_charvar_15", "capture_var", "var", "tau_rule", "tau_matcher", "tau_body", "tau_collapse_positives_cb", "tau_positives_upwards_cb", "tau_rec_relation", "tau_ref", 
			"__E_tau_ref_16", "tau_ref_args", "__E_tau_ref_args_17", "tau_ref_arg", "__E_tau_ref_args_18", "tau_or", "tau_and", "tau_neg", "tau_wff", "wff_rule", 
			"wff_matcher", "wff_body", "bf_eq_cb", "bf_neq_cb", "wff_has_clashing_subformulas_cb", "wff_has_subformula_cb", "wff_remove_existential_cb", "wff_remove_bexistential_cb", "wff_remove_buniversal_cb", "wff_rec_relation", 
			"wff_ref", "wff_conditional", "wff_ball", "wff_bex", "wff_all", "wff_ex", "wff_imply", "wff_equiv", "wff_or", "wff_xor", 
			"wff_and", "wff_neg", "bf_interval", "bf_neq", "bf_eq", "bf_nleq_lower", "bf_nleq_upper", "bf_greater", "bf_less_equal", "bf_less", 
			"__E_wff_ref_19", "wff_ref_args", "__E_wff_ref_args_20", "wff_ref_arg", "__E_wff_ref_args_21", "__E_wff_and_22", "__E_wff_all_23", "__E_wff_ex_24", "__E_wff_ball_25", "__E_wff_bex_26", 
			"__E_bf_nleq_lower_27", "__E_bf_nleq_upper_28", "__E_bf_interval_29", "bf_rule", "bf_matcher", "bf_body", "bf_is_zero_cb", "bf_is_one_cb", "bf_has_subformula_cb", "bf_remove_funiversal_cb", 
			"bf_remove_fexistential_cb", "bf_and_cb", "bf_or_cb", "bf_xor_cb", "bf_neg_cb", "bf_splitter", "bf_ref", "bf_all", "bf_ex", "bf_or", 
			"bf_xor", "bf_and", "bf_neg", "bf_constant", "bf_rec_relation", "__E_bf_ref_30", "bf_ref_args", "__E_bf_ref_args_31", "bf_ref_arg", "__E_bf_ref_args_32", 
			"__E_bf_and_33", "__E_bf_all_34", "__E_bf_ex_35", "constant", "binding", "source_binding", "named_binding", "type", "source", "source0", 
			"__E_source_36", "__E_source_37", "bf_and_cb_sym", "bf_cb_arg", "bf_or_cb_sym", "bf_xor_cb_sym", "bf_neg_cb_sym", "bf_eq_cb_sym", "wff_cb_arg", "bf_neq_cb_sym", 
			"bf_is_zero_cb_sym", "bf_is_one_cb_sym", "bf_remove_funiversal_cb_sym", "bf_remove_fexistential_cb_sym", "wff_remove_existential_cb_sym", "wff_remove_bexistential_cb_sym", "wff_remove_buniversal_cb_sym", "wff_has_clashing_subformulas_cb_sym", "bf_has_subformula_cb_sym", "wff_has_subformula_cb_sym", 
			"__E_tau_collapse_positives_cb_38", "tau_collapse_positives_cb_sym", "tau_cb_arg", "__E_tau_collapse_positives_cb_39", "tau_positives_upwards_cb_sym", "inputs", "input", "__E_inputs_40", "__E_inputs_41", "builder", 
			"builder_head", "builder_body", "__E_builder_head_42", "__E_builder_head_43", "bf_builder_body", "wff_builder_body", "tau_builder_body", "library", "rules", "__E_rules_44", 
			"rule", "__E_rules_45", "nso_rr", "nso_rec_relations", "nso_main", "__E_nso_rec_relations_46", "nso_rec_relation", "__E_nso_rec_relations_47", "gssotc_rr", "gssotc_rec_relations", 
			"gssotc_main", "__E_gssotc_rec_relations_48", "gssotc_rec_relation", "__E_gssotc_rec_relations_49", "start", "__E_start_50", "cli", "cli_command", "__E_cli_51", "__E_cli_52", 
			"help_cmd", "version_cmd", "quit_cmd", "clear_cmd", "get_cmd", "set_cmd", "toggle_cmd", "memory_list_cmd", "memory_clear_cmd", "memory_print_cmd", 
			"memory_del_cmd", "memory_store_cmd", "file", "normalize_cmd", "execute_cmd", "solve_cmd", "is_satisfiable_cmd", "is_valid_cmd", "is_unsatisfiable_cmd", "dnf_cmd", 
			"cnf_cmd", "anf_cmd", "nnf_cmd", "pnf_cmd", "mnf_cmd", "onf_cmd", "bf_substitute_cmd", "wff_substitute_cmd", "bf_instantiate_cmd", "wff_instantiate_cmd", 
			"def_rule_cmd", "def_list_cmd", "def_del_cmd", "def_clear_cmd", "normalize_cmd_sym", "normalize_cmd_arg", "memory", "wff_selection", "solve_cmd_sym", "solve_cmd_arg", 
			"execute_cmd_sym", "execute_cmd_arg", "is_satisfiable_cmd_sym", "is_satisfiable_cmd_arg", "is_valid_cmd_sym", "is_valid_cmd_arg", "is_unsatisfiable_cmd_sym", "is_unsatisfiable_cmd_arg", "file_sym", "file_path", 
			"instantiate_cmd_sym", "__E_bf_instantiate_cmd_53", "bf_selection", "__E_bf_instantiate_cmd_54", "__E_wff_instantiate_cmd_55", "__E_wff_instantiate_cmd_56", "substitute_cmd_sym", "__E_bf_substitute_cmd_57", "__E_bf_substitute_cmd_58", "__E_bf_substitute_cmd_59", 
			"substitutecmd__sym", "__E_wff_substitute_cmd_60", "__E_wff_substitute_cmd_61", "__E_wff_substitute_cmd_62", "dnf_cmd_sym", "nf_cmd_arg", "cnf_cmd_sym", "anf_cmd_sym", "nnf_cmd_sym", "pnf_cmd_sym", 
			"mnf_cmd_sym", "onf_cmd_sym", "onf_cmd_arg", "def_cmd_sym", "def_rule_cmd_arg", "__E_def_rule_cmd_arg_63", "def_cmd_list", "def_cmd_del", "def_cmf_clear", "help_cmd_sym", 
			"__E_help_cmd_64", "cli_cmd_sym", "version_cmd_sym", "quit_cmd_sym", "clear_cmd_sym", "selection_sym", "bf_var_selection", "wff_var_selection", "absolute_memory", "relative_memory", 
			"__E_absolute_memory_65", "absolute_memory_sym", "memory_sym", "memory_id", "relative_memory_sym", "memory_cmd_sym", "memory_clear_cmd_sym", "memory_del_cmd_sym", "memory_store_cmd_arg", "__E_memory_store_cmd_arg_66", 
			"file_cmd_sym", "examples_sym", "get_cmd_sym", "set_cmd_sym", "toggle_cmd_sym", "__E_get_cmd_67", "option", "__E_set_cmd_68", "option_value", "bool_option", 
			"severity_opt", "status_opt", "colors_opt", "debug_repl_opt", "option_value_true", "option_value_false", "severity", "error_sym", "debug_sym", "trace_sym", 
			"info_sym", "__N_0", "__N_1", "__N_2", "__N_3", 
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
			9, 10, 11, 12, 13
		};
		o.to_inline = {
			{ 14, 15, 14 },
			{ 16, 17, 16 },
			{ 18, 19, 18 }
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
		//       __E_comment_0(21)    => printable(8).
		q(nt(21), (nt(8)));
		//       __E_comment_0(21)    => '\t'.
		q(nt(21), (t(1)));
		//       __E_comment_1(22)    => null.
		q(nt(22), (nul));
		//       __E_comment_1(22)    => __E_comment_0(21) __E_comment_1(22).
		q(nt(22), (nt(21)+nt(22)));
		//       __E_comment_2(23)    => '\n'.
		q(nt(23), (t(2)));
		//       __E_comment_2(23)    => '\r'.
		q(nt(23), (t(3)));
		//       __E_comment_2(23)    => eof(1).
		q(nt(23), (nt(1)));
		//       comment(20)          => '#' __E_comment_1(22) __E_comment_2(23).
		q(nt(20), (t(4)+nt(22)+nt(23)));
		//       __E____3(24)         => space(2).
		q(nt(24), (nt(2)));
		//       __E____3(24)         => comment(20).
		q(nt(24), (nt(20)));
		//       __(10)               => __E____3(24) _(9).
		q(nt(10), (nt(24)+nt(9)));
		//       __E___4(25)          => __(10).
		q(nt(25), (nt(10)));
		//       __E___4(25)          => null.
		q(nt(25), (nul));
		//       _(9)                 => __E___4(25).
		q(nt(9), (nt(25)));
		//       dot(26)              => '.'.
		q(nt(26), (t(5)));
		//       open_parenthesis(27) => '('.
		q(nt(27), (t(6)));
		//       close_parenthesis(28) => ')'.
		q(nt(28), (t(7)));
		//       open_bracket(29)     => '['.
		q(nt(29), (t(8)));
		//       close_bracket(30)    => ']'.
		q(nt(30), (t(9)));
		//       open_brace(31)       => '{'.
		q(nt(31), (t(10)));
		//       close_brace(32)      => '}'.
		q(nt(32), (t(11)));
		//       minus(33)            => '-'.
		q(nt(33), (t(12)));
		//       colon(34)            => ':'.
		q(nt(34), (t(13)));
		//       semicolon(35)        => ';'.
		q(nt(35), (t(14)));
		//       less(36)             => '<'.
		q(nt(36), (t(15)));
		//       comma(37)            => ','.
		q(nt(37), (t(16)));
		//       apostrophe(38)       => '\''.
		q(nt(38), (t(17)));
		//       quote(39)            => '"'.
		q(nt(39), (t(18)));
		//       tau_def(40)          => ':' ':' ':' '='.
		q(nt(40), (t(13)+t(13)+t(13)+t(19)));
		//       tau_builder_def(41)  => '=' ':' ':' ':'.
		q(nt(41), (t(19)+t(13)+t(13)+t(13)));
		//       tau_and_sym(13)      => '&' '&' '&'.
		q(nt(13), (t(20)+t(20)+t(20)));
		//       tau_or_sym(42)       => '|' '|' '|'.
		q(nt(42), (t(21)+t(21)+t(21)));
		//       tau_neg_sym(43)      => '!' '!' '!'.
		q(nt(43), (t(22)+t(22)+t(22)));
		//       wff_def(44)          => ':' ':' '='.
		q(nt(44), (t(13)+t(13)+t(19)));
		//       wff_builder_def(45)  => '=' ':' ':'.
		q(nt(45), (t(19)+t(13)+t(13)));
		//       wff_and_sym(12)      => '&' '&'.
		q(nt(12), (t(20)+t(20)));
		//       wff_or_sym(46)       => '|' '|'.
		q(nt(46), (t(21)+t(21)));
		//       wff_xor_sym(47)      => '^'.
		q(nt(47), (t(23)));
		//       wff_conditional_sym(48) => '?'.
		q(nt(48), (t(24)));
		//       wff_neg_sym(49)      => '!'.
		q(nt(49), (t(22)));
		//       wff_imply_sym(50)    => '-' '>'.
		q(nt(50), (t(12)+t(25)));
		//       wff_equiv_sym(51)    => '<' '-' '>'.
		q(nt(51), (t(15)+t(12)+t(25)));
		//       wff_all_sym(52)      => 'a' 'l' 'l'.
		q(nt(52), (t(26)+t(27)+t(27)));
		//       wff_ex_sym(53)       => 'e' 'x'.
		q(nt(53), (t(28)+t(29)));
		//       wff_ball_sym(54)     => 'b' 'o' 'o' 'l' '_' 'a' 'l' 'l'.
		q(nt(54), (t(30)+t(31)+t(31)+t(27)+t(32)+t(26)+t(27)+t(27)));
		//       wff_bex_sym(55)      => 'b' 'o' 'o' 'l' '_' 'e' 'x'.
		q(nt(55), (t(30)+t(31)+t(31)+t(27)+t(32)+t(28)+t(29)));
		//       wff_t(56)            => 'T'.
		q(nt(56), (t(33)));
		//       wff_f(57)            => 'F'.
		q(nt(57), (t(34)));
		//       bf_def(58)           => ':' '='.
		q(nt(58), (t(13)+t(19)));
		//       bf_builder_def(59)   => '=' ':'.
		q(nt(59), (t(19)+t(13)));
		//       bf_and_sym(11)       => '&'.
		q(nt(11), (t(20)));
		//       bf_or_sym(60)        => '|'.
		q(nt(60), (t(21)));
		//       bf_xor_sym(61)       => '+'.
		q(nt(61), (t(35)));
		//       bf_neg_sym(62)       => apostrophe(38).
		q(nt(62), (nt(38)));
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
		//       __E_offsets_8(82)    => _(9) comma(37) _(9) offset(81).
		q(nt(82), (nt(9)+nt(37)+nt(9)+nt(81)));
		//       __E_offsets_9(83)    => null.
		q(nt(83), (nul));
		//       __E_offsets_9(83)    => __E_offsets_8(82) __E_offsets_9(83).
		q(nt(83), (nt(82)+nt(83)));
		//       offsets(80)          => open_bracket(29) _(9) offset(81) __E_offsets_9(83) _(9) close_bracket(30).
		q(nt(80), (nt(29)+nt(9)+nt(81)+nt(83)+nt(9)+nt(30)));
		//       offset(81)           => num(84).
		q(nt(81), (nt(84)));
		//       offset(81)           => capture(85).
		q(nt(81), (nt(85)));
		//       offset(81)           => shift(86).
		q(nt(81), (nt(86)));
		//       __N_0(381)           => io_var(89).
		q(nt(381), (nt(89)));
		//       __E_offset_10(87)    => variable(88) & ~( __N_0(381) ).	 # conjunctive
		q(nt(87), (nt(88)) & ~(nt(381)));
		//       offset(81)           => __E_offset_10(87).
		q(nt(81), (nt(87)));
		//       __E_shift_11(90)     => capture(85).
		q(nt(90), (nt(85)));
		//       __N_1(382)           => io_var(89).
		q(nt(382), (nt(89)));
		//       __E___E_shift_11_12(91) => variable(88) & ~( __N_1(382) ).	 # conjunctive
		q(nt(91), (nt(88)) & ~(nt(382)));
		//       __E_shift_11(90)     => __E___E_shift_11_12(91).
		q(nt(90), (nt(91)));
		//       shift(86)            => __E_shift_11(90) _(9) minus(33) _(9) num(84).
		q(nt(86), (nt(90)+nt(9)+nt(33)+nt(9)+nt(84)));
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
		//       in(93)               => in_var_name(95) open_bracket(29) offset(81) close_bracket(30).
		q(nt(93), (nt(95)+nt(29)+nt(81)+nt(30)));
		//       out(94)              => out_var_name(96) open_bracket(29) offset(81) close_bracket(30).
		q(nt(94), (nt(96)+nt(29)+nt(81)+nt(30)));
		//       bool_variable(97)    => '?' charvar(92).
		q(nt(97), (t(24)+nt(92)));
		//       __N_2(383)           => 'F'.
		q(nt(383), (t(34)));
		//       __N_3(384)           => 'T'.
		q(nt(384), (t(33)));
		//       __E_charvar_13(98)   => ~( __N_2(383) ) & ~( __N_3(384) ) & alpha(5).	 # conjunctive
		q(nt(98), ~(nt(383)) & ~(nt(384)) & (nt(5)));
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
		//       tau_rule(103)        => tau_matcher(104) _(9) tau_def(40) _(9) tau_body(105) _(9) dot(26).
		q(nt(103), (nt(104)+nt(9)+nt(40)+nt(9)+nt(105)+nt(9)+nt(26)));
		//       tau_matcher(104)     => tau(18).
		q(nt(104), (nt(18)));
		//       tau_body(105)        => tau(18).
		q(nt(105), (nt(18)));
		//       tau_body(105)        => tau_collapse_positives_cb(106).
		q(nt(105), (nt(106)));
		//       tau_body(105)        => tau_positives_upwards_cb(107).
		q(nt(105), (nt(107)));
		//       tau_rec_relation(108) => tau_ref(109) _(9) tau_def(40) _(9) tau(18) _(9) dot(26).
		q(nt(108), (nt(109)+nt(9)+nt(40)+nt(9)+nt(18)+nt(9)+nt(26)));
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
		//       tau_ref_args(111)    => open_parenthesis(27) __E_tau_ref_args_18(114) _(9) close_parenthesis(28).
		q(nt(111), (nt(27)+nt(114)+nt(9)+nt(28)));
		//       tau_ref_arg(113)     => bf(14).
		q(nt(113), (nt(14)));
		//       tau_ref_arg(113)     => capture(85).
		q(nt(113), (nt(85)));
		//       tau_ref_arg(113)     => variable(88).
		q(nt(113), (nt(88)));
		//       tau(18)              => tau_ref(109).
		q(nt(18), (nt(109)));
		//       tau(18)              => tau_or(115).
		q(nt(18), (nt(115)));
		//       tau(18)              => tau_and(116).
		q(nt(18), (nt(116)));
		//       tau(18)              => tau_neg(117).
		q(nt(18), (nt(117)));
		//       tau(18)              => tau_wff(118).
		q(nt(18), (nt(118)));
		//       tau(18)              => capture(85).
		q(nt(18), (nt(85)));
		//       tau(18)              => tau_parenthesis(19).
		q(nt(18), (nt(19)));
		//       tau_parenthesis(19)  => open_parenthesis(27) _(9) tau(18) _(9) close_parenthesis(28).
		q(nt(19), (nt(27)+nt(9)+nt(18)+nt(9)+nt(28)));
		//       tau_and(116)         => tau(18) _(9) tau_and_sym(13) _(9) tau(18).
		q(nt(116), (nt(18)+nt(9)+nt(13)+nt(9)+nt(18)));
		//       tau_or(115)          => tau(18) _(9) tau_or_sym(42) _(9) tau(18).
		q(nt(115), (nt(18)+nt(9)+nt(42)+nt(9)+nt(18)));
		//       tau_neg(117)         => tau_neg_sym(43) _(9) tau(18).
		q(nt(117), (nt(43)+nt(9)+nt(18)));
		//       tau_wff(118)         => open_brace(31) _(9) wff(16) _(9) close_brace(32).
		q(nt(118), (nt(31)+nt(9)+nt(16)+nt(9)+nt(32)));
		//       wff_rule(119)        => wff_matcher(120) _(9) wff_def(44) _(9) wff_body(121) _(9) dot(26).
		q(nt(119), (nt(120)+nt(9)+nt(44)+nt(9)+nt(121)+nt(9)+nt(26)));
		//       wff_matcher(120)     => wff(16).
		q(nt(120), (nt(16)));
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
		//       wff_body(121)        => wff(16).
		q(nt(121), (nt(16)));
		//       wff_rec_relation(129) => wff_ref(130) _(9) wff_def(44) _(9) wff(16) _(9) dot(26).
		q(nt(129), (nt(130)+nt(9)+nt(44)+nt(9)+nt(16)+nt(9)+nt(26)));
		//       wff(16)              => wff_ref(130).
		q(nt(16), (nt(130)));
		//       wff(16)              => wff_conditional(131).
		q(nt(16), (nt(131)));
		//       wff(16)              => wff_ball(132).
		q(nt(16), (nt(132)));
		//       wff(16)              => wff_bex(133).
		q(nt(16), (nt(133)));
		//       wff(16)              => wff_all(134).
		q(nt(16), (nt(134)));
		//       wff(16)              => wff_ex(135).
		q(nt(16), (nt(135)));
		//       wff(16)              => wff_imply(136).
		q(nt(16), (nt(136)));
		//       wff(16)              => wff_equiv(137).
		q(nt(16), (nt(137)));
		//       wff(16)              => wff_or(138).
		q(nt(16), (nt(138)));
		//       wff(16)              => wff_xor(139).
		q(nt(16), (nt(139)));
		//       wff(16)              => wff_and(140).
		q(nt(16), (nt(140)));
		//       wff(16)              => wff_neg(141).
		q(nt(16), (nt(141)));
		//       wff(16)              => wff_t(56).
		q(nt(16), (nt(56)));
		//       wff(16)              => wff_f(57).
		q(nt(16), (nt(57)));
		//       wff(16)              => bf_interval(142).
		q(nt(16), (nt(142)));
		//       wff(16)              => bf_neq(143).
		q(nt(16), (nt(143)));
		//       wff(16)              => bf_eq(144).
		q(nt(16), (nt(144)));
		//       wff(16)              => bf_nleq_lower(145).
		q(nt(16), (nt(145)));
		//       wff(16)              => bf_nleq_upper(146).
		q(nt(16), (nt(146)));
		//       wff(16)              => bf_greater(147).
		q(nt(16), (nt(147)));
		//       wff(16)              => bf_less_equal(148).
		q(nt(16), (nt(148)));
		//       wff(16)              => bf_less(149).
		q(nt(16), (nt(149)));
		//       wff(16)              => capture(85).
		q(nt(16), (nt(85)));
		//       wff(16)              => bool_variable(97).
		q(nt(16), (nt(97)));
		//       wff(16)              => wff_parenthesis(17).
		q(nt(16), (nt(17)));
		//       wff_parenthesis(17)  => open_parenthesis(27) _(9) wff(16) _(9) close_parenthesis(28).
		q(nt(17), (nt(27)+nt(9)+nt(16)+nt(9)+nt(28)));
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
		//       wff_ref_args(151)    => open_parenthesis(27) __E_wff_ref_args_21(154) _(9) close_parenthesis(28).
		q(nt(151), (nt(27)+nt(154)+nt(9)+nt(28)));
		//       wff_ref_arg(153)     => bf(14).
		q(nt(153), (nt(14)));
		//       wff_ref_arg(153)     => capture(85).
		q(nt(153), (nt(85)));
		//       wff_ref_arg(153)     => variable(88).
		q(nt(153), (nt(88)));
		//       __E_wff_and_22(155)  => wff_and_sym(12) _(9).
		q(nt(155), (nt(12)+nt(9)));
		//       __E_wff_and_22(155)  => null.
		q(nt(155), (nul));
		//       wff_and(140)         => wff(16) _(9) __E_wff_and_22(155) wff(16).
		q(nt(140), (nt(16)+nt(9)+nt(155)+nt(16)));
		//       wff_or(138)          => wff(16) _(9) wff_or_sym(46) _(9) wff(16).
		q(nt(138), (nt(16)+nt(9)+nt(46)+nt(9)+nt(16)));
		//       wff_xor(139)         => wff(16) _(9) wff_xor_sym(47) _(9) wff(16).
		q(nt(139), (nt(16)+nt(9)+nt(47)+nt(9)+nt(16)));
		//       wff_conditional(131) => wff(16) _(9) wff_conditional_sym(48) _(9) wff(16) _(9) colon(34) _(9) wff(16).
		q(nt(131), (nt(16)+nt(9)+nt(48)+nt(9)+nt(16)+nt(9)+nt(34)+nt(9)+nt(16)));
		//       wff_neg(141)         => wff_neg_sym(49) _(9) wff(16).
		q(nt(141), (nt(49)+nt(9)+nt(16)));
		//       wff_imply(136)       => wff(16) _(9) wff_imply_sym(50) _(9) wff(16).
		q(nt(136), (nt(16)+nt(9)+nt(50)+nt(9)+nt(16)));
		//       wff_equiv(137)       => wff(16) _(9) wff_equiv_sym(51) _(9) wff(16).
		q(nt(137), (nt(16)+nt(9)+nt(51)+nt(9)+nt(16)));
		//       __E_wff_all_23(156)  => variable(88).
		q(nt(156), (nt(88)));
		//       __E_wff_all_23(156)  => capture(85).
		q(nt(156), (nt(85)));
		//       wff_all(134)         => wff_all_sym(52) __(10) __E_wff_all_23(156) __(10) wff(16).
		q(nt(134), (nt(52)+nt(10)+nt(156)+nt(10)+nt(16)));
		//       __E_wff_ex_24(157)   => variable(88).
		q(nt(157), (nt(88)));
		//       __E_wff_ex_24(157)   => capture(85).
		q(nt(157), (nt(85)));
		//       wff_ex(135)          => wff_ex_sym(53) __(10) __E_wff_ex_24(157) __(10) wff(16).
		q(nt(135), (nt(53)+nt(10)+nt(157)+nt(10)+nt(16)));
		//       __E_wff_ball_25(158) => bool_variable(97).
		q(nt(158), (nt(97)));
		//       __E_wff_ball_25(158) => capture(85).
		q(nt(158), (nt(85)));
		//       wff_ball(132)        => wff_ball_sym(54) __(10) __E_wff_ball_25(158) __(10) wff(16).
		q(nt(132), (nt(54)+nt(10)+nt(158)+nt(10)+nt(16)));
		//       __E_wff_bex_26(159)  => bool_variable(97).
		q(nt(159), (nt(97)));
		//       __E_wff_bex_26(159)  => capture(85).
		q(nt(159), (nt(85)));
		//       wff_bex(133)         => wff_bex_sym(55) __(10) __E_wff_bex_26(159) __(10) wff(16).
		q(nt(133), (nt(55)+nt(10)+nt(159)+nt(10)+nt(16)));
		//       bf_eq(144)           => bf(14) _(9) bf_equality_sym(63) _(9) bf(14).
		q(nt(144), (nt(14)+nt(9)+nt(63)+nt(9)+nt(14)));
		//       bf_neq(143)          => bf(14) _(9) bf_nequality_sym(64) _(9) bf(14).
		q(nt(143), (nt(14)+nt(9)+nt(64)+nt(9)+nt(14)));
		//       bf_less(149)         => bf(14) _(9) bf_less_sym(65) _(9) bf(14).
		q(nt(149), (nt(14)+nt(9)+nt(65)+nt(9)+nt(14)));
		//       bf_less_equal(148)   => bf(14) _(9) bf_less_equal_sym(66) _(9) bf(14).
		q(nt(148), (nt(14)+nt(9)+nt(66)+nt(9)+nt(14)));
		//       __E_bf_nleq_lower_27(160) => variable(88).
		q(nt(160), (nt(88)));
		//       __E_bf_nleq_lower_27(160) => capture(85).
		q(nt(160), (nt(85)));
		//       bf_nleq_lower(145)   => bf(14) _(9) bf_not_less_equal_sym(67) _(9) __E_bf_nleq_lower_27(160).
		q(nt(145), (nt(14)+nt(9)+nt(67)+nt(9)+nt(160)));
		//       __E_bf_nleq_upper_28(161) => variable(88).
		q(nt(161), (nt(88)));
		//       __E_bf_nleq_upper_28(161) => capture(85).
		q(nt(161), (nt(85)));
		//       bf_nleq_upper(146)   => __E_bf_nleq_upper_28(161) _(9) bf_not_less_equal_sym(67) _(9) bf(14).
		q(nt(146), (nt(161)+nt(9)+nt(67)+nt(9)+nt(14)));
		//       bf_greater(147)      => bf(14) _(9) bf_greater_sym(68) _(9) bf(14).
		q(nt(147), (nt(14)+nt(9)+nt(68)+nt(9)+nt(14)));
		//       __E_bf_interval_29(162) => variable(88).
		q(nt(162), (nt(88)));
		//       __E_bf_interval_29(162) => capture(85).
		q(nt(162), (nt(85)));
		//       bf_interval(142)     => bf(14) _(9) bf_less_equal_sym(66) _(9) __E_bf_interval_29(162) _(9) bf_less_equal_sym(66) _(9) bf(14).
		q(nt(142), (nt(14)+nt(9)+nt(66)+nt(9)+nt(162)+nt(9)+nt(66)+nt(9)+nt(14)));
		//       bf_rule(163)         => bf_matcher(164) _(9) bf_def(58) _(9) bf_body(165) _(9) dot(26).
		q(nt(163), (nt(164)+nt(9)+nt(58)+nt(9)+nt(165)+nt(9)+nt(26)));
		//       bf_matcher(164)      => bf(14).
		q(nt(164), (nt(14)));
		//       bf_body(165)         => bf(14).
		q(nt(165), (nt(14)));
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
		//       bf(14)               => bf_splitter(175).
		q(nt(14), (nt(175)));
		//       bf(14)               => bf_ref(176).
		q(nt(14), (nt(176)));
		//       bf(14)               => bf_all(177).
		q(nt(14), (nt(177)));
		//       bf(14)               => bf_ex(178).
		q(nt(14), (nt(178)));
		//       bf(14)               => bf_or(179).
		q(nt(14), (nt(179)));
		//       bf(14)               => bf_xor(180).
		q(nt(14), (nt(180)));
		//       bf(14)               => bf_and(181).
		q(nt(14), (nt(181)));
		//       bf(14)               => bf_neg(182).
		q(nt(14), (nt(182)));
		//       bf(14)               => bf_constant(183).
		q(nt(14), (nt(183)));
		//       bf(14)               => bf_t(72).
		q(nt(14), (nt(72)));
		//       bf(14)               => bf_f(73).
		q(nt(14), (nt(73)));
		//       bf(14)               => capture(85).
		q(nt(14), (nt(85)));
		//       bf(14)               => variable(88).
		q(nt(14), (nt(88)));
		//       bf(14)               => bf_parenthesis(15).
		q(nt(14), (nt(15)));
		//       bf_parenthesis(15)   => open_parenthesis(27) _(9) bf(14) _(9) close_parenthesis(28).
		q(nt(15), (nt(27)+nt(9)+nt(14)+nt(9)+nt(28)));
		//       bf_rec_relation(184) => bf_ref(176) _(9) bf_def(58) _(9) bf(14) _(9) dot(26).
		q(nt(184), (nt(176)+nt(9)+nt(58)+nt(9)+nt(14)+nt(9)+nt(26)));
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
		//       bf_ref_args(186)     => open_parenthesis(27) __E_bf_ref_args_32(189) _(9) close_parenthesis(28).
		q(nt(186), (nt(27)+nt(189)+nt(9)+nt(28)));
		//       bf_ref_arg(188)      => bf(14).
		q(nt(188), (nt(14)));
		//       bf_ref_arg(188)      => capture(85).
		q(nt(188), (nt(85)));
		//       bf_ref_arg(188)      => variable(88).
		q(nt(188), (nt(88)));
		//       __E_bf_and_33(190)   => bf_and_sym(11) _(9).
		q(nt(190), (nt(11)+nt(9)));
		//       __E_bf_and_33(190)   => null.
		q(nt(190), (nul));
		//       bf_and(181)          => bf(14) _(9) __E_bf_and_33(190) bf(14).
		q(nt(181), (nt(14)+nt(9)+nt(190)+nt(14)));
		//       bf_or(179)           => bf(14) _(9) bf_or_sym(60) _(9) bf(14).
		q(nt(179), (nt(14)+nt(9)+nt(60)+nt(9)+nt(14)));
		//       bf_xor(180)          => bf(14) _(9) bf_xor_sym(61) _(9) bf(14).
		q(nt(180), (nt(14)+nt(9)+nt(61)+nt(9)+nt(14)));
		//       bf_neg(182)          => bf(14) _(9) bf_neg_sym(62).
		q(nt(182), (nt(14)+nt(9)+nt(62)));
		//       __E_bf_all_34(191)   => variable(88).
		q(nt(191), (nt(88)));
		//       __E_bf_all_34(191)   => capture(85).
		q(nt(191), (nt(85)));
		//       bf_all(177)          => bf_all_sym(69) __(10) __E_bf_all_34(191) __(10) bf(14).
		q(nt(177), (nt(69)+nt(10)+nt(191)+nt(10)+nt(14)));
		//       __E_bf_ex_35(192)    => variable(88).
		q(nt(192), (nt(88)));
		//       __E_bf_ex_35(192)    => capture(85).
		q(nt(192), (nt(85)));
		//       bf_ex(178)           => bf_ex_sym(70) __(10) __E_bf_ex_35(192) __(10) bf(14).
		q(nt(178), (nt(70)+nt(10)+nt(192)+nt(10)+nt(14)));
		//       bf_splitter(175)     => bf_splitter_sym(71) _(9) open_parenthesis(27) _(9) bf(14) _(9) close_parenthesis(28).
		q(nt(175), (nt(71)+nt(9)+nt(27)+nt(9)+nt(14)+nt(9)+nt(28)));
		//       bf_constant(183)     => open_brace(31) _(9) constant(193) _(9) close_brace(32).
		q(nt(183), (nt(31)+nt(9)+nt(193)+nt(9)+nt(32)));
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
		//       source_binding(195)  => type(197) _(9) colon(34) _(9) source(198).
		q(nt(195), (nt(197)+nt(9)+nt(34)+nt(9)+nt(198)));
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
		//       bf_cb_arg(203)       => bf(14).
		q(nt(203), (nt(14)));
		//       wff_cb_arg(208)      => wff(16).
		q(nt(208), (nt(16)));
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
		//       tau_cb_arg(222)      => tau(18).
		q(nt(222), (nt(18)));
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
		//       inputs(225)          => _(9) less(36) _(9) input(226) __E_inputs_41(228) _(9) dot(26).
		q(nt(225), (nt(9)+nt(36)+nt(9)+nt(226)+nt(228)+nt(9)+nt(26)));
		//       input(226)           => in(93) _(9) colon(34) _(9) open_brace(31) _(9) source_binding(195) _(9) close_brace(32).
		q(nt(226), (nt(93)+nt(9)+nt(34)+nt(9)+nt(31)+nt(9)+nt(195)+nt(9)+nt(32)));
		//       builder(229)         => _(9) builder_head(230) _(9) builder_body(231) _(9) dot(26).
		q(nt(229), (nt(9)+nt(230)+nt(9)+nt(231)+nt(9)+nt(26)));
		//       __E_builder_head_42(232) => __(10) capture(85).
		q(nt(232), (nt(10)+nt(85)));
		//       __E_builder_head_43(233) => null.
		q(nt(233), (nul));
		//       __E_builder_head_43(233) => __E_builder_head_42(232) __E_builder_head_43(233).
		q(nt(233), (nt(232)+nt(233)));
		//       builder_head(230)    => open_parenthesis(27) _(9) capture(85) __E_builder_head_43(233) _(9) close_parenthesis(28).
		q(nt(230), (nt(27)+nt(9)+nt(85)+nt(233)+nt(9)+nt(28)));
		//       builder_body(231)    => bf_builder_body(234).
		q(nt(231), (nt(234)));
		//       builder_body(231)    => wff_builder_body(235).
		q(nt(231), (nt(235)));
		//       builder_body(231)    => tau_builder_body(236).
		q(nt(231), (nt(236)));
		//       bf_builder_body(234) => bf_builder_def(59) _(9) bf(14).
		q(nt(234), (nt(59)+nt(9)+nt(14)));
		//       wff_builder_body(235) => wff_builder_def(45) _(9) wff(16).
		q(nt(235), (nt(45)+nt(9)+nt(16)));
		//       tau_builder_body(236) => tau_builder_def(41) _(9) tau(18).
		q(nt(236), (nt(41)+nt(9)+nt(18)));
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
		//       nso_main(244)        => _(9) wff(16) _(9) dot(26).
		q(nt(244), (nt(9)+nt(16)+nt(9)+nt(26)));
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
		//       gssotc_main(250)     => _(9) tau(18) _(9) semicolon(35).
		q(nt(250), (nt(9)+nt(18)+nt(9)+nt(35)));
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
		//       __E_cli_51(258)      => _(9) dot(26) _(9) cli_command(257).
		q(nt(258), (nt(9)+nt(26)+nt(9)+nt(257)));
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
		//       cli_command(257)     => memory_list_cmd(267).
		q(nt(257), (nt(267)));
		//       cli_command(257)     => memory_clear_cmd(268).
		q(nt(257), (nt(268)));
		//       cli_command(257)     => memory_print_cmd(269).
		q(nt(257), (nt(269)));
		//       cli_command(257)     => memory_del_cmd(270).
		q(nt(257), (nt(270)));
		//       cli_command(257)     => memory_store_cmd(271).
		q(nt(257), (nt(271)));
		//       cli_command(257)     => file(272).
		q(nt(257), (nt(272)));
		//       cli_command(257)     => normalize_cmd(273).
		q(nt(257), (nt(273)));
		//       cli_command(257)     => execute_cmd(274).
		q(nt(257), (nt(274)));
		//       cli_command(257)     => solve_cmd(275).
		q(nt(257), (nt(275)));
		//       cli_command(257)     => is_satisfiable_cmd(276).
		q(nt(257), (nt(276)));
		//       cli_command(257)     => is_valid_cmd(277).
		q(nt(257), (nt(277)));
		//       cli_command(257)     => is_unsatisfiable_cmd(278).
		q(nt(257), (nt(278)));
		//       cli_command(257)     => dnf_cmd(279).
		q(nt(257), (nt(279)));
		//       cli_command(257)     => cnf_cmd(280).
		q(nt(257), (nt(280)));
		//       cli_command(257)     => anf_cmd(281).
		q(nt(257), (nt(281)));
		//       cli_command(257)     => nnf_cmd(282).
		q(nt(257), (nt(282)));
		//       cli_command(257)     => pnf_cmd(283).
		q(nt(257), (nt(283)));
		//       cli_command(257)     => mnf_cmd(284).
		q(nt(257), (nt(284)));
		//       cli_command(257)     => onf_cmd(285).
		q(nt(257), (nt(285)));
		//       cli_command(257)     => bf_substitute_cmd(286).
		q(nt(257), (nt(286)));
		//       cli_command(257)     => wff_substitute_cmd(287).
		q(nt(257), (nt(287)));
		//       cli_command(257)     => bf_instantiate_cmd(288).
		q(nt(257), (nt(288)));
		//       cli_command(257)     => wff_instantiate_cmd(289).
		q(nt(257), (nt(289)));
		//       cli_command(257)     => def_rule_cmd(290).
		q(nt(257), (nt(290)));
		//       cli_command(257)     => def_list_cmd(291).
		q(nt(257), (nt(291)));
		//       cli_command(257)     => def_del_cmd(292).
		q(nt(257), (nt(292)));
		//       cli_command(257)     => def_clear_cmd(293).
		q(nt(257), (nt(293)));
		//       normalize_cmd(273)   => normalize_cmd_sym(294) __(10) normalize_cmd_arg(295).
		q(nt(273), (nt(294)+nt(10)+nt(295)));
		//       normalize_cmd_arg(295) => wff(16).
		q(nt(295), (nt(16)));
		//       normalize_cmd_arg(295) => nso_rr(242).
		q(nt(295), (nt(242)));
		//       normalize_cmd_arg(295) => memory(296).
		q(nt(295), (nt(296)));
		//       normalize_cmd_arg(295) => wff_selection(297).
		q(nt(295), (nt(297)));
		//       solve_cmd(275)       => solve_cmd_sym(298) __(10) solve_cmd_arg(299).
		q(nt(275), (nt(298)+nt(10)+nt(299)));
		//       solve_cmd_arg(299)   => wff(16).
		q(nt(299), (nt(16)));
		//       solve_cmd_arg(299)   => memory(296).
		q(nt(299), (nt(296)));
		//       solve_cmd_arg(299)   => wff_selection(297).
		q(nt(299), (nt(297)));
		//       execute_cmd(274)     => execute_cmd_sym(300) __(10) execute_cmd_arg(301).
		q(nt(274), (nt(300)+nt(10)+nt(301)));
		//       execute_cmd_arg(301) => tau(18).
		q(nt(301), (nt(18)));
		//       execute_cmd_arg(301) => gssotc_rr(248).
		q(nt(301), (nt(248)));
		//       is_satisfiable_cmd(276) => is_satisfiable_cmd_sym(302) __(10) is_satisfiable_cmd_arg(303).
		q(nt(276), (nt(302)+nt(10)+nt(303)));
		//       is_satisfiable_cmd_arg(303) => gssotc_rr(248).
		q(nt(303), (nt(248)));
		//       is_valid_cmd(277)    => is_valid_cmd_sym(304) __(10) is_valid_cmd_arg(305).
		q(nt(277), (nt(304)+nt(10)+nt(305)));
		//       is_valid_cmd_arg(305) => gssotc_rr(248).
		q(nt(305), (nt(248)));
		//       is_unsatisfiable_cmd(278) => is_unsatisfiable_cmd_sym(306) __(10) is_unsatisfiable_cmd_arg(307).
		q(nt(278), (nt(306)+nt(10)+nt(307)));
		//       is_unsatisfiable_cmd_arg(307) => gssotc_rr(248).
		q(nt(307), (nt(248)));
		//       file(272)            => file_sym(308) quote(39) file_path(309) quote(39).
		q(nt(272), (nt(308)+nt(39)+nt(309)+nt(39)));
		//       __E_bf_instantiate_cmd_53(311) => bf(14).
		q(nt(311), (nt(14)));
		//       __E_bf_instantiate_cmd_53(311) => bf_selection(312).
		q(nt(311), (nt(312)));
		//       __E_bf_instantiate_cmd_53(311) => memory(296).
		q(nt(311), (nt(296)));
		//       __E_bf_instantiate_cmd_54(313) => bf_selection(312).
		q(nt(313), (nt(312)));
		//       __E_bf_instantiate_cmd_54(313) => memory(296).
		q(nt(313), (nt(296)));
		//       bf_instantiate_cmd(288) => instantiate_cmd_sym(310) __(10) variable(88) __(10) __E_bf_instantiate_cmd_53(311) __(10) __E_bf_instantiate_cmd_54(313).
		q(nt(288), (nt(310)+nt(10)+nt(88)+nt(10)+nt(311)+nt(10)+nt(313)));
		//       __E_wff_instantiate_cmd_55(314) => wff(16).
		q(nt(314), (nt(16)));
		//       __E_wff_instantiate_cmd_55(314) => wff_selection(297).
		q(nt(314), (nt(297)));
		//       __E_wff_instantiate_cmd_55(314) => memory(296).
		q(nt(314), (nt(296)));
		//       __E_wff_instantiate_cmd_56(315) => wff_selection(297).
		q(nt(315), (nt(297)));
		//       __E_wff_instantiate_cmd_56(315) => memory(296).
		q(nt(315), (nt(296)));
		//       wff_instantiate_cmd(289) => instantiate_cmd_sym(310) __(10) variable(88) __(10) __E_wff_instantiate_cmd_55(314) __(10) __E_wff_instantiate_cmd_56(315).
		q(nt(289), (nt(310)+nt(10)+nt(88)+nt(10)+nt(314)+nt(10)+nt(315)));
		//       __E_bf_substitute_cmd_57(317) => bf(14).
		q(nt(317), (nt(14)));
		//       __E_bf_substitute_cmd_57(317) => memory(296).
		q(nt(317), (nt(296)));
		//       __E_bf_substitute_cmd_57(317) => bf_selection(312).
		q(nt(317), (nt(312)));
		//       __E_bf_substitute_cmd_58(318) => memory(296).
		q(nt(318), (nt(296)));
		//       __E_bf_substitute_cmd_58(318) => bf_selection(312).
		q(nt(318), (nt(312)));
		//       __E_bf_substitute_cmd_58(318) => bf_selection(312).
		q(nt(318), (nt(312)));
		//       __E_bf_substitute_cmd_59(319) => bf_selection(312).
		q(nt(319), (nt(312)));
		//       __E_bf_substitute_cmd_59(319) => memory(296).
		q(nt(319), (nt(296)));
		//       bf_substitute_cmd(286) => substitute_cmd_sym(316) __(10) __E_bf_substitute_cmd_57(317) __(10) __E_bf_substitute_cmd_58(318) __E_bf_substitute_cmd_59(319).
		q(nt(286), (nt(316)+nt(10)+nt(317)+nt(10)+nt(318)+nt(319)));
		//       __E_wff_substitute_cmd_60(321) => wff(16).
		q(nt(321), (nt(16)));
		//       __E_wff_substitute_cmd_60(321) => memory(296).
		q(nt(321), (nt(296)));
		//       __E_wff_substitute_cmd_60(321) => wff_selection(297).
		q(nt(321), (nt(297)));
		//       __E_wff_substitute_cmd_61(322) => memory(296).
		q(nt(322), (nt(296)));
		//       __E_wff_substitute_cmd_61(322) => wff_selection(297).
		q(nt(322), (nt(297)));
		//       __E_wff_substitute_cmd_62(323) => wff_selection(297).
		q(nt(323), (nt(297)));
		//       __E_wff_substitute_cmd_62(323) => memory(296).
		q(nt(323), (nt(296)));
		//       wff_substitute_cmd(287) => substitutecmd__sym(320) __(10) __E_wff_substitute_cmd_60(321) __(10) __E_wff_substitute_cmd_61(322) __E_wff_substitute_cmd_62(323).
		q(nt(287), (nt(320)+nt(10)+nt(321)+nt(10)+nt(322)+nt(323)));
		//       dnf_cmd(279)         => dnf_cmd_sym(324) __(10) nf_cmd_arg(325).
		q(nt(279), (nt(324)+nt(10)+nt(325)));
		//       cnf_cmd(280)         => cnf_cmd_sym(326) __(10) nf_cmd_arg(325).
		q(nt(280), (nt(326)+nt(10)+nt(325)));
		//       anf_cmd(281)         => anf_cmd_sym(327) __(10) nf_cmd_arg(325).
		q(nt(281), (nt(327)+nt(10)+nt(325)));
		//       nnf_cmd(282)         => nnf_cmd_sym(328) __(10) nf_cmd_arg(325).
		q(nt(282), (nt(328)+nt(10)+nt(325)));
		//       pnf_cmd(283)         => pnf_cmd_sym(329) __(10) nf_cmd_arg(325).
		q(nt(283), (nt(329)+nt(10)+nt(325)));
		//       mnf_cmd(284)         => mnf_cmd_sym(330) __(10) nf_cmd_arg(325).
		q(nt(284), (nt(330)+nt(10)+nt(325)));
		//       nf_cmd_arg(325)      => bf(14).
		q(nt(325), (nt(14)));
		//       nf_cmd_arg(325)      => wff(16).
		q(nt(325), (nt(16)));
		//       nf_cmd_arg(325)      => memory(296).
		q(nt(325), (nt(296)));
		//       onf_cmd(285)         => onf_cmd_sym(331) __(10) variable(88) __(10) onf_cmd_arg(332).
		q(nt(285), (nt(331)+nt(10)+nt(88)+nt(10)+nt(332)));
		//       onf_cmd_arg(332)     => wff(16).
		q(nt(332), (nt(16)));
		//       onf_cmd_arg(332)     => memory(296).
		q(nt(332), (nt(296)));
		//       def_rule_cmd(290)    => def_cmd_sym(333) __(10) def_rule_cmd_arg(334).
		q(nt(290), (nt(333)+nt(10)+nt(334)));
		//       __E_def_rule_cmd_arg_63(335) => bf_rec_relation(184).
		q(nt(335), (nt(184)));
		//       __E_def_rule_cmd_arg_63(335) => wff_rec_relation(129).
		q(nt(335), (nt(129)));
		//       __E_def_rule_cmd_arg_63(335) => tau_rec_relation(108).
		q(nt(335), (nt(108)));
		//       def_rule_cmd_arg(334) => __E_def_rule_cmd_arg_63(335).
		q(nt(334), (nt(335)));
		//       def_list_cmd(291)    => def_cmd_sym(333) __(10) def_cmd_list(336).
		q(nt(291), (nt(333)+nt(10)+nt(336)));
		//       def_del_cmd(292)     => def_cmd_sym(333) __(10) def_cmd_del(337) __(10) digits(77).
		q(nt(292), (nt(333)+nt(10)+nt(337)+nt(10)+nt(77)));
		//       def_clear_cmd(293)   => def_cmd_sym(333) __(10) def_cmf_clear(338).
		q(nt(293), (nt(333)+nt(10)+nt(338)));
		//       __E_help_cmd_64(340) => __(10) cli_cmd_sym(341).
		q(nt(340), (nt(10)+nt(341)));
		//       __E_help_cmd_64(340) => null.
		q(nt(340), (nul));
		//       help_cmd(260)        => help_cmd_sym(339) __E_help_cmd_64(340).
		q(nt(260), (nt(339)+nt(340)));
		//       version_cmd(261)     => version_cmd_sym(342).
		q(nt(261), (nt(342)));
		//       quit_cmd(262)        => quit_cmd_sym(343).
		q(nt(262), (nt(343)));
		//       clear_cmd(263)       => clear_cmd_sym(344).
		q(nt(263), (nt(344)));
		//       wff_selection(297)   => selection_sym(345) __(10) memory(296).
		q(nt(297), (nt(345)+nt(10)+nt(296)));
		//       bf_selection(312)    => selection_sym(345) __(10) memory(296).
		q(nt(312), (nt(345)+nt(10)+nt(296)));
		//       bf_var_selection(346) => selection_sym(345) __(10) memory(296).
		q(nt(346), (nt(345)+nt(10)+nt(296)));
		//       wff_var_selection(347) => selection_sym(345) __(10) memory(296).
		q(nt(347), (nt(345)+nt(10)+nt(296)));
		//       file_path(309)       => chars(74).
		q(nt(309), (nt(74)));
		//       memory(296)          => absolute_memory(348).
		q(nt(296), (nt(348)));
		//       memory(296)          => relative_memory(349).
		q(nt(296), (nt(349)));
		//       __E_absolute_memory_65(350) => absolute_memory_sym(351).
		q(nt(350), (nt(351)));
		//       __E_absolute_memory_65(350) => memory_sym(352) __(10).
		q(nt(350), (nt(352)+nt(10)));
		//       absolute_memory(348) => __E_absolute_memory_65(350) memory_id(353).
		q(nt(348), (nt(350)+nt(353)));
		//       relative_memory(349) => relative_memory_sym(354) memory_id(353).
		q(nt(349), (nt(354)+nt(353)));
		//       memory_id(353)       => digits(77).
		q(nt(353), (nt(77)));
		//       memory_list_cmd(267) => memory_cmd_sym(355).
		q(nt(267), (nt(355)));
		//       memory_clear_cmd(268) => memory_cmd_sym(355) __(10) memory_clear_cmd_sym(356).
		q(nt(268), (nt(355)+nt(10)+nt(356)));
		//       memory_print_cmd(269) => memory_cmd_sym(355) __(10) memory(296).
		q(nt(269), (nt(355)+nt(10)+nt(296)));
		//       memory_del_cmd(270)  => memory_cmd_sym(355) __(10) memory_del_cmd_sym(357) __(10) memory(296).
		q(nt(270), (nt(355)+nt(10)+nt(357)+nt(10)+nt(296)));
		//       memory_store_cmd(271) => memory_cmd_sym(355) __(10) memory_store_cmd_arg(358).
		q(nt(271), (nt(355)+nt(10)+nt(358)));
		//       __E_memory_store_cmd_arg_66(359) => bf(14).
		q(nt(359), (nt(14)));
		//       __E_memory_store_cmd_arg_66(359) => wff(16).
		q(nt(359), (nt(16)));
		//       __E_memory_store_cmd_arg_66(359) => tau(18).
		q(nt(359), (nt(18)));
		//       memory_store_cmd_arg(358) => __E_memory_store_cmd_arg_66(359).
		q(nt(358), (nt(359)));
		//       cli_cmd_sym(341)     => help_cmd_sym(339).
		q(nt(341), (nt(339)));
		//       cli_cmd_sym(341)     => version_cmd_sym(342).
		q(nt(341), (nt(342)));
		//       cli_cmd_sym(341)     => quit_cmd_sym(343).
		q(nt(341), (nt(343)));
		//       cli_cmd_sym(341)     => clear_cmd_sym(344).
		q(nt(341), (nt(344)));
		//       cli_cmd_sym(341)     => memory_cmd_sym(355).
		q(nt(341), (nt(355)));
		//       cli_cmd_sym(341)     => selection_sym(345).
		q(nt(341), (nt(345)));
		//       cli_cmd_sym(341)     => instantiate_cmd_sym(310).
		q(nt(341), (nt(310)));
		//       cli_cmd_sym(341)     => substitute_cmd_sym(316).
		q(nt(341), (nt(316)));
		//       cli_cmd_sym(341)     => def_cmd_sym(333).
		q(nt(341), (nt(333)));
		//       cli_cmd_sym(341)     => normalize_cmd_sym(294).
		q(nt(341), (nt(294)));
		//       cli_cmd_sym(341)     => execute_cmd_sym(300).
		q(nt(341), (nt(300)));
		//       cli_cmd_sym(341)     => file_cmd_sym(360).
		q(nt(341), (nt(360)));
		//       cli_cmd_sym(341)     => solve_cmd_sym(298).
		q(nt(341), (nt(298)));
		//       cli_cmd_sym(341)     => is_satisfiable_cmd_sym(302).
		q(nt(341), (nt(302)));
		//       cli_cmd_sym(341)     => is_valid_cmd_sym(304).
		q(nt(341), (nt(304)));
		//       cli_cmd_sym(341)     => is_unsatisfiable_cmd_sym(306).
		q(nt(341), (nt(306)));
		//       cli_cmd_sym(341)     => examples_sym(361).
		q(nt(341), (nt(361)));
		//       cli_cmd_sym(341)     => get_cmd_sym(362).
		q(nt(341), (nt(362)));
		//       cli_cmd_sym(341)     => set_cmd_sym(363).
		q(nt(341), (nt(363)));
		//       cli_cmd_sym(341)     => toggle_cmd_sym(364).
		q(nt(341), (nt(364)));
		//       cli_cmd_sym(341)     => cnf_cmd_sym(326).
		q(nt(341), (nt(326)));
		//       cli_cmd_sym(341)     => dnf_cmd_sym(324).
		q(nt(341), (nt(324)));
		//       cli_cmd_sym(341)     => nnf_cmd_sym(328).
		q(nt(341), (nt(328)));
		//       cli_cmd_sym(341)     => mnf_cmd_sym(330).
		q(nt(341), (nt(330)));
		//       cli_cmd_sym(341)     => onf_cmd_sym(331).
		q(nt(341), (nt(331)));
		//       help_cmd_sym(339)    => 'h'.
		q(nt(339), (t(42)));
		//       help_cmd_sym(339)    => 'h' 'e' 'l' 'p'.
		q(nt(339), (t(42)+t(28)+t(27)+t(56)));
		//       version_cmd_sym(342) => 'v'.
		q(nt(342), (t(51)));
		//       version_cmd_sym(342) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
		q(nt(342), (t(51)+t(28)+t(45)+t(43)+t(41)+t(31)+t(49)));
		//       quit_cmd_sym(343)    => 'q'.
		q(nt(343), (t(54)));
		//       quit_cmd_sym(343)    => 'e'.
		q(nt(343), (t(28)));
		//       quit_cmd_sym(343)    => 'q' 'u' 'i' 't'.
		q(nt(343), (t(54)+t(44)+t(41)+t(52)));
		//       quit_cmd_sym(343)    => 'e' 'x' 'i' 't'.
		q(nt(343), (t(28)+t(29)+t(41)+t(52)));
		//       instantiate_cmd_sym(310) => 'i'.
		q(nt(310), (t(41)));
		//       instantiate_cmd_sym(310) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
		q(nt(310), (t(41)+t(49)+t(43)+t(52)+t(26)+t(49)+t(52)+t(41)+t(26)+t(52)+t(28)));
		//       substitute_cmd_sym(316) => 's'.
		q(nt(316), (t(43)));
		//       substitute_cmd_sym(316) => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
		q(nt(316), (t(43)+t(44)+t(30)+t(43)+t(52)+t(41)+t(52)+t(44)+t(52)+t(28)));
		//       file_cmd_sym(360)    => 'r'.
		q(nt(360), (t(45)));
		//       file_cmd_sym(360)    => 'r' 'e' 'a' 'd'.
		q(nt(360), (t(45)+t(28)+t(26)+t(53)));
		//       clear_cmd_sym(344)   => 'c'.
		q(nt(344), (t(47)));
		//       clear_cmd_sym(344)   => 'c' 'l' 'e' 'a' 'r'.
		q(nt(344), (t(47)+t(27)+t(28)+t(26)+t(45)));
		//       normalize_cmd_sym(294) => 'n'.
		q(nt(294), (t(49)));
		//       normalize_cmd_sym(294) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
		q(nt(294), (t(49)+t(31)+t(45)+t(46)+t(26)+t(27)+t(41)+t(55)+t(28)));
		//       execute_cmd_sym(300) => 'e' 'x' 'e' 'c' 'u' 't' 'e'.
		q(nt(300), (t(28)+t(29)+t(28)+t(47)+t(44)+t(52)+t(28)));
		//       solve_cmd_sym(298)   => 's' 'o' 'l' 'v' 'e'.
		q(nt(298), (t(43)+t(31)+t(27)+t(51)+t(28)));
		//       is_satisfiable_cmd_sym(302) => 's' 'a' 't'.
		q(nt(302), (t(43)+t(26)+t(52)));
		//       is_valid_cmd_sym(304) => 'v' 'a' 'l' 'i' 'd'.
		q(nt(304), (t(51)+t(26)+t(27)+t(41)+t(53)));
		//       is_unsatisfiable_cmd_sym(306) => 'u' 'n' 's' 'a' 't'.
		q(nt(306), (t(44)+t(49)+t(43)+t(26)+t(52)));
		//       onf_cmd_sym(331)     => 'o' 'n' 'f'.
		q(nt(331), (t(31)+t(49)+t(36)));
		//       dnf_cmd_sym(324)     => 'd' 'n' 'f'.
		q(nt(324), (t(53)+t(49)+t(36)));
		//       cnf_cmd_sym(326)     => 'c' 'n' 'f'.
		q(nt(326), (t(47)+t(49)+t(36)));
		//       anf_cmd_sym(327)     => 'a' 'n' 'f'.
		q(nt(327), (t(26)+t(49)+t(36)));
		//       nnf_cmd_sym(328)     => 'n' 'n' 'f'.
		q(nt(328), (t(49)+t(49)+t(36)));
		//       pnf_cmd_sym(329)     => 'p' 'n' 'f'.
		q(nt(329), (t(56)+t(49)+t(36)));
		//       mnf_cmd_sym(330)     => 'm' 'n' 'f'.
		q(nt(330), (t(46)+t(49)+t(36)));
		//       def_cmd_sym(333)     => 'd' 'e' 'f'.
		q(nt(333), (t(53)+t(28)+t(36)));
		//       def_cmd_list(336)    => 'l' 'i' 's' 't'.
		q(nt(336), (t(27)+t(41)+t(43)+t(52)));
		//       def_cmd_del(337)     => 'd' 'e' 'l'.
		q(nt(337), (t(53)+t(28)+t(27)));
		//       def_cmf_clear(338)   => 'c' 'l' 'e' 'a' 'r'.
		q(nt(338), (t(47)+t(27)+t(28)+t(26)+t(45)));
		//       memory_clear_cmd_sym(356) => 'c'.
		q(nt(356), (t(47)));
		//       memory_clear_cmd_sym(356) => 'c' 'l' 'e' 'a' 'r'.
		q(nt(356), (t(47)+t(27)+t(28)+t(26)+t(45)));
		//       memory_del_cmd_sym(357) => 'd'.
		q(nt(357), (t(53)));
		//       memory_del_cmd_sym(357) => 'd' 'e' 'l'.
		q(nt(357), (t(53)+t(28)+t(27)));
		//       memory_del_cmd_sym(357) => 'd' 'e' 'l' 'e' 't' 'e'.
		q(nt(357), (t(53)+t(28)+t(27)+t(28)+t(52)+t(28)));
		//       memory_cmd_sym(355)  => 'm'.
		q(nt(355), (t(46)));
		//       memory_cmd_sym(355)  => 'm' 'e' 'm'.
		q(nt(355), (t(46)+t(28)+t(46)));
		//       memory_cmd_sym(355)  => 'm' 'e' 'm' 'o' 'r' 'y'.
		q(nt(355), (t(46)+t(28)+t(46)+t(31)+t(45)+t(57)));
		//       memory_sym(352)      => 'm'.
		q(nt(352), (t(46)));
		//       absolute_memory_sym(351) => '@'.
		q(nt(351), (t(58)));
		//       relative_memory_sym(354) => '%'.
		q(nt(354), (t(59)));
		//       selection_sym(345)   => 's'.
		q(nt(345), (t(43)));
		//       selection_sym(345)   => 's' 'e' 'l' 'e' 'c' 't' 'i' 'o' 'n'.
		q(nt(345), (t(43)+t(28)+t(27)+t(28)+t(47)+t(52)+t(41)+t(31)+t(49)));
		//       examples_sym(361)    => 'e' 'x' 'a' 'm' 'p' 'l' 'e' 's'.
		q(nt(361), (t(28)+t(29)+t(26)+t(46)+t(56)+t(27)+t(28)+t(43)));
		//       __E_get_cmd_67(365)  => __(10) option(366).
		q(nt(365), (nt(10)+nt(366)));
		//       __E_get_cmd_67(365)  => null.
		q(nt(365), (nul));
		//       get_cmd(264)         => get_cmd_sym(362) __E_get_cmd_67(365).
		q(nt(264), (nt(362)+nt(365)));
		//       __E_set_cmd_68(367)  => __(10).
		q(nt(367), (nt(10)));
		//       __E_set_cmd_68(367)  => _(9) '=' _(9).
		q(nt(367), (nt(9)+t(19)+nt(9)));
		//       set_cmd(265)         => set_cmd_sym(363) __(10) option(366) __E_set_cmd_68(367) option_value(368).
		q(nt(265), (nt(363)+nt(10)+nt(366)+nt(367)+nt(368)));
		//       toggle_cmd(266)      => toggle_cmd_sym(364) __(10) bool_option(369).
		q(nt(266), (nt(364)+nt(10)+nt(369)));
		//       get_cmd_sym(362)     => 'g' 'e' 't'.
		q(nt(362), (t(50)+t(28)+t(52)));
		//       set_cmd_sym(363)     => 's' 'e' 't'.
		q(nt(363), (t(43)+t(28)+t(52)));
		//       toggle_cmd_sym(364)  => 't' 'o' 'g' 'g' 'l' 'e'.
		q(nt(364), (t(52)+t(31)+t(50)+t(50)+t(27)+t(28)));
		//       option(366)          => bool_option(369).
		q(nt(366), (nt(369)));
		//       option(366)          => severity_opt(370).
		q(nt(366), (nt(370)));
		//       bool_option(369)     => status_opt(371).
		q(nt(369), (nt(371)));
		//       bool_option(369)     => colors_opt(372).
		q(nt(369), (nt(372)));
		//       bool_option(369)     => debug_repl_opt(373).
		q(nt(369), (nt(373)));
		//       status_opt(371)      => 's'.
		q(nt(371), (t(43)));
		//       status_opt(371)      => 's' 't' 'a' 't' 'u' 's'.
		q(nt(371), (t(43)+t(52)+t(26)+t(52)+t(44)+t(43)));
		//       colors_opt(372)      => 'c'.
		q(nt(372), (t(47)));
		//       colors_opt(372)      => 'c' 'o' 'l' 'o' 'r'.
		q(nt(372), (t(47)+t(31)+t(27)+t(31)+t(45)));
		//       colors_opt(372)      => 'c' 'o' 'l' 'o' 'r' 's'.
		q(nt(372), (t(47)+t(31)+t(27)+t(31)+t(45)+t(43)));
		//       severity_opt(370)    => 's' 'e' 'v'.
		q(nt(370), (t(43)+t(28)+t(51)));
		//       severity_opt(370)    => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
		q(nt(370), (t(43)+t(28)+t(51)+t(28)+t(45)+t(41)+t(52)+t(57)));
		//       debug_repl_opt(373)  => 'd'.
		q(nt(373), (t(53)));
		//       debug_repl_opt(373)  => 'd' 'r'.
		q(nt(373), (t(53)+t(45)));
		//       debug_repl_opt(373)  => 'd' 'b' 'g'.
		q(nt(373), (t(53)+t(30)+t(50)));
		//       debug_repl_opt(373)  => 'd' 'e' 'b' 'u' 'g'.
		q(nt(373), (t(53)+t(28)+t(30)+t(44)+t(50)));
		//       debug_repl_opt(373)  => 'd' 'e' 'b' 'u' 'g' '-' 'r' 'e' 'p' 'l'.
		q(nt(373), (t(53)+t(28)+t(30)+t(44)+t(50)+t(12)+t(45)+t(28)+t(56)+t(27)));
		//       option_value(368)    => option_value_true(374).
		q(nt(368), (nt(374)));
		//       option_value(368)    => option_value_false(375).
		q(nt(368), (nt(375)));
		//       option_value(368)    => severity(376).
		q(nt(368), (nt(376)));
		//       option_value_true(374) => 't'.
		q(nt(374), (t(52)));
		//       option_value_true(374) => 't' 'r' 'u' 'e'.
		q(nt(374), (t(52)+t(45)+t(44)+t(28)));
		//       option_value_true(374) => 'o' 'n'.
		q(nt(374), (t(31)+t(49)));
		//       option_value_true(374) => '1'.
		q(nt(374), (t(38)));
		//       option_value_true(374) => 'y'.
		q(nt(374), (t(57)));
		//       option_value_true(374) => 'y' 'e' 's'.
		q(nt(374), (t(57)+t(28)+t(43)));
		//       option_value_false(375) => 'f'.
		q(nt(375), (t(36)));
		//       option_value_false(375) => 'f' 'a' 'l' 's' 'e'.
		q(nt(375), (t(36)+t(26)+t(27)+t(43)+t(28)));
		//       option_value_false(375) => 'o' 'f' 'f'.
		q(nt(375), (t(31)+t(36)+t(36)));
		//       option_value_false(375) => '0'.
		q(nt(375), (t(39)));
		//       option_value_false(375) => 'n'.
		q(nt(375), (t(49)));
		//       option_value_false(375) => 'n' 'o'.
		q(nt(375), (t(49)+t(31)));
		//       severity(376)        => error_sym(377).
		q(nt(376), (nt(377)));
		//       severity(376)        => debug_sym(378).
		q(nt(376), (nt(378)));
		//       severity(376)        => trace_sym(379).
		q(nt(376), (nt(379)));
		//       severity(376)        => info_sym(380).
		q(nt(376), (nt(380)));
		//       error_sym(377)       => 'e'.
		q(nt(377), (t(28)));
		//       error_sym(377)       => 'e' 'r' 'r' 'o' 'r'.
		q(nt(377), (t(28)+t(45)+t(45)+t(31)+t(45)));
		//       info_sym(380)        => 'i'.
		q(nt(380), (t(41)));
		//       info_sym(380)        => 'i' 'n' 'f' 'o'.
		q(nt(380), (t(41)+t(49)+t(36)+t(31)));
		//       debug_sym(378)       => 'd'.
		q(nt(378), (t(53)));
		//       debug_sym(378)       => 'd' 'e' 'b' 'u' 'g'.
		q(nt(378), (t(53)+t(28)+t(30)+t(44)+t(50)));
		//       trace_sym(379)       => 't'.
		q(nt(379), (t(52)));
		//       trace_sym(379)       => 't' 'r' 'a' 'c' 'e'.
		q(nt(379), (t(52)+t(45)+t(26)+t(47)+t(28)));
		return q;
	}
};

#endif // __TAU_PARSER_H__
