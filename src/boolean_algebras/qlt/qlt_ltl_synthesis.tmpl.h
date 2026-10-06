// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file qlt_ltl_synthesis.tmpl.h
 * @brief qlt's propositional-synthesis fast paths for LTL(ABA): Algorithms A, B, D.
 *
 * Included from qlt_descriptor.tmpl.h and nowhere else. Core reaches all of it
 * through one capability, try_propositional_synthesis, so the (Q,<) order-type
 * theory stays out of the LTL pipeline.
 *
 * ltl_aba.h cannot be included here -- it reaches normalizer.h, which is not
 * available where qlt.h enters through the generated pack header -- so what
 * core supplies is forward-declared instead and completed at instantiation.
 */

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_LTL_SYNTHESIS_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_LTL_SYNTHESIS_TMPL_H__

#include <cctype>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include "algorithm_a_skeleton.h"
#include "algorithm_b_skeleton.h"
#include "algorithm_d_game.h"
#include "ba_types.h"
#include "boolean_algebras/qlt/omcat_constants.h"
#include "boolean_algebras/qlt/qlt.h"
#include "boolean_algebras/qlt/qlt_qe.tmpl.h"
#include "ltl_aba_limits.h"
#include <limits>
// Unlike ltl_aba.h / normalizer.h, definitions.h only reaches io_context.h,
// so it is safe to include directly here -- needed by the LA-10 constant-
// output witness builder to resolve a freshly-parsed atom's io_vars.
#include "definitions.h"
// Reaches nothing beyond the standard library itself, unlike normalizer.h,
// so it is safe here; needed for `result<...>` before ltl_aba_result.h.
#include "tau_diagnostics.h"
#include "ltl_aba_result.h"
// Generic LTL-over-strings backend (ltlsynt/autfilt/ltlfilt); no tau tree
// type, so it carries none of normalizer.h's include weight and is safe
// here. Gives is_tautology(), the ltlfilt fast path below.
#include "backends/spot/spot.h"

