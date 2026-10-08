// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Family 8, round-robin arbiter: how does the engine scale with the lookback?
// Requests r1..rk, grants g1..gk, and the parts
//   always gj[t] & gl[t] = 0 for all j < l              (lookback 0)
//   always rj[t-d] & (gj[t-d] | ... | gj[t])' = 0       (lookback d)
// and, in the variant pending, also
//   always gj[t] & (rj[t-d] | ... | rj[t])' = 0         (lookback d).
// Both are T iff d + 1 >= k, at every width, and F otherwise; F needs T >= d.
// The default horizon is T = d; a given horizon below d is skipped.

#include "families.h"
#include "program.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace bv_bench {

namespace {

// `(s[t-d] | ... | s[t])`
std::string window(instance_writer& w, stream_id s, unsigned d) {
	std::string u = "(" + w.at(s, d);
	for (unsigned back = d; back-- > 0;) u += " | " + w.at(s, back);
	return u + ")";
}

// T (d + 1 >= k): at step t, client (t mod k) + 1 gets everything (plain) or
// the requests of its window (pending), the others nothing; every window of
// d + 1 steps holds a turn of every client.
// F (d + 1 < k): the environment requests 1 for every client. Per step an
// element goes to at most one client, so over the steps 0..d some client
// misses it.
std::string unrolled(unsigned n, unsigned k, unsigned d, unsigned T,
	bool pending)
{
	program p(n);
	std::vector<stream_id> r, g;
	for (unsigned j = 1; j <= k; ++j) {
		r.push_back(p.input("r" + std::to_string(j)));
		g.push_back(p.output("g" + std::to_string(j)));
	}
	if (k > 1) p.always(0, [=](instance_writer& w) {
		std::string body;
		for (unsigned j = 0; j < k; ++j)
			for (unsigned l = j + 1; l < k; ++l) {
				if (!body.empty()) body += " && ";
				body += "(" + w.at(g[j], 0) + " & " + w.at(g[l], 0)
					+ " = " + w.constant(0) + ")";
			}
		return body;
	});
	p.always(d, [=](instance_writer& w) {
		std::string body;
		for (unsigned j = 0; j < k; ++j) {
			if (!body.empty()) body += " && ";
			body += "(" + w.at(r[j], d) + " & " + window(w, g[j], d)
				+ "' = " + w.constant(0) + ")";
		}
		return body;
	});
	if (pending) p.always(d, [=](instance_writer& w) {
		std::string body;
		for (unsigned j = 0; j < k; ++j) {
			if (!body.empty()) body += " && ";
			body += "(" + w.at(g[j], 0) + " & " + window(w, r[j], d)
				+ "' = " + w.constant(0) + ")";
		}
		return body;
	});
	return p.formula(T);
}

} // namespace

std::vector<instance> family_8(const grid& g) {
	const auto widths = g.values("width", { 1, 2, 4, 8, 16, 32, 64 },
		{ 2, 8 });
	const auto ks = g.values("k", { 2, 3, 4, 5 }, { 2, 3 });
	if (std::ranges::count(ks, 0u))
		throw std::invalid_argument("family 8 needs k >= 1");

	std::vector<instance> out;
	size_t skipped = 0;
	for (unsigned k : ks) {
		// Either side of the threshold d + 1 = k, and 0.
		std::vector<unsigned> smoke, full{ 0 };
		for (unsigned d = k < 2 ? 0 : k - 2; d <= k; ++d) {
			if (d > 0) full.push_back(d);
			if (d < k) smoke.push_back(d);
		}
		for (unsigned d : g.values("d", full, smoke)) {
			const std::string params = "k=" + std::to_string(k)
				+ ";d=" + std::to_string(d);
			const char expected = d + 1 >= k ? 'T' : 'F';
			for (unsigned T : g.values("steps", { d }, { d })) {
				if (T < d) { ++skipped; continue; }
				for (unsigned n : widths) {
					out.push_back({ "plain", params, n, T, expected,
						unrolled(n, k, d, T, false) });
					out.push_back({ "pending", params, n, T, expected,
						unrolled(n, k, d, T, true) });
				}
			}
		}
	}
	if (skipped) std::cerr << "# family 8: skipped " << skipped
		<< " (d, T) points with T < d\n";
	return out;
}

} // namespace bv_bench
