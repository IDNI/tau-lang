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
		bdd::ref x = bdd::build_bdd(spec);
		CHECK((x == bdd::T));
	}
	TEST_CASE("F") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
		const char* sample = "0";
		tref spec = tau::get(sample, opts);
		bdd::ref x = bdd::build_bdd(spec);
		CHECK((x == bdd::F));
	}
	TEST_CASE("!T") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
		const char* sample = "1'";
		tref spec = tau::get(sample, opts);
		bdd::ref x = bdd::build_bdd(spec);
		CHECK((x == bdd::F));
	}
	TEST_CASE("!F") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
		const char* sample = "0'";
		tref spec = tau::get(sample, opts);
		bdd::ref x = bdd::build_bdd(spec);
		CHECK((x == bdd::T));
	}
	TEST_CASE("xyz") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
		const char* sample = "xyz";
		tref spec = tau::get(sample, opts);
		bdd::ref xx = bdd::build_bdd(spec);
		tref t = bdd::to_tau_term(xx, 1);
		CHECK(tau::get(t).to_str() == "xyz");
	}
	TEST_CASE("zyx is spelled in name order") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		// The order is the variables' names, not the input's spelling.
		const char* sample = "zyx";
		tref spec = tau::get(sample, opts);
		bdd::ref xx = bdd::build_bdd(spec);
		tref t = bdd::to_tau_term(xx, 1);
		CHECK(tau::get(t).to_str() == "xyz");
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
		bdd::ref xx = bdd::build_bdd(spec);
		tref t = bdd::to_tau_term(xx, 1);
		CHECK(tau::get(t).to_str() == "x&(yz'|y')|x'");
	}
	TEST_CASE("(x r(y))': a reference is one leaf") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		bdd::ref xx = bdd::build_bdd(tau::get("(x & r(y))'", opts));
		// x is the one decision variable; the reference, `y` and all, is
		// a leaf under it
		REQUIRE(!bdd::leaf(xx));
		CHECK(tau::subtree_equals(bdd::get_var(xx), tx));
		CHECK(bdd::leaf(bdd::get_high(xx)));
		CHECK(bdd::get_high(xx) != bdd::T);
		CHECK(bdd::get_high(xx) != bdd::F);
		CHECK(bdd::get_low(xx) == bdd::T);
		CHECK(bdd::node_count(xx) == 2);
	}
	TEST_CASE("xyzqwert is spelled in name order") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		const char* sample = "xyzqwert";
		tref spec = tau::get(sample, opts);
		bdd::ref xx = bdd::build_bdd(spec);
		tref t = bdd::to_tau_term(xx, 1);
		auto result = tau::get(t).to_str();
		INFO("result: " << result);
		CHECK(result == "eqrtwxyz");
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
		// BDDs
		const char* bdd1s = "xab";
		tref bdd1 = tau::get(bdd1s, opts);
		const char* bdd2s = "ycd";
		tref bdd2 = tau::get(bdd2s, opts);
		const char* bdd3s = "ef";
		tref bdd3 = tau::get(bdd3s, opts);
		// Construction
		bdd::ref x = bdd::build_bdd(bdd1);
		bdd::ref y = bdd::build_bdd(bdd2);
		bdd::ref z = bdd::build_bdd(bdd3);
		bdd::refs bdds = {x,y,z};
		bdd::ref c = bdd::bdd_and_many(std::move(bdds));
		tref ct = bdd::to_tau_term(c, 1);
		auto result = tau::get(ct).to_str();
		// one decision node per variable, in name order
		INFO("result: " << result);
		CHECK(result == "abcdefxy");
	}

	TEST_CASE("2") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		// BDD
		const char* bdd1s = "((vw'xy'z)'|(vw'xy'z))ab & (e|f) & ((v'|w|x'|y|z')|(vw'xy'z))bc & ((vw'xy'z)|(vw'xy'z)')cd";
		tref bdd1 = tau::get(bdd1s, opts);
		// Construction: every `P ∪ P′` member folds, leaving abcd(e|f)
		bdd::ref x = bdd::build_bdd(bdd1);
		CHECK(x == bdd::build_bdd(tau::get("abcd & (e|f)", opts)));
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
		// Construction
		bdd::ref x = bdd::build_bdd(bdd1);
		bdd::ref y = bdd::build_bdd(bdd2);
		bdd::ref z = bdd::build_bdd(bdd3);
		bdd::refs bdds = {x, y, z};
		bdd::ref c1 = bdd::bdd_and_many(std::move(bdds));
		// The prefix nests opposite to the order: it is quantified one run
		// at a time, innermost first.
		bdd::quants q {{tz, bdd::all}, {ty, bdd::ex}, {tx, bdd::all},
			{tw, bdd::ex}, {tv, bdd::all}, {ts, bdd::ex}};
		bdd::ref c2 = bdd::bdd_quant(c1, q);
		tref ct2 = bdd::to_tau_term(c2, 1);
		CHECK(tau::get(ct2).equals_1());
	}

	TEST_CASE("a mixed prefix: the runs do not commute") {
		using bdd = tau_term_bdd<node_t>;
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		// (x ≡ y): ∀x ∃y holds, ∃y ∀x does not
		bdd::ref f = bdd::build_bdd(tau::get("(x|y') & (y|x')", opts));
		CHECK(bdd::bdd_quant(f, {{tx, bdd::all}, {ty, bdd::ex}}) == bdd::T);
		CHECK(bdd::bdd_quant(f, {{ty, bdd::ex}, {tx, bdd::all}}) == bdd::F);
		// a run of one kind is one set quantification
		CHECK(bdd::bdd_quant(f, {{tx, bdd::ex}, {ty, bdd::ex}}) == bdd::T);
		CHECK(bdd::bdd_quant(f, {{tx, bdd::all}, {ty, bdd::all}}) == bdd::F);
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
		bdd::ref xx = bdd::build_bdd(tau::get("xyz", opts));
		const trefs& fvs = hbdd::get_free_tau_vars(xx.b);
		CHECK(std::is_sorted(fvs.begin(), fvs.end(), tau::subtree_less));
		CHECK(fvs.size() == 3);
	}

	TEST_CASE("multi-node BDD with term leaf") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		bdd::ref xx = bdd::build_bdd(tau::get("x & y & r(z)", opts));
		const trefs& fvs = hbdd::get_free_tau_vars(xx.b);
		CHECK(std::is_sorted(fvs.begin(), fvs.end(), tau::subtree_less));
		CHECK(fvs.size() == 3);
	}

	TEST_CASE("diamond-shaped BDD: the shared node is walked once") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		// (x|y)z: the z node is reached from x's high and from y's high
		bdd::ref xx = bdd::build_bdd(tau::get("xz|yz", opts));
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
		tref bdd_node = hbdd::convert_to_tau_node(tau::get("xy", opts));
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
		bdd::ref g = bdd::build_bdd(tau::get("xy", opts));
		bdd::ref h = bdd::build_bdd(tau::get("x|y", opts));
		CHECK(bdd::bdd_ite(bdd::T, g, h) == g);
	}

	TEST_CASE("ITE(F,g,h) == h") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		bdd::ref g = bdd::build_bdd(tau::get("xy", opts));
		bdd::ref h = bdd::build_bdd(tau::get("x|y", opts));
		CHECK(bdd::bdd_ite(bdd::F, g, h) == h);
	}

	TEST_CASE("ITE(f,T,F) == f") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		bdd::ref f = bdd::build_bdd(tau::get("xy", opts));
		CHECK(bdd::bdd_ite(f, bdd::T, bdd::F) == f);
	}

	TEST_CASE("ITE(f,F,T) == NOT(f)") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		bdd::ref f = bdd::build_bdd(tau::get("xy", opts));
		CHECK(bdd::bdd_ite(f, bdd::F, bdd::T) == bdd::bdd_not(f));
	}

	TEST_CASE("ITE(f,g,g) == g") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		bdd::ref f = bdd::build_bdd(tau::get("x|y", opts));
		bdd::ref g = bdd::build_bdd(tau::get("xy", opts));
		CHECK(bdd::bdd_ite(f, g, g) == g);
	}

	TEST_CASE("ITE(NOT(f),g,h) == ITE(f,h,g)") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		bdd::ref f = bdd::build_bdd(tau::get("xy", opts));
		bdd::ref g = bdd::build_bdd(tau::get("yz", opts));
		bdd::ref h = bdd::build_bdd(tau::get("x|z", opts));
		CHECK(bdd::bdd_ite(bdd::bdd_not(f), g, h) ==
		      bdd::bdd_ite(f, h, g));
	}

	TEST_CASE("ITE agrees with composed form") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		bdd::ref f = bdd::build_bdd(tau::get("xy", opts));
		bdd::ref g = bdd::build_bdd(tau::get("yz", opts));
		bdd::ref h = bdd::build_bdd(tau::get("x|z", opts));
		bdd::ref ite_result = bdd::bdd_ite(f, g, h);
		bdd::ref composed = bdd::bdd_or(
			bdd::bdd_and(f, g),
			bdd::bdd_and(bdd::bdd_not(f), h));
		CHECK(ite_result == composed);
	}

	TEST_CASE("cache: second call returns same ref") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		bdd::ref f = bdd::build_bdd(tau::get("xy", opts));
		bdd::ref g = bdd::build_bdd(tau::get("yz", opts));
		bdd::ref h = bdd::build_bdd(tau::get("x|z", opts));
		bdd::ref r1 = bdd::bdd_ite(f, g, h);
		bdd::ref r2 = bdd::bdd_ite(f, g, h);
		CHECK(r1.b == r2.b);
		CHECK(r1.inv == r2.inv);
	}

	// f is a leaf, so top must come from g or h.
	TEST_CASE("ITE(leaf,g,h) picks top from a non-leaf root") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tz = tau::trim(tau::get("z", opts));
		bdd::ref y = bdd::build_bdd(tau::get("r(y)", opts));
		REQUIRE(bdd::leaf(y));
		bdd::ref z = bdd::from_bit(tz);
		bdd::ref r = bdd::bdd_ite(y, z, bdd::bdd_not(z));
		bdd::ref composed = bdd::bdd_or(
			bdd::bdd_and(y, z),
			bdd::bdd_and(bdd::bdd_not(y), bdd::bdd_not(z)));
		CHECK(r == composed);
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
		bdd::ref xi = bdd::from_bit(tx);
		CHECK(bdd::bdd_compose(xi, tx, xi) == xi);
	}

	TEST_CASE("compose bit(xi) with xi := T") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		bdd::ref xi = bdd::from_bit(tx);
		CHECK(bdd::bdd_compose(xi, tx, bdd::T) == bdd::T);
	}

	TEST_CASE("compose bit(xi) with xi := F") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		bdd::ref xi = bdd::from_bit(tx);
		CHECK(bdd::bdd_compose(xi, tx, bdd::F) == bdd::F);
	}

	TEST_CASE("variable rename: bit(xi) with xi := bit(xj)") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::ref xi = bdd::from_bit(tx);
		bdd::ref xj = bdd::from_bit(ty);
		CHECK(bdd::bdd_compose(xi, tx, xj) == xj);
	}

	TEST_CASE("multi-var: AND(xi,xj) with {xi->T, xj->F} == F") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::ref f = bdd::build_bdd(tau::get("xy", opts));
		bdd::subs_t subs {{tx, bdd::T}, {ty, bdd::F}};
		CHECK(bdd::bdd_compose(f, std::move(subs)) == bdd::F);
	}

	TEST_CASE("agrees with naive form for non-trivial f") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		// f = x AND y, g = y AND z, replace x with g
		// result should equal ITE(g, f_hi, f_lo) = ITE(g, y, F) = g AND y = y AND z AND y = yz
		bdd::ref f = bdd::build_bdd(tau::get("xy", opts));
		bdd::ref g = bdd::build_bdd(tau::get("yz", opts));
		bdd::ref expected = bdd::build_bdd(tau::get("yz", opts));
		CHECK(bdd::bdd_compose(f, tx, g) == expected);
	}

	TEST_CASE("g has variable above xi: rename xi to xa where xa is above xi") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref ta = tau::trim(tau::get("a", opts));
		tref tx = tau::trim(tau::get("x", opts));
		// order: a above x, by name
		bdd::ref xi = bdd::from_bit(tx);
		bdd::ref xa = bdd::from_bit(ta);
		// f = bit(x), replace x with a (a is above x in the order)
		// result should be bit(a)
		CHECK(bdd::bdd_compose(xi, tx, xa) == xa);
	}

	TEST_CASE("multi-var: g with variable above xi") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref ta = tau::trim(tau::get("a", opts));
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		// order: a above x above y, by name
		// f = x AND y, substitute x := a (a is above x), y := a
		// result = a AND a = a
		bdd::ref f = bdd::build_bdd(tau::get("xy", opts));
		bdd::ref xa = bdd::from_bit(ta);
		bdd::subs_t subs {{tx, xa}, {ty, xa}};
		CHECK(bdd::bdd_compose(f, std::move(subs)) == xa);
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
		// "xa": both are decision variables
		hbdd h = hbdd::build(tau::get("xa", opts));
		// ∃x at formula level → ∀x at BDD level (Schröder: ∃x(f=0) ↔ (∀x f)=0)
		hbdd::quants q = {{tx, bdd::all}};
		tref result = h.bdd_quant(q).to_tau_term(1);
		// ∀x(xa) = cofactor[x=0]·cofactor[x=1] = 0·a = 0
		CHECK(tau::get(result).equals_0());
	}

	TEST_CASE("build + bdd_quant + to_tau_term: ITE-form xa|x'b leaves the product of the cofactors") {
		using bdd = tau_term_bdd<node_t>;
		using hbdd = term_handle<node_t>;
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		// ITE(x, a, b): high cofactor (x=1) = a, low cofactor (x=0) = b
		hbdd h = hbdd::build(tau::get("xa|x'b", opts));
		hbdd::quants q = {{tx, bdd::all}};
		tref result = h.bdd_quant(q).to_tau_term(1);
		// ∀x(xa|x'b) = cofactor[x=0]·cofactor[x=1] = b·a, spelled in name
		// order
		CHECK(tau::get(result).to_str() == "ab");
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
		tref ti = tau::trim(bi_var);    // strip bf wrapper → typed variable node
		tref ba_ref = tau::get("r(a)", opts);  // a reference: a leaf
		// Term: i[0] AND r(a) (both already bf-level)
		tref term = tau::build_bf_and(bi_var, ba_ref);
		bdd::ref br = bdd::build_bdd(term);
		// i[0] must be a decision node (has children), not a leaf
		CHECK(!bdd::leaf(br));
		CHECK(tau::subtree_equals(bdd::get_var(br), ti));
		// Round-trip: the result of to_tau_term contains an io_var sub-node
		tref result = bdd::to_tau_term(br, 1);
		CHECK(tau::get(result).find_top(is<node_t, tau::io_var>) != nullptr);
	}
}

