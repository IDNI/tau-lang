// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt
#ifndef __DEF_H__
#define __DEF_H__

#define TAU_VERSION "0.7"
#include "version_license.h"

// basic macro for conditional execution of code
#ifdef DEBUG
#	define DBG(x) x
#	include <cxxabi.h>
#else
#define DBG(x)
#endif

#include <algorithm>
#include <iostream>
#include <vector>

#define pfst(x) (*(x).begin())
#define hasbc(x, y, f) std::binary_search(x.begin(), x.end(), y, f)

typedef int sym_t;

template<typename B> struct zero {
	bool operator()(const B&) const;
};

template<typename B> struct one {
	bool operator()(const B&) const;
};

template<typename T, typename V> bool has(const T& t, const V& v) {
	return t.find(v) != t.end();
}

template<typename T, typename V> bool hasv(const T& t, const V& v) {
	return std::find(t.begin(), t.end(), v) != t.end();
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
	os << "[";
	for (size_t i=0; i < vec.size(); ++i)
		if (i+1 < vec.size()) os << vec[i] << ",";
	if (!vec.empty()) os << vec.back();
	os << "]";
	return os;
}
//-----------------------------------------------------------------------------
// GIT_* macros are populated at compile time by -D or they're set to "n/a"
#ifndef GIT_DESCRIBED
#define GIT_DESCRIBED   "n/a"
#endif
#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH "n/a"
#endif
#ifndef GIT_BRANCH
#define GIT_BRANCH      "n/a"
#endif

#endif // __DEF_H__
