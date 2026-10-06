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
// When every stream takes a code (a two-element type, a type read only
// through equalities or complements, one with few elements, a modular
// type or a dense order, see code_window), a region is a BDD over the codes
// of the window's values (code_regions); otherwise it is a formula whose
// quantifiers
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
	struct edge { tref label; size_t dst; bool shift; };
	struct vertex {
		int owner = 0;        // 0 environment, 1 system
		int priority = 0;     // max parity, odd favours the system
		chooser picks = chooser::none;
		std::vector<edge> edges;
	};
	std::vector<vertex> v;
	size_t init = 0;
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
	if (game.player.size() != game.num_states || game.init < 0
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
	const size_t n = game.num_states;
	const size_t sys_sink = n, env_sink = n + 1;
	// one vertex per coloured target: it reads its target's region
	std::map<std::pair<int, int>, size_t> colored;
	a.v.resize(n + 2);
	a.init = game.init;
	a.v[sys_sink] = { 0, 1, chooser::none, { { tau::_T(), sys_sink, false } } };
	a.v[env_sink] = { 0, 0, chooser::none, { { tau::_T(), env_sink, false } } };
	for (size_t q = 0; q < n; ++q) {
		const int owner = game.player[q] == 1 ? 1 : 0;
		a.v[q].owner = owner;
		a.v[q].priority = prio(game.state_color[q], game.state_priority[q]);
		a.v[q].picks = owner ? chooser::outputs : chooser::inputs;
		const bool shift = owner == 1;
		tref covered = tau::_F();
		for (size_t j = 0; j < game.trans[q].size(); ++j) {
			const auto& [guard, next, color] = game.trans[q][j];
			if (next >= n) return false;
			auto cubes = alg_d::hoa_guard::to_dnf(guard);
			if (!cubes) return false;
			tref label = tau::_F();
			for (const auto& c : *cubes) {
				tref conj = tau::_T();
				for (const auto& l : c) {
					// a label reads its owner's props only
					if (l.ap >= atom_of.size()
						|| l.ap >= game.controllable.size())
							return false;
					if (game.controllable[l.ap] != (owner == 1))
						return false;
					if (!atom_of[l.ap]) continue;
					conj = tau::build_wff_and(conj, l.pos ? atom_of[l.ap]
						: tau::build_wff_neg(atom_of[l.ap]));
				}
				label = tau::build_wff_or(label, conj);
			}
			label = shift_io_vars<node>(label, 0);
			covered = tau::build_wff_or(covered, label);
			size_t dst = next;
			const int ep = game.edge_priority[q][j];
			if (color >= 0 && ep >= 0) {
				auto [it, fresh] = colored.emplace(
					std::pair{ next, ep }, a.v.size());
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
//
// With `closed_type`, a type whose owner decides closed formulas over it
// whatever their quantifier prefix (pack_decide_closed), and every stream of
// that type, a region keeps its quantifiers and is never normalized: its
// emptiness is the truth of its existential closure, and the steps before
// step 0 are decided the same way. Every question such regions ask is one
// such decision, whose missing answer fails the regions.
template <NodeType node>
struct formula_regions {
	using tau = tree<node>;
	using region = tref;
	using arena = data_arena<node>;
	const arena& a;
	bool failed = false;
	data_quantifier<node> dq;
	// Reports of the checks that answer through `failed`; merged by the caller.
	report rep;
	std::map<tref, tref> normal;
	std::optional<size_t> closed_type;

	explicit formula_regions(const arena& ar,
		std::optional<size_t> closed = std::nullopt)
		: a(ar), closed_type(closed) {}

	tref norm(tref f) {
		if (failed) return tau::_F();
		const auto& t = tau::get(f);
		if (t.equals_T() || t.equals_F()) return f;
		if (auto it = normal.find(f); it != normal.end()) return it->second;
		if (closed_type) return f;
		tref n = dq.eliminate(f);
		rep.append(std::move(dq.rep));
		dq.rep.clear();
		if (!n) { failed = true; return tau::_F(); }
		normal.emplace(f, n);
		return n;
	}

	// `var`, a stream read at the current step, bound in `body` under a
	// fresh name: a region that keeps its quantifiers is read one step
	// later by the predecessor, which moves every stream it names, and a
	// bound one must stay where it is.
	tref bind(tref var, tref body, bool exists) {
		tref fresh = tau::build_variable(find_ba_type<node>(var));
		body = rewriter::replace<node>(body,
			subtree_map<node, tref>{ { var, fresh } });
		return exists ? tau::build_wff_ex(fresh, body, false)
			: tau::build_wff_all(fresh, body, false);
	}

	// The truth of the closed formula `q`, when decided.
	std::optional<bool> decide(tref q) {
		const auto& t = tau::get(q);
		if (t.equals_T()) return true;
		if (t.equals_F()) return false;
		return pack_decide_closed<node>(*closed_type, q);
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
		if (closed_type) {
			tref q = f;
			for (tref v : tau::get(f).get_free_vars())
				q = tau::build_wff_ex(v, q, false);
			auto sat = decide(q);
			if (!sat) { failed = true; return true; }
			return !*sat;
		}
		if (qlt_order_conj_unsat<node>(f)) return true;
		// an undecided check is no emptiness: the game fails instead
		auto sat = is_non_temp_nso_satisfiable<node>(f);
		if (!sat.has_value()) {
			failed = true;
			rep.append(std::move(sat).report());
			return true;
		}
		bool empty = !sat.value();
		rep.append(std::move(sat).report());
		return empty;
	}
	std::optional<bool> reached(tref f) {
		if (!closed_type) {
			auto v = dq.reached_before_start(f);
			rep.append(std::move(dq.rep));
			dq.rep.clear();
			return v;
		}
		tref q = dq.before_start(f, true);
		rep.append(std::move(dq.rep));
		dq.rep.clear();
		return q ? decide(q) : std::nullopt;
	}
	// The positions from which the chooser of vertex `i` takes edge `j`
	// into `Y`.
	tref move(size_t i, size_t j, const std::vector<tref>& Y) {
		const auto& e = a.v[i].edges[j];
		tref tgt = e.shift ? shift_io_vars<node>(Y[e.dst], 1) : Y[e.dst];
		return norm(tau::build_wff_and(e.label, tgt));
	}

	tref pre(int p, size_t i, const std::vector<tref>& Y,
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
				body = closed_type ? bind(var, body, mine)
					: dq.quantify(var, body, mine);
		}
		return norm(body);
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
// it), which makes the game on the codes the game on the data.
//
// A stream also compared with the complement of another (x = y') takes an
// orbit code instead: in a Boolean algebra no value is its own complement,
// so the values fall into pairs {v, v'}, {0, 1} among them. The code is the
// pair (bits 0 .. width-2, pair 0 for {0, 1}) and which of the two it is
// (the last bit, 1 for 1 and for the complement of the value first met).
// Equalities and complements read the values only through these pairs, and
// the type realizes every such pattern of a window when it has as many
// values, none equal to another or to its complement, as the window holds
// (make_code_window checks it).
//
// A stream of a type with few elements (at most 16, a power of two) read
// in any other way takes a value code: code c is the c-th element,
// values[c], element 0 being 0. The code then is the value, and each
// comparison is tabulated over the values of its streams.
//
// A stream of a type whose values are the integers below 2^n under
// modular semantics (pack_modular_width) takes its n bits as its code, and
// a comparison over such streams becomes a circuit on the bits
// (code_regions::blast): the code is the value.
//
// A stream of a dense linear order without endpoints (pack_type_is_dense_
// order) read through order comparisons takes no bits of its own. What the
// comparisons read of a window is its order type: how its values and the
// constants of the atoms compare, and every order type of such a window is
// realized by the order, and any two windows of one order type are carried
// onto each other by an automorphism fixing the constants. So a window is
// coded by one relation per pair of points (a slot of a stream at a step
// back, or a constant): two variables, `lt` (the first point is below the
// second) and `eq`; neither set means above. An assignment that is no weak
// order codes no window; the moves of the game keep to weak orders
// (code_regions::quantify_step), so the positions that are none form a
// part of the game no play from a window enters.
//
// Bit b of stream s at step t-k is variable b * block() + k * S + s, S the
// number of streams: the same bit of every value sits in one layer, so an
// equality of two values stays small in the BDD. The relations follow the
// bits, ordered by the step of their more recent point first. Reading a
// region of the next step at the current one renames each variable to the
// same bit, or the same relation, one step closer (shift).
struct code_window {
	struct stream {
		std::string name;
		bool input = false;
		bool two = false;       // a two-element type, one bit
		bool orbit = false;     // an orbit code, see above
		std::vector<htref> values;   // a value code: the elements
		size_t modular = 0;     // the width of a modular type, see above
		bool order = false;     // read through the order relations
		size_t width = 1;
		size_t tid = 0;
		size_t reach = 0;       // the deepest step back it is read at
		// whether the stream's values are the codes themselves
		bool finite() const {
			return two || !values.empty() || modular;
		}
	};
	// A point of the order relations: a slot (stream s, step back k) or a
	// constant.
	struct point {
		size_t s = SIZE_MAX, k = 0, tid = 0;
		htref constant;
		bool slot() const { return s != SIZE_MAX; }
	};
	std::vector<stream> streams;
	std::map<std::string, size_t> index;
	size_t depth = 0, max_width = 0;
	std::vector<point> points;
	// the first variable (lt) of the relation of an ordered pair of
	// points; eq follows it
	std::map<std::pair<size_t, size_t>, uint32_t> rel;
	// per relation variable pair, its points
	std::vector<std::pair<size_t, size_t>> rel_points;
	// the order of two constants, -1, 0 or 1
	std::map<std::pair<size_t, size_t>, int> fixed;
	std::vector<uint32_t> shift;

	uint32_t block() const {
		return (uint32_t)((depth + 1) * streams.size());
	}
	uint32_t var(size_t s, size_t k, size_t b) const {
		return (uint32_t)(b * block() + k * streams.size() + s);
	}
	uint32_t code_vars() const { return (uint32_t)(max_width * block()); }
	uint32_t vars() const {
		return code_vars() + (uint32_t)(2 * rel_points.size());
	}
	bool has_order() const { return !rel_points.empty(); }
	size_t slot_point(size_t s, size_t k) const {
		for (size_t p = 0; p < points.size(); ++p)
			if (points[p].s == s && points[p].k == k) return p;
		return SIZE_MAX;
	}
	// The first variable of the relation of points p and q, and whether it
	// relates them as (q, p); nullopt for two constants or p == q.
	std::optional<std::pair<uint32_t, bool>> relation(size_t p, size_t q)
		const
	{
		if (auto it = rel.find({ p, q }); it != rel.end())
			return std::pair{ it->second, false };
		if (auto it = rel.find({ q, p }); it != rel.end())
			return std::pair{ it->second, true };
		return std::nullopt;
	}

	// Lays out the relations of `points` after the code bits, and fills
	// `shift`: a variable of a region over the next step's window renamed
	// to the variable of the same value as the current step reads it.
	void layout() {
		// a pair is oriented by (step back, stream) of its points, a slot
		// before a constant; the key orders the relations so that moving
		// both points one step changes only the first component
		using key = std::tuple<size_t, int, size_t, size_t, size_t>;
		std::vector<std::pair<key, std::pair<size_t, size_t>>> pairs;
		auto before = [&](size_t p, size_t q) {
			const auto& a = points[p];
			const auto& b = points[q];
			return std::pair{ a.k, a.s } < std::pair{ b.k, b.s };
		};
		for (size_t p = 0; p < points.size(); ++p)
			for (size_t q = 0; q < points.size(); ++q) {
				const auto& a = points[p];
				const auto& b = points[q];
				if (p == q || !a.slot() || a.tid != b.tid) continue;
				if (b.slot()) {
					if (!before(p, q)) continue;
					pairs.push_back({ key{ a.k, 1, b.k - a.k, a.s, b.s },
						{ p, q } });
				} else pairs.push_back({ key{ a.k, 0, 0, a.s, q },
					{ p, q } });
			}
		std::sort(pairs.begin(), pairs.end());
		rel.clear();
		rel_points.clear();
		for (auto& [_, pq] : pairs) {
			rel.emplace(pq, code_vars() + 2 * (uint32_t)rel_points.size());
			rel_points.push_back(pq);
		}
		shift.assign(vars(), data_bdd::leaf);
		for (size_t s = 0; s < streams.size(); ++s)
			for (size_t k = 1; k <= depth; ++k)
				for (size_t b = 0; b < streams[s].width; ++b)
					shift[var(s, k, b)] = var(s, k - 1, b);
		auto back = [&](size_t p) {
			const auto& a = points[p];
			return !a.slot() ? p : a.k ? slot_point(a.s, a.k - 1)
				: SIZE_MAX;
		};
		for (size_t i = 0; i < rel_points.size(); ++i) {
			auto [p, q] = rel_points[i];
			const size_t p2 = back(p), q2 = back(q);
			if (p2 == SIZE_MAX || q2 == SIZE_MAX) continue;
			const uint32_t v = code_vars() + 2 * (uint32_t)i;
			const uint32_t v2 = rel.at({ p2, q2 });
			shift[v] = v2;
			shift[v + 1] = v2 + 1;
		}
	}
};

// One side of an equality read on codes: an io_var (its variable node) or
// the constant 0 or 1.
template <NodeType node>
struct code_side { tref var = nullptr; int constant = -1; };

// The sides of `cmp` when it is an equality or a disequality between io_vars
// and the constants 0 and 1, else nullopt; `equal` tells which. A
// complement is read off: x' = c is x = c', and x' = y' is x = y; `flip`
// tells an equality of one variable with the complement of the other.
template <NodeType node>
static std::optional<std::array<code_side<node>, 2>> code_equality(tref cmp,
	bool& equal, bool& flip)
{
	using tau = tree<node>;
	flip = false;
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
		flip = complement[0] != complement[1];
		return sides;
	}
	for (size_t i = 0; i < 2; ++i)
		if (complement[i]) sides[1 - i].constant = 1 - sides[1 - i].constant;
	return sides;
}

// Whether a window of `n` values of type `tid` realizes every pattern its
// codes can take: `n` values distinct from 0, 1 and one another, and with
// `orbits` from each other's complements, none its own complement.
template <NodeType node>
static bool codes_realized(size_t tid, size_t n, bool orbits, report& rep) {
	using tau = tree<node>;
	auto var = [&](size_t j) {
		return build_out_var_at_t<node>(build_var_name<node>(
			"o__code" + std::to_string(j)), tid);
	};
	auto decided_false = [&](tref f) {
		auto sat = is_non_temp_nso_satisfiable<node>(f);
		bool decided = sat.has_value();
		bool value = decided && !sat.value();
		rep.append(std::move(sat).report());
		return value;
	};
	if (orbits) {
		tref x = var(0);
		if (!decided_false(tau::build_bf_eq(x, tau::build_bf_neg(x)))
			|| !decided_false(tau::build_bf_neq(x, tau::build_bf_neg(
				tau::build_bf_neg(x))))
			|| !decided_false(tau::build_bf_neq(build_bf_t_type<node>(tid),
				tau::build_bf_neg(build_bf_f_type<node>(tid)))))
				return false;
	}
	tref all = tau::_T();
	trefs fresh;
	for (size_t j = 0; j < n; ++j) {
		tref x = var(j);
		all = tau::build_wff_and(all, tau::build_wff_and(
			tau::build_bf_neq(x, build_bf_f_type<node>(tid)),
			tau::build_bf_neq(x, build_bf_t_type<node>(tid))));
		for (tref y : fresh) {
			all = tau::build_wff_and(all, tau::build_bf_neq(x, y));
			if (orbits) all = tau::build_wff_and(all,
				tau::build_bf_neq(x, tau::build_bf_neg(y)));
		}
		fresh.push_back(x);
	}
	auto sat = is_non_temp_nso_satisfiable<node>(all);
	bool decided = sat.has_value();
	bool value = decided && sat.value();
	rep.append(std::move(sat).report());
	return value;
}

// The elements of type `tid` when it has at most `max` of them, a power of
// two, and its constants of the values 0, 1, ... name them: element 0 is 0,
// no two are equal and no other value exists.
template <NodeType node>
static std::optional<trefs> finite_elements(size_t tid, size_t max,
	report& rep)
{
	using tau = tree<node>;
	// the number of elements per type and bound, 0 for too many; the
	// checks call the solver, whose answers the LTL(ABA) budgets and the
	// algebras' options steer, so a change of either drops the entries
	static std::mutex known_mutex;
	static std::map<std::pair<std::string, size_t>, size_t> known;
	static size_t known_budget = ltl_verdict_budget_fingerprint(
		pack_ba_options_fingerprint<node>());
	static const bool reset_registered = (on_reset([] {
		std::lock_guard lock(known_mutex);
		known.clear();
	}), true);
	(void)reset_registered;
	auto type_name = get_ba_type_name<node>(tid);
	if (!type_name.has_value()) {
		rep.append(std::move(type_name).report());
		return std::nullopt;
	}
	const std::string name = type_name.value();
	rep.append(std::move(type_name).report());
	auto elements = [&](size_t n) {
		trefs els;
		for (size_t v = 0; v < n; ++v)
			els.push_back(pack_value_constant<node>(tid, v));
		return els;
	};
	{
		std::lock_guard lock(known_mutex);
		if (const size_t fp = ltl_verdict_budget_fingerprint(
				pack_ba_options_fingerprint<node>());
			fp != known_budget) { known.clear(); known_budget = fp; }
		if (auto it = known.find({ name, max }); it != known.end()) {
			if (!it->second) return std::nullopt;
			return elements(it->second);
		}
	}
	// a check the solver leaves open decides nothing, so its answer is
	// not remembered
	bool undecided = false;
	auto remember = [&](std::optional<trefs> els) {
		if (!undecided) {
			std::lock_guard lock(known_mutex);
			known.emplace(std::pair{ name, max }, els ? els->size() : 0);
		}
		return els;
	};
	auto decided_false = [&](tref f) {
		auto sat = is_non_temp_nso_satisfiable<node>(f);
		if (!sat.has_value()) undecided = true;
		bool value = sat.has_value() && !sat.value();
		rep.append(std::move(sat).report());
		return value;
	};
	trefs els;
	for (size_t n = 2; n <= max; n *= 2) {
		while (els.size() < n) {
			tref e = pack_value_constant<node>(tid, els.size());
			if (!e) return remember(std::nullopt);
			for (tref d : els)
				if (!decided_false(tau::build_bf_eq(e, d)))
					return remember(std::nullopt);
			els.push_back(e);
		}
		if (!decided_false(tau::build_bf_neq(els[0],
			build_bf_f_type<node>(tid)))) return remember(std::nullopt);
		tref x = build_out_var_at_t<node>(build_var_name<node>("o__code"),
			tid);
		tref other = tau::_T();
		for (tref e : els)
			other = tau::build_wff_and(other, tau::build_bf_neq(x, e));
		if (decided_false(other)) return remember(els);
	}
	return remember(std::nullopt);
}

// The widest modular type whose streams take their bits as codes.
inline constexpr size_t max_blasted_width = 16;

// The point of `w` an operand of an order comparison stands for: a slot of
// an order stream or a constant of the same type; SIZE_MAX for anything
// else.
template <NodeType node>
static size_t order_point(const code_window& w, tref operand) {
	using tau = tree<node>;
	const auto& b = tau::get(operand);
	if (!b.is(tau::bf) || b.children_size() != 1) return SIZE_MAX;
	if (is_child<node, tau::io_var>(b.first())) {
		tref v = b.first();
		auto it = w.index.find(get_var_name<node>(v));
		if (it == w.index.end() || !w.streams[it->second].order)
			return SIZE_MAX;
		return w.slot_point(it->second, (size_t)get_io_var_shift<node>(v));
	}
	if (!tau::get(b.first()).is_ba_constant()) return SIZE_MAX;
	for (size_t p = 0; p < w.points.size(); ++p) {
		const auto& x = w.points[p];
		if (x.slot()) continue;
		auto c = pack_dense_order_compare<node>(x.tid, x.constant->get(),
			operand);
		if (c && *c == 0) return p;
	}
	return SIZE_MAX;
}

// The code window of `atoms`, when every stream has a two-element type, is
// read only through equalities and complements, has few elements, a
// modular type of at most max_blasted_width bits, or a dense order read
// through order comparisons; at most `max_vars` variables.
template <NodeType node>
static std::optional<code_window> make_code_window(
	const std::vector<std::pair<tref, std::string>>& atoms, size_t max_vars,
	report& rep)
{
	using tau = tree<node>;
	data_quantifier<node> dq;
	// The window's checks answer through `nullopt`; their reports merge here.
	auto fail = [&]() -> std::optional<code_window> {
		rep.append(std::move(dq.rep));
		return std::nullopt;
	};
	code_window w;
	for (auto& [atom, _] : atoms)
		for (tref v : tau::get(atom).select_top(is_child<node, tau::io_var>)) {
			if (is_io_initial<node>(v)) return fail();
			const size_t k = (size_t)get_io_var_shift<node>(v);
			w.depth = std::max(w.depth, k);
			auto [it, fresh] = w.index.emplace(get_var_name<node>(v),
				w.streams.size());
			if (fresh) {
				code_window::stream s;
				s.name = it->first;
				s.input = is_input_stream<node>(v);
				s.two = dq.is_two_element(v);
				s.tid = find_ba_type<node>(v);
				w.streams.push_back(s);
			}
			auto& s = w.streams[it->second];
			s.reach = std::max(s.reach, k);
		}
	// A stream of another type is read only through equalities, or has few
	// elements, a modular type or a dense order; the types some complement
	// relates take orbit codes.
	std::set<size_t> orbit_types, value_types, modular_types, order_types;
	for (auto& [atom, _] : atoms)
		for (tref c : tau::get(atom).select_all(is_aba_comparison<node>)) {
			auto vars = tau::get(c).select_top(is_child<node, tau::io_var>);
			if (std::all_of(vars.begin(), vars.end(), [&](tref v) {
				return w.streams[w.index.at(get_var_name<node>(v))].two; }))
					continue;
			bool equal, flip;
			if (!code_equality<node>(c, equal, flip)) {
				for (tref v : vars) {
					const auto& s = w.streams[w.index.at(
						get_var_name<node>(v))];
					if (s.two) continue;
					const size_t n = pack_modular_width<node>(s.tid);
					if (pack_type_is_dense_order<node>(s.tid))
						order_types.insert(s.tid);
					else if (n && n <= max_blasted_width)
						modular_types.insert(s.tid);
					else value_types.insert(s.tid);
				}
			} else if (flip) orbit_types.insert(find_ba_type<node>(vars[0]));
		}
	// A dense order has no element for the codes of 0 and 1 to stand
	// for: its streams take order-type codes even when only equalities
	// read them.
	for (auto& s : w.streams)
		if (!s.two && pack_type_is_dense_order<node>(s.tid))
			order_types.insert(s.tid);
	for (auto& s : w.streams) {
		if (s.two) continue;
		if (modular_types.contains(s.tid)) {
			s.modular = pack_modular_width<node>(s.tid);
			// few enough values to tabulate what the circuits miss
			if (s.modular <= 4)
				for (size_t v = 0; v < (size_t{1} << s.modular); ++v) {
					tref e = pack_value_constant<node>(s.tid, v);
					if (!e) return fail();
					s.values.push_back(tau::geth(e));
				}
		} else if (order_types.contains(s.tid)) s.order = true;
	}
	for (size_t tid : value_types) {
		auto els = finite_elements<node>(tid, 16, rep);
		if (!els) return fail();
		for (auto& s : w.streams)
			if (s.tid == tid && !s.two)
				for (tref e : *els) s.values.push_back(tau::geth(e));
	}
	// The points of the order relations: every slot of an order stream and
	// every constant its comparisons name, which must be order
	// comparisons of such slots and constants.
	for (size_t s = 0; s < w.streams.size(); ++s)
		if (w.streams[s].order)
			for (size_t k = 0; k <= w.streams[s].reach; ++k)
				w.points.push_back({ s, k, w.streams[s].tid, {} });
	for (auto& [atom, _] : atoms)
		for (tref c : tau::get(atom).select_all(is_aba_comparison<node>)) {
			auto vars = tau::get(c).select_top(is_child<node, tau::io_var>);
			if (vars.empty() || !w.streams[w.index.at(
				get_var_name<node>(vars[0]))].order) continue;
			const size_t tid = w.streams[w.index.at(
				get_var_name<node>(vars[0]))].tid;
			const auto& op = tau::get(c)[0];
			for (size_t i = 0; i < op.children_size(); ++i) {
				tref x = op.child(i);
				if (order_point<node>(w, x) != SIZE_MAX) continue;
				const auto& b = tau::get(x);
				if (!b.is(tau::bf) || b.children_size() != 1
					|| !tau::get(b.first()).is_ba_constant()
					|| !pack_dense_order_compare<node>(tid, x, x))
						return fail();
				w.points.push_back({ SIZE_MAX, 0, tid, tau::geth(x) });
			}
		}
	for (size_t p = 0; p < w.points.size(); ++p)
		for (size_t q = 0; q < w.points.size(); ++q) {
			const auto& a = w.points[p];
			const auto& b = w.points[q];
			if (p == q || a.slot() || b.slot() || a.tid != b.tid) continue;
			auto c = pack_dense_order_compare<node>(a.tid,
				a.constant->get(), b.constant->get());
			if (!c) return fail();
			w.fixed[{ p, q }] = *c;
		}
	// Each other type needs as many values as a window holds, plus 0 and 1.
	std::map<size_t, size_t> slots;
	for (auto& s : w.streams)
		if (!s.finite() && !s.order) slots[s.tid] += w.depth + 1;
	std::map<size_t, size_t> width;
	for (auto& [tid, n] : slots) {
		const bool orbits = orbit_types.contains(tid);
		// codes 0 .. n+1, or pairs 0 .. n and the bit telling the value
		size_t bits = 1;
		while ((size_t{1} << bits) < n + (orbits ? 1 : 2)) ++bits;
		width[tid] = bits + (orbits ? 1 : 0);
		if (!codes_realized<node>(tid, n, orbits, rep)) return fail();
	}
	for (auto& x : w.streams) {
		x.orbit = !x.finite() && !x.order && orbit_types.contains(x.tid);
		if (x.order) x.width = 0;
		else if (x.modular) x.width = x.modular;
		else if (x.values.empty()) x.width = x.two ? 1 : width[x.tid];
		else for (x.width = 0; (size_t{1} << x.width) < x.values.size();)
			++x.width;
		w.max_width = std::max(w.max_width, x.width);
	}
	w.layout();
	if (w.vars() > max_vars) return fail();
	rep.append(std::move(dq.rep));
	return w;
}

// Points p and q of `w` in relation r: -1 p below q, 0 equal, 1 above. A
// variable `known` gives a value (0 or 1) is read as that value.
static data_bdd::id order_literal(const code_window& w, data_bdd& bdd,
	size_t p, size_t q, int r, const std::vector<int>* known = nullptr)
{
	if (p == q) return r == 0 ? data_bdd::T : data_bdd::F;
	auto rel = w.relation(p, q);
	if (!rel) {
		if (auto it = w.fixed.find({ p, q }); it != w.fixed.end())
			return it->second == r ? data_bdd::T : data_bdd::F;
		return data_bdd::F;
	}
	auto [v, flip] = *rel;
	if (flip) r = -r;
	auto bit = [&](uint32_t x, bool pos) {
		if (known && x < known->size() && (*known)[x] >= 0)
			return ((*known)[x] == 1) == pos ? data_bdd::T : data_bdd::F;
		return bdd.var(x, pos);
	};
	return bdd.conj(bit(v, r == -1), bit(v + 1, r == 0));
}

// The weak orders of the points `pts` (of one type), as far as the pairs
// and triples with a point flagged in `in_x` read them. Over points whose
// relations are a weak order, it holds exactly when the relations of the
// flagged points extend that order to a weak order: every weak order of a
// subset extends to one of the whole, so no other point constrains them.
static data_bdd::id order_consistent(const code_window& w, data_bdd& bdd,
	const std::vector<size_t>& pts, const std::vector<bool>& in_x,
	const std::vector<int>* known = nullptr)
{
	// the relations (a, b), (b, c), (a, c) of the 13 weak orders of a, b, c
	static const auto triples = [] {
		std::set<std::array<int, 3>> out;
		auto cmp = [](int x, int y) { return x < y ? -1 : x > y ? 1 : 0; };
		for (int a = 0; a < 3; ++a)
			for (int b = 0; b < 3; ++b)
				for (int c = 0; c < 3; ++c)
					out.insert({ cmp(a, b), cmp(b, c), cmp(a, c) });
		return out;
	}();
	data_bdd::id all = data_bdd::T;
	auto lit = [&](size_t i, size_t j, int r) {
		return order_literal(w, bdd, pts[i], pts[j], r, known);
	};
	const size_t n = pts.size();
	for (size_t i = 0; i < n; ++i)
		for (size_t j = i + 1; j < n; ++j) {
			if (!in_x[i] && !in_x[j]) continue;
			all = bdd.conj(all, bdd.disj(lit(i, j, -1),
				bdd.disj(lit(i, j, 0), lit(i, j, 1))));
			for (size_t l = j + 1; l < n; ++l) {
				data_bdd::id some = data_bdd::F;
				for (const auto& t : triples)
					some = bdd.disj(some, bdd.conj(lit(i, j, t[0]),
						bdd.conj(lit(j, l, t[1]), lit(i, l, t[2]))));
				all = bdd.conj(all, some);
			}
		}
	for (size_t i = 0; i < n; ++i)
		for (size_t j = i + 1; j < n; ++j)
			for (size_t l = j + 1; l < n; ++l) {
				if (in_x[i] || in_x[j] || !in_x[l]) continue;
				data_bdd::id some = data_bdd::F;
				for (const auto& t : triples)
					some = bdd.disj(some, bdd.conj(lit(i, j, t[0]),
						bdd.conj(lit(j, l, t[1]), lit(i, l, t[2]))));
				all = bdd.conj(all, some);
			}
	return all;
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
	// failed, for a reason other than a full table
	bool declined = false;
	bool failed = false;
	// Reports of the checks that answer through `failed`; merged by the caller.
	report rep;
	std::vector<region> labels;           // per vertex, per edge
	std::vector<size_t> edge_base;
	std::map<std::vector<size_t>, region> consistent;

	// 0 for either cap is no bound; for the nodes, as many as an id names
	code_regions(const arena& ar, const code_window& win, size_t max_nodes,
		size_t max_memo)
		: a(ar), w(win), bdd(max_nodes && max_nodes < data_bdd::max_ids
			? max_nodes : data_bdd::max_ids, max_memo,
			data_bdd::memo_policy::clear) {}

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

	// Whether the last work failed only for want of nodes, so that it may
	// be redone after a collect() that makes room for it.
	bool out_of_nodes() const { return bdd.full && !declined; }
	bool wants_collect() const { return bdd.wants_collect(); }

	// Frees the nodes that neither the labels nor the regions `each_root`
	// marks reach. After a full table the cached order extensions go too,
	// as they may have been computed while full, and `failed` is cleared:
	// the caller redoes the work that ran out, or fails.
	template <typename Roots>
	void collect(Roots&& each_root) {
		if (bdd.full) {
			consistent.clear();
			failed = declined;
		}
		bdd.collect([&](auto&& mark) {
			for (region l : labels) mark(l);
			for (const auto& [_, c] : consistent) mark(c);
			each_root(mark);
		});
	}

	// Whether point p is a slot of the chooser's streams at step t-k.
	bool chosen(size_t p, size_t k, bool inputs) const {
		const auto& x = w.points[p];
		return x.slot() && x.k == k && w.streams[x.s].input == inputs;
	}

	// The variables of the chooser's streams at step t-k: their bits and
	// the relations of their slots.
	std::vector<bool> step_vars(size_t k, bool inputs) const {
		std::vector<bool> qs(w.vars(), false);
		for (size_t s = 0; s < w.streams.size(); ++s)
			if (w.streams[s].input == inputs)
				for (size_t b = 0; b < w.streams[s].width; ++b)
					qs[w.var(s, k, b)] = true;
		for (size_t i = 0; i < w.rel_points.size(); ++i) {
			auto [p, q] = w.rel_points[i];
			if (!chosen(p, k, inputs) && !chosen(q, k, inputs)) continue;
			qs[w.code_vars() + 2 * i] = qs[w.code_vars() + 2 * i + 1] = true;
		}
		return qs;
	}

	// `body` with the chooser's values at step t-k quantified: over the
	// values, or, for order streams, over the relations that extend the
	// order of the other points to a weak order. Only the points `body`
	// relates to the chosen slots constrain that extension.
	region quantify_step(region body, size_t k, bool inputs, bool exists) {
		if (w.has_order()) {
			std::set<size_t> used;
			for (uint32_t v : bdd.support(body)) {
				if (v < w.code_vars()) continue;
				auto [p, q] = w.rel_points[(v - w.code_vars()) / 2];
				if (chosen(p, k, inputs) || chosen(q, k, inputs))
					used.insert(p), used.insert(q);
			}
			std::map<size_t, std::vector<size_t>> by_type;
			for (size_t p : used) by_type[w.points[p].tid].push_back(p);
			for (auto& [_, pts] : by_type) {
				std::vector<bool> in_x;
				std::vector<size_t> key;
				for (size_t p : pts) {
					in_x.push_back(chosen(p, k, inputs));
					key.push_back(2 * p + in_x.back());
				}
				auto it = consistent.find(key);
				if (it == consistent.end()) it = consistent.emplace(key,
					order_consistent(w, bdd, pts, in_x)).first;
				body = exists ? bdd.conj(body, it->second)
					: bdd.disj(bdd.neg(it->second), body);
			}
		}
		return bdd.quantify(body, step_vars(k, inputs), exists);
	}

	// The value of io_var `v` equals the constant `c`, or the value of io_var
	// `u`, or its complement with `flip`.
	region code_eq(tref v, int c, tref u, bool flip = false) {
		const size_t s = w.index.at(get_var_name<node>(v));
		const size_t k = (size_t)get_io_var_shift<node>(v);
		const auto& x = w.streams[s];
		// the last bit of an orbit code tells the value of its pair
		const size_t last = x.orbit ? x.width - 1 : x.width;
		region r = data_bdd::T;
		for (size_t b = 0; b < x.width; ++b) {
			region bit;
			if (u) {
				const size_t s2 = w.index.at(get_var_name<node>(u));
				const size_t k2 = (size_t)get_io_var_shift<node>(u);
				region p = bdd.var(w.var(s, k, b));
				region q = bdd.var(w.var(s2, k2, b), !(flip && b == last));
				bit = bdd.disj(bdd.conj(p, q),
					bdd.conj(bdd.neg(p), bdd.neg(q)));
			} else if (x.orbit)
				bit = bdd.var(w.var(s, k, b), b == last && c == 1);
			else bit = bdd.var(w.var(s, k, b), (c >> b) & 1);
			r = bdd.conj(r, bit);
		}
		return r;
	}

	// A comparison over streams whose codes are their values, tabulated by
	// substituting each combination of values and normalizing; nullopt
	// beyond 4096 combinations.
	std::optional<region> tabulate(tref cmp) {
		struct slot { tref var; size_t s, k; trefs values; };
		std::vector<slot> vars;
		size_t combinations = 1;
		for (tref v : tau::get(cmp).select_top(is_child<node, tau::io_var>)) {
			if (std::any_of(vars.begin(), vars.end(),
				[&](auto& x) { return tau::subtree_equals(x.var, v); }))
					continue;
			const size_t s = w.index.at(get_var_name<node>(v));
			const size_t tid = find_ba_type<node>(v);
			slot x{ v, s, (size_t)get_io_var_shift<node>(v), {} };
			if (w.streams[s].two) x.values = { build_bf_f_type<node>(tid),
				build_bf_t_type<node>(tid) };
			else for (const auto& e : w.streams[s].values)
				x.values.push_back(e->get());
			if (x.values.empty()) return std::nullopt;
			combinations *= x.values.size();
			if (const size_t cap = ltl_data_game_max_combinations();
				cap && combinations > cap) return std::nullopt;
			vars.push_back(std::move(x));
		}
		region r = data_bdd::F;
		for (size_t val = 0; val < combinations; ++val) {
			subtree_map<node, tref> m;
			region cube = data_bdd::T;
			size_t rest = val;
			for (const auto& x : vars) {
				const size_t c = rest % x.values.size();
				rest /= x.values.size();
				m.emplace(x.var, tau::trim(x.values[c]));
				for (size_t b = 0; b < w.streams[x.s].width; ++b)
					cube = bdd.conj(cube, bdd.var(w.var(x.s, x.k, b),
						c >> b & 1));
			}
			auto n = normalize_non_temp<node>(rewriter::replace<node>(cmp, m));
			if (!n.has_value() || !n.value()) {
				rep.append(std::move(n).report());
				return std::nullopt;
			}
			tref normed = n.value();
			rep.append(std::move(n).report());
			const auto& t = tau::get(normed);
			if (!t.equals_T() && !t.equals_F()) return std::nullopt;
			if (t.equals_T()) r = bdd.disj(r, cube);
		}
		return r;
	}

	using bits = bit_circuits::bits;

	bits add(const bits& x, const bits& y) {
		return bit_circuits::add(bdd, x, y);
	}
	region less(const bits& x, const bits& y) {
		return bit_circuits::less(bdd, x, y);
	}
	region same(const bits& x, const bits& y) {
		return bit_circuits::same(bdd, x, y);
	}
	bits select(region c, const bits& x, const bits& y) {
		return bit_circuits::select(bdd, c, x, y);
	}
	bits shifted(const bits& x, const bits& y, bool left) {
		return bit_circuits::shifted(bdd, x, y, left);
	}

	// The bits of `term`, a term over streams of modular type `tid` and
	// width `n`, least significant first; nullopt for an operator the
	// circuits do not cover, or a product that outgrows its share of the
	// BDD.
	std::optional<bits> blast(tref term, size_t tid, size_t n) {
		const auto& b = tau::get(term);
		if (!b.is(tau::bf) || b.children_size() != 1) return std::nullopt;
		if (is_child<node, tau::io_var>(b.first())) {
			tref v = b.first();
			auto it = w.index.find(get_var_name<node>(v));
			if (it == w.index.end()) return std::nullopt;
			const auto& x = w.streams[it->second];
			if (x.modular != n) return std::nullopt;
			const size_t k = (size_t)get_io_var_shift<node>(v);
			bits out(n);
			for (size_t i = 0; i < n; ++i)
				out[i] = bdd.var(w.var(it->second, k, i));
			return out;
		}
		const auto& op = tau::get(b.first());
		const auto nt = op.value.nt;
		if (op.is_ba_constant() || nt == tau::bf_t || nt == tau::bf_f) {
			auto c = pack_modular_value<node>(tid, term);
			if (!c) return std::nullopt;
			bits out(n);
			for (size_t i = 0; i < n; ++i)
				out[i] = i < 64 && (*c >> i & 1) ? data_bdd::T
					: data_bdd::F;
			return out;
		}
		if (nt == tau::bf_parenthesis && op.children_size() == 1)
			return blast(op.child(0), tid, n);
		if (nt == tau::bf_neg && op.children_size() == 1) {
			auto x = blast(op.child(0), tid, n);
			if (!x) return std::nullopt;
			for (auto& e : *x) e = bdd.neg(e);
			return x;
		}
		if (op.children_size() != 2) return std::nullopt;
		auto x = blast(op.child(0), tid, n);
		if (!x) return std::nullopt;
		auto y = blast(op.child(1), tid, n);
		if (!y) return std::nullopt;
		bits out(n);
		auto bitwise = [&](auto f) {
			for (size_t i = 0; i < n; ++i) out[i] = f((*x)[i], (*y)[i]);
			return out;
		};
		switch (nt) {
		case tau::bf_and: return bitwise([&](region p, region q) {
			return bdd.conj(p, q); });
		case tau::bf_or: return bitwise([&](region p, region q) {
			return bdd.disj(p, q); });
		case tau::bf_xor: return bitwise([&](region p, region q) {
			return bdd.exor(p, q); });
		case tau::bf_nand: return bitwise([&](region p, region q) {
			return bdd.neg(bdd.conj(p, q)); });
		case tau::bf_nor: return bitwise([&](region p, region q) {
			return bdd.neg(bdd.disj(p, q)); });
		case tau::bf_xnor: return bitwise([&](region p, region q) {
			return bdd.iff(p, q); });
		case tau::bf_add: return add(*x, *y);
		case tau::bf_sub: return bit_circuits::sub(bdd, *x, *y);
		case tau::bf_mul: {
			// a product may grow the BDD exponentially, so it gets a
			// quarter of the table
			if (!bit_circuits::mul(bdd, *x, *y,
				bdd.size() + bdd.max_nodes / 4, out))
					return std::nullopt;
			return out;
		}
		case tau::bf_shl: return shifted(*x, *y, true);
		case tau::bf_shr: return shifted(*x, *y, false);
		case tau::bf_min: return select(less(*y, *x), *y, *x);
		case tau::bf_max: return select(less(*x, *y), *y, *x);
		default: return std::nullopt;
		}
	}

	// A comparison over streams of one modular type as a circuit.
	std::optional<region> blast_comparison(tref cmp) {
		const auto& t = tau::get(cmp);
		auto vars = t.select_top(is_child<node, tau::io_var>);
		if (vars.empty()) return std::nullopt;
		const auto& s = w.streams[w.index.at(get_var_name<node>(vars[0]))];
		const auto& op = t[0];
		std::vector<bits> xs;
		for (size_t i = 0; i < op.children_size(); ++i) {
			auto x = blast(op.child(i), s.tid, s.modular);
			if (!x || bdd.full) return std::nullopt;
			xs.push_back(std::move(*x));
		}
		const auto nt = op.value.nt;
		if (nt == tau::bf_interval) {
			if (xs.size() != 3) return std::nullopt;
			return bdd.conj(bdd.neg(less(xs[1], xs[0])),
				bdd.neg(less(xs[2], xs[1])));
		}
		if (xs.size() != 2) return std::nullopt;
		const bits& x = xs[0];
		const bits& y = xs[1];
		switch (nt) {
		case tau::bf_eq: return same(x, y);
		case tau::bf_neq: return bdd.neg(same(x, y));
		case tau::bf_lt: return less(x, y);
		case tau::bf_nlt: return bdd.neg(less(x, y));
		case tau::bf_lteq: return bdd.neg(less(y, x));
		case tau::bf_nlteq: return less(y, x);
		case tau::bf_gt: return less(y, x);
		case tau::bf_ngt: return bdd.neg(less(y, x));
		case tau::bf_gteq: return bdd.neg(less(x, y));
		case tau::bf_ngteq: return less(x, y);
		default: return std::nullopt;
		}
	}

	// An order comparison over order streams and constants.
	std::optional<region> order_comparison(tref cmp) {
		const auto& op = tau::get(cmp)[0];
		std::vector<size_t> ps;
		for (size_t i = 0; i < op.children_size(); ++i) {
			const size_t p = order_point<node>(w, op.child(i));
			if (p == SIZE_MAX) return std::nullopt;
			ps.push_back(p);
		}
		auto lit = [&](size_t i, size_t j, int r) {
			return order_literal(w, bdd, ps[i], ps[j], r);
		};
		auto at_most = [&](size_t i, size_t j) {
			return bdd.disj(lit(i, j, -1), lit(i, j, 0));
		};
		const auto nt = op.value.nt;
		if (nt == tau::bf_interval) {
			if (ps.size() != 3) return std::nullopt;
			return bdd.conj(at_most(0, 1), at_most(1, 2));
		}
		if (ps.size() != 2) return std::nullopt;
		switch (nt) {
		case tau::bf_eq: return lit(0, 1, 0);
		case tau::bf_neq: return bdd.neg(lit(0, 1, 0));
		case tau::bf_lt: return lit(0, 1, -1);
		case tau::bf_nlt: return bdd.neg(lit(0, 1, -1));
		case tau::bf_lteq: return at_most(0, 1);
		case tau::bf_nlteq: return bdd.neg(at_most(0, 1));
		case tau::bf_gt: return lit(0, 1, 1);
		case tau::bf_ngt: return bdd.neg(lit(0, 1, 1));
		case tau::bf_gteq: return at_most(1, 0);
		case tau::bf_ngteq: return bdd.neg(at_most(1, 0));
		default: return std::nullopt;
		}
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
		auto all = [&](auto pred) {
			return std::all_of(vars.begin(), vars.end(), [&](tref v) {
				return pred(w.streams[w.index.at(get_var_name<node>(v))]);
			});
		};
		if (all([](const auto& s) { return s.finite(); })) {
			if (all([](const auto& s) { return s.modular > 0; }))
				if (auto r = blast_comparison(f)) return r;
			if (bdd.full) return std::nullopt;
			return tabulate(f);
		}
		if (all([](const auto& s) { return s.order; }))
			return order_comparison(f);
		bool equal, flip;
		auto sides = code_equality<node>(f, equal, flip);
		if (!sides) return std::nullopt;
		auto& [l, rr] = *sides;
		region r;
		if (l.var && rr.var) r = code_eq(l.var, -1, rr.var, flip);
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
				std::optional<region> r;
				for (;;) {
					const size_t room = bdd.room();
					r = eval(e.label);
					if (!out_of_nodes()) break;
					collect([](auto&&) {});
					if (!bdd.worth_redoing(room)) return false;
				}
				if (!r || bdd.full) return false;
				labels.push_back(*r);
			}
		}
		return true;
	}

	region pre(int p, size_t i, const std::vector<region>& Y,
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
				tgt = bdd.rename(tgt, w.shift, ok);
				if (!ok) { failed = declined = true; return data_bdd::F; }
			}
			const region l = labels[edge_base[i] + j];
			body = mine ? bdd.disj(body, bdd.conj(l, tgt))
				: bdd.conj(body, bdd.disj(bdd.neg(l), tgt));
		}
		if (x.picks != arena::chooser::none)
			body = quantify_step(body, 0,
				x.picks == arena::chooser::inputs, mine);
		return check(body);
	}

	region move(size_t i, size_t j, const std::vector<region>& Y) {
		const auto& e = a.v[i].edges[j];
		region tgt = Y[e.dst];
		if (e.shift) {
			bool ok = true;
			tgt = bdd.rename(tgt, w.shift, ok);
			if (!ok) { failed = declined = true; return data_bdd::F; }
		}
		return check(bdd.conj(labels[edge_base[i] + j], tgt));
	}

	std::optional<bool> reached(region r) {
		for (size_t k = 1; k <= w.depth; ++k) {
			r = quantify_step(r, k, false, true);
			r = quantify_step(r, k, true, false);
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
	std::vector<std::vector<size_t>> preds;
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
					i) == preds[e.dst].end())
						preds[e.dst].push_back(i);
		}
	}

	struct won {
		std::vector<region> env, sys;
		moves_t moves;   // of the system, over its region
	};

	// Regions whose nodes can be freed: code_regions.
	static constexpr bool collects = requires (R& x) { x.out_of_nodes(); };

	// The regions the frames of solve() still need; each frame pushes its
	// own and pops them when it returns.
	std::vector<const std::vector<region>*> held;
	std::vector<const moves_t*> held_moves;
	struct frame {
		data_game_solver& s;
		size_t regions, moves;
		~frame() {
			s.held.resize(regions);
			s.held_moves.resize(moves);
		}
	};
	frame enter() { return { *this, held.size(), held_moves.size() }; }

	// Frees the nodes that neither the held regions nor those of the
	// attractor under way reach.
	void collect(const std::vector<region>& Y, const std::vector<region>& G,
		const moves_t* moves)
	{
		if constexpr (collects) r.collect([&](auto&& mark) {
			auto all = [&](const std::vector<region>& X) {
				for (region x : X) mark(x);
			};
			auto all_moves = [&](const moves_t& m) {
				for (const auto& x : m) all(x);
			};
			for (const auto* X : held) all(*X);
			for (const auto* m : held_moves) all_moves(*m);
			all(Y);
			all(G);
			if (moves) all_moves(*moves);
		});
	}

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
	// before it. Between the steps of a vertex, the nodes nothing needs
	// are freed once the table fills, and a step that runs out of nodes is
	// redone after freeing them when that more than doubled its room.
	std::vector<region> attractor(int p, std::vector<region> Y,
		const std::vector<region>& G, moves_t* moves = nullptr)
	{
		std::vector<bool> dirty(n, true);
		for (size_t round = 0; !r.failed; ++round) {
			if (max_rounds && round >= max_rounds) {
				if constexpr (collects) r.declined = true;
				r.failed = true;
				break;
			}
			bool changed = false;
			for (size_t i = 0; i < n && !r.failed; ++i) {
				if (!dirty[i]) continue;
				dirty[i] = false;
				if (r.empty(G[i])) continue;
				const bool recorded = moves && owner[i] == 1;
				std::vector<region> before;
				if constexpr (collects) {
					if (r.wants_collect()) collect(Y, G, moves);
					if (recorded) before = (*moves)[i];
				}
				region grown, added;
				for (;;) {
					size_t room = 0;
					if constexpr (collects) room = r.bdd.room();
					grown = r.disj(Y[i],
						r.conj(G[i], r.pre(p, i, Y, G)));
					added = r.minus(grown, Y[i]);
					if (!r.empty(added) && recorded)
						for (size_t j = 0; j < edges[i]; ++j)
							(*moves)[i][j] = r.disj((*moves)[i][j],
								r.conj(added, r.move(i, j, Y)));
					if constexpr (collects) {
						if (r.out_of_nodes()) {
							if (recorded) (*moves)[i] = before;
							collect(Y, G, moves);
							if (r.bdd.worth_redoing(room)) continue;
							// the step's regions may name freed nodes
							r.failed = true;
							added = r.bottom();
						}
					}
					break;
				}
				if (r.empty(added)) continue;
				Y[i] = std::move(grown);
				changed = true;
				for (size_t j : preds[i]) dirty[j] = true;
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
		auto held_here = enter();
		held.push_back(&G);
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
		held.push_back(&U);
		held_moves.push_back(&attracted);
		auto A = attractor(p, U, G, record && p ? &attracted : nullptr);
		auto sub = solve(minus(G, A));
		if (r.failed) return { none, none, no_moves() };
		held.push_back(&sub.env);
		held.push_back(&sub.sys);
		held_moves.push_back(&sub.moves);
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
								r.conj(U[i], r.move(i, j, G)));
			}
			return all;
		}
		moves_t kept = no_moves();
		held_moves.push_back(&kept);
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
	using solver_fn = std::function<result<std::optional<values>>(tref, int_t)>;
	// The value of a stream (name, type, input) at an absolute step >= 0,
	// or nullptr when there is none.
	using value_fn = std::function<tref(const std::string&, size_t, bool,
		int_t)>;

	struct stream { std::string name; size_t tid = 0; bool input = false; };
	struct vertex { int picks = -1; std::vector<size_t> dst; };

	std::vector<stream> streams;
	std::map<std::string, size_t> index;
	size_t depth = 0;
	// picks 0: the environment chooses the inputs, 1: the system the
	// outputs, -1: nobody (a coloured vertex or a sink)
	std::vector<vertex> v;
	size_t init = 0;
	// Reports of the checks that answer through a value; merged by the caller.
	report rep;

	// The Mealy view (code_strategy::build_mealy), which `step` then
	// plays: a machine over the atoms of the view, each edge a guard on
	// the atoms of the inputs, the atoms the outputs meet and the next
	// state; null when no view was built.
	struct mealy_edge {
		std::vector<std::pair<size_t, int8_t>> guard;
		std::vector<size_t> out;
		size_t dst = 0;
	};
	std::shared_ptr<ltl_aba_solution<node>> view;
	std::vector<std::vector<mealy_edge>> machine;

	virtual ~data_game_strategy() = default;

	void reset() {
		at = init;
		ready = false;
		state_ = view ? view->aut.initial_state : 0;
	}

	// Starts the play from `prior` instead of values of its own:
	// prior[s][k-1] is the value of stream s k steps before the first step
	// played. False when the initial vertex is not won from them.
	result<bool> start_from(const std::vector<std::vector<tref>>& prior) {
		result<bool> r;
		window win(streams.size(), std::vector<tref>(depth + 1, nullptr));
		for (size_t s = 0; s < streams.size(); ++s)
			for (size_t k = 1; k <= depth; ++k) {
				if (s >= prior.size() || k > prior[s].size()
					|| !prior[s][k - 1]) return r.with_value(false);
				win[s][k] = prior[s][k - 1];
			}
		TAU_TRY(auto won, won_from(win));
		if (!won || !*won) return r.with_value(false);
		TAU_TRY(bool view_ok, restart_view(win));
		(void)view_ok;
		before.assign(streams.size(), {});
		for (size_t s = 0; s < streams.size(); ++s)
			for (size_t k = 1; k <= depth; ++k)
				before[s].push_back(tau::geth(win[s][k]));
		at = init;
		ready = true;
		state_ = view ? view->aut.initial_state : 0;
		return r.with_value(true);
	}

	// The value of stream `s` `k` steps before the first step played,
	// nullptr before the play has started.
	tref value_before(size_t s, size_t k) const {
		if (!ready || s >= before.size() || k == 0
			|| k > before[s].size()) return nullptr;
		return before[s][k - 1]->get();
	}

	// The state of the Mealy view, when the strategy plays one.
	std::optional<size_t> state() const {
		if (!view) return std::nullopt;
		return state_;
	}

	// The names of the input streams the next step reads; nullopt when
	// it reads all of them.
	std::optional<std::set<std::string>> reads() const {
		using tau = tree<node>;
		std::set<std::string> names;
		if (!view) {
			auto d = direct_reads();
			if (!d) return std::nullopt;
			// a slot read later is read from memory, so its stream is
			// read at every step
			for (size_t s = 0; s < streams.size(); ++s)
				if (streams[s].input && (d->now[s] || d->back[s]))
					names.insert(streams[s].name);
			return names;
		}
		auto add = [&](size_t a) {
			for (tref x : tau::get(view->atoms[a].first).select_top(
				is_child<node, tau::io_var>))
				if (get_io_var_shift<node>(x) == 0) {
					auto it = index.find(get_var_name<node>(x));
					if (it != index.end() && streams[it->second].input)
						names.insert(it->first);
				}
		};
		for (const auto& e : machine[state_]) {
			for (auto [a, _] : e.guard) add(a);
			for (size_t a : e.out) add(a);
		}
		return names;
	}

	// The outputs of absolute step `t`, keyed like the solver keys them.
	result<values> step(const value_fn& get, int_t t, const solver_fn& solve) {
		if (view) return play(get, t, solve);
		result<values> r;
		if (!ready) {
			TAU_TRY(bool chosen, choose_before(solve));
			if (!chosen) return r.with_error(code::internal_error,
				"the data game strategy found no values before step 0");
			ready = true;
		}
		window w(streams.size(), std::vector<tref>(depth + 1, nullptr));
		const auto d = direct_reads();
		for (size_t s = 0; s < streams.size(); ++s)
			for (size_t k = 0; k <= depth; ++k) {
				const int_t time = t - (int_t)k;
				if (k == 0 && !streams[s].input) continue;
				if (d && (k ? k > d->back[s] : !d->now[s])) continue;
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
			std::optional<size_t> next;
			for (size_t j = 0; j < v[at].dst.size() && !next; ++j) {
				TAU_TRY(auto holds, holds_label(at, j, w));
				if (!holds) return r.with_error(code::solver_error,
					"the data game strategy cannot read an edge label");
				if (*holds) next = follow(v[at].dst[j]);
			}
			if (!next) return r.with_error(code::internal_error,
				"the data game strategy has no edge for the inputs");
			at = *next;
		}
		values out;
		// a sink: the play is decided and no output matters
		if (v[at].picks != 1) return r.with_value(std::move(out));
		TAU_TRY(tref c, constraint(at, w, t));
		TAU_TRY(auto sol, solve(c, t));
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
			TAU_TRY(auto holds, holds_move(at, j, w));
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
	size_t at = 0;
	bool ready = false;
	size_t state_ = 0;

	// One step of the Mealy view: the values before step 0 solve the
	// view's history, the first edge whose guard the inputs meet gives
	// the atoms the outputs are solved for.
	result<values> play(const value_fn& get, int_t t,
		const solver_fn& solve)
	{
		using tau = tree<node>;
		result<values> r;
		if (!ready) {
			before.assign(streams.size(), {});
			for (size_t s = 0; s < streams.size(); ++s)
				for (size_t k = 1; k <= depth; ++k)
					before[s].push_back(tau::geth(
						build_bf_f_type<node>(streams[s].tid)));
			if (!view->history.empty()) {
				TAU_TRY(auto sol, solve(tau::build_wff_and(view->history), 0));
				if (!sol) return r.with_error(code::internal_error,
					"the data game strategy found no values before "
					"step 0");
				for (size_t s = 0; s < streams.size(); ++s)
					for (size_t k = 1; k <= depth; ++k) {
						const auto& x = streams[s];
						tref key = x.input ? build_in_var_at_n<node>(
							x.name, -(int_t)k, x.tid)
							: build_out_var_at_n<node>(x.name,
								-(int_t)k, x.tid);
						if (auto it = sol->find(key); it != sol->end())
							before[s][k - 1] = tau::geth(it->second);
					}
			}
			state_ = view->aut.initial_state;
			ready = true;
		}
		// an atom with every value of the window in place, the outputs of
		// step t left as the variables of that step
		std::string missing;
		auto ground = [&](tref f) -> tref {
			subtree_map<node, tref> m;
			for (tref x : tau::get(f).select_top(
				is_child<node, tau::io_var>))
			{
				const size_t s = index.at(get_var_name<node>(x));
				const auto& st = streams[s];
				const int_t time = t - get_io_var_shift<node>(x);
				tref val = time < 0 ? before[s][(size_t)(-time - 1)]->get()
					: time == t && !st.input ? build_out_var_at_n<node>(
						st.name, t, st.tid)
					: get(st.name, st.tid, st.input, time);
				if (!val) { missing = st.name; return nullptr; }
				m.emplace(x, tau::trim(val));
			}
			return rewriter::replace<node>(f, m);
		};
		std::map<size_t, bool> truth;
		for (const auto& e : machine[state_]) {
			bool holds = true;
			for (auto [a, sign] : e.guard) {
				auto it = truth.find(a);
				if (it == truth.end()) {
					tref g = ground(view->atoms[a].first);
					if (!g) return r.with_error(code::internal_error,
						"the data game strategy reads the unknown value "
						"of " + missing);
					auto n = normalize_non_temp<node>(g);
					if (!n.has_value() || !n.value()
						|| (!tau::get(n.value()).equals_T()
						&& !tau::get(n.value()).equals_F()))
					{
						r.merge(std::move(n));
						return r.with_error(code::solver_error,
							"the data game strategy cannot compare "
							"the values");
					}
					bool is_t = tau::get(n.value()).equals_T();
					r.append(std::move(n).report());
					it = truth.emplace(a, is_t).first;
				}
				if (it->second != (sign > 0)) { holds = false; break; }
			}
			if (!holds) continue;
			values out;
			if (!e.out.empty()) {
				trefs parts;
				for (size_t a : e.out) {
					tref g = ground(view->atoms[a].first);
					if (!g) return r.with_error(code::internal_error,
						"the data game strategy reads the unknown value "
						"of " + missing);
					parts.push_back(g);
				}
				TAU_TRY(auto sol, solve(tau::build_wff_and(parts), t));
				if (!sol) return r.with_error(code::internal_error,
					"the data game strategy has no outputs in its move");
				out = std::move(*sol);
			}
			for (size_t s = 0; s < streams.size(); ++s)
				if (!streams[s].input) {
					tref key = build_out_var_at_n<node>(streams[s].name, t,
						streams[s].tid);
					if (!out.contains(key)) out.emplace(key,
						build_bf_f_type<node>(streams[s].tid));
				}
			state_ = e.dst;
			return r.with_value(std::move(out));
		}
		return r.with_error(code::internal_error,
			"the data game strategy has no edge for the inputs");
	}
	// before[s][k-1]: the value of stream s at step -k
	std::vector<std::vector<htref>> before;

	// whether the initial vertex is won from the values of `w` before the
	// first step
	virtual result<std::optional<bool>> won_from(const window& w) = 0;
	// rebuilds the Mealy view, if any, to start from the values of `w`
	virtual result<bool> restart_view(const window& w)
		{ (void)w; return result<bool>{true}; }
	// whether the label of edge `j` of environment vertex `i` holds
	virtual result<std::optional<bool>> holds_label(size_t i, size_t j,
		const window& w) = 0;
	// whether move `j` of system vertex `i` holds
	virtual result<std::optional<bool>> holds_move(size_t i, size_t j,
		const window& w) = 0;
	// a formula over the outputs of step `t` whose solutions are the
	// moves of system vertex `i`
	virtual result<tref> constraint(size_t i, const window& w, int_t t) = 0;
	// fills `before`
	virtual result<bool> choose_before(const solver_fn& solve) = 0;

	// The slots of the window `step` reads without a view: now[s], whether
	// it reads stream s at the step played, back[s], the deepest step back
	// it ever reads s at.
	struct slot_reads { std::vector<bool> now; std::vector<size_t> back; };
	// the slots the next step reads; nullopt when it reads all of them
	virtual std::optional<slot_reads> direct_reads() const {
		return std::nullopt;
	}

	size_t follow(size_t x) const {
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
	using base::rep;

	code_window w;
	data_bdd bdd;
	std::vector<std::vector<data_bdd::id>> labels, moves;
	data_bdd::id won_init = data_bdd::F;

	code_strategy(code_window win, data_bdd b)
		: w(std::move(win)), bdd(std::move(b)) {}

	bool build_mealy(size_t max_states, size_t max_edges,
		const std::vector<int>* from = nullptr);

protected:
	using base::before;

	result<std::optional<bool>> won_from(const window& win) override {
		result<std::optional<bool>> r;
		TAU_TRY(auto bits, encode(win));
		if (!bits) return r.with_value(std::nullopt);
		return r.with_value(eval(won_init, *bits));
	}

	result<bool> restart_view(const window& win) override {
		result<bool> r;
		if (!this->view) return r.with_value(true);
		this->view = nullptr;
		this->machine.clear();
		TAU_TRY(auto bits, encode(win));
		if (bits) {
			for (auto& b : *bits) if (b < 0) b = 0;
			build_mealy(data_game_mealy_max_states,
				data_game_mealy_max_edges, &*bits);
		}
		return r.with_value(true);
	}

	// Whether two values are equal; nullopt when undecided.
	std::optional<bool> same(tref x, tref y) {
		if (tau::subtree_equals(x, y)) return true;
		auto n = normalize_non_temp<node>(tau::build_bf_eq(x, y));
		if (!n.has_value() || !n.value()) {
			rep.append(std::move(n).report());
			return std::nullopt;
		}
		const auto& t = tau::get(n.value());
		std::optional<bool> out;
		if (t.equals_T()) out = true;
		else if (t.equals_F()) out = false;
		rep.append(std::move(n).report());
		return out;
	}

	static result<tref> complement(tref x) {
		return normalize_ba<node>(tau::build_bf_neg(x));
	}

	// The value of point p of the order relations in `win`, nullptr when
	// unknown.
	tref point_value(size_t p, const window& win) const {
		const auto& x = w.points[p];
		if (!x.slot()) return x.constant->get();
		return x.k < win[x.s].size() ? win[x.s][x.k] : nullptr;
	}

	// The bits of the known values of `win`, -1 for an unknown one. A
	// value of a coded stream gets code 0 when it is 0, 1 when it is 1,
	// and otherwise the code of the first equal value met or a new one; a
	// value of an orbit-coded stream gets the pair of the first value met
	// it equals or complements, or a new one. A relation of two known
	// points of an order gets their order.
	result<std::optional<std::vector<int>>> encode(const window& win) {
		result<std::optional<std::vector<int>>> r;
		// `same` answers through a value; its report is merged here.
		auto drop = [&]() { r.append(std::move(rep)); rep.clear(); };
		std::vector<int> bits(w.vars(), -1);
		std::map<size_t, std::vector<std::pair<tref, size_t>>> seen;
		for (size_t k = 0; k < win[0].size(); ++k)
			for (size_t s = 0; s < streams.size(); ++s) {
				tref x = win[s][k];
				if (!x) continue;
				const size_t tid = streams[s].tid;
				const auto& ws = w.streams[s];
				size_t c;
				if (ws.order) continue;
				if (ws.modular) {
					auto v = pack_modular_value<node>(tid, x);
					if (!v) return r.with_value(std::nullopt);
					for (size_t b = 0; b < ws.width; ++b)
						bits[w.var(s, k, b)] = (int)(*v >> b & 1);
					continue;
				}
				if (!ws.values.empty()) {
					c = ws.values.size();
					for (size_t j = 0; j < ws.values.size(); ++j) {
						auto eq = same(x, ws.values[j]->get());
						if (!eq) { drop(); return r.with_value(std::nullopt); }
						if (*eq) { c = j; break; }
					}
					if (c == ws.values.size()) return r.with_value(std::nullopt);
					for (size_t b = 0; b < ws.width; ++b)
						bits[w.var(s, k, b)] = (int)(c >> b & 1);
					continue;
				}
				auto one = same(x, build_bf_t_type<node>(tid));
				if (!one) { drop(); return r.with_value(std::nullopt); }
				if (ws.two) c = *one ? 1 : 0;
				else if (*one) c = ws.orbit ? size_t{1} << (ws.width - 1) : 1;
				else {
					auto zero = same(x, build_bf_f_type<node>(tid));
					if (!zero) { drop(); return r.with_value(std::nullopt); }
					if (*zero) c = 0;
					else {
						auto& reps = seen[tid];
						const size_t fresh = ws.orbit ? reps.size() + 1
							: reps.size() + 2;
						c = fresh;
						for (auto& [y, cy] : reps) {
							auto eq = same(x, y);
							if (!eq) { drop(); return r.with_value(std::nullopt); }
							if (*eq) { c = cy; break; }
							if (!ws.orbit) continue;
							TAU_TRY(tref neg_y, complement(y));
							auto neq = same(x, neg_y);
							if (!neq) { drop(); return r.with_value(std::nullopt); }
							if (*neq) {
								c = cy | size_t{1} << (ws.width - 1);
								break;
							}
						}
						if (c == fresh) reps.emplace_back(x, c);
					}
				}
				for (size_t b = 0; b < ws.width; ++b)
					bits[w.var(s, k, b)] = (int)(c >> b & 1);
			}
		for (size_t i = 0; i < w.rel_points.size(); ++i) {
			auto [p, q] = w.rel_points[i];
			tref x = point_value(p, win), y = point_value(q, win);
			if (!x || !y) continue;
			auto c = pack_dense_order_compare<node>(w.points[p].tid, x, y);
			if (!c) return r.with_value(std::nullopt);
			bits[w.code_vars() + 2 * i] = *c < 0;
			bits[w.code_vars() + 2 * i + 1] = *c == 0;
		}
		return r.with_value(std::move(bits));
	}

	// The relations that extend the order of the points `win` knows to the
	// order slots of `slots`, with them as weak orders. A point of neither
	// stays out: the relations to it would only grow the BDD.
	data_bdd::id order_extensions(const std::vector<int>& bits,
		const window& win,
		const std::vector<std::tuple<size_t, size_t, tref>>& slots)
	{
		data_bdd::id r = data_bdd::T;
		std::map<size_t, std::vector<size_t>> by_type;
		std::map<size_t, std::vector<bool>> flagged;
		for (size_t p = 0; p < w.points.size(); ++p) {
			const auto& x = w.points[p];
			const bool mine = x.slot() && std::any_of(slots.begin(),
				slots.end(), [&](const auto& sl) {
					return std::get<0>(sl) == x.s
						&& std::get<1>(sl) == x.k; });
			if (!mine && !point_value(p, win)) continue;
			by_type[x.tid].push_back(p);
			flagged[x.tid].push_back(mine);
		}
		for (auto& [tid, pts] : by_type) {
			const std::vector<bool>& in_x = flagged[tid];
			const bool any = std::find(in_x.begin(), in_x.end(), true)
				!= in_x.end();
			if (any) r = bdd.conj(r, order_consistent(w, bdd, pts, in_x,
				&bits));
		}
		return r;
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

	// Completes the unknown bits of `bits` along a path of `n` to true. A
	// search that meets an unknown variable `stop` flags ends there, false,
	// with that variable in `*met`.
	bool pick(data_bdd::id n, std::vector<int>& bits,
		const std::vector<bool>* stop = nullptr, uint32_t* met = nullptr)
	{
		std::set<data_bdd::id> dead;
		bool ended = false;
		std::function<bool(data_bdd::id)> go = [&](data_bdd::id m) {
			if (m <= data_bdd::T) return m == data_bdd::T;
			if (dead.contains(m)) return false;
			const auto x = bdd.nodes[m];
			if (bits[x.var] >= 0) return go(bits[x.var] ? x.hi : x.lo);
			if (stop && (*stop)[x.var]) {
				*met = x.var;
				ended = true;
				return false;
			}
			for (int b : { 0, 1 }) {
				bits[x.var] = b;
				if (go(b ? x.hi : x.lo)) return true;
				if (ended) return false;
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
	result<tref> decode(const std::vector<int>& bits, const window& win,
		const std::vector<std::tuple<size_t, size_t, tref>>& slots)
	{
		result<tref> r;
		tref f = tau::_T();
		// the value decoded for each slot of an order stream
		std::vector<tref> placed(slots.size(), nullptr);
		for (size_t i = 0; i < slots.size(); ++i) {
			auto [s, k, x] = slots[i];
			const size_t tid = streams[s].tid;
			const auto& ws = w.streams[s];
			const size_t c = code_of(bits, s, k);
			// an orbit code: the pair and the value in it
			const size_t side = ws.orbit ? c >> (ws.width - 1) : 0;
			const size_t pair = ws.orbit
				? c & ((size_t{1} << (ws.width - 1)) - 1) : c;
			auto as = [&](tref y, size_t other) -> result<tref> {
				result<tref> ar;
				if (!ws.orbit || side == other)
					return ar.with_value(y);
				return complement(y);
			};
			if (ws.order) {
				// the point the relations to every known point and earlier
				// slot place it at: equal to one, else between the
				// greatest below it and the least above it
				const size_t me = w.slot_point(s, k);
				auto cmp = [&](tref a, tref b) {
					return pack_dense_order_compare<node>(tid, a, b)
						.value_or(0);
				};
				tref same = nullptr, lo = nullptr, hi = nullptr;
				for (size_t p = 0; p < w.points.size(); ++p) {
					if (p == me || w.points[p].tid != tid) continue;
					tref y = point_value(p, win);
					for (size_t i2 = 0; i2 < i && !y; ++i2)
						if (w.slot_point(std::get<0>(slots[i2]),
							std::get<1>(slots[i2])) == p)
								y = placed[i2];
					auto rel = w.relation(me, p);
					if (!y || !rel) continue;
					auto [v, flip] = *rel;
					int r = bits[v] > 0 ? -1 : bits[v + 1] > 0 ? 0 : 1;
					if (flip) r = -r;
					if (r == 0) same = y;
					else if (r < 0) { if (!hi || cmp(y, hi) < 0) hi = y; }
					else if (!lo || cmp(y, lo) > 0) lo = y;
				}
				placed[i] = same ? same
					: pack_dense_order_between<node>(tid, lo, hi);
				f = tau::build_wff_and(f, placed[i]
					? tau::build_bf_eq(x, placed[i]) : tau::_F());
				continue;
			}
			if (ws.modular) {
				tref e = pack_value_constant<node>(tid, c);
				f = tau::build_wff_and(f, e ? tau::build_bf_eq(x, e)
					: tau::_F());
				continue;
			}
			if (!ws.values.empty()) {
				f = tau::build_wff_and(f, tau::build_bf_eq(x,
					ws.values[c]->get()));
				continue;
			}
			if (ws.two || (ws.orbit ? pair == 0 : c < 2)) {
				f = tau::build_wff_and(f, tau::build_bf_eq(x,
					(ws.orbit ? side : c) ? build_bf_t_type<node>(tid)
						: build_bf_f_type<node>(tid)));
				continue;
			}
			auto same_pair = [&](size_t c2) {
				return ws.orbit ? (c2 & ((size_t{1} << (ws.width - 1)) - 1))
					== pair : c2 == c;
			};
			auto side_of = [&](size_t c2) {
				return ws.orbit ? c2 >> (ws.width - 1) : 0;
			};
			tref known = nullptr;
			for (size_t s2 = 0; s2 < streams.size() && !known; ++s2)
				if (streams[s2].tid == tid && !w.streams[s2].two)
					for (size_t k2 = 0; k2 < win[s2].size(); ++k2)
						if (win[s2][k2] && same_pair(code_of(bits, s2, k2))) {
							TAU_TRY(tref known_v, as(win[s2][k2],
								side_of(code_of(bits, s2, k2))));
							known = known_v;
							break;
						}
			if (known) {
				f = tau::build_wff_and(f, tau::build_bf_eq(x, known));
				continue;
			}
			// a value none of the window holds, nor its complement
			f = tau::build_wff_and(f, tau::build_wff_and(
				tau::build_bf_neq(x, build_bf_f_type<node>(tid)),
				tau::build_bf_neq(x, build_bf_t_type<node>(tid))));
			for (size_t s2 = 0; s2 < streams.size(); ++s2)
				if (streams[s2].tid == tid)
					for (tref y : win[s2]) if (y) {
						f = tau::build_wff_and(f, tau::build_bf_neq(x, y));
						if (ws.orbit) {
							TAU_TRY(tref cy, complement(y));
							f = tau::build_wff_and(f,
								tau::build_bf_neq(x, cy));
						}
					}
			for (size_t i2 = 0; i2 < i; ++i2) {
				auto [s2, k2, y] = slots[i2];
				if (streams[s2].tid != tid || w.streams[s2].two) continue;
				const size_t c2 = code_of(bits, s2, k2);
				if (same_pair(c2))
					f = tau::build_wff_and(f, tau::build_bf_eq(x,
						side_of(c2) == side ? y : tau::build_bf_neg(y)));
				else {
					f = tau::build_wff_and(f, tau::build_bf_neq(x, y));
					if (ws.orbit) f = tau::build_wff_and(f,
						tau::build_bf_neq(x, tau::build_bf_neg(y)));
				}
			}
		}
		return r.with_value(f);
	}

	result<std::optional<bool>> holds_label(size_t i, size_t j, const window& win)
		override
	{
		result<std::optional<bool>> r;
		TAU_TRY(auto bits, encode(win));
		if (!bits) return r.with_value(std::nullopt);
		return r.with_value(eval(labels[i][j], *bits));
	}

	result<std::optional<bool>> holds_move(size_t i, size_t j, const window& win)
		override
	{
		result<std::optional<bool>> r;
		TAU_TRY(auto bits, encode(win));
		if (!bits) return r.with_value(std::nullopt);
		return r.with_value(eval(moves[i][j], *bits));
	}

	result<tref> constraint(size_t i, const window& win, int_t t) override {
		result<tref> r;
		TAU_TRY(auto bits, encode(win));
		if (!bits) return r.with_error(code::solver_error,
			"the data game strategy cannot compare the values");
		std::vector<std::tuple<size_t, size_t, tref>> slots;
		for (size_t s = 0; s < streams.size(); ++s)
			if (!streams[s].input)
				slots.emplace_back(s, 0, build_out_var_at_n<node>(
					streams[s].name, t, streams[s].tid));
		data_bdd::id any = data_bdd::F;
		for (auto m : moves[i]) any = bdd.disj(any, m);
		any = bdd.conj(any, order_extensions(*bits, win, slots));
		if (bdd.full || !pick(any, *bits))
			return r.with_error(code::internal_error,
				"the data game strategy has no move from the history");
		TAU_TRY(tref d, decode(*bits, win, slots));
		return r.with_value(d);
	}

	result<bool> choose_before(const solver_fn& solve) override {
		result<bool> r;
		const size_t n = streams.size(), d = w.depth;
		window win(n, std::vector<tref>(d + 1, nullptr));
		for (size_t s = 0; s < n; ++s)
			for (size_t k = 1; k <= d; ++k)
				if (streams[s].input) {
					// an order has no 0 below its points, but a zero
					// constant among them
					tref zero = w.streams[s].order
						? pack_zero_constant<node>(streams[s].tid) : nullptr;
					win[s][k] = zero ? zero
						: build_bf_f_type<node>(streams[s].tid);
				}
		std::vector<std::tuple<size_t, size_t, tref>> slots;
		for (size_t s = 0; s < n; ++s)
			for (size_t k = 1; k <= d; ++k)
				if (!streams[s].input)
					slots.emplace_back(s, k,
						this->before_var(s, k, streams[s].tid));
		TAU_TRY(auto known, encode(win));
		if (!known) return r.with_error(code::solver_error,
			"the data game strategy cannot compare the values");
		std::vector<int> bits = std::move(*known);
		if (!pick(bdd.conj(won_init, order_extensions(bits, win, slots)),
			bits)) return r.with_value(false);
		values sol;
		if (!slots.empty()) {
			TAU_TRY(tref win_bits, decode(bits, win, slots));
			TAU_TRY(auto got, solve(win_bits, 0));
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

// ── The Mealy view of a code_strategy ────────────────────────────────────────
//
// A state is a game vertex where a step starts and what the last `depth`
// values of every stream hold for the steps to come: the codes of the
// streams read through equalities, renamed canonically (the regions do not
// tell apart two codes of distinct values, nor the two values of a pair),
// the bits of a modular stream that a label or a move reads at that step
// back or deeper, and how the slots of the order streams and the constants
// compare. A step reads the inputs through atoms: an input read through
// equalities is compared with 0, 1, the values of its type, the window and
// the inputs before it; a modular input is read bit by bit, `(x & 2^b) !=
// 0`, only as far as the step's labels and moves and the next state read
// its bits; an order input by where it falls among the window's slots, the
// constants and the order inputs before it (`x < p`, `p < x` or `x = p`,
// p standing for its class of equal points). The outputs the strategy
// picks are a cube over the same terms: equalities, `x = c` for a modular
// output, and for an order output the point it equals, else the points
// right below and above it. A slot of the window that no label or move
// reads is left out of the state, and an input read by none of the step's
// labels and moves nor ever from the window is not read at all. The
// machine found is minimized, and a state's guards drop the inputs its
// moves do not depend on. It becomes `view`, a solution over those atoms,
// read at the absolute step played, with `history` giving the values
// before step 0; false, and no view, when it exceeds `max_states` or
// `max_edges`.
template <NodeType node>
bool code_strategy<node>::build_mealy(size_t max_states, size_t max_edges,
	const std::vector<int>* from)
{
	using tau = tree<node>;
	// The view's nodes take at most half the room left, and a table the
	// view fills is usable again afterwards: the moves are then played
	// without a view, on the same table.
	struct room_guard {
		data_bdd& b;
		const bool was_full;
		const size_t limit;
		~room_guard() { b.limit = limit; if (!was_full) b.full = false; }
	} guard{ bdd, bdd.full, bdd.limit };
	bdd.grow_at_most(bdd.room() / 2);
	const size_t S = streams.size(), d = w.depth;
	const uint32_t V = w.vars();
	const auto& vs = this->v;
	// a modular stream coded by its bits, with no element table
	auto blasted = [&](size_t s) {
		return w.streams[s].modular && w.streams[s].values.empty();
	};
	auto ordered = [&](size_t s) { return w.streams[s].order; };
	// The slots (stream, step back) variable x reads: the slot of a bit,
	// the slots among the two points of a relation.
	auto slots_of = [&](uint32_t x) {
		std::vector<std::pair<size_t, size_t>> out;
		if (x < w.code_vars()) {
			const size_t at = x % w.block();
			out.emplace_back(at % S, at / S);
		} else {
			auto [p, q] = w.rel_points[(x - w.code_vars()) / 2];
			for (size_t r : { p, q })
				if (w.points[r].slot())
					out.emplace_back(w.points[r].s, w.points[r].k);
		}
		return out;
	};
	// reach[s]: the deepest step back at which a label or a move reads s;
	// read0[s]: whether a move reads s at the current step; read[x]:
	// whether a label or a move reads variable x
	std::vector<size_t> reach(S, 0);
	std::vector<bool> read0(S, false), read(V, false);
	{
		std::set<data_bdd::id> seen;
		std::function<void(data_bdd::id)> scan = [&](data_bdd::id n) {
			if (n <= data_bdd::T || !seen.insert(n).second) return;
			const uint32_t x = bdd.nodes[n].var;
			read[x] = true;
			for (auto [s, k] : slots_of(x))
				if (k) reach[s] = std::max(reach[s], k);
				else read0[s] = true;
			scan(bdd.nodes[n].lo);
			scan(bdd.nodes[n].hi);
		};
		for (auto& ls : labels) for (auto l : ls) scan(l);
		for (auto& ms : moves) for (auto m : ms) scan(m);
	}
	auto kept = [&](size_t s, size_t k) { return k >= 1 && k <= reach[s]; };
	// later[x]: bit x of a modular stream, at step back k >= 1, is read at
	// that step back or deeper
	std::vector<bool> later(V, false);
	for (size_t s = 0; s < S; ++s)
		if (blasted(s))
			for (size_t b = 0; b < w.streams[s].width; ++b)
				for (size_t k = d; k >= 1; --k)
					later[w.var(s, k, b)] = read[w.var(s, k, b)]
						|| (k < d && later[w.var(s, k + 1, b)]);
	auto set_code = [&](std::vector<int>& bits, size_t s, size_t k,
		size_t c)
	{
		for (size_t b = 0; b < w.streams[s].width; ++b)
			bits[w.var(s, k, b)] = (int)(c >> b & 1);
	};
	const auto pair_mask = [&](size_t s) {
		return (size_t{1} << (w.streams[s].width - 1)) - 1;
	};
	// whether two orbit codes stand for complementary values
	auto complements = [&](size_t s, size_t c1, size_t c2) {
		return w.streams[s].orbit && c1 != c2
			&& (c1 & pair_mask(s)) == (c2 & pair_mask(s));
	};
	auto coded = [&](size_t s) {   // an equality or an orbit code
		return !w.streams[s].finite() && !w.streams[s].order;
	};
	// the value a code stands for when it is 0 or 1, else -1
	auto constant = [&](size_t s, size_t c) -> int {
		const auto& x = w.streams[s];
		if (x.two) return (int)c;
		if (!x.values.empty()) return -1;
		if (x.orbit) return (c & pair_mask(s)) ? -1
			: (int)(c >> (x.width - 1));
		return c < 2 ? (int)c : -1;
	};
	// A point of the order relations the window keeps: a constant, or a
	// slot the state holds.
	auto held = [&](size_t p) {
		const auto& x = w.points[p];
		return !x.slot() || kept(x.s, x.k);
	};
	// how point p compares with point q in `bits`: -1 below, 0 equal, 1
	// above
	auto rel_get = [&](const std::vector<int>& bits, size_t p, size_t q) {
		if (p == q) return 0;
		if (auto rel = w.relation(p, q)) {
			auto [v, flip] = *rel;
			const int r = bits[v] > 0 ? -1 : bits[v + 1] > 0 ? 0 : 1;
			return flip ? -r : r;
		}
		auto it = w.fixed.find({ p, q });
		return it != w.fixed.end() ? it->second : 0;
	};
	auto rel_set = [&](std::vector<int>& bits, size_t p, size_t q, int r) {
		if (auto rel = w.relation(p, q)) {
			auto [v, flip] = *rel;
			if (flip) r = -r;
			bits[v] = r < 0;
			bits[v + 1] = r == 0;
		}
	};
	// the points of type `tid` the window keeps
	auto held_points = [&](size_t tid) {
		std::vector<size_t> out;
		for (size_t p = 0; p < w.points.size(); ++p)
			if (w.points[p].tid == tid && held(p)) out.push_back(p);
		return out;
	};
	// `pts` in classes of points equal in `bits`, the lowest first
	auto classes = [&](const std::vector<int>& bits,
		const std::vector<size_t>& pts)
	{
		std::vector<std::vector<size_t>> cs;
		for (size_t p : pts) {
			size_t i = 0;
			while (i < cs.size() && rel_get(bits, cs[i][0], p) < 0) ++i;
			if (i < cs.size() && rel_get(bits, cs[i][0], p) == 0)
				cs[i].push_back(p);
			else cs.insert(cs.begin() + (std::ptrdiff_t)i,
				std::vector<size_t>{ p });
		}
		return cs;
	};
	// Renames the codes of the window's slots 1 .. d canonically, keeps
	// the bits and relations the steps to come read, and clears every
	// other variable.
	auto canonical = [&](const std::vector<int>& bits) {
		std::vector<int> out(V, 0);
		std::map<size_t, std::map<size_t, std::pair<size_t, size_t>>> ren;
		std::map<size_t, size_t> next;
		for (size_t k = 1; k <= d; ++k)
			for (size_t s = 0; s < S; ++s) {
				if (!kept(s, k) || ordered(s)) continue;
				const auto& x = w.streams[s];
				size_t c = code_of(bits, s, k);
				if (coded(s) && constant(s, c) < 0) {
					const size_t tid = streams[s].tid;
					const size_t p = x.orbit ? c & pair_mask(s) : c;
					const size_t side = x.orbit ? c >> (x.width - 1) : 0;
					auto& m = ren[tid];
					auto it = m.find(p);
					if (it == m.end()) {
						auto [nx, _] = next.emplace(tid, x.orbit ? 1 : 2);
						it = m.emplace(p, std::pair{ nx->second++, side })
							.first;
					}
					c = it->second.first;
					if (x.orbit) c |= (side ^ it->second.second)
						<< (x.width - 1);
				}
				set_code(out, s, k, c);
				if (blasted(s))
					for (size_t b = 0; b < x.width; ++b)
						if (!later[w.var(s, k, b)]) out[w.var(s, k, b)] = 0;
			}
		for (size_t i = 0; i < w.rel_points.size(); ++i) {
			auto [p, q] = w.rel_points[i];
			if (!held(p) || !held(q)) continue;
			const uint32_t v = w.code_vars() + 2 * (uint32_t)i;
			out[v] = bits[v] > 0;
			out[v + 1] = bits[v + 1] > 0;
		}
		return out;
	};
	auto term = [&](size_t s, size_t k) -> tref {
		const auto& x = streams[s];
		if (k == 0) return x.input
			? tau::build_in_var_at_t(build_var_name<node>(x.name), x.tid)
			: tau::build_out_var_at_t(build_var_name<node>(x.name), x.tid);
		return x.input ? tau::build_in_var_at_t_minus(x.name, k, x.tid)
			: tau::build_out_var_at_t_minus(x.name, k, x.tid);
	};
	auto zero = [&](size_t s) { return build_bf_f_type<node>(streams[s].tid); };
	auto one = [&](size_t s) { return build_bf_t_type<node>(streams[s].tid); };

	// The atoms, each kept once: the current value of stream s compared,
	// equal or not, with a constant 0 or 1 (kind 0), an element of its type
	// (1), a slot (s2, k2) (2) or its complement (3); bit `val` of s set
	// (4); s the value `val` of its modular type (5); s below (6), above
	// (7) or equal to (8) a slot (s2, k2) or, with s2 = SIZE_MAX, the
	// constant point `val`; s the zero constant of its order (9).
	using akey = std::tuple<bool, int, size_t, size_t, size_t, size_t>;
	std::map<akey, size_t> atom_index;
	std::vector<akey> atom_key;
	std::vector<std::pair<tref, std::string>> atoms;
	std::vector<bool> atom_input;
	// a constant of a modular or an order type the type does not give
	bool missing = false;
	auto atom = [&](bool eq, int kind, size_t s, size_t s2, size_t k2,
		size_t val) -> size_t
	{
		akey key{ eq, kind, s, s2, k2, val };
		if (auto it = atom_index.find(key); it != atom_index.end())
			return it->second;
		const size_t tid = streams[s].tid;
		tref x = term(s, 0), a;
		if (kind <= 3) {
			tref other = kind == 0 ? (val ? one(s) : zero(s))
				: kind == 1 ? w.streams[s].values[val]->get()
				: kind == 2 ? term(s2, k2)
				: tau::build_bf_neg(term(s2, k2));
			a = eq ? tau::build_bf_eq(x, other) : tau::build_bf_neq(x, other);
		} else if (kind == 4 || kind == 5 || kind == 9) {
			tref c = kind == 9 ? pack_zero_constant<node>(tid)
				: pack_value_constant<node>(tid,
					kind == 4 ? size_t{1} << val : val);
			if (!c) missing = true, c = zero(s);
			a = kind == 4 ? tau::build_bf_neq(tau::build_bf_and(x, c),
				zero(s)) : tau::build_bf_eq(x, c);
		} else {
			tref p = s2 == SIZE_MAX ? w.points[val].constant->get()
				: term(s2, k2);
			a = kind == 6 ? tau::build_bf_lt(x, p)
				: kind == 7 ? tau::build_bf_lt(p, x)
				: tau::build_bf_eq(x, p);
		}
		atom_index.emplace(key, atoms.size());
		atom_key.push_back(key);
		atoms.emplace_back(a, "p" + std::to_string(atoms.size()));
		atom_input.push_back(streams[s].input);
		return atoms.size() - 1;
	};
	// the atom of `kind` (6, 7 or 8) comparing stream s with point p
	auto point_atom = [&](int kind, size_t s, size_t p) {
		const auto& x = w.points[p];
		return x.slot() ? atom(true, kind, s, x.s, x.k, 0)
			: atom(true, kind, s, SIZE_MAX, 0, p);
	};
	// The slots an input or an output of the step is compared with: the
	// window's kept slots, then the current values met before it.
	auto partners = [&](size_t s, const std::vector<bool>& current) {
		std::vector<std::pair<size_t, size_t>> ps;
		const size_t tid = streams[s].tid;
		for (size_t k = 1; k <= d; ++k)
			for (size_t s2 = 0; s2 < S; ++s2)
				if (kept(s2, k) && streams[s2].tid == tid && coded(s2))
					ps.emplace_back(s2, k);
		for (size_t s2 = 0; s2 < S; ++s2)
			if (current[s2] && streams[s2].tid == tid && coded(s2)
				&& (streams[s2].input != streams[s].input || s2 < s))
					ps.emplace_back(s2, 0);
		return ps;
	};
	// The moves of system vertex u, with the relations of the outputs of
	// order streams to the points `ins` and the window know kept to weak
	// orders, those points placed as `known` places them.
	std::map<size_t, data_bdd::id> any_of;
	auto any_move = [&](size_t u, const std::vector<size_t>& ins,
		const std::vector<int>& known)
	{
		auto [it, fresh] = any_of.emplace(u, data_bdd::F);
		if (fresh)
			for (auto m : moves[u]) it->second = bdd.disj(it->second, m);
		data_bdd::id any = it->second;
		std::map<size_t, std::pair<std::vector<size_t>, std::vector<bool>>>
			by_type;
		for (size_t s = 0; s < S; ++s)
			if (ordered(s) && !streams[s].input) {
				auto& [pts, in_x] = by_type[streams[s].tid];
				pts.push_back(w.slot_point(s, 0));
				in_x.push_back(true);
			}
		for (auto& [tid, pi] : by_type) {
			auto& [pts, in_x] = pi;
			for (size_t p : held_points(tid))
				pts.push_back(p), in_x.push_back(false);
			for (size_t s : ins)
				if (ordered(s) && streams[s].tid == tid)
					pts.push_back(w.slot_point(s, 0)), in_x.push_back(false);
			any = bdd.conj(any, order_consistent(w, bdd, pts, in_x, &known));
		}
		return any;
	};

	struct medge {
		std::map<size_t, int8_t> guard;  // input atom -> 1 or -1
		std::vector<size_t> out;         // output atoms, all positive
		size_t dst = 0;
	};
	using skey = std::pair<size_t, std::vector<int>>;
	std::map<skey, size_t> state_of;
	std::vector<skey> states;
	std::vector<std::vector<medge>> edges;
	size_t edge_count = 0;
	auto state = [&](size_t vx, std::vector<int> bits) {
		if (vs[vx].picks != 0) bits.assign(V, 0);
		skey key{ vx, std::move(bits) };
		auto [it, fresh] = state_of.emplace(key, states.size());
		if (fresh) states.push_back(it->first), edges.emplace_back();
		return it->second;
	};
	// the start: the codes `from` gives, else inputs 0 before step 0,
	// outputs and the order of the kept slots as won_init allows
	std::vector<int> start(V, 0);
	data_bdd::id won = won_init;
	if (from) start = *from;
	else if (w.has_order()) {
		// as the moves played without a view start: the inputs 0 (an
		// order's zero constant), the outputs as won_init allows, over the
		// relations of the points these values place
		window win(S, std::vector<tref>(d + 1, nullptr));
		std::vector<std::tuple<size_t, size_t, tref>> slots;
		for (size_t s = 0; s < S; ++s)
			for (size_t k = 1; k <= d; ++k)
				if (!streams[s].input) slots.emplace_back(s, k,
					this->before_var(s, k, streams[s].tid));
				else if (tref z = ordered(s)
					? pack_zero_constant<node>(streams[s].tid) : nullptr)
						win[s][k] = z;
				else win[s][k] = build_bf_f_type<node>(streams[s].tid);
		auto known = encode(win);
		const bool have = known.has_value() && known.value();
		if (have) start = *known.value();
		rep.append(std::move(known).report());
		if (!have) return false;
		won = bdd.conj(won_init, order_extensions(start, win, slots));
	} else {
		for (size_t s = 0; s < S; ++s)
			if (!streams[s].input)
				for (size_t k = 1; k <= d; ++k)
					if (kept(s, k))
						for (size_t b = 0; b < w.streams[s].width; ++b)
							start[w.var(s, k, b)] = -1;
	}
	if (bdd.full || !pick(won, start)) return false;
	const size_t first = state(this->follow(this->init), canonical(start));

	for (size_t q = 0; q < states.size(); ++q) {
		if (states.size() > max_states || edge_count > max_edges)
			return false;
		const size_t vx = states[q].first;
		const std::vector<int> cbits = states[q].second;
		if (vs[vx].picks != 0) {
			// a sink: the play is decided
			edges[q].push_back({ {}, {}, q });
			++edge_count;
			continue;
		}
		// the inputs this step reads
		std::vector<bool> need(S, false);
		{
			std::set<data_bdd::id> seen;
			std::function<void(data_bdd::id)> scan = [&](data_bdd::id n) {
				if (n <= data_bdd::T || !seen.insert(n).second) return;
				const uint32_t x = bdd.nodes[n].var;
				bool now = false;
				for (auto [s, k] : slots_of(x))
					if (!k) {
						now = true;
						if (streams[s].input) need[s] = true;
					}
				if (!now) { scan(cbits[x] ? bdd.nodes[n].hi
					: bdd.nodes[n].lo); return; }
				scan(bdd.nodes[n].lo);
				scan(bdd.nodes[n].hi);
			};
			for (size_t j = 0; j < vs[vx].dst.size(); ++j) {
				scan(labels[vx][j]);
				const size_t u = this->follow(vs[vx].dst[j]);
				if (vs[u].picks == 1) for (auto m : moves[u]) scan(m);
			}
			for (size_t s = 0; s < S; ++s)
				if (streams[s].input && reach[s]) need[s] = true;
		}
		std::vector<size_t> ins;
		for (size_t s = 0; s < S; ++s) if (need[s]) ins.push_back(s);
		// the bits of modular inputs, split only as far as the step reads
		// them
		std::vector<bool> open(V, false);
		std::vector<int> bits = cbits;
		for (size_t s : ins)
			if (blasted(s))
				for (size_t b = 0; b < w.streams[s].width; ++b) {
					open[w.var(s, 0, b)] = true;
					bits[w.var(s, 0, b)] = -1;
				}
		// per order input, the atoms placing it among the points before it
		std::vector<std::vector<size_t>> placed(S);
		// One edge for the codes in `bits`; an open bit the edge reads
		// is split on first.
		std::function<bool()> leaf = [&]() -> bool {
			uint32_t split = data_bdd::leaf;
			auto ev = [&](data_bdd::id n, const std::vector<int>& b)
				-> std::optional<bool>
			{
				while (n > data_bdd::T) {
					const auto& x = bdd.nodes[n];
					if (b[x.var] < 0) {
						if (open[x.var]) split = x.var;
						return std::nullopt;
					}
					n = b[x.var] ? x.hi : x.lo;
				}
				return n == data_bdd::T;
			};
			auto branch = [&]() {
				const uint32_t u = split;
				for (int b : { 0, 1 }) {
					bits[u] = b;
					if (!leaf()) return false;
				}
				bits[u] = -1;
				return true;
			};
			size_t next = SIZE_MAX;
			for (size_t j = 0; j < vs[vx].dst.size() && next == SIZE_MAX;
				++j)
			{
				auto holds = ev(labels[vx][j], bits);
				if (split != data_bdd::leaf) return branch();
				if (holds.value_or(false))
					next = this->follow(vs[vx].dst[j]);
			}
			if (next == SIZE_MAX || vs[next].picks == 0) return false;
			std::vector<int> full = bits;
			size_t after = SIZE_MAX;
			if (vs[next].picks == 1) {
				for (size_t s = 0; s < S; ++s)
					if (!streams[s].input)
						for (size_t b = 0; b < w.streams[s].width; ++b)
							full[w.var(s, 0, b)] = -1;
				for (size_t i = 0; i < w.rel_points.size(); ++i) {
					auto [p, p2] = w.rel_points[i];
					for (size_t r : { p, p2 }) {
						const auto& x = w.points[r];
						if (x.slot() && x.k == 0 && !streams[x.s].input)
							full[w.code_vars() + 2 * i]
								= full[w.code_vars() + 2 * i + 1] = -1;
					}
				}
				data_bdd::id any = any_move(next, ins, full);
				if (bdd.full) return false;
				if (!pick(any, full, &open, &split)) {
					if (split != data_bdd::leaf) return branch();
					return false;
				}
				for (uint32_t x = 0; x < V; ++x)
					if (open[x] && bits[x] < 0) full[x] = -1;
				for (size_t j = 0; j < vs[next].dst.size() && after == SIZE_MAX;
					++j)
				{
					auto holds = ev(moves[next][j], full);
					if (split != data_bdd::leaf) return branch();
					if (holds.value_or(false))
						after = this->follow(vs[next].dst[j]);
				}
				if (after == SIZE_MAX || (vs[after].picks != 0
					&& vs[after].picks != -1)) return false;
				// the open bits the next state keeps
				if (d >= 1)
					for (uint32_t x = 0; x < V; ++x)
						if (open[x] && bits[x] < 0 && later[x + S]) {
							split = x;
							return branch();
						}
			}
			medge e;
			// the guard: every atom of an input read
			for (size_t s : ins) {
				const auto& x = w.streams[s];
				const size_t c = code_of(bits, s, 0);
				if (blasted(s)) {
					for (size_t b = 0; b < x.width; ++b)
						if (bits[w.var(s, 0, b)] >= 0)
							e.guard[atom(true, 4, s, 0, 0, b)]
								= bits[w.var(s, 0, b)] ? 1 : -1;
				} else if (x.order)
					for (size_t a : placed[s]) e.guard[a] = 1;
				else if (x.two) e.guard[atom(true, 0, s, 0, 0, 1)]
					= c ? 1 : -1;
				else if (!x.values.empty())
					for (size_t val = 0; val < x.values.size(); ++val)
						e.guard[atom(true, 1, s, 0, 0, val)]
							= c == val ? 1 : -1;
				else {
					for (size_t val = 0; val < 2; ++val)
						e.guard[atom(true, 0, s, 0, 0, val)]
							= constant(s, c) == (int)val ? 1 : -1;
					for (auto [s2, k2] : partners(s, need)) {
						const size_t c2 = code_of(bits, s2, k2);
						e.guard[atom(true, 2, s, s2, k2, 0)]
							= c == c2 ? 1 : -1;
						if (x.orbit) e.guard[atom(true, 3, s, s2,
							k2, 0)] = complements(s, c, c2) ? 1 : -1;
					}
				}
			}
			if (vs[next].picks != 1) {
				e.dst = state(next, {});
				edges[q].push_back(std::move(e));
				return ++edge_count <= max_edges;
			}
			// the outputs: a cube of equalities, or of order comparisons
			std::vector<bool> current = need;
			for (size_t s = 0; s < S; ++s) {
				if (streams[s].input) continue;
				const auto& x = w.streams[s];
				const size_t tid = streams[s].tid;
				if (x.order) {
					// the point it equals, else its neighbours among the
					// points the window, the inputs and the outputs before
					// it know
					const size_t me = w.slot_point(s, 0);
					std::vector<size_t> known = held_points(tid);
					for (size_t s2 = 0; s2 < S; ++s2)
						if (ordered(s2) && streams[s2].tid == tid
							&& (streams[s2].input ? need[s2] : s2 < s))
								known.push_back(w.slot_point(s2, 0));
					size_t same = SIZE_MAX, lo = SIZE_MAX, hi = SIZE_MAX;
					for (size_t p : known) {
						const int r = rel_get(full, me, p);
						if (r == 0) { same = p; break; }
						if (r > 0) {
							if (lo == SIZE_MAX || rel_get(full, p, lo) > 0)
								lo = p;
						} else if (hi == SIZE_MAX
							|| rel_get(full, p, hi) < 0) hi = p;
					}
					if (same != SIZE_MAX)
						e.out.push_back(point_atom(8, s, same));
					else if (lo == SIZE_MAX && hi == SIZE_MAX)
						e.out.push_back(atom(true, 9, s, 0, 0, 0));
					else {
						if (lo != SIZE_MAX)
							e.out.push_back(point_atom(7, s, lo));
						if (hi != SIZE_MAX)
							e.out.push_back(point_atom(6, s, hi));
					}
					continue;
				}
				if (!read0[s] && !reach[s]) continue;
				const size_t c = code_of(full, s, 0);
				if (blasted(s)) e.out.push_back(atom(true, 5, s, 0, 0, c));
				else if (x.two) e.out.push_back(atom(true, 0, s, 0, 0, c));
				else if (!x.values.empty())
					e.out.push_back(atom(true, 1, s, 0, 0, c));
				else if (constant(s, c) >= 0)
					e.out.push_back(atom(true, 0, s, 0, 0,
						(size_t)constant(s, c)));
				else {
					auto ps = partners(s, current);
					bool found = false;
					for (auto [s2, k2] : ps) {
						const size_t c2 = code_of(full, s2, k2);
						if (c2 == c || complements(s, c, c2)) {
							e.out.push_back(atom(true,
								c2 == c ? 2 : 3, s, s2, k2, 0));
							found = true;
							break;
						}
					}
					if (!found) {
						for (size_t val = 0; val < 2; ++val)
							e.out.push_back(atom(false, 0, s, 0, 0,
								val));
						for (auto [s2, k2] : ps) {
							e.out.push_back(atom(false, 2, s, s2, k2,
								0));
							if (x.orbit) e.out.push_back(atom(false,
								3, s, s2, k2, 0));
						}
					}
				}
				current[s] = true;
			}
			// the window of the next step
			std::vector<int> shifted(V, 0);
			for (size_t s = 0; s < S; ++s)
				for (size_t k = 0; k < d; ++k)
					if (kept(s, k + 1))
						set_code(shifted, s, k + 1, code_of(full, s, k));
			for (uint32_t v = w.code_vars(); v < V; ++v)
				if (w.shift[v] != data_bdd::leaf)
					shifted[v] = full[w.shift[v]] > 0;
			e.dst = state(after, canonical(shifted));
			// an order output is solved after the outputs it is placed
			// against, so its atoms keep the order of the outputs
			if (!w.has_order()) std::sort(e.out.begin(), e.out.end());
			edges[q].push_back(std::move(e));
			return ++edge_count <= max_edges;
		};
		// every choice of codes for the inputs read
		std::function<bool(size_t)> choose = [&](size_t i) -> bool {
			if (i == ins.size()) return leaf();
			const size_t s = ins[i];
			const auto& x = w.streams[s];
			if (blasted(s)) return choose(i + 1);
			if (x.order) {
				// every place among the points before it: the gap below
				// class g (even 2g) or class g itself (odd 2g + 1)
				const size_t tid = streams[s].tid;
				const size_t me = w.slot_point(s, 0);
				std::vector<size_t> pts = held_points(tid);
				for (size_t j = 0; j < i; ++j)
					if (ordered(ins[j]) && streams[ins[j]].tid == tid)
						pts.push_back(w.slot_point(ins[j], 0));
				const auto cs = classes(bits, pts);
				const std::vector<int> saved = bits;
				for (size_t pos = 0; pos <= 2 * cs.size(); ++pos) {
					const size_t g = pos / 2;
					const bool on = pos % 2;
					placed[s].clear();
					for (size_t c = 0; c < cs.size(); ++c)
						for (size_t p : cs[c])
							rel_set(bits, me, p, c < g ? 1
								: on && c == g ? 0 : -1);
					if (on) placed[s].push_back(point_atom(8, s, cs[g][0]));
					else {
						if (g > 0) placed[s].push_back(
							point_atom(7, s, cs[g - 1][0]));
						if (g < cs.size()) placed[s].push_back(
							point_atom(6, s, cs[g][0]));
					}
					if (!choose(i + 1)) return false;
					bits = saved;
				}
				placed[s].clear();
				return true;
			}
			std::vector<size_t> codes;
			if (x.two) codes = { 0, 1 };
			else if (!x.values.empty())
				for (size_t c = 0; c < x.values.size(); ++c)
					codes.push_back(c);
			else {
				std::set<size_t> held_codes;
				std::vector<bool> before_s(S, false);
				for (size_t j = 0; j < i; ++j) before_s[ins[j]] = true;
				for (auto [s2, k2] : partners(s, before_s)) {
					const size_t c2 = code_of(bits, s2, k2);
					if (constant(s, c2) < 0)
						held_codes.insert(x.orbit ? c2 & pair_mask(s) : c2);
				}
				size_t fresh = x.orbit ? 1 : 2;
				while (held_codes.contains(fresh)) ++fresh;
				held_codes.insert(fresh);
				if (x.orbit) {
					const size_t high = size_t{1} << (x.width - 1);
					codes = { 0, high };
					for (size_t p : held_codes) {
						if (p >= high) return false;
						codes.push_back(p);
						if (p != fresh) codes.push_back(p | high);
					}
				} else {
					codes = { 0, 1 };
					for (size_t c : held_codes) {
						if (c >> x.width) return false;
						codes.push_back(c);
					}
				}
			}
			for (size_t c : codes) {
				set_code(bits, s, 0, c);
				if (!choose(i + 1)) return false;
			}
			set_code(bits, s, 0, 0);
			return true;
		};
		if (!choose(0) || missing) return false;
	}
	if (states.size() > max_states || edge_count > max_edges)
		return false;

	// minimization: states with the same edges into the same blocks
	std::vector<size_t> block(states.size(), 0);
	for (size_t blocks = 1;;) {
		using sig_t = std::vector<std::tuple<std::vector<std::pair<size_t,
			int8_t>>, std::vector<size_t>, size_t>>;
		std::map<std::pair<size_t, sig_t>, size_t> ids;
		std::vector<size_t> nb(states.size());
		for (size_t q = 0; q < states.size(); ++q) {
			sig_t sig;
			for (const auto& e : edges[q])
				sig.emplace_back(std::vector<std::pair<size_t, int8_t>>(
					e.guard.begin(), e.guard.end()), e.out, block[e.dst]);
			std::sort(sig.begin(), sig.end());
			auto [it, _] = ids.emplace(std::pair{ block[q], std::move(sig) },
				ids.size());
			nb[q] = it->second;
		}
		block = std::move(nb);
		if (ids.size() == blocks) break;
		blocks = ids.size();
	}

	auto view = std::make_shared<ltl_aba_solution<node>>();
	auto& sol = *view;
	sol.data_game = true;
	sol.atoms = atoms;
	std::vector<int> ap_of(atoms.size());
	for (size_t i = 0; i < atoms.size(); ++i)
		if (atom_input[i]) {
			ap_of[i] = (int)sol.aut.aps.size();
			sol.aut.aps.push_back(atoms[i].second);
			sol.input_props.push_back(atoms[i].second);
		}
	for (size_t i = 0; i < atoms.size(); ++i)
		if (!atom_input[i]) {
			ap_of[i] = (int)sol.aut.aps.size();
			sol.aut.aps.push_back(atoms[i].second);
			sol.output_props.push_back(atoms[i].second);
		}
	size_t blocks = 0;
	for (size_t b : block) blocks = std::max(blocks, b + 1);
	sol.aut.num_states = blocks;
	sol.aut.initial_state = block[first];
	sol.aut.edges.resize(blocks);
	sol.aut.state_accepting.assign(blocks, false);
	std::vector<bool> done(blocks, false);
	using medges = std::vector<typename base::mealy_edge>;
	std::vector<medges> machine(blocks);
	for (size_t q = 0; q < states.size(); ++q) {
		if (done[block[q]]) continue;
		done[block[q]] = true;
		for (const auto& e : edges[q])
			machine[block[q]].push_back({ { e.guard.begin(), e.guard.end() },
				e.out, block[e.dst] });
	}
	// Whether atom `a` reads the current value of input `s`.
	auto reads_input = [&](size_t a, size_t s) {
		const auto& [eq, kind, s1, s2, k2, val] = atom_key[a];
		const bool slot = kind == 2 || kind == 3 || kind == 6 || kind == 7
			|| kind == 8;
		return s1 == s || (slot && k2 == 0 && s2 == s);
	};
	// An input a state's move does not depend on is not read there: its
	// atoms leave the guards when the edges that differ only in them agree
	// on the outputs and the next state, and no output is compared with it.
	for (auto& es : machine)
		for (size_t s = S; s-- > 0;) {
			if (!streams[s].input) continue;
			bool read_here = false;
			for (const auto& e : es)
				for (size_t a : e.out)
					read_here = read_here || reads_input(a, s);
			if (read_here) continue;
			medges merged;
			std::map<std::vector<std::pair<size_t, int8_t>>, size_t> at;
			bool agree = true;
			for (const auto& e : es) {
				typename base::mealy_edge m{ {}, e.out, e.dst };
				for (auto g : e.guard)
					if (!reads_input(g.first, s)) m.guard.push_back(g);
				auto [it, fresh] = at.emplace(m.guard, merged.size());
				if (fresh) merged.push_back(std::move(m));
				else if (merged[it->second].out != e.out
					|| merged[it->second].dst != e.dst)
						{ agree = false; break; }
			}
			if (agree) es = std::move(merged);
		}
	for (size_t q = 0; q < blocks; ++q)
		for (const auto& e : machine[q]) {
			std::string label;
			auto lit = [&](size_t a, bool pos) {
				if (!label.empty()) label += "&";
				label += (pos ? "" : "!") + std::to_string(ap_of[a]);
			};
			for (auto [a, g] : e.guard) lit(a, g > 0);
			for (size_t a : e.out) lit(a, true);
			sol.aut.edges[q].push_back({ label.empty() ? "t" : label,
				e.dst, false });
		}
	// the values before step 0: 0 for the inputs, the start's codes for
	// the outputs, and for the order streams points placed as the start
	// orders them, each against the constants and the slots before it
	auto at = [&](size_t s, size_t k) {
		const auto& x = streams[s];
		return x.input ? build_in_var_at_n<node>(x.name, -(int_t)k, x.tid)
			: build_out_var_at_n<node>(x.name, -(int_t)k, x.tid);
	};
	auto point_at = [&](size_t p) {
		const auto& x = w.points[p];
		return x.slot() ? at(x.s, x.k) : x.constant->get();
	};
	std::vector<std::pair<size_t, size_t>> met;
	std::vector<size_t> met_points;
	for (size_t k = 1; k <= d; ++k)
		for (size_t s = 0; s < S; ++s) {
			if (!kept(s, k)) continue;
			const auto& x = w.streams[s];
			const size_t c = code_of(start, s, k);
			tref v = at(s, k);
			if (x.order) {
				const size_t me = w.slot_point(s, k);
				std::vector<size_t> known;
				for (size_t p = 0; p < w.points.size(); ++p)
					if (!w.points[p].slot()
						&& w.points[p].tid == streams[s].tid)
							known.push_back(p);
				for (size_t p : met_points)
					if (w.points[p].tid == streams[s].tid)
						known.push_back(p);
				size_t same = SIZE_MAX, lo = SIZE_MAX, hi = SIZE_MAX;
				for (size_t p : known) {
					const int r = rel_get(start, me, p);
					if (r == 0) { same = p; break; }
					if (r > 0) {
						if (lo == SIZE_MAX || rel_get(start, p, lo) > 0)
							lo = p;
					} else if (hi == SIZE_MAX || rel_get(start, p, hi) < 0)
						hi = p;
				}
				if (same != SIZE_MAX)
					sol.history.push_back(tau::build_bf_eq(v,
						point_at(same)));
				else if (lo == SIZE_MAX && hi == SIZE_MAX) {
					tref z = pack_zero_constant<node>(streams[s].tid);
					if (!z) return false;
					sol.history.push_back(tau::build_bf_eq(v, z));
				} else {
					if (lo != SIZE_MAX) sol.history.push_back(
						tau::build_bf_lt(point_at(lo), v));
					if (hi != SIZE_MAX) sol.history.push_back(
						tau::build_bf_lt(v, point_at(hi)));
				}
				met_points.push_back(me);
				continue;
			}
			if (blasted(s)) {
				tref e = pack_value_constant<node>(streams[s].tid, c);
				if (!e) return false;
				sol.history.push_back(tau::build_bf_eq(v, e));
			} else if (!x.values.empty())
				sol.history.push_back(tau::build_bf_eq(v,
					x.values[c]->get()));
			else if (constant(s, c) >= 0)
				sol.history.push_back(tau::build_bf_eq(v,
					constant(s, c) ? one(s) : zero(s)));
			else {
				bool found = false;
				for (auto [s2, k2] : met) {
					if (streams[s2].tid != streams[s].tid) continue;
					const size_t c2 = code_of(start, s2, k2);
					if (c2 == c || complements(s, c, c2)) {
						tref y = at(s2, k2);
						sol.history.push_back(tau::build_bf_eq(v, c2 == c
							? y : tau::build_bf_neg(y)));
						found = true;
						break;
					}
				}
				if (!found) {
					sol.history.push_back(tau::build_bf_neq(v, zero(s)));
					sol.history.push_back(tau::build_bf_neq(v, one(s)));
					for (auto [s2, k2] : met) {
						if (streams[s2].tid != streams[s].tid) continue;
						tref y = at(s2, k2);
						sol.history.push_back(tau::build_bf_neq(v, y));
						if (x.orbit) sol.history.push_back(
							tau::build_bf_neq(v, tau::build_bf_neg(y)));
					}
				}
			}
			if (coded(s)) met.emplace_back(s, k);
		}
	this->view = std::move(view);
	this->machine = std::move(machine);
	this->reset();
	return true;
}

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
	using base::rep;

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
		if (!n.has_value() || !n.value()) {
			rep.append(std::move(n).report());
			return std::nullopt;
		}
		const auto& t = tau::get(n.value());
		std::optional<bool> out;
		if (t.equals_T()) out = true;
		else if (t.equals_F()) out = false;
		rep.append(std::move(n).report());
		return out;
	}

	result<std::optional<bool>> won_from(const window& win) override {
		result<std::optional<bool>> r;
		auto v = truth(won_init->get(), win);
		r.append(std::move(rep));
		rep.clear();
		return r.with_value(v);
	}

	// The free variables of the labels and moves: those of the vertex the
	// next step starts at, and of the system vertices its edges lead to,
	// at the step played; those of every vertex at the steps before.
	std::optional<typename base::slot_reads> direct_reads() const override
	{
		typename base::slot_reads d{ std::vector<bool>(streams.size()),
			std::vector<size_t>(streams.size(), 0) };
		auto scan = [&](const htref& f, bool played) {
			for (tref x : tau::get(f->get()).select_top(
				is_child<node, tau::io_var>))
			{
				auto it = index.find(get_var_name<node>(x));
				if (it == index.end()) continue;
				const size_t s = it->second;
				const size_t k = (size_t)get_io_var_shift<node>(x);
				if (k) d.back[s] = std::max(d.back[s], k);
				else if (played) d.now[s] = true;
			}
		};
		for (const auto& fs : labels) for (const auto& f : fs) scan(f, false);
		for (const auto& fs : moves) for (const auto& f : fs) scan(f, false);
		const size_t at = this->at;
		if (this->v[at].picks == 0) {
			for (const auto& f : labels[at]) scan(f, true);
			for (size_t x : this->v[at].dst)
				if (const size_t n = this->follow(x); this->v[n].picks == 1)
					for (const auto& f : moves[n]) scan(f, true);
		} else if (this->v[at].picks == 1)
			for (const auto& f : moves[at]) scan(f, true);
		return d;
	}

	result<std::optional<bool>> holds_label(size_t i, size_t j, const window& win)
		override
	{
		result<std::optional<bool>> r;
		auto v = truth(labels[i][j]->get(), win);
		r.append(std::move(rep));
		rep.clear();
		return r.with_value(v);
	}

	result<std::optional<bool>> holds_move(size_t i, size_t j, const window& win)
		override
	{
		result<std::optional<bool>> r;
		auto v = truth(moves[i][j]->get(), win);
		r.append(std::move(rep));
		rep.clear();
		return r.with_value(v);
	}

	result<tref> constraint(size_t i, const window& win, int_t t) override {
		result<tref> r;
		tref any = tau::_F();
		for (const auto& m : moves[i])
			any = tau::build_wff_or(any, m->get());
		auto n = normalize_non_temp<node>(at_step(any, win, t));
		if (!n.has_value() || !n.value()) {
			r.merge(std::move(n));
			return r.with_error(
				code::solver_error, "the data game strategy cannot read "
				"its move");
		}
		tref out = n.value();
		r.merge(std::move(n));
		return r.with_value(out);
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
		TAU_TRY(auto sol, solve(rewriter::replace<node>(won_init->get(), m), 0));
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

// The type of every stream the atoms read, when they all have one and its
// owner decides closed formulas over it (formula_regions::closed_type).
template <NodeType node>
static std::optional<size_t> closed_decision_type(
	const std::vector<std::pair<tref, std::string>>& atoms)
{
	using tau = tree<node>;
	std::optional<size_t> tid;
	for (auto& [atom, _] : atoms)
		for (tref v : tau::get(atom).select_top(
			is_child<node, tau::io_var>))
		{
			const size_t t = find_ba_type<node>(v);
			if (tid && *tid != t) return std::nullopt;
			tid = t;
		}
	if (!tid || !pack_type_decides_closed<node>(*tid)) return std::nullopt;
	return tid;
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
	// Reports of the deciders this call tries; a caller that decides demotes
	// the rest to warnings.
	report drops;
	const auto window = make_code_window<node>(atoms, 1024, drops);
	if (!window && !formulas) {
		r.append(std::move(drops));
		if (r.report().has_error()) return r;
		return r.with_value(data_game_verdict::undecided);
	}
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
	if (!built) {
		r.append(std::move(drops));
		if (r.report().has_error()) return r;
		return r.with_value(data_game_verdict::undecided);
	}
	const bool keep = strategy != nullptr;
	std::optional<bool> wins;
	if (window) {
		code_regions<node> codes(arena, *window,
			ltl_data_game_max_nodes(), ltl_data_game_max_memo());
		if (codes.init()) {
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
						codes.labels.begin()
							+ static_cast<std::ptrdiff_t>(codes.edge_base[i]),
						codes.labels.begin()
							+ static_cast<std::ptrdiff_t>(codes.edge_base[i]
								+ arena.v[i].edges.size()));
					st->moves.push_back(w->moves[i]);
				}
				st->won_init = w->sys[arena.init];
				// without a Mealy view the moves are played directly
				st->build_mealy(data_game_mealy_max_states,
					data_game_mealy_max_edges);
				*strategy = st;
			}
		}
		drops.append(std::move(codes.rep));
	}
	// codes the BDD cannot hold leave the game to the formulas, and those
	// with quantifiers the normalizer leaves standing to a type whose owner
	// decides them whole
	std::vector<std::optional<size_t>> modes{ std::nullopt };
	const size_t closed_seconds = ltl_closed_regions_timeout();
	if (auto t = closed_decision_type<node>(atoms); t && closed_seconds)
		modes.push_back(t);
	for (auto mode : modes) {
		if (wins || !formulas) break;
		// The decisions of closed regions share one time budget,
		// ltl_closed_regions_timeout(), each taking at most a quarter of
		// it, and one that passes either fails them, and only them:
		// nothing else reads its missing answer. A question these regions
		// can answer at all is answered in seconds, one they cannot keeps
		// cvc5 busy past any budget, so the quarter ends the latter early.
		std::optional<time_budget_handled> budget;
		if (mode) budget.emplace(std::chrono::seconds(closed_seconds),
			std::chrono::milliseconds(closed_seconds * 250));
		formula_regions<node> regions(arena, mode);
		data_game_solver solver(regions, arena,
			ltl_max_refinement_rounds(), keep);
		auto w = solver.solve_all();
		if (w) wins = regions.reached(w->sys[arena.init]);
		if (budget && budget->ran_out()) wins.reset();
		// a region that keeps quantifiers gives no moves to play
		if (keep && wins && *wins && !mode) {
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
		drops.append(std::move(regions.rep));
	}
	LOG_DEBUG << "[ltl_aba] data game: " << arena.v.size() << " vertices, "
		<< (wins ? (*wins ? "system wins" : "environment wins")
			: "undecided");
	if (!wins) {
		r.append(std::move(drops));
		if (r.report().has_error()) return r;
		return r.with_value(data_game_verdict::undecided);
	}
	// the game is decided: a decider that lost is only history
	drops.demote_errors_to_warnings();
	r.append(std::move(drops));
	return r.with_value(*wins ? data_game_verdict::realizable
		: data_game_verdict::unrealizable);
}

} // namespace idni::tau_lang
