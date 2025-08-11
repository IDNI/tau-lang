// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "boolean_algebras/sbf_ba.h"
#include "../parser/sbf_parser.generated.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "sbf_ba"

namespace idni::tau_lang {
	using namespace idni;

// evaluates a parsed bdd terminal node recursively
inline sbf_ba sbf_eval_node(const sbf_parser::tree::traverser& t) {
	using tt = sbf_parser::tree::traverser;
	using type = sbf_parser::nonterminal;
	// LOG_DEBUG << "eval_node";
	auto n  = t | tt::only_child;
	auto nt = n | tt::nonterminal;
	switch (nt) {
	case type::zero: return bdd_handle<Bool>::hfalse;
	case type::one:  return bdd_handle<Bool>::htrue;
	case type::negation: {
		auto e = sbf_eval_node(n | tt::only_child);
		LOG_TRACE << e << "' = " << ~e;
		return ~e;
	}
	case type::variable: {
		// get var id from var node's terminals
		auto var_name = n | tt::terminals;
		auto v = var_dict(var_name);
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
			// LOG_TRACE << l << " | " << r << " -> " << (l | r);
			return l | r;
		case type::exclusive_disjunction:
			// LOG_TRACE << l << " ^ " << r << " -> " << (l ^ r);
			return l ^ r;
		case type::conjunction:
		case type::conjunction_nosep:
			// LOG_TRACE << l << " & " << r << " -> " << (l & r);
			return l & r;
		default: return bdd_handle<Bool>::hfalse;
		}
	}
}

template <typename... BAs>
requires BAsPack<BAs...>
std::optional<constant_with_type<BAs...>> sbf_ba_factory<BAs...>::parse(
	const std::string& src)
{
	// check source cache
	auto sid = dict(src);
	if (auto cn = cache.find(sid); cn != cache.end())
		return constant_with_type<BAs...>{ cn->second, "sbf" };
	auto result = sbf_parser::instance().parse(src.c_str(), src.size());
	if (!result.found) {
		auto msg = result.parse_error
			.to_str(sbf_parser::error::info_lvl::INFO_BASIC);
		LOG_ERROR << "[sbf] " << msg << "\n";
		return {}; // Syntax error
	}
	auto t = sbf_parser::tree::traverser(result.get_shaped_tree2())
							| sbf_parser::sbf;
	auto v = t.has_value() ? sbf_eval_node(t) : bdd_handle<Bool>::hfalse;
	return constant_with_type<BAs...>{
		cache.emplace(sid, std::variant<BAs...>{ v }).first->second,
		"sbf" };
}

template <typename... BAs>
requires BAsPack<BAs...>
std::string sbf_ba_factory<BAs...>::zero() const {
	return "0";
}

template <typename... BAs>
requires BAsPack<BAs...>
std::variant<BAs...> sbf_ba_factory<BAs...>::splitter_one() const {
	return std::variant<BAs...>(bdd_splitter_one<Bool>());
}

template <typename... BAs>
requires BAsPack<BAs...>
sbf_ba_factory<BAs...>& sbf_ba_factory<BAs...>::instance() {
	static sbf_ba_factory<BAs...> factory;
	return factory;
}

} // namespace idni::tau_lang
