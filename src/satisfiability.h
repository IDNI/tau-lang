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

#include <iostream>

#include "tau.h"

using namespace std;
using namespace idni::tau;

namespace idni::tau {



template <typename... BAs>
rr<nso<BAs...>> replace_captures_by_shift(rr<nso<BAs...>>& form, int step) {
	std::map<nso<BAs...>, nso<BAs...>> changes;
	for(auto& n: select_all(form.main, is_non_terminal<tau_parser::shift, BAs...>)) {
		auto digits = make_node<tau_sym<BAs...>>(step, {});
		auto num = make_node<tau_sym<BAs...>>(tau_parser::num, {digits});
		if (auto c = n | tau_parser::capture; c.has_value()) {
			changes[c.value()] = num;
		} else if (auto c = n | tau_parser::shift | tau_parser::capture; c.has_value()) {
			changes[c.value()] = num;
		}
	}
	auto nmain = replace<nso<BAs...>>(form.main, changes);
	return { form.rec_relations, nmain };
}

template <typename... BAs>
rr<nso<BAs...>> apply_rec_relations_by_shift(rr<nso<BAs...>>& form) {
	// TODO (LOW) exit if no rec. relations
	std::map<nso<BAs...>, nso<BAs...>> changes;

	auto nmain = form.main
		| repeat_all<step<BAs...>, BAs...>(step<BAs...>(form.rec_relations));

	for (const auto& shift: select_top(nmain, is_non_terminal<tau_parser::shift, BAs...>)) {
		int off = 0;
		std::optional<nso<BAs...>> current(shift);
		while (current.has_value()) {
			off += get<size_t>(((current.value() | tau_parser::num).value())->value);
			current = current.value() | tau_parser::shift;
		}

		auto digits = make_node<tau_sym<BAs...>>(off, {});
		auto num = make_node<tau_sym<BAs...>>(tau_source_sym(tau_parser::num), {digits});
		auto nshift = make_node<tau_sym<BAs...>>(tau_source_sym(tau_parser::shift), {
			shift | tau_parser::capture | optional_value_extractor<nso<BAs...>>,
			shift | tau_parser::minus | optional_value_extractor<nso<BAs...>>,
			num});

		changes[shift] = nshift;
	}

	auto nnmain = replace<nso<BAs...>>(nmain, changes);
	return { form.rec_relations, nnmain };
}


template <typename... BAs>
rr<nso<BAs...>> prepare_main_for_step(rr<nso<BAs...>>& form, int step) {
	auto nform = replace_captures_by_shift<BAs...>(form, step);
	return apply_rec_relations_by_shift<BAs...>(nform);
}


template<typename... BAs>
void get_clauses(const gssotc<BAs...>& n, std::vector<gssotc<BAs...>>& clauses) {
	if (auto check = n | tau_parser::tau_or; !check.has_value() && is_non_terminal(tau_parser::tau, n)) clauses.push_back(n);
	else for (auto& c: n->child)
		if (is_non_terminal(tau_parser::tau_or, c)) get_clauses(c ,clauses);
}

template<typename... BAs>
std::vector<gssotc<BAs...>> get_clauses(const gssotc<BAs...>& n) {
	std::vector<gssotc<BAs...>> clauses;
	get_clauses(n, clauses);

	DBG(std::cout << "(I) clauses: " ;)
	DBG(for (auto& c: clauses) std::cout << c << ", ";)
	DBG(std::cout << std::endl;)

	return clauses;
}

template<typename... BAs>
using extracted_bindings = std::map<std::variant<BAs...>, std::string>;

template<typename... BAs>
std::string clause_to_string(const gssotc<BAs...>& clause,
		extracted_bindings<tau_ba<BAs...>, BAs...>& extracted_bindings,
		size_t binding_seed = 0) {
	std::basic_stringstream<char> str;

	auto visitor = overloaded(
		[&str] (const tau_source_sym& l) {
			if (!l.nt()) str << l.t(); },
		[&str, &extracted_bindings, &binding_seed] (const std::variant<tau_ba<BAs...>, BAs...>& bae) {
			if (extracted_bindings.contains(bae)) str << extracted_bindings[bae];
			else {
				auto binding = "binding" + std::to_string(binding_seed++);
				extracted_bindings[bae] = binding;
				str << binding;
			}},
		[&str] (const size_t& n) { str << n; });

	auto print = [&str, &visitor] (const auto& e) {
		std::visit(visitor, e->value);
		return e;
	};
	using print_t = decltype(print);

	post_order_tree_traverser<
			print_t,
			all_t<gssotc<BAs...>>,
			gssotc<BAs...>>(
		print, all<gssotc<BAs...>>)(clause);

	return str.str();
}

template<typename... BAs>
void get_positive_and_negative_literals(const gssotc<BAs...> collapsed,
		std::optional<gssotc<BAs...>>& positive, std::vector<gssotc<BAs...>>& negatives) {
	auto is_negative = [] (const auto& n) {
		auto check = n | tau_parser::tau | tau_parser::tau_neg | tau_parser::wff;
		return check.has_value();
	};
	auto is_positive = [] (const auto& n) {
		auto check = n | tau_parser::tau | tau_parser::wff;
		return !check.has_value();
	};

	negatives = select_top(collapsed, is_negative);
	positive = find_top(collapsed, is_positive);

	#ifdef DEBUG
	if (positive.has_value()) std::cout << "(I) positive: " << positive.value() << std::endl;
	if (!negatives.empty()) {
		std::cout << "(I) negatives: ";
		for (auto& n: negatives) std::cout << n << ", ";
		std::cout << std::endl;
	}
	#endif // DEBUG
}

template<typename... BAs>
std::pair<std::optional<gssotc<BAs...>>, std::vector<gssotc<BAs...>>> get_positive_and_negative_literals(
		const gssotc<BAs...> collapsed) {
	std::optional<gssotc<BAs...>> positive;
	std::vector<gssotc<BAs...>> negatives;
	get_positive_and_negative_literals(collapsed, positive, negatives);
	return {positive, negatives};
}

template<typename...BAs>
struct tau_spec_vars {

