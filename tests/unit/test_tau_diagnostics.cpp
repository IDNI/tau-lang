// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "tau_diagnostics.h"

#include <string>

using namespace idni::tau_lang;

namespace {

// The index of the first node of @p rep whose message or name is @p text,
// or the node count when there is none.
size_t node_named(const report& rep, std::string_view text) {
	const auto& ns = rep.nodes();
	for (size_t i = 0; i < ns.size(); ++i)
		if (rep.str(ns[i].key) == text) return i;
	return ns.size();
}

}

TEST_SUITE("tau_diagnostics") {

	TEST_CASE("a rejected candidate is named under its scope and demoted") {
		report r;
		report cand;
		cand.error(code::unsat, "the candidate failed");
		append_rejected_candidate(r, "the candidate lost",
			"line one\nline two", std::move(cand));
		CHECK( !r.has_error() );
		const auto& ns = r.nodes();
		size_t scope = node_named(r, "rejected candidate");
		size_t why = node_named(r, "the candidate lost");
		size_t err = node_named(r, "the candidate failed");
		REQUIRE( scope < ns.size() );
		REQUIRE( why < ns.size() );
		REQUIRE( err < ns.size() );
		CHECK( ns[why].parent == static_cast<int32_t>(scope) );
		CHECK( ns[err].parent == static_cast<int32_t>(scope) );
		CHECK( is_warning(ns[err].tag) );
		CHECK( node_attr_text(r, ns[why], tau_lang::label::value)
			== std::optional<std::string>("line one line two") );
	}

	TEST_CASE("a long candidate is truncated") {
		report r;
		append_rejected_candidate(r, "the candidate lost",
			std::string(200, 'x'), report{});
		size_t why = node_named(r, "the candidate lost");
		REQUIRE( why < r.nodes().size() );
		CHECK( node_attr_text(r, r.nodes()[why], tau_lang::label::value)
			== std::optional<std::string>(std::string(120, 'x') + "...") );
	}

	TEST_CASE("without demotion the errors of the candidate stay") {
		report r;
		report cand;
		cand.error(code::unsat, "the candidate failed");
		append_rejected_candidate(r, "the candidate lost", "c",
			std::move(cand), false);
		CHECK( r.has_error() );
		size_t err = node_named(r, "the candidate failed");
		REQUIRE( err < r.nodes().size() );
		CHECK( is_error(r.nodes()[err].tag) );
	}
}
