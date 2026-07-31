// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <limits>
#include <stdexcept>

#include "boolean_algebras/ba_descriptor.h"
#include "boolean_algebras/ba_pack_traits.h"

namespace idni::tau_lang {

// Parse a plain Bool constant ("0"/"false"/"F" or "1"/"true"/"T").
template <typename... BAs>
requires BAsPack<BAs...>
inline std::optional<typename node<BAs...>::constant_with_type> parse_bool(
		const std::string& src) {
	if (src == "0" || src == "false" || src == "F")
		return typename node<BAs...>::constant_with_type{
			std::variant<BAs...>{ Bool(false) }, ba_descriptor<Bool, node<BAs...>>::type_tree() };
	if (src == "1" || src == "true" || src == "T")
		return typename node<BAs...>::constant_with_type{
			std::variant<BAs...>{ Bool(true) }, ba_descriptor<Bool, node<BAs...>>::type_tree() };
	return {};
}

} // namespace idni::tau_lang

// Bool's descriptor lives here rather than beside bool_ba.h: its parse() names
// parse_bool above, which must be visible in the descriptor's definition
// context.  Bool is not a TAU_BAS plugin, but packs like node<bv, Bool> reach
// the generic dispatcher and so need it described.
#include "boolean_algebras/bool_descriptor.tmpl.h"
// after the descriptor: bool_type/bool_type_id derive from it
#include "boolean_algebras/bool_types.tmpl.h"

