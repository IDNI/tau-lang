// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// ltl_aba_data_game.tmpl.h - The synthesis game of the skeleton played on the data

namespace idni::tau_lang {

// ── The data game ────────────────────────────────────────────────────────────
//
// `ltlsynt --print-game-hoa` gives the parity game of the plain skeleton: an
// environment vertex reads the input props, a system vertex the output props,
// and the edges of each vertex cover every valuation of its props. Here that
// game is played on the data instead of on the props. A vertex's region is a
// set of values of the last L steps of every stream (L = the deepest lookback
// of the atoms) and, at a system vertex, of the inputs of the current step:
// the environment picks the inputs of a step after the history is fixed, the
// system then picks the outputs, and the atoms of an edge label are
// evaluated on those values. A prop that is no atom (a step counter bit, a
// step guard, a past tester) is the system's to set, so it is quantified
// away inside each cube of a label.
//
// Regions are computed with Zielonka's algorithm, every attractor a fixpoint
// of the controllable predecessor. A predecessor quantifies the choosing
// player's current values: ex for the player it attracts for, all for the
// other one, whose moves out of the subgame do not count. A valuation no
// edge admits ends the play: the vertex's owner loses, which two sink
// vertices of the right parity model. The system wins when the steps
// before step 0, each played like any other step, can reach the initial
// vertex's region (data_quantifier::reached_before_start).
//
// When every stream has a two-element type or is read only through
// equalities, a region is a BDD over codes of the window's values
// (code_window, code_regions); otherwise it is a formula whose quantifiers
// the normalizer eliminates (formula_regions), and one it leaves standing
// makes the game undecided. The formulas over a fixed set of variables and
// constants are finitely many up to equivalence, so each fixpoint ends; the
// rounds of a formula fixpoint are still capped by
// ltl_max_refinement_rounds().

enum class data_game_verdict { realizable, unrealizable, undecided };

// The arena: the game vertices, two sinks and a vertex per coloured target.
// A label is a formula over the atoms of one step.
template <NodeType node>
struct data_arena {
	enum class chooser { none, inputs, outputs };
	struct edge { tref label; int dst; bool shift; };
	struct vertex {
		int owner = 0;        // 0 environment, 1 system
		int priority = 0;     // max parity, odd favours the system
		chooser picks = chooser::none;
		std::vector<edge> edges;
	};
	std::vector<vertex> v;
	int init = 0;
};

// Builds the arena of `game` over `atoms`; false when a label cannot be read
// or the acceptance is not a parity condition.
template <NodeType node>
static bool build_data_arena(data_arena<node>& a, const alg_d::synth_game& game,
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	using tau = tree<node>;
	using arena = data_arena<node>;
	using chooser = typename arena::chooser;
	if (!game.acc_known || game.multi_colored || game.num_states <= 0)
		return false;
	if ((int)game.player.size() != game.num_states || game.init < 0
		|| game.init >= game.num_states || game.player[game.init] != 0)
			return false;
	std::vector<tref> atom_of(game.aps.size(), nullptr);
	for (size_t i = 0; i < game.aps.size(); ++i)
		for (auto& [fm, name] : atoms)
			if (name == game.aps[i]) { atom_of[i] = fm; break; }
	// Colours sit two above the priority of a run that sees none.
	const int uncolored = game.acc_accepts_uncolored ? 1 : 0;
	auto prio = [&](int color, int priority) {
		return color < 0 ? uncolored : priority + 2;
	};
	const int n = game.num_states;
	const int sys_sink = n, env_sink = n + 1;
	// one vertex per coloured target: it reads its target's region
	std::map<std::pair<int, int>, int> colored;
	a.v.resize(n + 2);
	a.init = game.init;
	a.v[sys_sink] = { 0, 1, chooser::none, { { tau::_T(), sys_sink, false } } };
	a.v[env_sink] = { 0, 0, chooser::none, { { tau::_T(), env_sink, false } } };
	for (int q = 0; q < n; ++q) {
		const int owner = game.player[q] == 1 ? 1 : 0;
		a.v[q].owner = owner;
		a.v[q].priority = prio(game.state_color[q], game.state_priority[q]);
		a.v[q].picks = owner ? chooser::outputs : chooser::inputs;
		const bool shift = owner == 1;
		tref covered = tau::_F();
		for (size_t j = 0; j < game.trans[q].size(); ++j) {
			const auto& [guard, next, color] = game.trans[q][j];
			if (next < 0 || next >= n) return false;
			auto cubes = alg_d::hoa_guard::to_dnf(guard);
			if (!cubes) return false;
			tref label = tau::_F();
			for (const auto& c : *cubes) {
				tref conj = tau::_T();
				for (const auto& l : c) {
					// a label reads its owner's props only
					if (l.ap < 0 || l.ap >= (int)atom_of.size()
						|| l.ap >= (int)game.controllable.size()
						|| game.controllable[l.ap] != (owner == 1))
							return false;
					if (!atom_of[l.ap]) continue;
					conj = tau::build_wff_and(conj, l.pos ? atom_of[l.ap]
						: tau::build_wff_neg(atom_of[l.ap]));
				}
				label = tau::build_wff_or(label, conj);
			}
			label = shift_io_vars<node>(label, 0);
			covered = tau::build_wff_or(covered, label);
			int dst = next;
			const int ep = game.edge_priority[q][j];
			if (color >= 0 && ep >= 0) {
				auto [it, fresh] = colored.emplace(
					std::pair{ next, ep }, (int)a.v.size());
				if (fresh) a.v.push_back({ 0, prio(color, ep),
					chooser::none, { { tau::_T(), next, false } } });
				dst = it->second;
			}
			a.v[q].edges.push_back({ label, dst, shift });
		}
		a.v[q].edges.push_back({ tau::build_wff_neg(covered),
			owner ? env_sink : sys_sink, shift });
	}
	return true;
}

// Regions as formulas over the io_vars of the window.
template <NodeType node>
struct formula_regions {
	using tau = tree<node>;
	using region = tref;
	using arena = data_arena<node>;
	const arena& a;
	bool failed = false;
	data_quantifier<node> dq;
	std::map<tref, tref> normal;

	explicit formula_regions(const arena& ar) : a(ar) {}

	tref norm(tref f) {
		if (failed) return tau::_F();
		const auto& t = tau::get(f);
		if (t.equals_T() || t.equals_F()) return f;
		if (auto it = normal.find(f); it != normal.end()) return it->second;
		tref n = data_quantifier<node>::eliminate(f);
		if (!n) { failed = true; return tau::_F(); }
		normal.emplace(f, n);
		return n;
	}
	tref top() { return tau::_T(); }
	tref bottom() { return tau::_F(); }
	tref conj(tref x, tref y) {
		if (tau::get(x).equals_T() || tau::subtree_equals(x, y)) return y;
		if (tau::get(y).equals_T()) return x;
		return norm(tau::build_wff_and(x, y));
	}
	tref disj(tref x, tref y) {
		if (tau::get(x).equals_F() || tau::subtree_equals(x, y)) return y;
		if (tau::get(y).equals_F()) return x;
		return norm(tau::build_wff_or(x, y));
	}
	tref minus(tref x, tref y) {
		return norm(tau::build_wff_and(x, tau::build_wff_neg(y)));
	}

