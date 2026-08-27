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

// True iff every io_var of `atom` sits at a constant absolute time position
// (as opposed to a relative index like o[t] or o[t-1]).
template <NodeType node>
static bool atom_is_positional(tref atom) {
	using tau = tree<node>;
	auto io_vars = tau::get(atom).select_top(is_child<node, tau::io_var>);
	if (io_vars.empty()) return false;
	for (tref v : io_vars) if (!is_io_initial<node>(v)) return false;
	return true;
}

// Highest absolute position among a positional atom's io_vars.
template <NodeType node>
static int_t atom_max_position(tref atom) {
	using tau = tree<node>;
	auto io_vars = tau::get(atom).select_top(is_child<node, tau::io_var>);
	return get_max_initial<node>(io_vars);
}

// The set of io_var names referenced by a (non-positional) atom.
template <NodeType node>
static std::set<std::string> atom_io_var_names(tref atom) {
	using tau = tree<node>;
	auto io_vars = tau::get(atom).select_top(is_child<node, tau::io_var>);
	std::set<std::string> names;
	for (tref v : io_vars) names.insert(get_var_name<node>(v));
	return names;
}

// The relative-time shift shared by every io_var of `atom`, or nullopt when
// the atom has no io_var, has a positional (absolute) io_var, or mixes
// io_vars at different relative shifts (no single reference step to align).
template <NodeType node>
static std::optional<int_t> atom_uniform_shift(tref atom) {
	using tau = tree<node>;
	auto io_vars = tau::get(atom).select_top(is_child<node, tau::io_var>);
	std::optional<int_t> shift;
	for (tref v : io_vars) {
		if (is_io_initial<node>(v)) return std::nullopt;
		int_t s = get_io_var_shift<node>(v);
		if (!shift) shift = s;
		else if (*shift != s) return std::nullopt;
	}
	return shift;
}

// Groups relative-time atoms by (io_var name set, BA type) -- the family a
// shift-chain constraint can relate. Positional atoms and atoms mixing
// io_vars at different shifts are excluded (see atom_uniform_shift).
// Each family's index list is in atoms' discovery order.
template <NodeType node>
static std::map<std::pair<std::set<std::string>, size_t>, std::vector<size_t>>
group_shift_families(const std::vector<std::pair<tref, std::string>>& atoms)
{
	std::map<std::pair<std::set<std::string>, size_t>, std::vector<size_t>> families;
	for (size_t i = 0; i < atoms.size(); ++i) {
		tref a = atoms[i].first;
		if (atom_is_positional<node>(a)) continue;
		auto shift = atom_uniform_shift<node>(a);
		if (!shift) continue;
		auto names = atom_io_var_names<node>(a);
		if (names.empty()) continue;
		families[{std::move(names), find_ba_type<node>(a)}].push_back(i);
	}
	return families;
}

// Returns the temporal operator (F/U/R/W/S/T/G/...) enclosing a positional
// atom of `atoms` within `n`'s subtree, or nullptr if none -- a temporal
// operator gives a fixed-position fact ambiguous recurring semantics, so
// hoisting refuses it.
template <NodeType node>
static tref find_positional_under_temporal_op(
    tref n,
    const std::vector<std::pair<tref, std::string>>& atoms)
{
	using tau = tree<node>;
	const auto& t = tau::get(n);
	if (!t.has_child()) return nullptr;
	const auto& op = t[0];
	if (is_temporal_op<node>(n)) {
		for (auto& [a, name] : atoms)
			if (atom_is_positional<node>(a) && contains<node>(n, a))
				return n;
		return nullptr;
	}
	for (size_t i = 0; i < op.children_size(); ++i)
		if (tref found = find_positional_under_temporal_op<node>(op.child(i), atoms))
			return found;
	return nullptr;
}

