// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __SBF_BA_H__
#define __SBF_BA_H__

#include <boost/log/trivial.hpp>

#include "tau_ba.h"

#include "../parser/sbf_parser.generated.h"

namespace idni::tau {

using sbf_ba = hbdd<Bool>;
using sp_sbf_node = sp_tau_node<tau_ba<sbf_ba>, sbf_ba>;
using sbf_source_sym = idni::lit<char, char>;
using sbf_sym = std::variant<sbf_source_sym, sbf_ba>;

// global static bdd variable cache
inline static std::map<int_t, sbf_ba> var_cache{};

template<typename...BAs>
struct sbf_ba_factory {

	// parses a SBF from a string
	std::optional<nso<BAs...>> parse(const std::string& src);

	// builds a SBF bounded node parsed from terminals of a source binding
	nso<BAs...> binding(const nso<BAs...>& sn);

	std::variant<BAs...> splitter_one () const;

	// static sbf_ba_factory<BAs...>& instance();
private:

	inline static std::map<std::string, nso<BAs...>> cache;
};

// using during testing
template<>
struct nso_factory<sbf_ba> {
	inline static sbf_ba_factory<sbf_ba> bf;

	std::optional<nso<sbf_ba>> parse(const std::string& src,
		const std::string& = "");

	nso<sbf_ba> binding(const nso<sbf_ba>& n,
		const std::string& = "");

	std::vector<std::string> types() const;

	nso<sbf_ba> splitter_one() const;

	std::string default_type() const;

	static nso_factory<sbf_ba>& instance();
private:
	nso_factory();
};

// using in repl
template<>
struct nso_factory<tau_ba<sbf_ba>, sbf_ba> {
	inline static sbf_ba_factory<tau_ba<sbf_ba>, sbf_ba> bf;
	inline static tau_ba_factory<sbf_ba> tf;

	std::optional<gssotc<sbf_ba>> parse(const std::string src,
		const std::string type_name);

	gssotc<sbf_ba> binding(
		const sp_tau_node<tau_ba<sbf_ba>, sbf_ba>& n,
		const std::string type_name);

	std::vector<std::string> types() const;

	std::string default_type() const;

	gssotc<sbf_ba> splitter_one(const std::string& type_name) const;

	static nso_factory<tau_ba<sbf_ba>, sbf_ba>& instance();
private:
	nso_factory();
};

} // namespace idni::tau
#include "sbf_ba.tmpl.h"
#endif // __SBF_BA_H__