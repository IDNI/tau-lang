// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// ltl_aba_builders.tmpl.h - Algorithms A/B/C/D, solve, realize, safety, explain, CTL*
// Split from ltl_aba.tmpl.h for readability.

namespace idni::tau_lang {


// try_propositional_synthesis dispatcher. The capability takes only the formula,
// so a BA must recognise its own and decline everything else; the assert catches
// two BAs claiming one formula, which is a self-gating bug rather than a pack
// configuration error.
template <typename Node, typename BA>
static propositional_synthesis<Node> ba_try_propositional_synthesis(tref fm,
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	if constexpr (ba_has_descriptor_v<Node, BA>
		&& requires { ba_descriptor<BA, Node>
			::try_propositional_synthesis(fm, atoms); })
	{
		return ba_descriptor<BA, Node>
			::try_propositional_synthesis(fm, atoms);
	}
	return synthesis_declined<Node>();
}

template <typename Node>
static propositional_synthesis<Node> pack_try_propositional_synthesis(tref fm,
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	propositional_synthesis<Node> out;
	[[maybe_unused]] int claimants = 0;
	[&]<std::size_t... Is>(std::index_sequence<Is...>) {
		([&] {
			using BA = std::tuple_element_t<Is,
				typename Node::bas_tuple>;
			if (auto r = ba_try_propositional_synthesis<Node, BA>(
				fm, atoms))
			{
				++claimants;
				if (!out) out = std::move(r);
			}
		}(), ...);
	}(std::make_index_sequence<
		std::tuple_size_v<typename Node::bas_tuple>>{});
	assert(claimants <= 1 && "pack_try_propositional_synthesis: two BAs claim "
		"the same formula");
	return out;
}


// partial_out, when non-null, stays populated even when the return value ends up std::nullopt.
template <NodeType node>
static std::optional<ltl_aba_solution<node>>
solve_ltl_aba(tref fm, ltl_aba_solution<node>* partial_out)
{
	using tau = tree<node>;

	// Past operators (S, T) are handled at skeleton level via DFA temporal
	// testers (ppLTLTT approach), not by AST-level compile-away.  The
	// formula is passed to extract_data_atoms unchanged — S/T nodes are
	// transparent to atom extraction (they are temporal operators, not
	// data atoms).  The skeleton generation intercepts S/T and emits
	// DFA state variables + constraints instead.
	ltl_aba_solution<node> sol;
	sol.atoms = extract_data_atoms<node>(fm);
	if (partial_out) partial_out->atoms = sol.atoms;

	// Split into maximal top-level conjuncts and pick out the ones whose
	// atoms are all positional (max-position hoisting); throws for the two
	// temporary refusals -- a conjunct mixing positional and relative-time
	// atoms, or a positional atom under F/U/R/W/S/T (see the function doc).
	std::vector<tref> hoist_conjuncts =
		collect_hoist_conjuncts<node>(fm, sol.atoms);

	// Past operators (S, T) require the ppLTLTT temporal tester encoding
	// in the default path.  Algorithm A/B/D use ltl_skeleton() which
	// passes S/T through as literal operators — those paths don't have
	// the DFA state-variable machinery.  Skip them when S/T are present.
	const bool has_past = has_past_operators<node>(fm);

	// Ask whichever BA owns these atoms to synthesise propositionally. Past
	// operators need the ppLTLTT temporal-tester encoding of the default path,
	// which those fast paths do not have, so they are not offered the formula.
	if (!has_past)
		if (auto claim = pack_try_propositional_synthesis<node>(
			fm, sol.atoms); claim) {
				if (!*claim && partial_out) *partial_out = sol;
				return *claim;
		}


	if (sol.atoms.empty()) {
		// Purely propositional: no io_var atoms.
		if (has_past) {
			std::vector<past_temporal_tester> testers;
			sol.skeleton = skeleton_str_with_testers<node>(fm, sol.atoms, testers);
			append_tester_constraints(sol.skeleton, testers);
			for (const auto& t : testers)
				sol.output_props.push_back(t.state_var);
			auto [real, hoa] = call_ltlsynt(sol.skeleton, {}, sol.output_props);
			if (!real) { if (partial_out) *partial_out = sol; return std::nullopt; }
			sol.aut = parse_hoa(hoa);
			return sol;
		}
		sol.skeleton = ltl_skeleton<node>(fm, sol.atoms);
		auto [real, hoa] = call_ltlsynt(sol.skeleton, {}, {});
		if (!real) { if (partial_out) *partial_out = sol; return std::nullopt; }
		sol.aut = parse_hoa(hoa);
		return sol;
	}

	for (auto& [f, name] : sol.atoms) {
		LOG_DEBUG << "[ltl_aba] atom " << name << " = "
		         << tree<node>::get(f).to_str()
		         << " lookback=" << (atom_has_lookback<node>(f) ? "yes" : "no");
	}

	// Classify atoms as input (only i* vars) or output (has o* var).
	for (auto& [f, name] : sol.atoms) {
		if (is_pure_input_atom<node>(f))
			sol.input_props.push_back(name);
		else
			sol.output_props.push_back(name);
	}

	// Hoist each all-positional conjunct: rewrite it to a lookback formula
	// guarded by the step counter reaching its own k_max, before the
	// skeleton walk -- see apply_step_counter_encoding's doc comment. A
	// no-op (returns "") when the formula has no positional atoms.
	std::string step_counter_extra = apply_step_counter_encoding<node>(
		hoist_conjuncts, sol.atoms, sol.input_props, sol.output_props,
		sol.counter_highest_initial_pos, sol.counter_relativized_props);

	// Erase each hoisted conjunct's own occurrence site to a literal T.
	// before the main skeleton walk, rather than relying on an individual
	// atom missing from `sol.atoms` (apply_step_counter_encoding already
	// dropped them): erasing only the leaf atoms is correct under a
	// top-level conjunction (1 && x == x) but wrong under any negative-
	// polarity shape reaching the conjunct -- e.g. a bare negation
	// `!(o[0]=1)` would erase its atom to "1" and leave "!1" ("0") behind,
	// turning a vacuous conjunct into a hard-coded falsehood. Replacing the
	// WHOLE conjunct (whatever its own Boolean shape) is correct regardless
	// of polarity.
	tref fm_for_skeleton = fm;
	if (!hoist_conjuncts.empty()) {
		subtree_map<node, tref> erase_map;
		for (tref c : hoist_conjuncts) erase_map[c] = tau::_T();
		fm_for_skeleton = rewriter::replace<node>(fm, erase_map);
	}

	// Build skeleton with DFA temporal testers for past operators (S, T).
	// The ppLTLTT approach (Azzopardi et al., ATVA'23) replaces each S/T
	// subformula with a fresh propositional state variable and encodes
	// the DFA transition + initial condition as pure future-LTL constraints
	// (G, X, propositional) that ltlsynt handles natively.
	auto [skel, testers] = ltl_skeleton_with_testers<node>(fm_for_skeleton, sol.atoms);
	sol.skeleton = std::move(skel) + step_counter_extra;

	// Cross-step shift-chain constraints: tie shifted instances of the same
	// signal together (e.g. o1[t]=1 and o1[t-1]!=1) before the consistency
	// pass, which folds the resulting input-only assumptions into its own
	// wrap so both sets combine behind a single implication.
	std::string shift_chain_input_assumptions;
	add_shift_chain_constraints<node>(sol.atoms, sol.skeleton,
		shift_chain_input_assumptions, &sol.shift_chain_constraints);

	add_consistency_constraints<node>(sol.atoms, sol.skeleton,
		&sol.consistency_constraints, has_past /*polarity_complete*/,
		std::move(shift_chain_input_assumptions));

	// Append DFA tester constraints and register state variables as outputs.
	append_tester_constraints(sol.skeleton, testers);
	for (const auto& t : testers) {
		sol.output_props.push_back(t.state_var);
		LOG_DEBUG << "[ltl_aba] ppLTLTT tester: " << t.state_var
		          << " init=" << t.initial_value
		          << " trans=" << t.transition
		          << " negate=" << t.negate_output;
	}

	LOG_DEBUG << "[ltl_aba] LTL skeleton: " << sol.skeleton;
	LOG_DEBUG << "[ltl_aba] inputs:  " << [&]{
		std::string s; for (auto& p : sol.input_props)  s += p + " "; return s; }();
	LOG_DEBUG << "[ltl_aba] outputs: " << [&]{
		std::string s; for (auto& p : sol.output_props) s += p + " "; return s; }();

	auto [realizable, hoa_text] =
	    call_ltlsynt(sol.skeleton, sol.input_props, sol.output_props);
	if (!realizable) { if (partial_out) *partial_out = sol; return std::nullopt; }

	sol.aut = parse_hoa(hoa_text);
	return sol;
}

// ── is_ltl_aba_realizable ─────────────────────────────────────────────────────

template <NodeType node>
bool is_ltl_aba_realizable(tref fm, int_t start_time, bool output) {
	LOG_DEBUG << "[ltl_aba] is_ltl_aba_realizable: " << LOG_FM(fm);

	// LT-5 / IN-1 backstop: a `wff_semantic_neg`, `A` or `E` that reaches
	// here was not handled by reduce_ctl_star_to_ltl (direct callers such
	// as preferences.h skip the reduction entirely). None has a
	// propositional encoding -- the skeleton would flatten it to "1" --
	// and A/E left in place could bounce back into is_tau_formula_sat's
	// CTL* branch forever. Refuse rather than answer wrongly.
	if (has_ctl_star_operators<node>(fm))
		throw ltl_synthesis_error(
		    "CTL* operators (A / E / semantic negation) reached the LTL "
		    "realizability check without a CTL* reduction; route the "
		    "formula through is_tau_formula_sat");

	// Safety fast-path: if the formula has no full-LTL operators AND
	// no Boolean combinations of models, it is a pure G/safety formula
	// the safety pipeline can decide on its own.  In that case the
	// entire six-phase LTL pipeline (data-atom extraction →
	// propositional skeleton → ltlsynt subprocess → HOA parse → ABA
	// oracle on every transition) reduces to a single
	// is_tau_formula_sat call.  Skipping ltlsynt (5–50 ms per call) on
	// pure safety specs is the largest single per-call win on the
	// synthesis hot path.
	//
	// The Boolean-combs-of-models guard is what prevents an infinite
	// recursion with is_tau_formula_sat: that function routes formulas
	// like `(G A) || (G B)` here precisely because it can't handle
	// them itself.  Without the guard, this fast-path would route back.
	if (!has_ltl_operators<node>(fm)
	    && has_no_boolean_combs_of_models<node>(fm)) {
		LOG_DEBUG << "[ltl_aba] safety fast-path "
		             "(no full-LTL operators, single G)";
		return is_tau_formula_sat<node>(fm, start_time, output);
	}

	auto maybe = solve_ltl_aba<node>(fm);
	LOG_DEBUG << "[ltl_aba] solve_ltl_aba returned: " << maybe.has_value();

	if (!maybe) {
		if (output) LOG_DEBUG << "[ltl_aba] UNREALIZABLE (propositional)";
		return false;
	}

	auto& sol = *maybe;
	LOG_DEBUG << "[ltl_aba] atoms=" << sol.atoms.size()
	          << " states=" << sol.aut.num_states;

	// Trivially realizable: no states produced. parse_hoa refuses a
	// strategy with fewer than one state (LA-8), so only solutions an
	// algorithm constructed deliberately without an automaton (the
	// constant-output fast path) reach this branch.
	if (sol.aut.num_states == 0) {
		if (output) LOG_INFO << "[ltl_aba] REALIZABLE";
		return true;
	}

	// Purely propositional (no data atoms) — ltlsynt verdict is final.
	if (sol.atoms.empty()) {
		if (output) LOG_INFO << "[ltl_aba] REALIZABLE (propositional)";
		return true;
	}

	LOG_DEBUG << "[ltl_aba] strategy has " << sol.aut.num_states << " state(s)";

	// ABA oracle: verify each strategy transition guard is ABA-feasible.
	// ABA inconsistency between proposition pairs has already been handled
	// by the consistency constraints added to the skeleton (see solve_ltl_aba).
	// Here we only check individual guards via existential satisfiability.
	for (int s = 0; s < sol.aut.num_states; ++s) {
		for (auto& e : sol.aut.edges[s]) {
			LOG_DEBUG << "[ltl_aba] checking edge " << s << "->[" << e.guard_label << "]->" << e.dst;
			if (!guard_is_aba_feasible<node>(
			        e.guard_label, sol.aut.aps, sol.atoms)) {
				LOG_DEBUG << "[ltl_aba] ABA infeasible edge "
				          << s << "→" << e.dst
				          << " guard=[" << e.guard_label << "]";
				if (output)
					LOG_INFO << "[ltl_aba] UNREALIZABLE "
					            "(propositionally realizable but ABA-infeasible)";
				return false;
			}
		}
	}

	if (output) LOG_INFO << "[ltl_aba] REALIZABLE";
	return true;
}

// ── Multi-state Mealy → safety formula ───────────────────────────────────────
//
// For a k-state Mealy machine, encodes the strategy as
// an always(phi) formula with lookback 1.
//
// Approach: introduce k auxiliary output bitvector variables o__ltl_s0__,
// o__ltl_s1__, ..., o__ltl_s{k-1}__ representing the automaton state in a
// one-hot encoding.  The always-formula encodes:
//   (a) one-hot constraint: exactly one state bit is true at every step,
//   (b) transition relation: if si[t-1]=1 and the edge guard holds, then
//       s_{dst}[t]=1 and the data atoms satisfy the guard's output conditions.
//
// The synthesis chooses the initial state bits si[-1] freely; any valid
// initialization satisfies the formula (since the strategy is realizable).

// A state bit is set to the carrier type's one, not to the numeric constant 1:
// at a one-bit carrier those coincide and `= { 1 }` normalizes to `x' = 0`,
// which the atom's consumers no longer recognize.
template <NodeType node>
static tref build_state_bit_eq(const std::string& name, int shift, bool set)
{
	using tau = tree<node>;
	DBG(assert(shift <= 0 && "build_state_bit_eq: shift must be <= 0");)
	const size_t tid = get_ba_type_id<node>(pack_bool_carrier_type<node>());
	tref var = shift == 0
		? build_out_var_at_t<node>(build_var_name<node>(name), tid)
		: build_out_var_at_t_minus<node>(name,
			static_cast<size_t>(-shift), tid);
	return tau::build_bf_eq(var, set ? build_bf_t_type<node>(tid)
					 : build_bf_f_type<node>(tid));
}

template <NodeType node>
static tref encode_mealy_as_safety(const ltl_aba_solution<node>& sol)
{
	using tau = tree<node>;
	const auto& aut = sol.aut;
	int k = aut.num_states;

	// Auxiliary output state variable names. Use "ms" (Mealy-state) prefix to
	// avoid collision with compile-away S-operator variables (o__ltl_sN__).
	std::vector<std::string> sv;
	for (int i = 0; i < k; ++i)
		sv.push_back("o__ltl_ms" + std::to_string(i) + "__");

	// ── (a) One-hot constraint at the current step ────────────────────────
	tref at_least = tau::_F();
	for (int i = 0; i < k; ++i)
		at_least = tau::build_wff_or(at_least, build_state_bit_eq<node>(sv[i], 0, true));

	tref at_most = tau::_T();
	for (int i = 0; i < k; ++i)
		for (int j = i + 1; j < k; ++j)
			at_most = tau::build_wff_and(at_most,
			    tau::build_wff_neg(
			        tau::build_wff_and(build_state_bit_eq<node>(sv[i], 0, true),
			                          build_state_bit_eq<node>(sv[j], 0, true))));

	tref one_hot = tau::build_wff_and(at_least, at_most);

	// ── (b) Transition rules with lookback-1 ──────────────────────────────
	// For each source state s:
	//   si[t-1]=1  →  ∨_edges_from_s (guard_formula ∧ s_{dst}[t]=1)
	tref trans = tau::_T();
	for (int s = 0; s < k; ++s) {
		tref prev_s = build_state_bit_eq<node>(sv[s], -1, true);
		tref edges_disj = tau::_F();
		for (const auto& e : aut.edges[s]) {
			if (e.dst < 0 || e.dst >= k) {
				LOG_ERROR << "[ltl_aba] HOA edge dst " << e.dst
				          << " out of range [0," << k << "), skipping";
				continue;
			}
			tref guard_fm = guard_to_aba<node>(
			    e.guard_label, aut.aps, sol.atoms);
			tref next_d = build_state_bit_eq<node>(sv[e.dst], 0, true);
			edges_disj = tau::build_wff_or(edges_disj,
			    tau::build_wff_and(guard_fm, next_d));
		}
		if (!aut.edges[s].empty()) {
			tref rule = tau::build_wff_or(
			    tau::build_wff_neg(prev_s), edges_disj);
			trans = tau::build_wff_and(trans, rule);
		} else {
			// LT-28: a state without outgoing edges (possible only
			// from a truncated HOA -- ltlsynt Mealy machines are
			// input-complete) must be forbidden as a predecessor,
			// not left unconstrained.
			trans = tau::build_wff_and(trans,
			    tau::build_wff_neg(prev_s));
		}
	}

	tref body = normalize_non_temp<node>(tau::build_wff_and(one_hot, trans));
	LOG_DEBUG << "[ltl_aba] multi-state safety body: " << LOG_FM(body);
	return tau::build_wff_always(body);
}

// Build the fixed-time initial-state and initial-output conditions for a
// multi-state Mealy encoding.  These are returned separately so the caller
// can combine them with the safety formula AFTER normalisation (normalising
// them together can cause the solver to report unsatisfiability at step 0).
//
// Returned pair: { sv[initial_state][0]={1},  init_out_disjunction }
// init_out_disjunction = ∨_e ( guard_e(t=0) ∧ sv[dst_e][1]={1} )
// If the initial state has no outgoing edges (shouldn't happen for a
// realizable formula), the second element is nullptr.
template <NodeType node>
static std::pair<tref,tref>
encode_mealy_initial_conditions(const ltl_aba_solution<node>& sol,
                                const std::vector<std::string>& sv)
{
	using tau = tree<node>;
	const auto& aut = sol.aut;
	const int k     = (int)sv.size();
	const int init_s = aut.initial_state;
	if (init_s < 0 || init_s >= k) return {nullptr, nullptr};

	// (1) sv[initial_state][0] = {1}
	tref sv_tmpl = build_state_bit_eq<node>(sv[init_s], 0, true);
	auto sv_io   = tau::get(sv_tmpl).select_top(is_child<node, tau::io_var>);
	tref init_sv = fm_at_time_point<node>(sv_tmpl, sv_io, 0);

	// (2) ∨_e ( guard_e(t=0) ∧ sv[dst_e][1]={1} )
	tref init_out = nullptr;
	for (const auto& e : aut.edges[init_s]) {
		if (e.dst < 0 || e.dst >= k) continue;
		tref gfm   = guard_to_aba<node>(e.guard_label, aut.aps, sol.atoms);
		auto gvars = tau::get(gfm).select_top(is_child<node, tau::io_var>);
		tref g0    = fm_at_time_point<node>(gfm, gvars, 0);
		tref sv_t  = build_state_bit_eq<node>(sv[e.dst], 0, true);
		auto sv_t_io = tau::get(sv_t).select_top(is_child<node, tau::io_var>);
		tref sv1   = fm_at_time_point<node>(sv_t, sv_t_io, 1);
		tref edge  = tau::build_wff_and(g0, sv1);
		init_out   = init_out ? tau::build_wff_or(init_out, edge) : edge;
	}

	return {init_sv, init_out};
}

// ── encode_solution_as_safety ────────────────────────────────────────────────
// Encodes an already-synthesised ltl_aba_solution as an always(phi) safety formula.

template <NodeType node>
static tref encode_solution_as_safety(const ltl_aba_solution<node>& sol) {
	using tau = tree<node>;

	// Purely propositional: realizable but no data constraints to encode.
	if (sol.atoms.empty()) return tau::_T();

	const auto& aut = sol.aut;

	// Trivially realizable: empty automaton.
	if (aut.num_states == 0) return tau::_T();

	if (aut.num_states > 1) {
		LOG_INFO << "[ltl_aba] Multi-state strategy ("
		         << aut.num_states
		         << " states) — encoding with auxiliary one-hot state bits";
		return encode_mealy_as_safety<node>(sol);
	}

	// Single-state strategy: the self-loop guard is the perpetual output constraint.
	if (aut.edges.empty() || aut.edges[0].empty())
		return tau::build_wff_always(tau::_T());

	// Build the disjunction of ABA guard formulas over all edges from state 0.
	tref combined = tau::_F();
	for (const auto& e : aut.edges[0]) {
		tref guard_fm = guard_to_aba<node>(e.guard_label, aut.aps, sol.atoms);
		tref norm_guard = normalize_non_temp<node>(guard_fm);
		combined = tau::build_wff_or(combined, norm_guard);
	}
	tref simplified = normalize_non_temp<node>(combined);
	LOG_DEBUG << "[ltl_aba] ltl_to_safety_formula result: always("
	          << LOG_FM(simplified) << ")";
	return tau::build_wff_always(simplified);
}

// ── ltl_to_safety_formula ─────────────────────────────────────────────────────
//
// `_full` does the work and returns BOTH the safety formula AND the
// ltl_aba_solution (when one was synthesised). The interpreter caches the
// solution so it can introspect the Mealy state at runtime, visualise the
// strategy, etc. — info that would otherwise be discarded after encoding.
//
// The thin wrapper `ltl_to_safety_formula(fm)` discards the solution to
// preserve the existing single-return API for callers that don't need it.

template <NodeType node>
std::tuple<tref, std::optional<ltl_aba_solution<node>>, std::vector<std::string>>
ltl_to_safety_formula_full(tref fm) {
	using tau = tree<node>;
	LOG_DEBUG << "[ltl_aba] ltl_to_safety_formula: " << LOG_FM(fm);

	// Fast path: if all LTL operators are past (S/T), compile them away and
	// return G(curr && rhs) safety invariants for each S operator.
	// No Mealy synthesis is needed on this path; the returned solution is empty.
	{
		auto [compiled_fast, safety_fm, init_fm, _aux, unanchored_aux] =
			compile_since_trigger<node>(fm);
		if (!has_ltl_operators<node>(compiled_fast)) {
			LOG_DEBUG << "[ltl_aba] ltl_to_safety_formula: "
			          << "pure past-LTL, returning safety formula";
			// LT-2: the compiled formula used to be DISCARDED here, so the
			// Boolean structure around each S never reached the interpreter
			// and only the per-operator invariants survived.  A tau spec must
			// hold at every step, so the obligation is G(compiled).
			//
			// The wrap is distributed over top-level conjuncts and skips a
			// conjunct that is already an `always`, so `(φ S ψ) && G(χ)` gives
			// `G(curr) && G(χ)` rather than the nested `G(curr && G(χ))` that
			// the normalizer would then have to unpick.
			//
			// `wff_and` is N-ARY: `A && B && C` is ONE node with three
			// children.  Reading only first()/second() dropped every conjunct
			// past the second — silently, straight out of the executed safety
			// formula.
			std::function<tref(tref)> wrap_always = [&](tref n) -> tref {
				trefs kids;
				{
					// Read the children out BEFORE recursing: the
					// recursive calls build nodes, and nothing here
					// should depend on a reference into the tree
					// store surviving that.
					const auto& nt_ = tau::get(n);
					if (nt_.equals_T()) return n;
					if (!nt_.has_child())
						return tau::build_wff_always(n);
					auto k = nt_[0].value.nt;
					if (k == tau::wff_always) return n;
					if (k != tau::wff_and)
						return tau::build_wff_always(n);
					const auto& op = nt_[0];
					for (size_t i = 0; i < op.children_size(); ++i)
						kids.push_back(op.child(i));
				}
				if (kids.empty()) return tau::build_wff_always(n);
				tref acc = nullptr;
				for (tref k : kids) {
					tref w = wrap_always(k);
					acc = acc ? tau::build_wff_and(acc, w) : w;
				}
				return acc;
			};
			tref obligation = wrap_always(compiled_fast);
			tref out = tau::build_wff_and(obligation,
			           tau::build_wff_and(safety_fm, init_fm));
			// LA-N3: hand the inner-S auxiliaries to the caller so the
			// interpreter can seed their t=0 anchor (S(-1) = false).
			return {out, std::nullopt, std::move(unanchored_aux)};
		}
	}

	auto maybe = solve_ltl_aba<node>(fm);
	if (!maybe) {
		LOG_DEBUG << "[ltl_aba] ltl_to_safety_formula: not realizable";
		return {nullptr, std::nullopt, {}};
	}

	auto& sol = *maybe;

	// LT-6: Algorithm B decides realizability by a route whose strategy is
	// not expressible over the user's data atoms (the P_σ / D-bit
	// machinery).  It used to be mapped to `{tau::_T(), sol}` under the
	// comment "purely propositional: realizable but no data constraints to
	// encode", which is wrong — realizability depended on a concrete output
	// strategy that `always T` does not encode.  The interpreter then ran
	// `always T` and emitted default outputs that can violate the very spec
	// that was reported REALIZABLE.
	//
	// Refusing to execute is the honest answer; the realizability verdict
	// from `is_ltl_aba_realizable` is unaffected.  (LA-10: the
	// constant-output fast path used to be refused here too; it now
	// materialises its witness — see `const_formula` below.)
	if (!sol.executable) {
		LOG_ERROR << "[ltl_aba] specification is REALIZABLE but the "
		             "synthesised strategy cannot be encoded as a safety "
		             "formula (Algorithm B strategy over bookkeeping "
		             "bits) — it is not executable\n";
		return {nullptr, std::nullopt, {}};
	}

	// LA-10: constant-output strategy — the executable form is the
	// materialised `always(⋀ o_k = c_k)` witness, not `always T`.
	if (sol.const_formula)
		return {sol.const_formula, std::move(sol), {}};

	// Purely propositional: realizable but no data constraints to encode.
	if (sol.atoms.empty()) return {tau::_T(), std::move(sol), {}};

	const auto& aut = sol.aut;

	// Trivially realizable: empty automaton.
	if (aut.num_states == 0) return {tau::_T(), std::move(sol), {}};

	if (aut.num_states > 1) {
		LOG_INFO << "[ltl_aba] Multi-state strategy ("
		         << aut.num_states
		         << " states) — encoding with auxiliary one-hot state bits";
		tref encoded = encode_mealy_as_safety<node>(sol);
		return {encoded, std::move(sol), {}};
	}

	// Single-state strategy: the self-loop guard is the perpetual output
	// constraint. LA-R6: ltlsynt Mealy machines are input-complete, so a
	// state with no outgoing edge only arises from a degraded automaton;
	// executing it as `always T` would drop every obligation (the 1-state
	// analogue of LT-28). Not executable.
	if (aut.edges.empty() || aut.edges[0].empty()) {
		LOG_ERROR << "[ltl_aba] single-state strategy has no outgoing "
		             "edge; the automaton is degraded and cannot be "
		             "executed\n";
		return {nullptr, std::nullopt, {}};
	}

	// Build the disjunction of ABA guard formulas over all edges from state 0.
	tref combined = tau::_F();
	for (const auto& e : aut.edges[0]) {
		tref guard_fm = guard_to_aba<node>(e.guard_label, aut.aps, sol.atoms);
		tref norm_guard = normalize_non_temp<node>(guard_fm);
		combined = tau::build_wff_or(combined, norm_guard);
	}
	tref simplified = normalize_non_temp<node>(combined);
	LOG_DEBUG << "[ltl_aba] ltl_to_safety_formula result: always("
	          << LOG_FM(simplified) << ")";
	return {tau::build_wff_always(simplified), std::move(sol), {}};
}

template <NodeType node>
tref ltl_to_safety_formula(tref fm) {
	auto [safety, _sol, _aux] = ltl_to_safety_formula_full<node>(fm);
	return safety;
}

// ── ltl_explain ───────────────────────────────────────────────────────────────

template <NodeType node>
bool ltl_explain(tref fm, std::ostream& out) {
	using tau = tree<node>;

	// IN-R3: `ltl` used to hand A/E/- straight to the skeleton, where the
	// tester variant flattened them to "1". Reduce like is_tau_formula_sat
	// does (or refuse, via ltl_synthesis_error, where no sound encoding
	// exists) before explaining anything.
	// TODO: unlike is_ltl_aba_realizable's fast path, this does not also require has_no_boolean_combs_of_models
	if (has_ctl_star_operators<node>(fm)) {
		auto reduction = reduce_ctl_star_to_ltl<node>(fm);
		out << "CTL* reduced to LTL: "
			<< tau::get(reduction.ltl_formula).to_str() << "\n";
		fm = reduction.ltl_formula;
	}

	if (!has_ltl_operators<node>(fm)) {
		out << "Formula has no LTL operators (treated as G(phi))\n";
		// Fall through to the existing safety pipeline.
		bool sat = is_tau_formula_sat<node>(fm, 0, false);
		out << (sat ? "REALIZABLE" : "UNREALIZABLE") << "\n";
		return sat;
	}

	// sol stays populated even when solve_ltl_aba returns std::nullopt.
	ltl_aba_solution<node> sol;
	std::optional<ltl_aba_solution<node>> maybe;
	try {
		maybe = solve_ltl_aba<node>(fm, &sol);
	} catch (const std::runtime_error& e) {
		out << "REFUSED: " << e.what() << "\n";
		return false;
	}
	if (maybe) sol = std::move(*maybe);

	out << "\nData atoms (" << sol.atoms.size() << "):\n";
	for (auto& [f, name] : sol.atoms)
		out << "  " << name << "  :=  " << tau::get(f).to_str() << "\n";

	out << "\nInput propositions:  ";
	for (size_t i = 0; i < sol.input_props.size(); ++i) {
		if (i) out << ", ";
		out << sol.input_props[i];
	}
	if (sol.input_props.empty()) out << "(none)";
	out << "\n";

	out << "Output propositions: ";
	for (size_t i = 0; i < sol.output_props.size(); ++i) {
		if (i) out << ", ";
		out << sol.output_props[i];
	}
	if (sol.output_props.empty()) out << "(none)";
	out << "\n";

	out << "\nShift-chain constraints added (" << sol.shift_chain_constraints.size() << "):\n";
	for (auto& c : sol.shift_chain_constraints)
		out << "  " << c << "\n";
	if (sol.shift_chain_constraints.empty())
		out << "  (none)\n";

	out << "\nABA consistency constraints added (" << sol.consistency_constraints.size() << "):\n";
	for (auto& c : sol.consistency_constraints)
		out << "  " << c << "\n";
	if (sol.consistency_constraints.empty())
		out << "  (none)\n";

	out << "\nLTL skeleton: " << sol.skeleton << "\n";

	if (!maybe) {
		out << "\nUNREALIZABLE\n";
		return false;
	}

	const hoa_automaton& aut = sol.aut;

	out << "\nStrategy: " << aut.num_states << " state(s), initial state "
	    << aut.initial_state << "\n";
	out << "Atomic propositions: ";
	for (size_t i = 0; i < aut.aps.size(); ++i) {
		if (i) out << ", ";
		out << aut.aps[i] << " (AP" << i << ")";
	}
	if (aut.aps.empty()) out << "(none)";
	out << "\n";

	for (int s = 0; s < aut.num_states; ++s) {
		out << "  state " << s;
		if (aut.state_accepting[s]) out << " [accepting]";
		out << ":\n";
		for (auto& e : aut.edges[s]) {
			out << "    --[" << e.guard_label << "]--> " << e.dst;
			if (e.accepting) out << " [accepting]";
			out << "\n";
		}
		if (aut.edges[s].empty())
			out << "    (no outgoing edges)\n";
	}

	// ── ABA oracle checks ────────────────────────────────────────────────
	if (!sol.atoms.empty()) {
		out << "\nABA oracle checks:\n";
		bool all_feasible = true;
		for (int s = 0; s < aut.num_states; ++s) {
			for (auto& e : aut.edges[s]) {
				// LT-20: use the SAME oracle as the real
				// pipeline (dead-edge pure-input check +
				// per-BA-type partition) -- the plain
				// existential check printed the opposite
				// verdict on dead catch-all edges.
				bool feasible = guard_is_aba_feasible<node>(
					e.guard_label, aut.aps, sol.atoms);
				out << "  state " << s << " --[" << e.guard_label
				    << "]--> " << e.dst << " : ";
				if (feasible) {
					out << "feasible\n";
				} else {
					tref guard_fm = guard_to_aba<node>(e.guard_label, aut.aps, sol.atoms);
					out << "INFEASIBLE\n";
					out << "    (formula: " << tau::get(guard_fm).to_str() << ")\n";
					all_feasible = false;
				}
			}
		}
		if (!all_feasible) {
			out << "\nUNREALIZABLE (ABA-infeasible transition)\n";
			return false;
		}
	}

	// A pure past-LTL formula's safety formula is the full ppLTLTT/Mealy encoding here, not a compiled-away shortcut.
	tref safety = encode_solution_as_safety<node>(sol);
	out << "\nSafety formula: " << tau::get(safety).to_str() << "\n";

	out << "\nREALIZABLE\n";
	return true;
}

// ── CTL* operators detection ─────────────────────────────────────────────────

template <NodeType node>
bool has_ctl_star_operators(tref fm) {
	using tau = tree<node>;
#ifdef TAU_CACHE
	using cache_t = subtree_unordered_map<node, bool>;
	static cache_t& cache = tau::template create_cache<cache_t>();
	if (auto it = cache.find(fm); it != cache.end()) return it->second;
#endif // TAU_CACHE
	bool result = tau::get(fm).find_top([](tref n) {
		const auto& t = tree<node>::get(n);
		if (!t.has_child()) return false;
		auto nt = t[0].value.nt;
		return nt == tau::wff_A || nt == tau::wff_E
		    || nt == tau::wff_semantic_neg;
	}) != nullptr;
#ifdef TAU_CACHE
	cache.emplace(fm, result);
#endif // TAU_CACHE
	return result;
}

// ── CTL* → LTL reduction ────────────────────────────────────────────────────
//
// A restricted form of the Bloem/Schewe/Khalimov reduction
// (arXiv:1711.10636), kept SOUND for synthesis at the price of completeness:
//
//   1. Bottom-up traversal of the CTL* formula tree, tracking the polarity
//      of each node and whether it is reachable from the root only through
//      universal contexts (∧, G/always, A).
//   2. `E χ` in POSITIVE polarity: fresh witness output w_i replaces E χ and
//      G(w_i → χ') is added, χ' the translated path formula. Without the
//      paper's direction outputs this constraint ranges over ALL paths, so
//      w_i asserts `A χ'`, which implies `E χ` on a non-empty tree: a
//      REALIZABLE verdict is therefore correct, an UNREALIZABLE one may be
//      over-strict (incomplete, never unsound).
//   3. `A χ` in positive polarity inside a universal context: at the root
//      state (and at every state reachable only through ∧/G from it)
//      "all paths satisfy χ" IS the synthesis semantics of χ itself, so
//      A χ reduces to χ'. `G(A φ) ≡ G φ` over a strategy tree because every
//      path from an inner node is a suffix of a root path.
//   4. Everything else -- A or E in negative polarity (under ¬, on the left
//      of →, either side of ↔/⊕, in a conditional's guard), A under an
//      existential/eventual context (∨, F, sometimes, U, ...), and `-φ` --
//      has no sound encoding here and is REFUSED with ltl_synthesis_error.
//      LA-N2: the previous `A χ ≡ ¬E¬χ` rewrite produced `¬w ∧ G(w → ¬χ)`,
//      which every strategy satisfies by holding w false, so `A` imposed
//      nothing and `A (F i1 = 1)` came out REALIZABLE.
//   5. The final LTL formula is: translated_root ∧ ⋀_i G(w_i → χ_i')

namespace ctl_star_detail {

// Counter for generating unique witness variable names.  LA-16: one per
// thread -- it is reset at the start of every reduction, and two
// concurrent reductions on a shared counter would hand out duplicate or
// skipped witness names.
static thread_local int witness_counter = 0;

inline std::string fresh_witness_name() {
	return "w_" + std::to_string(witness_counter++);
}

// Reset counter for each new reduction
inline void reset_witness_counter() {
	witness_counter = 0;
}

} // namespace ctl_star_detail

// Recursive bottom-up translation of a CTL* state/path formula to LTL.
// Witness constraints are accumulated in `constraints` (each is a G(w → χ) pair).
// New witness output names are accumulated in `witnesses`.
// `positive`: polarity of `fm` in the root formula; `universal`: `fm` is
// reachable from the root only through ∧ / always / A (see the header
// comment above for why both matter).
template <NodeType node>
static tref translate_ctl_star(tref fm,
		std::vector<std::pair<std::string, tref>>& constraints,
		std::vector<std::string>& witnesses,
		bool positive = true, bool universal = true) {
	using tau = tree<node>;
	const auto& t = tau::get(fm);
	if (!t.has_child()) return fm;

	auto nt = t[0].value.nt;

	// Handle E χ: introduce witness output
	if (nt == tau::wff_E) {
		if (!positive) throw ltl_synthesis_error(
			"E in negative polarity has no sound LTL encoding here: "
			"the witness constraint G(w -> chi) only bounds w from "
			"above, so a negated witness would be vacuous");
		tref inner = t[0].child(0);
		// Recursively translate the inner path formula (positive,
		// but no longer a universal context: w marks SOME state).
		tref translated_inner = translate_ctl_star<node>(
			inner, constraints, witnesses, true, false);
		// IN-R6: rewrite every `sometimes` inside the witness
		// constraint to its full-LTL twin `F`.  The two operators are
		// the same eventuality (LS-3), but `sometimes` is not a
		// full-LTL operator, so a constraint like
		// `G(w=1 → sometimes χ)` would route the whole reduced
		// formula into the safety pipeline, whose eventual-variable
		// transform cannot handle sometimes-under-G — while as
		// `G(w=1 → F χ)` the formula self-routes to ltlsynt, which
		// handles it natively (the sat path already ends up there).
		for (;;) {
			tref st = tau::get(translated_inner).find_top(
				is_child<node, tau::wff_sometimes>);
			if (!st) break;
			translated_inner = rewriter::replace<node>(
				translated_inner, st,
				build_wff_F<node>(tau::trim2(st)));
		}
		// Create fresh witness variable
		std::string wname = ctl_star_detail::fresh_witness_name();
		witnesses.push_back(wname);
		// Build witness as a wff: (o_w_i[t] = 1) serves as the
		// propositional witness for the E-subformula.
		// We use the Boolean carrier's type for the witness output.
		size_t carrier_tid = get_ba_type_id<node>(
			pack_bool_carrier_type<node>());
		tref w_bf = build_out_var_at_t<node>(
			build_var_name<node>(wname), carrier_tid, "t");
		tref bf_one = build_bf_t_type<node>(carrier_tid);
		tref witness_wff = tau::build_bf_eq(w_bf, bf_one);
		// Add constraint: G(witness → translated_path)
		tref implication = tau::build_wff_imply(witness_wff, translated_inner);
		tref always_constraint = tau::build_wff_always(implication);
		constraints.emplace_back(wname, always_constraint);
		return witness_wff;
	}

	// Handle A χ: only where "all paths from here" coincides with the
	// all-paths synthesis semantics of the enclosing formula (LA-N2).
	if (nt == tau::wff_A) {
		if (!positive) throw ltl_synthesis_error(
			"A in negative polarity has no sound LTL encoding here");
		if (!universal) throw ltl_synthesis_error(
			"A under an existential or eventual context (||, F, "
			"sometimes, U, R, W, S, T, E, conditional) is not "
			"soundly encodable without CTL* direction outputs; "
			"refusing rather than answering vacuously");
		return translate_ctl_star<node>(t[0].child(0), constraints,
			witnesses, true, true);
	}

	// Handle semantic negation -φ: unrealizability check.
	//
	// -φ means "φ is not realizable by the system", i.e. the environment can
	// force ¬φ.  Deciding it needs the input/output roles to be swapped for
	// the subformula, which is NOT implemented (LT-5): the case used to call
	// `apply_semantic_negation`, which only re-wraps the node — and without
	// translating the children, so any E/A inside survived too.  The
	// surviving node then reached `skeleton_wff`'s default case and, because
	// it contains io_vars, came out as the propositional constant "1", so
	// every non-constant `-φ` was answered REALIZABLE regardless of φ.
	//
	// Refuse rather than answer wrongly.  The hooks fold `-T`/`-F` before
	// anything gets here, so constant semantic negations still work.
	if (nt == tau::wff_semantic_neg) {
		throw ltl_synthesis_error(
		    "semantic negation (-) over data formulas is not implemented: "
		    "the input/output role swap it requires has no implementation, "
		    "and answering it as propositional TRUE would be unsound");
	}

	// For all other nodes, recursively translate children
	// Reconstruct the node with translated children
	auto& op = t[0];
	size_t nch = op.children_size();
	if (nch == 0) return fm;

	// Check if any child has CTL* operators
	bool has_ctl = false;
	for (size_t i = 0; i < nch; ++i) {
		if (has_ctl_star_operators<node>(op.child(i))) {
			has_ctl = true;
			break;
		}
	}
	if (!has_ctl) return fm;

	// Polarity / context of each child. Both-polarity connectives (↔, ⊕,
	// a conditional's guard) cannot host A/E soundly at all.
	auto both = [&](size_t i) {
		if (has_ctl_star_operators<node>(op.child(i)))
			throw ltl_synthesis_error("A/E under a both-polarity "
				"connective (<->, ^, conditional guard) has no "
				"sound LTL encoding here");
	};
	std::vector<std::pair<bool,bool>> ctx(nch, {positive, false});
	switch (nt) {
	case tau::wff_and:
	case tau::wff_always:
		for (auto& c : ctx) c = {positive, universal};
		break;
	case tau::wff_neg:
	case tau::wff_imply:
		ctx[0] = {!positive, false};
		break;
	case tau::wff_rimply:
		if (nch == 2) ctx[1] = {!positive, false};
		break;
	case tau::wff_equiv:
	case tau::wff_xor:
		for (size_t i = 0; i < nch; ++i) both(i);
		break;
	case tau::wff_conditional:
		both(0);
		break;
	default: // or, sometimes, F, U, R, W, S, T: positive, not universal
		break;
	}

	// Translate children and rebuild
	std::vector<tref> new_children;
	new_children.reserve(nch);
	for (size_t i = 0; i < nch; ++i) {
		new_children.push_back(translate_ctl_star<node>(op.child(i),
			constraints, witnesses, ctx[i].first, ctx[i].second));
	}

	// Rebuild node with same operator but new children
	if (nch == 1) {
		// Unary operators: neg, sometimes, always, F
		switch (nt) {
		case tau::wff_neg:      return tau::build_wff_neg(new_children[0]);
		case tau::wff_sometimes:return tau::build_wff_sometimes(new_children[0]);
		case tau::wff_always:   return tau::build_wff_always(new_children[0]);
		case tau::wff_F:        return tau::build_wff_F(new_children[0]);
		default:                break; // falls to the LT-13 LOG_ERROR
		}
	} else if (nch == 2) {
		// Binary operators
		switch (nt) {
		case tau::wff_and:   return tau::build_wff_and(new_children[0], new_children[1]);
		case tau::wff_or:    return tau::build_wff_or(new_children[0], new_children[1]);
		case tau::wff_imply: return tau::build_wff_imply(new_children[0], new_children[1]);
		case tau::wff_equiv: return tau::build_wff_equiv(new_children[0], new_children[1]);
		case tau::wff_xor:   return tau::build_wff_xor(new_children[0], new_children[1]);
		case tau::wff_U:     return tau::build_wff_U(new_children[0], new_children[1]);
		case tau::wff_R:     return tau::build_wff_R(new_children[0], new_children[1]);
		case tau::wff_W:     return tau::build_wff_W(new_children[0], new_children[1]);
		case tau::wff_S:     return tau::build_wff_S(new_children[0], new_children[1]);
		case tau::wff_T:     return tau::build_wff_T(new_children[0], new_children[1]);
		// LT-13: rimply was missing -- `phi <- E psi` kept its E
		// untranslated and later collapsed to "1" in the skeleton
		case tau::wff_rimply: return tau::build_wff_rimply(
					new_children[0], new_children[1]);
		default:             break;
		}
	} else if (nch == 3 && nt == tau::wff_conditional) {
		return tau::build_wff_conditional(
			new_children[0], new_children[1], new_children[2]);
	}
	// LT-13 / IN-1: a silent identity here left embedded A/E/- untranslated
	// in any connective missing from the switches above; the survivor then
	// reached the skeleton (constant "1") or bounced between
	// is_tau_formula_sat and is_ltl_aba_realizable. Refuse instead.
	LOG_ERROR << "translate_ctl_star: unhandled connective "
		<< node::name(nt) << " with CTL* content in its subtree";
	throw ltl_synthesis_error(std::string("translate_ctl_star: unhandled "
		"connective ") + node::name(nt) + " with CTL* content in its "
		"subtree; the formula cannot be reduced to LTL");
}

// True iff the formula contains a `wff_semantic_neg` node.
template <NodeType node>
bool has_semantic_negation(tref fm) {
	using tau = tree<node>;
	return tau::get(fm).find_top([](tref n) {
		const auto& t = tree<node>::get(n);
		if (!t.has_child()) return false;
		return t[0].value.nt == tau::wff_semantic_neg;
	}) != nullptr;
}

template <NodeType node>
ctl_star_reduction<node> reduce_ctl_star_to_ltl(tref fm) {
	using tau = tree<node>;
	ctl_star_detail::reset_witness_counter();

	std::vector<std::pair<std::string, tref>> constraints;
	std::vector<std::string> witnesses;

	tref translated = translate_ctl_star<node>(fm, constraints, witnesses);

	// Build the conjunction: translated_root ∧ constraint_1 ∧ ... ∧ constraint_n
	tref result = translated;
	for (auto& [name, constraint] : constraints) {
		result = tau::build_wff_and(result, constraint);
	}

	// IN-R6: every witness is built over the Boolean carrier's type (see
	// the wff_E case in translate_ctl_star, which builds w_bf over the
	// same pack_bool_carrier_type<node>()); record the type ids so the
	// interpreter can register the streams without re-deriving them.
	std::vector<size_t> witness_types(witnesses.size(),
		get_ba_type_id<node>(pack_bool_carrier_type<node>()));

	return ctl_star_reduction<node>{result, witnesses,
		std::move(witness_types)};
}

// ── Semantic negation implementation ─────────────────────────────────────────
//
// -φ means "there is no winning system strategy satisfying φ", i.e. the
// environment wins for ¬φ.  In synthesis terms: swap the input/output roles
// and synthesize ¬φ.
//
// NOT IMPLEMENTED (LT-5).  This function is an AST constructor and nothing
// more: it wraps `fm` in a `wff_semantic_neg` node.  The previous comment
// claimed "the actual role swap happens at the synthesis call site" — there is
// no such site anywhere in src/, which is exactly why every non-constant `-φ`
// used to be decided as propositional TRUE.  `translate_ctl_star` and
// `is_ltl_aba_realizable` now reject a surviving `wff_semantic_neg` instead.

template <NodeType node>
tref apply_semantic_negation(tref fm) {
	using tau = tree<node>;
	return tau::build_wff_semantic_neg(fm);
}

} // namespace idni::tau_lang
