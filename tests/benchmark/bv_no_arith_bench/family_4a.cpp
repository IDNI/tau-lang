// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// Family 4a, generated programs: inputs i1.., outputs o1.., parameters p1..,
// and per output oj one `always` part, a random decision tree whose leaves
// assign oj[t] a term over the current inputs, the history at a few drawn
// lags, the outputs o1..o(j-1) of the step and the parameters, but never
// oj[t] or a later output of the step. Conditions compare such terms by =, !=
// and the Boolean-algebra order a <= b, written a & b' = 0 (the bv `<=` is
// unsigned, arithmetic); an auxiliary binder, `ex z (z = a && z rel b)` or
// `all z (z <= a -> z <= b)`, says a rel b or a <= b. A chained output's
// leaves are o(j-1)[t] & h, which entails oj[k] <= o(j-1)[k], added at a few
// steps k from that part's lookback on.
//   none      (T) at step t choose o1[t], o2[t], ... in turn: a part with an
//             instance at t forces one value from what is bound before it,
//             any other output takes 0. Each rule is total for every value
//             of the parameters, which are bound `all` in front.
//   input     (F) also (i1[k] & ... & im[k]) = 0 at a step k: the
//             environment sets every input of step k to 1.
//   conflict  (F) also a copy of oj's part at a step k >= its lookback with
//             every leaf complemented: both copies take the same branch, so
//             oj[k] = v and oj[k] = v'.
// The F arguments need only 0 != 1, so each verdict holds at every width
// n >= 1. A horizon below some part's lookback is skipped.

#include "families.h"
#include "program.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <optional>
#include <random>
#include <stdexcept>

namespace bv_bench {

namespace {

enum class tier { small, medium, practical };
const char* const tier_names[] = { "small", "medium", "practical" };

// A default grid row. A `_pct` field is the chance, in percent, of the choice
// it names.
struct config {
	tier level;
	unsigned steps, inputs, outputs, lookback, lags, depth, params;
	unsigned cond_atoms;   // comparisons per condition
	unsigned expr_atoms;   // atoms per leaf term
	unsigned aux_pct, order_pct, chain_pct;
	unsigned side_k;       // entailed constraints per chained output
};

const config configs[] = {
	//  tier              T  in out lb lag dep par ca ea aux ord chain side
	{ tier::small,        2, 1, 1,  1, 1,  1,  0,  1, 2, 30, 40,   0, 0 },
	{ tier::small,        2, 2, 1,  2, 2,  2,  0,  2, 2, 20, 30,   0, 0 },
	{ tier::small,        3, 1, 2,  2, 2,  1,  0,  1, 2, 20, 40, 100, 1 },
	{ tier::small,        3, 2, 1,  3, 2,  2,  1,  2, 3, 30, 30,   0, 0 },
	{ tier::medium,       6, 2, 2,  4, 2,  2,  0,  2, 3, 20, 30,  60, 1 },
	{ tier::medium,       8, 3, 2,  5, 3,  2,  1,  2, 3, 20, 30,  50, 2 },
	{ tier::medium,      10, 2, 1,  6, 2,  1,  0,  2, 3, 15, 35,   0, 0 },
	{ tier::practical,   50, 2, 2, 20, 4,  2,  0,  2, 3, 20, 30,  60, 3 },
	{ tier::practical,   50, 3, 2, 20, 4,  3,  0,  2, 3, 20, 30,  50, 3 },
	{ tier::practical,   50, 2, 3, 20, 4,  2,  0,  2, 3, 20, 30,  60, 3 },
};
constexpr unsigned n_configs = sizeof configs / sizeof configs[0];

const char* const sabotage_names[] = { "none", "input", "conflict" };

// The knob values of one program.
struct shape {
	unsigned inputs, outputs, lookback, lags, depth, params;
};

struct term {
	enum class kind { input, output, param, zero, one, neg, op };
	kind k = kind::zero;
	unsigned id = 0;          // stream or parameter index
	unsigned lag = 0;
	char op = 0;              // '&', '|' or '^'
	std::vector<term> args;   // neg: one; op: two
};

enum class rel { eq, neq, le, lt, nle };

struct cond {
	enum class kind { cmp, aux_ex, aux_all, neg, conj, disj };
	kind k = kind::cmp;
	rel r = rel::eq;
	std::vector<term> sides;  // cmp and aux: left, right
	std::vector<cond> args;   // neg: one; conj and disj: two
};

// A leaf assigning `value` when `branches` is empty, else
// `test ? branches[0] : branches[1]`.
struct tree {
	cond test;
	term value;
	std::vector<tree> branches;
};

// Draws by `%` rather than through the standard distributions, whose output
// differs between standard libraries: a seed gives the same programs anywhere.
class dice {
public:
	explicit dice(std::seed_seq& seq) : rng(seq) {}
	size_t below(size_t n) { return rng() % n; }
	bool chance(unsigned pct) { return below(100) < pct; }
	// k distinct values of lo..hi, ascending; hi = lo - 1 is the empty range.
	std::vector<unsigned> sample(unsigned lo, unsigned hi, unsigned k) {
		std::vector<unsigned> v(hi + 1 - lo);
		for (unsigned i = 0; i < v.size(); ++i) v[i] = lo + i;
		for (unsigned i = 0; i < k; ++i)
			std::swap(v[i], v[i + below(v.size() - i)]);
		v.resize(k);
		std::ranges::sort(v);
		return v;
	}
private:
	std::mt19937_64 rng;
};

term atom(term::kind k, unsigned id, unsigned lag) {
	return { k, id, lag, 0, {} };
}

// push_back, since an initializer list would copy the subtrees.
term unary(term a) {
	term out{ term::kind::neg, 0, 0, 0, {} };
	out.args.push_back(std::move(a));
	return out;
}

term binary(char op, term a, term b) {
	term out{ term::kind::op, 0, 0, op, {} };
	out.args.push_back(std::move(a));
	out.args.push_back(std::move(b));
	return out;
}

cond compound(cond::kind k, cond a, std::optional<cond> b = std::nullopt) {
	cond out;
	out.k = k;
	out.args.push_back(std::move(a));
	if (b) out.args.push_back(std::move(*b));
	return out;
}

struct program_gen {
	const config& c;
	const shape& sh;
	dice& d;
	std::vector<unsigned> lag_set;

