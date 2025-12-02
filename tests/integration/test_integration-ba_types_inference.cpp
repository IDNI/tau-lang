// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_tau_helpers.h"

#include "ba_types_inference.h"

TEST_SUITE("Configuration") {

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

tau::get_options parse_bf_no_infer() {
	static tau::get_options opts{ .parse = { .start = tau::bf },
		.infer_ba_types = false,
		.reget_with_hooks = false };
	return opts;
}

tau::get_options parse_wff_no_infer() {
	static tau::get_options opts{ .parse = { .start = tau::wff },
		.infer_ba_types = false,
		.reget_with_hooks = false };
	return opts;
}

tau::get_options parse_cli_no_infer() {
	static tau::get_options opts{ .parse = { .start = tau::cli },
		.infer_ba_types = false,
		.reget_with_hooks = false };
	return opts;
}

tau::get_options parse_definitions_no_infer() {
	static tau::get_options opts{ .parse = { .start = tau::definitions },
		.infer_ba_types = false,
		.reget_with_hooks = false };
	return opts;
}

tref parse(const std::string& sample, const tau::get_options& opts = parse_wff_no_infer()) {
	auto src = tree<node_t>::get(sample, opts);
	if (src == nullptr) {
		TAU_LOG_ERROR << "Parsing failed for: " << sample;
	}
	return src;
}

tref parse_bf(const std::string& sample, const tau::get_options& opts = parse_bf_no_infer()) {
	auto src = tree<node_t>::get(sample, opts);
	if (src == nullptr) {
		TAU_LOG_ERROR << "Parsing failed for: " << sample;
	}
	return src;
}

tref parse_definitions(const std::string& sample, const tau::get_options& opts = parse_definitions_no_infer()) {
	auto src = tree<node_t>::get(sample, opts);
	if (src == nullptr) {
		TAU_LOG_ERROR << "Parsing failed for: " << sample;
	}
	return src;
}

using size_t = size_t;

TEST_SUITE("type_scoped_resolver") {

	TEST_CASE("merging in the same scope") {
		type_scoped_resolver<node_t> r;
		tref a = parse("T"); // just an existing tref for testing
		tref b = parse("F"); // just an existing tref for testing
		r.insert(a);
		r.insert(b);
		size_t t = 1;
		CHECK(r.assign(a, t));
		CHECK(r.merge(a, b));
		CHECK(r.type_id_of(a) == r.type_id_of(b));
		CHECK(r.type_id_of(a) == t);
	}

	TEST_CASE("merging in different scopes") {
		type_scoped_resolver<node_t> r;
		tref a = parse("T"); // just an existing tref for testing
		tref b = parse("F"); // just an existing tref for testing
		size_t t = 1;
		r.insert(a);
		r.open({{b, t}});
		CHECK(r.merge(a, b));
		CHECK(r.type_id_of(a) == r.type_id_of(b));
		CHECK(r.type_id_of(a) == t);
	}

	TEST_CASE("merging conflicting types in the same scope") {
		type_scoped_resolver<node_t> r;
		tref a = parse("T"); // just an existing tref for testing
		tref b = parse("F"); // just an existing tref for testing
		r.insert(a);
		r.insert(b);
		size_t t1 = 1;
		size_t t2 = 2;
		CHECK(r.assign(a, t1));
		CHECK(r.assign(b, t2));
		CHECK(!r.merge(a, b)); // conflicting types
	}

	TEST_CASE("merging conflicting types in different scopes") {
		type_scoped_resolver<node_t> r;
		tref a = parse("T"); // just an existing tref for testing
		tref b = parse("F"); // just an existing tref for testing
		size_t t1 = 1;
		size_t t2 = 2;
		r.insert(a);
		CHECK(r.assign(a, t1));
		r.open({{b, t2}});
		CHECK(!r.merge(a, b)); // conflicting types
	}
}

bool check_vars(tref inferred, std::vector<std::pair<std::string, size_t>>& expected) {
	auto vars = tau::get(inferred).select_top(is<node_t, tau::variable>);
	if (vars.empty() && expected.size() > 0) {
		TAU_LOG_ERROR << "No variables found in.";
		return false;
	}
	if (vars.size() != expected.size()) {
		TAU_LOG_ERROR << "Expected " << expected.size()
			<< " variables, found " << vars.size();
		return false;
	}
	for (size_t i = 0; i < expected.size(); i++) {
		auto [expected_name, expected_type] = expected[i];
		auto name = get_var_name<node_t>(vars[i]);
		if (name != expected_name) {
			TAU_LOG_ERROR << "Variable " << (i+1)
				<< " expected name '" << expected_name
				<< "', found '" << name;
			return false;
		}
		size_t vtype = tau::get(vars[i]).get_ba_type();
		if (vtype != expected_type) {
			TAU_LOG_ERROR << "Variable '" << name
			<< "' expected type " << ba_types<node_t>::name(expected_type)
			<< ", found " << ba_types<node_t>::name(vtype);
			return false;
		}
		TAU_LOG_TRACE << "Variable " << name << " matched\n";
	}
	return true;
}

bool check_refs(tref inferred, std::vector<size_t>& expected) {
	auto refs = tau::get(inferred).select_top(is<node_t, tau::ref>);
	if (refs.empty() && expected.size() > 0) {
		TAU_LOG_ERROR << "No refs found";
		return false;
	}
	if (refs.size() != expected.size()) {
		TAU_LOG_ERROR << "Expected " << expected.size()
			<< " refs, found " << refs.size();
		return false;
	}
	for (size_t i = 0; i < expected.size(); i++) {
		size_t ctype = tau::get(refs[i]).get_ba_type();
		if (ctype != expected[i]) {
			TAU_LOG_ERROR << "Ref '" << refs[i]
				<< "' expected type " << ba_types<node_t>::name(expected[i])
				<< ", found " << ba_types<node_t>::name(ctype);
			return false;
		}
		TAU_LOG_TRACE << "Ref '" << refs[i] << " matched\n";
	}
	return true;
}

bool check_ctes(tref inferred, std::vector<size_t>& expected) {
	auto ctes = tau::get(inferred).select_top(is<node_t, tau::ba_constant>);
	if (ctes.empty() && expected.size() > 0) {
		TAU_LOG_ERROR << "No constants found";
		return false;
	}
	if (ctes.size() != expected.size()) {
		TAU_LOG_ERROR << "Expected " << expected.size()
			<< " constants, found " << ctes.size();
		return false;
	}
	for (size_t i = 0; i < expected.size(); i++) {
		size_t ctype = tau::get(ctes[i]).get_ba_type();
		if (ctype != expected[i]) {
			TAU_LOG_ERROR << "Constant '" << ctes[i]
				<< "' expected type " << ba_types<node_t>::name(expected[i])
				<< ", found " << ba_types<node_t>::name(ctype);
			return false;
		}
		TAU_LOG_TRACE << "Constant '" << ctes[i] << " matched\n";
	}
	return true;
}

bool check_bv_ctes(tref inferred, std::vector<size_t>& expected) {
	auto ctes = tau::get(inferred).select_top(is<node_t, tau::ba_constant>);
	if (ctes.empty() && expected.size() > 0) {
		TAU_LOG_ERROR << "No constants found";
		return false;
	}
	if (ctes.size() != expected.size()) {
		TAU_LOG_ERROR << "Expected " << expected.size()
			<< " constants, found " << ctes.size();
		return false;
	}
	for (size_t i = 0; i < expected.size(); i++) {
		size_t ctype = tau::get(ctes[i]).get_ba_type();
		if (ctype != expected[i]) {
			TAU_LOG_ERROR << "Constant '" << ctes[i]
				<< "' expected type " << ba_types<node_t>::name(expected[i])
				<< ", found " << ba_types<node_t>::name(ctype);
			return false;
		}
	}
	return true;
}

TEST_SUITE("infer_ba_types: variables and constants") {

	TEST_CASE("simple case 1") {
		tref parsed = parse("x = 1");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 1 (y2)") {
		tref parsed = parse("1 = x");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 2") {
		tref parsed = parse("x = 1:sbf");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 2 (y2)") {
		tref parsed = parse("1:sbf = x");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 3") {
		tref parsed = parse("x = 1:tau");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 3 (y2)") {
		tref parsed = parse("1:tau = x");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 4") {
		tref parsed = parse("x = { #b1001 }:bv");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 4 (y2)") {
		// TODO (HIGH) should fail
		// tref parsed = parse("x = 1:bv[8]");
		tref parsed = parse("x = { #b1001 }:bv[8]");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv8_type_id<node_t>}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 4 (y3)") {
		tref parsed = parse("x:bv =  { #b1001 }");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<size_t> {
			bv_type_id<node_t>()
		};
		CHECK( check_bv_ctes(inferred, expected) );
	}

	TEST_CASE("simple case 4 (y4)") {
		tref parsed = parse("x:bv[16] ={ 1 }");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<size_t> {
			bv_type_id<node_t>()
		};
		CHECK( check_bv_ctes(inferred, expected) );
	}

	TEST_CASE("simple case 7") {
		tref parsed = parse("x:sbf = y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", sbf_type_id<node_t>()},
			{"y", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 7 (y2)") {
		tref parsed = parse("x = y:sbf");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", sbf_type_id<node_t>()},
			{"y", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 8") {
		tref parsed = parse("x:tau = y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()},
			{"y", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 8 (y2)") {
		tref parsed = parse("x = y:tau");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()},
			{"y", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 9") {
		tref parsed = parse("x:bv = y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv_type_id<node_t>()},
			{"y", bv_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 9 (y2)") {
		tref parsed = parse("x = y:bv");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv_type_id<node_t>()},
			{"y", bv_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 9 (y3)") {
		tref parsed = parse("x:bv[8] = y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv8_type_id<node_t>},
			{"y", bv8_type_id<node_t>}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 9 (y2)") {
		tref parsed = parse("x = y:bv[8]");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv8_type_id<node_t>},
			{"y", bv8_type_id<node_t>}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("type chaining (tau) right to left") {
		tref parsed = parse("x = y && y = z:tau");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()},
			{"y", tau_type_id<node_t>()},
			{"z", tau_type_id<node_t>()},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("type chaining (tau) left to right") {
		tref parsed = parse("x:tau = y && y = z");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()},
			{"y", tau_type_id<node_t>()},
			{"z", tau_type_id<node_t>()},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("type chaining (sbf) right to left") {
		tref parsed = parse("x = y && y = z:sbf");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", sbf_type_id<node_t>()},
			{"y", sbf_type_id<node_t>()},
			{"z", sbf_type_id<node_t>()},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("type chaining (sbf) left to right") {
		tref parsed = parse("x:sbf = y && y = z");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", sbf_type_id<node_t>()},
			{"y", sbf_type_id<node_t>()},
			{"z", sbf_type_id<node_t>()},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("type chaining (bv) left to right") {
		tref parsed = parse("x:bv = y && y = z");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv_type_id<node_t>()},
			{"y", bv_type_id<node_t>()},
			{"z", bv_type_id<node_t>()},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("type chaining (bv[8]) left to right") {
		tref parsed = parse("x:bv[8] = y && y = z");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv8_type_id<node_t>},
			{"y", bv8_type_id<node_t>},
			{"z", bv8_type_id<node_t>},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("type chaining (bv) right to left") {
		tref parsed = parse("x = y && y = z:bv");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv_type_id<node_t>()},
			{"y", bv_type_id<node_t>()},
			{"z", bv_type_id<node_t>()},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("type chaining (bv[8]) right to left") {
		tref parsed = parse("x = y && y = z:bv[8]");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv8_type_id<node_t>},
			{"y", bv8_type_id<node_t>},
			{"z", bv8_type_id<node_t>},
		};
		CHECK( check_vars(inferred, expected) );
	}

	// ex quantifiers behave the same as all quantifiers for type inference
	// so no neede to test them separately

	TEST_CASE("all bv") {
		tref parsed = parse("all x:bv x = y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv_type_id<node_t>()},
			{"y", bv_type_id<node_t>()},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all bv (y2)") {
		tref parsed = parse("all x x:bv = y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv_type_id<node_t>()},
			{"y", bv_type_id<node_t>()},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all bv (y3)") {
		tref parsed = parse("all x x = y:bv");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv_type_id<node_t>()},
			{"y", bv_type_id<node_t>()},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all bv[8]") {
		tref parsed = parse("all x:bv[8] x = y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv8_type_id<node_t>},
			{"y", bv8_type_id<node_t>},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all bv[8] (y2)") {
		tref parsed = parse("all x x:bv[8] = y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv8_type_id<node_t>},
			{"y", bv8_type_id<node_t>},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all bv[8] (y3)") {
		tref parsed = parse("all x x = y:bv[8]");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv8_type_id<node_t>},
			{"y", bv8_type_id<node_t>},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all sbf") {
		tref parsed = parse("all x:sbf x = y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", sbf_type_id<node_t>()},
			{"y", sbf_type_id<node_t>()},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all sbf (y2)") {
		tref parsed = parse("all x x:sbf = y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", sbf_type_id<node_t>()},
			{"y", sbf_type_id<node_t>()},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all sbf (y3)") {
		tref parsed = parse("all x x = y:sbf");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", sbf_type_id<node_t>()},
			{"y", sbf_type_id<node_t>()},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all tau") {
		tref parsed = parse("all x:tau x = y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()},
			{"y", tau_type_id<node_t>()},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all tau (y2)") {
		tref parsed = parse("all x x:tau = y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()},
			{"y", tau_type_id<node_t>()},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all tau (y3)") {
		tref parsed = parse("all x x = y:tau");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()},
			{"y", tau_type_id<node_t>()},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple explicit tau constant") {
		tref parsed = parse("x = { T }:tau");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple explicit tau constant  (y2)") {
		tref parsed = parse("{ T }:tau = x");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple explicit sbf constant") {
		bdd_init<Bool>();
		tref parsed = parse("x = { x }:sbf");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple explicit sbf constant (y2)") {
		bdd_init<Bool>();
		tref parsed = parse("{ x }:sbf = x");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple failing case") {
		tref parsed = parse("x:tau = x:sbf");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("simple failing case (y2)") {
		tref parsed = parse("x:tau = {x}:sbf");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("simple failing case (y3)") {
		tref parsed = parse("x:tau = 1:sbf");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("simple failing case (y4)") {
		tref parsed = parse("x:tau ={ 1 }");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("simple failing case (y5)") {
		tref parsed = parse("x:tau = x:bv");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("bv constants") {
		tref parsed = parse("{ #b1 }: bv = { #b0 } : bv & { #b10 }");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected_ctes = std::vector<size_t> {
			bv_type_id<node_t>(),
			bv_type_id<node_t>(),
			bv_type_id<node_t>()
		};
		CHECK( check_bv_ctes(inferred, expected_ctes) );
	}

	TEST_CASE("bv8 constants") {
		tref parsed = parse("{ #b1 } : bv[8] = { #b0 } : bv[8] & { #b10 } : bv");
		CHECK( parsed != nullptr );
		if (parsed == nullptr) {
			TAU_LOG_ERROR << "Parsing failed";
			return;
		}
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected_ctes = std::vector<size_t> {
			bv8_type_id<node_t>,
			bv8_type_id<node_t>,
			bv8_type_id<node_t>
		};
		CHECK( check_bv_ctes(inferred, expected_ctes) );
	}

	TEST_CASE("sbf constants") {
		tref parsed = parse("{ 0 } : sbf = { 1 } { 0 } : sbf");
		CHECK( parsed != nullptr );
		if (parsed == nullptr) {
			TAU_LOG_ERROR << "Parsing failed";
			return;
		}
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected_ctes = std::vector<size_t> {
			sbf_type_id<node_t>(),
			sbf_type_id<node_t>()
		};
		CHECK( check_ctes(inferred, expected_ctes) );
	}

	TEST_CASE("sbf constants (y2)") {
		tref parsed = parse("{ 0 } = { 1 } & { 0 } : sbf");
		CHECK( parsed != nullptr );
		if (parsed == nullptr) {
			TAU_LOG_ERROR << "Parsing failed";
			return;
		}
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected_ctes = std::vector<size_t> {
			sbf_type_id<node_t>(),
			sbf_type_id<node_t>()
		};
		CHECK( check_ctes(inferred, expected_ctes) );
	}

	TEST_CASE("tau constants") {
		tref parsed = parse("{ F } = { T } { F }");
		CHECK( parsed != nullptr );
		if (parsed == nullptr) {
			TAU_LOG_ERROR << "Parsing failed";
			return;
		}
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected_ctes = std::vector<size_t> {
			tau_type_id<node_t>(),
			tau_type_id<node_t>()
		};
		CHECK( check_ctes(inferred, expected_ctes) );
	}

	TEST_CASE("complex case: Ohad's example") {
		tref parsed = parse("all x x = y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()},
			{"y", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("complex case: Ohad's example (y2)") {
		using node = node_t;
		tref parsed = parse("(all x x = y) && x = y:sbf");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		if (inferred != nullptr) {
			TAU_LOG_TRACE << "Expected nullptr got: " << TAU_LOG_FM_TREE(inferred);
		}
		CHECK( inferred == nullptr );
	}

	TEST_CASE("complex case: shadowing") {
		tref parsed = parse("all x (all x x = 1:sbf)");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()},
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("complex case: shadowing (y2)") {
		tref parsed = parse("all x ((all x x = 1:sbf) && x = 1:sbf)");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("complex case: shadowing (y3)") {
		tref parsed = parse("all x ((all x x = 1:sbf) && x = 1:tau)");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()},
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("complex case: bv chaining") {
		tref parsed = parse("all x x & y = z && y = { 1 }:bv[8]");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv8_type_id<node_t>},
			{"y", bv8_type_id<node_t>},
			{"z", bv8_type_id<node_t>}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("mixed sbf and tau") {
		tref parsed = parse("ex x ex y x = 0:tau && xy != 0:sbf");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}
}

TEST_SUITE("infer_ba_types: symbols") {

	template<typename node_t::type nt>
	bool check_symbol(tref inferred,
			size_t expected_type) {
		auto syms = tau::get(inferred).select_top(is<node_t, nt>);
		if (syms.empty()) {
			TAU_LOG_ERROR << "No symbols found";
			return false;
		}
		for (auto& s : syms) {
			auto stype = tau::get(s).get_ba_type();
			if (stype != expected_type) {
				TAU_LOG_ERROR << "Symbol '" << node_t::name(nt)
					<< "' expected type " << ba_types<node_t>::name(expected_type)
					<< ", found " << ba_types<node_t>::name(stype);
				return false;
			}
		}
		return true;
	}

	TEST_CASE("tau bf_eq symbol") {
		tref parsed = parse("x:tau = y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_eq>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("sbf bf_eq symbol") {
		tref parsed = parse("x:sbf = y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_eq>(inferred, sbf_type_id<node_t>()) );
	}

	TEST_CASE("bv bf_eq symbol") {
		tref parsed = parse("x:bv = y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_eq>(inferred, bv_type_id<node_t>()) );
	}

	TEST_CASE("bv[8] bf_eq symbol") {
		tref parsed = parse("x:bv[8] = y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_eq>(inferred, bv8_type_id<node_t>) );
	}

	TEST_CASE("tau bf_eq symbol involving constant") {
		tref parsed = parse("x:tau = { x = 0 }");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_eq>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_eq symbol involving bf_t") {
		tref parsed = parse("x:tau = 1");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_eq>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_eq symbol involving bf_f") {
		tref parsed = parse("x:tau = 0");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_eq>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_neq symbol") {
		tref parsed = parse("x:tau != 0");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_neq>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_lteq symbol") {
		tref parsed = parse("x:tau <= 0");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_lteq>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_nlteq symbol") {
		tref parsed = parse("x:tau !<= 0");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_nlteq>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_gt symbol") {
		tref parsed = parse("x:tau > 0");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_gt>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_ngt symbol") {
		tref parsed = parse("x:tau !> 0");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_ngt>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_gteq symbol") {
		tref parsed = parse("x:tau >= 0");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_gteq>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_ngteq symbol") {
		tref parsed = parse("x:tau !>= 0");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_ngteq>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_lt symbol") {
		tref parsed = parse("x:tau < 0");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_lt>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_nlt symbol") {
		tref parsed = parse("x:tau !< 0");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_nlt>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_interval symbol") {
		tref parsed = parse("x:tau <= y <= z");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_interval>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("sbf bf_interval symbol") {
		tref parsed = parse("x:sbf <= y <= z");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_interval>(inferred, sbf_type_id<node_t>()) );
	}

	TEST_CASE("bv bf_interval symbol") {
		tref parsed = parse("x:bv <= y <= z");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_interval>(inferred, bv_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_and symbol") {
		tref parsed = parse_bf("x:bv & y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_and>(inferred, bv_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_or symbol") {
		tref parsed = parse_bf("x:bv | y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_or>(inferred, bv_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_xor symbol") {
		tref parsed = parse_bf("x:bv ^ y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_xor>(inferred, bv_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_neg symbol") {
		tref parsed = parse_bf("x:bv'");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_neg>(inferred, bv_type_id<node_t>()) );
	}

	TEST_CASE("bv bf_add symbol") {
		tref parsed = parse_bf("x:bv + y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_add>(inferred, bv_type_id<node_t>()) );
	}

	TEST_CASE("bv bf_sub symbol") {
		tref parsed = parse_bf("x:bv - y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_sub>(inferred, bv_type_id<node_t>()) );
	}

	TEST_CASE("bv bf_mul symbol") {
		tref parsed = parse_bf("x:bv * y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_mul>(inferred, bv_type_id<node_t>()) );
	}

	TEST_CASE("bv bf_div symbol") {
		tref parsed = parse_bf("x:bv / y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_div>(inferred, bv_type_id<node_t>()) );
	}

	TEST_CASE("bv bf_mod symbol") {
		tref parsed = parse_bf("x:bv % y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_mod>(inferred, bv_type_id<node_t>()) );
	}

	TEST_CASE("bv bf_shr symbol") {
		tref parsed = parse_bf("x:bv >> y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_shr>(inferred, bv_type_id<node_t>()) );
	}

	TEST_CASE("bv bf_shl symbol") {
		tref parsed = parse_bf("x:bv << y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_shl>(inferred, bv_type_id<node_t>()) );
	}

	TEST_CASE("bv[8] bf_shl symbol") {
		tref parsed = parse_bf("x:bv[8] << y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_shl>(inferred, bv8_type_id<node_t>) );
	}

	TEST_CASE("tau bf_shl symbol") {
		tref parsed = parse_bf("x:tau << y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("sbf bf_shl symbol") {
		tref parsed = parse_bf("x:sbf << y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("sbf xor symbol") {
		tref parsed = parse_bf("x:sbf ^ y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_xor>(inferred, sbf_type_id<node_t>()) );
	}

	TEST_CASE("sbf xor symbol (y2)") {
		tref parsed = parse_bf("x:sbf ^ 1");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_xor>(inferred, sbf_type_id<node_t>()) );
	}

	TEST_CASE("sbf xor symbol (y3)") {
		tref parsed = parse_bf("1 ^ y:sbf");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_xor>(inferred, sbf_type_id<node_t>()) );
	}

	TEST_CASE("tau xor symbol") {
		tref parsed = parse_bf("x ^ y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_xor>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau xor symbol (y2)") {
		tref parsed = parse_bf("x ^ 1");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_xor>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau xor symbol (y3)") {
		tref parsed = parse_bf("1 ^ y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_xor>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("bv xor symbol") {
		tref parsed = parse_bf("x:bv ^ y");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_xor>(inferred, bv_type_id<node_t>()) );
	}

	TEST_CASE("bv xor symbol (y2)") {
		tref parsed = parse_bf("x:bv ^ 1");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_xor>(inferred, bv_type_id<node_t>()) );
	}

	TEST_CASE("bv xor symbol (y3)") {
		tref parsed = parse_bf("1 ^ y:bv");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_xor>(inferred, bv_type_id<node_t>()) );
	}
}

TEST_SUITE("infer_ba_types: bf formulas") {

	TEST_CASE("simple case 1") {
		tref parsed = parse("x", parse_bf_no_infer());
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		DBG(LOG_INFO << "Inferred: " << tau::get(inferred).tree_to_str();)
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 2") {
		tref parsed = parse("f[8](x)", parse_bf_no_infer());
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		DBG(LOG_INFO << "Inferred: " << tau::get(inferred).tree_to_str();)
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}
}

TEST_SUITE("infer_ba_types: cli commands") {

	TEST_CASE("simple case 1") {
		tref parsed = parse("x = 1", parse_cli_no_infer());
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		DBG(LOG_INFO << "Inferred: " << tau::get(inferred).tree_to_str();)
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}
}


TEST_SUITE("infer_ba_types: definitions") {

	TEST_CASE("absent typing") {
		tref parsed = parse_definitions("g[n](x) := g[n-1](x).");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		DBG(LOG_INFO << "Inferred: " << tau::get(inferred).tree_to_str();)
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"n", untyped_type_id<node_t>()},
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}
	
	// Should we allow this?

	/*TEST_CASE("functional sbf typing: right position") {
		tref parsed = parse_definitions("g[n](x) := g[n-1](x):sbf.");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		DBG(LOG_INFO << "Inferred: " << tau::get(inferred).tree_to_str();)
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"n", untyped_type_id<node_t>()},
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}*/

	TEST_CASE("functional sbf typing: left position") {
		tref parsed = parse_definitions("g[n](x):sbf := g[n-1](x).");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		DBG(LOG_INFO << "Inferred: " << tau::get(inferred).tree_to_str();)
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"n", untyped_type_id<node_t>()},
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("predicate sbf: in the body") {
		tref parsed = parse_definitions("g[n](x) := g[n-1](x:sbf).");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	/*TEST_CASE("predicate sbf: right position") {
		tref parsed = parse_definitions("g[n](x:sbf) := g[n-1](x).");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		DBG(LOG_INFO << "Inferred: " << tau::get(inferred).tree_to_str();)
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"n", untyped_type_id<node_t>()},
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}*/ 

	TEST_CASE("incompatible types") {
		tref parsed = parse_definitions("g[n](x:tau) := g[n-1](x:sbf).");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

}

TEST_SUITE("infer_ba_types: I/O vars") {

	TEST_CASE("different time stamp propagation") {
		DBG(using node = node_t;)
		tref parsed = parse("i1[t] = o1[t] && i1[t-1] = 1:sbf");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		DBG(LOG_INFO << "Inferred: " << LOG_FM_TREE(inferred);)
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"i1", sbf_type_id<node_t>()},
			{"o1", sbf_type_id<node_t>()},
			{"i1", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}
	TEST_CASE("Propagation of bv type") {
		DBG(using node = node_t;)
		tref parsed = parse("o1[t]:bv = o1[t-1] + o1[t-2] && o1[0] = { 1 } && o1[1] = { 1 }");
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		DBG(std::cout << "Inferred: " << LOG_FM_TREE(inferred);)
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"o1", bv_type_id<node_t>(16)},
			{"o1", bv_type_id<node_t>(16)},
			{"o1", bv_type_id<node_t>(16)},
			{"o1", bv_type_id<node_t>(16)},
			{"o1", bv_type_id<node_t>(16)}
		};
		CHECK( check_vars(inferred, expected) );
	}
}


TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
