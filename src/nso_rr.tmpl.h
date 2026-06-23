// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

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
	LOG_TRACE << "--------------------------------";
	LOG_TRACE << "rule:       " << LOG_RULE(r);
	LOG_TRACE << "applied to: " << TAU_TO_STR(n);
	if (n != nn) {
		LOG_TRACE << "result:     " << LOG_FM(nn);
		LOG_TRACE << "--------------------------------";
	} else {
		LOG_TRACE << "no change";
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
	DBG(	LOG_TRACE << "-- transform_ref_args_to_captures:";)
	DBG(LOG_TRACE << "nso_rr main: " << tau::get(nso_rr.main->get()).tree_to_str();
		for (const auto& [k, v] : nso_rr.rec_relations) {
			LOG_TRACE << tau::get(k->get()).tree_to_str() << " -> " << tau::get(v->get()).tree_to_str();
		});
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
	subtree_set<node> head_vars;
	bool collecting = false;
	auto def_transformer = [&](tref n) -> tref {
		const auto& t = tau::get(n);
		if (t.is(tau::ref_arg) && t[0][0].is(tau::variable)) {
			// If we collect head variables, save it
			if (collecting) head_vars.insert(tau::trim(n));
			auto type = t[0][0].get_ba_type();
			return tau::get_typed(tau::ref_arg,
				tau::get_typed(tau::bf,
					tau::get(node(tau::capture,	t[0][0][0].data()))
				, type)
			, type);
		}
		if (t.is(tau::bf) && t[0].is(tau::variable)) {
			// If we do not collect head variables, check if the
			// current variable is contained in the head of the rule
			if (!collecting && !head_vars.contains(n)) return n;
			auto type = t[0].get_ba_type();
			return tau::get_typed(tau::bf,
				tau::get(node(tau::capture,	t[0][0].data())), type);
		}
		return n;
	};
	// We need to skip IO stream variables
	auto visit = [](tref n) {
		if (is_io_var<node>(n)) return false;
		return true;
	};
	auto transform = [&](const htref& h, bool def = false) {
		tref n = pre_order<node>(h->get())
				.apply_unique_until_change(transformer, visit);
		if (def) n = pre_order<node>(n)
				.apply_unique_until_change(def_transformer, visit);
		if (n != h->get()) return tau::geth(n);
		return h;
	};
	rr<node> ret(nso_rr);
	for (auto& r : ret.rec_relations) {
		head_vars.clear();
		collecting = true;
		r .first = transform(r.first, true);
		collecting = false;
		// Only convert vars to capture that appear in the head of the definition
		r.second = transform(r.second, true);
	}
	ret.main = transform(nso_rr.main);
	DBG(LOG_TRACE << "-- transform_ref_args_to_captures result: " << LOG_RR_DUMP(ret);)
	DBG(LOG_TRACE << "ret main: " << tau::get(ret.main->get()).tree_to_str();
		for (const auto& [k, v] : ret.rec_relations) {
			LOG_TRACE << tau::get(k->get()).tree_to_str() << " -> " << tau::get(v->get()).tree_to_str();
		};)
	return ret;
}

// Applies the recurrence relations the formula comes with to the formula.
// This is the rr-overload of nso_rr_apply, complementing the rule/rules overloads.
template <NodeType node>
tref nso_rr_apply(const rr<node>& nso_rr) {
	using tt = typename tree<node>::traverser;
	LOG_DEBUG << "Start nso_rr_apply";
	LOG_DEBUG << "Spec: " << LOG_RR(nso_rr);
	rr<node> rr_ = transform_ref_args_to_captures<node>(nso_rr);
	tref main = calculate_all_fixed_points<node>(rr_);
	if (!main) return nullptr;
	// Substitute function and recurrence relation definitions
	tref new_main = main
		| repeat_all<node, step<node>>(step<node>(rr_.rec_relations))
		| tt::ref;
	LOG_DEBUG << "End nso_rr_apply";
	LOG_DEBUG << "Spec: " << LOG_RR(nso_rr);
	LOG_DEBUG << "New main: " << LOG_FM(new_main);
	return new_main;
}

} // namespace idni::tau_lang