	// What oj's rule may read: never oj[t] or a later output of the step.
	std::vector<term> pool(unsigned j) const {
		std::vector<term> p;
		for (unsigned s = 0; s < sh.inputs; ++s)
			p.push_back(atom(term::kind::input, s, 0));
		for (unsigned lag : lag_set) {
			for (unsigned s = 0; s < sh.inputs; ++s)
				p.push_back(atom(term::kind::input, s, lag));
			for (unsigned s = 0; s < sh.outputs; ++s)
				p.push_back(atom(term::kind::output, s, lag));
		}
		for (unsigned s = 0; s < j; ++s)
			p.push_back(atom(term::kind::output, s, 0));
		for (unsigned k = 0; k < sh.params; ++k)
			p.push_back(atom(term::kind::param, k, 0));
		return p;
	}

	term pick(const std::vector<term>& p, bool constants) {
		const size_t i = d.below(p.size() + (constants ? 2 : 0));
		term a = i < p.size() ? p[i] : atom(i == p.size()
			? term::kind::zero : term::kind::one, 0, 0);
		return d.chance(30) ? unary(std::move(a)) : a;
	}

	term expr(const std::vector<term>& p, unsigned atoms) {
		term e = pick(p, false);
		for (unsigned i = 1; i < atoms; ++i) {
			term a = pick(p, true);
			const char op = "&&||^"[d.below(5)];
			e = d.chance(50) ? binary(op, std::move(e), std::move(a))
				: binary(op, std::move(a), std::move(e));
		}
		return e;
	}

	cond comparison(const std::vector<term>& p) {
		term l = expr(p, 2);
		term r = d.chance(70) ? expr(p, 1) : atom(term::kind::zero, 0, 0);
		cond out;
		out.sides.push_back(std::move(l));
		out.sides.push_back(std::move(r));
		if (d.chance(c.aux_pct)) {
			if (d.chance(50)) out.k = cond::kind::aux_all;
			else {
				out.k = cond::kind::aux_ex;
				out.r = std::array{ rel::eq, rel::neq, rel::le,
					rel::lt }[d.below(4)];
			}
		} else out.r = d.chance(c.order_pct)
			? std::array{ rel::le, rel::lt, rel::nle }[d.below(3)]
			: std::array{ rel::eq, rel::neq }[d.below(2)];
		return out;
	}

