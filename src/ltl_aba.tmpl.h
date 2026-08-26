// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "ltl_aba.h"
#include "formula_type_set.h"
#include "algorithm_a_skeleton.h"
#include "algorithm_b_skeleton.h"
#include "algorithm_d_game.h"
#include "satisfiability.h"

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
#include <unordered_map>
#include <vector>
#include <sys/wait.h>
#include <unistd.h>
#include <spawn.h>
#include <signal.h>
#include <fcntl.h>

extern char **environ;

// Split into logical sub-files for readability.
// Include order matches the original code order.
#include "ltl_aba_helpers.tmpl.h"
#include "ltl_aba_synthesis.tmpl.h"
#include "ltl_aba_normalization.tmpl.h"
#include "ltl_aba_builders.tmpl.h"
