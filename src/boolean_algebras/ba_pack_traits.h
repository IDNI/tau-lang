// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ba_pack_traits.h
 * @brief Traits about a BA pack as a whole, asked without naming a BA.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BA_PACK_TRAITS_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BA_PACK_TRAITS_H__

#include <cstddef>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>

#include "boolean_algebras/ba_descriptor.h"

namespace idni::tau_lang {

/**
 * @brief `true` for the wrapper BA that embeds a whole Tau spec.
 *
 * The primary sits in core so generic dispatch can ask without including the
 * tau plugin; the tau plugin specializes it for `tau_ba<BAs...>`.
 */
template <typename T>
struct is_tau_ba : std::false_type {};

template <typename T>
inline constexpr bool is_tau_ba_v = is_tau_ba<T>::value;

/**
 * @brief `true` for a BA with arithmetic terms and its own decision procedure.
 *
 * The primary sits in core so the normalizer can ask whether the configured
 * pack needs the arithmetic machinery (predicate blasting, the arithmetic
 * skip, the theory solver) at all; a BA that has it specializes this in its
 * own header.
 */
template <typename BA>
struct ba_has_arithmetic_theory : std::false_type {};

template <typename BA>
inline constexpr bool ba_has_arithmetic_theory_v =
	ba_has_arithmetic_theory<BA>::value;

/** @internal @brief Fold of @ref ba_has_arithmetic_theory over a pack. */
template <typename Node, std::size_t... Is>
constexpr bool pack_has_arithmetic_theory_impl(std::index_sequence<Is...>) {
	using pack = typename Node::bas_tuple;
	return (ba_has_arithmetic_theory_v<std::tuple_element_t<Is, pack>>
		|| ...);
}

/** @brief `true` when any BA of @p Node's pack has an arithmetic theory. */
template <typename Node>
inline constexpr bool pack_has_arithmetic_theory_v =
	pack_has_arithmetic_theory_impl<Node>(
		std::make_index_sequence<
			std::tuple_size_v<typename Node::bas_tuple>>{});

/** @brief `true` when @p BA's descriptor solves formulas itself. */
template <typename Node, typename BA, typename Form>
concept ba_solves = requires(Form f) { ba_descriptor<BA, Node>::solve(f); };

/** @brief `true` when @p BA's descriptor answers satisfiability itself. */
template <typename Node, typename BA, typename Form>
concept ba_checks_sat = requires(Form f) { ba_descriptor<BA, Node>::is_sat(f); };

namespace detail {

template <typename Node, typename Form, typename First, typename... Rest>
auto pack_solve_impl(Form form) {
	if constexpr (ba_solves<Node, First, Form>)
		return ba_descriptor<First, Node>::solve(form);
	else if constexpr (sizeof...(Rest) > 0)
		return pack_solve_impl<Node, Form, Rest...>(form);
	else static_assert(sizeof...(Rest) > 0,
		"pack_solve: no BA in this pack provides solve()");
}

template <typename Node, typename Form, typename First, typename... Rest>
bool pack_is_sat_impl(Form form) {
	if constexpr (ba_checks_sat<Node, First, Form>)
		return ba_descriptor<First, Node>::is_sat(form);
	else if constexpr (sizeof...(Rest) > 0)
		return pack_is_sat_impl<Node, Form, Rest...>(form);
	else static_assert(sizeof...(Rest) > 0,
		"pack_is_sat: no BA in this pack provides is_sat()");
}

} // namespace detail

/**
 * @brief Solve @p form with the first BA whose descriptor offers a solver.
 *
 * Templated on @p Form and returning `auto` so core need not name the solution
 * type, which would pull solver headers into these traits.
 */
template <typename Node, typename Form>
auto pack_solve(Form form) {
	return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
		return detail::pack_solve_impl<Node, Form,
			std::tuple_element_t<Is, typename Node::bas_tuple>...>(form);
	}(std::make_index_sequence<
		std::tuple_size_v<typename Node::bas_tuple>>{});
}

/** @brief Ask the first BA whose descriptor offers it whether @p form is sat. */
template <typename Node, typename Form>
bool pack_is_sat(Form form) {
	return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
		return detail::pack_is_sat_impl<Node, Form,
			std::tuple_element_t<Is, typename Node::bas_tuple>...>(form);
	}(std::make_index_sequence<
		std::tuple_size_v<typename Node::bas_tuple>>{});
}

