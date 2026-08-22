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

// A cube: one product of the guard's sum-of-products, as (AP index, polarity).
using guard_cube = std::vector<std::pair<int,bool>>;

// Parse an HOA guard label into its DNF cubes.
//
// LG-4: every emitter used to tokenise the label by top-level '&' and then
// read each conjunct with `for (char c : idx_str) if (isdigit(c)) idx =
// idx*10+(c-'0')`.  That loop SKIPS '|', '(' and ')' rather than rejecting
// them, so `0|1` came out as the single literal (1, true) — wrong AP, and the
// other disjunct silently gone — while `(0|1)` failed the leading isdigit test
// and was dropped entirely, widening the guard to `true`.  Spot prints
// strategy edge labels as sums of products, so both shapes are normal.
//
// The expansion is delegated to the one guard parser that implements the full
// grammar (`alg_d::hoa_guard::to_dnf`, the same module whose evaluator the
// Algorithm-D game already uses).  A nullopt result means the label did not
// parse or its expansion blew the cube cap; callers must REFUSE the edge in
// that case rather than emit a partial reading.
inline std::optional<std::vector<guard_cube>> parse_guard_cubes(
	const std::string& g)
{
	auto d = alg_d::hoa_guard::to_dnf(g);
	if (!d) return std::nullopt;
	std::vector<guard_cube> cubes;
	cubes.reserve(d->size());
	for (const auto& c : *d) {
		guard_cube gc;
		gc.reserve(c.size());
		for (const auto& l : c) gc.emplace_back(l.ap, l.pos);
		cubes.push_back(std::move(gc));
	}
	return cubes;
}