// Splits `fm` into maximal top-level conjuncts, descending through wff_and
// and unconditionally through a wff_always's body -- G is transparent
// scaffolding here, not a temporal wrapper of its own, so leaf classification
// below does the real positional/relative split.
// A leaf conjunct with a positional atom must be hoistable alone: mixing it
// with relative-time content is fine as long as each ends up its own leaf
// (a wff_and body under G splits into separate conjuncts); reaching a
// positional atom through F/U/R/W/S/T is still a hard error; conjuncts
// without positional content are left untouched.
// Returns the conjuncts to hoist, in discovery order.
template <NodeType node>
static std::vector<tref> collect_hoist_conjuncts(
    tref fm,
    const std::vector<std::pair<tref, std::string>>& atoms)
{
	using tau = tree<node>;
	bool any_positional = false;
	for (auto& [a, name] : atoms)
		if (atom_is_positional<node>(a)) { any_positional = true; break; }
	if (!any_positional) return {};

	std::vector<tref> hoist;
	std::function<void(tref)> walk = [&](tref n) {
		const auto& t = tau::get(n);
		if (t.has_child()) {
			const auto& op = t[0];
			if (op.value.nt == tau::wff_and) {
				walk(op.first());
				walk(op.second());
				return;
			}
			if (op.value.nt == tau::wff_always) {
				walk(op.first());
				return;
			}
		}
		// n is a leaf (maximal) top-level conjunct.
		bool has_pos = false, has_nonpos = false;
		for (auto& [a, name] : atoms) {
			if (!contains<node>(n, a)) continue;
			if (atom_is_positional<node>(a)) has_pos = true;
			else has_nonpos = true;
		}
		if (!has_pos) return; // no positional content -- not this pass's concern
		if (has_nonpos)
			throw std::runtime_error(
				"conjunct '" + tau::get(n).to_str() + "' mixes a positional "
				"atom with relative-time content in the same conjunct; not "
				"yet supported (no single evaluation step is defined for "
				"the mix)");
		if (tref bad = find_positional_under_temporal_op<node>(n, atoms))
			throw std::runtime_error(
				"positional atom under '" + tau::get(bad).to_str() + "' "
				"is not yet supported (a temporal operator gives a "
				"fixed-position fact recurring semantics that has not "
				"been derived from the interpreter yet)");
		hoist.push_back(n);
	};
	walk(fm);
	return hoist;
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

// past_temporal_tester is declared in ltl_aba.h.

// Skeleton generation with temporal tester collection.
// When encountering S/T nodes, creates a DFA tester entry and returns
// the state variable name (or its negation for T) instead of the
// unsupported S/T operator string.
template <NodeType node>
static std::string skeleton_wff_with_testers(
    tref n,
    const std::vector<std::pair<tref, std::string>>& atoms,
    std::vector<past_temporal_tester>& testers);

template <NodeType node>
static std::string skeleton_str_with_testers(
    tref n,
    const std::vector<std::pair<tref, std::string>>& atoms,
    std::vector<past_temporal_tester>& testers)
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
    std::vector<past_temporal_tester>& testers)
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
std::pair<std::string, std::vector<past_temporal_tester>>
ltl_skeleton_with_testers(
    tref fm,
    const std::vector<std::pair<tref, std::string>>& atoms)
{
	std::vector<past_temporal_tester> testers;
	std::string skel = skeleton_str_with_testers<node>(fm, atoms, testers);
	return {std::move(skel), std::move(testers)};
}

// Append temporal tester DFA constraints to a skeleton string.
// For each tester:  && !state_var && G(X(state_var) <-> transition)
inline void append_tester_constraints(
    std::string& skeleton,
    const std::vector<past_temporal_tester>& testers)
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
	auto has_out = tau::get(atom).find_top([](tref n) {
		return io_var_direction<node>(n) == 2;
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
		return io_var_direction<node>(n) == 1;
	});
	bool result = has_in != nullptr;
#ifdef TAU_CACHE
	cache.emplace(atom, result);
#endif // TAU_CACHE
	return result;
}

// Recognizes the shape aba_existential_feasible's consistency-pruning callers
// build -- a conjunction of (possibly negated) structured data atoms,
// coordinate==coordinate or coordinate==constant, over ONE atomless BA's
// io_vars -- and translates it into ocltl_phi_delta_direct's own input shape.
// Builds only the (dims, atoms, D) triple; the caller decides whether to
// trust the closed form's answer.

