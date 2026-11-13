// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <sstream>

#include "test_init.h"
#include "test_tau_helpers.h"

using tau_api = api<node_t>;

#include "test_api-fixture.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "testing"

// Every htref entry point is checked against the tref entry point of the
// same name on the same input: a handle answers what the tree it holds
// answers, and a value comes back as a live handle.

// The handle holds the tree, up to the order of and/or.
static bool holds(const htref& h, tref t) {
	return h != nullptr && t != nullptr
		&& matches_tree_mod_and_or(h->get(), t);
}

// Both calls gave a value and the handle holds the tree.
static bool holds(const result<htref>& h, const result<tref>& t) {
	return h.has_value() && t.has_value() && holds(h.value(), t.value());
}

static bool is_parse_failure(const result<htref>& r) {
	return !r.has_value() && r.has_error()
		&& report_has_code(r.report(), code::parse_error);
}

static bool is_invalid_argument(const auto& r) {
	return !r.has_value() && r.has_error()
		&& report_has_code(r.report(), code::invalid_argument);
}

// The handle solution maps each variable to the value the tref solution
// maps the same variable to.
static bool same_solution(const std::map<htref, htref>& hs,
	const subtree_map<node_t, tref>& ts)
{
	if (hs.size() != ts.size()) return false;
	for (const auto& [var, val] : hs) {
		if (!var || !val) return false;
		auto it = ts.find(var->get());
		if (it == ts.end()
			|| !matches_tree_mod_and_or(val->get(), it->second))
				return false;
	}
	return true;
}

