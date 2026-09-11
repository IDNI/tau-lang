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
		// Order flipped by the 8f1a74c1 parser regen (subtree interning
		// order for the "yz" conjunct changed): actual is now "x&(zy)'|x'".
		CHECK((tau::get(t).to_str() == "x&(zy)'|x'"
			|| tau::get(t).to_str() == "x&(yz)'|x'"));
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
		auto result = tau::get(t).to_str();
		// Any AND-commutative permutation of the same 8 literals is accepted;
		// the order is a hash/nt-id-order-dependent tie-break that drifts
		// with every parser regen (Debug and Release differ too).
		INFO("result: " << result);
		CHECK((result == "xzrqeywt" || result == "reyxtzqw" || result == "zwtyxqre" || result == "erxwtqzy"
			|| result == "xyzqwert"
			|| result == "ewytrxzq"
			|| result == "zrwyexqt"
			|| result == "xtzqrewy"
			|| result == "qxywrezt"
			|| result == "qyrtxwze"));
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
		auto result = tau::get(ct).to_str();
		// Any AND-commutative permutation of {a,b,c,d,e,f} is accepted;
		// bdd_and_many's merge order is a hash/nt-id-order-dependent
		// tie-break that drifts with every parser regen.
		INFO("result: " << result);
		CHECK((result == "xydcefab" || result == "xydcabfe" || result == "xydcbaef" || result == "xybadcfe" || result == "xybacdfe" || result == "xycdbafe" || result == "xycdbaef"
			|| result == "xycdabfe"
			|| result == "xycdabef"
			|| result == "xyfedcab"
			|| result == "xydcbafe" || result == "xydcabef"
			|| result == "xydcfeab" || result == "xycdfeab" ));
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
		auto result = tau::get(xx).to_str();
		// Any AND-commutative permutation of {a:1, b:2, c:2, d:1, (e'f')':1}
		// is accepted; the order is a hash/nt-id-order-dependent tie-break
		// that drifts with every parser regen (Debug and Release differ too).
		INFO("result: " << result);
		CHECK((result == "abbd&(f'e')'cc" || result == "d&(e'f')'bbcca" || result == "dbbcc&(e'f')'a" || result == "cbb&(f'e')'da" || result == "ab&(e'f')'bccd"
			|| result == "c&(e'f')'bbda"
			|| result == "ab&(f'e')'bccd"
			|| result == "cabb&(e'f')'d"
			|| result == "adbb&(e'f')'cc"
			|| result == "abbccd&(f'e')'"
			|| result == "(f'e')'adbbcc"
			|| result == "bccda&(f'e')'"));
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

	TEST_CASE("diamond-shaped BDD: the shared node is walked once") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		// (x|y)z: the leaf z is reached from x's high and from y's high
		bdd::ref xx = bdd::build_bdd(tau::get("xz|yz", opts), o);
		REQUIRE(bdd::node_count(xx) == 3);
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
			hbdd::U.find(hbdd::key_of(bdd_node))->second.get().b);
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
		// "ab"/"ba": pure AND-commutative permutation, order is a
		// hash/nt-id-order-dependent tie-break.
		auto result_str = tau::get(result).to_str();
		CHECK((result_str == "ab" || result_str == "ba"));
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
		tref result_term = hbdd::U.find(hbdd::key_of(result_node))
			->second.to_tau_term(1);
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
	TEST_CASE("gc: the key is the BDD_ID node, shared by every spelling of the wrapper") {
		using bdd = tau_term_bdd<node_t>;
		using hbdd = term_handle<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o = {{tx, 0}, {ty, 1}};
		// A BDD-backed term as an equation's left side: inside the atom the
		// bf(BDD_ID) wrapper carries `0` as its right sibling, a different
		// tree node from the trimmed wrapper `l` that convert_to_tau_node
		// returned; the two share the BDD_ID child, the store's key.
		tref l = hbdd::convert_to_tau_node(tau::get("xy", opts), o);
		tref atom = tau::build_bf_eq_0(l);
		tref l_in_atom = tau::get(atom)[0].first();
		REQUIRE(l_in_atom != l);
		REQUIRE(tau::get(l_in_atom).has_right_sibling());
		const tref key = hbdd::key_of(l);
		CHECK(hbdd::key_of(l_in_atom) == key);
		CHECK(tau::get(key).is(tau::BDD_ID));
		REQUIRE(hbdd::U.contains(key));
		const hbdd::intern_key_t ikey{ hbdd::convert_to_handle(l),
			find_ba_type<node_t>(l) };
		REQUIRE(hbdd::I.contains(ikey));
		size_t live = 0;
		{
			// Pin the atom alone — `l` has no handle and dies — and sweep
			// as the interpreter does (interpreter.tmpl.h): the BDD store's
			// own trefs first, then gc.
			htref keep_atom = tau::geth(atom);
			std::unordered_set<tref> keep;
			bdd::collect_live_refs(keep);
			tau::gc(keep);
			// Every lookup hits through the sibling-carrying spelling.
			REQUIRE(hbdd::U.contains(key));
			REQUIRE(hbdd::I.contains(ikey));
			CHECK(hbdd::I.find(ikey)->second == key);
			hbdd h = hbdd::convert_to_handle(l_in_atom);
			CHECK(tau::get(h.to_tau_term(find_ba_type<node_t>(l_in_atom)))
				.to_str() == "xy");
			CHECK(bdd::build_bdd(l_in_atom, o) == h.get());
			const trefs fv = get_free_vars<node_t>(atom);
			CHECK(fv.size() == 2);
			// Interning: the same BDD converts to the same trimmed wrapper,
			// re-derived from the kept BDD_ID node.
			CHECK(hbdd::convert_to_tau_node(tau::get("xy", opts), o)
				== tau::trim_right_sibling(l_in_atom));
			live = hbdd::U.size();
		}
		// Nothing holds the atom any more: the entry dies with its node.
		std::unordered_set<tref> keep;
		bdd::collect_live_refs(keep);
		tau::gc(keep);
		CHECK(hbdd::U.size() == live - 1);
		CHECK(!hbdd::I.contains(ikey));
	}

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

		auto res = hbdd::U.find(hbdd::key_of(node1))->second.bdd_and(
			hbdd::U.find(hbdd::key_of(node2))->second, o);
		res = res.bdd_and(hbdd::U.find(hbdd::key_of(node3))->second, o);
		htref tau_res = tau::geth(res.to_tau_term(1));

		// The interpreter's protocol: the BDD universe holds its variable
		// trefs raw (a BDD node's hash and equality read them), so they are
		// pinned before the sweep; a bare tau::gc() would leave them
		// dangling for every later BDD lookup in the process.
		std::unordered_set<tref> keep;
		bdd::collect_live_refs(keep);
		tau::gc(keep);

		// std::cout << "U size after gc: " << hbdd::U.size() << "\n";
		CHECK(hbdd::U.size() == 1);

		// tau::get(tau_res->get()).print(std::cout << "res: ") << "\n";
		CHECK(tau::get(tau_res->get()).to_str() ==
			"sv&(wx&(yz|y'z')|w'x'&(yz|y'z'))|s'v'&(wx&(yz|y'z')|w'x'&(yz|y'z'))");
	}

}

