// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

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
using namespace idni::tau;

nso<tau_ba<sbf_ba>, sbf_ba> normalize_test_tau(const char* src) {
	rr<nso<tau_ba<sbf_ba>, sbf_ba>> nso_rr = make_nso_rr_using_factory<
		tau_ba<sbf_ba>, sbf_ba>(src).value();
	return normalizer<tau_ba<sbf_ba>, sbf_ba>(nso_rr);
}

nso<tau_ba<sbf_ba>, sbf_ba> get_tau(const char* src) {
	rr<nso<tau_ba<sbf_ba>, sbf_ba>> nso_rr = make_nso_rr_using_factory<
		tau_ba<sbf_ba>, sbf_ba>(src).value();
	return normalizer<tau_ba<sbf_ba>, sbf_ba>(nso_rr);
}

#endif // __TEST_INTEGRATION_HELPERS_TAU_H__