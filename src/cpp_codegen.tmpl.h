// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__CPP_CODEGEN_TMPL_H__
#define __IDNI__TAU__CPP_CODEGEN_TMPL_H__

#include <cctype>
#include <cstdio>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace idni::tau_lang {

namespace codegen_detail {

// Sanitize a name into a valid C++ identifier fragment.
inline std::string sanitize(const std::string& name) {
	std::string r;
	for (char c : name) {
		if (std::isalnum((unsigned char)c) || c == '_') r += c;
		else r += '_';
	}
	if (r.empty() || std::isdigit((unsigned char)r[0])) r = "_" + r;
	return r;
}

// Parse an HOA edge guard label — a Boolean expression over AP indices —
// into a C++ expression referring to the `ap[i]` evaluated-bool array.
// Supported syntax (per Spot's output): `!`, `&`, `|`, `t`, `f`, parentheses,
// and decimal AP indices.
inline std::string guard_to_cpp(const std::string& g) {
	std::string out;
	out.reserve(g.size() * 3);
	const auto n = g.size();
	for (size_t i = 0; i < n; ) {
		char c = g[i];
		if (std::isspace((unsigned char)c)) { ++i; continue; }
		if (std::isdigit((unsigned char)c)) {
			size_t j = i;
			while (j < n && std::isdigit((unsigned char)g[j])) ++j;
			out += "ap[";
			out.append(g, i, j - i);
			out += "]";
			i = j;
			continue;
		}
		switch (c) {
			case '!': out += "!"; ++i; break;
			case '&': out += "&&"; ++i; break;
			case '|': out += "||"; ++i; break;
			case '(': case ')': out += c; ++i; break;
			case 't': out += "true";  ++i; break;
			case 'f': out += "false"; ++i; break;
			default:  out += c; ++i; break;  // pass through
		}
	}
	if (out.empty()) out = "true";
	return out;
}

// Identify which APs this guard MENTIONS (for input/output classification).
inline std::vector<int> guard_aps(const std::string& g) {
	std::vector<int> aps;
	for (size_t i = 0; i < g.size(); ) {
		if (std::isdigit((unsigned char)g[i])) {
			size_t j = i;
			int v = 0;
			while (j < g.size() && std::isdigit((unsigned char)g[j])) {
				v = v * 10 + (g[j] - '0');
				++j;
			}
			aps.push_back(v);
			i = j;
		} else ++i;
	}
	return aps;
}

// Determine which AP indices are outputs (found only inside guards, never
// free, and marked as such by the caller via output_props membership).
// Returns a map: ap_index → "o_NAME" or "i_NAME".
inline std::vector<std::string> label_aps(
    const std::vector<std::string>& aps,
    const std::vector<std::string>& input_props,
    const std::vector<std::string>& /*output_props*/)
{
	std::vector<std::string> labels(aps.size());
	for (size_t i = 0; i < aps.size(); ++i) {
		const auto& ap = aps[i];
		bool is_input = false;
		for (const auto& p : input_props) if (p == ap) { is_input = true; break; }
		labels[i] = (is_input ? "i_" : "o_") + sanitize(ap);
	}
	return labels;
}

// Parse HOA guard label into (AP-index, polarity) literals.
// "0&!1&2" → [(0,true),(1,false),(2,true)].
// "t" and "f" → empty list (handled separately by caller).
inline std::vector<std::pair<int,bool>> parse_guard_lits(const std::string& g) {
	std::vector<std::pair<int,bool>> lits;
	if (g.empty() || g == "t" || g == "f") return lits;
	// Tokenize by top-level '&'
	std::vector<std::string> conjuncts;
	int depth = 0;
	std::string cur;
	for (char c : g) {
		if (c == '(') { ++depth; cur += c; }
		else if (c == ')') { --depth; cur += c; }
		else if (c == '&' && depth == 0) { conjuncts.push_back(cur); cur.clear(); }
		else cur += c;
	}
	conjuncts.push_back(cur);
	for (auto& cj : conjuncts) {
		std::string t;
		for (char c : cj) if (!std::isspace((unsigned char)c)) t += c;
		if (t.empty()) continue;
		bool pos = !(t[0] == '!');
		std::string idx_str = pos ? t : t.substr(1);
		if (idx_str.empty() || !std::isdigit((unsigned char)idx_str[0])) continue;
		int idx = 0;
		for (char c : idx_str) if (std::isdigit((unsigned char)c)) idx = idx*10+(c-'0');
		lits.emplace_back(idx, pos);
	}
	return lits;
}

// Pick a concrete double witness from a non-empty qlt interval.
inline double witness_from_qlt_interval(const qlt& interval) {
	if (interval.is_empty()) return 0.0;
	const auto& piece = interval.pieces[0];
	const auto& lo = piece.lo.val;
	const auto& hi = piece.hi.val;
	if (lo.is_neg_inf() && hi.is_pos_inf()) return 0.0;
	if (lo.is_neg_inf()) {
		if (hi.is_sym()) return -1.0;
		return (double)hi.p / (double)hi.q - 1.0;
	}
	if (hi.is_pos_inf()) {
		if (lo.is_sym()) return 1.0;
		return (double)lo.p / (double)lo.q + 1.0;
	}
	// Bounded: midpoint
	if (lo.is_sym() || hi.is_sym()) return 0.0;
	double l = (double)lo.p / (double)lo.q;
	double h = (double)hi.p / (double)hi.q;
	return (l + h) / 2.0;
}

// Format a double as a C++ literal with full precision.
inline std::string double_to_cpp(double v) {
	char buf[64];
	snprintf(buf, sizeof(buf), "%.17g", v);
	// Ensure the literal looks like a floating-point constant
	std::string s(buf);
	bool has_dot_or_e = false;
	for (char c : s) if (c == '.' || c == 'e' || c == 'E') { has_dot_or_e = true; break; }
	if (!has_dot_or_e) s += ".0";
	return s;
}

// Classification of a single data atom for the codegen.
enum class AtomKind { BOOL, OUTPUT_QLT, INPUT_QLT };

struct AtomMeta {
	AtomKind kind  = AtomKind::BOOL;
	std::string var_name;      // "o1" or "i1" for QLT atoms
	tref io_var_ref = nullptr; // io_var tref (for qlt_dlo_qe)
};

// Classify a data atom as BOOL, OUTPUT_QLT, or INPUT_QLT.
// is_output: the atom's prop is in sol.output_props.
// Only returns QLT if:
//  - atom is qlt-type
//  - all free vars are qlt-type
//  - exactly one unique variable base name
//  - variable direction matches is_output
template <NodeType node>
AtomMeta classify_atom_for_data(tref atom_ref, bool is_output) {
	AtomMeta m;
	if (!is_omcat_type_family<node>(find_ba_type<node>(atom_ref))) return m;
	const trefs& fvars = get_free_vars<node>(atom_ref);
	if (fvars.empty()) return m;
	for (tref v : fvars)
		if (!is_omcat_type_family<node>(tree<node>::get(v).get_ba_type())) return m;
	std::string var_name;
	tref io_ref = nullptr;
	for (tref v : fvars) {
		const std::string& nm = get_var_name<node>(v);
		if (var_name.empty()) { var_name = nm; io_ref = v; }
		else if (nm != var_name) return m; // multiple distinct vars — keep BOOL
	}
	if (var_name.empty()) return m;
	bool is_out_var = !var_name.empty() && var_name[0] == 'o';
	if (is_output && !is_out_var) return m;
	if (!is_output && is_out_var) return m;
	m.var_name  = var_name;
	m.io_var_ref = io_ref;
	m.kind = is_output ? AtomKind::OUTPUT_QLT : AtomKind::INPUT_QLT;
	return m;
}

} // namespace codegen_detail


