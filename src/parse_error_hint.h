// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Library-level parse-error classifier (Q40-UX1).
//
// Post-processes a raw formula string that failed parsing to extract a
// short, actionable hint that callers can show to the user.  Pure header;
// no side effects; no dependencies beyond <regex>, <set>, <string>.
//
// The main tau binary, tau_eval, the REPL, and any other consumer can call
// `tau_lang::classify_parse_error(formula_text)` after `get_nso_rr` / parser
// failure to produce a human-friendly explanation alongside the raw parser
// message.

#ifndef __IDNI__TAU__PARSE_ERROR_HINT_H__
#define __IDNI__TAU__PARSE_ERROR_HINT_H__

#include <regex>
#include <set>
#include <string>

namespace idni::tau_lang {

// Analyze the formula text for common mistakes and return a short hint.
// Returns empty string if no specific hint applies.
inline std::string classify_parse_error(const std::string& formula) {
	static const std::regex re_fwd_ref(R"(\[t\s*\+)");
	static const std::regex re_type_ann(R"([\}a-zA-Z0-9]:\s*[a-zA-Z]\w*)");
	static const std::regex re_type_extract(R"(:(\w+))");
	static const std::regex re_nested_g(R"(\bG\s*\(\s*G\b)");

	// Forward reference [t+k] (banned; only [t-k] is allowed).
	if (std::regex_search(formula, re_fwd_ref))
		return "forward time references [t+k] are not allowed "
		       "(only [t-k])";

	// Missing trailing dot.
	{
		std::string trimmed = formula;
		size_t end = trimmed.find_last_not_of(" \t\r\n");
		if (end != std::string::npos && trimmed[end] != '.')
			return "missing spec terminator '.' at end of formula";
	}

	// Unknown type annotation (after the rename: valid are
	// tau, sbf, bv, qlt, qint, nlang).
	// Syntax: {source}:type or variable:type — match }:word or \w:word.
	if (std::regex_search(formula, re_type_ann)) {
		static const std::set<std::string> known_types = {
			"tau", "sbf", "bv", "qlt", "qint", "nlang"
		};
		// Extract all :word occurrences and check each.
		std::string s = formula;
		auto it = std::sregex_iterator(s.begin(), s.end(), re_type_extract);
		for (; it != std::sregex_iterator(); ++it) {
			std::string t = (*it)[1].str();
			if (!known_types.count(t))
				return "unknown type annotation ':" + t
				     + "' (valid: tau, sbf, bv, qlt, qint, nlang)";
		}
	}

	// Nested G/F — not allowed by the grammar.
	if (std::regex_search(formula, re_nested_g))
		return "nested G operators are not allowed";

	return "";
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__PARSE_ERROR_HINT_H__
