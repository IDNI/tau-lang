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
// When every stream has a two-element type and the window is small, a
// region is a bit set over the values of the window (bit_regions);
// otherwise it is a formula whose quantifiers the normalizer eliminates
// (formula_regions), and one it leaves standing makes the game undecided.
// The formulas over a fixed set of variables and constants are finitely
// many up to equivalence, so each fixpoint ends; the rounds of a formula
// fixpoint are still capped by ltl_max_refinement_rounds().

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

// The window of bit_regions: an index per stream, and whether it is an
// input. Bit (k * streams + s) of an index is the value of stream s at step
// t-k, k = 0 the current step.
struct bit_window {
	std::map<std::string, size_t> index;
	std::vector<bool> is_input;
	size_t streams = 0, bits = 0;
};

// The window of `atoms` when every stream has a two-element type and the
// window has at most `max_bits` bits.
template <NodeType node>
static std::optional<bit_window> make_bit_window(
	const std::vector<std::pair<tref, std::string>>& atoms, size_t max_bits)
{
	using tau = tree<node>;
	data_quantifier<node> dq;
	bit_window w;
	int_t depth = 0;
	for (auto& [atom, _] : atoms)
		for (tref v : tau::get(atom).select_top(is_child<node, tau::io_var>)) {
			if (is_io_initial<node>(v) || !dq.is_two_element(v))
				return std::nullopt;
			depth = std::max(depth, get_io_var_shift<node>(v));
			if (w.index.emplace(get_var_name<node>(v), w.index.size()).second)
				w.is_input.push_back(is_input_var<node>(v));
		}
	w.streams = w.index.size();
	w.bits = w.streams * (size_t)(depth + 1);
	if (w.bits > max_bits) return std::nullopt;
	return w;
}

// Regions as bit sets over every value of a bit_window.
template <NodeType node>
struct bit_regions {
	using tau = tree<node>;
	using region = std::vector<uint64_t>;
	using arena = data_arena<node>;
	const arena& a;
	bool failed = false;
	const std::map<std::string, size_t>& index;
	const std::vector<bool>& is_input;
	size_t streams, bits, size;
	std::vector<region> labels;           // per vertex, per edge
	std::vector<size_t> edge_base;

	bit_regions(const arena& ar, const bit_window& w) : a(ar),
		index(w.index), is_input(w.is_input), streams(w.streams),
		bits(w.bits), size(size_t{1} << w.bits) {}

	region top() {
		region r(words(), ~uint64_t{0});
		trim(r);
		return r;
	}
	region bottom() { return region(words(), 0); }
	size_t words() const { return (size + 63) / 64; }
	void trim(region& r) const {
		if (size % 64) r.back() &= (uint64_t{1} << (size % 64)) - 1;
	}
	static bool get(const region& r, size_t x) { return r[x >> 6] >> (x & 63) & 1; }
	static void set(region& r, size_t x) { r[x >> 6] |= uint64_t{1} << (x & 63); }
	region conj(const region& x, const region& y) {
		region r(x);
		for (size_t i = 0; i < r.size(); ++i) r[i] &= y[i];
		return r;
	}
	region disj(const region& x, const region& y) {
		region r(x);
		for (size_t i = 0; i < r.size(); ++i) r[i] |= y[i];
		return r;
	}
	region minus(const region& x, const region& y) {
		region r(x);
		for (size_t i = 0; i < r.size(); ++i) r[i] &= ~y[i];
		return r;
	}

	bool empty(const region& r) {
		for (uint64_t w : r) if (w) return false;
		return true;
	}