// A coordinate's identity: which io_var, at which relative shift.
struct ocltl_swap_coord_key {
	std::string name;
	int_t shift = 0;
	bool operator==(const ocltl_swap_coord_key&) const = default;
};

// Local, ad hoc sigma/rho split: a shift>=1 or input coordinate is
// already-committed (sigma-side); a shift==0 output coordinate is the
// step's own target (rho-side).
// [[maybe_unused]]: unused when this TU only extern-declares the ltl_aba
// template family (e.g. TAU_ARTIFACT_PREINST builds).
[[maybe_unused]] static bool ocltl_swap_coord_is_rho(bool is_output, int_t shift) {
	return is_output && shift == 0;
}

// Walks fm expecting exactly a conjunction of (possibly negated) atoms; on
// success appends each leaf literal (atom tref, asserted-true polarity) to
// `out` and returns true. Any other shape (disjunction, temporal operator,
// quantifier, non-atom leaf) fails the match.
template <NodeType node>
static bool collect_conjunction_literals(tref fm,
    std::vector<std::pair<tref, bool>>& out)
{
	using tau = tree<node>;
	const auto& t = tau::get(fm);
	if (t.equals_T()) return true;  // vacuous conjunct -- contributes nothing
	if (t.equals_F()) return false; // callers already special-case bare F
	if (!t.has_child()) return false;
	auto nt = t[0].value.nt;
	if (nt == tau::wff_and)
		return collect_conjunction_literals<node>(t[0].first(), out)
		    && collect_conjunction_literals<node>(t[0].second(), out);
	bool neg = false;
	tref atom = fm;
	if (nt == tau::wff_neg) { neg = true; atom = t[0].first(); }
	if (!is_aba_comparison<node>(atom) || !has_io_var<node>(atom)) return false;
	out.emplace_back(atom, !neg);
	return true;
}

// Classifies a ground BA-constant operand as its algebra's unit (true) or
// zero (false); nullopt for anything else (a variable, or a constant that is
// neither -- phi_delta's coordinate==constant atom only models {0,1}).
template <NodeType node>
static std::optional<bool> ocltl_swap_classify_constant(tref n) {
	using tau = tree<node>;
	const auto& t = tau::get(n);
	// Reuses nso_ba.tmpl.h's is_zero/is_one (handles every constant
	// representation, e.g. {T.}/{F.} collapsing to bf_1/bf_0); takes `n`
	// at the un-unwrapped `bf` level those expect, not the io_var unwrap.
	if (idni::tau_lang::is_zero(t)) return false;
	if (idni::tau_lang::is_one(t)) return true;
	return std::nullopt;
}

// The translated match: a phi_delta-ready (dims, atoms, D) triple deciding
// exactly the conjunction match_ocltl_swap_shape<node> was given.
struct ocltl_swap_match {
	ocltl_phi_delta_dims dims;
	std::vector<ocltl_delta_atom> atoms;
	size_t D = 0;
};