TEST_SUITE("Tau API - htref - parsing") {

	TEST_CASE_FIXTURE(api_fixture, "geth_term") {
		for (const auto& term : terms) {
			CAPTURE(term);
			CHECK(holds(tau_api::geth_term(term),
				tau_api::get_term(term)));
		}
		CHECK(is_parse_failure(tau_api::geth_term("x +")));
	}

	TEST_CASE_FIXTURE(api_fixture, "geth_formula") {
		for (const auto& formula : formulas) {
			CAPTURE(formula);
			CHECK(holds(tau_api::geth_formula(formula),
				tau_api::get_formula(formula)));
		}
		CHECK(is_parse_failure(tau_api::geth_formula("&& x =")));
	}

	TEST_CASE_FIXTURE(api_fixture, "geth_function_def") {
		for (const auto& function_def : function_defs) {
			CAPTURE(function_def);
			CHECK(holds(tau_api::geth_function_def(function_def),
				tau_api::get_function_def(function_def)));
		}
		CHECK(is_parse_failure(tau_api::geth_function_def("hp_f(x) :=")));
		CHECK(is_invalid_argument(
			tau_api::geth_function_def("hp_f(x) := x = 0")));
	}

	TEST_CASE_FIXTURE(api_fixture, "geth_predicate_def") {
		for (const auto& predicate_def : predicate_defs) {
			CAPTURE(predicate_def);
			CHECK(holds(tau_api::geth_predicate_def(predicate_def),
				tau_api::get_predicate_def(predicate_def)));
		}
		CHECK(is_parse_failure(tau_api::geth_predicate_def(":= x = 0")));
		CHECK(is_invalid_argument(
			tau_api::geth_predicate_def("hp_p(x) := x'")));
	}

	TEST_CASE_FIXTURE(api_fixture, "geth_stream_def") {
		for (const auto& stream_def : stream_defs) {
			CAPTURE(stream_def);
			CHECK(holds(tau_api::geth_stream_def(stream_def),
				tau_api::get_stream_def(stream_def)));
		}
		auto bad = tau_api::geth_stream_def("not a stream def");
		CHECK(!bad.has_value());
		CHECK(bad.has_error());
	}

	TEST_CASE_FIXTURE(api_fixture, "geth_spec") {
		for (const auto& spec : specs) {
			CAPTURE(spec);
			CHECK(holds(tau_api::geth_spec(spec), tau_api::get_spec(spec)));
		}
		CHECK(is_parse_failure(tau_api::geth_spec(") = (")));
	}

	TEST_CASE_FIXTURE(api_fixture, "geth_definition") {
		for (const auto& definition : predicate_defs) {
			CAPTURE(definition);
			CHECK(holds(tau_api::geth_definition(definition),
				tau_api::get_definition(definition)));
		}
		for (const auto& definition : function_defs) {
			CAPTURE(definition);
			CHECK(holds(tau_api::geth_definition(definition),
				tau_api::get_definition(definition)));
		}
		auto bad = tau_api::geth_definition("this is not a definition");
		CHECK(!bad.has_value());
		CHECK(bad.has_error());
	}

	TEST_CASE_FIXTURE(api_fixture, "geth_spec_or_term") {
		for (const auto& spec : specs) {
			CAPTURE(spec);
			auto h = tau_api::geth_spec_or_term(spec);
			CHECK(holds(h, tau_api::get_spec_or_term(spec)));
			REQUIRE(h.has_value());
			CHECK(tau::get(h.value()).is(tau::spec));
		}
		for (const auto& term : terms) {
			CAPTURE(term);
			auto h = tau_api::geth_spec_or_term(term);
			CHECK(holds(h, tau_api::get_spec_or_term(term)));
			REQUIRE(h.has_value());
			CHECK(tau::get(h.value()).is(tau::bf));
		}
		CHECK(is_parse_failure(tau_api::geth_spec_or_term("&& x =")));
	}

	TEST_CASE_FIXTURE(api_fixture, "geth_formula_or_term") {
		for (const auto& formula : formulas) {
			CAPTURE(formula);
			auto h = tau_api::geth_formula_or_term(formula);
			CHECK(holds(h, tau_api::get_formula_or_term(formula)));
			REQUIRE(h.has_value());
			CHECK(tau::get(h.value()).is(tau::wff));
		}
		for (const auto& term : terms) {
			CAPTURE(term);
			auto h = tau_api::geth_formula_or_term(term);
			CHECK(holds(h, tau_api::get_formula_or_term(term)));
			REQUIRE(h.has_value());
			CHECK(tau::get(h.value()).is(tau::bf));
		}
		CHECK(is_parse_failure(tau_api::geth_formula_or_term("&& x =")));
	}
}

TEST_SUITE("Tau API - htref - querying") {

	TEST_CASE_FIXTURE(api_fixture, "contains") {
		auto t_r = tau_api::geth_term("x + 0", false);
		REQUIRE(t_r.has_value());
		htref t = t_r.value();
		tref tt = t->get();
		for (auto nt : { tau::bf, tau::bf_add, tau::variable, tau::bf_f,
				tau::bf_t, tau::wff, tau::wff_f, tau::wff_t })
		{
			CHECK(tau_api::contains(t, nt) == tau_api::contains(tt, nt));
		}
		CHECK(tau_api::contains(t, tau::bf_add));
		CHECK(!tau_api::contains(t, tau::wff));

		auto f_r = tau_api::geth_formula("x = 0 && y ^ 1 = 1", false);
		REQUIRE(f_r.has_value());
		htref f = f_r.value();
		tref tf = f->get();
		for (auto nt : { tau::wff, tau::wff_and, tau::bf_eq, tau::bf_xor,
				tau::variable, tau::bf, tau::bf_f, tau::bf_t,
				tau::wff_t, tau::wff_f, tau::spec })
		{
			CHECK(tau_api::contains(f, nt) == tau_api::contains(tf, nt));
		}
		CHECK(tau_api::contains(f, tau::wff_and));
		CHECK(!tau_api::contains(f, tau::spec));
		CHECK(!tau_api::contains(htref{}, tau::bf));
	}

	TEST_CASE_FIXTURE(api_fixture, "is_term and is_formula") {
		for (const auto& term : terms) {
			CAPTURE(term);
			auto h = tau_api::geth_formula_or_term(term);
			REQUIRE(h.has_value());
			CHECK(tau_api::is_term(h.value()));
			CHECK(!tau_api::is_formula(h.value()));
			CHECK(tau_api::is_term(h.value())
				== tau_api::is_term(h.value()->get()));
		}
		for (const auto& formula : formulas) {
			CAPTURE(formula);
			auto h = tau_api::geth_formula_or_term(formula);
			REQUIRE(h.has_value());
			CHECK(tau_api::is_formula(h.value()));
			CHECK(!tau_api::is_term(h.value()));
			CHECK(tau_api::is_formula(h.value())
				== tau_api::is_formula(h.value()->get()));
		}
		CHECK(!tau_api::is_term(htref{}));
		CHECK(!tau_api::is_formula(htref{}));
	}
}

