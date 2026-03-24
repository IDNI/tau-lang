// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/bv_ba.h" // Only for IDE resolution, not really needed.
#include "../parser/bitvector_parser.generated.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_ba_hooks"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

template<NodeType node>
bool is_bv_constant([[maybe_unused]] tref t) {
	return false;
}

template<NodeType node>
bool is_zero_bv_constant(tref t) {
	DBG( assert(is_bv_constant<node>(t)); )
	return false;
}

template<NodeType node>
tref make_bitvector_zero([[maybe_unused]] size_t bitwidth) {
	return nullptr;
}

template<NodeType node>
bool lsb(tref t) {
	DBG( assert(is_bv_constant<node>(t)); )
	return false;
}

template<NodeType node>
tref shr_by_one(tref t) {
	DBG( assert(is_bv_constant<node>(t)); )
	return nullptr;
}

template<NodeType node>
tref shl_by_one(tref t) {
	DBG( assert(is_bv_constant<node>(t)); )
	return nullptr;
}

} // namespace idni::tau_lang