	bool empty(tref f) {
		const auto& t = tau::get(f);
		if (t.equals_F()) return true;
		if (t.equals_T()) return false;
		if (qlt_order_conj_unsat<node>(f)) return true;
		// an undecided check is no emptiness: the game fails instead
		auto sat = is_non_temp_nso_satisfiable<node>(f);
		if (!sat.has_value()) { failed = true; return true; }
		return !sat.value();
	}
	std::optional<bool> reached(tref f) {
		return dq.reached_before_start(f);
	}
	// The positions from which the chooser of vertex `i` takes edge `j`
	// into `Y`.
	tref move(int i, size_t j, const std::vector<tref>& Y) {
		const auto& e = a.v[i].edges[j];
		tref tgt = e.shift ? shift_io_vars<node>(Y[e.dst], 1) : Y[e.dst];
		return norm(tau::build_wff_and(e.label, tgt));
	}

	tref pre(int p, int i, const std::vector<tref>& Y,
		const std::vector<tref>& G)
	{
		const auto& x = a.v[i];
		const bool mine = x.owner == p;
		tref body = mine ? tau::_F() : tau::_T();
		for (const auto& e : x.edges) {
			tref tgt = mine ? Y[e.dst]
				: tau::build_wff_or(Y[e.dst], tau::build_wff_neg(G[e.dst]));
			if (e.shift) tgt = shift_io_vars<node>(tgt, 1);
			body = mine
				? tau::build_wff_or(body, tau::build_wff_and(e.label, tgt))
				: tau::build_wff_and(body,
					tau::build_wff_or(tau::build_wff_neg(e.label), tgt));
		}
		if (x.picks != arena::chooser::none) {
			auto [ins, outs] = data_quantifier<node>::current_vars(body);
			for (tref var : x.picks == arena::chooser::inputs ? ins : outs)
				body = dq.quantify(var, body, mine);
		}
		return norm(body);
	}
};

// A reduced ordered BDD without complement edges for the regions over the
// codes of a code_window; node 0 is false and node 1 true. A table grown
// past `max_nodes` sets `full`, and the game is then undecided.
struct data_bdd {
	using id = uint32_t;
	static constexpr id F = 0, T = 1;
	static constexpr uint32_t leaf = UINT32_MAX;
	struct nd { uint32_t var; id lo, hi; };
	struct key_hash {
		size_t operator()(const std::array<uint32_t, 3>& k) const {
			uint64_t h = k[0];
			h = h * 0x9E3779B97F4A7C15ull + k[1];
			h = h * 0x9E3779B97F4A7C15ull + k[2];
			return (size_t)(h ^ (h >> 29));
		}
	};
	std::vector<nd> nodes{ { leaf, F, F }, { leaf, T, T } };
	std::unordered_map<std::array<uint32_t, 3>, id, key_hash> unique, memo;
	size_t max_nodes;
	bool full = false;

	explicit data_bdd(size_t cap) : max_nodes(cap) {}

