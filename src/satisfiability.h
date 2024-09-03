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

// Make all variable names equally long by appending 0's in front of stream number
template<typename... BAs>
vector<string> produce_io_var_names (const vector<nso<BAs...>>& io_vars) {
	vector<string> names;
	for (const nso<BAs...>& io_var : io_vars) {
		stringstream name;
		name << trim(trim2(io_var));
		names.emplace_back(name.str());
	}
	return names;
}

template<typename... BAs>
int_t get_io_var_shift(const nso<BAs...>& io_var) {
	// If there is a shift
	if ((trim2(io_var)->child[1] | tau_parser::shift).has_value())
		return size_t_extractor<BAs...>(trim2(io_var)->child[1]->child[0]->child[1]->child[0]).value();
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
	int_t max_init = 0;
	for (const nso<BAs...>& v : io_vars) {
		if (trim2(v)->child[1] | tau_parser::num) {
			int_t init = size_t_extractor<BAs...>(trim2(trim2(v)->child[1])).value();
			max_init = max(max_init, init);
		}
	}
	return max_init;
}

template<typename... BAs>
nso<BAs...> build_inf_var(const string& name, const int_t time_point) {
	stringstream var; var << name << ":" << time_point;
	return wrap(tau_parser::variable, wrap<BAs...>(tau_parser::charvar, var.str()));
}

template<typename... BAs>
nso<BAs...> transform_io_var(const nso<BAs...>& io_var, const string& io_var_name, int_t time_point) {
	// Check if io_var has constant time point
	if ((trim2(io_var)->child[1] | tau_parser::num).has_value())
		return io_var;
	auto shift = get_io_var_shift(io_var);
	stringstream time;
	time << io_var_name << "[" << (time_point - shift) << "]";
	return build_bf_var<BAs...>(time.str());
}

template<typename... BAs>
nso<BAs...> existentially_quantify_output_streams(nso<BAs...> fm, const auto &io_vars, const auto &io_var_names,
                                                  const auto& initials, int_t time_point) {
	// This map is needed in order to get the minimal shift for streams with same name
	set<int_t> quantifiable_o_vars;
	for (int_t i = 0; i < (int_t)io_vars.size(); ++i) {
		// Skip input streams
		if (io_vars[i] | tau_parser::io_var | tau_parser::in)
			continue;
		// Skip initial conditions
		if (trim2(io_vars[i])->child[1] | tau_parser::num)
			continue;
		quantifiable_o_vars.insert(i);
	}
	for (const auto& pos : quantifiable_o_vars) {
		// Do not quantify time steps which are predefined by initial conditions
		if (initials.contains({io_var_names[pos], time_point}))
			continue;
		stringstream n;
		n <<  io_var_names[pos] << "[" << time_point << "]";
		auto var = build_bf_var<BAs...>(n.str());
		fm = build_wff_ex(var, fm);
	}
	return fm;
}

template<typename... BAs>
nso<BAs...> universally_quantify_input_streams(nso<BAs...> fm, const auto &io_vars, const auto &io_var_names,
                                               const auto& initials, int_t time_point) {
	// This map is needed in order to get the minimal shift for streams with same name
	set<int_t> quantifiable_i_vars;
	for (int_t i = 0; i < (int_t)io_vars.size(); ++i) {
		// Skip output streams
		if (io_vars[i] | tau_parser::io_var | tau_parser::out)
			continue;
		// Skip initial conditions
		if (trim2(io_vars[i])->child[1] | tau_parser::num)
			continue;
		quantifiable_i_vars.insert(i);
	}
	for (const auto& pos : quantifiable_i_vars) {
		// Do not quantify time steps which are predefined by initial conditions
		if (initials.contains({io_var_names[pos], time_point}))
			continue;
		stringstream n;
		n << io_var_names[pos] << "[" << time_point << "]";
		auto var = build_bf_var<BAs...>(n.str());
		fm = build_wff_all(var, fm);
	}
	return fm;
}

template<typename... BAs>
nso<BAs...> build_initial_step(const nso<BAs...>& original_fm, const auto &io_vars, const auto &io_var_names,
                       int_t time_point) {
	map<nso<BAs...>, nso<BAs...>> changes;
	for (size_t i = 0; i < io_vars.size(); ++i) {
		auto new_io_var = transform_io_var(io_vars[i], io_var_names[i], time_point);
		changes[io_vars[i]] = new_io_var;
	}
	return replace(original_fm, changes);
}

