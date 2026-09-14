// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file adt_flatten.h
 * @brief ADT flattener: rewrites a parsed spec so every ADT construct
 * (member paths, tuple equalities/inequalities, quantifiers over tuple-typed
 * variables, alias annotations) is expanded into ordinary base-typed
 * variables and atoms, per `private/2026-08-05-adt-design.md` section 3.
 *
 * Runs on the parsed spec tree right after parsing, before `infer_ba_types`
 * and before ref resolution: `parse -> adt_flatten -> infer_ba_types -> ...`.
 * `type_def` nodes are erased from the rewritten tree; downstream machinery
 * (BA type inference, normalizer, satisfiability, solver) never sees ADTs.
 *
 * Template implementation lives in adt_flatten.tmpl.h. Like adt_types.h, this
 * header does NOT include adt_flatten.tmpl.h at its own end: the
 * implementation uses the tree traverser (`tt`), `is<node,...>`, and the
 * `tree<node>::build_*` static constructors, all only fully defined partway
 * through tau_tree.tmpl.h's include chain. tau_tree.tmpl.h includes
 * adt_flatten.h early (next to adt_types.h) and adt_flatten.tmpl.h later
 * (next to adt_types.tmpl.h) -- see adt_types.h's file header for the same
 * split applied to the registry.
 */

#ifndef __IDNI__TAU__ADT_ADT_FLATTEN_H__
#define __IDNI__TAU__ADT_ADT_FLATTEN_H__

#include "io_context.h"

namespace idni::tau_lang {

/**
 * @brief Flatten every ADT construct reachable under @p spec.
 *
 * Two passes over the parsed (pre-`infer_ba_types`) spec tree:
 * 1. scope-aware ADT typing -- an annotation `x : Point` anywhere in a
 *    quantifier scope (or the top-level/global scope) fixes `x`'s ADT type
 *    for that scope; conflicting registry-type annotations for the same
 *    variable in one scope are an error; member access on a variable with no
 *    known ADT type anywhere in scope is an error.
 * 2. bottom-up rewrite: a full member path to a base member becomes a plain
 *    variable named after the dotted path; `=`/`!=` with a tuple-typed side
 *    expand into a conjunction/disjunction of member-wise atoms; a
 *    quantifier over a tuple-typed variable expands into one binder per flat
 *    member; an alias annotation is rewritten to its base-type target;
 *    `type_def` statements are erased.
 *
 * @tparam node Tree node type satisfying `NodeType`.
 * @param spec Parsed spec tree (as produced by `tau::get` with
 * `infer_ba_types = false`).
 * @param ctx I/O context; unused in this pass (io stream ADT layouts arrive
 * in a later task) -- accepted so callers do not need to special-case it.
 * @param session_type_defs Optional REPL-session `type_def`s (declared on
 * earlier, separately parsed lines) to pre-register into the registry
 * before @p spec's own type_defs -- see `adt_registry::build`'s override
 * semantics. `nullptr` (the default) matches today's spec-only behavior.
 * @return The flattened tree, or `nullptr` after `LOG_ERROR` on any spec
 * error (registry error, conflicting annotation, untyped/unknown member
 * access, member access on a non-tuple, shape mismatch, or a tuple-typed
 * term used outside `=`/`!=`/a quantifier binder).
 */
template <NodeType node>
tref adt_flatten(tref spec, io_context<node>* ctx = nullptr,
	const std::vector<htref>* session_type_defs = nullptr);

} // namespace idni::tau_lang

#endif // __IDNI__TAU__ADT_ADT_FLATTEN_H__
