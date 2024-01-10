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

template<typename...BAs>
using wff = sp_tau_node<BAs...>;

// tau rules
RULE(TAU_DISTRIBUTE_0, "(($X ||| $Y) &&& $Z) := (($X &&& $Z) ||| ($Y &&& $Z)).")
RULE(TAU_DISTRIBUTE_1, "($X &&& ($Y ||| $Z)) := (($X &&& $Y) ||| ($X &&& $Z)).")
RULE(TAU_PUSH_NEGATION_INWARDS_0, "% ($X &&& $Y) := (% $X ||| % $Y).")
RULE(TAU_PUSH_NEGATION_INWARDS_1, "% ($X ||| $Y) := (% $X &&& % $Y).")
RULE(TAU_ELIM_DOUBLE_NEGATION_0, "% % $X :=  $X.")
RULE(TAU_SIMPLIFY_ONE_0, "( T ||| $X ) := T.")
RULE(TAU_SIMPLIFY_ONE_1, "( $X ||| T ) := T.")
RULE(TAU_SIMPLIFY_ONE_2, "( T &&& $X ) := $X.")
RULE(TAU_SIMPLIFY_ONE_3, "( $X &&& T ) := $X.")
RULE(TAU_SIMPLIFY_ONE_4, "% T := F.")
RULE(TAU_SIMPLIFY_ZERO_0, "( F &&& $X ) := F.")
RULE(TAU_SIMPLIFY_ZERO_1, "( $X &&& F ) := F.")
RULE(TAU_SIMPLIFY_ZERO_2, "( F ||| $X ) := $X.")
RULE(TAU_SIMPLIFY_ZERO_3, "( $X ||| F ) := $X.")
RULE(TAU_SIMPLIFY_ZERO_4, "% F := T.")
RULE(TAU_SIMPLIFY_SELF_0, "( $X &&& $X ) := $X.")
RULE(TAU_SIMPLIFY_SELF_1, "( $X ||| $X ) := $X.")
RULE(TAU_SIMPLIFY_SELF_2, "( $X &&& % $X ) := F.")
RULE(TAU_SIMPLIFY_SELF_3, "( $X ||| % $X ) := T.")
RULE(TAU_SIMPLIFY_SELF_4, "( % $X &&& $X ) := F.")
RULE(TAU_SIMPLIFY_SELF_5, "( % $X ||| $X ) := T.")

RULE(TAU_COLLAPSE_POSITIVES_0, "($X &&& $Y) := tau_collapse_positives_cb $X $Y.")
RULE(TAU_COLLAPSE_POSITIVES_1, "($X &&& ($Y &&& $Z)) := tau_collapse_positives_cb $X $Y $Z.")
RULE(TAU_COLLAPSE_POSITIVES_2, "($X &&& ($Y &&& $Z)) := tau_collapse_positives_cb $X $Z $Y.")
RULE(TAU_COLLAPSE_POSITIVES_3, "($X &&& ($Y &&& $Z)) := tau_collapse_positives_cb $Y $Z $X.")
RULE(TAU_COLLAPSE_POSITIVES_4, "(($X &&& $Y) &&& $Z) := tau_collapse_positives_cb $Y $Z $X.")
RULE(TAU_COLLAPSE_POSITIVES_5, "(($X &&& $Y) &&& $Z) := tau_collapse_positives_cb $X $Z $Y.")
RULE(TAU_COLLAPSE_POSITIVES_6, "(($X &&& $Y) &&& $Z) := tau_collapse_positives_cb $X $Y $Z.")
RULE(TAU_PUSH_POSITIVES_UPWARDS_0, "($X &&& ($Y &&& $Z)) := tau_positives_upwards_cb $Y ($Y &&& ($X &&& $Z)).")
RULE(TAU_PUSH_POSITIVES_UPWARDS_1, "($X &&& ($Y &&& $Z)) := tau_positives_upwards_cb $Z ($Z &&& ($X &&& $Y)).")
RULE(TAU_PUSH_POSITIVES_UPWARDS_2, "(($X &&& $Y) &&& $Z) := tau_positives_upwards_cb $X ($X &&& ($Y &&& $Z)).")
RULE(TAU_PUSH_POSITIVES_UPWARDS_3, "(($X &&& $Y) &&& $Z) := tau_positives_upwards_cb $Y ($Y &&& ($X &&& $Z)).")

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
// "Deducing this" on CRTP..

