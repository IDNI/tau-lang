// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <chrono>
#include <string>
#ifdef __linux__
#include <malloc.h>
#endif

#include "ba_types.h"
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

		auto it = inputs.find(canonize<node>(var));
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
		auto maybe_line = it->second->get(time_point); // get a value from input stream
		if (!maybe_line.has_value()) {
			LOG_ERROR
				<< "Failed to read from input stream '"
				<< get_var_name<node>(var) << "'\n";
			DBG(LOG_TRACE << "read[result]: {}\n"
				<< "read end\n";)
		       return {};
		}
		std::string line = maybe_line.value();
		// std::cout << line << "\n"; // ???

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
bool interpreter<node>::serialize_constant(std::stringstream& ss,
	tref constant, size_t type) const
{
	DBG(LOG_TRACE << "serialize_constant[constant]: " << LOG_FM_DUMP(constant) << "\n";)
	auto value = tt(constant) | tau::ba_constant;
	if (!value) {
		// is bf_t
		if (auto check = tt(constant) | tau::bf_t; check)
			ss << node::ba::one(get_ba_type_tree<node>(type));
		// is bf_f
		else if (auto check = tt(constant) | tau::bf_f; check)
			ss << node::ba::zero(get_ba_type_tree<node>(type));
		// is something else but not a BA element
		else return false;
	} else ss << (value | tt::ba_constant);
	DBG(LOG_TRACE << "serialize_constant[ss]: " << ss.str() << "\n";)
	return true;
}

