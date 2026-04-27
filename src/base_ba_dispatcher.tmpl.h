// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

namespace idni::tau_lang {

template <>
inline std::optional<ba_constants<node<bv, sbf_ba>>::constant_with_type> ba_constants<node<bv, sbf_ba>>::get(
		const std::string& constant_source,	tref type_tree,
		[[maybe_unused]] const std::string options) {
	if (is_bv_type_family<node<bv, sbf_ba>>(type_tree))
		return parse_bv<bv, sbf_ba>(constant_source, type_tree);
	return parse_sbf<bv, sbf_ba>(constant_source);
}

template <>
inline std::optional<typename ba_constants<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>::constant_with_type> ba_constants<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>::get(
		const std::string& constant_source,	tref type_tree,
		[[maybe_unused]] const std::string options) {
	if (is_sbf_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>(type_tree))
		return parse_sbf<tau_ba<bv, sbf_ba>, bv, sbf_ba>(constant_source);
	if (is_bv_type_family<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>(type_tree))
		return parse_bv<tau_ba<bv, sbf_ba>, bv, sbf_ba>(constant_source, type_tree);
	return parse_tau<bv, sbf_ba>(constant_source);
}

/*template<>
struct base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba> {};

template<>
struct base_ba_dispatcher<bv, sbf_ba> {};*/

template<>
inline bool base_ba_dispatcher<bv, sbf_ba>::is_syntactic_one(const std::variant<bv, sbf_ba>& elem) {
	// Note that syntactic zero and one are the same as semantic zero and one
	// for bv and sbf, so we can use the same function
	return (std::holds_alternative<bv>(elem))
		? is_bv_syntactic_one(std::get<bv>(elem))
		: is_sbf_one(std::get<sbf_ba>(elem));
}

template<>
inline bool base_ba_dispatcher<bv, sbf_ba>::is_syntactic_zero(const std::variant<bv, sbf_ba>& elem) {
	// See bove comment
	return (std::holds_alternative<bv>(elem))
		? is_bv_syntactic_zero(std::get<bv>(elem))
		: is_sbf_zero(std::get<sbf_ba>(elem));
}

template<>
inline bool base_ba_dispatcher<bv, sbf_ba>::is_one(const std::variant<bv, sbf_ba>& elem) {
	// See bove comment
	return (std::holds_alternative<bv>(elem))
		? is_bv_syntactic_one(std::get<bv>(elem))
		: is_sbf_one(std::get<sbf_ba>(elem));
}

template<>
inline bool base_ba_dispatcher<bv, sbf_ba>::is_zero(const std::variant<bv, sbf_ba>& elem) {
	// See bove comment
	return (std::holds_alternative<bv>(elem))
		? is_bv_syntactic_zero(std::get<bv>(elem))
		: is_sbf_zero(std::get<sbf_ba>(elem));
}

template<>
inline bool base_ba_dispatcher<bv, sbf_ba>::is_closed(const std::variant<bv, sbf_ba>&) {
	// We return true as bv and sbf are closed (for our pourposes)
	return true;
}

template<>
inline std::vector<std::string> base_ba_dispatcher<bv, sbf_ba>::types() {
	return { "sbf", "bv" };
}

template<>
inline tref base_ba_dispatcher<bv, sbf_ba>::default_type() {
	return sbf_type<node<bv,sbf_ba>>();
}

template<>
inline std::string base_ba_dispatcher<bv, sbf_ba>::one(const tref type_tree) {
	using node_t = node<bv, sbf_ba>;
	if (is_bv_type_family<node_t>(type_tree))
		return make_bitvector_top_elem(get_bv_size<node_t>(type_tree)
			).getBitVectorValue(10);
	else return "1";
}

template<>
inline std::string base_ba_dispatcher<bv, sbf_ba>::zero(const tref type_tree) {
	using node_t = node<bv, sbf_ba>;
	if (is_bv_type_family<node_t>(type_tree))
		return make_bitvector_bottom_elem(get_bv_size<node_t>(type_tree)
			).getBitVectorValue(10);
	return "0";
}

template<>
inline tref base_ba_dispatcher<bv, sbf_ba>::splitter_one(tref type_tree) {
	using tau = tree<node<bv, sbf_ba>>;
	return is_sbf_type<node<bv, sbf_ba>>(type_tree)
		? tau::get(tau::bf, tau::get_ba_constant(
			typename tau::constant(sbf_splitter_one()),
			sbf_type<node<bv, sbf_ba>>()))
		: nullptr; // There is no splitter one for bv
}

template<>
inline std::variant<bv, sbf_ba> base_ba_dispatcher<bv, sbf_ba>::splitter(const std::variant<bv, sbf_ba>& elem, splitter_type st) {
	DBG(assert(!std::holds_alternative<bv>(elem));)
	return std::variant<bv, sbf_ba>(sbf_splitter(std::get<sbf_ba>(elem), st));
}

template<>
inline tref base_ba_dispatcher<bv, sbf_ba>::unpack_tau_ba(const std::variant<bv, sbf_ba>&) {
	return nullptr; // There is no tau_ba present
}

template<>
inline std::variant<bv, sbf_ba> base_ba_dispatcher<bv, sbf_ba>::pack_tau_ba(
	tref) {
	// There is no tau_ba
	return {};
}

template<>
inline std::variant<bv, sbf_ba> base_ba_dispatcher<bv, sbf_ba>::normalize(const std::variant<bv, sbf_ba>& v) {
	return std::holds_alternative<bv>(v)
		? std::variant<bv, sbf_ba>(normalize_bv(std::get<bv>(v)))
		: std::variant<bv, sbf_ba>(normalize_sbf(std::get<sbf_ba>(v)));
}

template<>
inline tref base_ba_dispatcher<bv, sbf_ba>::simplify_symbol(tref symbol) {
	auto ba_type = tau::get(symbol).get_ba_type();
	return is_bv_type_family<node_t>(ba_type) ? simplify_bv_symbol<node_t>(symbol) : symbol;
}

template<>
inline tref base_ba_dispatcher<bv, sbf_ba>::simplify_term(tref term) {
	auto ba_type = tau::get(term).get_ba_type();
	return is_bv_type_family<node_t>(ba_type) ? simplify_bv_term<node_t>(term) : term;
}

template<>
inline bool base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba>::is_syntactic_one(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& elem) {
	if (std::holds_alternative<tau_ba<bv, sbf_ba>>(elem))
		return std::get<tau_ba<bv, sbf_ba>>(elem).is_one();
	else if (std::holds_alternative<bv>(elem))
		return is_bv_syntactic_one(std::get<bv>(elem));
	// Note that syntactic zero and one are the same as semantic zero and one
	// for sbf, so we can use the same function
	else return is_sbf_one(std::get<sbf_ba>(elem));
}

template<>
inline bool base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba>::is_syntactic_zero(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& elem) {
	if (std::holds_alternative<tau_ba<bv, sbf_ba>>(elem))
		return std::get<tau_ba<bv, sbf_ba>>(elem).is_zero();
	else if (std::holds_alternative<bv>(elem))
		return is_bv_syntactic_zero(std::get<bv>(elem));
	// See above comment
	else return is_sbf_zero(std::get<sbf_ba>(elem));
}

template<>
inline bool base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba>::is_one(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& elem) {
	if (std::holds_alternative<tau_ba<bv, sbf_ba>>(elem))
		return std::get<tau_ba<bv, sbf_ba>>(elem).is_one();
	else if (std::holds_alternative<bv>(elem))
		// Note that syntactic zero and one are the same as semantic zero and one
		// for bv, so we can use the same function
		return is_bv_syntactic_one(std::get<bv>(elem));
	else return is_sbf_one(std::get<sbf_ba>(elem));
}

template<>
inline bool base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba>::is_zero(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& elem) {
	if (std::holds_alternative<tau_ba<bv, sbf_ba>>(elem))
		return std::get<tau_ba<bv, sbf_ba>>(elem).is_zero();
	else if (std::holds_alternative<bv>(elem))
	// See above comment
	return is_bv_syntactic_zero(std::get<bv>(elem));
	else return is_sbf_zero(std::get<sbf_ba>(elem));
}

template<>
inline bool base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba>::is_closed(const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& elem) {
	// We return true as bv and Bool are closed (for our pourposes)
	return (std::holds_alternative<tau_ba<bv, sbf_ba>>(elem))
		? is_tau_closed<bv, sbf_ba>(std::get<tau_ba<bv, sbf_ba>>(elem))
		: true;
}

template<>
inline std::vector<std::string> base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba>::types() {
	return { "sbf", "tau", "bv" };
}

template<>
inline tref base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba>::default_type() {
	return tau_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>();
}

template<>
inline std::string base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba>::one(
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

template<>
inline std::string base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba>::zero(
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

template<>
inline tref base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba>::unpack_tau_ba(
		const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& v) {
	if (!std::holds_alternative<tau_ba<bv, sbf_ba>>(v)) return {};
	const auto unpacked = std::get<tau_ba<bv, sbf_ba>>(v);
	return unpacked.nso_rr.main->get();
}

template<>
inline std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> base_ba_dispatcher<tau_ba<bv, sbf_ba>,
bv, sbf_ba>::pack_tau_ba(tref c) {
	tau_ba<bv, sbf_ba> t {c};
	return {t};
}

template<>
inline tref base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba>::splitter_one(
		tref type_tree)
{
	using node_t = node<tau_ba<bv, sbf_ba>, bv, sbf_ba>;
	using tau = tree<node_t>;
	DBG(assert(!is_bv_type_family<node_t>(type_tree)));
	return is_sbf_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>(type_tree)
		? tau::get(tau::bf,
			tau::get_ba_constant(
				typename tau::constant(sbf_splitter_one()),
				sbf_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>()))
		: tau::get(tau::bf,
			tau::get_ba_constant(
				typename tau::constant(tau_splitter_one<bv, sbf_ba>()),
				tau_type<node<tau_ba<bv, sbf_ba>, bv, sbf_ba>>()));
			}

template<>
inline std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba>::splitter(
		const std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>& elem, splitter_type st) {
	DBG(assert(!std::holds_alternative<bv>(elem));)
	if (std::holds_alternative<tau_ba<bv, sbf_ba>>(elem)) {
		auto fml = std::get<tau_ba<bv, sbf_ba>>(elem);
		auto unpacked = unpack_tau_ba(elem);
		return std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>(
			tau_splitter<tau_ba<bv, sbf_ba>, bv, sbf_ba>(unpacked, st));
	}
	return std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba>(
		sbf_splitter(std::get<sbf_ba>(elem), st));
}

template<>
inline std::variant<tau_ba<bv, sbf_ba>, bv, sbf_ba> base_ba_dispatcher<tau_ba<bv, sbf_ba>,
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

template<>
inline tref base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba>::simplify_symbol(tref symbol) {
	auto ba_type = tau::get(symbol).get_ba_type();
	return is_bv_type_family<node_t>(ba_type)
		? simplify_bv_symbol<node_t>(symbol) : symbol;
}

template<>
inline tref base_ba_dispatcher<tau_ba<bv, sbf_ba>, bv, sbf_ba>::simplify_term(tref term) {
	auto ba_type = tau::get(term).get_ba_type();
	return is_bv_type_family<node_t>(ba_type)
		? simplify_bv_term<node_t>(term) : term;
}

} // namespace idni::tau_lang