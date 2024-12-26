// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __NSO_BA_H__
#define __NSO_BA_H__

#include "builders.h"

namespace idni::tau_lang {

template <typename... BAs>
tau<BAs...> operator&(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	auto bf_constant_and = [](const auto& l, const auto& r) -> tau<BAs...> {
		auto lc = l
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		auto rc = r
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		return build_bf_constant<BAs...>(lc & rc);
	};

	// trivial cases
	if ( l == _0<BAs...> || r == _0<BAs...> ) return _0<BAs...>;
	if ( l == _1<BAs...> ) return r;
	if ( r == _1<BAs...> ) return l;

	// more elaborate cases
	if (is_child_non_terminal<tau_parser::bf_constant, BAs...>(l)
		&& is_child_non_terminal<tau_parser::bf_constant, BAs...>(r))
			return bf_constant_and(l, r);
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_non_terminal<tau_parser::bf, BAs...>(r))
			return build_bf_and<BAs...>(l, r);
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_eq, BAs...>(r))
	{
		auto rr = r
			| tau_parser::bf_eq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_eq<BAs...>(l & rr);
	}
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_neq, BAs...>(r)) {
		auto rr = r
			| tau_parser::bf_neq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_neq<BAs...>(l & rr);
	}
	if (is_non_terminal<tau_parser::wff>(l)
			&& is_non_terminal<tau_parser::wff, BAs...>(r))
		return build_wff_and<BAs...>(l, r);
	throw std::logic_error("wrong types");
}

template <typename... BAs>
tau<BAs...> operator|(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_non_terminal<tau_parser::bf, BAs...>(r))
			return build_bf_or<BAs...>(l, r);
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_eq, BAs...>(r))
	{
		auto rr = r
			| tau_parser::bf_eq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_eq<BAs...>(l | rr);
	}
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_neq, BAs...>(r))
	{
		auto rr = r
			| tau_parser::bf_neq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_neq<BAs...>(l | rr);
	}
	if (is_non_terminal<tau_parser::wff>(l)
		&& is_non_terminal<tau_parser::wff, BAs...>(r))
			return build_wff_or<BAs...>(l, r);
	throw std::logic_error("wrong types");
}

template <typename... BAs>
tau<BAs...> operator~(const tau<BAs...>& l) {
	auto bf_constant_neg = [](const auto& l) -> tau<BAs...> {
		auto lc = l
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		return build_bf_constant<BAs...>(~lc);
	};

	// trivial cases
	if (l == _0<BAs...>) return _1<BAs...>;
	if (l == _1<BAs...>) return _0<BAs...>;

	// more elaborate cases
	if (is_child_non_terminal<tau_parser::bf_constant, BAs...>(l))
		return bf_constant_neg(l);
	if (is_non_terminal<tau_parser::bf>(l))
		return build_bf_neg<BAs...>(l);
	if (is_child_non_terminal<tau_parser::bf_eq, BAs...>(l)) {
		auto ll = l
			| tau_parser::bf_eq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_eq<BAs...>(~ll);
	}
	if (is_child_non_terminal<tau_parser::bf_neq, BAs...>(l)) {
		auto ll = l
			| tau_parser::bf_neq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_neq<BAs...>(~ll);
	}
	if (is_non_terminal<tau_parser::wff>(l))
		return build_wff_neg<BAs...>(l);
	throw std::logic_error("wrong types");
}

template <typename... BAs>
tau<BAs...> operator^(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	auto bf_constant_xor = [](const auto& l, const auto& r) -> tau<BAs...> {
		auto lc = l
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		auto rc = r
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		return build_bf_constant<BAs...>(lc ^ rc);
	};

	// trivial cases
	if (l == _0<BAs...>) return r;
	if (r == _0<BAs...>) return l;

	// more elaborate cases
	if (is_child_non_terminal<tau_parser::bf_constant, BAs...>(l)
		&& is_child_non_terminal<tau_parser::bf_constant, BAs...>(r))
			return bf_constant_xor(l, r);
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_non_terminal<tau_parser::bf, BAs...>(r))
			return build_bf_xor<BAs...>(l, r);
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_eq, BAs...>(r))
	{
		auto rr = r
			| tau_parser::bf_eq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_eq<BAs...>(l ^ rr);
	}
	if (is_non_terminal<tau_parser::bf>(l)
		&& is_child_non_terminal<tau_parser::bf_neq, BAs...>(r))
	{
		auto rr = r
			| tau_parser::bf_neq
			| tau_parser::bf
			| optional_value_extractor<tau<BAs...>>;
		return build_wff_neq<BAs...>(l ^ rr);
	}
	if (is_non_terminal<tau_parser::wff>(l)
		&& is_non_terminal<tau_parser::wff, BAs...>(r))
			return build_wff_xor<BAs...>(l, r);
	throw std::logic_error("wrong types");
}

template <typename... BAs>
tau<BAs...> operator+(const tau<BAs...>& l,
	const tau<BAs...>& r)
{
	return l ^ r;
}

template <typename... BAs>
bool is_zero(const tau<BAs...>& l) {
	auto bf_constant_is_zero = [](const auto& l) -> bool {
		auto lc = l
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		return is_zero(lc);
	};

	// trivial cases
	if (l == _0<BAs...>) return true;
	if (l == _1<BAs...>) return false;

	// more elaborate cases
	if (is_child_non_terminal<tau_parser::bf_constant, BAs...>(l))
		return bf_constant_is_zero(l);
	if (is_non_terminal<tau_parser::bf>(l))
		return l == _0<BAs...>;
	if (is_non_terminal<tau_parser::wff>(l))
		return l == _F<BAs...>;
	throw std::logic_error("wrong types");
}

template <typename... BAs>
bool is_one(const tau<BAs...>& l) {
	auto bf_constant_is_one = [](const auto& l) -> bool {
		auto lc = l
			| tau_parser::bf_constant
			| tau_parser::constant
			| only_child_extractor<BAs...>
			| ba_extractor<BAs...>
			| optional_value_extractor<std::variant<BAs...>>;
		return is_one(lc);
	};

	// trivial cases
	if (l == _0<BAs...>) return false;
	if (l == _1<BAs...>) return true;

	// more elaborate cases
	if (is_child_non_terminal<tau_parser::bf_constant, BAs...>(l))
		return bf_constant_is_one(l);
	if (is_non_terminal<tau_parser::bf>(l))
		return l == _1<BAs...>;
	if (is_non_terminal<tau_parser::wff>(l))
		return l == _T<BAs...>;
	throw std::logic_error("wrong types");
}

template <typename... BAs>
bool operator==(const tau<BAs...>& l, const bool& r) {
	return r ? is_one(l) : is_zero(l);
}

template <typename... BAs>
bool operator==(const bool l, const tau<BAs...>& r) {
	return r == l;
}

// Splitter function for a nso tau_parser::bf_constant node holding a BA constant
template <typename... BAs>
tau<BAs...> splitter(const tau<BAs...>& n,
	splitter_type st = splitter_type::upper)
{
	// Lambda for calling splitter on n
	auto _splitter = [&st](const auto& n) -> std::variant<BAs...> {
		return splitter(n, st);
	};

	assert(std::holds_alternative<std::variant<BAs...>>(trim2(n)->value));
	auto ba_constant = get<std::variant<BAs...>>(trim2(n)->value);
	std::variant<BAs...> v = std::visit(_splitter, ba_constant);
	return build_bf_constant<BAs...>(v);
}

} // namespace idni::tau_lang

#endif // __NSO_BA_H__