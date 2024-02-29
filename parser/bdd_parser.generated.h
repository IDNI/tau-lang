// This file is generated from a file parser/bdd.tgf by
//       https://github.com/IDNI/parser/tools/tgf
//
#ifndef __BDD_PARSER_H__
#define __BDD_PARSER_H__
#include <string.h>
#include "parser.h"
struct bdd_parser {
	using char_type     = char;
	using terminal_type = char;
	using traits_type   = std::char_traits<char_type>;
	using int_type      = typename traits_type::int_type;
	using symbol_type   = idni::lit<char_type, terminal_type>;
	using location_type = std::array<size_t, 2>;
	using node_type     = std::pair<symbol_type, location_type>;
	using parser_type   = idni::parser<char_type, terminal_type>;
	using options       = parser_type::options;
	using parse_options = parser_type::parse_options;
	using forest_type   = parser_type::pforest;
	using input_type    = parser_type::input;
	using decoder_type  = parser_type::input::decoder_type;
	using encoder_type  = std::function<std::basic_string<char_type>(
			const std::vector<terminal_type>&)>;
	bdd_parser() :
		nts(load_nonterminals()), cc(load_cc()),
		g(nts, load_prods(), nt(4), cc), p(g, load_opts()) {}
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
	bool found(int start = -1) { return p.found(start); }
	typename parser_type::error get_error() { return p.get_error(); }
	enum nonterminal {
		nul, space, alpha, alnum, start, _, bdd, __, _R__0, disjunction, 
		bdd2, conjunction, bdd3, exclusive_or, bdd4, negation, bdd5, literal, _Rexclusive_or_1, var, 
		T, F, _Rvar_2, _Rvar_3, 
	};
	size_t id(const std::basic_string<char_type>& name) {
		return nts.get(name);
	}
	const std::basic_string<char_type>& name(size_t id) {
		return nts.get(id);
	}
private:
	std::vector<terminal_type> ts{
		'\0', '(', ')', '|', '&', '+', '^', '\'', '1', 
		'0', 
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
			"", "space", "alpha", "alnum", "start", "_", "bdd", "__", "_R__0", "disjunction", 
			"bdd2", "conjunction", "bdd3", "exclusive_or", "bdd4", "negation", "bdd5", "literal", "_Rexclusive_or_1", "var", 
			"T", "F", "_Rvar_2", "_Rvar_3", 
		}) nts.get(nt);
		return nts;
	}
	idni::char_class_fns<terminal_type> load_cc() {
		return idni::predefined_char_classes<char_type, terminal_type>({
			"space",
			"alpha",
			"alnum",
		}, nts);
	}
	options load_opts() {
		options o;
		return o;
	}
	idni::prods<char_type, terminal_type> load_prods() {
		idni::prods<char_type, terminal_type> q,
			nul(symbol_type{});
		// start => _ bdd _.
		q(nt(4), (nt(5)+nt(6)+nt(5)));
		// __ => space _.
		q(nt(7), (nt(1)+nt(5)));
		// _R__0 => null.
		q(nt(8), (nul));
		// _R__0 => __.
		q(nt(8), (nt(7)));
		// _ => _R__0.
		q(nt(5), (nt(8)));
		// bdd => disjunction.
		q(nt(6), (nt(9)));
		// bdd => bdd2.
		q(nt(6), (nt(10)));
		// bdd2 => conjunction.
		q(nt(10), (nt(11)));
		// bdd2 => bdd3.
		q(nt(10), (nt(12)));
		// bdd3 => exclusive_or.
		q(nt(12), (nt(13)));
		// bdd3 => bdd4.
		q(nt(12), (nt(14)));
		// bdd4 => negation.
		q(nt(14), (nt(15)));
		// bdd4 => bdd5.
		q(nt(14), (nt(16)));
		// bdd5 => '(' _ bdd _ ')'.
		q(nt(16), (t(1)+nt(5)+nt(6)+nt(5)+t(2)));
		// bdd5 => literal.
		q(nt(16), (nt(17)));
		// disjunction => bdd _ '|' _ bdd2.
		q(nt(9), (nt(6)+nt(5)+t(3)+nt(5)+nt(10)));
		// conjunction => bdd2 _ '&' _ bdd3.
		q(nt(11), (nt(10)+nt(5)+t(4)+nt(5)+nt(12)));
		// conjunction => bdd2 __ bdd3.
		q(nt(11), (nt(10)+nt(7)+nt(12)));
		// _Rexclusive_or_1 => '+'.
		q(nt(18), (t(5)));
		// _Rexclusive_or_1 => '^'.
		q(nt(18), (t(6)));
		// exclusive_or => bdd3 _ _Rexclusive_or_1 _ bdd4.
		q(nt(13), (nt(12)+nt(5)+nt(18)+nt(5)+nt(14)));
		// negation => bdd4 _ '\''.
		q(nt(15), (nt(14)+nt(5)+t(7)));
		// literal => var.
		q(nt(17), (nt(19)));
		// literal => T.
		q(nt(17), (nt(20)));
		// literal => F.
		q(nt(17), (nt(21)));
		// _Rvar_2 => alnum.
		q(nt(22), (nt(3)));
		// _Rvar_3 => null.
		q(nt(23), (nul));
		// _Rvar_3 => _Rvar_2 _Rvar_3.
		q(nt(23), (nt(22)+nt(23)));
		// var => alpha _Rvar_3.
		q(nt(19), (nt(2)+nt(23)));
		// T => '1'.
		q(nt(20), (t(8)));
		// F => '0'.
		q(nt(21), (t(9)));
		return q;
	}
};
#endif // __BDD_PARSER_H__
