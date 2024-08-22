// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentiTd cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <limits>
#include <iostream>

#include "defs.h"
#include "nso_rr.h"
#include "bool_ba.h"
#include "bdd_handle.h"
#include "normalizer.h"
#include "measure.h"

#include "../integration/test_integration_helpers-tau.h"

using namespace std;
using namespace idni;
using namespace idni::rewriter;
using namespace idni::tau;

static const char* luccas_sample_label = "Lucca's example";
static const char* luccas_sample =
	"ex a ex b ex c ex d ex f ex e (ax + bx' = cy + dy'"
	"|| ax + bx' != ey + fy') <-> (ax + bx' = cy + gy').";

// static const char* ohads_sample =
//	"all a all b all c all d all p all q all r all s all m"
//	"all j all k all l (all x ex y f(x,y)=0 || (g(x,y)=0 &&"
//	"h(x,y)!=0)) && !(all y0 all y1 all z0 all z1 ex x"
//	"f(x,y1x+y0'x)=0 && (g(x,y1x+y0'x)!=0 && h(x,y1x+y0'x)=0))";

int execute_benchmark(const char* label, const char* sample) {
	// removing all measures
	measures::remove_all<nso<tau_ba<bdd_test>, bdd_test>>();
	// benchmarking the normalization of a tau formula
	measures::start_timer("tau_normalization");
	normalize_test_tau(sample);
	// measures::stop_timer("tau_normalization");
    std::cout << "\n " << label << "\n";
	std::cout << "------------------------------------------------------------------------------------------\n";
	std::cout << " (time): " << measures::get_timer("tau_normalization") << " ms\n";
	std::cout << " (rules):";
	#ifdef TAU_MEASURE
	if (measures::rule_counters<nso<tau_ba<bdd_test>, bdd_test>>.empty()) {
		std::cout << "n/a\n";
	} else {
		std::cout << "\n\n";
		using rules_counters = vector<std::pair<rule<nso<tau_ba<bdd_test>, bdd_test>>, size_t>>;
		rules_counters counters(measures::rule_counters<nso<tau_ba<bdd_test>, bdd_test>>.begin(),
			measures::rule_counters<nso<tau_ba<bdd_test>, bdd_test>>.end());
		int width = std::floor(std::log10(counters[0].second)) + 2;
		std::cout << "\t" << std::setw(width) << "uses"
			<< std::setw(width) << "hits"
			<< std::setw(7) << "ratio"
			<< "  rule\n";
		std::sort(counters.begin(), counters.end(),	[](auto a, auto b) { return a.second > b.second; });
		size_t total_counters = 0, total_hits = 0;
		for (auto [rule, counter] : counters) {
			double ratio = measures::rule_hits<nso<tau_ba<bdd_test>, bdd_test>>[rule] * 100 / (double)counter;
			std::cout << "\t" << std::setw(width) << counter
				<< std::setw(width) << measures::rule_hits<nso<tau_ba<bdd_test>, bdd_test>>[rule]
				<< std::setw(7) << std::fixed << std::setprecision(2) << ratio << "%"
				<< " " << rule.first << ":=" << rule.second << "\n";
			total_counters += counter;
			total_hits += measures::rule_hits<nso<tau_ba<bdd_test>, bdd_test>>[rule];
		}
		double total_ratio = total_hits * 100 / (double)total_counters;
		std::cout << "\n";
		std::cout << "\t" << std::setw(width) << total_counters
			<< std::setw(width) << total_hits
			<< std::setw(7) << std::fixed << std::setprecision(2) << total_ratio << "%"
			<< " totals\n";

	}
	#else
	std::cout << "n/a\n";
	#endif // TAU_MEASURE
	std::cout << "------------------------------------------------------------------------------------------\n";
	std::cout << "Tau git commit: " << GIT_COMMIT_HASH << "\n";
	return measures::get_timer("tau_normalization");
}

int main(int, char**) {
	// removing output but errors
	boost::log::core::get()->set_filter(
		boost::log::trivial::severity >= boost::log::trivial::severity_level::error);

	// output information
	std::cout << "Benchmarking wff normalization\n\n";
	execute_benchmark(luccas_sample_label, luccas_sample);
}

// main method