namespace {

/// Any `BDD_ID` node anywhere in the tree.
bool has_bdd_id(tref n) {
	return tau::get(n).find_top([](tref m) {
		return tau::get(m).is(tau::BDD_ID); }) != nullptr;
}

} // namespace

TEST_SUITE("tau_term_bdd::less_then / make_canonical") {
	using bdd = tau_term_bdd<node_t>;
	tau::get_options opts = { .parse = { .start = tau::bf } };

	TEST_CASE("free variables by name") {
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		CHECK(bdd::less_then(tx, ty));
		CHECK(!bdd::less_then(ty, tx));
		CHECK(!bdd::less_then(tx, tx));
	}

	TEST_CASE("then binder variables, outermost (highest id) first") {
		tref tx = tau::trim(tau::get("x", opts));
		const size_t type = tau::get(tx).get_ba_type();
		tref v1 = tau::build_variable("1", type);
		tref v2 = tau::build_variable("2", type);
		tref v10 = tau::build_variable("10", type);
		CHECK(bdd::less_then(tx, v2));
		CHECK(!bdd::less_then(v2, tx));
		CHECK(bdd::less_then(v2, v1));
		CHECK(!bdd::less_then(v1, v2));
		// by the id, not by the spelling
		CHECK(bdd::less_then(v10, v2));
	}

	TEST_CASE("one name: ties by content order, either way exactly once") {
		tref i0 = tau::trim(build_in_var_at_n<node_t>("i", 0,
			tau_type_id<node_t>()));
		tref i1 = tau::trim(build_in_var_at_n<node_t>("i", 1,
			tau_type_id<node_t>()));
		REQUIRE(!tau::subtree_equals(i0, i1));
		CHECK(bdd::less_then(i0, i1) != bdd::less_then(i1, i0));
		CHECK(bdd::less_then(i0, i1) == tau::subtree_less(i0, i1));
	}

	TEST_CASE("make_canonical orders the pair the same way regardless of input order") {
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		bdd::ref a = bdd::build_bdd(tau::get("x", opts));
		bdd::ref b = bdd::build_bdd(tau::get("y", opts));

		bdd::ref x1 = a, y1 = b;
		bdd::make_canonical(x1, y1);
		bdd::ref x2 = b, y2 = a;
		bdd::make_canonical(x2, y2);

		CHECK(x1 == x2);
		CHECK(y1 == y2);
	}
}

