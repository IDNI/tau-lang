// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "satisfiability.h"
#include "normalizer.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "satisfiability"

namespace idni::tau_lang {

inline static bool use_debug_output_in_sat = false;

/**
 * @internal
 * @brief Print a diagnostic message describing a fixpoint computation's
 * outcome, if diagnostic output was requested.
 * @param message Human-readable description of what was computed (e.g. how
 * many steps a fixpoint search took).
 * @param result Printed form of the resulting formula.
 * @param output When `false`, this call is a no-op.
 * @endinternal
 *
 * @par Example
 * Calling `is_tau_formula_sat<node_t>(spec, 0, true)` (note the `output =
 * true` argument) propagates `output` down into `transform_to_execution`
 * and then into `always_to_unbounded_continuation`, which calls
 * `print_fixpoint_info("Temporal normalization of always specification
 * reached fixpoint after 3 steps, yielding the result: ", TAU_TO_STR(res),
 * true)`, printing that message either via `LOG_INFO` or `std::cerr`
 * depending on the `use_debug_output_in_sat` flag. With the default
 * `output = false` (as used by every example above), the call does
 * nothing.
 */
inline void print_fixpoint_info(const std::string& message,
	const std::string& result, const bool output)
{
	if (!output) return;
	if (!use_debug_output_in_sat)
		LOG_INFO << "\n" << message << "\n" << result << "\n\n";
	else std::cerr << message << "\n" << result << "\n";
}

template <NodeType node>
bool has_stream_flag(const tref& fm) {
	using tau = tree<node>;
	return tau::get(fm).find_top(is<node, tau::wff_sometimes>)
		|| tau::get(fm).find_top(is<node, tau::constraint>);
}

/**
 * @internal
 * @brief Compute the greatest constant time point among the constant-time
 * (initial-condition) IO variables in @p io_vars.
 * @tparam node Tree node type.
 * @param io_vars IO variable nodes to inspect (may mix constant-time and
 * relative-time variables).
 * @return The largest constant time point found, or `0` if none exist.
 * @endinternal
 *
 * @par Example
 * Given `io_vars = { o1[0], o1[2], i1[t] }` (`i1[t]` is a relative-time
 * variable, not a constant-time one, so `is_io_initial` excludes it),
 * `get_lookback_after_normalization<node>(io_vars)` returns `2`, the
 * largest constant time point among the remaining `o1[0]` and `o1[2]`.
 */
template <NodeType node>
int_t get_lookback_after_normalization(const trefs& io_vars) {
	int_t max_lookback = 0;
	for (tref v : io_vars) if (is_io_initial<node>(v)) {
		int_t lookback = get_io_time_point<node>(v);
		max_lookback = std::max(max_lookback, lookback);
	}
	return max_lookback;
}

// Check if a formula has a temporary output stream
// which are used for flag handling
template <NodeType node>
bool has_temporary_io_var(tref fm) {
	using tau = tree<node>;
	auto io_vars = tau::get(fm)
		.select_top(is_child<node, tau::io_var>);
	for (tref var : io_vars) // Check if the name of var starts with "_"
		if (get_var_name<node>(var)[0] == '_') return true;
	return false;
}

/**
 * @internal
 * @brief Instantiate a single relative-time IO variable at a concrete time
 * point.
 * @tparam node Tree node type.
 * @param io_var IO variable node to instantiate (e.g. `o1[t-1]`).
 * @param time_point Time step the variable's implicit `t` refers to.
 * @return `io_var` unchanged if it already refers to a constant time
 * point; otherwise a new IO variable at `time_point` minus @p io_var's
 * relative shift.
 * @endinternal
 *
 * @par Example
 * Given the IO variable `o1[t-1]` (relative shift `1`) and `time_point =
 * 5`, `transform_io_var<node>(io_var, 5)` returns the constant-time
 * variable `o1[4]` (`5 - 1`). Given `o1[t]` (shift `0`) with the same
 * `time_point`, it returns `o1[5]`. An already-constant variable such as
 * `o1[0]` is returned unchanged regardless of `time_point`. This is the
 * per-variable building block used by `fm_at_time_point`.
 */
template <NodeType node>
tref transform_io_var(tref io_var, int_t time_point) {
	using tau = tree<node>;
	// Check if io_var has constant time point
	if (is_io_initial<node>(io_var)) return io_var;
	int_t shift = get_io_var_shift<node>(io_var);
	size_t type = tau::get(io_var).get_ba_type();
	if (tau::get(io_var).is_input_variable())
		return tau::trim(build_in_var_at_n<node>(
			get_var_name_node<node>(io_var), time_point - shift, type));
	else return tau::trim(build_out_var_at_n<node>(
			get_var_name_node<node>(io_var), time_point - shift, type));
}

template <NodeType node>
tref existentially_quantify_output_streams(tref fm, const trefs& io_vars,
                                int_t time_point, const auto& initials)
{
	using tau = tree<node>;
	// This map is needed in order to get the minimal shift for streams with same name
	std::set<int_t> quantifiable_o_vars;
	for (int_t i = 0; i < (int_t) io_vars.size(); ++i) {
		// Skip input streams
		if (tau::get(io_vars[i])[0].is_input_variable()) continue;
		// Skip initial conditions
		if (is_io_initial<node>(io_vars[i])) continue;
		quantifiable_o_vars.insert(i);
	}
	subtree_set<node> cache;
	for (int_t pos : quantifiable_o_vars) {
		// Do not quantify time steps which are predefined by initial conditions
		if (initials.contains({
			get_var_name<node>(io_vars[pos]), time_point })) continue;
		size_t type = tau::get(io_vars[pos]).get_ba_type();
		tref var = tau::trim(build_out_var_at_n<node>(
			get_var_name_node<node>(io_vars[pos]), time_point, type));
		auto res = cache.emplace(var);
		// Due to the way that phi/chi infinity are build, we do not
		// rename the bound variables
		if (res.second) fm = tau::build_wff_ex(var, fm, false);
	}
	return fm;
}

template <NodeType node>
tref universally_quantify_input_streams(tref fm, const trefs& io_vars,
					int_t time_point, const auto& initials)
{
	using tau = tree<node>;
	// This map is needed in order to get the minimal shift for streams with same name
	std::set<int_t> quantifiable_i_vars;
	for (int_t i = 0; i < (int_t)io_vars.size(); ++i) {
		// Skip output streams
		if (tau::get(io_vars[i])[0].is_output_variable()) continue;
		// Skip initial conditions
		if (is_io_initial<node>(io_vars[i])) continue;
		quantifiable_i_vars.insert(i);
	}
	subtree_set<node> cache;
	for (int_t pos : quantifiable_i_vars) {
		// Do not quantify time steps which are predefined by initial conditions
		if (initials.contains({
			get_var_name<node>(io_vars[pos]), time_point })) continue;
		size_t type = tau::get(io_vars[pos]).get_ba_type();
		tref var = tau::trim(build_in_var_at_n<node>(
			get_var_name_node<node>(io_vars[pos]), time_point, type));
		auto res = cache.emplace(var);
		// Due to the way that phi/chi infinity are build, we do not
		// rename the bound variables
		if (res.second) fm = tau::build_wff_all(var, fm, false);
	}
	return fm;
}

/**
 * @internal
 * @brief Evaluate a constant-time constraint (e.g. `t <= 3`) at a concrete
 * time point, returning the result as a Boolean-algebra constant.
 * @tparam node Tree node type.
 * @param constraint Constraint node comparing `t` against a fixed numeral
 * (`ctn_eq`, `ctn_neq`, `ctn_lt`, `ctn_lteq`, `ctn_gt`, or `ctn_gteq`).
 * @param time_point Concrete value substituted for `t`.
 * @return The Boolean-algebra constant `_1` if the constraint holds at
 * `time_point`, otherwise `_0`.
 * @endinternal
 *
 * @par Example
 * For the constraint `t <= 3` and `time_point = 2`, `calculate_ctn`
 * evaluates `2 <= 3` and returns `_1` (true); for `time_point = 5` it
 * evaluates `5 <= 3` and returns `_0` (false).
 */
template <NodeType node>
tref calculate_ctn(tref constraint, int_t time_point) {
	DBG(assert(constraint != nullptr);)
	using tau = tree<node>;
	using tt = tau::traverser;
	const auto& ctn = tau::get(constraint);
	tt t = ctn();
	int_t condition;
	bool is_left;
	auto to_ba = [](const bool c) {
		const size_t type = sbf_type_id<node>();
		return c ? tau::_1(type) : tau::_0(type);
	};

	if (ctn[0][0].is(tau::num))
		is_left = true,  condition = ctn[0][0].get_num();
	else    is_left = false, condition = ctn[0][1].get_num();

	if (t | tau::ctn_neq) return to_ba(condition != time_point);
	if (t | tau::ctn_eq)  return to_ba(condition == time_point);
	if (t | tau::ctn_gteq)
		return is_left  ? to_ba(condition >= time_point)
				: to_ba(time_point >= condition);
	if (t | tau::ctn_gt)
		return is_left  ? to_ba(condition > time_point)
				: to_ba(time_point > condition);
	if (t | tau::ctn_lteq)
		return is_left  ? to_ba(condition <= time_point)
				: to_ba(time_point <= condition);
	if (t | tau::ctn_lt)
		return is_left  ? to_ba(condition < time_point)
				: to_ba(time_point < condition);
	// The above is exhaustive for possible children of constraint
	assert(false); return nullptr;
}

/**
 * @internal
 * @brief Check whether @p time_point still lies in the "initial phase" of a
 * constant-time constraint, i.e. before the point at which its associated
 * flag stream (built by `transform_ctn_to_streams`) has stabilized and no
 * longer needs an explicit initial condition.
 * @tparam node Tree node type.
 * @param constraint Constraint node, already reduced to one of `ctn_lt`,
 * `ctn_lteq`, `ctn_gt`, or `ctn_gteq` (equality/inequality constraints must
 * already have been converted before calling this).
 * @param time_point Time step to check.
 * @return `true` if `time_point` is still within the constraint's initial
 * phase.
 * @endinternal
 *
 * @par Example
 * For the constraint `t <= 3` (`ctn_lteq`, condition `3`),
 * `is_initial_ctn_phase` returns `true` while `3 + 1 >= time_point`, i.e.
 * for `time_point` in `{0, 1, 2, 3, 4}`, and `false` for `time_point >=
 * 5`.
 */
template <NodeType node>
bool is_initial_ctn_phase(tref constraint, int_t time_point) {
	using tau = tree<node>;
	const auto& ctn = tau::get(constraint);
	const int_t condition = ctn[0][0].is(tau::num) ? ctn[0][0].get_num()
						 : ctn[0][1].get_num();

	// At this point the equality and inequality constraints should have been converted
	auto t = ctn();
	DBG(assert(!(t | tau::ctn_neq) && !(t | tau::ctn_eq));)

	if (t | tau::ctn_gteq) return condition >= time_point;
	if (t | tau::ctn_gt)   return condition + 1 >= time_point;
	if (t | tau::ctn_lteq) return condition + 1 >= time_point;
	if (t | tau::ctn_lt)   return condition >= time_point;

	// The above is exhaustive for possible children of constraints
	assert(false); return {};
}

template <NodeType node>
tref fm_at_time_point(tref original_fm, const trefs &io_vars, int_t time_point) {
	subtree_map<node, tref> changes;
	for (size_t i = 0; i < io_vars.size(); ++i)
		changes[io_vars[i]] =
				transform_io_var<node>(io_vars[i], time_point);
	return rewriter::replace<node>(original_fm, changes);
}

template <NodeType node>
std::pair<tref, tref> build_initial_step_chi(tref chi, tref st,
	const trefs& io_vars, int_t time_point, auto& pholder_to_st)
{
	using tau = tree<node>;
	subtree_map<node, tref> changes;
	for (size_t i = 0; i < io_vars.size(); ++i) {
		auto new_io_var = transform_io_var<node>(io_vars[i],time_point);
		changes[io_vars[i]] = new_io_var;
	}
	tref c_pholder = build_out_var_at_n<node>("_pholder", time_point, 0);
	c_pholder = tau::build_bf_eq_0(c_pholder);
	pholder_to_st.emplace(c_pholder, rewriter::replace<node>(st, changes));
	tref new_fm = tau::build_wff_and(rewriter::replace<node>(chi, changes),
					 c_pholder);
	return std::make_pair(new_fm, c_pholder);
}

template <NodeType node>
tref build_step(tref original_fm, tref prev_fm, const trefs &io_vars,
	const auto& initials, int_t step_num, int_t time_point, tref& cached_fm)
{
	using tau = tree<node>;
	// Use build_initial_step otherwise
	DBG(assert(step_num > 0);)
	subtree_map<node, tref> changes;
	for (size_t i = 0; i < io_vars.size(); ++i) {
		auto new_io_var = transform_io_var<node>(
					io_vars[i], time_point + step_num);
		changes[io_vars[i]] = new_io_var;
	}

	tref most_inner_step = rewriter::replace<node>(original_fm, changes);
	tref q_most_inner_step = existentially_quantify_output_streams<node>(
		most_inner_step, io_vars, time_point + step_num, initials);
	q_most_inner_step = universally_quantify_input_streams<node>(
		q_most_inner_step, io_vars, time_point + step_num, initials);
	changes = { { cached_fm,
			tau::build_wff_and(cached_fm, q_most_inner_step) }};
	cached_fm = most_inner_step;
	return rewriter::replace<node>(prev_fm, changes);
}

template <NodeType node>
tref build_step_chi(tref chi, tref st, tref prev_fm, const trefs& io_vars,
	const auto& initials, int_t step_num, int_t time_point, tref& cached_fm,
	auto& pholder_to_st)
{
	using tau = tree<node>;
	// Use build_initial_step otherwise
	DBG(assert(step_num > 0);)
	subtree_map<node, tref> changes;
	for (size_t i = 0; i < io_vars.size(); ++i) {
		auto new_io_var = transform_io_var<node>(
					io_vars[i], time_point + step_num);
		changes[io_vars[i]] = new_io_var;
	}
	// We need a placeholder symbol in order to substitute during the next step
	tref c_chi = rewriter::replace<node>(chi, changes);
	tref c_pholder = build_out_var_at_n<node>("_pholder",
							time_point + step_num, sbf_type_id<node>());
	c_pholder = tau::build_bf_eq_0(c_pholder);
	tref c_st = rewriter::replace<node>(st, changes);
	pholder_to_st.emplace(c_pholder, c_st);
	// Quantify formula which is to be added to chi
	tref q_most_inner_step = existentially_quantify_output_streams<node>(
		tau::build_wff_and(c_chi, c_pholder), io_vars,
		time_point + step_num, initials);
	q_most_inner_step = universally_quantify_input_streams<node>(
		q_most_inner_step, io_vars, time_point + step_num, initials);
	// If build_step_chi is used with empty sometimes clause
	if (tau::get(st).equals_T())
		changes = { { cached_fm,  q_most_inner_step } };
	else changes = { { cached_fm,
			tau::build_wff_or(cached_fm, q_most_inner_step) }};
	cached_fm = c_pholder;
	return rewriter::replace<node>(prev_fm, changes);
}

/**
 * @internal
 * @brief Comparator for sorting constant-time IO variables into
 * time-compatible quantification order.
 * @tparam node Tree node type.
 * @param v1 First IO variable (or its wrapping `bf`) to compare.
 * @param v2 Second IO variable (or its wrapping `bf`) to compare.
 * @return `true` if `v1` should be ordered before `v2`: ascending by time
 * point first; at equal time points, input variables before output
 * variables; ties among variables of the same kind broken by variable
 * name.
 * @endinternal
 *
 * @par Example
 * Sorting `{ o1[2], i1[2], o1[0] }` with `constant_io_comp<node>` yields
 * `{ o1[0], i1[2], o1[2] }`: `o1[0]` comes first (smallest time point);
 * `i1[2]` and `o1[2]` tie on time point `2`, but the input variable `i1[2]`
 * is ordered before the output variable `o1[2]`.
 */
// comparator for sorting constant io variables
template <NodeType node>
inline auto constant_io_comp = [](tref v1, tref v2) {
	using tau = tree<node>;
	// trim the bf of v1 and v2 if present
	v1 = tau::get(v1).is(tau::bf) ? tau::trim(v1) : v1;
	v2 = tau::get(v2).is(tau::bf) ? tau::trim(v2) : v2;
	if (get_io_time_point<node>(v1) < get_io_time_point<node>(v2))
		return true;
	if (get_io_time_point<node>(v1) == get_io_time_point<node>(v2)) {
		bool v1_in = tau::get(v1)[0].is_input_variable();
		bool v2_in = tau::get(v2)[0].is_input_variable();
		if (!v1_in &&  v2_in) return false;
		if ( v1_in && !v2_in) return true;
		if ( v1_in &&  v2_in)
			return get_var_name<node>(v1) < get_var_name<node>(v2);
		if (!v1_in && !v2_in)
			return get_var_name<node>(v1) < get_var_name<node>(v2);
			// This covers all cases already
		else return false;
	} else return false;
};

/**
 * @internal
 * @brief Check whether a fully time-instantiated "run" (a formula built
 * from IO variables at constant time points only, as produced by repeated
 * `fm_at_time_point` calls) is satisfiable under time-compatible
 * quantification, i.e. all input streams universally and all output
 * streams existentially quantified in increasing time order.
 *
 * This method is designed to be called on the output of
 * `find_fixpoint_phi`/`find_fixpoint_chi` when the run was started at the
 * earliest well-defined time point.
 * @tparam node Tree node type.
 * @param fm Formula consisting solely of IO variables at constant time
 * points.
 * @return `true` if the time-compatibly quantified formula is satisfiable
 * (short-circuiting to `true`/`false` immediately if `fm` is already `T`
 * or `F`).
 * @endinternal
 *
 * @par Example
 * `always_to_unbounded_continuation` builds up a `run` formula by
 * conjoining successive `fm_at_time_point<node>(ubd_ctn, io_vars, t)`
 * instantiations and calls `is_run_satisfiable<node>(run)` after each step
 * to detect a contradiction as early as possible. For the always-part of
 * "smaller_lookback_one_st" from
 * tests/integration/test_integration-satisfiability1.cpp:17
 * (`o1[t] = o1[t-1] && o1[t-1] = 1`), the run instantiated at times 0 and
 * 1 (`o1[0] = 1 && (o1[1] = o1[0] && o1[0] = 1)`) is satisfiable, since an
 * output stream constantly equal to `1` exists; for the always-part of
 * "equal_lookback_one_st" (line 13, `o1[t-1] = 0`) combined with a run step
 * that additionally forces `o1[t] = 1`, no such assignment exists and
 * `is_run_satisfiable` returns `false`.
 */
template <NodeType node>
bool is_run_satisfiable(tref fm) {
	using tau = tree<node>;

	DBG(LOG_TRACE
		<< "is_run_satisfiable begin\n"
		<< "is_run_satisfiable[fm]: " << LOG_FM(fm);)

	const auto& t = tau::get(fm);
	if (t.equals_F()) return false;
	if (t.equals_T()) return true;

	const trefs& free_io_vars = t.get_free_vars();
	// TODO: filter free_io_vars instead of searching whole formula again
	trefs io_vars = t.select_top(is_child<node, tau::io_var>);
	std::sort(io_vars.begin(), io_vars.end(), constant_io_comp<node>);

	// All io_vars in fm have to refer to constant time positions
	DBG(assert(std::all_of(io_vars.begin(), io_vars.end(),
		[](tref el) { return is_io_initial<node>(el); }));)

	auto sat_fm = fm;
	while (!io_vars.empty()) {
		if (!tau::contains_subtree(free_io_vars, io_vars.back())) {
			io_vars.pop_back();
			continue;
		}
		if (tau::get(io_vars.back())[0].is_input_variable())
		sat_fm = tau::build_wff_all(io_vars.back(), sat_fm, false);
		else    sat_fm = tau::build_wff_ex(io_vars.back(), sat_fm, false);
		io_vars.pop_back();
	}

	DBG(LOG_TRACE << "is_run_satisfiable[sat_fm]: " << LOG_FM(sat_fm));

	auto result = is_non_temp_nso_satisfiable<node>(sat_fm);

	DBG(LOG_TRACE
		<< "is_run_satisfiable[result]: " << result << "\n"
		<< "is_run_satisfiable end\n");

	return result;
}

// Assumption is that the provided fm is an unbound continuation
template <NodeType node>
tref get_uninterpreted_constants_constraints(tref fm, trefs& io_vars, const int_t start_time) {
	using tau = tree<node>;
	// Substitute lookback as current time point
	int_t look_back = get_max_shift<node>(io_vars);
	tref uconst_ctns = fm_at_time_point<node>(fm, io_vars, look_back + start_time);
	io_vars = tau::get(uconst_ctns).select_top(is_child<node, tau::io_var>);

	// All io_vars in fm have to refer to constant time positions
	DBG(assert(std::all_of(io_vars.begin(), io_vars.end(),
		[](tref el){ return is_io_initial<node>(el); }));)
	std::sort(io_vars.begin(), io_vars.end(), constant_io_comp<node>);

	trefs free_io_vars(get_free_vars<node>(uconst_ctns));
	while (io_vars.size()) {
		if (!tau::contains_subtree(free_io_vars, io_vars.back()))
		{
			io_vars.pop_back();
			continue;
		}
		auto& v = io_vars.back();

		for (auto it = free_io_vars.begin();
			it != free_io_vars.end(); ++it)
		{
			if (tau::subtree_equals(*it, v)) {
				free_io_vars.erase(it);
				break;
			}
		}
		uconst_ctns = tau::get(v).is_input_variable()
			? tau::build_wff_all(v, uconst_ctns, false)
			: tau::build_wff_ex( v, uconst_ctns, false);
		io_vars.pop_back();
	}
	// Existentially quantify remaining variables
	trefs uconsts;
	for (tref v : free_io_vars) {
		if (!tau::get(v).child_is(tau::uconst_name))
			uconst_ctns = tau::build_wff_ex(v, uconst_ctns, false);
		else uconsts.push_back(v);
	}
	// Eliminate all variables
	uconst_ctns = normalize_non_temp<node>(uconst_ctns);
	// Now add all uninterpreted constants which disappeared during elimination of variables
	// and set them to 0
	trefs left_uconsts = tau::get(uconst_ctns).select_top(
		is_child<node, tau::uconst_name>);
	for (tref uc : uconsts) {
		if (std::ranges::find_if(left_uconsts, [&uc](const auto& n) {
			return tau::get(n) == tau::get(uc);
		}) == left_uconsts.end())
			uconst_ctns = tau::build_wff_and(uconst_ctns,
				tau::build_bf_eq_0(tau::get(tau::bf, uc)));
	}

	LOG_DEBUG<<"Formula describing constraints on uninterpreted constants: "
		<< LOG_FM(uconst_ctns);

	return uconst_ctns;
}

/**
 * @internal
 * @brief Compute the unbounded (infinite-horizon) continuation of an
 * `always`-part `base_fm` by repeatedly unrolling one more time step and
 * checking for a fixpoint (`is_nso_impl(phi_prev, phi)`), i.e. the point at
 * which adding another step no longer strengthens the accumulated
 * formula.
 * @tparam node Tree node type.
 * @param base_fm Always-part local specification to unroll.
 * @param ctn_initials Initial-condition formula for any constant-time
 * constraint flags occurring in `base_fm`.
 * @param io_vars IO variable nodes appearing in `base_fm`.
 * @param initials Set of `(variable name, time point)` pairs marking
 * positions predefined by explicit initial conditions (skipped when
 * existentially/universally quantifying).
 * @param time_point Time step at which unrolling starts.
 * @return A pair `(phi, steps)`: `phi` is the formula at the fixpoint (or
 * at the point where the step cap `max_fixpoint_steps` was hit), and
 * `steps` is the number of steps taken to reach it.
 * @endinternal
 *
 * @par Example
 * This is a deeper fixpoint-search helper operating on partially unrolled
 * AST state, so a literal spec-string round trip does not apply; the
 * following is illustrative rather than a runnable snippet. For the
 * always-part of "smaller_lookback_one_st" in
 * tests/integration/test_integration-satisfiability1.cpp:17
 * (`o1[t] = o1[t-1] && o1[t-1] = 1`), each unrolling step directly
 * determines `o1` at the new time point from the previous one with no
 * additional free choices, so the accumulated formula stabilizes (up to
 * logical implication) after very few steps — `find_fixpoint_phi` returns
 * that stabilized formula together with the step count it took.
 */
template <NodeType node>
std::pair<tref, int_t> find_fixpoint_phi(tref base_fm, tref ctn_initials,
	const trefs& io_vars, const auto& initials, int_t time_point)
{
	using tau = tree<node>;
	tref phi_prev = fm_at_time_point<node>(base_fm, io_vars, time_point);
	phi_prev = tau::build_wff_and(ctn_initials, phi_prev);
	int_t step_num = 1;
	tref cache = phi_prev;
	tref phi = build_step<node>(base_fm, phi_prev, io_vars, initials, step_num,
							time_point, cache);

	LOG_DEBUG << "Continuation at step " << step_num << ": " << LOG_FM(phi);

	int_t lookback = get_max_shift<node>(io_vars);
	// Find fix point once all initial conditions have been passed and
	// the time_point is greater equal the step_num
	// SO-1: this search has no convergence guarantee; cap the step count so
	// a non-converging formula fails loudly instead of hanging forever.
	// This is a safety net, not full error propagation: callers still
	// receive a (non-fixpoint) result rather than a failure signal.
	constexpr int_t max_fixpoint_steps = 1'000'000;
	while (step_num < lookback || !is_nso_impl<node>(phi_prev, phi)){
		if (step_num >= max_fixpoint_steps) {
			LOG_ERROR << "find_fixpoint_phi: exceeded " << max_fixpoint_steps
				<< " steps without reaching a fixpoint, giving up";
			break;
		}
		phi_prev = phi;
		++step_num;

		phi = build_step<node>(base_fm, phi_prev, io_vars, initials, step_num,
							time_point, cache);

		LOG_DEBUG << "Continuation at step " << step_num << ": "
								<< LOG_FM(phi);
	}
	LOG_DEBUG << "Unbounded continuation of Tau formula reached fixpoint"
		<< " after " << step_num-1 << " steps: " << LOG_FM(phi_prev);
	return std::make_pair(phi_prev, step_num - 1);
}

/**
 * @internal
 * @brief Compute the unbounded continuation of the combined always/flag
 * ("chi") state used to decide whether a `sometimes` clause's guard flag
 * can ever be raised, by repeatedly unrolling one more time step
 * (`build_step_chi`) and checking for a fixpoint against the previous
 * step's (placeholder-substituted) formula.
 * @tparam node Tree node type.
 * @param chi_base Always-part local specification driving the recurrence.
 * @param st Eventual-variable flag formula (from
 * `transform_to_eventual_variables`) whose satisfiability is being
 * tracked.
 * @param io_vars IO variable nodes appearing in `chi_base`.
 * @param initials Set of `(variable name, time point)` pairs marking
 * positions predefined by explicit initial conditions.
 * @param time_point Time step at which unrolling starts.
 * @return A pair `(chi, steps)`: `chi` is the (placeholder-substituted)
 * formula at the fixpoint (or at the point where the step cap
 * `max_fixpoint_steps` was hit), and `steps` is the number of steps taken.
 * @endinternal
 *
 * @par Example
 * Like `find_fixpoint_phi`, this operates on partially unrolled,
 * placeholder-substituted AST state rather than a parseable spec string,
 * so the following is illustrative only. `to_unbounded_continuation` calls
 * `find_fixpoint_chi` only after the initial segment up to
 * `flag_boundary` failed to raise the flag directly; it then unrolls the
 * always-part together with the flag recurrence until two consecutive
 * steps agree (up to implication), yielding a formula describing every
 * time point from which the flag could still be raised, or `F` once
 * normalized if it never can be (see the "flag_boundary" tests in
 * tests/integration/test_integration-solver.cpp:841-878 for concrete
 * sat/unsat outcomes of this overall code path).
 */
template <NodeType node>
std::pair<tref, int_t> find_fixpoint_chi(tref chi_base, tref st,
	const trefs& io_vars, const auto& initials, int_t time_point)
{
	subtree_map<node, tref> pholder_to_st;
	auto [chi_prev, cache] = build_initial_step_chi<node>(
		chi_base, st, io_vars, time_point, pholder_to_st);

	int_t lookback = get_max_shift<node>(io_vars);
	int_t step_num = 1;

	tref chi = build_step_chi<node>(chi_base, st, chi_prev, io_vars,
		 initials, step_num, time_point, cache, pholder_to_st);

	tref chi_replc = rewriter::replace<node>(chi, pholder_to_st);
	tref chi_prev_replc = rewriter::replace<node>(chi_prev, pholder_to_st);

	LOG_DEBUG << "Continuation at step " << step_num << ": "
			<< LOG_FM(rewriter::replace<node>(chi, pholder_to_st));

	// Find fix point once the lookback is greater the step_num
	// SO-1: same unbounded-search concern as find_fixpoint_phi above.
	constexpr int_t max_fixpoint_steps = 1'000'000;
	while (step_num < lookback || !is_nso_impl<node>(chi_prev_replc, chi_replc))
	{
		if (step_num >= max_fixpoint_steps) {
			LOG_ERROR << "find_fixpoint_chi: exceeded " << max_fixpoint_steps
				<< " steps without reaching a fixpoint, giving up";
			break;
		}
		chi_prev = chi, chi_prev_replc = chi_replc, ++step_num;
		chi = build_step_chi<node>(chi_base, st, chi_prev, io_vars,
			initials, step_num, time_point, cache, pholder_to_st);
		chi_replc = rewriter::replace<node>(chi, pholder_to_st);

		LOG_DEBUG << "Continuation at step " << step_num << ": "
			<< LOG_FM(chi_replc);
	}
	LOG_DEBUG << "Unbounded continuation of Tau formula "
		<< "reached fixpoint after " << step_num - 1 << " steps: "
		<< LOG_FM(normalize_non_temp<node>(chi_prev_replc));
	return { chi_prev_replc, step_num - 1 };
}

template <NodeType node>
tref transform_back_non_initials(tref fm, const int_t highest_init_cond) {
	using tau = tree<node>;
	// First, make quantifiers canonical again
	fm = canonize_quantifier_ids<node>(fm);
	// Find lookback
	auto current_io_vars = tau::get(fm).select_top(
			is_child<node, tau::io_var>);
	int_t lookback = get_lookback_after_normalization<node>(current_io_vars);

	subtree_map<node, tref> changes;
	// Get time positions which are higher than highest_init_cond and transform back to
	// time variable depending on t
	for (tref io_var : current_io_vars) {
		int_t time_point = get_io_time_point<node>(io_var);
		if (time_point <= highest_init_cond) continue;
		tref transformed_var;
		size_t type = tau::get(io_var).get_ba_type();
		if (time_point - lookback != 0)
			transformed_var = tau::get(io_var).is_input_variable()
				? tau::trim(build_in_var_at_t_minus<node>(
					get_var_name_node<node>(io_var),
					abs(time_point - lookback), type,"t"))
				: tau::trim(build_out_var_at_t_minus<node>(
					get_var_name_node<node>(io_var),
					abs(time_point - lookback), type,"t"));
		else
			transformed_var = tau::get(io_var).is_input_variable()
				? tau::trim(build_in_var_at_t<node>(
					get_var_name_node<node>(io_var), type, "t"))
				: tau::trim(build_out_var_at_t<node>(
					get_var_name_node<node>(io_var), type, "t"));
		changes.emplace(io_var, transformed_var);
	}
	return rewriter::replace<node>(fm, changes);
}

template <NodeType node>
tref build_flag_on_lookback(tref var_name_node, const std::string& var,
							const int_t lookback)
{
	size_t flag_type = get_ba_type_id<node>(sbf_type<node>());
	if (lookback >= 2) return build_out_var_at_t_minus<node>(
		var_name_node, lookback - 1, flag_type, var);
	else return build_out_var_at_t<node>(var_name_node, flag_type, var);
}

template <NodeType node>
tref build_prev_flag_on_lookback(tref io_var_node,
				const std::string& var, const int_t lookback)
{
	size_t flag_type = get_ba_type_id<node>(sbf_type<node>());
	if (lookback >= 2)
		return build_out_var_at_t_minus<node>(io_var_node, lookback, flag_type, var);
	else return build_out_var_at_t_minus<node>(io_var_node, 1, flag_type, var);
}

/**
 * @internal
 * @brief Replace every constant-time constraint (e.g. `t <= 3`) in @p fm
 * with a fresh Boolean flag output stream, plus the recurrence rules and
 * initial conditions needed for that flag to track the constraint over
 * time.
 * @tparam node Tree node type.
 * @param fm Formula possibly containing constant-time constraint nodes.
 * @param flag_initials [out] Conjunction of initial conditions fixing each
 * new flag stream's value at the time points within the constraint's
 * initial phase (see `is_initial_ctn_phase`).
 * @param flag_rules [out] Conjunction of recurrence rules relating each
 * flag's value at one time point to the previous one, taking @p lookback
 * into account.
 * @param lookback Lookback of the surrounding formula, used to pick which
 * previous time point the flag rule refers to.
 * @param start_time Time step at which the run begins (used to seed
 * initial conditions).
 * @param reset_ctn_id When `true`, resets the (function-local static)
 * flag-numbering counter back to `0` before processing @p fm.
 * @return `fm` with each constraint replaced by `_fK[t] != 0` for a fresh
 * flag stream `_fK`, or `fm` unchanged if it contains no constraints.
 * @endinternal
 *
 * @par Example
 * This transforms an internal constraint node into fresh, generated flag
 * streams (`_f0`, `_f1`, ...), so there is no direct spec-string
 * round-trip; the following is conceptual. For a constraint `t <= 3`
 * occurring in a formula, `transform_ctn_to_streams` introduces a flag
 * stream `_f0[t]` such that `_f0[t] != 0` replaces the constraint,
 * together with `flag_rules` encoding "once `_f0` drops to `0` it stays
 * `0`" (or the dual, for `>`/`>=` constraints) and `flag_initials` fixing
 * `_f0[0..3] = 1` to match `t <= 3` holding at those initial time points.
 * This underlies the constant-time initial-condition handling exercised
 * by the "flag_boundary" tests in
 * tests/integration/test_integration-solver.cpp:841-878 (e.g. `o1[8] =
 * 1`).
 */
template <NodeType node>
tref transform_ctn_to_streams(tref fm, tref& flag_initials,
	tref& flag_rules, const int_t lookback, const int_t start_time,
	bool reset_ctn_id)
{
	using tau = tree<node>;
	auto to_eq_1 = [](tref n) {
			return tau::build_bf_eq_0(tau::build_bf_neg(n)); };
	auto create_initial =
		[&flag_initials](tref ctn, tref flag_iovar, const int_t t)
	{
		tref flag_init_cond = transform_io_var<node>(flag_iovar, t);
		flag_init_cond = tau::get(tau::bf, flag_init_cond);
		flag_initials = tau::build_wff_and(tau::build_bf_eq_0(
				tau::build_bf_xor(flag_init_cond,
						calculate_ctn<node>(ctn, t))),
			flag_initials);
	};
	flag_initials = tau::_T();
	subtree_map<node, tref> changes;
	// transform constraints to their respective output streams and add required conditions
	// We make the variable static so that we can transform different parts of the formula independently
	static size_t ctn_id = 0;
	if (reset_ctn_id) ctn_id = 0, reset_ctn_id = false;
	for (tref ctn : tau::get(fm).select_top(is<node, tau::constraint>)) {
		const auto& ct = tau::get(ctn);
		std::string ctnvar = tau::get(
			ct.find_top(is<node, tau::ctnvar>)).get_string();
		std::stringstream ss; ss << "_f" << ctn_id++;
		tref var = tau::build_var_name(ss.str());
		size_t flag_type = get_ba_type_id<node>(sbf_type<node>());
		tref flag_iovar = tau::trim(
			build_out_var_at_t<node>(var, flag_type, ctnvar));

		// Take lookback of formula into account for constructing rule
		tref flag_rule1 = build_prev_flag_on_lookback<node>(
						var, ctnvar, lookback);
		tref flag_rule2 = build_flag_on_lookback<node>(
						var, ctnvar, lookback);
		changes[ctn] = tau::trim(to_eq_1(tau::get(tau::bf, flag_iovar)));
		if (ct() | tau::ctn_gt || ct() | tau::ctn_gteq) {
			// Add flag rule _fk[lookback] != 0 -> _fk[lookback-1] = 1
			auto flag_rule = tau::build_wff_or(
				tau::build_bf_eq_0(flag_rule1),
				to_eq_1(flag_rule2));
			// Conjunct flag rule with formula
			flag_rules = tau::build_wff_and(flag_rules, flag_rule);
		} else {
			// Flag is of type less or less_equal
			// Add flag rule _fk[lookback] != 1 -> _fk[lookback-1] = 0
			auto flag_rule = tau::build_wff_or(to_eq_1(flag_rule1),
						tau::build_bf_eq_0(flag_rule2));
			// Conjunct flag rule with formula
			flag_rules = tau::build_wff_and(flag_rules, flag_rule);
		}

		// Add initial conditions for flag
		int_t t = start_time;
		// Check if start_time is higher then initial ctn phase
		if (is_initial_ctn_phase<node>(ctn, t))
			while (is_initial_ctn_phase<node>(ctn, t)) {
				create_initial(ctn, flag_iovar, t);
				++t;
			}
		else // The flag needs to be initialized
			create_initial(ctn, flag_iovar, t);
	}
	if (!changes.empty()) return rewriter::replace<node>(fm, changes);
	return fm;
}

/**
 * @internal
 * @brief Compute the unbounded (infinite-horizon) continuation of a single
 * `always` statement: the formula obtained by unrolling the recurrence
 * until it reaches a fixpoint, then checking that formula stays
 * satisfiable across every initial time step.
 *
 * Assumes the formula has run through the normalizer before and is a
 * single `always` statement (with the `always` wrapper optionally already
 * stripped).
 * @tparam node Tree node type.
 * @param fm The `always`-part local specification (or the whole `wff_always`
 * node) to compute the continuation of.
 * @param start_time Time step at which execution begins.
 * @param output When `true`, print diagnostic fixpoint information via
 * `print_fixpoint_info`.
 * @return `F` if the always-part is unsatisfiable at some initial time
 * step; otherwise the unbounded continuation formula (still wrapped in
 * `always` if the recurrence remains open-ended).
 * @endinternal
 *
 * @par Example
 * `transform_to_execution` calls this on the always-part of a spec before
 * combining it with any `sometimes` clauses. For the always-part of
 * "smaller_lookback_one_st" in
 * tests/integration/test_integration-satisfiability1.cpp:17
 * (`o1[t] = o1[t-1] && o1[t-1] = 1`), `always_to_unbounded_continuation`
 * conceptually determines that `o1` is forced to be constantly `1` from
 * time step 1 onward and returns a formula equivalent to that recurrence
 * (satisfiable). For the always-part of "equal_lookback_one_st" (line 13,
 * `o1[t-1] = 0`), it likewise returns a satisfiable continuation (`o1`
 * constantly `0`); it is the combination with the conflicting `sometimes`
 * clause elsewhere in `transform_to_execution` that later makes the whole
 * spec unsatisfiable. This is illustrative rather than a literal
 * `create_spec`/`CHECK` snippet, since `always_to_unbounded_continuation`
 * is called on an already-extracted always-part fragment, not a fresh
 * spec string.
 */
template <NodeType node>
tref always_to_unbounded_continuation(tref fm, const int_t start_time,
	const bool output)
{
	using tau = tree<node>;

	DBG(LOG_TRACE
		<< "always_to_unbounded_continuation begin\n"
		<< "always_to_unbounded_continuation[fm]: " << LOG_FM(fm) << "\n";)

	assert(has_no_boolean_combs_of_models<node>(fm));

	if (tau::get(fm).child_is(tau::wff_always)) fm = tau::trim2(fm);

	// Preparation to transform flags to output streams
	trefs io_vars = tau::get(fm).select_top(is_child<node, tau::io_var>);
	int_t lookback = get_max_shift<node>(io_vars);
	tref flag_initials = tau::_T(), flag_rules = tau::_T();
	tref transformed_fm = transform_ctn_to_streams<node>(
		fm, flag_initials, flag_rules, lookback, start_time, true);
	if (lookback == 0 && fm != transformed_fm) {
		io_vars = tau::get(transformed_fm)
				.select_top(is_child<node, tau::io_var>);
		fm = shift_io_vars_in_fm<node>(transformed_fm, io_vars, 1);
	} else fm = transformed_fm;
	fm = tau::build_wff_and(fm, flag_rules);

	DBG(LOG_TRACE
		<< "always_to_unbounded_continuation(removed flags): "
		<< LOG_FM(tau::build_wff_and(fm, flag_initials)) << "\n";)

	// TODO (LOW) Maybe is better to compute io_vars separately and merge them
	io_vars = tau::get(tau::build_wff_and(fm, flag_initials))
			.select_top(is_child<node, tau::io_var>);

	// Save positions of io_variables which are initial conditions
	std::set<std::pair<std::string, int_t>> initials;
	for (int_t i = 0; i < (int_t) io_vars.size(); ++i)
            if (is_io_initial<node>(io_vars[i]))
                initials.emplace(get_var_name<node>(io_vars[i]),
                    get_io_time_point<node>(io_vars[i]));

	// Calculate unbound continuation of fm
	lookback = get_max_shift<node>(io_vars);
	int_t point_after_inits = get_max_initial<node>(io_vars) + 1;
	auto [ubd_ctn, steps] = find_fixpoint_phi<node>(fm, flag_initials, io_vars,
					initials, lookback + point_after_inits);

	ubd_ctn = normalize_non_temp<node>(ubd_ctn);
	ubd_ctn = transform_back_non_initials<node>(ubd_ctn, point_after_inits - 1);

	// Run phi_inf until all initial conditions are taken into account
	io_vars = tau::get(ubd_ctn).select_top(is_child<node, tau::io_var>);
	tref run = tau::_T();
	const int_t s = start_time + lookback;
	// // In case no initial condition is being checked
	// // we still need to check a run once
	bool conjunct_with_run = true;
	if (point_after_inits + lookback <= s) {
		point_after_inits = s + 1 - lookback; // By def of s, this is positive
		conjunct_with_run = false;
	}
	// variable furthest back needs to pass all initial conditions
	for (int_t t = s; t < point_after_inits + lookback; ++t) {
		auto current_step = fm_at_time_point<node>(ubd_ctn, io_vars, t);
		run = tau::build_wff_and(run, current_step);

		DBG(LOG_TRACE << "always_to_unbounded_continuation[run]: " << LOG_FM(run) << "\n";)

		// Check if run is still sat
		run = normalize_non_temp<node>(run);
		if (!is_run_satisfiable<node>(run)) {
			print_fixpoint_info(
				"Temporal normalization of always specification reached fixpoint after "
				+ std::to_string(steps) +
				" steps, yielding the result: ",
				TAU_TO_STR(tau::_F()), output);
			return tau::_F();
		}
	}
	auto result = normalize_non_temp<node>(
		conjunct_with_run ? tau::build_wff_and(ubd_ctn, run) : ubd_ctn);
	// The following is std::cout because it should always be printed
	print_fixpoint_info(
		"Temporal normalization of always specification reached fixpoint after "
		+ std::to_string(steps) + " steps, yielding the result: ",
		TAU_TO_STR(tau::get(result).child_is(tau::wff_always)
			? tau::trim2(result) : result), output);

	DBG(LOG_TRACE
		<< "always_to_unbounded_continuation[result]: " << LOG_FM(result) << "\n"
		<< "always_to_unbounded_continuation end\n";)
	return result;
}

// Creates a guard using the names of the input streams in uninterpreted constants
template <NodeType node>
tref create_guard(const trefs& io_vars, const int_t number) {
	using tau = tree<node>;
	tref guard = tau::_T();
	for (tref io_var : io_vars) {
		// Check if input stream variable
		if (tau::get(io_var).is_input_variable()) {
			// Give name of io_var and make it non-user definable with "_"
			size_t type = tau::get(io_var).get_ba_type();
			tref uc = tau::build_bf_uconst("_" + TAU_TO_STR(io_var),
							std::to_string(number), type);
			tref cdn = tau::build_bf_eq(tau::get(tau::bf, io_var), uc);
			guard = tau::build_wff_and(guard, cdn);
		}
	}
	return guard;
}

/**
 * @internal
 * @brief Replace every `sometimes` sub-formula in a single normalized Tau
 * DNF clause with an "eventual variable" flag stream and a corresponding
 * `always`-part assumption, so that satisfiability of the `sometimes`
 * clauses can be decided as part of the always-part's fixpoint search.
 *
 * Assumes @p fm is a single normalized Tau DNF clause.
 * @tparam node Tree node type.
 * @param fm Normalized Tau DNF clause, possibly containing `always` and/or
 * `sometimes` sub-formulas.
 * @param reset_ctn_stream Forwarded to `transform_ctn_to_streams` for each
 * `sometimes` clause, to reset the flag-numbering counter.
 * @param start_time Time step at which execution begins.
 * @return A pair `(res, max_st_lookback)`. If @p fm has no `sometimes`
 * sub-formula, `res` is `fm` unchanged and `max_st_lookback` is `0`.
 * Otherwise `res` is `fm` with each `sometimes` clause replaced by a flag
 * assumption folded into the `always`-part, plus a new `sometimes`
 * clause tracking when all flags have latched to zero, and
 * `max_st_lookback` is the greatest lookback among the original
 * `sometimes` clauses.
 * @endinternal
 *
 * @par Example
 * This is a deeper AST-to-AST transformation introducing generated flag
 * streams (`_e0`, `_e1`, ...), so the following is conceptual rather than
 * a literal `create_spec`/`CHECK` snippet. For "smaller_lookback_one_st"
 * in tests/integration/test_integration-satisfiability1.cpp:17
 * (`(always o1[t] = o1[t-1] && o1[t-1] = 1) && (sometimes o2[t] = 0)`),
 * `transform_to_eventual_variables` introduces a flag stream `_e0[t]` and
 * folds the assumption "once `_e0` transitions from nonzero to zero,
 * `o2[t] = 0` must hold at that time" into the always-part, replacing the
 * original `sometimes o2[t] = 0` with `sometimes _e0[t] = 0` (tracking
 * when the flag has latched). Since `o2` is otherwise unconstrained, this
 * flag can always be made to latch immediately, reflecting the spec's
 * satisfiability.
 */
// Assumes single normalized Tau DNF clause
template <NodeType node>
std::pair<tref, int_t> transform_to_eventual_variables(tref fm,
	bool reset_ctn_stream, const int_t start_time)
{
	using tau = tree<node>;
	const auto& t = tau::get(fm);
	trefs smt_fms = t.select_top(is_child<node, tau::wff_sometimes>);
	if (smt_fms.empty()) return { fm, 0 };
	tref aw_fm = t.find_top(is_child<node, tau::wff_always>);

	int_t max_st_lookback = get_max_shift<node>(
		t.select_top_until(is_child<node, tau::io_var>,
					is_child<node, tau::wff_always>));

	int_t aw_lookback = 0;
	trefs aw_io_vars;
	if (aw_fm != nullptr) {
		aw_io_vars = tau::get(aw_fm)
				.select_top(is_child<node, tau::io_var>);
		aw_lookback = get_max_shift<node>(aw_io_vars);
	}

	LOG_TRACE << "transforming eventual variables: " << LOG_FM(fm);
	size_t flag_type = sbf_type_id<node>();
	tref ev_assm = tau::_T();
	tref ev_collection = tau::_0(flag_type);
	for (size_t n = 0; n < smt_fms.size(); ++n) {
		trefs st_io_vars = tau::get(smt_fms[n])
				.select_top(is_child<node, tau::io_var>);
		int_t st_lookback = get_max_shift<node>(st_io_vars);

		// Transform constant time constraints to io var in sometimes statement
		tref ctn_initials = tau::_T(), ctn_assm = tau::_T();
		smt_fms[n] = transform_ctn_to_streams<node>(
			smt_fms[n], ctn_initials, ctn_assm, st_lookback,
			start_time, reset_ctn_stream);
		st_io_vars = tau::get(smt_fms[n])
				.select_top(is_child<node, tau::io_var>);

		std::stringstream ss; ss << "_e" << n;
		tref out = tau::build_var_name(ss.str());
		// Build the eventual var flags based on the maximal lookback
		tref eNt_without_lookback = build_out_var_at_t<node>(
			out, flag_type, "t");
		tref eNt = build_flag_on_lookback<node>(out, "t",
						max_st_lookback + aw_lookback);
		tref eNt_prev = build_prev_flag_on_lookback<node>(out, "t",
						max_st_lookback + aw_lookback);

		tref eN0_is_not_zero = tau::build_bf_neq_0(
			build_out_var_at_n<node>(out, start_time, flag_type));
		tref eNt_is_zero          = tau::build_bf_eq_0(eNt);
		// tref eNt_is_not_zero      = tau::build_bf_neq_0(eNt);
		tref eNt_prev_is_zero     = tau::build_bf_eq_0(eNt_prev);
		tref eNt_prev_is_not_zero = tau::build_bf_neq_0(eNt_prev);
		// transform `sometimes psi` to:
		// (_eN[t-1] != 0 && _eN[t] == 0) -> psi (N is nth `sometimes`)
		tref shifted_sometimes = (max_st_lookback==0 && aw_lookback==0)
			? shift_io_vars_in_fm<node>(tau::trim2(smt_fms[n]),
				st_io_vars, 1)
			: shift_io_vars_in_fm<node>(tau::trim2(smt_fms[n]),
				st_io_vars, (max_st_lookback - st_lookback)
								+ aw_lookback);

		// Guard statement using uninterpreted constants to express that
		// "if the inputs equal the uninterpreted constants, the Tau formula
		// under sometimes is implied"
		// This mimics an existential quantifier capturing the inputs but at the same
		// time the inputs are not quantified
		st_io_vars = tau::get(shifted_sometimes)
				.select_top(is_child<node, tau::io_var>);
		tref guard = create_guard<node>(st_io_vars, n);
		shifted_sometimes = tau::build_wff_imply(
						guard, shifted_sometimes);

		ev_assm = tau::build_wff_and(ev_assm, tau::build_wff_imply(
			tau::build_wff_and(eNt_prev_is_not_zero, eNt_is_zero),
							shifted_sometimes));

		// for each _eN add conjunction
		// 	(_eN[0] != 0 && (_eN[t-1] = 0 -> _eN[t] = 0))
		ev_assm = tau::build_wff_and(ev_assm,
			tau::build_wff_and(eN0_is_not_zero,
				tau::build_wff_imply(
					eNt_prev_is_zero, eNt_is_zero)));

		// Add flag assumptions from constant time constraints
		ev_assm = tau::build_wff_and(ev_assm,
				tau::build_wff_and(ctn_initials, ctn_assm));

		LOG_TRACE << "trans_ev/ev_assm: " << LOG_FM(ev_assm) << "\n";

		ev_collection = tau::build_bf_or(
			ev_collection, eNt_without_lookback);
	}
	tref res = tau::_T();
	// Check if always part is present
	if (aw_fm != nullptr) {
		tref aw = tau::trim2(aw_fm);
		// Conjunct former always part and eventual variable assumptions
		res = tau::build_wff_always(
			tau::build_wff_and(aw, ev_assm));
		// Conjunct new sometimes part if present
		if (!tau::get(ev_assm).equals_T()) {
			// if the lookback of all parts is 0, we need to shift the always part
			// to account for the modified lookback due to the eventual variables
			if (max_st_lookback == 0 && aw_lookback == 0)
				res = shift_io_vars_in_fm<node>(
							res, aw_io_vars, 1);
			res = tau::build_wff_and(res,
				tau::build_wff_sometimes(
					tau::build_bf_eq_0(ev_collection)));
		}
	} else {
		// Conjunct new sometimes part if present
		if (!tau::get(ev_assm).equals_T())
			res = tau::build_wff_and(
				tau::build_wff_always(ev_assm),
				tau::build_wff_sometimes(
					tau::build_bf_eq_0(ev_collection)));
		else return  { fm, max_st_lookback };
	}

	LOG_TRACE << "transformed eventual variables: " << LOG_FM(res);
	return { res, max_st_lookback };
}

template <NodeType node>
tref add_st_ctn(tref st, const int_t timepoint, const int_t steps) {
	using tau = tree<node>;
	tref st_ctn = tau::_T();
	trefs io_vars = tau::get(st).select_top(is_child<node, tau::io_var>);
	for (int_t s = 0; s <= steps; ++s) {
		subtree_map<node, tref> changes;
		for (size_t i = 0; i < io_vars.size(); ++i) {
			tref new_io_var =
				transform_io_var<node>(io_vars[i], timepoint + s);
			changes[io_vars[i]] = new_io_var;
		}
		if (s == steps) st_ctn = tau::build_wff_and(st_ctn,
						tau::replace(st, changes));
		else st_ctn = tau::build_wff_and(st_ctn,
			tau::build_wff_neg(rewriter::replace<node>(st, changes)));
	}
	return st_ctn;
}

template <NodeType node>
tref make_initial_run(tref aw, const int_t max_st_lookback) {
	// get lookback of aw
	using tau = tree<node>;
	trefs io_vars = tau::get(aw).select_top(is_child<node, tau::io_var>);
	const int_t t = get_max_shift<node>(io_vars);

	tref run = nullptr;
	for (int_t i = 0; i < max_st_lookback; ++i) {
		auto current_aw = fm_at_time_point<node>(aw, io_vars, t + i);
		if (run) run = normalize_non_temp<node>(
					tau::build_wff_and(run, current_aw));
		else run = current_aw;
	}
	return run;
}

/**
 * @internal
 * @brief Given the unbounded continuation of an always-part combined with
 * eventual-variable flags (from `transform_to_eventual_variables`),
 * determine whether the flag guarding the original `sometimes` clause can
 * ever be raised, first by direct search over an initial time segment and,
 * failing that, by computing the flag's own unbounded continuation
 * (`find_fixpoint_chi`).
 *
 * Assumes `ubd_aw_continuation` is the result of computing the unbounded
 * always continuation of the always-part of the output of
 * `transform_to_eventual_variables`, and that `ev_var_flags` is the
 * `sometimes` part of that same output.
 * @tparam node Tree node type.
 * @param ubd_aw_continuation Unbounded continuation of the always-part.
 * @param ev_var_flags The `sometimes` flag-tracking formula produced
 * alongside `ubd_aw_continuation` by `transform_to_eventual_variables`.
 * @param original_aw The original (pre-continuation) always-part, `T` if
 * none, conjoined back into a satisfiable result.
 * @param start_time Time step at which execution begins.
 * @param max_st_lookback Greatest lookback among the original `sometimes`
 * clauses, as returned by `transform_to_eventual_variables`.
 * @param output When `true`, print diagnostic fixpoint information via
 * `print_fixpoint_info`.
 * @return `F` if the flag can never be raised (the `sometimes` clause is
 * unsatisfiable given the always-part); otherwise a formula describing a
 * run in which the flag is raised, conjoined with `original_aw`.
 * @endinternal
 *
 * @par Example
 * This operates on already fixpoint-transformed AST fragments, so the
 * following is conceptual rather than a literal `create_spec`/`CHECK`
 * snippet. For "smaller_lookback_one_st" in
 * tests/integration/test_integration-satisfiability1.cpp:17 (whose
 * `sometimes o2[t] = 0` was turned into a flag by
 * `transform_to_eventual_variables`), `to_unbounded_continuation` finds
 * that the flag can be raised within the initial segment (since `o2` is
 * unconstrained by the always-part), so it returns a satisfying run
 * immediately, without needing to fall back to `find_fixpoint_chi`. The
 * "flag_boundary" tests in
 * tests/integration/test_integration-solver.cpp:841-878 pin down the size
 * of that initial segment (`flag_boundary`) for specs designed to stress
 * it, for both the sat and unsat outcomes.
 */
template <NodeType node>
tref to_unbounded_continuation(tref ubd_aw_continuation,
	tref ev_var_flags, tref original_aw, const int_t start_time,
	const int_t max_st_lookback, const bool output)
{
	LOG_DEBUG << "Begin to_unbounded_continuation";

	using tau = tree<node>;
	DBG(assert(has_no_boolean_combs_of_models<node>(ubd_aw_continuation));)
	DBG(assert(is_child<node>(ev_var_flags, tau::wff_sometimes));)

	tref st_flags = tau::trim2(ev_var_flags);
	tref aw = is_child<node>(ubd_aw_continuation, tau::wff_always)
				? tau::trim2(ubd_aw_continuation)
				: ubd_aw_continuation;
	tref ori_aw_ctn = original_aw != nullptr
				? (is_child<node>(original_aw, tau::wff_always)
					? tau::trim2(original_aw)
					: original_aw)
				: tau::_T();

	trefs io_vars = tau::get(aw).select_top(is_child<node, tau::io_var>);
	trefs st_io_vars = tau::get(st_flags)
				.select_top(is_child<node, tau::io_var>);

	// Note that time_point is also the lookback
	const int_t time_point = get_max_shift<node>(io_vars);

	// There must not be a constant time constraint at this point
	assert(!tau::get(aw).find_top(is<node, tau::constraint>));

	int_t point_after_inits = get_max_initial<node>(io_vars) + 1;
	// Shift flags in order to match lookback of always part
	st_flags = shift_io_vars_in_fm<node>(
					st_flags, st_io_vars, time_point - 1);
	st_io_vars = tau::get(st_flags).select_top(is_child<node, tau::io_var>);

	// Create the initial phase of the always part
	tref run = make_initial_run<node>(ori_aw_ctn, max_st_lookback);
	// Check if flag can be raised up to the highest initial condition + 2
	// which corresponds to checking the sometimes statement up to time point
	// of the highest initial condition + 1
	const int_t s = start_time + time_point;
	// TODO: flag_boundary is upper bound, improve!
	const int_t flag_boundary =
		std::max(time_point + point_after_inits, s + time_point + 1) + 1;
	for (int_t i = s; i <= flag_boundary; ++i) {
		auto current_aw = fm_at_time_point<node>(aw, io_vars, i);
		if (run) run = tau::build_wff_and(run, current_aw);
		else run = current_aw;
		auto current_flag = fm_at_time_point<node>(st_flags, st_io_vars, i);
		auto normed_run = normalize_non_temp<node>(
					tau::build_wff_and(run, current_flag));
		if (is_run_satisfiable<node>(normed_run)) {
			LOG_DEBUG << "Flag raised at time point "<<i-time_point;
			LOG_DEBUG << LOG_FM(normed_run);
			tref res = tau::build_wff_and(normed_run, ori_aw_ctn);
			print_fixpoint_info("Temporal normalization of "
				"Tau specification did not rely on fixpoint "
				"finding, yielding the result: ",
				TAU_TO_STR(res), output);
			return res;
		}
		// Since the flag could not be raised in this step, we can add the assumption
		// that it will never be raised at this timepoint
		run = normalize_non_temp<node>(tau::build_wff_and(run,
					tau::build_wff_neg(current_flag)));
	}
	// Since flag could not be raised in the initial segment, we now check if it
	// can be raised at all. To this end we calculate chi_inf

	// Save positions of io_variables which are initial conditions
	std::set<std::pair<std::string, int_t>> initials;

	for (int_t i = 0; i < (int_t) io_vars.size(); ++i)
		if (is_io_initial<node>(io_vars[i]))
			initials.emplace(get_var_name<node>(io_vars[i]),
				get_io_time_point<node>(io_vars[i]));

	// Calculate fix point and get unbound continuation
	LOG_TRACE << "chi base: " << LOG_FM(tau::build_wff_and(aw, st_flags));

	// Find fixpoint of chi after highest initial condition
	auto [chi_inf, steps] = find_fixpoint_chi<node>(aw, st_flags, io_vars,
		initials, time_point + point_after_inits);
	chi_inf = normalize_non_temp<node>(chi_inf);

	// LOG_TRACE << "Fixpoint chi after normalize: " << chi_inf;
	if (tau::get(chi_inf).equals_F()) {
		print_fixpoint_info(
			"Temporal normalization of Tau specification reached "
			"fixpoint after " + std::to_string(steps) +
			" steps, yielding the result: ",
			TAU_TO_STR(tau::_F()), output);
		return tau::_F();
	}
	chi_inf = transform_back_non_initials<node>(chi_inf, point_after_inits-1);
	io_vars = tau::get(chi_inf).select_top(is_child<node, tau::io_var>);
	auto chi_inf_anchored = fm_at_time_point<node>(chi_inf, io_vars,
				std::max(point_after_inits, time_point));

	LOG_TRACE << "Fm to check sat: "
			<< LOG_FM(tau::build_wff_and(run, chi_inf_anchored));
	if (!is_run_satisfiable<node>(
		tau::build_wff_and(run, chi_inf_anchored)))
	{
		print_fixpoint_info(
			"Temporal normalization of Tau specification reached "
			"fixpoint after " + std::to_string(steps) +
			" steps, yielding the result: ",
			TAU_TO_STR(tau::_F()), output);
		return tau::_F();
	}
	// Here we know that the formula is satisfiable at some point
	// Since the initial segment is already checked we continue from there
	for (int_t i = flag_boundary + 1; true; ++i) {
		auto current_aw = fm_at_time_point<node>(aw, io_vars, i);
		run = tau::build_wff_and(run, current_aw);
		auto current_flag
			= fm_at_time_point<node>(st_flags, st_io_vars, i);

		auto normed_run = normalize_non_temp<node>(
					tau::build_wff_and(run, current_flag));
		// The formula is guaranteed to have be sat at some point
		// Therefore, the loop will exit eventually
		if (is_run_satisfiable<node>(normed_run)) {
			LOG_DEBUG << "Flag raised at time point "<<i-time_point;
			LOG_DEBUG << LOG_FM(normed_run);
			tref res = tau::build_wff_and(normed_run, ori_aw_ctn);
			print_fixpoint_info(
				"Temporal normalization of Tau specification "
				"reached fixpoint after "+std::to_string(steps)+
				" steps, yielding the result: ",
				TAU_TO_STR(res), output);
			return res;
		}
		// Since the flag could not be raised in this step, we can add the assumption
		// that it will never be raised at this timepoint
		run = normalize_non_temp<node>(tau::build_wff_and(run,
					tau::build_wff_neg(current_flag)));
	}
}

template <NodeType node>
tref transform_to_execution(tref fm, const int_t start_time, const bool output){
	using tau = tree<node>;
	DBG(assert(get_dnf_wff_clauses<node>(fm).size() == 1);)
	// Make sure that no function/predicate symbol is still present
	if (auto ref = tau::get(fm).find_top(is<node, tau::ref>); ref) {
		BOOST_LOG_TRIVIAL(error)
			<< "(Error) Unresolved function or predicate symbol "
			<< tau::get(ref) << " found. Returning unsat\n";
		return _F<node>();
	}
#ifdef TAU_CACHE
	using cache_t = std::map<std::pair<tref, int_t>, tref,
				subtree_pair_less<node, int_t>>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	if (auto it = cache.find(std::make_pair(fm, start_time));
		it != cache.end()) return it->second;
#endif // TAU_CACHE
	auto elim_aw = [](tref f) {
		return tau::get(f)
			.child_is(tau::wff_always) ? tau::trim2(f) : f;
	};
	LOG_DEBUG << "Start transform_to_execution: " << LOG_FM(fm);

	tref aw_fm = tau::get(fm).find_top(is_child<node, tau::wff_always>);
	std::pair<tref, int_t> ev_t;
	tref ubd_aw_fm = nullptr;
	if (aw_fm != nullptr) {
		// If there is an always part, replace it with its unbound continuation
		ubd_aw_fm = always_to_unbounded_continuation<node>(
						aw_fm, start_time, output);
		auto ubd_fm = rewriter::replace<node>(fm, aw_fm,
					tau::build_wff_always(ubd_aw_fm));
		ev_t = transform_to_eventual_variables<node>(
						ubd_fm, false, start_time);
		// Check if there is a sometimes present
		if (ev_t.first == ubd_fm) {
#ifdef TAU_CACHE
			cache.emplace(std::make_pair(
				elim_aw(ubd_fm), start_time), elim_aw(ubd_fm));
			return cache.emplace(std::make_pair(fm, start_time),
					     elim_aw(ubd_fm)).first->second;
#endif // TAU_CACHE
			return elim_aw(ubd_fm);
		}
	} else {
		ev_t = transform_to_eventual_variables<node>(
							fm, true, start_time);
		// Check if there is a sometimes present
		if (ev_t.first == fm) {
			// Here we deal with a non-temporal formula
			// Use aw_fm to store result
			aw_fm = elim_aw(fm);
			if (!is_non_temp_nso_satisfiable<node>(fm))
				aw_fm = tau::_F();
#ifdef TAU_CACHE
			return cache.emplace(std::make_pair(fm, start_time),
					     aw_fm).first->second;
#endif // TAU_CACHE
			return aw_fm;
		}
	}
	auto aw_after_ev = tau::get(ev_t.first)
				.find_top(is_child<node, tau::wff_always>);
	if (aw_after_ev == nullptr) {
#ifdef TAU_CACHE
		return cache.emplace(std::make_pair(fm, start_time),
				     elim_aw(fm)).first->second;
#endif // TAU_CACHE
		return elim_aw(fm);
	}
	trefs st = tau::get(ev_t.first)
				.select_top(is_child<node, tau::wff_sometimes>);
	DBG(assert(st.size() < 2);)

	tref res;
	if (!tau::get(aw_after_ev).equals_F() && !st.empty())
		res = normalize_non_temp<node>(
			to_unbounded_continuation<node>(
				aw_after_ev, st[0], ubd_aw_fm, start_time,
				ev_t.second, output));
	else res = aw_after_ev;
	res = elim_aw(res);
	LOG_DEBUG << "End transform_to_execution: " << LOG_FM(res);
#ifdef TAU_CACHE
	cache.emplace(std::make_pair(res, start_time), res);
	return cache.emplace(std::make_pair(fm, start_time),
			     res).first->second;
#endif // TAU_CACHE
	return res;
}

template <NodeType node>
bool is_tau_formula_sat(tref fm, const int_t start_time, const bool output) {
	using tau = tree<node>;
	LOG_DEBUG << "Start is_tau_formula_sat: " << LOG_FM(fm);
	tref normalized_fm = normalize_with_temp_simp<node>(fm);
	// Convert each disjunct to unbounded continuation
	for (tref clause : expression_paths<node>(normalized_fm)) {
		if (!tau::get(transform_to_execution<node>(
			clause, start_time, output)).equals_F()) {
			LOG_DEBUG << "End is_tau_formula_sat: true";
			return true;
		}
	}
	LOG_DEBUG << "End is_tau_formula_sat: false";
	return false;
}

// Check for temporal formulas if f1 implies f2
template <NodeType node>
bool is_tau_impl(tref f1, tref f2) {
	using tau = tree<node>;
	tref f1_norm = normalize<node>(f1);
	tref f2_norm = normalize<node>(f2);
	tref imp_check = normalize_with_temp_simp<node>(
		tau::build_wff_neg(tau::build_wff_imply(f1_norm, f2_norm)));
	// Now check that each disjunct is not satisfiable
	for (tref c : expression_paths<node>(imp_check)) {
		auto ctn = transform_to_execution<node>(c);
		if (!tau::get(ctn).equals_F()) return false;
	}
	return true;
}

// The formulas need to be closed
template <NodeType node>
bool are_tau_equivalent(tref f1, tref f2) {
	using tau = tree<node>;
	// Negate equivalence for unsat check
	tref f1_norm = normalize<node>(f1);
	tref f2_norm = normalize<node>(f2);
	tref equiv_check = normalize_with_temp_simp<node>(
		tau::build_wff_neg(tau::build_wff_equiv(f1_norm, f2_norm)));
	// Now check that each disjunct is not satisfiable
	for (const auto& c : expression_paths<node>(equiv_check)) {
		auto ctn = transform_to_execution<node>(c);
		if (!tau::get(ctn).equals_F()) return false;
	}
	return true;
}

template <NodeType node>
tref simp_tau_unsat_valid(tref fm, const int_t start_time, const bool output) {
	using tau = tree<node>;
	LOG_DEBUG << "Start simp_tau_unsat_valid: " << LOG_FM(fm);
	// Check if formula is valid
	if (is_tau_impl<node>(tau::_T(), fm)) return tau::_T();
	tref normalized_fm = normalize_with_temp_simp<node>(fm);
	trefs clauses = {tau::_F()};
	// Check satisfiability of each clause
	for (tref clause: expression_paths<node>(normalized_fm))
		if (!tau::get(transform_to_execution<node>(
			clause, start_time, output)).equals_F()) {
			clauses.push_back(clause);
		}
	tref res = tau::build_wff_or(clauses);
	LOG_DEBUG << "End simp_tau_unsat_valid: " << LOG_FM(res);
	return res;
}

/*
 *  Possible tests:
 *  (o1[t-1] = 0 -> o1[t] = 1) && (o1[t-1] = 1 -> o1[t] = 0) && o1[0] = 0, passing
 *  o1[0] = 0 && o1[t] = 0 -> o1[0] = 0 && o1[t] = 0, passing
 *  o1[t] = i1[t] && o1[3] = 0 -> F, passing
 *  o1[t-1] = i1[t] -> F, passing
 *  o1[t-2] = 0 && o1[1] = 0 -> should be o1[t-2] = 0 && o1[t-1] = 0 && o1[t] = 0 && o1[1] = 0, passing
 */

} // namespace idni::tau_lang
