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
#include "ltl_aba_result.h"

namespace idni::tau_lang {

// Defined in the LTL template files, which follow this one.
std::pair<std::string, int> run_cmd(const std::string& cmd);

std::pair<bool, std::string> call_ltlsynt(const std::string& formula,
	const std::vector<std::string>& input_props,
	const std::vector<std::string>& output_props);

template <NodeType node>
std::vector<std::pair<tref, std::string>> extract_data_atoms(tref fm);

template <NodeType node>
std::string ltl_skeleton(tref fm,
	const std::vector<std::pair<tref, std::string>>& atoms);

template <NodeType node>
bool atom_has_any_input(tref atom);

// ── Algorithm A: binary T_3 type encoding ────────────────────────────────────
//
// TAU_LTL_ALG=A selects this path for single-stream pure-qlt formulas.
// Enumerates T_3 = (memory, input, output) order-types for (ℚ,<), binary-encodes
// them into ⌈log₂|T_3|⌉ system output Q-bits, and calls ltlsynt on the resulting
// propositional skeleton. No ABA oracle needed: the T_3 encoding is exact.
// Restricted to: all atoms qlt-type, single output var, lookback ≤ 1.

// Role of an io_var in the (m=memory, x=input, y=output) T_3 triple.
enum class T3VarRole { M, X, Y };

template <NodeType node>
static std::optional<T3VarRole> t3_role_of(tref io_var) {
	const std::string& nm = get_var_name<node>(io_var);
	if (nm.empty()) return std::nullopt;
	int_t shift = get_io_var_shift<node>(io_var);
	if (nm[0] == 'o' && shift == 0) return T3VarRole::Y;
	if (nm[0] == 'i' && shift == 0) return T3VarRole::X;
	if (nm[0] == 'o' && shift == 1) return T3VarRole::M;
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

// Returns true/false if the qlt comparison atom holds in T3, nullopt if unsupported.
template <NodeType node>
static std::optional<bool> qlt_atom_holds_in_type3(
	tref atom,
	const omcat::QltType3& T3,
	const std::vector<omcat::Rat>& constants)
{
	using tau = tree<node>;
	const auto& t = tau::get(atom);
	if (!t.has_child()) return std::nullopt;
	auto op = t[0].value.nt;
	if      (op == tau::bf_nlt)   op = tau::bf_gteq;
	else if (op == tau::bf_ngt)   op = tau::bf_lteq;
	else if (op == tau::bf_ngteq) op = tau::bf_lt;
	else if (op == tau::bf_nlteq) op = tau::bf_gt;
	if (op != tau::bf_lt  && op != tau::bf_lteq && op != tau::bf_gt &&
	    op != tau::bf_gteq && op != tau::bf_eq  && op != tau::bf_neq)
		return std::nullopt;

	tref lhs = t[0].first();
	tref rhs = t[0].second();
	// find_top returns the PARENT node (whose first child is io_var) because
	// get_var_name / get_io_var_shift expect that parent, not the io_var itself.
	tref lhs_io = tau::get(lhs).find_top([](tref n) {
		return is_child<node>(n, tau::io_var); });
	tref rhs_io = tau::get(rhs).find_top([](tref n) {
		return is_child<node>(n, tau::io_var); });

	auto flip_rel = [](omcat::Rel r) {
		return r == omcat::Rel::LT ? omcat::Rel::GT :
		       r == omcat::Rel::GT ? omcat::Rel::LT : omcat::Rel::EQ;
	};
	auto rel_holds = [&](omcat::Rel rel, size_t nt) -> bool {
		if (nt == tau::bf_lt)   return rel == omcat::Rel::LT;
		if (nt == tau::bf_lteq) return rel != omcat::Rel::GT;
		if (nt == tau::bf_gt)   return rel == omcat::Rel::GT;
		if (nt == tau::bf_gteq) return rel != omcat::Rel::LT;
		if (nt == tau::bf_eq)   return rel == omcat::Rel::EQ;
		if (nt == tau::bf_neq)  return rel != omcat::Rel::EQ;
		return false;
	};

	if (lhs_io && rhs_io) {
		auto rl = t3_role_of<node>(lhs_io);
		auto rr = t3_role_of<node>(rhs_io);
		if (!rl || !rr) return std::nullopt;
		omcat::Rel rel;
		auto p = std::make_pair(*rl, *rr);
		using R = T3VarRole;
		if      (p == std::make_pair(R::M, R::X)) rel = T3.rel_mx;
		else if (p == std::make_pair(R::X, R::M)) rel = flip_rel(T3.rel_mx);
		else if (p == std::make_pair(R::M, R::Y)) rel = T3.rel_my;
		else if (p == std::make_pair(R::Y, R::M)) rel = flip_rel(T3.rel_my);
		else if (p == std::make_pair(R::X, R::Y)) rel = T3.rel_xy;
		else if (p == std::make_pair(R::Y, R::X)) rel = flip_rel(T3.rel_xy);
		else return std::nullopt; // same role (e.g. y vs y)
		return rel_holds(rel, op);
	}

	if (lhs_io || rhs_io) {
		tref io_var = lhs_io ? lhs_io : rhs_io;
		tref const_side = lhs_io ? rhs : lhs;
		bool io_is_lhs = (lhs_io != nullptr);
		auto role = t3_role_of<node>(io_var);
		if (!role) return std::nullopt;
		omcat::QltType1 t1;
		if      (*role == T3VarRole::M) t1 = T3.restrict_m();
		else if (*role == T3VarRole::X) t1 = T3.restrict_x();
		else                            t1 = T3.restrict_y();
		auto cs = omcat::collect_qlt_constants<node>(const_side);
		if (cs.size() != 1) return std::nullopt;
		int j = -1;
		for (int k = 0; k < (int)constants.size(); ++k)
			if (omcat::cmp(constants[k], cs[0]) == 0) { j = k; break; }
		if (j < 0) return std::nullopt;
		auto eff = op;
		if (!io_is_lhs) {
			if      (eff == tau::bf_lt)   eff = tau::bf_gt;
			else if (eff == tau::bf_gt)   eff = tau::bf_lt;
			else if (eff == tau::bf_lteq) eff = tau::bf_gteq;
			else if (eff == tau::bf_gteq) eff = tau::bf_lteq;
		}
		if (eff == tau::bf_lt)   return t1.less_than(j);
		if (eff == tau::bf_lteq) return t1.less_than(j) || t1.equal_to(j);
		if (eff == tau::bf_gt)   return t1.greater_than(j);
		if (eff == tau::bf_gteq) return t1.greater_than(j) || t1.equal_to(j);
		if (eff == tau::bf_eq)   return t1.equal_to(j);
		if (eff == tau::bf_neq)  return !t1.equal_to(j);
		return std::nullopt;
	}
	return std::nullopt;
}

// Evaluate a pure-output atom under a per-variable T₁ assignment (constant-output
// strategy). Variable name keyed — unlike qlt_atom_holds_in_type3 which uses
// fixed M/X/Y roles and can't distinguish o1 from o2. Treats o_k[t-s] (any
// shift) as aliasing to o_k, since the strategy is constant over time.
// Returns nullopt if the atom involves an input variable or can't be evaluated.
template <NodeType node>
static std::optional<bool> eval_pure_output_atom_at(
	tref atom,
	const std::map<std::string, int>& var_pos,
	const std::vector<omcat::Rat>& constants)
{
	using tau = tree<node>;
	if (atom_has_any_input<node>(atom)) return std::nullopt;
	const auto& t = tau::get(atom);
	if (!t.has_child()) return std::nullopt;
	auto op = t[0].value.nt;
	if      (op == tau::bf_nlt)   op = tau::bf_gteq;
	else if (op == tau::bf_ngt)   op = tau::bf_lteq;
	else if (op == tau::bf_ngteq) op = tau::bf_lt;
	else if (op == tau::bf_nlteq) op = tau::bf_gt;
	if (op != tau::bf_lt  && op != tau::bf_lteq && op != tau::bf_gt &&
	    op != tau::bf_gteq && op != tau::bf_eq  && op != tau::bf_neq)
		return std::nullopt;

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

	auto rel_holds = [&](omcat::Rel rel, size_t o) -> bool {
		if (o == tau::bf_lt)   return rel == omcat::Rel::LT;
		if (o == tau::bf_lteq) return rel != omcat::Rel::GT;
		if (o == tau::bf_gt)   return rel == omcat::Rel::GT;
		if (o == tau::bf_gteq) return rel != omcat::Rel::LT;
		if (o == tau::bf_eq)   return rel == omcat::Rel::EQ;
		if (o == tau::bf_neq)  return rel != omcat::Rel::EQ;
		return false;
	};

	if (lhs_io && rhs_io) {
		auto p1 = lookup(lhs_io);
		auto p2 = lookup(rhs_io);
		if (!p1 || !p2) return std::nullopt;
		omcat::QltType1 t1a{*p1, constants};
		omcat::QltType1 t1b{*p2, constants};
		omcat::Rel rel;
		if (*p1 == *p2) {
			// Same T₁ position: constant strategy picks same value → EQ.
			rel = omcat::Rel::EQ;
		} else {
			omcat::Rat va = t1a.realize();
			omcat::Rat vb = t1b.realize();
			int c = omcat::cmp(va, vb);
			rel = c < 0 ? omcat::Rel::LT : (c == 0 ? omcat::Rel::EQ : omcat::Rel::GT);
		}
		return rel_holds(rel, op);
	}
	if (lhs_io || rhs_io) {
		tref io = lhs_io ? lhs_io : rhs_io;
		bool io_is_lhs = (lhs_io != nullptr);
		auto p = lookup(io);
		if (!p) return std::nullopt;
		omcat::QltType1 t1{*p, constants};
		auto cs = omcat::collect_qlt_constants<node>(io_is_lhs ? rhs : lhs);
		// Range constant handling: {[a,b]} collects two endpoints.
		// Interpret `io_var = {[a,b]}` as closed-interval membership,
		// `!=` as non-membership.
		if (cs.size() == 2 && (op == tau::bf_eq || op == tau::bf_neq)) {
			omcat::Rat a = cs[0], b = cs[1];
			if (omcat::cmp(a, b) > 0) std::swap(a, b);
			int ja = -1, jb = -1;
			for (int k = 0; k < (int)constants.size(); ++k) {
				if (omcat::cmp(constants[k], a) == 0) ja = k;
				if (omcat::cmp(constants[k], b) == 0) jb = k;
			}
			if (ja < 0 || jb < 0) return std::nullopt;
			// Membership: pos_y ∈ [2*ja+1, 2*jb+1] (point-at-a through point-at-b).
			long lo_pos = 2L * ja + 1;
			long hi_pos = 2L * jb + 1;
			long pp = static_cast<long>(*p);
			#pragma GCC diagnostic push
			#pragma GCC diagnostic ignored "-Wstrict-overflow"
			bool in_range = (pp >= lo_pos && pp <= hi_pos);
			#pragma GCC diagnostic pop
			return (op == tau::bf_eq) ? in_range : !in_range;
		}
		if (cs.size() != 1) return std::nullopt;
		int j = -1;
		for (int k = 0; k < (int)constants.size(); ++k)
			if (omcat::cmp(constants[k], cs[0]) == 0) { j = k; break; }
		if (j < 0) return std::nullopt;
		auto eff = op;
		if (!io_is_lhs) {
			if      (eff == tau::bf_lt)   eff = tau::bf_gt;
			else if (eff == tau::bf_gt)   eff = tau::bf_lt;
			else if (eff == tau::bf_lteq) eff = tau::bf_gteq;
			else if (eff == tau::bf_gteq) eff = tau::bf_lteq;
		}
		if (eff == tau::bf_lt)   return t1.less_than(j);
		if (eff == tau::bf_lteq) return t1.less_than(j) || t1.equal_to(j);
		if (eff == tau::bf_gt)   return t1.greater_than(j);
		if (eff == tau::bf_gteq) return t1.greater_than(j) || t1.equal_to(j);
		if (eff == tau::bf_eq)   return t1.equal_to(j);
		if (eff == tau::bf_neq)  return !t1.equal_to(j);
	}
	return std::nullopt;
}

// Pre-check: is the formula REALIZABLE via a constant-output strategy?
// Enumerates T₁ positions per output variable; for each combo, substitutes
// pure-output atom truth values into the LTL skeleton and uses `ltlfilt` to
// simplify. If any combo reduces to "1" (true), the system wins with that
// constant output choice. This fast-path avoids the expensive Algorithm B
// ltlsynt call for formulas with trivially-satisfiable U/W/R right-sides.
template <NodeType node>
static bool constant_output_realizable(
	tref fm,
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	using tau = tree<node>;
	std::set<std::string> out_names;
	for (auto& [f, _] : atoms) {
		auto ios = tau::get(f).select_top(is_child<node, tau::io_var>);
		for (tref io_parent : ios) {
			const std::string& nm = get_var_name<node>(io_parent);
			if (!nm.empty() && nm[0] == 'o') out_names.insert(nm);
		}
	}
	if (out_names.empty()) return false;

	auto constants = omcat::collect_qlt_constants<node>(fm);
	int T1_size = 2 * (int)constants.size() + 1;
	if (T1_size <= 0) return false;

	std::vector<std::string> out_vec(out_names.begin(), out_names.end());
	int n_out = (int)out_vec.size();
	unsigned long long total_u = 1;
	const unsigned long long CAP = 100ULL;
	for (int i = 0; i < n_out; ++i) {
		if (total_u > CAP) return false;
		total_u *= (unsigned long long)T1_size;
		if (total_u > CAP) return false;
	}
	long long total = (long long)total_u;

	std::string phi_star_base = ltl_skeleton<node>(fm, atoms);

	for (long long combo = 0; combo < total; ++combo) {
		std::map<std::string, int> var_pos;
		unsigned long long rem = (unsigned long long)combo;
		for (int i = 0; i < n_out; ++i) {
			var_pos[out_vec[i]] = (int)(rem % (unsigned long long)T1_size);
			rem /= (unsigned long long)T1_size;
		}

		std::string phi = phi_star_base;
		for (int i = (int)atoms.size(); i-- > 0; ) {
			auto val = eval_pure_output_atom_at<node>(
				atoms[i].first, var_pos, constants);
			if (!val) continue;
			std::string fp = "p" + std::to_string(i);
			std::string rep = *val ? "true" : "false";
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

		// Shell-escape for single-quoted arg.
		std::string escaped;
		for (char c : phi) {
			if (c == '\'') escaped += "'\\''";
			else escaped += c;
		}
		std::string cmd = "ltlfilt -f '" + escaped + "' 2>/dev/null";
		auto [out, rc] = run_cmd(cmd);
		while (!out.empty() && std::isspace((unsigned char)out.back())) out.pop_back();
		if (rc == 0 && out == "1") {
			LOG_DEBUG << "[ltl_aba] constant-output fast-path REALIZABLE "
			          << "(combo=" << combo << ")";
			return true;
		}
	}
	return false;
}

template <NodeType node>
static std::optional<LtlAbaSolution<node>>
solve_ltl_aba_algorithm_a(
	tref fm,
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	auto constants = omcat::collect_qlt_constants<node>(fm);
	auto T3 = omcat::enumerate_qlt_T3(constants);
	int n_types = (int)T3.size();
	LOG_DEBUG << "[ltl_aba:algA] T3 count=" << n_types
	          << " constants=" << constants.size();
	if (n_types == 0) return std::nullopt;

	int K = (int)atoms.size();
	// atom_mask[i] = T₃ type indices where D_i holds (true or undetermined).
	std::vector<std::vector<int>> atom_mask(K);
	for (int i = 0; i < K; ++i)
		for (int t = 0; t < n_types; ++t) {
			auto h = qlt_atom_holds_in_type3<node>(atoms[i].first, T3[t], constants);
			if (h != false) atom_mask[i].push_back(t); // true or undetermined: include
		}

	// Build per-T₃-type D-bitmask, then extract feasible (sigma, rho, A) triples.
	std::vector<int> type_A(n_types, 0);
	for (int i = 0; i < K; ++i)
		for (int t : atom_mask[i])
			type_A[t] |= (1 << i);

	int T1_size = 2 * (int)constants.size() + 1;
	std::vector<std::tuple<int,int,int>> feasible_set;
	feasible_set.reserve(n_types);
	for (int t = 0; t < n_types; ++t)
		feasible_set.emplace_back(T3[t].pos_m, T3[t].pos_y, type_A[t]);

	// Build phi* skeleton and rename p_i → D_i (highest index first).
	std::string phi_star = ltl_skeleton<node>(fm, atoms);
	for (int i = K; i-- > 0; ) {
		std::string fp = "p" + std::to_string(i);
		std::string td = "d_" + std::to_string(i);
		size_t pos = 0;
		while ((pos = phi_star.find(fp, pos)) != std::string::npos) {
			size_t end = pos + fp.size();
			bool l_ok = pos == 0 || (!std::isalnum((unsigned char)phi_star[pos-1])
			                         && phi_star[pos-1] != '_');
			bool r_ok = end >= phi_star.size()
			         || (!std::isalnum((unsigned char)phi_star[end])
			             && phi_star[end] != '_');
			if (l_ok && r_ok) { phi_star.replace(pos, fp.size(), td); pos += td.size(); }
			else pos = end;
		}
	}

	auto bundle = alg_a::build_algorithm_a_skeleton(T1_size, K, feasible_set, phi_star);
	LOG_DEBUG << "[ltl_aba:algA] skeleton: " << bundle.formula;

	// R-bits + D-bits are system outputs; phi* uses D_i directly as propositions.
	std::vector<std::string> input_props;
	std::vector<std::string> output_props(bundle.outs.begin(), bundle.outs.end());

	auto [realizable, hoa_text] = call_ltlsynt(bundle.formula, input_props, output_props);
	if (!realizable) return std::nullopt;

	LtlAbaSolution<node> sol;
	// Populate sol.atoms with the d_i propositions phi_star uses, so
	// downstream consumers (the codegen witness emitter in
	// cpp_codegen.tmpl.h, the safety-formula extractor in
	// ltl_to_safety_formula) can map AP names back to the original data
	// atoms and emit qlt witnesses, executable safety formulas, etc.
	// Previously sol.atoms was left empty, which forced the codegen down
	// the propositional fallback that emits `bool o_d_0` instead of
	// `double o1` — failing test_cpp_codegen_data_atoms's structural
	// checks.
	//
	// Also populate sol.output_props with the d_i names so
	// emit_cpp_program_data's Outputs-struct emission iterates over the
	// data-atom propositions and fills in `double <var>` fields.
	sol.atoms.reserve(atoms.size());
	sol.output_props.reserve(atoms.size());
	for (size_t i = 0; i < atoms.size(); ++i) {
		std::string name = "d_" + std::to_string(i);
		sol.atoms.emplace_back(atoms[i].first, name);
		sol.output_props.push_back(name);
	}
	sol.aut = parse_hoa(hoa_text);
	return sol;
}

// Algorithm B: P_σ binary encoding — adds ⌈log₂|T₂|⌉ input propositions for
// the T₂ = (pos_m, pos_x, rel_mx) type.  Needed for SOUNDNESS when the formula
// contains input-variable atoms (the system observes x's type via P-bits and can
// then pick the correct output type ρ).
template <NodeType node>
static std::optional<LtlAbaSolution<node>>
solve_ltl_aba_algorithm_b(
	tref fm,
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	auto constants = omcat::collect_qlt_constants<node>(fm);
	auto T2 = omcat::enumerate_qlt_T2(constants);
	auto T3 = omcat::enumerate_qlt_T3(constants);
	int T2_size = (int)T2.size();
	int n_types = (int)T3.size();
	if (n_types == 0 || T2_size == 0) return std::nullopt;

	int K       = (int)atoms.size();
	int T1_size = 2 * (int)constants.size() + 1;

	// Compute D-bitmask per T₃ type.
	std::vector<int> type_A(n_types, 0);
	for (int i = 0; i < K; ++i)
		for (int t = 0; t < n_types; ++t) {
			auto h = qlt_atom_holds_in_type3<node>(atoms[i].first, T3[t], constants);
			if (h != false) type_A[t] |= (1 << i);
		}

	// Build T₂ lookup: (pos_m, pos_x, rel_mx) → T₂ index.
	std::map<std::tuple<int,int,int>, int> t2_lookup;
	for (int s = 0; s < T2_size; ++s)
		t2_lookup[{ T2[s].pos_m, T2[s].pos_x, (int)T2[s].rel }] = s;

	// Build feasible_set_b: (T2_idx, rho, A).
	std::vector<std::tuple<int,int,int>> feasible_set_b;
	feasible_set_b.reserve(n_types);
	for (int t = 0; t < n_types; ++t) {
		auto key = std::make_tuple(T3[t].pos_m, T3[t].pos_x, (int)T3[t].rel_mx);
		auto it  = t2_lookup.find(key);
		if (it == t2_lookup.end()) continue;
		feasible_set_b.emplace_back(it->second, T3[t].pos_y, type_A[t]);
	}

	// t2_pos_m[σ] = pos_m of T₂[σ].
	std::vector<int> t2_pos_m(T2_size);
	for (int s = 0; s < T2_size; ++s) t2_pos_m[s] = T2[s].pos_m;

	// Build phi* skeleton and rename p_i → d_i.
	std::string phi_star = ltl_skeleton<node>(fm, atoms);
	for (int i = K; i-- > 0; ) {
		std::string fp = "p" + std::to_string(i);
		std::string td = "d_" + std::to_string(i);
		size_t pos = 0;
		while ((pos = phi_star.find(fp, pos)) != std::string::npos) {
			size_t end = pos + fp.size();
			bool l_ok = pos == 0 || (!std::isalnum((unsigned char)phi_star[pos-1])
			                         && phi_star[pos-1] != '_');
			bool r_ok = end >= phi_star.size()
			         || (!std::isalnum((unsigned char)phi_star[end])
			             && phi_star[end] != '_');
			if (l_ok && r_ok) { phi_star.replace(pos, fp.size(), td); pos += td.size(); }
			else pos = end;
		}
	}

	auto bundle = alg_b::build_algorithm_b_skeleton(
		T1_size, T2_size, K, feasible_set_b, t2_pos_m, phi_star);
	LOG_DEBUG << "[ltl_aba:algB] T2=" << T2_size << " T1=" << T1_size
	          << " K=" << K << " n_pbits=" << bundle.n_pbits
	          << " n_rbits=" << bundle.n_rbits;

	auto [realizable, hoa_text] = call_ltlsynt(bundle.formula, bundle.ins, bundle.outs);
	if (!realizable) return std::nullopt;

	LtlAbaSolution<node> sol;
	sol.aut = parse_hoa(hoa_text);
	return sol;
}

/**
 * @brief Synthesise @p fm propositionally, when qlt's order types can encode it.
 *
 * Declining and proving unrealizable are different answers: the first lets core
 * fall through to the ABA oracle, the second is final. Algorithms A, B and D all
 * produce both, so the result carries the distinction rather than collapsing it.
 */
template <NodeType node>
static propositional_synthesis<node> qlt_try_propositional_synthesis(tref fm,
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	LtlAbaSolution<node> sol;
	sol.atoms = atoms;

	const bool alg_d_mode = [] {
		const char* v = std::getenv("TAU_LTL_ALG");
		return v && std::string_view(v) == "D";
	}();
	bool alg_d_has_input = false;
	for (auto& [f, _] : sol.atoms)
		if (atom_has_any_input<node>(f)) { alg_d_has_input = true; break; }
	if (alg_d_mode && !alg_d_has_input
	    && is_algorithm_a_applicable<node>(sol.atoms)) {
		auto constants = omcat::collect_qlt_constants<node>(fm);
		auto T3 = omcat::enumerate_qlt_T3(constants);
		int K = (int)sol.atoms.size();
		int T1_size = 2 * (int)constants.size() + 1;

		// Compute D-bitmask for each T3 type
		std::vector<int> type_A(T3.size(), 0);
		for (int i = 0; i < K; ++i) {
			for (int t = 0; t < (int)T3.size(); ++t) {
				auto h = qlt_atom_holds_in_type3<node>(sol.atoms[i].first, T3[t], constants);
				if (h != false) type_A[t] |= (1 << i);
			}
		}

		// Build φ*(D_i)
		std::string phi_star = ltl_skeleton<node>(fm, sol.atoms);
		for (int i = K; i-- > 0; ) {
			std::string fp = "p" + std::to_string(i);
			std::string td = "d_" + std::to_string(i);
			size_t pos = 0;
			while ((pos = phi_star.find(fp, pos)) != std::string::npos) {
				size_t end = pos + fp.size();
				bool l_ok = pos == 0 || (!std::isalnum((unsigned char)phi_star[pos-1]) && phi_star[pos-1] != '_');
				bool r_ok = end >= phi_star.size() || (!std::isalnum((unsigned char)phi_star[end]) && phi_star[end] != '_');
				if (l_ok && r_ok) { phi_star.replace(pos, fp.size(), td); pos += td.size(); }
				else pos = end;
			}
		}

		LOG_DEBUG << "[ltl_aba:algD] T3=" << T3.size() << " T1=" << T1_size
		          << " K=" << K << " phi_star=" << phi_star;

		bool realizable = alg_d::solve_algorithm_d(phi_star, T1_size, T3, type_A, K);
		LOG_DEBUG << "[ltl_aba:algD] result=" << (realizable ? "REALIZABLE" : "UNREALIZABLE");

		if (!realizable) return synthesis_unrealizable<node>();

		// Realizable: call ltlsynt for the strategy automaton.
		// Use the simplified propositional formula; data execution may need
		// the full structural-constraint formula for perfect correctness but
		// this gives a usable strategy for most test cases.
		std::vector<std::string> D_outs;
		for (int i = 0; i < K; ++i) D_outs.push_back("d_" + std::to_string(i));
		auto [real2, hoa_text] = call_ltlsynt(phi_star, {}, D_outs);
		if (!real2) {
			// Propositional call disagrees — fall through to default path
			LOG_DEBUG << "[ltl_aba:algD] ltlsynt disagreed; falling through";
		} else {
			sol.aut = parse_hoa(hoa_text);
			return synthesis_solved(sol);
		}
	} else if (alg_d_mode) {
		LOG_DEBUG << "[ltl_aba:algD] not applicable (input variables, non-qlt, or large lookback);"
		             " falling through to default";
	}

	const char* alg_env = std::getenv("TAU_LTL_ALG");
	const bool alg_b_mode = !alg_env || std::string_view(alg_env) == "B";
	const bool alg_a_mode = alg_env && std::string_view(alg_env) == "A";
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
		// nullopt for every type, leaving the atom completely
		// unconstrained in the symbolic encoding.  Without this
		// guard, ltlsynt happily synthesises a strategy where
		// `α` and `¬α` both hold simultaneously, returning
		// REALIZABLE for direct contradictions like
		// `F(o1={top}) && G(o1!={top})`.  Falling through to
		// the default add_consistency_constraints + ABA-oracle
		// path catches these correctly.
		bool alg_a_can_classify = true;
		{
			auto a_constants = omcat::collect_qlt_constants<node>(fm);
			auto a_T3        = omcat::enumerate_qlt_T3(a_constants);
			if (a_T3.empty())
				alg_a_can_classify = false;
			else for (auto& [f, _] : sol.atoms) {
				bool any_determined = false;
				for (auto& t : a_T3) {
					auto h = qlt_atom_holds_in_type3<node>(
					    f, t, a_constants);
					if (h.has_value()) {
						any_determined = true;
						break;
					}
				}
				if (!any_determined) {
					alg_a_can_classify = false;
					LOG_DEBUG << "[ltl_aba] atom outside T_3 "
					             "(top/bot qlt constant?) — "
					             "skipping Algorithm A";
					break;
				}
			}
		}

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
		std::set<std::string> distinct_output_names;
		for (auto& [f, _] : sol.atoms) {
			const auto& t = tree<node>::get(f);
			if (!t.has_child()) continue;
			auto add_side = [&](tref side) {
				if (!side) return;
				tref iv = tree<node>::get(side).find_top([](tref n) {
					return is_child<node>(n, tree<node>::io_var); });
				if (!iv) return;
				const std::string& nm = get_var_name<node>(iv);
				if (!nm.empty() && nm[0] == 'o')
					distinct_output_names.insert(nm);
			};
			add_side(t[0].first());
			add_side(t[0].second());
		}
		if (distinct_output_names.size() > 1) {
			alg_a_can_classify = false;
			LOG_DEBUG << "[ltl_aba] multiple output vars ("
			          << distinct_output_names.size()
			          << ") — Algorithm A's single-Y/M slot would "
			             "conflate them; falling through to default "
			             "ABA-oracle path";
		}

		if (!any_input && alg_a_can_classify) {
			// Pure-output: Algorithm A is sound and fast.
			LOG_DEBUG << "[ltl_aba] using Algorithm A (pure-output)";
			return propositional_synthesis<node>{
				solve_ltl_aba_algorithm_a<node>(fm, sol.atoms)};
		}
		if (alg_a_mode)
			LOG_DEBUG << "[ltl_aba] TAU_LTL_ALG=A ignored because input variables are present";
		// Algorithm B is only sound when the same T_3-classification
		// holds: it shares the symbolic atom-mask with Algorithm A.
		// Atoms that don't classify (top/bot qlt constants etc.)
		// must fall through to the default add_consistency_constraints
		// path, which uses the ABA oracle directly and catches the
		// pairwise-infeasibility constraints those atoms induce.
		if (alg_b_mode && alg_a_can_classify) {
			// Fast-path: constant-output strategy check. If the system
			// can pick fixed output values that reduce the formula to a
			// tautology over remaining (input) atoms, REALIZABLE.
			// Catches trivially-satisfiable U/W/R right-sides that
			// Algorithm B's large P_σ-encoded formula would make
			// ltlsynt time out on.
			if (constant_output_realizable<node>(fm, sol.atoms)) {
				LtlAbaSolution<node> trivial;
				trivial.atoms = sol.atoms;
				// num_states = 0 signals trivially realizable.
				return synthesis_solved(trivial);
			}
			// Has input vars: Algorithm B required for soundness.
			LOG_DEBUG << "[ltl_aba] using Algorithm B (P_σ binary encoding)";
			return propositional_synthesis<node>{
				solve_ltl_aba_algorithm_b<node>(fm, sol.atoms)};
		}
		if (!alg_a_can_classify)
			LOG_DEBUG << "[ltl_aba] T_3 cannot classify atoms — "
			             "falling through to default ABA-oracle path";
	} else if (alg_b_mode) {
		LOG_DEBUG << "[ltl_aba] Alg B not applicable; using default path";
	}

	return synthesis_declined<node>();
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__QLT__QLT_LTL_SYNTHESIS_TMPL_H__
