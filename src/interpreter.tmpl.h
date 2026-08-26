// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <algorithm>
#include <set>
#include <sstream>
#include <chrono>
#include <string>
#ifdef __linux__
#include <malloc.h>
#endif

#include "ba_types.h"
#include "pointwise_revision.h"
#include "tau_bdd.h"
#include "utility/term.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "interpreter"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// interpreter IO (read/write and rebuild_inputs/rebuild_outputs)
// -----------------------------------------------------------------------------

template <NodeType node>
std::pair<std::optional<assignment<node>>, bool> interpreter<node>::read(
	const trefs& in_vars, size_t time_step)
{
	DBG(LOG_TRACE   << "read begin\n"
			<< "read[time_point]: " << time_point << "\n"
			<< "read[in_vars]: { ";
		for (auto& var : in_vars) LOG_TRACE << LOG_FM_DUMP(var) << ", ";
		LOG_TRACE << "}\n";)

	trefs sorted_in_vars = in_vars;
	std::ranges::sort(sorted_in_vars, constant_io_comp<node>);
	// // For formatting, get the maximal length of an output stream name with type information
	// int_t max_length = 0;
	// for (tref var : in_vars)
	// 	max_length = std::max(max_length, (int_t)TAU_TO_STR(var).length());
	assignment<node> value;
	for (tref var : sorted_in_vars) {
		tref vn = canonize<node>(var);

		DBG(LOG_TRACE << "read[var]: " << LOG_FM_DUMP(var) << "\n";)
		DBG(LOG_TRACE << "read[vn]: " << LOG_FM_DUMP(vn) << "\n";)

		// Skip output stream variables
		if (tau::get(var).is_output_variable())
			continue;
		// Skip input stream variables with time point greater time_step
		if (get_io_time_point<node>(tau::trim(var)) > (int_t)time_step)
			continue;

		auto it = inputs.find(vn);
		if (it == inputs.end())  {
			LOG_ERROR
				<< "Failed to find input stream for stream '"
				<< get_var_name<node>(var) << "'\n";
			DBG(LOG_TRACE << "read[result]: {}\n"
				<< "read end\n";)
			DBG(LOG_TRACE << ctx;)
			DBG(LOG_TRACE << dump_to_str());
			return {};
		}
		// AP2-7: query with the parameter, not the member -- the only
		// caller passes time_point today, but a future lookback pre-read
		// with time_step != time_point would read the wrong step.
		auto maybe_line = it->second->get(time_step); // get a value from input stream
		if (!maybe_line.has_value()) {
			LOG_ERROR
				<< "Failed to read from input stream '"
				<< get_var_name<node>(var) << "'\n";
			DBG(LOG_TRACE << "read[result]: {}\n"
				<< "read end\n";)
		       return {};
		}
		std::string line = maybe_line.value();

		if (line.empty()) return { value, true }; // no more inputs
		size_t type = ctx.type_of(vn);
		if (type == 0) {
			LOG_ERROR << "Failed to find type for "
				  << get_var_name<node>(var);

			DBG(LOG_TRACE << "read[result]: {}\n"
				      << "read end\n";)

			return {};
		}
		auto cnst = ba_constants<node>::get(line,
				get_ba_type_tree<node>(type));
		if (!cnst) {
			LOG_ERROR
				<< "Failed to parse input value '"
				<< line << "' for stream '"
				<< get_var_name<node>(var)
				<< get_ba_type_name<node>(type) << "'";
			DBG(LOG_TRACE
				<< "read[result]: {}\n"
				<< "read end";)

			return {};
		}

		tref wrapped_const = build_bf_ba_constant<node>(
			cnst.value().first, type);

		DBG(LOG_TRACE << "read[wrapped_const]: " << LOG_FM(wrapped_const) << "\n";)

		// Check that the input is a closed formula
		if (has_open_tau_fm_in_constant<node>(wrapped_const)) {

			DBG(LOG_TRACE
				<< "read[result]: {}\n"
				<< "read end\n";)
			return {};
		}
		value[var] = wrapped_const;
	}

	DBG(LOG_TRACE << "read end\n";)

	return { value, false };
}

template <NodeType node>
bool interpreter<node>::write(const assignment<node>& output_values) {
	// Sort variables in output by time
	trefs io_vars;
	for (const auto& [var, _ ] : output_values) {
		// DBG(LOG_TRACE << "io var: " << LOG_FM_TREE(var));
		// DBG(LOG_TRACE << "io var dump: " << LOG_FM_DUMP(var));
		// The write loop below already skips the LTL aux state bits
		// (o__ltl_s*, o__ltl_ms*) via is_excluded_output -- they are
		// internal encoding artefacts, not user-visible streams, and no
		// output stream is registered for them. Skip them here instead, ie.
		// before the sort AND before the shape assert: a solution can key
		// an aux bit by its COMPLEMENT (o__ltl_s0__[t]', bf_neg-wrapped,
		// seen on the qlt S-operator execution), a node shape neither
		// get_var_name nor constant_io_comp can read, so both the assert
		// and the sort would die long before the skip below was reached.
		// Unwrap complements for the exclusion check only; a genuinely
		// mis-shaped non-excluded key still hits the assert.
		tref base = var;
		while (tau::get(base).has_child()
			&& tau::get(base)[0].is(tau::bf_neg))
			base = tau::get(base)[0].first();
		if (is_excluded_output(base)) continue;
		DBG(if (!tau::get(var)[0].child_is(tau::io_var))
			LOG_ERROR << "write: non-io key: " << TAU_TO_STR(var)
				<< "\n" << LOG_FM_DUMP(var) << "\n";)
		assert(tau::get(var)[0].child_is(tau::io_var));
		io_vars.push_back(var);
	}
	std::ranges::sort(io_vars, constant_io_comp<node>);

	// // For formatting, get the maximal length of an output stream name with type information
	// int_t max_length = 0;
	// for (tref io_var : io_vars)
	// 	max_length = std::max(max_length, (int_t)TAU_TO_STR(io_var).length());
	// for each stream in out.streams, write the value from the solution
	for (tref io_var : io_vars) {
		// get the BA element associated with io_var_name
		DBG(LOG_TRACE << "write[io_var]: " << LOG_FM_DUMP(io_var));
		tref vn = canonize<node>(io_var);
		assert(vn != nullptr);
		DBG(LOG_TRACE << "write[canonized]: " << LOG_FM(vn));
		std::stringstream ss;
		if (!serialize_constant<node>(ss, output_values.find(io_var)->second,
			ctx.type_of(vn)))
		{
			LOG_ERROR << "No Boolean algebra element assigned to "
				"output '" << TAU_TO_STR(io_var) << "'";
				return false;
		}
		// Internal streams are never written, even when one was auto-added
		// for them: collect_output_streams attaches a default console
		// stream to every output variable it finds in the spec, which put
		// the LTL state bits in `outputs` and so past the check below.
		if (is_excluded_output(vn)) continue;
		auto it = outputs.find(vn);
		if (it == outputs.end()) {
			// AP2-15: excluded outputs (_e*/_f*) were already filtered
			// before the sort above. Other `_`-prefixed streams are
			// still returned in step()'s assignment but have no
			// registered output stream, so they are silently unwritten
			// here by design.
			if (auto name = get_var_name<node>(vn);
				!name.empty() && name.front() == '_') continue;
			LOG_ERROR << "Failed to find output stream for stream '"
				<< get_var_name<node>(vn) << "'";
			DBG(LOG_TRACE << ctx;)
			DBG(LOG_TRACE << dump_to_str());
			return false;
		}
		// write value to output stream
		DBG(LOG_TRACE << "write/put(serialized_constant): " << ss.str();)
		if (!it->second->put(ss.str(),
			get_io_time_point<node>(tau::trim(io_var))))
		{
			LOG_ERROR << "Failed to write to output stream '"
				<< get_var_name<node>(vn) << "'";
			return false;
		}
	}
	return true; // success
}

template<NodeType node>
bool interpreter<node>::rebuild_inputs(
	const subtree_map<node, size_t>& current_inputs)
{
	// Close all input streams
	inputs.clear();
	return build_inputs(current_inputs, inputs);
}

template<NodeType node>
bool interpreter<node>::build_inputs(
	const subtree_map<node, size_t>& current_inputs,
	input_streams<node>& inputs)
{
	// open the corresponding streams for input and store them in streams
	for (auto& [current_var, stream_id] : current_inputs) {
		DBG(LOG_TRACE << "rebuild_inputs[current_var]: " << LOG_FM_DUMP(current_var) << "\n";)
		tref var = canonize<node>(current_var);
		DBG(LOG_TRACE << "rebuild_inputs[var]: " << LOG_FM(var) << "\n";)
		auto it = ctx.inputs.find(var);
		if (it == ctx.inputs.end()) {
			LOG_ERROR << "Failed to find input stream for stream '"
				<< get_var_name<node>(var) << "'\n";
			DBG(LOG_TRACE << ctx;)
			DBG(LOG_TRACE << dump_to_str());
			return false; // stop interpreting: failed to open an input stream
		}
		std::string vn = get_var_name<node>(var);
		if (auto it = ctx.input_remaps.find(vn); it != ctx.input_remaps.end()) {
			inputs.emplace(var, std::move(it->second->rebuild()));
		} else {
			if (stream_id == 0) inputs.emplace(var,
				ctx.console_input_factory
					? ctx.console_input_factory(vn)
					: std::make_shared<console_prompt_input_stream>(vn));
			else inputs.emplace(var,
				std::make_shared<file_input_stream>(dict(stream_id)));
		}
	}
	return true;
}

template<NodeType node>
bool interpreter<node>::rebuild_outputs(
	const subtree_map<node, size_t>& current_outputs)
{
	// Delete old streams
	outputs.clear();
	return build_outputs(current_outputs, outputs);
}

template<NodeType node>
bool interpreter<node>::build_outputs(
	const subtree_map<node, size_t>& current_outputs,
	output_streams<node>& outputs)
{
	// open the corresponding streams for output and store them in streams
	for (auto& [current_var, stream_id] : current_outputs) {
		tref var = canonize<node>(current_var);
		auto it = ctx.outputs.find(var);
		if (it == ctx.outputs.end()) {
			LOG_ERROR << "Failed to find output stream for stream '"
				<< get_var_name<node>(var) << "' when rebuilding outputs.";
			return false; // stop interpreting: failed to open an output stream
		}
		std::string vn = get_var_name<node>(var);
		if (auto it = ctx.output_remaps.find(vn); it != ctx.output_remaps.end())
			outputs.emplace(var, std::move(it->second->rebuild()));
		else {
			if (stream_id == 0) outputs.emplace(var,
				std::make_shared<console_prompt_output_stream>(vn));
			else outputs.emplace(var,
				std::make_shared<file_output_stream>(dict(stream_id)));
		}
	}
	return true;
}

// -----------------------------------------------------------------------------
// interpreter


template <NodeType node>
interpreter<node>::interpreter(
	std::vector<htrefs>& ubt_ctn, auto& original_spec, auto& output_partition,
	assignment<node>& memory,
	const io_context<node>& ctx)
	: ubt_ctn(std::move(ubt_ctn)), original_spec(std::move(original_spec)),
		memory(std::move(memory)), ctx(ctx),
		output_partition(std::move(output_partition))
{
	compute_lookback_and_initial();
}

