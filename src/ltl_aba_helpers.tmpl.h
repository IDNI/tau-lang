// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// ltl_aba_helpers.tmpl.h - Helpers, formula analysis, skeleton, io classification
// Split from ltl_aba.tmpl.h for readability.

namespace idni::tau_lang {

// ── helpers ───────────────────────────────────────────────────────────────────

// True if `nt` is one of the full-LTL operators we add (not G/always).
// G is aliased to wff_always and handled by the existing safety pipeline.
static inline bool is_full_ltl_nt(size_t nt) {
	return nt == tau_parser::wff_F
	    || nt == tau_parser::wff_U
	    || nt == tau_parser::wff_R
	    || nt == tau_parser::wff_W
	    || nt == tau_parser::wff_S
	    || nt == tau_parser::wff_T;
}

// True if `n` is ANY temporal operator (including safety-fragment ones).
template <NodeType node>
static bool is_temporal_op(tref n) {
	using tau = tree<node>;
	const auto& t = tau::get(n);
	if (!t.has_child()) return false;
	auto nt = t[0].value.nt;
	return nt == tau::wff_always
	    || nt == tau::wff_sometimes
	    || nt == tau::wff_F
	    || nt == tau::wff_U
	    || nt == tau::wff_R
	    || nt == tau::wff_W
	    || nt == tau::wff_S
	    || nt == tau::wff_T
	    || nt == tau::wff_A
	    || nt == tau::wff_E;
}

// Maximum temporal nesting depth of a formula (G/F/U/R/W/S count as one level each).
// Used to guard against stack-overflow or runaway synthesis on deeply nested formulas.
template <NodeType node>
static int ltl_nesting_depth(tref n, int depth = 0) {
	using tau = tree<node>;
	const auto& t = tau::get(n);
	if (!t.has_child()) return depth;
	int this_depth = is_temporal_op<node>(n) ? depth + 1 : depth;
	const auto& op = t[0];
	int max_child = this_depth;
	for (size_t i = 0; i < op.children_size(); ++i)
		max_child = std::max(max_child,
		                     ltl_nesting_depth<node>(op.child(i), this_depth));
	return max_child;
}

// True if `n` is one of the ABA comparison predicates (bf_eq, bf_neq, …)
template <NodeType node>
static bool is_aba_comparison(tref n) {
	using tau = tree<node>;
	const auto& t = tau::get(n);
	if (!t.has_child()) return false;
	auto nt = t[0].value.nt;
	return nt == tau::bf_eq  || nt == tau::bf_neq
	    || nt == tau::bf_lt  || nt == tau::bf_nlt
	    || nt == tau::bf_lteq|| nt == tau::bf_nlteq
	    || nt == tau::bf_gt  || nt == tau::bf_ngt
	    || nt == tau::bf_gteq|| nt == tau::bf_ngteq
	    || nt == tau::bf_interval;
}

// True if the formula subtree has at least one io_var node.
template <NodeType node>
static bool has_io_var(tref n) {
	using tau = tree<node>;
	return tau::get(n).find_top(is<node, tau::io_var>) != nullptr;
}

// True if the formula contains any past-LTL operator (S or T).
template <NodeType node>
static bool has_past_operators(tref fm) {
	using tau = tree<node>;
	return tau::get(fm).find_top([](tref n) {
		const auto& t = tree<node>::get(n);
		if (!t.has_child()) return false;
		auto nt = t[0].value.nt;
		return nt == tau::wff_S || nt == tau::wff_T;
	}) != nullptr;
}

// ── has_ltl_operators ─────────────────────────────────────────────────────────

template <NodeType node>
bool has_ltl_operators(tref fm) {
	using tau = tree<node>;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(fm); it != cache.end()) return it->second;
#endif // TAU_CACHE
	bool result = tau::get(fm).find_top([](tref n) {
		const auto& t = tree<node>::get(n);
		if (!t.has_child()) return false;
		return is_full_ltl_nt(t[0].value.nt);
	}) != nullptr;
#ifdef TAU_CACHE
	cache.emplace(fm, result);
#endif // TAU_CACHE
	return result;
}

// ── extract_data_atoms ────────────────────────────────────────────────────────