// Definitions go into one process-wide registry as soon as they parse, so
// each case names symbols no other case of this file names.
TEST_SUITE("Tau API - htref - using definitions") {

	TEST_CASE_FIXTURE(api_fixture, "apply_def") {
		auto def = tau_api::geth_function_def("hd_f(x) := x + 1");
		auto expr = tau_api::geth_term("hd_f(t)", false);
		REQUIRE(def.has_value());
		REQUIRE(expr.has_value());
		CHECK(tau_api::contains(expr.value(), tau::ref));
		auto applied = tau_api::apply_def(def.value(), expr.value());
		REQUIRE(applied.has_value());
		CHECK(!tau_api::contains(applied.value(), tau::ref));
		CHECK(holds(applied, tau_api::apply_def(def.value()->get(),
			expr.value()->get())));
		CHECK(is_invalid_argument(tau_api::apply_def(htref{}, expr.value())));
		CHECK(is_invalid_argument(tau_api::apply_def(def.value(), htref{})));
	}

	TEST_CASE_FIXTURE(api_fixture, "apply_defs") {
		auto f = tau_api::geth_function_def("hd_g(x) := x + 1");
		auto g = tau_api::geth_function_def("hd_h(x) := x'");
		auto expr = tau_api::geth_term("hd_g(t) + hd_h(t)", false);
		REQUIRE(f.has_value());
		REQUIRE(g.has_value());
		REQUIRE(expr.has_value());
		CHECK(tau_api::contains(expr.value(), tau::ref));
		auto applied = tau_api::apply_defs(
			std::set<htref>{ f.value(), g.value() }, expr.value());
		REQUIRE(applied.has_value());
		CHECK(!tau_api::contains(applied.value(), tau::ref));
		CHECK(holds(applied, tau_api::apply_defs(
			subtree_set<node_t>{ f.value()->get(), g.value()->get() },
			expr.value()->get())));
		CHECK(is_invalid_argument(
			tau_api::apply_defs(std::set<htref>{ f.value() }, htref{})));
	}

	TEST_CASE_FIXTURE(api_fixture, "apply_all_defs") {
		auto spec = tau_api::geth_spec("hd_k(x) := x'.\nhd_k(z) = 0.");
		REQUIRE(spec.has_value());
		CHECK(tau_api::contains(spec.value(), tau::ref));
		auto applied = tau_api::apply_all_defs(spec.value());
		REQUIRE(applied.has_value());
		CHECK(!tau_api::contains(applied.value(), tau::ref));
		CHECK(matches_wff_mod_and_or(applied.value()->get(), "z' = 0"));
		CHECK(holds(applied, tau_api::apply_all_defs(spec.value()->get())));
		CHECK(is_invalid_argument(tau_api::apply_all_defs(htref{})));
	}
}

