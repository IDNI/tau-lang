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
		g(nts, load_prods(), nt(198), cc), p(g, load_opts()) {}
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
		ws_comment, _Rws_comment_0, ws_required, ws, hex_escape, unicode_escape, char_escape_encode, esc, q_char, q_str,
		q_bqstr, char_punct, _Rchar_punct_1, _Rchar_punct_2, _Rchar_punct_3, char0, char_, string_char, bqstring_char, chars,
		_Rchars_4, _Rchars_5, char_class, digits, _Rdigits_6, _Rdigits_7, definition, dot, open_parenthesis, close_parenthesis,
		open_bracket, close_bracket, open_brace, close_brace, minus, colon, semicolon, less, comma, sym,
		offsets, step, offset, _Roffsets_8, _Roffsets_9, num, capture, shift, variable, var,
		timed, in, out, _Rtimed_10, tau, wff, tau_and, tau_or, tau_neg, tau_and_sym,
		tau_or_sym, tau_neg_sym, wff_rule, wff_matcher, wff_body, wff_rec_relation, wff_ref, bf_eq_cb, bf_neq_cb, wff_has_clashing_subformulas_cb,
		wff_has_subformula_cb, wff_remove_existential, wff_and, wff_neg, wff_xor, wff_conditional, wff_or, wff_all, wff_ex, wff_imply,
		wff_equiv, wff_t, wff_f, bf_eq, bf_neq, bf_less, bf_less_equal, bf_greater, wff_ref_args, _Rwff_ref_args_11,
		_Rwff_ref_args_12, wff_and_sym, wff_or_sym, wff_xor_sym, wff_conditional_sym, wff_neg_sym, wff_imply_sym, wff_equiv_sym, wff_all_sym, _Rwff_all_13,
		wff_ex_sym, _Rwff_ex_14, bf, bf_equality_sym, bf_nequality_sym, bf_less_sym, bf_less_equal_sym, bf_greater_sym, bf_rule, bf_matcher,
		bf_body, bf_rec_relation, bf_ref, bf_is_zero_cb, bf_is_one_cb, bf_has_clashing_subformulas_cb, bf_has_subformula_cb, bf_constant, bf_and, bf_neg,
		bf_xor, bf_or, bf_all, bf_ex, bf_subs_cb, bf_t, bf_f, bf_ref_args, _Rbf_ref_args_15, _Rbf_ref_args_16,
		bf_and_sym, bf_or_sym, bf_xor_sym, bf_neg_sym, bf_all_sym, _Rbf_all_17, bf_ex_sym, _Rbf_ex_18, constant, binding,
		bf_and_cb, bf_or_cb, bf_xor_cb, bf_neg_cb, source_binding, named_binding, type, source, source0, _Rsource_19,
		_Rsource_20, bf_cb_arg, bf_and_cb_sym, bf_or_cb_sym, bf_xor_cb_sym, bf_neg_cb_sym, bf_subs_cb_sym, bf_eq_cb_sym, wff_cb_arg, bf_neq_cb_sym,
		bf_is_zero_cb_sym, bf_is_one_cb_sym, bf_has_clashing_subformulas_cb_sym, wff_has_clashing_subformulas_cb_sym, bf_has_subformula_cb_sym, wff_has_subformula_cb_sym, wff_remove_existential_cb_sym, input, inputs, _Rinputs_21,
		_Rinputs_22, main, rule, rules, _Rrules_23, _Rrules_24, rec_relation, rec_relations, _Rrec_relations_25, _Rrec_relations_26,
		formula, library, builder, builder_head, builder_body, _Rbuilder_head_27, _Rbuilder_head_28, gssotc, start, __neg_0,
		__neg_1, __neg_2, __neg_3, __neg_4, __neg_5,
	};
	size_t id(const std::basic_string<char>& name) { return nts.get(name); }
