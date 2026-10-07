// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#include "boolean_algebras/bv/bv_ba.h"

#include <random>

namespace {

// cvc5 takes a sort width as a 32-bit value; the widths here stay a few bits
cvc5::Sort bv_sort(size_t w) {
	DBG(assert(w >= 1 && w <= 0xffffffffu);)
	return cvc5_term_manager.mkBitVectorSort(static_cast<uint32_t>(w));
}

// cvc5's verdict on a Boolean term, as bv_formula_sat_status configures it
std::optional<bool> cvc5_sat(const cvc5::Term& f) {
	cvc5::Solver solver(cvc5_term_manager);
	config_cvc5_solver_alternating_quantifiers(solver);
	config_cvc5_solver(solver, true);
	solver.assertFormula(f);
	auto r = solver.checkSat();
	if (r.isUnknown()) return std::nullopt;
	return r.isSat();
}

// Random formulas over 4-bit values: every operator the builders make,
// comparisons, Boolean connectives and quantifiers of both kinds.
struct formula_gen {
	static constexpr size_t W = 4;
	std::mt19937 rng;
	std::vector<cvc5::Term> scope;
	size_t fresh = 0;

	explicit formula_gen(unsigned seed) : rng(seed) {
		scope.push_back(cvc5_term_manager.mkConst(bv_sort(W), "x"));
		scope.push_back(cvc5_term_manager.mkConst(bv_sort(W), "y"));
	}
	size_t pick(size_t n) { return rng() % n; }

	cvc5::Term term(size_t depth) {
		if (depth == 0 || pick(3) == 0) {
			if (pick(3) == 0) return make_bitvector_value(W, pick(1u << W));
			return scope[pick(scope.size())];
		}
		const cvc5::Term a = term(depth - 1), b = term(depth - 1);
		switch (pick(15)) {
		case 0: return make_bitvector_add(a, b);
		case 1: return make_bitvector_sub(a, b);
		case 2: return make_bitvector_mul(a, b);
		case 3: return make_bitvector_and(a, b);
		case 4: return make_bitvector_or(a, b);
		case 5: return make_bitvector_xor(a, b);
		case 6: return make_bitvector_nand(a, b);
		case 7: return make_bitvector_nor(a, b);
		case 8: return make_bitvector_xnor(a, b);
		case 9: return make_bitvector_shl(a, b);
		case 10: return make_bitvector_shr(a, b);
		case 11: return make_bitvector_min(a, b);
		case 12: return make_bitvector_max(a, b);
		case 13: return make_bitvector_not(a);
		default: return make_bitvector_extract(
			make_bitvector_zero_extend(a, 3), W - 1, 0);
		}
	}

	cvc5::Term formula(size_t depth) {
		if (depth == 0 || pick(4) == 0) {
			const cvc5::Term a = term(2), b = term(2);
			switch (pick(6)) {
			case 0: return make_term_equal(a, b);
			case 1: return make_term_distinct(a, b);
			case 2: return make_term_less(a, b);
			case 3: return make_term_less_equal(a, b);
			case 4: return make_term_greater(a, b);
			default: return make_term_greater_equal(a, b);
			}
		}
		switch (pick(5)) {
		case 0: return make_term_not(formula(depth - 1));
		case 1: return make_term_and(formula(depth - 1), formula(depth - 1));
		case 2: return make_term_or(formula(depth - 1), formula(depth - 1));
		default: {
			cvc5::Term v = make_bitvector_var(bv_sort(W),
				"q" + std::to_string(fresh++));
			scope.push_back(v);
			const cvc5::Term body = formula(depth - 1);
			scope.pop_back();
			return pick(2) ? make_term_forall(v, body)
				: make_term_exists(v, body);
		}
		}
	}
};

// ex x1 ... ex xn (x1 < x0 && x2 < x1 && ... && xn < x(n-1)), x0 free
cvc5::Term falling_chain(size_t w, size_t n) {
	std::vector<cvc5::Term> xs{ cvc5_term_manager.mkConst(bv_sort(w), "x0") };
	for (size_t i = 1; i <= n; ++i)
		xs.push_back(make_bitvector_var(bv_sort(w), "x" + std::to_string(i)));
	cvc5::Term body = make_term_less(xs[n], xs[n - 1]);
	for (size_t i = n - 1; i >= 1; --i)
		body = make_term_exists(xs[i + 1],
			make_term_and(make_term_less(xs[i], xs[i - 1]), body));
	return make_term_exists(xs[1], body);
}

} // namespace

