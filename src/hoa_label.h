// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file hoa_label.h
 * @brief The one reader of HOA edge labels, and the folds over what it reads.
 *
 * A label is a Boolean expression over AP indices:
 *
 *     label := disj | (nothing)
 *     disj  := conj ('|' conj)*
 *     conj  := atom ('&' atom)*
 *     atom  := 't' | 'f' | N | '!' atom | '(' disj ')'
 *
 * Blanks (space, tab) may stand between tokens, and an empty label is `t`.
 * Every consumer of a label (the strategy reader, the game reader, the ABA
 * oracle, the product game, minimisation and code generation) reads it here
 * and folds over the result, so a label means the same everywhere and a
 * malformed one is refused everywhere.
 */

#ifndef __IDNI__TAU__HOA_LABEL_H__
#define __IDNI__TAU__HOA_LABEL_H__

#include "ltl_aba_limits.h"

#include <algorithm>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace idni::tau_lang::hoa_label {

/** @brief The kind of a node of a label expression. */
enum class kind : uint8_t { tt, ff, ap, neg, conj, disj };

/** @brief One node of a label expression. */
struct node {
	kind k = kind::tt;
	size_t ap = 0;   // the AP index of an `ap` node
	size_t lhs = 0;  // the operand of `neg`, the left operand of `conj`/`disj`
	size_t rhs = 0;  // the right operand of `conj`/`disj`
};

/** @brief A label expression; operands precede their operator, the root is
 * the last node. */
struct expr {
	std::vector<node> nodes;
	const node& root() const { return nodes.back(); }
};

/** @brief The longest AP index read, in digits: an index that fits an int. */
inline constexpr size_t max_index_digits = 9;

namespace detail {

inline bool blank(char c) { return c == ' ' || c == '\t'; }
inline bool digit(char c) { return c >= '0' && c <= '9'; }

} // namespace detail

/**
 * @brief Read a label.
 *
 * Iterative (operator precedence), so a deeply nested label cannot exhaust
 * the stack.
 * @param text the label, without its brackets
 * @return the expression, or nullopt when @p text is not a label
 */
inline std::optional<expr> read(std::string_view text) {
	using namespace detail;
	expr e;
	std::vector<char> ops;      // pending '!', '&', '|' and '('
	std::vector<size_t> vals;   // nodes not yet an operand of another
	auto apply = [&](char op) {
		node n;
		n.rhs = vals.back();
		if (op == '!') {
			n.k = kind::neg, n.lhs = n.rhs, n.rhs = 0;
			vals.back() = e.nodes.size();
		} else {
			vals.pop_back();
			n.k = op == '&' ? kind::conj : kind::disj;
			n.lhs = vals.back();
			vals.back() = e.nodes.size();
		}
		e.nodes.push_back(n);
	};
	// an operand is complete: the negations in front of it bind first
	auto operand_done = [&] {
		while (!ops.empty() && ops.back() == '!') apply('!'), ops.pop_back();
	};
	auto leaf = [&](node n) {
		vals.push_back(e.nodes.size());
		e.nodes.push_back(n);
		operand_done();
	};
	bool operand = true; // an operand is expected next
	size_t i = 0;
	while (i < text.size()) {
		char c = text[i];
		if (blank(c)) { ++i; continue; }
		if (operand) {
			if (c == 't' || c == 'f') {
				leaf({ c == 't' ? kind::tt : kind::ff, 0, 0, 0 });
				++i, operand = false;
			} else if (digit(c)) {
				size_t b = i, ap = 0;
				while (i < text.size() && digit(text[i]))
					ap = ap * 10 + static_cast<size_t>(text[i++] - '0');
				if (i - b > max_index_digits) return std::nullopt;
				leaf({ kind::ap, ap, 0, 0 });
				operand = false;
			} else if (c == '!' || c == '(') ops.push_back(c), ++i;
			else return std::nullopt;
			continue;
		}
		if (c == '&' || c == '|') {
			// '&' binds tighter than '|'; both group to the left
			while (!ops.empty() && (ops.back() == '&'
				|| (c == '|' && ops.back() == '|')))
					apply(ops.back()), ops.pop_back();
			ops.push_back(c), ++i, operand = true;
		} else if (c == ')') {
			while (!ops.empty() && ops.back() != '(')
				apply(ops.back()), ops.pop_back();
			if (ops.empty()) return std::nullopt;
			ops.pop_back(), ++i;
			operand_done();
		} else return std::nullopt;
	}
	if (e.nodes.empty() && ops.empty()) {
		e.nodes.push_back({ kind::tt, 0, 0, 0 });
		return e;
	}
	if (operand) return std::nullopt;
	while (!ops.empty()) {
		if (ops.back() == '(') return std::nullopt;
		apply(ops.back()), ops.pop_back();
	}
	return e;
}

