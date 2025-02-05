// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __BITVECTOR_BA_H__
#define __BITVECTOR_BA_H__

#include "z3++.h"

using namespace z3;

namespace idni::tau_lang {

struct bitvector_ba {

	bitvector_ba(expr e, const unsigned& size);

	bitvector_ba operator&(const bitvector_ba& x) const;
	bitvector_ba operator|(const bitvector_ba& x) const;
	bitvector_ba operator^(const bitvector_ba& x) const;
	bitvector_ba operator+(const bitvector_ba& x) const;
	bitvector_ba operator~() const;

	auto operator<=>(const bitvector_ba& x) const = default;

	static bitvector_ba from(const int& i, const unsigned& size);
	static bitvector_ba from(const unsigned& n, const bool* bits);

	bool is_zero() const;
	bool is_one() const;

private:
	static context ctx;
	unsigned size;
	expr e;
};

bool operator==(const bitvector_ba& x, const bool& b);
bool operator==(const bool& b, const bitvector_ba& x);
bool operator!=(const bitvector_ba& x, const bool& b);
bool operator!=(const bool& b, const bitvector_ba& x);

} // namespace idni::tau_lang

std::ostream& operator<<(std::ostream& os, const idni::tau_lang::bitvector_ba& x);

#include "bitvector_ba.tmpl.h"
#endif // __BITVECTOR_BA_H__