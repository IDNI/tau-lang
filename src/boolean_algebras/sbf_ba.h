// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __SBF_BA_H__
#define __SBF_BA_H__

#include "tau.h"
#include "boolean_algebras/tau_ba.h"
#include "../parser/sbf_parser.generated.h"

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
template <typename...BAs>
struct sbf_ba_factory {

	/**
	 * @brief parses a SBF from a string
	 *
	 * @param src source string
	 * @return optional parsed node if parsing successful
	 */
	std::optional<std::variant<BAs...>> parse(const std::string& src);

	/**
	 * @brief builds a SBF bounded node from a parsed terminals of a source binding
	 *
	 * @param sn tau code node with parsed SBF
	 * @return bounded constant
	 */
	tref binding(const std::string& source);

	std::variant<BAs...> splitter_one() const;

	std::string one() const;

	std::string zero() const;

	static sbf_ba_factory<BAs...>& instance();
private:

	inline static std::map<std::string, std::variant<BAs...>> cache;
};

/**
 * @brief NSO factory used during testing
 */
template<>
struct nso_factory<sbf_ba> {
	inline static sbf_ba_factory<sbf_ba> bf;

	std::optional<std::variant<sbf_ba>> parse(const std::string& src,
						const std::string = "");

	tref binding(const std::string& source, const std::string& type_name);

	std::vector<std::string> types() const;

	tref splitter_one() const;

	std::string default_type() const;

	std::string one(const std::string type_name) const;

	std::string zero(const std::string type_name) const;

	tref unpack_tau_ba(const std::variant<sbf_ba>&) const;

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

	std::optional<std::variant<tau_ba<sbf_ba>, sbf_ba>> parse(
		const std::string& src, const std::string type_name);

	tref binding(const std::string& n, const std::string type_name);

	std::vector<std::string> types() const;

	std::string default_type() const;

	tref splitter_one(const std::string type_name) const;

	std::string one(const std::string type_name = "tau") const;

	std::string zero(const std::string type_name = "tau") const;

	tref unpack_tau_ba(const std::variant<tau_ba<sbf_ba>, sbf_ba>& v) const;

	static nso_factory<tau_ba<sbf_ba>, sbf_ba>& instance();
private:
	nso_factory();
};

} // namespace idni::tau_lang

// Hash for hbdd as specialization of std::hash
template<>
struct std::hash<hbdd<Bool>> {
	size_t operator()(const hbdd<Bool>& h) const noexcept {
		return h->hash();
	}
};

#include "sbf_ba.tmpl.h"
#endif // __SBF_BA_H__