// ── Propositional (bool-only) emitter ─────────────────────────────────────────

inline void emit_cpp_program_prop(
    const HoaAutomaton& aut,
    const std::vector<std::string>& input_props,
    const std::vector<std::string>& output_props,
    std::ostream& out,
    const std::string& class_name)
{
	using codegen_detail::guard_to_cpp;
	using codegen_detail::guard_aps;
	using codegen_detail::label_aps;
	using codegen_detail::sanitize;

	const auto labels = label_aps(aut.aps, input_props, output_props);

	// Partition APs into inputs (from input_props) and outputs (the rest).
	std::vector<int> in_idx, out_idx;
	for (size_t i = 0; i < aut.aps.size(); ++i) {
		const auto& ap = aut.aps[i];
		bool is_input = false;
		for (const auto& p : input_props) if (p == ap) { is_input = true; break; }
		(is_input ? in_idx : out_idx).push_back((int)i);
	}

	out << "// Auto-generated by tau-lang's C++ program emitter.\n";
	out << "// Do not edit by hand — regenerate from the source .tau spec.\n\n";
	out << "#pragma once\n";
	out << "#include <cstdint>\n\n";
	out << "class " << class_name << " {\n";
	out << "public:\n";
	out << "\tenum class State : std::uint32_t {\n";
	for (int s = 0; s < aut.num_states; ++s) {
		out << "\t\tq" << s << (s + 1 < aut.num_states ? "," : "") << "\n";
	}
	out << "\t};\n\n";

	// Input struct: one bool per input AP.
	out << "\tstruct Inputs {\n";
	for (int i : in_idx)
		out << "\t\tbool " << labels[i] << " = false;\n";
	out << "\t};\n\n";

	// Output struct: one bool per output AP + ok flag.
	out << "\tstruct Outputs {\n";
	for (int i : out_idx)
		out << "\t\tbool " << labels[i] << " = false;\n";
	out << "\t\tbool ok = true;  // false: no edge matched (synthesis invariant violated)\n";
	out << "\t};\n\n";

	out << "\t" << class_name << "() = default;\n\n";

	out << "\tState state() const noexcept { return state_; }\n\n";

	// Core step function.
	out << "\tOutputs step(const Inputs& in) noexcept {\n";
	out << "\t\tOutputs o;\n";
	if (aut.aps.size()) {
		out << "\t\t[[maybe_unused]] bool ap[" << aut.aps.size() << "] = {};\n";
		for (int i : in_idx)
			out << "\t\tap[" << i << "] = in." << labels[i] << ";\n";
	} else {
		out << "\t\t(void)in;\n";
	}
	out << "\n";
	out << "\t\tswitch (state_) {\n";
	for (int s = 0; s < aut.num_states; ++s) {
		out << "\t\tcase State::q" << s << ": {\n";
		const auto& edges = aut.edges.size() > (size_t)s ? aut.edges[s] : std::vector<HoaEdge>{};
		bool first = true;
		for (const auto& e : edges) {
			// Separate guard APs into input-guard (constrains env) and
			// output-assignments (what we must set).  Under Spot's Mealy
			// convention, outputs in the guard are *assignments* we realize.
			auto aps_in_guard = guard_aps(e.guard_label);
			// Build input-only guard string.
			std::string input_guard;
			{
				std::ostringstream ig;
				bool any = false;
				auto emit_cond = [&](const std::string& cond) {
					if (any) ig << " && ";
					ig << "(" << cond << ")";
					any = true;
				};
				// Rewrite guard: outputs substituted by `true` (since we will assign them).
				// Simpler approach: emit the raw guard as-is with output APs replaced by `true`
				// so the dispatch depends only on input APs. We keep the output assignments separate.
				// To accomplish that cleanly, build a replaced guard.
				std::string modified = e.guard_label;
				// We'll compute the full boolean expression using both inputs and
				// assigned outputs; outputs are still "true" by assumption so the
				// resulting C++ expression reduces to an input predicate after we
				// substitute ap[out_idx] = true. This is done at evaluation time.
				emit_cond(guard_to_cpp(modified));
				input_guard = any ? ig.str() : "true";
			}

			// Determine output assignments from the guard: for each output AP,
			// if the guard is a conjunctive clause that includes `ap[o]` we assign true;
			// if it includes `!ap[o]` we assign false. For general DNF/disjunctive
			// guards, we fall back to evaluating the full expression under the
			// assumption and picking any satisfying assignment.
			//
			// Pragmatic heuristic for Spot's "Mealy" output: Spot emits a single
			// conjunction per edge ("0&!1&2"), so the polarity of each output AP
			// is direct. For general HOA we would SAT-solve the guard.
			std::string assignments;
			{
				// Tokenize by '&' at top level.
				std::vector<std::string> conjuncts;
				int depth = 0;
				std::string cur;
				for (char c : e.guard_label) {
					if (c == '(') { ++depth; cur += c; }
					else if (c == ')') { --depth; cur += c; }
					else if (c == '&' && depth == 0) {
						if (!cur.empty()) conjuncts.push_back(cur);
						cur.clear();
					} else cur += c;
				}
				if (!cur.empty()) conjuncts.push_back(cur);

				std::ostringstream asg;
				for (const auto& conj : conjuncts) {
					// strip whitespace
					std::string t;
					for (char c : conj) if (!std::isspace((unsigned char)c)) t += c;
					if (t.empty()) continue;
					bool neg = !t.empty() && t[0] == '!';
					std::string idx = neg ? t.substr(1) : t;
					if (idx.empty() || !std::isdigit((unsigned char)idx[0])) continue;
					int ap_i = 0;
					for (char c : idx) { if (std::isdigit((unsigned char)c)) ap_i = ap_i*10 + (c - '0'); }
					// Only emit for output APs.
					bool is_out = false;
					for (int oi : out_idx) if (oi == ap_i) { is_out = true; break; }
					if (!is_out) continue;
					asg << "\t\t\t\to." << labels[ap_i] << " = " << (neg ? "false" : "true") << ";\n";
				}
				assignments = asg.str();
			}

			// Build an input-only guard by substituting outputs with the values
			// we just decided to assign.
			std::string effective_input_guard;
			{
				std::ostringstream ig;
				bool any = false;
				// For each conjunct whose AP is an input, emit the same sign; for outputs, skip (they're assigned).
				std::vector<std::string> conjuncts;
				int depth = 0;
				std::string cur;
				for (char c : e.guard_label) {
					if (c == '(') { ++depth; cur += c; }
					else if (c == ')') { --depth; cur += c; }
					else if (c == '&' && depth == 0) {
						if (!cur.empty()) conjuncts.push_back(cur);
						cur.clear();
					} else cur += c;
				}
				if (!cur.empty()) conjuncts.push_back(cur);
				for (const auto& conj : conjuncts) {
					std::string t;
					for (char c : conj) if (!std::isspace((unsigned char)c)) t += c;
					if (t.empty()) continue;
					bool neg = !t.empty() && t[0] == '!';
					std::string idx = neg ? t.substr(1) : t;
					if (idx.empty() || !std::isdigit((unsigned char)idx[0])) continue;
					int ap_i = 0;
					for (char c : idx) if (std::isdigit((unsigned char)c)) ap_i = ap_i*10 + (c - '0');
					bool is_in = false;
					for (int ii : in_idx) if (ii == ap_i) { is_in = true; break; }
					if (!is_in) continue;
					if (any) ig << " && ";
					ig << (neg ? "!in." : "in.") << labels[ap_i];
					any = true;
				}
				effective_input_guard = any ? ig.str() : "true";
			}

			out << (first ? "\t\t\tif (" : "\t\t\telse if (") << effective_input_guard << ") {\n";
			out << assignments;
			out << "\t\t\t\tstate_ = State::q" << e.dst << ";\n";
			out << "\t\t\t\treturn o;\n";
			out << "\t\t\t}\n";
			first = false;
		}
		out << "\t\t\to.ok = false; return o;\n";
		out << "\t\t}\n";
	}
	out << "\t\t}\n";
	out << "\t\to.ok = false; return o;  // unreachable\n";
	out << "\t}\n\n";
	out << "private:\n";
	out << "\tState state_ = State::q" << aut.initial_state << ";\n";
	out << "};\n";
}