	id mk(uint32_t v, id lo, id hi) {
		if (lo == hi) return lo;
		std::array<uint32_t, 3> k{ v, lo, hi };
		if (auto it = unique.find(k); it != unique.end()) return it->second;
		if (nodes.size() >= max_nodes) { full = true; return F; }
		nodes.push_back({ v, lo, hi });
		const id n = (id)(nodes.size() - 1);
		unique.emplace(k, n);
		return n;
	}
	id var(uint32_t v, bool pos = true) {
		return pos ? mk(v, F, T) : mk(v, T, F);
	}
	// op 0 conjunction, 1 disjunction
	id apply(uint32_t op, id a, id b) {
		if (op == 0) {
			if (a == F || b == F) return F;
			if (a == T) return b;
			if (b == T || a == b) return a;
		} else {
			if (a == T || b == T) return T;
			if (a == F) return b;
			if (b == F || a == b) return a;
		}
		if (a > b) std::swap(a, b);
		std::array<uint32_t, 3> k{ op, a, b };
		if (auto it = memo.find(k); it != memo.end()) return it->second;
		const nd x = nodes[a], y = nodes[b];
		const uint32_t v = std::min(x.var, y.var);
		const id lo = apply(op, x.var == v ? x.lo : a, y.var == v ? y.lo : b);
		const id hi = apply(op, x.var == v ? x.hi : a, y.var == v ? y.hi : b);
		const id r = mk(v, lo, hi);
		memo.emplace(k, r);
		return r;
	}
	id conj(id a, id b) { return apply(0, a, b); }
	id disj(id a, id b) { return apply(1, a, b); }
	id neg(id a) {
		if (a <= T) return a == T ? F : T;
		std::array<uint32_t, 3> k{ 2, a, 0 };
		if (auto it = memo.find(k); it != memo.end()) return it->second;
		const nd x = nodes[a];
		const id r = mk(x.var, neg(x.lo), neg(x.hi));
		memo.emplace(k, r);
		return r;
	}
	// Quantifies the variables flagged in `qs`.
	id quantify(id a, const std::vector<bool>& qs, bool exists) {
		std::unordered_map<id, id> seen;
		std::function<id(id)> go = [&](id n) -> id {
			if (n <= T) return n;
			if (auto it = seen.find(n); it != seen.end()) return it->second;
			const nd x = nodes[n];
			const id lo = go(x.lo), hi = go(x.hi);
			const id r = x.var < qs.size() && qs[x.var]
				? apply(exists ? 1 : 0, lo, hi) : mk(x.var, lo, hi);
			seen.emplace(n, r);
			return r;
		};
		return go(a);
	}
	// Every variable v of `a` renamed v - d; false in `ok` when one is
	// below d.
	id lower(id a, uint32_t d, bool& ok) {
		std::unordered_map<id, id> seen;
		std::function<id(id)> go = [&](id n) -> id {
			if (n <= T) return n;
			if (auto it = seen.find(n); it != seen.end()) return it->second;
			const nd x = nodes[n];
			if (x.var < d) { ok = false; return F; }
			const id r = mk(x.var - d, go(x.lo), go(x.hi));
			seen.emplace(n, r);
			return r;
		};
		return go(a);
	}
};

// The window of code_regions. A stream of a two-element type takes one bit;
// a stream of another type read only through equalities with its own type,
// 0 and 1 takes a code of `width` bits, code 0 standing for 0, code 1 for
// 1 and every other code for a value distinct from both and from the other
// codes. Only equalities read the values, so a history counts only through
// which of its values are equal (its equality type); the codes realize
// every such type of a window, and so does the type when it has as many
// elements as a window has values plus 0 and 1 (make_code_window checks
// it), which makes the game on the codes the game on the data. The bits of
// stream s at step t-k are variables k * per_step + offset[s] + b, so
// moving a region to the next step subtracts per_step.
struct code_window {
	struct stream {
		std::string name;
		bool input = false;
		bool two = false;       // a two-element type, one bit
		size_t width = 1, offset = 0;
	};
	std::vector<stream> streams;
	std::map<std::string, size_t> index;
	size_t per_step = 0, depth = 0;
	uint32_t var(size_t s, size_t k, size_t b) const {
		return (uint32_t)(k * per_step + streams[s].offset + b);
	}
	uint32_t vars() const { return (uint32_t)((depth + 1) * per_step); }
};

// One side of an equality read on codes: an io_var (its variable node) or
// the constant 0 or 1.
template <NodeType node>
struct code_side { tref var = nullptr; int constant = -1; };

// The sides of `cmp` when it is an equality or a disequality between io_vars
// and the constants 0 and 1, else nullopt; `equal` tells which. A
// complement is read off: x' = c is x = c', and x' = y' is x = y.
template <NodeType node>
static std::optional<std::array<code_side<node>, 2>> code_equality(tref cmp,
	bool& equal)
{
	using tau = tree<node>;
	const auto& t = tau::get(cmp);
	if (!t.has_child()) return std::nullopt;
	const auto nt = t[0].value.nt;
	if ((nt != tau::bf_eq && nt != tau::bf_neq) || t[0].children_size() != 2)
		return std::nullopt;
	equal = nt == tau::bf_eq;
	std::array<code_side<node>, 2> sides;
	bool complement[2] = { false, false };
	auto var_of = [](const tree<node>& b) -> tref {
		return b.children_size() == 1 && is_child<node, tau::io_var>(b.first())
			? b.first() : nullptr;
	};
	for (size_t i = 0; i < 2; ++i) {
		const auto& x = tau::get(t[0].child(i));
		if (x.equals_0()) { sides[i].constant = 0; continue; }
		if (x.equals_1()) { sides[i].constant = 1; continue; }
		if ((sides[i].var = var_of(x))) continue;
		if (x.children_size() != 1) return std::nullopt;
		const auto& n = tau::get(x.first());
		if (!n.is(tau::bf_neg) || n.children_size() != 1
			|| !(sides[i].var = var_of(tau::get(n.first()))))
				return std::nullopt;
		complement[i] = true;
	}
	if (sides[0].var && sides[1].var) {
		if (complement[0] != complement[1]) return std::nullopt;
		return sides;
	}
	for (size_t i = 0; i < 2; ++i)
		if (complement[i]) sides[1 - i].constant = 1 - sides[1 - i].constant;
	return sides;
}

// The code window of `atoms`, when every stream has a two-element type or
// is read only through equalities; at most `max_vars` variables.
template <NodeType node>
static std::optional<code_window> make_code_window(
	const std::vector<std::pair<tref, std::string>>& atoms, size_t max_vars)
{
	using tau = tree<node>;
	data_quantifier<node> dq;
	code_window w;
	std::map<size_t, size_t> type_of;   // stream -> type id
	for (auto& [atom, _] : atoms)
		for (tref v : tau::get(atom).select_top(is_child<node, tau::io_var>)) {
			if (is_io_initial<node>(v)) return std::nullopt;
			w.depth = std::max(w.depth, (size_t)get_io_var_shift<node>(v));
			auto [it, fresh] = w.index.emplace(get_var_name<node>(v),
				w.streams.size());
			if (!fresh) continue;
			code_window::stream s;
			s.name = it->first;
			s.input = is_input_stream<node>(v);
			s.two = dq.is_two_element(v);
			w.streams.push_back(s);
			type_of[it->second] = find_ba_type<node>(v);
		}
	// A stream of another type is read only through equalities.
	for (auto& [atom, _] : atoms)
		for (tref c : tau::get(atom).select_all(is_aba_comparison<node>)) {
			auto vars = tau::get(c).select_top(is_child<node, tau::io_var>);
			if (std::all_of(vars.begin(), vars.end(), [&](tref v) {
				return w.streams[w.index.at(get_var_name<node>(v))].two; }))
					continue;
			bool equal;
			if (!code_equality<node>(c, equal)) return std::nullopt;
		}
	// Each such type needs as many values as a window holds, plus 0 and 1.
	std::map<size_t, size_t> slots;
	for (size_t s = 0; s < w.streams.size(); ++s)
		if (!w.streams[s].two) slots[type_of[s]] += w.depth + 1;
	std::map<size_t, size_t> width;
	for (auto& [tid, n] : slots) {
		size_t bits = 1;
		while ((size_t{1} << bits) < n + 2) ++bits;
		width[tid] = bits;
		tref all = tau::_T();
		trefs fresh;
		for (size_t j = 0; j < n; ++j) {
			tref x = build_out_var_at_t<node>(build_var_name<node>(
				"o__code" + std::to_string(j)), tid);
			all = tau::build_wff_and(all, tau::build_wff_and(
				tau::build_bf_neq(x, build_bf_f_type<node>(tid)),
				tau::build_bf_neq(x, build_bf_t_type<node>(tid))));
			for (tref y : fresh)
				all = tau::build_wff_and(all, tau::build_bf_neq(x, y));
			fresh.push_back(x);
		}
		auto sat = is_non_temp_nso_satisfiable<node>(all);
		if (!sat.has_value() || !sat.value()) return std::nullopt;
	}
	for (size_t s = 0; s < w.streams.size(); ++s) {
		w.streams[s].width = w.streams[s].two ? 1 : width[type_of[s]];
		w.streams[s].offset = w.per_step;
		w.per_step += w.streams[s].width;
	}
	if (w.vars() > max_vars) return std::nullopt;
	return w;
}

// Regions as BDDs over the codes of a code_window.
template <NodeType node>
struct code_regions {
	using tau = tree<node>;
	using region = data_bdd::id;
	using arena = data_arena<node>;
	const arena& a;
	const code_window& w;
	data_bdd bdd;
	bool failed = false;
	std::vector<region> labels;           // per vertex, per edge
	std::vector<size_t> edge_base;

	code_regions(const arena& ar, const code_window& win, size_t max_nodes)
		: a(ar), w(win), bdd(max_nodes) {}

	region top() { return data_bdd::T; }
	region bottom() { return data_bdd::F; }
	region conj(region x, region y) { return check(bdd.conj(x, y)); }
	region disj(region x, region y) { return check(bdd.disj(x, y)); }
	region minus(region x, region y) {
		return check(bdd.conj(x, bdd.neg(y)));
	}
	bool empty(region r) { return r == data_bdd::F; }
	region check(region r) {
		if (bdd.full) failed = true;
		return r;
	}

	// The variables of the chooser's streams at step t-k.
	std::vector<bool> step_vars(size_t k, bool inputs) const {
		std::vector<bool> qs(w.vars(), false);
		for (size_t s = 0; s < w.streams.size(); ++s)
			if (w.streams[s].input == inputs)
				for (size_t b = 0; b < w.streams[s].width; ++b)
					qs[w.var(s, k, b)] = true;
		return qs;
	}