TEST_SUITE("BDD handle creation") {
	TEST_CASE("gc: the key is the BDD_ID node's id, shared by every spelling of the wrapper") {
		using bdd = tau_term_bdd<node_t>;
		using hbdd = term_handle<node_t>;
		tau::get_options opts = {
			.parse = { .start = tau::bf },
		};
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		// A BDD-backed term as an equation's left side: inside the atom the
		// bf(BDD_ID) wrapper carries `0` as its right sibling, a different
		// tree node from the trimmed wrapper `l` that convert_to_tau_node
		// returned; the two share the BDD_ID child, whose id is the key.
		tref l = hbdd::convert_to_tau_node(tau::get("xy", opts));
		tref atom = tau::build_bf_eq_0(l);
		tref l_in_atom = tau::get(atom)[0].first();
		REQUIRE(l_in_atom != l);
		REQUIRE(tau::get(l_in_atom).has_right_sibling());
		const tref id_node = tau::get(l).first();
		CHECK(tau::get(l_in_atom).first() == id_node);
		CHECK(tau::get(id_node).is(tau::BDD_ID));
		const size_t key = hbdd::key_of(l);
		CHECK(hbdd::key_of(l_in_atom) == key);
		CHECK(hbdd::key_of(id_node) == key);
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
			CHECK(hbdd::I.find(ikey)->second.id_node == id_node);
			CHECK(hbdd::I.find(ikey)->second.id == key);
			hbdd h = hbdd::convert_to_handle(l_in_atom);
			CHECK(tau::get(h.to_tau_term(find_ba_type<node_t>(l_in_atom)))
				.to_str() == "xy");
			CHECK(bdd::build_bdd(l_in_atom) == h.get());
			const trefs fv = get_free_vars<node_t>(atom);
			CHECK(fv.size() == 2);
			// Interning: the same BDD converts to the same trimmed wrapper,
			// re-derived from the kept BDD_ID node.
			CHECK(hbdd::convert_to_tau_node(tau::get("xy", opts))
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
		// BDDs
		const char* bdd1s = "(z|y') & (y|z')";
		tref bdd1 = tau::get(bdd1s, opts);
		const char* bdd2s = "(w|x') & (x|w')";
		tref bdd2 = tau::get(bdd2s, opts);
		const char* bdd3s = "(s|v') & (v|s')";
		tref bdd3 = tau::get(bdd3s, opts);
		// Construction of handles
		tref node1 = hbdd::convert_to_tau_node(bdd1);
		tref node2 = hbdd::convert_to_tau_node(bdd2);
		tref node3 = hbdd::convert_to_tau_node(bdd3);
		// Keep this reference in U after gc
		htref node1_handle = tau::geth(node1);

		// tau::get(node1).print_tree(std::cout << "node1 tree: ") << "\n";
		// tau::get(node2).print_tree(std::cout << "node2 tree: ") << "\n";
		// tau::get(node3).print_tree(std::cout << "node3 tree: ") << "\n";
		// std::cout << "U size before gc: " << hbdd::U.size() << "\n";

		auto res = hbdd::U.find(hbdd::key_of(node1))->second.bdd_and(
			hbdd::U.find(hbdd::key_of(node2))->second);
		res = res.bdd_and(hbdd::U.find(hbdd::key_of(node3))->second);
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

TEST_SUITE("BDD is_ordered") {
	using bdd = tau_term_bdd<node_t>;

	TEST_CASE("decision variables strictly in the global order down every path") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref tx = tau::trim(tau::get("x", opts));
		tref ty = tau::trim(tau::get("y", opts));
		bdd::ref f = bdd::build_bdd(tau::get("xy", opts));
		CHECK(bdd::is_ordered(f));
		// y above x: against the order
		CHECK(!bdd::is_ordered(bdd::add(ty, bdd::from_bit(tx), bdd::F)));
		// a decision node over a term that is no variable
		CHECK(!bdd::is_ordered(bdd::add(tau::trim(tau::get("r(a)", opts)),
			bdd::T, bdd::F)));
		// terminals and leaves are ordered
		CHECK(bdd::is_ordered(bdd::T));
		CHECK(bdd::is_ordered(bdd::F));
		CHECK(bdd::is_ordered(bdd::build_bdd(tau::get("r(z)", opts))));
	}
}

TEST_SUITE("BDD visit_nodes / node_count") {
	using bdd = tau_term_bdd<node_t>;

	TEST_CASE("a shared node is counted once, terminals zero") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		// (x|y)z: the z node hangs off both x's high and y's high
		bdd::ref f = bdd::build_bdd(tau::get("xz|yz", opts));
		CHECK(bdd::node_count(f) == 3);
		CHECK(bdd::node_count(bdd::T) == 0);
		CHECK(bdd::node_count(bdd::F) == 0);
		CHECK(bdd::node_count(bdd::build_bdd(tau::get("r(a)", opts))) == 1);
		// xy|x'y': one y node, reached under both inverters
		bdd::ref g = bdd::build_bdd(tau::get("xy|x'y'", opts));
		CHECK(bdd::node_count(g) == 2);
	}

	TEST_CASE("fn stops the walk and the walk reports it") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		bdd::ref f = bdd::build_bdd(tau::get("xz|yz", opts));
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
		bdd::ref f = bdd::build_bdd(tau::get("xy|x'z", opts));
		auto shannon = [&](tref v) {
			bdd::ref hi = bdd::bdd_cofactor(f, v, true);
			bdd::ref lo = bdd::bdd_cofactor(f, v, false);
			bdd::ref b  = bdd::from_bit(v);
			CHECK(bdd::bdd_and(f, b) == bdd::bdd_and(hi, b));
			CHECK(bdd::bdd_and(f, bdd::bdd_not(b))
				== bdd::bdd_and(lo, bdd::bdd_not(b)));
			CHECK(bdd::is_ordered(hi));
			CHECK(bdd::is_ordered(lo));
			// v is the variable of no node of either cofactor
			auto absent = [&v](bdd::ref c, bool) {
				return !tau::subtree_equals(bdd::get_var(c), v); };
			CHECK(bdd::visit_nodes(hi, absent));
			CHECK(bdd::visit_nodes(lo, absent));
			// the compose entry point delegates here for a terminal g
			CHECK(bdd::bdd_compose(f, v, bdd::T) == hi);
			CHECK(bdd::bdd_compose(f, v, bdd::F) == lo);
		};
		shannon(tx);  // the top variable: child selection
		shannon(ty);  // deeper: the nodes above it are rebuilt
		// a variable f does not hold: identity
		tref tw = tau::trim(tau::get("w", opts));
		CHECK(bdd::bdd_cofactor(f, tw, true) == f);
		CHECK(bdd::bdd_cofactor(f, tw, false) == f);
		// a terminal and a leaf are their own cofactors
		bdd::ref leaf = bdd::build_bdd(tau::get("r(a)", opts));
		CHECK(bdd::bdd_cofactor(bdd::T, tx, true) == bdd::T);
		CHECK(bdd::bdd_cofactor(leaf, tx, false) == leaf);
	}
}

