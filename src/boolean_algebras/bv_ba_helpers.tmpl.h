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

// True iff the constant held by `t` is a concrete bitvector value whose
// base-2 string prints exactly as "0". Preconditions: is_ba_constant()
// (DBG-asserted) and the constant variant must hold a `bv` (std::get
// throws otherwise). Returns false for non-value terms (e.g. symbolic).
template<NodeType node>
bool is_zero_bv_constant(tref t) {
	using tau = tree<node>;




// Build a BA-constant tree node holding the all-zero bitvector of
// `bitwidth` bits, typed with the bv[bitwidth] type id.
template<NodeType node>
tref build_bv_zero(size_t bitwidth) {
	using tau = tree<node>;

	auto zero = make_bitvector_value(bitwidth, 0);
	auto type = bv_type_id<node>(bitwidth);
	return tau::get_ba_constant(zero, type);
}

// True iff the least significant bit of the bitvector constant in `t` is
// 1 (last character of its base-2 string). Preconditions: is_ba_constant()
// (DBG-asserted) and the constant must hold a `bv`. Returns false when the
// term is not a concrete bitvector value.
template<NodeType node>
bool is_bv_lsb_one(tref t) {
	using tau = tree<node>;

	DBG( assert(tau::get(t).is_ba_constant()); )

	auto constant = tau::get(t).get_ba_constant();
	auto cte = std::get<bv>(constant);
	if (!cte.isBitVectorValue()) return false;
	std::string bv_str = cte.getBitVectorValue();
	return !bv_str.empty() && bv_str.back() == '1';
}

// Logical right shift by one of the bitvector constant in `t`: drops the
// last base-2 digit and rebuilds a BA constant of the same type (an empty
// result becomes "0"). Precondition (DBG-asserted): is_ba_constant(), and
// the constant must hold a `bv`. Returns nullptr when the term is not a
// concrete bitvector value or its base-2 string is empty.
template<NodeType node>
tref bv_shr_by_one(tref t) {
	using tau = tree<node>;

	DBG( assert(tau::get(t).is_ba_constant()); )

	auto type = tau::get(t).get_ba_type();
	auto bitwidth = get_bv_width<node>(type);
	auto constant = tau::get(t).get_ba_constant();
	auto cte = std::get<bv>(constant);
	if (!cte.isBitVectorValue()) return nullptr;
	std::string bv_str = cte.getBitVectorValue();
	if (bv_str.empty()) return nullptr;
	bv_str.pop_back();
	auto shifted_bv_str = bv_str.empty() ? "0" : bv_str;
	auto shifted_bv = make_bitvector_value(bitwidth, shifted_bv_str);
	typename node::constant new_cte = {shifted_bv};
	auto new_constant = tau::get_ba_constant(new_cte, type);
	return new_constant;
}

// Left shift by one of the bitvector constant in `t`: appends a '0' to
// its base-2 string and rebuilds a BA constant of the same type. The
// string is NOT truncated to the width, so the caller must ensure the
// shifted value still fits (cvc5 rejects a constant wider than its size).
// Precondition (DBG-asserted): is_ba_constant(), and the constant must
// hold a `bv`. Returns nullptr when the term is not a concrete bitvector
// value or its base-2 string is empty.
template<NodeType node>
tref bv_shl_by_one(tref t) {
	using tau = tree<node>;

	DBG( assert(tau::get(t).is_ba_constant()); )

	auto type = tau::get(t).get_ba_type();
	auto bitwidth = get_bv_width<node>(type);
	auto constant = tau::get(t).get_ba_constant();
	auto cte = std::get<bv>(constant);
	if (!cte.isBitVectorValue()) return nullptr;
	std::string bv_str = cte.getBitVectorValue();
	if (bv_str.empty()) return nullptr;
	bv_str.push_back('0');
	auto shifted_bv = make_bitvector_value(bitwidth, bv_str);
	typename node::constant new_cte = {shifted_bv};
	auto new_constant = tau::get_ba_constant(new_cte, type);
	return new_constant;
}

// Bit width of the BA type of node `t`. No DBG precondition; if the type
// is not in the bv family this logs an error and returns 0 (0 is never a
// valid bv width, so it doubles as the failure value).
template<NodeType node>
size_t get_bv_type_bitwidth(tref t) {
	auto type = tree<node>::get(t).get_ba_type();
	if (!is_bv_type_family<node>(type)) {
		LOG_ERROR << "Type is not a bitvector type.";
		return 0;
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