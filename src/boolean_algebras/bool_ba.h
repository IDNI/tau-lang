// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_BA_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_BA_H__

#include "defs.h"

namespace idni::tau_lang {

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

Bool normalize_bool(const Bool& b);
bool is_ba_syntactic_one (const Bool& b);
bool is_ba_syntactic_zero(const Bool& b);
bool is_ba_closed(const Bool&);

std::ostream& operator<<(std::ostream& os, const Bool& b);

} // namespace idni::tau_lang

template<>
struct std::hash<idni::tau_lang::Bool> {
	size_t operator()(const idni::tau_lang::Bool& b) {
		return b.b ? 1 : 0;
	}
};

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_BA_H__
