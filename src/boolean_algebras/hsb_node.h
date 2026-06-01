// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_NODE_H__
#define __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_NODE_H__

#include "../../parser/hsb_parser.generated.h"
#include "../../external/parser/src/utility/hashing.h"
#include "../../external/parser/src/utility/tree.h"

namespace idni::tau_lang {

// =============================================================================
// hsb_node — value type stored in lcrs_tree<hsb_node>
//
// Stores the grammar nonterminal (nt) that identifies the formula node kind,
// and an optional data index (for hsb_hs nodes: index into hsb_halfspace_pool).
// The hash field is precomputed to satisfy bintree<T>'s fast hash path.
//
// Only 6 nonterminals appear in live hsb trees (the rest are parse-internal):
//   hsb_parser::hsb_top, hsb_bot, hsb_not, hsb_and, hsb_or, hsb_hs
// =============================================================================

struct hsb_node {
	using kind = hsb_parser::nonterminal;

	kind   nt   = hsb_parser::hsb_bot;
	size_t data = 0;    ///< Pool index for hsb_hs; 0 for all others.
	size_t hash = 0;    ///< Precomputed hash of (nt, data).

	bool operator==(const hsb_node& o) const noexcept {
		return nt == o.nt && data == o.data;
	}
	bool operator<(const hsb_node& o) const noexcept {
		if (nt != o.nt) return static_cast<size_t>(nt)
		                     < static_cast<size_t>(o.nt);
		return data < o.data;
	}
};

// Construct an hsb_node with precomputed hash.
inline hsb_node make_hsb_node(hsb_parser::nonterminal nt,
                               size_t data = 0) noexcept {
	size_t seed = 0;
	idni::hash_combine(seed, static_cast<size_t>(nt));
	idni::hash_combine(seed, data);
	return hsb_node{ nt, data, seed };
}

using hsb_tree = idni::lcrs_tree<hsb_node>;

} // namespace idni::tau_lang

// std::hash specialization required by bintree<hsb_node>
template<>
struct std::hash<idni::tau_lang::hsb_node> {
	size_t operator()(const idni::tau_lang::hsb_node& n) const noexcept {
		return n.hash;
	}
};

#endif // __IDNI__TAU__BOOLEAN_ALGEBRAS__HSB_NODE_H__
