// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "ltl_aba.h"
#include "ba_provider.h"
#include "algorithm_a_skeleton.h"
#include "algorithm_b_skeleton.h"
#include "algorithm_d_game.h"
#include "omcat_constants.h"
#include "ba_type_traits.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ltl_aba"

#include <array>
#include <atomic>
#include <map>
#include <set>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <fstream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <vector>
#include <sys/wait.h>
#include <unistd.h>
#include <spawn.h>
#include <signal.h>
#include <fcntl.h>

extern char **environ;

namespace idni::tau_lang {

// Forward declaration: is_tau_formula_sat is defined in satisfiability.tmpl.h
// which includes this file, so we forward-declare to avoid a circular include.
template <NodeType node>
bool is_tau_formula_sat(tref fm, int_t start_time, bool output);

} // namespace idni::tau_lang

// Split into logical sub-files for readability.
// Include order matches the original code order.
#include "ltl_aba_helpers.tmpl.h"
#include "ltl_aba_synthesis.tmpl.h"
#include "ltl_aba_normalization.tmpl.h"
#include "ltl_aba_builders.tmpl.h"
