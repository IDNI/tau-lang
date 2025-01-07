// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __LANGUAGE_H__
#define __LANGUAGE_H__

#include <optional>

#include "nso_rr.h"

namespace idni::tau_lang {

template <typename... BAs>
tau<BAs...> trim(const tau<BAs...>& n);

template <typename... BAs>
tau<BAs...> trim2(const tau<BAs...>& n);

template <typename... BAs>
tau<BAs...> wrap(tau_parser::nonterminal nt, const std::vector<tau<BAs...>>& nn);

template <typename... BAs>
tau<BAs...> wrap(tau_parser::nonterminal nt,
	const std::initializer_list<tau<BAs...>> ch);

template <typename... BAs>
tau<BAs...> wrap(tau_parser::nonterminal nt, const tau<BAs...>& n);

template <typename... BAs>
tau<BAs...> wrap(tau_parser::nonterminal nt, const tau<BAs...>& c1,
	const tau<BAs...>& c2);

template <typename... BAs>
tau<BAs...> wrap(tau_parser::nonterminal nt, const std::string& terminals);

template <typename...BAs>
struct nso_factory {

	std::optional<tau<BAs...>> parse(const std::string&, const std::string& = "") const;

	tau<BAs...> binding(const tau<BAs...>&,	const std::string& = "") const;

	std::vector<std::string> types() const;

	std::string default_type() const;

	std::string one(const std::string& type_name) const;

	std::string zero(const std::string& type_name) const;

	tau<BAs...> splitter_one(const std::string& = "") const;

	static nso_factory<BAs...>& instance();
};

// creates a specific rule from a generic rule
// TODO (LOW) should depend in node_t instead of BAs...
template <typename... BAs>
rewriter::rule<tau<BAs...>> make_rule(const tau<BAs...>& rule);

// create a set of rules from a given tau source.
// TODO (LOW) should depend in node_t instead of BAs...
template <typename... BAs>
rules<tau<BAs...>> make_rules(tau<BAs...>& tau_source);

// create a set of relations from a given tau source.
// TODO (LOW) should depend in node_t instead of BAs...
template <typename... BAs>
rec_relations<tau<BAs...>> make_rec_relations(
	const tau<BAs...>& tau_source);

// make a tau source from the given source code string.
sp_tau_source_node make_tau_source(const std::string& source,
	idni::parser<>::parse_options options = {});

// make a tau source from the given source code stream.
sp_tau_source_node make_tau_source(std::istream& is,
	idni::parser<>::parse_options options = {});

// make a tau source from the given source code stream.
sp_tau_source_node make_tau_source_from_file(const std::string& filename,
	idni::parser<>::parse_options options = {});

// create tau code from tau source
template <typename... BAs>
// TODO (LOW) should depend on node_t instead of BAs...
tau<BAs...> make_tau_code(sp_tau_source_node& tau_source);

// make a library from the given tau source.
// TODO (LOW) should depend on node_t instead of BAs...
template <typename... BAs>
library<tau<BAs...>> make_library(sp_tau_source_node& tau_source);

// make a library from the given tau source string.
// TODO (LOW) should depend on node_t instead of BAs...
template <typename... BAs>
library<tau<BAs...>> make_library(const std::string& source);

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
tau<BAs...> bind_tau_code_using_bindings(tau<BAs...>& code,
	const bindings<BAs...>& bindings);

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
tau<BAs...> bind_tau_code_using_factory(const tau<BAs...>& code);

// make a nso_rr from the given tau code
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_from_binded_code(
	const tau<BAs...>& code);

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_bindings(
	const tau<BAs...>& code, const bindings<BAs...>& bindings);

template <typename... BAs>
tau<BAs...> make_nso_using_bindings(const tau<BAs...>& code,
	const bindings<BAs...>& bindings);

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_bindings(
	sp_tau_source_node& source, const bindings<BAs...>& bindings);

template <typename... BAs>
std::optional<tau<BAs...>> make_nso_using_bindings(sp_tau_source_node& source,
	const bindings<BAs...>& bindings);

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_bindings(
	const std::string& input, const bindings<BAs...>& bindings);

// make a nso from the given tau source and bindings.
template <typename... BAs>
std::optional<tau<BAs...>> make_nso_using_bindings(
	const std::string& input, const bindings<BAs...>& bindings,
	idni::parser<>::parse_options options =  { .start = tau_parser::wff });

template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_factory(
	const tau<BAs...>& code);

template <typename... BAs>
tau<BAs...> make_nso_using_factory(const tau<BAs...>& code);

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_factory(
	sp_tau_source_node& source);

template <typename... BAs>
std::optional<tau<BAs...>> make_nso_using_factory(
	sp_tau_source_node& source);

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_factory(
	const std::string& input);

// make a nso_rr from the given tau source and bindings.
template <typename... BAs>
std::optional<tau<BAs...>> make_nso_using_factory(
	const std::string& input,
	idni::parser<>::parse_options options  =  { .start = tau_parser::wff });

} // namespace tau

#include "language.tmpl.h"

#endif // __LANGUAGE_H__