// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __SATISFIABILITY_H__
#define __SATISFIABILITY_H__

#include "normalizer.h"

namespace idni::tau_lang {

inline static bool use_debug_output_in_sat = false;
inline void print_fixpoint_info(const std::string& message, const std::string& result, const bool output) {
	if (!output) return;
	if (!use_debug_output_in_sat) BOOST_LOG_TRIVIAL(info) << "\n" << message << "\n" << result << "\n\n";
	else std::cerr << message << "\n" << result << "\n";
}

template<typename... BAs>
tau<BAs...> build_io_out (const std::string& name, const std::string& var) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::out_var_name, name);
	auto offset = wrap<BAs...>(p::offset, wrap<BAs...>(p::variable, var));
	return wrap(p::variable, wrap(p::io_var, wrap(p::out, { var_name, offset })));
}

template<typename... BAs>
tau<BAs...> build_io_in (const std::string& name, const std::string& var) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::in_var_name, name);
	auto offset = wrap<BAs...>(p::offset, wrap<BAs...>(p::variable, var));
	return wrap(p::variable, wrap(p::io_var, wrap(p::in, { var_name, offset })));
}

template<typename... BAs>
tau<BAs...> build_io_out_const (const std::string& name, const int_t pos) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::out_var_name, name);
	auto offset = wrap<BAs...>(p::offset, build_num<BAs...>(pos));
	return wrap(p::variable, wrap(p::io_var, wrap(p::out, { var_name, offset })));
}

template<typename... BAs>
tau<BAs...> build_io_in_const (const std::string& name, const int_t pos) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::in_var_name, name);
	auto offset = wrap<BAs...>(p::offset, build_num<BAs...>(pos));
	return wrap(p::variable, wrap(p::io_var, wrap(p::in, { var_name, offset })));
}

template<typename... BAs>
tau<BAs...> build_io_out_shift (const std::string& name, const std::string& var, const int_t shift) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::out_var_name, name);
	auto shift_node = wrap<BAs...>(p::shift, {wrap<BAs...>(p::variable, var), build_num<BAs...>(shift)});
	auto offset = wrap<BAs...>(p::offset, shift_node);
	return wrap(p::variable, wrap(p::io_var, wrap(p::out, { var_name, offset })));
}

template<typename... BAs>
tau<BAs...> build_io_in_shift (const std::string& name, const std::string& var, const int_t shift) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::in_var_name, name);
	auto shift_node = wrap<BAs...>(p::shift, {wrap<BAs...>(p::variable, var), build_num<BAs...>(shift)});
	auto offset = wrap<BAs...>(p::offset, shift_node);
	return wrap(p::variable, wrap(p::io_var, wrap(p::in, { var_name, offset })));
}

template<typename... BAs>
int_t get_lookback_after_normalization(const auto& io_vars) {
	int_t max_lookback = 0;
	for (const auto& v : io_vars) {
		if (is_io_initial(v)) {
			int_t lookback = get_io_time_point(v);
			max_lookback = std::max(max_lookback, lookback);
		}
	}
	return max_lookback;
}

// Check if a formula has a temporary output stream
// which are used for flag handling
template<typename... BAs>
bool has_temporary_io_var (const tau<BAs...>& fm) {
	using p = tau_parser;
	auto io_vars = rewriter::select_top(
		fm, is_child_non_terminal<p::io_var, BAs...>);
	for (const auto& var : io_vars) {
		// Check if the name of var starts with "_"
		if (get_io_name(var)[0] == '_')
			return true;
	}
	return false;
}

template<typename... BAs>
tau<BAs...> transform_io_var(const tau<BAs...>& io_var, const std::string& io_var_name, int_t time_point) {
	// Check if io_var has constant time point
	if (is_io_initial(io_var))
		return io_var;
	auto shift = get_io_var_shift(io_var);
	if (io_var | tau_parser::io_var | tau_parser::in)
		return build_io_in_const<BAs...>(io_var_name, time_point - shift);
	else return build_io_out_const<BAs...>(io_var_name, time_point - shift);
}

template<typename... BAs>
tau<BAs...> existentially_quantify_output_streams(tau<BAs...> fm, const auto& io_vars,
                                                  int_t time_point, const auto& initials) {
	// This map is needed in order to get the minimal shift for streams with same name
	std::set<int_t> quantifiable_o_vars;
	for (int_t i = 0; i < (int_t)io_vars.size(); ++i) {
		// Skip input streams
		if (io_vars[i] | tau_parser::io_var | tau_parser::in)
			continue;
		// Skip initial conditions
		if (is_io_initial(io_vars[i]))
			continue;
		quantifiable_o_vars.insert(i);
	}
	std::set<tau<BAs...>> cache;
	for (const auto& pos : quantifiable_o_vars) {
		// Do not quantify time steps which are predefined by initial conditions
		if (initials.contains({get_io_name(io_vars[pos]), time_point}))
			continue;
		tau<BAs...> var = build_io_out_const<BAs...>(get_io_name(io_vars[pos]), time_point);
		auto res = cache.emplace(var);
		if (res.second) fm = build_wff_ex(var, fm);
	}
	return fm;
}

template<typename... BAs>
tau<BAs...> universally_quantify_input_streams(tau<BAs...> fm, const auto& io_vars,
                                               int_t time_point, const auto& initials) {
	// This map is needed in order to get the minimal shift for streams with same name
	std::set<int_t> quantifiable_i_vars;
	for (int_t i = 0; i < (int_t)io_vars.size(); ++i) {
		// Skip output streams
		if (io_vars[i] | tau_parser::io_var | tau_parser::out)
			continue;
		// Skip initial conditions
		if (is_io_initial(io_vars[i]))
			continue;
		quantifiable_i_vars.insert(i);
	}
	std::set<tau<BAs...>> cache;
	for (const auto& pos : quantifiable_i_vars) {
		// Do not quantify time steps which are predefined by initial conditions
		if (initials.contains({get_io_name(io_vars[pos]), time_point}))
			continue;
		tau<BAs...> var = build_io_in_const<BAs...>(get_io_name(io_vars[pos]), time_point);
		auto res = cache.emplace(var);
		if (res.second) fm = build_wff_all(var, fm);
	}
	return fm;
}

template<typename... BAs>
tau<BAs...> calculate_ctn(const tau<BAs...>& constraint, int_t time_point) {
	auto to_ba = [](const bool c){return c ? _1<BAs...> : _0<BAs...>;};
	int_t condition;
	bool is_left;
	if (is_non_terminal(tau_parser::num, trim2(constraint))) {
		is_left = true;
		condition = size_t_extractor<BAs...>(trim(trim2(constraint))).value();
	}
	else {
		is_left = false;
		condition = size_t_extractor<BAs...>(trim(constraint)->child[1]->child[0]).value();
	}
	if (constraint | tau_parser::ctn_neq)
		return to_ba(condition != time_point);
	if (constraint | tau_parser::ctn_eq)
		return to_ba(condition == time_point);
	if (constraint | tau_parser::ctn_greater_equal)
		return is_left ? to_ba(condition >= time_point) : to_ba(time_point >= condition);
	if (constraint | tau_parser::ctn_greater)
		return is_left ? to_ba(condition > time_point) : to_ba(time_point > condition);
	if (constraint | tau_parser::ctn_less_equal)
		return is_left ? to_ba(condition <= time_point) : to_ba(time_point <= condition);
	if (constraint | tau_parser::ctn_less)
		return is_left ? to_ba(condition < time_point) : to_ba(time_point < condition);
	// The above is exhaustive for possible children of constraint
	assert(false);
	return {};
}

