// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// ltl_aba_builders.tmpl.h - Algorithms A/B/C/D, solve, realize, safety, explain, CTL*
// Split from ltl_aba.tmpl.h for readability.

namespace idni::tau_lang {


/**
 * @brief `true` when @p BA synthesises a strategy for its own propositional
 *        formulas. Declared here rather than in ba_descriptor.h because the
 *        result type is the LTL solution's.
 */
template <typename Node, typename BA>
concept ba_has_propositional_synthesis = ba_has_descriptor_v<Node, BA>
	&& requires(tref fm,
		const std::vector<std::pair<tref, std::string>>& atoms) {
		{ ba_descriptor<BA, Node>::try_propositional_synthesis(fm, atoms) }
			-> std::convertible_to<result<propositional_synthesis<Node>>>; };

/** @brief How many BAs of @p Node's pack declare the capability. */
template <typename Node>
constexpr std::size_t pack_propositional_synthesizer_count() {
	return []<std::size_t... Is>(std::index_sequence<Is...>) {
		return (std::size_t{0} + ... + std::size_t{
			ba_has_propositional_synthesis<Node,
				std::tuple_element_t<Is, typename Node::bas_tuple>>});
	}(std::make_index_sequence<std::tuple_size_v<typename Node::bas_tuple>>{});
}

template <typename Node, typename BA>
static result<propositional_synthesis<Node>> ba_try_propositional_synthesis(
	tref fm, const std::vector<std::pair<tref, std::string>>& atoms)
{
	if constexpr (ba_has_propositional_synthesis<Node, BA>)
		return ba_descriptor<BA, Node>::try_propositional_synthesis(fm, atoms);
	// result's value constructor is explicit
	return result<propositional_synthesis<Node>>{synthesis_declined<Node>()};
}

// Resolution: the single BA declaring the capability, which takes only the
// formula and so must recognise its own; a second declarer is refused at
// compile time, so no build can resolve two claimants by pack order.
template <typename Node>
static result<propositional_synthesis<Node>> pack_try_propositional_synthesis(
	tref fm, const std::vector<std::pair<tref, std::string>>& atoms)
{
	static_assert(pack_propositional_synthesizer_count<Node>() <= 1,
		"two BAs declare try_propositional_synthesis");
	result<propositional_synthesis<Node>> r;
	propositional_synthesis<Node> out;
	pack_visit_all<Node>([&]<typename BA>() {
		// an undecided BA is not a claimant: its report is the answer
		auto got = r.merge_take(
			ba_try_propositional_synthesis<Node, BA>(fm, atoms));
		if (!got) return;
		if (*got && !out) out = std::move(*got);
	});
	if (r.has_error()) return r;
	return r.with_value(std::move(out));
}


// X^n(inner), used by append_step_guard_drivers.
static std::string nest_x(std::string inner, int_t n) {
	for (int_t i = 0; i < n; ++i) inner = "X(" + inner + ")";
	return inner;
}

// Drives each step_guard_prop(k) low for steps [0, k) and G-true from k
// on, !g & X(!g) & ... & X^k(G(g)), registered as an output the way
// apply_step_counter_encoding drives its own bits.
template <NodeType node>
static void append_step_guard_drivers(ltl_aba_solution<node>& sol,
	const std::set<int_t>& step_guards)
{
	for (int_t k : step_guards) {
		std::string g = step_guard_prop(k);
		sol.output_props.push_back(g);
		sol.step_guard_ks.push_back(k);
		for (int_t i = 0; i < k; ++i)
			sol.skeleton += " & " + nest_x("!" + g, i);
		sol.skeleton += " & " + nest_x("G(" + g + ")", k);
		LOG_DEBUG << "[ltl_aba] step guard " << g << " from step " << k;
	}
}

// partial_out, when non-null, stays populated even when the return value ends up std::nullopt.
template <NodeType node>
static result<std::optional<ltl_aba_solution<node>>>
solve_ltl_aba(tref fm, ltl_aba_solution<node>* partial_out)
{
	using tau = tree<node>;
	result<std::optional<ltl_aba_solution<node>>> r;

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
	// atoms are all positional (max-position hoisting); refuses (as a
	// result<> error) for the two temporary cases -- a conjunct mixing
	// positional and relative-time atoms, or a positional atom under
	// F/U/R/W/S/T (see the function doc).
	TAU_TRY(std::vector<tref> hoist_conjuncts,
		collect_hoist_conjuncts<node>(fm, sol.atoms));

	// Past operators (S, T) require the ppLTLTT temporal tester encoding
	// in the default path.  Algorithm A/B/D use ltl_skeleton() which
	// passes S/T through as literal operators — those paths don't have
	// the DFA state-variable machinery.  Skip them when S/T are present.
	const bool has_past = has_past_operators<node>(fm);

	// Ask whichever BA owns these atoms to synthesise propositionally. Past
	// operators need the ppLTLTT temporal-tester encoding of the default path,
	// which those fast paths do not have, so they are not offered the formula.
	// Same for a formula needing a __step_ge guard: ltl_skeleton(), which the
	// fast paths use, never drives one.
	if (ltl_propositional_synthesis && !has_past
		&& collect_step_guards<node>(fm).empty())
	{
		TAU_TRY(auto claim, pack_try_propositional_synthesis<node>(
			fm, sol.atoms));
		if (claim) {
			if (!*claim && partial_out) *partial_out = sol;
			return r.with_value(std::move(*claim));
		}
	}


	if (sol.atoms.empty()) {
		// Purely propositional: no io_var atoms.
		if (has_past) {
			std::vector<past_temporal_tester> testers;
			TAU_TRY(sol.skeleton, skeleton_str_with_testers<node>(fm, sol.atoms, testers));
			append_tester_constraints(sol.skeleton, testers);
			for (const auto& t : testers)
				sol.output_props.push_back(t.state_var);
			append_step_guard_drivers<node>(sol, collect_step_guards<node>(fm));
			TAU_TRY(auto ltlsynt_out, call_ltlsynt(sol.skeleton, {}, sol.output_props));
			auto& [real, hoa] = ltlsynt_out;
			if (!real) {
				if (partial_out) *partial_out = sol;
				return r.with_value(std::nullopt);
			}
			TAU_TRY(sol.aut, parse_hoa(hoa));
			return r.with_value(std::move(sol));
		}
		TAU_TRY(sol.skeleton, ltl_skeleton<node>(fm, sol.atoms));
		TAU_TRY(auto ltlsynt_out, call_ltlsynt(sol.skeleton, {}, {}));
		auto& [real, hoa] = ltlsynt_out;
		if (!real) {
			if (partial_out) *partial_out = sol;
			return r.with_value(std::nullopt);
		}
		TAU_TRY(sol.aut, parse_hoa(hoa));
		return r.with_value(std::move(sol));
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
	TAU_TRY(std::string step_counter_extra, apply_step_counter_encoding<node>(
		hoist_conjuncts, sol.atoms, sol.input_props, sol.output_props,
		sol.counter_highest_initial_pos, sol.counter_relativized_props,
		sol.counter_gated_props, sol.counter_bits));

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
	TAU_TRY(auto skel_and_testers, ltl_skeleton_with_testers<node>(fm_for_skeleton, sol.atoms));
	auto& [skel, testers] = skel_and_testers;
	sol.skeleton = std::move(skel) + step_counter_extra;
	std::string game_skeleton = sol.skeleton;

	// Cross-step shift-chain constraints: tie shifted instances of the same
	// signal together (e.g. o1[t]=1 and o1[t-1]!=1) before the consistency
	// pass, which folds the resulting input-only assumptions into its own
	// wrap so both sets combine behind a single implication.
	std::string shift_chain_input_assumptions;
	if (ltl_input_twins)
		add_input_twins<node>(sol.atoms, sol.input_props);
	TAU_TRY_VOID(add_shift_chain_constraints<node>(sol.atoms, sol.skeleton,
		shift_chain_input_assumptions, &sol.shift_chain_constraints));

	TAU_TRY_VOID(add_consistency_constraints<node>(sol.atoms, sol.skeleton,
		&sol.consistency_constraints, has_past /*polarity_complete*/,
		std::move(shift_chain_input_assumptions)));
	if (ltl_observed_abstraction)
		add_present_twins<node>(sol.atoms, sol.output_props, sol.skeleton);
	const size_t constraints_end = sol.skeleton.size();

	// Append DFA tester constraints and register state variables as outputs.
	append_tester_constraints(sol.skeleton, testers);
	for (const auto& t : testers) {
		sol.output_props.push_back(t.state_var);
		LOG_DEBUG << "[ltl_aba] ppLTLTT tester: " << t.state_var
		          << " init=" << t.initial_value
		          << " trans=" << t.transition
		          << " negate=" << t.negate_output;
	}
	append_step_guard_drivers<node>(sol, collect_step_guards<node>(fm_for_skeleton));
	if (!ltl_observed_abstraction)
		sol.game_skeleton = game_skeleton
			+ sol.skeleton.substr(constraints_end);

	LOG_DEBUG << "[ltl_aba] LTL skeleton: " << sol.skeleton;
	LOG_DEBUG << "[ltl_aba] inputs:  " << [&]{
		std::string s; for (auto& p : sol.input_props)  s += p + " "; return s; }();
	LOG_DEBUG << "[ltl_aba] outputs: " << [&]{
		std::string s; for (auto& p : sol.output_props) s += p + " "; return s; }();

	TAU_TRY(auto ltlsynt_out,
	    call_ltlsynt(sol.skeleton, sol.input_props, sol.output_props));
	auto& [realizable, hoa_text] = ltlsynt_out;
	if (!realizable) {
		if (partial_out) *partial_out = sol;
		return r.with_value(std::nullopt);
	}

	TAU_TRY(sol.aut, parse_hoa(hoa_text));
	TAU_TRY_VOID(gate_counter_props<node>(sol));
	return r.with_value(std::move(sol));
}

// The oracle part of the realizability check, shared with execution: checks
// every edge of sol's strategy against the ABA (and the window oracle for
// relations spanning several steps), blocks an infeasible edge and
// re-synthesizes, until a strategy passes (true, sol holds it) or ltlsynt
// finds none (false). An error is undecided.
template <NodeType node>
static result<bool> refine_ltl_aba_solution(ltl_aba_solution<node>& sol,
	bool output, bool* lost = nullptr)
{
	result<bool> r;
	auto backend_failed = [&]() -> result<bool> {
		return r.with_error(code::solver_error,
			messages::unknown_realizability_no_verdict);
	};
	LOG_DEBUG << "[ltl_aba] atoms=" << sol.atoms.size()
	          << " states=" << sol.aut.num_states;

	// Trivially realizable: no states produced. parse_hoa refuses a
	// strategy with fewer than one state (LA-8), so only solutions an
	// algorithm constructed deliberately without an automaton (the
	// constant-output fast path) reach this branch.
	if (sol.aut.num_states == 0) {
		if (output) LOG_INFO << "[ltl_aba] REALIZABLE";
		return r.with_value(true);
	}

	// Purely propositional (no data atoms) — ltlsynt verdict is final.
	if (sol.atoms.empty()) {
		if (output) LOG_INFO << "[ltl_aba] REALIZABLE (propositional)";
		return r.with_value(true);
	}

	LOG_DEBUG << "[ltl_aba] strategy has " << sol.aut.num_states << " state(s)";

	// ABA oracle: verify each strategy transition guard is ABA-feasible.
	// ABA inconsistency between proposition pairs has already been handled
	// by the consistency constraints added to the skeleton (see solve_ltl_aba).
	// Here we only check individual guards via existential satisfiability.
	//
	// A rejection means only THIS strategy is bad, not that none exists:
	// block the edge's infeasible atom combinations as system-side
	// conjuncts (sound -- the ABA rules them out) and re-synthesize, bounded.
	auto check_edges = [&]() -> std::optional<std::pair<size_t, size_t>> {
		for (size_t s = 0; s < sol.aut.edges.size(); ++s)
			for (size_t ei = 0; ei < sol.aut.edges[s].size(); ++ei) {
				auto& e = sol.aut.edges[s][ei];
				LOG_DEBUG << "[ltl_aba] checking edge " << s << "->[" << e.guard_label << "]->" << e.dst;
				auto feas_r = guard_is_aba_feasible<node>(
				        e.guard_label, sol.aut.aps, sol.atoms);
				bool feasible = feas_r.has_value() && feas_r.value();
				r.append(std::move(feas_r).report());
				if (!feasible) return std::make_pair(s, ei);
			}
		return std::nullopt;
	};

	// r carries an UNKNOWN-branded error, not a value -- undecided must
	// never be read as a false verdict, nor be printed as an empty report.
	auto undecided = [&](const char* why) {
		if (output) LOG_INFO << "[ltl_aba] UNKNOWN (" << why << ")";
		return r.with_error(code::solver_error, why);
	};

	auto realizable_now = [&]() {
		if (output) LOG_INFO << "[ltl_aba] REALIZABLE";
		return r.with_value(true);
	};

	// Runtime parameter (ltl_max_refinement_rounds(); 0 = unlimited): each
	// round blocks one infeasible edge and re-runs ltlsynt.
	const size_t max_refinement_rounds = ltl_max_refinement_rounds();
	for (size_t round = 0; ; ++round) {
		std::vector<std::string> clauses;
		if (auto rejected = check_edges()) {
			auto& e = sol.aut.edges[rejected->first][rejected->second];
			LOG_DEBUG << "[ltl_aba] ABA infeasible edge " << rejected->first
			          << "→" << e.dst << " guard=[" << e.guard_label << "]";
			TAU_TRY(auto products, guard_infeasible_products<node>(
					e.guard_label, sol.aut.aps, sol.atoms));
			for (auto& product : products)
				if (!product.empty())
					clauses.push_back("G(!(" + product_clause_text(product) + "))");
		} else {
			// Single edges all pass; a relation spanning >= 3 consecutive
			// steps is invisible to the per-edge check, so ask the window
			// oracle, then play the strategy against the data.
			int_t W = 1 + max_atom_lookback<node>(sol.atoms);
			if (W > 1) {
				TAU_TRY(auto wres, window_infeasible_paths<node>(sol, W,
					ltl_window_max_paths()));
				if (wres.path_cap_reached)
					return undecided("window oracle path cap");
				clauses = std::move(wres.blocking_clauses);
			}
			if (clauses.empty()) {
				size_t rounds = 0;
				TAU_TRY(auto dverdict, strategy_wins_on_data<node>(sol,
					max_refinement_rounds, rounds));
				switch (dverdict)
				{
				case strategy_data_verdict::wins:
					return realizable_now();
				case strategy_data_verdict::undecided:
					return undecided("the strategy could not be "
						"checked against the data");
				case strategy_data_verdict::loses:
					break;
				}
				// A losing strategy may take a path no data realizes
				// that is longer than W; such a path is blocked like
				// any other. A path the data realizes only when the
				// system picks what the environment picks gives no
				// clause, and the verdict stays open.
				for (int_t w = W + 1; clauses.empty()
					&& w <= W + (int_t)rounds + 1; ++w)
				{
					TAU_TRY(auto wres, window_infeasible_paths<node>(sol, w,
						ltl_window_max_paths()));
					if (wres.path_cap_reached) break;
					clauses = std::move(wres.blocking_clauses);
				}
				if (clauses.empty() && sol.observed) {
					TAU_TRY(auto obs, add_forceability_observations<node>(sol));
					clauses = std::move(obs);
				}
				if (clauses.empty()) {
					if (lost) *lost = true;
					return undecided("the strategy loses against the "
						"data and no blocking clause was found");
				}
			}
		}

		if (max_refinement_rounds && round >= max_refinement_rounds)
			return undecided("ABA refinement bound reached "
				"(--ltl-refinement-rounds / `set ltlrefinementrounds`, "
				"0 = unlimited)");
		bool added_new = false;
		for (auto& clause : clauses) {
			if (sol.skeleton.find(clause) != std::string::npos) continue;
			LOG_DEBUG << "[ltl_aba] ABA refinement round " << round << ": " << clause;
			sol.skeleton += " && " + clause;
			sol.consistency_constraints.push_back(clause);
			added_new = true;
		}
		if (!added_new) return undecided("ABA refinement bound reached");

		auto ltlsynt_opt = r.merge_take(
			call_ltlsynt(sol.skeleton, sol.input_props, sol.output_props));
		if (!ltlsynt_opt) return backend_failed();
		auto& [ok, hoa] = *ltlsynt_opt;
		if (!ok) {
			if (sol.observation_props.empty()) return r.with_value(false);
			return undecided("no strategy wins once the claims the "
				"data cannot force are observed, but the observations "
				"give the environment choices the data may not allow");
		}
		auto aut_opt = r.merge_take(parse_hoa(hoa));
		if (!aut_opt) return backend_failed();
		sol.aut = std::move(*aut_opt);
		TAU_TRY_VOID(gate_counter_props<node>(sol));
	}
}

// Refines `sol`. When its strategy loses against the data and no path can
// be blocked, the formula is solved again as the observed abstraction and
// that strategy is refined, observations added as it loses; `sol` then
// holds the observed solution. The first attempt is a rejected candidate.
template <NodeType node>
static result<bool> refine_or_observe(tref fm, ltl_aba_solution<node>& sol,
	bool output)
{
	using tau = tree<node>;
	result<bool> r;
	bool lost = false;
	auto first = refine_ltl_aba_solution<node>(sol, output, &lost);
	if (first.has_value() || !lost) return first;
	{
		auto sc = r.open("rejected candidate");
		r.info("the strategy loses against the data; the formula is "
			"solved again with observations",
			{{label::value, truncate_for_message(tau::get(fm).to_str())}});
		report rep = std::move(first).report();
		rep.demote_errors_to_warnings();
		r.append(std::move(rep));
	}
	std::optional<std::optional<ltl_aba_solution<node>>> second;
	{
		const bool outer = ltl_observed_abstraction;
		ltl_observed_abstraction = true;
		second = r.merge_take(solve_ltl_aba<node>(fm));
		ltl_observed_abstraction = outer;
	}
	if (!second) return r;
	if (!*second) return r.with_error(code::solver_error,
		"UNKNOWN: the strategy loses against the data and the observed "
		"abstraction has none; realizability could not be decided");
	auto& observed = **second;
	observed.observed = true;
	auto refined = r.merge_take(
		refine_ltl_aba_solution<node>(observed, output));
	if (!refined) return r;
	if (*refined) sol = std::move(observed);
	return r.with_value(*refined);
}

// Whether every input atom reads a single step. The environment of the
// abstraction sets an input prop at the step that reads it; props of
// different steps are tied only by the shift chains, and one reading a past
// step is tied to the step it reads only through its present-time twin
// (add_input_twins). With single-step atoms and their twins, each step's
// input props are one valuation the data of that step can take.
template <NodeType node>
static bool input_atoms_read_one_step(
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	for (auto& [atom, _] : atoms)
		if (is_pure_input_atom<node>(atom)
			&& (atom_is_positional<node>(atom)
				|| !atom_uniform_shift<node>(atom)))
					return false;
	return true;
}

// ── is_ltl_aba_realizable ─────────────────────────────────────────────────────

template <NodeType node>
result<bool> is_ltl_aba_realizable(tref fm, int_t start_time, bool output) {
	using tau = tree<node>;
	result<bool> r;
	LOG_DEBUG << "[ltl_aba] is_ltl_aba_realizable: " << LOG_FM(fm);

	// This function is called directly by api::realizable as well as by
	// is_tau_formula_sat, so -- like is_tau_formula_sat does for its own
	// callers -- it brands every undecided exit UNKNOWN itself rather than
	// leaving it to whichever caller happens to print the report.
	auto backend_failed = [&]() -> result<bool> {
		return r.with_error(code::solver_error,
			"UNKNOWN: the synthesis backend failed or produced no "
			"verdict; realizability could not be decided");
	};

	// LT-5 / IN-1 backstop: a `wff_semantic_neg`, `A` or `E` that reaches
	// here was not handled by reduce_ctl_star_to_ltl (direct callers such
	// as preferences.h skip the reduction entirely). None has a
	// propositional encoding -- the skeleton would flatten it to "1" --
	// and A/E left in place could bounce back into is_tau_formula_sat's
	// CTL* branch forever. Refuse rather than answer wrongly.
	if (has_ctl_star_operators<node>(fm)) {
		return r.with_error(code::solver_error,
		    "CTL* operators (A / E / semantic negation) reached the LTL "
		    "realizability check without a CTL* reduction; route the "
		    "formula through is_tau_formula_sat");
	}

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
	auto no_bool_combs = [&] {
		auto nbc = has_no_boolean_combs_of_models<node>(fm);
		if (nbc.has_value()) {
			bool v = nbc.value();
			r.merge(std::move(nbc));
			return v;
		}
		// the six-phase pipeline is the fallback: a rejected candidate
		auto sc = r.open("rejected candidate");
		r.info("the formula could not be checked for Boolean "
			"combinations of models");
		report cand = std::move(nbc).report();
		cand.demote_errors_to_warnings();
		r.append(std::move(cand));
		return false;
	};
	if (!realizability_has_game_operators<node>(fm) && no_bool_combs()) {
		LOG_DEBUG << "[ltl_aba] safety fast-path "
		             "(no full-LTL operators, single G)";
		// An error here (e.g. transform_to_execution's multiple-sometimes
		// refusal) is undecided, not a decided "not realizable" -- do not
		// let has_value()==false collapse silently into `false`.
		auto sat_opt = r.merge_take(is_tau_formula_sat<node>(fm, start_time, output));
		if (!sat_opt) return backend_failed();
		return r.with_value(*sat_opt);
	}

	// a nested call (the safety fast path above) keeps the caller's flag
	const bool outer_incomplete = ltl_verdict_incomplete;
	ltl_verdict_incomplete = false;
	struct restore_flag {
		bool outer;
		~restore_flag() { ltl_verdict_incomplete |= outer; }
	} restore{ outer_incomplete };
	auto unrealizable = [&](const char* how) {
		if (ltl_verdict_incomplete) {
			if (output) LOG_INFO << "[ltl_aba] UNKNOWN (" << how
				<< ", but a consistency cap gave up)";
			return r.with_error(code::solver_error, std::string(
				"UNKNOWN: unrealizable only because a consistency cap "
				"(--max-consistency-subsets / --max-cover-products) "
				"skipped checks; realizability could not be decided"));
		}
		if (output) LOG_INFO << "[ltl_aba] UNREALIZABLE (" << how << ")";
		return r.with_value(false);
	};

	ltl_aba_solution<node> partial;
	auto maybe_opt = r.merge_take(solve_ltl_aba<node>(fm, &partial));
	if (!maybe_opt) return backend_failed();
	auto maybe = std::move(*maybe_opt);
	LOG_DEBUG << "[ltl_aba] solve_ltl_aba returned: " << maybe.has_value();

	// The data game decides exactly. Over bit sets it runs first; over
	// formulas, whose quantifier elimination can be slow, it settles an
	// UNREALIZABLE or undecided abstraction, whose attempt is then a
	// rejected candidate.
	auto on_data = [&](const ltl_aba_solution<node>& s, bool formulas,
		result<bool>* attempt) -> std::optional<bool>
	{
		if (s.game_skeleton.empty()) return std::nullopt;
		auto game = solve_data_game<node>(s.game_skeleton, s.atoms,
			s.input_props, s.output_props, formulas);
		if (!game.has_value()) {
			auto sc = r.open("rejected candidate");
			r.info("the data game could not be built",
				{{label::value, truncate_for_message(
					tau::get(fm).to_str())}});
			report rep = std::move(game).report();
			rep.demote_errors_to_warnings();
			r.append(std::move(rep));
			return std::nullopt;
		}
		if (game.value() == data_game_verdict::undecided) {
			r.merge(std::move(game));
			return std::nullopt;
		}
		if (attempt) {
			auto sc = r.open("rejected candidate");
			r.info("the abstraction gave no verdict the data confirms",
				{{label::value, truncate_for_message(
					tau::get(fm).to_str())}});
			report rep = std::move(*attempt).report();
			rep.demote_errors_to_warnings();
			r.append(std::move(rep));
		}
		const bool wins = game.value() == data_game_verdict::realizable;
		r.merge(std::move(game));
		if (output) LOG_INFO << "[ltl_aba] " << (wins
			? "REALIZABLE" : "UNREALIZABLE") << " (data game)";
		return wins;
	};

	// The default abstraction without a winning strategy proves nothing:
	// its consistency constraints quantify the inputs universally and so
	// can forbid the system a combination the environment's data makes
	// true. The observed abstraction, whose constraints forbid only
	// combinations no data satisfies, over-approximates the system when
	// every input atom reads a single step: a winning strategy on the
	// data, played on data chosen to match the input props, gives the
	// props a play that abstraction allows. Its UNREALIZABLE, refined by
	// clauses that also block only what no data realizes, is a proof.
	auto unrealizable_on_sound_abstraction = [&](const char* how,
		const ltl_aba_solution<node>& s) -> result<bool>
	{
		if (!input_atoms_read_one_step<node>(s.atoms)) {
			if (output) LOG_INFO << "[ltl_aba] UNKNOWN (" << how
				<< ", but an input atom reads several steps)";
			return r.with_error(code::solver_error, std::string(
				"UNKNOWN: the abstraction has no winning strategy, but "
				"an input atom reading several steps lets its "
				"environment choose what the data has fixed; "
				"realizability could not be decided"));
		}
		std::optional<std::optional<ltl_aba_solution<node>>> sound;
		{
			const bool outer = ltl_observed_abstraction;
			const bool outer_twins = ltl_input_twins;
			ltl_observed_abstraction = ltl_input_twins = true;
			sound = r.merge_take(solve_ltl_aba<node>(fm));
			ltl_observed_abstraction = outer;
			ltl_input_twins = outer_twins;
		}
		if (!sound) return backend_failed();
		if (!*sound) return unrealizable(how);
		auto refined = r.merge_take(
			refine_or_observe<node>(fm, **sound, output));
		if (!refined) return std::move(r);
		if (*refined) return r.with_value(true);
		return unrealizable(how);
	};

	if (!maybe) {
		if (auto v = on_data(partial, true, nullptr)) return r.with_value(*v);
		// only the default path has a game skeleton; the others decide
		// their own abstraction
		if (partial.game_skeleton.empty())
			return unrealizable("propositional");
		return unrealizable_on_sound_abstraction("propositional", partial);
	}
	if (auto v = on_data(*maybe, false, nullptr)) return r.with_value(*v);

	const ltl_aba_solution<node> abstraction = *maybe;
	auto refined = refine_or_observe<node>(fm, *maybe, output);
	if (refined.has_value() && refined.value()) {
		r.merge(std::move(refined));
		return r.with_value(true);
	}
	if (auto v = on_data(abstraction, true, &refined))
		return r.with_value(*v);
	auto verdict = r.merge_take(std::move(refined));
	if (!verdict) return r;
	if (!*verdict)
		return unrealizable_on_sound_abstraction("ABA-refined", abstraction);
	return r.with_value(true);
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

// Exactly one of the state bits `sv` is set at the current step.
template <NodeType node>
static tref mealy_one_hot(const std::vector<std::string>& sv) {
	using tau = tree<node>;
	const size_t k = sv.size();
	tref at_least = tau::_F();
	for (size_t i = 0; i < k; ++i)
		at_least = tau::build_wff_or(at_least, build_state_bit_eq<node>(sv[i], 0, true));
	tref at_most = tau::_T();
	for (size_t i = 0; i < k; ++i)
		for (size_t j = i + 1; j < k; ++j)
			at_most = tau::build_wff_and(at_most,
			    tau::build_wff_neg(
			        tau::build_wff_and(build_state_bit_eq<node>(sv[i], 0, true),
			                          build_state_bit_eq<node>(sv[j], 0, true))));
	return tau::build_wff_and(at_least, at_most);
}

template <NodeType node>
static result<tref> encode_mealy_as_safety(const ltl_aba_solution<node>& sol)
{
	using tau = tree<node>;
	result<tref> r;
	const auto& aut = sol.aut;
	const size_t k = aut.num_states;

	// Auxiliary output state variable names. Use "ms" (Mealy-state) prefix to
	// avoid collision with compile-away S-operator variables (o__ltl_sN__).
	std::vector<std::string> sv;
	for (size_t i = 0; i < k; ++i)
		sv.push_back("o__ltl_ms" + std::to_string(i) + "__");

	// ── (a) One-hot constraint at the current step ────────────────────────
	tref one_hot = mealy_one_hot<node>(sv);

	// ── (b) Transition rules with lookback-1 ──────────────────────────────
	// For each source state s:
	//   si[t-1]=1  →  ∨_edges_from_s (guard_formula ∧ s_{dst}[t]=1)
	tref trans = tau::_T();
	for (size_t s = 0; s < aut.edges.size(); ++s) {
		tref prev_s = build_state_bit_eq<node>(sv[s], -1, true);
		tref edges_disj = tau::_F();
		for (const auto& e : aut.edges[s]) {
			if (e.dst >= k) {
				return r.with_error(code::internal_error,
					"[ltl_aba] HOA edge dst "
					+ std::to_string(e.dst)
					+ " out of range [0,"
					+ std::to_string(k) + ")",
					{{label::actual, e.dst},
					 {label::limit, k}});
			}
			tref guard_fm = guard_to_aba<node>(
			    e.guard_label, aut.aps, sol.atoms);
			tref next_d = build_state_bit_eq<node>(
				sv[e.dst], 0, true);
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

	TAU_TRY(tref body,
		normalize_non_temp<node>(tau::build_wff_and(one_hot, trans)));
	LOG_DEBUG << "[ltl_aba] multi-state safety body: " << LOG_FM(body);
	return r.with_value(tau::build_wff_always(body));
}

// Fixed-time constraints for the steps 0 .. warmup-1, which run before the
// interpreter enforces the G body of encode_mealy_as_safety: the machine is
// in its initial state before step 0 and takes exactly one transition per
// step, so sv[·][t] is the state after step t, as in the G body. A guard
// literal over an atom that would read a negative time is dropped (the
// strategy's choice cannot depend on it at that step): for the cube and DNF
// labels ltlsynt emits that is the existential projection of the guard.
template <NodeType node>
static result<tref> encode_mealy_warmup(const ltl_aba_solution<node>& sol,
                                const std::vector<std::string>& sv,
                                int_t warmup)
{
	using tau = tree<node>;
	result<tref> r;
	const auto& aut = sol.aut;
	const size_t k = sv.size();
	const size_t init_s = aut.initial_state;
	if (init_s >= k) return r.with_value(nullptr);
	auto at = [](tref fm, int_t t) -> result<tref> {
		auto io = tau::get(fm).select_top(is_child<node, tau::io_var>);
		return fm_at_time_point<node>(fm, io, t);
	};
	tref all = tau::_T();
	for (int_t t = 0; t < std::max<int_t>(warmup, 1); ++t) {
		std::vector<std::pair<tref, std::string>> atoms_t;
		for (const auto& a : sol.atoms)
			if (max_atom_lookback<node>({a}) <= t) atoms_t.push_back(a);
		auto edges_from = [&](size_t s) {
			tref d = tau::_F();
			for (const auto& e : aut.edges[s]) {
				if (e.dst >= k) continue;
				d = tau::build_wff_or(d, tau::build_wff_and(
					guard_to_aba<node>(e.guard_label, aut.aps, atoms_t),
					// e.dst was range-checked against k just
					// above.
					build_state_bit_eq<node>(
						sv[e.dst], 0, true)));
			}
			return d;
		};
		tref step = tau::_T();
		// init_s was range-checked against k before the loop.
		if (t == 0) step = edges_from(init_s);
		else for (size_t s = 0; s < sv.size(); ++s)
			step = tau::build_wff_and(step, tau::build_wff_or(
				tau::build_wff_neg(
					build_state_bit_eq<node>(sv[s], -1, true)),
				edges_from(s)));
		TAU_TRY(tref at_t,
			at(tau::build_wff_and(step, mealy_one_hot<node>(sv)), t));
		all = tau::build_wff_and(all, at_t);
	}
	return r.with_value(all);
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
result<std::tuple<tref, std::optional<ltl_aba_solution<node>>,
                  std::vector<std::string>>>
ltl_to_safety_formula_full(tref fm,
	std::shared_ptr<data_game_strategy<node>>* data_strategy, bool synthesize,
	bool* unrealizable)
{
	using tau = tree<node>;
	using full_t = std::tuple<tref, std::optional<ltl_aba_solution<node>>,
		std::vector<std::string>>;
	result<full_t> r;
	LOG_DEBUG << "[ltl_aba] ltl_to_safety_formula: " << LOG_FM(fm);

	// Fast path: if all LTL operators are past (S/T), compile them away and
	// return G(curr && rhs) safety invariants for each S operator.
	// No Mealy synthesis is needed on this path; the returned solution is empty.
	// Wraps each top-level conjunct in G unless it already is one.
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

	{
		TAU_TRY(auto past_compiled, compile_since_trigger<node>(fm));
		auto& [compiled_fast, safety_fm, init_fm, _aux, unanchored_aux] =
			past_compiled;
		// The compiled invariants read their auxiliaries at t-1, which
		// makes step 0 a warm-up step the interpreter does not enforce.
		// That matches the spec only when it has a lookback of its own;
		// otherwise the Mealy route, whose past-operator testers start at
		// step 0, executes it.
		if (!synthesize
			&& !realizability_has_game_operators<node>(compiled_fast)
			&& body_max_lookback<node>(fm) > 0)
		{
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
			tref obligation = wrap_always(compiled_fast);
			tref out = tau::build_wff_and(obligation,
			           tau::build_wff_and(safety_fm, init_fm));
			// LA-N3: hand the inner-S auxiliaries to the caller so the
			// interpreter can seed their t=0 anchor (S(-1) = false).
			return r.with_value(full_t{out, std::nullopt,
				std::move(unanchored_aux)});
		}
	}

	// A pure-past spec rerouted here (no lookback of its own) is an
	// invariant, as on the fast path: every step, not only step 0.
	TAU_TRY(auto past_compiled, compile_since_trigger<node>(fm));
	if (!realizability_has_game_operators<node>(
		std::get<0>(past_compiled)))
		fm = wrap_always(fm);
	ltl_aba_solution<node> partial;
	auto maybe_r = solve_ltl_aba<node>(fm, &partial);
	// A strategy over bookkeeping bits cannot be played: solve again by
	// the default path, whose abstraction and data game give strategies
	// over the data.
	if (maybe_r.has_value() && maybe_r.value()
		&& !maybe_r.value()->executable)
	{
		ltl_propositional_synthesis = false;
		partial = {};
		auto again = solve_ltl_aba<node>(fm, &partial);
		ltl_propositional_synthesis = true;
		if (again.has_value()) maybe_r = std::move(again);
		else {
			// the re-solve could not decide: a rejected candidate
			auto sc = r.open("rejected candidate");
			r.info("the spec could not be solved again without "
				"bookkeeping bits");
			report cand = std::move(again).report();
			cand.demote_errors_to_warnings();
			r.append(std::move(cand));
		}
	}
	if (!maybe_r.has_value()) {
		r.merge(std::move(maybe_r));
		return r;
	}
	auto maybe_opt = std::move(maybe_r.value());
	r.merge(std::move(maybe_r));
	auto& maybe = maybe_opt;
	// With `data_strategy`, execution plays the strategy of the data game
	// whenever that game decides the formula, in the order the
	// realizability check asks it: on codes before the abstraction, on
	// formulas only once the abstraction gives no strategy to execute.
	const ltl_aba_solution<node> game_source = maybe ? *maybe : partial;
	bool data_decided = false;
	auto on_data = [&](bool formulas) {
		if (!data_strategy || data_decided
			|| game_source.game_skeleton.empty()) return false;
		auto game = solve_data_game<node>(game_source.game_skeleton,
			game_source.atoms, game_source.input_props,
			game_source.output_props, formulas, data_strategy);
		if (game.has_value()) {
			data_decided = game.value()
				!= data_game_verdict::undecided;
			if (unrealizable
				&& game.value()
					== data_game_verdict::unrealizable)
					*unrealizable = true;
			r.merge(std::move(game));
		} else {
			// the data game could not be built: a rejected candidate
			auto sc = r.open("rejected candidate");
			r.info("the data game could not be built");
			report cand = std::move(game).report();
			cand.demote_errors_to_warnings();
			r.append(std::move(cand));
		}
		return *data_strategy != nullptr;
	};
	auto none = [&]() -> full_t {
		on_data(true);
		return {nullptr, std::nullopt, {}};
	};
	if (on_data(false))
		return r.with_value(full_t{nullptr, std::nullopt, {}});
	if (!maybe) {
		LOG_DEBUG << "[ltl_aba] ltl_to_safety_formula: not realizable";
		// only the default path has a game skeleton; the others decide
		// their own abstraction exactly, as the realizability check
		// takes them
		if (unrealizable && game_source.game_skeleton.empty()
			&& !ltl_verdict_incomplete)
				*unrealizable = true;
		return r.with_value(none());
	}

	auto& sol = *maybe;
	// Execute the strategy the realizability check accepts, not ltlsynt's
	// first one, which may take an edge the ABA rules out.
	if (sol.executable) {
		auto refined = refine_or_observe<node>(fm, sol, false);
		if (!refined.has_value() || !refined.value()) {
			LOG_DEBUG << "[ltl_aba] ltl_to_safety_formula: no strategy "
				"survives the ABA refinement";
			auto out = none();
			if (!refined.has_value()
				&& (data_strategy && *data_strategy)) {
				// the data game is the other candidate and it decided
				auto sc = r.open("rejected candidate");
				r.info("no strategy survives the ABA refinement");
				report cand = std::move(refined).report();
				cand.demote_errors_to_warnings();
				r.append(std::move(cand));
			} else r.merge(std::move(refined));
			return r.with_value(std::move(out));
		}
	}

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
		if (none(); data_strategy && *data_strategy)
			return r.with_value(full_t{nullptr, std::nullopt, {}});
		return r.with_error(code::solver_error,
			"[ltl_aba] specification is REALIZABLE but the "
			"synthesised strategy cannot be encoded as a safety "
			"formula (Algorithm B strategy over bookkeeping "
			"bits) — it is not executable");
	}

	// LA-10: constant-output strategy — the executable form is the
	// materialised `always(⋀ o_k = c_k)` witness, not `always T`.
	if (sol.const_formula)
		return r.with_value(full_t{sol.const_formula, std::move(sol), {}});

	// Purely propositional: realizable but no data constraints to encode.
	if (sol.atoms.empty())
		return r.with_value(full_t{tau::_T(), std::move(sol), {}});

	const auto& aut = sol.aut;

	// Trivially realizable: empty automaton.
	if (aut.num_states == 0)
		return r.with_value(full_t{tau::_T(), std::move(sol), {}});

	if (aut.num_states > 1) {
		LOG_INFO << "[ltl_aba] Multi-state strategy ("
		         << aut.num_states
		         << " states) — encoding with auxiliary one-hot state bits";
		TAU_TRY(tref encoded, encode_mealy_as_safety<node>(sol));
		return r.with_value(full_t{encoded, std::move(sol), {}});
	}

	// Single-state strategy: the self-loop guard is the perpetual output
	// constraint. LA-R6: ltlsynt Mealy machines are input-complete, so a
	// state with no outgoing edge only arises from a degraded automaton;
	// executing it as `always T` would drop every obligation (the 1-state
	// analogue of LT-28). Not executable.
	if (aut.edges.empty() || aut.edges[0].empty()) {
		if (none(); data_strategy && *data_strategy)
			return r.with_value(full_t{nullptr, std::nullopt, {}});
		return r.with_error(code::internal_error,
			"[ltl_aba] single-state strategy has no outgoing "
			"edge; the automaton is degraded and cannot be "
			"executed");
	}

	// Build the disjunction of ABA guard formulas over all edges from state 0.
	tref combined = tau::_F();
	for (const auto& e : aut.edges[0]) {
		tref guard_fm = guard_to_aba<node>(e.guard_label, aut.aps, sol.atoms);
		auto norm_guard_r = normalize_non_temp<node>(guard_fm);
		if (!norm_guard_r.has_value()) {
			r.merge(std::move(norm_guard_r));
			return r.with_error(code::internal_error,
				"[ltl_aba] ltl_to_safety_formula_full: "
				"normalization of a guard formula failed");
		}
		tref norm_guard = norm_guard_r.value();
		r.merge(std::move(norm_guard_r));
		combined = tau::build_wff_or(combined, norm_guard);
	}
	auto simplified_r = normalize_non_temp<node>(combined);
	if (!simplified_r.has_value()) {
		r.merge(std::move(simplified_r));
		return r.with_error(code::internal_error,
			"[ltl_aba] ltl_to_safety_formula_full: "
			"final normalization failed");
	}
	tref simplified = simplified_r.value();
	r.merge(std::move(simplified_r));
	LOG_DEBUG << "[ltl_aba] ltl_to_safety_formula result: always("
	          << LOG_FM(simplified) << ")";
	return r.with_value(full_t{tau::build_wff_always(simplified),
		std::move(sol), {}});
}

template <NodeType node>
result<tref> ltl_to_safety_formula(tref fm) {
	result<tref> r;
	TAU_TRY(auto full, ltl_to_safety_formula_full<node>(fm));
	return r.with_value(std::get<0>(full));
}

// ── ltl_explain ───────────────────────────────────────────────────────────────

template <NodeType node>
result<bool> ltl_explain(tref fm, std::ostream& out,
	const std::function<result<bool>()>& decide)
{
	using tau = tree<node>;
	using tt = tau::traverser;
	result<bool> r;
	bool exact_reduction = true;

	// Same input contract as api::realizable: a formula, or a spec whose
	// main part is one. A term used to reach the backends as a formula:
	// `ltl x:bv[1]` aborted on a cvc5 exception and `ltl x:sbf` answered
	// UNREALIZABLE (issue #131).
	if (!fm || !(tau::get(fm).is(tau::wff) || (tau::get(fm).is(tau::spec)
		&& (tt(fm) | tau::main | tau::wff | tt::ref))))
	{
		return r.with_error(code::invalid_argument, "Invalid formula");
	}

	// IN-R3: `ltl` used to hand A/E/- straight to the skeleton, where the
	// tester variant flattened them to "1". Reduce like is_tau_formula_sat
	// does (or refuse, via a result<T> error, where no sound encoding
	// exists) before explaining anything.
	// TODO: unlike is_ltl_aba_realizable's fast path, this does not also require has_no_boolean_combs_of_models
	//
	// A refusal here is undecided, not a decided verdict.  Nothing is
	// printed to `out` for it (mirroring the days this was an exception
	// that unwound out of this function before anything was printed);
	// the detail text is merged into `r`'s report alongside a fresh
	// UNKNOWN-branded summary (same shape as is_tau_formula_sat's CTL*
	// branch, satisfiability.tmpl.h), and the caller (ltl_cmd) prints the
	// whole report exactly once.
	// The verdict below comes from the same procedure `realizable` runs,
	// which decides the normalized formula and reduces after it: do both in
	// that order here, or the two commands answer from different atoms.
	// The always statements form one always part with one warm-up, as in
	// every other decision procedure.
	fm = flatten_always_conjuncts<node>(fm);
	{
		auto nf = normalize<node>(fm);
		if (nf.has_value()) {
			if (nf.value()) fm = nf.value();
			r.merge(std::move(nf));
		} else {
			// the explanation goes on with the unnormalized formula
			auto sc = r.open("rejected candidate");
			r.info("the formula could not be normalized",
				{{label::value, truncate_for_message(
					TAU_TO_STR(fm))}});
			report cand = std::move(nf).report();
			cand.demote_errors_to_warnings();
			r.append(std::move(cand));
		}
	}
	if (has_ctl_star_operators<node>(fm)) {
		auto reduction_r = reduce_ctl_star_to_ltl<node>(fm);
		if (!reduction_r.has_value()) {
			r.merge(std::move(reduction_r));
			return r.with_error(code::solver_error,
				messages::unknown_realizability_timed_out);
		}
		auto& reduction = *reduction_r;
		out << "CTL* reduced to LTL: "
			<< tau::get(reduction.ltl_formula).to_str() << "\n";
		fm = reduction.ltl_formula;
		exact_reduction = reduction.exact;
	}

	if (!realizability_has_game_operators<node>(fm)) {
		out << "Formula has no LTL operators (treated as G(phi))\n";
		// Fall through to the existing safety pipeline. An error here
		// (e.g. transform_to_execution's multiple-sometimes refusal) is
		// undecided, not a decided UNREALIZABLE.
		auto sat_r = decide ? decide()
			: is_tau_formula_sat<node>(fm, 0, false);
		if (!sat_r.has_value()) {
			r.merge(std::move(sat_r));
			return r.with_error(code::solver_error,
				messages::unknown_realizability_no_verdict);
		}
		bool sat = sat_r.value();
		r.merge(std::move(sat_r));
		out << (sat ? "REALIZABLE" : "UNREALIZABLE") << "\n";
		return r.with_value(sat);
	}

	// The trace below is the first ltlsynt round and its per-edge oracle
	// checks, or the round's refusal; the verdict comes from `decide` when
	// given, otherwise from is_ltl_aba_realizable, the procedure
	// `realizable` runs (window oracle, refinement rounds).
	auto verdict = [&]() -> result<bool> {
		auto real = decide ? decide()
			: is_ltl_aba_realizable<node>(fm, 0, false);
		if (!real.has_value()) {
			r.merge(std::move(real));
			return r.with_error(code::solver_error,
				"UNKNOWN: the synthesis backend failed or produced no "
				"verdict; realizability could not be decided");
		}
		bool realizable = real.value();
		r.merge(std::move(real));
		if (!decide && !realizable && !exact_reduction) {
			return r.with_error(code::solver_error,
				"UNKNOWN: the CTL* reduction is unrealizable, but an E "
				"witness over a past operator ranges over every input "
				"branch, which is stricter than E; realizability could "
				"not be decided");
		}
		// what `run` executes
		if (realizable) {
			std::shared_ptr<data_game_strategy<node>> data;
			auto full =
				ltl_to_safety_formula_full<node>(fm, &data);
			if (!full.has_value()) {
				// The verdict is already decided; the safety formula
				// only describes how `run` executes it, so a formula the
				// encoding does not cover cannot turn it into an error.
				auto sc = r.open("execution strategy not built");
				report cand = std::move(full).report();
				cand.demote_errors_to_warnings();
				r.append(std::move(cand));
				out << "\nThe strategy is not executable\n";
			} else {
				tref safety = std::get<0>(full.value());
				r.merge(std::move(full));
				if (data) out << "\nExecution plays the strategy of the "
					"data game\n";
				else if (safety) out << "\nSafety formula: "
					<< tau::get(safety).to_str() << "\n";
				else out << "\nThe strategy is not executable\n";
			}
		}
		out << "\n" << (realizable ? "REALIZABLE" : "UNREALIZABLE")
			<< "\n";
		return r.with_value(realizable);
	};

	// sol stays populated even when solve_ltl_aba returns std::nullopt.
	ltl_aba_solution<node> sol;
	std::optional<ltl_aba_solution<node>> maybe;
	auto maybe_r = solve_ltl_aba<node>(fm, &sol);
	if (!maybe_r.has_value()) {
		// collect_hoist_conjuncts refuses a positional atom it cannot
		// hoist: the trace ends at the refusal, and the verdict is still
		// the one every other path prints
		if (report_has_code(maybe_r.report(), code::unsupported_operation)) {
			for (auto& n : maybe_r.report().nodes())
				if (n.tag == code::unsupported_operation)
					out << "REFUSED: " << maybe_r.report().str(n.key)
					    << "\n";
			return verdict();
		}
		r.merge(std::move(maybe_r));
		return r.with_error(code::solver_error,
			messages::unknown_realizability_timed_out);
	}
	maybe = std::move(maybe_r.value());
	r.merge(std::move(maybe_r));
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
		out << "\nFirst round: ltlsynt found no strategy\n";
		return verdict();
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

	for (size_t s = 0; s < aut.edges.size(); ++s) {
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
		for (size_t s = 0; s < aut.edges.size(); ++s) {
			for (auto& e : aut.edges[s]) {
				// LT-20: use the SAME oracle as the real
				// pipeline (dead-edge pure-input check +
				// per-BA-type partition) -- the plain
				// existential check printed the opposite
				// verdict on dead catch-all edges.
				auto feas_r = guard_is_aba_feasible<node>(
					e.guard_label, aut.aps, sol.atoms);
				bool feasible = feas_r.has_value() && feas_r.value();
				r.append(std::move(feas_r).report());
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
			out << "\nFirst round: ABA-infeasible transition\n";
			return verdict();
		}
	}

	return verdict();
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
//      existential/eventual context (∨, F, sometimes, U, ...), and `-φ`
//      under a temporal operator or a path quantifier -- has no sound
//      encoding here and is REFUSED with a result<T> error. The caller
//      (reduce_ctl_star_to_ltl) first folds Boolean-context `-φ` to
//      constants and, on a refusal, retries on the NNF form, where
//      ¬A χ = E ¬χ and ¬E χ = A ¬χ turn negative quantifiers positive.
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

// One-step unfolding: `shift_one_step(χ)` read at t+1 says what χ says at t.
// Each temporal case is the expansion law of its operator (`G φ = φ ∧ X G φ`,
// `φ U ψ = ψ ∨ (φ ∧ X(φ U ψ))`, ...) with the now-part shifted one step into
// the past and the future part left where it is, so no next-step operator is
// needed. Atoms shift their lookback by one; a fixed-time atom names the same
// step whatever the position, so it stays. Past operators have no lookback
// form here and are refused.
template <NodeType node>
static result<tref> shift_one_step(tref fm) {
	using tau = tree<node>;
	result<tref> r;
	const auto& t = tau::get(fm);
	if (!t.has_child()) return r.with_value(fm);
	const auto& op = t[0];
	const auto nt = op.value.nt;
	auto N = [&](tref x) { return shift_one_step<node>(x); };
	switch (nt) {
	case tau::wff_t: case tau::wff_f: return r.with_value(fm);
	case tau::wff_neg: {
		TAU_TRY(tref a, N(op.child(0)));
		return r.with_value(tau::build_wff_neg(a));
	}
	case tau::wff_and: case tau::wff_or: case tau::wff_imply:
	case tau::wff_rimply: case tau::wff_equiv: case tau::wff_xor: {
		TAU_TRY(tref a, N(op.child(0)));
		TAU_TRY(tref b, N(op.child(1)));
		switch (nt) {
		case tau::wff_and:    return r.with_value(tau::build_wff_and(a, b));
		case tau::wff_or:     return r.with_value(tau::build_wff_or(a, b));
		case tau::wff_imply:  return r.with_value(tau::build_wff_imply(a, b));
		case tau::wff_rimply: return r.with_value(tau::build_wff_rimply(a, b));
		case tau::wff_equiv:  return r.with_value(tau::build_wff_equiv(a, b));
		default:              return r.with_value(tau::build_wff_xor(a, b));
		}
	}
	case tau::wff_conditional: {
		TAU_TRY(tref c, N(op.child(0)));
		TAU_TRY(tref a, N(op.child(1)));
		TAU_TRY(tref b, N(op.child(2)));
		return r.with_value(tau::build_wff_conditional(c, a, b));
	}
	case tau::wff_always: {     // G φ = φ ∧ X G φ
		TAU_TRY(tref a, N(op.child(0)));
		return r.with_value(tau::build_wff_and(a, fm));
	}
	case tau::wff_sometimes: {  // F φ = φ ∨ X F φ
		TAU_TRY(tref a, N(op.child(0)));
		return r.with_value(tau::build_wff_or(a, fm));
	}
	case tau::wff_until: {      // φ U ψ = ψ ∨ (φ ∧ X(φ U ψ))
		TAU_TRY(tref a, N(op.child(0)));
		TAU_TRY(tref b, N(op.child(1)));
		return r.with_value(tau::build_wff_or(b,
			tau::build_wff_and(a, fm)));
	}
	case tau::wff_weak_until: { // φ W ψ = ψ ∨ (φ ∧ X(φ W ψ))
		TAU_TRY(tref a, N(op.child(0)));
		TAU_TRY(tref b, N(op.child(1)));
		return r.with_value(tau::build_wff_or(b,
			tau::build_wff_and(a, fm)));
	}
	case tau::wff_release: {    // φ R ψ = ψ ∧ (φ ∨ X(φ R ψ))
		TAU_TRY(tref a, N(op.child(0)));
		TAU_TRY(tref b, N(op.child(1)));
		return r.with_value(tau::build_wff_and(b,
			tau::build_wff_or(a, fm)));
	}
	case tau::wff_since: case tau::wff_trigger:
		return r.with_error(code::unsupported_operation,
			"a past operator (S / T) under E has no one-step unfolding: "
			"the witness path cannot be pinned by directions");
	case tau::wff_ex: case tau::wff_all:
		return r.with_error(code::unsupported_operation,
			"a data quantifier under E has no one-step unfolding");
	default: {
		auto io = tau::get(fm).select_top(is_child<node, tau::io_var>);
		return r.with_value(shift_io_vars_in_fm<node>(fm, io, 1));
	}
	}
}

// Recursive bottom-up translation of a CTL* state/path formula to LTL.
// Witness constraints are accumulated in `constraints` (each is a G(w → χ) pair).
// New witness output names are accumulated in `witnesses`.
// `positive`: polarity of `fm` in the root formula; `universal`: `fm` is
// reachable from the root only through ∧ / always / A (see the header
// comment above for why both matter).
template <NodeType node>
static result<tref> translate_ctl_star(tref fm,
		std::vector<std::pair<std::string, tref>>& constraints,
		std::vector<std::string>& witnesses,
		std::vector<size_t>& witness_types,
		const std::vector<std::pair<std::string, size_t>>& inputs,
		bool& exact,
		bool positive = true, bool universal = true) {
	using tau = tree<node>;
	result<tref> r;
	const auto& t = tau::get(fm);
	if (!t.has_child()) { return r.with_value(fm); }

	size_t nt = t[0].value.get_nt();

	// Handle E χ: introduce witness output
	if (nt == tau::wff_E) {
		if (!positive) {
			return r.with_error(code::solver_error,
				"E in negative polarity has no sound LTL encoding "
				"here, because the witness constraint G(w -> chi) "
				"only bounds w from above, so a negated witness "
				"would be vacuous");
		}
		tref inner = t[0].child(0);
		// Recursively translate the inner path formula (positive,
		// but no longer a universal context: w marks SOME state).
		TAU_TRY(auto translated_inner, translate_ctl_star<node>(
			inner, constraints, witnesses, witness_types, inputs,
			exact, true, false));
		// Create fresh witness variable
		std::string wname = ctl_star_detail::fresh_witness_name();
		// Build witness as a wff: (o_w_i[t] = 1) serves as the
		// propositional witness for the E-subformula.
		// We use the Boolean carrier's type for the witness output.
		size_t carrier_tid = get_ba_type_id<node>(
			pack_bool_carrier_type<node>());
		witnesses.push_back(wname);
		witness_types.push_back(carrier_tid);
		tref bf_one = build_bf_t_type<node>(carrier_tid);
		tref w_bf = build_out_var_at_t<node>(
			build_var_name<node>(wname), carrier_tid, "t");
		tref witness_wff = tau::build_bf_eq(w_bf, bf_one);

		// With inputs, the witness path has to be pinned, or the
		// constraint would range over every input branch and the
		// witness would certify A χ. One direction output per input
		// stream names the value the witness path takes next, and the
		// constraint moves one step later (shift_one_step), where
		// "the path follows the directions from here on" is a plain
		// always: G(w[t-1] -> (G follow -> N(χ))).
		auto next_r = inputs.empty() ? result<tref>{}
			: shift_one_step<node>(translated_inner);
		if (!inputs.empty() && next_r.has_value()) {
			tref next = next_r.value();
			r.merge(std::move(next_r));
			tref follow = tau::_T();
			for (const auto& [iname, itype] : inputs) {
				std::string dname = wname + "_d_" + iname;
				witnesses.push_back(dname);
				witness_types.push_back(itype);
				follow = tau::build_wff_and(follow,
					tau::build_bf_eq(
						build_in_var_at_t<node>(
							build_var_name<node>(iname),
							itype, "t"),
						build_out_var_at_t_minus<node>(
							dname, 1, itype)));
			}
			tref w_prev = tau::build_bf_eq(
				build_out_var_at_t_minus<node>(wname, 1, carrier_tid),
				bf_one);
			constraints.emplace_back(wname, tau::build_wff_always(
				tau::build_wff_imply(w_prev,
					tau::build_wff_imply(
						tau::build_wff_always(follow),
						next))));
			return r.with_value(witness_wff);
		}
		if (!inputs.empty()) {
			// the witness path has no one-step unfolding: the
			// all-paths encoding below is the fallback
			auto sc = r.open("rejected candidate");
			r.info("the witness path has no one-step unfolding");
			report cand = std::move(next_r).report();
			cand.demote_errors_to_warnings();
			r.append(std::move(cand));
		}
		// No input to steer (the tree is a single path, so A χ and
		// E χ agree), or χ has no one-step unfolding (a past operator
		// inside): the all-paths encoding, exact in the first case and
		// stricter than E in the second.
		if (!inputs.empty()) exact = false;
		constraints.emplace_back(wname, tau::build_wff_always(
			tau::build_wff_imply(witness_wff, translated_inner)));
		return r.with_value(witness_wff);
	}

	// Handle A χ: only where "all paths from here" coincides with the
	// all-paths synthesis semantics of the enclosing formula (LA-N2).
	if (nt == tau::wff_A) {
		if (!positive) {
			return r.with_error(code::solver_error,
				"A in negative polarity has no sound LTL encoding here");
		}
		if (!universal) {
			return r.with_error(code::solver_error,
				"A under an existential or eventual context (||, F, "
				"sometimes, U, R, W, S, T, E, conditional) is not "
				"soundly encodable without CTL* direction outputs; "
				"refusing rather than answering vacuously");
		}
		return translate_ctl_star<node>(t[0].child(0), constraints,
			witnesses, witness_types, inputs, exact, true, true);
	}

	// A `-φ` still here sits under a temporal operator or a path
	// quantifier and φ reads the past (resolve_semantic_negations folds
	// every other one): "φ is unrealizable from this history on" then
	// depends on the history, which no encoding here tracks.
	if (nt == tau::wff_semantic_neg) {
		return r.with_error(code::solver_error,
		    "semantic negation (-) of a formula reading the past (lookback, "
		    "S / T, a fixed-time atom) under a temporal operator or a path "
		    "quantifier is not implemented: its game depends on the "
		    "history");
	}

	// For all other nodes, recursively translate children
	// Reconstruct the node with translated children
	auto& op = t[0];
	size_t nch = op.children_size();
	if (nch == 0) { return r.with_value(fm); }

	// Check if any child has CTL* operators
	bool has_ctl = false;
	for (size_t i = 0; i < nch; ++i) {
		if (has_ctl_star_operators<node>(op.child(i))) {
			has_ctl = true;
			break;
		}
	}
	if (!has_ctl) { return r.with_value(fm); }

	// Polarity / context of each child. Both-polarity connectives (↔, ⊕,
	// a conditional's guard) cannot host A/E soundly at all.
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
		for (size_t i = 0; i < nch; ++i) {
			if (has_ctl_star_operators<node>(op.child(i))) {
				return r.with_error(code::solver_error, messages::a_e_both_polarity_unsound);
			}
		}
		break;
	case tau::wff_conditional:
		if (has_ctl_star_operators<node>(op.child(0))) {
			return r.with_error(code::solver_error, messages::a_e_both_polarity_unsound);
		}
		break;
	default: // or, sometimes, F, U, R, W, S, T: positive, not universal
		break;
	}

	// Translate children and rebuild
	std::vector<tref> new_children;
	new_children.reserve(nch);
	for (size_t i = 0; i < nch; ++i) {
		TAU_TRY(auto child, translate_ctl_star<node>(op.child(i),
			constraints, witnesses, witness_types, inputs, exact,
			ctx[i].first, ctx[i].second));
		new_children.push_back(child);
	}

	// Rebuild node with same operator but new children
	if (nch == 1) {
		// Unary operators: neg, sometimes, always
		switch (nt) {
		case tau::wff_neg:       return r.with_value(tau::build_wff_neg(new_children[0]));
		case tau::wff_sometimes: return r.with_value(tau::build_wff_sometimes(new_children[0]));
		case tau::wff_always:    return r.with_value(tau::build_wff_always(new_children[0]));
		default:                 break; // falls to the error below
		}
	} else if (nch == 2) {
		// Binary operators
		switch (nt) {
		case tau::wff_and:   return r.with_value(tau::build_wff_and(new_children[0], new_children[1]));
		case tau::wff_or:    return r.with_value(tau::build_wff_or(new_children[0], new_children[1]));
		case tau::wff_imply: return r.with_value(tau::build_wff_imply(new_children[0], new_children[1]));
		case tau::wff_equiv: return r.with_value(tau::build_wff_equiv(new_children[0], new_children[1]));
		case tau::wff_xor:   return r.with_value(tau::build_wff_xor(new_children[0], new_children[1]));
		case tau::wff_until:      return r.with_value(tau::build_wff_until(new_children[0], new_children[1]));
		case tau::wff_release:    return r.with_value(tau::build_wff_release(new_children[0], new_children[1]));
		case tau::wff_weak_until: return r.with_value(tau::build_wff_weak_until(new_children[0], new_children[1]));
		case tau::wff_since:      return r.with_value(tau::build_wff_since(new_children[0], new_children[1]));
		case tau::wff_trigger:    return r.with_value(tau::build_wff_trigger(new_children[0], new_children[1]));
		// LT-13: rimply was missing -- `phi <- E psi` kept its E
		// untranslated and later collapsed to "1" in the skeleton
		case tau::wff_rimply: return r.with_value(tau::build_wff_rimply(
					new_children[0], new_children[1]));
		default:             break;
		}
	} else if (nch == 3 && nt == tau::wff_conditional) {
		return r.with_value(tau::build_wff_conditional(
			new_children[0], new_children[1], new_children[2]));
	}
	// LT-13 / IN-1: a silent identity here left embedded A/E/- untranslated
	// in any connective missing from the switches above; the survivor then
	// reached the skeleton (constant "1") or bounced between
	// is_tau_formula_sat and is_ltl_aba_realizable. Refuse instead.
	return r.with_error(code::solver_error,
		"translate_ctl_star found an unhandled connective with CTL* "
		"content in its subtree; the formula cannot be reduced to LTL",
		{{label::name, node::name(nt)}});
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

// Folds every `-ψ` to a constant. `-ψ` is a closed statement about ψ's own
// game ("ψ has no winning system strategy"), decided by determinacy from ψ's
// realizability verdict. Under a temporal operator or a path quantifier it
// reads as "ψ, started fresh here, is unrealizable" -- the same game at every
// point, as a specification that starts later (a revision) reads its own
// lookback: what precedes its start is warm-up.
template <NodeType node>
static result<tref> resolve_semantic_negations(tref fm) {
	using tau = tree<node>;
	result<tref> r;
	if (!has_semantic_negation<node>(fm)) return r.with_value(fm);
	const auto& t = tau::get(fm);
	if (!t.has_child()) return r.with_value(fm);
	const auto& op = t[0];
	auto nt = op.value.nt;
	if (nt == tau::wff_semantic_neg) {
		TAU_TRY(bool real, is_ctl_star_realizable<node>(
			op.child(0), 0, false));
		return r.with_value(real ? tau::_F() : tau::_T());
	}
	// data quantifiers bind variables a folded body would lose
	if (nt == tau::wff_ex || nt == tau::wff_all) return r.with_value(fm);
	trefs ch;
	bool changed = false;
	for (size_t i = 0; i < op.children_size(); ++i) {
		tref c = op.child(i);
		if (!tau::get(c).is(tau::wff)) return r.with_value(fm);
		TAU_TRY(tref f, resolve_semantic_negations<node>(c));
		changed |= f != c;
		ch.push_back(f);
	}
	if (!changed) return r.with_value(fm);
	return r.with_value(tau::get(t.value, tau::get(op.value, ch)));
}

template <NodeType node>
result<bool> is_ctl_star_realizable(tref fm, int_t start_time, bool output) {
	result<bool> r;
	if (!has_ctl_star_operators<node>(fm))
		return is_ltl_aba_realizable<node>(fm, start_time, output);
	// The reduction's witness constraints are decided over the atoms the
	// formula has: a contradiction the atoms still spell out separately
	// (`o1 = i1`, `o1 = 0`, `i1 = 1`) is one the oracle need not catch, so
	// normalize first, as api::realizable does before calling here.
	{
		auto nf = normalize<node>(fm);
		if (nf.has_value()) {
			if (nf.value()) fm = nf.value();
			r.merge(std::move(nf));
		} else {
			// the reduction runs on the unnormalized formula
			auto sc = r.open("rejected candidate");
			r.info("the formula could not be normalized",
				{{label::value, truncate_for_message(
					TAU_TO_STR(fm))}});
			report cand = std::move(nf).report();
			cand.demote_errors_to_warnings();
			r.append(std::move(cand));
		}
	}
	TAU_TRY(auto reduction, reduce_ctl_star_to_ltl<node>(fm));
	TAU_TRY(bool real, is_ltl_aba_realizable<node>(reduction.ltl_formula,
		start_time, output));
	// An E witness encoded without directions forces χ on every path from
	// its state, which is stricter than E; an unrealizable reduction then
	// says nothing about fm.
	if (!real && !reduction.exact) {
		return r.with_error(code::solver_error,
			"UNKNOWN: the CTL* reduction is unrealizable, but an E "
			"witness over a past operator ranges over every input "
			"branch, which is stricter than E; realizability could "
			"not be decided");
	}
	return r.with_value(real);
}

template <NodeType node>
result<ctl_star_reduction<node>> reduce_ctl_star_to_ltl(tref fm) {
	using tau = tree<node>;
	result<ctl_star_reduction<node>> r;

	// Resolved before the counter reset: deciding a `-ψ` runs a reduction
	// of its own.
	TAU_TRY(fm, resolve_semantic_negations<node>(fm));
	ctl_star_detail::reset_witness_counter();

	// One direction output per input stream is what pins an E witness to
	// one branch; the streams are collected once, by name, so every E
	// steers the same tree.
	std::vector<std::pair<std::string, size_t>> inputs;
	for (tref v : tau::get(fm).select_all(is_child<node, tau::io_var>)) {
		tref iov = tau::trim(v);
		if (!tau::get(iov).is_input_variable()) continue;
		std::string name = get_var_name<node>(iov);
		if (std::ranges::none_of(inputs, [&](const auto& p) {
			return p.first == name; }))
			inputs.emplace_back(name, tau::get(v).get_ba_type());
	}

	std::vector<std::pair<std::string, tref>> constraints;
	std::vector<std::string> witnesses;
	std::vector<size_t> witness_types;
	bool exact = true;

	auto translated_r = translate_ctl_star<node>(fm, constraints, witnesses,
		witness_types, inputs, exact);
	if (!translated_r.has_value()) {
		// A or E in negative polarity has a positive dual (¬A χ = E ¬χ,
		// ¬E χ = A ¬χ); the NNF form is equivalent, so a successful
		// translation of it is as sound as one of fm.
		tref nnf = to_nnf<node>(fm);
		if (nnf && nnf != fm) {
			constraints.clear();
			witnesses.clear();
			witness_types.clear();
			exact = true;
			ctl_star_detail::reset_witness_counter();
			auto nnf_r = translate_ctl_star<node>(nnf, constraints,
				witnesses, witness_types, inputs, exact);
			if (nnf_r.has_value()) translated_r = std::move(nnf_r);
			else {
				// the NNF form failed too: a rejected candidate
				auto sc = r.open("rejected candidate");
				r.info("the NNF form has no sound reduction "
					"either");
				report cand = std::move(nnf_r).report();
				cand.demote_errors_to_warnings();
				r.append(std::move(cand));
			}
		}
	}
	if (!translated_r.has_value()) {
		r.merge(std::move(translated_r));
		return r.with_error(code::solver_error,
			"the CTL* formula has no sound reduction to LTL");
	}
	tref translated = translated_r.value();
	r.merge(std::move(translated_r));

	// Build the conjunction: translated_root ∧ constraint_1 ∧ ... ∧ constraint_n
	tref result = translated;
	for (auto& [name, constraint] : constraints) {
		result = tau::build_wff_and(result, constraint);
	}

	return r.with_value(ctl_star_reduction<node>{result, witnesses,
		std::move(witness_types), exact});
}

} // namespace idni::tau_lang
