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

#ifndef __TAU_H__
#define __TAU_H__

#include <iostream>

#include "normalizer.h"
#include "splitter.h"

using namespace std;
using namespace idni::tau;

// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of sp_tau_node,...

namespace idni::tau {

// Check https://gcc.gnu.org/bugzilla/show_bug.cgi?id=102609 to follow up on
// the implementation of "Deducing this" on gcc.
// See also (https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0847r7.html)
// and https://devblogs.microsoft.com/cppblog/cpp23-deducing-this/ for how to use
// "Deducing this" on CRTP.

template<typename...BAs>
struct tau_ba;

template<typename...BAs>
bool is_satisfiable(const rr<nso<tau_ba<BAs...>, BAs...>>& tau_spec);

template<typename...BAs>
struct tau_ba {

	tau_ba(rules<nso<tau_ba<BAs...>, BAs...>>& rec_relations, nso<tau_ba<BAs...>, BAs...>& main) : nso_rr({rec_relations, main}) {}
	tau_ba(nso<tau_ba<BAs...>, BAs...>& main) : nso_rr({main}) {}

	auto operator<=>(const tau_ba<BAs...>&) const = default;

	tau_ba<BAs...> operator~() const {
		// TODO (HIGH) replace by ...tau... in the future
		nso<tau_ba<BAs...>, BAs...> nmain = build_wff_neg<tau_ba<BAs...>, BAs...>(nso_rr.main);
		auto nrec_relations = nso_rr.rec_relations;
		return tau_ba<BAs...>(nrec_relations, nmain);
	}

	tau_ba<BAs...> operator&(const tau_ba<BAs...>& other) const {
		// TODO (HIGH) replace by ...tau... in the future
		nso<tau_ba<BAs...>, BAs...> nmain = build_wff_and<tau_ba<BAs...>, BAs...>(nso_rr.main, other.nso_rr.main);
		rules<nso<tau_ba<BAs...>, BAs...>>  nrec_relations = merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
		return tau_ba<BAs...>(nrec_relations, nmain);
	}

	tau_ba<BAs...> operator|(const tau_ba<BAs...>& other) const {
		// TODO (HIGH) replace by ...tau... in the future
		nso<tau_ba<BAs...>, BAs...> nmain = build_wff_or<tau_ba<BAs...>, BAs...>(nso_rr.main, other.nso_rr.main);
		rules<nso<tau_ba<BAs...>, BAs...>>  nrec_relations = merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
		tau_ba<BAs...> nnso_rr(nrec_relations, nmain);
		return nnso_rr;
	}

	tau_ba<BAs...> operator+(const tau_ba<BAs...>& other) const {
		// TODO (HIGH) replace by ...tau... in the future
		nso<tau_ba<BAs...>, BAs...> nmain = build_wff_xor_from_def<tau_ba<BAs...>, BAs...>(nso_rr.main, other.nso_rr.main);
		rules<nso<tau_ba<BAs...>, BAs...>>  nrec_relations = merge(nso_rr.rec_relations, other.nso_rr.rec_relations);
		return tau_ba<BAs...>(nrec_relations, nmain);
	}

	tau_ba<BAs...> operator^(const tau_ba<BAs...>& other) const {
		return *this + other;
	}

	bool is_zero() const {
		// TODO (HIGH) replace by satisfability in the future
		auto vars = get_free_vars_from_nso(nso_rr.main);
		auto wff = nso_rr.main;
		for(auto& v: vars) wff = build_wff_all<tau_ba<BAs...>, BAs...>(v, wff);
		auto nnso_rr = rr<nso<tau_ba<BAs...>, BAs...>>(nso_rr.rec_relations, wff);
		auto normalized = normalizer<tau_ba<BAs...>, BAs...>(nnso_rr);
		auto check = normalized | tau_parser::wff_f;
		return check.has_value();
	}

	bool is_one() const {
		// TODO (HIGH) replace by satisfability in the future
		auto vars = get_free_vars_from_nso(nso_rr.main);
		auto wff = build_wff_neg(nso_rr.main);
		for(auto& v: vars) wff = build_wff_all<tau_ba<BAs...>, BAs...>(v, wff);
		auto nnso_rr = rr<nso<tau_ba<BAs...>, BAs...>>(nso_rr.rec_relations, wff);
		auto normalized = normalizer<tau_ba<BAs...>, BAs...>(nnso_rr);
		auto check = normalized | tau_parser::wff_f;
		return check.has_value();
	}

