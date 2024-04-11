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
	using sptree_type     = parser_type::psptree;
	using input_type      = parser_type::input;
	using decoder_type    = parser_type::input::decoder_type;
	using encoder_type    = std::function<std::basic_string<char_type>(
			const std::vector<terminal_type>&)>;
	bdd_parser() :
		nts(load_nonterminals()), cc(load_cc()),
		g(nts, load_prods(), nt(6), cc, load_grammar_opts()),
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
		nul, space, alpha, alnum, _, __, start, bdd, __E___0, disjunction, 
		bdd2, conjunction, bdd3, exclusive_or, bdd4, negation, bdd5, lit, __E_exclusive_or_1, var, 
		T, F, __E_var_2, __E_var_3, 
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
		'\0', '(', ')', '|', '&', '^', '+', '\'', '1', 
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
			"", "space", "alpha", "alnum", "_", "__", "start", "bdd", "__E___0", "disjunction", 
			"bdd2", "conjunction", "bdd3", "exclusive_or", "bdd4", "negation", "bdd5", "lit", "__E_exclusive_or_1", "var", 
			"T", "F", "__E_var_2", "__E_var_3", 
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
	grammar_type::options load_grammar_opts() {
		grammar_type::options o;
		o.transform_negation = false;
		o.trim_terminals = false;
		o.inline_char_classes = true;
		o.auto_disambiguate = true;
		o.to_trim = { 4, 5 };
		return o;
	}
	options load_opts() {
		options o;
		return o;
	}
	idni::prods<char_type, terminal_type> load_prods() {
		idni::prods<char_type, terminal_type> q,
			nul(symbol_type{});
		//       start(6)             => _(4) bdd(7) _(4).
		q(nt(6), (nt(4)+nt(7)+nt(4)));
		//       __(5)                => space(1) _(4).
		q(nt(5), (nt(1)+nt(4)));
		//       __E___0(8)           => __(5).
		q(nt(8), (nt(5)));
		//       __E___0(8)           => null.
		q(nt(8), (nul));
		//       _(4)                 => __E___0(8).
		q(nt(4), (nt(8)));
		//       bdd(7)               => disjunction(9).
		q(nt(7), (nt(9)));
		//       bdd(7)               => bdd2(10).
		q(nt(7), (nt(10)));
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
		//       bdd5(16)             => lit(17).
		q(nt(16), (nt(17)));
		//       bdd5(16)             => '(' _(4) bdd(7) _(4) ')'.
		q(nt(16), (t(1)+nt(4)+nt(7)+nt(4)+t(2)));
		//       disjunction(9)       => bdd(7) _(4) '|' _(4) bdd2(10).
		q(nt(9), (nt(7)+nt(4)+t(3)+nt(4)+nt(10)));
		//       conjunction(11)      => bdd2(10) _(4) '&' _(4) bdd3(12).
		q(nt(11), (nt(10)+nt(4)+t(4)+nt(4)+nt(12)));
		//       conjunction(11)      => bdd2(10) __(5) bdd3(12).
		q(nt(11), (nt(10)+nt(5)+nt(12)));
		//       __E_exclusive_or_1(18) => '^'.
		q(nt(18), (t(5)));
		//       __E_exclusive_or_1(18) => '+'.
		q(nt(18), (t(6)));
		//       exclusive_or(13)     => bdd3(12) _(4) __E_exclusive_or_1(18) _(4) bdd4(14).
		q(nt(13), (nt(12)+nt(4)+nt(18)+nt(4)+nt(14)));
		//       negation(15)         => bdd4(14) _(4) '\''.
		q(nt(15), (nt(14)+nt(4)+t(7)));
		//       lit(17)              => var(19).
		q(nt(17), (nt(19)));
		//       lit(17)              => T(20).
		q(nt(17), (nt(20)));
		//       lit(17)              => F(21).
		q(nt(17), (nt(21)));
		//       __E_var_2(22)        => alnum(3).
		q(nt(22), (nt(3)));
		//       __E_var_3(23)        => null.
		q(nt(23), (nul));
		//       __E_var_3(23)        => __E_var_2(22).
		q(nt(23), (nt(22)));
		//       __E_var_3(23)        => __E_var_2(22) __E_var_3(23).
		q(nt(23), (nt(22)+nt(23)));
		//       var(19)              => alpha(2) __E_var_3(23).
		q(nt(19), (nt(2)+nt(23)));
		//       T(20)                => '1'.
		q(nt(20), (t(8)));
		//       F(21)                => '0'.
		q(nt(21), (t(9)));
		return q;
	}
};

#endif // __BDD_PARSER_H__
