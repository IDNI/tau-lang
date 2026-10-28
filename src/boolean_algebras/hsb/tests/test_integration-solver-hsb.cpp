// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#ifdef DEBUG // in release it is included with tau.h
#	include "solver.h"
#endif
#include "test_integration-solver_ba_suite.h"

struct hsb_solver_ba {
	static tref type() { return hsb_type<node_t>(); }
	static constexpr const char* name = "hsb";
	static constexpr const char* a = "{x[0] < 0}:hsb";
	static constexpr const char* b = "{x[1] < 0}:hsb";
	static constexpr const char* x = "a";
	static constexpr const char* y = "b";
};

TEST_SUITE("test helpers") {

	TEST_CASE("no solution means code::unsat and no other error") {
		result<bool> unsat;
		(void)unsat.with_error(code::unsat, "no solution");
		CHECK( report_fails_only_with(unsat.report(), code::unsat) );
		(void)unsat.with_error(code::internal_error, "internal error");
		CHECK( !report_fails_only_with(unsat.report(), code::unsat) );
		result<bool> internal;
		(void)internal.with_error(code::internal_error, "internal error");
		CHECK( !report_fails_only_with(internal.report(), code::unsat) );
		result<bool> ok;
		(void)ok.with_value(true);
		CHECK( !report_fails_only_with(ok.report(), code::unsat) );
		CHECK( !test_no_solution("a : hsb != 0.", hsb_type<node_t>()) );
	}
}

TAU_SOLVER_BA_SUITE(hsb_solver_ba);
