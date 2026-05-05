// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <list>
#include <numeric>

#include "nso_rr.h"
#include "tau_bdd.h"
#include "normal_forms.h"
#include "ba_types.h"
#include "union_find_with_sets.h"
#include "heuristics/bv_predicate_blasting.h"
#include "heuristics/simplify_using_equality.h"

// Split into logical sub-files for readability.
// Include order matches the original code order.
#include "normal_forms_helpers.tmpl.h"
#include "normal_forms_dnf.tmpl.h"
#include "normal_forms_nnf.tmpl.h"
#include "normal_forms_bf.tmpl.h"
#include "normal_forms_boole.tmpl.h"
