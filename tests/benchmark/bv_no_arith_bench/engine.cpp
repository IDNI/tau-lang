// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// The questions to the tau library; see engine.h. The node pack is the one the
// anti-prenex suites run on (bitvectors plus Bool).

#include "test_Bool_helpers.h"
#include "anti_prenex/anti_prenex.h"

#include "engine.h"

#include <stdexcept>

namespace bv_bench {

using namespace idni::tau_lang;

namespace {

using steady = std::chrono::steady_clock;

tref parse(const std::string& text) {
	// Stream variables such as `i1_3` need multi-letter names.
	api<node_t>::set_charvar(false);
	const auto spec = get_nso_rr<node_t>(tau::get(text));
	if (!spec || !spec->main) throw std::runtime_error(
		"the formula does not parse");
	return spec->main->get();
}

} // namespace

verdict engine_verdict(const std::string& formula) {
	const tref phi = parse(formula);
	const steady::time_point start = steady::now();
	const tref got = anti_prenexing::anti_prenex<node_t>(phi);
	const double ms = ms_since(start);
	const auto& result = tau::get(got);
	if (result.equals_T()) return { "T", 0, ms };
	if (result.equals_F()) return { "F", 0, ms };
	return { "open", result.select_all(is_child_quantifier<node_t>).size(), ms };
}

verdict oracle_verdict(const std::string& formula) {
	const tref phi = parse(formula);
	const steady::time_point start = steady::now();
	const auto status = bv_formula_sat_status<node_t>(phi);
	const double ms = ms_since(start);
	if (!status) return { "untranslated", 0, ms };
	switch (*status) {
	case bv_sat_status::sat:   return { "T", 0, ms };
	case bv_sat_status::unsat: return { "F", 0, ms };
	default:                   return { "unknown", 0, ms };
	}
}

} // namespace bv_bench
