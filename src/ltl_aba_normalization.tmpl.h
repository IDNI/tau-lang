// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// ltl_aba_normalization.tmpl.h - Guard-to-ABA, feasibility, oracle, consistency, S/T compile
// Split from ltl_aba.tmpl.h for readability.

namespace idni::tau_lang {


// ── guard → ABA formula ───────────────────────────────────────────────────────

// Parse a single HOA guard label into a tau-lang formula.
// Grammar: guard ::= 't' | 'f' | N | '!N' | '(guard)' | guard '&' guard | guard '|' guard
template <NodeType node>
static tref parse_guard_expr(
    const std::string& s,
    size_t& pos,
    const std::vector<std::string>& aps,
    const std::vector<std::pair<tref, std::string>>& atoms)
{
	using tau = tree<node>;

	auto skip_ws = [&]{ while (pos < s.size() && s[pos]==' ') ++pos; };
	skip_ws();
	if (pos >= s.size()) return tau::_T();

	// Recursive-descent parser with correct HOA precedence: ! > & > |
	// parse_atom handles literals, negations, and parenthesised sub-expressions.
	// parse_conj handles & (right-recursive).
	// parse_guard_expr handles | (right-recursive, lowest precedence).

	std::function<tref()> parse_atom, parse_conj;

	parse_atom = [&]() -> tref {
		skip_ws();
		if (pos >= s.size()) return tau::_T();
		if (s[pos] == 't') { ++pos; return tau::_T(); }
		if (s[pos] == 'f') { ++pos; return tau::_F(); }
		if (s[pos] == '(') {
			++pos;
			tref inner = parse_guard_expr<node>(s, pos, aps, atoms);
			skip_ws();
			if (pos < s.size() && s[pos] == ')') ++pos;
			return inner;
		}
		bool neg = false;
		if (s[pos] == '!') { neg = true; ++pos; skip_ws(); }
		size_t start = pos;
		while (pos < s.size() && std::isdigit(s[pos])) ++pos;
		if (start == pos) return neg ? tau::_F() : tau::_T(); // no digits
		int idx = std::stoi(s.substr(start, pos - start));
		// User-data atoms map AP-name → atom tref via `atoms`.  APs that
		// appear in the strategy but are NOT in `atoms` are bookkeeping
		// propositions added by the synthesis encoding (Algorithm A's
		// R-bits, Algorithm B's P-bits, etc.).  They have no
		// user-visible meaning in the derived safety formula — the
		// strategy will assign them consistently at runtime — so they
		// drop out as TRUE (regardless of polarity).  Without this
		// drop, `[d_0 & r_0 & !r_1]` would parse to
		// `(d_0_atom) ∧ ⊤ ∧ ¬⊤ = FALSE`, which is exactly the bug
		// QE-03 / QE-09 / QE-15 etc. tripped over.
		if (idx >= 0 && idx < (int)aps.size()) {
			const std::string& ap_name = aps[idx];
			for (auto& [f, name] : atoms)
				if (name == ap_name)
					return neg ? tau::build_wff_neg(f) : f;
		}
		return tau::_T();
	};

	parse_conj = [&]() -> tref {
		tref lhs = parse_atom();
		skip_ws();
		while (pos < s.size() && s[pos] == '&') {
			++pos;
			tref rhs = parse_atom();
			lhs = tau::build_wff_and(lhs, rhs);
			skip_ws();
		}
		return lhs;
	};

	tref result = parse_conj();
	skip_ws();
	while (pos < s.size() && s[pos] == '|') {
		++pos;
		tref rhs = parse_conj();
		result = tau::build_wff_or(result, rhs);
		skip_ws();
	}
	return result;
}

template <NodeType node>
tref guard_to_aba(
    const std::string& guard_label,
    const std::vector<std::string>& aps,
    const std::vector<std::pair<tref, std::string>>& atoms)
{
	size_t pos = 0;
	return parse_guard_expr<node>(guard_label, pos, aps, atoms);
}

// ── ABA feasibility checks ───────────────────────────────────────────────────
//
// Two distinct checks are used at different points in the pipeline:
//
// 1. aba_synthesis_feasible (used in add_consistency_constraints):
//    Synthesis-feasibility — can the system ALWAYS satisfy this constraint
//    regardless of env choices?  Detected by wrapping as G(fm) and running
//    the safety synthesis pipeline.  Used to add G(!p) / G(!(pi&&pj)) skeleton
//    constraints that prune ltlsynt-infeasible atoms/pairs before ltlsynt runs.
//
// 2. aba_existential_feasible (used in guard_is_aba_feasible / oracle):
//    Existential satisfiability — does there EXIST any (i,o) satisfying this
//    guard?  After the consistency constraints have been added, ltlsynt only
//    produces guards built from individually-feasible and pairwise-compatible
//    atoms, so existential satisfiability is sufficient for the oracle check.
//    Crucially, input atoms in a guard represent conditions the env satisfies
//    when that edge fires, so the guard is evaluated GIVEN those inputs hold —
//    using synthesis here would give false infeasibility for conditional guards.

template <NodeType node>
static bool aba_synthesis_feasible(tref fm) {
	using tau = tree<node>;
	if (tau::get(fm).equals_T()) return true;
	if (tau::get(fm).equals_F()) return false;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(fm); it != cache.end()) return it->second;
#endif // TAU_CACHE
	// Wrap as G(fm); G alone (wff_always) is not a full-LTL operator so
	// is_tau_formula_sat routes through the safety pipeline, not back here.
	tref g_fm = tau::build_wff_always(fm);
	bool result = is_tau_formula_sat<node>(g_fm, 0, false);
#ifdef TAU_CACHE
	cache.emplace(fm, result);
#endif // TAU_CACHE
	return result;
}

