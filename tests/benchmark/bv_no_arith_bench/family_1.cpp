// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Family 1, width thresholds: does the finite engine find the width at which a
// program turns satisfiable? Outputs only:
//   W1          o1..ok, always (oj[t] != 0 for all j && oj[t] & ol[t] = 0
//               for j < l): T iff n >= k;
//   W1-persist  W1 and always oj[t] = oj[t-1]: the same verdict, T >= 1;
//   W2          o, always (single(o[t]) && o[t] != o[t-1] && ... &&
//               o[t] != o[t-m]): T iff n >= m + 1, and F needs T >= 2m.
// The default widths add those either side of each threshold; W2's default
// horizons are 2m and 3m, and a given one below 2m is skipped.

#include "families.h"
#include "program.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace bv_bench {

namespace {

// T: oj[t] = {j}. F: k nonempty pairwise disjoint subsets of an n-set need
// n >= k. The persistence part keeps that constant choice valid.
std::string w1(unsigned n, unsigned k, unsigned T, bool persist) {
	program p(n);
	std::vector<stream_id> o;
	for (unsigned j = 1; j <= k; ++j)
		o.push_back(p.output("o" + std::to_string(j)));
	p.always(0, [=](instance_writer& w) {
		std::string body;
		for (unsigned j = 0; j < k; ++j) {
			if (!body.empty()) body += " && ";
			body += "(" + w.at(o[j], 0) + " != " + w.constant(0) + ")";
		}
		for (unsigned j = 0; j < k; ++j)
			for (unsigned l = j + 1; l < k; ++l)
				body += " && (" + w.at(o[j], 0) + " & " + w.at(o[l], 0)
					+ " = " + w.constant(0) + ")";
		return body;
	});
	if (persist) p.always(1, [=](instance_writer& w) {
		std::string body;
		for (unsigned j = 0; j < k; ++j) {
			if (!body.empty()) body += " && ";
			body += "(" + w.at(o[j], 0) + " = " + w.at(o[j], 1) + ")";
		}
		return body;
	});
	return p.formula(T);
}

// T: o[t] = {t mod (m + 1)}, distinct from the m before it. F: the steps
// m..2m are m + 1 singletons, any two at most m apart, so pairwise distinct.
std::string w2(unsigned n, unsigned m, unsigned T) {
	program p(n);
	const stream_id o = p.output("o");
	p.always(m, [=](instance_writer& w) {
		const std::string now = w.at(o, 0);
		std::string body = "(" + now + " != " + w.constant(0) + ") && "
			+ at_most_one(w, now);
		for (unsigned back = 1; back <= m; ++back)
			body += " && (" + now + " != " + w.at(o, back) + ")";
		return body;
	});
	return p.formula(T);
}

// `widths` plus a - 1, a and a + 1, without 0.
std::vector<unsigned> around(std::vector<unsigned> widths, unsigned a) {
	for (unsigned n = a < 2 ? 1 : a - 1; n <= a + 1; ++n) widths.push_back(n);
	std::ranges::sort(widths);
	widths.erase(std::ranges::unique(widths).begin(), widths.end());
	return widths;
}

} // namespace

std::vector<instance> family_1(const grid& g) {
	const std::vector<unsigned> defaults{ 1, 2, 4, 8, 16, 32, 64 };
	const auto ks = g.values("k", { 2, 3, 5, 8 }, { 3, 5 });
	const auto ms = g.values("m", { 1, 2, 3, 4 }, { 1, 2 });
	if (std::ranges::count(ks, 0u))
		throw std::invalid_argument("family 1 needs k >= 1");
	if (std::ranges::count(ms, 0u))
		throw std::invalid_argument("family 1 needs m >= 1");

	std::vector<instance> out;
	const auto w1_steps = g.values("steps", { 1, 3 }, { 1 });
	for (unsigned k : ks) {
		const auto widths = g.values("width", around(defaults, k),
			around({}, k));
		const std::string params = "k=" + std::to_string(k);
		for (unsigned n : widths) for (unsigned T : w1_steps) {
			const char expected = n >= k ? 'T' : 'F';
			out.push_back({ "W1", params, n, T, expected,
				w1(n, k, T, false) });
			if (T >= 1) out.push_back({ "W1-persist", params, n, T,
				expected, w1(n, k, T, true) });
		}
	}
	size_t skipped = 0;
	for (unsigned m : ms) {
		const auto widths = g.values("width", around(defaults, m + 1),
			around({}, m + 1));
		const auto steps = g.values("steps", { 2 * m, 3 * m }, { 2 * m });
		const std::string params = "m=" + std::to_string(m);
		for (unsigned T : steps) {
			if (T < 2 * m) { ++skipped; continue; }
			for (unsigned n : widths) out.push_back({ "W2", params, n, T,
				n >= m + 1 ? 'T' : 'F', w2(n, m, T) });
		}
	}
	if (skipped) std::cerr << "# family 1: skipped " << skipped
		<< " (m, T) points with T < 2m\n";
	return out;
}

} // namespace bv_bench
