// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

#ifndef __EXPERIMENTAL_EXECUTION_H__
#define __EXPERIMENTAL_EXECUTION_H__

#include <string>
#include <type_traits>
#include <optional>
#include <boost/log/trivial.hpp>

#include "parser.h"
#include "rewriting.h"

/*#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG*/

namespace idni::tau::experimental {

// Check https://en.cppreference.com/w/cpp/utility/functional/function for more
// information about std::function and how to use it.

template<typename N, typename F>
auto operator|(const rewriter::sp_node<N>& n, F f) {
	return f(n);
}

template<typename N, typename F, typename S>
auto operator|(const rewriter::sp_node<N>& n, const std::pair<F, S>& p) {
	return n | p.first | p.second;
}

template<typename F, typename S>
auto operator|(const F f, const S s) {
	return std::make_pair(f, s);
}

template<typename F>
struct repeat {

	repeat(F f) : f(f) {}

	auto operator()(const auto& n) const {
		auto current = n;
		auto next = f(current);
		while (current != next) {
			current = next;
			next = f(current);
		}
		return current;
	}

	F f;
};

template<typename N, typename selector_t, typename function_t>
struct apply_all {

	apply_all(selector_t s, function_t f) : s(s), f(f) {}

	N operator()(const N& n) const {
		std::map<N, N> changes;
		for (auto& ns: s(n)){
			changes[n] = f(n);
		}
		return replace(n, changes);
	}

	selector_t s;
	function_t f;
};

template<typename N, typename predicate_t>
struct top {

	top(predicate_t p) : p(p) {}

	std::vector<N> operator()(const N& n) const {
		return select_top(n, p);
	}

	predicate_t p;
};

template<typename N, typename predicate_t>
struct bottom {

	bottom(predicate_t p) : p(p) {}

	std::optional<N> operator()(const N& n) const {
		return find_bottom(n, p);
	}

	predicate_t p;
};

template<typename N, typename predicate_t>
struct satisfying {

	satisfying(predicate_t p) : p(p) {}

	std::vector<N> operator()(const N& n) const {
		return select_all(n, p);
	}

	predicate_t p;
};

} // namespace idni::tau::experimental

#endif // __EXPERIMENTAL_EXECUTION_H__