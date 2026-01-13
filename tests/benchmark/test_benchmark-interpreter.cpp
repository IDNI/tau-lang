// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <limits>
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <thread>
#include <unistd.h>

#include "defs.h"
#include "utility/measure.h"

#include "../integration/test_integration_helpers.h"
#include "test_benchmark-helper.h"

using namespace std;
using namespace idni;
using namespace idni::tau_lang;

// using valgrind for memory leak detection
//
// valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes
// --log-file=logs/test_benchmark-bernays-schonfinkel1.memcheck
// ./build/test_benchmark-wff_normalization -t 600 -m 16384
//
// using callgrind for profiling
//
// valgrind --tool=callgrind --callgrind-out-file=logs/test_benchmark-bernays-schonfinkel1.callgrind
// ./build/test_benchmark-wff_normalization -t 600 -m 16384

static const std::vector<std::tuple<std::string, std::string, std::string>> interpreter_samples = {
//	{ "Lucca's example", "luccas_example", // OK
//		"ex a ex b ex c ex d ex f ex e (ax + bx' = cy + dy'"
//		"|| ax + bx' != ey + fy') <-> (ax + bx' = cy + gy')."},
	{ "Bernays–Schönfinkel 1", "bernays-schonfinkel1", // OOM
		"all x ex y all z ex w all u ex v ((x<y && y<z)"
		"|| (z<w && w<u)|| (u<v && v<x))."}
//	{ "Bernays–Schönfinkel 1 (simplified)", "bernays-schonfinkel_simplified", // OK
//		"all x ex y all z ex w ((x<y && y<z) || (z<w && w<x))."},
//	{ "Bernays–Schönfinkel 2", "bernays-schonfinkel2", // OOM
//		"all x ex y all z ex w all u ex v ((x<y && y<z)"
//		"|| (z<w && w<u)) -> (u<v && v<x)."},
//	{ "Bernays–Schönfinkel 3", "bernays-schonfinkel3", // OK
//		"ex x all y ex z all w ex c all o (x<y && y>w)"
//		"&& (z>x || w<y) -> (o>y || z>c)."}
//	{ "Ohad's example", "ohads_example", // ???
//		"all a all b all c all d all p all q all r all s all m"
//		"all j all k all l (all x ex y f(x,y)=0 || (g(x,y)=0 &&"
//		"h(x,y)!=0)) && !(all y0 all y1 all z0 all z1 ex x"
//		"f(x,y1x+y0'x)=0 && (g(x,y1x+y0'x)!=0 && h(x,y1x+y0'x)=0))"}
};

int execute_interpreter_benchmark(const std::string label, const std::string file, const std::string sample) {
	// creating output stream
	auto filename = "test_benchmark-"+ std::string(file) + "." + GIT_COMMIT_HASH + ".measures";
	std::ofstream outfile(filename);
	if (!outfile.is_open()) {
		std::cerr << "Error: could not open file " << filename << " for writing\n";
		return 1;
	}

	// removing all measures
	measures::remove_all<tau_<tau_ba<bv, sbf>, bv, sbf>>();
	// benchmarking the normalization of a tau formula
	measures::start_timer("tau_normalization");
	normalize_test_tau(sample.c_str());
	// measures::stop_timer("tau_normalization");

	outfile << "\n " << label << "\n";
	outfile << "------------------------------------------------------------------------------------------\n";
	outfile << " (time): " << measures::get_timer("tau_normalization") << " ms\n";
	outfile << " (rules):";
#ifdef TAU_MEASURE
	if (measures::rule_counters<tau_<tau_ba<bv, sbf>, bv, sbf>>.empty()) {
		outfile << "n/a\n";
	} else {
		outfile << "\n\n";
		using rules_counters = vector<std::pair<rule<tau_<tau_ba<bv, sbf>, bv, sbf>>, size_t>>;
		rules_counters counters(measures::rule_counters<tau_<tau_ba<bv, sbf>, bv, sbf>>.begin(),
			measures::rule_counters<tau_<tau_ba<bv, sbf>, bv, sbf>>.end());
		int width = std::floor(std::log10(counters[0].second)) + 2;
		outfile << "\t" << std::setw(width) << "uses"
			<< std::setw(width) << "hits"
			<< std::setw(7) << "ratio"
			<< "  rule\n";
		std::sort(counters.begin(), counters.end(),	[](auto a, auto b) { return a.second > b.second; });
		size_t total_counters = 0, total_hits = 0;
		for (auto [rule, counter] : counters) {
			double ratio = measures::rule_hits<tau_<tau_ba<bv, sbf>, bv, sbf>>[rule] * 100 / (double)counter;
			outfile << "\t" << std::setw(width) << counter
				<< std::setw(width) << measures::rule_hits<tau_<tau_ba<bv, sbf>, bv, sbf>>[rule]
				<< std::setw(7) << std::fixed << std::setprecision(2) << ratio << "%"
				<< " " << rule.first << ":=" << rule.second << "\n";
			total_counters += counter;
			total_hits += measures::rule_hits<tau_<tau_ba<bv, sbf>, bv, sbf>>[rule];
		}
		double total_ratio = total_hits * 100 / (double)total_counters;
		outfile << "\n\n";
		outfile << "\t" << " (total uses): " << total_counters << "\n"
			<< "\t" << " (total hits): " << total_hits << "\n"
			<< "\t" << " (total ratio): " << std::fixed << std::setprecision(2) << total_ratio << "%\n";
		outfile.close();
	}
#else // TAU_MEASURE
	outfile << "n/a\n";
#endif // TAU_MEASURE
	outfile << "------------------------------------------------------------------------------------------\n";
	outfile << "Tau git commit: " << GIT_COMMIT_HASH << "\n";
	return 0;
}

int main(int argc, char* argv[]) {
	// parsing command line arguments
	int time_limit = 60; // default time limit in seconds
    size_t memory_limit = 8192; // default memory limit in MB

    int opt;
    while ((opt = getopt(argc, argv, "t:m:")) != -1) {
        switch (opt) {
            case 't':
                time_limit = std::stoi(optarg);
                break;
            case 'm':
                memory_limit = std::stoul(optarg);
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " [-t time_in_seconds] [-m memory_in_mb]\n";
                return 1;
        }
    }

	// removing output but errors
	logging::set_filter(boost::log::trivial::severity_level::error);

	// exiting after 1 minute
	std::thread(exit_after_seconds, time_limit).detach();

	// exiting after 8192 MB
	std::thread(exit_after_mb, memory_limit).detach();

	// iterating over all samples
	int return_code = 0;
	for (auto& sample: interpreter_samples) {
		// executing the benchmark
		auto success = execute_interpreter_benchmark(std::get<0>(sample), std::get<1>(sample), std::get<2>(sample));
		std::cout << "Benchmark " << std::get<0>(sample) << (success ? ": Failed\n" : ": Passed\n");
	}
	return return_code;
}

// main method
