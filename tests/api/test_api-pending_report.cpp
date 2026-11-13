// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

using tau_api = api<node_t>;

namespace {

// The messages of the nodes of @p rep tagged @p c.
std::vector<std::string> messages_tagged(const report& rep, code c) {
	std::vector<std::string> msgs;
	for (const auto& n : rep.nodes())
		if (n.tag == c) msgs.emplace_back(rep.str(n.key));
	return msgs;
}

bool holds(const std::vector<std::string>& msgs, std::string_view text) {
	for (const auto& m : msgs)
		if (m.find(text) != std::string_view::npos) return true;
	return false;
}

report error_report(std::string_view msg) {
	report rep;
	rep.error(code::runtime_error, msg);
	return rep;
}

} // namespace

// The pending report (tau_diagnostics.h) holds the reports of work whose
// signature carries no channel; every api call runs inside with_budget,
// which drains it into the call's result.
TEST_SUITE("Tau API - pending report") {

	TEST_CASE("a pending error reaches the next api result as a warning") {
		take_pending_report();
		note_pending_report(error_report("a printer met an invalid id"));
		auto r = tau_api::normalize_formula("x = 0");
		REQUIRE( r.has_value() );
		CHECK( !r.has_error() );
		CHECK( messages_tagged(r.report(), code::runtime_error).empty() );
		CHECK( holds(messages_tagged(r.report(), code::warning),
			"a printer met an invalid id") );
		CHECK( take_pending_report().nodes().empty() );
	}

	TEST_CASE("a pending error stays an error on a failed api result") {
		take_pending_report();
		note_pending_report(error_report("an extractor met an invalid id"));
		auto r = tau_api::normalize_formula(tref(nullptr));
		REQUIRE( !r.has_value() );
		CHECK( holds(messages_tagged(r.report(), code::runtime_error),
			"an extractor met an invalid id") );
		CHECK( take_pending_report().nodes().empty() );
	}

	TEST_CASE("the drain is the outermost boundary only") {
		take_pending_report();
		auto r = with_budget<node_t>([] {
			note_pending_report(error_report("noted inside"));
			auto inner = with_budget<node_t>([] {
				result<bool> r;
				return r.with_value(true);
			});
			// the nested unit leaves the report for the outermost one
			CHECK( inner.report().nodes().empty() );
			return inner;
		});
		REQUIRE( r.has_value() );
		CHECK( holds(messages_tagged(r.report(), code::warning),
			"noted inside") );
	}

	TEST_CASE("a result with no error takes nothing when nothing is pending") {
		take_pending_report();
		auto r = tau_api::normalize_formula("x = 0");
		REQUIRE( r.has_value() );
		CHECK( messages_tagged(r.report(), code::warning).empty() );
	}
}
