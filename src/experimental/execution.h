// LICENSE
// This software is free for use and redistribution while including this
// license notice, unless:
// 1. is used for commercial or non-personal purposes, or
// 2. used for a product which includes or associated with a blockchain or other
// decentralized database technology, or
// 3. used for a product which includes or associated with the issuance or use
// of cryptographic or electronic currencies/coins/tokens.
// On all of the mentioned cases, an explicit and written permission is required
// from the Author (Ohad Asor).
// Contact ohad@idni.org for requesting a permission. This license may be
// modified over time by the Author.

#ifndef __EXPERIMENTAL_EXECUTION_H__
#define __EXPERIMENTAL_EXECUTION_H__

#include <string>
#include <type_traits>
#include <optional>
#include <boost/log/trivial.hpp>

#include "parser.h"
#include "nso_rr.h"
#include "normal_forms.h"
#include "bdd_handle.h"
#include "variant_ba.h"

#ifdef DEBUG
#include "debug_helpers.h"
#endif // DEBUG

using namespace idni::rewriter;

namespace idni::tau::experimental {

// Check https://en.cppreference.com/w/cpp/utility/functional/function for more
// information about std::function and how to use it.

/*template<typename F, typename N>
concept node_transformer = requires(F f, N n) {
    { f(n) -> N };
};*/

template<typename N, typename F>
auto operator|(const N& n, F f) {
	return f(n);
}

template<typename N, typename...Fs>
N operator|(const N& n, const std::tuple<Fs...>& fs) {
	return std::apply([&](const auto&... funcs) {
			return (..., funcs(n));
	}, fs);
}

template<typename N, typename...Fs>
auto operator|(const std::tuple<Fs...>& fs, std::function<N(const N&)>& f) {
	return std::tuple_cat(fs, std::make_tuple(f));
}

template<typename N>
auto operator|(const std::function<N(const N&)>& f, const std::function<N(const N&)>& g) {
	return make_tuple(f, g);
}

template<typename N, typename F>
struct repeat {

	repeat(F f) : f(f) {}

	N operator()(const N& n) const {
		auto current = n, next = f(current);
		while (current != next) { current = next; next = f(current); }
		return current;
	}

	F f;
};

template<typename N>
struct for_all {

	for_all(std::function<std::vector<N>(N)> s, std::function<N(const N&)> f) : s(s), f(f) {}

	N operator()(const N& n) const {
		std::map<N, N> changes;
		for (auto& ns: s(n)){
			changes[n] = f(n);
		}
		return replace<N>(n, changes);
	}

	std::function<std::vector<N>(N)> s;
	std::function<N(const N&)> f;
};

template<typename N>
struct top {

	top(std::function<bool(const N&)> p) : p(p) {}

	std::vector<N> operator()(const N& n) const {
		return select_top(n, p);
	}

	std::function<bool(N)> p;
};

template<typename N>
struct botton {

	botton(std::function<bool(const N&)> p) : p(p) {}

	std::vector<N> operator()(const N& n) const {
		return select_bottom(n, p);
	}

	std::function<bool(N)> p;
};

template<typename N>
struct satisfying {

	satisfying(std::function<bool(const N&)> p) : p(p) {}

	std::vector<N> operator()(const N& n) const {
		return select_all(n, p);
	}

	std::function<bool(N)> p;
};

} // namespace idni::tau::experimental

#endif // __EXPERIMENTAL_EXECUTION_H__