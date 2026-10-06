// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ba_pack_traits.h
 * @brief Traits about a BA pack as a whole, asked without naming a BA.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BA_PACK_TRAITS_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BA_PACK_TRAITS_H__

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
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

/** @brief Shorthand for `is_tau_ba<T>::value`. */
template <typename T>
inline constexpr bool is_tau_ba_v = is_tau_ba<T>::value;

/** @brief `true` when some BA of @p Node's pack is the wrapper. */
template <typename Node>
inline constexpr bool pack_has_tau_ba_v =
	[]<std::size_t... Is>(std::index_sequence<Is...>) {
		return (is_tau_ba_v<std::tuple_element_t<Is,
			typename Node::bas_tuple>> || ...);
	}(std::make_index_sequence<std::tuple_size_v<typename Node::bas_tuple>>{});

/**
 * @brief `true` when @p BA brings arithmetic terms and its own decision
 *        procedure: exactly the two capabilities the arithmetic pipeline
 *        (predicate blasting, the arithmetic skip, the theory solver)
 *        dispatches on, so the gate and the dispatch cannot disagree.
 */
template <typename Node, typename BA>
inline constexpr bool ba_has_arithmetic_theory_v =
	ba_arith_ops_v<Node, BA> && ba_has_solve<Node, BA>;

/** @internal @brief Fold of @ref ba_has_arithmetic_theory_v over a pack. */
template <typename Node, std::size_t... Is>
constexpr bool pack_has_arithmetic_theory_impl(std::index_sequence<Is...>) {
	using pack = typename Node::bas_tuple;
	return (ba_has_arithmetic_theory_v<Node, std::tuple_element_t<Is, pack>>
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
 * @return The first engaged result, or nullopt when no BA's probe answers.
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

/**
 * @brief Apply @p f to the first BA of @p Node's pack that owns @p ba_type.
 *
 * @p f is invoked as `f.template operator()<BA>()` and returns a
 * `std::optional`; the fold stops at the owner whether or not its result is
 * engaged, and answers nullopt when no BA owns the type -- the ordinary
 * "type nothing in the pack owns" outcome, which the null id shares. A BA
 * lacking the capability the caller wants is skipped by the caller's own
 * `if constexpr`, so the fold never instantiates a member for a BA without
 * it.
 * @param ba_type Type id to resolve; 0 (the null id) owns nothing.
 * @param f The per-BA callable, invoked for the owner only.
 * @return The owner's result, or nullopt when no BA owns @p ba_type.
 */
template <typename Node, typename F>
auto pack_owner_apply(size_t ba_type, F&& f) {
	using pack = typename Node::bas_tuple;
	using result_t = decltype(f.template operator()<
		std::tuple_element_t<0, pack>>());
	result_t out = std::nullopt;
	if (!ba_type) return out;
	[&]<std::size_t... Is>(std::index_sequence<Is...>) {
		bool done = false;
		// A lambda called inside the fold pattern crashes clang 17 and 19.
		auto step = [&]<std::size_t I>() {
			using BA = std::tuple_element_t<I, pack>;
			if (done) return;
			if constexpr (ba_has_descriptor_v<Node, BA>)
				if (ba_descriptor<BA, Node>::owns_type(ba_type)) {
					done = true;
					out = f.template operator()<BA>();
				}
		};
		(step.template operator()<Is>(), ...);
	}(std::make_index_sequence<std::tuple_size_v<pack>>{});
	return out;
}

/**
 * @brief Position in @p Node's pack of the BA owning @p ba_type, or nullopt
 *        when no BA owns it.
 *
 * Two type ids answer the same position exactly when one BA owns both (every
 * width of a parameterised family), which is how a caller groups the atoms
 * one BA's solver must see together.
 */
template <typename Node>
std::optional<size_t> pack_owner_index(size_t ba_type) {
	using pack = typename Node::bas_tuple;
	std::optional<size_t> out;
	if (!ba_type) return out;
	[&]<std::size_t... Is>(std::index_sequence<Is...>) {
		// A lambda called inside the fold pattern crashes clang 17 and 19.
		auto step = [&]<std::size_t I>() {
			using BA = std::tuple_element_t<I, pack>;
			if (out) return;
			if constexpr (ba_has_descriptor_v<Node, BA>)
				if (ba_descriptor<BA, Node>::owns_type(ba_type)) out = I;
		};
		(step.template operator()<Is>(), ...);
	}(std::make_index_sequence<std::tuple_size_v<pack>>{});
	return out;
}

/**
 * @brief Solve @p form, whose atoms are of the type @p ba_type, with the
 *        solver of the BA owning that type.
 *
 * The value is nullopt when no BA owns @p ba_type, when its owner declares
 * no `solve`, or when the owner's solver finds no solution: the caller
 * treats all three as "not solved here". An error is the owner's report of
 * why it could not try, never "no solution". @p Solution is the caller's
 * solution type, so these traits need no solver header; the value of the
 * owner's answer must convert to `std::optional<Solution>`.
 * @param ba_type Type id of the atoms of @p form.
 * @param form The formula to solve, passed to the owner's `solve`.
 */
template <typename Node, typename Solution, typename Form>
result<std::optional<Solution>> pack_solve(size_t ba_type, Form form) {
	using answer_t = result<std::optional<Solution>>;
	auto out = pack_owner_apply<Node>(ba_type,
		[&]<typename BA>() -> std::optional<answer_t> {
			if constexpr (ba_has_solve<Node, BA>) {
				using owner_t = typename decltype(
					ba_descriptor<BA, Node>::solve(form))::value_type;
				static_assert(std::is_convertible_v<owner_t,
						std::optional<Solution>>,
					"pack_solve: the owner's solve() answer does not "
					"convert to the caller's solution type");
				return ba_descriptor<BA, Node>::solve(form).transform(
					[](owner_t&& v) -> std::optional<Solution> {
						return std::move(v); });
			}
			return std::nullopt;
		});
	if (!out) return answer_t{ std::optional<Solution>{} };
	return std::move(*out);
}

/** @brief `true` when some BA in the pack can solve @p form at all. */
template <typename Node, typename Form>
bool pack_can_solve(Form form) {
	bool out = false;
	pack_visit_all<Node>([&]<typename BA>() {
		if constexpr (ba_has_can_solve<Node, BA>)
			if (!out) out = ba_descriptor<BA, Node>::can_solve(form);
	});
	return out;
}

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
		if constexpr (ba_has_preprocessing_residue<Node, BA>)
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
		if constexpr (ba_has_sat_status<Node, BA>)
			return ba_descriptor<BA, Node>::sat_status(form);
		return std::nullopt;
	});
}