template <NodeType node>
std::optional<interpreter<node>>
	interpreter<node>::make_interpreter(tref spec,
		const io_context<node>& ctx)
{
	DBG(LOG_TRACE << "make_interpreter[spec]: " << LOG_FM_DUMP(spec) << "\n";)
	// IN-M9 (found by the IN-RT4 api-level execution tests): CTL* specs
	// reached this point unreduced.  A/E are not full-LTL operators, so
	// `A (always phi)` was classified as a plain G spec, normalised as
	// such and handed to the solver, which refused the A node ("Found
	// clause containing non-equation") and reported a false "unsat".
	// is_tau_formula_sat reduces first; so must execution.  The reducer
	// throws ltl_synthesis_error for placements it cannot encode soundly
	// (LA-N2); the api/REPL callers already catch it.
	//
	// IN-R6: an E reduction introduces witness outputs w_<n>.  They are
	// registered below as internal output streams (reserved prefix `w_`,
	// excluded from printing via is_excluded_output) so classification
	// passes over rebuilt nodes keep treating them as outputs.  A
	// witness-carrying reduction is also FORCE-ROUTED through the LTL
	// pipeline: its shape is `w=1 ∧ G(w=1 → sometimes χ)`, whose
	// sometimes-under-G lies outside the safety pipeline's
	// eventual-variable transform, while the sat path already sends the
	// reduced formula to is_ltl_aba_realizable — routing execution the
	// same way keeps verdict and execution in agreement.
	bool witness_ltl_route = false;
	io_context<node> ctx_with_witnesses;
	if (has_ctl_star_operators<node>(spec)) {
		auto reduction = reduce_ctl_star_to_ltl<node>(spec);
		if (!reduction.ltl_formula) {
			LOG_ERROR << "Tau specification is not executable (CTL* reduction failed)\n";
			return {};
		}
		spec = reduction.ltl_formula;
		if (!reduction.witnesses.empty()) {
			witness_ltl_route = true;
			ctx_with_witnesses = ctx;
			auto& gctx = *definitions<node>::instance()
				.get_io_context();
			for (size_t i = 0; i < reduction.witnesses.size(); ++i) {
				const auto& wname = reduction.witnesses[i];
				const size_t wtype =
					i < reduction.witness_types.size()
					? reduction.witness_types[i]
					: get_ba_type_id<node>(pack_bool_carrier_type<node>());
				ctx_with_witnesses.add_output(wname, wtype,
					std::make_shared<
						vector_output_stream>());
				// The global context too: text-parse helpers
				// (build_state_bit_eq-style) resolve against it, and
				// the w_ prefix falls outside the i/o name
				// heuristic.
				gctx.add_output(wname, wtype,
					std::make_shared<
						vector_output_stream>());
			}
		}
		DBG(LOG_TRACE << "make_interpreter[ctl* reduced]: " << LOG_FM_DUMP(spec) << "\n";)
	}
	const io_context<node>& ctx_eff =
		witness_ltl_route ? ctx_with_witnesses : ctx;
	// Handle G(phi_A) && G(phi_B) with different BA types:
	// the normalizer merges them into G(phi_A && phi_B) which breaks on mixed
	// types.  Normalize each G formula independently then combine.
	if (!has_ltl_operators<node>(spec) && !witness_ltl_route) {
		auto get_g_body = [](tref c) -> tref {
			const auto& ct = tree<node>::get(c);
			if (!ct.has_child()) return nullptr;
			if (ct[0].value.nt == tree<node>::wff_always)
				return tree<node>::trim2(c);
			return nullptr;
		};
		auto get_ba_type_fm = [](tref body) -> size_t {
			size_t found = 0;
			tree<node>::get(body).find_top([&found](tref n) {
				size_t bt = tree<node>::get(n).value.ba_type;
				if (bt != 0) { found = bt; return true; }
				return false;
			});
			return found;
		};
		std::vector<tref> g_parts, other_parts;
		std::function<void(tref)> split_g = [&](tref n) {
			const auto& t = tree<node>::get(n);
			if (t.has_child() && t[0].value.nt == tree<node>::wff_and) {
				split_g(t[0].first());
				split_g(t[0].second());
			} else if (tref body = get_g_body(n); body)
				g_parts.push_back(n);
			else
				other_parts.push_back(n);
		};
		split_g(spec);
		if (g_parts.size() > 1) {
			size_t first_type = get_ba_type_fm(get_g_body(g_parts[0]));
			bool mixed = false;
			for (size_t i = 1; i < g_parts.size(); ++i)
				if (get_ba_type_fm(get_g_body(g_parts[i])) != first_type)
					{ mixed = true; break; }
			if (mixed) {
				tref combined = tau::_T();
				for (tref g : g_parts)
					combined = tau::build_wff_and(combined, normalizer<node>(g));
				for (tref o : other_parts)
					combined = tau::build_wff_and(combined, normalizer<node>(o));
				spec = combined;
				goto post_normalization;
			}
		}
	}
	// Find a satisfiable unbound continuation from spec.
	// Skip normalizer for LTL formulas — it converts wff_F → wff_sometimes,
	// which would make has_ltl_operators return false and bypass ltl_to_safety_formula.
	if (!has_ltl_operators<node>(spec) && !witness_ltl_route)
		spec = normalizer<node>(spec);
post_normalization:
	// Full LTL formulas (F/U/R/W) need a different execution strategy.
	// Convert the realizable LTL spec to an equivalent safety (always) formula
	// that the existing interpreter pipeline can execute step-by-step.
	//
	// We use the `_full` variant so that, when ltlsynt produces a strategy
	// automaton (multi-state or single-state Mealy), we can cache the
	// LtlAbaSolution on the interpreter for downstream introspection
	// (current_state, visualise_mealy_dot, determinise, boundary_traces).
	std::optional<LtlAbaSolution<node>> ltl_sol;
	std::vector<std::string> since_aux_anchor;
	if (has_ltl_operators<node>(spec) || witness_ltl_route) {
		auto [safety_spec, sol_opt, unanchored_aux] =
			ltl_to_safety_formula_full<node>(spec);
		if (!safety_spec) {
			LOG_ERROR << "Tau specification is unsat (not LTL-realizable)\n";
			return {};
		}
		ltl_sol = std::move(sol_opt);
		since_aux_anchor = std::move(unanchored_aux);
		// Normalize the derived safety formula and recurse with it.
		spec = normalizer<node>(safety_spec);
	}
	// For each spec clause, we check if it is executable
	for (tref clause : expression_paths<node>(spec)) {
		union_find_with_sets<decltype(stream_comp), node> output_partition(stream_comp);
		auto spec_partition = create_spec_partition(clause, output_partition);
		std::vector<htrefs> ubt_ctn;
		bool executable = true;
		for (auto& [spec_part, out_rep] : spec_partition) {
			tref clause_t = spec_part->get();
			auto ubd_ctn_part = get_executable_spec(clause_t);
			if (ubd_ctn_part == nullptr) {
				// Need to try next clause
				executable = false; break;
			}
			ubt_ctn.push_back({ tree<node>::geth(ubd_ctn_part) });
		}
		if (!executable) continue;
		// All parts of spec are realizable; each starts with a single
		// alternative (I1).
		std::vector<std::pair<htrefs, htref>> spec_parts;
		spec_parts.reserve(spec_partition.size());
		for (auto& [spec_part, out_rep] : spec_partition)
			spec_parts.emplace_back(htrefs{ spec_part }, out_rep);
		assignment<node> memory;
		auto i = interpreter{ ubt_ctn, spec_parts, output_partition,
			memory, ctx_eff };

		// Cache the LTL synthesis solution (if any) for downstream
		// introspection of the Mealy strategy. Empty for pure-safety /
		// pure-past-LTL specs that bypassed solve_ltl_aba.
		i.cached_solution = ltl_sol;

		// LA-N3: anchor the inner-S auxiliaries of the pure-past
		// compile-away at t = formula_time_point - 1 (S(-1) = false).
		// Kept on the interpreter so reset() can re-seed.
		i.since_aux_anchor_ = since_aux_anchor;
		i.seed_since_aux_bits();

		// For multi-state Mealy strategies we need two things:
		//
		// (1) Pre-populate memory with initial state bit values
		//     so that the first non-auto-continued step sees the
		//     correct lookback values (ms_j[t = formula_time_point-1]).
		//
		// (2) Add the initial output constraint (init_out) as a bare
		//     ubt_ctn entry so that the auto-continued step 0 emits a
		//     valid output for o1[t=0] (instead of the default zero).
		//     This mirrors how S-operator initial conditions work: they
		//     are added as non-G entries in ubt_ctn and are handled by
		//     get_ubt_ctn_at's QE loop, which retains time-0 vars and
		//     existentially eliminates time>0 vars.
		//
		// Note: init_sv (the carrier equation ms_j[t=0]={1}) is
		// intentionally NOT added to ubt_ctn: a raw carrier io_var equation
		// is not handled by solution_with_max_update the way an ordered
		// theory's constraints are, and memory pre-population achieves the
		// same goal safely.
		if constexpr (pack_has_arithmetic_theory_v<node>)
		if (ltl_sol && ltl_sol->aut.num_states > 1
				&& i.formula_time_point >= 1) {
			const int k      = ltl_sol->aut.num_states;
			const int init_s = ltl_sol->aut.initial_state;
			if (init_s >= 0 && init_s < k) {
				std::vector<std::string> sv_names;
				for (int j = 0; j < k; ++j)
					sv_names.push_back(
						"o__ltl_ms" + std::to_string(j) + "__");

				// (1) Memory pre-population — shared with
				// reset(), which must re-seed the same values.
				i.seed_mealy_initial_state();

				// (2) Initial output constraint: encode_mealy_initial_conditions
				// returns init_out = ∨_e (guard_e(t=0) ∧ ms_dst[t=1]=1).
				// After get_ubt_ctn_at's QE, the time-1 state bits are
				// eliminated and only the output constraint at t=0 remains.
				// This forces step 0 to emit a valid initial output.
				auto [init_sv, init_out] =
					encode_mealy_initial_conditions<node>(*ltl_sol, sv_names);
				if (init_out) {
					// One single-alternative continuation part;
					// ubt_ctn holds a htrefs (alternatives) per
					// part since the factored-revision redesign.
					// IN-N11: it needs its matching entry in
					// original_spec -- without one, update()'s
					// size invariant failed (Debug abort on any
					// update of a multi-state Mealy spec) and
					// in Release every later update paired
					// parts with the wrong continuations. No
					// representative: the initial-output
					// constraint is never revised.
					i.ubt_ctn.push_back(htrefs{
						tree<node>::geth(init_out) });
					i.original_spec.emplace_back(htrefs{
						tree<node>::geth(init_out) }, nullptr);
					i.compute_lookback_and_initial();
				}
			}
		}

		// rebuild io streams according to the SELECTED clause -- AP2-5:
		// collecting from the whole spec opened (and truncated) file
		// outputs and prompted console inputs referenced only in
		// rejected clauses. update() already collects per chosen spec.
		subtree_map<node, size_t> output_streams;
		if (!i.collect_output_streams(clause, output_streams)) return {};
		LOG_TRACE << "interpreter::make_interpreter/rebuild_outputs";
		if (!i.rebuild_outputs(output_streams)) return {};
		subtree_map<node, size_t> input_streams;
		if (!i.collect_input_streams(clause, input_streams)) return {};
		LOG_TRACE << "interpreter::make_interpreter/rebuild_inputs";
		if (!i.rebuild_inputs(input_streams)) return {};

		DBG(LOG_TRACE << "interpreter created\n";)
		DBG(LOG_TRACE << i.dump_to_str();)
		// DBG(LOG_TRACE << ctx;)

		return i;
	}
	// Given specification is not realizable
	LOG_ERROR << "Tau specification is unsat\n";
	return {};
}

template <NodeType node>
std::vector<std::pair<htref, htref>>
interpreter<node>::create_spec_partition(tref spec, auto& output_partition) {
	// Get CNF clauses of DNF clause
	trefs clauses = get_cnf_wff_clauses<node>(spec);
	// Split each always statement into conjuncts again
	for (size_t i = 0; i < clauses.size(); ++i) {
		if (tau::get(clauses[i]).child_is(tau::wff_always)) {
			trefs aw_clauses = get_cnf_wff_clauses<node>(
				tau::trim2(clauses[i]));
			DBG(assert(!aw_clauses.empty()));
			clauses[i] = tau::build_wff_always(aw_clauses[0]);
			for (size_t j = 1; j < aw_clauses.size(); ++j) {
				clauses.push_back(
					tau::build_wff_always(aw_clauses[j]));
			}
		}
	}
	// Compute sets of output streams
	std::vector<std::pair<htref, htref>> partition;
	for (tref c : clauses) {
		// Get output streams and uninterpreted constants
		trefs ostreams;
		auto f = [&ostreams](tref n) {
			if (tau::get(n).is(tau::io_var) && is_output_var<node>(n)) {
				ostreams.push_back(tau::trim(n));
				return false;
			} else if (tau::get(n).is(tau::uconst)) {
				ostreams.push_back(n);
				return false;
			}
			return true;
		};
		pre_order<node>(c).visit_unique(f);
		// Save collection of output streams into union find
		if (!ostreams.empty()) output_partition.insert(ostreams.front());
		for (size_t i = 1; i < ostreams.size(); ++i) {
			output_partition.merge(ostreams[i-1], ostreams[i]);
		}
		if (!ostreams.empty())
			partition.emplace_back(tree<node>::geth(c),
				tree<node>::geth(output_partition.find(ostreams.front())));
		else partition.emplace_back(tree<node>::geth(c), htref{});
	}
	for (size_t i = 0; i < partition.size(); ++i) {
		// If no output/uninterpreted constants are present, skip
		if (!partition[i].second) continue;
		for (size_t j = i+1; j < partition.size(); ++j) {
			// If no output/uninterpreted constants are present, skip
			if (!partition[j].second) continue;
			// Check overlap between output streams of clauses
			if (output_partition.connected(partition[i].second->get(), partition[j].second->get())) {
				// If there is an overlap, conjunct the spec parts
				partition[i].first = tree<node>::geth(tau::build_wff_and(
					partition[i].first->get(), partition[j].first->get()));
				partition.erase(partition.begin()+j);
				--j;
			}
		}
		// Unsqueeze always statements again
		partition[i].first = tree<node>::geth(unsqueeze_always(partition[i].first->get()));
	}
	return partition;
}

// The semantic formula of a partitioned spec under I1: the conjunction over
// parts of the disjunction of each part's ordered alternatives. Only used
// for the packed `this` stream, telemetry and semantic checks -- never
// stored, so the alternatives themselves keep growing additively.
// Disjunction of a part's ordered alternatives -- the part's semantic formula.
template <NodeType node>
static tref part_alts_fm(const htrefs& alts) {
	using tau = tree<node>;
	trefs fms;
	fms.reserve(alts.size());
	for (const htref& h : alts) fms.push_back(h->get());
	return tau::build_wff_or(fms);
}

template <NodeType node>
static tref combined_spec_fm(
	const std::vector<std::pair<htrefs, htref>>& parts)
{
	using tau = tree<node>;
	trefs part_fms;
	part_fms.reserve(parts.size());
	for (const auto& [alts, _] : parts)
		part_fms.push_back(part_alts_fm<node>(alts));
	return tau::build_wff_and(part_fms);
}

template <NodeType node>
std::pair<std::optional<assignment<node>>, bool>
	interpreter<node>::step()
{
	// Compute systems for the current step
	if (!calculate_initial_spec()) return {};
	if (announced_step_ != (int_t)time_point) { // announce only once
		LOG_INFO << "Execution step: " << time_point << "\n";
		announced_step_ = (int_t)time_point;
	}
	// Get inputs for this step
	auto [step_inputs, _] = build_inputs_for_step(time_point);
	step_inputs = appear_within_lookback(step_inputs);
	// Get values for inputs which do not exceed time_point
	LOG_TRACE << "interpreter::step/read";
	auto [values, is_quit] = read(step_inputs, time_point);
	DBG(if (values.has_value())
			for (auto [k, v] : values.value())
				LOG_DEBUG << "Input: " << LOG_FM_DUMP(k) << " = " << LOG_FM_TREE(v) << "\n";)
	// Empty input: clean end-of-inputs/quit signal
	if (is_quit) return {};
	// Hard error reading/parsing an input (read() already logged it):
	// stop like the quit case above, not a "successful", auto-continuing
	// empty step -- that made every caller's driver loop treat a read
	// error as ordinary progress and keep looping on it instead of
	// stopping.
	if (!values.has_value()) return {};

	return step(values.value());
}

// LTL state-variable names: Mealy "o__ltl_ms<i>__", S-operator "o__ltl_s<i>__".
template <NodeType node>
static bool is_ltl_state_var_name(const std::string& name) {
	return (name.size() > 9 && name.compare(0, 9, "o__ltl_ms") == 0)
		|| (name.size() > 8 && name.compare(0, 8, "o__ltl_s") == 0);
}

// True if any io_var inside `part` is one of those state variables.
template <NodeType node>
static bool mentions_ltl_state_var(tref part) {
	using tau = tree<node>;
	bool found = false;
	auto f = [&found](tref n) {
		if (found) return false;
		if (tau::get(n).is(tau::io_var)
			&& is_ltl_state_var_name<node>(
				get_var_name<node>(tau::trim(n))))
		{
			found = true;
			return false;
		}
		return true;
	};
	pre_order<node>(part).visit_unique(f);
	return found;
}

