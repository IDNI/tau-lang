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
		// LA-13: `!` used to be accepted only in front of digits, so
		// `!(...)` returned F without consuming the group and `!f` read
		// as F.  Negate a group or a constant structurally; the
		// bookkeeping-AP drop-out below stays polarity-insensitive.
		if (neg && pos < s.size()
		    && (s[pos] == '(' || s[pos] == 't' || s[pos] == 'f')) {
			tref inner = parse_atom();
			const auto& it = tau::get(inner);
			if (it.equals_T()) return tau::_F();
			if (it.equals_F()) return tau::_T();
			return tau::build_wff_neg(inner);
		}
		size_t start = pos;
		while (pos < s.size() && std::isdigit(s[pos])) ++pos;
		if (start == pos) return neg ? tau::_F() : tau::_T(); // no digits
		// LA-N6: a digit run longer than any AP index fits in an int
		// is no user atom; treat it as a bookkeeping AP (T) instead of
		// letting std::stoi throw out of the oracle.
		int idx = pos - start > 9 ? -1
			: std::stoi(s.substr(start, pos - start));
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
// aba_synthesis_feasible (before ltlsynt) quantifies inputs universally,
// the rest existentially, since the env picks inputs adversarially.
// aba_existential_feasible (the oracle) quantifies everything existentially,
// since a guard's own inputs already hold when it fires.

// ── Mechanism 1(a) shadow crosscheck (ocLTL runtime-alignment design) ───────
//
// phi_delta(sigma,rho,D) decides whether some tau EXACTLY EQUALS the given
// sigma/rho (Def. 3's "P_σ(m,x)"), so phi_delta_direct(allfalse,allfalse,D=1)
// disagrees with the solver whenever an atom is a genuine constraint (it
// forces a minterm to zero, contradicting the independent all-false type).
// Wired SHADOW-ONLY: the solver's answer stays authoritative; this only
// measures the agreement rate via ocltl_swap_stats/ocltl_swap_crosscheck.
struct ocltl_swap_counters {
	std::atomic<size_t> total_calls{0}; // aba_existential_feasible's compute() ran (always counted, cheap)
	std::atomic<size_t> eligible{0};    // fm matched match_ocltl_swap_shape (crosscheck-gated)
	std::atomic<size_t> ineligible{0};  // fm did not match (shape or BA gate; crosscheck-gated)
	std::atomic<size_t> agree{0};       // closed form == solver, among eligible
	std::atomic<size_t> disagree{0};    // closed form != solver, among eligible
};

inline ocltl_swap_counters& ocltl_swap_stats() {
	static ocltl_swap_counters c;
	return c;
}

// Env-gated (TAU_PHI_DELTA_CROSSCHECK=1): on a shape match, compares the
// closed form against the solver's result and logs any disagreement (LOG
// channel, never stdout, see AGENTS.md) -- never changes the returned value.
template <NodeType node>
static void ocltl_swap_crosscheck(tref fm, bool solver_result) {
	static const bool enabled = [] {
		const char* v = std::getenv("TAU_PHI_DELTA_CROSSCHECK");
		return v && *v && v[0] != '0';
	}();
	if (!enabled) return;
	auto match = match_ocltl_swap_shape<node>(fm);
	if (!match) {
		ocltl_swap_stats().ineligible.fetch_add(1, std::memory_order_relaxed);
		return;
	}
	ocltl_swap_stats().eligible.fetch_add(1, std::memory_order_relaxed);
	size_t k_sigma = match->dims.d_m + match->dims.d_x;
	std::vector<bool> sigma(size_t{1} << k_sigma, false);
	std::vector<bool> rho(size_t{1} << match->dims.d_y, false);
	bool closed_form = ocltl_phi_delta_direct(
		match->dims, match->atoms, sigma, rho, match->D);
	if (closed_form == solver_result) {
		ocltl_swap_stats().agree.fetch_add(1, std::memory_order_relaxed);
	} else {
		ocltl_swap_stats().disagree.fetch_add(1, std::memory_order_relaxed);
		LOG_ERROR << "[ocltl_phi_delta swap] DISAGREEMENT: solver="
			<< solver_result << " closed_form=" << closed_form
			<< " k_sigma=" << k_sigma << " k_rho=" << match->dims.d_y
			<< " |atoms|=" << match->atoms.size() << " D=" << match->D
			<< " fm=" << LOG_FM(fm);
	}
}

// ── LT-4: joint satisfiability of a conjunction of qlt order atoms ───────────
//
// `aba_existential_feasible`'s qlt fast path eliminates each free variable
// SEPARATELY against the original formula, and `qlt_dlo_qe` only detects a
// contradiction when the same subtree bounds the eliminated variable from both
// sides.  A transitivity chain over three or more variables —
// `o1 < o2 ∧ o2 < o3 ∧ ¬(o1 < o3)` — therefore survives every single
// elimination while being jointly UNSAT, so the oracle passes an infeasible
// strategy edge and the specification comes back falsely REALIZABLE.
//
// `qlt_dlo_qe`'s API returns an interval, not a residual formula, so the
// "eliminate sequentially, substituting the residual" variant of the fix is not
// available.  Instead: qlt's comparisons are a dense linear order, and a
// conjunction of order literals over one is unsatisfiable exactly when the
// implied ≤-graph contains a cycle carrying at least one strict edge.  Build
// that graph over the atoms' operand subtrees — structurally equal operands are
// interned to the same tref, so identity is the right key — and close it.
//
// The check is ONE-DIRECTIONAL by construction: `true` means "provably UNSAT",
// `false` means only "not proven UNSAT".  Disjunctions, `≠` literals and order
// facts between distinct constants are ignored, which can only ever hide a
// contradiction (making the caller fall through to the general solver), never
// invent one.  That is the safe direction: a spurious "infeasible" would turn
// into a spurious UNREALIZABLE verdict.
template <NodeType node>
static bool qlt_order_conj_unsat(tref fm) {
	using tau = tree<node>;

	// Term identity MUST be structural, not tref identity.  `o1[t]:qlt`
	// occurring in `o1 < o2` and again in `!(o1 < o3)` is the same term but not
	// necessarily the same tref — that is exactly why the codebase carries
	// `subtree_map` / `subtree_set` (see extract_data_atoms' "structural
	// equality (subtree_equals)" note, and `qlt_dlo_qe`, which only detects
	// `fv < var && var < fv` across two atoms because its endpoint sets are
	// `subtree_set`).  Keying by raw tref turned the three chain variables into
	// six disconnected graph nodes, so no cycle could ever close.
	std::vector<tref> terms;
	subtree_map<node, int> term_idx;
	auto idx_of = [&](tref n) -> int {
		if (auto it = term_idx.find(n); it != term_idx.end()) return it->second;
		int id = (int)terms.size();
		terms.push_back(n);
		term_idx.emplace(n, id);
		return id;
	};

	// rel: 0 = ignore, 1 = a <= b, 2 = a < b, 3 = a == b.
	std::vector<std::tuple<int,int,int>> rels; // (lhs_idx, rhs_idx, rel)

	std::function<void(tref,bool)> walk = [&](tref n, bool neg) {
		const auto& t = tau::get(n);
		if (!t.has_child()) return;
		auto op = t[0].value.nt;
		if (op == tau::wff_neg) { walk(t[0].first(), !neg); return; }
		// Only descend through connectives that are conjunctive under the
		// current polarity; anything else is ignored (see the one-directional
		// note above).  ¬(A ∧ B) is a disjunction and must NOT be split.
		//
		// wff_and / wff_or are N-ARY: `A && B && C` is ONE node with three
		// children, not a nested pair.  Reading only first()/second() silently
		// dropped every conjunct past the second, which is precisely long
		// enough to hide a three-variable transitivity chain.
		if ((!neg && op == tau::wff_and) || (neg && op == tau::wff_or)) {
			const auto& cop = t[0];
			for (size_t i = 0; i < cop.children_size(); ++i)
				walk(cop.child(i), neg);
			return;
		}
		if (op == tau::wff) { walk(t[0].first(), neg); return; }

		// Order atoms only.  An atom whose BA type is known and is NOT in the
		// omcat/qlt family is skipped; an atom whose type cannot be determined
		// still participates, since operands only ever join the graph when
		// they are the same subtree and a strict-order cycle is a
		// contradiction in any linearly ordered BA.
		{
			size_t atom_ti = find_ba_type<node>(n);
			if (atom_ti != 0 && !pack_type_is_non_aba_omcat<node>(atom_ti)) return;
		}

		tref l = t[0].first();
		tref r = t[0].second();
		if (!l || !r) return;

		// Normalise to (lhs, rhs, rel) with the "n"-prefixed spellings the
		// tau tree uses for already-negated comparisons folded in.
		int rel = 0;
		bool flip = false;
		switch (op) {
		case tau::bf_lt:    rel = 2;              break;  // l <  r
		case tau::bf_lteq:  rel = 1;              break;  // l <= r
		case tau::bf_gt:    rel = 2; flip = true; break;  // r <  l
		case tau::bf_gteq:  rel = 1; flip = true; break;  // r <= l
		case tau::bf_eq:    rel = 3;              break;  // l == r
		case tau::bf_nlt:   rel = 1; flip = true; break;  // !(l<r)  ≡ r <= l
		case tau::bf_nlteq: rel = 2; flip = true; break;  // !(l<=r) ≡ r <  l
		case tau::bf_ngt:   rel = 1;              break;  // !(l>r)  ≡ l <= r
		case tau::bf_ngteq: rel = 2;              break;  // !(l>=r) ≡ l <  r
		default:            return;                       // incl. bf_neq
		}
		if (neg) {
			// Complement: ¬(a<b) ≡ b<=a, ¬(a<=b) ≡ b<a; a negated equality
			// carries no order content.
			if      (rel == 2) { rel = 1; flip = !flip; }
			else if (rel == 1) { rel = 2; flip = !flip; }
			else               return;
		}
		if (flip) { tref tmp = l; l = r; r = tmp; }
		rels.emplace_back(idx_of(l), idx_of(r), rel);
	};
	walk(fm, false);

	const int n = (int)terms.size();
	if (n == 0 || rels.empty()) return false;

	// best[i][j]: 0 = no known relation, 1 = i <= j, 2 = i < j.
	std::vector<int> best((size_t)n * n, 0);
	auto at = [&](int i, int j) -> int& { return best[(size_t)i * n + j]; };
	for (auto& [i, j, rel] : rels) {
		if (rel == 3) {                       // i == j: both directions, ≤
			at(i, j) = std::max(at(i, j), 1);
			at(j, i) = std::max(at(j, i), 1);
		} else {
			at(i, j) = std::max(at(i, j), rel);
		}
	}
	// Transitive closure keeping track of whether a strict edge is on the path.
	for (int k = 0; k < n; ++k)
		for (int i = 0; i < n; ++i) {
			if (!at(i, k)) continue;
			for (int j = 0; j < n; ++j) {
				if (!at(k, j)) continue;
				int w = (at(i, k) == 2 || at(k, j) == 2) ? 2 : 1;
				if (w > at(i, j)) at(i, j) = w;
			}
		}
	// A cycle through a strict edge means some x satisfies x < x.
	for (int i = 0; i < n; ++i)
		if (at(i, i) == 2) return true;
	return false;
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
		// Over a non-aba omcat theory, ask that theory per free variable
		// rather than the general satisfiability path below.
		//
		// LT-4: a per-variable check alone is not enough -- "every variable
		// is individually non-empty" is NOT joint satisfiability.  The
		// per-variable elimination only detects a contradiction when the
		// same subtree appears as both a lower and an upper bound of the
		// variable being eliminated, so a transitivity chain over three or
		// more omcat variables — `o1<o2 ∧ o2<o3 ∧ ¬(o1<o3)` — survives every
		// single elimination while being jointly UNSAT.  The oracle would
		// then pass an infeasible strategy edge and report a false
		// REALIZABLE.
		//
		// Two complementary guards are applied:
		//   * `qlt_order_conj_unsat` proves joint UNSAT for order chains --
		//     it is what actually catches the transitivity case, since
		//     neither the per-variable QE nor `is_non_temp_nso_satisfiable`
		//     does; despite its name it is pack-generic (any dense-order
		//     omcat family), not qlt-specific;
		//   * the free-variable cap below keeps the per-variable path from
		//     claiming SAT in the ≥3-variable cases it cannot decide.
		// With at most two free variables the residual after eliminating one
		// is a constraint on a single remaining variable, so per-variable
		// emptiness and joint emptiness coincide and the fast path is exact.
		// Above that we fall through to the general solver.
		//
		// The cap is a runtime parameter, not a compile-time constant:
		// TAU_LTL_OMCAT_QE_MAX_VARS (default 2).  Setting it higher restores
		// the old — unsound — behaviour and is only useful for measuring
		// what the fast path was buying.

		// Joint check first: it is the only thing that catches a
		// transitivity chain, and it is exact when it fires.
		if (qlt_order_conj_unsat<node>(fm)) return false;

		const trefs& free_vars = tau::get(fm).get_free_vars();
		size_t qe_max_vars = 2;
		if (const char* env_cap = std::getenv("TAU_LTL_OMCAT_QE_MAX_VARS")) {
			int v = std::atoi(env_cap);
			if (v > 0) qe_max_vars = (size_t)v;
		}
		if (!free_vars.empty() && free_vars.size() <= qe_max_vars) {
			bool all_omcat = true;
			for (tref v : free_vars)
				if (!pack_type_is_non_aba_omcat<node>(
					tree<node>::get(v).get_ba_type()))
						{ all_omcat = false; break; }
			if (all_omcat) {
				bool all_determined = true;
				for (tref v : free_vars) {
					if (auto sat = pack_omcat_qe<node>(
						tree<node>::get(v).get_ba_type(), v, fm);
						sat)
					{
						if (!*sat) return false;
					} else { all_determined = false; break; }
				}
				if (all_determined) return true;
			}
		}
		// A free variable whose algebra can always satisfy its own outputs
		// makes the formula feasible without asking the solver.
		for (tref v : tau::get(fm).get_free_vars())
			if (pack_type_output_always_satisfiable<node>(
				tree<node>::get(v).get_ba_type())) return true;
		auto sat = is_non_temp_nso_satisfiable<node>(fm);
		return sat.has_value() && sat.value();
	};
	ocltl_swap_stats().total_calls.fetch_add(1, std::memory_order_relaxed);
	bool result = compute();
	ocltl_swap_crosscheck<node>(fm, result); // shadow-only, see the note above
