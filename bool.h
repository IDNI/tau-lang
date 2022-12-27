#ifndef __BOOL_H__
#define __BOOL_H__
#include <ostream>

struct Bool {
	bool b;
	Bool() {}
	Bool(bool b) : b(b) {}
	static const Bool& zero() { static Bool b(false); return b; }
	static const Bool& one() { static Bool b(true); return b; }
	const Bool& operator&(const Bool& t) const {return(b&t.b)?one():zero();}
	const Bool& operator|(const Bool& t) const {return(b|t.b)?one():zero();}
	const Bool& operator~() const { return b ? zero() : one(); }
	bool operator<(const Bool& t) const { return b < t.b; }
	bool operator==(const Bool& t) const { return b == t.b; }
};

ostream& operator<<(ostream& os, const Bool& b) { return os << (b.b?"T":"F"); }
#endif
