// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file output_preferences.h
 * @brief The `output-prefer` option: soft preferences on the values the
 * outputs of a step take.
 */

#ifndef __IDNI__TAU__OUTPUT_PREFERENCES_H__
#define __IDNI__TAU__OUTPUT_PREFERENCES_H__

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <vector>

namespace idni::tau_lang {

/// `output-prefer`: soft preferences on the outputs of each step, as
/// `stream=value` entries separated by commas, the first the strongest. A
/// value is written as in an input file; `prev` is the value the stream took
/// at the step before. Empty prefers nothing.
inline std::string output_prefer_param;

/// One entry of `output-prefer`.
struct output_preference {
	std::string stream;
	std::string value;
};

/// The entries of an `output-prefer` text, blanks around names and values
/// removed. nullopt when an entry is malformed; @p bad then receives it.
inline std::optional<std::vector<output_preference>> parse_output_preferences(
	const std::string& text, std::string* bad = nullptr)
{
	auto trim = [](std::string s) {
		auto blank = [](unsigned char c) { return std::isspace(c); };
		s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), blank));
		s.erase(std::find_if_not(s.rbegin(), s.rend(), blank).base(),
			s.end());
		return s;
	};
	auto name_ok = [](const std::string& n) {
		if (n.empty() || std::isdigit((unsigned char)n[0])) return false;
		return std::all_of(n.begin(), n.end(), [](unsigned char c) {
			return std::isalnum(c) || c == '_'; });
	};
	std::vector<output_preference> out;
	if (trim(text).empty()) return out;
	size_t from = 0;
	while (from <= text.size()) {
		size_t comma = text.find(',', from);
		if (comma == std::string::npos) comma = text.size();
		std::string entry = text.substr(from, comma - from);
		size_t eq = entry.find('=');
		output_preference p;
		if (eq != std::string::npos) {
			p.stream = trim(entry.substr(0, eq));
			p.value = trim(entry.substr(eq + 1));
		}
		if (eq == std::string::npos || !name_ok(p.stream)
			|| p.value.empty())
		{
			if (bad) *bad = trim(entry);
			return std::nullopt;
		}
		out.push_back(std::move(p));
		from = comma + 1;
	}
	return out;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__OUTPUT_PREFERENCES_H__
