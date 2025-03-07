// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

namespace idni::tau_lang {

using parse_forest = idni::parser<char, char>::pforest;
using parse_result = idni::parser<char, char>::result;
using traverser_t  = traverser<sbf_sym, sbf_parser>;

static constexpr const auto& get_children =
		traverser_t::get_children_extractor();
static constexpr const auto& get_only_child =
		traverser_t::get_only_child_extractor();
static constexpr const auto& get_terminals =
		traverser_t::get_terminal_extractor();
static constexpr const auto& get_nonterminal =
		traverser_t::get_nonterminal_extractor();

template <typename...BAs>
// evaluates a parsed bdd terminal node recursively
sbf_ba eval_node(const traverser_t& t) {
	//BOOST_LOG_TRIVIAL(debug) << "eval_node";
	auto n  = t | get_only_child;
	auto nt = n | get_nonterminal;
	switch (nt) {
	case sbf_parser::zero: return bdd_handle<Bool>::hfalse;
	case sbf_parser::one:  return bdd_handle<Bool>::htrue;
	case sbf_parser::negation: {
		auto e = eval_node(n | get_only_child);
		BOOST_LOG_TRIVIAL(trace) << e << "' = " << ~e;
		return ~e;
	}
	case sbf_parser::variable: {
		// get var id from var node's terminals
		auto var_name = n | get_terminals;
		auto v = dict(var_name);
		// use cached var if exists
		if (auto cn = var_cache.find(v);
			cn != var_cache.end())
				return cn->second;
		// otherwise create a new var and cache it
		auto ref = bdd<Bool>::bit(v);
		return var_cache.emplace(v, bdd_handle<Bool>::get(ref))
			.first->second;
	}
	default:
		auto o = (n | get_children)();
		auto l = eval_node(o[0]), r = eval_node(o[1]);
		switch (nt) {
		case sbf_parser::disjunction:
			BOOST_LOG_TRIVIAL(trace)
				<< l << " | " << r << " -> " << (l | r);
			return l | r;
		case sbf_parser::exclusive_disjunction:
			BOOST_LOG_TRIVIAL(trace)
				<< l << " ^ " << r << " -> " << (l ^ r);
			return l ^ r;
		case sbf_parser::conjunction:
		case sbf_parser::conjunction_nosep:
			BOOST_LOG_TRIVIAL(trace)
				<< l << " & " << r << " -> " << (l & r);
			return l & r;
		default: return bdd_handle<Bool>::hfalse;
		}
	}
}
template <typename...BAs>
std::optional<tau<BAs...>> sbf_ba_factory<BAs...>::parse(
	const std::string& src)
{
	// check source cache
	if (auto cn = cache.find(src); cn != cache.end())
		return cn->second;
	auto result = sbf_parser::instance().parse(src.c_str(), src.size());
	if (!result.found) {
		auto msg = result.parse_error
			.to_str(tau_parser::error::info_lvl::INFO_BASIC);
		BOOST_LOG_TRIVIAL(error) << "(Error) " << msg << "\n";
		return std::optional<tau<BAs...>>{}; // Syntax error
	}
	using parse_symbol = sbf_parser::node_type;
	using namespace rewriter;
	auto root = make_node_from_tree<sbf_parser,
		drop_location_t<parse_symbol, sbf_sym>,
		sbf_sym>(
			drop_location<parse_symbol, sbf_sym>,
			result.get_shaped_tree());
	auto t = traverser_t(root) | sbf_parser::sbf;
	auto b = t.has_value()? eval_node(t): bdd_handle<Bool>::hfalse;
	std::variant<BAs...> vp {b};
	auto n = rewriter::make_node<tau_sym<BAs...>>(vp, {});
	return cache.emplace(src, n).first->second;
}

template <typename...BAs>
tau<BAs...> sbf_ba_factory<BAs...>::binding(const tau<BAs...>& sn) {
	auto source = sn
		| tau_parser::source
		| optional_value_extractor<tau<BAs...>>;
	std::string src = make_string(
		tau_node_terminal_extractor<BAs...>, source);
	if (auto parsed = parse(src); parsed.has_value())
		return parsed.value();
	return nullptr;
}

template <typename...BAs>
std::variant<BAs...> sbf_ba_factory<BAs...>::splitter_one() const {
	return std::variant<BAs...>(bdd_splitter_one<Bool>());
}

std::optional<tau<sbf_ba>> nso_factory<sbf_ba>::parse(const std::string& src,
	const std::string&)
{
	return bf.parse(src);
}

tau<sbf_ba> nso_factory<sbf_ba>::binding(const tau<sbf_ba>& n,
	const std::string&)
{
	return bf.binding(n);
}

std::vector<std::string> nso_factory<sbf_ba>::types() const {
	return { "sbf" };
}

tau<sbf_ba> nso_factory<sbf_ba>::splitter_one() const {
	return build_bf_constant(bf.splitter_one(), build_type<sbf_ba>("sbf"));
}

std::string nso_factory<sbf_ba>::default_type() const {
	return "sbf";
}

std::string nso_factory<sbf_ba>::one(const std::string) const {
	return "1";
}

std::string nso_factory<sbf_ba>::zero(const std::string) const {
	return "0";
}

std::optional<tau<sbf_ba> > nso_factory<sbf_ba>::unpack_tau_ba(
	const std::variant<sbf_ba>&) const {
	// There is no tau_ba present
	return {};
}

nso_factory<sbf_ba>& nso_factory<sbf_ba>::instance() {
	static nso_factory<sbf_ba> factory;
	return factory;
}

nso_factory<sbf_ba>::nso_factory() {};


std::optional<tau_nso<sbf_ba>> nso_factory<tau_ba<sbf_ba>, sbf_ba>::parse(
	const std::string src, const std::string type_name)
{
	if (type_name == "sbf")	return bf.parse(src);
	return tf.parse(src);
}

tau_nso<sbf_ba> nso_factory<tau_ba<sbf_ba>, sbf_ba>::binding(
	const tau<tau_ba<sbf_ba>, sbf_ba>& n,
	const std::string type_name)
{
	if (type_name == "sbf") return bf.binding(n);
	return tf.binding(n);
}

std::vector<std::string> nso_factory<tau_ba<sbf_ba>, sbf_ba>::types() const {
	return { "sbf", "tau" };
}

std::string nso_factory<tau_ba<sbf_ba>, sbf_ba>::default_type() const {
	return "tau";
}

std::string nso_factory<tau_ba<sbf_ba>, sbf_ba>::one(const std::string type_name) const {
	return type_name == "sbf" ? "1" : "T";
}

std::string nso_factory<tau_ba<sbf_ba>, sbf_ba>::zero(const std::string type_name) const {
	return type_name == "sbf" ? "0" : "F";
}

std::optional<tau_nso<sbf_ba> > nso_factory<tau_ba<sbf_ba>, sbf_ba>::unpack_tau_ba(
		const std::variant<tau_ba<sbf_ba>, sbf_ba>& v) const {
	if (!std::holds_alternative<tau_ba<sbf_ba>>(v))
		return {};
	const auto unpacked = std::get<tau_ba<sbf_ba>>(v);
	return std::optional(unpacked.nso_rr.main);
}

tau_nso<sbf_ba> nso_factory<tau_ba<sbf_ba>, sbf_ba>::splitter_one(
	const std::string& type_name) const
{
	if (type_name == "sbf")
		return build_bf_constant(bf.splitter_one(), build_type<tau_ba<sbf_ba>, sbf_ba>("sbf"));
	else return build_bf_constant(tf.splitter_one(), build_type<tau_ba<sbf_ba>, sbf_ba>("tau"));
}

nso_factory<tau_ba<sbf_ba>, sbf_ba>&
	nso_factory<tau_ba<sbf_ba>, sbf_ba>::instance()
{
	static nso_factory<tau_ba<sbf_ba>, sbf_ba> factory;
	return factory;
}

nso_factory<tau_ba<sbf_ba>, sbf_ba>::nso_factory() {};

std::optional<tau<bitvector_ba>> nso_factory<bitvector_ba>::parse(const std::string& src,
	const std::string&)
{
	return bvf.parse(src);
}

tau<bitvector_ba> nso_factory<bitvector_ba>::binding(const tau<bitvector_ba>& n,
	const std::string&)
{
	return bvf.binding(n);
}

std::vector<std::string> nso_factory<bitvector_ba>::types() const {
	return { "sbf" };
}

tau<bitvector_ba> nso_factory<bitvector_ba>::splitter_one() const {
	throw std::logic_error("Unsupported");
}

std::string nso_factory<bitvector_ba>::default_type() const {
	return "sbf";
}

std::string nso_factory<bitvector_ba>::one(const std::string) const {
	return "1";
}

std::string nso_factory<bitvector_ba>::zero(const std::string) const {
	return "0";
}

std::optional<tau<bitvector_ba>> nso_factory<bitvector_ba>::unpack_tau_ba(
	const std::variant<bitvector_ba>&) const {
	// There is no tau_ba present
	return {};
}

nso_factory<bitvector_ba>& nso_factory<bitvector_ba>::instance() {
	static nso_factory<bitvector_ba> factory;
	return factory;
}

nso_factory<bitvector_ba>::nso_factory() {};

std::optional<tau<tau_ba<bitvector_ba, sbf_ba>, bitvector_ba, sbf_ba>> nso_factory<tau_ba<bitvector_ba, sbf_ba>, bitvector_ba, sbf_ba>::parse(const std::string& src,
	const std::string type_name)
	{
		return bvf.parse(src);
	}

tau<tau_ba<bitvector_ba, sbf_ba>, bitvector_ba, sbf_ba> nso_factory<tau_ba<bitvector_ba, sbf_ba>, bitvector_ba, sbf_ba>::binding(const tau<tau_ba<bitvector_ba, sbf_ba>, bitvector_ba, sbf_ba>& n,
	const std::string type_name)
{
	return bvf.binding(n);
}

std::vector<std::string> nso_factory<tau_ba<bitvector_ba, sbf_ba>, bitvector_ba, sbf_ba>::types() const {
	return { "sbf" };
}

tau<tau_ba<bitvector_ba, sbf_ba>, bitvector_ba, sbf_ba> nso_factory<tau_ba<bitvector_ba, sbf_ba>, bitvector_ba, sbf_ba>::splitter_one(const std::string& type_name) const {
	throw std::logic_error("Unsupported");
}

std::string nso_factory<tau_ba<bitvector_ba, sbf_ba>, bitvector_ba, sbf_ba>::default_type() const {
	return "tau";
}

std::string nso_factory<tau_ba<bitvector_ba, sbf_ba>, bitvector_ba, sbf_ba>::one(const std::string type_name) const {
	return "1";
}

std::string nso_factory<tau_ba<bitvector_ba, sbf_ba>, bitvector_ba, sbf_ba>::zero(const std::string type_name) const {
	return "0";
}

std::optional<tau<tau_ba<bitvector_ba, sbf_ba>, bitvector_ba, sbf_ba>> nso_factory<tau_ba<bitvector_ba, sbf_ba>, bitvector_ba, sbf_ba>::unpack_tau_ba(
	const std::variant<tau_ba<bitvector_ba, sbf_ba>, bitvector_ba, sbf_ba>&) const {
	// There is no tau_ba present
	return {};
}

nso_factory<tau_ba<bitvector_ba, sbf_ba>, bitvector_ba, sbf_ba>& nso_factory<tau_ba<bitvector_ba, sbf_ba>, bitvector_ba, sbf_ba>::instance() {
	static nso_factory<tau_ba<bitvector_ba, sbf_ba>, bitvector_ba, sbf_ba> factory;
	return factory;
}

nso_factory<tau_ba<bitvector_ba, sbf_ba>, bitvector_ba, sbf_ba>::nso_factory() {};

} // namespace idni::tau_lang
