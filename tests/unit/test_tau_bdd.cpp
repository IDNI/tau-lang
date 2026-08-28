// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <algorithm>

#include "test_init.h"
// the bv fixture, not test_bool_only_helpers.h: "BDD and many" pins a conjunct
// order that follows the ba_type pool indices (D8)
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
		// y and z are not in the explicit order, so they are combined as
		// opaque leaf terms; their relative print order is decided by a
		// content-hash tie-break (see tau_bdd.tmpl.h am_cmp/subtree_less)
		// that is not a guaranteed canonical order and can flip whenever
		// the parser grammar changes (nonterminal ids feed the hash).
		CHECK((tau::get(t).to_str() == "x&(yz)'|x'"
			|| tau::get(t).to_str() == "x&(zy)'|x'"));
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
		// None of x,y,z,q,w,e,r,t are in the explicit order, so they are
		// combined as opaque leaf terms whose relative print order is a
		// deterministic (see tau_bdd.tmpl.h operator<'s content-string
		// tie-break), but not alphabetical, function of how the pairwise
		// merges are structured -- not a property worth pinning down to a
		// single "canonical" permutation. Check content instead of order:
		// every one of the 8 variables must appear exactly once.
		std::string res = tau::get(t).to_str();
		CHECK(res.size() == 8);
		for (char c : std::string("xyzqwert"))
			CHECK(std::count(res.begin(), res.end(), c) == 1);
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
		// The 8 single-letter conjuncts print with no delimiter at all
		// (bare bf_and juxtaposition), in an order that follows the
		// ba_type pool indices (D8) rather than anything canonical, so
		// compare the tree modulo bf_and commutativity instead of the
		// printed string.
		CHECK(matches_bf_mod_and_or(ct, "xyefcdba"));
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
		// The product's factor order and duplicate-literal spelling are
		// a subtree_less / NDEBUG-dependent artifact that shifts with
		// every parser regeneration (three regens produced five distinct
		// spellings, differing even in duplicate counts -- idempotent in
		// a product, so harmless). Pin the content instead: the negated
		// factor in either orientation, the variables {a, b, c, d}, and
		// nothing else.
		std::string res = tau::get(xx).to_str();
		// The factor prints with or without an explicit `&` (Release
		// spells the product by juxtaposition: "(f'e')'dccbba").
		bool factor_found = false;
		for (const char* f : {"&(e'f')'", "&(f'e')'",
			"(e'f')'", "(f'e')'"}) {
			auto pos = res.find(f);
			if (pos == std::string::npos) continue;
			factor_found = true;
			res.erase(pos, std::string(f).size());
			break;
		}
		CHECK( factor_found );
		std::sort(res.begin(), res.end());
		res.erase(std::unique(res.begin(), res.end()), res.end());
		CHECK( res == "abcd" );
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

TEST_SUITE("BDD get_free_tau_vars") {
	using bdd = tau_term_bdd<node_t>;
	using hbdd = term_handle<node_t>;

	TEST_CASE("null tref returns empty") {
		CHECK(hbdd::get_free_tau_vars(nullptr).empty());
	}

	TEST_CASE("T terminal returns empty") {
		CHECK(hbdd::get_free_tau_vars(bdd::T.b).empty());
	}

	TEST_CASE("F terminal returns empty") {
		CHECK(hbdd::get_free_tau_vars(bdd::F.b).empty());
	}

	TEST_CASE("same pointer on second call") {
		// Pool-based deduplication means both calls return the same reference
		// whether or not TAU_CACHE is active.
		const trefs& fvs1 = hbdd::get_free_tau_vars(bdd::T.b);
		const trefs& fvs2 = hbdd::get_free_tau_vars(bdd::T.b);
		CHECK(&fvs1 == &fvs2);
	}

	TEST_CASE("multi-node BDD") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		tref tz = tau::trim(tau::get("z", opts));
		bdd::order o {{tx, 0}, {ty, 1}, {tz, 2}};
		bdd::ref xx = bdd::build_bdd(tau::get("xyz", opts), o);
		const trefs& fvs = hbdd::get_free_tau_vars(xx.b);
		CHECK(std::is_sorted(fvs.begin(), fvs.end(), tau::subtree_less));
		CHECK(fvs.size() == 3);
	}

	TEST_CASE("multi-node BDD with term leaf") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		bdd::ref xx = bdd::build_bdd(tau::get("xyz", opts), o);
		const trefs& fvs = hbdd::get_free_tau_vars(xx.b);
		CHECK(std::is_sorted(fvs.begin(), fvs.end(), tau::subtree_less));
		CHECK(fvs.size() == 3);
	}

	TEST_CASE("BDD_ID: get_free_vars agrees with get_free_tau_vars") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		tref bdd_node = hbdd::convert_to_tau_node(tau::get("xy", opts), o);
		// Traverse via the BDD_ID collector branch inside get_free_vars
		const trefs& via_extractor = get_free_vars<node_t>(bdd_node);
		// Traverse the BDD directly
		const trefs& direct = hbdd::get_free_tau_vars(
			hbdd::U.find(bdd_node)->second.get().b);
		CHECK(via_extractor == direct);
	}
}