TEST_SUITE("BDD xor") {
	using bdd = tau_term_bdd<node_t>;

	TEST_CASE("identities, and build_bdd of a bf_xor term agrees") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		bdd::ref f = bdd::build_bdd(tau::get("xy", opts));
		CHECK(bdd::bdd_xor(f, f) == bdd::F);
		CHECK(bdd::bdd_xor(f, bdd::F) == f);
		CHECK(bdd::bdd_xor(f, bdd::T) == bdd::bdd_not(f));
		tref l = tau::get("xy", opts), r = tau::get("x|z", opts);
		tref x = tau::build_bf_xor(l, r);
		REQUIRE(tau::get(x).child_is(tau::bf_xor));
		CHECK(bdd::build_bdd(x) == bdd::bdd_xor(
			bdd::build_bdd(l), bdd::build_bdd(r)));
	}
}

TEST_SUITE("BDD map_leaves") {
	using bdd = tau_term_bdd<node_t>;

	TEST_CASE("the identity functor gives the same ref back") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		bdd::ref f = bdd::build_bdd(tau::get("x & r(a) | x' & r(b)", opts));
		auto id = [](tref l) { return l; };
		CHECK(bdd::map_leaves(f, id) == f);
	}

	TEST_CASE("every leaf to one term: the leaves merge and the node folds") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		bdd::ref f = bdd::build_bdd(tau::get("x & r(a) | x' & r(b)", opts));
		tref c = tau::get("r(c)", opts);
		auto to_c = [c](tref) { return c; };
		bdd::ref r = bdd::map_leaves(f, to_c);
		CHECK(r == bdd::build_bdd(c));
		CHECK(bdd::node_count(r) == 1);
		// to a constant: the whole BDD collapses to a terminal
		tref one = tau::get("1", opts);
		auto to_1 = [one](tref) { return one; };
		CHECK(bdd::map_leaves(f, to_1) == bdd::T);
	}

	TEST_CASE("a leaf that gains a decision variable is re-canonicalised") {
		tau::get_options opts = { .parse = { .start = tau::bf } };
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		bdd::ref f = bdd::build_bdd(tau::get("p & r(a) | p' & r(b)", opts));
		tref a = tau::get("r(a)", opts);
		tref pc = tau::get("p & r(c)", opts);
		// r(a) ← p·r(c) brings the decision variable p into the leaf again
		auto put = [a, pc](tref l) {
			return tau::subtree_equals(l, a) ? pc : l; };
		bdd::ref r = bdd::map_leaves(f, put);
		CHECK(bdd::is_ordered(r));
		CHECK(r == bdd::build_bdd(tau::get("p & r(c) | p' & r(b)", opts)));
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
		tref tz = tau::trim(tau::get("z", opts));
		tref n = hbdd::convert_to_tau_node(tau::get("xy", opts));
		REQUIRE(hbdd::is_bdd_backed(n));
		// the bare term: no BDD_ID left, and the same BDD when rebuilt
		tref plain = hbdd::convert_to_tau_terms(n);
		CHECK(!has_bdd_id(plain));
		CHECK(bdd::build_bdd(plain) == hbdd::convert_to_handle(n).get());
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
		tref inner = hbdd::convert_to_tau_node(tau::get("xy", opts));
		// the inner node inside a reference argument, which is a LEAF of an
		// outer BDD over p -- so the term the outer conversion produces
		// still holds a BDD_ID, one the traversal does not re-enter
		tref shell = tau::get("p & r(a)", opts);
		tref term = rewriter::replace<node_t>(shell,
			tau::get("a", opts), inner);
		REQUIRE(has_bdd_id(term));
		tref outer = hbdd::convert_to_tau_node(term);
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
		tref ta = tau::trim(tau::get("a", opts));
		bdd::ref f = bdd::build_bdd(tau::get("x & r(a) | y & r(b)", opts));
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
		bdd::ref diamond = bdd::build_bdd(
			tau::get("x & r(z) | y & r(z)", opts));
		bdd::ref tree_shaped = bdd::build_bdd(tau::get("x & r(z)", opts));
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
		const size_t type = find_ba_type<node_t>(tau::get("x", opts));
		CHECK(tau::get(hbdd::convert_to_tau_node_or_term(
			hbdd(bdd::T), type)).equals_1());
		CHECK(tau::get(hbdd::convert_to_tau_node_or_term(
			hbdd(bdd::F), type)).equals_0());
		// a single leaf: its plain term
		hbdd leaf_h(bdd::add(tau::trim(tau::get("r(a)", opts))));
		tref lt = hbdd::convert_to_tau_node_or_term(leaf_h, type);
		CHECK(!hbdd::is_bdd_backed(lt));
		CHECK(tau::subtree_equals(lt, tau::get("r(a)", opts)));
		// a branching BDD: the node convert_to_tau_node interns
		hbdd h = hbdd::build(tau::get("xy", opts));
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
		tref n = hbdd::convert_to_tau_node(tau::get("xy", opts));
		CHECK(hbdd::is_bdd_backed(n));
		// the wrapper carrying a right sibling inside an atom
		tref atom = tau::build_bf_eq_0(n);
		CHECK(hbdd::is_bdd_backed(tau::get(atom)[0].first()));
		// the bare BDD_ID node is not one
		CHECK(!hbdd::is_bdd_backed(tau::get(n).first()));
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
		hbdd h = hbdd::build(tau::get("xy|x'a", opts));
		trefs v { ty, tx }, w { ty, tx };
		hbdd q = h.bdd_ex(v);
		CHECK(q.get() == bdd::bdd_ex(h.get(), w));
		// both lists come back sorted by the order, as the static leaves them
		CHECK(v == w);
		CHECK(v[0] == tx);
	}
}