TEST_SUITE("Tau API - htref - printing") {

	TEST_CASE_FIXTURE(api_fixture, "print") {
		auto fm = tau_api::geth_formula("x = 0 && y = 1");
		REQUIRE(fm.has_value());
		std::ostringstream hos, tos;
		tau_api::print(hos, fm.value());
		tau_api::print(tos, fm.value()->get());
		CHECK(!hos.str().empty());
		CHECK(hos.str() == tos.str());
		std::ostringstream null_os;
		tau_api::print(null_os, htref{});
		CHECK(null_os.str().empty());
	}

	TEST_CASE_FIXTURE(api_fixture, "to_str") {
		auto fm = tau_api::geth_formula("x = 0 && y = 1");
		REQUIRE(fm.has_value());
		CHECK(tau_api::to_str(fm.value()) == "x = 0 && y = 1");
		CHECK(tau_api::to_str(fm.value())
			== tau_api::to_str(fm.value()->get()));
		CHECK(tau_api::to_str(htref{}) == "");
	}
}

TEST_SUITE("Tau API - htref - substitution") {

	TEST_CASE_FIXTURE(api_fixture, "substitute") {
		auto x = tau_api::geth_term("x");
		auto y = tau_api::geth_term("y");
		auto expr = tau_api::geth_term("x + 1");
		REQUIRE(x.has_value());
		REQUIRE(y.has_value());
		REQUIRE(expr.has_value());
		auto hs = tau_api::substitute(expr.value(), x.value(), y.value());
		REQUIRE(hs.has_value());
		CHECK(tau_api::to_str(hs.value()) == "y+1");
		CHECK(holds(hs, tau_api::substitute(expr.value()->get(),
			x.value()->get(), y.value()->get())));
		CHECK(is_invalid_argument(
			tau_api::substitute(htref{}, x.value(), y.value())));
		CHECK(is_invalid_argument(
			tau_api::substitute(expr.value(), htref{}, y.value())));
		CHECK(is_invalid_argument(
			tau_api::substitute(expr.value(), x.value(), htref{})));
	}

	TEST_CASE_FIXTURE(api_fixture, "substitute map") {
		auto x = tau_api::geth_term("x");
		auto y = tau_api::geth_term("y");
		auto a = tau_api::geth_term("a");
		auto b = tau_api::geth_term("b");
		auto expr = tau_api::geth_term("x + a");
		REQUIRE(x.has_value());
		REQUIRE(y.has_value());
		REQUIRE(a.has_value());
		REQUIRE(b.has_value());
		REQUIRE(expr.has_value());
		std::map<htref, htref> that_with{
			{ x.value(), y.value() }, { a.value(), b.value() } };
		auto hs = tau_api::substitute(expr.value(), that_with);
		REQUIRE(hs.has_value());
		CHECK(tau_api::to_str(hs.value()) == "y+b");
		CHECK(holds(hs, tau_api::substitute(expr.value()->get(),
			std::map<tref, tref>{ { x.value()->get(), y.value()->get() },
				{ a.value()->get(), b.value()->get() } })));
		CHECK(is_invalid_argument(tau_api::substitute(htref{}, that_with)));
		CHECK(is_invalid_argument(tau_api::substitute(expr.value(),
			std::map<htref, htref>{ { x.value(), htref{} } })));
	}

	// The pairs apply at once: a swap exchanges the variables.
	TEST_CASE_FIXTURE(api_fixture, "substitute map is simultaneous") {
		auto x = tau_api::geth_term("x");
		auto y = tau_api::geth_term("y");
		auto expr = tau_api::geth_term("x + y'");
		REQUIRE(x.has_value());
		REQUIRE(y.has_value());
		REQUIRE(expr.has_value());
		auto hs = tau_api::substitute(expr.value(), std::map<htref, htref>{
			{ x.value(), y.value() }, { y.value(), x.value() } });
		REQUIRE(hs.has_value());
		CHECK(tau_api::to_str(hs.value()) == "y+x'");
	}
}

