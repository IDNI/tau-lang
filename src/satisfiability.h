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

namespace idni::tau {

inline static bool use_debug_output_in_sat = false;
inline void print_fixpoint_info(const std::string& info) {
	if (!use_debug_output_in_sat) BOOST_LOG_TRIVIAL(info) << info;
	else std::cerr << info << "\n";
}

template<typename... BAs>
nso<BAs...> build_io_out (const std::string& name, const std::string& var) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::out_var_name, name);
	auto offset = wrap<BAs...>(p::offset, wrap<BAs...>(p::variable, var));
	return wrap(p::variable, wrap(p::io_var, wrap(p::out, { var_name, offset })));
}

template<typename... BAs>
nso<BAs...> build_io_in (const std::string& name, const std::string& var) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::in_var_name, name);
	auto offset = wrap<BAs...>(p::offset, wrap<BAs...>(p::variable, var));
	return wrap(p::variable, wrap(p::io_var, wrap(p::in, { var_name, offset })));
}

template<typename... BAs>
nso<BAs...> build_io_out_const (const std::string& name, const int_t pos) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::out_var_name, name);
	auto offset = wrap<BAs...>(p::offset, build_num<BAs...>(pos));
	return wrap(p::variable, wrap(p::io_var, wrap(p::out, { var_name, offset })));
}

template<typename... BAs>
nso<BAs...> build_io_in_const (const std::string& name, const int_t pos) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::in_var_name, name);
	auto offset = wrap<BAs...>(p::offset, build_num<BAs...>(pos));
	return wrap(p::variable, wrap(p::io_var, wrap(p::in, { var_name, offset })));
}

template<typename... BAs>
nso<BAs...> build_io_out_shift (const std::string& name, const std::string& var, const int_t shift) {
	using p = tau_parser;
	auto var_name = wrap<BAs...>(p::out_var_name, name);
	auto shift_node = wrap<BAs...>(p::shift, {wrap<BAs...>(p::variable, var), build_num<BAs...>(shift)});
	auto offset = wrap<BAs...>(p::offset, shift_node);
	return wrap(p::variable, wrap(p::io_var, wrap(p::out, { var_name, offset })));
}

