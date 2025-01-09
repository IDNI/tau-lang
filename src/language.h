// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __LANGUAGE_H__
#define __LANGUAGE_H__

#include <optional>

#include "nso_rr.h"

namespace idni::tau_lang {

/**
 * Trims the given tau object.
 * @param n The tau object to be trimmed.
 * @return The trimmed tau object.
 */
template <typename... BAs>
tau<BAs...> trim(const tau<BAs...>& n);

/**
 * Trims twice the given tau object.
 * @param n The tau object to be trimmed.
 * @return The trimmed tau object.
 */
template <typename... BAs>
tau<BAs...> trim2(const tau<BAs...>& n);

/**
 * Wraps the given nonterminal and tau objects into a new tau object.
 * @param nt The nonterminal to wrap.
 * @param nn The vector of tau objects to wrap.
 * @return The wrapped tau object.
 */
template <typename... BAs>
tau<BAs...> wrap(tau_parser::nonterminal nt, const std::vector<tau<BAs...>>& nn);

/**
 * Wraps the given nonterminal and initializer list of tau objects into a new tau object.
 * @param nt The nonterminal to wrap.
 * @param ch The initializer list of tau objects to wrap.
 * @return The wrapped tau object.
 */
template <typename... BAs>
tau<BAs...> wrap(tau_parser::nonterminal nt,
	const std::initializer_list<tau<BAs...>> ch);

/**
 * Wraps the given nonterminal and tau object into a new tau object.
 * @param nt The nonterminal to wrap.
 * @param n The tau object to wrap.
 * @return The wrapped tau object.
 */
template <typename... BAs>
tau<BAs...> wrap(tau_parser::nonterminal nt, const tau<BAs...>& n);

/**
 * Wraps the given nonterminal and two tau objects into a new tau object.
 * @param nt The nonterminal to wrap.
 * @param c1 The first tau object to wrap.
 * @param c2 The second tau object to wrap.
 * @return The wrapped tau object.
 */
template <typename... BAs>
tau<BAs...> wrap(tau_parser::nonterminal nt, const tau<BAs...>& c1,
	const tau<BAs...>& c2);

/**
 * Wraps the given nonterminal and string of terminals into a new tau object.
 * @param nt The nonterminal to wrap.
 * @param terminals The string of terminals to wrap.
 * @return The wrapped tau object.
 */
template <typename... BAs>
tau<BAs...> wrap(tau_parser::nonterminal nt, const std::string& terminals);

/**
 * Factory for creating and manipulating tau objects.
 */
template <typename... BAs>
struct nso_factory {

	/**
	 * Parses the given string into a tau object.
	 * @param input The string to parse.
	 * @param options Optional parse options.
	 * @return The parsed tau object, or std::nullopt if parsing fails.
	 */
	std::optional<tau<BAs...>> parse(const std::string& input,
		const std::string& options = "") const;

	/**
	 * Binds the given tau object with the specified type.
	 * @param n The tau object to bind.
	 * @param type The type to bind with.
	 * @return The bound tau object.
	 */
	tau<BAs...> binding(const tau<BAs...>& n, const std::string& type = "") const;

	/**
	 * Returns a vector of available types.
	 * @return A vector of type names.
	 */
	std::vector<std::string> types() const;

	/**
	 * Returns the default type.
	 * @return The default type name.
	 */
	std::string default_type() const;

	/**
	 * Returns a string representation of one for the given type.
	 * @param type_name The type name.
	 * @return The string representation of one.
	 */
	std::string one(const std::string& type_name) const;

	/**
	 * Returns a string representation of zero for the given type.
	 * @param type_name The type name.
	 * @return The string representation of zero.
	 */
	std::string zero(const std::string& type_name) const;

	/**
	 * Returns a tau object representing a splitter with one.
	 * @param type Optional type name.
	 * @return The splitter tau object.
	 */
	tau<BAs...> splitter_one(const std::string& type = "") const;

