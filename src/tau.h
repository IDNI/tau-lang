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

using namespace std;
using namespace idni::tau;

// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of sp_tau_node,...

namespace idni::tau {

// tau rules
RULE(TAU_DISTRIBUTE_0, "($X ||| $Y) &&& $Z :::= $X &&& $Z ||| $Y &&& $Z.")
RULE(TAU_DISTRIBUTE_1, "$X &&& ($Y ||| $Z) :::= $X &&& $Y ||| $X &&& $Z.")
RULE(TAU_PUSH_NEGATION_INWARDS_0, "!!! ($X &&& $Y) :::= !!! $X ||| !!! $Y.")
RULE(TAU_PUSH_NEGATION_INWARDS_1, "!!! ($X ||| $Y) :::= !!! $X &&& !!! $Y.")
RULE(TAU_ELIM_DOUBLE_NEGATION_0, "!!! !!! $X :::=  $X.")
RULE(TAU_SIMPLIFY_ONE_0, "{T} ||| $X :::= {T}.")
RULE(TAU_SIMPLIFY_ONE_1, "$X ||| {T} :::= {T}.")
RULE(TAU_SIMPLIFY_ONE_2, "{T} &&& $X :::= $X.")
RULE(TAU_SIMPLIFY_ONE_3, "$X &&& {T} :::= $X.")
RULE(TAU_SIMPLIFY_ONE_4, "!!! {T} :::= {F}.")
RULE(TAU_SIMPLIFY_ZERO_0, "{F} &&& $X :::= {F}.")
RULE(TAU_SIMPLIFY_ZERO_1, "$X &&& {F} :::= {F}.")
RULE(TAU_SIMPLIFY_ZERO_2, "{F} ||| $X :::= $X.")
RULE(TAU_SIMPLIFY_ZERO_3, "$X ||| {F} :::= $X.")
RULE(TAU_SIMPLIFY_ZERO_4, "!!! {F} :::= {T}.")
RULE(TAU_SIMPLIFY_SELF_0, "$X &&& $X :::= $X.")
RULE(TAU_SIMPLIFY_SELF_1, "$X ||| $X :::= $X.")
RULE(TAU_SIMPLIFY_SELF_2, "$X &&& !!! $X :::= {F}.")
RULE(TAU_SIMPLIFY_SELF_3, "$X ||| !!! $X :::= {T}.")
RULE(TAU_SIMPLIFY_SELF_4, "!!! $X &&& $X :::= {F}.")
RULE(TAU_SIMPLIFY_SELF_5, "!!! $X ||| $X :::= {T}.")

RULE(TAU_COLLAPSE_POSITIVES_0, "$X &&& $Y :::= tau_collapse_positives_cb $X $Y.")
RULE(TAU_COLLAPSE_POSITIVES_1, "$X &&& ($Y &&& $Z) :::= tau_collapse_positives_cb $X $Y $Z.")
RULE(TAU_COLLAPSE_POSITIVES_2, "$X &&& ($Y &&& $Z) :::= tau_collapse_positives_cb $X $Z $Y.")
RULE(TAU_COLLAPSE_POSITIVES_3, "$X &&& ($Y &&& $Z) :::= tau_collapse_positives_cb $Y $Z $X.")
RULE(TAU_COLLAPSE_POSITIVES_4, "($X &&& $Y) &&& $Z :::= tau_collapse_positives_cb $Y $Z $X.")
RULE(TAU_COLLAPSE_POSITIVES_5, "($X &&& $Y) &&& $Z :::= tau_collapse_positives_cb $X $Z $Y.")
RULE(TAU_COLLAPSE_POSITIVES_6, "($X &&& $Y) &&& $Z :::= tau_collapse_positives_cb $X $Y $Z.")
RULE(TAU_PUSH_POSITIVES_UPWARDS_0, "$X &&& ($Y &&& $Z) :::= tau_positives_upwards_cb $Y ($Y &&& ($X &&& $Z)).")
RULE(TAU_PUSH_POSITIVES_UPWARDS_1, "$X &&& ($Y &&& $Z) :::= tau_positives_upwards_cb $Z ($Z &&& ($X &&& $Y)).")
RULE(TAU_PUSH_POSITIVES_UPWARDS_2, "($X &&& $Y) &&& $Z :::= tau_positives_upwards_cb $X ($X &&& ($Y &&& $Z)).")
RULE(TAU_PUSH_POSITIVES_UPWARDS_3, "($X &&& $Y) &&& $Z :::= tau_positives_upwards_cb $Y ($Y &&& ($X &&& $Z)).")

template<typename... BAs>
static auto to_dnf_tau = make_library<BAs...>(
	TAU_DISTRIBUTE_0
	+ TAU_DISTRIBUTE_1
	+ TAU_PUSH_NEGATION_INWARDS_0
	+ TAU_PUSH_NEGATION_INWARDS_1
	+ TAU_ELIM_DOUBLE_NEGATION_0
);

template<typename... BAs>
static auto simplify_tau = make_library<BAs...>(
	TAU_SIMPLIFY_ONE_0
	+ TAU_SIMPLIFY_ONE_1
	+ TAU_SIMPLIFY_ONE_2
	+ TAU_SIMPLIFY_ONE_3
	+ TAU_SIMPLIFY_ONE_4
	+ TAU_SIMPLIFY_ZERO_0
	+ TAU_SIMPLIFY_ZERO_1
	+ TAU_SIMPLIFY_ZERO_2
	+ TAU_SIMPLIFY_ZERO_3
	+ TAU_SIMPLIFY_ZERO_4
	+ TAU_SIMPLIFY_SELF_0
	+ TAU_SIMPLIFY_SELF_1
	+ TAU_SIMPLIFY_SELF_2
	+ TAU_SIMPLIFY_SELF_3
	+ TAU_SIMPLIFY_SELF_4
	+ TAU_SIMPLIFY_SELF_5
);

template<typename... BAs>
static auto collapse_positives_tau = make_library<BAs...>(
	TAU_COLLAPSE_POSITIVES_0
	+ TAU_COLLAPSE_POSITIVES_1
	+ TAU_COLLAPSE_POSITIVES_2
	+ TAU_COLLAPSE_POSITIVES_3
	+ TAU_COLLAPSE_POSITIVES_4
	+ TAU_COLLAPSE_POSITIVES_5
	+ TAU_COLLAPSE_POSITIVES_6
	+ TAU_PUSH_POSITIVES_UPWARDS_0
	+ TAU_PUSH_POSITIVES_UPWARDS_1
	+ TAU_PUSH_POSITIVES_UPWARDS_2
	+ TAU_PUSH_POSITIVES_UPWARDS_3
);

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
		nso<tau_ba<BAs...>, BAs...> nmain = build_wff_xor<tau_ba<BAs...>, BAs...>(nso_rr.main, other.nso_rr.main);
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

/*// TODO (HIGH) give a proper implementation for <=> operator
template<typename...BAs>
auto operator<=>(const tau_ba<BAs...>& l, const tau_ba<BAs...>& r) {
	return std::addressof(l)<=>std::addressof(r);
}*/

// TODO (HIGH) give a proper implementation for == operator
template<typename...BAs>
bool operator==(const tau_ba<BAs...>& other, const bool& b) {
	auto normalized = normalizer<tau_ba<BAs...>, BAs...>(other.nso_rr);
	auto is_one = (normalized | tau_parser::wff_t).has_value();
	auto is_zero = (normalized | tau_parser::wff_f).has_value();
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

template<typename...BAs>
using gssotc = nso<tau_ba<BAs...>, BAs...>;

template<typename...BAs>
using tau_spec = rr<gssotc<BAs...>>;

template <typename... BAs>
using gssotc_rule = rule<gssotc<BAs...>>;

// defines a vector of rec. relations in the tau language, the order is important as it defines
// the order of the rec relations in the rewriting process of the tau language.
template <typename... BAs>
using gssotc_rec_relation = rule<gssotc<BAs...>>;

template<typename base_factory_t, typename...BAs>
struct tau_factory {

	tau_factory(base_factory_t& bf) : bf(bf) {}

	gssotc<BAs...> build(const std::string type_name, const gssotc<BAs...>& n) {
		if (auto nn = bf.build(type_name, n); nn != n) return nn;
		auto source = n | tau_parser::source_binding | tau_parser::source | optional_value_extractor<gssotc<BAs...>>;
		std::string var = idni::tau::make_string(idni::tau::tau_node_terminal_extractor<tau_ba<BAs...>, BAs...>, source);
		auto form = make_nso_rr_using_factory<tau_factory<base_factory_t, BAs...>, tau_ba<BAs...>, BAs...>(var, *this).main;
		tau_ba<BAs...> t(form);
		return make_node<tau_sym<tau_ba<BAs...>, BAs...>>(t, {});
	}

	base_factory_t& bf;
};

template<typename...BAs>
struct tau_splitter {

	std::variant<tau_ba<BAs...>, BAs...> operator()(const std::variant<tau_ba<BAs...>, BAs...>& n) {
		// TODO (HIGH) implement tau_ba splitter
	}

};

// creates a specific rule from a generic rule.
// TODO (LOW) should depend in node_t instead of BAs...
template<typename... BAs>
rec_relation<gssotc<BAs...>> make_gssotc_rec_relation(const gssotc<BAs...>& rule) {
	auto rr = rule | tau_parser::gssotc_rec_relation_form
		| only_child_extractor<tau_ba<BAs...>, BAs...>
		| optional_value_extractor<gssotc<BAs...>>;
	auto type = rr | non_terminal_extractor<tau_ba<BAs...>, BAs...> | optional_value_extractor<size_t>;
	switch (type) {
	case tau_parser::bf_rec_relation:  return make_rec_relation<tau_ba<BAs...>, BAs...>(tau_parser::bf_ref,  tau_parser::bf,  rr);
	case tau_parser::wff_rec_relation: return make_rec_relation<tau_ba<BAs...>, BAs...>(tau_parser::wff_ref, tau_parser::wff, rr);
	case tau_parser::tau_rec_relation: return make_rec_relation<tau_ba<BAs...>, BAs...>(tau_parser::tau_ref, tau_parser::tau, rr);
	default: assert(false); return {};
	};
}

// create a set of relations from a given tau source.
template<typename... BAs>
// TODO (LOW) should depend in node_t instead of BAs...
rec_relations<gssotc<BAs...>> make_gssotc_rec_relations(const gssotc<BAs...>& tau_source) {
	rec_relations<gssotc<BAs...>> rs;
	// TODO (LOW) change call to select by operator|| and operator|
	for (auto& r: select_top(tau_source, is_non_terminal<tau_parser::gssotc_rec_relation, tau_ba<BAs...>, BAs...>))
		rs.push_back(make_gssotc_rec_relation<BAs...>(r));
	return rs;
}

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
tau_spec<BAs...> make_tau_spec_using_binder(sp_tau_node<tau_ba<BAs...>, BAs...>& code, binder_t& binder) {
	auto binded = bind_tau_code_using_binder<binder_t, tau_ba<BAs...>, BAs...>(code, binder);
	auto main = binded | tau_parser::gssotc_rr | tau_parser::gssotc_main | tau_parser::tau | optional_value_extractor<nso<tau_ba<BAs...>, BAs...>>;
	auto gssotc_rr = make_gssotc_rec_relations(binded);
	auto nso_rr = make_rec_relations(binded);
	gssotc_rr.insert(gssotc_rr.end(), nso_rr.begin(), nso_rr.end());
	return { gssotc_rr, main };
}

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
tau_spec<BAs...> make_tau_spec_using_binder(sp_tau_source_node& source, binder_t& binder) {
	auto code = make_tau_code<tau_ba<BAs...>, BAs...>(source);
	return make_tau_spec_using_binder<binder_t, BAs...>(code, binder);
}

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
tau_spec<BAs...> make_tau_spec_using_binder(std::string& input, binder_t& binder) {
	auto source = make_tau_source(input);
	return make_tau_spec_using_binder<binder_t, BAs...>(source, binder);
}

// make a nso_rr from the given tau source and bindings.
template<typename... BAs>
tau_spec<BAs...> make_tau_spec_using_bindings(sp_tau_node<tau_ba<BAs...>, BAs...>& code, const bindings<BAs...>& bindings) {
	name_binder<tau_ba<BAs...>, BAs...> nb(bindings);
	return make_tau_spec_using_binder<name_binder<tau_ba<BAs...>, BAs...>, BAs...>(code, nb);
}

// make a nso_rr from the given tau source and bindings.
template<typename... BAs>
tau_spec<BAs...> make_tau_spec_using_bindings(sp_tau_source_node& source, const bindings<BAs...>& bindings) {
	auto code = make_tau_code<tau_ba<BAs...>, BAs...>(source);
	return make_tau_spec_using_bindings<BAs...>(code, bindings);
}

// make a nso_rr from the given tau source and bindings.
template<typename... BAs>
tau_spec<BAs...> make_tau_spec_using_bindings(const std::string& input, const bindings<BAs...>& bindings) {
	auto source = make_tau_source(input);
	return make_tau_spec_using_bindings<tau_ba<BAs...>, BAs...>(source, bindings);
}

// make a nso_rr from the given tau source and bindings.
template<typename factory_t, typename... BAs>
tau_spec<BAs...> make_tau_spec_using_factory(sp_tau_node<tau_ba<BAs...>, BAs...>& code, factory_t& factory) {
	factory_binder<factory_t, tau_ba<BAs...>, BAs...> bs(factory);
	return make_tau_spec_using_binder<factory_binder<factory_t, tau_ba<BAs...>, BAs...>, BAs...>(code, bs);
}

// make a nso_rr from the given tau source and bindings.
template<typename factory_t, typename... BAs>
tau_spec<BAs...> make_tau_spec_using_factory(sp_tau_source_node& source, factory_t& factory) {
	auto code = make_tau_code<tau_ba<BAs...>, BAs...>(source);
	return make_tau_spec_using_factory<factory_t, BAs...>(code, factory);
}

// make a nso_rr from the given tau source and bindings.
template<typename factory_t, typename... BAs>
tau_spec<BAs...> make_tau_spec_using_factory(const std::string& input, factory_t& factory) {
	auto source = make_tau_source(input);
	return make_tau_spec_using_factory<factory_t, BAs...>(source, factory);
}

} // namespace idni::tau

// << for printing tau_ba's form
template <typename... BAs>
std::ostream& operator<<(std::ostream& os, const idni::tau::tau_ba<BAs...>& rs) {
	return os << " : " << rs.nso_rr;
}

#endif // __TAU_H__