/** @brief One more than the largest AP index of @p e; 0 without an AP. */
inline size_t ap_bound(const expr& e) {
	size_t b = 0;
	for (const node& n : e.nodes)
		if (n.k == kind::ap) b = std::max(b, n.ap + 1);
	return b;
}

/**
 * @brief Fold @p e bottom up.
 * @param f called once per node, operands first, as `f(node, done)`, where
 * `done[k]` is the value of node `k` (it may be moved from)
 * @return the value of the root
 */
template <typename R, typename F>
R fold(const expr& e, F&& f) {
	std::vector<R> done;
	done.reserve(e.nodes.size());
	for (const node& n : e.nodes) {
		R v = f(n, done);
		done.push_back(std::move(v));
	}
	return std::move(done.back());
}

/**
 * @brief The value of @p e under an assignment.
 * @param value_of the value of an AP index
 */
template <typename V>
bool eval(const expr& e, V&& value_of) {
	return fold<char>(e, [&](const node& n, std::vector<char>& v) -> char {
		switch (n.k) {
		case kind::tt:   return 1;
		case kind::ff:   return 0;
		case kind::ap:   return value_of(n.ap) ? 1 : 0;
		case kind::neg:  return !v[n.lhs];
		case kind::conj: return v[n.lhs] && v[n.rhs];
		case kind::disj: return v[n.lhs] || v[n.rhs];
		}
		return 0;
	}) != 0;
}

// ── DNF (sum of products) ────────────────────────────────────────────────────

/** @brief One literal of a cube: AP index and polarity. */
struct lit { size_t ap; bool pos; };
/** @brief A conjunction of literals; the empty cube is true. A DNF is a
 * vector of cubes, and the empty vector is false. */
using cube = std::vector<lit>;
using dnf = std::vector<cube>;

namespace detail {

/// Sort a cube by AP and drop duplicates; false when an AP occurs with both
/// polarities (the cube is unsatisfiable).
inline bool normalise_cube(cube& c) {
	std::sort(c.begin(), c.end(), [](const lit& a, const lit& b) {
		return a.ap != b.ap ? a.ap < b.ap : (int)a.pos < (int)b.pos;
	});
	cube out;
	for (const auto& l : c) {
		if (!out.empty() && out.back().ap == l.ap) {
			if (out.back().pos != l.pos) return false;  // p & !p
			continue;                                   // duplicate
		}
		out.push_back(l);
	}
	c.swap(out);
	return true;
}

inline std::optional<dnf> dnf_and(const dnf& a, const dnf& b, size_t cap) {
	dnf r;
	for (const auto& ca : a)
		for (const auto& cb : b) {
			if (r.size() >= cap) return std::nullopt;
			cube m = ca;
			m.insert(m.end(), cb.begin(), cb.end());
			if (normalise_cube(m)) r.push_back(std::move(m));
		}
	return r;
}

inline std::optional<dnf> dnf_or(dnf a, const dnf& b, size_t cap) {
	for (const auto& cb : b) {
		if (a.size() >= cap) return std::nullopt;
		a.push_back(cb);
	}
	return a;
}

// ¬(c1 ∨ … ∨ cn) = ∧_i (∨_l ¬l)
inline std::optional<dnf> dnf_not(const dnf& a, size_t cap) {
	dnf r{ cube{} };
	for (const auto& c : a) {
		if (c.empty()) return dnf{};   // ¬true
		dnf d;
		for (const auto& l : c) d.push_back(cube{ lit{ l.ap, !l.pos } });
		auto m = dnf_and(r, d, cap);
		if (!m) return std::nullopt;
		r = std::move(*m);
	}
	return r;
}

} // namespace detail

