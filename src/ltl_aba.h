// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// LTL(ABA) Realizability — extends tau-lang's safety fragment to full LTL.
//
// Operators: F (finally/eventually), U (until), R (release), W (weak until).
//   G (globally) reuses wff_always — the existing safety pipeline handles it.
//   X (neXt) and Y (Yesterday) are subsumed by io_var time indices and are
//   therefore not needed as explicit grammar operators.
//
// Algorithm (oracle-assisted synthesis, see book.tex §6.7):
//   1. Extract data atoms (maximal temporal-operator-free subformulas).
//   2. Build the propositional LTL skeleton over abstract propositions p0,p1,...
//   3. Classify each proposition as input (only i* io_vars) or output (has o* io_var).
//   4. Call Spot's ltlsynt for propositional realizability + strategy extraction.
//   5. For each strategy automaton transition, verify ABA feasibility via tau-lang's
//      existing ABA engine (the oracle).
//   6. Return REALIZABLE iff there is an ABA-feasible accepting strategy.

#ifndef __IDNI__TAU__LTL_ABA_H__
#define __IDNI__TAU__LTL_ABA_H__

#include "normalizer.h"
#include "boolean_algebras/nlang_ba.h"
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

namespace idni::tau_lang {

// ── Detection ────────────────────────────────────────────────────────────────

// True iff the formula contains any full-LTL operator: wff_F, wff_U, wff_R,
// wff_W, and the past operators wff_S, wff_T (past formulas must route to
// the LTL pipeline's temporal testers).  wff_always (G) is already handled
// by the safety pipeline.
template <NodeType node>
bool has_ltl_operators(tref fm);

// ── Data-atom extraction ──────────────────────────────────────────────────────

// A "data atom" is a maximal subtree that contains no temporal operators
// (wff_always, wff_F, wff_U, wff_R, wff_W) but does contain at least one
// io_var.  Each distinct atom is assigned a fresh proposition name "p0","p1"...
//
// Returns a vector of {tref, proposition_name} in discovery order.
// The map from tref → name is built using structural equality (subtree_equals).
template <NodeType node>
std::vector<std::pair<tref, std::string>> extract_data_atoms(tref fm);

// ── Propositional LTL skeleton ────────────────────────────────────────────────

// Produce a Spot-parseable LTL formula string by replacing every data atom
// with its assigned proposition name.  Non-atom Boolean subformulas are kept.
template <NodeType node>
std::string ltl_skeleton(tref fm,
    const std::vector<std::pair<tref, std::string>>& atoms);

// ── DFA temporal testers (ppLTLTT approach) ──────────────────────────────────

// Past-operator S/T → DFA temporal tester following Azzopardi et al. (ATVA'23).
// Each S/T subformula is replaced by a fresh propositional state variable
// in the skeleton.  The DFA transition + initial condition are encoded as
// LTL constraints (G, X, propositional) that ltlsynt handles natively.
struct PastTemporalTester {
	std::string state_var;
	/// Informational only (LT-15): always false ((φ S ψ)(−1) = false);
	/// the encoding hard-codes !state_var at t=0. Kept for the
	/// explain/debug output.
	bool        initial_value;
	std::string transition;
	/// Informational only (LT-15): the negation is already inlined in the
	/// expression returned by skeleton_str_with_testers; this flag merely
	/// annotates the explain/debug output for T-testers.
	bool        negate_output;
};

// Build skeleton with temporal testers.  Returns {skeleton, testers}.
// Caller must call append_tester_constraints() and add state vars to outputs.
template <NodeType node>
std::pair<std::string, std::vector<PastTemporalTester>>
ltl_skeleton_with_testers(
    tref fm,
    const std::vector<std::pair<tref, std::string>>& atoms);

// Append DFA tester constraints to the skeleton string.
void append_tester_constraints(
    std::string& skeleton,
    const std::vector<PastTemporalTester>& testers);

// ── Input / output classification ────────────────────────────────────────────

// True iff every io_var appearing in the atom is an INPUT variable.
// Primarily reads the resolved direction bit (data 1=input / 2=output);
// only unresolved io_vars fall back to the name prefix ('o' = output).
// Note the resolver additionally classifies `this` as input and `u` as
// output (see tau_tree_extractors.tmpl.h); the prefix fallback here does
// not replicate that.
template <NodeType node>
bool is_pure_input_atom(tref atom);

// ── Spot interface ────────────────────────────────────────────────────────────

// Raised when a Spot subprocess could not deliver a verdict (LT-7).  A failed
// or killed ltlsynt is NOT an UNREALIZABLE answer, and reporting it as one
// turns a slow-but-realizable specification into a silently wrong result.
struct ltl_synthesis_error : std::runtime_error {
	using std::runtime_error::runtime_error;
};

// How to read a (exit_code, stdout) pair from a Spot subprocess.
//
//   ok         a verdict was produced (REALIZABLE / UNREALIZABLE).
//   not_found  the binary is not on PATH — spawn_capture maps ENOENT to 127.
//   failed     no verdict: the TAU_LTL_TIMEOUT_SEC watchdog killed the child
//              (exit 128 + SIGTERM = 143), the spawn itself failed (-1), the
//              tool reported a usage/internal error, or it exited non-zero
//              with nothing on stdout.
enum class spot_exit_kind { ok, not_found, failed };

inline spot_exit_kind classify_spot_exit(int exit_code, const std::string& out) {
	if (exit_code == 127) return spot_exit_kind::not_found;
	// Negative: spawn_capture could not create the pipe or the process.
	// >= 128: killed by signal 128 + N — in particular 143 = SIGTERM, which
	// is exactly what the timeout watchdog sends.
	if (exit_code < 0 || exit_code >= 128) return spot_exit_kind::failed;
	// ltlsynt exits 0 on REALIZABLE and 1 on UNREALIZABLE; every other code
	// is a usage or internal error.
	if (exit_code != 0 && exit_code != 1) return spot_exit_kind::failed;
	// A non-zero exit with no output at all carries no verdict either.
	if (exit_code != 0 && out.empty()) return spot_exit_kind::failed;
	return spot_exit_kind::ok;
}

// Invoke ltlsynt as a subprocess and return {realizable, hoa_strategy_text}.
// hoa_strategy_text is non-empty only when realizable == true.
//
// Throws ltl_synthesis_error when the subprocess produced no verdict (see
// classify_spot_exit).  A missing binary keeps the historical behaviour of
// logging an error and returning {false, ""} so that environments without
// Spot degrade rather than abort.
std::pair<bool, std::string> call_ltlsynt(
    const std::string& ltl_formula,
    const std::vector<std::string>& input_props,
    const std::vector<std::string>& output_props);

// ── HOA automaton ─────────────────────────────────────────────────────────────

struct HoaEdge {
    std::string guard_label;  // Boolean formula over AP indices, e.g. "0&!1"
    int dst = 0;
    bool accepting = false;   // true if this edge carries an acceptance mark
};

struct HoaAutomaton {
    int num_states = 0;
    int initial_state = 0;
    std::vector<std::string> aps;       // atomic proposition names
    std::vector<std::vector<HoaEdge>> edges; // edges[src] = list of outgoing edges
    std::vector<bool> state_accepting;  // true if state has acceptance mark
};

HoaAutomaton parse_hoa(const std::string& hoa_text);

// ── DPA (Deterministic Parity Automaton) — Algorithm D Phase 1 ───────────────
//
// A DPA edge carries a single parity color (0..num_colors-1).
// color == -1 means the edge has no acceptance mark in the HOA; in min-even
// parity this is treated as "worst priority" (never contributes to acceptance).
struct DpaEdge {
    std::string guard_label;
    int dst    = 0;
    int color  = -1;  // parity color; -1 = unmarked
};

// Deterministic Parity Automaton parsed from Spot's HOA output.
struct DpaAutomaton {
    int num_states    = 0;
    int initial_state = 0;
    int num_colors    = 0;  // total number of colors in the parity condition
    bool min_even     = true; // true = min-even parity (Spot default for -D)
    std::vector<std::string>            aps;   // atomic proposition names
    std::vector<std::vector<DpaEdge>>   edges; // edges[src] = outgoing edges
};

// Call ltl2tgba with parity='min even' -D --complete on the given LTL formula.
// Returns the raw HOA text, or empty string on error.
std::string call_ltl2tgba_dpa(const std::string& ltl_formula);

// Parse the HOA output of ltl2tgba (with parity acceptance) into a DpaAutomaton.
DpaAutomaton parse_dpa_hoa(const std::string& hoa_text);

// ── ABA oracle ────────────────────────────────────────────────────────────────

// Given a HOA transition guard label and the proposition→data_atom mapping,
// build the ABA conjunction of instantiated data atoms / their negations.
// Returns T if guard is trivially true (no propositions), F if trivially false.
template <NodeType node>
tref guard_to_aba(const std::string& guard_label,
    const std::vector<std::string>& aps,
    const std::vector<std::pair<tref, std::string>>& atoms);

// ── CTL* → LTL reduction (Bloem/Schewe/Khalimov, arXiv:1711.10636) ──────────
//
// The reduction works bottom-up on state subformulas:
//   - E χ introduces a fresh witness output variable w and adds G(w → χ_LTL)
//   - A χ is rewritten to ¬E¬χ first
// The result is a pure LTL formula over I ∪ O ∪ W (witnesses).

// State subformula context for CTL* reduction
struct CtlStarWitness {
    tref original;       // the E χ subformula
    std::string name;    // fresh witness output name, e.g. "w_0"
    tref path_formula;   // the inner path formula χ
};

// Reduce a CTL* formula containing A/E quantifiers to an equivalent LTL
// synthesis problem. Returns the reduced formula (pure LTL) and the set of
// LT-23: the witness variables are SELF-CLASSIFYING -- they are built with
// node::output_variable() (direction bit = output), so extract_data_atoms /
// is_pure_input_atom already treat them as outputs and the only caller
// (is_tau_formula_sat) rightly uses ltl_formula alone. Do NOT additionally
// add `witnesses` to an output set; the vector is informational. Note the
// names (`w_<n>`) have no `o` prefix, so classification rests entirely on
// the direction bit.
template <NodeType node>
struct CtlStarReduction {
    tref ltl_formula;                    // reduced LTL formula
    std::vector<std::string> witnesses;  // witness output variable names
};

template <NodeType node>
CtlStarReduction<node> reduce_ctl_star_to_ltl(tref fm);

// Check if a formula contains CTL* path quantifiers (A or E)
template <NodeType node>
bool has_ctl_star_operators(tref fm);

// ── Semantic negation ─────────────────────────────────────────────────────────
//
// Semantic negation -φ means "φ is unrealizable by the system", i.e. the
// environment can force ¬φ.  Deciding it means swapping the input/output roles
// for the subformula and checking realizability of ¬φ.
//
// THAT ROLE SWAP IS NOT IMPLEMENTED (LT-5).  `apply_semantic_negation` only
// re-wraps its argument in a `wff_semantic_neg` node; the header used to claim
// the swap happened "at the synthesis call site", but no such site exists.
// The surviving node then reached `skeleton_wff`'s default case which, because
// the subformula contains io_vars, emitted the propositional constant "1" — so
// every non-constant `-φ` was silently decided REALIZABLE regardless of φ.
//
// Until the swap exists, a `wff_semantic_neg` that survives constant folding
// is REJECTED with `ltl_synthesis_error` rather than answered wrongly.  The
// hook-level folding of `-T` → F and `-F` → T is unaffected and keeps working.

// True iff the formula contains a `wff_semantic_neg` node.
template <NodeType node>
bool has_semantic_negation(tref fm);

template <NodeType node>
tref apply_semantic_negation(tref fm);

// ── Explain pipeline ─────────────────────────────────────────────────────────

// Print a human-readable trace of the LTL(ABA) translation pipeline.
// Returns true if realizable.
template <NodeType node>
bool ltl_explain(tref fm, std::ostream& out);

// ── Main entry point ──────────────────────────────────────────────────────────

// Check whether `fm` is LTL(ABA)-realizable.
//
// `fm` must already be normalised (i.e. have passed through the standard
// tau-lang normaliser).  `start_time` and `output` follow the same convention
// as `is_tau_formula_sat`.
template <NodeType node>
bool is_ltl_aba_realizable(tref fm, int_t start_time, bool output);

// Strategy-automaton solution returned by `solve_ltl_aba` and (via
// `ltl_to_safety_formula_full`) cached on the interpreter for runtime
// introspection of the Mealy strategy.
//
// Forward-declared here; the full struct definition lives in
// `ltl_aba_normalization.tmpl.h` (per upstream PR #90 god-file split).
// Forward-decl is enough for `std::optional<LtlAbaSolution<node>>` in the
// signature below — the type only needs to be complete at instantiation
// sites (interpreter.impl.h, cpp_codegen.tmpl.h), which include the tmpl
// chain that defines it.
template <NodeType node>
struct LtlAbaSolution;

// Convert a realizable LTL formula to a tau-lang safety formula (always(phi))
// that the existing interpreter pipeline can execute.
//
// Strategy:
//   - Pure past-LTL formulas: compiled away to safety + init invariants
//     (no Mealy synthesis needed; `_full` returns nullopt for the solution).
//   - Single-state Mealy strategies: extract the self-loop output constraint,
//     wrap in always().
//   - Multi-state Mealy strategies: encoded as a safety formula with auxiliary
//     one-hot state bits `o__ltl_ms<i>__` per `encode_mealy_as_safety`.
//
// Returns nullptr if not realizable.
template <NodeType node>
tref ltl_to_safety_formula(tref fm);

// Variant that ALSO returns the LtlAbaSolution<node> (the strategy automaton +
// atoms map) when one was synthesised. The interpreter caches it so that
// downstream code can introspect the Mealy state at runtime, visualise the
// strategy, extract boundary traces, etc. — the strategy's information is
// otherwise discarded after the safety encoding.
//
// The optional is empty when:
//   - the formula was unrealizable (returned nullptr),
//   - the formula was pure past-LTL (no synthesis needed).
//
// On the multi-state encoding path, the caller can read sol.aut.num_states,
// sol.aut.edges, sol.atoms etc. without re-running synthesis.
template <NodeType node>
std::tuple<tref, std::optional<LtlAbaSolution<node>>>
ltl_to_safety_formula_full(tref fm);

} // namespace idni::tau_lang

#include "ltl_aba.tmpl.h"

#endif // __IDNI__TAU__LTL_ABA_H__
