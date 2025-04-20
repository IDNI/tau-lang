// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "nso_ba.h"

namespace idni::tau_lang {

template <BAsPack... BAs>
const tree<node<BAs...>>& operator&(const tree<node<BAs...>>& lt,
	const tree<node<BAs...>>& rt)
{
	using tau = tree<node<BAs...>>;
	using tt = tau::traverser;

	auto bf_constant_and = [](const auto& lt, const auto& rt) -> tau {
		auto l = tt(lt.get()) | tau::bf_constant;
		auto r = tt(rt.get()) | tau::bf_constant;
		std::variant<BAs...> lc = l | tau::constant | tt::only_child
					| tt::template ba_constant<BAs...>;
		std::variant<BAs...> rc = r | tau::constant | tt::only_child
					| tt::template ba_constant<BAs...>;
		auto type_l = l | tau::type;
		auto type_r = r | tau::type;

		DBG(assert(type_l && type_r);)
		DBG(assert(type_l.value_tree() == type_r.value_tree());)

		return tau::get(tau::build_bf_constant(lc&rc, type_l.value()));
	};

	// trivial cases
	if (lt.value == tau::_0().value
		|| rt.value == tau::_0().value) return tau::_0();
	if (lt.value == tau::_1().value) return rt;
	if (rt.value == tau::_1().value) return lt;

	// more elaborate cases
	if (lt.first_tree().is(tau::bf_constant)
		&& rt.first_tree().is(tau::bf_constant))
			return bf_constant_and(lt, rt);
	if (lt.is(tau::bf) && rt.is(tau::bf))
		return tau::get(tau::build_bf_and(lt.get(), rt.get()));
	if (lt.is(tau::bf) && rt.only_child_tree().is(tau::bf_eq))
		return tau::get(tau::build_wff_eq((lt & rt[0][0]).get()));
	if (lt.is(tau::bf) && rt.only_child_tree().is(tau::bf_neq))
		return tau::get(tau::build_wff_neq((lt & rt[0][0]).get()));
	if (lt.is(tau::wff) && rt.is(tau::wff))
		return tau::get(tau::build_wff_and(lt.get(), rt.get()));
	if (lt.is(tau::wff) && rt.is(tau::wff))
		return tau::get(tau::build_wff_and(lt.get(), rt.get()));
	DBG(throw std::logic_error("nso_ba and: wrong types");)
}

template <BAsPack... BAs>
const tree<node<BAs...>>& operator|(const tree<node<BAs...>>& lt,
	const tree<node<BAs...>>& rt)
{
	using tau = tree<node<BAs...>>;
	using tt = tau::traverser;

	auto bf_constant_or = [](const auto& lt, const auto& rt) -> tau {
		auto l = tt(lt.get()) | tau::bf_constant;
		auto r = tt(rt.get()) | tau::bf_constant;
		std::variant<BAs...> lc = l | tau::constant | tt::only_child
					| tt::template ba_constant<BAs...>;
		std::variant<BAs...> rc = r | tau::constant | tt::only_child
					| tt::template ba_constant<BAs...>;
		auto type_l = l | tau::type;
		auto type_r = r | tau::type;

		DBG(assert(type_l.has_value() && type_r.has_value());)
		DBG(assert(type_l.value_tree().data() == type_r.value_tree().data());)

		return tau::get(tau::build_bf_constant(lc|rc, type_l.value()));
	};

	if (lt.first_tree().is(tau::bf_constant)
		&& rt.first_tree().is(tau::bf_constant))
			return bf_constant_or(lt, rt);
	if (lt.is(tau::bf) && rt.is(tau::bf))
		return tau::get(tau::build_bf_or(lt.get(), rt.get()));
	if (lt.is(tau::bf) && rt.only_child_tree().is(tau::bf_eq))
		return tau::get(tau::build_wff_eq((lt | rt[0][0]).get()));
	if (lt.is(tau::bf) && rt.only_child_tree().is(tau::bf_neq))
		return tau::get(tau::build_wff_neq((lt | rt[0][0]).get()));
	if (lt.is(tau::wff) && rt.is(tau::wff))
		return tau::get(tau::build_wff_or(lt.get(), rt.get()));
	DBG(throw std::logic_error("nso_ba or: wrong types");)
}

template <BAsPack... BAs>
const tree<node<BAs...>>& operator~(const tree<node<BAs...>>& lt) {
	using tau = tree<node<BAs...>>;
	using tt = tau::traverser;

	auto bf_constant_neg = [](const auto& lt) -> tau {
		auto l = tt(lt.get()) | tau::bf_constant;
		std::variant<BAs...> lc = l | tau::constant | tt::only_child
					| tt::template ba_constant<BAs...>;
		auto type_l = l | tau::type;
		DBG(assert(type_l.has_value());)
		return tau::get(
			tau::build_bf_constant((~lc).get(), type_l.value()));
	};

	// trivial cases
	if (lt.value == tau::_0().value) return tau::_1();
	if (lt.value == tau::_1().value) return tau::_0();

	// more elaborate cases
	if (lt.first_tree().is(tau::bf_constant))
		return bf_constant_neg(lt);
	if (lt.first_tree().is(tau::bf_eq))
		return tau::get(tau::build_wff_eq((~lt[0][0]).get()));
	if (lt.first_tree().is(tau::bf_neq))
		return tau::get(tau::build_wff_neq((~lt[0][0]).get()));
	if (lt.first_tree().is(tau::wff))
		return tau::get(tau::build_wff_neg(lt.get()));
	DBG(throw std::logic_error("nso_ba neg: wrong types");)
}

template <BAsPack... BAs>
const tree<node<BAs...>>& operator^(const tree<node<BAs...>>& lt,
	const tree<node<BAs...>>& rt)
{
	using tau = tree<node<BAs...>>;
	using tt = tau::traverser;

	auto bf_constant_xor = [](const auto& lt, const auto& rt) -> tau {
		auto l = tt(lt.get()) | tau::bf_constant;
		auto r = tt(rt.get()) | tau::bf_constant;
		std::variant<BAs...> lc = l | tau::constant | tt::only_child
					| tt::template ba_constant<BAs...>;
		std::variant<BAs...> rc = r | tau::constant | tt::only_child
					| tt::template ba_constant<BAs...>;
		auto type_l = l | tau::type;
		auto type_r = r | tau::type;
		DBG(assert(type_l.has_value() && type_r.has_value());)
		DBG(assert(type_l.value_tree().data() == type_r.value_tree().data());)

		return tau::get(
			tau::build_bf_constant((lc ^ rc).get(), type_l.value()));
	};

	// trivial cases
	if (lt.value == tau::_0().value) return rt;
	if (rt.value == tau::_0().value) return lt;

	// more elaborate cases
	if (lt.first_tree().is(tau::bf_constant)
		&& rt.first_tree().is(tau::bf_constant))
			return bf_constant_xor(lt, rt);
	if (lt.is(tau::bf) && rt.is(tau::bf))
		return tau::get(tau::build_bf_xor(lt.get(), rt.get()));
	if (lt.is(tau::bf) && rt.only_child_tree().is(tau::bf_eq))
		return tau::get(tau::build_wff_eq((lt ^ rt[0][0]).get()));
	if (lt.is(tau::bf) && rt.only_child_tree().is(tau::bf_neq))
		return tau::get(tau::build_wff_neq((lt ^ rt[0][0]).get()));
	if (lt.is(tau::wff) && rt.is(tau::wff))
		return tau::get(tau::build_wff_xor(lt.get(), rt.get()));
	DBG(throw std::logic_error("nso_ba xor: wrong types");)
}

template <BAsPack... BAs>
const tree<node<BAs...>>& operator+(const tree<node<BAs...>>& lt,
	const tree<node<BAs...>>& rt)
{
	return lt ^ rt;
}

template <BAsPack... BAs>
bool is_zero(const tree<node<BAs...>>& lt) {
	using tau = tree<node<BAs...>>;
	using tt = tau::traverser;

	auto bf_constant_is_zero = [](const auto& lt) -> bool {
		auto l = tt(lt.get()) | tau::bf_constant;
		std::variant<BAs...> lc = l | tau::constant | tt::only_child
					| tt::template ba_constant<BAs...>;
		return is_zero(lc);
	};

	// trivial cases
	if (lt.value == tau::_0().value) return true;
	if (lt.value == tau::_1().value) return false;

	// more elaborate cases
	if (lt.first_tree().is(tau::bf_constant))
		return bf_constant_is_zero(lt);
	if (lt.is(tau::bf))  return lt == tau::_0();
	if (lt.is(tau::wff)) return lt == tau::_F();
	DBG(throw std::logic_error("nso_ba is_zero: wrong types");)
}

template <BAsPack... BAs>
bool is_one(const tree<node<BAs...>>& lt) {
	using tau = tree<node<BAs...>>;
	using tt = tau::traverser;

	auto bf_constant_is_one = [](const auto& lt) -> bool {
		auto l = tt(lt.get()) | tau::bf_constant;
		std::variant<BAs...> lc = l | tau::constant | tt::only_child
					| tt::template ba_constant<BAs...>;
		return is_one(lc);
	};

	// trivial cases
	if (lt.value == tau::_0().value) return false;
	if (lt.value == tau::_1().value) return true;

	// more elaborate cases
	if (lt.first_tree().is(tau::bf_constant))
		return bf_constant_is_one(lt);
	if (lt.is(tau::bf))  return lt == tau::_1;
	if (lt.is(tau::wff)) return lt == tau::_T;
	DBG(throw std::logic_error("nso_ba is_one: wrong types");)
}

// We overload the == operator for tau in order to account for typed constants
template <BAsPack... BAs>
bool operator==(const tree<node<BAs...>>& lt, const tree<node<BAs...>>& rt) {
	return lt == rt;
}

// Also define != again in terms of ==
template <BAsPack... BAs>
bool operator!=(const tree<node<BAs...>>& lt, const tree<node<BAs...>>& rt) {
	return !(lt == rt);
}

// We overload spaceship operator in order to have deterministic operators across
// program runs
// In this comparison typed and non-typed Tau constants are considered different
template <BAsPack... BAs>
std::weak_ordering operator<=>(const tree<node<BAs...>>& lt,
	const tree<node<BAs...>>& rt)
{
	return lt <=> rt;
}

// We list all ordering operators explicitly
template <BAsPack... BAs>
bool operator<(const tree<node<BAs...>>& lt, const tree<node<BAs...>>& rt) {
	return (lt <=> rt) < 0;
}
template <BAsPack... BAs>
bool operator<=(const tree<node<BAs...>>& lt, const tree<node<BAs...>>& rt) {
	return (lt <=> rt) <= 0;
}
template <BAsPack... BAs>
bool operator>(const tree<node<BAs...>>& lt, const tree<node<BAs...>>& rt) {
	return (lt <=> rt) > 0;
}
template <BAsPack... BAs>
bool operator>=(const tree<node<BAs...>>& lt, const tree<node<BAs...>>& rt) {
	return (lt <=> rt) >= 0;
}

template <BAsPack... BAs>
bool operator==(const tree<node<BAs...>>& lt, const bool& rt) {
	return rt ? is_one(lt) : is_zero(lt);
}

template <BAsPack... BAs>
bool operator==(const bool lt, const tree<node<BAs...>>& rt) {
	return rt == lt;
}

// Splitter function for a nso tau_parser::bf_constant node holding a BA constant
template <BAsPack... BAs>
const tree<node<BAs...>>& splitter(const tree<node<BAs...>>& t,
	splitter_type st)
{
	using tau = tree<node<BAs...>>;
	using tt = tau::traverser;

	// Lambda for calling splitter on n
	auto _splitter = [&st](const auto& t) -> std::variant<BAs...> {
		return splitter(t, st);
	};
	DBG(assert(t.is(tau::bf_constant));)
	auto c = ba_constants<BAs...>::get(t.get_ba_constant_id());
	std::variant<BAs...> v = std::visit(_splitter, c);

	auto type = tt(t.get()) | tau::type;
	DBG(assert(type.has_value());)
	return tau::get(tau::build_bf_constant(v, type.value()));
}

} // namespace idni::tau_lang
