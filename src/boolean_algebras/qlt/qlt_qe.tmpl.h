// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file qlt_qe.tmpl.h
 * @brief Quantifier elimination for qlt, whose variables denote sets of
 * rationals: finite unions of intervals.
 *
 * Included from qlt_descriptor.tmpl.h and nowhere else, so it instantiates only
 * when qlt is in the pack.
 *
 * A closed formula is decided exactly by cells. Its constants partition Q into
 * finitely many regions, the nonempty minterms of the constants. Under values
 * of the variables bound so far, each region splits into cells, one per choice
 * of membership in each of those variables, and every term is a union of
 * cells. An equation asks only which cells are empty, so a formula's truth
 * depends only on the number of points of each cell: infinitely many when the
 * cell holds a nondegenerate interval, the count of its points otherwise. A
 * quantifier over x splits each cell of n points into two cells of a and n - a
 * points, any 0 <= a <= n being realised by a finite union of intervals (an
 * infinite cell splits into an infinite part and a part of any size). With d
 * quantifiers still to come, counts of 2^d and above behave alike, so each
 * count is kept saturated at 2^d and the search is finite.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_QE_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_QE_TMPL_H__

#include <map>

#include "boolean_algebras/qlt/qlt.h"
#include "tau_tree.h"

namespace idni::tau_lang {

namespace qlt_cells_detail {

// Variables a decision takes: one bit each in a 32-bit cell mask.
inline constexpr size_t max_vars = 5;
// Regions a decision takes.
inline constexpr size_t max_regions = 64;
// Splits a decision tries before it declines.
inline constexpr size_t max_splits = size_t{1} << 22;

// For each region, the cells a term covers: bit b stands for the cell whose
// membership in variable j is bit j of b.
using masks = std::vector<uint32_t>;

// A formula compiled against the regions. `zero` holds when no nonempty cell
// lies in `cells`; `ex`/`all` bind variable `var` over kids[0]; `depth` is the
// quantifier nesting depth of the subformula and `free` the variables its truth
// depends on, one bit each.
struct formula {
	enum kind_t : uint8_t { truth, zero, conj, disj, neg, ex, all } kind;
	bool value = false;
	size_t var = 0;
	size_t depth = 0;
	masks cells;
	std::vector<size_t> kids;
	uint32_t free = 0;
};

// A nonempty cell: its region, its membership bits and its saturated count.
struct cell {
	uint32_t region;
	uint32_t bits;
	size_t count;
};

// Number of points of a nonempty set, SIZE_MAX for infinitely many.
inline size_t points_of(const qlt& r) {
	for (const auto& p : r.pieces)
		if (qlt_sem_cmp(p.lo.val, p.hi.val) == std::partial_ordering::less)
			return SIZE_MAX;
	return r.pieces.size();
}

// No over-approximation and no named endpoint, so set operations are exact.
inline bool exact(const qlt& q) {
	if (q.inexact) return false;
	for (const auto& p : q.pieces)
		if (p.lo.val.is_sym() || p.hi.val.is_sym()) return false;
	return true;
}

} // namespace qlt_cells_detail

// The truth of the closed formula fm, nullopt when it has a free variable, a
// shape outside equations and order comparisons between Boolean combinations
// of qlt constants and variables, or exceeds the bounds above.
template<NodeType node>
static std::optional<bool> qlt_decide_closed(tref fm) {
	using tau = tree<node>;
	using namespace qlt_cells_detail;
	if (!get_free_vars<node>(fm).empty()) return std::nullopt;
	// The regions: the nonempty minterms of the constants.
	std::vector<qlt> regions{ qlt::top() };
	for (tref c : tau::get(fm).select_all([](tref n) {
		return tau::get(n).is_ba_constant(); }))
	{
		auto v = tau::get(c).get_ba_constant();
		if (!std::holds_alternative<qlt>(v)) return std::nullopt;
		const qlt& q = std::get<qlt>(v);
		if (!exact(q)) return std::nullopt;
		std::vector<qlt> next;
		for (const qlt& r : regions)
			for (qlt part : { r & q, r & ~q }) {
				if (!exact(part)) return std::nullopt;
				if (!part.is_empty()) next.push_back(std::move(part));
			}
		if (next.size() > max_regions) return std::nullopt;
		regions = std::move(next);
	}
	const size_t nr = regions.size();
	trefs vars;
	for (tref q : tau::get(fm).select_all([](tref n) {
		const auto& t = tau::get(n);
		return t.is(tau::wff_ex) || t.is(tau::wff_all); }))
	{
		tref v = tau::get(q).first();
		if (std::none_of(vars.begin(), vars.end(), [&](tref w) {
			return tau::get(w) == tau::get(v); })) vars.push_back(v);
	}
	if (vars.size() > max_vars) return std::nullopt;
	const size_t k = vars.size();
	const uint32_t all_cells = k == max_vars ? UINT32_MAX
		: (uint32_t{1} << (uint32_t{1} << k)) - 1;
	auto var_index = [&](tref v) {
		size_t j = 0;
		while (tau::get(vars[j]) != tau::get(v)) ++j;
		return j;
	};
	std::function<std::optional<masks>(tref)> term
		= [&](tref n) -> std::optional<masks> {
		const auto& t = tau::get(n);
		if (!t.is(tau::bf) || !t.has_child()) return std::nullopt;
		if (t.child_is(tau::bf_t)) return masks(nr, all_cells);
		if (t.child_is(tau::bf_f)) return masks(nr, 0);
		if (t[0].is_ba_constant()) {
			const qlt q = std::get<qlt>(t[0].get_ba_constant());
			masks m(nr, 0);
			for (size_t i = 0; i < nr; ++i)
				if (!(regions[i] & q).is_empty()) m[i] = all_cells;
			return m;
		}
		if (t.child_is(tau::variable)) {
			const size_t j = var_index(t.first());
			uint32_t in = 0;
			for (uint32_t b = 0; b < (uint32_t{1} << k); ++b)
				if (b >> j & 1) in |= uint32_t{1} << b;
			return masks(nr, in);
		}
		if (t.child_is(tau::bf_neg)) {
			auto a = term(t[0].first());
			if (!a) return std::nullopt;
			for (auto& m : *a) m = ~m & all_cells;
			return a;
		}
		const bool is_and = t.child_is(tau::bf_and);
		const bool is_or = t.child_is(tau::bf_or);
		if (!is_and && !is_or && !t.child_is(tau::bf_xor))
			return std::nullopt;
		std::optional<masks> acc;
		for (size_t c = 0; c < t[0].children_size(); ++c) {
			auto b = term(t[0].child(c));
			if (!b) return std::nullopt;
			if (!acc) { acc = std::move(b); continue; }
			for (size_t i = 0; i < nr; ++i)
				(*acc)[i] = is_and ? (*acc)[i] & (*b)[i]
					: is_or ? (*acc)[i] | (*b)[i]
					: (*acc)[i] ^ (*b)[i];
		}
		return acc;
	};
	std::vector<formula> fs;
	auto add = [&](formula f) {
		for (size_t c : f.kids) {
			f.depth = std::max(f.depth, fs[c].depth);
			f.free |= fs[c].free;
		}
		if (f.kind == formula::ex || f.kind == formula::all) {
			++f.depth;
			f.free &= ~(uint32_t{1} << f.var);
		}
		// A cell set depends on x_j when flipping bit j of some cell
		// moves it in or out of the set.
		if (f.kind == formula::zero)
			for (size_t j = 0; j < k; ++j)
				for (uint32_t m : f.cells)
					for (uint32_t b = 0; b < (uint32_t{1} << k); ++b)
						if ((m >> b & 1) != (m >> (b ^ (uint32_t{1} << j)) & 1))
							f.free |= uint32_t{1} << j;
		fs.push_back(std::move(f));
		return fs.size() - 1;
	};
	auto zero_of = [&](const masks& a, const masks& b, bool sym) {
		masks d(nr);
		for (size_t i = 0; i < nr; ++i)
			d[i] = sym ? a[i] ^ b[i] : a[i] & ~b[i];
		return add({ formula::zero, false, 0, 0, std::move(d), {} });
	};
	auto negate = [&](size_t f) {
		return add({ formula::neg, false, 0, 0, {}, { f } });
	};
	auto join = [&](formula::kind_t kind, std::vector<size_t> kids) {
		return add({ kind, false, 0, 0, {}, std::move(kids) });
	};
	// a < b in the Boolean order: a & b' = 0 and a != b.
	auto strict = [&](const masks& a, const masks& b) {
		return join(formula::conj, { zero_of(a, b, false),
			negate(zero_of(a, b, true)) });
	};
	std::function<std::optional<size_t>(tref)> compile
		= [&](tref n) -> std::optional<size_t> {
		const auto& t = tau::get(n);
		if (t.equals_T()) return add({ formula::truth, true, 0, 0, {}, {} });
		if (t.equals_F()) return add({ formula::truth, false, 0, 0, {}, {} });
		if (!t.is(tau::wff) || !t.has_child()) return std::nullopt;
		const auto op = t[0].value.nt;
		if (op == tau::wff_ex || op == tau::wff_all) {
			auto body = compile(t[0].second());
			if (!body) return std::nullopt;
			return add({ op == tau::wff_ex ? formula::ex : formula::all,
				false, var_index(t[0].first()), 0, {}, { *body } });
		}
		if (op == tau::wff_neg || op == tau::wff_and || op == tau::wff_or
			|| op == tau::wff_imply || op == tau::wff_equiv
			|| op == tau::wff_xor)
		{
			std::vector<size_t> kids;
			for (size_t c = 0; c < t[0].children_size(); ++c) {
				auto a = compile(t[0].child(c));
				if (!a) return std::nullopt;
				kids.push_back(*a);
			}
			if (op == tau::wff_neg) return negate(kids[0]);
			if (op == tau::wff_and) return join(formula::conj, kids);
			if (op == tau::wff_or) return join(formula::disj, kids);
			if (kids.size() != 2) return std::nullopt;
			const size_t a = kids[0], b = kids[1];
			if (op == tau::wff_imply)
				return join(formula::disj, { negate(a), b });
			const size_t same = join(formula::disj, {
				join(formula::conj, { a, b }),
				join(formula::conj, { negate(a), negate(b) }) });
			return op == tau::wff_equiv ? same : negate(same);
		}
		if (op != tau::bf_eq && op != tau::bf_neq
			&& op != tau::bf_lt && op != tau::bf_nlt
			&& op != tau::bf_lteq && op != tau::bf_nlteq
			&& op != tau::bf_gt && op != tau::bf_ngt
			&& op != tau::bf_gteq && op != tau::bf_ngteq)
				return std::nullopt;
		auto a = term(t[0].first()), b = term(t[0].second());
		if (!a || !b) return std::nullopt;
		switch (op) {
		case tau::bf_eq:    return zero_of(*a, *b, true);
		case tau::bf_neq:   return negate(zero_of(*a, *b, true));
		case tau::bf_lteq:  return zero_of(*a, *b, false);
		case tau::bf_nlteq: return negate(zero_of(*a, *b, false));
		case tau::bf_gteq:  return zero_of(*b, *a, false);
		case tau::bf_ngteq: return negate(zero_of(*b, *a, false));
		case tau::bf_lt:    return strict(*a, *b);
		case tau::bf_nlt:   return negate(strict(*a, *b));
		case tau::bf_gt:    return strict(*b, *a);
		default:            return negate(strict(*b, *a));
		}
	};
	auto root = compile(fm);
	if (!root) return std::nullopt;
	size_t splits = 0;
	bool exhausted = false;
	std::map<std::vector<size_t>, bool> memo;
	std::function<bool(size_t, const std::vector<cell>&)> holds
		= [&](size_t f, const std::vector<cell>& in) -> bool {
		if (exhausted) return false;
		const formula& x = fs[f];
		switch (x.kind) {
		case formula::truth: return x.value;
		case formula::zero:
			for (const cell& c : in)
				if (x.cells[c.region] >> c.bits & 1) return false;
			return true;
		case formula::neg: return !holds(x.kids[0], in);
		case formula::conj:
			for (size_t c : x.kids) if (!holds(c, in)) return false;
			return true;
		case formula::disj:
			for (size_t c : x.kids) if (holds(c, in)) return true;
			return false;
		default: break;
		}
		// Cells the formula cannot tell apart merge: same region, same
		// membership in the variables it depends on.
		const size_t own_cap = size_t{1} << x.depth;
		std::vector<cell> cells;
		for (const cell& c : in) {
			const uint32_t bits = c.bits & x.free;
			auto it = std::find_if(cells.begin(), cells.end(),
				[&](const cell& d) {
					return d.region == c.region && d.bits == bits; });
			if (it == cells.end()) cells.push_back({ c.region, bits,
				std::min(c.count, own_cap) });
			else it->count = std::min(it->count + c.count, own_cap);
		}
		std::sort(cells.begin(), cells.end(), [](const cell& a, const cell& b) {
			return std::tie(a.region, a.bits, a.count)
				< std::tie(b.region, b.bits, b.count); });
		std::vector<size_t> key{ f };
		for (const cell& c : cells) {
			key.push_back(c.region);
			key.push_back(c.bits);
			key.push_back(c.count);
		}
		if (auto it = memo.find(key); it != memo.end()) return it->second;
		auto remember = [&](bool v) {
			if (!exhausted) memo.emplace(std::move(key), v);
			return v;
		};
		// Every way of splitting each cell into its parts in and out of
		// the bound variable, counted at the body's saturation.
		const bool universal = x.kind == formula::all;
		const size_t body = x.kids[0];
		const size_t cap = size_t{1} << fs[body].depth;
		const uint32_t bit = uint32_t{1} << x.var;
		std::vector<std::vector<std::pair<size_t, size_t>>> ways;
		for (const cell& c : cells) {
			std::vector<std::pair<size_t, size_t>> w;
			for (size_t a = 0; a <= c.count; ++a) {
				std::pair<size_t, size_t> p{ std::min(a, cap),
					std::min(c.count - a, cap) };
				if (std::find(w.begin(), w.end(), p) == w.end())
					w.push_back(p);
			}
			ways.push_back(std::move(w));
		}
		std::vector<size_t> pick(cells.size(), 0);
		std::vector<cell> next;
		for (;;) {
			if (++splits > max_splits) { exhausted = true; return false; }
			next.clear();
			for (size_t i = 0; i < cells.size(); ++i) {
				const auto [in, out] = ways[i][pick[i]];
				if (in) next.push_back({ cells[i].region,
					cells[i].bits | bit, in });
				if (out) next.push_back({ cells[i].region,
					cells[i].bits & ~bit, out });
			}
			if (holds(body, next) != universal)
				return remember(!universal);
			if (exhausted) return false;
			size_t i = 0;
			while (i < cells.size() && ++pick[i] == ways[i].size())
				pick[i++] = 0;
			if (i == cells.size()) return remember(universal);
		}
	};
	const size_t cap = size_t{1} << fs[*root].depth;
	std::vector<cell> cells;
	for (size_t i = 0; i < nr; ++i)
		cells.push_back({ (uint32_t) i, 0,
			std::min(points_of(regions[i]), cap) });
	const bool value = holds(*root, cells);
	if (exhausted) return std::nullopt;
	return value;
}

// True when body is a conjunction of disequations `var != t` with t free of
// var. Such a body excludes finitely many of the infinitely many values of var,
// so `ex var body` holds.
template<NodeType node>
static bool qlt_only_excludes(tref var, tref body) {
	using tau = tree<node>;
	auto is_bare_var = [&](tref side) {
		const auto& t = tau::get(side);
		return t.child_is(tau::variable)
			&& tau::get(t.first()) == tau::get(var);
	};
	bool any = false;
	std::function<bool(tref)> check = [&](tref n) {
		const auto& t = tau::get(n);
		if (t.equals_T()) return true;
		if (!t.is(tau::wff) || !t.has_child()) return false;
		if (t[0].is(tau::wff_and)) {
			for (size_t c = 0; c < t[0].children_size(); ++c)
				if (!check(t[0].child(c))) return false;
			return true;
		}
		const tree<node>* at = &t[0];
		if (t[0].is(tau::wff_neg)) {
			const auto& ti = tau::get(t[0].first());
			if (!ti.is(tau::wff) || !ti.has_child()
				|| !ti[0].is(tau::bf_eq)) return false;
			at = &ti[0];
		} else if (!t[0].is(tau::bf_neq)) return false;
		tref lhs = at->first(), rhs = at->second();
		const bool in_l = contains<node>(lhs, var);
		const bool in_r = contains<node>(rhs, var);
		if (in_l == in_r || !is_bare_var(in_l ? lhs : rhs)) return false;
		return any = true;
	};
	return check(body) && any;
}

// The omcat_qe capability: the truth of `ex var body`, or of body itself when
// body is var's own `ex`/`all` node; nullopt when undecided.
template<NodeType node>
static std::optional<bool> qlt_omcat_qe(tref var, tref body) {
	using tau = tree<node>;
	const auto& t = tau::get(body);
	const bool own = t.has_child()
		&& (t[0].is(tau::wff_ex) || t[0].is(tau::wff_all))
		&& tau::get(t[0].first()) == tau::get(var);
	tref fm = own ? body : tau::build_wff_ex(var, body, false);
	if (auto r = qlt_decide_closed<node>(fm)) return r;
	tref inner = own ? t[0].second() : body;
	if (own && t[0].is(tau::wff_all)) return std::nullopt;
	if (qlt_only_excludes<node>(var, inner)) return true;
	// The conjuncts free of other variables are necessary: when no value
	// satisfies them, none satisfies the body.
	trefs closed;
	std::function<void(tref)> split = [&](tref n) {
		const auto& c = tau::get(n);
		if (c.has_child() && c[0].is(tau::wff_and)) {
			for (size_t i = 0; i < c[0].children_size(); ++i)
				split(c[0].child(i));
			return;
		}
		for (tref fv : get_free_vars<node>(n))
			if (tau::get(fv) != tau::get(var)) return;
		closed.push_back(n);
	};
	split(inner);
	if (!closed.empty() && qlt_decide_closed<node>(tau::build_wff_ex(
		var, tau::build_wff_and(closed), false)) == false) return false;
	return std::nullopt;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_QE_TMPL_H__
