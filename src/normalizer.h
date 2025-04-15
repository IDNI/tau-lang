// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__NORMALIZER_H__
#define __IDNI__TAU__NORMALIZER_H__

#include "boolean_algebras/variant_ba.h"
#include "normal_forms.h"
#include "boolean_algebras/bdds/bdd_handle.h"

// TODO (MEDIUM) fix proper types (alias) at this level of abstraction
//
// We should talk about statement, nso_rr (nso_with_rr?), library, rule, builder,
// bindings, etc... instead of sp_tau_node,...

namespace idni::tau_lang {

RULE(WFF_ELIM_FORALL, "all $X $Y ::= ! ex $X !$Y.")

template <typename... BAs>
rr normalizer(const rr& fm) {
	return fm; // TODO (HIGH)
}

template <typename... BAs>
tref normalizer(tref fm) {
	return fm; // TODO (HIGH)
}

// Given a tree produce a number i such that the uninterpreted constant const_i is
// not present
template<typename... BAs>
tref get_new_uniter_const(tref fm, const std::string& name) {
	using tau = tree<node<BAs...>>;
	auto uniter_consts = rewriter::select_top<node<BAs...>>(fm,
		is_non_terminal<tau_parser::uninterpreted_constant, BAs...>);
	std::set ids{ 0 };
	for (auto uniter_const : uniter_consts) {
		if (const std::string& s = tau::get(uniter_const).get_string();
			s.find(name) != std::string::npos)
		{
			std::string id = s.substr(name.length() + 1, s.size()-1);
			if (!s.empty()) ids.insert(std::stoi(id));
		}
	}
	std::string id = std::to_string(*ids.rbegin() + 1);
	auto uniter_const = tau::build_bf_uniter_const("", name + id);
	return uniter_const;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__NORMALIZER_H__
