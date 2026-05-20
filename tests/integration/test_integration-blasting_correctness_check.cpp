// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

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

static constexpr long long TIME_LIMIT_MS  = 120'000; // 2 minutes per test
static constexpr size_t    MEMORY_LIMIT_MB = 2048;   // 2 GB per test

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}

	TEST_CASE("logging") {
		logging::trace();
	}

}

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

// ---------------------------------------------------------------------------
// blasting-8bit-4var
// ---------------------------------------------------------------------------

TEST_SUITE("blasting_8bit_4var") {

	TEST_CASE("add_commutativity") {
		const char* f = "all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] (a + b) + (c + d) = (d + c) + (b + a)";
		check_blasting_correctness(f);
	}

	TEST_CASE("chain_ineq_gap") {
		const char* f = "ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] a < b && b < c && c < d && d - a >= { 3 }:bv[8]";
		check_blasting_correctness(f);
	}

	TEST_CASE("distributivity") {
		const char* f = "all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] a & (b | c) | d = (a & b) | (a & c) | d";
		check_blasting_correctness(f);
	}

	TEST_CASE("arith_composition") {
		const char* f = "ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] a > { 0 }:bv[8] && b = a * { 3 }:bv[8] && c = b + a && d = c >> { 2 }:bv[8] && d > { 0 }:bv[8]";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_assoc") {
		const char* f = "all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] (a ^ b) ^ c = a ^ (b ^ c) && a | d = d | a";
		check_blasting_correctness(f);
	}

	TEST_CASE("mod_existence") {
		const char* f = "ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] a % { 5 }:bv[8] = { 2 }:bv[8] && b = a + { 3 }:bv[8] && c = a | b && d = c & a && d !>= b";
		check_blasting_correctness(f);
	}

	TEST_CASE("or_and_order") {
		const char* f = "all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] a | b >= a && a | b >= b && c & d <= c && c & d <= d";
		check_blasting_correctness(f);
	}

	TEST_CASE("shift_existence") {
		const char* f = "ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] a > { 0 }:bv[8] && b = a << { 1 }:bv[8] && b > a && c = b << { 1 }:bv[8] && d = c >> { 1 }:bv[8] && d = b";
		check_blasting_correctness(f);
	}

}

// ---------------------------------------------------------------------------
// blasting-8bit-8var
// ---------------------------------------------------------------------------

TEST_SUITE("blasting_8bit_8var") {

	TEST_CASE("add_or_commutativity") {
		const char* f = "all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] all e:bv[8] all f:bv[8] all g:bv[8] all h:bv[8] a + b + c + d = d + c + b + a && e | f | g | h = h | g | f | e";
		check_blasting_correctness(f);
	}

	TEST_CASE("chain_ineq_gap") {
		const char* f = "ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] ex e:bv[8] ex f:bv[8] ex g:bv[8] ex h:bv[8] a < b && b < c && c < d && d < e && e < f && f < g && g < h && h - a >= { 7 }:bv[8]";
		check_blasting_correctness(f);
	}

	TEST_CASE("and_commutativity") {
		const char* f = "all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] all e:bv[8] all f:bv[8] all g:bv[8] all h:bv[8] (a & b) | (c & d) | (e & f) | (g & h) = (b & a) | (d & c) | (f & e) | (h & g)";
		check_blasting_correctness(f);
	}

	TEST_CASE("arith_composition") {
		const char* f = "ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] ex e:bv[8] ex f:bv[8] ex g:bv[8] ex h:bv[8] a > { 0 }:bv[8] && b = a * { 2 }:bv[8] && b > a && c = b + a && d = c >> { 1 }:bv[8] && e = d + a && f = e % { 7 }:bv[8] && g = f + a && h = g & a && h > { 0 }:bv[8]";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_xnor_nand_commutativity") {
		const char* f = "all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] all e:bv[8] all f:bv[8] all g:bv[8] all h:bv[8] a ^ b ^ c ^ d = d ^ c ^ b ^ a && e !^ f = f !^ e && g !& h = h !& g";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_and_sat") {
		const char* f = "ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] ex e:bv[8] ex f:bv[8] ex g:bv[8] ex h:bv[8] a < b && b < c && c < d && e = a + d && f = b + c && g = e ^ f && h = g & { 15 }:bv[8] && h > { 0 }:bv[8]";
		check_blasting_correctness(f);
	}

	TEST_CASE("add_commutativity_nested") {
		const char* f = "all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] all e:bv[8] all f:bv[8] all g:bv[8] all h:bv[8] ((a & b) | (c ^ d)) + ((e | f) & (g !^ h)) = ((e | f) & (g !^ h)) + ((a & b) | (c ^ d))";
		check_blasting_correctness(f);
	}

	// TEST_CASE("shift_mul_div_sat") {
	// 	const char* f = "ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] ex e:bv[8] ex f:bv[8] ex g:bv[8] ex h:bv[8] a > { 0 }:bv[8] && b = a << { 1 }:bv[8] && b > a && c = b << { 1 }:bv[8] && d = c >> { 1 }:bv[8] && d = b && e = a * { 3 }:bv[8] && f = e / { 3 }:bv[8] && f = a && g = a | b && h = g & c && h > { 0 }:bv[8]";
	// 	check_blasting_correctness(f);
	// }

}

