// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#ifndef __SATISFIABILITY_H__
#define __SATISFIABILITY_H__

#include "normalizer.h"

using namespace std;
using namespace idni::tau;

namespace idni::tau {

// ------ Helpers for variables having io_var as child ---------------
template<typename... BAs>
auto is_io_initial (const nso<BAs...>& io_var) {
	return (trim2(io_var)->child[1] | tau_parser::num).has_value();
}

template<typename... BAs>
auto is_io_shift (const nso<BAs...>& io_var) {
	return (trim2(io_var)->child[1] | tau_parser::shift).has_value();
}

template<typename... BAs>
auto get_io_time_point (const nso<BAs...>& io_var) {
	return size_t_extractor<BAs...>(trim2(trim2(io_var)->child[1])).value();
}

template<typename... BAs>
auto get_io_shift (const nso<BAs...>& io_var) {
	return size_t_extractor<BAs...>(trim2(io_var)->child[1]->child[0]->child[1]->child[0]).value();
}

template<typename... BAs>
string get_io_name (const nso<BAs...>& io_var) {
	stringstream ss; ss << trim(trim2(io_var));
	return ss.str();
}

template<typename... BAs>
nso<BAs...> build_io_out (const string& name, const string& var) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::out_var_name, name);
	auto offset = wrap<BAs...>(p::offset, wrap<BAs...>(p::variable, var));
	return wrap(p::variable, wrap(p::io_var, wrap(p::out, { var_name, offset })));
}

template<typename... BAs>
nso<BAs...> build_io_in (const string& name, const string& var) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::out_var_name, name);
	auto offset = wrap<BAs...>(p::offset, wrap<BAs...>(p::variable, var));
	return wrap(p::variable, wrap(p::io_var, wrap(p::in, { var_name, offset })));
}

template<typename... BAs>
nso<BAs...> build_io_out_const (const string& name, const int_t pos) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::out_var_name, name);
	auto offset = wrap<BAs...>(p::offset, build_num<BAs...>(pos));
	return wrap(p::variable, wrap(p::io_var, wrap(p::out, { var_name, offset })));
}

template<typename... BAs>
nso<BAs...> build_io_in_const (const string& name, const int_t pos) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::out_var_name, name);
	auto offset = wrap<BAs...>(p::offset, build_num<BAs...>(pos));
	return wrap(p::variable, wrap(p::io_var, wrap(p::in, { var_name, offset })));
}

template<typename... BAs>
nso<BAs...> build_io_out_shift (const string& name, const string& var, const int_t shift) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::out_var_name, name);
	auto shift_node = wrap<BAs...>(p::shift, {wrap<BAs...>(p::variable, var), build_num<BAs...>(shift)});
	auto offset = wrap<BAs...>(p::offset, shift_node);
	return wrap(p::variable, wrap(p::io_var, wrap(p::out, { var_name, offset })));
}
// -----------------------------------------------
template<typename... BAs>
int_t get_io_var_shift(const nso<BAs...>& io_var) {
	// If there is a shift
	if (is_io_shift(io_var))
		return get_io_shift(io_var);
	return 0;
}

int_t get_max_shift(const auto& io_vars) {
	int_t max_shift = 0;
	for (const auto& v : io_vars)
		max_shift = max(max_shift, get_io_var_shift(v));
	return max_shift;
}

template<typename... BAs>
int_t get_max_initial(const auto& io_vars) {
	int_t max_init = -1;
	for (const nso<BAs...>& v : io_vars) {
		if (is_io_initial(v)) {
			int_t init = get_io_time_point(v);
			max_init = max(max_init, init);
		}
	}
	return max_init;
}

template<typename... BAs>
int_t get_lookback_after_normalization(const auto& io_vars) {
	int_t max_lookback = 0;
	for (const auto& v : io_vars) {
		if (is_io_initial(v)) {
			int_t lookback = get_io_time_point(v);
			max_lookback = max(max_lookback, lookback);
		}
	}
	return max_lookback;
}

template<typename... BAs>
nso<BAs...> transform_io_var(const nso<BAs...>& io_var, const string& io_var_name, int_t time_point) {
	// Check if io_var has constant time point
	if (is_io_initial(io_var))
		return io_var;
	auto shift = get_io_var_shift(io_var);
	if (io_var_name[0] == 'i')
		return build_io_in_const<BAs...>(io_var_name, time_point - shift);
	else return build_io_out_const<BAs...>(io_var_name, time_point - shift);
}

template<typename... BAs>
nso<BAs...> existentially_quantify_output_streams(nso<BAs...> fm, const auto& io_vars,
                                                  int_t time_point, const auto& initials) {
	// This map is needed in order to get the minimal shift for streams with same name
	set<int_t> quantifiable_o_vars;
	for (int_t i = 0; i < (int_t)io_vars.size(); ++i) {
		// Skip input streams
		if (io_vars[i] | tau_parser::io_var | tau_parser::in)
			continue;
		// Skip initial conditions
		if (is_io_initial(io_vars[i]))
			continue;
		quantifiable_o_vars.insert(i);
	}
	set<nso<BAs...>> cache;
	for (const auto& pos : quantifiable_o_vars) {
		// Do not quantify time steps which are predefined by initial conditions
		if (initials.contains({get_io_name(io_vars[pos]), time_point}))
			continue;
		nso<BAs...> var = build_io_out_const<BAs...>(get_io_name(io_vars[pos]), time_point);
		auto res = cache.emplace(var);
		if (res.second) fm = build_wff_ex(var, fm);
	}
	return fm;
}

template<typename... BAs>
nso<BAs...> universally_quantify_input_streams(nso<BAs...> fm, const auto& io_vars,
                                               int_t time_point, const auto& initials) {
	// This map is needed in order to get the minimal shift for streams with same name
	set<int_t> quantifiable_i_vars;
	for (int_t i = 0; i < (int_t)io_vars.size(); ++i) {
		// Skip output streams
		if (io_vars[i] | tau_parser::io_var | tau_parser::out)
			continue;
		// Skip initial conditions
		if (is_io_initial(io_vars[i]))
			continue;
		quantifiable_i_vars.insert(i);
	}
	set<nso<BAs...>> cache;
	for (const auto& pos : quantifiable_i_vars) {
		// Do not quantify time steps which are predefined by initial conditions
		if (initials.contains({get_io_name(io_vars[pos]), time_point}))
			continue;
		nso<BAs...> var = build_io_in_const<BAs...>(get_io_name(io_vars[pos]), time_point);
		auto res = cache.emplace(var);
		if (res.second) fm = build_wff_all(var, fm);
	}
	return fm;
}