// Recursive helper: walk the AST.  Any ABA comparison that contains an io_var
// is recorded as a data atom, whether or not it is inside a temporal operator.
// The `inside_temporal` flag is kept for future extensibility but is no longer
// used to gate atom extraction — every io_var-containing comparison is a data
// atom regardless of depth.  This is required so that atoms introduced by the
// S/T compile-away pass (which appear at the top level of the conjunction) are
// correctly represented in the propositional LTL skeleton.
template <NodeType node>
static void collect_atoms(
    tref n,
    std::vector<std::pair<tref, std::string>>& atoms,
    bool inside_temporal)
{
	using tau = tree<node>;
	const auto& t = tau::get(n);
	if (!t.has_child()) return; // terminal node

	// All tref-level nodes are wff wrappers: t[0] is the actual operator.
	const auto& op = t[0];

	// If this is a temporal operator, mark children as "inside temporal".
	if (is_temporal_op<node>(n)) {
		for (size_t i = 0; i < op.children_size(); ++i)
			collect_atoms<node>(op.child(i), atoms, true);
		return;
	}

	// If this is an ABA comparison with io_vars, record it as a data atom.
	// (Previously gated on inside_temporal, but that excluded top-level atoms
	// produced by S/T compile-away.)
	if (is_aba_comparison<node>(n) && has_io_var<node>(n)) {
		for (auto& [existing, _] : atoms)
			if (tau::subtree_equals(existing, n)) return; // deduplicate
		std::string name = "p" + std::to_string(atoms.size());
		atoms.emplace_back(n, std::move(name));
		return;
	}

	// Otherwise recurse: walk operator's children.
	for (size_t i = 0; i < op.children_size(); ++i)
		collect_atoms<node>(op.child(i), atoms, inside_temporal);
}

template <NodeType node>
std::vector<std::pair<tref, std::string>> extract_data_atoms(tref fm) {
	std::vector<std::pair<tref, std::string>> atoms;
	collect_atoms<node>(fm, atoms, false);
	return atoms;
}

// ── ltl_skeleton ──────────────────────────────────────────────────────────────

// Find the proposition name for a given formula node, or "" if not a data atom.
template <NodeType node>
static std::string find_prop(
    tref n,
    const std::vector<std::pair<tref, std::string>>& atoms)
{
	using tau = tree<node>;
	for (auto& [f, name] : atoms)
		if (tau::subtree_equals(f, n)) return name;
	return "";
}

template <NodeType node>
static std::string skeleton_str(
    tref n,
    const std::vector<std::pair<tref, std::string>>& atoms);

template <NodeType node>
static std::string skeleton_wff(
    tref n,
    const std::vector<std::pair<tref, std::string>>& atoms)
{
	using tau = tree<node>;
	const auto& t = tau::get(n);
	if (!t.has_child()) return "t"; // terminal → treat as true
	auto nt = t[0].value.nt;
	const auto& inner = t[0];

	// Check if this is a data atom.
	auto prop = find_prop<node>(n, atoms);
	if (!prop.empty()) return prop;

	switch (nt) {
	case tau::wff_t: return "1";
	case tau::wff_f: return "0";
	case tau::wff_neg:
		return "!" + skeleton_str<node>(inner.first(), atoms);
	case tau::wff_and:
		return "(" + skeleton_str<node>(inner.first(), atoms)
		     + " & " + skeleton_str<node>(inner.second(), atoms) + ")";
	case tau::wff_or:
		return "(" + skeleton_str<node>(inner.first(), atoms)
		     + " | " + skeleton_str<node>(inner.second(), atoms) + ")";
	case tau::wff_xor:
		return "(" + skeleton_str<node>(inner.first(), atoms)
		     + " ^ " + skeleton_str<node>(inner.second(), atoms) + ")";
	case tau::wff_imply:
		return "(" + skeleton_str<node>(inner.first(), atoms)
		     + " -> " + skeleton_str<node>(inner.second(), atoms) + ")";
	case tau::wff_equiv:
		return "(" + skeleton_str<node>(inner.first(), atoms)
		     + " <-> " + skeleton_str<node>(inner.second(), atoms) + ")";
	case tau::wff_always:
		// G (globally) is wff_always — handled by existing safety pipeline
		// but we also emit it in the skeleton for ltlsynt.
		return "G(" + skeleton_str<node>(inner.first(), atoms) + ")";
	case tau::wff_sometimes:
	case tau::wff_F:
		return "F(" + skeleton_str<node>(inner.first(), atoms) + ")";
	case tau::wff_U:
		return "(" + skeleton_str<node>(inner.first(), atoms)
		     + " U " + skeleton_str<node>(inner.second(), atoms) + ")";
	case tau::wff_R:
		return "(" + skeleton_str<node>(inner.first(), atoms)
		     + " R " + skeleton_str<node>(inner.second(), atoms) + ")";
	case tau::wff_W:
		return "(" + skeleton_str<node>(inner.first(), atoms)
		     + " W " + skeleton_str<node>(inner.second(), atoms) + ")";
	case tau::wff_S:
		return "(" + skeleton_str<node>(inner.first(), atoms)
		     + " S " + skeleton_str<node>(inner.second(), atoms) + ")";
	case tau::wff_T:
		return "(" + skeleton_str<node>(inner.first(), atoms)
		     + " T " + skeleton_str<node>(inner.second(), atoms) + ")";
	case tau::wff_rimply:
		return "(" + skeleton_str<node>(inner.second(), atoms)
		     + " -> " + skeleton_str<node>(inner.first(), atoms) + ")";
	case tau::wff_conditional:
		// phi ? psi : chi = (phi -> psi) && (!phi -> chi)
		return "((" + skeleton_str<node>(inner.first(), atoms)
		     + " -> " + skeleton_str<node>(inner.second(), atoms)
		     + ") & (!" + skeleton_str<node>(inner.first(), atoms)
		     + " -> " + skeleton_str<node>(inner.third(), atoms) + "))";
	default: {
		// Unknown node or ABA comparison without io_vars.
		// If it has io_vars, it should have been extracted as a data atom.
		if (has_io_var<node>(n)) {
			auto p2 = find_prop<node>(n, atoms);
			return p2.empty() ? "1" : p2;
		}
		// No io_var: evaluate statically — returns "1" (true) or "0" (false).
		tref normalized = normalize_non_temp<node>(n);
		if (tree<node>::get(normalized).equals_F()) return "0";
		return "1";
	}
	}
}