// ---------------------------------------------------------------------------
// blasting-8bit-16var
// ---------------------------------------------------------------------------

TEST_SUITE("blasting_8bit_16var") {

	TEST_CASE("add_or_commutativity") {
		const char* f = "all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] all e:bv[8] all f:bv[8] all g:bv[8] all h:bv[8] all i:bv[8] all j:bv[8] all k:bv[8] all l:bv[8] all m:bv[8] all o:bv[8] all p:bv[8] all q:bv[8] a + b + c + d + e + f + g + h = h + g + f + e + d + c + b + a && i | j | k | l | m | o | p | q = q | p | o | m | l | k | j | i";
		check_blasting_correctness(f);
	}

	TEST_CASE("chain_ineq_gap") {
		const char* f = "ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] ex e:bv[8] ex f:bv[8] ex g:bv[8] ex h:bv[8] ex i:bv[8] ex j:bv[8] ex k:bv[8] ex l:bv[8] ex m:bv[8] ex o:bv[8] ex p:bv[8] ex q:bv[8] a < b && b < c && c < d && d < e && e < f && f < g && g < h && h < i && i < j && j < k && k < l && l < m && m < o && o < p && p < q && q - a >= { 15 }:bv[8]";
		check_blasting_correctness(f);
	}

	TEST_CASE("and_commutativity") {
		const char* f = "all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] all e:bv[8] all f:bv[8] all g:bv[8] all h:bv[8] all i:bv[8] all j:bv[8] all k:bv[8] all l:bv[8] all m:bv[8] all o:bv[8] all p:bv[8] all q:bv[8] (a & b) | (c & d) | (e & f) | (g & h) | (i & j) | (k & l) | (m & o) | (p & q) = (b & a) | (d & c) | (f & e) | (h & g) | (j & i) | (l & k) | (o & m) | (q & p)";
		check_blasting_correctness(f);
	}

	TEST_CASE("arith_composition") {
		const char* f = "ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] ex e:bv[8] ex f:bv[8] ex g:bv[8] ex h:bv[8] ex i:bv[8] ex j:bv[8] ex k:bv[8] ex l:bv[8] ex m:bv[8] ex o:bv[8] ex p:bv[8] ex q:bv[8] a > { 0 }:bv[8] && b = a * { 2 }:bv[8] && b > a && c = b + a && d = c >> { 1 }:bv[8] && e = d + a && f = e % { 7 }:bv[8] && g = f + a && h = g & a && h > { 0 }:bv[8] && i = a | b && j = i ^ c && k = j & d && l = k + e && m = l >> { 1 }:bv[8] && o = m | f && p = o ^ g && q = p & h && q > { 0 }:bv[8]";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_xnor_nand_nor_commutativity") {
		const char* f = "all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] all e:bv[8] all f:bv[8] all g:bv[8] all h:bv[8] all i:bv[8] all j:bv[8] all k:bv[8] all l:bv[8] all m:bv[8] all o:bv[8] all p:bv[8] all q:bv[8] a ^ b ^ c ^ d ^ e ^ f ^ g ^ h = h ^ g ^ f ^ e ^ d ^ c ^ b ^ a && i !^ j = j !^ i && k !& l = l !& k && m !| o = o !| m && p | q >= p && p | q >= q";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_and_sat") {
		const char* f = "ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] ex e:bv[8] ex f:bv[8] ex g:bv[8] ex h:bv[8] ex i:bv[8] ex j:bv[8] ex k:bv[8] ex l:bv[8] ex m:bv[8] ex o:bv[8] ex p:bv[8] ex q:bv[8] a < b && b < c && c < d && d < e && e < f && f < g && g < h && i = a + h && j = b + g && k = c + f && l = d + e && m = i ^ j && o = k ^ l && p = m ^ o && q = p + a && q > a";
		check_blasting_correctness(f);
	}

	TEST_CASE("or_and_order") {
		const char* f = "all a:bv[8] all b:bv[8] all c:bv[8] all d:bv[8] all e:bv[8] all f:bv[8] all g:bv[8] all h:bv[8] all i:bv[8] all j:bv[8] all k:bv[8] all l:bv[8] all m:bv[8] all o:bv[8] all p:bv[8] all q:bv[8] a | b >= a && c | d >= c && e | f >= e && g | h >= g && i & j <= i && k & l <= k && m & o <= m && p & q <= p";
		check_blasting_correctness(f);
	}

	// TEST_CASE("shift_mul_div_sat") {
	// 	const char* f = "ex a:bv[8] ex b:bv[8] ex c:bv[8] ex d:bv[8] ex e:bv[8] ex f:bv[8] ex g:bv[8] ex h:bv[8] ex i:bv[8] ex j:bv[8] ex k:bv[8] ex l:bv[8] ex m:bv[8] ex o:bv[8] ex p:bv[8] ex q:bv[8] a > { 0 }:bv[8] && b = a << { 1 }:bv[8] && b > a && c = b << { 1 }:bv[8] && d = c >> { 1 }:bv[8] && d = b && e = a * { 3 }:bv[8] && f = e / { 3 }:bv[8] && f = a && g = a | b && h = g & c && h > { 0 }:bv[8] && i = a + b && j = i % { 7 }:bv[8] && k = j + a && l = k & b && m = l | c && o = m ^ d && p = o + a && q = p >> { 1 }:bv[8] && q > { 0 }:bv[8]";
	// 	check_blasting_correctness(f);
	// }

}

