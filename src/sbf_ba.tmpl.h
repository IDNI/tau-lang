// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "sbf_ba.h"

namespace idni::tau {

template <typename...BAs>
std::optional<nso<BAs...>> sbf_ba_factory<BAs...>::parse(
	const std::string& src)
{
	// check source cache
	if (auto cn = cache.find(src); cn != cache.end())
		return cn->second;
	auto& p = sbf_parser::instance();
	auto r = p.parse(src.c_str(), src.size());
	if (!r.found) return std::optional<nso<BAs...>>{};
	using parse_symbol = sbf_parser::node_type;
	using namespace rewriter;
	auto root = make_node_from_tree<sbf_parser,
		drop_location_t<parse_symbol, sbf_sym>,
		sbf_sym>(
			drop_location<parse_symbol, sbf_sym>,
			r.get_shaped_tree());
	auto t = traverser_t(root) | sbf_parser::sbf;
	return std::optional<nso<BAs...>>{ build_node(t.has_value()
		? eval_node(t) : bdd_handle<Bool>::hfalse) };
}

template <typename...BAs>
nso<BAs...> sbf_ba_factory<BAs...>::binding(const nso<BAs...>& sn) {
	auto source = sn
		| tau_parser::source
		| optional_value_extractor<nso<BAs...>>;
	std::string src = make_string(
		tau_node_terminal_extractor<BAs...>, source);
	if (auto parsed = parse(src); parsed.has_value())
		return parsed.value();
	return sn;
}

template <typename...BAs>
std::variant<BAs...> sbf_ba_factory<BAs...>::splitter_one() const {
	return std::variant<BAs...>(bdd_splitter_one<Bool>());
}

template <typename...BAs>
nso<BAs...> sbf_ba_factory<BAs...>::build_node(const sbf_ba& b) {
	std::variant<BAs...> vp{b};
	return rewriter::make_node<tau_sym<BAs...>>(vp, {});
}

template <typename...BAs>
// evaluates a parsed bdd terminal node recursively
sbf_ba sbf_ba_factory<BAs...>::eval_node(const traverser_t& t) {
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
		auto o = (n || sbf_parser::sbf)();
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
			BOOST_LOG_TRIVIAL(trace)
				<< l << " & " << r << " -> " << (l & r);
			return l & r;
		default: return bdd_handle<Bool>::hfalse;
		}
	}
}

std::optional<nso<sbf_ba>> nso_factory<sbf_ba>::parse(const std::string& src,
	const std::string&)
{
	return bf.parse(src);
}

nso<sbf_ba> nso_factory<sbf_ba>::binding(const nso<sbf_ba>& n,
	const std::string&)
{
	return bf.binding(n);
}

std::vector<std::string> nso_factory<sbf_ba>::types() const {
	return { "sbf" };
}

nso<sbf_ba> nso_factory<sbf_ba>::splitter_one() const {
	return build_bf_constant(bf.splitter_one());
}

std::string nso_factory<sbf_ba>::default_type() const {
	return "sbf";
}

nso_factory<sbf_ba>& nso_factory<sbf_ba>::instance() {
	static nso_factory<sbf_ba> factory;
	return factory;
}

nso_factory<sbf_ba>::nso_factory() {};


std::optional<gssotc<sbf_ba>> nso_factory<tau_ba<sbf_ba>, sbf_ba>::parse(
	const std::string src, const std::string type_name)
{
	if (type_name == "sbf")	return bf.parse(src);
	return tf.parse(src);
}

gssotc<sbf_ba> nso_factory<tau_ba<sbf_ba>, sbf_ba>::binding(
	const sp_tau_node<tau_ba<sbf_ba>, sbf_ba>& n,
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

gssotc<sbf_ba> nso_factory<tau_ba<sbf_ba>, sbf_ba>::splitter_one(
	const std::string& type_name) const
{
	if (type_name == "sbf")
		return build_bf_constant(bf.splitter_one());
	else return build_bf_constant(tf.splitter_one());
}

nso_factory<tau_ba<sbf_ba>, sbf_ba>&
	nso_factory<tau_ba<sbf_ba>, sbf_ba>::instance()
{
	static nso_factory<tau_ba<sbf_ba>, sbf_ba> factory;
	return factory;
}

nso_factory<tau_ba<sbf_ba>, sbf_ba>::nso_factory() {};

} // namespace idni::tau