template<typename... BAs>
nso<BAs...> calculate_flag(const nso<BAs...>& flag, int_t time_point) {
	auto to_ba = [](const bool c){return c ? _1<BAs...> : _0<BAs...>;};
	int_t condition;
	bool is_left;
	if (is_non_terminal(tau_parser::num, trim2(flag))) {
		is_left = true;
		condition = size_t_extractor<BAs...>(trim(trim2(flag))).value();
	}
	else {
		is_left = false;
		condition = size_t_extractor<BAs...>(trim(flag)->child[1]->child[0]).value();
	}
	if (flag | tau_parser::flag_neq)
		return to_ba(condition != time_point);
	if (flag | tau_parser::flag_eq)
		return to_ba(condition == time_point);
	if (flag | tau_parser::flag_greater_equal)
		return is_left ? to_ba(condition >= time_point) : to_ba(time_point >= condition);
	if (flag | tau_parser::flag_greater)
		return is_left ? to_ba(condition > time_point) : to_ba(time_point > condition);
	if (flag | tau_parser::flag_less_equal)
		return is_left ? to_ba(condition <= time_point) : to_ba(time_point <= condition);
	if (flag | tau_parser::flag_less)
		return is_left ? to_ba(condition < time_point) : to_ba(time_point < condition);
	// The above is exhaustive for possible children of flag
	assert(false);
	return {};
}

template<typename... BAs>
bool is_initial_flag_phase(const nso<BAs...>& flag, int_t time_point) {
	const int_t condition = is_non_terminal(tau_parser::num, trim2(flag)) ?
		size_t_extractor<BAs...>(trim(trim2(flag))).value() :
		size_t_extractor<BAs...>(trim(flag)->child[1]->child[0]).value();

	// At this point the equality and inequality flags should have been converted
	assert(!(flag | tau_parser::flag_neq) && !(flag | tau_parser::flag_eq));

	if (flag | tau_parser::flag_greater_equal)
		return condition >= time_point;
	if (flag | tau_parser::flag_greater)
		return condition + 1 >= time_point;
	if (flag | tau_parser::flag_less_equal)
		return condition + 1 >= time_point;
	if (flag | tau_parser::flag_less)
		return condition >= time_point;

	// The above is exhaustive for possible children of flag
	assert(false);
	return {};
}

template<typename... BAs>
nso<BAs...> build_initial_step(const nso<BAs...>& original_fm,
	auto &io_vars, int_t time_point)
{
	map<nso<BAs...>, nso<BAs...>> changes;
	for (size_t i = 0; i < io_vars.size(); ++i) {
		auto new_io_var = transform_io_var(io_vars[i], get_io_name(io_vars[i]),
								time_point);
		changes[io_vars[i]] = new_io_var;
	}
	nso<BAs...> new_fm = replace(original_fm, changes);
	return new_fm;
}

template<typename... BAs>
nso<BAs...> build_step(const nso<BAs...>& original_fm,
	const nso<BAs...>& prev_fm, const auto& io_vars, auto& initials, int_t step_num,
	int_t time_point, nso<BAs...>& cached_fm)
{
	// Use build_initial_step otherwise
	assert(step_num > 0);
	map<nso<BAs...>, nso<BAs...> > changes;
	for (size_t i = 0; i < io_vars.size(); ++i) {
		auto new_io_var = transform_io_var(io_vars[i], get_io_name(io_vars[i]),
							time_point + step_num);
		changes[io_vars[i]] = new_io_var;
	}

	nso<BAs...> most_inner_step = replace(original_fm, changes);
	auto q_most_inner_step = existentially_quantify_output_streams(most_inner_step, io_vars, time_point + step_num,
	                                                               initials);
	q_most_inner_step = universally_quantify_input_streams(q_most_inner_step, io_vars, time_point + step_num,
	                                                       initials);
	changes = {{cached_fm, build_wff_and(cached_fm, q_most_inner_step)}};
	cached_fm = most_inner_step;
	return replace(prev_fm, changes);
}

template<typename... BAs>
nso<BAs...> transform_eventual_variables(const nso<BAs...>& fm_orig) {
	using p = tau_parser;
	auto sometimes = select_all(fm_orig,
				is_non_terminal<p::wff_sometimes, BAs...>);
	// if not more than 1 `sometimes` return original formula
	if (sometimes.size() < 2) return fm_orig;
	auto fm = fm_orig;
	BOOST_LOG_TRIVIAL(trace) << "(T) -- transforming eventual variables";
	BOOST_LOG_TRIVIAL(trace) << fm;
	// for each `sometimes psi` replace it with (N is the nth number of `sometimes`):
	map<nso<BAs...>,nso<BAs...>> changes;
	nso<BAs...> always_conjs = nullptr, sometimes_conjs = nullptr;
	auto capture = wrap<BAs...>(p::capture, "t");
	auto offset = wrap<BAs...>(p::offset, capture);
	auto offset_prev = wrap<BAs...>(p::offset,
		wrap<BAs...>(p::shift, { capture, build_num<BAs...>(1) }));
	auto offset0 = wrap<BAs...>(p::offset, build_num<BAs...>(0));
	auto smt = sometimes[0];
	for (size_t n = 0; ; ++n) {
		std::stringstream ss; ss << "_e" << n;
		auto iovar = wrap<BAs...>(p::out_var_name, ss.str());
		auto eNt = wrap<BAs...>(p::bf,
			wrap<BAs...>(p::variable,
			wrap<BAs...>(p::io_var,
			wrap<BAs...>(p::out, { iovar, offset }))));
		auto eNt_prev = wrap<BAs...>(p::bf,
			wrap<BAs...>(p::variable,
			wrap<BAs...>(p::io_var,
			wrap<BAs...>(p::out, { iovar, offset_prev }))));
		auto eN0_is_zero = build_wff_eq(wrap<BAs...>(p::bf,
			wrap<BAs...>(p::variable,
			wrap<BAs...>(p::io_var,
			wrap<BAs...>(p::out, { iovar, offset0 })))));
		auto eNt_is_zero      = build_wff_eq(eNt);
		auto eNt_is_one       = build_wff_eq(build_bf_neg(eNt));
		auto eNt_prev_is_zero = build_wff_eq(eNt_prev);
		auto eNt_prev_is_one  = build_wff_eq(build_bf_neg(eNt_prev));
		// transform `sometimes psi` to:
		// (_eN[t] = 1 && _eN[t-1] = 0) -> psi (N is nth `sometimes`)
		changes[smt] = build_wff_always(build_wff_imply(
				build_wff_and(eNt_is_one, eNt_prev_is_zero),
				smt->child[0]));
		fm = replace(fm, changes);
		changes.clear();
		// for each _eN add conjunction
		// 	always (_eN[0] = 0 && (_eN[t]   = 0 || _eN[t] = 1)
		//                         && (_eN[t-1] = 1 -> _eN[t] = 1))
		auto conj = build_wff_always(
			build_wff_and(eN0_is_zero, build_wff_and(
				build_wff_or(eNt_is_zero, eNt_is_one),
				build_wff_imply(eNt_prev_is_one, eNt_is_one))));
		if (always_conjs == nullptr) always_conjs = conj;
		else always_conjs = build_wff_and(always_conjs, conj);
		// create conjunctions for the transformed `sometimes`
		// 	sometimes _eN[t] & _eN+1[t] & ... & _eN+M[t] = 1
		if (sometimes_conjs == nullptr) sometimes_conjs = eNt;
		else sometimes_conjs = build_bf_and(sometimes_conjs, eNt);
		auto opt_smt = find_top(fm,
				is_non_terminal<p::wff_sometimes, BAs...>);
		if (!opt_smt.has_value()) break;
		smt = opt_smt.value();
	}
	// conjunct all with replaced formula
	auto ret = build_wff_and(fm,
		build_wff_and(always_conjs, build_wff_sometimes(
				build_wff_eq(build_bf_neg(sometimes_conjs)))));
	BOOST_LOG_TRIVIAL(trace) << "(T) -- transformed eventual variables";
	BOOST_LOG_TRIVIAL(trace) << ret;
	return ret;
}

