// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "satisfiability.h"
#include "normalizer.h"
#include "ltl_aba.h"
#include "boolean_algebras/ba_pack_traits.h"
#include "heuristics/preprocess_placement.h"

#include <cstdlib>
#include <iostream>
#include <optional>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "satisfiability"

namespace idni::tau_lang {

inline static bool use_debug_output_in_sat = false;

/// Cap on the fixpoint searches in `find_fixpoint_phi`/`find_fixpoint_chi`;
/// 0 = unlimited. A runtime parameter by policy, never a header constant —
/// set via `--max-fixpoint-steps` or REPL `fixpointsteps`, or
/// `api::set_max_fixpoint_steps`. SO-1 caveat: these searches have no
/// convergence guarantee, so an unlimited run on a non-converging spec does
/// not terminate. The shipped default is therefore FINITE (500, the value the
/// pre-parameter constant had): a give-up is wrong-but-loud where an
/// unlimited run hangs `sat`/`run`/pointwise revision. Pass 0 to opt into
/// unlimited. The CLI default in main.cpp must agree with this value.
inline size_t max_fixpoint_steps = 500;

/// Functional continuation. Every iterate of `find_fixpoint_phi` extends
/// the previous one by exactly one quantified block for the newest time
/// point, `all inputs ex outputs (step)`, conjoined in the innermost
/// scope, where the previous time point's outputs are bound. A
/// specification of functional shape (`functional_step_shape`: every
/// output of a time point defined by one equation or one conditional tree
/// over the inputs, the earlier outputs and the other outputs of the time
/// point, without a cycle) has a block that holds for every value of what
/// it reads from outside, so conjoining it changes nothing: `C && B == C`
/// pointwise, the iterate equals the previous one and the implication
/// between the two holds. The shape settles the fixpoint, the run check
/// and the constant closure without deciding the implication, which on
/// such a specification puts the whole telescope, with the previous
/// outputs existentially bound in negative polarity, in front of the
/// quantified solver.
/// 0 = the implication only; 1 = the shape where it applies, the
/// implication elsewhere (the default); 2 = shadow: the implication
/// decides every check, `functional_continuation_shape_hits` counts the
/// checks the shape would settle and `functional_continuation_mismatches`
/// those among them whose implication does not hold. The environment
/// variable TAU_FUNCTIONAL_CONTINUATION (0, 1 or 2; any other value
/// selects 0) overrides the flag.
inline int functional_continuation = 1;
/// Fixpoint checks made with the switch at 1 or 2.
inline size_t functional_continuation_checks = 0;
/// Fixpoint checks the functional shape settles (at 1) or would settle
/// (at 2).
inline size_t functional_continuation_shape_hits = 0;
/// Shadow mode: checks the shape would settle whose implication does not
/// hold.
inline size_t functional_continuation_mismatches = 0;
/// Shadow mode: checks the shape would settle whose implication the
/// normalizer could not decide (no verdict, counted apart from a decided
/// contradiction).
inline size_t functional_continuation_undecided = 0;
/// Run and constant closures settled by the functional shape.
inline size_t functional_continuation_closure_skips = 0;
inline int functional_continuation_mode() {
	static const std::optional<int> env = []() -> std::optional<int> {
		const char* v = std::getenv("TAU_FUNCTIONAL_CONTINUATION");
		if (!v || !*v) return std::nullopt;
		return v[0] == '2' ? 2 : v[0] == '1' ? 1 : 0;
	}();
	// When selected through the environment variable, the shadow mode
	// reports its counts once, at exit, so a whole run can be checked.
	static const bool report = []() {
		if (env && *env == 2) std::atexit([]() {
			std::cerr << "functional continuation shadow: checks "
				<< functional_continuation_checks << ", by shape "
				<< functional_continuation_shape_hits << ", mismatches "
				<< functional_continuation_mismatches << ", undecided "
				<< functional_continuation_undecided << ", closures skipped "
				<< functional_continuation_closure_skips << std::endl;
		});
		return true;
	}();
	(void)report;
	return env ? *env : functional_continuation;
}

/// Cap on `to_unbounded_continuation`'s eventual-flag search past the flag
/// boundary; 0 = unlimited. Same SO-1 caveat as `max_fixpoint_steps` — and a
/// bounded give-up here reports unsatisfiable, which is wrong but bounded and
/// loud. Shipped default 500 (finite, see above); set via
/// `--max-flag-search-steps`, REPL `flagsteps`, or
/// `api::set_max_flag_search_steps`.
inline size_t max_flag_search_steps = 500;

/**
 * @brief Fingerprint of every runtime parameter that can change a
 * satisfiability or realizability verdict: the two temporal-normalization
 * caps above, the master preprocessing switch, the functional-continuation
 * switch, the options the algebras of
 * the pack declare (`pack_ba_options_fingerprint`) and the LTL(ABA) knobs
 * (`ltl_verdict_budget_fingerprint`). The verdict memos in this file are
 * keyed on the formula only and drop their entries when it changes. (The
 * semantic PWR fallback lives in pointwise_revision.h, which includes this
 * header; it steers the revision, not these memos.)
 */
template <NodeType node>
size_t verdict_budget_fingerprint() {
	size_t seed = 0;
	auto mix = [&seed](size_t v) {
		seed ^= v + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
	};
	mix(max_fixpoint_steps);
	mix(max_flag_search_steps);
	mix(preprocessing);
	// The switch selects the path a continuation is settled on; a memoized
	// continuation is not reused across its settings.
	mix(functional_continuation_mode());
	return ltl_verdict_budget_fingerprint(
		pack_ba_options_fingerprint<node>(seed));
}

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
	// The input/output data bit is assigned while parsing a *spec*; it is not
	// derived from the `i`/`o` name prefix. So an io_var that reached here
	// without going through spec parsing is neither is_input_variable() nor
	// is_output_variable(), and testing only the former and falling to an
	// `else` silently rebuilt such a variable as an *output*:
	// transform_io_var(i1[t-2], 5) returned i1[3] with is_output_variable()
	// == true. Every production caller does work on spec-parsed trees, which
	// is exactly why the unclassified case must be loud rather than defaulted
	// -- it is otherwise a wrong answer nothing reports.
	if (tau::get(io_var).is_input_variable())
		return tau::trim(build_in_var_at_n<node>(
			get_var_name_node<node>(io_var), time_point - shift, type));
	if (tau::get(io_var).is_output_variable())
		return tau::trim(build_out_var_at_n<node>(
			get_var_name_node<node>(io_var), time_point - shift, type));
	DBG(assert(false && "transform_io_var: io_var is neither input nor output");)
	LOG_ERROR << "transform_io_var: " << LOG_FM(io_var) << " is classified"
		" neither as input nor as output (the in/out bit comes from spec"
		" parsing, not from the name prefix); returning it unchanged rather"
		" than rebuilding it as an output variable.";
	return io_var;
}

/**
 * @internal
 * @brief Prefix @p fm with `ex` quantifiers over every output stream at
 * @p time_point — the output half of the time-compatible quantification
 * pattern (see README.md, "Satisfiability and execution").
 *
 * Constant-time (initial-condition) variables in @p io_vars are skipped,
 * as are streams whose value at @p time_point is predefined by
 * @p initials; each remaining output stream name is quantified once.
 * The bound variables are deliberately not renamed: the phi unrolling
 * (`build_step`) splices later steps underneath the quantifiers built
 * here, and their lookback occurrences must be captured by exactly these
 * binders.
 * @tparam node Tree node type.
 * @param fm Formula to quantify.
 * @param io_vars IO variable nodes of the surrounding formula.
 * @param time_point Time step whose output values are quantified.
 * @param initials Set of `(variable name, time point)` pairs predefined
 * by explicit initial conditions.
 * @return @p fm wrapped in one `ex o[time_point]` per output stream.
 * @endinternal
 */
