// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// ltl_aba_builders.tmpl.h - solve, refine, realize, strategy to safety
// formula, explain, CTL* reduction

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ltl_aba"

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

// BA's own try_propositional_synthesis when it declares the capability, else
// a declined synthesis.
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
// Returns a declined synthesis when no BA claims the formula, and an error
// when the claimant is undecided.
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
inline std::string nest_x(std::string inner, int_t n) {
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
		LOG_DEBUG << "Step guard " << g << " driven true from step " << k;
	}
}

// The text of a data atom as a user writes it: the normal form `f' = 0`
// prints as `f = 1` and `f' != 0` as `f != 1`.
template <NodeType node>
static std::string atom_text(tref a) {
	using tau = tree<node>;
	const tau& t = tau::get(a);
	const bool eq = t.child_is(tau::bf_eq), neq = t.child_is(tau::bf_neq);
	if ((eq || neq) && t[0][0].child_is(tau::bf_neg) && t[0][1].equals_0())
	{
		// with hooks on, the built atom folds back to the normal form
		use_hooks_guard<node> hooks_off(false);
		tref f = tau::trim2(t[0].first());
		tref one = tau::_1(find_ba_type<node>(t[0].first()));
		return tau::get(eq ? tau::build_bf_eq(f, one)
			: tau::build_bf_neq(f, one)).to_str();
	}
	return t.to_str();
}

// The text of each data atom by its prop, when ltl-witness prints them.
template <NodeType node>
static ltlsynt_legend ltlsynt_legend_of(
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	ltlsynt_legend legend;
	if (!ltl_witness_param && !ltl_witness_into_report) return legend;
	for (const auto& [a, p] : atoms)
		legend.emplace_back(p, atom_text<node>(a));
	return legend;
}

/** @internal @copydoc solve_ltl_aba @endinternal */
// Under a decision over finite traces, reads @p skeleton, the formula part
// of a skeleton, over the finite traces of LTLf: ltlfilt relativises it to
// ltl_alive_prop, an output, so the system chooses where the trace ends.
// The constraints appended after it (consistency, shift chains, testers,
// step guards) hold on every step and are not relativised.
template <NodeType node>
static result<void> read_over_finite_traces(std::string& skeleton,
	ltl_aba_solution<node>& sol)
{
	result<void> r;
	if (!ltl_finite_decision) return r;
	TAU_TRY(skeleton, from_ltlf(skeleton, ltl_alive_prop,
		ltl_timeout_sec()));
	sol.output_props.emplace_back(ltl_alive_prop);
	return r;
}

