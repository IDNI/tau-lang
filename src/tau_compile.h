// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// `tau compile`: wraps the existing cpp_codegen emitter into an end-to-end
// spec → executable pipeline. The emitter itself (src/cpp_codegen.{h,tmpl.h})
// is untouched here; this only drives it and the resulting cmake project.

#ifndef __IDNI__TAU__TAU_COMPILE_H__
#define __IDNI__TAU__TAU_COMPILE_H__

#include <string>

#include "api.h"
#include "ltl_aba.h"
#include "cpp_codegen.h"
#include "tau_diagnostics.h"

namespace idni::tau_lang {

// Returned by compile_spec, wrapped in a result: the report carries the
// failure reason, so this struct only holds the success payload.
struct codegen_result {
	std::string exe_path;
	bool ok() const { return !exe_path.empty(); }
};

// Parse, synthesize and emit `spec_src` via the existing cpp_codegen
// emitter, then drive cmake configure + build in `build_dir` to produce an
// executable, copied (or built directly) to `out_exe`.
// out_exe empty  → the executable is left at build_dir/program.
// build_dir empty → defaults to <current dir>/spec.build.
// cxx empty       → TAU_CXX, else clang++ when on PATH, else cmake's default.
template <NodeType Node>
result<codegen_result> compile_spec(
	const std::string& spec_src,
	const std::string& out_exe = "",
	const std::string& build_dir = "",
	const std::string& cxx = "");

} // namespace idni::tau_lang

#include "tau_compile.tmpl.h"

#endif // __IDNI__TAU__TAU_COMPILE_H__