template <NodeType node>
static std::string skeleton_str(
    tref n,
    const std::vector<std::pair<tref, std::string>>& atoms)
{
	using tau = tree<node>;
	const auto& t = tau::get(n);
	// wff wrapper
	if (t.is(tau::wff)) return skeleton_wff<node>(n, atoms);
	// bf/atom — treat as data prop
	auto prop = find_prop<node>(n, atoms);
	return prop.empty() ? "t" : prop;
}

template <NodeType node>
std::string ltl_skeleton(
    tref fm,
    const std::vector<std::pair<tref, std::string>>& atoms)
{
	return skeleton_str<node>(fm, atoms);
}

// ── DFA temporal tester for pure-past LTL (ppLTLTT approach) ──────────────────
//
// Pure-past LTL operators (S, T) are not supported by ltlsynt.  Instead of
// emitting them literally, we encode them as DFA temporal testers following
// the ppLTLTT approach (Azzopardi et al., ATVA'23).
//
// Each S/T subformula is replaced by a fresh propositional state variable
// in the skeleton, and the DFA transition + initial condition are added as
// LTL constraints that ltlsynt handles natively (using only X, G, and
// propositional connectives).
//
// For φ S ψ ("φ Since ψ"):
//   Recurrence: (φ S ψ)(t) = ψ(t) ∨ (φ(t) ∧ (φ S ψ)(t−1))
//   Initial:    (φ S ψ)(−1) = false
//   DFA state:  __past_s{k}  tracks whether φ S ψ currently holds
//   Encoding:   !__past_s{k} && G(X(__past_s{k}) <-> (ψ_skel | (φ_skel & __past_s{k})))
//
// For φ T ψ ("φ Trigger ψ", dual of S):
//   φ T ψ = ¬(¬φ S ¬ψ)
//   DFA state:  __past_t{k}  tracks whether ¬φ S ¬ψ currently holds
//   Encoding:   !__past_t{k} && G(X(__past_t{k}) <-> (!ψ_skel | (!φ_skel & __past_t{k})))
//   Output:     !__past_t{k}  (negation because T = ¬S(¬,¬))

// PastTemporalTester is declared in ltl_aba.h.

// Skeleton generation with temporal tester collection.
// When encountering S/T nodes, creates a DFA tester entry and returns
// the state variable name (or its negation for T) instead of the
// unsupported S/T operator string.
template <NodeType node>
static std::string skeleton_wff_with_testers(
    tref n,
    const std::vector<std::pair<tref, std::string>>& atoms,
    std::vector<PastTemporalTester>& testers);

