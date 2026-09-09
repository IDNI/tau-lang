// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include <functional>
#include <unordered_set>
#include <utility>

#include "adt_types.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "adt"

namespace idni::tau_lang {

// -----------------------------------------------------------------------------
// adt_registry<node>::build
//
// Grammar shapes relied on below (see parser/tau.tgf):
//
//   type_def     -> new_type_name [type_parents] type_body
//   type_parents -> type_name+
//   type_body    -> tuple | type [subtype]     (alias case: no `typed` wrapper)
//   tuple        -> member+
//   member       -> member_name typed
//   typed        -> type [subtype]
//
// `type`, `type_name`, `new_type_name`, `member_name` are string terminals
// whose `.data()` is
// the dict id of the matched text (same pool `dict()` uses). A `typed` node
// (and, for the alias case, a `type_body` node reconstructed the same way)
// is structurally identical to the type trees `ba_types.h`'s constructors
// build (`bv_type`, `sbf_type`, ...), so either can be handed directly to
// `get_ba_type_id` -- no separate extraction step is needed.

template <NodeType node>
std::optional<adt_registry<node>> adt_registry<node>::build(tref spec,
	const htrefs* prior_type_defs)
{
	using tau = tree<node>;
	using tt = typename tau::traverser;

	// 1. Collect every type_def: prior_type_defs (an earlier, separate
	// parse's declarations) first, then spec's own, both in declaration
	// order, through one path -- a spec is parsed in parts, so whether two
	// declarations for one name land in the same parse or in two separate
	// ones is an artifact of input plumbing, not user intent. A repeated
	// name always replaces the earlier definition, after a LOG_WARNING
	// names the type.
	std::unordered_map<size_t, tref> defs;
	std::vector<size_t> order; // declaration order (for a stable resolution pass)
	auto name_of = [&](tref td) { return tt(td) | tau::new_type_name | tt::data; };
	auto declare = [&](tref td) {
		size_t name_sid = name_of(td);
		if (auto it = defs.find(name_sid); it != defs.end()) {
			LOG_WARNING << "ADT: type '" << dict(name_sid)
				<< "' redeclared, replacing the earlier"
				" definition\n";
			it->second = td;
		} else {
			defs.emplace(name_sid, td);
			order.push_back(name_sid);
		}
	};
	if (prior_type_defs)
		for (const htref& h : *prior_type_defs) declare(h->get());
	bool declares_locally = false;
	for (tref td : tau::get(spec).select_all(is<node, tau::type_def>)) {
		declares_locally = true;
		declare(td);
	}

	adt_registry<node> reg;
	reg.declares_locally_ = declares_locally;
	if (defs.empty()) return reg;

	// 2. Resolve every collected name eagerly (order-independent: a
	// definition may reference a type declared later in the spec), via DFS
	// with a "currently being resolved" set for cycle detection (a cycle can
	// run through members, aliases, or parents alike).
	std::unordered_set<size_t> visiting;

	std::function<bool(size_t)> resolve = [&](size_t name_sid) -> bool {
		if (reg.resolved_.contains(name_sid)) return true;
		if (visiting.contains(name_sid)) {
			LOG_ERROR << "ADT: cyclic type definition involving '"
				<< dict(name_sid) << "'\n";
			return false;
		}
		visiting.insert(name_sid);

		tref def = defs.at(name_sid);
		tref body = tt(def) | tau::type_body | tt::ref;
		resolved_type result;

		// Claims a group of one-level members as a unit: either an entire
		// inherited parent's own member list, or one own `member`
		// declaration (always a single member at this level). Rejects the
		// group if any name it introduces was already claimed by an earlier
		// group -- that is what "name clash" / "duplicate member" means
		// here.
		std::unordered_set<size_t> claimed;
		auto claim = [&](std::vector<adt_level_member<node>> group) -> bool {
			std::unordered_set<size_t> names;
			for (const auto& m : group) names.insert(m.name);
			for (size_t n : names) {
				if (claimed.contains(n)) {
					LOG_ERROR << "ADT: duplicate member '"
						<< dict(n) << "' in type '"
						<< dict(name_sid) << "'\n";
					return false;
				}
			}
			for (auto& m : group) result.level_members.push_back(std::move(m));
			claimed.insert(names.begin(), names.end());
			return true;
		};

		if (tref tuple_ref = tt(body) | tau::tuple | tt::ref; tuple_ref) {
			result.is_tuple = true;

			// Parents' own members come first, in declaration order; a
			// parent's OWN resolution already carries its own ancestors, so
			// splicing its level members in is enough to inherit the whole
			// chain.
			if (tref parents = tt(def) | tau::type_parents | tt::ref; parents) {
				for (tref pn : (tt(parents) || tau::type_name).values()) {
					size_t pname = tau::get(pn).data();
					if (!defs.contains(pname)) {
						LOG_ERROR << "ADT: type '" << dict(name_sid)
							<< "' inherits from unknown type '"
							<< dict(pname) << "'\n";
						return false;
					}
					if (!resolve(pname)) return false;
					if (!reg.resolved_.at(pname).is_tuple) {
						LOG_ERROR << "ADT: type '" << dict(name_sid)
							<< "' inherits from non-tuple type '"
							<< dict(pname) << "'\n";
						return false;
					}
					if (!claim(reg.resolved_.at(pname).level_members))
						return false;
				}
			}

			// Then the type's own members, in declaration order. A member
			// naming a registered tuple type links to it (adt_type); a
			// member naming a registered alias resolves through to that
			// alias's base type right away, same as before.
			for (tref mem : (tt(tuple_ref) || tau::member).values()) {
				size_t mname = tt(mem) | tau::member_name | tt::data;
				tref typed_ref = tt(mem) | tau::typed | tt::ref;
				size_t tname = tt(typed_ref) | tau::type | tt::data;

				adt_level_member<node> lm;
				lm.name = mname;
				if (defs.contains(tname)) {
					if (!resolve(tname)) return false;
					const auto& sub = reg.resolved_.at(tname);
					if (sub.is_tuple) lm.adt_type = tname;
					else lm.base_type = sub.alias_target;
				} else {
					// Unregistered type name: base type passthrough --
					// `typed_ref` is already shaped like a base type tree.
					lm.base_type = typed_ref;
				}
				if (!claim({ std::move(lm) })) return false;
			}

			// Walk the finished one-level members to the flat leaf list
			// `members` returns: a base-typed member is already a leaf; a
			// linking member's leaves are its target's own (already-walked
			// and cached) leaves, with its own name prefixed onto each path.
			for (const auto& lm : result.level_members) {
				if (lm.base_type) {
					result.members.push_back(adt_member<node>{
						{ lm.name }, lm.base_type });
				} else {
					for (const auto& sm : reg.resolved_.at(lm.adt_type).members) {
						std::vector<size_t> path{ lm.name };
						path.insert(path.end(), sm.path.begin(), sm.path.end());
						result.members.push_back(adt_member<node>{
							std::move(path), sm.base_type });
					}
				}
			}
		} else {
			// Alias case: type_body's children are [type, subtype?]
			// directly (no `typed` wrapper -- see grammar note above).
			tref type_child = tt(body) | tau::type | tt::ref;
			size_t tname = tau::get(type_child).data();
			tref subtype_child = tt(body) | tau::subtype | tt::ref;

			if (defs.contains(tname)) {
				// Alias of a registered type: a tuple stays a tuple (same
				// level and flat member lists); a base alias chains through.
				if (!resolve(tname)) return false;
				const auto& sub = reg.resolved_.at(tname);
				result.is_tuple = sub.is_tuple;
				if (sub.is_tuple) {
					result.level_members = sub.level_members;
					result.members = sub.members;
				} else result.alias_target = sub.alias_target;
			} else {
				// Unregistered type name: base type alias. Rebuild the
				// same `typed(type[, subtype])` shape ba_types.h's
				// constructors use, generically -- no base type is
				// hardcoded here.
				result.alias_target = subtype_child
					? tau::get(tau::typed, type_child, subtype_child)
					: tau::get(tau::typed, type_child);
			}
		}

		reg.resolved_.emplace(name_sid, std::move(result));
		visiting.erase(name_sid);
		return true;
	};

	for (size_t name_sid : order)
		if (!resolve(name_sid)) return std::nullopt;

	return reg;
}

template <NodeType node>
bool adt_registry<node>::defines(size_t name_sid) const {
	return resolved_.contains(name_sid);
}

template <NodeType node>
bool adt_registry<node>::is_tuple(size_t name_sid) const {
	auto it = resolved_.find(name_sid);
	return it != resolved_.end() && it->second.is_tuple;
}

template <NodeType node>
bool adt_registry<node>::is_alias(size_t name_sid) const {
	auto it = resolved_.find(name_sid);
	return it != resolved_.end() && !it->second.is_tuple;
}

template <NodeType node>
tref adt_registry<node>::alias_target(size_t name_sid) const {
	return resolved_.at(name_sid).alias_target;
}

template <NodeType node>
const std::vector<adt_member<node>>& adt_registry<node>::members(size_t name_sid) const {
	return resolved_.at(name_sid).members;
}

template <NodeType node>
bool adt_registry<node>::empty() const {
	return resolved_.empty();
}

template <NodeType node>
bool adt_registry<node>::declares_locally() const {
	return declares_locally_;
}

} // namespace idni::tau_lang