// ---------------------------------------------------------------------------
// blasting-32bit-4var
// ---------------------------------------------------------------------------

TEST_SUITE("blasting_32bit_4var") {

	TEST_CASE("add_commutativity") {
		const char* f = "all a:bv[32] b:bv[32] c:bv[32] d:bv[32] (a + b) + (c + d) = (d + c) + (b + a)";
		check_blasting_correctness(f);
	}

	TEST_CASE("chain_ineq_gap") {
		const char* f = "ex a:bv[32] b:bv[32] c:bv[32] d:bv[32] a < b && b < c && c < d && d - a >= { 3 }:bv[32]";
		check_blasting_correctness(f);
	}

	TEST_CASE("distributivity") {
		const char* f = "all a:bv[32] b:bv[32] c:bv[32] d:bv[32] a & (b | c) | d = (a & b) | (a & c) | d";
		check_blasting_correctness(f);
	}

	TEST_CASE("arith_composition") {
		const char* f = "ex a:bv[32] b:bv[32] c:bv[32] d:bv[32] a > { 0 }:bv[32] && b = a * { 3 }:bv[32] && c = b + a && d = c >> { 2 }:bv[32] && d > { 0 }:bv[32]";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_assoc") {
		const char* f = "all a:bv[32] b:bv[32] c:bv[32] d:bv[32] (a ^ b) ^ c = a ^ (b ^ c) && a | d = d | a";
		check_blasting_correctness(f);
	}

	TEST_CASE("mod_existence") {
		const char* f = "ex a:bv[32] b:bv[32] c:bv[32] d:bv[32] a % { 5 }:bv[32] = { 2 }:bv[32] && b = a + { 3 }:bv[32] && c = a | b && d = c & a && d !>= b";
		check_blasting_correctness(f);
	}

	TEST_CASE("or_and_order") {
		const char* f = "all a:bv[32] b:bv[32] c:bv[32] d:bv[32] a | b >= a && a | b >= b && c & d <= c && c & d <= d";
		check_blasting_correctness(f);
	}

	TEST_CASE("shift_existence") {
		const char* f = "ex a:bv[32] b:bv[32] c:bv[32] d:bv[32] a > { 0 }:bv[32] && b = a << { 1 }:bv[32] && b > a && c = b << { 1 }:bv[32] && d = c >> { 1 }:bv[32] && d = b";
		check_blasting_correctness(f);
	}

}

