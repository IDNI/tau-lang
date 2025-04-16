// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __HOOKS_DEPRECIATING_H__
#define __HOOKS_DEPRECIATING_H__

#include <optional>
#include "nso_rr_depreciating.h"
#include "builders_depreciating.h"

namespace idni::tau_lang_depreciating {

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
	 * @return std::optional<tau_<BAs...>> The created tau node, or std::nullopt if creation fails.
	 */
	std::optional<tau_<BAs...>> operator()(
		const rewriter::depreciating::node<tau_sym<BAs...>>& n);
};

} // namespace idni::tau_lang

namespace idni::rewriter::depreciating  {

/**
 * @brief Template specialization for creating a node hook for tau_sym.
 *
 * This struct defines an operator() that takes a rewriter node and returns
 * an optional tau node.
 *
 * @tparam BAs Variadic template parameters.
 */
template <typename...BAs>
struct make_node_hook<idni::tau_lang_depreciating::tau_sym<BAs...>> {
	/**
	 * @brief Creates a tau node from a rewriter node.
	 *
	 * @param n The rewriter node to be converted.
	 * @return std::optional<tau_<BAs...>> The created tau node, or std::nullopt if creation fails.
	 */
	std::optional<idni::tau_lang_depreciating::tau_<BAs...>> operator()(
		const rewriter::depreciating::node<idni::tau_lang_depreciating::tau_sym<BAs...>>& n);
};

} // namespace idni::tau_lang_depreciating	

#include "hooks_depreciating.tmpl.h"

#endif // __HOOKS_DEPRECIATING_H__