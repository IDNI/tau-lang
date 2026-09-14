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

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU_BA_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU_BA_H__

#include "tau_tree.h"
#include "splitter_types.h"
#include "splitter.h"

// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of tau,...

namespace idni::tau_lang {

/// Support-component factoring of the Tau-BA constant/valid tests
/// (`is_zero`/`is_one`; see the note in tau_ba.tmpl.h): a conjunction whose
/// conjuncts share no variables is satisfiable exactly when every component
/// is, and validity distributes over conjunction, so the whole-constant
/// decision of an accumulating spec is replaced by one decision per
/// component, each remembered across steps. On by default (GitHub #92: the
/// accumulating run of #90 goes from 42 s to 5 s with identical output);
/// disabled via `api::set_ba_component_factoring(false)`,
/// `--ba-component-factoring=false`, the REPL option `factoring`, or the
/// environment variable TAU_BA_COMPONENT_FACTORING=0 (any other value
/// enables; the variable overrides the flag in both directions).
inline bool ba_component_factoring = true;

/// How many decided rows of the Tau-BA decision caches (`is_zero`/`is_one`
/// and the per-component factoring) keep their key tree pinned across the
/// interpreter's per-step sweep, oldest released first. Rows whose key tree
/// nothing else holds were dropped at every sweep and their constant
/// re-decided at the next step (GitHub #92). 0 disables the pinning; set via
/// api::set_ba_decision_pins, --ba-decision-pins, or the REPL option
/// decisionpins.
inline size_t ba_decision_pins = 4096;

/// Misses of the cached is_zero/is_one predicate (decisions computed rather
/// than found), for tests and diagnostics.
inline size_t tau_ba_predicate_misses = 0;

// Check https://gcc.gnu.org/bugzilla/show_bug.cgi?id=102609 to follow up on
// the implementation of "Deducing this" on gcc.
// See also (https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0847r7.html)
// and https://devblogs.microsoft.com/cppblog/cpp23-deducing-this/ for how to use
// "Deducing this" on CRTP.

template <typename... BAs>
requires BAsPack<BAs...>
struct tau_ba {
	using node = tau_lang::node<tau_ba<BAs...>, BAs...>;
	using tau = tau_lang::tree<node>;

	/**
	 * @brief Constructor initializing tau_ba with recursive relations and main tau_nso_t.
	 *
	 * @param rec_relations Reference to rules of tau_nso_t.
	 * @param main Reference to main tau_nso_t.
	 */
	tau_ba(const rewriter::rules& rec_relations, htref main);

	/**
	 * @brief Constructor initializing tau_ba with recursive relations and main tau_nso_t.
	 *
	 * @param rec_relations Reference to rules of tau_nso_t.
	 * @param main Reference to main tau_nso_t.
	 */
	tau_ba(const rewriter::rules& rec_relations, tref main);

	/**
	 * @brief Constructor initializing tau_ba with main tau_nso_t.
	 *
	 * @param main Reference to main tau_nso_t.
	 */
	tau_ba(htref main);

	/**
	 * @brief Constructor initializing tau_ba with main tau_nso_t.
	 *
	 * @param main Reference to main tau_nso_t.
	 */
	tau_ba(tref main);

	tau_ba();

	/**
	 * @brief Three-way comparison operator.
	 *
	 * @param other Reference to another tau_ba<BAs...>.
	 * @return Result of the comparison.
	 */
	auto operator<=>(const tau_ba<BAs...>&) const;

	/**
	 * @brief Bitwise NOT operator.
	 *
	 * @return Result of the bitwise NOT operation.
	 */
	tau_ba<BAs...> operator~() const;

	/**
	 * @brief Bitwise AND operator.
	 *
	 * @param other Reference to another tau_ba<BAs...>.
	 * @return Result of the bitwise AND operation.
	 */
	tau_ba<BAs...> operator&(const tau_ba<BAs...>& other) const;

