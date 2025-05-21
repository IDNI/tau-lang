// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__BA_TYPES_INFERENCE_H__
#define __IDNI__TAU__BA_TYPES_INFERENCE_H__

#include "tau_tree.h"

namespace idni::tau_lang {

// propagator object takes a formula argument with () operator and it checks and
// propagates BA types accross atomic formulas and checks variable scopes
template <NodeType node>
struct ba_types_inference {
	using tau = tree<node>;
	using tt = tau::traverser;
	using var_scopes_t = std::map<size_t, size_t>; // var_name_sid -> var_scope_id
	inline static bool disabled = false;
	tref operator()(tref n);
	tref operator()(const tt& n);
private:
	// helper to get the proper key node for a variable
	tref get_var_key_node(tref n) const;

	// removes type info from constants and vars and adds their scope_id
	// used as a preparation (first) step before type checking and propagation
	// vscids = vars scope ids
	// csid = constant scope id
	// tsid = temporal scope id
	tref add_scope_ids(tref n,
			var_scopes_t& vscids, size_t csid, size_t& tsid);

	// checks and propagates types within the scope (global or quantifier)
	tref check_and_propagate(tref n);

	// check appearance of a single type in all BA elems, then propagate it
	bool propagate(tref n);

	// transform type info - remove scope_id and add type subnode
	// used as a cleaning (last) step after type checking and propagation
	tref remove_scope_ids(tref n) const;

	void clear();

	std::map<size_t, size_t>  vars;     // var_sid_id -> var_scope_id
	subtree_map<node, size_t> types;    // node -> ba_type_id
	subtree_map<node, tref>   resolved; // untyped node -> resolved node
	size_t untyped_n = 0;

	std::ostream& dump(std::ostream& os) const;
	std::string dump_to_str() const;
};


} // namespace idni::tau_lang

#include "ba_types_inference.tmpl.h"

#endif // __IDNI__TAU__BA_TYPES_INFERENCE_H__
