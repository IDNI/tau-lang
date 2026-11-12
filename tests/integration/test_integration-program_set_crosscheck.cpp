// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// A second, independent decision of program-set expressions, compared with
// api::program_set_realizable on a corpus: the verdict cases of
// test_integration-program_set.cpp and a seeded random corpus.
//
// The route re-decides the realizability of the COMPILED specification
// (api::program_set_compile: a formula over the visible streams and the
// hidden prediction and selector streams) through complete types instead
// of tau's own skeleton. Over streams of type sbf, an atomless Boolean
// algebra, with the constants 0 and 1 only, an atom `f(x1..xk) = 0` holds
// exactly when every minterm `x1^e1 & ... & xk^ek` with f(e) = 1 is zero,
// so it depends only on the complete type of the tuple (x1..xk), the set of
// its nonzero minterms. In an atomless algebra every nonempty set of
// minterms is the type of some tuple, and a nonempty type of a tuple
// extends to every nonempty type of a longer tuple that restricts to it.
// The realizability is therefore the propositional game below, which
// ltlsynt decides directly through the Spot backend:
//  - the coordinates are every sbf stream of the compiled formula at each
//    lag it is read with (0..L_x); one output proposition m<e> per minterm
//    e of the whole tuple, one input proposition p<e'> per minterm e' of
//    the environment's part, the lagged coordinates and the inputs at lag 0;
//  - the system guarantees, every step, p<e'> <-> OR of the m<e> extending
//    e' (its type restricts to the environment's);
//  - the environment is assumed to pick, every step, a nonempty pattern
//    whose restriction to the lagged coordinates is the pattern the
//    previous step chose over their sources (lag k at this step is lag
//    k-1 at the previous one);
//  - a stream of the Boolean carrier type (a union selector) is a value bit
//    per lag, the lagged bit copying the previous step's value, and an
//    atom over such streams is the value of the Boolean function;
//  - the play has one step before step 0, where the system chooses the
//    values the hidden streams are read with at lag 1 at step 0 (tau's
//    "chosen before step 0"); the compiled formula is read from the next
//    step on, under X; the memory of a visible stream at step 0 is never
//    read, since every literal reading it is guarded;
//  - an atom `f = 0` is the conjunction of !m<e> over the e with f(e) = 1,
//    `f != 0` its negation;
//  - warm-up: every top-level conjunct of the compiled formula (a clause)
//    starts at its deepest visible lag k as a whole, under k nexts, the
//    way the skeleton starts a clause; a lag on a hidden stream gives no
//    warm-up (body_max_lookback); a time constraint `[t > k]` counts
//    absolute steps, the proposition s<k+1> ("step >= k+1", driven by the
//    system).
// The route declines (and the comparison skips) a formula with a CTL*
// operator, a binary temporal operator, a quantifier, a stream read at a
// fixed step, a constant other than 0 and 1, or more than 2 lagged
// coordinates or 7 coordinates in all: the synthesized automaton has to
// remember the pattern over the lagged coordinates, 2^(2^lagged) states,
// and 3 lagged coordinates already take minutes. The random corpus stays
// inside that budget as written; the compiler's delayed always parts (a
// positive literal beside a deeper lookback) can push one expression over
// it, which is declined and reported. It keeps the unions
// to lag-free safety leaves, where both routes decide one in well under a
// second (see generator::next). The cases after it hold what the corpus
// turned up, reduced by hand, with the verdicts the analysis gives.

#include "test_init.h"
#include "test_tau_helpers.h"
#include "api.h"
#include "backends/spot/spot.h"
#include "boolean_algebras/sbf/sbf_ba.h"

#include <chrono>
#include <map>
#include <random>
#include <set>
#include <sstream>

using namespace idni::tau_lang;

namespace {

// The first error of @p rep, rendered.
std::string first_error(const report& rep) {
	std::ostringstream why;
	rep.print(idni::diagnostics::sinks{
		.error = &why, .warning = &why, .info = {} }, term::colors(false));
	return why.str();
}

// The term @p term parsed with the hooks off (on, two tau constants fold
// into one), or nullptr.
tref parse_term(const char* term) {
	use_hooks_guard<node_t> hooks_off(false);
	return tau::get(term, parse_bf()).value_or(nullptr);
}

// The compiler's verdict: the nonemptiness api::program_set_realizable
// decides, nullopt (with @p why) when it refuses the expression.
std::optional<bool> by_compiler(const char* term, std::string& why) {
	tref t = parse_term(term);
	REQUIRE( t );
	auto r = api<node_t>::program_set_realizable(t);
	if (!r.has_value()) { why = first_error(r.report()); return std::nullopt; }
	return r.value();
}

// A stream at a lag.
struct coord {
	std::string stream;
	int_t lag;
	auto operator<=>(const coord&) const = default;
};

std::string join(const std::vector<std::string>& v, const char* sep) {
	std::string s;
	for (size_t i = 0; i < v.size(); ++i) { if (i) s += sep; s += v[i]; }
	return s;
}

// The complete-type game of a compiled program-set expression.
struct type_game {
	static constexpr size_t max_coords = 7, max_lagged = 2;

