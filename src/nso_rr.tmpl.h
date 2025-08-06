// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "nso_rr.h"

#ifdef TAU_MEASURE
#include "utility/measure.h"
#endif // TAU_MEASURE

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "nso_rr"

namespace idni::tau_lang {

template <NodeType node>
tref nso_rr_apply(const rewriter::rule& r, const tref& n) {
	static const auto is_capture = [](const tref& n) {
		return tree<node>::get(n).is(node::type::capture);
	};

#ifdef TAU_CACHE
	using cache_t = std::map<std::pair<rewriter::rule, tref>, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find({r, n}); it != cache.end()) return it->second;
#endif // TAU_CACHE

#ifdef TAU_MEASURE
	measures::increase_rule_counter<tau_<BAs...>>(r);
#endif // TAU_MEASURE

	try {
		auto nn = rewriter::apply_rule<node, decltype(is_capture)>(
							r, n, is_capture);
#ifdef DEBUG
		if (n != nn) {
			LOG_TRACE << "--------------------------------";
			LOG_TRACE << "rule:       " << LOG_RULE(r);
			LOG_TRACE << "applied to: " << TAU_TO_STR(n);
			LOG_TRACE << "result:     " << LOG_FM(nn);
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
	using cache_t = std::map<std::pair<rewriter::rules, tref>, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
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

template <NodeType node>
rr<node> transform_ref_args_to_captures(const rr<node>& nso_rr) {
	using tau = tree<node>;
	LOG_TRACE << "-- transform_ref_args_to_captures: " << LOG_RR(nso_rr);
	auto transformer = [](tref n) -> tref {
		const auto& t = tau::get(n);
		if (t.is(tau::offset) && t[0].is(tau::variable))
			return tau::get(tau::offset,
				tau::get(node(tau::capture, t[0][0].data())));
		if (t.is(tau::shift) && t[0].is(tau::variable))
			return tau::get(tau::shift, {
				tau::get(node(tau::capture, t[0][0].data())),
				t[0].right_sibling() });
		return n;
	};
	auto def_transformer = [&transformer](tref n) -> tref {
		const auto& t = tau::get(n);
		if (t.is(tau::ref_arg) && t[0][0].is(tau::variable))
			return tau::get(tau::ref_arg, tau::get(tau::bf,
					tau::get(node(tau::capture,
							t[0][0][0].data()))));
		if (t.is(tau::bf) && t[0].is(tau::variable))
			return tau::get(tau::bf,
				tau::get(node(tau::capture,
						t[0][0].data())));
		return n;
	};
	auto transform = [&](const htref& h, bool def = false) {
		tref n = pre_order<node>(h->get())
				.apply_unique_until_change(transformer);
		if (def) n = pre_order<node>(n)
				.apply_unique_until_change(def_transformer);
		if (n != h->get()) return tau::geth(n);
		return h;
	};
	rr<node> ret(nso_rr);
	for (auto& r : ret.rec_relations) r.first = transform(r.first, true),
					  r.second = transform(r.second, true);
	ret.main = transform(nso_rr.main);
	LOG_TRACE << "-- transform_ref_args_to_captures result: " << LOG_RR_DUMP(ret);
	return ret;
}

} // namespace idni::tau_lang
