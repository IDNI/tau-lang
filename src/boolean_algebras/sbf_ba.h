// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__SBF_BA_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__SBF_BA_H__

#include "boolean_algebras/bdds/bdd_handle.h"
#include "boolean_algebras/tau_ba.h"
#include "boolean_algebras/variant_ba.h"

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

	/**
	 * @brief parses a SBF from a string
	 *
	 * @param src source string
	 * @return optional parsed node if parsing successful
	 */
	std::optional<constant_with_type<BAs...>> parse(
		const std::string& constant_source);

	std::string one() const;

	std::string zero() const;

	constant_with_type<BAs...> splitter_one() const;

	static sbf_ba_factory<BAs...>& instance();
private:

	inline static std::map<size_t, std::variant<BAs...>> cache;
};

/**
 * @brief NSO factory used during testing
 * TODO (LOW) move to tests?
 */
template <>
struct nso_factory<sbf_ba> {
	inline static sbf_ba_factory<sbf_ba> bf;

	std::optional<constant_with_type<sbf_ba>> parse(
		const std::string& constant_source,
		const std::string = "");

	std::vector<std::string> types() const;

	std::string default_type() const;

	std::string one(const std::string type_name) const;

	std::string zero(const std::string type_name) const;

	constant_with_type<sbf_ba> splitter_one() const;

	std::optional<rr> unpack_tau_ba(const std::variant<sbf_ba>&) const;

	static nso_factory<sbf_ba>& instance();
private:
	nso_factory();
};

/**
 * @brief NSO factory used in REPL
 */
template<>
struct nso_factory<tau_ba<sbf_ba>, sbf_ba> {
	static sbf_ba_factory<tau_ba<sbf_ba>, sbf_ba>& bf() {
		return sbf_ba_factory<tau_ba<sbf_ba>, sbf_ba>::instance(); }
	static tau_ba_factory<sbf_ba>& tf() {
		return tau_ba_factory<sbf_ba>::instance(); }

	std::optional<constant_with_type<tau_ba<sbf_ba>, sbf_ba>> parse(
		const std::string& constant_source,
		const std::string type_name);

	std::vector<std::string> types() const;

	std::string default_type() const;

	std::string one(const std::string type_name = "tau") const;

	std::string zero(const std::string type_name = "tau") const;

	constant_with_type<tau_ba<sbf_ba>, sbf_ba> splitter_one(
		const std::string type_name) const;

	std::optional<rr> unpack_tau_ba(
		const std::variant<tau_ba<sbf_ba>, sbf_ba>& v) const;

	static nso_factory<tau_ba<sbf_ba>, sbf_ba>& instance();
private:
	nso_factory();
};

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