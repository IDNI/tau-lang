// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "ltl_aba.h"
#include "formula_type_set.h"
#include "algorithm_a_skeleton.h"
#include "algorithm_b_skeleton.h"
#include "algorithm_d_game.h"
#include "satisfiability.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "ltl_aba"

#include <algorithm>
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
#include <unordered_map>
#include <vector>
#include <unistd.h>
#include <fcntl.h>

#ifndef __EMSCRIPTEN__
// posix_spawn is declared but not implemented under Emscripten; wasm has no process model.
#include <thread>
#include <sys/wait.h>
#include <spawn.h>
#include <signal.h>

extern char **environ;
#endif // __EMSCRIPTEN__

// Split into logical sub-files for readability.
// Include order matches the original code order.
#include "ltl_aba_helpers.tmpl.h"
#include "ltl_aba_synthesis.tmpl.h"
#include "ltl_aba_normalization.tmpl.h"
#include "ltl_aba_builders.tmpl.h"
