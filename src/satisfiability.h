// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__SATISFIABILITY_H__
#define __IDNI__TAU__SATISFIABILITY_H__

#include "normalizer.h"

namespace idni::tau_lang {

inline static bool use_debug_output_in_sat = false;
inline void print_fixpoint_info(const std::string& message,
	const std::string& result, const bool output)
{
	if (!output) return;
	if (!use_debug_output_in_sat) BOOST_LOG_TRIVIAL(info) << "\n"
					<< message << "\n" << result << "\n\n";
	else std::cerr << message << "\n" << result << "\n";
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__SATISFIABILITY_H__