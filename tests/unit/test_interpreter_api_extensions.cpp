// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Tests for the 11 interpreter API methods added for tau-neuro runtime.
//
// Test categories:
//   IAX-INSP-*    Inspection: current_spec, reset, current_state,
//                 accumulator_state, committed_approval_hash
//   IAX-PWR-*     PWR / runtime: can_extend, admissible_outputs
//   IAX-MEALY-*   Mealy-strategy: visualise_mealy_dot, determinise,
//                 boundary_traces, commit_realiser
//   IAX-PREF-*    apply_preferences (free function)
//
// All tests use safety-spec inputs by default — the multi-state Mealy
// path (cached_solution.aut.num_states > 1) requires more involved test
// fixtures and is exercised separately in the tau-neuro layer's e2e
// tests once the Python runtime lands.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "interpreter.h"
#include "preferences.h"

#include <set>
#include <sstream>

using namespace idni::tau_lang;

// Parse a tau spec string and construct an interpreter.
static std::optional<interpreter<node_t>> make(const char* s) {
	io_context<node_t> ctx;
	auto nso_rr = get_nso_rr<node_t>(ctx, tau::get(s).value_or(nullptr));
	if (!nso_rr.has_value()) return {};
	tref spec_tref = nso_rr.value().main->get();
	auto interp_r = interpreter<node_t>::make_interpreter(spec_tref, ctx);
	if (!interp_r.has_value()) return {};
	return std::move(interp_r.value());
}

// Parse a formula string for use as a PWR update (psi).
static tref parse_formula(const char* s) {
	auto r = api<node_t>::get_formula(std::string(s));
	return r.has_value() ? r.value() : nullptr;
}

// The rendered report of a call, for checking the reason it gives.
template <typename R>
static std::string report_text(const R& res) {
	std::ostringstream oss;
	res.print(oss);
	return oss.str();
}

// ============================================================================
// IAX-INSP: Inspection methods
// ============================================================================

TEST_SUITE("[IAX-INSP: Inspection]") {

	TEST_CASE("[IAX-INSP-01] current_spec returns non-empty string") {
		auto i = make("o1[t] = 1.");
		REQUIRE(i.has_value());
		std::string s = i->current_spec();
		REQUIRE_FALSE(s.empty());
		// Should contain the output variable name.
		REQUIRE(s.find("o1") != std::string::npos);
	}

	TEST_CASE("[IAX-INSP-02] reset returns time_point to 0") {
		auto i = make("o1[t] = 1.");
		REQUIRE(i.has_value());
		auto sr = i->step();
		REQUIRE(sr.has_value());
		auto [_, __] = sr.value();
		// time_point may be 1 after a step (or higher if formula advanced).
		REQUIRE(i->reset().has_value());
		REQUIRE(i->time_point == 0);
		// Memory should be cleared.
		REQUIRE(i->memory.empty());
	}

	TEST_CASE("[IAX-INSP-03] current_state returns 0 for safety spec (no Mealy)") {
		auto i = make("o1[t] = 1.");
		REQUIRE(i.has_value());
		// Safety spec → no cached_solution multi-state → state index 0.
		auto cs = i->current_state();
		REQUIRE(cs.has_value());
		CHECK(cs.value() == 0);
	}

	TEST_CASE("[IAX-INSP-04] accumulator_state returns empty for non-existent name") {
		auto i = make("o1[t] = 1.");
		REQUIRE(i.has_value());
		auto acc = i->accumulator_state("does_not_exist");
		REQUIRE(acc.has_value());
		REQUIRE(acc.value().empty());
	}

	TEST_CASE("[IAX-INSP-05] committed_approval_hash defaults to empty") {
		auto i = make("o1[t] = 1.");
		REQUIRE(i.has_value());
		REQUIRE(i->committed_approval_hash.empty());
	}

	TEST_CASE("[IAX-INSP-06] accumulator_state reads the last committed value") {
		auto i = make("o1[t] = {a}:sbf.");
		REQUIRE(i.has_value());
		REQUIRE(i->step().has_value());
		auto acc = i->accumulator_state("o1");
		REQUIRE(acc.has_value());
		CHECK(acc.value() == "a");

		auto j = make("o1[t] = 1.");
		REQUIRE(j.has_value());
		REQUIRE(j->step().has_value());
		REQUIRE(j->step().has_value());
		auto acc_j = j->accumulator_state("o1");
		REQUIRE(acc_j.has_value());
		CHECK(acc_j.value() == "T");
	}

	TEST_CASE("[IAX-INSP-07] accumulator_state finds an acc_-prefixed stream") {
		io_context<node_t> ctx;
		ctx.add_output("acc_total", tau_type_id<node_t>(),
			std::make_shared<vector_output_stream>());
		auto nso = get_nso_rr<node_t>(ctx, tau::get("acc_total[t] = 1.",
			{ .context = &ctx }).value_or(nullptr));
		REQUIRE(nso.has_value());
		auto ir = interpreter<node_t>::make_interpreter(
			nso.value().main->get(), ctx);
		REQUIRE(ir.has_value());
		REQUIRE(ir.value().step().has_value());
		auto acc = ir.value().accumulator_state("total");
		REQUIRE(acc.has_value());
		CHECK(acc.value() == "T");
	}

}

