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

// TODO (IMPORTANT) execution
// TODO (IMPORTANT) basic implementations
// TODO (IMPORTANT) testing


#ifndef __EXECUTOR_H__
#define __EXECUTOR_H__

#include "tau_ba.h"
#include "satisfiability.h"

template<typename...BAs>
using tau_input = std::map<gssotc<BAs...>, std::variant<BAs...>>;

template<typename...BAs>
using tau_inputs = std::vector<tau_input<BAs...>>;

template<typename...BAs>
using tau_output = std::pair<gssotc<BAs...>, std::variant<BAs...>>;

template<typename...BAs>
using tau_outputs = std::vector<tau_output<BAs...>>;

template<typename factory_t, typename splitter_t, typename...BAs>
struct executable {

	std::variant<BAs...> execute(const tau_input<BAs...>& input) {
		// check satisfiability
		BOOST_LOG_TRIVIAL(trace) << "(I) -- Begin execute_step " << it;
		BOOST_LOG_TRIVIAL(trace) << spec;

		auto current = build_main_step<tau_ba<BAs...>, BAs...>(spec.main, it)
			| repeat_all<step<tau_ba<BAs...>, BAs...>, tau_ba<BAs...>, BAs...>(step<tau_ba<BAs...>, BAs...>(spec.rec_relations));

		// compute output
		auto output = execute_gssotc(current, inputs).value();
		// update executable
		inputs.push_back(input);
		outputs.push_back(output);
		it++;
		return output;
	}

	tau_spec<BAs...> spec;
	tau_inputs<BAs...> inputs;
	tau_outputs<BAs...> outputs;
	factory_t factory;
	splitter_t splitter;
	size_t it = 0;

private:

	std::optional<tau_outputs<BAs...>> execute_gssotc_clause_general(const gssotc<BAs...>& positive, const std::vector<gssotc<BAs...>>& negatives, const tau_input<BAs...>& input, const tau_output<BAs...>& output, size_t loopback) {
		// TODO (HIGH) change this...
		auto etas = build_eta_nso_rr<BAs...>(positive, negatives, inputs, outputs);
		for (size_t current = 1; /* until return statement */ ; ++current) {
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

	std::optional<tau_outputs<BAs...>> execute_gssotc_clause_no_negatives_with_loopback(const gssotc<BAs...>& positive, const tau_input<BAs...>& input, const tau_output<BAs...>& output, size_t loopback) {
		// TODO (HIGH) change this...
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

	std::optional<tau_outputs<BAs...>> execute_gssotc_clause_no_negatives_no_loopback(const gssotc<BAs...>& positive, const tau_input<BAs...>& input, const tau_output<BAs...>& output) {
		// TODO (HIGH) change this...
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

	std::optional<tau_outputs<BAs...>> execute_gssotc_clause_no_outputs(const gssotc<BAs...>& clause, const tau_input<BAs...>& input) {
		auto wff = clause | tau_parser::tau_wff | tau_parser::wff | optional_value_extractor<gssotc<BAs...>>;
		bindings<tau_ba<BAs...>, BAs...> bindings;
		std::basic_stringstream<char> main;
		// TODO (HIGH) change this, the inputs are vars... in this case
		main << build_universal_quantifiers(inputs) << wff << ".";
		std::string str = main.str();
		auto normalized = normalizer<tau_ba<BAs...>, BAs...>(str, bindings).main;
		if (auto check = normalized | tau_parser::wff_t; check.has_value()) {
			BOOST_LOG_TRIVIAL(trace) << "(I) -- Check is_gssotc_clause_satisfiable_no_outputs: true";
			return check;
		}

		BOOST_LOG_TRIVIAL(trace) << "(I) -- Check is_gssotc_clause_satisfiable: false";
		return {};
	}

	std::optional<tau_outputs<BAs...>> execute_gssotc_clause(const gssotc<BAs...>& clause, const tau_inputs<BAs...>& input) {

		BOOST_LOG_TRIVIAL(trace) << "(I) -- Executing gssotc clause";
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

			return execute_gssotc_clause_no_outputs(collapsed, inputs);
		} else if (negatives.empty() && positive.has_value() && loopback == 0) {

			BOOST_LOG_TRIVIAL(trace) << "(I) -- No negatives and no loopback case";
			BOOST_LOG_TRIVIAL(trace) << clause;

			return execute_gssotc_clause_no_negatives_with_loopback(positive, inputs, outputs);
		} else if (negatives.empty() && positive.has_value()) {

			BOOST_LOG_TRIVIAL(trace) << "(I) -- No negatives with loopback case";
			BOOST_LOG_TRIVIAL(trace) << clause;

			return execute_gssotc_clause_no_negatives_with_loopback(positive, inputs, outputs, loopback);
		}


		BOOST_LOG_TRIVIAL(trace) << "(I) -- General case";
		BOOST_LOG_TRIVIAL(trace) << clause;

		return execute_gssotc_clause_general(positive, negatives, inputs, outputs, loopback);
	}

	std::optional<tau_outputs<BAs...>> execute_gssotc(const gssotc<BAs...>& form, const tau_inputs<BAs...>& input) {
		auto dnf = form
			| repeat_all<step<tau_ba<BAs...>, BAs...>, tau_ba<BAs...>, BAs...>(
				to_dnf_tau<tau_ba<BAs...>, BAs...>
				| simplify_tau<tau_ba<BAs...>, BAs...>);

		BOOST_LOG_TRIVIAL(trace) << "(I) -- Converting to dnf and simplifying";
		BOOST_LOG_TRIVIAL(trace) << dnf;

		auto clauses = get_gssotc_clauses(dnf);
		for (auto& clause: clauses)
			if (auto check = execute_gssotc_clause(clause); check.has_value()) return check;
		return {};
	}
};

template<typename factory_t, typename splitter_t, typename...BAs>
std::optional<executable<factory_t, splitter_t, BAs...>> execute(factory_t& factory, splitter_t& splitter, const tau_spec<BAs...>& spec) {
	BOOST_LOG_TRIVIAL(trace) << "(I) -- Begin execute";
	BOOST_LOG_TRIVIAL(trace) << spec;

	if (is_tau_spec_satisfiable(spec)) {
		return { .spec = spec, .factory = factory, .splitter = splitter };
	}
	return {};
}

# endif //__EXECUTOR_H__