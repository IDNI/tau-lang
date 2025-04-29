// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__TESTS__INTEGRATION__TEST_INTEGRATION_HELPERS_H__
#define __IDNI__TAU__TESTS__INTEGRATION__TEST_INTEGRATION_HELPERS_H__

#include <cassert>
#include "doctest.h"

#include "boolean_algebras/bool_ba.h"
#include "boolean_algebras/sbf_ba.h"

using namespace std;
using namespace idni;
using namespace idni::tau_lang;

namespace testing = doctest;

using tau = tree<node<sbf_ba>>;
using tt  = tau::traverser;

bool check_normalized(const char* sample, tau::node::type nt) {
	auto nso_rr_opt = get_nso_rr<node<sbf_ba>>(tau::get(sample));
	if (!nso_rr_opt.has_value()) return false;
	tref result = normalizer<node<sbf_ba>>(nso_rr_opt.value());
	if (!result) return false;
	auto x = tt(result) | nt;
	return x.has_value();
}

#endif // __IDNI__TAU__TESTS__INTEGRATION__TEST_INTEGRATION_HELPERS_H__