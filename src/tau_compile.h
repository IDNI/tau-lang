// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// `tau gen` / `tau compile`: turn a Tau spec into a C++ artifact and, for
// compile, build it through the SDK's cmake script. All build logic lives in
// cmake/tau-compile.cmake; this file emits the artifact and spawns the
// script, so no build path is baked into the tau binary.

#ifndef __IDNI__TAU__TAU_COMPILE_H__
#define __IDNI__TAU__TAU_COMPILE_H__

#include <string>
#include <vector>

#include "api.h"
#include "ltl_aba.h"
#include "cpp_codegen.h"
#include "tau_diagnostics.h"

namespace idni::tau_lang {

// Returned by gen_spec/compile_spec, wrapped in a result: the report carries
// the failure reason, so this struct only holds the success payload. exe_path
// is the artifact directory after gen_spec and the executable after
// compile_spec.
struct codegen_result {
	std::string exe_path;
	bool ok() const { return !exe_path.empty(); }
};

// The directory holding TauConfig.cmake, cmake/tau-compile.cmake and the
// artifact template. `platform` names a target platform: the search is
// $TAU_SDK_DIR, then <exe_dir>/../<platform>/sdk for a source tree, then one
// box per platform under the emitting binary's lib dir
// (<prefix>/lib/tau/sdk/<platform>/lib/cmake/Tau). An empty `platform` asks
// for the running tau's own SDK: <exe_dir>/sdk for a build tree, else the box
// beside the installed binary. `platform` is a platform name, never a preset
// name: compile_spec maps a preset with the table compiled into tau first.
// Only `tau compile` needs it; `tau gen` embeds everything it writes into the
// artifact.
inline result<std::string> resolve_sdk_dir(const std::string& platform = "");

// Parse, synthesize and emit the artifact for `spec_src` into `out_dir`:
// main.cpp, the CMakeLists and the preset files it includes, all from
// templates compiled into tau. No SDK, no toolchain, no build.
// out_dir empty -> <current dir>/spec.build.
template <NodeType Node>
result<codegen_result> gen_spec(
	const std::string& spec_src,
	const std::string& out_dir = "",
	const std::string& exe_name = "program",
	const std::string& verb = "gen");

// gen_spec, then the SDK's cmake script to configure, build and copy the
// executable to `out_exe`.
// out_exe empty  -> the executable is left at <artifact dir>/program.
// build_dir empty -> defaults to <current dir>/spec.build.
// cxx empty       -> TAU_CXX, else cmake's default for a native build, else
//                    the compiler of the platform preset.
// preset empty    -> a native build: the running tau's own SDK, cmake's
//                    compiler and a Release build type unless a
//                    -DCMAKE_BUILD_TYPE is given. A non-empty name maps to a
//                    platform whose SDK box and toolchain build the artifact.
// extra_args      -> -D... and -G <gen> items forwarded to the configure; a
//                    -D value wins over the preset.
template <NodeType Node>
result<codegen_result> compile_spec(
	const std::string& spec_src,
	const std::string& out_exe = "",
	const std::string& build_dir = "",
	const std::string& cxx = "",
	const std::string& preset = "",
	const std::vector<std::string>& extra_args = {});

} // namespace idni::tau_lang

#include "tau_compile.tmpl.h"

#endif // __IDNI__TAU__TAU_COMPILE_H__
