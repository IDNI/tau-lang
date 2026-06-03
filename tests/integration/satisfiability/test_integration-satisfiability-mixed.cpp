// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_integration-satisfiability_helper.h"

TEST_SUITE("Configuration") {
	TEST_CASE("logging") {
		// logging::trace();
	}
	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("Mixed Boolean algebras specs") {
	TEST_CASE("simple always: no loopback") {
		tref spec = create_spec("(always o1[t]:bv[4] = { 1 } && o2[t] = 1).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("simple always: loopback in tau") {
		tref spec = create_spec("(always o1[t]:bv[4] = { 1 } && o2[t-2] = o2[t]).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("simple always: loopback in bv") {
		tref spec = create_spec("(always o1[t]:bv[4] = o1[t-2] && o2[t] = 1).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("simple always: loopback in bv and tau") {
		tref spec = create_spec("(always o1[t]:bv[4] = o1[t-2] && o2[t] = o2[t]).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("simple always: constant position") {
		tref spec = create_spec("(always o1[0]:bv[4] = { 1 } && o2[0] = 1).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("simple sometimes: no loopback") {
		tref spec = create_spec("(sometimes o1[t]:bv[4] = { 0 } || o2[t] = 0).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("simple sometimes: loopback in tau") {
		tref spec = create_spec("(sometimes o1[t]:bv[4] = { 0 } || o2[t-2] = o2[t]).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("simple sometimes: loopback in bv") {
		tref spec = create_spec("(sometimes o1[t]:bv[4] = o1[t-2] || o2[t] = 0).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("simple sometimes: loopback in bv and tau") {
		tref spec = create_spec("(sometimes o1[t]:bv[4] = o1[t-2] || o2[t] = o2[t]).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("mixing always and sometimes: sat") {
		tref spec = create_spec("(always o1[t]:bv[4] = o1[t-1] && o2[t] = o2[t-1]) || (sometimes o1[t]:bv[4] = { 1 } && o2[t] = 1).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("mixing always and sometimes: unsat") {
		tref spec = create_spec("(always o1[t]:bv[4] = { 1 } && o2[t] = 1) && (sometimes o1[t-1]:bv[4] = { 0 } || o2[t-1] = 0).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Mixed: qlt with Bool") {
	TEST_CASE("qlt and Bool always sat") {
		tref spec = create_spec("(always o1[t]:qlt > {0}:qlt && o2[t] = 1).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("qlt and Bool unsat from qlt contradiction") {
		tref spec = create_spec("(always o1[t]:qlt > {3/4}:qlt && o1[t]:qlt < {1/4}:qlt && o2[t] = 1).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("qlt positive sometimes and Bool sometimes sat") {
		tref spec = create_spec("(sometimes o1[t]:qlt > {0}:qlt) && (sometimes o2[t] = 0).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Mixed: qlt with bv") {
	TEST_CASE("qlt and bv[4] always sat") {
		tref spec = create_spec("(always o1[t]:qlt = {1/2}:qlt && o2[t]:bv[4] = { 1 }).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("qlt contradiction with bv[4] unsat") {
		tref spec = create_spec("(always o1[t]:qlt > {3/4}:qlt) && (always o1[t]:qlt < {1/4}:qlt) && (always o2[t]:bv[4] = { 1 }).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Mixed: hsb with Bool") {
	TEST_CASE("hsb top and Bool always sat") {
		tref spec = create_spec("(always o1[t]:hsb = {top}:hsb && o2[t] = 1).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("hsb conflict with Bool sometimes unsat") {
		tref spec = create_spec("(always o1[t]:hsb = {top}:hsb) && (always o1[t]:hsb = {bot}:hsb) && (sometimes o2[t] = 1).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Mixed: qint with Bool") {
	TEST_CASE("qint interval and Bool always sat") {
		tref spec = create_spec("(always o1[t]:qint = {[0, 1)}:qint && o2[t] = 0).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("qint contradiction with Bool unsat") {
		tref spec = create_spec("(always o1[t]:qint = {[0, 1/2)}:qint) && (always o1[t]:qint = {[1/2, 1)}:qint) && (always o2[t] = 1).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Mixed: qlt, hsb and bv") {
	TEST_CASE("qlt, hsb and bv[4] three streams sat") {
		tref spec = create_spec("(always o1[t]:qlt = {1/2}:qlt && o2[t]:hsb = {top}:hsb && o3[t]:bv[4] = { 1 }).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("qlt, hsb and bv[4] qlt contradiction unsat") {
		tref spec = create_spec("(always o1[t]:qlt > {3/4}:qlt) && (always o1[t]:qlt < {1/4}:qlt) && (always o2[t]:hsb = {top}:hsb) && (always o3[t]:bv[4] = { 1 }).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Mixed: tau with sbf") {
	TEST_CASE("tau and sbf always sat") {
		tref spec = create_spec("(always o1[t] = 1 && o2[t]:sbf = 1).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("sbf contradiction with tau unsat") {
		tref spec = create_spec("(always o2[t]:sbf = 0) && (always o2[t]:sbf = 1) && (always o1[t] = 1).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Mixed: sbf with qint") {
	TEST_CASE("sbf and qint always sat") {
		tref spec = create_spec("(always o1[t]:sbf = 1 && o2[t]:qint = {[0, 1)}:qint).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("sbf contradiction with qint unsat") {
		tref spec = create_spec("(always o1[t]:sbf = 1) && (sometimes o1[t]:sbf = 0) && (always o2[t]:qint = {[0, 1)}:qint).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Mixed: sbf with hsb") {
	TEST_CASE("sbf and hsb always sat") {
		tref spec = create_spec("(always o1[t]:sbf = 1 && o2[t]:hsb = {top}:hsb).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("sbf contradiction with hsb unsat") {
		tref spec = create_spec("(always o1[t]:sbf = 1) && (sometimes o1[t]:sbf = 0) && (always o2[t]:hsb = {top}:hsb).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Mixed: sbf with bv") {
	TEST_CASE("sbf and bv[4] always sat") {
		tref spec = create_spec("(always o1[t]:sbf = 1 && o2[t]:bv[4] = { 1 }).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("sbf contradiction with bv unsat") {
		tref spec = create_spec("(always o1[t]:sbf = 1) && (sometimes o1[t]:sbf = 0) && (always o2[t]:bv[4] = { 1 }).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Mixed: qint with hsb") {
	TEST_CASE("qint and hsb always sat") {
		tref spec = create_spec("(always o1[t]:qint = {[0, 1)}:qint && o2[t]:hsb = {top}:hsb).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("qint contradiction with hsb unsat") {
		tref spec = create_spec("(always o1[t]:qint = {[0, 1/2)}:qint) && (always o1[t]:qint = {[1/2, 1)}:qint) && (always o2[t]:hsb = {top}:hsb).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Mixed: qint with bv") {
	TEST_CASE("qint and bv[4] always sat") {
		tref spec = create_spec("(always o1[t]:qint = {[0, 1)}:qint && o2[t]:bv[4] = { 1 }).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("qint contradiction with bv unsat") {
		tref spec = create_spec("(always o1[t]:qint = {[0, 1/2)}:qint) && (always o1[t]:qint = {[1/2, 1)}:qint) && (always o2[t]:bv[4] = { 1 }).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Mixed: hsb with bv") {
	TEST_CASE("hsb and bv[4] always sat") {
		tref spec = create_spec("(always o1[t]:hsb = {top}:hsb && o2[t]:bv[4] = { 1 }).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("hsb contradiction with bv unsat") {
		tref spec = create_spec("(always o1[t]:hsb = {top}:hsb) && (always o1[t]:hsb = {bot}:hsb) && (always o2[t]:bv[4] = { 1 }).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Mixed: all five BAs (sbf, tau, qint, hsb, bv)") {
	TEST_CASE("all five BAs independent constraints sat") {
		tref spec = create_spec("(always o1[t]:sbf = 1 && o2[t] = 1 && o3[t]:qint = {[0, 1)}:qint && o4[t]:hsb = {top}:hsb && o5[t]:bv[4] = { 1 }).");
		CHECK(is_tau_formula_sat<node_t>(spec));
	}
	TEST_CASE("sbf contradiction in presence of all five BAs unsat") {
		tref spec = create_spec("(always o1[t]:sbf = 1) && (sometimes o1[t]:sbf = 0) && (always o2[t] = 1) && (always o3[t]:qint = {[0, 1)}:qint) && (always o4[t]:hsb = {top}:hsb) && (always o5[t]:bv[4] = { 1 }).");
		CHECK(!is_tau_formula_sat<node_t>(spec));
	}
}

TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