template <NodeType node>
static bool aba_existential_feasible(tref fm) {
	using tau = tree<node>;
	if (tau::get(fm).equals_T()) return true;
	if (tau::get(fm).equals_F()) return false;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(fm); it != cache.end()) return it->second;
#endif // TAU_CACHE
	auto compute = [&]() -> bool {
		// For qlt formulas: use qlt_dlo_qe directly on each free variable.
		if constexpr (ba_variant_includes_v<qlt, typename tau::constant>) {
			const trefs& free_vars = tau::get(fm).get_free_vars();
			if (!free_vars.empty()) {
				bool all_qlt = true;
				for (tref v : free_vars)
					if (!is_omcat_type_family<node>(tree<node>::get(v).get_ba_type()))
						{ all_qlt = false; break; }
				if (all_qlt) {
					bool all_determined = true;
					for (tref v : free_vars) {
						if (auto r = qlt_dlo_qe<node>(v, fm); r) {
							if (r.value().is_empty()) return false;
						} else {
							all_determined = false; break;
						}
					}
					if (all_determined) return true;
				}
			}
		}
		constexpr bool has_nlang_ba = ba_variant_includes_v<nlang_ba, typename tau::constant>;
		if constexpr (has_nlang_ba) {
			const trefs& fvars = tau::get(fm).get_free_vars();
			for (tref v : fvars)
				if (is_nlang_type<node>(tree<node>::get(v).get_ba_type())) return true;
		}
		return is_non_temp_nso_satisfiable<node>(fm);
	};
	bool result = compute();
#ifdef TAU_CACHE
	cache.emplace(fm, result);
#endif // TAU_CACHE
	return result;
}

// Unified feasibility dispatch (code_restruct_suggestion #6).
//
// Picks between existential (∃(i,o). fm) and synthesis (∀i.∃o. fm) feasibility.
// The pattern previously duplicated at call sites is now centralized here:
//
//   - Pure-input formulas → EXISTENTIAL: pure-input constraints are env
//     assumptions, not system obligations; ∃i captures env freedom.
//   - Omcat (qlt) output-only formulas → EXISTENTIAL: system sets outputs
//     directly; qlt conjunctions are brittle under safety synthesis.
//   - Input-bearing or non-omcat formulas → SYNTHESIS: env picks inputs
//     adversarially; system needs ∀i.∃o guarantees.
//
// Callers still compute `pure_input` and `has_input` themselves (each has a
// different fast path: single-atom uses is_pure_input_atom; pair uses both),
// so this helper takes them as parameters.  Keeps the core policy in one place.
template <NodeType node>
static bool aba_feasible_dispatch(tref fm, bool pure_input, bool has_input) {
	if (pure_input) return aba_existential_feasible<node>(fm);
	size_t ti = find_ba_type<node>(fm);
	if (is_omcat_type_family<node>(ti) && !has_input)
		return aba_existential_feasible<node>(fm);
	// nlang: for pure-output formulas the system can always satisfy o=p by
	// setting its output to the proposition p. Route to existential feasibility
	// to avoid the safety fixpoint (which is expensive for nlang and unnecessary
	// when there are no inputs to adversarially constrain the output).
	using dispatch_tau = tree<node>;
	constexpr bool has_nlang_ba2 = ba_variant_includes_v<nlang_ba, typename dispatch_tau::constant>;
	if constexpr (has_nlang_ba2) {
		if (is_nlang_type<node>(ti) && !has_input)
			return aba_existential_feasible<node>(fm);
	}
	return aba_synthesis_feasible<node>(fm);
}

// ── ABA oracle: check if a strategy guard is ABA-feasible ────────────────────
//
// A guard g(i, o) is feasible if there exists some concrete (i, o) satisfying
// it — i.e., existential satisfiability.  Each edge in a Mealy strategy fires
// CONDITIONALLY (for specific input values), so the oracle's job is only to
// verify ABA consistency at that edge, not to prove global synthesis-feasibility.

