// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// codegen_witness contract: the emitted witness is a self-contained tref
// expression built from an exact qlt_rational(p, q), never a rounded double.

#include "test_init.h"
#include "test_codegen_helpers.h"
#include "cpp_codegen.h"
#include "ltl_aba.h"

#include <filesystem>
#include <fstream>
#include <limits>
#include <optional>
#include <sstream>
#include <string>

using namespace idni::tau_lang;

namespace {

// The witness the pack spells for the only variable of @p atom.
std::optional<std::string> witness_of(const char* atom) {
	tref fm = parse_wff_or_fail(atom);
	REQUIRE(fm != nullptr);
	const auto& fv = get_free_vars<node_t>(fm);
	REQUIRE(fv.size() == 1);
	return pack_codegen_witness<node_t>(
		tree<node_t>::get(fv[0]).get_ba_type(), fv[0], fm);
}

} // namespace

TEST_SUITE("qlt_codegen") {

	// A narrow interval (width 1e-6) whose exact midpoint the old contract
	// would have pushed through a C++ double literal ((double)p/(double)q
	// then %.17g) -- the round-trip that can place a rounded value outside a
	// narrow interval even though the source rational was exactly inside it.
	// The new contract removes that structurally: the witness is an exact
	// qlt_rational(p, q) embedded as integer literals, so it satisfies the
	// interval by construction, not by luck of the rounding.
	TEST_CASE("narrow interval: witness is an exact qlt_rational strictly inside it"
		* doctest::skip(!ltlsynt_available())) {
		auto sol = synth(
			"G(o1[t]:qlt > {1000001/2000000}:qlt "
			"&& o1[t]:qlt < {1000003/2000000}:qlt)");
		REQUIRE(sol.has_value());
		auto d = build_program_desc<node_t>(*sol);
		REQUIRE(d.has_value());
		CHECK(d->needs_tau_link);
		std::ostringstream os;
		emit_program(*d, os);
		std::string s = os.str();

		// The old contract emitted a bare double literal (something like
		// "0.50000099999999995" from %.17g) with no "qlt_rational(" text at
		// all -- this assertion alone would reject that output.
		REQUIRE(has(s, "qlt_rational("));

		long long p = 0, q = 0;
		REQUIRE(extract_qlt_rational(s, p, q));
		REQUIRE(q > 0);
		// lo = 1000001/2000000 < p/q < hi = 1000003/2000000, checked by
		// exact integer cross-multiplication -- no floating point at all.
		CHECK(p * 2000000 > 1000001LL * q);
		CHECK(p * 2000000 < 1000003LL * q);
	}

	TEST_CASE("the least long long is spelled without an out-of-range literal") {
		CHECK(qlt_ll_literal(std::numeric_limits<long long>::min())
			== "(-9223372036854775807LL - 1)");
		CHECK(qlt_ll_literal(-5) == "-5LL");
		CHECK(qlt_ll_literal(7) == "7LL");
		std::string s = qlt_witness_expr<node_t>(qlt_rational(
			std::numeric_limits<long long>::min(), 1));
		CHECK(has(s, "qlt_rational((-9223372036854775807LL - 1), 1LL)"));
		CHECK_FALSE(has(s, "-9223372036854775808"));
	}

	TEST_CASE("the emitted literals compile warning-free under -Werror"
		* doctest::skip(!tau_test_cxx_available(tau_test_cxx())
			|| tau_test_cxx_is_msvc(tau_test_cxx()))) {
		namespace fs = std::filesystem;
		auto dir = tau_test_tmp("qlt_literal");
		auto src = dir / "lit.cpp";
		{
			std::ofstream f(src);
			f << "#include <climits>\n"
			  << "static_assert(" << qlt_ll_literal(LLONG_MIN)
			  << " == LLONG_MIN);\n"
			  << "static_assert(" << qlt_ll_literal(LLONG_MAX)
			  << " == LLONG_MAX);\n"
			  << "static_assert(" << qlt_ll_literal(-3)
			  << " == -3);\nint main() {}\n";
		}
		auto run = tau_test_run({ tau_test_cxx(), "-std=c++17", "-Wall",
			"-Wextra", "-Werror", "-fsyntax-only", src.string() });
		CHECK_MESSAGE(run.exit_code == 0, run.out << run.err);
	}

	TEST_CASE("a ray with a representable point yields it") {
		auto w = witness_of("o1[t]:qlt < {-9223372036854775806}:qlt");
		REQUIRE(w.has_value());
		CHECK(has(*w, "qlt_rational(-9223372036854775807LL, 1LL)"));
	}

	// No rational the representation fits lies below the least long long,
	// nor strictly between these two reciprocals, whose midpoint has a
	// denominator past long long: the witness is declined, never a value
	// outside the constraint.
	TEST_CASE("an open ray below the least long long has no witness") {
		auto w = witness_of("o1[t]:qlt < {-9223372036854775808}:qlt");
		CHECK_MESSAGE(!w.has_value(), w.value_or(""));
	}

	TEST_CASE("a gap whose midpoint does not fit has no witness") {
		auto w = witness_of("o1[t]:qlt > {1/4000000009}:qlt "
			"&& o1[t]:qlt < {1/4000000007}:qlt");
		CHECK_MESSAGE(!w.has_value(), w.value_or(""));
	}

	TEST_CASE("an output with no witness is unsupported, not a wrong value"
		* doctest::skip(!ltlsynt_available())) {
		auto sol = synth("G(o1[t]:qlt > {1/4000000009}:qlt "
			"&& o1[t]:qlt < {1/4000000007}:qlt)");
		REQUIRE(sol.has_value());
		auto d = build_program_desc<node_t>(*sol);
		CHECK_FALSE(d.has_value());
		CHECK(report_has_code(d.report(), code::unsupported_operation));
	}
}
