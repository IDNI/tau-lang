// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Family 7, prediction: is the order of the quantifiers respected? Over m
// streams, each with its own part:
//   predict (F)  always oj[t-k] = ij[t]: an output must equal a later input,
//                which the environment picks after it, and bv[n] has two
//                values for n >= 1;
//   delay   (T)  always oj[t] = ij[t-k];
//   copy    (T)  always oj[t] = ij[t].
// predict and delay have no instance below T = k; such points are skipped.

#include "families.h"
#include "program.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace bv_bench {

namespace {

// m streams, each with the part `oj[t - out_back] = ij[t - in_back]`.
std::string unrolled(unsigned n, unsigned m, unsigned T, unsigned out_back,
	unsigned in_back)
{
	program p(n);
	for (unsigned j = 1; j <= m; ++j) {
		const stream_id i = p.input("i" + std::to_string(j));
		const stream_id o = p.output("o" + std::to_string(j));
		p.always(std::max(out_back, in_back),
			[=](instance_writer& w) {
				return w.at(o, out_back) + " = " + w.at(i, in_back); });
	}
	return p.formula(T);
}

} // namespace

std::vector<instance> family_7(const grid& g) {
	const auto widths = g.values("width", { 1, 2, 4, 8, 16, 32, 64 },
		{ 1, 2, 8, 64 });
	const auto steps = g.values("steps", { 4, 8 }, { 4 });
	const auto ks = g.values("k", { 1, 2, 3, 4 }, { 1, 4 });
	const auto ms = g.values("m", { 1, 2, 4 }, { 1, 2 });
	if (std::ranges::count(ks, 0u))
		throw std::invalid_argument("family 7 needs k >= 1 (k = 0 is copy)");
	if (std::ranges::count(ms, 0u))
		throw std::invalid_argument("family 7 needs m >= 1");

	std::vector<instance> out;
	size_t skipped = 0;
	for (unsigned m : ms) for (unsigned n : widths) for (unsigned T : steps) {
		const std::string streams = "m=" + std::to_string(m);
		out.push_back({ "copy", streams, n, T, 'T', unrolled(n, m, T, 0, 0) });
		for (unsigned k : ks) {
			if (T < k) { ++skipped; continue; }
			const std::string params = "k=" + std::to_string(k)
				+ ";" + streams;
			out.push_back({ "delay", params, n, T, 'T',
				unrolled(n, m, T, 0, k) });
			out.push_back({ "predict", params, n, T, 'F',
				unrolled(n, m, T, k, 0) });
		}
	}
	if (skipped) std::cerr << "# family 7: skipped " << skipped
		<< " (k, T) points with T < k\n";
	return out;
}

} // namespace bv_bench