	// the type is ewquivalent to tau_spec<BAs...>
	const rr<nso<tau_ba<BAs...>, BAs...>> nso_rr;

	private:

	nso<tau_ba<BAs...>, BAs...> rename(const nso<tau_ba<BAs...>, BAs...>& form) const  {
		// TODO (MEDIUM) implement properly
		return form;
	}

	rule<rr<nso<tau_ba<BAs...>, BAs...>>> rename(const rule<nso<tau_ba<BAs...>, BAs...>>& rule) const {
		// TODO (MEDIUM) implement properly
		return rule;
	}

	rules<nso<tau_ba<BAs...>, BAs...>> merge(const rules<nso<tau_ba<BAs...>, BAs...>>& rs1, const rules<nso<tau_ba<BAs...>, BAs...>>& rs2) const {
		// TODO (MEDIUM) implement properly calling renaming
		rules<nso<tau_ba<BAs...>, BAs...>> nrs;
		nrs.insert(nrs.end(), rs1.begin(), rs1.end());
		nrs.insert(nrs.end(), rs2.begin(), rs2.end());
		return nrs;
	}
};

// TODO (HIGH) give a proper implementation for == operator
// We assume "other" to be normalized already
template<typename...BAs>
bool operator==(const tau_ba<BAs...>& other, const bool& b) {
	//auto normalized = normalizer<tau_ba<BAs...>, BAs...>(other.nso_rr);
	auto is_one = (other.nso_rr.main | tau_parser::wff_t).has_value();
	auto is_zero = (other.nso_rr.main | tau_parser::wff_f).has_value();
	return b ? is_one : is_zero ;
}

// TODO (HIGH) give a proper implementation for != operator
template<typename...BAs>
bool operator==(const bool& b, const tau_ba<BAs...>& other) {
	return other == b;
}

template<typename...BAs>
bool operator!=(const tau_ba<BAs...>& other, const bool& b) {
	return !(other == b);
}

template<typename...BAs>
bool operator!=(const bool& b, const tau_ba<BAs...>& other) {
	return !(other == b);
}

template<typename... BAs>
auto normalize (const tau_ba<BAs...>& fm) {
	auto res = normalizer<tau_ba<BAs...>, BAs...>(fm.nso_rr);
	return tau_ba<BAs...>(res);
}

template<typename... BAs>
auto splitter (const tau_ba<BAs...>& fm, splitter_type st) {
	auto s = tau_splitter(fm.nso_rr.main, st);
	return tau_ba<BAs...>(s);
}

template<typename...BAs>
using gssotc = nso<tau_ba<BAs...>, BAs...>;

template<typename...BAs>
using tau_spec = rr<gssotc<BAs...>>;


template<typename...BAs>
struct tau_ba_factory {

	gssotc<BAs...> parse(const std::string& src) {
		auto form = make_nso_rr_using_factory<tau_ba<BAs...>, BAs...>(src).main;
		tau_ba<BAs...> t(form);
		return make_node<tau_sym<tau_ba<BAs...>, BAs...>>(t, {});
	}

	gssotc<BAs...> binding(const gssotc<BAs...>& n) {
		auto source = n | tau_parser::source_binding | tau_parser::source | optional_value_extractor<gssotc<BAs...>>;
		std::string src = idni::tau::make_string(idni::tau::tau_node_terminal_extractor<tau_ba<BAs...>, BAs...>, source);
		return parse(src);
	}

	std::variant<tau_ba<BAs...>, BAs...> one () const {
		auto one = _T<tau_ba<BAs...>, BAs...>;
		return variant<tau_ba<BAs...>, BAs...>(tau_ba<BAs...>(one));
	}
};

} // namespace idni::tau

// << for printing tau_ba's form
template <typename... BAs>
std::ostream& operator<<(std::ostream& os, const idni::tau::tau_ba<BAs...>& rs) {
	return os << " : " << rs.nso_rr;
}

#endif // __TAU_H__