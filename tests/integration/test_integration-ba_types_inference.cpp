// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "test_init.h"
#include "test_tau_helpers.h"
#include "parser_helper.h"

#include "ba_types_inference.h"

TEST_SUITE("Configuration") {

	TEST_CASE("bdd init") {
		bdd_init<Bool>();
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
	auto ref_parents = tau::get(inferred).select_top(is_child<node_t, tau::ref>);
	if (ref_parents.empty() && expected.size() > 0) {
		TAU_LOG_ERROR << "No refs found";
		return false;
	}
	if (ref_parents.size() != expected.size()) {
		TAU_LOG_ERROR << "Expected " << expected.size()
			<< " refs, found " << ref_parents.size();
		return false;
	}
	for (size_t i = 0; i < expected.size(); i++) {
		size_t rtype = tau::get(ref_parents[i])[0].get_type();
		if (rtype != expected[i]) {
			TAU_LOG_ERROR << "Ref '" << ref_parents[i]
				<< "' expected type " << expected[i]
				<< ", found " << rtype;
			return false;
		}
		TAU_LOG_TRACE << "Ref '" << ref_parents[i] << " matched\n";
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

bool check_bf_ctes(tref inferred, std::vector<size_t>& expected) {
	auto is_bf_constant = [](tref n) -> bool {
		using tau = tree<node_t>;

		return tau::get(n).is(tau::bf_t) || tau::get(n).is(tau::bf_f);
	};
	auto ctes = tau::get(inferred).select_top(is_bf_constant);
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
		tref parsed = tau::get("x = 1", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 1 (y2)") {
		tref parsed = tau::get("1 = x", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 2") {
		tref parsed = tau::get("x = 1:sbf", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 2 (y2)") {
		tref parsed = tau::get("1:sbf = x", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 3") {
		tref parsed = tau::get("x = 1:tau", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 3 (y2)") {
		tref parsed = tau::get("1:tau = x", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 4") {
		tref parsed = tau::get("x = { #b1001 }:bv[16]", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv16_type_id<node_t>}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 4 (y2)") {
		// TODO (HIGH) should fail
		// tref parsed = tau::get("x = 1:bv[8]", parse_opts_wff_no_infer);
		tref parsed = tau::get("x = { #b1001 }:bv[8]", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv8_type_id<node_t>}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 4 (y3)") {
		tref parsed = tau::get("x:bv[16] =  { #b1001 }", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<size_t> {
			bv16_type_id<node_t>
		};
		CHECK( check_bv_ctes(inferred, expected) );
	}

	TEST_CASE("simple case 4 (y4)") {
		tref parsed = tau::get("x:bv[16] ={ 1 }", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<size_t> {
			bv16_type_id<node_t>
		};
		CHECK( check_bv_ctes(inferred, expected) );
	}

	TEST_CASE("simple case 7") {
		tref parsed = tau::get("x:sbf = y", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("x = y:sbf", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("x:tau = y", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("x = y:tau", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("x:bv[16] = y", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv16_type_id<node_t>},
			{"y", bv16_type_id<node_t>}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 9 (y2)") {
		tref parsed = tau::get("x = y:bv[16]", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv16_type_id<node_t>},
			{"y", bv16_type_id<node_t>}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 9 (y3)") {
		tref parsed = tau::get("x:bv[8] = y", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("x = y:bv[8]", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("x = y && y = z:tau", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("x:tau = y && y = z", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("x = y && y = z:sbf", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("x:sbf = y && y = z", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("x:bv[16] = y && y = z", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv16_type_id<node_t>},
			{"y", bv16_type_id<node_t>},
			{"z", bv16_type_id<node_t>},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("type chaining (bv[8]) left to right") {
		tref parsed = tau::get("x:bv[8] = y && y = z", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("x = y && y = z:bv[16]", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv16_type_id<node_t>},
			{"y", bv16_type_id<node_t>},
			{"z", bv16_type_id<node_t>},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("type chaining (bv[8]) right to left") {
		tref parsed = tau::get("x = y && y = z:bv[8]", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("all x:bv[16] x = y", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv16_type_id<node_t>},
			{"y", bv16_type_id<node_t>},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all bv (y2)") {
		tref parsed = tau::get("all x x:bv[16] = y", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv16_type_id<node_t>},
			{"y", bv16_type_id<node_t>},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all bv (y3)") {
		tref parsed = tau::get("all x x = y:bv[16]", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv16_type_id<node_t>},
			{"y", bv16_type_id<node_t>},
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("all bv[8]") {
		tref parsed = tau::get("all x:bv[8] x = y", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("all x x:bv[8] = y", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("all x x = y:bv[8]", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("all x:sbf x = y", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("all x x:sbf = y", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("all x x = y:sbf", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("all x:tau x = y", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("all x x:tau = y", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("all x x = y:tau", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("x = { T }:tau", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple explicit tau constant  (y2)") {
		tref parsed = tau::get("{ T }:tau = x", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("x = { x }:sbf", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("{ x }:sbf = x", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple failing case") {
		tref parsed = tau::get("x:tau = x:sbf", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("simple failing case (y2)") {
		tref parsed = tau::get("x:tau = {x}:sbf", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("simple failing case (y3)") {
		tref parsed = tau::get("x:tau = 1:sbf", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("simple failing case (y4)") {
		tref parsed = tau::get("x:tau ={ 1 }", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("simple failing case (y5)") {
		tref parsed = tau::get("x:tau = x:bv[16]", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("bv constants") {
		tref parsed = tau::get("{ #b1 }:bv[16] = { #b0 } :bv[16] & { #b10 }", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected_ctes = std::vector<size_t> {
			bv16_type_id<node_t>,
			bv16_type_id<node_t>,
			bv16_type_id<node_t>
		};
		CHECK( check_bv_ctes(inferred, expected_ctes) );
	}

	TEST_CASE("bv8 constants") {
		tref parsed = tau::get("{ #b1 } : bv[8] = { #b0 } : bv[8] & { #b10 } :bv[8]", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("{ 0 } : sbf = { 1 } { 0 } : sbf", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("{ 0 } = { 1 } & { 0 } : sbf", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("{ F } = { T } { F }", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("all x x = y", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("(all x x = y) && x = y:sbf", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		if (inferred != nullptr) {
			TAU_LOG_TRACE << "Expected nullptr got: " << TAU_LOG_FM_TREE(inferred);
		}
		CHECK( inferred == nullptr );
	}

	TEST_CASE("complex case: shadowing") {
		tref parsed = tau::get("all x (all x x = 1:sbf)", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("all x ((all x x = 1:sbf) && x = 1:tau)", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()},
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("complex case:bv[16] chaining") {
		tref parsed = tau::get("all x x & y = z && y = { 1 }:bv[8]", parse_opts_wff_no_infer);
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
		tref parsed = tau::get("ex x ex y x = 0:tau && xy != 0:sbf", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("bv specizalization") {
		tref parsed = tau::get("ex x :bv[16] x = 1 : bv[8]", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv8_type_id<node_t>},
		};
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
		tref parsed = tau::get("x:tau = y", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_eq>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("sbf bf_eq symbol") {
		tref parsed = tau::get("x:sbf = y", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_eq>(inferred, sbf_type_id<node_t>()) );
	}

	TEST_CASE("bv bf_eq symbol") {
		tref parsed = tau::get("x:bv[16] = y", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_eq>(inferred, bv16_type_id<node_t>) );
	}

	TEST_CASE("bv[8] bf_eq symbol") {
		tref parsed = tau::get("x:bv[8] = y", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_eq>(inferred, bv8_type_id<node_t>) );
	}

	TEST_CASE("tau bf_eq symbol involving constant") {
		tref parsed = tau::get("x:tau = { x = 0 }", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_eq>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_eq symbol involving bf_t") {
		tref parsed = tau::get("x:tau = 1", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_eq>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_eq symbol involving bf_f") {
		tref parsed = tau::get("x:tau = 0", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_eq>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_neq symbol") {
		tref parsed = tau::get("x:tau != 0", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_neq>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_lteq symbol") {
		tref parsed = tau::get("x:tau <= 0", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_lteq>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_nlteq symbol") {
		tref parsed = tau::get("x:tau !<= 0", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_nlteq>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_gt symbol") {
		tref parsed = tau::get("x:tau > 0", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_gt>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_ngt symbol") {
		tref parsed = tau::get("x:tau !> 0", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_ngt>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_gteq symbol") {
		tref parsed = tau::get("x:tau >= 0", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_gteq>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_ngteq symbol") {
		tref parsed = tau::get("x:tau !>= 0", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_ngteq>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_lt symbol") {
		tref parsed = tau::get("x:tau < 0", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_lt>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_nlt symbol") {
		tref parsed = tau::get("x:tau !< 0", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_nlt>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau bf_interval symbol") {
		tref parsed = tau::get("x:tau <= y <= z", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_interval>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("sbf bf_interval symbol") {
		tref parsed = tau::get("x:sbf <= y <= z", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_interval>(inferred, sbf_type_id<node_t>()) );
	}

	TEST_CASE("bv bf_interval symbol") {
		tref parsed = tau::get("x:bv[16] <= y <= z", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_interval>(inferred, bv16_type_id<node_t>) );
	}

	TEST_CASE("tau bf_and symbol") {
		tref parsed = tau::get("x:bv[16] & y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_and>(inferred, bv16_type_id<node_t>) );
	}

	TEST_CASE("tau bf_or symbol") {
		tref parsed = tau::get("x:bv[16] | y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_or>(inferred, bv16_type_id<node_t>) );
	}

	TEST_CASE("tau bf_xor symbol") {
		tref parsed = tau::get("x:bv[16] ^ y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_xor>(inferred, bv16_type_id<node_t>) );
	}

	TEST_CASE("tau bf_neg symbol") {
		tref parsed = tau::get("x:bv[16]'", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_neg>(inferred, bv16_type_id<node_t>) );
	}

	TEST_CASE("bv bf_add symbol") {
		tref parsed = tau::get("x:bv[16] + y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_add>(inferred, bv16_type_id<node_t>) );
	}

	TEST_CASE("bv bf_sub symbol") {
		tref parsed = tau::get("x:bv[16] - y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_sub>(inferred, bv16_type_id<node_t>) );
	}

	TEST_CASE("bv bf_mul symbol") {
		tref parsed = tau::get("x:bv[16] * y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_mul>(inferred, bv16_type_id<node_t>) );
	}

	TEST_CASE("bv bf_div symbol") {
		tref parsed = tau::get("x:bv[16] / y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_div>(inferred, bv16_type_id<node_t>) );
	}

	TEST_CASE("bv bf_mod symbol") {
		tref parsed = tau::get("x:bv[16] % y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_mod>(inferred, bv16_type_id<node_t>) );
	}

	TEST_CASE("bv bf_shr symbol") {
		tref parsed = tau::get("x:bv[16] >> y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_shr>(inferred, bv16_type_id<node_t>) );
	}

	TEST_CASE("bv bf_shl symbol") {
		tref parsed = tau::get("x:bv[16] << y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_shl>(inferred, bv16_type_id<node_t>) );
	}

	TEST_CASE("bv[8] bf_shl symbol") {
		tref parsed = tau::get("x:bv[8] << y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_shl>(inferred, bv8_type_id<node_t>) );
	}

	TEST_CASE("tau bf_shl symbol") {
		tref parsed = tau::get("x:tau << y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("sbf bf_shl symbol") {
		tref parsed = tau::get("x:sbf << y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("sbf xor symbol") {
		tref parsed = tau::get("x:sbf ^ y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_xor>(inferred, sbf_type_id<node_t>()) );
	}

	TEST_CASE("sbf xor symbol (y2)") {
		tref parsed = tau::get("x:sbf ^ 1", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_xor>(inferred, sbf_type_id<node_t>()) );
	}

	TEST_CASE("sbf xor symbol (y3)") {
		tref parsed = tau::get("1 ^ y:sbf", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_xor>(inferred, sbf_type_id<node_t>()) );
	}

	TEST_CASE("tau xor symbol") {
		tref parsed = tau::get("x ^ y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_xor>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau xor symbol (y2)") {
		tref parsed = tau::get("x ^ 1", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_xor>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("tau xor symbol (y3)") {
		tref parsed = tau::get("1 ^ y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_xor>(inferred, tau_type_id<node_t>()) );
	}

	TEST_CASE("bv xor symbol") {
		tref parsed = tau::get("x:bv[16] ^ y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_xor>(inferred, bv16_type_id<node_t>) );
	}

	TEST_CASE("bv xor symbol (y2)") {
		tref parsed = tau::get("x:bv[16] ^ 1", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_xor>(inferred, bv16_type_id<node_t>) );
	}

	TEST_CASE("bv xor symbol (y3)") {
		tref parsed = tau::get("1 ^ y:bv[16]", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( check_symbol<tau::bf_xor>(inferred, bv16_type_id<node_t>) );
	}
}

TEST_SUITE("infer_ba_types: bf formulas") {

	TEST_CASE("simple case 1") {
		tref parsed = tau::get("x", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case 2") {
		tref parsed = tau::get("f[8](x)", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		//DBG(LOG_INFO << "Inferred: " << tau::get(inferred).tree_to_str();)
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}
}

TEST_SUITE("infer_ba_types: cli commands") {

	TEST_CASE("simple case (y1)") {
		tref parsed = tau::get("x = 1", parse_opts_cli_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("simple case (y2)") {
		tref parsed = tau::get("n f(x)", parse_opts_cli_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}
}

TEST_SUITE("infer_ba_types: I/O vars") {

	TEST_CASE("different time stamp propagation") {
		tref parsed = tau::get("i1[t] = o1[t] && i1[t-1] = 1:sbf", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"i1", sbf_type_id<node_t>()},
			{"o1", sbf_type_id<node_t>()},
			{"i1", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}
}

TEST_SUITE("bitvectors") {

	TEST_CASE("Propagation of bv type") {
		//DBG(using node = node_t;)
		tref parsed = tau::get("o1[t]:bv[16] = o1[t-1] + o1[t-2] && o1[0] = { 1 } && o1[1] = { 1 }", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		//DBG(std::cout << "Inferred: " << LOG_FM_TREE(inferred);)
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

TEST_SUITE("infer_ba_types: definitions") {

	TEST_CASE("absent typing") {
		tref parsed = tau::get("g[n](x) := g[n-1](x).", parse_opts_definitions_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"n", untyped_type_id<node_t>()},
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("functional sbf typing: right position") {
		tref parsed = tau::get("g[n](x) := g[n-1](x):sbf.", parse_opts_definitions_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"n", untyped_type_id<node_t>()},
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("functional sbf typing: left position") {
		tref parsed = tau::get("g[n](x):sbf := g[n-1](x).", parse_opts_definitions_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"n", untyped_type_id<node_t>()},
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("predicate sbf: in the body") {
		tref parsed = tau::get("g[n](x) := g[n-1](x:sbf).", parse_opts_definitions_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("predicate sbf: in the head") {
		tref parsed = tau::get("g[n](x:sbf) := g[n-1](x) && T.", parse_opts_definitions_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"n", untyped_type_id<node_t>()},
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("predicate sbf: right position") {
		tref parsed = tau::get("g[n](x:sbf) := g[n-1](x).", parse_opts_definitions_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"n", untyped_type_id<node_t>()},
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("incompatible types") {
		tref parsed = tau::get("g[n](x:tau) := g[n-1](x:sbf).", parse_opts_definitions_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
		DBG(if (inferred) LOG_INFO << "Inferred: " << tau::get(inferred).tree_to_str();)
	}
}

TEST_SUITE("type_inference_options") {

	static auto no_defaults_use = type_inference_options { .use_defaults = false };

	TEST_CASE("use defaults: x >> y") {
		tref parsed = tau::get("x >> y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("no use defaults: x >> y") {
		tref parsed = tau::get("x >> y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed, nullptr, nullptr, no_defaults_use);
		CHECK( inferred == parsed );
	}

	TEST_CASE("use defaults: x << y") {
		tref parsed = tau::get("x << y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("no use defaults: x << y") {
		tref parsed = tau::get("x << y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed, nullptr, nullptr, no_defaults_use);
		CHECK( inferred == parsed );
	}

	TEST_CASE("use defaults: x + y") {
		tref parsed = tau::get("x + y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("no use defaults: x + y") {
		tref parsed = tau::get("x + y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed, nullptr, nullptr, no_defaults_use);
		CHECK( inferred == parsed );
	}

	TEST_CASE("use defaults: x - y") {
		tref parsed = tau::get("x - y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("no use defaults: x - y") {
		tref parsed = tau::get("x - y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed, nullptr, nullptr, no_defaults_use);
		CHECK( inferred == parsed );
	}

	TEST_CASE("use defaults: x % y") {
		tref parsed = tau::get("x % y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("no use defaults: x % y") {
		tref parsed = tau::get("x % y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed, nullptr, nullptr, no_defaults_use);
		CHECK( inferred == parsed );
	}

	TEST_CASE("use defaults: x * y") {
		tref parsed = tau::get("x * y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("no use defaults: x * y") {
		tref parsed = tau::get("x * y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed, nullptr, nullptr, no_defaults_use);
		CHECK( inferred == parsed );
	}

	TEST_CASE("use defaults: x / y") {
		tref parsed = tau::get("x / y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("no use defaults: x / y") {
		tref parsed = tau::get("x / y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed, nullptr, nullptr, no_defaults_use);
		CHECK( inferred == parsed );
	}

	TEST_CASE("use defaults: x !| y") {
		tref parsed = tau::get("x !| y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("no use defaults: x !| y") {
		tref parsed = tau::get("x !| y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed, nullptr, nullptr, no_defaults_use);
		CHECK( inferred == parsed );
	}

	TEST_CASE("use defaults: x !^ y") {
		tref parsed = tau::get("x !^ y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("no use defaults: x !^ y") {
		tref parsed = tau::get("x !^ y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed, nullptr, nullptr, no_defaults_use);
		CHECK( inferred == parsed );
	}

	TEST_CASE("use defaults: x !& y") {
		tref parsed = tau::get("x !& y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("no use defaults: x !& y") {
		tref parsed = tau::get("x !& y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed, nullptr, nullptr, no_defaults_use);
		CHECK( inferred == parsed );
	}

	TEST_CASE("use defaults: x | y") {
		tref parsed = tau::get("x | y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( inferred != parsed );
	}

	TEST_CASE("no use defaults: x | y") {
		tref parsed = tau::get("x | y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed, nullptr, nullptr, no_defaults_use);
		CHECK( inferred == parsed );
	}

	TEST_CASE("use defaults: x ^ y") {
		tref parsed = tau::get("x ^ y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( inferred != parsed );
	}

	TEST_CASE("no use defaults: x ^ y") {
		tref parsed = tau::get("x ^ y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed, nullptr, nullptr, no_defaults_use);
		CHECK( inferred == parsed );
	}

	TEST_CASE("use defaults: x & y") {
		tref parsed = tau::get("x & y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( inferred != parsed );
	}

	TEST_CASE("no use defaults: x & y") {
		tref parsed = tau::get("x & y", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed, nullptr, nullptr, no_defaults_use);
		CHECK( inferred == parsed );
	}

	TEST_CASE("use defaults: y'") {
		tref parsed = tau::get("y'", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( inferred != parsed );
	}

	TEST_CASE("no use defaults: y'") {
		tref parsed = tau::get("y'", parse_opts_bf_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed, nullptr, nullptr, no_defaults_use);
		CHECK( inferred == parsed );
	}

	TEST_CASE("incremental construction of terms in api: success") {
		tref y = tau::get("y:bv[16]", parse_opts_bf_no_infer);
		CHECK( y != nullptr );
		auto inferred_y = infer_ba_types<node_t>(y, nullptr, nullptr, no_defaults_use).first;
		//CHECK ( inferred_y == y);
		tref x = tau::get("x", parse_opts_bf_no_infer);
		CHECK( x != nullptr );
		auto inferred_x = infer_ba_types<node_t>(x, nullptr, nullptr, no_defaults_use).first;
		CHECK( inferred_x == x );
		CHECK( is_buildable<node_t>(tau::bf_and, inferred_x, inferred_y) );
		auto x_and_y = build_bf_and<node_t>(inferred_x, inferred_y);
		auto inferred_x_and_y = infer_ba_types<node_t>(x_and_y, nullptr, nullptr, no_defaults_use).first;
		CHECK( inferred_x_and_y != nullptr);
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", bv16_type_id<node_t>},
			{"y", bv16_type_id<node_t>},
		};
		CHECK( check_vars(inferred_x_and_y, expected) );
	}

	TEST_CASE("incremental construction of terms in api: fail") {
		tref y = tau::get("y:sbf", parse_opts_bf_no_infer);
		CHECK( y != nullptr );
		auto inferred_y = infer_ba_types<node_t>(y, nullptr, nullptr, no_defaults_use).first;
		//CHECK ( inferred_y == y);
		tref x = tau::get("x", parse_opts_bf_no_infer);
		CHECK( x != nullptr );
		auto inferred_x = infer_ba_types<node_t>(x, nullptr, nullptr, no_defaults_use).first;
		CHECK( inferred_x == x );
		CHECK( !is_buildable<node_t>(tau::bf_add, inferred_x, inferred_y) );
	}
}

TEST_SUITE("regression tests") {

	TEST_CASE("satisfiability3/qual_lookback_one_st") {
		tref parsed = tau::get("(G o1[t-1]:bv[16] = { 0 }) && (F o1[t]:bv[16] = { 1 } && o1[t-1]:bv[16] = { 0 })", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"o1", bv16_type_id<node_t>},
			{"o1", bv16_type_id<node_t>},
		};
		CHECK( check_vars(inferred, expected) );
		auto expected_ctes = std::vector<size_t> {
			bv16_type_id<node_t>,
			bv16_type_id<node_t>
		};
		CHECK( check_ctes(inferred, expected_ctes) );
	}

	TEST_CASE("splitter/Tau_splitter_7") {
		tref parsed = tau::get("(F o1[t] = 1)", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"o1", tau_type_id<node_t>()},
		};
		CHECK( check_vars(inferred, expected) );
		auto expected_bf_ctes = std::vector<size_t> {
			tau_type_id<node_t>()
		};
		CHECK( check_bf_ctes(inferred, expected_bf_ctes) );
	}

	TEST_CASE("Lucca's question") {
		tref parsed = tau::get("(ex x:sbf x = 0) && (ex x:tau x = 0)", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", sbf_type_id<node_t>()},
			{"x", tau_type_id<node_t>()},
		};
		CHECK( check_vars(inferred, expected) );
		CHECK( check_vars(inferred, expected) );
		auto expected_bf_ctes = std::vector<size_t> {
			sbf_type_id<node_t>(),
			tau_type_id<node_t>()
		};
		CHECK( check_bf_ctes(inferred, expected_bf_ctes) );
	}

	TEST_CASE("nso_rr_execution/wff_rec_relation: direct substitution") {
		tref parsed = tau::get("g(x):tau fallback 1", parse_opts_cli_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
		auto expected_bf_ctes = std::vector<size_t> {
			tau_type_id<node_t>()
		};
		CHECK( check_bf_ctes(inferred, expected_bf_ctes) );
	}

	TEST_CASE("nso_rr_fixed_point/fallback type mismatch") {
		tref parsed = tau::get("g(x):tau fallback 1:sbf", parse_opts_cli_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("nso_rr_fixed_point/fallback type mismatch (modified)") {
		tref parsed = tau::get("g(x) fallback 1:sbf", parse_opts_cli_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
		auto expected_bf_ctes = std::vector<size_t> {
			sbf_type_id<node_t>()
		};
		CHECK( check_bf_ctes(inferred, expected_bf_ctes) );
	}

	TEST_CASE("nso_rr_execution/wff_rec_relation: direct substitution") {
		tref parsed = tau::get("g(x:tau) fallback T", parse_opts_cli_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("nso_rr_execution/direct substitution, wff_rec_relation case") {
		tref parsed = tau::get("g[0, 0](y:tau) := y."
			"h[0, 0](y:tau) := y."
			"g[0, 0](y:tau) && h[0, 0](1:tau)."
		, parse_opts_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"y", tau_type_id<node_t>()}
		};
		auto expected_bf_ctes = std::vector<size_t> {
			tau_type_id<node_t>()
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("nso_rr_fixed_point/loop fallback T") {
		tref parsed = tau::get("g(x) fallback T", parse_opts_cli_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", tau_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("Lucca's constant parsing example (y1)") {
		tref parsed = tau::get("x = {1}:bv[16] && x = y:bv[32]", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("Lucca's constant parsing example (y2)") {
		tref parsed = tau::get("x = 1 && x = y:sbf", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("Andrei's example") {
		tref parsed = tau::get("G u[t]:tau = i0[t]:tau && (o2[t]:bv[16] = 0 || i2[t]:bv[16] !< i1[t]:bv[16]) && (o2[t]:bv[16] = { 1 }:bv[16] || i2[t]:bv[16] < i1[t]:bv[16]) && (i3[t]:bv[16] != i4[t]:bv[16] || o3[t]:bv[16] = 0) && (o3[t]:bv[16] = { 1 }:bv[16] || i3[t]:bv[16] = i4[t]:bv[16]) && (i1[t]:bv[16] != 0 || o4[t]:bv[16] = 0) && (o4[t]:bv[16] = { 1 }:bv[16] || i1[t]:bv[16] = 0) && o1[t]:bv[16] = i1[t]:bv[16] && (i0[t]:bv[16]|i1[t]:bv[16] != 0 || i1[t]:bv[16] = 0)", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("improper function type inference in formula (y3)") {
		tref parsed = tau::get("all x:sbf f(x:tau) = 1", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("improper function type inference in cli") {
		tref parsed = tau::get("f(x):sbf", parse_opts_cli_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
	}

	TEST_CASE("improper function type inference in formula (y1)") {
		tref parsed = tau::get("f(x):sbf = 1", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
		auto expected_bf_ctes = std::vector<size_t> {
			sbf_type_id<node_t>()
		};
		CHECK( check_bf_ctes(inferred, expected_bf_ctes) );
	}

	TEST_CASE("improper function type inference in formula (y2)") {
		tref parsed = tau::get("all x:sbf f(x):tau = 1", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("improper function type inference in formula (y4)") {
		tref parsed = tau::get("f(x:tau):sbf = 1", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("Issue 52 (y1)") {
		// Original test used bare :bv which unified with :bv[15].
		// With mandatory bitwidths, conflicting explicit sizes are a type error.
		tref parsed = tau::get("x:bv[16] = {1}:bv[15]", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("Issue 52 (y2)") {
		// Original test used bare :bv which unified with :bv[15].
		// With mandatory bitwidths, conflicting explicit sizes are a type error.
		tref parsed = tau::get("x:bv[15] = {1}:bv[16]", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("Issue 52 (y3)") {
		// Original test used bare :bv which unified with :bv[15].
		// With mandatory bitwidths, conflicting explicit sizes are a type error.
		tref parsed = tau::get("x:bv[15] = 1:bv[16]", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred == nullptr );
	}

	TEST_CASE("Andrei's question (y1)") {
		tref parsed = tau::get("o11[t]:bv[64] = i3[t]:bv[64]|i4[t]:bv[64]", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
	}

	TEST_CASE("Andrei's question (y2)") {
		tref parsed = tau::get("G (i0[t]:tau = 0 || o0[t]:tau = 0 && u[t]:tau = i0[t]:tau) && (i0[t]:tau != 0 || o0[t]:tau' = 0) && i1[t]:bv[64] = o1[t]:bv[64] && (o2[t]:bv[1] = 0 || i2[t]:bv[64] !< o1[t]:bv[64]) && (o2[t]:bv[1]' = 0 || i2[t]:bv[64] < o1[t]:bv[64]) && (o1[t]:bv[64] != 0 || o4[t]:bv[1] = 0) && (o4[t]:bv[1]' = 0 || o1[t]:bv[64] = 0) && (i3[t]:bv[64] != i4[t]:bv[64] || o3[t]:bv[1] = 0) && (o3[t]:bv[1]' = 0 || i3[t]:bv[64] = i4[t]:bv[64]) && o11[t]:bv[64] = 0 && (i4[t]:bv[64]|i3[t]:bv[64] = 0 || o11[t]:bv[64] = i3[t]:bv[64]|i4[t]:bv[64]) && (i4[t]:bv[64]|i3[t]:bv[64] != 0 || o11[t]:bv[64] = i4[t]:bv[64]'i3[t]:bv[64]') && (i4[t]:bv[64]|i3[t]:bv[64] = 0 || o11[t]:bv[64] = i3[t]:bv[64]|i4[t]:bv[64]) && (i4[t]:bv[64]|i3[t]:bv[64] != 0 || o11[t]:bv[64] = i4[t]:bv[64]'i3[t]:bv[64]') && o12[t]:bv[64] = { 1 }:bv[64] && o10[t]:bv[64] = 0 && (i1[t]:bv[64]&({ 170 }:bv[64]|i3[t]:bv[64])|i1[t]:bv[64]'i2[t]:bv[64]&({ 170 }:bv[64]|i3[t]:bv[64]) = 0 || o8[t]:bv[64] = i1[t]:bv[64]&(i3[t]:bv[64]|{ 170 }:bv[64])|i1[t]:bv[64]'i2[t]:bv[64]&(i3[t]:bv[64]|{ 170 }:bv[64])) && (i1[t]:bv[64]&({ 170 }:bv[64]|i3[t]:bv[64])|i1[t]:bv[64]'i2[t]:bv[64]&({ 170 }:bv[64]|i3[t]:bv[64]) != 0 || o8[t]:bv[64] = { 18446744073709551445 }:bv[64] i1[t]:bv[64]i3[t]:bv[64]'|i1[t]:bv[64]'&({ 18446744073709551445 }:bv[64] i3[t]:bv[64]'|i2[t]:bv[64]'))", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
	}
}

TEST_SUITE("typed annotations as structural children") {

	// Helper: check no node in tree has a typed structural child
	static bool no_typed_structural_children(tref root) {
		bool clean = true;
		auto f = [&](tref n) {
			for (auto c : tau::get(n).get_children())
				if (tau::get(c).is(tau::typed)) { clean = false; return false; }
			return clean;
		};
		pre_order<node_t>(root).search_unique(f);
		return clean;
	}

	TEST_CASE("before inference: variable with :sbf has typed structural child") {
		tref parsed = tau::get("x:sbf = 1", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto vars = tau::get(parsed).select_top(is<node_t, tau::variable>);
		CHECK( vars.size() == 1 );
		bool has_typed = false;
		for (auto c : tau::get(vars[0]).get_children())
			if (tau::get(c).is(tau::typed)) { has_typed = true; break; }
		CHECK( has_typed );
		// And ba_type must be 0 (not pre-stamped)
		CHECK( tau::get(vars[0]).get_ba_type() == 0 );
	}

	TEST_CASE("after inference: variable with :sbf has ba_type stamped, no typed child") {
		tref parsed = tau::get("x:sbf = 1", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto vars = tau::get(inferred).select_top(is<node_t, tau::variable>);
		CHECK( vars.size() == 1 );
		CHECK( tau::get(vars[0]).get_ba_type() == sbf_type_id<node_t>() );
		CHECK( no_typed_structural_children(inferred) );
	}

	TEST_CASE("before inference: bf_t with :sbf has typed structural child") {
		tref parsed = tau::get("1:sbf = x", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto bfts = tau::get(parsed).select_top(is<node_t, tau::bf_t>);
		CHECK( bfts.size() == 1 );
		bool has_typed = false;
		for (auto c : tau::get(bfts[0]).get_children())
			if (tau::get(c).is(tau::typed)) { has_typed = true; break; }
		CHECK( has_typed );
		CHECK( tau::get(bfts[0]).get_ba_type() == 0 );
	}

	TEST_CASE("after inference: bf_t with :sbf has ba_type stamped, no typed child") {
		tref parsed = tau::get("1:sbf = x", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto bfts = tau::get(inferred).select_top(is<node_t, tau::bf_t>);
		CHECK( bfts.size() == 1 );
		CHECK( tau::get(bfts[0]).get_ba_type() == sbf_type_id<node_t>() );
		CHECK( no_typed_structural_children(inferred) );
	}

	TEST_CASE("after inference: bf_f with :tau has ba_type stamped, no typed child") {
		tref parsed = tau::get("0:tau = x", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto bffs = tau::get(inferred).select_top(is<node_t, tau::bf_f>);
		CHECK( bffs.size() == 1 );
		CHECK( tau::get(bffs[0]).get_ba_type() == tau_type_id<node_t>() );
		CHECK( no_typed_structural_children(inferred) );
	}

	TEST_CASE("after inference: type annotation propagates from bf_t to variable") {
		tref parsed = tau::get("x = 1:sbf", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto expected = std::vector<std::pair<std::string, size_t>> {
			{"x", sbf_type_id<node_t>()}
		};
		CHECK( check_vars(inferred, expected) );
		CHECK( no_typed_structural_children(inferred) );
	}

	TEST_CASE("after inference: type annotation propagates from variable to bf_f") {
		tref parsed = tau::get("x:tau = 0", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		auto bffs = tau::get(inferred).select_top(is<node_t, tau::bf_f>);
		CHECK( bffs.size() == 1 );
		CHECK( tau::get(bffs[0]).get_ba_type() == tau_type_id<node_t>() );
		CHECK( no_typed_structural_children(inferred) );
	}

	TEST_CASE("no typed children remain anywhere after inference on complex formula") {
		tref parsed = tau::get("x:bv[16] = y && z:sbf = 1", parse_opts_wff_no_infer);
		CHECK( parsed != nullptr );
		auto [inferred, _] = infer_ba_types<node_t>(parsed);
		CHECK( inferred != nullptr );
		CHECK( no_typed_structural_children(inferred) );
	}
}

TEST_SUITE("Cleanup") {

	TEST_CASE("ba_constants cleanup") {
		ba_constants<node_t>::cleanup();
	}
}