	std::string reason;                   // why the route declined
	std::vector<coord> coords;            // the main tuple of sbf coordinates
	std::vector<coord> sources;           // the sources tuple, when apart
	std::vector<coord> bits;              // the carrier-type coordinates
	std::set<std::string> inputs;
	std::set<int_t> steps;                // the step guards the formula reads
	const size_t sbf_type = sbf_type_id<node_t>();
	const size_t bit_type =
		get_ba_type_id<node_t>(pack_bool_carrier_type<node_t>());

	bool decline(const std::string& r) {
		if (reason.empty()) reason = r;
		return false;
	}

	static bool bit_of(size_t e, size_t j) { return (e >> j) & 1; }
	static std::string m(size_t e) { return "m" + std::to_string(e); }
	static std::string p(size_t e) { return "p" + std::to_string(e); }
	static std::string r(size_t e) { return "r" + std::to_string(e); }
	std::string bit_name(const coord& c) const {
		for (size_t i = 0; i < bits.size(); ++i)
			if (bits[i] == c) return "b" + std::to_string(i) + "_"
				+ std::to_string(c.lag);
		return "b_unknown";
	}

	// The streams of @p fm as coordinates; false when one is outside the
	// fragment or the tuple is over the budget.
	bool collect(tref fm) {
		std::map<std::string, std::pair<size_t, int_t>> streams; // type, max lag
		std::set<coord> read;
		for (tref v : tau::get(fm).select_all(is_child<node_t, tau::io_var>)) {
			if (is_io_initial<node_t>(v))
				return decline("a stream read at a fixed step");
			if (tau::get(v).find_top(is<node_t, tau::fwd_shift>))
				return decline("a stream read ahead");
			const std::string& name = get_var_name<node_t>(v);
			const int_t lag = get_io_var_shift<node_t>(v);
			auto& [type, max_lag] = streams[name];
			type = tau::get(v).get_ba_type();
			max_lag = std::max(max_lag, lag);
			read.insert({ name, lag });
			if (tau::get(v).is_input_variable()) inputs.insert(name);
		}
		// The main tuple holds every coordinate an atom reads and every
		// coordinate of the environment (a lagged one, an input at lag 0).
		// The source of a lagged coordinate (its stream one lag earlier)
		// that is outside it is an output no atom reads at lag 0, such as a
		// prediction: when every source is such a coordinate, the sources
		// form a tuple of their own, disjoint from the main one, so any
		// types of the two are jointly realizable.
		auto in_main = [&](const coord& c) {
			return read.contains(c) || c.lag >= 1 || inputs.contains(c.stream);
		};
		bool source_in = false;
		for (const auto& [name, tl] : streams)
			for (int_t k = 1; k <= tl.second; ++k)
				source_in |= in_main({ name, k - 1 });
		size_t lagged = 0;
		for (const auto& [name, tl] : streams) {
			const auto& [type, max_lag] = tl;
			if (type != sbf_type && type != bit_type)
				return decline("a stream of a type outside sbf and the "
					"Boolean carrier: " + name);
			for (int_t k = 0; k <= max_lag; ++k) {
				const coord c{ name, k };
				if (type == bit_type) bits.push_back(c);
				else if (in_main(c) || source_in) {
					coords.push_back(c);
					if (k > 0) ++lagged;
				} else sources.push_back(c);
			}
		}
		if (coords.size() > max_coords || lagged > max_lagged)
			return decline("over the budget: " + std::to_string(coords.size())
				+ " coordinates, " + std::to_string(lagged) + " lagged");
		return true;
	}

	// The coordinates a term reads, and its type; false outside the
	// fragment (a constant other than 0 and 1, a cast, a reference).
	bool scan(tref b, std::set<coord>& reads, size_t& type) {
		const auto& t = tau::get(b);
		if (!t.is(tau::bf)) return decline("not a term: " + t.to_str());
		const auto& c = t[0];
		if (c.is(tau::bf_t) || c.is(tau::bf_f)) return true;
		if (c.is(tau::bf_and) || c.is(tau::bf_or) || c.is(tau::bf_xor)) {
			for (tref k : c.get_children())
				if (!scan(k, reads, type)) return false;
			return true;
		}
		if (c.is(tau::bf_neg)) return scan(c.child(0), reads, type);
		if (c.is(tau::variable)) {
			tref v = t.child(0);
			if (!is_child<node_t, tau::io_var>(v))
				return decline("a variable that is no stream: " + t.to_str());
			const size_t vt = tau::get(v).get_ba_type();
			if (type != 0 && type != vt)
				return decline("an atom over two types: " + t.to_str());
			type = vt;
			reads.insert({ get_var_name<node_t>(v),
				get_io_var_shift<node_t>(v) });
			return true;
		}
		return decline("a term outside the fragment: " + t.to_str());
	}