// The BA-naming lambda must live in its own function template: MSVC
// instantiates a lambda inside a discarded `if constexpr` branch anyway and
// errors with C2039. Called only from the true branch, this one is
// instantiated for the declaring BA alone.
/// @internal Runs @p BA's `preprocess` on the value of @p out; an error in
/// @p out is passed through untouched. @endinternal
template <typename Node, typename Form, typename BA>
result<Form> pack_preprocess_one(result<Form>&& out) {
	return std::move(out).and_then([](Form f) -> result<Form> {
		return ba_descriptor<BA, Node>::preprocess(f);
	});
}

/**
 * @brief Run @p form through the preprocessing of every BA that offers it.
 *
 * A BA whose preprocessing is disabled (or that offers none) returns its
 * input unchanged, so callers test the result against the input rather than
 * consulting a flag. The chain stops at the first declaring BA whose
 * preprocess fails, carrying that report forward instead of running the rest
 * on a formula that never got fixed up.
 * @return The preprocessed formula, or the first failing BA's error.
 */
template <typename Node, typename Form>
result<Form> pack_preprocess(Form form) {
	result<Form> out{form};
	pack_visit_all<Node>([&]<typename BA>() {
		if constexpr (ba_has_preprocess<Node, BA>)
			out = pack_preprocess_one<Node, Form, BA>(std::move(out));
	});
	return out;
}

/**
 * @brief Run @p form through the case-split quantifier elimination of every
 * BA that offers it.
 *
 * No BA in the pack declaring the capability means nothing case-splits this
 * formula, so this returns @p form unchanged -- the same "absent means
 * ordinary" convention as @ref pack_preprocess.
 */
template <typename Node, typename Form>
Form pack_case_split_quantifiers(Form form) {
	Form out = form;
	pack_visit_all<Node>([&]<typename BA>() {
		if constexpr (ba_has_case_split_quantifiers<Node, BA>)
			out = ba_descriptor<BA, Node>::case_split_quantifiers(out);
	});
	return out;
}

/**
 * @brief Run @p form through the definitional-existential elimination of
 * every BA that offers it (before the case split); absent means ordinary,
 * as for @ref pack_case_split_quantifiers.
 */
