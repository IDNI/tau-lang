/**
 * @file tau_ba.h
 * @brief `tau_ba<BAs...>` — a Boolean algebra whose elements are Tau specs.
 *
 * Embeds Tau recurrence-relation formulas as first-class BA elements, enabling
 * the Tau BA to be used as a component type inside other BAs (e.g. `variant_ba`).
 * Implements the full BA interface (AND, OR, XOR, NOT, comparisons) plus the
 * `normalize`/`splitter` interface required by the solver.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU__TAU_BA_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU__TAU_BA_H__

#include "tau_tree.h"
#include "env_limits.h"
#include "tau_diagnostics.h"
#include "splitter_types.h"
#include "splitter.h"

// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of tau,...

namespace idni::tau_lang {

/// Support-component factoring of the Tau-BA constant/valid tests
/// (`is_zero`/`is_one`; see the note in tau_ba.tmpl.h): a conjunction whose
/// conjuncts share no variables and refer to no absolute time
/// (`refers_to_absolute_time`) is satisfiable exactly when every component
/// is, and validity distributes over conjunction, so the whole-constant
/// decision of an accumulating spec is replaced by one decision per
/// component, each remembered across steps. On by default (GitHub #92: the
/// accumulating run of #90 goes from 42 s to 5 s with identical output);
/// disabled via `api<node>::set_ba_component_factoring(false)`,
/// `--ba-component-factoring=false`, the REPL option `factoring`, or the
/// environment variable TAU_BA_COMPONENT_FACTORING=0 (any other value
/// enables; the variable overrides the flag in both directions).
inline bool ba_component_factoring = true;

/// How many decided rows of the Tau-BA decision caches (`is_zero`/`is_one`
/// and the per-component factoring) keep their key tree pinned across the
/// interpreter's per-step sweep, oldest released first. Rows whose key tree
/// nothing else holds were dropped at every sweep and their constant
/// re-decided at the next step (GitHub #92). Default 4096; 0 disables the
/// pinning; set via
/// api<node>::set_ba_decision_pins, --ba-decision-pins, or the REPL option
/// decisionpins.
/// Environment fallback `TAU_BA_DECISION_PINS`.
inline env_limit<size_t> ba_decision_pins{ "TAU_BA_DECISION_PINS", 4096 };

/// Misses of the cached is_zero/is_one predicate (decisions computed rather
/// than found), for tests and diagnostics.
inline size_t tau_ba_predicate_misses = 0;

// Check https://gcc.gnu.org/bugzilla/show_bug.cgi?id=102609 to follow up on
// the implementation of "Deducing this" on gcc.
// See also (https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0847r7.html)
// and https://devblogs.microsoft.com/cppblog/cpp23-deducing-this/ for how to use
// "Deducing this" on CRTP.

/**
 * @brief A Tau spec (recurrence relations plus a main formula) used as a
 * Boolean-algebra element.
 *
 * The Boolean operations combine the main formulas and merge the
 * recurrence relations; `is_zero`/`is_one` run the temporal decision
 * procedure and are memoized (see `ba_component_factoring`,
 * `ba_decision_pins`).
 * @tparam BAs The other base algebras of the pack.
 */
template <typename... BAs>
requires BAsPack<BAs...>
struct tau_ba {
	using node = tau_lang::node<tau_ba<BAs...>, BAs...>;
	using tau = tau_lang::tree<node>;

	/**
	 * @brief Constructs the element from recurrence relations and a main
	 * formula.
	 *
	 * @param rec_relations Recurrence relations of the spec.
	 * @param main Main formula of the spec.
	 */
	tau_ba(const rewriter::rules& rec_relations, htref main);

	/**
	 * @brief Constructs the element from recurrence relations and a main
	 * formula.
	 *
	 * @param rec_relations Recurrence relations of the spec.
	 * @param main Main formula of the spec.
	 */
	tau_ba(const rewriter::rules& rec_relations, tref main);

	/**
	 * @brief Constructs the element from a main formula, with no recurrence
	 * relations.
	 *
	 * @param main Main formula of the spec.
	 */
	tau_ba(htref main);

	/**
	 * @brief Constructs the element from a main formula, with no recurrence
	 * relations.
	 *
	 * @param main Main formula of the spec.
	 */
	tau_ba(tref main);

	/// Constructs an element with a default (empty) `nso_rr`.
	tau_ba();

	/**
	 * @brief Three-way comparison, memberwise over `nso_rr`.
	 *
	 * @return Result of the comparison.
	 */
	auto operator<=>(const tau_ba<BAs...>&) const;

	/**
	 * @brief Negation: the NNF of `¬main`, with the temporal quantifiers of
	 * `main` normalized; the recurrence relations are kept.
	 *
	 * Total: a failed normalization falls back to the unnormalized main.
	 * @return The complement element.
	 */
	tau_ba<BAs...> operator~() const;

	/**
	 * @brief Conjunction of the (temporally normalized) main formulas, with
	 * the recurrence relations of both operands merged.
	 *
	 * @param other The right operand.
	 * @return The meet of the two elements.
	 */
	tau_ba<BAs...> operator&(const tau_ba<BAs...>& other) const;

	/**
	 * @brief Disjunction of the (temporally normalized) main formulas, with
	 * the recurrence relations of both operands merged.
	 *
	 * @param other The right operand.
	 * @return The join of the two elements.
	 */
	tau_ba<BAs...> operator|(const tau_ba<BAs...>& other) const;

	/**
	 * @brief Symmetric difference (exclusive or) of the (temporally
	 * normalized) main formulas, with the recurrence relations merged.
	 *
	 * @param other The right operand.
	 * @return The symmetric difference of the two elements.
	 */
	tau_ba<BAs...> operator+(const tau_ba<BAs...>& other) const;