// TODO (HIGH) give a proper implementation for <=>, == and != operators
template<typename...BAs>
auto operator<=>(const tau_sym<BAs...>& l, const tau_sym<BAs...>& r) {
	return std::addressof(l)<=>std::addressof(r);
	/*auto cmp = overloaded(*/
		/*[](const tau_source_sym& l, const tau_source_sym& r) -> std::partial_ordering {*/
		/*[](const tau_source_sym& l, const tau_source_sym& r) -> std::strong_ordering {
			return l<=>r;*/
			/*std::strong_ordering strong = l<=> r;
			if (strong == std::strong_ordering::less) {
				return std::partial_ordering::less;
			} else if (strong == std::strong_ordering::equal) {
				return std::partial_ordering::equivalent;
			} else  {
				return std::partial_ordering::greater;
			}*/
		/*}, [](const tau_source_sym&, const auto&) -> std::partial_ordering { return std::partial_ordering::greater; },*/
		/*}, [](const tau_source_sym&, const auto&) -> std::strong_ordering { return std::strong_ordering::greater; },*/
		/*[](const auto&, const tau_source_sym&) -> std::partial_ordering { return std::strong_ordering::less; },*/
		/*[](const auto&, const tau_source_sym&) -> std::strong_ordering { return std::strong_ordering::less; },*/
		/*[](const size_t& l, const size_t& r) -> std::partial_ordering {*/
		/*[](const size_t& l, const size_t& r) -> std::strong_ordering {
			return l<=>r;*/
			/*std::strong_ordering strong = l<=> r;
			if (strong == std::strong_ordering::less) {
				return std::partial_ordering::less;
			} else if (strong == std::strong_ordering::equal) {
				return std::partial_ordering::equivalent;
			} else  {
				return std::partial_ordering::greater;
			}*/
		/*},*/
		/*[](const size_t&, const auto&) -> std::partial_ordering { return std::partial_ordering::greater; },*/
		/*[](const size_t&, const auto&) -> std::strong_ordering { return std::strong_ordering::greater; },*/
		/*[](const auto&, const size_t&) -> std::partial_ordering { return std::partial_ordering::less; },*/
		/*[](const auto&, const size_t&) -> std::strong_ordering { return std::strong_ordering::less; },*/
		// we could allow unordered BAs by defining here the order of the BAs as follows
		// []<typename T>(const T& l, const T& r) -> std::partial_ordering {
		//		if ((l & ~r == false) | (l ^ ~r != false)) return std::partial_ordering::less;
		//		if (l ^ r == false) return std::partial_ordering::equivalent;
		//		if ((l & ~r != false) | (l ^ ~r == false)) return std::partial_ordering::greater;
		//		return std::partial_ordering::unordered;
		// otherwise, we need the following:
		/*[]<typename T>(const T& l, const T& r) -> std::partial_ordering {*/
		/*[]<typename T>(const T& l, const T& r) -> std::strong_ordering {
			return std::addressof(l)<=>std::addressof(r);*/
			//return l<=>r;
		/*},*/
		/*[](const auto&, const auto&) -> std::partial_ordering { return std::partial_ordering::unordered; }*/
		/*[](const auto&, const auto&) -> std::partial_ordering { throw std::logic_error("bad type"); }
	);
	return std::visit(cmp, l, r);*/
}

template<typename...BAs>
struct tau {

	tau(nso_rr<tau<BAs...>, BAs...>& form) : form(form) {}
	tau(wff<tau<BAs...>, BAs...>& main) : form(main) {}

	auto operator<=>(const tau<BAs...>& other) {
		return form <=> other.form;
	}

	bool operator==(const tau<BAs...>& other) const {
		return form.main == other.form.main;
	}

	bool operator!=(const tau<BAs...>& other) const {
		return form.main != other.form.main;
	}

	tau<BAs...> operator~() const {
		auto nform = build_wff_neg<tau<BAs...>, BAs...>(form.main);
		return tau<BAs...>(nform);
	}

