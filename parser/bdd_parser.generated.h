// This file is generated from a file parser/bdd.tgf by
//       https://github.com/IDNI/parser/tools/tgf
//
#ifndef __BDD_PARSER_H__
#define __BDD_PARSER_H__
#include <string.h>
#include "parser.h"
struct bdd_parser {
	bdd_parser() :
		nts(load_nonterminals()), cc(load_cc()),
		g(nts, load_prods(), nt(4), cc), p(g, load_opts()) {}
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
		nul, space, alpha, alnum, start, ws, bdd, ws_required, disjunction, bdd2,
		conjunction, bdd3, exclusive_or, bdd4, negation, bdd5, literal, _Rexclusive_or_0, var, T,
		F, _Rvar_1, _Rvar_2,
	};
	size_t id(const std::basic_string<char>& name) { return nts.get(name); }
private:
	std::vector<char> ts{
		'\0', '(', ')', '|', '&', '+', '^', '\'', '1',
		'0',
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
			"", "space", "alpha", "alnum", "start", "ws", "bdd", "ws_required", "disjunction", "bdd2",
			"conjunction", "bdd3", "exclusive_or", "bdd4", "negation", "bdd5", "literal", "_Rexclusive_or_0", "var", "T",
			"F", "_Rvar_1", "_Rvar_2",
		}) nts.get(nt);
		return nts;
	}
	idni::char_class_fns<char> load_cc() {
		return idni::predefined_char_classes<char, char>({
			"space",
			"alpha",
			"alnum",
		}, nts);
	}
	idni::parser<char, char>::options load_opts() {
		idni::parser<char, char>::options o;
		return o;
	}
	idni::prods<char, char> load_prods() {
		idni::prods<char, char> q, nul(idni::lit<char, char>{});
		// start => ws bdd ws.
		q(nt(4), (nt(5)+nt(6)+nt(5)));
		// ws_required => space ws.
		q(nt(7), (nt(1)+nt(5)));
		// ws => null.
		q(nt(5), (nul));
		// ws => ws_required.
		q(nt(5), (nt(7)));
		// bdd => disjunction.
		q(nt(6), (nt(8)));
		// bdd => bdd2.
		q(nt(6), (nt(9)));
		// bdd2 => conjunction.
		q(nt(9), (nt(10)));
		// bdd2 => bdd3.
		q(nt(9), (nt(11)));
		// bdd3 => exclusive_or.
		q(nt(11), (nt(12)));
		// bdd3 => bdd4.
		q(nt(11), (nt(13)));
		// bdd4 => negation.
		q(nt(13), (nt(14)));
		// bdd4 => bdd5.
		q(nt(13), (nt(15)));
		// bdd5 => '(' ws bdd ws ')'.
		q(nt(15), (t(1)+nt(5)+nt(6)+nt(5)+t(2)));
		// bdd5 => literal.
		q(nt(15), (nt(16)));
		// disjunction => bdd ws '|' ws bdd2.
		q(nt(8), (nt(6)+nt(5)+t(3)+nt(5)+nt(9)));
		// conjunction => bdd2 ws '&' ws bdd3.
		q(nt(10), (nt(9)+nt(5)+t(4)+nt(5)+nt(11)));
		// conjunction => bdd2 ws_required bdd3.
		q(nt(10), (nt(9)+nt(7)+nt(11)));
		// _Rexclusive_or_0 => '+'.
		q(nt(17), (t(5)));
		// _Rexclusive_or_0 => '^'.
		q(nt(17), (t(6)));
		// exclusive_or => bdd3 ws _Rexclusive_or_0 ws bdd4.
		q(nt(12), (nt(11)+nt(5)+nt(17)+nt(5)+nt(13)));
		// negation => bdd5 ws '\''.
		q(nt(14), (nt(15)+nt(5)+t(7)));
		// literal => var.
		q(nt(16), (nt(18)));
		// literal => T.
		q(nt(16), (nt(19)));
		// literal => F.
		q(nt(16), (nt(20)));
		// _Rvar_1 => alnum.
		q(nt(21), (nt(3)));
		// _Rvar_2 => null.
		q(nt(22), (nul));
		// _Rvar_2 => _Rvar_1 _Rvar_2.
		q(nt(22), (nt(21)+nt(22)));
		// var => alpha _Rvar_2.
		q(nt(18), (nt(2)+nt(22)));
		// T => '1'.
		q(nt(19), (t(8)));
		// F => '0'.
		q(nt(20), (t(9)));
		return q;
	}
};
#endif // __BDD_PARSER_H__