template <NodeType node>
static result<std::optional<ltl_aba_solution<node>>>
solve_ltl_aba(tref fm, ltl_aba_solution<node>* partial_out)
{
	using tau = tree<node>;
	result<std::optional<ltl_aba_solution<node>>> r;
	LOG_TRACE << "solve_ltl_aba/fm: " << LOG_FM(fm);

	// Past operators (S, T) are handled at skeleton level via DFA temporal
	// testers (ppLTLTT approach), not by AST-level compile-away.  The
	// formula is passed to extract_data_atoms unchanged — S/T nodes are
	// transparent to atom extraction (they are temporal operators, not
	// data atoms).  The skeleton generation intercepts S/T and emits
	// DFA state variables + constraints instead.
	ltl_aba_solution<node> sol;
	TAU_TRY(fm, merge_complementary_atoms<node>(fm));
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
	// in the default path. ltl_skeleton(), which the propositional fast
	// paths use, refuses S/T -- those paths don't have the DFA
	// state-variable machinery. Skip them when S/T are present.
	const bool has_past = has_past_operators<node>(fm);

	// Ask whichever BA owns these atoms to synthesise propositionally. Past
	// operators need the ppLTLTT temporal-tester encoding of the default path,
	// which those fast paths do not have, so they are not offered the formula.
	// Same for a formula needing a __step_ge guard: ltl_skeleton(), which the
	// fast paths use, never drives one.
	if (ltl_propositional_synthesis && !has_past && !ltl_finite_decision
		&& collect_all_step_guards<node>(fm).empty())
	{
		TAU_TRY(auto claim, pack_try_propositional_synthesis<node>(
			fm, sol.atoms));
		if (claim) {
			LOG_DEBUG << "Formula synthesised propositionally by its BA, "
				"strategy found: " << LOG_VAL(claim->has_value());
			if (!*claim && partial_out) *partial_out = sol;
			return r.with_value(std::move(*claim));
		}
	}


	if (sol.atoms.empty()) {
		// Purely propositional: no io_var atoms.
		LOG_DEBUG << "No data atom: ltlsynt decides the propositional "
			"skeleton alone";
		if (has_past) {
			std::vector<past_temporal_tester> testers;
			TAU_TRY(sol.skeleton, skeleton_str_with_testers<node>(fm, sol.atoms, testers));
			TAU_TRY_VOID(read_over_finite_traces<node>(sol.skeleton, sol));
			append_tester_constraints(sol.skeleton, testers);
			for (const auto& t : testers)
				sol.output_props.push_back(t.state_var);
			append_step_guard_drivers<node>(sol, skeleton_step_guards<node>(fm));
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
		TAU_TRY_VOID(read_over_finite_traces<node>(sol.skeleton, sol));
		TAU_TRY(auto ltlsynt_out, call_ltlsynt(sol.skeleton, {},
			sol.output_props));
		auto& [real, hoa] = ltlsynt_out;
		if (!real) {
			if (partial_out) *partial_out = sol;
			return r.with_value(std::nullopt);
		}
		TAU_TRY(sol.aut, parse_hoa(hoa));
		return r.with_value(std::move(sol));
	}

	LOG_DEBUG << "Data atoms abstracted as propositions: "
		<< LOG_VAL(sol.atoms);

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
	TAU_TRY_VOID(read_over_finite_traces<node>(skel, sol));
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
	for (const auto& t : testers)
		sol.output_props.push_back(t.state_var);
	append_step_guard_drivers<node>(sol, skeleton_step_guards<node>(fm_for_skeleton));
	if (!ltl_observed_abstraction)
		sol.game_skeleton = game_skeleton
			+ sol.skeleton.substr(constraints_end);

	LOG_DEBUG << "LTL skeleton (with consistency, shift-chain and "
		<< testers.size() << " past-operator tester constraints): "
		<< sol.skeleton;
	LOG_DEBUG << "Skeleton inputs: " << LOG_VAL(sol.input_props)
		<< ", outputs: " << LOG_VAL(sol.output_props);

	TAU_TRY(auto ltlsynt_out, call_ltlsynt(sol.skeleton, sol.input_props,
		sol.output_props, ltlsynt_legend_of<node>(sol.atoms)));
	auto& [realizable, hoa_text] = ltlsynt_out;
	if (!realizable) {
		if (partial_out) *partial_out = sol;
		return r.with_value(std::nullopt);
	}

	TAU_TRY(sol.aut, parse_hoa(hoa_text));
	TAU_TRY_VOID(gate_counter_props<node>(sol));
	LOG_DEBUG << "Abstract strategy with " << sol.aut.num_states
		<< " states, to check against the data";
	return r.with_value(std::move(sol));
}

// The oracle part of the realizability check, shared with execution: checks
// every edge of sol's strategy against the ABA (and the window oracle for
// relations spanning several steps), blocks an infeasible edge and
// re-synthesizes, until a strategy passes (true, sol holds it) or ltlsynt
// finds none (false). An error is undecided. `lost`, when non-null, is set
// when the strategy loses against the data and no path can be blocked
// (see refine_or_observe).
template <NodeType node>
static result<bool> refine_ltl_aba_solution(ltl_aba_solution<node>& sol,
	bool output, bool* lost = nullptr)
{
	result<bool> r;
	auto backend_failed = [&]() -> result<bool> {
		return r.with_error(code::solver_error,
			messages::unknown_realizability_no_verdict);
	};

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

	LOG_DEBUG << "Checking each edge of the " << sol.aut.num_states
		<< "-state strategy against the ABA over " << sol.atoms.size()
		<< " data atoms";

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
			LOG_DEBUG << "Refinement round " << round << ": edge "
				<< rejected->first << " -> " << e.dst << " has a guard the "
				"ABA refuses: " << e.guard_label;
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
				LOG_DEBUG << "Refinement round " << round << ": every edge "
					"is feasible, checking windows of " << W << " steps";
				TAU_TRY(auto wres, window_infeasible_paths<node>(sol, W,
					ltl_window_max_paths()));
				if (wres.path_cap_reached)
					return undecided("window oracle path cap");
				clauses = std::move(wres.blocking_clauses);
			}
			if (clauses.empty()) {
				size_t rounds = 0;
				TAU_TRY(auto dverdict, strategy_wins_on_data<node>(sol,
					ltl_data_fixpoint_rounds(), rounds));
				LOG_DEBUG << "Strategy played against the data: "
					<< (dverdict == strategy_data_verdict::wins ? "wins"
					: dverdict == strategy_data_verdict::loses ? "loses"
					: "undecided");
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

		// A route that keeps no skeleton (Algorithm A) cannot synthesize
		// again with the edge blocked: its formula and outputs are gone.
		if (sol.skeleton.empty())
			return undecided("the strategy takes a step the data "
				"refuses, and its route keeps no skeleton to "
				"synthesize again from");
		if (max_refinement_rounds && round >= max_refinement_rounds)
			return undecided("ABA refinement bound reached "
				"(--ltl-refinement-rounds / `set ltl-refinement-rounds`, "
				"0 = unlimited)");
		bool added_new = false;
		for (auto& clause : clauses) {
			if (sol.skeleton.find(clause) != std::string::npos) continue;
			sol.skeleton += " && " + clause;
			sol.consistency_constraints.push_back(clause);
			added_new = true;
		}
		if (!added_new) return undecided("ABA refinement bound reached");
		LOG_DEBUG << "Re-synthesising with the blocking clauses: "
			<< LOG_VAL(clauses);

		auto ltlsynt_opt = r.merge_take(call_ltlsynt(sol.skeleton,
			sol.input_props, sol.output_props,
			ltlsynt_legend_of<node>(sol.atoms)));
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

/** @internal @copydoc is_ltl_aba_realizable @endinternal */
template <NodeType node>
result<bool> is_ltl_aba_realizable(tref fm, int_t start_time, bool output) {
	using tau = tree<node>;
	result<bool> r;
	LOG_TRACE << "is_ltl_aba_realizable/fm: " << LOG_FM(fm);
	ltl_semantics_scope semantics;

	// This function is called directly by api::realizable as well as by
	// is_tau_formula_sat, so -- like is_tau_formula_sat does for its own
	// callers -- it brands every undecided exit UNKNOWN itself rather than
	// leaving it to whichever caller happens to print the report.
	auto backend_failed = [&]() -> result<bool> {
		return r.with_error(code::solver_error,
			messages::unknown_realizability_no_verdict);
	};

	// Backstop: a `wff_semantic_neg`, `A` or `E` that reaches
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

	// A formula whose only temporal operators are past ones holds at every
	// step, as `run` executes it (ltl_to_safety_formula_full).
	if (has_past_operators<node>(fm)
		&& !tau::get(fm).find_top(is_future_temporal_quantifier<node>))
		fm = always_of_bare_conjuncts<node>(fm);

	// ltl-compositional: the parts sharing no output are realizable each
	// for itself exactly when the conjunction is, since each strategy
	// reads the same inputs and writes its own outputs.
	if (ltl_compositional_param && !ltl_compositional_part
		&& realizability_has_game_operators<node>(fm))
		if (trefs parts = ltl_output_groups<node>(fm); !parts.empty())
	{
		struct part_scope {
			~part_scope() { ltl_compositional_part = false; }
		} restore;
		ltl_compositional_part = true;
		LOG_DEBUG << "Conjunction split into " << parts.size()
			<< " parts sharing no output, decided one by one: "
			<< LOG_VAL(parts);
		auto sc = r.open("compositional");
		r.report().count("ltl_compositional_parts", parts.size());
		std::vector<report> undecided;
		for (tref part : parts) {
			auto v = is_ltl_aba_realizable<node>(part, start_time, output);
			if (!v.has_value()) {
				undecided.push_back(std::move(v).report());
				continue;
			}
			const bool realizable = v.value();
			r.merge(std::move(v));
			if (realizable) continue;
			// one unrealizable part decides the conjunction
			for (report& u : undecided) {
				u.demote_errors_to_warnings();
				r.append(std::move(u));
			}
			return r.with_value(false);
		}
		if (undecided.empty()) return r.with_value(true);
		for (report& u : undecided) r.append(std::move(u));
		return r;
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
			"combinations of models",
			{{label::value, truncate_for_message(
				TAU_TO_STR(fm))}});
		report cand = std::move(nbc).report();
		cand.demote_errors_to_warnings();
		r.append(std::move(cand));
		return false;
	};
	// over finite traces a safety formula is a game too: the system may
	// end the trace at any step
	if (!ltl_finite_decision && !realizability_has_game_operators<node>(fm)
		&& no_bool_combs()) {
		LOG_DEBUG << "Safety formula (no full-LTL operator, no Boolean "
			"combination of models): decided by the Tau fixpoints";
		// An error here (e.g. transform_to_execution's multiple-sometimes
		// refusal) is undecided, not a decided "not realizable" -- do not
		// let has_value()==false collapse silently into `false`.
		auto sat_opt = r.merge_take(is_tau_formula_sat<node>(fm, start_time, output));
		if (!sat_opt) return backend_failed();
		return r.with_value(*sat_opt);
	}

	ltl_verdict_scope verdict_scope;
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
	LOG_DEBUG << "Abstraction solved, winning strategy found: "
		<< LOG_VAL(maybe.has_value());

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
		// the refinement solves again, and its UNREALIZABLE is a proof
		// only on the same abstraction: both flags hold throughout
		struct sound_flags {
			bool observed = ltl_observed_abstraction;
			bool twins = ltl_input_twins;
			sound_flags() { ltl_observed_abstraction = ltl_input_twins = true; }
			~sound_flags() {
				ltl_observed_abstraction = observed;
				ltl_input_twins = twins;
			}
		} flags;
		auto sound = r.merge_take(solve_ltl_aba<node>(fm));
		if (!sound) return backend_failed();
		if (!*sound) return unrealizable(how);
		(*sound)->observed = true;
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
// Approach: introduce k auxiliary outputs of the Boolean carrier type,
// o__ltl_ms0__, o__ltl_ms1__, ..., o__ltl_ms{k-1}__, representing the
// automaton state in a one-hot encoding.  The always-formula encodes:
//   (a) one-hot constraint: exactly one state bit is true at every step,
//   (b) transition relation: if si[t-1]=1 and the edge guard holds, then
//       s_{dst}[t]=1 and the data atoms satisfy the guard's output conditions.
//
// encode_mealy_warmup pins the steps before the G body applies to the
// initial state.

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

// The data atoms over an output that every edge of `sol` implies, with the
// polarity each edge gives them; none when a label passes the cube cap.
template <NodeType node>
static result<std::vector<std::pair<tref, bool>>>
	mealy_common_output_literals(const ltl_aba_solution<node>& sol)
{
	using tau = tree<node>;
	result<std::vector<std::pair<tref, bool>>> r;
	const auto& aut = sol.aut;
	std::optional<std::map<size_t, bool>> common;
	for (const auto& edges : aut.edges) for (const auto& e : edges) {
		auto cubes = hoa_label::to_dnf(e.guard_label);
		if (!cubes) return r.with_value({});
		// the literals of this label: those in every cube of it
		std::optional<std::map<size_t, bool>> label;
		for (const auto& c : *cubes) {
			std::map<size_t, bool> lits;
			for (const auto& l : c) lits.emplace(l.ap, l.pos);
			if (!label) { label = std::move(lits); continue; }
			std::erase_if(*label, [&](const auto& kv) {
				auto it = lits.find(kv.first);
				return it == lits.end() || it->second != kv.second; });
		}
		if (!label) continue;   // an unsatisfiable label constrains nothing
		if (!common) { common = std::move(label); continue; }
		std::erase_if(*common, [&](const auto& kv) {
			auto it = label->find(kv.first);
			return it == label->end() || it->second != kv.second; });
	}
	std::vector<std::pair<tref, bool>> out;
	if (!common) return r.with_value(std::move(out));
	const std::vector<tref> atom = ap_atoms(aut.aps, sol.atoms);
	for (const auto& [ap, pos] : *common) {
		if (ap >= atom.size() || !atom[ap]) continue;
		const bool reads_output = std::ranges::any_of(
			tau::get(atom[ap]).select_top(is_child<node, tau::io_var>),
			[](tref v) { return !tau::get(v).is_input_variable(); });
		if (reads_output) out.emplace_back(atom[ap], pos);
	}
	return r.with_value(std::move(out));
}

/** @internal @copydoc encode_mealy_as_safety @endinternal */
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

	// An output atom every edge asserts with one polarity is a conjunct of
	// its own: G(c ∧ E') ≡ E, since one rule fires at every step. Left in
	// the edges, its streams would join the machine's spec part, and a
	// revision dropping the machine would drop them too (the definition of
	// the update stream u, say).
	TAU_TRY(auto common, mealy_common_output_literals<node>(sol));
	subtree_map<node, tref> hoisted;
	tref kept = tau::_T();
	for (const auto& [atom, pos] : common) {
		hoisted.emplace(atom, pos ? tau::_T() : tau::_F());
		kept = tau::build_wff_and(kept,
			pos ? atom : tau::build_wff_neg(atom));
	}
	if (!common.empty())
		LOG_DEBUG << "Output atoms every edge asserts, kept out of the "
			"transitions: " << LOG_FM(kept);

	// ── (b) Transition rules with lookback-1 ──────────────────────────────
	// For each source state s:
	//   si[t-1]=1  →  ∨_edges_from_s (guard_formula ∧ s_{dst}[t]=1)
	tref trans = tau::_T();
	for (size_t s = 0; s < aut.edges.size(); ++s) {
		tref prev_s = build_state_bit_eq<node>(sv[s], -1, true);
		tref edges_disj = tau::_F();
		for (const auto& e : aut.edges[s]) {
			if (e.dst >= k)
				return r.with_error(code::out_of_range, std::string(
					"the strategy automaton has an edge to state ")
					+ std::to_string(e.dst) + ", outside [0, "
					+ std::to_string(k) + ")",
					{{label::actual, e.dst}, {label::limit, k}});
			TAU_TRY(tref guard_fm, guard_to_aba<node>(
			    e.guard_label, aut.aps, sol.atoms));
			if (!hoisted.empty())
				guard_fm = rewriter::replace<node>(guard_fm, hoisted);
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
			// A state without outgoing edges (possible only
			// from a truncated HOA -- ltlsynt Mealy machines are
			// input-complete) must be forbidden as a predecessor,
			// not left unconstrained.
			trans = tau::build_wff_and(trans,
			    tau::build_wff_neg(prev_s));
		}
	}

	TAU_TRY(tref body, normalize_non_temp<node>(tau::build_wff_and(kept,
		tau::build_wff_and(one_hot, trans))));
	LOG_DEBUG << "Strategy of " << k << " states encoded with one-hot "
		"state bits, always body: " << LOG_FM(body);
	return r.with_value(tau::build_wff_always(body));
}

// Fixed-time constraints for the steps 0 .. warmup-1, which run before the
// interpreter enforces the G body of encode_mealy_as_safety: the machine is
// in its initial state before step 0 and takes exactly one transition per
// step, so sv[·][t] is the state after step t, as in the G body. A guard
// literal over an atom that would read a negative time is dropped (the
// strategy's choice cannot depend on it at that step): for the cube and DNF
// labels ltlsynt emits that is the existential projection of the guard.
/** @internal @copydoc encode_mealy_warmup @endinternal */
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
		auto edges_from = [&](size_t s) -> result<tref> {
			result<tref> r;
			tref d = tau::_F();
			for (const auto& e : aut.edges[s]) {
				if (e.dst >= k) continue;
				TAU_TRY(tref g, guard_to_aba<node>(
					e.guard_label, aut.aps, atoms_t));
				// e.dst was range-checked against k just above.
				d = tau::build_wff_or(d, tau::build_wff_and(g,
					build_state_bit_eq<node>(
						sv[e.dst], 0, true)));
			}
			return r.with_value(d);
		};
		tref step = tau::_T();
		// init_s was range-checked against k before the loop.
		if (t == 0) {
			TAU_TRY(step, edges_from(init_s));
		} else for (size_t s = 0; s < sv.size(); ++s) {
			TAU_TRY(tref from_s, edges_from(s));
			step = tau::build_wff_and(step, tau::build_wff_or(
				tau::build_wff_neg(
					build_state_bit_eq<node>(sv[s], -1, true)),
				from_s));
		}
		TAU_TRY(tref at_t,
			at(tau::build_wff_and(step, mealy_one_hot<node>(sv)), t));
		all = tau::build_wff_and(all, at_t);
	}
	return r.with_value(all);
}

