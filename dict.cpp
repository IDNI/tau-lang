#include "dict.h"
#include <string>
#include <vector>
#include <map>
#include <sstream>
using namespace std;

vector<string> v({"dummy"});
map<string, size_t> m;

sym_t dict(const char* s) {
	if (auto it = m.find(s); it != m.end()) return it->second;
	return m.emplace(s, v.size()), v.push_back(s), v.size() - 1;
}

const char* dict(sym_t n) {
	if ((size_t)n >= v.size()) {
		static string tmp;
		stringstream ss;
		ss << "x[" << n << "]";
		return (tmp = ss.str()).c_str();
	}
	return v[n].c_str();
}

sym_t dict(const string& s) { return dict(s.c_str()); }