template <NodeType node>
std::pair<std::optional<assignment<node>>, bool>
	interpreter<node>::step(const assignment<node>& values)
{
	// Deferred from the previous step's tail -- see the note above its
	// return: sweeping there frees the just-returned output map's nodes.
	// `values` (this step's inputs) is a caller-built local the sweep
	// cannot see through collect_live_refs, so it is pinned explicitly.
	maybe_gc(&values);

	bool auto_continue = false;
	// Save inputs in memory
	for (const auto& [var, value] : values) {
		DBG(LOG_TRACE << "step[var]: " << LOG_FM_DUMP(var);)
		assert(get_io_time_point<node>(tau::trim(var)) <= (int_t)time_point);
		// If there is at least one input, continue automatically in execution
		auto_continue = true;
		memory[var] = value;
	}

	// Engine-side declare_open: A3 v1 scaffolding only.
	//
	// Full engine-side dispatch (build F as tau-syntax string, call
	// handler, validate, inject chosen value as tref into memory) is
	// non-trivial because admissible_outputs() returns tref-keyed
	// assignments, not string-keyed; the projection + serialisation
	// + tau::parse round-trip needs careful integration with the
	// existing solver machinery.
	//
	// V1 (this commit): just enforce re-entrance — set the flag during
	// any handler that the host might call from inside its own dispatch
	// path. Mutating method guards (declare_open, undeclare_open) check
	// in_oracle_handler_ and refuse. The actual per-step dispatch
	// remains in Session._dispatch_open_streams (Phase A2 Python side).
	//
	// V2 (deferred): full engine-side dispatch with tref injection.
	if (!open_handlers_.empty()) {
		DBG(LOG_DEBUG << "[declare_open] step with "
			<< open_handlers_.size() << " open stream(s); "
			<< "host-side dispatch (A2) is authoritative\n";)
	}
	bool has_this_stream = has_this_input_stream();
	DBG(LOG_TRACE << "step/has_this_stream: " << has_this_stream << "\n";)
	// If the "this" input stream is present, write the current spec into it
	if (has_this_stream) {
		tref current_this_stream = build_in_var_at_n<node>(
			"this", time_point, get_ba_type_id<node>(tau_type<node>()));
		// IN-M2: feed back the spec this step will actually follow
		// (first solvable alternative per part), not the disjunction.
		tref wrapped_spec = build_bf_ba_constant<node>(
			node::ba::pack_tau_ba(unsqueeze_always(
				executed_spec_fm(true))),
				get_ba_type_id<node>(tau_type<node>()));
		memory[current_this_stream] = wrapped_spec;
	}

	solution<node> global;
	chosen_alt_.assign(step_spec.size(), 0);

	// Solve all Mealy-state-touching conjuncts jointly, and before any
	// part that might read the resulting state is attempted: the one-hot
	// state choice and the transition rules constraining it live in
	// separate step_spec parts (each with a single alternative -- these
	// are synthesized safety-encoding conjuncts, not pointwise-revision
	// fallbacks, see calculate_initial_spec/update), and solving them
	// independently in step_spec order commits to a state with no
	// backtracking.
	std::vector<bool> state_part_done(step_spec.size(), false);
	{
		trefs state_parts;
		std::vector<size_t> state_idx;
		for (size_t i = 0; i < step_spec.size(); ++i) {
			if (!step_spec[i].empty()
					&& mentions_ltl_state_var<node>(step_spec[i].front())) {
				state_parts.push_back(step_spec[i].front());
				state_idx.push_back(i);
			}
		}
		if (!state_parts.empty()) {
			tref joint = state_parts.size() == 1
				? state_parts.front() : tau::build_wff_and(state_parts);
			bool joint_solved = false;
			for (tref path : expression_paths<node>(joint)) {
				tref updated = update_to_time_point(path, formula_time_point);
				tref current = rewriter::replace<node>(updated, memory);
				current = normalize_non_temp<node>(current);
				auto path_solution = solution_with_max_update(current);
				if (!path_solution) continue;
				joint_solved = true;
				for (const auto& [var, value] : path_solution.value()) {
					if (tt(var) | tau::variable | tau::io_var) {
						assert(tau::get(value).is(tau::bf));
						if (get_io_time_point<node>(tau::trim(var)) <= (int_t)time_point) {
							memory.emplace(var, value);
							if (!is_excluded_output(tau::trim(var)))
								global.emplace(var, value);
						}
					} else memory.emplace(var, value);
				}
				break;
			}
			if (!joint_solved) {
				LOG_ERROR << "Internal error: Tau specification is unexpectedly unsat\n";
				return {};
			}
			for (size_t i : state_idx) state_part_done[i] = true;
		}
	}

	for (size_t part_idx = 0; part_idx < step_spec.size(); ++part_idx) {
		if (state_part_done[part_idx]) continue;
		const trefs& part_alts = step_spec[part_idx];
		// Try the part's alternatives in their preference order; the
		// first one with a solvable path wins. This trial order is what
		// implements the pointwise-revision preference for the
		// accumulated spec over its fallback updates (I1) -- a later
		// alternative only fires when no earlier one is solvable at
		// this time point.
		bool solved = false;
		for (size_t alt_idx = 0; alt_idx < part_alts.size(); ++alt_idx) {
		tref spec_part = part_alts[alt_idx];
		// A state-bit constraint must reach the solver as a constraint. Its
		// atoms are plain Boolean-algebra facts once the carrier is a single
		// bit, and normalization answers those with T -- satisfiable, hence
		// true -- which throws away the very choice the encoding needs
		// committed, leaving the bits to the zero fill.
		const bool state_part = mentions_ltl_state_var<node>(spec_part);
		for (tref path : expression_paths<node>(spec_part)) {
			// rewriting the inputs and inserting them into memory
			tref updated = update_to_time_point(path, formula_time_point);
			// TODO: Check why constant time positions are not being replaced
			tref current = rewriter::replace<node>(updated, memory);
			// Simplify after updating stream variables
			// TODO: Maybe replace by syntactic simp?
			if (!state_part) current = normalize_non_temp<node>(current);
#ifdef DEBUG
			LOG_TRACE << "step/equations: " << LOG_FM(path) << "\n"
				<< "step/updated: " << LOG_FM(updated) << "\n"
				<< "step/current: " << LOG_FM_DUMP(current) << "\n"
				<< "step/memory: ";
			for (const auto& [k, v]: memory)
				LOG_TRACE << "\t" << k << " := " << v << "\n"
					<< "\t\t" << LOG_FM_DUMP(k) << "\n"
					<< "\t\t" << LOG_FM_DUMP(v) << "\n";
#endif // DEBUG
			auto path_solution = solution_with_max_update(current);
#ifdef DEBUG
			if (path_solution) {
				LOG_TRACE << "step/solution: ";
				if (path_solution.value().empty())
					LOG_TRACE << "\t{}";
				else for (const auto& [k, v]: path_solution.value()) {
					LOG_TRACE << "\t" << TAU_TO_STR(k) << " := "
								<< TAU_TO_STR(v) << " ";
					LOG_TRACE << LOG_FM_DUMP(k) << "\n";
					LOG_TRACE << LOG_FM_DUMP(v) << "\n";
				}
				auto substituted = rewriter::replace<node>(
						current, path_solution.value());
				auto check = normalize_non_temp<node>(substituted);
				LOG_TRACE << "step/check: " << LOG_FM(check) << "\n";
			} else {
				LOG_TRACE << "step/solution: no solution\n";
			}
#endif // DEBUG
			if (path_solution) {
				solved = true;
				chosen_alt_[part_idx] = alt_idx;
				for (const auto& [var, value] : path_solution.value()) {
					// Check if we are dealing with a stream variable
					if (tt(var) | tau::variable | tau::io_var) {
						DBG(LOG_TRACE << LOG_FM_TREE(value));
						assert(tau::get(value).is(tau::bf));
						if (get_io_time_point<node>(tau::trim(var)) <= (int_t)time_point) {
							memory.emplace(var, value);
							// Exclude temporary streams in solution
							if (!is_excluded_output(tau::trim(var)))
								global.emplace(var, value);
						}
					} else {
						// Only io-stream keys belong in the
						// step's output map: write() and the
						// api's output serialization read the
						// stream name and time point off the
						// key. A solution can carry auxiliary
						// entries (a plain solver variable, a
						// constant keyed by an inverted
						// equation, a complemented aux bit);
						// those are memory-only.
						memory.emplace(var, value);
						DBG(LOG_ERROR << "step: non-io "
							"solution key kept out of "
							"outputs: " << TAU_TO_STR(var)
							<< "\n";)
					}
				}
				break;
			}
		}
		if (solved) break;
		}
		if (!solved) {
			LOG_ERROR << "Internal error: Tau specification is unexpectedly unsat\n";
			return {};
		}
	}
	// Complete outputs using time_point and current solution
	for (const auto& [o, _] : outputs) {
		const size_t ctype = ctx.type_of(o);
		tref ot = build_out_var_at_n<node>(get_var_name_node<node>(o), time_point, ctype);
		if (auto it = global.find(ot); it == global.end()) {
			auto emit_default_zero = [&]() {
				// The owning BA supplies its own zero; nullptr means no BA in
				// the pack owns this type, so fall back to the generic zero.
				if (tref zero_term = pack_zero_constant<node>(ctype)) {
					memory.emplace(ot, zero_term);
					global.emplace(ot, zero_term);
					return;
				}
				memory.emplace(ot, tau::_0(ctype));
				global.emplace(ot, tau::_0(ctype));
			};
			emit_default_zero();
		}
	}
	if (global.empty()) LOG_INFO << "currently no output is specified";
	DBG(LOG_TRACE << dump_to_str();)
	// update time_point and formula_time_point
	const size_t completed_time_point = time_point;
	if (time_point < formula_time_point) {
		// auto continue until lookback
		auto_continue = true;
		++time_point;
	} else {
		// auto continue until highest initial position
		if ((int_t) time_point < highest_initial_pos)
			auto_continue = true;
		++time_point;
		formula_time_point = time_point;
	}
	prune_memory(completed_time_point);
	// NO maybe_gc() here: the caller consumes the returned output map
	// AFTER this return (run()'s write, api::step's serialization), and
	// prune_memory above may have evicted the completed step's entries --
	// the map's nodes' only remaining anchors. A sweep at this point frees
	// them under the caller's feet (seen as nondeterministic garbage keys
	// aborting write() on the qlt S-operator execution). The sweep runs at
	// the START of the next step instead; the returned map is pinned
	// through that sweep via last_outputs_ (IN-M1), so a host may keep
	// reading it while it feeds the next step.
	last_outputs_ = global;
	return { global, auto_continue };
}

template <NodeType node>
void interpreter<node>::prune_memory(size_t completed_time_point) {
	// Once calculate_initial_spec() reaches final_system, step_spec is
	// fixed to ubt_ctn with its literal ("initial") time positions still
	// embedded, and every future step substitutes memory into it -- so
	// any entry at or below highest_initial_pos is read for the lifetime
	// of the interpreter and must never be evicted.
	//
	// Every other entry was produced by resolving a relative shift
	// (see update_to_time_point / fm_at_time_point), and those are only
	// ever read within [formula_time_point - lookback, formula_time_point]
	// (see update_to_time_point's use in step() and appear_within_lookback).
	// formula_time_point is non-decreasing, so once an entry's time point
	// falls below that window it can never be read by a future step
	// again -- it would ordinarily be safe to drop.
	//
	// However, run()'s driver calls update() *after* step() returns,
	// referencing completed_time_point (== old time_point) via a literal
	// shift such as `o2[-1]` (see run()'s use of `time_point - 1` and
	// is_memory_access_valid). That is a step later than this function
	// sees, so never evict the step that was just completed -- only
	// entries strictly older than it -- giving such a caller one full
	// step to reference it before it is gone.
	const int_t oldest_needed = std::min(
		(int_t)formula_time_point - lookback,
		(int_t)completed_time_point);
	std::erase_if(memory, [&](const auto& kv) {
		// IN-M5: a solution can key an aux bit by its complement
		// (o__ltl_s0__[t]', bf_neg-wrapped; see write()). Unwrap so
		// such entries age out like the stream entry they stand for,
		// instead of accumulating one per step for the run's lifetime.
		tref key = kv.first;
		while (tau::get(key).has_child()
			&& tau::get(key)[0].is(tau::bf_neg))
			key = tau::get(key)[0].first();
		tref v = tau::trim(key);
		// Leave non-stream entries (e.g. solved uninterpreted constants)
		// alone: they are not time-indexed and do not grow with time_point.
		if (!is_io_var<node>(v) || !is_io_initial<node>(v)) return false;
		int_t t = get_io_time_point<node>(v);
		if (t <= highest_initial_pos) return false;
		return t < oldest_needed;
	});
}

template <NodeType node>
void interpreter<node>::collect_live_refs(std::unordered_set<tref>& keep) const {
	// ubt_ctn, original_spec, and ctx.{types,inputs,outputs} hold htrefs
	// directly; bintree<node>::gc() preserves their nodes via M's
	// non-expired weak_ptr entries, no walk needed.
	for (const trefs& part_alts : step_spec)
		keep.insert(part_alts.begin(), part_alts.end());
	for (auto& [k, v] : memory)  { keep.insert(k); keep.insert(v); }
	for (auto& [k, _] : inputs)  keep.insert(k);
	for (auto& [k, _] : outputs) keep.insert(k);
	// IN-M1: the previous step's returned map (see step()'s lifetime
	// note) and the raw atom trefs of the cached Mealy solution, which
	// nothing else anchors.
	for (auto& [k, v] : last_outputs_) { keep.insert(k); keep.insert(v); }
	if (cached_solution)
		for (const auto& [atom, _] : cached_solution->atoms)
			keep.insert(atom);
	output_partition.collect_live_refs(keep);
}

template <NodeType node>
void interpreter<node>::maybe_gc(const assignment<node>* pin) {
	if (gc_growth_factor <= 0.0) return;
	const size_t m_pre = tau::m_size();
	// Floor: don't sweep until M is non-trivially large. Bounds peak size
	// for any workload (a sweep fires no later than M = gc_min_size + growth).
	if (m_pre < gc_min_size) return;
	// Amortize: don't sweep unless M grew enough since the last sweep.
	if ((double)m_pre < gc_growth_factor * (double)m_at_last_gc) return;

	const auto t0 = std::chrono::steady_clock::now();
	std::unordered_set<tref> keep;
	if (pin) for (const auto& [k, v] : *pin) {
		keep.insert(k);
		keep.insert(v);
	}
	collect_live_refs(keep);
	definitions<node>::instance().collect_live_refs(keep);
	// The Tau-BDD store holds raw Tau trefs (variables) and is not swept
	// by bintree<node>::gc(), so its references must be pinned.
	tau_term_bdd<node>::collect_live_refs(keep);
	// ba_constants::T, ba_types::type_trees(), ubt_ctn, original_spec and
	// the io_context maps hold htrefs — gc() preserves their nodes via M's
	// non-expired weak_ptr entries, no explicit walk needed. Caches made
	// with tree<node>::create_cache<>() are filtered by gc_callbacks.
	bintree<node>::gc(keep);

	const size_t m_post = tau::m_size();
#ifdef __linux__
	// Return free heap pages to the OS so RSS reflects live allocation
	// rather than allocator fragmentation. Cheap (microseconds).
	malloc_trim(0);
#endif
	const auto dt_us = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::steady_clock::now() - t0).count();
	m_at_last_gc = m_post;
	LOG_DEBUG << TC.YELLOW() << "interpreter::gc" << TC.CLEAR()
		<< " M=" << m_pre << "->" << m_post
		<< " keep=" << keep.size() << " " << dt_us << "us"
		<< " step=" << time_point;
}

