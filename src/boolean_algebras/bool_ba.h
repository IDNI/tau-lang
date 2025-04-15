// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_BA_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_BA_H__

#include "defs.h"

struct Bool {
	Bool();
	Bool(bool b);

	static const Bool& zero();
	static const Bool& one();

	Bool operator&(const Bool& x) const;
	Bool operator|(const Bool& x) const;
	Bool operator^(const Bool& x) const;
	Bool operator+(const Bool& x) const;
	Bool operator~() const;
	auto operator<=>(const Bool& x) const = default;

	bool is_zero() const;
	bool is_one() const;

	bool b;
};

Bool normalize (const Bool& b);
bool is_syntactic_one (const Bool& b);
bool is_syntactic_zero(const Bool& b);


template<>
struct std::hash<Bool> {
	size_t operator()(const Bool& b) {
		return b.b ? 1 : 0;
	}
};

std::ostream& operator<<(std::ostream& os, const Bool& b);

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_BA_H__