/** @brief `true` when @p BA's descriptor reports what it can solve. */
template <typename Node, typename BA, typename Form>
concept ba_can_solve_c = requires(Form f) {
	ba_descriptor<BA, Node>::can_solve(f);
};

/** @brief `true` when @p BA's descriptor reports a definite sat status. */
template <typename Node, typename BA, typename Form>
concept ba_reports_sat_status = requires(Form f) { ba_descriptor<BA, Node>::sat_status(f); };

/** @brief `true` when some BA in the pack can solve @p form at all. */
template <typename Node, typename Form>
bool pack_can_solve(Form form) {
	bool out = false;
	[&]<std::size_t... Is>(std::index_sequence<Is...>) {
		([&] {
			using BA = std::tuple_element_t<Is, typename Node::bas_tuple>;
			if constexpr (ba_can_solve_c<Node, BA, Form>)
				if (!out) out = ba_descriptor<BA, Node>::can_solve(form);
		}(), ...);
	}(std::make_index_sequence<
		std::tuple_size_v<typename Node::bas_tuple>>{});
	return out;
}

/**
 * @brief Definite satisfiability of @p form, or nullopt when undecided.
 *
 * Deliberately distinct from @ref pack_is_sat: a BA that cannot translate the
 * formula, or whose solver gives up, must report "cannot decide" rather than
 * "not satisfiable", so callers never turn an unknown into a false.
 */
template <typename Node, typename Form>
std::optional<bool> pack_sat_status(Form form) {
	std::optional<bool> out;
	[&]<std::size_t... Is>(std::index_sequence<Is...>) {
		([&] {
			using BA = std::tuple_element_t<Is, typename Node::bas_tuple>;
			if constexpr (ba_reports_sat_status<Node, BA, Form>)
				if (!out) out = ba_descriptor<BA, Node>::sat_status(form);
		}(), ...);
	}(std::make_index_sequence<
		std::tuple_size_v<typename Node::bas_tuple>>{});
	return out;
}

/** @brief `true` when @p BA's descriptor preprocesses formulas itself. */
template <typename Node, typename BA, typename Form>
concept ba_preprocesses = requires(Form f) {
	ba_descriptor<BA, Node>::preprocess(f);
};

/**
 * @brief Run @p form through the preprocessing of every BA that offers it.
 *
 * A BA whose preprocessing is disabled returns @p form unchanged, so callers
 * test the result against the input rather than consulting a flag.
 */
template <typename Node, typename Form>
Form pack_preprocess(Form form) {
	Form out = form;
	[&]<std::size_t... Is>(std::index_sequence<Is...>) {
		([&] {
			using BA = std::tuple_element_t<Is, typename Node::bas_tuple>;
			if constexpr (ba_preprocesses<Node, BA, Form>)
				out = ba_descriptor<BA, Node>::preprocess(out);
		}(), ...);
	}(std::make_index_sequence<
		std::tuple_size_v<typename Node::bas_tuple>>{});
	return out;
}

/** @brief Enable or disable preprocessing on every BA that supports it. */
template <typename Node>
void pack_set_preprocessing(bool enabled) {
	[&]<std::size_t... Is>(std::index_sequence<Is...>) {
		([&] {
			using BA = std::tuple_element_t<Is, typename Node::bas_tuple>;
			if constexpr (requires {
				ba_descriptor<BA, Node>::set_preprocessing(enabled); })
				ba_descriptor<BA, Node>::set_preprocessing(enabled);
		}(), ...);
	}(std::make_index_sequence<
		std::tuple_size_v<typename Node::bas_tuple>>{});
}

/**
 * @brief `true` when @p BA declares the grammar's arithmetic term operators.
 *
 * Optional capability, so a BA that says nothing simply has none.
 */
template <typename Node, typename BA>
constexpr bool ba_has_arith_ops() {
	if constexpr (requires {
		{ ba_descriptor<BA, Node>::arith_ops }
			-> std::convertible_to<bool>; })
		return ba_descriptor<BA, Node>::arith_ops;
	else return false;
}

/** @internal @brief Shared body of the two @ref pack_type_has_arith_ops. */
template <typename Node, typename Type>
bool pack_type_has_arith_ops_impl(Type type) {
	bool out = false;
	[&]<std::size_t... Is>(std::index_sequence<Is...>) {
		([&] {
			using BA = std::tuple_element_t<Is, typename Node::bas_tuple>;
			if constexpr (ba_has_arith_ops<Node, BA>())
				if (!out && ba_descriptor<BA, Node>::owns_type(type))
					out = true;
		}(), ...);
	}(std::make_index_sequence<
		std::tuple_size_v<typename Node::bas_tuple>>{});
	return out;
}