template <NodeType node>
std::vector<trefs> interpreter<node>::get_ubt_ctn_at(int_t t) {
	LOG_TRACE << "get_ubt_ctn_at begin \n";
	LOG_TRACE << "get_ubt_ctn_at[t]: " << t << "\n";

	const int_t ut = t < (int_t)formula_time_point
					? (int_t)formula_time_point : t;
	std::vector<trefs> upd_ubt_ctn;
	if (t >= std::max(highest_initial_pos, (int_t)formula_time_point)) {
		for (const htrefs& part : ubt_ctn) {
			trefs part_alts;
			part_alts.reserve(part.size());
			for (const auto& h : part)
				part_alts.push_back(
					update_to_time_point(h->get(), ut));
			upd_ubt_ctn.push_back(std::move(part_alts));
		}
		return upd_ubt_ctn;
	}
	// Adjust ubt_ctn to time_point by eliminating inputs and outputs
	// which are greater than current time_point in a time-compatible fashion
	for (const htrefs& part : ubt_ctn) {
		trefs part_alts;
		part_alts.reserve(part.size());
		for (const auto& h : part) {
		auto step_ubt_ctn = update_to_time_point(h->get(), ut);
		auto io_vars = tau::get(step_ubt_ctn).select_top(
				is_child<node, tau::io_var>);
		std::sort(io_vars.begin(), io_vars.end(), constant_io_comp<node>);
		// All io_vars in fm have to refer to constant time positions
		assert(std::all_of(io_vars.begin(), io_vars.end(),
			[](const auto& el) { return is_io_initial<node>(el); }));
		while (!io_vars.empty()) {
			tref v = io_vars.back();
			if (get_io_time_point<node>(v) <= t) {
				io_vars.pop_back();
				continue;
			}
			if (tau::get(v).is_input_variable())
				step_ubt_ctn = build_wff_all<node>(v, step_ubt_ctn, false);
			else step_ubt_ctn = build_wff_ex<node>(v, step_ubt_ctn, false);
			io_vars.pop_back();
		}
		LOG_TRACE << "get_ubt_ctn_at[step_ubt_ctn]: " << tau::get(step_ubt_ctn) << "\n";

		// Eliminate added quantifiers
		part_alts.push_back(normalize_non_temp<node>(step_ubt_ctn));
		}
		upd_ubt_ctn.push_back(std::move(part_alts));
	}
	LOG_TRACE << "get_ubt_ctn_at end \n";
	return upd_ubt_ctn;
}

template <NodeType node>
bool interpreter<node>::calculate_initial_spec() {
	LOG_TRACE << "calculate_initial_spec begin \n";
	if (final_system) return true;

	size_t initial_segment = std::max(highest_initial_pos, (int_t)formula_time_point);
	LOG_TRACE << "calculate_initial_spec[initial_segment]: " << initial_segment << "\n";
	LOG_TRACE << "calculate_initial_spec[time_point]: " << time_point << "\n";
	// If time_point < initial_segment, recompute systems
	if (time_point < initial_segment) {
		step_spec = get_ubt_ctn_at(time_point);
	} else if (time_point == initial_segment) {
		// TODO: update constant time positions with values from memory to simplify step_spec
		step_spec.clear();
		step_spec.reserve(ubt_ctn.size());
		for (const htrefs& part : ubt_ctn) {
			trefs part_alts;
			part_alts.reserve(part.size());
			for (const auto& h : part) part_alts.push_back(h->get());
			step_spec.push_back(std::move(part_alts));
		}
		final_system = true;
	}
	LOG_TRACE << "calculate_initial_systems[result]: true";
	LOG_TRACE << "calculate_initial_systems end";
	return true;
}

template <NodeType node>
bool interpreter<node>::has_this_input_stream() const {
	for (const auto& [var, _] : inputs)
		if (get_var_name<node>(var) == "this"
			&& ctx.type_of(var) == get_ba_type_id<node>(
							tau_type<node>()))
			return true;
	return false;
}

template <NodeType node>
std::pair<trefs, bool> interpreter<node>::build_inputs_for_step(
	const size_t t)
{
	LOG_TRACE << "build_inputs_for_step begin";
	trefs step_inputs;
	bool has_this_stream = false;
	if (inputs.empty()) LOG_TRACE << "build_inputs_for_step[inputs]: empty";
	for (auto& [var, _] : inputs) {
		LOG_TRACE << "build_inputs_for_step[var]: " << LOG_FM_DUMP(var);
		LOG_TRACE << "build_inputs_for_step[ctx.type_of(var)] " << LOG_BA_TYPE(ctx.type_of(var));
		if (get_var_name<node>(var) == "this") {
			if (size_t vt = ctx.type_of(var);
				vt == get_ba_type_id<node>(tau_type<node>())) {
				has_this_stream = true;
				LOG_TRACE << "build_inputs_for_step[has_this_stream]: true";
				continue;
			}
		}
		step_inputs.emplace_back(build_in_var_at_n<node>(
			get_var_name_node<node>(var), t, ctx.type_of(var)));
		DBG(LOG_TRACE << "build_inputs_for_step[step_input]: " << LOG_FM_DUMP(step_inputs.back());)
	}
	LOG_TRACE << "build_inputs_for_step end (step_inputs size: " << step_inputs.size() << ")";
	return { step_inputs, has_this_stream };
}

template <NodeType node>
tref interpreter<node>::update_to_time_point(
	tref f, const int_t t) {
	LOG_TRACE << "update_to_time_point begin\n";
	// update the f according to current time_point, i.e. for each
	// input/output var which has a shift, we replace it with the value
	// corresponding to the current time_point minus the shift.
	auto io_vars = tau::get(f).select_top(is_child<node, tau::io_var>);
	auto result = fm_at_time_point<node>(f, io_vars, t);
	LOG_TRACE << "update_to_time_point[result]: " << LOG_FM_DUMP(result) << "\n";
	LOG_TRACE << "update_to_time_point end\n";
	return result;
}

template <NodeType node>
bool interpreter<node>::is_memory_access_valid(const auto& io_vars) const
{
	// Check for each constant time point accessing memory, if it is available
	for (tref io_var : io_vars) {
		if (is_io_initial<node>(io_var) &&
			get_io_time_point<node>(io_var) < (int_t)time_point) {
			const auto& v = tau::get(tau::bf, io_var);
			if (!memory.contains(v)) return false;
		}
	}
	return true;
}

template <NodeType node>
void interpreter<node>::compute_lookback_and_initial() {
	trefs io_vars;
	for (const htrefs& part : ubt_ctn) for (const auto& h : part) {
		const trefs current_io_vars = tau::get(h->get()).select_top(
			is_child<node, tau::io_var>);
		io_vars.insert(io_vars.end(),
			current_io_vars.begin(), current_io_vars.end());
	}
	lookback = get_max_shift<node>(io_vars);
	formula_time_point = time_point + lookback;
	highest_initial_pos = get_max_initial<node>(io_vars);
}

template <NodeType node>
tref interpreter<node>::get_executable_spec(
	tref& clause, const size_t start_time) {
	LOG_TRACE << "get_executable_spec begin\n";

	DBG(LOG_TRACE << "compute_systems/clause: " << LOG_FM(clause);)
	tref executable = transform_to_execution<node>(clause, start_time, true);
	DBG(LOG_TRACE << "compute_systems/executable: " << LOG_FM(executable);)
	if (tau::get(executable).equals_F()) return nullptr;
	// Make sure that no constant time position is smaller than 0
	trefs io_vars = tau::get(executable).select_top(
		is_child<node, tau::io_var>);
	for (tref io_var : io_vars) {
		if (is_io_initial<node>(io_var)
			&& get_io_time_point<node>(io_var) < 0)
		{
			LOG_ERROR << "Constant time position is smaller than 0\n";
			return nullptr;
		}
	}
	// compute model for uninterpreted constants and solve it
	tref constraints = get_uninterpreted_constants_constraints<node>(
		executable, io_vars, start_time);
	if (tau::get(constraints).equals_F()) return nullptr;
	DBG(LOG_TRACE << "compute_systems/constraints: " << constraints;)
	if (!tau::get(constraints).equals_T()) {
		// setting proper options for the solver
		solver_options options = {
			.splitter_one = node::ba::splitter_one(
				tau_type<node>()),
			.mode = solver_mode::general
		};
		bool solve_error = false;
		auto model = solve<node>(constraints, options, solve_error);
		if (solve_error) {
			LOG_ERROR << "Internal error in solver\n";
			return nullptr;
		}
		if (!model) return nullptr;

		LOG_INFO << "Tau specification part " << tau::get(clause) << " is executed setting ";
		for (const auto& [uc, v] : model.value())
			LOG_INFO << TAU_TO_STR(uc) << " := " << TAU_TO_STR(v);

		executable = rewriter::replace<node>(executable, model.value());
		clause = rewriter::replace<node>(clause, model.value());
		LOG_INFO << "Resulting Tau specification part: " << TAU_TO_STR(clause) << "\n";
		LOG_TRACE << "get_executable_spec[spec]: " << LOG_FM(executable) << "\n";
	}
	LOG_TRACE << "get_executable_spec[spec]: " << LOG_FM(executable);
	LOG_TRACE << "get_executable_spec end\n";
	return executable;
}

template <NodeType node>
bool interpreter<node>::compute_part_continuations(htrefs& alts, htrefs& ctns,
	const size_t start_time)
{
	// Uninterpreted constants are solved per alternative here; a model is
	// baked into the alternative it was solved on, so each alternative
	// stays self-consistent even when the chosen values differ.
	//
	// Only the first alternative and the last one (the most recent
	// last-resort update clause, the part's universally executable anchor)
	// get the full executability transform: it runs a phi-fixpoint with a
	// cvc5 implication check per unroll step, and for a middle alternative
	// its only effects are fixpoint tightening of a step formula that is
	// solved in milliseconds anyway, or an F verdict that would send a
	// conditional alternative to the body continuation regardless. Stack
	// samples on bv[64]x14 put ~80% of the per-update wall in these
	// transforms, one per alternative. Middle alternatives that need the
	// transform's machinery (sometimes clauses, uninterpreted constants)
	// still take it.
	// IN-M8: both shortcuts below skip get_executable_spec, and with it
	// its rejection of constant time positions below 0. Apply that check
	// here so a lookback-shifted alternative cannot slip through as a
	// continuation with an unresolvable negative position.
	auto has_negative_position = [](tref f) {
		for (tref v : tau::get(f).select_top(is_child<node, tau::io_var>))
			if (is_io_initial<node>(v) && get_io_time_point<node>(v) < 0)
				return true;
		return false;
	};
	htrefs kept;
	ctns.clear();
	for (size_t idx = 0; idx < alts.size(); ++idx) {
		const htref& alt = alts[idx];
		if (idx != 0 && idx != alts.size() - 1) {
			const tau& c = tau::get(alt->get());
			tref aw = c.find_top(is_child<node, tau::wff_always>);
			if (aw && !c.find_top(
					is_child<node, tau::wff_sometimes>)
				&& !c.find_top(is<node, tau::uconst>)
				&& !has_negative_position(alt->get()))
			{
				kept.push_back(alt);
				ctns.push_back(tree<node>::geth(
					rewriter::replace<node>(alt->get(),
						aw, tau::trim2(aw))));
				continue;
			}
		}
		tref clause_t = alt->get();
		tref ctn = get_executable_spec(clause_t, start_time);
		if (ctn == nullptr) {
			// The executability transform judges universal
			// executability over all inputs, so it rejects a
			// CONDITIONAL alternative -- one viable only for
			// cooperating inputs, which is exactly what the
			// factored revision keeps around (I1). Such an
			// alternative is still a valid pointwise step
			// formula: keep it with its always body as the
			// continuation and let step() fall through when the
			// current inputs do not allow it. Restricted to pure
			// always alternatives -- a sometimes clause needs the
			// eventuality machinery of the transform and
			// uninterpreted constants need its model baking.
			const tau& c = tau::get(alt->get());
			tref aw = c.find_top(is_child<node, tau::wff_always>);
			if (aw && !c.find_top(
					is_child<node, tau::wff_sometimes>)
				&& !c.find_top(is<node, tau::uconst>)
				&& !has_negative_position(alt->get()))
			{
				LOG_DEBUG << "update/keeping conditional "
					"specification alternative: "
					<< LOG_FM(alt->get()) << "\n";
				kept.push_back(alt);
				ctns.push_back(tree<node>::geth(
					rewriter::replace<node>(alt->get(),
						aw, tau::trim2(aw))));
				continue;
			}
			// A dead alternative can never fire in step(); keep
			// the part running on the others.
			LOG_DEBUG << "update/dropping non-executable "
				"specification alternative: "
				<< LOG_FM(alt->get()) << "\n";
			continue;
		}
		// get_executable_spec may rewrite its tref& clause arg.
		kept.push_back(tree<node>::geth(clause_t));
		ctns.push_back(tree<node>::geth(ctn));
	}
	if (kept.empty()) return false;
	alts = std::move(kept);
	return true;
}

// The I3/B10 semantic no-op checks in update()/pointwise_revision are
// optimizations -- skipping them is always sound. They are restricted to
// arithmetic-free formulas because is_tau_impl/are_tau_equivalent can hang
// even on SMALL mixed lookback bv pairs (observed on bv_stress_check inputs
// of ~100-200 chars; recorded as B11 in
// private/review-pointwise-revision-2026-08-16.md).
template <NodeType node>
static bool pwr_contains_arith_content(tref f) {
	return tree<node>::get(f).find_top([](tref t) {
		return pack_type_has_arith_ops<node>(
			tree<node>::get(t).get_ba_type());
	}) != nullptr;
}