	tau<BAs...> operator&(const tau<BAs...>& other) const {
		auto nform = build_wff_and<tau<BAs...>, BAs...>(form.main, other.form.main);
		return tau<BAs...>(nform);
	}

	tau<BAs...> operator|(const tau<BAs...>& other) const {
		auto nform = build_wff_or<tau<BAs...>, BAs...>(form.main, other.form.main);
		return tau<BAs...>(nform);
	}

	tau<BAs...> operator^(const tau<BAs...>& other) const {
		auto nform = build_wff_xor<tau<BAs...>, BAs...>(form.main, other.form.main);
		return tau<BAs...>(nform);
	}

	tau<BAs...> operator+(const tau<BAs...>& other) const {
		auto nform = build_wff_xor<tau<BAs...>, BAs...>(form.main, other.form.main);
		return tau<BAs...>(nform);
	}

	bool is_zero() const {
		auto normalized = normalizer<tau<BAs...>, BAs...>(form);
		return (normalized.main | tau_parser::wff_f).has_value();
	}

	bool is_one() const {
		auto normalized = normalizer<tau<BAs...>, BAs...>(form);
		return (normalized.main | tau_parser::wff_t).has_value();
	}

	// REVIEW (HIGH) this should be a wff<tau<BAs...>, BAs...>
	//
	// Maybe confirm with Ohad.
	nso_rr<tau<BAs...>, BAs...> form;
};

template<typename...BAs>
bool operator==(const tau<BAs...>& other, const bool& b) {
	auto normalized = normalizer<tau<BAs...>, BAs...>(other.form);
	auto is_one = (normalized.main | tau_parser::wff_t).has_value();
	auto is_zero = (normalized.main | tau_parser::wff_f).has_value();
	return b ? is_one : is_zero ;
}

template<typename...BAs>
bool operator==(const bool& b, const tau<BAs...>& other) {
	return other == b;
}

template<typename...BAs>
bool operator!=(const tau<BAs...>& other, const bool& b) {
	return !(other == b);
}

template<typename...BAs>
bool operator!=(const bool& b, const tau<BAs...>& other) {
	return !(other == b);
}

template<typename base_factory_t, typename...BAs>
struct tau_factory {

	tau_factory(base_factory_t& bf) : bf(bf) {}

	sp_tau_node<tau<BAs...>, BAs...> build(const std::string type_name, const sp_tau_node<tau<BAs...>, BAs...>& n) {
		if (auto nn = bf.build(type_name, n); nn != n) return nn;
		auto source = n | tau_parser::source_binding | tau_parser::source | optional_value_extractor<sp_tau_node<tau<BAs...>, BAs...>>;
		std::string var = idni::tau::make_string(idni::tau::tau_node_terminal_extractor<tau<BAs...>, BAs...>, source);
		factory_binder<tau_factory<base_factory_t, BAs...>, tau<BAs...>, BAs...> fb(*this);
		auto form = make_nso_rr_using_factory<factory_binder<tau_factory<base_factory_t, BAs...>, tau<BAs...>, BAs...>, tau<BAs...>, BAs...>(var, fb).main;
		tau<BAs...> t(form);
		return make_node<tau_sym<tau<BAs...>, BAs...>>(t, {});
	}

	base_factory_t& bf;
};


// TODO (HIGH) rewrite this using tau formulas instead of tau_parser::nso_rr
template<typename base_factory_t, typename...BAs>
nso_rr<tau<BAs...>, BAs...> make_tau_using_factory(const std::string& src, base_factory_t& bf) {
	tau_factory<base_factory_t, BAs...> tf(bf);
	return make_nso_rr_using_factory<tau_factory<base_factory_t, BAs...>, tau<BAs...>>(src, tf);
}

// TODO (HIGH) rewrite this using tau formulas instead of tau_parser::nso_rr
template<typename...BAs>
nso_rr<tau<BAs...>, BAs...> make_tau_using_bindings(const std::string& src, const bindings<tau<BAs...>>& bs) {
	return make_nso_rr_using_bindings<tau<BAs...>>(src, bs);
}

// TODO (HIGH) add convert tau nso_rr to dnf

// TODO (HIGH) add convert tau dnf nso_rr to single positive dnf

} // namespace idni::tau

#endif // __TAU_H__