// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cassert>
#include <stdexcept>

#include "var_dict.h"

using namespace std;

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// bdd var dict

// BA1-26: anonymous namespace -- these had external linkage under one-letter
// names in a shared namespace, an ODR/link-collision hazard.
namespace {
// symbol -> name table; slot 0 is a dummy so real symbols start at 1
vector<string> v({ "dummy" });
// name -> symbol map, the inverse of v
map<string, size_t> m;
} // namespace

// Intern name s: return its symbol, appending a new entry to v/m the
// first time the name is seen
sym_t var_dict(const char* s) {
	if (auto it = m.find(s); it != m.end()) return it->second;
	return m.emplace(s, v.size()), v.push_back(s), v.size() - 1;
}

// Returned by value: a pointer into the function-local static buffer this
// used to return got overwritten by the next auto-generated-name call, and
// a pointer into v[n] could dangle after a later push_back reallocates the
// vector (v[n]'s short-string-optimized buffer lives inside the string
// object, which push_back may move to a new address).
result<std::string> var_dict(sym_t n) {
	result<std::string> r;
	// BA1-26: a stale/corrupt id must not read OOB; report instead.
	if ((size_t)n > v.size())
		return r.with_assert_check_error(code::out_of_range,
			"the variable id is invalid",
			{{label::actual, (size_t)n}, {label::limit, v.size()}});
	if ((size_t)n == v.size()) {
		do {
			stringstream ss;
			ss << "x" << n;
			if (auto it = m.find(ss.str()); it == m.end()) {
				var_dict(ss.str());
				return r.with_assert_check_value(ss.str());
			}
			++n;
		} while (true);
	}
	return r.with_assert_check_value(v[n]);
}

// std::string convenience overload of the interning function above
sym_t var_dict(const string& s) { return var_dict(s.c_str()); }

} // namespace idni::tau_lang