/** @brief `true` when the BA owning type id @p ba_type declares arith_ops. */
template <typename Node>
bool pack_type_has_arith_ops(size_t ba_type) {
	return pack_type_has_arith_ops_impl<Node>(ba_type);
}

/** @brief `true` when the BA owning type tree @p type declares arith_ops. */
template <typename Node>
bool pack_type_has_arith_ops(const intptr_t* type) {
	return pack_type_has_arith_ops_impl<Node>(type);
}


/** @brief `true` when @p BA's descriptor builds a canonical zero constant. */
template <typename Node, typename BA>
concept ba_has_zero_constant = requires(size_t t) {
	ba_descriptor<BA, Node>::zero_constant(t);
};

/**
 * @brief Canonical zero constant for @p ba_type, from the BA that owns it.
 *
 * Returns nullptr when no BA in the pack owns the type or offers the
 * capability, so callers branch on the result rather than on a BA name. Unlike
 * pack_solve, reaching the empty case here is an ordinary runtime outcome, not
 * a sign that a gate has drifted. `tref` is spelled out as `const intptr_t*`,
 * as in ba_descriptor.h, to keep parser headers out of these traits.
 */
template <typename Node>
const intptr_t* pack_zero_constant(size_t ba_type) {
	const intptr_t* out = nullptr;
	[&]<std::size_t... Is>(std::index_sequence<Is...>) {
		([&] {
			using BA = std::tuple_element_t<Is, typename Node::bas_tuple>;
			if constexpr (ba_has_zero_constant<Node, BA>)
				if (!out && ba_descriptor<BA, Node>::owns_type(ba_type))
					out = ba_descriptor<BA, Node>::zero_constant(ba_type);
		}(), ...);
	}(std::make_index_sequence<
		std::tuple_size_v<typename Node::bas_tuple>>{});
	return out;
}

/** @brief `true` when @p BA's descriptor builds a constant from a plain value. */
template <typename Node, typename BA>
concept ba_has_value_constant = requires(size_t t, size_t v) {
	ba_descriptor<BA, Node>::value_constant(t, v);
};

/**
 * @brief Constant of @p ba_type holding @p value, from the BA that owns it.
 *
 * Returns nullptr when no BA owns the type or offers the capability. Lets core
 * ask for "the constant 1 of this type" without knowing how wide it is or how
 * the BA represents it.
 */
template <typename Node>
const intptr_t* pack_value_constant(size_t ba_type, size_t value) {
	const intptr_t* out = nullptr;
	[&]<std::size_t... Is>(std::index_sequence<Is...>) {
		([&] {
			using BA = std::tuple_element_t<Is, typename Node::bas_tuple>;
			if constexpr (ba_has_value_constant<Node, BA>)
				if (!out && ba_descriptor<BA, Node>::owns_type(ba_type))
					out = ba_descriptor<BA, Node>::value_constant(
						ba_type, value);
		}(), ...);
	}(std::make_index_sequence<
		std::tuple_size_v<typename Node::bas_tuple>>{});
	return out;
}

/**
 * @brief `true` when the BA owning @p ba_type is a non-aba omega-categorical BA.
 *
 * Reads the descriptor flag rather than asking whether the pack contains a
 * particular BA, so core states the property it depends on instead of the name
 * of the algebra that happens to have it.
 */
template <typename Node>
bool pack_type_is_non_aba_omcat(size_t ba_type) {
	if (!ba_type) return false;
	bool out = false;
	[&]<std::size_t... Is>(std::index_sequence<Is...>) {
		([&] {
			using BA = std::tuple_element_t<Is, typename Node::bas_tuple>;
			if constexpr (ba_has_descriptor_v<Node, BA>)
				if (!out && ba_descriptor<BA, Node>::non_aba_omcat
					&& ba_descriptor<BA, Node>::owns_type(ba_type))
						out = true;
		}(), ...);
	}(std::make_index_sequence<
		std::tuple_size_v<typename Node::bas_tuple>>{});
	return out;
}

/**
 * @brief Defines `pack_ba_type_has_<mem>_hook<Node>(ba_type)` for one operator.
 *
 * Answers "does the BA owning this type define this comparison at all", which
 * is what a hook needs in order to stop rather than fall through to another
 * type family's handling when the owner declines to fold. Distinct from asking
 * whether the fold produced a result -- an owner that returns nothing still
 * owns the operator. Unlike the reference this resolves the owner through
 * `owns_type(size_t)` rather than a type-tree round-trip, keeping these traits
 * free of ba_types.
 */
