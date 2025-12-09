// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

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
	// This automatic simplification is needed in order to not call the
	// ba constant normalization procedure several times
	if (operand.isBitVectorValue()) {
		std::string v = operand.getBitVectorValue(10);
		return cvc5_term_manager.mkBitVector(
			operand.getSort().getBitVectorSize(), ~std::stoll(v));
	}
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

bool operator==(const Term& , const bool& ) { return false; }

bool operator==(const bool& , const Term& ) { return false; }

bool operator!=(const Term& , const bool& ) { return false; }

bool operator!=(const bool& , const Term& ) { return false; }


} // namespace idni::tau_lang