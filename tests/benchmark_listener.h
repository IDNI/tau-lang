// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Canonical benchmark listener for doctest.
// Emits per-test timing and peak memory data to a JSON file for CI
// regression detection. Performance worsening is treated as a bug (CI
// failure).

#ifndef __IDNI__TAU__BENCHMARK_LISTENER_H__
#define __IDNI__TAU__BENCHMARK_LISTENER_H__

#include "quiet_doctest.h"
#include "test_env.h"
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

struct benchmark_listener : public doctest::IReporter {

	// Per-test result with timing and peak memory
	struct test_timing {
		std::string suite;
		std::string name;
		double ms;
		uint64_t rss_kb;
		bool passed;
	};

	std::chrono::high_resolution_clock::time_point test_start_;
	std::vector<test_timing> results_;
	std::string current_suite_;
	std::string current_name_;
	std::string output_file_;
	const doctest::ContextOptions& opt_;

	benchmark_listener(const doctest::ContextOptions& in)
		: opt_(in) {
		// Output file: TAU_BENCHMARK_FILE env var, or default name.
		// A constructor carries no report, so a failed read prints it.
		auto env = test_env("TAU_BENCHMARK_FILE");
		env.print_pending();
		if (env.has_value() && env.value() && !env.value()->empty())
			output_file_ = *env.value();
		else output_file_ = "benchmark_timing.json";
	}

	// The peak resident set of this process is a high-water mark, so a
	// case would inherit the peak of the cases before it. Linux resets the
	// mark on a write of 5 to /proc/self/clear_refs, which makes the value
	// read at the end of the case its own peak. Other platforms report 0.
	static void reset_peak_rss() {
#if defined(__linux__)
		std::ofstream("/proc/self/clear_refs") << "5";
#endif
	}

	static uint64_t peak_rss_kb() {
#if defined(__linux__)
		std::ifstream status("/proc/self/status");
		std::string line;
		while (std::getline(status, line))
			if (line.rfind("VmHWM:", 0) == 0)
				return std::strtoull(line.c_str() + 6, nullptr, 10);
#endif
		return 0;
	}

	static constexpr bool rss_per_case() {
#if defined(__linux__)
		return true;
#else
		return false;
#endif
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
			    << ", \"rss_kb\": " << r.rss_kb
			    << ", \"passed\": " << (r.passed ? "true" : "false")
			    << "}";
			if (i + 1 < results_.size()) out << ",";
			out << "\n";
		}
		out << "  ],\n"
		    << "  \"rss_kb_per_case\": "
		    << (rss_per_case() ? "true" : "false") << ",\n"
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
		reset_peak_rss();
		test_start_ = std::chrono::high_resolution_clock::now();
	}

	void test_case_reenter(const doctest::TestCaseData&) override {}

	void test_case_end(const doctest::CurrentTestCaseStats& st) override {
		auto end = std::chrono::high_resolution_clock::now();
		double ms = std::chrono::duration<double, std::milli>(
			end - test_start_).count();
		const uint64_t rss_kb = peak_rss_kb();
		results_.push_back({
			current_suite_,
			current_name_,
			ms,
			rss_kb,
			st.testCaseSuccess
		});
		// Always print per-test timing
		std::cout << "[bench " << ms << " ms";
		if (rss_per_case()) std::cout << ", " << rss_kb << " kB";
		std::cout << "] " << current_suite_ << " / "
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

REGISTER_LISTENER("benchmark", 1, benchmark_listener);

#endif // __IDNI__TAU__BENCHMARK_LISTENER_H__
