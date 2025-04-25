// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

// TODO (LOW) rename file to msnso_rr.h
// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of sp_tau_node,...

#ifndef __IDNI__TAU__NSO_RR_H__
#define __IDNI__TAU__NSO_RR_H__

// #include <map>
// #include <variant>
// #include <string>
// #include <vector>
// #include <memory>
// #include <optional>
// #include <algorithm>
// #include <functional>
// #include <ranges>
// #include <numeric>

#include "tau_tree.h"
// #include "boolean_algebras/bool_ba.h"
// #include "boolean_algebras/variant_ba.h"
#include "splitter_types.h"
// #include "utils.h"

#ifdef TAU_MEASURE
#include "measure.h"
#endif // TAU_MEASURE

namespace idni::tau_lang {

// apply one tau rule to the given expression
// IDEA maybe this could be operator|
template <NodeType node>
tref nso_rr_apply(const rewriter::rule& r, const tref& n) {
	static const auto is_capture = [](const tref& n) {
		return tree<node>::get(n).is(node::type::capture);
	};

	#ifdef TAU_CACHE
	static std::map<std::pair<rewriter::rule, tref>, tref> cache;
	if (auto it = cache.find({r, n}); it != cache.end()) return it->second;
	#endif // TAU_CACHE

	#ifdef TAU_MEASURE
	measures::increase_rule_counter<tau_<BAs...>>(r);
	#endif // TAU_MEASURE

	try {
		auto nn = rewriter::apply_rule<node, decltype(is_capture)>(
							r, n, is_capture);

		#ifdef TAU_MEASURE
		if (n != nn) measures::increase_rule_hit<tau_<BAs...>>(r);
		#endif // TAU_MEASURE

		#ifdef TAU_CACHE
		cache[{r, n}] = nn;
		#endif // TAU_CACHE

		return nn;
	} catch (const std::exception& e) {
		BOOST_LOG_TRIVIAL(warning) << "(Warning) " << e.what();
		return n;
	}
}

// apply the given rules to the given expression
// IDEA maybe this could be operator|
template <NodeType node>
tref nso_rr_apply(const rewriter::rules& rs, tref n) {
	#ifdef TAU_CACHE
	static std::map<std::pair<rewriter::rules, tref>, tref> cache;
	if (auto it = cache.find({rs, n}); it != cache.end()) return it->second;
	#endif // TAU_CACHE

	if (rs.empty()) return n;
	tref nn = n;
	for (auto& r : rs) nn = nso_rr_apply<node>(r, nn);

	#ifdef TAU_CACHE
	cache[{rs, n}] = nn;
	#endif // TAU_CACHE
	return nn;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__NSO_RR_H__
