// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file tau_compile.h
 * @brief `tau gen` / `tau compile`: turn a Tau spec into a C++ artifact and,
 * for compile, build it through the SDK's cmake script.
 *
 * All build logic lives in cmake/tau-compile.cmake; this file emits the
 * artifact and spawns the script, so no build path is baked into the tau
 * binary.
 */

#ifndef __IDNI__TAU__TAU_COMPILE_H__
#define __IDNI__TAU__TAU_COMPILE_H__

#include <string>
#include <vector>

#include "api.h"
#include "ltl_aba.h"
#include "cpp_codegen.h"
#include "tau_diagnostics.h"

namespace idni::tau_lang {

/**
 * @brief Success payload of gen_spec/compile_spec, wrapped in a result whose
 * report carries any failure reason.
 */
struct codegen_result {
	/// The artifact directory after gen_spec, the executable after
	/// compile_spec.
	std::string exe_path;
	/// `true` when @ref exe_path is set.
	bool ok() const { return !exe_path.empty(); }
};

/**
 * @brief Find the SDK directory holding TauConfig.cmake,
 * cmake/tau-compile.cmake and the artifact template.
 *
 * The search is @p sdk_dir, then `<exe_dir>/../<platform>/sdk` for a
 * source tree, then one box per platform under the emitting binary's lib,
 * lib64 and multiarch lib dirs (`<prefix>/lib/tau/sdk/<platform>/lib/cmake/Tau`),
 * then `<exe_dir>/sdk` and the flat Windows zip's `<exe_dir>/cmake/Tau`, each
 * of these last two only when its own platform marker agrees. An empty
 * @p platform asks for the running tau's own SDK: `<exe_dir>/sdk` for a build
 * tree, else the first box beside the installed binary. Only `tau compile` needs it;
 * `tau gen` embeds everything it writes into the artifact.
 *
 * @param platform A platform name, never a preset name: compile_spec maps a
 * preset with the table compiled into tau first. Empty for this platform.
 * @param sdk_dir The SDK the caller names, from `sdk-dir`; empty searches.
 * @return The SDK directory; a `not_found` error naming the package to
 * install when none matches or @p sdk_dir holds no SDK, and an
 * `unsupported_operation` error on wasm.
 */
inline result<std::string> resolve_sdk_dir(const std::string& platform = "",
	const std::string& sdk_dir = "");

/**
 * @brief Parse, synthesize and emit the artifact for @p spec_src.
 *
 * Writes main.cpp, the CMakeLists, the presets, the platform map and the
 * toolchain files they name, all from templates compiled into tau, plus a
 * marker file that lets a later run write into the same directory. No SDK, no
 * toolchain, no build. The artifact plays the strategy as a table when `run`
 * would play a data game's Mealy machine of at most compile_max_table_edges
 * edges, and otherwise embeds the spec and solves as it runs. Clears the
 * process-wide definitions for the call and restores them afterwards.
 *
 * @tparam Node Tree node type.
 * @param spec_src The spec text, as `run` reads it.
 * @param out_dir The artifact directory; empty for `<current dir>/spec.build`.
 * A non-empty directory without the marker is refused.
 * @param exe_name The executable target name written into the CMakeLists.
 * @param verb The command name the messages start with (`gen`, `compile`).
 * @return The artifact directory in @ref codegen_result::exe_path; an error
 * when the spec does not parse, has free variables, is unrealizable or
 * undecided, runs no strategy, or a file cannot be written.
 */
template <NodeType Node>
result<codegen_result> gen_spec(
	const std::string& spec_src,
	const std::string& out_dir = "",
	const std::string& exe_name = "program",
	const std::string& verb = "gen");

/**
 * @brief gen_spec, then the SDK's cmake script to configure, build and copy
 * the executable to @p out_exe.
 *
 * The build runs beside the destination and replaces an existing output only
 * after it succeeds; the script's output goes to compile.log in the artifact
 * directory.
 *
 * @tparam Node Tree node type.
 * @param spec_src The spec text.
 * @param out_exe Destination of the executable; empty leaves it at
 * `<artifact dir>/program`. A wasm build writes `<out_exe>.js` and its .wasm.
 * @param build_dir The artifact directory; empty for
 * `<current dir>/spec.build`.
 * @param cxx The compiler, from `cxx`; empty takes cmake's default for a
 * native build, else the compiler of the platform preset.
 * @param preset Empty for a native build: the running tau's own SDK, cmake's
 * compiler and a Release build type unless a -DCMAKE_BUILD_TYPE is given. A
 * non-empty name maps to a platform whose SDK box and toolchain build the
 * artifact.
 * @param extra_args `-D...` and `-G <gen>` items forwarded to the configure; a
 * -D value wins over the preset.
 * @param sdk_dir The SDK, from `sdk-dir`; empty for @ref resolve_sdk_dir's
 * search.
 * @return The executable's path in @ref codegen_result::exe_path; an error
 * from gen_spec, an unknown preset, a missing SDK, or a failed build (with
 * the end of compile.log).
 */
template <NodeType Node>
result<codegen_result> compile_spec(
	const std::string& spec_src,
	const std::string& out_exe = "",
	const std::string& build_dir = "",
	const std::string& cxx = "",
	const std::string& preset = "",
	const std::vector<std::string>& extra_args = {},
	const std::string& sdk_dir = "");

} // namespace idni::tau_lang

#include "tau_compile.tmpl.h"

#endif // __IDNI__TAU__TAU_COMPILE_H__
