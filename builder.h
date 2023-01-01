#include "dict.h"
#include "tau.h"
#include "bool.h"
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

hbdd<Bool> operator|(string x, const hbdd<Bool>& y) {
	bool nx = '\'' == x.back();
	if (nx) x.pop_back();
	return	bdd_handle<Bool>::bit(!nx, dict(x)) | y;
}

hbdd<Bool> operator&(string x, const hbdd<Bool>& y) {
	bool nx = '\'' == x.back();
	if (nx) x.pop_back();
	return	bdd_handle<Bool>::bit(!nx, dict(x)) & y;
}

hbdd<Bool> operator|(const hbdd<Bool>& y, string x) {
	bool nx = '\'' == x.back();
	if (nx) x.pop_back();
	return	bdd_handle<Bool>::bit(!nx, dict(x)) | y;
}

hbdd<Bool> operator&(const hbdd<Bool>& y, string x) {
	bool nx = '\'' == x.back();
	if (nx) x.pop_back();
	return	bdd_handle<Bool>::bit(!nx, dict(x)) & y;
}
