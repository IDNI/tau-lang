// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

// #include "init_log.h"

#include "doctest.h"

#include "test_integration_helpers.h"
#include "../unit/test_helpers_depreciating.h"

using namespace idni::depreciating::rewriter;
using namespace idni::tau_lang;

namespace testing = doctest;

bool test_ref_types(const char* rec_relation,
	const std::string& expected_error = "", bool expect_fail = false)
{
	// boost::log::core::get()->set_filter(
	// 	boost::log::trivial::severity >= boost::log::trivial::trace);

	auto source = make_tau_source(rec_relation, { .start = tau_parser::rr });
	auto code = make_tau_code<sbf_ba>(source);
	factory_binder<sbf_ba> fb;
	auto binded = bind_tau_code_using_binder<
				factory_binder<sbf_ba>, sbf_ba>(code, fb);
	if (!binded) return false;

	// auto nt = binded | non_terminal_extractor<sbf_ba>
	// 	| optional_value_extractor<size_t>;
	// std::cerr << "nt: " << tau_parser::instance().name(nt) << "\n";

	auto main = binded | tau_parser::main
		| tau_parser::wff | optional_value_extractor<tau_depreciating<sbf_ba>>;
	auto nso_rr = rr<tau_depreciating<sbf_ba>>{ make_rec_relations<sbf_ba>(binded), main };

	rr_types<sbf_ba> ts(nso_rr);
	auto nso_rr_opt = infer_ref_types<sbf_ba>(nso_rr, ts);
	bool fail = !ts.ok();

	if (!expect_fail) expect_fail = expected_error != "";
	// std::cerr << "fail: " << fail << " expect_fail: " << expect_fail << "\n";
	if (fail) { // type checking or inference error 
		if (expected_error == "Unknown") {
			if (ts.unresolved().size()) {
				// std::cerr << "Unresolved not empty\n";
				return expect_fail;
			} else {
				// std::cerr << "Unresolved empty\n";
				return false;
			}
		}
		for (auto& e : ts.errors())
			if (e.find(expected_error) != std::string::npos)
				return expect_fail;
		return false;
	} else {
		auto normalized = normalizer<sbf_ba>(nso_rr_opt.value());
		if (normalized == nullptr) {
			// std::cerr << "normalizer failed\n";
			return expect_fail;
		} else if (!(normalized | tau_parser::wff_t).has_value()) {
			// std::cerr << "not T " << expect_fail << "\n";
			return expect_fail;
		}
	}
	return !expect_fail;
}

TEST_SUITE("Boolean function recurrence relation fixed point calculation") {

	TEST_CASE("arities make different signature") { CHECK( test_ref_types(
		"f[0](x) := T."
		"f[0](x, y) := 1."
		"g[0](x) := F."
		"g[0, 0](x) := 0."
		"f[0](x) && !g[0](x) && f[0](x, y) != g[0, 0](x)."
	) ); }

	TEST_CASE("fp call over inferred refs (loop)") { CHECK( test_ref_types(
		"w[0](x, y) := T."
		"q[0](x, y) := w[0](x, y)."
		"q[n](x, y) := !q[n-1](x, y)."
		"!q(0, 0)."
	) ); }

	TEST_CASE("fp call over inferred refs, incl. bf") { CHECK( test_ref_types(
		"f[0](x) := x."
		"f[n](x) := f[n-1](x)'."
		"f[0](x, y) := T."
		"f[n](x, y) := f[n-1](x, y) && f[n-1](x) = 1."
		"f(1, 0)."
	) ); }

	TEST_CASE("type mismatch 1") { CHECK( test_ref_types(
		"e[0, 0](x) := 1."
		"e[1, 0](x) := T."
		"F.", "Type mismatch"
	) ); }

	TEST_CASE("type mismatch 2") { CHECK( test_ref_types(
		"f[0](x, y) := 1."
		"f[1](x, y) := T."
		"F.", "Type mismatch"
	) ); }

	TEST_CASE("unsupported multiindex offset arity fp call") { CHECK( test_ref_types(
		"g[0, 0](x) := T."
		"g[m, n](x) := !g[m-1, n-1](x)."
		"g(0).", "", true 
	) ); }

	TEST_CASE("typo undetected") { CHECK( test_ref_types(
		"f[0](x, y) := T."
		"f[n](x) := f[n-1](x, y)."
		"f[1](0, 0).", "", true
	) ); }

	TEST_CASE("typo unresolved 1") { CHECK( test_ref_types(
		"f[0](x) := T."
		"f[n](x) := f[n-1](x, 0)."
		"f[1](0).", "Unknown"
	) ); }

	TEST_CASE("typo unresolved 2") { CHECK( test_ref_types(
		"f[0](x) := T."
		"f[n](x) := f[m, n-1](x)."
		"f[1](0).", "Unknown"
	) ); }


}
