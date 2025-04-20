// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <cassert>

#include "doctest.h"

#ifndef __TEST_INTEGRATION_HELPERS_SBF_H__
#define __TEST_INTEGRATION_HELPERS_SBF_H__

#include "depreciating/boolean_algebras/sbf_ba_depreciating.h"
#include "boolean_algebras/bool_ba.h"
#include "depreciating/nso_rr_depreciating.h"
#include "boolean_algebras/bdds/babdd.h"
#include "boolean_algebras/bdds/bdd_handle.h"
#include "depreciating/normalizer_depreciating.h"
#include "boolean_algebras/variant_ba.h"
#include "depreciating/boolean_algebras/nso_ba_depreciating.h"
#include "dict.h"

using namespace std;
using namespace idni::tau_lang_depreciating;
using namespace idni::rewriter::depreciating;

namespace testing = doctest;

namespace idni::tau_lang_depreciating {

rr<tau_<sbf_ba>> sbf_make_nso_rr(const std::string& src) {
	auto sample_src = make_tau_source(src);
	return make_nso_rr_using_factory<sbf_ba>(sample_src).value();
}

tau_<sbf_ba> sbf_make_nso(const std::string& src) {
	return sbf_make_nso_rr(src).main;
}

rr<tau_<tau_ba<sbf_ba>, sbf_ba>> tau_make_nso_rr_test(const std::string& src) {
	auto sample_src = make_tau_source(src);
	return make_nso_rr_using_factory<tau_ba<sbf_ba>, sbf_ba>(sample_src).value();
}

tau_<tau_ba<sbf_ba>, sbf_ba> tau_make_nso_test(const std::string& src) {
	return tau_make_nso_rr_test(src).main;
}

tau_<tau_ba<sbf_ba>, sbf_ba> normalize_test_tau(const char* src) {
	rr<tau_<tau_ba<sbf_ba>, sbf_ba>> nso_rr = make_nso_rr_using_factory<
		tau_ba<sbf_ba>, sbf_ba>(src).value();
	return normalizer<tau_ba<sbf_ba>, sbf_ba>(nso_rr);
}

std::ostream& print_tau(std::ostream &os, tau_<sbf_ba> n, size_t l = 0) {
	os << "{";
	// for (size_t t = 0; t < l; t++) os << " ";
	std::visit(idni::tau_lang::overloaded{
		[&os](tau_source_sym v) { if (v.nt()) os << v.n(); else os << v.t(); },
		[&os](std::variant<sbf_ba> v) {
			if (auto b = std::get<0>(v); b == true) os << "true";
			else if (auto b = std::get<0>(v); b == false) os << "false";
			else os << "...sbf..."; }
	}, n->value);
	for (auto& d : n->child) print_tau(os, d, l + 1);
	os << "}";
	return os;
}

std::ostream& pretty_print_tau(std::ostream &os, tau_<sbf_ba> n, size_t l = 0) {
	// for (size_t t = 0; t < l; t++) os << " ";
	std::visit(idni::tau_lang::overloaded{
		[&os](tau_source_sym v) { if (!v.nt()) os << v.t(); },
		[&os](std::variant<sbf_ba> v) {
			if (auto b = std::get<0>(v); b == true) os << "true";
			else if (auto b = std::get<0>(v); b == false) os << "false";
			else os << "...sbf..."; }
	}, n->value);
	for (auto& d : n->child) pretty_print_tau(os, d, l + 1);
	return os;
}

}
#endif // __TEST_INTEGRATION_HELPERS_SBF_H__