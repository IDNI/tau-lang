// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md
//
// Operator-preference strengthening for spec synthesis (plan v10 §6 row A1
// "Preference canonicalisation"; claude-code-addendum.tex §5).
//
// Given a spec Φ and an ordered list of preferences over output variables,
// produce a strengthened spec Φ_pref = Φ ∧ tie_breakers(≼).
//
// Realisability gate: each preference is conjuncted in declaration order;
// preferences that would render the spec unrealisable are dropped (logged).
// This realises a "lex-priority" semantics — earlier preferences win over
// later ones when in conflict.
//
// USAGE:
//   PreferenceOrder po;
//   po.entries.push_back({"o1", "0"});  // prefer o1 = 0 always
//   po.entries.push_back({"o2", "1"});  // prefer o2 = 1 always
//   tref strengthened = apply_preferences<node_t>(spec, po);
//   auto interp = api<node_t>::get_interpreter(strengthened);

#ifndef __IDNI__TAU__PREFERENCES_H__
#define __IDNI__TAU__PREFERENCES_H__

#include "api.h"
#include "ltl_aba.h"

#include <string>
#include <vector>

namespace idni::tau_lang {

struct PreferenceEntry {
	std::string var_name;        // output stream variable name (e.g. "o1")
	std::string preferred_value; // value literal in tau syntax (e.g. "0", "1", "<:5>")
};

struct PreferenceOrder {
	std::vector<PreferenceEntry> entries;
};

// Strengthen `spec` with tie-breaker clauses derived from `po`. Returns the
// new spec (or `spec` unchanged if `po.entries` is empty / all preferences
// are unrealisable on top of the running spec).
//
// Preferences are conjuncted in declaration order; each is gated on
// realisability of the spec-so-far ∧ this-preference. Failing preferences
// are silently dropped (logged at DEBUG level).
template <NodeType node>
tref apply_preferences(tref spec, const PreferenceOrder& po) {
	using tau = tree<node>;
	if (po.entries.empty()) return spec;

	tref result = spec;
	for (const auto& entry : po.entries) {
		// Build the temporal preference clause: G(var[t] = preferred_value).
		// String-parse via api::get_formula, which handles all type / BA
		// machinery uniformly.
		std::string pref_str = "G(" + entry.var_name + "[t] = "
		                       + entry.preferred_value + ")";
		tref pref_clause = api<node>::get_formula(pref_str);
		if (pref_clause == nullptr) {
			// Parse failed (typo in var name, value-type mismatch, etc.).
			// Skip; do not corrupt the spec.
			continue;
		}
		tref candidate = tau::build_wff_and(result, pref_clause);
		if (is_ltl_aba_realizable<node>(candidate, 0, false)) {
			result = candidate;
		}
		// else: preference would make the spec unrealisable; drop it.
	}
	return result;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__PREFERENCES_H__
