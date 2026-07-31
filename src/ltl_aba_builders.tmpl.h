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
	return std::nullopt;
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


template <NodeType node>
static std::optional<LtlAbaSolution<node>>
solve_ltl_aba(tref fm)
{
	// Past operators (S, T) are handled at skeleton level via DFA temporal
	// testers (ppLTLTT approach), not by AST-level compile-away.  The
	// formula is passed to extract_data_atoms unchanged — S/T nodes are
	// transparent to atom extraction (they are temporal operators, not
	// data atoms).  The skeleton generation intercepts S/T and emits
	// DFA state variables + constraints instead.
	LtlAbaSolution<node> sol;
	sol.atoms = extract_data_atoms<node>(fm);

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
			fm, sol.atoms); claim)
				return *claim;


	if (sol.atoms.empty()) {
		// Purely propositional: no io_var atoms.
		if (has_past) {
			std::vector<PastTemporalTester> testers;
			sol.skeleton = skeleton_str_with_testers<node>(fm, sol.atoms, testers);
			append_tester_constraints(sol.skeleton, testers);
			std::vector<std::string> out_props;
			for (const auto& t : testers)
				out_props.push_back(t.state_var);
			auto [real, hoa] = call_ltlsynt(sol.skeleton, {}, out_props);
			if (!real) return std::nullopt;
			sol.aut = parse_hoa(hoa);
			return sol;
		}
		sol.skeleton = ltl_skeleton<node>(fm, sol.atoms);
		auto [real, hoa] = call_ltlsynt(sol.skeleton, {}, {});
		if (!real) return std::nullopt;
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

	// Build skeleton with DFA temporal testers for past operators (S, T).
	// The ppLTLTT approach (Azzopardi et al., ATVA'23) replaces each S/T
	// subformula with a fresh propositional state variable and encodes
	// the DFA transition + initial condition as pure future-LTL constraints
	// (G, X, propositional) that ltlsynt handles natively.
	auto [skel, testers] = ltl_skeleton_with_testers<node>(fm, sol.atoms);
	sol.skeleton = std::move(skel);
	add_consistency_constraints<node>(sol.atoms, sol.skeleton,
		nullptr, has_past /*polarity_complete*/);

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
	if (!realizable) return std::nullopt;

	sol.aut = parse_hoa(hoa_text);
	return sol;
}

// ── is_ltl_aba_realizable ─────────────────────────────────────────────────────

