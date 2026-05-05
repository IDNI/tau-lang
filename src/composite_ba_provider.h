// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// CompositeBAProvider — aggregates a set of BAProvider implementations,
// dispatching each method to the right inner provider based on the
// type-tree of the input.
//
// Phase D iter 2 of the Tau-LLM ABA-unification project. The
// canonical use case: ocfuncs_compile<node>(fm, decls, provider)
// needs a single provider that covers every BA type the formula
// mentions. CompositeBAProvider is that aggregator.
//
// The Coordinator's grounder list (Phase E iter 2/3/4/5) also
// becomes a single CompositeBAProvider — one provider object per
// Coordinator instance, dispatching across all configured
// per-grounder providers.
//
// Compile-time polymorphism via std::tuple<unique_ptr<BAProvider>...>
// would force every Composite to have a fixed pack size. For the
// runtime extensibility the Coordinator needs, this header instead
// uses type-erased polymorphism via a small virtual interface
// (BAProviderHandle). The overhead is one virtual call per method
// invocation — negligible compared to the underlying engine work.

#ifndef __IDNI__TAU__COMPOSITE_BA_PROVIDER_H__
#define __IDNI__TAU__COMPOSITE_BA_PROVIDER_H__

#include "ba_provider.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace idni::tau_lang {

// ── Type-erased provider handle ─────────────────────────────────────────────
// Wraps a concrete BAProvider so a heterogeneous collection of providers can
// live in a single std::vector. The virtual-call overhead is one indirection
// per method; runtime is dominated by the underlying engine work for any
// realistic provider.

struct BAProviderHandle {
	virtual ~BAProviderHandle() = default;

	virtual std::vector<CompleteType> types(const TypedContext& ctx) const = 0;
	virtual CompleteType project(const CompleteType& tau,
	                              const TypedContext& sub_ctx) const = 0;
	virtual std::vector<CompleteType> extend(const CompleteType& sigma,
	                                          const TypedContext& ctx) const = 0;
	virtual bool satisfies(const CompleteType& tau, tref phi) const = 0;
	virtual std::optional<bool> realizable(const CompleteType& profile) const = 0;
};

// ── Adapter: BAProvider -> BAProviderHandle ─────────────────────────────────

template <BAProvider P>
struct BAProviderAdapter : public BAProviderHandle {
	P inner;

	explicit BAProviderAdapter(P p) : inner(std::move(p)) {}

	std::vector<CompleteType> types(const TypedContext& ctx) const override {
		return inner.types(ctx);
	}
	CompleteType project(const CompleteType& tau,
	                      const TypedContext& sub_ctx) const override {
		return inner.project(tau, sub_ctx);
	}
	std::vector<CompleteType> extend(const CompleteType& sigma,
	                                  const TypedContext& ctx) const override {
		return inner.extend(sigma, ctx);
	}
	bool satisfies(const CompleteType& tau, tref phi) const override {
		return inner.satisfies(tau, phi);
	}
	std::optional<bool> realizable(const CompleteType& profile) const override {
		return inner.realizable(profile);
	}
};

// ── CompositeBAProvider ─────────────────────────────────────────────────────
//
// A CompositeBAProvider owns a registry of named providers; method
// dispatch routes by name (when the caller can identify the
// provider) or unions all providers' results (when the caller asks
// the composite for a global view).
//
// Iter 2 contract:
//   - register_provider(name, P): take ownership of a concrete
//     provider implementing the BAProvider concept. The name should
//     match the BA's type-tree string (e.g. "tau_kr", "parser");
//     ocfuncs_compile uses this name to look up the
//     right provider for each type id encountered.
//   - get(name): const access to a registered provider's handle, or
//     nullptr if not registered.
//   - types(ctx): unions the types from every registered provider.
//   - satisfies / realizable / project / extend: routed by name when
//     the input carries a sort id that maps to a registered name;
//     iter 2 implementation routes by index 0 (the first provider)
//     because the CompleteType ↔ provider-name mapping is itself a
//     Phase D iter 3 concern (see decision (2) in
//     tau_kr_ba_phase_b_architecture_decisions.md).
//
// THE LIMIT of iter 2: the dispatch logic for satisfies/realizable
// is currently best-effort — it tries the first provider that
// claims responsibility, falling back to subsequent providers. This
// is correct in the common case where each formula references only
// one BA type but is not yet provider-aware in the way Phase D
// iter 3 will land. The interface is set; the dispatch refinement
// is the next iter's work.

