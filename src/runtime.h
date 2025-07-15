// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __RUNTIME_H__
#define __RUNTIME_H__

#include "boolean_algebras/bool_ba.h"
#include "boolean_algebras/sbf_ba.h"
#include "boolean_algebras/tau_ba.h"
#include "base_bas/cvc5.h"
#include "nso_rr.h"
namespace idni::tau_lang {


/**
 * @brief NSO factory used during testing
 */
template<>
struct nso_factory<sbf_ba> {
	inline static sbf_ba_factory<sbf_ba> bf;
	//inline static bv_ba_factory<sbf_ba> bvf;

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
	inline static bv_ba_factory<tau_ba<sbf_ba>, sbf_ba> bvf;

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

	// TODO (MEDIUM) move this method to interpreter
	/**
	 * Returns the tau formula stored in the variant, if present
	 * @param v Variant for formula extraction
	 * @return Extracted formula if present
	 */
	std::optional<tau_nso<sbf_ba>> unpack_tau_ba(
		const std::variant<tau_ba<sbf_ba>, sbf_ba>& v) const;

	// TODO (MEDIUM) move this method to interpreter
	/**
	 * Returns a std::variant holding the tau_ba of the given formula
	 * @param c Formula to wrap inside tau_ba
	 * @return std::variant holding a tau_ba
	 */
	std::variant<tau_ba<sbf_ba>, sbf_ba> pack_tau_ba(
		const tau<tau_ba<sbf_ba>, sbf_ba>& c) const;

	static nso_factory<tau_ba<sbf_ba>, sbf_ba>& instance();
private:
	nso_factory();
};

} // namespace idni::tau_lang

#include "runtime.tmpl.h"

#endif // __RUNTIME_H__