TEST_SUITE("BDD ITE") {
	using bdd = tau_term_bdd<node_t>;
	using hbdd = term_handle<node_t>;

	TEST_CASE("ITE(T,g,h) == g") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		bdd::ref g = bdd::build_bdd(tau::get("xy", opts), o);
		bdd::ref h = bdd::build_bdd(tau::get("x|y", opts), o);
		CHECK(bdd::bdd_ite(bdd::T, g, h, o) == g);
	}

	TEST_CASE("ITE(F,g,h) == h") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		bdd::ref g = bdd::build_bdd(tau::get("xy", opts), o);
		bdd::ref h = bdd::build_bdd(tau::get("x|y", opts), o);
		CHECK(bdd::bdd_ite(bdd::F, g, h, o) == h);
	}

	TEST_CASE("ITE(f,T,F) == f") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		bdd::ref f = bdd::build_bdd(tau::get("xy", opts), o);
		CHECK(bdd::bdd_ite(f, bdd::T, bdd::F, o) == f);
	}

	TEST_CASE("ITE(f,F,T) == NOT(f)") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		bdd::ref f = bdd::build_bdd(tau::get("xy", opts), o);
		CHECK(bdd::bdd_ite(f, bdd::F, bdd::T, o) == bdd::bdd_not(f));
	}

	TEST_CASE("ITE(f,g,g) == g") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		bdd::ref f = bdd::build_bdd(tau::get("x|y", opts), o);
		bdd::ref g = bdd::build_bdd(tau::get("xy", opts), o);
		CHECK(bdd::bdd_ite(f, g, g, o) == g);
	}

	TEST_CASE("ITE(NOT(f),g,h) == ITE(f,h,g)") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		tref tz = tau::trim(tau::get("z", opts));
		bdd::order o {{tx, 0}, {ty, 1}, {tz, 2}};
		bdd::ref f = bdd::build_bdd(tau::get("xy", opts), o);
		bdd::ref g = bdd::build_bdd(tau::get("yz", opts), o);
		bdd::ref h = bdd::build_bdd(tau::get("x|z", opts), o);
		CHECK(bdd::bdd_ite(bdd::bdd_not(f), g, h, o) ==
		      bdd::bdd_ite(f, h, g, o));
	}

	TEST_CASE("ITE agrees with composed form") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		tref tz = tau::trim(tau::get("z", opts));
		bdd::order o {{tx, 0}, {ty, 1}, {tz, 2}};
		bdd::ref f = bdd::build_bdd(tau::get("xy", opts), o);
		bdd::ref g = bdd::build_bdd(tau::get("yz", opts), o);
		bdd::ref h = bdd::build_bdd(tau::get("x|z", opts), o);
		bdd::ref ite_result = bdd::bdd_ite(f, g, h, o);
		bdd::ref composed = bdd::bdd_or(
			bdd::bdd_and(f, g, o),
			bdd::bdd_and(bdd::bdd_not(f), h, o), o);
		CHECK(ite_result == composed);
	}

	TEST_CASE("cache: second call returns same ref") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		tref tz = tau::trim(tau::get("z", opts));
		bdd::order o {{tx, 0}, {ty, 1}, {tz, 2}};
		bdd::ref f = bdd::build_bdd(tau::get("xy", opts), o);
		bdd::ref g = bdd::build_bdd(tau::get("yz", opts), o);
		bdd::ref h = bdd::build_bdd(tau::get("x|z", opts), o);
		bdd::ref r1 = bdd::bdd_ite(f, g, h, o);
		bdd::ref r2 = bdd::bdd_ite(f, g, h, o);
		CHECK(r1.b == r2.b);
		CHECK(r1.inv == r2.inv);
	}
}