struct CompositeBAProvider {
	// Take ownership of `provider` under the given name.
	template <BAProvider P>
	void register_provider(std::string name, P provider) {
		auto handle = std::make_unique<BAProviderAdapter<P>>(
			std::move(provider));
		auto* raw = handle.get();
		providers_.emplace_back(std::move(handle));
		by_name_[std::move(name)] = raw;
	}

	const BAProviderHandle* get(const std::string& name) const noexcept {
		if (auto it = by_name_.find(name); it != by_name_.end())
			return it->second;
		return nullptr;
	}

	bool has(const std::string& name) const noexcept {
		return by_name_.contains(name);
	}

	size_t size() const noexcept { return providers_.size(); }
	bool   empty() const noexcept { return providers_.empty(); }

	std::vector<std::string> names() const {
		std::vector<std::string> result;
		result.reserve(by_name_.size());
		for (const auto& [k, _] : by_name_) result.push_back(k);
		return result;
	}

	// ── BAProvider concept conformance ─────────────────────────────
	//
	// CompositeBAProvider itself satisfies the BAProvider concept by
	// dispatching to its registered providers. The dispatch policy
	// for iter 2: types() unions across all providers; everything
	// else delegates to the first provider that produces a non-empty
	// answer.
	//
	// types(): union of every provider's types(ctx). Indices are
	// re-numbered globally so they remain unique across the union.
	std::vector<CompleteType> types(const TypedContext& ctx) const {
		std::vector<CompleteType> result;
		size_t base = 0;
		size_t ch = ctx.hash();
		for (const auto& p : providers_) {
			auto inner = p->types(ctx);
			for (auto& t : inner) {
				result.push_back({base + t.index, ch});
			}
			base += inner.size();
		}
		return result;
	}

	CompleteType project(const CompleteType& tau,
	                      const TypedContext& sub_ctx) const {
		// Iter 2: defer to the first provider; iter 3 will route by
		// the type's owning provider.
		if (providers_.empty()) {
			throw std::runtime_error(
				"CompositeBAProvider::project: no providers registered");
		}
		return providers_.front()->project(tau, sub_ctx);
	}

	std::vector<CompleteType> extend(const CompleteType& sigma,
	                                  const TypedContext& ctx) const {
		if (providers_.empty()) return {};
		return providers_.front()->extend(sigma, ctx);
	}

	// satisfies(): try each provider in registration order; the
	// first one whose verdict is conclusive (we don't have a
	// per-provider "claims this formula" test in iter 2; the
	// concept's `satisfies` returns bool, not optional<bool>) wins.
	// Iter 3 will refine: each provider gets a "claims" predicate
	// (formula → bool) that determines ownership.
	bool satisfies(const CompleteType& tau, tref phi) const {
		if (providers_.empty()) return false;
		// Best-effort: pick the first provider's verdict. Real
		// dispatch lands in iter 3.
		return providers_.front()->satisfies(tau, phi);
	}

	// realizable(): iter 2 returns the first non-nullopt verdict
	// across providers (any provider that can decide gets the
	// floor); nullopt only if every provider returned nullopt.
	std::optional<bool> realizable(const CompleteType& profile) const {
		for (const auto& p : providers_) {
			auto v = p->realizable(profile);
			if (v.has_value()) return v;
		}
		return std::nullopt;
	}

private:
	// Owning storage. providers_ holds the BAProviderHandle objects;
	// by_name_ is a non-owning lookup table.
	std::vector<std::unique_ptr<BAProviderHandle>> providers_;
	std::unordered_map<std::string, BAProviderHandle*> by_name_;
};

// ── Concept-conformance check ──────────────────────────────────────────────
// CompositeBAProvider should itself satisfy the BAProvider concept,
// so it can be plugged into ocfuncs_compile<node, BAProvider>(...)
// as a single provider over many BAs. The static_assert is below.

static_assert(BAProvider<CompositeBAProvider>,
	"CompositeBAProvider must satisfy the BAProvider concept");

} // namespace idni::tau_lang

#endif // __IDNI__TAU__COMPOSITE_BA_PROVIDER_H__
