// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// codegen_constant_expr contract: sbf rebuilds a ground BDD constant from its
// own DNF as a self-contained C++ expression, never round-tripping it through
// sbf's own parser at artifact startup. Variable identity is carried by name
// (var_dict(std::string("X"))), never by the numeric BDD id this process
// happened to assign -- the artifact's own BDD tables start empty.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "cpp_codegen.h"
#include "ltl_aba.h"

#include <optional>
#include <sstream>
#include <string>

using namespace idni::tau_lang;

namespace {

sbf_ba parse_sbf_value(const std::string& src) {
	auto parsed = ba_descriptor<sbf_ba, node_t>::parse(src, nullptr);
	assert(parsed.has_value());
	return std::get<sbf_ba>(parsed->first);
}

// The bare (non-bf-wrapped) ba_constant node codegen_constant_expr expects,
// exactly what build_atom_term_expr hands it after tau::trim.
tref sbf_constant_tref(const sbf_ba& v) {
	return tree<node_t>::get_ba_constant(
		typename tree<node_t>::constant(v), sbf_type<node_t>());
}

// The spelling of @p cst; building it must not fail.
std::optional<std::string> constant_expr(tref cst) {
	auto e = sbf_codegen_constant_expr<node_t>(cst);
	REQUIRE(e.has_value());
	return e.value();
}

bool has(const std::string& s, const std::string& pat) {
	return s.find(pat) != std::string::npos;
}

std::optional<ltl_aba_solution<node_t>> synth(const std::string& spec) {
	auto fm = api<node_t>::get_formula(spec);
	if (!fm.has_value()) return std::nullopt;
	auto r = solve_ltl_aba<node_t>(fm.value());
	REQUIRE(r.has_value()); // undecided is not "unrealizable"
	return r.value();
}

} // namespace

TEST_SUITE("sbf_codegen") {

	TEST_CASE("codegen_constant_expr: the constants 1 and 0 spell htrue/hfalse") {
		auto et = constant_expr(
			sbf_constant_tref(parse_sbf_value("1")));
		REQUIRE(et.has_value());
		CHECK(has(*et, "bdd_handle<::idni::tau_lang::Bool>::htrue"));

		auto ef = constant_expr(
			sbf_constant_tref(parse_sbf_value("0")));
		REQUIRE(ef.has_value());
		CHECK(has(*ef, "bdd_handle<::idni::tau_lang::Bool>::hfalse"));
	}

	TEST_CASE("codegen_constant_expr: a single variable interns its name, not a numeric id") {
		auto ex = constant_expr(
			sbf_constant_tref(parse_sbf_value("X")));
		REQUIRE(ex.has_value());
		CHECK(has(*ex, "var_dict(std::string(\"X\"))"));
		CHECK(has(*ex, "bit(true,"));
		CHECK_FALSE(has(*ex, "bit(false,"));
	}

	TEST_CASE("codegen_constant_expr: negation flips the literal's sign, not its name") {
		auto e = constant_expr(
			sbf_constant_tref(parse_sbf_value("X'")));
		REQUIRE(e.has_value());
		CHECK(has(*e, "var_dict(std::string(\"X\"))"));
		CHECK(has(*e, "bit(false,"));
	}

	TEST_CASE("codegen_constant_expr: a conjunction is one clause over both literals") {
		auto e = constant_expr(
			sbf_constant_tref(parse_sbf_value("X & Y")));
		REQUIRE(e.has_value());
		CHECK(has(*e, "var_dict(std::string(\"X\"))"));
		CHECK(has(*e, "var_dict(std::string(\"Y\"))"));
		CHECK(has(*e, " & "));
		CHECK_FALSE(has(*e, " | "));
	}

	TEST_CASE("codegen_constant_expr: an xor-shaped constant emits two clauses") {
		auto e = constant_expr(
			sbf_constant_tref(parse_sbf_value("X ^ Y")));
		REQUIRE(e.has_value());
		CHECK(has(*e, " | "));
		CHECK(has(*e, "bit(true,"));
		CHECK(has(*e, "bit(false,"));
	}

	// Clause/literal order is sorted by variable name rather than by the
	// numeric BDD id var_dict happened to assign during this parse -- the
	// same logical constant always emits the same text.
	TEST_CASE("codegen_constant_expr: the emitted expression is deterministic") {
		const char* src = "z' | x b (1'^(a b) | 0+c | a) ^ d | d^e&1";
		auto e1 = constant_expr(
			sbf_constant_tref(parse_sbf_value(src)));
		auto e2 = constant_expr(
			sbf_constant_tref(parse_sbf_value(src)));
		REQUIRE(e1.has_value());
		REQUIRE(e2.has_value());
		CHECK(*e1 == *e2);
	}

	// bit() takes any id, so a BDD can hold a variable var_dict has no name
	// for; the spelling goes by name and cannot be built.
	TEST_CASE("codegen_constant_expr: a variable without a name is an error") {
		const sbf_ba unnamed = bdd_handle<Bool>::bit(true, 4000);
		auto e = sbf_codegen_constant_expr<node_t>(
			sbf_constant_tref(unnamed));
		CHECK(e.has_error());
	}

	// The bug this closes: an sbf atom's ground constant reached
	// build_atom_term_expr and was refused ("atom constant's owning BA type
	// ':sbf' declined codegen_constant_expr"), even though the atom itself
	// correctly classifies as witness_template (sbf owns no codegen_witness).
	// Reproduces tests/codegen_specs/sbf_temporal.tau's atom shape directly
	// against the atoms table, without a full artifact build.
	TEST_CASE("build_program_desc: an sbf ground-equality atom's constant "
	          "emits rather than throwing"
		* doctest::skip(!ltlsynt_available())) {
		auto sol = synth("G(o1[t]:sbf = {X & Y}:sbf)");
		REQUIRE(sol.has_value());
		auto d = build_program_desc<node_t>(*sol);
		REQUIRE(d.has_value());
		// The same whether or not sbf is the pack's bool carrier: the prop's
		// truth does not decide o1's value (X & Y is neither 0 nor 1), so
		// the atom is solved for o1 as a template rather than written as a
		// flag, which would put the carrier's 1 into o1 instead of X & Y.
		REQUIRE(d->atoms.size() == 1);
		REQUIRE(d->outputs.size() == 1);
		CHECK(d->outputs[0].kind == field_kind::witness_template);
		const std::string& e = d->atoms[0].ground_expr;
		CHECK(has(e, "build_bf_eq<"));
		CHECK(has(e, "var_dict(std::string(\"X\"))"));
		CHECK(has(e, "var_dict(std::string(\"Y\"))"));
	}
}


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