// auto print_free_vars(const auto& fm) {
// 	auto vars = get_free_vars_from_nso(fm);
// 	for (const auto& v : vars) {
// 		cout << v << ", ";
// 	}
// 	cout << "\n";
// }

template<typename... BAs>
nso<BAs...> find_fixpoint_phi (const nso<BAs...>& base_fm, const auto& io_vars,
	const auto& initials, const int_t time_point) {
	nso<BAs...> phi_prev = build_initial_step(base_fm, io_vars, time_point);
	int_t step_num = 1;
	nso<BAs...> cache = phi_prev;
	nso<BAs...> phi = build_step(base_fm, phi_prev, io_vars,
		 initials, step_num, time_point, cache);

	BOOST_LOG_TRIVIAL(debug) << "Continuation at step " << step_num;
	BOOST_LOG_TRIVIAL(debug) << "(F) " << phi;

	int_t max_initial_condition = get_max_initial<BAs...>(io_vars);
	int_t lookback = get_max_shift(io_vars);
	// Find fix point once all initial conditions have been passed and
	// the time_point is greater equal the step_num
	while (step_num < max(max_initial_condition, lookback)
		|| !are_nso_equivalent(phi_prev, phi))
	{
		phi_prev = phi;
		++step_num;

		phi = build_step(base_fm, phi_prev, io_vars,
			initials, step_num, time_point,cache);

		BOOST_LOG_TRIVIAL(debug) << "Continuation at step " << step_num;
		BOOST_LOG_TRIVIAL(debug) << "(F) " << phi;
	}
	BOOST_LOG_TRIVIAL(debug) << "Unbounded continuation of Tau formula "
		"reached fixpoint after " << step_num - 1 << " steps";
	BOOST_LOG_TRIVIAL(debug) << phi_prev;
	return phi_prev;
}

template<typename... BAs>
nso<BAs...> transform_back_non_initials(const nso<BAs...>& fm, const int_t highest_init_cond) {
	// Find lookback
	auto current_io_vars = select_top(fm,
			is_child_non_terminal<tau_parser::io_var, BAs...>);
	int_t lookback = get_lookback_after_normalization(current_io_vars);

	map<nso<BAs...>,nso<BAs...>> changes;
	// Get time positions which are higher than highest_init_cond and transform back to
	// time variable depending on t
	for (const auto& io_var : current_io_vars) {
		int_t time_point = size_t_extractor<BAs...>(
			trim2(trim2(io_var)->child[1])).value();
		if (time_point <= highest_init_cond)
			continue;

		nso<BAs...> transformed_var;
		if (time_point - lookback != 0)
			transformed_var = build_io_out_shift<BAs...>(get_io_name(io_var), "t", abs(time_point - lookback));
		else transformed_var = build_io_out<BAs...>(get_io_name(io_var), "t");

		changes.emplace(io_var, transformed_var);
	}
	return replace(fm, changes);
}

template<typename... BAs>
nso<BAs...> transform_flags_to_streams(nso<BAs...> fm, const int_t lookback) {
	using p = tau_parser;
	map<nso<BAs...>, nso<BAs...>> changes;
	// transform flags to their respective output streams and add required conditions
	size_t flag_id = 0;
	for (const auto& flag : select_top(fm, is_non_terminal<p::flag, BAs...>))
	{
		string flagvar = make_string(tau_node_terminal_extractor<BAs...>,
			find_top(flag, is_non_terminal<p::flagvar, BAs...>).value());
		std::stringstream ss; ss << "_f" << flag_id++;
		auto flag_iovar = build_io_out<BAs...>(ss.str(), flagvar);
		changes[flag] = lookback == 0 ? build_io_out_shift<BAs...>(
										ss.str(), flagvar, 1) : flag_iovar;

		// Take lookback of formula into account for constructing rule
		nso<BAs...> flag_rule1, flag_rule2;
		if (lookback >= 2) {
			flag_rule1 = wrap(tau_parser::bf,
				build_io_out_shift<BAs...>(ss.str(), flagvar, lookback));
			flag_rule2 = wrap(tau_parser::bf,
				build_io_out_shift<BAs...>(ss.str(), flagvar, lookback - 1));
		} else {
			flag_rule1 = wrap(tau_parser::bf,
				build_io_out_shift<BAs...>(ss.str(), flagvar, 1));
			flag_rule2 = wrap(p::bf, flag_iovar);
		}
		if (flag | p::flag_greater || flag | p::flag_greater_equal) {
			// Add flag rule _fk[lookback-1] = 1 -> _fk[lookback] = 1
			auto flag_rule = build_wff_imply(build_wff_eq(build_bf_neg(flag_rule1)),
				build_wff_eq(build_bf_neg(flag_rule2)));
			// Conjunct flag rule with formula
			fm = build_wff_and(fm, flag_rule);
		} else {
			// Flag is of type less or less_equal
			// Add flag rule _fk[lookback-1] = 0 -> _fk[lookback] = 0
			auto flag_rule = build_wff_imply(build_wff_eq(flag_rule1),
				build_wff_eq(flag_rule2));
			// Conjunct flag rule with formula
			fm = build_wff_and(fm, flag_rule);
		}

		// Add initial conditions for flag
		int_t t = 0;
		while (is_initial_flag_phase(flag, t)) {
			nso<BAs...> flag_init_cond =
			transform_io_var(flag_iovar, get_io_name(flag_iovar), t);
			flag_init_cond = wrap(tau_parser::bf, flag_init_cond);
			fm = build_wff_and(build_wff_eq(build_bf_xor(
			flag_init_cond, calculate_flag(flag, t))), fm);
			++t;
		}
	}
	if (!changes.empty()) fm = replace(fm, changes);
	return fm;
}

