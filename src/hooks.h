// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __HOOKS_H__
#define __HOOKS_H__

#include <optional>
#include "nso_rr.h"
#include "debug_helpers.h"
#include "builders.h"

namespace idni::tau_lang {

/**
 * @brief Template struct to create a tau node.
 *
 * This struct defines an operator() that takes a rewriter node and returns
 * an optional tau node.
 *
 * @tparam BAs Variadic template parameters.
 */
template <typename...BAs>
struct make_tau_node {
	/**
	 * @brief Creates a tau node from a rewriter node.
	 *
	 * @param n The rewriter node to be converted.
	 * @return std::optional<tau<BAs...>> The created tau node, or std::nullopt if creation fails.
	 */
	std::optional<tau<BAs...>> operator()(
		const rewriter::node<tau_sym<BAs...>>& n);
};

} // namespace idni::tau_lang

namespace idni::rewriter {

/**
 * @brief Template specialization for creating a node hook for tau_sym.
 *
 * This struct defines an operator() that takes a rewriter node and returns
 * an optional tau node.
 *
 * @tparam BAs Variadic template parameters.
 */
template <typename...BAs>
struct make_node_hook<idni::tau_lang::tau_sym<BAs...>> {
	/**
	 * @brief Creates a tau node from a rewriter node.
	 *
	 * @param n The rewriter node to be converted.
	 * @return std::optional<idni::tau_lang::tau<BAs...>> The created tau node, or std::nullopt if creation fails.
	 */
	std::optional<idni::tau_lang::tau<BAs...>> operator()(
		const rewriter::node<idni::tau_lang::tau_sym<BAs...>>& n);
};

} // namespace idni::rewriter

#include "hooks.tmpl.h"

#endif // __HOOKS_H__