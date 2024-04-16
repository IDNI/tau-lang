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
		g(nts, load_prods(), nt(246), cc, load_grammar_opts()),
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
		wff_bex, wff_all, wff_ex, wff_imply, wff_equiv, bf_interval, bf_neq, bf_eq, bf_not_less_equal, bf_greater, 
		bf_less_equal, bf_less, wff_or, wff_and, wff_xor, wff_neg, __E_wff_ref_19, wff_ref_args, __E_wff_ref_args_20, wff_ref_arg, 
		__E_wff_ref_args_21, __E_wff_all_22, __E_wff_ex_23, __E_wff_ball_24, __E_wff_bex_25, bf_rule, bf_matcher, bf_body, bf_is_zero_cb, bf_is_one_cb, 
		bf_has_subformula_cb, bf_remove_funiversal_cb, bf_remove_fexistential_cb, bf_and_cb, bf_or_cb, bf_xor_cb, bf_neg_cb, bf_splitter, bf_ref, bf_all, 
		bf_ex, bf_or, bf_and, bf_xor, bf_neg, bf_constant, bf_rec_relation, __E_bf_ref_26, bf_ref_args, __E_bf_ref_args_27, 
		bf_ref_arg, __E_bf_ref_args_28, __E_bf_and_29, __E_bf_all_30, __E_bf_ex_31, constant, binding, source_binding, named_binding, type, 
		source, source0, __E_source_32, __E_source_33, bf_and_cb_sym, bf_cb_arg, bf_or_cb_sym, bf_xor_cb_sym, bf_neg_cb_sym, bf_eq_cb_sym, 
		wff_cb_arg, bf_neq_cb_sym, bf_is_zero_cb_sym, bf_is_one_cb_sym, bf_remove_funiversal_cb_sym, bf_remove_fexistential_cb_sym, wff_remove_existential_cb_sym, wff_remove_bexistential_cb_sym, wff_remove_buniversal_cb_sym, wff_has_clashing_subformulas_cb_sym, 
		bf_has_subformula_cb_sym, wff_has_subformula_cb_sym, __E_tau_collapse_positives_cb_34, tau_collapse_positives_cb_sym, tau_cb_arg, __E_tau_collapse_positives_cb_35, tau_positives_upwards_cb_sym, inputs, input, __E_inputs_36, 
		__E_inputs_37, builder, builder_head, builder_body, __E_builder_head_38, __E_builder_head_39, bf_builder_body, wff_builder_body, tau_builder_body, library, 
		rules, __E_rules_40, rule, __E_rules_41, nso_rr, nso_rec_relations, nso_main, __E_nso_rec_relations_42, nso_rec_relation, __E_nso_rec_relations_43, 
		gssotc_rr, gssotc_rec_relations, gssotc_main, __E_gssotc_rec_relations_44, gssotc_rec_relation, __E_gssotc_rec_relations_45, start, __E_start_46, cli, cli_command, 
		__E_cli_47, __E_cli_48, help_cmd, version_cmd, quit_cmd, clear_cmd, get_cmd, set_cmd, toggle_cmd, list_outputs_cmd, 
		clear_outputs_cmd, print_output_cmd, file, normalize_cmd, bf_dnf_cmd, bf_cnf_cmd, bf_anf_cmd, bf_nnf_cmd, bf_pnf_cmd, bf_mnf_cmd, 
		bf_substitute, wff_substitute, bf_instantiate, wff_instantiate, wff_onf_cmd, wff_dnf_cmd, wff_cnf_cmd, wff_anf_cmd, wff_nnf_cmd, wff_pnf_cmd, 
		wff_mnf_cmd, def_wff_cmd, def_bf_cmd, def_tau_cmd, def_list_cmd, def_del_cmd, file_sym, file_path, normalize_cmd_sym, normalize_cmd_arg, 
		output, wff_selection, bf_instantiate_cmd, instantiate_cmd_sym, __E_bf_instantiate_cmd_49, bf_selection, __E_bf_instantiate_cmd_50, wff_instantiate_cmd, __E_wff_instantiate_cmd_51, __E_wff_instantiate_cmd_52, 
		bf_substitute_cmd, substitute_cmd_sym, __E_bf_substitute_cmd_53, __E_bf_substitute_cmd_54, __E_bf_substitute_cmd_55, wff_substitute_cmd, substitutecmd__sym, __E_wff_substitute_cmd_56, __E_wff_substitute_cmd_57, __E_wff_substitute_cmd_58, 
		dnf_cmd_sym, __E_bf_dnf_cmd_59, cnf_cmd_sym, __E_bf_cnf_cmd_60, anf_cmd_sym, __E_bf_anf_cmd_61, nnf_cmd_sym, __E_bf_nnf_cmd_62, pnf_cmd_sym, __E_bf_pnf_cmd_63, 
		mnf_cmd_sym, __E_bf_mnf_cmd_64, onf_cmd_sym, __E_wff_onf_cmd_65, __E_wff_dnf_cmd_66, __E_wff_cnf_cmd_67, __E_wff_anf_cmd_68, __E_wff_nnf_cmd_69, __E_wff_pnf_cmd_70, __E_wff_mnf_cmd_71, 
		def_cmd_sym, def_clear_cmd, help_cmd_sym, __E_help_cmd_72, cli_cmd_sym, version_cmd_sym, quit_cmd_sym, clear_cmd_sym, selection_sym, bf_var_selection, 
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
			"", "eof", "space", "digit", "xdigit", "alpha", "alnum", "punct", "printable", "comment", 
			"__E_comment_0", "__E_comment_1", "__E_comment_2", "__", "__E____3", "_", "__E___4", "dot", "open_parenthesis", "close_parenthesis", 
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
			"wff_bex", "wff_all", "wff_ex", "wff_imply", "wff_equiv", "bf_interval", "bf_neq", "bf_eq", "bf_not_less_equal", "bf_greater", 
			"bf_less_equal", "bf_less", "wff_or", "wff_and", "wff_xor", "wff_neg", "__E_wff_ref_19", "wff_ref_args", "__E_wff_ref_args_20", "wff_ref_arg", 
			"__E_wff_ref_args_21", "__E_wff_all_22", "__E_wff_ex_23", "__E_wff_ball_24", "__E_wff_bex_25", "bf_rule", "bf_matcher", "bf_body", "bf_is_zero_cb", "bf_is_one_cb", 
			"bf_has_subformula_cb", "bf_remove_funiversal_cb", "bf_remove_fexistential_cb", "bf_and_cb", "bf_or_cb", "bf_xor_cb", "bf_neg_cb", "bf_splitter", "bf_ref", "bf_all", 
			"bf_ex", "bf_or", "bf_and", "bf_xor", "bf_neg", "bf_constant", "bf_rec_relation", "__E_bf_ref_26", "bf_ref_args", "__E_bf_ref_args_27", 
			"bf_ref_arg", "__E_bf_ref_args_28", "__E_bf_and_29", "__E_bf_all_30", "__E_bf_ex_31", "constant", "binding", "source_binding", "named_binding", "type", 
			"source", "source0", "__E_source_32", "__E_source_33", "bf_and_cb_sym", "bf_cb_arg", "bf_or_cb_sym", "bf_xor_cb_sym", "bf_neg_cb_sym", "bf_eq_cb_sym", 
			"wff_cb_arg", "bf_neq_cb_sym", "bf_is_zero_cb_sym", "bf_is_one_cb_sym", "bf_remove_funiversal_cb_sym", "bf_remove_fexistential_cb_sym", "wff_remove_existential_cb_sym", "wff_remove_bexistential_cb_sym", "wff_remove_buniversal_cb_sym", "wff_has_clashing_subformulas_cb_sym", 
			"bf_has_subformula_cb_sym", "wff_has_subformula_cb_sym", "__E_tau_collapse_positives_cb_34", "tau_collapse_positives_cb_sym", "tau_cb_arg", "__E_tau_collapse_positives_cb_35", "tau_positives_upwards_cb_sym", "inputs", "input", "__E_inputs_36", 
			"__E_inputs_37", "builder", "builder_head", "builder_body", "__E_builder_head_38", "__E_builder_head_39", "bf_builder_body", "wff_builder_body", "tau_builder_body", "library", 
			"rules", "__E_rules_40", "rule", "__E_rules_41", "nso_rr", "nso_rec_relations", "nso_main", "__E_nso_rec_relations_42", "nso_rec_relation", "__E_nso_rec_relations_43", 
			"gssotc_rr", "gssotc_rec_relations", "gssotc_main", "__E_gssotc_rec_relations_44", "gssotc_rec_relation", "__E_gssotc_rec_relations_45", "start", "__E_start_46", "cli", "cli_command", 
			"__E_cli_47", "__E_cli_48", "help_cmd", "version_cmd", "quit_cmd", "clear_cmd", "get_cmd", "set_cmd", "toggle_cmd", "list_outputs_cmd", 
			"clear_outputs_cmd", "print_output_cmd", "file", "normalize_cmd", "bf_dnf_cmd", "bf_cnf_cmd", "bf_anf_cmd", "bf_nnf_cmd", "bf_pnf_cmd", "bf_mnf_cmd", 
			"bf_substitute", "wff_substitute", "bf_instantiate", "wff_instantiate", "wff_onf_cmd", "wff_dnf_cmd", "wff_cnf_cmd", "wff_anf_cmd", "wff_nnf_cmd", "wff_pnf_cmd", 
			"wff_mnf_cmd", "def_wff_cmd", "def_bf_cmd", "def_tau_cmd", "def_list_cmd", "def_del_cmd", "file_sym", "file_path", "normalize_cmd_sym", "normalize_cmd_arg", 
			"output", "wff_selection", "bf_instantiate_cmd", "instantiate_cmd_sym", "__E_bf_instantiate_cmd_49", "bf_selection", "__E_bf_instantiate_cmd_50", "wff_instantiate_cmd", "__E_wff_instantiate_cmd_51", "__E_wff_instantiate_cmd_52", 
			"bf_substitute_cmd", "substitute_cmd_sym", "__E_bf_substitute_cmd_53", "__E_bf_substitute_cmd_54", "__E_bf_substitute_cmd_55", "wff_substitute_cmd", "substitutecmd__sym", "__E_wff_substitute_cmd_56", "__E_wff_substitute_cmd_57", "__E_wff_substitute_cmd_58", 
			"dnf_cmd_sym", "__E_bf_dnf_cmd_59", "cnf_cmd_sym", "__E_bf_cnf_cmd_60", "anf_cmd_sym", "__E_bf_anf_cmd_61", "nnf_cmd_sym", "__E_bf_nnf_cmd_62", "pnf_cmd_sym", "__E_bf_pnf_cmd_63", 
			"mnf_cmd_sym", "__E_bf_mnf_cmd_64", "onf_cmd_sym", "__E_wff_onf_cmd_65", "__E_wff_dnf_cmd_66", "__E_wff_cnf_cmd_67", "__E_wff_anf_cmd_68", "__E_wff_nnf_cmd_69", "__E_wff_pnf_cmd_70", "__E_wff_mnf_cmd_71", 
			"def_cmd_sym", "def_clear_cmd", "help_cmd_sym", "__E_help_cmd_72", "cli_cmd_sym", "version_cmd_sym", "quit_cmd_sym", "clear_cmd_sym", "selection_sym", "bf_var_selection", 
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
		//       __E_offsets_8(76)    => _(15) comma(28) _(15) offset(75).
		q(nt(76), (nt(15)+nt(28)+nt(15)+nt(75)));
		//       __E_offsets_9(77)    => null.
		q(nt(77), (nul));
		//       __E_offsets_9(77)    => __E_offsets_8(76) __E_offsets_9(77).
		q(nt(77), (nt(76)+nt(77)));
		//       offsets(74)          => open_bracket(20) _(15) offset(75) __E_offsets_9(77) _(15) close_bracket(21).
		q(nt(74), (nt(20)+nt(15)+nt(75)+nt(77)+nt(15)+nt(21)));
		//       offset(75)           => num(78).
		q(nt(75), (nt(78)));
		//       offset(75)           => capture(79).
		q(nt(75), (nt(79)));
		//       offset(75)           => shift(80).
		q(nt(75), (nt(80)));
		//       __N_0(372)           => io_var(83).
		q(nt(372), (nt(83)));
		//       __E_offset_10(81)    => variable(82) & ~( __N_0(372) ).	 # conjunctive
		q(nt(81), (nt(82)) & ~(nt(372)));
		//       offset(75)           => __E_offset_10(81).
		q(nt(75), (nt(81)));
		//       __E_shift_11(84)     => capture(79).
		q(nt(84), (nt(79)));
		//       __N_1(373)           => io_var(83).
		q(nt(373), (nt(83)));
		//       __E___E_shift_11_12(85) => variable(82) & ~( __N_1(373) ).	 # conjunctive
		q(nt(85), (nt(82)) & ~(nt(373)));
		//       __E_shift_11(84)     => __E___E_shift_11_12(85).
		q(nt(84), (nt(85)));
		//       shift(80)            => __E_shift_11(84) _(15) minus(24) _(15) num(78).
		q(nt(80), (nt(84)+nt(15)+nt(24)+nt(15)+nt(78)));
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
		//       __N_2(374)           => 'F'.
		q(nt(374), (t(34)));
		//       __N_3(375)           => 'T'.
		q(nt(375), (t(33)));
		//       __E_charvar_13(92)   => ~( __N_2(374) ) & ~( __N_3(375) ) & alpha(5).	 # conjunctive
		q(nt(92), ~(nt(374)) & ~(nt(375)) & (nt(5)));
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
		//       tau_rule(97)         => tau_matcher(98) _(15) tau_def(31) _(15) tau_body(99) _(15) dot(17).
		q(nt(97), (nt(98)+nt(15)+nt(31)+nt(15)+nt(99)+nt(15)+nt(17)));
		//       tau_matcher(98)      => tau(100).
		q(nt(98), (nt(100)));
		//       tau_body(99)         => tau(100).
		q(nt(99), (nt(100)));
		//       tau_body(99)         => tau_collapse_positives_cb(101).
		q(nt(99), (nt(101)));
		//       tau_body(99)         => tau_positives_upwards_cb(102).
		q(nt(99), (nt(102)));
		//       tau_rec_relation(103) => tau_ref(104) _(15) tau_def(31) _(15) tau(100) _(15) dot(17).
		q(nt(103), (nt(104)+nt(15)+nt(31)+nt(15)+nt(100)+nt(15)+nt(17)));
		//       __E_tau_ref_16(105)  => offsets(74).
		q(nt(105), (nt(74)));
		//       __E_tau_ref_16(105)  => null.
		q(nt(105), (nul));
		//       tau_ref(104)         => sym(73) _(15) __E_tau_ref_16(105) _(15) tau_ref_args(106).
		q(nt(104), (nt(73)+nt(15)+nt(105)+nt(15)+nt(106)));
		//       __E_tau_ref_args_17(107) => _(15) tau_ref_arg(108).
		q(nt(107), (nt(15)+nt(108)));
		//       __E_tau_ref_args_18(109) => null.
		q(nt(109), (nul));
		//       __E_tau_ref_args_18(109) => __E_tau_ref_args_17(107) __E_tau_ref_args_18(109).
		q(nt(109), (nt(107)+nt(109)));
		//       tau_ref_args(106)    => open_parenthesis(18) __E_tau_ref_args_18(109) _(15) close_parenthesis(19).
		q(nt(106), (nt(18)+nt(109)+nt(15)+nt(19)));
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
		//       tau(100)             => open_parenthesis(18) _(15) tau(100) _(15) close_parenthesis(19).
		q(nt(100), (nt(18)+nt(15)+nt(100)+nt(15)+nt(19)));
		//       tau_and(112)         => tau(100) _(15) tau_and_sym(33) _(15) tau(100).
		q(nt(112), (nt(100)+nt(15)+nt(33)+nt(15)+nt(100)));
		//       tau_or(111)          => tau(100) _(15) tau_or_sym(34) _(15) tau(100).
		q(nt(111), (nt(100)+nt(15)+nt(34)+nt(15)+nt(100)));
		//       tau_neg(113)         => tau_neg_sym(35) _(15) tau(100).
		q(nt(113), (nt(35)+nt(15)+nt(100)));
		//       tau_wff(114)         => open_brace(22) _(15) wff(115) _(15) close_brace(23).
		q(nt(114), (nt(22)+nt(15)+nt(115)+nt(15)+nt(23)));
		//       wff_rule(116)        => wff_matcher(117) _(15) wff_def(36) _(15) wff_body(118) _(15) dot(17).
		q(nt(116), (nt(117)+nt(15)+nt(36)+nt(15)+nt(118)+nt(15)+nt(17)));
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
		//       wff_rec_relation(126) => wff_ref(127) _(15) wff_def(36) _(15) wff(115) _(15) dot(17).
		q(nt(126), (nt(127)+nt(15)+nt(36)+nt(15)+nt(115)+nt(15)+nt(17)));
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
		//       wff(115)             => bf_interval(135).
		q(nt(115), (nt(135)));
		//       wff(115)             => bf_neq(136).
		q(nt(115), (nt(136)));
		//       wff(115)             => bf_eq(137).
		q(nt(115), (nt(137)));
		//       wff(115)             => bf_not_less_equal(138).
		q(nt(115), (nt(138)));
		//       wff(115)             => bf_greater(139).
		q(nt(115), (nt(139)));
		//       wff(115)             => bf_less_equal(140).
		q(nt(115), (nt(140)));
		//       wff(115)             => bf_less(141).
		q(nt(115), (nt(141)));
		//       wff(115)             => wff_or(142).
		q(nt(115), (nt(142)));
		//       wff(115)             => wff_and(143).
		q(nt(115), (nt(143)));
		//       wff(115)             => wff_xor(144).
		q(nt(115), (nt(144)));
		//       wff(115)             => wff_neg(145).
		q(nt(115), (nt(145)));
		//       wff(115)             => wff_t(49).
		q(nt(115), (nt(49)));
		//       wff(115)             => wff_f(50).
		q(nt(115), (nt(50)));
		//       wff(115)             => capture(79).
		q(nt(115), (nt(79)));
		//       wff(115)             => bool_variable(91).
		q(nt(115), (nt(91)));
		//       wff(115)             => open_parenthesis(18) _(15) wff(115) _(15) close_parenthesis(19).
		q(nt(115), (nt(18)+nt(15)+nt(115)+nt(15)+nt(19)));
		//       __E_wff_ref_19(146)  => offsets(74).
		q(nt(146), (nt(74)));
		//       __E_wff_ref_19(146)  => null.
		q(nt(146), (nul));
		//       wff_ref(127)         => sym(73) _(15) __E_wff_ref_19(146) _(15) wff_ref_args(147).
		q(nt(127), (nt(73)+nt(15)+nt(146)+nt(15)+nt(147)));
		//       __E_wff_ref_args_20(148) => _(15) wff_ref_arg(149).
		q(nt(148), (nt(15)+nt(149)));
		//       __E_wff_ref_args_21(150) => null.
		q(nt(150), (nul));
		//       __E_wff_ref_args_21(150) => __E_wff_ref_args_20(148) __E_wff_ref_args_21(150).
		q(nt(150), (nt(148)+nt(150)));
		//       wff_ref_args(147)    => open_parenthesis(18) __E_wff_ref_args_21(150) _(15) close_parenthesis(19).
		q(nt(147), (nt(18)+nt(150)+nt(15)+nt(19)));
		//       wff_ref_arg(149)     => bf(110).
		q(nt(149), (nt(110)));
		//       wff_ref_arg(149)     => capture(79).
		q(nt(149), (nt(79)));
		//       wff_ref_arg(149)     => variable(82).
		q(nt(149), (nt(82)));
		//       wff_and(143)         => wff(115) _(15) wff_and_sym(38) _(15) wff(115).
		q(nt(143), (nt(115)+nt(15)+nt(38)+nt(15)+nt(115)));
		//       wff_or(142)          => wff(115) _(15) wff_or_sym(39) _(15) wff(115).
		q(nt(142), (nt(115)+nt(15)+nt(39)+nt(15)+nt(115)));
		//       wff_xor(144)         => wff(115) _(15) wff_xor_sym(40) _(15) wff(115).
		q(nt(144), (nt(115)+nt(15)+nt(40)+nt(15)+nt(115)));
		//       wff_conditional(128) => wff(115) _(15) wff_conditional_sym(41) _(15) wff(115) _(15) colon(25) _(15) wff(115).
		q(nt(128), (nt(115)+nt(15)+nt(41)+nt(15)+nt(115)+nt(15)+nt(25)+nt(15)+nt(115)));
		//       wff_neg(145)         => wff_neg_sym(42) _(15) wff(115).
		q(nt(145), (nt(42)+nt(15)+nt(115)));
		//       wff_imply(133)       => wff(115) _(15) wff_imply_sym(43) _(15) wff(115).
		q(nt(133), (nt(115)+nt(15)+nt(43)+nt(15)+nt(115)));
		//       wff_equiv(134)       => wff(115) _(15) wff_equiv_sym(44) _(15) wff(115).
		q(nt(134), (nt(115)+nt(15)+nt(44)+nt(15)+nt(115)));
		//       __E_wff_all_22(151)  => variable(82).
		q(nt(151), (nt(82)));
		//       __E_wff_all_22(151)  => capture(79).
		q(nt(151), (nt(79)));
		//       wff_all(131)         => wff_all_sym(45) __(13) __E_wff_all_22(151) __(13) wff(115).
		q(nt(131), (nt(45)+nt(13)+nt(151)+nt(13)+nt(115)));
		//       __E_wff_ex_23(152)   => variable(82).
		q(nt(152), (nt(82)));
		//       __E_wff_ex_23(152)   => capture(79).
		q(nt(152), (nt(79)));
		//       wff_ex(132)          => wff_ex_sym(46) __(13) __E_wff_ex_23(152) __(13) wff(115).
		q(nt(132), (nt(46)+nt(13)+nt(152)+nt(13)+nt(115)));
		//       __E_wff_ball_24(153) => bool_variable(91).
		q(nt(153), (nt(91)));
		//       __E_wff_ball_24(153) => capture(79).
		q(nt(153), (nt(79)));
		//       wff_ball(129)        => wff_ball_sym(47) __(13) __E_wff_ball_24(153) __(13) wff(115).
		q(nt(129), (nt(47)+nt(13)+nt(153)+nt(13)+nt(115)));
		//       __E_wff_bex_25(154)  => bool_variable(91).
		q(nt(154), (nt(91)));
		//       __E_wff_bex_25(154)  => capture(79).
		q(nt(154), (nt(79)));
		//       wff_bex(130)         => wff_bex_sym(48) __(13) __E_wff_bex_25(154) __(13) wff(115).
		q(nt(130), (nt(48)+nt(13)+nt(154)+nt(13)+nt(115)));
		//       bf_eq(137)           => bf(110) _(15) bf_equality_sym(57) _(15) bf(110).
		q(nt(137), (nt(110)+nt(15)+nt(57)+nt(15)+nt(110)));
		//       bf_neq(136)          => bf(110) _(15) bf_nequality_sym(58) _(15) bf(110).
		q(nt(136), (nt(110)+nt(15)+nt(58)+nt(15)+nt(110)));
		//       bf_less(141)         => bf(110) _(15) bf_less_sym(59) _(15) bf(110).
		q(nt(141), (nt(110)+nt(15)+nt(59)+nt(15)+nt(110)));
		//       bf_less_equal(140)   => bf(110) _(15) bf_less_equal_sym(60) _(15) bf(110).
		q(nt(140), (nt(110)+nt(15)+nt(60)+nt(15)+nt(110)));
		//       bf_not_less_equal(138) => bf(110) _(15) bf_not_less_equal_sym(61) _(15) bf(110).
		q(nt(138), (nt(110)+nt(15)+nt(61)+nt(15)+nt(110)));
		//       bf_greater(139)      => bf(110) _(15) bf_greater_sym(62) _(15) bf(110).
		q(nt(139), (nt(110)+nt(15)+nt(62)+nt(15)+nt(110)));
		//       bf_interval(135)     => bf(110) _(15) bf_less_equal_sym(60) _(15) bf(110) _(15) bf_less_equal_sym(60) _(15) bf(110).
		q(nt(135), (nt(110)+nt(15)+nt(60)+nt(15)+nt(110)+nt(15)+nt(60)+nt(15)+nt(110)));
		//       bf_rule(155)         => bf_matcher(156) _(15) bf_def(51) _(15) bf_body(157) _(15) dot(17).
		q(nt(155), (nt(156)+nt(15)+nt(51)+nt(15)+nt(157)+nt(15)+nt(17)));
		//       bf_matcher(156)      => bf(110).
		q(nt(156), (nt(110)));
		//       bf_body(157)         => bf(110).
		q(nt(157), (nt(110)));
		//       bf_body(157)         => bf_is_zero_cb(158).
		q(nt(157), (nt(158)));
		//       bf_body(157)         => bf_is_one_cb(159).
		q(nt(157), (nt(159)));
		//       bf_body(157)         => bf_has_subformula_cb(160).
		q(nt(157), (nt(160)));
		//       bf_body(157)         => bf_remove_funiversal_cb(161).
		q(nt(157), (nt(161)));
		//       bf_body(157)         => bf_remove_fexistential_cb(162).
		q(nt(157), (nt(162)));
		//       bf_body(157)         => bf_and_cb(163).
		q(nt(157), (nt(163)));
		//       bf_body(157)         => bf_or_cb(164).
		q(nt(157), (nt(164)));
		//       bf_body(157)         => bf_xor_cb(165).
		q(nt(157), (nt(165)));
		//       bf_body(157)         => bf_neg_cb(166).
		q(nt(157), (nt(166)));
		//       bf_body(157)         => bf_not_less_equal(138).
		q(nt(157), (nt(138)));
		//       bf(110)              => bf_splitter(167).
		q(nt(110), (nt(167)));
		//       bf(110)              => bf_ref(168).
		q(nt(110), (nt(168)));
		//       bf(110)              => bf_all(169).
		q(nt(110), (nt(169)));
		//       bf(110)              => bf_ex(170).
		q(nt(110), (nt(170)));
		//       bf(110)              => bf_or(171).
		q(nt(110), (nt(171)));
		//       bf(110)              => bf_and(172).
		q(nt(110), (nt(172)));
		//       bf(110)              => bf_xor(173).
		q(nt(110), (nt(173)));
		//       bf(110)              => bf_neg(174).
		q(nt(110), (nt(174)));
		//       bf(110)              => bf_constant(175).
		q(nt(110), (nt(175)));
		//       bf(110)              => bf_t(66).
		q(nt(110), (nt(66)));
		//       bf(110)              => bf_f(67).
		q(nt(110), (nt(67)));
		//       bf(110)              => capture(79).
		q(nt(110), (nt(79)));
		//       bf(110)              => variable(82).
		q(nt(110), (nt(82)));
		//       bf(110)              => open_parenthesis(18) _(15) bf(110) _(15) close_parenthesis(19).
		q(nt(110), (nt(18)+nt(15)+nt(110)+nt(15)+nt(19)));
		//       bf_rec_relation(176) => bf_ref(168) _(15) bf_def(51) _(15) bf(110) _(15) dot(17).
		q(nt(176), (nt(168)+nt(15)+nt(51)+nt(15)+nt(110)+nt(15)+nt(17)));
		//       __E_bf_ref_26(177)   => offsets(74).
		q(nt(177), (nt(74)));
		//       __E_bf_ref_26(177)   => null.
		q(nt(177), (nul));
		//       bf_ref(168)          => sym(73) __E_bf_ref_26(177) bf_ref_args(178).
		q(nt(168), (nt(73)+nt(177)+nt(178)));
		//       __E_bf_ref_args_27(179) => _(15) bf_ref_arg(180).
		q(nt(179), (nt(15)+nt(180)));
		//       __E_bf_ref_args_28(181) => null.
		q(nt(181), (nul));
		//       __E_bf_ref_args_28(181) => __E_bf_ref_args_27(179) __E_bf_ref_args_28(181).
		q(nt(181), (nt(179)+nt(181)));
		//       bf_ref_args(178)     => open_parenthesis(18) __E_bf_ref_args_28(181) _(15) close_parenthesis(19).
		q(nt(178), (nt(18)+nt(181)+nt(15)+nt(19)));
		//       bf_ref_arg(180)      => bf(110).
		q(nt(180), (nt(110)));
		//       bf_ref_arg(180)      => capture(79).
		q(nt(180), (nt(79)));
		//       bf_ref_arg(180)      => variable(82).
		q(nt(180), (nt(82)));
		//       __E_bf_and_29(182)   => bf_and_sym(53) _(15).
		q(nt(182), (nt(53)+nt(15)));
		//       __E_bf_and_29(182)   => null.
		q(nt(182), (nul));
		//       bf_and(172)          => bf(110) _(15) __E_bf_and_29(182) bf(110).
		q(nt(172), (nt(110)+nt(15)+nt(182)+nt(110)));
		//       bf_or(171)           => bf(110) _(15) bf_or_sym(54) _(15) bf(110).
		q(nt(171), (nt(110)+nt(15)+nt(54)+nt(15)+nt(110)));
		//       bf_xor(173)          => bf(110) _(15) bf_xor_sym(55) _(15) bf(110).
		q(nt(173), (nt(110)+nt(15)+nt(55)+nt(15)+nt(110)));
		//       bf_neg(174)          => bf(110) _(15) bf_neg_sym(56).
		q(nt(174), (nt(110)+nt(15)+nt(56)));
		//       __E_bf_all_30(183)   => variable(82).
		q(nt(183), (nt(82)));
		//       __E_bf_all_30(183)   => capture(79).
		q(nt(183), (nt(79)));
		//       bf_all(169)          => bf_all_sym(63) __(13) __E_bf_all_30(183) __(13) bf(110).
		q(nt(169), (nt(63)+nt(13)+nt(183)+nt(13)+nt(110)));
		//       __E_bf_ex_31(184)    => variable(82).
		q(nt(184), (nt(82)));
		//       __E_bf_ex_31(184)    => capture(79).
		q(nt(184), (nt(79)));
		//       bf_ex(170)           => bf_ex_sym(64) __(13) __E_bf_ex_31(184) __(13) bf(110).
		q(nt(170), (nt(64)+nt(13)+nt(184)+nt(13)+nt(110)));
		//       bf_splitter(167)     => bf_splitter_sym(65) _(15) open_parenthesis(18) _(15) bf(110) _(15) close_parenthesis(19).
		q(nt(167), (nt(65)+nt(15)+nt(18)+nt(15)+nt(110)+nt(15)+nt(19)));
		//       bf_constant(175)     => open_brace(22) _(15) constant(185) _(15) close_brace(23).
		q(nt(175), (nt(22)+nt(15)+nt(185)+nt(15)+nt(23)));
		//       constant(185)        => binding(186).
		q(nt(185), (nt(186)));
		//       constant(185)        => capture(79).
		q(nt(185), (nt(79)));
		//       binding(186)         => source_binding(187).
		q(nt(186), (nt(187)));
		//       binding(186)         => named_binding(188).
		q(nt(186), (nt(188)));
		//       named_binding(188)   => chars(68).
		q(nt(188), (nt(68)));
		//       source_binding(187)  => type(189) _(15) colon(25) _(15) source(190).
		q(nt(187), (nt(189)+nt(15)+nt(25)+nt(15)+nt(190)));
		//       type(189)            => chars(68).
		q(nt(189), (nt(68)));
		//       type(189)            => null.
		q(nt(189), (nul));
		//       source0(191)         => alnum(6).
		q(nt(191), (nt(6)));
		//       source0(191)         => space(2).
		q(nt(191), (nt(2)));
		//       source0(191)         => punct(7).
		q(nt(191), (nt(7)));
		//       __E_source_32(192)   => source0(191).
		q(nt(192), (nt(191)));
		//       __E_source_33(193)   => __E_source_32(192).
		q(nt(193), (nt(192)));
		//       __E_source_33(193)   => __E_source_32(192) __E_source_33(193).
		q(nt(193), (nt(192)+nt(193)));
		//       source(190)          => __E_source_33(193).
		q(nt(190), (nt(193)));
		//       bf_and_cb(163)       => bf_and_cb_sym(194) __(13) bf_cb_arg(195) __(13) bf_cb_arg(195).
		q(nt(163), (nt(194)+nt(13)+nt(195)+nt(13)+nt(195)));
		//       bf_or_cb(164)        => bf_or_cb_sym(196) __(13) bf_cb_arg(195) __(13) bf_cb_arg(195).
		q(nt(164), (nt(196)+nt(13)+nt(195)+nt(13)+nt(195)));
		//       bf_xor_cb(165)       => bf_xor_cb_sym(197) __(13) bf_cb_arg(195) __(13) bf_cb_arg(195).
		q(nt(165), (nt(197)+nt(13)+nt(195)+nt(13)+nt(195)));
		//       bf_neg_cb(166)       => bf_neg_cb_sym(198) __(13) bf_cb_arg(195).
		q(nt(166), (nt(198)+nt(13)+nt(195)));
		//       bf_eq_cb(119)        => bf_eq_cb_sym(199) __(13) bf_cb_arg(195) __(13) wff_cb_arg(200) __(13) wff_cb_arg(200).
		q(nt(119), (nt(199)+nt(13)+nt(195)+nt(13)+nt(200)+nt(13)+nt(200)));
		//       bf_neq_cb(120)       => bf_neq_cb_sym(201) __(13) bf_cb_arg(195) __(13) wff_cb_arg(200) __(13) wff_cb_arg(200).
		q(nt(120), (nt(201)+nt(13)+nt(195)+nt(13)+nt(200)+nt(13)+nt(200)));
		//       bf_is_zero_cb(158)   => bf_is_zero_cb_sym(202) __(13) bf_cb_arg(195) __(13) bf_cb_arg(195).
		q(nt(158), (nt(202)+nt(13)+nt(195)+nt(13)+nt(195)));
		//       bf_is_one_cb(159)    => bf_is_one_cb_sym(203) __(13) bf_cb_arg(195) __(13) bf_cb_arg(195).
		q(nt(159), (nt(203)+nt(13)+nt(195)+nt(13)+nt(195)));
		//       bf_remove_funiversal_cb(161) => bf_remove_funiversal_cb_sym(204) __(13) bf_cb_arg(195) __(13) bf_cb_arg(195) __(13) bf_cb_arg(195) __(13) bf_cb_arg(195).
		q(nt(161), (nt(204)+nt(13)+nt(195)+nt(13)+nt(195)+nt(13)+nt(195)+nt(13)+nt(195)));
		//       bf_remove_fexistential_cb(162) => bf_remove_fexistential_cb_sym(205) __(13) bf_cb_arg(195) __(13) bf_cb_arg(195) __(13) bf_cb_arg(195) __(13) bf_cb_arg(195).
		q(nt(162), (nt(205)+nt(13)+nt(195)+nt(13)+nt(195)+nt(13)+nt(195)+nt(13)+nt(195)));
		//       wff_remove_existential_cb(123) => wff_remove_existential_cb_sym(206) __(13) wff_cb_arg(200) __(13) wff_cb_arg(200).
		q(nt(123), (nt(206)+nt(13)+nt(200)+nt(13)+nt(200)));
		//       wff_remove_bexistential_cb(124) => wff_remove_bexistential_cb_sym(207) __(13) wff_cb_arg(200) __(13) wff_cb_arg(200) __(13) wff_cb_arg(200) __(13) wff_cb_arg(200).
		q(nt(124), (nt(207)+nt(13)+nt(200)+nt(13)+nt(200)+nt(13)+nt(200)+nt(13)+nt(200)));
		//       wff_remove_buniversal_cb(125) => wff_remove_buniversal_cb_sym(208) __(13) wff_cb_arg(200) __(13) wff_cb_arg(200) __(13) wff_cb_arg(200) __(13) wff_cb_arg(200).
		q(nt(125), (nt(208)+nt(13)+nt(200)+nt(13)+nt(200)+nt(13)+nt(200)+nt(13)+nt(200)));
		//       wff_has_clashing_subformulas_cb(121) => wff_has_clashing_subformulas_cb_sym(209) __(13) wff_cb_arg(200) __(13) wff_cb_arg(200).
		q(nt(121), (nt(209)+nt(13)+nt(200)+nt(13)+nt(200)));
		//       bf_has_subformula_cb(160) => bf_has_subformula_cb_sym(210) __(13) bf_cb_arg(195) __(13) bf_cb_arg(195) __(13) bf_cb_arg(195).
		q(nt(160), (nt(210)+nt(13)+nt(195)+nt(13)+nt(195)+nt(13)+nt(195)));
		//       wff_has_subformula_cb(122) => wff_has_subformula_cb_sym(211) __(13) wff_cb_arg(200) __(13) wff_cb_arg(200) __(13) wff_cb_arg(200).
		q(nt(122), (nt(211)+nt(13)+nt(200)+nt(13)+nt(200)+nt(13)+nt(200)));
		//       bf_has_subformula_cb_sym(210) => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b'.
		q(nt(210), (t(30)+t(36)+t(32)+t(42)+t(26)+t(43)+t(32)+t(43)+t(44)+t(30)+t(36)+t(31)+t(45)+t(46)+t(44)+t(27)+t(26)+t(32)+t(47)+t(30)));
		//       wff_has_clashing_subformulas_cb_sym(209) => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b'.
		q(nt(209), (t(48)+t(36)+t(36)+t(32)+t(42)+t(26)+t(43)+t(32)+t(47)+t(27)+t(26)+t(43)+t(42)+t(41)+t(49)+t(50)+t(32)+t(43)+t(44)+t(30)+t(36)+t(31)+t(45)+t(46)+t(44)+t(27)+t(26)+t(43)+t(32)+t(47)+t(30)));
		//       wff_has_subformula_cb_sym(211) => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b'.
		q(nt(211), (t(48)+t(36)+t(36)+t(32)+t(42)+t(26)+t(43)+t(32)+t(43)+t(44)+t(30)+t(36)+t(31)+t(45)+t(46)+t(44)+t(27)+t(26)+t(32)+t(47)+t(30)));
		//       wff_remove_existential_cb_sym(206) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(206), (t(48)+t(36)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(28)+t(29)+t(41)+t(43)+t(52)+t(28)+t(49)+t(52)+t(41)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       wff_remove_bexistential_cb_sym(207) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(207), (t(48)+t(36)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(30)+t(28)+t(29)+t(41)+t(43)+t(52)+t(28)+t(49)+t(52)+t(41)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       wff_remove_buniversal_cb_sym(208) => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b'.
		q(nt(208), (t(48)+t(36)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(30)+t(44)+t(49)+t(41)+t(51)+t(28)+t(45)+t(43)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       bf_remove_fexistential_cb_sym(205) => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(205), (t(30)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(36)+t(28)+t(29)+t(41)+t(43)+t(52)+t(28)+t(49)+t(52)+t(41)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       bf_remove_funiversal_cb_sym(204) => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b'.
		q(nt(204), (t(30)+t(36)+t(32)+t(45)+t(28)+t(46)+t(31)+t(51)+t(28)+t(32)+t(36)+t(44)+t(49)+t(41)+t(51)+t(28)+t(45)+t(43)+t(26)+t(27)+t(32)+t(47)+t(30)));
		//       bf_cb_arg(195)       => bf(110).
		q(nt(195), (nt(110)));
		//       wff_cb_arg(200)      => wff(115).
		q(nt(200), (nt(115)));
		//       bf_and_cb_sym(194)   => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b'.
		q(nt(194), (t(30)+t(36)+t(32)+t(26)+t(49)+t(53)+t(32)+t(47)+t(30)));
		//       bf_or_cb_sym(196)    => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b'.
		q(nt(196), (t(30)+t(36)+t(32)+t(31)+t(45)+t(32)+t(47)+t(30)));
		//       bf_xor_cb_sym(197)   => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b'.
		q(nt(197), (t(30)+t(36)+t(32)+t(29)+t(31)+t(45)+t(32)+t(47)+t(30)));
		//       bf_neg_cb_sym(198)   => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b'.
		q(nt(198), (t(30)+t(36)+t(32)+t(49)+t(28)+t(50)+t(32)+t(47)+t(30)));
		//       bf_eq_cb_sym(199)    => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b'.
		q(nt(199), (t(30)+t(36)+t(32)+t(28)+t(54)+t(32)+t(47)+t(30)));
		//       bf_neq_cb_sym(201)   => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b'.
		q(nt(201), (t(30)+t(36)+t(32)+t(49)+t(28)+t(54)+t(32)+t(47)+t(30)));
		//       bf_is_zero_cb_sym(202) => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b'.
		q(nt(202), (t(30)+t(36)+t(32)+t(41)+t(43)+t(32)+t(55)+t(28)+t(45)+t(31)+t(32)+t(47)+t(30)));
		//       bf_is_one_cb_sym(203) => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b'.
		q(nt(203), (t(30)+t(36)+t(32)+t(41)+t(43)+t(32)+t(31)+t(49)+t(28)+t(32)+t(47)+t(30)));
		//       __E_tau_collapse_positives_cb_34(212) => tau_collapse_positives_cb_sym(213) __(13) tau_cb_arg(214) __(13) tau_cb_arg(214) __(13) tau_cb_arg(214).
		q(nt(212), (nt(213)+nt(13)+nt(214)+nt(13)+nt(214)+nt(13)+nt(214)));
		//       tau_collapse_positives_cb(101) => __E_tau_collapse_positives_cb_34(212).
		q(nt(101), (nt(212)));
		//       __E_tau_collapse_positives_cb_35(215) => tau_collapse_positives_cb_sym(213) __(13) tau_cb_arg(214) __(13) tau_cb_arg(214).
		q(nt(215), (nt(213)+nt(13)+nt(214)+nt(13)+nt(214)));
		//       tau_collapse_positives_cb(101) => __E_tau_collapse_positives_cb_35(215).
		q(nt(101), (nt(215)));
		//       tau_positives_upwards_cb(102) => tau_positives_upwards_cb_sym(216) __(13) tau_cb_arg(214) __(13) tau_cb_arg(214).
		q(nt(102), (nt(216)+nt(13)+nt(214)+nt(13)+nt(214)));
		//       tau_cb_arg(214)      => capture(79).
		q(nt(214), (nt(79)));
		//       tau_cb_arg(214)      => tau(100).
		q(nt(214), (nt(100)));
		//       tau_collapse_positives_cb_sym(213) => 't' 'a' 'u' '_' 'c' 'o' 'l' 'l' 'a' 'p' 's' 'e' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'c' 'b'.
		q(nt(213), (t(52)+t(26)+t(44)+t(32)+t(47)+t(31)+t(27)+t(27)+t(26)+t(56)+t(43)+t(28)+t(32)+t(56)+t(31)+t(43)+t(41)+t(52)+t(41)+t(51)+t(28)+t(43)+t(32)+t(47)+t(30)));
		//       tau_positives_upwards_cb_sym(216) => 't' 'a' 'u' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'u' 'p' 'w' 'a' 'r' 'd' 's' '_' 'c' 'b'.
		q(nt(216), (t(52)+t(26)+t(44)+t(32)+t(56)+t(31)+t(43)+t(41)+t(52)+t(41)+t(51)+t(28)+t(43)+t(32)+t(44)+t(56)+t(48)+t(26)+t(45)+t(53)+t(43)+t(32)+t(47)+t(30)));
		//       __E_inputs_36(219)   => _(15) input(218).
		q(nt(219), (nt(15)+nt(218)));
		//       __E_inputs_37(220)   => null.
		q(nt(220), (nul));
		//       __E_inputs_37(220)   => __E_inputs_36(219) __E_inputs_37(220).
		q(nt(220), (nt(219)+nt(220)));
		//       inputs(217)          => _(15) less(27) _(15) input(218) __E_inputs_37(220) _(15) dot(17).
		q(nt(217), (nt(15)+nt(27)+nt(15)+nt(218)+nt(220)+nt(15)+nt(17)));
		//       input(218)           => in(87) _(15) colon(25) _(15) open_brace(22) _(15) source_binding(187) _(15) close_brace(23).
		q(nt(218), (nt(87)+nt(15)+nt(25)+nt(15)+nt(22)+nt(15)+nt(187)+nt(15)+nt(23)));
		//       builder(221)         => _(15) builder_head(222) _(15) builder_body(223) _(15) dot(17).
		q(nt(221), (nt(15)+nt(222)+nt(15)+nt(223)+nt(15)+nt(17)));
		//       __E_builder_head_38(224) => __(13) capture(79).
		q(nt(224), (nt(13)+nt(79)));
		//       __E_builder_head_39(225) => null.
		q(nt(225), (nul));
		//       __E_builder_head_39(225) => __E_builder_head_38(224) __E_builder_head_39(225).
		q(nt(225), (nt(224)+nt(225)));
		//       builder_head(222)    => open_parenthesis(18) _(15) capture(79) __E_builder_head_39(225) _(15) close_parenthesis(19).
		q(nt(222), (nt(18)+nt(15)+nt(79)+nt(225)+nt(15)+nt(19)));
		//       builder_body(223)    => bf_builder_body(226).
		q(nt(223), (nt(226)));
		//       builder_body(223)    => wff_builder_body(227).
		q(nt(223), (nt(227)));
		//       builder_body(223)    => tau_builder_body(228).
		q(nt(223), (nt(228)));
		//       bf_builder_body(226) => bf_builder_def(52) _(15) bf(110).
		q(nt(226), (nt(52)+nt(15)+nt(110)));
		//       wff_builder_body(227) => wff_builder_def(37) _(15) wff(115).
		q(nt(227), (nt(37)+nt(15)+nt(115)));
		//       tau_builder_body(228) => tau_builder_def(32) _(15) tau(100).
		q(nt(228), (nt(32)+nt(15)+nt(100)));
		//       library(229)         => rules(230).
		q(nt(229), (nt(230)));
		//       __E_rules_40(231)    => _(15) rule(232).
		q(nt(231), (nt(15)+nt(232)));
		//       __E_rules_41(233)    => null.
		q(nt(233), (nul));
		//       __E_rules_41(233)    => __E_rules_40(231) __E_rules_41(233).
		q(nt(233), (nt(231)+nt(233)));
		//       rules(230)           => __E_rules_41(233).
		q(nt(230), (nt(233)));
		//       rule(232)            => wff_rule(116).
		q(nt(232), (nt(116)));
		//       rule(232)            => bf_rule(155).
		q(nt(232), (nt(155)));
		//       rule(232)            => tau_rule(97).
		q(nt(232), (nt(97)));
		//       nso_rr(234)          => nso_rec_relations(235) nso_main(236).
		q(nt(234), (nt(235)+nt(236)));
		//       __E_nso_rec_relations_42(237) => _(15) nso_rec_relation(238).
		q(nt(237), (nt(15)+nt(238)));
		//       __E_nso_rec_relations_43(239) => null.
		q(nt(239), (nul));
		//       __E_nso_rec_relations_43(239) => __E_nso_rec_relations_42(237) __E_nso_rec_relations_43(239).
		q(nt(239), (nt(237)+nt(239)));
		//       nso_rec_relations(235) => __E_nso_rec_relations_43(239).
		q(nt(235), (nt(239)));
		//       nso_rec_relation(238) => bf_rec_relation(176).
		q(nt(238), (nt(176)));
		//       nso_rec_relation(238) => wff_rec_relation(126).
		q(nt(238), (nt(126)));
		//       nso_main(236)        => _(15) wff(115) _(15) dot(17).
		q(nt(236), (nt(15)+nt(115)+nt(15)+nt(17)));
		//       gssotc_rr(240)       => gssotc_rec_relations(241) gssotc_main(242).
		q(nt(240), (nt(241)+nt(242)));
		//       __E_gssotc_rec_relations_44(243) => _(15) gssotc_rec_relation(244).
		q(nt(243), (nt(15)+nt(244)));
		//       __E_gssotc_rec_relations_45(245) => null.
		q(nt(245), (nul));
		//       __E_gssotc_rec_relations_45(245) => __E_gssotc_rec_relations_44(243) __E_gssotc_rec_relations_45(245).
		q(nt(245), (nt(243)+nt(245)));
		//       gssotc_rec_relations(241) => __E_gssotc_rec_relations_45(245).
		q(nt(241), (nt(245)));
		//       gssotc_rec_relation(244) => tau_rec_relation(103).
		q(nt(244), (nt(103)));
		//       gssotc_rec_relation(244) => wff_rec_relation(126).
		q(nt(244), (nt(126)));
		//       gssotc_rec_relation(244) => bf_rec_relation(176).
		q(nt(244), (nt(176)));
		//       gssotc_main(242)     => _(15) tau(100) _(15) semicolon(26).
		q(nt(242), (nt(15)+nt(100)+nt(15)+nt(26)));
		//       __E_start_46(247)    => inputs(217).
		q(nt(247), (nt(217)));
		//       __E_start_46(247)    => builder(221).
		q(nt(247), (nt(221)));
		//       __E_start_46(247)    => library(229).
		q(nt(247), (nt(229)));
		//       __E_start_46(247)    => nso_rr(234).
		q(nt(247), (nt(234)));
		//       __E_start_46(247)    => gssotc_rr(240).
		q(nt(247), (nt(240)));
		//       start(246)           => __E_start_46(247) _(15).
		q(nt(246), (nt(247)+nt(15)));
		//       __E_cli_47(250)      => _(15) dot(17) _(15) cli_command(249).
		q(nt(250), (nt(15)+nt(17)+nt(15)+nt(249)));
		//       __E_cli_48(251)      => null.
		q(nt(251), (nul));
		//       __E_cli_48(251)      => __E_cli_47(250) __E_cli_48(251).
		q(nt(251), (nt(250)+nt(251)));
		//       cli(248)             => _(15) cli_command(249) __E_cli_48(251).
		q(nt(248), (nt(15)+nt(249)+nt(251)));
		//       cli_command(249)     => help_cmd(252).
		q(nt(249), (nt(252)));
		//       cli_command(249)     => version_cmd(253).
		q(nt(249), (nt(253)));
		//       cli_command(249)     => quit_cmd(254).
		q(nt(249), (nt(254)));
		//       cli_command(249)     => clear_cmd(255).
		q(nt(249), (nt(255)));
		//       cli_command(249)     => get_cmd(256).
		q(nt(249), (nt(256)));
		//       cli_command(249)     => set_cmd(257).
		q(nt(249), (nt(257)));
		//       cli_command(249)     => toggle_cmd(258).
		q(nt(249), (nt(258)));
		//       cli_command(249)     => list_outputs_cmd(259).
		q(nt(249), (nt(259)));
		//       cli_command(249)     => clear_outputs_cmd(260).
		q(nt(249), (nt(260)));
		//       cli_command(249)     => print_output_cmd(261).
		q(nt(249), (nt(261)));
		//       cli_command(249)     => file(262).
		q(nt(249), (nt(262)));
		//       cli_command(249)     => normalize_cmd(263).
		q(nt(249), (nt(263)));
		//       cli_command(249)     => bf_dnf_cmd(264).
		q(nt(249), (nt(264)));
		//       cli_command(249)     => bf_cnf_cmd(265).
		q(nt(249), (nt(265)));
		//       cli_command(249)     => bf_anf_cmd(266).
		q(nt(249), (nt(266)));
		//       cli_command(249)     => bf_nnf_cmd(267).
		q(nt(249), (nt(267)));
		//       cli_command(249)     => bf_pnf_cmd(268).
		q(nt(249), (nt(268)));
		//       cli_command(249)     => bf_mnf_cmd(269).
		q(nt(249), (nt(269)));
		//       cli_command(249)     => bf_substitute(270).
		q(nt(249), (nt(270)));
		//       cli_command(249)     => wff_substitute(271).
		q(nt(249), (nt(271)));
		//       cli_command(249)     => bf_instantiate(272).
		q(nt(249), (nt(272)));
		//       cli_command(249)     => wff_instantiate(273).
		q(nt(249), (nt(273)));
		//       cli_command(249)     => wff_onf_cmd(274).
		q(nt(249), (nt(274)));
		//       cli_command(249)     => wff_dnf_cmd(275).
		q(nt(249), (nt(275)));
		//       cli_command(249)     => wff_cnf_cmd(276).
		q(nt(249), (nt(276)));
		//       cli_command(249)     => wff_anf_cmd(277).
		q(nt(249), (nt(277)));
		//       cli_command(249)     => wff_nnf_cmd(278).
		q(nt(249), (nt(278)));
		//       cli_command(249)     => wff_pnf_cmd(279).
		q(nt(249), (nt(279)));
		//       cli_command(249)     => wff_mnf_cmd(280).
		q(nt(249), (nt(280)));
		//       cli_command(249)     => def_wff_cmd(281).
		q(nt(249), (nt(281)));
		//       cli_command(249)     => def_bf_cmd(282).
		q(nt(249), (nt(282)));
		//       cli_command(249)     => def_tau_cmd(283).
		q(nt(249), (nt(283)));
		//       cli_command(249)     => def_list_cmd(284).
		q(nt(249), (nt(284)));
		//       cli_command(249)     => def_del_cmd(285).
		q(nt(249), (nt(285)));
		//       cli_command(249)     => bf(110).
		q(nt(249), (nt(110)));
		//       cli_command(249)     => wff(115).
		q(nt(249), (nt(115)));
		//       cli_command(249)     => nso_rr(234).
		q(nt(249), (nt(234)));
		//       file(262)            => file_sym(286) quote(30) file_path(287) quote(30).
		q(nt(262), (nt(286)+nt(30)+nt(287)+nt(30)));
		//       normalize_cmd(263)   => normalize_cmd_sym(288) __(13) normalize_cmd_arg(289).
		q(nt(263), (nt(288)+nt(13)+nt(289)));
		//       normalize_cmd_arg(289) => wff(115).
		q(nt(289), (nt(115)));
		//       normalize_cmd_arg(289) => nso_rr(234).
		q(nt(289), (nt(234)));
		//       normalize_cmd_arg(289) => output(290).
		q(nt(289), (nt(290)));
		//       normalize_cmd_arg(289) => wff_selection(291).
		q(nt(289), (nt(291)));
		//       __E_bf_instantiate_cmd_49(294) => bf(110).
		q(nt(294), (nt(110)));
		//       __E_bf_instantiate_cmd_49(294) => bf_selection(295).
		q(nt(294), (nt(295)));
		//       __E_bf_instantiate_cmd_49(294) => output(290).
		q(nt(294), (nt(290)));
		//       __E_bf_instantiate_cmd_50(296) => bf_selection(295).
		q(nt(296), (nt(295)));
		//       __E_bf_instantiate_cmd_50(296) => output(290).
		q(nt(296), (nt(290)));
		//       bf_instantiate_cmd(292) => instantiate_cmd_sym(293) __(13) variable(82) __(13) __E_bf_instantiate_cmd_49(294) __(13) __E_bf_instantiate_cmd_50(296).
		q(nt(292), (nt(293)+nt(13)+nt(82)+nt(13)+nt(294)+nt(13)+nt(296)));
		//       __E_wff_instantiate_cmd_51(298) => wff(115).
		q(nt(298), (nt(115)));
		//       __E_wff_instantiate_cmd_51(298) => wff_selection(291).
		q(nt(298), (nt(291)));
		//       __E_wff_instantiate_cmd_51(298) => output(290).
		q(nt(298), (nt(290)));
		//       __E_wff_instantiate_cmd_52(299) => wff_selection(291).
		q(nt(299), (nt(291)));
		//       __E_wff_instantiate_cmd_52(299) => output(290).
		q(nt(299), (nt(290)));
		//       wff_instantiate_cmd(297) => instantiate_cmd_sym(293) __(13) variable(82) __(13) __E_wff_instantiate_cmd_51(298) __(13) __E_wff_instantiate_cmd_52(299).
		q(nt(297), (nt(293)+nt(13)+nt(82)+nt(13)+nt(298)+nt(13)+nt(299)));
		//       __E_bf_substitute_cmd_53(302) => bf(110).
		q(nt(302), (nt(110)));
		//       __E_bf_substitute_cmd_53(302) => output(290).
		q(nt(302), (nt(290)));
		//       __E_bf_substitute_cmd_53(302) => bf_selection(295).
		q(nt(302), (nt(295)));
		//       __E_bf_substitute_cmd_54(303) => output(290).
		q(nt(303), (nt(290)));
		//       __E_bf_substitute_cmd_54(303) => bf_selection(295).
		q(nt(303), (nt(295)));
		//       __E_bf_substitute_cmd_54(303) => bf_selection(295).
		q(nt(303), (nt(295)));
		//       __E_bf_substitute_cmd_55(304) => bf_selection(295).
		q(nt(304), (nt(295)));
		//       __E_bf_substitute_cmd_55(304) => output(290).
		q(nt(304), (nt(290)));
		//       bf_substitute_cmd(300) => substitute_cmd_sym(301) __(13) __E_bf_substitute_cmd_53(302) __(13) __E_bf_substitute_cmd_54(303) __E_bf_substitute_cmd_55(304).
		q(nt(300), (nt(301)+nt(13)+nt(302)+nt(13)+nt(303)+nt(304)));
		//       __E_wff_substitute_cmd_56(307) => wff(115).
		q(nt(307), (nt(115)));
		//       __E_wff_substitute_cmd_56(307) => output(290).
		q(nt(307), (nt(290)));
		//       __E_wff_substitute_cmd_56(307) => wff_selection(291).
		q(nt(307), (nt(291)));
		//       __E_wff_substitute_cmd_57(308) => output(290).
		q(nt(308), (nt(290)));
		//       __E_wff_substitute_cmd_57(308) => wff_selection(291).
		q(nt(308), (nt(291)));
		//       __E_wff_substitute_cmd_58(309) => wff_selection(291).
		q(nt(309), (nt(291)));
		//       __E_wff_substitute_cmd_58(309) => output(290).
		q(nt(309), (nt(290)));
		//       wff_substitute_cmd(305) => substitutecmd__sym(306) __(13) __E_wff_substitute_cmd_56(307) __(13) __E_wff_substitute_cmd_57(308) __E_wff_substitute_cmd_58(309).
		q(nt(305), (nt(306)+nt(13)+nt(307)+nt(13)+nt(308)+nt(309)));
		//       __E_bf_dnf_cmd_59(311) => bf(110).
		q(nt(311), (nt(110)));
		//       __E_bf_dnf_cmd_59(311) => output(290).
		q(nt(311), (nt(290)));
		//       __E_bf_dnf_cmd_59(311) => bf_selection(295).
		q(nt(311), (nt(295)));
		//       bf_dnf_cmd(264)      => dnf_cmd_sym(310) __(13) __E_bf_dnf_cmd_59(311).
		q(nt(264), (nt(310)+nt(13)+nt(311)));
		//       __E_bf_cnf_cmd_60(313) => bf(110).
		q(nt(313), (nt(110)));
		//       __E_bf_cnf_cmd_60(313) => output(290).
		q(nt(313), (nt(290)));
		//       __E_bf_cnf_cmd_60(313) => bf_selection(295).
		q(nt(313), (nt(295)));
		//       bf_cnf_cmd(265)      => cnf_cmd_sym(312) __(13) __E_bf_cnf_cmd_60(313).
		q(nt(265), (nt(312)+nt(13)+nt(313)));
		//       __E_bf_anf_cmd_61(315) => bf(110).
		q(nt(315), (nt(110)));
		//       __E_bf_anf_cmd_61(315) => output(290).
		q(nt(315), (nt(290)));
		//       __E_bf_anf_cmd_61(315) => bf_selection(295).
		q(nt(315), (nt(295)));
		//       bf_anf_cmd(266)      => anf_cmd_sym(314) __(13) __E_bf_anf_cmd_61(315).
		q(nt(266), (nt(314)+nt(13)+nt(315)));
		//       __E_bf_nnf_cmd_62(317) => bf(110).
		q(nt(317), (nt(110)));
		//       __E_bf_nnf_cmd_62(317) => output(290).
		q(nt(317), (nt(290)));
		//       __E_bf_nnf_cmd_62(317) => bf_selection(295).
		q(nt(317), (nt(295)));
		//       bf_nnf_cmd(267)      => nnf_cmd_sym(316) __(13) __E_bf_nnf_cmd_62(317).
		q(nt(267), (nt(316)+nt(13)+nt(317)));
		//       __E_bf_pnf_cmd_63(319) => bf(110).
		q(nt(319), (nt(110)));
		//       __E_bf_pnf_cmd_63(319) => output(290).
		q(nt(319), (nt(290)));
		//       __E_bf_pnf_cmd_63(319) => bf_selection(295).
		q(nt(319), (nt(295)));
		//       bf_pnf_cmd(268)      => pnf_cmd_sym(318) __(13) __E_bf_pnf_cmd_63(319).
		q(nt(268), (nt(318)+nt(13)+nt(319)));
		//       __E_bf_mnf_cmd_64(321) => bf(110).
		q(nt(321), (nt(110)));
		//       __E_bf_mnf_cmd_64(321) => output(290).
		q(nt(321), (nt(290)));
		//       __E_bf_mnf_cmd_64(321) => bf_selection(295).
		q(nt(321), (nt(295)));
		//       bf_mnf_cmd(269)      => mnf_cmd_sym(320) __(13) __E_bf_mnf_cmd_64(321).
		q(nt(269), (nt(320)+nt(13)+nt(321)));
		//       __E_wff_onf_cmd_65(323) => wff(115).
		q(nt(323), (nt(115)));
		//       __E_wff_onf_cmd_65(323) => output(290).
		q(nt(323), (nt(290)));
		//       __E_wff_onf_cmd_65(323) => wff_selection(291).
		q(nt(323), (nt(291)));
		//       wff_onf_cmd(274)     => onf_cmd_sym(322) __(13) variable(82) __(13) __E_wff_onf_cmd_65(323).
		q(nt(274), (nt(322)+nt(13)+nt(82)+nt(13)+nt(323)));
		//       __E_wff_dnf_cmd_66(324) => wff(115).
		q(nt(324), (nt(115)));
		//       __E_wff_dnf_cmd_66(324) => output(290).
		q(nt(324), (nt(290)));
		//       __E_wff_dnf_cmd_66(324) => wff_selection(291).
		q(nt(324), (nt(291)));
		//       wff_dnf_cmd(275)     => dnf_cmd_sym(310) __(13) __E_wff_dnf_cmd_66(324).
		q(nt(275), (nt(310)+nt(13)+nt(324)));
		//       __E_wff_cnf_cmd_67(325) => wff(115).
		q(nt(325), (nt(115)));
		//       __E_wff_cnf_cmd_67(325) => output(290).
		q(nt(325), (nt(290)));
		//       __E_wff_cnf_cmd_67(325) => wff_selection(291).
		q(nt(325), (nt(291)));
		//       wff_cnf_cmd(276)     => cnf_cmd_sym(312) __(13) __E_wff_cnf_cmd_67(325).
		q(nt(276), (nt(312)+nt(13)+nt(325)));
		//       __E_wff_anf_cmd_68(326) => wff(115).
		q(nt(326), (nt(115)));
		//       __E_wff_anf_cmd_68(326) => output(290).
		q(nt(326), (nt(290)));
		//       __E_wff_anf_cmd_68(326) => wff_selection(291).
		q(nt(326), (nt(291)));
		//       wff_anf_cmd(277)     => anf_cmd_sym(314) __(13) __E_wff_anf_cmd_68(326).
		q(nt(277), (nt(314)+nt(13)+nt(326)));
		//       __E_wff_nnf_cmd_69(327) => wff(115).
		q(nt(327), (nt(115)));
		//       __E_wff_nnf_cmd_69(327) => output(290).
		q(nt(327), (nt(290)));
		//       __E_wff_nnf_cmd_69(327) => wff_selection(291).
		q(nt(327), (nt(291)));
		//       wff_nnf_cmd(278)     => nnf_cmd_sym(316) __(13) __E_wff_nnf_cmd_69(327).
		q(nt(278), (nt(316)+nt(13)+nt(327)));
		//       __E_wff_pnf_cmd_70(328) => wff(115).
		q(nt(328), (nt(115)));
		//       __E_wff_pnf_cmd_70(328) => output(290).
		q(nt(328), (nt(290)));
		//       __E_wff_pnf_cmd_70(328) => wff_selection(291).
		q(nt(328), (nt(291)));
		//       wff_pnf_cmd(279)     => pnf_cmd_sym(318) __(13) __E_wff_pnf_cmd_70(328).
		q(nt(279), (nt(318)+nt(13)+nt(328)));
		//       __E_wff_mnf_cmd_71(329) => wff(115).
		q(nt(329), (nt(115)));
		//       __E_wff_mnf_cmd_71(329) => output(290).
		q(nt(329), (nt(290)));
		//       __E_wff_mnf_cmd_71(329) => wff_selection(291).
		q(nt(329), (nt(291)));
		//       wff_mnf_cmd(280)     => mnf_cmd_sym(320) __(13) __E_wff_mnf_cmd_71(329).
		q(nt(280), (nt(320)+nt(13)+nt(329)));
		//       def_wff_cmd(281)     => def_cmd_sym(330) __(13) wff_rec_relation(126).
		q(nt(281), (nt(330)+nt(13)+nt(126)));
		//       def_bf_cmd(282)      => def_cmd_sym(330) __(13) bf_rec_relation(176).
		q(nt(282), (nt(330)+nt(13)+nt(176)));
		//       def_tau_cmd(283)     => def_cmd_sym(330) __(13) tau_rec_relation(103).
		q(nt(283), (nt(330)+nt(13)+nt(103)));
		//       def_list_cmd(284)    => def_cmd_sym(330) __(13) 'l' 'i' 's' 't'.
		q(nt(284), (nt(330)+nt(13)+t(27)+t(41)+t(43)+t(52)));
		//       def_del_cmd(285)     => def_cmd_sym(330) __(13) 'd' 'e' 'l' __(13) digits(71).
		q(nt(285), (nt(330)+nt(13)+t(53)+t(28)+t(27)+nt(13)+nt(71)));
		//       def_clear_cmd(331)   => def_cmd_sym(330) __(13) 'c' 'l' 'e' 'a' 'r'.
		q(nt(331), (nt(330)+nt(13)+t(47)+t(27)+t(28)+t(26)+t(45)));
		//       __E_help_cmd_72(333) => __(13) cli_cmd_sym(334).
		q(nt(333), (nt(13)+nt(334)));
		//       __E_help_cmd_72(333) => null.
		q(nt(333), (nul));
		//       help_cmd(252)        => help_cmd_sym(332) __E_help_cmd_72(333).
		q(nt(252), (nt(332)+nt(333)));
		//       version_cmd(253)     => version_cmd_sym(335).
		q(nt(253), (nt(335)));
		//       quit_cmd(254)        => quit_cmd_sym(336).
		q(nt(254), (nt(336)));
		//       clear_cmd(255)       => clear_cmd_sym(337).
		q(nt(255), (nt(337)));
		//       wff_selection(291)   => selection_sym(338) __(13) digits(71).
		q(nt(291), (nt(338)+nt(13)+nt(71)));
		//       bf_selection(295)    => selection_sym(338) __(13) digits(71).
		q(nt(295), (nt(338)+nt(13)+nt(71)));
		//       bf_var_selection(339) => selection_sym(338) __(13) digits(71).
		q(nt(339), (nt(338)+nt(13)+nt(71)));
		//       wff_var_selection(340) => selection_sym(338) __(13) digits(71).
		q(nt(340), (nt(338)+nt(13)+nt(71)));
		//       file_path(287)       => chars(68).
		q(nt(287), (nt(68)));
		//       output(290)          => absolute_output(341).
		q(nt(290), (nt(341)));
		//       output(290)          => relative_output(342).
		q(nt(290), (nt(342)));
		//       __E_absolute_output_73(343) => absolute_output_sym(344).
		q(nt(343), (nt(344)));
		//       __E_absolute_output_73(343) => output_sym(345) __(13).
		q(nt(343), (nt(345)+nt(13)));
		//       absolute_output(341) => __E_absolute_output_73(343) output_id(346).
		q(nt(341), (nt(343)+nt(346)));
		//       relative_output(342) => relative_output_sym(347) output_id(346).
		q(nt(342), (nt(347)+nt(346)));
		//       output_id(346)       => digits(71).
		q(nt(346), (nt(71)));
		//       list_outputs_cmd(259) => output_sym(345).
		q(nt(259), (nt(345)));
		//       list_outputs_cmd(259) => absolute_output_sym(344).
		q(nt(259), (nt(344)));
		//       list_outputs_cmd(259) => relative_output_sym(347).
		q(nt(259), (nt(347)));
		//       __E_clear_outputs_cmd_74(348) => output_sym(345) __(13).
		q(nt(348), (nt(345)+nt(13)));
		//       __E_clear_outputs_cmd_74(348) => absolute_output_sym(344).
		q(nt(348), (nt(344)));
		//       __E_clear_outputs_cmd_74(348) => relative_output_sym(347).
		q(nt(348), (nt(347)));
		//       clear_outputs_cmd(260) => __E_clear_outputs_cmd_74(348) clear_sym(349).
		q(nt(260), (nt(348)+nt(349)));
		//       print_output_cmd(261) => output(290).
		q(nt(261), (nt(290)));
		//       cli_cmd_sym(334)     => help_cmd_sym(332).
		q(nt(334), (nt(332)));
		//       cli_cmd_sym(334)     => version_cmd_sym(335).
		q(nt(334), (nt(335)));
		//       cli_cmd_sym(334)     => quit_cmd_sym(336).
		q(nt(334), (nt(336)));
		//       cli_cmd_sym(334)     => clear_cmd_sym(337).
		q(nt(334), (nt(337)));
		//       cli_cmd_sym(334)     => output_sym(345).
		q(nt(334), (nt(345)));
		//       cli_cmd_sym(334)     => absolute_output_sym(344).
		q(nt(334), (nt(344)));
		//       cli_cmd_sym(334)     => relative_output_sym(347).
		q(nt(334), (nt(347)));
		//       cli_cmd_sym(334)     => selection_sym(338).
		q(nt(334), (nt(338)));
		//       cli_cmd_sym(334)     => instantiate_cmd_sym(293).
		q(nt(334), (nt(293)));
		//       cli_cmd_sym(334)     => substitute_cmd_sym(301).
		q(nt(334), (nt(301)));
		//       cli_cmd_sym(334)     => normalize_cmd_sym(288).
		q(nt(334), (nt(288)));
		//       cli_cmd_sym(334)     => file_cmd_sym(350).
		q(nt(334), (nt(350)));
		//       cli_cmd_sym(334)     => get_cmd_sym(351).
		q(nt(334), (nt(351)));
		//       cli_cmd_sym(334)     => set_cmd_sym(352).
		q(nt(334), (nt(352)));
		//       cli_cmd_sym(334)     => toggle_cmd_sym(353).
		q(nt(334), (nt(353)));
		//       cli_cmd_sym(334)     => cnf_cmd_sym(312).
		q(nt(334), (nt(312)));
		//       cli_cmd_sym(334)     => dnf_cmd_sym(310).
		q(nt(334), (nt(310)));
		//       cli_cmd_sym(334)     => nnf_cmd_sym(316).
		q(nt(334), (nt(316)));
		//       cli_cmd_sym(334)     => mnf_cmd_sym(320).
		q(nt(334), (nt(320)));
		//       cli_cmd_sym(334)     => onf_cmd_sym(322).
		q(nt(334), (nt(322)));
		//       help_cmd_sym(332)    => 'h'.
		q(nt(332), (t(42)));
		//       help_cmd_sym(332)    => 'h' 'e' 'l' 'p'.
		q(nt(332), (t(42)+t(28)+t(27)+t(56)));
		//       version_cmd_sym(335) => 'v'.
		q(nt(335), (t(51)));
		//       version_cmd_sym(335) => 'v' 'e' 'r' 's' 'i' 'o' 'n'.
		q(nt(335), (t(51)+t(28)+t(45)+t(43)+t(41)+t(31)+t(49)));
		//       quit_cmd_sym(336)    => 'q'.
		q(nt(336), (t(54)));
		//       quit_cmd_sym(336)    => 'e'.
		q(nt(336), (t(28)));
		//       quit_cmd_sym(336)    => 'q' 'u' 'i' 't'.
		q(nt(336), (t(54)+t(44)+t(41)+t(52)));
		//       quit_cmd_sym(336)    => 'e' 'x' 'i' 't'.
		q(nt(336), (t(28)+t(29)+t(41)+t(52)));
		//       instantiate_sym(354) => 'i'.
		q(nt(354), (t(41)));
		//       instantiate_sym(354) => 'i' 'n' 's' 't' 'a' 'n' 't' 'i' 'a' 't' 'e'.
		q(nt(354), (t(41)+t(49)+t(43)+t(52)+t(26)+t(49)+t(52)+t(41)+t(26)+t(52)+t(28)));
		//       substitute_sym(355)  => 's'.
		q(nt(355), (t(43)));
		//       substitute_sym(355)  => 's' 'u' 'b' 's' 't' 'i' 't' 'u' 't' 'e'.
		q(nt(355), (t(43)+t(44)+t(30)+t(43)+t(52)+t(41)+t(52)+t(44)+t(52)+t(28)));
		//       normalize_cmd_sym(288) => 'n'.
		q(nt(288), (t(49)));
		//       normalize_cmd_sym(288) => 'n' 'o' 'r' 'm' 'a' 'l' 'i' 'z' 'e'.
		q(nt(288), (t(49)+t(31)+t(45)+t(46)+t(26)+t(27)+t(41)+t(55)+t(28)));
		//       file_cmd_sym(350)    => 'r'.
		q(nt(350), (t(45)));
		//       file_cmd_sym(350)    => 'r' 'e' 'a' 'd'.
		q(nt(350), (t(45)+t(28)+t(26)+t(53)));
		//       clear_cmd_sym(337)   => 'c'.
		q(nt(337), (t(47)));
		//       clear_cmd_sym(337)   => 'c' 'l' 'e' 'a' 'r'.
		q(nt(337), (t(47)+t(27)+t(28)+t(26)+t(45)));
		//       onf_cmd_sym(322)     => 'o' 'n' 'f'.
		q(nt(322), (t(31)+t(49)+t(36)));
		//       dnf_cmd_sym(310)     => 'd' 'n' 'f'.
		q(nt(310), (t(53)+t(49)+t(36)));
		//       cnf_cmd_sym(312)     => 'c' 'n' 'f'.
		q(nt(312), (t(47)+t(49)+t(36)));
		//       anf_cmd_sym(314)     => 'a' 'n' 'f'.
		q(nt(314), (t(26)+t(49)+t(36)));
		//       nnf_cmd_sym(316)     => 'n' 'n' 'f'.
		q(nt(316), (t(49)+t(49)+t(36)));
		//       pnf_cmd_sym(318)     => 'p' 'n' 'f'.
		q(nt(318), (t(56)+t(49)+t(36)));
		//       mnf_cmd_sym(320)     => 'm' 'n' 'f'.
		q(nt(320), (t(46)+t(49)+t(36)));
		//       def_cmd_sym(330)     => 'd' 'e' 'f'.
		q(nt(330), (t(53)+t(28)+t(36)));
		//       selection_sym(338)   => 's'.
		q(nt(338), (t(43)));
		//       selection_sym(338)   => 's' 'e' 'l' 'e' 'c' 't' 'i' 'o' 'n'.
		q(nt(338), (t(43)+t(28)+t(27)+t(28)+t(47)+t(52)+t(41)+t(31)+t(49)));
		//       output_sym(345)      => 'o'.
		q(nt(345), (t(31)));
		//       output_sym(345)      => 'o' 'u' 't' 'p' 'u' 't'.
		q(nt(345), (t(31)+t(44)+t(52)+t(56)+t(44)+t(52)));
		//       absolute_output_sym(344) => '&'.
		q(nt(344), (t(20)));
		//       relative_output_sym(347) => '%'.
		q(nt(347), (t(57)));
		//       __E_get_cmd_75(356)  => __(13) option(357).
		q(nt(356), (nt(13)+nt(357)));
		//       __E_get_cmd_75(356)  => null.
		q(nt(356), (nul));
		//       get_cmd(256)         => get_cmd_sym(351) __E_get_cmd_75(356).
		q(nt(256), (nt(351)+nt(356)));
		//       __E_set_cmd_76(358)  => __(13).
		q(nt(358), (nt(13)));
		//       __E_set_cmd_76(358)  => _(15) '=' _(15).
		q(nt(358), (nt(15)+t(19)+nt(15)));
		//       set_cmd(257)         => set_cmd_sym(352) __(13) option(357) __E_set_cmd_76(358) option_value(359).
		q(nt(257), (nt(352)+nt(13)+nt(357)+nt(358)+nt(359)));
		//       toggle_cmd(258)      => toggle_cmd_sym(353) __(13) bool_option(360).
		q(nt(258), (nt(353)+nt(13)+nt(360)));
		//       get_cmd_sym(351)     => 'g' 'e' 't'.
		q(nt(351), (t(50)+t(28)+t(52)));
		//       set_cmd_sym(352)     => 's' 'e' 't'.
		q(nt(352), (t(43)+t(28)+t(52)));
		//       toggle_cmd_sym(353)  => 't' 'o' 'g' 'g' 'l' 'e'.
		q(nt(353), (t(52)+t(31)+t(50)+t(50)+t(27)+t(28)));
		//       option(357)          => bool_option(360).
		q(nt(357), (nt(360)));
		//       option(357)          => severity_opt(361).
		q(nt(357), (nt(361)));
		//       bool_option(360)     => status_opt(362).
		q(nt(360), (nt(362)));
		//       bool_option(360)     => colors_opt(363).
		q(nt(360), (nt(363)));
		//       bool_option(360)     => debug_repl_opt(364).
		q(nt(360), (nt(364)));
		//       status_opt(362)      => 's'.
		q(nt(362), (t(43)));
		//       status_opt(362)      => 's' 't' 'a' 't' 'u' 's'.
		q(nt(362), (t(43)+t(52)+t(26)+t(52)+t(44)+t(43)));
		//       colors_opt(363)      => 'c'.
		q(nt(363), (t(47)));
		//       colors_opt(363)      => 'c' 'o' 'l' 'o' 'r'.
		q(nt(363), (t(47)+t(31)+t(27)+t(31)+t(45)));
		//       colors_opt(363)      => 'c' 'o' 'l' 'o' 'r' 's'.
		q(nt(363), (t(47)+t(31)+t(27)+t(31)+t(45)+t(43)));
		//       severity_opt(361)    => 's' 'e' 'v'.
		q(nt(361), (t(43)+t(28)+t(51)));
		//       severity_opt(361)    => 's' 'e' 'v' 'e' 'r' 'i' 't' 'y'.
		q(nt(361), (t(43)+t(28)+t(51)+t(28)+t(45)+t(41)+t(52)+t(58)));
		//       debug_repl_opt(364)  => 'd'.
		q(nt(364), (t(53)));
		//       debug_repl_opt(364)  => 'd' 'r'.
		q(nt(364), (t(53)+t(45)));
		//       debug_repl_opt(364)  => 'd' 'b' 'g'.
		q(nt(364), (t(53)+t(30)+t(50)));
		//       debug_repl_opt(364)  => 'd' 'e' 'b' 'u' 'g'.
		q(nt(364), (t(53)+t(28)+t(30)+t(44)+t(50)));
		//       debug_repl_opt(364)  => 'd' 'e' 'b' 'u' 'g' '-' 'r' 'e' 'p' 'l'.
		q(nt(364), (t(53)+t(28)+t(30)+t(44)+t(50)+t(12)+t(45)+t(28)+t(56)+t(27)));
		//       option_value(359)    => option_value_true(365).
		q(nt(359), (nt(365)));
		//       option_value(359)    => option_value_false(366).
		q(nt(359), (nt(366)));
		//       option_value(359)    => severity(367).
		q(nt(359), (nt(367)));
		//       option_value_true(365) => 't'.
		q(nt(365), (t(52)));
		//       option_value_true(365) => 't' 'r' 'u' 'e'.
		q(nt(365), (t(52)+t(45)+t(44)+t(28)));
		//       option_value_true(365) => 'o' 'n'.
		q(nt(365), (t(31)+t(49)));
		//       option_value_true(365) => '1'.
		q(nt(365), (t(38)));
		//       option_value_true(365) => 'y'.
		q(nt(365), (t(58)));
		//       option_value_true(365) => 'y' 'e' 's'.
		q(nt(365), (t(58)+t(28)+t(43)));
		//       option_value_false(366) => 'f'.
		q(nt(366), (t(36)));
		//       option_value_false(366) => 'f' 'a' 'l' 's' 'e'.
		q(nt(366), (t(36)+t(26)+t(27)+t(43)+t(28)));
		//       option_value_false(366) => 'o' 'f' 'f'.
		q(nt(366), (t(31)+t(36)+t(36)));
		//       option_value_false(366) => '0'.
		q(nt(366), (t(39)));
		//       option_value_false(366) => 'n'.
		q(nt(366), (t(49)));
		//       option_value_false(366) => 'n' 'o'.
		q(nt(366), (t(49)+t(31)));
		//       severity(367)        => error_sym(368).
		q(nt(367), (nt(368)));
		//       severity(367)        => debug_sym(369).
		q(nt(367), (nt(369)));
		//       severity(367)        => trace_sym(370).
		q(nt(367), (nt(370)));
		//       severity(367)        => info_sym(371).
		q(nt(367), (nt(371)));
		//       error_sym(368)       => 'e'.
		q(nt(368), (t(28)));
		//       error_sym(368)       => 'e' 'r' 'r' 'o' 'r'.
		q(nt(368), (t(28)+t(45)+t(45)+t(31)+t(45)));
		//       info_sym(371)        => 'i'.
		q(nt(371), (t(41)));
		//       info_sym(371)        => 'i' 'n' 'f' 'o'.
		q(nt(371), (t(41)+t(49)+t(36)+t(31)));
		//       debug_sym(369)       => 'd'.
		q(nt(369), (t(53)));
		//       debug_sym(369)       => 'd' 'e' 'b' 'u' 'g'.
		q(nt(369), (t(53)+t(28)+t(30)+t(44)+t(50)));
		//       trace_sym(370)       => 't'.
		q(nt(370), (t(52)));
		//       trace_sym(370)       => 't' 'r' 'a' 'c' 'e'.
		q(nt(370), (t(52)+t(45)+t(26)+t(47)+t(28)));
		return q;
	}
};

#endif // __TAU_PARSER_H__
