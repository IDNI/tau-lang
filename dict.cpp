#include "dict.h"
#include <string>
#include <vector>
#include <map>
using namespace std;

vector<string> v;
map<string, size_t> m;

sym_t dict(const char* s) {
	if (auto it = m.find(s); it != m.end()) return it->second;
	return m.emplace(s, v.size()), v.push_back(s), v.size() - 1;
}

const char* dict(sym_t n) { return v[n].c_str(); }
