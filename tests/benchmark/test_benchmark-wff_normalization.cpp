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

#include <chrono>
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
#include "benchmarking.h"

#include "../integration/test_integration_helpers-tau.h"

using namespace std;
using namespace idni;
using namespace idni::rewriter;
using namespace idni::tau;

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

static const char* luccas_sample =
	"ex a ex b ex c ex d ex f ex e (ax + bx' = cy + dy'"
	"|| ax + bx' != ey + fy') <-> (ax + bx' = cy + gy').";

// static const char* ohads_sample =
//	"all a all b all c all d all p all q all r all s all m"
//	"all j all k all l (all x ex y f(x,y)=0 || (g(x,y)=0 &&"
//	"h(x,y)!=0)) && !(all y0 all y1 all z0 all z1 ex x"
//	"f(x,y1x+y0'x)=0 && (g(x,y1x+y0'x)!=0 && h(x,y1x+y0'x)=0))";

int execute_benchmark(const char* sample) {
	#ifdef TAU_BENCHMARK
	benchmarking::counters["rule_applications"] = 0;
	#endif // TAU_BENCHMARK
	// benchmarking the normalization of a tau formula
	auto start = high_resolution_clock::now();
	normalize_test_tau(sample);
	auto end = high_resolution_clock::now();
	// getting number of milliseconds as an integer
	auto duration = duration_cast<milliseconds>(end - start);
	int duration_in_ms = static_cast<int>(duration.count());
	return duration_in_ms;
}

int main(int, char**) {
	// removing output but errors
	boost::log::core::get()->set_filter(
		boost::log::trivial::severity >= boost::log::trivial::severity_level::error);

	// output information
	std::cout << "Benchmarking wff normalization\n\n";
	std::cout << "Sample Name \t\t\tTime (ms)\tRule applications\n";
	std::cout << "------------------------------------------------------------------------------------------\n";
    std::cout << "Lucca's example\t\t\t" << execute_benchmark(luccas_sample) << " ms\t\t";
	#ifdef TAU_BENCHMARK
	std::cout << benchmarking::counters["rule_applications"] << "\n";
	#else
	std::cout << "n/a\n";
	#endif // TAU_BENCHMARK
    // TODO (LOW) check why it fails here with syntyax error but works in the REPL
	// std::cout << "Ohad's example\t\t" << execute_benchmark(ohads_sample) << " ms\n";
	// #ifdef TAU_BENCHMARK
	// std::cout << benchmarking::counters["rule_applications"] << "\n";
	// #elseif
	// std::cout << "n/a\n";
	// #endif // TAU_BENCHMARK
	std::cout << "------------------------------------------------------------------------------------------\n";
	std::cout << "Tau git commit: " << GIT_COMMIT_HASH << "\n";
}

// main method