namespace {

/// A `bf` term from source.
tref pbf(const char* s) {
	static tau::get_options opts = { .parse = { .start = tau::bf } };
	tref t = tau::get(s, opts);
	REQUIRE(t != nullptr);
	return t;
}
/// The trimmed `variable` node of a one-variable term — the shape
/// and quantifier subscripts hold.
tref pvar(const char* s) { return tau::trim(pbf(s)); }
/// The subscript of a functional-quantifier chain (its `bf` wrapper).
tref qvar(tref n) {
	return tau::trim_right_sibling(tau::get(n)[0].first());
}
/// The body of a functional-quantifier chain (its `bf` wrapper).
tref qbody(tref n) {
	return tau::trim_right_sibling(tau::get(n)[0].second());
}
/// The binder variable of canonical id @p id, of `pvar`'s type: it sorts
/// below every free variable, an outer (higher) id above an inner one.
tref bvar(size_t id) {
	return tau::build_variable(std::to_string(id),
		tau::get(pvar("q")).get_ba_type());
}
/// The `bf` term of `bvar(id)`.
tref bbf(size_t id) { return tau::get(tau::bf, bvar(id)); }

} // namespace

TEST_SUITE("BDD find_biggest_var_id") {
	using bdd = tau_term_bdd<node_t>;
	using hbdd = term_handle<node_t>;

	TEST_CASE("a BDD_ID is no black box: the stored BDD's ids count") {
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref q = pvar("q");
		// a numeric variable that becomes a DECISION variable, so it has
		// no tree node left in the term that carries the BDD
		tref v7 = tau::build_variable("7", tau::get(q).get_ba_type());
		tref plain = tau::build_bf_and(tau::get(tau::bf, v7),
			tau::get(tau::bf, q));
		CHECK(find_biggest_var_id<node_t>(plain) == 7);
		tref backed = hbdd::convert_to_tau_node(plain);
		REQUIRE(hbdd::is_bdd_backed(backed));
		// no variable node anywhere in the term itself
		CHECK(tau::get(backed).find_top([](tref m) {
			return tau::get(m).is(tau::variable); }) == nullptr);
		CHECK(find_biggest_var_id<node_t>(backed) == 7);
	}
}