// ---------------------------------------------------------------------------
// blasting-32bit-8var
// ---------------------------------------------------------------------------

TEST_SUITE("blasting_32bit_8var") {

	TEST_CASE("add_or_commutativity") {
		const char* f = "all a:bv[32] all b:bv[32] all c:bv[32] all d:bv[32] all e:bv[32] all f:bv[32] all g:bv[32] all h:bv[32] a + b + c + d = d + c + b + a && e | f | g | h = h | g | f | e";
		check_blasting_correctness(f);
	}

	TEST_CASE("chain_ineq_gap") {
		const char* f = "ex a:bv[32] ex b:bv[32] ex c:bv[32] ex d:bv[32] ex e:bv[32] ex f:bv[32] ex g:bv[32] ex h:bv[32] a < b && b < c && c < d && d < e && e < f && f < g && g < h && h - a >= { 7 }:bv[32]";
		check_blasting_correctness(f);
	}

	TEST_CASE("and_commutativity") {
		const char* f = "all a:bv[32] all b:bv[32] all c:bv[32] all d:bv[32] all e:bv[32] all f:bv[32] all g:bv[32] all h:bv[32] (a & b) | (c & d) | (e & f) | (g & h) = (b & a) | (d & c) | (f & e) | (h & g)";
		check_blasting_correctness(f);
	}

	TEST_CASE("arith_composition") {
		const char* f = "ex a:bv[32] ex b:bv[32] ex c:bv[32] ex d:bv[32] ex e:bv[32] ex f:bv[32] ex g:bv[32] ex h:bv[32] a > { 0 }:bv[32] && b = a * { 2 }:bv[32] && b > a && c = b + a && d = c >> { 1 }:bv[32] && e = d + a && f = e % { 7 }:bv[32] && g = f + a && h = g & a && h > { 0 }:bv[32]";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_xnor_nand_commutativity") {
		const char* f = "all a:bv[32] all b:bv[32] all c:bv[32] all d:bv[32] all e:bv[32] all f:bv[32] all g:bv[32] all h:bv[32] a ^ b ^ c ^ d = d ^ c ^ b ^ a && e !^ f = f !^ e && g !& h = h !& g";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_and_sat") {
		const char* f = "ex a:bv[32] ex b:bv[32] ex c:bv[32] ex d:bv[32] ex e:bv[32] ex f:bv[32] ex g:bv[32] ex h:bv[32] a < b && b < c && c < d && e = a + d && f = b + c && g = e ^ f && h = g & { 255 }:bv[32] && h > { 0 }:bv[32]";
		check_blasting_correctness(f);
	}

	TEST_CASE("add_commutativity_nested") {
		const char* f = "all a:bv[32] all b:bv[32] all c:bv[32] all d:bv[32] all e:bv[32] all f:bv[32] all g:bv[32] all h:bv[32] ((a & b) | (c ^ d)) + ((e | f) & (g !^ h)) = ((e | f) & (g !^ h)) + ((a & b) | (c ^ d))";
		check_blasting_correctness(f);
	}

	// TEST_CASE("shift_mul_div_sat") {
	// 	const char* f = "ex a:bv[32] ex b:bv[32] ex c:bv[32] ex d:bv[32] ex e:bv[32] ex f:bv[32] ex g:bv[32] ex h:bv[32] a > { 0 }:bv[32] && b = a << { 1 }:bv[32] && b > a && c = b << { 1 }:bv[32] && d = c >> { 1 }:bv[32] && d = b && e = a * { 3 }:bv[32] && f = e / { 3 }:bv[32] && f = a && g = a | b && h = g & c && h > { 0 }:bv[32]";
	// 	check_blasting_correctness(f);
	// }

}

