// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __TEST_INTEGRATION_BLASTING_CORRECTNESS_CHECK_HELPER_H__
#define __TEST_INTEGRATION_BLASTING_CORRECTNESS_CHECK_HELPER_H__

#include <atomic>
#include <chrono>
#include <fstream>
#include <future>
#include <sys/resource.h>
#include <thread>
#include <unistd.h>

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv_ba.h"
#include "heuristics/bv_predicate_blasting.h"

static constexpr long long TIME_LIMIT_MS  = 1'200'000; // 2 minutes per test
static constexpr size_t    MEMORY_LIMIT_MB = 16384;   // 2 GB per test

static tref parse_wff(const std::string& s) {
	static tree<node_t>::get_options opts{ .parse = { .start = tree<node_t>::wff }};
	return tree<node_t>::get(s, opts);
}

static std::string normalize_blasting_on(const std::string& s) {
	auto wff = parse_wff(s);
	if (!wff) return "parse_error";
	bool saved = bv_blasting; bv_blasting = true;
	auto r = normalizer<node_t>(wff);
	bv_blasting = saved;
	return r ? tau::get(r).to_str() : "null";
}

static std::string normalize_blasting_off(const std::string& s) {
	auto wff = parse_wff(s);
	if (!wff) return "parse_error";
	bool saved = bv_blasting; bv_blasting = false;
	auto r = normalizer<node_t>(wff);
	bv_blasting = saved;
	return r ? tau::get(r).to_str() : "null";
}

static void check_blasting_correctness(const char* f) {
	using namespace std::chrono;

	// Enforce MEMORY_LIMIT_MB during execution via RLIMIT_AS.
	// Any allocation that would push virtual memory past the cap causes
	// operator new to throw std::bad_alloc, which the worker catches.
	struct rlimit saved_rlimit;
	getrlimit(RLIMIT_AS, &saved_rlimit);
	{
		std::ifstream statm("/proc/self/statm");
		size_t vm_pages = 0;
		statm >> vm_pages;
		rlim_t page_size = static_cast<rlim_t>(sysconf(_SC_PAGESIZE));
		rlim_t new_cur   = static_cast<rlim_t>(vm_pages) * page_size
		                 + static_cast<rlim_t>(MEMORY_LIMIT_MB) * 1024ULL * 1024ULL;
		if (saved_rlimit.rlim_max != RLIM_INFINITY && new_cur > saved_rlimit.rlim_max)
			new_cur = saved_rlimit.rlim_max;
		struct rlimit new_rlimit = { new_cur, saved_rlimit.rlim_max };
		setrlimit(RLIMIT_AS, &new_rlimit);
	}

	// Heap-allocate results so a detached worker cannot dangle.
	auto on_result  = std::make_shared<std::string>();
	auto off_result = std::make_shared<std::string>();
	auto oom_flag   = std::make_shared<std::atomic<bool>>(false);
	std::string formula(f);

	std::promise<void> done;
	auto done_fut = done.get_future();
	auto t0 = steady_clock::now();

	std::thread worker([formula, on_result, off_result, oom_flag, p = std::move(done)]() mutable {
		try {
			*on_result = normalize_blasting_on(formula.c_str());
			if (!oom_flag->load())
				*off_result = normalize_blasting_off(formula.c_str());
		} catch (const std::bad_alloc&) {
			oom_flag->store(true);
		}
		p.set_value();
	});

	bool timed_out = done_fut.wait_for(milliseconds(TIME_LIMIT_MS)) == std::future_status::timeout;
	long long elapsed_ms = duration_cast<milliseconds>(steady_clock::now() - t0).count();

	setrlimit(RLIMIT_AS, &saved_rlimit); // always restore before returning

	if (timed_out) {
		worker.detach();
		INFO("elapsed " << elapsed_ms << "ms (limit " << TIME_LIMIT_MS << "ms)");
		CHECK_MESSAGE(false, "timed out after " << TIME_LIMIT_MS << "ms");
		return;
	}

	worker.join();

	if (oom_flag->load()) {
		CHECK_MESSAGE(false, "memory limit exceeded (" << MEMORY_LIMIT_MB << " MB)");
		return;
	}

	CHECK(*on_result == *off_result);
}

#endif // __TEST_INTEGRATION_BLASTING_CORRECTNESS_CHECK_HELPER_H__
