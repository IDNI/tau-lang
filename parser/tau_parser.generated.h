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
		g(nts, load_prods(), nt(189), cc), p(g, load_opts()) {}
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
		open_bracket, close_bracket, open_brace, close_brace, minus, colon, less, comma, sym, offsets,
		step, offset, _Roffsets_8, _Roffsets_9, num, capture, shift, variable, var, timed,
		in, out, _Rtimed_10, wff_rule, wff_matcher, wff_body, wff_rec_relation, wff_ref, wff, bf_eq_cb,
		bf_neq_cb, wff_has_clashing_subformulas_cb, wff_has_subformula_cb, wff_remove_existential, wff_and, wff_neg, wff_xor, wff_conditional, wff_or, wff_all,
		wff_ex, wff_imply, wff_equiv, wff_t, wff_f, bf_eq, bf_neq, bf_less, bf_less_equal, bf_greater,
		wff_ref_args, _Rwff_ref_args_11, _Rwff_ref_args_12, wff_and_sym, wff_or_sym, wff_xor_sym, wff_conditional_sym, wff_neg_sym, wff_imply_sym, wff_equiv_sym,
		wff_all_sym, _Rwff_all_13, wff_ex_sym, _Rwff_ex_14, bf, bf_equality_sym, bf_nequality_sym, bf_less_sym, bf_less_equal_sym, bf_greater_sym,
		bf_rule, bf_matcher, bf_body, bf_rec_relation, bf_ref, bf_is_zero_cb, bf_is_one_cb, bf_has_clashing_subformulas_cb, bf_has_subformula_cb, bf_constant,
		bf_and, bf_neg, bf_xor, bf_or, bf_all, bf_ex, bf_subs_cb, bf_t, bf_f, bf_ref_args,
		_Rbf_ref_args_15, _Rbf_ref_args_16, bf_and_sym, bf_or_sym, bf_xor_sym, bf_neg_sym, bf_all_sym, _Rbf_all_17, bf_ex_sym, _Rbf_ex_18,
		constant, binding, bf_and_cb, bf_or_cb, bf_xor_cb, bf_neg_cb, source_binding, named_binding, type, source,
		source0, _Rsource_19, _Rsource_20, bf_cb_arg, bf_and_cb_sym, bf_or_cb_sym, bf_xor_cb_sym, bf_neg_cb_sym, bf_subs_cb_sym, bf_eq_cb_sym,
		wff_cb_arg, bf_neq_cb_sym, bf_is_zero_cb_sym, bf_is_one_cb_sym, bf_has_clashing_subformulas_cb_sym, wff_has_clashing_subformulas_cb_sym, bf_has_subformula_cb_sym, wff_has_subformula_cb_sym, wff_remove_existential_cb_sym, input,
		inputs, _Rinputs_21, _Rinputs_22, main, rule, rules, _Rrules_23, _Rrules_24, rec_relation, rec_relations,
		_Rrec_relations_25, _Rrec_relations_26, formula, library, builder, builder_head, builder_body, _Rbuilder_head_27, _Rbuilder_head_28, start,
		__neg_0, __neg_1, __neg_2, __neg_3, __neg_4, __neg_5,
	};
	size_t id(const std::basic_string<char>& name) { return nts.get(name); }