// ---------------------------------------------------------------------------
// blasting-32bit-16var
// ---------------------------------------------------------------------------

TEST_SUITE("blasting_32bit_16var") {

	TEST_CASE("add_or_commutativity") {
		const char* f = "all a:bv[32] b:bv[32] c:bv[32] d:bv[32] e:bv[32] f:bv[32] g:bv[32] h:bv[32] i:bv[32] j:bv[32] k:bv[32] l:bv[32] m:bv[32] o:bv[32] p:bv[32] q:bv[32] a + b + c + d + e + f + g + h = h + g + f + e + d + c + b + a && i | j | k | l | m | o | p | q = q | p | o | m | l | k | j | i";
		check_blasting_correctness(f);
	}

	TEST_CASE("chain_ineq_gap") {
		const char* f = "ex a:bv[32] b:bv[32] c:bv[32] d:bv[32] e:bv[32] f:bv[32] g:bv[32] h:bv[32] i:bv[32] j:bv[32] k:bv[32] l:bv[32] m:bv[32] o:bv[32] p:bv[32] q:bv[32] a < b && b < c && c < d && d < e && e < f && f < g && g < h && h < i && i < j && j < k && k < l && l < m && m < o && o < p && p < q && q - a >= { 15 }:bv[32]";
		check_blasting_correctness(f);
	}

	TEST_CASE("and_commutativity") {
		const char* f = "all a:bv[32] b:bv[32] c:bv[32] d:bv[32] e:bv[32] f:bv[32] g:bv[32] h:bv[32] i:bv[32] j:bv[32] k:bv[32] l:bv[32] m:bv[32] o:bv[32] p:bv[32] q:bv[32] (a & b) | (c & d) | (e & f) | (g & h) | (i & j) | (k & l) | (m & o) | (p & q) = (b & a) | (d & c) | (f & e) | (h & g) | (j & i) | (l & k) | (o & m) | (q & p)";
		check_blasting_correctness(f);
	}

	TEST_CASE("arith_composition") {
		const char* f = "ex a:bv[32] b:bv[32] c:bv[32] d:bv[32] e:bv[32] f:bv[32] g:bv[32] h:bv[32] i:bv[32] j:bv[32] k:bv[32] l:bv[32] m:bv[32] o:bv[32] p:bv[32] q:bv[32] a > { 0 }:bv[32] && b = a * { 2 }:bv[32] && b > a && c = b + a && d = c >> { 1 }:bv[32] && e = d + a && f = e % { 7 }:bv[32] && g = f + a && h = g & a && h > { 0 }:bv[32] && i = a | b && j = i ^ c && k = j & d && l = k + e && m = l >> { 1 }:bv[32] && o = m | f && p = o ^ g && q = p & h && q > { 0 }:bv[32]";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_xnor_nand_nor_commutativity") {
		const char* f = "all a:bv[32] b:bv[32] c:bv[32] d:bv[32] e:bv[32] f:bv[32] g:bv[32] h:bv[32] i:bv[32] j:bv[32] k:bv[32] l:bv[32] m:bv[32] o:bv[32] p:bv[32] q:bv[32] a ^ b ^ c ^ d ^ e ^ f ^ g ^ h = h ^ g ^ f ^ e ^ d ^ c ^ b ^ a && i !^ j = j !^ i && k !& l = l !& k && m !| o = o !| m && p | q >= p && p | q >= q";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_and_sat") {
		const char* f = "ex a:bv[32] b:bv[32] c:bv[32] d:bv[32] e:bv[32] f:bv[32] g:bv[32] h:bv[32] i:bv[32] j:bv[32] k:bv[32] l:bv[32] m:bv[32] o:bv[32] p:bv[32] q:bv[32] a < b && b < c && c < d && d < e && e < f && f < g && g < h && i = a + h && j = b + g && k = c + f && l = d + e && m = i ^ j && o = k ^ l && p = m ^ o && q = p + a && q > a";
		check_blasting_correctness(f);
	}

	TEST_CASE("or_and_order") {
		const char* f = "all a:bv[32] b:bv[32] c:bv[32] d:bv[32] e:bv[32] f:bv[32] g:bv[32] h:bv[32] i:bv[32] j:bv[32] k:bv[32] l:bv[32] m:bv[32] o:bv[32] p:bv[32] q:bv[32] a | b >= a && c | d >= c && e | f >= e && g | h >= g && i & j <= i && k & l <= k && m & o <= m && p & q <= p";
		check_blasting_correctness(f);
	}

	TEST_CASE("shift_mul_div_sat") {
		const char* f = "ex a:bv[32] b:bv[32] c:bv[32] d:bv[32] e:bv[32] f:bv[32] g:bv[32] h:bv[32] i:bv[32] j:bv[32] k:bv[32] l:bv[32] m:bv[32] o:bv[32] p:bv[32] q:bv[32] a > { 0 }:bv[32] && b = a << { 1 }:bv[32] && b > a && c = b << { 1 }:bv[32] && d = c >> { 1 }:bv[32] && d = b && e = a * { 3 }:bv[32] && f = e / { 3 }:bv[32] && f = a && g = a | b && h = g & c && h > { 0 }:bv[32] && i = a + b && j = i % { 7 }:bv[32] && k = j + a && l = k & b && m = l | c && o = m ^ d && p = o + a && q = p >> { 1 }:bv[32] && q > { 0 }:bv[32]";
		check_blasting_correctness(f);
	}

}

