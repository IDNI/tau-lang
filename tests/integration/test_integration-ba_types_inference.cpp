// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_tau_helpers.h"

#include "ba_types_inference.h"

TEST_SUITE("Configuration") {

	TEST_CASE("logging") {
		logging::trace();
	}

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

tref parse(const std::string& sample, const tau::get_options& opts = parse_wff_no_infer()) {
	auto src = tree<node_t>::get(sample, opts);
	if (src == nullptr) {
		TAU_LOG_ERROR << "Parsing failed for: " << sample;
	}
	return src;
}

using type_t = tree<node_t>::type_t;

TEST_SUITE("type_scoped_resolver") {

	TEST_CASE("merging in the same scope") {
		type_scoped_resolver<node_t> r;
		tref a = parse("T"); // just an existing tref for testing
		tref b = parse("F"); // just an existing tref for testing
		r.insert(a);
		r.insert(b);
		type_t t = {1, (tref)1};
		CHECK(r.assign(a, t));
		CHECK(r.merge(a, b));
		CHECK(r.type_of(a) == r.type_of(b));
		CHECK(r.type_of(a) == t);
	}

	TEST_CASE("merging in different scopes") {
		type_scoped_resolver<node_t> r;
		tref a = parse("T"); // just an existing tref for testing
		tref b = parse("F"); // just an existing tref for testing
		type_t t = {1, (tref)1};
		r.insert(a);
		r.open({{b, t}});
		CHECK(r.merge(a, b));
		CHECK(r.type_of(a) == r.type_of(b));
		CHECK(r.type_of(a) == t);
	}

	TEST_CASE("merging conflicting types in the same scope") {
		type_scoped_resolver<node_t> r;
		tref a = parse("T"); // just an existing tref for testing
		tref b = parse("F"); // just an existing tref for testing
		r.insert(a);
		r.insert(b);
		type_t t1 = {1, (tref)1};
		type_t t2 = {2, (tref)2};
		CHECK(r.assign(a, t1));
		CHECK(r.assign(b, t2));
		CHECK(!r.merge(a, b)); // conflicting types
	}

	TEST_CASE("merging conflicting types in different scopes") {
		type_scoped_resolver<node_t> r;
		tref a = parse("T"); // just an existing tref for testing
		tref b = parse("F"); // just an existing tref for testing
		type_t t1 = {1, (tref)1};
		type_t t2 = {2, (tref)2};
		r.insert(a);
		CHECK(r.assign(a, t1));
		r.open({{b, t2}});
		CHECK(!r.merge(a, b)); // conflicting types
	}
}

bool check_vars(tref inferred, std::vector<std::pair<std::string, type_t>>& expected) {
	// using node = node_t;
	//
	// DBG(LOG_TRACE << "Checking variables in: "
	// 	<< LOG_FM_TREE(inferred);)

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
		if (vtype != expected_type.first) {
			TAU_LOG_ERROR << "Variable '" << name
			<< "' expected type id " << expected_type.first
			<< ", found " << vtype;
			return false;
		}
		auto vsubtype = tt(vars[i]) | tau::subtype | tt::ref;
		if (vsubtype != expected_type.second) {
			TAU_LOG_ERROR << "Variable '" << name
			<< "' unexpected subtype '";
			return false;
		}
		TAU_LOG_TRACE << "Variable " << name << " matched\n";
	}
	return true;
}

bool check_ctes(tref inferred, std::vector<type_t>& expected) {
	// using node = node_t;
	//
	// DBG(LOG_TRACE << "Checking bf constants in: "
	// 	<< LOG_FM_TREE(inferred);)

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
		if (ctype != expected[i].first) {
			TAU_LOG_ERROR << "Constant '" << ctes[i]
				<< "' expected type id " << expected[i].first
				<< ", found " << ctype;
			return false;
		}
		auto csubtype = tt(ctes[i]) | tau::subtype | tt::ref;
		if (csubtype != expected[i].second) {
			TAU_LOG_ERROR << "Constant '" << ctes[i]
				<< "' unexpected subtype '";
			return false;
		}
		TAU_LOG_TRACE << "Constant '" << ctes[i] << " matched\n";
	}
	return true;
}

bool check_bv_ctes(tref inferred, std::vector<type_t>& expected) {
	// using node = node_t;
	//
	// DBG(LOG_TRACE << "Checking bv constants in: "
	// 	<< LOG_FM_TREE(inferred);)

	auto ctes = tau::get(inferred).select_top(is<node_t, tau::bv_constant>);
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
		if (ctype != expected[i].first) {
			TAU_LOG_ERROR << "Constant '" << ctes[i]
				<< "' expected type id " << expected[i].first
				<< ", found " << ctype;
			return false;
		}
		auto csubtype = tt(ctes[i]) | tau::subtype | tt::ref;
		if (csubtype == nullptr) {
			auto es = tt(expected[i].second) | tau::num | tt::num;
			auto cs = tt(ctes[i]) | tt::ba_constant;
			if (std::holds_alternative<cvc5::Term>(cs) && es != 0) {
				auto c = std::get<cvc5::Term>(cs);
				if (c.getSort().getBitVectorSize() == es) continue;
				TAU_LOG_ERROR << "Constant '" << ctes[i]
					<< "' expected subtype '" << expected[i].second
					<< "', found 'untyped'" << c.getSort().getBitVectorSize();
				return false;
			}
		}
		if (csubtype != expected[i].second) {
			TAU_LOG_ERROR << "Constant '" << ctes[i]
				<< "' expected subtype '" << expected[i].second
				<< "', found '" << tau::get(csubtype).dump_to_str() ;
			return false;
		}
	}
	return true;
}