// ── ltl_to_safety_formula_full ────────────────────────────────────────────────

/** @internal @copydoc ltl_to_safety_formula_full @endinternal */
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
	auto nothing = [&r]() { return r.with_value(full_t{}); };
	// A losing candidate whose report folds into the answer another route
	// gives.
	auto rejected = [&](std::string_view why, report rep) {
		auto sc = r.open("rejected candidate");
		r.info(why, {{label::value, truncate_for_message(
			tau::get(fm).to_str())}});
		rep.demote_errors_to_warnings();
		r.append(std::move(rep));
	};
	LOG_TRACE << "ltl_to_safety_formula_full/fm: " << LOG_FM(fm);
	if (ltl_finite_routing())
		return r.with_error(code::invalid_argument, "ltl-semantics finite "
			"decides a specification over finite traces only; a run or "
			"a program follows it over infinite traces, so it needs "
			"ltl-semantics infinite");
	ltl_verdict_scope verdict_scope;

	// Fast path: if all LTL operators are past (S/T), compile them away and
	// return the G(curr <-> rhs) tracking invariant of each S operator.
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

	TAU_TRY(auto since_trigger, compile_since_trigger<node>(fm));
	{
		auto& [compiled_fast, safety_fm, unanchored_aux] =
			since_trigger;
		// The compiled invariants read their auxiliaries at t-1, and the
		// interpreter seeds each one to 0 at the step before the first
		// enforced one. The testers of the decision start at step 0 and
		// read the warm-up steps a lookback leaves, so a spec with past
		// operators runs on the Mealy route, which plays the strategy the
		// decision found. Without past operators this is plain safety.
		if (!synthesize
			&& !realizability_has_game_operators<node>(compiled_fast)
			&& !has_past_operators<node>(fm)
			&& body_max_lookback<node>(fm) > 0)
		{
			LOG_DEBUG << "Pure past LTL: S/T compiled away into a "
				"safety formula, no synthesis needed";
			// The compiled formula carries the Boolean structure around
			// each S. A tau spec must hold at every step, so the obligation
			// is G(compiled).
			//
			// The wrap is distributed over top-level conjuncts and skips a
			// conjunct that is already an `always`, so `(φ S ψ) && G(χ)` gives
			// `G(curr) && G(χ)` rather than the nested `G(curr && G(χ))` that
			// the normalizer would then have to unpick. `wff_and` is N-ary,
			// so wrap_always reads every child.
			tref obligation = wrap_always(compiled_fast);
			tref out = tau::build_wff_and(obligation, safety_fm);
			// hand the inner-S auxiliaries to the caller so the
			// interpreter can seed their t=0 anchor (S(-1) = false)
			return r.with_value(full_t{ out, std::nullopt,
				std::move(unanchored_aux) });
		}
	}

	// A pure-past spec rerouted here (no lookback of its own) is an
	// invariant, as on the fast path: every step, not only step 0.
	if (!realizability_has_game_operators<node>(std::get<0>(since_trigger)))
		fm = wrap_always(fm);
	ltl_aba_solution<node> partial;
	auto maybe_r = solve_ltl_aba<node>(fm, &partial);
	// A strategy over bookkeeping bits cannot be played: solve again by
	// the default path, whose abstraction and data game give strategies
	// over the data.
	if (maybe_r.has_value() && maybe_r.value()
		&& !maybe_r.value()->executable)
	{
		partial = {};
		auto again = [&] {
			struct restore_synthesis {
				bool outer;
				~restore_synthesis() {
					ltl_propositional_synthesis = outer;
				}
			} restore{ ltl_propositional_synthesis };
			ltl_propositional_synthesis = false;
			return solve_ltl_aba<node>(fm, &partial);
		}();
		if (again.has_value()) {
			r.merge(std::move(maybe_r));
			maybe_r = std::move(again);
		} else rejected("the default path gave no strategy over the data",
			std::move(again).report());
	}
	TAU_TRY(auto maybe, std::move(maybe_r));
	// With `data_strategy`, execution plays the strategy of the data game
	// whenever that game decides the formula, in the order the
	// realizability check asks it: on codes before the abstraction, on
	// formulas only once the abstraction gives no strategy to execute.
	const ltl_aba_solution<node> game_source = maybe ? *maybe : partial;
	bool data_decided = false, data_unrealizable = false;
	auto on_data = [&](bool formulas) {
		if (!data_strategy || data_decided
			|| game_source.game_skeleton.empty()) return false;
		auto game = solve_data_game<node>(game_source.game_skeleton,
			game_source.atoms, game_source.input_props,
			game_source.output_props, formulas, data_strategy);
		if (!game.has_value()) {
			rejected("the data game could not be built",
				std::move(game).report());
			return false;
		}
		data_decided = game.value() != data_game_verdict::undecided;
		data_unrealizable = game.value() == data_game_verdict::unrealizable;
		if (unrealizable && data_unrealizable) *unrealizable = true;
		if (game.has_value()) r.merge(std::move(game));
		else {
			// the data game could not be built: a rejected candidate
			auto sc = r.open("rejected candidate");
			r.info("the data game could not be built",
				{{label::value, truncate_for_message(
					TAU_TO_STR(fm))}});
			report cand = std::move(game).report();
			cand.demote_errors_to_warnings();
			r.append(std::move(cand));
		}
		return *data_strategy != nullptr;
	};
	// the data game on formulas, the last route to an executable strategy
	auto none = [&]() { on_data(true); return nothing(); };
	auto has_data = [&]() { return data_strategy && *data_strategy; };
	if (on_data(false)) {
		LOG_DEBUG << "The data game on codes gives the strategy to execute";
		return nothing();
	}
	// The data game decides exactly: no strategy of the abstraction is
	// executed once it has shown that none exists, and refining one can
	// take minutes.
	if (data_unrealizable) {
		LOG_DEBUG << "The data game shows no strategy exists";
		return nothing();
	}
	if (!maybe) {
		LOG_DEBUG << "The abstraction has no winning strategy";
		// only the default path has a game skeleton; the others decide
		// their own abstraction exactly, as the realizability check
		// takes them
		if (unrealizable && game_source.game_skeleton.empty()
			&& !ltl_verdict_incomplete)
				*unrealizable = true;
		return none();
	}

	auto& sol = *maybe;
	// Execute the strategy the realizability check accepts, not ltlsynt's
	// first one, which may take an edge the ABA rules out.
	if (sol.executable) {
		auto refined = refine_or_observe<node>(fm, sol, false);
		if (!refined.has_value()) {
			on_data(true);
			if (!has_data()) {
				r.merge(std::move(refined));
				return std::move(r);
			}
			rejected("the ABA refinement of the strategy failed",
				std::move(refined).report());
			return nothing();
		}
		const bool survives = refined.value();
		r.merge(std::move(refined));
		if (!survives) {
			LOG_DEBUG << "No strategy survives the ABA refinement";
			if (!refined.has_value() && has_data()) {
				// the data game is the other candidate and it decided
				auto sc = r.open("rejected candidate");
				r.info("no strategy survives the ABA refinement",
					{{label::value, truncate_for_message(
						TAU_TO_STR(fm))}});
				report cand = std::move(refined).report();
				cand.demote_errors_to_warnings();
				r.append(std::move(cand));
			} else r.merge(std::move(refined));
			return none();
		}
	}

	// Algorithm B decides realizability by a route whose strategy is not
	// expressible over the user's data atoms (the P_σ / D-bit machinery):
	// `always T` would not encode it, and its default outputs could violate
	// the spec. Execution is refused; the realizability verdict from
	// `is_ltl_aba_realizable` is unaffected.
	if (!sol.executable) {
		if (none(); has_data()) return nothing();
		return r.with_error(code::unsupported_operation,
			"the specification is REALIZABLE but the synthesised "
			"strategy cannot be encoded as a safety formula (Algorithm B "
			"strategy over bookkeeping bits); it is not executable");
	}

	// Constant-output strategy: the executable form is the materialised
	// `always(⋀ o_k = c_k)` witness, not `always T`.
	if (sol.const_formula) {
		LOG_DEBUG << "Constant-output strategy: " << LOG_FM(sol.const_formula);
		return r.with_value(full_t{ sol.const_formula, std::move(sol), {} });
	}

	// Purely propositional: realizable but no data constraints to encode.
	if (sol.atoms.empty())
		return r.with_value(full_t{ tau::_T(), std::move(sol), {} });

	const auto& aut = sol.aut;

	// Trivially realizable: empty automaton.
	if (aut.num_states == 0)
		return r.with_value(full_t{ tau::_T(), std::move(sol), {} });

	if (aut.num_states > 1) {
		LOG_DEBUG << "Multi-state strategy (" << aut.num_states
			<< " states): encoding it with auxiliary one-hot state bits";
		TAU_TRY(tref encoded, encode_mealy_as_safety<node>(sol));
		return r.with_value(full_t{ encoded, std::move(sol), {} });
	}

	// Single-state strategy: the self-loop guard is the perpetual output
	// constraint. LA-R6: ltlsynt Mealy machines are input-complete, so a
	// state with no outgoing edge only arises from a degraded automaton;
	// executing it as `always T` would drop every obligation (the 1-state
	// analogue of the edgeless-state check of encode_mealy_as_safety). Not
	// executable.
	if (aut.edges.empty() || aut.edges[0].empty()) {
		if (none(); has_data()) return nothing();
		return r.with_error(code::invalid_state,
			"the single-state strategy has no outgoing edge; the "
			"automaton is degraded and cannot be executed");
	}

	// Build the disjunction of ABA guard formulas over all edges from state 0.
	tref combined = tau::_F();
	for (const auto& e : aut.edges[0]) {
		TAU_TRY(tref guard_fm, guard_to_aba<node>(
			e.guard_label, aut.aps, sol.atoms));
		TAU_TRY(tref norm_guard, normalize_non_temp<node>(guard_fm));
		combined = tau::build_wff_or(combined, norm_guard);
	}
	TAU_TRY(tref simplified, normalize_non_temp<node>(combined));
	LOG_DEBUG << "Single-state strategy: its self-loop guards give the "
		"always body " << LOG_FM(simplified);
	return r.with_value(full_t{ tau::build_wff_always(simplified),
		std::move(sol), {} });
}

