// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Standalone formula evaluator for scripted fuzz testing.
// Usage: tau_eval "<formula string ending in .>"
// Prints: REALIZABLE, UNREALIZABLE, or PARSE_ERROR
// Exit codes: 0=REALIZABLE, 1=UNREALIZABLE, 2=PARSE_ERROR

#define bas_pack tau_ba<qint, qlt, nlang_ba, bv, sbf_ba, hsb>, qint, qlt, nlang_ba, bv, sbf_ba, hsb

#include "boolean_algebras/qint.h"
#include "boolean_algebras/qlt.h"
#include "boolean_algebras/nlang_ba.h"
#include "boolean_algebras/hsb.h"
#include "test_helpers.h"

#include <boost/log/core.hpp>
#include <iostream>
#include <string>
#include <regex>

using namespace idni::tau_lang;

using node_t = tau_lang::node<bas_pack>;
using tau    = tree<node_t>;

#include "parse_error_hint.h"
using idni::tau_lang::classify_parse_error;

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cerr << "usage: tau_eval <formula>\n";
		return 2;
	}

	// Suppress tau's logging output (logging writes to cout; disable the core
	// so no error/warning messages mix with our REALIZABLE/UNREALIZABLE result)
	logging::error();
	boost::log::core::get()->set_logging_enabled(false);

	std::string formula(argv[1]);

	// Parse at the result level first so we can extract error details.
	auto parse_result = tau_parser::instance().parse(
		formula.c_str(), formula.size());

	if (!parse_result.found) {
		std::cout << "PARSE_ERROR\n";
		// Write diagnostic to stderr: parser's own message + our hint.
		std::string parser_msg = parse_result.parse_error
			.to_str(tau_parser::error::info_lvl::INFO_BASIC);
		// Strip trailing newline for cleaner output.
		while (!parser_msg.empty() && parser_msg.back() == '\n')
			parser_msg.pop_back();
		std::string hint = classify_parse_error(formula);
		std::cerr << "parse error: " << parser_msg;
		if (!hint.empty()) std::cerr << "\nhint: " << hint;
		std::cerr << "\n";
		return 2;
	}

	auto nso = get_nso_rr<node_t>(tau::get(formula.c_str()));
	if (!nso.has_value()) {
		std::string hint = classify_parse_error(formula);
		std::cout << "PARSE_ERROR\n";
		std::cerr << "parse error: formula parsed but could not be resolved";
		if (!hint.empty()) std::cerr << "\nhint: " << hint;
		std::cerr << "\n";
		return 2;
	}
	tref fm = nso.value().main->get();
	if (!fm) {
		std::cout << "PARSE_ERROR\n";
		std::cerr << "parse error: empty formula\n";
		return 2;
	}
	bool result = is_tau_formula_sat<node_t>(fm);
	std::cout << (result ? "REALIZABLE" : "UNREALIZABLE") << "\n";
	return result ? 0 : 1;
}
