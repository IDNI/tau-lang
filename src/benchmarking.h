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

#ifdef TAU_BENCHMARK

#define BNCHMRK(x) x

#ifndef __BENCHMARKING_H__
#define __BENCHMARKING_H__

namespace idni::tau::benchmarking {

	static std::map<std::string, size_t> counters = {
		{"rule_applications", 0}
	};

} // namespace idni::tau::benchmarking

#endif // __BENCHMARKING_H__

#else
#define BNCHMRK(x)
#endif // TAU_BENCHMARK