	void add(const gssotc<BAs...>& io) {
		auto pos = io
			| only_child_extractor<tau_ba<BAs...>, BAs...>
			| tau_parser::var_pos
			| only_child_extractor<tau_ba<BAs...>, BAs...>
			| non_terminal_extractor<tau_ba<BAs...>, BAs...>
			| optional_value_extractor<size_t>;
		auto var_name = (io
			| only_child_extractor<tau_ba<BAs...>, BAs...>
			| optional_value_extractor<gssotc<BAs...>>)->child[0];
		switch (pos) {
			case tau_parser::current_pos:
				name.emplace(var_name);
				break;
			case tau_parser::absolute_pos:
				name.emplace(var_name);
				loopback = max(loopback,
					io | only_child_extractor<tau_ba<BAs...>, BAs...>
					| tau_parser::var_pos
					| tau_parser::absolute_pos
					| tau_parser::num
					| only_child_extractor<tau_ba<BAs...>, BAs...>
					| offset_extractor<tau_ba<BAs...>, BAs...>
					| optional_value_extractor<size_t>);
				break;
			case tau_parser::relative_pos:
				name.emplace(var_name);
				loopback = max(loopback,
					io | only_child_extractor<tau_ba<BAs...>, BAs...>
					| tau_parser::var_pos
					| tau_parser::relative_pos
					| tau_parser::num
					| only_child_extractor<tau_ba<BAs...>, BAs...>
					| offset_extractor<tau_ba<BAs...>, BAs...>
					| optional_value_extractor<size_t>);
				break;
			default:
				throw std::logic_error("bad timed pos");
		}
	}