	// Evaluates the label of every edge; false when a comparison does not
	// evaluate to a truth value.
	bool init() {
		std::map<tref, region> truth;
		auto table = [&](tref atom) -> const region* {
			if (auto it = truth.find(atom); it != truth.end())
				return &it->second;
			std::vector<std::pair<tref, size_t>> vars;
			for (tref v : tau::get(atom).select_top(
				is_child<node, tau::io_var>))
			{
				size_t bit = (size_t)get_io_var_shift<node>(v) * streams
					+ index.at(get_var_name<node>(v));
				if (std::none_of(vars.begin(), vars.end(),
					[&](auto& p) { return tau::subtree_equals(p.first, v); }))
						vars.emplace_back(v, bit);
			}
			std::vector<bool> holds(size_t{1} << vars.size());
			for (size_t val = 0; val < holds.size(); ++val) {
				subtree_map<node, tref> m;
				for (size_t j = 0; j < vars.size(); ++j) {
					size_t tid = find_ba_type<node>(vars[j].first);
					m.emplace(vars[j].first, tau::trim(val >> j & 1
						? build_bf_t_type<node>(tid)
						: build_bf_f_type<node>(tid)));
				}
				auto n = normalize_non_temp<node>(
					rewriter::replace<node>(atom, m));
				if (!n.has_value() || !n.value()) return nullptr;
				const auto& t = tau::get(n.value());
				if (!t.equals_T() && !t.equals_F()) return nullptr;
				holds[val] = t.equals_T();
			}
			region r = bottom();
			for (size_t x = 0; x < size; ++x) {
				size_t val = 0;
				for (size_t j = 0; j < vars.size(); ++j)
					if (x >> vars[j].second & 1) val |= size_t{1} << j;
				if (holds[val]) set(r, x);
			}
			return &truth.emplace(atom, std::move(r)).first->second;
		};
		// A label is a Boolean combination of comparisons, each
		// evaluated on its own values.
		std::function<std::optional<region>(tref)> eval =
			[&](tref f) -> std::optional<region> {
			const auto& t = tau::get(f);
			if (t.equals_T()) return top();
			if (t.equals_F()) return bottom();
			if (t.has_child()) {
				auto nt = t[0].value.nt;
				if (nt == tau::wff_neg) {
					auto x = eval(t[0].first());
					if (!x) return std::nullopt;
					return minus(top(), *x);
				}
				if (nt == tau::wff_and || nt == tau::wff_or) {
					std::optional<region> acc;
					const auto& op = t[0];
					for (size_t c = 0; c < op.children_size(); ++c) {
						auto x = eval(op.child(c));
						if (!x) return std::nullopt;
						acc = !acc ? *x : nt == tau::wff_and
							? conj(*acc, *x) : disj(*acc, *x);
					}
					if (acc) return acc;
				}
			}
			const region* r = table(f);
			if (!r) return std::nullopt;
			return *r;
		};
		for (const auto& x : a.v) {
			edge_base.push_back(labels.size());
			for (const auto& e : x.edges) {
				auto r = eval(e.label);
				if (!r) return false;
				labels.push_back(std::move(*r));
			}
		}
		return true;
	}

	// The index of the next step's window: every value one step older,
	// the oldest dropped, the current step left open (zero).
	size_t shifted(size_t x) const {
		return (x << streams) & (size - 1);
	}

	region pre(int p, int i, const std::vector<region>& Y,
		const std::vector<region>& G)
	{
		const auto& xv = a.v[i];
		const bool mine = xv.owner == p;
		region body = bottom();
		for (size_t x = 0; x < size; ++x) {
			bool acc = !mine;
			for (size_t j = 0; j < xv.edges.size(); ++j) {
				const auto& e = xv.edges[j];
				if (!get(labels[edge_base[i] + j], x)) continue;
				size_t y = e.shift ? shifted(x) : x;
				bool in = get(Y[e.dst], y);
				if (mine) { if (in) { acc = true; break; } }
				else if (!in && get(G[e.dst], y)) { acc = false; break; }
			}
			if (acc) set(body, x);
		}
		if (xv.picks == arena::chooser::none) return body;
		return quantify(body, 0,
			xv.picks == arena::chooser::inputs, mine);
	}

	// Quantifies the values at step t-k of the inputs (or the outputs).
	region quantify(const region& r, size_t k, bool inputs, bool exists) {
		size_t qmask = 0;
		for (size_t s = 0; s < streams; ++s)
			if (is_input[s] == inputs) qmask |= size_t{1} << (k * streams + s);
		if (!qmask) return r;
		region out = bottom();
		for (size_t x = 0; x < size; ++x) {
			if (x & qmask) continue;
			bool acc = !exists;
			for (size_t sub = qmask; ; sub = (sub - 1) & qmask) {
				bool b = get(r, x | sub);
				if (exists ? b : !b) { acc = exists; break; }
				if (!sub) break;
			}
			if (!acc) continue;
			for (size_t sub = qmask; ; sub = (sub - 1) & qmask) {
				set(out, x | sub);
				if (!sub) break;
			}
		}
		return out;
	}

