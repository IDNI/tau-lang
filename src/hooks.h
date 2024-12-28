// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __HOOKS_H__
#define __HOOKS_H__

#include <optional>

#include "nso_rr.h"
#include "builders.h"

namespace idni::tau_lang {

template <typename...BAs>
struct make_tau_node {
	std::optional<tau<BAs...>> operator()(
		const rewriter::node<tau_sym<BAs...>>& n);
};

} // namespace idni::tau_lang

namespace idni::rewriter {

template <typename...BAs>
struct make_node_hook<idni::tau_lang::tau_sym<BAs...>> {
	std::optional<idni::tau_lang::tau<BAs...>> operator()(
		const rewriter::node<idni::tau_lang::tau_sym<BAs...>>& n);
};

} // namespace idni::rewriter

#include "hooks.tmpl.h"

#endif // __HOOKS_H__