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

/**
 * @brief Boolean algebra factory for Simple Boolean function
 *
 * @tparam BAs Boolean algebras
 */
template <typename... BAs>
requires BAsPack<BAs...>
struct sbf_ba_factory {

	static std::string one();

	static std::string zero();

	static std::variant<BAs...> splitter_one();

	//inline static std::map<size_t, std::variant<BAs...>> cache;
};

inline bool is_closed(const sbf_ba&) { return true; }

inline tref base_ba_symbol_simplification(tref sym, const sbf_ba&) { return sym; }

template <typename... BAs>
requires BAsPack<BAs...>
std::optional<typename node<BAs...>::constant_with_type> parse_sbf(const std::string& src);

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