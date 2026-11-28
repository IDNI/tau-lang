// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__WORK_BUDGET_GUEST_H__
#define __IDNI__TAU__WORK_BUDGET_GUEST_H__

namespace idni::tau_lang::work_budget_guest {

enum class phase {
	boot, decode, parse, prepare, solve, render, embedded_update,
	update_plan, update_metadata, update_install, report, inspect,
	bv_translate, bv_decision, bv_model_check, bv_model_value,
	hsb_decision, hsb_model_value
};
enum class opaque { input_file, output_file, spot, console_input };

#ifdef TAU_WORK_BUDGET_GUEST
extern "C" __attribute__((import_module("tau"), import_name("phase")))
void enter(phase);
extern "C" __attribute__((import_module("tau"), import_name("unsupported")))
void refuse(opaque);
#else
inline void enter(phase) {}
inline void refuse(opaque) {}
#endif
}

#endif
