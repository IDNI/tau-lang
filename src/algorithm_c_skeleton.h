// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Algorithm C (oracle-assisted abstract game) skeleton builder.
//
// Given:
//   - |T_1|               number of memory 1-types
//   - K = |Δ|             number of data subformulas (D-propositions)
//   - phi_star_ltl        LTL skeleton over D_1..D_K (replace each δ_i with D_i)
//
// Build the LTL skeleton φ̂_ora = Φ_A → (Φ_R ∧ middle ∧ φ*) in Spot/ltlsynt
// syntax for feeding to the propositional synthesiser.  See
// LTL_ABA_DESIGN.md / Algorithm C for the semantics.

#ifndef __IDNI__TAU__ALGORITHM_C_SKELETON_H__
#define __IDNI__TAU__ALGORITHM_C_SKELETON_H__

#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

namespace idni::tau_lang::alg_c {

// Proposition naming conventions (consistent with existing skeleton emitter).
inline std::string A_name(int rho, uint32_t J) {
	return "A_" + std::to_string(rho) + "_" + std::to_string(J);
}
inline std::string R_name(int rho) { return "R_" + std::to_string(rho); }
inline std::string D_name(int i)   { return "D_" + std::to_string(i); }

struct SkeletonBundle {
	std::string formula;             // the skeleton itself (Spot syntax)
	std::vector<std::string> ins;    // env-controlled propositions
	std::vector<std::string> outs;   // system-controlled propositions
};

// Build the Algorithm C skeleton.
//
// phi_star_ltl is expected to be a ready propositional-LTL string over
// D_0, D_1, ..., D_{K-1} in Spot syntax.  The emitter wraps it with the
// Φ_A → (Φ_R ∧ middle ∧ φ*) scaffolding.
//
// Returns the complete skeleton plus the input/output proposition lists
// for ltlsynt's --ins / --outs arguments.
inline SkeletonBundle build_algorithm_c_skeleton(
    int T1_size,
    int K,
    const std::string& phi_star_ltl)
{
	SkeletonBundle b;
	const uint32_t total_J = (K >= 31) ? 0 : (1u << K);  // 0 treated as "all"

	// Enumerate A_{ρ,J} (env inputs) and R_ρ, D_i (system outputs).
	for (int rho = 0; rho < T1_size; ++rho) {
		for (uint32_t J = 0; J < total_J; ++J)
			b.ins.push_back(A_name(rho, J));
		b.outs.push_back(R_name(rho));
	}
	for (int i = 0; i < K; ++i) b.outs.push_back(D_name(i));

	// Φ_R: exactly one R_ρ holds at every step.
	// With |T_1| fresh Boolean outputs we emit the "at least one" + pairwise
	// exclusion.  (A binary encoding would be smaller — follow-up.)
	std::ostringstream phi_R;
	phi_R << "G(";
	{
		std::string or_chain;
		for (int rho = 0; rho < T1_size; ++rho) {
			if (!or_chain.empty()) or_chain += " | ";
			or_chain += R_name(rho);
		}
		phi_R << "(" << or_chain << ")";
	}
	for (int i = 0; i < T1_size; ++i)
		for (int j = i + 1; j < T1_size; ++j)
			phi_R << " & !(" << R_name(i) << " & " << R_name(j) << ")";
	phi_R << ")";

	// Middle: G(⋀_{ρ,J} (R_ρ ∧ ⋀_i ¬(D_i ⊕ j_i)) → A_{ρ,J}).
	// For each (ρ, J) the implication constrains that if the system selected
	// R_ρ and D-pattern matching J, then the env's A_{ρ,J} must hold (i.e.,
	// the system's declared move was indeed achievable).
	std::ostringstream middle;
	middle << "G(";
	bool first_clause = true;
	for (int rho = 0; rho < T1_size; ++rho) {
		for (uint32_t J = 0; J < total_J; ++J) {
			if (!first_clause) middle << " & ";
			first_clause = false;
			middle << "(";
			middle << "(" << R_name(rho);
			for (int i = 0; i < K; ++i) {
				bool in_J = (J >> i) & 1;
				middle << " & " << (in_J ? "" : "!") << D_name(i);
			}
			middle << ")";
			middle << " -> " << A_name(rho, J);
			middle << ")";
		}
	}
	middle << ")";

	// Φ_A: env assumption that A-propositions are consistent with the
	// previous step's R-memory.  For simplicity (and correctness at first
	// rendering) we currently emit `true` — the runtime oracle is what
	// guarantees consistency; ltlsynt merely needs to produce a strategy
	// that wins against all A-sequences.  A tighter Φ_A belongs in
	// follow-up work once the runtime oracle is wired.
	std::string phi_A = "true";

	std::ostringstream out;
	out << "(" << phi_A << ") -> ("
	    << phi_R.str() << " & "
	    << middle.str() << " & "
	    << "(" << phi_star_ltl << ")"
	    << ")";
	b.formula = out.str();
	return b;
}

} // namespace idni::tau_lang::alg_c

#endif // __IDNI__TAU__ALGORITHM_C_SKELETON_H__
