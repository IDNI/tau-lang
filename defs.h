#ifndef __DEF_H__
#define __DEF_H__
typedef int sym_t;
//#define DEBUG

#ifdef DEBUG
#define DBG(x) x
#include <iostream>
#else
#include <iostream>
#define DBG(x)
#endif

#define pfst(x) (*(x).begin())
using namespace std;

template<typename T, typename V> bool has(const T& t, const V& v) {
	return t.find(v) != t.end();
}

template<typename T, typename V> bool hasv(const T& t, const V& v) {
	return std::find(t.begin(), t.end(), v) != t.end();
}
#endif
