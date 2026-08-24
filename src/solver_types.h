// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file solver_types.h
 * @brief The solver's vocabulary types, without the solver's templates.
 *
 * Split out of solver.h so a BA plugin can name a solution or a solver_options
 * in its descriptor: solver.h ends by including solver.tmpl.h, whose folds call
 * back into the descriptors, so a descriptor including it would close a cycle.
 */

#ifndef __IDNI__TAU__SOLVER_TYPES_H__
#define __IDNI__TAU__SOLVER_TYPES_H__

#include <algorithm>
#include <cstdint>
#include <map>
#include <optional>

#include "tau_tree.h"

namespace idni::tau_lang {

/**
 * @typedef typed_nso
 * @brief Alias for tau tree node.
 */
using typed_nso = tref;

/**
 * @typedef var
 * @brief Alias for tau tree node.
 */
using var = tref;

/**
 * @typedef minterm
 * @brief Alias for tau tree node.
 */
using minterm = tref;

/**
 * @typedef equality
 * @brief Alias for tau tree node.
 */
using equality = tref;

/**
 * @typedef inequality
 * @brief Alias for tau tree node.
 */
using inequality = tref;

/**
 * @typedef equation
 * @brief Alias for tau tree node.
 */
using equation = tref;

/**
 * @typedef equations
 * @brief Alias for a set of tau tree nodes.
 */
template <NodeType node>
using equations = subtree_set<node>;

/**
 * @typedef equation_system
 * @brief Alias for a pair consisting of an optional equality and a set of inequalities.
 */
template<NodeType node>
using equation_system = std::pair<std::optional<equality>,
				  std::set<inequality, subtree_less<node>>>;

/**
 * @typedef inequality_system
 * @brief Alias for a set of inequalities.
 */
template<NodeType node>
using inequality_system = subtree_set<node>;

/**
 * @typedef minterm_system
 * @brief Alias for a set of inequalities.
 */
template<NodeType node>
using minterm_system = subtree_set<node>;

/**
 * @typedef solution
 * @brief Alias for a map of variables to tau templates.
 */
template<NodeType node>
using solution = subtree_map<node, tref>;

/**
 * @enum solver_mode
 * @brief Enumeration for solver modes.
 */
enum solver_mode {
	maximum, /**< Maximum mode */
	minimum, /**< Minimum mode */
	general  /**< General mode */
};

/**
 * @struct fresh_element_ledger
 * @brief Registry of atomless-witness generators minted disjoint from a
 * finite exclusion set, permanently independent of it (TABA, Homomorphisms
 * and Hemimorphisms) as long as later questions use a recorded superset of
 * the mint-time exclusion set (monotone scope).
 *
 * Membership is by `tref` identity (hash-consed), keeping this struct
 * untemplated on `node`.
 */
struct fresh_element_ledger {
	struct generator {
		size_t index; /**< Position in generators, returned by mint() */
		tref value; /**< The minted splitter share */
		trefs disjoint_from; /**< Exclusion set in scope at mint time */
	};
	std::vector<generator> generators;

	// GC roots: this ledger's storage is invisible to interpreter::
	// collect_live_refs' `keep` set, so per-run values must be pinned here
	// against a later bintree<node>::gc() sweep. htref stays untemplated
	// (plain htree::sp); the tref<->htref conversion happens at the call
	// site in solver.tmpl.h.
	htrefs pins;

	// Records a generator built disjoint from disjoint_from at mint time.
	// Pinning is the caller's job (pin(), above) -- this method only
	// compares/copies by identity, never dereferences.
	size_t mint(tref value, trefs disjoint_from) {
		size_t index = generators.size();
		generators.push_back({ index, value, std::move(disjoint_from) });
		return index;
	}

	// Root h against the next bintree<node>::gc() sweep for the rest of
	// this run.
	void pin(htref h) { pins.push_back(std::move(h)); }

	// Monotone-scope invariant: exclusion must be a recorded superset of
	// the generator's mint-time disjoint-from set.
	void consult([[maybe_unused]] size_t index,
			[[maybe_unused]] const trefs& exclusion) const {
		DBG(for (tref e : generators.at(index).disjoint_from)
			assert(std::find(exclusion.begin(), exclusion.end(), e)
				!= exclusion.end());)
	}

	// Values actually committed as a step's output (ledger_commit_witness),
	// distinct from `generators` (mostly the ladder's internal bookkeeping).
	// A row's target found here needs no disjointness re-check (freeness);
	// anything else does. Keyed on the tref bit pattern, not the tref
	// itself (comparing unrelated pointers with < is UB).
	std::map<uintptr_t, size_t> committed_index;

	// Marks value as ledger-expressed for the rest of this run, recording a
	// generator entry (disjoint_from = every value committed before it) so
	// consult() can still answer a later monotone-scope question about it.
	// Returns the generator index.
	size_t register_committed(tref value) {
		trefs committed;
		committed.reserve(committed_index.size());
		for (auto& [key, idx] : committed_index)
			committed.push_back(generators.at(idx).value);
		size_t index = mint(value, std::move(committed));
		committed_index.emplace(reinterpret_cast<uintptr_t>(value), index);
		return index;
	}

	// True iff value was registered via register_committed this run --
	// category (a) in the fast exclusion path's partition.
	bool is_committed(tref value) const {
		return committed_index.contains(reinterpret_cast<uintptr_t>(value));
	}

	// Generator index of a previously committed value, for consult().
	std::optional<size_t> committed_generator(tref value) const {
		auto it = committed_index.find(reinterpret_cast<uintptr_t>(value));
		if (it == committed_index.end()) return std::nullopt;
		return it->second;
	}

	// The still-fully-fresh region, shrunk by every register_committed so a
	// further split of it stays disjoint from everything committed
	// (freeness); null until the first commit means "the whole carrier".
	// Held as htref and reassigned (never mutated) on every shrink: each
	// new region structurally contains every prior value, so rooting only
	// the latest is enough -- no separate per-value pin needed.
	htref fresh_region;
};

/**
 * @struct solver_options
 * @brief Structure for solver options.
 */
struct solver_options {
	tref splitter_one = nullptr; /**< Splitter option */
	solver_mode mode = solver_mode::general; /**< Solver mode option */
	size_t type_id = 0;
	fresh_element_ledger* ledger = nullptr; /**< Fresh-generator ledger, per interpreter run; nullptr = today's behavior */
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__SOLVER_TYPES_H__
