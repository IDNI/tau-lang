// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "nso_rr.h"

#ifdef TAU_MEASURE
#include "utility/measure.h"
#endif // TAU_MEASURE

namespace idni::tau_lang {

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
		DBG(LOG_TRACE << "try to apply rule: " << to_str<node>(r);)
		auto nn = rewriter::apply_rule<node, decltype(is_capture)>(
							r, n, is_capture);
		#ifdef DEBUG
		if (n != nn) {
			using tau = tree<node>;
			LOG_TRACE << "--------------------------------";
			LOG_TRACE << "apply to: " << TAU_TO_STR(n);
			LOG_TRACE << "result: " << TAU_TO_STR(nn);
			LOG_TRACE << "--------------------------------";
		}
		#endif // DEBUG

		#ifdef TAU_MEASURE
		if (n != nn) measures::increase_rule_hit<tau_<BAs...>>(r);
		#endif // TAU_MEASURE

		#ifdef TAU_CACHE
		cache[{r, n}] = nn;
		#endif // TAU_CACHE

		return nn;
	} catch (const std::exception& e) {
		LOG_WARNING << e.what();
		return n;
	}
}

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