	// The code of io_var `v` equals `c`, or the code of io_var `u`.
	region code_eq(tref v, int c, tref u) {
		const size_t s = w.index.at(get_var_name<node>(v));
		const size_t k = (size_t)get_io_var_shift<node>(v);
		region r = data_bdd::T;
		for (size_t b = 0; b < w.streams[s].width; ++b) {
			region bit;
			if (u) {
				const size_t s2 = w.index.at(get_var_name<node>(u));
				const size_t k2 = (size_t)get_io_var_shift<node>(u);
				region x = bdd.var(w.var(s, k, b));
				region y = bdd.var(w.var(s2, k2, b));
				bit = bdd.disj(bdd.conj(x, y),
					bdd.conj(bdd.neg(x), bdd.neg(y)));
			} else bit = bdd.var(w.var(s, k, b), (c >> b) & 1);
			r = bdd.conj(r, bit);
		}
		return r;
	}

	// A subformula over two-element streams, tabulated by substituting 0
	// and 1 and normalizing.
	std::optional<region> two_element(tref cmp) {
		std::vector<std::pair<tref, uint32_t>> vars;
		for (tref v : tau::get(cmp).select_top(is_child<node, tau::io_var>))
			if (std::none_of(vars.begin(), vars.end(),
				[&](auto& p) { return tau::subtree_equals(p.first, v); }))
					vars.emplace_back(v, w.var(
						w.index.at(get_var_name<node>(v)),
						(size_t)get_io_var_shift<node>(v), 0));
		region r = data_bdd::F;
		for (size_t val = 0; val < (size_t{1} << vars.size()); ++val) {
			subtree_map<node, tref> m;
			region cube = data_bdd::T;
			for (size_t j = 0; j < vars.size(); ++j) {
				const bool one = val >> j & 1;
				const size_t tid = find_ba_type<node>(vars[j].first);
				m.emplace(vars[j].first, tau::trim(one
					? build_bf_t_type<node>(tid)
					: build_bf_f_type<node>(tid)));
				cube = bdd.conj(cube, bdd.var(vars[j].second, one));
			}
			auto n = normalize_non_temp<node>(rewriter::replace<node>(cmp, m));
			if (!n.has_value() || !n.value()) return std::nullopt;
			const auto& t = tau::get(n.value());
			if (!t.equals_T() && !t.equals_F()) return std::nullopt;
			if (t.equals_T()) r = bdd.disj(r, cube);
		}
		return r;
	}

	// A label: a Boolean combination of comparisons.
	std::optional<region> eval(tref f) {
		const auto& t = tau::get(f);
		if (t.equals_T()) return data_bdd::T;
		if (t.equals_F()) return data_bdd::F;
		if (!t.has_child()) return std::nullopt;
		const auto nt = t[0].value.nt;
		const auto& op = t[0];
		auto sub = [&](size_t i) { return eval(op.child(i)); };
		if (nt == tau::wff_neg) {
			auto x = sub(0);
			if (!x) return std::nullopt;
			return bdd.neg(*x);
		}
		if (nt == tau::wff_and || nt == tau::wff_or) {
			region acc = nt == tau::wff_and ? data_bdd::T : data_bdd::F;
			for (size_t i = 0; i < op.children_size(); ++i) {
				auto x = sub(i);
				if (!x) return std::nullopt;
				acc = nt == tau::wff_and ? bdd.conj(acc, *x)
					: bdd.disj(acc, *x);
			}
			return acc;
		}
		if (nt == tau::wff_imply || nt == tau::wff_rimply
			|| nt == tau::wff_equiv || nt == tau::wff_xor)
		{
			if (op.children_size() != 2) return std::nullopt;
			auto x = sub(0), y = sub(1);
			if (!x || !y) return std::nullopt;
			if (nt == tau::wff_imply) return bdd.disj(bdd.neg(*x), *y);
			if (nt == tau::wff_rimply) return bdd.disj(*x, bdd.neg(*y));
			region same = bdd.disj(bdd.conj(*x, *y),
				bdd.conj(bdd.neg(*x), bdd.neg(*y)));
			return nt == tau::wff_equiv ? same : bdd.neg(same);
		}
		auto vars = t.select_top(is_child<node, tau::io_var>);
		if (std::all_of(vars.begin(), vars.end(), [&](tref v) {
			return w.streams[w.index.at(get_var_name<node>(v))].two; }))
				return two_element(f);
		bool equal;
		auto sides = code_equality<node>(f, equal);
		if (!sides) return std::nullopt;
		auto& [l, rr] = *sides;
		region r;
		if (l.var && rr.var) r = code_eq(l.var, -1, rr.var);
		else if (l.var) r = code_eq(l.var, rr.constant, nullptr);
		else if (rr.var) r = code_eq(rr.var, l.constant, nullptr);
		else r = l.constant == rr.constant ? data_bdd::T : data_bdd::F;
		return equal ? r : bdd.neg(r);
	}

	// Evaluates the label of every edge; false when one is not a Boolean
	// combination of comparisons the window reads.
	bool init() {
		for (const auto& x : a.v) {
			edge_base.push_back(labels.size());
			for (const auto& e : x.edges) {
				auto r = eval(e.label);
				if (!r || bdd.full) return false;
				labels.push_back(*r);
			}
		}
		return true;
	}

	region pre(int p, int i, const std::vector<region>& Y,
		const std::vector<region>& G)
	{
		const auto& x = a.v[i];
		const bool mine = x.owner == p;
		region body = mine ? data_bdd::F : data_bdd::T;
		for (size_t j = 0; j < x.edges.size(); ++j) {
			const auto& e = x.edges[j];
			region tgt = mine ? Y[e.dst] : bdd.disj(Y[e.dst], bdd.neg(G[e.dst]));
			if (e.shift) {
				bool ok = true;
				tgt = bdd.lower(tgt, (uint32_t)w.per_step, ok);
				if (!ok) { failed = true; return data_bdd::F; }
			}
			const region l = labels[edge_base[i] + j];
			body = mine ? bdd.disj(body, bdd.conj(l, tgt))
				: bdd.conj(body, bdd.disj(bdd.neg(l), tgt));
		}
		if (x.picks != arena::chooser::none)
			body = bdd.quantify(body,
				step_vars(0, x.picks == arena::chooser::inputs), mine);
		return check(body);
	}

	region move(int i, size_t j, const std::vector<region>& Y) {
		const auto& e = a.v[i].edges[j];
		region tgt = Y[e.dst];
		if (e.shift) {
			bool ok = true;
			tgt = bdd.lower(tgt, (uint32_t)w.per_step, ok);
			if (!ok) { failed = true; return data_bdd::F; }
		}
		return check(bdd.conj(labels[edge_base[i] + j], tgt));
	}

