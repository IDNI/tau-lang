// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Elimination of definitional existentials (boolean_algebras/bv/heuristics/
// bv_definitional_elimination.h, opt-in via bv_definitional_elimination).
// Every step is an identity on the formula, so normalization with the switch
// on must agree with the pipeline without it -- where a total definition is
// found and substituted, where the definition is unread and the quantifier
// folds to T, and where the pass must decline (partial definition,
// non-exclusive cells, a witness under a foreign binder, a reader that is no
// definition).

#include "test_integration-interpreter_helper.h"

namespace {

struct defelim_config {
	bool prev;
	defelim_config(bool on) : prev(bv_definitional_elimination) {
		bv_definitional_elimination = on;
	}
	~defelim_config() { bv_definitional_elimination = prev; }
};

tref parse_wff(const std::string& sample) {
	static tree<node_t>::get_options opts{ .parse = { .start = tree<node_t>::wff }};
	auto src = tree<node_t>::get(sample, opts);
	if (!src.has_value()) { src.print(); return nullptr; }
	return src.value();
}

tref normalized(const std::string& sample, bool on) {
	defelim_config c(on);
	auto wff = parse_wff(sample);
	if (!wff) return nullptr;
	auto r = normalizer<node_t>(wff);
	return r.has_value() ? r.value() : nullptr;
}

std::string norm(const std::string& sample, bool on) {
	tref r = normalized(sample, on);
	return r ? tau::get(r).to_str() : "parse_error";
}

// Same verdict with and without the switch: equal normal forms, or the
// equivalence of the two results, closed universally over their free
// variables, normalizes to T. (The closure matters: the two pipelines spell
// order atoms differently, and only the closed formula is decided by the
// solver rather than by the propositional normal form.)
bool agree(const std::string& sample) {
	tref off = normalized(sample, false), on = normalized(sample, true);
	if (!off || !on) return false;
	if (tau::get(off) == tau::get(on)) return true;
	defelim_config c(false);
	tref eq = tau::build_wff_equiv(off, on);
	const trefs& fv = get_free_vars<node_t>(eq);
	tref closed = fv.empty() ? eq : tau::build_wff_all_many(fv, eq);
	auto r = normalizer<node_t>(closed);
	return r.has_value() && tau::get(r.value()).equals_T();
}

// The block form directly, on a parsed `ex ...` formula: is the binder gone?
bool binder_removed(const std::string& sample) {
	tref fm = parse_wff(sample);
	if (!fm) return false;
	tref r = bv_eliminate_definitional_existentials<node_t>(fm);
	return tau::get(r).find_top(is_quantifier<node_t>) == nullptr;
}

strings run_spec(const std::string& spec_src, bool on) {
	defelim_config c(on);
	bdd_init<Bool>();
	auto spec = create_spec(spec_src.c_str());
	io_context<node_t> ctx;
	strings i1_values = { "1", "5", "2", "7" };
	ctx.add_input("i1", bv_type_id<node_t>(8),
		std::make_shared<vector_input_stream>(i1_values));
	auto o2 = std::make_shared<vector_output_stream>();
	ctx.add_output("o2", bv_type_id<node_t>(8), o2);
	auto maybe_i = run<node_t>(spec, ctx, 4);
	REQUIRE(maybe_i.has_value());
	return o2->get_values();
}

} // namespace

TEST_SUITE("configuration") {
	TEST_CASE("bdd_init") { bdd_init<Bool>(); }
	TEST_CASE("logging") { logging::trace(); }
}

