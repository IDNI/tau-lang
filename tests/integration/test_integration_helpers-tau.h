// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __TEST_INTEGRATION_HELPERS_TAU_H__
#define __TEST_INTEGRATION_HELPERS_TAU_H__

#include "../src/nso_rr.h"
#include "../src/babdd.h"
#include "../src/bdd_handle.h"
#include "../src/bool_ba.h"
#include "../src/normalizer.h"
#include "../src/sbf_ba.h"
#include "../src/tau_ba.h"
#include "dict.h"


using namespace idni::rewriter;
using namespace idni::tau_lang;

tau<tau_ba<sbf_ba>, sbf_ba> normalize_test_tau(const char* src) {
	rr<tau<tau_ba<sbf_ba>, sbf_ba>> nso_rr = make_nso_rr_using_factory<
		tau_ba<sbf_ba>, sbf_ba>(src).value();
	return normalizer<tau_ba<sbf_ba>, sbf_ba>(nso_rr);
}

tau<tau_ba<sbf_ba>, sbf_ba> get_tau(const char* src) {
	rr<tau<tau_ba<sbf_ba>, sbf_ba>> nso_rr = make_nso_rr_using_factory<
		tau_ba<sbf_ba>, sbf_ba>(src).value();
	return normalizer<tau_ba<sbf_ba>, sbf_ba>(nso_rr);
}

#endif // __TEST_INTEGRATION_HELPERS_TAU_H__