template <NodeType node>
bool is_ltl_aba_realizable(tref fm, int_t start_time, bool output) {
	LOG_DEBUG << "[ltl_aba] is_ltl_aba_realizable: " << LOG_FM(fm);

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

	// Trivially realizable: no states produced.
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
static tref encode_mealy_as_safety(const LtlAbaSolution<node>& sol)
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
encode_mealy_initial_conditions(const LtlAbaSolution<node>& sol,
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

// ── ltl_to_safety_formula ─────────────────────────────────────────────────────
//
// `_full` does the work and returns BOTH the safety formula AND the
// LtlAbaSolution (when one was synthesised). The interpreter caches the
// solution so it can introspect the Mealy state at runtime, visualise the
// strategy, etc. — info that would otherwise be discarded after encoding.
//
// The thin wrapper `ltl_to_safety_formula(fm)` discards the solution to
// preserve the existing single-return API for callers that don't need it.

template <NodeType node>
std::tuple<tref, std::optional<LtlAbaSolution<node>>>
ltl_to_safety_formula_full(tref fm) {
	using tau = tree<node>;
	LOG_DEBUG << "[ltl_aba] ltl_to_safety_formula: " << LOG_FM(fm);

	// Fast path: if all LTL operators are past (S/T), compile them away and
	// return G(curr && rhs) safety invariants for each S operator.
	// No Mealy synthesis is needed on this path; the returned solution is empty.
	{
		auto [compiled_fast, safety_fm, init_fm, _aux] = compile_since_trigger<node>(fm);
		if (!has_ltl_operators<node>(compiled_fast)) {
			LOG_DEBUG << "[ltl_aba] ltl_to_safety_formula: "
			          << "pure past-LTL, returning safety formula";
			return {tau::build_wff_and(safety_fm, init_fm), std::nullopt};
		}
	}

	auto maybe = solve_ltl_aba<node>(fm);
	if (!maybe) {
		LOG_DEBUG << "[ltl_aba] ltl_to_safety_formula: not realizable";
		return {nullptr, std::nullopt};
	}

	auto& sol = *maybe;

	// Purely propositional: realizable but no data constraints to encode.
	if (sol.atoms.empty()) return {tau::_T(), std::move(sol)};

	const auto& aut = sol.aut;

	// Trivially realizable: empty automaton.
	if (aut.num_states == 0) return {tau::_T(), std::move(sol)};

	if (aut.num_states > 1) {
		LOG_INFO << "[ltl_aba] Multi-state strategy ("
		         << aut.num_states
		         << " states) — encoding with auxiliary one-hot state bits";
		tref encoded = encode_mealy_as_safety<node>(sol);
		return {encoded, std::move(sol)};
	}

	// Single-state strategy: the self-loop guard is the perpetual output constraint.
	if (aut.edges.empty() || aut.edges[0].empty())
		return {tau::build_wff_always(tau::_T()), std::move(sol)};

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
	return {tau::build_wff_always(simplified), std::move(sol)};
}

template <NodeType node>
tref ltl_to_safety_formula(tref fm) {
	auto [safety, _sol] = ltl_to_safety_formula_full<node>(fm);
	return safety;
}

// ── ltl_explain ───────────────────────────────────────────────────────────────

template <NodeType node>
bool ltl_explain(tref fm, std::ostream& out) {
	using tau = tree<node>;

	if (!has_ltl_operators<node>(fm)) {
		out << "Formula has no LTL operators (treated as G(phi))\n";
		// Fall through to the existing safety pipeline.
		bool sat = is_tau_formula_sat<node>(fm, 0, false);
		out << (sat ? "REALIZABLE" : "UNREALIZABLE") << "\n";
		return sat;
	}

	// ── Data atoms (S/T handled at skeleton level via ppLTLTT testers) ───
	auto atoms = extract_data_atoms<node>(fm);
	out << "\nData atoms (" << atoms.size() << "):\n";
	for (auto& [f, name] : atoms)
		out << "  " << name << "  :=  " << tau::get(f).to_str() << "\n";

	// ── Input / output classification ─────────────────────────────────────
	std::vector<std::string> input_props, output_props;
	for (auto& [f, name] : atoms) {
		if (is_pure_input_atom<node>(f))
			input_props.push_back(name);
		else
			output_props.push_back(name);
	}
	out << "\nInput propositions:  ";
	for (size_t i = 0; i < input_props.size(); ++i) {
		if (i) out << ", ";
		out << input_props[i];
	}
	if (input_props.empty()) out << "(none)";
	out << "\n";

	out << "Output propositions: ";
	for (size_t i = 0; i < output_props.size(); ++i) {
		if (i) out << ", ";
		out << output_props[i];
	}
	if (output_props.empty()) out << "(none)";
	out << "\n";

	// ── Skeleton with DFA temporal testers (ppLTLTT approach) ────────────
	auto [skeleton, testers] = ltl_skeleton_with_testers<node>(fm, atoms);
	std::vector<std::string> consistency_constraints;
	add_consistency_constraints<node>(atoms, skeleton, &consistency_constraints,
		has_past_operators<node>(fm) /*polarity_complete*/);
	append_tester_constraints(skeleton, testers);
	for (const auto& t : testers)
		output_props.push_back(t.state_var);

	out << "\nABA consistency constraints added (" << consistency_constraints.size() << "):\n";
	for (auto& c : consistency_constraints)
		out << "  " << c << "\n";
	if (consistency_constraints.empty())
		out << "  (none)\n";

	if (!testers.empty()) {
		out << "\nppLTLTT temporal testers (" << testers.size() << "):\n";
		for (const auto& t : testers)
			out << "  " << t.state_var << ": G(X(" << t.state_var
			    << ") <-> " << t.transition << ")"
			    << (t.negate_output ? " [negated output]" : "") << "\n";
	}

	// ── LTL skeleton ─────────────────────────────────────────────────────
	out << "\nLTL skeleton: " << skeleton << "\n";

	// ── Call ltlsynt ─────────────────────────────────────────────────────
	out << "\nCalling ltlsynt...\n";
	auto [realizable, hoa_text] = call_ltlsynt(skeleton, input_props, output_props);

	if (!realizable) {
		out << "\nUNREALIZABLE\n";
		return false;
	}

	// ── Parse HOA strategy ───────────────────────────────────────────────
	HoaAutomaton aut = parse_hoa(hoa_text);

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
	if (!atoms.empty()) {
		out << "\nABA oracle checks:\n";
		bool all_feasible = true;
		for (int s = 0; s < aut.num_states; ++s) {
			for (auto& e : aut.edges[s]) {
				tref guard_fm = guard_to_aba<node>(e.guard_label, aut.aps, atoms);
				bool feasible = aba_existential_feasible<node>(guard_fm);
				out << "  state " << s << " --[" << e.guard_label
				    << "]--> " << e.dst << " : ";
				if (feasible) {
					out << "feasible\n";
				} else {
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

	// ── Safety formula ───────────────────────────────────────────────────
	// Re-derive via solve path (atoms already computed above, just encode)
	tref safety = ltl_to_safety_formula<node>(fm);
	if (safety) {
		out << "\nSafety formula: " << tau::get(safety).to_str() << "\n";
	} else {
		out << "\nSafety formula: (not encodable as simple always(phi))\n";
	}

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
// Implements the Bloem/Schewe/Khalimov reduction (arXiv:1711.10636).
//
// Algorithm:
//   1. Bottom-up traversal of the CTL* formula tree
//   2. For each E χ subformula:
//      - Create a fresh witness output variable w_i
//      - Replace E χ with w_i
//      - Add constraint G(w_i → translate_path(χ))
//   3. For each A χ subformula:
//      - Rewrite to ¬(E ¬χ) and apply step 2
//   4. The final LTL formula is: translated_root ∧ ⋀_i G(w_i → χ_i_LTL)

namespace ctl_star_detail {

// Counter for generating unique witness variable names
inline int witness_counter = 0;

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
template <NodeType node>
static tref translate_ctl_star(tref fm,
		std::vector<std::pair<std::string, tref>>& constraints,
		std::vector<std::string>& witnesses) {
	using tau = tree<node>;
	const auto& t = tau::get(fm);
	if (!t.has_child()) return fm;

	auto nt = t[0].value.nt;

	// Handle E χ: introduce witness output
	if (nt == tau::wff_E) {
		tref inner = t[0].child(0);
		// Recursively translate the inner path formula
		tref translated_inner = translate_ctl_star<node>(
			inner, constraints, witnesses);
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

	// Handle A χ: rewrite as ¬(E ¬χ)
	if (nt == tau::wff_A) {
		tref inner = t[0].child(0);
		// A χ ≡ ¬E¬χ
		tref negated_inner = tau::build_wff_neg(inner);
		// Build E(¬χ) and translate it
		tref e_neg = tau::build_wff_E(negated_inner);
		tref translated_e = translate_ctl_star<node>(
			e_neg, constraints, witnesses);
		// Return ¬(translated E(¬χ))
		return tau::build_wff_neg(translated_e);
	}

	// Handle semantic negation -φ: unrealizability check
	// -φ means "φ is not realizable by the system"
	// Equivalent to: the environment can force ¬φ
	// We encode as: swap inputs/outputs in the inner formula
	if (nt == tau::wff_semantic_neg) {
		tref inner = t[0].child(0);
		return apply_semantic_negation<node>(inner);
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

	// Translate children and rebuild
	std::vector<tref> new_children;
	new_children.reserve(nch);
	for (size_t i = 0; i < nch; ++i) {
		new_children.push_back(
			translate_ctl_star<node>(op.child(i), constraints, witnesses));
	}

	// Rebuild node with same operator but new children
	if (nch == 1) {
		// Unary operators: neg, sometimes, always, F
		switch (nt) {
		case tau::wff_neg:      return tau::build_wff_neg(new_children[0]);
		case tau::wff_sometimes:return tau::build_wff_sometimes(new_children[0]);
		case tau::wff_always:   return tau::build_wff_always(new_children[0]);
		case tau::wff_F:        return tau::build_wff_F(new_children[0]);
		default:                return fm;
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
		default:             return fm;
		}
	} else if (nch == 3 && nt == tau::wff_conditional) {
		return tau::build_wff_conditional(
			new_children[0], new_children[1], new_children[2]);
	}
	return fm;
}

template <NodeType node>
CtlStarReduction<node> reduce_ctl_star_to_ltl(tref fm) {
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

	return CtlStarReduction<node>{result, witnesses};
}

// ── Semantic negation implementation ─────────────────────────────────────────
//
// -φ means "there is no winning system strategy satisfying φ"
// Equivalent to: the environment wins for ¬φ
// In synthesis terms: swap input/output roles and synthesize ¬φ
//
// For the AST level, semantic negation rewrites the formula to prepare it for
// synthesis with swapped roles. The actual role swap happens at the synthesis
// call site.

template <NodeType node>
tref apply_semantic_negation(tref fm) {
	using tau = tree<node>;
	// Semantic negation at the AST level is represented as wff_semantic_neg.
	// The actual player-role swap (input ↔ output) is performed at the
	// synthesis layer when the satisfiability / realizability check is
	// invoked. Here we simply wrap the formula in the semantic negation
	// node so downstream passes can detect and handle it.
	return tau::build_wff_semantic_neg(fm);
}

} // namespace idni::tau_lang