#ifdef TAU_CACHE
	cache.emplace(fm, result);
#endif // TAU_CACHE
	return result;
}

// Per-step feasibility under an adversarial input: each free stream/time
// instance is quantified in chronological order (earliest shift outermost),
// input instances universally, everything else existentially -- a later
// instance may depend on an earlier one, never the reverse.
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
	auto is_input = [](tref v) {
		return tau::get(v).child_is(tau::io_var) && tau::get(v)[0].is_input_variable();
	};
	// get_io_var_shift returns the lookback magnitude (t-k -> k, t -> 0),
	// so a larger shift means an earlier point in time.
	auto shift = [](tref v) {
		return tau::get(v).child_is(tau::io_var)
			? get_io_var_shift<node>(v) : std::numeric_limits<int_t>::max();
	};
	trefs vars = tau::get(fm).get_free_vars();
	std::sort(vars.begin(), vars.end(), [&](tref a, tref b) {
		if (shift(a) != shift(b)) return shift(a) > shift(b);
		if (is_input(a) != is_input(b)) return is_input(a);
		return get_var_name<node>(a) < get_var_name<node>(b);
	});
	tref q_fm = fm;
	for (auto it = vars.rbegin(); it != vars.rend(); ++it)
		q_fm = is_input(*it) ? tau::build_wff_all(*it, q_fm, false)
		                      : tau::build_wff_ex(*it, q_fm, false);
	// q_fm is closed by construction, so a solving BA can decide it directly
	// through its own quantifier support instead of DNF/Shannon case-split.
	// A nullopt (cvc5 unknown or translation failure) is not a "no": fall
	// through to the general solver rather than reading it as infeasible.
	auto sat_nt = [](tref f) {
		auto sr = is_non_temp_nso_satisfiable<node>(f);
		return sr.has_value() && sr.value();
	};
	bool result;
	if (pack_can_solve<node>(q_fm)) {
		if (auto sat = pack_sat_status<node>(q_fm)) result = *sat;
		else result = sat_nt(q_fm);
	} else result = sat_nt(q_fm);
#ifdef TAU_CACHE
	cache.emplace(fm, result);
#endif // TAU_CACHE
	return result;
}

// Unified feasibility dispatch (code_restruct_suggestion #6).
//
// Picks between existential (∃(i,o). fm) and adversarial-input (∀i.∃o. fm)
// feasibility. The pattern previously duplicated at call sites is now
// centralized here:
//
//   - Pure-input formulas → EXISTENTIAL: pure-input constraints are env
//     assumptions, not system obligations; ∃i captures env freedom.
//   - Non-aba omcat output-only formulas → EXISTENTIAL: no input to be
//     adversarial about.
//   - Input-bearing or non-omcat formulas → aba_synthesis_feasible: env
//     picks inputs adversarially; system needs ∀i.∃o guarantees.
//
// Callers still compute `pure_input` and `has_input` themselves (each has a
// different fast path: single-atom uses is_pure_input_atom; pair uses both),
// so this helper takes them as parameters.  Keeps the core policy in one place.
template <NodeType node>
static bool aba_feasible_dispatch(tref fm, bool pure_input, bool has_input) {
	if (pure_input) return aba_existential_feasible<node>(fm);
	size_t ti = find_ba_type<node>(fm);
	if (pack_type_is_non_aba_omcat<node>(ti) && !has_input)
		return aba_existential_feasible<node>(fm);
	if (pack_type_output_always_satisfiable<node>(ti) && !has_input)
		return aba_existential_feasible<node>(fm);
	return aba_synthesis_feasible<node>(fm);
}

// ── ABA oracle: check if a strategy guard is ABA-feasible ────────────────────
//
// A guard g(i, o) is feasible if there exists some concrete (i, o) satisfying
// it — i.e., existential satisfiability.  Each edge in a Mealy strategy fires
// CONDITIONALLY (for specific input values), so the oracle's job is only to
// verify ABA consistency at that edge, not to prove global synthesis-feasibility.

// Split a parsed guard formula into its top-level disjuncts / conjuncts.
// The guard parser builds plain wff_or / wff_and spines, so a flat walk is
// enough; the helpers exist only so the oracle does not have to re-implement
// the traversal twice.
// wff_or / wff_and are N-ARY: `a | b | c` is one node with three children.
template <NodeType node>
static void collect_guard_or(tref fm, trefs& out) {
	using tau = tree<node>;
	const auto& t = tau::get(fm);
	if (t.has_child() && t[0].value.nt == tau::wff_or) {
		const auto& op = t[0];
		for (size_t i = 0; i < op.children_size(); ++i)
			collect_guard_or<node>(op.child(i), out);
		return;
	}
	out.push_back(fm);
}

template <NodeType node>
static void collect_guard_and(tref fm, trefs& out) {
	using tau = tree<node>;
	const auto& t = tau::get(fm);
	if (t.has_child() && t[0].value.nt == tau::wff_and) {
		const auto& op = t[0];
		for (size_t i = 0; i < op.children_size(); ++i)
			collect_guard_and<node>(op.child(i), out);
		return;
	}
	out.push_back(fm);
}

