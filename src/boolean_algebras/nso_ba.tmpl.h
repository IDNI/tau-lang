// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "boolean_algebras/nso_ba.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "nso_ba"

namespace idni::tau_lang {

template <typename... BAs>
requires BAsPack<BAs...>
const tree<node<BAs...>>& operator&(const tree<node<BAs...>>& lt,
	const tree<node<BAs...>>& rt)
{
	using tau = tree<node<BAs...>>;

	auto bf_constant_and = [](const auto& lt, const auto& rt) {
		DBG(assert(lt.get_ba_type() == rt.get_ba_type()
						&& lt.get_ba_type() > 0);)
		return tau::build_bf_ba_constant(
				lt.get_ba_constant() & rt.get_ba_constant(),
				lt.get_ba_type());
	};
	// trivial cases
	if (lt.equals_0() || rt.equals_0()) return tau::get_0();
	if (lt.equals_1()) return rt;
	if (rt.equals_1()) return lt;
	// more elaborate cases
	if (lt.child_is(tau::bf_constant) && rt.child_is(tau::bf_constant))
		return tau::get(bf_constant_and(lt[0], rt[0]));
	if (lt.is(tau::bf) && rt.is(tau::bf))
		return tau::get(tau::build_bf_and(lt.get(), rt.get()));
	if (lt.is(tau::bf) && rt[0].is(tau::bf_eq))
		return tau::get(tau::build_bf_eq((lt & rt[0][0]).get()));
	if (lt.is(tau::bf) && rt[0].is(tau::bf_neq))
		return tau::get(tau::build_bf_neq((lt & rt[0][0]).get()));
	if (lt.is(tau::wff) && rt.is(tau::wff))
		return tau::get(tau::build_wff_and(lt.get(), rt.get()));
	throw std::logic_error("nso_ba and: wrong types");
}

template <typename... BAs>
requires BAsPack<BAs...>
const tree<node<BAs...>>& operator|(const tree<node<BAs...>>& lt,
	const tree<node<BAs...>>& rt)
{
	using tau = tree<node<BAs...>>;

	auto bf_constant_or = [](const auto& lt, const auto& rt) {
		DBG(assert(lt.get_ba_type() == rt.get_ba_type()
						&& lt.get_ba_type() > 0);)
		return tau::build_bf_ba_constant(
				lt.get_ba_constant() | rt.get_ba_constant(),
				lt.get_ba_type());
	};

	if (lt[0].is_ba_constant() && rt[0].is_ba_constant())
		return tau::get(bf_constant_or(lt[0], rt[0]));
	if (lt.is(tau::bf) && rt.is(tau::bf))
		return tau::get(tau::build_bf_or(lt.get(), rt.get()));
	if (lt.is(tau::bf) && rt[0].is(tau::bf_eq))
		return tau::get(tau::build_bf_eq((lt | rt[0][0]).get()));
	if (lt.is(tau::bf) && rt[0].is(tau::bf_neq))
		return tau::get(tau::build_bf_neq((lt | rt[0][0]).get()));
	if (lt.is(tau::wff) && rt.is(tau::wff))
		return tau::get(tau::build_wff_or(lt.get(), rt.get()));
	throw std::logic_error("nso_ba or: wrong types");
}

template <typename... BAs>
requires BAsPack<BAs...>
const tree<node<BAs...>>& operator~(const tree<node<BAs...>>& lt) {
	using tau = tree<node<BAs...>>;

	auto bf_constant_neg = [](const auto& lt) {
		DBG(assert(lt.get_ba_type() > 0);)
		return tau::build_bf_ba_constant(~lt.get_ba_constant(),
					     lt.get_ba_type());
	};

	// trivial cases
	if (lt.equals_0()) return tau::get_1();
	if (lt.equals_1()) return tau::get_0();

	// more elaborate cases
	if (lt[0].is_ba_constant())
		return tau::get(bf_constant_neg(lt[0]));
	if (lt.is(tau::bf))
		return tau::get(tau::build_bf_neg(lt.get()));
	if (lt[0].is(tau::bf_eq))
		return tau::get(tau::build_bf_eq((~lt[0][0]).get()));
	if (lt[0].is(tau::bf_neq))
		return tau::get(tau::build_bf_neq((~lt[0][0]).get()));
	if (lt.is(tau::wff))
		return tau::get(tau::build_wff_neg(lt.get()));
	throw std::logic_error("nso_ba neg: wrong types");
}

template <typename... BAs>
requires BAsPack<BAs...>
const tree<node<BAs...>>& operator^(const tree<node<BAs...>>& lt,
	const tree<node<BAs...>>& rt)
{
	using tau = tree<node<BAs...>>;

	auto bf_constant_xor = [](const auto& lt, const auto& rt) {
		DBG(assert(lt.get_ba_type() == rt.get_ba_type()
						&& lt.get_ba_type() > 0);)
		return tau::build_bf_ba_constant(
				lt.get_ba_constant() ^ rt.get_ba_constant(),
				lt.get_ba_type());
	};

	// trivial cases
	if (lt.equals_0()) return rt;
	if (rt.equals_0()) return lt;

	// more elaborate cases
	if (lt[0].is_ba_constant() && rt[0].is_ba_constant())
		return tau::get(bf_constant_xor(lt[0], rt[0]));
	if (lt.is(tau::bf) && rt.is(tau::bf))
		return tau::get(tau::build_bf_xor(lt.get(), rt.get()));
	if (lt.is(tau::bf) && rt[0].is(tau::bf_eq))
		return tau::get(tau::build_bf_eq((lt ^ rt[0][0]).get()));
	if (lt.is(tau::bf) && rt[0].is(tau::bf_neq))
		return tau::get(tau::build_bf_neq((lt ^ rt[0][0]).get()));
	if (lt.is(tau::wff) && rt.is(tau::wff))
		return tau::get(tau::build_wff_xor(lt.get(), rt.get()));
	throw std::logic_error("nso_ba xor: wrong types");
}

template <typename... BAs>
requires BAsPack<BAs...>
const tree<node<BAs...>>& operator+(const tree<node<BAs...>>& lt,
	const tree<node<BAs...>>& rt)
{
	return lt ^ rt;
}

template <typename... BAs>
requires BAsPack<BAs...>
bool is_zero(const tree<node<BAs...>>& lt) {
	using tau = tree<node<BAs...>>;

	// trivial cases
	if (lt.equals_0()) return true;
	if (lt.equals_1()) return false;

	// more elaborate cases
	if (lt[0].is_ba_constant()) return is_zero(lt[0].get_ba_constant());
	if (lt.is(tau::bf))  return lt.equals_0();
	if (lt.is(tau::wff)) return lt.equals_F();
	DBG(throw std::logic_error("nso_ba is_zero: wrong types");)
	return false;
}

template <typename... BAs>
requires BAsPack<BAs...>
bool is_one(const tree<node<BAs...>>& lt) {
	using tau = tree<node<BAs...>>;

	// trivial cases
	if (lt.equals_0()) return false;
	if (lt.equals_1()) return true;

	// more elaborate cases
	if (lt[0].is_ba_constant()) return is_one(lt[0].get_ba_constant());
	if (lt.is(tau::bf))  return lt.equals_1();
	if (lt.is(tau::wff)) return lt.equals_T();
	DBG(throw std::logic_error("nso_ba is_one: wrong types");)
	return false;
}

// We overload the == operator for tau in order to account for typed constants
template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const tree<node<BAs...>>& lt, const tree<node<BAs...>>& rt) {
	return tree<node<BAs...>>::subtree_equals(lt.get(), rt.get());
}