TEST_SUITE("BDD build_functional_quantifiers") {
	using bdd = tau_term_bdd<node_t>;
	using hbdd = term_handle<node_t>;

	TEST_CASE("canonical: permutations, absent subscripts, merged runs") {
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref y = pvar("y"), z = pvar("z"), u = pvar("u");
		// `w` stays free, so nothing folds
		tref f = pbf("y & z & w");
		tref a = bdd::build_functional_quantifiers(
			{{y, bdd::all}, {z, bdd::all}}, f);
		tref b = bdd::build_functional_quantifiers(
			{{z, bdd::all}, {y, bdd::all}}, f);
		CHECK(a == b);
		CHECK(tau::get(a).child_is(tau::bf_fall));
		// a subscript that is not free in the body is dropped
		CHECK(bdd::build_functional_quantifiers({{u, bdd::all}}, f) == f);
		CHECK(bdd::build_functional_quantifiers({{u, bdd::all}, {y, bdd::all}}, f)
			== bdd::build_functional_quantifiers({{y, bdd::all}}, f));
		// an empty prefix and a constant body give the body back
		CHECK(bdd::build_functional_quantifiers({}, f) == f);
		CHECK(bdd::build_functional_quantifiers({{y, bdd::all}}, pbf("1"))
			== pbf("1"));
		// a body that is already a chain of the same kind merges into one
		// sorted run: `fall z (fall y ...)` is the node `fall {y,z} ...`
		tref inner = bdd::build_functional_quantifiers({{y, bdd::all}}, f);
		CHECK(bdd::build_functional_quantifiers({{z, bdd::all}}, inner) == a);
		// a repeated subscript binds at its innermost occurrence
		CHECK(bdd::build_functional_quantifiers(
			{{y, bdd::ex}, {y, bdd::all}}, f)
			== bdd::build_functional_quantifiers({{y, bdd::all}}, f));
	}

	TEST_CASE("mixed kinds nest in the given order") {
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref y = pvar("y"), z = pvar("z");
		tref f = pbf("y & z & w");
		tref m = bdd::build_functional_quantifiers(
			{{y, bdd::all}, {z, bdd::ex}}, f);
		REQUIRE(tau::get(m).child_is(tau::bf_fall));
		CHECK(tau::subtree_equals(qvar(m), y));
		tref in = qbody(m);
		REQUIRE(tau::get(in).child_is(tau::bf_fex));
		CHECK(tau::subtree_equals(qvar(in), z));
		CHECK(qbody(in) == f);
		// the other nesting is another node (the two runs do not commute)
		CHECK(bdd::build_functional_quantifiers(
			{{z, bdd::ex}, {y, bdd::all}}, f) != m);
	}

	TEST_CASE("fold: a chain binding every free variable of its body") {
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref y = pvar("y"), u = pvar("u");
		CHECK(tau::get(bdd::build_functional_quantifiers(
			{{y, bdd::all}}, pbf("y"))).equals_0());
		CHECK(tau::get(bdd::build_functional_quantifiers(
			{{y, bdd::ex}}, pbf("y"))).equals_1());
		// a variable-free leaf is the chain's value as it stands:
		// ∀y (y ∪ r(1)) = r(1), not a terminal and not a chain
		CHECK(tau::subtree_equals(bdd::build_functional_quantifiers(
			{{y, bdd::all}}, pbf("y | r(1)")), pbf("r(1)")));
		// `z` stays free: no fold
		tref c = bdd::build_functional_quantifiers({{y, bdd::all}},
			pbf("y | z"));
		CHECK(tau::get(c).child_is(tau::bf_fall));
		CHECK(!tau::get(c).equals_0());
		CHECK(!tau::get(c).equals_1());
		// a backed body folds too: ∀y ∀u (y ∪ u) = 0
		tref n = hbdd::convert_to_tau_node(pbf("y | u"));
		REQUIRE(hbdd::is_bdd_backed(n));
		CHECK(tau::get(bdd::build_functional_quantifiers(
			{{y, bdd::all}, {u, bdd::all}}, n)).equals_0());
		// a subscript hidden in a leaf is reached by no quantification:
		// the chain stands
		CHECK(tau::get(bdd::build_functional_quantifiers({{y, bdd::all}},
			pbf("y | r(y)"))).child_is(tau::bf_fall));
	}
}