template <NodeType node>
static bool guard_is_aba_feasible(
    const std::string& guard_label,
    const std::vector<std::string>& aps,
    const std::vector<std::pair<tref, std::string>>& atoms)
{
	using tau = tree<node>;

	// Parse guard label into (atom_fm, negated, is_pure_input) tuples.
	struct GuardLit { tref atom; bool neg; bool pure_input; };
	std::vector<GuardLit> lits;
	{
		std::string s = guard_label;
		size_t pos = 0;
		auto skip_ws = [&]{ while (pos < s.size() && s[pos]==' ') ++pos; };
		skip_ws();
		while (pos < s.size()) {
			if (s[pos] == '&') { ++pos; skip_ws(); continue; }
			bool neg = false;
			if (s[pos] == '!') { neg = true; ++pos; skip_ws(); }
			if (pos >= s.size() || !std::isdigit(s[pos])) break;
			size_t start = pos;
			while (pos < s.size() && std::isdigit(s[pos])) ++pos;
			int idx = std::stoi(s.substr(start, pos - start));
			skip_ws();
			tref atom_fm = nullptr;
			if (idx >= 0 && idx < (int)aps.size()) {
				for (auto& [f, name] : atoms)
					if (name == aps[idx]) { atom_fm = f; break; }
			}
			if (!atom_fm) continue; // bookkeeping/tester AP — skip
			lits.push_back({atom_fm, neg, is_pure_input_atom<node>(atom_fm)});
		}
	}

	// Dead-edge check: if the input-only literals are already infeasible,
	// the environment can never trigger this guard — skip it (return true).
	// This handles the case where ltlsynt emits "catch-all" edges covering
	// input combinations excluded by G(!(pi && pj)) assumptions.
	{
		tref input_conj = tau::_T();
		bool has_input = false;
		for (auto& gl : lits) {
			if (!gl.pure_input) continue;
			has_input = true;
			tref lit = gl.neg ? tau::build_wff_neg(gl.atom) : gl.atom;
			input_conj = tau::build_wff_and(input_conj, lit);
		}
		if (has_input && !aba_existential_feasible<node>(input_conj))
			return true; // Dead edge — env can never make these inputs hold.
	}

	// Full guard check: group by BA type and verify each type-partition is
	// ABA-feasible.  Different BA types involve independent variables.
	auto types_seen = FormulaTypeSet<node>::from_atoms(atoms);
	if (types_seen.single_type()) {
		// Fast path: single type — build full conjunction and check.
		tref conj = tau::_T();
		for (auto& gl : lits) {
			tref lit = gl.neg ? tau::build_wff_neg(gl.atom) : gl.atom;
			conj = tau::build_wff_and(conj, lit);
		}
		return aba_existential_feasible<node>(conj);
	}

	// Cross-type: split by BA type, check each partition independently.
	std::map<size_t, std::vector<tref>> per_type;
	for (auto& gl : lits) {
		tref lit = gl.neg ? tau::build_wff_neg(gl.atom) : gl.atom;
		per_type[find_ba_type<node>(gl.atom)].push_back(lit);
	}
	for (auto& [tid, type_lits] : per_type) {
		tref conj = tau::_T();
		for (auto& lit : type_lits) conj = tau::build_wff_and(conj, lit);
		if (!aba_existential_feasible<node>(conj)) return false;
	}
	return true;
}


// True if the formula contains any io_var with a non-zero shift (lookback).
// Such atoms represent past-time values and must NOT be fed into the safety
// synthesis pipeline for individual feasibility — the safety pipeline's
// initial-value convention makes lookback atoms appear unconditionally
// infeasible from t=0, which is incorrect in the LTL context.
template <NodeType node>
static bool atom_has_lookback(tref atom) {
	using tau = tree<node>;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(atom); it != cache.end()) return it->second;
#endif // TAU_CACHE
	auto found = tau::get(atom).find_top([](tref n) {
		const auto& nd = tree<node>::get(n);
		return nd.is(tau::shift);
	});
	bool result = found != nullptr;
#ifdef TAU_CACHE
	cache.emplace(atom, result);
#endif // TAU_CACHE
	return result;
}

// ── ABA consistency constraints ──────────────────────────────────────────────
//
// For each atom and each pair of atoms, check ABA feasibility.  Append
// G(!atom) / G(!(pi && pj)) suffixes to `skeleton` for infeasible ones and
// (optionally) record the constraint strings in `out_constraints`.
//
// Individual atoms: use synthesis-feasibility (∀i.∃o).  This catches mixed
// output+input atoms (e.g. o1 & i1 = T.) that the env can always block.
//
// Pairs: use synthesis-feasibility when BOTH atoms involve output variables.
// This catches pairs like (o1=i1[t-1], o1=c) or (o1=i1, o1=i2) where the
// env can choose inputs to make both simultaneously satisfiable or not.
// When at least one atom is pure-input, use existential ABA consistency only
// (an input atom represents an env-given condition, not a system obligation).

