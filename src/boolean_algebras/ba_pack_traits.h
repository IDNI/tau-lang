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
#include <vector>

#include "boolean_algebras/ba_descriptor.h"
#include "utility/tree_types.h"

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

/**
 * @brief Invoke @p visit.template operator()<BA>() for every BA in @p Node's
 *        pack, in pack order.
 *
 * For a fold that must see every BA rather than stop at the first hit --
 * accumulating into a variable @p visit captures by reference, or applying a
 * per-BA side effect. Pack order is also evaluation order: the fold expression
 * sequences left to right, so a visitor threading state from one BA into the
 * next (as @ref pack_preprocess does) sees each step's result in turn.
 */
template <typename Node, typename Visit>
void pack_visit_all(Visit&& visit) {
	using pack = typename Node::bas_tuple;
	[&]<std::size_t... Is>(std::index_sequence<Is...>) {
		(visit.template operator()<std::tuple_element_t<Is, pack>>(), ...);
	}(std::make_index_sequence<std::tuple_size_v<pack>>{});
}

/** @brief `true` when @p BA's descriptor solves formulas itself. */
template <typename Node, typename BA, typename Form>
concept ba_solves = ba_has_descriptor_v<Node, BA>
	&& requires(Form f) { ba_descriptor<BA, Node>::solve(f); };

/** @brief `true` when @p BA's descriptor answers satisfiability itself. */
template <typename Node, typename BA, typename Form>
concept ba_checks_sat = ba_has_descriptor_v<Node, BA>
	&& requires(Form f) { ba_descriptor<BA, Node>::is_sat(f); };

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
concept ba_can_solve_c = ba_has_descriptor_v<Node, BA>
	&& requires(Form f) {
		ba_descriptor<BA, Node>::can_solve(f);
	};

/** @brief `true` when @p BA's descriptor reports a definite sat status. */
template <typename Node, typename BA, typename Form>
concept ba_reports_sat_status = ba_has_descriptor_v<Node, BA>
	&& requires(Form f) { ba_descriptor<BA, Node>::sat_status(f); };

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

/** @brief `true` when @p BA's descriptor flags re-decomposable preprocessing residue. */
template <typename Node, typename BA, typename Form>
concept ba_flags_preprocessing_residue = ba_has_descriptor_v<Node, BA>
	&& requires(Form f) {
		ba_descriptor<BA, Node>::has_preprocessing_residue(f);
	};

/**
 * @brief `true` when some BA in the pack flags @p form as still carrying its
 * own preprocessing residue.
 *
 * Preprocessing (@ref pack_preprocess) can rewrite a formula into a shape
 * that closing its free variables would make expensive to decide again (bv's
 * predicate blasting is the motivating case: closing an already-blasted
 * scope's free variables wraps the auxiliary bit quantifiers it introduced in
 * a universal block, which defeats the solver's usual instantiation strategy
 * and can hang it outright). A BA whose preprocessing has no such trap simply
 * declares nothing, so the fold answers `false` for it -- same "absent means
 * ordinary" convention as every other optional capability here.
 */
template <typename Node, typename Form>
bool pack_has_preprocessing_residue(Form form) {
	bool out = false;
	pack_visit_all<Node>([&]<typename BA>() {
		if constexpr (ba_flags_preprocessing_residue<Node, BA, Form>)
			if (!out) out = ba_descriptor<BA, Node>::has_preprocessing_residue(form);
	});
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
	return pack_first_owner<Node>([&]<typename BA>() -> std::optional<bool> {
		if constexpr (ba_reports_sat_status<Node, BA, Form>)
			return ba_descriptor<BA, Node>::sat_status(form);
		return std::nullopt;
	});
}

/** @brief `true` when @p BA's descriptor preprocesses formulas itself. */
template <typename Node, typename BA, typename Form>
concept ba_preprocesses = ba_has_descriptor_v<Node, BA>
	&& requires(Form f) {
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
	pack_visit_all<Node>([&]<typename BA>() {
		if constexpr (ba_preprocesses<Node, BA, Form>)
			out = ba_descriptor<BA, Node>::preprocess(out);
	});
	return out;
}

