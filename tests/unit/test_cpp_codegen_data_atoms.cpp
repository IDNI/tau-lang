// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Tests for #60: data-atom outputs in the tau→C++ codegen. An output whose
// algebra gives no witness to bake in is solved at runtime: it routes as a
// witness template, which the standalone emitter refuses.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "cpp_codegen.h"
#include "ltl_aba.h"

#include <optional>
#include <sstream>
#include <string>

using namespace idni::tau_lang;

namespace {

// Parse and synthesize a formula, returning the solution.
// Returns nullopt if UNREALIZABLE or parse failure.
static std::optional<ltl_aba_solution<node_t>> synth(const std::string& spec) {
	auto fm = api<node_t>::get_formula(spec);
	if (!fm.has_value()) return std::nullopt;
	auto r = solve_ltl_aba<node_t>(fm.value());
	REQUIRE(r.has_value()); // undecided is not "unrealizable"
	return r.value();
}

} // namespace

TEST_SUITE("cpp_codegen_data_atoms") {

	TEST_CASE("G(o1:bv = i1:bv[8]): output routes as a witness template; the "
	          "standalone emitter refuses") {
		auto sol = synth("G(o1[t]:bv = i1[t]:bv[8])");
		REQUIRE_MESSAGE(sol, "the spec must parse and be REALIZABLE: an UNREALIZABLE here is the regression this suite exists to catch");
		auto d = build_program_desc<node_t>(*sol);
		REQUIRE(d.has_value());
		REQUIRE(d->atoms.size() == 1);
		CHECK(d->needs_tau_link);
		bool tmpl_field = false;
		for (auto& f : d->outputs)
			if (f.kind == field_kind::witness_template && f.prop == "o1")
				tmpl_field = true;
		CHECK(tmpl_field);
		bool tmpl_edge = false;
		for (auto& es : d->edges) for (auto& e : es)
			if (!e.witness_template_props.empty()) tmpl_edge = true;
		CHECK(tmpl_edge);
		std::ostringstream os;
		auto er = emit_program(*d, os);
		CHECK_FALSE(er.has_value());
		CHECK(er.has_error());
		CHECK(report_has_code(er.report(), code::unsupported_operation));
		bool found_summary = false, found_name_attr = false;
		for (auto& n : er.report().nodes()) {
			if (n.tag != code::unsupported_operation) continue;
			if (er.report().str(n.key) ==
				"the output needs runtime witness solving, which the "
				"standalone emitted step() does not support; drive the "
				"program through the interpreter's table step provider")
				found_summary = true;
			if (auto name = node_attr_text(er.report(), n, tau_lang::label::name)) {
				found_name_attr = true;
				CHECK(*name == "o1");
			}
		}
		CHECK(found_summary);
		CHECK(found_name_attr);
	}

	TEST_CASE("G(o1:qlt > 1/2): a qlt output, a set, routes as a witness "
	          "template too") {
		auto sol = synth("G(o1[t]:qlt > {1/2}:qlt)");
		REQUIRE_MESSAGE(sol, "the spec must parse and be REALIZABLE: an UNREALIZABLE here is the regression this suite exists to catch");
		auto d = build_program_desc<node_t>(*sol);
		REQUIRE(d.has_value());
		CHECK(d->needs_tau_link);
		bool tmpl_field = false;
		for (auto& f : d->outputs)
			if (f.kind == field_kind::witness_template && f.prop == "o1")
				tmpl_field = true;
		CHECK(tmpl_field);
		bool tmpl_edge = false;
		for (auto& es : d->edges) for (auto& e : es)
			if (!e.witness_template_props.empty()) tmpl_edge = true;
		CHECK(tmpl_edge);
		std::ostringstream os;
		auto er = emit_program(*d, os);
		CHECK_FALSE(er.has_value());
		CHECK(er.has_error());
		CHECK(report_has_code(er.report(), code::unsupported_operation));
		bool found_summary = false, found_name_attr = false;
		for (auto& n : er.report().nodes()) {
			if (n.tag != code::unsupported_operation) continue;
			if (er.report().str(n.key) ==
				"the output needs runtime witness solving, which the "
				"standalone emitted step() does not support; drive the "
				"program through the interpreter's table step provider")
				found_summary = true;
			if (auto name = node_attr_text(er.report(), n, tau_lang::label::name)) {
				found_name_attr = true;
				CHECK(*name == "o1");
			}
		}
		CHECK(found_summary);
		CHECK(found_name_attr);
	}

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