// Surgical completeness extension of the pairwise + oracle pipeline.
//
// The original algorithm is sound (oracle checks each edge) but theoretically
// incomplete: ltlsynt, given only pairwise constraints, may return a strategy
// that uses a positive triple (or larger) whose conjunction is ∀∃-infeasible;
// the oracle then rejects and we return UNREAL — even though a different
// propositional strategy with only feasible edges may exist for the same
// formula.
//
// `extend_consistency_positive_k_ary` closes this gap for POSITIVE polarities
// (the ones ltlsynt would actually use in a guard of an edge that *forces*
// several atoms to hold simultaneously).  It walks positive subsets of atoms
// in DPLL order: when a positive k-subset is ∀∃-infeasible (and none of its
// proper subsets already forbade it), emit `G(!(⋀ k-subset))` and prune.
//
// Why positive-only: negative-polarity infeasibility (e.g. ∀∃ ¬δ_1 ∧ ¬δ_2
// infeasible) is real but represents a *tautology* the system outputs cannot
// violate — adding a forbid on negative literals doesn't help ltlsynt choose
// a better strategy and can over-constrain mixed atoms (where ltlsynt cannot
// independently choose D_i polarities).  Positive forbids are the ones that
// actually matter for soundness of edge guards.
//
// Emission is conservative: we emit only if the k-subset is NOT already
// subsumed by a shorter forbid already in out_constraints.  This keeps the
// constraint set minimal.
template <NodeType node>
static void extend_consistency_positive_k_ary(
    const std::vector<std::pair<tref, std::string>>& atoms,
    std::string& skeleton,
    std::vector<std::string>* out_constraints)
{
	using tau = tree<node>;
	const int n = static_cast<int>(atoms.size());
	if (n < 3) return;  // pairwise already handled by caller

	// Collect existing forbid patterns (as sets of atom indices) so we can
	// skip subsumed k-subsets.
	std::vector<std::vector<int>> existing_forbid_sets;
	if (out_constraints) {
		for (const auto& c : *out_constraints) {
			// Parse "G(!(name1 && name2 && ...))" into a set of indices.
			std::vector<int> idxs;
			for (int i = 0; i < n; ++i) {
				if (c.find(atoms[i].second) != std::string::npos) idxs.push_back(i);
			}
			if (idxs.size() >= 2) existing_forbid_sets.push_back(std::move(idxs));
		}
	}
	auto is_subsumed = [&](const std::vector<int>& cand) {
		for (const auto& f : existing_forbid_sets) {
			// cand is subsumed iff f ⊆ cand.
			if (f.size() > cand.size()) continue;
			bool all_in = true;
			for (int fi : f) {
				bool found = false;
				for (int ci : cand) if (ci == fi) { found = true; break; }
				if (!found) { all_in = false; break; }
			}
			if (all_in) return true;
		}
		return false;
	};

	// DPLL walk over positive subsets of size ≥ 3.
	std::function<void(int, tref, std::vector<int>&)> walk =
	    [&](int idx, tref prefix_body, std::vector<int>& sel) {
		const int size = static_cast<int>(sel.size());
		if (size >= 3) {
			if (is_subsumed(sel)) return;
			// Skip if ANY atom is pure-output-lookback: the safety pipeline's
			// t=0 convention treats uninitialized lookback values as F, which
			// makes lookback atoms spuriously infeasible under G-wrap.  This
			// matches the pairwise check's skip condition.
			bool any_pure_out_lb = false;
			for (int i : sel) {
				bool is_mixed = atom_has_any_input<node>(atoms[i].first);
				bool pure_out_lb = atom_has_lookback<node>(atoms[i].first) && !is_mixed;
				if (pure_out_lb) { any_pure_out_lb = true; break; }
			}
			if (any_pure_out_lb) return;
			if (!aba_synthesis_feasible<node>(prefix_body)) {
				std::string pat;
				for (int i : sel) {
					if (!pat.empty()) pat += " && ";
					pat += atoms[i].second;
				}
				std::string c = "G(!(" + pat + "))";
				skeleton += " && " + c;
				if (out_constraints) {
					out_constraints->push_back(c);
					existing_forbid_sets.push_back(sel);
				}
				return;  // prune: supersets already implied.
			}
		}
		if (idx == n) return;
		// Skip atom idx.
		walk(idx + 1, prefix_body, sel);
		// Include atom idx (positive only).
		tref new_prefix = tau::build_wff_and(prefix_body, atoms[idx].first);
		sel.push_back(idx);
		walk(idx + 1, new_prefix, sel);
		sel.pop_back();
	};
	std::vector<int> sel;
	walk(0, tau::_T(), sel);
}

