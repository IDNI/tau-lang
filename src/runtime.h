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
/*template <>
struct nso_factory<sbf_ba> {

	static std::vector<std::string> types();

	static std::string default_type();

	static std::string one(const std::string type_name);

	static std::string zero(const std::string type_name);

	static tref splitter_one();

	static tref unpack_tau_ba(const std::variant<sbf_ba>&);

	static std::variant<sbf_ba> pack_tau_ba(tref);

};*/

/**
 * @brief NSO factory used in REPL
 */
/*template<>
struct nso_factory<tau_ba<sbf_ba>, sbf_ba> {

	static std::vector<std::string> types();

	static std::string default_type();

	static std::string one(const std::string type_name = "tau");

	static std::string zero(const std::string type_name = "tau");

	static tref splitter_one(const std::string type_name);

	static tref unpack_tau_ba(const std::variant<tau_ba<sbf_ba>, sbf_ba>& v);

	static std::variant<tau_ba<sbf_ba>, sbf_ba> pack_tau_ba(tref c);

};*/


template <>
struct nso_factory<bv, sbf_ba> {

	static std::vector<std::string> types();

	static std::string default_type();

	static std::string one(const std::string type_name);

	static std::string zero(const std::string type_name);

	static tref splitter_one();

	static tref unpack_tau_ba(const std::variant<bv, sbf_ba>&);

	static std::variant<bv, sbf_ba> pack_tau_ba(tref);

};

/**
 * @brief NSO factory used in REPL
 */
template<>
struct nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba> {

	static std::vector<std::string> types();

	static std::string default_type();

	static std::string one(const std::string type_name = "tau");

	static std::string zero(const std::string type_name = "tau");

	static tref splitter_one(const std::string type_name);

	static tref unpack_tau_ba(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& v);

	static std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> pack_tau_ba(tref c);

};

/*template <>
std::optional<typename ba_constants<node<sbf_ba>>::constant_with_type> ba_constants<node<sbf_ba>>::get(
		[[maybe_unused]] const std::string& constant_source,
		[[maybe_unused]] const std::string type_name,
		[[maybe_unused]] const std::string options) {
	return sbf_ba_factory<sbf_ba>::parse(constant_source);
}

template <>
std::optional<typename ba_constants<node<tau_ba<sbf_ba>, sbf_ba>>::constant_with_type> ba_constants<node<tau_ba<sbf_ba>, sbf_ba>>::get(
		[[maybe_unused]] const std::string& constant_source,
		[[maybe_unused]] const std::string type_name,
		[[maybe_unused]] const std::string options) {
	return type_name == "sbf"
		? sbf_ba_factory<tau_ba<sbf_ba>, sbf_ba>::parse(constant_source)
		: tau_ba_factory<sbf_ba>::parse(constant_source);
}*/

template <>
std::optional<typename ba_constants<node<bv, sbf_ba>>::constant_with_type> ba_constants<node<bv, sbf_ba>>::get(
		[[maybe_unused]] const std::string& constant_source,
		[[maybe_unused]] const std::string type_name,
		[[maybe_unused]] const std::string options) {
	return sbf_ba_factory<bv, sbf_ba>::parse(constant_source);
}

template <>
std::optional<typename ba_constants<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>::constant_with_type> ba_constants<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>::get(
		[[maybe_unused]] const std::string& constant_source,
		[[maybe_unused]] const std::string type_name,
		[[maybe_unused]] const std::string options) {
	return type_name == "sbf"
		? sbf_ba_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::parse(constant_source)
		: tau_ba_factory<bv, sbf_ba>::parse(constant_source);
}

} // namespace idni::tau_lang

#include "runtime.tmpl.h"

#endif // __RUNTIME_H__