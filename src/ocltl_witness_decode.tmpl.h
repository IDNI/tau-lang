// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

namespace idni::tau_lang {

template <typename BA, typename Node>
requires ocltl_atomless_ba<BA, Node>
BA ocltl_decode_witness(const std::vector<BA>& a,
	const std::vector<ocltl_target_atom>& atoms, splitter_type st)
{
	return ocltl_witness_wide<BA, Node>(a,
		ocltl_build_target_mask(a.size(), atoms), st);
}

} // namespace idni::tau_lang