TEST_SUITE("BDD compose") {
	using bdd = tau_term_bdd<node_t>;

	TEST_CASE("identity: compose bit(xi) with xi := bit(xi)") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		bdd::ref xi = bdd::from_bit(tx);
		CHECK(bdd::bdd_compose(xi, tx, xi, o) == xi);
	}

	TEST_CASE("compose bit(xi) with xi := T") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		bdd::ref xi = bdd::from_bit(tx);
		CHECK(bdd::bdd_compose(xi, tx, bdd::T, o) == bdd::T);
	}

	TEST_CASE("compose bit(xi) with xi := F") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		bdd::ref xi = bdd::from_bit(tx);
		CHECK(bdd::bdd_compose(xi, tx, bdd::F, o) == bdd::F);
	}

	TEST_CASE("variable rename: bit(xi) with xi := bit(xj)") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		bdd::ref xi = bdd::from_bit(tx);
		bdd::ref xj = bdd::from_bit(ty);
		CHECK(bdd::bdd_compose(xi, tx, xj, o) == xj);
	}

	TEST_CASE("multi-var: AND(xi,xj) with {xi->T, xj->F} == F") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		tref tz = tau::trim(tau::get("z", opts));
		bdd::order o {{tx, 0}, {ty, 1}, {tz, 2}};
		bdd::ref f = bdd::build_bdd(tau::get("xy", opts), o);
		bdd::subs_t subs {{tx, bdd::T}, {ty, bdd::F}};
		CHECK(bdd::bdd_compose(f, std::move(subs), o) == bdd::F);
	}

	TEST_CASE("agrees with naive form for non-trivial f") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		tref tz = tau::trim(tau::get("z", opts));
		// f = x AND y, g = y AND z, replace x with g
		// result should equal ITE(g, f_hi, f_lo) = ITE(g, y, F) = g AND y = y AND z AND y = yz
		bdd::order o {{tx, 0}, {ty, 1}, {tz, 2}};
		bdd::ref f = bdd::build_bdd(tau::get("xy", opts), o);
		bdd::ref g = bdd::build_bdd(tau::get("yz", opts), o);
		bdd::ref expected = bdd::build_bdd(tau::get("yz", opts), o);
		CHECK(bdd::bdd_compose(f, tx, g, o) == expected);
	}

	TEST_CASE("g has variable above xi: rename xi to xa where xa is above xi") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref ta = tau::trim(tau::get("a", opts));
		tref tx = tau::trim(tau::get("x", opts));
		tref tb = tau::trim(tau::get("b", opts));
		// order: a(0) above x(1) above b(2)
		bdd::order o {{ta, 0}, {tx, 1}, {tb, 2}};
		bdd::ref xi = bdd::from_bit(tx);
		bdd::ref xa = bdd::from_bit(ta);
		// f = bit(x), replace x with a (a is above x in the order)
		// result should be bit(a)
		CHECK(bdd::bdd_compose(xi, tx, xa, o) == xa);
	}

	TEST_CASE("multi-var: g with variable above xi") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref ta = tau::trim(tau::get("a", opts));
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		// order: a(0) above x(1) above y(2)
		// f = x AND y, substitute x := a (a is above x), y := a
		// result = a AND a = a
		bdd::order o {{ta, 0}, {tx, 1}, {ty, 2}};
		bdd::ref f = bdd::build_bdd(tau::get("xy", opts), o);
		bdd::ref xa = bdd::from_bit(ta);
		bdd::subs_t subs {{tx, xa}, {ty, xa}};
		CHECK(bdd::bdd_compose(f, std::move(subs), o) == xa);
	}
}

TEST_SUITE("BDD term_handle quantifier elimination") {
	TEST_CASE("build + bdd_quant + to_tau_term: free leaf is zero after all-x on xa") {
		using bdd = tau_term_bdd<node_t>;
		using hbdd = term_handle<node_t>;
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		bdd::order o = {{tx, 0}};
		// "xa": x is the BDD variable, a is a free-variable leaf not in order
		hbdd h = hbdd::build(tau::get("xa", opts), o);
		// ∃x at formula level → ∀x at BDD level (Schröder: ∃x(f=0) ↔ (∀x f)=0)
		hbdd::quants q = {{tx, bdd::all}};
		tref result = h.bdd_quant(q, o).to_tau_term(1);
		// ∀x(xa) = cofactor[x=0]·cofactor[x=1] = 0·a = 0
		CHECK(tau::get(result).equals_0());
	}

	TEST_CASE("build + bdd_quant + to_tau_term: ITE-form xa|x'b leaves product of free leaves") {
		using bdd = tau_term_bdd<node_t>;
		using hbdd = term_handle<node_t>;
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		bdd::order o = {{tx, 0}};
		// ITE(x, a, b): high cofactor (x=1) = a, low cofactor (x=0) = b
		hbdd h = hbdd::build(tau::get("xa|x'b", opts), o);
		hbdd::quants q = {{tx, bdd::all}};
		tref result = h.bdd_quant(q, o).to_tau_term(1);
		// ∀x(xa|x'b) = cofactor[x=0]·cofactor[x=1] = b·a
		// Product order flipped by the 8f1a74c1 parser regen on Release
		// (NDEBUG subtree interning order changed); actual is "ab".
		CHECK((tau::get(result).to_str() == "ab"
			|| tau::get(result).to_str() == "ba"));
	}
}