template<typename... BAs>
bool is_initial_ctn_phase(const tau<BAs...>& constraint, int_t time_point) {
	const int_t condition = is_non_terminal(tau_parser::num, trim2(constraint)) ?
		size_t_extractor<BAs...>(trim(trim2(constraint))).value() :
		size_t_extractor<BAs...>(trim(constraint)->child[1]->child[0]).value();

	// At this point the equality and inequality constraints should have been converted
	assert(!(constraint | tau_parser::ctn_neq) && !(constraint | tau_parser::ctn_eq));

	if (constraint | tau_parser::ctn_greater_equal)
		return condition >= time_point;
	if (constraint | tau_parser::ctn_greater)
		return condition + 1 >= time_point;
	if (constraint | tau_parser::ctn_less_equal)
		return condition + 1 >= time_point;
	if (constraint | tau_parser::ctn_less)
		return condition >= time_point;

	// The above is exhaustive for possible children of constraints
	assert(false);
	return {};
}

template<typename... BAs>
tau<BAs...> fm_at_time_point(const tau<BAs...>& original_fm,
	auto &io_vars, int_t time_point)
{
	std::map<tau<BAs...>, tau<BAs...>> changes;
	for (size_t i = 0; i < io_vars.size(); ++i) {
		auto new_io_var = transform_io_var(io_vars[i], get_io_name(io_vars[i]),
								time_point);
		changes[io_vars[i]] = new_io_var;
	}
	tau<BAs...> new_fm = replace(original_fm, changes);
	return new_fm;
}

template<typename... BAs>
std::pair<tau<BAs...>, tau<BAs...> > build_initial_step_chi(
	const tau<BAs...>& chi, const tau<BAs...>& st, auto& io_vars,
	int_t time_point, auto& pholder_to_st) {
	std::map<tau<BAs...>, tau<BAs...>> changes;
	for (size_t i = 0; i < io_vars.size(); ++i) {
		auto new_io_var = transform_io_var(io_vars[i], get_io_name(io_vars[i]),
								time_point);
		changes[io_vars[i]] = new_io_var;
	}
	tau<BAs...> c_pholder = build_io_out_const<BAs...>("_pholder", time_point);
	c_pholder = build_wff_eq(wrap(tau_parser::bf, c_pholder));
	pholder_to_st.emplace(c_pholder, replace(st, changes));
	tau<BAs...> new_fm = build_wff_and(replace(chi, changes), c_pholder);
	return make_pair(new_fm, c_pholder);
}

template<typename... BAs>
tau<BAs...> build_step(const tau<BAs...>& original_fm,
	const tau<BAs...>& prev_fm, const auto& io_vars, auto& initials, int_t step_num,
	int_t time_point, tau<BAs...>& cached_fm) {
	// Use build_initial_step otherwise
	assert(step_num > 0);
	std::map<tau<BAs...>, tau<BAs...> > changes;
	for (size_t i = 0; i < io_vars.size(); ++i) {
		auto new_io_var = transform_io_var(io_vars[i], get_io_name(io_vars[i]),
							time_point + step_num);
		changes[io_vars[i]] = new_io_var;
	}

	tau<BAs...> most_inner_step = replace(original_fm, changes);
	auto q_most_inner_step = existentially_quantify_output_streams(most_inner_step, io_vars, time_point + step_num,
	                                                               initials);
	q_most_inner_step = universally_quantify_input_streams(q_most_inner_step, io_vars, time_point + step_num,
	                                                       initials);
	changes = {{cached_fm, build_wff_and(cached_fm, q_most_inner_step)}};
	cached_fm = most_inner_step;
	return replace(prev_fm, changes);
}

template<typename... BAs>
tau<BAs...> build_step_chi(const tau<BAs...>& chi, const tau<BAs...>& st,
	const tau<BAs...>& prev_fm, const auto& io_vars, auto& initials, int_t step_num,
	int_t time_point, auto& cached_fm, auto& pholder_to_st) {
	// Use build_initial_step otherwise
	assert(step_num > 0);
	std::map<tau<BAs...>, tau<BAs...> > changes;
	for (size_t i = 0; i < io_vars.size(); ++i) {
		auto new_io_var = transform_io_var(io_vars[i], get_io_name(io_vars[i]),
							time_point + step_num);
		changes[io_vars[i]] = new_io_var;
	}
	// We need a placeholder symbol in order to substitute during the next step
	tau<BAs...> c_chi = replace(chi, changes);
	tau<BAs...> c_pholder = build_io_out_const<BAs...>("_pholder", time_point + step_num);
	c_pholder = build_wff_eq(wrap(tau_parser::bf, c_pholder));
	tau<BAs...> c_st = replace(st, changes);
	pholder_to_st.emplace(c_pholder, c_st);
	// Quantify formula which is to be added to chi
	auto q_most_inner_step = existentially_quantify_output_streams(
		build_wff_and(c_chi, c_pholder), io_vars, time_point + step_num,
		initials);
	q_most_inner_step = universally_quantify_input_streams(
		q_most_inner_step, io_vars, time_point + step_num, initials);
	// If build_step_chi is used with empty sometimes clause
	if (st == _T<BAs...>) changes = {{cached_fm,  q_most_inner_step}};
	else changes = {{ cached_fm,
			     build_wff_or(cached_fm, q_most_inner_step) }};
	cached_fm = c_pholder;
	return replace(prev_fm, changes);
}

// comparator for sorting constant io variables
inline auto constant_io_comp = [](const auto& v1, const auto& v2) {
	using p = tau_parser;
	// trim the bf of v1 and v2 if present
	auto v1_ = is_non_terminal(p::bf, v1) ? trim(v1) : v1;
	auto v2_ = is_non_terminal(p::bf, v2) ? trim(v2) : v2;
	if (get_io_time_point(v1_) < get_io_time_point(v2_))
		return true;
	if (get_io_time_point(v1_) == get_io_time_point(v2_)) {
		bool v1_in = (v1_ | p::io_var | p::in).has_value();
		bool v2_in = (v2_ | p::io_var | p::in).has_value();

		if (!v1_in && v2_in) return false;
		if (v1_in && !v2_in) return true;
		if (v1_in && v2_in) return get_io_name(v1_) < get_io_name(v2_);
		if (!v1_in && !v2_in) return get_io_name(v1_) < get_io_name(v2_);
			// This covers all cases already
		else return false;
	} else return false;
};

// This method is designed to be called on the output of find_fixpoint_phi/chi
// when the run was started at the earliest well-defined time point
template<typename... BAs>
bool is_run_satisfiable(const tau<BAs...>& fm) {
	using p = tau_parser;
	if (fm == _F<BAs...>) return false;
	if (fm == _T<BAs...>) return true;

	auto free_io_vars = get_free_vars_from_nso(fm);
	std::vector<tau<BAs...> > io_vars = select_top(fm,
				is_child_non_terminal<p::io_var, BAs...>);
	sort(io_vars.begin(), io_vars.end(), constant_io_comp);

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
		if (v | p::io_var | p::in) sat_fm = build_wff_all(v, sat_fm);
		else sat_fm = build_wff_ex(v, sat_fm);
		io_vars.pop_back();
	}

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Formula for sat check";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << sat_fm;

	return is_non_temp_nso_satisfiable(sat_fm);
}