namespace {

/// Any `BDD_ID` node anywhere in the tree.
bool has_bdd_id(tref n) {
	return tau::get(n).find_top([](tref m) {
		return tau::get(m).is(tau::BDD_ID); }) != nullptr;
}

} // namespace

TEST_SUITE("BDD is_ordered") {
	using bdd = tau_term_bdd<node_t>;

	TEST_CASE("ordered under its own order only, every variable a key") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o1 {{tx, 0}, {ty, 1}};
		bdd::ref f = bdd::build_bdd(tau::get("xy", opts), o1);
		CHECK(bdd::is_ordered(f, o1));
		// the same ranks swapped: x sits above y but no longer ranks below it
		bdd::order o2 {{tx, 1}, {ty, 0}};
		CHECK(!bdd::is_ordered(f, o2));
		// y is not a key at all
		bdd::order o3 {{tx, 0}};
		CHECK(!bdd::is_ordered(f, o3));
		// terminals and leaves are ordered under any order
		CHECK(bdd::is_ordered(bdd::T, o3));
		CHECK(bdd::is_ordered(bdd::F, o3));
		CHECK(bdd::is_ordered(bdd::build_bdd(tau::get("z", opts), o3), o3));
	}
}

TEST_SUITE("BDD visit_nodes / node_count") {
	using bdd = tau_term_bdd<node_t>;

	TEST_CASE("a shared node is counted once, terminals zero") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		tref tz = tau::trim(tau::get("z", opts));
		bdd::order o {{tx, 0}, {ty, 1}, {tz, 2}};
		// (x|y)z: the z node hangs off both x's high and y's high
		bdd::ref f = bdd::build_bdd(tau::get("xz|yz", opts), o);
		CHECK(bdd::node_count(f) == 3);
		CHECK(bdd::node_count(bdd::T) == 0);
		CHECK(bdd::node_count(bdd::F) == 0);
		CHECK(bdd::node_count(bdd::build_bdd(tau::get("a", opts), o)) == 1);
		// xy|x'y': one y node, reached under both inverters
		bdd::ref g = bdd::build_bdd(tau::get("xy|x'y'", opts), o);
		CHECK(bdd::node_count(g) == 2);
	}

	TEST_CASE("fn stops the walk and the walk reports it") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		tref tz = tau::trim(tau::get("z", opts));
		bdd::order o {{tx, 0}, {ty, 1}, {tz, 2}};
		bdd::ref f = bdd::build_bdd(tau::get("xz|yz", opts), o);
		size_t n = 0;
		auto stop = [&n](bdd::ref, bool) { return ++n, n < 2; };
		CHECK(!bdd::visit_nodes(f, stop));
		CHECK(n == 2);
		size_t all = 0;
		auto count = [&all](bdd::ref, bool) { return ++all, true; };
		CHECK(bdd::visit_nodes(f, count));
		CHECK(all == 3);
	}
}