template <typename Node, typename Form>
Form pack_eliminate_definitional_existentials(Form form) {
	Form out = form;
	pack_visit_all<Node>([&]<typename BA>() {
		if constexpr (ba_has_eliminate_definitional_existentials<Node, BA>)
			out = ba_descriptor<BA, Node>::eliminate_definitional_existentials(out);
	});
	return out;
}

// See pack_preprocess_one for why the BA-naming lambda sits in its own
// function template.
/// @internal Runs @p BA's `widen_arithmetic` on the value of @p out; an error
/// in @p out is passed through untouched. @endinternal
template <typename Node, typename Form, typename BA>
result<Form> pack_widen_arithmetic_one(result<Form>&& out) {
	return std::move(out).and_then([](Form f) -> result<Form> {
		return ba_descriptor<BA, Node>::widen_arithmetic(f);
	});
}

/**
 * @brief Elaborate @p form's arithmetic atoms through the exact-width
 * widening of every BA that offers it.
 *
 * No BA in the pack declaring the capability means no BA widens this
 * formula, so this returns @p form unchanged -- the same "absent means
 * ordinary" convention as @ref pack_preprocess. Stops the chain and carries
 * the failing report forward the same way, too.
 * @return The widened formula, which a BA may answer as nullptr when an atom
 * exceeds its width cap; or the first failing BA's error.
 */
template <typename Node, typename Form>
result<Form> pack_widen_arithmetic(Form form) {
	result<Form> out{form};
	pack_visit_all<Node>([&]<typename BA>() {
		if constexpr (ba_has_widen_arithmetic<Node, BA>)
			out = pack_widen_arithmetic_one<Node, Form, BA>(
				std::move(out));
	});
	return out;
}

/**
 * @brief Whether any BA's arithmetic widening is currently active.
 *
 * A cache whose entries are built by ordinary node construction (which runs
 * a widening-capable BA's own fit-gated folding hooks, not just @ref
 * pack_widen_arithmetic) must key on this, or a stale entry built under one
 * setting is replayed after the setting flips. False when no BA in the pack
 * declares the capability.
 */
template <typename Node>
bool pack_widening_active() {
	bool active = false;
	pack_visit_all<Node>([&]<typename BA>() {
		if constexpr (ba_has_widening_state<Node, BA>)
			active = active || ba_descriptor<BA, Node>::widening_state();
	});
	return active;
}

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
		if constexpr (ba_has_formula_is_preprocessable<Node, BA>)
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
		if constexpr (ba_has_set_charvar<Node, BA>)
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
		if constexpr (ba_has_set_preprocessing<Node, BA>)
			ba_descriptor<BA, Node>::set_preprocessing(enabled);
	});
}

/**
 * @brief Set tau's OWN component-factoring switch, owned by tau_ba.h.
 *
 * Optional: a pack without tau leaves this a silent no-op, same shape
 * as @ref pack_set_preprocessing.
 */
template <typename Node>
void pack_set_ba_component_factoring(bool state) {
	pack_visit_all<Node>([&]<typename BA>() {
		if constexpr (ba_has_component_factoring<Node, BA>)
			ba_descriptor<BA, Node>::set_ba_component_factoring(state);
	});
}

/**
 * @brief Set the cap on pinned decided rows, owned by tau_ba.h.
 *
 * Optional: a pack without tau leaves this a silent no-op, same shape
 * as @ref pack_set_ba_component_factoring.
 */
template <typename Node>
void pack_set_ba_decision_pins(size_t n) {
	pack_visit_all<Node>([&]<typename BA>() {
		if constexpr (ba_has_decision_pins<Node, BA>)
			ba_descriptor<BA, Node>::set_ba_decision_pins(n);
	});
}

/**
 * @brief Read the cap on pinned decided rows, owned by tau_ba.h.
 *
 * Optional: a pack without tau pins nothing, so 0 is the answer.
 * @return The cap of the last pack member declaring one (the option, else
 * `TAU_BA_DECISION_PINS`, else 4096 for tau), or 0 when none declares it.
 */
template <typename Node>
size_t pack_ba_decision_pins() {
	size_t n = 0;
	pack_visit_all<Node>([&]<typename BA>() {
		if constexpr (ba_has_decision_pins<Node, BA>)
			n = ba_descriptor<BA, Node>::ba_decision_pins();
	});
	return n;
}

