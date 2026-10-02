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
#include "bounded_call.h"
#include "tau_tree.h"

namespace idni::tau_lang {

namespace qlt_cells_detail {

// Variables a decision takes: one bit each in a cell's membership bits.
inline constexpr size_t max_vars = 16;
// Regions a decision takes: one bit each in a constant's region mask.
inline constexpr size_t max_regions = 64;
// Quantifier nesting a decision takes: counts are kept up to 2^depth.
inline constexpr size_t max_depth = 20;
// Splits a decision tries before it declines.
inline constexpr size_t max_splits = size_t{1} << 20;

// A term compiled against the regions: a constant holds the regions it covers,
// a variable its index; `free` holds the variables it reads, one bit each.
struct cell_term {
	enum kind_t : uint8_t { top, bottom, constant, var, neg, conj, disj, sum }
		kind;
	uint64_t regions = 0;
	size_t index = 0;
	std::vector<size_t> kids = {};
	uint32_t free = 0;
};

// A formula compiled against the regions. `zero` holds when no nonempty cell
// lies in a & b' (a ^ b when `sym`); `ex`/`all` bind variable `var` over
// kids[0]; `depth` is the quantifier nesting depth of the subformula and `free`
// the variables its truth depends on, one bit each.
struct formula {
	enum kind_t : uint8_t { truth, zero, conj, disj, neg, ex, all } kind;
	bool value = false;
	size_t var = 0;
	size_t depth = 0;
	size_t a = 0, b = 0;
	bool sym = false;
	std::vector<size_t> kids = {};
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
	auto var_index = [&](tref v) {
		size_t j = 0;
		while (tau::get(vars[j]) != tau::get(v)) ++j;
		return j;
	};
	std::vector<cell_term> ts;
	auto add_term = [&](cell_term t) {
		for (size_t c : t.kids) t.free |= ts[c].free;
		if (t.kind == cell_term::var) t.free |= uint32_t{1} << t.index;
		ts.push_back(std::move(t));
		return ts.size() - 1;
	};
	std::function<std::optional<size_t>(tref)> compile_term
		= [&](tref n) -> std::optional<size_t> {
		const auto& t = tau::get(n);
		if (!t.is(tau::bf) || !t.has_child()) return std::nullopt;
		if (t.child_is(tau::bf_t)) return add_term({ cell_term::top });
		if (t.child_is(tau::bf_f)) return add_term({ cell_term::bottom });
		if (t[0].is_ba_constant()) {
			const qlt q = std::get<qlt>(t[0].get_ba_constant());
			uint64_t in = 0;
			for (size_t i = 0; i < nr; ++i)
				if (!(regions[i] & q).is_empty())
					in |= uint64_t{1} << i;
			return add_term({ cell_term::constant, in });
		}
		if (t.child_is(tau::variable))
			return add_term({ cell_term::var, 0, var_index(t.first()) });
		cell_term::kind_t kind;
		if (t.child_is(tau::bf_neg)) kind = cell_term::neg;
		else if (t.child_is(tau::bf_and)) kind = cell_term::conj;
		else if (t.child_is(tau::bf_or)) kind = cell_term::disj;
		else if (t.child_is(tau::bf_xor)) kind = cell_term::sum;
		else return std::nullopt;
		cell_term c{ kind };
		for (size_t i = 0; i < t[0].children_size(); ++i) {
			auto a = compile_term(t[0].child(i));
			if (!a) return std::nullopt;
			c.kids.push_back(*a);
		}
		return add_term(std::move(c));
	};
	// Whether the cell of `region` with membership `bits` lies in term x.
	std::function<bool(size_t, uint32_t, uint32_t)> in_term
		= [&](size_t x, uint32_t region, uint32_t bits) -> bool {
		const cell_term& t = ts[x];
		switch (t.kind) {
		case cell_term::top: return true;
		case cell_term::bottom: return false;
		case cell_term::constant: return t.regions >> region & 1;
		case cell_term::var: return bits >> t.index & 1;
		case cell_term::neg: return !in_term(t.kids[0], region, bits);
		case cell_term::conj:
			for (size_t c : t.kids)
				if (!in_term(c, region, bits)) return false;
			return true;
		case cell_term::disj:
			for (size_t c : t.kids)
				if (in_term(c, region, bits)) return true;
			return false;
		default: {
			bool v = false;
			for (size_t c : t.kids) v ^= in_term(c, region, bits);
			return v;
		}
		}
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
		if (f.kind == formula::zero) f.free |= ts[f.a].free | ts[f.b].free;
		fs.push_back(std::move(f));
		return fs.size() - 1;
	};
	auto zero_of = [&](size_t a, size_t b, bool sym) {
		return add({ formula::zero, false, 0, 0, a, b, sym, {} });
	};
	auto negate = [&](size_t f) {
		return add({ formula::neg, false, 0, 0, 0, 0, false, { f } });
	};
	auto join = [&](formula::kind_t kind, std::vector<size_t> kids) {
		return add({ kind, false, 0, 0, 0, 0, false, std::move(kids) });
	};
	// a < b in the Boolean order: a & b' = 0 and a != b.
	auto strict = [&](size_t a, size_t b) {
		return join(formula::conj, { zero_of(a, b, false),
			negate(zero_of(a, b, true)) });
	};
	auto group = [&](formula::kind_t kind, std::vector<size_t> kids) {
		return kids.size() == 1 ? kids[0] : join(kind, std::move(kids));
	};
	// The quantifier over variable j of body, scoped as narrowly as it
	// goes: across the parts of its body free of j, and through the
	// connective it distributes over, so each search splits only what it
	// reads.
	std::function<size_t(bool, size_t, size_t)> quant
		= [&](bool exists, size_t j, size_t body) -> size_t {
		const uint32_t bit = uint32_t{1} << j;
		if (!(fs[body].free & bit)) return body;
		const auto kind = fs[body].kind;
		const auto spread = exists ? formula::disj : formula::conj;
		const auto keep = exists ? formula::conj : formula::disj;
		if (kind == spread) {
			std::vector<size_t> kids;
			for (size_t c : std::vector<size_t>(fs[body].kids))
				kids.push_back(quant(exists, j, c));
			return group(spread, std::move(kids));
		}
		if (kind == keep) {
			std::vector<size_t> with, without;
			for (size_t c : fs[body].kids)
				(fs[c].free & bit ? with : without).push_back(c);
			if (!without.empty()) {
				without.push_back(quant(exists, j,
					group(keep, std::move(with))));
				return group(keep, std::move(without));
			}
		}
		return add({ exists ? formula::ex : formula::all, false, j, 0,
			0, 0, false, { body } });
	};
	std::function<std::optional<size_t>(tref)> compile
		= [&](tref n) -> std::optional<size_t> {
		const auto& t = tau::get(n);
		if (t.equals_T()) return add({ formula::truth, true });
		if (t.equals_F()) return add({ formula::truth, false });
		if (!t.is(tau::wff) || !t.has_child()) return std::nullopt;
		const auto op = t[0].value.nt;
		if (op == tau::wff_ex || op == tau::wff_all) {
			auto body = compile(t[0].second());
			if (!body) return std::nullopt;
			return quant(op == tau::wff_ex, var_index(t[0].first()),
				*body);
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
		auto a = compile_term(t[0].first()), b = compile_term(t[0].second());
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
	// counts are kept up to 2^depth
	if (!root || fs[*root].depth > max_depth) return std::nullopt;
	size_t splits = 0;
	bool exhausted = false;
	// A caller sharing a deadline among its questions bounds this one too.
	std::optional<std::chrono::steady_clock::time_point> deadline;
	if (shared_deadline()) deadline = budget_deadline(
		std::chrono::steady_clock::duration::max() / 2);
	auto out_of_budget = [&] {
		if (++splits > max_splits) return true;
		if (deadline && (splits & 1023) == 0
			&& std::chrono::steady_clock::now() > *deadline)
		{
			note_time_budget_exhausted(
				"a closed qlt decision ran past its time budget");
			return true;
		}
		return false;
	};
	std::map<std::vector<size_t>, bool> memo;
	std::function<bool(size_t, const std::vector<cell>&)> holds
		= [&](size_t f, const std::vector<cell>& in) -> bool {
		if (exhausted) return false;
		const formula& x = fs[f];
		switch (x.kind) {
		case formula::truth: return x.value;
		case formula::zero:
			for (const cell& c : in) {
				const bool ia = in_term(x.a, c.region, c.bits);
				const bool ib = in_term(x.b, c.region, c.bits);
				if (x.sym ? ia != ib : ia && !ib) return false;
			}
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
		const bool universal = x.kind == formula::all;
		// A block of quantifiers of one kind over a quantifier-free body
		// only asks which of the 2^m parts of each cell are nonempty: at
		// least one, at most as many as the cell has points.
		std::vector<size_t> block{ x.var };
		size_t inner = x.kids[0];
		while (fs[inner].kind == x.kind && block.size() < 4) {
			block.push_back(fs[inner].var);
			inner = fs[inner].kids[0];
		}
		if (fs[inner].depth == 0) {
			const uint32_t parts = uint32_t{1} << block.size();
			auto spread = [&](uint32_t part, uint32_t bits) {
				for (size_t b = 0; b < block.size(); ++b) {
					const uint32_t vb = uint32_t{1} << block[b];
					bits = part >> b & 1 ? bits | vb : bits & ~vb;
				}
				return bits;
			};
			// An existential block over a conjunction of (negated)
			// emptiness atoms: each cell takes parts no positive atom
			// forbids, and more of them only helps a negated one, so as
			// many as it has points for.
			std::vector<size_t> lits;
			std::function<void(size_t)> flatten = [&](size_t f) {
				if (fs[f].kind != formula::conj) lits.push_back(f);
				else for (size_t c : fs[f].kids) flatten(c);
			};
			if (!universal) flatten(inner);
			auto is_zero = [&](size_t f) {
				return fs[f].kind == formula::zero;
			};
			auto is_literal = [&](size_t f) {
				return is_zero(f) || (fs[f].kind == formula::neg
					&& is_zero(fs[f].kids[0]));
			};
			auto in_diff = [&](const formula& z, uint32_t region,
				uint32_t bits)
			{
				const bool ia = in_term(z.a, region, bits);
				const bool ib = in_term(z.b, region, bits);
				return z.sym ? ia != ib : ia && !ib;
			};
			if (!lits.empty() && std::all_of(lits.begin(), lits.end(),
				is_literal))
			{
				std::vector<size_t> negs;
				for (size_t l : lits)
					if (!is_zero(l)) negs.push_back(fs[l].kids[0]);
				// per cell, the allowed parts and, per part, the
				// negated atoms it meets
				std::vector<std::vector<uint32_t>> choices;
				for (const cell& c : cells) {
					std::vector<uint32_t> ok;
					std::vector<uint32_t> meets;
					for (uint32_t part = 0; part < parts; ++part) {
						const uint32_t bits = spread(part, c.bits);
						bool free_part = true;
						for (size_t l : lits)
							if (is_zero(l) && in_diff(fs[l],
								c.region, bits))
									free_part = false;
						if (!free_part) continue;
						uint32_t m = 0;
						for (size_t n = 0; n < negs.size(); ++n)
							if (in_diff(fs[negs[n]], c.region, bits))
								m |= uint32_t{1} << n;
						ok.push_back(part);
						meets.push_back(m);
					}
					if (ok.empty()) return remember(false);
					// the negated atoms met by each choice of
					// min(count, |ok|) allowed parts
					const size_t take = std::min(c.count, ok.size());
					std::vector<uint32_t> met;
					for (uint32_t m = 1; m < (uint32_t{1} << ok.size()); ++m) {
						if ((size_t) std::popcount(m) != take) continue;
						uint32_t u = 0;
						for (size_t b = 0; b < ok.size(); ++b)
							if (m >> b & 1) u |= meets[b];
						if (std::find(met.begin(), met.end(), u) == met.end())
							met.push_back(u);
					}
					choices.push_back(std::move(met));
				}
				const uint32_t all_negs = negs.size() >= 32 ? UINT32_MAX
					: (uint32_t{1} << negs.size()) - 1;
				std::vector<size_t> pick(cells.size(), 0);
				for (;;) {
					if (out_of_budget()) { exhausted = true; return false; }
					uint32_t u = 0;
					for (size_t i = 0; i < cells.size(); ++i)
						u |= choices[i][pick[i]];
					if ((u & all_negs) == all_negs) return remember(true);
					size_t i = 0;
					while (i < cells.size() && ++pick[i] == choices[i].size())
						pick[i++] = 0;
					if (i == cells.size()) return remember(false);
				}
			}
			std::vector<std::vector<uint32_t>> subsets;
			for (const cell& c : cells) {
				std::vector<uint32_t> w;
				for (uint32_t m = 1; m < (uint32_t{1} << parts); ++m)
					if ((size_t) std::popcount(m) <= c.count)
						w.push_back(m);
				subsets.push_back(std::move(w));
			}
			std::vector<size_t> pick(cells.size(), 0);
			std::vector<cell> next;
			for (;;) {
				if (out_of_budget()) { exhausted = true; return false; }
				next.clear();
				for (size_t i = 0; i < cells.size(); ++i) {
					const uint32_t m = subsets[i][pick[i]];
					for (uint32_t part = 0; part < parts; ++part)
						if (m >> part & 1) next.push_back({
							cells[i].region,
							spread(part, cells[i].bits), 1 });
				}
				if (holds(inner, next) != universal)
					return remember(!universal);
				if (exhausted) return false;
				size_t i = 0;
				while (i < cells.size() && ++pick[i] == subsets[i].size())
					pick[i++] = 0;
				if (i == cells.size()) return remember(universal);
			}
		}
		// Every way of splitting each cell into its parts in and out of
		// the bound variable, counted at the body's saturation.
		const size_t body = x.kids[0];
		const size_t cap = size_t{1} << fs[body].depth;
		const uint32_t bit = uint32_t{1} << x.var;
		std::vector<std::vector<std::pair<size_t, size_t>>> ways;
		for (const cell& c : cells) {
			// a count of 2 cap or more splits into every pair one of
			// whose parts reaches cap; a smaller one is exact
			std::vector<std::pair<size_t, size_t>> w;
			if (c.count >= 2 * cap) {
				for (size_t a = 0; a <= cap; ++a) w.emplace_back(a, cap);
				for (size_t b = cap; b-- > 0;) w.emplace_back(cap, b);
			} else for (size_t a = 0; a <= c.count; ++a)
				w.emplace_back(std::min(a, cap),
					std::min(c.count - a, cap));
			// A witness of `ex` is likelier among splits that keep
			// both parts large, a counterexample of `all` among those
			// that empty one: those come first.
			auto large = [](const std::pair<size_t, size_t>& p) {
				return std::make_pair(std::min(p.first, p.second),
					p.first + p.second);
			};
			std::stable_sort(w.begin(), w.end(), [&](const auto& x,
				const auto& y)
			{
				return universal ? large(x) < large(y)
					: large(y) < large(x);
			});
			ways.push_back(std::move(w));
		}
		std::vector<size_t> pick(cells.size(), 0);
		std::vector<cell> next;
		for (;;) {
			if (out_of_budget()) { exhausted = true; return false; }
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
