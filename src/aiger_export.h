// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

// AIGER export of an LTL specification over bv[1] streams.
//
// The skeleton of the LTL pipeline is written over atom propositions, two
// of which can stand for one bit (`o1[t] = 1`, `o1[t] = 0`), and over
// atoms that read the past. Here each atom becomes a Boolean formula over
// the bits of its streams, a past read `x[t-k]` a history output tied by
// `G(x <-> X h)`, a read `x[k]` of a fixed position an output that holds
// x's value at step k from step 0 on, and ltlsynt synthesizes and verifies a circuit
// (`--aiger --verify`). The bookkeeping outputs (history and fixed copies,
// step guards, step counters, temporal testers) are removed from it, which changes no other
// output, so the circuit shipped is the one Spot checked. The same game
// gives the Mealy machine a table program plays (bit_machine_of).

#ifndef __IDNI__TAU__AIGER_EXPORT_H__
#define __IDNI__TAU__AIGER_EXPORT_H__

#include <map>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace idni::tau_lang {

namespace aiger_detail {

// The bits read by the atoms, and the history and frozen outputs they
// need.
struct bits {
	std::set<std::string> inputs, outputs;
	std::map<std::pair<std::string, int_t>, std::string> history;
	/// x[k] at a fixed position: an output that holds x's value at step k
	/// at every step
	std::map<std::pair<std::string, int_t>, std::string> fixed;

	std::string at(const std::string& name, int_t back, bool input) {
		(input ? inputs : outputs).insert(name);
		if (back == 0) return name;
		for (int_t k = 1; k <= back; ++k)
			history.emplace(std::make_pair(name, k),
				"__h_" + name + "_" + std::to_string(k));
		return history.at({ name, back });
	}

