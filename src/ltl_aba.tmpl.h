// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md
//
// Umbrella implementation header for ltl_aba.h: pulls in the algorithm
// headers (A/B skeletons, the Algorithm D game), the formula type-set
// helpers and satisfiability.h, sets the "ltl_aba" log channel, and then
// includes the sub-files in the original code order:
// ltl_aba_helpers.tmpl.h, ltl_aba_synthesis.tmpl.h,
// ltl_aba_normalization.tmpl.h, ltl_aba_data_game.tmpl.h and
// ltl_aba_builders.tmpl.h.

#include "ltl_aba.h"
#include "formula_type_set.h"
#include "algorithm_a_skeleton.h"
#include "algorithm_b_skeleton.h"
#include "algorithm_d_game.h"
#include "satisfiability.h"
#include "backends/bdds/data_bdd.h"
#include "reset_hooks.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ltl_aba"

#include <algorithm>
#include <array>
#include <atomic>
#include <charconv>
#include <limits>
#include <map>
#include <set>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <fstream>
#include <memory>
#include <mutex>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <vector>

// Split into logical sub-files for readability.
// Include order matches the original code order.
#include "ltl_aba_helpers.tmpl.h"
#include "ltl_aba_synthesis.tmpl.h"
#include "ltl_aba_normalization.tmpl.h"
#include "ltl_aba_data_game.tmpl.h"
#include "ltl_aba_builders.tmpl.h"
