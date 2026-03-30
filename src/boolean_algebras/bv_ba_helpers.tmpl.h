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
tref build_bv_zero([[maybe_unused]] size_t bitwidth) {
	// TODO (HIGH) exclude bf on the top
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

template<NodeType node>
size_t get_bv_type_bitwidth(tref t) {
	auto type = tree<node>::get(t).get_ba_type();
	if (!is_bv_type_family<node>(type)) {
		LOG_ERROR << "Type is not a bitvector type.";
		return 0;
	}
	return get_bv_width<node>(type);
}

template<NodeType node>
std::optional<size_t> get_bv_constant_value(tref t) {
	auto constant = tree<node>::get(t).get_ba_constant();
	auto bv = std::get<bv>(constant);
	if (bv.isBitVectorValue()) {
		auto value_str = bv.getBitVectorValue();
		try {
			size_t value = std::stoull(value_str, nullptr, 2);
			return value;
		} catch (const std::exception& e) {
			LOG_ERROR << "Failed to parse bitvector constant value: " << e.what();
		}
	}
	return std::nullopt;
}

} // namespace idni::tau_lang