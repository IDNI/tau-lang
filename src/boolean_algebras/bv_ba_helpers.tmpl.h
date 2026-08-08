// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/bv_ba.h" // Only for IDE resolution, not really needed.
#include "../parser/bitvector_parser.generated.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_ba_helpers"

namespace idni::tau_lang {

// (BA1-15: is_zero_bv_constant, build_bv_zero, is_bv_lsb_one, bv_shr_by_one
// and bv_shl_by_one were deleted -- zero callers, and two carried latent
// defects (width-1-only zero compare; width+1 string into mkBitVector).
// Recover from git if needed.)

using namespace cvc5;
using namespace idni;

template<NodeType node>
bool is_bv_constant(tref t) {
	using tau = tree<node>;

	DBG( LOG_INFO << "Checking if tref " << LOG_FM_TREE(t) << " is a bitvector constant.\n"; )

	DBG( assert(tau::get(t).is_ba_constant()); )

	auto type = tau::get(t).get_ba_type();
	return is_bv_type_family<node>(type);
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
	auto cte = std::get<bv>(constant);
	if (cte.isBitVectorValue()) {
		auto value_str = cte.getBitVectorValue();
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