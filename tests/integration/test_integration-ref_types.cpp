// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_Bool_helpers.h"

bool test_ref_types(const char* rec_relation,
	const std::string& expected_error = "", bool expect_fail = false)
{
	using node = node_t;
	DBG(LOG_INFO << "Ref type inference test input: `"
		<< rec_relation << "`");
	auto nso_rr = get_nso_rr(rec_relation, true);
	if (!nso_rr.has_value()) return false;
	DBG(LOG_INFO << "Infer types for:\n" << LOG_RR(nso_rr.value());)

	ref_types<node> ts(nso_rr.value());
	auto nso_rr_inferred = infer_ref_types<node>(nso_rr.value(), ts);
	bool fail = !ts.ok();

	if (!expect_fail) expect_fail = expected_error != "";
	if (fail != expect_fail) {
		LOG_ERROR << (fail ? "failed" : "success") <<" but expected to "
			<< (expect_fail ? "fail" : "be successful") << ". # "
			<< expected_error;
		LOG_TRACE << "for spec: " << LOG_RR(nso_rr.value());
	}
	if (fail) { // type checking or inference error 
		if (expected_error == "Unknown") {
			if (ts.unresolved().size()) {
				LOG_ERROR << "Unresolved not empty";
				return expect_fail;
			} else {
				LOG_ERROR << "Unresolved empty";
				return false;
			}
		}
		for (auto& e : ts.errors())
			if (e.find(expected_error) != std::string::npos)
				return expect_fail;
		return false;
	} else {
		tref normalized = normalizer<node>(nso_rr_inferred.value());
		if (normalized == nullptr) {
			LOG_ERROR << "normalizer failed\n";
			return expect_fail;
		} else if (!tau::get(normalized).child_is(tau::wff_t)) {
			LOG_ERROR << "not T " << expect_fail << "\n";
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

	// TEST_CASE("fp call over inferred refs (loop)") { CHECK( test_ref_types(
	// 	"w[0](x, y) := T."
	// 	"q[0](x, y) := w[0](x, y)."
	// 	"q[n](x, y) := !q[n-1](x, y)."
	// 	"!q(0, 0)."
	// ) ); }

	// TEST_CASE("fp call over inferred refs, incl. bf") { CHECK( test_ref_types(
	// 	"f[0](x) := x."
	// 	"f[n](x) := f[n-1](x)'."
	// 	"f[0](x, y) := T."
	// 	"f[n](x, y) := f[n-1](x, y) && f[n-1](x) = 1."
	// 	"f(1, 0)."
	// ) ); }

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
