// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __TEST_HELPERS_H__
#define __TEST_HELPERS_H__

#include <cassert>
#include "doctest.h"

#include "defs.h"
#include "init_log.h"

#include "boolean_algebras/variant_ba.h"
#include "boolean_algebras/bool_ba.h"
#include "boolean_algebras/bdds/bdd_handle.h"
#include "boolean_algebras/sbf_ba.h"
#include "nso_rr.h"
#include "language.h"

using namespace idni;
using namespace idni::tau_lang;
using namespace std;

namespace testing = doctest;

template <typename... BAs>
tref make(const std::string& s, tau_parser::parse_options opts = {}) {
	return tau::get<BAs...>(s, opts);
}

template <typename... BAs>
tt make_tt(const std::string& s, tau_parser::parse_options opts = {}) {
	return tt(make<BAs...>(s, opts));
}

#endif // __TEST_HELPERS_H__