TEST_SUITE("BDD IO variable") {
	TEST_CASE("initial input var i[0] is a BDD decision node, not a leaf") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		// Build i[0] via the tree builder (returns bf-wrapped IO variable)
		tref bi_var = build_in_var_at_n<node_t>("i", 0, tau_type_id<node_t>());
		tref ti = tau::trim(bi_var);    // strip bf wrapper → typed variable node for the BDD key
		tref ba_var = tau::get("a", opts);  // bf(a) — free leaf not in order
		// Term: i[0] AND a (both already bf-level)
		tref term = tau::build_bf_and(bi_var, ba_var);
		// IO variable i[0] is the only BDD variable; a becomes a leaf atom
		bdd::order o = {{ti, 0}};
		bdd::ref br = bdd::build_bdd(term, o);
		// i[0] must be a decision node (has children), not a leaf
		CHECK(!bdd::leaf(br));
		CHECK(tau::subtree_equals(bdd::get_var(br), ti));
		// Round-trip: the result of to_tau_term contains an io_var sub-node
		tref result = bdd::to_tau_term(br, 1);
		CHECK(tau::get(result).find_top(is<node_t, tau::io_var>) != nullptr);
	}
}

TEST_SUITE("BDD term_handle substitute") {
	TEST_CASE("substitute x→z in BDD of xy gives BDD of yz") {
		using bdd = tau_term_bdd<node_t>;
		using hbdd = term_handle<node_t>;
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		tref tz = tau::trim(tau::get("z", opts));
		// Order: x(0) < y(1) < z(2); z is above both x and y
		bdd::order o = {{tx, 0}, {ty, 1}, {tz, 2}};
		// Build BDD for "xy" and register as a BDD_ID tau node in U
		tref node_xy = hbdd::convert_to_tau_node(tau::get("xy", opts), o);
		// Build handle for the substitution value: single-variable BDD of "z"
		hbdd with_z = hbdd::build(tau::get("z", opts), o);
		// Substitute x → z across the formula containing the BDD node
		tref result_node = hbdd::substitute(node_xy, tx, with_z, o);
		// Retrieve the tau term for the resulting BDD (z has rank 2 > y rank 1, so y is above z)
		tref result_term = hbdd::U.find(result_node)->second.to_tau_term(1);
		CHECK(tau::get(result_term).to_str() == "yz");
	}
}

TEST_SUITE("tau_term_bdd::less_then / make_canonical") {
	using bdd = tau_term_bdd<node_t>;
	tau::get_options opts = { .parse = { .start = tau::bf } };

	TEST_CASE("less_then compares by order rank") {
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		CHECK(bdd::less_then(tx, ty, o));
		CHECK(!bdd::less_then(ty, tx, o));
	}

	TEST_CASE("less_then returns false when a variable is absent from the order") {
		tref tx = tau::trim(tau::get("x", opts));
		tref tz = tau::trim(tau::get("z", opts));
		bdd::order o {{tx, 0}};
		CHECK(!bdd::less_then(tx, tz, o));
		CHECK(!bdd::less_then(tz, tx, o));
	}

	TEST_CASE("make_canonical orders the pair the same way regardless of input order") {
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		bdd::ref a = bdd::build_bdd(tau::get("x", opts), o);
		bdd::ref b = bdd::build_bdd(tau::get("y", opts), o);

		bdd::ref x1 = a, y1 = b;
		bdd::make_canonical(x1, y1);
		bdd::ref x2 = b, y2 = a;
		bdd::make_canonical(x2, y2);

		CHECK(x1 == x2);
		CHECK(y1 == y2);
	}
}

