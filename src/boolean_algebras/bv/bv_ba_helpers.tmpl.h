// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/bv/bv_ba.h" // Only for IDE resolution, not really needed.
#include "boolean_algebras/bv/parser/bitvector_parser.generated.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_ba_helpers"

namespace idni::tau_lang {

// (BA1-15: is_zero_bv_constant, build_bv_zero, is_bv_lsb_one, bv_shr_by_one
// and bv_shl_by_one were deleted -- zero callers, and two carried latent
// defects (width-1-only zero compare; width+1 string into mkBitVector).
// Recover from git if needed.)

using namespace cvc5;
using namespace idni;

// True iff the BA type of constant node `t` belongs to the bv family.
// Precondition (DBG-asserted): tau::get(t).is_ba_constant(). Checks the
// type only; it does not inspect the stored constant value.
template<NodeType node>
bool is_bv_constant(tref t) {
	using tau = tree<node>;

	DBG( LOG_INFO << "Checking if tref " << LOG_FM_TREE(t) << " is a bitvector constant.\n"; )

	DBG( assert(tau::get(t).is_ba_constant()); )

	auto type = tau::get(t).get_ba_type();
	return is_bv_type_family<node>(type);
}






// Bit width of the BA type of node `t`. No DBG precondition; reports when
// the type is not in the bv family or carries no explicit bitwidth.
template<NodeType node>
result<size_t> get_bv_type_bitwidth(tref t) {
	auto type = tree<node>::get(t).get_ba_type();
	if (!is_bv_type_family<node>(type)) {
		result<size_t> r;
		return r.with_assert_check_error(code::type_error,
			"get_bv_type_bitwidth: type is not a bitvector type");
	}
	return get_bv_width<node>(type);
}

// Numeric (unsigned) value of the bitvector constant in `t`, parsed from
// its base-2 string. Precondition: `t` is a BA constant holding a `bv`
// (std::get throws otherwise; not DBG-asserted here). Returns nullopt if
// the term is not a concrete bitvector value or the value does not fit
// in size_t (the stoull failure is logged).
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