// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "test_helpers.h"
#include "normalizer.h"
#include "boolean_algebras/bdds/bdd_handle.h"

using namespace idni::rewriter;
using namespace idni::tau_lang;

namespace testing = doctest;

// TODO (HIGH) write tests to check build_dnf_from_clauses
// TODO (HIGH) write tests to check to_minterm
// TODO (HIGH) write tests to check get_dnf_clauses
// TODO (HIGH) write tests to check get_positive_negative_literals
// TODO (HIGH) write tests to check get_literals

// TODO (VERY_LOW) write tests to check make_tau_source
// TODO (VERY_LOW) write tests to check make_tau_source_from_file
// TODO (VERY_LOW) write tests to check process_digits
// TODO (VERY_LOW) write tests to check make_tau_code
// TODO (VERY_LOW) write tests to check make_library
// TODO (VERY_LOW) write tests to check make_nso_rr_using_binder
// TODO (VERY_LOW) write tests to check make_nso_rr_using_bindings
// TODO (VERY_LOW) write tests to check make_nso_rr_using_factory
// TODO (VERY_LOW) write tests to check make_builder

// TODO (VERY_LOW) write tests to check tau_apply_builder
// TODO (VERY_LOW) write tests to check trim
// TODO (VERY_LOW) write tests to check wrap

// TODO (VERY_LOW) write tests to check nso_rr_apply_if
// TODO (VERY_LOW) write tests to check nso_rr_apply

// TODO (VERY_LOW) write tests to check operator<< (all versions)


// TODO (LOW) write tests to check steps
// TODO (LOW) write tests to check repeat_each
// TODO (LOW) write tests to check repeat_all
// TODO (LOW) write tests to check repeat
// TODO (LOW) write tests to check operator| (all versions)

// TODO (LOW) write tests to check get_free_vars_from_nso
