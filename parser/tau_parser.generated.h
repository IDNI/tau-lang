// This file is generated from a file ./tau.tgfby 
//       https://github.com/IDNI/parser/tools/tgf
//
#ifndef __TAU_PARSER_H__
#define __TAU_PARSER_H__
#include <string.h>
#include "parser.h"
struct tau_parser {
	tau_parser() :
		nts(load_nonterminals()), cc(load_cc()),
		g(nts, load_prods(), nt(213), cc), p(g, load_opts()) {}
	std::unique_ptr<typename idni::parser<char, char>::pforest> parse(
		const char* data, size_t size = 0,
		char eof = std::char_traits<char>::eof())
			{ return p.parse(data, size, eof); }
	std::unique_ptr<typename idni::parser<char, char>::pforest> parse(
		int fd, size_t size = 0,
		char eof = std::char_traits<char>::eof())
			{ return p.parse(fd, size, eof); }
	std::unique_ptr<typename idni::parser<char, char>::pforest> parse(
		std::basic_istream<char>& is,
		size_t size = 0,
		char eof = std::char_traits<char>::eof())
			{ return p.parse(is, size, eof); }
	bool found() { return p.found(); }
	typename idni::parser<char, char>::error get_error()
		{ return p.get_error(); }
	enum nonterminal {
		nul, eof, space, digit, xdigit, alpha, alnum, punct, printable, eol, 
		ws_comment, ws_comment_0, ws_required, ws, hex_escape, unicode_escape, char_escape_encode, esc, q_char, q_str, 
		q_bqstr, char_punct, char_punct_1, char_punct_2, char_punct_3, char0, char_, string_char, bqstring_char, chars, 
		chars_4, chars_5, char_class, sym, definition, equality, nequality, dot, open_parenthesis, close_parenthesis, 
		open_bracket, close_bracket, open_brace, close_brace, minus, colon, less, indexes, index, indexes_6, 
		indexes_7, variable, capture, ignore, offset, num, num_8, captures, captures_9, captures_10, 
		var, timed, in, out, timed_11, wff_rule, wff_matcher, wff_body, wff, wff_ref, 
		bf_eq_cb, bf_neq_cb, wff_has_clashing_subformulas_cb, wff_has_subformula_cb, wff_remove_existential, wff_ref_args, wff_ref_args_12, wff_ref_args_13, wff_eq, wff_neq, 
		wff_and, wff_neg, wff_xor, cbf_if, wff_or, wff_all, wff_ex, wff_imply, wff_equiv, cbf_and_wff, 
		wff_coimply, wff_t, wff_f, cbf, wff_and_sym, wff_or_sym, wff_xor_sym, wff_neg_sym, wff_imply_sym, wff_equiv_sym, 
		wff_coimply_sym, wff_all_sym, wff_all_14, wff_ex_sym, wff_ex_15, cbf_and_wff_sym, cbf_if_sym, cbf_if_condition, cbf_then_sym, cbf_if_then, 
		cbf_else_sym, cbf_if_else, cbf_rule, cbf_matcher, cbf_body, cbf_ref, cbf_ref_args, cbf_ref_args_16, cbf_ref_args_17, bf, 
		cbf_and, cbf_neg, cbf_xor, cbf_or, cbf_imply, cbf_equiv, cbf_coimply, cbf_and_sym, cbf_or_sym, cbf_xor_sym, 
		cbf_neg_sym, cbf_imply_sym, cbf_equiv_sym, cbf_coimply_sym, bf_rule, bf_matcher, bf_body, bf_is_zero_cb, bf_is_one_cb, bf_has_clashing_subformulas_cb, 
		bf_has_subformula_cb, bf_constant, bf_and, bf_neg, bf_xor, bf_or, bf_all, bf_ex, bf_less, bf_less_equal, 
		bf_greater, bf_less_cb, bf_less_equal_cb, bf_greater_cb, bf_subs_cb, bf_t, bf_f, bf_and_sym, bf_or_sym, bf_xor_sym, 
		bf_neg_sym, bf_less_sym, bf_less_equal_sym, bf_greater_sym, bf_all_sym, bf_all_18, bf_ex_sym, bf_ex_19, constant, binding, 
		bf_and_cb, bf_or_cb, bf_xor_cb, bf_neg_cb, source_binding, named_binding, type, source, source0, source_20, 
		source_21, bf_cb_arg, bf_and_cb_sym, bf_or_cb_sym, bf_xor_cb_sym, bf_neg_cb_sym, bf_less_cb_sym, bf_less_equal_cb_sym, bf_greater_cb_sym, bf_subs_cb_sym, 
		bf_eq_cb_sym, wff_cb_arg, bf_neq_cb_sym, bf_is_zero_cb_sym, bf_is_one_cb_sym, bf_has_clashing_subformulas_cb_sym, wff_has_clashing_subformulas_cb_sym, bf_has_subformula_cb_sym, wff_has_subformula_cb_sym, wff_remove_existential_sym, 
		wff_remove_existential_22, input, inputs, inputs_23, main, rule, rules, rules_24, rules_25, formula, 
		library, builder, builder_body, start, __neg_0, __neg_1, __neg_2, __neg_3, __neg_4, __neg_5, 
	};
	size_t id(const std::basic_string<char>& name) { return nts.get(name); }
private:
	std::vector<char> ts{
		'\0', '\n', '\r', '#', '\\', 'x', 'u', '\'', '"', 
		'`', 'a', 'l', 'n', 'm', 'p', 'h', 'b', 'k', 'c', 
		't', 'r', 'd', 'i', 'g', 'e', 'o', 'f', 'w', 's', 
		':', '=', '!', '.', '(', ')', '[', ']', '{', '}', 
		'-', '<', '_', '$', '?', 'y', 'q', 'v', 'T', 'F', 
		'z', 
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
			"ws_comment", "ws_comment_0", "ws_required", "ws", "hex_escape", "unicode_escape", "char_escape_encode", "esc", "q_char", "q_str", 
			"q_bqstr", "char_punct", "char_punct_1", "char_punct_2", "char_punct_3", "char0", "char_", "string_char", "bqstring_char", "chars", 
			"chars_4", "chars_5", "char_class", "sym", "definition", "equality", "nequality", "dot", "open_parenthesis", "close_parenthesis", 
			"open_bracket", "close_bracket", "open_brace", "close_brace", "minus", "colon", "less", "indexes", "index", "indexes_6", 
			"indexes_7", "variable", "capture", "ignore", "offset", "num", "num_8", "captures", "captures_9", "captures_10", 
			"var", "timed", "in", "out", "timed_11", "wff_rule", "wff_matcher", "wff_body", "wff", "wff_ref", 
			"bf_eq_cb", "bf_neq_cb", "wff_has_clashing_subformulas_cb", "wff_has_subformula_cb", "wff_remove_existential", "wff_ref_args", "wff_ref_args_12", "wff_ref_args_13", "wff_eq", "wff_neq", 
			"wff_and", "wff_neg", "wff_xor", "cbf_if", "wff_or", "wff_all", "wff_ex", "wff_imply", "wff_equiv", "cbf_and_wff", 
			"wff_coimply", "wff_t", "wff_f", "cbf", "wff_and_sym", "wff_or_sym", "wff_xor_sym", "wff_neg_sym", "wff_imply_sym", "wff_equiv_sym", 
			"wff_coimply_sym", "wff_all_sym", "wff_all_14", "wff_ex_sym", "wff_ex_15", "cbf_and_wff_sym", "cbf_if_sym", "cbf_if_condition", "cbf_then_sym", "cbf_if_then", 
			"cbf_else_sym", "cbf_if_else", "cbf_rule", "cbf_matcher", "cbf_body", "cbf_ref", "cbf_ref_args", "cbf_ref_args_16", "cbf_ref_args_17", "bf", 
			"cbf_and", "cbf_neg", "cbf_xor", "cbf_or", "cbf_imply", "cbf_equiv", "cbf_coimply", "cbf_and_sym", "cbf_or_sym", "cbf_xor_sym", 
			"cbf_neg_sym", "cbf_imply_sym", "cbf_equiv_sym", "cbf_coimply_sym", "bf_rule", "bf_matcher", "bf_body", "bf_is_zero_cb", "bf_is_one_cb", "bf_has_clashing_subformulas_cb", 
			"bf_has_subformula_cb", "bf_constant", "bf_and", "bf_neg", "bf_xor", "bf_or", "bf_all", "bf_ex", "bf_less", "bf_less_equal", 
			"bf_greater", "bf_less_cb", "bf_less_equal_cb", "bf_greater_cb", "bf_subs_cb", "bf_t", "bf_f", "bf_and_sym", "bf_or_sym", "bf_xor_sym", 
			"bf_neg_sym", "bf_less_sym", "bf_less_equal_sym", "bf_greater_sym", "bf_all_sym", "bf_all_18", "bf_ex_sym", "bf_ex_19", "constant", "binding", 
			"bf_and_cb", "bf_or_cb", "bf_xor_cb", "bf_neg_cb", "source_binding", "named_binding", "type", "source", "source0", "source_20", 
			"source_21", "bf_cb_arg", "bf_and_cb_sym", "bf_or_cb_sym", "bf_xor_cb_sym", "bf_neg_cb_sym", "bf_less_cb_sym", "bf_less_equal_cb_sym", "bf_greater_cb_sym", "bf_subs_cb_sym", 
			"bf_eq_cb_sym", "wff_cb_arg", "bf_neq_cb_sym", "bf_is_zero_cb_sym", "bf_is_one_cb_sym", "bf_has_clashing_subformulas_cb_sym", "wff_has_clashing_subformulas_cb_sym", "bf_has_subformula_cb_sym", "wff_has_subformula_cb_sym", "wff_remove_existential_sym", 
			"wff_remove_existential_22", "input", "inputs", "inputs_23", "main", "rule", "rules", "rules_24", "rules_25", "formula", 
			"library", "builder", "builder_body", "start", "__neg_0", "__neg_1", "__neg_2", "__neg_3", "__neg_4", "__neg_5", 
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
		// eol => eof.
		q(nt(9), (nt(1)));
		// eol => '\n'.
		q(nt(9), (t(1)));
		// eol => '\r'.
		q(nt(9), (t(2)));
		// ws_comment_0 => printable.
		q(nt(11), (nt(8)));
		// ws_comment_0 => printable ws_comment_0.
		q(nt(11), (nt(8)+nt(11)));
		// ws_comment => '#' eol.
		q(nt(10), (t(3)+nt(9)));
		// ws_comment => '#' ws_comment_0 eol.
		q(nt(10), (t(3)+nt(11)+nt(9)));
		// ws_required => space ws.
		q(nt(12), (nt(2)+nt(13)));
		// ws_required => ws_comment ws.
		q(nt(12), (nt(10)+nt(13)));
		// ws => ws_required.
		q(nt(13), (nt(12)));
		// ws => ε.
		q(nt(13), (nul));
		// hex_escape => '\\' '\\' 'x' xdigit xdigit.
		q(nt(14), (t(4)+t(4)+t(5)+nt(4)+nt(4)));
		// unicode_escape => '\\' '\\' 'u' xdigit xdigit xdigit xdigit.
		q(nt(15), (t(4)+t(4)+t(6)+nt(4)+nt(4)+nt(4)+nt(4)));
		// char_escape_encode => hex_escape.
		q(nt(16), (nt(14)));
		// char_escape_encode => unicode_escape.
		q(nt(16), (nt(15)));
		// esc => '\\' '\\' '\\' '\\'.
		q(nt(17), (t(4)+t(4)+t(4)+t(4)));
		// q_char => '\''.
		q(nt(18), (t(7)));
		// q_str => '"'.
		q(nt(19), (t(8)));
		// q_bqstr => '`'.
		q(nt(20), (t(9)));
		// char_punct_1 => esc q_char.
		q(nt(22), (nt(17)+nt(18)));
		// char_punct_2 => esc q_str.
		q(nt(23), (nt(17)+nt(19)));
		// char_punct_3 => esc q_bqstr.
		q(nt(24), (nt(17)+nt(20)));
		// __neg_0 => q_char.
		q(nt(214), (nt(18)));
		// __neg_1 => q_str.
		q(nt(215), (nt(19)));
		// __neg_2 => q_bqstr.
		q(nt(216), (nt(20)));
		// __neg_3 => char_punct_1.
		q(nt(217), (nt(22)));
		// __neg_4 => char_punct_2.
		q(nt(218), (nt(23)));
		// __neg_5 => char_punct_3.
		q(nt(219), (nt(24)));
		// char_punct => punct & ~( __neg_0 ) & ~( __neg_1 ) & ~( __neg_2 ) & ~( __neg_3 ) & ~( __neg_4 ) & ~( __neg_5 ).
		q(nt(21), (nt(7)) & ~(nt(214)) & ~(nt(215)) & ~(nt(216)) & ~(nt(217)) & ~(nt(218)) & ~(nt(219)));
		// char0 => space.
		q(nt(25), (nt(2)));
		// char0 => alnum.
		q(nt(25), (nt(6)));
		// char0 => char_escape_encode.
		q(nt(25), (nt(16)));
		// char0 => char_punct.
		q(nt(25), (nt(21)));
		// char_ => esc q_char.
		q(nt(26), (nt(17)+nt(18)));
		// char_ => q_str.
		q(nt(26), (nt(19)));
		// char_ => q_bqstr.
		q(nt(26), (nt(20)));
		// char_ => char0.
		q(nt(26), (nt(25)));
		// string_char => esc q_str.
		q(nt(27), (nt(17)+nt(19)));
		// string_char => q_char.
		q(nt(27), (nt(18)));
		// string_char => q_bqstr.
		q(nt(27), (nt(20)));
		// string_char => char0.
		q(nt(27), (nt(25)));
		// bqstring_char => esc q_bqstr.
		q(nt(28), (nt(17)+nt(20)));
		// bqstring_char => q_char.
		q(nt(28), (nt(18)));
		// bqstring_char => q_str.
		q(nt(28), (nt(19)));
		// bqstring_char => char0.
		q(nt(28), (nt(25)));
		// chars_4 => alnum.
		q(nt(30), (nt(6)));
		// chars_5 => chars_4 chars_5.
		q(nt(31), (nt(30)+nt(31)));
		// chars_5 => ε.
		q(nt(31), (nul));
		// chars => alpha chars_5.
		q(nt(29), (nt(5)+nt(31)));
		// char_class => 'a' 'l' 'n' 'u' 'm'.
		q(nt(32), (t(10)+t(11)+t(12)+t(6)+t(13)));
		// char_class => 'a' 'l' 'p' 'h' 'a'.
		q(nt(32), (t(10)+t(11)+t(14)+t(15)+t(10)));
		// char_class => 'b' 'l' 'a' 'n' 'k'.
		q(nt(32), (t(16)+t(11)+t(10)+t(12)+t(17)));
		// char_class => 'c' 'n' 't' 'r' 'l'.
		q(nt(32), (t(18)+t(12)+t(19)+t(20)+t(11)));
		// char_class => 'd' 'i' 'g' 'i' 't'.
		q(nt(32), (t(21)+t(22)+t(23)+t(22)+t(19)));
		// char_class => 'e' 'o' 'f'.
		q(nt(32), (t(24)+t(25)+t(26)));
		// char_class => 'g' 'r' 'a' 'p' 'h'.
		q(nt(32), (t(23)+t(20)+t(10)+t(14)+t(15)));
		// char_class => 'l' 'o' 'w' 'e' 'r'.
		q(nt(32), (t(11)+t(25)+t(27)+t(24)+t(20)));
		// char_class => 'p' 'r' 'i' 'n' 't' 'a' 'b' 'l' 'e'.
		q(nt(32), (t(14)+t(20)+t(22)+t(12)+t(19)+t(10)+t(16)+t(11)+t(24)));
		// char_class => 'p' 'u' 'n' 'c' 't'.
		q(nt(32), (t(14)+t(6)+t(12)+t(18)+t(19)));
		// char_class => 's' 'p' 'a' 'c' 'e'.
		q(nt(32), (t(28)+t(14)+t(10)+t(18)+t(24)));
		// char_class => 'u' 'p' 'p' 'e' 'r'.
		q(nt(32), (t(6)+t(14)+t(14)+t(24)+t(20)));
		// char_class => 'x' 'd' 'i' 'g' 'i' 't'.
		q(nt(32), (t(5)+t(21)+t(22)+t(23)+t(22)+t(19)));
		// sym => chars.
		q(nt(33), (nt(29)));
		// definition => ws ':' '=' ws.
		q(nt(34), (nt(13)+t(29)+t(30)+nt(13)));
		// equality => ws '=' '=' ws.
		q(nt(35), (nt(13)+t(30)+t(30)+nt(13)));
		// nequality => ws '!' '=' ws.
		q(nt(36), (nt(13)+t(31)+t(30)+nt(13)));
		// dot => ws '.' ws.
		q(nt(37), (nt(13)+t(32)+nt(13)));
		// open_parenthesis => ws '(' ws.
		q(nt(38), (nt(13)+t(33)+nt(13)));
		// close_parenthesis => ws ')' ws.
		q(nt(39), (nt(13)+t(34)+nt(13)));
		// open_bracket => ws '[' ws.
		q(nt(40), (nt(13)+t(35)+nt(13)));
		// close_bracket => ws ']' ws.
		q(nt(41), (nt(13)+t(36)+nt(13)));
		// open_brace => ws '{' ws.
		q(nt(42), (nt(13)+t(37)+nt(13)));
		// close_brace => ws '}' ws.
		q(nt(43), (nt(13)+t(38)+nt(13)));
		// minus => ws '-' ws.
		q(nt(44), (nt(13)+t(39)+nt(13)));
		// colon => ws ':' ws.
		q(nt(45), (nt(13)+t(29)+nt(13)));
		// less => ws '<' ws.
		q(nt(46), (nt(13)+t(40)+nt(13)));
		// indexes_6 => index.
		q(nt(49), (nt(48)));
		// indexes_7 => indexes_6 indexes_7.
		q(nt(50), (nt(49)+nt(50)));
		// indexes_7 => ε.
		q(nt(50), (nul));
		// indexes => open_bracket indexes_7 close_bracket.
		q(nt(47), (nt(40)+nt(50)+nt(41)));
		// index => variable.
		q(nt(48), (nt(51)));
		// index => capture.
		q(nt(48), (nt(52)));
		// index => ignore.
		q(nt(48), (nt(53)));
		// index => offset.
		q(nt(48), (nt(54)));
		// num_8 => digit.
		q(nt(56), (nt(3)));
		// num_8 => digit num_8.
		q(nt(56), (nt(3)+nt(56)));
		// num => num_8.
		q(nt(55), (nt(56)));
		// captures_9 => ws_required capture.
		q(nt(58), (nt(12)+nt(52)));
		// captures_10 => captures_9 captures_10.
		q(nt(59), (nt(58)+nt(59)));
		// captures_10 => ε.
		q(nt(59), (nul));
		// captures => open_parenthesis capture open_parenthesis close_parenthesis.
		q(nt(57), (nt(38)+nt(52)+nt(38)+nt(39)));
		// captures => open_parenthesis capture captures_10 close_parenthesis.
		q(nt(57), (nt(38)+nt(52)+nt(59)+nt(39)));
		// variable => var.
		q(nt(51), (nt(60)));
		// variable => timed.
		q(nt(51), (nt(61)));
		// timed_11 => in.
		q(nt(64), (nt(62)));
		// timed_11 => out.
		q(nt(64), (nt(63)));
		// timed => timed_11 indexes.
		q(nt(61), (nt(64)+nt(47)));
		// offset => variable.
		q(nt(54), (nt(51)));
		// offset => variable minus num.
		q(nt(54), (nt(51)+nt(44)+nt(55)));
		// ignore => '_'.
		q(nt(53), (t(41)));
		// capture => '$' chars.
		q(nt(52), (t(42)+nt(29)));
		// var => '?' chars.
		q(nt(60), (t(43)+nt(29)));
		// in => '?' 'i' '_' chars.
		q(nt(62), (t(43)+t(22)+t(41)+nt(29)));
		// out => '?' 'o' '_' chars.
		q(nt(63), (t(43)+t(25)+t(41)+nt(29)));
		// wff_rule => wff_matcher definition wff_body dot.
		q(nt(65), (nt(66)+nt(34)+nt(67)+nt(37)));
		// wff_matcher => wff.
		q(nt(66), (nt(68)));
		// wff_matcher => wff_ref.
		q(nt(66), (nt(69)));
		// wff_body => wff.
		q(nt(67), (nt(68)));
		// wff_body => bf_eq_cb.
		q(nt(67), (nt(70)));
		// wff_body => bf_neq_cb.
		q(nt(67), (nt(71)));
		// wff_body => wff_has_clashing_subformulas_cb.
		q(nt(67), (nt(72)));
		// wff_body => wff_has_subformula_cb.
		q(nt(67), (nt(73)));
		// wff_body => wff_remove_existential.
		q(nt(67), (nt(74)));
		// wff_ref => sym indexes wff_ref_args.
		q(nt(69), (nt(33)+nt(47)+nt(75)));
		// wff_ref_args_12 => variable.
		q(nt(76), (nt(51)));
		// wff_ref_args_13 => wff_ref_args_12 wff_ref_args_13.
		q(nt(77), (nt(76)+nt(77)));
		// wff_ref_args_13 => ε.
		q(nt(77), (nul));
		// wff_ref_args => open_parenthesis wff_ref_args_13 close_parenthesis.
		q(nt(75), (nt(38)+nt(77)+nt(39)));
		// wff => variable.
		q(nt(68), (nt(51)));
		// wff => capture.
		q(nt(68), (nt(52)));
		// wff => ignore.
		q(nt(68), (nt(53)));
		// wff => wff_ref.
		q(nt(68), (nt(69)));
		// wff => wff_eq.
		q(nt(68), (nt(78)));
		// wff => wff_neq.
		q(nt(68), (nt(79)));
		// wff => wff_and.
		q(nt(68), (nt(80)));
		// wff => wff_neg.
		q(nt(68), (nt(81)));
		// wff => wff_xor.
		q(nt(68), (nt(82)));
		// wff => cbf_if.
		q(nt(68), (nt(83)));
		// wff => wff_or.
		q(nt(68), (nt(84)));
		// wff => wff_all.
		q(nt(68), (nt(85)));
		// wff => wff_ex.
		q(nt(68), (nt(86)));
		// wff => wff_imply.
		q(nt(68), (nt(87)));
		// wff => wff_equiv.
		q(nt(68), (nt(88)));
		// wff => cbf_and_wff.
		q(nt(68), (nt(89)));
		// wff => wff_coimply.
		q(nt(68), (nt(90)));
		// wff => wff_t.
		q(nt(68), (nt(91)));
		// wff => wff_f.
		q(nt(68), (nt(92)));
		// wff_eq => open_parenthesis cbf equality wff_f close_parenthesis.
		q(nt(78), (nt(38)+nt(93)+nt(35)+nt(92)+nt(39)));
		// wff_neq => open_parenthesis cbf nequality wff_f close_parenthesis.
		q(nt(79), (nt(38)+nt(93)+nt(36)+nt(92)+nt(39)));
		// wff_and => open_parenthesis wff wff_and_sym wff close_parenthesis.
		q(nt(80), (nt(38)+nt(68)+nt(94)+nt(68)+nt(39)));
		// wff_or => open_parenthesis wff wff_or_sym wff close_parenthesis.
		q(nt(84), (nt(38)+nt(68)+nt(95)+nt(68)+nt(39)));
		// wff_xor => open_parenthesis wff wff_xor_sym wff close_parenthesis.
		q(nt(82), (nt(38)+nt(68)+nt(96)+nt(68)+nt(39)));
		// wff_neg => wff_neg_sym wff.
		q(nt(81), (nt(97)+nt(68)));
		// wff_imply => open_parenthesis wff wff_imply_sym wff close_parenthesis.
		q(nt(87), (nt(38)+nt(68)+nt(98)+nt(68)+nt(39)));
		// wff_equiv => open_parenthesis wff wff_equiv_sym wff close_parenthesis.
		q(nt(88), (nt(38)+nt(68)+nt(99)+nt(68)+nt(39)));
		// wff_coimply => open_parenthesis wff wff_coimply_sym wff close_parenthesis.
		q(nt(90), (nt(38)+nt(68)+nt(100)+nt(68)+nt(39)));
		// wff_all_14 => variable.
		q(nt(102), (nt(51)));
		// wff_all_14 => capture.
		q(nt(102), (nt(52)));
		// wff_all_14 => ignore.
		q(nt(102), (nt(53)));
		// wff_all => wff_all_sym wff_all_14 ws_required wff.
		q(nt(85), (nt(101)+nt(102)+nt(12)+nt(68)));
		// wff_ex_15 => variable.
		q(nt(104), (nt(51)));
		// wff_ex_15 => capture.
		q(nt(104), (nt(52)));
		// wff_ex_15 => ignore.
		q(nt(104), (nt(53)));
		// wff_ex => wff_ex_sym wff_ex_15 ws_required wff.
		q(nt(86), (nt(103)+nt(104)+nt(12)+nt(68)));
		// cbf_and_wff => open_parenthesis cbf cbf_and_wff_sym wff close_parenthesis.
		q(nt(89), (nt(38)+nt(93)+nt(105)+nt(68)+nt(39)));
		// cbf_if => open_parenthesis cbf_if_sym cbf_if_condition cbf_then_sym cbf_if_then cbf_else_sym cbf_if_else close_parenthesis.
		q(nt(83), (nt(38)+nt(106)+nt(107)+nt(108)+nt(109)+nt(110)+nt(111)+nt(39)));
		// cbf_if_condition => wff.
		q(nt(107), (nt(68)));
		// cbf_if_then => cbf.
		q(nt(109), (nt(93)));
		// cbf_if_else => cbf.
		q(nt(111), (nt(93)));
		// wff_and_sym => ws 'w' 'f' 'f' '_' 'a' 'n' 'd' ws.
		q(nt(94), (nt(13)+t(27)+t(26)+t(26)+t(41)+t(10)+t(12)+t(21)+nt(13)));
		// wff_or_sym => ws 'w' 'f' 'f' '_' 'o' 'r' ws.
		q(nt(95), (nt(13)+t(27)+t(26)+t(26)+t(41)+t(25)+t(20)+nt(13)));
		// wff_xor_sym => ws 'w' 'f' 'f' '_' 'x' 'o' 'r' ws.
		q(nt(96), (nt(13)+t(27)+t(26)+t(26)+t(41)+t(5)+t(25)+t(20)+nt(13)));
		// wff_neg_sym => ws 'w' 'f' 'f' '_' 'n' 'e' 'g' ws.
		q(nt(97), (nt(13)+t(27)+t(26)+t(26)+t(41)+t(12)+t(24)+t(23)+nt(13)));
		// wff_imply_sym => ws 'w' 'f' 'f' '_' 'i' 'm' 'p' 'l' 'y' ws.
		q(nt(98), (nt(13)+t(27)+t(26)+t(26)+t(41)+t(22)+t(13)+t(14)+t(11)+t(44)+nt(13)));
		// wff_equiv_sym => ws 'w' 'f' 'f' '_' 'e' 'q' 'u' 'i' 'v' ws.
		q(nt(99), (nt(13)+t(27)+t(26)+t(26)+t(41)+t(24)+t(45)+t(6)+t(22)+t(46)+nt(13)));
		// wff_coimply_sym => ws 'w' 'f' 'f' '_' 'c' 'o' 'i' 'm' 'p' 'l' 'y' ws.
		q(nt(100), (nt(13)+t(27)+t(26)+t(26)+t(41)+t(18)+t(25)+t(22)+t(13)+t(14)+t(11)+t(44)+nt(13)));
		// wff_all_sym => ws 'w' 'f' 'f' '_' 'a' 'l' 'l' ws.
		q(nt(101), (nt(13)+t(27)+t(26)+t(26)+t(41)+t(10)+t(11)+t(11)+nt(13)));
		// wff_ex_sym => ws 'w' 'f' 'f' '_' 'e' 'x' ws.
		q(nt(103), (nt(13)+t(27)+t(26)+t(26)+t(41)+t(24)+t(5)+nt(13)));
		// wff_t => ws 'T' ws.
		q(nt(91), (nt(13)+t(47)+nt(13)));
		// wff_f => ws 'F' ws.
		q(nt(92), (nt(13)+t(48)+nt(13)));
		// cbf_and_wff_sym => ws 'c' 'b' 'f' '_' 'a' 'n' 'd' '_' 'w' 'f' 'f' ws.
		q(nt(105), (nt(13)+t(18)+t(16)+t(26)+t(41)+t(10)+t(12)+t(21)+t(41)+t(27)+t(26)+t(26)+nt(13)));
		// cbf_rule => cbf_matcher definition cbf_body dot.
		q(nt(112), (nt(113)+nt(34)+nt(114)+nt(37)));
		// cbf_matcher => cbf.
		q(nt(113), (nt(93)));
		// cbf_matcher => cbf_ref.
		q(nt(113), (nt(115)));
		// cbf_body => cbf.
		q(nt(114), (nt(93)));
		// cbf_ref => sym indexes cbf_ref_args.
		q(nt(115), (nt(33)+nt(47)+nt(116)));
		// cbf_ref_args_16 => variable.
		q(nt(117), (nt(51)));
		// cbf_ref_args_17 => cbf_ref_args_16 cbf_ref_args_17.
		q(nt(118), (nt(117)+nt(118)));
		// cbf_ref_args_17 => ε.
		q(nt(118), (nul));
		// cbf_ref_args => open_parenthesis cbf_ref_args_17 close_parenthesis.
		q(nt(116), (nt(38)+nt(118)+nt(39)));
		// cbf => cbf_ref.
		q(nt(93), (nt(115)));
		// cbf => bf.
		q(nt(93), (nt(119)));
		// cbf => cbf_and.
		q(nt(93), (nt(120)));
		// cbf => cbf_neg.
		q(nt(93), (nt(121)));
		// cbf => cbf_xor.
		q(nt(93), (nt(122)));
		// cbf => cbf_or.
		q(nt(93), (nt(123)));
		// cbf => cbf_imply.
		q(nt(93), (nt(124)));
		// cbf => cbf_equiv.
		q(nt(93), (nt(125)));
		// cbf => cbf_coimply.
		q(nt(93), (nt(126)));
		// cbf_and => open_parenthesis cbf cbf_and_sym cbf close_parenthesis.
		q(nt(120), (nt(38)+nt(93)+nt(127)+nt(93)+nt(39)));
		// cbf_or => open_parenthesis cbf cbf_or_sym cbf close_parenthesis.
		q(nt(123), (nt(38)+nt(93)+nt(128)+nt(93)+nt(39)));
		// cbf_xor => open_parenthesis cbf cbf_xor_sym cbf close_parenthesis.
		q(nt(122), (nt(38)+nt(93)+nt(129)+nt(93)+nt(39)));
		// cbf_neg => cbf_neg_sym cbf.
		q(nt(121), (nt(130)+nt(93)));
		// cbf_imply => open_parenthesis cbf cbf_imply_sym cbf close_parenthesis.
		q(nt(124), (nt(38)+nt(93)+nt(131)+nt(93)+nt(39)));
		// cbf_equiv => open_parenthesis cbf cbf_equiv_sym cbf close_parenthesis.
		q(nt(125), (nt(38)+nt(93)+nt(132)+nt(93)+nt(39)));
		// cbf_coimply => open_parenthesis cbf cbf_coimply_sym cbf close_parenthesis.
		q(nt(126), (nt(38)+nt(93)+nt(133)+nt(93)+nt(39)));
		// cbf_and_sym => ws 'c' 'b' 'f' '_' 'a' 'n' 'd' ws.
		q(nt(127), (nt(13)+t(18)+t(16)+t(26)+t(41)+t(10)+t(12)+t(21)+nt(13)));
		// cbf_or_sym => ws 'c' 'b' 'f' '_' 'o' 'r' ws.
		q(nt(128), (nt(13)+t(18)+t(16)+t(26)+t(41)+t(25)+t(20)+nt(13)));
		// cbf_xor_sym => ws 'c' 'b' 'f' '_' 'x' 'o' 'r' ws.
		q(nt(129), (nt(13)+t(18)+t(16)+t(26)+t(41)+t(5)+t(25)+t(20)+nt(13)));
		// cbf_neg_sym => ws 'c' 'b' 'f' '_' 'n' 'e' 'g' ws.
		q(nt(130), (nt(13)+t(18)+t(16)+t(26)+t(41)+t(12)+t(24)+t(23)+nt(13)));
		// cbf_imply_sym => ws 'c' 'b' 'f' '_' 'i' 'm' 'p' 'l' 'y' ws.
		q(nt(131), (nt(13)+t(18)+t(16)+t(26)+t(41)+t(22)+t(13)+t(14)+t(11)+t(44)+nt(13)));
		// cbf_equiv_sym => ws 'c' 'b' 'f' '_' 'e' 'q' 'u' 'i' 'v' ws.
		q(nt(132), (nt(13)+t(18)+t(16)+t(26)+t(41)+t(24)+t(45)+t(6)+t(22)+t(46)+nt(13)));
		// cbf_coimply_sym => ws 'c' 'b' 'f' '_' 'c' 'o' 'i' 'm' 'p' 'l' 'y' ws.
		q(nt(133), (nt(13)+t(18)+t(16)+t(26)+t(41)+t(18)+t(25)+t(22)+t(13)+t(14)+t(11)+t(44)+nt(13)));
		// cbf_if_sym => ws 'i' 'f' ws.
		q(nt(106), (nt(13)+t(22)+t(26)+nt(13)));
		// cbf_then_sym => ws 't' 'h' 'e' 'n' ws.
		q(nt(108), (nt(13)+t(19)+t(15)+t(24)+t(12)+nt(13)));
		// cbf_else_sym => ws 'e' 'l' 's' 'e' ws.
		q(nt(110), (nt(13)+t(24)+t(11)+t(28)+t(24)+nt(13)));
		// bf_rule => bf_matcher definition bf_body dot.
		q(nt(134), (nt(135)+nt(34)+nt(136)+nt(37)));
		// bf_matcher => bf.
		q(nt(135), (nt(119)));
		// bf_body => bf.
		q(nt(136), (nt(119)));
		// bf_body => bf_is_zero_cb.
		q(nt(136), (nt(137)));
		// bf_body => bf_is_one_cb.
		q(nt(136), (nt(138)));
		// bf_body => bf_has_clashing_subformulas_cb.
		q(nt(136), (nt(139)));
		// bf_body => bf_has_subformula_cb.
		q(nt(136), (nt(140)));
		// bf => variable.
		q(nt(119), (nt(51)));
		// bf => capture.
		q(nt(119), (nt(52)));
		// bf => ignore.
		q(nt(119), (nt(53)));
		// bf => bf_constant.
		q(nt(119), (nt(141)));
		// bf => bf_and.
		q(nt(119), (nt(142)));
		// bf => bf_neg.
		q(nt(119), (nt(143)));
		// bf => bf_xor.
		q(nt(119), (nt(144)));
		// bf => bf_or.
		q(nt(119), (nt(145)));
		// bf => bf_all.
		q(nt(119), (nt(146)));
		// bf => bf_ex.
		q(nt(119), (nt(147)));
		// bf => bf_less.
		q(nt(119), (nt(148)));
		// bf => bf_less_equal.
		q(nt(119), (nt(149)));
		// bf => bf_greater.
		q(nt(119), (nt(150)));
		// bf => bf_less_cb.
		q(nt(119), (nt(151)));
		// bf => bf_less_equal_cb.
		q(nt(119), (nt(152)));
		// bf => bf_greater_cb.
		q(nt(119), (nt(153)));
		// bf => bf_subs_cb.
		q(nt(119), (nt(154)));
		// bf => bf_t.
		q(nt(119), (nt(155)));
		// bf => bf_f.
		q(nt(119), (nt(156)));
		// bf_and => open_parenthesis bf bf_and_sym bf close_parenthesis.
		q(nt(142), (nt(38)+nt(119)+nt(157)+nt(119)+nt(39)));
		// bf_or => open_parenthesis bf bf_or_sym bf close_parenthesis.
		q(nt(145), (nt(38)+nt(119)+nt(158)+nt(119)+nt(39)));
		// bf_xor => open_parenthesis bf bf_xor_sym ws bf close_parenthesis.
		q(nt(144), (nt(38)+nt(119)+nt(159)+nt(13)+nt(119)+nt(39)));
		// bf_neg => bf_neg_sym bf.
		q(nt(143), (nt(160)+nt(119)));
		// bf_less => open_parenthesis bf bf_less_sym bf close_parenthesis.
		q(nt(148), (nt(38)+nt(119)+nt(161)+nt(119)+nt(39)));
		// bf_less_equal => open_parenthesis bf bf_less_equal_sym bf close_parenthesis.
		q(nt(149), (nt(38)+nt(119)+nt(162)+nt(119)+nt(39)));
		// bf_greater => open_parenthesis bf bf_greater_sym bf close_parenthesis.
		q(nt(150), (nt(38)+nt(119)+nt(163)+nt(119)+nt(39)));
		// bf_all_18 => variable.
		q(nt(165), (nt(51)));
		// bf_all_18 => capture.
		q(nt(165), (nt(52)));
		// bf_all => bf_all_sym ws_required bf_all_18 ws_required bf.
		q(nt(146), (nt(164)+nt(12)+nt(165)+nt(12)+nt(119)));
		// bf_ex_19 => variable.
		q(nt(167), (nt(51)));
		// bf_ex_19 => capture.
		q(nt(167), (nt(52)));
		// bf_ex => bf_ex_sym ws_required bf_ex_19 ws_required bf.
		q(nt(147), (nt(166)+nt(12)+nt(167)+nt(12)+nt(119)));
		// bf_and_sym => ws 'b' 'f' '_' 'a' 'n' 'd' ws.
		q(nt(157), (nt(13)+t(16)+t(26)+t(41)+t(10)+t(12)+t(21)+nt(13)));
		// bf_or_sym => ws 'b' 'f' '_' 'o' 'r' ws.
		q(nt(158), (nt(13)+t(16)+t(26)+t(41)+t(25)+t(20)+nt(13)));
		// bf_xor_sym => ws 'b' 'f' '_' 'x' 'o' 'r' ws.
		q(nt(159), (nt(13)+t(16)+t(26)+t(41)+t(5)+t(25)+t(20)+nt(13)));
		// bf_neg_sym => ws 'b' 'f' '_' 'n' 'e' 'g' ws.
		q(nt(160), (nt(13)+t(16)+t(26)+t(41)+t(12)+t(24)+t(23)+nt(13)));
		// bf_less_sym => ws 'b' 'f' '_' 'l' 'e' 's' 's' ws.
		q(nt(161), (nt(13)+t(16)+t(26)+t(41)+t(11)+t(24)+t(28)+t(28)+nt(13)));
		// bf_less_equal_sym => ws 'b' 'f' '_' 'l' 'e' 's' 's' '_' 'e' 'q' 'u' 'a' 'l' ws.
		q(nt(162), (nt(13)+t(16)+t(26)+t(41)+t(11)+t(24)+t(28)+t(28)+t(41)+t(24)+t(45)+t(6)+t(10)+t(11)+nt(13)));
		// bf_greater_sym => ws 'b' 'f' '_' 'g' 'r' 'e' 'a' 't' 'e' 'r' ws.
		q(nt(163), (nt(13)+t(16)+t(26)+t(41)+t(23)+t(20)+t(24)+t(10)+t(19)+t(24)+t(20)+nt(13)));
		// bf_all_sym => ws 'b' 'f' '_' 'a' 'l' 'l' ws.
		q(nt(164), (nt(13)+t(16)+t(26)+t(41)+t(10)+t(11)+t(11)+nt(13)));
		// bf_ex_sym => ws 'b' 'f' '_' 'e' 'x' ws.
		q(nt(166), (nt(13)+t(16)+t(26)+t(41)+t(24)+t(5)+nt(13)));
		// bf_t => ws 'T' ws.
		q(nt(155), (nt(13)+t(47)+nt(13)));
		// bf_f => ws 'F' ws.
		q(nt(156), (nt(13)+t(48)+nt(13)));
		// bf_constant => open_brace constant close_brace.
		q(nt(141), (nt(42)+nt(168)+nt(43)));
		// constant => capture.
		q(nt(168), (nt(52)));
		// constant => ignore.
		q(nt(168), (nt(53)));
		// constant => binding.
		q(nt(168), (nt(169)));
		// constant => bf_and_cb.
		q(nt(168), (nt(170)));
		// constant => bf_or_cb.
		q(nt(168), (nt(171)));
		// constant => bf_xor_cb.
		q(nt(168), (nt(172)));
		// constant => bf_neg_cb.
		q(nt(168), (nt(173)));
		// binding => source_binding.
		q(nt(169), (nt(174)));
		// binding => named_binding.
		q(nt(169), (nt(175)));
		// named_binding => chars.
		q(nt(175), (nt(29)));
		// source_binding => type colon source.
		q(nt(174), (nt(176)+nt(45)+nt(177)));
		// type => chars.
		q(nt(176), (nt(29)));
		// type => ε.
		q(nt(176), (nul));
		// source0 => space.
		q(nt(178), (nt(2)));
		// source0 => alnum.
		q(nt(178), (nt(6)));
		// source0 => char_escape_encode.
		q(nt(178), (nt(16)));
		// source0 => char_punct.
		q(nt(178), (nt(21)));
		// source_20 => source0.
		q(nt(179), (nt(178)));
		// source_21 => source_20.
		q(nt(180), (nt(179)));
		// source_21 => source_20 source_21.
		q(nt(180), (nt(179)+nt(180)));
		// source => source_21.
		q(nt(177), (nt(180)));
		// bf_and_cb => bf_cb_arg bf_and_cb_sym bf_cb_arg.
		q(nt(170), (nt(181)+nt(182)+nt(181)));
		// bf_or_cb => bf_cb_arg bf_or_cb_sym bf_cb_arg.
		q(nt(171), (nt(181)+nt(183)+nt(181)));
		// bf_xor_cb => bf_cb_arg bf_xor_cb_sym bf_cb_arg.
		q(nt(172), (nt(181)+nt(184)+nt(181)));
		// bf_neg_cb => bf_neg_cb_sym bf_cb_arg.
		q(nt(173), (nt(185)+nt(181)));
		// bf_less_cb => bf_less_cb_sym bf_cb_arg ws_required bf_cb_arg ws_required bf_cb_arg ws_required bf_cb_arg.
		q(nt(151), (nt(186)+nt(181)+nt(12)+nt(181)+nt(12)+nt(181)+nt(12)+nt(181)));
		// bf_less_equal_cb => bf_less_equal_cb_sym bf_cb_arg ws_required bf_cb_arg ws_required bf_cb_arg ws_required bf_cb_arg.
		q(nt(152), (nt(187)+nt(181)+nt(12)+nt(181)+nt(12)+nt(181)+nt(12)+nt(181)));
		// bf_greater_cb => bf_greater_cb_sym bf_cb_arg ws_required bf_cb_arg ws_required bf_cb_arg ws_required bf_cb_arg.
		q(nt(153), (nt(188)+nt(181)+nt(12)+nt(181)+nt(12)+nt(181)+nt(12)+nt(181)));
		// bf_subs_cb => bf_subs_cb_sym bf_cb_arg ws_required bf_cb_arg ws_required bf_cb_arg.
		q(nt(154), (nt(189)+nt(181)+nt(12)+nt(181)+nt(12)+nt(181)));
		// bf_eq_cb => bf_eq_cb_sym bf_cb_arg ws_required wff_cb_arg ws_required wff_cb_arg.
		q(nt(70), (nt(190)+nt(181)+nt(12)+nt(191)+nt(12)+nt(191)));
		// bf_neq_cb => bf_neq_cb_sym bf_cb_arg ws_required wff_cb_arg ws_required wff_cb_arg.
		q(nt(71), (nt(192)+nt(181)+nt(12)+nt(191)+nt(12)+nt(191)));
		// bf_is_zero_cb => bf_is_zero_cb_sym bf_cb_arg ws_required bf_cb_arg.
		q(nt(137), (nt(193)+nt(181)+nt(12)+nt(181)));
		// bf_is_one_cb => bf_is_one_cb_sym bf_cb_arg ws_required bf_cb_arg.
		q(nt(138), (nt(194)+nt(181)+nt(12)+nt(181)));
		// bf_has_clashing_subformulas_cb => bf_has_clashing_subformulas_cb_sym bf_cb_arg ws_required bf_cb_arg.
		q(nt(139), (nt(195)+nt(181)+nt(12)+nt(181)));
		// wff_has_clashing_subformulas_cb => wff_has_clashing_subformulas_cb_sym wff_cb_arg ws_required wff_cb_arg.
		q(nt(72), (nt(196)+nt(191)+nt(12)+nt(191)));
		// bf_has_subformula_cb => bf_has_subformula_cb_sym bf_cb_arg ws_required bf_cb_arg ws_required bf_cb_arg.
		q(nt(140), (nt(197)+nt(181)+nt(12)+nt(181)+nt(12)+nt(181)));
		// wff_has_subformula_cb => wff_has_subformula_cb_sym wff_cb_arg ws_required wff_cb_arg ws_required wff_cb_arg.
		q(nt(73), (nt(198)+nt(191)+nt(12)+nt(191)+nt(12)+nt(191)));
		// wff_remove_existential_22 => variable.
		q(nt(200), (nt(51)));
		// wff_remove_existential_22 => capture.
		q(nt(200), (nt(52)));
		// wff_remove_existential => wff_remove_existential_sym wff_remove_existential_22 ws_required wff.
		q(nt(74), (nt(199)+nt(200)+nt(12)+nt(68)));
		// bf_cb_arg => capture.
		q(nt(181), (nt(52)));
		// bf_cb_arg => ignore.
		q(nt(181), (nt(53)));
		// bf_cb_arg => bf.
		q(nt(181), (nt(119)));
		// wff_cb_arg => capture.
		q(nt(191), (nt(52)));
		// wff_cb_arg => ignore.
		q(nt(191), (nt(53)));
		// wff_cb_arg => wff.
		q(nt(191), (nt(68)));
		// bf_and_cb_sym => ws 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b' ws.
		q(nt(182), (nt(13)+t(16)+t(26)+t(41)+t(10)+t(12)+t(21)+t(41)+t(18)+t(16)+nt(13)));
		// bf_or_cb_sym => ws 'b' 'f' '_' 'o' 'r' '_' 'c' 'b' ws.
		q(nt(183), (nt(13)+t(16)+t(26)+t(41)+t(25)+t(20)+t(41)+t(18)+t(16)+nt(13)));
		// bf_xor_cb_sym => ws 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b' ws.
		q(nt(184), (nt(13)+t(16)+t(26)+t(41)+t(5)+t(25)+t(20)+t(41)+t(18)+t(16)+nt(13)));
		// bf_neg_cb_sym => ws 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b' ws.
		q(nt(185), (nt(13)+t(16)+t(26)+t(41)+t(12)+t(24)+t(23)+t(41)+t(18)+t(16)+nt(13)));
		// bf_less_cb_sym => ws 'b' 'f' '_' 'l' 'e' 's' 's' '_' 'c' 'b' ws.
		q(nt(186), (nt(13)+t(16)+t(26)+t(41)+t(11)+t(24)+t(28)+t(28)+t(41)+t(18)+t(16)+nt(13)));
		// bf_less_equal_cb_sym => ws 'b' 'f' '_' 'l' 'e' 's' 's' '_' 'e' 'q' 'u' 'a' 'l' '_' 'c' 'b' ws.
		q(nt(187), (nt(13)+t(16)+t(26)+t(41)+t(11)+t(24)+t(28)+t(28)+t(41)+t(24)+t(45)+t(6)+t(10)+t(11)+t(41)+t(18)+t(16)+nt(13)));
		// bf_greater_cb_sym => ws 'b' 'f' '_' 'g' 'r' 'e' 'a' 't' 'e' 'r' '_' 'c' 'b' ws.
		q(nt(188), (nt(13)+t(16)+t(26)+t(41)+t(23)+t(20)+t(24)+t(10)+t(19)+t(24)+t(20)+t(41)+t(18)+t(16)+nt(13)));
		// bf_subs_cb_sym => ws 'b' 'f' '_' 's' 'u' 'b' 's' '_' 'c' 'b' ws.
		q(nt(189), (nt(13)+t(16)+t(26)+t(41)+t(28)+t(6)+t(16)+t(28)+t(41)+t(18)+t(16)+nt(13)));
		// bf_eq_cb_sym => ws 'b' 'f' '_' 'e' 'q' '_' 'c' 'b' ws.
		q(nt(190), (nt(13)+t(16)+t(26)+t(41)+t(24)+t(45)+t(41)+t(18)+t(16)+nt(13)));
		// bf_neq_cb_sym => ws 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b' ws.
		q(nt(192), (nt(13)+t(16)+t(26)+t(41)+t(12)+t(24)+t(45)+t(41)+t(18)+t(16)+nt(13)));
		// bf_is_zero_cb_sym => ws 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b' ws.
		q(nt(193), (nt(13)+t(16)+t(26)+t(41)+t(22)+t(28)+t(41)+t(49)+t(24)+t(20)+t(25)+t(41)+t(18)+t(16)+nt(13)));
		// bf_is_one_cb_sym => ws 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b' ws.
		q(nt(194), (nt(13)+t(16)+t(26)+t(41)+t(22)+t(28)+t(41)+t(25)+t(12)+t(24)+t(41)+t(18)+t(16)+nt(13)));
		// bf_has_clashing_subformulas_cb_sym => ws 'b' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' ws.
		q(nt(195), (nt(13)+t(16)+t(26)+t(41)+t(15)+t(10)+t(28)+t(41)+t(18)+t(11)+t(10)+t(28)+t(15)+t(22)+t(12)+t(23)+t(41)+t(28)+t(6)+t(16)+t(26)+t(25)+t(20)+t(13)+t(6)+t(11)+t(10)+t(28)+t(41)+t(18)+t(16)+nt(13)));
		// bf_has_subformula_cb_sym => ws 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' ws.
		q(nt(197), (nt(13)+t(16)+t(26)+t(41)+t(15)+t(10)+t(28)+t(41)+t(28)+t(6)+t(16)+t(26)+t(25)+t(20)+t(13)+t(6)+t(11)+t(10)+t(41)+t(18)+t(16)+nt(13)));
		// wff_has_clashing_subformulas_cb_sym => ws 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' ws.
		q(nt(196), (nt(13)+t(27)+t(26)+t(26)+t(41)+t(15)+t(10)+t(28)+t(41)+t(18)+t(11)+t(10)+t(28)+t(15)+t(22)+t(12)+t(23)+t(41)+t(28)+t(6)+t(16)+t(26)+t(25)+t(20)+t(13)+t(6)+t(11)+t(10)+t(28)+t(41)+t(18)+t(16)+nt(13)));
		// wff_has_subformula_cb_sym => ws 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' ws.
		q(nt(198), (nt(13)+t(27)+t(26)+t(26)+t(41)+t(15)+t(10)+t(28)+t(41)+t(28)+t(6)+t(16)+t(26)+t(25)+t(20)+t(13)+t(6)+t(11)+t(10)+t(41)+t(18)+t(16)+nt(13)));
		// wff_remove_existential_sym => ws 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' ws.
		q(nt(199), (nt(13)+t(27)+t(26)+t(26)+t(41)+t(20)+t(24)+t(13)+t(25)+t(46)+t(24)+t(41)+t(24)+t(5)+t(22)+t(28)+t(19)+t(24)+t(12)+t(19)+t(22)+t(10)+t(11)+nt(13)));
		// input => in colon open_brace source_binding close_brace.
		q(nt(201), (nt(62)+nt(45)+nt(42)+nt(174)+nt(43)));
		// inputs_23 => less input input inputs_23.
		q(nt(203), (nt(46)+nt(201)+nt(201)+nt(203)));
		// inputs_23 => ε.
		q(nt(203), (nul));
		// inputs => inputs_23 dot.
		q(nt(202), (nt(203)+nt(37)));
		// main => wff dot.
		q(nt(204), (nt(68)+nt(37)));
		// rule => wff_rule.
		q(nt(205), (nt(65)));
		// rule => cbf_rule.
		q(nt(205), (nt(112)));
		// rule => bf_rule.
		q(nt(205), (nt(134)));
		// rules_24 => rule.
		q(nt(207), (nt(205)));
		// rules_25 => rules_24 rules_25.
		q(nt(208), (nt(207)+nt(208)));
		// rules_25 => ε.
		q(nt(208), (nul));
		// rules => rules_25.
		q(nt(206), (nt(208)));
		// formula => rules main.
		q(nt(209), (nt(206)+nt(204)));
		// library => rules.
		q(nt(210), (nt(206)));
		// builder => captures definition builder_body dot.
		q(nt(211), (nt(57)+nt(34)+nt(212)+nt(37)));
		// builder_body => wff.
		q(nt(212), (nt(68)));
		// builder_body => cbf.
		q(nt(212), (nt(93)));
		// builder_body => bf.
		q(nt(212), (nt(119)));
		// start => inputs.
		q(nt(213), (nt(202)));
		// start => formula.
		q(nt(213), (nt(209)));
		// start => library.
		q(nt(213), (nt(210)));
		// start => builder.
		q(nt(213), (nt(211)));
		return q;
	}
};
#endif // __TAU_PARSER_H__
