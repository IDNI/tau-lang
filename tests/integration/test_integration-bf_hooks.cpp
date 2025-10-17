// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "test_init.h"
#include "test_tau_helpers.h"

TEST_SUITE("configuration") {

	TEST_CASE("bdd_init") {
		bdd_init<Bool>();
	}
}

TEST_SUITE("bf operator hooks") {

	// we should get an error during parsing and hence return true if we get an error
	bool check_unbound_hook(const char* sample) {
		tref tau_sample = tau::get(sample, parse_bf());

#ifdef DEBUG
		using node = node_t;
		string str(sample);
		if (tau_sample) cout << "sample: " << str
			<< " expected error, got : " << TAU_LOG_FM_DUMP(tau_sample) << "\n";
		else cout << "sample: " << str << " expected error, got it\n";
#endif // DEBUG

		return tau_sample == nullptr;
	}

	// we should be able to parse the sample and the expected result should be the same
	bool check_hook(const char* sample_, const char* expected_) {
		TAU_LOG_TRACE << "===== sample =====";
		auto sample = string(sample_) + " = 0.";
		tref tau_sample   = tau::get(sample);
		TAU_LOG_TRACE << "===== expected =====";
		auto expected = string(expected_) + " = 0.";
		tref tau_expected = tau::get(expected);

#ifdef DEBUG
		using node = node_t;
		cout << "sample: " << sample << "\nexpected: \t";
		if (tau_expected == 0) cout << "nullptr";
		else cout << TAU_DUMP_TO_STR(tau_expected);
		cout << "\ngot:      \t";
		if (tau_sample == 0) cout << "nullptr";
		else cout << TAU_DUMP_TO_STR(tau_sample);
		cout << "\n";
#endif // DEBUG

		return tau::subtree_equals(tau_sample, tau_expected);
	}

	bool check_type(const char* sample, const tref type) {
		tref parsed = tau::get(std::string(sample) + " = 0.", { .reget_with_hooks = false });
		using node = node_t;
		// DBG(TAU_LOG_TRACE << "parsed: " << TAU_LOG_FM_DUMP(parsed);)
		tref c = tau::get(parsed).find_top(is<node, tau::ba_constant>);
		size_t type_id = tau::get(c).get_ba_type();
		size_t type_expected_id = get_ba_type_id<node_t>(type);
		auto sample_type = get_ba_type_name<node_t>(type_id);
		auto expected_type = get_ba_type_name<node_t>(type_expected_id);

#ifdef DEBUG
		string str(sample);
		if (c) cout << "sample: " << str << " expected type: "
			<< expected_type << " got: " << sample_type << "\n";
		else cout << "sample: " << str << " expected type: "
				<< expected_type << " got: tau\n";
#endif // DEBUG

		return type_id == type_expected_id && sample_type == expected_type;
	}

	TEST_CASE("conversion to 1/0") {
		CHECK( check_hook("{T}", "1:tau") );
		CHECK( check_hook("{F}", "0:tau") );

		CHECK( check_hook("{T}:tau", "1:tau") );
		CHECK( check_hook("{F}:tau", "0:tau") );

		CHECK( check_hook("{1}:sbf", "1:sbf") );
		CHECK( check_hook("{0}:sbf", "0:sbf") );
	}

	TEST_CASE("'") {
		CHECK( check_hook("0'", "1") );
		CHECK( check_hook("1'", "0") );

		CHECK( check_hook("0:sbf'", "1:sbf") );
		CHECK( check_hook("1:sbf'", "0:sbf") );

		CHECK( check_hook("0:tau'", "1:tau") );
		CHECK( check_hook("1:tau'", "0:tau") );

		CHECK( check_hook("x''", "x") );

		CHECK( check_type("{a}:sbf'", sbf_type<node_t>()) );
		CHECK( check_type("{o1[t] = 0}'",  tau_type<node_t>()) );
		CHECK( check_type("{o1[t] = 0}:tau'", tau_type<node_t>()) );
	}

	TEST_CASE("|") {
		CHECK( check_hook("1|0", "1") );
		CHECK( check_hook("0|1", "1") );
		CHECK( check_hook("1|1", "1") );
		CHECK( check_hook("0|0", "0") );

		CHECK( check_hook("1:sbf|0:sbf", "1:sbf") );
		CHECK( check_hook("0:sbf|1:sbf", "1:sbf") );
		CHECK( check_hook("1:sbf|1:sbf", "1:sbf") );
		CHECK( check_hook("0:sbf|0:sbf", "0:sbf") );

		CHECK( check_hook("1:sbf|0", "1:sbf") );
		CHECK( check_hook("0:sbf|1", "1:sbf") );
		CHECK( check_hook("1:sbf|1", "1:sbf") );
		CHECK( check_hook("0:sbf|0", "0:sbf") );

		CHECK( check_hook("1|0:sbf", "1:sbf") );
		CHECK( check_hook("0|1:sbf", "1:sbf") );
		CHECK( check_hook("1|1:sbf", "1:sbf") );
		CHECK( check_hook("0|0:sbf", "0:sbf") );

		CHECK( check_hook("1:tau|0:tau", "1:tau") );
		CHECK( check_hook("0:tau|1:tau", "1:tau") );
		CHECK( check_hook("1:tau|1:tau", "1:tau") );
		CHECK( check_hook("0:tau|0:tau", "0:tau") );

		CHECK( check_hook("1|0:tau", "1:tau") );
		CHECK( check_hook("0|1:tau", "1:tau") );
		CHECK( check_hook("1|1:tau", "1:tau") );
		CHECK( check_hook("0|0:tau", "0:tau") );

		CHECK( check_hook("1:tau|0", "1:tau") );
		CHECK( check_hook("0:tau|1", "1:tau") );
		CHECK( check_hook("1:tau|1", "1:tau") );
		CHECK( check_hook("0:tau|0", "0:tau") );

		CHECK( !check_hook("1:sbf|0:tau", "1") );
		CHECK( !check_hook("0:sbf|1:tau", "1") );
		CHECK( !check_hook("1:sbf|1:tau", "1") );
		CHECK( !check_hook("0:sbf|0:tau", "0") );

		CHECK( !check_hook("1:tau|0:sbf", "1") );
		CHECK( !check_hook("0:tau|1:sbf", "1") );
		CHECK( !check_hook("1:tau|1:sbf", "1") );
		CHECK( !check_hook("0:tau|0:sbf", "0") );

		CHECK( !check_hook("{1}:sbf|0:tau", "1") );
		CHECK( !check_hook("{0}:sbf|1:tau", "1") );
		CHECK( !check_hook("{1}:sbf|1:tau", "1") );
		CHECK( !check_hook("{0}:sbf|0:tau", "0") );

		CHECK( !check_hook("1:tau|{0}:sbf", "1") );
		CHECK( !check_hook("0:tau|{1}:sbf", "1") );
		CHECK( !check_hook("1:tau|{1}:sbf", "1") );
		CHECK( !check_hook("0:tau|{0}:sbf", "0") );

		CHECK( !check_hook("1:sbf|{F}:tau", "1") );
		CHECK( !check_hook("0:sbf|{T}:tau", "1") );
		CHECK( !check_hook("1:sbf|{T}:tau", "1") );
		CHECK( !check_hook("0:sbf|{F}:tau", "0") );

		CHECK( !check_hook("{T}:tau|1:sbf", "1") );
		CHECK( !check_hook("{F}:tau|0:sbf", "0") );
		CHECK( !check_hook("{T}:tau|1:sbf", "1") );
		CHECK( !check_hook("{F}:tau|0:sbf", "0") );

		CHECK( check_hook("1|x", "1") );
		CHECK( check_hook("x|1", "1") );
		CHECK( check_hook("0|x", "x") );
		CHECK( check_hook("x|0", "x") );
		CHECK( check_hook("x|x", "x") );
		CHECK( check_hook("x|x'", "1") );
		CHECK( check_hook("x'|x", "1") );

		CHECK( check_type("{a}:sbf|{b}:sbf", sbf_type<node_t>()) );
		CHECK( check_type("{o1[t] = 0}|{o2[t] = 0}", tau_type<node_t>()) );
		CHECK( check_type("{o1[t] = 0}:tau|{o2[t] = 0}", tau_type<node_t>()) );
		CHECK( check_type("{o1[t] = 0}|{o2[t] = 0}:tau", tau_type<node_t>()) );

	}

	TEST_CASE("&") {
		CHECK( check_hook("0&1", "0") );
		CHECK( check_hook("1&0", "0") );
		CHECK( check_hook("1&1", "1") );
		CHECK( check_hook("0&0", "0") );

		CHECK( check_hook("1:sbf&0:sbf", "0:sbf") );
		CHECK( check_hook("0:sbf&1:sbf", "0:sbf") );
		CHECK( check_hook("1:sbf&1:sbf", "1:sbf") );
		CHECK( check_hook("0:sbf&0:sbf", "0:sbf") );

		CHECK( check_hook("1:sbf&0", "0:sbf") );
		CHECK( check_hook("0:sbf&1", "0:sbf") );
		CHECK( check_hook("1:sbf&1", "1:sbf") );
		CHECK( check_hook("0:sbf&0", "0:sbf") );

		CHECK( check_hook("1&0:sbf", "0:sbf") );
		CHECK( check_hook("0&1:sbf", "0:sbf") );
		CHECK( check_hook("1&1:sbf", "1:sbf") );
		CHECK( check_hook("0&0:sbf", "0:sbf") );

		CHECK( check_hook("1:tau&0:tau", "0:tau") );
		CHECK( check_hook("0:tau&1:tau", "0:tau") );
		CHECK( check_hook("1:tau&1:tau", "1:tau") );
		CHECK( check_hook("0:tau&0:tau", "0:tau") );

		CHECK( check_hook("1&0:tau", "0:tau") );
		CHECK( check_hook("0&1:tau", "0:tau") );
		CHECK( check_hook("1&1:tau", "1:tau") );
		CHECK( check_hook("0&0:tau", "0:tau") );

		CHECK( check_hook("1:tau&0", "0:tau") );
		CHECK( check_hook("0:tau&1", "0:tau") );
		CHECK( check_hook("1:tau&1", "1:tau") );
		CHECK( check_hook("0:tau&0", "0:tau") );

		CHECK( !check_hook("1:sbf&0:tau", "0") );
		CHECK( !check_hook("0:sbf&1:tau", "0") );
		CHECK( !check_hook("1:sbf&1:tau", "1") );
		CHECK( !check_hook("0:sbf&0:tau", "0") );

		CHECK( !check_hook("1:tau&0:sbf", "0") );
		CHECK( !check_hook("0:tau&1:sbf", "0") );
		CHECK( !check_hook("1:tau&1:sbf", "1") );
		CHECK( !check_hook("0:tau&0:sbf", "0") );

		CHECK( !check_hook("{1}:sbf&0:tau", "0") );
		CHECK( !check_hook("{0}:sbf&1:tau", "0") );
		CHECK( !check_hook("{1}:sbf&1:tau", "1") );
		CHECK( !check_hook("{0}:sbf&0:tau", "0") );

		CHECK( !check_hook("1:tau&{0}:sbf", "0") );
		CHECK( !check_hook("0:tau&{1}:sbf", "0") );
		CHECK( !check_hook("1:tau&{1}:sbf", "1") );
		CHECK( !check_hook("0:tau&{0}:sbf", "0") );

		CHECK( !check_hook("1:sbf&{F}:tau", "0") );
		CHECK( !check_hook("0:sbf&{T}:tau", "0") );
		CHECK( !check_hook("1:sbf&{T}:tau", "1") );
		CHECK( !check_hook("0:sbf&{F}:tau", "0") );

		CHECK( !check_hook("{T}:tau&0:sbf", "0") );
		CHECK( !check_hook("{F}:tau&1:sbf", "0") );
		CHECK( !check_hook("{T}:tau&1:sbf", "1") );
		CHECK( !check_hook("{F}:tau&0:sbf", "0") );

		CHECK( check_hook("1&x", "x") );
		CHECK( check_hook("x&1", "x") );
		CHECK( check_hook("0&x", "0") );
		CHECK( check_hook("x&0", "0") );
		CHECK( check_hook("x&x", "x") );
		CHECK( check_hook("x&x'", "0") );
		CHECK( check_hook("x'&x", "0") );

		CHECK( check_type("{a}:sbf&{b}:sbf", sbf_type<node_t>()) );
		CHECK( check_type("{o1[t] = 0}&{o2[t] = 0}", tau_type<node_t>()) );
		CHECK( check_type("{o1[t] = 0}:tau&{o2[t] = 0}", tau_type<node_t>()) );
		CHECK( check_type("{o1[t] = 0}&{o2[t] = 0}:tau", tau_type<node_t>()) );
	}

	TEST_CASE("^") {
		CHECK( check_hook("0^1", "1") );
		CHECK( check_hook("1^0", "1") );
		CHECK( check_hook("1^1", "0") );
		CHECK( check_hook("0^0", "0") );

		CHECK( check_hook("1:sbf^0:sbf", "1:sbf") );
		CHECK( check_hook("0:sbf^1:sbf", "1:sbf") );
		CHECK( check_hook("1:sbf^1:sbf", "0:sbf") );
		CHECK( check_hook("0:sbf^0:sbf", "0:sbf") );

		CHECK( check_hook("1:sbf^0", "1:sbf") );
		CHECK( check_hook("0:sbf^1", "1:sbf") );
		CHECK( check_hook("1:sbf^1", "0:sbf") );
		CHECK( check_hook("0:sbf^0", "0:sbf") );

		CHECK( check_hook("1^0:sbf", "1:sbf") );
		CHECK( check_hook("0^1:sbf", "1:sbf") );
		CHECK( check_hook("1^1:sbf", "0:sbf") );
		CHECK( check_hook("0^0:sbf", "0:sbf") );

		CHECK( check_hook("1:tau^0:tau", "1:tau") );
		CHECK( check_hook("0:tau^1:tau", "1:tau") );
		CHECK( check_hook("1:tau^1:tau", "0:tau") );
		CHECK( check_hook("0:tau^0:tau", "0:tau") );

		CHECK( check_hook("1^0:tau", "1:tau") );
		CHECK( check_hook("0^1:tau", "1:tau") );
		CHECK( check_hook("1^1:tau", "0:tau") );
		CHECK( check_hook("0^0:tau", "0:tau") );

		CHECK( check_hook("1:tau^0", "1:tau") );
		CHECK( check_hook("0:tau^1", "1:tau") );
		CHECK( check_hook("1:tau^1", "0:tau") );
		CHECK( check_hook("0:tau^0", "0:tau") );

		CHECK( !check_hook("1:sbf^0:tau", "1") );
		CHECK( !check_hook("0:sbf^1:tau", "1") );
		CHECK( !check_hook("1:sbf^1:tau", "0") );
		CHECK( !check_hook("0:sbf^0:tau", "0") );

		CHECK( !check_hook("1:tau^0:sbf", "1") );
		CHECK( !check_hook("0:tau^1:sbf", "1") );
		CHECK( !check_hook("1:tau^1:sbf", "0") );
		CHECK( !check_hook("0:tau^0:sbf", "0") );

		CHECK( !check_hook("{1}:sbf^0:tau", "1") );
		CHECK( !check_hook("{0}:sbf^1:tau", "1") );
		CHECK( !check_hook("{1}:sbf^1:tau", "0") );
		CHECK( !check_hook("{0}:sbf^0:tau", "0") );

		CHECK( !check_hook("1:tau^{0}:sbf", "1") );
		CHECK( !check_hook("0:tau^{1}:sbf", "1") );
		CHECK( !check_hook("1:tau^{1}:sbf", "0") );
		CHECK( !check_hook("0:tau^{0}:sbf", "0") );

		CHECK( !check_hook("1:sbf^{F}:tau", "1") );
		CHECK( !check_hook("0:sbf^{T}:tau", "1") );
		CHECK( !check_hook("1:sbf^{T}:tau", "0") );
		CHECK( !check_hook("0:sbf^{F}:tau", "0") );

		CHECK( !check_hook("{T}:tau^0:sbf", "1") );
		CHECK( !check_hook("{F}:tau^1:sbf", "1") );
		CHECK( !check_hook("{T}:tau^1:sbf", "0") );
		CHECK( !check_hook("{F}:tau^0:sbf", "0") );

		CHECK( check_hook("1^x", "x'") );
		CHECK( check_hook("x^1", "x'") );
		CHECK( check_hook("0^x", "x") );
		CHECK( check_hook("x^0", "x") );
		CHECK( check_hook("x^x", "0") );
		CHECK( check_hook("x^x'", "1") );
		CHECK( check_hook("x'^x", "1") );

		CHECK( check_type("{a}:sbf^{b}:sbf", sbf_type<node_t>()) );
		CHECK( check_type("{o1[t] = 0}^{o2[t] = 0}", tau_type<node_t>()) );
		CHECK( check_type("{o1[t] = 0}:tau^{o2[t] = 0}", tau_type<node_t>()) );
		CHECK( check_type("{o1[t] = 0}^{o2[t] = 0}:tau", tau_type<node_t>()) );
	}
}