// ── ltl_explain ───────────────────────────────────────────────────────────────

/** @internal @copydoc ltl_explain @endinternal */
template <NodeType node>
result<bool> ltl_explain(tref fm, std::ostream& out,
	const std::function<result<bool>()>& decide)
{
	using tau = tree<node>;
	using tt = tau::traverser;
	result<bool> r;
	bool exact_reduction = true;

	// Same input contract as api::realizable: a formula, or a spec whose
	// main part is one; a term is no formula to decide.
	if (!fm || !(tau::get(fm).is(tau::wff) || (tau::get(fm).is(tau::spec)
		&& (tt(fm) | tau::main | tau::wff | tt::ref))))
	{
		return r.with_error(code::invalid_argument, "Invalid formula");
	}

	// A/E/- have no skeleton of their own (the tester variant would flatten
	// them to "1"): reduce like is_tau_formula_sat does (or refuse, via a
	// result<T> error, where no sound encoding exists) before explaining
	// anything.
	// TODO: unlike is_ltl_aba_realizable's fast path, this does not also require has_no_boolean_combs_of_models
	//
	// A refusal here is undecided, not a decided verdict.  Nothing is
	// printed to `out` for it; the detail text is merged into `r`'s report alongside a fresh
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
			return r.with_error(code::unsupported_operation,
				messages::unknown_ctl_star_no_reduction);
		}
		auto& reduction = *reduction_r;
		out << "CTL* reduced to LTL: "
			<< tau::get(reduction.ltl_formula).to_str() << "\n";
		fm = reduction.ltl_formula;
		exact_reduction = reduction.exact;
	}

	// a disjunction the game decides is traced like full LTL
	bool by_game = realizability_has_game_operators<node>(fm);
	if (!by_game) {
		auto nf = normalize_with_temp_simp<node>(fm);
		if (nf.has_value()) {
			by_game = disjunction_needs_game<node>(fm, nf.value());
			r.merge(std::move(nf));
		} else {
			// the formula is traced as an invariant
			auto sc = r.open("rejected candidate");
			r.info("the formula could not be normalized",
				{{label::value, truncate_for_message(
					TAU_TO_STR(fm))}});
			report cand = std::move(nf).report();
			cand.demote_errors_to_warnings();
			r.append(std::move(cand));
		}
	}
	if (!by_game) {
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
	// `realizable` runs (window oracle, refinement rounds). `round_wins`
	// tells whether that round alone found a strategy every edge of which
	// is feasible, when the trace shows it.
	auto verdict = [&](std::optional<bool> round_wins = std::nullopt)
		-> result<bool>
	{
		auto real = decide ? decide()
			: is_ltl_aba_realizable<node>(fm, 0, false);
		if (!real.has_value()) {
			r.merge(std::move(real));
			return r.with_error(code::solver_error,
				messages::unknown_realizability_no_verdict);
		}
		bool realizable = real.value();
		r.merge(std::move(real));
		if (!decide && !realizable && !exact_reduction) {
			return r.with_error(code::solver_error,
				messages::unknown_ctl_star_e_witness);
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
		if (round_wins && *round_wins != realizable)
			out << "\nThe verdict is not the first round's: "
				<< (realizable ? "refinement rounds, the data game "
					"or the window oracle found a strategy"
				: "the strategy loses against the data, or the "
					"data game or the safety pipeline decided")
				<< "\n";
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
	// solve_ltl_aba fills sol.atoms before it solves
	auto extracted = sol.atoms;
	if (maybe) sol = std::move(*maybe);

	// Algorithm B's solution carries no atoms, since its strategy is over
	// bookkeeping bits; its skeleton names the extracted atoms d_i in order.
	auto shown = sol.atoms;
	if (maybe && shown.empty() && !sol.executable)
		for (size_t i = 0; i < extracted.size(); ++i)
			shown.emplace_back(extracted[i].first,
				"d_" + std::to_string(i));
	out << "\nData atoms (" << shown.size() << "):\n";
	for (auto& [f, name] : shown)
		out << "  " << name << "  :=  " << atom_text<node>(f) << "\n";

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
		return verdict(false);
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
				// Use the SAME oracle as the real
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
					TAU_TRY(tref guard_fm, guard_to_aba<node>(
						e.guard_label, aut.aps, sol.atoms));
					out << "INFEASIBLE\n";
					out << "    (formula: " << tau::get(guard_fm).to_str() << ")\n";
					all_feasible = false;
				}
			}
		}
		if (!all_feasible) {
			out << "\nFirst round: ABA-infeasible transition\n";
			return verdict(false);
		}
	}

	return verdict(true);
}

