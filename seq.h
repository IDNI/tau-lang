#ifndef __SEQ_H__
#define __SEQ_H__
#include "fof.h"
#include <iostream>
#define ever ;;

template<typename B>
fof<B> shr(const fof<B>& f) {
	static auto g = [](sym_t v)->sym_t { return v >= 0 ? v + 1 : v; };
	return transform_vars(f, g);
}

template<typename B>
fof<B> shl(const fof<B>& f) {
	static auto g = [](sym_t v)->sym_t { return v >= 0 ? v - 1 : v; };
	return transform_vars(f, g);
}

template<typename B>
void seq(fof<B> f) {
	vector<fof<B>> v({f});
	size_t n = 0;
	cout << f << endl;
	for (ever) {
//		cout << "f:" << f << endl;
//		cout << "shr(f): " << shr(f) << endl;
//		cout << "f & shr(f): " << (f & shr(f)) << endl;
//		cout << "ex f & shr(f): " << ex(f & shr(f), 0) << endl;
		f = shl(ex(f & shr(f), 0));
		cout << ++n << '\t' << f << endl;
		if (f == fof<B>::zero()) { cout << "unsat" << endl; return; }
		for (size_t k = 0; k != v.size(); ++k)
			if (v[k] == f) {
				cout << "equals iter " << k << endl;
				return;
			}
		v.push_back(f);
	}
}
#endif
