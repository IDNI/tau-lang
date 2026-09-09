// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file adt_types.h
 * @brief ADT type registry: collects and resolves `type_def`s from a parsed spec.
 *
 * Defines `adt_member<node>` -- one flattened field of a resolved tuple type
 * -- and `adt_registry<node>`, built once per spec via `adt_registry::build`.
 * Every `type_def` reachable under the spec is resolved eagerly to either:
 * - an alias of a base BA type tree (e.g. `type byte = bv[8]`), or
 * - a flat, ordered member list (e.g. `Line -> [(p.a, sbf), (p.b, sbf), ...]`),
 *   with a type's parents' flattened members placed first, in declaration
 *   order, followed by the type's own members (`type X of (A, B) is {...}`).
 * Aliasing a tuple type resolves to that same flat member list; aliasing a
 * subtyped base (`bv[8]`) or a plain base name resolves to that type tree.
 *
 * The registry never hardcodes a list of base BA type names: any annotation
 * that does not name a registered type is passed through untouched as a base
 * type, so custom BAs keep working unchanged. An unregistered name is only
 * an error in a `type_parents` position (inheriting from something unknown);
 * in a member position it is simply a base-typed leaf.
 *
 * Template implementations live in adt_types.tmpl.h. Unlike the usual
 * .h-includes-its-own-.tmpl.h idiom used elsewhere in this codebase, this
 * header does NOT include adt_types.tmpl.h at its own end: adt_types.tmpl.h
 * uses the tree traverser (`tt`) and `select_all`, which are only fully
 * defined partway through tau_tree.tmpl.h's include chain -- well after
 * tau_tree.h has already pulled in this header. tau_tree.tmpl.h includes
 * adt_types.h early (with the other plain headers) and adt_types.tmpl.h
 * later, next to tau_tree_from_parser.tmpl.h -- the same split
 * antiprenexing.h/.tmpl.h use via normal_forms.h, for the same reason (see
 * antiprenexing.h's file header).
 */

#ifndef __IDNI__TAU__ADT_ADT_TYPES_H__
#define __IDNI__TAU__ADT_ADT_TYPES_H__

#include <cstddef>
#include <optional>
#include <unordered_map>
#include <vector>

namespace idni::tau_lang {

/**
 * @brief One flattened field of a resolved ADT tuple type.
 *
 * `path` holds the member-name dict ids from the tuple's own member down to
 * the leaf, outer to inner (e.g. member `p.a` is `[sid("p"), sid("a")]`).
 * `base_type` is the leaf's base BA type tree, shaped exactly like the type
 * trees `ba_types.h`'s constructors build (`bv_type`, `sbf_type`, ...), so it
 * can be handed directly to `get_ba_type_id`.
 *
 * @tparam node Tree node type satisfying `NodeType`.
 */
template <NodeType node>
struct adt_member {
	std::vector<size_t> path; ///< Member-name dict ids, outer -> inner.
	tref base_type;           ///< Leaf's base BA type tree.
};

/**
 * @brief One member of a resolved ADT type, one nesting level deep.
 *
 * `name` holds the member's own dict id. A member either links to another
 * registered ADT tuple type, or it is base-typed directly. `adt_type` holds
 * the linked type's dict id, 0 when the member does not link. `base_type`
 * holds the member's own base BA type tree, null when it links. `adt_member`
 * is the flat leaf list a chain of these walks to.
 *
 * @tparam node Tree node type satisfying `NodeType`.
 */
template <NodeType node>
struct adt_level_member {
	size_t name = 0;           ///< This member's own dict id.
	size_t adt_type = 0;       ///< The ADT type this member links to, 0 if none.
	tref base_type = nullptr;  ///< This member's own base BA type tree, null when it links.
};

/**
 * @brief Registry mapping ADT type names to their resolved definition.
 *
 * Built once per spec from every `type_def` reachable under it, resolved
 * eagerly and order-independently (a `type_def` may reference a type
 * declared later in the same spec). Each registered name resolves to either
 * an alias of a base type tree, or a flat member list (see `adt_member`).
 *
 * @tparam node Tree node type satisfying `NodeType`.
 */
template <NodeType node>
struct adt_registry {
	/**
	 * @brief Collect every `type_def` under @p spec, plus every tree in
	 * @p prior_type_defs, and resolve all of them.
	 *
	 * @p prior_type_defs supplies declarations from earlier, separately
	 * parsed statements (a REPL session's own accumulated `type_def`s) so
	 * a name they declared resolves here too; they are collected first, in
	 * order, followed by @p spec's own `type_def`s, in declaration order. A
	 * `type_def` for a name already collected -- whether the earlier one came
	 * from @p prior_type_defs or from @p spec itself -- always replaces it,
	 * after `LOG_WARNING` names the type: whether two declarations for one
	 * name land in the same parse or in two separate ones is an artifact of
	 * how the caller fed text in, not a distinction in what the user meant,
	 * so both are handled through the one path.
	 *
	 * @return The built registry, or `std::nullopt` after `LOG_ERROR` on: a
	 * duplicate member (including one introduced via inheritance), a cycle
	 * (through members, aliases, or parents), a type inheriting from
	 * something that does not resolve to a tuple, or a `type_parents` entry
	 * naming an unregistered type. An unregistered type name in a *member*
	 * position is not an error: it is treated as a base (non-ADT) type and
	 * passed through unchanged.
	 */
	static std::optional<adt_registry> build(tref spec,
		const htrefs* prior_type_defs = nullptr);

	/** @brief Return `true` if @p name_sid has a `type_def` in this registry. */
	bool defines(size_t name_sid) const;
	/**
	 * @brief Return `true` if @p name_sid resolves to a flat member list.
	 * Safe probe: `false` (not a throw) for a @p name_sid this registry
	 * does not define -- the common case for an ordinary base-typed name.
	 */
	bool is_tuple(size_t name_sid) const;
	/**
	 * @brief Return `true` if @p name_sid resolves to a base type alias.
	 * Safe probe: `false` (not a throw) for a @p name_sid this registry
	 * does not define.
	 */
	bool is_alias(size_t name_sid) const;
	/** @brief Return the aliased base type tree for @p name_sid. @pre `is_alias(name_sid)`. */
	tref alias_target(size_t name_sid) const;
	/** @brief Return the flat member list for @p name_sid. @pre `is_tuple(name_sid)`. */
	const std::vector<adt_member<node>>& members(size_t name_sid) const;
	/** @brief Return `true` if this registry defines no types at all (fast-path check). */
	bool empty() const;
	/**
	 * @brief Return `true` if @p build's own @p spec argument held at least
	 * one `type_def` (independent of any @p prior_type_defs supplied).
	 *
	 * Distinguishes "this parse itself asserts ADT types" from "every type
	 * this registry knows about was declared elsewhere": an unresolvable
	 * member access is only ever a spec's own mistake when the spec itself
	 * declares a type. A printed-and-reparsed flattened spec (no `type_def`
	 * left, since flattening erases them) never does, so a stray dotted name
	 * such flattening produced elsewhere is not one -- even though a
	 * @p prior_type_defs entry from an earlier, separate parse may still make
	 * this registry non-empty. See adt_resolve_var's use of this in
	 * adt_flatten.tmpl.h for why that keeps flatten(flatten(x)) == flatten(x).
	 */
	bool declares_locally() const;

private:
	/**
	 * @brief One resolved type: either a base-type alias or a tuple.
	 *
	 * A tuple's `level_members` is its own representation, one nesting level
	 * deep (parents' members first, in declaration order, then the type's
	 * own). `members` is that same tuple walked to its flat leaf list, built
	 * once here so `adt_registry::members` can keep returning a reference.
	 */
	struct resolved_type {
		bool is_tuple = false;
		tref alias_target = nullptr;
		std::vector<adt_level_member<node>> level_members{};
		std::vector<adt_member<node>> members{};
	};

	/// @brief name dict id -> resolved definition, populated by `build`.
	std::unordered_map<size_t, resolved_type> resolved_{};
	/// @brief Set by `build` when its own @p spec argument held a `type_def`.
	bool declares_locally_ = false;
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__ADT_ADT_TYPES_H__
