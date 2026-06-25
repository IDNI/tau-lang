/**
 * @file sbf_ba.h
 * @brief Simple Boolean Function (SBF) Boolean algebra backed by BDDs.
 *
 * `sbf_ba` is an alias for `hbdd<Bool>` — a hash-consed BDD over classical
 * `Bool` coefficients.  Inline helpers provide the `nso_factory` interface
 * (splitter, normalise, one/zero predicates) and a string parser.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__SBF_BA_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__SBF_BA_H__

#include "boolean_algebras/bdds/bdd_handle.h"
#include "tau_tree.h"

namespace idni::tau_lang {

/**
 * @brief Simple Boolean function Boolean algebra represented by bdd
 */
using sbf_ba = hbdd<Bool>;

/**
 * @brief global static bdd variable cache
 */
inline static std::map<int_t, sbf_ba> var_cache{};


/** @brief Symbol simplification (no-op for SBF — returns @p sym unchanged). */
inline tref simplify_sbf_symbol(tref sym) { return sym; }

/** @brief Term simplification (no-op for SBF — returns @p term unchanged). */
inline tref simplify_sbf_term(tref term) { return term; }

/** @brief Return the BDD splitter of @p elem using strategy @p st. */
inline sbf_ba sbf_splitter(const sbf_ba& elem, splitter_type st) { return elem->splitter(st); }

/** @brief Return the splitter-one element (bad-splitter of the BDD `true`). */
inline sbf_ba sbf_splitter_one() { return bdd_handle<Bool>::htrue->splitter(splitter_type::bad); }

/** @brief Normalise an SBF element (identity — BDDs are already canonical). */
inline sbf_ba normalize_sbf(const sbf_ba& elem) { return elem; }

/** @brief Parse @p src as an SBF constant; return `nullopt` on failure. */
template <typename... BAs>
requires BAsPack<BAs...>
std::optional<typename node<BAs...>::constant_with_type> parse_sbf(const std::string& src);

/** @brief Return `true` if @p x is the SBF one (BDD `true`). */
inline bool is_sbf_one(const sbf_ba& x) { return x->is_one(); }

/** @brief Return `true` if @p x is the SBF zero (BDD `false`). */
inline bool is_sbf_zero(const sbf_ba& x) { return x->is_zero(); }

} // namespace idni::tau_lang

// Hash for hbdd as specialization of std::hash
template<>
struct std::hash<idni::tau_lang::hbdd<idni::tau_lang::Bool>> {
	size_t operator()(const idni::tau_lang::hbdd<idni::tau_lang::Bool>& h)
		const noexcept
	{
		return h->hash();
	}
};

#include "boolean_algebras/sbf_ba.tmpl.h"
#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__SBF_BA_H__
