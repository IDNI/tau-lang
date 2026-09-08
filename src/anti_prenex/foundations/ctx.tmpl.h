// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

/**
 * @file ctx.tmpl.h
 * @brief Template implementations for ctx.h (package E). Included by ctx.h.
 *
 * `table_ptr` below is the ONLY place in `src/anti_prenex/` that spells
 * `#ifdef TAU_CACHE`; everything else treats a null table as a miss, a no-op
 * or a plain computation.
 */

#ifndef __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__CTX_TMPL_H__
#define __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__CTX_TMPL_H__

#include <cassert>
#include <type_traits>
#include <utility>

namespace idni::tau_lang::anti_prenexing {

// --- ctx ------------------------------------------------------------------------

template <NodeType node>
ctx<node> ctx<node>::for_component(const block& P, ba_type_id type,
	bool keep_functional)
{
	// The six knobs are the struct's default member initialisers, which
	// read options.h HERE, when this `ctx` is constructed — a knob changed
	// between two components applies to the second one (§1: the constants
	// are provisional and tunable).
	ctx c;
	c.type = type;
	c.keep_functional = keep_functional;
	c.expand_count = 0;               // §5: the one component-scoped counter
	// §5: `ctx.order ← { P[i] ↦ |P|-i }` (inner → LOWER) and
	// `ctx.prio ← { P[i] ↦ i+1 }` (inner → HIGHER), both over ranks
	// 1..|P|, with `P[0]` the OUTERMOST variable. The two conventions are
	// opposite and cannot be merged (§1).
	const int_t n = static_cast<int_t>(P.size());
	for (int_t i = 0; i < n; ++i) {
		c.order[P[i]] = n - i;
		c.prio[P[i]]  = i + 1;
	}
	return c;
}

// --- taint ----------------------------------------------------------------------

namespace detail {

/// The one mutable home of §1 `taint_count`, per node type. Never gated: the
/// counter is arithmetic, not a cache, and `taint()` is its only writer.
template <NodeType node>
size_t& taint_counter() {
	static size_t count = 0;
	return count;
}

/// Debug-only check that a key names real nodes: a bare `tref` key IS the
/// node, a `(tref, bool)` key holds one, and `cof_memo`'s `(tref, tref)`
/// holds one in EACH half — a null in either would be dereferenced by
/// `hash_lcrs_tref` in every build type.
template <typename key_t>
bool key_is_nonnull(const key_t& key) {
	if constexpr (std::is_same_v<key_t, tref>)
		return key != nullptr;
	else if constexpr (std::is_same_v<key_t, std::pair<tref, tref>>)
		return key.first != nullptr && key.second != nullptr;
	else return key.first != nullptr;
}

} // namespace detail

template <NodeType node>
size_t taint_count() { return detail::taint_counter<node>(); }

template <NodeType node>
void taint() { ++detail::taint_counter<node>(); }

// --- the tables -------------------------------------------------------------------

template <NodeType node, table T>
typename table_traits<node, T>::map_t* table_ptr() {
	using map_t = typename table_traits<node, T>::map_t;
	// A function-local static bound to a GC-registered `create_cache`
	// instance, the idiom `get_free_vars` uses (tau_tree_extractors.tmpl.h).
	// The map OBJECT outlives every sweep — a sweep assigns a rebuilt map
	// to it — so this pointer stays good; iterators and entry references
	// do not (ctx.h, LIFETIME).
	if constexpr (table_traits<node, T>::gated) {
#ifdef TAU_CACHE
		static map_t& m = tree<node>::template create_cache<map_t>();
		return &m;
#else
		return nullptr; // the table does not exist in this build
#endif
	} else {
		static map_t& m = tree<node>::template create_cache<map_t>();
		return &m;
	}
}

// --- reads, writes, the wrapper, the flush -------------------------------------------

template <NodeType node, table T>
const typename table_traits<node, T>::value_t*
find(const typename table_traits<node, T>::key_t& key) {
	DBG(assert(detail::key_is_nonnull(key));)
	auto* m = table_ptr<node, T>();
	if (!m) return nullptr;                      // gated, TAU_CACHE off
	auto it = m->find(key);
	return it == m->end() ? nullptr : &it->second;
}

template <NodeType node, table T>
std::optional<typename table_traits<node, T>::value_t>
lookup(const typename table_traits<node, T>::key_t& key) {
	if (const auto* v = find<node, T>(key)) return *v;
	return std::nullopt;
}

template <NodeType node, table T>
	requires (!table_traits<node, T>::gated)
TAU_ANTI_PRENEX_NO_DANGLING
const typename table_traits<node, T>::value_t&
store(const typename table_traits<node, T>::key_t& key,
	typename table_traits<node, T>::value_t value)
{
	DBG(assert(detail::key_is_nonnull(key));)
	auto* m = table_ptr<node, T>();              // never null: unconditional
	return m->insert_or_assign(key, std::move(value)).first->second;
}

template <NodeType node, table T>
	requires (table_traits<node, T>::gated)
void store(const typename table_traits<node, T>::key_t& key,
	typename table_traits<node, T>::value_t value)
{
	DBG(assert(detail::key_is_nonnull(key));)
	if (auto* m = table_ptr<node, T>())
		m->insert_or_assign(key, std::move(value));
}

template <NodeType node, table T, typename Compute>
	requires (table_traits<node, T>::gated)
typename table_traits<node, T>::value_t
memoised(const typename table_traits<node, T>::key_t& key, Compute&& compute)
{
	DBG(assert(detail::key_is_nonnull(key));)
	auto* m = table_ptr<node, T>();
	if (!m) return compute();                    // no table: plain computation
	if (auto it = m->find(key); it != m->end()) return it->second;
	if constexpr (table_traits<node, T>::taint_aware) {
		// §1 cache scope, TAINT: a computation that hit a budget returns
		// its result (sound, inv. 3) and writes no entry. Transitivity is
		// free — a hit inside the computation is a hit inside every
		// enclosing one, so every wrapper up the stack sees the counter
		// move too.
		const size_t before = taint_count<node>();
		auto value = compute();
		if (taint_count<node>() == before)
			m->insert_or_assign(key, value);
		return value;
	} else {
		auto value = compute();
		m->insert_or_assign(key, value);
		return value;
	}
}

template <NodeType node, table T, typename Compute>
	requires (!table_traits<node, T>::gated)
TAU_ANTI_PRENEX_NO_DANGLING
const typename table_traits<node, T>::value_t&
memoised(const typename table_traits<node, T>::key_t& key, Compute&& compute)
{
	static_assert(!table_traits<node, T>::taint_aware,
		"a structural facet is a pure function of its node: a budget hit "
		"cannot change it, so the taint rule does not apply");
	DBG(assert(detail::key_is_nonnull(key));)
	auto* m = table_ptr<node, T>();              // never null: unconditional
	if (auto it = m->find(key); it != m->end()) return it->second;
	// `compute()` may fill the same table for this node's children; those
	// insertions leave existing entries where they are, and the iterator
	// below is taken afterwards, so nothing here can dangle (ctx.h,
	// LIFETIME).
	auto value = compute();
	return m->insert_or_assign(key, std::move(value)).first->second;
}

namespace detail {

/// One step of the flush sweep: clear table `T` if its policy says so.
template <NodeType node, table T>
void clear_if_solver_flushed() {
	if constexpr (table_traits<node, T>::solver_flushed)
		if (auto* m = table_ptr<node, T>()) m->clear();
}

} // namespace detail

template <NodeType node>
void flush_solver_dependent() {
	// Driven by the `solver_flushed` flag over `all_tables` rather than by
	// a hand-written list, so a table added later cannot be missed here.
	[]<size_t... I>(std::index_sequence<I...>) {
		(detail::clear_if_solver_flushed<node, all_tables[I]>(), ...);
	}(std::make_index_sequence<all_tables.size()>{});
}

} // namespace idni::tau_lang::anti_prenexing

#endif // __IDNI__TAU__ANTI_PRENEX__FOUNDATIONS__CTX_TMPL_H__
