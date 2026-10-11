// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_TYPES_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_TYPES_TMPL_H__

namespace idni::tau_lang {

// Derived from the descriptor, which builds both from its own type_name.
/** @internal @copydoc nlang_type @endinternal */
template <NodeType node>
tref nlang_type() {
	return ba_descriptor<nlang_ba, node>::type_tree();
}

/** @internal @copydoc nlang_type_id @endinternal */
template <NodeType node>
size_t nlang_type_id() {
	static const size_t id = ba_types<node>::id(
		ba_descriptor<nlang_ba, node>::type_tree());
	return id;
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__NLANG__NLANG_TYPES_TMPL_H__
