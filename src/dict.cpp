// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cassert>

#include "dict.h"

using namespace std;

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// bdd var dict

vector<string> v({ "dummy" });
map<string, size_t> m;

sym_t dict(const char* s) {
	if (auto it = m.find(s); it != m.end()) return it->second;
	return m.emplace(s, v.size()), v.push_back(s), v.size() - 1;
}

const char* dict(sym_t n) {
	assert((size_t)n <= v.size());
	if ((size_t)n == v.size()) {
		static string tmp;
		do {
			stringstream ss;
			ss << "x" << n;
			if (auto it = m.find(ss.str()); it == m.end()) {
				dict(ss.str());
				return (tmp = ss.str()).c_str();
			}
			++n;
		} while (true);
	}
	return v[n].c_str();
}

sym_t dict(const string& s) { return dict(s.c_str()); }

// -----------------------------------------------------------------------------
// tau strings dict

vector<string> S{ "" };                // strings
map<string, size_t> SM{ { "", 0 } };   // string -> id map

size_t string_id(const std::string& s) {
	if (auto it = SM.find(s); it != SM.end()) return it->second;
	return SM.emplace(s, S.size()), S.push_back(s), S.size() - 1;
}

const std::string& string_from_id(size_t id) {
	DBG(assert(id <= S.size());)
	return S[id];
}

} // namespace idni::tau_lang