template<typename... BAs>
nso<BAs...> build_io_in_shift (const std::string& name, const std::string& var, const int_t shift) {
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
bool has_temporary_io_var (const nso<BAs...>& fm) {
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
nso<BAs...> transform_io_var(const nso<BAs...>& io_var, const std::string& io_var_name, int_t time_point) {
	// Check if io_var has constant time point
	if (is_io_initial(io_var))
		return io_var;
	auto shift = get_io_var_shift(io_var);
	if (io_var | tau_parser::io_var | tau_parser::in)
		return build_io_in_const<BAs...>(io_var_name, time_point - shift);
	else return build_io_out_const<BAs...>(io_var_name, time_point - shift);
}

template<typename... BAs>
nso<BAs...> existentially_quantify_output_streams(nso<BAs...> fm, const auto& io_vars,
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
	std::set<nso<BAs...>> cache;
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
	std::set<nso<BAs...>> cache;
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
nso<BAs...> calculate_ctn(const nso<BAs...>& constraint, int_t time_point) {
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
bool is_initial_ctn_phase(const nso<BAs...>& constraint, int_t time_point) {
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
nso<BAs...> fm_at_time_point(const nso<BAs...>& original_fm,
	auto &io_vars, int_t time_point)
{
	std::map<nso<BAs...>, nso<BAs...>> changes;
	for (size_t i = 0; i < io_vars.size(); ++i) {
		auto new_io_var = transform_io_var(io_vars[i], get_io_name(io_vars[i]),
								time_point);
		changes[io_vars[i]] = new_io_var;
	}
	nso<BAs...> new_fm = replace(original_fm, changes);
	return new_fm;
}

template<typename... BAs>
std::pair<nso<BAs...>, nso<BAs...> > build_initial_step_chi(
	const nso<BAs...>& chi, const nso<BAs...>& st, auto& io_vars,
	int_t time_point, auto& pholder_to_st) {
	std::map<nso<BAs...>, nso<BAs...>> changes;
	for (size_t i = 0; i < io_vars.size(); ++i) {
		auto new_io_var = transform_io_var(io_vars[i], get_io_name(io_vars[i]),
								time_point);
		changes[io_vars[i]] = new_io_var;
	}
	nso<BAs...> c_pholder = build_io_out_const<BAs...>("_pholder", time_point);
	c_pholder = build_wff_eq(wrap(tau_parser::bf, c_pholder));
	pholder_to_st.emplace(c_pholder, replace(st, changes));
	nso<BAs...> new_fm = build_wff_and(replace(chi, changes), c_pholder);
	return make_pair(new_fm, c_pholder);
}

template<typename... BAs>
nso<BAs...> build_step(const nso<BAs...>& original_fm,
	const nso<BAs...>& prev_fm, const auto& io_vars, auto& initials, int_t step_num,
	int_t time_point, nso<BAs...>& cached_fm) {
	// Use build_initial_step otherwise
	assert(step_num > 0);
	std::map<nso<BAs...>, nso<BAs...> > changes;
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
nso<BAs...> build_step_chi(const nso<BAs...>& chi, const nso<BAs...>& st,
	const nso<BAs...>& prev_fm, const auto& io_vars, auto& initials, int_t step_num,
	int_t time_point, auto& cached_fm, auto& pholder_to_st) {
	// Use build_initial_step otherwise
	assert(step_num > 0);
	std::map<nso<BAs...>, nso<BAs...> > changes;
	for (size_t i = 0; i < io_vars.size(); ++i) {
		auto new_io_var = transform_io_var(io_vars[i], get_io_name(io_vars[i]),
							time_point + step_num);
		changes[io_vars[i]] = new_io_var;
	}
	// We need a placeholder symbol in order to substitute during the next step
	nso<BAs...> c_chi = replace(chi, changes);
	nso<BAs...> c_pholder = build_io_out_const<BAs...>("_pholder", time_point + step_num);
	c_pholder = build_wff_eq(wrap(tau_parser::bf, c_pholder));
	nso<BAs...> c_st = replace(st, changes);
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

// This method is designed to be called on the output of find_fixpoint_phi/chi
// when the run was started at the earliest well-defined time point
template<typename... BAs>
bool is_run_satisfiable (const nso<BAs...>& fm) {
	using p = tau_parser;
	if (fm == _F<BAs...>) return false;
	if (fm == _T<BAs...>) return true;

	auto free_io_vars = get_free_vars_from_nso(fm);
	std::vector<nso<BAs...> > io_vars = select_top(fm,
				is_child_non_terminal<p::io_var, BAs...>);
	auto initial_comp = [](const auto& v1, const auto& v2) {
		if (get_io_time_point(v1) < get_io_time_point(v2))
			return true;
		if (get_io_time_point(v1) == get_io_time_point(v2)) {
			bool v1_in = (v1 | p::io_var | p::in).has_value();
			bool v2_in = (v2 | p::io_var | p::in).has_value();

			if (!v1_in && v2_in) return false;
			if (v1_in && !v2_in) return true;
			if (v1_in && v2_in) return get_io_name(v1) < get_io_name(v2);
			if (!v1_in && !v2_in) return get_io_name(v1) < get_io_name(v2);
			// This covers all cases already
			else return false;
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
nso<BAs...> get_uninterpreted_constants_constraints (const nso<BAs...>& fm) {
	using p = tau_parser;
	std::vector<nso<BAs...> > io_vars = select_top(fm,
				is_child_non_terminal<p::io_var, BAs...>);

	// Substitute lookback as current time point
	auto look_back = get_max_shift(io_vars);
	auto uconst_ctns = fm_at_time_point(fm, io_vars, look_back);
	io_vars = select_top(uconst_ctns,
			     is_child_non_terminal<p::io_var, BAs...>);

	auto initial_comp = [](const auto& v1, const auto& v2) {
		if (get_io_time_point(v1) < get_io_time_point(v2))
			return true;
		if (get_io_time_point(v1) == get_io_time_point(v2)) {
			bool v1_in = (v1 | p::io_var | p::in).has_value();
			bool v2_in = (v2 | p::io_var | p::in).has_value();

			if (!v1_in && v2_in) return false;
			if (v1_in && !v2_in) return true;
			if (v1_in && v2_in) return get_io_name(v1) < get_io_name(v2);
			if (!v1_in && !v2_in) return get_io_name(v1) < get_io_name(v2);
			// This covers all cases already
			else return false;
		} else return false;
	};
	// All io_vars in fm have to refer to constant time positions
	assert(all_of(io_vars.begin(), io_vars.end(),
		[](const auto& el){return is_io_initial(el);}));
	sort(io_vars.begin(), io_vars.end(), initial_comp);

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
	for (const auto& v : free_io_vars) {
		if (!is_child_non_terminal(tau_parser::uninterpreted_constant, v))
			uconst_ctns = build_wff_ex(v, uconst_ctns);
	}
	// Eliminate all variables
	uconst_ctns = normalize_non_temp(uconst_ctns);

	BOOST_LOG_TRIVIAL(debug) << "(I) -- Formula describing constraints on uninterpreted constants";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << uconst_ctns;

	return uconst_ctns;
}

template<typename... BAs>
std::pair<nso<BAs...>, int_t> find_fixpoint_phi(const nso<BAs...>& base_fm, const nso<BAs...>& ctn_initials,
	const auto& io_vars, const auto& initials, const int_t time_point) {
	nso<BAs...> phi_prev = fm_at_time_point(base_fm, io_vars, time_point);
	phi_prev = build_wff_and(ctn_initials, phi_prev);
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
std::pair<nso<BAs...>, int_t> find_fixpoint_chi(const nso<BAs...>& chi_base, const nso<BAs...>& st,
			      const auto& io_vars, const auto& initials,
			      const int_t time_point) {
	std::map<nso<BAs...>, nso<BAs...>> pholder_to_st;
	auto [chi_prev, cache] = build_initial_step_chi(
		chi_base, st, io_vars, time_point, pholder_to_st);

	// int_t max_initial_condition = get_max_initial<BAs...>(io_vars);
	int_t lookback = get_max_shift(io_vars);
	int_t step_num = 1;

	nso<BAs...> chi = build_step_chi(chi_base, st, chi_prev, io_vars,
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
nso<BAs...> transform_back_non_initials(const nso<BAs...>& fm, const int_t highest_init_cond) {
	// Find lookback
	auto current_io_vars = select_top(fm,
			is_child_non_terminal<tau_parser::io_var, BAs...>);
	int_t lookback = get_lookback_after_normalization(current_io_vars);

	std::map<nso<BAs...>,nso<BAs...>> changes;
	// Get time positions which are higher than highest_init_cond and transform back to
	// time variable depending on t
	for (const auto& io_var : current_io_vars) {
		int_t time_point = size_t_extractor<BAs...>(
			trim2(trim2(io_var)->child[1])).value();
		if (time_point <= highest_init_cond)
			continue;

		nso<BAs...> transformed_var;
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
nso<BAs...> build_flag_on_lookback (const std::string& name, const std::string& var,
									const int_t lookback) {
	if (lookback >= 2)
		return wrap(tau_parser::bf,
				build_io_out_shift<BAs...>(name, var, lookback - 1));
	else return wrap(tau_parser::bf, build_io_out<BAs...>(name, var));
}

template<typename... BAs>
nso<BAs...> build_prev_flag_on_lookback (const std::string& name, const std::string& var, const int_t lookback) {
	if (lookback >= 2)
		return wrap(tau_parser::bf,
				build_io_out_shift<BAs...>(name, var, lookback));
	else return wrap(tau_parser::bf,
				build_io_out_shift<BAs...>(name, var, 1));
}

template<typename... BAs>
nso<BAs...> transform_ctn_to_streams(nso<BAs...> fm, nso<BAs...>& flag_initials, const int_t lookback, bool reset_ctn_id = false) {
	using p = tau_parser;
	auto to_eq_1 = [](const auto& n) {
		return build_wff_eq(build_bf_neg(n));
	};
	flag_initials = _T<BAs...>;
	std::map<nso<BAs...>, nso<BAs...>> changes;
	// transform constraints to their respective output streams and add required conditions
	// We make the variable static so that we can transform different parts of the formula independently
	static size_t ctn_id = 0;
	if (reset_ctn_id) ctn_id = 0;
	for (const auto& ctn : select_top(fm, is_non_terminal<p::constraint, BAs...>))
	{
		std::string ctnvar = make_string(tau_node_terminal_extractor<BAs...>,
			find_top(ctn, is_non_terminal<p::ctnvar, BAs...>).value());
		std::stringstream ss; ss << "_f" << ctn_id++;

		// Take lookback of formula into account for constructing rule
		auto flag_rule1 = build_prev_flag_on_lookback<BAs...>(ss.str(), ctnvar, lookback);
		auto flag_rule2 = build_flag_on_lookback<BAs...>(ss.str(), ctnvar, lookback);
		changes[ctn] = trim(to_eq_1(flag_rule1));
		if (ctn | p::ctn_greater || ctn | p::ctn_greater_equal) {
			// Add flag rule _fk[lookback] != 0 -> _fk[lookback-1] = 1
			auto flag_rule = build_wff_or(
				build_wff_eq(flag_rule1), to_eq_1(flag_rule2));
			// Conjunct flag rule with formula
			fm = build_wff_and(fm, flag_rule);
		} else {
			// Flag is of type less or less_equal
			// Add flag rule _fk[lookback] != 1 -> _fk[lookback-1] = 0
			auto flag_rule = build_wff_or(
				to_eq_1(flag_rule1), build_wff_eq(flag_rule2));
			// Conjunct flag rule with formula
			fm = build_wff_and(fm, flag_rule);
		}

		// Add initial conditions for flag
		auto flag_iovar = build_io_out<BAs...>(ss.str(), ctnvar);
		int_t t = 0;
		while (is_initial_ctn_phase(ctn, t)) {
			nso<BAs...> flag_init_cond =
			transform_io_var(flag_iovar, get_io_name(flag_iovar), t);
			flag_init_cond = wrap(tau_parser::bf, flag_init_cond);
			flag_initials = build_wff_and(build_wff_eq(build_bf_xor(
			flag_init_cond, calculate_ctn(ctn, t))), flag_initials);
			++t;
		}
	}
	if (!changes.empty()) fm = replace(fm, changes);
	return fm;
}

// We assume that the formula has run through the normalizer before
// and is a single always statement
template<typename... BAs>
nso<BAs...> always_to_unbounded_continuation(nso<BAs...> fm)
{
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin always_to_unbounded_continuation";
	BOOST_LOG_TRIVIAL(debug) << "Start fm for always_to_unbound: " << fm << "\n";

	using p = tau_parser;
	assert(has_no_boolean_combs_of_models(fm));
	if (is_child_non_terminal(p::wff_always, fm)) fm = trim2(fm);

	// Preparation to transform flags to output streams
	std::vector<nso<BAs...> > io_vars = select_top(fm,
				is_child_non_terminal<p::io_var, BAs...>);
	int_t lookback = get_max_shift(io_vars);
	nso<BAs...> flag_initials;
	auto transformed_fm = transform_ctn_to_streams(fm, flag_initials, lookback, true);
	if (lookback == 0 && fm != transformed_fm) {
		fm = shift_io_vars_in_fm(transformed_fm, io_vars, 1);
	} else fm = transformed_fm;

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
	nso<BAs...> run = _T<BAs...>;
	for (int_t t = time_point; t < point_after_inits + time_point; ++t) {
		auto current_step = fm_at_time_point(ubd_ctn, io_vars, t);
		run = build_wff_and(run, current_step);
		// Check if run is still sat
		run = normalize_non_temp(run);
		if (!is_run_satisfiable(run)) {
			print_fixpoint_info(
				"Temporal normalization of always specification reached fixpoint after "
				+ std::to_string(steps) +
				" steps, yielding the result: ");
			print_fixpoint_info(tau_to_str(_F<BAs...>));
			return _F<BAs...>;
		}
	}
	auto res = normalize_non_temp(build_wff_and(ubd_ctn, run));
	// The following is std::cout because it should always be printed
	print_fixpoint_info("Temporal normalization of always specification reached fixpoint after " + std::to_string(steps) + " steps, yielding the result: ");
	print_fixpoint_info(tau_to_str(is_child_non_terminal(p::wff_always, res)
					       ? trim2(res)
					       : res));
	BOOST_LOG_TRIVIAL(debug) << "(I) -- End always_to_unbounded_continuation";
	return res;
}

// Creates a guard using the names of the input streams in uninterpreted constants
template<typename... BAs>
nso<BAs...> create_guard(const auto& io_vars, const int_t number) {
	using p = tau_parser;
	nso<BAs...> guard = _T<BAs...>;
	for (const auto& io_var : io_vars) {
		// Check if input stream variable
		if (io_var | p::io_var | p::in) {
			// Give name of io_var and make it non-user definable with "_"
			auto uiter_const = build_bf_uniter_const<BAs...>("_" + tau_to_str(io_var), std::to_string(number));
			auto cdn = build_wff_eq(build_bf_xor(wrap(p::bf, io_var), uiter_const));
			guard = build_wff_and(guard, cdn);
		}
	}
	return guard;
}

// Assumes single normalized Tau DNF clause
template<typename... BAs>
nso<BAs...> transform_to_eventual_variables(const nso<BAs...>& fm) {
	using p = tau_parser;
	auto smt_fms = select_top(fm, is_child_non_terminal<p::wff_sometimes, BAs...>);
	if (smt_fms.empty()) return fm;
	auto aw_fm = find_top(fm, is_child_non_terminal<p::wff_always, BAs...>);

	int_t max_lookback = get_max_shift(
		select_top(fm, is_child_non_terminal<p::io_var, BAs...>));

	int_t aw_lookback = 0;
	std::vector<nso<BAs...>> aw_io_vars;
	if (aw_fm.has_value()) {
		aw_io_vars = select_top(aw_fm.value(),
			is_child_non_terminal<p::io_var, BAs...>);
		aw_lookback = get_max_shift(aw_io_vars);
	}

	BOOST_LOG_TRIVIAL(trace) << "(T) -- transforming eventual variables";
	BOOST_LOG_TRIVIAL(trace) << fm;
	nso<BAs...> ev_assm = _T<BAs...>;
	nso<BAs...> ev_collection = _0<BAs...>;
	for (size_t n = 0; n < smt_fms.size(); ++n) {
		auto st_io_vars = select_top(smt_fms[n], is_child_non_terminal<p::io_var, BAs...>);
		int_t st_lookback = get_max_shift(st_io_vars);

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
		// transform `sometimes psi` to:
		// (_eN[t-1] != 0 && _eN[t] == 0) -> psi (N is nth `sometimes`)
		auto shifted_sometimes = max_lookback == 0 ?
			shift_io_vars_in_fm(trim2(smt_fms[n]), st_io_vars, 1) :
			shift_io_vars_in_fm(trim2(smt_fms[n]), st_io_vars,
				max_lookback - st_lookback);

		// Guard statement using uninterpreted constants to express that
		// "if the inputs equal the uninterpreted constants, the Tau formula
		// under sometimes is implied"
		// This mimics an existential quantifier capturing the inputs but at the same
		// time the inputs are not quantified
		st_io_vars = select_top(shifted_sometimes, is_child_non_terminal<p::io_var, BAs...>);
		auto guard = create_guard<BAs...>(st_io_vars, n);
		shifted_sometimes = build_wff_imply(guard, shifted_sometimes);

		ev_assm = build_wff_and(ev_assm, build_wff_imply(
				build_wff_and(eNt_prev_is_not_zero, eNt_is_zero),
				shifted_sometimes));

		// for each _eN add conjunction
		// 	(_eN[0] != 0 && (_eN[t-1] = 0 -> _eN[t] = 0))
		ev_assm = build_wff_and( ev_assm,
				build_wff_and(eN0_is_not_zero,
				build_wff_imply(eNt_prev_is_zero, eNt_is_zero)));

		ev_collection = build_bf_or(ev_collection, eNt_without_lookback);
	}
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
	return res;
}

template<typename... BAs>
nso<BAs...> add_st_ctn (const nso<BAs...>& st, const int_t timepoint, const int_t steps) {
	nso<BAs...> st_ctn = _T<BAs...>;
	auto io_vars = select_top(
		st, is_child_non_terminal<tau_parser::io_var, BAs...>);
	for (int_t s = 0; s <= steps; ++s) {
		std::map<nso<BAs...>, nso<BAs...> > changes;
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
nso<BAs...> to_unbounded_continuation(const nso<BAs...>& ubd_aw_continuation,
				      const nso<BAs...>& ev_var_flags,
				      const auto& original_aw,
				      const bool reset_ctn_streams) {
	BOOST_LOG_TRIVIAL(debug) << "(I) -- Begin to_unbounded_continuation";

	using p = tau_parser;
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

	std::vector<nso<BAs...>> io_vars = select_top(aw,
			is_child_non_terminal<p::io_var, BAs...>);
	std::vector<nso<BAs...>> st_io_vars = select_top(st_flags,
		is_child_non_terminal<p::io_var, BAs...>);

	int_t time_point = get_max_shift(io_vars);
	// Check if a constant time constraint is still present
	// Can happen due to constraints in sometimes clause
	if (find_top(aw, is_non_terminal<p::constraint, BAs...>)) {
		// Transform constraint to stream
		nso<BAs...> ctn_initials = _T<BAs...>;
		aw = transform_ctn_to_streams(aw, ctn_initials, time_point, reset_ctn_streams);
		aw = build_wff_and(ctn_initials, aw);
		// The lookback cannot be 0 due to presents of eventual variables
		// Therefore, no adjustment of aw is needed

		// Search again for io_vars after transformation
		io_vars = select_top(aw, is_child_non_terminal<p::io_var, BAs...>);
		time_point = get_max_shift(io_vars);
	}

	int_t point_after_inits = get_max_initial<BAs...>(io_vars) + 1;
	// Shift flags in order to match lookback of always part
	st_flags = shift_io_vars_in_fm(st_flags, st_io_vars, time_point - 1);
	st_io_vars = select_top(st_flags, is_child_non_terminal<p::io_var, BAs...>);

	// Check if flag can be raised up to the highest initial condition + 1
	nso<BAs...> run;
	int_t flag_boundary = std::max(point_after_inits + time_point, 2*time_point + 1) + 1;
	for (int_t i = time_point; i <= flag_boundary; ++i) {
		auto current_aw = fm_at_time_point(aw, io_vars, i);
		if (run) run = build_wff_and(run, current_aw);
		else run = current_aw;
		auto current_flag = fm_at_time_point(st_flags, st_io_vars, i);

		auto normed_run = normalize_non_temp(build_wff_and(run, current_flag));
		if (is_run_satisfiable(normed_run)) {
			BOOST_LOG_TRIVIAL(debug) << "Flag raised at time point " << i - time_point;
			BOOST_LOG_TRIVIAL(debug) << "(F) " << normed_run;
			auto res = build_wff_and(normed_run, original_aw_continuation);
			print_fixpoint_info("Temporal normalization of Tau specification did not rely on fixpoint finding, yielding the result: ");
			print_fixpoint_info(tau_to_str(res));
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
		print_fixpoint_info("Temporal normalization of Tau specification reached fixpoint after " + std::to_string(steps) + " steps, yielding the result: ");
        print_fixpoint_info(tau_to_str(_F<BAs...>));
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
		print_fixpoint_info("Temporal normalization of Tau specification reached fixpoint after " + std::to_string(steps) + " steps, yielding the result: ");
        print_fixpoint_info(tau_to_str(_F<BAs...>));
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
            print_fixpoint_info("Temporal normalization of Tau specification reached fixpoint after " + std::to_string(steps) + " steps, yielding the result: ");
            print_fixpoint_info(tau_to_str(res));
			return res;
		}
		// Since the flag could not be raised in this step, we can add the assumption
		// that it will never be raised at this timepoint
		run = normalize_non_temp(build_wff_and(run, build_wff_neg(current_flag)));
	}
}

// Assumes a single normalized Tau DNF clause
template<typename... BAs>
nso<BAs...> transform_to_execution(nso<BAs...> fm) {
	assert(get_dnf_wff_clauses(fm).size() == 1);
	using p = tau_parser;
	auto elim_aw = [](const auto& f) {
		return is_child_non_terminal(p::wff_always, f) ? trim2(f) : f;
	};
	BOOST_LOG_TRIVIAL(debug) << "(I) Start transform_to_execution";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << fm;
	// We merge all always statements on each clause
	fm = pull_always_out_for_inf(fm);
	BOOST_LOG_TRIVIAL(debug) << "(I) Merged always statements on each clause";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << fm;
	auto aw_fm = find_top(fm, is_child_non_terminal<p::wff_always, BAs...>);
	nso<BAs...> ev_t;
	nso<BAs...> ubd_aw_fm;
	bool reset_ctn_stream = false;
	if (aw_fm.has_value()) {
		// If there is an always part, replace it with its unbound continuation
		ubd_aw_fm = always_to_unbounded_continuation(aw_fm.value());
		std::map<nso<BAs...>, nso<BAs...> > changes = {
			{aw_fm.value(), build_wff_always(ubd_aw_fm)}
		};
		auto ubd_fm = replace(fm, changes);
		ev_t = transform_to_eventual_variables(ubd_fm);
		// Check if there is a sometimes present
		if (ev_t == ubd_fm) return elim_aw(ubd_fm);
	} else {
		reset_ctn_stream = true;
		ev_t = transform_to_eventual_variables(fm);
		// Check if there is a sometimes present
		if (ev_t == fm) return elim_aw(fm);
	}
	auto aw_after_ev = find_top(ev_t, is_child_non_terminal<p::wff_always, BAs...>);
	if (!aw_after_ev.has_value()) return elim_aw(fm);
	auto st = select_top(ev_t, is_child_non_terminal<p::wff_sometimes, BAs...>);
	assert(st.size() < 2);

	nso<BAs...> res;
	if (aw_after_ev.value() != _F<BAs...> && !st.empty())
		res = normalize_non_temp(to_unbounded_continuation(
			aw_after_ev.value(), st[0], ubd_aw_fm, reset_ctn_stream));
	else res = aw_after_ev.value();
	BOOST_LOG_TRIVIAL(debug) << "(I) End transform_to_execution";
	return elim_aw(res);
}

// Assumes that fm has been normalized
template<typename... BAs>
bool is_tau_formula_sat (const nso<BAs...>& normalized_fm) {
	BOOST_LOG_TRIVIAL(debug) << "(I) Start is_tau_formula_sat";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << normalized_fm;

	auto clauses = get_leaves(normalized_fm, tau_parser::wff_or,
				  tau_parser::wff);
	// Convert each disjunct to unbounded continuation
	for (auto& clause: clauses) {
		if (transform_to_execution(clause) != _F<BAs...>) {
			BOOST_LOG_TRIVIAL(debug) << "(I) End is_tau_formula_sat";
			return true;
		}
	}
	BOOST_LOG_TRIVIAL(debug) << "(I) End is_tau_formula_sat";
	return false;
}

// Check for temporal formulas if f1 implies f2
template<typename... BAs>
bool is_tau_impl (const nso<BAs...>& f1, const nso<BAs...>& f2) {
	auto imp_check = normalizer_step(build_wff_imply(f1,f2));
	imp_check = to_dnf2(build_wff_neg(imp_check));
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
bool are_tau_equivalent (const nso<BAs...>& f1, const nso<BAs...>& f2) {
	// Negate equivalence for unsat check
	auto equiv_check = normalizer_step(build_wff_equiv(f1, f2));
	equiv_check = to_dnf2(build_wff_neg(equiv_check));
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

} // namespace idni::tau

#endif // __SATISFIABILITY_H__