// ── CTL* operators detection ─────────────────────────────────────────────────

/** @internal @copydoc has_ctl_star_operators @endinternal */
template <NodeType node>
bool has_ctl_star_operators(tref fm) {
	using tau = tree<node>;
	return cached<node, bool>(fm, [&] {
		bool result = tau::get(fm).find_top([](tref n) {
			const auto& t = tree<node>::get(n);
			if (!t.has_child()) return false;
			auto nt = t[0].value.nt;
			return nt == tau::wff_A || nt == tau::wff_E
			    || nt == tau::wff_semantic_neg;
		}) != nullptr;
		return result;
	});
}

// ── CTL* → LTL reduction ────────────────────────────────────────────────────
//
// A restricted form of the Bloem/Schewe/Khalimov reduction
// (arXiv:1711.10636), kept SOUND for synthesis at the price of completeness:
//
//   1. Bottom-up traversal of the CTL* formula tree, tracking the polarity
//      of each node and whether it is reachable from the root only through
//      universal contexts (∧, G/always, A).
//   2. `E χ` in POSITIVE polarity: fresh witness output w_i replaces E χ.
//      With input streams, one direction output per input names the value
//      w_i's branch takes next, and G(w_i[t-1] → (G follow → N(χ'))) pins
//      χ' to that branch (shift_one_step). Without inputs, or when χ' has
//      no one-step unfolding (a past operator inside), G(w_i → χ') is added
//      instead: it ranges over ALL paths, so w_i asserts `A χ'`, which
//      implies `E χ` on a non-empty tree; with inputs that is stricter than
//      E, the reduction is not exact, and an UNREALIZABLE verdict is
//      undecided. An E with inputs that may be opened at more than one step
//      (under G, ...) has witness paths that meet; with one set of
//      directions they follow the same one from there, which CTL* does not
//      ask, so that reduction is exact for a realizable verdict only, and
//      `by_tableau` keeps one set per state of a tableau of χ instead
//      (encode_e_by_tableau), which is exact.
//   3. `A χ` in positive polarity inside a universal context: at the root
//      state (and at every state reachable only through ∧/G from it)
//      "all paths satisfy χ" IS the synthesis semantics of χ itself, so
//      A χ reduces to χ'. `G(A φ) ≡ G φ` over a strategy tree because every
//      path from an inner node is a suffix of a root path.
//   4. Everything else -- A or E in negative polarity (under ¬, on the left
//      of →, either side of ↔/⊕, in a conditional's guard), A under an
//      existential/eventual context (∨, F, sometimes, U, ...), and a `-φ`
//      under a data quantifier -- has no sound encoding here and is
//      REFUSED with a result<T> error. The caller (reduce_ctl_star_to_ltl)
//      first folds every other `-φ` to a constant and, on a refusal,
//      retries on the NNF form, where ¬A χ = E ¬χ and ¬E χ = A ¬χ turn
//      negative quantifiers positive. `A χ` is not rewritten as `¬E¬χ`:
//      that gives `¬w ∧ G(w → ¬χ)`, which every strategy satisfies by
//      holding w false.
//   5. The final LTL formula is: translated_root ∧ ⋀_i G(w_i → χ_i')