TEST_SUITE("BDD handle creation") {
	TEST_CASE("creation and gc") {
		using bdd = tau_term_bdd<node_t>;
		using hbdd = term_handle<node_t>;
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
		// Construction of handles
		tref node1 = hbdd::convert_to_tau_node(bdd1, o);
		tref node2 = hbdd::convert_to_tau_node(bdd2, o);
		tref node3 = hbdd::convert_to_tau_node(bdd3, o);
		// Keep this reference in U after gc
		htref node1_handle = tau::geth(node1);

		// tau::get(node1).print_tree(std::cout << "node1 tree: ") << "\n";
		// tau::get(node2).print_tree(std::cout << "node2 tree: ") << "\n";
		// tau::get(node3).print_tree(std::cout << "node3 tree: ") << "\n";
		// std::cout << "U size before gc: " << hbdd::U.size() << "\n";

		auto res = hbdd::U.find(node1)->second.bdd_and(
			hbdd::U.find(node2)->second, o);
		res = res.bdd_and(hbdd::U.find(node3)->second, o);
		htref tau_res = tau::geth(res.to_tau_term(1));

		tau::gc();

		// std::cout << "U size after gc: " << hbdd::U.size() << "\n";
		CHECK(hbdd::U.size() == 1);

		// tau::get(tau_res->get()).print(std::cout << "res: ") << "\n";
		CHECK(tau::get(tau_res->get()).to_str() ==
			"sv&(wx&(yz|y'z')|w'x'&(yz|y'z'))|s'v'&(wx&(yz|y'z')|w'x'&(yz|y'z'))");
	}
}

// TT1-3: `bdd_ex` and `bdd_all` could not be instantiated at all.  Under
// TAU_CACHE (ON in Release) the private recursion at tau_bdd.tmpl.h dropped the
// `memo` argument, and `tau_term_bdd_handle::bdd_ex` bound its `const trefs&`
// parameter to `tbdd::bdd_ex`'s `trefs&` -- an error in every config.  Nothing
// in src/ or tests/ called them, so the breakage stayed latent; these cases
// keep both entry points instantiated in both configs.
TEST_SUITE("BDD ex/all quantification") {

	TEST_CASE("bdd_ex removes the quantified variable") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		bdd::ref xy = bdd::build_bdd(tau::get("xy", opts), o);
		bdd::ref y  = bdd::build_bdd(tau::get("y", opts), o);
		trefs v {tx};
		// ex x (x & y) == y
		CHECK((bdd::bdd_ex(xy, v, o) == y));
	}

	TEST_CASE("bdd_all over the only variable of a conjunct is F") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		bdd::ref xy = bdd::build_bdd(tau::get("xy", opts), o);
		trefs v {tx};
		// all x (x & y) == 0
		CHECK((bdd::bdd_all(xy, v, o) == bdd::F));
	}

	// TT1-24: the handle wrappers below had zero coverage (which is how
	// TT1-4's uncompilable convert_to_handle survived).
	TEST_CASE("convert_to_tau_node registers a mapping convert_to_handle finds") {
		using hbdd = term_handle<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		hbdd::order o {{tx, 0}, {ty, 1}};
		hbdd xy = hbdd::build(tau::get("xy", opts), o);
		tref n = hbdd::convert_to_tau_node(xy, 0);
		REQUIRE( n != nullptr );
		CHECK((hbdd::convert_to_handle(n) == xy));
	}

	TEST_CASE("handle bdd_ite/bdd_and_many/bdd_or_many mirror tbdd semantics") {
		using hbdd = term_handle<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		hbdd::order o {{tx, 0}, {ty, 1}};
		hbdd x  = hbdd::build(tau::get("x", opts), o);
		hbdd y  = hbdd::build(tau::get("y", opts), o);
		hbdd xy = hbdd::build(tau::get("xy", opts), o);
		hbdd x_or_y = hbdd::build(tau::get("x|y", opts), o);
		// ite(x, y, y) == y; ite(x, y, 0) == x & y
		CHECK((x.bdd_ite(y, y, o) == y));
		CHECK((x.bdd_ite(y, hbdd::build(tau::get("0", opts), o), o)
			== xy));
		CHECK((hbdd::bdd_and_many({x, y}, o) == xy));
		CHECK((hbdd::bdd_or_many({x, y}, o) == x_or_y));
		// compose: x[x := y] == y (single and simultaneous forms)
		CHECK((x.bdd_compose(tx, y, o) == y));
		CHECK((x.bdd_compose({{tx, y}}, o) == y));
	}

	TEST_CASE("handle bdd_ex/bdd_all accept a const trefs&") {
		using hbdd = term_handle<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
#ifdef TAU_CACHE
		tau_term_bdd<node_t>::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		hbdd::order o {{tx, 0}, {ty, 1}};
		const trefs v {tx};
		hbdd xy = hbdd::build(tau::get("xy", opts), o);
		hbdd y  = hbdd::build(tau::get("y", opts), o);
		hbdd f  = hbdd::build(tau::get("0", opts), o);
		CHECK((xy.bdd_ex(v, o) == y));
		CHECK((xy.bdd_all(v, o) == f));
	}
}