/**
 * @brief Read tau's OWN component-factoring switch, owned by tau_ba.h.
 *
 * Optional: a pack without tau declares nothing, so `false` is the answer --
 * same "absent means no" convention as @ref pack_has_preprocessing_residue.
 */
template <typename Node>
bool pack_ba_component_factoring_enabled() {
	bool out = false;
	pack_visit_all<Node>([&]<typename BA>() {
		if constexpr (ba_has_component_factoring<Node, BA>)
			if (!out) out = ba_descriptor<BA, Node>::ba_component_factoring_enabled();
	});
	return out;
}

/** @internal @brief Shared body of the two @ref pack_type_has_arith_ops. */
template <typename Node, typename Type>
bool pack_type_has_arith_ops_impl(Type type) {
	if constexpr (std::is_same_v<Type, size_t>) if (!type) return false;
	bool out = false;
	pack_visit_all<Node>([&]<typename BA>() {
		if constexpr (ba_arith_ops_v<Node, BA>) {
			if (out) return;
			if constexpr (std::is_same_v<Type, size_t>)
				out = ba_descriptor<BA, Node>::owns_type(type);
			else out = ba_descriptor<BA, Node>::matches_type(type);
		}
	});
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
	return pack_owner_apply<Node>(ba_type, [&]<typename BA>()
		-> std::optional<bool> {
			if constexpr (ba_has_term_is_blasteable<Node, BA>)
				return ba_descriptor<BA, Node>::term_is_blasteable(term);
			return std::nullopt;
		}).value_or(false);
}

/**
 * @brief `true` when the BA owning type id @p ba_type declares atomless.
 *
 * Gates the atomless-only inequality-system shortcut in `solve_inequality_
 * system` (solver.tmpl.h): `atomless` is a mandatory descriptor member, so an
 * owner that answers at all always has an opinion -- the empty case (no owner)
 * is the ordinary "ask about a type nothing in the pack owns" outcome.
 */
template <typename Node>
bool pack_type_is_atomless(size_t ba_type) {
	return pack_owner_apply<Node>(ba_type, []<typename BA>()
		-> std::optional<bool> {
			return ba_descriptor<BA, Node>::atomless;
		}).value_or(false);
}

/**
 * @brief `true` when a BA of the pack owns type id @p ba_type and does not
 * declare it atomless.
 *
 * Not the negation of @ref pack_type_is_atomless: a type no BA owns is
 * neither. Guards the laws that hold only in an atomless Boolean algebra.
 */
template <typename Node>
bool pack_type_is_atomic(size_t ba_type) {
	return pack_owner_apply<Node>(ba_type, []<typename BA>()
		-> std::optional<bool> {
			return !ba_descriptor<BA, Node>::atomless;
		}).value_or(false);
}

/**
 * @brief Canonical zero constant for @p ba_type, from the BA that owns it.
 *
 * Returns nullptr when no BA in the pack owns the type or offers the
 * capability, so callers branch on the result rather than on a BA name.
 */
template <typename Node>
tref pack_zero_constant(size_t ba_type) {
	return pack_owner_apply<Node>(ba_type, [&]<typename BA>()
		-> std::optional<tref> {
			if constexpr (ba_has_zero_constant<Node, BA>)
				return ba_descriptor<BA, Node>::zero_constant(ba_type);
			return std::nullopt;
		}).value_or(nullptr);
}

/**
 * @brief Constant of @p ba_type holding @p value, from the BA that owns it.
 *
 * Returns nullptr when no BA owns the type or offers the capability. Lets core
 * ask for "the constant 1 of this type" without knowing how wide it is or how
 * the BA represents it.
 */
template <typename Node>
tref pack_value_constant(size_t ba_type, size_t value) {
	return pack_owner_apply<Node>(ba_type, [&]<typename BA>()
		-> std::optional<tref> {
			if constexpr (ba_has_value_constant<Node, BA>)
				return ba_descriptor<BA, Node>::value_constant(
					ba_type, value);
			return std::nullopt;
		}).value_or(nullptr);
}

/**
 * @brief The width n when the values of @p ba_type are the integers
 * 0 .. 2^n - 1 under unsigned modular semantics, from the BA that owns it.
 *
 * 0 when no BA owns the type, its owner does not declare the capability, or
 * the owner answers that the type is not read that way right now.
 */
