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

#include "normalizer2.h"

using namespace std;
using namespace idni::tau;

// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of sp_tau_node,...

namespace idni::tau {

// tau rules
RULE(TAU_DISTRIBUTE_0, "(($X ||| $Y) &&& $Z) :::= (($X &&& $Z) ||| ($Y &&& $Z)).")
RULE(TAU_DISTRIBUTE_1, "($X &&& ($Y ||| $Z)) :::= (($X &&& $Y) ||| ($X &&& $Z)).")
RULE(TAU_PUSH_NEGATION_INWARDS_0, "!!! ($X &&& $Y) :::= (!!! $X ||| !!! $Y).")
RULE(TAU_PUSH_NEGATION_INWARDS_1, "!!! ($X ||| $Y) :::= (!!! $X &&& !!! $Y).")
RULE(TAU_ELIM_DOUBLE_NEGATION_0, "!!! !!! $X :::=  $X.")
RULE(TAU_SIMPLIFY_ONE_0, "( {T} ||| $X ) :::= {T}.")
RULE(TAU_SIMPLIFY_ONE_1, "( $X ||| {T} ) :::= {T}.")
RULE(TAU_SIMPLIFY_ONE_2, "( {T} &&& $X ) :::= $X.")
RULE(TAU_SIMPLIFY_ONE_3, "( $X &&& {T} ) :::= $X.")
RULE(TAU_SIMPLIFY_ONE_4, "!!! {T} :::= {F}.")
RULE(TAU_SIMPLIFY_ZERO_0, "( {F} &&& $X ) :::= {F}.")
RULE(TAU_SIMPLIFY_ZERO_1, "( $X &&& {F} ) :::= {F}.")
RULE(TAU_SIMPLIFY_ZERO_2, "( {F} ||| $X ) :::= $X.")
RULE(TAU_SIMPLIFY_ZERO_3, "( $X ||| {F} ) :::= $X.")
RULE(TAU_SIMPLIFY_ZERO_4, "!!! {F} :::= {T}.")
RULE(TAU_SIMPLIFY_SELF_0, "( $X &&& $X ) :::= $X.")
RULE(TAU_SIMPLIFY_SELF_1, "( $X ||| $X ) :::= $X.")
RULE(TAU_SIMPLIFY_SELF_2, "( $X &&& !!! $X ) :::= {F}.")
RULE(TAU_SIMPLIFY_SELF_3, "( $X ||| !!! $X ) :::= {T}.")
RULE(TAU_SIMPLIFY_SELF_4, "( !!! $X &&& $X ) :::= {F}.")
RULE(TAU_SIMPLIFY_SELF_5, "( !!! $X ||| $X ) :::= {T}.")

RULE(TAU_COLLAPSE_POSITIVES_0, "($X &&& $Y) :::= tau_collapse_positives_cb $X $Y.")
RULE(TAU_COLLAPSE_POSITIVES_1, "($X &&& ($Y &&& $Z)) :::= tau_collapse_positives_cb $X $Y $Z.")
RULE(TAU_COLLAPSE_POSITIVES_2, "($X &&& ($Y &&& $Z)) :::= tau_collapse_positives_cb $X $Z $Y.")
RULE(TAU_COLLAPSE_POSITIVES_3, "($X &&& ($Y &&& $Z)) :::= tau_collapse_positives_cb $Y $Z $X.")
RULE(TAU_COLLAPSE_POSITIVES_4, "(($X &&& $Y) &&& $Z) :::= tau_collapse_positives_cb $Y $Z $X.")
RULE(TAU_COLLAPSE_POSITIVES_5, "(($X &&& $Y) &&& $Z) :::= tau_collapse_positives_cb $X $Z $Y.")
RULE(TAU_COLLAPSE_POSITIVES_6, "(($X &&& $Y) &&& $Z) :::= tau_collapse_positives_cb $X $Y $Z.")
RULE(TAU_PUSH_POSITIVES_UPWARDS_0, "($X &&& ($Y &&& $Z)) :::= tau_positives_upwards_cb $Y ($Y &&& ($X &&& $Z)).")
RULE(TAU_PUSH_POSITIVES_UPWARDS_1, "($X &&& ($Y &&& $Z)) :::= tau_positives_upwards_cb $Z ($Z &&& ($X &&& $Y)).")
RULE(TAU_PUSH_POSITIVES_UPWARDS_2, "(($X &&& $Y) &&& $Z) :::= tau_positives_upwards_cb $X ($X &&& ($Y &&& $Z)).")
RULE(TAU_PUSH_POSITIVES_UPWARDS_3, "(($X &&& $Y) &&& $Z) :::= tau_positives_upwards_cb $Y ($Y &&& ($X &&& $Z)).")

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