// This method is designed to be called on the output of find_fixpoint_phi
// when the run was started at the earliest well-defined time point
template<typename... BAs>
bool is_raw_unbound_continuation_satisfiable (const nso<BAs...>& fm) {
	using p = tau_parser;
	auto free_io_vars = get_free_vars_from_nso(fm);
	vector<nso<BAs...> > io_vars = select_top(fm,
				is_child_non_terminal<p::io_var, BAs...>);
	auto initial_comp = [](const auto& v1, const auto& v2) {
		if (get_io_time_point(v1) < get_io_time_point(v2))
			return true;
		if (get_io_time_point(v1) == get_io_time_point(v2)) {
			if (get_io_name(v2)[0] == 'i') return false;
			else return true;
		} else return false;
	};
	sort(io_vars.begin(), io_vars.end(), initial_comp);

	// All io_vars in fm have to refer to constant time positions
	assert(all_of(io_vars.begin(), io_vars.end(),
		[](const auto& el){return is_io_initial(el);}));
	auto sat_fm = fm;
	while(!io_vars.empty()) {
		if (!free_io_vars.contains(io_vars.back())) {
			io_vars.pop_back();
			continue;
		}
		auto& v = io_vars.back();
		if (get_io_name(v)[0] == 'i') sat_fm = build_wff_all(v, sat_fm);
		else sat_fm = build_wff_ex(v, sat_fm);
		io_vars.pop_back();
	}

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Formula for sat check";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << sat_fm;

	return is_non_temp_nso_satisfiable(sat_fm);
}

