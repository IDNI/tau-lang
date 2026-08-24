// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Parity tests for table_step_provider: build a strategy + baked atoms
// in-process from a real solve_ltl_aba solution (no `tau compile`, no
// emitted C++), drive it through make_table_interpreter, and check its
// per-step outputs against the ordinary solve-provider interpreter on the
// same spec and the same input sequence.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "table_step_provider.h"
#include "cpp_codegen.h"
#include "ltl_aba.h"

#include <map>
#include <memory>

using namespace idni::tau_lang;

namespace {

std::string carrier_type_str() {
	return get_ba_type_name<node_t>(
		get_ba_type_id<node_t>(pack_bool_carrier_type<node_t>()));
}

// Parse `spec` (a full ". "-terminated tau spec string) against `ctx` --
// resolves io_vars to the same node identity ctx's registered streams use,
// which raw api<node_t>::get_formula() does not (a bare reparse leaves the
// interpreter unable to match a formula's io_var against ctx.inputs/outputs
// for anything beyond a single output-only spec).
tref parse_against(io_context<node_t>& ctx, const std::string& spec) {
	auto nso = get_nso_rr<node_t>(ctx, tau::get(spec));
	if (!nso.has_value()) return nullptr;
	return nso.value().main->get();
}

// Run `fm` through the ordinary (solve-provider) interpreter for `steps`
// steps against `ctx` (already carrying the input/output streams), and
// return the values written to "o1".
strings run_solve_o1(tref fm, io_context<node_t>& ctx,
	std::shared_ptr<vector_output_stream> o1, size_t steps)
{
	run<node_t>(fm, ctx, steps);
	return o1->get_values();
}

// Same as run_solve_o1, but through a table-mode interpreter driven by
// `provider` instead of re-solving.
strings run_table_o1(std::shared_ptr<step_provider<node_t>> provider,
	io_context<node_t>& ctx, std::shared_ptr<vector_output_stream> o1,
	size_t steps, int_t lookback, int_t highest_initial_pos)
{
	auto interp = interpreter<node_t>::make_table_interpreter(
		ctx, std::move(provider), lookback, highest_initial_pos);
	if (!interp.has_value()) return {};
	// api<node_t>::step() is the public wrapper around step()+write() --
	// write() itself stays private to interpreter, reached only through it.
	for (size_t k = 0; k < steps; ++k) api<node_t>::step(*interp);
	return o1->get_values();
}

// Parse a single relative-time atom template (e.g. "o1[t]:tau != {T.}:tau.")
// the same shape edge_witness_templates stores: unresolved `t`, no ctx
// needed since ocltl_direct_decode_edge only ever grounds via
// update_to_time_point + rewriter::replace on the raw tref.
tref parse_tmpl_atom(const std::string& src) {
	return get_nso_rr<node_t>(tau::get(src)).value().main->get();
}

// A grounded template atom holds after substituting `sol` in for its free
// vars: either trivially true, or satisfiable once any remaining witness
// (bare uconst) is existentially closed -- same check check_solution uses.
bool atom_holds(tref tmpl, const assignment<node_t>& memory,
	const solution<node_t>& sol, int_t formula_time_point = 0)
{
	tref updated = update_to_time_point<node_t>(tmpl, formula_time_point);
	tref grounded = rewriter::replace<node_t>(updated, memory);
	tref substituted = rewriter::replace<node_t>(grounded, sol);
	tref check = normalizer<node_t>(substituted);
	if (tree<node_t>::get(check).equals_T()) return true;
	return is_non_temp_nso_satisfiable<node_t>(check);
}

} // namespace

