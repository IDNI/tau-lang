// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __NSO_BA_H__
#define __NSO_BA_H__

#include "hooks.h"

namespace idni::tau_lang {

template <typename... BAs>
tau<BAs...> operator&(const tau<BAs...>& l,	const tau<BAs...>& r);

template <typename... BAs>
tau<BAs...> operator|(const tau<BAs...>& l, const tau<BAs...>& r);

template <typename... BAs>
tau<BAs...> operator~(const tau<BAs...>& l);

template <typename... BAs>
tau<BAs...> operator^(const tau<BAs...>& l, const tau<BAs...>& r);

template <typename... BAs>
tau<BAs...> operator+(const tau<BAs...>& l, const tau<BAs...>& r);

template <typename... BAs>
bool is_zero(const tau<BAs...>& l);

template <typename... BAs>
bool is_one(const tau<BAs...>& l);

// We overload the == operator for tau in order to store additional data
// which is not taken into account for the quality check
template <typename... BAs>
bool operator==(const tau<BAs...> &l, const tau<BAs...>& r);

template <typename... BAs>
bool operator!=(const tau<BAs...>& l, const tau<BAs...>& r);

template <typename... BAs>
std::weak_ordering operator<=>(const tau<BAs...>& l, const tau<BAs...>& r);

template<typename... BAs>
bool operator<(const tau<BAs...>& l, const tau<BAs...>& r);

template<typename... BAs>
bool operator<=(const tau<BAs...>& l, const tau<BAs...>& r);

template<typename... BAs>
bool operator>(const tau<BAs...>& l, const tau<BAs...>& r);

template<typename... BAs>
bool operator>=(const tau<BAs...>& l, const tau<BAs...>& r);

template <typename... BAs>
bool operator==(const tau<BAs...>& l, const bool& r);

template <typename... BAs>
bool operator==(const bool l, const tau<BAs...>& r);

// Splitter function for a nso tau_parser::bf_constant node holding a BA constant
template <typename... BAs>
tau<BAs...> splitter(const tau<BAs...>& n, splitter_type st = splitter_type::upper);

} // namespace idni::tau_lang


template <typename... BAs>
std::ostream& operator<<(std::ostream& stream,
	const idni::tau_lang::tau<BAs...>& n);

// << for node<tau_sym>
template <typename... BAs>
std::ostream& operator<<(std::ostream& stream,
	const idni::rewriter::node<idni::tau_lang::tau_sym<BAs...>>& n);


// outputs a sp_tau_source_node to a stream, using the stringify transformer
// and assumes that the constants also override operator<<.
std::ostream& operator<<(std::ostream& stream,
			const idni::tau_lang::sp_tau_source_node& n);

// << tau_source_node (make it shared to make use of the previous operator)
std::ostream& operator<<(std::ostream& stream,
					const idni::tau_lang::tau_source_node& n);

#include "nso_ba.tmpl.h"

#endif // __NSO_BA_H__