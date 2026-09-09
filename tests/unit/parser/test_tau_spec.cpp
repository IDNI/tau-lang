// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "testing"

// #define TAU_TEST_MULTILINE_PARSING_BRUTE_FORCE 1

std::vector<std::string> split(const std::string& str, char delimiter) {
	std::vector<std::string> result;
	std::istringstream stream(str);
	std::string line;
	while (std::getline(stream, line, delimiter)) result.push_back(line);
	return result;
}

struct specification_fixture {  // 0      1       2          3              4
	enum class value_type { space, newline, sep_dot, sep_newline, sep_dot_newline };
	std::vector<std::vector<std::string>> fixture;
	std::vector<value_type> indices;
	specification_fixture() :
		fixture(init_fixture()),
		indices(init_indices(fixture))
		{}

	std::string get() {
		std::stringstream ss;
		size_t index = 0;
		auto out = [&]() {
			switch (indices[index++]) {
				case value_type::space:
					ss << " ";
					break;
				case value_type::newline:
					ss << "\n";
					break;
				case value_type::sep_dot:
					ss << ".";
					break;
				case value_type::sep_newline:
					ss << "\n";
					break;
				case value_type::sep_dot_newline:
					ss << ".\n";
					break;
			}
		};
		for (size_t i = 0; i < fixture.size(); ++i) {
			for (size_t j = 0; j < fixture[i].size(); ++j) {
				out();
				ss << fixture[i][j];
			}
			out();
			out();
		}
		std::stringstream ss2;
		for (const auto& v : indices) ss2 << (int)v;
		TAU_LOG_INFO << "indices: " << ss2.str();

		return ss.str();
	}

	bool next() {
		for (size_t i = indices.size() - 1; ; --i) {
			switch (indices[i]) {
				case value_type::space:
					indices[i] = value_type::newline;
					return true;
				case value_type::newline:
					if (i == 0) return false;
					indices[i] = value_type::space;
					continue;

				case value_type::sep_dot:
					indices[i] = value_type::sep_newline;
					return true;
				case value_type::sep_newline:
					indices[i] = value_type::sep_dot_newline;
					return true;
				case value_type::sep_dot_newline:
					if (i == 0) return false;
					indices[i] = value_type::sep_dot;
					continue;
			}
			if (i == 0) break;
		}
		return false;
	}

	std::vector<std::vector<std::string>> init_fixture() {
		std::vector<std::string> fixture_spec = {
			// "o : tau := out console",
			// "f[t](x) := i[t] & x = 0",
			"i := in console",
			"g(x) := x = 0 || x = 1",
			"o[t] = g(i[t]) && g(0) || g(1)"
		};
		std::vector<std::vector<std::string>> result(fixture_spec.size());
		for (size_t i = 0; i < fixture_spec.size(); ++i) {
			result[i] = split(fixture_spec[i], ' ');
			DBG(TAU_LOG_TRACE << "fixture line parts[" << i << "]: " << result[i].size();)
		}
		return result;
	}

	std::vector<value_type> init_indices(
		const std::vector<std::vector<std::string>>& fixture)
	{
		std::vector<value_type> result;
		for (size_t i = 0; i < fixture.size(); ++i) {
			for (size_t j = 0; j < fixture[i].size(); ++j) {
				result.push_back(value_type::space);
			}
			result.push_back(value_type::space);
			result.push_back(value_type::sep_dot);
		}
		return result;
	}

	std::vector<value_type> init_indices_end(
		const std::vector<std::vector<std::string>>& fixture)
	{
		std::vector<value_type> result;
		for (size_t i = 0; i < fixture.size(); ++i) {
			for (size_t j = 0; j < fixture[i].size(); ++j) {
				result.push_back(value_type::newline);
			}
			result.push_back(value_type::newline);
			result.push_back(value_type::sep_dot_newline);
		}
		return result;
	}

};

template <NodeType node>
tref get_spec_multiline(const strings& src) {
	tau_spec<node> spec;
	for (const auto& line : src) if (!spec.parse(line)) break;
	if (spec.errors().size()) {
		for (const auto& error : spec.errors())
			TAU_LOG_ERROR << error;
		return nullptr;
	}
	return spec.get();
}

bool has_main(tref fm) {
	return (tt(fm) | tau::main).has_value();
}