TEST_SUITE("BDD build_bdd functional-quantifier chain") {
	using bdd = tau_term_bdd<node_t>;

	TEST_CASE("the slide: the variables above stay, a closed piece folds") {
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref y1 = bvar(1);
		// ∀1 (x·1 ∪ x′·z) = x·∀1 1 ∪ x′·∀1 z = x′·z: the free `x` and
		// `z` sit above the binder variable, and the piece `1` folds
		tref f = bdd::build_functional_quantifiers({{y1, bdd::all}},
			tau::build_bf_or(tau::build_bf_and(pbf("x"), bbf(1)),
				pbf("x' & z")));
		REQUIRE(tau::get(f).child_is(tau::bf_fall));
		bdd::ref r = bdd::build_bdd(f);
		CHECK(bdd::is_ordered(r));
		CHECK(r == bdd::build_bdd(pbf("x' & z")));
		// what surrounds the chain stays above it
		bdd::ref s = bdd::build_bdd(tau::build_bf_and(pbf("a"), f));
		REQUIRE(!bdd::leaf(s));
		CHECK(tau::subtree_equals(bdd::get_var(s), pvar("a")));
		CHECK(bdd::get_high(s) == r);
	}

	TEST_CASE("the slide: a piece holding a free variable in a leaf is one chain leaf") {
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref y1 = bvar(1);
		// ∀1 (x·(1 ∪ r(a)) ∪ x′·z): the piece under `x` holds `a` free in
		// a leaf, so it stays a chain over its own subscript, ONE LEAF
		tref piece = tau::build_bf_or(bbf(1), pbf("r(a)"));
		tref g = bdd::build_functional_quantifiers({{y1, bdd::all}},
			tau::build_bf_or(tau::build_bf_and(pbf("x"), piece),
				pbf("x' & z")));
		bdd::ref r = bdd::build_bdd(g);
		CHECK(bdd::is_ordered(r));
		REQUIRE(!bdd::leaf(r));
		CHECK(tau::subtree_equals(bdd::get_var(r), pvar("x")));
		CHECK(bdd::get_low(r) == bdd::from_bit(pvar("z")));
		bdd::ref hi = bdd::get_high(r);
		REQUIRE(bdd::leaf(hi));
		tref lt = bdd::get_var_term(hi);
		REQUIRE(tau::get(lt).child_is(tau::bf_fall));
		CHECK(tau::subtree_equals(qvar(lt), y1));
		CHECK(bdd::build_bdd(qbody(lt)) == bdd::build_bdd(piece));
		// the subscript is bound inside that leaf; `a` hides there
		const trefs lfv = term_handle<node_t>::get_free_leaf_vars(r.b);
		REQUIRE(lfv.size() == 1);
		CHECK(tau::subtree_equals(lfv[0], pvar("a")));
	}

	TEST_CASE("the slide hands every piece the whole prefix, one run at a time") {
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref y2 = bvar(2), y1 = bvar(1);
		// the piece under `x` is `2 ↔ 1`: ∀2 ∃1 of it is 1, ∃2 ∀1 of it
		// is 0, so the two nestings slide to x ∪ z and x′·z
		tref iff = tau::build_bf_or(tau::build_bf_and(bbf(2), bbf(1)),
			tau::build_bf_and(tau::build_bf_neg(bbf(2)),
				tau::build_bf_neg(bbf(1))));
		tref body = tau::build_bf_or(tau::build_bf_and(pbf("x"), iff),
			pbf("x' & z"));
		tref ae = bdd::build_functional_quantifiers(
			{{y2, bdd::all}, {y1, bdd::ex}}, body);
		REQUIRE(tau::get(ae).child_is(tau::bf_fall));
		CHECK(bdd::build_bdd(ae) == bdd::build_bdd(pbf("x | z")));
		tref ea = bdd::build_functional_quantifiers(
			{{y2, bdd::ex}, {y1, bdd::all}}, body);
		REQUIRE(tau::get(ea).child_is(tau::bf_fex));
		CHECK(bdd::build_bdd(ea) == bdd::build_bdd(pbf("x' & z")));
	}

	TEST_CASE("a chain over a stored BDD slides like its spelling") {
		using hbdd = term_handle<node_t>;
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref y1 = bvar(1);
		// §7 DISCHARGE's shape: a chain over a STORED BDD
		tref plain = tau::build_bf_or(tau::build_bf_and(pbf("x"),
			tau::build_bf_and(bbf(1), pbf("r(b)"))), pbf("x' & a"));
		tref stored = hbdd::convert_to_tau_node(plain);
		REQUIRE(hbdd::is_bdd_backed(stored));
		tref chain = bdd::build_functional_quantifiers(
			{{y1, bdd::all}}, stored);
		REQUIRE(tau::get(chain).child_is(tau::bf_fall));
		REQUIRE(qbody(chain) == stored);
		bdd::ref r = bdd::build_bdd(chain);
		CHECK(bdd::is_ordered(r));
		CHECK(r == bdd::build_bdd(bdd::build_functional_quantifiers(
			{{y1, bdd::all}}, plain)));
		// every piece is spelled plain: a chain leaf, no stored BDD left
		size_t chains = 0;
		auto plain_leaf = [&chains](bdd::ref n, bool is_leaf) {
			if (!is_leaf) return true;
			tref t = bdd::get_var_term(n);
			if (tau::get(t).child_is(tau::bf_fall)) ++chains;
			return tau::get(t).find_top([](tref m) {
				return tau::get(m).is(tau::BDD_ID); }) == nullptr;
		};
		CHECK(bdd::visit_nodes(r, plain_leaf));
		CHECK(chains == 1);
	}

	TEST_CASE("an input chain goes through the constructor on its way in") {
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref ty = pvar("y"), tz = pvar("z");
		// either way round the run comes out in content order: one BDD,
		// one chain leaf, since `w` hides in the body's leaf
		tref body = pbf("y & z & r(w)");
		bdd::ref rr = bdd::build_bdd(tau::build_bf_fall(tz,
			tau::build_bf_fall(ty, body, false), false));
		CHECK(rr == bdd::build_bdd(tau::build_bf_fall(ty,
			tau::build_bf_fall(tz, body, false), false)));
		CHECK(rr == bdd::build_bdd(bdd::build_functional_quantifiers(
			{{ty, bdd::all}, {tz, bdd::all}}, body)));
		REQUIRE(bdd::leaf(rr));
		CHECK(tau::get(bdd::get_var_term(rr)).child_is(tau::bf_fall));
		// a closed one folds: `fall y y` is F
		CHECK(bdd::build_bdd(
			tau::build_bf_fall(ty, pbf("y"), false)) == bdd::F);
		// a degenerate subscript is dropped and what is left is built:
		// `fall y x` is the variable x
		CHECK(bdd::build_bdd(tau::build_bf_fall(ty, pbf("x"), false))
			== bdd::from_bit(pvar("x")));
	}

	TEST_CASE("a hazard leaf terminates in one leaf") {
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref ty = pvar("y");
		// `r(x)` hides `x` inside a reference argument
		tref f = bdd::build_functional_quantifiers({{ty, bdd::all}},
			pbf("y & r(x)"));
		REQUIRE(tau::get(f).child_is(tau::bf_fall));
		bdd::ref r = bdd::build_bdd(f);
		CHECK(bdd::leaf(r));
		CHECK(bdd::node_count(r) == 1);
		CHECK(tau::get(bdd::get_var_term(r)).child_is(tau::bf_fall));
	}

	TEST_CASE("shadowing: a bound subscript spelled like a free variable stays bound") {
#ifdef TAU_CACHE
		bdd::clear_caches();
#endif
		tref q = pvar("q");
		tref one = tau::build_variable("1", tau::get(q).get_ba_type());
		tref b1 = tau::get(tau::bf, one);
		tref chain = bdd::build_functional_quantifiers({{one, bdd::all}},
			tau::build_bf_or(b1, tau::get(tau::bf, q)));
		REQUIRE(tau::get(chain).child_is(tau::bf_fall));
		// the chain holds only `q` free: its own `1` is bound, and the
		// slide quantifies it away, ∀1 (1 ∪ q) = q
		CHECK(get_free_vars<node_t>(chain).size() == 1);
		CHECK(bdd::build_bdd(chain) == bdd::from_bit(q));
		// the free `1` beside it stays a decision variable
		tref f = tau::build_bf_and(b1, chain);
		bdd::ref r = bdd::build_bdd(f);
		CHECK(r == bdd::bdd_and(bdd::from_bit(one), bdd::from_bit(q)));
		// as a function: the free `1` set to 1 leaves the chain, set to 0
		// gives 0 — the bound `1` is untouched either way
		CHECK(bdd::bdd_cofactor(r, one, true) == bdd::build_bdd(chain));
		CHECK(bdd::bdd_cofactor(r, one, false) == bdd::F);
	}
}

#ifdef TAU_CACHE
TEST_SUITE("BDD operation tables") {
	using bdd = tau_term_bdd<node_t>;

	TEST_CASE("a tree collection clears the tables, never the node store") {
		bdd::clear_caches();
		tref s = pbf("x & y | x' & r(c)");
		bdd::ref r1 = bdd::build_bdd(s);
		REQUIRE(!bdd::and_memo.empty());
		{
			htref keep_s = tau::geth(s);
			std::unordered_set<tref> keep;
			bdd::collect_live_refs(keep);
			tau::gc(keep);
			CHECK(bdd::and_memo.empty());
			CHECK(bdd::and_many_memo.empty());
			CHECK(bdd::ex_memo.empty());
			CHECK(bdd::ite_memo.empty());
			// the nodes are where they were: the same BDD comes back
			CHECK(bdd::build_bdd(s) == r1);
		}
	}
}
#endif