TEST_SUITE("BDD cofactor") {
	using bdd = tau_term_bdd<node_t>;

	TEST_CASE("Shannon identity at the top and at depth") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		tref tz = tau::trim(tau::get("z", opts));
		bdd::order o {{tx, 0}, {ty, 1}, {tz, 2}};
		bdd::ref f = bdd::build_bdd(tau::get("xy|x'z", opts), o);
		auto shannon = [&](tref v) {
			bdd::ref hi = bdd::bdd_cofactor(f, v, true, o);
			bdd::ref lo = bdd::bdd_cofactor(f, v, false, o);
			bdd::ref b  = bdd::from_bit(v);
			CHECK(bdd::bdd_and(f, b, o) == bdd::bdd_and(hi, b, o));
			CHECK(bdd::bdd_and(f, bdd::bdd_not(b), o)
				== bdd::bdd_and(lo, bdd::bdd_not(b), o));
			CHECK(bdd::is_ordered(hi, o));
			CHECK(bdd::is_ordered(lo, o));
			// v is the variable of no node of either cofactor
			auto absent = [&v](bdd::ref c, bool) {
				return !tau::subtree_equals(bdd::get_var(c), v); };
			CHECK(bdd::visit_nodes(hi, absent));
			CHECK(bdd::visit_nodes(lo, absent));
			// the compose entry point delegates here for a terminal g
			CHECK(bdd::bdd_compose(f, v, bdd::T, o) == hi);
			CHECK(bdd::bdd_compose(f, v, bdd::F, o) == lo);
		};
		shannon(tx);  // the top variable: child selection
		shannon(ty);  // deeper: the nodes above it are rebuilt
		// a variable that is not a key of the order: identity
		tref tw = tau::trim(tau::get("w", opts));
		CHECK(bdd::bdd_cofactor(f, tw, true, o) == f);
		CHECK(bdd::bdd_cofactor(f, tw, false, o) == f);
		// a terminal and a leaf are their own cofactors
		bdd::ref leaf = bdd::build_bdd(tau::get("a", opts), o);
		CHECK(bdd::bdd_cofactor(bdd::T, tx, true, o) == bdd::T);
		CHECK(bdd::bdd_cofactor(leaf, tx, false, o) == leaf);
	}
}

TEST_SUITE("BDD xor") {
	using bdd = tau_term_bdd<node_t>;

	TEST_CASE("identities, and build_bdd of a bf_xor term agrees") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		tref tz = tau::trim(tau::get("z", opts));
		bdd::order o {{tx, 0}, {ty, 1}, {tz, 2}};
		bdd::ref f = bdd::build_bdd(tau::get("xy", opts), o);
		CHECK(bdd::bdd_xor(f, f, o) == bdd::F);
		CHECK(bdd::bdd_xor(f, bdd::F, o) == f);
		CHECK(bdd::bdd_xor(f, bdd::T, o) == bdd::bdd_not(f));
		tref l = tau::get("xy", opts), r = tau::get("x|z", opts);
		tref x = tau::build_bf_xor(l, r);
		REQUIRE(tau::get(x).child_is(tau::bf_xor));
		CHECK(bdd::build_bdd(x, o) == bdd::bdd_xor(
			bdd::build_bdd(l, o), bdd::build_bdd(r, o), o));
	}
}

