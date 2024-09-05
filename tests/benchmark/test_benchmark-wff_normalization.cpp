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

static const std::vector<std::tuple<std::string, std::string, std::string>> samples = {
	{ "Lucca's example", "luccas_example",
		"ex a ex b ex c ex d ex f ex e (ax + bx' = cy + dy'"
		"|| ax + bx' != ey + fy') <-> (ax + bx' = cy + gy')."},
//	{ "Bernays–Schönfinkel 1", "bernays-schonfinkel1",
//		"all x ex y all z ex w all u ex v ((x<y && y<z)"
//		"|| (z<w && w<u)|| (u<v && v<x))."},
	{ "Bernays–Schönfinkel 1 (simplified)", "bernays-schonfinkel_simplified",
		"all x ex y all z ex w ((x<y && y<z) || (z<w && w<x))."},
//	{ "Bernays–Schönfinkel 2", "bernays-schonfinkel2",
//		"all x ex y all z ex w all u ex v ((x<y && y<z)"
//		"|| (z<w && w<u)) -> (u<v && v<x)."},
	{ "Bernays–Schönfinkel 3", "bernays-schonfinkel3",
		"ex x all y ex z all w ex c all o (x<y && y>w)"
		"&& (z>x || w<y) -> (o>y || z>c)."}
//	{ "Ohad's example", "ohads_example",
//		"all a all b all c all d all p all q all r all s all m"
//		"all j all k all l (all x ex y f(x,y)=0 || (g(x,y)=0 &&"
//		"h(x,y)!=0)) && !(all y0 all y1 all z0 all z1 ex x"
//		"f(x,y1x+y0'x)=0 && (g(x,y1x+y0'x)!=0 && h(x,y1x+y0'x)=0))"}
};

int execute_benchmark(const std::string label, const std::string file, const std::string sample) {
	// creating output stream
	auto filename = "test_benchmark-"+ std::string(file) + "." + GIT_COMMIT_HASH + ".measures";
	std::ofstream outfile(filename);
	if (!outfile.is_open()) {
		std::cerr << "Error: could not open file " << filename << " for writing\n";
		return 1;
	}

	// removing all measures
	measures::remove_all<nso<tau_ba<bdd_test>, bdd_test>>();
	// benchmarking the normalization of a tau formula
	measures::start_timer("tau_normalization");
	normalize_test_tau(sample.c_str());
	// measures::stop_timer("tau_normalization");

    outfile << "\n " << label << "\n";
	outfile << "------------------------------------------------------------------------------------------\n";
	outfile << " (time): " << measures::get_timer("tau_normalization") << " ms\n";
	outfile << " (rules):";
	#ifdef TAU_MEASURE
	if (measures::rule_counters<nso<tau_ba<bdd_test>, bdd_test>>.empty()) {
		outfile << "n/a\n";
	} else {
		outfile << "\n\n";
		using rules_counters = vector<std::pair<rule<nso<tau_ba<bdd_test>, bdd_test>>, size_t>>;
		rules_counters counters(measures::rule_counters<nso<tau_ba<bdd_test>, bdd_test>>.begin(),
			measures::rule_counters<nso<tau_ba<bdd_test>, bdd_test>>.end());
		int width = std::floor(std::log10(counters[0].second)) + 2;
		outfile << "\t" << std::setw(width) << "uses"
			<< std::setw(width) << "hits"
			<< std::setw(7) << "ratio"
			<< "  rule\n";
		std::sort(counters.begin(), counters.end(),	[](auto a, auto b) { return a.second > b.second; });
		size_t total_counters = 0, total_hits = 0;
		for (auto [rule, counter] : counters) {
			double ratio = measures::rule_hits<nso<tau_ba<bdd_test>, bdd_test>>[rule] * 100 / (double)counter;
			outfile << "\t" << std::setw(width) << counter
				<< std::setw(width) << measures::rule_hits<nso<tau_ba<bdd_test>, bdd_test>>[rule]
				<< std::setw(7) << std::fixed << std::setprecision(2) << ratio << "%"
				<< " " << rule.first << ":=" << rule.second << "\n";
			total_counters += counter;
			total_hits += measures::rule_hits<nso<tau_ba<bdd_test>, bdd_test>>[rule];
		}
		double total_ratio = total_hits * 100 / (double)total_counters;
		outfile << "\n\n";
		outfile << "\t" << " (total uses): " << total_counters << "\n"
			<< "\t" << " (total hits): " << total_hits << "\n"
			<< "\t" << " (total ratio): " << std::fixed << std::setprecision(2) << total_ratio << "%\n";
		outfile.close();
	}
	#else
	outfile << "n/a\n";
	#endif // TAU_MEASURE
	outfile << "------------------------------------------------------------------------------------------\n";
	outfile << "Tau git commit: " << GIT_COMMIT_HASH << "\n";
	return 0;
}

int main(int, char**) {
	// removing output but errors
	boost::log::core::get()->set_filter(
		boost::log::trivial::severity >= boost::log::trivial::severity_level::error);

	// iterating over all samples
	int return_code = 0;
	for (auto& sample: samples) {
		// executing the benchmark
		auto success = execute_benchmark(std::get<0>(sample), std::get<1>(sample), std::get<2>(sample));
		std::cout << "Benchmark " << std::get<0>(sample) << (success ? ": Failed\n" : ": Passed\n");
	}
	return return_code;
}

// main method