	tau_ba(rules<nso<tau_ba<BAs...>, BAs...>>& rec_relations, nso<tau_ba<BAs...>, BAs...>& main) : rr_nso({rec_relations, main}) {}
	tau_ba(nso<tau_ba<BAs...>, BAs...>& main) : rr_nso({main}) {}

	auto operator<=>(const tau_ba<BAs...>&) const = default;
	//bool operator==(const tau_ba<BAs...>&) const = default;

	tau_ba<BAs...> operator~() const {
		// TODO (HIGH) replace by ...tau... in the future
		nso<tau_ba<BAs...>, BAs...> nmain = build_wff_neg<tau_ba<BAs...>, BAs...>(rr_nso.main);
		auto nrec_relations = rr_nso.rec_relations;
		return tau_ba<BAs...>(nrec_relations, nmain);
	}

	tau_ba<BAs...> operator&(const tau_ba<BAs...>& other) const {
		// TODO (HIGH) replace by ...tau... in the future
		nso<tau_ba<BAs...>, BAs...> nmain = build_wff_and<tau_ba<BAs...>, BAs...>(rr_nso.main, other.rr_nso.main);
		rules<nso<tau_ba<BAs...>, BAs...>>  nrec_relations = merge(rr_nso.rec_relations, other.rr_nso.rec_relations);
		return tau_ba<BAs...>(nrec_relations, nmain);
	}

	tau_ba<BAs...> operator|(const tau_ba<BAs...>& other) const {
		// TODO (HIGH) replace by ...tau... in the future
		nso<tau_ba<BAs...>, BAs...> nmain = build_wff_or<tau_ba<BAs...>, BAs...>(rr_nso.main, other.rr_nso.main);
		rules<nso<tau_ba<BAs...>, BAs...>>  nrec_relations = merge(rr_nso.rec_relations, other.rr_nso.rec_relations);
		tau_ba<BAs...> nrr_nso(nrec_relations, nmain);
		return nrr_nso;
	}

	tau_ba<BAs...> operator+(const tau_ba<BAs...>& other) const {
		// TODO (HIGH) replace by ...tau... in the future
		nso<tau_ba<BAs...>, BAs...> nmain = build_wff_xor<tau_ba<BAs...>, BAs...>(rr_nso.main, other.rr_nso.main);
		rules<nso<tau_ba<BAs...>, BAs...>>  nrec_relations = merge(rr_nso.rec_relations, other.rr_nso.rec_relations);
		return tau_ba<BAs...>(nrec_relations, nmain);
	}

	tau_ba<BAs...> operator^(const tau_ba<BAs...>& other) const {
		return *this + other;
	}

	bool is_zero() const {
		// TODO (HIGH) replace by satisfability in the future
		auto vars = get_free_vars_from_nso(rr_nso.main);
		auto wff = rr_nso.main;
		for(auto& v: vars) wff = build_wff_all<BAs...>(v, wff);
		auto nrr_nso = rr<nso<tau_ba<BAs...>, BAs...>>(rr_nso.rec_relations, wff);
		auto normalized = normalizer<tau_ba<BAs...>, BAs...>(nrr_nso);
		auto check = normalized | tau_parser::wff_f;
		return check.has_value();
	}

	bool is_one() const {
		// TODO (HIGH) replace by satisfability in the future
		auto vars = get_free_vars_from_nso(rr_nso.main);
		auto wff = build_wff_neg(rr_nso.main);
		for(auto& v: vars) wff = build_wff_all<BAs...>(v, wff);
		auto nrr_nso = rr<nso<tau_ba<BAs...>, BAs...>>(rr_nso.rec_relations, wff);
		auto normalized = normalizer<tau_ba<BAs...>, BAs...>(nrr_nso);
		auto check = normalized | tau_parser::wff_f;
		return check.has_value();
	}

	// the type is ewquivalent to tau_spec<BAs...>
	const rr<nso<tau_ba<BAs...>, BAs...>> rr_nso;

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
	auto normalized = normalizer<tau_ba<BAs...>, BAs...>(other.rr_nso);
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
		factory_binder<tau_factory<base_factory_t, BAs...>, tau_ba<BAs...>, BAs...> fb(*this);
		auto form = make_nso_rr_using_factory<factory_binder<tau_factory<base_factory_t, BAs...>, tau_ba<BAs...>, BAs...>, tau_ba<BAs...>, BAs...>(var, fb).main;
		tau_ba<BAs...> t(form);
		return make_node<tau_sym<tau_ba<BAs...>, BAs...>>(t, {});
	}

	base_factory_t& bf;
};

