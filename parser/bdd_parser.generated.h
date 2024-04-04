// This file is generated from a file parser/bdd.tgf by
//       https://github.com/IDNI/parser/tools/tgf
//
#ifndef __BDD_PARSER_H__
#define __BDD_PARSER_H__

#include <string.h>

#include "parser.h"

struct bdd_parser {
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
	using input_type      = parser_type::input;
	using decoder_type    = parser_type::input::decoder_type;
	using encoder_type    = std::function<std::basic_string<char_type>(
			const std::vector<terminal_type>&)>;
	bdd_parser() :
		nts(load_nonterminals()), cc(load_cc()),
		g(nts, load_prods(), nt(4), cc, load_grammar_opts()),
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
	bool found(size_t start = SIZE_MAX) { return p.found(start); }
	typename parser_type::error get_error() { return p.get_error(); }
	enum nonterminal {
		nul, alnum, alpha, space, start, _, bdd, __, _R__0, disjunction, 
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
			"", "alnum", "alpha", "space", "start", "_", "bdd", "__", "_R__0", "disjunction", 
			"bdd2", "conjunction", "bdd3", "exclusive_or", "bdd4", "negation", "bdd5", "literal", "_Rexclusive_or_1", "var", 
			"T", "F", "_Rvar_2", "_Rvar_3", 
		}) nts.get(nt);
		return nts;
	}
	idni::char_class_fns<terminal_type> load_cc() {
		return idni::predefined_char_classes<char_type, terminal_type>({
			"alnum",
			"alpha",
			"space",
		}, nts);
	}
	grammar_type::options load_grammar_opts() {
		grammar_type::options o;
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
		//       start(4)             => _(5) bdd(6) _(5).
		q(nt(4), (nt(5)+nt(6)+nt(5)));
		//       __(7)                => space(3) _(5).
		q(nt(7), (nt(3)+nt(5)));
		//       _R__0(8)             => null.
		q(nt(8), (nul));
		//       _R__0(8)             => __(7).
		q(nt(8), (nt(7)));
		//       _(5)                 => _R__0(8).
		q(nt(5), (nt(8)));
		//       bdd(6)               => disjunction(9).
		q(nt(6), (nt(9)));
		//       bdd(6)               => bdd2(10).
		q(nt(6), (nt(10)));
		//       bdd2(10)             => conjunction(11).
		q(nt(10), (nt(11)));
		//       bdd2(10)             => bdd3(12).
		q(nt(10), (nt(12)));
		//       bdd3(12)             => exclusive_or(13).
		q(nt(12), (nt(13)));
		//       bdd3(12)             => bdd4(14).
		q(nt(12), (nt(14)));
		//       bdd4(14)             => negation(15).
		q(nt(14), (nt(15)));
		//       bdd4(14)             => bdd5(16).
		q(nt(14), (nt(16)));
		//       bdd5(16)             => '(' _(5) bdd(6) _(5) ')'.
		q(nt(16), (t(1)+nt(5)+nt(6)+nt(5)+t(2)));
		//       bdd5(16)             => literal(17).
		q(nt(16), (nt(17)));
		//       disjunction(9)       => bdd(6) _(5) '|' _(5) bdd2(10).
		q(nt(9), (nt(6)+nt(5)+t(3)+nt(5)+nt(10)));
		//       conjunction(11)      => bdd2(10) _(5) '&' _(5) bdd3(12).
		q(nt(11), (nt(10)+nt(5)+t(4)+nt(5)+nt(12)));
		//       conjunction(11)      => bdd2(10) __(7) bdd3(12).
		q(nt(11), (nt(10)+nt(7)+nt(12)));
		//       _Rexclusive_or_1(18) => '+'.
		q(nt(18), (t(5)));
		//       _Rexclusive_or_1(18) => '^'.
		q(nt(18), (t(6)));
		//       exclusive_or(13)     => bdd3(12) _(5) _Rexclusive_or_1(18) _(5) bdd4(14).
		q(nt(13), (nt(12)+nt(5)+nt(18)+nt(5)+nt(14)));
		//       negation(15)         => bdd4(14) _(5) '\''.
		q(nt(15), (nt(14)+nt(5)+t(7)));
		//       literal(17)          => var(19).
		q(nt(17), (nt(19)));
		//       literal(17)          => T(20).
		q(nt(17), (nt(20)));
		//       literal(17)          => F(21).
		q(nt(17), (nt(21)));
		//       _Rvar_2(22)          => alnum(1).
		q(nt(22), (nt(1)));
		//       _Rvar_3(23)          => null.
		q(nt(23), (nul));
		//       _Rvar_3(23)          => _Rvar_2(22) _Rvar_3(23).
		q(nt(23), (nt(22)+nt(23)));
		//       var(19)              => alpha(2) _Rvar_3(23).
		q(nt(19), (nt(2)+nt(23)));
		//       T(20)                => '1'.
		q(nt(20), (t(8)));
		//       F(21)                => '0'.
		q(nt(21), (t(9)));
		return q;
	}
};

#endif // __BDD_PARSER_H__