	// The value of the scanned term @p b at the assignment @p val.
	static bool eval(tref b, const std::map<coord, bool>& val) {
		const auto& t = tau::get(b);
		const auto& c = t[0];
		if (c.is(tau::bf_t)) return true;
		if (c.is(tau::bf_f)) return false;
		if (c.is(tau::bf_neg)) return !eval(c.child(0), val);
		if (c.is(tau::bf_and)) {
			for (tref k : c.get_children()) if (!eval(k, val)) return false;
			return true;
		}
		if (c.is(tau::bf_or)) {
			for (tref k : c.get_children()) if (eval(k, val)) return true;
			return false;
		}
		if (c.is(tau::bf_xor)) {
			bool r = false;
			for (tref k : c.get_children()) r ^= eval(k, val);
			return r;
		}
		tref v = t.child(0);
		return val.at({ get_var_name<node_t>(v), get_io_var_shift<node_t>(v) });
	}

	// The literal `l = r` (`l != r` with @p neq) as a propositional formula
	// over the type propositions.
	std::optional<std::string> atom(tref l, tref r, bool neq) {
		std::set<coord> reads;
		size_t type = 0;
		if (!scan(l, reads, type) || !scan(r, reads, type)) return std::nullopt;
		auto f = [&](const std::map<coord, bool>& val) {
			return eval(l, val) != eval(r, val); };
		std::string s;
		if (reads.empty()) s = f({}) ? "0" : "1";
		else if (type == sbf_type) {
			std::vector<std::string> lits;
			for (size_t e = 0; e < (size_t(1) << coords.size()); ++e) {
				std::map<coord, bool> val;
				for (size_t j = 0; j < coords.size(); ++j)
					val[coords[j]] = bit_of(e, j);
				if (f(val)) lits.push_back("!" + m(e));
			}
			s = lits.empty() ? "1" : "(" + join(lits, " & ") + ")";
		} else {
			std::vector<coord> rs(reads.begin(), reads.end());
			std::vector<std::string> lits;
			for (size_t a = 0; a < (size_t(1) << rs.size()); ++a) {
				std::map<coord, bool> val;
				std::vector<std::string> conj;
				for (size_t j = 0; j < rs.size(); ++j) {
					val[rs[j]] = bit_of(a, j);
					conj.push_back((bit_of(a, j) ? "" : "!") + bit_name(rs[j]));
				}
				if (f(val)) lits.push_back("!(" + join(conj, " & ") + ")");
			}
			s = lits.empty() ? "1" : "(" + join(lits, " & ") + ")";
		}
		return neq ? "!" + s : s;
	}

	// `[t > k]` and its kin over the step propositions, as
	// time_constraint_skeleton reads them.
	std::string time_constraint(const tau& c) {
		const bool num_left = c[0][0].is(tau::num);
		const int_t k = get_payload_int<node_t>(num_left ? c[0][0] : c[0][1]);
		auto ge = [&](int_t j) -> std::string {
			if (j <= 0) return "1";
			steps.insert(j);
			return "s" + std::to_string(j);
		};
		auto nt = c[0].value.nt;
		if (num_left) {
			if (nt == tau::ctn_gt) nt = tau::ctn_lt;
			else if (nt == tau::ctn_lt) nt = tau::ctn_gt;
			else if (nt == tau::ctn_gteq) nt = tau::ctn_lteq;
			else if (nt == tau::ctn_lteq) nt = tau::ctn_gteq;
		}
		if (nt == tau::ctn_gteq) return ge(k);
		if (nt == tau::ctn_gt) return ge(k + 1);
		if (nt == tau::ctn_lteq) return "!" + ge(k + 1);
		if (nt == tau::ctn_lt) return "!" + ge(k);
		if (nt == tau::ctn_eq) return "(" + ge(k) + " & !" + ge(k + 1) + ")";
		return "!(" + ge(k) + " & !" + ge(k + 1) + ")";
	}

	// The formula @p n in Spot syntax.
	std::optional<std::string> wff(tref n) {
		const auto& t = tau::get(n);
		if (!t.is(tau::wff)) {
			decline("not a formula: " + t.to_str());
			return std::nullopt;
		}
		const auto& c = t[0];
		std::vector<std::string> parts;
		for (tref k : c.get_children()) {
			if (!tau::get(k).is(tau::wff)) continue;
			auto s = wff(k);
			if (!s) return std::nullopt;
			parts.push_back(*s);
		}
		if (c.is(tau::wff_and) || c.is(tau::wff_or))
			return "(" + join(parts, c.is(tau::wff_and) ? " & " : " | ") + ")";
		if (c.is(tau::wff_neg)) return "!" + parts.at(0);
		if (c.is(tau::wff_imply))
			return "(" + parts.at(0) + " -> " + parts.at(1) + ")";
		if (c.is(tau::wff_equiv))
			return "(" + parts.at(0) + " <-> " + parts.at(1) + ")";
		if (c.is(tau::wff_xor))
			return "!(" + parts.at(0) + " <-> " + parts.at(1) + ")";
		if (c.is(tau::wff_always)) return "G(" + parts.at(0) + ")";
		if (c.is(tau::wff_sometimes)) return "F(" + parts.at(0) + ")";
		if (c.is(tau::wff_t)) return "1";
		if (c.is(tau::wff_f)) return "0";
		if (c.is(tau::bf_eq) || c.is(tau::bf_neq))
			return atom(c.child(0), c.child(1), c.is(tau::bf_neq));
		if (c.is(tau::constraint)) return time_constraint(c);
		decline("a connective outside the fragment: " + t.to_str());
		return std::nullopt;
	}

