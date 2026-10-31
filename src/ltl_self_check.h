// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Self-check of a realizable verdict: the specification is executed for a
// few steps on generated inputs, and each of its top-level clauses is read on
// the trace. A clause the trace already violates shows that the strategy
// executed is not one of the specification, so the verdict is not given.
//
// The trace is finite, so a clause is read in three values: violated,
// satisfied, or open (an `always` the trace does not break, a `sometimes` it
// does not meet, an atom that reads before step 0 or after the last step).
// Only a violation counts.

#ifndef __IDNI__TAU__LTL_SELF_CHECK_H__
#define __IDNI__TAU__LTL_SELF_CHECK_H__

#include <map>
#include <optional>
#include <random>
#include <string>

namespace idni::tau_lang {

namespace self_check_detail {

enum class tv { f, t, open };

inline tv tv_not(tv a) {
	return a == tv::t ? tv::f : a == tv::f ? tv::t : tv::open;
}
inline tv tv_and(tv a, tv b) {
	if (a == tv::f || b == tv::f) return tv::f;
	if (a == tv::t && b == tv::t) return tv::t;
	return tv::open;
}
inline tv tv_or(tv a, tv b) { return tv_not(tv_and(tv_not(a), tv_not(b))); }

// The values a run gave each stream, by name and step.
template <NodeType node>
struct trace {
	std::map<std::pair<std::string, int_t>, htref> values;
	int_t length = 0;
};

// fm's value at step t of tr.
template <NodeType node>
tv eval(tref fm, int_t t, const trace<node>& tr) {
	using tau = tree<node>;
	const auto& n = tau::get(fm);
	if (n.equals_T()) return tv::t;
	if (n.equals_F()) return tv::f;
	if (!n.has_child()) return tv::open;
	const auto& op = n[0];
	auto sub = [&](size_t i, int_t at) { return eval<node>(op.child(i), at, tr); };
	// The disjunction over the steps u of "left up to u, right at u", of the
	// negated operands when @p negated. A step whose right side is open may
	// already meet it, so a later failure of the left side decides nothing.
	auto until = [&](bool negated) {
		auto side = [&](size_t i, int_t at) {
			tv v = sub(i, at);
			return negated ? tv_not(v) : v;
		};
		tv met = tv::f, before = tv::t;
		for (int_t u = t; u < tr.length; ++u) {
			met = tv_or(met, tv_and(before, side(1, u)));
			if (met == tv::t) return tv::t;
			before = tv_and(before, side(0, u));
			if (before == tv::f) return met;
		}
		return tv::open;
	};
	switch (op.get_type()) {
	case tau::wff_neg: return tv_not(sub(0, t));
	case tau::wff_and: {
		tv v = tv::t;
		for (size_t i = 0; i < op.children_size(); ++i)
			v = tv_and(v, sub(i, t));
		return v;
	}
	case tau::wff_or: {
		tv v = tv::f;
		for (size_t i = 0; i < op.children_size(); ++i)
			v = tv_or(v, sub(i, t));
		return v;
	}
	case tau::wff_imply: return tv_or(tv_not(sub(0, t)), sub(1, t));
	case tau::wff_rimply: return tv_or(sub(0, t), tv_not(sub(1, t)));
	case tau::wff_equiv: case tau::wff_xor: {
		tv a = sub(0, t), b = sub(1, t);
		if (a == tv::open || b == tv::open) return tv::open;
		return ((a == b) == (op.get_type() == tau::wff_equiv))
			? tv::t : tv::f;
	}
	case tau::wff_always: {
		for (int_t u = t; u < tr.length; ++u)
			if (sub(0, u) == tv::f) return tv::f;
		return tv::open;
	}
	case tau::wff_sometimes: {
		for (int_t u = t; u < tr.length; ++u)
			if (sub(0, u) == tv::t) return tv::t;
		return tv::open;
	}
	case tau::wff_until: case tau::wff_weak_until:
		return until(false);
	case tau::wff_release:
		return tv_not(until(true));
	case tau::wff_since: case tau::wff_trigger: {
		// φ S ψ: ψ at some step k <= t and φ after it; φ T ψ is
		// !(!φ S !ψ)
		const bool trig = op.get_type() == tau::wff_trigger;
		auto lit = [&](size_t i, int_t at) {
			tv v = sub(i, at);
			return trig ? tv_not(v) : v;
		};
		tv s = tv::f;
		for (int_t u = 0; u <= t; ++u)
			s = tv_or(lit(1, u), tv_and(lit(0, u), s));
		return trig ? tv_not(s) : s;
	}
	default: break;
	}
	// a formula without a temporal operator: its streams and time
	// constraints read at step t
	if (n.find_top(is_child_temporal_quantifier<node>)) return tv::open;
	subtree_map<node, tref> m;
	for (tref v : n.select_top(is_child<node, tau::io_var>)) {
		const int_t at = is_io_initial<node>(v) ? get_io_time_point<node>(v)
			: t - get_io_var_shift<node>(v);
		auto it = tr.values.find({ get_var_name<node>(v), at });
		if (at < 0 || it == tr.values.end()) return tv::open;
		m[v] = tau::trim(it->second->get());
	}
	for (tref c : n.select_top(is<node, tau::constraint>))
		m[c] = tau::trim(ctn_holds_at<node>(c, t) ? tau::_T() : tau::_F());
	tref ground = m.empty() ? fm : rewriter::replace<node>(fm, m);
	if (tau::get(ground).equals_T()) return tv::t;
	if (tau::get(ground).equals_F()) return tv::f;
	auto sat = is_non_temp_nso_satisfiable<node>(ground);
	if (!sat.has_value()) return tv::open;
	return sat.value() ? tv::t : tv::f;
}

} // namespace self_check_detail

/**
 * @brief Run @p fm for at most @p steps steps on generated inputs and read
 * each top-level clause on the trace.
 *
 * Each input of a step takes `0` or `1` of its type, drawn with a fixed
 * seed, so a check is repeatable. A clause starts at its deepest lookback,
 * the `always` clauses at the deepest of theirs (README "Lookback
 * initialization").
 * @return The text of the first clause the trace violates, with the step
 * count, or nullopt when none is (also when @p fm cannot be executed: the
 * check then has nothing to say).
 */
template <NodeType node>
result<std::optional<std::string>> ltl_self_check(tref fm, size_t steps) {
	using tau = tree<node>;
	using namespace self_check_detail;
	result<std::optional<std::string>> r;
	const htref keep_fm = tau::geth(fm);
	// the run's report is the check's own business: a run that fails tells
	// nothing about the verdict
	auto interp = api<node>::get_interpreter(fm);
	if (!interp.has_value()) return r.with_value(std::nullopt);
	// the strategies the run took from the verdict (data_game_reuse_scope)
	size_t reused = 0;
	for (const auto& n : interp.report().nodes())
		if (n.tag == code::info_count
			&& interp.report().str(n.key) == "data_games_reused")
				reused += static_cast<size_t>(n.value);
	if (reused) r.report().count("data_games_reused", reused);
	auto& i = interp.value();
	// the caller holds its trees as trefs, which a sweep would free
	i.gc_growth_factor = 0.0;
	trace<node> tr;
	std::mt19937 rng(0x5e1fc4ec);
	for (size_t k = 0; k < steps; ++k) {
		// near the node budget a step sweeps whatever gc_growth_factor says
		if (over_tref_soft_mark<node>()) break;
		auto needed = api<node>::get_inputs_for_step(i);
		if (!needed.has_value()) break;
		assignment<node> values;
		for (const auto& in : needed.value()) {
			tref var = nullptr;
			for (const auto& [v, _] : i.ctx.inputs)
				if (get_var_name<node>(v->get()) == in.name) {
					var = v->get();
					break;
				}
			if (!var) break;
			const size_t type = i.ctx.type_of(var);
			tref c = (rng() & 1) ? tau::_1(type) : tau::_0(type);
			values[build_in_var_at_n<node>(in.name,
				static_cast<int_t>(in.time_point), type)] = c;
			tr.values[{ in.name, static_cast<int_t>(in.time_point) }]
				= tau::geth(c);
		}
		const int_t now = i.time_point;
		auto stepped = i.step(values);
		if (!stepped.has_value() || !stepped.value().first) break;
		for (const auto& [out, val] : *stepped.value().first) {
			const int_t at = get_io_time_point<node>(tau::trim(out));
			if (at >= 0) tr.values[{ get_var_name<node>(out), at }]
				= tau::geth(val);
		}
		tr.length = now + 1;
	}
	if (tr.length == 0) return r.with_value(std::nullopt);
	// An input the strategy never read leaves its outputs as they are for
	// any value of it, so it is drawn here; a clause over it is then read
	// like the others instead of staying open.
	for (const auto& [v, _] : i.ctx.inputs) {
		const std::string name = get_var_name<node>(v->get());
		if (name == "this") continue;
		const size_t type = i.ctx.type_of(v->get());
		for (int_t t = 0; t < tr.length; ++t)
			if (!tr.values.contains({ name, t }))
				tr.values[{ name, t }] = tau::geth((rng() & 1)
					? tau::_1(type) : tau::_0(type));
	}

	// the clauses as tau reads them: without a future operator the formula
	// is one invariant, and a bare conjunct beside one is an `always`
	tref spec = keep_fm->get();
	if (tau::get(spec).is(tau::spec))
		spec = tau::get(spec).find_top(is<node, tau::wff>);
	if (!spec) return r.with_value(std::nullopt);
	const bool has_future = tau::get(spec).find_top(
		is_future_temporal_quantifier<node>) != nullptr;
	trefs clauses = has_future ? get_cnf_wff_clauses<node>(spec)
		: trefs{ tau::build_wff_always(spec) };
	auto lookback = [](tref c) {
		return get_max_shift<node>(tau::get(c).select_top(
			is_child<node, tau::io_var>), true);
	};
	auto is_always = [&](tref c) {
		return tau::get(c).child_is(tau::wff_always)
			|| !tau::get(c).find_top(is_future_temporal_quantifier<node>);
	};
	int_t always_start = 0;
	for (tref c : clauses)
		if (is_always(c)) always_start = std::max(always_start, lookback(c));
	for (tref c : clauses) {
		tref clause = is_always(c) && !tau::get(c).child_is(tau::wff_always)
			? tau::build_wff_always(c) : c;
		const int_t start = is_always(c) ? always_start : lookback(c);
		if (start >= tr.length) continue;
		if (eval<node>(clause, start, tr) == tv::f)
			return r.with_value(std::string("the run of ")
				+ std::to_string(tr.length) + " steps violates "
				+ truncate_for_message(TAU_TO_STR(c)));
	}
	return r.with_value(std::nullopt);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__LTL_SELF_CHECK_H__