private:
	std::vector<char> ts{
		'\0', '\n', '\r', '#', '\\', 'x', 'u', '\'', '"',
		'`', 'a', 'l', 'n', 'm', 'p', 'h', 'b', 'k', 'c',
		't', 'r', 'd', 'i', 'g', 'e', 'o', 'f', 'w', 's',
		':', '=', '.', '(', ')', '[', ']', '{', '}', '-',
		';', '<', ',', '$', '?', '_', '&', '|', '!', '^',
		'>', 'T', 'F', '+', '~', '1', '0', 'q', 'z', 'v',
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
			"ws_comment", "_Rws_comment_0", "ws_required", "ws", "hex_escape", "unicode_escape", "char_escape_encode", "esc", "q_char", "q_str",
			"q_bqstr", "char_punct", "_Rchar_punct_1", "_Rchar_punct_2", "_Rchar_punct_3", "char0", "char_", "string_char", "bqstring_char", "chars",
			"_Rchars_4", "_Rchars_5", "char_class", "digits", "_Rdigits_6", "_Rdigits_7", "definition", "dot", "open_parenthesis", "close_parenthesis",
			"open_bracket", "close_bracket", "open_brace", "close_brace", "minus", "colon", "semicolon", "less", "comma", "sym",
			"offsets", "step", "offset", "_Roffsets_8", "_Roffsets_9", "num", "capture", "shift", "variable", "var",
			"timed", "in", "out", "_Rtimed_10", "tau", "wff", "tau_and", "tau_or", "tau_neg", "tau_and_sym",
			"tau_or_sym", "tau_neg_sym", "wff_rule", "wff_matcher", "wff_body", "wff_rec_relation", "wff_ref", "bf_eq_cb", "bf_neq_cb", "wff_has_clashing_subformulas_cb",
			"wff_has_subformula_cb", "wff_remove_existential", "wff_and", "wff_neg", "wff_xor", "wff_conditional", "wff_or", "wff_all", "wff_ex", "wff_imply",
			"wff_equiv", "wff_t", "wff_f", "bf_eq", "bf_neq", "bf_less", "bf_less_equal", "bf_greater", "wff_ref_args", "_Rwff_ref_args_11",
			"_Rwff_ref_args_12", "wff_and_sym", "wff_or_sym", "wff_xor_sym", "wff_conditional_sym", "wff_neg_sym", "wff_imply_sym", "wff_equiv_sym", "wff_all_sym", "_Rwff_all_13",
			"wff_ex_sym", "_Rwff_ex_14", "bf", "bf_equality_sym", "bf_nequality_sym", "bf_less_sym", "bf_less_equal_sym", "bf_greater_sym", "bf_rule", "bf_matcher",
			"bf_body", "bf_rec_relation", "bf_ref", "bf_is_zero_cb", "bf_is_one_cb", "bf_has_clashing_subformulas_cb", "bf_has_subformula_cb", "bf_constant", "bf_and", "bf_neg",
			"bf_xor", "bf_or", "bf_all", "bf_ex", "bf_subs_cb", "bf_t", "bf_f", "bf_ref_args", "_Rbf_ref_args_15", "_Rbf_ref_args_16",
			"bf_and_sym", "bf_or_sym", "bf_xor_sym", "bf_neg_sym", "bf_all_sym", "_Rbf_all_17", "bf_ex_sym", "_Rbf_ex_18", "constant", "binding",
			"bf_and_cb", "bf_or_cb", "bf_xor_cb", "bf_neg_cb", "source_binding", "named_binding", "type", "source", "source0", "_Rsource_19",
			"_Rsource_20", "bf_cb_arg", "bf_and_cb_sym", "bf_or_cb_sym", "bf_xor_cb_sym", "bf_neg_cb_sym", "bf_subs_cb_sym", "bf_eq_cb_sym", "wff_cb_arg", "bf_neq_cb_sym",
			"bf_is_zero_cb_sym", "bf_is_one_cb_sym", "bf_has_clashing_subformulas_cb_sym", "wff_has_clashing_subformulas_cb_sym", "bf_has_subformula_cb_sym", "wff_has_subformula_cb_sym", "wff_remove_existential_cb_sym", "input", "inputs", "_Rinputs_21",
			"_Rinputs_22", "main", "rule", "rules", "_Rrules_23", "_Rrules_24", "rec_relation", "rec_relations", "_Rrec_relations_25", "_Rrec_relations_26",
			"formula", "library", "builder", "builder_head", "builder_body", "_Rbuilder_head_27", "_Rbuilder_head_28", "gssotc", "start", "__neg_0",
			"__neg_1", "__neg_2", "__neg_3", "__neg_4", "__neg_5",
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
		// _Rws_comment_0 => printable.
		q(nt(11), (nt(8)));
		// _Rws_comment_0 => printable _Rws_comment_0.
		q(nt(11), (nt(8)+nt(11)));
		// ws_comment => '#' eol.
		q(nt(10), (t(3)+nt(9)));
		// ws_comment => '#' _Rws_comment_0 eol.
		q(nt(10), (t(3)+nt(11)+nt(9)));
		// ws_required => space ws.
		q(nt(12), (nt(2)+nt(13)));
		// ws_required => ws_comment ws.
		q(nt(12), (nt(10)+nt(13)));
		// ws => null.
		q(nt(13), (nul));
		// ws => ws_required.
		q(nt(13), (nt(12)));
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
		// _Rchar_punct_1 => esc q_char.
		q(nt(22), (nt(17)+nt(18)));
		// _Rchar_punct_2 => esc q_str.
		q(nt(23), (nt(17)+nt(19)));
		// _Rchar_punct_3 => esc q_bqstr.
		q(nt(24), (nt(17)+nt(20)));
		// __neg_0 => q_char.
		q(nt(199), (nt(18)));
		// __neg_1 => q_str.
		q(nt(200), (nt(19)));
		// __neg_2 => q_bqstr.
		q(nt(201), (nt(20)));
		// __neg_3 => _Rchar_punct_1.
		q(nt(202), (nt(22)));
		// __neg_4 => _Rchar_punct_2.
		q(nt(203), (nt(23)));
		// __neg_5 => _Rchar_punct_3.
		q(nt(204), (nt(24)));
		// char_punct => punct & ~( __neg_0 ) & ~( __neg_1 ) & ~( __neg_2 ) & ~( __neg_3 ) & ~( __neg_4 ) & ~( __neg_5 ).
		q(nt(21), (nt(7)) & ~(nt(199)) & ~(nt(200)) & ~(nt(201)) & ~(nt(202)) & ~(nt(203)) & ~(nt(204)));
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
		// _Rchars_4 => alnum.
		q(nt(30), (nt(6)));
		// _Rchars_5 => null.
		q(nt(31), (nul));
		// _Rchars_5 => _Rchars_4 _Rchars_5.
		q(nt(31), (nt(30)+nt(31)));
		// chars => alpha _Rchars_5.
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
		// _Rdigits_6 => digit.
		q(nt(34), (nt(3)));
		// _Rdigits_7 => null.
		q(nt(35), (nul));
		// _Rdigits_7 => _Rdigits_6 _Rdigits_7.
		q(nt(35), (nt(34)+nt(35)));
		// digits => digit _Rdigits_7.
		q(nt(33), (nt(3)+nt(35)));
		// definition => ws ':' '=' ws.
		q(nt(36), (nt(13)+t(29)+t(30)+nt(13)));
		// dot => ws '.' ws.
		q(nt(37), (nt(13)+t(31)+nt(13)));
		// open_parenthesis => ws '(' ws.
		q(nt(38), (nt(13)+t(32)+nt(13)));
		// close_parenthesis => ws ')' ws.
		q(nt(39), (nt(13)+t(33)+nt(13)));
		// open_bracket => ws '[' ws.
		q(nt(40), (nt(13)+t(34)+nt(13)));
		// close_bracket => ws ']' ws.
		q(nt(41), (nt(13)+t(35)+nt(13)));
		// open_brace => ws '{' ws.
		q(nt(42), (nt(13)+t(36)+nt(13)));
		// close_brace => ws '}' ws.
		q(nt(43), (nt(13)+t(37)+nt(13)));
		// minus => ws '-' ws.
		q(nt(44), (nt(13)+t(38)+nt(13)));
		// colon => ws ':' ws.
		q(nt(45), (nt(13)+t(29)+nt(13)));
		// semicolon => ws ';' ws.
		q(nt(46), (nt(13)+t(39)+nt(13)));
		// less => ws '<' ws.
		q(nt(47), (nt(13)+t(40)+nt(13)));
		// comma => ws ',' ws.
		q(nt(48), (nt(13)+t(41)+nt(13)));
		// sym => chars.
		q(nt(49), (nt(29)));
		// _Roffsets_8 => comma offset.
		q(nt(53), (nt(48)+nt(52)));
		// _Roffsets_9 => null.
		q(nt(54), (nul));
		// _Roffsets_9 => _Roffsets_8 _Roffsets_9.
		q(nt(54), (nt(53)+nt(54)));
		// offsets => open_bracket step _Roffsets_9 close_bracket.
		q(nt(50), (nt(40)+nt(51)+nt(54)+nt(41)));
		// offset => num.
		q(nt(52), (nt(55)));
		// offset => capture.
		q(nt(52), (nt(56)));
		// offset => shift.
		q(nt(52), (nt(57)));
		// step => num.
		q(nt(51), (nt(55)));
		// step => capture.
		q(nt(51), (nt(56)));
		// step => shift.
		q(nt(51), (nt(57)));
		// shift => capture minus num.
		q(nt(57), (nt(56)+nt(44)+nt(55)));
		// num => ws digits ws.
		q(nt(55), (nt(13)+nt(33)+nt(13)));
		// variable => var.
		q(nt(58), (nt(59)));
		// variable => timed.
		q(nt(58), (nt(60)));
		// _Rtimed_10 => in.
		q(nt(63), (nt(61)));
		// _Rtimed_10 => out.
		q(nt(63), (nt(62)));
		// timed => _Rtimed_10 offsets.
		q(nt(60), (nt(63)+nt(50)));
		// capture => '$' chars.
		q(nt(56), (t(42)+nt(29)));
		// var => chars.
		q(nt(59), (nt(29)));
		// in => '?' 'i' '_' chars.
		q(nt(61), (t(43)+t(22)+t(44)+nt(29)));
		// out => '?' 'o' '_' chars.
		q(nt(62), (t(43)+t(25)+t(44)+nt(29)));
		// tau => wff.
		q(nt(64), (nt(65)));
		// tau => tau_and.
		q(nt(64), (nt(66)));
		// tau => tau_or.
		q(nt(64), (nt(67)));
		// tau => tau_neg.
		q(nt(64), (nt(68)));
		// tau_and => open_parenthesis tau tau_and_sym tau close_parenthesis.
		q(nt(66), (nt(38)+nt(64)+nt(69)+nt(64)+nt(39)));
		// tau_or => open_parenthesis tau tau_or_sym tau close_parenthesis.
		q(nt(67), (nt(38)+nt(64)+nt(70)+nt(64)+nt(39)));
		// tau_neg => tau_neg_sym tau.
		q(nt(68), (nt(71)+nt(64)));
		// tau_and_sym => ws '&' '&' '&' ws.
		q(nt(69), (nt(13)+t(45)+t(45)+t(45)+nt(13)));
		// tau_or_sym => ws '|' '|' '|' ws.
		q(nt(70), (nt(13)+t(46)+t(46)+t(46)+nt(13)));
		// tau_neg_sym => ws '!' '!' '!' ws.
		q(nt(71), (nt(13)+t(47)+t(47)+t(47)+nt(13)));
		// wff_rule => wff_matcher definition wff_body dot.
		q(nt(72), (nt(73)+nt(36)+nt(74)+nt(37)));
		// wff_rec_relation => wff_ref definition wff dot.
		q(nt(75), (nt(76)+nt(36)+nt(65)+nt(37)));
		// wff_matcher => wff.
		q(nt(73), (nt(65)));
		// wff_matcher => wff_ref.
		q(nt(73), (nt(76)));
		// wff_body => wff.
		q(nt(74), (nt(65)));
		// wff_body => bf_eq_cb.
		q(nt(74), (nt(77)));
		// wff_body => bf_neq_cb.
		q(nt(74), (nt(78)));
		// wff_body => wff_has_clashing_subformulas_cb.
		q(nt(74), (nt(79)));
		// wff_body => wff_has_subformula_cb.
		q(nt(74), (nt(80)));
		// wff_body => wff_remove_existential.
		q(nt(74), (nt(81)));
		// wff => capture.
		q(nt(65), (nt(56)));
		// wff => wff_ref.
		q(nt(65), (nt(76)));
		// wff => wff_and.
		q(nt(65), (nt(82)));
		// wff => wff_neg.
		q(nt(65), (nt(83)));
		// wff => wff_xor.
		q(nt(65), (nt(84)));
		// wff => wff_conditional.
		q(nt(65), (nt(85)));
		// wff => wff_or.
		q(nt(65), (nt(86)));
		// wff => wff_all.
		q(nt(65), (nt(87)));
		// wff => wff_ex.
		q(nt(65), (nt(88)));
		// wff => wff_imply.
		q(nt(65), (nt(89)));
		// wff => wff_equiv.
		q(nt(65), (nt(90)));
		// wff => wff_t.
		q(nt(65), (nt(91)));
		// wff => wff_f.
		q(nt(65), (nt(92)));
		// wff => bf_eq.
		q(nt(65), (nt(93)));
		// wff => bf_neq.
		q(nt(65), (nt(94)));
		// wff => bf_less.
		q(nt(65), (nt(95)));
		// wff => bf_less_equal.
		q(nt(65), (nt(96)));
		// wff => bf_greater.
		q(nt(65), (nt(97)));
		// wff_ref => sym offsets wff_ref_args.
		q(nt(76), (nt(49)+nt(50)+nt(98)));
		// _Rwff_ref_args_11 => variable.
		q(nt(99), (nt(58)));
		// _Rwff_ref_args_12 => null.
		q(nt(100), (nul));
		// _Rwff_ref_args_12 => _Rwff_ref_args_11 _Rwff_ref_args_12.
		q(nt(100), (nt(99)+nt(100)));
		// wff_ref_args => open_parenthesis _Rwff_ref_args_12 close_parenthesis.
		q(nt(98), (nt(38)+nt(100)+nt(39)));
		// wff_and => open_parenthesis wff wff_and_sym wff close_parenthesis.
		q(nt(82), (nt(38)+nt(65)+nt(101)+nt(65)+nt(39)));
		// wff_or => open_parenthesis wff wff_or_sym wff close_parenthesis.
		q(nt(86), (nt(38)+nt(65)+nt(102)+nt(65)+nt(39)));
		// wff_xor => open_parenthesis wff wff_xor_sym wff close_parenthesis.
		q(nt(84), (nt(38)+nt(65)+nt(103)+nt(65)+nt(39)));
		// wff_conditional => open_parenthesis wff wff_conditional_sym wff colon wff close_parenthesis.
		q(nt(85), (nt(38)+nt(65)+nt(104)+nt(65)+nt(45)+nt(65)+nt(39)));
		// wff_neg => wff_neg_sym wff.
		q(nt(83), (nt(105)+nt(65)));
		// wff_imply => open_parenthesis wff wff_imply_sym wff close_parenthesis.
		q(nt(89), (nt(38)+nt(65)+nt(106)+nt(65)+nt(39)));
		// wff_equiv => open_parenthesis wff wff_equiv_sym wff close_parenthesis.
		q(nt(90), (nt(38)+nt(65)+nt(107)+nt(65)+nt(39)));
		// _Rwff_all_13 => capture.
		q(nt(109), (nt(56)));
		// _Rwff_all_13 => variable.
		q(nt(109), (nt(58)));
		// wff_all => wff_all_sym _Rwff_all_13 ws_required wff.
		q(nt(87), (nt(108)+nt(109)+nt(12)+nt(65)));
		// _Rwff_ex_14 => capture.
		q(nt(111), (nt(56)));
		// _Rwff_ex_14 => variable.
		q(nt(111), (nt(58)));
		// wff_ex => wff_ex_sym ws_required _Rwff_ex_14 ws_required wff.
		q(nt(88), (nt(110)+nt(12)+nt(111)+nt(12)+nt(65)));
		// bf_eq => open_parenthesis bf bf_equality_sym bf close_parenthesis.
		q(nt(93), (nt(38)+nt(112)+nt(113)+nt(112)+nt(39)));
		// bf_neq => open_parenthesis bf bf_nequality_sym bf close_parenthesis.
		q(nt(94), (nt(38)+nt(112)+nt(114)+nt(112)+nt(39)));
		// bf_less => open_parenthesis bf bf_less_sym bf close_parenthesis.
		q(nt(95), (nt(38)+nt(112)+nt(115)+nt(112)+nt(39)));
		// bf_less_equal => open_parenthesis bf bf_less_equal_sym bf close_parenthesis.
		q(nt(96), (nt(38)+nt(112)+nt(116)+nt(112)+nt(39)));
		// bf_greater => open_parenthesis bf bf_greater_sym bf close_parenthesis.
		q(nt(97), (nt(38)+nt(112)+nt(117)+nt(112)+nt(39)));
		// wff_and_sym => ws '&' '&' ws.
		q(nt(101), (nt(13)+t(45)+t(45)+nt(13)));
		// wff_or_sym => ws '|' '|' ws.
		q(nt(102), (nt(13)+t(46)+t(46)+nt(13)));
		// wff_xor_sym => ws '^' ws.
		q(nt(103), (nt(13)+t(48)+nt(13)));
		// wff_conditional_sym => ws '?' ws.
		q(nt(104), (nt(13)+t(43)+nt(13)));
		// wff_neg_sym => ws '!' ws.
		q(nt(105), (nt(13)+t(47)+nt(13)));
		// wff_imply_sym => ws '-' '>' ws.
		q(nt(106), (nt(13)+t(38)+t(49)+nt(13)));
		// wff_equiv_sym => ws '<' '-' '>' ws.
		q(nt(107), (nt(13)+t(40)+t(38)+t(49)+nt(13)));
		// wff_all_sym => ws 'a' 'l' 'l' ws.
		q(nt(108), (nt(13)+t(10)+t(11)+t(11)+nt(13)));
		// wff_ex_sym => ws 'e' 'x' ws.
		q(nt(110), (nt(13)+t(24)+t(5)+nt(13)));
		// wff_t => ws 'T' ws.
		q(nt(91), (nt(13)+t(50)+nt(13)));
		// wff_f => ws 'F' ws.
		q(nt(92), (nt(13)+t(51)+nt(13)));
		// bf_rule => bf_matcher definition bf_body dot.
		q(nt(118), (nt(119)+nt(36)+nt(120)+nt(37)));
		// bf_rec_relation => bf_ref definition bf dot.
		q(nt(121), (nt(122)+nt(36)+nt(112)+nt(37)));
		// bf_matcher => bf.
		q(nt(119), (nt(112)));
		// bf_body => bf.
		q(nt(120), (nt(112)));
		// bf_body => bf_is_zero_cb.
		q(nt(120), (nt(123)));
		// bf_body => bf_is_one_cb.
		q(nt(120), (nt(124)));
		// bf_body => bf_has_clashing_subformulas_cb.
		q(nt(120), (nt(125)));
		// bf_body => bf_has_subformula_cb.
		q(nt(120), (nt(126)));
		// bf => capture.
		q(nt(112), (nt(56)));
		// bf => variable.
		q(nt(112), (nt(58)));
		// bf => bf_ref.
		q(nt(112), (nt(122)));
		// bf => bf_constant.
		q(nt(112), (nt(127)));
		// bf => bf_and.
		q(nt(112), (nt(128)));
		// bf => bf_neg.
		q(nt(112), (nt(129)));
		// bf => bf_xor.
		q(nt(112), (nt(130)));
		// bf => bf_or.
		q(nt(112), (nt(131)));
		// bf => bf_all.
		q(nt(112), (nt(132)));
		// bf => bf_ex.
		q(nt(112), (nt(133)));
		// bf => bf_subs_cb.
		q(nt(112), (nt(134)));
		// bf => bf_t.
		q(nt(112), (nt(135)));
		// bf => bf_f.
		q(nt(112), (nt(136)));
		// bf_ref => sym offsets bf_ref_args.
		q(nt(122), (nt(49)+nt(50)+nt(137)));
		// _Rbf_ref_args_15 => variable.
		q(nt(138), (nt(58)));
		// _Rbf_ref_args_16 => null.
		q(nt(139), (nul));
		// _Rbf_ref_args_16 => _Rbf_ref_args_15 _Rbf_ref_args_16.
		q(nt(139), (nt(138)+nt(139)));
		// bf_ref_args => open_parenthesis _Rbf_ref_args_16 close_parenthesis.
		q(nt(137), (nt(38)+nt(139)+nt(39)));
		// bf_and => open_parenthesis bf bf_and_sym bf close_parenthesis.
		q(nt(128), (nt(38)+nt(112)+nt(140)+nt(112)+nt(39)));
		// bf_or => open_parenthesis bf bf_or_sym bf close_parenthesis.
		q(nt(131), (nt(38)+nt(112)+nt(141)+nt(112)+nt(39)));
		// bf_xor => open_parenthesis bf bf_xor_sym ws bf close_parenthesis.
		q(nt(130), (nt(38)+nt(112)+nt(142)+nt(13)+nt(112)+nt(39)));
		// bf_neg => bf_neg_sym bf.
		q(nt(129), (nt(143)+nt(112)));
		// _Rbf_all_17 => capture.
		q(nt(145), (nt(56)));
		// _Rbf_all_17 => variable.
		q(nt(145), (nt(58)));
		// bf_all => bf_all_sym ws_required _Rbf_all_17 ws_required bf.
		q(nt(132), (nt(144)+nt(12)+nt(145)+nt(12)+nt(112)));
		// _Rbf_ex_18 => capture.
		q(nt(147), (nt(56)));
		// _Rbf_ex_18 => variable.
		q(nt(147), (nt(58)));
		// bf_ex => bf_ex_sym ws_required _Rbf_ex_18 ws_required bf.
		q(nt(133), (nt(146)+nt(12)+nt(147)+nt(12)+nt(112)));
		// bf_and_sym => ws '&' ws.
		q(nt(140), (nt(13)+t(45)+nt(13)));
		// bf_or_sym => ws '|' ws.
		q(nt(141), (nt(13)+t(46)+nt(13)));
		// bf_xor_sym => ws '+' ws.
		q(nt(142), (nt(13)+t(52)+nt(13)));
		// bf_neg_sym => ws '~' ws.
		q(nt(143), (nt(13)+t(53)+nt(13)));
		// bf_equality_sym => ws '=' ws.
		q(nt(113), (nt(13)+t(30)+nt(13)));
		// bf_nequality_sym => ws '!' '=' ws.
		q(nt(114), (nt(13)+t(47)+t(30)+nt(13)));
		// bf_less_sym => ws '<' ws.
		q(nt(115), (nt(13)+t(40)+nt(13)));
		// bf_less_equal_sym => ws '<' '=' ws.
		q(nt(116), (nt(13)+t(40)+t(30)+nt(13)));
		// bf_greater_sym => ws '>' ws.
		q(nt(117), (nt(13)+t(49)+nt(13)));
		// bf_all_sym => ws 'f' 'a' 'l' 'l' ws.
		q(nt(144), (nt(13)+t(26)+t(10)+t(11)+t(11)+nt(13)));
		// bf_ex_sym => ws 'f' 'e' 'x' ws.
		q(nt(146), (nt(13)+t(26)+t(24)+t(5)+nt(13)));
		// bf_t => ws '1' ws.
		q(nt(135), (nt(13)+t(54)+nt(13)));
		// bf_f => ws '0' ws.
		q(nt(136), (nt(13)+t(55)+nt(13)));
		// bf_constant => open_brace constant close_brace.
		q(nt(127), (nt(42)+nt(148)+nt(43)));
		// constant => capture.
		q(nt(148), (nt(56)));
		// constant => binding.
		q(nt(148), (nt(149)));
		// constant => bf_and_cb.
		q(nt(148), (nt(150)));
		// constant => bf_or_cb.
		q(nt(148), (nt(151)));
		// constant => bf_xor_cb.
		q(nt(148), (nt(152)));
		// constant => bf_neg_cb.
		q(nt(148), (nt(153)));
		// binding => source_binding.
		q(nt(149), (nt(154)));
		// binding => named_binding.
		q(nt(149), (nt(155)));
		// named_binding => chars.
		q(nt(155), (nt(29)));
		// source_binding => type colon source.
		q(nt(154), (nt(156)+nt(45)+nt(157)));
		// type => null.
		q(nt(156), (nul));
		// type => chars.
		q(nt(156), (nt(29)));
		// source0 => space.
		q(nt(158), (nt(2)));
		// source0 => alnum.
		q(nt(158), (nt(6)));
		// source0 => char_escape_encode.
		q(nt(158), (nt(16)));
		// source0 => char_punct.
		q(nt(158), (nt(21)));
		// _Rsource_19 => source0.
		q(nt(159), (nt(158)));
		// _Rsource_20 => _Rsource_19.
		q(nt(160), (nt(159)));
		// _Rsource_20 => _Rsource_19 _Rsource_20.
		q(nt(160), (nt(159)+nt(160)));
		// source => _Rsource_20.
		q(nt(157), (nt(160)));
		// bf_and_cb => bf_cb_arg bf_and_cb_sym bf_cb_arg.
		q(nt(150), (nt(161)+nt(162)+nt(161)));
		// bf_or_cb => bf_cb_arg bf_or_cb_sym bf_cb_arg.
		q(nt(151), (nt(161)+nt(163)+nt(161)));
		// bf_xor_cb => bf_cb_arg bf_xor_cb_sym bf_cb_arg.
		q(nt(152), (nt(161)+nt(164)+nt(161)));
		// bf_neg_cb => bf_neg_cb_sym bf_cb_arg.
		q(nt(153), (nt(165)+nt(161)));
		// bf_subs_cb => bf_subs_cb_sym bf_cb_arg ws_required bf_cb_arg ws_required bf_cb_arg.
		q(nt(134), (nt(166)+nt(161)+nt(12)+nt(161)+nt(12)+nt(161)));
		// bf_eq_cb => bf_eq_cb_sym bf_cb_arg ws_required wff_cb_arg ws_required wff_cb_arg.
		q(nt(77), (nt(167)+nt(161)+nt(12)+nt(168)+nt(12)+nt(168)));
		// bf_neq_cb => bf_neq_cb_sym bf_cb_arg ws_required wff_cb_arg ws_required wff_cb_arg.
		q(nt(78), (nt(169)+nt(161)+nt(12)+nt(168)+nt(12)+nt(168)));
		// bf_is_zero_cb => bf_is_zero_cb_sym bf_cb_arg ws_required bf_cb_arg.
		q(nt(123), (nt(170)+nt(161)+nt(12)+nt(161)));
		// bf_is_one_cb => bf_is_one_cb_sym bf_cb_arg ws_required bf_cb_arg.
		q(nt(124), (nt(171)+nt(161)+nt(12)+nt(161)));
		// bf_has_clashing_subformulas_cb => bf_has_clashing_subformulas_cb_sym bf_cb_arg ws_required bf_cb_arg.
		q(nt(125), (nt(172)+nt(161)+nt(12)+nt(161)));
		// wff_has_clashing_subformulas_cb => wff_has_clashing_subformulas_cb_sym wff_cb_arg ws_required wff_cb_arg.
		q(nt(79), (nt(173)+nt(168)+nt(12)+nt(168)));
		// bf_has_subformula_cb => bf_has_subformula_cb_sym bf_cb_arg ws_required bf_cb_arg ws_required bf_cb_arg.
		q(nt(126), (nt(174)+nt(161)+nt(12)+nt(161)+nt(12)+nt(161)));
		// wff_has_subformula_cb => wff_has_subformula_cb_sym wff_cb_arg ws_required wff_cb_arg ws_required wff_cb_arg.
		q(nt(80), (nt(175)+nt(168)+nt(12)+nt(168)+nt(12)+nt(168)));
		// wff_remove_existential => wff_remove_existential_cb_sym wff_cb_arg ws_required wff_cb_arg.
		q(nt(81), (nt(176)+nt(168)+nt(12)+nt(168)));
		// bf_cb_arg => capture.
		q(nt(161), (nt(56)));
		// bf_cb_arg => bf.
		q(nt(161), (nt(112)));
		// wff_cb_arg => capture.
		q(nt(168), (nt(56)));
		// wff_cb_arg => wff.
		q(nt(168), (nt(65)));
		// bf_and_cb_sym => ws 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b' ws.
		q(nt(162), (nt(13)+t(16)+t(26)+t(44)+t(10)+t(12)+t(21)+t(44)+t(18)+t(16)+nt(13)));
		// bf_or_cb_sym => ws 'b' 'f' '_' 'o' 'r' '_' 'c' 'b' ws.
		q(nt(163), (nt(13)+t(16)+t(26)+t(44)+t(25)+t(20)+t(44)+t(18)+t(16)+nt(13)));
		// bf_xor_cb_sym => ws 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b' ws.
		q(nt(164), (nt(13)+t(16)+t(26)+t(44)+t(5)+t(25)+t(20)+t(44)+t(18)+t(16)+nt(13)));
		// bf_neg_cb_sym => ws 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b' ws.
		q(nt(165), (nt(13)+t(16)+t(26)+t(44)+t(12)+t(24)+t(23)+t(44)+t(18)+t(16)+nt(13)));
		// bf_subs_cb_sym => ws 'b' 'f' '_' 's' 'u' 'b' 's' '_' 'c' 'b' ws.
		q(nt(166), (nt(13)+t(16)+t(26)+t(44)+t(28)+t(6)+t(16)+t(28)+t(44)+t(18)+t(16)+nt(13)));
		// bf_eq_cb_sym => ws 'b' 'f' '_' 'e' 'q' '_' 'c' 'b' ws.
		q(nt(167), (nt(13)+t(16)+t(26)+t(44)+t(24)+t(56)+t(44)+t(18)+t(16)+nt(13)));
		// bf_neq_cb_sym => ws 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b' ws.
		q(nt(169), (nt(13)+t(16)+t(26)+t(44)+t(12)+t(24)+t(56)+t(44)+t(18)+t(16)+nt(13)));
		// bf_is_zero_cb_sym => ws 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b' ws.
		q(nt(170), (nt(13)+t(16)+t(26)+t(44)+t(22)+t(28)+t(44)+t(57)+t(24)+t(20)+t(25)+t(44)+t(18)+t(16)+nt(13)));
		// bf_is_one_cb_sym => ws 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b' ws.
		q(nt(171), (nt(13)+t(16)+t(26)+t(44)+t(22)+t(28)+t(44)+t(25)+t(12)+t(24)+t(44)+t(18)+t(16)+nt(13)));
		// bf_has_clashing_subformulas_cb_sym => ws 'b' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' ws.
		q(nt(172), (nt(13)+t(16)+t(26)+t(44)+t(15)+t(10)+t(28)+t(44)+t(18)+t(11)+t(10)+t(28)+t(15)+t(22)+t(12)+t(23)+t(44)+t(28)+t(6)+t(16)+t(26)+t(25)+t(20)+t(13)+t(6)+t(11)+t(10)+t(28)+t(44)+t(18)+t(16)+nt(13)));
		// bf_has_subformula_cb_sym => ws 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' ws.
		q(nt(174), (nt(13)+t(16)+t(26)+t(44)+t(15)+t(10)+t(28)+t(44)+t(28)+t(6)+t(16)+t(26)+t(25)+t(20)+t(13)+t(6)+t(11)+t(10)+t(44)+t(18)+t(16)+nt(13)));
		// wff_has_clashing_subformulas_cb_sym => ws 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' ws.
		q(nt(173), (nt(13)+t(27)+t(26)+t(26)+t(44)+t(15)+t(10)+t(28)+t(44)+t(18)+t(11)+t(10)+t(28)+t(15)+t(22)+t(12)+t(23)+t(44)+t(28)+t(6)+t(16)+t(26)+t(25)+t(20)+t(13)+t(6)+t(11)+t(10)+t(28)+t(44)+t(18)+t(16)+nt(13)));
		// wff_has_subformula_cb_sym => ws 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' ws.
		q(nt(175), (nt(13)+t(27)+t(26)+t(26)+t(44)+t(15)+t(10)+t(28)+t(44)+t(28)+t(6)+t(16)+t(26)+t(25)+t(20)+t(13)+t(6)+t(11)+t(10)+t(44)+t(18)+t(16)+nt(13)));
		// wff_remove_existential_cb_sym => ws 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' ws.
		q(nt(176), (nt(13)+t(27)+t(26)+t(26)+t(44)+t(20)+t(24)+t(13)+t(25)+t(58)+t(24)+t(44)+t(24)+t(5)+t(22)+t(28)+t(19)+t(24)+t(12)+t(19)+t(22)+t(10)+t(11)+t(44)+t(18)+t(16)+nt(13)));
		// input => in colon open_brace source_binding close_brace.
		q(nt(177), (nt(61)+nt(45)+nt(42)+nt(154)+nt(43)));
		// _Rinputs_21 => input.
		q(nt(179), (nt(177)));
		// _Rinputs_22 => null.
		q(nt(180), (nul));
		// _Rinputs_22 => _Rinputs_21 _Rinputs_22.
		q(nt(180), (nt(179)+nt(180)));
		// inputs => less input _Rinputs_22 dot.
		q(nt(178), (nt(47)+nt(177)+nt(180)+nt(37)));
		// main => wff dot.
		q(nt(181), (nt(65)+nt(37)));
		// rule => wff_rule.
		q(nt(182), (nt(72)));
		// rule => bf_rule.
		q(nt(182), (nt(118)));
		// _Rrules_23 => rule.
		q(nt(184), (nt(182)));
		// _Rrules_24 => null.
		q(nt(185), (nul));
		// _Rrules_24 => _Rrules_23 _Rrules_24.
		q(nt(185), (nt(184)+nt(185)));
		// rules => _Rrules_24.
		q(nt(183), (nt(185)));
		// rec_relation => wff_rec_relation dot.
		q(nt(186), (nt(75)+nt(37)));
		// rec_relation => bf_rec_relation.
		q(nt(186), (nt(121)));
		// _Rrec_relations_25 => rec_relation.
		q(nt(188), (nt(186)));
		// _Rrec_relations_26 => null.
		q(nt(189), (nul));
		// _Rrec_relations_26 => _Rrec_relations_25 _Rrec_relations_26.
		q(nt(189), (nt(188)+nt(189)));
		// rec_relations => _Rrec_relations_26.
		q(nt(187), (nt(189)));
		// formula => rec_relations main.
		q(nt(190), (nt(187)+nt(181)));
		// library => rules.
		q(nt(191), (nt(183)));
		// builder => builder_head definition builder_body dot.
		q(nt(192), (nt(193)+nt(36)+nt(194)+nt(37)));
		// _Rbuilder_head_27 => ws_required capture.
		q(nt(195), (nt(12)+nt(56)));
		// _Rbuilder_head_28 => null.
		q(nt(196), (nul));
		// _Rbuilder_head_28 => _Rbuilder_head_27 _Rbuilder_head_28.
		q(nt(196), (nt(195)+nt(196)));
		// builder_head => open_parenthesis capture _Rbuilder_head_28 close_parenthesis.
		q(nt(193), (nt(38)+nt(56)+nt(196)+nt(39)));
		// builder_body => wff.
		q(nt(194), (nt(65)));
		// builder_body => bf.
		q(nt(194), (nt(112)));
		// gssotc => tau semicolon.
		q(nt(197), (nt(64)+nt(46)));
		// start => inputs.
		q(nt(198), (nt(178)));
		// start => formula.
		q(nt(198), (nt(190)));
		// start => library.
		q(nt(198), (nt(191)));
		// start => builder.
		q(nt(198), (nt(192)));
		// start => gssotc.
		q(nt(198), (nt(197)));
		return q;
	}
};
#endif // __TAU_PARSER_H__