TEST_SUITE("bv definitional elimination") {

	TEST_CASE("an equation defines: substituted where read, T where unread") {
		CHECK(agree("ex x (x:bv[8] = { 5 }:bv[8] && x:bv[8] < { 7 }:bv[8])"));
		CHECK(norm("ex x (x:bv[8] = { 5 }:bv[8] && x:bv[8] < { 7 }:bv[8])", true) == "T");
		CHECK(binder_removed("ex x (x:bv[8] = y:bv[8] + { 1 }:bv[8] && x:bv[8] < { 7 }:bv[8])"));
		CHECK(agree("ex x (x:bv[8] = y:bv[8] + { 1 }:bv[8] && x:bv[8] < { 7 }:bv[8])"));
	}

	TEST_CASE("a chain of equations under nested binders") {
		// seed -> next -> dice, each defined in an inner scope of the block.
		CHECK(binder_removed("ex a ex b ex c (a:bv[8] = y:bv[8] ^ z:bv[8] && b:bv[8] = a:bv[8] + { 3 }:bv[8] && c:bv[8] = b:bv[8] % { 6 }:bv[8] && c:bv[8] < { 6 }:bv[8])"));
		CHECK(agree("ex a ex b ex c (a:bv[8] = y:bv[8] ^ z:bv[8] && b:bv[8] = a:bv[8] + { 3 }:bv[8] && c:bv[8] = b:bv[8] % { 6 }:bv[8] && c:bv[8] < { 6 }:bv[8])"));
	}

	TEST_CASE("a conditional defines: exclusive cells") {
		// (c < 1 ? x = 4 : x = 3) in clause form, then read.
		const char* s = "ex x ((c:bv[8] < { 1 }:bv[8] || x:bv[8] = { 3 }:bv[8]) && (c:bv[8] !< { 1 }:bv[8] || x:bv[8] = { 4 }:bv[8]) && (x:bv[8] = { 3 }:bv[8] || a = 0))";
		CHECK(agree(s));
		CHECK(binder_removed(s));
		// unread: the whole quantifier is T
		CHECK(norm("ex x ((c:bv[8] < { 1 }:bv[8] || x:bv[8] = { 3 }:bv[8]) && (c:bv[8] !< { 1 }:bv[8] || x:bv[8] = { 4 }:bv[8]))", true) == "T");
	}

	TEST_CASE("a nested conditional after CNF distribution") {
		// (c < 1 ? x = 4 : (c < 2 ? x = 5 : x = 6)) as flat clauses.
		const char* s = "ex x ((c:bv[8] !< { 1 }:bv[8] || x:bv[8] = { 4 }:bv[8]) && (c:bv[8] < { 1 }:bv[8] || c:bv[8] !< { 2 }:bv[8] || x:bv[8] = { 5 }:bv[8]) && (c:bv[8] < { 1 }:bv[8] || c:bv[8] < { 2 }:bv[8] || x:bv[8] = { 6 }:bv[8]) && (x:bv[8] != { 5 }:bv[8] || a = 0))";
		CHECK(agree(s));
		CHECK(binder_removed(s));
	}

	TEST_CASE("a guard that is a disjunction of conjunctions") {
		// pid != k ? (hit ? x = 1 : x = 0) : x = 0, with hit = (q = 2 && r = 3) || q = 4
		const char* s = "ex x ((p:bv[8] = { 1 }:bv[8] || (q:bv[8] = { 2 }:bv[8] && r:bv[8] = { 3 }:bv[8]) || q:bv[8] = { 4 }:bv[8] || x:bv[8] = { 0 }:bv[8]) && (p:bv[8] = { 1 }:bv[8] || q:bv[8] != { 2 }:bv[8] || r:bv[8] != { 3 }:bv[8] || x:bv[8] = { 1 }:bv[8]) && (p:bv[8] = { 1 }:bv[8] || q:bv[8] != { 4 }:bv[8] || x:bv[8] = { 1 }:bv[8]) && (p:bv[8] != { 1 }:bv[8] || x:bv[8] = { 0 }:bv[8]) && (x:bv[8] = { 1 }:bv[8] || a = 0))";
		CHECK(agree(s));
		CHECK(binder_removed(s));
	}

	TEST_CASE("two equations for one variable: one defines, the other stays a condition") {
		CHECK(agree("ex x (x:bv[8] = y:bv[8] + { 1 }:bv[8] && x:bv[8] = z:bv[8] && x:bv[8] < { 9 }:bv[8])"));
		CHECK(binder_removed("ex x (x:bv[8] = y:bv[8] + { 1 }:bv[8] && x:bv[8] = z:bv[8] && x:bv[8] < { 9 }:bv[8])"));
		CHECK(norm("ex x (x:bv[8] = { 1 }:bv[8] && x:bv[8] = { 2 }:bv[8])", true) == "F");
	}

	TEST_CASE("declines: a partial definition keeps its binder") {
		const char* s = "ex x ((c:bv[8] < { 1 }:bv[8] || x:bv[8] = { 3 }:bv[8]) && x:bv[8] < { 5 }:bv[8])";
		CHECK(agree(s));
		CHECK(!binder_removed(s));
	}

	TEST_CASE("non-exclusive cells keep the consistency atoms") {
		// D1 = c < 2, D2 = c < 5, D3 = c !< 2: exhaustive, but cells 2 and 3 overlap.
		const char* s = "ex x ((c:bv[8] < { 2 }:bv[8] || x:bv[8] = { 3 }:bv[8]) && (c:bv[8] < { 5 }:bv[8] || x:bv[8] = { 4 }:bv[8]) && (c:bv[8] !< { 2 }:bv[8] || x:bv[8] = { 6 }:bv[8]) && (x:bv[8] = { 4 }:bv[8] || a = 0))";
		CHECK(agree(s));
	}

	TEST_CASE("declines: the witness sits under a foreign binder of another kind") {
		const char* s = "ex x (all y:bv[8] (x:bv[8] = y:bv[8] + { 1 }:bv[8] || a = 0) && x:bv[8] < { 3 }:bv[8])";
		CHECK(agree(s));
		CHECK(!binder_removed(s));
	}

	TEST_CASE("declines: a reader is no definition") {
		CHECK(agree("ex x (x:bv[8] < { 3 }:bv[8] && (x:bv[8] = { 1 }:bv[8] || a = 0))"));
		CHECK(!binder_removed("ex x (x:bv[8] < { 3 }:bv[8] && (x:bv[8] = { 1 }:bv[8] || a = 0))"));
	}

	TEST_CASE("declines: the variable is read outside the scope of its definition") {
		// x is defined in the inner scope but also read beside it.
		const char* s = "ex x (ex y (x:bv[8] = y:bv[8] + { 1 }:bv[8] && y:bv[8] < { 3 }:bv[8]) && x:bv[8] < { 9 }:bv[8])";
		CHECK(agree(s));
		CHECK(!binder_removed(s));
	}

	TEST_CASE("an inner binder of the same name is the one eliminated") {
		const char* s = "ex x (ex x (x:bv[8] = { 5 }:bv[8] && x:bv[8] < { 7 }:bv[8]) && x:bv[8] > { 3 }:bv[8])";
		CHECK(agree(s));
		tref r = bv_eliminate_definitional_existentials<node_t>(parse_wff(s));
		REQUIRE(r != nullptr);
		// The outer binder stays (x is read by `x > 3` outside the inner
		// scope), the inner one is gone, and no x is left free.
		CHECK(tau::get(r).select_all(is<node_t, tau::wff_ex>).size() == 1);
		CHECK(get_free_vars<node_t>(r).empty());
	}

	TEST_CASE("the clause cap turns a wide conjunct into a reader") {
		// Flattening (c < 1 || (x = 3 && a = 0 && b = 0)) needs three pieces.
		const char* s = "ex x ((c:bv[8] < { 1 }:bv[8] || (x:bv[8] = { 3 }:bv[8] && a = 0 && b = 0)) && (c:bv[8] !< { 1 }:bv[8] || x:bv[8] = { 4 }:bv[8]) && (x:bv[8] = { 3 }:bv[8] || d = 0))";
		CHECK(agree(s));
		const size_t saved = bv_defelim_max_clauses;
		bv_defelim_max_clauses = 1;
		const bool declined = !binder_removed(s);
		bv_defelim_max_clauses = saved;
		CHECK(declined);
		CHECK(binder_removed(s));
	}

	TEST_CASE("a run with a computed value read by a guard: same outputs") {
		const char* spec = "(o1[t]:bv[8] = i1[t]:bv[8] + { 1 }:bv[8]) && ((o1[t]:bv[8] > { 3 }:bv[8]) ? (o2[t]:bv[8] = { 1 }:bv[8]) : (o2[t]:bv[8] = { 0 }:bv[8])).";
		CHECK(run_spec(spec, false) == run_spec(spec, true));
		CHECK(run_spec(spec, true) == strings{ "0", "1", "0", "1" });
	}
}
