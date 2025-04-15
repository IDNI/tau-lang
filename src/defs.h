// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__DEF_H__
#define __IDNI__TAU__DEF_H__

#include "version_license.h"

#include "../external/parser/src/defs.h"

#ifdef DEBUG
#	include <cxxabi.h>
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

#endif // __IDNI__TAU__DEF_H__