	std::string at_step(const std::string& name, int_t k, bool input) {
		(input ? inputs : outputs).insert(name);
		return fixed.emplace(std::make_pair(name, k), "__f_" + name + "_"
			+ std::to_string(k)).first->second;
	}
};

// A bv[1] term as a Boolean formula in Spot syntax.
template <NodeType node>
result<std::string> bit_term(tref n, bits& b) {
	using tau = tree<node>;
	result<std::string> r;
	const auto& t = tau::get(n);
	auto sub = [&](size_t i) { return bit_term<node>(t[0].child(i), b); };
	auto bin = [&](const char* op) -> result<std::string> {
		result<std::string> r;
		TAU_TRY(auto a, sub(0));
		TAU_TRY(auto c, sub(1));
		return r.with_value("(" + a + " " + op + " " + c + ")");
	};
	if (t.is(tau::bf) && t.has_child()) {
		switch (t[0].get_type()) {
		case tau::bf_t: return r.with_value("1");
		case tau::bf_f: return r.with_value("0");
		case tau::bf_neg: {
			TAU_TRY(auto a, sub(0));
			return r.with_value("!" + a);
		}
		case tau::bf_and: return bin("&");
		case tau::bf_or: return bin("|");
		case tau::bf_xor: case tau::bf_add: case tau::bf_sub: return bin("^");
		case tau::bf_parenthesis: return sub(0);
		case tau::variable: {
			tref v = t.first();
			if (!tau::get(v).child_is(tau::io_var)) break;
			if (is_io_initial<node>(v)) {
				const int_t k = get_io_time_point<node>(v);
				const bool in = tau::get(v).is_input_variable();
				// the copy is chosen at step 0, before a later input
				// is read
				if (in && k > 0) return r.with_error(
					code::unsupported_operation,
					"the AIGER export does not take an input at a "
					"fixed position after step 0",
					{{label::value, truncate_for_message(TAU_TO_STR(n))}});
				return r.with_value(b.at_step(get_var_name<node>(v), k,
					in));
			}
			return r.with_value(b.at(get_var_name<node>(v),
				get_io_var_shift<node>(v),
				tau::get(v).is_input_variable()));
		}
		case tau::ba_constant: {
			std::string s = TAU_TO_STR(t.first());
			if (s.find('1') != std::string::npos && s.find('0')
				== std::string::npos) return r.with_value("1");
			if (s.find('0') != std::string::npos && s.find('1')
				== std::string::npos) return r.with_value("0");
			break;
		}
		default: break;
		}
	}
	return r.with_error(code::unsupported_operation,
		"the AIGER export takes atoms over bits only (=, !=, <, <=, >, "
		">= of terms with ', &, |, + and the constants 0 and 1)",
		{{label::value, truncate_for_message(TAU_TO_STR(n))}});
}

// A comparison of bv[1] terms as a Boolean formula.
template <NodeType node>
result<std::string> bit_atom(tref atom, bits& b) {
	using tau = tree<node>;
	result<std::string> r;
	const auto& t = tau::get(atom);
	if (!t.has_child() || t[0].children_size() != 2)
		return r.with_error(code::unsupported_operation,
			"the AIGER export takes comparisons of bits only",
			{{label::value, truncate_for_message(TAU_TO_STR(atom))}});
	const auto& c = t[0];
	TAU_TRY(auto x, bit_term<node>(c.child(0), b));
	TAU_TRY(auto y, bit_term<node>(c.child(1), b));
	auto f = [&]() -> std::optional<std::string> {
		switch (c.get_type()) {
		case tau::bf_eq:   return "(" + x + " <-> " + y + ")";
		case tau::bf_neq:  return "!(" + x + " <-> " + y + ")";
		case tau::bf_lt:   case tau::bf_ngteq: return "(!" + x + " & " + y + ")";
		case tau::bf_lteq: case tau::bf_ngt:   return "(!" + x + " | " + y + ")";
		case tau::bf_gt:   case tau::bf_nlteq: return "(" + x + " & !" + y + ")";
		case tau::bf_gteq: case tau::bf_nlt:   return "(" + x + " | !" + y + ")";
		default: return std::nullopt;
		}
	}();
	if (!f) return r.with_error(code::unsupported_operation,
		"the AIGER export takes comparisons of bits only",
		{{label::value, truncate_for_message(TAU_TO_STR(atom))}});
	return r.with_value(*f);
}

// The circuit with only the outputs named in `keep`, in their order.
inline result<std::string> keep_outputs(const std::string& aag,
	const std::set<std::string>& keep)
{
	result<std::string> r;
	std::istringstream in(aag);
	std::string header;
	std::getline(in, header);
	std::istringstream hs(header);
	std::string tag;
	size_t m, ni, nl, no, na;
	if (!(hs >> tag >> m >> ni >> nl >> no >> na) || tag != "aag")
		return r.with_error(code::solver_error,
			"ltlsynt printed no AIGER circuit",
			{{label::value, truncate_for_message(header)}});
	std::vector<std::string> lines;
	for (std::string l; std::getline(in, l); ) lines.push_back(l);
	if (lines.size() < ni + nl + no + na)
		return r.with_error(code::solver_error,
			"the AIGER circuit of ltlsynt is truncated");
	std::vector<std::string> outs(lines.begin() + (long)(ni + nl),
		lines.begin() + (long)(ni + nl + no));
	std::vector<std::string> names(no);
	std::vector<std::string> symbols, rest;
	for (size_t k = ni + nl + no + na; k < lines.size(); ++k) {
		const std::string& l = lines[k];
		size_t sp = l.find(' ');
		if (!l.empty() && l[0] == 'o' && sp != std::string::npos) {
			size_t idx = std::stoul(l.substr(1, sp - 1));
			if (idx < no) names[idx] = l.substr(sp + 1);
		} else if (!l.empty() && (l[0] == 'i' || l[0] == 'l'))
			symbols.push_back(l);
		else rest.push_back(l);
	}
	std::vector<size_t> kept;
	for (size_t k = 0; k < no; ++k) if (keep.contains(names[k])) kept.push_back(k);
	std::ostringstream out;
	out << "aag " << m << " " << ni << " " << nl << " " << kept.size() << " "
		<< na << "\n";
	for (size_t k = 0; k < ni + nl; ++k) out << lines[k] << "\n";
	for (size_t k : kept) out << outs[k] << "\n";
	for (size_t k = ni + nl + no; k < ni + nl + no + na; ++k)
		out << lines[k] << "\n";
	for (const auto& s : symbols) out << s << "\n";
	for (size_t j = 0; j < kept.size(); ++j)
		out << "o" << j << " " << names[kept[j]] << "\n";
	for (const auto& s : rest) out << s << "\n";
	return r.with_value(out.str());
}

// ltlsynt's verified circuit for formula, or nullopt when it is
// unrealizable.
inline result<std::optional<std::string>> synthesize_aiger(
	const std::string& formula, const std::vector<std::string>& ins,
	const std::vector<std::string>& outs, int timeout_sec,
	synthesis_options opts)
{
	result<std::optional<std::string>> r;
	opts.aiger = true;
	TAU_TRY(auto v, synthesize(formula, ins, outs, timeout_sec, opts));
	if (!v.realizable) return r.with_value(std::nullopt);
	const std::string verified = "Circuit was verified";
	auto pos = v.hoa.find(verified);
	if (pos == std::string::npos)
		return r.with_error(code::solver_error,
			"ltlsynt did not verify the AIGER circuit",
			{{label::value, truncate_for_message(v.hoa)}});
	return r.with_value(v.hoa.substr(0, pos));
}

// The skeleton of a formula over the bits of its streams, with the names
// ltlsynt reads and the streams a controller drives.
struct bit_game {
	std::string formula;
	std::vector<std::string> ins, outs;
	/// the output streams, the outputs a controller keeps
	std::set<std::string> streams;
};

template <NodeType node>
result<bit_game> bit_game_of(tref fm) {
	using tau = tree<node>;
	result<bit_game> r;
	if (ltl_finite_semantics())
		return r.with_error(code::unsupported_operation,
			"a circuit follows a strategy over infinite traces, so it "
			"needs ltl-semantics infinite");
	if (has_ctl_star_operators<node>(fm))
		return r.with_error(code::unsupported_operation,
			"the AIGER export does not take CTL* operators: a circuit "
			"is one linear-time controller, and A/E quantify over its "
			"runs");
	for (tref v : tau::get(fm).select_top(is_child<node, tau::io_var>)) {
		if (get_var_name<node>(v) == "u")
			return r.with_error(code::unsupported_operation,
				"the AIGER export does not take the update stream u");
		TAU_TRY(auto tname, get_ba_type_name<node>(find_ba_type<node>(v)));
		if (tname != ":bv[1]")
			return r.with_error(code::unsupported_operation,
				"the AIGER export takes bv[1] streams only",
				{{label::name, get_var_name<node>(v)},
				 {label::type_name, tname}});
	}
	ltl_aba_solution<node> partial;
	TAU_TRY(auto solved, solve_ltl_aba<node>(fm, &partial));
	const auto& sol = solved ? *solved : partial;

	bits b;
	// bit formulas are exact, so the game needs none of the consistency
	// constraints and shift chains ltlsynt reads over atom propositions
	std::string skel = sol.game_skeleton.empty() ? sol.skeleton
		: sol.game_skeleton;
	std::set<std::string> atom_props;
	for (const auto& [atom, prop] : sol.atoms) {
		TAU_TRY(auto f, bit_atom<node>(atom, b));
		atom_props.insert(prop);
		skel = std::regex_replace(skel,
			std::regex("\\b" + prop + "\\b"), "(" + f + ")");
	}
	for (const auto& p : sol.input_props)
		if (!atom_props.contains(p))
			return r.with_error(code::unsupported_operation,
				"the AIGER export does not take this specification: "
				"its skeleton has an input that is no stream",
				{{label::name, p}});
	for (const auto& [key, h] : b.history) {
		const auto& [name, k] = key;
		const std::string prev = k == 1 ? name : b.history.at({ name, k - 1 });
		skel = "(" + skel + ") & G(" + prev + " <-> X(" + h + "))";
	}
	for (const auto& [key, f] : b.fixed) {
		const auto& [name, k] = key;
		std::string at_k = "(" + f + " <-> " + name + ")";
		for (int_t j = 0; j < k; ++j) at_k = "X" + at_k;
		skel = "(" + skel + ") & " + at_k + " & G(" + f + " <-> X(" + f
			+ "))";
	}
	bit_game g;
	g.formula = std::move(skel);
	g.ins.assign(b.inputs.begin(), b.inputs.end());
	std::set<std::string> outset(b.outputs.begin(), b.outputs.end());
	for (const auto& [_, h] : b.history) outset.insert(h);
	for (const auto& [_, f] : b.fixed) outset.insert(f);
	for (const auto& p : sol.output_props)
		if (!atom_props.contains(p)) outset.insert(p);
	// a stream no atom reads is still an output of the circuit
	for (tref v : tau::get(fm).select_top(is_child<node, tau::io_var>))
		if (!tau::get(v).is_input_variable()) {
			outset.insert(get_var_name<node>(v));
			g.streams.insert(get_var_name<node>(v));
		} else if (!b.inputs.contains(get_var_name<node>(v)))
			g.ins.push_back(get_var_name<node>(v)), b.inputs.insert(
				get_var_name<node>(v));
	g.outs.assign(outset.begin(), outset.end());
	return r.with_value(std::move(g));
}

// An error unless `realizable` gives the verdict ltlsynt gave on the bit
// game.
template <NodeType node>
result<bool> same_verdict(tref fm, bool realizable) {
	result<bool> r;
	TAU_TRY(bool tau_real, api<node>::realizable(fm));
	if (tau_real != realizable)
		return r.with_error(code::internal_error,
			"the AIGER export and `realizable` disagree on this "
			"specification; nothing is emitted",
			{{label::value, tau_real ? "realizable says T"
				: "realizable says F"}});
	if (!realizable) return r.with_error(code::unsat,
		"the specification is unrealizable: there is no circuit");
	return r.with_value(true);
}

} // namespace aiger_detail