	/**
	 * @brief Same as `operator+`.
	 *
	 * @param other The right operand.
	 * @return The symmetric difference of the two elements.
	 */
	tau_ba<BAs...> operator^(const tau_ba<BAs...>& other) const;

	/**
	 * @brief Checks if the tau_ba is zero, i.e. its normalized spec is
	 * unsatisfiable; the result reports why on a decision failure.
	 *
	 * Memoized by main tree when the element has no recurrence relations;
	 * obeys `ba_component_factoring`.
	 */
	result<bool> is_zero() const;

	/**
	 * @brief Checks if the tau_ba is one, i.e. its normalized spec is valid;
	 * the result reports why on a decision failure.
	 *
	 * Memoized by main tree when the element has no recurrence relations;
	 * obeys `ba_component_factoring`.
	 */
	result<bool> is_one() const;

	/**
	 * @brief The spec this element stands for: its recurrence relations and
	 * main formula.
	 */
	const rr<node> nso_rr;

private:
};

/**
 * @brief Compares a tau_ba with the bottom (`false`) or top (`true`) element.
 *
 * @tparam BAs Variadic template parameters.
 * @param other Reference to tau_ba.
 * @param b `true` asks `is_one()`, `false` asks `is_zero()`.
 * @return True only when the decision says so: an undecided element equals
 * neither truth value. `is_one()` and `is_zero()` carry the report.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const tau_ba<BAs...>& other, const bool& b);

/**
 * @brief Compares a tau_ba with the bottom or top element; same as
 * `other == b`.
 *
 * @tparam BAs Variadic template parameters.
 * @param b `true` for top, `false` for bottom.
 * @param other Reference to tau_ba.
 * @return The result of `other == b`.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const bool& b, const tau_ba<BAs...>& other);

/**
 * @brief Syntactic equality of two tau_ba objects: same main tree and same
 * recurrence relations; no decision procedure runs.
 *
 * @tparam BAs Variadic template parameters.
 * @param lhs Reference to first tau_ba.
 * @param rhs Reference to second tau_ba.
 * @return True if equal, otherwise false.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const tau_ba<BAs...>& lhs, const tau_ba<BAs...>& rhs);

/**
 * @brief Negation of `other == b`.
 *
 * @tparam BAs Variadic template parameters.
 * @param other Reference to tau_ba.
 * @param b `true` for top, `false` for bottom.
 * @return True if not equal, otherwise false.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator!=(const tau_ba<BAs...>& other, const bool& b);

/**
 * @brief Negation of `other == b`.
 *
 * @tparam BAs Variadic template parameters.
 * @param b `true` for top, `false` for bottom.
 * @param other Reference to tau_ba.
 * @return True if not equal, otherwise false.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator!=(const bool& b, const tau_ba<BAs...>& other);

/**
 * @brief Splits the given tau_ba based on splitter type.
 *
 * Normalizes the spec (memoized when it has no recurrence relations) and
 * runs `tau_splitter` on the result.
 * @tparam BAs Variadic template parameters.
 * @param fm Reference to tau_ba.
 * @param st Splitter type.
 * @return Split tau_ba, with no recurrence relations, or the report of a
 * failed normalization or split.
 */
template <typename... BAs>
requires BAsPack<BAs...>
result<tau_ba<BAs...>> splitter(const tau_ba<BAs...>& fm, splitter_type st);

/**
 * @brief Returns a splitter of the top element: `tau_bad_splitter` of `T`.
 *
 * @tparam BAs Variadic template parameters.
 * @return A tau_ba strictly between bottom and top.
 */
template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_splitter_one();

/**
 * @brief Checks if the tau_ba is closed: once its recurrence relations and
 * definitions are applied, no reference remains and every free variable is
 * a stream or an uninterpreted constant.
 *
 * @tparam BAs Variadic template parameters.
 * @param fm Reference to tau_ba.
 * @return Whether @p fm is closed, or the report of a failed application.
 */
template <typename... BAs>
requires BAsPack<BAs...>
result<bool> is_tau_closed(const tau_ba<BAs...>& fm);

/**
 * @brief Parse @p src as a Tau spec constant; the result reports why on failure.
 * @tparam BAs BA pack.
 * @param src Source string.
 * @return The constant, typed `tau`, or the parse errors.
 */
template <typename... BAs>
requires BAsPack<BAs...>
result<typename node<tau_ba<BAs...>, BAs...>::constant_with_type>
	parse_tau(const std::string& src);

/**
 * @brief Print the NSO recurrence-relation of a `tau_ba` to @p os.
 * @return @p os.
 */
template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& operator<<(std::ostream& os, const tau_ba<BAs...>& rs);

} // namespace idni::tau_lang

/// Hash for tau_ba: the hash of its `nso_rr`.
template <typename... BAs>
requires idni::tau_lang::BAsPack<BAs...>
struct std::hash<idni::tau_lang::tau_ba<BAs...>> {
	size_t operator()(const idni::tau_lang::tau_ba<BAs...>& f) const
								noexcept;
};

template<typename ... BAs> requires idni::tau_lang::BAsPack<BAs...>
std::size_t std::hash<idni::tau_lang::tau_ba<BAs...>>::operator()(
	const idni::tau_lang::tau_ba<BAs...>& f) const noexcept {
	using namespace idni::tau_lang;
	return std::hash<rr<node<tau_ba<BAs...>, BAs...>>>{}(f.nso_rr);
}

#include "boolean_algebras/tau/tau_ba.tmpl.h"
#include "boolean_algebras/tau/tau_descriptor.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU__TAU_BA_H__