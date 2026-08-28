// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__TABLE_STEP_PROVIDER_TMPL_H__
#define __IDNI__TAU__TABLE_STEP_PROVIDER_TMPL_H__

#include <map>
#include <memory>
#include <set>

namespace idni::tau_lang {

// True if `atom` is a bf_neq disequality over an atomless-typed BA -- the
// shape a pure inequality web needs. Structural only: does not inspect
// operand shape beyond the top-level operator, since solve_inequality_
// system_atomless's cofactor elimination doesn't require the variable at
// the top either (e.g. `o1[t]!=o2[t]` XOR-folds to `o1[t]^o2[t] != 0`).
template <NodeType node>
static bool ocltl_direct_decode_atom_shaped(tref atom) {
	using tau = tree<node>;
	const auto& t = tau::get(atom);
	if (!t.has_child()) return false;
	if (t[0].value.nt != tau::bf_neq) return false;
	return pack_type_is_atomless<node>(find_ba_type<node>(atom));
}

// True when every atom on an edge's template list is direct-decode shaped --
// the whole edge is then eligible for the atomless decode below instead of
// the general solve_with_max_update path.
template <NodeType node>
static bool ocltl_direct_decode_edge_eligible(const trefs& tmpls) {
	for (tref atom : tmpls)
		if (!ocltl_direct_decode_atom_shaped<node>(atom)) return false;
	return true;
}

// True for an output-direction io_var coordinate -- the only kind this
// decode may pick a value for; an input coordinate is the environment's
// to supply.
template <NodeType node>
static bool ocltl_is_output_coord(tref v) {
	using tau = tree<node>;
	const auto& tv = tau::get(v);
	bool is_input = tv.child_is(tau::io_var)
		? tv[0].is_input_variable() : tv.is_input_variable();
	return !is_input;
}

// Direct Prop-4/TABA-distinct-representatives decode for an eligible edge:
// grounds every template atom against committed memory/current inputs, then
// decides the step's new output coordinate(s) one at a time via
// solve_inequality_system_atomless, in first-appearance order (a mutual
// constraint naming two undecided coordinates is deferred to the second
// coordinate's turn, e.g. atomless2's o1[t]!=o2[t] forces o1 before o2).
//
// Grounds at time_point, not formula_time_point: during warm-up that keeps
// this the real constraint on the step being decided rather than a future
// position's. Any resulting negative-time (before-stream-start) references
// simply stay free -- only output-direction vars are tracked as `coords`
// below, so they become ordinary extra degrees of freedom for the solver.
// Post warm-up, time_point == formula_time_point, so this is a no-op there.
//
// Returns nullopt if some atom doesn't reduce to <=1 free output
// coordinate, or the system is unsatisfiable for this history; either way
// produce() falls back to the general path.
//
// `ledger` is this run's fresh_element_ledger, threaded into every solve so
// its cross-step exclusion fast path can engage; each decided coordinate is
// registered with it only once the whole edge succeeds, so a sibling
// coordinate of the SAME step still sees it as external.
template <NodeType node>
static std::optional<solution<node>> ocltl_direct_decode_edge(
	const trefs& tmpls, const assignment<node>& memory,
	size_t time_point, size_t formula_time_point,
	fresh_element_ledger& ledger)
{
	using tau = tree<node>;
	(void)formula_time_point; // kept for call-site symmetry with produce()

	trefs grounded;
	grounded.reserve(tmpls.size());
	for (tref tmpl : tmpls) {
		tref updated = update_to_time_point<node>(tmpl, (int_t)time_point);
		grounded.push_back(rewriter::replace<node>(updated, memory));
	}

	// New output coordinates to decide, in first-appearance order. A free
	// var that isn't one of THIS step's own output coordinates (wrong
	// direction, or a before-stream-start lookback reference only possible
	// during warm-up) is left out of `coords` and simply stays free below.
	trefs coords;
	auto coord_index = [&](tref v) -> size_t {
		for (size_t i = 0; i < coords.size(); ++i)
			if (tau::subtree_equals(coords[i], v)) return i;
		coords.push_back(v);
		return coords.size() - 1;
	};
	for (tref g : grounded)
		for (tref v : get_free_vars<node>(g))
			if (ocltl_is_output_coord<node>(v)
				&& get_io_time_point<node>(v) == (int_t)time_point)
				coord_index(v);

	solution<node> sol;
	std::vector<bool> consumed(grounded.size(), false);
	for (tref var : coords) {
		inequality_system<node> sys;
		bool var_constrained = false;
		for (size_t i = 0; i < grounded.size(); ++i) {
			if (consumed[i]) continue;
			tref g = rewriter::replace<node>(grounded[i], sol);
			trefs fv = get_free_vars<node>(g);
			if (fv.empty()
				|| (fv.size() == 1 && tau::subtree_equals(fv[0], var)))
			{
				sys.insert(g);
				consumed[i] = true;
				if (!fv.empty()) var_constrained = true;
			}
			// else: still names a second, not-yet-decided coordinate --
			// deferred to that coordinate's own turn.
		}
		solver_options opts;
		opts.type_id = find_ba_type<node>(var);
		opts.splitter_one = node::ba::splitter_one(
			get_ba_type_tree<node>(opts.type_id));
		opts.ledger = &ledger;
		auto val = solve_inequality_system_atomless<node>(sys, opts);
		if (!val) return std::nullopt; // genuinely unsat for this history
		// The solver's keys are bf-wrapped variables, not the bare `var`
		// from get_free_vars; unwrap to compare by content. Keep the
		// matched bf-wrapped key as `sol_key`: both the substitution below
		// and this function's return value need that shape.
		tref v = nullptr;
		tref sol_key = var; // unconstrained fallback: never matched again
		for (const auto& [k, kv] : *val) {
			tref k_var = tau::get(k).child_is(tau::variable)
				? tau::get(k).first_tree().get() : k;
			if (tau::subtree_equals(k_var, var)) { v = kv; sol_key = k; break; }
		}
		if (!v && var_constrained)
			// var was named by a row of sys, so a miss here is a bug, not
			// the unconstrained case below -- bail rather than silently
			// substitute a value that may violate its own constraint.
			return std::nullopt;
		// var had no constraint of its own (every atom mentioning it was
		// also mutual and got attributed elsewhere -- shouldn't happen for
		// a real spec, but any concrete element is a valid witness here).
		if (!v) v = pack_zero_constant<node>(opts.type_id);
		if (!v) v = tau::_0(opts.type_id);
		sol[sol_key] = v;
	}
	// An unconsumed atom is fine as long as no coordinate this edge is
	// deciding is still tangled in it -- e.g. a before-stream-start
	// lookback reference (only possible during warm-up, grounded at
	// time_point) never becomes a coordinate and is never substituted, so
	// such an atom is vacuously satisfiable (an infinite-domain existential
	// over a coordinate never picked) and safe to leave alone. Two
	// coordinates still tangled together past every turn is the real
	// "unexpected shape" this guards against.
	for (size_t i = 0; i < grounded.size(); ++i) {
		if (consumed[i]) continue;
		tref g = rewriter::replace<node>(grounded[i], sol);
		for (tref v : get_free_vars<node>(g))
			for (tref c : coords)
				if (tau::subtree_equals(c, v)) return std::nullopt;
	}
	// Commit every coordinate this edge decided, once, only now that the
	// whole edge succeeded -- v is registered exactly as the solver
	// produced it. A discarded decode (nullopt above) commits nothing.
	for (const auto& [key, v] : sol)
		ledger_commit_witness<node>(ledger, v, tau::get(v).get_ba_type());
	return sol;
}

template <NodeType node>
table_step_provider<node>::table_step_provider(
	codegen::strategy strat,
	std::vector<std::pair<std::string, tref>> input_atoms,
	std::vector<std::string> flag_outputs,
	std::vector<std::vector<std::vector<std::pair<std::string, tref>>>> edge_witnesses,
	std::vector<std::vector<trefs>> edge_witness_templates,
	std::vector<std::vector<std::vector<bool>>> edge_witness_template_is_counter)
	: strat_(std::move(strat)), flag_outputs_(std::move(flag_outputs)),
	  edge_witness_template_is_counter_(std::move(edge_witness_template_is_counter)),
	  state_(strat_.initial_state)
{
	using tau = tree<node>;
	for (auto& [name, atom_ref] : input_atoms)
		input_atoms_.emplace_back(std::move(name), tau::geth(atom_ref));
	edge_witnesses_.reserve(edge_witnesses.size());
	for (auto& state_edges : edge_witnesses) {
		auto& out_state = edge_witnesses_.emplace_back();
		out_state.reserve(state_edges.size());
		for (auto& edge : state_edges) {
			auto& out_edge = out_state.emplace_back();
			out_edge.reserve(edge.size());
			for (auto& [name, val] : edge)
				out_edge.emplace_back(std::move(name), tau::geth(val));
		}
	}
	edge_witness_templates_.reserve(edge_witness_templates.size());
	edge_direct_decode_eligible_.reserve(edge_witness_templates.size());
	for (auto& state_edges : edge_witness_templates) {
		auto& out_state = edge_witness_templates_.emplace_back();
		out_state.reserve(state_edges.size());
		auto& elig_state = edge_direct_decode_eligible_.emplace_back();
		elig_state.reserve(state_edges.size());
		for (auto& edge : state_edges) {
			auto& out_edge = out_state.emplace_back();
			out_edge.reserve(edge.size());
			for (tref tmpl : edge) out_edge.push_back(tau::geth(tmpl));
			bool eligible = !edge.empty()
				&& ocltl_direct_decode_edge_eligible<node>(edge);
#ifdef LOG_DEBUG
			// LOG_* is only defined when linked against the interpreter;
			// an emitted artifact includes this file standalone.
			if (!edge.empty() && !eligible)
				LOG_DEBUG << "table_step_provider: edge falls back to "
					"solution_with_max_update (template atoms are not a "
					"pure atomless inequality web)\n";
#endif
			elig_state.push_back(eligible);
		}
	}
}

template <NodeType node>
trefs table_step_provider<node>::live_probe_atoms() const {
	trefs atoms;
	atoms.reserve(input_atoms_.size());
	for (auto& [name, a] : input_atoms_) atoms.push_back(a->get());
	for (auto& state_edges : edge_witness_templates_)
		for (auto& edge_tmpls : state_edges)
			for (auto& h : edge_tmpls) atoms.push_back(h->get());
	return atoms;
}

template <NodeType node>
std::optional<solution<node>> table_step_provider<node>::produce(
	const trefs&, const assignment<node>& memory,
	size_t time_point, size_t formula_time_point)
{
	using tau = tree<node>;

	// Guard bits: one per baked input atom, evaluated fresh against the
	// memory the interpreter just committed this step's raw inputs into.
	const size_t n = input_atoms_.size();
	auto ap = std::make_unique<bool[]>(n ? n : 1);
	for (size_t k = 0; k < n; ++k)
		ap[k] = evaluate_atom<node>(input_atoms_[k].second->get(), memory,
			formula_time_point);
	const codegen::edge* e = codegen::strategy_step(strat_, state_, ap.get());
	if (!e) return std::nullopt;

	solution<node> result;
	const size_t carrier_tid =
		get_ba_type_id<node>(pack_bool_carrier_type<node>());
	// Flag outputs: read straight off the matched edge's guard, materialized
	// the same way the interpreter's own solve path fills a state bit.
	for (size_t k = 0; k < flag_outputs_.size(); ++k) {
		std::int8_t g = e->guard[n + k];
		if (g == 0) continue;  // don't-care: interpreter default-zeros it
		tref val = pack_value_constant<node>(carrier_tid, g == 1 ? 1 : 0);
		tref key = build_out_var_at_n<node>(
			flag_outputs_[k], (int_t)time_point, carrier_tid);
		result.emplace(key, val);
	}

	const auto& state_edges = strat_.edges[state_];
	const size_t edge_idx = (size_t)(e - state_edges.data());

	// Witness/data outputs baked for this specific edge.
	if (state_ >= 0 && state_ < (int)edge_witnesses_.size()
		&& edge_idx < edge_witnesses_[state_].size())
		for (auto& [name, val] : edge_witnesses_[state_][edge_idx]) {
			tref v = val->get();
			size_t vt = tau::get(v).get_ba_type();
			tref key = build_out_var_at_n<node>(
				name, (int_t)time_point, vt);
			result.emplace(key, v);
		}

	// Input-dependent witness outputs: no bakeable constant exists, so
	// ground the edge's atom conjunction and solve for what remains, the
	// same way the solve provider does; step()'s shared commit block
	// decides what reaches memory/output.
	if (state_ >= 0 && state_ < (int)edge_witness_templates_.size()
		&& edge_idx < edge_witness_templates_[state_].size()
		&& !edge_witness_templates_[state_][edge_idx].empty())
	{
		const auto& htmpls = edge_witness_templates_[state_][edge_idx];
		trefs tmpls;
		tmpls.reserve(htmpls.size());
		for (auto& h : htmpls) tmpls.push_back(h->get());

		bool eligible = state_ < (int)edge_direct_decode_eligible_.size()
			&& edge_idx < edge_direct_decode_eligible_[state_].size()
			&& edge_direct_decode_eligible_[state_][edge_idx];

		std::optional<solution<node>> ws;
		if (eligible)
			ws = ocltl_direct_decode_edge<node>(tmpls, memory,
				time_point, formula_time_point, ledger_);
		if (!ws) {
			// Ineligible edge, or (defensively) an eligible one whose direct
			// decode failed at runtime -- the general path. Each template
			// atom grounds its own "t": a hoisted positional atom's
			// relativization means the counter's own absolute step
			// (time_point), everything else means formula_time_point --
			// they only diverge during lookback warmup (time_point <
			// formula_time_point), so this is a no-op split otherwise.
			const auto& is_counter = state_ < (int)edge_witness_template_is_counter_.size()
				&& edge_idx < edge_witness_template_is_counter_[state_].size()
				? edge_witness_template_is_counter_[state_][edge_idx]
				: std::vector<bool>{};
			tref conj = nullptr;
			for (size_t k = 0; k < tmpls.size(); ++k) {
				bool is_ctr = k < is_counter.size() && is_counter[k];
				tref grounded = update_to_time_point<node>(tmpls[k],
					is_ctr ? (int_t)time_point : (int_t)formula_time_point);
				conj = conj ? tau::build_wff_and(conj, grounded) : grounded;
			}
			tref current = rewriter::replace<node>(conj, memory);
			current = normalize_non_temp<node>(current);
			ws = solution_with_max_update<node>(current, time_point);
		}
		if (!ws) return std::nullopt;
		for (const auto& [var, value] : ws.value())
			result.emplace(var, value);
	}

	state_ = e->dst;
	return result;
}

template <NodeType node>
std::pair<std::shared_ptr<table_step_provider<node>>, std::pair<int, int>>
make_table_provider(const ltl_aba_solution<node>& sol)
{
	using namespace codegen_detail;

	std::map<std::string, tref> prop_to_atom;
	for (auto& [atom_ref, prop] : sol.atoms) prop_to_atom[prop] = atom_ref;
	std::map<std::string, int> prop_to_ap;
	for (int i = 0; i < (int)sol.aut.aps.size(); ++i)
		prop_to_ap[sol.aut.aps[i]] = i;

	std::vector<int> in_ap_idx;
	std::vector<std::pair<std::string, tref>> input_atoms;
	for (auto& p : sol.input_props) {
		in_ap_idx.push_back(prop_to_ap.count(p) ? prop_to_ap.at(p) : -1);
		input_atoms.emplace_back(p, prop_to_atom.at(p));
	}

	// Flag outputs keep a guard slot (their bit is the value; the counter's
	// own o__ltl_ctr bits stay too -- the interpreter's write path already
	// excludes them from real streams); data-typed output atoms instead
	// become per-edge templates below.
	std::vector<int> flag_out_ap_idx;
	std::vector<std::string> flag_outputs;
	std::set<std::string> template_props;
	for (auto& p : sol.output_props) {
		tref atom_ref = prop_to_atom.at(p);
		// This strategy stepper carries no PWR concept of its own -- ordinary
		// classification throughout, same as build_program_desc's non-PWR
		// (revisable=false) path.
		if (atom_is_data_typed<node>(atom_ref, /*revisable=*/false)) {
			template_props.insert(p);
			continue;
		}
		trefs fvars = get_free_vars<node>(atom_ref);
		if (fvars.size() != 1) return {nullptr, {0, 0}};
		flag_out_ap_idx.push_back(prop_to_ap.count(p) ? prop_to_ap.at(p) : -1);
		flag_outputs.push_back(get_var_name<node>(fvars[0]));
	}

	codegen::strategy strat;
	strat.num_states = sol.aut.num_states;
	strat.initial_state = sol.aut.initial_state;
	strat.num_inputs = (int)in_ap_idx.size();
	strat.edges.resize(sol.aut.num_states);
	std::vector<std::vector<trefs>> templates(sol.aut.num_states);
	std::vector<std::vector<std::vector<bool>>> template_is_counter(sol.aut.num_states);
	for (int s = 0; s < sol.aut.num_states; ++s) {
		const auto& edges = sol.aut.edges.size() > (size_t)s
		                  ? sol.aut.edges[s] : std::vector<hoa_edge>{};
		for (auto& e : edges) {
			// One edge per cube; an unparsable guard label refuses the edge.
			auto cubes = parse_guard_cubes(e.guard_label);
			if (!cubes) continue;
			for (auto& cube : *cubes) {
				codegen::edge ed;
				ed.dst = e.dst;
				ed.guard = guard_from_cube(cube,
					in_ap_idx, flag_out_ap_idx);
				trefs tmpls;
				std::vector<bool> is_counter;
				for (auto& [ap_idx, positive] : cube) {
					if (!positive) continue;
					if (ap_idx < 0
						|| ap_idx >= (int)sol.aut.aps.size()) continue;
					const auto& prop = sol.aut.aps[ap_idx];
					if (template_props.count(prop)) {
						tmpls.push_back(prop_to_atom.at(prop));
						is_counter.push_back(
							sol.counter_relativized_props.count(prop) > 0);
					}
				}
				strat.edges[s].push_back(std::move(ed));
				templates[s].push_back(std::move(tmpls));
				template_is_counter[s].push_back(std::move(is_counter));
			}
		}
	}

	auto [lookback, hip] = compute_auto_continue_bounds<node>(sol.atoms);
	// Positional atoms were relativized by apply_step_counter_encoding; the
	// pre-desugared highest step it recorded wins, as in build_program_desc.
	if (sol.counter_highest_initial_pos >= 0)
		hip = (int)sol.counter_highest_initial_pos;

	auto provider = std::make_shared<table_step_provider<node>>(
		std::move(strat), std::move(input_atoms), std::move(flag_outputs),
		std::vector<std::vector<std::vector<std::pair<std::string, tref>>>>{},
		std::move(templates), std::move(template_is_counter));
	return {provider, {lookback, hip}};
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TABLE_STEP_PROVIDER_TMPL_H__