#define TAU_PACK_TRAITS_WFF_HOOK(mem) \
	template <typename Node, typename BA> \
	constexpr bool ba_has_##mem##_hook_v = requires( \
		const intptr_t* const* ch, const intptr_t* r) { \
		ba_wff_hooks<BA, Node>::mem(ch, r); \
	}; \
	template <typename Node, typename BA> \
	bool ba_owns_##mem##_hook_type(size_t ba_type) { \
		if constexpr (!ba_has_descriptor_v<Node, BA>) return false; \
		else if constexpr (!ba_has_##mem##_hook_v<Node, BA>) return false; \
		else return ba_descriptor<BA, Node>::owns_type(ba_type); \
	} \
	template <typename Node> \
	bool pack_ba_type_has_##mem##_hook(size_t ba_type) { \
		if (!ba_type) return false; \
		return [&]<std::size_t... Is>(std::index_sequence<Is...>) { \
			using pack = typename Node::bas_tuple; \
			return (ba_owns_##mem##_hook_type<Node, \
				std::tuple_element_t<Is, pack>>(ba_type) || ...); \
		}(std::make_index_sequence< \
			std::tuple_size_v<typename Node::bas_tuple>>{}); \
	}

TAU_PACK_TRAITS_WFF_HOOK(wff_lt)
TAU_PACK_TRAITS_WFF_HOOK(wff_nlt)
TAU_PACK_TRAITS_WFF_HOOK(wff_lteq)
TAU_PACK_TRAITS_WFF_HOOK(wff_nlteq)
TAU_PACK_TRAITS_WFF_HOOK(wff_gt)
TAU_PACK_TRAITS_WFF_HOOK(wff_ngt)
TAU_PACK_TRAITS_WFF_HOOK(wff_gteq)
TAU_PACK_TRAITS_WFF_HOOK(wff_ngteq)

#undef TAU_PACK_TRAITS_WFF_HOOK

/**
 * @brief `true` when @p BA declares one of its types can hold a plain 0 or 1.
 *
 * Optional capability: a BA that does not declare `can_host_bool` simply is not
 * a candidate carrier. Probed rather than required, so core still names no BA
 * when it needs a bit.
 */
template <typename Node, typename BA>
constexpr bool ba_can_host_bool() {
	if constexpr (requires {
		{ ba_descriptor<BA, Node>::can_host_bool }
			-> std::convertible_to<bool>; })
		return ba_descriptor<BA, Node>::can_host_bool;
	else return false;
}

/**
 * @brief Position of @p name in the comma-separated @p order, or -1.
 *
 * The Boolean-carrier preference order arrives as one string from
 * `-DTAU_BOOL_CARRIERS`, so ranking a name means scanning it. Blanks are not
 * skipped; the resolver strips them at configure time.
 */
constexpr int ba_carrier_rank(const char* order, const char* name) {
	int rank = 0;
	for (const char* p = order; ; ++rank) {
		const char* q = name;
		while (*p && *p != ',' && *q && *p == *q) ++p, ++q;
		if (!*q && (!*p || *p == ',')) return rank;
		while (*p && *p != ',') ++p;
		if (!*p) return -1;
		++p;
	}
}

/** @brief @p BA's carrier type, defaulting to its type_tree(). */
template <typename Node, typename BA>
const intptr_t* ba_bool_carrier_type() {
	if constexpr (requires { ba_descriptor<BA, Node>::bool_carrier_type(); })
		return ba_descriptor<BA, Node>::bool_carrier_type();
	else return ba_descriptor<BA, Node>::type_tree();
}

/** @brief `true` when some BA in @p Node's pack can host a Boolean. */
template <typename Node>
constexpr bool pack_can_host_bool() {
	return []<std::size_t... Is>(std::index_sequence<Is...>) {
		return (ba_can_host_bool<Node,
			std::tuple_element_t<Is, typename Node::bas_tuple>>()
				|| ...);
	}(std::make_index_sequence<
		std::tuple_size_v<typename Node::bas_tuple>>{});
}

