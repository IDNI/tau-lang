// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#include "api.h"

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