private:
	std::vector<char> ts{
		'\0', '\n', '\r', '#', '\\', 'x', 'u', '\'', '"',
		'`', 'a', 'l', 'n', 'm', 'p', 'h', 'b', 'k', 'c',
		't', 'r', 'd', 'i', 'g', 'e', 'o', 'f', 'w', 's',
		':', '=', '.', '(', ')', '[', ']', '{', '}', '-',
		'<', ',', '$', '?', '_', '&', '|', '^', '!', '>',
		'T', 'F', '+', '~', '1', '0', 'q', 'z', 'v',
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
			"open_bracket", "close_bracket", "open_brace", "close_brace", "minus", "colon", "less", "comma", "sym", "offsets",
			"step", "offset", "_Roffsets_8", "_Roffsets_9", "num", "capture", "shift", "variable", "var", "timed",
			"in", "out", "_Rtimed_10", "wff_rule", "wff_matcher", "wff_body", "wff_rec_relation", "wff_ref", "wff", "bf_eq_cb",
			"bf_neq_cb", "wff_has_clashing_subformulas_cb", "wff_has_subformula_cb", "wff_remove_existential", "wff_and", "wff_neg", "wff_xor", "wff_conditional", "wff_or", "wff_all",
			"wff_ex", "wff_imply", "wff_equiv", "wff_t", "wff_f", "bf_eq", "bf_neq", "bf_less", "bf_less_equal", "bf_greater",
			"wff_ref_args", "_Rwff_ref_args_11", "_Rwff_ref_args_12", "wff_and_sym", "wff_or_sym", "wff_xor_sym", "wff_conditional_sym", "wff_neg_sym", "wff_imply_sym", "wff_equiv_sym",
			"wff_all_sym", "_Rwff_all_13", "wff_ex_sym", "_Rwff_ex_14", "bf", "bf_equality_sym", "bf_nequality_sym", "bf_less_sym", "bf_less_equal_sym", "bf_greater_sym",
			"bf_rule", "bf_matcher", "bf_body", "bf_rec_relation", "bf_ref", "bf_is_zero_cb", "bf_is_one_cb", "bf_has_clashing_subformulas_cb", "bf_has_subformula_cb", "bf_constant",
			"bf_and", "bf_neg", "bf_xor", "bf_or", "bf_all", "bf_ex", "bf_subs_cb", "bf_t", "bf_f", "bf_ref_args",
			"_Rbf_ref_args_15", "_Rbf_ref_args_16", "bf_and_sym", "bf_or_sym", "bf_xor_sym", "bf_neg_sym", "bf_all_sym", "_Rbf_all_17", "bf_ex_sym", "_Rbf_ex_18",
			"constant", "binding", "bf_and_cb", "bf_or_cb", "bf_xor_cb", "bf_neg_cb", "source_binding", "named_binding", "type", "source",
			"source0", "_Rsource_19", "_Rsource_20", "bf_cb_arg", "bf_and_cb_sym", "bf_or_cb_sym", "bf_xor_cb_sym", "bf_neg_cb_sym", "bf_subs_cb_sym", "bf_eq_cb_sym",
			"wff_cb_arg", "bf_neq_cb_sym", "bf_is_zero_cb_sym", "bf_is_one_cb_sym", "bf_has_clashing_subformulas_cb_sym", "wff_has_clashing_subformulas_cb_sym", "bf_has_subformula_cb_sym", "wff_has_subformula_cb_sym", "wff_remove_existential_cb_sym", "input",
			"inputs", "_Rinputs_21", "_Rinputs_22", "main", "rule", "rules", "_Rrules_23", "_Rrules_24", "rec_relation", "rec_relations",
			"_Rrec_relations_25", "_Rrec_relations_26", "formula", "library", "builder", "builder_head", "builder_body", "_Rbuilder_head_27", "_Rbuilder_head_28", "start",
			"__neg_0", "__neg_1", "__neg_2", "__neg_3", "__neg_4", "__neg_5",
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
		// ws => ws_required.
		q(nt(13), (nt(12)));
		// ws => null.
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
		// _Rchar_punct_1 => esc q_char.
		q(nt(22), (nt(17)+nt(18)));
		// _Rchar_punct_2 => esc q_str.
		q(nt(23), (nt(17)+nt(19)));
		// _Rchar_punct_3 => esc q_bqstr.
		q(nt(24), (nt(17)+nt(20)));
		// __neg_0 => q_char.
		q(nt(190), (nt(18)));
		// __neg_1 => q_str.
		q(nt(191), (nt(19)));
		// __neg_2 => q_bqstr.
		q(nt(192), (nt(20)));
		// __neg_3 => _Rchar_punct_1.
		q(nt(193), (nt(22)));
		// __neg_4 => _Rchar_punct_2.
		q(nt(194), (nt(23)));
		// __neg_5 => _Rchar_punct_3.
		q(nt(195), (nt(24)));
		// char_punct => punct & ~( __neg_0 ) & ~( __neg_1 ) & ~( __neg_2 ) & ~( __neg_3 ) & ~( __neg_4 ) & ~( __neg_5 ).
		q(nt(21), (nt(7)) & ~(nt(190)) & ~(nt(191)) & ~(nt(192)) & ~(nt(193)) & ~(nt(194)) & ~(nt(195)));
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
		// _Rchars_5 => _Rchars_4 _Rchars_5.
		q(nt(31), (nt(30)+nt(31)));
		// _Rchars_5 => null.
		q(nt(31), (nul));
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
		// _Rdigits_7 => _Rdigits_6 _Rdigits_7.
		q(nt(35), (nt(34)+nt(35)));
		// _Rdigits_7 => null.
		q(nt(35), (nul));
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
		// less => ws '<' ws.
		q(nt(46), (nt(13)+t(39)+nt(13)));
		// comma => ws ',' ws.
		q(nt(47), (nt(13)+t(40)+nt(13)));
		// sym => chars.
		q(nt(48), (nt(29)));
		// _Roffsets_8 => comma offset.
		q(nt(52), (nt(47)+nt(51)));
		// _Roffsets_9 => _Roffsets_8 _Roffsets_9.
		q(nt(53), (nt(52)+nt(53)));
		// _Roffsets_9 => null.
		q(nt(53), (nul));
		// offsets => open_bracket step _Roffsets_9 close_bracket.
		q(nt(49), (nt(40)+nt(50)+nt(53)+nt(41)));
		// offset => num.
		q(nt(51), (nt(54)));
		// offset => capture.
		q(nt(51), (nt(55)));
		// offset => shift.
		q(nt(51), (nt(56)));
		// step => num.
		q(nt(50), (nt(54)));
		// step => capture.
		q(nt(50), (nt(55)));
		// step => shift.
		q(nt(50), (nt(56)));
		// shift => capture minus num.
		q(nt(56), (nt(55)+nt(44)+nt(54)));
		// num => ws digits ws.
		q(nt(54), (nt(13)+nt(33)+nt(13)));
		// variable => var.
		q(nt(57), (nt(58)));
		// variable => timed.
		q(nt(57), (nt(59)));
		// _Rtimed_10 => in.
		q(nt(62), (nt(60)));
		// _Rtimed_10 => out.
		q(nt(62), (nt(61)));
		// timed => _Rtimed_10 offsets.
		q(nt(59), (nt(62)+nt(49)));
		// capture => '$' chars.
		q(nt(55), (t(41)+nt(29)));
		// var => chars.
		q(nt(58), (nt(29)));
		// in => '?' 'i' '_' chars.
		q(nt(60), (t(42)+t(22)+t(43)+nt(29)));
		// out => '?' 'o' '_' chars.
		q(nt(61), (t(42)+t(25)+t(43)+nt(29)));
		// wff_rule => wff_matcher definition wff_body dot.
		q(nt(63), (nt(64)+nt(36)+nt(65)+nt(37)));
		// wff_rec_relation => wff_ref definition wff dot.
		q(nt(66), (nt(67)+nt(36)+nt(68)+nt(37)));
		// wff_matcher => wff_ref.
		q(nt(64), (nt(67)));
		// wff_matcher => wff.
		q(nt(64), (nt(68)));
		// wff_body => wff.
		q(nt(65), (nt(68)));
		// wff_body => bf_eq_cb.
		q(nt(65), (nt(69)));
		// wff_body => bf_neq_cb.
		q(nt(65), (nt(70)));
		// wff_body => wff_has_clashing_subformulas_cb.
		q(nt(65), (nt(71)));
		// wff_body => wff_has_subformula_cb.
		q(nt(65), (nt(72)));
		// wff_body => wff_remove_existential.
		q(nt(65), (nt(73)));
		// wff => capture.
		q(nt(68), (nt(55)));
		// wff => wff_ref.
		q(nt(68), (nt(67)));
		// wff => wff_and.
		q(nt(68), (nt(74)));
		// wff => wff_neg.
		q(nt(68), (nt(75)));
		// wff => wff_xor.
		q(nt(68), (nt(76)));
		// wff => wff_conditional.
		q(nt(68), (nt(77)));
		// wff => wff_or.
		q(nt(68), (nt(78)));
		// wff => wff_all.
		q(nt(68), (nt(79)));
		// wff => wff_ex.
		q(nt(68), (nt(80)));
		// wff => wff_imply.
		q(nt(68), (nt(81)));
		// wff => wff_equiv.
		q(nt(68), (nt(82)));
		// wff => wff_t.
		q(nt(68), (nt(83)));
		// wff => wff_f.
		q(nt(68), (nt(84)));
		// wff => bf_eq.
		q(nt(68), (nt(85)));
		// wff => bf_neq.
		q(nt(68), (nt(86)));
		// wff => bf_less.
		q(nt(68), (nt(87)));
		// wff => bf_less_equal.
		q(nt(68), (nt(88)));
		// wff => bf_greater.
		q(nt(68), (nt(89)));
		// wff_ref => sym offsets wff_ref_args.
		q(nt(67), (nt(48)+nt(49)+nt(90)));
		// _Rwff_ref_args_11 => variable.
		q(nt(91), (nt(57)));
		// _Rwff_ref_args_12 => _Rwff_ref_args_11 _Rwff_ref_args_12.
		q(nt(92), (nt(91)+nt(92)));
		// _Rwff_ref_args_12 => null.
		q(nt(92), (nul));
		// wff_ref_args => open_parenthesis _Rwff_ref_args_12 close_parenthesis.
		q(nt(90), (nt(38)+nt(92)+nt(39)));
		// wff_and => open_parenthesis wff wff_and_sym wff close_parenthesis.
		q(nt(74), (nt(38)+nt(68)+nt(93)+nt(68)+nt(39)));
		// wff_or => open_parenthesis wff wff_or_sym wff close_parenthesis.
		q(nt(78), (nt(38)+nt(68)+nt(94)+nt(68)+nt(39)));
		// wff_xor => open_parenthesis wff wff_xor_sym wff close_parenthesis.
		q(nt(76), (nt(38)+nt(68)+nt(95)+nt(68)+nt(39)));
		// wff_conditional => open_parenthesis wff wff_conditional_sym wff colon wff close_parenthesis.
		q(nt(77), (nt(38)+nt(68)+nt(96)+nt(68)+nt(45)+nt(68)+nt(39)));
		// wff_neg => wff_neg_sym wff.
		q(nt(75), (nt(97)+nt(68)));
		// wff_imply => open_parenthesis wff wff_imply_sym wff close_parenthesis.
		q(nt(81), (nt(38)+nt(68)+nt(98)+nt(68)+nt(39)));
		// wff_equiv => open_parenthesis wff wff_equiv_sym wff close_parenthesis.
		q(nt(82), (nt(38)+nt(68)+nt(99)+nt(68)+nt(39)));
		// _Rwff_all_13 => capture.
		q(nt(101), (nt(55)));
		// _Rwff_all_13 => variable.
		q(nt(101), (nt(57)));
		// wff_all => wff_all_sym _Rwff_all_13 ws_required wff.
		q(nt(79), (nt(100)+nt(101)+nt(12)+nt(68)));
		// _Rwff_ex_14 => capture.
		q(nt(103), (nt(55)));
		// _Rwff_ex_14 => variable.
		q(nt(103), (nt(57)));
		// wff_ex => wff_ex_sym ws_required _Rwff_ex_14 ws_required wff.
		q(nt(80), (nt(102)+nt(12)+nt(103)+nt(12)+nt(68)));
		// bf_eq => open_parenthesis bf bf_equality_sym bf close_parenthesis.
		q(nt(85), (nt(38)+nt(104)+nt(105)+nt(104)+nt(39)));
		// bf_neq => open_parenthesis bf bf_nequality_sym bf close_parenthesis.
		q(nt(86), (nt(38)+nt(104)+nt(106)+nt(104)+nt(39)));
		// bf_less => open_parenthesis bf bf_less_sym bf close_parenthesis.
		q(nt(87), (nt(38)+nt(104)+nt(107)+nt(104)+nt(39)));
		// bf_less_equal => open_parenthesis bf bf_less_equal_sym bf close_parenthesis.
		q(nt(88), (nt(38)+nt(104)+nt(108)+nt(104)+nt(39)));
		// bf_greater => open_parenthesis bf bf_greater_sym bf close_parenthesis.
		q(nt(89), (nt(38)+nt(104)+nt(109)+nt(104)+nt(39)));
		// wff_and_sym => ws '&' '&' ws.
		q(nt(93), (nt(13)+t(44)+t(44)+nt(13)));
		// wff_or_sym => ws '|' '|' ws.
		q(nt(94), (nt(13)+t(45)+t(45)+nt(13)));
		// wff_xor_sym => ws '^' ws.
		q(nt(95), (nt(13)+t(46)+nt(13)));
		// wff_conditional_sym => ws '?' ws.
		q(nt(96), (nt(13)+t(42)+nt(13)));
		// wff_neg_sym => ws '!' ws.
		q(nt(97), (nt(13)+t(47)+nt(13)));
		// wff_imply_sym => ws '-' '>' ws.
		q(nt(98), (nt(13)+t(38)+t(48)+nt(13)));
		// wff_equiv_sym => ws '<' '-' '>' ws.
		q(nt(99), (nt(13)+t(39)+t(38)+t(48)+nt(13)));
		// wff_all_sym => ws 'a' 'l' 'l' ws.
		q(nt(100), (nt(13)+t(10)+t(11)+t(11)+nt(13)));
		// wff_ex_sym => ws 'e' 'x' ws.
		q(nt(102), (nt(13)+t(24)+t(5)+nt(13)));
		// wff_t => ws 'T' ws.
		q(nt(83), (nt(13)+t(49)+nt(13)));
		// wff_f => ws 'F' ws.
		q(nt(84), (nt(13)+t(50)+nt(13)));
		// bf_rule => bf_matcher definition bf_body dot.
		q(nt(110), (nt(111)+nt(36)+nt(112)+nt(37)));
		// bf_rec_relation => bf_ref definition bf dot.
		q(nt(113), (nt(114)+nt(36)+nt(104)+nt(37)));
		// bf_matcher => bf.
		q(nt(111), (nt(104)));
		// bf_body => bf.
		q(nt(112), (nt(104)));
		// bf_body => bf_is_zero_cb.
		q(nt(112), (nt(115)));
		// bf_body => bf_is_one_cb.
		q(nt(112), (nt(116)));
		// bf_body => bf_has_clashing_subformulas_cb.
		q(nt(112), (nt(117)));
		// bf_body => bf_has_subformula_cb.
		q(nt(112), (nt(118)));
		// bf => capture.
		q(nt(104), (nt(55)));
		// bf => variable.
		q(nt(104), (nt(57)));
		// bf => bf_ref.
		q(nt(104), (nt(114)));
		// bf => bf_constant.
		q(nt(104), (nt(119)));
		// bf => bf_and.
		q(nt(104), (nt(120)));
		// bf => bf_neg.
		q(nt(104), (nt(121)));
		// bf => bf_xor.
		q(nt(104), (nt(122)));
		// bf => bf_or.
		q(nt(104), (nt(123)));
		// bf => bf_all.
		q(nt(104), (nt(124)));
		// bf => bf_ex.
		q(nt(104), (nt(125)));
		// bf => bf_subs_cb.
		q(nt(104), (nt(126)));
		// bf => bf_t.
		q(nt(104), (nt(127)));
		// bf => bf_f.
		q(nt(104), (nt(128)));
		// bf_ref => sym offsets bf_ref_args.
		q(nt(114), (nt(48)+nt(49)+nt(129)));
		// _Rbf_ref_args_15 => variable.
		q(nt(130), (nt(57)));
		// _Rbf_ref_args_16 => _Rbf_ref_args_15 _Rbf_ref_args_16.
		q(nt(131), (nt(130)+nt(131)));
		// _Rbf_ref_args_16 => null.
		q(nt(131), (nul));
		// bf_ref_args => open_parenthesis _Rbf_ref_args_16 close_parenthesis.
		q(nt(129), (nt(38)+nt(131)+nt(39)));
		// bf_and => open_parenthesis bf bf_and_sym bf close_parenthesis.
		q(nt(120), (nt(38)+nt(104)+nt(132)+nt(104)+nt(39)));
		// bf_or => open_parenthesis bf bf_or_sym bf close_parenthesis.
		q(nt(123), (nt(38)+nt(104)+nt(133)+nt(104)+nt(39)));
		// bf_xor => open_parenthesis bf bf_xor_sym ws bf close_parenthesis.
		q(nt(122), (nt(38)+nt(104)+nt(134)+nt(13)+nt(104)+nt(39)));
		// bf_neg => bf_neg_sym bf.
		q(nt(121), (nt(135)+nt(104)));
		// _Rbf_all_17 => capture.
		q(nt(137), (nt(55)));
		// _Rbf_all_17 => variable.
		q(nt(137), (nt(57)));
		// bf_all => bf_all_sym ws_required _Rbf_all_17 ws_required bf.
		q(nt(124), (nt(136)+nt(12)+nt(137)+nt(12)+nt(104)));
		// _Rbf_ex_18 => capture.
		q(nt(139), (nt(55)));
		// _Rbf_ex_18 => variable.
		q(nt(139), (nt(57)));
		// bf_ex => bf_ex_sym ws_required _Rbf_ex_18 ws_required bf.
		q(nt(125), (nt(138)+nt(12)+nt(139)+nt(12)+nt(104)));
		// bf_and_sym => ws '&' ws.
		q(nt(132), (nt(13)+t(44)+nt(13)));
		// bf_or_sym => ws '|' ws.
		q(nt(133), (nt(13)+t(45)+nt(13)));
		// bf_xor_sym => ws '+' ws.
		q(nt(134), (nt(13)+t(51)+nt(13)));
		// bf_neg_sym => ws '~' ws.
		q(nt(135), (nt(13)+t(52)+nt(13)));
		// bf_equality_sym => ws '=' ws.
		q(nt(105), (nt(13)+t(30)+nt(13)));
		// bf_nequality_sym => ws '!' '=' ws.
		q(nt(106), (nt(13)+t(47)+t(30)+nt(13)));
		// bf_less_sym => ws '<' ws.
		q(nt(107), (nt(13)+t(39)+nt(13)));
		// bf_less_equal_sym => ws '<' '=' ws.
		q(nt(108), (nt(13)+t(39)+t(30)+nt(13)));
		// bf_greater_sym => ws '>' ws.
		q(nt(109), (nt(13)+t(48)+nt(13)));
		// bf_all_sym => ws 'f' 'a' 'l' 'l' ws.
		q(nt(136), (nt(13)+t(26)+t(10)+t(11)+t(11)+nt(13)));
		// bf_ex_sym => ws 'f' 'e' 'x' ws.
		q(nt(138), (nt(13)+t(26)+t(24)+t(5)+nt(13)));
		// bf_t => ws '1' ws.
		q(nt(127), (nt(13)+t(53)+nt(13)));
		// bf_f => ws '0' ws.
		q(nt(128), (nt(13)+t(54)+nt(13)));
		// bf_constant => open_brace constant close_brace.
		q(nt(119), (nt(42)+nt(140)+nt(43)));
		// constant => capture.
		q(nt(140), (nt(55)));
		// constant => binding.
		q(nt(140), (nt(141)));
		// constant => bf_and_cb.
		q(nt(140), (nt(142)));
		// constant => bf_or_cb.
		q(nt(140), (nt(143)));
		// constant => bf_xor_cb.
		q(nt(140), (nt(144)));
		// constant => bf_neg_cb.
		q(nt(140), (nt(145)));
		// binding => source_binding.
		q(nt(141), (nt(146)));
		// binding => named_binding.
		q(nt(141), (nt(147)));
		// named_binding => chars.
		q(nt(147), (nt(29)));
		// source_binding => type colon source.
		q(nt(146), (nt(148)+nt(45)+nt(149)));
		// type => chars.
		q(nt(148), (nt(29)));
		// type => null.
		q(nt(148), (nul));
		// source0 => space.
		q(nt(150), (nt(2)));
		// source0 => alnum.
		q(nt(150), (nt(6)));
		// source0 => char_escape_encode.
		q(nt(150), (nt(16)));
		// source0 => char_punct.
		q(nt(150), (nt(21)));
		// _Rsource_19 => source0.
		q(nt(151), (nt(150)));
		// _Rsource_20 => _Rsource_19.
		q(nt(152), (nt(151)));
		// _Rsource_20 => _Rsource_19 _Rsource_20.
		q(nt(152), (nt(151)+nt(152)));
		// source => _Rsource_20.
		q(nt(149), (nt(152)));
		// bf_and_cb => bf_cb_arg bf_and_cb_sym bf_cb_arg.
		q(nt(142), (nt(153)+nt(154)+nt(153)));
		// bf_or_cb => bf_cb_arg bf_or_cb_sym bf_cb_arg.
		q(nt(143), (nt(153)+nt(155)+nt(153)));
		// bf_xor_cb => bf_cb_arg bf_xor_cb_sym bf_cb_arg.
		q(nt(144), (nt(153)+nt(156)+nt(153)));
		// bf_neg_cb => bf_neg_cb_sym bf_cb_arg.
		q(nt(145), (nt(157)+nt(153)));
		// bf_subs_cb => bf_subs_cb_sym bf_cb_arg ws_required bf_cb_arg ws_required bf_cb_arg.
		q(nt(126), (nt(158)+nt(153)+nt(12)+nt(153)+nt(12)+nt(153)));
		// bf_eq_cb => bf_eq_cb_sym bf_cb_arg ws_required wff_cb_arg ws_required wff_cb_arg.
		q(nt(69), (nt(159)+nt(153)+nt(12)+nt(160)+nt(12)+nt(160)));
		// bf_neq_cb => bf_neq_cb_sym bf_cb_arg ws_required wff_cb_arg ws_required wff_cb_arg.
		q(nt(70), (nt(161)+nt(153)+nt(12)+nt(160)+nt(12)+nt(160)));
		// bf_is_zero_cb => bf_is_zero_cb_sym bf_cb_arg ws_required bf_cb_arg.
		q(nt(115), (nt(162)+nt(153)+nt(12)+nt(153)));
		// bf_is_one_cb => bf_is_one_cb_sym bf_cb_arg ws_required bf_cb_arg.
		q(nt(116), (nt(163)+nt(153)+nt(12)+nt(153)));
		// bf_has_clashing_subformulas_cb => bf_has_clashing_subformulas_cb_sym bf_cb_arg ws_required bf_cb_arg.
		q(nt(117), (nt(164)+nt(153)+nt(12)+nt(153)));
		// wff_has_clashing_subformulas_cb => wff_has_clashing_subformulas_cb_sym wff_cb_arg ws_required wff_cb_arg.
		q(nt(71), (nt(165)+nt(160)+nt(12)+nt(160)));
		// bf_has_subformula_cb => bf_has_subformula_cb_sym bf_cb_arg ws_required bf_cb_arg ws_required bf_cb_arg.
		q(nt(118), (nt(166)+nt(153)+nt(12)+nt(153)+nt(12)+nt(153)));
		// wff_has_subformula_cb => wff_has_subformula_cb_sym wff_cb_arg ws_required wff_cb_arg ws_required wff_cb_arg.
		q(nt(72), (nt(167)+nt(160)+nt(12)+nt(160)+nt(12)+nt(160)));
		// wff_remove_existential => wff_remove_existential_cb_sym wff_cb_arg ws_required wff_cb_arg.
		q(nt(73), (nt(168)+nt(160)+nt(12)+nt(160)));
		// bf_cb_arg => capture.
		q(nt(153), (nt(55)));
		// bf_cb_arg => bf.
		q(nt(153), (nt(104)));
		// wff_cb_arg => capture.
		q(nt(160), (nt(55)));
		// wff_cb_arg => wff.
		q(nt(160), (nt(68)));
		// bf_and_cb_sym => ws 'b' 'f' '_' 'a' 'n' 'd' '_' 'c' 'b' ws.
		q(nt(154), (nt(13)+t(16)+t(26)+t(43)+t(10)+t(12)+t(21)+t(43)+t(18)+t(16)+nt(13)));
		// bf_or_cb_sym => ws 'b' 'f' '_' 'o' 'r' '_' 'c' 'b' ws.
		q(nt(155), (nt(13)+t(16)+t(26)+t(43)+t(25)+t(20)+t(43)+t(18)+t(16)+nt(13)));
		// bf_xor_cb_sym => ws 'b' 'f' '_' 'x' 'o' 'r' '_' 'c' 'b' ws.
		q(nt(156), (nt(13)+t(16)+t(26)+t(43)+t(5)+t(25)+t(20)+t(43)+t(18)+t(16)+nt(13)));
		// bf_neg_cb_sym => ws 'b' 'f' '_' 'n' 'e' 'g' '_' 'c' 'b' ws.
		q(nt(157), (nt(13)+t(16)+t(26)+t(43)+t(12)+t(24)+t(23)+t(43)+t(18)+t(16)+nt(13)));
		// bf_subs_cb_sym => ws 'b' 'f' '_' 's' 'u' 'b' 's' '_' 'c' 'b' ws.
		q(nt(158), (nt(13)+t(16)+t(26)+t(43)+t(28)+t(6)+t(16)+t(28)+t(43)+t(18)+t(16)+nt(13)));
		// bf_eq_cb_sym => ws 'b' 'f' '_' 'e' 'q' '_' 'c' 'b' ws.
		q(nt(159), (nt(13)+t(16)+t(26)+t(43)+t(24)+t(55)+t(43)+t(18)+t(16)+nt(13)));
		// bf_neq_cb_sym => ws 'b' 'f' '_' 'n' 'e' 'q' '_' 'c' 'b' ws.
		q(nt(161), (nt(13)+t(16)+t(26)+t(43)+t(12)+t(24)+t(55)+t(43)+t(18)+t(16)+nt(13)));
		// bf_is_zero_cb_sym => ws 'b' 'f' '_' 'i' 's' '_' 'z' 'e' 'r' 'o' '_' 'c' 'b' ws.
		q(nt(162), (nt(13)+t(16)+t(26)+t(43)+t(22)+t(28)+t(43)+t(56)+t(24)+t(20)+t(25)+t(43)+t(18)+t(16)+nt(13)));
		// bf_is_one_cb_sym => ws 'b' 'f' '_' 'i' 's' '_' 'o' 'n' 'e' '_' 'c' 'b' ws.
		q(nt(163), (nt(13)+t(16)+t(26)+t(43)+t(22)+t(28)+t(43)+t(25)+t(12)+t(24)+t(43)+t(18)+t(16)+nt(13)));
		// bf_has_clashing_subformulas_cb_sym => ws 'b' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' ws.
		q(nt(164), (nt(13)+t(16)+t(26)+t(43)+t(15)+t(10)+t(28)+t(43)+t(18)+t(11)+t(10)+t(28)+t(15)+t(22)+t(12)+t(23)+t(43)+t(28)+t(6)+t(16)+t(26)+t(25)+t(20)+t(13)+t(6)+t(11)+t(10)+t(28)+t(43)+t(18)+t(16)+nt(13)));
		// bf_has_subformula_cb_sym => ws 'b' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' ws.
		q(nt(166), (nt(13)+t(16)+t(26)+t(43)+t(15)+t(10)+t(28)+t(43)+t(28)+t(6)+t(16)+t(26)+t(25)+t(20)+t(13)+t(6)+t(11)+t(10)+t(43)+t(18)+t(16)+nt(13)));
		// wff_has_clashing_subformulas_cb_sym => ws 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 'c' 'l' 'a' 's' 'h' 'i' 'n' 'g' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' 's' '_' 'c' 'b' ws.
		q(nt(165), (nt(13)+t(27)+t(26)+t(26)+t(43)+t(15)+t(10)+t(28)+t(43)+t(18)+t(11)+t(10)+t(28)+t(15)+t(22)+t(12)+t(23)+t(43)+t(28)+t(6)+t(16)+t(26)+t(25)+t(20)+t(13)+t(6)+t(11)+t(10)+t(28)+t(43)+t(18)+t(16)+nt(13)));
		// wff_has_subformula_cb_sym => ws 'w' 'f' 'f' '_' 'h' 'a' 's' '_' 's' 'u' 'b' 'f' 'o' 'r' 'm' 'u' 'l' 'a' '_' 'c' 'b' ws.
		q(nt(167), (nt(13)+t(27)+t(26)+t(26)+t(43)+t(15)+t(10)+t(28)+t(43)+t(28)+t(6)+t(16)+t(26)+t(25)+t(20)+t(13)+t(6)+t(11)+t(10)+t(43)+t(18)+t(16)+nt(13)));
		// wff_remove_existential_cb_sym => ws 'w' 'f' 'f' '_' 'r' 'e' 'm' 'o' 'v' 'e' '_' 'e' 'x' 'i' 's' 't' 'e' 'n' 't' 'i' 'a' 'l' '_' 'c' 'b' ws.
		q(nt(168), (nt(13)+t(27)+t(26)+t(26)+t(43)+t(20)+t(24)+t(13)+t(25)+t(57)+t(24)+t(43)+t(24)+t(5)+t(22)+t(28)+t(19)+t(24)+t(12)+t(19)+t(22)+t(10)+t(11)+t(43)+t(18)+t(16)+nt(13)));
		// input => in colon open_brace source_binding close_brace.
		q(nt(169), (nt(60)+nt(45)+nt(42)+nt(146)+nt(43)));
		// _Rinputs_21 => input.
		q(nt(171), (nt(169)));
		// _Rinputs_22 => _Rinputs_21 _Rinputs_22.
		q(nt(172), (nt(171)+nt(172)));
		// _Rinputs_22 => null.
		q(nt(172), (nul));
		// inputs => less input _Rinputs_22 dot.
		q(nt(170), (nt(46)+nt(169)+nt(172)+nt(37)));
		// main => wff dot.
		q(nt(173), (nt(68)+nt(37)));
		// rule => wff_rule.
		q(nt(174), (nt(63)));
		// rule => bf_rule.
		q(nt(174), (nt(110)));
		// _Rrules_23 => rule.
		q(nt(176), (nt(174)));
		// _Rrules_24 => _Rrules_23 _Rrules_24.
		q(nt(177), (nt(176)+nt(177)));
		// _Rrules_24 => null.
		q(nt(177), (nul));
		// rules => _Rrules_24.
		q(nt(175), (nt(177)));
		// rec_relation => wff_rec_relation dot.
		q(nt(178), (nt(66)+nt(37)));
		// rec_relation => bf_rec_relation.
		q(nt(178), (nt(113)));
		// _Rrec_relations_25 => rec_relation.
		q(nt(180), (nt(178)));
		// _Rrec_relations_26 => _Rrec_relations_25 _Rrec_relations_26.
		q(nt(181), (nt(180)+nt(181)));
		// _Rrec_relations_26 => null.
		q(nt(181), (nul));
		// rec_relations => _Rrec_relations_26.
		q(nt(179), (nt(181)));
		// formula => rec_relations main.
		q(nt(182), (nt(179)+nt(173)));
		// library => rules.
		q(nt(183), (nt(175)));
		// builder => builder_head definition builder_body dot.
		q(nt(184), (nt(185)+nt(36)+nt(186)+nt(37)));
		// _Rbuilder_head_27 => ws_required capture.
		q(nt(187), (nt(12)+nt(55)));
		// _Rbuilder_head_28 => _Rbuilder_head_27 _Rbuilder_head_28.
		q(nt(188), (nt(187)+nt(188)));
		// _Rbuilder_head_28 => null.
		q(nt(188), (nul));
		// builder_head => open_parenthesis capture _Rbuilder_head_28 close_parenthesis.
		q(nt(185), (nt(38)+nt(55)+nt(188)+nt(39)));
		// builder_body => wff.
		q(nt(186), (nt(68)));
		// builder_body => bf.
		q(nt(186), (nt(104)));
		// start => inputs.
		q(nt(189), (nt(170)));
		// start => formula.
		q(nt(189), (nt(182)));
		// start => library.
		q(nt(189), (nt(183)));
		// start => builder.
		q(nt(189), (nt(184)));
		return q;
	}
};
#endif // __TAU_PARSER_H__