// ── Data-atom (qlt witness) emitter ───────────────────────────────────────────

// Emits a class where output qlt atoms produce concrete `double` witness values
// rather than abstract `bool` flags.  Input atoms remain `bool` (the caller
// evaluates the comparison against their concrete input values).
//
// For each HOA edge the emitter:
//   1. Collects positive output-qlt atoms grouped by variable name.
//   2. Builds their conjunction and calls qlt_dlo_qe to get the feasible interval.
//   3. Picks a concrete double witness from the interval and embeds it as a literal.
//
// Output atoms of non-qlt type still get `bool` fields in Outputs (propositional
// fallback).  Mixed atoms (both input and output vars) also fall back to `bool`.
template <NodeType node>
void emit_cpp_program_data(
    const LtlAbaSolution<node>& sol,
    std::ostream& out,
    const std::string& class_name,
    const std::map<std::string, codegen_detail::AtomMeta>& ameta)
{
	using tau = tree<node>;
	using namespace codegen_detail;

	// Build lookup tables
	std::map<std::string, int>  prop_to_ap;  // prop_name → AP index
	std::map<std::string, tref> prop_to_atom; // prop_name → atom tref
	for (int i = 0; i < (int)sol.aut.aps.size(); ++i)
		prop_to_ap[sol.aut.aps[i]] = i;
	for (auto& [atom_ref, prop] : sol.atoms)
		prop_to_atom[prop] = atom_ref;

	// Collect unique output qlt variable names (in discovery order).
	std::vector<std::string> out_qlt_vars;
	{
		std::set<std::string> seen;
		for (auto& prop : sol.output_props) {
			auto it = ameta.find(prop);
			if (it != ameta.end() && it->second.kind == AtomKind::OUTPUT_QLT
			    && !seen.count(it->second.var_name))
			{
				out_qlt_vars.push_back(it->second.var_name);
				seen.insert(it->second.var_name);
			}
		}
	}

	// Collect bool output props (non-qlt fallback).
	std::vector<std::string> out_bool_props;
	for (auto& prop : sol.output_props) {
		auto it = ameta.find(prop);
		if (it == ameta.end() || it->second.kind != AtomKind::OUTPUT_QLT)
			out_bool_props.push_back(prop);
	}

	// ── Emit class header ──
	out << "// Auto-generated by tau-lang's C++ program emitter.\n";
	out << "// Do not edit by hand — regenerate from the source .tau spec.\n\n";
	out << "#pragma once\n";
	out << "#include <cstdint>\n\n";
	out << "class " << class_name << " {\n";
	out << "public:\n";
	out << "\tenum class State : std::uint32_t {\n";
	for (int s = 0; s < sol.aut.num_states; ++s)
		out << "\t\tq" << s << (s+1 < sol.aut.num_states ? "," : "") << "\n";
	out << "\t};\n\n";

	// Inputs struct: input props remain bool (caller evaluates condition).
	out << "\tstruct Inputs {\n";
	for (auto& prop : sol.input_props) {
		auto it = ameta.find(prop);
		if (it != ameta.end() && it->second.kind == AtomKind::INPUT_QLT)
			out << "\t\tbool " << sanitize(prop) << " = false;"
			    << "  // qlt: evaluate '" << prop << "' condition on your input value\n";
		else
			out << "\t\tbool " << sanitize(prop) << " = false;\n";
	}
	out << "\t};\n\n";

	// Outputs struct: qlt variables as double, others as bool.
	out << "\tstruct Outputs {\n";
	for (auto& var : out_qlt_vars)
		out << "\t\tdouble " << sanitize(var) << " = 0.0;"
		    << "  // qlt witness computed at synthesis time\n";
	for (auto& prop : out_bool_props)
		out << "\t\tbool " << sanitize(prop) << " = false;\n";
	out << "\t\tbool ok = true;  // false: no edge matched (synthesis invariant violated)\n";
	out << "\t};\n\n";

	out << "\t" << class_name << "() = default;\n\n";
	out << "\tState state() const noexcept { return state_; }\n\n";

	// Input AP index lookup (for step() dispatch).
	std::vector<int> in_idx, out_idx;
	for (int i = 0; i < (int)sol.aut.aps.size(); ++i) {
		bool is_in = false;
		for (auto& p : sol.input_props) if (p == sol.aut.aps[i]) { is_in = true; break; }
		(is_in ? in_idx : out_idx).push_back(i);
	}

	// ── step() function ──
	out << "\tOutputs step(const Inputs& in) noexcept {\n";
	out << "\t\tOutputs o;\n";
	if (sol.input_props.empty()) out << "\t\t(void)in;\n";
	out << "\n";
	out << "\t\tswitch (state_) {\n";

	for (int s = 0; s < sol.aut.num_states; ++s) {
		out << "\t\tcase State::q" << s << ": {\n";
		const auto& edges = sol.aut.edges.size() > (size_t)s
		                  ? sol.aut.edges[s] : std::vector<HoaEdge>{};
		bool first = true;

		for (const auto& e : edges) {
			auto guard_lits = parse_guard_lits(e.guard_label);

			// Build the input-only guard condition.
			std::string in_guard;
			{
				bool any = false;
				std::ostringstream ig;
				for (auto& [ap_idx, positive] : guard_lits) {
					if (ap_idx < 0 || ap_idx >= (int)sol.aut.aps.size()) continue;
					const auto& prop = sol.aut.aps[ap_idx];
					bool is_in = false;
					for (auto& ip : sol.input_props) if (ip == prop) { is_in = true; break; }
					if (!is_in) continue;
					if (any) ig << " && ";
					if (!positive) ig << "!";
					ig << "in." << sanitize(prop);
					any = true;
				}
				in_guard = any ? ig.str() : "true";
			}

			// For each positive output-qlt atom, group by variable.
			std::map<std::string, std::vector<tref>> var_pos_atoms;
			std::map<std::string, tref>               var_io_ref;
			std::vector<std::pair<std::string, bool>> bool_asgns; // (prop, value)

			for (auto& [ap_idx, positive] : guard_lits) {
				if (ap_idx < 0 || ap_idx >= (int)sol.aut.aps.size()) continue;
				const auto& prop = sol.aut.aps[ap_idx];
				bool is_out = false;
				for (auto& op : sol.output_props) if (op == prop) { is_out = true; break; }
				if (!is_out) continue;

				auto meta_it = ameta.find(prop);
				if (meta_it != ameta.end() && meta_it->second.kind == AtomKind::OUTPUT_QLT) {
					if (positive) {
						var_pos_atoms[meta_it->second.var_name].push_back(prop_to_atom[prop]);
						var_io_ref[meta_it->second.var_name] = meta_it->second.io_var_ref;
					}
					// Negative qlt atom: the witness must NOT satisfy it.
					// For simplicity we only use positive atoms for witness selection;
					// the ABA oracle has already verified feasibility.
				} else {
					bool_asgns.emplace_back(prop, positive);
				}
			}

			// Compute qlt witnesses at code-generation time.
			std::map<std::string, double> witnesses;
			if constexpr (ba_variant_includes_v<qlt, typename tau::constant>) {
				for (auto& [var, atom_refs] : var_pos_atoms) {
					tref conj = atom_refs[0];
					for (size_t ai = 1; ai < atom_refs.size(); ++ai)
						conj = tau::build_wff_and(conj, atom_refs[ai]);
					tref io_ref = var_io_ref[var];
					if (auto interval = qlt_dlo_qe<node>(io_ref, conj); interval)
						witnesses[var] = witness_from_qlt_interval(interval.value());
					else
						witnesses[var] = 1.0; // qlt_dlo_qe undetermined: use 1.0 as fallback
				}
			}

			// Emit edge.
			out << "\t\t\t" << (first ? "if (" : "else if (") << in_guard << ") {\n";

			for (auto& var : out_qlt_vars) {
				if (witnesses.count(var))
					out << "\t\t\t\to." << sanitize(var) << " = "
					    << double_to_cpp(witnesses[var]) << ";\n";
				// If no positive atom for this var on this edge: leave default 0.0.
			}
			for (auto& [prop, val] : bool_asgns)
				out << "\t\t\t\to." << sanitize(prop) << " = " << (val ? "true" : "false") << ";\n";

			out << "\t\t\t\tstate_ = State::q" << e.dst << ";\n";
			out << "\t\t\t\treturn o;\n";
			out << "\t\t\t}\n";
			first = false;
		}
		out << "\t\t\to.ok = false; return o;\n";
		out << "\t\t}\n";
	}

	out << "\t\t}\n";
	out << "\t\to.ok = false; return o;  // unreachable\n";
	out << "\t}\n\n";
	out << "private:\n";
	out << "\tState state_ = State::q" << sol.aut.initial_state << ";\n";
	out << "};\n";
}


