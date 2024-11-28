// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __SBF_BA_H__
#define __SBF_BA_H__

#include <boost/log/trivial.hpp>

#include "tau_ba.h"

#include "../parser/sbf_parser.generated.h"

namespace idni::tau {

using sbf_ba = hbdd<Bool>;
using sp_sbf_node = sp_tau_node<tau_ba<sbf_ba>, sbf_ba>;

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
	using parse_forest = idni::parser<char, char>::pforest;
	using parse_result = idni::parser<char, char>::result;
	using traverser_t  = traverser<tau_sym<BAs...>, sbf_parser>;

	static constexpr const auto& get_only_child =
			traverser_t::get_only_child_extractor();
	static constexpr const auto& get_terminals =
			traverser_t::get_terminal_extractor();
	static constexpr const auto& get_nonterminal =
			traverser_t::get_nonterminal_extractor();
	inline static std::map<std::string, nso<BAs...>> cache;

	nso<BAs...> build_node(const sbf_ba& b);
	// evaluates a parsed bdd terminal node recursively
	sbf_ba eval_node(const traverser_t& t);
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