template <NodeType node>
static bool guard_is_aba_feasible(
    const std::string& guard_label,
    const std::vector<std::string>& aps,
    const std::vector<std::pair<tref, std::string>>& atoms)
{
	using tau = tree<node>;

	// LT-3: this used to hand-lex the label, accepting only '!', digits and
	// '&' and `break`ing on '|' or '('.  Spot prints strategy edge labels as
	// sums of products (e.g. `[0&1 | !0&!1]`), so that lexer
	//   (a) truncated a disjunctive label to its FIRST product — if that
	//       product was infeasible the oracle returned a spurious
	//       UNREALIZABLE for the whole specification; and
	//   (b) produced an EMPTY literal list for a label starting with '(' —
	//       which the per-type check then read as the empty conjunction ⊤,
	//       declaring the edge feasible without checking anything.
	//
	// The production parser (`parse_guard_expr` / `guard_to_aba`) already
	// implements the full grammar, including '|' and parentheses, precisely
	// because ltlsynt emits them.  Reuse it, split the result into top-level
	// disjuncts, and accept the edge iff SOME disjunct passes — which is
	// exactly what "the edge fires when the label holds" means.
	tref guard_fm = guard_to_aba<node>(guard_label, aps, atoms);

	// A label that parses to FALSE ('f', or a product containing both an atom
	// and its negation after bookkeeping APs drop out) is a DEAD edge: the
	// environment can never trigger it, so it is not evidence of
	// infeasibility.  Same convention as the input-only dead-edge check below.
	if (tau::get(guard_fm).equals_F()) return true;

	trefs disjuncts;
	collect_guard_or<node>(guard_fm, disjuncts);

	auto types_seen = formula_type_set<node>::from_atoms(atoms);
	const bool single_type = types_seen.single_type();

	// LA-R1 / LA-R2 / LA-N5: per-input-class semantics.
	//
	// The edge fires under EVERY input class its label admits — the
	// environment picks the inputs, the system only picks the output part.
	// "Some product feasible" (the LT-3 rule) is therefore exact only when
	// all products share one input class; with different pure-input parts
	// the environment can choose the class whose only product has an
	// infeasible output part (LA-R2).  And a product whose input part is
	// infeasible is a DEAD product, not a licence to accept the whole edge
	// (LA-R1: the old code `return true`d on the first dead product).
	//
	// Rule: split each product P_k into its pure-input part I_k and the
	// rest; drop input-dead products; P_k is feasible iff its full literal
	// set is feasible (per BA type: different types are independent
	// variables — LA-N5 applies that partition to the input check too);
	// the edge is feasible iff every live product's input class is COVERED
	// — by itself, by a feasible product whose input literals are a subset
	// of its own (syntactic entailment; this is how Spot's ISOP printing
	// produces overlapping cubes), or, when all atoms share one BA type,
	// semantically: I_k ∧ ¬(∨_{j feasible} I_j) is infeasible.  No live
	// product at all (every product input-dead) is a vacuous edge.

	// Per-product literal record.  `atom` is the underlying comparison,
	// recovered from the parsed literal so that the BA type and the
	// pure-input classification are read off the atom rather than off its
	// negation.
	struct GuardLit { tref lit; tref atom; bool pure_input; };
	struct Product {
		std::vector<GuardLit> lits;
		trefs input_lits;     // pure-input literals (sorted for subset tests)
		bool feasible = false;
	};

	// Conjunction of literals, partitioned by BA type unless the whole atom
	// set is single-typed.  `pick` selects which literals participate.
	auto conj_feasible = [&](const std::vector<GuardLit>& lits,
	                         auto&& pick) -> bool {
		if (single_type) {
			tref conj = tau::_T();
			for (auto& gl : lits)
				if (pick(gl)) conj = tau::build_wff_and(conj, gl.lit);
			return aba_existential_feasible<node>(conj);
		}
		std::map<size_t, tref> per_type;
		for (auto& gl : lits) {
			if (!pick(gl)) continue;
			auto [it, ins] = per_type.try_emplace(
				find_ba_type<node>(gl.atom), tau::_T());
			it->second = tau::build_wff_and(it->second, gl.lit);
		}
		for (auto& [tid, conj] : per_type)
			if (!aba_existential_feasible<node>(conj)) return false;
		return true;
	};

	std::vector<Product> live;
	for (tref d : disjuncts) {
		if (tau::get(d).equals_F()) continue;   // dead product

		trefs raw_lits;
		collect_guard_and<node>(d, raw_lits);

		Product p;
		bool product_is_false = false;
		for (tref l : raw_lits) {
			const auto& lt = tau::get(l);
			if (lt.equals_T()) continue;        // bookkeeping AP
			if (lt.equals_F()) { product_is_false = true; break; }
			tref atom = l;
			if (lt.has_child() && lt[0].value.nt == tau::wff_neg)
				atom = lt[0].first();
			const bool pure_input = is_pure_input_atom<node>(atom);
			p.lits.push_back({l, atom, pure_input});
			if (pure_input) p.input_lits.push_back(l);
		}
		if (product_is_false) continue;         // dead product

		// Input-dead product: the environment can never trigger it.
		if (!p.input_lits.empty()
		    && !conj_feasible(p.lits, [](const GuardLit& gl) {
		           return gl.pure_input; }))
			continue;

		p.feasible = conj_feasible(p.lits, [](const GuardLit&) {
			return true; });
		std::sort(p.input_lits.begin(), p.input_lits.end());
		live.push_back(std::move(p));
	}

	// Every product input-dead (or the label was `f`): vacuous edge.
	if (live.empty()) return true;

	// Coverage.  A feasible product with NO input literals covers every
	// class; otherwise try syntactic subset, then the semantic check.
	tref feasible_inputs_disj = nullptr;   // ∨_{j feasible} I_j (single type)
	for (auto& pj : live) {
		if (!pj.feasible) continue;
		if (pj.input_lits.empty()) return true;
		if (single_type) {
			tref ij = tau::_T();
			for (tref l : pj.input_lits) ij = tau::build_wff_and(ij, l);
			feasible_inputs_disj = feasible_inputs_disj
				? tau::build_wff_or(feasible_inputs_disj, ij) : ij;
		}
	}
	for (auto& pk : live) {
		if (pk.feasible) continue;
		bool covered = false;
		for (auto& pj : live) {
			if (!pj.feasible) continue;
			if (std::includes(pk.input_lits.begin(), pk.input_lits.end(),
			                  pj.input_lits.begin(), pj.input_lits.end())) {
				covered = true;
				break;
			}
		}
		if (!covered && single_type && feasible_inputs_disj) {
			tref ik = tau::_T();
			for (tref l : pk.input_lits) ik = tau::build_wff_and(ik, l);
			tref uncovered = tau::build_wff_and(ik,
				tau::build_wff_neg(feasible_inputs_disj));
			covered = !aba_existential_feasible<node>(uncovered);
		}
		if (!covered && !single_type) {
			// §13 / Batch O8: exact coverage for MIXED-type guards.
			// The single-type semantic check above cannot run (one
			// existential query cannot span independent BA types),
			// so the syntactic subset test used to be the last
			// word — a false UNREALIZABLE whenever the feasible
			// input classes only jointly cover I_k.  Expand
			// I_k ∧ ⋀_{j feasible} ¬I_j into products of literals
			// (¬I_j = ∨_{l ∈ I_j} ¬l distributed); a product is
			// feasible iff each BA type's sub-conjunction is
			// (independent variables), and I_k is COVERED iff no
			// product is feasible.  The expansion is capped by the
			// runtime parameter max_cover_products; beyond it the
			// pre-O8 syntactic verdict stands (logged) — sound, at
			// worst incomplete.
			auto negate_lit = [&](tref l) -> tref {
				const auto& lt = tau::get(l);
				if (lt.has_child()
					&& lt[0].value.nt == tau::wff_neg)
					return lt[0].first();
				return tau::build_wff_neg(l);
			};
			std::vector<trefs> products{ pk.input_lits };
			bool blown = false;
			for (auto& pj : live) {
				if (!pj.feasible) continue;
				// A feasible product with no input literals
				// covers everything and already returned above.
				std::vector<trefs> next;
				for (auto& prod : products) {
					for (tref l : pj.input_lits) {
						if (max_cover_products
							&& next.size() >=
							max_cover_products) {
							blown = true;
							break;
						}
						trefs np = prod;
						np.push_back(negate_lit(l));
						next.push_back(std::move(np));
					}
					if (blown) break;
				}
				if (blown) break;
				products = std::move(next);
			}
			if (blown) {
				LOG_WARNING << "[ltl_aba] mixed-type coverage "
					"expansion exceeded "
					<< max_cover_products
					<< " products (--max-cover-products / "
					"`set maxcoverproducts`, 0 = "
					"unlimited); keeping the syntactic "
					"verdict for this input class -- the "
					"edge may be refused although it is "
					"coverable (false UNREALIZABLE at "
					"worst)\n";
			} else {
				bool some_feasible = false;
				for (auto& prod : products) {
					std::vector<GuardLit> lits;
					lits.reserve(prod.size());
					for (tref l : prod) {
						tref atom = l;
						const auto& lt = tau::get(l);
						if (lt.has_child()
							&& lt[0].value.nt ==
								tau::wff_neg)
							atom = lt[0].first();
						lits.push_back({l, atom, true});
					}
					if (conj_feasible(lits,
						[](const GuardLit&) {
							return true; })) {
						some_feasible = true;
						break;
					}
				}
				covered = !some_feasible;
			}
		}
		if (!covered) return false;
	}
	return true;
}


