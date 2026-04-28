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