template <NodeType node>
std::optional<typename interpreter<node>::update_plan>
	interpreter<node>::plan_update(tref update)
{
	DBG(LOG_TRACE << "interpreter::plan_update(update = \"" << LOG_FM(update) << "\")";)
	// TODO: shift spec time according to new lookback from update
	trefs io_vars = tau::get(update)
				.select_top(is_child<node, tau::io_var>);
	// the constant time positions in update are seen relative to
	// time_point, i.e. time point 0 is shifted to time_point
	tref shifted_update = shift_const_io_vars_in_fm<node>(
						update, io_vars, time_point);
	if (tau::get(shifted_update).equals_F()) {
		LOG_WARNING << "No update performed: constant time position below 0 was found\n";
		return {};
	}
	io_vars = tau::get(shifted_update)
				.select_top(is_child<node, tau::io_var>);
	if (!is_memory_access_valid(io_vars)) {
		LOG_WARNING << "No update performed: invalid memory access was found\n";
		return {};
	}
	shifted_update = rewriter::replace<node>(shifted_update, memory);
	shifted_update = normalizer<node>(shifted_update);
	LOG_TRACE << "update/shifted_update: " << LOG_FM(shifted_update) << "\n";

	// The constant time positions in original_spec need to be replaced by
	// present assignments from memory and already executed sometimes
	// statements need to be removed. This working copy does not depend on
	// the update clause, so compute it once for the whole clause loop (I6).
	auto memory_spec = original_spec;
	for (auto& [alts, rep] : memory_spec) {
		// update current spec part with memory
		// TODO: maybe update constant time positions to current time point in order to avoid loosing initial conditions on restarting updated specification
		for (htref& alt : alts) {
			alt = tree<node>::geth(rewriter::replace<node>(
				alt->get(), memory));
			LOG_DEBUG << "update/memory replaced spec: "
				<< LOG_FM(alt->get()) << "\n";
		}
	}
	// TODO: memory_spec = remove_happend_sometimes(memory_spec);

	// For each clause of update, check if we can do pointwise revision
	for (tref clause : expression_paths<node>(shifted_update)) {
		auto current_spec = memory_spec;
		auto current_ubd_ctn = ubt_ctn;
		// IN-N11: the two partitions are parallel by construction
		// (make_interpreter pairs every ubt_ctn entry, including the
		// Mealy initial-output part). Keep the check loud in both
		// builds -- a mismatch would silently pair parts with the
		// wrong continuations below.
		if (current_spec.size() != current_ubd_ctn.size()) {
			LOG_ERROR << "interpreter::update: spec partition ("
				<< current_spec.size() << ") and continuation "
				"partition (" << current_ubd_ctn.size()
				<< ") are out of step; refusing the update\n";
			return {};
		}
		union_find_with_sets<decltype(stream_comp), node> uf(stream_comp);
		auto upd_partition = create_spec_partition(clause, uf);
		// Merge current output_partition into uf
		uf.merge(output_partition);
		// Merge now overlapping spec parts
		// B4: the continuation of a conjunction is not the conjunction
		// of continuations (sometimes-parts interact in
		// transform_to_execution), so merged parts are marked here and
		// their continuation recomputed below via get_executable_spec.
		std::vector<bool> part_merged(current_spec.size(), false);
		for (size_t i = 0; i < current_spec.size(); ++i) {
			// A part with no representative has no output stream
			// (or uninterpreted constant) to revise: it is an
			// input-only constraint, or the Mealy initial-output
			// part make_interpreter adds (IN-N11); it never merges
			// and never receives an update (PW-10h).
			if (!current_spec[i].second) continue;
			for (size_t j = i+1; j < current_spec.size(); ++j) {
				// If no output/uninterpreted constant present, skip
				if (!current_spec[j].second) continue;
				if (uf.connected(current_spec[i].second->get(), current_spec[j].second->get())) {
					// Merged part alternatives: the cross
					// product of both parts' alternatives
					// in lexicographic preference order --
					// (⋁A)∧(⋁B) with the stronger pairs
					// tried first.
					htrefs merged;
					merged.reserve(current_spec[i].first.size()
						* current_spec[j].first.size());
					for (const htref& a : current_spec[i].first)
						for (const htref& b : current_spec[j].first)
							merged.push_back(tree<node>::geth(
								tau::build_wff_and(
									a->get(), b->get())));
					current_spec[i].first = std::move(merged);
					part_merged[i] = true;
					current_spec.erase(current_spec.begin()+j);
					current_ubd_ctn.erase(current_ubd_ctn.begin()+j);
					part_merged.erase(part_merged.begin()+j);
					--j;
				}
			}
		}
		// Collect overlapping update parts
		// Initialize the collected updates for current spec parts
		trefs collected_updates(current_spec.size(), tau::_T());
		for (size_t i = 0; i < current_spec.size(); ++i) {
			// If no output/uninterpreted constant present, skip
			if (!current_spec[i].second) continue;
			for (size_t j = 0; j < upd_partition.size(); ++j) {
				// If no output/uninterpreted constant present, skip
				if (!upd_partition[j].second) continue;
				// Check if current spec part overlaps with current update part
				if (uf.connected(current_spec[i].second->get(), upd_partition[j].second->get())) {
					// Add current update part to update collection
					collected_updates[i] = tau::build_wff_and(collected_updates[i], upd_partition[j].first->get());
					// Now remove update part from upd_partition
					upd_partition.erase(upd_partition.begin()+j);
					--j;
				}
			}
		}
		// Unsqueeze always statements in current_spec and collected_updates
		for (auto& [part_alts, _] : current_spec) {
			// Unsqueeze always parts in each alternative
			for (htref& alt : part_alts) alt = tree<node>::geth(
				unsqueeze_always(alt->get()));
		}
		for (tref& upd: collected_updates) {
			// Unsqueeze always parts in upd
			upd = unsqueeze_always(upd);
		}
		// Now do pointwise revision on each part of current spec with collected_updates
		bool update_valid = true;
		for (size_t i = 0; i < current_spec.size(); ++i) {
			// A part with no update keeps its spec; its continuations
			// only need recomputing when the part was merged (B4).
			bool need_recompute = part_merged[i];
			if (!tau::get(collected_updates[i]).equals_T()) {
				auto revision = pointwise_revision(
					current_spec[i].first,
					collected_updates[i], time_point);
				// nullopt when no update clause yields a sat
				// revision or the definitions in a clause do
				// not settle; without a revised part the
				// update cannot be accepted.
				if (!revision) { update_valid = false; break; }
				LOG_DEBUG << "update/pointwise revision on part: "
					<< LOG_FM(part_alts_fm<node>(
						current_spec[i].first)) << "\n";
				// Unchanged-revision early-out: elementwise
				// syntactic equality first; the semantic
				// fallback compares the parts' disjunctions
				// (review B10). Restricted to bv-free
				// formulas -- see pwr_contains_arith_content
				// (B11).
				bool unchanged = revision->size()
					== current_spec[i].first.size();
				if (unchanged)
					for (size_t k = 0; k < revision->size(); ++k)
						if (!tau::subtree_equals(
							current_spec[i].first[k]->get(),
							(*revision)[k]->get()))
						{
							unchanged = false;
							break;
						}
				if (!unchanged) {
					tref old_fm = part_alts_fm<node>(
						current_spec[i].first);
					tref new_fm = part_alts_fm<node>(
						*revision);
					unchanged = !pwr_contains_arith_content<node>(old_fm)
						&& !pwr_contains_arith_content<node>(new_fm)
						&& are_tau_equivalent<node>(
							old_fm, new_fm);
				}
				if (!unchanged) {
					current_spec[i].first =
						std::move(*revision);
					need_recompute = true;
				}
			}
			if (!need_recompute) continue;
			if (!compute_part_continuations(current_spec[i].first,
				current_ubd_ctn[i], time_point))
			{
				update_valid = false;
				break;
			}
		}
		if (!update_valid) continue;
		// Here, all pointwise revisions were successful
		// We now add the remaining update parts left in upd_partition
		for (auto& upd : upd_partition) {
			tref clause_t = upd.first->get();
			tref new_ubd_ctn_part = get_executable_spec(clause_t, time_point);
			upd.first = tree<node>::geth(clause_t);
			if (new_ubd_ctn_part == nullptr) {
				update_valid = false;
				break;
			}
			current_ubd_ctn.push_back(
				{ tree<node>::geth(new_ubd_ctn_part) });
			current_spec.emplace_back(htrefs{ upd.first },
				upd.second);
		}
		if (!update_valid) continue;
		// Here, all update parts were successful
		// The unbound continuation from start_time is possible for all parts,
		// so it is safe to swap the current spec by update_unbound.
		// B1: collect (and thereby validate) the stream maps BEFORE
		// committing anything -- a failed collect used to leave the
		// interpreter half-updated (new spec, cleared stream maps),
		// making every subsequent step() fail on stream lookups. The
		// collects auto-register unknown console streams and reject
		// untyped ones, so a rebuild after successful collects cannot
		// fail on a missing stream.
		subtree_map<node, size_t> out_stream_ids, in_stream_ids;
		bool streams_ok = true;
		for (const auto& [part_alts, _] : current_spec) {
			for (const htref& alt : part_alts)
				if (!collect_output_streams(alt->get(),
					out_stream_ids))
				{
					streams_ok = false;
					break;
				}
			if (!streams_ok) break;
		}
		if (streams_ok) for (const auto& [part_alts, _] : current_spec) {
			for (const htref& alt : part_alts)
				if (!collect_input_streams(alt->get(),
					in_stream_ids))
				{
					streams_ok = false;
					break;
				}
			if (!streams_ok) break;
		}
		if (!streams_ok) {
			LOG_WARNING << "No update performed: stream collection "
				"failed for the revised specification\n";
			continue;
		}

		// PW-4 (B1): open the revised streams into locals BEFORE
		// anything is committed -- a failure here used to leave the
		// interpreter half-updated (new spec, cleared stream maps).
		output_streams<node> new_outputs;
		input_streams<node>  new_inputs;
		if (!build_outputs(out_stream_ids, new_outputs)) {
			LOG_WARNING << "No update performed: output stream "
				"rebuild failed for the revised specification\n";
			continue;
		}
		if (!build_inputs(in_stream_ids, new_inputs)) {
			LOG_WARNING << "No update performed: input stream "
				"rebuild failed for the revised specification\n";
			continue;
		}
		tref updated_spec = unsqueeze_always(
			combined_spec_fm<node>(current_spec));
		return update_plan(std::move(current_ubd_ctn),
			std::move(current_spec), std::move(uf),
			std::move(new_inputs), std::move(new_outputs),
			TAU_TO_STR(updated_spec));
	}
	// No more clause left in update and all clauses are not realizable
	LOG_WARNING << "No update performed: updated specification is unsat\n";
	return {};
}

template <NodeType node>
bool interpreter<node>::update(tref update) {
	DBG(LOG_TRACE << "interpreter::update(update = \"" << LOG_FM(update) << "\")";)
	auto plan = plan_update(update);
	if (!plan) return false;
	// I7: growth telemetry -- the only prior symptom of the revision
	// doubling was the interpreter getting slower.
	LOG_INFO << "Updated specification (" << plan->spec_str.size()
		<< " chars): " << plan->spec_str << "\n\n";
	if (spec_size_warn_threshold
		&& plan->spec_str.size() > spec_size_warn_threshold)
		LOG_WARNING << "Updated specification size "
			<< plan->spec_str.size() << " chars exceeds the "
			"spec-size-warn threshold "
			<< spec_size_warn_threshold << "\n";
	// Commit: every component was validated by plan_update, so nothing
	// below can fail and leave the interpreter half-updated.
	ubt_ctn = std::move(plan->ubt_ctn);
	original_spec = std::move(plan->spec);
	output_partition = std::move(plan->partition);
	outputs = std::move(plan->outputs);
	inputs = std::move(plan->inputs);
	// The systems for solver need to be recomputed at beginning of next step
	final_system = false;
	chosen_alt_.clear();
	compute_lookback_and_initial();
	// IN-N3: the synthesised strategy (if any) described the spec
	// before this revision; keep it for reset()'s re-seeding, but mark
	// it stale for the introspection API.
	if (cached_solution) cached_solution_stale_ = true;
	return true;
}

template <NodeType node>
bool interpreter<node>::can_extend(tref psi) {
	// Dry-run update(): plan_update computes everything update() would
	// commit without mutating the interpreter, so the two agree by
	// construction (PW-N9 / IN-M7).
	if (psi == nullptr) return true;
	return plan_update(psi).has_value();
}

template <NodeType node>
std::optional<htrefs> interpreter<node>::pointwise_revision(
	const htrefs& alts_in, tref update, const int_t start_time)
{
	// Split any temporally disjunctive alternative into adjacent
	// alternatives, so each carries a single temporal clause. This
	// dissolves the old first-clause-only handling of disjunctive specs
	// (review B3) -- preference among the pieces of a split alternative
	// is arbitrary but fixed. The alternatives are NOT run through the
	// temporal spec normalizer: an alternative only needs to be viable
	// for cooperating inputs (step() falls through to the next one
	// otherwise), while the temporal normalizer decides universal
	// executability over all inputs and would collapse exactly the
	// conditional alternatives the factored revision exists to keep.
	// Nested-temporal content (U/R/W/S/T/F operators) lies outside the
	// always/sometimes decomposition this factored revision performs --
	// feeding it through anyway made the U-of-U revision diverge (the
	// clause loop can neither classify nor discharge the nested untils).
	// Delegate such pairs to the full temporal algorithm
	// (pointwise_revision_temporal, pwr-ltl.tex §3), one alternative at a
	// time; its single revised formula becomes that alternative.
	auto has_nested_temporal = [](tref f) {
		return tree<node>::get(f).find_top([](tref m) {
			const auto& t = tree<node>::get(m);
			if (!t.is(tree<node>::wff) || !t.has_child())
				return false;
			const auto nt = t[0].value.nt;
			return nt == tree<node>::wff_U
			    || nt == tree<node>::wff_R
			    || nt == tree<node>::wff_W
			    || nt == tree<node>::wff_S
			    || nt == tree<node>::wff_T
			    || nt == tree<node>::wff_F;
		}) != nullptr;
	};
	{
		bool nested = has_nested_temporal(update);
		if (!nested) for (const htref& h : alts_in)
			if (has_nested_temporal(h->get())) {
				nested = true;
				break;
			}
		if (nested) {
			trefs out;
			size_t failed = 0;
			for (const htref& h : alts_in) {
				tref r = pointwise_revision_temporal<node>(
					h->get(), update, start_time);
				// IN-M6: nullptr (the revision could not be
				// built) and F (the alternative is gone) are
				// different outcomes; say which.
				if (!r) {
					++failed;
					LOG_WARNING << "Pointwise revision of a "
						"nested-temporal alternative could not "
						"be built: " << TAU_TO_STR(h->get())
						<< "\n";
					continue;
				}
				if (tau::get(r).equals_F()) {
					LOG_DEBUG << "update/nested-temporal "
						"alternative revised to F, dropped: "
						<< LOG_FM(h->get()) << "\n";
					continue;
				}
				out.push_back(r);
			}
			if (out.empty()) {
				if (failed) LOG_WARNING << "No update performed: "
					"the temporal revision could not be built "
					"for any alternative\n";
				return {};
			}
			// Same dedupe and cap as the factored path below.
			return finalize_alternatives(out);
		}
	}

	trefs alts;
	for (const htref& h : alts_in)
		for (tref c : expression_paths<node>(h->get())) {
			if (tau::get(c).equals_F()) continue;
			alts.push_back(c);
		}
	auto to_htrefs = [](const trefs& v) {
		htrefs r;
		r.reserve(v.size());
		for (tref f : v) r.push_back(tree<node>::geth(f));
		return r;
	};
	update = normalizer<node>(update);
	// If the update is T, nothing changes
	if (tau::get(update).equals_T()) return to_htrefs(alts);
	// PW-R6: one satisfiability memo per factored revision — the clause,
	// sometimes-conjunction and gate checks repeat identical hash-consed
	// (formula, start_time) queries, each a subprocess on temporal
	// content (see pointwise_revision.h for the delegated-path twin).
	pwr_sat_memo memo;
	for (tref clause : expression_paths<node>(update)) {
		tref upd_always = tau::get(clause).find_top(
			is_child<node, tau::wff_always>);
		trefs upd_sometime = tau::get(clause).select_top(
			is_child<node, tau::wff_sometimes>);

		// Check if the update by itself is sat from current time point onwards
		// taking the memory into account
		LOG_TRACE << "pwr/clause: " << LOG_FM(clause) << "\n";
		if (!pwr_memo_sat<node>(clause, start_time, &memo))
			continue;

		// An update already implied by the running spec is a no-op;
		// conjoining it anyway re-embeds it verbatim and feeds the
		// per-update growth (review I3).
		tref spec_fm = tau::build_wff_or(alts);
		if (!alts.empty()
			&& !pwr_contains_arith_content<node>(spec_fm)
			&& !pwr_contains_arith_content<node>(clause)
			&& is_tau_impl<node>(spec_fm, clause)) {
			LOG_DEBUG << "pwr/update already implied by the "
				"specification; keeping it unchanged\n";
			return to_htrefs(alts);
		}

		// TODO: call type inference algorithm in order to unify
		// types between current spec and update

		// Decompose each alternative into its always body and its
		// sometimes clauses.
		const size_t n = alts.size();
		trefs bodies(n, nullptr);
		std::vector<trefs> alt_sometimes(n);
		bool any_always = false;
		for (size_t i = 0; i < n; ++i) {
			if (tref aw = tau::get(alts[i]).find_top(
				is_child<node, tau::wff_always>); aw)
			{
				bodies[i] = tau::trim2(aw);
				any_always = true;
			}
			alt_sometimes[i] = tau::get(alts[i]).select_top(
				is_child<node, tau::wff_sometimes>);
		}

		// Conjoin an alternative's own sometimes clauses when they
		// remain sat with it; drop them otherwise (the accumulated
		// spec's eventualities give way to the update, matching the
		// previous single-formula behavior).
		auto with_spec_sometimes = [&](tref base, const trefs& sts) {
			if (sts.empty()) return base;
			tref with = build_wff_and<node>(base,
				build_wff_and<node>(sts));
			return pwr_memo_sat<node>(with, start_time, &memo)
				? with : base;
		};

		trefs new_alts;
		if (!any_always) {
			// No alternative carries an always part: the update
			// clause replaces the always-free spec, keeping each
			// alternative's sometimes clauses where possible.
			if (n == 0) new_alts.push_back(clause);
			for (size_t i = 0; i < n; ++i)
				new_alts.push_back(with_spec_sometimes(
					clause, alt_sometimes[i]));
		} else {
			// B2a: align every alternative's always body (and the
			// update body) to the common lookback frame ONCE and
			// build every piece from the aligned bodies.
			tref u_body = upd_always
				? tau::trim2(upd_always) : nullptr;
			trefs u_ios;
			int_t lb = 0;
			if (u_body) {
				u_ios = tau::get(u_body).select_top(
					is_child<node, tau::io_var>);
				lb = get_max_shift<node>(u_ios);
			}
			std::vector<trefs> b_ios(n);
			for (size_t i = 0; i < n; ++i) {
				if (!bodies[i]) continue;
				b_ios[i] = tau::get(bodies[i]).select_top(
					is_child<node, tau::io_var>);
				lb = std::max(lb,
					get_max_shift<node>(b_ios[i]));
			}
			if (u_body)
				if (int_t lb_u = get_max_shift<node>(u_ios);
					lb_u < lb) u_body =
						shift_io_vars_in_fm<node>(
							u_body, u_ios,
							lb - lb_u);
			for (size_t i = 0; i < n; ++i) {
				if (!bodies[i]) {
					bodies[i] = u_body ? u_body
						: nullptr;
					continue;
				}
				if (int_t lb_i = get_max_shift<node>(b_ios[i]);
					lb_i < lb) bodies[i] =
						shift_io_vars_in_fm<node>(
							bodies[i], b_ios[i],
							lb - lb_i);
				if (u_body) bodies[i] = build_wff_and<node>(
					bodies[i], u_body);
				// Pointwise (non-temporal) simplification
				// only; see the note on the alternative
				// flattening above.
				bodies[i] = normalize_non_temp<node>(
					bodies[i]);
			}
			// Gate: is the plain conjunction of the update with
			// the part -- one always over the disjunction of the
			// aligned bodies -- satisfiable?
			trefs live_bodies;
			for (tref b : bodies)
				if (b && !tau::get(b).equals_F())
					live_bodies.push_back(b);
			tref gate = build_wff_and<node>(build_wff_always<node>(
				tau::build_wff_or(live_bodies)),
				build_wff_and<node>(upd_sometime));
			LOG_TRACE << "pwr/gate: " << LOG_FM(gate) << "\n";
			const bool plain_ok =
				pwr_memo_sat<node>(gate, start_time, &memo);
			if (!plain_ok && !upd_always) {
				// Without an always part in the update there
				// is no weaker always to fall back on; the
				// accumulated spec is discarded (review B8).
				LOG_WARNING << "Pointwise revision failed; "
					"replacing the accumulated "
					"specification with the update "
					"clause\n";
				tref d = normalize_with_temp_simp<node>(clause);
				if (!d) return {};
				return to_htrefs({ d });
			}
			for (size_t i = 0; i < n; ++i) {
				// Alternative without an always part under an
				// always-free update: it contributes no
				// conjoined body; the clause itself takes its
				// place.
				if (!bodies[i]) {
					new_alts.push_back(
						with_spec_sometimes(clause,
							alt_sometimes[i]));
					continue;
				}
				// Dead alternative: the conjunction with the
				// update is pointwise unsatisfiable.
				if (tau::get(bodies[i]).equals_F()) continue;
				new_alts.push_back(with_spec_sometimes(
					build_wff_and<node>(
						build_wff_always<node>(
							bodies[i]),
						build_wff_and<node>(
							upd_sometime)),
					alt_sometimes[i]));
			}
			if (!plain_ok) {
				// I1: instead of embedding the guarded
				// ¬∃outs.(S∧U) ∨ (S∧U) disjunction -- which
				// copies the whole accumulated spec into the
				// stored formula and doubles it per update --
				// the update clause is appended as a
				// last-resort alternative. step() trying the
				// alternatives in order implements the guard
				// operationally: the update-only alternative
				// fires exactly when no stronger alternative
				// has a solution at the current time point,
				// so the guard formula (and its embedded spec
				// copy) is never built at all.
				LOG_DEBUG << "pwr/plain conjunction unsat; "
					"appending the update clause as a "
					"last-resort alternative\n";
				new_alts.push_back(clause);
			}
		}
		htrefs result = finalize_alternatives(new_alts);
		if (result.empty()) continue;
		return result;
	}
	// No update clause yields a satisfiable revision
	return {};
}