// True if the formula contains any io_var with a non-zero shift (lookback).
// Such atoms represent past-time values and must NOT be fed into the safety
// synthesis pipeline for individual feasibility — the safety pipeline's
// initial-value convention makes lookback atoms appear unconditionally
// infeasible from t=0, which is incorrect in the LTL context.
//
// That "initial lookback values are 0" convention is one of the three t=0
// conventions in the codebase; the authoritative statement of all three
// (this one, Algorithm D's initial memory ρ₀ = type_of(0), and the LA-N3
// inner-S auxiliary anchor S(-1) = false) lives at `alg_d::initial_memory`
// in algorithm_d_game.h.
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

// True if `atom` is a ground equality (bf_eq) over exactly one io_var: the
// shape the pairwise consistency fast path can decide without a solver
// call. Positional atoms (no uniform shift) are excluded.
template <NodeType node>
static bool atom_is_ground_single_stream_eq(tref atom) {
	using tau = tree<node>;
	const auto& t = tau::get(atom);
	if (!t.has_child() || t[0].value.nt != tau::bf_eq) return false;
	if (tau::get(atom).select_top(is_child<node, tau::io_var>).size() != 1)
		return false;
	return atom_uniform_shift<node>(atom).has_value();
}

// Two ground single-stream equalities over the same io_var/shift agree iff
// they carry the same constant; distinct constants are infeasible (forbid),
// the same constant is trivially co-satisfiable. Returns false (fall
// through to the solver) whenever the shape doesn't provably match.
template <NodeType node>
static bool ground_eq_pair_syntactically_infeasible(tref a, tref b) {
	using tau = tree<node>;
	if (!atom_is_ground_single_stream_eq<node>(a)
	    || !atom_is_ground_single_stream_eq<node>(b))
		return false;
	if (atom_io_var_names<node>(a) != atom_io_var_names<node>(b)) return false;
	if (*atom_uniform_shift<node>(a) != *atom_uniform_shift<node>(b)) return false;
	return !tau::subtree_equals(a, b);
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

// Exhaustive DPLL walk over positive subsets of size >= 3: cost scales with
// the number of FEASIBLE subsets visited, not just infeasible ones. Fallback
// for groups too large for the mus enumeration's explored bitset.
template <NodeType node>
static void extend_consistency_positive_k_ary_walk(
    const std::vector<std::pair<tref, std::string>>& atoms,
    std::string& skeleton,
    std::vector<std::string>* out_constraints,
    std::vector<std::vector<int>>& existing_forbid_sets,
    const std::function<bool(const std::vector<int>&)>& is_subsumed)
{
	using tau = tree<node>;
	const int n = static_cast<int>(atoms.size());

	// LT-17: the walk performs Θ(2^n) synthesis checks when the atoms are
	// mostly jointly feasible (supersets of an infeasible set are pruned,
	// but feasible sets prune nothing). Cap the checks at the runtime
	// parameter `max_consistency_subsets` (0 = unlimited) and skip the
	// rest: sound (the per-edge oracle still catches any jointly
	// infeasible guard), at worst incomplete (a false UNREALIZABLE if
	// ltlsynt picks such an edge — D3 = skip + log, never throw).
	size_t checks_spent = 0;
	bool cap_fired = false;

	// DPLL walk over positive subsets of size ≥ 3.
	std::function<void(int, tref, std::vector<int>&)> walk =
	    [&](int idx, tref prefix_body, std::vector<int>& sel) {
		if (cap_fired) return;
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
			if (max_consistency_subsets
				&& checks_spent >= max_consistency_subsets) {
				cap_fired = true;
				LOG_WARNING << "[ltl_aba] k-ary consistency "
					"walk capped after "
					<< checks_spent << " subset checks "
					"(--max-consistency-subsets / `set "
					"maxsubsets`, 0 = unlimited); "
					"remaining subsets skipped -- the "
					"verdict stays sound (the oracle "
					"checks every strategy edge) but may "
					"be a false UNREALIZABLE\n";
				return;
			}
			++checks_spent;
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

// MARCO-style enumeration of minimal infeasible k-subsets (k>=3): cost
// scales with the number of infeasible subsets found (grows to a maximal
// feasible witness instead of exhaustive descent). Requires n <= 22.
template <NodeType node>
static void extend_consistency_positive_k_ary_mus(
    const std::vector<std::pair<tref, std::string>>& atoms,
    std::string& skeleton,
    std::vector<std::string>* out_constraints,
    std::vector<std::vector<int>>& existing_forbid_sets,
    const std::function<bool(const std::vector<int>&)>& is_subsumed)
{
	using tau = tree<node>;
	const int n = static_cast<int>(atoms.size());
	const uint32_t universe = (1u << n) - 1;

	auto build_conj = [&](uint32_t mask) {
		tref body = tau::_T();
		for (int i = 0; i < n; ++i)
			if (mask & (1u << i)) body = tau::build_wff_and(body, atoms[i].first);
		return body;
	};
	auto for_each_submask = [](uint32_t mask, auto&& f) {
		uint32_t sub = mask;
		while (true) {
			f(sub);
			if (sub == 0) break;
			sub = (sub - 1) & mask;
		}
	};

	std::vector<bool> explored(std::size_t(1) << n, false);
	std::vector<uint32_t> mus_masks;

	// Same cap as extend_consistency_positive_k_ary_walk: one synthesis
	// check per seed. Capping stays sound (the per-edge oracle still
	// catches an infeasible guard) but can yield a false UNREALIZABLE.
	size_t checks_spent = 0;
	bool cap_fired = false;
	auto warn_capped = [&]() {
		if (cap_fired) return;
		cap_fired = true;
		LOG_WARNING << "[ltl_aba] k-ary consistency "
			"walk capped after "
			<< checks_spent << " subset checks "
			"(--max-consistency-subsets / `set "
			"maxsubsets`, 0 = unlimited); "
			"remaining subsets skipped -- the "
			"verdict stays sound (the oracle "
			"checks every strategy edge) but may "
			"be a false UNREALIZABLE\n";
	};
	auto over_cap = [&]() {
		return max_consistency_subsets
			&& checks_spent >= max_consistency_subsets;
	};
	auto feasible_checked = [&](tref t) {
		++checks_spent;
		return aba_synthesis_feasible<node>(t);
	};

	auto process_seed = [&](uint32_t seed) {
		if (over_cap()) { warn_capped(); return; }
		tref conj = build_conj(seed);
		if (feasible_checked(conj)) {
			// The check just spent may already be at the cap: if `seed` is
			// already maximal, the growth loop below makes no further calls
			// and would never otherwise notice.
			if (over_cap()) warn_capped();
			// Grow to a maximal feasible set; every subset of it is feasible too.
			uint32_t max_set = seed;
			tref max_conj = conj;
			for (int i = 0; i < n; ++i) {
				if (max_set & (1u << i)) continue;
				if (over_cap()) { warn_capped(); break; }
				tref grown = tau::build_wff_and(max_conj, atoms[i].first);
				if (feasible_checked(grown)) {
					max_set |= (1u << i);
					max_conj = grown;
				}
			}
			for_each_submask(max_set, [&](uint32_t s) { explored[s] = true; });
		} else {
			// Same as the grow branch: the check just spent may already be
			// at the cap, before the shrink loop gets a chance to test it.
			if (over_cap()) warn_capped();
			// Shrink by deletion, never below size 3 (the oracle floor).
			uint32_t cur = seed;
			bool shrink_complete = true;
			for (int i = 0; i < n; ++i) {
				if (!(cur & (1u << i))) continue;
				if (std::popcount(cur) <= 3) break;
				if (over_cap()) { warn_capped(); shrink_complete = false; break; }
				uint32_t candidate = cur & ~(1u << i);
				if (!feasible_checked(build_conj(candidate)))
					cur = candidate;
			}
			// A cap-cut-short shrink is not a proven MUS -- `cur` is whatever
			// size it had when the cap fired. Recording it would under-constrain
			// the skeleton instead of leaving the gap for the oracle to catch.
			if (shrink_complete) {
				uint32_t complement = universe & ~cur;
				for_each_submask(complement, [&](uint32_t s) { explored[cur | s] = true; });
				mus_masks.push_back(cur);
			}
		}
	};

	for (int k = n; k >= 3; --k) {
		uint32_t mask = (1u << k) - 1;
		const uint32_t last = mask << (n - k);
		while (true) {
			if (!explored[mask]) process_seed(mask);
			if (mask == last) break;
			uint32_t c = mask & (~mask + 1u);
			uint32_t r = mask + c;
			mask = (((r ^ mask) >> 2) / c) | r;
		}
	}

	// Sorting by the walk's own emission-order key (atom 0 most significant)
	// keeps every subset ahead of its supersets, so is_subsumed correctly
	// drops any mask here that turns out to be a superset of an earlier one.
	auto walk_order_key = [&](uint32_t mask) {
		uint32_t key = 0;
		for (int i = 0; i < n; ++i)
			if (mask & (1u << i)) key |= (1u << (n - 1 - i));
		return key;
	};
	std::sort(mus_masks.begin(), mus_masks.end(),
	    [&](uint32_t a, uint32_t b) { return walk_order_key(a) < walk_order_key(b); });
	for (uint32_t m : mus_masks) {
		std::vector<int> sel;
		for (int i = 0; i < n; ++i) if (m & (1u << i)) sel.push_back(i);
		if (is_subsumed(sel)) continue;
		// Skip if any atom is pure-output-lookback: uninitialized lookback
		// is F at t=0, which makes such atoms spuriously infeasible under G-wrap.
		bool any_pure_out_lb = false;
		for (int i : sel) {
			bool is_mixed = atom_has_any_input<node>(atoms[i].first);
			bool pure_out_lb = atom_has_lookback<node>(atoms[i].first) && !is_mixed;
			if (pure_out_lb) { any_pure_out_lb = true; break; }
		}
		if (any_pure_out_lb) continue;
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
	}
}

template <NodeType node>
static void extend_consistency_positive_k_ary(
    const std::vector<std::pair<tref, std::string>>& group,
    std::string& skeleton,
    std::vector<std::string>* out_constraints)
{
	using tau = tree<node>;
	const std::vector<std::pair<tref, std::string>>& atoms = group;
	const int n = static_cast<int>(atoms.size());
	if (n < 3) return;  // pairwise already handled by caller

	// Collect existing forbid patterns (as sets of atom indices) so we can
	// skip subsumed k-subsets.
	std::vector<std::vector<int>> existing_forbid_sets;
	if (out_constraints) {
		for (const auto& c : *out_constraints) {
			// Parse "G(!(name1 && name2 && ...))" into a set of
			// indices. LT-9: token-boundary matching -- a raw
			// substring find read `p1` inside `p10`/`p11`, so with
			// >= 11 atoms a forbid was mis-parsed and a needed
			// constraint wrongly skipped as subsumed.
			auto contains_name = [&](const std::string& hay,
					const std::string& name) {
				size_t pos = 0;
				auto is_word = [](char ch) {
					return std::isalnum(
						(unsigned char) ch)
						|| ch == '_';
				};
				while ((pos = hay.find(name, pos))
					!= std::string::npos) {
					bool l_ok = pos == 0
						|| !is_word(hay[pos - 1]);
					size_t end = pos + name.size();
					bool r_ok = end >= hay.size()
						|| !is_word(hay[end]);
					if (l_ok && r_ok) return true;
					pos = end;
				}
				return false;
			};
			std::vector<int> idxs;
			for (int i = 0; i < n; ++i) {
				if (contains_name(c, atoms[i].second))
					idxs.push_back(i);
			}
			if (idxs.size() >= 2) existing_forbid_sets.push_back(std::move(idxs));
		}
	}
	std::function<bool(const std::vector<int>&)> is_subsumed =
	    [&](const std::vector<int>& cand) {
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

	// The mus enumeration's explored bitset is 2^n entries, unallocatable
	// much past n=22, so groups above that bound fall back to the walk.
	const bool use_walk = n > 22;
	if (use_walk) {
		// Feasibility is downward-closed: check the whole group up front,
		// since the walk has no cheap way to notice a feasible group
		// before paying for the Theta(2^n) descent.
		tref all = tau::_T();
		for (const auto& a : atoms)
			all = tau::build_wff_and(all, a.first);
		if (aba_synthesis_feasible<node>(all)) return;
		extend_consistency_positive_k_ary_walk<node>(
		    atoms, skeleton, out_constraints, existing_forbid_sets, is_subsumed);
	} else
		extend_consistency_positive_k_ary_mus<node>(
		    atoms, skeleton, out_constraints, existing_forbid_sets, is_subsumed);
}

// Ties shifted instances of one signal together (group_shift_families).
// lo is aligned to hi's step; pp/pn/np/nn = SAT(lo'&hi/lo'&!hi/!lo'&hi/!lo'&!hi)
// select: pp&&nn -> equivalent, pn&&np -> complementary, pp&&np&&nn -> lo=>hi,
// pp&&pn&&nn -> hi=>lo, pn&&np&&nn -> exclusion, else -> none.
// Input-only pairs go to input_assumptions; everything else to `skeleton`.
template <NodeType node>
static void add_shift_chain_constraints(
    const std::vector<std::pair<tref, std::string>>& atoms,
    std::string& skeleton,
    std::string& input_assumptions,
    std::vector<std::string>* out_constraints = nullptr)
{
	using tau = tree<node>;
	int solver_calls = 0, emitted = 0;

	auto families = group_shift_families<node>(atoms);
	for (auto& [key, idxs] : families) {
		for (size_t p = 0; p < idxs.size(); ++p) {
			for (size_t q = p + 1; q < idxs.size(); ++q) {
				size_t ia = idxs[p], ib = idxs[q];
				tref a_fm = atoms[ia].first, b_fm = atoms[ib].first;
				int_t sa = *atom_uniform_shift<node>(a_fm);
				int_t sb = *atom_uniform_shift<node>(b_fm);
				if (sa == sb) continue; // same reference frame -- not a chain pair

				bool a_is_lo = sa < sb;
				tref lo = a_is_lo ? a_fm : b_fm;
				tref hi = a_is_lo ? b_fm : a_fm;
				const std::string& lo_prop = a_is_lo ? atoms[ia].second : atoms[ib].second;
				const std::string& hi_prop = a_is_lo ? atoms[ib].second : atoms[ia].second;
				int_t delta = a_is_lo ? sb - sa : sa - sb;

				// Align lo to hi's reference step by substituting each of lo's
				// io_vars with hi's identically-named io_var subtree (the family
				// key guarantees the same name set) rather than rebuilding an
				// offset from scratch, so the aligned tree matches hi's own
				// io_var representation exactly.
				auto lo_io_vars = tau::get(lo).select_top(is_child<node, tau::io_var>);
				auto hi_io_vars = tau::get(hi).select_top(is_child<node, tau::io_var>);
				subtree_map<node, tref> align;
				for (tref lv : lo_io_vars) {
					const std::string& name = get_var_name<node>(lv);
					for (tref hv : hi_io_vars)
						if (get_var_name<node>(hv) == name) { align[lv] = hv; break; }
				}
				tref lo_aligned = rewriter::replace<node>(lo, align);
				tref not_lo = tau::build_wff_neg(lo_aligned);
				tref not_hi = tau::build_wff_neg(hi);

				bool pp = aba_existential_feasible<node>(tau::build_wff_and(lo_aligned, hi));
				bool pn = aba_existential_feasible<node>(tau::build_wff_and(lo_aligned, not_hi));
				bool np = aba_existential_feasible<node>(tau::build_wff_and(not_lo, hi));
				bool nn = aba_existential_feasible<node>(tau::build_wff_and(not_lo, not_hi));
				solver_calls += 4;

				std::string x_lo = lo_prop;
				for (int_t k = 0; k < delta; ++k) x_lo = "X(" + x_lo + ")";

				std::string constraint;
				if (pp && nn && !pn && !np)
					constraint = "G(" + x_lo + " <-> " + hi_prop + ")";
				else if (!pp && !nn && pn && np)
					constraint = "G(" + x_lo + " <-> !" + hi_prop + ")";
				else if (pp && np && nn && !pn)
					constraint = "G(" + x_lo + " -> " + hi_prop + ")";
				else if (pp && pn && nn && !np)
					constraint = "G(" + hi_prop + " -> " + x_lo + ")";
				else if (!pp && pn && np && nn)
					constraint = "G(" + x_lo + " -> !" + hi_prop + ")";
				else
					continue; // e.g. all four satisfiable -- instances stay independent

				if (is_pure_input_atom<node>(lo) && is_pure_input_atom<node>(hi)) {
					if (!input_assumptions.empty()) input_assumptions += " && ";
					input_assumptions += constraint;
				} else {
					skeleton += " && " + constraint;
				}
				if (out_constraints) out_constraints->push_back(constraint);
				++emitted;
			}
		}
	}
	LOG_DEBUG << "[ltl_aba] shift-chain constraints: " << emitted
	          << " emitted, " << solver_calls << " solver call(s)";
}

template <NodeType node>
static void add_consistency_constraints(
    const std::vector<std::pair<tref, std::string>>& atoms,
    std::string& skeleton,
    std::vector<std::string>* out_constraints = nullptr,
    bool polarity_complete = false,
    std::string seed_input_assumptions = "")
{
	using tau = tree<node>;

	// Snapshot original skeleton to detect G(!atom) constraints from the
	// actual formula (not from constraints we are about to add).
	const std::string original_skeleton = skeleton;

	// Input-only infeasibility constraints are env assumptions, not requirements.
	// Collect them separately and wrap the skeleton with (assumptions) -> (...)
	// so ltlsynt treats them as environment guarantees, not system obligations.
	// Seeded from the caller's own input-only constraints (e.g. shift-chain)
	// so the whole set is combined behind a single implication wrap below.
	std::string input_assumptions = std::move(seed_input_assumptions);
	int fast_path_pairs = 0;

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
				// LT-18: this block is only entered when the atom
				// is NOT pure-input, so pass false directly.
				bool feasible = pure_out_lookback
				    || aba_feasible_dispatch<node>(atoms[i].first,
				                                   /*pure_input=*/false, is_mixed);
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
			// Syntactic fast path (ground_eq_pair_syntactically_infeasible):
			// two distinct ground equalities over the same stream/shift are
			// provably infeasible without a solver call.
			bool infeasible;
			if (ground_eq_pair_syntactically_infeasible<node>(
			        atoms[i].first, atoms[j].first)) {
				infeasible = true;
				++fast_path_pairs;
			} else {
				tref conj = tau::build_wff_and(atoms[i].first, atoms[j].first);
				bool pure_input =
				    is_pure_input_atom<node>(atoms[i].first) ||
				    is_pure_input_atom<node>(atoms[j].first);
				bool has_input = atom_has_any_input<node>(atoms[i].first)
				              || atom_has_any_input<node>(atoms[j].first);
				infeasible = !aba_feasible_dispatch<node>(conj, pure_input, has_input);
			}
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
				tref neg_i = tau::build_wff_neg(atoms[i].first);
				for (size_t j = i + 1; j < atoms.size(); ++j) {
					if (is_pure_input_atom<node>(atoms[j].first)) continue;
					if (find_ba_type<node>(atoms[i].first)
					    != find_ba_type<node>(atoms[j].first)) continue;
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
	LOG_DEBUG << "[ltl_aba] pairwise consistency: " << fast_path_pairs
	          << " pair(s) decided by the ground-equality fast path (no solver call)";
}

// ── step-counter encoding of positional atoms (max-position hoisting) ──
// A binary step counter guards each hoisted conjunct C (collect_hoist_
// conjuncts) at its own max position k_max: io_var x[j] in C becomes x[t]
// (j==k_max) or x[t-(k_max-j)], reparsed as C', guarded by
// G(minterm(k_max) -> C').
// A relativized atom identical to an existing one merges into it (one prop
// per formula); atoms from different conjuncts that coincidentally match
// text stay separate, one prop per position, since pairwise mutual-
// exclusion forbids rely on that.
// Returns the extra skeleton conjuncts; output_props gains the counter bits.
// counter_relativized_props gains every prop this pass resolves a hoisted
// conjunct's atom to -- its "t" means the counter's own absolute step, not
// formula_time_point's lookback-shifted one (table_step_provider::produce
// grounds it accordingly).
template <NodeType node>
static std::string apply_step_counter_encoding(
    const std::vector<tref>& hoist_conjuncts,
    std::vector<std::pair<tref, std::string>>& atoms,
    std::vector<std::string>& input_props,
    std::vector<std::string>& output_props,
    int_t& out_max_pos,
    std::set<std::string>& counter_relativized_props)
{
	using tau = tree<node>;

	if (hoist_conjuncts.empty()) return "";

	// Per-conjunct k_max (its own maximum position), and the global maximum
	// across every hoisted conjunct, which sizes the counter.
	std::vector<int_t> k_max(hoist_conjuncts.size(), -1);
	int_t max_pos = -1;
	for (size_t c = 0; c < hoist_conjuncts.size(); ++c) {
		for (auto& [a, name] : atoms)
			if (atom_is_positional<node>(a) && contains<node>(hoist_conjuncts[c], a))
				k_max[c] = std::max(k_max[c], atom_max_position<node>(a));
		max_pos = std::max(max_pos, k_max[c]);
	}
	out_max_pos = max_pos;

	const int_t clamp = max_pos + 1; // parked state, held forever past the trace
	int w = 1;
	while ((int_t(1) << w) < clamp + 1) ++w;

	std::vector<std::string> bits(w);
	for (int b = 0; b < w; ++b)
		bits[b] = "o__ltl_ctr" + std::to_string(b) + "__";
	for (auto& b : bits) output_props.push_back(b);

	auto minterm = [&](int_t value) {
		std::string body;
		for (int b = 0; b < w; ++b) {
			if (!body.empty()) body += " & ";
			body += ((value >> b) & 1) ? bits[b] : ("!" + bits[b]);
		}
		return body;
	};

	std::string extra;
	// Initial condition: counter = 0 at t=0 (bare, un-G'd -- the S/T init convention).
	extra += " & " + minterm(0);

	// Ripple-carry +1, frozen once the counter reaches `clamp`.
	std::string is_clamped = minterm(clamp);
	for (int b = 0; b < w; ++b) {
		std::string carry = "1";
		for (int k = 0; k < b; ++k) carry += " & " + bits[k];
		std::string incr_bit = "(" + bits[b] + " ^ (" + carry + "))";
		std::string next_bit = "((" + is_clamped + " & " + bits[b] + ") | "
		                        "(!(" + is_clamped + ") & " + incr_bit + "))";
		extra += " & G(X(" + bits[b] + ") <-> " + next_bit + ")";
	}

	// Snapshot of the original positional atoms, taken before any mutation.
	// An atom shared by more than one hoisted conjunct is the same
	// underlying fact and resolves to one shared prop across all of them.
	std::vector<tref> orig_pos_atoms;
	for (auto& [a, name] : atoms)
		if (atom_is_positional<node>(a)) orig_pos_atoms.push_back(a);
	std::vector<std::string> orig_prop(orig_pos_atoms.size());
	std::vector<bool> orig_done(orig_pos_atoms.size(), false);

	// Marks atoms[i] as hoist-derived (added by some conjunct's rewrite);
	// never a merge target for a different original atom's rewrite.
	std::vector<bool> hoist_derived(atoms.size(), false);
	for (size_t i = 0; i < atoms.size(); ++i)
		if (atom_is_positional<node>(atoms[i].first)) hoist_derived[i] = true;

	// Fresh prop names come from a monotonic counter, not atoms.size(): a
	// later conjunct's addition could land at a size vacated by an earlier
	// drop and repeat a name.
	size_t next_prop_idx = atoms.size();

	// Rewrites name[j] to name[t] (j==km) or name[t-(km-j)] textually, over
	// every positional io_var found directly on `n` (not via `atoms`, which
	// may already have dropped it for an earlier conjunct).
	auto relativize_text = [&](tref n, int_t km) {
		std::string txt = tau::get(n).to_str();
		for (tref v : tau::get(n).select_top(is_child<node, tau::io_var>)) {
			if (!is_io_initial<node>(v)) continue;
			const std::string& vname = get_var_name<node>(v);
			int_t j = get_io_time_point<node>(v);
			std::string from = vname + "[" + std::to_string(j) + "]";
			std::string to = (j == km) ? (vname + "[t]")
			    : (vname + "[t-" + std::to_string(km - j) + "]");
			size_t at = 0;
			while ((at = txt.find(from, at)) != std::string::npos) {
				txt.replace(at, from.size(), to);
				at += to.size();
			}
		}
		typename tau::get_options opts;
		opts.parse.start = tau::wff;
		tref rel = tau::get(txt, std::move(opts));
		if (!rel) return rel;
		// A bare wff parse leaves io_vars unclassified (transform_io_var
		// later rejects that); resolve them here by name-prefix
		// direction, the way build_carrier_eq_aux does.
		return resolve_io_vars<node>(
			*definitions<node>::instance().get_io_context(), rel);
	};

	auto drop_prop = [](std::vector<std::string>& props, const std::string& name) {
		auto it = std::find(props.begin(), props.end(), name);
		if (it != props.end()) props.erase(it);
	};

	// Renders C's Boolean shape into skeleton text, matching each atom leaf
	// against `orig_pos_atoms` at its ORIGINAL, pre-rewrite level -- not via
	// find_prop's relativized-text lookup, which can't disambiguate atoms
	// the "stay separate" rule deliberately keeps distinct despite identical
	// text.
	std::function<std::string(tref)> build_guard_skel = [&](tref n) -> std::string {
		for (size_t oi = 0; oi < orig_pos_atoms.size(); ++oi)
			if (tau::subtree_equals(orig_pos_atoms[oi], n)) return orig_prop[oi];
		const auto& t = tau::get(n);
		if (!t.has_child()) return "t";
		auto nt = t[0].value.nt;
		const auto& inner = t[0];
		switch (nt) {
		case tau::wff_t: return "1";
		case tau::wff_f: return "0";
		case tau::wff_neg:
			return "!" + build_guard_skel(inner.first());
		case tau::wff_and:
			return "(" + build_guard_skel(inner.first())
			     + " & " + build_guard_skel(inner.second()) + ")";
		case tau::wff_or:
			return "(" + build_guard_skel(inner.first())
			     + " | " + build_guard_skel(inner.second()) + ")";
		case tau::wff_xor:
			return "(" + build_guard_skel(inner.first())
			     + " ^ " + build_guard_skel(inner.second()) + ")";
		case tau::wff_imply:
			return "(" + build_guard_skel(inner.first())
			     + " -> " + build_guard_skel(inner.second()) + ")";
		case tau::wff_equiv:
			return "(" + build_guard_skel(inner.first())
			     + " <-> " + build_guard_skel(inner.second()) + ")";
		case tau::wff_rimply:
			return "(" + build_guard_skel(inner.second())
			     + " -> " + build_guard_skel(inner.first()) + ")";
		case tau::wff_conditional:
			return "((" + build_guard_skel(inner.first())
			     + " -> " + build_guard_skel(inner.second())
			     + ") & (!" + build_guard_skel(inner.first())
			     + " -> " + build_guard_skel(inner.third()) + "))";
		default:
			// Scope validation (collect_hoist_conjuncts) guarantees every
			// leaf reachable here is one of C's own positional atoms,
			// matched above; unreached in practice.
			return "1";
		}
	};

	for (size_t c = 0; c < hoist_conjuncts.size(); ++c) {
		tref C = hoist_conjuncts[c];
		int_t km = k_max[c];

		// Resolve each of C's own original positional atoms to a prop:
		// reuse a shared atom's prop already resolved by an earlier
		// conjunct, merge into a naturally-occurring atom, or add fresh.
		for (size_t oi = 0; oi < orig_pos_atoms.size(); ++oi) {
			tref a_orig = orig_pos_atoms[oi];
			if (!contains<node>(C, a_orig)) continue;
			if (orig_done[oi]) continue; // already resolved via a shared conjunct

			tref rel = relativize_text(a_orig, km);
			assert(rel != nullptr
				&& "apply_step_counter_encoding: atom relativization failed");

			// Compare against a resolved COPY, not the stored tref:
			// mutating atoms[k].first would break its match against this
			// atom's other, unrelativized occurrences elsewhere in the formula.
			size_t existing = atoms.size();
			for (size_t k = 0; k < atoms.size(); ++k) {
				if (hoist_derived[k]) continue;
				tref natural_resolved = resolve_io_vars<node>(
					*definitions<node>::instance().get_io_context(),
					atoms[k].first);
				if (tau::subtree_equals(natural_resolved, rel)) {
					existing = k; break;
				}
			}
			std::string pname;
			if (existing != atoms.size()) {
				pname = atoms[existing].second;
			} else {
				pname = "p" + std::to_string(next_prop_idx++);
				atoms.emplace_back(rel, pname);
				hoist_derived.push_back(true);
				bool pure_input = is_pure_input_atom<node>(rel);
				if (pure_input) input_props.push_back(pname);
				else output_props.push_back(pname);
				// A fresh OUTPUT prop is pinned false outside its own step
				// (nothing else references it, and the generic guard alone
				// wouldn't stop it holding on the trailing silent self-loop).
				// An input prop is never pinned this way: its truth is the
				// environment's choice, tying it to the counter would
				// over-constrain.
				if (!pure_input)
					extra += " & G(" + minterm(km) + " | !" + pname + ")";
			}
			orig_prop[oi] = pname;
			orig_done[oi] = true;
			counter_relativized_props.insert(pname);
		}

		// Drop C's own original positional atoms still present in `atoms`
		// (a shared one may already be gone, dropped while resolving an
		// earlier conjunct that also referenced it): their occurrence site
		// then resolves to "1" when the outer skeleton walk revisits it.
		for (size_t i = 0; i < atoms.size(); ) {
			if (atom_is_positional<node>(atoms[i].first)
			    && contains<node>(C, atoms[i].first)) {
				drop_prop(input_props, atoms[i].second);
				drop_prop(output_props, atoms[i].second);
				atoms.erase(atoms.begin() + (long)i);
				hoist_derived.erase(hoist_derived.begin() + (long)i);
			} else ++i;
		}

		// Guard the whole conjunct (its Boolean shape, atoms substituted
		// with their resolved props) at k_max.
		extra += " & G((" + minterm(km) + ") -> " + build_guard_skel(C) + ")";
	}

	return extra;
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
struct ltl_aba_solution {
	std::vector<std::pair<tref, std::string>> atoms;
	std::vector<std::string> input_props;
	std::vector<std::string> output_props;
	std::string skeleton;         // skeleton sent to ltlsynt
	std::vector<std::string> shift_chain_constraints;
	std::vector<std::string> consistency_constraints;
	hoa_automaton aut;
	// Set only when apply_step_counter_encoding rewrote positional atoms
	// away (atom_is_positional can no longer find them); -1 means
	// build_program_desc derives highest_initial_pos from `atoms` as usual.
	int_t counter_highest_initial_pos = -1;
	// Props apply_step_counter_encoding resolved a hoisted conjunct's atom
	// to: table_step_provider::produce grounds these at the counter's own
	// absolute step (time_point) instead of the lookback-shifted
	// formula_time_point every other template atom uses.
	std::set<std::string> counter_relativized_props;

	// False when realizability was decided by a route whose strategy CANNOT
	// be re-expressed as a safety formula over the user's data atoms:
	//
	//   - Algorithm B: the strategy lives over the P_σ / R bits and the
	//     returned solution carries no `atoms` at all.
	//
	// The verdict is still sound — `is_ltl_aba_realizable` uses it as before
	// — but `ltl_to_safety_formula_full` must refuse to execute such a
	// solution instead of encoding it as `always T`, which silently drops
	// every obligation the strategy was carrying (LT-6).
	//
	// LA-10: the constant-output fast path used to be a second
	// non-executable route (`num_states == 0` recorded "some fixed output
	// combination works" without saying which).  It now materialises its
	// witness: `const_outputs` names each output stream with its constant
	// rational value (as the literal text "p/q"), and `const_formula` is
	// the executable `always(⋀_k o_k = c_k)` over the user's streams.
	// Convention: `num_states == 0` with `executable == true` and a
	// non-empty `const_outputs` means "constant strategy const_formula".
	bool executable = true;
	std::vector<std::pair<std::string, std::string>> const_outputs;
	tref const_formula = nullptr;
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

// Build   name[t+shift]:<carrier> = {value}   (shift ≤ 0: -1 → t-1, 0 → t).
template <NodeType node>
static tref build_carrier_eq_aux(const std::string& name, int shift, int value) {
	using tau = tree<node>;
	assert(shift <= 0 && "build_carrier_eq_aux: shift must be <= 0 (past or current)");
	std::string t_str = (shift == 0) ? "t" : ("t-" + std::to_string(-shift));
	// The Boolean carrier's type, spelled in full — a bare name without its
	// parameter is rejected by the grammar. IN-M4: ONE width source — this
	// is the same carrier type seed_aux_lookback_bits builds its seed
	// values in, so the two never desynchronise.
	std::string type_str = get_ba_type_name<node>(
		get_ba_type_id<node>(pack_bool_carrier_type<node>()));
	std::string expr = name + "[" + t_str + "]" + type_str + " = { "
	                 + std::to_string(value) + " }";
	typename tau::get_options opts;
	opts.parse.start = tau::wff;
	tref fm = tau::get(expr, std::move(opts));
	// A bare wff parse never sets
	// the io_var input/output bit (that happens during spec parsing), leaving
	// these aux variables classified as neither, which transform_io_var and
	// existentially_quantify_output_streams both reject. Resolve them the way
	// get_nso_rr resolves a bare formula -- the "o" prefix marks them outputs.
	return resolve_io_vars<node>(
		*definitions<node>::instance().get_io_context(), fm);
}

// Recursively rewrite all wff_S / wff_T nodes.
// Uses `counter` for fresh auxiliary names.
// `aux_pairs` collects (curr, prev) atom refs for each S operator.
// LT-14 STATUS: no caller consumes `aux_pairs` today -- the described
// G(X(p_prev) <-> p_curr) ltlsynt-skeleton integration does not exist
// (solve_ltl_aba never calls compile_since_trigger; the sole caller,
// ltl_to_safety_formula_full, binds it unused). The output is kept for
// that documented-but-unbuilt integration; treat it as inert until then.
// `safety_invs` collects G(curr && rhs) for the outermost S, and
// G(curr ↔ rhs) for inner (nested) S operators.  The biconditional form
// for inner S lets the auxiliary variable be false at t=0 without forcing
// the inner ψ to hold — only the outermost S's ψ is required at t=0.
// For an outermost T the S it rewrites to is always compiled as inner, and
// the outer obligation is added as G(¬curr) — the negated compiled formula.
//
// `spine_pol` records how the current node's truth is asserted by the
// top-level conjunct spine:
//
//    +1  asserted positively (an even number of negations on the spine)
//    -1  asserted negatively (an odd number of negations)
//     0  not on the spine at all (below a disjunction, an implication, a
//        temporal operator, or inside another S/T's operands)
//
// An S is "outer" — i.e. gets the always-true treatment G(curr && rhs) —
// only at `spine_pol == +1`.  This is the LT-2 fix: the old boolean
// `is_outer` was propagated unchanged through wff_neg and wff_or, so EVERY
// S not nested inside another S was forced to hold at every step.  Both
// `!(φ S ψ)` and `(φ S ψ) || χ` therefore compiled to a safety formula
// demanding `φ S ψ` always — the exact opposite of the first, and an
// over-constraint of the second.  Off the spine the S keeps only its
// biconditional tracking invariant plus a t=0 anchor, and its Boolean
// context is carried by the compiled formula that
// `ltl_to_safety_formula_full` now conjoins (it used to discard it).
template <NodeType node>
static tref compile_since_trigger_rec(
    tref fm,
    int& counter,
    std::vector<std::pair<tref,tref>>& aux_pairs,
    std::vector<tref>& safety_invs,
    std::vector<tref>& init_conds,
    std::vector<std::string>& unanchored_aux,
    int spine_pol = 1)
{
	using tau = tree<node>;
	const auto& t = tau::get(fm);
	if (!t.has_child()) return fm;

	const bool is_outer = (spine_pol > 0);
	auto nt = t[0].value.nt;

	// wff_T: φ T ψ = ¬(¬φ S ¬ψ)
	//
	// Trigger semantics (past dual of Release):
	//   π,i ⊨ φ T ψ  iff  ∀ j ≤ i : ψ@j  ∨  ∃ k ∈ (j,i] : φ@k
	// j = i is in range and its excuse window (i,i] is empty, so φ T ψ at i
	// requires ψ at i unconditionally.
	if (nt == tau::wff_T) {
		tref phi = t[0].first();
		tref psi = t[0].second();

		// Compile nested S/T inside the operands first (always inner), so
		// that the ψ used for the t=0 initial condition below is the
		// compiled one, matching what goes into the rewritten S.
		phi = compile_since_trigger_rec<node>(phi, counter, aux_pairs, safety_invs, init_conds, unanchored_aux, /*spine_pol=*/0);
		psi = compile_since_trigger_rec<node>(psi, counter, aux_pairs, safety_invs, init_conds, unanchored_aux, /*spine_pol=*/0);

		tref neg_phi = tau::build_wff_neg(phi);
		tref neg_psi = tau::build_wff_neg(psi);
		tref s_node  = tau::build_wff_S(neg_phi, neg_psi);

		// The rewritten S must NOT inherit is_outer: the outer obligation
		// belongs to ¬(the S), not to the S.  Funnelling is_outer through
		// here made the S branch assert G(since) and ¬ψ@0 — i.e. exactly
		// G(¬(φ T ψ)) plus the negation of the required t=0 condition.
		// With is_outer=false the S contributes only its tracking
		// invariant G(curr ↔ rhs), and the obligation is encoded below
		// from the negated compiled formula.
		tref s_rewr  = compile_since_trigger_rec<node>(s_node, counter, aux_pairs, safety_invs, init_conds, unanchored_aux, /*spine_pol=*/0);
		tref compiled = tau::build_wff_neg(s_rewr);

		if (is_outer) {
			// Outermost T safety invariant: G(¬curr).  Together with the
			// inner S's G(curr ↔ rhs) this pins the Since auxiliary to 0
			// and forces ψ at every step, which is what φ T ψ holding at
			// every step means.
			safety_invs.push_back(tau::build_wff_always(compiled));

			// t=0 condition: Trigger requires ψ (not ¬ψ) at position 0.
			// Encoded on the user-facing io_vars shifted to t=0, as the S
			// branch does, so the interpreter's fixpoint pipeline never
			// has to reason about aux[t-1] at time 0.
			auto psi_io_vars = tau::get(psi).select_top(is_child<node, tau::io_var>);
			tref psi_at_0 = fm_at_time_point<node>(psi, psi_io_vars, 0);
			init_conds.push_back(psi_at_0);
		}

		return compiled;
	}

	// wff_S: φ S ψ
	if (nt == tau::wff_S) {
		tref phi = t[0].first();
		tref psi = t[0].second();

		// Recursively compile nested S/T inside operands first.
		// Any S inside phi or psi is by definition inner (nested), so
		// pass is_outer=false to suppress the always-true requirement
		// and the psi-at-0 initial condition for those sub-operators.
		phi = compile_since_trigger_rec<node>(phi, counter, aux_pairs, safety_invs, init_conds, unanchored_aux, /*spine_pol=*/0);
		psi = compile_since_trigger_rec<node>(psi, counter, aux_pairs, safety_invs, init_conds, unanchored_aux, /*spine_pol=*/0);

		// Fresh auxiliary output variable name (o-prefix → controllable output).
		std::string aux_name = "o__ltl_s" + std::to_string(counter++) + "__";

		// Atoms: aux[t]=1  and  aux[t-1]=1
		tref curr = build_carrier_eq_aux<node>(aux_name, 0,  1);
		tref prev = build_carrier_eq_aux<node>(aux_name, -1, 1);

		// Tracking relation: G(curr ↔ (ψ ∨ (φ ∧ prev))).  It is pushed into
		// `safety_invs` below (in one of two forms, depending on the spine
		// polarity), which is what actually reaches the caller.
		tref rhs = tau::build_wff_or(psi, tau::build_wff_and(phi, prev));

		// LA-N3 — the t=0 anchor.  Strong-past semantics require
		// S(-1) = false: without it the system can set the auxiliary's
		// initial lookback to 1 and claim `φ S ψ` at the first enforced
		// step through the `φ ∧ prev` arm although ψ never held.  The
		// outermost S closes this with `ψ@0` plus `G(curr && rhs)`; an
		// off-spine (inner) S is recorded in `unanchored_aux` below and
		// the interpreter seeds its auxiliary to bv-0 at
		// t = formula_time_point - 1 (`seed_since_aux_bits`) — the same
		// memory pre-population the Mealy state bits use: single-BA-type,
		// no negative time index.
		//
		// Two shapes remain do-not-retry traps (both tried and reverted):
		//  - `curr@0 ↔ ψ@0` in `init_conds` is a CROSS-BA-TYPE
		//    biconditional outside any `always` at absolute time 0 (`curr`
		//    is bv, ψ is in the user's BA); it made the interpreter reject
		//    a satisfiable spec in Debug and crash in Release (the same
		//    mixed-type fragility make_interpreter documents at its
		//    G(phi_A) && G(phi_B) special case).
		//  - `¬prev` as a non-G initial condition references the negative
		//    time index `aux[-1]`, which the interpreter's fixpoint
		//    pipeline mis-treats as a G-unrolled seed (the reason the
		//    outermost branch shifts ψ to time 0 instead).

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

			// LA-N3 anchor: `rhs` mentions aux[t-1], which nothing in the
			// formula constrains at the first enforced step.  Record the
			// auxiliary so the interpreter seeds it to 0 there (strong
			// past: S(-1) = false; a T rewrites to this branch too, and
			// S(-1) = false is exactly T(-1) = true).  The outermost
			// branch above is NOT recorded: its auxiliary is anchored by
			// `ψ@0` + G(curr && rhs), and seeding it to 0 would force ψ at
			// every step, outlawing the φ-chain.
			unanchored_aux.push_back(aux_name);
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
	//
	// The spine polarity is propagated, NOT the old boolean is_outer:
	//   wff_and  keeps a positive spine (asserting A ∧ B asserts both);
	//            under a negative spine it drops off — ¬(A ∧ B) asserts
	//            neither operand.
	//   wff_or   is the dual: it keeps a NEGATIVE spine (¬(A ∨ B) asserts
	//            ¬A and ¬B) and drops off a positive one.
	//   wff_neg  flips the sign.
	//   everything else (implications, temporal operators, conditionals)
	//            drops off the spine entirely.
	const auto& op = t[0];
	size_t nc = op.children_size();
	if (nc == 0) return fm;

	int child_pol = 0;
	if      (nt == tau::wff_neg) child_pol = -spine_pol;
	else if (nt == tau::wff_and) child_pol = (spine_pol > 0) ?  1 : 0;
	else if (nt == tau::wff_or)  child_pol = (spine_pol < 0) ? -1 : 0;

	if (nc == 1) {
		tref new_c = compile_since_trigger_rec<node>(op.first(), counter, aux_pairs, safety_invs, init_conds, unanchored_aux, child_pol);
		if (new_c == op.first()) return fm;
		return tau::get(tau::wff, tau::get(nt, new_c));
	}
	if (nc == 2) {
		tref new_l = compile_since_trigger_rec<node>(op.first(),  counter, aux_pairs, safety_invs, init_conds, unanchored_aux, child_pol);
		tref new_r = compile_since_trigger_rec<node>(op.second(), counter, aux_pairs, safety_invs, init_conds, unanchored_aux, child_pol);
		if (new_l == op.first() && new_r == op.second()) return fm;
		return tau::get(tau::wff, tau::get(nt, new_l, new_r));
	}
	if (nc == 3) {
		tref new_a = compile_since_trigger_rec<node>(op.first(),  counter, aux_pairs, safety_invs, init_conds, unanchored_aux, child_pol);
		tref new_b = compile_since_trigger_rec<node>(op.second(), counter, aux_pairs, safety_invs, init_conds, unanchored_aux, child_pol);
		tref new_c = compile_since_trigger_rec<node>(op.third(),  counter, aux_pairs, safety_invs, init_conds, unanchored_aux, child_pol);
		if (new_a == op.first() && new_b == op.second() && new_c == op.third())
			return fm;
		// 3-child case: use build_wff_conditional for wff_conditional,
		// otherwise fall back to initializer_list get.
		if (nt == tau::wff_conditional)
			return tau::build_wff_conditional(new_a, new_b, new_c);
		tref ch3[3] = { new_a, new_b, new_c };
		return tau::get(tau::wff, tau::get(nt, ch3, 3));
	}
	// Arity > 3.  This used to `return fm` unchanged, silently leaving any
	// S/T below a wider node uncompiled — the pure-past fast path then just
	// declines itself (the S survives, so has_ltl_operators stays true and
	// solve_ltl_aba's ppLTLTT encoding takes over), but a silent arity limit
	// in a rewriting pass is a trap.  Handle it generically.
	trefs kids;
	kids.reserve(nc);
	for (size_t i = 0; i < nc; ++i) kids.push_back(op.child(i));
	trefs new_kids;
	new_kids.reserve(nc);
	bool changed = false;
	for (tref c : kids) {
		tref nc_ = compile_since_trigger_rec<node>(c, counter, aux_pairs, safety_invs, init_conds, unanchored_aux, child_pol);
		if (nc_ != c) changed = true;
		new_kids.push_back(nc_);
	}
	if (!changed) return fm;
	return tau::get(tau::wff, tau::get(nt, new_kids.data(), new_kids.size()));
}

// Top-level S/T compilation pass.
// Returns {compiled_formula, safety_formula, init_formula, aux_pairs,
// unanchored_aux}.
//
// compiled_formula is fm with every S/T node replaced by its auxiliary atom,
// and fm unchanged when there are no S/T nodes.  It carries the spec's Boolean
// structure and nothing else; the per-operator relations travel in
// safety_formula.  Callers wrap it themselves — `ltl_to_safety_formula_full`
// conjoins `G(compiled_formula)`.
//
// safety_formula: G(outermost_s && rhs) for a positively-asserted top-level S
//   (always-true requirement), G(¬outermost_s) for a top-level T (which is
//   ¬S), and G(inner_s ↔ rhs) for every other S (biconditional tracks the
//   auxiliary without forcing it to 1 at t=0).
// init_formula: psi_at_0 for an OUTERMOST S/T only (t=0 condition; for T it is
//   the un-negated psi, since φ T ψ requires ψ at position 0).  An off-spine S
//   contributes nothing here — its t=0 anchor is the interpreter-side seeding
//   of `unanchored_aux` (LA-N3; see the anchor note in the S branch).
// aux_pairs: one entry per S operator: (curr_atom, prev_atom).
// Callers use aux_pairs to add G(X(p_prev) <-> p_curr) to the ltlsynt skeleton.
// unanchored_aux: the auxiliary names of every INNER (off-spine) S — the ones
//   whose tracking invariant leaves aux[t-1] free at the first enforced step.
//   The interpreter seeds each to bv-0 at t = formula_time_point - 1
//   (seed_since_aux_bits), encoding S(-1) = false / T(-1) = true.
template <NodeType node>
static std::tuple<tref, tref, tref, std::vector<std::pair<tref,tref>>,
                  std::vector<std::string>>
compile_since_trigger(tref fm) {
	using tau = tree<node>;
	// LT-16(b): has_since_trigger was a verbatim duplicate of
	// has_past_operators (ltl_aba_helpers.tmpl.h); one predicate now.
	if (!has_past_operators<node>(fm))
		return {fm, tau::_T(), tau::_T(), {}, {}};

	std::vector<std::pair<tref,tref>> aux_pairs;
	std::vector<tref> safety_invs;
	std::vector<tref> init_conds;
	std::vector<std::string> unanchored_aux;
	int counter = 0;
	tref compiled = compile_since_trigger_rec<node>(fm, counter, aux_pairs, safety_invs, init_conds, unanchored_aux);

	tref safety_fm = tau::_T();
	for (tref si : safety_invs)
		safety_fm = tau::build_wff_and(safety_fm, si);

	tref init_fm = tau::_T();
	for (tref ic : init_conds)
		init_fm = tau::build_wff_and(init_fm, ic);

	LOG_DEBUG << "[ltl_aba] S/T compile-away: "
	          << counter << " auxiliary variable(s) introduced";
	return {compiled, safety_fm, init_fm, std::move(aux_pairs),
	        std::move(unanchored_aux)};
}

} // namespace idni::tau_lang
