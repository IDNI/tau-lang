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
std::string get_io_name (const nso<BAs...>& io_var) {
	std::stringstream ss; ss << trim(trim2(io_var));
	return ss.str();
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
	auto var_name = wrap<BAs...>(p::out_var_name, name);
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
	auto var_name = wrap<BAs...>(p::out_var_name, name);
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
		max_shift = std::max(max_shift, get_io_var_shift(v));
	return max_shift;
}

template<typename... BAs>
int_t get_max_initial(const auto& io_vars) {
	int_t max_init = -1;
	for (const nso<BAs...>& v : io_vars) {
		if (is_io_initial(v)) {
			int_t init = get_io_time_point(v);
			max_init = std::max(max_init, init);
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
	if (io_var_name[0] == 'i')
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
bool is_raw_unbound_continuation_satisfiable (const nso<BAs...>& fm) {
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

template<typename... BAs>
std::pair<nso<BAs...>, int_t> find_fixpoint_phi(const nso<BAs...>& base_fm, const nso<BAs...>& ctn_initials,
	const auto& io_vars, const auto& initials, const int_t time_point, bool raw = false) {
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
	return raw ? make_pair(phi, step_num) : make_pair(phi_prev, step_num - 1);
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
	BOOST_LOG_TRIVIAL(debug) << "(F) " << normalizer_step(chi_prev_replc);
	return {chi_prev_replc, step_num};
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
		if (time_point - lookback != 0)
			transformed_var = build_io_out_shift<BAs...>(get_io_name(io_var), "t", abs(time_point - lookback));
		else transformed_var = build_io_out<BAs...>(get_io_name(io_var), "t");

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
nso<BAs...> transform_ctn_to_streams(nso<BAs...> fm, nso<BAs...>& flag_initials, const int_t lookback) {
	using p = tau_parser;
	auto to_eq_1 = [](const auto& n) {
		return build_wff_eq(build_bf_neg(n));
	};
	flag_initials = _T<BAs...>;
	std::map<nso<BAs...>, nso<BAs...>> changes;
	// transform constraints to their respective output streams and add required conditions
	size_t ctn_id = 0;
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

// Shifts a formula of lookback 0 to previous time step
template<typename... BAs>
nso<BAs...> shift_io_vars_in_fm (const nso<BAs...>& fm, const auto& io_vars, const int_t shift) {
	if (shift <= 0) return fm;
	std::map<nso<BAs...>, nso<BAs...>> changes;
    for (const auto& io_var : io_vars) {
        // Skip initial conditions
        if (is_io_initial(io_var))
            continue;
    	int_t var_shift = get_io_var_shift(io_var);
        changes[io_var] = build_io_out_shift<BAs...>(
            get_io_name(io_var), "t", var_shift + shift);
    }
    return replace(fm, changes);
}

// We assume that the formula has run through the normalizer before
// and is a single always statement
template<typename... BAs>
std::pair<nso<BAs...>, int_t> always_to_unbounded_continuation(nso<BAs...> fm,
	bool sat_check = false)
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
	auto transformed_fm = transform_ctn_to_streams(fm, flag_initials, lookback);
	if (lookback == 0 && fm != transformed_fm)
		fm = shift_io_vars_in_fm(transformed_fm, io_vars, 1);
	else fm = transformed_fm;

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

	// Calculate fix point and get unbound continuation of fm
	int_t time_point = get_max_shift(io_vars);
	auto [phi_inf, step_num] =
		find_fixpoint_phi(fm, flag_initials, io_vars, initials, time_point, true);
	if (sat_check) return make_pair(phi_inf, lookback);
	nso<BAs...> res;
	if (is_raw_unbound_continuation_satisfiable(phi_inf)) {
		int_t point_after_inits = get_max_initial<BAs...>(io_vars) + 1;
		auto [unbound_continuation, _ ] = find_fixpoint_phi(fm, flag_initials, io_vars, initials,
									time_point + point_after_inits);
		unbound_continuation = normalizer_step(unbound_continuation);
		res = transform_back_non_initials(unbound_continuation, point_after_inits - 1);
	} else res = _F<BAs...>;
	BOOST_LOG_TRIVIAL(debug) << "(I) -- End always_to_unbounded_continuation";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << res;
	return make_pair(res, lookback);
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
				      const auto& original_aw) {
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

	int_t point_after_inits = get_max_initial<BAs...>(io_vars) + 1;
	int_t time_point = get_max_shift(io_vars);
	// Shift flags in order to match lookback of always part
	st_flags = shift_io_vars_in_fm(st_flags, st_io_vars, time_point);
	st_io_vars = select_top(st_flags, is_child_non_terminal<p::io_var, BAs...>);

	// Check if flag can be raised up to the highest initial condition + 1
	nso<BAs...> run;
	int_t flag_boundary = std::max(point_after_inits + time_point, 2*time_point + 1) + 1;
	for (int_t i = time_point; i <= flag_boundary; ++i) {
		auto current_aw = fm_at_time_point(aw, io_vars, i);
		if (run) run = build_wff_and(run, current_aw);
		else run = current_aw;
		auto current_flag = fm_at_time_point(st_flags, st_io_vars, i);

		auto normed_run = normalizer_step(build_wff_and(run, current_flag));
		if (is_raw_unbound_continuation_satisfiable(normed_run)) {
			BOOST_LOG_TRIVIAL(debug) << "Flag raised at time point " << i - time_point;
			BOOST_LOG_TRIVIAL(debug) << "(F) " << normed_run;
			return build_wff_and(normed_run, original_aw_continuation);
		}
		// Since the flag could not be raised in this step, we can add the assumption
		// that it will never be raised at this timepoint
		run = normalizer_step(build_wff_and(run, build_wff_neg(current_flag)));
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
	auto [chi_inf, _ ] = find_fixpoint_chi(aw, st_flags, io_vars,
		initials, time_point + point_after_inits);
	chi_inf = normalizer_step(chi_inf);
	BOOST_LOG_TRIVIAL(trace) << "Fixpoint chi after normalize: " << chi_inf;
	if (chi_inf == _F<BAs...>) return _F<BAs...>;

	chi_inf = transform_back_non_initials(chi_inf, point_after_inits - 1);
	io_vars = select_top(chi_inf, is_child_non_terminal<p::io_var, BAs...>);
	auto chi_inf_anchored = fm_at_time_point(chi_inf, io_vars, point_after_inits);

	auto sat_check = is_non_temp_nso_satisfiable(build_wff_and(run, chi_inf_anchored));
	BOOST_LOG_TRIVIAL(trace) << "Fm to check sat:";
	BOOST_LOG_TRIVIAL(trace) << "(F) " << sat_check;
	if (sat_check == _F<BAs...>) return _F<BAs...>;

	// Here we know that the formula is satisfiable at some point
	// Since the initial segment is already checked we continue from there
	for (int_t i = flag_boundary + 1; true; ++i) {
		auto current_aw = fm_at_time_point(aw, io_vars, i);
		run = build_wff_and(run, current_aw);
		auto current_flag = fm_at_time_point(st_flags, st_io_vars, i);

		auto normed_run = normalizer_step(build_wff_and(run, current_flag));
		// The formula is guaranteed to have be sat at some point
		// Therefore, the loop will exit eventually
		if (is_raw_unbound_continuation_satisfiable(normed_run)) {
			BOOST_LOG_TRIVIAL(debug) << "Flag raised at time point " << i - time_point;
			BOOST_LOG_TRIVIAL(debug) << "(F) " << normed_run;
			return build_wff_and(normed_run, original_aw_continuation);
		}
		// Since the flag could not be raised in this step, we can add the assumption
		// that it will never be raised at this timepoint
		run = normalizer_step(build_wff_and(run, build_wff_neg(current_flag)));
	}
}

// Assumes a single normalized Tau DNF clause
template<typename... BAs>
nso<BAs...> transform_to_execution(const nso<BAs...>& fm) {
	BOOST_LOG_TRIVIAL(debug) << "(I) Start transform_to_execution";
	using p = tau_parser;
	auto aw_fm = find_top(fm, is_child_non_terminal<p::wff_always, BAs...>);
	nso<BAs...> ev_t;
	nso<BAs...> ubd_aw_fm;
	if (aw_fm.has_value()) {
		// If there is an always part, replace it with its unbound continuation
		ubd_aw_fm = always_to_unbounded_continuation(aw_fm.value()).first;
		std::map<nso<BAs...>, nso<BAs...> > changes = {
			{aw_fm.value(), build_wff_always(ubd_aw_fm)}
		};
		ev_t = transform_to_eventual_variables(replace(fm, changes));
	} else {
		ev_t = transform_to_eventual_variables(fm);
	}

	auto aw_after_ev = find_top(ev_t, is_child_non_terminal<p::wff_always, BAs...>);
	if (!aw_after_ev.has_value()) return fm;
	auto st = select_top(ev_t, is_child_non_terminal<p::wff_sometimes, BAs...>);
	assert(st.size() < 2);

	nso<BAs...> res;
	if (aw_after_ev.value() != _F<BAs...> && !st.empty())
		res = normalizer_step(to_unbounded_continuation(
				aw_after_ev.value(), st[0], ubd_aw_fm));
	else res = aw_after_ev.value();
	BOOST_LOG_TRIVIAL(debug) << "(I) End transform_to_execution";
	return is_child_non_terminal(p::wff_always, res) ? trim2(res) : res;
}

template<typename... BAs>
bool is_tau_formula_sat (const nso<BAs...>& fm) {
	BOOST_LOG_TRIVIAL(debug) << "(I) Start is_tau_formula_sat";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << fm;

	using p = tau_parser;
	auto ev_t = transform_to_eventual_variables(fm);
	BOOST_LOG_TRIVIAL(trace) << "(I) After eventual variable transformation";
	BOOST_LOG_TRIVIAL(trace) << "(F) " << ev_t;

	auto aw = find_top(ev_t, is_child_non_terminal<p::wff_always, BAs...>);
	if (!aw.has_value()) return is_non_temp_nso_satisfiable(fm);
	auto st = select_top(ev_t, is_child_non_terminal<p::wff_sometimes, BAs...>);
	assert(st.size() < 2);

	auto io_vars = select_top(aw.value(), is_child_non_terminal<p::io_var, BAs...>);
	auto shifted_aw = shift_io_vars_in_fm(aw.value(), io_vars, 1);

	auto [aw_part, lookback] = always_to_unbounded_continuation(shifted_aw, true);
	BOOST_LOG_TRIVIAL(trace) << "(I) After conversion of always part to unbounded continuation";
	BOOST_LOG_TRIVIAL(trace) << "(F) " << aw_part;

	if (st.empty()) return is_raw_unbound_continuation_satisfiable(aw_part);

	auto st_part = push_negation_in(build_wff_neg(trim2(st[0])));

	std::map<nso<BAs...>, nso<BAs...> > num_change = {
		{ wrap<BAs...>(p::variable, "t"), build_num<BAs...>(lookback) }
	};

	nso<BAs...> imp = build_wff_imply<BAs...>(aw_part, replace(st_part, num_change));
	auto vars = get_free_vars_from_nso(imp);
	for(auto& v: vars) {
		imp = build_wff_all<BAs...>(v, imp);
	}
	auto res = normalizer_step(imp) == _F<BAs...>;
	BOOST_LOG_TRIVIAL(debug) << "(I) End is_tau_formula_sat";
	BOOST_LOG_TRIVIAL(debug) << "(F) " << res;
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