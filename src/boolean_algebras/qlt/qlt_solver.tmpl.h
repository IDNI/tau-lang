// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file qlt_solver.tmpl.h
 * @brief qlt's solver for pure ordering systems, which the BA-level
 *        solve_inequality_system cannot handle.
 *
 * Included from qlt_descriptor.tmpl.h and nowhere else.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_SOLVER_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_SOLVER_TMPL_H__

#include "boolean_algebras/qlt/qlt.h"
#include "boolean_algebras/qlt/qlt_qe.tmpl.h"
#include "solver_types.h"

namespace idni::tau_lang {

// Pick a concrete witness rational from the first non-empty, non-symbolic piece.
// Returns nullopt if none found.
template <NodeType node>
std::optional<qlt_rational> qlt_pick_witness(const qlt& interval) {
	for (const auto& p : interval.pieces) {
		const auto& lo = p.lo.val;
		const auto& hi = p.hi.val;
		if (lo.is_sym() || hi.is_sym()) continue;
		if (lo.is_neg_inf() && hi.is_pos_inf()) return qlt_rational(0, 1);
		if (lo.is_neg_inf() && hi.is_finite())
			return qlt_rational(hi.p - hi.q, hi.q); // hi - 1
		if (lo.is_finite() && hi.is_pos_inf())
			return qlt_rational(lo.p + lo.q, lo.q); // lo + 1
		// both finite
		if (lo == hi) {
			if (p.lo.bound == qlt_bound::CLOSED && p.hi.bound == qlt_bound::CLOSED)
				return lo;
			continue; // empty degenerate interval
		}
		return lo.midpoint(hi);
	}
	return {};
}

// Joint solver for a conjunction of atoms `a op b`, each side a qlt variable,
// a finite singleton constant or typed 0 / 1, and op one of <, <=, >, >=, =,
// != or a negation of one. Over the dense order (Q,<) such a system is
// decided and solved as a whole:
//   1. `<` / `<=` atoms become strict / non-strict edges, `=` a non-strict
//      edge each way, and distinct constants are chained by strict edges in
//      their numeric order;
//   2. the transitive closure (keeping the strongest edge kind) is
//      unsatisfiable iff some node reaches itself strictly; nodes reaching
//      each other are forced equal and form one class;
//   3. `!=` inside one class is unsatisfiable; any other disequality holds
//      because distinct classes get distinct values;
//   4. classes are assigned in topological order: a constant class keeps its
//      value, any other class takes an unused rational strictly above every
//      earlier class that reaches it and strictly below every constant it
//      reaches (non-empty by density; the closure guarantees lower < upper).
// Anything outside this fragment declines with nullopt, as does an
// unsatisfiable system; the caller then falls through to its
// solve-then-verify path. Disequalities this reader cannot parse are skipped
// (see below): solve() in solver.tmpl.h checks the model against every atom.
namespace qlt_dlo_detail {

enum class rel : uint8_t { none = 0, le = 1, lt = 2 };

inline qlt_rational floor_int(const qlt_rational& r) {
	long long f = r.p >= 0 ? r.p / r.q : -((-r.p + r.q - 1) / r.q);
	return qlt_rational(f, 1);
}

// A rational strictly inside (lo, hi) -- either side optional (unbounded) --
// not contained in `used`. Integers are preferred for readable models.
inline qlt_rational pick_between(const std::optional<qlt_rational>& lo,
	const std::optional<qlt_rational>& hi,
	const std::vector<qlt_rational>& used)
{
	auto is_used = [&](const qlt_rational& r) {
		return std::find(used.begin(), used.end(), r) != used.end();
	};
	auto inside = [&](const qlt_rational& r) {
		return (!lo || *lo < r) && (!hi || r < *hi);
	};
	qlt_rational c = lo ? floor_int(*lo) + qlt_rational(1, 1)
		: hi ? floor_int(*hi) + qlt_rational(hi->q == 1 ? -1 : 0, 1)
		: qlt_rational(0, 1);
	const qlt_rational step(lo || !hi ? 1 : -1, 1);
	for (size_t i = 0; i <= used.size() && inside(c); ++i, c = c + step)
		if (!is_used(c)) return c;
	// Bounded on both sides and no free integer inside: bisect towards lo.
	// (An unbounded side always yields a free integer above.)
	DBG(assert(lo && hi);)
	qlt_rational m = lo->midpoint(*hi);
	while (is_used(m)) m = lo->midpoint(m);
	return m;
}

} // namespace qlt_dlo_detail

// The omcat_solve_inequality_system capability: solves a pure DLO ordering
// system jointly (see above), or declines.
template <NodeType node>
static std::optional<solution<node>> qlt_omcat_solve_inequality_system(
	const inequality_system<node>& sys, const solver_options& options)
{
	using tau = tree<node>;
	using namespace qlt_dlo_detail;

	// Nodes: variables (keyed by their bf node, as get_variables returns
	// them) and distinct finite constants.
	std::vector<tref> node_var;                        // nullptr: constant
	std::vector<std::optional<qlt_rational>> node_val; // nullopt: variable
	std::vector<qlt_rational> csts;
	auto var_node = [&](tref v) {
		for (size_t j = 0; j < node_var.size(); ++j)
			if (node_var[j] && (node_var[j] == v
				|| tau::get(node_var[j]) == tau::get(v))) return j;
		node_var.push_back(v), node_val.push_back(std::nullopt);
		return node_var.size() - 1;
	};
	auto cst_node = [&](const qlt_rational& c) {
		for (size_t j = 0; j < node_val.size(); ++j)
			if (node_val[j] && *node_val[j] == c) return j;
		csts.push_back(c);
		node_var.push_back(nullptr), node_val.push_back(c);
		return node_val.size() - 1;
	};
	// Typed 0 / 1 act as the -inf / +inf ends of the order (as in
	// qlt_dlo_qe_interval); no model value is ever one of them.
	constexpr size_t NEG_INF = SIZE_MAX, POS_INF = SIZE_MAX - 1;
	// Finite endpoints of the constants of skipped disequalities: model
	// values avoid them (see below).
	std::vector<qlt_rational> avoid;
	auto collect_endpoints = [&](tref n) {
		for (tref k : tau::get(n).select_all(is<node, tau::ba_constant>)) {
			auto cv = tau::get(k).get_ba_constant();
			if (!std::holds_alternative<qlt>(cv)) continue;
			for (const auto& p : std::get<qlt>(cv).pieces)
				for (const auto& e : { p.lo.val, p.hi.val })
					if (e.is_finite()) avoid.push_back(e);
		}
	};
	// A side is a variable or a finite singleton qlt constant.
	auto side = [&](tref s) -> std::optional<size_t> {
		const auto& t = tau::get(s);
		if (!t.is(tau::bf) || !t.has_child()) return std::nullopt;
		const auto& c = t[0];
		if (c.is(tau::variable)) return var_node(s);
		if (c.is(tau::bf_f)) return NEG_INF;
		if (c.is(tau::bf_t)) return POS_INF;
		if (!c.is_ba_constant()) return std::nullopt;
		auto cv = c.get_ba_constant();
		if (!std::holds_alternative<qlt>(cv)) return std::nullopt;
		const qlt& q = std::get<qlt>(cv);
		if (q.pieces.size() != 1) return std::nullopt;
		const auto& p = q.pieces[0];
		if (p.lo.val != p.hi.val || !p.lo.val.is_finite()
			|| p.lo.bound != qlt_bound::CLOSED
			|| p.hi.bound != qlt_bound::CLOSED) return std::nullopt;
		return cst_node(p.lo.val);
	};
	struct atom { size_t a, b; size_t op; }; // a op b, op in lt/lteq/eq/neq
	std::vector<atom> atoms;
	for (tref n : sys) {
		const auto& t = tau::get(n);
		if (!t.is(tau::wff) || !t.has_child()) return {};
		size_t op = t[0].value.nt;
		tref lhs, rhs;
		bool neg = false;
		if (op == tau::wff_neg) {
			const auto& ti = tau::get(t[0].first());
			if (!ti.is(tau::wff) || !ti.has_child()) return {};
			op = ti[0].value.nt, lhs = ti[0].first(), rhs = ti[0].second();
			neg = true;
		} else lhs = t[0].first(), rhs = t[0].second();
		if      (op == tau::bf_nlt)   op = tau::bf_lt,   neg = !neg;
		else if (op == tau::bf_ngt)   op = tau::bf_gt,   neg = !neg;
		else if (op == tau::bf_nlteq) op = tau::bf_lteq, neg = !neg;
		else if (op == tau::bf_ngteq) op = tau::bf_gteq, neg = !neg;
		if (op != tau::bf_lt && op != tau::bf_gt && op != tau::bf_lteq
			&& op != tau::bf_gteq && op != tau::bf_eq
			&& op != tau::bf_neq) return {};
		auto a = side(lhs), b = side(rhs);
		const bool is_neq = neg ? op == tau::bf_eq : op == tau::bf_neq;
		// A disequality normalization rendered in XOR form (`x != {c}`
		// becomes `x{c}' | x'{c} != 0`) is not read here. Skipping it is
		// safe: distinct classes get distinct values off every constant,
		// so it can fail only if its sides are forced equal, and the
		// caller verifies the model against every atom anyway.
		if ((!a || !b) && is_neq) { collect_endpoints(n); continue; }
		if (!a || !b) return {};
		// orient > and >= as < and <=
		if (op == tau::bf_gt)   op = tau::bf_lt,   std::swap(a, b);
		if (op == tau::bf_gteq) op = tau::bf_lteq, std::swap(a, b);
		// negate over a total order: !(a<b) = b<=a, !(a<=b) = b<a
		if (neg) {
			if      (op == tau::bf_lt)   op = tau::bf_lteq, std::swap(a, b);
			else if (op == tau::bf_lteq) op = tau::bf_lt,   std::swap(a, b);
			else if (op == tau::bf_eq)   op = tau::bf_neq;
			else                         op = tau::bf_eq;
		}
		if (*a >= POS_INF || *b >= POS_INF) {
			// Against an end of the order: decided for every point.
			// A violated one declines (the value may be the typed
			// 0 / 1 itself, which the fallback path can find).
			bool holds;
			if (op == tau::bf_lt) holds = *a != *b
				&& (*a == NEG_INF || *b == POS_INF);
			else if (op == tau::bf_lteq)
				holds = *a == NEG_INF || *b == POS_INF;
			else holds = (op == tau::bf_eq) == (*a == *b);
			if (!holds) return {};
			continue;
		}
		atoms.push_back({ *a, *b, op });
	}
	const size_t n = node_val.size();

	// Transitive closure of the strongest relation between nodes.
	std::vector<std::vector<rel>> r(n, std::vector<rel>(n, rel::none));
	auto add = [&](size_t a, size_t b, rel k) {
		if (r[a][b] < k) r[a][b] = k;
	};
	for (const auto& at : atoms) {
		if (at.op == tau::bf_lt) add(at.a, at.b, rel::lt);
		else if (at.op == tau::bf_lteq) add(at.a, at.b, rel::le);
		else if (at.op == tau::bf_eq)
			add(at.a, at.b, rel::le), add(at.b, at.a, rel::le);
	}
	for (size_t i = 0; i < n; ++i) for (size_t j = 0; j < n; ++j)
		if (node_val[i] && node_val[j] && *node_val[i] < *node_val[j])
			add(i, j, rel::lt);
	for (size_t k = 0; k < n; ++k)
		for (size_t i = 0; i < n; ++i) if (r[i][k] != rel::none)
			for (size_t j = 0; j < n; ++j) if (r[k][j] != rel::none)
				add(i, j, std::max(r[i][k], r[k][j]));
	for (size_t i = 0; i < n; ++i) if (r[i][i] == rel::lt) return {};
	// Classes: nodes reaching each other (necessarily non-strictly).
	auto same = [&](size_t i, size_t j) {
		return i == j || (r[i][j] != rel::none && r[j][i] != rel::none);
	};
	for (const auto& at : atoms)
		if (at.op == tau::bf_neq && same(at.a, at.b)) return {};
	std::vector<size_t> cls(n);
	for (size_t i = 0; i < n; ++i) {
		cls[i] = i;
		for (size_t j = 0; j < i; ++j) if (same(i, j)) { cls[i] = cls[j]; break; }
	}
	// Class value when it contains a constant (at most one: two distinct
	// constants are strictly ordered, so merging them was a strict cycle).
	std::vector<std::optional<qlt_rational>> val(n);
	for (size_t i = 0; i < n; ++i)
		if (node_val[i]) val[cls[i]] = node_val[i];
	// Topological order of the representatives: by the number of classes
	// reaching them (a DAG's closure orders ancestors first).
	std::vector<size_t> reps;
	for (size_t i = 0; i < n; ++i) if (cls[i] == i) reps.push_back(i);
	std::vector<size_t> ancestors(n, 0);
	for (size_t v : reps) for (size_t u : reps)
		if (u != v && r[u][v] != rel::none) ++ancestors[v];
	std::stable_sort(reps.begin(), reps.end(), [&](size_t a, size_t b) {
		return ancestors[a] < ancestors[b]; });
	std::vector<qlt_rational> used(csts.begin(), csts.end());
	used.insert(used.end(), avoid.begin(), avoid.end());
	std::vector<bool> done(n, false);
	for (size_t v : reps) {
		std::optional<qlt_rational> lo, hi;
		for (size_t u : reps) if (done[u] && r[u][v] != rel::none)
			if (!lo || *lo < *val[u]) lo = val[u];
		if (!val[v]) {
			for (size_t c : reps)
				if (c != v && val[c] && r[v][c] != rel::none)
					if (!hi || *val[c] < *hi) hi = val[c];
			// hi is a constant, hence reached by `lo`'s class too:
			// the closure makes lo < hi (defensive check).
			if (lo && hi && !(*lo < *hi)) return {};
			val[v] = pick_between(lo, hi, used);
			used.push_back(*val[v]);
		} else if (lo && !(*lo < *val[v])) return {};
		done[v] = true;
	}
	solution<node> result;
	for (size_t i = 0; i < n; ++i) {
		if (!node_var[i]) continue;
		qlt_piece p;
		p.lo = qlt_endpoint{*val[cls[i]], qlt_bound::CLOSED};
		p.hi = qlt_endpoint{*val[cls[i]], qlt_bound::CLOSED};
		qlt singleton{{p}};
		typename tau::constant c = qlt{singleton};
		tref witness_raw = ba_constants<node>::get(c, options.type_id);
		result[node_var[i]] = tau::get(tau::bf, witness_raw);
	}
	return result;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_SOLVER_TMPL_H__