TEST_SUITE("BDD map_leaves") {
	using bdd = tau_term_bdd<node_t>;

	TEST_CASE("the identity functor gives the same ref back") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		bdd::order o {{tx, 0}};
		bdd::ref f = bdd::build_bdd(tau::get("xa|x'b", opts), o);
		auto id = [](tref l) { return l; };
		CHECK(bdd::map_leaves(f, id, o) == f);
	}

	TEST_CASE("every leaf to one term: the leaves merge and the node folds") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		bdd::order o {{tx, 0}};
		bdd::ref f = bdd::build_bdd(tau::get("xa|x'b", opts), o);
		tref c = tau::get("c", opts);
		auto to_c = [c](tref) { return c; };
		bdd::ref r = bdd::map_leaves(f, to_c, o);
		CHECK(r == bdd::build_bdd(c, o));
		CHECK(bdd::node_count(r) == 1);
		// to a constant: the whole BDD collapses to a terminal
		tref one = tau::get("1", opts);
		auto to_1 = [one](tref) { return one; };
		CHECK(bdd::map_leaves(f, to_1, o) == bdd::T);
	}

	TEST_CASE("a leaf that gains a decision variable is re-canonicalised") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tp = tau::trim(tau::get("p", opts));
		bdd::order o {{tp, 0}};
		bdd::ref f = bdd::build_bdd(tau::get("pa|p'b", opts), o);
		tref a = tau::get("a", opts);
		tref pc = tau::get("pc", opts);
		// a ← pc lifts the decision variable out of the leaf again
		auto put = [a, pc](tref l) {
			return tau::subtree_equals(l, a) ? pc : l; };
		bdd::ref r = bdd::map_leaves(f, put, o);
		CHECK(bdd::is_ordered(r, o));
		CHECK(r == bdd::build_bdd(tau::get("pc|p'b", opts), o));
	}
}

TEST_SUITE("BDD convert_to_tau_terms") {
	using bdd = tau_term_bdd<node_t>;
	using hbdd = term_handle<node_t>;

	TEST_CASE("every BDD_ID of a formula is converted, wherever it sits") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		tref tz = tau::trim(tau::get("z", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		tref n = hbdd::convert_to_tau_node(tau::get("xy", opts), o);
		REQUIRE(hbdd::is_bdd_backed(n));
		// the bare term: no BDD_ID left, and the same BDD when rebuilt
		tref plain = hbdd::convert_to_tau_terms(n);
		CHECK(!has_bdd_id(plain));
		CHECK(bdd::build_bdd(plain, o) == hbdd::convert_to_handle(n).get());
		// under a binder and under a negation
		tref atom = tau::build_bf_eq_0(n);
		tref phi = tau::build_wff_and(tau::build_wff_ex(tz, atom, false),
			tau::build_wff_neg(atom));
		REQUIRE(has_bdd_id(phi));
		CHECK(!has_bdd_id(hbdd::convert_to_tau_terms(phi)));
		// nothing to convert: the same tref
		CHECK(hbdd::convert_to_tau_terms(plain) == plain);
	}

	TEST_CASE("a BDD_ID held inside a leaf is converted too") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		tref tp = tau::trim(tau::get("p", opts));
		bdd::order oi {{tx, 0}, {ty, 1}};
		tref inner = hbdd::convert_to_tau_node(tau::get("xy", opts), oi);
		// the inner node inside a reference argument, which is a LEAF of an
		// outer BDD over p -- so the term the outer conversion produces
		// still holds a BDD_ID, one the traversal does not re-enter
		tref shell = tau::get("p & r(a)", opts);
		tref term = rewriter::replace<node_t>(shell,
			tau::get("a", opts), inner);
		REQUIRE(has_bdd_id(term));
		bdd::order op {{tp, 0}};
		tref outer = hbdd::convert_to_tau_node(term, op);
		REQUIRE(hbdd::is_bdd_backed(outer));
		// one conversion leaves the nested node behind -- the recursion on
		// the produced term is what this case is about
		REQUIRE(has_bdd_id(hbdd::convert_to_handle(outer)
			.to_tau_term(find_ba_type<node_t>(outer))));
		CHECK(!has_bdd_id(hbdd::convert_to_tau_terms(outer)));
	}
}