TEST_SUITE("Tau API - htref - normal forms") {

	TEST_CASE_FIXTURE(api_fixture, "boole_normal_form") {
		auto fm = tau_api::geth_formula("x = 0 && y = 1");
		REQUIRE(fm.has_value());
		auto h = tau_api::boole_normal_form(fm.value());
		REQUIRE(h.has_value());
		CHECK(matches_wff_mod_and_or(h.value()->get(), "x = 0 && y' = 0"));
		CHECK(holds(h, tau_api::boole_normal_form(fm.value()->get())));
		CHECK(is_invalid_argument(tau_api::boole_normal_form(htref{})));
	}

	TEST_CASE_FIXTURE(api_fixture, "dnf") {
		auto fm = tau_api::geth_formula("(x = 0 || y = 0) && z = 0");
		REQUIRE(fm.has_value());
		auto h = tau_api::dnf(fm.value());
		REQUIRE(h.has_value());
		CHECK(tau_api::contains(h.value(), tau::wff_or));
		CHECK(holds(h, tau_api::dnf(fm.value()->get())));
		CHECK(is_invalid_argument(tau_api::dnf(htref{})));
	}

	TEST_CASE_FIXTURE(api_fixture, "cnf") {
		auto fm = tau_api::geth_formula("x = 0 || (y = 0 && z = 0)");
		REQUIRE(fm.has_value());
		auto h = tau_api::cnf(fm.value());
		REQUIRE(h.has_value());
		CHECK(tau_api::contains(h.value(), tau::wff_and));
		CHECK(holds(h, tau_api::cnf(fm.value()->get())));
		CHECK(is_invalid_argument(tau_api::cnf(htref{})));
	}

	TEST_CASE_FIXTURE(api_fixture, "nnf") {
		auto fm = tau_api::geth_formula("!(x = 0 && y = 0)");
		REQUIRE(fm.has_value());
		auto h = tau_api::nnf(fm.value());
		REQUIRE(h.has_value());
		CHECK(tau_api::contains(h.value(), tau::wff_or));
		CHECK(!tau_api::contains(h.value(), tau::wff_neg));
		CHECK(holds(h, tau_api::nnf(fm.value()->get())));
		CHECK(is_invalid_argument(tau_api::nnf(htref{})));
	}

	TEST_CASE_FIXTURE(api_fixture, "onf") {
		auto fm = tau_api::geth_formula("x & y = 0");
		auto x = tau_api::geth_term("x");
		REQUIRE(fm.has_value());
		REQUIRE(x.has_value());
		auto h = tau_api::onf(fm.value(), x.value());
		REQUIRE(h.has_value());
		CHECK(tau_api::is_formula(h.value()));
		CHECK(holds(h, tau_api::onf(fm.value()->get(), x.value()->get())));
		CHECK(is_invalid_argument(tau_api::onf(fm.value(), htref{})));
		CHECK(is_invalid_argument(tau_api::onf(htref{}, x.value())));
	}

	TEST_CASE_FIXTURE(api_fixture, "pnf reports what the tref overload reports") {
		auto fm = tau_api::geth_formula("(ex x x = 0) && y = 0");
		REQUIRE(fm.has_value());
		auto h = tau_api::pnf(fm.value());
		auto t = tau_api::pnf(fm.value()->get());
		CHECK(h.has_value() == t.has_value());
		CHECK(h.has_error() == t.has_error());
		CHECK(report_has_code(h.report(), code::unsupported_operation)
			== report_has_code(t.report(), code::unsupported_operation));
		CHECK(is_invalid_argument(tau_api::pnf(htref{})));
	}

	TEST_CASE_FIXTURE(api_fixture, "mnf") {
		auto fm = tau_api::geth_formula("x = 0 || (x = 0 && y != 0)");
		REQUIRE(fm.has_value());
		auto h = tau_api::mnf(fm.value());
		REQUIRE(h.has_value());
		CHECK(matches_wff_mod_and_or(h.value()->get(), "x = 0"));
		CHECK(holds(h, tau_api::mnf(fm.value()->get())));
		auto term = tau_api::geth_term("x | x & y");
		REQUIRE(term.has_value());
		auto ht = tau_api::mnf(term.value());
		REQUIRE(ht.has_value());
		CHECK(holds(ht, tau_api::mnf(term.value()->get())));
		CHECK(is_invalid_argument(tau_api::mnf(htref{})));
	}
}