// creates a specific rule from a generic rule.
// TODO (LOW) should depend in node_t instead of BAs...
template<typename... BAs>
rec_relation<gssotc<BAs...>> make_gssotc_rec_relation(gssotc<BAs...>& rule) {
	auto type = only_child_extractor<tau_ba<BAs...>, BAs...>(rule) | non_terminal_extractor<tau_ba<BAs...>, BAs...> | optional_value_extractor<size_t>;
	switch (type) {
	case tau_parser::bf_rec_relation: return make_rec_relation<tau_ba<BAs...>, BAs...>(tau_parser::bf_rec_relation, tau_parser::bf, rule);
	case tau_parser::wff_rec_relation: return make_rec_relation<tau_ba<BAs...>, BAs...>(tau_parser::wff_rec_relation, tau_parser::wff, rule);
	case tau_parser::tau_rec_relation: return make_rec_relation<tau_ba<BAs...>, BAs...>(tau_parser::tau_rec_relation, tau_parser::tau, rule);
	default: assert(false); return {};
	};
}

// create a set of relations from a given tau source.
template<typename... BAs>
// TODO (LOW) should depend in node_t instead of BAs...
rec_relations<gssotc<BAs...>> make_gssotc_rec_relations(gssotc<BAs...>& tau_source) {
	rec_relations<gssotc<BAs...>> rs;
	// TODO (LOW) change call to select by operator|| and operator|
	for (auto& r: select_top(tau_source, is_non_terminal<tau_parser::gssotc_rec_relation, tau_ba<BAs...>, BAs...>))
		rs.push_back(make_gssotc_rec_relation<BAs...>(r));
	return rs;
}

// make a nso_rr from the given tau source and binder.
template<typename binder_t, typename... BAs>
tau_spec<BAs...> make_tau_spec_using_binder(sp_tau_source_node& tau_source, binder_t& binder) {
	auto src = make_tau_code<tau_ba<BAs...>, BAs...>(tau_source);
	auto unbinded_main = src | tau_parser::gssotc_rr | tau_parser::gssotc_main | tau_parser::tau | optional_value_extractor<nso<tau_ba<BAs...>, BAs...>>;
	auto binded_main = post_order_traverser<
			binder_t,
			all_t<gssotc<BAs...>>,
			gssotc<BAs...>>(
		binder, all<gssotc<BAs...>>)(unbinded_main);
	auto gssotc_rr = make_gssotc_rec_relations(src);
	auto nso_rr = make_rec_relations(src);
	gssotc_rr.insert(gssotc_rr.end(), nso_rr.begin(), nso_rr.end());
	return { gssotc_rr, binded_main };
}

// make a nso_rr from the given tau source and bindings.
template<typename... BAs>
tau_spec<BAs...> make_tau_spec_using_bindings(sp_tau_source_node& tau_source, const bindings<BAs...>& bindings) {
	name_binder<tau_ba<BAs...>, BAs...> nb(bindings);
	bind_transformer<name_binder<tau_ba<BAs...>, BAs...>, tau_ba<BAs...>, BAs...> bs(nb);
	return make_tau_spec_using_binder<bind_transformer<name_binder<tau_ba<BAs...>, BAs...>, tau_ba<BAs...>, BAs...>, BAs...>(tau_source, bs);
}

// make a nso_rr from the given tau source and bindings.
template<typename factory_t, typename... BAs>
tau_spec<BAs...> make_tau_spec_using_factory(sp_tau_source_node& tau_source, factory_t& factory) {
	bind_transformer<factory_t, tau_ba<BAs...>, BAs...> bs(factory);
	return make_tau_spec_using_binder<bind_transformer<factory_t, tau_ba<BAs...>,  BAs...>, BAs...>(tau_source, bs);
}

// make a nso_rr from the given tau source and bindings.
template<typename factory_t, typename... BAs>
tau_spec<BAs...> make_tau_spec_using_factory(const std::string& source, factory_t& factory) {
	auto tau_source = make_tau_source(source);
	return make_tau_spec_using_factory<factory_t, tau_ba<BAs...>, BAs...>(tau_source, factory);
}

// make a nso_rr from the given tau source and bindings.
template<typename... BAs>
tau_spec<BAs...> make_tau_spec_using_bindings(const std::string& source, const bindings<BAs...>& bindings) {
	auto tau_source = make_tau_source(source);
	name_binder<BAs...> nb(bindings);
	bind_transformer<name_binder<BAs...>, BAs...> bs(nb);
	return make_tau_spec_using_bindings<
			bind_transformer<name_binder<BAs...>, BAs...>,
			BAs...>(
		tau_source, bs);
}

} // namespace idni::tau

// << for printing tau_ba's form
template <typename... BAs>
std::ostream& operator<<(std::ostream& os, const idni::tau::tau_ba<BAs...>& rs) {
	return os << rs.rr_nso;
}

#endif // __TAU_H__