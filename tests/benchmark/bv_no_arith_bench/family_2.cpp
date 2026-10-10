// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Family 2, pigeonhole, a wall marker kept small: input i, holes h1..hk, and
// one part
//   always ((h1[t] | ... | hk[t]) = i[t] && atmost1(hj[t]) for all j),
// T iff k >= n. The default widths stop at 6; per width the default k is
// either side of the threshold.

#include "families.h"
#include "program.h"

#include <algorithm>
#include <stdexcept>

namespace bv_bench {

namespace {

// T: each element of i[t] goes to its own hole. F: the environment picks
// i = 1, whose n elements do not fit in k < n holes of at most one each.
std::string unrolled(unsigned n, unsigned k, unsigned T) {
	program p(n);
	const stream_id i = p.input("i");
	std::vector<stream_id> h;
	for (unsigned j = 1; j <= k; ++j)
		h.push_back(p.output("h" + std::to_string(j)));
	p.always(0, [=](instance_writer& w) {
		std::string holes = w.at(h[0], 0);
		for (unsigned j = 1; j < k; ++j) holes += " | " + w.at(h[j], 0);
		std::string body = "(" + holes + " = " + w.at(i, 0) + ")";
		for (unsigned j = 0; j < k; ++j)
			body += " && " + at_most_one(w, w.at(h[j], 0));
		return body;
	});
	return p.formula(T);
}

} // namespace

std::vector<instance> family_2(const grid& g) {
	const auto widths = g.values("width", { 1, 2, 3, 4, 5, 6 }, { 2, 3, 4 });
	const auto steps = g.values("steps", { 1, 3 }, { 1 });

	std::vector<instance> out;
	for (unsigned n : widths) {
		std::vector<unsigned> near;
		for (unsigned k = n < 2 ? 1 : n - 1; k <= n + 1; ++k)
			near.push_back(k);
		const auto ks = g.values("k", near, near);
		if (std::ranges::count(ks, 0u))
			throw std::invalid_argument("family 2 needs k >= 1");
		for (unsigned k : ks) for (unsigned T : steps)
			out.push_back({ "pigeonhole", "k=" + std::to_string(k), n, T,
				k >= n ? 'T' : 'F', unrolled(n, k, T) });
	}
	return out;
}

} // namespace bv_bench
