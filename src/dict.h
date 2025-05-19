// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__DICT_H__
#define __IDNI__TAU__DICT_H__

#include <string>
#include <vector>
#include <map>

namespace idni::tau_lang {

inline static vector<string> S{ "" };                // strings
inline static map<string, size_t> SM{ { "", 0 } };   // string -> id map

inline size_t dict(const std::string& s) {
	if (auto it = SM.find(s); it != SM.end()) return it->second;
	return SM.emplace(s, S.size()), S.push_back(s), S.size() - 1;
}

inline const std::string& dict(size_t id) {
	DBG(assert(id <= S.size());)
	return S[id];
}


} // namespace idni::tau_lang

#endif // __IDNI__TAU__DICT_H__