// ---------------------------------------------------------------------------
// blasting-128bit-4var
// ---------------------------------------------------------------------------

TEST_SUITE("blasting_128bit_4var") {

	TEST_CASE("add_commutativity") {
		const char* f = "all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] (a + b) + (c + d) = (d + c) + (b + a)";
		check_blasting_correctness(f);
	}

	TEST_CASE("chain_ineq_gap") {
		const char* f = "ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] a < b && b < c && c < d && d - a >= { 3 }:bv[128]";
		check_blasting_correctness(f);
	}

	TEST_CASE("distributivity") {
		const char* f = "all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] a & (b | c) | d = (a & b) | (a & c) | d";
		check_blasting_correctness(f);
	}

	TEST_CASE("arith_composition") {
		const char* f = "ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] a > { 0 }:bv[128] && b = a * { 3 }:bv[128] && c = b + a && d = c >> { 2 }:bv[128] && d > { 0 }:bv[128]";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_assoc") {
		const char* f = "all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] (a ^ b) ^ c = a ^ (b ^ c) && a | d = d | a";
		check_blasting_correctness(f);
	}

	TEST_CASE("mod_existence") {
		const char* f = "ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] a % { 5 }:bv[128] = { 2 }:bv[128] && b = a + { 3 }:bv[128] && c = a | b && d = c & a && d !>= b";
		check_blasting_correctness(f);
	}

	TEST_CASE("or_and_order") {
		const char* f = "all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] a | b >= a && a | b >= b && c & d <= c && c & d <= d";
		check_blasting_correctness(f);
	}

	TEST_CASE("shift_existence") {
		const char* f = "ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] a > { 0 }:bv[128] && b = a << { 1 }:bv[128] && b > a && c = b << { 1 }:bv[128] && d = c >> { 1 }:bv[128] && d = b";
		check_blasting_correctness(f);
	}

}

// ---------------------------------------------------------------------------
// blasting-128bit-8var
// ---------------------------------------------------------------------------