// We assume that the formula has run through the normalizer before
// and is a single always statement
template<typename... BAs>
nso<BAs...> always_to_unbounded_continuation(nso<BAs...> fm)
{
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin always_to_unbounded_continuation";

	using p = tau_parser;
	assert(has_no_boolean_combs_of_models(fm));
	if (is_child_non_terminal(p::wff_always, fm)) fm = trim2(fm);

	// Preparation to transform flags to output streams
	vector<nso<BAs...> > io_vars = select_top(fm,
				is_child_non_terminal<p::io_var, BAs...>);
	int_t lookback = get_max_shift(io_vars);
	auto transformed_fm = transform_flags_to_streams(fm, lookback);

	if (lookback == 0 && fm != transformed_fm) {
		map<nso<BAs...>, nso<BAs...>> changes;
		for (const auto& io_var : io_vars) {
			// Skip initial conditions
			if (is_io_initial(io_var))
				continue;
			changes[io_var] = build_io_out_shift<BAs...>(
				get_io_name(io_var), "t", 1);
		}
		fm = replace(transformed_fm, changes);
	} else fm = transformed_fm;

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Removed flags";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << fm;

	io_vars = select_top(fm,
				is_child_non_terminal<p::io_var, BAs...>);

	// Save positions of io_variables which are initial conditions
	set<pair<string, int_t>> initials;

	for (int_t i = 0; i < (int_t) io_vars.size(); ++i)
		if (trim2(io_vars[i])->child[1] | p::num)
			initials.emplace(get_io_name(io_vars[i]), size_t_extractor<BAs...>(
				trim2(trim2(io_vars[i])->child[1])).value());

	// Calculate fix point and get unbound continuation of fm
	int_t time_point = get_max_shift(io_vars);
	nso<BAs...> phi_inf = find_fixpoint_phi(fm, io_vars, initials, time_point);
	nso<BAs...> res;
	if (is_raw_unbound_continuation_satisfiable(phi_inf)) {
		int_t point_after_inits = get_max_initial<BAs...>(io_vars) + 1;
		nso<BAs...> unbound_continuation = find_fixpoint_phi(fm, io_vars, initials,
												time_point + point_after_inits);
		unbound_continuation = normalizer_step(unbound_continuation);
		res = transform_back_non_initials(unbound_continuation, point_after_inits - 1);
	} else res = _F<BAs...>;
	BOOST_LOG_TRIVIAL(debug) << "(I) -- End always_to_unbounded_continuation";
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

/*
// TODO (HIGH) check if we could use regular rules instead of callback rules

RULE(TAU_COLLAPSE_POSITIVES_0, "$X &&& $Y :::= tau_collapse_positives_cb $X $Y.")
RULE(TAU_COLLAPSE_POSITIVES_1, "$X &&& ($Y &&& $Z) :::= tau_collapse_positives_cb $X $Y $Z.")
RULE(TAU_COLLAPSE_POSITIVES_2, "$X &&& ($Y &&& $Z) :::= tau_collapse_positives_cb $X $Z $Y.")
RULE(TAU_COLLAPSE_POSITIVES_3, "$X &&& ($Y &&& $Z) :::= tau_collapse_positives_cb $Y $Z $X.")
RULE(TAU_COLLAPSE_POSITIVES_4, "($X &&& $Y) &&& $Z :::= tau_collapse_positives_cb $Y $Z $X.")
RULE(TAU_COLLAPSE_POSITIVES_5, "($X &&& $Y) &&& $Z :::= tau_collapse_positives_cb $X $Z $Y.")
RULE(TAU_COLLAPSE_POSITIVES_6, "($X &&& $Y) &&& $Z :::= tau_collapse_positives_cb $X $Y $Z.")
RULE(TAU_PUSH_POSITIVES_UPWARDS_0, "$X &&& ($Y &&& $Z) :::= tau_positives_upwards_cb $Y ($Y &&& ($X &&& $Z)).")
RULE(TAU_PUSH_POSITIVES_UPWARDS_1, "$X &&& ($Y &&& $Z) :::= tau_positives_upwards_cb $Z ($Z &&& ($X &&& $Y)).")
RULE(TAU_PUSH_POSITIVES_UPWARDS_2, "($X &&& $Y) &&& $Z :::= tau_positives_upwards_cb $X ($X &&& ($Y &&& $Z)).")
RULE(TAU_PUSH_POSITIVES_UPWARDS_3, "($X &&& $Y) &&& $Z :::= tau_positives_upwards_cb $Y ($Y &&& ($X &&& $Z)).")

template<typename... BAs>
static auto collapse_positives_tau = make_library<BAs...>(
	TAU_COLLAPSE_POSITIVES_0
	+ TAU_COLLAPSE_POSITIVES_1
	+ TAU_COLLAPSE_POSITIVES_2
	+ TAU_COLLAPSE_POSITIVES_3
	+ TAU_COLLAPSE_POSITIVES_4
	+ TAU_COLLAPSE_POSITIVES_5
	+ TAU_COLLAPSE_POSITIVES_6
	+ TAU_PUSH_POSITIVES_UPWARDS_0
	+ TAU_PUSH_POSITIVES_UPWARDS_1
	+ TAU_PUSH_POSITIVES_UPWARDS_2
	+ TAU_PUSH_POSITIVES_UPWARDS_3
);

template<typename... BAs>
void get_gssotc_clauses(const gssotc<BAs...>& n, std::vector<gssotc<BAs...>>& clauses) {
	if (auto check = n | tau_parser::tau_or; check.has_value()) {
		for (auto& c: check || tau_parser::tau) get_gssotc_clauses(c , clauses);
	} else {
		clauses.push_back(n);
		BOOST_LOG_TRIVIAL(trace) << "(I) found get_gssotc_clause: " << n;
	}
}

template<typename... BAs>
std::vector<gssotc<BAs...>> get_gssotc_clauses(const gssotc<BAs...>& n) {
	std::vector<gssotc<BAs...>> clauses;
	get_gssotc_clauses(n, clauses);
	return clauses;
}

template<typename... BAs>
void get_gssotc_literals(const gssotc<BAs...>& clause, std::vector<gssotc<BAs...>>& literals) {
	if (auto check = clause | tau_parser::tau_and; check.has_value())
		for (auto& c: check || tau_parser::tau) get_gssotc_literals(c , literals);
	else {
		literals.push_back(clause);
		BOOST_LOG_TRIVIAL(trace) << "(I) found get_gssotc_clause: " << clause;
	}
}

template<typename... BAs>
std::vector<gssotc<BAs...>> get_gssotc_literals(const gssotc<BAs...>& clause) {
	std::vector<gssotc<BAs...>> literals;
	get_gssotc_literals(clause, literals);
	return literals;
}

template<typename... BAs>
static const auto is_gssotc_positive_literal = [] (const gssotc<BAs...>& n) {
	auto check = n | tau_parser::tau_wff;
	return check.has_value();
};
template<typename... BAs>
using is_gssotc_positive_literal_t = decltype(is_gssotc_positive_literal<BAs...>);

template<typename... BAs>
static const auto is_gssotc_negative_literal = [] (const gssotc<BAs...>& n) {
	auto check = n | tau_parser::tau_neg | tau_parser::wff;
	return check.has_value();
};

template<typename... BAs>
using is_gssotc_negative_literal_t = decltype(is_gssotc_negative_literal<BAs...>);

template<typename... BAs>
std::pair<std::optional<gssotc<BAs...>>, std::vector<gssotc<BAs...>>> get_gssotc_positive_negative_literals(const gssotc<BAs...> clause) {
	std::optional<gssotc<BAs...>> positive;
	std::vector<gssotc<BAs...>> negatives;

	for(auto& l: get_gssotc_literals(clause)) {
		if (is_gssotc_positive_literal<BAs...>(l)) positive = {l};
		else negatives.push_back(l);
	}

	#ifdef DEBUG
	if (positive.has_value()) BOOST_LOG_TRIVIAL(trace) << "(I) positive: " << positive.value();
	if (!negatives.empty()) {
		for (auto& n: negatives) BOOST_LOG_TRIVIAL(trace) << "(I) negative: " << n;
	}
	#endif // DEBUG

	return {positive, negatives};
}

template<typename...BAs>
struct tau_spec_vars {

	void add(const gssotc<BAs...>& io) {
		vars.emplace(io);
		auto offset = io
			| only_child_extractor<tau_ba<BAs...>, BAs...>
			| tau_parser::offset
			| optional_value_extractor<gssotc<BAs...>>;
		auto pos = offset
			| only_child_extractor<tau_ba<BAs...>, BAs...>
			| non_terminal_extractor<tau_ba<BAs...>, BAs...>
			| optional_value_extractor<size_t>;
		auto var_name = (io
			| only_child_extractor<tau_ba<BAs...>, BAs...>
			| optional_value_extractor<gssotc<BAs...>>)->child[0];
		name.emplace(var_name);
		switch (pos) {
			case tau_parser::num:
				loopback = max(loopback,
					offset | tau_parser::num
					| only_child_extractor<tau_ba<BAs...>, BAs...>
					| offset_extractor<tau_ba<BAs...>, BAs...>
					| optional_value_extractor<size_t>);
				break;
			case tau_parser::shift:
				loopback = max(loopback,
					offset | tau_parser::shift
					| tau_parser::num
					| only_child_extractor<tau_ba<BAs...>, BAs...>
					| offset_extractor<tau_ba<BAs...>, BAs...>
					| optional_value_extractor<size_t>);
				break;
			default:
				break;
		}
	}

	std::set<gssotc<BAs...>> vars;
	std::set<gssotc<BAs...>> name;
	size_t loopback = 0;
};

template<typename... BAs>
std::pair<tau_spec_vars<BAs...>, tau_spec_vars<BAs...>> get_gssotc_shifted_io_vars(const gssotc<BAs...> collapsed) {
	tau_spec_vars<BAs...> inputs;
	tau_spec_vars<BAs...> outputs;
	for (const auto& variable: select_top(collapsed, is_non_terminal<tau_parser::io_var, tau_ba<BAs...>, BAs...>)) {
		auto type = variable
			| only_child_extractor<tau_ba<BAs...>, BAs...>
			| non_terminal_extractor<tau_ba<BAs...>, BAs...>
			| optional_value_extractor<size_t>;
		auto shift = variable
			| only_child_extractor<tau_ba<BAs...>, BAs...>
			| tau_parser::offset
			| tau_parser::shift;
		if (shift.has_value()) {
			(type == tau_parser::in ? inputs : outputs).add(variable);
		}
	}

	BOOST_LOG_TRIVIAL(trace) << "(I) inputs:\n" ; for (auto& i: inputs.name) BOOST_LOG_TRIVIAL(trace) << i;
	BOOST_LOG_TRIVIAL(trace) << "(I) outputs:\n" ; for (auto& o: outputs.name) BOOST_LOG_TRIVIAL(trace) << o;

	return {inputs, outputs};
}


template<typename... BAs>
std::pair<tau_spec_vars<BAs...>, tau_spec_vars<BAs...>> get_gssotc_io_vars(const gssotc<BAs...> collapsed) {
	tau_spec_vars<BAs...> inputs;
	tau_spec_vars<BAs...> outputs;
	for (const auto& variable: select_top(collapsed, is_non_terminal<tau_parser::io_var, tau_ba<BAs...>, BAs...>)) {
		auto type = variable
			| only_child_extractor<tau_ba<BAs...>, BAs...>
			| non_terminal_extractor<tau_ba<BAs...>, BAs...>
			| optional_value_extractor<size_t>;
		(type == tau_parser::in ? inputs : outputs).add(variable);
	}

	BOOST_LOG_TRIVIAL(trace) << "(I) inputs:\n" ; for (auto& i: inputs.name) BOOST_LOG_TRIVIAL(trace) << i;
	BOOST_LOG_TRIVIAL(trace) << "(I) outputs:\n" ; for (auto& o: outputs.name) BOOST_LOG_TRIVIAL(trace) << o;

	return {inputs, outputs};
}

template<typename... BAs>
std::string build_args_from_vars(const tau_spec_vars<BAs...>& vars, size_t index) {
	std::basic_stringstream<char> str;
	for (const auto& var: vars.name) {
		print_terminals(str, var) << "[" << index << "]";
		if (var != *vars.name.rbegin()) str << ", ";
	}
	return str.str();
}

template<typename... BAs>
std::string build_args_from_vars(const tau_spec_vars<BAs...>& vars) {
	std::basic_stringstream<char> str;
	for (const auto& var: vars.name) {
		print_terminals(str, var);
		if (var != *vars.name.rbegin()) str << ", ";
	}
	return str.str();
}

template<typename... BAs>
std::string build_args_from_vars(const tau_spec_vars<BAs...>& inputs, const tau_spec_vars<BAs...>& outputs) {
	std::basic_stringstream<char> str;
	for (const auto& var: inputs.vars) {
		print_terminals(str, var);
		if (var != *inputs.name.rbegin()) str << ", ";
	}
	if (!inputs.vars.empty() && !outputs.vars.empty()) str << ", ";
	for (const auto& var: outputs.vars) {
		print_terminals(str, var);
		if (var != *outputs.name.rbegin()) str << ", ";
	}
	return str.str();
}

template<typename... BAs>
std::string build_universal_quantifiers(const tau_spec_vars<BAs...>& vars, size_t index) {
	std::basic_stringstream<char> str;
	for (const auto& var: vars.name) {
		print_terminals(str << "all ", var) << "[" << index << "] ";
	}
	return str.str();
}

template<typename... BAs>
std::string build_universal_quantifiers(const tau_spec_vars<BAs...>& vars) {
	std::basic_stringstream<char> str;
	for (const auto& var: vars.vars) str << "all " << var << " ";
	return str.str();
}

template<typename... BAs>
std::string build_existential_quantifiers(const tau_spec_vars<BAs...>& vars, size_t index) {
	std::basic_stringstream<char> str;
	for (const auto& var: vars.name) {
		print_terminals(str << "ex ", var) << "[" << index << "] ";
	}
	return str.str();
}

template<typename... BAs>
std::string build_existential_quantifiers(const tau_spec_vars<BAs...>& vars) {
	std::basic_stringstream<char> str;
	for (const auto& var: vars.vars) str << "ex " << var << " ";
	return str.str();
}

template<typename... BAs>
std::string build_boolean_existential_quantifiers(size_t size) {
	std::basic_stringstream<char> str;
	for (size_t i = 1; i <= size; ++i) {
		str << "bex $nn" << i;
		if (i < size) str << ",";
	}
	return str.str();
}

template<typename... BAs>
std::pair<std::string, bindings<tau_ba<BAs...>, BAs...>>  build_eta_nso_rr(
		const std::optional<gssotc<BAs...>>& positive,
		const std::vector<gssotc<BAs...>>& negatives,
		const tau_spec_vars<BAs...>& inputs,
		const tau_spec_vars<BAs...>& outputs) {
	auto print_t = [] (size_t negatives) {
		std::basic_stringstream<char> str;
		for (size_t i = 1; i <= negatives; ++i) {
			str << "t";
			if (i < negatives) str << ",";
		}
		return str.str();
	};
	auto print_negative_vars = [] (size_t negatives) {
		std::basic_stringstream<char> str;
		for (size_t i = 1; i <= negatives; ++i) {
			str << "$n" << i;
			if (i < negatives) str << ",";
		}
		return str.str();
	};
	auto print_new_negative_vars = [] (size_t negatives) {
		std::basic_stringstream<char> str;
		for (size_t i = 1; i <= negatives; ++i) {
			str << "$nn" << i;
			if (i < negatives) str << ",";
		}
		return str.str();
	};

	bindings<tau_ba<BAs...>, BAs...> bindings;
	size_t loopback = max(inputs.loopback, outputs.loopback);
	std::basic_stringstream<char> nsorr;
	nsorr << "eta[t](" << build_args_from_vars(outputs) << ") := ";
		// add call to etas
		if (negatives.size() > 0) {
			nsorr << "etas[t](" << build_args_from_vars(outputs) << print_t(outputs.name.size()) << ").\n";
		}
	nsorr << "etas[t](" << build_args_from_vars(outputs) << print_negative_vars(outputs.name.size()) <<") := ";
		// quantify input and output variables
		for (size_t i = 1; i <= loopback; ++i) {
			nsorr << build_universal_quantifiers(inputs, i);
			nsorr << build_existential_quantifiers(outputs, i);
		}
		// boolean quantify new negative vars
		nsorr << build_boolean_existential_quantifiers(outputs.name.size());
		// add positive literal if it exists
		if (positive.has_value()) {
			nsorr << positive.value();
			if (negatives.size() > 0) nsorr << " && ";
		}
		// generate a disjunction of all negations conjucted with the max_loopback variables and
		if (negatives.size() > 0) {
			for (size_t i = 0; i < negatives.size(); ++i) {
				nsorr << "(( $n" << i << " && " << "$nn" << i << " ) &&& " << negatives[i] << ") &&& ";
			}
			// add recursive call with t-1
			nsorr << "etas[t-1](" << build_args_from_vars(outputs) << print_new_negative_vars(outputs.name.size()) << ").\n";
		}
	// add main

	BOOST_LOG_TRIVIAL(trace) << "(I) build_eta_nso_rr: " << nsorr.str();

	return {nsorr.str(), bindings};
}


template<typename... BAs>
std::string build_check_nso_rr(const tau_spec_vars<BAs...>& outputs, size_t loopback, size_t current) {
	std::basic_stringstream<char> check;
	check << build_existential_quantifiers(outputs, loopback) <<
		" (eta[" << current << "](" << build_args_from_vars(outputs) << ").\n";

	BOOST_LOG_TRIVIAL(trace) << "(I) build_check_nso_rr: " << check.str();

	return check.str();
}

template<typename... BAs>
std::string build_main_nso_rr(const tau_spec_vars<BAs...>& outputs, size_t loopback, size_t current, size_t previous) {
	std::basic_stringstream<char> main;
	main << build_universal_quantifiers(outputs, loopback) <<
		" (eta[" << current << "](" << build_args_from_vars(outputs) << ") <-> eta[" << previous << "]).\n";

	BOOST_LOG_TRIVIAL(trace) << "(I) build_main_nso_rr: " << main.str();

	return main.str();
}


template<typename... BAs>
std::pair<std::string, bindings<tau_ba<BAs...>, BAs...>> build_main_nso_rr_wff(const std::optional<gssotc<BAs...>>& positive, const tau_spec_vars<BAs...>& inputs, const tau_spec_vars<BAs...>& outputs, size_t loopback) {
	std::basic_stringstream<char> main;
	bindings<tau_ba<BAs...>, BAs...> bindings;

	auto wff = positive | tau_parser::tau_wff | tau_parser::wff| optional_value_extractor<gssotc<BAs...>>;

	// print quantifiers related to free variables
	for (size_t i = 1; i <= loopback; ++i) {
		main << build_universal_quantifiers(inputs, i);
		main << build_existential_quantifiers(outputs, i);
	}
	// print the main wff
	main << build_string_from_clause(wff, bindings)	<< ".";

	BOOST_LOG_TRIVIAL(trace) << "(I) get_main_nso_wo_rr: " << main.str();

	return {main.str(), bindings};
}

template<typename... BAs>
std::pair<std::string, bindings<tau_ba<BAs...>, BAs...>> build_main_nso_rr_wff_no_loopbacks(const std::optional<gssotc<BAs...>>& positive, const tau_spec_vars<BAs...>& inputs, const tau_spec_vars<BAs...>& outputs) {
	std::basic_stringstream<char> main;
	bindings<tau_ba<BAs...>, BAs...> bindings;

	auto wff = positive | tau_parser::tau_wff | tau_parser::wff| optional_value_extractor<gssotc<BAs...>>;

	// print quantifiers related to free variables
	main << build_universal_quantifiers(inputs);
	main << build_existential_quantifiers(outputs);
	// print the main wff
	main << wff	<< ".\n";

	BOOST_LOG_TRIVIAL(trace) << "(I) get_main_nso_wo_rr: " << main.str();

	return {main.str(), bindings};
}

template<typename... BAs>
bool is_gssotc_clause_satisfiable_no_outputs(const gssotc<BAs...>& clause, const tau_spec_vars<BAs...>& inputs) {
	auto wff = clause | tau_parser::tau_wff | tau_parser::wff | optional_value_extractor<gssotc<BAs...>>;
	bindings<tau_ba<BAs...>, BAs...> bindings;
	std::basic_stringstream<char> main;
	main << build_universal_quantifiers(inputs) << wff << ".";
	std::string str = main.str();
	auto normalized = normalizer<tau_ba<BAs...>, BAs...>(str, bindings).main;
	auto check = normalized | tau_parser::wff_t;
	if (check.has_value()) {
		BOOST_LOG_TRIVIAL(trace) << "(I) -- Check is_gssotc_clause_satisfiable: true";
		return true;
	}

	BOOST_LOG_TRIVIAL(trace) << "(I) -- Check is_gssotc_clause_satisfiable: false";
	return false;
}

template<typename... BAs>
bool is_gssotc_clause_satisfiable_no_negatives_no_loopback(const std::optional<gssotc<BAs...>>& positive,  const tau_spec_vars<BAs...>& inputs, const tau_spec_vars<BAs...>& outputs) {

	// TODO (HIGH) fix formula to be normalized, must include a phi call and a phi definition
	auto [main_wo_rr, bindings] = build_main_nso_rr_wff_no_loopbacks<BAs...>(positive, inputs, outputs);
	BOOST_LOG_TRIVIAL(trace) << "(I) -- Check normalizer";
	BOOST_LOG_TRIVIAL(trace) << main_wo_rr;
	auto normalize = normalizer<tau_ba<BAs...>, BAs...>(main_wo_rr, bindings).main;

	if ((normalize | tau_parser::wff_f).has_value()) {
		BOOST_LOG_TRIVIAL(trace) << "(I) -- Check is_gssotc_clause_satisfiable: false";
		return false;
	}

	BOOST_LOG_TRIVIAL(trace) << "(I) -- Check is_gssotc_clause_satisfiable: true";
	return true;
}

template<typename... BAs>
gssotc<BAs...> get_loopback_adjusted_nso(const gssotc<BAs...>& positive, size_t loopback) {
	std::map<gssotc<BAs...>, gssotc<BAs...>> changes;

	for (const auto& io: select_top(positive, is_non_terminal<tau_parser::io_var, tau_ba<BAs...>, BAs...>)) {
		auto shift = io
			| only_child_extractor<tau_ba<BAs...>, BAs...>
			| tau_parser::offset
			| tau_parser::shift;
		if (shift.has_value()) {
			auto num = io
				| only_child_extractor<tau_ba<BAs...>, BAs...>
				| tau_parser::offset
				| tau_parser::shift
				| tau_parser::num;
			auto val = num
				| only_child_extractor<tau_ba<BAs...>, BAs...>
				| offset_extractor<tau_ba<BAs...>, BAs...>
				| optional_value_extractor<size_t>;
			auto nval = loopback + 1 - val;
			auto nnum = build_num_from_num<tau_ba<BAs...>, BAs...>(shift.value(), nval);
			changes[shift.value()] = nnum;
		}
	}
	return replace<gssotc<BAs...>>(positive, changes);
}

template<typename... BAs>
std::string build_phi_invocation(const std::optional<gssotc<BAs...>>& positive, size_t loopback) {
	std::basic_stringstream<char> phi;
	auto npositive = get_loopback_adjusted_nso<BAs...>(positive.value(), loopback);
	phi << npositive;
	return phi.str();
}

template<typename... BAs>
std::string build_phi_main_nso_rr(const std::optional<gssotc<BAs...>>& positive, const tau_spec_vars<BAs...>& inputs, const tau_spec_vars<BAs...>& outputs, size_t loopback) {
	std::basic_stringstream<char> main_phi;
	main_phi << build_universal_quantifiers(inputs, 1);
	main_phi << " phi[t](" << build_args_from_vars(inputs, 1) << ").\n";
	return main_phi.str();
}

template<typename... BAs>
std::string build_phi_general_case_nso_rr(const std::optional<gssotc<BAs...>>& positive, const tau_spec_vars<BAs...>& inputs, const tau_spec_vars<BAs...>& outputs, size_t loopback) {
	std::basic_stringstream<char> gen_phi;

	auto wff = positive | tau_parser::tau_wff | tau_parser::wff | optional_value_extractor<gssotc<BAs...>>;
	auto nwff = get_loopback_adjusted_nso<BAs...>(wff, loopback);

	gen_phi << "phi[t](" << build_args_from_vars(outputs, 1) << ") ::= ";
	for (size_t i = 1; i < loopback + 1; ++i) {
		gen_phi << build_universal_quantifiers(inputs, i);
		gen_phi << build_existential_quantifiers(outputs, i);
	}
	gen_phi << "( phi[t-1](" << build_args_from_vars(inputs, loopback + 1) << ") ";
	gen_phi << "&& " << nwff << " ).\n";

	BOOST_LOG_TRIVIAL(trace) << "(I) -- Result build_phi_general_case_nso_rr:\n" << gen_phi.str() << "\n";

	return gen_phi.str();
}

template<typename... BAs>
std::string build_phi_base_case_nso_rr(const std::optional<gssotc<BAs...>>& positive, const tau_spec_vars<BAs...>& inputs, const tau_spec_vars<BAs...>& outputs, size_t loopback) {
	std::basic_stringstream<char> base_phi;

	auto wff = positive | tau_parser::tau_wff | tau_parser::wff | optional_value_extractor<gssotc<BAs...>>;
	auto nwff = get_loopback_adjusted_nso<BAs...>(wff, loopback + 1);

	base_phi << "phi[" << loopback + 1 << "](" << build_args_from_vars(outputs, 1) << ") ::= ";
	for (size_t i = 2; i <= loopback + 1; ++i) {
		base_phi << build_universal_quantifiers(inputs, i);
		base_phi << build_existential_quantifiers(outputs, i);
	}
	// print the main wff
	base_phi << nwff << ".\n";

	BOOST_LOG_TRIVIAL(trace) << "(I) -- Result build_phi_base_case_nso_rr:\n" << base_phi.str() << "\n";

	return base_phi.str();
}


template<typename... BAs>
bool is_gssotc_clause_satisfiable_no_negatives_with_loopback(const std::optional<gssotc<BAs...>>& positive,  const tau_spec_vars<BAs...>& inputs, const tau_spec_vars<BAs...>& outputs, size_t loopback) {
	std::basic_stringstream<char> nso_rr;

	nso_rr
		<< build_phi_base_case_nso_rr<BAs...>(positive, inputs, outputs, loopback)
		<< build_phi_general_case_nso_rr<BAs...>(positive, inputs, outputs, loopback)
		<< build_phi_main_nso_rr<BAs...>(positive, inputs, outputs, loopback);

	BOOST_LOG_TRIVIAL(trace) << "(I) -- Check normalizer";
	BOOST_LOG_TRIVIAL(trace) << nso_rr.str();

	// TODO (MEDIUM) remove bindings from the following code
	bindings<tau_ba<BAs...>, BAs...> bindings;
	std::string source = nso_rr.str();
	auto normalized = normalizer<tau_ba<BAs...>, BAs...>(source, bindings).main;

	if ((normalized | tau_parser::wff_f).has_value()) {
		BOOST_LOG_TRIVIAL(trace) << "(I) -- Check is_gssotc_clause_satisfiable: false";
		return false;
	}

	BOOST_LOG_TRIVIAL(trace) << "(I) -- Check is_gssotc_clause_satisfiable: true";
	return true;
}

template<typename... BAs>
bool is_gssotc_clause_satisfiable_general(const std::optional<gssotc<BAs...>>& positive, const std::vector<gssotc<BAs...>> negatives,  const tau_spec_vars<BAs...>& inputs, const tau_spec_vars<BAs...>& outputs, size_t loopback) {
	auto etas = build_eta_nso_rr<BAs...>(positive, negatives, inputs, outputs);
	for (size_t current = 1;  until return statement ; ++current) {
		auto [eta, extracted_bindings] = build_eta_nso_rr<BAs...>(positive, negatives, inputs, outputs);
		auto check = build_check_nso_rr(outputs, loopback, current);
		auto normalize = normalizer<tau_ba<BAs...>, BAs...>(eta.append(check), extracted_bindings).main;
		if ((normalize | tau_parser::wff_f).has_value()) {
			BOOST_LOG_TRIVIAL(trace) << "(I) --Check is_gssotc_clause_satisfiable: false";
			return false;
		}
		for (size_t previous = 1; previous < current; ++previous) {
			auto main = build_main_nso_rr(outputs, loopback, current, previous);
			auto normalize = normalizer<tau_ba<BAs...>, BAs...>(eta.append(main), extracted_bindings).main;
			if ((normalize | tau_parser::wff_t).has_value()) return true;
		}
	}
}

template<typename... BAs>
bool is_gssotc_clause_satisfiable(const gssotc<BAs...>& clause) {

	BOOST_LOG_TRIVIAL(trace) << "(I) -- Checking is_gssotc_clause_satisfiable";
	BOOST_LOG_TRIVIAL(trace) << clause;

	auto collapsed = clause |
		repeat_all<step<tau_ba<BAs...>, BAs...>, tau_ba<BAs...>, BAs...>(
			simplify_tau<tau_ba<BAs...>, BAs...>
			| collapse_positives_tau<tau_ba<BAs...>, BAs...>);

	auto [positive, negatives] = get_gssotc_positive_negative_literals(collapsed);
	auto [inputs, outputs] = get_gssotc_io_vars(collapsed);
	size_t loopback = max(inputs.loopback, outputs.loopback);

	// TODO (HIGH) Case no output variables but input variables
	if (outputs.name.empty()) {

		BOOST_LOG_TRIVIAL(trace) << "(I) -- No variables case";
		BOOST_LOG_TRIVIAL(trace) << "(F) " << collapsed;

		return is_gssotc_clause_satisfiable_no_outputs(collapsed, inputs);
	} else if (negatives.empty() && positive.has_value() && loopback == 0) {

		BOOST_LOG_TRIVIAL(trace) << "(I) -- No negatives and no loopback case";
		BOOST_LOG_TRIVIAL(trace) << clause;

		return is_gssotc_clause_satisfiable_no_negatives_no_loopback(positive, inputs, outputs);
	} else if (negatives.empty() && positive.has_value()) {

		BOOST_LOG_TRIVIAL(trace) << "(I) -- No negatives with loopback case";
		BOOST_LOG_TRIVIAL(trace) << clause;

		return is_gssotc_clause_satisfiable_no_negatives_with_loopback(positive, inputs, outputs, loopback);
	}


	BOOST_LOG_TRIVIAL(trace) << "(I) -- General case";
	BOOST_LOG_TRIVIAL(trace) << clause;

	return is_gssotc_clause_satisfiable_general(positive, negatives, inputs, outputs, loopback);
}

template<typename... BAs>
bool is_gssotc_satisfiable(const gssotc<BAs...>& form) {
	auto dnf = form
		| repeat_all<step<tau_ba<BAs...>, BAs...>, tau_ba<BAs...>, BAs...>(
			to_dnf_tau<tau_ba<BAs...>, BAs...>
			| simplify_tau<tau_ba<BAs...>, BAs...>);

	BOOST_LOG_TRIVIAL(trace) << "(I) -- Converting to dnf and simplifying";
	BOOST_LOG_TRIVIAL(trace) << dnf;

	auto clauses = get_gssotc_clauses(dnf);
	for (auto& clause: clauses) {
		if (is_gssotc_clause_satisfiable(clause)) return true;
	}
	return false;
}

template <typename... BAs>
bool is_gssotc_equivalent_to(gssotc<BAs...> n1, gssotc<BAs...> n2) {
	return !is_gssotc_satisfiable(build_tau_neg(build_tau_equiv(n1, n2)));
}

template <typename... BAs>
auto is_gssotc_equivalent_to_any_of(const gssotc<BAs...>& n, std::vector<gssotc<BAs...>>& previous) {
	return std::any_of(previous.begin(), previous.end(), [n] (const gssotc<BAs...>& p) {
		return is_gssotc_equivalent_to<BAs...>(n, p);
	});
}

// check satisfability of a tau_spec (boolean combination case)
template<typename...BAs>
bool is_tau_spec_satisfiable(const tau_spec<BAs...>& tau_spec) {
	BOOST_LOG_TRIVIAL(trace) << "(I) -- Begin is_tau_spec_satisfiable tau_spec ";
	BOOST_LOG_TRIVIAL(trace) << tau_spec;

	auto loopback = get_max_loopback_in_rr(tau_spec.main);
	BOOST_LOG_TRIVIAL(trace) << "(I) Max loopback: " << loopback;

	std::vector<gssotc<BAs...>> previous;

	for (int i = loopback; ; i++) {
		auto current = build_main_step<tau_ba<BAs...>, BAs...>(tau_spec.main, i)
			| repeat_all<step<tau_ba<BAs...>, BAs...>, tau_ba<BAs...>, BAs...>(step<tau_ba<BAs...>, BAs...>(tau_spec.rec_relations));

		BOOST_LOG_TRIVIAL(trace) << "(I) -- Begin is_tau_spec_satisfiable step";
		BOOST_LOG_TRIVIAL(trace) << current;

		if (!is_gssotc_satisfiable(current)) {
			BOOST_LOG_TRIVIAL(trace) << "(I) -- End is_tau_spec_satisfiable: false";
			return false;
		}
		if (is_gssotc_equivalent_to_any_of(current, previous)) {
			BOOST_LOG_TRIVIAL(trace) << "(I) -- End is_tau_spec_satisfiable: true";
			return true;
		} else previous.push_back(current);
	}
}
*/

} // namespace idni::tau

#endif // __SATISFIABILITY_H__