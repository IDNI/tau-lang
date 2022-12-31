#ifndef __SEQ_H__
#define __SEQ_H__
#include "fof.h"
#include <iostream>
#define ever ;;

template<typename B>
using f2f = function<fof<bf<B>>(const fof<bf<B>>&)>;

template<typename B> size_t seq(const fof<bf<B>>& f, f2f<B> init, f2f<B> F) {
	vector<fof<B>> v({init(f)});
	for (ever) {
		if (v.back() == fof<B>::zero()) return 0;
		v.push_back(F(v.back()));
		cout << v.size() << '\t' << v.back() << endl;
		for (size_t k = 0; k != v.size() - 1; ++k)
			if (v[k] == v.back()) {
				cout << "equals iter " << k << endl;
				return v.size();
			}
	}
}

template<typename B> size_t seq(const fof<bf<B>>& f, f2f<B> F) {
	return seq<B>(f, [](const fof<bf<B>>& f){ return f; }, F);
}

template<typename B> void seq(fof<B> f) {
	f2f<B> init = [](const fof<B>& g) { return ex(g, 0); };
	f2f<B> F = [](const fof<B>& g) { return shl(ex(ex(g & shr(g), 0), 1)); };
	seq(f, F);
	//seq(f, init, F);
	return;
	vector<fof<B>> v({f});
	size_t n = 0;
	cout << f << endl;
	for (ever) {
//		cout << "f:" << f << endl;
//		cout << "shr(f): " << shr(f) << endl;
//		cout << "f & shr(f): " << (f & shr(f)) << endl;
//		cout << "ex f & shr(f): " << ex(f & shr(f), 0) << endl;
		f = shl(ex(ex(f & shr(f), 0), 1));
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
