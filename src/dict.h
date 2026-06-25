// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file dict.h
 * @brief Bidirectional string ↔ integer dictionary used across Tau components.
 */

#ifndef __IDNI__TAU__DICT_H__
#define __IDNI__TAU__DICT_H__

#include <string>
#include <vector>
#include <map>

// -----------------------------------------------------------------------------
// dict - string <-> size_t map

namespace idni::tau_lang {

/** @brief Return the global string table (index 0 is the empty string). */
inline std::vector<std::string>& S() {
	static std::vector<std::string> S{ "" };                // strings
	return S;
}

/**
 * @brief Map string @p s to a stable integer id, inserting it if absent.
 * @param s String to look up or register.
 * @return Integer id for @p s (0 means the empty string).
 */
inline size_t dict(const std::string& s) {
	static std::map<std::string, size_t> SM{ { "", 0 } };   // string -> id map
	if (auto it = SM.find(s); it != SM.end()) return it->second;
	return SM.emplace(s, S().size()), S().push_back(s), S().size() - 1;
}

/**
 * @brief Return the string registered under integer @p id.
 * @param id Id previously returned by `dict(string)`.
 * @return Reference to the corresponding string.
 */
inline const std::string& dict(size_t id) {
	DBG(assert(id <= S().size());)
	return S()[id];
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__DICT_H__
