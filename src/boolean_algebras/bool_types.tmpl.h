// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_TYPES_TMPL_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_TYPES_TMPL_H__

namespace idni::tau_lang {

/** @brief Type tree of the Bool type. */
template <NodeType node>
tref bool_type() { return ba_type_tree<Bool, node>(); }

/** @brief Type id of the Bool type, computed once per node type. */
template <NodeType node>
size_t bool_type_id() { return ba_type_id<Bool, node>(); }

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__BOOL_TYPES_TMPL_H__
