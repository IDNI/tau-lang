// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __IDNI__TAU__RR_H__
#define __IDNI__TAU__RR_H__

#include "tau.h"

namespace idni::tau_lang {

struct rr {
	rr(const rewriter::rules& rec_relations, const htree::sp& main)
				: rec_relations(rec_relations), main(main) {};
	rr(const htree::sp& main) : main(main) {};

	auto operator<=>(const rr&) const = default;

	rewriter::rules rec_relations;
	htree::sp main;
};

} // namespace idni::tau_lang

#endif // __IDNI__TAU__RR_H__
