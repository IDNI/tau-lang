// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Library-level parse-error classifier (Q40-UX1).
//
// Post-processes a raw formula string that failed parsing to extract a
// short, actionable hint that callers can show to the user.  Pure header;
// no side effects; no dependencies beyond <algorithm>, <regex>, <string>.
//
// The REPL, the tau_eval test driver, and any other consumer can call
// `tau_lang::classify_parse_error<node>(formula_text)` after `get_nso_rr` /
// parser failure to produce a human-friendly explanation alongside the raw
// parser message.  `node` supplies the configured pack's type names through
// `node::ba::type_names()` and `node::ba::types_joined()`.

#ifndef __IDNI__TAU__PARSE_ERROR_HINT_H__
#define __IDNI__TAU__PARSE_ERROR_HINT_H__

#include <algorithm>
#include <regex>
#include <string>

namespace idni::tau_lang {

/**
 * @brief Analyze the formula text for common mistakes and return a short
 * hint.
 *
 * Returns empty string if no specific hint applies.  The checks, in order,
 * first match wins: a bare numeral other than 0 and 1 beside a bitvector
 * operand, a missing trailing `.`
 * on formula-like input (one containing `=`, `always` or `sometimes`), an
 * unknown non-numeric type annotation (against the configured pack's type
 * names). Pure: no side effects.
 * @tparam node Node type supplying `node::ba::type_names()` and
 * `node::ba::types_joined()`.
 * @param formula Raw formula text that failed to parse.
 * @return A one-line hint, or the empty string.
 */
template <typename node>
inline std::string classify_parse_error(const std::string& formula) {
	static const std::regex re_type_ann(R"([\}a-zA-Z0-9]:\s*[a-zA-Z]\w*)");
	static const std::regex re_type_extract(R"(:(\w+))");
	// a bv operand beside a bare numeral, as in `o1[t]:bv[8] = 255`
	static const std::regex re_bare_bv_numeral(
		R"(:\s*bv\s*\[\s*(\d+)\s*\]\s*(?:!?=|!?<=?|!?>=?|[-+*/%])\s*(\d+)\b)");

	// Only 0 and 1 are numerals of every algebra; another bv constant is
	// written with its type.
	{
		std::smatch m;
		if (std::regex_search(formula, m, re_bare_bv_numeral)) {
			const std::string n = m[2].str();
			if (n != "0" && n != "1")
				return "a bitvector constant other than 0 and 1 is "
					"written with its type: { " + n + " }:bv["
					+ m[1].str() + "]";
		}
	}

	// Missing trailing dot, only when the input looks like a formula/spec
	// (contains '=' or a temporal keyword), so a misspelled REPL command
	// does not get this hint.
	{
		std::string trimmed = formula;
		size_t end = trimmed.find_last_not_of(" \t\r\n");
		bool formula_like = formula.find('=') != std::string::npos
			|| formula.find("always") != std::string::npos
			|| formula.find("sometimes") != std::string::npos;
		if (formula_like && end != std::string::npos
			&& trimmed[end] != '.')
			return "missing spec terminator '.' at end of formula";
	}

	// Unknown type annotation; the valid names are the configured pack's.
	// Syntax: {source}:type or variable:type — match }:word or \w:word.
	if (std::regex_search(formula, re_type_ann)) {
		constexpr auto known_types = node::ba::type_names();
		// Extract all :word occurrences and check each.
		std::string s = formula;
		auto it = std::sregex_iterator(s.begin(), s.end(), re_type_extract);
		for (; it != std::sregex_iterator(); ++it) {
			std::string t = (*it)[1].str();
			// Numeric colon uses (bv widths, offsets) are not type
			// annotations -- skip them.
			if (!t.empty() && std::isdigit(
				static_cast<unsigned char>(t[0]))) continue;
			if (std::ranges::find(known_types, t) == known_types.end())
				return "unknown type annotation ':" + t
				     + "' (valid: "
				     + std::string(node::ba::types_joined()) + ")";
		}
	}

	return "";
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__PARSE_ERROR_HINT_H__