template <NodeType node>
htrefs interpreter<node>::finalize_alternatives(const trefs& alts) {
	// Drop dead alternatives and duplicates (keeping the earliest,
	// i.e. strongest, position).
	trefs result;
	for (tref d : alts) {
		if (tau::get(d).equals_F()) continue;
		bool dup = false;
		for (tref r : result)
			if (tau::subtree_equals(r, d)) {
				dup = true;
				break;
			}
		if (!dup) result.push_back(d);
	}
	// Runtime cap on the alternatives kept per part: retain the
	// strongest prefix and the newest last-resort clause, drop the
	// middle preference tiers (see max_revision_alts).
	if (max_revision_alts && result.size() > max_revision_alts) {
		LOG_WARNING << "Pointwise revision produced "
			<< result.size() << " alternatives; keeping "
			"the first " << max_revision_alts - 1
			<< " and the newest one per "
			"--max-revision-alts\n";
		tref last = result.back();
		result.resize(max_revision_alts - 1);
		result.push_back(last);
	}
	htrefs r;
	r.reserve(result.size());
	for (tref f : result) r.push_back(tree<node>::geth(f));
	return r;
}

// ── step(values, u) — combined step + optional PWR ────────────────────────────

template <NodeType node>
std::pair<std::optional<assignment<node>>, bool>
interpreter<node>::step(const assignment<node>& values, std::optional<tref> u)
{
	auto result = step(values);
	if (u.has_value() && u.value() != nullptr)
		update(u.value());
	return result;
}

// ── current_spec ──────────────────────────────────────────────────────────────

template <NodeType node>
std::string interpreter<node>::current_spec() const {
	// The running spec is the conjunction of all spec partition entries,
	// re-wrapped in always() (the `unsqueeze_always` invariant). This
	// matches what `update()` (update() below) produces when it
	// reports the post-revision spec to the log, and what the routing-
	// clause-driven `this` stream feeds back at line 503-505.
	if (original_spec.empty()) return "T.";
	// IN-M2: a multi-alternative part is executed as its first solvable
	// alternative, so once a step has chosen, report the chosen ones.
	tref combined = unsqueeze_always(
		const_cast<interpreter*>(this)->executed_spec_fm(false));
	return TAU_TO_STR(combined);
}

template <NodeType node>
std::optional<size_t> interpreter<node>::first_solvable_alternative(
	size_t part)
{
	if (part >= step_spec.size()) return {};
	const trefs& part_alts = step_spec[part];
	for (size_t alt_idx = 0; alt_idx < part_alts.size(); ++alt_idx)
		for (tref path : expression_paths<node>(part_alts[alt_idx])) {
			tref current = normalize_non_temp<node>(
				rewriter::replace<node>(update_to_time_point(
					path, formula_time_point), memory));
			if (solution_with_max_update(current)) return alt_idx;
		}
	return {};
}

template <NodeType node>
tref interpreter<node>::executed_spec_fm(bool use_memory) {
	// Parts are parallel between original_spec (alternatives) and
	// step_spec/ubt_ctn (their continuations, same order): a chosen
	// continuation index selects the spec alternative it came from.
	const bool aligned = step_spec.size() == original_spec.size();
	trefs part_fms;
	part_fms.reserve(original_spec.size());
	for (size_t i = 0; i < original_spec.size(); ++i) {
		const htrefs& alts = original_spec[i].first;
		std::optional<size_t> pick;
		if (alts.size() > 1 && aligned) {
			if (use_memory) pick = first_solvable_alternative(i);
			else if (i < chosen_alt_.size()) pick = chosen_alt_[i];
		}
		if (pick && *pick < alts.size())
			part_fms.push_back(alts[*pick]->get());
		else part_fms.push_back(part_alts_fm<node>(alts));
	}
	return tau::build_wff_and(part_fms);
}

// ── reset ─────────────────────────────────────────────────────────────────────

template <NodeType node>
void interpreter<node>::reset() {
	// Clear the execution snapshot; preserve spec / streams / cached_solution.
	memory.clear();
	time_point = 0;
	formula_time_point = 0;
	final_system = false;
	step_spec.clear();
	announced_step_ = -1;
	chosen_alt_.clear();
	last_outputs_.clear();
	// Recompute lookback from ubt_ctn (unchanged) so calculate_initial_spec
	// will rebuild step_spec on the next step().
	compute_lookback_and_initial();
	// AP2-3: make_interpreter pre-populated `memory` with the multi-state
	// Mealy initial one-hot state bits; a reset() that only cleared
	// `memory` lost them, so the first steps after reset missed their
	// state-bit lookback values.
	seed_mealy_initial_state();
	// LA-N3: same for the inner-S auxiliary anchors.
	seed_since_aux_bits();
}

template <NodeType node>
void interpreter<node>::seed_aux_lookback_bits(
	const std::map<std::string, int>& bits)
{
	using tau = tree<node>;
	if (bits.empty() || formula_time_point < 1) return;
	// The carrier type's symbolic true/false, matching build_state_bit_eq
	// and current_state()'s read-side check -- not a numeric constant,
	// which a one-bit carrier normalizes away (see build_state_bit_eq).
	const size_t carrier_tid = get_ba_type_id<node>(
		pack_bool_carrier_type<node>());
	tref bv_one_val  = build_bf_t_type<node>(carrier_tid);
	tref bv_zero_val = build_bf_f_type<node>(carrier_tid);
	// The template variables must be the SPEC'S OWN interned io_var
	// nodes: a freshly parsed `name[t-1]` template can differ structurally
	// from the encoded formula's node (inference context), and then the
	// memory keys derived from it never substitute -- the solver
	// re-chooses the state bits freely at the first step and the one-hot
	// system goes unsat a step later (seen on the qlt F execution,
	// Release). Harvest the lookback occurrences from ubt_ctn instead.
	std::map<std::string, tref> lookback_occ;
	for (const htrefs& part : ubt_ctn) for (const htref& h : part)
		for (tref v : tau::get(h->get()).select_top(
			is_child<node, tau::io_var>)) {
			// v is the `variable` node (transform_io_var's input);
			// the io_var child feeds the name/shift checks.
			tref iov = tau::trim(v);
			const std::string& n = get_var_name<node>(iov);
			if (!bits.contains(n)) continue;
			// Only the lookback ([t-1]) occurrence has the shape
			// transform_io_var expects for the seed key.
			if (is_io_initial<node>(iov)
				|| get_io_var_shift<node>(iov) != 1) continue;
			lookback_occ.emplace(n, v);
		}
	for (const auto& [name, bit] : bits) {
		auto it = lookback_occ.find(name);
		if (it == lookback_occ.end()) {
			// IN-M4: a bit with no shift-(-1) occurrence in the
			// executable spec cannot be seeded — its t = -1 value
			// will be the interpreter's default rather than the
			// strategy's initial state. Today the Mealy encoding
			// mentions every bit at shift -1, so this arm firing
			// means that encoding changed; say so instead of
			// silently skipping.
			LOG_WARNING << "seed_aux_lookback_bits: no lookback "
				"occurrence of state bit '" << name
				<< "' in the executable spec; its initial "
				"value stays at the interpreter default\n";
			continue;
		}
		// transform_io_var(name[t-1], formula_time_point)
		// → name[t = formula_time_point - 1]  (= name[t=0])
		// This is the exact key update_to_time_point produces for the
		// lookback var when processing the G body.
		// bf-wrapped, matching the level the step's own solution
		// entries use: a bare variable-level key is shadowed by a
		// later bf-level entry during replace (the whole bf subtree
		// matches first), which let a step-0 solution override the
		// seeds with zeros.
		tref mem_key = tau::get(tau::bf, {transform_io_var<node>(
			it->second, formula_time_point)});
		memory.emplace(mem_key, bit ? bv_one_val : bv_zero_val);
	}
}

template <NodeType node>
void interpreter<node>::seed_mealy_initial_state() {
	if (!(cached_solution && cached_solution->aut.num_states > 1
			&& formula_time_point >= 1)) return;
	const int k      = cached_solution->aut.num_states;
	const int init_s = cached_solution->aut.initial_state;
	if (init_s < 0 || init_s >= k) return;
	std::map<std::string, int> bits;
	for (int j = 0; j < k; ++j)
		bits.emplace("o__ltl_ms" + std::to_string(j) + "__",
			j == init_s ? 1 : 0);
	seed_aux_lookback_bits(bits);
}

template <NodeType node>
void interpreter<node>::seed_since_aux_bits() {
	// LA-N3: every inner (off-spine) S auxiliary is anchored to 0 at the
	// step before the first enforced one — S(-1) = false, and since a T
	// compiles to a negated S, T(-1) = true is the same seed. Outermost
	// auxiliaries are NOT in this list (their anchor is ψ@0 plus
	// G(curr && rhs); a 0-seed would outlaw their φ-chain).
	std::map<std::string, int> bits;
	for (const std::string& n : since_aux_anchor_) bits.emplace(n, 0);
	seed_aux_lookback_bits(bits);
}

// ── current_state ─────────────────────────────────────────────────────────────