	std::optional<bool> reached(region r) {
		for (size_t k = 1; k <= w.depth; ++k) {
			r = bdd.quantify(r, step_vars(k, false), true);
			r = bdd.quantify(r, step_vars(k, true), false);
		}
		if (bdd.full) return std::nullopt;
		return r != data_bdd::F;
	}
};

// Zielonka's algorithm over the regions of `R`. With `record` set, it also
// keeps a winning strategy of the system: for each system vertex and edge,
// the positions (history, inputs and the outputs chosen) from which the
// system takes that edge. The strategy is positional and composed as in
// the proof of Zielonka's algorithm: in an attractor of the system, the
// moves into the part attracted earlier (a rank that decreases); in the
// vertices of the top priority when it is odd and the system wins the whole
// subgame, any move staying in the subgame; elsewhere the strategies of the
// subgames solved recursively. The domains of these pieces are disjoint, so
// their union is one strategy.
template <typename R>
struct data_game_solver {
	using region = typename R::region;
	using moves_t = std::vector<std::vector<region>>;
	R& r;
	size_t n;
	size_t max_rounds;
	bool record;
	std::vector<std::vector<int>> preds;
	std::vector<int> priority, owner;
	std::vector<size_t> edges;

	data_game_solver(R& regions, const auto& arena, size_t rounds,
		bool keep_strategy = false)
		: r(regions), n(arena.v.size()), max_rounds(rounds),
		record(keep_strategy), preds(n)
	{
		for (size_t i = 0; i < n; ++i) {
			priority.push_back(arena.v[i].priority);
			owner.push_back(arena.v[i].owner);
			edges.push_back(arena.v[i].edges.size());
			for (const auto& e : arena.v[i].edges)
				if (std::find(preds[e.dst].begin(), preds[e.dst].end(),
					(int)i) == preds[e.dst].end())
						preds[e.dst].push_back((int)i);
		}
	}

	struct won {
		std::vector<region> env, sys;
		moves_t moves;   // of the system, over its region
	};

	moves_t no_moves() const {
		moves_t m(n);
		if (record) for (size_t i = 0; i < n; ++i)
			if (owner[i] == 1) m[i].assign(edges[i], r.bottom());
		return m;
	}
	void add_moves(moves_t& to, const moves_t& from) {
		for (size_t i = 0; i < from.size(); ++i)
			for (size_t j = 0; j < from[i].size(); ++j)
				to[i][j] = r.disj(to[i][j], from[i][j]);
	}

	bool empty(const std::vector<region>& X) {
		for (const auto& x : X) if (!r.empty(x)) return false;
		return true;
	}

	// The attractor of `Y` for player `p` in the subgame `G`; with `moves`,
	// the system's moves of each position it adds into the part added
	// before it.
	std::vector<region> attractor(int p, std::vector<region> Y,
		const std::vector<region>& G, moves_t* moves = nullptr)
	{
		std::vector<bool> dirty(n, true);
		for (size_t round = 0; !r.failed; ++round) {
			if (max_rounds && round >= max_rounds) {
				r.failed = true;
				break;
			}
			bool changed = false;
			for (size_t i = 0; i < n && !r.failed; ++i) {
				if (!dirty[i]) continue;
				dirty[i] = false;
				if (r.empty(G[i])) continue;
				region grown = r.disj(Y[i],
					r.conj(G[i], r.pre(p, (int)i, Y, G)));
				region added = r.minus(grown, Y[i]);
				if (r.empty(added)) continue;
				if (moves && owner[i] == 1)
					for (size_t j = 0; j < edges[i]; ++j)
						(*moves)[i][j] = r.disj((*moves)[i][j],
							r.conj(added, r.move((int)i, j, Y)));
				Y[i] = std::move(grown);
				changed = true;
				for (int j : preds[i]) dirty[j] = true;
			}
			if (!changed) break;
		}
		return Y;
	}

	std::vector<region> minus(const std::vector<region>& X,
		const std::vector<region>& Y)
	{
		std::vector<region> out;
		for (size_t i = 0; i < n; ++i) out.push_back(r.minus(X[i], Y[i]));
		return out;
	}

	// The regions of the subgame `G` won by the environment and by the
	// system, with the system's strategy when recording.
	won solve(const std::vector<region>& G) {
		const std::vector<region> none(n, r.bottom());
		int top = -1;
		for (size_t i = 0; i < n; ++i)
			if (!r.empty(G[i])) top = std::max(top, priority[i]);
		if (top < 0 || r.failed) return { none, none, no_moves() };
		const int p = top & 1;
		std::vector<region> U(n, r.bottom());
		for (size_t i = 0; i < n; ++i)
			if (priority[i] == top) U[i] = G[i];
		moves_t attracted = no_moves();
		auto A = attractor(p, U, G, record && p ? &attracted : nullptr);
		auto sub = solve(minus(G, A));
		if (r.failed) return { none, none, no_moves() };
		auto& lost = p ? sub.env : sub.sys;
		if (empty(lost)) {
			if (!p) return { G, none, no_moves() };
			won all{ none, G, std::move(sub.moves) };
			if (record) {
				add_moves(all.moves, attracted);
				for (size_t i = 0; i < n; ++i)
					if (owner[i] == 1 && !r.empty(U[i]))
						for (size_t j = 0; j < edges[i]; ++j)
							all.moves[i][j] = r.disj(all.moves[i][j],
								r.conj(U[i], r.move((int)i, j, G)));
			}
			return all;
		}
		moves_t kept = no_moves();
		auto B = attractor(1 - p, lost, G,
			record && !p ? &kept : nullptr);
		auto rest = solve(minus(G, B));
		if (r.failed) return { none, none, no_moves() };
		auto& theirs = p ? rest.env : rest.sys;
		for (size_t i = 0; i < n; ++i) theirs[i] = r.disj(theirs[i], B[i]);
		if (record && !p) {
			add_moves(rest.moves, sub.moves);
			add_moves(rest.moves, kept);
		}
		return rest;
	}

	// The solution of the whole game; nullopt when undecided.
	std::optional<won> solve_all() {
		const std::vector<region> all(n, r.top());
		auto w = solve(all);
		if (r.failed) return std::nullopt;
		return w;
	}
};

// ── A strategy of the data game ──────────────────────────────────────────────
//
// data_game_solver's moves, played one step at a time. The memory is the
// game vertex, an environment vertex when a step starts, and the last
// `depth` values of every stream, which the caller keeps. A step follows the
// edge the inputs take to a system vertex, asks a solver for outputs in one
// of that vertex's moves, and follows the edge those outputs take. The
// values of the steps before step 0 are the strategy's own: every input is 0
// and the outputs are chosen so that the initial vertex is won, which the
// system can do whatever the inputs of those steps are.
template <NodeType node>
struct data_game_strategy {
	using tau = tree<node>;
	using values = subtree_map<node, tref>;
	// Solves a formula over the outputs of absolute step `t`; nullopt when
	// it has no solution.
	using solver_fn = std::function<std::optional<values>(tref, int_t)>;
	// The value of a stream (name, type, input) at an absolute step >= 0,
	// or nullptr when there is none.
	using value_fn = std::function<tref(const std::string&, size_t, bool,
		int_t)>;

	struct stream { std::string name; size_t tid = 0; bool input = false; };
	struct vertex { int picks = -1; std::vector<int> dst; };

	std::vector<stream> streams;
	std::map<std::string, size_t> index;
	size_t depth = 0;
	// picks 0: the environment chooses the inputs, 1: the system the
	// outputs, -1: nobody (a coloured vertex or a sink)
	std::vector<vertex> v;
	int init = 0;

	virtual ~data_game_strategy() = default;

