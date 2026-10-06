/**
 * @file sbf_ba.h
 * @brief Simple Boolean Function (SBF) Boolean algebra backed by BDDs.
 *
 * `sbf_ba` is an alias for `hbdd<Bool>` — a hash-consed BDD over classical
 * `Bool` coefficients.  Inline helpers provide the `nso_factory` interface
 * (splitter, normalise, one/zero predicates) and a string parser.
 */

// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__SBF__SBF_BA_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__SBF__SBF_BA_H__

#include "backends/bdds/bdd_handle.h"
#include "tau_tree.h"
#include "tau_diagnostics.h"

namespace idni::tau_lang {

/** @brief Type tree of the sbf type. */
template <NodeType node> tref sbf_type();
/** @brief Type id of the sbf type. */
template <NodeType node> size_t sbf_type_id();


/**
 * @brief Simple Boolean function Boolean algebra represented by bdd
 */
using sbf_ba = hbdd<Bool>;

/**
 * @brief Cache of the BDD of each sbf variable, keyed by its `var_dict` id.
 *
 * Filled by the sbf parser; a variable whose BDD was built on an exhausted
 * node table is not cached.
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

/**
 * @brief Parse @p src as an SBF constant.
 *
 * Results are cached per source string, except one built on an exhausted BDD
 * node table. Initializes the BDD library if needed.
 * @param src Source text of the constant; a text without an sbf expression
 * yields the BDD `false`.
 * @return The constant with the sbf type, or a `parse_error` (or an internal
 * error for an unknown operator) in the report.
 */
template <typename... BAs>
requires BAsPack<BAs...>
result<typename node<BAs...>::constant_with_type> parse_sbf(const std::string& src);
/** @brief Return `true` if @p x is the SBF one (BDD `true`). */
inline bool is_sbf_one(const sbf_ba& x) { return x->is_one(); }

/** @brief Return `true` if @p x is the SBF zero (BDD `false`). */
inline bool is_sbf_zero(const sbf_ba& x) { return x->is_zero(); }

} // namespace idni::tau_lang

/// Hash of an sbf constant: the hash of its BDD handle.
template<>
struct std::hash<idni::tau_lang::hbdd<idni::tau_lang::Bool>> {
	size_t operator()(const idni::tau_lang::hbdd<idni::tau_lang::Bool>& h)
		const noexcept
	{
		return static_cast<size_t>(h->hash());
	}
};

#include "boolean_algebras/sbf/sbf_ba.tmpl.h"
#include "boolean_algebras/sbf/sbf_descriptor.tmpl.h"
#include "boolean_algebras/sbf/sbf_types.tmpl.h"
#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__SBF__SBF_BA_H__