// Also define != again in terms of ==
template <typename... BAs>
requires BAsPack<BAs...>
bool operator!=(const tree<node<BAs...>>& lt, const tree<node<BAs...>>& rt) {
	return !(lt == rt);
}

// We overload spaceship operator in order to have deterministic operators across
// program runs
// In this comparison typed and non-typed Tau constants are considered different
template <typename... BAs>
requires BAsPack<BAs...>
std::weak_ordering operator<=>(
	const tree<node<BAs...>>& lt,
	const tree<node<BAs...>>& rt)
{
	if (tree<node<BAs...>>::subtree_equals(lt.get(), rt.get()))
		return std::weak_ordering::equivalent;
	return tree<node<BAs...>>::subtree_less(lt.get(), rt.get())
		? std::weak_ordering::less
		: std::weak_ordering::greater;
}

// We list all ordering operators explicitly
template <typename... BAs>
requires BAsPack<BAs...>
bool operator<(const tree<node<BAs...>>& lt, const tree<node<BAs...>>& rt) {
	return (lt <=> rt) < 0;
}
template <typename... BAs>
requires BAsPack<BAs...>
bool operator<=(const tree<node<BAs...>>& lt, const tree<node<BAs...>>& rt) {
	return (lt <=> rt) <= 0;
}
template <typename... BAs>
requires BAsPack<BAs...>
bool operator>(const tree<node<BAs...>>& lt, const tree<node<BAs...>>& rt) {
	return (lt <=> rt) > 0;
}
template <typename... BAs>
requires BAsPack<BAs...>
bool operator>=(const tree<node<BAs...>>& lt, const tree<node<BAs...>>& rt) {
	return (lt <=> rt) >= 0;
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const tree<node<BAs...>>& lt, const bool& rt) {
	return rt ? is_one(lt) : is_zero(lt);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const bool lt, const tree<node<BAs...>>& rt) {
	return rt == lt;
}

} // namespace idni::tau_lang
