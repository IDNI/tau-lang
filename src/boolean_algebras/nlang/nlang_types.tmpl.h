// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_TYPES_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_TYPES_TMPL_H__

namespace idni::tau_lang {

// Derived from the descriptor, which builds both from its own type_name.
template <NodeType node>
tref nlang_type() {
	return ba_descriptor<nlang_ba, node>::type_tree();
}

template <NodeType node>
size_t nlang_type_id() {
	return ba_descriptor<nlang_ba, node>::type_id_for(0);
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_TYPES_TMPL_H__
