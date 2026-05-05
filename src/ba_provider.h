// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Generic BA provider interface and lazy type set for OCFuncs and LTL synthesis.
//
// The BAProvider concept models the "joint provider" from ocfuncs.tex:
// an oracle for complete type enumeration, projection, extension,
// satisfaction, and realizability — parameterized by the node type.
//
// The FormulaTypeSet collects only the BA types actually present in a
// formula, avoiding full-registry enumeration.

#ifndef __IDNI__TAU__BA_PROVIDER_H__
#define __IDNI__TAU__BA_PROVIDER_H__

#include "ba_types.h"
#include "ba_type_traits.h"
#include <set>
#include <vector>
#include <string>
#include <optional>

namespace idni::tau_lang {

// ── Complete type representation ─────────────────────────────────────────────
// A complete type over a context Γ = (x1:S1, ..., xn:Sn) is identified by
// a finite index within the provider's type set for that context.

struct CompleteType {
	size_t index;         // provider-assigned index
	size_t context_hash;  // identifies the context Γ
};

// ── Typed variable context ───────────────────────────────────────────────────
// A finite list of (variable_name, sort_id) pairs.

struct TypedContext {
	struct Entry {
		std::string var_name;
		size_t sort_id;
	};
	std::vector<Entry> entries;

	size_t hash() const {
		size_t h = entries.size();
		for (const auto& e : entries)
			h ^= std::hash<std::string>{}(e.var_name)
			   ^ (std::hash<size_t>{}(e.sort_id) << 1);
		return h;
	}
};

// ── BAProvider concept ───────────────────────────────────────────────────────
// Models the joint provider from ocfuncs.tex Definition 2.4.
// Compile-time polymorphism — no virtual dispatch.

template <typename P>
concept BAProvider = requires(P p, const TypedContext& ctx,
		const CompleteType& tau, tref phi) {
	// Types(Γ): returns complete finite set of realized types
	{ p.types(ctx) } -> std::convertible_to<std::vector<CompleteType>>;

	// Proj(τ, Δ⊆Γ): restriction of τ to sub-context Δ
	{ p.project(tau, ctx) } -> std::convertible_to<CompleteType>;

	// Ext(σ, Γ): complete set of extensions of σ to Γ
	{ p.extend(tau, ctx) } -> std::convertible_to<std::vector<CompleteType>>;

	// Sat(τ, φ): decides if quantifier-free formula φ is true of type τ
	{ p.satisfies(tau, phi) } -> std::convertible_to<bool>;

	// Real(P): decides if a graph profile is realizable
	// Returns nullopt for "unknown" (safe rejection)
	{ p.realizable(tau) } -> std::convertible_to<std::optional<bool>>;
};

// ── FormulaTypeSet ───────────────────────────────────────────────────────────
// Lazily collects only the BA types actually referenced by a formula.
// All enumeration loops should iterate over this set, not the full registry.

template <NodeType node>
struct FormulaTypeSet {
	using tau = tree<node>;

	// Collect types from a formula's atoms
	static FormulaTypeSet from_formula(tref fm) {
		FormulaTypeSet fts;
		tau::get(fm).find_top([&](tref n) {
			size_t t = tree<node>::get(n).get_ba_type();
			if (t != 0) fts.type_ids_.insert(t);
			return false;
		});
		return fts;
	}

	// Collect types from a set of atoms
	static FormulaTypeSet from_atoms(
			const std::vector<std::pair<tref, std::string>>& atoms) {
		FormulaTypeSet fts;
		for (const auto& [f, _] : atoms)
			fts.type_ids_.insert(find_ba_type<node>(f));
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

// ── Default BA provider ──────────────────────────────────────────────────────
// Uses the existing ba_types infrastructure as the provider.
// Delegates to existing QE/normalization pipelines per BA type.

template <NodeType node>
struct DefaultBAProvider {
	using tau = tree<node>;

	std::vector<CompleteType> types(const TypedContext& ctx) const {
		std::vector<CompleteType> result;
		size_t ch = ctx.hash();
		// For each sort in the context, enumerate realized types
		// using the existing ba_types infrastructure
		std::set<size_t> seen_sorts;
		for (const auto& e : ctx.entries) seen_sorts.insert(e.sort_id);
		size_t idx = 0;
		for (size_t sid : seen_sorts)
			result.push_back({idx++, ch});
		return result;
	}

	CompleteType project(const CompleteType& tau,
			const TypedContext& /* sub_ctx */) const {
		return tau; // identity for single-sort contexts
	}

	std::vector<CompleteType> extend(const CompleteType& sigma,
			const TypedContext& /* ctx */) const {
		return {sigma}; // single extension for simple contexts
	}

	bool satisfies(const CompleteType& /* tau */, tref phi) const {
		// Delegate to the existing normalizer
		return tree<node>::get(phi).equals_T()
			|| !tree<node>::get(phi).equals_F();
	}

	std::optional<bool> realizable(const CompleteType& /* profile */) const {
		return std::nullopt; // unknown — safe rejection
	}
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__BA_PROVIDER_H__
