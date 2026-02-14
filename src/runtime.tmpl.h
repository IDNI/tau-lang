// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

namespace idni::tau_lang {
inline std::vector<std::string> nso_factory<bv, sbf_ba>::types() {
	return { "sbf", "bv" };
}

inline tref nso_factory<bv, sbf_ba>::default_type() {
	return sbf_type<node<bv,sbf_ba>>();
}

inline std::string nso_factory<bv, sbf_ba>::one(const tref type_tree) {
	using node_t = node<bv, sbf_ba>;
	if (is_bv_type_family<node_t>(type_tree))
		return make_bitvector_top_elem(get_bv_size<node_t>(type_tree)
			).getBitVectorValue(10);
	else return "1";
}

inline std::string nso_factory<bv, sbf_ba>::zero(const tref type_tree) {
	using node_t = node<bv, sbf_ba>;
	if (is_bv_type_family<node_t>(type_tree))
		return make_bitvector_bottom_elem(get_bv_size<node_t>(type_tree)
			).getBitVectorValue(10);
	return "0";
}

inline tref nso_factory<bv, sbf_ba>::splitter_one() {
	using tau = tree<node<bv, sbf_ba>>;
	return tau::get(tau::bf, tau::get_ba_constant(
		sbf_ba_factory<bv, sbf_ba>::splitter_one(),
		sbf_type<node<bv, sbf_ba>>()));
}

inline tref nso_factory<bv, sbf_ba>::unpack_tau_ba(const std::variant<bv, sbf_ba>&) {
	return nullptr; // There is no tau_ba present
}

inline std::variant<bv, sbf_ba> nso_factory<bv, sbf_ba>::pack_tau_ba(
	tref) {
	// There is no tau_ba
	return {};
}

inline std::variant<bv, sbf_ba> nso_factory<bv, sbf_ba>::normalize(const std::variant<bv, sbf_ba>& v) {
	return std::holds_alternative<bv>(v)
		? std::variant<bv, sbf_ba>(normalize_bv(std::get<bv>(v)))
		: std::variant<bv, sbf_ba>(normalize_sbf(std::get<sbf_ba>(v)));
}

inline tref nso_factory<bv, sbf_ba>::simplify_symbol(tref symbol) {
	auto ba_type = tau::get(symbol).get_ba_type();
	return is_bv_type_family<node_t>(ba_type) ? simplify_bv_symbol<node_t>(symbol) : symbol;
}

inline tref nso_factory<bv, sbf_ba>::simplify_term(tref term) {
	auto ba_type = tau::get(term).get_ba_type();
	return is_bv_type_family<node_t>(ba_type) ? simplify_bv_term<node_t>(term) : term;
}

inline std::vector<std::string> nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::types() {
	return { "sbf", "tau", "bv" };
}

inline tref nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::default_type() {
	return tau_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>();
}

inline std::string nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::one(
	tref type_tree)
{
	using node_t = node<tau_ba<bv, sbf_ba>, bv, sbf_ba>;
	if (is_sbf_type<node_t>(type_tree)) {
		return "1";
	} else if (is_bv_type_family<node_t>(type_tree)) {
		return make_bitvector_top_elem(get_bv_size<node_t>(type_tree)
			).getBitVectorValue(10);
	} else return "T";
}

inline std::string nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::zero(
	tref type_tree)
{
	using node_t = node<tau_ba<bv, sbf_ba>, bv, sbf_ba>;
	if (is_sbf_type<node_t>(type_tree)) {
		return "0";
	} else if (is_bv_type_family<node_t>(type_tree)) {
		return make_bitvector_bottom_elem(get_bv_size<node_t>(type_tree)
			).getBitVectorValue(10);
	} else return "F";
}

inline tref nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::splitter_one(
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

inline tref nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::unpack_tau_ba(
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

inline std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> nso_factory<tau_ba<bv, sbf_ba>,
		bv, sbf_ba>::normalize(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& v) {
	if (std::holds_alternative<tau_ba<bv, sbf_ba>>(v)) {
		return std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>(
			normalize_tau(std::get<tau_ba<bv, sbf_ba>>(v)));
	} else if (std::holds_alternative<bv>(v)) {
		return std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>(
			normalize_bv(std::get<bv>(v)));
	} else {
		DBG(assert(std::holds_alternative<sbf_ba>(v)));
		return std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>(
			normalize_sbf(std::get<sbf_ba>(v)));
	}
}

inline tref nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::simplify_symbol(tref symbol) {
	auto ba_type = tau::get(symbol).get_ba_type();
	return is_bv_type_family<node_t>(ba_type)
		? simplify_bv_symbol<node_t>(symbol) : symbol;
}

inline tref nso_factory<tau_ba<bv, sbf_ba>, bv, sbf_ba>::simplify_term(tref term) {
	auto ba_type = tau::get(term).get_ba_type();
	return is_bv_type_family<node_t>(ba_type)
		? simplify_bv_term<node_t>(term) : term;
}

} // namespace idni::tau_lang