	// The deepest lag of a visible stream in @p clause (body_max_lookback):
	// a lag on a hidden stream gives no warm-up.
	static int_t clause_lookback(tref clause) {
		int_t k = 0;
		for (tref v : tau::get(clause).select_all(is_child<node_t, tau::io_var>))
			if (!is_program_set_stream(get_var_name<node_t>(v)))
				k = std::max(k, get_io_var_shift<node_t>(v));
		return k;
	}

	// The top-level conjuncts of @p fm.
	static void clauses(tref fm, trefs& out) {
		const auto& t = tau::get(fm);
		if (t.child_is(tau::wff_and))
			for (tref c : t[0].get_children()) clauses(c, out);
		else out.push_back(fm);
	}

	// @p s under @p k nexts.
	static std::string nexts(size_t k, const std::string& s) {
		std::string r = s;
		for (size_t i = 0; i < k; ++i) r = "X(" + r + ")";
		return r;
	}

	// The verdict of the game of @p compiled, nullopt when declined.
	std::optional<bool> decide(const program_set_compiled<node_t>& compiled) {
		if (!collect(compiled.formula)) return std::nullopt;
		// every clause starts at its deepest visible lag
		trefs top;
		clauses(compiled.formula, top);
		std::vector<std::string> shifted;
		for (tref c : top) {
			auto s = wff(c);
			if (!s) return std::nullopt;
			shifted.push_back(nexts((size_t) clause_lookback(c), *s));
		}
		const std::string body = "(" + join(shifted, " & ") + ")";
		const size_t n = coords.size();
		// the sources of the lagged coordinates sit in the sources tuple
		// when it is apart, in the main one otherwise
		const std::vector<coord>& src_tuple = sources.empty() ? coords : sources;
		std::vector<size_t> env, lagged, source; // indices into the tuples
		for (size_t j = 0; j < n; ++j) {
			if (coords[j].lag >= 1 || inputs.contains(coords[j].stream))
				env.push_back(j);
			if (coords[j].lag >= 1) {
				lagged.push_back(j);
				for (size_t i = 0; i < src_tuple.size(); ++i)
					if (src_tuple[i].stream == coords[j].stream
						&& src_tuple[i].lag == coords[j].lag - 1)
						source.push_back(i);
			}
		}
		auto extends = [&](size_t e, const std::vector<size_t>& at,
			size_t pattern)
		{
			for (size_t j = 0; j < at.size(); ++j)
				if (bit_of(e, at[j]) != bit_of(pattern, j)) return false;
			return true;
		};
		std::vector<std::string> ins, outs, guarantees, assumptions;
		for (size_t e = 0; e < (size_t(1) << n); ++e) outs.push_back(m(e));
		if (!sources.empty()) {
			// a type of its own, which has to be nonempty
			std::vector<std::string> rs;
			for (size_t e = 0; e < (size_t(1) << sources.size()); ++e)
				rs.push_back(r(e));
			outs.insert(outs.end(), rs.begin(), rs.end());
			guarantees.push_back("(" + join(rs, " | ") + ")");
		}
		if (!env.empty()) {
			std::vector<std::string> nonempty;
			for (size_t ep = 0; ep < (size_t(1) << env.size()); ++ep) {
				ins.push_back(p(ep));
				nonempty.push_back(p(ep));
				std::vector<std::string> ext;
				for (size_t e = 0; e < (size_t(1) << n); ++e)
					if (extends(e, env, ep)) ext.push_back(m(e));
				guarantees.push_back("(" + p(ep) + " <-> (" + join(ext, " | ")
					+ "))");
			}
			assumptions.push_back("(" + join(nonempty, " | ") + ")");
			// the lagged coordinates within the environment's part
			std::vector<size_t> lagged_in_env;
			for (size_t j : lagged)
				lagged_in_env.push_back(
					(size_t) (std::ranges::find(env, j) - env.begin()));
			for (size_t f = 0; f < (size_t(1) << lagged.size()); ++f) {
				std::vector<std::string> cur, nxt;
				for (size_t e = 0; e < (size_t(1) << src_tuple.size()); ++e)
					if (extends(e, source, f))
						cur.push_back(sources.empty() ? m(e) : r(e));
				for (size_t ep = 0; ep < (size_t(1) << env.size()); ++ep)
					if (extends(ep, lagged_in_env, f)) nxt.push_back(p(ep));
				assumptions.push_back("((" + join(cur, " | ") + ") <-> X("
					+ join(nxt, " | ") + "))");
			}
		}
		for (const coord& c : bits) {
			const std::string name = bit_name(c);
			if (c.lag == 0 && inputs.contains(c.stream)) ins.push_back(name);
			else outs.push_back(name);
			if (c.lag >= 1) guarantees.push_back("(X(" + name + ") <-> "
				+ bit_name({ c.stream, c.lag - 1 }) + ")");
		}
		std::vector<std::string> drivers;
		for (int_t k : steps) {
			const std::string s = "s" + std::to_string(k);
			outs.push_back(s);
			std::vector<std::string> d;
			for (int_t j = 0; j < k; ++j) d.push_back(nexts((size_t)j, "!" + s));
			d.push_back(nexts((size_t)k, "G(" + s + ")"));
			drivers.push_back("(" + join(d, " & ") + ")");
		}
		if (outs.empty()) {
			decline("no stream");
			return std::nullopt;
		}
		std::vector<std::string> parts;
		if (!guarantees.empty())
			parts.push_back("G(" + join(guarantees, " & ") + ")");
		if (!drivers.empty())
			parts.push_back("X(" + join(drivers, " & ") + ")");
		std::string goal = "X" + body;
		if (!assumptions.empty())
			goal = "(G(" + join(assumptions, " & ") + ") -> " + goal + ")";
		parts.push_back(goal);
		auto v = synthesize(join(parts, " & "), ins, outs, 30, {});
		if (!v.has_value()) {
			decline("ltlsynt gave no verdict: " + first_error(v.report()));
			return std::nullopt;
		}
		return v.value().realizable;
	}
};

// The complete-type verdict of @p term, nullopt (with @p why) when the
// compiler refuses the expression or the route declines its formula.
std::optional<bool> by_types(const char* term, std::string& why) {
	tref t = parse_term(term);
	REQUIRE( t );
	auto c = api<node_t>::program_set_compile(t);
	if (!c.has_value()) { why = first_error(c.report()); return std::nullopt; }
	type_game game;
	auto v = game.decide(c.value());
	if (!v) why = game.reason;
	return v;
}

struct tally {
	size_t agree = 0, disagree = 0, compiler_undecided = 0, route_declined = 0;
	double compiler_s = 0, route_s = 0;
	std::vector<std::string> declined, undecided, slow;
};

const std::optional<bool> T = true, F = false;

// Decides @p term through both routes, checks them against each other
// (and against @p expected when known) and accounts for the outcome.
void cross_check(const std::string& term, tally& tl,
	std::optional<std::optional<bool>> expected = std::nullopt)
{
	using clock = std::chrono::steady_clock;
	INFO( "term: " << term );
	std::string why_c, why_r;
	auto t0 = clock::now();
	auto c = by_compiler(term.c_str(), why_c);
	auto t1 = clock::now();
	auto r = by_types(term.c_str(), why_r);
	auto t2 = clock::now();
	const double cs = std::chrono::duration<double>(t1 - t0).count();
	const double rs = std::chrono::duration<double>(t2 - t1).count();
	tl.compiler_s += cs;
	tl.route_s += rs;
	if (cs > 1 || rs > 1) {
		std::ostringstream s;
		s << term << " -- compiler " << cs << " s, types " << rs << " s";
		tl.slow.push_back(s.str());
	}
	INFO( "compiler: " << (c ? (*c ? "nonempty" : "empty") : "undecided: "
		+ why_c) );
	INFO( "types: " << (r ? (*r ? "nonempty" : "empty") : "declined: "
		+ why_r) );
	if (expected) {
		CHECK( c == *expected );
		if (r && *expected) CHECK( *r == **expected );
	}
	if (!c) ++tl.compiler_undecided, tl.undecided.push_back(term + " -- " + why_c);
	if (!r) ++tl.route_declined, tl.declined.push_back(term + " -- " + why_r);
	if (c && r) {
		CHECK( *c == *r );
		if (*c == *r) ++tl.agree; else ++tl.disagree;
	}
}

void report_tally(const char* corpus, const tally& tl) {
	std::ostringstream s;
	s << corpus << ": " << tl.agree << " agreements, " << tl.disagree
		<< " disagreements, " << tl.compiler_undecided
		<< " undecided by the compiler, " << tl.route_declined
		<< " declined by the route; compiler " << tl.compiler_s
		<< " s, types " << tl.route_s << " s";
	for (const auto& d : tl.declined) s << "\n  declined: " << d;
	for (const auto& u : tl.undecided) s << "\n  undecided: " << u;
	for (const auto& w : tl.slow) s << "\n  over a second: " << w;
	MESSAGE( s.str() );
}

// ── the random corpus ──────────────────────────────────────────────────

struct atom_shape {
	const char* text;
	bool reads_input;
	const char* lagged;   // the visible stream read at lag 1, or ""
};

const atom_shape atoms[] = {
	{ "o1[t]:sbf = i1[t]:sbf", true, "" },
	{ "o1[t]:sbf != i1[t]:sbf", true, "" },
	{ "o1[t]:sbf = 0", false, "" },
	{ "o1[t]:sbf = 1", false, "" },
	{ "o1[t]:sbf != 0", false, "" },
	{ "o1[t]:sbf != 1", false, "" },
	{ "i1[t]:sbf = 0", true, "" },
	{ "i1[t]:sbf = 1", true, "" },
	{ "i1[t]:sbf != 0", true, "" },
	{ "o1[t]:sbf & i1[t]:sbf = 0", true, "" },
	{ "o1[t]:sbf | i1[t]:sbf = 1", true, "" },
	{ "o1[t]:sbf & i1[t]:sbf' = 0", true, "" },
	{ "o1[t]:sbf = o1[t-1]:sbf", false, "o1" },
	{ "o1[t]:sbf != o1[t-1]:sbf", false, "o1" },
	{ "o1[t]:sbf & o1[t-1]:sbf = 0", false, "o1" },
	{ "o1[t]:sbf = o1[t-1]:sbf'", false, "o1" },
	{ "o1[t]:sbf = i1[t-1]:sbf", true, "i1" },
	{ "i1[t]:sbf = i1[t-1]:sbf", true, "i1" },
};

struct leaf {
	std::string spec;
	bool reads_input = false;
	bool liveness = false;            // holds a sometimes
	std::set<std::string> lagged;
};

struct expr {
	int leaf_index = -1;              // a leaf, or
	char op = 0;                      // '&' or '|' over
	std::vector<expr> args;
	bool complemented = false;
};

struct generator {
	std::mt19937 rng{ 20261112u };
	std::vector<leaf> leaves;

