// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "nso_rr.h"
#include "execution.h"

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

// calculate fixed points called from main and replace them by their results
template <NodeType node>
struct fixed_point_transformer {
	using tau = tree<node>;
	using tt = tau::traverser;
	using type = typename node::type;

	fixed_point_transformer(const rr<node>& defs)
		: defs(defs), fpcalls(find_fpcalls(defs)) {}

	tref operator()(tref n) {
		const auto& t = tau::get(n);
		if (!t.has_child()) return n;
		if (auto it = changes.find(n); it != changes.end())
			return it->second;
		tref ref = t.first();
		bool is_ref = (t.is(tau::wff) && is<node, tau::wff_ref>(ref))
			|| (t.is(tau::bf) && is<node, tau::bf_ref>(ref));
		if (!is_ref) return n;
		auto sig = get_rr_sig<node>(ref);
		if (auto fpopt = fpcall(sig); fpopt) { // is fp call
			auto offset_arity = fpopt.value().offset_arity;
			// TODO we don't support FP calc for multiindex offsets yet
			if (offset_arity > 1) {
				LOG_ERROR << "Fixed point"
					" calculation of multiindex offset "
					"relations is not supported yet";
				return nullptr;
			}
			auto typ = t.get_type();
			auto fp = calculate_fixed_point<node>(defs, n, typ,
				offset_arity, get_fallback(typ, ref));
			if (!fp) return nullptr;
			return changes.emplace(n, fp).first->second;
		}
		bool changed = false;
		trefs ch;
		if (changes.contains(ref))
			changed = true, ch.push_back(changes[ref]);
		else ch.push_back(ref);
		auto nn = tau::get(t.value, ch);
		if (changed) changes[n] = nn;
		return nn;
	}

	tref get_fallback(type nt, tref ref) {
		auto fallback = tt(ref) | tau::ref | tau::fp_fallback;
		if (!fallback) return nt == tau::wff
		// TODO: Review getting type from ref
			? tau::_F() : tau::_0(find_ba_type<node>(ref));
		return fallback | tt::only_child | tt::ref;
	}

	std::unordered_map<rr_sig, rr_sig> find_fpcalls(const rr<node>& defs) {
		std::unordered_map<rr_sig, rr_sig> fpcalls;
		for (const auto& [head, _] : defs.rec_relations) {
			rr_sig sig = get_rr_sig<node>(tau::trim(head->get()));
			DBG(LOG_TRACE << "looking for a fp call for " << LOG_FM(tau::trim(head->get()));)
			DBG(LOG_TRACE << "signature " << LOG_RR_SIG(sig);)
			// TODO (LOW) decide how to call fp calculation for various
			// offset arity rels with otherwise same signature.
			// We currently call the rel with the least offset arity.
			// Should we provide a way how to specify exact relation to call?
			if (sig.offset_arity > 0) {
				rr_sig fp_sig(sig);
				fp_sig.offset_arity = 0;
				if (auto fp_exists = fpcall(fp_sig); fp_exists) {
					DBG(LOG_TRACE << "FP call " << LOG_RR_SIG(fp_sig) << " -> " << LOG_RR_SIG(sig) << " exists";)
					if (sig.offset_arity < fp_exists.value().offset_arity)
						fpcalls[fp_sig] = sig;
				} else {
					DBG(LOG_TRACE << "FP call " << LOG_RR_SIG(fp_sig) << " -> " << LOG_RR_SIG(sig) << " does not exist, adding";)
					fpcalls.emplace(fp_sig, sig);
				}
			} DBG(else LOG_TRACE << "FP call for " << LOG_RR_SIG(sig) << " not needed, offset arity is 0";)
		}
#ifdef DEBUG
		LOG_TRACE << fpcalls.size() << " FP calls";
		for (const auto& [fp_sig, sig] : fpcalls)
			LOG_TRACE << "FP call: " << LOG_RR_SIG(fp_sig)
				<< " -> " << LOG_RR_SIG(sig);
#endif
		return fpcalls;
	}

	// returns ref to calculate fp by provided by fp call sig, or no value
	std::optional<rr_sig> fpcall(const rr_sig& fp_sig) const {
		if (auto it = fpcalls.find(fp_sig); it != fpcalls.end())
			return { it->second };
		return {};
	}

	subtree_map<node, tref> changes;
	rr<node> defs;
	std::unordered_map<rr_sig, rr_sig> fpcalls;
};

template <NodeType node>
tref calculate_all_fixed_points(const rr<node>& nso_rr) {
	if (!is_valid<node>(nso_rr)) return nullptr;
	// transform fp calculation calls by calculation results
	fixed_point_transformer<node> fpt(nso_rr);
	tref new_main = rewriter::post_order_traverser<node, decltype(fpt),
		decltype(all)>(fpt, all)(nso_rr.main->get());
	if (!new_main) return nullptr;
	if (fpt.changes.size()) {
		new_main = rewriter::replace<node>(new_main, fpt.changes);
		LOG_DEBUG << "Calculated fixed points.";
		LOG_DEBUG << "New main: " << LOG_FM(new_main);
	}
	return new_main;
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