TEST_SUITE("cvc5_bitblast_sat") {

	TEST_CASE("agrees with cvc5 on random quantified formulas") {
		formula_gen gen(20260928);
		size_t decided = 0, compared = 0;
		for (size_t i = 0; i < 400; ++i) {
			const cvc5::Term f = gen.formula(4);
			const auto bdd = cvc5_bitblast_sat(f, 16, size_t{1} << 20);
			if (!bdd) continue;
			++decided;
			const auto ref = cvc5_sat(f);
			if (!ref) continue;
			++compared;
			CAPTURE(f.toString());
			CHECK( *bdd == *ref );
		}
		CHECK( decided == 400 );
		CHECK( compared > 300 );
	}

	TEST_CASE("a falling chain longer than the values is unsatisfiable") {
		// 255 steps down from a free x0 fit in 8 bits, 256 do not
		CHECK( cvc5_bitblast_sat(falling_chain(8, 255), 16, size_t{1} << 20)
			== std::optional<bool>(true) );
		CHECK( cvc5_bitblast_sat(falling_chain(8, 256), 16, size_t{1} << 20)
			== std::optional<bool>(false) );
	}

	TEST_CASE("a table too small for every node built decides the same") {
		// the nodes of the terms already read are freed as the table
		// fills; 512 nodes decide what 2^20 do for most formulas
		formula_gen gen(20260928);
		size_t decided = 0;
		for (size_t i = 0; i < 400; ++i) {
			const cvc5::Term f = gen.formula(4);
			const auto small = cvc5_bitblast_sat(f, 16, 512);
			if (!small) continue;
			++decided;
			CAPTURE(f.toString());
			CHECK( small == cvc5_bitblast_sat(f, 16, size_t{1} << 20) );
		}
		CHECK( decided > 300 );
	}

	TEST_CASE("a falling chain fits in a table smaller than all it builds") {
		// the chain builds some 2^16 nodes, of which a few hundred are
		// read at any time
		CHECK( cvc5_bitblast_sat(falling_chain(8, 255), 16, size_t{1} << 13)
			== std::optional<bool>(true) );
		CHECK( cvc5_bitblast_sat(falling_chain(8, 256), 16, size_t{1} << 13)
			== std::optional<bool>(false) );
	}

	TEST_CASE("declines what it has no circuit or room for") {
		cvc5::Term x = cvc5_term_manager.mkConst(bv_sort(8), "dx");
		cvc5::Term y = cvc5_term_manager.mkConst(bv_sort(8), "dy");
		cvc5::Term z = cvc5_term_manager.mkConst(bv_sort(17), "dz");
		CHECK( !cvc5_bitblast_sat(make_term_equal(z, z), 16, 1024) );
		CHECK( !cvc5_bitblast_sat(make_term_equal(
			make_bitvector_div(x, y), x), 16, 1024) );
		CHECK( !cvc5_bitblast_sat(make_term_equal(
			make_bitvector_mul(x, y), make_bitvector_value(8, 1)), 16, 64) );
		CHECK( cvc5_bitblast_sat(make_term_equal(
			make_bitvector_mul(x, y), make_bitvector_value(8, 1)), 16, 1 << 16)
			== std::optional<bool>(true) );
		// a product of two 16-bit values would not fit; a square does
		cvc5::Term u = cvc5_term_manager.mkConst(bv_sort(16), "du");
		cvc5::Term v = cvc5_term_manager.mkConst(bv_sort(16), "dv");
		CHECK( !cvc5_bitblast_sat(make_term_equal(
			make_bitvector_mul(u, v), make_bitvector_value(16, 2)), 16,
			size_t{1} << 20) );
		CHECK( cvc5_bitblast_sat(make_term_equal(
			make_bitvector_mul(u, u), make_bitvector_value(16, 2)), 16,
			size_t{1} << 20) == std::optional<bool>(false) );
	}

	TEST_CASE("a product of two values past 10 bits declines whatever its first factor") {
		cvc5::Term a = cvc5_term_manager.mkConst(bv_sort(11), "pa");
		cvc5::Term b = cvc5_term_manager.mkConst(bv_sort(11), "pb");
		cvc5::Term three = make_bitvector_value(11, 3);
		auto product = [](std::vector<cvc5::Term> factors) {
			return cvc5_term_manager.mkTerm(cvc5::Kind::BITVECTOR_MULT,
				factors);
		};
		auto equals_two = [](const cvc5::Term& p) {
			return make_term_equal(p, make_bitvector_value(11, 2));
		};
		const size_t nodes = size_t{1} << 21;
		CHECK( !cvc5_bitblast_sat(equals_two(product({ a, b })), 16, nodes) );
		CHECK( !cvc5_bitblast_sat(equals_two(product({ three, a, b })), 16,
			nodes) );
		CHECK( !cvc5_bitblast_sat(equals_two(product({ a, three, b })), 16,
			nodes) );
		// a square by a constant stays small
		CHECK( cvc5_bitblast_sat(equals_two(product({ three, a, a })), 16,
			nodes).has_value() );
	}

	TEST_CASE("a passed deadline declines, and says so") {
		bool late = false;
		const auto past = data_bdd::clock::now() - std::chrono::seconds(1);
		CHECK( !cvc5_bitblast_sat(falling_chain(8, 255), 16, size_t{1} << 20,
			past, &late) );
		CHECK( late );
		late = true;
		CHECK( cvc5_bitblast_sat(falling_chain(8, 255), 16, size_t{1} << 20,
			data_bdd::clock::now() + std::chrono::hours(1), &late)
			== std::optional<bool>(true) );
		CHECK( !late );
	}
}