template <NodeType node>
static std::string skeleton_str_with_testers(
    tref n,
    const std::vector<std::pair<tref, std::string>>& atoms,
    std::vector<PastTemporalTester>& testers)
{
	using tau = tree<node>;
	const auto& t = tau::get(n);
	if (t.is(tau::wff)) return skeleton_wff_with_testers<node>(n, atoms, testers);
	auto prop = find_prop<node>(n, atoms);
	return prop.empty() ? "t" : prop;
}

template <NodeType node>
static std::string skeleton_wff_with_testers(
    tref n,
    const std::vector<std::pair<tref, std::string>>& atoms,
    std::vector<PastTemporalTester>& testers)
{
	using tau = tree<node>;
	const auto& t = tau::get(n);
	if (!t.has_child()) return "t";
	auto nt = t[0].value.nt;
	const auto& inner = t[0];

	auto prop = find_prop<node>(n, atoms);
	if (!prop.empty()) return prop;

	switch (nt) {
	case tau::wff_t: return "1";
	case tau::wff_f: return "0";
	case tau::wff_neg:
		return "!" + skeleton_str_with_testers<node>(inner.first(), atoms, testers);
	case tau::wff_and:
		return "(" + skeleton_str_with_testers<node>(inner.first(), atoms, testers)
		     + " & " + skeleton_str_with_testers<node>(inner.second(), atoms, testers) + ")";
	case tau::wff_or:
		return "(" + skeleton_str_with_testers<node>(inner.first(), atoms, testers)
		     + " | " + skeleton_str_with_testers<node>(inner.second(), atoms, testers) + ")";
	case tau::wff_xor:
		return "(" + skeleton_str_with_testers<node>(inner.first(), atoms, testers)
		     + " ^ " + skeleton_str_with_testers<node>(inner.second(), atoms, testers) + ")";
	case tau::wff_imply:
		return "(" + skeleton_str_with_testers<node>(inner.first(), atoms, testers)
		     + " -> " + skeleton_str_with_testers<node>(inner.second(), atoms, testers) + ")";
	case tau::wff_equiv:
		return "(" + skeleton_str_with_testers<node>(inner.first(), atoms, testers)
		     + " <-> " + skeleton_str_with_testers<node>(inner.second(), atoms, testers) + ")";
	case tau::wff_always:
		return "G(" + skeleton_str_with_testers<node>(inner.first(), atoms, testers) + ")";
	case tau::wff_sometimes:
	case tau::wff_F:
		return "F(" + skeleton_str_with_testers<node>(inner.first(), atoms, testers) + ")";
	case tau::wff_U:
		return "(" + skeleton_str_with_testers<node>(inner.first(), atoms, testers)
		     + " U " + skeleton_str_with_testers<node>(inner.second(), atoms, testers) + ")";
	case tau::wff_R:
		return "(" + skeleton_str_with_testers<node>(inner.first(), atoms, testers)
		     + " R " + skeleton_str_with_testers<node>(inner.second(), atoms, testers) + ")";
	case tau::wff_W:
		return "(" + skeleton_str_with_testers<node>(inner.first(), atoms, testers)
		     + " W " + skeleton_str_with_testers<node>(inner.second(), atoms, testers) + ")";

	// ── ppLTLTT temporal testers for past operators ──────────────────
	//
	// The state variable tracks the S/T recurrence from the PREVIOUS
	// step: __past_s{k}(t) holds iff (φ S ψ)(t−1) held.  The
	// replacement expression evaluates the recurrence at the CURRENT
	// step using the state variable.
	//
	//   (φ S ψ)(t) = ψ(t) ∨ (φ(t) ∧ (φ S ψ)(t−1))
	//             = ψ(t) ∨ (φ(t) ∧ __past_s{k}(t))
	//
	// The G(X(s) <-> ...) constraint updates the state variable for
	// the next step.  !s at t=0 encodes (φ S ψ)(−1) = false.
	case tau::wff_S: {
		std::string phi_skel = skeleton_str_with_testers<node>(inner.first(), atoms, testers);
		std::string psi_skel = skeleton_str_with_testers<node>(inner.second(), atoms, testers);
		std::string svar = "__past_s" + std::to_string(testers.size());
		// Transition: next-step state = current evaluation of the S recurrence
		std::string eval = "(" + psi_skel + " | (" + phi_skel + " & " + svar + "))";
		testers.push_back({svar, false, eval, false});
		// Return the CURRENT-step evaluation (not the state variable)
		return eval;
	}
	case tau::wff_T: {
		// φ T ψ = ¬(¬φ S ¬ψ)
		std::string phi_skel = skeleton_str_with_testers<node>(inner.first(), atoms, testers);
		std::string psi_skel = skeleton_str_with_testers<node>(inner.second(), atoms, testers);
		std::string svar = "__past_t" + std::to_string(testers.size());
		// State tracks ¬φ S ¬ψ from the previous step
		std::string neg_eval = "(!" + psi_skel + " | (!" + phi_skel + " & " + svar + "))";
		testers.push_back({svar, false, neg_eval, true});
		// T = ¬S(¬,¬): negate the current evaluation of the inner S
		return "(!" + neg_eval + ")";
	}

	case tau::wff_rimply:
		return "(" + skeleton_str_with_testers<node>(inner.second(), atoms, testers)
		     + " -> " + skeleton_str_with_testers<node>(inner.first(), atoms, testers) + ")";
	case tau::wff_conditional: {
		std::string cond = skeleton_str_with_testers<node>(inner.first(), atoms, testers);
		return "((" + cond
		     + " -> " + skeleton_str_with_testers<node>(inner.second(), atoms, testers)
		     + ") & (!" + cond
		     + " -> " + skeleton_str_with_testers<node>(inner.third(), atoms, testers) + "))";
	}
	default: {
		if (has_io_var<node>(n)) {
			auto p2 = find_prop<node>(n, atoms);
			return p2.empty() ? "1" : p2;
		}
		tref normalized = normalize_non_temp<node>(n);
		if (tree<node>::get(normalized).equals_F()) return "0";
		return "1";
	}
	}
}

