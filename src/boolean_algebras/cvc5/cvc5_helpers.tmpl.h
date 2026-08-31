// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/cvc5/cvc5.h" // Only for IDE resolution, not really needed.

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "cvc5"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

// Bit width of the sort of bitvector value `b` (declared in cvc5.h).
// Precondition (DBG-asserted): b.isBitVectorValue() — in release builds a
// non-bitvector sort makes getBitVectorSize() throw.
inline size_t get_cvc5_size(const Term& b) {
	DBG( assert(b.isBitVectorValue()); )
	return b.getSort().getBitVectorSize();
}

} // namespace idni::tau_lang
