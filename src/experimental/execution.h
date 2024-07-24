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

template<typename N>
N operator|(const N& n, const std::function<N(N)>& f) {
	return f(n);
}

template<typename N, typename...Fs>
N operator|(const N& n, const std::tuple<Fs...>& fs) {
	return std::apply([&](const auto&... funcs) {
			return (..., funcs(n));
	}, fs);
}

template<typename N, typename...Fs>
auto operator|(const std::tuple<Fs...>& fs, const std::function<N(N)>& f) {
	return std::tuple_cat(fs, std::make_tuple(f));
}

template<typename N>
auto operator|(const std::function<N(N)>& f, const std::function<N(N)>& g) {
	return make_tuple(f, g);
}

template<typename N, typename F>
struct repeat {

	repeat(F f) : f(f) {}

	N operator()(const N& n) const {
		auto nn = n;
		while (nn != f(nn)) nn = f(nn);
		return nn;
	}

	F f;
};

template<typename N, typename F>
struct for_all {

	for_all(std::function<std::vector<N>(N)> s, F f) : s(s), f(f) {}

	N operator()(const N& n) const {
		auto nn = n;
		for (auto nn = n; nn != f(nn); nn = f(nn));
		return nn;
	}

	std::function<std::vector<N>(N)> s;
	F f;
};

template<typename N>
struct top {

	top(std::function<bool(N)> p) : p(p) {}

	std::vector<N> operator()(const N& n) const {
		return select_top(n, p);
	}

	std::function<bool(N)> p;
};

template<typename N>
struct botton {

	botton(std::function<bool(N)> p) : p(p) {}

	std::vector<N> operator()(const N& n) const {
		return select_bottom(n, p);
	}

	std::function<bool(N)> p;
};

template<typename N>
struct satisfying {

	satisfying(std::function<bool(N)> p) : p(p) {}

	std::vector<N> operator()(const N& n) const {
		return select_all(n, p);
	}

	std::function<bool(N)> p;
};

} // namespace idni::tau::experimental

#endif // __EXPERIMENTAL_EXECUTION_H__