template <NodeType node>
static void add_consistency_constraints(
    const std::vector<std::pair<tref, std::string>>& atoms,
    std::string& skeleton,
    std::vector<std::string>* out_constraints = nullptr,
    bool polarity_complete = false)
{
	using tau = tree<node>;

	// Snapshot original skeleton to detect G(!atom) constraints from the
	// actual formula (not from constraints we are about to add).
	const std::string original_skeleton = skeleton;

	// Input-only infeasibility constraints are env assumptions, not requirements.
	// Collect them separately and wrap the skeleton with (assumptions) -> (...)
	// so ltlsynt treats them as environment guarantees, not system obligations.
	std::string input_assumptions;

	for (size_t i = 0; i < atoms.size(); ++i) {
		if (!is_pure_input_atom<node>(atoms[i].first)) {
			// Skip individual check for atoms with lookback shifts: the safety
			// synthesis pipeline treats initial lookback values as 0, which
			// makes lookback atoms appear unconditionally infeasible even though
			// they become true through temporal feedback.
			{
				// Skip feasibility only for pure-output lookback atoms: the safety
				// pipeline's t=0 convention makes them appear infeasible.
				// Mixed atoms (containing input vars) must use synthesis (∀i.∃o).
				bool is_mixed = atom_has_any_input<node>(atoms[i].first);
				bool pure_out_lookback = atom_has_lookback<node>(atoms[i].first)
				    && !is_mixed;
				bool pure_input = is_pure_input_atom<node>(atoms[i].first);
				bool feasible = pure_out_lookback
				    || aba_feasible_dispatch<node>(atoms[i].first,
				                                   pure_input, is_mixed);
				if (!feasible) {
					std::string c = "G(!" + atoms[i].second + ")";
					skeleton += " && " + c;
					if (out_constraints) out_constraints->push_back(c);
				}
			}
		}
		for (size_t j = i + 1; j < atoms.size(); ++j) {
			// Skip cross-type pairs: atoms from different type families are
			// jointly feasible by design (independent variables, independent solvers).
			if (find_ba_type<node>(atoms[i].first) != find_ba_type<node>(atoms[j].first))
				continue;
			// Skip pairs where either atom is a pure-output lookback: the safety
			// pipeline's t=0 convention makes such pairs appear infeasible.
			// Input-containing lookback atoms (e.g. o1=i1[t-1]) must be checked —
			// the env can block them by choosing past inputs adversarially.
			bool i_pure_out_lb = atom_has_lookback<node>(atoms[i].first)
			    && !atom_has_any_input<node>(atoms[i].first);
			bool j_pure_out_lb = atom_has_lookback<node>(atoms[j].first)
			    && !atom_has_any_input<node>(atoms[j].first);
			if (i_pure_out_lb || j_pure_out_lb)
				continue;
			tref conj = tau::build_wff_and(atoms[i].first, atoms[j].first);
			bool pure_input =
			    is_pure_input_atom<node>(atoms[i].first) ||
			    is_pure_input_atom<node>(atoms[j].first);
			bool has_input = atom_has_any_input<node>(atoms[i].first)
			              || atom_has_any_input<node>(atoms[j].first);
			bool infeasible =
			    !aba_feasible_dispatch<node>(conj, pure_input, has_input);
			if (infeasible) {
				bool both_pure_input = is_pure_input_atom<node>(atoms[i].first)
				                    && is_pure_input_atom<node>(atoms[j].first);
				std::string c = "G(!(" + atoms[i].second
				              + " && " + atoms[j].second + "))";
				if (both_pure_input) {
					// Input-only constraint: env assumption, not system requirement.
					if (!input_assumptions.empty()) input_assumptions += " && ";
					input_assumptions += c;
				} else {
					skeleton += " && " + c;
				}
				if (out_constraints) out_constraints->push_back(c);
			}
		}
	}

	// Algorithm B: polarity-complete pairwise constraints.
	// Extends the positive pairwise check above with the 3 mixed-polarity
	// combinations (¬p_i ∧ p_j), (p_i ∧ ¬p_j), (¬p_i ∧ ¬p_j) for each pair.
	// Uses existential feasibility (∃m,x,y. combo) — same standard as the
	// oracle — so we emit exactly the constraints the oracle would later reject.
	// Gated on TAU_LTL_ALG=B; default behaviour is unchanged.
	{
		const bool alg_b_mode = [] {
			const char* v = std::getenv("TAU_LTL_ALG");
			return v && std::string_view(v) == "B";
		}();
		if (alg_b_mode || polarity_complete) {
			for (size_t i = 0; i < atoms.size(); ++i) {
				if (is_pure_input_atom<node>(atoms[i].first)) continue;
				bool i_pure_out_lb = atom_has_lookback<node>(atoms[i].first)
				                  && !atom_has_any_input<node>(atoms[i].first);
				if (i_pure_out_lb) continue;
				tref neg_i = tau::build_wff_neg(atoms[i].first);
				for (size_t j = i + 1; j < atoms.size(); ++j) {
					if (is_pure_input_atom<node>(atoms[j].first)) continue;
					if (find_ba_type<node>(atoms[i].first)
					    != find_ba_type<node>(atoms[j].first)) continue;
					bool j_pure_out_lb = atom_has_lookback<node>(atoms[j].first)
					                  && !atom_has_any_input<node>(atoms[j].first);
					if (j_pure_out_lb) continue;
					tref neg_j = tau::build_wff_neg(atoms[j].first);
					const auto& ni = atoms[i].second;
					const auto& nj = atoms[j].second;
					auto emit = [&](tref combo, const std::string& c) {
						if (skeleton.find(c) == std::string::npos
						    && !aba_existential_feasible<node>(combo)) {
							skeleton += " && " + c;
							if (out_constraints) out_constraints->push_back(c);
						}
					};
					// (¬p_i ∧ p_j)
					emit(tau::build_wff_and(neg_i, atoms[j].first),
					     "G(!(!(" + ni + ") && " + nj + "))");
					// (p_i ∧ ¬p_j)
					emit(tau::build_wff_and(atoms[i].first, neg_j),
					     "G(!(" + ni + " && !(" + nj + ")))");
					// (¬p_i ∧ ¬p_j)
					emit(tau::build_wff_and(neg_i, neg_j),
					     "G(!(!(" + ni + ") && !(" + nj + ")))");
				}
			}
		}
	}

	// Phase 3: global-negative constraint propagation.
	// If G(!pj) appears in the original formula skeleton, atom pj is globally
	// false.  Any other non-pure-input atom pi that cannot hold simultaneously
	// with !pj must therefore also be globally false (add G(!pi)).
	// This catches cases like F(o=i) && G(!(o=0)): o=i cannot hold while o!=0
	// when the adversarial environment sends i=0.
	for (size_t j = 0; j < atoms.size(); ++j) {
		std::string g_not_pj = "G(!" + atoms[j].second + ")";
		if (original_skeleton.find(g_not_pj) == std::string::npos) continue;

		tref neg_pj = tau::build_wff_neg(atoms[j].first);
		for (size_t i = 0; i < atoms.size(); ++i) {
			if (i == j) continue;
			if (is_pure_input_atom<node>(atoms[i].first)) continue;
			// Skip cross-type: different-type atoms are independent.
			if (find_ba_type<node>(atoms[i].first) != find_ba_type<node>(atoms[j].first))
				continue;
			tref conj = tau::build_wff_and(atoms[i].first, neg_pj);
			if (!aba_synthesis_feasible<node>(conj)) {
				std::string c = "G(!" + atoms[i].second + ")";
				if (skeleton.find(c) == std::string::npos) {
					skeleton += " && " + c;
					if (out_constraints) out_constraints->push_back(c);
				}
			}
		}
	}

	// Wrap skeleton with input-only assumptions as env guarantees.
	// G(!(pi && pj)) constraints on pure-input atoms are env behavior the system
	// cannot guarantee — they must appear as assumptions, not requirements.
	if (!input_assumptions.empty())
		skeleton = "(" + input_assumptions + ") -> (" + skeleton + ")";

	// Completeness extension: positive k-ary (k≥3) infeasibility constraints.
	// Pairwise-only leaves a soundness-incompleteness gap where ltlsynt may
	// pick a strategy using a triple-infeasible guard (pairs all feasible, but
	// triple not); oracle rejection would then yield a false UNREAL while a
	// different strategy might be REAL.  Positive k-ary forbids strictly
	// extend pairwise: they never block a strategy compatible with the real
	// ABA feasibility, only strategies that rely on k-wise-infeasible guards.
	// Partition by BA to keep independence (atoms in different BAs cannot
	// jointly constrain each other).
	{
		std::map<size_t, std::vector<std::pair<tref, std::string>>> by_type;
		for (const auto& a : atoms) {
			if (is_pure_input_atom<node>(a.first)) continue;  // env assumptions only
			by_type[find_ba_type<node>(a.first)].push_back(a);
		}
		for (auto& [ti, grp] : by_type) {
			if (grp.size() >= 3)
				extend_consistency_positive_k_ary<node>(grp, skeleton, out_constraints);
		}
	}
}