/**
 * @brief The DNF cubes of @p e.
 * @param max_cubes the cap on the cubes of every intermediate DNF
 * (0 = unlimited)
 * @return the cubes, or nullopt past the cap. A caller must then refuse the
 * edge: a partial reading widens the label.
 */
inline std::optional<dnf> to_dnf(const expr& e,
	size_t max_cubes = ltl_guard_max_cubes())
{
	using namespace detail;
	const size_t cap = max_cubes ? max_cubes : SIZE_MAX;
	using part = std::optional<dnf>;
	return fold<part>(e, [&](const node& n, std::vector<part>& v) -> part {
		auto both = [&]() { return v[n.lhs] && v[n.rhs]; };
		switch (n.k) {
		case kind::tt:   return dnf{ cube{} };
		case kind::ff:   return dnf{};
		case kind::ap:   return dnf{ cube{ lit{ n.ap, true } } };
		case kind::neg:
			return v[n.lhs] ? dnf_not(*v[n.lhs], cap) : std::nullopt;
		case kind::conj:
			return both() ? dnf_and(*v[n.lhs], *v[n.rhs], cap)
				: std::nullopt;
		case kind::disj:
			return both() ? dnf_or(std::move(*v[n.lhs]), *v[n.rhs], cap)
				: std::nullopt;
		}
		return std::nullopt;
	});
}

/**
 * @brief The DNF cubes of the label @p text.
 * @return the cubes, or nullopt when @p text is not a label or its
 * expansion passes @p max_cubes
 */
inline std::optional<dnf> to_dnf(std::string_view text,
	size_t max_cubes = ltl_guard_max_cubes())
{
	auto e = read(text);
	if (!e) return std::nullopt;
	return to_dnf(*e, max_cubes);
}

// ── Sum of products as printed ───────────────────────────────────────────────

/** @brief A literal as the label spells it: an AP index or a constant, with
 * its polarity. */
struct sop_lit {
	kind k;         // tt, ff or ap
	size_t ap = 0;
	bool pos = true;
	std::string str() const {
		return (pos ? "" : "!") + (k == kind::ap ? std::to_string(ap)
			: std::string(k == kind::tt ? "t" : "f"));
	}
};

/**
 * @brief The products of @p e as written, when @p e is a disjunction of
 * conjunctions of literals (`t`, `f`, an AP index, or one of them negated).
 * @return the products, or nullopt for any other shape
 */
inline std::optional<std::vector<std::vector<sop_lit>>> sum_of_products(
	const expr& e)
{
	std::vector<std::vector<sop_lit>> out;
	auto literal = [&](size_t i, std::vector<sop_lit>& p) {
		const node* n = &e.nodes[i];
		bool pos = true;
		if (n->k == kind::neg) pos = false, n = &e.nodes[n->lhs];
		if (n->k != kind::tt && n->k != kind::ff && n->k != kind::ap)
			return false;
		p.push_back({ n->k, n->ap, pos });
		return true;
	};
	std::vector<size_t> todo{ e.nodes.size() - 1 }, products;
	while (!todo.empty()) {
		size_t i = todo.back();
		todo.pop_back();
		if (e.nodes[i].k == kind::disj)
			todo.push_back(e.nodes[i].rhs), todo.push_back(e.nodes[i].lhs);
		else products.push_back(i);
	}
	for (size_t i : products) {
		std::vector<sop_lit> p;
		std::vector<size_t> lits{ i };
		while (!lits.empty()) {
			size_t k = lits.back();
			lits.pop_back();
			if (e.nodes[k].k == kind::conj)
				lits.push_back(e.nodes[k].rhs),
				lits.push_back(e.nodes[k].lhs);
			else if (!literal(k, p)) return std::nullopt;
		}
		out.push_back(std::move(p));
	}
	return out;
}

} // namespace idni::tau_lang::hoa_label

#endif // __IDNI__TAU__HOA_LABEL_H__