namespace ctl_star_detail {

// Counter for generating unique witness variable names. One per
// thread -- it is reset at the start of every reduction, and two
// concurrent reductions on a shared counter would hand out duplicate or
// skipped witness names.
static thread_local int witness_counter = 0;

// A fresh witness output name, `w_<n>`.
inline std::string fresh_witness_name() {
	return "w_" + std::to_string(witness_counter++);
}

// Reset counter for each new reduction
inline void reset_witness_counter(int first = 0) {
	witness_counter = first;
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
	case tau::wff_and: case tau::wff_or: {
		// N-ary: every child, not only the first two
		TAU_TRY(tref acc, N(op.child(0)));
		for (size_t i = 1; i < op.children_size(); ++i) {
			TAU_TRY(tref b, N(op.child(i)));
			acc = nt == tau::wff_and ? tau::build_wff_and(acc, b)
				: tau::build_wff_or(acc, b);
		}
		return r.with_value(acc);
	}
	case tau::wff_imply:
	case tau::wff_rimply: case tau::wff_equiv: case tau::wff_xor: {
		TAU_TRY(tref a, N(op.child(0)));
		TAU_TRY(tref b, N(op.child(1)));
		switch (nt) {
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

// ── tableau of an E's path formula ──────────────────────────────────────────
//
// An E asked at several steps of a path has a witness path from each, and
// two of them meet wherever one passes the other's start. They can follow
// the same directions from a meeting node exactly when they owe the same
// obligations there, so the directions are kept per obligation set, the
// states of a GPVW tableau of χ (Gerth/Peled/Vardi/Wolper): a chain of
// witness paths in one state merges, chains in two states do not.

namespace ctl_star_detail {

// One way to meet a set of obligations at a node: the literals it asks
// there, and the obligations it leaves from the next node on.
struct tableau_cover {
	trefs lits;
	trefs next;
	trefs expanded; // formulas already unfolded on the way to this cover
};

// How translate_ctl_star encodes an E asked at several steps: through the
// tableau of its path formula, or, cheaper, with one set of directions,
// which `shared` records -- exact for a realizable verdict only.
struct mode {
	bool by_tableau = false;
	bool shared = false;
};

// The tableau of a path formula χ: its obligation sets other than the
// empty one, the covers of {χ} and of each set, and its eventualities.
struct tableau {
	std::vector<trefs> states;
	std::vector<tableau_cover> root_covers;
	std::vector<std::vector<tableau_cover>> covers;
	trefs eventualities;
};

} // namespace ctl_star_detail

template <NodeType node>
static bool contains_tree(const trefs& v, tref f) {
	return std::ranges::any_of(v, [&](tref g) {
		return tree<node>::subtree_equals(g, f); });
}

template <NodeType node>
static void push_unique_tree(trefs& v, tref f) {
	if (!contains_tree<node>(v, f)) v.push_back(f);
}

// The covers of the obligations `todo` on top of `cur`, appended to `out`:
// a formula without a temporal operator is a literal of the node, and each
// temporal operator unfolds by its expansion law. F and U are the
// eventualities: their postponing cover keeps them in `next`. False for a
// formula the tableau does not read (a past operator, or a quantifier over a
// temporal one), or past `cap` covers (0 = unlimited).
template <NodeType node>
static bool tableau_covers(trefs todo, ctl_star_detail::tableau_cover cur,
	std::vector<ctl_star_detail::tableau_cover>& out, size_t cap)
{
	using tau = tree<node>;
	while (!todo.empty()) {
		tref f = todo.back();
		todo.pop_back();
		const auto& t = tau::get(f);
		if (t.equals_T()) continue;
		if (t.equals_F()) return true; // this branch has no cover
		if (contains_tree<node>(cur.expanded, f)) continue;
		cur.expanded.push_back(f);
		if (!t.find_top(is_temporal_quantifier<node>)) {
			push_unique_tree<node>(cur.lits, f);
			continue;
		}
		if (!t.has_child()) return false;
		const auto& op = t[0];
		auto branch = [&](trefs now, trefs later) {
			trefs td = todo;
			td.insert(td.end(), now.begin(), now.end());
			auto c = cur;
			for (tref x : later) push_unique_tree<node>(c.next, x);
			return tableau_covers<node>(std::move(td), std::move(c),
				out, cap);
		};
		switch (op.value.nt) {
		case tau::wff_and:
			for (size_t i = 0; i < op.children_size(); ++i)
				todo.push_back(op.child(i));
			continue;
		case tau::wff_or:
			for (size_t i = 0; i < op.children_size(); ++i)
				if (!branch({ op.child(i) }, {})) return false;
			return true;
		case tau::wff_always:       // G φ = φ ∧ X G φ
			todo.push_back(op.child(0));
			push_unique_tree<node>(cur.next, f);
			continue;
		case tau::wff_sometimes:    // F φ = φ ∨ X F φ
			return branch({ op.child(0) }, {}) && branch({}, { f });
		case tau::wff_until:        // φ U ψ = ψ ∨ (φ ∧ X(φ U ψ))
		case tau::wff_weak_until:
			return branch({ op.child(1) }, {})
				&& branch({ op.child(0) }, { f });
		case tau::wff_release:      // φ R ψ = ψ ∧ (φ ∨ X(φ R ψ))
			return branch({ op.child(1), op.child(0) }, {})
				&& branch({ op.child(1) }, { f });
		default:
			return false;
		}
	}
	if (cap && out.size() >= cap) return false;
	out.push_back(std::move(cur));
	return true;
}

// Index of the obligation set `s` in `states`; -1 for the empty set, which
// owes nothing, and for a set not there.
template <NodeType node>
static int tableau_index(const std::vector<trefs>& states, const trefs& s) {
	if (s.empty()) return -1;
	for (size_t i = 0; i < states.size(); ++i)
		if (states[i].size() == s.size() && std::ranges::all_of(s,
			[&](tref f) { return contains_tree<node>(states[i], f); }))
				return (int)i;
	return -1;
}

// tableau_index, adding a new non-empty set.
template <NodeType node>
static int tableau_state(std::vector<trefs>& states, const trefs& s) {
	if (s.empty()) return -1;
	if (int i = tableau_index<node>(states, s); i >= 0) return i;
	states.push_back(s);
	return (int)states.size() - 1;
}

// The tableau of the NNF path formula `chi`, or nullopt when a formula of it
// is one the tableau does not read or the states or the covers of a set
// pass `cap` (0 = unlimited).
template <NodeType node>
static std::optional<ctl_star_detail::tableau> build_tableau(tref chi,
	size_t cap)
{
	using tau = tree<node>;
	ctl_star_detail::tableau tab;
	if (!tableau_covers<node>({ chi }, {}, tab.root_covers, cap))
		return std::nullopt;
	for (const auto& c : tab.root_covers)
		tableau_state<node>(tab.states, c.next);
	for (size_t i = 0; i < tab.states.size(); ++i) {
		if (cap && tab.states.size() > cap) return std::nullopt;
		std::vector<ctl_star_detail::tableau_cover> cs;
		if (!tableau_covers<node>(tab.states[i], {}, cs, cap))
			return std::nullopt;
		for (const auto& c : cs) tableau_state<node>(tab.states, c.next);
		tab.covers.push_back(std::move(cs));
	}
	if (cap && tab.states.size() > cap) return std::nullopt;
	for (tref e : tau::get(chi).select_all([](tref n) {
		const auto& t = tree<node>::get(n);
		return t.child_is(tau::wff_sometimes)
			|| t.child_is(tau::wff_until); }))
		push_unique_tree<node>(tab.eventualities, e);
	return tab;
}

// The constraints of `E χ`, witnessed by `witness` (w at t-lb), through the
// tableau of χ. A chain is the set of witness paths in one state at a node:
//   r_i      a chain in state i is here and owes states[i] from the child
//            it reaches by its directions d_i,<input> on;
//   start    w → some cover of {χ}: its literals now, its state recorded;
//   step     a chain in i reaching the child its directions name meets some
//            cover of states[i] there;
// and, when χ has eventualities, a Miyano-Hayashi breakpoint over the
// chains: z_i marks a chain still owing the eventuality of the round (the
// one-hot k_j), a round starts with every chain when none is left, and
// G F (none is left) rejects a chain postponing an eventuality forever.
template <NodeType node>
static void encode_e_by_tableau(const ctl_star_detail::tableau& tab,
	const std::string& wname, tref witness, size_t carrier_tid,
	const std::vector<std::pair<std::string, size_t>>& inputs,
	std::vector<std::pair<std::string, tref>>& constraints,
	std::vector<std::string>& witnesses, std::vector<size_t>& witness_types)
{
	using tau = tree<node>;
	const size_t n = tab.states.size(), m = tab.eventualities.size();
	tref one = build_bf_t_type<node>(carrier_tid);
	auto bit = [&](const std::string& name, int_t back) {
		return tau::build_bf_eq(back == 0
			? build_out_var_at_t<node>(build_var_name<node>(name),
				carrier_tid, "t")
			: build_out_var_at_t_minus<node>(name, (size_t)back,
				carrier_tid), one);
	};
	auto output = [&](std::string name, size_t type) {
		witnesses.push_back(name);
		witness_types.push_back(type);
		return name;
	};
	std::vector<std::string> r(n), z(n), k(m > 1 ? m : 0);
	for (size_t i = 0; i < n; ++i) {
		r[i] = output(wname + "_r" + std::to_string(i), carrier_tid);
		if (m) z[i] = output(wname + "_z" + std::to_string(i),
			carrier_tid);
	}
	for (size_t j = 0; j < k.size(); ++j)
		k[j] = output(wname + "_k" + std::to_string(j), carrier_tid);
	std::vector<tref> follow(n, tau::_T());
	for (size_t i = 0; i < n; ++i)
		for (const auto& [iname, itype] : inputs) {
			std::string d = output(wname + "_d" + std::to_string(i)
				+ "_" + iname, itype);
			follow[i] = tau::build_wff_and(follow[i], tau::build_bf_eq(
				build_in_var_at_t<node>(build_var_name<node>(iname),
					itype, "t"),
				build_out_var_at_t_minus<node>(d, 1, itype)));
		}
	auto r_at = [&](int i, int_t back) {
		return i < 0 ? tau::_T() : bit(r[(size_t)i], back);
	};
	auto z_at = [&](int i, int_t back) {
		return i < 0 ? tau::_T() : bit(z[(size_t)i], back);
	};
	// the cover meets the eventuality of the round, read at t-1
	auto meets_round = [&](const ctl_star_detail::tableau_cover& c) {
		tref any = tau::_F();
		for (size_t j = 0; j < m; ++j)
			if (!contains_tree<node>(c.next, tab.eventualities[j]))
				any = tau::build_wff_or(any,
					m == 1 ? tau::_T() : bit(k[j], 1));
		return any;
	};
	// some cover of `cs` met here; `from` the state the chain comes from
	auto some_cover = [&](const std::vector<ctl_star_detail::tableau_cover>&
		cs, int from)
	{
		tref any = tau::_F();
		for (const auto& c : cs) {
			const int to = tableau_index<node>(tab.states, c.next);
			tref e = r_at(to, 0);
			for (tref l : c.lits) e = tau::build_wff_and(e, l);
			if (m && from >= 0) e = tau::build_wff_and(e,
				tau::build_wff_imply(z_at(from, 1), tau::build_wff_or(
					z_at(to, 0), meets_round(c))));
			any = tau::build_wff_or(any, e);
		}
		return any;
	};
	auto always = [&](tref f) {
		constraints.emplace_back(wname, tau::build_wff_always(f));
	};
	always(tau::build_wff_imply(witness, some_cover(tab.root_covers, -1)));
	for (size_t i = 0; i < n; ++i)
		always(tau::build_wff_imply(
			tau::build_wff_and(r_at((int)i, 1), follow[i]),
			some_cover(tab.covers[i], (int)i)));
	if (!m || !n) return;
	auto none_left = [&](int_t back) {
		tref all = tau::_T();
		for (size_t i = 0; i < n; ++i)
			all = tau::build_wff_and(all,
				tau::build_wff_neg(z_at((int)i, back)));
		return all;
	};
	tref every_chain = tau::_T();
	for (size_t i = 0; i < n; ++i)
		every_chain = tau::build_wff_and(every_chain,
			tau::build_wff_imply(r_at((int)i, 0), z_at((int)i, 0)));
	always(tau::build_wff_imply(none_left(1), every_chain));
	if (k.size()) {
		tref one_hot = tau::_F(), at_most_one = tau::_T();
		tref advance = tau::_T(), hold = tau::_T();
		for (size_t j = 0; j < k.size(); ++j) {
			one_hot = tau::build_wff_or(one_hot, bit(k[j], 0));
			for (size_t l = j + 1; l < k.size(); ++l)
				at_most_one = tau::build_wff_and(at_most_one,
					tau::build_wff_neg(tau::build_wff_and(
						bit(k[j], 0), bit(k[l], 0))));
			advance = tau::build_wff_and(advance, tau::build_wff_equiv(
				bit(k[j], 0), bit(k[(j + k.size() - 1) % k.size()], 1)));
			hold = tau::build_wff_and(hold, tau::build_wff_equiv(
				bit(k[j], 0), bit(k[j], 1)));
		}
		always(tau::build_wff_and(one_hot, at_most_one));
		always(tau::build_wff_imply(none_left(1), advance));
		always(tau::build_wff_imply(tau::build_wff_neg(none_left(1)), hold));
	}
	always(tau::build_wff_sometimes(none_left(0)));
}

// Recursive bottom-up translation of a CTL* state/path formula to LTL.
// Witness constraints are accumulated in `constraints` (each is a G(w → χ) pair).
// New witness output names are accumulated in `witnesses`, their types in
// `witness_types`. `inputs` are the input streams (name, type) a direction
// output pins an E witness through; `exact` is cleared when an E witness is
// encoded over all paths, or may be opened at more than one step, so an
// UNREALIZABLE verdict is undecided.
// `positive`: polarity of `fm` in the root formula; `universal`: `fm` is
// reachable from the root only through ∧ / always / A (see the header
// comment above for why both matter); `recurring`: `fm` may have to hold
// at more than one step of a path.
template <NodeType node>
static result<tref> translate_ctl_star(tref fm,
		std::vector<std::pair<std::string, tref>>& constraints,
		std::vector<std::string>& witnesses,
		std::vector<size_t>& witness_types,
		const std::vector<std::pair<std::string, size_t>>& inputs,
		bool& exact, ctl_star_detail::mode& mode,
		bool positive = true, bool universal = true,
		bool recurring = false) {
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
			exact, mode, true, false, recurring));
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
		// χ reading lookback L is first enforced at step L, as the clause
		// `E χ` would be, so `E χ` at t is stored at w[t-L]: the witness
		// literal then carries the lookback, which keeps it from firing
		// (or being asked) before the constraint below can reach it.
		const int_t lb = body_max_lookback<node>(translated_inner);
		auto w_at = [&](int_t back) {
			return tau::build_bf_eq(back == 0
				? build_out_var_at_t<node>(build_var_name<node>(wname),
					carrier_tid, "t")
				: build_out_var_at_t_minus<node>(wname, (size_t)back,
					carrier_tid), bf_one);
		};
		tref witness_wff = w_at(lb);

		// Asked at several steps, with inputs to steer: the directions are
		// kept per tableau state (see build_tableau), or shared.
		if (recurring && !inputs.empty() && !mode.by_tableau)
			mode.shared = true;
		else if (recurring && !inputs.empty()) {
			auto tab = build_tableau<node>(to_nnf<node>(translated_inner),
				ltl_ctl_star_tableau_max_states());
			if (tab) {
				encode_e_by_tableau<node>(*tab, wname, witness_wff,
					carrier_tid, inputs, constraints, witnesses,
					witness_types);
				return r.with_value(witness_wff);
			}
			auto sc = r.open("rejected candidate");
			r.info("the path formula of an E asked at several steps has no "
				"tableau within --ltl-ctl-star-tableau-max-states; its "
				"witness paths share one set of directions",
				{{label::value, truncate_for_message(
					TAU_TO_STR(translated_inner))}});
			mode.shared = true;
		}

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
			// One direction set per E: the witness paths of two steps
			// meet at the later one and follow the same directions from
			// there, which CTL* does not ask, so only a realizable
			// verdict is exact then.
			if (recurring) exact = false;
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
			tref w_prev = w_at(lb + 1);
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
			r.info("the witness path has no one-step unfolding",
				{{label::value, truncate_for_message(
					TAU_TO_STR(translated_inner))}});
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
	// all-paths synthesis semantics of the enclosing formula.
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
			witnesses, witness_types, inputs, exact, mode, true, true,
			recurring);
	}

