#include "dict.h"
#include "babdd.h"
#include "tau.h"
#include <cstring>

hbdd<Bool> operator|(string x, string y) {
	bool nx = '\'' == x.back();
	bool ny = '\'' == y.back();
	if (nx) x.pop_back();
	if (ny) y.pop_back();
	return	bdd_handle<Bool>::bit(!nx, dict(x)) |
		bdd_handle<Bool>::bit(!ny, dict(y));
}

hbdd<Bool> operator&(string x, string y) {
	bool nx = '\'' == x.back();
	bool ny = '\'' == y.back();
	if (nx) x.pop_back();
	if (ny) y.pop_back();
	return	bdd_handle<Bool>::bit(!nx, dict(x)) &
		bdd_handle<Bool>::bit(!ny, dict(y));
}

//template<typename T, typename... BAs> tau<BAs...> operator&&(const hbdd<B>
