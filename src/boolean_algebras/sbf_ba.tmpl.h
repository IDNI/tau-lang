// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "boolean_algebras/sbf_ba.h"
#include "../parser/sbf_parser.generated.h"

namespace idni::tau_lang {

// evaluates a parsed bdd terminal node recursively
inline sbf_ba sbf_eval_node(const sbf_parser::tree::traverser& t) {
	using tt = sbf_parser::tree::traverser;
	using type = sbf_parser::nonterminal;
	// BOOST_LOG_TRIVIAL(debug) << "eval_node";
	auto n  = t | tt::only_child;
	auto nt = n | tt::nonterminal;
	switch (nt) {
	case type::zero: return bdd_handle<Bool>::hfalse;
	case type::one:  return bdd_handle<Bool>::htrue;
	case type::negation: {
		auto e = sbf_eval_node(n | tt::only_child);
		BOOST_LOG_TRIVIAL(trace) << e << "' = " << ~e;
		return ~e;
	}
	case type::variable: {
		// get var id from var node's terminals
		auto var_name = n | tt::terminals;
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
		auto o = (n | tt::children)();
		auto l = sbf_eval_node(o[0]), r = sbf_eval_node(o[1]);
		switch (nt) {
		case type::disjunction:
			// BOOST_LOG_TRIVIAL(trace)
			// 	<< l << " | " << r << " -> " << (l | r);
			return l | r;
		case type::exclusive_disjunction:
			// BOOST_LOG_TRIVIAL(trace)
			// 	<< l << " ^ " << r << " -> " << (l ^ r);
			return l ^ r;
		case type::conjunction:
		case type::conjunction_nosep:
			// BOOST_LOG_TRIVIAL(trace)
			// 	<< l << " & " << r << " -> " << (l & r);
			return l & r;
		default: return bdd_handle<Bool>::hfalse;
		}
	}
}
template <typename... BAs>
requires BAsPack<BAs...>
std::optional<std::variant<BAs...>> sbf_ba_factory<BAs...>::parse(
	const std::string& src)
{
	// check source cache
	if (auto cn = cache.find(src); cn != cache.end())
		return cn->second;
	auto result = sbf_parser::instance().parse(src.c_str(), src.size());
	if (!result.found) {
		auto msg = result.parse_error
			.to_str(sbf_parser::error::info_lvl::INFO_BASIC);
		BOOST_LOG_TRIVIAL(error) << "(Error) " << msg << "\n";
		return {}; // Syntax error
	}
	auto t = sbf_parser::tree::traverser(result.get_shaped_tree2())
							| sbf_parser::sbf;
	auto v = t.has_value()? sbf_eval_node(t) : bdd_handle<Bool>::hfalse;
	return cache.emplace(src, std::variant<BAs...>{ v }).first->second;
}

template <typename... BAs>
requires BAsPack<BAs...>
tref sbf_ba_factory<BAs...>::binding(const std::string& source) {
	// std::cout << "sbf_ba_factory::binding `" << source << "`\n";
	if (auto p = parse(source); p)
		return ba_constants_binder<BAs...>::instance()
							.bind(p.value(), "sbf");
	return nullptr;
}

template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> sbf_ba_factory<BAs...>::splitter_one() const {
	return std::variant<BAs...>(bdd_splitter_one<Bool>());
}

template <typename... BAs>
requires BAsPack<BAs...>
std::string sbf_ba_factory<BAs...>::zero() const {
	return "0";
}

template <typename... BAs>
requires BAsPack<BAs...>
sbf_ba_factory<BAs...>& sbf_ba_factory<BAs...>::instance() {
	static sbf_ba_factory<BAs...> factory;
	return factory;
}

// ---------------------------------------------------------------------

std::optional<std::variant<sbf_ba>> nso_factory<sbf_ba>::parse(
	const std::string& src, const std::string)
{
	return bf.parse(src);
}

tref nso_factory<sbf_ba>::binding(const std::string& n, const std::string&) {
	return bf.binding(n);
}

std::vector<std::string> nso_factory<sbf_ba>::types() const {
	return { "sbf" };
}

tref nso_factory<sbf_ba>::splitter_one() const {
	return ba_constants_binder<sbf_ba>::instance()
						.bind(bf.splitter_one(), "sbf");
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

tref nso_factory<sbf_ba>::unpack_tau_ba(const std::variant<sbf_ba>&) const {
	// There is no tau_ba present
	return nullptr;
}

nso_factory<sbf_ba>& nso_factory<sbf_ba>::instance() {
	static nso_factory<sbf_ba> factory;
	return factory;
}

nso_factory<sbf_ba>::nso_factory() {};

std::optional<std::variant<tau_ba<sbf_ba>, sbf_ba>>
	nso_factory<tau_ba<sbf_ba>, sbf_ba>::parse(
	const std::string& src, const std::string type_name)
{
	if (type_name == "sbf")	return bf().parse(src);
	return tf().parse(src);
}

tref nso_factory<tau_ba<sbf_ba>, sbf_ba>::binding(const std::string& n,
	const std::string type_name)
{
	if (type_name == "sbf") return bf().binding(n);
	return tf().binding(n);
}

std::vector<std::string> nso_factory<tau_ba<sbf_ba>, sbf_ba>::types() const {
	return { "sbf", "tau" };
}

std::string nso_factory<tau_ba<sbf_ba>, sbf_ba>::default_type() const {
	return "tau";
}

std::string nso_factory<tau_ba<sbf_ba>, sbf_ba>::one(
	const std::string type_name) const
{
	return type_name == "sbf" ? "1" : "T";
}

std::string nso_factory<tau_ba<sbf_ba>, sbf_ba>::zero(
	const std::string type_name) const
{
	return type_name == "sbf" ? "0" : "F";
}

tref nso_factory<tau_ba<sbf_ba>, sbf_ba>::unpack_tau_ba(
	const std::variant<tau_ba<sbf_ba>, sbf_ba>& v) const
{
	if (!std::holds_alternative<tau_ba<sbf_ba>>(v))
		return {};
	const auto unpacked = std::get<tau_ba<sbf_ba>>(v);
	return unpacked.nso_rr.main->get();
}

tref nso_factory<tau_ba<sbf_ba>, sbf_ba>::splitter_one(
	const std::string type_name) const
{
	if (type_name=="sbf")
		return ba_constants_binder<tau_ba<sbf_ba>, sbf_ba>::instance()
					.bind(bf().splitter_one(), "sbf");
	else return ba_constants_binder<tau_ba<sbf_ba>, sbf_ba>::instance()
					.bind(tf().splitter_one(), "tau");
}

nso_factory<tau_ba<sbf_ba>, sbf_ba>&
	nso_factory<tau_ba<sbf_ba>, sbf_ba>::instance()
{
	static nso_factory<tau_ba<sbf_ba>, sbf_ba> factory;
	return factory;
}

nso_factory<tau_ba<sbf_ba>, sbf_ba>::nso_factory() {};

} // namespace idni::tau_lang