namespace idni::tau_lang {

result<std::pair<bool, std::string>> call_ltlsynt(const std::string& formula,
	const std::vector<std::string>& input_props,
	const std::vector<std::string>& output_props);

template <NodeType node>
std::vector<std::pair<tref, std::string>> extract_data_atoms(tref fm);

template <NodeType node>
result<std::string> ltl_skeleton(tref fm,
	const std::vector<std::pair<tref, std::string>>& atoms);

template <NodeType node>
bool atom_has_any_input(tref atom);

template <NodeType node>
bool is_pure_input_atom(tref atom);

template <NodeType node>
tref resolve_io_vars(const io_context<node>& ctx, tref fm);

// LT-8 / LA-N1: used by the Algorithm D fast path to un-vacuate the ABA
// oracle over the automaton's own d_i-named atoms (ltl_aba_normalization.tmpl.h).
// Declared without the real definition's default arguments to avoid a
// "redefinition of default argument" diagnostic; the one call site here
// passes all four arguments explicitly.
template <NodeType node>
static result<void> add_consistency_constraints(
	const std::vector<std::pair<tref, std::string>>& atoms,
	std::string& skeleton,
	std::vector<std::string>* out_constraints,
	bool polarity_complete,
	std::string seed_input_assumptions);

// What a qlt atom's truth value is under one T_3 type or one constant-output
// assignment; `undecided` marks an atom this path cannot evaluate.
enum class atom_verdict { holds, fails, undecided };

// A decided boolean as a verdict: `true` holds, `false` fails.
inline atom_verdict verdict_from_bool(bool b) {
	return b ? atom_verdict::holds : atom_verdict::fails;
}

// ── Algorithm A: binary T_3 type encoding ────────────────────────────────────
//
// TAU_LTL_ALG=A selects this path for single-stream pure-qlt formulas.
// Enumerates T_3 = (memory, input, output) order-types for (ℚ,<), binary-encodes
// them into ⌈log₂|T_3|⌉ system output Q-bits, and calls ltlsynt on the resulting
// propositional skeleton. No ABA oracle needed: the T_3 encoding is exact.
// Restricted to: all atoms qlt-type, single output var, lookback ≤ 1.

// Role of an io_var in the (m=memory, x=input, y=output) T_3 triple.
enum class t3_var_role { M, X, Y };

template <NodeType node>
static std::optional<t3_var_role> t3_role_of(tref io_var) {
	const std::string& nm = get_var_name<node>(io_var);
	if (nm.empty()) return std::nullopt;
	int_t shift = get_io_var_shift<node>(io_var);
	if (nm[0] == 'o' && shift == 0) return t3_var_role::Y;
	if (nm[0] == 'i' && shift == 0) return t3_var_role::X;
	if (nm[0] == 'o' && shift == 1) return t3_var_role::M;
	return std::nullopt;
}

// True iff all atoms are qlt-typed, have lookback ≤ 1, and each comparison
// side has at most one io_var (no compound expressions like o1 & i1).
template <NodeType node>
static bool is_algorithm_a_applicable(
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	using tau = tree<node>;
	if (atoms.empty()) return false;
	// LG-9: bound K -- the A/B/D encodings compute 1 << K (signed-shift
	// UB at K >= 31) and enumerate 2^K masks (exponential strings well
	// before that). The cap is qlt's runtime option `qlt-t3-cap`
	// (qlt_t3_encoding_cap, qlt.h), shared with the semantic PWR.
	const int t3_cap = qlt_t3_encoding_cap_effective();
	if ((int) atoms.size() > t3_cap) {
		LOG_WARNING << "[ltl_aba] " << atoms.size() << " data atoms "
			"exceed the T3-encoding cap (" << t3_cap << ", option "
			"qlt-t3-cap); falling back to the default ABA-oracle path";
		return false;
	}
	for (auto& [f, _] : atoms) {
		if (!ba_descriptor<qlt, node>::owns_type(find_ba_type<node>(f))) return false;
		auto bad = tau::get(f).find_top([](tref n) {
			if (!is_child<node>(n, tau::io_var)) return false;
			return get_io_var_shift<node>(n) > 1;
		});
		if (bad) return false;
		// Reject compound atoms where a comparison side has >1 io_var
		// (e.g. (o1 & i1) = {c}). T3 encoding only handles single-var sides.
		const auto& t = tau::get(f);
		if (t.has_child()) {
			tref lhs = t[0].first();
			tref rhs = t[0].second();
			if (lhs && tau::get(lhs).select_top(is_child<node, tau::io_var>).size() > 1)
				return false;
			if (rhs && tau::get(rhs).select_top(is_child<node, tau::io_var>).size() > 1)
				return false;
		}
	}
	return true;
}

// The verdict of the comparison @p op of @p atom with each stream it reads at
// the point @p value_of gives it, read as qlt_point_eval reads points: `=` a
// constant is being that set, an order atom compares with every point of it.
// The point stands for its whole T1 cell only when every finite endpoint of the
// atom's constants is one of @p constants; otherwise, and for a named or
// inexact endpoint, the verdict is undecided.
template <NodeType node, typename F>
static atom_verdict qlt_atom_at_points(tref atom, size_t op,
	const std::vector<omcat::rational>& constants, F&& value_of)
{
	using tau = tree<node>;
	auto ends = qlt_point_eval<node>::ends_of(atom);
	if (!ends) return atom_verdict::undecided;
	for (const auto& e : *ends) {
		bool cut = false;
		for (const auto& c : constants)
			if (omcat::cmp(c, omcat::rational(e.p, e.q)) == 0) {
				cut = true;
				break;
			}
		if (!cut) return atom_verdict::undecided;
	}
	qlt_point_eval<node> ev(std::move(*ends));
	for (tref v : tau::get(atom).select_all([](tref n) {
		return is_child<node>(n, tau::io_var); }))
	{
		std::optional<omcat::rational> x = value_of(v);
		if (!x) return atom_verdict::undecided;
		ev.env.emplace_back(v, qlt_rational(x->p, x->q));
	}
	const auto& t = tau::get(atom);
	auto h = ev.compare(op, t[0].first(), t[0].second());
	if (!h) return atom_verdict::undecided;
	return verdict_from_bool(*h);
}

// The atom's verdict in T3, or `undecided` when it is not a comparison this
// path can evaluate.
template <NodeType node>
static result<atom_verdict> qlt_atom_holds_in_type3(
	tref atom,
	const omcat::qlt_type3& T3,
	const std::vector<omcat::rational>& constants)
{
	using tau = tree<node>;
	result<atom_verdict> r;
	const auto& t = tau::get(atom);
	if (!t.has_child()) return r.with_value(atom_verdict::undecided);
	size_t op = t[0].value.get_nt();
	if      (op == tau::bf_nlt)   op = tau::bf_gteq;
	else if (op == tau::bf_ngt)   op = tau::bf_lteq;
	else if (op == tau::bf_ngteq) op = tau::bf_lt;
	else if (op == tau::bf_nlteq) op = tau::bf_gt;
	if (op != tau::bf_lt  && op != tau::bf_lteq && op != tau::bf_gt &&
	    op != tau::bf_gteq && op != tau::bf_eq  && op != tau::bf_neq)
		return r.with_value(atom_verdict::undecided);

	tref lhs = t[0].first();
	tref rhs = t[0].second();
	// find_top returns the PARENT node (whose first child is io_var) because
	// get_var_name / get_io_var_shift expect that parent, not the io_var itself.
	tref lhs_io = tau::get(lhs).find_top([](tref n) {
		return is_child<node>(n, tau::io_var); });
	tref rhs_io = tau::get(rhs).find_top([](tref n) {
		return is_child<node>(n, tau::io_var); });

	auto flip_rel = [](omcat::relation r) {
		return r == omcat::relation::LT ? omcat::relation::GT :
		       r == omcat::relation::GT ? omcat::relation::LT : omcat::relation::EQ;
	};
	auto rel_holds = [&](omcat::relation rel, size_t nt) -> bool {
		if (nt == tau::bf_lt)   return rel == omcat::relation::LT;
		if (nt == tau::bf_lteq) return rel != omcat::relation::GT;
		if (nt == tau::bf_gt)   return rel == omcat::relation::GT;
		if (nt == tau::bf_gteq) return rel != omcat::relation::LT;
		if (nt == tau::bf_eq)   return rel == omcat::relation::EQ;
		if (nt == tau::bf_neq)  return rel != omcat::relation::EQ;
		return false;
	};

	if (lhs_io && rhs_io) {
		auto rl = t3_role_of<node>(lhs_io);
		auto rr = t3_role_of<node>(rhs_io);
		if (!rl || !rr) return r.with_value(atom_verdict::undecided);
		omcat::relation rel;
		auto p = std::make_pair(*rl, *rr);
		using R = t3_var_role;
		if      (p == std::make_pair(R::M, R::X)) rel = T3.rel_mx;
		else if (p == std::make_pair(R::X, R::M)) rel = flip_rel(T3.rel_mx);
		else if (p == std::make_pair(R::M, R::Y)) rel = T3.rel_my;
		else if (p == std::make_pair(R::Y, R::M)) rel = flip_rel(T3.rel_my);
		else if (p == std::make_pair(R::X, R::Y)) rel = T3.rel_xy;
		else if (p == std::make_pair(R::Y, R::X)) rel = flip_rel(T3.rel_xy);
		else return r.with_value(atom_verdict::undecided); // same role (e.g. y vs y)
		return r.with_value(verdict_from_bool(rel_holds(rel, op)));
	}

	if (lhs_io || rhs_io) {
		return r.with_value(qlt_atom_at_points<node>(atom, op, constants,
			[&](tref io) -> std::optional<omcat::rational> {
				auto role = t3_role_of<node>(io);
				if (!role) return std::nullopt;
				if (*role == t3_var_role::M) return T3.restrict_m().realize();
				if (*role == t3_var_role::X) return T3.restrict_x().realize();
				return T3.restrict_y().realize();
			}));
	}
	return r.with_value(atom_verdict::undecided);
}

// ── Algorithm A/B soundness guards (shared with semantic_pwr_optimal) ────────
//
// Both guards below gate the T_3 symbolic encoding.  They were inline in
// `solve_ltl_aba` and `semantic_pwr_optimal` ran the SAME encoding without
// either of them (LS-2), so they are factored out here and called from both.

// Algorithm A's T_3 encoding only handles atoms whose truth value is decidable
// from a T_3 type plus the formula's named rational constants.  Atoms
// involving the qlt extremes `{top}:qlt` / `{bot}:qlt` — or any constant whose
// finite-rational witness is empty — yield `qlt_atom_holds_in_type3 ==
// atom_verdict::undecided` for every type, leaving the atom unconstrained
// in the symbolic encoding.  Without this guard ltlsynt happily synthesises a
// strategy where `α` and `¬α` both hold simultaneously, returning REALIZABLE
// for direct contradictions like `F(o1={top}) && G(o1!={top})`.
template <NodeType node>
static result<bool> alg_a_can_classify(
    tref fm, const std::vector<std::pair<tref, std::string>>& atoms)
{
	result<bool> r;
	TAU_TRY(auto a_constants, omcat::collect_qlt_constants<node>(fm));
	auto a_T3        = omcat::enumerate_qlt_T3(a_constants);
	if (a_T3.empty()) return r.with_value(false);
	for (auto& [f, _] : atoms) {
		bool any_determined = false;
		for (auto& t : a_T3) {
			TAU_TRY(auto h,
				qlt_atom_holds_in_type3<node>(f, t, a_constants));
			if (h != atom_verdict::undecided) { any_determined = true; break; }
		}
		if (!any_determined) return r.with_value(false);
	}
	return r.with_value(true);
}

// Algorithm A's T_3 encoding has a SINGLE current-output slot (Y) and a SINGLE
// past-output slot (M).  Two distinct output variables get conflated into the
// same slot, so `o1[t]>0 && o2[t]<0` becomes "Y>0 && Y<0" — unsatisfiable in
// any T_3 type — and the encoding returns a spurious verdict.  Multi-input is
// fine: t3_role_of merges i_k → X but those flow through Algorithm B's P_σ
// encoding, which is distinguisher-friendly; the conflation is harmful only on
// the OUTPUT side.
template <NodeType node>
static size_t count_distinct_output_vars(
    const std::vector<std::pair<tref, std::string>>& atoms)
{
	std::set<std::string> names;
	for (auto& [f, _] : atoms) {
		const auto& t = tree<node>::get(f);
		if (!t.has_child()) continue;
		auto add_side = [&](tref side) {
			if (!side) return;
			tref iv = tree<node>::get(side).find_top([](tref n) {
				return is_child<node>(n, tree<node>::io_var); });
			if (!iv) return;
			const std::string& nm = get_var_name<node>(iv);
			if (!nm.empty() && nm[0] == 'o') names.insert(nm);
		};
		add_side(t[0].first());
		add_side(t[0].second());
	}
	return names.size();
}

// Evaluate a pure-output atom under a per-variable T₁ assignment (constant-output
// strategy). Variable name keyed — unlike qlt_atom_holds_in_type3 which uses
// fixed M/X/Y roles and can't distinguish o1 from o2. Treats o_k[t-s] (any
// shift) as aliasing to o_k, since the strategy is constant over time.
// Returns `undecided` if the atom involves an input variable or can't be evaluated.
template <NodeType node>
static result<atom_verdict> eval_pure_output_atom_at(
	tref atom,
	const std::map<std::string, int>& var_pos,
	const std::vector<omcat::rational>& constants)
{
	using tau = tree<node>;
	result<atom_verdict> r;
	if (atom_has_any_input<node>(atom)) return r.with_value(atom_verdict::undecided);
	const auto& t = tau::get(atom);
	if (!t.has_child()) return r.with_value(atom_verdict::undecided);
	size_t op = t[0].value.get_nt();
	if      (op == tau::bf_nlt)   op = tau::bf_gteq;
	else if (op == tau::bf_ngt)   op = tau::bf_lteq;
	else if (op == tau::bf_ngteq) op = tau::bf_lt;
	else if (op == tau::bf_nlteq) op = tau::bf_gt;
	if (op != tau::bf_lt  && op != tau::bf_lteq && op != tau::bf_gt &&
	    op != tau::bf_gteq && op != tau::bf_eq  && op != tau::bf_neq)
		return r.with_value(atom_verdict::undecided);

	tref lhs = t[0].first();
	tref rhs = t[0].second();
	tref lhs_io = tau::get(lhs).find_top([](tref n) {
		return is_child<node>(n, tau::io_var); });
	tref rhs_io = tau::get(rhs).find_top([](tref n) {
		return is_child<node>(n, tau::io_var); });

	auto lookup = [&](tref io_parent) -> std::optional<int> {
		const std::string& nm = get_var_name<node>(io_parent);
		auto it = var_pos.find(nm);
		if (it == var_pos.end()) return std::nullopt;
		return it->second;
	};

	auto rel_holds = [&](omcat::relation rel, size_t o) -> bool {
		if (o == tau::bf_lt)   return rel == omcat::relation::LT;
		if (o == tau::bf_lteq) return rel != omcat::relation::GT;
		if (o == tau::bf_gt)   return rel == omcat::relation::GT;
		if (o == tau::bf_gteq) return rel != omcat::relation::LT;
		if (o == tau::bf_eq)   return rel == omcat::relation::EQ;
		if (o == tau::bf_neq)  return rel != omcat::relation::EQ;
		return false;
	};

	if (lhs_io && rhs_io) {
		auto p1 = lookup(lhs_io);
		auto p2 = lookup(rhs_io);
		if (!p1 || !p2) return r.with_value(atom_verdict::undecided);
		omcat::qlt_type1 t1a{*p1, constants};
		omcat::qlt_type1 t1b{*p2, constants};
		omcat::relation rel;
		if (*p1 == *p2) {
			// Same T₁ position: constant strategy picks same value → EQ.
			rel = omcat::relation::EQ;
		} else {
			omcat::rational va = t1a.realize();
			omcat::rational vb = t1b.realize();
			int c = omcat::cmp(va, vb);
			rel = c < 0 ? omcat::relation::LT : (c == 0 ? omcat::relation::EQ : omcat::relation::GT);
		}
		return r.with_value(verdict_from_bool(rel_holds(rel, op)));
	}
	if (lhs_io || rhs_io) {
		return r.with_value(qlt_atom_at_points<node>(atom, op, constants,
			[&](tref io) -> std::optional<omcat::rational> {
				auto p = lookup(io);
				if (!p) return std::nullopt;
				return omcat::qlt_type1{ *p, constants }.realize();
			}));
	}
	return r.with_value(atom_verdict::undecided);
}

// Pre-check: is the formula REALIZABLE via a constant-output strategy?
// Enumerates T₁ positions per output variable; for each combo, substitutes
// pure-output atom truth values into the LTL skeleton and uses `ltlfilt` to
// simplify. If any combo reduces to "1" (true), the system wins with that
// constant output choice. This fast-path avoids the expensive Algorithm B
// ltlsynt call for formulas with trivially-satisfiable U/W/R right-sides.
template <NodeType node>
// LA-10: on success, returns the WINNING assignment (output stream name →
// T1 position of its constant value) instead of a bare true — the caller
// materialises it as `always(⋀ o_k = c_k)` so the strategy survives into
// execution and codegen instead of being discarded.
static result<std::optional<std::map<std::string, int>>> constant_output_realizable(
	tref fm,
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	using tau = tree<node>;
	result<std::optional<std::map<std::string, int>>> r;
	std::set<std::string> out_names;
	for (auto& [f, _] : atoms) {
		auto ios = tau::get(f).select_top(is_child<node, tau::io_var>);
		for (tref io_parent : ios) {
			const std::string& nm = get_var_name<node>(io_parent);
			if (!nm.empty() && nm[0] == 'o') out_names.insert(nm);
		}
	}
	if (out_names.empty()) return r.with_value(std::nullopt);

	TAU_TRY(auto constants, omcat::collect_qlt_constants<node>(fm));
	int T1_size = 2 * (int)constants.size() + 1;
	if (T1_size <= 0) return r.with_value(std::nullopt);

	std::vector<std::string> out_vec(out_names.begin(), out_names.end());
	int n_out = (int)out_vec.size();
	unsigned long long total_u = 1;
	// Runtime parameter qlt_const_output_max (qlt.h; option
	// qlt-const-output-max); 0 = unlimited, bounded here only by the
	// arithmetic itself.
	const size_t const_output_max = qlt_const_output_max();
	const unsigned long long CAP = const_output_max
		? (unsigned long long) const_output_max
		: std::numeric_limits<unsigned long long>::max() / 2;
	for (int i = 0; i < n_out; ++i) {
		if (total_u > CAP) return r.with_value(std::nullopt);
		if (total_u > CAP / (unsigned long long) T1_size) return r.with_value(std::nullopt);
		total_u *= (unsigned long long)T1_size;
		if (total_u > CAP) return r.with_value(std::nullopt);
	}
	long long total = (long long)total_u;

	TAU_TRY(auto phi_star_base, (ltl_skeleton<node>(fm, atoms)));

	for (long long combo = 0; combo < total; ++combo) {
		std::map<std::string, int> var_pos;
		unsigned long long rem = (unsigned long long)combo;
		for (size_t i = 0; i < out_vec.size(); ++i) {
			var_pos[out_vec[i]] = (int)(rem % (unsigned long long)T1_size);
			rem /= (unsigned long long)T1_size;
		}

		std::string phi = phi_star_base;
		for (size_t i = atoms.size(); i-- > 0; ) {
			TAU_TRY(auto val, eval_pure_output_atom_at<node>(
				atoms[i].first, var_pos, constants));
			if (val == atom_verdict::undecided) continue;
			std::string fp = "p" + std::to_string(i);
			std::string rep = val == atom_verdict::holds ? "true" : "false";
			size_t pos = 0;
			while ((pos = phi.find(fp, pos)) != std::string::npos) {
				size_t end = pos + fp.size();
				bool l_ok = pos == 0 || (!std::isalnum((unsigned char)phi[pos-1])
				                         && phi[pos-1] != '_');
				bool r_ok = end >= phi.size()
				         || (!std::isalnum((unsigned char)phi[end])
				             && phi[end] != '_');
				if (l_ok && r_ok) { phi.replace(pos, fp.size(), rep); pos += rep.size(); }
				else pos = end;
			}
		}

		// Quick pre-filter: a formula with remaining input props can only
		// be a tautology if temporal operators make those props irrelevant.
		// The patterns "A U true", "A W true", "A R true" can propagate to 1.
		// Skip ltlfilt calls where none of these patterns are present.
		bool has_input_prop = false;
		for (size_t ci = 0; ci + 1 < phi.size(); ++ci)
			if (phi[ci] == 'p' && std::isdigit((unsigned char)phi[ci+1])) {
				has_input_prop = true; break;
			}
		if (has_input_prop) {
			bool maybe_taut = (phi.find("U true") != std::string::npos ||
			                   phi.find("W true") != std::string::npos ||
			                   phi.find("R true") != std::string::npos ||
			                   phi.find("U 1)") != std::string::npos  ||
			                   phi.find("W 1)") != std::string::npos  ||
			                   phi.find("R 1)") != std::string::npos);
			if (!maybe_taut) continue;
		}

		// This fast path runs up to CAP times on the default
		// Algorithm-B gate; an ltlfilt failure only leaves this
		// candidate unproven.
		auto taut = is_tautology(phi, ltl_timeout_sec());
		const bool is_taut = taut.has_value() && taut.value();
		if (taut.has_value()) r.merge(std::move(taut));
		else {
			auto sc = r.open("rejected candidate");
			r.info("ltlfilt did not prove the constant output a "
				"tautology", {{label::value, truncate_for_message(phi)}});
			report rep = std::move(taut).report();
			rep.demote_errors_to_warnings();
			r.append(std::move(rep));
		}
		if (is_taut) {
			LOG_DEBUG << "[ltl_aba] constant-output fast-path REALIZABLE "
			          << "(combo=" << combo << ")";
			return r.with_value(std::move(var_pos));
		}
	}
	return r.with_value(std::nullopt);
}

// Shared between solve_ltl_aba_algorithm_a and qlt_semantic_pwr_optimal
// (qlt_semantic_pwr.tmpl.h).
//
// Per-T3-type D-bitmask: bit i of type_A[t] is set iff atom i holds (true or
// undetermined) in T3 type t.
template <NodeType node>
static result<std::vector<int>> qlt_type_A_bitmasks(
	const std::vector<std::pair<tref, std::string>>& atoms,
	const std::vector<omcat::qlt_type3>& T3,
	const std::vector<omcat::rational>& constants)
{
	result<std::vector<int>> r;
	std::vector<int> type_A(T3.size(), 0);
	for (int i = 0; i < (int)atoms.size(); ++i)
		for (size_t t = 0; t < T3.size(); ++t) {
			TAU_TRY(auto h, qlt_atom_holds_in_type3<node>(
				atoms[(size_t) i].first, T3[t], constants));
			if (h != atom_verdict::fails) type_A[t] |= (1 << i);
		}
	return r.with_value(std::move(type_A));
}

// Rename the skeleton's p_i propositions to d_i on word boundaries,
// highest index first so p1 is not clobbered while renaming p10.
static inline std::string rename_skeleton_props_to_d(std::string phi_star,
	int K)
{
	for (int i = K; i-- > 0; ) {
		std::string fp = "p" + std::to_string(i);
		std::string td = "d_" + std::to_string(i);
		size_t pos = 0;
		while ((pos = phi_star.find(fp, pos)) != std::string::npos) {
			size_t end = pos + fp.size();
			bool l_ok = pos == 0
				|| (!std::isalnum((unsigned char)phi_star[pos-1])
				    && phi_star[pos-1] != '_');
			bool r_ok = end >= phi_star.size()
				|| (!std::isalnum((unsigned char)phi_star[end])
				    && phi_star[end] != '_');
			if (l_ok && r_ok) {
				phi_star.replace(pos, fp.size(), td);
				pos += td.size();
			} else pos = end;
		}
	}
	return phi_star;
}

template <NodeType node>
static result<propositional_synthesis<node>>
solve_ltl_aba_algorithm_a(
	tref fm,
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	result<propositional_synthesis<node>> r;

	TAU_TRY(auto constants, omcat::collect_qlt_constants<node>(fm));
	auto T3 = omcat::enumerate_qlt_T3(constants);
	const size_t n_types = T3.size();
	LOG_DEBUG << "[ltl_aba:algA] T3 count=" << n_types
	          << " constants=" << constants.size();
	if (n_types == 0) return r.with_value(synthesis_declined<node>());

	int K = (int)atoms.size();
	// Per-T₃-type D-bitmask, then extract feasible (sigma, rho, A) triples.
	TAU_TRY(auto type_A, qlt_type_A_bitmasks<node>(atoms, T3, constants));

	int T1_size = 2 * (int)constants.size() + 1;
	std::vector<std::tuple<int,int,int>> feasible_set;
	feasible_set.reserve(n_types);
	for (size_t t = 0; t < n_types; ++t)
		feasible_set.emplace_back(T3[t].pos_m, T3[t].pos_y, type_A[t]);

	// Build phi* skeleton and rename p_i → D_i.
	TAU_TRY(auto phi_star_skel, ltl_skeleton<node>(fm, atoms));
	std::string phi_star = rename_skeleton_props_to_d(
		std::move(phi_star_skel), K);

	auto bundle = alg_a::build_algorithm_a_skeleton(T1_size, K, feasible_set, phi_star);
	LOG_DEBUG << "[ltl_aba:algA] skeleton: " << bundle.formula;

	// R-bits + D-bits are system outputs; phi* uses D_i directly as propositions.
	std::vector<std::string> input_props;
	std::vector<std::string> output_props(bundle.outs.begin(), bundle.outs.end());

	TAU_TRY(auto ltlsynt_out, call_ltlsynt(bundle.formula, input_props, output_props));
	auto& [realizable, hoa_text] = ltlsynt_out;
	if (!realizable) return r.with_value(synthesis_unrealizable<node>());

	ltl_aba_solution<node> sol;
	// Populate sol.atoms with the d_i propositions phi_star uses, so
	// downstream consumers (the codegen witness emitter in
	// cpp_codegen.tmpl.h, the safety-formula extractor in
	// ltl_to_safety_formula_full) can map AP names back to the original data
	// atoms and emit qlt witnesses, executable safety formulas, etc.;
	// without them the codegen emits `bool o_d_0` instead of `double o1`.
	//
	// Also populate sol.output_props with the d_i names so
	// build_program_desc's Outputs struct iterates over the data-atom
	// propositions and fills in `double <var>` fields.
	sol.atoms.reserve(atoms.size());
	sol.output_props.reserve(atoms.size());
	for (size_t i = 0; i < atoms.size(); ++i) {
		std::string name = "d_" + std::to_string(i);
		sol.atoms.emplace_back(atoms[i].first, name);
		sol.output_props.push_back(name);
	}
	TAU_TRY(sol.aut, parse_hoa(hoa_text));
	return r.with_value(synthesis_solved(sol));
}

// Algorithm B: P_σ binary encoding — adds ⌈log₂|T₂|⌉ input propositions for
// the T₂ = (pos_m, pos_x, rel_mx) type.  Needed for SOUNDNESS when the formula
// contains input-variable atoms (the system observes x's type via P-bits and can
// then pick the correct output type ρ).
template <NodeType node>
static result<propositional_synthesis<node>>
solve_ltl_aba_algorithm_b(
	tref fm,
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	result<propositional_synthesis<node>> r;

	TAU_TRY(auto constants, omcat::collect_qlt_constants<node>(fm));
	auto T2 = omcat::enumerate_qlt_T2(constants);
	auto T3 = omcat::enumerate_qlt_T3(constants);
	int T2_size = (int)T2.size();
	const size_t n_types = T3.size();
	if (n_types == 0 || T2_size == 0) return r.with_value(synthesis_declined<node>());

	int K       = (int)atoms.size();
	int T1_size = 2 * (int)constants.size() + 1;

	// D-bitmask per T₃ type: the one helper Algorithm A and the semantic
	// PWR use (LS-12); B kept an inline copy until 2026-09-17.
	TAU_TRY(auto type_A, qlt_type_A_bitmasks<node>(atoms, T3, constants));

	// Build T₂ lookup: (pos_m, pos_x, rel_mx) → T₂ index.
	std::map<std::tuple<int,int,int>, int> t2_lookup;
	for (int s = 0; s < T2_size; ++s) {
		// The lookup value keeps the T₂ index as an int.
		const auto& t2 = T2[(size_t) s];
		t2_lookup[{ t2.pos_m, t2.pos_x, (int) t2.rel }] = s;
	}

	// Build feasible_set_b: (T2_idx, rho, A).
	std::vector<std::tuple<int,int,int>> feasible_set_b;
	feasible_set_b.reserve(n_types);
	for (size_t t = 0; t < n_types; ++t) {
		auto key = std::make_tuple(T3[t].pos_m, T3[t].pos_x, (int)T3[t].rel_mx);
		auto it  = t2_lookup.find(key);
		if (it == t2_lookup.end()) continue;
		feasible_set_b.emplace_back(it->second, T3[t].pos_y, type_A[t]);
	}

	// t2_pos_m[σ] = pos_m of T₂[σ].
	std::vector<int> t2_pos_m(T2.size());
	for (size_t s = 0; s < t2_pos_m.size(); ++s) t2_pos_m[s] = T2[s].pos_m;

	// Build phi* skeleton and rename p_i → d_i (LT-16: shared helper).
	TAU_TRY(auto phi_star_skel, ltl_skeleton<node>(fm, atoms));
	std::string phi_star = rename_skeleton_props_to_d(
		std::move(phi_star_skel), K);

	auto bundle = alg_b::build_algorithm_b_skeleton(
		T1_size, T2_size, K, feasible_set_b, t2_pos_m, phi_star);
	LOG_DEBUG << "[ltl_aba:algB] T2=" << T2_size << " T1=" << T1_size
	          << " K=" << K << " n_pbits=" << bundle.n_pbits
	          << " n_rbits=" << bundle.n_rbits;

	TAU_TRY(auto ltlsynt_out, call_ltlsynt(bundle.formula, bundle.ins, bundle.outs));
	auto& [realizable, hoa_text] = ltlsynt_out;
	if (!realizable) return r.with_value(synthesis_unrealizable<node>());

	ltl_aba_solution<node> sol;
	TAU_TRY(sol.aut, parse_hoa(hoa_text));
	// The strategy is over the P_σ / R bookkeeping bits, not over the user's
	// data atoms (`sol.atoms` is intentionally left empty), so it cannot be
	// re-encoded as a safety formula.  See ltl_aba_solution::executable (LT-6).
	sol.executable = false;
	return r.with_value(synthesis_solved(sol));
}

/**
 * @brief Synthesise @p fm propositionally, when qlt's order types can encode it.
 *
 * Declining and proving unrealizable are different answers: the first lets core
 * fall through to the ABA oracle, the second is final. Algorithms A, B and D all
 * produce both, so the result carries the distinction rather than collapsing it.
 */
template <NodeType node>
static result<propositional_synthesis<node>> qlt_try_propositional_synthesis(
	tref fm, const std::vector<std::pair<tref, std::string>>& atoms)
{
	result<propositional_synthesis<node>> r;

	ltl_aba_solution<node> sol;
	sol.atoms = atoms;

	const std::string alg_choice = ltl_algorithm_choice();
	const bool alg_d_mode = alg_choice == "D";
	bool alg_d_has_input = false;
	for (auto& [f, _] : sol.atoms)
		if (atom_has_any_input<node>(f)) { alg_d_has_input = true; break; }
	// D shares A's T_3 encoding, so it needs A's guards too: one output
	// slot, and every atom decided by some T_3 type
	bool alg_d_classifiable = false;
	if (alg_d_mode && !alg_d_has_input
	    && is_algorithm_a_applicable<node>(sol.atoms)
	    && count_distinct_output_vars<node>(sol.atoms) <= 1) {
		TAU_TRY(alg_d_classifiable,
			alg_a_can_classify<node>(fm, sol.atoms));
	}
	if (alg_d_classifiable) {
		TAU_TRY(auto constants, omcat::collect_qlt_constants<node>(fm));
		auto T3 = omcat::enumerate_qlt_T3(constants);
		int K = (int)sol.atoms.size();
		size_t T1_size = 2 * constants.size() + 1;

		// Compute D-bitmask for each T3 type.
		TAU_TRY(auto type_A,
			qlt_type_A_bitmasks<node>(sol.atoms, T3, constants));

		// Build φ*(D_i) (LT-16: shared rename helper).
		TAU_TRY(auto phi_star_skel, ltl_skeleton<node>(fm, sol.atoms));
		std::string phi_star = rename_skeleton_props_to_d(
			std::move(phi_star_skel), K);

		LOG_DEBUG << "[ltl_aba:algD] T3=" << T3.size() << " T1=" << T1_size
		          << " K=" << K << " phi_star=" << phi_star;

		// LG-12: fixed initial memory ρ₀ = type_of(0) — the
		// interpreter's own lookback-at-t=0 convention.
		TAU_TRY(auto realizable, alg_d::solve_algorithm_d(phi_star,
			T1_size, T3, type_A, K,
			alg_d::initial_memory(constants)));
		LOG_DEBUG << "[ltl_aba:algD] result=" << (realizable ? "REALIZABLE" : "UNREALIZABLE");

		if (!realizable) { return r.with_value(synthesis_unrealizable<node>()); }

		// Realizable: call ltlsynt for the strategy automaton.
		//
		// The automaton carries the d_i names, so sol.atoms is renamed
		// to d_i (as Algorithm A does): the ABA oracle and the safety
		// encoding match atoms by name. The strategy call carries the
		// add_consistency_constraints suffix the default path uses,
		// over the renamed atoms, so ltlsynt cannot pick an
		// output-contradictory edge (`d_0 & d_1` for (o1>0) U (o1<0),
		// ALG-D-28): the data-infeasible combinations are excluded from
		// the strategy instead of being scored by the oracle afterwards.
		for (int i = 0; i < K; ++i)
			sol.atoms[(size_t) i].second = "d_" + std::to_string(i);
		std::string strategy_skeleton = phi_star;
		TAU_TRY_VOID(add_consistency_constraints<node>(sol.atoms,
			strategy_skeleton, nullptr, /*polarity_complete=*/false,
			/*seed_input_assumptions=*/""));
		std::vector<std::string> D_outs;
		for (int i = 0; i < K; ++i) D_outs.push_back("d_" + std::to_string(i));
		TAU_TRY(auto ltlsynt_out, call_ltlsynt(strategy_skeleton, {}, D_outs));
		auto& [real2, hoa_text] = ltlsynt_out;
		if (!real2) {
			// Propositional call disagrees: restore the p_i names and fall
			// through to the selection below, which reaches Algorithm A
			// (D's applicability implies A's)
			LOG_DEBUG << "[ltl_aba:algD] ltlsynt disagreed; falling through";
			for (int i = 0; i < K; ++i)
				sol.atoms[(size_t) i].second = "p" + std::to_string(i);
		} else {
			sol.skeleton = strategy_skeleton;
			sol.output_props = D_outs;
			TAU_TRY(sol.aut, parse_hoa(hoa_text));
			return r.with_value(synthesis_solved(sol));
		}
	} else if (alg_d_mode) {
		LOG_DEBUG << "[ltl_aba:algD] not applicable (input variables, non-qlt, "
		             "large lookback, several outputs or an atom outside T_3);"
		             " falling through to default";
	}

	// The choice comes from `--ltl-alg` / `set ltlalg` / TAU_LTL_ALG;
	// ltl_algorithm_choice() (ltl_aba_limits.h) validates it and reports an
	// unrecognised value once (LS-8), returning "" for the default routing.
	const bool alg_b_mode = alg_choice.empty() || alg_choice == "B";
	const bool alg_a_mode = alg_choice == "A";
	if (is_algorithm_a_applicable<node>(sol.atoms)) {
		// Check whether any atom has an input variable.
		bool any_input = false;
		for (auto& [f, _] : sol.atoms)
			if (atom_has_any_input<node>(f)) { any_input = true; break; }

		// Algorithm A's T_3 encoding only handles atoms whose
		// truth value is decidable from a T_3 type plus the
		// formula's named rational constants.  Atoms involving
		// the qlt boolean-algebra extremes `{top}:qlt` /
		// `{bot}:qlt` (or any constant whose finite-rational
		// witness is empty) yield qlt_atom_holds_in_type3 ==
		// atom_verdict::undecided for every type, leaving the atom completely
		// unconstrained in the symbolic encoding.  Without this
		// guard, ltlsynt happily synthesises a strategy where
		// `α` and `¬α` both hold simultaneously, returning
		// REALIZABLE for direct contradictions like
		// `F(o1={top}) && G(o1!={top})`.  Falling through to
		// the default add_consistency_constraints + ABA-oracle
		// path catches these correctly.
		bool alg_a_can_classify_ok = false;
		TAU_TRY(alg_a_can_classify_ok,
			alg_a_can_classify<node>(fm, sol.atoms));
		if (!alg_a_can_classify_ok)
			LOG_DEBUG << "[ltl_aba] atom outside T_3 "
			             "(top/bot qlt constant?) — "
			             "skipping Algorithm A";

		// Algorithm A's T_3 encoding has a SINGLE current-output slot
		// (Y) and a SINGLE past-output slot (M).  Two distinct output
		// variables (o1, o2, …) get conflated into the same slot,
		// making every multi-output atom collapse to a single rational
		// witness.  Concretely, `o1[t]>0 && o2[t]<0` becomes "Y>0 &&
		// Y<0" — unsatisfiable in any T_3 type — so Algorithm A
		// returns spurious UNREALIZABLE.  Multi-output specs must
		// fall through to the default ABA-oracle path which builds
		// disjoint per-variable constraints.
		//
		// (Multi-input is fine: t3_role_of merges i_k → X but those
		// flow through Algorithm B's P_σ encoding which is
		// distinguisher-friendly.  The conflation is harmful only on
		// the OUTPUT side.)
		const size_t n_out_vars =
			count_distinct_output_vars<node>(sol.atoms);
		if (n_out_vars > 1) {
			alg_a_can_classify_ok = false;
			LOG_DEBUG << "[ltl_aba] multiple output vars ("
			          << n_out_vars
			          << ") — Algorithm A's single-Y/M slot would "
			             "conflate them; falling through to default "
			             "ABA-oracle path";
		}

		if (!any_input && alg_a_can_classify_ok) {
			// Pure-output: Algorithm A is sound and fast.
			LOG_DEBUG << "[ltl_aba] using Algorithm A (pure-output)";
			TAU_TRY(auto alg_a_syn,
				solve_ltl_aba_algorithm_a<node>(fm, sol.atoms));
			return r.with_value(std::move(alg_a_syn));
		}
		if (alg_a_mode)
			LOG_DEBUG << "[ltl_aba] algorithm A (ltl-alg) ignored because input variables are present";
		// Algorithm B is only sound when the same T_3-classification
		// holds: it shares the symbolic atom-mask with Algorithm A.
		// Atoms that don't classify (top/bot qlt constants etc.)
		// must fall through to the default add_consistency_constraints
		// path, which uses the ABA oracle directly and catches the
		// pairwise-infeasibility constraints those atoms induce.
		if (alg_b_mode && alg_a_can_classify_ok) {
			// Fast-path: constant-output strategy check. If the system
			// can pick fixed output values that reduce the formula to a
			// tautology over remaining (input) atoms, REALIZABLE.
			// Catches trivially-satisfiable U/W/R right-sides that
			// Algorithm B's large P_σ-encoded formula would make
			// ltlsynt time out on.
			TAU_TRY(auto win, constant_output_realizable<node>(
				fm, sol.atoms));
			if (win) {
				// Materialise the winning constant
				// combination as `always(⋀ o_k = c_k)` so the
				// strategy survives into execution and codegen.  Any
				// representative of the winning 1-type works —
				// atom truth only depends on the type — and
				// qlt_type1::realize() picks one (the constant
				// for a point type, the mediant / ±1 for an
				// interval).
				TAU_TRY(auto constants,
					omcat::collect_qlt_constants<node>(fm));
				ltl_aba_solution<node> trivial;
				trivial.atoms = sol.atoms;
				// Classify props so the codegen data emitter
				// puts the constant vars into Outputs (the
				// shared classification loop below is only
				// reached on the default path).
				for (auto& [f, name] : trivial.atoms) {
					if (is_pure_input_atom<node>(f))
						trivial.input_props
							.push_back(name);
					else
						trivial.output_props
							.push_back(name);
				}
				tref conj = nullptr;
				bool built_ok = true;
				for (const auto& [var, pos] : *win) {
					omcat::qlt_type1 t1;
					t1.pos = pos;
					t1.constants = constants;
					omcat::rational v = t1.realize();
					std::string lit = std::to_string(v.p)
						+ (v.q == 1 ? std::string()
						   : "/" + std::to_string(v.q));
					// Text-parse route: wff start symbol,
					// io classification resolved by name.
					std::string expr = var + "[t]:qlt = {"
						+ lit + "}:qlt";
					typename tree<node>::get_options opts;
					opts.parse.start = tree<node>::wff;
					// A failed parse stays inside the
					// fail-safe fallback below, so merge
					// the report without an early return.
					auto eq_opt = r.merge_take(tree<node>::get(
						expr, std::move(opts)));
					if (!eq_opt || !*eq_opt) {
						built_ok = false; break;
					}
					tref eq = *eq_opt;
					eq = resolve_io_vars<node>(
						*definitions<node>::instance()
							.get_io_context(), eq);
					conj = conj
						? tree<node>::build_wff_and(conj, eq)
						: eq;
					trivial.const_outputs.emplace_back(var, lit);
				}
				if (built_ok && conj) {
					trivial.const_formula =
						tree<node>::build_wff_always(conj);
					trivial.executable = true;
				} else {
					// Fail-safe: keep the sound verdict, but
					// the strategy is not executable.
					r.warning("[ltl_aba] constant-output "
						"witness could not be built; the "
						"strategy stays non-executable");
					trivial.const_outputs.clear();
					trivial.executable = false;
				}
				return r.with_value(synthesis_solved(trivial));
			}
			// Has input vars: Algorithm B required for soundness.
			LOG_DEBUG << "[ltl_aba] using Algorithm B (P_σ binary encoding)";
			TAU_TRY(auto alg_b_syn,
				solve_ltl_aba_algorithm_b<node>(fm, sol.atoms));
			return r.with_value(std::move(alg_b_syn));
		}
		if (!alg_a_can_classify_ok)
			LOG_DEBUG << "[ltl_aba] T_3 cannot classify atoms — "
			             "falling through to default ABA-oracle path";
	} else if (alg_b_mode) {
		LOG_DEBUG << "[ltl_aba] Alg B not applicable; using default path";
	}

	return r.with_value(synthesis_declined<node>());
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_LTL_SYNTHESIS_TMPL_H__