/**
 * @brief The AIGER circuit (ASCII, `aag`) of a controller for @p fm, a
 * prepared formula whose streams are all bv[1].
 *
 * The inputs and outputs of the circuit are the input and output streams,
 * by name. The circuit is a correct controller, not the one `run` executes:
 * where the strategy leaves an output free, `run` picks it per step and the
 * circuit fixes it.
 * @return The circuit; an error for a formula outside the fragment (a
 * non-bv[1] stream, an input at a fixed position after step 0 under a
 * temporal operator, a CTL* operator, the update stream, an atom that is
 * not a comparison of bits), for an unrealizable formula, and when
 * ltlsynt's verdict on the circuit's formula differs from the one
 * `realizable` gives.
 */
template <NodeType node>
result<std::string> aiger_of(tref fm) {
	using namespace aiger_detail;
	result<std::string> r;
	TAU_TRY(auto g, bit_game_of<node>(fm));
	TAU_TRY(auto circuit, synthesize_aiger(g.formula, g.ins, g.outs,
		ltl_timeout_sec(), ltlsynt_options()));
	TAU_TRY(bool agreed, same_verdict<node>(fm, circuit.has_value()));
	(void)agreed;
	return keep_outputs(*circuit, g.streams);
}

/**
 * @brief The Mealy machine ltlsynt synthesizes and verifies for the bit
 * game of @p fm (see aiger_of), as a solution whose atoms are the streams:
 * `x[t] = 1` for each stream x, with the stream's name as its proposition.
 *
 * The history copies and the bookkeeping of the bit game are propositions
 * of the machine only, so a table of the solution keeps them as its state.
 * Like the circuit, the machine is a controller of @p fm, not the strategy
 * `run` executes.
 * @return The solution; the errors of aiger_of, and an error when Spot
 * does not verify the machine.
 */
