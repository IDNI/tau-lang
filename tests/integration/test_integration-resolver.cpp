// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_tau_helpers.h"

#include "resolver.h"

TEST_SUITE("Configuration") {
	TEST_CASE("logging") {
		logging::trace();
	}

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("new_infer_ba_types") {

	tref parse(const std::string& sample) {
		auto opts = tau::get_options{
			.parse = { .start = tau::wff },
			.infer_ba_types = false,
			.reget_with_hooks = false
		};
		auto src = tree<node_t>::get(sample, opts);
		if (src == nullptr) {
			TAU_LOG_ERROR << "Parsing failed for: " << sample;
		}
		return src;
	}

	bool check_vars(tref inferred, std::vector<std::pair<std::string, type_t>>& expected) {
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
		auto ctes = tau::get(inferred).select_top(is<node_t, tau::bf_constant>);
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

	/*TEST_CASE("simple case 1") {
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
	}*/







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

	/*TEST_CASE("all bv") {
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

	TEST_CASE("ex bv") {
		tref parsed = parse("ex x:bv x =_ y");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", bv_type},
			{"y", bv_type},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("ex bv[16]") {
		tref parsed = parse("ex x:bv[16] x =_ y");
		CHECK( parsed != nullptr );
		tref inferred = new_infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, type_t>> {
			{"x", bv16_type},
			{"y", bv16_type},
		};
		CHECK( check_vars(inferred, expected) );
	}*/














	/*TEST_CASE("all typed") {
		using node = node_t;
		auto n = infer("{ 0 } : sbf = { 1 } : sbf & { 0 } : sbf.");
		LOG_DEBUG << "inferred: " << LOG_FM(n);
		CHECK( are_all_typed_as(n, "sbf") );
	}

	TEST_CASE("all typed (y2)") {
		using node = node_t;
		auto n = infer("x : bv[16] =_ y : bv[16].");
		LOG_DEBUG << "inferred: " << LOG_FM(n);
		CHECK( are_all_typed_as(n, "bv") );
	}

	TEST_CASE("all typed (y3)") {
		using node = node_t;
		auto n = infer("#b1 : bv[16] =_ #b0 : bv[16] & #b1 : bv.");
		LOG_DEBUG << "inferred: " << LOG_FM(n);
		CHECK( are_all_typed_as(n, "bv") );
	}

	TEST_CASE("some typed") {
		using node = node_t;
		auto n = infer("{ 0 } : sbf = { 1 } { 0 } : sbf.");
		LOG_DEBUG << "inferred: " << LOG_FM(n);
		CHECK( are_all_typed_as(n, "sbf") );
	}

	TEST_CASE("some typed (y2)") {
		using node = node_t;
		auto n = infer("#b1 : bv =_ #b0 & #b1.");
		LOG_DEBUG << "inferred: " << LOG_FM(n);
		CHECK( are_all_typed_as(n, "bv") );
	}

	TEST_CASE("only 1 typed") {
		auto n = infer("{ 0 } = { 1 } & { 0 } : sbf.");
		CHECK( are_all_typed_as(n, "sbf") );
	}

	TEST_CASE("none typed") {
		auto n = infer("{ F } = { T } { F }.");
		CHECK( are_all_typed_as(n, "tau") );
	}

	TEST_CASE("type over many variables beyond atomic formulas") {
		auto n = infer("x = y && y = z && z = w && w = { x1 }:sbf.");
		CHECK( are_all_typed_as(n, "sbf") );
	}

	TEST_CASE("type over many variables beyond atomic formulas (y2)") {
		auto n = infer("x =_ y && y =_ z && z =_ w : bv.");
		CHECK( are_all_typed_as(n, "bv") );
	}

	TEST_CASE("type over many variables beyond atomic formulas (y3)") {
		auto n = infer("x =_ y && y =_ z && z =_ w : bv[16].");
		CHECK( are_all_typed_as(n, "bv") );
	}

	TEST_CASE("type mismatch head vs body") {
		auto n = expect_infer_fail("{ T }:tau = { 0 }:sbf { 1 }:sbf.");
		CHECK( n );
	}

	TEST_CASE("type mismatch in body") {
		auto n = expect_infer_fail("{ 0 }:sbf = { T }:tau { 1 }:sbf.");
		CHECK( n );
	}

	TEST_CASE("type mismatch of default type in head vs body") {
		auto n = expect_infer_fail("{ T }:tau = { 0 }:sbf { 1 }.");
		CHECK( n );
	}

	TEST_CASE("type mismatch of default type in body") {
		auto n = expect_infer_fail("{ 0 } = { T }:tau { 1 }:sbf { 1 }:sbf.");
		CHECK( n );
	}*/

















	/*TEST_CASE("simple explicit tau constant") {
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
	}*/
}