// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__VARIANT_BA_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__VARIANT_BA_H__

#include <variant>

#include "tau_tree.h"
#include "splitter_types.h"

namespace idni::tau_lang {

template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> operator&(const std::variant<BAs...>& l,
	const std::variant<BAs...>& r)
{
	return std::visit(overloaded(
		[]<typename T>(const T& l, const T& r) -> std::variant<BAs...> {
			return l & r;},
		[](const auto&, const auto&) -> std::variant<BAs...> {
			throw std::logic_error("wrong types");}
	), l, r);
}

template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> operator|(const std::variant<BAs...>& l,
	const std::variant<BAs...>& r)
{
	return std::visit(overloaded(
		[]<typename T>(const T& l, const T& r) -> std::variant<BAs...> {
			return l | r;},
		[](const auto&, const auto&) -> std::variant<BAs...> {
			throw std::logic_error("wrong types");}
	), l, r);
}

template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> operator^(const std::variant<BAs...>& l,
	const std::variant<BAs...>& r)
{
	return std::visit(overloaded(
		[]<typename T>(const T& l, const T& r) -> std::variant<BAs...> {
			return l ^ r;},
		[](const auto&, const auto&) -> std::variant<BAs...> {
			throw std::logic_error("wrong types");}
	), l, r);
}

template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> operator+(const std::variant<BAs...>& l,
	const std::variant<BAs...>& r)
{
	return l ^ r;
}

template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> operator~(const std::variant<BAs...>& l) {
	return std::visit(overloaded(
		[](const auto& l) -> std::variant<BAs...> {
			return ~l;
	}), l);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const std::variant<BAs...>& l, const bool& r) {
	return std::visit(overloaded(
			[&r](const auto& l) -> bool {
				return l == r;
			}
		), l);
}

template <NodeType node>
typename node::constant normalize_ba(const typename node::constant& elem){
	return std::visit(overloaded(
		[](const auto& el) {
			return typename node::constant(normalize(el));
		}
	), elem);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool is_syntactic_one(const std::variant<BAs...>& elem) {
	return std::visit(overloaded(
		[](const auto& el) {
			return is_syntactic_one(el);
		}
	), elem);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool is_syntactic_zero(const std::variant<BAs...>& elem) {
	return std::visit(overloaded(
		[](const auto& el) {
			return is_syntactic_zero(el);
		}
	), elem);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool is_closed(const std::variant<BAs...>& elem) {
	return std::visit(overloaded(
		[](const auto& el) {
			return is_closed(el);
		}
	), elem);
}

template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> splitter_ba(const std::variant<BAs...>& elem,
	splitter_type st)
{
	return std::visit(overloaded(
		[&st](const auto& el) {
			return std::variant<BAs...>(splitter(el, st));
		}
	), elem);
}

template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> splitter_ba(const std::variant<BAs...>& elem) {
	return splitter_ba(elem, splitter_type::upper);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool is_zero(const std::variant<BAs...>& l) {
	return std::visit(overloaded(
		[](const auto& l) -> bool {
			return l == false;
		}
	), l);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool is_one(const std::variant<BAs...>& l) {
	return std::visit(overloaded(
		[](const auto& l) -> bool {
			return l == true;
		}
	), l);
}

template <typename... BAs>
requires BAsPack<BAs...>
tref base_ba_symbol_simplification(tref symbol, const std::variant<BAs...>& v) {
	auto f = [&](const auto& ba_type) -> tref {
		return base_ba_symbol_simplification<BAs...>(symbol, ba_type);
	};
	return std::visit(overloaded(f), v);
}

template <typename... BAs>
requires BAsPack<BAs...>
tref base_ba_term_simplification(tref symbol, const std::variant<BAs...>& v) {
	auto f = [&](const auto& ba_type) -> tref {
		return base_ba_term_simplification<BAs...>(symbol, ba_type);
	};
	return std::visit(overloaded(f), v);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const bool& l, const std::variant<BAs...>& r) {
	return r == l;
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator!=(const std::variant<BAs...>& l, const bool& r) {
	return !(l == r);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator!=(const bool& l, const std::variant<BAs...>& r) {
	return r != l;
}

} // namespace idni::tau_lang

//TODO (MEDIUM) add << for variant_ba

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__VARIANT_BA_H__