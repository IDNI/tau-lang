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
	// `htref` is `std::shared_ptr<htree>`, so `std::hash<htref>` hashes the
	// address of the handle rather than the tree it denotes, while
	// `rr::operator==` compares tree content (see `compare_trees` above).
	// Structurally equal trees then receive different hashes, hash consing
	// stops merging them, and node identity - together with every decision
	// taken on it - depends on the heap layout of the individual run.
	// Both members are affected: `rec_relations` is a
	// `vector<pair<htref, htref>>`. The tree nodes already carry a
	// content-derived hash; use it.
	auto h = [](const idni::htref& r) noexcept -> size_t {
		return r ? idni::hash_tref<node>{}(r->get()) : 0;
	};
	size_t seed = rr.rec_relations.size();   // mirrors std::hash<vector<T>>
	for (const auto& [head, body] : rr.rec_relations)
		idni::hash_combine(seed, h(head), h(body));
	idni::hash_combine(seed, h(rr.main));
	return seed;
}