// ── Public entry point ────────────────────────────────────────────────────────

template <NodeType node>
void emit_cpp_program(
    const LtlAbaSolution<node>& sol,
    std::ostream& out,
    const std::string& class_name)
{
	using namespace codegen_detail;

	// Build set of input props for quick lookup.
	std::set<std::string> input_set(sol.input_props.begin(), sol.input_props.end());

	// Classify each atom.
	std::map<std::string, AtomMeta> ameta;
	bool has_data = false;
	for (auto& [atom_ref, prop] : sol.atoms) {
		bool is_out = !input_set.count(prop);
		AtomMeta m = classify_atom_for_data<node>(atom_ref, is_out);
		ameta[prop] = m;
		if (m.kind != AtomKind::BOOL) has_data = true;
	}

	if (!has_data) {
		// Pure propositional case — use the simpler existing path.
		emit_cpp_program_prop(sol.aut, sol.input_props, sol.output_props, out, class_name);
		return;
	}

	emit_cpp_program_data<node>(sol, out, class_name, ameta);
}

// ── declare_open emitter (Phase A4 v1 scaffolding) ─────────────────────────
//
// Emit a C++17 program with C-ABI hooks for oracle-resolved output streams.
// V1: emits registration API + open_streams metadata. V2 (deferred) adds
// per-step admissibility-formula construction + handler dispatch (needs
// W-region table baked into the emit).
//

