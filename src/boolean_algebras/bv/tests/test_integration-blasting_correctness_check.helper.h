// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __TEST_INTEGRATION_BLASTING_CORRECTNESS_CHECK_HELPER_H__
#define __TEST_INTEGRATION_BLASTING_CORRECTNESS_CHECK_HELPER_H__

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <new>
#include <sstream>
#include <string>

#include "test_init.h"
#include "test_tau_helpers.h"
#include "test_memory_query.h"

#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#elif !defined(__EMSCRIPTEN__)
#include <sys/resource.h>
#endif

#include "boolean_algebras/bv/bv_ba.h"
#include "boolean_algebras/bv/heuristics/bv_predicate_blasting.h"

// 4 GB. The runaway this bounds is a blasting blow-up, not a genuine
// multi-gigabyte result, so a larger number only delays the kill.
static constexpr size_t MEMORY_LIMIT_MB = 4096;

// Per-term wall clock, enforced in the parent by spawn_capture's watchdog.
static constexpr int BLASTING_TIMEOUT_SEC = 120;

static tref parse_wff(const std::string& s) {
	static tree<node_t>::get_options opts{ .parse = { .start = tree<node_t>::wff }};
	return tree<node_t>::get(s, opts).value_or(nullptr);
}

static std::string normalize_blasting_on(const std::string& s) {
	auto wff = parse_wff(s);
	if (!wff) return "parse_error";
	bool saved = preprocessing; preprocessing = true;
	auto r = normalizer<node_t>(wff);
	preprocessing = saved;
	return r.has_value() ? tau::get(r.value()).to_str() : "null";
}

static std::string normalize_blasting_off(const std::string& s) {
	auto wff = parse_wff(s);
	if (!wff) return "parse_error";
	bool saved = preprocessing; preprocessing = false;
	auto r = normalizer<node_t>(wff);
	preprocessing = saved;
	return r.has_value() ? tau::get(r.value()).to_str() : "null";
}

// Cap the worker's memory before it runs a term. POSIX bounds virtual address
// space with RLIMIT_AS; Windows bounds private commit with a Job Object, the
// closest twin. The cap is relative to what the process already maps, so it
// never clips memory already allocated -- a too-low absolute cap turns an
// allocation failure into SIGBUS (Windows: an uncatchable job kill), which no
// catch can see.
static void apply_blasting_memory_cap() {
#if defined(_WIN32) && !defined(__CYGWIN__)
	HANDLE job = CreateJobObjectA(nullptr, nullptr);
	if (!job) return;
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION info{};
	info.BasicLimitInformation.LimitFlags =
		JOB_OBJECT_LIMIT_PROCESS_MEMORY | JOB_OBJECT_LIMIT_JOB_MEMORY;
	const uint64_t limit = tau_test_current_vm_bytes()
		+ static_cast<uint64_t>(MEMORY_LIMIT_MB) * 1024ULL * 1024ULL;
	info.ProcessMemoryLimit = static_cast<SIZE_T>(limit);
	info.JobMemoryLimit = static_cast<SIZE_T>(limit);
	SetInformationJobObject(job, JobObjectExtendedLimitInformation,
		&info, sizeof(info));
	// AssignProcessToJobObject fails when this process already sits in a job
	// that forbids nesting; there is then no cap, which the parent's
	// wall-clock timeout still bounds.
	AssignProcessToJobObject(job, GetCurrentProcess());
#elif !defined(__EMSCRIPTEN__)
	struct rlimit saved;
	if (getrlimit(RLIMIT_AS, &saved) != 0) return;
	if (const uint64_t vm = tau_test_current_vm_bytes()) {
		rlim_t want = static_cast<rlim_t>(vm)
			+ static_cast<rlim_t>(MEMORY_LIMIT_MB)
				* 1024ULL * 1024ULL;
		if (saved.rlim_max != RLIM_INFINITY && want > saved.rlim_max)
			want = saved.rlim_max;
		struct rlimit lim = { want, saved.rlim_max };
		setrlimit(RLIMIT_AS, &lim);
	}
#endif
}

// The worker: one term per process, so a runaway blasting case is killed at
// BLASTING_TIMEOUT_SEC and its memory capped without taking the suite down.
// The marker line tells the parent which way it ended; a crash writes none.
static int blasting_child_main(int argc, char** argv) {
	if (argc < 3 || std::string(argv[1]) != "--blasting-child") return -1;
	bdd_init<Bool>();
	apply_blasting_memory_cap();
	const std::string f = argv[2];
	std::string on, off;
	bool oom = false;
	try {
		on = normalize_blasting_on(f);
		off = normalize_blasting_off(f);
	} catch (const std::bad_alloc&) {
		oom = true;
	}
	std::cout << "TAU_BLASTING_CHECK|"
		<< (oom ? "oom" : (on == off ? "eq" : "ne")) << "\n";
	if (!oom && on != off)
		std::cout << "on: " << on << "\noff: " << off << "\n";
	std::cout.flush();
	return 0;
}

namespace {
	struct _blasting_child_registrar {
		_blasting_child_registrar() { test_child_hook = &blasting_child_main; }
	};
	inline _blasting_child_registrar _blasting_child_registrar_instance;
}

static void check_blasting_correctness(const char* f) {
	const std::string self = tau_test_exe_path();
	if (self.empty()) {
		// No path baked in (a hand-built binary): run inline, uncapped.
		std::string on, off;
		bool oom = false;
		try {
			on = normalize_blasting_on(f);
			off = normalize_blasting_off(f);
		} catch (const std::bad_alloc&) {
			oom = true;
		}
		if (oom) {
			CHECK_MESSAGE(false, "memory limit exceeded ("
				<< MEMORY_LIMIT_MB << " MB)");
			return;
		}
		CHECK(on == off);
		return;
	}

	auto run = tau_test_run({ self, "--blasting-child", f }, "",
		BLASTING_TIMEOUT_SEC);
	if (run.out.find("TAU_BLASTING_CHECK|eq") != std::string::npos) return;
	if (run.timed_out) {
		CHECK_MESSAGE(false, "blasting child timed out after "
			<< BLASTING_TIMEOUT_SEC << "s for: " << f);
		return;
	}
	if (run.out.find("TAU_BLASTING_CHECK|oom") != std::string::npos) {
		CHECK_MESSAGE(false, "memory limit exceeded ("
			<< MEMORY_LIMIT_MB << " MB) for: " << f);
		return;
	}
	std::ostringstream msg;
	msg << "blasting child reported no equivalence for: " << f
		<< "\n--- child stdout ---\n" << run.out
		<< "\n--- child stderr ---\n" << run.err;
	CHECK_MESSAGE(false, msg.str());
}

#endif // __TEST_INTEGRATION_BLASTING_CORRECTNESS_CHECK_HELPER_H__