TEST_SUITE("blasting_128bit_8var") {

	TEST_CASE("add_or_commutativity") {
		const char* f = "all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] all e:bv[128] all f:bv[128] all g:bv[128] all h:bv[128] a + b + c + d = d + c + b + a && e | f | g | h = h | g | f | e";
		check_blasting_correctness(f);
	}

	TEST_CASE("chain_ineq_gap") {
		const char* f = "ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] ex e:bv[128] ex f:bv[128] ex g:bv[128] ex h:bv[128] a < b && b < c && c < d && d < e && e < f && f < g && g < h && h - a >= { 7 }:bv[128]";
		check_blasting_correctness(f);
	}

	TEST_CASE("and_commutativity") {
		const char* f = "all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] all e:bv[128] all f:bv[128] all g:bv[128] all h:bv[128] (a & b) | (c & d) | (e & f) | (g & h) = (b & a) | (d & c) | (f & e) | (h & g)";
		check_blasting_correctness(f);
	}

	TEST_CASE("arith_composition") {
		const char* f = "ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] ex e:bv[128] ex f:bv[128] ex g:bv[128] ex h:bv[128] a > { 0 }:bv[128] && b = a * { 2 }:bv[128] && b > a && c = b + a && d = c >> { 1 }:bv[128] && e = d + a && f = e % { 7 }:bv[128] && g = f + a && h = g & a && h > { 0 }:bv[128]";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_xnor_nand_commutativity") {
		const char* f = "all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] all e:bv[128] all f:bv[128] all g:bv[128] all h:bv[128] a ^ b ^ c ^ d = d ^ c ^ b ^ a && e !^ f = f !^ e && g !& h = h !& g";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_and_sat") {
		const char* f = "ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] ex e:bv[128] ex f:bv[128] ex g:bv[128] ex h:bv[128] a < b && b < c && c < d && e = a + d && f = b + c && g = e ^ f && h = g & { 255 }:bv[128] && h > { 0 }:bv[128]";
		check_blasting_correctness(f);
	}

	TEST_CASE("add_commutativity_nested") {
		const char* f = "all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] all e:bv[128] all f:bv[128] all g:bv[128] all h:bv[128] ((a & b) | (c ^ d)) + ((e | f) & (g !^ h)) = ((e | f) & (g !^ h)) + ((a & b) | (c ^ d))";
		check_blasting_correctness(f);
	}

	// TEST_CASE("shift_mul_div_sat") {
	// 	const char* f = "ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] ex e:bv[128] ex f:bv[128] ex g:bv[128] ex h:bv[128] a > { 0 }:bv[128] && b = a << { 1 }:bv[128] && b > a && c = b << { 1 }:bv[128] && d = c >> { 1 }:bv[128] && d = b && e = a * { 3 }:bv[128] && f = e / { 3 }:bv[128] && f = a && g = a | b && h = g & c && h > { 0 }:bv[128]";
	// 	check_blasting_correctness(f);
	// }

}

// ---------------------------------------------------------------------------
// blasting-128bit-16var
// ---------------------------------------------------------------------------