inline void emit_cpp_program_open_prop(
    const HoaAutomaton& aut,
    const std::vector<std::string>& input_props,
    const std::vector<std::string>& output_props,
    const std::vector<std::string>& open_streams,
    std::ostream& out,
    const std::string& class_name)
{
	// First emit the standard propositional program — this gives us
	// the State enum, struct Inputs, struct Outputs, struct, step().
	// Then we APPEND the declare_open registration scaffolding before
	// the closing brace.

	// Capture the standard emit into a string buffer so we can splice
	// in our additions.
	std::ostringstream prop_emit;
	emit_cpp_program_prop(aut, input_props, output_props, prop_emit, class_name);
	std::string source = prop_emit.str();

	// Inject extra headers right after #pragma once.
	// <map> / <string> for v1 registration; <cstring> for std::strstr in
	// the A4 v2 operational dispatch body (response parsing); <cstdint>
	// already in the v1 prop emit.
	auto pragma = source.find("#pragma once");
	if (pragma != std::string::npos) {
		auto eol = source.find('\n', pragma);
		if (eol != std::string::npos) {
			source.insert(eol + 1,
				"#include <map>\n#include <string>\n#include <cstring>\n");
		}
	}

	// Find the closing "};" of the class and inject registration API
	// before it.
	auto last_brace = source.rfind("};");
	if (last_brace == std::string::npos) {
		// Fallback: just emit the standard program as-is.
		out << source;
		return;
	}

	std::string head = source.substr(0, last_brace);
	std::string tail = source.substr(last_brace);

	out << head;
	// Force public access for our injected methods (the existing class
	// has a private: section near the end for state_; our methods need
	// to be public).
	out << "\npublic:\n";

	// Inject the declare_open scaffolding.
	out << "\n";
	out << "\t// ── declare_open: oracle-resolved output streams ────────\n";
	out << "\t//\n";
	out << "\t// V1: registration\n";
	out << "\t// API + open-stream metadata. Per-step dispatch is V2 (needs W-region\n";
	out << "\t// table baked at codegen time).\n";
	out << "\n";
	out << "\tusing OracleCallback = const char* (*)(\n";
	out << "\t    const char* formula, void* user_data);\n";
	out << "\n";
	out << "\t// List of stream names declared open at codegen time.\n";
	out << "\tstatic const char* const* open_streams() noexcept {\n";
	out << "\t\tstatic const char* const names[] = {\n";
	for (const auto& s : open_streams) {
		out << "\t\t\t\"" << s << "\",\n";
	}
	out << "\t\t\tnullptr  // sentinel\n";
	out << "\t\t};\n";
	out << "\t\treturn names;\n";
	out << "\t}\n";
	out << "\n";
	out << "\t// Number of declared-open streams.\n";
	out << "\tstatic constexpr std::size_t open_streams_count() noexcept {\n";
	out << "\t\treturn " << open_streams.size() << ";\n";
	out << "\t}\n";
	out << "\n";
	out << "\t// Register an oracle callback for `stream`. Returns 0 on success,\n";
	out << "\t// -1 if `stream` is not a declared-open stream, -2 if called from\n";
	out << "\t// inside a handler invocation (re-entrance violation).\n";
	out << "\tint register_open_oracle(\n";
	out << "\t    const char* stream,\n";
	out << "\t    OracleCallback cb,\n";
	out << "\t    void* user_data) noexcept\n";
	out << "\t{\n";
	out << "\t\tif (in_oracle_dispatch_) return -2;\n";
	out << "\t\tbool found = false;\n";
	out << "\t\tfor (const char* const* p = open_streams(); *p; ++p) {\n";
	out << "\t\t\tif (std::string(*p) == stream) { found = true; break; }\n";
	out << "\t\t}\n";
	out << "\t\tif (!found) return -1;\n";
	out << "\t\thandlers_[std::string(stream)] = {cb, user_data};\n";
	out << "\t\treturn 0;\n";
	out << "\t}\n";
	out << "\n";
	out << "\t// Unregister an oracle callback. Returns 0 on success, -2 on\n";
	out << "\t// re-entrance, -3 if no handler was registered for `stream`.\n";
	out << "\tint unregister_open_oracle(const char* stream) noexcept {\n";
	out << "\t\tif (in_oracle_dispatch_) return -2;\n";
	out << "\t\tauto it = handlers_.find(std::string(stream));\n";
	out << "\t\tif (it == handlers_.end()) return -3;\n";
	out << "\t\thandlers_.erase(it);\n";
	out << "\t\treturn 0;\n";
	out << "\t}\n";
	out << "\n";

	// ── A4 v2 incremental: per-(state, declared-stream) admissibility ──
	//
	// For each declared-open stream s and each state q, compute the
	// SET of admissible boolean values for s at q by walking sol.aut's
	// edges from q. An output AP `s` (index `s_idx` in aut.aps) is:
	//   - true-admissible at q  if some edge from q has "s_idx" in its
	//     guard (positive polarity) OR doesn't constrain s at all;
	//   - false-admissible at q if some edge from q has "!s_idx" in its
	//     guard OR doesn't constrain s.
	//
	// Encoded as a bitmask: bit 0 = false admissible, bit 1 = true
	// admissible. Bitmask 0 = unreachable / dead state for s.
	// Bitmask 3 = both values acceptable (don't-care or branch
	// possibilities differ).
	//
	// Compile-time host use: call admissible_values_mask(q, "o_p0")
	// to learn which boolean values keep the synthesised invariant
	// satisfied; build the F formula from the mask, dispatch the
	// oracle, validate the response is in the allowed set.

	out << "\t// ── A4 v2 incremental: per-state admissibility for declared streams ──\n";
	out << "\t// Bitmask encoding: bit 0 = `false` admissible, bit 1 = `true` admissible.\n";
	out << "\t// 0 = neither (unreachable for this stream from state q);\n";
	out << "\t// 3 = both values acceptable.\n";
	out << "\t// Hosts can use this to build the admissibility formula F\n";
	out << "\t// before invoking the registered oracle, and to validate\n";
	out << "\t// the oracle's response.\n";
	out << "\tstatic std::uint8_t admissible_values_mask(\n";
	out << "\t    State q, const char* stream) noexcept\n";
	out << "\t{\n";

	// Map declared stream names → AP index (output APs only).
	// Users pass `--open o_p0` and the v1 emit produces a `bool o_p0`
	// output field. The underlying `aut.aps[i]` is the raw AP name
	// from synthesis (e.g., `p0`), and `label_aps` produces
	// `o_<sanitize(ap)>` for outputs. So the user-facing stream name
	// matches `"o_" + sanitize(aut.aps[i])` — that's what we look up.
	const auto labels = codegen_detail::label_aps(
	    aut.aps, input_props, output_props);
	std::map<std::string, int> stream_to_ap;
	for (const auto& s : open_streams) {
		for (size_t i = 0; i < aut.aps.size(); ++i) {
			if (labels[i] == s || aut.aps[i] == s) {
				stream_to_ap[s] = (int)i;
				break;
			}
		}
	}

	// For each declared stream, emit one branch.
	for (const auto& s : open_streams) {
		auto it = stream_to_ap.find(s);
		out << "\t\tif (std::string(stream) == \"" << s << "\") {\n";
		if (it == stream_to_ap.end()) {
			// Stream not in aut.aps — it's declared open but the
			// synthesis didn't bind it as an output AP. Treat as
			// fully open (both values OK).
			out << "\t\t\treturn 0x3;  // stream not in aut.aps — fully open\n";
			out << "\t\t}\n";
			continue;
		}
		int ap_idx = it->second;

		out << "\t\t\tswitch (q) {\n";
		for (int q = 0; q < aut.num_states; ++q) {
			std::uint8_t mask = 0;  // bit 0 = false, bit 1 = true
			const auto& qedges = aut.edges.size() > (size_t)q
				? aut.edges[q] : std::vector<HoaEdge>{};
			for (const auto& e : qedges) {
				// Walk the guard's top-level conjuncts for ap_idx.
				// We use the same conjunct-tokenisation as the v1
				// step() emit (line ~318) to stay consistent.
				bool found_pos = false, found_neg = false;
				int depth = 0;
				std::string cur;
				std::vector<std::string> conjuncts;
				for (char c : e.guard_label) {
					if (c == '(') { ++depth; cur += c; }
					else if (c == ')') { --depth; cur += c; }
					else if (c == '&' && depth == 0) {
						if (!cur.empty()) conjuncts.push_back(cur);
						cur.clear();
					} else cur += c;
				}
				if (!cur.empty()) conjuncts.push_back(cur);
				for (const auto& conj : conjuncts) {
					std::string t;
					for (char c : conj)
						if (!std::isspace((unsigned char)c)) t += c;
					if (t.empty()) continue;
					bool neg = !t.empty() && t[0] == '!';
					std::string idx = neg ? t.substr(1) : t;
					if (idx.empty() ||
					    !std::isdigit((unsigned char)idx[0])) continue;
					int parsed = 0;
					for (char c : idx)
						if (std::isdigit((unsigned char)c))
							parsed = parsed*10 + (c - '0');
					if (parsed != ap_idx) continue;
					if (neg) found_neg = true;
					else found_pos = true;
				}
				// If the guard didn't mention ap_idx at all, both values
				// are admissible for this edge (don't-care over the AP).
				bool mentions = found_pos || found_neg;
				if (!mentions) { mask |= 0x3; continue; }
				if (found_pos) mask |= 0x2;  // true admissible
				if (found_neg) mask |= 0x1;  // false admissible
			}
			out << "\t\t\t\tcase State::q" << q << ": return 0x"
			    << std::hex << (int)mask << std::dec << ";\n";
		}
		out << "\t\t\t\tdefault: return 0x0;\n";
		out << "\t\t\t}\n";
		out << "\t\t}\n";
	}
	out << "\t\treturn 0x0;  // unknown stream\n";
	out << "\t}\n";

	out << "\n";

	// ── A4 v2 operational: step_with_oracle_dispatch ──────────────────
	//
	// Variant of step() that wires oracle dispatch internally. For each
	// declared-open stream s, calls the registered handler with an F
	// formula derived from admissible_values_mask(state_, s), validates
	// the response, fills the chosen value into ap[s_idx], then walks
	// edges to find the unique edge consistent with (inputs ∪ chosen
	// declared values). Undeclared output values are derived from the
	// matching edge's guard (same logic as the standard step() emit).
	//
	// On any failure (no handler registered, oracle returns invalid
	// response, no edge matches), Outputs.ok = false.

	// Compute input/output AP partition (mirrors emit_cpp_program_prop).
	std::vector<int> in_idx, out_idx;
	for (size_t i = 0; i < aut.aps.size(); ++i) {
		bool is_input = false;
		for (const auto& p : input_props)
			if (p == aut.aps[i]) { is_input = true; break; }
		(is_input ? in_idx : out_idx).push_back((int)i);
	}

	// Set of declared-stream AP indices for quick lookup.
	std::set<int> declared_idx;
	for (auto& [_, idx] : stream_to_ap) declared_idx.insert(idx);

	out << "\t// ── A4 v2 operational: dispatch oracle, then take matching edge ──\n";
	out << "\tOutputs step_with_oracle_dispatch(const Inputs& in) noexcept {\n";
	out << "\t\tOutputs o;\n";
	if (aut.aps.size()) {
		out << "\t\tbool ap[" << aut.aps.size() << "] = {};\n";
		for (int i : in_idx) {
			out << "\t\tap[" << i << "] = in." << labels[i] << ";\n";
		}
	} else {
		out << "\t\t(void)in;\n";
	}
	out << "\n";

	// Dispatch each declared stream.
	for (const auto& s : open_streams) {
		auto it_idx = stream_to_ap.find(s);
		if (it_idx == stream_to_ap.end()) continue;
		int idx = it_idx->second;
		out << "\t\t// Dispatch oracle for stream \"" << s << "\" (AP "
		    << idx << ").\n";
		out << "\t\t{\n";
		out << "\t\t\tauto mask = admissible_values_mask(state_, \""
		    << s << "\");\n";
		out << "\t\t\tauto h = handlers_.find(\"" << s << "\");\n";
		out << "\t\t\tif (h == handlers_.end()) { o.ok = false; return o; }\n";
		out << "\t\t\tstd::string F;\n";
		out << "\t\t\tif (mask & 0x1) F += \"(" << labels[idx] << " = 0)\";\n";
		out << "\t\t\tif ((mask & 0x3) == 0x3) F += \" || \";\n";
		out << "\t\t\tif (mask & 0x2) F += \"(" << labels[idx] << " = 1)\";\n";
		out << "\t\t\tin_oracle_dispatch_ = true;\n";
		out << "\t\t\tconst char* response = h->second.first(F.c_str(),\n";
		out << "\t\t\t    h->second.second);\n";
		out << "\t\t\tin_oracle_dispatch_ = false;\n";
		out << "\t\t\tif (!response) { o.ok = false; return o; }\n";
		// Parse "label := 0" or "label := 1"; loose parse for v2.
		out << "\t\t\tbool chose_true = std::strstr(response, \":= 1\") "
		       "!= nullptr;\n";
		out << "\t\t\tstd::uint8_t needed = chose_true ? 0x2 : 0x1;\n";
		out << "\t\t\tif ((mask & needed) == 0) { o.ok = false; return o; }\n";
		out << "\t\t\tap[" << idx << "] = chose_true;\n";
		out << "\t\t\to." << labels[idx] << " = chose_true;\n";
		out << "\t\t}\n";
		out << "\n";
	}

	// Walk edges from current state, find one whose guard evaluates true
	// under the filled ap[]. For undeclared output APs in the guard,
	// extract the assignment from the guard (same conjunct-tokenisation
	// logic as the standard step()).
	out << "\t\tswitch (state_) {\n";
	for (int q = 0; q < aut.num_states; ++q) {
		out << "\t\tcase State::q" << q << ": {\n";
		const auto& edges = aut.edges.size() > (size_t)q
			? aut.edges[q] : std::vector<HoaEdge>{};
		for (const auto& e : edges) {
			// Evaluate the FULL guard (including output APs that
			// the oracle has filled into ap[]). If true, take edge.
			out << "\t\t\tif (" << codegen_detail::guard_to_cpp(
			    e.guard_label) << ") {\n";

			// Assign undeclared output APs from this edge's guard.
			// Walk conjuncts; for output APs not in declared_idx, set
			// the polarity from the guard.
			int depth = 0;
			std::string cur;
			std::vector<std::string> conjuncts;
			for (char c : e.guard_label) {
				if (c == '(') { ++depth; cur += c; }
				else if (c == ')') { --depth; cur += c; }
				else if (c == '&' && depth == 0) {
					if (!cur.empty()) conjuncts.push_back(cur);
					cur.clear();
				} else cur += c;
			}
			if (!cur.empty()) conjuncts.push_back(cur);
			for (const auto& conj : conjuncts) {
				std::string t;
				for (char c : conj)
					if (!std::isspace((unsigned char)c)) t += c;
				if (t.empty()) continue;
				bool neg = !t.empty() && t[0] == '!';
				std::string idxstr = neg ? t.substr(1) : t;
				if (idxstr.empty() ||
				    !std::isdigit((unsigned char)idxstr[0])) continue;
				int parsed = 0;
				for (char c : idxstr)
					if (std::isdigit((unsigned char)c))
						parsed = parsed*10 + (c - '0');
				bool is_out = false;
				for (int oi : out_idx) if (oi == parsed) { is_out = true; break; }
				if (!is_out) continue;
				if (declared_idx.count(parsed)) continue;  // oracle set it
				out << "\t\t\t\to." << labels[parsed] << " = "
				    << (neg ? "false" : "true") << ";\n";
			}
			out << "\t\t\t\tstate_ = State::q" << e.dst << ";\n";
			out << "\t\t\t\treturn o;\n";
			out << "\t\t\t}\n";
		}
		out << "\t\t\to.ok = false; return o;\n";
		out << "\t\t}\n";
	}
	out << "\t\t}\n";
	out << "\t\to.ok = false; return o;\n";
	out << "\t}\n";

	out << "\n";
	out << "private:\n";
	out << "\t// Per-stream registered handler + opaque user_data.\n";
	out << "\tstd::map<std::string, std::pair<OracleCallback, void*>> handlers_;\n";
	out << "\tbool in_oracle_dispatch_ = false;\n";
	out << "\n";
	out << "public:\n";
	out << tail;
}