// Attempts to view fm as a conjunction of structured data atoms over one
// atomless BA (see collect_conjunction_literals); nullopt if it doesn't
// match this shape (wrong operator, mixed/non-atomless BA types, etc.).
template <NodeType node>
static std::optional<ocltl_swap_match> match_ocltl_swap_shape(tref fm) {
	using tau = tree<node>;
	std::vector<std::pair<tref, bool>> literals;
	if (!collect_conjunction_literals<node>(fm, literals)) return std::nullopt;
	if (literals.empty()) return std::nullopt; // vacuous -- nothing to decide via this path

	std::optional<size_t> ba_type;
	std::vector<ocltl_swap_coord_key> sigma_keys, rho_keys;
	auto coord_index = [&](const ocltl_swap_coord_key& k, bool rho) -> size_t {
		auto& keys = rho ? rho_keys : sigma_keys;
		for (size_t i = 0; i < keys.size(); ++i) if (keys[i] == k) return i;
		keys.push_back(k);
		return keys.size() - 1;
	};
	// var_is_input mirrors add_shift_chain_constraints's own is_input
	// lambda (ltl_aba_normalization.tmpl.h); v may be the "variable"
	// wrapper or (defensively) an already-bare io_var.
	auto var_is_input = [](tref v) {
		const auto& tv = tau::get(v);
		return tv.child_is(tau::io_var) ? tv[0].is_input_variable() : tv.is_input_variable();
	};

	struct pending_atom {
		bool is_eq_eq; size_t p; bool p_rho; size_t q; bool q_rho;
		bool c; bool asserted;
	};
	std::vector<pending_atom> pending;

	for (auto& [atom, asserted] : literals) {
		const auto& t = tau::get(atom);
		if (!t.has_child()) return std::nullopt;
		auto op = t[0].value.nt;
		bool flip = false;
		if (op == tau::bf_neq) flip = true;
		else if (op != tau::bf_eq) return std::nullopt; // lt/gt/interval/... -- not this shape
		bool lit_asserted = asserted != flip;

		size_t ty = find_ba_type<node>(atom);
		if (!pack_type_is_atomless<node>(ty)) return std::nullopt;
		if (ba_type && *ba_type != ty) return std::nullopt; // mixed BA types -- not this shape
		ba_type = ty;

		// A comparison operand is a `bf` wrapping its content one level down:
		// is_io_var wants that unwrapped depth, but classify_constant wants
		// the raw un-unwrapped operand -- both are kept.
		tref raw_lhs = t[0].first(), raw_rhs = t[0].second();
		auto unwrap_bf = [&](tref v) {
			return tau::get(v).has_child() ? tau::get(v).first() : v;
		};
		tref lhs = unwrap_bf(raw_lhs), rhs = unwrap_bf(raw_rhs);
		bool lhs_var = is_io_var<node>(lhs), rhs_var = is_io_var<node>(rhs);

		auto var_key = [&](tref v) {
			int_t shift = get_io_var_shift<node>(v);
			bool rho = ocltl_swap_coord_is_rho(!var_is_input(v), shift);
			return std::pair{ ocltl_swap_coord_key{
				get_var_name<node>(v), shift }, rho };
		};

		if (lhs_var && rhs_var) {
			auto [lk, lrho] = var_key(lhs);
			auto [rk, rrho] = var_key(rhs);
			if (lk == rk && lrho == rrho) {
				if (!lit_asserted) return std::nullopt; // p != p required -- let the solver see it
				continue; // p == p required -- tautology, contributes nothing
			}
			size_t p = coord_index(lk, lrho), q = coord_index(rk, rrho);
			pending.push_back({ true, p, lrho, q, rrho, false, lit_asserted });
		} else if (lhs_var || rhs_var) {
			tref var = lhs_var ? lhs : rhs;
			tref cst = lhs_var ? raw_rhs : raw_lhs;
			auto c = ocltl_swap_classify_constant<node>(cst);
			if (!c) return std::nullopt;
			auto [vk, vrho] = var_key(var);
			size_t p = coord_index(vk, vrho);
			pending.push_back({ false, p, vrho, 0, false, *c, lit_asserted });
		} else return std::nullopt; // neither side is a bare io_var -- not this shape
	}
	if (!ba_type) return std::nullopt; // every literal was a same-coordinate tautology

	// Second pass: sigma coords get [0, d_m); rho coords get [d_m, d_m+d_y).
	size_t d_m = sigma_keys.size(), d_y = rho_keys.size();
	if (d_m + d_y == 0 || d_m + d_y > 30) return std::nullopt; // degenerate, or too wide to be worth it here
	ocltl_swap_match m;
	m.dims = { d_m, 0, d_y };
	for (auto& pa : pending) {
		size_t p = pa.p + (pa.p_rho ? d_m : 0);
		ocltl_delta_atom da = pa.is_eq_eq
			? ocltl_atom_coordinate_eq(p, pa.q + (pa.q_rho ? d_m : 0))
			: ocltl_atom_coordinate_const(p, pa.c);
		size_t i = m.atoms.size();
		m.atoms.push_back(std::move(da));
		if (pa.asserted) m.D |= (size_t{1} << i);
	}
	return m;
}

} // namespace idni::tau_lang