	cond condition(const std::vector<term>& p) {
		cond out = comparison(p);
		for (unsigned i = 1; i < c.cond_atoms; ++i) {
			cond a = comparison(p);
			if (d.chance(20)) a = compound(cond::kind::neg, std::move(a));
			out = compound(d.chance(50) ? cond::kind::conj
				: cond::kind::disj, std::move(out), std::move(a));
		}
		return out;
	}

	tree rule(const std::vector<term>& p, unsigned j, unsigned depth,
		bool chained)
	{
		tree out;
		if (depth == 0) {
			out.value = expr(p, c.expr_atoms);
			if (chained) out.value = binary('&',
				atom(term::kind::output, j - 1, 0), std::move(out.value));
			return out;
		}
		out.test = condition(p);
		out.branches.push_back(rule(p, j, depth - 1, chained));
		out.branches.push_back(rule(p, j, depth - 1, chained));
		return out;
	}
};

template <typename F> void each_atom(const term& e, F&& f) {
	if (e.args.empty()) f(e);
	for (const term& a : e.args) each_atom(a, f);
}

template <typename F> void each_atom(const cond& c, F&& f) {
	for (const term& s : c.sides) each_atom(s, f);
	for (const cond& a : c.args) each_atom(a, f);
}

template <typename F> void each_atom(const tree& r, F&& f) {
	if (r.branches.empty()) return each_atom(r.value, f);
	each_atom(r.test, f);
	for (const tree& b : r.branches) each_atom(b, f);
}

// The rules of one program; lookback[j] is the deepest lag oj's rule reads.
struct generated {
	std::vector<tree> rules;
	std::vector<char> chained;
	std::vector<unsigned> lookback;
};

generated generate(const config& c, const shape& sh, dice& d) {
	program_gen pg{ c, sh, d, d.sample(1, sh.lookback, sh.lags) };
	generated g;
	g.chained.push_back(0);
	for (unsigned j = 1; j < sh.outputs; ++j)
		g.chained.push_back(d.chance(c.chain_pct));
	for (unsigned j = 0; j < sh.outputs; ++j) {
		const unsigned low = sh.depth == 0 ? 0 : sh.depth - 1;
		g.rules.push_back(pg.rule(pg.pool(j), j,
			low + d.below(sh.depth - low + 1), g.chained[j]));
	}
	// Every input and parameter occurs: a missing one is xor-ed into the
	// first leaf of o1's rule, which is never chained.
	std::vector<char> in_seen(sh.inputs), par_seen(sh.params);
	for (const tree& r : g.rules) each_atom(r, [&](const term& a) {
		if (a.k == term::kind::input) in_seen[a.id] = 1;
		if (a.k == term::kind::param) par_seen[a.id] = 1;
	});
	tree* first = &g.rules[0];
	while (!first->branches.empty()) first = &first->branches[0];
	for (unsigned s = 0; s < sh.inputs; ++s) if (!in_seen[s])
		first->value = binary('^', std::move(first->value),
			atom(term::kind::input, s, 0));
	for (unsigned k = 0; k < sh.params; ++k) if (!par_seen[k])
		first->value = binary('^', std::move(first->value),
			atom(term::kind::param, k, 0));
	for (const tree& r : g.rules) {
		unsigned deepest = 0;
		each_atom(r, [&](const term& a) {
			deepest = std::max(deepest, a.lag); });
		g.lookback.push_back(deepest);
	}
	return g;
}

struct stream_ids {
	std::vector<stream_id> in, out, par;
};

std::string bf(const term& e, const stream_ids& s, instance_writer& w) {
	switch (e.k) {
	case term::kind::input:  return w.at(s.in[e.id], e.lag);
	case term::kind::output: return w.at(s.out[e.id], e.lag);
	case term::kind::param:  return w.at(s.par[e.id], 0);
	case term::kind::zero:   return w.constant(0);
	case term::kind::one:    return w.constant(0) + "'";
	case term::kind::neg:    return "(" + bf(e.args[0], s, w) + ")'";
	case term::kind::op:     break;
	}
	return "(" + bf(e.args[0], s, w) + " " + e.op + " "
		+ bf(e.args[1], s, w) + ")";
}

std::string relation(rel r, const std::string& a, const std::string& b,
	instance_writer& w)
{
	switch (r) {
	case rel::eq:  return "(" + a + " = " + b + ")";
	case rel::neq: return "(" + a + " != " + b + ")";
	case rel::le:  return included(a, b, w);
	case rel::lt:  return "(" + included(a, b, w) + " && (" + a + " != "
		+ b + "))";
	case rel::nle: break;
	}
	return "(" + a + " & (" + b + ")' != " + w.constant(0) + ")";
}

// Locals fix the order of the fresh() calls, and so the binder names.
std::string wff(const cond& c, const stream_ids& s, instance_writer& w) {
	switch (c.k) {
	case cond::kind::cmp:
		return relation(c.r, bf(c.sides[0], s, w), bf(c.sides[1], s, w), w);
	case cond::kind::aux_ex:
	case cond::kind::aux_all: {
		const std::string z = w.fresh("z");
		const std::string zt = w.typed(z);
		const std::string a = bf(c.sides[0], s, w);
		const std::string b = bf(c.sides[1], s, w);
		if (c.k == cond::kind::aux_ex) return "(ex " + z + " ((" + zt
			+ " = " + a + ") && " + relation(c.r, zt, b, w) + "))";
		return "(all " + z + " (" + included(zt, a, w) + " -> "
			+ included(zt, b, w) + "))";
	}
	case cond::kind::neg: return "!" + wff(c.args[0], s, w);
	case cond::kind::conj:
	case cond::kind::disj: break;
	}
	const std::string l = wff(c.args[0], s, w);
	const std::string r = wff(c.args[1], s, w);
	return "(" + l + (c.k == cond::kind::conj ? " && " : " || ") + r + ")";
}

std::string rule_text(const tree& r, stream_id o, bool complemented,
	const stream_ids& s, instance_writer& w)
{
	if (r.branches.empty()) {
		const std::string v = bf(r.value, s, w);
		return "(" + w.at(o, 0) + " = "
			+ (complemented ? "(" + v + ")'" : v) + ")";
	}
	const std::string test = wff(r.test, s, w);
	const std::string then = rule_text(r.branches[0], o, complemented, s, w);
	const std::string other = rule_text(r.branches[1], o, complemented, s, w);
	return "(" + test + " ? " + then + " : " + other + ")";
}

// What one instance adds at single steps.
struct extras {
	std::vector<std::pair<unsigned, unsigned>> side;  // (output j, step)
	unsigned sabotage = 0;
	unsigned step = 0;    // of the sabotage
	unsigned target = 0;  // the output whose part the conflict copies
};

std::string unrolled(const generated& gen, const shape& sh, const extras& x,
	unsigned n, unsigned T)
{
	program p(n);
	stream_ids s;
	for (unsigned j = 1; j <= sh.inputs; ++j)
		s.in.push_back(p.input("i" + std::to_string(j)));
	for (unsigned j = 1; j <= sh.outputs; ++j)
		s.out.push_back(p.output("o" + std::to_string(j)));
	for (unsigned k = 1; k <= sh.params; ++k)
		s.par.push_back(p.parameter("p" + std::to_string(k)));
	for (unsigned j = 0; j < sh.outputs; ++j)
		p.always(gen.lookback[j], [&, j](instance_writer& w) {
			return rule_text(gen.rules[j], s.out[j], false, s, w); });
	for (const auto& [j, k] : x.side) p.at_step(k, [&, j](instance_writer& w) {
		return included(w.at(s.out[j], 0), w.at(s.out[j - 1], 0), w); });
	if (x.sabotage == 1) p.at_step(x.step, [&](instance_writer& w) {
		std::string product = w.at(s.in[0], 0);
		for (unsigned j = 1; j < sh.inputs; ++j)
			product += " & " + w.at(s.in[j], 0);
		return "((" + product + ") = " + w.constant(0) + ")";
	});
	if (x.sabotage == 2) p.at_step(x.step, [&](instance_writer& w) {
		return rule_text(gen.rules[x.target], s.out[x.target], true, s, w);
	});
	return p.formula(T);
}

} // namespace

std::vector<instance> family_4a(const grid& g) {
	std::vector<unsigned> every(n_configs);
	for (unsigned i = 0; i < n_configs; ++i) every[i] = i;
	const auto config_ids = g.values("config", every, { 0, 1, 2, 3 });
	const auto sabotages = g.values("sabotage", { 0, 1, 2 }, { 0, 1, 2 });
	for (unsigned ci : config_ids) if (ci >= n_configs)
		throw std::invalid_argument("family 4a has the configs 0.."
			+ std::to_string(n_configs - 1));
	for (unsigned sab : sabotages) if (sab > 2) throw std::invalid_argument(
		"family 4a's sabotage is 0 (none), 1 (input) or 2 (conflict)");

	std::vector<instance> out;
	size_t skipped = 0;
	for (unsigned ci : config_ids) {
		const config& c = configs[ci];
		auto knob = [&](const char* name, unsigned v) {
			return g.values(name, { v }, { v }); };
		// Fewer widths past the small tier, where a point may cost the limit.
		const auto widths = g.values("width", c.level == tier::small
			? std::vector<unsigned>{ 1, 2, 4, 8, 16, 32, 64 }
			: std::vector<unsigned>{ 1, 8, 64 }, { 1, 2, 64 });
		std::vector<shape> shapes;
		for (unsigned in : knob("inputs", c.inputs))
		for (unsigned o : knob("outputs", c.outputs))
		for (unsigned lb : knob("lookback", c.lookback))
		for (unsigned lags : knob("lags", c.lags))
		for (unsigned dep : knob("depth", c.depth))
		for (unsigned par : knob("params", c.params)) {
			if (in == 0 || o == 0) throw std::invalid_argument(
				"family 4a needs inputs >= 1 and outputs >= 1");
			shapes.push_back({ in, o, lb, std::min(lags, lb), dep, par });
		}
		for (const shape& sh : shapes)
		for (unsigned prog = 0; prog < g.count; ++prog) {
			std::seed_seq seq{ g.seed, ci, sh.inputs, sh.outputs,
				sh.lookback, sh.lags, sh.depth, sh.params, prog };
			dice d(seq);
			const generated gen = generate(c, sh, d);
			const unsigned deepest = std::ranges::max(gen.lookback);
			const std::string params = "config=" + std::to_string(ci)
				+ ";inputs=" + std::to_string(sh.inputs)
				+ ";outputs=" + std::to_string(sh.outputs)
				+ ";lookback=" + std::to_string(sh.lookback)
				+ ";lags=" + std::to_string(sh.lags)
				+ ";depth=" + std::to_string(sh.depth)
				+ ";params=" + std::to_string(sh.params)
				+ ";program=" + std::to_string(prog);
			for (unsigned T : knob("steps", c.steps)) {
				if (T < deepest) { ++skipped; continue; }
				// Draws that depend on T come from copies, so that every
				// horizon and sabotage shares the rules.
				dice dt = d;
				extras x;
				for (unsigned j = 1; j < sh.outputs; ++j) {
					if (!gen.chained[j]) continue;
					const unsigned lo = gen.lookback[j];
					for (unsigned k : dt.sample(lo, T,
						std::min(c.side_k, T - lo + 1)))
							x.side.emplace_back(j, k);
				}
				for (unsigned sab : sabotages) {
					dice ds = dt;
					x.sabotage = sab;
					x.target = sab == 2 ? ds.below(sh.outputs) : 0;
					const unsigned lo = std::max(T / 2,
						sab == 2 ? gen.lookback[x.target] : 0);
					x.step = lo + ds.below(T - lo + 1);
					const std::string variant = std::string(
						tier_names[static_cast<int>(c.level)]) + "/"
						+ sabotage_names[sab];
					for (unsigned n : widths)
						out.push_back({ variant, params, n, T,
							sab == 0 ? 'T' : 'F',
							unrolled(gen, sh, x, n, T) });
				}
			}
		}
	}
	if (skipped) std::cerr << "# family 4a: skipped " << skipped
		<< " (program, T) points with T below a part's lookback\n";
	return out;
}

} // namespace bv_bench