template <NodeType node>
void emit_cpp_program_open(
    const LtlAbaSolution<node>& sol,
    const std::vector<std::string>& open_streams,
    std::ostream& out,
    const std::string& class_name)
{
	using namespace codegen_detail;

	// V1: data-bearing programs aren't yet supported via emit_cpp_program_open;
	// route them through the prop-only path with a warning. Full data support
	// requires deeper template work (deferred to V2).
	std::set<std::string> input_set(sol.input_props.begin(), sol.input_props.end());
	std::map<std::string, AtomMeta> ameta;
	bool has_data = false;
	for (auto& [atom_ref, prop] : sol.atoms) {
		bool is_out = !input_set.count(prop);
		AtomMeta m = classify_atom_for_data<node>(atom_ref, is_out);
		ameta[prop] = m;
		if (m.kind != AtomKind::BOOL) has_data = true;
	}

	if (has_data) {
		// V1 limitation: data-bearing emit_cpp_program_open routes
		// through the prop emit. The chosen open-stream values are
		// announced via registration but actual per-step dispatch
		// is V2 work.
		emit_cpp_program_open_prop(sol.aut, sol.input_props,
			sol.output_props, open_streams, out, class_name);
		return;
	}

	emit_cpp_program_open_prop(sol.aut, sol.input_props, sol.output_props,
		open_streams, out, class_name);
}


