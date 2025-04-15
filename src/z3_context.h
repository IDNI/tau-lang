// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __Z3_CONTEXT_H__
#define __Z3_CONTEXT_H__

#include <z3++.h>

namespace idni::tau_lang {

using namespace z3;

static z3::context z3_context;

void z3_config() {
	set_param("pp.bv-literals", false);
	set_param("smt.mbqi", true);
}

} // namespace idni::tau_lang

#endif // __Z3_CONTEXT_H__