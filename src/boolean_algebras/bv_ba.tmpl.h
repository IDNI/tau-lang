// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "bv_ba.h" // Only for IDE resolution, not really needed.

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

bv operator|(const bv& lhs, const bv& rhs) {
	return make_bv_or(lhs, rhs);
}

bv operator&(const bv& lhs, const bv& rhs) {
	return make_bv_and(lhs, rhs);
}

bv operator^(const bv& lhs, const bv& rhs) {
	return make_bv_xor(lhs, rhs);
}

bv operator~(const bv& operand) {
	return make_bv_not(operand);
}

bv operator+(const bv& lhs, const bv& rhs) {
	return make_bv_add(lhs, rhs);
}

bv operator-(const bv& lhs, const bv& rhs) {
	return make_bv_sub(lhs, rhs);
}

bv operator*(const bv& lhs, const bv& rhs) {
	return make_bv_mul(lhs, rhs);
}

bv operator/(const bv& lhs, const bv& rhs) {
	return make_bv_div(lhs, rhs);
}

bv operator%(const bv& lhs, const bv& rhs) {
	return make_bv_mod(lhs, rhs);
}

bv operator<<(const bv& lhs, const bv& rhs) {
	return make_bv_shl(lhs, rhs);
}

bv operator>>(const bv& lhs, const bv& rhs) {
	return make_bv_shr(lhs, rhs);
}

} // namespace idni::tau_lang