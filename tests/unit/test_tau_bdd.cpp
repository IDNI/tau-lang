// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_Bool_helpers.h"
#include "tau_bdd.h"

using namespace idni::tau_lang;

TEST_SUITE("BDD creation terms") {
	TEST_CASE("1") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
		const char* sample = "1";
		tref spec = tau::get(sample, opts);
		bdd::order o;
		bdd::ref x = bdd::build_bdd(spec, o);
		CHECK((x == bdd::T));
	}
	TEST_CASE("F") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
		const char* sample = "0";
		tref spec = tau::get(sample, opts);
		bdd::order o;
		bdd::ref x = bdd::build_bdd(spec, o);
		CHECK((x == bdd::F));
	}
	TEST_CASE("!T") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
		const char* sample = "1'";
		tref spec = tau::get(sample, opts);
		bdd::order o;
		bdd::ref x = bdd::build_bdd(spec, o);
		CHECK((x == bdd::F));
	}
	TEST_CASE("!F") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
		const char* sample = "0'";
		tref spec = tau::get(sample, opts);
		bdd::order o;
		bdd::ref x = bdd::build_bdd(spec, o);
		CHECK((x == bdd::T));
	}
	TEST_CASE("xyz") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};

		const char* sample = "xyz";
		tref spec = tau::get(sample, opts);
		const char* x = "x";
		tref tx = tau::trim(tau::get(x, opts));
		const char* y = "y";
		tref ty = tau::trim(tau::get(y, opts));
		const char* z = "z";
		tref tz = tau::trim(tau::get(z, opts));
		bdd::order o {{tx, 0}, {ty, 1}, {tz, 2}};
		bdd::ref xx = bdd::build_bdd(spec, o);
		tref t = bdd::to_tau_term(xx, 1);
		CHECK(tau::get(t).to_str() == "xyz");
	}
	TEST_CASE("zyx") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		const char* sample = "xyz";
		tref spec = tau::get(sample, opts);
		const char* x = "x";
		tref tx = tau::trim(tau::get(x, opts));
		const char* y = "y";
		tref ty = tau::trim(tau::get(y, opts));
		const char* z = "z";
		tref tz = tau::trim(tau::get(z, opts));
		bdd::order o {{tx, 2}, {ty, 1}, {tz, 0}};
		bdd::ref xx = bdd::build_bdd(spec, o);
		tref t = bdd::to_tau_term(xx, 1);
		CHECK(tau::get(t).to_str() == "zyx");
	}
	TEST_CASE("(xyz)'") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		const char* sample = "(xyz)'";
		tref spec = tau::get(sample, opts);
		const char* x = "x";
		tref tx = tau::trim(tau::get(x, opts));
		const char* y = "y";
		tref ty = tau::trim(tau::get(y, opts));
		const char* z = "z";
		tref tz = tau::trim(tau::get(z, opts));
		bdd::order o {{tx, 0}, {ty, 1}, {tz, 2}};
		bdd::ref xx = bdd::build_bdd(spec, o);
		tref t = bdd::to_tau_term(xx, 1);
		CHECK(tau::get(t).to_str() == "x&(yz'|y')|x'");
	}
	TEST_CASE("(xyz)' only x") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		const char* sample = "(xyz)'";
		tref spec = tau::get(sample, opts);
		const char* x = "x";
		tref tx = tau::trim(tau::get(x, opts));
		bdd::order o {{tx, 0}};
		bdd::ref xx = bdd::build_bdd(spec, o);
		tref t = bdd::to_tau_term(xx, 1);
		CHECK(tau::get(t).to_str() == "x&(zy)'|x'");
	}
	TEST_CASE("xyzqwert no var") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		const char* sample = "xyzqwert";
		tref spec = tau::get(sample, opts);
		bdd::order o {};
		bdd::ref xx = bdd::build_bdd(spec, o);
		tref t = bdd::to_tau_term(xx, 1);
		CHECK(tau::get(t).to_str() == "xyzqwert");
	}
}

