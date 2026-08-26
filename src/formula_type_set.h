// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Lazy BA type set for LTL synthesis.
//
// The formula_type_set collects only the BA types actually present in a
// formula, avoiding full-registry enumeration.

#ifndef __IDNI__TAU__FORMULA_TYPE_SET_H__
#define __IDNI__TAU__FORMULA_TYPE_SET_H__

#include "ba_types.h"
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace idni::tau_lang {

// ── formula_type_set ─────────────────────────────────────────────────────────
// Lazily collects only the BA types actually referenced by a formula.
// All enumeration loops should iterate over this set, not the full registry.

template <NodeType node>
struct formula_type_set {
	using tau = tree<node>;

	// Collect types from a formula's atoms
	static formula_type_set from_formula(tref fm) {
		formula_type_set fts;
		tau::get(fm).find_top([&](tref n) {
			size_t t = tree<node>::get(n).get_ba_type();
			if (t != 0) fts.type_ids_.insert(t);
			return false;
		});
		return fts;
	}

	// Collect types from a set of atoms
	static formula_type_set from_atoms(
			const std::vector<std::pair<tref, std::string>>& atoms) {
		formula_type_set fts;
		for (const auto& [f, _] : atoms)
			// Skip untyped (0) like from_formula does -- an untyped
			// atom otherwise defeats single_type().
			if (size_t tid = find_ba_type<node>(f); tid != 0)
				fts.type_ids_.insert(tid);
		return fts;
	}

	const std::set<size_t>& type_ids() const { return type_ids_; }
	size_t size() const { return type_ids_.size(); }
	bool single_type() const { return type_ids_.size() <= 1; }
	bool contains(size_t tid) const { return type_ids_.contains(tid); }

	// Check if any type in the set satisfies a predicate
	template <typename Pred>
	bool any_of(Pred&& pred) const {
		for (size_t tid : type_ids_)
			if (pred(tid)) return true;
		return false;
	}

	// Iterate
	auto begin() const { return type_ids_.begin(); }
	auto end() const { return type_ids_.end(); }

private:
	std::set<size_t> type_ids_;
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__FORMULA_TYPE_SET_H__