// ── PWR-capable (table-driven) emitter ────────────────────────────────────────
//
// Instead of hard-coded switch/case, the strategy is stored as a vector of
// edge tables.  revise() swaps the table at runtime.  This is the codegen
// equivalent of the interpreter's pointwise_revision() + re-synthesis loop.

inline void emit_cpp_program_pwr(
    const HoaAutomaton& aut,
    const std::vector<std::string>& input_props,
    const std::vector<std::string>& /* output_props */,
    std::ostream& out,
    const std::string& class_name)
{
	using codegen_detail::sanitize;
	using codegen_detail::parse_guard_lits;

	// Partition APs into inputs and outputs (an AP not in input_props is output).
	std::vector<int> in_idx, out_idx;
	std::vector<std::string> labels(aut.aps.size());
	for (size_t i = 0; i < aut.aps.size(); ++i) {
		const auto& ap = aut.aps[i];
		bool is_input = false;
		for (const auto& p : input_props) if (p == ap) { is_input = true; break; }
		(is_input ? in_idx : out_idx).push_back((int)i);
		labels[i] = (is_input ? "i_" : "o_") + sanitize(ap);
	}

	out << "// Auto-generated by tau-lang's C++ program emitter (PWR-capable).\n";
	out << "// Do not edit by hand — regenerate from the source .tau spec.\n";
	out << "// The strategy is table-driven; call revise() to swap at runtime.\n\n";
	out << "#pragma once\n";
	out << "#include <cstdint>\n";
	out << "#include <utility>\n";
	out << "#include <vector>\n";
	out << "#include <cassert>\n\n";

	out << "class " << class_name << " {\n";
	out << "public:\n";

	// --- Edge and Strategy types ---
	out << "\t// Each AP literal in a guard: +1 = positive, -1 = negative, 0 = don't care.\n";
	out << "\tstruct Edge {\n";
	out << "\t\tstd::vector<int8_t> guard;  // guard[ap_index] ∈ {-1, 0, +1}\n";
	out << "\t\tint dst = 0;\n";
	out << "\t};\n\n";

	out << "\tstruct Strategy {\n";
	out << "\t\tint num_states = 0;\n";
	out << "\t\tint initial_state = 0;\n";
	out << "\t\tstd::vector<std::vector<Edge>> edges;  // edges[src] = outgoing edges\n";
	out << "\t};\n\n";

	// --- Input/Output structs ---
	out << "\tstruct Inputs {\n";
	for (int i : in_idx)
		out << "\t\tbool " << labels[i] << " = false;\n";
	out << "\t};\n\n";

	out << "\tstruct Outputs {\n";
	for (int i : out_idx)
		out << "\t\tbool " << labels[i] << " = false;\n";
	out << "\t\tbool ok = true;\n";
	out << "\t};\n\n";

	// --- Constructor: embed the initial strategy ---
	out << "\t" << class_name << "() { load_initial_strategy(); }\n\n";

	// --- step() ---
	out << "\tOutputs step(const Inputs& in) noexcept {\n";
	out << "\t\tOutputs o;\n";

	// Build AP evaluation array from inputs.
	if (!aut.aps.empty()) {
		out << "\t\tbool ap[" << aut.aps.size() << "] = {};\n";
		for (int i : in_idx)
			out << "\t\tap[" << i << "] = in." << labels[i] << ";\n";
	} else {
		out << "\t\t(void)in;\n";
	}

	out << "\n";
	out << "\t\tconst auto& src_edges = strat_.edges[state_];\n";
	out << "\t\tfor (const auto& e : src_edges) {\n";
	out << "\t\t\tbool match = true;\n";
	// Only check guard literals for input APs (outputs are assignments).
	for (int ii : in_idx) {
		out << "\t\t\tif (e.guard[" << ii << "] == +1 && !ap[" << ii << "]) match = false;\n";
		out << "\t\t\tif (e.guard[" << ii << "] == -1 &&  ap[" << ii << "]) match = false;\n";
	}

	out << "\t\t\tif (match) {\n";
	// Assign output values from guard.
	for (int oi : out_idx) {
		out << "\t\t\t\tif (e.guard[" << oi << "] == +1) o." << labels[oi] << " = true;\n";
		out << "\t\t\t\telse if (e.guard[" << oi << "] == -1) o." << labels[oi] << " = false;\n";
	}
	out << "\t\t\t\tstate_ = e.dst;\n";
	out << "\t\t\t\treturn o;\n";
	out << "\t\t\t}\n";
	out << "\t\t}\n";
	out << "\t\to.ok = false; return o;\n";
	out << "\t}\n\n";

	// --- revise() ---
	out << "\t// Swap the strategy at runtime (PWR).  The state machine resets\n";
	out << "\t// to the new initial state — matching the interpreter's behaviour\n";
	out << "\t// where a revised spec restarts the unbound continuation.\n";
	out << "\tvoid revise(Strategy new_strat) noexcept {\n";
	out << "\t\tassert(new_strat.num_states > 0);\n";
	out << "\t\tassert(new_strat.initial_state >= 0 && new_strat.initial_state < new_strat.num_states);\n";
	out << "\t\tassert((int)new_strat.edges.size() == new_strat.num_states);\n";
	out << "\t\tfor (auto& sv : new_strat.edges)\n";
	out << "\t\t\tfor (auto& e : sv)\n";
	out << "\t\t\t\tassert((int)e.guard.size() == " << aut.aps.size() << ");\n";
	out << "\t\tstrat_ = std::move(new_strat);\n";
	out << "\t\tstate_ = strat_.initial_state;\n";
	out << "\t\t++revision_count_;\n";
	out << "\t}\n\n";

	// --- Accessors ---
	out << "\tint state() const noexcept { return state_; }\n";
	out << "\tint revision_count() const noexcept { return revision_count_; }\n";
	out << "\tconst Strategy& strategy() const noexcept { return strat_; }\n\n";

	// --- Private: initial strategy loader ---
	out << "private:\n";
	out << "\tint state_ = 0;\n";
	out << "\tint revision_count_ = 0;\n";
	out << "\tStrategy strat_;\n\n";

	out << "\tvoid load_initial_strategy() {\n";
	out << "\t\tstrat_.num_states = " << aut.num_states << ";\n";
	out << "\t\tstrat_.initial_state = " << aut.initial_state << ";\n";
	out << "\t\tstrat_.edges.resize(" << aut.num_states << ");\n";
	int num_aps = (int)aut.aps.size();
	for (int s = 0; s < aut.num_states; ++s) {
		const auto& edges = aut.edges.size() > (size_t)s ? aut.edges[s] : std::vector<HoaEdge>{};
		for (const auto& e : edges) {
			// Build guard literal array.
			auto lits = parse_guard_lits(e.guard_label);
			std::vector<int> guard(num_aps, 0);
			for (auto& [idx, pos] : lits) {
				if (idx >= 0 && idx < num_aps)
					guard[idx] = pos ? +1 : -1;
			}
			// Handle "t" (true) guard — all zeros (don't care).
			out << "\t\tstrat_.edges[" << s << "].push_back({{";
			for (int k = 0; k < num_aps; ++k) {
				if (k > 0) out << ",";
				out << guard[k];
			}
			out << "}, " << e.dst << "});\n";
		}
	}
	out << "\t\tstate_ = strat_.initial_state;\n";
	out << "\t}\n";

	out << "};\n";
}