/**
 * @brief The type core builds a plain 0/1 in — an LTL state bit, a CTL* witness.
 *
 * Resolution is per pack, not per build: of the BAs declaring `can_host_bool`,
 * the one ranking earliest in `TAU_PACK_BOOL_CARRIERS` wins, and pack order
 * decides when the configured order ranks none of them. So one configured
 * order serves the generated pack and every hand-written one, each resolving
 * to what it actually holds. The names live in the configuration, never here.
 *
 * Never null: a pack with nothing to carry a bit fails the static_assert.
 */
template <typename Node>
const intptr_t* pack_bool_carrier_type() {
	static_assert(pack_can_host_bool<Node>(),
		"no BA in this pack declares can_host_bool, so core has no type "
		"to build a plain 0 or 1 in");
	const intptr_t* out = nullptr;
	int best = -1;
	[&]<std::size_t... Is>(std::index_sequence<Is...>) {
		([&] {
			using BA = std::tuple_element_t<Is, typename Node::bas_tuple>;
			if constexpr (ba_can_host_bool<Node, BA>()) {
#ifdef TAU_PACK_BOOL_CARRIERS
				constexpr int rank = ba_carrier_rank(
					TAU_PACK_BOOL_CARRIERS,
					ba_descriptor<BA, Node>::type_name);
#else
				constexpr int rank = -1;
#endif
				if (rank >= 0 && (best < 0 || rank < best))
					best = rank, out = ba_bool_carrier_type<Node, BA>();
				else if (rank < 0 && best < 0 && !out)
					out = ba_bool_carrier_type<Node, BA>();
			}
		}(), ...);
	}(std::make_index_sequence<
		std::tuple_size_v<typename Node::bas_tuple>>{});
	return out;
}

/**
 * @brief Result of @p probe on the first BA of @p Node's pack that returns one.
 *
 * @p probe is invoked as `probe.template operator()<BA>()` for each BA in pack
 * order and must return a `std::optional`; the first engaged result wins.
 */
template <typename Node, typename Probe>
auto pack_first_owner(Probe&& probe) {
	using pack = typename Node::bas_tuple;
	return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
		using result_t = decltype(probe.template operator()<
			std::tuple_element_t<0, pack>>());
		result_t out = std::nullopt;
		(void)((out = probe.template operator()<
			std::tuple_element_t<Is, pack>>(), out.has_value())
				|| ...);
		return out;
	}(std::make_index_sequence<std::tuple_size_v<pack>>{});
}

/** @brief `true` when @p BA eliminates a quantifier over its own theory. */
template <typename Node, typename BA>
concept ba_has_omcat_qe = ba_has_descriptor_v<Node, BA>
	&& requires(tref var, tref body) {
		ba_descriptor<BA, Node>::omcat_qe(var, body);
	};

/**
 * @brief Ask the BA owning @p ba_type_id to eliminate a quantifier over @p var.
 *
 * `true`/`false` answer satisfiability of the quantified @p body; `nullopt` says
 * the theory could not decide it, which is also the answer when no BA owns the
 * type or its owner has no theory-specific elimination -- all three fall through
 * to the atomless-BA path, so core needs no third state.
 */
template <typename Node>
std::optional<bool> pack_omcat_qe(size_t ba_type_id, tref var, tref body) {
	return pack_first_owner<Node>([&]<typename BA>() -> std::optional<bool> {
		if constexpr (ba_has_omcat_qe<Node, BA>)
			if (ba_descriptor<BA, Node>::owns_type(ba_type_id))
				return ba_descriptor<BA, Node>::omcat_qe(
					var, body);
		return std::nullopt;
	});
}

/** @brief `true` when @p BA spells a witness of its own for generated code. */
template <typename Node, typename BA>
concept ba_has_codegen_witness = ba_has_descriptor_v<Node, BA>
	&& requires(tref var, tref conj) {
		ba_descriptor<BA, Node>::codegen_witness(var, conj);
	};

/**
 * @brief A C++ literal satisfying @p conj, from the BA owning @p ba_type_id.
 *
 * The literal's spelling and its type are the owner's, so the emitter places a
 * value it never has to name; nullopt means no owner contributes one.
 */
template <typename Node>
std::optional<std::string> pack_codegen_witness(size_t ba_type_id, tref var,
	tref conj)
{
	return pack_first_owner<Node>([&]<typename BA>()
		-> std::optional<std::string> {
			if constexpr (ba_has_codegen_witness<Node, BA>)
				if (ba_descriptor<BA, Node>::owns_type(ba_type_id))
					return ba_descriptor<BA, Node>
						::codegen_witness(var, conj);
			return std::nullopt;
		});
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BA_PACK_TRAITS_H__
