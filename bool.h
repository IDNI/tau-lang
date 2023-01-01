#ifndef __BOOL_H__
#define __BOOL_H__
struct Bool {
	bool b;
	Bool(bool b) : b(b) {}
	template<typename T> Bool(const T&) {
		static_assert(is_same<T, bool>::value);
	}
	static const Bool& zero() { static Bool b(false); return b; }
	static const Bool& one() { static Bool b(true); return b; }
	Bool operator&(const Bool& x) const;
	Bool operator|(const Bool& x) const;
	Bool operator~() const;
	bool operator<(const Bool& x) const { return b < x.b; }
	bool operator==(bool t) const { return b == t; }
};

Bool Bool::operator&(const Bool& x) const {
	return (*this == false) ? zero() : x;
}
Bool Bool::operator|(const Bool& x) const {
	return (*this == true) ? one() : x;
}

Bool Bool::operator~() const { return *this == true ? zero() : one(); }
#endif
