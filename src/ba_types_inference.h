// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__BA_TYPES_INFERENCE_H__
#define __IDNI__TAU__BA_TYPES_INFERENCE_H__

#include "tau_tree.h"

namespace idni::tau_lang {

// propagator object takes a formula argument with () operator and it checks and
// propagates BA types accross atomic formulas and checks variable scopes
template <typename... BAs>
requires BAsPack<BAs...>
struct ba_types_inference {
	using node = tau_lang::node<BAs...>;
	using tau = tree<node>;
	using tt = tau::traverser;
	using var_scopes_t = std::unordered_map<std::string, size_t>;
	inline static bool disabled = false;
	tref operator()(tref n);
	tref operator()(const tt& n);
private:
	// removes type info from constants and vars and adds their scope_id
	// used as a preparation (first) step before type checking and propagation
	// vsc = vars scope ids
	// csid = constant scope id
	// tsid = temporal scope id
	tref replace_types_with_scope_ids(tref n, var_scopes_t& vsc,
		size_t csid, size_t& tsid);

	// checks and propagates types within the scope (global or quantifier)
	tref check_and_propagate(tref n);
	static constexpr auto is_ba_element = [](tref el) {
		return is<node>(el, tau::bf_constant)
			|| is<node>(el, tau::bf_variable);
	};
	tref get_var_key_node(tref n) const;
	// check appearance of a single type in all BA elems, then propagate it
	bool propagate(tref n);

	// transform type info - remove scope_id and add type subnode
	// used as a cleaning (last) step after type checking and propagation
	tref replace_scope_ids_with_types(tref n) const;

	tau::subtree_string_map ts; // TODO should be subtree_string_unordered_map
	size_t untyped_n = 0;
};


} // idni::tau_lang namespace

#include "ba_types_inference.tmpl.h"

#endif // __IDNI__TAU__BA_TYPES_INFERENCE_H__