TEST_SUITE("Tau API - htref - procedures") {

	TEST_CASE_FIXTURE(api_fixture, "simplify") {
		auto t = tau_api::geth_term("x & 1", false);
		REQUIRE(t.has_value());
		auto h = tau_api::simplify(t.value());
		REQUIRE(h.has_value());
		CHECK(matches_bf_mod_and_or(h.value()->get(), "x"));
		CHECK(holds(h, tau_api::simplify(t.value()->get())));
		auto h2 = tau_api::simplify(h.value());
		REQUIRE(h2.has_value());
		CHECK(h2.value()->get() == h.value()->get());
		CHECK(is_invalid_argument(tau_api::simplify(htref{})));
	}

	TEST_CASE_FIXTURE(api_fixture, "syntactic_term_simplification") {
		auto t = tau_api::geth_term("x & x", false);
		REQUIRE(t.has_value());
		auto h = tau_api::syntactic_term_simplification(t.value());
		REQUIRE(h.has_value());
		CHECK(matches_bf_mod_and_or(h.value()->get(), "x"));
		CHECK(holds(h, tau_api::syntactic_term_simplification(
			t.value()->get())));
		CHECK(is_invalid_argument(
			tau_api::syntactic_term_simplification(htref{})));
	}

	TEST_CASE_FIXTURE(api_fixture, "syntactic_formula_simplification") {
		auto fm = tau_api::geth_formula("x = 0 && x = 0", false);
		REQUIRE(fm.has_value());
		auto h = tau_api::syntactic_formula_simplification(fm.value());
		REQUIRE(h.has_value());
		CHECK(matches_wff_mod_and_or(h.value()->get(), "x = 0"));
		CHECK(holds(h, tau_api::syntactic_formula_simplification(
			fm.value()->get())));
		CHECK(is_invalid_argument(
			tau_api::syntactic_formula_simplification(htref{})));
	}

	TEST_CASE_FIXTURE(api_fixture, "normalize_term") {
		auto t = tau_api::geth_term("x & x'");
		REQUIRE(t.has_value());
		auto h = tau_api::normalize_term(t.value());
		REQUIRE(h.has_value());
		CHECK(matches_bf_mod_and_or(h.value()->get(), "0"));
		CHECK(holds(h, tau_api::normalize_term(t.value()->get())));
		auto fm = tau_api::geth_formula("x & y = 0");
		REQUIRE(fm.has_value());
		CHECK(is_invalid_argument(tau_api::normalize_term(fm.value())));
		CHECK(is_invalid_argument(tau_api::normalize_term(htref{})));
	}

	TEST_CASE_FIXTURE(api_fixture, "normalize_formula") {
		auto fm = tau_api::geth_formula("x & x' = 0");
		REQUIRE(fm.has_value());
		auto h = tau_api::normalize_formula(fm.value());
		REQUIRE(h.has_value());
		CHECK(matches_wff_mod_and_or(h.value()->get(), "T"));
		CHECK(holds(h, tau_api::normalize_formula(fm.value()->get())));
		auto t = tau_api::geth_term("x & y");
		REQUIRE(t.has_value());
		CHECK(is_invalid_argument(tau_api::normalize_formula(t.value())));
		CHECK(is_invalid_argument(tau_api::normalize_formula(htref{})));
	}

	TEST_CASE_FIXTURE(api_fixture, "anti_prenex") {
		auto fm = tau_api::geth_formula("(ex x r(x, y)) && z = 0");
		REQUIRE(fm.has_value());
		auto h = tau_api::anti_prenex(fm.value());
		REQUIRE(h.has_value());
		CHECK(tau_api::contains(h.value(), tau::wff_ex));
		CHECK(holds(h, tau_api::anti_prenex(fm.value()->get())));
		CHECK(is_invalid_argument(tau_api::anti_prenex(htref{})));
	}

	TEST_CASE_FIXTURE(api_fixture, "eliminate_quantifiers") {
		auto fm = tau_api::geth_formula("ex x (x = 0 && y = x)");
		REQUIRE(fm.has_value());
		auto h = tau_api::eliminate_quantifiers(fm.value());
		REQUIRE(h.has_value());
		CHECK(!tau_api::contains(h.value(), tau::wff_ex));
		CHECK(holds(h, tau_api::eliminate_quantifiers(fm.value()->get())));
		auto t = tau_api::geth_term("x & y'");
		REQUIRE(t.has_value());
		CHECK(is_invalid_argument(tau_api::eliminate_quantifiers(t.value())));
		CHECK(is_invalid_argument(tau_api::eliminate_quantifiers(htref{})));
	}
}

