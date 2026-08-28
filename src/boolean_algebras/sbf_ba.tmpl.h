// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

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

	auto n  = t | tt::only_child;
	auto nt = n | tt::nonterminal;
	switch (nt) {
	// bdd_handle<Bool>::hfalse/htrue are only ever assigned inside bdd_init
	// (bdds/bdd_handle.h); every OTHER path through this function reaches
	// them only after a bdd_handle<Bool>::get(...)/bdd<Bool>::bit(...) call
	// (the `variable`/binary-op cases below, and their own recursion), which
	// transitively forces bdd<Bool,o>'s inline-static initializer (`I`) to
	// run first. A bare "0"/"1" leaf -- the very first sbf value ever parsed
	// in a process where nothing else has touched the bdd/sbf machinery yet
	// (as narrow a case as one interpreter unit test's own fixture, with no
	// other sbf-consuming test in the same binary before it) -- never goes
	// through such a call, so hfalse/htrue could still be their default
	// (empty/null) construction here. bdd_init is idempotent (guarded by its
	// own `if (!V.empty()) return;`), so calling it defensively is a no-op
	// on every other path.
	case type::zero: bdd_init<Bool>(); return bdd_handle<Bool>::hfalse;
	case type::one:  bdd_init<Bool>(); return bdd_handle<Bool>::htrue;
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
		default:
			LOG_ERROR << "[sbf] unrecognized binary nonterminal: "
				<< (size_t) nt << "\n";
			DBG(assert(false);)
			return bdd_handle<Bool>::hfalse;
		}
	}
}

template <typename... BAs>
requires BAsPack<BAs...>
std::optional<typename node<BAs...>::constant_with_type> parse_sbf(
	const std::string& src)
{
	bdd_init<Bool>(); // ensure BDD is initialized (inline-static initializer may be elided by LTO)
	static std::map<size_t, std::variant<BAs...>> cache;

	// check source cache
	auto sid = dict(src);
	if (auto cn = cache.find(sid); cn != cache.end())
		return typename node<BAs...>::constant_with_type{ cn->second, sbf_type<node<BAs...>>() };
	//parse the source
	auto result = sbf_parser::instance().parse(src.c_str(), src.size());
	if (!result.found) {
		auto msg = result.parse_error
			.to_str(sbf_parser::error::info_lvl::INFO_BASIC);
		LOG_ERROR << "[sbf] " << msg << "\n";
		return {}; // Syntax error
	}
	// get the sbf_constant node
	auto t = sbf_parser::tree::traverser(result.get_shaped_tree2())
							| sbf_parser::sbf;
	auto v = t.has_value() ? sbf_eval_node(t) : bdd_handle<Bool>::hfalse;
	return typename node<BAs...>::constant_with_type{
		cache.emplace(sid, std::variant<BAs...>{ v }).first->second,
		sbf_type<node<BAs...>>() };
}

} // namespace idni::tau_lang