template <typename Node>
size_t pack_modular_width(size_t ba_type) {
	return pack_owner_apply<Node>(ba_type, [&]<typename BA>()
		-> std::optional<size_t> {
			if constexpr (ba_has_modular_bits<Node, BA>)
				return ba_descriptor<BA, Node>::modular_width(ba_type);
			return std::nullopt;
		}).value_or(0);
}

/**
 * @brief Whether the BA owning @p ba_type decides closed formulas over it
 * whatever their quantifier prefix (see ba_has_closed_decision).
 */
template <typename Node>
bool pack_type_decides_closed(size_t ba_type) {
	return pack_owner_apply<Node>(ba_type, [&]<typename BA>()
		-> std::optional<bool> {
			if constexpr (ba_has_closed_decision<Node, BA>)
				return true;
			return std::nullopt;
		}).value_or(false);
}

/**
 * @brief The truth of the closed formula @p form over @p ba_type, decided by
 * the BA owning the type with no quantifier eliminated first; nullopt when
 * undecided or when the owner does not decide closed formulas.
 */
template <typename Node>
std::optional<bool> pack_decide_closed(size_t ba_type, tref form) {
	return pack_owner_apply<Node>(ba_type, [&]<typename BA>()
		-> std::optional<bool> {
			if constexpr (ba_has_closed_decision<Node, BA>)
				return ba_descriptor<BA, Node>::decide_closed(form);
			return std::nullopt;
		});
}

/**
 * @brief The truth of the formula @p form without variables, streams or
 * temporal operators, all of whose constants are of @p ba_type, decided by
 * the BA owning the type; nullopt when undecided or when the owner does not
 * decide such formulas (see ba_has_ground_decision).
 */
template <typename Node>
std::optional<bool> pack_decide_ground(size_t ba_type, tref form) {
	return pack_owner_apply<Node>(ba_type, [&]<typename BA>()
		-> std::optional<bool> {
			if constexpr (ba_has_ground_decision<Node, BA>)
				return ba_descriptor<BA, Node>::decide_ground(form);
			return std::nullopt;
		});
}

/**
 * @brief The integer the constant @p c of @p ba_type holds, when the type is
 * read with modular semantics (see pack_modular_width); nullopt otherwise.
 */
template <typename Node>
std::optional<uint64_t> pack_modular_value(size_t ba_type, tref c) {
	return pack_owner_apply<Node>(ba_type, [&]<typename BA>()
		-> std::optional<uint64_t> {
			if constexpr (ba_has_modular_bits<Node, BA>)
				return ba_descriptor<BA, Node>::modular_value(ba_type, c);
			return std::nullopt;
		});
}

/**
 * @brief `true` when the values of @p ba_type form a dense linear order
 * without endpoints, read by = and the order comparisons.
 */
template <typename Node>
bool pack_type_is_dense_order(size_t ba_type) {
	return pack_owner_apply<Node>(ba_type, []<typename BA>()
		-> std::optional<bool> {
			return ba_has_dense_order<Node, BA>;
		}).value_or(false);
}

/**
 * @brief The order of the constants @p a and @p b of a dense-order type:
 * -1, 0 or 1; nullopt when either is not a point of the order or the type
 * is not one.
 */
template <typename Node>
std::optional<int> pack_dense_order_compare(size_t ba_type, tref a, tref b) {
	return pack_owner_apply<Node>(ba_type, [&]<typename BA>()
		-> std::optional<int> {
			if constexpr (ba_has_dense_order<Node, BA>)
				return ba_descriptor<BA, Node>::dense_order_compare(
					ba_type, a, b);
			return std::nullopt;
		});
}

/**
 * @brief A point of the dense-order type @p ba_type above @p lo and below
 * @p hi (nullptr for no bound); nullptr when a bound is not a point or the
 * type is not a dense order.
 */
