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
// here. Gives are_tautologies(), the ltlfilt fast path below.
#include "backends/spot/spot.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "qlt"

namespace idni::tau_lang {

// Core helpers this file uses, declared in ltl_aba.h and
// tau_tree_extractors.tmpl.h and completed at instantiation.
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

// Used by the Algorithm D fast path to un-vacuate the ABA oracle over the
// automaton's own d_i-named atoms (ltl_aba_normalization.tmpl.h).
// Declared without the real definition's default arguments to avoid a
// "redefinition of default argument" diagnostic; the one call site here
// passes every argument explicitly.
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
// Restricted to the fragment is_algorithm_a_applicable accepts.

// Role of an io_var in the (m=memory, x=input, y=output) T_3 triple.
enum class t3_var_role { M, X, Y };

// The declared direction of the stream of @p io_var (the parent of an io_var
// node): 1 input, 2 output, 0 unknown.
template <NodeType node>
static size_t t3_direction_of(tref io_var) {
	return io_var_direction<node>(tree<node>::trim(io_var));
}

// The T_3 role of an io_var by declared direction and shift: o[t] is Y, i[t]
// is X, o[t-1] is M; nullopt for any other variable, a positional one
// included (its shift reads 0).
template <NodeType node>
static std::optional<t3_var_role> t3_role_of(tref io_var) {
	if (is_io_initial<node>(io_var)) return std::nullopt;
	const size_t dir = t3_direction_of<node>(io_var);
	int_t shift = get_io_var_shift<node>(io_var);
	if (dir == 2 && shift == 0) return t3_var_role::Y;
	if (dir == 1 && shift == 0) return t3_var_role::X;
	if (dir == 2 && shift == 1) return t3_var_role::M;
	return std::nullopt;
}

// True iff there is at least one atom, no more than the T3-encoding cap
// (qlt-t3-cap; warns when exceeded), all are qlt-typed, every io_var has a
// T_3 role (so none is positional, of unknown direction or an input read
// with lookback), the atoms name at most one input stream (T_3 has a
// single input slot X), and each comparison side has at most one io_var
// (no compound expressions like o1 & i1).
template <NodeType node>
static bool is_algorithm_a_applicable(
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	using tau = tree<node>;
	if (atoms.empty()) return false;
	// Bound K -- the A/B/D encodings compute 1 << K (signed-shift
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
	std::set<std::string> inputs;
	for (auto& [f, _] : atoms) {
		if (!ba_descriptor<qlt, node>::owns_type(find_ba_type<node>(f))) return false;
		for (tref v : tau::get(f).select_top(is_child<node, tau::io_var>)) {
			auto role = t3_role_of<node>(v);
			if (!role) return false;
			if (*role == t3_var_role::X)
				inputs.insert(get_var_name<node>(v));
		}
		if (inputs.size() > 1) return false;
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
// path can evaluate. @p constants are the formula's qlt constants, the cut
// points of T3.
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
		// Only the order of the two slots is known here, so each side
		// must be the bare variable: `o1 | {9}` is not o1.
		if (tau::trim(lhs) != lhs_io || tau::trim(rhs) != rhs_io)
			return r.with_value(atom_verdict::undecided);
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
// Both guards below gate the T_3 symbolic encoding, here and in the
// semantic PWR, which runs the same encoding (LS-2).

// Algorithm A's T_3 encoding only handles atoms whose truth value is decidable
// from a T_3 type plus the formula's named rational constants.  Atoms
// involving the qlt extremes `{top}:qlt` / `{bot}:qlt` — or any constant whose
// finite-rational witness is empty — yield `qlt_atom_holds_in_type3 ==
// atom_verdict::undecided` for every type, leaving the atom unconstrained
// in the symbolic encoding.  Without this guard ltlsynt happily synthesises a
// strategy where `α` and `¬α` both hold simultaneously, returning REALIZABLE
// for direct contradictions like `F(o1={top}) && G(o1!={top})`.
// Returns whether every atom is decided by at least one T_3 type (false when
// there is no T_3 type, or a 1-type has no long long witness); an error when
// the constants cannot be collected.
template <NodeType node>
static result<bool> alg_a_can_classify(
    tref fm, const std::vector<std::pair<tref, std::string>>& atoms)
{
	result<bool> r;
	TAU_TRY(auto a_constants, omcat::collect_qlt_constants<node>(fm));
	auto a_T3        = omcat::enumerate_qlt_T3(a_constants);
	if (a_T3.empty()) return r.with_value(false);
	// An atom is judged at a type's witness; a type without one would be
	// counted as holding every atom (qlt_type_A_bitmasks).
	for (const auto& t1 : omcat::enumerate_qlt_T1(a_constants))
		if (!t1.realize()) return r.with_value(false);
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
// any T_3 type — and the encoding returns a spurious verdict.  The single
// input slot X conflates inputs the same way; is_algorithm_a_applicable
// declines a second input stream.
// Returns the number of distinct output streams the atoms' sides name.
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
			if (t3_direction_of<node>(iv) == 2)
				names.insert(get_var_name<node>(iv));
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
		if (tau::trim(lhs) != lhs_io || tau::trim(rhs) != rhs_io)
			return r.with_value(atom_verdict::undecided);
		// The same T₁ position gets the same constant value; distinct
		// positions are ordered as their cells are.
		omcat::relation rel = *p1 < *p2 ? omcat::relation::LT
			: *p1 == *p2 ? omcat::relation::EQ : omcat::relation::GT;
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
// constant output choice, the first such combo in enumeration order. This
// fast-path avoids the expensive Algorithm B ltlsynt call.
template <NodeType node>
// LA-10: on success, returns the WINNING assignment (output stream name →
// T1 position of its constant value); the caller materialises it as
// `always(⋀ o_k = c_k)` so the strategy survives into execution and codegen.
// nullopt when no combination is proven (an ltlfilt failure only demotes
// that batch of candidates) or the formula has no output stream.
static result<std::optional<std::map<std::string, int>>> constant_output_realizable(
	tref fm,
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	using tau = tree<node>;
	result<std::optional<std::map<std::string, int>>> r;
	std::set<std::string> out_names;
	for (auto& [f, _] : atoms) {
		auto ios = tau::get(f).select_top(is_child<node, tau::io_var>);
		for (tref io_parent : ios)
			if (t3_direction_of<node>(io_parent) == 2)
				out_names.insert(get_var_name<node>(io_parent));
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
	const size_t const_output_max = qlt_const_output_max;
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

	// The candidates go to ltlfilt in batches, one process per batch; a
	// failed batch only leaves its candidates unproven.
	constexpr size_t batch_size = 64;
	std::vector<std::map<std::string, int>> batch_pos;
	std::vector<std::string> batch_phi;
	auto flush = [&]() -> std::optional<std::map<std::string, int>> {
		auto taut = are_tautologies(batch_phi, ltl_timeout_sec());
		std::optional<std::map<std::string, int>> win;
		if (taut.has_value()) {
			for (size_t k = 0; k < batch_phi.size(); ++k)
				if (taut.value()[k]) { win = batch_pos[k]; break; }
			r.merge(std::move(taut));
		} else {
			auto sc = r.open("rejected candidate");
			r.info("ltlfilt did not prove a constant output a "
				"tautology", {{label::value, truncate_for_message(
					batch_phi.front())}});
			report rep = std::move(taut).report();
			rep.demote_errors_to_warnings();
			r.append(std::move(rep));
		}
		batch_pos.clear();
		batch_phi.clear();
		return win;
	};
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
		batch_pos.push_back(std::move(var_pos));
		batch_phi.push_back(std::move(phi));
		if (batch_phi.size() < batch_size && combo + 1 < total) continue;
		if (auto win = flush()) {
			LOG_DEBUG << "Constant outputs reduce the formula to a "
				"tautology over the inputs: realizable";
			return r.with_value(std::move(win));
		}
	}
	return r.with_value(std::nullopt);
}

// Shared by Algorithms A, B and D and the semantic PWR
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

// Algorithm A (see the section comment above) on @p fm and its data atoms:
// declined when the constants admit no T_3 type, unrealizable when ltlsynt
// says so, else solved with the d_i atoms as output props.
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
	LOG_DEBUG << "Algorithm A: " << n_types << " T_3 types over "
		<< constants.size() << " qlt constants";
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

	auto bundle = alg_a::build_algorithm_a_skeleton(T1_size, K, feasible_set,
		phi_star, alg_d::initial_memory(constants));
	LOG_DEBUG << "Algorithm A skeleton for ltlsynt: " << bundle.formula;

	// R-bits + D-bits are system outputs; phi* uses D_i directly as propositions.
	std::vector<std::string> input_props;
	std::vector<std::string> output_props(bundle.outs.begin(), bundle.outs.end());

	TAU_TRY(auto ltlsynt_out, call_ltlsynt(bundle.formula, input_props, output_props));
	auto& [realizable, hoa_text] = ltlsynt_out;
	if (!realizable) return r.with_value(synthesis_unrealizable<node>());

	ltl_aba_solution<node> sol;
	sol.skeleton = bundle.formula;
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
// then pick the correct output type ρ). Declined when the constants admit no
// T_2 or T_3 type; a solved strategy is over the bookkeeping bits, so it is
// not executable.
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
	// PWR use.
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

	// Build phi* skeleton and rename p_i → d_i.
	TAU_TRY(auto phi_star_skel, ltl_skeleton<node>(fm, atoms));
	std::string phi_star = rename_skeleton_props_to_d(
		std::move(phi_star_skel), K);

	// No initial memory type is fixed, as A and D fix it: the P_sigma bits
	// of step 0 are left to the environment. solve_ltl_aba offers this path
	// no formula with a step guard, so no atom reads o[t-1] and the memory
	// part of sigma decides no atom.
	auto bundle = alg_b::build_algorithm_b_skeleton(
		T1_size, T2_size, K, feasible_set_b, t2_pos_m, phi_star);
	LOG_DEBUG << "Algorithm B skeleton over " << K << " atoms, |T_2| = "
		<< T2_size << ", |T_1| = " << T1_size << ", " << bundle.n_pbits
		<< " P_sigma bits and " << bundle.n_rbits << " R bits";

	TAU_TRY(auto ltlsynt_out, call_ltlsynt(bundle.formula, bundle.ins, bundle.outs));
	auto& [realizable, hoa_text] = ltlsynt_out;
	if (!realizable) return r.with_value(synthesis_unrealizable<node>());

	ltl_aba_solution<node> sol;
	sol.skeleton = bundle.formula;
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
 * The algorithm follows ltl_algorithm_choice() (`--ltl-alg`, TAU_LTL_ALG);
 * D falls through to the default routing when it does not apply.
 * @param fm The LTL(ABA) formula.
 * @param atoms Its data atoms with their proposition names.
 * @return Solved, unrealizable or declined; an error when a constant
 * collection, a skeleton, ltlsynt or the HOA parse fails.
 */
template <NodeType node>
static result<propositional_synthesis<node>> qlt_try_propositional_synthesis(
	tref fm, const std::vector<std::pair<tref, std::string>>& atoms)
{
	result<propositional_synthesis<node>> r;
	LOG_TRACE << "qlt_try_propositional_synthesis/fm: " << LOG_FM(fm);

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

		// Build φ*(D_i).
		TAU_TRY(auto phi_star_skel, ltl_skeleton<node>(fm, sol.atoms));
		std::string phi_star = rename_skeleton_props_to_d(
			std::move(phi_star_skel), K);

		LOG_DEBUG << "Algorithm D over " << K << " atoms, " << T3.size()
			<< " T_3 types, |T_1| = " << T1_size << ", skeleton phi*: "
			<< phi_star;

		// LG-12: fixed initial memory ρ₀ = type_of(0) — the
		// interpreter's own lookback-at-t=0 convention.
		TAU_TRY(auto realizable, alg_d::solve_algorithm_d(phi_star,
			T1_size, T3, type_A, K,
			alg_d::initial_memory(constants)));
		LOG_DEBUG << "Algorithm D verdict: "
			<< (realizable ? "realizable" : "unrealizable");

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
			LOG_DEBUG << "The ltlsynt strategy call disagrees with "
				"Algorithm D: falling through to Algorithm A";
			for (int i = 0; i < K; ++i)
				sol.atoms[(size_t) i].second = "p" + std::to_string(i);
		} else {
			sol.skeleton = strategy_skeleton;
			sol.output_props = D_outs;
			TAU_TRY(sol.aut, parse_hoa(hoa_text));
			return r.with_value(synthesis_solved(sol));
		}
	} else if (alg_d_mode) {
		LOG_DEBUG << "Algorithm D not applicable (input variables, non-qlt, "
			"large lookback, several outputs or an atom outside T_3): "
			"default routing";
	}

	// The choice comes from `--ltl-alg` / `set ltl-alg` / TAU_LTL_ALG;
	// ltl_algorithm_choice() (ltl_aba_limits.h) validates it and reports an
	// unrecognised value once, returning "" for the default routing.
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
			LOG_DEBUG << "An atom no T_3 type classifies (a top/bot "
				"qlt constant): Algorithms A and B skipped";

		// Algorithm A's T_3 encoding has a SINGLE current-output slot
		// (Y) and a SINGLE past-output slot (M).  Two distinct output
		// variables (o1, o2, …) get conflated into the same slot,
		// making every multi-output atom collapse to a single rational
		// witness.  Concretely, `o1[t]>0 && o2[t]<0` becomes "Y>0 &&
		// Y<0" — unsatisfiable in any T_3 type — so Algorithm A
		// returns spurious UNREALIZABLE.  Multi-output specs must
		// fall through to the default ABA-oracle path which builds
		// disjoint per-variable constraints.  The single input slot
		// X is guarded the same way, in is_algorithm_a_applicable.
		const size_t n_out_vars =
			count_distinct_output_vars<node>(sol.atoms);
		if (n_out_vars > 1) {
			alg_a_can_classify_ok = false;
			LOG_DEBUG << "Output variables sharing Algorithm A's single "
				"Y slot: " << n_out_vars << ", Algorithms A and B skipped";
		}

		if (!any_input && alg_a_can_classify_ok) {
			// Pure-output: Algorithm A is sound and fast.
			LOG_DEBUG << "No input variable: Algorithm A synthesises";
			TAU_TRY(auto alg_a_syn,
				solve_ltl_aba_algorithm_a<node>(fm, sol.atoms));
			return r.with_value(std::move(alg_a_syn));
		}
		if (alg_a_mode)
			LOG_DEBUG << "Algorithm A (ltl-alg) ignored: input "
				"variables are present";
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
					auto w = t1.realize();
					if (!w) { built_ok = false; break; }
					omcat::rational v = *w;
					std::string lit = std::to_string(v.p)
						+ (v.q == 1 ? std::string()
						   : "/" + std::to_string(v.q));
					// Text-parse route: wff start symbol,
					// io classification resolved by name.
					std::string expr = var + "[t]:qlt = {"
						+ lit + "}:qlt";
					typename tree<node>::get_options opts;
					opts.parse.start = tree<node>::wff;
					// A failed parse takes the fail-safe
					// fallback below: its report is a
					// rejected candidate, not an error.
					auto parsed = tree<node>::get(expr,
						std::move(opts));
					if (!parsed.has_value() || !parsed.value()) {
						auto sc = r.open("rejected candidate");
						r.info("the constant-output witness "
							"did not parse", {{label::value,
								truncate_for_message(expr)}});
						report rep = std::move(parsed).report();
						rep.demote_errors_to_warnings();
						r.append(std::move(rep));
						built_ok = false; break;
					}
					tref eq = parsed.value();
					r.merge(std::move(parsed));
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
					LOG_DEBUG << "Constant-output strategy: "
						<< LOG_FM(trivial.const_formula);
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
			// An Algorithm B run that gives no verdict (a timeout,
			// an ltlsynt failure) declines: the default path decides.
			LOG_DEBUG << "Input variables present: Algorithm B (P_sigma "
				"binary encoding) synthesises";
			auto alg_b_syn = solve_ltl_aba_algorithm_b<node>(fm, sol.atoms);
			if (!alg_b_syn.has_value()) {
				auto sc = r.open("rejected candidate");
				r.info("Algorithm B gave no verdict; the default "
					"path decides", {{label::value, truncate_for_message(
						tree<node>::get(fm).to_str())}});
				report rep = std::move(alg_b_syn).report();
				rep.demote_errors_to_warnings();
				r.append(std::move(rep));
				return r.with_value(synthesis_declined<node>());
			}
			TAU_TRY(auto solved, std::move(alg_b_syn));
			return r.with_value(std::move(solved));
		}
		if (!alg_a_can_classify_ok)
			LOG_DEBUG << "Atoms outside the T_3 encoding: the default "
				"ABA-oracle path decides";
	} else if (alg_b_mode) {
		LOG_DEBUG << "Algorithm B not applicable: the default ABA-oracle "
			"path decides";
	}

	return r.with_value(synthesis_declined<node>());
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_LTL_SYNTHESIS_TMPL_H__