	/**
	 * Returns the singleton instance of the factory.
	 * @return The singleton instance.
	 */
	static nso_factory<BAs...>& instance();
};

/**
 * Creates a specific rule from a generic rule.
 * @param rule The generic rule.
 * @return The specific rule.
 */
template <typename... BAs>
rewriter::rule<tau<BAs...>> make_rule(const tau<BAs...>& rule);

/**
 * Creates a set of rules from a given tau source.
 * @param tau_source The tau source.
 * @return The set of rules.
 */
template <typename... BAs>
rules<tau<BAs...>> make_rules(tau<BAs...>& tau_source);

/**
 * Creates a set of relations from a given tau source.
 * @param tau_source The tau source.
 * @return The set of relations.
 */
template <typename... BAs>
rec_relations<tau<BAs...>> make_rec_relations(const tau<BAs...>& tau_source);

/**
 * Creates a tau source from the given source code string.
 * @param source The source code string.
 * @param options Optional parse options.
 * @return The tau source node.
 */
sp_tau_source_node make_tau_source(const std::string& source,
	idni::parser<>::parse_options options = {});

/**
 * Creates a tau source from the given source code stream.
 * @param is The input stream.
 * @param options Optional parse options.
 * @return The tau source node.
 */
sp_tau_source_node make_tau_source(std::istream& is,
	idni::parser<>::parse_options options = {});

/**
 * Creates a tau source from the given source code file.
 * @param filename The file name.
 * @param options Optional parse options.
 * @return The tau source node.
 */
sp_tau_source_node make_tau_source_from_file(const std::string& filename,
	idni::parser<>::parse_options options = {});

/**
 * Creates tau code from the given tau source.
 * @param tau_source The tau source node.
 * @return The tau code.
 */
template <typename... BAs>
tau<BAs...> make_tau_code(sp_tau_source_node& tau_source);

/**
 * Creates a library from the given tau source.
 * @param tau_source The tau source node.
 * @return The library.
 */
template <typename... BAs>
library<tau<BAs...>> make_library(sp_tau_source_node& tau_source);

/**
 * Creates a library from the given tau source string.
 * @param source The source string.
 * @return The library.
 */
template <typename... BAs>
library<tau<BAs...>> make_library(const std::string& source);

/**
 * Binds tau code using the given bindings.
 * @param code The tau code.
 * @param bindings The bindings.
 * @return The bound tau code.
 */
template <typename... BAs>
tau<BAs...> bind_tau_code_using_bindings(tau<BAs...>& code,
	const bindings<BAs...>& bindings);

/**
 * Binds tau code using the factory.
 * @param code The tau code.
 * @return The bound tau code.
 */
template <typename... BAs>
tau<BAs...> bind_tau_code_using_factory(const tau<BAs...>& code);

/**
 * Creates a nso_rr from the given tau code.
 * @param code The tau code.
 * @return The nso_rr, or std::nullopt if creation fails.
 */
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_from_binded_code(
	const tau<BAs...>& code);

/**
 * Creates a nso_rr from the given tau code and bindings.
 * @param code The tau code.
 * @param bindings The bindings.
 * @return The nso_rr, or std::nullopt if creation fails.
 */
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_bindings(const tau<BAs...>& code,
	const bindings<BAs...>& bindings);

/**
 * Creates a nso from the given tau code and bindings.
 * @param code The tau code.
 * @param bindings The bindings.
 * @return The nso, or std::nullopt if creation fails.
 */
template <typename... BAs>
tau<BAs...> make_nso_using_bindings(const tau<BAs...>& code,
	const bindings<BAs...>& bindings);

/**
 * Creates a nso_rr from the given tau source and bindings.
 * @param source The tau source node.
 * @param bindings The bindings.
 * @return The nso_rr, or std::nullopt if creation fails.
 */
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_bindings(
	sp_tau_source_node& source, const bindings<BAs...>& bindings);

/**
 * Creates a nso from the given tau source and bindings.
 * @param source The tau source node.
 * @param bindings The bindings.
 * @return The nso, or std::nullopt if creation fails.
 */
template <typename... BAs>
std::optional<tau<BAs...>> make_nso_using_bindings(sp_tau_source_node& source,
	const bindings<BAs...>& bindings);

/**
 * Creates a nso_rr from the given input string and bindings.
 * @param input The input string.
 * @param bindings The bindings.
 * @return The nso_rr, or std::nullopt if creation fails.
 */
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_bindings(
	const std::string& input, const bindings<BAs...>& bindings);

/**
 * Creates a nso from the given input string and bindings.
 * @param input The input string.
 * @param bindings The bindings.
 * @param options Optional parse options.
 * @return The nso, or std::nullopt if creation fails.
 */
template <typename... BAs>
std::optional<tau<BAs...>> make_nso_using_bindings(const std::string& input,
	const bindings<BAs...>& bindings,
	idni::parser<>::parse_options options = { .start = tau_parser::wff });

/**
 * Creates a nso_rr using the factory from the given tau code.
 * @param code The tau code.
 * @return The nso_rr, or std::nullopt if creation fails.
 */
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_factory(const tau<BAs...>& code);

/**
 * Creates a nso using the factory from the given tau code.
 * @param code The tau code.
 * @return The nso, or std::nullopt if creation fails.
 */
template <typename... BAs>
tau<BAs...> make_nso_using_factory(const tau<BAs...>& code);

/**
 * Creates a nso_rr using the factory from the given tau source node.
 * @param source The tau source node.
 * @return The nso_rr, or std::nullopt if creation fails.
 */
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_factory(sp_tau_source_node& source);

/**
 * Creates a nso using the factory from the given tau source node.
 * @param source The tau source node.
 * @return The nso, or std::nullopt if creation fails.
 */
template <typename... BAs>
std::optional<tau<BAs...>> make_nso_using_factory(sp_tau_source_node& source);

/**
 * Creates a nso_rr using the factory from the given input string.
 * @param input The input string.
 * @return The nso_rr, or std::nullopt if creation fails.
 */
template <typename... BAs>
std::optional<rr<tau<BAs...>>> make_nso_rr_using_factory(const std::string& input);

/**
 * Creates a nso using the factory from the given input string.
 * @param input The input string.
 * @param options Optional parse options.
 * @return The nso, or std::nullopt if creation fails.
 */
template <typename... BAs>
std::optional<tau<BAs...>> make_nso_using_factory(const std::string& input,
	idni::parser<>::parse_options options = { .start = tau_parser::wff });

} // namespace tau