TEST_SUITE("BDD and many") {
	TEST_CASE("1") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		// Vars
		const char* vs = "v";
		tref tv = tau::trim(tau::get(vs, opts));
		const char* ws = "w";
		tref tw = tau::trim(tau::get(ws, opts));	
		const char* xs = "x";
		tref tx = tau::trim(tau::get(xs, opts));
		const char* ys = "y";
		tref ty = tau::trim(tau::get(ys, opts));
		const char* zs = "z";
		tref tz = tau::trim(tau::get(zs, opts));
		// BDDs
		const char* bdd1s = "xab";
		tref bdd1 = tau::get(bdd1s, opts);
		const char* bdd2s = "ycd";
		tref bdd2 = tau::get(bdd2s, opts);
		const char* bdd3s = "ef";
		tref bdd3 = tau::get(bdd3s, opts);
		// Ordering
		bdd::order o = {{tv, -2}, {tw, -1}, {tx, 0}, {ty, 1}, {tz, 2}};
		// Construction
		bdd::ref x = bdd::build_bdd(bdd1, o);
		bdd::ref y = bdd::build_bdd(bdd2, o);
		bdd::ref z = bdd::build_bdd(bdd3, o);
		bdd::refs bdds = {x,y,z};
		bdd::ref c = bdd::bdd_and_many(std::move(bdds), o);
		tref ct = bdd::to_tau_term(c, 1);
		CHECK((tau::get(ct).to_str() == "xydcbafe"));
	}
	TEST_CASE("2") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		// Vars
		const char* vs = "v";
		tref tv = tau::trim(tau::get(vs, opts));
		const char* ws = "w";
		tref tw = tau::trim(tau::get(ws, opts));
		const char* xs = "x";
		tref tx = tau::trim(tau::get(xs, opts));
		const char* ys = "y";
		tref ty = tau::trim(tau::get(ys, opts));
		const char* zs = "z";
		tref tz = tau::trim(tau::get(zs, opts));
		// BDD
		const char* bdd1s = "((vw'xy'z)'|(vw'xy'z))ab & (e|f) & ((v'|w|x'|y|z')|(vw'xy'z))bc & ((vw'xy'z)|(vw'xy'z)')cd";
		tref bdd1 = tau::get(bdd1s, opts);
		// Ordering
		bdd::order o = {{tv, -2}, {tw, -1}, {tx, 0}, {ty, 1}, {tz, 2}};
		// Construction
		bdd::ref x = bdd::build_bdd(bdd1, o);
		tref xx = bdd::to_tau_term(x, 1);
		CHECK(tau::get(xx).to_str() == "ab&(f'e')'bccd");
	}
}

TEST_SUITE("BDD quantification") {
	TEST_CASE("1") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		// Vars
		const char* ss = "s";
		tref ts = tau::trim(tau::get(ss, opts));
		const char* vs = "v";
		tref tv = tau::trim(tau::get(vs, opts));
		const char* ws = "w";
		tref tw = tau::trim(tau::get(ws, opts));
		const char* xs = "x";
		tref tx = tau::trim(tau::get(xs, opts));
		const char* ys = "y";
		tref ty = tau::trim(tau::get(ys, opts));
		const char* zs = "z";
		tref tz = tau::trim(tau::get(zs, opts));
		// BDDs
		const char* bdd1s = "(z|y') & (y|z')";
		tref bdd1 = tau::get(bdd1s, opts);
		const char* bdd2s = "(w|x') & (x|w')";
		tref bdd2 = tau::get(bdd2s, opts);
		const char* bdd3s = "(s|v') & (v|s')";
		tref bdd3 = tau::get(bdd3s, opts);
		// Ordering
		bdd::order o = {{ts, -3}, {tv, -2}, {tw, -1}, {tx, 0}, {ty, 1}, {tz, 2}};
		// Construction
		bdd::ref x = bdd::build_bdd(bdd1, o);
		bdd::ref y = bdd::build_bdd(bdd2, o);
		bdd::ref z = bdd::build_bdd(bdd3, o);
		bdd::refs bdds = {x, y, z};
		bdd::ref c1 = bdd::bdd_and_many(std::move(bdds), o);
		bdd::quants q {{tz, bdd::all}, {ty, bdd::ex}, {tx, bdd::all},
			{tw, bdd::ex}, {tv, bdd::all}, {ts, bdd::ex}};
		bdd::ref c2 = bdd::bdd_quant(c1, q, o);
		tref ct2 = bdd::to_tau_term(c2, 1);
		CHECK(tau::get(ct2).equals_1());
	}
}