/** @brief `true` when @p BA's descriptor classifies a formula's preprocessability. */
template <typename Node, typename BA, typename Form>
concept ba_classifies_preprocessing = ba_has_descriptor_v<Node, BA>
	&& requires(Form f) {
		ba_descriptor<BA, Node>::formula_is_preprocessable(f);
	};

/**
 * @brief `true` when some BA in the pack says its own preprocessing
 * (@ref pack_preprocess) can still make progress on @p form.
 *
 * Formula-level sibling of @ref pack_term_is_blasteable: a BA that offers
 * preprocessing may also classify which formulas it can actually rewrite, so
 * a caller can skip the attempt instead of running preprocessing only to get
 * @p form back unchanged (or, worse, transformed into a shape a later gate
 * cannot make progress on either). No BA in the pack offering the
 * classification means nothing can preprocess this formula, so this answers
 * `false` -- which is the conservative-and-correct choice at the one call
 * site that reads it: a BA that never offered preprocessing in the first
 * place could not have made progress there regardless, so skipping the call
 * skips no step that would otherwise have done something.
 */
template <typename Node, typename Form>
bool pack_formula_is_preprocessable(Form form) {
	bool out = false;
	pack_visit_all<Node>([&]<typename BA>() {
		if constexpr (ba_classifies_preprocessing<Node, BA, Form>)
			if (!out) out = ba_descriptor<BA, Node>::formula_is_preprocessable(form);
	});
	return out;
}

/**
 * @brief Switch every BA with a grammar of its own between var and charvar.
 *
 * A BA that parses its own literals has its own grammar to keep in step with
 * core's; one that does not simply declares nothing.
 */
template <typename Node>
void pack_set_charvar(bool charvar) {
	pack_visit_all<Node>([&]<typename BA>() {
		if constexpr (ba_has_descriptor_v<Node, BA> && requires {
			ba_descriptor<BA, Node>::set_charvar(charvar); })
			ba_descriptor<BA, Node>::set_charvar(charvar);
	});
}

/**
 * @brief Enable or disable every BA's OWN preprocessing switch (bv's
 * `bv_blasting`, addressed as `bv-blasting`).
 *
 * Distinct from the core master `preprocessing` (heuristics/
 * preprocess_placement.h, set by `api::set_preprocessing`): a BA whose
 * descriptor declares `preprocess` still needs the master on as well --
 * see `ba_descriptor<bv,...>::preprocess` (bv_descriptor.tmpl.h).
 */
template <typename Node>
void pack_set_preprocessing(bool enabled) {
	pack_visit_all<Node>([&]<typename BA>() {
		if constexpr (ba_has_descriptor_v<Node, BA> && requires {
			ba_descriptor<BA, Node>::set_preprocessing(enabled); })
			ba_descriptor<BA, Node>::set_preprocessing(enabled);
	});
}

/**
 * @brief `true` when @p BA declares the grammar's arithmetic term operators.
 *
 * Optional capability, so a BA that says nothing simply has none.
 */
