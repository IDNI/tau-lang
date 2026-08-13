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

} // namespace idni::tau_lang

template<idni::tau_lang::NodeType node>
std::size_t std::hash<idni::tau_lang::rr<node>>::operator()(
	const idni::tau_lang::rr<node>& rr) const noexcept {
	// htref is a shared_ptr<htree>: hashing it directly (as the generic
	// hash_combine(seed, rr.rec_relations, rr.main) used to) falls
	// through to std::hash<shared_ptr<htree>>, which hashes the raw
	// pointer -- non-reproducible across processes/allocators. This is a
	// determinism bug, not a hash-primitive choice, so it is fixed under
	// every policy. htree itself is type-erased (just a tref), so there
	// is no standalone std::hash<htree> to specialize; hash_htree<node>
	// supplies the missing type context and reads the pointed-to tree's
	// own content-derived hash instead. A non-null htref can still wrap a
	// null tref (htree::null()), which hash_tref/bintree::get() cannot
	// dereference, so both levels of null are checked before hashing.
	auto htref_hash = [](const idni::htref& h) -> std::uint64_t {
		return (h && h->get()) ? idni::hash_htree<node>{}(*h) : 0;
	};
	std::uint64_t seed = 0;
	for (const auto& [a, b] : rr.rec_relations) {
		idni::hash_combine(seed, htref_hash(a));
		idni::hash_combine(seed, htref_hash(b));
	}
	idni::hash_combine(seed, htref_hash(rr.main));
	return static_cast<size_t>(seed);
}