namespace idni::tau_lang {

// Generic, descriptor-driven definitions.  Every pack dispatches through these;
// no hand-written per-pack specialization survives.

namespace detail {

template <typename... BAs>
tref default_type_for_pack() {
	using node_t = node<BAs...>;
	tref best = nullptr;
	int best_priority = std::numeric_limits<int>::max();
	(([&] {
		constexpr int priority =
			ba_descriptor<BAs, node_t>::default_type_priority;
		if (priority < best_priority) {
			best_priority = priority;
			best = ba_descriptor<BAs, node_t>::type_tree();
		}
	})(), ...);
	return best;
}

} // namespace detail

template <typename... BAs>
requires BAsPack<BAs...>
bool base_ba_dispatcher<BAs...>::is_syntactic_one(
	const std::variant<BAs...>& elem)
{
	return std::visit([](const auto& x) {
		return ba_descriptor<std::decay_t<decltype(x)>, node_t>
			::is_syntactic_one(x);
	}, elem);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool base_ba_dispatcher<BAs...>::is_syntactic_zero(
	const std::variant<BAs...>& elem)
{
	return std::visit([](const auto& x) {
		return ba_descriptor<std::decay_t<decltype(x)>, node_t>
			::is_syntactic_zero(x);
	}, elem);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool base_ba_dispatcher<BAs...>::is_one(const std::variant<BAs...>& elem) {
	return std::visit([](const auto& x) {
		return ba_descriptor<std::decay_t<decltype(x)>, node_t>::is_one(x);
	}, elem);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool base_ba_dispatcher<BAs...>::is_zero(const std::variant<BAs...>& elem) {
	return std::visit([](const auto& x) {
		return ba_descriptor<std::decay_t<decltype(x)>, node_t>::is_zero(x);
	}, elem);
}

template <typename... BAs>
requires BAsPack<BAs...>
bool base_ba_dispatcher<BAs...>::is_closed(const std::variant<BAs...>& elem) {
	return std::visit([](const auto& x) {
		return ba_descriptor<std::decay_t<decltype(x)>, node_t>::is_closed(x);
	}, elem);
}

template <typename... BAs>
requires BAsPack<BAs...>
std::vector<std::string> base_ba_dispatcher<BAs...>::types() {
	return { ba_descriptor<BAs, node_t>::type_name... };
}

template <typename... BAs>
requires BAsPack<BAs...>
tref base_ba_dispatcher<BAs...>::default_type() {
	return detail::default_type_for_pack<BAs...>();
}

template <typename... BAs>
requires BAsPack<BAs...>
std::string base_ba_dispatcher<BAs...>::one(const tref type_tree) {
	std::optional<std::string> out;
	(void)((ba_descriptor<BAs, node_t>::matches_type(type_tree)
		? (out = ba_descriptor<BAs, node_t>::literal_one(type_tree), true)
		: false) || ...);
	// A type no BA owns -- the untyped type a bf_t carries, for one -- falls
	// back to the pack's Boolean carrier asked about its *own* type, which is
	// what the per-pack chains returned from their trailing clause: "1" from
	// sbf, and equally "1" from bv[1]. Passing type_tree on instead would ask
	// a width-dependent literal about a type carrying no width.
	if constexpr (pack_can_host_bool<node_t>()) if (!out) {
		const tref carrier = pack_bool_carrier_type<node_t>();
		(void)((ba_descriptor<BAs, node_t>::matches_type(carrier)
			? (out = ba_descriptor<BAs, node_t>::literal_one(carrier), true)
			: false) || ...);
	}
	if (!out) throw std::runtime_error("unsupported type for one");
	return *out;
}

template <typename... BAs>
requires BAsPack<BAs...>
std::string base_ba_dispatcher<BAs...>::zero(const tref type_tree) {
	std::optional<std::string> out;
	(void)((ba_descriptor<BAs, node_t>::matches_type(type_tree)
		? (out = ba_descriptor<BAs, node_t>::literal_zero(type_tree), true)
		: false) || ...);
	// See one(): an unowned type falls back to the carrier's own literals.
	if constexpr (pack_can_host_bool<node_t>()) if (!out) {
		const tref carrier = pack_bool_carrier_type<node_t>();
		(void)((ba_descriptor<BAs, node_t>::matches_type(carrier)
			? (out = ba_descriptor<BAs, node_t>::literal_zero(carrier), true)
			: false) || ...);
	}
	if (!out) throw std::runtime_error("unsupported type for zero");
	return *out;
}

template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> base_ba_dispatcher<BAs...>::splitter(
	const std::variant<BAs...>& elem, splitter_type st)
{
	return std::visit([&](const auto& x) -> std::variant<BAs...> {
		using BA = std::decay_t<decltype(x)>;
		return std::variant<BAs...>(
			ba_descriptor<BA, node_t>::splitter(x, st));
	}, elem);
}

template <typename... BAs>
requires BAsPack<BAs...>
tref base_ba_dispatcher<BAs...>::splitter_one(tref type_tree) {
	std::optional<tref> out;
	(void)((ba_descriptor<BAs, node_t>::matches_type(type_tree)
		? (out = ba_descriptor<BAs, node_t>::splitter_one(type_tree), true)
		: false) || ...);
	return out.value_or(nullptr);
}

template <typename... BAs>
requires BAsPack<BAs...>
tref base_ba_dispatcher<BAs...>::unpack_tau_ba(
	const std::variant<BAs...>& elem)
{
	return std::visit([](const auto& x) -> tref {
		return ba_descriptor<std::decay_t<decltype(x)>, node_t>::unpack(x);
	}, elem);
}

// Only the wrapper BA packs a tree back into a constant; the first one that
// declares itself the wrapper and accepts the tree wins.
template <typename node_t, typename First, typename... Rest>
struct pack_tau_ba_one {
	template <typename... PackBAs>
	static std::variant<PackBAs...> go(tref t) {
		if constexpr (is_tau_ba_v<First>) {
			if (auto packed = ba_descriptor<First, node_t>::pack(t))
				return std::variant<PackBAs...>(
					std::in_place_type<First>, std::move(*packed));
		}
		if constexpr (sizeof...(Rest) > 0)
			return pack_tau_ba_one<node_t, Rest...>
				::template go<PackBAs...>(t);
		return std::variant<PackBAs...>{};
	}
};

template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> base_ba_dispatcher<BAs...>::pack_tau_ba(tref t) {
	return pack_tau_ba_one<node_t, BAs...>::template go<BAs...>(t);
}

template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> base_ba_dispatcher<BAs...>::normalize(
	const std::variant<BAs...>& v)
{
	return std::visit([](const auto& x) -> std::variant<BAs...> {
		using BA = std::decay_t<decltype(x)>;
		return std::variant<BAs...>(
			ba_descriptor<BA, node_t>::normalize(x));
	}, v);
}

template <typename... BAs>
requires BAsPack<BAs...>
tref base_ba_dispatcher<BAs...>::simplify_symbol(tref symbol) {
	const size_t ba_type = tau::get(symbol).get_ba_type();
	tref out = symbol;
	(void)((ba_descriptor<BAs, node_t>::owns_type(ba_type)
		? (out = ba_descriptor<BAs, node_t>::simplify_symbol(symbol), true)
		: false) || ...);
	return out;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref base_ba_dispatcher<BAs...>::simplify_term(tref term) {
	const size_t ba_type = tau::get(term).get_ba_type();
	tref out = term;
	(void)((ba_descriptor<BAs, node_t>::owns_type(ba_type)
		? (out = ba_descriptor<BAs, node_t>::simplify_term(term), true)
		: false) || ...);
	return out;
}

// Constant parsing: the owning BA's descriptor parses its own constants.
template <NodeType node>
struct ba_constants_parse;

// Walked recursively rather than folded: constant_with_type holds a variant
// whose tau_ba alternative has a const member, so it is not assignable.
template <typename node_t, typename First, typename... Rest>
struct ba_constants_parse_one {
	static std::optional<typename node_t::constant_with_type> get(
		const std::string& src, tref type_tree)
	{
		if (ba_descriptor<First, node_t>::matches_type(type_tree))
			return ba_descriptor<First, node_t>::parse(src, type_tree);
		if constexpr (sizeof...(Rest) > 0)
			return ba_constants_parse_one<node_t, Rest...>::get(
				src, type_tree);
		return std::nullopt;
	}
};

template <typename... BAs>
struct ba_constants_parse<node<BAs...>> {
	using node_t = node<BAs...>;

	static std::optional<typename node_t::constant_with_type> get(
		const std::string& src, tref type_tree)
	{
		return ba_constants_parse_one<node_t, BAs...>::get(src, type_tree);
	}
};

template <NodeType node>
std::optional<typename node::constant_with_type> ba_constants<node>::get(
	const std::string& constant_source, tref type_tree,
	[[maybe_unused]] const std::string options)
{
	return ba_constants_parse<node>::get(constant_source, type_tree);
}

} // namespace idni::tau_lang