TEST_SUITE("blasting_128bit_16var") {

	TEST_CASE("add_or_commutativity") {
		const char* f = "all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] all e:bv[128] all f:bv[128] all g:bv[128] all h:bv[128] all i:bv[128] all j:bv[128] all k:bv[128] all l:bv[128] all m:bv[128] all o:bv[128] all p:bv[128] all q:bv[128] a + b + c + d + e + f + g + h = h + g + f + e + d + c + b + a && i | j | k | l | m | o | p | q = q | p | o | m | l | k | j | i";
		check_blasting_correctness(f);
	}

	TEST_CASE("chain_ineq_gap") {
		const char* f = "ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] ex e:bv[128] ex f:bv[128] ex g:bv[128] ex h:bv[128] ex i:bv[128] ex j:bv[128] ex k:bv[128] ex l:bv[128] ex m:bv[128] ex o:bv[128] ex p:bv[128] ex q:bv[128] a < b && b < c && c < d && d < e && e < f && f < g && g < h && h < i && i < j && j < k && k < l && l < m && m < o && o < p && p < q && q - a >= { 15 }:bv[128]";
		check_blasting_correctness(f);
	}

	TEST_CASE("and_commutativity") {
		const char* f = "all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] all e:bv[128] all f:bv[128] all g:bv[128] all h:bv[128] all i:bv[128] all j:bv[128] all k:bv[128] all l:bv[128] all m:bv[128] all o:bv[128] all p:bv[128] all q:bv[128] (a & b) | (c & d) | (e & f) | (g & h) | (i & j) | (k & l) | (m & o) | (p & q) = (b & a) | (d & c) | (f & e) | (h & g) | (j & i) | (l & k) | (o & m) | (q & p)";
		check_blasting_correctness(f);
	}

	TEST_CASE("arith_composition") {
		const char* f = "ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] ex e:bv[128] ex f:bv[128] ex g:bv[128] ex h:bv[128] ex i:bv[128] ex j:bv[128] ex k:bv[128] ex l:bv[128] ex m:bv[128] ex o:bv[128] ex p:bv[128] ex q:bv[128] a > { 0 }:bv[128] && b = a * { 2 }:bv[128] && b > a && c = b + a && d = c >> { 1 }:bv[128] && e = d + a && f = e % { 7 }:bv[128] && g = f + a && h = g & a && h > { 0 }:bv[128] && i = a | b && j = i ^ c && k = j & d && l = k + e && m = l >> { 1 }:bv[128] && o = m | f && p = o ^ g && q = p & h && q > { 0 }:bv[128]";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_xnor_nand_nor_commutativity") {
		const char* f = "all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] all e:bv[128] all f:bv[128] all g:bv[128] all h:bv[128] all i:bv[128] all j:bv[128] all k:bv[128] all l:bv[128] all m:bv[128] all o:bv[128] all p:bv[128] all q:bv[128] a ^ b ^ c ^ d ^ e ^ f ^ g ^ h = h ^ g ^ f ^ e ^ d ^ c ^ b ^ a && i !^ j = j !^ i && k !& l = l !& k && m !| o = o !| m && p | q >= p && p | q >= q";
		check_blasting_correctness(f);
	}

	TEST_CASE("xor_and_sat") {
		const char* f = "ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] ex e:bv[128] ex f:bv[128] ex g:bv[128] ex h:bv[128] ex i:bv[128] ex j:bv[128] ex k:bv[128] ex l:bv[128] ex m:bv[128] ex o:bv[128] ex p:bv[128] ex q:bv[128] a < b && b < c && c < d && d < e && e < f && f < g && g < h && i = a + h && j = b + g && k = c + f && l = d + e && m = i ^ j && o = k ^ l && p = m ^ o && q = p + a && q > a";
		check_blasting_correctness(f);
	}

	TEST_CASE("or_and_order") {
		const char* f = "all a:bv[128] all b:bv[128] all c:bv[128] all d:bv[128] all e:bv[128] all f:bv[128] all g:bv[128] all h:bv[128] all i:bv[128] all j:bv[128] all k:bv[128] all l:bv[128] all m:bv[128] all o:bv[128] all p:bv[128] all q:bv[128] a | b >= a && c | d >= c && e | f >= e && g | h >= g && i & j <= i && k & l <= k && m & o <= m && p & q <= p";
		check_blasting_correctness(f);
	}

	// TEST_CASE("shift_mul_div_sat") {
	// 	const char* f = "ex a:bv[128] ex b:bv[128] ex c:bv[128] ex d:bv[128] ex e:bv[128] ex f:bv[128] ex g:bv[128] ex h:bv[128] ex i:bv[128] ex j:bv[128] ex k:bv[128] ex l:bv[128] ex m:bv[128] ex o:bv[128] ex p:bv[128] ex q:bv[128] a > { 0 }:bv[128] && b = a << { 1 }:bv[128] && b > a && c = b << { 1 }:bv[128] && d = c >> { 1 }:bv[128] && d = b && e = a * { 3 }:bv[128] && f = e / { 3 }:bv[128] && f = a && g = a | b && h = g & c && h > { 0 }:bv[128] && i = a + b && j = i % { 7 }:bv[128] && k = j + a && l = k & b && m = l | c && o = m ^ d && p = o + a && q = p >> { 1 }:bv[128] && q > { 0 }:bv[128]";
	// 	check_blasting_correctness(f);
	// }

}
