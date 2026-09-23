// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "nso_rr.h"
#include "execution.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "nso_rr"

namespace idni::tau_lang {

template <NodeType node>
std::unordered_map<std::string, size_t>& rule_apply_counts() {
	static std::unordered_map<std::string, size_t> m;
	return m;
}

template <NodeType node>
std::unordered_map<std::string, size_t>& rule_hit_counts() {
	static std::unordered_map<std::string, size_t> m;
	return m;
}

template <NodeType node>
void flush_rule_counts(report& rep) {
	auto& applies = rule_apply_counts<node>();
	auto& hits = rule_hit_counts<node>();
	for (auto& [name, count] : applies)
		rep.count(name + " applications", count);
	for (auto& [name, count] : hits)
		rep.count(name + " hits", count);
	applies.clear();
	hits.clear();
}

// Returns @p r with its body's bound variables renamed clear of every bound
// variable of @p n, so that applying it cannot capture an argument taken from
// @p n. A body without bound variables (the common case) is returned as is,
// without scanning @p n.
template <NodeType node>
rewriter::rule alpha_shift_rule_body(const rewriter::rule& r, tref n) {
	using tau = tree<node>;
	tref body = r.second->get();
	if (max_bound_var_id<node>(body) == 0) return r;
	tref shifted = shift_bound_var_ids<node>(body, max_bound_var_id<node>(n));
	if (shifted == body) return r;
	return { r.first, tau::geth(shifted) };
}

template <NodeType node>
tref nso_rr_apply(const rewriter::rule& r, const tref& n) {
	static const auto is_capture = [](const tref& n) {
		return tree<node>::get(n).is(node::type::capture);
	};

#ifdef TAU_CACHE
	// apply_rule below rebuilds nodes via ordinary construction, which runs
	// a widening-capable BA's own construction-time folding hooks -- key on
	// pack_widening_active so a stale entry built under one setting is not
	// replayed after the setting flips.
	using cache_t = std::map<std::tuple<rewriter::rule, tref, bool>, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	const bool widening = pack_widening_active<node>();
	if (auto it = cache.find({r, n, widening}); it != cache.end())
		return it->second;
#endif // TAU_CACHE

	// A definition body carries bound-variable ids numbered per formula
	// (canonize_quantifier_ids), so the body's `ex 1` and a call site's
	// `all 1` are the same name: splicing the body in as it stands would
	// capture the argument. Move the body's ids above every id n uses.
	// Recomputed per application -- an expansion earlier in the same round
	// may itself have introduced shifted ids. The result is renumbered once
	// the expansion has settled, in the rr overload below.
	const rewriter::rule ar = alpha_shift_rule_body<node>(r, n);
	auto nn = rewriter::apply_rule<node, decltype(is_capture)>(
							ar, n, is_capture);
		if (rule_counting) {
			auto name = to_str<node>(r);
			++rule_apply_counts<node>()[name];
			if (n != nn) ++rule_hit_counts<node>()[name];
		}
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

#ifdef TAU_CACHE
		cache[{r, n, widening}] = nn;
#endif // TAU_CACHE

		return nn;
}

template <NodeType node>
tref nso_rr_apply(const rewriter::rules& rs, tref n) {
#ifdef TAU_CACHE
	// See the single-rule overload above: key on pack_widening_active for
	// the same reason.
	using cache_t = std::map<std::tuple<rewriter::rules, tref, bool>, tref>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	const bool widening = pack_widening_active<node>();
	if (auto it = cache.find({rs, n, widening}); it != cache.end())
		return it->second;
#endif // TAU_CACHE

	if (rs.empty()) return n;
	tref nn = n;
	for (auto& r : rs) nn = nso_rr_apply<node>(r, nn);

#ifdef TAU_CACHE
	cache[{rs, n, widening}] = nn;
#endif // TAU_CACHE
	return nn;
}

// Bottom-up tree transformer that finds fixpoint calls in a formula (an
// offset-free reference to a recurrence defined with offsets, e.g. `g(y)`
// for `g[n](x) := ...`), runs calculate_fixed_point on each, and records
// the results in `changes` for the caller to substitute. Non-call nodes
// are rebuilt with their children's recorded replacements. Applied by
// calculate_all_fixed_points below over a spec's main formula.
template <NodeType node>
struct fixed_point_transformer {
	using tau = tree<node>;
	using tt = tau::traverser;
	using type = typename node::type;

	fixed_point_transformer(const rr<node>& defs)
		: defs(defs), fpcalls(find_fpcalls(defs)) {}