// Assumption is that the provided fm is an unbound continuation
template<typename... BAs>
tau<BAs...> get_uninterpreted_constants_constraints(const tau<BAs...>& fm) {
	using p = tau_parser;
	std::vector<tau<BAs...> > io_vars = select_top(fm,
				is_child_non_terminal<p::io_var, BAs...>);

	// Substitute lookback as current time point
	auto look_back = get_max_shift(io_vars);
	auto uconst_ctns = fm_at_time_point(fm, io_vars, look_back);
	io_vars = select_top(uconst_ctns,
			     is_child_non_terminal<p::io_var, BAs...>);

	// All io_vars in fm have to refer to constant time positions
	assert(all_of(io_vars.begin(), io_vars.end(),
		[](const auto& el){return is_io_initial(el);}));
	sort(io_vars.begin(), io_vars.end(), constant_io_comp);

	auto free_io_vars = get_free_vars_from_nso(uconst_ctns);
	while(!io_vars.empty()) {
		if (!free_io_vars.contains(io_vars.back())) {
			io_vars.pop_back();
			continue;
		}
		auto& v = io_vars.back();
		free_io_vars.erase(v);
		if (v | p::io_var | p::in) uconst_ctns = build_wff_all(v, uconst_ctns);
		else uconst_ctns = build_wff_ex(v, uconst_ctns);
		io_vars.pop_back();
	}
	// Existentially quantify remaining variables
	std::vector<tau<BAs...>> uconsts;
	for (const auto& v : free_io_vars) {
		if (!is_child_non_terminal(p::uninterpreted_constant, v))
			uconst_ctns = build_wff_ex(v, uconst_ctns);
		else uconsts.push_back(v);
	}
	// Eliminate all variables
	uconst_ctns = normalize_non_temp(uconst_ctns);
	// Now add all uninterpreted constants which disappeared during elimination of variables
	// and set them to 0
	auto left_uconsts = select_top(uconst_ctns,
		is_child_non_terminal<p::uninterpreted_constant, BAs...>);
	for (const auto& uc : uconsts) {
		if (std::ranges::find(left_uconsts, uc) == left_uconsts.end())
			uconst_ctns = build_wff_and(uconst_ctns, build_wff_eq(wrap(p::bf, uc)));
	}

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Formula describing constraints on uninterpreted constants";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << uconst_ctns;

	return uconst_ctns;
}

