// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __SBF_BA_H__
#define __SBF_BA_H__

#include <boost/log/trivial.hpp>

#include "tau_ba.h"

#include "../parser/sbf_parser.generated.h"

namespace idni::tau_lang {

/**
 * @brief Simple Boolean function Boolean algebra represented by bdd 
 */
using sbf_ba = hbdd<Bool>;
using sbf_source_sym = idni::lit<char, char>;
using sbf_sym = std::variant<sbf_source_sym, sbf_ba>;

/**
 * @brief global static bdd variable cache
 */
inline static std::map<int_t, sbf_ba> var_cache{};

/**
 * @brief Boolean algebra factory for Simple Boolean function
 * 
 * @tparam BAs Boolean algebras
 */
template<typename...BAs>
struct sbf_ba_factory {

	/**
	 * @brief parses a SBF from a string
	 *
	 * @param src source string
	 * @return optional parsed node if parsing successful
	 */
	std::optional<tau<BAs...>> parse(const std::string& src);

	/**
	 * @brief builds a SBF bounded node from a parsed terminals of a source binding
	 * 
	 * @param sn tau code node with parsed SBF 
	 * @return bounded constant
	 */
	tau<BAs...> binding(const tau<BAs...>& sn);

	std::variant<BAs...> splitter_one () const;

	std::string one() const;

	std::string zero() const;
	// static sbf_ba_factory<BAs...>& instance();
private:

	inline static std::map<std::string, tau<BAs...>> cache;
};

/**
 * @brief NSO factory used during testing
 */
template<>
struct nso_factory<sbf_ba> {
	inline static sbf_ba_factory<sbf_ba> bf;

	std::optional<tau<sbf_ba>> parse(const std::string& src,
		const std::string& = "");

	tau<sbf_ba> binding(const tau<sbf_ba>& n,
		const std::string& = "");

	std::vector<std::string> types() const;

	tau<sbf_ba> splitter_one() const;

	std::string default_type() const;

	std::string one(const std::string type_name) const;

	std::string zero(const std::string type_name) const;

	static nso_factory<sbf_ba>& instance();
private:
	nso_factory();
};

/**
 * @brief NSO factory used in REPL
 */
template<>
struct nso_factory<tau_ba<sbf_ba>, sbf_ba> {
	inline static sbf_ba_factory<tau_ba<sbf_ba>, sbf_ba> bf;
	inline static tau_ba_factory<sbf_ba> tf;

	std::optional<tau_nso<sbf_ba>> parse(const std::string src,
		const std::string type_name);

	tau_nso<sbf_ba> binding(
		const tau<tau_ba<sbf_ba>, sbf_ba>& n,
		const std::string type_name);

	std::vector<std::string> types() const;

	std::string default_type() const;

	tau_nso<sbf_ba> splitter_one(const std::string& type_name) const;

	std::string one(const std::string type_name = "tau") const;

	std::string zero(const std::string type_name = "tau") const;

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