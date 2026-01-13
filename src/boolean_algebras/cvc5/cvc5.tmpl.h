// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/cvc5/cvc5.h" // Only for IDE resolution, not really needed.

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

Term operator|(const Term& lhs, const Term& rhs) {
	return make_bitvector_or(lhs, rhs);
}

Term operator&(const Term& lhs, const Term& rhs) {
	return make_bitvector_and(lhs, rhs);
}

Term operator^(const Term& lhs, const Term& rhs) {
	return make_bitvector_xor(lhs, rhs);
}

Term operator~(const Term& operand) {
	return make_bitvector_not(operand);
}

Term operator+(const Term& lhs, const Term& rhs) {
	return make_bitvector_add(lhs, rhs);
}

Term operator-(const Term& lhs, const Term& rhs) {
	return make_bitvector_sub(lhs, rhs);
}

Term operator*(const Term& lhs, const Term& rhs) {
	return make_bitvector_mul(lhs, rhs);
}

Term operator/(const Term& lhs, const Term& rhs) {
	return make_bitvector_div(lhs, rhs);
}

Term operator%(const Term& lhs, const Term& rhs) {
	return make_bitvector_mod(lhs, rhs);
}

Term operator<<(const Term& lhs, const Term& rhs) {
	return make_bitvector_shl(lhs, rhs);
}

Term operator>>(const Term& lhs, const Term& rhs) {
	return make_bitvector_shr(lhs, rhs);
}

} // namespace idni::tau_lang