template<typename... BAs>
std::pair<tau<BAs...>, int_t> find_fixpoint_phi(const tau<BAs...>& base_fm, const tau<BAs...>& ctn_initials,
	const auto& io_vars, const auto& initials, const int_t time_point) {
	tau<BAs...> phi_prev = fm_at_time_point(base_fm, io_vars, time_point);
	phi_prev = build_wff_and(ctn_initials, phi_prev);
	int_t step_num = 1;
	tau<BAs...> cache = phi_prev;
	tau<BAs...> phi = build_step(base_fm, phi_prev, io_vars,
		 initials, step_num, time_point, cache);

	BOOST_LOG_TRIVIAL(debug) << "Continuation at step " << step_num;
	BOOST_LOG_TRIVIAL(debug) << "(F) " << phi;

	int_t max_initial_condition = get_max_initial<BAs...>(io_vars);
	int_t lookback = get_max_shift(io_vars);
	// Find fix point once all initial conditions have been passed and
	// the time_point is greater equal the step_num
	while (step_num < std::max(max_initial_condition, lookback)
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
	return make_pair(phi_prev, step_num - 1);
}

template<typename... BAs>
std::pair<tau<BAs...>, int_t> find_fixpoint_chi(const tau<BAs...>& chi_base, const tau<BAs...>& st,
			      const auto& io_vars, const auto& initials,
			      const int_t time_point) {
	std::map<tau<BAs...>, tau<BAs...>> pholder_to_st;
	auto [chi_prev, cache] = build_initial_step_chi(
		chi_base, st, io_vars, time_point, pholder_to_st);

	// int_t max_initial_condition = get_max_initial<BAs...>(io_vars);
	int_t lookback = get_max_shift(io_vars);
	int_t step_num = 1;

	tau<BAs...> chi = build_step_chi(chi_base, st, chi_prev, io_vars,
		 initials, step_num, time_point, cache, pholder_to_st);

	auto chi_replc = replace(chi, pholder_to_st);
	auto chi_prev_replc = replace(chi_prev, pholder_to_st);

	BOOST_LOG_TRIVIAL(debug) << "Continuation at step " << step_num;
	BOOST_LOG_TRIVIAL(debug) << "(F) " << replace(chi, pholder_to_st);

	// Find fix point once all initial conditions have been passed and
	// the lookback is greater equal the step_num
	while (step_num < lookback || !are_nso_equivalent(
		       chi_prev_replc, chi_replc)) {
		chi_prev = chi;
		chi_prev_replc = chi_replc;
		++step_num;

		chi = build_step_chi(chi_base, st, chi_prev, io_vars,
			initials, step_num, time_point, cache, pholder_to_st);
		chi_replc = replace(chi, pholder_to_st);

        BOOST_LOG_TRIVIAL(debug) << "Continuation at step " << step_num;
		BOOST_LOG_TRIVIAL(debug) << "(F) " << chi_replc;
	}
	BOOST_LOG_TRIVIAL(debug) << "Unbounded continuation of Tau formula "
		"reached fixpoint after " << step_num - 1 << " steps";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << normalize_non_temp(chi_prev_replc);
	return {chi_prev_replc, step_num - 1};
}

template<typename... BAs>
tau<BAs...> transform_back_non_initials(const tau<BAs...>& fm, const int_t highest_init_cond) {
	// Find lookback
	auto current_io_vars = select_top(fm,
			is_child_non_terminal<tau_parser::io_var, BAs...>);
	int_t lookback = get_lookback_after_normalization(current_io_vars);

	std::map<tau<BAs...>,tau<BAs...>> changes;
	// Get time positions which are higher than highest_init_cond and transform back to
	// time variable depending on t
	for (const auto& io_var : current_io_vars) {
		int_t time_point = size_t_extractor<BAs...>(
			trim2(trim2(io_var)->child[1])).value();
		if (time_point <= highest_init_cond)
			continue;

		tau<BAs...> transformed_var;
		if (time_point - lookback != 0) {
			if (io_var | tau_parser::io_var | tau_parser::in)
				transformed_var = build_io_in_shift<BAs...>(
					get_io_name(io_var), "t",
					abs(time_point - lookback));
			else
				transformed_var = build_io_out_shift<BAs...>(
					get_io_name(io_var), "t",
					abs(time_point - lookback));
		}
		else {
			if (io_var | tau_parser::io_var | tau_parser::in)
				transformed_var = build_io_in<BAs...>(
					get_io_name(io_var), "t");
			else
				transformed_var = build_io_out<BAs...>(
					get_io_name(io_var), "t");
		}

		changes.emplace(io_var, transformed_var);
	}
	return replace(fm, changes);
}

template<typename... BAs>
tau<BAs...> build_flag_on_lookback (const std::string& name, const std::string& var,
									const int_t lookback) {
	if (lookback >= 2)
		return wrap(tau_parser::bf,
				build_io_out_shift<BAs...>(name, var, lookback - 1));
	else return wrap(tau_parser::bf, build_io_out<BAs...>(name, var));
}

template<typename... BAs>
tau<BAs...> build_prev_flag_on_lookback (const std::string& name, const std::string& var, const int_t lookback) {
	if (lookback >= 2)
		return wrap(tau_parser::bf,
				build_io_out_shift<BAs...>(name, var, lookback));
	else return wrap(tau_parser::bf,
				build_io_out_shift<BAs...>(name, var, 1));
}

template<typename... BAs>
tau<BAs...> transform_ctn_to_streams(const tau<BAs...>& fm, tau<BAs...>& flag_initials,
					tau<BAs...>& flag_rules,
					const int_t lookback,
					const bool reset_ctn_id) {
	using p = tau_parser;
	auto to_eq_1 = [](const auto& n) {
		return build_wff_eq(build_bf_neg(n));
	};
	flag_initials = _T<BAs...>;
	std::map<tau<BAs...>, tau<BAs...>> changes;
	// transform constraints to their respective output streams and add required conditions
	// We make the variable static so that we can transform different parts of the formula independently
	static size_t ctn_id = 0;
	if (reset_ctn_id) ctn_id = 0;
	for (const auto& ctn : select_top(fm, is_non_terminal<p::constraint, BAs...>))
	{
		std::string ctnvar = make_string(tau_node_terminal_extractor<BAs...>,
			find_top(ctn, is_non_terminal<p::ctnvar, BAs...>).value());
		std::stringstream ss; ss << "_f" << ctn_id++;
		auto flag_iovar = build_io_out<BAs...>(ss.str(), ctnvar);

		// Take lookback of formula into account for constructing rule
		auto flag_rule1 = build_prev_flag_on_lookback<BAs...>(ss.str(), ctnvar, lookback);
		auto flag_rule2 = build_flag_on_lookback<BAs...>(ss.str(), ctnvar, lookback);
		changes[ctn] = trim(to_eq_1( wrap(tau_parser::bf, flag_iovar)));
		if (ctn | p::ctn_greater || ctn | p::ctn_greater_equal) {
			// Add flag rule _fk[lookback] != 0 -> _fk[lookback-1] = 1
			auto flag_rule = build_wff_or(
				build_wff_eq(flag_rule1), to_eq_1(flag_rule2));
			// Conjunct flag rule with formula
			flag_rules = build_wff_and(flag_rules, flag_rule);
		} else {
			// Flag is of type less or less_equal
			// Add flag rule _fk[lookback] != 1 -> _fk[lookback-1] = 0
			auto flag_rule = build_wff_or(
				to_eq_1(flag_rule1), build_wff_eq(flag_rule2));
			// Conjunct flag rule with formula
			flag_rules = build_wff_and(flag_rules, flag_rule);
		}

		// Add initial conditions for flag
		int_t t = 0;
		while (is_initial_ctn_phase(ctn, t)) {
			tau<BAs...> flag_init_cond = transform_io_var(
				flag_iovar, get_io_name(flag_iovar), t);
			flag_init_cond = wrap(tau_parser::bf, flag_init_cond);
			flag_initials = build_wff_and(build_wff_eq(build_bf_xor(
			flag_init_cond, calculate_ctn(ctn, t))), flag_initials);
			++t;
		}
	}
	if (!changes.empty()) return replace(fm, changes);
	return fm;
}

// We assume that the formula has run through the normalizer before
// and is a single always statement
template<typename... BAs>
tau<BAs...> always_to_unbounded_continuation(tau<BAs...> fm,
	const bool output = false)
{
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin always_to_unbounded_continuation";
	BOOST_LOG_TRIVIAL(debug) << "Start fm for always_to_unbound: " << fm << "\n";

	using p = tau_parser;
	assert(has_no_boolean_combs_of_models(fm));
	if (is_child_non_terminal(p::wff_always, fm)) fm = trim2(fm);

	// Preparation to transform flags to output streams
	std::vector<tau<BAs...> > io_vars = select_top(fm,
				is_child_non_terminal<p::io_var, BAs...>);
	int_t lookback = get_max_shift(io_vars);
	tau<BAs...> flag_initials = _T<BAs...>, flag_rules = _T<BAs...>;
	auto transformed_fm = transform_ctn_to_streams(fm, flag_initials, flag_rules, lookback, true);
	if (lookback == 0 && fm != transformed_fm) {
		io_vars = select_top(transformed_fm, is_child_non_terminal<p::io_var, BAs...>);
		fm = shift_io_vars_in_fm(transformed_fm, io_vars, 1);
	} else fm = transformed_fm;
	fm = build_wff_and(fm, flag_rules);
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Removed flags";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << build_wff_and(fm, flag_initials);

	io_vars = select_top(build_wff_and(fm, flag_initials),
			is_child_non_terminal<p::io_var, BAs...>);

	// Save positions of io_variables which are initial conditions
	std::set<std::pair<std::string, int_t>> initials;
	for (int_t i = 0; i < (int_t) io_vars.size(); ++i)
            if (is_io_initial(io_vars[i]))
                initials.emplace(get_io_name(io_vars[i]),
                    get_io_time_point(io_vars[i]));

	// Calculate unbound continuation of fm
	int_t time_point = get_max_shift(io_vars);
	int_t point_after_inits = get_max_initial<BAs...>(io_vars) + 1;
	auto [ubd_ctn, steps] = find_fixpoint_phi(
		fm, flag_initials, io_vars, initials,
		time_point + point_after_inits);

	ubd_ctn = normalize_non_temp(ubd_ctn);
	ubd_ctn = transform_back_non_initials(ubd_ctn, point_after_inits - 1);
	// Run phi_inf until all initial conditions are taken into account
	io_vars = select_top(ubd_ctn, is_child_non_terminal<p::io_var, BAs...>);
	tau<BAs...> run = _T<BAs...>;
	for (int_t t = time_point; t < point_after_inits + time_point; ++t) {
		auto current_step = fm_at_time_point(ubd_ctn, io_vars, t);
		run = build_wff_and(run, current_step);
		// Check if run is still sat
		run = normalize_non_temp(run);
		if (!is_run_satisfiable(run)) {
			print_fixpoint_info(
				"Temporal normalization of always specification reached fixpoint after "
				+ std::to_string(steps) +
				" steps, yielding the result: ",
				tau_to_str(_F<BAs...>), output);
			return _F<BAs...>;
		}
	}
	auto res = normalize_non_temp(build_wff_and(ubd_ctn, run));
	// The following is std::cout because it should always be printed
	print_fixpoint_info(
		"Temporal normalization of always specification reached fixpoint after "
		+ std::to_string(steps) + " steps, yielding the result: ",
		tau_to_str(is_child_non_terminal(p::wff_always, res)
				? trim2(res)
				: res), output);
	BOOST_LOG_TRIVIAL(debug) << "(I) -- End always_to_unbounded_continuation";
	return res;
}

// Creates a guard for input stream variables
template<typename... BAs>
tau<BAs...> create_guard(const auto& io_vars, int_t& guard_id) {
	using p = tau_parser;
	auto create_uconst_guard = [&guard_id]() {
		return build_bf_uniter_const<BAs...>("", "_g" + std::to_string(guard_id++));
	};
	auto create_stream_guard = [&guard_id](const int_t shift, const bool init = false) {
		assert((init || shift > 0));
		if (init == true) {
			return wrap(
				p::bf, build_io_out_const<BAs...>(
					"_g" + std::to_string(guard_id++), shift));
		}
		else return wrap(
			p::bf, build_io_out_shift<BAs...>(
				"_g" + std::to_string(guard_id++), "t", shift));
	};
	auto add_guard = [](auto& guard, const auto& new_guard, const auto io_var) {
		auto cdn = build_wff_eq(build_bf_xor(wrap(p::bf, io_var), new_guard));
		guard = build_wff_and(guard, cdn);
	};
	tau<BAs...> guard = _T<BAs...>;
	for (const auto& io_var : io_vars) {
		// Check if input stream variable
		if (io_var | p::io_var | p::in) {
			if (is_io_initial(io_var)) {
				int_t t = get_io_time_point(io_var);
				tau<BAs...> g;
				if (t == 0) g = create_uconst_guard();
				else g = create_stream_guard(t-1, true);
				add_guard(guard, g, io_var);
			} else {
				int_t t = get_io_var_shift(io_var);
				add_guard(guard, create_stream_guard(t+1), io_var);
			}
		}
	}
	return guard;
}

// Given a formula, wrap its input variables in guards to mimic
// existential quantification for input
template<typename... BAs>
tau<BAs...> make_inputs_guarded(const tau<BAs...>& fm, const bool conj, int_t& guard_id) {
	using p = tau_parser;
	auto guarded_step_0 = [](const auto& ori_fm, const auto& io_vars, int_t& guard_id) {
		int_t t = get_max_shift(io_vars);
		auto fm_at_0 = fm_at_time_point(ori_fm, io_vars, t);
		auto new_io_vars = select_top(
			fm_at_0, is_child_non_terminal<p::io_var, BAs...>);
		auto guard = create_guard<BAs...>(new_io_vars, guard_id);
		return build_wff_imply(guard, fm_at_0);
	};

	auto current_io_vars = select_top(fm,
			is_child_non_terminal<p::io_var, BAs...>);
	// Create guarded step 0
	int_t guard_at_0_id = guard_id;
	auto guarded_fm_at_0 = guarded_step_0(
		fm, current_io_vars, guard_at_0_id);
	// Create rest of guard
	auto guard = create_guard<BAs...>(current_io_vars, guard_id);
	if (conj)
		return build_wff_and(guarded_fm_at_0,
			build_wff_imply(guard, fm));
	else return build_wff_or(guarded_fm_at_0, build_wff_imply(guard, fm));
}

// Creates a guard for the input streams in io_vars
// template<typename... BAs>
// tau<BAs...> create_guard(const auto& io_vars, int_t& guard_id) {
// 	using p = tau_parser;
// 	tau<BAs...> guard = _T<BAs...>;
// 	for (const auto& io_var : io_vars) {
// 		// Check if input stream variable
// 		if (io_var | p::io_var | p::in) {
// 			auto var_guard = wrap(
// 				p::bf, build_io_out<BAs...>("_g" + std::to_string(guard_id), "t"));
// 			auto cdn = build_wff_eq(build_bf_xor(wrap(p::bf, io_var), var_guard));
// 			guard = build_wff_and(guard, cdn);
// 			// Increase the seed by 1
// 			++guard_id;
// 		}
// 	}
// 	return guard;
// }

template<typename... BAs>
tau<BAs...> transform_neg_sometimes_to_guarded_always(const tau<BAs...>& fm, int_t& guard_id) {
	using p = tau_parser;
	// Find negated always parts
	auto is_neg_st = [](const auto& n) {
		if (is_child_non_terminal(p::wff_neg, n) &&
			is_child_non_terminal(p::wff_sometimes, trim2(n)))
			return true;
		else return false;
	};
	auto neg_st_fms = select_top(fm, is_neg_st);
	// Do nothing if there is no negated sometimes
	if (neg_st_fms.empty()) return fm;
	// Replace negated sometimes parts in fm with T
	std::map<tau<BAs...>, tau<BAs...>> changes;
	for (const auto& el : neg_st_fms)
		changes.emplace(el, _T<BAs...>);
	auto new_fm = replace(fm, changes);
	// Put !(always phi) into the form !phi
	for (auto& neg_st_fm : neg_st_fms) {
		auto tmp = trim2(trim2(neg_st_fm));
		neg_st_fm = push_negation_in(build_wff_neg(tmp));
	}
	// make statements guarded
	for (size_t i = 0; i < neg_st_fms.size(); ++i)
		neg_st_fms[i] = make_inputs_guarded(
					neg_st_fms[i], true, guard_id);

	// conjunct with initial always part
	auto _aw = find_top(fm, is_child_non_terminal<p::wff_always, BAs...>);
	auto aw = _aw.has_value() ? trim2(_aw.value()) : _T<BAs...>;
	for (auto& neg_st_fm : neg_st_fms) {
		aw = always_conjunction(aw, neg_st_fm);
	}
	if (_aw.has_value()) {
		changes.clear();
		return replace(new_fm, changes);
		changes.emplace(_aw.value(), build_wff_always(aw));
	}
	std::cout << "trans_neg_st/aw: " << aw << "\n";
	return build_wff_and(build_wff_always(aw), new_fm);
}

template<typename... BAs>
void transform_sometimes_to_eventual (std::vector<tau<BAs...>>& st_fms,
						bool reset_ctn_stream,
						const int_t max_lookback,
						const size_t flag_num_shift,
						auto& ev_assm, auto& ev_collection
						) {
	using p = tau_parser;
	for (size_t n = 0; n < st_fms.size(); ++n) {
		auto st_io_vars = select_top(st_fms[n], is_child_non_terminal<p::io_var, BAs...>);
		int_t st_lookback = get_max_shift(st_io_vars);

		// Transform constant time constraints to io var in sometimes statement
		tau<BAs...> ctn_initials = _T<BAs...>, ctn_assm = _T<BAs...>;
		st_fms[n] = transform_ctn_to_streams(st_fms[n], ctn_initials, ctn_assm, st_lookback, reset_ctn_stream);
		st_io_vars = select_top(st_fms[n], is_child_non_terminal<p::io_var, BAs...>);

		std::stringstream ss; ss << "_e" << n + flag_num_shift;
		// Build the eventual var flags based on the maximal lookback
		auto eNt_without_lookback = wrap(p::bf, build_io_out<BAs...>(ss.str(), "t"));
		auto eNt = build_flag_on_lookback<BAs...>(ss.str(), "t", max_lookback);
		auto eNt_prev = build_prev_flag_on_lookback<BAs...>(ss.str(), "t", max_lookback);

		auto eN0_is_not_zero = build_wff_neq(wrap(p::bf,
			build_io_out_const<BAs...>(ss.str(), 0)));
		auto eNt_is_zero      = build_wff_eq(eNt);
		auto eNt_is_not_zero  = build_wff_neq(eNt);
		auto eNt_prev_is_zero = build_wff_eq(eNt_prev);
		auto eNt_prev_is_not_zero	= build_wff_neq(eNt_prev);
		// transform `!always psi` to:
		// (_eN[t-1] != 0 && _eN[t] == 0) -> psi (N is nth `!always`)
		auto shifted_st = max_lookback == 0 ?
			shift_io_vars_in_fm(trim2(st_fms[n]), st_io_vars, 1) :
			shift_io_vars_in_fm(trim2(st_fms[n]), st_io_vars,
				max_lookback - st_lookback);

		ev_assm = build_wff_and(ev_assm, build_wff_imply(
				build_wff_and(eNt_prev_is_not_zero, eNt_is_zero),
				shifted_st));

		// for each _eN add conjunction
		// 	(_eN[0] != 0 && (_eN[t-1] = 0 -> _eN[t] = 0))
		ev_assm = build_wff_and( ev_assm,
				build_wff_and(eN0_is_not_zero,
				build_wff_imply(eNt_prev_is_zero, eNt_is_zero)));

		// Add flag assumptions from constant time constraints
		ev_assm = build_wff_and(ev_assm, build_wff_and(ctn_initials, ctn_assm));

		ev_collection = build_bf_or(ev_collection, eNt_without_lookback);
	}
}

template<typename... BAs>
void transform_neg_aw_to_eventual (std::vector<tau<BAs...>>& neg_aw_fms,
						bool reset_ctn_stream,
						const int_t max_lookback,
						int_t& guard_id,
						auto& ev_assm, auto& ev_collection) {
	using p = tau_parser;
	for (size_t n = 0; n < neg_aw_fms.size(); ++n) {
		auto neg_aw_io_vars = select_top(neg_aw_fms[n], is_child_non_terminal<p::io_var, BAs...>);
		int_t neg_aw_lookback = get_max_shift(neg_aw_io_vars);

		// Transform constant time constraints to io var in sometimes statement
		tau<BAs...> ctn_initials = _T<BAs...>, ctn_assm = _T<BAs...>;
		neg_aw_fms[n] = transform_ctn_to_streams(neg_aw_fms[n], ctn_initials, ctn_assm, neg_aw_lookback, reset_ctn_stream);
		neg_aw_io_vars = select_top(neg_aw_fms[n], is_child_non_terminal<p::io_var, BAs...>);

		std::stringstream ss; ss << "_e" << n;
		// Build the eventual var flags based on the maximal lookback
		auto eNt_without_lookback = wrap(p::bf, build_io_out<BAs...>(ss.str(), "t"));
		auto eNt = build_flag_on_lookback<BAs...>(ss.str(), "t", max_lookback);
		auto eNt_prev = build_prev_flag_on_lookback<BAs...>(ss.str(), "t", max_lookback);

		auto eN0_is_not_zero = build_wff_neq(wrap(p::bf,
			build_io_out_const<BAs...>(ss.str(), 0)));
		auto eNt_is_zero      = build_wff_eq(eNt);
		auto eNt_is_not_zero  = build_wff_neq(eNt);
		auto eNt_prev_is_zero = build_wff_eq(eNt_prev);
		auto eNt_prev_is_not_zero	= build_wff_neq(eNt_prev);
		// transform `!always psi` to:
		// (_eN[t-1] != 0 && _eN[t] == 0) -> psi (N is nth `!always`)
		auto shifted_neg_aw = max_lookback == 0 ?
			shift_io_vars_in_fm(neg_aw_fms[n], neg_aw_io_vars, 1) :
			shift_io_vars_in_fm(neg_aw_fms[n], neg_aw_io_vars,
				max_lookback - neg_aw_lookback);

		// Guard statement to express that
		// "if the inputs equal the guard, the Tau formula
		// under !always is implied"
		// This mimics an existential quantifier capturing the inputs but at the same
		// time the inputs are not quantified
		shifted_neg_aw = make_inputs_guarded(shifted_neg_aw, false, guard_id);

		ev_assm = build_wff_and(ev_assm, build_wff_imply(
				build_wff_and(eNt_prev_is_not_zero, eNt_is_zero),
				shifted_neg_aw));

		// for each _eN add conjunction
		// 	(_eN[0] != 0 && (_eN[t-1] = 0 -> _eN[t] = 0))
		ev_assm = build_wff_and( ev_assm,
				build_wff_and(eN0_is_not_zero,
				build_wff_imply(eNt_prev_is_zero, eNt_is_zero)));

		// Add flag assumptions from constant time constraints
		ev_assm = build_wff_and(ev_assm, build_wff_and(ctn_initials, ctn_assm));

		ev_collection = build_bf_or(ev_collection, eNt_without_lookback);
	}
}

// Assumes single normalized Tau DNF clause
template<typename... BAs>
tau<BAs...> transform_to_eventual_variables(const tau<BAs...>& fm,
					int_t& guard_id,
					const bool reset_ctn_stream) {
	using p = tau_parser;
	// Collect all statements to be transformed to eventual variables
	auto is_neg_aw = [](const auto& n) {
		if (is_child_non_terminal(p::wff_neg, n) &&
			is_child_non_terminal(p::wff_always, trim2(n)))
			return true;
		else return false;
	};
	auto neg_aw_fms = select_top(fm, is_neg_aw);
	// Put !(always phi) into the form !phi and add input guards
	for (auto& neg_aw_fm : neg_aw_fms) {
		auto tmp = trim2(trim2(neg_aw_fm));
		auto neg_tmp = push_negation_in(build_wff_neg(tmp));
		neg_aw_fm = make_inputs_guarded(neg_tmp, false, guard_id);
	}
	auto aw_fm = find_top_until(
		fm, is_child_non_terminal<p::wff_always, BAs...>,
		is_child_non_terminal<p::wff_neg, BAs...>);

	auto st_fms = select_top_until(
		fm, is_child_non_terminal<p::wff_sometimes, BAs...>,
		is_child_non_terminal<p::wff_neg, BAs...>);
	// Add formula from neg_aw_fms to st_fms, since after guarding inputs
	// they are ordinary sometimes statements
	std::ranges::move(neg_aw_fms, std::back_inserter(st_fms));
	int_t max_lookback = get_max_shift(
		select_top(fm, is_child_non_terminal<p::io_var, BAs...>));

	int_t aw_lookback = 0;
	std::vector<tau<BAs...>> aw_io_vars;
	if (aw_fm.has_value()) {
		aw_io_vars = select_top(aw_fm.value(),
			is_child_non_terminal<p::io_var, BAs...>);
		aw_lookback = get_max_shift(aw_io_vars);
	}

	BOOST_LOG_TRIVIAL(trace) << "(T) -- transforming eventual variables";
	BOOST_LOG_TRIVIAL(trace) << fm;
	tau<BAs...> ev_assm = _T<BAs...>;
	tau<BAs...> ev_collection = _0<BAs...>;
	// Transform negated always parts
	// transform_neg_aw_to_eventual(neg_aw_fms, reset_ctn_stream, max_lookback,
	// 				guard_id, ev_assm, ev_collection);
	// Transform sometimes parts
	transform_sometimes_to_eventual(st_fms, reset_ctn_stream, max_lookback,
					neg_aw_fms.size(), ev_assm, ev_collection);
	// Build formula out of eventual variable transformations
	auto res = _T<BAs...>;
	// Check if always part is present
	if (aw_fm.has_value()) {
		auto aw = trim2(aw_fm.value());
		// Adjust lookback
		aw = shift_io_vars_in_fm(aw, aw_io_vars, max_lookback - aw_lookback);
		// Conjunct former always part and eventual variable assumptions
		res = build_wff_always(
			build_wff_and(aw, ev_assm));
		// Conjunct new sometimes part if present
		if (ev_assm != _T<BAs...>) {
			// if the lookback of all parts is 0, we need to shift the always part
			// to account for the modified lookback due to the eventual variables
			if (max_lookback == 0) res = shift_io_vars_in_fm(res, aw_io_vars, 1);
			res = build_wff_and(
				res, build_wff_sometimes(
					build_wff_eq(ev_collection)));
		}
	} else {
		// Conjunct new sometimes part if present
		if (ev_assm != _T<BAs...>) {
			res = build_wff_always(ev_assm);
			res = build_wff_and(
				res, build_wff_sometimes(
					build_wff_eq(ev_collection)));
		} else return fm;
	}

	BOOST_LOG_TRIVIAL(trace) << "(T) -- transformed eventual variables";
	BOOST_LOG_TRIVIAL(trace) << res;
	std::cout << "trans even vars, res: " << res << "\n";
	return res;
}

template<typename... BAs>
tau<BAs...> add_st_ctn (const tau<BAs...>& st, const int_t timepoint, const int_t steps) {
	tau<BAs...> st_ctn = _T<BAs...>;
	auto io_vars = select_top(
		st, is_child_non_terminal<tau_parser::io_var, BAs...>);
	for (int_t s = 0; s <= steps; ++s) {
		std::map<tau<BAs...>, tau<BAs...> > changes;
		for (size_t i = 0; i < io_vars.size(); ++i) {
			auto new_io_var = transform_io_var(
				io_vars[i], get_io_name(io_vars[i]), timepoint + s);
			changes[io_vars[i]] = new_io_var;
		}
		if (s == steps) st_ctn = build_wff_and(
					    st_ctn, replace(st, changes));
		else st_ctn = build_wff_and(
			     st_ctn, build_wff_neg(replace(st, changes)));
	}
	return st_ctn;
}

// Assumes that ubd_aw_continuation is the result of computing the unbounded always continuation of
// the always part of the output of "transform_to_eventual_variables" and
// that ev_var_flags is the sometimes part of "transform_to_eventual_variables"
template<typename... BAs>
tau<BAs...> to_unbounded_continuation(const tau<BAs...>& ubd_aw_continuation,
				      const tau<BAs...>& ev_var_flags,
				      const auto& original_aw,
				      const bool output = false) {
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin to_unbounded_continuation";

	using p = tau_parser;
	std::cout << "ubd_aw_continuation: " << ubd_aw_continuation << "\n";
	std::cout << "ev_var_flags: " << ev_var_flags << "\n";
	assert(has_no_boolean_combs_of_models(ubd_aw_continuation));
	assert(is_child_non_terminal(p::wff_sometimes, ev_var_flags));

	auto st_flags = trim2(ev_var_flags);
	auto aw = is_child_non_terminal(p::wff_always, ubd_aw_continuation)
			  ? trim2(ubd_aw_continuation)
			  : ubd_aw_continuation;
	auto original_aw_continuation = original_aw != nullptr
						? (is_child_non_terminal(
							   p::wff_always, original_aw)
							   ? trim2(original_aw)
							   : original_aw)
						: _T<BAs...>;

	std::vector<tau<BAs...>> io_vars = select_top(aw,
			is_child_non_terminal<p::io_var, BAs...>);
	std::vector<tau<BAs...>> st_io_vars = select_top(st_flags,
		is_child_non_terminal<p::io_var, BAs...>);

	int_t time_point = get_max_shift(io_vars);

	// There must not be a constant time constraint at this point
	assert(!find_top(aw, is_non_terminal<p::constraint, BAs...>));

	int_t point_after_inits = get_max_initial<BAs...>(io_vars) + 1;
	// Shift flags in order to match lookback of always part
	st_flags = shift_io_vars_in_fm(st_flags, st_io_vars, time_point - 1);
	st_io_vars = select_top(st_flags, is_child_non_terminal<p::io_var, BAs...>);

	// Check if flag can be raised up to the highest initial condition + 1
	tau<BAs...> run;
	int_t flag_boundary = std::max(point_after_inits + time_point, 2*time_point + 1) + 1;
	for (int_t i = time_point; i <= flag_boundary; ++i) {
		auto current_aw = fm_at_time_point(aw, io_vars, i);
		if (run) run = build_wff_and(run, current_aw);
		else run = current_aw;
		auto current_flag = fm_at_time_point(st_flags, st_io_vars, i);

		std::cout << "run: " << build_wff_and(run, current_flag) << "\n";
		auto normed_run = normalize_non_temp(build_wff_and(run, current_flag));
		std::cout << "normed_run: " << normed_run << "\n";
		if (is_run_satisfiable(normed_run)) {
			BOOST_LOG_TRIVIAL(debug) << "Flag raised at time point " << i - time_point;
			BOOST_LOG_TRIVIAL(debug) << "(F) " << normed_run;
			auto res = build_wff_and(normed_run, original_aw_continuation);
			print_fixpoint_info(
				"Temporal normalization of Tau specification did not rely on fixpoint finding, yielding the result: ",
				tau_to_str(res), output);
			return res;
		}
		// Since the flag could not be raised in this step, we can add the assumption
		// that it will never be raised at this timepoint
		run = normalize_non_temp(build_wff_and(run, build_wff_neg(current_flag)));
	}
	// Since flag could not be raised in the initial segment, we now check if it
	// can be raised at all. To this end we calculate chi_inf

	// Save positions of io_variables which are initial conditions
	std::set<std::pair<std::string, int_t>> initials;

	for (int_t i = 0; i < (int_t) io_vars.size(); ++i)
		if (is_io_initial(io_vars[i]))
			initials.emplace(get_io_name(io_vars[i]),
				get_io_time_point(io_vars[i]));

	// Calculate fix point and get unbound continuation
	BOOST_LOG_TRIVIAL(trace) << "chi base: " << build_wff_and(aw, st_flags);

	// Find fixpoint of chi after highest initial condition
	auto [chi_inf, steps ] = find_fixpoint_chi(aw, st_flags, io_vars,
		initials, time_point + point_after_inits);
	chi_inf = normalize_non_temp(chi_inf);

	// BOOST_LOG_TRIVIAL(trace) << "Fixpoint chi after normalize: " << chi_inf;
	if (chi_inf == _F<BAs...>) {
		print_fixpoint_info(
			"Temporal normalization of Tau specification reached fixpoint after "
			+ std::to_string(steps) +
			" steps, yielding the result: ",
			tau_to_str(_F<BAs...>), output);
		return _F<BAs...>;
	}

	chi_inf = transform_back_non_initials(chi_inf, point_after_inits - 1);
	io_vars = select_top(chi_inf, is_child_non_terminal<p::io_var, BAs...>);
	auto chi_inf_anchored = fm_at_time_point(chi_inf, io_vars, point_after_inits);

	auto sat_check = is_run_satisfiable(
		build_wff_and(run, chi_inf_anchored));
	BOOST_LOG_TRIVIAL(trace) << "Fm to check sat:";
	BOOST_LOG_TRIVIAL(trace) << "(F) " << sat_check;
	if (sat_check == _F<BAs...>) {
		print_fixpoint_info(
			"Temporal normalization of Tau specification reached fixpoint after "
			+ std::to_string(steps) +
			" steps, yielding the result: ",
			tau_to_str(_F<BAs...>), output);
		return _F<BAs...>;
	}
	// Here we know that the formula is satisfiable at some point
	// Since the initial segment is already checked we continue from there
	for (int_t i = flag_boundary + 1; true; ++i) {
		auto current_aw = fm_at_time_point(aw, io_vars, i);
		run = build_wff_and(run, current_aw);
		auto current_flag = fm_at_time_point(st_flags, st_io_vars, i);

		auto normed_run = normalize_non_temp(build_wff_and(run, current_flag));
		// The formula is guaranteed to have be sat at some point
		// Therefore, the loop will exit eventually
		if (is_run_satisfiable(normed_run)) {
			BOOST_LOG_TRIVIAL(debug) << "Flag raised at time point " << i - time_point;
			BOOST_LOG_TRIVIAL(debug) << "(F) " << normed_run;
			auto res = build_wff_and(normed_run, original_aw_continuation);
			print_fixpoint_info(
				"Temporal normalization of Tau specification reached fixpoint after "
				+ std::to_string(steps) +
				" steps, yielding the result: ",
				tau_to_str(res), output);
			return res;
		}
		// Since the flag could not be raised in this step, we can add the assumption
		// that it will never be raised at this timepoint
		run = normalize_non_temp(build_wff_and(run, build_wff_neg(current_flag)));
	}
}

// Assumes a single normalized Tau DNF clause
template<typename... BAs>
tau<BAs...> transform_to_execution(const tau<BAs...>& fm, const bool output = false) {
	assert(get_dnf_wff_clauses(fm).size() == 1);
#ifdef TAU_CACHE
	static std::map<tau<BAs...>, tau<BAs...>> cache;
	if (auto it = cache.find(fm); it != cache.end())
		return it->second;
#endif
	using p = tau_parser;
	auto elim_aw = [](const auto& f) {
		return is_child_non_terminal(p::wff_always, f) ? trim2(f) : f;
	};
	BOOST_LOG_TRIVIAL(debug) << "(I) Start transform_to_execution";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << fm;

	// Convert !sometimes parts to guarded always statements
	int_t guard_id = 0;
	auto trans_fm = transform_neg_sometimes_to_guarded_always(fm, guard_id);
	std::cout << "trans_fm: " << trans_fm << "\n";
	// ptree(std::cout, trans_fm);
	// std::cout << "\n";

	auto aw_fm = find_top_until(trans_fm, is_child_non_terminal<p::wff_always, BAs...>,
			is_child_non_terminal<p::wff_neg, BAs...>);
	tau<BAs...> ev_t;
	tau<BAs...> ubd_aw_fm;
	if (aw_fm.has_value()) {
		std::cout << "aw_fm: " << aw_fm.value() << "\n";
		// If there is an always part, replace it with its unbound continuation
		ubd_aw_fm = always_to_unbounded_continuation(aw_fm.value(), output);
		std::map<tau<BAs...>, tau<BAs...> > changes = {
			{aw_fm.value(), build_wff_always(ubd_aw_fm)}
		};
		auto ubd_fm = replace(trans_fm, changes);
		ev_t = transform_to_eventual_variables(ubd_fm, guard_id, false);
		// Check if there is a sometimes present
		if (ev_t == ubd_fm) {
#ifdef TAU_CACHE
			cache.emplace(elim_aw(ubd_fm), elim_aw(ubd_fm));
			return cache.emplace(fm, elim_aw(ubd_fm)).first->second;
#endif
			return elim_aw(ubd_fm);
		}
	} else {
		ev_t = transform_to_eventual_variables(fm, guard_id, true);
		// Check if there is a sometimes present
		if (ev_t == fm) {
#ifdef TAU_CACHE
			return cache.emplace(fm, elim_aw(fm)).first->second;
#endif
			return elim_aw(fm);
		}
	}
	auto aw_after_ev = find_top_until(ev_t, is_child_non_terminal<p::wff_always, BAs...>,
				is_child_non_terminal<p::wff_neg, BAs...>);
	if (!aw_after_ev.has_value()) {
#ifdef TAU_CACHE
		return cache.emplace(fm, elim_aw(fm)).first->second;
#endif
		return elim_aw(fm);
	}
	auto st = select_top(ev_t, is_child_non_terminal<p::wff_sometimes, BAs...>);
	assert(st.size() < 2);

	tau<BAs...> res;
	if (aw_after_ev.value() != _F<BAs...> && !st.empty())
		res = normalize_non_temp(to_unbounded_continuation(
			aw_after_ev.value(), st[0], ubd_aw_fm, output));
	else res = aw_after_ev.value();
	BOOST_LOG_TRIVIAL(debug) << "(I) End transform_to_execution";
	res = elim_aw(res);
#ifdef TAU_CACHE
	cache.emplace(res, res);
	return cache.emplace(fm, res).first->second;
#endif
	return res;
}

// Assumes that fm has been normalized
template<typename... BAs>
bool is_tau_formula_sat (const tau<BAs...>& normalized_fm, const bool output = false) {
	BOOST_LOG_TRIVIAL(debug) << "(I) Start is_tau_formula_sat";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << normalized_fm;

	auto clauses = get_leaves(normalized_fm, tau_parser::wff_or,
				  tau_parser::wff);
	// Convert each disjunct to unbounded continuation
	for (auto& clause: clauses) {
		if (transform_to_execution(clause, output) != _F<BAs...>) {
			BOOST_LOG_TRIVIAL(debug) << "(I) End is_tau_formula_sat";
			return true;
		}
	}
	BOOST_LOG_TRIVIAL(debug) << "(I) End is_tau_formula_sat";
	return false;
}

// Check for temporal formulas if f1 implies f2
template<typename... BAs>
bool is_tau_impl (const tau<BAs...>& f1, const tau<BAs...>& f2) {
	auto f1_norm = normalizer_step(f1);
	auto f2_norm = normalizer_step(f2);
	auto imp_check = normalize_temporal_simp(
		(build_wff_neg(build_wff_imply(f1_norm, f2_norm))));
	auto clauses = get_dnf_wff_clauses(imp_check);
	// Now check that each disjunct is not satisfiable
	for (const auto& c : clauses) {
		auto ctn = transform_to_execution(c);
		if (ctn != _F<BAs...>)
			return false;
	}
	return true;
}

// The formulas need to be closed
template<typename... BAs>
bool are_tau_equivalent (const tau<BAs...>& f1, const tau<BAs...>& f2) {
	// Negate equivalence for unsat check
	auto f1_norm = normalizer_step(f1);
	auto f2_norm = normalizer_step(f2);
	auto equiv_check = normalize_temporal_simp(
		build_wff_neg(build_wff_equiv(f1_norm, f2_norm)));
	auto clauses = get_dnf_wff_clauses(equiv_check);
	// Now check that each disjunct is not satisfiable
	for (const auto& c : clauses) {
		auto ctn = transform_to_execution(c);
		if (ctn != _F<BAs...>)
			return false;
	}
	return true;
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

#endif // __SATISFIABILITY_H__