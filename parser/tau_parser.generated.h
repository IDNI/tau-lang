// This file is generated from a file parser/tau.tgf by
//       https://github.com/IDNI/parser/tools/tgf
//
#ifndef __TAU_PARSER_H__
#define __TAU_PARSER_H__
#include <string.h>
#include "parser.h"
struct tau_parser {
	tau_parser() :
		nts(load_nonterminals()), cc(load_cc()),
		g(nts, load_prods(), nt(233), cc), p(g, load_opts()) {}
	std::unique_ptr<typename idni::parser<char, char>::pforest> parse(
		const char* data, size_t size = 0, size_t max_l = 0,
		char eof = std::char_traits<char>::eof())
			{ return p.parse(data, size, max_l, eof); }
	std::unique_ptr<typename idni::parser<char, char>::pforest> parse(
		std::basic_istream<char>& is, size_t max_l = 0,
		char eof = std::char_traits<char>::eof())
			{ return p.parse(is, max_l, eof); }
	std::unique_ptr<typename idni::parser<char, char>::pforest> parse(
		std::string fn, mmap_mode m, size_t max_l = 0,
		char eof = std::char_traits<char>::eof())
			{ return p.parse(fn, m, max_l, eof); }
#ifndef WIN32
	std::unique_ptr<typename idni::parser<char, char>::pforest> parse(
		int fd, size_t max_l = 0,
		char eof = std::char_traits<char>::eof())
			{ return p.parse(fd, max_l, eof); }
#endif //WIN32
	bool found() { return p.found(); }
	typename idni::parser<char, char>::error get_error()
		{ return p.get_error(); }
	enum nonterminal {
		nul, eof, space, digit, xdigit, alpha, alnum, punct, printable, eol,
		ws_comment, _Rws_comment_0, _Rws_comment_1, ws_required, ws, hex_escape, unicode_escape, char_escape_encode, esc, q_char,
		q_str, q_bqstr, char_punct, _Rchar_punct_2, _Rchar_punct_3, _Rchar_punct_4, char0, char_, string_char, bqstring_char,
		chars, _Rchars_5, _Rchars_6, digits, _Rdigits_7, bf_def, wff_def, tau_def, dot, open_parenthesis,
		close_parenthesis, open_bracket, close_bracket, open_brace, close_brace, minus, colon, semicolon, less, comma,
		sym, offsets, offset, _Roffsets_8, _Roffsets_9, num, capture, shift, variable, var,
		io_var, pos_var, in, out, in_var_name, var_pos, out_var_name, current_pos, absolute_pos, relative_pos,
		bool_variable, capture_var, tau_rule, tau_matcher, tau_body, tau, tau_collapse_positives_cb, tau_positives_upwards_cb, wff, tau_and,
		tau_or, tau_neg, tau_and_sym, tau_or_sym, tau_neg_sym, wff_rule, wff_matcher, wff_body, wff_rec_relation, wff_ref,
		bf_eq_cb, bf_neq_cb, wff_has_clashing_subformulas_cb, wff_has_subformula_cb, wff_remove_existential_cb, wff_remove_bexistential_cb, wff_remove_buniversal_cb, wff_and, wff_neg, wff_xor,
		wff_conditional, wff_or, wff_all, wff_ex, wff_imply, wff_equiv, wff_t, wff_f, wff_ball, wff_bex,
		bf_eq, bf_neq, bf_less, bf_less_equal, bf_greater, wff_ref_args, _Rwff_ref_args_10, _Rwff_ref_args_11, _Rwff_ref_args_12, wff_and_sym,
		wff_or_sym, wff_xor_sym, wff_conditional_sym, wff_neg_sym, wff_imply_sym, wff_equiv_sym, wff_all_sym, _Rwff_all_13, wff_ex_sym, _Rwff_ex_14,
		wff_ball_sym, _Rwff_ball_15, wff_bex_sym, _Rwff_bex_16, bf, bf_equality_sym, bf_nequality_sym, bf_less_sym, bf_less_equal_sym, bf_greater_sym,
		bf_rule, bf_matcher, bf_body, bf_rec_relation, bf_ref, bf_is_zero_cb, bf_is_one_cb, bf_has_clashing_subformulas_cb, bf_has_subformula_cb, bf_remove_funiversal_cb,
		bf_remove_fexistential_cb, bf_and_cb, bf_or_cb, bf_xor_cb, bf_neg_cb, bf_constant, bf_and, bf_neg, bf_xor, bf_or,
		bf_all, bf_ex, bf_t, bf_f, bf_ref_args, _Rbf_ref_args_17, _Rbf_ref_args_18, _Rbf_ref_args_19, bf_and_sym, bf_or_sym,
		bf_xor_sym, bf_neg_sym, bf_all_sym, _Rbf_all_20, bf_ex_sym, _Rbf_ex_21, constant, binding, source_binding, named_binding,
		type, source, source0, _Rsource_22, _Rsource_23, bf_and_cb_sym, bf_cb_arg, bf_or_cb_sym, bf_xor_cb_sym, bf_neg_cb_sym,
		bf_eq_cb_sym, wff_cb_arg, bf_neq_cb_sym, bf_is_zero_cb_sym, bf_is_one_cb_sym, bf_remove_funiversal_cb_sym, bf_remove_fexistential_cb_sym, wff_remove_existential_cb_sym, wff_remove_bexistential_cb_sym, wff_remove_buniversal_cb_sym,
		bf_has_clashing_subformulas_cb_sym, wff_has_clashing_subformulas_cb_sym, bf_has_subformula_cb_sym, wff_has_subformula_cb_sym, tau_collapse_positives_cb_sym, tau_cb_arg, _Rtau_collapse_positives_cb_24, _Rtau_collapse_positives_cb_25, tau_positives_upwards_cb_sym, input,
		inputs, _Rinputs_26, _Rinputs_27, main, rule, rules, _Rrules_28, _Rrules_29, rec_relation, rec_relations,
		_Rrec_relations_30, _Rrec_relations_31, nso_rr, library, builder, builder_head, builder_body, _Rbuilder_head_32, _Rbuilder_head_33, bf_builder_body,
		wff_builder_body, tau_builder_body, gssotc, start, _Rstart_34, __neg_0, __neg_1, __neg_2, __neg_3, __neg_4,
		__neg_5, __neg_6, __neg_7,
	};
	size_t id(const std::basic_string<char>& name) { return nts.get(name); }
private:
	std::vector<char> ts{
		'\0', '\n', '\r', '\t', '#', '\\', 'x', 'u', '\'',
		'"', '`', ':', '=', '.', '(', ')', '[', ']', '{',
		'}', '-', ';', '<', ',', '?', '$', 't', 'i', '_',
		'o', '&', '|', '!', '^', '>', 'a', 'l', 'e', 'b',
		'T', 'F', '+', '~', 'f', '1', '0', 'h', 's', 'c',
		'n', 'g', 'r', 'm', 'w', 'v', 'd', 'q', 'z', 'p',
	};
	idni::nonterminals<char, char> nts{};
	idni::char_class_fns<char> cc;
	idni::grammar<char, char> g;
	idni::parser<char, char> p;
	idni::prods<char, char> t(size_t tid) {
		return idni::prods<char, char>(ts[tid]);
	}
	idni::prods<char, char> nt(size_t ntid) {
		return idni::prods<char, char>(idni::lit<char, char>(ntid, &nts));
	}
	idni::nonterminals<char, char> load_nonterminals() const {
		idni::nonterminals<char, char> nts{};
		for (const auto& nt : {
			"", "eof", "space", "digit", "xdigit", "alpha", "alnum", "punct", "printable", "eol",
			"ws_comment", "_Rws_comment_0", "_Rws_comment_1", "ws_required", "ws", "hex_escape", "unicode_escape", "char_escape_encode", "esc", "q_char",
			"q_str", "q_bqstr", "char_punct", "_Rchar_punct_2", "_Rchar_punct_3", "_Rchar_punct_4", "char0", "char_", "string_char", "bqstring_char",
			"chars", "_Rchars_5", "_Rchars_6", "digits", "_Rdigits_7", "bf_def", "wff_def", "tau_def", "dot", "open_parenthesis",
			"close_parenthesis", "open_bracket", "close_bracket", "open_brace", "close_brace", "minus", "colon", "semicolon", "less", "comma",
			"sym", "offsets", "offset", "_Roffsets_8", "_Roffsets_9", "num", "capture", "shift", "variable", "var",
			"io_var", "pos_var", "in", "out", "in_var_name", "var_pos", "out_var_name", "current_pos", "absolute_pos", "relative_pos",
			"bool_variable", "capture_var", "tau_rule", "tau_matcher", "tau_body", "tau", "tau_collapse_positives_cb", "tau_positives_upwards_cb", "wff", "tau_and",
			"tau_or", "tau_neg", "tau_and_sym", "tau_or_sym", "tau_neg_sym", "wff_rule", "wff_matcher", "wff_body", "wff_rec_relation", "wff_ref",
			"bf_eq_cb", "bf_neq_cb", "wff_has_clashing_subformulas_cb", "wff_has_subformula_cb", "wff_remove_existential_cb", "wff_remove_bexistential_cb", "wff_remove_buniversal_cb", "wff_and", "wff_neg", "wff_xor",
			"wff_conditional", "wff_or", "wff_all", "wff_ex", "wff_imply", "wff_equiv", "wff_t", "wff_f", "wff_ball", "wff_bex",
			"bf_eq", "bf_neq", "bf_less", "bf_less_equal", "bf_greater", "wff_ref_args", "_Rwff_ref_args_10", "_Rwff_ref_args_11", "_Rwff_ref_args_12", "wff_and_sym",
			"wff_or_sym", "wff_xor_sym", "wff_conditional_sym", "wff_neg_sym", "wff_imply_sym", "wff_equiv_sym", "wff_all_sym", "_Rwff_all_13", "wff_ex_sym", "_Rwff_ex_14",
			"wff_ball_sym", "_Rwff_ball_15", "wff_bex_sym", "_Rwff_bex_16", "bf", "bf_equality_sym", "bf_nequality_sym", "bf_less_sym", "bf_less_equal_sym", "bf_greater_sym",
			"bf_rule", "bf_matcher", "bf_body", "bf_rec_relation", "bf_ref", "bf_is_zero_cb", "bf_is_one_cb", "bf_has_clashing_subformulas_cb", "bf_has_subformula_cb", "bf_remove_funiversal_cb",
			"bf_remove_fexistential_cb", "bf_and_cb", "bf_or_cb", "bf_xor_cb", "bf_neg_cb", "bf_constant", "bf_and", "bf_neg", "bf_xor", "bf_or",
			"bf_all", "bf_ex", "bf_t", "bf_f", "bf_ref_args", "_Rbf_ref_args_17", "_Rbf_ref_args_18", "_Rbf_ref_args_19", "bf_and_sym", "bf_or_sym",
			"bf_xor_sym", "bf_neg_sym", "bf_all_sym", "_Rbf_all_20", "bf_ex_sym", "_Rbf_ex_21", "constant", "binding", "source_binding", "named_binding",
			"type", "source", "source0", "_Rsource_22", "_Rsource_23", "bf_and_cb_sym", "bf_cb_arg", "bf_or_cb_sym", "bf_xor_cb_sym", "bf_neg_cb_sym",
			"bf_eq_cb_sym", "wff_cb_arg", "bf_neq_cb_sym", "bf_is_zero_cb_sym", "bf_is_one_cb_sym", "bf_remove_funiversal_cb_sym", "bf_remove_fexistential_cb_sym", "wff_remove_existential_cb_sym", "wff_remove_bexistential_cb_sym", "wff_remove_buniversal_cb_sym",
			"bf_has_clashing_subformulas_cb_sym", "wff_has_clashing_subformulas_cb_sym", "bf_has_subformula_cb_sym", "wff_has_subformula_cb_sym", "tau_collapse_positives_cb_sym", "tau_cb_arg", "_Rtau_collapse_positives_cb_24", "_Rtau_collapse_positives_cb_25", "tau_positives_upwards_cb_sym", "input",
			"inputs", "_Rinputs_26", "_Rinputs_27", "main", "rule", "rules", "_Rrules_28", "_Rrules_29", "rec_relation", "rec_relations",
			"_Rrec_relations_30", "_Rrec_relations_31", "nso_rr", "library", "builder", "builder_head", "builder_body", "_Rbuilder_head_32", "_Rbuilder_head_33", "bf_builder_body",
			"wff_builder_body", "tau_builder_body", "gssotc", "start", "_Rstart_34", "__neg_0", "__neg_1", "__neg_2", "__neg_3", "__neg_4",
			"__neg_5", "__neg_6", "__neg_7",
		}) nts.get(nt);
		return nts;
	}
	idni::char_class_fns<char> load_cc() {
		return idni::predefined_char_classes<char, char>({
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
	idni::parser<char, char>::options load_opts() {
		idni::parser<char, char>::options o;
		return o;
	}
	idni::prods<char, char> load_prods() {
		idni::prods<char, char> q, nul(idni::lit<char, char>{});
		// eol => '\n'.
		q(nt(9), (t(1)));
		// eol => '\r'.
		q(nt(9), (t(2)));
		// eol => eof.
		q(nt(9), (nt(1)));
		// _Rws_comment_0 => '\t'.
		q(nt(11), (t(3)));
		// _Rws_comment_0 => printable.
		q(nt(11), (nt(8)));
		// _Rws_comment_1 => null.
		q(nt(12), (nul));
		// _Rws_comment_1 => _Rws_comment_0 _Rws_comment_1.
		q(nt(12), (nt(11)+nt(12)));
		// ws_comment => '#' _Rws_comment_1 eol.
		q(nt(10), (t(4)+nt(12)+nt(9)));
		// ws_required => space ws.
		q(nt(13), (nt(2)+nt(14)));
		// ws_required => ws_comment ws.
		q(nt(13), (nt(10)+nt(14)));
		// ws => null.
		q(nt(14), (nul));
		// ws => ws_required.
		q(nt(14), (nt(13)));
		// hex_escape => '\\' '\\' 'x' xdigit xdigit.
		q(nt(15), (t(5)+t(5)+t(6)+nt(4)+nt(4)));
		// unicode_escape => '\\' '\\' 'u' xdigit xdigit xdigit xdigit.
		q(nt(16), (t(5)+t(5)+t(7)+nt(4)+nt(4)+nt(4)+nt(4)));
		// char_escape_encode => hex_escape.
		q(nt(17), (nt(15)));
		// char_escape_encode => unicode_escape.
		q(nt(17), (nt(16)));
		// esc => '\\' '\\' '\\' '\\'.
		q(nt(18), (t(5)+t(5)+t(5)+t(5)));
		// q_char => '\''.
		q(nt(19), (t(8)));
		// q_str => '"'.
		q(nt(20), (t(9)));
		// q_bqstr => '`'.
		q(nt(21), (t(10)));
		// _Rchar_punct_2 => esc q_char.
		q(nt(23), (nt(18)+nt(19)));
		// _Rchar_punct_3 => esc q_str.
		q(nt(24), (nt(18)+nt(20)));
		// _Rchar_punct_4 => esc q_bqstr.
		q(nt(25), (nt(18)+nt(21)));
		// __neg_0 => q_char.
		q(nt(235), (nt(19)));
		// __neg_1 => q_str.
		q(nt(236), (nt(20)));
		// __neg_2 => q_bqstr.
		q(nt(237), (nt(21)));
		// __neg_3 => _Rchar_punct_2.
		q(nt(238), (nt(23)));
		// __neg_4 => _Rchar_punct_3.
		q(nt(239), (nt(24)));
		// __neg_5 => _Rchar_punct_4.
		q(nt(240), (nt(25)));
		// char_punct => punct & ~( __neg_0 ) & ~( __neg_1 ) & ~( __neg_2 ) & ~( __neg_3 ) & ~( __neg_4 ) & ~( __neg_5 ).
		q(nt(22), (nt(7)) & ~(nt(235)) & ~(nt(236)) & ~(nt(237)) & ~(nt(238)) & ~(nt(239)) & ~(nt(240)));
		// char0 => space.
		q(nt(26), (nt(2)));
		// char0 => alnum.
		q(nt(26), (nt(6)));
		// char0 => char_escape_encode.
		q(nt(26), (nt(17)));
		// char0 => char_punct.
		q(nt(26), (nt(22)));
		// char_ => esc q_char.
		q(nt(27), (nt(18)+nt(19)));
		// char_ => q_str.
		q(nt(27), (nt(20)));
		// char_ => q_bqstr.
		q(nt(27), (nt(21)));
		// char_ => char0.
		q(nt(27), (nt(26)));
		// string_char => esc q_str.
		q(nt(28), (nt(18)+nt(20)));
		// string_char => q_char.
		q(nt(28), (nt(19)));
		// string_char => q_bqstr.
		q(nt(28), (nt(21)));
		// string_char => char0.
		q(nt(28), (nt(26)));
		// bqstring_char => esc q_bqstr.
		q(nt(29), (nt(18)+nt(21)));
		// bqstring_char => q_char.
		q(nt(29), (nt(19)));
		// bqstring_char => q_str.
		q(nt(29), (nt(20)));
		// bqstring_char => char0.
		q(nt(29), (nt(26)));
		// _Rchars_5 => alnum.
		q(nt(31), (nt(6)));
		// _Rchars_6 => null.
		q(nt(32), (nul));
		// _Rchars_6 => _Rchars_5 _Rchars_6.
		q(nt(32), (nt(31)+nt(32)));
		// chars => alpha _Rchars_6.
		q(nt(30), (nt(5)+nt(32)));
		// _Rdigits_7 => digit.
		q(nt(34), (nt(3)));
		// _Rdigits_7 => digit _Rdigits_7.
		q(nt(34), (nt(3)+nt(34)));
		// digits => _Rdigits_7.
		q(nt(33), (nt(34)));
		// bf_def => ':' '='.
		q(nt(35), (t(11)+t(12)));
		// wff_def => ':' ':' '='.
		q(nt(36), (t(11)+t(11)+t(12)));
		// tau_def => ':' ':' ':' '='.
		q(nt(37), (t(11)+t(11)+t(11)+t(12)));
		// dot => '.'.
		q(nt(38), (t(13)));
		// open_parenthesis => '('.
		q(nt(39), (t(14)));
		// close_parenthesis => ')'.
		q(nt(40), (t(15)));
		// open_bracket => '['.
		q(nt(41), (t(16)));
		// close_bracket => ']'.
		q(nt(42), (t(17)));
		// open_brace => '{'.
		q(nt(43), (t(18)));
		// close_brace => '}'.
		q(nt(44), (t(19)));
		// minus => '-'.
		q(nt(45), (t(20)));
		// colon => ':'.
		q(nt(46), (t(11)));
		// semicolon => ';'.
		q(nt(47), (t(21)));
		// less => '<'.
		q(nt(48), (t(22)));
		// comma => ','.
		q(nt(49), (t(23)));
		// sym => chars.
		q(nt(50), (nt(30)));
		// _Roffsets_8 => ws comma ws offset.
		q(nt(53), (nt(14)+nt(49)+nt(14)+nt(52)));
		// _Roffsets_9 => null.
		q(nt(54), (nul));
		// _Roffsets_9 => _Roffsets_8 _Roffsets_9.
		q(nt(54), (nt(53)+nt(54)));
		// offsets => open_bracket ws offset _Roffsets_9 ws close_bracket.
		q(nt(51), (nt(41)+nt(14)+nt(52)+nt(54)+nt(14)+nt(42)));
		// offset => num.
		q(nt(52), (nt(55)));
		// offset => capture.
		q(nt(52), (nt(56)));
		// offset => shift.
		q(nt(52), (nt(57)));
		// shift => capture ws minus ws num.
		q(nt(57), (nt(56)+nt(14)+nt(45)+nt(14)+nt(55)));
		// num => digits.
		q(nt(55), (nt(33)));
		// variable => var.
		q(nt(58), (nt(59)));
		// variable => io_var.
		q(nt(58), (nt(60)));
		// variable => pos_var.
		q(nt(58), (nt(61)));
		// io_var => in.
		q(nt(60), (nt(62)));
		// io_var => out.
		q(nt(60), (nt(63)));
		// in => in_var_name var_pos.
		q(nt(62), (nt(64)+nt(65)));
		// out => out_var_name var_pos.
		q(nt(63), (nt(66)+nt(65)));
		// var_pos => current_pos.
		q(nt(65), (nt(67)));
		// var_pos => absolute_pos.
		q(nt(65), (nt(68)));
		// var_pos => relative_pos.
		q(nt(65), (nt(69)));
		// bool_variable => '?' chars.
		q(nt(70), (t(24)+nt(30)));
		// capture => capture_var.
		q(nt(56), (nt(71)));
		// capture_var => '$' chars.
		q(nt(71), (t(25)+nt(30)));
		// var => chars.
		q(nt(59), (nt(30)));
		// pos_var => 't'.
		q(nt(61), (t(26)));
		// current_pos => open_bracket ws pos_var ws close_bracket.
		q(nt(67), (nt(41)+nt(14)+nt(61)+nt(14)+nt(42)));
		// absolute_pos => open_bracket ws num ws close_bracket.
		q(nt(68), (nt(41)+nt(14)+nt(55)+nt(14)+nt(42)));
		// relative_pos => open_bracket ws pos_var ws minus ws num ws close_bracket.
		q(nt(69), (nt(41)+nt(14)+nt(61)+nt(14)+nt(45)+nt(14)+nt(55)+nt(14)+nt(42)));
		// in_var_name => 'i' '_' chars.
		q(nt(64), (t(27)+t(28)+nt(30)));
		// out_var_name => 'o' '_' chars.
		q(nt(66), (t(29)+t(28)+nt(30)));
		// tau_rule => tau_matcher ws tau_def ws tau_body ws dot.
		q(nt(72), (nt(73)+nt(14)+nt(37)+nt(14)+nt(74)+nt(14)+nt(38)));
		// tau_matcher => tau.
		q(nt(73), (nt(75)));
		// tau_body => tau.
		q(nt(74), (nt(75)));
		// tau_body => tau_collapse_positives_cb.
		q(nt(74), (nt(76)));
		// tau_body => tau_positives_upwards_cb.
		q(nt(74), (nt(77)));
		// tau => open_brace ws wff ws close_brace.
		q(nt(75), (nt(43)+nt(14)+nt(78)+nt(14)+nt(44)));
		// tau => capture.
		q(nt(75), (nt(56)));
		// tau => tau_and.
		q(nt(75), (nt(79)));
		// tau => tau_or.
		q(nt(75), (nt(80)));
		// tau => tau_neg.
		q(nt(75), (nt(81)));
		// tau_and => open_parenthesis ws tau ws tau_and_sym ws tau ws close_parenthesis.
		q(nt(79), (nt(39)+nt(14)+nt(75)+nt(14)+nt(82)+nt(14)+nt(75)+nt(14)+nt(40)));
		// tau_or => open_parenthesis ws tau ws tau_or_sym ws tau ws close_parenthesis.
		q(nt(80), (nt(39)+nt(14)+nt(75)+nt(14)+nt(83)+nt(14)+nt(75)+nt(14)+nt(40)));
		// tau_neg => tau_neg_sym ws tau.
		q(nt(81), (nt(84)+nt(14)+nt(75)));
		// tau_and_sym => '&' '&' '&'.
		q(nt(82), (t(30)+t(30)+t(30)));
		// tau_or_sym => '|' '|' '|'.
		q(nt(83), (t(31)+t(31)+t(31)));
		// tau_neg_sym => '!' '!' '!'.
		q(nt(84), (t(32)+t(32)+t(32)));
		// wff_rule => wff_matcher ws wff_def ws wff_body ws dot.
		q(nt(85), (nt(86)+nt(14)+nt(36)+nt(14)+nt(87)+nt(14)+nt(38)));
		// wff_rec_relation => wff_ref ws wff_def ws wff ws dot.
		q(nt(88), (nt(89)+nt(14)+nt(36)+nt(14)+nt(78)+nt(14)+nt(38)));
		// wff_matcher => wff.
		q(nt(86), (nt(78)));
		// wff_body => wff.
		q(nt(87), (nt(78)));
		// wff_body => bf_eq_cb.
		q(nt(87), (nt(90)));
		// wff_body => bf_neq_cb.
		q(nt(87), (nt(91)));
		// wff_body => wff_has_clashing_subformulas_cb.
		q(nt(87), (nt(92)));
		// wff_body => wff_has_subformula_cb.
		q(nt(87), (nt(93)));
		// wff_body => wff_remove_existential_cb.
		q(nt(87), (nt(94)));
		// wff_body => wff_remove_bexistential_cb.
		q(nt(87), (nt(95)));
		// wff_body => wff_remove_buniversal_cb.
		q(nt(87), (nt(96)));
		// wff => capture.
		q(nt(78), (nt(56)));
		// wff => bool_variable.
		q(nt(78), (nt(70)));
		// wff => wff_ref.
		q(nt(78), (nt(89)));
		// wff => wff_and.
		q(nt(78), (nt(97)));
		// wff => wff_neg.
		q(nt(78), (nt(98)));
		// wff => wff_xor.
		q(nt(78), (nt(99)));
		// wff => wff_conditional.
		q(nt(78), (nt(100)));
		// wff => wff_or.
		q(nt(78), (nt(101)));
		// wff => wff_all.
		q(nt(78), (nt(102)));
		// wff => wff_ex.
		q(nt(78), (nt(103)));
		// wff => wff_imply.
		q(nt(78), (nt(104)));
		// wff => wff_equiv.
		q(nt(78), (nt(105)));
		// wff => wff_t.
		q(nt(78), (nt(106)));
		// wff => wff_f.
		q(nt(78), (nt(107)));
		// wff => wff_ball.
		q(nt(78), (nt(108)));
		// wff => wff_bex.
		q(nt(78), (nt(109)));
		// wff => bf_eq.
		q(nt(78), (nt(110)));
		// wff => bf_neq.
		q(nt(78), (nt(111)));
		// wff => bf_less.
		q(nt(78), (nt(112)));
		// wff => bf_less_equal.
		q(nt(78), (nt(113)));
		// wff => bf_greater.
		q(nt(78), (nt(114)));
		// wff_ref => sym ws offsets ws wff_ref_args.
		q(nt(89), (nt(50)+nt(14)+nt(51)+nt(14)+nt(115)));
		// _Rwff_ref_args_10 => capture.
		q(nt(116), (nt(56)));
		// _Rwff_ref_args_10 => variable.
		q(nt(116), (nt(58)));
		// _Rwff_ref_args_11 => _Rwff_ref_args_10.
		q(nt(117), (nt(116)));
		// _Rwff_ref_args_12 => null.
		q(nt(118), (nul));
		// _Rwff_ref_args_12 => _Rwff_ref_args_11 _Rwff_ref_args_12.
		q(nt(118), (nt(117)+nt(118)));
		// wff_ref_args => open_parenthesis ws _Rwff_ref_args_12 ws close_parenthesis.
		q(nt(115), (nt(39)+nt(14)+nt(118)+nt(14)+nt(40)));
		// wff_and => open_parenthesis ws wff ws wff_and_sym ws wff ws close_parenthesis.
		q(nt(97), (nt(39)+nt(14)+nt(78)+nt(14)+nt(119)+nt(14)+nt(78)+nt(14)+nt(40)));
		// wff_or => open_parenthesis ws wff ws wff_or_sym ws wff ws close_parenthesis.
		q(nt(101), (nt(39)+nt(14)+nt(78)+nt(14)+nt(120)+nt(14)+nt(78)+nt(14)+nt(40)));
		// wff_xor => open_parenthesis ws wff ws wff_xor_sym ws wff ws close_parenthesis.
		q(nt(99), (nt(39)+nt(14)+nt(78)+nt(14)+nt(121)+nt(14)+nt(78)+nt(14)+nt(40)));
		// wff_conditional => open_parenthesis ws wff ws wff_conditional_sym ws wff ws colon ws wff ws close_parenthesis.
		q(nt(100), (nt(39)+nt(14)+nt(78)+nt(14)+nt(122)+nt(14)+nt(78)+nt(14)+nt(46)+nt(14)+nt(78)+nt(14)+nt(40)));
		// wff_neg => wff_neg_sym ws wff.
		q(nt(98), (nt(123)+nt(14)+nt(78)));
		// wff_imply => open_parenthesis ws wff ws wff_imply_sym ws wff ws close_parenthesis.
		q(nt(104), (nt(39)+nt(14)+nt(78)+nt(14)+nt(124)+nt(14)+nt(78)+nt(14)+nt(40)));
		// wff_equiv => open_parenthesis ws wff ws wff_equiv_sym ws wff ws close_parenthesis.
		q(nt(105), (nt(39)+nt(14)+nt(78)+nt(14)+nt(125)+nt(14)+nt(78)+nt(14)+nt(40)));
		// _Rwff_all_13 => capture.
		q(nt(127), (nt(56)));
		// _Rwff_all_13 => variable.
		q(nt(127), (nt(58)));
		// wff_all => wff_all_sym ws_required _Rwff_all_13 ws_required wff.
		q(nt(102), (nt(126)+nt(13)+nt(127)+nt(13)+nt(78)));
		// _Rwff_ex_14 => capture.
		q(nt(129), (nt(56)));
		// _Rwff_ex_14 => variable.
		q(nt(129), (nt(58)));
		// wff_ex => wff_ex_sym ws_required _Rwff_ex_14 ws_required wff.
		q(nt(103), (nt(128)+nt(13)+nt(129)+nt(13)+nt(78)));
		// _Rwff_ball_15 => capture.
		q(nt(131), (nt(56)));
		// _Rwff_ball_15 => bool_variable.
		q(nt(131), (nt(70)));
		// wff_ball => wff_ball_sym ws_required _Rwff_ball_15 ws_required wff.
		q(nt(108), (nt(130)+nt(13)+nt(131)+nt(13)+nt(78)));
		// _Rwff_bex_16 => capture.
		q(nt(133), (nt(56)));
		// _Rwff_bex_16 => bool_variable.
		q(nt(133), (nt(70)));
		// wff_bex => wff_bex_sym ws_required _Rwff_bex_16 ws_required wff.
		q(nt(109), (nt(132)+nt(13)+nt(133)+nt(13)+nt(78)));
		// bf_eq => open_parenthesis ws bf ws bf_equality_sym ws bf ws close_parenthesis.
		q(nt(110), (nt(39)+nt(14)+nt(134)+nt(14)+nt(135)+nt(14)+nt(134)+nt(14)+nt(40)));
		// bf_neq => open_parenthesis ws bf ws bf_nequality_sym ws bf ws close_parenthesis.
		q(nt(111), (nt(39)+nt(14)+nt(134)+nt(14)+nt(136)+nt(14)+nt(134)+nt(14)+nt(40)));
		// bf_less => open_parenthesis ws bf ws bf_less_sym ws bf ws close_parenthesis.
		q(nt(112), (nt(39)+nt(14)+nt(134)+nt(14)+nt(137)+nt(14)+nt(134)+nt(14)+nt(40)));
		// bf_less_equal => open_parenthesis ws bf ws bf_less_equal_sym ws bf ws close_parenthesis.
		q(nt(113), (nt(39)+nt(14)+nt(134)+nt(14)+nt(138)+nt(14)+nt(134)+nt(14)+nt(40)));
		// bf_greater => open_parenthesis ws bf ws bf_greater_sym ws bf ws close_parenthesis.
		q(nt(114), (nt(39)+nt(14)+nt(134)+nt(14)+nt(139)+nt(14)+nt(134)+nt(14)+nt(40)));
		// wff_and_sym => '&' '&'.
		q(nt(119), (t(30)+t(30)));
		// wff_or_sym => '|' '|'.
		q(nt(120), (t(31)+t(31)));
		// wff_xor_sym => '^'.
		q(nt(121), (t(33)));
		// wff_conditional_sym => '?'.
		q(nt(122), (t(24)));
		// wff_neg_sym => '!'.
		q(nt(123), (t(32)));
		// wff_imply_sym => '-' '>'.
		q(nt(124), (t(20)+t(34)));
		// wff_equiv_sym => '<' '-' '>'.
		q(nt(125), (t(22)+t(20)+t(34)));
		// wff_all_sym => 'a' 'l' 'l'.
		q(nt(126), (t(35)+t(36)+t(36)));
		// wff_ex_sym => 'e' 'x'.
		q(nt(128), (t(37)+t(6)));
		// wff_ball_sym => 'b' 'a' 'l' 'l'.
		q(nt(130), (t(38)+t(35)+t(36)+t(36)));
		// wff_bex_sym => 'b' 'e' 'x'.
		q(nt(132), (t(38)+t(37)+t(6)));
		// wff_t => 'T'.
		q(nt(106), (t(39)));
		// wff_f => 'F'.
		q(nt(107), (t(40)));
		// bf_rule => bf_matcher ws bf_def ws bf_body ws dot.
		q(nt(140), (nt(141)+nt(14)+nt(35)+nt(14)+nt(142)+nt(14)+nt(38)));
		// bf_rec_relation => bf_ref ws bf_def ws bf ws dot.
		q(nt(143), (nt(144)+nt(14)+nt(35)+nt(14)+nt(134)+nt(14)+nt(38)));
		// bf_matcher => bf.
		q(nt(141), (nt(134)));
		// bf_body => bf.
		q(nt(142), (nt(134)));
		// bf_body => bf_is_zero_cb.
		q(nt(142), (nt(145)));
		// bf_body => bf_is_one_cb.
		q(nt(142), (nt(146)));
		// bf_body => bf_has_clashing_subformulas_cb.
		q(nt(142), (nt(147)));
		// bf_body => bf_has_subformula_cb.
		q(nt(142), (nt(148)));
		// bf_body => bf_remove_funiversal_cb.
		q(nt(142), (nt(149)));
		// bf_body => bf_remove_fexistential_cb.
		q(nt(142), (nt(150)));
		// bf_body => bf_and_cb.
		q(nt(142), (nt(151)));
		// bf_body => bf_or_cb.
		q(nt(142), (nt(152)));
		// bf_body => bf_xor_cb.
		q(nt(142), (nt(153)));
		// bf_body => bf_neg_cb.
		q(nt(142), (nt(154)));
		// bf => capture.
		q(nt(134), (nt(56)));
		// bf => variable.
		q(nt(134), (nt(58)));
		// bf => bf_ref.
		q(nt(134), (nt(144)));
		// bf => bf_constant.
		q(nt(134), (nt(155)));
		// bf => bf_and.
		q(nt(134), (nt(156)));
		// bf => bf_neg.
		q(nt(134), (nt(157)));
		// bf => bf_xor.
		q(nt(134), (nt(158)));
		// bf => bf_or.
		q(nt(134), (nt(159)));
		// bf => bf_all.
		q(nt(134), (nt(160)));
		// bf => bf_ex.
		q(nt(134), (nt(161)));
		// bf => bf_t.
		q(nt(134), (nt(162)));
		// bf => bf_f.
		q(nt(134), (nt(163)));
		// bf_ref => sym offsets bf_ref_args.
		q(nt(144), (nt(50)+nt(51)+nt(164)));
		// _Rbf_ref_args_17 => capture.
		q(nt(165), (nt(56)));
		// _Rbf_ref_args_17 => variable.
		q(nt(165), (nt(58)));
		// _Rbf_ref_args_18 => _Rbf_ref_args_17.
		q(nt(166), (nt(165)));
		// _Rbf_ref_args_19 => null.
		q(nt(167), (nul));
		// _Rbf_ref_args_19 => _Rbf_ref_args_18 _Rbf_ref_args_19.
		q(nt(167), (nt(166)+nt(167)));
		// bf_ref_args => open_parenthesis ws _Rbf_ref_args_19 ws close_parenthesis.
		q(nt(164), (nt(39)+nt(14)+nt(167)+nt(14)+nt(40)));
		// bf_and => open_parenthesis ws bf ws bf_and_sym ws bf ws close_parenthesis.
		q(nt(156), (nt(39)+nt(14)+nt(134)+nt(14)+nt(168)+nt(14)+nt(134)+nt(14)+nt(40)));
		// bf_or => open_parenthesis ws bf ws bf_or_sym ws bf ws close_parenthesis.
		q(nt(159), (nt(39)+nt(14)+nt(134)+nt(14)+nt(169)+nt(14)+nt(134)+nt(14)+nt(40)));
		// bf_xor => open_parenthesis ws bf ws bf_xor_sym ws bf ws close_parenthesis.
		q(nt(158), (nt(39)+nt(14)+nt(134)+nt(14)+nt(170)+nt(14)+nt(134)+nt(14)+nt(40)));
		// bf_neg => bf_neg_sym ws bf.
		q(nt(157), (nt(171)+nt(14)+nt(134)));
		// _Rbf_all_20 => capture.
		q(nt(173), (nt(56)));
		// _Rbf_all_20 => variable.
		q(nt(173), (nt(58)));
		// bf_all => bf_all_sym ws_required _Rbf_all_20 ws_required bf.
		q(nt(160), (nt(172)+nt(13)+nt(173)+nt(13)+nt(134)));
		// _Rbf_ex_21 => capture.
		q(nt(175), (nt(56)));
		// _Rbf_ex_21 => variable.
		q(nt(175), (nt(58)));
		// bf_ex => bf_ex_sym ws_required _Rbf_ex_21 ws_required bf.
		q(nt(161), (nt(174)+nt(13)+nt(175)+nt(13)+nt(134)));
		// bf_and_sym => '&'.
		q(nt(168), (t(30)));
		// bf_or_sym => '|'.
		q(nt(169), (t(31)));
		// bf_xor_sym => '+'.
		q(nt(170), (t(41)));
		// bf_neg_sym => '~'.
		q(nt(171), (t(42)));
		// bf_equality_sym => '='.
		q(nt(135), (t(12)));
		// bf_nequality_sym => '!' '='.
		q(nt(136), (t(32)+t(12)));
		// bf_less_sym => '<'.
		q(nt(137), (t(22)));
		// bf_less_equal_sym => '<' '='.
		q(nt(138), (t(22)+t(12)));
		// bf_greater_sym => '>'.
		q(nt(139), (t(34)));
		// bf_all_sym => 'f' 'a' 'l' 'l'.
		q(nt(172), (t(43)+t(35)+t(36)+t(36)));
		// bf_ex_sym => 'f' 'e' 'x'.
		q(nt(174), (t(43)+t(37)+t(6)));
		// bf_t => '1'.
		q(nt(162), (t(44)));
		// bf_f => '0'.
		q(nt(163), (t(45)));
		// bf_constant => open_brace ws constant ws close_brace.
		q(nt(155), (nt(43)+nt(14)+nt(176)+nt(14)+nt(44)));
		// constant => capture.
		q(nt(176), (nt(56)));
		// constant => binding.
		q(nt(176), (nt(177)));
		// binding => source_binding.
		q(nt(177), (nt(178)));
		// binding => named_binding.
		q(nt(177), (nt(179)));
		// named_binding => chars.
		q(nt(179), (nt(30)));
		// source_binding => type ws colon ws source.
		q(nt(178), (nt(180)+nt(14)+nt(46)+nt(14)+nt(181)));
		// type => null.
		q(nt(180), (nul));
		// type => chars.
		q(nt(180), (nt(30)));
		// source0 => space.
		q(nt(182), (nt(2)));
		// source0 => alnum.
		q(nt(182), (nt(6)));
		// source0 => punct.
		q(nt(182), (nt(7)));
		// _Rsource_22 => source0.
		q(nt(183), (nt(182)));
		// _Rsource_23 => _Rsource_22.
		q(nt(184), (nt(183)));
		// _Rsource_23 => _Rsource_22 _Rsource_23.
		q(nt(184), (nt(183)+nt(184)));
		// source => _Rsource_23.
		q(nt(181), (nt(184)));
		// bf_and_cb => bf_and_cb_sym ws_required bf_cb_arg ws_required bf_cb_arg.
		q(nt(151), (nt(185)+nt(13)+nt(186)+nt(13)+nt(186)));
		// bf_or_cb => bf_or_cb_sym ws_required bf_cb_arg ws_required bf_cb_arg.
		q(nt(152), (nt(187)+nt(13)+nt(186)+nt(13)+nt(186)));
		// bf_xor_cb => bf_xor_cb_sym ws_required bf_cb_arg ws_required bf_cb_arg.
		q(nt(153), (nt(188)+nt(13)+nt(186)+nt(13)+nt(186)));
		// bf_neg_cb => bf_neg_cb_sym ws_required bf_cb_arg.
		q(nt(154), (nt(189)+nt(13)+nt(186)));
		// bf_eq_cb => bf_eq_cb_sym ws bf_cb_arg ws_required wff_cb_arg ws_required wff_cb_arg.
		q(nt(90), (nt(190)+nt(14)+nt(186)+nt(13)+nt(191)+nt(13)+nt(191)));
		// bf_neq_cb => bf_neq_cb_sym ws bf_cb_arg ws_required wff_cb_arg ws_required wff_cb_arg.
		q(nt(91), (nt(192)+nt(14)+nt(186)+nt(13)+nt(191)+nt(13)+nt(191)));
		// bf_is_zero_cb => bf_is_zero_cb_sym ws bf_cb_arg ws_required bf_cb_arg.
		q(nt(145), (nt(193)+nt(14)+nt(186)+nt(13)+nt(186)));
		// bf_is_one_cb => bf_is_one_cb_sym ws bf_cb_arg ws_required bf_cb_arg.
		q(nt(146), (nt(194)+nt(14)+nt(186)+nt(13)+nt(186)));
		// bf_remove_funiversal_cb => bf_remove_funiversal_cb_sym ws bf_cb_arg ws_required bf_cb_arg ws_required bf_cb_arg ws_required bf_cb_arg.
		q(nt(149), (nt(195)+nt(14)+nt(186)+nt(13)+nt(186)+nt(13)+nt(186)+nt(13)+nt(186)));
		// bf_remove_fexistential_cb => bf_remove_fexistential_cb_sym ws bf_cb_arg ws_required bf_cb_arg ws_required bf_cb_arg ws_required bf_cb_arg.
		q(nt(150), (nt(196)+nt(14)+nt(186)+nt(13)+nt(186)+nt(13)+nt(186)+nt(13)+nt(186)));
		// wff_remove_existential_cb => wff_remove_existential_cb_sym ws wff_cb_arg ws_required wff_cb_arg.
		q(nt(94), (nt(197)+nt(14)+nt(191)+nt(13)+nt(191)));
		// wff_remove_bexistential_cb => wff_remove_bexistential_cb_sym ws wff_cb_arg ws_required wff_cb_arg ws_required wff_cb_arg ws_required wff_cb_arg.
		q(nt(95), (nt(198)+nt(14)+nt(191)+nt(13)+nt(191)+nt(13)+nt(191)+nt(13)+nt(191)));
		// wff_remove_buniversal_cb => wff_remove_buniversal_cb_sym ws wff_cb_arg ws_required wff_cb_arg ws_required wff_cb_arg ws_required wff_cb_arg.
		q(nt(96), (nt(199)+nt(14)+nt(191)+nt(13)+nt(191)+nt(13)+nt(191)+nt(13)+nt(191)));
		// bf_has_clashing_subformulas_cb => bf_has_clashing_subformulas_cb_sym ws bf_cb_arg ws_required bf_cb_arg.
		q(nt(147), (nt(200)+nt(14)+nt(186)+nt(13)+nt(186)));
		// wff_has_clashing_subformulas_cb => wff_has_clashing_subformulas_cb_sym ws wff_cb_arg ws_required wff_cb_arg.
		q(nt(92), (nt(201)+nt(14)+nt(191)+nt(13)+nt(191)));
		// bf_has_subformula_cb => bf_has_subformula_cb_sym ws bf_cb_arg ws_required bf_cb_arg ws_required bf_cb_arg.
		q(nt(148), (nt(202)+nt(14)+nt(186)+nt(13)+nt(186)+nt(13)+nt(186)));
		// wff_has_subformula_cb => wff_has_subformula_cb_sym ws wff_cb_arg ws_required wff_cb_arg ws_required wff_cb_arg.
		q(nt(93), (nt(203)+nt(14)+nt(191)+nt(13)+nt(191)+nt(13)+nt(191)));
		// bf_has_clashing_subformulas_cb_sym => 'b' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b'.
		q(nt(200), (t(38)+t(43)+t(28)+t(46)+t(35)+t(47)+t(28)+t(48)+t(36)+t(35)+t(47)+t(46)+t(27)+t(49)+t(50)+t(28)+t(47)+t(7)+t(38)+t(43)+t(29)+t(51)+t(52)+t(7)+t(36)+t(35)+t(47)+t(28)+t(48)+t(38)));
		// bf_has_subformula_cb_sym => 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b'.
		q(nt(202), (t(38)+t(43)+t(28)+t(46)+t(35)+t(47)+t(28)+t(47)+t(7)+t(38)+t(43)+t(29)+t(51)+t(52)+t(7)+t(36)+t(35)+t(28)+t(48)+t(38)));
		// wff_has_clashing_subformulas_cb_sym => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b'.
		q(nt(201), (t(53)+t(43)+t(43)+t(28)+t(46)+t(35)+t(47)+t(28)+t(48)+t(36)+t(35)+t(47)+t(46)+t(27)+t(49)+t(50)+t(28)+t(47)+t(7)+t(38)+t(43)+t(29)+t(51)+t(52)+t(7)+t(36)+t(35)+t(47)+t(28)+t(48)+t(38)));
		// wff_has_subformula_cb_sym => 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b'.
		q(nt(203), (t(53)+t(43)+t(43)+t(28)+t(46)+t(35)+t(47)+t(28)+t(47)+t(7)+t(38)+t(43)+t(29)+t(51)+t(52)+t(7)+t(36)+t(35)+t(28)+t(48)+t(38)));
		// wff_remove_existential_cb_sym => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(197), (t(53)+t(43)+t(43)+t(28)+t(51)+t(37)+t(52)+t(29)+t(54)+t(37)+t(28)+t(37)+t(6)+t(27)+t(47)+t(26)+t(37)+t(49)+t(26)+t(27)+t(35)+t(36)+t(28)+t(48)+t(38)));
		// wff_remove_bexistential_cb_sym => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(198), (t(53)+t(43)+t(43)+t(28)+t(51)+t(37)+t(52)+t(29)+t(54)+t(37)+t(28)+t(38)+t(37)+t(6)+t(27)+t(47)+t(26)+t(37)+t(49)+t(26)+t(27)+t(35)+t(36)+t(28)+t(48)+t(38)));
		// wff_remove_buniversal_cb_sym => 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'b' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b'.
		q(nt(199), (t(53)+t(43)+t(43)+t(28)+t(51)+t(37)+t(52)+t(29)+t(54)+t(37)+t(28)+t(38)+t(7)+t(49)+t(27)+t(54)+t(37)+t(51)+t(47)+t(35)+t(36)+t(28)+t(48)+t(38)));
		// bf_remove_fexistential_cb_sym => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b'.
		q(nt(196), (t(38)+t(43)+t(28)+t(51)+t(37)+t(52)+t(29)+t(54)+t(37)+t(28)+t(43)+t(37)+t(6)+t(27)+t(47)+t(26)+t(37)+t(49)+t(26)+t(27)+t(35)+t(36)+t(28)+t(48)+t(38)));
		// bf_remove_funiversal_cb_sym => 'b' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'f' 'u' 'n' 'i' 'v' 'e' 'r' 's' 'a' 'l' '_' 'c' 'b'.
		q(nt(195), (t(38)+t(43)+t(28)+t(51)+t(37)+t(52)+t(29)+t(54)+t(37)+t(28)+t(43)+t(7)+t(49)+t(27)+t(54)+t(37)+t(51)+t(47)+t(35)+t(36)+t(28)+t(48)+t(38)));
		// bf_cb_arg => capture.
		q(nt(186), (nt(56)));
		// __neg_6 => capture.
		q(nt(241), (nt(56)));
		// bf_cb_arg => ~( __neg_6 ) & bf.
		q(nt(186), ~(nt(241)) & (nt(134)));
		// wff_cb_arg => capture.
		q(nt(191), (nt(56)));
		// __neg_7 => capture.
		q(nt(242), (nt(56)));
		// wff_cb_arg => ~( __neg_7 ) & wff.
		q(nt(191), ~(nt(242)) & (nt(78)));
		// bf_and_cb_sym => 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b'.
		q(nt(185), (t(38)+t(43)+t(28)+t(35)+t(49)+t(55)+t(28)+t(48)+t(38)));
		// bf_or_cb_sym => 'b' 'f' '_' 'o' 'r' '_' 'c' 'b'.
		q(nt(187), (t(38)+t(43)+t(28)+t(29)+t(51)+t(28)+t(48)+t(38)));
		// bf_xor_cb_sym => 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b'.
		q(nt(188), (t(38)+t(43)+t(28)+t(6)+t(29)+t(51)+t(28)+t(48)+t(38)));
		// bf_neg_cb_sym => 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b'.
		q(nt(189), (t(38)+t(43)+t(28)+t(49)+t(37)+t(50)+t(28)+t(48)+t(38)));
		// bf_eq_cb_sym => 'b' 'f' '_' 'e' 'q' '_' 'c' 'b'.
		q(nt(190), (t(38)+t(43)+t(28)+t(37)+t(56)+t(28)+t(48)+t(38)));
		// bf_neq_cb_sym => 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b'.
		q(nt(192), (t(38)+t(43)+t(28)+t(49)+t(37)+t(56)+t(28)+t(48)+t(38)));
		// bf_is_zero_cb_sym => 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b'.
		q(nt(193), (t(38)+t(43)+t(28)+t(27)+t(47)+t(28)+t(57)+t(37)+t(51)+t(29)+t(28)+t(48)+t(38)));
		// bf_is_one_cb_sym => 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b'.
		q(nt(194), (t(38)+t(43)+t(28)+t(27)+t(47)+t(28)+t(29)+t(49)+t(37)+t(28)+t(48)+t(38)));
		// _Rtau_collapse_positives_cb_24 => tau_collapse_positives_cb_sym ws_required tau_cb_arg ws_required tau_cb_arg ws_required tau_cb_arg.
		q(nt(206), (nt(204)+nt(13)+nt(205)+nt(13)+nt(205)+nt(13)+nt(205)));
		// _Rtau_collapse_positives_cb_25 => tau_collapse_positives_cb_sym ws_required tau_cb_arg ws_required tau_cb_arg.
		q(nt(207), (nt(204)+nt(13)+nt(205)+nt(13)+nt(205)));
		// tau_collapse_positives_cb => _Rtau_collapse_positives_cb_24.
		q(nt(76), (nt(206)));
		// tau_collapse_positives_cb => _Rtau_collapse_positives_cb_25.
		q(nt(76), (nt(207)));
		// tau_positives_upwards_cb => tau_positives_upwards_cb_sym ws_required tau_cb_arg ws_required tau_cb_arg.
		q(nt(77), (nt(208)+nt(13)+nt(205)+nt(13)+nt(205)));
		// tau_cb_arg => capture.
		q(nt(205), (nt(56)));
		// tau_cb_arg => tau.
		q(nt(205), (nt(75)));
		// tau_collapse_positives_cb_sym => 't' 'a' 'u' '_' 'c' 'o' 'l' 'l' 'a' 'p' 's' 'e' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'c' 'b'.
		q(nt(204), (t(26)+t(35)+t(7)+t(28)+t(48)+t(29)+t(36)+t(36)+t(35)+t(58)+t(47)+t(37)+t(28)+t(58)+t(29)+t(47)+t(27)+t(26)+t(27)+t(54)+t(37)+t(47)+t(28)+t(48)+t(38)));
		// tau_positives_upwards_cb_sym => 't' 'a' 'u' '_' 'p' 'o' 's' 'i' 't' 'i' 'v' 'e' 's' '_' 'u' 'p' 'w' 'a' 'r' 'd' 's' '_' 'c' 'b'.
		q(nt(208), (t(26)+t(35)+t(7)+t(28)+t(58)+t(29)+t(47)+t(27)+t(26)+t(27)+t(54)+t(37)+t(47)+t(28)+t(7)+t(58)+t(53)+t(35)+t(51)+t(55)+t(47)+t(28)+t(48)+t(38)));
		// input => in ws colon ws open_brace ws source_binding ws close_brace.
		q(nt(209), (nt(62)+nt(14)+nt(46)+nt(14)+nt(43)+nt(14)+nt(178)+nt(14)+nt(44)));
		// _Rinputs_26 => ws input.
		q(nt(211), (nt(14)+nt(209)));
		// _Rinputs_27 => null.
		q(nt(212), (nul));
		// _Rinputs_27 => _Rinputs_26 _Rinputs_27.
		q(nt(212), (nt(211)+nt(212)));
		// inputs => ws less ws input _Rinputs_27 ws dot.
		q(nt(210), (nt(14)+nt(48)+nt(14)+nt(209)+nt(212)+nt(14)+nt(38)));
		// main => ws wff ws dot.
		q(nt(213), (nt(14)+nt(78)+nt(14)+nt(38)));
		// rule => tau_rule.
		q(nt(214), (nt(72)));
		// rule => wff_rule.
		q(nt(214), (nt(85)));
		// rule => bf_rule.
		q(nt(214), (nt(140)));
		// _Rrules_28 => ws rule.
		q(nt(216), (nt(14)+nt(214)));
		// _Rrules_29 => null.
		q(nt(217), (nul));
		// _Rrules_29 => _Rrules_28 _Rrules_29.
		q(nt(217), (nt(216)+nt(217)));
		// rules => _Rrules_29.
		q(nt(215), (nt(217)));
		// rec_relation => wff_rec_relation.
		q(nt(218), (nt(88)));
		// rec_relation => bf_rec_relation.
		q(nt(218), (nt(143)));
		// _Rrec_relations_30 => ws rec_relation.
		q(nt(220), (nt(14)+nt(218)));
		// _Rrec_relations_31 => null.
		q(nt(221), (nul));
		// _Rrec_relations_31 => _Rrec_relations_30 _Rrec_relations_31.
		q(nt(221), (nt(220)+nt(221)));
		// rec_relations => _Rrec_relations_31.
		q(nt(219), (nt(221)));
		// nso_rr => rec_relations main.
		q(nt(222), (nt(219)+nt(213)));
		// library => rules.
		q(nt(223), (nt(215)));
		// builder => ws builder_head ws builder_body ws dot.
		q(nt(224), (nt(14)+nt(225)+nt(14)+nt(226)+nt(14)+nt(38)));
		// _Rbuilder_head_32 => ws_required capture.
		q(nt(227), (nt(13)+nt(56)));
		// _Rbuilder_head_33 => null.
		q(nt(228), (nul));
		// _Rbuilder_head_33 => _Rbuilder_head_32 _Rbuilder_head_33.
		q(nt(228), (nt(227)+nt(228)));
		// builder_head => open_parenthesis ws capture _Rbuilder_head_33 ws close_parenthesis.
		q(nt(225), (nt(39)+nt(14)+nt(56)+nt(228)+nt(14)+nt(40)));
		// builder_body => bf_builder_body.
		q(nt(226), (nt(229)));
		// builder_body => wff_builder_body.
		q(nt(226), (nt(230)));
		// builder_body => tau_builder_body.
		q(nt(226), (nt(231)));
		// bf_builder_body => bf_def ws bf.
		q(nt(229), (nt(35)+nt(14)+nt(134)));
		// wff_builder_body => wff_def ws wff.
		q(nt(230), (nt(36)+nt(14)+nt(78)));
		// tau_builder_body => tau_def ws tau.
		q(nt(231), (nt(37)+nt(14)+nt(75)));
		// gssotc => ws tau ws semicolon.
		q(nt(232), (nt(14)+nt(75)+nt(14)+nt(47)));
		// _Rstart_34 => inputs.
		q(nt(234), (nt(210)));
		// _Rstart_34 => nso_rr.
		q(nt(234), (nt(222)));
		// _Rstart_34 => library.
		q(nt(234), (nt(223)));
		// _Rstart_34 => builder.
		q(nt(234), (nt(224)));
		// _Rstart_34 => gssotc.
		q(nt(234), (nt(232)));
		// start => _Rstart_34 ws.
		q(nt(233), (nt(234)+nt(14)));
		return q;
	}
};
#endif // __TAU_PARSER_H__