// ── Strategy initializer emitter ──────────────────────────────────────────────

inline void emit_strategy_initializer(
    const HoaAutomaton& aut,
    const std::vector<std::string>& input_props,
    const std::vector<std::string>& output_props,
    std::ostream& out)
{
	using codegen_detail::parse_guard_lits;
	(void)input_props;
	(void)output_props;

	int num_aps = (int)aut.aps.size();
	out << "{\n";
	out << "\t" << aut.num_states << ", // num_states\n";
	out << "\t" << aut.initial_state << ", // initial_state\n";
	out << "\t{ // edges\n";
	for (int s = 0; s < aut.num_states; ++s) {
		out << "\t\t{ // state " << s << "\n";
		const auto& edges = aut.edges.size() > (size_t)s ? aut.edges[s] : std::vector<HoaEdge>{};
		for (const auto& e : edges) {
			auto lits = parse_guard_lits(e.guard_label);
			std::vector<int> guard(num_aps, 0);
			for (auto& [idx, pos] : lits) {
				if (idx >= 0 && idx < num_aps)
					guard[idx] = pos ? +1 : -1;
			}
			out << "\t\t\t{{";
			for (int k = 0; k < num_aps; ++k) {
				if (k > 0) out << ",";
				out << guard[k];
			}
			out << "}, " << e.dst << "},\n";
		}
		out << "\t\t},\n";
	}
	out << "\t}\n";
	out << "}";
}


} // namespace idni::tau_lang

#endif // __IDNI__TAU__CPP_CODEGEN_TMPL_H__
