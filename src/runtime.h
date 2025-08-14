// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __RUNTIME_H__
#define __RUNTIME_H__

#include "boolean_algebras/bool_ba.h"
#include "boolean_algebras/sbf_ba.h"
#include "boolean_algebras/tau_ba.h"
//#include "boolean_algebras/bv_ba.h"
#include "nso_rr.h"
namespace idni::tau_lang {


/**
 * @brief NSO factory used during testing
 * TODO (LOW) move to tests?
 */
template <>
struct nso_factory<sbf_ba> {

	static std::optional<constant_with_type<sbf_ba>> parse(
		const std::string& constant_source,
		const std::string = "");

	static std::vector<std::string> types();

	static std::string default_type();

	static std::string one(const std::string type_name);

	static std::string zero(const std::string type_name);

	static tref splitter_one();

	static tref unpack_tau_ba(const std::variant<sbf_ba>&);

	static std::variant<sbf_ba> pack_tau_ba(tref);

};

/**
 * @brief NSO factory used in REPL
 */
template<>
struct nso_factory<tau_ba<sbf_ba>, sbf_ba> {

	static std::optional<constant_with_type<tau_ba<sbf_ba>, sbf_ba>> parse(
		const std::string& constant_source,
		const std::string type_name);

	static std::vector<std::string> types();

	static std::string default_type();

	static std::string one(const std::string type_name = "tau");

	static std::string zero(const std::string type_name = "tau");

	static tref splitter_one(const std::string type_name);

	static tref unpack_tau_ba(const std::variant<tau_ba<sbf_ba>, sbf_ba>& v);

	static std::variant<tau_ba<sbf_ba>, sbf_ba> pack_tau_ba(tref c);

};


} // namespace idni::tau_lang

#include "runtime.tmpl.h"

#endif // __RUNTIME_H__