template <typename Node>
tref pack_dense_order_between(size_t ba_type, tref lo, tref hi) {
	return pack_owner_apply<Node>(ba_type, [&]<typename BA>()
		-> std::optional<tref> {
			if constexpr (ba_has_dense_order<Node, BA>)
				return ba_descriptor<BA, Node>::dense_order_between(
					ba_type, lo, hi);
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
	return pack_owner_apply<Node>(ba_type, []<typename BA>()
		-> std::optional<bool> {
			return ba_descriptor<BA, Node>::non_aba_omcat;
		}).value_or(false);
}

/**
 * @brief Defines `pack_ba_type_has_<mem>_hook<Node>(ba_type)` for one operator.
 *
 * Answers "does the BA owning this type define this comparison at all", which
 * is what a hook needs in order to stop rather than fall through to another
 * type family's handling when the owner declines to fold. Distinct from asking
 * whether the fold produced a result -- an owner that returns nothing still
 * owns the operator. It resolves the owner through `owns_type(size_t)` rather
 * than a type-tree round-trip, keeping these traits free of ba_types. Also
 * defines the probe `ba_has_<mem>_hook_v<Node, BA>`.
 */
#define TAU_PACK_TRAITS_WFF_HOOK(mem) \
	template <typename Node, typename BA> \
	constexpr bool ba_has_##mem##_hook_v = requires( \
		const tref* ch, tref r) { \
		ba_wff_hooks<BA, Node>::mem(ch, r); \
	}; \
	template <typename Node> \
	bool pack_ba_type_has_##mem##_hook(size_t ba_type) { \
		return pack_owner_apply<Node>(ba_type, []<typename BA>() \
			-> std::optional<bool> { \
				return ba_has_##mem##_hook_v<Node, BA>; \
			}).value_or(false); \
	}

TAU_PACK_TRAITS_WFF_HOOK(wff_lt)
TAU_PACK_TRAITS_WFF_HOOK(wff_nlt)
TAU_PACK_TRAITS_WFF_HOOK(wff_lteq)
TAU_PACK_TRAITS_WFF_HOOK(wff_nlteq)
TAU_PACK_TRAITS_WFF_HOOK(wff_gt)
TAU_PACK_TRAITS_WFF_HOOK(wff_ngt)
TAU_PACK_TRAITS_WFF_HOOK(wff_gteq)
TAU_PACK_TRAITS_WFF_HOOK(wff_ngteq)
TAU_PACK_TRAITS_WFF_HOOK(wff_eq)
TAU_PACK_TRAITS_WFF_HOOK(wff_neq)

#undef TAU_PACK_TRAITS_WFF_HOOK

