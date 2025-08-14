// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "bv_ba.h" // Only for IDE resolution, not really needed.

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

bool is_synthetic_one(const bv& b) {
	return b.isBitVectorValue() && b.getBitVectorValue(2) == "1";
}

bool is_synthetic_zero(const bv& b) {
	return b.isBitVectorValue() && b.getBitVectorValue(2) == "0";
}

size_t get_bv_size(const bv& b) {
	DBG( assert(b.isBitVectorValue()); )
	return b.getSort().getBitVectorSize();
}

template<NodeType node>
size_t get_bv_size(const tref t) {
	using tt = typename tree<node>::traverser;
	if (auto num = tt(t)
			| node::type::type
			| node::type::bv_type
			| node::type::num; num) {
		return tt(num) | tt::only_child | tt::num;
	}
	return bv_default_size;
}

} // namespace idni::tau_lang