template <NodeType node>
int interpreter<node>::current_state() const {
	// If the spec needed Mealy synthesis (general LTL with future operators),
	// the strategy state is encoded in auxiliary one-hot bits
	// `o__ltl_ms<i>__` per `encode_mealy_as_safety`. Find the unique i with
	// value 1 at the most-recently committed time step.
	if (!cached_solution.has_value()
		|| cached_solution->aut.num_states <= 1) {
		// Pure-safety / pure-past-LTL / single-state strategy: no Mealy
		// state to extract. The interpreter's snapshot identity is
		// fully captured by (time_point, memory) via opaque audit hash;
		// for state ID purposes we report 0 (the only state).
		return 0;
	}

	const int k = cached_solution->aut.num_states;
	// At t=0 (no step yet committed), the encoding leaves the initial
	// state unpinned — at_least + at_most force some ms[i]=1 but none is
	// distinguished. Report the HOA-declared initial_state in this case.
	if (time_point == 0) return cached_solution->aut.initial_state;

	// Find the auxiliary bit set to 1 at the most-recently committed step.
	// `memory` holds the full history; we look up time_point - 1 because
	// time_point has already advanced past the last commit.
	//
	// `build_state_bit_eq<node>(sv[i], 0, true)` in encode_mealy_as_safety
	// builds `o__ltl_ms<i>__[t] = 1` in the carrier type. The committed value
	// in `memory[o__ltl_ms<i>__[t]]` is therefore that type's one for an
	// active state and its zero for an inactive one.
	//
	// The bit is read semantically, not by formatting it: a value written as
	// `bf_t` of the carrier type is the one, and a value written as a
	// constant is asked whether it *is* the one. Comparing a serialization
	// against "1" only worked while the carrier's one happened to print that
	// way, which stops being true as soon as the carrier's width changes.
	const size_t lookup_t = time_point - 1;
	for (int i = 0; i < k; ++i) {
		const std::string aux_name =
			"o__ltl_ms" + std::to_string(i) + "__";
		for (const auto& [var, val] : memory) {
			tref trimmed = tau::trim(var);
			if (get_var_name<node>(trimmed) != aux_name) continue;
			// IN-N12: not every memory entry matching the aux-name
			// pattern is a position-carrying io_var --
			// get_io_time_point indexes [1][0] unguarded and
			// aborts on any other shape, so verify it first.
			tref ivn = io_var_node<node>(trimmed);
			if (!ivn || tau::get(ivn).children_size() < 2
				|| !tau::get(ivn)[1].has_child()) continue;
			if (get_io_time_point<node>(trimmed) != (int_t)lookup_t) continue;
			const auto& v = tau::get(tau::trim(val));
			if (v.is(tau::bf_t)) return i;
			if (v.is_ba_constant()
				&& node::ba::is_one(v.get_ba_constant())) return i;
		}
	}
	// No aux bit set — fall back to HOA initial state (shouldn't happen
	// once t >= 1, but defensive against memory not yet populated for
	// auxiliary variables on certain code paths).
	return cached_solution->aut.initial_state;
}

// ── admissible_outputs ────────────────────────────────────────────────────────

template <NodeType node>
std::vector<assignment<node>>
interpreter<node>::admissible_outputs(size_t max_results)
{
	std::vector<assignment<node>> results;

	// Lazy initialisation: ensure step_spec is populated for time_point.
	// Mirrors what step() does at the top of its body.
	if (!calculate_initial_spec()) return results;
	if (step_spec.empty()) return results;

	// Build the working spec form from step_spec, also substituting memory
	// (mirrors step()'s line 515-520 update_to_time_point + replace).
	// We use memory-substituted, time-updated step_spec parts so that the
	// solver sees the same constraints step() would.
	tref current_form = tau::_T();
	for (size_t i = 0; i < step_spec.size(); ++i) {
		const trefs& part_alts = step_spec[i];
		// IN-M2: step() executes a part as its FIRST solvable
		// alternative under the current memory; the admissible set is
		// that alternative's. The disjunction is only the fallback
		// when none is solvable (the step would fail anyway).
		tref part = part_alts.size() == 1 ? part_alts[0] : nullptr;
		if (!part)
			if (auto pick = first_solvable_alternative(i))
				part = part_alts[*pick];
		if (!part) part = tau::build_wff_or(part_alts);
		tref updated = update_to_time_point(part, formula_time_point);
		updated = rewriter::replace<node>(updated, memory);
		updated = normalize_non_temp<node>(updated);
		current_form = tau::build_wff_and(current_form, updated);
	}

	// Determine the BA type for blocking-clause _0 construction. We use
	// the type of any output variable as a representative — all output
	// vars in a single spec partition share the canonical splitter type.
	// Per-variable type is looked up via ctx.type_of(var) inside the loop.

	while (results.size() < max_results) {
		solver_options opts = {
			.splitter_one = node::ba::splitter_one(tau_type<node>()),
			.mode = solver_mode::general
		};
		bool err = false;
		auto sol_opt = solve<node>(current_form, opts, err);
		if (err) break;
		if (!sol_opt) break;

		// Filter out aux/excluded outputs (o__ltl_ms*, o__ltl_s*, _e*, _f*)
		// per the codebase's is_excluded_output convention.
		assignment<node> filtered;
		for (const auto& [var, val] : sol_opt.value()) {
			if (tt(var) | tau::variable | tau::io_var) {
				if (is_excluded_output(tau::trim(var))) continue;
			}
			filtered.emplace(var, val);
		}
		results.push_back(std::move(filtered));

		// Build blocking clause: ∨_i ((var_i ⊕ val_i) ≠ 0)
		// over all variables in the FULL solution (not the filtered one),
		// so we don't accept the same internal assignment again.
		tref block = tau::_F();
		bool block_has_term = false;
		for (const auto& [var, val] : sol_opt.value()) {
			tref xored = build_bf_xor<node>(var, val);
			tref eq0 = tau::build_bf_eq_0(xored);
			tref neq = tau::build_wff_neg(eq0);
			block = tau::build_wff_or(block, neq);
			block_has_term = true;
		}
		if (!block_has_term) break;  // empty solution; nothing to block
		current_form = tau::build_wff_and(current_form, block);
	}
	return results;
}

// ── accumulator_state ─────────────────────────────────────────────────────────

template <NodeType node>
std::string interpreter<node>::accumulator_state(const std::string& name) const
{
	// Per paper §5.4 Def 5.7, accumulators are spec-language variables
	// realised as output streams. Their committed values live in `memory`
	// at the most-recent time step.
	if (memory.empty()) return "";
	const size_t lookup_t = time_point > 0 ? time_point - 1 : 0;

	// Try both bare-name and `acc_<name>` conventions; spec authors may use
	// either prefix.
	const std::string candidates[] = {name, "acc_" + name};

	for (const auto& candidate : candidates) {
		for (const auto& [var, val] : memory) {
			tref trimmed = tau::trim(var);
			if (get_var_name<node>(trimmed) != candidate) continue;
			if (get_io_time_point<node>(trimmed) != (int_t)lookup_t)
				continue;
			std::stringstream ss;
			size_t ctype = ctx.type_of(trimmed);
			if (ctype == 0) continue;
			if (!serialize_constant<node>(ss, val, ctype)) continue;
			std::string s = ss.str();
			while (!s.empty() && (s.back() == ' ' || s.back() == '\n'))
				s.pop_back();
			return s;
		}
	}
	return "";
}

// ── visualise_mealy_dot ───────────────────────────────────────────────────────

template <NodeType node>
std::string interpreter<node>::visualise_mealy_dot() const {
	if (!cached_solution.has_value() || cached_solution_stale_) return "";
	const auto& aut = cached_solution->aut;

	auto escape = [](const std::string& s) {
		std::string out;
		out.reserve(s.size() + 8);
		for (char c : s) {
			if (c == '"' || c == '\\') out.push_back('\\');
			out.push_back(c);
		}
		return out;
	};

	std::stringstream ss;
	ss << "digraph mealy {\n";
	ss << "  rankdir=LR;\n";
	ss << "  node [shape=circle];\n";
	// Invisible initial-state pointer.
	ss << "  __init [shape=none, label=\"\"];\n";
	ss << "  __init -> " << aut.initial_state << ";\n";
	for (int s = 0; s < aut.num_states; ++s) {
		ss << "  " << s << " [label=\"q" << s;
		if (s < (int)aut.state_accepting.size() && aut.state_accepting[s])
			ss << " (acc)";
		ss << "\"];\n";
	}
	// AP legend as a comment so consumers know what the guard tokens
	// "0", "1", "2" reference.
	if (!aut.aps.empty()) {
		ss << "  // APs:";
		for (size_t i = 0; i < aut.aps.size(); ++i)
			ss << " " << i << "=" << aut.aps[i];
		ss << "\n";
	}
	for (int s = 0; s < (int)aut.edges.size(); ++s) {
		for (const auto& e : aut.edges[s]) {
			ss << "  " << s << " -> " << e.dst << " [label=\""
			   << escape(e.guard_label);
			if (e.accepting) ss << " (acc)";
			ss << "\"];\n";
		}
	}
	ss << "}\n";
	return ss.str();
}

// ── determinise ───────────────────────────────────────────────────────────────

template <NodeType node>
HoaAutomaton interpreter<node>::determinise() const {
	if (!cached_solution.has_value() || cached_solution_stale_)
		return HoaAutomaton{};
	return cached_solution->aut;
}

// ── boundary_traces ───────────────────────────────────────────────────────────

template <NodeType node>
std::vector<std::vector<int>>
interpreter<node>::boundary_traces(int n, int max_length) const {
	std::vector<std::vector<int>> traces;
	if (n <= 0 || max_length <= 0) return traces;
	if (!cached_solution.has_value() || cached_solution_stale_) return traces;
	const auto& aut = cached_solution->aut;
	if (aut.num_states == 0) return traces;

	// DFS from initial_state, collect simple paths (no repeated state)
	// up to max_length. Then sort by length (longest first) and take n.
	//
	// "Simple path" rules out cycles, which is what we want for "extremal
	// before-it-loops" traces. A separate cycle-discovery pass could be
	// added later if the canonical §13.2 "longest delay before eventually
	// fires" semantics requires it (cycles are the natural representation
	// of unbounded delay).
	std::vector<std::vector<int>> all_paths;
	std::vector<int> stack;
	std::vector<bool> on_stack(aut.num_states, false);
	// AP2-9: initial_state is untrusted elsewhere (make_interpreter range
	// checks it); an out-of-range value would overrun on_stack.
	if (aut.initial_state < 0
		|| aut.initial_state >= (int) aut.num_states) return {};

	std::function<void(int, int)> dfs = [&](int u, int depth) {
		stack.push_back(u);
		on_stack[u] = true;

		// Always record current path (non-empty).
		all_paths.push_back(stack);

		// Stop deepening at max_length or if no outgoing edges.
		if (depth + 1 < max_length
		    && u < (int)aut.edges.size()
		    && !aut.edges[u].empty()) {
			for (const auto& e : aut.edges[u]) {
				if (e.dst < 0 || e.dst >= aut.num_states) continue;
				if (on_stack[e.dst]) continue;  // skip cycles
				dfs(e.dst, depth + 1);
			}
		}

		on_stack[u] = false;
		stack.pop_back();
	};

	dfs(aut.initial_state, 0);

	// Sort: longest first; ties by lexicographic state tuple.
	std::sort(all_paths.begin(), all_paths.end(),
		[](const std::vector<int>& a, const std::vector<int>& b) {
			if (a.size() != b.size()) return a.size() > b.size();
			return a < b;
		});
	if ((int)all_paths.size() > n)
		all_paths.resize(n);
	return all_paths;
}

// ── commit_realiser ───────────────────────────────────────────────────────────

template <NodeType node>
void interpreter<node>::commit_realiser(const std::string& approval_hash) {
	committed_approval_hash = approval_hash;
}

// ── declare_open / undeclare_open / open_streams ────────────────────────────
//
// as-is and validated at step() time when the engine attempts to dispatch
// the handler. Re-entrance check refuses to declare from inside a handler
// invocation (would mutate dispatch table mid-call).

template <NodeType node>
void interpreter<node>::declare_open(const std::string& stream_name,
                                     OracleHandler handler)
{
	if (in_oracle_handler_) {
		throw std::runtime_error(
			"declare_open: re-entrance violation — cannot declare from "
			"inside an OracleHandler invocation (stream: " + stream_name + ")");
	}
	// The declared contract: throw if stream_name is not an output stream
	// of the current spec. Silently accepting a typo'd name would mean
	// the handler simply never fires.
	bool known = false;
	for (const auto& [var, _] : outputs)
		if (get_var_name<node>(var) == stream_name) {
			known = true;
			break;
		}
	if (!known) throw std::runtime_error(
		"declare_open: '" + stream_name
		+ "' is not an output stream of the current spec");
	if (open_handlers_.find(stream_name) == open_handlers_.end()) {
		open_streams_order_.push_back(stream_name);
	}
	open_handlers_[stream_name] = std::move(handler);
}

template <NodeType node>
void interpreter<node>::undeclare_open(const std::string& stream_name)
{
	if (in_oracle_handler_) {
		throw std::runtime_error(
			"undeclare_open: re-entrance violation — cannot undeclare from "
			"inside an OracleHandler invocation (stream: " + stream_name + ")");
	}
	auto it = open_handlers_.find(stream_name);
	if (it == open_handlers_.end()) return;  // no-op
	open_handlers_.erase(it);
	auto vit = std::find(open_streams_order_.begin(),
	                     open_streams_order_.end(), stream_name);
	if (vit != open_streams_order_.end()) open_streams_order_.erase(vit);
}

template <NodeType node>
std::vector<std::string> interpreter<node>::open_streams() const
{
	return open_streams_order_;
}

// ── can_extend ────────────────────────────────────────────────────────────────

template <NodeType node>
std::optional<assignment<node>> interpreter<node>::solution_with_max_update(
	tref spec)
{
	// using tau = tree<node>;
	// DBG(LOG_TRACE << "solution_with_max_update/spec: " << LOG_FM_DUMP(spec) << "\n";)
	auto get_solution = [](const auto& fm) {
		// DBG(LOG_TRACE << "get_solution/fm: " << LOG_FM_DUMP(fm) << "\n";)
		// setting proper options for the solver
		solver_options options = {
			.splitter_one = node::ba::splitter_one(tau_type<node>()),
			.mode = solver_mode::general
		};
		// solve the given system of equations
		bool solve_error = false;
		std::optional<solution<node>> s = solve<node>(fm, options, solve_error);
		if (solve_error) {
			LOG_ERROR << "Internal error in solver\n";
			return std::optional<solution<node>>();
		}
// #ifdef DEBUG
// 		if (s) for (auto [k, v] : s.value()) LOG_TRACE
// 			<< "get_solution/solution: \n\t\tkey: " << LOG_FM_DUMP(k) << "\n\t\tvalue: " << LOG_FM_DUMP(v);
// #endif // DEBUG
		return s;
	};
	tref u = build_out_var_at_n<node>("u", time_point,
		tau_type_id<node>());
	auto is_u_stream = [&u](const auto& n) {
		return n == u;
	};
	for (tref path : expression_paths<node>(spec)) {
		// Find update stream in clause
		tref update = tau::get(path).find_top(is_u_stream);
		// If there is no update in clause
		if (!update) continue;

		// Obtain single f = 0 part of clause
		tref f = squeeze_positives<node>(path, tau_type_id<node>());
		// If no positive parts exists, the update cannot be maximized
		if (!f) continue;

		// Check that f is wide (not 0 and has more than one zero), otherwise continue
		f = bf_reduced_dnf<node>(f);
		if (tau::get(f).equals_0()) continue;
		tref f0 = rewriter::replace<node>(f, u, tau::_0(
			get_ba_type_id<node>(tau_type<node>())));
		tref f1 = rewriter::replace<node>(f, u, tau::_1(
			get_ba_type_id<node>(tau_type<node>())));
		tref f0_xor_f1 = bf_reduced_dnf<node>(build_bf_xor<node>(f0, f1));
		if (tau::get(f0_xor_f1).equals_0()
			|| tau::get(f0_xor_f1).equals_1()) continue;

		// Here we know that f is wide
		tref max_u = build_bf_neg<node>(f1);
		tref max_u_spec = rewriter::replace<node>(path, u, max_u);
		auto sol = get_solution(max_u_spec);
		if (!sol.has_value()) continue;
		// Now we need to add solution for u[t]
		max_u = rewriter::replace<node>(max_u, sol.value());
		max_u = bf_reduced_dnf<node>(
			replace_free_vars_by<node>(max_u,
			tau::_0_trimmed(find_ba_type<node>(max_u))));
		sol.value().emplace(u, max_u);
		return sol;
	}
	// In case there is no maximal solution for u on any path of spec
	return get_solution(spec);
}

