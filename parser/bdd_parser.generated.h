// This file is generated from a file /home/klip/idni/tau-lang.devel/parser/bdd.tgfby 
//       https://github.com/IDNI/parser/tools/tgf
//
#ifndef __BDD_PARSER_H__
#define __BDD_PARSER_H__
#include <string.h>
#include "parser.h"
struct bdd_parser {
	bdd_parser() :
		nts(load_nonterminals()), cc(load_cc()),
		g(nts, load_prods(), nt(60), cc), p(g, load_opts()) {}
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
		chars_4, chars_5, char_class, equality, nequality, dot, open_parenthesis, close_parenthesis, open_bracket, close_bracket, 
		open_brace, close_brace, less_than, greater_than, minus, plus, colon, var, bdd, bdd_and, 
		bdd_neg, bdd_xor, bdd_or, T, F, bdd_arg, bdd_and_sym, bdd_or_sym, bdd_xor_sym, bdd_neg_sym, 
		start, __neg_0, __neg_1, __neg_2, __neg_3, __neg_4, __neg_5, 
	};
	size_t id(const std::basic_string<char>& name) { return nts.get(name); }
private:
	std::vector<char> ts{
		'\0', '\n', '\r', '#', '\\', 'x', 'u', '\'', '"', 
		'`', 'a', 'l', 'n', 'm', 'p', 'h', 'b', 'k', 'c', 
		't', 'r', 'd', 'i', 'g', 'e', 'o', 'f', 'w', 's', 
		'=', '!', '.', '(', ')', '[', ']', '{', '}', '<', 
		'>', '-', '+', ':', '?', '_', '1', '0', 
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
			"chars_4", "chars_5", "char_class", "equality", "nequality", "dot", "open_parenthesis", "close_parenthesis", "open_bracket", "close_bracket", 
			"open_brace", "close_brace", "less_than", "greater_than", "minus", "plus", "colon", "var", "bdd", "bdd_and", 
			"bdd_neg", "bdd_xor", "bdd_or", "T", "F", "bdd_arg", "bdd_and_sym", "bdd_or_sym", "bdd_xor_sym", "bdd_neg_sym", 
			"start", "__neg_0", "__neg_1", "__neg_2", "__neg_3", "__neg_4", "__neg_5", 
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
		q(nt(61), (nt(18)));
		// __neg_1 => q_str.
		q(nt(62), (nt(19)));
		// __neg_2 => q_bqstr.
		q(nt(63), (nt(20)));
		// __neg_3 => char_punct_1.
		q(nt(64), (nt(22)));
		// __neg_4 => char_punct_2.
		q(nt(65), (nt(23)));
		// __neg_5 => char_punct_3.
		q(nt(66), (nt(24)));
		// char_punct => punct & ~( __neg_0 ) & ~( __neg_1 ) & ~( __neg_2 ) & ~( __neg_3 ) & ~( __neg_4 ) & ~( __neg_5 ).
		q(nt(21), (nt(7)) & ~(nt(61)) & ~(nt(62)) & ~(nt(63)) & ~(nt(64)) & ~(nt(65)) & ~(nt(66)));
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
		// equality => ws '=' ws.
		q(nt(33), (nt(13)+t(29)+nt(13)));
		// nequality => ws '!' '=' ws.
		q(nt(34), (nt(13)+t(30)+t(29)+nt(13)));
		// dot => ws '.' ws.
		q(nt(35), (nt(13)+t(31)+nt(13)));
		// open_parenthesis => ws '(' ws.
		q(nt(36), (nt(13)+t(32)+nt(13)));
		// close_parenthesis => ws ')' ws.
		q(nt(37), (nt(13)+t(33)+nt(13)));
		// open_bracket => ws '[' ws.
		q(nt(38), (nt(13)+t(34)+nt(13)));
		// close_bracket => ws ']' ws.
		q(nt(39), (nt(13)+t(35)+nt(13)));
		// open_brace => ws '{' ws.
		q(nt(40), (nt(13)+t(36)+nt(13)));
		// close_brace => ws '}' ws.
		q(nt(41), (nt(13)+t(37)+nt(13)));
		// less_than => ws '<' ws.
		q(nt(42), (nt(13)+t(38)+nt(13)));
		// greater_than => ws '>' ws.
		q(nt(43), (nt(13)+t(39)+nt(13)));
		// minus => ws '-' ws.
		q(nt(44), (nt(13)+t(40)+nt(13)));
		// plus => ws '+' ws.
		q(nt(45), (nt(13)+t(41)+nt(13)));
		// colon => ws ':' ws.
		q(nt(46), (nt(13)+t(42)+nt(13)));
		// var => '?' chars.
		q(nt(47), (t(43)+nt(29)));
		// bdd => var.
		q(nt(48), (nt(47)));
		// bdd => bdd_and.
		q(nt(48), (nt(49)));
		// bdd => bdd_neg.
		q(nt(48), (nt(50)));
		// bdd => bdd_xor.
		q(nt(48), (nt(51)));
		// bdd => bdd_or.
		q(nt(48), (nt(52)));
		// bdd => T.
		q(nt(48), (nt(53)));
		// bdd => F.
		q(nt(48), (nt(54)));
		// bdd_and => open_parenthesis bdd_arg bdd_and_sym bdd_arg close_parenthesis.
		q(nt(49), (nt(36)+nt(55)+nt(56)+nt(55)+nt(37)));
		// bdd_or => open_parenthesis bdd_arg bdd_or_sym bdd_arg close_parenthesis.
		q(nt(52), (nt(36)+nt(55)+nt(57)+nt(55)+nt(37)));
		// bdd_xor => open_parenthesis bdd_arg bdd_xor_sym bdd_arg close_parenthesis.
		q(nt(51), (nt(36)+nt(55)+nt(58)+nt(55)+nt(37)));
		// bdd_neg => bdd_neg_sym open_parenthesis bdd_arg close_parenthesis.
		q(nt(50), (nt(59)+nt(36)+nt(55)+nt(37)));
		// bdd_arg => bdd.
		q(nt(55), (nt(48)));
		// bdd_and_sym => ws 'b' 'd' 'd' '_' 'a' 'n' 'd' ws.
		q(nt(56), (nt(13)+t(16)+t(21)+t(21)+t(44)+t(10)+t(12)+t(21)+nt(13)));
		// bdd_or_sym => ws 'b' 'd' 'd' '_' 'o' 'r' ws.
		q(nt(57), (nt(13)+t(16)+t(21)+t(21)+t(44)+t(25)+t(20)+nt(13)));
		// bdd_xor_sym => ws 'b' 'd' 'd' '_' 'x' 'o' 'r' ws.
		q(nt(58), (nt(13)+t(16)+t(21)+t(21)+t(44)+t(5)+t(25)+t(20)+nt(13)));
		// bdd_neg_sym => ws 'b' 'd' 'd' '_' 'n' 'e' 'g' ws.
		q(nt(59), (nt(13)+t(16)+t(21)+t(21)+t(44)+t(12)+t(24)+t(23)+nt(13)));
		// T => ws '1' ws.
		q(nt(53), (nt(13)+t(45)+nt(13)));
		// F => ws '0' ws.
		q(nt(54), (nt(13)+t(46)+nt(13)));
		// start => bdd.
		q(nt(60), (nt(48)));
		return q;
	}
};
#endif // __BDD_PARSER_H__