	// A `-φ` still here sits under a data quantifier, which
	// resolve_semantic_negations does not fold through: the quantified
	// variable is free in φ.
	if (nt == tau::wff_semantic_neg) {
		return r.with_error(code::solver_error,
		    "semantic negation (-) under a data quantifier is not "
		    "implemented");
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
	// The operands that must hold at every step of a stretch: a G body,
	// the left of U/W, the right of R, the left of S, the right of T.
	std::vector<bool> rec(nch, recurring);
	switch (nt) {
	case tau::wff_always: rec[0] = true; break;
	case tau::wff_until: case tau::wff_weak_until: case tau::wff_since:
		rec[0] = true; break;
	case tau::wff_release: case tau::wff_trigger:
		if (nch == 2) rec[1] = true;
		break;
	default: break;
	}
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
			mode, ctx[i].first, ctx[i].second, rec[i]));
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
		// rimply too: `phi <- E psi` must not keep its E untranslated,
		// which would collapse to "1" in the skeleton
		case tau::wff_rimply: return r.with_value(tau::build_wff_rimply(
					new_children[0], new_children[1]));
		default:             break;
		}
	} else if (nch == 3 && nt == tau::wff_conditional) {
		return r.with_value(tau::build_wff_conditional(
			new_children[0], new_children[1], new_children[2]));
	}
	// A silent identity here would leave embedded A/E/- untranslated in
	// any connective missing from the switches above; the survivor would
	// reach the skeleton (constant "1") or bounce between
	// is_tau_formula_sat and is_ltl_aba_realizable. Refuse instead.
	return r.with_error(code::solver_error,
		"translate_ctl_star found an unhandled connective with CTL* "
		"content in its subtree; the formula cannot be reduced to LTL",
		{{label::name, node::name(nt)}});
}