template <NodeType node>
bool interpreter<node>::is_excluded_output(tref var) {
	if (tau::get(var).is_input_variable()) return false;
	const std::string& io_name = get_var_name<node>(var);
	if (io_name.size() > 9 && io_name.substr(0, 9) == "o__ltl_ms") return true;
	if (io_name.size() > 8 && io_name.substr(0, 8) == "o__ltl_s") return true;
	// IN-R6: CTL* witness outputs (w_<n>) are internal encoding
	// artefacts like the aux state bits — solved every step, never
	// printed. The prefix is reserved (user stream definitions named
	// `w_...` are rejected at definition time).
	if (io_name.size() > 2 && io_name[0] == 'w' && io_name[1] == '_')
		return true;
	return io_name[0] == '_' && io_name.size() > 1 &&
		(io_name[1] == 'e' || io_name[1] == 'f');
}

template <NodeType node>
trefs interpreter<node>::appear_within_lookback(const trefs& vars){
	trefs appeared;
	for (size_t t = time_point; t <= time_point + (size_t)lookback; ++t) {
		for (const htrefs& part : ubt_ctn) for (const auto& h : part) {
			tref step_ubt_ctn = update_to_time_point(h->get(),
				t < formula_time_point ? formula_time_point : t);
			step_ubt_ctn = rewriter::replace<node>(step_ubt_ctn, memory);
			// We only apply a heuristic in order to decide if the variable still appears
			step_ubt_ctn = syntactic_formula_simplification<node>(step_ubt_ctn);
			// Try to find var in step_ubt_ctn
			for (tref v : vars) {
				if (contains<node>(step_ubt_ctn, v))
					if (std::ranges::find_if(
						appeared, [&v](const auto& n) {
							return tau::get(n) == tau::get(v);
						}) == appeared.end())
						appeared.emplace_back(v);
			}
		}
	}
	return appeared;
}

template <NodeType node>
tref interpreter<node>::unsqueeze_always(tref cnf_expression) {
	// Squeeze always statements again
	trefs clauses = get_cnf_wff_clauses<node>(cnf_expression);
	trefs aw_clauses;
	for (tref& c : clauses) {
		if (tau::get(c).child_is(tau::wff_always)) {
			aw_clauses.push_back(tau::trim2(c));
			c = tau::_T();
		}
	}
	// B6: fold via always_conjunction instead of a verbatim
	// build_wff_and of the bodies -- clauses with different lookbacks
	// must be shifted to a common frame before they share one always,
	// exactly as always_conjunction (used by the normalizer and by
	// pointwise_revision) does.
	tref aw_body = nullptr;
	for (tref b : aw_clauses)
		aw_body = aw_body ? always_conjunction<node>(aw_body, b) : b;
	return tau::build_wff_and(
		tau::build_wff_always(aw_body ? aw_body : tau::_T()),
		tau::build_wff_and(clauses));
}

template <NodeType node>
tref unpack_tau_constant(tref constant) {
	using tau = tree<node>;
	const auto& c = tree<node>::get(tau::trim(constant));
	if (!c.is_ba_constant()) return {};
	tref main = node::ba::unpack_tau_ba(c.get_ba_constant());
	return main;
}

// B5 diagnostic: a rule arriving on a tau-typed input stream while the
// update stream u solves to 0 vanishes silently -- the update trigger never
// fires (solution_with_max_update cannot always maximize u, e.g. during the
// initial segment; old review §2.9). Warn so the drop is visible instead of
// the caller believing the rule was applied.
template <NodeType node>
void warn_if_update_dropped(interpreter<node>& i,
	const assignment<node>& output)
{
	using tau = tree<node>;
	tref update_stream = build_out_var_at_n<node>("u", i.time_point - 1,
		get_ba_type_id<node>(tau_type<node>()));
	if (size_t t = i.ctx.type_of(update_stream);
		t == 0 || t != get_ba_type_id<node>(tau_type<node>()))
		return;
	if (auto it = output.find(update_stream); it != output.end()
		&& !tau::get(it->second).equals_0())
		return; // the update fired; nothing was dropped
	for (const auto& [var, val] : i.memory) {
		if (!tau::get(var).is(tau::bf)) continue;
		tref v = tau::trim(var);
		const tau& tv = tau::get(v);
		if (!tv.is(tau::variable) || !tv.child_is(tau::io_var)
			|| !tv[0].is_input_variable()) continue;
		if (!is_io_initial<node>(v) || get_io_time_point<node>(v)
			!= (int_t)i.time_point - 1) continue;
		if (i.ctx.type_of(var)
			!= get_ba_type_id<node>(tau_type<node>())) continue;
		tref rule = unpack_tau_constant<node>(val);
		if (!rule || tau::get(rule).equals_F()) continue;
		// A rule constrains streams; a plain value does not.
		if (!tau::get(rule).find_top(is_child<node, tau::io_var>))
			continue;
		LOG_WARNING << "An update rule was provided on input stream "
			<< get_var_name<node>(v) << " but the update stream u "
			"solved to 0; the rule was not applied\n";
		return;
	}
}

// returns true if there is a free variable in formula fm
// prints error messages by default
template <NodeType node>
bool has_free_vars(tref fm, bool silent) {
	using tau = tree<node>;
	const trefs& free_vars = get_free_vars<node>(fm);
	if (!free_vars.empty()) {
		// all elements of the set must be quantified
		std::stringstream ss; bool has_real_free_vars = false;
		for (auto it = free_vars.begin(), end = free_vars.end(); it != end; ++it) {
			if (is_child<node>(*it, tau::io_var)) {
				const tau& io_var_node = tau::get(*it)[0];
				if (       !io_var_node.is_input_variable()
					&& !io_var_node.is_output_variable())
				{
					if (!silent) LOG_ERROR << "The stream "
						<< io_var_node << " is not "
						<< "defined as an input or "
						<< "output stream";
					return true;
				}
			} else if (!is_child<node>(*it, tau::uconst_name)) {
				ss << tau::get(*it) << " ";
				has_real_free_vars = true;
			}
		}
		if (has_real_free_vars) {
			if (!silent) LOG_ERROR << "The following variable(s) must be "
				<< "quantified and cannot appear free: "
				<< ss.str();
			return true;
		}
	}
	return false;
}

template <NodeType node>
std::optional<interpreter<node>> run(tref form, const io_context<node>& ctx,
	const size_t steps)
{
	DBG(LOG_TRACE << "run begin\n";
		LOG_TRACE << "run[form]: " << LOG_FM(form);
		LOG_TRACE << "run[steps]: " << steps;)

	using tau = tree<node>;
	// Clear global definitions to avoid type-registry contamination from
	// previous runs (e.g. sequential test cases sharing the singleton).
	definitions<node>::instance().clear();
	DBG(LOG_TRACE << "run[form]: " << LOG_FM(form));
	auto intrprtr_o = interpreter<node>::make_interpreter(form, ctx);
	if (!intrprtr_o) return {};
	auto& intrprtr = intrprtr_o.value();

	LOG_INFO << "-----------------------------------------------------------------------------------------------------------";
	LOG_INFO << "Please provide requested input, or press ENTER to terminate                                               |";
	LOG_INFO << "If no input is requested, press ENTER to continue to the next execution step, or type q(uit) to terminate |";
	LOG_INFO << "-----------------------------------------------------------------------------------------------------------\n\n";

	// Continuously perform execution step until user quits
	while (true) {
		auto [output, auto_continue] = intrprtr.step();

		DBG(LOG_TRACE << "run[output]: ";
			if (output.has_value()) {
				LOG_TRACE << "{ ";
				for (const auto& [v, val] : output.value())
					LOG_TRACE << LOG_FM_DUMP(v) << " := " << LOG_FM_TREE(val) << "; ";
				LOG_TRACE << "}\n";
			} else LOG_TRACE << "no output\n";
			LOG_TRACE << "run[auto_continue]: " << auto_continue << "\n";
		)

		// If the user provided empty input for an input stream, quit
		if (!output.has_value()) break;
		if (!intrprtr.write(output.value())) break;
		// If there is no input, ask the user if execution should continue
		if (!auto_continue && steps == 0) {
			std::string line;
			term::enable_getline_mode();
			std::getline(std::cin, line);
			term::disable_getline_mode();
			// On closed/exhausted stdin, getline never sets line to "q"/
			// "quit" and keeps returning immediately, spinning this loop
			// forever; stop the same way repl_evaluator::run_cmd does.
			if (std::cin.eof() || std::cin.fail()) { std::cin.clear(); break; }
			if (line == "q" || line == "quit") break;
		} else std::cout << "\n";

		// Update interpreter in case the output stream u is present and unequal to 0
		auto update_stream = build_out_var_at_n<node>(
			"u", intrprtr.time_point - 1, get_ba_type_id<node>(tau_type<node>()));
		// Update only if u is of type tau
		if (size_t t = intrprtr.ctx.type_of(update_stream);
			t != 0 && t == get_ba_type_id<node>(tau_type<node>()))
		{
			auto it = output.value().find(update_stream);
			if (it != output.value().end() && !tau::get(it->second)
				.equals_0())
			{
				if (tref update = unpack_tau_constant<node>(
					it->second); update != nullptr)
				{
					intrprtr.update(update);
				}
			}
		}
		warn_if_update_dropped(intrprtr, output.value());
		if (steps != 0 && intrprtr.time_point == steps) break;
	}
	DBG(LOG_TRACE << "run end\n";)
	return intrprtr_o;
}

template <NodeType node>
bool interpreter<node>::collect_input_streams(tref dnf,
	subtree_map<node, size_t>& current_inputs)
{
	using tau = tree<node>;
	// select current input variables
	auto is_in_var = [](tref n) {
		const tau& tn = tau::get(n);
		if (tn.is(tau::variable)) return tn[0].is_input_variable();
		return false;
	};
	trefs in_vars = tau::get(dnf).select_all(is_in_var);
	for (tref var_node : in_vars) {
		size_t type_id = tau::get(var_node).get_ba_type();
		DBG(LOG_TRACE << "collect_input_streams[var_node]: " << LOG_FM_DUMP(var_node) << "\n";)
		if (type_id == 0) {
			TAU_LOG_ERROR << "The following input stream must be typed: "
				<< tau::get(var_node).to_str() << "\n";
			return false;
		}
		tref var = canonize<node>(var_node);
		// size_t var_sid = get_var_name_sid<node>(var);
		// update current input streams by known stream id
		if (auto it = ctx.inputs.find(var); it != ctx.inputs.end())
			current_inputs[var] = it->second;
		else {
			// or default to console input stream
			DBG(LOG_TRACE << "collect_input_streams[adding default input console]: "
				<< get_var_name<node>(var) << " "
				<< LOG_BA_TYPE(type_id);)
			ctx.add_input_console(get_var_name<node>(var), type_id);
			current_inputs[var] = ctx.inputs.find(var)->second;
		}
	}
	return true;
}

template<NodeType node>
subtree_map<node, size_t> interpreter<node>::collect_input_streams(tref dnf) {
	subtree_map<node, size_t> current_inputs;
	if (collect_input_streams(dnf, current_inputs))
		return current_inputs;
	else return {};
}

template <NodeType node>
bool interpreter<node>::collect_output_streams(tref dnf,
	subtree_map<node, size_t>& current_outputs)
{
	using tau = tree<node>;
	// select current output variables
	auto is_out_var = [](tref n) {
		const tau& tn = tau::get(n);
		if (tn.is(tau::variable)) return tn[0].is_output_variable();
		return false;
	};
	trefs out_vars = tau::get(dnf).select_all(is_out_var);
	for (tref var_node : out_vars) {
		// IN-N2: the LTL aux state bits (o__ltl_ms*, o__ltl_s*) are
		// encoding artefacts, not user streams; registering them here
		// made them console outputs that the complete-outputs loop in
		// step() then emitted as default zeros.
		if (is_excluded_output(var_node)) continue;
		size_t type_id = tau::get(var_node).get_ba_type();
		DBG(LOG_TRACE << "collect_output_streams[var_node]: " << LOG_FM_DUMP(var_node) << "\n";)
		if (type_id == 0) {
			TAU_LOG_ERROR << "The following output stream must be typed: "
				<< tau::get(var_node).to_str() << "\n";
			return false;
		}
		tref var = canonize<node>(var_node);
		// size_t var_sid = get_var_name_sid<node>(var);
		// update current output streams by known stream id
		if (auto it = ctx.outputs.find(var); it != ctx.outputs.end())
			current_outputs[var] = it->second;
		else {
			// or default to console output stream
			DBG(LOG_TRACE << "collect_output_streams[adding default output console]: "
				<< get_var_name<node>(var) << " "
				<< LOG_BA_TYPE(type_id);)
			// An internal stream still needs registering -- the outputs map
			// is what makes it solvable -- but not a *labelled* console
			// stream: that label widens the name column shared by every
			// other output, and write() never emits these anyway.
			if (is_excluded_output(var))
				ctx.add_output(get_var_name<node>(var), type_id,
					std::make_shared<vector_output_stream>());
			else ctx.add_output_console(get_var_name<node>(var), type_id);
			current_outputs[var] = ctx.outputs.find(var)->second;
		}
	}
	return true;
}

template<NodeType node>
subtree_map<node, size_t> interpreter<node>::collect_output_streams(tref dnf) {
	subtree_map<node, size_t> current_outputs;
	if (collect_output_streams(dnf, current_outputs))
		return current_outputs;
	else return {};
}

template <NodeType node>
std::ostream& interpreter<node>::dump(std::ostream& os) const {
	os << "\n" << TC.GREEN() << "=== Interpreter ===" << TC.CLEAR() << "\n";
	os << "Time point:      " << time_point << "\n";
	os << "Inputs:         ";
	if (ctx.inputs.empty()) os << " none";
	for (const auto& [var, _] : ctx.inputs) os << " " << get_var_name<node>(var->get());
	os << "\n";
	os << "Outputs:        ";
	if (ctx.outputs.empty()) os << " none";
	for (const auto& [var, _] : ctx.outputs) os << " " << get_var_name<node>(var->get());
	os << "\n";
	os << "Current inputs: ";
	if (inputs.empty()) os << " none";
	for (const auto& [var, _] : inputs) os << " " << get_var_name<node>(var);
	os << "\n";
	os << "Current outputs:";
	if (outputs.empty()) os << " none";
	for (const auto& [var, _] : outputs) os << " " << get_var_name<node>(var);
	os << "\n";
	os << "Memory:         ";
	if (memory.empty()) os << " empty";
	os << "\n";
	for (const auto& [k, v]: memory)
		os //<< "\t" << k << " := " << v
			<< "\t" << LOG_FM(k) << " := " << LOG_FM(v) << "\n";
	return os << "\n";
}

template <NodeType node>
std::string interpreter<node>::dump_to_str() const {
	std::stringstream ss;
	return dump(ss), ss.str();
}

} // namespace idni::tau_lang
