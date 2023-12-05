// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#include <iostream>

#include "normalizer2.h"

using namespace std;
using namespace idni::tau;

namespace idni::tau {

template<typename...BAs>
struct tau {

	tau(const formula<BAs...>& f) : form(f) {}

	bool is_zero() const {
		return (normalizer<BAs...>(form).main | tau_parser::wff_f).has_value();
	}

	bool is_one() const {
		return (normalizer<BAs...>(form).main | tau_parser::wff_t).has_value();
	}

	tau<BAs...> operator<=>(const tau<BAs...>& that) {
			if (auto cmp = form.main <=> that.main; cmp != 0) { return cmp; }
			return std::lexicographical_compare_three_way(
				form.rec_relations.begin(), form.rec_relations.end(),
				that.form.rec_relations.begin(), that.form.rec_relations.end());
	}

	// the formula to be executed
	formula<BAs...> form;
};

template<typename...BAs>
tau<BAs...> operator&(const tau<BAs...>& l, const tau<BAs...>& r) {
	return tau<BAs...>({{}, build_wff_and<BAs...>(l.form.main, r.form.main)});
}

template<typename...BAs>
tau<BAs...> operator|(const tau<BAs...>& l, const tau<BAs...>& r) {
	return tau<BAs...>({{}, build_wff_or<BAs...>(l.form.main, r.form.main)});
}

template<typename...BAs>
tau<BAs...> operator^(const tau<BAs...>& l, const tau<BAs...>& r) {
	return tau<BAs...>({{}, build_wff_xor<BAs...>(l.form.main, r.form.main)});
}

template<typename...BAs>
tau<BAs...> operator+(const tau<BAs...>& l, const tau<BAs...>& r) {
	return tau<BAs...>({{}, build_wff_xor<BAs...>(l.form.main, r.form.main)});
}

template<typename...BAs>
tau<BAs...> operator~(const tau<BAs...>& t) {
	return tau<BAs...>({{}, build_wff_not<BAs...>(t.form.main)});
}

template<typename...BAs>
struct tau_factory {

	sp_tau_node<tau<BAs...>> build(const std::string type_name, const sp_tau_node<tau<BAs...>>& n) {
		if (type_name != "tau") return n;
		std::string var = make_string_with_skip<
			tau_node_terminal_extractor_t<tau<BAs...>>,
			not_whitespace_predicate_t<tau<BAs...>>,
			sp_tau_node<tau<BAs...>>>(
				tau_node_terminal_extractor<tau<BAs...>>,
				not_whitespace_predicate<tau<BAs...>>, n);
		// TODO (MEDIUM) use a cache as we have a proper order for tau struct
		return make_formula_using_factory<tau_factory<BAs...>, tau<BAs...>>(var, *this);
	}
};



} // namespace idni::tau