// The five bdd_* memo tables (and_memo, ex_memo, and_many_memo, quant_memo,
// ite_memo) are keyed by BDD refs alone, not by order, so an entry computed
// under one order is wrong under another -- sync_order_cache() is what is
// supposed to catch that, at every public entry point, by comparing the
// order given against last_order and clearing the tables when it differs.
// Every other TEST_CASE in this file either starts from an explicit
// clear_caches() or only ever uses one order, so a broken sync_order_cache()
// would go unnoticed. This suite calls a public entry point (bdd_and) twice,
// back to back, under two different orders, with no manual clear_caches()
// in between, and checks that the second call is not served the first
// call's answer out of and_memo.
#ifdef TAU_CACHE
TEST_SUITE("BDD order cache clearing") {
	// clear_caches() itself still drops all five memo tables plus
	// last_order/has_last_order together -- a genuine order change makes
	// every entry wrong, so there is no way to partially reuse one. This
	// is no longer exercised around a gc sweep (interpreter::maybe_gc
	// used to call clear_caches() out of band before every sweep; it no
	// longer does -- see tau_term_bdd::prune_caches and
	// collect_live_refs' pinning of last_order's keys instead), but
	// clear_caches()'s own order-change contract is unchanged and still
	// needs covering directly.
	TEST_CASE("clear_caches drops the order cache") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
		bdd::clear_caches();
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		bdd::ref bx = bdd::build_bdd(tau::get("x", opts), o);
		bdd::ref by = bdd::build_bdd(tau::get("y", opts), o);
		bdd::bdd_and(bx, by, o);
		REQUIRE(bdd::has_last_order);
		REQUIRE(!bdd::last_order.empty());
		bdd::clear_caches();
		CHECK(!bdd::has_last_order);
		CHECK(bdd::last_order.empty());
	}
}

TEST_SUITE("BDD order cache invalidation") {
	TEST_CASE("bdd_and recomputes under a new order with no manual clear_caches") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
		bdd::clear_caches();
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		// Single-variable BDDs: from_bit() interns purely on the
		// variable's tref, so bx/by are the same refs regardless of
		// which order build_bdd is given -- only the AND below is
		// order-sensitive, and it is applied to the very same {bx, by}
		// pair both times.
		bdd::order o1 {{tx, 0}, {ty, 1}};
		bdd::ref bx = bdd::build_bdd(tau::get("x", opts), o1);
		bdd::ref by = bdd::build_bdd(tau::get("y", opts), o1);

		// x ranks above y: AND puts x at the root, populating and_memo
		// keyed on {bx, by} under o1.
		bdd::ref r1 = bdd::bdd_and(bx, by, o1);
		REQUIRE(!bdd::leaf(r1));
		CHECK(tau::subtree_equals(bdd::get_var(r1), tx));

		// A genuinely different order, same refs, no manual
		// clear_caches(): sync_order_cache() must notice on its own and
		// drop and_memo, or this would come back as the stale o1 answer
		// (x at the root) served straight out of the {bx, by} entry.
		bdd::order o2 {{ty, 0}, {tx, 1}};
		bdd::ref r2 = bdd::bdd_and(bx, by, o2);
		REQUIRE(!bdd::leaf(r2));
		CHECK(tau::subtree_equals(bdd::get_var(r2), ty));
	}
}

