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
//   type_def     -> type_name [type_parents] type_body
//   type_parents -> type_name+
//   type_body    -> tuple | type [subtype]     (alias case: no `typed` wrapper)
//   tuple        -> member+
//   member       -> member_name typed
//   typed        -> type [subtype]
//
// `type`, `type_name`, `member_name` are string terminals whose `.data()` is
// the dict id of the matched text (same pool `dict()` uses). A `typed` node
// (and, for the alias case, a `type_body` node reconstructed the same way)
// is structurally identical to the type trees `ba_types.h`'s constructors
// build (`bv_type`, `sbf_type`, ...), so either can be handed directly to
// `get_ba_type_id` -- no separate extraction step is needed.

template <NodeType node>
std::optional<adt_registry<node>> adt_registry<node>::build(tref spec) {
	using tau = tree<node>;
	using tt = typename tau::traverser;

	// 1. Collect every type_def, rejecting duplicate names up front.
	std::unordered_map<size_t, tref> defs;
	std::vector<size_t> order; // declaration order (for a stable resolution pass)
	for (tref td : tau::get(spec).select_all(is<node, tau::type_def>)) {
		size_t name_sid = tt(td) | tau::type_name | tt::data;
		if (!defs.emplace(name_sid, td).second) {
			LOG_ERROR << "ADT: duplicate type name '"
				<< dict(name_sid) << "'\n";
			return std::nullopt;
		}
		order.push_back(name_sid);
	}

	adt_registry<node> reg;
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

		// Claims a group of flattened members as a unit: either an entire
		// inherited parent's member list, or the (possibly multi-leaf)
		// expansion of one own `member` declaration. Rejects the group if
		// any of the top-level (outermost path element) names it introduces
		// was already claimed by an earlier group -- that is what "name
		// clash" / "duplicate member" means here, independent of how deep
		// the colliding names' own sub-shapes happen to be.
		std::unordered_set<size_t> claimed;
		auto claim = [&](std::vector<adt_member<node>> group) -> bool {
			std::unordered_set<size_t> names;
			for (const auto& m : group) names.insert(m.path.front());
			for (size_t n : names) {
				if (claimed.contains(n)) {
					LOG_ERROR << "ADT: duplicate member '"
						<< dict(n) << "' in type '"
						<< dict(name_sid) << "'\n";
					return false;
				}
			}
			for (auto& m : group) result.members.push_back(std::move(m));
			claimed.insert(names.begin(), names.end());
			return true;
		};

		if (tref tuple_ref = tt(body) | tau::tuple | tt::ref; tuple_ref) {
			result.is_tuple = true;

			// Parents' flattened members come first, in declaration order.
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
					if (!claim(reg.resolved_.at(pname).members))
						return false;
				}
			}

			// Then the type's own members, in declaration order.
			for (tref mem : (tt(tuple_ref) || tau::member).values()) {
				size_t mname = tt(mem) | tau::member_name | tt::data;
				tref typed_ref = tt(mem) | tau::typed | tt::ref;
				size_t tname = tt(typed_ref) | tau::type | tt::data;

				std::vector<adt_member<node>> group;
				if (defs.contains(tname)) {
					// Member names a registered type: splice its
					// resolution, prefixing every sub-path with mname.
					if (!resolve(tname)) return false;
					const auto& sub = reg.resolved_.at(tname);
					if (sub.is_tuple) {
						for (const auto& sm : sub.members) {
							std::vector<size_t> path{ mname };
							path.insert(path.end(),
								sm.path.begin(), sm.path.end());
							group.push_back(adt_member<node>{
								std::move(path), sm.base_type });
						}
					} else {
						group.push_back(adt_member<node>{
							{ mname }, sub.alias_target });
					}
				} else {
					// Unregistered type name: base type passthrough --
					// `typed_ref` is already shaped like a base type tree.
					group.push_back(adt_member<node>{
						{ mname }, typed_ref });
				}
				if (!claim(std::move(group))) return false;
			}
		} else {
			// Alias case: type_body's children are [type, subtype?]
			// directly (no `typed` wrapper -- see grammar note above).
			tref type_child = tt(body) | tau::type | tt::ref;
			size_t tname = tau::get(type_child).data();
			tref subtype_child = tt(body) | tau::subtype | tt::ref;

			if (defs.contains(tname)) {
				// Alias of a registered type: a tuple stays a tuple
				// (same flat member list); a base alias chains through.
				if (!resolve(tname)) return false;
				const auto& sub = reg.resolved_.at(tname);
				result.is_tuple = sub.is_tuple;
				if (sub.is_tuple) result.members = sub.members;
				else result.alias_target = sub.alias_target;
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

} // namespace idni::tau_lang