// ── Internal: solve LTL(ABA) problem ─────────────────────────────────────────
//
// Bundles the common steps shared by is_ltl_aba_realizable and
// ltl_to_safety_formula: atom extraction, classification, skeleton + ABA
// consistency constraints, ltlsynt call, HOA parse.
//
// Returns {atoms, input_props, output_props, automaton} on success,
// std::nullopt when propositionally unrealizable.
// The caller must perform the ABA oracle check if desired.

template <NodeType node>
struct LtlAbaSolution {
	std::vector<std::pair<tref, std::string>> atoms;
	std::vector<std::string> input_props;
	std::vector<std::string> output_props;
	std::string skeleton;         // skeleton sent to ltlsynt
	HoaAutomaton aut;
};

// ── S/T compile-away pass ─────────────────────────────────────────────────────
//
// φ S ψ  ("φ Since ψ"): introduce auxiliary output o__ltl_s{k}__ with:
//   G(o__ltl_s{k}__[t]:bv = {1}  ↔  (ψ[t]  ∨  (φ[t]  ∧  o__ltl_s{k}__[t-1]:bv = {1})))
// and replace wff_S(φ,ψ) with the atom  o__ltl_s{k}__[t]:bv = {1}.
//
// φ T ψ  ("φ Trigger ψ") = ¬(¬φ S ¬ψ): rewrite and apply S.
//
// After the pass the formula no longer contains wff_S or wff_T, so it can
// be sent to ltlsynt.

// Return true if the tree rooted at fm contains any wff_S or wff_T node.
template <NodeType node>
static bool has_since_trigger(tref fm) {
	using tau = tree<node>;
	return tau::get(fm).find_top([](tref n) {
		const auto& t = tree<node>::get(n);
		if (!t.has_child()) return false;
		auto nt = t[0].value.nt;
		return nt == tree<node>::wff_S || nt == tree<node>::wff_T;
	}) != nullptr;
}