	size_t pick(size_t k) { return rng() % k; }

	leaf make_leaf() {
		leaf l;
		auto atom = [&] {
			const atom_shape& a = atoms[pick(std::size(atoms))];
			l.reads_input |= a.reads_input;
			if (*a.lagged) l.lagged.insert(a.lagged);
			return std::string(a.text);
		};
		const size_t shape = pick(6);
		switch (shape) {
		case 0: l.spec = "always " + atom(); break;
		case 1: l.spec = "sometimes " + atom(); break;
		case 2: { auto a = atom(), b = atom();
			l.spec = "always (" + a + " -> " + b + ")"; break; }
		case 3: { auto a = atom(), b = atom();
			l.spec = "always (" + a + " -> sometimes " + b + ")"; break; }
		case 4: l.spec = "always sometimes " + atom(); break;
		default: l.spec = "always (" + atom() + " || [t > 0])"; break;
		}
		l.liveness = shape == 1 || shape == 3 || shape == 4;
		return l;
	}

	expr make_tree(size_t n_leaves) {
		if (n_leaves == 1) {
			expr e;
			e.leaf_index = (int) leaves.size();
			leaves.push_back(make_leaf());
			return e;
		}
		expr e;
		e.op = pick(2) ? '&' : '|';
		const size_t left = 1 + pick(n_leaves - 1);
		e.args.push_back(make_tree(left));
		e.args.push_back(make_tree(n_leaves - left));
		return e;
	}

