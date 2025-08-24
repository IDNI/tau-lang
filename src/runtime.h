// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __RUNTIME_H__
#define __RUNTIME_H__

#include "boolean_algebras/bool_ba.h"
#include "boolean_algebras/sbf_ba.h"
#include "boolean_algebras/tau_ba.h"
#include "boolean_algebras/bv_ba.h"
#include "nso_rr.h"

namespace idni::tau_lang {

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

template <>
std::optional<typename ba_constants<node<bv, sbf_ba>>::constant_with_type> ba_constants<node<bv, sbf_ba>>::get(
		[[maybe_unused]] const std::string& constant_source,
		[[maybe_unused]] const std::string type_name,
		[[maybe_unused]] const std::string options) {
	if (type_name == "sbf") return parse_sbf<bv, sbf_ba>(constant_source);
	if (type_name == "bv") return parse_bv<bv, sbf_ba>(constant_source);
	return {};
}

template <>
std::optional<typename ba_constants<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>::constant_with_type> ba_constants<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>::get(
		[[maybe_unused]] const std::string& constant_source,
		[[maybe_unused]] const std::string type_name,
		[[maybe_unused]] const std::string options) {
	if (type_name == "sbf") return parse_sbf<tau_ba<bv, sbf_ba>, bv, sbf_ba>(constant_source);
	if (type_name == "bv") return parse_bv<tau_ba<bv, sbf_ba>, bv, sbf_ba>(constant_source);
	if (type_name == "tau") return parse_tau<bv, sbf_ba>(constant_source);
	return {};
}

} // namespace idni::tau_lang

#include "runtime.tmpl.h"

#endif // __RUNTIME_H__