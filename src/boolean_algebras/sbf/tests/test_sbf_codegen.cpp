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

bool has(const std::string& s, const std::string& pat) {
	return s.find(pat) != std::string::npos;
}

std::optional<ltl_aba_solution<node_t>> synth(const std::string& spec) {
	tref fm = api<node_t>::get_formula(spec);
	if (!fm) return std::nullopt;
	return solve_ltl_aba<node_t>(fm);
}

} // namespace

TEST_SUITE("sbf_codegen") {

	TEST_CASE("codegen_constant_expr: the constants 1 and 0 spell htrue/hfalse") {
		auto et = sbf_codegen_constant_expr<node_t>(
			sbf_constant_tref(parse_sbf_value("1")));
		REQUIRE(et.has_value());
		CHECK(has(*et, "bdd_handle<::idni::tau_lang::Bool>::htrue"));

		auto ef = sbf_codegen_constant_expr<node_t>(
			sbf_constant_tref(parse_sbf_value("0")));
		REQUIRE(ef.has_value());
		CHECK(has(*ef, "bdd_handle<::idni::tau_lang::Bool>::hfalse"));
	}

	TEST_CASE("codegen_constant_expr: a single variable interns its name, not a numeric id") {
		auto ex = sbf_codegen_constant_expr<node_t>(
			sbf_constant_tref(parse_sbf_value("X")));
		REQUIRE(ex.has_value());
		CHECK(has(*ex, "var_dict(std::string(\"X\"))"));
		CHECK(has(*ex, "bit(true,"));
		CHECK_FALSE(has(*ex, "bit(false,"));
	}

	TEST_CASE("codegen_constant_expr: negation flips the literal's sign, not its name") {
		auto e = sbf_codegen_constant_expr<node_t>(
			sbf_constant_tref(parse_sbf_value("X'")));
		REQUIRE(e.has_value());
		CHECK(has(*e, "var_dict(std::string(\"X\"))"));
		CHECK(has(*e, "bit(false,"));
	}

	TEST_CASE("codegen_constant_expr: a conjunction is one clause over both literals") {
		auto e = sbf_codegen_constant_expr<node_t>(
			sbf_constant_tref(parse_sbf_value("X & Y")));
		REQUIRE(e.has_value());
		CHECK(has(*e, "var_dict(std::string(\"X\"))"));
		CHECK(has(*e, "var_dict(std::string(\"Y\"))"));
		CHECK(has(*e, " & "));
		CHECK_FALSE(has(*e, " | "));
	}

	TEST_CASE("codegen_constant_expr: an xor-shaped constant emits two clauses") {
		auto e = sbf_codegen_constant_expr<node_t>(
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
		auto e1 = sbf_codegen_constant_expr<node_t>(
			sbf_constant_tref(parse_sbf_value(src)));
		auto e2 = sbf_codegen_constant_expr<node_t>(
			sbf_constant_tref(parse_sbf_value(src)));
		REQUIRE(e1.has_value());
		REQUIRE(e2.has_value());
		CHECK(*e1 == *e2);
	}

	// The bug this closes: an sbf atom's ground constant reached
	// build_atom_term_expr and was refused ("atom constant's owning BA type
	// ':sbf' declined codegen_constant_expr"), even though the atom itself
	// correctly classifies as witness_template (sbf owns no codegen_witness).
	// Reproduces tests/codegen_specs/sbf_temporal.tau's atom shape directly
	// against the atoms table, without a full artifact build.
	TEST_CASE("build_program_desc: an sbf ground-equality atom's constant "
	          "emits rather than throwing") {
		auto sol = synth("G(o1[t]:sbf = {X & Y}:sbf)");
		REQUIRE(sol.has_value());
		auto d = build_program_desc<node_t>(*sol);
		REQUIRE(d.has_value());
		// When sbf is the pack's bool carrier, the free vars in this atom
		// carry the carrier type, so atom_is_data_typed sees only flags and
		// the atom never reaches the atoms table.
		if (ba_types<node_t>::id(pack_bool_carrier_type<node_t>())
		    == sbf_type_id<node_t>()) {
			CHECK(d->atoms.empty());
			REQUIRE(d->outputs.size() == 1);
			CHECK(d->outputs[0].kind == field_kind::flag);
			REQUIRE(d->flag_output_vars.size() == 1);
			CHECK(d->flag_output_vars[0] == "o1");
		} else {
			REQUIRE(d->atoms.size() == 1);
			const std::string& e = d->atoms[0].ground_expr;
			CHECK(has(e, "build_bf_eq<"));
			CHECK(has(e, "var_dict(std::string(\"X\"))"));
			CHECK(has(e, "var_dict(std::string(\"Y\"))"));
		}
	}
}


TEST_SUITE("Cleanup") {
	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