	/**
	 * @brief Bitwise OR operator.
	 *
	 * @param other Reference to another tau_ba<BAs...>.
	 * @return Result of the bitwise OR operation.
	 */
	tau_ba<BAs...> operator|(const tau_ba<BAs...>& other) const;

	/**
	 * @brief Addition operator.
	 *
	 * @param other Reference to another tau_ba<BAs...>.
	 * @return Result of the addition operation.
	 */
	tau_ba<BAs...> operator+(const tau_ba<BAs...>& other) const;

	/**
	 * @brief Bitwise XOR operator.
	 *
	 * @param other Reference to another tau_ba<BAs...>.
	 * @return Result of the bitwise XOR operation.
	 */
	tau_ba<BAs...> operator^(const tau_ba<BAs...>& other) const;

	/**
	 * @brief Checks if the tau_ba is zero.
	 *
	 * @return True if zero, otherwise false.
	 */
	bool is_zero() const;

	/**
	 * @brief Checks if the tau_ba is one.
	 *
	 * @return True if one, otherwise false.
	 */
	bool is_one() const;

	/**
	 * @brief Type equivalent to tau_spec<BAs...>.
	 */
	const rr<node> nso_rr;

private:
};

/**
 * @brief Equality operator for tau_ba and bool.
 *
 * @tparam BAs Variadic template parameters.
 * @param other Reference to tau_ba.
 * @param b Reference to bool.
 * @return True if equal, otherwise false.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const tau_ba<BAs...>& other, const bool& b);

/**
 * @brief Equality operator for bool and tau_ba.
 *
 * @tparam BAs Variadic template parameters.
 * @param b Reference to bool.
 * @param other Reference to tau_ba.
 * @return True if equal, otherwise false.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator==(const bool& b, const tau_ba<BAs...>& other);

/**
 * @brief Equality operator for two tau_ba objects.
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
 * @brief Inequality operator for tau_ba and bool.
 *
 * @tparam BAs Variadic template parameters.
 * @param other Reference to tau_ba.
 * @param b Reference to bool.
 * @return True if not equal, otherwise false.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator!=(const tau_ba<BAs...>& other, const bool& b);

/**
 * @brief Inequality operator for bool and tau_ba.
 *
 * @tparam BAs Variadic template parameters.
 * @param b Reference to bool.
 * @param other Reference to tau_ba.
 * @return True if not equal, otherwise false.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool operator!=(const bool& b, const tau_ba<BAs...>& other);

/**
 * @brief Splits the given tau_ba based on splitter type.
 *
 * @tparam BAs Variadic template parameters.
 * @param fm Reference to tau_ba.
 * @param st Splitter type.
 * @return Split tau_ba.
 */
template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> splitter(const tau_ba<BAs...>& fm, splitter_type st);

/**
 * @brief Splits tau_ba into one.
 *
 * @tparam BAs Variadic template parameters.
 * @return Split tau_ba.
 */
template <typename... BAs>
requires BAsPack<BAs...>
tau_ba<BAs...> tau_splitter_one();

/**
 * @brief Checks if the tau_ba is closed.
 *
 * @tparam BAs Variadic template parameters.
 * @param fm Reference to tau_ba.
 * @return True if closed, otherwise false.
 */
template <typename... BAs>
requires BAsPack<BAs...>
bool is_tau_closed(const tau_ba<BAs...>& fm);

/**
 * @brief Parse @p src as a Tau spec constant; return `nullopt` on failure.
 * @tparam BAs BA pack.
 * @param src Source string.
 */
template <typename... BAs>
requires BAsPack<BAs...>
std::optional<typename node<tau_ba<BAs...>, BAs...>::constant_with_type>
	parse_tau(const std::string& src);

/** @brief Print the NSO recurrence-relation of a `tau_ba` to @p os. */
template <typename... BAs>
requires BAsPack<BAs...>
std::ostream& operator<<(std::ostream& os, const tau_ba<BAs...>& rs);

} // namespace idni::tau_lang

// Hash for tau_ba using specialization to std::hash
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

#include "boolean_algebras/tau_ba.tmpl.h"

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__TAU_BA_H__