TEST_SUITE("bounded bitvector decision") {

	using tau = tree<node_t>;
	tau::get_options wff_opts{ .parse = { .start = tau::wff } };

	TEST_CASE("a question past its budget is unknown and noted") {
		if (!bounded_calls_available()) return;
		const size_t timeout = bv_solve_timeout;
		const size_t nodes = bv_bitblast_max_nodes;
		bv_solve_timeout = 1;
		bv_bitblast_max_nodes = 0;
		take_time_budget_exhausted();
		auto wff = [](const std::string& src) {
			return tau::get(src, wff_opts).value_or(nullptr);
		};
		tref hard = wff("all x:bv[16] ex y:bv[16] "
			"all z:bv[16] ex u:bv[16] (y * x != z * u && z - u < z + y "
			"&& x * z != y * u + {3}:bv[16])");
		REQUIRE( hard );
		auto s = bv_formula_sat_status<node_t>(hard);
		const std::string noted = take_time_budget_exhausted();
		// cvc5 does not decide it in a second
		CHECK( s == bv_sat_status::unknown );
		CHECK( noted.find("bv-solve-timeout, 1 s") != std::string::npos );
		// asked again, the budget is noted again
		bv_formula_sat_status<node_t>(hard);
		CHECK( !take_time_budget_exhausted().empty() );
		// once a budget ran out, the unit of work asks nothing more
		note_time_budget_exhausted("earlier");
		tref easy = wff("ex x:bv[8] x = {3}:bv[8]");
		REQUIRE( easy );
		CHECK( bv_formula_sat_status<node_t>(easy) == bv_sat_status::unknown );
		take_time_budget_exhausted();
		tau::clear_caches();
		bv_solve_timeout = timeout;
		bv_bitblast_max_nodes = nodes;
	}

	TEST_CASE("a quantified question is answered in its child") {
		const size_t nodes = bv_bitblast_max_nodes;
		bv_bitblast_max_nodes = 0;
		take_time_budget_exhausted();
		tref f = tau::get("all x:bv[8] ex y:bv[8] x * y = x",
			wff_opts).value_or(nullptr);
		REQUIRE( f );
		CHECK( bv_formula_sat_status<node_t>(f) == bv_sat_status::sat );
		CHECK( take_time_budget_exhausted().empty() );
		bv_bitblast_max_nodes = nodes;
	}

	TEST_CASE("solving a question past its budget gives no solution") {
		if (!bounded_calls_available()) return;
		const size_t timeout = bv_solve_timeout;
		bv_solve_timeout = 1;
		take_time_budget_exhausted();
		tref hard = tau::get("all x:bv[16] ex y:bv[16] "
			"all z:bv[16] ex u:bv[16] (y * x != z * u && z - u < z + y "
			"&& x * z != y * u + {3}:bv[16])", wff_opts).value_or(nullptr);
		REQUIRE( hard );
		const auto start = std::chrono::steady_clock::now();
		CHECK( !solve_bv<node_t>(hard).value().has_value() );
		CHECK( std::chrono::steady_clock::now() - start
			< std::chrono::seconds(30) );
		CHECK( take_time_budget_exhausted().find("bv-solve-timeout, 1 s")
			!= std::string::npos );
		// once a budget ran out, nothing more is solved
		note_time_budget_exhausted("earlier");
		tref easy = tau::get("x:bv[8] = {3}:bv[8]", wff_opts)
			.value_or(nullptr);
		REQUIRE( easy );
		CHECK( !solve_bv<node_t>(easy).value().has_value() );
		take_time_budget_exhausted();
		CHECK( solve_bv<node_t>(easy).value().has_value() );
		bv_solve_timeout = timeout;
	}

	TEST_CASE("a bounded question decided sat is solved for its model") {
		take_time_budget_exhausted();
		tref f = tau::get("ex y:bv[8] z:bv[8] * y = {6}:bv[8]",
			wff_opts).value_or(nullptr);
		REQUIRE( f );
		auto s = solve_bv<node_t>(f).value();
		REQUIRE( s.has_value() );
		CHECK( s->size() == 1 );
		CHECK( take_time_budget_exhausted().empty() );
	}
}