TEST_SUITE("Tau API - htref - decisions") {

	TEST_CASE_FIXTURE(api_fixture, "sat, unsat, valid and valid_spec") {
		for (const char* src : { "T", "F", "x = 0", "x = 0 && x != 0",
				"x = 0 || x != 0" })
		{
			CAPTURE(src);
			auto fm = tau_api::geth_formula(src);
			REQUIRE(fm.has_value());
			tref t = fm.value()->get();
			auto hs = tau_api::sat(fm.value());
			auto ts = tau_api::sat(t);
			REQUIRE(hs.has_value());
			REQUIRE(ts.has_value());
			CHECK(hs.value() == ts.value());
			auto hu = tau_api::unsat(fm.value());
			auto tu = tau_api::unsat(t);
			REQUIRE(hu.has_value());
			REQUIRE(tu.has_value());
			CHECK(hu.value() == tu.value());
			CHECK(hu.value() == !hs.value());
			auto hv = tau_api::valid(fm.value());
			auto tv = tau_api::valid(t);
			REQUIRE(hv.has_value());
			REQUIRE(tv.has_value());
			CHECK(hv.value() == tv.value());
			auto hvs = tau_api::valid_spec(fm.value());
			auto tvs = tau_api::valid_spec(t);
			REQUIRE(hvs.has_value());
			REQUIRE(tvs.has_value());
			CHECK(hvs.value() == tvs.value());
		}
		CHECK(is_invalid_argument(tau_api::sat(htref{})));
		CHECK(is_invalid_argument(tau_api::unsat(htref{})));
		CHECK(is_invalid_argument(tau_api::valid(htref{})));
		CHECK(is_invalid_argument(tau_api::valid_spec(htref{})));
	}

	TEST_CASE_FIXTURE(api_fixture, "realizable and unrealizable") {
		for (const char* src : { "T", "F", "x = 0" }) {
			CAPTURE(src);
			auto fm = tau_api::geth_formula(src);
			REQUIRE(fm.has_value());
			tref t = fm.value()->get();
			auto hr = tau_api::realizable(fm.value());
			auto tr = tau_api::realizable(t);
			REQUIRE(hr.has_value());
			REQUIRE(tr.has_value());
			CHECK(hr.value() == tr.value());
			auto hu = tau_api::unrealizable(fm.value());
			auto tu = tau_api::unrealizable(t);
			REQUIRE(hu.has_value());
			REQUIRE(tu.has_value());
			CHECK(hu.value() == tu.value());
			CHECK(hu.value() == !hr.value());
		}
		CHECK(is_invalid_argument(tau_api::realizable(htref{})));
		CHECK(is_invalid_argument(tau_api::unrealizable(htref{})));
	}

	TEST_CASE_FIXTURE(api_fixture, "a term handle is rejected") {
		auto t = tau_api::geth_term("x & y'");
		REQUIRE(t.has_value());
		CHECK(is_invalid_argument(tau_api::sat(t.value())));
		CHECK(is_invalid_argument(tau_api::unsat(t.value())));
		CHECK(is_invalid_argument(tau_api::valid(t.value())));
		CHECK(is_invalid_argument(tau_api::valid_spec(t.value())));
		CHECK(is_invalid_argument(tau_api::realizable(t.value())));
		CHECK(is_invalid_argument(tau_api::unrealizable(t.value())));
	}
}