// collect_live_refs() pins two disjoint sources of Tau trefs: the decision
// variable of every live BDD node (walked from the BDD universe directly),
// and -- since maybe_gc no longer clears the BDD caches out of band --
// last_order's keys, so a swept order key can never be left dangling for
// sync_order_cache()'s next `o == last_order` comparison to dereference.
TEST_SUITE("BDD collect_live_refs pins last_order") {
	TEST_CASE("collect_live_refs inserts last_order's keys into the keep set") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
		bdd::clear_caches();
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		// "qqorder" is a variable name no other TEST_CASE in this file
		// uses, so it cannot already be a decision variable somewhere in
		// the (process-lifetime, never-swept-in-tests) BDD universe from
		// an earlier test -- unlike "x"/"y", which most other cases also
		// build BDDs over. It ranks in the order below but is never
		// built into a BDD, so it can only reach `keep` via last_order,
		// not via the decision-variable walk -- isolating exactly the
		// behaviour under test.
		tref tq = tau::trim(tau::get("qqorder", opts));
		bdd::order o {{tx, 0}, {ty, 1}, {tq, 2}};
		bdd::ref bx = bdd::build_bdd(tau::get("x", opts), o);
		bdd::ref by = bdd::build_bdd(tau::get("y", opts), o);
		// Populate last_order through bdd_and: build_bdd alone does not
		// call sync_order_cache(), so has_last_order would stay false.
		bdd::bdd_and(bx, by, o);
		REQUIRE(bdd::has_last_order);
		REQUIRE(bdd::last_order.contains(tq));

		std::unordered_set<tref> keep;
		bdd::collect_live_refs(keep);
		CHECK(keep.contains(tx));
		CHECK(keep.contains(ty));
		CHECK(keep.contains(tq));
	}
}

TEST_SUITE("BDD prune_caches") {
	TEST_CASE("prune_caches drops ex_memo/quant_memo entries keyed on a swept variable") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
		bdd::clear_caches();
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		bdd::ref xy = bdd::build_bdd(tau::get("xy", opts), o);

		const trefs vx {tx};
		const trefs vy {ty};
		bdd::bdd_ex(xy, vx, o);
		bdd::bdd_ex(xy, vy, o);
		REQUIRE(bdd::ex_memo.contains(vx));
		REQUIRE(bdd::ex_memo.contains(vy));

		const bdd::quants qx {{tx, bdd::ex}};
		const bdd::quants qy {{ty, bdd::ex}};
		bdd::bdd_quant(xy, qx, o);
		bdd::bdd_quant(xy, qy, o);
		REQUIRE(bdd::quant_memo.contains(qx));
		REQUIRE(bdd::quant_memo.contains(qy));

		// ty survives the (simulated) sweep, tx does not.
		const std::unordered_set<tref> kept {ty};
		bdd::prune_caches(kept);

		CHECK(!bdd::ex_memo.contains(vx));
		CHECK(bdd::ex_memo.contains(vy));
		CHECK(!bdd::quant_memo.contains(qx));
		CHECK(bdd::quant_memo.contains(qy));
	}
}

TEST_SUITE("BDD prune_caches leaves the ref-keyed tables alone") {
	// and_memo, and_many_memo and ite_memo are keyed (and valued) purely
	// by BDD-store refs, never by a raw Tau tref, so a Tau-tree sweep
	// cannot invalidate them -- this encodes the owner's actual
	// requirement that they simply survive gc.
	TEST_CASE("and_memo, and_many_memo and ite_memo survive an empty kept set") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
		bdd::clear_caches();
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		tref tz = tau::trim(tau::get("z", opts));
		bdd::order o {{tx, 0}, {ty, 1}, {tz, 2}};
		bdd::ref bx = bdd::build_bdd(tau::get("x", opts), o);
		bdd::ref by = bdd::build_bdd(tau::get("y", opts), o);
		bdd::ref bz = bdd::build_bdd(tau::get("z", opts), o);

		bdd::bdd_and(bx, by, o);
		bdd::refs many {bx, by, bz};
		bdd::bdd_and_many(std::move(many), o);
		bdd::bdd_ite(bx, by, bz, o);

		REQUIRE(!bdd::and_memo.empty());
		REQUIRE(!bdd::and_many_memo.empty());
		REQUIRE(!bdd::ite_memo.empty());
		const size_t and_sz = bdd::and_memo.size();
		const size_t and_many_sz = bdd::and_many_memo.size();
		const size_t ite_sz = bdd::ite_memo.size();

		// Deliberately empty: if prune_caches() ever touched these
		// tables, an empty kept set would clear them.
		const std::unordered_set<tref> kept {};
		bdd::prune_caches(kept);

		CHECK(bdd::and_memo.size() == and_sz);
		CHECK(bdd::and_many_memo.size() == and_many_sz);
		CHECK(bdd::ite_memo.size() == ite_sz);
	}
}

