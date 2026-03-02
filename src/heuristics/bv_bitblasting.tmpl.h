// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_bitblasting"

namespace idni::tau_lang {

using bool_node = idni::tau_lang::node<Boolean>;
using bitblast_solution = solution<bool_node>;
static auto bool_id = bool_type_id<bool_node>();

template<NodeType node>
tref basic_bitblast(const typename tree<node>::traverser& term) {
	using tau = tree<node>;
	using tt = tau::traverser;

	auto nt = form | tt::nt;

	trefs children;
	for (auto child : form | tt::children)
		children.push_back(basic_bitblast(child));
	// TODO (HIGH) we need a deep copy og all the fields of the original node,
	// not just the nt and the children
	return bool_node::get(nt, children);
}

// bf_bitblast returns a vector of bits representing the bitblasted term, where
// the i-th element of the vector corresponds to the i-th bit of the original
// term (0-indexed from the most significant bit).
template<NodeType node>
std::optional<trefs> bf_bitblast(const typename tree<node>::traverser& term) {
	using tau = tree<node>;
	using tt = tau::traverser;

	static auto _one = bool_node::get_typed(bool_node::bf_t, bool_id);
	static auto zero = bool_node::get_typed(bool_node::bf_f, bool_id);

	auto nt = form | tt::nt;
	auto ref = form | tt::ref;
	auto node = tau::get(ref);
	auto type = node.get_ba_type();
	auto bv_size = get_bv_size<node>(type);

	switch (nt) {
		case tau::bf_add: case tau::bf_sub: case tau::bf_mul:
		case tau::bf_div: case tau::bf_mod: case tau::bf_shl:
		case tau::bf_shr: {
			// Unsupported operations for now
			LOG_ERROR << "Arithmetic operations are not supported in bitblasting yet.";
			return std::nullopt;
		}
		case tau::bf {
			return bool_node::get(bool_node::bf, bf_bitblast(form | tt::first));
		}
		case tau::bf_neg: {
			auto bits = bf_bitblast(form | tt::first);
			if (!bits) return std::nullopt;
			trefs result;
			for (size_t i = 0; i < bv_size; ++i) {
				result.push_back(bool_node::get(bool_node::bf_neg, bit));
			}
			return result;
		}
		case tau::bf_and: case tau::bf_nand: case tau::bf_or:
		case tau::bf_nor: case tau::bf_xor:	case tau::bf_xnor: {
			auto left_bits = bf_bitblast(form | tt::first);
			auto right_bits = bf_bitblast(form | tt::second);
			if (!left_bits || !right_bits) return std::nullopt;
			trefs result;
			for (size_t i = 0; i < bv_size; ++i)
				result.push_back(bool_node::get(nt, left_bits.value()[i], right_bits.value()[i]));
			return result;
		}
		case tau::bf_t:	case tau::bf_f: {
			trefs result;
			for (size_t i = 0; i < bv_size; ++i)
				result.push_back(nt == bool_node::bf_t ? _one : zero);
				return result;
			}
		case tau::ba_constant: {
			auto cte = form | tt::ba_constant;
			DBG(assert(std::holds_alternative<bv>(cte));)
			auto str = std::get<bv>(cte).getBitVectorValue(2);
			trefs result;
			for (size_t i = 0; i < bv_size; ++i)
				result.push_back(str[i] == '1' ? _one : zero);
			return result;
		}
		case tau::variable: case tau::ctnvar: {
			auto booled = basic_bitblast(form);
			auto booled_node = bool_node::get(bool_node::variable, booled);
			trefs result;
			auto bit = booled_node.retype(bool_id);
			for (size_t i = 0; i < bv_size; ++i) {
				result.push_back(bool_node::add_child(bit,
					bool_node::get(bool_node::offset, bool_node::get_num(i))));
			}
			return result;
		}
		default:
			// Unsupported operations for now
			LOG_ERROR << "Unsupported operation in bitblasting: " << LOG_FM(form);
			return std::nullopt;
	}
}

template<NodeType node>
tref wff_bitblast(const typename tree<node>::traverser& term) { return nullptr; }

template<NodeType node>
std::optional<tref> bitblast(tref term) { return std::nullopt; }

template<NodeType node>
std::optional<solution<node>> unbitblast(bitblast_solution& solution) { return nullptr; }

template<NodeType node>
std::optional<solution<node>> bv_bitblasting_solve(tref term) {
	return bitblast<node>(term);
}

template<NodeType node>
bool bv_bitblasting_sat(tref term) {
	return bitblast<node>(term);
}

template<NodeType node>
bool bv_bitblasting_unsat(tref term) {
	return bitblast<node>(term);
}

template<NodeType node>
bool bv_bitblasting_valid(tref term) {
	return bitblast<node>(term);
}


} // namespace idni::tau_lang