TEST_SUITE("table_step_provider") {

	TEST_CASE("propositional spec: table provider matches the solve provider") {
		bdd_init<Bool>();
		std::string ct = carrier_type_str();
		size_t carrier_tid = get_ba_type_id<node_t>(pack_bool_carrier_type<node_t>());
		strings i1_vals = {"1", "0", "1", "1", "0"};
		std::string spec = "G(o1[t]" + ct + " = {1}" + ct
			+ " <-> i1[t]" + ct + " = {1}" + ct + ").";

		io_context<node_t> solve_ctx;
		solve_ctx.add_input("i1", carrier_tid, std::make_shared<vector_input_stream>(i1_vals));
		auto solve_o1 = std::make_shared<vector_output_stream>();
		solve_ctx.add_output("o1", carrier_tid, solve_o1);
		tref fm = parse_against(solve_ctx, spec);
		REQUIRE(fm != nullptr);

		auto sol = solve_ltl_aba<node_t>(fm);
		if (!sol) { MESSAGE("UNREALIZABLE; skip"); return; }
		auto [provider, bounds] = make_table_provider<node_t>(*sol);
		REQUIRE(provider != nullptr);
		CHECK(bounds.first == 0);   // no lookback in this spec

		auto solve_vals = run_solve_o1(fm, solve_ctx, solve_o1, 5);

		io_context<node_t> table_ctx;
		table_ctx.add_input("i1", carrier_tid, std::make_shared<vector_input_stream>(i1_vals));
		auto table_o1 = std::make_shared<vector_output_stream>();
		table_ctx.add_output("o1", carrier_tid, table_o1);
		auto table_vals = run_table_o1(provider, table_ctx, table_o1, 5,
			bounds.first, bounds.second);

		REQUIRE(solve_vals.size() == 5);
		CHECK(table_vals == solve_vals);
	}

#ifdef TAU_PACK_HAS_BA_BV
	TEST_CASE("data-atom spec (bv): table provider matches the solve provider "
	          "on a real (non-carrier) guard atom")
	{
		bdd_init<Bool>();
		std::string ct = carrier_type_str();
		size_t carrier_tid = get_ba_type_id<node_t>(pack_bool_carrier_type<node_t>());
		size_t bv4_tid = bv_type_id<node_t>(4);
		strings i1_vals = {"2", "7", "4", "9", "0"};
		std::string spec = "G(o1[t]" + ct + " = {1}" + ct
			+ " <-> i1[t]:bv[4] < {5}:bv[4]).";

		io_context<node_t> solve_ctx;
		solve_ctx.add_input("i1", bv4_tid, std::make_shared<vector_input_stream>(i1_vals));
		auto solve_o1 = std::make_shared<vector_output_stream>();
		solve_ctx.add_output("o1", carrier_tid, solve_o1);
		tref fm = parse_against(solve_ctx, spec);
		REQUIRE(fm != nullptr);

		auto sol = solve_ltl_aba<node_t>(fm);
		if (!sol) { MESSAGE("UNREALIZABLE; skip"); return; }
		auto [provider, bounds] = make_table_provider<node_t>(*sol);
		REQUIRE(provider != nullptr);

		auto solve_vals = run_solve_o1(fm, solve_ctx, solve_o1, 5);

		io_context<node_t> table_ctx;
		table_ctx.add_input("i1", bv4_tid, std::make_shared<vector_input_stream>(i1_vals));
		auto table_o1 = std::make_shared<vector_output_stream>();
		table_ctx.add_output("o1", carrier_tid, table_o1);
		auto table_vals = run_table_o1(provider, table_ctx, table_o1, 5,
			bounds.first, bounds.second);

		REQUIRE(solve_vals.size() == 5);
		CHECK(table_vals == solve_vals);
	}

	// Echo: the output's value IS the step's input, so no bakeable constant
	// exists (edge_witnesses cannot express it). make_table_provider routes
	// the data-typed output atom as a per-edge witness template; the
	// provider grounds it against the committed input and solves for o1
	// each step.
	TEST_CASE("input-dependent witness (bv echo): template output follows "
	          "the input, matching the solve provider")
	{
		bdd_init<Bool>();
		size_t bv4_tid = bv_type_id<node_t>(4);
		strings i1_vals = {"2", "7", "4", "9", "0"};
		std::string spec = "G(o1[t]:bv[4] = i1[t]:bv[4]).";

		io_context<node_t> solve_ctx;
		solve_ctx.add_input("i1", bv4_tid, std::make_shared<vector_input_stream>(i1_vals));
		auto solve_o1 = std::make_shared<vector_output_stream>();
		solve_ctx.add_output("o1", bv4_tid, solve_o1);
		tref fm = parse_against(solve_ctx, spec);
		REQUIRE(fm != nullptr);

		auto sol = solve_ltl_aba<node_t>(fm);
		if (!sol) { MESSAGE("UNREALIZABLE; skip"); return; }
		REQUIRE(sol->atoms.size() == 1);
		auto [provider, bounds] = make_table_provider<node_t>(*sol);
		REQUIRE(provider != nullptr);

		auto solve_vals = run_solve_o1(fm, solve_ctx, solve_o1, 5);

		io_context<node_t> table_ctx;
		table_ctx.add_input("i1", bv4_tid, std::make_shared<vector_input_stream>(i1_vals));
		auto table_o1 = std::make_shared<vector_output_stream>();
		table_ctx.add_output("o1", bv4_tid, table_o1);
		auto table_vals = run_table_o1(provider, table_ctx, table_o1, 5,
			bounds.first, bounds.second);

		REQUIRE(solve_vals.size() == 5);
		CHECK(table_vals == solve_vals);
		// The echo itself: the emitted value is each step's own input.
		CHECK(table_vals == i1_vals);
	}
#endif

	// Lookback shifts memory reads through the solve path's own catch-up
	// window (formula_time_point starts ahead of time_point by `lookback`);
	// during that window the solve provider existentially quantifies the
	// not-yet-reached output away (output defaults to zero), while the
	// table provider deterministically evaluates the guard from whatever
	// memory already holds. Both are well-defined; they only have to agree
	// once real history exists, i.e. from step `lookback` onward -- the
	// same convention test_ltl_strategy_exec.cpp's own lookback tests use.
	TEST_CASE("lookback-bearing spec: table provider matches the solve "
	          "provider from step `lookback` onward")
	{
		bdd_init<Bool>();
		std::string ct = carrier_type_str();
		size_t carrier_tid = get_ba_type_id<node_t>(pack_bool_carrier_type<node_t>());
		strings i1_vals = {"1", "0", "1", "1", "0", "0"};
		const size_t steps = 6;
		std::string spec = "G(o1[t]" + ct + " = {1}" + ct
			+ " <-> i1[t-1]" + ct + " = {1}" + ct + ").";

		io_context<node_t> solve_ctx;
		solve_ctx.add_input("i1", carrier_tid, std::make_shared<vector_input_stream>(i1_vals));
		auto solve_o1 = std::make_shared<vector_output_stream>();
		solve_ctx.add_output("o1", carrier_tid, solve_o1);
		tref fm = parse_against(solve_ctx, spec);
		REQUIRE(fm != nullptr);

		auto sol = solve_ltl_aba<node_t>(fm);
		if (!sol) { MESSAGE("UNREALIZABLE; skip"); return; }
		auto [provider, bounds] = make_table_provider<node_t>(*sol);
		REQUIRE(provider != nullptr);
		REQUIRE(bounds.first == 1);  // one step of lookback baked

		auto solve_vals = run_solve_o1(fm, solve_ctx, solve_o1, steps);

		io_context<node_t> table_ctx;
		table_ctx.add_input("i1", carrier_tid, std::make_shared<vector_input_stream>(i1_vals));
		auto table_o1 = std::make_shared<vector_output_stream>();
		table_ctx.add_output("o1", carrier_tid, table_o1);
		auto table_vals = run_table_o1(provider, table_ctx, table_o1, steps,
			bounds.first, bounds.second);

		REQUIRE(solve_vals.size() == steps);
		REQUIRE(table_vals.size() == steps);
		for (size_t k = (size_t)bounds.first; k < steps; ++k)
			CHECK(table_vals[k] == solve_vals[k]);
	}

#ifdef TAU_PACK_HAS_BA_QLT
	// Exercises the witness/tref-factory path: edge_witnesses supplies a
	// precomputed value tref directly (the same ground constant the atom
	// itself carries), sidestepping codegen's own witness-picking (which
	// only ever surfaces as a C++ source string meant to be compiled).
	TEST_CASE("data-atom spec (qlt): witness output reaches the real stream") {
		bdd_init<Bool>();
		std::string spec = "G(o1[t]:qlt = {1/2}:qlt).";

		io_context<node_t> solve_ctx;
		auto solve_o1 = std::make_shared<vector_output_stream>();
		solve_ctx.add_output("o1", qlt_type_id<node_t>(), solve_o1);
		tref fm = parse_against(solve_ctx, spec);
		REQUIRE(fm != nullptr);

		auto sol = solve_ltl_aba<node_t>(fm);
		if (!sol) { MESSAGE("UNREALIZABLE; skip"); return; }
		REQUIRE(sol->atoms.size() == 1);
		REQUIRE(sol->aut.num_states >= 1);

		// Pull the atom's own ground constant operand -- no witness-picking
		// needed, an equality atom already carries the value it asserts.
		tref atom_ref = sol->atoms[0].first;
		const auto& t = tree<node_t>::get(atom_ref);
		REQUIRE(t.has_child());
		// Check via trim() (peeks through the bf wrapper) but keep `value`
		// itself untrimmed -- write()/serialize_constant expect the same
		// bf-wrapped shape build_bf_ba_constant produces, not the bare
		// ba_constant trim() would unwrap down to.
		tref lhs = t[0].first(), rhs = t[0].second();
		tref value = tree<node_t>::get(tau::trim(lhs)).is_ba_constant() ? lhs : rhs;
		REQUIRE(tree<node_t>::get(tau::trim(value)).is_ba_constant());

		codegen::strategy strat;
		strat.num_states = sol->aut.num_states;
		strat.initial_state = sol->aut.initial_state;
		strat.num_inputs = 0;
		strat.edges.resize(sol->aut.num_states);
		std::vector<std::vector<std::vector<std::pair<std::string, tref>>>>
			edge_witnesses(sol->aut.num_states);
		for (int s = 0; s < sol->aut.num_states; ++s) {
			edge_witnesses[s].resize(sol->aut.edges.size() > (size_t)s
				? sol->aut.edges[s].size() : 0);
			size_t ei = 0;
			for (auto& e : sol->aut.edges[s]) {
				strat.edges[s].push_back({{}, e.dst});
				edge_witnesses[s][ei].emplace_back("o1", value);
				++ei;
			}
		}

		auto provider = std::make_shared<table_step_provider<node_t>>(
			std::move(strat), std::vector<std::pair<std::string, tref>>{},
			std::vector<std::string>{}, std::move(edge_witnesses));

		auto solve_vals = run_solve_o1(fm, solve_ctx, solve_o1, 3);

		io_context<node_t> table_ctx;
		auto table_o1 = std::make_shared<vector_output_stream>();
		table_ctx.add_output("o1", qlt_type_id<node_t>(), table_o1);
		auto table_vals = run_table_o1(provider, table_ctx, table_o1, 3, 0, 0);

		REQUIRE(solve_vals.size() == 3);
		CHECK(table_vals == solve_vals);
	}
#endif

	// Regression for the ocltl_direct_decode_edge solution-key mismatch:
	// solve_inequality_system_atomless returns keys shaped like its own
	// `vars` extraction (bf-wrapped variable), so a bare-variable lookup
	// must not miss and silently substitute a ground zero for a constrained
	// coordinate (o1 here), which would corrupt the deferred mutual atom
	// o1!=o2 on o2's turn. Must find o1's real witness and return a
	// solution where every atom holds.
	TEST_CASE("ocltl_direct_decode_edge: a solved coordinate's witness is "
	          "found and substituted into a deferred mutual atom") {
		bdd_init<Bool>();
		trefs tmpls = {
			parse_tmpl_atom("o1[t]:tau != {T.}:tau."),
			parse_tmpl_atom("o1[t]:tau != {F.}:tau."),
			parse_tmpl_atom("o1[t]:tau != o2[t]:tau."),
		};
		assignment<node_t> memory;
		fresh_element_ledger ledger;
		auto sol = ocltl_direct_decode_edge<node_t>(tmpls, memory, 0, 0, ledger);
		REQUIRE(sol.has_value());
		for (tref tmpl : tmpls)
			CHECK(atom_holds(tmpl, memory, sol.value()));
	}

	// Cross-step contract: round 2 is decided with round 1's commit already
	// in memory, and its witness for the coordinate referencing round 1's
	// value (o1[t-1]) semantically avoids it -- same ledger across rounds.
	TEST_CASE("ocltl_direct_decode_edge: round 2 avoids round 1's committed "
	          "witness") {
		bdd_init<Bool>();
		trefs tmpls1 = {
			parse_tmpl_atom("o1[t]:tau != {T.}:tau."),
			parse_tmpl_atom("o1[t]:tau != {F.}:tau."),
		};
		assignment<node_t> memory;
		fresh_element_ledger ledger;
		auto sol1 = ocltl_direct_decode_edge<node_t>(
			tmpls1, memory, 0, 0, ledger);
		REQUIRE(sol1.has_value());
		for (tref tmpl : tmpls1) CHECK(atom_holds(tmpl, memory, sol1.value()));

		assignment<node_t> memory2 = sol1.value(); // round 1's commit
		trefs tmpls2 = {
			parse_tmpl_atom("o1[t]:tau != {T.}:tau."),
			parse_tmpl_atom("o1[t]:tau != {F.}:tau."),
			parse_tmpl_atom("o1[t]:tau != o1[t-1]:tau."),
		};
		auto sol2 = ocltl_direct_decode_edge<node_t>(
			tmpls2, memory2, 1, 1, ledger);
		REQUIRE(sol2.has_value());
		for (tref tmpl : tmpls2)
			CHECK(atom_holds(tmpl, memory2, sol2.value(), 1));
	}

	TEST_CASE("cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
