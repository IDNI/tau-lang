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
	inline static sbf_ba_factory<sbf_ba> bf;

	std::optional<constant_with_type<sbf_ba>> parse(
		const std::string& constant_source,
		const std::string = "");

	std::vector<std::string> types() const;

	std::string default_type() const;

	std::string one(const std::string type_name) const;

	std::string zero(const std::string type_name) const;

	tref splitter_one() const;

	tref unpack_tau_ba(const std::variant<sbf_ba>&) const;

	std::variant<sbf_ba> pack_tau_ba(tref) const;

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

	tref splitter_one( const std::string type_name) const;

	tref unpack_tau_ba(const std::variant<tau_ba<sbf_ba>, sbf_ba>& v) const;

	std::variant<tau_ba<sbf_ba>, sbf_ba> pack_tau_ba(tref c) const;

	static nso_factory<tau_ba<sbf_ba>, sbf_ba>& instance();
private:
	nso_factory();
};


} // namespace idni::tau_lang

#include "runtime.tmpl.h"

#endif // __RUNTIME_H__