template<typename... BAs>
nso<BAs...> build_step(const nso<BAs...>& original_fm, const nso<BAs...>& prev_fm, const auto &io_vars, const auto &io_var_names,
						const auto& initials, int_t step_num, int_t time_point, nso<BAs...>& cached_fm) {
	// Use build_initial_step otherwise
	assert(step_num > 0);
	map<nso<BAs...>, nso<BAs...>> changes;
    for (size_t i = 0; i < io_vars.size(); ++i) {
        auto new_io_var = transform_io_var(io_vars[i], io_var_names[i], time_point + step_num);
        changes[io_vars[i]] = new_io_var;
    }
	nso<BAs...> most_inner_step = replace(original_fm, changes);
	auto q_most_inner_step = existentially_quantify_output_streams(most_inner_step, io_vars, io_var_names, initials, time_point + step_num);
	q_most_inner_step = universally_quantify_input_streams(q_most_inner_step, io_vars, io_var_names, initials, time_point + step_num);
	changes = {{cached_fm, build_wff_and(cached_fm, q_most_inner_step)}};
	cached_fm = most_inner_step;
	return replace(prev_fm, changes);
}

template<typename... BAs>
nso<BAs...> transform_initials(const nso<BAs...>& fm, const auto& io_vars, const auto& io_var_names) {
	map<nso<BAs...>,nso<BAs...>> changes;
	for (size_t i = 0; i < io_vars.size(); ++i) {
		// If io_var is initial, add to changes
		if (trim2(io_vars[i])->child[1] | tau_parser::num) {
			int_t time_point = size_t_extractor<BAs...>(trim2(trim2(io_vars[i])->child[1])).value();
			changes.emplace(io_vars[i], build_inf_var<BAs...>(io_var_names[i], time_point));
		}
	}
	return replace(fm, changes);
}

// We assume that the formula has run through the normalizer before
// TODO: Flag resolution
template<typename... BAs>
nso<BAs...> always_to_unbounded_continuation (nso<BAs...> fm, bool enable_output=false) {
	assert(has_no_boolean_combs_of_models(fm));
	if (is_child_non_terminal(tau_parser::wff_always, fm))
		fm = trim2(fm);
	vector<nso<BAs...>> io_vars = select_top(fm, is_child_non_terminal<tau_parser::io_var, BAs...>);
	auto new_io_var_names = produce_io_var_names(io_vars);
	// Save positions of io_variables which are initial conditions
	set<pair<string, int_t>> initials;
	for (int_t i = 0; i < (int_t)io_vars.size(); ++i)
		if (trim2(io_vars[i])->child[1] | tau_parser::num)
			initials.emplace(new_io_var_names[i], size_t_extractor<BAs...>(trim2(trim2(io_vars[i])->child[1])).value());

	int_t time_point = get_max_shift(io_vars);
	nso<BAs...> prev_unbounded_fm = build_initial_step(fm, io_vars, new_io_var_names, time_point);
	int_t step_num = 1;
	nso<BAs...> cache = prev_unbounded_fm;
	nso<BAs...> unbounded_fm =
		build_step(fm, prev_unbounded_fm, io_vars, new_io_var_names, initials, step_num, time_point, cache);

    // cout << "Continuation at step " << step_num << "\n";
    // cout << unbounded_fm << "\n";

	int_t max_initial_condition = get_max_initial<BAs...>(io_vars);
	if (time_point + 1 >= max_initial_condition) max_initial_condition = 1;
	while(step_num < max_initial_condition || !are_nso_equivalent(prev_unbounded_fm, unbounded_fm)) {
		prev_unbounded_fm = unbounded_fm;
		++step_num;

		unbounded_fm =
			build_step(fm, prev_unbounded_fm, io_vars, new_io_var_names, initials, step_num, time_point, cache);
		// cout << "Continuation at step " << step_num << "\n";
		// cout << unbounded_fm << "\n";
	}
	if (enable_output)
		cout << "Unbounded continuation of Tau formula reached fixpoint after " << step_num - 1 << " steps.\n";
	return transform_initials(prev_unbounded_fm, io_vars, new_io_var_names);
}

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