static type_t tau_type = {get_ba_type_id<node_t>("tau"), nullptr};
static type_t sbf_type = {get_ba_type_id<node_t>("sbf"), nullptr};
static type_t bv_type = {get_ba_type_id<node_t>("bv"), nullptr};
static type_t bv16_type = {get_ba_type_id<node_t>("bv"), tau::get(tau::subtype, tau::get_num(16))};

TEST_SUITE("new_infer_ba_types: wff formulas") {

	TEST_CASE("simple case 1") {
		tref parsed = parse("x = 1");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", tau_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 1 (y2)") {
		tref parsed = parse("1 = x");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", tau_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 2") {
		tref parsed = parse("x = 1:sbf");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", sbf_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 2 (y2)") {
		tref parsed = parse("1:sbf = x");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", sbf_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 3") {
		tref parsed = parse("x = 1:tau");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", tau_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 3 (y2)") {
		tref parsed = parse("1:tau = x");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", tau_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 4") {
		// TODO (HIGH) should fail
		// tref parsed = parse("x = 1:bv");
		tref parsed = parse("x =_ 1:bv");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", bv_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 4 (y2)") {
		// TODO (HIGH) should fail
		// tref parsed = parse("x = 1:bv[16]");
		tref parsed = parse("x =_ 1:bv[16]");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", bv16_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 4 (y3)") {
		tref parsed = parse("x:bv =_ 1");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<type_t> {
			{bv_type}
		};
		CHECK( check_bv_ctes(inferred, expected) );
	}

	TEST_CASE("simple case 4 (y4)") {
		tref parsed = parse("x:bv[16] =_ 1");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<type_t> {
			{bv16_type}
		};
		CHECK( check_bv_ctes(inferred, expected) );
	}

	TEST_CASE("simple case 7") {
		tref parsed = parse("x:sbf = y");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", sbf_type},
			{"y", sbf_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 7 (y2)") {
		tref parsed = parse("x = y:sbf");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", sbf_type},
			{"y", sbf_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 8") {
		tref parsed = parse("x:tau = y");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", tau_type},
			{"y", tau_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 8 (y2)") {
		tref parsed = parse("x = y:tau");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", tau_type},
			{"y", tau_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 9") {
		tref parsed = parse("x:bv =_ y");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", bv_type},
			{"y", bv_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 9 (y2)") {
		tref parsed = parse("x =_ y:bv");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", bv_type},
			{"y", bv_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 9 (y3)") {
		tref parsed = parse("x:bv[16] =_ y");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", bv16_type},
			{"y", bv16_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 9 (y2)") {
		tref parsed = parse("x =_ y:bv[16]");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", bv16_type},
			{"y", bv16_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("type chaining (tau) right to left") {
		tref parsed = parse("x = y && y = z:tau");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", tau_type},
			{"y", tau_type},
			{"z", tau_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("type chaining (tau) left to right") {
		tref parsed = parse("x:tau = y && y = z");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", tau_type},
			{"y", tau_type},
			{"z", tau_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("type chaining (sbf) right to left") {
		tref parsed = parse("x = y && y = z:sbf");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", sbf_type},
			{"y", sbf_type},
			{"z", sbf_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("type chaining (sbf) left to right") {
		tref parsed = parse("x:sbf = y && y = z");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", sbf_type},
			{"y", sbf_type},
			{"z", sbf_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("type chaining (bv) left to right") {
		tref parsed = parse("x:bv =_ y && y =_ z");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", bv_type},
			{"y", bv_type},
			{"z", bv_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("type chaining (bv[16]) left to right") {
		tref parsed = parse("x:bv[16] =_ y && y =_ z");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", bv16_type},
			{"y", bv16_type},
			{"z", bv16_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("type chaining (bv) right to left") {
		tref parsed = parse("x =_ y && y =_ z:bv");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", bv_type},
			{"y", bv_type},
			{"z", bv_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("type chaining (bv[16]) right to left") {
		tref parsed = parse("x =_ y && y =_ z:bv[16]");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", bv16_type},
			{"y", bv16_type},
			{"z", bv16_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	// ex quantifiers behave the same as all quantifiers for type inference
	// so no neede to test them separately

	TEST_CASE("all bv") {
		tref parsed = parse("all x:bv x =_ y");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", bv_type},
			{"y", bv_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all bv (y2)") {
		tref parsed = parse("all x x:bv =_ y");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", bv_type},
			{"y", bv_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all bv (y3)") {
		tref parsed = parse("all x x =_ y:bv");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", bv_type},
			{"y", bv_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all bv[16]") {
		tref parsed = parse("all x:bv[16] x =_ y");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", bv16_type},
			{"y", bv16_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all bv[16] (y2)") {
		tref parsed = parse("all x x:bv[16] =_ y");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", bv16_type},
			{"y", bv16_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all bv[16] (y3)") {
		tref parsed = parse("all x x =_ y:bv[16]");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", bv16_type},
			{"y", bv16_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all sbf") {
		tref parsed = parse("all x:sbf x = y");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", sbf_type},
			{"y", sbf_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all sbf (y2)") {
		tref parsed = parse("all x x:sbf = y");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", sbf_type},
			{"y", sbf_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all sbf (y3)") {
		tref parsed = parse("all x x = y:sbf");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", sbf_type},
			{"y", sbf_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all tau") {
		tref parsed = parse("all x:tau x = y");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", tau_type},
			{"y", tau_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all tau (y2)") {
		tref parsed = parse("all x x:tau = y");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", tau_type},
			{"y", tau_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all tau (y3)") {
		tref parsed = parse("all x x = y:tau");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", tau_type},
			{"y", tau_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple explicit tau constant") {
		tref parsed = parse("x = { T }:tau");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", tau_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple explicit tau constant  (y2)") {
		tref parsed = parse("{ T }:tau = x");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", tau_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple explicit sbf constant") {
		tref parsed = parse("x = { x }:sbf");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", sbf_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple explicit sbf constant (y2)") {
		tref parsed = parse("{ x }:sbf = x");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", sbf_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple failing case") {
		tref parsed = parse("x:tau = x:sbf");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("simple failing case (y2)") {
		tref parsed = parse("x:tau = {x}:sbf");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("simple failing case (y3)") {
		tref parsed = parse("x:tau = 1:sbf");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("simple failing case (y4)") {
		tref parsed = parse("x:tau =_ 1");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("simple failing case (y5)") {
		tref parsed = parse("x:tau =_ x:bv");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("bv constants") {
		tref parsed = parse("#b1 : bv =_ #b0 : bv & #b10");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected_ctes = std::vector<type_t> {
			bv_type,
			bv_type,
			bv_type
		};
		CHECK( check_bv_ctes(inferred, expected_ctes) );
	}

	TEST_CASE("bv16 constants") {
		tref parsed = parse("#b1 : bv[16] =_ #b0 : bv[16] & #b10 : bv");
		CHECK( parsed != nullptr );
		if (parsed == nullptr) {
			TAU_LOG_ERROR << "Parsing failed";
			return;
		}
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected_ctes = std::vector<type_t> {
			bv16_type,
			bv16_type,
			bv16_type
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
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected_ctes = std::vector<type_t> {
			sbf_type,
			sbf_type
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
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected_ctes = std::vector<type_t> {
			sbf_type,
			sbf_type
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
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected_ctes = std::vector<type_t> {
			tau_type,
			tau_type
		};
		CHECK( check_ctes(inferred, expected_ctes) );
	}

	TEST_CASE("complex case: Ohad's example") {
		tref parsed = parse("all x x = y");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", tau_type},
			{"y", tau_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("complex case: Ohad's example (y2)") {
		using node = node_t;
		tref parsed = parse("(all x x = y) && x = y:sbf");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		if (inferred != nullptr) {
			TAU_LOG_TRACE << "Expected nullptr got: " << TAU_LOG_FM_TREE(inferred);
		}
		CHECK( inferred == nullptr );
	}

	TEST_CASE("complex case: shadowing") {
		tref parsed = parse("all x (all x x = 1:sbf)");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", tau_type},
			{"x", sbf_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("complex case: shadowing (y2)") {
		tref parsed = parse("all x ((all x x = 1:sbf) && x = 1:sbf)");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", sbf_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("complex case: shadowing (y3)") {
		tref parsed = parse("all x ((all x x = 1:sbf) && x = 1:tau)");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", tau_type},
			{"x", sbf_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("bv default (y1)") {
		tref parsed = parse("all x x & y =_ z");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", bv_type},
			{"y", bv_type},
			{"z", bv_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("bv default (y2)") {
		tref parsed = parse("all x x & y =_ z && y =_ 1:bv[16]");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", bv16_type},
			{"y", bv16_type},
			{"z", bv16_type}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("mixed sbf and tau") {
		tref parsed = parse("ex x ex y x = 0:tau && xy != 0:sbf");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}
}

TEST_SUITE("new_infer_ba_types: bf formulas") {

	TEST_CASE("simple case 1") {
		tref parsed = parse("x", parse_bf_no_infer());
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", tau_type}
		};
		CHECK( check_vars(inferred, expected) );
	}
}

TEST_SUITE("new_infer_ba_types: cli commands") {

	TEST_CASE("simple case 1") {
		tref parsed = parse("x = 1", parse_cli_no_infer());
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", tau_type}
		};
		CHECK( check_vars(inferred, expected) );
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