// The three suites above prove prune_caches()'s pruning logic in isolation,
// by calling it directly on a hand-built `kept` set -- but nothing proves
// that the gc_callback which is supposed to invoke it during a real sweep is
// ever registered. If sync_order_cache()'s
// bintree<node>::gc_callbacks.push_back(...) silently failed (or ran but
// never fired), every one of those direct-call tests would still pass, and
// a real gc() sweep would never prune anything. This suite drives an actual
// bintree<node_t>::gc() sweep -- mirroring interpreter::maybe_gc's own
// sequence, collect_live_refs() into `keep` before gc(keep) -- and checks
// that an ex_memo/quant_memo entry keyed on a variable that did not survive
// the sweep is gone afterwards.
TEST_SUITE("BDD prune_caches via a real gc sweep") {
	TEST_CASE("a real gc() sweep prunes an ex_memo/quant_memo entry keyed "
			"on a swept variable") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
		bdd::clear_caches();
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		bdd::ref xy = bdd::build_bdd(tau::get("xy", opts), o);

		// "unplug" is spelled only from letters (g,h,j,k,l,m,n,p,u) that
		// no other TEST_CASE in this file ever uses as a bf sample --
		// neither as a standalone variable, nor embedded in a
		// multi-letter sample string (the grammar parses e.g.
		// "xyzqwert" as an AND of its individual single-letter
		// variables, see "xyzqwert no var" above, so a letter buried in
		// any earlier sample is just as committed as a standalone one).
		// So the tref this produces cannot already be a decision
		// variable of any BDD built earlier in this process: the BDD
		// store, unlike the Tau tree, is never swept, so any such
		// history would pin it forever via collect_live_refs' walk of
		// decision variables (and via add()'s protect_bdd_atom).
		tref tunplug = tau::trim(tau::get("unplug", opts));

		// tunplug deliberately stays out of o, so it is not pinned via
		// last_order either; it is never passed to build_bdd/add(), so
		// it never becomes a decision variable and is never
		// protect_bdd_atom'd; and nothing here ever takes an htref on
		// it. It only reaches ex_memo/quant_memo as a *quantified*
		// variable -- less_then() returns false for a variable absent
		// from the order, so bdd_ex/bdd_quant do not require it to
		// already occur in xy's BDD.
		const trefs v_unplug {tunplug};
		bdd::bdd_ex(xy, v_unplug, o);
		REQUIRE(bdd::ex_memo.contains(v_unplug));

		const bdd::quants q_unplug {{tunplug, bdd::ex}};
		bdd::bdd_quant(xy, q_unplug, o);
		REQUIRE(bdd::quant_memo.contains(q_unplug));

		// Positive control: tx is pinned twice over -- it is a decision
		// variable of xy, and a key of last_order via o -- so its own
		// ex_memo/quant_memo entries must survive the very same sweep.
		// Without this, a prune_caches() that (incorrectly) wiped the
		// tables completely would make the CHECKs below pass for the
		// wrong reason.
		const trefs v_tx {tx};
		bdd::bdd_ex(xy, v_tx, o);
		REQUIRE(bdd::ex_memo.contains(v_tx));

		const bdd::quants q_tx {{tx, bdd::ex}};
		bdd::bdd_quant(xy, q_tx, o);
		REQUIRE(bdd::quant_memo.contains(q_tx));

		const size_t m_pre = tau::m_size();
		std::unordered_set<tref> keep;
		// Mirrors interpreter::maybe_gc's sequence: pin the BDD
		// universe's live refs (and last_order's keys) before sweeping.
		// A bare/empty-keep sweep would not do this, and could free a
		// tau node a live BDD node still points at, corrupting every
		// later TEST_CASE in this binary.
		bdd::collect_live_refs(keep);
		bintree<node_t>::gc(keep);
		const size_t m_post = tau::m_size();

		// tunplug must actually have been swept -- otherwise this test
		// would prove nothing about the gc_callback under test, only
		// about prune_caches() called on a set that happens to already
		// exclude tunplug.
		REQUIRE(m_post < m_pre);
		// bintree<T>::gc(keep) repopulates `keep` with the complete
		// survivor set on a real sweep (see its tail: every key it
		// keeps in M() is inserted into `keep` before gc_callbacks
		// run), so this equality holds exactly when `keep` is the
		// authoritative survivor set, i.e. the sweep actually ran --
		// the same completeness check maybe_gc itself relies on.
		REQUIRE(keep.size() == m_post);
		CHECK(!keep.contains(tunplug));

		CHECK(!bdd::ex_memo.contains(v_unplug));
		CHECK(!bdd::quant_memo.contains(q_unplug));
		CHECK(bdd::ex_memo.contains(v_tx));
		CHECK(bdd::quant_memo.contains(q_tx));
	}
}
#endif
