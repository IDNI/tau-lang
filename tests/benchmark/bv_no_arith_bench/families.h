// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// What a family hands the harness: closed formulas, each with the answer its
// generator argues for. A family reads its knob values from the grid, which
// holds what the command line gave and otherwise the family's own defaults,
// the full ones or the smoke ones.

#ifndef __IDNI__TAU__BV_NO_ARITH_BENCH__FAMILIES_H__
#define __IDNI__TAU__BV_NO_ARITH_BENCH__FAMILIES_H__

#include <algorithm>
#include <map>
#include <string>
#include <vector>

namespace bv_bench {

struct instance {
	std::string variant;
	std::string params;    // `name=value` pairs joined by `;`
	unsigned width;
	unsigned steps;        // the horizon T: steps 0..T
	char expected;         // 'T' or 'F'
	std::string formula;
};

struct grid {
	bool smoke = false;
	unsigned seed = 1;
	unsigned count = 1;
	/// `width` and `steps` from --widths and --steps, the rest from --knob.
	std::map<std::string, std::vector<unsigned>> given;

	std::vector<unsigned> values(const std::string& knob,
		const std::vector<unsigned>& full,
		const std::vector<unsigned>& smoke_values) const
	{
		if (auto it = given.find(knob); it != given.end()) return it->second;
		return smoke ? smoke_values : full;
	}
};

/// `widths` plus a - 1, a and a + 1, without 0, sorted.
inline std::vector<unsigned> around(std::vector<unsigned> widths, unsigned a) {
	for (unsigned n = a < 2 ? 1 : a - 1; n <= a + 1; ++n) widths.push_back(n);
	std::ranges::sort(widths);
	widths.erase(std::ranges::unique(widths).begin(), widths.end());
	return widths;
}

/// Family 7, prediction; knobs `k` (lookback) and `m` (streams).
std::vector<instance> family_7(const grid& g);
/// Family 8, round-robin arbiter; knobs `k` (clients) and `d` (lookback).
std::vector<instance> family_8(const grid& g);
/// Family 1, width thresholds; knobs `k` (W1 outputs) and `m` (W2 lookback).
std::vector<instance> family_1(const grid& g);
/// Family 2, pigeonhole; knob `k` (holes).
std::vector<instance> family_2(const grid& g);
/// Family 4a, generated decision-tree programs; knobs `config` (a default
/// grid row), `inputs`, `outputs`, `lookback`, `lags`, `depth`, `params` and
/// `sabotage`.
std::vector<instance> family_4a(const grid& g);
/// Family 4b, hand-written templates; knobs `template`, `variant`, `copies`
/// and `d` (the queue's lookback).
std::vector<instance> family_4b(const grid& g);

} // namespace bv_bench

#endif // __IDNI__TAU__BV_NO_ARITH_BENCH__FAMILIES_H__