// Build   name[t+shift]:bv = {value}   (shift ≤ 0: -1 → t-1, 0 → t).
template <NodeType node>
static tref build_bv_eq_aux(const std::string& name, int shift, int value) {
	using tau = tree<node>;
	assert(shift <= 0 && "build_bv_eq_aux: shift must be <= 0 (past or current)");
	std::string t_str = (shift == 0) ? "t" : ("t-" + std::to_string(-shift));
	std::string expr = name + "[" + t_str + "]:bv = { "
	                 + std::to_string(value) + " }";
	typename tau::get_options opts;
	opts.parse.start = tau::wff;
	return tau::get(expr, std::move(opts));
}

// Recursively rewrite all wff_S / wff_T nodes.
// Appends G-invariants to `invariants`; uses `counter` for fresh names.
// `aux_pairs` collects (curr, prev) atom refs for each S operator, used
// later to add temporal connection constraints G(X(p_prev) <-> p_curr)
// to the ltlsynt skeleton.
// `safety_invs` collects G(curr && rhs) for the outermost S, and
// G(curr ↔ rhs) for inner (nested) S operators.  The biconditional form
// for inner S lets the auxiliary variable be false at t=0 without forcing
// the inner ψ to hold — only the outermost S's ψ is required at t=0.
// `is_outer` is true for the top-level call and for the first S node
// encountered; false for S operators nested inside another S's phi/psi.
template <NodeType node>
static tref compile_since_trigger_rec(
    tref fm,
    std::vector<tref>& invariants,
    int& counter,
    std::vector<std::pair<tref,tref>>& aux_pairs,
    std::vector<tref>& safety_invs,
    std::vector<tref>& init_conds,
    bool is_outer = true)
{
	using tau = tree<node>;
	const auto& t = tau::get(fm);
	if (!t.has_child()) return fm;

	auto nt = t[0].value.nt;

	// wff_T: φ T ψ = ¬(¬φ S ¬ψ)
	if (nt == tau::wff_T) {
		tref phi     = t[0].first();
		tref psi     = t[0].second();
		tref neg_phi = tau::build_wff_neg(phi);
		tref neg_psi = tau::build_wff_neg(psi);
		tref s_node  = tau::build_wff_S(neg_phi, neg_psi);
		tref s_rewr  = compile_since_trigger_rec<node>(s_node, invariants, counter, aux_pairs, safety_invs, init_conds, is_outer);
		return tau::build_wff_neg(s_rewr);
	}

	// wff_S: φ S ψ
	if (nt == tau::wff_S) {
		tref phi = t[0].first();
		tref psi = t[0].second();

		// Recursively compile nested S/T inside operands first.
		// Any S inside phi or psi is by definition inner (nested), so
		// pass is_outer=false to suppress the always-true requirement
		// and the psi-at-0 initial condition for those sub-operators.
		phi = compile_since_trigger_rec<node>(phi, invariants, counter, aux_pairs, safety_invs, init_conds, /*is_outer=*/false);
		psi = compile_since_trigger_rec<node>(psi, invariants, counter, aux_pairs, safety_invs, init_conds, /*is_outer=*/false);

		// Fresh auxiliary output variable name (o-prefix → controllable output).
		std::string aux_name = "o__ltl_s" + std::to_string(counter++) + "__";

		// Atoms: aux[t]=1  and  aux[t-1]=1
		tref curr = build_bv_eq_aux<node>(aux_name, 0,  1);
		tref prev = build_bv_eq_aux<node>(aux_name, -1, 1);

		// Invariant: G(curr ↔ (ψ ∨ (φ ∧ prev)))
		tref rhs  = tau::build_wff_or(psi, tau::build_wff_and(phi, prev));
		tref inv  = tau::build_wff_always(tau::build_wff_equiv(curr, rhs));
		invariants.push_back(inv);

		// Initial condition: aux[-1] must start as 0 (strong-past LTL semantics).
		// Without this, the system could freely set aux[-1]=1 and trivially satisfy S
		// at t=0 even when ψ never actually held.  As a non-G-wrapped formula this
		// constrains ONLY position 0 of the run (LTL semantics evaluate at position 0).
		tref init_cond = tau::build_wff_neg(prev); // !(aux[t-1]={1})
		invariants.push_back(init_cond);

		if (is_outer) {
			// For the outermost S only: at t=0, S semantics forces ψ to hold
			// directly.  Encoding as an initial constraint on the user-facing
			// io_vars (shifted to t=0) avoids the aux[t-1] reference, which
			// the interpreter's fixpoint pipeline would mis-treat as a
			// G-unrolled seed.
			auto psi_io_vars = tau::get(psi).select_top(is_child<node, tau::io_var>);
			tref psi_at_0 = fm_at_time_point<node>(psi, psi_io_vars, 0);
			init_conds.push_back(psi_at_0);

			// Outermost S safety invariant: G(curr && rhs).
			// The top-level spec must ALWAYS hold, so curr must always be 1.
			// This does NOT propagate rhs backwards in time.
			safety_invs.push_back(tau::build_wff_always(tau::build_wff_and(curr, rhs)));
		} else {
			// Inner S safety invariant: G(curr ↔ rhs).
			// The inner S auxiliary is NOT required to be 1 at all times —
			// it is free to be 0 at t=0 when the outermost S's ψ (not this
			// ψ) is what holds the spec at t=0.  The biconditional correctly
			// tracks the auxiliary's value without imposing the always-true
			// requirement that would otherwise force this ψ at t=0.
			safety_invs.push_back(tau::build_wff_always(tau::build_wff_equiv(curr, rhs)));
		}

		// Record the (curr, prev) pair so the caller can add the temporal
		// connection G(X(p_prev) <-> p_curr) to the ltlsynt skeleton.  Without
		// this, ltlsynt treats p_prev as an independent output proposition and
		// the system can freely set it to 1 at any step t>0, breaking the
		// recurrence semantics of S.
		aux_pairs.emplace_back(curr, prev);

		// Replace φ S ψ with curr ("since holds now").
		return curr;
	}

	// Recurse into operator children (covers wff_and, wff_or, wff_neg,
	// wff_F, wff_U, wff_R, wff_W, wff_always, etc.)
	// Propagate is_outer so that S operators encountered here (e.g. at
	// the top level of a conjunction) retain the same is_outer status.
	const auto& op = t[0];
	size_t nc = op.children_size();
	if (nc == 0) return fm;

	if (nc == 1) {
		tref new_c = compile_since_trigger_rec<node>(op.first(), invariants, counter, aux_pairs, safety_invs, init_conds, is_outer);
		if (new_c == op.first()) return fm;
		return tau::get(tau::wff, tau::get(nt, new_c));
	}
	if (nc == 2) {
		tref new_l = compile_since_trigger_rec<node>(op.first(),  invariants, counter, aux_pairs, safety_invs, init_conds, is_outer);
		tref new_r = compile_since_trigger_rec<node>(op.second(), invariants, counter, aux_pairs, safety_invs, init_conds, is_outer);
		if (new_l == op.first() && new_r == op.second()) return fm;
		return tau::get(tau::wff, tau::get(nt, new_l, new_r));
	}
	if (nc == 3) {
		tref new_a = compile_since_trigger_rec<node>(op.first(),  invariants, counter, aux_pairs, safety_invs, init_conds, is_outer);
		tref new_b = compile_since_trigger_rec<node>(op.second(), invariants, counter, aux_pairs, safety_invs, init_conds, is_outer);
		tref new_c = compile_since_trigger_rec<node>(op.third(),  invariants, counter, aux_pairs, safety_invs, init_conds, is_outer);
		if (new_a == op.first() && new_b == op.second() && new_c == op.third())
			return fm;
		// 3-child case: use build_wff_conditional for wff_conditional,
		// otherwise fall back to initializer_list get.
		if (nt == tau::wff_conditional)
			return tau::build_wff_conditional(new_a, new_b, new_c);
		tref ch3[3] = { new_a, new_b, new_c };
		return tau::get(tau::wff, tau::get(nt, ch3, 3));
	}
	return fm; // arity > 3: leave unchanged
}

