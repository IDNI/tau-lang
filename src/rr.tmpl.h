// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#include "rr.h"

#undef LOG_CHANNEL_NAME
#define LOG_CHANNEL_NAME "rr"

namespace idni::tau_lang {

template <NodeType node>
rr<node>::rr(const rewriter::rules& rec_relations, const htref& main)
				: rec_relations(rec_relations), main(main) {};

template <NodeType node>
rr<node>::rr(const htref& main) : main(main) {}

template<NodeType node>
rr<node>::rr(){}

// Content-based weak ordering of two optional tree handles, comparing
// the trees themselves, never the handle addresses. Convention: nullptr
// orders first — an empty handle is less than any non-empty one, and two
// empty handles are equivalent. rr::operator<=> relies on this so rrs
// with unset main or rule slots compare without dereferencing them.
template <NodeType node>
std::weak_ordering compare_trees(const htref& a, const htref& b) {
	if (a && b) {
		const auto& a_tree = tree<node>::get(a->get());
		const auto& b_tree = tree<node>::get(b->get());
		if (a_tree < b_tree) return std::weak_ordering::less;
		if (b_tree < a_tree) return std::weak_ordering::greater;
		return std::weak_ordering::equivalent;
	} else if (a) {
		return std::weak_ordering::greater;
	} else if (b)
		return std::weak_ordering::less;
	return std::weak_ordering::equivalent;
}

template <NodeType node>
std::weak_ordering rr<node>::operator<=>(const rr<node>& that) const {
	if (rec_relations.size() != that.rec_relations.size())
		return rec_relations.size() <=> that.rec_relations.size();
	for (size_t i = 0; i < rec_relations.size(); ++i) {
		const auto& r1 = rec_relations[i];
		const auto& r2 = that.rec_relations[i];
		std::weak_ordering c = compare_trees<node>(r1.first, r2.first);
		if (c != 0) return c;
		c = compare_trees<node>(r1.second, r2.second);
		if (c != 0) return c;
	}
	return compare_trees<node>(main, that.main);
}

template <NodeType node>
constexpr bool rr<node>::operator<(const rr<node>& that) const {
	return (*this <=> that) < 0;
}

template <NodeType node>
constexpr bool rr<node>::operator<=(const rr<node>& that) const {
	return (*this <=> that) <= 0;
}

template <NodeType node>
constexpr bool rr<node>::operator>(const rr<node>& that) const {
	return (*this <=> that) > 0;
}

template <NodeType node>
constexpr bool rr<node>::operator>=(const rr<node>& that) const {
	return (*this <=> that) >= 0;
}

template <NodeType node>
constexpr auto rr<node>::operator==(const rr<node>& that) const {
	return (*this <=> that) == 0;
}

template <NodeType node>
constexpr auto rr<node>::operator!=(const rr<node>& that) const {
	return !(*this == that);
}

template <NodeType node>
std::uint64_t rr_hash(const rr<node>& r) noexcept {
	// Hash the trees' content, never the `htref` handles: htref is a
	// shared_ptr<htree>, and std::hash of it hashes the raw pointer,
	// which is not reproducible across processes. The tree's own 64-bit
	// hash is read: std::hash of a tree is a size_t and keeps only 32
	// bits on wasm32. A non-null htref can still wrap a null tref
	// (htree::null()), so both levels of null are checked.
	auto htref_hash = [](const idni::htref& h) -> std::uint64_t {
		return (h && h->get()) ? tree<node>::get(h->get()).hash : 0;
	};
	std::uint64_t seed = 0;
	for (const auto& [a, b] : r.rec_relations) {
		idni::hash_combine(seed, htref_hash(a));
		idni::hash_combine(seed, htref_hash(b));
	}
	idni::hash_combine(seed, htref_hash(r.main));
	return seed;
}

} // namespace idni::tau_lang

template<idni::tau_lang::NodeType node>
std::size_t std::hash<idni::tau_lang::rr<node>>::operator()(
	const idni::tau_lang::rr<node>& rr) const noexcept {
	return static_cast<size_t>(idni::tau_lang::rr_hash(rr));
}