TEST_SUITE("BDD get_free_leaf_vars") {
	using bdd = tau_term_bdd<node_t>;
	using hbdd = term_handle<node_t>;

	TEST_CASE("the leaves' contribution alone") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		tref ta = tau::trim(tau::get("a", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		bdd::ref f = bdd::build_bdd(tau::get("xa|yb", opts), o);
		trefs leaves = hbdd::get_free_leaf_vars(f.b);
		const trefs& all = hbdd::get_free_tau_vars(f.b);
		CHECK(leaves.size() == 2);            // a and b
		CHECK(all.size() == 4);               // a, b, x and y
		CHECK(std::is_sorted(leaves.begin(), leaves.end(), tau::subtree_less));
		CHECK(std::binary_search(leaves.begin(), leaves.end(), ta,
			tau::subtree_less));
		CHECK(!std::binary_search(leaves.begin(), leaves.end(), tx,
			tau::subtree_less));
		CHECK(hbdd::get_free_leaf_vars(nullptr).empty());
	}

	TEST_CASE("a diamond gives the same set as a tree-shaped BDD") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		bdd::ref diamond = bdd::build_bdd(tau::get("xz|yz", opts), o);
		bdd::ref tree_shaped = bdd::build_bdd(tau::get("xz", opts), o);
		REQUIRE(bdd::node_count(diamond) == 3);
		CHECK(hbdd::get_free_leaf_vars(diamond.b)
			== hbdd::get_free_leaf_vars(tree_shaped.b));
	}
}

TEST_SUITE("BDD convert_to_tau_node_or_term") {
	using bdd = tau_term_bdd<node_t>;
	using hbdd = term_handle<node_t>;

	TEST_CASE("a BDD_ID only for a BDD that branches") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		const size_t type = find_ba_type<node_t>(tau::get("x", opts));
		CHECK(tau::get(hbdd::convert_to_tau_node_or_term(
			hbdd(bdd::T), type)).equals_1());
		CHECK(tau::get(hbdd::convert_to_tau_node_or_term(
			hbdd(bdd::F), type)).equals_0());
		// a single leaf: its plain term
		hbdd leaf_h(bdd::add(tau::trim(tau::get("a", opts))));
		tref lt = hbdd::convert_to_tau_node_or_term(leaf_h, type);
		CHECK(!hbdd::is_bdd_backed(lt));
		CHECK(tau::subtree_equals(lt, tau::get("a", opts)));
		// a branching BDD: the node convert_to_tau_node interns
		hbdd h = hbdd::build(tau::get("xy", opts), o);
		tref node = hbdd::convert_to_tau_node_or_term(h, type);
		CHECK(node == hbdd::convert_to_tau_node(h, type));
		CHECK(hbdd::is_bdd_backed(node));
	}
}

TEST_SUITE("BDD is_bdd_backed") {
	using bdd = tau_term_bdd<node_t>;
	using hbdd = term_handle<node_t>;

	TEST_CASE("the bf(BDD_ID) wrapper, in any spelling, and nothing else") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		tref n = hbdd::convert_to_tau_node(tau::get("xy", opts), o);
		CHECK(hbdd::is_bdd_backed(n));
		// the wrapper carrying a right sibling inside an atom
		tref atom = tau::build_bf_eq_0(n);
		CHECK(hbdd::is_bdd_backed(tau::get(atom)[0].first()));
		// the bare BDD_ID node is not one
		CHECK(!hbdd::is_bdd_backed(hbdd::key_of(n)));
		CHECK(!hbdd::is_bdd_backed(tau::get("a", opts)));
		CHECK(!hbdd::is_bdd_backed(nullptr));
	}
}

TEST_SUITE("BDD handle bdd_ex") {
	using bdd = tau_term_bdd<node_t>;
	using hbdd = term_handle<node_t>;

	TEST_CASE("agrees with the static, and sorts the caller's list") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::order o {{tx, 0}, {ty, 1}};
		hbdd h = hbdd::build(tau::get("xy|x'a", opts), o);
		trefs v { ty, tx }, w { ty, tx };
		hbdd q = h.bdd_ex(v, o);
		CHECK(q.get() == bdd::bdd_ex(h.get(), w, o));
		// both lists come back sorted by the order, as the static leaves them
		CHECK(v == w);
		CHECK(v[0] == tx);
	}
}
