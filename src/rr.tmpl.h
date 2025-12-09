// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#include "rr.h"

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
	size_t seed = 0;
	idni::hash_combine(seed, rr.rec_relations, rr.main);
	return seed;
}