template <NodeType node>
tref existentially_quantify_output_streams(tref fm, const trefs& io_vars,
                                int_t time_point, const auto& initials)
{
	using tau = tree<node>;
	// This map is needed in order to get the minimal shift for streams with same name
	std::set<int_t> quantifiable_o_vars;
	for (int_t i = 0; i < (int_t) io_vars.size(); ++i) {
		// Skip input streams. An io_var classified as neither (see
		// transform_io_var) would be quantified existentially below as if it
		// were an output; make that loud instead of defaulting silently.
		DBG(assert(tau::get(io_vars[i])[0].is_input_variable()
			|| tau::get(io_vars[i])[0].is_output_variable());)
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

/**
 * @internal
 * @brief Prefix @p fm with `all` quantifiers over every input stream at
 * @p time_point — the input half of the time-compatible quantification
 * pattern, dual to `existentially_quantify_output_streams`.
 *
 * Constant-time (initial-condition) variables in @p io_vars are skipped,
 * as are streams whose value at @p time_point is predefined by
 * @p initials; each remaining input stream name is quantified once, and
 * the bound variables are not renamed, for the same splicing reason as
 * in `existentially_quantify_output_streams`. Applied after that
 * function by the step builders, so the `all` binders end up outermost:
 * `all i[n] ex o[n] ...`.
 * @tparam node Tree node type.
 * @param fm Formula to quantify.
 * @param io_vars IO variable nodes of the surrounding formula.
 * @param time_point Time step whose input values are quantified.
 * @param initials Set of `(variable name, time point)` pairs predefined
 * by explicit initial conditions.
 * @return @p fm wrapped in one `all i[time_point]` per input stream.
 * @endinternal
 */
template <NodeType node>
tref universally_quantify_input_streams(tref fm, const trefs& io_vars,
					int_t time_point, const auto& initials)
{
	using tau = tree<node>;
	// This map is needed in order to get the minimal shift for streams with same name
	std::set<int_t> quantifiable_i_vars;
	for (int_t i = 0; i < (int_t)io_vars.size(); ++i) {
		// SO-10: fail loudly on an unclassified io_var, like the
		// existential sibling does.
		DBG(assert(tau::get(io_vars[i])[0].is_input_variable()
			|| tau::get(io_vars[i])[0].is_output_variable());)
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
		const size_t type = get_ba_type_id<node>(pack_bool_carrier_type<node>());
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
	// The above is exhaustive for the possible children of a constraint;
	// the grammar admits only these kinds, so an unrecognized one is a
	// shape error caught in debug. Return _0 to keep the caller total.
	DBG(assert(false && "Unrecognized constraint kind");)
	return to_ba(false);
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

	// Unlike calculate_ctn, which branches on whether the numeral is the left
	// or the right operand, the tests below are deliberately side-agnostic.
	// The initial phase is "up to and including the time point at which the
	// flag stream has settled", and that point depends only on `condition`,
	// not on the direction of the comparison: `c >= t` settles at t = c and
	// `t >= c` settles at t = c as well (the flag is constant on either side
	// of c), so both are covered by `condition >= time_point`; `c > t` and
	// `t > c` likewise both settle one step later, hence the `+ 1`. The bound
	// is intentionally an over-approximation -- an extra initial condition is
	// harmless, a missing one is not.
	if (t | tau::ctn_gteq) return condition >= time_point;
	if (t | tau::ctn_gt)   return condition + 1 >= time_point;
	if (t | tau::ctn_lteq) return condition + 1 >= time_point;
	if (t | tau::ctn_lt)   return condition >= time_point;

	// The above is exhaustive for the possible children of a constraint;
	// the grammar admits only these kinds.
	DBG(assert(false && "Unrecognized constraint kind");)
	return false;
}

template <NodeType node>
tref fm_at_time_point(tref original_fm, const trefs &io_vars, int_t time_point) {
	subtree_map<node, tref> changes;
	for (size_t i = 0; i < io_vars.size(); ++i)
		changes[io_vars[i]] =
				transform_io_var<node>(io_vars[i], time_point);
	return rewriter::replace<node>(original_fm, changes);
}

/**
 * @internal
 * @brief Extend the phi unrolling (`find_fixpoint_phi`) by one step:
 * instantiate @p original_fm at time `time_point + step_num`, wrap it in
 * that step's time-compatible quantifier prefix, and splice it into the
 * innermost position of the telescope built so far.
 *
 * The new step is quantified `all inputs ex outputs` at its own time
 * point and conjoined underneath every earlier step's quantifiers by
 * replacing @p cached_fm — the previous step's unquantified
 * instantiation — with `cached_fm && <quantified new step>`. Writing
 * `fm@k` for the instantiation at `time_point + k`, the result has the
 * shape `fm@0 && all i@1 ex o@1 (fm@1 && all i@2 ex o@2 (fm@2 && ...))`;
 * lookback occurrences inside `fm@k` are intentionally captured by the
 * earlier steps' binders (see `existentially_quantify_output_streams`).
 * @tparam node Tree node type.
 * @param original_fm Always-part local specification to instantiate.
 * @param prev_fm Telescope built so far (steps `0..step_num-1`).
 * @param io_vars IO variable nodes appearing in @p original_fm.
 * @param initials Set of `(variable name, time point)` pairs predefined
 * by explicit initial conditions (not re-quantified).
 * @param step_num Index of the step to add; must be `> 0` (step 0 is
 * built by `fm_at_time_point` in `find_fixpoint_phi`).
 * @param time_point Time step at which the unrolling started.
 * @param cached_fm [in,out] Splice cursor: on entry the previous step's
 * unquantified instantiation, on exit the new step's.
 * @return @p prev_fm with the new step spliced in.
 * @endinternal
 */
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

/**
 * @internal
 * @brief Move every constant-time IO variable of @p fm whose time point is at
 * least @p from by @p delta steps.
 *
 * Variables below @p from (initial conditions) stay put. All occurrences are
 * replaced at once, so a variable moved onto the time point of another one
 * is not moved again.
 * @tparam node Tree node type.
 * @param fm Formula whose IO variables all refer to constant time points.
 * @param from Smallest time point that moves.
 * @param delta Number of steps to move by; may be negative.
 * @return @p fm with the moved variables.
 * @endinternal
 */
template <NodeType node>
tref shift_state_io_vars(tref fm, int_t from, int_t delta) {
	using tau = tree<node>;
	subtree_map<node, tref> changes;
	for (tref v : tau::get(fm).select_top(is_child<node, tau::io_var>)) {
		DBG(assert(is_io_initial<node>(v));)
		const int_t tp = get_io_time_point<node>(v);
		if (tp < from || changes.contains(v)) continue;
		const size_t type = tau::get(v).get_ba_type();
		tref name = get_var_name_node<node>(v);
		changes.emplace(v, tau::trim(tau::get(v).is_input_variable()
			? build_in_var_at_n<node>(name, tp + delta, type)
			: build_out_var_at_n<node>(name, tp + delta, type)));
	}
	return rewriter::replace<node>(fm, changes);
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
result<bool> is_run_satisfiable(tref fm) {
	result<bool> r;
	using tau = tree<node>;

	DBG(LOG_TRACE
		<< "is_run_satisfiable begin\n"
		<< "is_run_satisfiable[fm]: " << LOG_FM(fm);)

	if (!fm) {
		return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
	}

	const auto& t = tau::get(fm);
	if (t.equals_F()) { return r.with_assert_check_value(false); }
	if (t.equals_T()) { return r.with_assert_check_value(true); }

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
			sat_fm = tau::build_wff_all(
				io_vars.back(), sat_fm, false);
		else sat_fm = tau::build_wff_ex(
			io_vars.back(), sat_fm, false);
		io_vars.pop_back();
	}

	DBG(LOG_TRACE << "is_run_satisfiable[sat_fm]: " << LOG_FM(sat_fm));

	TAU_TRY(tref sat_normed, normalize_non_temp<node>(sat_fm));
	TAU_TRY(bool sat, is_non_temp_nso_satisfiable<node>(sat_normed));
	r = sat;

	DBG(LOG_TRACE
		<< "is_run_satisfiable[result]: " << r.value() << "\n"
		<< "is_run_satisfiable end\n");

	DBG(assert(r.is_well_formed());)
	return r;
}

// Assumption is that the provided fm is an unbound continuation
template <NodeType node>
tref get_uninterpreted_constants_constraints(tref fm, trefs& io_vars, const int_t start_time,
	const bool functional, const int_t spec_max_initial)
{
	using tau = tree<node>;
	// A continuation of functional shape without uninterpreted constants
	// has nothing to constrain, and its closure is T by the same
	// argument that settles its fixpoint, unless an initial condition of
	// the specification lies at or beyond the time point the closure
	// examines (`spec_max_initial`, the specification's own initial
	// conditions: the continuation also carries the run prefix at fixed
	// time points, whose conjuncts are instances of the step).
	// Substitute lookback as current time point
	int_t look_back = get_max_shift<node>(io_vars);
	if (functional
		&& spec_max_initial < look_back + start_time
		&& tau::get(fm).find_top(is_child<node, tau::uconst_name>) == nullptr) {
		++functional_continuation_closure_skips;
		// In the shadow mode the closure is computed as well.
		if (functional_continuation_mode() == 1) return tau::_T();
	}
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

		// free_io_vars is a copy of get_free_vars' result, which is sorted
		// by subtree_less, so locate v by binary search instead of scanning
		// the whole vector per io_var.
		if (auto it = std::lower_bound(free_io_vars.begin(),
			free_io_vars.end(), v, tau::subtree_less);
			it != free_io_vars.end() && tau::subtree_equals(*it, v))
				free_io_vars.erase(it);
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
	if (auto normed = normalize_non_temp<node>(uconst_ctns);
		normed.has_value()) uconst_ctns = normed.value();
	else {
		LOG_ERROR << "get_uninterpreted_constants_constraints: "
			"normalization failed; propagating failure.";
		return nullptr;
	}
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
/// Whether the normalizer decides two formulas equivalent, closed
/// universally over every variable and stream occurrence they mention:
/// 1 equivalent, 0 not, -1 undecided.
template <NodeType node>
int closed_equivalence(tref a, tref b) {
	using tau = tree<node>;
	if (tau::get(a) == tau::get(b)) return 1;
	tref eq = tau::build_wff_equiv(a, b);
	trefs outside = get_free_vars<node>(eq);
	subtree_set<node> seen;
	for (tref v : outside) seen.insert(v);
	for (tref v : tau::get(eq).select_top(is_child<node, tau::io_var>)) {
		tref k = tau::trim_right_sibling(v);
		if (seen.insert(k).second) outside.push_back(k);
	}
	auto r = normalize_non_temp<node>(tau::build_wff_all_many(outside, eq));
	if (!r.has_value()) return -1;
	if (tau::get(r.value()).equals_T()) return 1;
	if (tau::get(r.value()).equals_F()) return 0;
	return -1;
}

/// The formula with the operands of every conjunction and disjunction
/// flattened and sorted: two formulas that differ only in the order and
/// nesting of their conjuncts and disjuncts have one such form.
template <NodeType node>
tref ac_canonical(tref f) {
	using tau = tree<node>;
	const tau& t = tau::get(f);
	if (!(t.is(tau::wff) && (t.child_is(tau::wff_and) || t.child_is(tau::wff_or)))) return f;
	const bool is_and = t.child_is(tau::wff_and);
	std::vector<tref> ops;
	std::function<void(tref)> flat = [&](tref m) {
		const tau& tm = tau::get(m);
		if (tm.is(tau::wff) && tm.child_is(is_and ? tau::wff_and : tau::wff_or)) { flat(tm[0].first()); flat(tm[0].second()); }
		else ops.push_back(ac_canonical<node>(m)); };
	flat(f);
	std::sort(ops.begin(), ops.end(), tau::subtree_less);
	tref r = ops[0];
	for (size_t i = 1; i < ops.size(); ++i) r = is_and ? tau::build_wff_and(r, ops[i]) : tau::build_wff_or(r, ops[i]);
	return r;
}

/// The always-part of a specification as written, for `functional_step_shape`:
/// the bodies of the always statements standing as its top-level conjuncts
/// (the normalizer merges them into one), or the whole formula when it has no
/// temporal operator (an implicit always). Nullptr for anything else: a
/// sometimes, an always below a negation, a disjunction or another always, or
/// a constant time constraint, under which the step is not the same at every
/// time point.
template <NodeType node>
tref functional_shape_body(tref fm) {
	using tau = tree<node>;
	if (!fm) return nullptr;
	const tau& t = tau::get(fm);
	if (t.find_top(is_child<node, tau::wff_sometimes>)
		|| t.find_top(is<node, tau::constraint>)) return nullptr;
	if (!t.find_top(is_child<node, tau::wff_always>)) return fm;
	std::vector<tref> conjs;
	std::function<void(tref)> conjuncts = [&](tref n) {
		const tau& tn = tau::get(n);
		if (tn.is(tau::wff) && tn.child_is(tau::wff_and)) { conjuncts(tn[0].first()); conjuncts(tn[0].second()); }
		else conjs.push_back(n); };
	conjuncts(fm);
	tref body = nullptr;
	for (tref c : conjs) {
		const tau& tc = tau::get(c);
		if (!(tc.is(tau::wff) && tc.child_is(tau::wff_always))) return nullptr;
		tref b = tau::trim2(c);
		if (tau::get(b).find_top(is_child<node, tau::wff_always>)) return nullptr;
		body = body ? tau::build_wff_and(body, b) : b;
	}
	return body;
}

/**
 * @brief Functional shape of a specification's always-part.
 *
 * The body, at its symbolic time point and before any normalization, is a
 * conjunction whose conjuncts are of three kinds: initial conditions
 * (every stream at a fixed time point), constraints that mention no output
 * of the current time point and hold for every value of what they mention
 * (decided by normalizing their universal closure), and clauses. The
 * clauses of a conjunct are its disjuncts, a disjunction or conjunction
 * that holds no definition standing as one literal (a guard), a
 * conjunction that holds one distributing (a conditional's branch that
 * holds a nested conditional; a conjunct distributing into more than 64
 * clauses is not recognized). A definition is an equation of an output of
 * the current time point standing as a disjunct of a clause, the output
 * not in the other side; an equation standing as a conjunct of a
 * conjunction reads the output, and an equation the body also negates as
 * a disjunct is a guard. Every clause holds exactly one definition, its
 * guard is free of the output it defines, and the cells of an output (its
 * clauses' guards and witnesses) form one conditional tree: a guard
 * literal occurs in every cell, in one sign on the cells of one branch and
 * in the other sign on those of the other, and dropping it and recursing
 * on both branches ends in single cells with no guard left. Guard literals
 * are compared in negation normal form with sorted operands and, failing
 * that, on the closed equivalence. The definitions read each other without
 * a cycle.
 *
 * Cells forming a conditional tree are total and exclusive, so on every
 * assignment exactly one cell forces the output to its witness; evaluated
 * in dependency order that assignment satisfies every clause, the
 * constraints hold on their own and the initial conditions are conjuncts
 * of every iterate. Hence `all inputs ex outputs (body)` holds for every
 * value of what the body reads from outside, the block a time point adds
 * changes nothing, and the continuation reaches its fixpoint as soon as
 * the lookback is covered (see `find_fixpoint_phi`).
 * @return true when the body is of the shape; false otherwise, and for
 * everything the rules above do not cover.
 */
template <NodeType node>
bool functional_step_shape(tref body) {
	using tau = tree<node>;
	if (!body) return false;
	auto say = [&](const char* why, tref at = nullptr) {
		LOG_DEBUG << "functional shape: not functional, " << why
			<< (at ? ": " : "") << (at ? tau::get(at).to_str() : std::string());
		return false; };
	auto key = [](tref a) { return tau::trim_right_sibling(a); };
	// v: a variable node; an output stream of the current time point
	auto current_output = [](tref v) {
		if (!tau::get(v).child_is(tau::io_var)) return false;
		return tau::get(io_var_node<node>(v)).is_output_variable()
			&& !is_io_initial<node>(v) && get_io_var_shift<node>(v) == 0; };
	auto outputs_in = [&](tref n, subtree_set<node>& out) {
		for (tref v : tau::get(n).select_top(is_child<node, tau::io_var>))
			if (current_output(v)) out.insert(key(v)); };
	auto mentions = [&](tref n, tref o) {
		subtree_set<node> vs; outputs_in(n, vs);
		return vs.contains(key(o)); };
	// An equation `o = c` of an output is a definition unless the body also
	// tests it, i.e. `o != c` occurs somewhere: the guards of a conditional
	// occur in both signs, a definition never does.
	auto eq_key = [&](tref l, tref r) { return tau::trim_right_sibling(tau::build_bf_eq(l, r)); };
	// A test is a negated equation standing as a disjunct of a conjunct
	// (the guard of a conditional as the parser builds it, `!(o = c)` or
	// `o != c`); a negated equation inside a guard conjunction reads an
	// output, it does not test its definition.
	subtree_set<node> tested;
	auto negated_equation = [&](tref m) -> tref {
		const tau& t = tau::get(m);
		if (!t.is(tau::wff)) return nullptr;
		if (t.child_is(tau::bf_neq)) return m;
		if (t.child_is(tau::wff_neg)) {
			tref inner = t[0].first();
			if (tau::get(inner).is(tau::wff) && tau::get(inner).child_is(tau::bf_eq))
				return inner;
		}
		return nullptr; };
	{
		std::vector<tref> cs0;
		std::function<void(tref)> conj0 = [&](tref n) {
			const tau& t = tau::get(n);
			if (t.is(tau::wff) && t.child_is(tau::wff_and)) { conj0(t[0].first()); conj0(t[0].second()); }
			else cs0.push_back(n); };
		conj0(body);
		std::function<void(tref)> disj0 = [&](tref m) {
			const tau& t = tau::get(m);
			if (t.is(tau::wff) && t.child_is(tau::wff_or)) { disj0(t[0].first()); disj0(t[0].second()); return; }
			if (tref e = negated_equation(m); e)
				tested.insert(eq_key(tau::get(e)[0].first(), tau::get(e)[0].second())); };
		for (tref c : cs0) disj0(c);
	}
	// d: a disjunct; the output it defines, if it is a definition
	auto equation_of = [&](tref d, tref& side, tref& term) -> bool {
		const tau& td = tau::get(d);
		if (!(td.is(tau::wff) && td.child_is(tau::bf_eq))) return false;
		tref l = td[0].first(), r = td[0].second();
		if (tested.contains(eq_key(l, r))) return false;
		if (tau::get(l).child_is(tau::variable) && current_output(tau::get(l).first())) { side = tau::get(l).first(); term = r; }
		else if (tau::get(r).child_is(tau::variable) && current_output(tau::get(r).first())) { side = tau::get(r).first(); term = l; }
		else return false;
		return !mentions(term, side);
	};
	// A formula holds a definition when an equation of an output stands as
	// a disjunct of one of its clauses, as the parser leaves a conditional's
	// branch (`!g || o = c`). An equation standing as a conjunct of a
	// conjunction is a reader of that output, as in a guard made of a
	// conjunction, and holds no definition.
	std::function<bool(tref)> has_definition = [&](tref n) -> bool {
		const tau& t = tau::get(n);
		if (!t.is(tau::wff)) return false;
		if (t.child_is(tau::wff_and)) return has_definition(t[0].first()) || has_definition(t[0].second());
		if (t.child_is(tau::wff_or)) {
			std::vector<tref> ds;
			std::function<void(tref)> flat = [&](tref m) {
				const tau& tm = tau::get(m);
				if (tm.is(tau::wff) && tm.child_is(tau::wff_or)) { flat(tm[0].first()); flat(tm[0].second()); }
				else ds.push_back(m); };
			flat(n);
			for (tref d : ds) {
				tref side, term;
				if (equation_of(d, side, term)) return true;
				if (tau::get(d).is(tau::wff) && tau::get(d).child_is(tau::wff_and) && has_definition(d)) return true;
			}
		}
		return false; };
	// The conjuncts of the body, and the clauses of a conjunct: a conjunct
	// `G || (P && Q)` (a conditional's branch holding a nested conditional)
	// distributes into `(G || P), (G || Q)`; capped, a conjunct beyond the
	// cap is not recognized.
	std::vector<tref> conjs;
	std::function<void(tref)> conjuncts = [&](tref n) {
		const tau& t = tau::get(n);
		if (t.is(tau::wff) && t.child_is(tau::wff_and)) { conjuncts(t[0].first()); conjuncts(t[0].second()); }
		else conjs.push_back(n); };
	conjuncts(body);
	using clause = std::vector<tref>;   // disjuncts
	std::function<bool(tref, std::vector<clause>&)> clauses_of = [&](tref n, std::vector<clause>& out) -> bool {
		// a conjunction: the clauses of both sides
		if (const tau& tn = tau::get(n); tn.is(tau::wff) && tn.child_is(tau::wff_and))
			return clauses_of(tn[0].first(), out) && clauses_of(tn[0].second(), out);
		std::vector<tref> ds;
		std::function<void(tref)> disjuncts = [&](tref m) {
			const tau& t = tau::get(m);
			// a disjunction that defines nothing is a guard and stays one
			// literal (the complement of the guard conjunction next to it)
			if (t.is(tau::wff) && t.child_is(tau::wff_or) && has_definition(m)) { disjuncts(t[0].first()); disjuncts(t[0].second()); }
			else ds.push_back(m); };
		disjuncts(n);
		std::vector<clause> acc{ {} };
		for (tref d : ds) {
			const tau& t = tau::get(d);
			// a conjunction that defines nothing is a guard and stays one
			// literal; a branch holding a nested conditional distributes
			if (t.is(tau::wff) && t.child_is(tau::wff_and) && has_definition(d)) {
				std::vector<clause> inner;
				if (!clauses_of(d, inner)) return false;
				std::vector<clause> next;
				for (const clause& a : acc) for (const clause& c : inner) {
					clause m = a; m.insert(m.end(), c.begin(), c.end()); next.push_back(std::move(m));
					if (next.size() > 64) return false;
				}
				acc = std::move(next);
			} else for (clause& a : acc) a.push_back(d);
		}
		out.insert(out.end(), acc.begin(), acc.end());
		return true;
	};
	// The clauses of every conjunct that mentions an output; a conjunct
	// without one is an initial condition or a constraint that must hold
	// on its own.
	std::vector<clause> all_clauses;
	for (tref c : conjs) {
		const tau& tc = tau::get(c);
		subtree_set<node> outs; outputs_in(c, outs);
		if (outs.empty()) {
			const trefs vs = tc.select_top(is_child<node, tau::io_var>);
			bool all_initial = !vs.empty();
			for (tref v : vs) if (!is_io_initial<node>(v)) { all_initial = false; break; }
			if (all_initial) continue;
			trefs outside = get_free_vars<node>(c);
			subtree_set<node> seen;
			for (tref v : outside) seen.insert(v);
			for (tref v : vs) { tref k = key(v); if (seen.insert(k).second) outside.push_back(k); }
			auto nres = normalize_non_temp<node>(tau::build_wff_all_many(outside, c));
			if (!nres.has_value() || !tau::get(nres.value()).equals_T())
				return say("a constraint without outputs is not valid", c);
			continue;
		}
		if (!clauses_of(c, all_clauses)) return say("a conjunct does not flatten", c);
	}
	// In a clause exactly one equation is a definition; the other
	// disjuncts are its guard.
	struct cell { clause guard; tref witness; };
	subtree_map<node, std::vector<cell>> cells;
	subtree_map<node, subtree_set<node>> deps;
	subtree_set<node> all; outputs_in(body, all);
	// An output with a bare definition (a clause that is just its
	// equation) is defined by it; a guarded clause equating it with
	// another output defines that other output.
	subtree_set<node> bare;
	for (const clause& cl : all_clauses)
		if (cl.size() == 1) { tref side, term; if (equation_of(cl[0], side, term)) bare.insert(key(side)); }
	// the side of an equation between two outputs that the clause defines
	auto equation_of_clause = [&](tref d, bool guarded, tref& side, tref& term) -> bool {
		if (!equation_of(d, side, term)) return false;
		if (!guarded) return true;
		const tau& td = tau::get(d);
		tref l = td[0].first(), r = td[0].second();
		tref other = tau::get(l).first() == side ? r : l;
		if (bare.contains(key(side)) && tau::get(other).child_is(tau::variable)
			&& current_output(tau::get(other).first()) && !bare.contains(key(tau::get(other).first()))) {
			side = tau::get(other).first(); term = tau::get(l).first() == side ? r : l;
			return !mentions(term, side);
		}
		return true;
	};
	for (const clause& cl : all_clauses) {
		tref o = nullptr, witness = nullptr; size_t defs = 0; clause guard;
		for (tref d : cl) {
			tref side, term;
			if (equation_of_clause(d, cl.size() > 1, side, term)) {
				++defs; o = key(side); witness = term;
			} else guard.push_back(d);
		}
		if (defs != 1) return say(defs == 0 ? "a clause defines nothing"
			: "a clause defines two outputs", cl[0]);
		for (tref g : guard) if (mentions(g, o)) return say("a guard mentions the output it defines", o);
		cells[o].push_back({ guard, witness });
		outputs_in(witness, deps[o]);
		for (tref g : guard) outputs_in(g, deps[o]);
	}
	// Every output's cells come from one conditional tree: a guard literal
	// occurs in every cell, in one sign on the cells of one branch and in
	// the other sign on the cells of the other branch; dropping it and
	// recursing on both branches ends in single cells with no guard left.
	// Such cells are total and exclusive by construction: on every branch
	// exactly one cell forces the output. A literal is a disjunct of the
	// guard in negation normal form, its complement the normal form of its
	// negation.
	struct lit { tref atom; bool neg; };
	// A guard and its complement are compared in negation normal form
	// with the operands of every conjunction and disjunction sorted, so
	// that the order in which they were assembled does not matter.
	auto canon = [&](tref f) { return ac_canonical<node>(f); };
	// Two guards are the same literal, or complements, when their
	// canonical forms are equal or, failing that, when the normalizer
	// decides their equivalence, closed universally over everything it
	// mentions: a guard and its complement may reach the recognizer in
	// forms that agree only as closed formulas, e.g. with a stream
	// replaced by one it is equated with in one of them.
	auto equivalent = [&](tref a, tref b) -> bool {
		return closed_equivalence<node>(a, b) == 1; };
	auto literal_of = [&](tref g, std::vector<tref>& atoms) -> lit {
		tref pos = canon(to_nnf<node>(g)), npos = canon(to_nnf<node>(tau::build_wff_neg(g)));
		for (size_t i = 0; i < atoms.size(); ++i) {
			if (equivalent(atoms[i], pos)) return { atoms[i], false };
			if (equivalent(atoms[i], npos)) return { atoms[i], true };
		}
		atoms.push_back(pos);
		return { pos, false };
	};
	using lcell = std::vector<lit>;
	std::function<bool(std::vector<lcell>&)> is_tree = [&](std::vector<lcell>& cs) -> bool {
		if (cs.size() == 1) return cs[0].empty();
		if (cs.empty()) return false;
		// an atom every cell mentions, exactly once
		for (const lit& cand : cs[0]) {
			std::vector<lcell> yes, no; bool ok = true;
			for (const lcell& c : cs) {
				int found = 0; bool sign = false; lcell rest;
				for (const lit& l : c)
					if (l.atom == cand.atom) { ++found; sign = l.neg; }
					else rest.push_back(l);
				if (found != 1) { ok = false; break; }
				(sign ? yes : no).push_back(std::move(rest));
			}
			if (!ok) continue;
			// the cells holding the literal negated force when the atom
			// holds; the others when it does not
			if (yes.empty() || no.empty()) continue;
			return is_tree(yes) && is_tree(no);
		}
		return false;
	};
	for (tref o : all) {
		auto it = cells.find(o);
		if (it == cells.end()) return say("no definition of an output", o);
		std::vector<tref> atoms; std::vector<lcell> cs;
		for (const cell& ce : it->second) {
			lcell c;
			for (tref g : ce.guard) c.push_back(literal_of(g, atoms));
			cs.push_back(std::move(c));
		}
		if (!is_tree(cs)) return say("the cells of an output are not one conditional tree", o);
	}
	subtree_map<node, int> mark;
	std::function<bool(tref)> acyclic = [&](tref o) -> bool {
		int& m = mark[o];
		if (m == 2) return true;
		if (m == 1) return false;
		m = 1;
		for (tref d : deps[o]) if (!acyclic(d)) return false;
		m = 2; return true;
	};
	for (tref o : all) if (!acyclic(o)) return say("cyclic definitions");
	LOG_DEBUG << "functional shape: " << all.size() << " outputs defined";
	return true;
}

template <NodeType node>
std::pair<tref, int_t> find_fixpoint_phi(tref base_fm, tref ctn_initials,
	const trefs& io_vars, const auto& initials, int_t time_point,
	bool functional = false)
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
	// SO-1: this search has no convergence guarantee; the global
	// max_fixpoint_steps (0 = unlimited, the default) lets a caller cap the
	// step count so a non-converging formula fails loudly instead of
	// hanging forever. This is a safety net, not full error propagation:
	// callers still receive a (non-fixpoint) result rather than a failure
	// signal. Real specs settle in a handful of steps (the flag_boundary
	// tests in tests/integration/test_integration-solver.cpp reach single
	// digits), so any generous bound leaves a wide margin.
	//
	// Checking the implication on the RAW iterates is deliberate. A
	// variant that normalized each iterate once (normalize_non_temp per
	// step) and ran is_nso_impl on the normal forms -- hoping the
	// positive-polarity block eliminations would be cache hits and equal
	// normal forms would shortcut the query -- measured ~10% SLOWER on
	// bv[64]x14 interpreter stress (22.0-22.9s vs 19.0-21.1s wall over
	// repeated runs, 2026-08-17): the extra per-step normalization of the
	// accumulated telescope costs more than it saves, buying only a ~21%
	// peak-RSS reduction. Do not reintroduce it for wall-clock reasons.
	auto impl = [](tref a, tref b) {
		auto ir = is_nso_impl<node>(a, b);
		return ir.has_value() && ir.value();
	};
	// A specification of functional shape (`functional`) is a fixpoint at
	// every check: the block the step adds holds for every value of what
	// it reads from outside, so the iterate equals the previous one.
	auto implied = [&](tref a, tref b) -> bool {
		const int mode = functional_continuation_mode();
		if (mode == 0) return impl(a, b);
		++functional_continuation_checks;
		if (!functional) return impl(a, b);
		++functional_continuation_shape_hits;
		if (mode == 1) return true;
		// Shadow: the implication decides, and one that does not hold
		// where the shape settles is counted.
		auto ir = is_nso_impl<node>(a, b);
		const bool iv = ir.has_value() && ir.value();
		if (!iv) {
			if (ir.has_value()) ++functional_continuation_mismatches;
			else ++functional_continuation_undecided;
		}
		return iv;
	};
	while (step_num < lookback || !implied(phi_prev, phi)){
		if (max_fixpoint_steps
			&& step_num >= (int_t)max_fixpoint_steps) {
			// A bounded give-up is not a fixpoint: the partial phi
			// would decide the query with a formula that is neither
			// the continuation nor a refutation. Surface it as
			// nullptr (no result) so the caller reports an error
			// instead of a verdict (--max-fixpoint-steps /
			// `set fixpointsteps`; 0 = unlimited).
			LOG_ERROR << "find_fixpoint_phi: exceeded " << max_fixpoint_steps
				<< " steps without reaching a fixpoint, giving up; "
				"raise --max-fixpoint-steps (`set fixpointsteps`, "
				"0 = unlimited) to decide this specification";
			return { nullptr, step_num };
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
 * @brief Compute the set of states of the combined always/flag ("chi")
 * recurrence from which a `sometimes` clause's flag can still be raised,
 * as the least fixpoint of a backward reachability iteration.
 *
 * Writing `chi@tp` and `st@tp` for @p chi_base and @p st instantiated at
 * @p time_point, and `Q` for the time-compatible quantification
 * `all i[tp+1] ex o[tp+1]`, the iterates are
 * `chi_0 = chi@tp && st@tp` and
 * `chi_{n+1} = chi@tp && (st@tp || Q chi_n')`, where `chi_n'` is `chi_n`
 * with its state (every variable at @p time_point minus the lookback or
 * later) moved one step forward. `chi_n` holds exactly in the states from
 * which the flag can be raised within `n` steps. Every iterate is
 * normalized, so its size stays bounded by the state and does not grow
 * with `n`.
 *
 * Each step adds one more chance to raise the flag, so the iterates only
 * weaken and the fixpoint is reached once `chi_{n+1} -> chi_n`. With
 * @p st `T` the flag part is absent, `chi_{n+1} = chi@tp && Q chi_n'`,
 * the iterates only strengthen and the test is `chi_n -> chi_{n+1}`.
 * @tparam node Tree node type.
 * @param chi_base Always-part local specification driving the recurrence.
 * @param st Eventual-variable flag formula (from
 * `transform_to_eventual_variables`) whose satisfiability is being
 * tracked.
 * @param io_vars IO variable nodes appearing in `chi_base` and `st`.
 * @param initials Set of `(variable name, time point)` pairs marking
 * positions predefined by explicit initial conditions.
 * @param time_point Time step at which the state is anchored; the
 * initial conditions must all lie before @p time_point minus the lookback.
 * @return A `result` carrying the pair `(chi, steps)`: `chi` is the
 * normalized fixpoint over constant-time variables anchored at
 * @p time_point, and `steps` is the number of steps taken. A failed
 * result means a normalization failed or the step cap
 * (`max_fixpoint_steps`) was hit before a fixpoint was reached.
 * @endinternal
 *
 * @par Example
 * For the always-part `o2[t] = o1[t-1] && o3[t] = o2[t-1] &&
 * o4[t] = o3[t-1]` and the flag `o4[t] = 1` anchored at 1, `chi_n` accepts
 * the states where one of `o4[1]`, `o3[1]`, ... already carries the 1
 * that reaches `o4` within `n` steps; the free `o1` makes every state
 * reach it, so the fixpoint is `chi@1` after four steps
 * (tests/integration/test_integration-satisfiability6.cpp).
 */
template <NodeType node>
result<std::pair<tref, int_t>> find_fixpoint_chi(tref chi_base, tref st,
	const trefs& io_vars, const auto& initials, int_t time_point)
{
	using tau = tree<node>;
	result<std::pair<tref, int_t>> r;
	const int_t lookback = get_max_shift<node>(io_vars);
	const int_t first_state = time_point - lookback;
	const bool weakening = !tau::get(st).equals_T();
	const tref chi_now = fm_at_time_point<node>(chi_base, io_vars,
								time_point);
	const tref st_now = fm_at_time_point<node>(st, io_vars, time_point);

	auto step = [&](tref prev) {
		tref next = shift_state_io_vars<node>(prev, first_state, 1);
		next = existentially_quantify_output_streams<node>(
			next, io_vars, time_point + 1, initials);
		next = universally_quantify_input_streams<node>(
			next, io_vars, time_point + 1, initials);
		if (weakening) next = tau::build_wff_or(st_now, next);
		return normalize_non_temp<node>(tau::build_wff_and(chi_now, next));
	};
	auto impl = [](tref a, tref b) {
		auto ir = is_nso_impl<node>(a, b);
		return ir.has_value() && ir.value();
	};

	TAU_TRY(tref chi_prev, normalize_non_temp<node>(
					tau::build_wff_and(chi_now, st_now)));
	int_t step_num = 1;
	TAU_TRY(tref chi, step(chi_prev));
	LOG_DEBUG << "Continuation at step " << step_num << ": " << LOG_FM(chi);

	// SO-1: same unbounded-search concern as find_fixpoint_phi above, and
	// the same cap (global max_fixpoint_steps, default 500, 0 = unlimited).
	while (step_num < lookback || !(weakening ? impl(chi, chi_prev)
						: impl(chi_prev, chi)))
	{
		if (max_fixpoint_steps
			&& step_num >= (int_t)max_fixpoint_steps) {
			// Same contract as find_fixpoint_phi: a give-up yields no
			// result rather than a partial chi.
			return r.with_error(code::solver_error,
				"find_fixpoint_chi exceeded the fixpoint step limit "
				"without reaching a fixpoint, giving up; raise "
				"--max-fixpoint-steps (`set fixpointsteps`, "
				"0 = unlimited) to decide this specification",
				{{label::limit, max_fixpoint_steps}});
		}
		chi_prev = chi, ++step_num;
		TAU_TRY(chi, step(chi_prev));
		LOG_DEBUG << "Continuation at step " << step_num << ": "
			<< LOG_FM(chi);
	}
	LOG_DEBUG << "Unbounded continuation of Tau formula "
		<< "reached fixpoint after " << step_num - 1 << " steps: "
		<< LOG_FM(chi_prev);
	return r.with_value(std::make_pair(chi_prev, step_num - 1));
}

/**
 * @internal
 * @brief Translate a fixpoint result from constant time points back to
 * relative (`t`-based) IO variables, keeping genuine initial conditions
 * constant.
 *
 * Inverse of the `fm_at_time_point` instantiation used by the fixpoint
 * searches: quantifier ids are re-canonized, the lookback is taken as
 * the greatest constant time point occurring in @p fm, and every IO
 * variable at a time point above @p highest_init_cond becomes `x[t]`
 * (when it sits at the lookback itself) or `x[t-k]` (`k` = lookback
 * minus its time point). Variables at or below @p highest_init_cond are
 * initial conditions and stay constant.
 * @tparam node Tree node type.
 * @param fm Formula over constant-time IO variables, as produced by
 * `find_fixpoint_phi` after normalization.
 * @param highest_init_cond Greatest time point still predefined by an
 * explicit initial condition.
 * @return @p fm with all non-initial IO variables made relative again.
 * @endinternal
 */
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

/**
 * @internal
 * @brief Build the "current" flag stream occurrence for a flag
 * recurrence rule, anchored to the surrounding formula's lookback.
 *
 * Returns the SBF-typed output stream `name[var-(lookback-1)]` when
 * `lookback >= 2`, and `name[var]` otherwise. Paired with
 * `build_prev_flag_on_lookback` this always yields two occurrences
 * exactly one step apart whose deeper member sits at
 * `max(lookback, 1)`, so a flag rule never increases the formula's
 * lookback beyond the minimum of 1 it needs.
 * @tparam node Tree node type.
 * @param var_name_node Flag stream name node (e.g. `_f0`, `_e0`).
 * @param var Name of the time variable (typically `"t"`).
 * @param lookback Lookback of the surrounding formula.
 * @return The current-step flag occurrence, e.g. `_f0[t-1]` for
 * `lookback == 2` and `_f0[t]` for `lookback <= 1`.
 * @endinternal
 */
template <NodeType node>
tref build_flag_on_lookback(tref var_name_node, const std::string& var,
							const int_t lookback)
{
	size_t flag_type = get_ba_type_id<node>(pack_bool_carrier_type<node>());
	if (lookback >= 2) return build_out_var_at_t_minus<node>(
		var_name_node, lookback - 1, flag_type, var);
	else return build_out_var_at_t<node>(var_name_node, flag_type, var);
}

/**
 * @internal
 * @brief Build the previous-step companion of `build_flag_on_lookback`:
 * the flag stream occurrence one step before the "current" one.
 *
 * Returns the SBF-typed output stream `name[var-lookback]` when
 * `lookback >= 2`, and `name[var-1]` otherwise — i.e. the occurrence at
 * the surrounding formula's lookback (at least 1).
 * @tparam node Tree node type.
 * @param io_var_node Flag stream name node (e.g. `_f0`, `_e0`).
 * @param var Name of the time variable (typically `"t"`).
 * @param lookback Lookback of the surrounding formula.
 * @return The previous-step flag occurrence, e.g. `_f0[t-2]` for
 * `lookback == 2` and `_f0[t-1]` for `lookback <= 1`.
 * @endinternal
 */
template <NodeType node>
tref build_prev_flag_on_lookback(tref io_var_node,
				const std::string& var, const int_t lookback)
{
	size_t flag_type = get_ba_type_id<node>(pack_bool_carrier_type<node>());
	if (lookback >= 2)
		return build_out_var_at_t_minus<node>(io_var_node, lookback, flag_type, var);
	else return build_out_var_at_t_minus<node>(io_var_node, 1, flag_type, var);
}

/**
 * @internal
 * @brief The flag-stream numbering counter used by `transform_ctn_to_streams`.
 *
 * Held in one place, per node type, instead of as a function-local `static`
 * inside `transform_ctn_to_streams`: callers reset it explicitly via that
 * function's `reset_ctn_id` argument, and tests can inspect or reset it
 * directly. Still process-global (and therefore still single-thread only, like
 * the rest of this subsystem), but no longer hidden.
 * @tparam node Tree node type.
 * @return Reference to the counter for @p node.
 * @endinternal
 */
template <NodeType node>
size_t& ctn_flag_counter() {
	static size_t ctn_id = 0;
	return ctn_id;
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
	const bool reset_ctn_id)
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
	// The flag counter is owned by the caller (see ctn_flag_counter) so that
	// different parts of one formula can be transformed independently without
	// the numbering being shared, unsynchronised, across every formula and
	// node type -- which is what a function-local static gave.
	size_t& ctn_id = ctn_flag_counter<node>();
	if (reset_ctn_id) ctn_id = 0;
	for (tref ctn : tau::get(fm).select_top(is<node, tau::constraint>)) {
		const auto& ct = tau::get(ctn);
		std::string ctnvar = tau::get(
			ct.find_top(is<node, tau::ctnvar>)).get_string();
		std::stringstream ss; ss << "_f" << ctn_id++;
		tref var = tau::build_var_name(ss.str());
		size_t flag_type = get_ba_type_id<node>(pack_bool_carrier_type<node>());
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
	const bool output, const bool functional = false)
{
	using tau = tree<node>;

	DBG(LOG_TRACE
		<< "always_to_unbounded_continuation begin\n"
		<< "always_to_unbounded_continuation[fm]: " << LOG_FM(fm) << "\n";)

	DBG({ auto nbc = has_no_boolean_combs_of_models<node>(fm);
		assert(nbc.has_value() && nbc.value()); })

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
					initials, lookback + point_after_inits, functional);
	// A fixpoint-step give-up surfaces as nullptr: no continuation, no
	// verdict (the caller reports an error).
	if (!ubd_ctn) return nullptr;

	{
		auto normed = normalize_non_temp<node>(ubd_ctn);
		if (!normed.has_value()) {
			// A normalization failure (a cap violation) is not a
			// refutation either; it used to return F, which every
			// caller read as unsatisfiable.
			LOG_ERROR << "always_to_unbounded_continuation: "
				"normalization of the unbound continuation failed";
			return nullptr;
		}
		ubd_ctn = normed.value();
	}
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
		auto normed_run = normalize_non_temp<node>(run);
		if (!normed_run.has_value()) {
			LOG_ERROR << "always_to_unbounded_continuation: "
				"normalization of the run failed";
			return tau::_F();
		}
		run = normed_run.value();
		// A specification of functional shape has a run at every time
		// point: its outputs are functions of the inputs and of the
		// earlier values, the initial ones included.
		std::optional<bool> satv;
		// An initial condition at the run's time point or later pins an
		// output the definitions determine as well; the run is then a
		// question of that value, not of the shape.
		const bool by_shape = functional
			&& get_max_initial<node>(io_vars) < t;
		if (by_shape) ++functional_continuation_closure_skips;
		// In the shadow mode the check runs as well and decides.
		if (by_shape && functional_continuation_mode() == 1) satv = true;
		else {
			auto sat = is_run_satisfiable<node>(run);
			if (sat.has_value()) satv = sat.value();
		}
		// An undecided run is not a refutation: nullptr is this
		// function's "no verdict".
		if (!satv.has_value()) {
			LOG_ERROR << "always_to_unbounded_continuation: the "
				"satisfiability of the run could not be decided";
			return nullptr;
		}
		if (!satv.value()) {
			print_fixpoint_info(
				"Temporal normalization of G specification reached fixpoint after "
				+ std::to_string(steps) +
				" steps, yielding the result: ",
				TAU_TO_STR(tau::_F()), output);
			return tau::_F();
		}
	}
	auto normed_result = normalize_non_temp<node>(
		conjunct_with_run ? tau::build_wff_and(ubd_ctn, run) : ubd_ctn);
	if (!normed_result.has_value()) {
		LOG_ERROR << "always_to_unbounded_continuation: "
			"final normalization failed";
		return tau::_F();
	}
	tref result = normed_result.value();
	print_fixpoint_info(
		"Temporal normalization of G specification reached fixpoint after "
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
		// Check if input stream variable. An io_var classified as neither (see
		// transform_io_var) contributes no conjunct, so the guard silently
		// comes out as `T`; make that loud instead.
		DBG(assert(tau::get(io_var).is_input_variable()
			|| tau::get(io_var).is_output_variable());)
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
	size_t flag_type = get_ba_type_id<node>(pack_bool_carrier_type<node>());
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

/**
 * @internal
 * @brief Build the initial segment of a run of the always-part @p aw:
 * its instantiations at the first @p max_st_lookback time points from
 * its own lookback onward, conjoined and normalized.
 *
 * Used by `to_unbounded_continuation` to seed its run with the early
 * time points of the original always-part: the eventual-variable flags
 * were built with the combined sometimes/always lookback
 * (`transform_to_eventual_variables`), so the flag-augmented
 * continuation's own instantiations only start that many steps later.
 * @tparam node Tree node type.
 * @param aw Original (pre-continuation) always-part.
 * @param max_st_lookback Greatest lookback among the original
 * `sometimes` clauses.
 * @return A `result<tref>` carrying the conjunction
 * `aw@l && ... && aw@(l+max_st_lookback-1)`, where `l` is @p aw's lookback;
 * an engaged value of `T` is the legitimate "no initial segment at all"
 * answer for @p max_st_lookback `0`, the identity callers conjoin in as an
 * empty conjunct rather than a failure. A failed result -- e.g. on a D4
 * bv-widening cap violation -- means normalizing the segment failed:
 * callers must propagate that failure rather than continue without the
 * initial run, since silently dropping those conjuncts only ever makes the
 * remaining formula EASIER to satisfy (an anti-conservative answer).
 * @endinternal
 */
template <NodeType node>
result<tref> make_initial_run(tref aw, const int_t max_st_lookback) {
	// get lookback of aw
	using tau = tree<node>;
	result<tref> r;
	trefs io_vars = tau::get(aw).select_top(is_child<node, tau::io_var>);
	const int_t t = get_max_shift<node>(io_vars);

	tref run = nullptr;
	for (int_t i = 0; i < max_st_lookback; ++i) {
		auto current_aw = fm_at_time_point<node>(aw, io_vars, t + i);
		if (run) {
			TAU_TRY(tref normed, normalize_non_temp<node>(
				tau::build_wff_and(run, current_aw)));
			run = normed;
		} else run = current_aw;
	}
	// A `nullptr` value reads as a failed result, not the empty run.
	return r.with_value(run ? run : tau::_T());
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
 * @return A `result<tref>` carrying `F` if the flag can never be raised
 * (the `sometimes` clause is unsatisfiable given the always-part), or a
 * formula describing a run in which the flag is raised, conjoined with
 * `original_aw`. A failed result means a normalization cap or a fixpoint-
 * or flag-search-step cap was hit before a verdict was reached.
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
result<tref> to_unbounded_continuation(tref ubd_aw_continuation,
	tref ev_var_flags, tref original_aw, const int_t start_time,
	const int_t max_st_lookback, const bool output)
{
	LOG_DEBUG << "Begin to_unbounded_continuation";

	using tau = tree<node>;
	result<tref> r;
	DBG({ auto nbc = has_no_boolean_combs_of_models<node>(ubd_aw_continuation);
		assert(nbc.has_value() && nbc.value()); })
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
	DBG(assert(!tau::get(aw).find_top(is<node, tau::constraint>));)

	int_t point_after_inits = get_max_initial<node>(io_vars) + 1;
	// Shift flags in order to match lookback of always part
	st_flags = shift_io_vars_in_fm<node>(
					st_flags, st_io_vars, time_point - 1);
	st_io_vars = tau::get(st_flags).select_top(is_child<node, tau::io_var>);

	// Create the initial phase of the always part. A failed result (as
	// opposed to an engaged `T`, which just means "no initial segment",
	// the max_st_lookback == 0 case) is a normalization failure -- a D4
	// bv-widening cap violation -- and must be propagated like every other
	// guarded site below: continuing with `run = nullptr` would silently
	// DROP the initial-run conjuncts and make the remaining search easier,
	// i.e. answer satisfiable/realizable when it must not.
	TAU_TRY(tref run, make_initial_run<node>(ori_aw_ctn, max_st_lookback));
	// Check if flag can be raised up to the highest initial condition + 2
	// which corresponds to checking the sometimes statement up to time point
	// of the highest initial condition + 1
	const int_t s = start_time + time_point;
	// TODO: flag_boundary is upper bound, improve!
	const int_t flag_boundary =
		std::max(time_point + point_after_inits, s + time_point + 1) + 1;
	for (int_t i = s; i <= flag_boundary; ++i) {
		auto current_aw = fm_at_time_point<node>(aw, io_vars, i);
		// `T` is the empty-run identity here, matching the old nullptr case.
		if (!tau::get(run).equals_T())
			run = tau::build_wff_and(run, current_aw);
		else run = current_aw;
		auto current_flag = fm_at_time_point<node>(st_flags, st_io_vars, i);
		auto normed_run = normalize_non_temp<node>(
					tau::build_wff_and(run, current_flag)).value_or(nullptr);
		// A cap violation surfaces as nullptr; propagate it rather than
		// dereferencing it below.
		if (!normed_run) return r.with_value(nullptr);
		auto sat = r.merge_take(is_run_satisfiable<node>(normed_run));
		// An undecided step is no verdict; reading it as "the flag
		// cannot be raised here" would add a false assumption below.
		if (!sat) return r;
		if (*sat) {
			LOG_DEBUG << "Flag raised at time point "<<i-time_point;
			LOG_DEBUG << LOG_FM(normed_run);
			tref res = tau::build_wff_and(normed_run, ori_aw_ctn);
			print_fixpoint_info("Temporal normalization of "
				"Tau specification did not rely on fixpoint "
				"finding, yielding the result: ",
				TAU_TO_STR(res), output);
			return r.with_value(res);
		}
		// Since the flag could not be raised in this step, we can add the assumption
		// that it will never be raised at this timepoint
		run = normalize_non_temp<node>(tau::build_wff_and(run,
					tau::build_wff_neg(current_flag))).value_or(nullptr);
		// A cap violation surfaces as nullptr; return it immediately --
		// falling into the next iteration's `equals_T()` check would
		// silently restart from `current_aw`, masking the failure.
		if (!run) return r.with_value(nullptr);
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
	auto chi_fp = r.merge_take(find_fixpoint_chi<node>(aw, st_flags, io_vars,
		initials, time_point + point_after_inits));
	// A fixpoint-step give-up surfaces as an error on `r`; propagate it.
	if (!chi_fp) return r;
	auto [chi_inf, steps] = *chi_fp;
	chi_inf = normalize_non_temp<node>(chi_inf).value_or(nullptr);
	// A cap violation surfaces as nullptr; propagate it rather than
	// dereferencing it below.
	if (!chi_inf) return r.with_value(nullptr);

	// LOG_TRACE << "Fixpoint chi after normalize: " << chi_inf;
	if (tau::get(chi_inf).equals_F()) {
		print_fixpoint_info(
			"Temporal normalization of Tau specification reached "
			"fixpoint after " + std::to_string(steps) +
			" steps, yielding the result: ",
			TAU_TO_STR(tau::_F()), output);
		return r.with_value(tau::_F());
	}
	// chi_inf is anchored at time_point + point_after_inits; its state
	// starts at point_after_inits and moves as a whole to the new anchor.
	auto chi_inf_anchored = shift_state_io_vars<node>(chi_inf,
		point_after_inits, std::max(point_after_inits, time_point)
					- (time_point + point_after_inits));

	LOG_TRACE << "Fm to check sat: "
			<< LOG_FM(tau::build_wff_and(run, chi_inf_anchored));
	auto run_sat = r.merge_take(is_run_satisfiable<node>(
		tau::build_wff_and(run, chi_inf_anchored)));
	// A real failure (as opposed to a legitimate "not satisfiable")
	// propagates as an error instead of masquerading as the F verdict.
	if (!run_sat) return r;
	if (!*run_sat)
	{
		print_fixpoint_info(
			"Temporal normalization of Tau specification reached "
			"fixpoint after " + std::to_string(steps) +
			" steps, yielding the result: ",
			TAU_TO_STR(tau::_F()), output);
		return r.with_value(tau::_F());
	}
	// Here we know that the formula is satisfiable at some point
	// Since the initial segment is already checked we continue from there
	//
	// SO-1: the "guaranteed to be sat at some point" argument below rests on
	// the is_run_satisfiable check *above*, made before this loop starts
	// conjoining !current_flag into `run` on every iteration -- so the
	// property it depends on is not preserved and the search is not
	// guaranteed to terminate. The global max_flag_search_steps (default
	// 500; 0 = unlimited) bounds it: a give-up reports unsatisfiable,
	// which is wrong but bounded and loud, where an unlimited run on such
	// a spec hangs. Deciding this properly needs a
	// tri-state (sat/unsat/unknown) result threaded through
	// transform_to_execution.
	const bool flag_search_bounded = max_flag_search_steps > 0;
	const int_t flag_search_limit = flag_boundary + 1
					+ (int_t)max_flag_search_steps;
	for (int_t i = flag_boundary + 1; true; ++i) {
		if (flag_search_bounded && i > flag_search_limit) {
			// A bounded give-up is no verdict: it used to report F,
			// which callers read as a proof of unsatisfiability. Surface
			// it as an error instead.
			print_fixpoint_info("Temporal normalization of Tau "
				"specification gave up after " +
				std::to_string(steps) + " fixpoint steps and " +
				std::to_string(max_flag_search_steps) +
				" flag search steps without a result",
				"", output);
			return r.with_error(code::solver_error,
				"to_unbounded_continuation could not raise the "
				"eventual variable flag past the flag boundary within "
				"the flag search step limit; giving up without a "
				"verdict. This is a bounded failure, not a proof of "
				"unsatisfiability; raise --max-flag-search-steps "
				"(`set flagsteps`, 0 = unlimited) to decide this "
				"specification.",
				{{label::limit, max_flag_search_steps}});
		}
		auto current_aw = fm_at_time_point<node>(aw, io_vars, i);
		run = tau::build_wff_and(run, current_aw);
		auto current_flag
			= fm_at_time_point<node>(st_flags, st_io_vars, i);

		auto normed_run = normalize_non_temp<node>(
					tau::build_wff_and(run, current_flag)).value_or(nullptr);
		// A cap violation surfaces as nullptr; propagate it rather than
		// dereferencing it below.
		if (!normed_run) return r.with_value(nullptr);
		// The formula is guaranteed to have be sat at some point
		// Therefore, the loop will exit eventually
		auto sat = r.merge_take(is_run_satisfiable<node>(normed_run));
		// An undecided step is no verdict; reading it as "the flag
		// cannot be raised here" would add a false assumption below.
		if (!sat) return r;
		if (*sat) {
			LOG_DEBUG << "Flag raised at time point "<<i-time_point;
			LOG_DEBUG << LOG_FM(normed_run);
			tref res = tau::build_wff_and(normed_run, ori_aw_ctn);
			print_fixpoint_info(
				"Temporal normalization of Tau specification "
				"reached fixpoint after "+std::to_string(steps)+
				" steps, yielding the result: ",
				TAU_TO_STR(res), output);
			return r.with_value(res);
		}
		// Since the flag could not be raised in this step, we can add the assumption
		// that it will never be raised at this timepoint
		run = normalize_non_temp<node>(tau::build_wff_and(run,
					tau::build_wff_neg(current_flag))).value_or(nullptr);
		// A cap violation surfaces as nullptr; return it immediately --
		// this unbounded loop rebuilds `run` unconditionally next
		// iteration, unlike the bounded loop above.
		if (!run) return r.with_value(nullptr);
	}
}

template <NodeType node>
result<tref> transform_to_execution(tref fm, const int_t start_time,
	const bool output, const bool functional)
{
	result<tref> r;
	using tau = tree<node>;
	if (!fm) {
		return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
	}
	DBG(assert(get_dnf_wff_clauses<node>(fm).size() == 1);)
	// Make sure that no function/predicate symbol is still present
	if (auto ref = tau::get(fm).find_top(is<node, tau::ref>); ref) {
		LOG_ERROR << "transform_to_execution: unresolved function or "
			"predicate symbol " << LOG_FM(ref) << " found; "
			"treating the formula as unsatisfiable";
		return r.with_assert_check_value(_F<node>());
	}
#ifdef TAU_CACHE
	using cache_t = std::map<std::pair<tref, int_t>, tref,
				subtree_pair_less<node, int_t>>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	// The continuation depends on the runtime budgets (fixpoint and flag
	// search steps, the synthesis knobs, the algebras' options); a change
	// between two queries must not return the first one's result.
	static size_t cache_budget = verdict_budget_fingerprint<node>();
	if (const size_t fp = verdict_budget_fingerprint<node>(); fp != cache_budget)
	{
		cache.clear();
		cache_budget = fp;
	}
	if (auto it = cache.find(std::make_pair(fm, start_time));
		it != cache.end())
	{
		return r.with_assert_check_value(it->second);
	}
#endif // TAU_CACHE
	auto elim_aw = [](tref f) {
		return tau::get(f)
			.child_is(tau::wff_always) ? tau::trim2(f) : f;
	};
	LOG_DEBUG << "Start transform_to_execution: " << LOG_FM(fm);

	tref aw_fm = tau::get(fm).find_top(is_child<node, tau::wff_always>);
	std::pair<tref, int_t> ev_t;
	tref ubd_aw_fm = nullptr;
	{
		auto _s = r.open("always_continuation");
		if (aw_fm != nullptr) {
			// If there is an always part, replace it with its unbound continuation
			ubd_aw_fm = always_to_unbounded_continuation<node>(
							aw_fm, start_time, output, functional);
			// nullptr = a bounded give-up (max_fixpoint_steps) or a
			// normalization cap: no verdict, so report an error
			// instead of deciding on a missing continuation.
			if (!ubd_aw_fm) {
				return r.with_assert_check_error(code::solver_error,
					"the temporal normalization gave up before "
					"reaching a result (see --max-fixpoint-steps, "
					"0 = unlimited); the specification could not "
					"be decided");
			}
			auto ubd_fm = rewriter::replace<node>(fm, aw_fm,
						tau::build_wff_always(ubd_aw_fm));
			ev_t = transform_to_eventual_variables<node>(
							ubd_fm, false, start_time);
			// Check if there is a sometimes present
			if (ev_t.first == ubd_fm) {
				tref res = elim_aw(ubd_fm);
#ifdef TAU_CACHE
				cache.emplace(std::make_pair(
					elim_aw(ubd_fm), start_time), res);
				r = cache.emplace(std::make_pair(fm, start_time),
						     res).first->second;
#else
				r = res;
#endif // TAU_CACHE
				DBG(assert(r.is_well_formed());)
				return r;
			}
		} else {
			ev_t = transform_to_eventual_variables<node>(
								fm, true, start_time);
			// Check if there is a sometimes present
			if (ev_t.first == fm) {
				// Here we deal with a non-temporal formula
				// Use aw_fm to store result
				aw_fm = elim_aw(fm);
				TAU_TRY_OR(bool sat,
					is_non_temp_nso_satisfiable<node>(fm),
					code::internal_error,
					"Failed to determine non-temporal satisfiability");
				if (!sat) aw_fm = tau::_F();
#ifdef TAU_CACHE
				r = cache.emplace(std::make_pair(fm, start_time),
						     aw_fm).first->second;
#else
				r = aw_fm;
#endif // TAU_CACHE
				DBG(assert(r.is_well_formed());)
				return r;
			}
		}
	}
	auto aw_after_ev = tau::get(ev_t.first)
				.find_top(is_child<node, tau::wff_always>);
	// Both producers of ev_t above wrap their result in build_wff_always
	// whenever they transformed anything (and the untransformed cases already
	// returned), so this should not happen. If it ever does, returning the
	// untransformed `fm` discards the eventual-variable transformation just
	// computed -- assert rather than let that pass silently.
	DBG(assert(aw_after_ev != nullptr);)
	if (aw_after_ev == nullptr) {
		tref res = elim_aw(fm);
#ifdef TAU_CACHE
		r = cache.emplace(std::make_pair(fm, start_time),
				     res).first->second;
#else
		r = res;
#endif // TAU_CACHE
		DBG(assert(r.is_well_formed());)
		return r;
	}
	trefs st = tau::get(ev_t.first)
				.select_top(is_child<node, tau::wff_sometimes>);
	// transform_to_eventual_variables folds every sometimes clause into
	// one flag-carrying clause, so at most one may survive here; a second
	// one means a nested `sometimes` leaked through the fragment
	// reduction. Refuse loudly rather than silently dropping the rest:
	// the error propagates to api::realizable/valid_spec/get_interpreter
	// as a solver_error verdict.
	if (st.size() >= 2) {
		LOG_ERROR << "transform_to_execution: " << st.size()
			<< " sometimes clauses survived the eventual-variable "
			"transform; the formula cannot be decided by the "
			"safety pipeline";
		return r.with_assert_check_error(code::solver_error, "nested or multiple `sometimes` "
			"clauses survived the eventual-variable transform");
	}

	tref res;
	{
		auto _s = r.open("unbounded_continuation");
		if (!tau::get(aw_after_ev).equals_F() && !st.empty()) {
			// A bounded give-up (max_fixpoint_steps /
			// max_flag_search_steps) or a normalization cap surfaces as
			// an error here; `result<tref>` rejects `nullptr` as a value.
			TAU_TRY(tref ctn, to_unbounded_continuation<node>(
					aw_after_ev, st[0], ubd_aw_fm, start_time,
					ev_t.second, output));
			TAU_TRY_OR(res, normalize_non_temp<node>(ctn),
				code::internal_error,
				"Normalization of the unbounded continuation failed");
		} else res = aw_after_ev;
	}
	res = elim_aw(res);
	LOG_DEBUG << "End transform_to_execution: " << LOG_FM(res);
#ifdef TAU_CACHE
	cache.emplace(std::make_pair(res, start_time), res);
	r = cache.emplace(std::make_pair(fm, start_time),
			     res).first->second;
#else
	r = res;
#endif // TAU_CACHE
	DBG(assert(r.is_well_formed());)
	return r;
}

// This is the cross-revision satisfiability result cache. Any U/R/W/S/T
// content routes a query through the full LTL(ABA) pipeline -- one ltlsynt
// subprocess per call -- and the pointwise revision asks the same (formula,
// start_time) query again on every later update. Memoise the verdict under
// TAU_CACHE, keyed like `transform_to_execution`'s cache and invalidated by
// the tree GC like every other create_cache table. The `output` flag only
// adds logging/exports on top of the same verdict, so it is not part of the
// key -- but an output=true call still runs the full computation for its
// side effects (and stores the verdict for others). Never share a cvc5
// solver or ltlsynt session across calls -- the result cache is the only
// safe port.
// fm with every input stream read as an output: satisfiable exactly when
// some input sequence lets fm hold, i.e. when some trace satisfies it.
// The stream is renamed as well as re-tagged: resolve_io_vars stamps every
// io_var from the io context and then the name prefix, so a stream still
// called i1 would be read as an input again on any path that parses or
// normalizes the formula. The new name is an output by prefix and is
// registered nowhere.
template <NodeType node>
tref inputs_as_outputs(tref fm) {
	using tau = tree<node>;
	subtree_map<node, tref> flip;
	for (tref v : tau::get(fm).select_all([](tref n) {
		const auto& t = tau::get(n);
		return t.is(tau::io_var) && t.is_input_variable(); }))
	{
		const auto& t = tau::get(v);
		trefs ch;
		ch.push_back(build_var_name<node>(
			"o_in_" + get_var_name<node>(v)));
		for (size_t i = 1; i < t.children_size(); ++i)
			ch.push_back(t.child(i));
		flip.emplace(v, tau::get(node::output_variable(), ch));
	}
	return flip.empty() ? fm : rewriter::replace<node>(fm, flip);
}

template <NodeType node>
result<bool> is_tau_formula_sat(tref fm, const int_t start_time,
	const bool output)
{
	result<bool> r;
	using tau = tree<node>;
	if (!fm) {
		return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
	}
	auto mark_undecided = [&]() {
		r.error(code::unsupported_operation,
			"UNKNOWN: the CTL* encoding is unrealizable, but its E "
			"witnesses range over every input branch, which is "
			"stricter than E; satisfiability could not be decided");
	};
#ifdef TAU_CACHE
	using cache_t = std::map<std::pair<tref, int_t>, bool,
				subtree_pair_less<node, int_t>>;
	static cache_t& cache = tree<node>::template create_cache<cache_t>();
	// Full-LTL formulas the realizability shortcut leaves undecided (see
	// below) are undecided on every call, not just the first -- cache
	// that verdict too, or a repeated query re-runs ltlsynt for nothing.
	static cache_t& undecided = tree<node>::template create_cache<cache_t>();
	// Both memos are keyed on (formula, start_time) only, while every
	// runtime budget (max_fixpoint_steps, max_flag_search_steps, the
	// LTL(ABA) caps and knobs, the algebras' options) can change the
	// verdict: `sat φ`, `set fixpointsteps 0`, `sat φ` must not return the
	// first query's answer. Drop the entries whenever the budgets moved.
	static size_t cache_budget = verdict_budget_fingerprint<node>();
	if (const size_t fp = verdict_budget_fingerprint<node>(); fp != cache_budget)
	{
		cache.clear();
		undecided.clear();
		cache_budget = fp;
	}
	if (!output) {
		if (auto it = cache.find(std::make_pair(fm, start_time));
			it != cache.end())
		{
			return r.with_assert_check_value(it->second);
		}
		if (undecided.contains(std::make_pair(fm, start_time))) {
			mark_undecided();
			DBG(assert(r.is_well_formed());)
			return r;
		}
	}
#endif // TAU_CACHE
	auto memoize = [&](bool value) {
#ifdef TAU_CACHE
		cache.emplace(std::make_pair(fm, start_time), value);
#endif // TAU_CACHE
		r = value;
	};

	LOG_DEBUG << "Start is_tau_formula_sat: " << LOG_FM(fm);
	// Merge top-level (G A) && (G B) → G(A && B) up-front.  Direct
	// callers (the PWR-output → is_realizable path in test_pwr_*,
	// for example) reach is_tau_formula_sat without going through
	// api::sat / api::realizable, so the flatten that lives there
	// would be skipped.  Doing it here too is idempotent and ensures
	// every entry point sees a normalised conjunction-of-G shape.
	fm = flatten_always_conjuncts<node>(fm);
	// CTL* formulas: reduce to LTL first, then check realizability
	if (has_ctl_star_operators<node>(fm)) {
		auto _s = r.open("ctl_star_reduction");
		// same reason as in is_ctl_star_realizable: the witness
		// constraints are decided over the formula's own atoms
		if (auto nf = normalize<node>(fm); nf.has_value() && nf.value())
			fm = nf.value();
		auto reduction = reduce_ctl_star_to_ltl<node>(fm);
		// a backend that gave no verdict leaves satisfiability unknown,
		// which is not the "not implemented" case mark_undecided states
		if (!reduction.has_value()) {
			r.merge(std::move(reduction));
			return r.with_assert_check_error(code::solver_error,
				messages::unknown_satisfiability_no_verdict);
		}
		// The reduction is plain LTL, so its satisfiability follows the
		// same rules as any other LTL formula: sat(A χ) = sat(χ). An E
		// witness encoded without directions strengthens the formula,
		// so then only a true verdict carries over to fm and a false
		// one leaves it undecided.
		auto reduced = is_tau_formula_sat<node>(
			reduction->ltl_formula, start_time, output);
		if (!reduced.has_value()) r.merge(std::move(reduced));
		else if (reduced.value() || reduction->exact)
			memoize(reduced.value());
		else {
#ifdef TAU_CACHE
			undecided.emplace(std::make_pair(fm, start_time), true);
#endif // TAU_CACHE
			mark_undecided();
		}
		DBG(assert(r.is_well_formed());)
		return r;
	}
	// Route to the full-LTL pipeline whenever the formula has full-LTL
	// operators OR has Boolean combinations of models that the safety
	// pipeline can't handle (e.g. (G A) || (G B), !(G A && G B), nested
	// G inside disjunction).  The flatten pass above has already merged
	// trivial top-level conjunctions of G into a single G; whatever
	// survives genuinely needs ltlsynt.
	//
	// Exception: formulas whose only temporal operators are wff_always
	// and wff_sometimes are handled correctly by the safety pipeline
	// via transform_to_eventual_variables + to_unbounded_continuation.
	// These should NOT be routed to ltlsynt, which cannot faithfully
	// encode lookback-atom initial-value semantics as propositional
	// constraints.
	if (sat_has_ltl_operators<node>(fm)) {
		auto _s = r.open("ltl_realizability");
		// A specification is satisfiable when it can be executed
		// indefinitely whatever the inputs (README "Satisfiability"):
		// for full LTL that is realizability, decided both ways.
		auto realizable = is_ltl_aba_realizable<node>(fm, start_time, output);
		if (!realizable.has_value()) {
			// no verdict at all
			r.merge(std::move(realizable));
			return r.with_assert_check_error(code::solver_error,
				"UNKNOWN: the synthesis backend failed or produced no "
				"verdict; satisfiability could not be decided");
		}
		memoize(realizable.value());
		DBG(assert(r.is_well_formed());)
		return r;
	}
	// A "boolean combination of models" that reaches this point can only be
	// G(...) && sometimes(...) shapes, handled by the safety pipeline's
	// flag-based unrolling: any F/U/R/W/S/T content already returned via
	// is_ltl_aba_realizable above. Fall through to the safety pipeline.
	tref normalized_fm;
	{
		auto _s = r.open("normalize");
		TAU_TRY_OR(normalized_fm, normalize_with_temp_simp<node>(fm),
			code::internal_error, "Normalization failed");
	}
	{
		auto _s = r.open("expression_paths");
		// Convert each disjunct to unbounded continuation. An undecided
		// disjunct does not stop the scan -- a later satisfiable one
		// still decides the disjunction -- but it keeps "no disjunct
		// satisfiable" from reading as F.
		std::optional<result<tref>> undecided_path;
		for (tref clause : expression_paths<node>(normalized_fm)) {
			auto val = transform_to_execution<node>(
				clause, start_time, output);
			if (!val.has_value()) {
				if (!undecided_path) undecided_path.emplace(
					std::move(val));
				continue;
			}
			if (!tau::get(val.value()).equals_F()) {
				LOG_DEBUG << "End is_tau_formula_sat: true";
				memoize(true);
				DBG(assert(r.is_well_formed());)
				return r;
			}
		}
		if (undecided_path) {
			TAU_TRY_OR(tref unused, std::move(*undecided_path),
				code::internal_error,
				"transform_to_execution returned neither a "
				"value nor an error while checking a "
				"disjunct's satisfiability");
			(void)unused;
		}
	}
	LOG_DEBUG << "End is_tau_formula_sat: false";
	memoize(false);
	DBG(assert(r.is_well_formed());)
	return r;
}

// Check for temporal formulas if f1 implies f2
template <NodeType node>
result<bool> is_tau_impl(tref f1, tref f2) {
	result<bool> r;
	using tau = tree<node>;
	if (!f1 || !f2) {
		return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
	}
	if (has_ctl_star_operators<node>(f1) || has_ctl_star_operators<node>(f2))
		return r.with_error(code::unsupported_operation,
			"implication between formulas with CTL* operators cannot "
			"be decided");
	TAU_TRY(tref f1n, normalize<node>(f1));
	TAU_TRY(tref f2n, normalize<node>(f2));
	TAU_TRY(tref imp_check, normalize_with_temp_simp<node>(
		tau::build_wff_neg(tau::build_wff_imply(f1n, f2n))));
	// transform_to_execution decides the safety fragment only; anything
	// sat routes to the LTL pipeline would be misread as unsatisfiable
	if (sat_has_ltl_operators<node>(imp_check))
		return r.with_error(code::unsupported_operation,
			"implication between full-LTL formulas cannot be decided "
			"by the safety pipeline");
	// Now check that each disjunct is not satisfiable
	auto _s = r.open("expression_paths");
	// One satisfiable disjunct refutes the implication even when another
	// is undecided; an undecided one only matters when no disjunct is
	// satisfiable, and then it is the result.
	std::optional<result<tref>> undecided_path;
	for (tref c : expression_paths<node>(imp_check)) {
		auto val = transform_to_execution<node>(c);
		if (!val.has_value()) {
			if (!undecided_path) undecided_path.emplace(std::move(val));
			continue;
		}
		if (!tau::get(val.value()).equals_F()) {
			return r.with_assert_check_value(false);
		}
	}
	if (undecided_path) {
		TAU_TRY(tref unused, std::move(*undecided_path));
		(void)unused;
	}
	return r.with_assert_check_value(true);
}

// The formulas need to be closed
template <NodeType node>
result<bool> are_tau_equivalent(tref f1, tref f2) {
	result<bool> r;
	using tau = tree<node>;
	if (!f1 || !f2) {
		return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
	}
	// Negate equivalence for unsat check
	TAU_TRY_OR(tref f1n, normalize<node>(f1), code::internal_error,
		"Normalization of the first formula failed");
	TAU_TRY_OR(tref f2n, normalize<node>(f2), code::internal_error,
		"Normalization of the second formula failed");
	TAU_TRY_OR(tref equiv_check, normalize_with_temp_simp<node>(
			tau::build_wff_neg(tau::build_wff_equiv(f1n, f2n))),
		code::internal_error, "Normalization of the equivalence check failed");
	if (has_ctl_star_operators<node>(equiv_check)
		|| sat_has_ltl_operators<node>(equiv_check))
		return r.with_error(code::unsupported_operation,
			"equivalence of full-LTL or CTL* formulas cannot be decided "
			"by the safety pipeline");
	// Now check that each disjunct is not satisfiable
	auto _s = r.open("expression_paths");
	// Same undecided-disjunct rule as is_tau_impl.
	std::optional<result<tref>> undecided_path;
	for (const auto& c : expression_paths<node>(equiv_check)) {
		auto val = transform_to_execution<node>(c);
		if (!val.has_value()) {
			if (!undecided_path) undecided_path.emplace(std::move(val));
			continue;
		}
		if (!tau::get(val.value()).equals_F()) {
			return r.with_assert_check_value(false);
		}
	}
	if (undecided_path) {
		TAU_TRY_OR(tref unused, std::move(*undecided_path),
			code::internal_error,
			"transform_to_execution returned neither a value nor "
			"an error while checking equivalence");
		(void)unused;
	}
	return r.with_assert_check_value(true);
}

template <NodeType node>
result<tref> simp_tau_unsat_valid(tref fm, const int_t start_time,
	const bool output, tref normalized)
{
	result<tref> r;
	using tau = tree<node>;
	if (!fm) {
		return r.with_assert_check_error(code::invalid_argument, messages::invalid_arguments);
	}
	LOG_DEBUG << "Start simp_tau_unsat_valid: " << LOG_FM(fm);
	// Check if formula is valid. Validity distributes over conjunction, so
	// where the formula is a conjunction of independent units the unit-wise
	// verdict is exact and cheap; the monolithic check stays for the rest.
	// (Measured on an accumulating run with a 137-clause `:tau` constant: the per-path
	// transform below cost ~62 s per rejection while returning the formula
	// unchanged; unit-wise it is milliseconds, with the same paths kept.)
	// The unit-wise verdicts are taken at start time 0 (the only start time
	// the caller uses); any other start time keeps the monolithic checks.
	const bool factor = pack_ba_component_factoring_enabled<node>()
		&& start_time == 0;
	int fv = factor ? factored_tau_valid<node>(fm) : -1;
	if (fv == 1) {
		return r.with_assert_check_value(tau::_T());
	}
	if (fv < 0) {
		// an undecided validity only means no simplification here
		if (auto v = is_tau_impl<node>(tau::_T(), fm);
			v.has_value() && v.value())
		{
			return r.with_assert_check_value(tau::_T());
		}
	}
	tref normalized_fm = normalized;
	if (!normalized_fm) {
		TAU_TRY_OR(normalized_fm, normalize_with_temp_simp<node>(fm),
			code::internal_error, "Normalization failed");
	}
	trefs clauses = {tau::_F()};
	// Check satisfiability of each clause -- unit-wise where exact
	{
		auto _s = r.open("expression_paths");
		for (tref clause: expression_paths<node>(normalized_fm)) {
			bool keep;
			int fs = factor ? factored_tau_sat<node>(clause) : -1;
			if (fs >= 0) keep = (fs == 1);
			else {
				TAU_TRY_OR(tref val, transform_to_execution<node>(
					clause, start_time, output),
					code::internal_error,
					"transform_to_execution returned "
					"neither a value nor an error while "
					"simplifying a disjunct");
				keep = !tau::get(val).equals_F();
			}
			if (keep) clauses.push_back(clause);
		}
	}
	r = tau::build_wff_or(clauses);
	LOG_DEBUG << "End simp_tau_unsat_valid: " << LOG_FM(r.value());
	DBG(assert(r.is_well_formed());)
	return r;
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
