// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Platform memory queries the blasting suites and the codegen bench gate on:
// how much memory is free, and how much this process has mapped. Kept out of
// test_helpers.h so only the two TUs that need one pull in windows.h, mach or
// /proc.

#ifndef __IDNI__TAU__TESTS__TEST_MEMORY_QUERY_H__
#define __IDNI__TAU__TESTS__TEST_MEMORY_QUERY_H__

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>

#if defined(_WIN32) && !defined(__CYGWIN__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <psapi.h>
#elif defined(__APPLE__)
#include <mach/mach.h>
#include <unistd.h>
#elif !defined(__EMSCRIPTEN__)
#include <unistd.h>
#endif

namespace idni::tau_lang {

/// Physical memory free for new allocations, in bytes, or 0 when the platform
/// cannot say. Linux reads MemAvailable, macOS adds the free, inactive and
/// speculative pages, Windows asks GlobalMemoryStatusEx.
inline uint64_t tau_test_available_mem_bytes() {
#if defined(_WIN32) && !defined(__CYGWIN__)
	MEMORYSTATUSEX st{};
	st.dwLength = sizeof(st);
	if (!GlobalMemoryStatusEx(&st)) return 0;
	return static_cast<uint64_t>(st.ullAvailPhys);
#elif defined(__APPLE__)
	mach_port_t host = mach_host_self();
	vm_size_t page = 0;
	if (host_page_size(host, &page) != KERN_SUCCESS) return 0;
	vm_statistics64_data_t vm{};
	mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
	if (host_statistics64(host, HOST_VM_INFO64,
		reinterpret_cast<host_info64_t>(&vm), &count) != KERN_SUCCESS)
		return 0;
	return (static_cast<uint64_t>(vm.free_count) + vm.inactive_count
		+ vm.speculative_count) * static_cast<uint64_t>(page);
#elif defined(__EMSCRIPTEN__)
	return 0;
#else
	std::ifstream meminfo("/proc/meminfo");
	std::string line;
	while (std::getline(meminfo, line)) {
		if (line.rfind("MemAvailable:", 0) != 0) continue;
		unsigned long long kb = 0;
		if (std::sscanf(line.c_str(), "MemAvailable: %llu kB", &kb) != 1)
			return 0;
		return static_cast<uint64_t>(kb) * 1024ULL;
	}
	return 0;
#endif
}

/// Virtual memory this process has mapped, in bytes, or 0 when unavailable.
/// The blasting cap is sized relative to this, so it never clips memory
/// already mapped -- that would turn an allocation failure into SIGBUS,
/// which a catch cannot see.
inline uint64_t tau_test_current_vm_bytes() {
#if defined(_WIN32) && !defined(__CYGWIN__)
	PROCESS_MEMORY_COUNTERS pmc{};
	if (!GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
		return 0;
	return static_cast<uint64_t>(pmc.PagefileUsage);
#elif defined(__APPLE__)
	mach_task_basic_info_data_t info;
	mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
	if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO,
		reinterpret_cast<task_info_t>(&info), &count) != KERN_SUCCESS)
		return 0;
	return static_cast<uint64_t>(info.virtual_size);
#elif defined(__EMSCRIPTEN__)
	return 0;
#else
	std::ifstream statm("/proc/self/statm");
	unsigned long long vm_pages = 0;
	if (!(statm >> vm_pages)) return 0;
	return static_cast<uint64_t>(vm_pages)
		* static_cast<uint64_t>(sysconf(_SC_PAGESIZE));
#endif
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TESTS__TEST_MEMORY_QUERY_H__
