// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// codegen_witness contract: the emitted witness is a self-contained tref
// expression built from an exact qlt_rational(p, q), never a rounded double.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "cpp_codegen.h"
#include "ltl_aba.h"

#include <optional>
#include <sstream>
#include <string>

using namespace idni::tau_lang;

namespace {

std::optional<ltl_aba_solution<node_t>> synth(const std::string& spec) {
	tref fm = api<node_t>::get_formula(spec);
	if (!fm) return std::nullopt;
	return solve_ltl_aba<node_t>(fm);
}

bool has(const std::string& s, const std::string& pat) {
	return s.find(pat) != std::string::npos;
}

// Extract the (p, q) integer components of the LAST `qlt_rational(p, q)`
// factory expression -- see qlt_codegen.tmpl.h's qlt_witness_expr. Earlier
// occurrences belong to program_desc::atoms' ground-constant rendering
// (emitted ahead of step()); the witness's own is emitted inside step().
bool extract_qlt_rational(const std::string& s, long long& p, long long& q) {
	static const std::string tag = "qlt_rational(";
	auto pos = s.rfind(tag);
	if (pos == std::string::npos) return false;
	pos += tag.size();
	auto comma = s.find(',', pos);
	if (comma == std::string::npos) return false;
	auto close = s.find(')', comma);
	if (close == std::string::npos) return false;
	try {
		p = std::stoll(s.substr(pos, comma - pos));
		q = std::stoll(s.substr(comma + 1, close - comma - 1));
	} catch (...) { return false; }
	return q != 0;
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
	TEST_CASE("narrow interval: witness is an exact qlt_rational strictly inside it") {
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
}


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
