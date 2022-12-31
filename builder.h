#include "dict.h"
#include "seq.h"
#include "bool.h"
#include <cstring>
using namespace std::literals;

bf<bf<Bool>> operator ""_v(const char* s, size_t n) {
	bool neg = s[n-1] == '\'';
	char* k = (char*)alloca(n + 1);
	strcpy(k, s);
	if (neg) k[n - 1] = 0;
	sym_t t = dict(k);
	return minterm<bf<Bool>>(!neg, term<bf<Bool>>(t));
}

fof<bf<Bool>> operator<<=(const bf<bf<Bool>>& f, int n) { // eq
	assert(n == 0 || n == 1);
	return fof<bf<Bool>>(term<bf<bf<Bool>>>(n ? ~f : f));
}

fof<bf<Bool>> operator<<(const bf<bf<Bool>>& f, int n) { // neq
	assert(n == 0 || n == 1);
	return fof<bf<Bool>>(clause<bf<Bool>>(false, term<bf<bf<Bool>>>(n ? ~f : f)));
}

template<typename B> fof<B> ex(const string& x, const fof<B>& f) {
	return ex(f, dict(x.c_str()));
}

template<typename B> fof<B> all(const string& x, const fof<B>& f) {
	return all(f, dict(x.c_str()));
}

ostream& operator<<(ostream& os, const term<bf<Bool>>& t) {
	if (t.t == term<bf<Bool>>::ELEM) return os << t.e;
	if (t.t == term<bf<Bool>>::VAR) return os << dict(t.sym);
	if (t.t == term<bf<Bool>>::BF) return os << t.f;
	if (t.t == term<bf<Bool>>::FUNC) os << t.name << "(";//"f[" << t.sym << "](";
	for (size_t n = 0; n != t.args.size(); ++n) {
		out<bf<Bool>>(os, t.args[n]);
		//os << t.args[n]; -- compiler error somehow
		os << (n == t.args.size() - 1 ? "" : ",");
	}
	return os << ")";
}