	std::optional<bool> reached(const region& r) {
		region q = r;
		for (size_t k = 1; k * streams < bits; ++k) {
			q = quantify(q, k, false, true);
			q = quantify(q, k, true, false);
		}
		return !empty(q);
	}
};

// Zielonka's algorithm over the regions of `R`.
template <typename R>
struct data_game_solver {
	using region = typename R::region;
	R& r;
	size_t n;
	size_t max_rounds;
	std::vector<std::vector<int>> preds;
	std::vector<int> priority;

	data_game_solver(R& regions, const auto& arena, size_t rounds)
		: r(regions), n(arena.v.size()), max_rounds(rounds), preds(n)
	{
		for (size_t i = 0; i < n; ++i) {
			priority.push_back(arena.v[i].priority);
			for (const auto& e : arena.v[i].edges)
				if (std::find(preds[e.dst].begin(), preds[e.dst].end(),
					(int)i) == preds[e.dst].end())
						preds[e.dst].push_back((int)i);
		}
	}

	bool empty(const std::vector<region>& X) {
		for (const auto& x : X) if (!r.empty(x)) return false;
		return true;
	}

	std::vector<region> attractor(int p, std::vector<region> Y,
		const std::vector<region>& G)
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
				if (r.empty(r.minus(grown, Y[i]))) continue;
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

	// The regions of the subgame `G` won by the environment (first) and
	// by the system (second).
	std::pair<std::vector<region>, std::vector<region>> solve(
		const std::vector<region>& G)
	{
		const std::vector<region> none(n, r.bottom());
		int top = -1;
		for (size_t i = 0; i < n; ++i)
			if (!r.empty(G[i])) top = std::max(top, priority[i]);
		if (top < 0 || r.failed) return { none, none };
		const int p = top & 1;
		std::vector<region> U(n, r.bottom());
		for (size_t i = 0; i < n; ++i)
			if (priority[i] == top) U[i] = G[i];
		auto sub = solve(minus(G, attractor(p, U, G)));
		if (r.failed) return { none, none };
		auto& lost = p ? sub.first : sub.second;
		if (empty(lost))
			return p ? std::pair{ none, G } : std::pair{ G, none };
		auto B = attractor(1 - p, lost, G);
		auto rest = solve(minus(G, B));
		if (r.failed) return { none, none };
		auto& theirs = p ? rest.first : rest.second;
		for (size_t i = 0; i < n; ++i) theirs[i] = r.disj(theirs[i], B[i]);
		return rest;
	}

	// nullopt when undecided
	std::optional<bool> system_wins(int init) {
		const std::vector<region> all(n, r.top());
		auto won = solve(all);
		if (r.failed) return std::nullopt;
		return r.reached(won.second[init]);
	}
};

// Decides the realizability of `skeleton` over `atoms` on the data; over
// formula regions only when `formulas` is set.
template <NodeType node>
static result<data_game_verdict> solve_data_game(const std::string& skeleton,
	const std::vector<std::pair<tref, std::string>>& atoms,
	const std::vector<std::string>& input_props,
	const std::vector<std::string>& output_props, bool formulas)
{
	using tau = tree<node>;
	result<data_game_verdict> r;
	for (auto& [atom, _] : atoms)
		for (tref var : tau::get(atom).select_top(is_child<node, tau::io_var>))
			if (is_io_initial<node>(var))
				return r.with_value(data_game_verdict::undecided);
	const auto window = make_bit_window<node>(atoms, 22);
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
	std::optional<bool> wins;
	bool on_bits = false;
	if (window) {
		bit_regions<node> bits(arena, *window);
		if ((on_bits = bits.init())) {
			// a finite lattice: every fixpoint ends without a cap
			data_game_solver solver(bits, arena, 0);
			wins = solver.system_wins(arena.init);
		}
	}
	if (!on_bits && formulas) {
		formula_regions<node> regions(arena);
		data_game_solver solver(regions, arena,
			ltl_max_refinement_rounds());
		wins = solver.system_wins(arena.init);
	}
	LOG_DEBUG << "[ltl_aba] data game: " << arena.v.size() << " vertices, "
		<< (wins ? (*wins ? "system wins" : "environment wins")
			: "undecided");
	if (!wins) return r.with_value(data_game_verdict::undecided);
	return r.with_value(*wins ? data_game_verdict::realizable
		: data_game_verdict::unrealizable);
}

} // namespace idni::tau_lang
