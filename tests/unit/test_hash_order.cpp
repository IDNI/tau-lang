// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#include "normalizer.h"

// The tree order compares node hashes first, so the printed order of a
// normalized formula follows them. The pins hold on every platform.
#if TAU_USE_PORTABLE_HASH

namespace {

struct hash_pin {
	const char* src;
	std::uint64_t hash;
};

struct print_pin {
	const char* src;
	const char* printed;
};

void check_hashes(const std::vector<hash_pin>& pins) {
	for (const auto& p : pins) {
		tref fm = tau::get(p.src, parse_wff()).value_or(nullptr);
		REQUIRE(fm != nullptr);
		INFO(std::string(p.src));
		CHECK_EQ(tau::get(fm).hash, p.hash);
	}
}

void check_prints(const std::vector<print_pin>& pins) {
	for (const auto& p : pins) {
		tref fm = tau::get(p.src, parse_wff()).value_or(nullptr);
		REQUIRE(fm != nullptr);
		auto nr = normalize_non_temp<node_t>(fm);
		REQUIRE(nr.has_value());
		INFO(std::string(p.src));
		CHECK_EQ(tau::get(nr.value()).to_str(), std::string(p.printed));
	}
}

} // namespace

TEST_SUITE("hash order") {

	TEST_CASE("untyped names") {
		check_hashes({
			{ "a = 0 && b = 0 && c = 0 && d = 0", 16603262042375972409ull },
			{ "x | y | z | w = 0", 5216481979381744574ull },
		});
		check_prints({
			{ "a = 0 && b = 0 && c = 0 && d = 0 && e = 0 && f = 0",
				"a = 0 && f = 0 && c = 0 && d = 0 && e = 0 && b = 0" },
			{ "p = 0 || q = 0 || r = 0 || s = 0 || t = 0 || u = 0",
				"q = 0 || s = 0 || u = 0 || r = 0 || p = 0 || t = 0" },
		});
	}

#ifdef TAU_PACK_HAS_BA_SBF
	TEST_CASE("sbf") {
		check_hashes({
			{ "x:sbf = {a | b}:sbf && y:sbf = {c & d}:sbf", 1749031699279516309ull },
		});
		check_prints({
			{ "x:sbf = 0 || y:sbf = 0 || z:sbf = 0 || w:sbf = 0",
				"x = 0 || y = 0 || z = 0 || w = 0" },
		});
	}
#endif

#ifdef TAU_PACK_HAS_BA_BV
	TEST_CASE("bv") {
		check_hashes({
			{ "x:bv[8] = { #x07 }:bv[8] && y:bv[16] = { #x002a }:bv[16]", 1706221739210031787ull },
		});
		check_prints({
			{ "x:bv[8] = { #x07 }:bv[8] || y:bv[8] = { #x2a }:bv[8]"
				" || z:bv[8] = { #xff }:bv[8]",
				"y = { 42 }:bv[8] || x = { 7 }:bv[8] || z' = 0" },
		});
	}
#endif

#ifdef TAU_PACK_HAS_BA_QLT
	TEST_CASE("qlt") {
		check_hashes({
			{ "x:qlt > {0}:qlt && y:qlt < {1}:qlt", 12917797719874598464ull },
		});
		check_prints({
			{ "x:qlt > {0}:qlt && y:qlt < {1}:qlt && z:qlt > {2}:qlt",
				"y < { 1 }:qlt && { 2 }:qlt < z && { 0 }:qlt < x" },
		});
	}
#endif

#ifdef TAU_PACK_HAS_BA_QINT
	TEST_CASE("qint") {
		check_hashes({
			{ "x:qint = {[0, 1)}:qint && y:qint = {[2, 3)}:qint", 18416050101415192169ull },
		});
		check_prints({
			{ "x:qint = {[0, 1)}:qint || y:qint = {[2, 3)}:qint"
				" || z:qint = {[4, 5)}:qint",
				"x = { [0, 1) }:qint || z = { [4, 5) }:qint || y = { [2, 3) }:qint" },
		});
	}
#endif

}

#endif // TAU_USE_PORTABLE_HASH
