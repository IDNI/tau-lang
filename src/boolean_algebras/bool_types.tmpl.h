// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_TYPES_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_TYPES_TMPL_H__

namespace idni::tau_lang {

// Derived from the descriptor, which builds both from its own type_name.
template <NodeType node>
tref bool_type() {
	return ba_descriptor<Bool, node>::type_tree();
}

template <NodeType node>
size_t bool_type_id() {
	static const size_t id = ba_types<node>::id(
		ba_descriptor<Bool, node>::type_tree());
	return id;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_TYPES_TMPL_H__
