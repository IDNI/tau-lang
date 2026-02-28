// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "bv_bitblasting"

namespace idni::tau_lang {

template<NodeType node>
tref bitblast(tref term) { return nullptr; }

template<NodeType node>
tref unbitblast(tref term) { return nullptr; }

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