	static void nodes(expr& e, std::vector<expr*>& out) {
		out.push_back(&e);
		for (auto& a : e.args) nodes(a, out);
	}

	// The complemented leaves after the complements are pushed in.
	void nnf_complements(const expr& e, bool negate, std::set<int>& out) const {
		const bool neg = negate != e.complemented;
		if (e.leaf_index >= 0) { if (neg) out.insert(e.leaf_index); return; }
		for (const auto& a : e.args) nnf_complements(a, neg, out);
	}

	// Whether a union is left after the complements are pushed in.
	bool nnf_has_union(const expr& e, bool negate) const {
		const bool neg = negate != e.complemented;
		if (e.leaf_index >= 0) return false;
		if ((e.op == '|') != neg) return true;
		for (const auto& a : e.args) if (nnf_has_union(a, neg)) return true;
		return false;
	}

	std::string text(const expr& e) const {
		std::string s;
		if (e.leaf_index >= 0) s = "{ " + leaves[(size_t) e.leaf_index].spec + " }:tau";
		else s = "(" + text(e.args[0]) + " " + e.op + " " + text(e.args[1])
			+ ")";
		return e.complemented ? s + "'" : s;
	}

	// An expression inside the route's budget: at most 2 lagged
	// coordinates (a prediction per complemented leaf when an input is
	// read, plus the visible streams read at lag 1) and 7 in all, and no
	// complemented leaf reading a lag beside an input (its guard reads the
	// prediction at lag 2 and the input at lag 1). A union, whose selector
	// takes both routes seconds to minutes with a `sometimes` or a lag
	// beside it, keeps lag-free safety leaves only.
	std::optional<std::string> next() {
		leaves.clear();
		expr e = make_tree(1 + pick(3));
		std::vector<expr*> all;
		nodes(e, all);
		for (size_t c = pick(3); c > 0; --c) all[pick(all.size())]->complemented = true;
		std::set<int> complemented;
		nnf_complements(e, false, complemented);
		bool reads_input = false;
		std::set<std::string> lagged;
		for (const auto& l : leaves) {
			reads_input |= l.reads_input;
			lagged.insert(l.lagged.begin(), l.lagged.end());
		}
		if (nnf_has_union(e, false))
			for (const auto& l : leaves)
				if (l.liveness || !l.lagged.empty()) return std::nullopt;
		if (reads_input) {
			for (int i : complemented)
				if (!leaves[(size_t) i].lagged.empty()) return std::nullopt;
			const size_t lagged_coords = lagged.size() + complemented.size();
			const size_t coords = 2 + complemented.size() + lagged_coords;
			if (lagged_coords > type_game::max_lagged
				|| coords > type_game::max_coords) return std::nullopt;
		}
		return text(e);
	}
};

} // namespace

#define PS_CASE(name) TEST_CASE(name * doctest::skip(!ltlsynt_available()))