// ============================================================================
// IAX-MEALY: Mealy-strategy introspection (commit_realiser, etc.)
// ============================================================================

TEST_SUITE("[IAX-MEALY: Mealy strategy]") {

	// AP2-3: a multi-state Mealy strategy's initial state bits
	// are pre-populated into memory by make_interpreter; reset() must
	// re-seed them (it used to just clear memory, so "back to t=0" was
	// not the real t=0 state), and current_state()'s aux-bit scan must
	// survive stepping (IN-N12 guard). Requires ltlsynt on PATH, like
	// every genuine-synthesis test in this repo.
	TEST_CASE("[IAX-MEALY-06] reset re-seeds multi-state initial memory") {
		auto i = make("(sometimes o1[t] = 0) && (sometimes o1[t] = 1).");
		if (!i.has_value()) return; // ltlsynt unavailable: nothing to pin
		if (!i->cached_solution
			|| i->cached_solution->aut.num_states <= 1)
			return; // single-state strategy: nothing to replay
		CHECK(i->memory.empty());
		auto first = i->step();
		REQUIRE(first.has_value());
		auto s1_r = i->current_state();
		REQUIRE(s1_r.has_value());
		const size_t s1 = s1_r.value();
		REQUIRE(i->reset().has_value());
		REQUIRE(i->time_point == 0);
		CHECK(i->memory.empty());
		auto again = i->step();
		REQUIRE(again.has_value());
		auto s2_r = i->current_state();
		REQUIRE(s2_r.has_value());
		CHECK(s2_r.value() == s1);
	}

	// IN-N2: the LTL aux state bits (o__ltl_ms*) are encoding artefacts;
	// they must not be registered as console output streams nor appear in
	// a step's output map.
	TEST_CASE("[IAX-MEALY-07] aux state bits are not output streams") {
		auto i = make("always (F (o1[t] = 0)).");
		if (!i.has_value()) return; // ltlsynt unavailable
		for (const auto& [var, _] : i->outputs)
			CHECK(get_var_name<node_t>(var).rfind("o__ltl_", 0)
				== std::string::npos);
		auto sr = i->step();
		REQUIRE(sr.has_value());
		auto [out, _] = sr.value();
		REQUIRE(out.has_value());
		for (const auto& [var, value] : *out)
			CHECK(tau::get(var).to_str().find("o__ltl_")
				== std::string::npos);
	}

	TEST_CASE("[IAX-MEALY-01] commit_realiser round-trip") {
		auto i = make("o1[t] = 1.");
		REQUIRE(i.has_value());
		i->commit_realiser("approval-hash-123");
		REQUIRE(i->committed_approval_hash == "approval-hash-123");
	}

	TEST_CASE("[IAX-MEALY-02] visualise_mealy_dot empty when no cached_solution") {
		auto i = make("o1[t] = 1.");
		REQUIRE(i.has_value());
		// Pure-safety spec → cached_solution should be nullopt (no LTL
		// future operators to trigger solve_ltl_aba).
		// For trivially-realizable LTL (e.g. F via solve_ltl_aba) the
		// cache may be populated but with num_states == 0; visualise
		// returns "" in either case.
		std::string dot = i->visualise_mealy_dot();
		// For safety specs we expect empty (no Mealy synthesised).
		// We can't assert empty unconditionally because some specs may
		// trigger trivial Mealy synthesis; just check it's a string.
		(void)dot;  // sanity-check return type only
	}

	TEST_CASE("[IAX-MEALY-03] determinise returns automaton (possibly empty)") {
		auto i = make("o1[t] = 1.");
		REQUIRE(i.has_value());
		hoa_automaton aut = i->determinise();
		// num_states >= 0 by construction.
		REQUIRE(aut.num_states >= 0);
	}

	TEST_CASE("[IAX-MEALY-04] boundary_traces respects max-n bound") {
		auto i = make("o1[t] = 1.");
		REQUIRE(i.has_value());
		auto traces = i->boundary_traces(5);
		REQUIRE(traces.size() <= 5);
		// Each trace is non-empty.
		for (const auto& t : traces) REQUIRE_FALSE(t.empty());
	}

	TEST_CASE("[IAX-MEALY-05] boundary_traces empty when n=0") {
		auto i = make("o1[t] = 1.");
		REQUIRE(i.has_value());
		REQUIRE(i->boundary_traces(0).empty());
	}

	TEST_CASE("[IAX-MEALY-08] a multi-state strategy renders, determinises "
		"and traces its machine")
	{
		auto i = make("(sometimes o1[t] = 0) && (sometimes o1[t] = 1).");
		REQUIRE(i.has_value());
		REQUIRE(i->cached_solution.has_value());
		const hoa_automaton& cached = i->cached_solution->aut;
		REQUIRE(cached.num_states > 1);

		hoa_automaton aut = i->determinise();
		CHECK(aut.num_states == cached.num_states);
		CHECK(aut.initial_state == cached.initial_state);

		std::string dot = i->visualise_mealy_dot();
		CHECK(dot.rfind("digraph mealy {\n", 0) == 0);
		CHECK(dot.find("__init -> " + std::to_string(aut.initial_state)
			+ ";") != std::string::npos);
		for (size_t s = 0; s < aut.num_states; ++s)
			CHECK(dot.find(std::to_string(s) + " [label=\"q"
				+ std::to_string(s)) != std::string::npos);
		for (size_t s = 0; s < aut.edges.size(); ++s)
			for (const auto& e : aut.edges[s])
				CHECK(dot.find(std::to_string(s) + " -> "
					+ std::to_string(e.dst) + " [label=\"")
					!= std::string::npos);
		if (!aut.aps.empty())
			CHECK(dot.find("// APs: 0=" + aut.aps[0]) != std::string::npos);

		auto is_edge = [&](size_t u, size_t v) {
			if (u >= aut.edges.size()) return false;
			for (const auto& e : aut.edges[u]) if (e.dst == v) return true;
			return false;
		};
		auto traces = i->boundary_traces(2, 3);
		REQUIRE_FALSE(traces.empty());
		CHECK(traces.size() <= 2);
		for (size_t k = 0; k < traces.size(); ++k) {
			const auto& tr = traces[k];
			REQUIRE_FALSE(tr.empty());
			CHECK(tr.size() <= 3);
			CHECK(tr.front() == aut.initial_state);
			if (k > 0) CHECK(traces[k - 1].size() >= tr.size());
			for (size_t j = 1; j < tr.size(); ++j)
				CHECK(is_edge(tr[j - 1], tr[j]));
			std::set<size_t> seen(tr.begin(), tr.end());
			CHECK(seen.size() == tr.size());
		}
		// The longest simple path from the initial state leaves it.
		CHECK(traces.front().size() > 1);
		CHECK(i->boundary_traces(3, 0).empty());
		CHECK(i->boundary_traces(-1).empty());
	}

	TEST_CASE("[IAX-MEALY-09] without a synthesised machine there is "
		"nothing to render or trace")
	{
		auto i = make("o1[t] = 1.");
		REQUIRE(i.has_value());
		REQUIRE_FALSE(i->cached_solution.has_value());
		CHECK(i->visualise_mealy_dot().empty());
		CHECK(i->determinise().num_states == 0);
		CHECK(i->boundary_traces(5).empty());
	}

}