template <NodeType node>
result<ltl_aba_solution<node>> bit_machine_of(tref fm) {
	using tau = tree<node>;
	using namespace aiger_detail;
	result<ltl_aba_solution<node>> r;
	TAU_TRY(auto g, bit_game_of<node>(fm));
	auto opts = ltlsynt_options();
	opts.verify = true;
	TAU_TRY(auto v, synthesize(g.formula, g.ins, g.outs, ltl_timeout_sec(),
		opts));
	TAU_TRY(bool agreed, same_verdict<node>(fm, v.realizable));
	(void)agreed;
	const std::string verified = "Strategy was verified";
	auto pos = v.hoa.find(verified);
	if (pos == std::string::npos)
		return r.with_error(code::solver_error,
			"ltlsynt did not verify the strategy",
			{{label::value, truncate_for_message(v.hoa)}});
	ltl_aba_solution<node> sol;
	TAU_TRY(sol.aut, parse_hoa(v.hoa.substr(0, v.hoa.rfind("/*", pos))));
	std::set<std::string> seen;
	for (tref x : tau::get(fm).select_top(is_child<node, tau::io_var>)) {
		const std::string& name = get_var_name<node>(x);
		if (!seen.insert(name).second) continue;
		const size_t tid = find_ba_type<node>(x);
		const bool in = tau::get(x).is_input_variable();
		tref var = in
			? tau::build_in_var_at_t(build_var_name<node>(name), tid)
			: tau::build_out_var_at_t(build_var_name<node>(name), tid);
		sol.atoms.emplace_back(tau::build_bf_eq(var,
			build_bf_t_type<node>(tid)), name);
		(in ? sol.input_props : sol.output_props).push_back(name);
	}
	return r.with_value(std::move(sol));
}

} // namespace idni::tau_lang

#endif // __IDNI__TAU__AIGER_EXPORT_H__