// Top-level S/T compilation pass.
// Returns {compiled_formula, safety_formula, init_formula, aux_pairs}.
// compiled_formula is fm unchanged if there are no S/T nodes.
// Otherwise: compiled_formula && G_inv_0 && G_inv_1 && ...
// safety_formula: G(outermost_s && rhs) for the top-level S (always-true
//   requirement), and G(inner_s ↔ rhs) for nested S operators (biconditional
//   tracks the auxiliary without forcing it to 1 at t=0).
// init_formula: psi_at_0 for the outermost S only (strong-past t=0 condition).
// aux_pairs: one entry per S operator: (curr_atom, prev_atom).
// Callers use aux_pairs to add G(X(p_prev) <-> p_curr) to the ltlsynt skeleton.
template <NodeType node>
static std::tuple<tref, tref, tref, std::vector<std::pair<tref,tref>>>
compile_since_trigger(tref fm) {
	using tau = tree<node>;
	if (!has_since_trigger<node>(fm))
		return {fm, tau::_T(), tau::_T(), {}};

	std::vector<tref> invariants;
	std::vector<std::pair<tref,tref>> aux_pairs;
	std::vector<tref> safety_invs;
	std::vector<tref> init_conds;
	int counter = 0;
	tref compiled = compile_since_trigger_rec<node>(fm, invariants, counter, aux_pairs, safety_invs, init_conds);

	tref result = compiled;
	for (tref inv : invariants)
		result = tau::build_wff_and(result, inv);

	tref safety_fm = tau::_T();
	for (tref si : safety_invs)
		safety_fm = tau::build_wff_and(safety_fm, si);

	tref init_fm = tau::_T();
	for (tref ic : init_conds)
		init_fm = tau::build_wff_and(init_fm, ic);

	LOG_DEBUG << "[ltl_aba] S/T compile-away: "
	          << invariants.size() << " auxiliary variable(s) introduced";
	return {result, safety_fm, init_fm, std::move(aux_pairs)};
}

} // namespace idni::tau_lang
