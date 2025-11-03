// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

namespace idni::tau_lang {

std::vector<std::string> nso_factory<bv, sbf_ba>::types() {
	return { "sbf", "bv" };
}

tref nso_factory<bv, sbf_ba>::default_type() {
	return sbf_type<node<bv,sbf_ba>>();
}

std::string nso_factory<bv, sbf_ba>::one(const tref type_tree) {
	using node_t = node<bv, sbf_ba>;
	if (is_bv_type_family<node_t>(type_tree))
		return make_bitvector_one(
			get_bv_size<node_t>(type_tree)).toString();
	else return "1";
}

std::string nso_factory<bv, sbf_ba>::zero(const tref type_tree) {
	using node_t = node<bv, sbf_ba>;
	if (is_bv_type_family<node_t>(type_tree))
		return make_bitvector_one(
			get_bv_size<node_t>(type_tree)).toString();
	return "0";
}

tref nso_factory<bv, sbf_ba>::splitter_one() {
	using tau = tree<node<bv, sbf_ba>>;
	return tau::get(tau::bf, tau::get_ba_constant(
		sbf_ba_factory<bv, sbf_ba>::splitter_one(),
		sbf_type<node<bv, sbf_ba>>()));
}

tref nso_factory<bv, sbf_ba>::unpack_tau_ba(const std::variant<bv, sbf_ba>&) {
	return nullptr; // There is no tau_ba present
}

std::variant<bv, sbf_ba> nso_factory<bv, sbf_ba>::pack_tau_ba(
	tref) {
	// There is no tau_ba
	return {};
}

std::vector<std::string> nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::types() {
	return { "sbf", "tau", "bv" };
}

tref nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::default_type() {
	return tau_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>();
}

std::string nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::one(
	tref type_tree)
{
	using node_t = node<tau_ba<bv, sbf_ba>, bv, sbf_ba>;
	if (is_sbf_type<node_t>(type_tree)) {
		return "1";
	} else if (is_bv_type_family<node_t>(type_tree)) {
		return make_bitvector_one(
			get_bv_size<node_t>(type_tree)).toString();
	} else return "T";
}

std::string nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::zero(
	tref type_tree)
{
	using node_t = node<tau_ba<bv, sbf_ba>, bv, sbf_ba>;
	if (is_sbf_type<node_t>(type_tree)) {
		return "0";
	} else if (is_bv_type_family<node_t>(type_tree)) {
		return make_bitvector_zero(
			get_bv_size<node_t>(type_tree)).toString();
	} else return "F";
}

tref nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::splitter_one(
		tref type_tree)
{
	using node_t = node<tau_ba<bv, sbf_ba>, bv, sbf_ba>;
	using tau = tree<node_t>;
	DBG(assert(!is_bv_type_family<node_t>(type_tree)));
	return is_sbf_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>(type_tree)
		? tau::get(tau::bf,
			tau::get_ba_constant(
				sbf_ba_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::splitter_one(),
				sbf_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>()))
		: tau::get(tau::bf,
			tau::get_ba_constant(
				tau_ba_factory<bv, sbf_ba>::splitter_one(),
				tau_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>()));
}

tref nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::unpack_tau_ba(
		const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& v)
{
	if (!std::holds_alternative<tau_ba<bv, sbf_ba>>(v)) return {};
	const auto unpacked = std::get<tau_ba<bv, sbf_ba>>(v);
	return unpacked.nso_rr.main->get();
}

inline std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> nso_factory<tau_ba<bv, sbf_ba>,
	bv, sbf_ba>::pack_tau_ba(tref c) {
	tau_ba<bv, sbf_ba> t {c};
	return {t};
}

} // namespace idni::tau_lang