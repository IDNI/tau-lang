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

// Kept in an anonymous namespace: the one-letter names must not collide
// with other translation units.
namespace {
// symbol -> name table; slot 0 is a dummy so real symbols start at 1
vector<string> v({ "dummy" });
// name -> symbol map, the inverse of v
map<string, size_t> m;
} // namespace

/** @internal @copydoc var_dict(const char*) @endinternal */
sym_t var_dict(const char* s) {
	if (auto it = m.find(s); it != m.end()) return static_cast<sym_t>(it->second);
	return m.emplace(s, v.size()), v.push_back(s), static_cast<sym_t>(v.size() - 1);
}

/** @internal @copydoc var_dict(sym_t) @endinternal */
// Returned by value: a pointer into v[n] could dangle after a later
// push_back reallocates the vector.
result<std::string> var_dict(sym_t n) {
	result<std::string> r;
	// A stale or corrupt id must not read out of bounds; report instead.
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
	// The two guards above leave n in range; v is indexed by size_t.
	return r.with_assert_check_value(v[static_cast<size_t>(n)]);
}

/** @internal @copydoc var_dict(const std::string&) @endinternal */
sym_t var_dict(const string& s) { return var_dict(s.c_str()); }

} // namespace idni::tau_lang
