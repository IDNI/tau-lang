// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__RR_H__
#define __IDNI__TAU__RR_H__

#include "utility/tree.h"

namespace idni::tau_lang {

template <NodeType node>
struct rr {
	rr(const rewriter::rules& rec_relations, const htref& main);
	rr(const htref& main);
	rr();

	std::weak_ordering operator<=>(const rr& that) const;
	constexpr bool     operator<  (const rr& that) const;
	constexpr bool     operator<= (const rr& that) const;
	constexpr bool     operator>  (const rr& that) const;
	constexpr bool     operator>= (const rr& that) const;
	constexpr auto     operator== (const rr& that) const;
	constexpr auto     operator!= (const rr& that) const;

	rewriter::rules rec_relations;
	htref main;
};

} // namespace idni::tau_lang

// Hash for tau_ba using specialization to std::hash
template <idni::tau_lang::NodeType node>
struct std::hash<idni::tau_lang::rr<node>> {
	size_t operator()(const idni::tau_lang::rr<node>& f) const
								noexcept;
};

#include "rr.tmpl.h"

#endif // __IDNI__TAU__RR_H__
