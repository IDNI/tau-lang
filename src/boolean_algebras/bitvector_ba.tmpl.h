// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "bitvector_ba.h"

namespace idni::tau_lang {


bitvector_ba::bitvector_ba(expr e, const unsigned& size) : size(size), e(e) {}


bitvector_ba bitvector_ba::operator&(const bitvector_ba& x) const {
	return bitvector_ba(e & x.e, size);
}

bitvector_ba bitvector_ba::operator|(const bitvector_ba& x) const {
	return bitvector_ba(e | x.e, size);
}

bitvector_ba bitvector_ba::operator^(const bitvector_ba& x) const {
	return bitvector_ba(e ^x.e, size);
}

bitvector_ba bitvector_ba::operator+(const bitvector_ba& x) const {
	return bitvector_ba(e ^ x.e, size);
}

bitvector_ba bitvector_ba::operator~() const {
	return bitvector_ba(!e, size);
}


} // namespace idni::tau_lang