#include "dict.h"
#include "seq.h"
#include <cstring>
using namespace std::literals;

minterm<Bool> operator ""_v(const char* s, size_t n) {
	bool neg = s[n] == '\'';
	char* k = (char*)alloca(n + 1);
	strcpy(k, s);
	if (neg) k[n] = 0;
	sym_t t = dict(k);
	return minterm<Bool>(!neg, term<Bool>(t));
}

fof<Bool> operator<<=(const bf<Bool>& f, int n) {
	assert(n == 0 || n == 1);
	return fof<Bool>(term<bf<Bool>>(n ? ~f : f));
}

fof<Bool> operator<<(const bf<Bool>& f, int n) {
	assert(n == 0 || n == 1);
	return fof<Bool>(clause<Bool>(false, term<bf<Bool>>(n ? ~f : f)));
}

template<typename B> fof<B> ex(const fof<B>& f, string s) {
	return ex(f, dict(s.c_str()));
}

template<typename B> fof<B> all(const fof<B>& f, string s) {
	return all(f, dict(s.c_str()));
}

ostream& operator<<(ostream& os, const term<Bool>& t) {
	if (t.t == term<Bool>::ELEM) return os << t.e;
	if (t.t == term<Bool>::VAR) return os << dict(t.sym);
	if (t.t == term<Bool>::BF) return os << t.f;
	if (t.t == term<Bool>::FUNC) os << t.name << "(";//"f[" << t.sym << "](";
	for (size_t n = 0; n != t.args.size(); ++n) {
		out<Bool>(os, t.args[n]);
		//os << t.args[n]; -- compiler error somehow
		os << (n == t.args.size() - 1 ? "" : ",");
	}
	return os << ")";
}
