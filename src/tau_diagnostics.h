// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__TAU_DIAGNOSTICS_H__
#define __IDNI__TAU__TAU_DIAGNOSTICS_H__

#include "utility/diagnostics.h"

namespace idni::tau_lang {

namespace diag = ::idni::diagnostics;

template <typename T>
using result = diag::result<T>;

using report = diag::report;
using code = diag::code;
using attr = diag::attr;
using diag_sink = diag::sink;
using diag_sinks = diag::sinks;

inline bool step_awaiting_input(const report& rep) {
	for (const auto& n : rep.nodes())
		if (n.tag == code::invalid_state) return true;
	return false;
}

inline bool report_has_code(const report& rep, code c) {
	for (const auto& n : rep.nodes())
		if (n.tag == c) return true;
	return false;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__TAU_DIAGNOSTICS_H__
