// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

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

	static tref default_type();

	static std::string one(const tref type_tree);

	static std::string zero(const tref type_tree);

	static tref splitter_one();

	static tref unpack_tau_ba(const std::variant<bv, sbf_ba>&);

	static std::variant<bv, sbf_ba> pack_tau_ba(tref);

	static std::variant<bv, sbf_ba> to_base_ba_type(tref type_tree);

};

/**
 * @brief NSO factory used in REPL
 */
template<>
struct nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba> {
	static std::vector<std::string> types();

	static tref default_type();

	static std::string one(tref type_tree = tau_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>());

	static std::string zero(tref type_tree = tau_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>());

	static tref splitter_one(tref type_tree);

	static tref unpack_tau_ba(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& v);

	static std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> pack_tau_ba(tref c);

	static std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> to_base_ba_type(tref type_tree);

};

template <>
inline std::optional<ba_constants<node<bv, sbf_ba>>::constant_with_type> ba_constants<node<bv, sbf_ba>>::get(
		const std::string& constant_source,
		tref type_tree,
		[[maybe_unused]] const std::string options) {
	if (is_bv_type_family<node<bv, sbf_ba>>(type_tree))
		return parse_bv<bv, sbf_ba>(constant_source, type_tree);
	return parse_sbf<bv, sbf_ba>(constant_source);
}

template <>
inline std::optional<typename ba_constants<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>::constant_with_type> ba_constants<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>::get(
		const std::string& constant_source,
		tref type_tree,
		[[maybe_unused]] const std::string options) {
	if (is_sbf_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>(type_tree))
		return parse_sbf<tau_ba<bv, sbf_ba>, bv, sbf_ba>(constant_source);
	if (is_bv_type_family<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>(type_tree))
		return parse_bv<tau_ba<bv, sbf_ba>, bv, sbf_ba>(constant_source, type_tree);
	return parse_tau<bv, sbf_ba>(constant_source);
}

} // namespace idni::tau_lang

#include "runtime.tmpl.h"

#endif // __RUNTIME_H__