TEST_SUITE("Tau API - htref - solving") {

	TEST_CASE_FIXTURE(api_fixture, "solve") {
		auto eq = tau_api::geth_formula("x | y = 0");
		REQUIRE(eq.has_value());
		for (auto mode : { solver_mode::general, solver_mode::minimum,
				solver_mode::maximum })
		{
			auto hs = tau_api::solve(eq.value(), mode);
			auto ts = tau_api::solve(eq.value()->get(), mode);
			REQUIRE(hs.has_value());
			REQUIRE(ts.has_value());
			CHECK(!hs.value().empty());
			CHECK(same_solution(hs.value(), ts.value()));
		}
		CHECK(is_invalid_argument(tau_api::solve(htref{})));
		auto temporal = tau_api::geth_formula("always o1[t] = 0");
		REQUIRE(temporal.has_value());
		CHECK(is_invalid_argument(tau_api::solve(temporal.value())));
	}

	TEST_CASE_FIXTURE(api_fixture, "lgrs") {
		auto eq = tau_api::geth_formula("x | y = 0");
		REQUIRE(eq.has_value());
		auto hl = tau_api::lgrs(eq.value());
		auto tl = tau_api::lgrs(eq.value()->get());
		REQUIRE(hl.has_value());
		REQUIRE(tl.has_value());
		CHECK(!hl.value().empty());
		CHECK(same_solution(hl.value(), tl.value()));
		CHECK(is_invalid_argument(tau_api::lgrs(htref{})));
		auto term = tau_api::geth_term("x | y");
		REQUIRE(term.has_value());
		CHECK(is_invalid_argument(tau_api::lgrs(term.value())));
	}
}

// Last in the file: the sweep frees every tree no handle holds, so no case
// after it may rely on a tree parsed before it.
TEST_SUITE("Tau API - htref - across a sweep") {

	TEST_CASE_FIXTURE(api_fixture, "a handle keeps its tree and answers after a sweep") {
		auto h_r = tau_api::geth_formula(
			"(p = 0 && q != 0) || (r = 0 && s != 0 && u = 0)");
		REQUIRE(h_r.has_value());
		htref h = h_r.value();
		const std::string before = tau_api::to_str(h);
		const size_t held = tau_api::tref_count();
		// a tree no handle holds
		REQUIRE(tau_api::get_formula(
			"(p != 0 && q = 0) || (r != 0 && s = 0 && u != 0)").has_value());
		const size_t grown = tau_api::tref_count();
		REQUIRE(grown > held);
		do_gc();
		CHECK(tau_api::tref_count() < grown);
		CHECK(tau_api::to_str(h) == before);
		CHECK(tau_api::is_formula(h));
		CHECK(tau_api::contains(h, tau::wff_or));
		auto s = tau_api::sat(h);
		REQUIRE(s.has_value());
		CHECK(s.value());
		// the same formula parsed again, after the sweep
		auto again = tau_api::get_formula(before);
		REQUIRE(again.has_value());
		CHECK(holds(h, again.value()));
		CHECK(holds(tau_api::dnf(h), tau_api::dnf(again.value())));
		CHECK(holds(tau_api::nnf(h), tau_api::nnf(again.value())));
	}
}