// Pick a concrete double witness from a qlt interval.  CG-R3: nullopt
// when no literal can be guaranteed to satisfy the cube -- the interval is
// empty (jointly unsatisfiable literals) or a bound is symbolic -- so the
// caller omits the cube instead of embedding a guess with ok=true.
inline std::optional<double> witness_from_qlt_interval(const qlt& interval) {
	if (interval.is_empty()) return std::nullopt;
	const auto& piece = interval.pieces[0];
	const auto& lo = piece.lo.val;
	const auto& hi = piece.hi.val;
	if (lo.is_neg_inf() && hi.is_pos_inf()) return 0.0;
	if (lo.is_neg_inf()) {
		if (hi.is_sym()) return std::nullopt;
		return (double)hi.p / (double)hi.q - 1.0;
	}
	if (hi.is_pos_inf()) {
		if (lo.is_sym()) return std::nullopt;
		return (double)lo.p / (double)lo.q + 1.0;
	}
	// Bounded: midpoint
	if (lo.is_sym() || hi.is_sym()) return std::nullopt;
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
	// LG-5: `num_states == 0` is solve_ltl_aba's "trivially realizable"
	// marker — the constant-output fast path proved that SOME fixed output
	// works without producing an automaton.  Emitting the normal shape for it
	// produced an EMPTY enum body while still writing
	// `State state_ = State::q0;`, so the generated header did not compile
	// and the CLI exited 0 without a word.  Emit a stateless, always-ok
	// program instead.
	const bool trivial = (aut.num_states == 0);
	if (!trivial) {
		out << "\tenum class State : std::uint32_t {\n";
		for (int s = 0; s < aut.num_states; ++s) {
			out << "\t\tq" << s << (s + 1 < aut.num_states ? "," : "") << "\n";
		}
		out << "\t};\n\n";
	}

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

	if (trivial) {
		out << "\t// Trivially realizable: synthesis proved a constant\n";
		out << "\t// output satisfies the specification, so there is no\n";
		out << "\t// automaton and no state to track.\n";
		out << "\tOutputs step(const Inputs& in) noexcept {\n";
		out << "\t\t(void)in;\n";
		out << "\t\treturn Outputs{};\n";
		out << "\t}\n";
		out << "};\n";
		return;
	}

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
			// One dispatch branch per CUBE of the guard's sum of
			// products: `(0|1)&2` yields two branches instead of one
			// mis-parsed conjunction, and `0|1` no longer collapses to
			// the single literal `1` (LG-4).
			auto cubes = codegen_detail::parse_guard_cubes(e.guard_label);
			if (!cubes) {
				out << "\t\t\t// guard [" << e.guard_label
				    << "] could not be expanded — edge omitted\n";
				continue;
			}
			for (const auto& cube : *cubes) {
				// Under Spot's Mealy convention the output literals
				// in a guard are assignments we realise; the input
				// literals are the branch condition.
				std::ostringstream asg, ig;
				bool any_in = false;
				for (const auto& [ap_i, positive] : cube) {
					if (ap_i < 0 || ap_i >= (int)aut.aps.size())
						continue;
					bool is_out = false;
					for (int oi : out_idx)
						if (oi == ap_i) { is_out = true; break; }
					if (is_out) {
						asg << "\t\t\t\to." << labels[ap_i] << " = "
						    << (positive ? "true" : "false") << ";\n";
						continue;
					}
					bool is_in = false;
					for (int ii : in_idx)
						if (ii == ap_i) { is_in = true; break; }
					if (!is_in) continue;
					if (any_in) ig << " && ";
					ig << (positive ? "in." : "!in.") << labels[ap_i];
					any_in = true;
				}
				std::string effective_input_guard =
					any_in ? ig.str() : "true";

				out << (first ? "\t\t\tif (" : "\t\t\telse if (")
				    << effective_input_guard << ") {\n";
				out << asg.str();
				out << "\t\t\t\tstate_ = State::q" << e.dst << ";\n";
				out << "\t\t\t\treturn o;\n";
				out << "\t\t\t}\n";
				first = false;
			}
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
	// LG-5: see emit_cpp_program_prop — `num_states == 0` means trivially
	// realizable, and the normal shape emits an empty enum body while still
	// naming State::q0 in the state_ member.
	const bool trivial = (sol.aut.num_states == 0);
	if (!trivial) {
		out << "\tenum class State : std::uint32_t {\n";
		for (int s = 0; s < sol.aut.num_states; ++s)
			out << "\t\tq" << s << (s+1 < sol.aut.num_states ? "," : "") << "\n";
		out << "\t};\n\n";
	}

	// Inputs struct: input props remain bool (caller evaluates condition).
	// LG-29: `i_`-prefixed like the prop emitter's label_aps naming, so a
	// spec moving between the pure-prop and data paths keeps its ABI.
	out << "\tstruct Inputs {\n";
	for (auto& prop : sol.input_props) {
		auto it = ameta.find(prop);
		if (it != ameta.end() && it->second.kind == AtomKind::INPUT_QLT)
			out << "\t\tbool i_" << sanitize(prop) << " = false;"
			    << "  // qlt: evaluate '" << prop << "' condition on your input value\n";
		else
			out << "\t\tbool i_" << sanitize(prop) << " = false;\n";
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

	if (trivial) {
		out << "\t// Trivially realizable: synthesis proved a constant\n";
		out << "\t// output satisfies the specification, so there is no\n";
		out << "\t// automaton and no state to track.\n";
		out << "\tOutputs step(const Inputs& in) noexcept {\n";
		out << "\t\t(void)in;\n";
		out << "\t\treturn Outputs{};\n";
		out << "\t}\n";
		out << "};\n";
		return;
	}

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
			// One branch per cube of the guard's sum of products
			// (LG-4); a label that cannot be expanded is refused
			// outright rather than read partially.
			auto cubes = parse_guard_cubes(e.guard_label);
			if (!cubes) {
				out << "\t\t\t// guard [" << e.guard_label
				    << "] could not be expanded — edge omitted\n";
				continue;
			}
			for (const auto& guard_lits : *cubes) {

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
					ig << "in.i_" << sanitize(prop);
					any = true;
				}
				in_guard = any ? ig.str() : "true";
			}

			// For each output-qlt literal, group by variable. LG-6:
			// negative literals are conjoined as wff negations -- the
			// witness must satisfy the WHOLE edge guard, not only its
			// positive atoms (o1>0 && !(o1<2) demands o1 >= 2, and a
			// positive-only pick of 1.0 would leave the synthesized
			// strategy while reporting ok=true).
			std::map<std::string, std::vector<tref>> var_qlt_lits;
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
					tref atom = prop_to_atom[prop];
					var_qlt_lits[meta_it->second.var_name].push_back(
						positive ? atom
							 : tau::build_wff_neg(atom));
					var_io_ref[meta_it->second.var_name] = meta_it->second.io_var_ref;
				} else {
					bool_asgns.emplace_back(prop, positive);
				}
			}

			// Compute qlt witnesses at code-generation time.
			std::map<std::string, double> witnesses;
			std::string no_witness;   // CG-R3: first variable without one
			if constexpr (ba_variant_includes_v<qlt, typename tau::constant>) {
				for (auto& [var, atom_refs] : var_qlt_lits) {
					tref conj = atom_refs[0];
					for (size_t ai = 1; ai < atom_refs.size(); ++ai)
						conj = tau::build_wff_and(conj, atom_refs[ai]);
					tref io_ref = var_io_ref[var];
					std::optional<double> w;
					if (auto interval = qlt_dlo_qe<node>(io_ref, conj); interval)
						w = witness_from_qlt_interval(interval.value());
					if (w) witnesses[var] = *w;
					else if (no_witness.empty()) no_witness = var;
				}
			}
			// CG-R3: a cube whose qlt literals admit no concrete value
			// (empty interval, symbolic bound, undetermined QE) is not
			// emitted at all -- the old picker embedded 0.0 / 1.0 and
			// the program reported ok=true while violating the guard.
			if (!no_witness.empty()) {
				out << "\t\t\t// cube of [" << e.guard_label
				    << "] omitted: no concrete witness for "
				    << no_witness << "\n";
				continue;
			}

			// Emit edge.
			out << "\t\t\t" << (first ? "if (" : "else if (") << in_guard << ") {\n";

			for (auto& var : out_qlt_vars) {
				if (witnesses.count(var))
					out << "\t\t\t\to." << sanitize(var) << " = "
					    << double_to_cpp(witnesses[var]) << ";\n";
				// If no qlt literal for this var on this edge: leave default 0.0.
			}
			for (auto& [prop, val] : bool_asgns)
				out << "\t\t\t\to." << sanitize(prop) << " = " << (val ? "true" : "false") << ";\n";

			out << "\t\t\t\tstate_ = State::q" << e.dst << ";\n";
			out << "\t\t\t\treturn o;\n";
			out << "\t\t\t}\n";
			first = false;
			} // per-cube
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

	// LG-5: for a trivially-realizable solution the prop emit above is a
	// stateless stub with no State type, and every piece of the declare_open
	// scaffolding below is State-typed (admissible_values_mask(State q, …),
	// the dispatch switch).  There is no automaton to expose, so emit the
	// stub as it stands.
	if (aut.num_states == 0) { out << source; return; }

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
				// A value is admissible for this edge if SOME cube of
				// the guard admits it.  The old scan tokenised only by
				// top-level '&', so a disjunctive or parenthesised
				// label reported the wrong mask (LG-4).
				auto cubes = codegen_detail::parse_guard_cubes(
					e.guard_label);
				if (!cubes) continue;   // unexpandable: contributes nothing
				for (const auto& cube : *cubes) {
					bool found_pos = false, found_neg = false;
					for (const auto& [parsed, positive] : cube) {
						if (parsed != ap_idx) continue;
						if (positive) found_pos = true;
						else          found_neg = true;
					}
					// A cube that does not mention ap_idx leaves both
					// values admissible (don't-care over the AP).
					if (!found_pos && !found_neg) { mask |= 0x3; continue; }
					if (found_pos) mask |= 0x2;  // true admissible
					if (found_neg) mask |= 0x1;  // false admissible
				}
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
			// CG-N3: evaluate each cube on its INPUT + DECLARED
			// projection only.  An undeclared output AP is not in
			// ap[] (nobody filled it), so evaluating the full guard
			// made every cube with a positive undeclared literal
			// false -- the edge could never fire.  The undeclared
			// outputs are instead ASSIGNED from the matching cube,
			// exactly as the standard step() derives them.  A label
			// that cannot be expanded (to_dnf refused) falls back to
			// the full-guard evaluation, which is at worst
			// conservative.
			auto cubes = codegen_detail::parse_guard_cubes(e.guard_label);
			if (!cubes) {
				out << "\t\t\tif (" << codegen_detail::guard_to_cpp(
				    e.guard_label) << ") {\n";
				out << "\t\t\t\tstate_ = State::q" << e.dst << ";\n";
				out << "\t\t\t\treturn o;\n";
				out << "\t\t\t}\n";
				continue;
			}
			for (const auto& cube : *cubes) {
				std::string cond;
				for (const auto& [parsed, positive] : cube) {
					if (parsed < 0 || parsed >= (int)aut.aps.size())
						continue;
					bool is_out = false;
					for (int oi : out_idx)
						if (oi == parsed) { is_out = true; break; }
					if (is_out && !declared_idx.count(parsed))
						continue;   // assigned below, not tested
					if (!cond.empty()) cond += " && ";
					cond += (positive ? "" : "!") + std::string("ap[")
					      + std::to_string(parsed) + "]";
				}
				if (cond.empty()) cond = "true";
				out << "\t\t\tif (" << cond << ") {\n";
				for (const auto& [parsed, positive] : cube) {
					if (parsed < 0 || parsed >= (int)aut.aps.size())
						continue;
					bool is_out = false;
					for (int oi : out_idx)
						if (oi == parsed) { is_out = true; break; }
					if (!is_out || declared_idx.count(parsed)) continue;
					out << "\t\t\t\to." << labels[parsed] << " = "
					    << (positive ? "true" : "false") << ";\n";
				}
				out << "\t\t\t\tstate_ = State::q" << e.dst << ";\n";
				out << "\t\t\t\treturn o;\n";
				out << "\t\t\t}\n";
			}
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

	// V1 limitation (LG-17): data-bearing specs route through the
	// prop-only emitter -- per-step data dispatch is V2 work. Emit the
	// promised warning instead of a silent no-op branch.
	if (has_data) TAU_LOG_WARNING << "[codegen] open-stream emitter: spec has "
		"data atoms; V1 emits propositional dispatch only";

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
	using codegen_detail::parse_guard_cubes;

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

	// LG-5: a trivially-realizable solution has no automaton.  The table
	// below would be emitted with num_states = 0, and the generated step()
	// would index strat_.edges[state_] on an empty vector.  Emit a stateless
	// stub; there is no strategy to revise either.
	if (aut.num_states == 0) {
		out << "\t// Trivially realizable: synthesis proved a constant output\n";
		out << "\t// satisfies the specification, so there is no strategy table.\n";
		out << "\tstruct Inputs {\n";
		for (int i : in_idx) out << "\t\tbool " << labels[i] << " = false;\n";
		out << "\t};\n\n";
		out << "\tstruct Outputs {\n";
		for (int i : out_idx) out << "\t\tbool " << labels[i] << " = false;\n";
		out << "\t\tbool ok = true;\n";
		out << "\t};\n\n";
		out << "\tOutputs step(const Inputs& in) noexcept {\n";
		out << "\t\t(void)in;\n";
		out << "\t\treturn Outputs{};\n";
		out << "\t}\n";
		out << "};\n";
		return;
	}

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
			// One table entry per CUBE: a Strategy guard is a single
			// literal vector, so a disjunctive label needs one edge per
			// product.  Dispatch is first-match, which makes the
			// expansion equivalent (LG-4).
			auto cubes = parse_guard_cubes(e.guard_label);
			if (!cubes) {
				out << "\t\t// guard [" << e.guard_label
				    << "] could not be expanded — edge omitted\n";
				continue;
			}
			for (const auto& cube : *cubes) {
				std::vector<int> guard(num_aps, 0);
				for (const auto& [idx, pos] : cube)
					if (idx >= 0 && idx < num_aps)
						guard[idx] = pos ? +1 : -1;
				// A "t" (true) guard is all zeros — don't care.
				out << "\t\tstrat_.edges[" << s << "].push_back({{";
				for (int k = 0; k < num_aps; ++k) {
					if (k > 0) out << ",";
					out << guard[k];
				}
				out << "}, " << e.dst << "});\n";
			}
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
	using codegen_detail::parse_guard_cubes;
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
			auto cubes = parse_guard_cubes(e.guard_label);
			if (!cubes) continue;   // unexpandable guard: emit no entry
			for (const auto& cube : *cubes) {
				std::vector<int> guard(num_aps, 0);
				for (const auto& [idx, pos] : cube)
					if (idx >= 0 && idx < num_aps)
						guard[idx] = pos ? +1 : -1;
				out << "\t\t\t{{";
				for (int k = 0; k < num_aps; ++k) {
					if (k > 0) out << ",";
					out << guard[k];
				}
				out << "}, " << e.dst << "},\n";
			}
		}
		out << "\t\t},\n";
	}
	out << "\t}\n";
	out << "}";
}


} // namespace idni::tau_lang

#endif // __IDNI__TAU__CPP_CODEGEN_TMPL_H__
