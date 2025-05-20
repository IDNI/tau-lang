// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__RR_H__
#define __IDNI__TAU__RR_H__

#include "utility/tree.h"

namespace idni::tau_lang {

template <NodeType node>
struct rr {
	rr(const rewriter::rules& rec_relations, const htree::sp& main);
	rr(const htree::sp& main);

	std::weak_ordering operator<=>(const rr& that) const;
	constexpr bool     operator<  (const rr& that) const;
	constexpr bool     operator<= (const rr& that) const;
	constexpr bool     operator>  (const rr& that) const;
	constexpr bool     operator>= (const rr& that) const;
	constexpr auto     operator== (const rr& that) const;
	constexpr auto     operator!= (const rr& that) const;

	rewriter::rules rec_relations;
	htree::sp main;
};

} // namespace idni::tau_lang

#include "rr.tmpl.h"

#endif // __IDNI__TAU__RR_H__
