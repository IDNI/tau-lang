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

// Returned by value: a pointer into the function-local static buffer this
// used to return got overwritten by the next auto-generated-name call, and
// a pointer into v[n] could dangle after a later push_back reallocates the
// vector (v[n]'s short-string-optimized buffer lives inside the string
// object, which push_back may move to a new address).
string var_dict(sym_t n) {
	assert((size_t)n <= v.size());
	if ((size_t)n == v.size()) {
		do {
			stringstream ss;
			ss << "x" << n;
			if (auto it = m.find(ss.str()); it == m.end()) {
				var_dict(ss.str());
				return ss.str();
			}
			++n;
		} while (true);
	}
	return v[n];
}

sym_t var_dict(const string& s) { return var_dict(s.c_str()); }

} // namespace idni::tau_lang