TEST_SUITE("program_set complete-type cross-check") {

	PS_CASE("the verdict cases of test_integration-program_set") {
		struct fixed { const char* name; const char* term; std::optional<bool> expected; };
		const fixed cases[] = {
			{ "T1", "{ always o1[t]:sbf = i1[t]:sbf }:tau'", T },
			{ "T2", "{ always o1[t]:sbf = i1[t]:sbf }:tau"
				" & { always o1[t]:sbf = i1[t]:sbf }:tau'", F },
			{ "T3", "{ always o1[t]:sbf = i1[t]:sbf }:tau"
				" & { sometimes o1[t]:sbf = 1 }:tau'", T },
			{ "T4", "{ always o1[t]:sbf = i1[t]:sbf }:tau"
				" & { always (i1[t]:sbf = 1 -> o1[t]:sbf = 1) }:tau'", F },
			{ "T5", "({ always o1[t]:sbf = 1 }:tau | { always o1[t]:sbf = 0 }:tau)"
				" & { always ((i1[t]:sbf = 1) <-> (o1[t]:sbf = 1)) }:tau", F },
			{ "T6", "{ always (i1[t]:sbf = 1 || [t > 0]) }:tau'", T },
			{ "T7", "{ always o1[t]:sbf = o1[t]:sbf }:tau'", F },
			{ "T8", "{ always ((i1[t]:sbf != 0 && i1[t]:sbf != 1) -> o1[t]:sbf != i1[t]:sbf) }:tau'"
				" & { always ((i1[t]:sbf != 0 && i1[t]:sbf != 1) -> o1[t]:sbf != i1[t]:sbf') }:tau'", T },
			{ "T9", "{ always (o1[t]:sbf = 1 || [t > 0]) }:tau'"
				" & { always (o1[t]:sbf != 1 || [t > 0]) }:tau'"
				" & { always o2[t]:sbf = i1[t]:sbf }:tau", T },
			{ "T9'", "{ always (o1[t]:sbf = 1 || [t > 0]) }:tau'"
				" & { always (o1[t]:sbf != 1 || [t > 0]) }:tau'", F },
			{ "T10", "{ (always i1[t]:sbf = 0) -> always (o1[t]:sbf != 1 || [t > 0]) }:tau'"
				" & { (always i1[t]:sbf = 0) -> always (o1[t]:sbf = 1 || [t > 0]) }:tau'", F },
			{ "T11", "{ sometimes o1[t]:sbf = 1 }:tau"
				" & { always sometimes o1[t]:sbf = 1 }:tau'", T },
			{ "T12", "{ always (i1[t]:sbf != 0 -> sometimes o1[t]:sbf = 1) }:tau"
				" & { sometimes o1[t]:sbf = 1 }:tau'", T },
			{ "T13", "{ always sometimes o1[t]:sbf = 1 }:tau"
				" & { sometimes o1[t]:sbf = 1 }:tau'", F },
			{ "T15", "({ always o1[t]:sbf = 1 }:tau | { always o1[t]:sbf = 1 }:tau')'", F },
			{ "copier not in zero", "{ always o1[t]:sbf = i1[t]:sbf }:tau"
				" & { always o1[t]:sbf = 0 }:tau'", T },
			{ "zero not in copier", "{ always o1[t]:sbf = 0 }:tau"
				" & { always o1[t]:sbf = i1[t]:sbf }:tau'", T },
			{ "one not in zero", "{ always o1[t]:sbf = 1 }:tau"
				" & { always o1[t]:sbf = 0 }:tau'", T },
			{ "copier in itself", "{ always o1[t]:sbf = i1[t]:sbf }:tau"
				" & { always o1[t]:sbf = i1[t]:sbf }:tau'", F },
			{ "B2", "{ always o1[t]:sbf = o1[t-1]:sbf }:tau", T },
			{ "B3", "{ always o1[t]:sbf = 1 }:tau"
				" & { always o1[t]:sbf = o1[t-1]:sbf }:tau'", F },
			{ "B4", "{ always (o1[t]:sbf = 1 || [t > 0]) }:tau"
				" & { always o1[t]:sbf = o1[t-1]:sbf }:tau'", T },
			{ "W1", "{ always o1[t]:sbf = i1[t-2]:sbf }:tau", T },
			{ "L1", "{ always i1[t+2]:sbf = i1[t+2]:sbf }:tau", T },
			{ "L2", "{ always o1[t]:sbf = i1[t+1]:sbf }:tau", F },
			{ "S1", "{ always o1[t-1]:sbf = i1[t]:sbf }:tau", F },
			{ "R1", "{ (always o1[t]:sbf = o1[t-1]:sbf)"
				" && (always (i1[t]:sbf = 1 -> o1[t]:sbf = 1))"
				" && (always (i1[t]:sbf = 0 -> o1[t]:sbf = 0)) }:tau", F },
			{ "R1'", "{ (always o1[t]:sbf = o1[t-1]:sbf)"
				" && (always (i1[t]:sbf = 1 -> o1[t]:sbf = 1)) }:tau", T },
			{ "C1", "{ E (sometimes i1[t]:sbf = 1) }:tau"
				" & { always o1[t]:sbf = i1[t]:sbf }:tau", T },
			{ "C2", "{ E (always i1[t]:sbf = 1) }:tau"
				" & { always o1[t]:sbf = 0 }:tau", F },
			{ "C3", "{ E (always i1[t]:sbf = 1) }:tau"
				" | { always o1[t]:sbf = 0 }:tau", T },
			{ "C4", "{ E (sometimes i1[t]:sbf = 1) }:tau'", std::nullopt },
			{ "C5", "({ E (sometimes i1[t]:sbf = 1) }:tau"
				" & { always o1[t]:sbf = 0 }:tau)'", std::nullopt },
		};
		tally tl;
		for (const auto& c : cases) {
			INFO( "case: " << c.name );
			cross_check(c.term, tl, c.expected);
		}
		report_tally("fixed corpus", tl);
		// the CTL* leaves are the only formulas outside the route's fragment
		CHECK( tl.route_declined == 5 );
		CHECK( tl.disagree == 0 );
	}

	PS_CASE("a seeded random corpus of 100 expressions") {
		generator gen;
		tally tl;
		size_t count = 0;
		while (count < 100) {
			auto term = gen.next();
			if (!term) continue;
			++count;
			cross_check(*term, tl);
		}
		report_tally("random corpus", tl);
		// an expression the compiler leaves undecided or the route declines
		// is reported above; a decline comes only from the lagged coordinates
		// the compiled form adds beyond the budget
		CHECK( tl.route_declined <= 1 );
		CHECK( tl.disagree == 0 );
	}

	// Found by the random corpus. With an input read somewhere and a
	// visible lag k in the expression, the compiler breaks a complemented
	// literal at steps >= k only, so a literal that can only be broken at
	// step 0 ([t > 0] beside it) is never broken and the complement is
	// decided empty. Both sets below are nonempty: the first holds the
	// constant-zero program (the zero input gives `0 | 0 != 1` at step 0),
	// and the second is `{ always o1[t] = o1[t-1] }:tau` itself, since no
	// program forces the first input (T6). The compiler agrees with both
	// verdicts once the constraint is `[t > 1]`.
	PS_CASE("the complement of a step-0 literal beside a lagged literal") {
		tally tl;
		cross_check("{ always o1[t]:sbf = o1[t-1]:sbf }:tau"
			" & { always (o1[t]:sbf | i1[t]:sbf = 1 || [t > 0]) }:tau'", tl, T);
		cross_check("{ always o1[t]:sbf = o1[t-1]:sbf }:tau"
			" & { always (i1[t]:sbf = 1 || [t > 0]) }:tau'", tl, T);
		cross_check("{ always o1[t]:sbf = o1[t-1]:sbf }:tau"
			" & { always (o1[t]:sbf | i1[t]:sbf = 1 || [t > 1]) }:tau'", tl, T);
		cross_check("{ always o1[t]:sbf = o1[t-1]:sbf }:tau"
			" & { always (i1[t]:sbf = 1 || [t > 1]) }:tau'", tl, T);
		report_tally("step-0 complements", tl);
	}

	// Found by the random corpus while the route guarded literals one by
	// one, which let a trigger at step 0 start the obligation before its
	// witness could fire. The early-time rule is per clause: a clause
	// starts at its deepest lookback as a whole, so the triggered clause
	// starts at step 1, and a program that never outputs 1 and repeats its
	// previous output on a trigger step (that output differs from the input
	// 1) is in both triggered sets. The untriggered control is empty: the
	// environment plays `i1[t] = o1[t-1]` from step 1 on, so the output can
	// repeat itself only by equalling the input.
	PS_CASE("an obligation an input triggers beside an inequality") {
		tally tl;
		cross_check("{ always (i1[t]:sbf = 1 -> sometimes o1[t]:sbf = o1[t-1]:sbf) }:tau"
			" & { always o1[t]:sbf != i1[t]:sbf }:tau", tl, T);
		cross_check("{ always (o1[t]:sbf & i1[t]:sbf' = 0 -> sometimes o1[t]:sbf = o1[t-1]:sbf) }:tau"
			" & { always o1[t]:sbf != i1[t]:sbf }:tau", tl, T);
		cross_check("{ sometimes o1[t]:sbf = o1[t-1]:sbf }:tau"
			" & { always o1[t]:sbf != i1[t]:sbf }:tau", tl, F);
		report_tally("triggered obligations", tl);
	}

	// Found while settling the rule above. The second set is empty (no
	// program forces the first input, T6), so the intersection is empty,
	// and each literal keeps its own warm-up; the compiler merges the two
	// always clauses and starts both at step 1, where the demand on the
	// first input is gone. One body with both conjuncts starts at step 1
	// by the rule and is nonempty.
	PS_CASE("a step-0 demand beside a lagged literal") {
		tally tl;
		cross_check("{ always o1[t]:sbf = o1[t-1]:sbf }:tau"
			" & { always (i1[t]:sbf = 1 || [t > 0]) }:tau", tl, F);
		cross_check("{ always (o1[t]:sbf = o1[t-1]:sbf"
			" && (i1[t]:sbf = 1 || [t > 0])) }:tau", tl, T);
		report_tally("step-0 demands", tl);
	}
}
