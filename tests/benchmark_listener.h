// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Canonical benchmark listener for doctest.
// Emits per-test timing data to a JSON file for CI regression detection.
// Performance worsening is treated as a bug (CI failure).

#ifndef __IDNI__TAU__BENCHMARK_LISTENER_H__
#define __IDNI__TAU__BENCHMARK_LISTENER_H__

#include "doctest.h"
#include <chrono>
#include <fstream>
#include <string>
#include <vector>

struct BenchmarkListener : public doctest::IReporter {

	// Per-test result with timing
	struct TestTiming {
		std::string suite;
		std::string name;
		double ms;
		bool passed;
	};

	std::chrono::high_resolution_clock::time_point test_start_;
	std::vector<TestTiming> results_;
	std::string current_suite_;
	std::string current_name_;
	std::string output_file_;
	const doctest::ContextOptions& opt_;

	BenchmarkListener(const doctest::ContextOptions& in)
		: opt_(in) {
		// Output file: TAU_BENCHMARK_FILE env var, or default name
		const char* env = std::getenv("TAU_BENCHMARK_FILE");
		if (env && env[0]) output_file_ = env;
		else output_file_ = "benchmark_timing.json";
	}

	void report_query(const doctest::QueryData&) override {}
	void test_run_start() override {}

	void test_run_end(const doctest::TestRunStats& s) override {
		// Write JSON output
		std::ofstream out(output_file_);
		if (!out.is_open()) return;
		out << "{\n  \"tests\": [\n";
		for (size_t i = 0; i < results_.size(); ++i) {
			const auto& r = results_[i];
			out << "    {\"suite\": \"";
			write_escaped(out, r.suite);
			out << "\", \"name\": \"";
			write_escaped(out, r.name);
			out << "\", \"ms\": " << r.ms
			    << ", \"passed\": " << (r.passed ? "true" : "false")
			    << "}";
			if (i + 1 < results_.size()) out << ",";
			out << "\n";
		}
		out << "  ],\n"
		    << "  \"total_tests\": " << s.numTestCasesPassingFilters << ",\n"
		    << "  \"passed\": " << (s.numTestCasesPassingFilters
					  - s.numTestCasesFailed) << ",\n"
		    << "  \"failed\": " << s.numTestCasesFailed << "\n"
		    << "}\n";
		out.close();

		// Print summary to stdout
		double total_ms = 0;
		for (const auto& r : results_) total_ms += r.ms;
		std::cout << "\n[benchmark] " << results_.size()
			  << " tests, total " << total_ms << " ms"
			  << " -> " << output_file_ << "\n";
	}

	void test_case_start(const doctest::TestCaseData& tc) override {
		current_suite_ = tc.m_test_suite;
		current_name_ = tc.m_name;
		test_start_ = std::chrono::high_resolution_clock::now();
	}

	void test_case_reenter(const doctest::TestCaseData&) override {}

	void test_case_end(const doctest::CurrentTestCaseStats& st) override {
		auto end = std::chrono::high_resolution_clock::now();
		double ms = std::chrono::duration<double, std::milli>(
			end - test_start_).count();
		results_.push_back({
			current_suite_,
			current_name_,
			ms,
			st.testCaseSuccess
		});
		// Always print per-test timing
		std::cout << "[bench " << ms << " ms] "
			  << current_suite_ << " / "
			  << current_name_ << "\n";
	}

	void test_case_exception(const doctest::TestCaseException&) override {}
	void subcase_start(const doctest::SubcaseSignature&) override {}
	void subcase_end() override {}
	void log_assert(const doctest::AssertData&) override {}
	void log_message(const doctest::MessageData&) override {}
	void test_case_skipped(const doctest::TestCaseData&) override {}

private:
	static void write_escaped(std::ostream& os, const std::string& s) {
		for (char c : s) {
			if (c == '"') os << "\\\"";
			else if (c == '\\') os << "\\\\";
			else if (c == '\n') os << "\\n";
			else os << c;
		}
	}
};

REGISTER_LISTENER("benchmark", 1, BenchmarkListener);

#endif // __IDNI__TAU__BENCHMARK_LISTENER_H__