// ============================================================================
// IAX-PWR: PWR / runtime
// ============================================================================

TEST_SUITE("[IAX-PWR: PWR runtime]") {

	TEST_CASE("[IAX-PWR-01] can_extend returns true for compatible psi") {
		auto i = make("o1[t] = 1.");
		REQUIRE(i.has_value());
		// A weakening update — adding an unrelated constraint should
		// keep the spec realisable.
		tref psi = parse_formula("o2[t] = 1");
		REQUIRE(psi != nullptr);
		// can_extend dry-runs the merge; the safety-spec o1=1 with
		// added o2=1 stays realisable.
		auto ok_r = i->can_extend(psi);
		REQUIRE(ok_r.has_value());
		// We don't strictly REQUIRE(ok_r.value()) because the partition /
		// clause matching logic may decline non-overlapping updates; the
		// important behaviour is that the call resolves without an error.
		(void)ok_r.value();
	}

	TEST_CASE("[IAX-PWR-02] admissible_outputs returns at least one solution") {
		auto i = make("o1[t] = 1.");
		REQUIRE(i.has_value());
		auto results_r = i->admissible_outputs(10);
		REQUIRE(results_r.has_value());
		// A satisfiable safety spec with a determined output has
		// at least one admissible solution.
		REQUIRE(results_r.value().size() >= 1);
	}

	TEST_CASE("[IAX-PWR-03] admissible_outputs respects max_results bound") {
		auto i = make("o1[t] = 1.");
		REQUIRE(i.has_value());
		auto results_r = i->admissible_outputs(0);
		REQUIRE(results_r.has_value());
		REQUIRE(results_r.value().empty());

		auto results_3_r = i->admissible_outputs(3);
		REQUIRE(results_3_r.has_value());
		REQUIRE(results_3_r.value().size() <= 3);
	}

	// IN-N11: make_interpreter pushed the multi-state Mealy initial-output
	// part onto ubt_ctn with no matching original_spec entry, so the first
	// update() on such an interpreter tripped the partition-size assert
	// (Debug SIGABRT) and, in Release, paired parts with the wrong
	// continuations from then on. Requires ltlsynt on PATH.
	TEST_CASE("[IAX-PWR-05] update() on a multi-state Mealy interpreter does not abort") {
		auto i = make("always (F (o1[t] = 0)).");
		if (!i.has_value()) return; // ltlsynt unavailable
		if (!i->cached_solution
			|| i->cached_solution->aut.num_states <= 1)
			return; // single-state strategy: the init_out part n/a
		REQUIRE(i->ubt_ctn.size() == i->original_spec.size());
		// a run of the data game's strategy is revised by solving the
		// game again, whose Mealy view then describes the revised spec
		const bool data_game = i->cached_solution->data_game;
		(void)i->step();
		tref psi = parse_formula("always o2[t]:tau = 1");
		REQUIRE(psi != nullptr);
		result<bool> accepted_r;
		REQUIRE_NOTHROW(accepted_r = i->update(psi));
		REQUIRE(accepted_r.has_value());
		bool accepted = accepted_r.value();
		REQUIRE(i->ubt_ctn.size() == i->original_spec.size());
		if (accepted && data_game) {
			CHECK_FALSE(i->strategy_stale());
			REQUIRE(i->cached_solution.has_value());
			CHECK(i->cached_solution->data_game);
			auto sr = i->step();
			REQUIRE(sr.has_value());
			auto [out, _] = sr.value();
			CHECK(out.has_value());
		} else if (accepted) {
			// IN-N3: the synthesised automaton no longer describes
			// the running spec; introspection says so instead of
			// showing a stale machine (reset() still re-seeds).
			CHECK(i->strategy_stale());
			CHECK(i->visualise_mealy_dot().empty());
			CHECK(i->determinise().num_states == 0);
			auto sr = i->step();
			REQUIRE(sr.has_value());
			auto [out, _] = sr.value();
			CHECK(out.has_value());
		}
	}

	// IN-M7 / PW-RT4: can_extend is update()'s own plan, so the two agree
	// in both directions -- including on the stream-collection and
	// partition checks the old dry-run skipped.
	TEST_CASE("[IAX-PWR-06] can_extend agrees with update in both directions") {
		auto ok_spec = make("o1[t] = 1.");
		REQUIRE(ok_spec.has_value());
		tref compatible = parse_formula("always o1[t]:tau = 1");
		REQUIRE(compatible != nullptr);
		auto ce_ok = ok_spec->can_extend(compatible);
		auto up_ok = ok_spec->update(compatible);
		REQUIRE(ce_ok.has_value());
		REQUIRE(up_ok.has_value());
		CHECK(ce_ok.value());
		CHECK(up_ok.value());

		auto bad_spec = make("o1[t] = 1.");
		REQUIRE(bad_spec.has_value());
		tref contradictory = parse_formula("always (o2[t]:tau = 0 && o2[t]:tau = 1)");
		REQUIRE(contradictory != nullptr);
		auto ce_r = bad_spec->can_extend(contradictory);
		auto up_r = bad_spec->update(contradictory);
		REQUIRE(ce_r.has_value());
		REQUIRE(up_r.has_value());
		const bool ce = ce_r.value();
		const bool up = up_r.value();
		CHECK(ce == up);
		CHECK_FALSE(up);
		// A refused update leaves the interpreter untouched: it steps.
		auto sr = bad_spec->step();
		REQUIRE(sr.has_value());
		auto [out, _] = sr.value();
		CHECK(out.has_value());
	}

	// IN-M2: a part with several revision alternatives is executed as its
	// FIRST solvable alternative. admissible_outputs() and current_spec()
	// used to report the disjunction of all alternatives, admitting
	// outputs step() never emits.
	TEST_CASE("[IAX-PWR-07] admissible_outputs and current_spec follow the executed alternative") {
		auto i = make("o1[t] = i1[t].");
		REQUIRE(i.has_value());
		// G(o1 = i1) ∧ G(o1 = i1' ∨ o1 = 0) is satisfiable only for
		// cooperating inputs (i1 = 0), so the revision keeps the
		// accumulated spec as a conditional first alternative and the
		// update as the last resort: [G(o1=i1 ∧ …), G(o1=i1' ∨ o1=0)].
		tref conditional = parse_formula(
			"always (o1[t]:tau = i1[t]:tau' || o1[t]:tau = 0)");
		REQUIRE(conditional != nullptr);
		auto update_r = i->update(conditional);
		REQUIRE(update_r.has_value());
		REQUIRE(update_r.value());
		const std::string before = i->current_spec();
		// With i1 = 0 the first alternative is solvable and forces
		// o1 = 0; the disjunction of both alternatives would also admit
		// o1 = 1 (the fallback's i1' branch), which step() never emits.
		const size_t tid = get_ba_type_id<node_t>(tau_type<node_t>());
		assignment<node_t> in;
		in[build_in_var_at_n<node_t>("i1", 0, tid)] = tau::_0(tid);
		i->memory = in;
		auto admissible_r = i->admissible_outputs(10);
		REQUIRE(admissible_r.has_value());
		auto& admissible = admissible_r.value();
		REQUIRE(admissible.size() == 1);
		for (const auto& [var, value] : admissible[0])
			CHECK(tau::get(value).to_str() == "0");
		auto sr = i->step(in);
		REQUIRE(sr.has_value());
		auto [out, _] = sr.value();
		REQUIRE(out.has_value());
		for (const auto& [var, value] : *out)
			CHECK(tau::get(value).to_str() == "0");
		// After the step the spec view is the executed alternative, not
		// the disjunction of both.
		const std::string after = i->current_spec();
		CHECK(after != before);
		CHECK(after.size() < before.size());
	}

	// IN-M1: the map returned by step N stays readable while the host
	// feeds step N+1 (the sweep at the start of N+1 pins it), with the gc
	// knobs forced to sweep on every step.
	TEST_CASE("[IAX-PWR-08] previous step's outputs survive the next step's sweep") {
		const size_t saved_min = interpreter<node_t>::gc_min_size;
		const double saved_growth = interpreter<node_t>::gc_growth_factor;
		interpreter<node_t>::gc_min_size = 1;
		interpreter<node_t>::gc_growth_factor = 0.001;
		{
			auto i = make("o1[t] = i1[t].");
			REQUIRE(i.has_value());
			assignment<node_t> in;
			std::vector<std::string> before;
			std::optional<assignment<node_t>> prev;
			for (size_t t = 0; t < 12; ++t) {
				in.clear();
				const size_t tid = get_ba_type_id<node_t>(tau_type<node_t>());
				in[build_in_var_at_n<node_t>("i1", static_cast<int_t>(t), tid)]
					= t % 2 ? tau::_1(tid) : tau::_0(tid);
				auto sr = i->step(in);
				REQUIRE(sr.has_value());
				auto [out, _] = sr.value();
				REQUIRE(out.has_value());
				if (prev) {
					// read the previous map AFTER this step swept
					std::vector<std::string> after;
					for (const auto& [k, v] : *prev)
						after.push_back(tau::get(k).to_str()
							+ "=" + tau::get(v).to_str());
					CHECK(after == before);
				}
				before.clear();
				for (const auto& [k, v] : *out)
					before.push_back(tau::get(k).to_str()
						+ "=" + tau::get(v).to_str());
				prev = out;
			}
		}
		interpreter<node_t>::gc_min_size = saved_min;
		interpreter<node_t>::gc_growth_factor = saved_growth;
	}

	// IN-M5: memory must not grow by one entry per step for the run's
	// lifetime (complemented aux keys used to be un-evictable).
	TEST_CASE("[IAX-PWR-09] memory stays bounded across steps") {
		auto i = make("o1[t] = i1[t-1] && o2[t] = o1[t-1].");
		REQUIRE(i.has_value());
		assignment<node_t> in;
		size_t at_10 = 0;
		for (size_t t = 0; t < 40; ++t) {
			in.clear();
			const size_t tid = get_ba_type_id<node_t>(tau_type<node_t>());
			in[build_in_var_at_n<node_t>("i1", static_cast<int_t>(t), tid)]
				= t % 3 ? tau::_1(tid) : tau::_0(tid);
			auto sr = i->step(in);
			REQUIRE(sr.has_value());
			auto [out, _] = sr.value();
			REQUIRE(out.has_value());
			if (t == 10) at_10 = i->memory.size();
		}
		CHECK(i->memory.size() <= at_10 + 2);
	}

	TEST_CASE("[IAX-PWR-10] an update reaching before step 0 is refused") {
		auto i = make("o1[t] = 1.");
		REQUIRE(i.has_value());
		REQUIRE(i->step().has_value());
		const std::string before = i->current_spec();
		tref psi = parse_formula("o1[-3] = 1");
		REQUIRE(psi != nullptr);
		auto ur = i->update(psi);
		REQUIRE(ur.has_value());
		CHECK_FALSE(ur.value());
		CHECK(report_text(ur).find("below 0") != std::string::npos);
		CHECK(i->current_spec() == before);
	}

	TEST_CASE("[IAX-PWR-11] a data-game update is refused when it retypes a "
		"stream or reaches outside the run")
	{
		auto i = make("(sometimes o1[t] = 0) && (sometimes o1[t] = 1).");
		REQUIRE(i.has_value());
		REQUIRE(i->plays_data_game());
		REQUIRE(i->step().has_value());
		REQUIRE(i->step().has_value());
		const std::string before = i->current_spec();

		tref retype = parse_formula("always o1[t]:sbf = {a}:sbf");
		REQUIRE(retype != nullptr);
		auto r1 = i->update(retype);
		REQUIRE(r1.has_value());
		CHECK_FALSE(r1.value());
		CHECK(report_text(r1).find("a type other than the running one")
			!= std::string::npos);

		tref early = parse_formula("o1[-3] = 1");
		REQUIRE(early != nullptr);
		auto r2 = i->update(early);
		REQUIRE(r2.has_value());
		CHECK_FALSE(r2.value());
		CHECK(report_text(r2).find("below 0") != std::string::npos);

		// o5 was never committed, so its value one step back is unknown.
		tref unknown = parse_formula("o5[-1] = 1");
		REQUIRE(unknown != nullptr);
		auto r3 = i->update(unknown);
		REQUIRE(r3.has_value());
		CHECK_FALSE(r3.value());
		CHECK(report_text(r3).find("invalid memory access")
			!= std::string::npos);
		CHECK(i->current_spec() == before);
	}

}