template <typename Node, typename BA>
constexpr bool ba_has_arith_ops() {
	if constexpr (ba_has_descriptor_v<Node, BA> && requires {
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
bool pack_type_has_arith_ops(tref type) {
	return pack_type_has_arith_ops_impl<Node>(type);
}

/** @brief `true` when @p BA's descriptor classifies an arithmetic term's blastability. */
template <typename Node, typename BA>
concept ba_classifies_blasting = ba_has_descriptor_v<Node, BA>
	&& requires(tref t) { ba_descriptor<BA, Node>::term_is_blasteable(t); };

/**
 * @brief `true` when the BA owning type id @p ba_type can predicate-blast
 * @p term.
 *
 * A term with an arithmetic operator (`+`, `-`, `*`, `/`, `%`, a shift, a
 * cast) is not automatically blasteable: an operator like `*` needs a
 * constant argument to turn into a per-bit predicate. This asks the term's
 * *owning* BA -- the one `pack_preprocess` would actually route it to --
 * whether it can, for a BA that declares `arith_ops` and offers the
 * classification; a BA with none of either simply cannot blast anything, so
 * this answers `false`. @p ba_type is taken separately from @p term (rather
 * than read off it internally) so this header need not depend on `tree<Node>`
 * -- the caller already has it from classifying the same term.
 */
template <typename Node>
bool pack_term_is_blasteable(size_t ba_type, tref term) {
	bool out = false;
	[&]<std::size_t... Is>(std::index_sequence<Is...>) {
		([&] {
			using BA = std::tuple_element_t<Is, typename Node::bas_tuple>;
			if constexpr (ba_classifies_blasting<Node, BA>)
				if (!out && ba_descriptor<BA, Node>::owns_type(ba_type))
					out = ba_descriptor<BA, Node>::term_is_blasteable(term);
		}(), ...);
	}(std::make_index_sequence<
		std::tuple_size_v<typename Node::bas_tuple>>{});
	return out;
}


/** @brief `true` when @p BA's descriptor builds a canonical zero constant. */
template <typename Node, typename BA>
concept ba_has_zero_constant = ba_has_descriptor_v<Node, BA>
	&& requires(size_t t) {
		ba_descriptor<BA, Node>::zero_constant(t);
	};

/**
 * @brief Canonical zero constant for @p ba_type, from the BA that owns it.
 *
 * Returns nullptr when no BA in the pack owns the type or offers the
 * capability, so callers branch on the result rather than on a BA name. Unlike
 * pack_solve, reaching the empty case here is an ordinary runtime outcome, not
 * a sign that a gate has drifted.
 */
template <typename Node>
tref pack_zero_constant(size_t ba_type) {
	return pack_first_owner<Node>([&]<typename BA>() -> std::optional<tref> {
		if constexpr (ba_has_zero_constant<Node, BA>)
			if (ba_descriptor<BA, Node>::owns_type(ba_type))
				return ba_descriptor<BA, Node>::zero_constant(ba_type);
		return std::nullopt;
	}).value_or(nullptr);
}

/** @brief `true` when @p BA's descriptor builds a constant from a plain value. */
template <typename Node, typename BA>
concept ba_has_value_constant = ba_has_descriptor_v<Node, BA>
	&& requires(size_t t, size_t v) {
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
tref pack_value_constant(size_t ba_type, size_t value) {
	return pack_first_owner<Node>([&]<typename BA>() -> std::optional<tref> {
		if constexpr (ba_has_value_constant<Node, BA>)
			if (ba_descriptor<BA, Node>::owns_type(ba_type))
				return ba_descriptor<BA, Node>::value_constant(
					ba_type, value);
		return std::nullopt;
	}).value_or(nullptr);
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
		const tref* ch, tref r) { \
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
	if constexpr (ba_has_descriptor_v<Node, BA> && requires {
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
tref ba_bool_carrier_type() {
	if constexpr (ba_has_descriptor_v<Node, BA> && requires {
		ba_descriptor<BA, Node>::bool_carrier_type(); })
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
tref pack_bool_carrier_type() {
	static_assert(pack_can_host_bool<Node>(),
		"no BA in this pack declares can_host_bool, so core has no type "
		"to build a plain 0 or 1 in");
	tref out = nullptr;
	int best = -1;
	pack_visit_all<Node>([&]<typename BA>() {
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
	});
	return out;
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

/** @brief `true` when @p BA revises a clause by its own winning region. */
template <typename Node, typename BA>
concept ba_has_semantic_pwr = ba_has_descriptor_v<Node, BA>
	&& requires(tref clause, tref update) {
		ba_descriptor<BA, Node>::semantic_pwr_optimal(clause, update);
	};

/**
 * @brief Revise @p clause against @p update through a BA's winning region.
 *
 * Takes no type id: the capability decides for itself whether the clause is
 * its own, so nullptr covers both "not mine" and "no revision found" -- the
 * caller falls back to the syntactic mode either way.
 */
template <typename Node>
tref pack_semantic_pwr_optimal(tref clause, tref update) {
	return pack_first_owner<Node>([&]<typename BA>() -> std::optional<tref> {
		if constexpr (ba_has_semantic_pwr<Node, BA>)
			return ba_descriptor<BA, Node>
				::semantic_pwr_optimal(clause, update);
		return std::nullopt;
	}).value_or(nullptr);
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

/**
 * @brief `true` when the BA owning @p ba_type is one whose outputs a system
 *        can always satisfy on its own.
 *
 * A claim about the algebra, not about a formula -- "the system can meet
 * `o = p` by setting its output to `p`" -- which is why the capability is a
 * flag. It lets a pure-output formula take existential feasibility instead of
 * the safety fixpoint.
 */
template <typename Node>
bool pack_type_output_always_satisfiable(size_t ba_type) {
	if (!ba_type) return false;
	bool out = false;
	[&]<std::size_t... Is>(std::index_sequence<Is...>) {
		([&] {
			using BA = std::tuple_element_t<Is,
				typename Node::bas_tuple>;
			if constexpr (ba_has_descriptor_v<Node, BA>
				&& requires { requires ba_descriptor<BA, Node>
					::output_always_satisfiable_by_system; })
				if (!out && ba_descriptor<BA, Node>
					::owns_type(ba_type))
						out = true;
		}(), ...);
	}(std::make_index_sequence<
		std::tuple_size_v<typename Node::bas_tuple>>{});
	return out;
}

/** @brief `true` when @p BA's descriptor declares its own CLI/REPL options. */
template <typename Node, typename BA>
concept ba_declares_options = ba_has_descriptor_v<Node, BA>
	&& ba_has_options_v<Node, BA>;

/**
 * @brief One BA-declared option paired with the family name that owns it.
 *
 * @p family is the owning descriptor's `type_name`, so a parameterized
 * family (`bv[8]`, `bv[16]`, ...) shares one option set under `bv`, the same
 * basis @ref pack_owns_ba_type_name matches on.
 */
struct ba_named_option {
	std::string family;
	ba_option option;
};

/**
 * @brief Every BA-declared option in @p Node's pack, paired with its
 * owning family name.
 *
 * Cached: a pack's option set is fixed at compile time. Entries are
 * de-duplicated by (family, option name), since several widths of one
 * parameterized family declare the same list against the same globals.
 */
template <typename Node>
const std::vector<ba_named_option>& pack_ba_options() {
	static const std::vector<ba_named_option> opts = [] {
		std::vector<ba_named_option> out;
		pack_visit_all<Node>([&]<typename BA>() {
			if constexpr (ba_declares_options<Node, BA>) {
				const std::string family =
					ba_descriptor<BA, Node>::type_name;
				for (const ba_option& o :
						ba_descriptor<BA, Node>::options()) {
					bool dup = false;
					for (const auto& e : out)
						if (e.family == family
							&& e.option.name
								== std::string(o.name))
							{ dup = true; break; }
					if (!dup) out.push_back({ family, o });
				}
			}
		});
		return out;
	}();
	return opts;
}

/**
 * @brief Every family name in @p Node's pack, for answering family
 * existence.
 */
template <typename Node>
const std::vector<std::string>& pack_ba_families() {
	static const std::vector<std::string> fams = [] {
		std::vector<std::string> out;
		pack_visit_all<Node>([&]<typename BA>() {
			if constexpr (ba_has_descriptor_v<Node, BA>) {
				const std::string f =
					ba_descriptor<BA, Node>::type_name;
				bool seen = false;
				for (const auto& e : out)
					if (e == f) { seen = true; break; }
				if (!seen) out.push_back(f);
			}
		});
		return out;
	}();
	return fams;
}

/** @brief Outcome of @ref pack_find_ba_option, distinguished for the REPL. */
enum class ba_option_lookup_status {
	no_such_family, ///< no BA in this pack has this family name
	no_such_option, ///< the family exists but declares no such option
	found,
};

/** @brief Result of @ref pack_find_ba_option. */
struct ba_option_lookup_result {
	ba_option_lookup_status status;
	/// Valid only when status == found; points into the static storage
	/// pack_ba_options() owns, so it outlives the call that returned it.
	const ba_option* option = nullptr;
};

/**
 * @brief Resolve a `family-name` REPL/CLI option against @p Node's pack,
 * distinguishing "no such family" from "no such option" so callers can
 * report each on its own.
 */
template <typename Node>
ba_option_lookup_result pack_find_ba_option(const std::string& family,
	const std::string& name)
{
	for (const auto& e : pack_ba_options<Node>())
		if (e.family == family && e.option.name == name)
			return { ba_option_lookup_status::found, &e.option };
	bool in_pack = false;
	for (const auto& f : pack_ba_families<Node>())
		if (f == family) { in_pack = true; break; }
	return { in_pack ? ba_option_lookup_status::no_such_option
		: ba_option_lookup_status::no_such_family, nullptr };
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BA_PACK_TRAITS_H__