	void reset() { at = init; ready = false; }

	// The outputs of absolute step `t`, keyed like the solver keys them.
	result<values> step(const value_fn& get, int_t t, const solver_fn& solve) {
		result<values> r;
		if (!ready) {
			TAU_TRY(bool chosen, choose_before(solve));
			if (!chosen) return r.with_error(code::internal_error,
				"the data game strategy found no values before step 0");
			ready = true;
		}
		window w(streams.size(), std::vector<tref>(depth + 1, nullptr));
		for (size_t s = 0; s < streams.size(); ++s)
			for (size_t k = 0; k <= depth; ++k) {
				const int_t time = t - (int_t)k;
				if (k == 0 && !streams[s].input) continue;
				tref x = time < 0 ? before[s][(size_t)(-time - 1)]->get()
					: get(streams[s].name, streams[s].tid,
						streams[s].input, time);
				if (!x) return r.with_error(code::internal_error,
					"the data game strategy reads the unknown value of "
					+ streams[s].name + " at step "
					+ std::to_string(time));
				w[s][k] = x;
			}
		if (v[at].picks == 0) {
			int next = -1;
			for (size_t j = 0; j < v[at].dst.size() && next < 0; ++j) {
				auto holds = holds_label(at, j, w);
				if (!holds) return r.with_error(code::solver_error,
					"the data game strategy cannot read an edge label");
				if (*holds) next = follow(v[at].dst[j]);
			}
			if (next < 0) return r.with_error(code::internal_error,
				"the data game strategy has no edge for the inputs");
			at = next;
		}
		values out;
		// a sink: the play is decided and no output matters
		if (v[at].picks != 1) return r.with_value(std::move(out));
		TAU_TRY(tref c, constraint(at, w, t));
		auto sol = solve(c, t);
		if (!sol) return r.with_error(code::internal_error,
			"the data game strategy has no outputs in its move");
		for (size_t s = 0; s < streams.size(); ++s) {
			if (streams[s].input) continue;
			tref key = build_out_var_at_n<node>(streams[s].name, t,
				streams[s].tid);
			auto it = sol->find(key);
			tref x = it != sol->end() ? it->second
				: build_bf_f_type<node>(streams[s].tid);
			out.emplace(key, x);
			w[s][0] = x;
		}
		for (const auto& [key, x] : *sol) out.emplace(key, x);
		for (size_t j = 0; j < v[at].dst.size(); ++j) {
			auto holds = holds_move(at, j, w);
			if (!holds) return r.with_error(code::solver_error,
				"the data game strategy cannot read a move");
			if (*holds) {
				at = follow(v[at].dst[j]);
				return r.with_value(std::move(out));
			}
		}
		return r.with_error(code::internal_error,
			"the outputs take no move of the data game strategy");
	}

protected:
	// w[s][k]: the value of stream s at step t-k, nullptr for an output of
	// step t before it is chosen
	using window = std::vector<std::vector<tref>>;
	int at = 0;
	bool ready = false;
	// before[s][k-1]: the value of stream s at step -k
	std::vector<std::vector<htref>> before;

	// whether the label of edge `j` of environment vertex `i` holds
	virtual std::optional<bool> holds_label(int i, size_t j,
		const window& w) = 0;
	// whether move `j` of system vertex `i` holds
	virtual std::optional<bool> holds_move(int i, size_t j,
		const window& w) = 0;
	// a formula over the outputs of step `t` whose solutions are the
	// moves of system vertex `i`
	virtual result<tref> constraint(int i, const window& w, int_t t) = 0;
	// fills `before`
	virtual result<bool> choose_before(const solver_fn& solve) = 0;

	int follow(int x) const {
		while (v[x].picks < 0 && v[x].dst.size() == 1 && v[x].dst[0] != x)
			x = v[x].dst[0];
		return x;
	}

	// Fresh names for the outputs before step 0 while they are solved.
	static tref before_var(size_t s, size_t k, size_t tid) {
		return build_out_var_at_n<node>("o__dg_before" + std::to_string(s)
			+ "_" + std::to_string(k), 0, tid);
	}
	// The value `sol` gives `var`, 0 when it gives none.
	static tref value_of(const values& sol, tref var, size_t tid) {
		auto it = sol.find(var);
		return it != sol.end() ? it->second : build_bf_f_type<node>(tid);
	}
};

// The strategy of a game played on code_regions.
template <NodeType node>
struct code_strategy : data_game_strategy<node> {
	using base = data_game_strategy<node>;
	using tau = tree<node>;
	using typename base::window;
	using typename base::values;
	using typename base::solver_fn;
	using base::streams;

	code_window w;
	data_bdd bdd;
	std::vector<std::vector<data_bdd::id>> labels, moves;
	data_bdd::id won_init = data_bdd::F;

	code_strategy(code_window win, data_bdd b)
		: w(std::move(win)), bdd(std::move(b)) {}

protected:
	using base::before;

	// Whether two values are equal; nullopt when undecided.
	std::optional<bool> same(tref x, tref y) {
		if (tau::subtree_equals(x, y)) return true;
		auto n = normalize_non_temp<node>(tau::build_bf_eq(x, y));
		if (!n.has_value() || !n.value()) return std::nullopt;
		const auto& t = tau::get(n.value());
		if (t.equals_T()) return true;
		if (t.equals_F()) return false;
		return std::nullopt;
	}

	// The bits of the known values of `win`, -1 for an unknown one. A
	// value of a coded stream gets code 0 when it is 0, 1 when it is 1,
	// and otherwise the code of the first equal value met or a new one.
	std::optional<std::vector<int>> encode(const window& win) {
		std::vector<int> bits(w.vars(), -1);
		std::map<size_t, std::vector<std::pair<tref, size_t>>> seen;
		for (size_t k = 0; k < win[0].size(); ++k)
			for (size_t s = 0; s < streams.size(); ++s) {
				tref x = win[s][k];
				if (!x) continue;
				const size_t tid = streams[s].tid;
				size_t c;
				auto one = same(x, build_bf_t_type<node>(tid));
				if (!one) return std::nullopt;
				if (w.streams[s].two) c = *one ? 1 : 0;
				else if (*one) c = 1;
				else {
					auto zero = same(x, build_bf_f_type<node>(tid));
					if (!zero) return std::nullopt;
					if (*zero) c = 0;
					else {
						auto& reps = seen[tid];
						c = reps.size() + 2;
						for (auto& [y, cy] : reps) {
							auto eq = same(x, y);
							if (!eq) return std::nullopt;
							if (*eq) { c = cy; break; }
						}
						if (c == reps.size() + 2) reps.emplace_back(x, c);
					}
				}
				for (size_t b = 0; b < w.streams[s].width; ++b)
					bits[w.var(s, k, b)] = (int)(c >> b & 1);
			}
		return bits;
	}

	// nullopt when the BDD reads an unknown bit
	std::optional<bool> eval(data_bdd::id n, const std::vector<int>& bits) {
		while (n > data_bdd::T) {
			const auto& x = bdd.nodes[n];
			if (bits[x.var] < 0) return std::nullopt;
			n = bits[x.var] ? x.hi : x.lo;
		}
		return n == data_bdd::T;
	}