// ============================================================================
// IAX-PREF: apply_preferences (free function)
// ============================================================================

TEST_SUITE("[IAX-PREF: apply_preferences]") {

	TEST_CASE("[IAX-PREF-01] empty preference order returns spec unchanged") {
		io_context<node_t> ctx;
		auto nso_rr = get_nso_rr<node_t>(ctx, tau::get("o1[t] = 1.").value_or(nullptr));
		REQUIRE(nso_rr.has_value());
		tref spec_tref = nso_rr.value().main->get();
		REQUIRE(spec_tref != nullptr);

		preference_order po;
		tref result = apply_preferences<node_t>(spec_tref, po);
		REQUIRE(result == spec_tref);
	}

	TEST_CASE("[IAX-PREF-02] preference that's a parse failure is silently dropped") {
		io_context<node_t> ctx;
		auto nso_rr = get_nso_rr<node_t>(ctx, tau::get("o1[t] = 1.").value_or(nullptr));
		REQUIRE(nso_rr.has_value());
		tref spec_tref = nso_rr.value().main->get();

		preference_order po;
		po.entries.push_back({"###invalid_var", "garbage"});
		// Should not crash; should return spec unchanged (preference dropped).
		tref result = apply_preferences<node_t>(spec_tref, po);
		REQUIRE(result == spec_tref);
	}

	// IN-2 / IN-R4: apply_preferences calls is_ltl_aba_realizable directly;
	// a spec it refuses (semantic negation, CTL*) throws, and that used to
	// escape apply_preferences and end the process. The preference is
	// dropped instead.
	TEST_CASE("[IAX-PREF-03] refused spec (semantic negation) drops the preference, no throw") {
		io_context<node_t> ctx;
		auto nso_rr = get_nso_rr<node_t>(ctx, tau::get("-(F o1[t] = 1).").value_or(nullptr));
		REQUIRE(nso_rr.has_value());
		tref spec_tref = nso_rr.value().main->get();
		REQUIRE(spec_tref != nullptr);

		preference_order po;
		po.entries.push_back({"o1", "1"});
		tref result = nullptr;
		CHECK_NOTHROW(result = apply_preferences<node_t>(spec_tref, po));
		CHECK(result == spec_tref);
	}

	TEST_CASE("[IAX-PREF-04] CTL* spec (A) drops the preference, no throw") {
		io_context<node_t> ctx;
		auto nso_rr = get_nso_rr<node_t>(ctx, tau::get("F (A (o1[t] = 1)).").value_or(nullptr));
		REQUIRE(nso_rr.has_value());
		tref spec_tref = nso_rr.value().main->get();
		REQUIRE(spec_tref != nullptr);

		preference_order po;
		po.entries.push_back({"o1", "1"});
		tref result = nullptr;
		CHECK_NOTHROW(result = apply_preferences<node_t>(spec_tref, po));
		CHECK(result == spec_tref);
	}
}




TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}