// Build the LTL skeleton with temporal testers for past operators.
// Returns the skeleton string and the list of testers.
// The caller must:
//   1. Append the tester constraints to the skeleton
//   2. Add the tester state variables to the output props
template <NodeType node>
std::pair<std::string, std::vector<PastTemporalTester>>
ltl_skeleton_with_testers(
    tref fm,
    const std::vector<std::pair<tref, std::string>>& atoms)
{
	std::vector<PastTemporalTester> testers;
	std::string skel = skeleton_str_with_testers<node>(fm, atoms, testers);
	return {std::move(skel), std::move(testers)};
}

// Append temporal tester DFA constraints to a skeleton string.
// For each tester:  && !state_var && G(X(state_var) <-> transition)
inline void append_tester_constraints(
    std::string& skeleton,
    const std::vector<PastTemporalTester>& testers)
{
	for (const auto& t : testers) {
		// Initial condition: state_var starts false (strong past semantics)
		skeleton += " & !" + t.state_var;
		// DFA transition
		skeleton += " & G(X(" + t.state_var + ") <-> " + t.transition + ")";
	}
}

// ── input / output classification ─────────────────────────────────────────────

template <NodeType node>
bool is_pure_input_atom(tref atom) {
	using tau = tree<node>;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(atom); it != cache.end()) return it->second;
#endif // TAU_CACHE
	// Look for any output io_var.
	// After resolve_io_vars: data==1=input, data==2=output.
	// For unresolved io_vars, fall back to name prefix ('o'=output).
	auto has_out = tau::get(atom).find_top([](tref n) {
		const auto& t = tree<node>::get(n);
		if (!t.is(tau::io_var)) return false;
		size_t dir = t.value.data;
		if (dir == 2) return true;   // resolved as output
		if (dir == 1) return false;  // resolved as input
		// Unresolved: infer from variable name prefix
		const std::string& nm = get_var_name<node>(n);
		return !nm.empty() && nm[0] == 'o';
	});
	bool result = has_out == nullptr; // no output io_var → pure input
#ifdef TAU_CACHE
	cache.emplace(atom, result);
#endif // TAU_CACHE
	return result;
}

// True if the formula contains any io_var that is an input variable.
// Symmetric to is_pure_input_atom (which checks for absence of output vars).
template <NodeType node>
bool atom_has_any_input(tref atom) {
	using tau = tree<node>;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(atom); it != cache.end()) return it->second;
#endif // TAU_CACHE
	auto has_in = tau::get(atom).find_top([](tref n) {
		const auto& t = tree<node>::get(n);
		if (!t.is(tau::io_var)) return false;
		size_t dir = t.value.data;
		if (dir == 1) return true;   // resolved as input
		if (dir == 2) return false;  // resolved as output
		const std::string& nm = get_var_name<node>(n);
		return !nm.empty() && nm[0] == 'i';
	});
	bool result = has_in != nullptr;
#ifdef TAU_CACHE
	cache.emplace(atom, result);
#endif // TAU_CACHE
	return result;
}


} // namespace idni::tau_lang