TEST_SUITE("specification multiline parsing") {

	TEST_CASE("simple case: o[t] = i[t].") {
		tref fm = get_spec_multiline<node_t>({
			"o[t] = i[t]."
		});
		CHECK( (tt(fm) | tau::main).has_value() );
	}

	TEST_CASE("simple case: o[t] = i[t]") {
		tref fm = get_spec_multiline<node_t>({
			"o[t] = i[t]"
		});
		CHECK( (tt(fm) | tau::main).has_value() );
	}

	TEST_CASE("simple case: no main") {
		tref fm = get_spec_multiline<node_t>({
			" "
		});
		CHECK( fm == nullptr );
	}

	TEST_CASE("simple case: multiple mains") {
		tref fm = get_spec_multiline<node_t>({
			"o1[t] = i1[t].",
			"o2[t] = i2[t]."
		});
		CHECK( fm == nullptr );
	}

	TEST_CASE("simple case: o[t]\\n=\\ni[t].") {
		tref fm = get_spec_multiline<node_t>({
			"o[t]\n=\ni[t]\n"
		});
		CHECK( (tt(fm) | tau::main).has_value() );
	}

	TEST_CASE("multiline inline no dots") {
		tref fm = get_spec_multiline<node_t>({
			"i\n:\ntau\n:=\nin\nconsole\n"
			"o\n:\ntau\n:=\nout\nconsole\n"
			"o[t]\n=\ni[t]\n"
		});
		CHECK( (tt(fm) | tau::main).has_value() );
	}

	TEST_CASE("multiple commands inline") {
		auto& defs = definitions<node_t>::instance();
		defs.clear();
		tref fm = get_spec_multiline<node_t>({
			"i : tau := in console. o : tau := out console. o[t] = i[t]"
		});
		CHECK( (tt(fm) | tau::main).has_value() );
                auto& ctx = *defs.get_io_context();
                CHECK( ctx.inputs.size() == 1 );
                CHECK( ctx.outputs.size() == 1 );
	}

TEST_SUITE("TauSpecAdd") {
	TEST_CASE("add(nullptr) returns false") {
		// The null-guard at the top of add() must fire immediately
		tau_spec<node_t> spec;
		CHECK( !spec.add(nullptr) );
	}

	TEST_CASE("add pre-built wff formula returns true") {
		// exercise the non-parsing path: attach a formula tree directly
		// rather than going through parse()
		tref wff_fm = get_nso_rr("xy = 0.").value().main->get();
		tau_spec<node_t> spec;
		CHECK( spec.add(wff_fm) );
	}

	TEST_CASE("add spec tref extracts main and sets main_") {
		// tau_spec::get() builds a proper tau::spec node (not the raw start
		// node that tau::get(string) returns). The add() spec branch extracts
		// the inner wff and calls set_main(), exercising the spec-type case.
		tau_spec<node_t> helper;
		helper.parse("xy = 0.");
		tref spec_fm = helper.get();
		REQUIRE( spec_fm != nullptr );
		using tt = tau::traverser;
		auto nt = tt(spec_fm) || tt::nt;
		REQUIRE( nt == tau::spec );
		tau_spec<node_t> spec;
		CHECK( spec.add(spec_fm) );
	}
}

TEST_SUITE("TauSpecAddAndEof") {
	// GR-R3: add() used to swallow a second main and still return true.
	TEST_CASE("[GR-R3] add() refuses a second main loudly") {
		tau_spec<node_t> helper1, helper2;
		helper1.parse("xy = 0.");
		helper2.parse("xy = 1.");
		tref m1 = helper1.get();
		tref m2 = helper2.get();
		REQUIRE( m1 != nullptr );
		REQUIRE( m2 != nullptr );
		tau_spec<node_t> spec;
		CHECK( spec.add(m1) );
		CHECK( spec.errors().empty() );
		CHECK( !spec.add(m2) );
		REQUIRE( !spec.errors().empty() );
		CHECK( spec.errors().back().find("Multiple main") != std::string::npos );
	}

	// GR-R4: a premature get() while a continuation is pending used to
	// record the eof message as a permanent error, bricking every later
	// parse() of the same object.
	TEST_CASE("[GR-R4] a premature get() does not poison the incremental parse") {
		tau_spec<node_t> spec;
		CHECK( spec.parse("o[t] =") );   // incomplete
		CHECK( spec.is_eof() );
		CHECK( spec.get() == nullptr );  // too early: reported ...
		CHECK( !spec.errors().empty() );
		CHECK( spec.parse(" i[t].") );   // ... but the continuation still completes it
		CHECK( !spec.is_eof() );
		REQUIRE( spec.get() != nullptr );
		CHECK( spec.errors().empty() );
	}

	// GR-RT6 / TT2-2: operator<< had no call site, so its friend fix was
	// never compiled into an instantiation.
	TEST_CASE("[GR-RT6] operator<<(ostream&, const tau_spec&) instantiates") {
		tau_spec<node_t> spec;
		REQUIRE( spec.parse("xy = 0.") );
		REQUIRE( spec.get() != nullptr );
		std::stringstream ss;
		ss << spec;
		CHECK( !ss.str().empty() );
	}
}

TEST_SUITE("TauSpecGet") {
	TEST_CASE("successful get() does not poison errors_ for later parse calls") {
		// Regression test for TT-16: tau_spec::get() used to push
		// "spec failed to transform to tau tree" into errors_
		// unconditionally, even when the transform succeeded. Since
		// parse_part() bails out whenever errors_ is non-empty, every
		// parse() call after a successful get() used to silently fail.
		tau_spec<node_t> spec;
		REQUIRE( spec.parse("xy = 0.") );
		REQUIRE( spec.get() != nullptr );
		CHECK( spec.errors().empty() );
		CHECK( spec.parse("xy = 1.") );
	}

	TEST_CASE("is_eof reports true for an incomplete part awaiting continuation") {
		// TT-21: tau_spec::is_eof() is otherwise untested. An incomplete
		// part still makes parse() return true (it means "expect more
		// input", not failure); is_eof() is how a caller distinguishes it
		// from a genuinely completed parse.
		tau_spec<node_t> spec;
		CHECK( spec.parse("o[t] =") ); // incomplete, needs the RHS
		CHECK( spec.is_eof() );
		CHECK( spec.parse(" i[t].") ); // completes the expression
		CHECK( !spec.is_eof() );
		REQUIRE( spec.get() != nullptr );
	}

	TEST_CASE("a line failing alone is re-parsed as continuation of the previous part") {
		// parse_with_prev_part's success path: line 1 parses complete
		// (no eof pending), line 2 alone is a hard parse error ("&&"
		// cannot start a spec) but line 1 + line 2 parse as one spec.
		tau_spec<node_t> spec;
		REQUIRE( spec.parse("o1[t] = 0") );
		REQUIRE( !spec.is_eof() );
		CHECK( spec.parse("&& o2[t] = 0.") );
		CHECK( spec.errors().empty() );
		std::cout << "PROBE continuation errors: ";
		for (auto& e : spec.errors()) std::cout << e << " | ";
		std::cout << "\n";
		tref fm = spec.get();
		CHECK( fm != nullptr );
	}

	TEST_CASE("a continuation that still awaits input keeps expecting more") {
		// parse_with_prev_part's eof path: line 2 alone is a hard error,
		// line 1 + line 2 is a valid prefix (unclosed paren), so the
		// spec goes back to expecting more input; line 3 completes it.
		tau_spec<node_t> spec;
		REQUIRE( spec.parse("o1[t] = 0") );
		CHECK( spec.parse("&& (o2[t]") );
		CHECK( spec.is_eof() );
		CHECK( spec.parse("| o2[t]) = 0.") );
		tref fm = spec.get();
		CHECK( fm != nullptr );
	}

	TEST_CASE("a line failing alone and with the previous part reports the error") {
		// parse_with_prev_part's failure path: the combined text is no
		// continuation either, so the original error surfaces.
		tau_spec<node_t> spec;
		REQUIRE( spec.parse("o1[t] = 0") );
		CHECK( !spec.parse(") ) )") );
		CHECK( !spec.errors().empty() );
		CHECK( spec.get() == nullptr );
	}

	TEST_CASE("conflicting stream types fail to build a spec") {
		// A type conflict inside one line dies in the tau-tree
		// transform, before get()'s own inference pass.
		tau_spec<node_t> spec;
		REQUIRE( spec.parse("o1[t]:sbf = 0 && o1[t]:tau = 0.") );
		CHECK( spec.get() == nullptr );
		REQUIRE( !spec.errors().empty() );
		CHECK( spec.errors()[0] == "spec failed to transform to tau tree" );
	}

	TEST_CASE("def/main type conflict is rejected") {
		// Both lines parse on their own; the sbf-typed definition body
		// conflicts with the tau-typed stream only once they are joined,
		// and the joint transform rejects the spec. (get()'s later
		// standalone "type inference failed" pass is only reachable via
		// add()-injected defs, since the transform itself infers types.)
		tau_spec<node_t> spec;
		REQUIRE( spec.parse("f(x) := x:sbf.") );
		REQUIRE( spec.parse("o1[t]:tau = f(y).") );
		CHECK( spec.get() == nullptr );
		REQUIRE( !spec.errors().empty() );
		CHECK( spec.errors()[0] == "spec failed to transform to tau tree" );
	}

	TEST_CASE("get() after a genuine (non-eof) parse error stays nullptr and does not crash") {
		// TT-21: repeated get() after an error is otherwise untested.
		// Regression test: get() used to skip straight to build_parse_tree()
		// even when errors_ already held a genuine parse error, ending up
		// with a null main formula and crashing further down instead of
		// failing cleanly.
		tau_spec<node_t> spec;
		CHECK( !spec.parse(") = (") ); // malformed, not an eof/continuation case
		CHECK( !spec.is_eof() );
		REQUIRE( !spec.errors().empty() );
		const size_t n_errors = spec.errors().size();
		CHECK( spec.get() == nullptr );
		CHECK( spec.get() == nullptr ); // repeated call: no crash, still nullptr
		CHECK( spec.errors().size() == n_errors );
	}
}

#ifdef TAU_TEST_MULTILINE_PARSING_BRUTE_FORCE
	TEST_CASE_FIXTURE(specification_fixture, "test cases") {
		size_t start = 0;
		size_t stop = 0;
		size_t tier = 0;
		size_t skip = 0;
		size_t limit = 1000;
		size_t test_case_number = start;
		size_t skipped = 0;
		size_t tested = 0;

		for (size_t i = 0; i != start; ++i) {
			// DBG(TAU_LOG_TRACE << "skipping test case: " << i;)
			if (!next()) break;
		}
		TAU_LOG_INFO << "Starting from test case: " << start;
		do {
			++tested;
			std::string test_case = get();

			auto& defs = definitions<node_t>::instance();
			defs.clear();

			TAU_LOG_INFO << TC.GREEN() << "TEST CASE " << test_case_number << TC.CLEAR()
				<< TC.WHITE() << TC.BRIGHT() << " `" << test_case
				<< "`" << TC.CLEAR();

			tau_spec<node_t> spec;
			for (const auto& line : split(test_case, '\n')) {
				spec.parse(line);
			}
			CHECK( spec.errors().empty() );

			auto maybe_nso_rr = spec.get_nso_rr();
			CHECK( maybe_nso_rr.has_value() );
			if (!maybe_nso_rr.has_value()) {
				TAU_LOG_INFO << TC.RED() << "test_case failed" << TC.CLEAR();
				continue;
			}

			auto& nso_rr = maybe_nso_rr.value();
			CHECK( nso_rr.rec_relations.size() == 1 );
			CHECK( tau::get(nso_rr.main).to_str() == "o[t]:tau = g(i[t]:tau) && g(0) || g(1)" );

			auto* io_context = defs.get_io_context();
			CHECK( io_context->inputs.size() == 1 );
			CHECK( io_context->outputs.size() == 1 );

			++test_case_number;
			if (test_case_number >= limit) {
				++tier;
				if (tier == 1) {
					limit = limit * 3;
					skip = 2;
				} else {
					limit = limit * 2;
					skip <<= 1;
				}
			}
			if (skip > 0) {
				TAU_LOG_INFO << "skipping test cases: " << skip;
				size_t skip_stop = test_case_number + skip;
				bool end = false;
				for ( ; test_case_number < skip_stop
					&& (stop == 0 || test_case_number != stop);
					++test_case_number)
				{
					if (!next()) { end = true; break; }
					skipped++;
				}
				if (end) break;
			}
		} while (next() && (stop == 0 || test_case_number < stop));
		TAU_LOG_INFO << "Number of combinations:  " << test_case_number;
		TAU_LOG_INFO << "Number of tests:         " << tested;
		TAU_LOG_INFO << "Number of skipped tests: " << skipped;
		// TAU_LOG_INFO << "Controll sum:  " << (test_case_number - tested - skipped);
	}
#endif

}