/** @internal @copydoc has_semantic_negation @endinternal */
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
// lookback: what precedes its start is warm-up. A `-` under a data
// quantifier, or under a node with a non-wff child, is left in place; an
// error deciding some ψ is returned.
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

/** @internal @copydoc is_ctl_star_realizable @endinternal */
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
	// shared directions only lose: decide again with the tableau
	if (!real && reduction.shared_directions) {
		LOG_DEBUG << "Reduction with shared E directions is unrealizable: "
			"reducing again by tableau";
		TAU_TRY(reduction, reduce_ctl_star_to_ltl<node>(fm, true));
		TAU_TRY(real, is_ltl_aba_realizable<node>(reduction.ltl_formula,
			start_time, output));
	}
	// An E witness encoded without directions forces χ on every path from
	// its state, which is stricter than E; an unrealizable reduction then
	// says nothing about fm.
	if (!real && !reduction.exact) {
		return r.with_error(code::solver_error,
			messages::unknown_ctl_star_e_witness);
	}
	return r.with_value(real);
}

/** @internal @copydoc reduce_ctl_star_to_ltl @endinternal */
template <NodeType node>
result<ctl_star_reduction<node>> reduce_ctl_star_to_ltl(tref fm,
	bool by_tableau, int first_witness)
{
	using tau = tree<node>;
	result<ctl_star_reduction<node>> r;
	LOG_TRACE << "reduce_ctl_star_to_ltl/fm: " << LOG_FM(fm);
	if (ltl_finite_routing())
		return r.with_error(code::invalid_argument, "ltl-semantics finite "
			"reads LTLf, which has no path quantifiers A and E");

	// Resolved before the counter reset: deciding a `-ψ` runs a reduction
	// of its own.
	TAU_TRY(fm, resolve_semantic_negations<node>(fm));
	ctl_star_detail::reset_witness_counter(first_witness);

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
	ctl_star_detail::mode mode{ by_tableau };

	auto translated_r = translate_ctl_star<node>(fm, constraints, witnesses,
		witness_types, inputs, exact, mode);
	if (!translated_r.has_value()) {
		// A or E in negative polarity has a positive dual (¬A χ = E ¬χ,
		// ¬E χ = A ¬χ); the NNF form is equivalent, so a successful
		// translation of it is as sound as one of fm.
		tref nnf = to_nnf<node>(fm);
		if (nnf && nnf != fm) {
			LOG_DEBUG << "No direct translation: translating the NNF "
				"form instead " << LOG_FM(nnf);
			constraints.clear();
			witnesses.clear();
			witness_types.clear();
			exact = true;
			mode = { by_tableau };
			ctl_star_detail::reset_witness_counter(first_witness);
			auto nnf_r = translate_ctl_star<node>(nnf, constraints,
				witnesses, witness_types, inputs, exact, mode);
			if (nnf_r.has_value()) translated_r = std::move(nnf_r);
			else {
				// the NNF form failed too: a rejected candidate
				auto sc = r.open("rejected candidate");
				r.info("the NNF form has no sound reduction "
					"either",
					{{label::value, truncate_for_message(
						TAU_TO_STR(nnf))}});
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

	LOG_DEBUG << "CTL* reduced to LTL with " << witnesses.size()
		<< " E witnesses (exact: " << LOG_VAL(exact) << ", "
		<< (by_tableau ? "by tableau" : "by shared directions") << ")";
	LOG_TRACE << "reduce_ctl_star_to_ltl/result: " << LOG_FM(result);
	return r.with_value(ctl_star_reduction<node>{result, witnesses,
		std::move(witness_types), exact, mode.shared});
}

} // namespace idni::tau_lang