	// Completes the unknown bits of `bits` along a path of `n` to true.
	bool pick(data_bdd::id n, std::vector<int>& bits) {
		std::set<data_bdd::id> dead;
		std::function<bool(data_bdd::id)> go = [&](data_bdd::id m) {
			if (m <= data_bdd::T) return m == data_bdd::T;
			if (dead.contains(m)) return false;
			const auto x = bdd.nodes[m];
			if (bits[x.var] >= 0) return go(bits[x.var] ? x.hi : x.lo);
			for (int b : { 0, 1 }) {
				bits[x.var] = b;
				if (go(b ? x.hi : x.lo)) return true;
			}
			bits[x.var] = -1;
			dead.insert(m);
			return false;
		};
		if (!go(n)) return false;
		for (auto& b : bits) if (b < 0) b = 0;
		return true;
	}

	size_t code_of(const std::vector<int>& bits, size_t s, size_t k) const {
		size_t c = 0;
		for (size_t b = 0; b < w.streams[s].width; ++b)
			if (bits[w.var(s, k, b)] > 0) c |= size_t{1} << b;
		return c;
	}

	// The formula giving each of `slots` (stream, step back, variable) the
	// value its code in `bits` stands for, next to the known values of
	// `win`.
	tref decode(const std::vector<int>& bits, const window& win,
		const std::vector<std::tuple<size_t, size_t, tref>>& slots)
	{
		tref f = tau::_T();
		for (size_t i = 0; i < slots.size(); ++i) {
			auto [s, k, x] = slots[i];
			const size_t tid = streams[s].tid;
			const size_t c = code_of(bits, s, k);
			if (w.streams[s].two || c < 2) {
				f = tau::build_wff_and(f, tau::build_bf_eq(x, c
					? build_bf_t_type<node>(tid)
					: build_bf_f_type<node>(tid)));
				continue;
			}
			tref known = nullptr;
			for (size_t s2 = 0; s2 < streams.size() && !known; ++s2)
				if (streams[s2].tid == tid && !w.streams[s2].two)
					for (size_t k2 = 0; k2 < win[s2].size(); ++k2)
						if (win[s2][k2] && code_of(bits, s2, k2) == c)
							{ known = win[s2][k2]; break; }
			if (known) {
				f = tau::build_wff_and(f, tau::build_bf_eq(x, known));
				continue;
			}
			// a value none of the window holds
			f = tau::build_wff_and(f, tau::build_wff_and(
				tau::build_bf_neq(x, build_bf_f_type<node>(tid)),
				tau::build_bf_neq(x, build_bf_t_type<node>(tid))));
			for (size_t s2 = 0; s2 < streams.size(); ++s2)
				if (streams[s2].tid == tid)
					for (tref y : win[s2]) if (y)
						f = tau::build_wff_and(f, tau::build_bf_neq(x, y));
			for (size_t i2 = 0; i2 < i; ++i2) {
				auto [s2, k2, y] = slots[i2];
				if (streams[s2].tid != tid || w.streams[s2].two) continue;
				f = tau::build_wff_and(f, code_of(bits, s2, k2) == c
					? tau::build_bf_eq(x, y) : tau::build_bf_neq(x, y));
			}
		}
		return f;
	}

	std::optional<bool> holds_label(int i, size_t j, const window& win)
		override
	{
		auto bits = encode(win);
		if (!bits) return std::nullopt;
		return eval(labels[i][j], *bits);
	}

	std::optional<bool> holds_move(int i, size_t j, const window& win)
		override
	{
		auto bits = encode(win);
		if (!bits) return std::nullopt;
		return eval(moves[i][j], *bits);
	}

	result<tref> constraint(int i, const window& win, int_t t) override {
		result<tref> r;
		auto bits = encode(win);
		if (!bits) return r.with_error(code::solver_error,
			"the data game strategy cannot compare the values");
		data_bdd::id any = data_bdd::F;
		for (auto m : moves[i]) any = bdd.disj(any, m);
		if (bdd.full || !pick(any, *bits))
			return r.with_error(code::internal_error,
				"the data game strategy has no move from the history");
		std::vector<std::tuple<size_t, size_t, tref>> slots;
		for (size_t s = 0; s < streams.size(); ++s)
			if (!streams[s].input)
				slots.emplace_back(s, 0, build_out_var_at_n<node>(
					streams[s].name, t, streams[s].tid));
		return r.with_value(decode(*bits, win, slots));
	}

	result<bool> choose_before(const solver_fn& solve) override {
		result<bool> r;
		const size_t n = streams.size(), d = w.depth;
		std::vector<int> bits(w.vars(), -1);
		window win(n, std::vector<tref>(d + 1, nullptr));
		for (size_t s = 0; s < n; ++s)
			for (size_t k = 1; k <= d; ++k)
				if (streams[s].input) {
					for (size_t b = 0; b < w.streams[s].width; ++b)
						bits[w.var(s, k, b)] = 0;
					win[s][k] = build_bf_f_type<node>(streams[s].tid);
				}
		if (!pick(won_init, bits)) return r.with_value(false);
		std::vector<std::tuple<size_t, size_t, tref>> slots;
		for (size_t s = 0; s < n; ++s)
			for (size_t k = 1; k <= d; ++k)
				if (!streams[s].input)
					slots.emplace_back(s, k,
						this->before_var(s, k, streams[s].tid));
		values sol;
		if (!slots.empty()) {
			auto got = solve(decode(bits, win, slots), 0);
			if (!got) return r.with_value(false);
			sol = std::move(*got);
		}
		before.assign(n, {});
		for (size_t s = 0; s < n; ++s)
			for (size_t k = 1; k <= d; ++k)
				before[s].push_back(tau::geth(streams[s].input
					? win[s][k] : this->value_of(sol, this->before_var(
						s, k, streams[s].tid), streams[s].tid)));
		return r.with_value(true);
	}
};

// The strategy of a game played on formula_regions.
template <NodeType node>
struct formula_strategy : data_game_strategy<node> {
	using base = data_game_strategy<node>;
	using tau = tree<node>;
	using typename base::window;
	using typename base::values;
	using typename base::solver_fn;
	using base::streams;
	using base::index;

	std::vector<std::vector<htref>> labels, moves;
	htref won_init;

protected:
	using base::before;

	// `f` with each io_var replaced by its value in `win`; an output of
	// step t with no value becomes the output variable of absolute step
	// `t`.
	tref at_step(tref f, const window& win, int_t t) {
		subtree_map<node, tref> m;
		for (tref x : tau::get(f).select_top(is_child<node, tau::io_var>)) {
			const size_t s = index.at(get_var_name<node>(x));
			const size_t k = (size_t)get_io_var_shift<node>(x);
			tref val = win[s][k];
			m.emplace(x, tau::trim(val ? val : build_out_var_at_n<node>(
				streams[s].name, t, streams[s].tid)));
		}
		return rewriter::replace<node>(f, m);
	}

