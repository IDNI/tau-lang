// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/cvc5/cvc5.h" // Only for IDE resolution, not really needed.

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

size_t get_Term_size(const Term& b) {
	DBG( assert(b.isBitVectorValue()); )
	return b.getSort().getBitVectorSize();
}

} // namespace idni::tau_lang
