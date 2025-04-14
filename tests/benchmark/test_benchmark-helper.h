// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __TEST_BENCHMARK_HELPER_H__
#define __TEST_BENCHMARK_HELPER_H__

#include <limits>
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <thread>
#include <unistd.h>

#include "boolean_algebras/tau_ba_depreciating.h"

// Function to get memory usage in bytes
size_t get_memory_usage() {
    std::ifstream statm("/proc/self/statm");
    size_t size;
    statm >> size;
    return size * sysconf(_SC_PAGESIZE);
}

void exit_after_mb(size_t limit_mb) {
    size_t limit_bytes = limit_mb * 1024 * 1024;
    while (true) {
        size_t memoryUsage = get_memory_usage();
        if (memoryUsage > limit_bytes) {
            std::cout << "Memory usage exceeded " << limit_mb << " MB. Exiting program.\n";
            std::exit(1);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Check every second
    }
}

void exit_after_seconds(int limit_sec) {
	std::this_thread::sleep_for(std::chrono::seconds(limit_sec));
	std::cout << "Time limit exceeded " << limit_sec << " seconds. Exiting program.\n";
	std::exit(1);
}

#endif // __TEST_BENCHMARK_HELPER_H__