/**
 * @brief Position of @p name in the comma-separated @p order, or -1.
 *
 * The Boolean-carrier preference order arrives as one string from
 * `-DTAU_BOOL_CARRIERS` (as the macro `TAU_PACK_BOOL_CARRIERS`), so ranking a
 * name means scanning it. Blanks are not skipped; the resolver strips them at
 * configure time.
 * @param order Comma-separated BA type names, most preferred first.
 * @param name The type name to rank.
 * @return The zero-based position of @p name, or -1 when it is absent.
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
	if constexpr (ba_has_bool_carrier_type<Node, BA>)
		return ba_descriptor<BA, Node>::bool_carrier_type();
	else return ba_descriptor<BA, Node>::type_tree();
}

/** @brief `true` when some BA in @p Node's pack can host a Boolean. */
template <typename Node>
constexpr bool pack_can_host_bool() {
	return []<std::size_t... Is>(std::index_sequence<Is...>) {
		return (ba_can_host_bool_v<Node,
			std::tuple_element_t<Is, typename Node::bas_tuple>>
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
		if constexpr (ba_can_host_bool_v<Node, BA>) {
			static_assert(ba_has_value_constant<Node, BA>,
				"a BA declaring can_host_bool must also build a plain "
				"value with value_constant: core writes carrier bits "
				"with it");
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
	return pack_owner_apply<Node>(ba_type_id, [&]<typename BA>()
		-> std::optional<bool> {
			if constexpr (ba_has_omcat_qe<Node, BA>)
				return ba_descriptor<BA, Node>::omcat_qe(var, body);
			return std::nullopt;
		});
}

/**
 * @brief Ask the BA owning @p ba_type_id for a quantifier-free formula
 * equivalent to `ex var. body`.
 *
 * The complement of @ref pack_omcat_qe for a body whose truth depends on the
 * other variables (`ex x (a < x && x < b)` is `a < b`), which that capability
 * can only answer "undetermined". nullptr when no BA owns the type, its owner
 * has no residual elimination, or the body is outside what it eliminates.
 */
template <typename Node>
tref pack_omcat_qe_residual(size_t ba_type_id, tref var, tref body) {
	return pack_owner_apply<Node>(ba_type_id, [&]<typename BA>()
		-> std::optional<tref> {
			if constexpr (ba_has_omcat_qe_residual<Node, BA>)
				return ba_descriptor<BA, Node>
					::omcat_qe_residual(var, body);
			return nullptr;
		}).value_or(nullptr);
}

/**
 * @brief Revise @p clause against @p update through a BA's winning region.
 *
 * Takes no type id: the capability decides for itself whether the clause is
 * its own, so a `nullptr` value covers both "not mine" and "no revision
 * found" -- the caller falls back to the syntactic mode either way. A failure
 * in the owner's decision travels in the result.
 */
template <typename Node>
result<tref> pack_semantic_pwr_optimal(tref clause, tref update) {
	result<tref> r;
	pack_visit_all<Node>([&]<typename BA>() {
		// The first owner answers; a later owner in the pack must not
		// override it, and an owner's error propagates.
		if (r.has_value() || r.has_error()) return;
		if constexpr (ba_has_semantic_pwr<Node, BA>) {
			auto opt = r.merge_take(ba_descriptor<BA, Node>
				::semantic_pwr_optimal(clause, update));
			if (opt.has_value()) r.emplace(std::move(*opt));
		}
	});
	// No owner in the pack: no revision, not a failure.
	if (!r.has_value() && !r.has_error()) r.emplace(nullptr);
	return r;
}

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
	return pack_owner_apply<Node>(ba_type_id, [&]<typename BA>()
		-> std::optional<std::string> {
			if constexpr (ba_has_codegen_witness<Node, BA>)
				return ba_descriptor<BA, Node>
					::codegen_witness(var, conj);
			return std::nullopt;
		});
}

/**
 * @brief `true` when the BA owning @p ba_type_id declares a codegen witness.
 *
 * A capability-existence probe, the same shape as @c pack_type_is_non_aba_omcat:
 * codegen consumers that must decide *ahead of calling* @c pack_codegen_witness
 * whether an owner can answer at all (rather than discovering it per-edge as a
 * `nullopt`) ask this instead of naming the BA that happens to implement it.
 */
template <typename Node>
bool pack_type_has_codegen_witness(size_t ba_type_id) {
	return pack_owner_apply<Node>(ba_type_id, []<typename BA>()
		-> std::optional<bool> {
			return ba_has_codegen_witness<Node, BA>;
		}).value_or(false);
}

/** @brief A self-contained C++ expression of type `tref` rebuilding the already-trimmed constant @p cst, from the BA owning @p ba_type_id; `nullopt` means no owner contributes one (a build-time error, never a lossy re-parsed fallback). */
template <typename Node>
std::optional<std::string> pack_codegen_constant_expr(size_t ba_type_id, tref cst) {
	return pack_owner_apply<Node>(ba_type_id, [&]<typename BA>()
		-> std::optional<std::string> {
			if constexpr (ba_has_codegen_constant_expr<Node, BA>)
				return ba_descriptor<BA, Node>
					::codegen_constant_expr(cst);
			return std::nullopt;
		});
}

/**
 * @brief The type tree of the pack BA named @p family: its default tree, or
 *        `type_tree_for(*param)` when @p param names a parameterized
 *        instance. nullptr when no pack member answers to the name, or when
 *        a parameter is given for a family that declares none.
 *
 * An emitted artifact's main resolves its baked ba-type table through this,
 * so a reduced pack works as long as it contains the families the spec uses.
 */
template <typename Node>
tref pack_type_tree(const std::string& family,
	std::optional<unsigned short> param = std::nullopt)
{
	tref out = nullptr;
	pack_visit_all<Node>([&]<typename BA>() {
		if constexpr (ba_has_descriptor_v<Node, BA>) {
			if (out || family != ba_descriptor<BA, Node>::type_name)
				return;
			if constexpr (ba_has_type_tree_for<Node, BA>)
				out = param
					? ba_descriptor<BA, Node>::type_tree_for(*param)
					: ba_descriptor<BA, Node>::type_tree();
			else if (!param) out = ba_descriptor<BA, Node>::type_tree();
		}
	});
	return out;
}

/**
 * @brief `true` when the BA owning @p ba_type names a parameterized family
 *        but @p type_tree carries no parameter of its own.
 *
 * pack_type_family_param answers `{name, nullopt}` alike for an
 * unparameterized owner and a bare parameterized one; this tells the two
 * apart. @p type_tree is taken separately from @p ba_type for the same
 * reason as pack_term_is_blasteable's @p term: this header must not depend
 * on tree<Node>.
 */
template <typename Node>
bool pack_type_family_incomplete(size_t ba_type, tref type_tree) {
	return pack_owner_apply<Node>(ba_type, [&]<typename BA>()
		-> std::optional<bool> {
			if constexpr (ba_has_type_tree_for<Node, BA>)
				return !ba_descriptor<BA, Node>::type_param(type_tree)
					.has_value();
			return std::nullopt;
		}).value_or(false);
}

/**
 * @brief The pack family name and parameter of @p type_tree, from its owner.
 *
 * The inverse of @c pack_type_tree, used at emission time to spell a
 * registry entry the artifact can replay. nullopt when no pack member owns
 * the tree (a reserved core type, or a syntactic type no BA answers to).
 */
template <typename Node>
std::optional<std::pair<std::string, std::optional<unsigned short>>>
pack_type_family_param(tref type_tree) {
	std::optional<std::pair<std::string, std::optional<unsigned short>>> out;
	pack_visit_all<Node>([&]<typename BA>() {
		if constexpr (ba_has_descriptor_v<Node, BA>) {
			if (out || !ba_descriptor<BA, Node>::matches_type(type_tree))
				return;
			std::optional<unsigned short> param;
			if constexpr (ba_has_type_tree_for<Node, BA>)
				param = ba_descriptor<BA, Node>::type_param(type_tree);
			out = {{ba_descriptor<BA, Node>::type_name, param}};
		}
	});
	return out;
}

/**
 * @brief Whether @p src is a truncated literal of the BA owning @p type_tree.
 *
 * nullopt when no BA owns the type or the owner does not classify
 * truncation; a definite `false` means the owner looked and found the
 * literal complete, which is not the same answer. By tree, not id: the REPL
 * asks while a literal is still being typed, before any id exists.
 */
template <typename Node>
std::optional<bool> pack_literal_incomplete(tref type_tree,
	const std::string& src)
{
	if (!type_tree) return std::nullopt;
	std::optional<bool> out;
	pack_visit_all<Node>([&]<typename BA>() {
		if constexpr (ba_has_literal_incomplete<Node, BA>)
			if (!out && ba_descriptor<BA, Node>::matches_type(type_tree))
				out = ba_descriptor<BA, Node>::literal_incomplete(src);
	});
	return out;
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
	return pack_owner_apply<Node>(ba_type, []<typename BA>()
		-> std::optional<bool> {
			return ba_output_always_satisfiable_v<Node, BA>;
		}).value_or(false);
}

/**
 * @brief One BA-declared option paired with the family name that owns it.
 *
 * @p family is the owning descriptor's `type_name`, so a parameterized
 * family (`bv[8]`, `bv[16]`, ...) shares one option set under `bv`, the same
 * basis @ref pack_owns_ba_type_name matches on.
 */
struct ba_named_option {
	/// Owning descriptor's `type_name`.
	std::string family;
	/// The option as the descriptor declares it.
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
			if constexpr (ba_has_options_v<Node, BA>) {
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

/// The current value of @p o as one word, for a fingerprint.
inline size_t ba_option_value_hash(const ba_option& o) {
	switch (o.kind) {
	case ba_option_kind::flag:  return (size_t) o.get_flag();
	case ba_option_kind::count: return o.get_count();
	case ba_option_kind::text:  break;
	}
	return std::hash<std::string>{}(o.get_text());
}

/**
 * @brief @p seed mixed with the current value of every BA-declared option
 * of @p Node's pack.
 *
 * These options steer how an algebra's formulas are decided, so a verdict
 * memo keyed on the formula alone drops its entries when this value moves.
 * @param seed The value to mix into.
 * @return The mixed hash; equal to @p seed when the pack declares no option.
 */
template <typename Node>
size_t pack_ba_options_fingerprint(size_t seed = 0) {
	for (const auto& e : pack_ba_options<Node>()) {
		const size_t v = ba_option_value_hash(e.option);
		seed ^= v + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
	}
	return seed;
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
	/// the option exists; see ba_option_lookup_result::option
	found,
};

/** @brief Result of @ref pack_find_ba_option. */
struct ba_option_lookup_result {
	/// What the lookup found.
	ba_option_lookup_status status;
	/// Valid only when status == found; points into the static storage
	/// pack_ba_options() owns, so it outlives the call that returned it.
	const ba_option* option = nullptr;
};

/**
 * @brief Resolve a `family-name` REPL/CLI option against @p Node's pack,
 * distinguishing "no such family" from "no such option" so callers can
 * report each on its own.
 * @param family The family part of the name (a descriptor `type_name`).
 * @param name The option name within that family.
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