	std::optional<bool> truth(tref f, const window& win) {
		auto n = normalize_non_temp<node>(at_step(f, win, 0));
		if (!n.has_value() || !n.value()) return std::nullopt;
		const auto& t = tau::get(n.value());
		if (t.equals_T()) return true;
		if (t.equals_F()) return false;
		return std::nullopt;
	}

	std::optional<bool> holds_label(int i, size_t j, const window& win)
		override
	{
		return truth(labels[i][j]->get(), win);
	}

	std::optional<bool> holds_move(int i, size_t j, const window& win)
		override
	{
		return truth(moves[i][j]->get(), win);
	}

	result<tref> constraint(int i, const window& win, int_t t) override {
		result<tref> r;
		tref any = tau::_F();
		for (const auto& m : moves[i])
			any = tau::build_wff_or(any, m->get());
		auto n = normalize_non_temp<node>(at_step(any, win, t));
		if (!n.has_value() || !n.value()) return r.with_error(
			code::solver_error, "the data game strategy cannot read "
			"its move");
		return r.with_value(n.value());
	}

	result<bool> choose_before(const solver_fn& solve) override {
		result<bool> r;
		const size_t n = streams.size();
		subtree_map<node, tref> m;
		for (tref x : tau::get(won_init->get()).select_top(
			is_child<node, tau::io_var>))
		{
			const size_t s = index.at(get_var_name<node>(x));
			const size_t k = (size_t)get_io_var_shift<node>(x);
			const size_t tid = streams[s].tid;
			m.emplace(x, tau::trim(streams[s].input
				? build_bf_f_type<node>(tid)
				: this->before_var(s, k, tid)));
		}
		auto sol = solve(rewriter::replace<node>(won_init->get(), m), 0);
		if (!sol) return r.with_value(false);
		before.assign(n, {});
		for (size_t s = 0; s < n; ++s)
			for (size_t k = 1; k <= this->depth; ++k) {
				const size_t tid = streams[s].tid;
				before[s].push_back(tau::geth(streams[s].input
					? build_bf_f_type<node>(tid)
					: this->value_of(*sol, this->before_var(s, k, tid),
						tid)));
			}
		return r.with_value(true);
	}
};

// The streams, depth and vertices of a strategy of `arena` over `atoms`.
template <NodeType node>
static void describe_strategy(data_game_strategy<node>& st,
	const data_arena<node>& arena,
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	using tau = tree<node>;
	using chooser = typename data_arena<node>::chooser;
	for (auto& [atom, _] : atoms)
		for (tref x : tau::get(atom).select_top(is_child<node, tau::io_var>)) {
			st.depth = std::max(st.depth, (size_t)get_io_var_shift<node>(x));
			auto [it, fresh] = st.index.emplace(get_var_name<node>(x),
				st.streams.size());
			if (fresh) st.streams.push_back({ it->first,
				find_ba_type<node>(x), is_input_stream<node>(x) });
		}
	for (const auto& x : arena.v) {
		typename data_game_strategy<node>::vertex y;
		y.picks = x.picks == chooser::inputs ? 0
			: x.picks == chooser::outputs ? 1 : -1;
		for (const auto& e : x.edges) y.dst.push_back(e.dst);
		st.v.push_back(std::move(y));
	}
	st.init = arena.init;
	st.reset();
}

// Decides the realizability of `skeleton` over `atoms` on the data; over
// formula regions only when `formulas` is set. With `strategy`, a won game
// also gives the system's strategy there.
template <NodeType node>
static result<data_game_verdict> solve_data_game(const std::string& skeleton,
	const std::vector<std::pair<tref, std::string>>& atoms,
	const std::vector<std::string>& input_props,
	const std::vector<std::string>& output_props, bool formulas,
	std::shared_ptr<data_game_strategy<node>>* strategy = nullptr)
{
	using tau = tree<node>;
	result<data_game_verdict> r;
	// every stream is an input or an output, and read at a relative step
	for (auto& [atom, _] : atoms)
		for (tref var : tau::get(atom).select_top(is_child<node, tau::io_var>))
			if (is_io_initial<node>(var)
				|| io_var_direction<node>(tau::trim(var)) == 0)
					return r.with_value(data_game_verdict::undecided);
	const auto window = make_code_window<node>(atoms, 256);
	if (!window && !formulas)
		return r.with_value(data_game_verdict::undecided);
	// ACD usually gives the smallest game and a parity condition, but may
	// name it Rabin or Streett, which the solver does not read; the
	// determinized game then comes with a parity condition.
	data_arena<node> arena;
	bool built = false;
	for (const char* algo : { "acd", "sd" }) {
		TAU_TRY(auto game, alg_d::call_ltlsynt_game(skeleton, input_props,
			output_props, algo));
		if ((built = build_data_arena<node>(arena, game, atoms))) break;
		arena = {};
	}
	if (!built) return r.with_value(data_game_verdict::undecided);
	const bool keep = strategy != nullptr;
	std::optional<bool> wins;
	bool on_codes = false;
	if (window) {
		code_regions<node> codes(arena, *window, size_t{1} << 21);
		if ((on_codes = codes.init())) {
			// a finite lattice: every fixpoint ends without a cap
			data_game_solver solver(codes, arena, 0, keep);
			auto w = solver.solve_all();
			if (w) wins = codes.reached(w->sys[arena.init]);
			if (keep && wins && *wins && !codes.failed) {
				auto st = std::make_shared<code_strategy<node>>(
					*window, std::move(codes.bdd));
				describe_strategy<node>(*st, arena, atoms);
				for (size_t i = 0; i < arena.v.size(); ++i) {
					st->labels.emplace_back(
						codes.labels.begin() + codes.edge_base[i],
						codes.labels.begin() + codes.edge_base[i]
							+ arena.v[i].edges.size());
					st->moves.push_back(w->moves[i]);
				}
				st->won_init = w->sys[arena.init];
				*strategy = st;
			}
		}
	}
	if (!on_codes && formulas) {
		formula_regions<node> regions(arena);
		data_game_solver solver(regions, arena,
			ltl_max_refinement_rounds(), keep);
		auto w = solver.solve_all();
		if (w) wins = regions.reached(w->sys[arena.init]);
		if (keep && wins && *wins) {
			auto st = std::make_shared<formula_strategy<node>>();
			describe_strategy<node>(*st, arena, atoms);
			for (size_t i = 0; i < arena.v.size(); ++i) {
				st->labels.emplace_back();
				for (const auto& e : arena.v[i].edges)
					st->labels.back().push_back(tau::geth(e.label));
				st->moves.emplace_back();
				for (tref m : w->moves[i])
					st->moves.back().push_back(tau::geth(m));
			}
			st->won_init = tau::geth(w->sys[arena.init]);
			*strategy = st;
		}
	}
	LOG_DEBUG << "[ltl_aba] data game: " << arena.v.size() << " vertices, "
		<< (wins ? (*wins ? "system wins" : "environment wins")
			: "undecided");
	if (!wins) return r.with_value(data_game_verdict::undecided);
	return r.with_value(*wins ? data_game_verdict::realizable
		: data_game_verdict::unrealizable);
}

} // namespace idni::tau_lang
