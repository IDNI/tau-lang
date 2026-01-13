// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cassert>

#include "var_dict.h"

using namespace std;

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// bdd var dict

vector<string> v({ "dummy" });
map<string, size_t> m;

sym_t var_dict(const char* s) {
	if (auto it = m.find(s); it != m.end()) return it->second;
	return m.emplace(s, v.size()), v.push_back(s), v.size() - 1;
}

const char* var_dict(sym_t n) {
	assert((size_t)n <= v.size());
	if ((size_t)n == v.size()) {
		static string tmp;
		do {
			stringstream ss;
			ss << "x" << n;
			if (auto it = m.find(ss.str()); it == m.end()) {
				var_dict(ss.str());
				return (tmp = ss.str()).c_str();
			}
			++n;
		} while (true);
	}
	return v[n].c_str();
}

sym_t var_dict(const string& s) { return var_dict(s.c_str()); }

} // namespace idni::tau_lang
