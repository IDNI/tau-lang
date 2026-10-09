// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "bounded_call.h"

#include <cstdlib>
#include <sstream>
#include <thread>

using namespace idni::tau_lang;

TEST_SUITE("bounded_call") {

	TEST_CASE("the child's answer reaches the parent") {
		auto out = run_bounded([] { return uint8_t{42}; }, 10'000);
		CHECK( out.status == bounded_outcome::done );
		CHECK( out.value == 42 );
		CHECK( !out.rep.has_error() );
	}

	TEST_CASE("the child works on a copy of the parent") {
		int state = 7;
		auto out = run_bounded([&] { state = 9; return (uint8_t)state; },
			10'000);
		CHECK( out.value == 9 );
		CHECK( state == (bounded_calls_available() ? 7 : 9) );
	}

	TEST_CASE("work past the bound is killed at the bound") {
		if (!bounded_calls_available()) return;
		auto out = run_bounded([] {
			for (;;) std::this_thread::sleep_for(
				std::chrono::seconds(1));
			return uint8_t{0};
		}, 300);
		CHECK( out.status == bounded_outcome::timed_out );
		CHECK( out.seconds >= 0.3 );
		CHECK( out.seconds < 5 );
		CHECK( out.rep.has_error() );
		CHECK( report_attr_value(out.rep, tau_lang::label::timeout)
			== 300 );
	}

	TEST_CASE("a child that dies gives no answer") {
		if (!bounded_calls_available()) return;
		auto out = run_bounded([] { std::_Exit(3); return uint8_t{1}; },
			10'000);
		CHECK( out.status == bounded_outcome::failed );
		CHECK( out.rep.has_error() );
		CHECK( report_attr_value(out.rep, tau_lang::label::exit_code)
			== 3 );
	}

	TEST_CASE("the reports of every budget that ran out are kept") {
		take_time_budget_exhausted();
		take_time_budget_report();
		diag::report first, second;
		first.error(diag::code::runtime_error, "first child");
		second.error(diag::code::runtime_error, "second child");
		note_time_budget_exhausted("first", std::move(first));
		note_time_budget_exhausted("second", std::move(second));
		CHECK( take_time_budget_exhausted() == "first" );
		auto rep = take_time_budget_report();
		CHECK( rep.has_error() );
		std::ostringstream os;
		rep.print(os);
		CHECK( os.str().find("first child") != std::string::npos );
		CHECK( os.str().find("second child") != std::string::npos );
		CHECK( !take_time_budget_report().has_error() );
	}

#ifdef TAU_HAS_BOUNDED_CALL
	TEST_CASE("the child does not outlive its parent") {
		int p[2];
		REQUIRE( ::pipe(p) == 0 );
		const pid_t helper = ::fork();
		REQUIRE( helper >= 0 );
		if (helper == 0) {
			run_bounded([&] {
				const pid_t me = ::getpid();
				(void)!::write(p[1], &me, sizeof me);
				for (;;) std::this_thread::sleep_for(
					std::chrono::seconds(1));
				return uint8_t{0};
			}, 600'000);
			::_exit(0);
		}
		pid_t child = 0;
		REQUIRE( ::read(p[0], &child, sizeof child) == sizeof child );
		::kill(helper, SIGKILL);
		::waitpid(helper, nullptr, 0);
		bool gone = false;
		for (int i = 0; i < 50 && !gone; ++i) {
			gone = ::kill(child, 0) != 0;
			if (!gone) std::this_thread::sleep_for(
				std::chrono::milliseconds(100));
		}
		CHECK( gone );
		if (!gone) ::kill(child, SIGKILL);
		::close(p[0]); ::close(p[1]);
	}
#endif

	TEST_CASE("a handled scope keeps its budgets from the boundary") {
		take_time_budget_exhausted();
		note_time_budget_exhausted("outer");
		{
			time_budget_handled h;
			CHECK( !h.ran_out() );
			note_time_budget_exhausted("inner");
			CHECK( h.ran_out() );
		}
		CHECK( take_time_budget_exhausted() == "outer" );
		{
			time_budget_handled h;
			note_time_budget_exhausted("inner");
		}
		CHECK( take_time_budget_exhausted().empty() );
	}

	TEST_CASE("the first budget noted is the one reported") {
		take_time_budget_exhausted();
		note_time_budget_exhausted("first");
		note_time_budget_exhausted("second");
		CHECK( take_time_budget_exhausted() == "first" );
	}
}
