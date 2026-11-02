// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_SPLITTER_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_SPLITTER_TMPL_H__

#include "boolean_algebras/hsb/hsb.h"

namespace idni::tau_lang {

// =============================================================================
// detail — LP feasibility helpers (QF_LRA via cvc5)
// =============================================================================

namespace detail {

/// One linear constraint `⟨w,x⟩ + b < 0` (or `<= 0`) handed to the LRA solver.
struct linear_constraint {
	/// Coefficients; entries past the dimension are ignored.
	std::vector<hsb_rational> w;
	/// Constant term.
	hsb_rational b;
	bool strict; ///< true => strict (<), false => non-strict (<=)
};

/// Collect all halfspace constraints from an AND-tree into @p out.
/// Returns false iff the node is null or structurally unsatisfiable (contains
/// bot).
/// @pre Must only be called on individual DNF clauses (pure conjunctions of
///      halfspaces); an or_/not_ node contributes no constraint, so the
///      clause is over-approximated.
inline bool collect_conjunction(tref n,
                                std::vector<linear_constraint>& out) {
	if (!n) return false;
	auto k = static_cast<hsb::kind>(hsb_tree::get(n).value.nt);
	switch (k) {
	case hsb::kind::bot:  return false;
	case hsb::kind::top:  return true;
	case hsb::kind::halfspace: {
		const auto& hs = hsb_halfspace_pool::get(hsb_tree::get(n).value.data);
		out.push_back({hs.w, hs.b, hs.is_strict()});
		return true;
	}
	case hsb::kind::and_:
		return collect_conjunction(hsb_tree::get(n).first(), out)
		    && collect_conjunction(hsb_tree::get(n).second(), out);
	default:
		// or_/not_ should not appear in a DNF clause; treat conservatively.
		return true;
	}
}

/// The real variables x0…x_{dim-1} of a QF_LRA query.
inline std::vector<cvc5::Term> lra_vars(size_t dim) {
	std::vector<cvc5::Term> vars(dim);
	for (size_t i = 0; i < dim; ++i)
		vars[i] = cvc5_term_manager.mkConst(
			cvc5_term_manager.getRealSort(),
			"x" + std::to_string(i));
	return vars;
}

/// The cvc5 term of @p c over @p vars. Every value enters cvc5 as its exact
/// fraction.
inline cvc5::Term lra_constraint_term(const linear_constraint& c,
		const std::vector<cvc5::Term>& vars) {
	auto mk_rational = [](const hsb_rational& v) -> cvc5::Term {
		return cvc5_term_manager.mkReal(v.to_fraction_string());
	};
	cvc5::Term sum = mk_rational(c.b);
	for (size_t i = 0; i < std::min(c.w.size(), vars.size()); ++i) {
		if (c.w[i].is_zero()) continue;
		cvc5::Term prod = cvc5_term_manager.mkTerm(
			cvc5::Kind::MULT, {mk_rational(c.w[i]), vars[i]});
		sum = cvc5_term_manager.mkTerm(cvc5::Kind::ADD, {sum, prod});
	}
	cvc5::Term zero = cvc5_term_manager.mkReal(0);
	return c.strict
		? cvc5_term_manager.mkTerm(cvc5::Kind::LT,  {sum, zero})
		: cvc5_term_manager.mkTerm(cvc5::Kind::LEQ, {sum, zero});
}

/// Set up a cvc5 QF_LRA solver, create the variables, assert all constraints
/// in @p cs, and return the variable terms.
inline std::vector<cvc5::Term> setup_lra_solver(
		cvc5::Solver& solver,
		const std::vector<linear_constraint>& cs,
		size_t dim) {
	solver.setOption("produce-models", "true");
	solver.setLogic("QF_LRA");
	auto vars = lra_vars(dim);
	for (auto& c : cs) solver.assertFormula(lra_constraint_term(c, vars));
	return vars;
}

/// The cvc5 term of the formula @p n over @p vars, of size linear in the
/// shared nodes of @p n.
inline cvc5::Term lra_formula_term(tref n, const std::vector<cvc5::Term>& vars,
		std::unordered_map<tref, cvc5::Term>& memo) {
	if (!n) return cvc5_term_manager.mkBoolean(false);
	if (auto it = memo.find(n); it != memo.end()) return it->second;
	auto sub = [&](tref c) { return lra_formula_term(c, vars, memo); };
	const auto& t = hsb_tree::get(n);
	cvc5::Term out;
	switch (static_cast<hsb::kind>(t.value.nt)) {
	case hsb::kind::bot: out = cvc5_term_manager.mkBoolean(false); break;
	case hsb::kind::top: out = cvc5_term_manager.mkBoolean(true); break;
	case hsb::kind::halfspace: {
		const auto& hs = hsb_halfspace_pool::get(t.value.data);
		out = lra_constraint_term({hs.w, hs.b, hs.is_strict()}, vars);
		break;
	}
	case hsb::kind::and_:
		out = cvc5_term_manager.mkTerm(cvc5::Kind::AND,
			{sub(t.first()), sub(t.second())});
		break;
	case hsb::kind::or_:
		out = cvc5_term_manager.mkTerm(cvc5::Kind::OR,
			{sub(t.first()), sub(t.second())});
		break;
	case hsb::kind::not_:
		out = cvc5_term_manager.mkTerm(cvc5::Kind::NOT, {sub(t.first())});
		break;
	}
	memo.emplace(n, out);
	return out;
}

/// Whether the formula @p n has a point in R^@p dim, by one QF_LRA query.
inline bool lra_formula_feasible(tref n, size_t dim) {
	cvc5::Solver solver(cvc5_term_manager);
	solver.setLogic("QF_LRA");
	auto vars = lra_vars(dim);
	std::unordered_map<tref, cvc5::Term> memo;
	solver.assertFormula(lra_formula_term(n, vars, memo));
	return solver.checkSat().isSat();
}

/// One QF_LRA solver over R^@p dim for a run of related feasibility checks:
/// constraints stack with push and pop, so a check shares the solver of the
/// one before it and only the constraints that differ are asserted again.
class lra_session {
public:
	explicit lra_session(size_t dim) : solver(cvc5_term_manager) {
		solver.setLogic("QF_LRA");
		vars = lra_vars(dim);
	}
	/// Open a level holding @p c.
	void push(const linear_constraint& c) {
		solver.push();
		solver.assertFormula(lra_constraint_term(c, vars));
	}
	/// Open a level holding the DNF clause @p conj.
	void push(const std::vector<tref>& conj) {
		solver.push();
		std::vector<linear_constraint> cs;
		for (tref nd : conj) if (!collect_conjunction(nd, cs)) {
			solver.assertFormula(cvc5_term_manager.mkBoolean(false));
			return;
		}
		for (const auto& c : cs)
			solver.assertFormula(lra_constraint_term(c, vars));
	}
	/// Close the last level.
	void pop() { solver.pop(); }
	/// Whether the open levels have a point.
	bool feasible() { return solver.checkSat().isSat(); }
private:
	cvc5::Solver solver;
	std::vector<cvc5::Term> vars;
};

/// Find a feasible point for @p cs via cvc5 model extraction; the origin for
/// an empty @p cs, `std::nullopt` when @p cs is infeasible. The point is the
/// exact rational model.
inline std::optional<std::vector<hsb_rational>>
find_feasible_point(const std::vector<linear_constraint>& cs, size_t dim) {
	if (cs.empty())
		return std::vector<hsb_rational>(dim);

	cvc5::Solver solver(cvc5_term_manager);
	auto vars = setup_lra_solver(solver, cs, dim);

	if (!solver.checkSat().isSat())
		return std::nullopt;

	std::vector<hsb_rational> pt(dim);
	for (size_t i = 0; i < dim; ++i) {
		auto v = hsb_rational::parse(
			solver.getValue(vars[i]).getRealValue());
		// cvc5 prints a real model value as `[-]p/q` or `[-]p`.
		DBG(assert(v.has_value()));
		if (!v) return std::nullopt;
		pt[i] = std::move(*v);
	}
	return pt;
}

/// Find a feasible point of @p cs additionally satisfying `x[axis] > val`
/// (@p greater_than) or `x[axis] < val`; `std::nullopt` when there is none.
/// @pre `axis < dim`.
inline std::optional<std::vector<hsb_rational>>
find_second_feasible_point(const std::vector<linear_constraint>& cs,
                           size_t dim, size_t axis, const hsb_rational& val,
                           bool greater_than) {
	auto ext = cs;
	linear_constraint extra;
	extra.w.assign(dim, hsb_rational{});
	if (greater_than) { extra.w[axis] = -1; extra.b =  val; extra.strict = true; }
	else              { extra.w[axis] =  1; extra.b = -val; extra.strict = true; }
	ext.push_back(extra);
	return find_feasible_point(ext, dim);
}

/// The normalized half-space node of the complement of @p hs.
inline tref complement_literal(tref hs) {
	const auto& h = hsb_halfspace_pool::get(hsb_tree::get(hs).value.data);
	hsb_halfspace neg = h;
	for (auto& c : neg.w) c = -c;
	neg.b = -neg.b;
	return hsb::mk_hs(neg.normalize());
}

// Forward declarations needed by to_dnf.
inline tref push_neg(tref n);
inline size_t infer_dim(tref n);

/// Append the DNF of @p n to @p dnf (each clause a conjunction of halfspace
/// nodes; an empty clause is true, no clause is false). A not_ node is
/// expanded through @ref push_neg; a null @p n is bottom and adds nothing.
/// The clause count can grow exponentially.
/// @return false, with @p dnf incomplete, past `hsb-max-dnf-clauses` clauses.
inline bool to_dnf(tref n, std::vector<std::vector<tref>>& dnf) {
	if (!n) return true;
	auto k = static_cast<hsb::kind>(hsb_tree::get(n).value.nt);
	switch (k) {
	case hsb::kind::bot:
		break; // contributes nothing
	case hsb::kind::top:
		dnf.push_back({}); // empty conjunction = tautology
		break;
	case hsb::kind::halfspace:
		dnf.push_back({n});
		break;
	case hsb::kind::and_: {
		std::vector<std::vector<tref>> left, right;
		if (!to_dnf(hsb_tree::get(n).first(), left)) return false;
		if (!to_dnf(hsb_tree::get(n).second(), right)) return false;
		if (hsb_max_dnf_clauses && !left.empty() && right.size()
			> (hsb_max_dnf_clauses - dnf.size()) / left.size())
				return false;
		// The negation of a normalized element is a product of its cells,
		// disjoint, so nearly all of a large product is empty.
		const bool prune = left.size() * right.size() > 64;
		std::unordered_map<tref, tref> complements;
		auto complement = [&](tref hs) {
			auto [it, added] = complements.try_emplace(hs, nullptr);
			if (added) it->second = complement_literal(hs);
			return it->second;
		};
		std::optional<lra_session> lra;
		if (prune) lra.emplace(infer_dim(n));
		for (auto& l : left) {
			if (prune) {
				lra->push(l);
				if (!lra->feasible()) { lra->pop(); continue; }
			}
			for (auto& r : right) {
				if (prune && std::any_of(r.begin(), r.end(), [&](tref hs) {
					return std::find(l.begin(), l.end(), complement(hs))
						!= l.end(); })) continue;
				auto conj = l;
				std::vector<tref> added;
				for (tref hs : r)
					if (std::find(l.begin(), l.end(), hs) == l.end())
						added.push_back(hs);
				conj.insert(conj.end(), added.begin(), added.end());
				if (prune) {
					lra->push(added);
					const bool feasible = lra->feasible();
					lra->pop();
					if (!feasible) continue;
				}
				dnf.push_back(std::move(conj));
			}
			if (prune) lra->pop();
		}
		break;
	}
	case hsb::kind::or_:
		return to_dnf(hsb_tree::get(n).first(), dnf)
			&& to_dnf(hsb_tree::get(n).second(), dnf);
	case hsb::kind::not_:
		return to_dnf(push_neg(hsb_tree::get(n).first()), dnf);
	}
	return !hsb_max_dnf_clauses || dnf.size() <= hsb_max_dnf_clauses;
}

/// Returns `¬n` with the negation pushed down to the halfspace leaves, each
/// replaced by its complement halfspace (`¬¬A` gives `A` as is); a null @p n
/// gives bottom.
inline tref push_neg(tref n) {
	if (!n) return hsb::mk_bot();
	auto k = static_cast<hsb::kind>(hsb_tree::get(n).value.nt);
	switch (k) {
	case hsb::kind::bot:       return hsb::mk_top();
	case hsb::kind::top:       return hsb::mk_bot();
	case hsb::kind::halfspace: {
		size_t idx = hsb_tree::get(n).value.data;
		return hsb::mk_hs_by_index(hsb_halfspace_pool::complement_index(idx));
	}
	case hsb::kind::not_:      return hsb_tree::get(n).first(); // ~~A = A
	case hsb::kind::and_:
		return hsb::mk_or(push_neg(hsb_tree::get(n).first()),
		                  push_neg(hsb_tree::get(n).second()));
	case hsb::kind::or_:
		return hsb::mk_and(push_neg(hsb_tree::get(n).first()),
		                   push_neg(hsb_tree::get(n).second()));
	}
	return hsb::mk_bot();
}

/// Convert to NNF using post_order — each not_ node has its child push_neg'd.
inline tref to_nnf(tref root) {
	auto f = [](tref n) -> tref {
		auto k = static_cast<hsb::kind>(hsb_tree::get(n).value.nt);
		if (k != hsb::kind::not_) return n;
		return push_neg(hsb_tree::get(n).first());
	};
	return post_order<hsb_node>(root).apply_unique(f);
}

/// Infer ambient dimension from the formula tree: the largest dimension of a
/// halfspace in it, 0 when it has none.
inline size_t infer_dim(tref n) {
	if (!n) return 0;
	auto k = static_cast<hsb::kind>(hsb_tree::get(n).value.nt);
	switch (k) {
	case hsb::kind::halfspace:
		return hsb_halfspace_pool::get(hsb_tree::get(n).value.data).dimension();
	case hsb::kind::and_:
	case hsb::kind::or_:
		return std::max(infer_dim(hsb_tree::get(n).first()),
		                infer_dim(hsb_tree::get(n).second()));
	case hsb::kind::not_:
		return infer_dim(hsb_tree::get(n).first());
	default: return 0;
	}
}

} // namespace detail

// =============================================================================
// Free function implementations
// =============================================================================

/**
 * @brief Returns true iff @p x is semantically equivalent to bottom (∅).
 *
 * Asks cvc5 once, with the Boolean structure of @p x as it stands: a DNF of
 * the negation of a normalized element grows exponentially.
 */
inline bool is_hsb_zero(const hsb& x) {
	auto k = x.root_kind();
	if (k == hsb::kind::bot) return true;
	if (k == hsb::kind::top) return false;
	return !detail::lra_formula_feasible(x.root_ref(),
		detail::infer_dim(x.root_ref()));
}

/**
 * @brief Returns true iff @p x is semantically equivalent to top (R^d).
 */
inline bool is_hsb_one(const hsb& x) {
	return is_hsb_zero(~x);
}

/// @brief No-op symbol simplifier (required by the tau dispatcher interface).
inline tref simplify_hsb_symbol(tref sym) { return sym; }

/// @brief No-op term simplifier (required by the tau dispatcher interface).
inline tref simplify_hsb_term(tref t) { return t; }

/**
 * @brief Returns a sub-element y with `bot < y < x` when one is found.
 *
 * Contract note: @p st is ignored, and when no LRA split is found
 * the input @p x is returned UNCHANGED -- callers looping "split until
 * proper subset" must guard against a fixpoint (sbf_splitter, by
 * contrast, always makes progress). Bottom splits to bottom, top to
 * `{x[0] < 0}`; otherwise the split cuts one feasible DNF clause in half
 * along an axis between two of its points.
 * @param x Element to split.
 * @return The sub-element, or @p x itself when no split is found.
 */
inline hsb hsb_splitter(const hsb& x, splitter_type /*st*/) {
	auto k = x.root_kind();
	if (k == hsb::kind::bot) return hsb::bottom();
	if (k == hsb::kind::top) {
		hsb_halfspace h; h.w = {1};
		return hsb::from_halfspace(h);
	}

	size_t dim = detail::infer_dim(x.root_ref());
	if (dim == 0) dim = 1;

	auto nnf = detail::to_nnf(x.root_ref());
	std::vector<std::vector<tref>> dnf;
	if (!detail::to_dnf(nnf, dnf)) return x;

	for (auto& conj : dnf) {
		std::vector<detail::linear_constraint> cs;
		bool ok = true;
		for (auto nd : conj)
			if (!detail::collect_conjunction(nd, cs)) { ok = false; break; }
		if (!ok) continue;

		auto pt_a = detail::find_feasible_point(cs, dim);
		if (!pt_a) continue;

		for (size_t axis = dim; axis-- > 0; ) {
			auto pt_b = detail::find_second_feasible_point(
				cs, dim, axis, (*pt_a)[axis], true);
			if (!pt_b)
				pt_b = detail::find_second_feasible_point(
					cs, dim, axis, (*pt_a)[axis], false);
			if (!pt_b) continue;

			hsb_rational c = ((*pt_a)[axis] + (*pt_b)[axis]) / 2;
			hsb_halfspace h;
			h.w.assign(dim, hsb_rational{}); h.w[axis] = 1; h.b = -c;
			hsb hs   = hsb::from_halfspace(h);
			hsb lower = x & hs;
			hsb upper = x & ~hs;
			if (!is_hsb_zero(lower) && !is_hsb_zero(upper)) return lower;
		}
	}
	return x;
}

/**
 * @brief Returns a fixed non-trivial seed element `{x[0] < 0}`.
 */
inline hsb hsb_splitter_one() {
	hsb_halfspace h; h.w = {1};
	return hsb::from_halfspace(h);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB__HSB_SPLITTER_TMPL_H__
