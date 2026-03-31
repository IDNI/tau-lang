// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "boolean_algebras/bv_ba.h" // Only for IDE resolution, not really needed.
#include "../parser/bitvector_parser.generated.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_ba_helpers"

namespace idni::tau_lang {

using namespace cvc5;
using namespace idni;

template<NodeType node>
bool is_bv_constant(tref t) {
	using tau = tree<node>;

	DBG( assert(tau::get(t).is_bv_constant()); )

	auto type = tau::get(t).get_ba_type();
	return is_bv_type_family<node>(type);
}

template<NodeType node>
bool is_zero_bv_constant(tref t) {
	using tau = tree<node>;

	DBG( assert(tau::get(t).is_bv_constant()); )

	auto constant = tau::get(t).get_ba_constant();
	auto bv = std::get<bv>(constant);
	if (!bv.isBitVectorValue()) return false;
    std::string bv_str = bv.getBitVectorValue();
    return bv_str == "0";
}

template<NodeType node>
tref build_bv_zero(size_t bitwidth) {
	using tau = tree<node>;

	auto zero = make_bitvector_value(0, bitwidth);
	auto type = bv_type_id<node>(bitwidth);
	return tau::get_ba_constant(zero, type);
}

template<NodeType node>
bool is_bv_lsb_one(tref t) {
	using tau = tree<node>;

	DBG( assert(tau::get(t).is_bv_constant()); )

	auto constant = tau::get(t).get_ba_constant();
	auto bv = std::get<bv>(constant);
	if (!bv.isBitVectorValue()) return false;
	std::string bv_str = bv.getBitVectorValue();
	return !bv_str.empty() && bv_str.back() == '1';
}

template<NodeType node>
tref bv_shr_by_one(tref t) {
	using tau = tree<node>;

	DBG( assert(tau::get(t).is_bv_constant()); )

	auto type = tau::get(t).get_ba_type();
	auto bitwidth = get_bv_width<node>(type);
	auto constant = tau::get(t).get_ba_constant();
	auto bv = std::get<bv>(constant);
	if (!bv.isBitVectorValue()) return nullptr;
	std::string bv_str = bv.getBitVectorValue();
	if (bv_str.empty()) return nullptr;
	bv_str.pop_back();
	auto shifted_bv_str = bv_str.empty() ? "0" : bv_str;
	auto shifted_bv = make_bitvector_value(bitwidth, shifted_bv_str);
	auto new_constant = tau::get_bv_constant(shifted_bv);
	return new_constant;
}

template<NodeType node>
tref bv_shl_by_one(tref t) {
	using tau = tree<node>;

	DBG( assert(tau::get(t).is_bv_constant()); )

	auto type = tau::get(t).get_ba_type();
	auto bitwidth = get_bv_width<node>(type);
	auto constant = tau::get(t).get_ba_constant();
	auto bv = std::get<bv>(constant);
	if (!bv.isBitVectorValue()) return nullptr;
	std::string bv_str = bv.getBitVectorValue();
	if (bv_str.empty()) return nullptr;
	bv_str.push_back('0');
	auto shifted_bv = make_bitvector_value(bitwidth, bv_str);
	auto new_constant = tau::get_bv_constant(shifted_bv);
	return new_constant;
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