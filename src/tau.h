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

namespace idni::tau {

template<typename...BAs>
using wff = sp_tau_node<BAs...>;

// Check https://gcc.gnu.org/bugzilla/show_bug.cgi?id=102609 to follow up on
// the implementation of "Deducing this" on gcc.
// See also (https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0847r7.html)
// and https://devblogs.microsoft.com/cppblog/cpp23-deducing-this/ for how to use
// "Deducing this" on CRTP..

template<typename...BAs>
auto operator<=>(const tau_sym<BAs...>& l, const tau_sym<BAs...>& r) {
	// TODO (HIGH) review return values when we have different types of tau_sym
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

	tau(formula<tau<BAs...>, BAs...>& form) : form(form) {}
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

	formula<tau<BAs...>, BAs...> form;
};

template<typename...BAs>
bool operator==(const tau<BAs...>& other, const bool& b) {
	auto nother = other;
	auto normalized = normalizer<tau<BAs...>, BAs...>(nother.form);
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
		auto form = make_formula_using_factory<factory_binder<tau_factory<base_factory_t, BAs...>, tau<BAs...>, BAs...>, tau<BAs...>, BAs...>(var, fb).main;
		tau<BAs...> t(form);
		return make_node<tau_sym<tau<BAs...>, BAs...>>(t, {});
	}

	base_factory_t& bf;
};

template<typename base_factory_t, typename...BAs>
formula<tau<BAs...>> make_tau_using_factory(const std::string& src, base_factory_t& bf) {
	tau_factory<base_factory_t, BAs...> tf(bf);
	return make_formula_using_factory<tau_factory<base_factory_t, BAs...>, tau<BAs...>>(src, tf);
}

template<typename...BAs>
formula<tau<BAs...>> make_tau_using_bindings(const std::string& src, const bindings<tau<BAs...>>& bs) {
	return make_formula_using_bindings<tau<BAs...>>(src, bs);
}

template<typename...BAs>
ostream& operator<<(ostream& os, const formula<tau<BAs...>>& t) {
	// TODO (MEDIUM) implement
	return os << "tau<>";
}

} // namespace idni::tau

#endif // __TAU_H__