	// Visitor step: when @p n is a wff- or bf-wrapped reference whose
	// signature names a fixpoint call (see find_fpcalls), calculate the
	// fixpoint in @p n's own world (wff or bf, from the wrapper) and
	// memoize it in `changes`; otherwise rebuild @p n from its children's
	// recorded replacements. Returns nullptr on an unsupported
	// multi-index call or a failed fixpoint calculation, which aborts the
	// traversal.
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
			// well-foundedness needs the full reachable definition
			// set, so `defs` stays unfiltered here -- only the
			// type-mismatch check inside narrows to this call's
			// own signature, via call_sig.
			auto fp = calculate_fixed_point<node>(defs, n, typ,
				offset_arity, get_fallback(typ, ref),
				fpopt.value());
			if (!fp) return nullptr;
			return changes.emplace(n, fp).first->second;
		}
		// RR-6: `changes` is only ever keyed by the parent nodes, so
		// the old contains(ref) propagation branch here was dead --
		// the rebuild always returned the identical canonical node.
		return n;
	}

	// The value calculate_fixed_point returns when the enumeration loops:
	// the ref's own `fallback` clause if it carries one, else F for
	// predicate (wff) calls and the typed 0 for function (bf) calls.
	tref get_fallback(type nt, tref ref) {
		auto fallback = tt(ref) | tau::ref | tau::fp_fallback;
		if (!fallback) return nt == tau::wff
		// TODO: Review getting type from ref
			? tau::_F() : tau::_0(find_ba_type<node>(ref));
		return fallback | tt::only_child | tt::ref;
	}

	// Builds the fixpoint-call table from the definitions: for every
	// rule head with a nonzero offset arity, maps the OFFSET-FREE
	// signature (same name and argument arity, offset arity 0) to the
	// indexed one -- that offset-free form is how a fixpoint call is
	// written. When several offset arities share a name, the least one
	// wins (see the in-body TODO).
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

// Replaces every fixpoint call in @p nso_rr's main formula by its
// calculated fixpoint (or fallback) value, leaving the rest of the
// formula unchanged. Returns the rewritten main, or nullptr when the
// spec is invalid or any fixpoint calculation fails (multi-index call,
// non-well-founded definitions, exhausted enumeration budget, or rules
// that never apply to the call).
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

// Turns @p nso_rr's definitions into applicable rewrite rules: variables
// in offset positions and in reference arguments become captures, so a
// rule head matches any call. In each rule's HEAD every variable
// argument is converted while being collected; in its BODY only the
// variables that appeared in the head are converted (a body variable of
// its own is a value, not a pattern hole). The main formula only gets
// its offset variables converted. IO stream variables are never touched
// (they are concrete streams, not pattern holes). Types carried by the
// converted arguments are preserved on the rebuilt nodes.
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
			// If we collect head variables, save it; in a body
			// only a variable the head declared is a pattern hole
			// -- a bound variable of the body handed to a nested
			// call (`p(a) := ex c q(c)`) is a value, and turning
			// it into a capture would leave an unbound capture in
			// the expansion.
			if (collecting) head_vars.insert(tau::trim(n));
			else if (!head_vars.contains(tau::trim(n))) return n;
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
result<tref> nso_rr_apply(const rr<node>& nso_rr) {
	result<tref> r;
	LOG_DEBUG << "Start nso_rr_apply";
	LOG_DEBUG << "Spec: " << LOG_RR(nso_rr);
	rr<node> rr_ = transform_ref_args_to_captures<node>(nso_rr);
	tref main = r.measure("calculate_fixed_points", [&] {
		return calculate_all_fixed_points<node>(rr_);
	});
	if (!main) {
		return r.with_assert_check_error(code::internal_error,
			"fixed point calculation did not terminate");
	}
	// Substitute function and recurrence relation definitions. Called
	// directly rather than through the traverser pipe so that the
	// non-termination signal (nullptr) is checked instead of being fed to
	// `tt::ref`.
	tref new_main = r.measure("apply_rec_relations", [&] {
		return repeat_all<node, step<node>>(step<node>(rr_.rec_relations))(main);
	});
	if (!new_main) {
		return r.with_assert_check_error(code::internal_error,
			"recurrence relation rewriting did not reach a fixed point");
	}
	// The expanded bodies came in with their ids shifted out of the way
	// (alpha_shift_rule_body). Renumber so the formula leaves with the
	// canonical numbering every other pass assumes: within a scope the
	// outer quantifier holds the larger id, which is what lets
	// find_biggest_quant_id stop at the outermost one.
	if (new_main != main)
		new_main = canonize_quantifier_ids<node>(new_main);
	LOG_DEBUG << "End nso_rr_apply";
	LOG_DEBUG << "Spec: " << LOG_RR(nso_rr);
	LOG_DEBUG << "New main: " << LOG_FM(new_main);
	return r.with_assert_check_value(new_main);
}

} // namespace idni::tau_lang