	std::set<gssotc<BAs...>> name;
	size_t loopback = 0;
};

template<typename... BAs>
std::pair<tau_spec_vars<BAs...>, tau_spec_vars<BAs...>> get_io_vars(const gssotc<BAs...> collapsed) {
	tau_spec_vars<BAs...> inputs;
	tau_spec_vars<BAs...> outputs;
	for (const auto& variable: select_top(collapsed, is_non_terminal<tau_parser::io_var, tau_ba<BAs...>, BAs...>)) {
		auto type = variable
			| only_child_extractor<tau_ba<BAs...>, BAs...>
			| non_terminal_extractor<tau_ba<BAs...>, BAs...>
			| optional_value_extractor<size_t>;
		(type == tau_parser::in ? inputs : outputs).add(variable);
	}

	DBG(std::cout << "(I) inputs: " ; for (auto& i: inputs.name) std::cout << i << ", "; std::cout << std::endl;)
	DBG(std::cout << "(I) outputs: " ; for (auto& o: outputs.name) std::cout << o << ", "; std::cout << std::endl;)

	return {inputs, outputs};
}

template<typename... BAs>
std::pair<std::string, extracted_bindings<tau_ba<BAs...>, BAs...>>  get_eta_nso_rr(
		const std::optional<gssotc<BAs...>>& positive,
		const std::vector<gssotc<BAs...>>& negatives,
		const tau_spec_vars<BAs...>& inputs,
		const tau_spec_vars<BAs...>& outputs) {
	auto print_vars = [] (const auto& vars) {
		std::basic_stringstream<char> str;
		for (const auto& var: vars.name) {
			str << var;
			if (var != *vars.name.rbegin()) str << ", ";
		}
		return str.str();
	};
	auto universally_quantify_vars = [] (const auto& vars, size_t loopback) {
		std::basic_stringstream<char> str;
		for (const auto& var: vars.name) {
			str << "all " << var << "[" << loopback << "] ";
		}
		return str.str();
	};
	auto existentially_quantify_vars = [] (const auto& vars, size_t loopback) {
		std::basic_stringstream<char> str;
		for (const auto& var: vars.name) {
			str << "ex " << var << "[" << loopback << "] ";
		}
		return str.str();
	};
	auto print_boolean_existential_quantifiers = [] (size_t negatives) {
		std::basic_stringstream<char> str;
		for (size_t i = 1; i <= negatives; ++i) {
			str << "bex $nn" << i << " ";
			if (i < negatives) str << ",";
		}
		return str.str();
	};
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

	extracted_bindings<tau_ba<BAs...>, BAs...> bindings;
	size_t binding_seed = 0;
	size_t loopback = max(inputs.loopback, outputs.loopback);
	std::basic_stringstream<char> nsorr;
	nsorr << "eta[t](" << print_vars(outputs) << ") := ";
		// add call to etas
		if (negatives.size() > 0) {
			nsorr << "etas[t](" << print_vars(outputs) << print_t(outputs.name.size()) << ").\n";
		}
	nsorr << "etas[t](" << print_vars(outputs) << print_negative_vars(outputs.name.size()) <<") := ";
		// quantify input and output variables
		for (size_t i = 1; i <= loopback; ++i) {
			nsorr << universally_quantify_vars(inputs, i);
			nsorr << existentially_quantify_vars(outputs, i);
		}
		// boolean quantify new negative vars
		nsorr << print_boolean_existential_quantifiers(outputs.name.size());
		// add positive literal if it exists
		if (positive.has_value()) {
			nsorr << clause_to_string(positive.value(), bindings, binding_seed);
			if (negatives.size() > 0) nsorr << " && ";
		}
		// generate a disjunction of all negations conjucted with the max_loopback variables and
		if (negatives.size() > 0) {
			for (size_t i = 0; i < negatives.size(); ++i) {
				nsorr << "(( $n" << i << " && " << "$nn" << i << " ) &&& " << clause_to_string(negatives[i], bindings, binding_seed) << ") &&& ";
			}
			// add recursive call with t-1
			nsorr << "etas[t-1](" << print_vars(outputs) << print_new_negative_vars(outputs.name.size()) << ").\n";
		}
	// add main

	DBG(std::cout << "(I) get_eta_nso_rr: " << nsorr.str() << std::endl;)

	return {nsorr.str(), bindings};
}

template<typename... BAs>
std::string get_check_nso_rr(const tau_spec_vars<BAs...>& outputs, size_t loopback, size_t current) {
	auto print_vars = [] (const auto& vars) {
		std::basic_stringstream<char> str;
		for (const auto& var: vars.name) {
			str << var;
			if (var != *vars.name.rbegin()) str << ", ";
		}
		return str.str();
	};
	auto existentially_quantify_vars = [] (const auto& vars, size_t loopback) {
		std::basic_stringstream<char> str;
		for (const auto& var: vars.name) {
			str << "ex " << var << "[" << loopback << "] ";
		}
		return str.str();
	};
	std::basic_stringstream<char> check;
	check << existentially_quantify_vars(outputs, loopback) <<
		" (eta[" << current << "](" << print_vars(outputs) << ").\n";

	DBG(std::cout << "(I) get_check_nso_rr: " << check.str() << std::endl;)

	return check.str();
}

template<typename... BAs>
std::string get_main_nso_rr(const tau_spec_vars<BAs...>& outputs, size_t loopback, size_t current, size_t previous) {
	auto print_vars = [] (const auto& vars) {
		std::basic_stringstream<char> str;
		for (const auto& var: vars.name) {
			str << var;
			if (var != *vars.name.rbegin()) str << ", ";
		}
		return str.str();
	};
	auto universally_quantify_vars = [] (const auto& vars, size_t loopback) {
		std::basic_stringstream<char> str;
		for (const auto& var: vars.name) {
			str << "all " << var << "[" << loopback << "] ";
		}
		return str.str();
	};
	std::basic_stringstream<char> main;
	main << universally_quantify_vars(outputs, loopback) <<
		" (eta[" << current << "](" << print_vars(outputs) << ") <-> eta[" << previous << "]).\n";

	DBG(std::cout << "(I) get_main_nso_rr: " << main.str() << std::endl;)

	return main.str();
}

template<typename... BAs>
std::pair<std::string, extracted_bindings<tau_ba<BAs...>, BAs...>> get_wff_main_nso_rr(const gssotc<BAs...>& positive, const tau_spec_vars<BAs...>& inputs, const tau_spec_vars<BAs...>& outputs, size_t loopback) {
	auto universally_quantify_vars = [] (const auto& vars, size_t index) {
		std::basic_stringstream<char> str;
		for (const auto& var: vars.name) {
			str << "all " << var << "[" << index << "] ";
		}
		return str.str();
	};
	auto existentially_quantify_vars = [] (const auto& vars, size_t index) {
		std::basic_stringstream<char> str;
		for (const auto& var: vars.name) {
			str << "ex " << var << "[" << index << "] ";
		}
		return str.str();
	};

	std::basic_stringstream<char> main;
	extracted_bindings<tau_ba<BAs...>, BAs...> ext_bindings;
	bindings<BAs...> bindings;

	auto wff = positive | tau_parser::wff | optional_value_extractor<gssotc<BAs...>>;

	//

	// print quantifiers related to free variables
	for (size_t i = 1; i <= loopback; ++i) {
		main << universally_quantify_vars(inputs, i);
		main << existentially_quantify_vars(outputs, i);
	}

	// print the main wff
	main << clause_to_string(wff, ext_bindings)	<< ".\n";

	DBG(std::cout << "(I) get_main_nso_wo_rr: " << main.str() << std::endl;)

	return {main.str(), ext_bindings};
}


template<typename... BAs>
bool is_satisfiable_clause(const gssotc<BAs...>& clause) {

	DBG(std::cout << "(I) is_satisfiable_clause: " << clause << std::endl;)

	auto collapsed = clause |
		repeat_all<step<tau_ba<BAs...>, BAs...>, tau_ba<BAs...>, BAs...>(
			simplify_tau<tau_ba<BAs...>, BAs...>
			| collapse_positives_tau<tau_ba<BAs...>, BAs...>);

	auto [positive, negatives] = get_positive_and_negative_literals(collapsed);
	auto [inputs, outputs] = get_io_vars(collapsed);
	size_t loopback = max(inputs.loopback, outputs.loopback);

	if (inputs.name.empty() && outputs.name.empty()) {
		auto check = clause | tau_parser::tau_wff | tau_parser::wff | tau_parser::wff_t;
		return check.has_value() ? true : false;
	}

	if (negatives.empty() && positive.has_value()) {
		auto [main_wo_rr, ext_bindings] = get_wff_main_nso_rr<BAs...>(positive.value(), inputs, outputs, loopback);
		bindings<BAs...> reversed_bindings; // TODO (LOW) reverse
		auto normalize = normalizer<BAs...>(main_wo_rr, reversed_bindings).main;

		if ((normalize | tau_parser::wff_f).has_value()) {
			DBG(std::cout << "(I) is_satisfiable_clause: false" << std::endl;)
			return false;
		}

		DBG(std::cout << "(I) is_satisfiable_clause: true" << std::endl;)
		return true;
	}

	auto etas = get_eta_nso_rr<BAs...>(positive, negatives, inputs, outputs);
	for (size_t current = 1; /* until return statement */ ; ++current) {
		auto [eta, extracted_bindings] = get_eta_nso_rr<BAs...>(positive, negatives, inputs, outputs);
		bindings<tau_ba<BAs...>, BAs...> reversed_bindings; // TODO (LOW) reverse
		auto check = get_check_nso_rr(outputs, loopback, current);
		auto normalize = normalizer<tau_ba<BAs...>, BAs...>(eta.append(check), reversed_bindings).main;
		if ((normalize | tau_parser::wff_f).has_value()) {
			DBG(std::cout << "(I) is_satisfiable_clause: false" << std::endl);
			return false;
		}
		for (size_t previous = 1; previous < current; ++previous) {
			auto main = get_main_nso_rr(outputs, loopback, current, previous);
			auto normalize = normalizer<tau_ba<BAs...>, BAs...>(eta.append(main), reversed_bindings).main;
			if ((normalize | tau_parser::wff_t).has_value()) return true;
		}
	}
}

// check satisfability of a tau_spec (boolean combination case)
template<typename...BAs>
bool is_satisfiable(const tau_spec<BAs...>& tau_spec) {
	DBG(std::cout << "(I) is_satisfiable: " << tau_spec << std::endl;)

	auto dnf = tau_spec.main
		| repeat_all<step<tau_ba<BAs...>, BAs...>, tau_ba<BAs...>, BAs...>(
			step<tau_ba<BAs...>, BAs...>(tau_spec.rec_relations))
		| repeat_all<step<tau_ba<BAs...>, BAs...>, tau_ba<BAs...>, BAs...>(
			to_dnf_tau<tau_ba<BAs...>, BAs...>
			| simplify_tau<tau_ba<BAs...>, BAs...>);

	for(auto& clause: get_clauses(dnf)) {
		if (is_satisfiable_clause(clause)) {
			DBG(std::cout << "(I) is_satisfiable: true" << std::endl);
			return true;
		}
	}

	DBG(std::cout << "(I) is_satisfiable: false" << std::endl);
	return false;
}

} // namespace idni::tau

#endif // __SATISFIABILITY_H__