template <NodeType node>
bool interpreter<node>::write(const assignment<node>& output_values) {
	// Sort variables in output by time
	trefs io_vars;
	for (const auto& [var, _ ] : output_values) {
		// DBG(LOG_TRACE << "io var: " << LOG_FM_TREE(var));
		// DBG(LOG_TRACE << "io var dump: " << LOG_FM_DUMP(var));
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
		auto value = tt(output_values.find(io_var)->second) | tau::ba_constant;
		std::stringstream ss;
		if (!serialize_constant(ss, output_values.find(io_var)->second,
			ctx.type_of(vn)))
		{
			LOG_ERROR << "No Boolean algebra element assigned to "
				"output '" << TAU_TO_STR(io_var) << "'";
				return false;
		}
		auto it = outputs.find(vn);
		if (it == outputs.end()) {
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
	// Find a satisfiable unbound continuation from spec
	spec = normalizer<node>(spec);
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
			memory, ctx };

		// rebuild io streams according to the spec
		subtree_map<node, size_t> output_streams;
		if (!i.collect_output_streams(spec, output_streams)) return {};
		LOG_TRACE << "interpreter::make_interpreter/rebuild_outputs";
		if (!i.rebuild_outputs(output_streams)) return {};
		subtree_map<node, size_t> input_streams;
		if (!i.collect_input_streams(spec, input_streams)) return {};
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

template <NodeType node>
std::pair<std::optional<assignment<node>>, bool>
	interpreter<node>::step(const assignment<node>& values)
{
	bool auto_continue = false;
	// Save inputs in memory
	for (const auto& [var, value] : values) {
		DBG(LOG_TRACE << "step[var]: " << LOG_FM_DUMP(var);)
		assert(get_io_time_point<node>(tau::trim(var)) <= (int_t)time_point);
		// If there is at least one input, continue automatically in execution
		auto_continue = true;
		memory[var] = value;
	}
	bool has_this_stream = false;
	for (auto& [var, _] : inputs) {
		if (get_var_name<node>(var) == "this"
			&& ctx.type_of(var) == get_ba_type_id<node>(
							tau_type<node>()))
		{
			has_this_stream = true;
			break;
		}
	}
	DBG(LOG_TRACE << "step/has_this_stream: " << has_this_stream << "\n";)
	// If the "this" input stream is present, write the current spec into it
	if (has_this_stream) {
		tref current_this_stream = build_in_var_at_n<node>(
			"this", time_point, get_ba_type_id<node>(tau_type<node>()));
		tref wrapped_spec = build_bf_ba_constant<node>(
			node::ba::pack_tau_ba(unsqueeze_always(
				combined_spec_fm<node>(original_spec))),
				get_ba_type_id<node>(tau_type<node>()));
		memory[current_this_stream] = wrapped_spec;
	}

	solution<node> global;
	for (const trefs& part_alts : step_spec) {
		// Try the part's alternatives in their preference order; the
		// first one with a solvable path wins. This trial order is what
		// implements the pointwise-revision preference for the
		// accumulated spec over its fallback updates (I1) -- a later
		// alternative only fires when no earlier one is solvable at
		// this time point.
		bool solved = false;
		for (tref spec_part : part_alts) {
		for (tref path : expression_paths<node>(spec_part)) {
			// rewriting the inputs and inserting them into memory
			tref updated = update_to_time_point(path, formula_time_point);
			// TODO: Check why constant time positions are not being replaced
			tref current = rewriter::replace<node>(updated, memory);
			// Simplify after updating stream variables
			// TODO: Maybe replace by syntactic simp?
			current = normalize_non_temp<node>(current);
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
				for (const auto& [var, value] : path_solution.value()) {
					// Check if we are dealing with a stream variable
					if (tt(var) | tau::variable | tau::io_var) {
						DBG(LOG_TRACE << LOG_FM_TREE(value));
						assert(tau::get(value).is(tau::bf));
						if (get_io_time_point<node>(tau::trim(var)) <= (int_t)time_point) {
							// std::cout << "time_point: " << time_point << "\n";
							// std::cout << "var: " << var << "\n";
							memory.emplace(var, value);
							// Exclude temporary streams in solution
							if (!is_excluded_output(tau::trim(var)))
								global.emplace(var, value);
						}
					} else {
						memory.emplace(var, value);
						global.emplace(var, value);
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
		bool is_bv = is_bv_type_family<node>(ctype);
		tref ot = build_out_var_at_n<node>(get_var_name_node<node>(o), time_point, ctype);
		if (auto it = global.find(ot); it == global.end()) {
			if (is_bv) {
				auto zero_bitvector = make_bitvector_bottom_elem(
					get_bv_size<node>(get_ba_type_tree<node>(ctype)));
				auto zero_term = tau::get(tau::bf, {
					tau::get_ba_constant(zero_bitvector, ctype)});
				memory.emplace(ot, zero_term);
				global.emplace(ot, zero_term);
			} else {
				memory.emplace(ot, tau::_0(ctype));
				global.emplace(ot, tau::_0(ctype));
			}
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
	maybe_gc();
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
		tref v = tau::trim(kv.first);
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
	output_partition.collect_live_refs(keep);
}

template <NodeType node>
void interpreter<node>::maybe_gc() {
	if constexpr (gc_growth_factor <= 0.0) return;
	const size_t m_pre = tau::m_size();
	// Floor: don't sweep until M is non-trivially large. Bounds peak size
	// for any workload (a sweep fires no later than M = gc_min_size + growth).
	if (m_pre < gc_min_size) return;
	// Amortize: don't sweep unless M grew enough since the last sweep.
	if ((double)m_pre < gc_growth_factor * (double)m_at_last_gc) return;

	const auto t0 = std::chrono::steady_clock::now();
	std::unordered_set<tref> keep;
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
	LOG_TRACE << "calculate_initial_systems begin \n";
	if (final_system) return true;

	size_t initial_segment = std::max(highest_initial_pos, (int_t)formula_time_point);
	LOG_TRACE << "calculate_initial_systems[initial_segment]: " << initial_segment << "\n";
	LOG_TRACE << "calculate_initial_systems[time_point]: " << time_point << "\n";
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
bool interpreter<node>::is_memory_access_valid(const auto& io_vars)
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
	htrefs kept;
	ctns.clear();
	for (size_t idx = 0; idx < alts.size(); ++idx) {
		const htref& alt = alts[idx];
		if (idx != 0 && idx != alts.size() - 1) {
			const tau& c = tau::get(alt->get());
			tref aw = c.find_top(is_child<node, tau::wff_always>);
			if (aw && !c.find_top(
					is_child<node, tau::wff_sometimes>)
				&& !c.find_top(is<node, tau::uconst>))
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
				&& !c.find_top(is<node, tau::uconst>))
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
// bv-free formulas because is_tau_impl/are_tau_equivalent can hang even on
// SMALL mixed lookback bv pairs (observed on bv_stress_check inputs of
// ~100-200 chars; recorded as B11 in
// private/review-pointwise-revision-2026-08-16.md).
template <NodeType node>
static bool pwr_contains_bv_content(tref f) {
	return tree<node>::get(f).find_top(
		is_tref_bv_type_family<node>) != nullptr;
}

template <NodeType node>
void interpreter<node>::update(tref update) {
	DBG(LOG_TRACE << "interpreter::update(update = \"" << LOG_FM(update) << "\")";)
	// TODO: shift spec time according to new lookback from update
	trefs io_vars = tau::get(update)
				.select_top(is_child<node, tau::io_var>);
	// the constant time positions in update are seen relative to
	// time_point, i.e. time point 0 is shifted to time_point
	tref shifted_update = shift_const_io_vars_in_fm<node>(
						update, io_vars, time_point);
	if (tau::get(shifted_update).equals_F()) {
		LOG_WARNING << "No update performed: constant time position below 0 was found\n";
		return;
	}
	io_vars = tau::get(shifted_update)
				.select_top(is_child<node, tau::io_var>);
	if (!is_memory_access_valid(io_vars)) {
		LOG_WARNING << "No update performed: invalid memory access was found\n";
		return;
	}
	shifted_update = rewriter::replace<node>(shifted_update, memory);
	shifted_update = normalizer<node>(shifted_update);
	LOG_TRACE << "update/shifted_update: " << LOG_FM(shifted_update) << "\n";
	// std::cout << "update/shifted_update: " << LOG_FM(shifted_update) << "\n";

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
		DBG(assert(current_spec.size() == current_ubd_ctn.size()));
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
			// If no output/uninterpreted constant present, skip
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
			// std::cout << "spec part i: " << tau::get(current_spec[i].first) << "\n";
			for (size_t j = 0; j < upd_partition.size(); ++j) {
				// If no output/uninterpreted constant present, skip
				if (!upd_partition[j].second) continue;
				// std::cout << "update part j: " << tau::get(upd_partition[j].first) << "\n";
				// Check if current spec part overlaps with current update part
				if (uf.connected(current_spec[i].second->get(), upd_partition[j].second->get())) {
					// std::cout << "Merged.\n";
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
				// formulas -- see pwr_contains_bv_content
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
					unchanged = !pwr_contains_bv_content<node>(old_fm)
						&& !pwr_contains_bv_content<node>(new_fm)
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
		subtree_map<node, size_t> output_streams, input_streams;
		bool streams_ok = true;
		for (const auto& [part_alts, _] : current_spec) {
			for (const htref& alt : part_alts)
				if (!collect_output_streams(alt->get(),
					output_streams))
				{
					streams_ok = false;
					break;
				}
			if (!streams_ok) break;
		}
		if (streams_ok) for (const auto& [part_alts, _] : current_spec) {
			for (const htref& alt : part_alts)
				if (!collect_input_streams(alt->get(),
					input_streams))
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

		tref updated_spec = unsqueeze_always(
			combined_spec_fm<node>(current_spec));
		// I7: growth telemetry -- the only prior symptom of the
		// revision doubling was the interpreter getting slower.
		const std::string spec_str = TAU_TO_STR(updated_spec);
		LOG_INFO << "Updated specification (" << spec_str.size()
			<< " chars): " << spec_str << "\n\n";
		if (spec_size_warn_threshold
			&& spec_str.size() > spec_size_warn_threshold)
			LOG_WARNING << "Updated specification size "
				<< spec_str.size() << " chars exceeds the "
				"spec-size-warn threshold "
				<< spec_size_warn_threshold << "\n";

		// Set new specification for interpreter
		ubt_ctn = std::move(current_ubd_ctn);
		original_spec = std::move(current_spec);
		output_partition = std::move(uf);
		// The systems for solver need to be recomputed at beginning of next step
		final_system = false;
		compute_lookback_and_initial();
		LOG_TRACE << "interpreter::update/rebuild_outputs";
		if (!rebuild_outputs(output_streams)) return;
		LOG_TRACE << "interpreter::update/rebuild_inputs";
		if (!rebuild_inputs(input_streams)) return;
		return;
	}
	// No more clause left in update and all clauses are not realizable
	LOG_WARNING << "No update performed: updated specification is unsat\n";
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
	for (tref clause : expression_paths<node>(update)) {
		tref upd_always = tau::get(clause).find_top(
			is_child<node, tau::wff_always>);
		trefs upd_sometime = tau::get(clause).select_top(
			is_child<node, tau::wff_sometimes>);

		// Check if the update by itself is sat from current time point onwards
		// taking the memory into account
		LOG_TRACE << "pwr/clause: " << LOG_FM(clause) << "\n";
		if (!is_tau_formula_sat<node>(clause, start_time))
			continue;

		// An update already implied by the running spec is a no-op;
		// conjoining it anyway re-embeds it verbatim and feeds the
		// per-update growth (review I3).
		tref spec_fm = tau::build_wff_or(alts);
		if (!alts.empty()
			&& !pwr_contains_bv_content<node>(spec_fm)
			&& !pwr_contains_bv_content<node>(clause)
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
			return is_tau_formula_sat<node>(with, start_time)
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
				is_tau_formula_sat<node>(gate, start_time);
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
		// Drop dead alternatives and duplicates (keeping the earliest,
		// i.e. strongest, position).
		trefs result;
		for (tref d : new_alts) {
			if (tau::get(d).equals_F()) continue;
			bool dup = false;
			for (tref r : result)
				if (tau::subtree_equals(r, d)) {
					dup = true;
					break;
				}
			if (!dup) result.push_back(d);
		}
		if (result.empty()) continue;
		return to_htrefs(result);
	}
	// No update clause yields a satisfiable revision
	return {};
}

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
bool has_free_vars(tref fm, bool silent = false) {
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
			ctx.add_output_console(get_var_name<node>(var), type_id);
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
