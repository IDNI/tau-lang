// To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.md

#ifndef __IDNI__TAU__CPP_CODEGEN_TMPL_H__
#define __IDNI__TAU__CPP_CODEGEN_TMPL_H__

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "dict.h"
#include "io_context.h"

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

// The atom-metadata a data-atom emitter groups witnesses by: which io_var
// (if any) this atom is single-variable over, and whether that var's owner
// contributes a flag or a witness. Replaces the old AtomKind/AtomMeta pair,
// which conflated ownership with direction (see classify_output_field).
struct atom_field_info {
	field_kind kind = field_kind::flag;
	std::string var_name;      // the io_var's base name, when single-variable
	tref io_var_ref = nullptr; // io_var tref (for the witness query)
	// Output-direction vars of a witness_template atom: each is a field
	// whose value the runtime solve produces.
	std::vector<std::pair<std::string, tref>> template_vars;
};

// Classify an OUTPUT atom's field kind purely from what owns @p io_var_ref's
// type: flag when it's the pack's resolved bool carrier, witness otherwise.
// @p atom_ref is reserved for the witness-expression query. A spec reaching
// codegen has already passed type inference, so ba_type 0 (no type at all,
// distinct from the real "untyped" BA type) means inference did not run for
// this variable -- an invariant violation, not an ordinary case.
template <NodeType node>
field_kind classify_output_field(tref /*atom_ref*/, tref io_var_ref) {
	const size_t vtype = tree<node>::get(io_var_ref).get_ba_type();
	if (vtype == 0)
		throw std::runtime_error("untyped variable '"
			+ get_var_name<node>(io_var_ref)
			+ "' reached codegen; spec did not go through type inference");
	if (vtype == ba_types<node>::id(pack_bool_carrier_type<node>()))
		return field_kind::flag;
	return field_kind::witness;
}

// The one variable name shared by every free-var node of atom_ref -- a
// self-lookback atom (e.g. o2[t] vs o2[t-1]) is two distinct nodes naming the
// same variable, so it still has a single stream key. nullopt when the atom
// has zero free vars or more than one distinct name; the shared answer both
// classify_atom_field and build_program_desc's flag-stream extraction need,
// so a self-lookback flag atom is never refused by one after being accepted
// by the other.
template <NodeType node>
std::optional<std::string> atom_single_var_name(tref atom_ref) {
	std::optional<std::string> name;
	for (tref v : get_free_vars<node>(atom_ref)) {
		const std::string& nm = get_var_name<node>(v);
		if (!name) name = nm;
		else if (nm != *name) return std::nullopt;
	}
	return name;
}

// True for a synthesis-internal output prop with no backing atom/io_var at
// all: the step-counter's o__ltl_ctr bits (apply_step_counter_encoding) and
// the ppLTLTT past-operator DFA testers (__past_s*/__past_t*,
// ltl_aba_helpers.tmpl.h's skeleton_wff_with_testers) are automaton
// bookkeeping registered straight into sol.output_props with no entry in
// sol.atoms -- unlike an ordinary flag atom, there is no io stream to key
// them on, so they must never reach the emitted Outputs surface.
inline bool is_internal_ltl_output_prop(const std::string& p) {
	return (p.size() > 10 && p.compare(0, 10, "o__ltl_ctr") == 0)
		|| (p.size() > 8 && p.compare(0, 8, "__past_s") == 0)
		|| (p.size() > 8 && p.compare(0, 8, "__past_t") == 0);
}

// True when `atom_ref` has a free io_var typed by a real, pack-registered data
// BA (not the bool carrier, not a reserved core type id) -- with one carve
// out: the reserved tau type is a real data type for an ordinary spec (it
// owns no codegen_witness, so it lands on witness_template), and only reads
// as flag-worthy PWR update-stream data when @p revisable positively
// marks this whole build a PWR revision (build_program_desc's own hard
// refusal then guarantees every other output is flag too).
template <NodeType node>
bool atom_is_data_typed(tref atom_ref, bool revisable) {
	const size_t carrier = ba_types<node>::id(pack_bool_carrier_type<node>());
	const size_t tau_id = ba_types<node>::id(tau_type<node>());
	for (tref v : get_free_vars<node>(atom_ref)) {
		size_t vt = tree<node>::get(v).get_ba_type();
		if (vt == 0 || vt == carrier) continue;
		if (vt == tau_id) {
			if (!revisable) return true;
			continue;
		}
		if (!is_reserved_ba_type<node>(vt)) return true;
	}
	return false;
}

// Classifies one atom for build_program_desc(): which io_vars it is over and
// how their values are produced.
//
// Inputs are always flag. A multi-variable output atom is always
// witness_template (no BA type can bake a constant for it), solved at
// runtime by table_step_provider. A single-variable output atom is flag
// over the bool carrier, or over the reserved tau type in a PWR revision
// (@p revisable); every other reserved core type is flag too. Over a real
// data BA otherwise, it yields a witness when the owner bakes constants
// (codegen_witness), else witness_template.
template <NodeType node>
atom_field_info classify_atom_field(
	tref atom_ref, bool is_output, bool revisable) {
	using tau = tree<node>;
	atom_field_info m;
	if (!is_output) return m;
	const trefs& fvars = get_free_vars<node>(atom_ref);
	if (fvars.empty()) return m;
	auto single = atom_single_var_name<node>(atom_ref);
	bool multi = !single.has_value();
	std::string var_name = multi ? std::string{} : *single;
	tref io_ref = multi ? nullptr : fvars[0];
	if (var_name.empty() && !multi) return m;
	if (multi) {
		std::set<std::string> seen;
		for (tref v : fvars) {
			const std::string& nm = get_var_name<node>(v);
			if (!seen.insert(nm).second) continue;
			tref io_node = tau::get(v).is(tau::io_var)
				? v : tau::get(v).child(0);
			if (io_var_direction<node>(io_node) != 1)
				m.template_vars.emplace_back(nm, v);
		}
		if (m.template_vars.empty()) return m;
		m.kind = field_kind::witness_template;
		return m;
	}
	if (!atom_is_data_typed<node>(atom_ref, revisable)) return m;
	if (classify_output_field<node>(atom_ref, io_ref) != field_kind::witness)
		return m;
	if (pack_type_has_codegen_witness<node>(
		tree<node>::get(io_ref).get_ba_type()))
	{
		m.var_name = var_name;
		m.io_var_ref = io_ref;
		m.kind = field_kind::witness;
		return m;
	}
	m.template_vars.emplace_back(var_name, io_ref);
	m.kind = field_kind::witness_template;
	return m;
}

// Every free io variable of an atom reaching codegen must carry a real BA
// type -- ba_type 0 means type inference never ran for it. Checked for both
// input and output atoms, single- and multi-variable alike, independently of
// classify_output_field's own check (which only ever sees a single-variable
// output atom's var).
template <NodeType node>
void validate_atom_io_types(tref atom_ref) {
	for (tref v : get_free_vars<node>(atom_ref))
		if (tree<node>::get(v).get_ba_type() == 0)
			throw std::runtime_error("untyped variable '"
				+ get_var_name<node>(v)
				+ "' reached codegen; spec did not go through type inference");
}

// Snapshot the emitting process's full ba-type registry, in id order, as
// replayable recipes: a reserved builder, a pack family (+param), or a bare
// syntactic type tree. The emitted main replays every entry and asserts its
// id, so each numeric type id baked anywhere in the desc resolves to the
// same type in the artifact -- static initialization registers types before
// main in both processes, in an order neither controls, and only a full
// replay-with-assert makes the numbering portable.
template <NodeType node>
std::vector<ba_type_entry> snapshot_ba_type_registry() {
	using namespace ba_types_detail;
	std::vector<ba_type_entry> table;
	for (size_t i = 0; i < ba_types<node>::count(); ++i) {
		tref t = ba_types<node>::type_tree(i);
		ba_type_entry e;
		e.id = i;
		if (t == untyped_type<node>()) {
			e.kind = ba_type_entry::recipe::reserved;
			e.name = "untyped";
		} else if (t == tau_type<node>()) {
			e.kind = ba_type_entry::recipe::reserved;
			e.name = "tau";
		} else if (t == nat_type<node>()) {
			e.kind = ba_type_entry::recipe::reserved;
			e.name = "nat";
		} else if (auto fp = pack_type_family_param<node>(t);
			fp && pack_type_tree<node>(fp->first, fp->second) == t)
		{
			e.kind = ba_type_entry::recipe::family;
			e.name = fp->first;
			e.param = fp->second;
		} else {
			std::string nm = ba_types<node>::name(i);
			if (!nm.empty() && nm[0] == ':') nm.erase(0, 1);
			if (make_syntactic_type_tree<node>(nm.c_str()) != t)
				throw std::runtime_error("ba type '"
					+ ba_types<node>::name(i)
					+ "' is not replayable by an emitted artifact");
			e.kind = ba_type_entry::recipe::syntactic;
			e.name = nm;
		}
		table.push_back(std::move(e));
	}
	return table;
}

// Reconstruct one ABA-comparison operand (a ground BA constant, a bare bf_t/
// bf_f literal, or a plain variable reference) as a self-contained C++
// expression of type tref. A bare literal (e.g. the `1` in `o1[t] = 1`) never
// becomes a ba_constant node -- type inference only ever retypes it in place
// -- so it carries the atom's real BA type once inference has run; when it
// is still 0 (untyped), fall back to `sibling_type`, the other operand's type.
template <NodeType node>
std::string build_atom_term_expr(tref term, size_t sibling_type = 0) {
	using tau = tree<node>;
	tref trimmed = tau::trim(term);
	if (tau::get(trimmed).is_ba_constant()) {
		size_t bt = tau::get(trimmed).get_ba_type();
		if (auto e = pack_codegen_constant_expr<node>(bt, trimmed); e)
			return *e;
		// is_one/is_zero are mandatory descriptor members (unlike
		// codegen_constant_expr), so a carrier-typed constant that is
		// trivially one or zero has a BA-agnostic C++ spelling -- a truth
		// bit rendered as a marker -- even when the carrier spells nothing
		// else. Any other BA type's constant must be rebuilt for real: a
		// marker there would misrepresent it, so its owner declining
		// codegen_constant_expr is a hard error, not a fallback.
		if (bt == ba_types<node>::id(pack_bool_carrier_type<node>())) {
			const auto& cst = tau::get(trimmed).get_ba_constant();
			if (node::ba::is_one(cst))
				return "::idni::tau_lang::build_bf_t_type<"
					"::idni::tau_lang::tau_pack::node_t>(" + std::to_string(bt) + ")";
			if (node::ba::is_zero(cst))
				return "::idni::tau_lang::build_bf_f_type<"
					"::idni::tau_lang::tau_pack::node_t>(" + std::to_string(bt) + ")";
			throw std::runtime_error(
				"atom constant's owning BA declined codegen_constant_expr "
				"for a feasible edge");
		}
		throw std::runtime_error("atom constant's owning BA type '"
			+ ba_types<node>::name(bt) + "' declined codegen_constant_expr; "
			"atom emission does not support this shape");
	}
	if (tau::get(trimmed).is(tau::bf_t) || tau::get(trimmed).is(tau::bf_f)) {
		size_t bt = tau::get(trimmed).get_ba_type();
		if (bt == 0) bt = sibling_type;
		if (bt == 0) throw std::runtime_error(
			"atom's bare literal operand is untyped and its sibling "
			"operand names no BA type either; atom emission does not "
			"support this shape");
		const char* fn = tau::get(trimmed).is(tau::bf_t)
			? "build_bf_t_type" : "build_bf_f_type";
		return std::string("::idni::tau_lang::") + fn
			+ "<::idni::tau_lang::tau_pack::node_t>(" + std::to_string(bt) + ")";
	}
	// A BA-level complement over an otherwise-plain operand (e.g. the tau BA's
	// own `x'`) is not itself a ground constant or a variable reference, but
	// its operand may recursively be one; build_bf_neg is the runtime builder
	// this rebuilds it with, mirroring how the comparison operators above are
	// rebuilt with their own build_bf_* counterparts.
	if (tau::get(trimmed).is(tau::bf_neg)) {
		tref inner = tau::get(trimmed).child(0);
		return "::idni::tau_lang::build_bf_neg<::idni::tau_lang::tau_pack::"
			"node_t>(" + build_atom_term_expr<node>(inner, sibling_type) + ")";
	}
	// A binary bf operator (e.g. `x & y`) is likewise rebuilt with its own
	// build_bf_* runtime builder, recursing into both operands.
	{
		const char* fn = nullptr;
		if (tau::get(trimmed).is(tau::bf_and)) fn = "build_bf_and";
		else if (tau::get(trimmed).is(tau::bf_or)) fn = "build_bf_or";
		else if (tau::get(trimmed).is(tau::bf_xor)) fn = "build_bf_xor";
		else if (tau::get(trimmed).is(tau::bf_nand)) fn = "build_bf_nand";
		else if (tau::get(trimmed).is(tau::bf_nor)) fn = "build_bf_nor";
		else if (tau::get(trimmed).is(tau::bf_xnor)) fn = "build_bf_xnor";
		else if (tau::get(trimmed).is(tau::bf_shl)) fn = "build_bf_shl";
		else if (tau::get(trimmed).is(tau::bf_shr)) fn = "build_bf_shr";
		else if (tau::get(trimmed).is(tau::bf_add)) fn = "build_bf_add";
		else if (tau::get(trimmed).is(tau::bf_sub)) fn = "build_bf_sub";
		else if (tau::get(trimmed).is(tau::bf_mul)) fn = "build_bf_mul";
		else if (tau::get(trimmed).is(tau::bf_div)) fn = "build_bf_div";
		else if (tau::get(trimmed).is(tau::bf_mod)) fn = "build_bf_mod";
		if (fn) {
			tref lhs = tau::get(trimmed).first();
			tref rhs = tau::get(trimmed).second();
			return std::string("::idni::tau_lang::") + fn
				+ "<::idni::tau_lang::tau_pack::node_t>("
				+ build_atom_term_expr<node>(lhs, sibling_type) + ", "
				+ build_atom_term_expr<node>(rhs, sibling_type) + ")";
		}
	}
	if (get_free_vars<node>(term).size() != 1 || !is_io_var<node>(trimmed))
		throw std::runtime_error(
			"atom operand is neither a ground constant nor a plain "
			"variable reference; atom emission does not support this shape");
	tref io_node = tau::get(trimmed).is(tau::io_var)
		? trimmed : tau::get(trimmed).child(0);
	size_t dir = io_var_direction<node>(io_node);
	size_t type_id = tau::get(trimmed).get_ba_type();
	const std::string& name = get_var_name<node>(trimmed);
	const char* fn = dir == 1 ? "build_in_var" : "build_out_var";
	std::ostringstream ss;
	ss << "::idni::tau_lang::" << fn;
	if (is_io_initial<node>(trimmed)) {
		ss << "_at_n<::idni::tau_lang::tau_pack::node_t>(\"" << name
		   << "\", " << get_io_time_point<node>(trimmed) << ", "
		   << type_id << ")";
	} else if (is_io_shift<node>(trimmed)) {
		ss << "_at_t_minus<::idni::tau_lang::tau_pack::node_t>(\"" << name
		   << "\", " << get_io_shift<node>(trimmed) << ", " << type_id
		   << ", \"t\")";
	} else {
		ss << "_at_t<::idni::tau_lang::tau_pack::node_t>("
		      "::idni::tau_lang::build_var_name<"
		      "::idni::tau_lang::tau_pack::node_t>(\"" << name << "\"), "
		   << type_id << ", \"t\")";
	}
	return ss.str();
}

// Reconstruct a relative-time ABA-comparison atom (sol.atoms[i].first) as a self-contained C++ expression of type tref.
template <NodeType node>
std::string build_atom_ground_expr(tref atom_ref) {
	using tau = tree<node>;
	const auto& t = tau::get(atom_ref);
	if (!t.has_child())
		throw std::runtime_error(
			"atom has no comparison operator; atom emission does not "
			"support this shape");
	auto nt = t[0].value.nt;
	const char* fn = nullptr;
	if (nt == tau::bf_eq) fn = "build_bf_eq";
	else if (nt == tau::bf_neq) fn = "build_bf_neq";
	else if (nt == tau::bf_lt) fn = "build_bf_lt";
	else if (nt == tau::bf_nlt) fn = "build_bf_nlt";
	else if (nt == tau::bf_lteq) fn = "build_bf_lteq";
	else if (nt == tau::bf_nlteq) fn = "build_bf_nlteq";
	else if (nt == tau::bf_gt) fn = "build_bf_gt";
	else if (nt == tau::bf_ngt) fn = "build_bf_ngt";
	else if (nt == tau::bf_gteq) fn = "build_bf_gteq";
	else if (nt == tau::bf_ngteq) fn = "build_bf_ngteq";
	else throw std::runtime_error(
		"atom's comparison operator (bf_interval or unrecognized) is "
		"not supported by atom emission");
	tref lhs_term = t[0].first(), rhs_term = t[0].second();
	size_t lhs_type = tau::get(tau::trim(lhs_term)).get_ba_type();
	size_t rhs_type = tau::get(tau::trim(rhs_term)).get_ba_type();
	std::string lhs = build_atom_term_expr<node>(lhs_term, rhs_type);
	std::string rhs = build_atom_term_expr<node>(rhs_term, lhs_type);
	return std::string("::idni::tau_lang::") + fn
		+ "<::idni::tau_lang::tau_pack::node_t>(" + lhs + ", " + rhs + ")";
}

// Returns {lookback, highest_initial_pos}: max relative shift among the
// non-positional atoms' io_vars, highest constant position among the rest.
template <NodeType node>
std::pair<int, int> compute_auto_continue_bounds(
    const std::vector<std::pair<tref, std::string>>& atoms)
{
	using tau = tree<node>;
	int highest_initial_pos = 0;
	trefs relative_io_vars;
	for (auto& [atom_ref, prop] : atoms) {
		if (atom_is_positional<node>(atom_ref)) {
			highest_initial_pos = std::max(highest_initial_pos,
				(int)atom_max_position<node>(atom_ref));
		} else {
			auto io_vars = tau::get(atom_ref).select_top(
				is_child<node, tau::io_var>);
			relative_io_vars.insert(relative_io_vars.end(),
				io_vars.begin(), io_vars.end());
		}
	}
	int lookback = (int)get_max_shift<node>(relative_io_vars);
	return {lookback, highest_initial_pos};
}

} // namespace codegen_detail


// ── Data-driven emit path (program_desc / emit_program) ───────────────────────
//
// One program_desc walked by one non-templated emit_program(): the prop,
// data-atom, PWR and declare_open shapes are all data over this one path.
// See src/codegen_strategy.h for the runtime shape and its guard convention;
// program_desc/edge_desc/field_desc are declared in cpp_codegen.h since
// build_program_desc()/emit_program() return/consume them across the
// template boundary.

namespace codegen_detail {

// One cube's literal-at-AP lookup, shared by build_edge_guard and
// build_program_desc's per-cube witness grouping.
inline std::int8_t cube_lit_at(const guard_cube& cube, int ap) {
	if (ap < 0) return 0;
	for (const auto& [idx, pos] : cube) if (idx == ap) return pos ? 1 : -1;
	return 0;
}

// Build one edge_desc's guard vector from a single cube: the first
// `in_ap_idx` entries mirror `in_ap_idx`'s APs (matching literals), the
// rest mirror `flag_out_ap_idx`'s APs (assignment literals) — see
// codegen_strategy.h's guard convention.
inline std::vector<std::int8_t> guard_from_cube(const guard_cube& cube,
    const std::vector<int>& in_ap_idx, const std::vector<int>& flag_out_ap_idx)
{
	std::vector<std::int8_t> guard(
		in_ap_idx.size() + flag_out_ap_idx.size(), 0);
	for (size_t k = 0; k < in_ap_idx.size(); ++k)
		guard[k] = cube_lit_at(cube, in_ap_idx[k]);
	for (size_t k = 0; k < flag_out_ap_idx.size(); ++k)
		guard[in_ap_idx.size() + k] = cube_lit_at(cube, flag_out_ap_idx[k]);
	return guard;
}

// Build one HOA edge's guard vector(s) from its raw guard string: the first
// `in_ap_idx` entries of each result mirror `in_ap_idx`'s APs (matching
// literals), the rest mirror `flag_out_ap_idx`'s APs (assignment literals)
// — see codegen_strategy.h's guard convention. A disjunctive guard (LG-4:
// spot can merge an edge's alternatives into one guard, e.g. "!0&!1 | 0&1"
// for a single self-loop) expands via the same DNF parser Algorithm D uses
// (alg_d::hoa_guard::to_dnf, through parse_guard_cubes) into one guard
// vector per cube; the caller creates one edge_desc per returned vector,
// all to the same destination. Returns an empty vector when the label does
// not parse -- the caller omits the edge rather than emit a partial guard.
inline std::vector<std::vector<std::int8_t>> build_edge_guard(
    const std::string& guard_label,
    const std::vector<int>& in_ap_idx,
    const std::vector<int>& flag_out_ap_idx)
{
	auto cubes = parse_guard_cubes(guard_label);
	if (!cubes) return {};
	std::vector<std::vector<std::int8_t>> out;
	out.reserve(cubes->size());
	for (const auto& cube : *cubes)
		out.push_back(guard_from_cube(cube, in_ap_idx, flag_out_ap_idx));
	return out;
}

// How many of `d.outputs`' leading entries are flag fields (by construction
// flag fields always come first — see build_program_desc/build_program_desc_prop).
inline size_t num_flag_outputs(const program_desc& d) {
	size_t n = 0;
	for (auto& f : d.outputs) { if (f.kind != field_kind::flag) break; ++n; }
	return n;
}

} // namespace codegen_detail

inline program_desc build_program_desc_prop(
    const hoa_automaton& aut,
    const std::vector<std::string>& input_props,
    const std::vector<std::string>& output_props,
    const std::string& class_name,
    bool revisable,
    const std::vector<std::string>& open_streams)
{
	using namespace codegen_detail;

	program_desc d;
	d.class_name = class_name;
	d.num_states = aut.num_states;
	d.initial_state = aut.initial_state;
	d.revisable = revisable;
	d.open_streams = open_streams;
	d.needs_tau_link = false;

	std::set<std::string> input_set(input_props.begin(), input_props.end());
	for (auto& p : input_props)
		d.inputs.push_back({p, sanitize(p), field_kind::flag});
	for (auto& p : output_props)
		d.outputs.push_back({p, sanitize(p), field_kind::flag});
	// An AP the caller's lists didn't name (shouldn't happen for a complete
	// HOA parse, but stay defensive rather than silently dropping an edge's
	// literal on it) is treated as an extra flag output.
	for (auto& ap : aut.aps) {
		if (input_set.count(ap)) continue;
		bool have = false;
		for (auto& f : d.outputs) if (f.prop == ap) { have = true; break; }
		if (!have) d.outputs.push_back({ap, sanitize(ap), field_kind::flag});
	}

	std::map<std::string, int> prop_to_ap;
	for (int i = 0; i < (int)aut.aps.size(); ++i) prop_to_ap[aut.aps[i]] = i;
	std::vector<int> in_ap_idx, out_ap_idx;
	for (auto& f : d.inputs)
		in_ap_idx.push_back(prop_to_ap.count(f.prop) ? prop_to_ap[f.prop] : -1);
	for (auto& f : d.outputs)
		out_ap_idx.push_back(prop_to_ap.count(f.prop) ? prop_to_ap[f.prop] : -1);

	d.edges.resize(aut.num_states);
	for (int s = 0; s < aut.num_states; ++s) {
		const auto& edges = aut.edges.size() > (size_t)s
		                  ? aut.edges[s] : std::vector<hoa_edge>{};
		for (auto& e : edges) {
			// LG-4: one edge_desc per cube of a (possibly disjunctive)
			// guard -- see build_edge_guard's doc comment.
			for (auto& guard : build_edge_guard(e.guard_label, in_ap_idx, out_ap_idx)) {
				edge_desc ed;
				ed.dst = e.dst;
				ed.guard = std::move(guard);
				d.edges[s].push_back(std::move(ed));
			}
		}
	}
	return d;
}

template <NodeType node>
std::optional<program_desc> build_program_desc(
    const ltl_aba_solution<node>& sol,
    const std::string& class_name,
    bool revisable,
    const std::vector<std::string>& open_streams,
    const io_context<node>* stream_ctx)
{
	using tau = tree<node>;
	using namespace codegen_detail;

	// Precondition: build_program_desc is handed a solved strategy, not a
	// formula, so it cannot run inference itself -- that is compile_spec's
	// job, at the string->tree step. Scan every atom's free io variables
	// up front and refuse immediately, naming the offending variable,
	// rather than let classify_output_field discover it mid-emission.
	for (auto& [atom_ref, prop] : sol.atoms)
		for (tref v : get_free_vars<node>(atom_ref))
			if (tau::get(v).get_ba_type() == 0)
				throw std::runtime_error(
					"build_program_desc: variable '"
					+ get_var_name<node>(v) + "' carries no BA "
					"type -- the formula did not go through type "
					"inference before reaching build_program_desc");

	std::set<std::string> input_set(sol.input_props.begin(), sol.input_props.end());
	std::map<std::string, atom_field_info> ameta;
	std::vector<atom_desc> atoms;
	for (auto& [atom_ref, prop] : sol.atoms) {
		validate_atom_io_types<node>(atom_ref);
		bool is_out = !input_set.count(prop);
		ameta[prop] = classify_atom_field<node>(atom_ref, is_out, revisable);
		if (atom_is_data_typed<node>(atom_ref, revisable))
			atoms.push_back({prop,
				build_atom_ground_expr<node>(atom_ref)});
	}

	// The emitted artifact needs a ground tref for every input guard atom
	// (table_step_provider's evaluate_atom) and every witness_template output
	// atom (emit_main's per-edge templates array looks each one up by prop) --
	// collect whichever the data gate above skipped. Carrier-typed atoms
	// included: build_atom_term_expr's is_one/is_zero fallback covers a
	// carrier whose owner spells no constants of its own, and a plain
	// variable reference is reconstructed generically regardless of type.
	{
		std::set<std::string> have;
		for (auto& a : atoms) have.insert(a.prop);
		for (auto& [atom_ref, prop] : sol.atoms) {
			if (have.count(prop)) continue;
			auto it = ameta.find(prop);
			bool is_template = it != ameta.end()
				&& it->second.kind == field_kind::witness_template;
			if (input_set.count(prop) || is_template) {
				atoms.push_back({prop, build_atom_ground_expr<node>(atom_ref)});
				have.insert(prop);
			}
		}
	}

	// Runtime-template variables, in first-appearance order; a variable
	// with any template atom is template-kind even when other atoms on it
	// could bake -- the per-edge joint solve then covers those too.
	std::vector<std::string> template_vars;
	std::set<std::string> template_var_set;
	for (auto& p : sol.output_props) {
		auto it = ameta.find(p);
		if (it == ameta.end()
			|| it->second.kind != field_kind::witness_template) continue;
		for (auto& [vn, ref] : it->second.template_vars)
			if (template_var_set.insert(vn).second)
				template_vars.push_back(vn);
	}

	// Baked witness variables, in first-appearance order over sol.output_props.
	std::vector<std::string> witness_vars;
	std::map<std::string, tref> var_io_ref;
	{
		std::set<std::string> seen;
		for (auto& p : sol.output_props) {
			auto it = ameta.find(p);
			if (it != ameta.end() && it->second.kind == field_kind::witness
			    && !seen.count(it->second.var_name)
			    && !template_var_set.count(it->second.var_name))
			{
				witness_vars.push_back(it->second.var_name);
				var_io_ref[it->second.var_name] = it->second.io_var_ref;
				seen.insert(it->second.var_name);
			}
		}
	}

	// HARD REFUSAL: a revisable (PWR) revision only carries flag data;
	// a witness field's value is a per-edge C++ expression baked into
	// source, which revise() cannot swap. Fail here, at build time, rather
	// than emit a program whose revise() would silently strand stale
	// witnesses.
	if (revisable && (!witness_vars.empty() || !template_vars.empty()))
		throw std::runtime_error(
			"PWR revision with data-atom outputs is not supported");

	program_desc d;
	d.class_name = class_name;
	d.num_states = sol.aut.num_states;
	d.initial_state = sol.aut.initial_state;
	d.revisable = revisable;
	d.open_streams = open_streams;
	d.atoms = std::move(atoms);
	// An atom template needs the same tau_pack/ba_constants surface a witness factory expression does.
	d.needs_tau_link = !witness_vars.empty() || !d.atoms.empty();
	std::tie(d.lookback, d.highest_initial_pos) =
		compute_auto_continue_bounds<node>(sol.atoms);
	// Atoms carrying a positional step were rewritten to current-time
	// equivalents by apply_step_counter_encoding, so the scan above finds
	// none -- highest_initial_pos comes from the pre-desugared step it
	// recorded instead.
	if (sol.counter_highest_initial_pos >= 0)
		d.highest_initial_pos = (int)sol.counter_highest_initial_pos;

	for (auto& p : sol.input_props)
		d.inputs.push_back({p, sanitize(p), field_kind::flag});
	// Flag fields first (their guard slot maps 1:1 to their AP), witness
	// fields after (no guard slot -- see edge_desc's own doc comment).
	// The step-counter's own bit props (see apply_step_counter_encoding) are
	// internal bookkeeping, never part of the emitted program's public
	// surface -- excluded the same way the interpreter excludes them from
	// its own output streams (interpreter.tmpl.h's is_excluded_output).
	std::map<std::string, int> prop_to_ap;
	std::map<std::string, tref> prop_to_atom;
	for (int i = 0; i < (int)sol.aut.aps.size(); ++i) prop_to_ap[sol.aut.aps[i]] = i;
	for (auto& [atom_ref, prop] : sol.atoms) prop_to_atom[prop] = atom_ref;

	for (auto& p : sol.output_props) {
		if (is_internal_ltl_output_prop(p)) continue;
		auto it = ameta.find(p);
		if (it == ameta.end() || it->second.kind == field_kind::flag) {
			d.outputs.push_back({p, sanitize(p), field_kind::flag});
			// The real variable this flag slot writes; empty when the atom
			// spans several distinct variable names (compile_spec then
			// refuses this desc -- no single variable to key the flag's
			// stream on). A self-lookback atom (o2[t] vs o2[t-1]) is two
			// free-var nodes naming the same variable, so it still counts
			// as single here, matching classify_atom_field.
			auto ai = prop_to_atom.find(p);
			auto single = ai != prop_to_atom.end()
				? atom_single_var_name<node>(ai->second) : std::nullopt;
			d.flag_output_vars.push_back(single.value_or(std::string{}));
		}
	}
	for (auto& v : witness_vars)
		d.outputs.push_back({v, sanitize(v), field_kind::witness});
	for (auto& v : template_vars)
		d.outputs.push_back({v, sanitize(v), field_kind::witness_template});

	// Real io streams, from the atoms' own variables; the counter's
	// o__ltl_ctr streams are internal bookkeeping.
	{
		// Name -> stream_id from the spec's own io_context (0 == console,
		// else a dict() id resolving to the bound filename), mirroring how
		// the interpreter itself resolves a stream's binding
		// (interpreter.tmpl.h's rebuild_inputs/rebuild_outputs).
		std::map<std::string, size_t> stream_id_of_name;
		if (stream_ctx) {
			for (auto& [hv, sid] : stream_ctx->inputs)
				stream_id_of_name[get_var_name<node>(hv->get())] = sid;
			for (auto& [hv, sid] : stream_ctx->outputs)
				stream_id_of_name[get_var_name<node>(hv->get())] = sid;
		}
		std::set<std::string> seen_streams;
		for (auto& [atom_ref, prop] : sol.atoms)
			for (tref v : get_free_vars<node>(atom_ref)) {
				const std::string& nm = get_var_name<node>(v);
				if (nm.size() > 10
					&& nm.compare(0, 10, "o__ltl_ctr") == 0) continue;
				if (!seen_streams.insert(nm).second) continue;
				tref io_node = tau::get(v).is(tau::io_var)
					? v : tau::get(v).child(0);
				auto& list = io_var_direction<node>(io_node) == 1
					? d.input_streams : d.output_streams;
				stream_desc sd;
				sd.name = nm;
				sd.ba_type = tau::get(v).get_ba_type();
				if (auto it = stream_id_of_name.find(nm);
					it != stream_id_of_name.end() && it->second != 0) {
					sd.bind = stream_desc::binding::file;
					sd.filename = dict(it->second);
				}
				list.push_back(std::move(sd));
			}
	}
	d.ba_type_table = snapshot_ba_type_registry<node>();

	std::vector<int> in_ap_idx, flag_out_ap_idx;
	for (auto& f : d.inputs) in_ap_idx.push_back(prop_to_ap.at(f.prop));
	for (auto& f : d.outputs) {
		if (f.kind != field_kind::flag) break;
		flag_out_ap_idx.push_back(prop_to_ap.at(f.prop));
	}

	d.edges.resize(sol.aut.num_states);
	for (int s = 0; s < sol.aut.num_states; ++s) {
		const auto& edges = sol.aut.edges.size() > (size_t)s
		                  ? sol.aut.edges[s] : std::vector<hoa_edge>{};
		for (auto& e : edges) {
			// A guard is a sum of products; each cube becomes its own
			// edge_desc so a disjunct is never dropped or mis-read.
			auto cubes = parse_guard_cubes(e.guard_label);
			if (!cubes) continue; // unparseable guard -- edge omitted
			for (auto& cube : *cubes) {
				edge_desc ed;
				ed.dst = e.dst;
				ed.guard = guard_from_cube(cube, in_ap_idx, flag_out_ap_idx);

				// A negative-signed literal enters the conjunction negated,
				// so the witness the BA picks still satisfies the cube.
				std::map<std::string, std::vector<std::pair<tref, bool>>>
					var_atoms;
				for (auto& [ap_idx, positive] : cube) {
					if (ap_idx < 0 || ap_idx >= (int)sol.aut.aps.size()) continue;
					const auto& prop = sol.aut.aps[ap_idx];
					auto it = ameta.find(prop);
					if (it == ameta.end()) continue;
					// Routed by prop for the runtime joint solve, which ignores
					// polarity, so a negative-signed one is dropped here.
					if (it->second.kind == field_kind::witness_template
						|| (it->second.kind == field_kind::witness
							&& template_var_set.count(it->second.var_name))) {
						if (!positive) continue;
						ed.witness_template_props.push_back(prop);
						ed.witness_template_is_counter.push_back(
							sol.counter_relativized_props.count(prop) > 0);
					}
					else if (it->second.kind == field_kind::witness) {
						tref atom = prop_to_atom.at(prop);
						var_atoms[it->second.var_name]
							.emplace_back(atom, positive);
					}
				}

				for (auto& [var, atom_list] : var_atoms) {
					// Not var_io_ref's first-occurrence io_var: a positional
					// variable's steps are distinct io_var nodes despite sharing var_name.
					tref io_ref = nullptr;
					for (tref v : get_free_vars<node>(atom_list[0].first))
						if (get_var_name<node>(v) == var) { io_ref = v; break; }
					if (!io_ref) io_ref = var_io_ref.at(var);
					size_t ba_type = tau::get(io_ref).get_ba_type();

					// A BA that rejects the full conjunction is retried with
					// negatives dropped; that fallback stays sound for the cube.
					tref full_conj = nullptr, pos_conj = nullptr;
					for (auto& [atom, positive] : atom_list) {
						tref signed_atom = positive
							? atom : tau::build_wff_neg(atom);
						full_conj = full_conj
							? tau::build_wff_and(full_conj, signed_atom)
							: signed_atom;
						if (positive)
							pos_conj = pos_conj
								? tau::build_wff_and(pos_conj, atom)
								: atom;
					}

					auto w = pack_codegen_witness<node>(
						ba_type, io_ref, full_conj);
					if (!w && pos_conj)
						w = pack_codegen_witness<node>(
							ba_type, io_ref, pos_conj);
					if (!w) {
						// No pos_conj means every literal was negative and
						// the owner declined; keep the output default.
						if (!pos_conj) continue;
						throw std::runtime_error(
							"output '" + var + "' is owned by a data BA "
							"that declined to supply a codegen witness "
							"for a feasible edge");
					}
					ed.witness_ctors.emplace_back(sanitize(var), *w);
				}

				d.edges[s].push_back(std::move(ed));
			}
		}
	}

	return d;
}

// Emit the declared-open appendix: open_streams()/register_open_oracle()/
// unregister_open_oracle()/admissible_values_mask(), the registration
// surface for oracle-resolved output streams. admissible_values_mask reads
// straight from edge_desc::guard's flag-output segment.
inline void emit_open_streams_appendix(
    const program_desc& d, size_t nflag, std::ostream& out)
{
	out << "\npublic:\n";
	out << "\tusing oracle_callback = const char* (*)(\n";
	out << "\t    const char* formula, void* user_data);\n\n";

	out << "\tstatic const char* const* open_streams() noexcept {\n";
	out << "\t\tstatic const char* const names[] = {\n";
	for (auto& s : d.open_streams) out << "\t\t\t\"" << s << "\",\n";
	out << "\t\t\tnullptr  // sentinel\n";
	out << "\t\t};\n\t\treturn names;\n\t}\n\n";

	out << "\tstatic constexpr std::size_t open_streams_count() noexcept {\n";
	out << "\t\treturn " << d.open_streams.size() << ";\n\t}\n\n";

	out << "\tint register_open_oracle(\n";
	out << "\t    const char* stream, oracle_callback cb, void* user_data) noexcept {\n";
	out << "\t\tif (in_oracle_dispatch_) return -2;\n";
	out << "\t\tbool found = false;\n";
	out << "\t\tfor (const char* const* p = open_streams(); *p; ++p)\n";
	out << "\t\t\tif (std::string(*p) == stream) { found = true; break; }\n";
	out << "\t\tif (!found) return -1;\n";
	out << "\t\thandlers_[std::string(stream)] = {cb, user_data};\n";
	out << "\t\treturn 0;\n\t}\n\n";

	out << "\tint unregister_open_oracle(const char* stream) noexcept {\n";
	out << "\t\tif (in_oracle_dispatch_) return -2;\n";
	out << "\t\tauto it = handlers_.find(std::string(stream));\n";
	out << "\t\tif (it == handlers_.end()) return -3;\n";
	out << "\t\thandlers_.erase(it);\n";
	out << "\t\treturn 0;\n\t}\n\n";

	out << "\t// Bitmask: bit0 = false admissible, bit1 = true admissible;\n";
	out << "\t// 0 = unreachable for this stream from q, 3 = both acceptable.\n";
	out << "\tstatic std::uint8_t admissible_values_mask(\n";
	out << "\t    int q, const char* stream) noexcept {\n";
	for (size_t k = 0; k < nflag; ++k) {
		// Accept both the bare prop and the "o_"-prefixed alias, so a
		// --open flag written either way resolves to the same field.
		out << "\t\tif (std::string(stream) == \"" << d.outputs[k].prop
		    << "\" || std::string(stream) == \"o_" << d.outputs[k].cpp_name
		    << "\") {\n";
		out << "\t\t\tswitch (q) {\n";
		for (int s = 0; s < d.num_states; ++s) {
			std::uint8_t mask = 0;
			const auto& edges = (size_t)s < d.edges.size()
			                   ? d.edges[s] : std::vector<edge_desc>{};
			for (auto& e : edges) {
				std::int8_t g = e.guard[d.inputs.size() + k];
				if (g == 0) mask |= 0x3;
				else if (g == 1) mask |= 0x2;
				else mask |= 0x1;
			}
			out << "\t\t\tcase " << s << ": return 0x"
			    << std::hex << (int)mask << std::dec << ";\n";
		}
		out << "\t\t\tdefault: return 0x0;\n\t\t\t}\n\t\t}\n";
	}
	out << "\t\treturn 0x0;  // unknown stream\n\t}\n\n";

	// ap[] uses inputs-then-flag-outputs order, matching edge_desc::guard.
	{
		std::map<std::string, size_t> stream_to_field;
		for (size_t k = 0; k < nflag; ++k)
			for (auto& s : d.open_streams)
				if (s == d.outputs[k].prop
					|| s == "o_" + d.outputs[k].cpp_name)
					stream_to_field[s] = k;
		std::set<size_t> declared_fields;
		for (auto& kv : stream_to_field) declared_fields.insert(kv.second);

		const size_t nbits = d.inputs.size() + nflag;
		out << "\toutputs step_with_oracle_dispatch(const inputs& in) noexcept {\n";
		out << "\t\toutputs o;\n";
		if (nbits) {
			out << "\t\tbool ap[" << nbits << "] = {};\n";
			for (size_t i = 0; i < d.inputs.size(); ++i)
				out << "\t\tap[" << i << "] = in." << d.inputs[i].cpp_name
				    << ";\n";
		} else out << "\t\t(void)in;\n";
		out << "\n";

		for (auto& s : d.open_streams) {
			auto it = stream_to_field.find(s);
			if (it == stream_to_field.end()) continue;
			size_t k = it->second, idx = d.inputs.size() + k;
			out << "\t\t{\n";
			out << "\t\t\tauto mask = admissible_values_mask(state_, \""
			    << s << "\");\n";
			out << "\t\t\tauto h = handlers_.find(\"" << s << "\");\n";
			out << "\t\t\tif (h == handlers_.end()) { o.ok = false; return o; }\n";
			out << "\t\t\tstd::string f;\n";
			out << "\t\t\tif (mask & 0x1) f += \"(" << d.outputs[k].cpp_name
			    << " = 0)\";\n";
			out << "\t\t\tif ((mask & 0x3) == 0x3) f += \" || \";\n";
			out << "\t\t\tif (mask & 0x2) f += \"(" << d.outputs[k].cpp_name
			    << " = 1)\";\n";
			out << "\t\t\tin_oracle_dispatch_ = true;\n";
			out << "\t\t\tconst char* response = h->second.first(f.c_str(),\n";
			out << "\t\t\t    h->second.second);\n";
			out << "\t\t\tin_oracle_dispatch_ = false;\n";
			out << "\t\t\tif (!response) { o.ok = false; return o; }\n";
			out << "\t\t\tbool chose_true = std::strstr(response, \":= 1\") "
			       "!= nullptr;\n";
			out << "\t\t\tstd::uint8_t needed = chose_true ? 0x2 : 0x1;\n";
			out << "\t\t\tif ((mask & needed) == 0) { o.ok = false; return o; }\n";
			out << "\t\t\tap[" << idx << "] = chose_true;\n";
			out << "\t\t\to." << d.outputs[k].cpp_name << " = chose_true;\n";
			out << "\t\t}\n";
		}

		out << "\n\t\tswitch (state_) {\n";
		for (int s = 0; s < d.num_states; ++s) {
			out << "\t\tcase " << s << ": {\n";
			const auto& edges = (size_t)s < d.edges.size()
			                   ? d.edges[s] : std::vector<edge_desc>{};
			size_t edge_idx = 0;
			for (auto& e : edges) {
				std::string cond;
				for (size_t i = 0; i < d.inputs.size(); ++i) {
					if (e.guard[i] == 0) continue;
					if (!cond.empty()) cond += " && ";
					if (e.guard[i] == -1) cond += "!";
					cond += "ap[" + std::to_string(i) + "]";
				}
				for (size_t k = 0; k < nflag; ++k) {
					if (!declared_fields.count(k)) continue;
					std::int8_t g = e.guard[d.inputs.size() + k];
					if (g == 0) continue;
					if (!cond.empty()) cond += " && ";
					if (g == -1) cond += "!";
					cond += "ap[" + std::to_string(d.inputs.size() + k) + "]";
				}
				if (cond.empty()) cond = "true";
				out << "\t\t\tif (" << cond << ") {\n";
				for (size_t k = 0; k < nflag; ++k) {
					if (declared_fields.count(k)) continue;
					std::int8_t g = e.guard[d.inputs.size() + k];
					if (g == 0) continue;
					out << "\t\t\t\to." << d.outputs[k].cpp_name << " = "
					    << (g == 1 ? "true" : "false") << ";\n";
				}
				for (auto& [cpp_name, expr] : e.witness_ctors) {
					std::string sv = "wd_s" + std::to_string(s) + "_e"
						+ std::to_string(edge_idx) + "_" + cpp_name;
					out << "\t\t\t\tstatic const tref " << sv << " = "
					    << expr << ";\n";
					out << "\t\t\t\to." << cpp_name << " = " << sv << ";\n";
				}
				out << "\t\t\t\tstate_ = " << e.dst << ";\n";
				out << "\t\t\t\treturn o;\n";
				out << "\t\t\t}\n";
				++edge_idx;
			}
			out << "\t\t\to.ok = false; return o;\n";
			out << "\t\t}\n";
		}
		out << "\t\t}\n";
		out << "\t\to.ok = false; return o;\n";
		out << "\t}\n\n";
	}

	out << "private:\n";
	out << "\tstd::map<std::string, std::pair<oracle_callback, void*>> handlers_;\n";
	out << "\tbool in_oracle_dispatch_ = false;\n";
}

// Emit program_desc::atoms as a lookup table of (prop, lazily-built tref) entries, for a future runtime consumer (not read by step() itself).
inline void emit_atoms_appendix(const program_desc& d, std::ostream& out) {
	out << "\tstruct atom_entry { const char* prop; tref (*value)(); };\n";
	out << "\tstatic const atom_entry* atoms() noexcept {\n";
	out << "\t\tstatic const atom_entry table[] = {\n";
	for (auto& a : d.atoms) {
		out << "\t\t\t{\"" << a.prop << "\", []() -> tref {\n";
		out << "\t\t\t\tstatic const tref v = " << a.ground_expr << ";\n";
		out << "\t\t\t\treturn v;\n";
		out << "\t\t\t}},\n";
	}
	out << "\t\t\t{nullptr, nullptr}\n";
	out << "\t\t};\n\t\treturn table;\n\t}\n\n";
	out << "\tstatic constexpr std::size_t atoms_count() noexcept {\n";
	out << "\t\treturn " << d.atoms.size() << ";\n\t}\n\n";
}

inline void emit_program(const program_desc& d, std::ostream& out)
{
	using namespace codegen_detail;

	const size_t nflag = num_flag_outputs(d);
	const bool has_witness = nflag != d.outputs.size();

	// A witness-template output's value is solved at runtime from the atom
	// templates; this standalone step() has no solver, so such programs run
	// through the interpreter's table_step_provider instead.
	for (auto& f : d.outputs)
		if (f.kind == field_kind::witness_template)
			throw std::runtime_error("output '" + f.prop
				+ "' needs runtime witness solving, which the "
				"standalone emitted step() does not support; drive the "
				"program through the interpreter's table step provider");

	out << "// Auto-generated by tau-lang's C++ program emitter.\n";
	out << "// Do not edit by hand — regenerate from the source .tau spec.\n\n";
	out << "#pragma once\n";
	out << "#include <cstdint>\n";
	if (!d.needs_tau_link || !d.atoms.empty()) out << "#include <cstddef>\n";
	if (!d.needs_tau_link) out << "#include <vector>\n";
	if (!has_witness) out << "#include <cassert>\n#include <utility>\n#include <string>\n";
	if (!d.open_streams.empty())
		out << "#include <map>\n#include <string>\n#include <cstring>\n";
	if (d.needs_tau_link) {
		// tref + ba_constants<node_t> + ba_descriptor<...> for the witness factory expressions baked into step().
		out << "#include \"tau_pack.h\"\n";
		// node::ba::* used by a BA descriptor, only forward-declared by tau_pack.h's tau_tree.h.
		out << "#include \"base_ba_dispatcher.h\"\n";
		// operator|/&/^ on the constant variant, used building a witness's own type_tree().
		out << "#include \"boolean_algebras/variant_ba.h\"\n";
		out << "#include \"ba_constants.h\"\n";
		out << "using tref = ::idni::tref;\n";
	}
	out << "\n";

	// The strategy shape (codegen::edge/strategy/strategy_step) is only
	// needed for the flag-only (table-driven-capable) step(); a
	// witness-bearing program never builds a strategy at all (see below).
	if (!has_witness) {
		if (d.needs_tau_link) {
			out << "#include \"codegen_strategy.h\"\n";
			out << "namespace tau_codegen_detail = ::idni::tau_lang::codegen;\n\n";
		} else {
			// Inlined so the artifact has no path dependency on this tree
			// at compile time -- see codegen_strategy.h, whose shape this
			// mirrors exactly.
			out << "namespace tau_codegen_detail {\n";
			out << "struct edge { std::vector<std::int8_t> guard; int dst = 0; };\n";
			out << "struct strategy {\n";
			out << "\tint num_states = 0;\n";
			out << "\tint initial_state = 0;\n";
			out << "\tint num_inputs = 0;\n";
			out << "\tstd::vector<std::vector<edge>> edges;\n";
			out << "\tstd::vector<std::string> aps;\n";
			out << "};\n";
			out << "inline const edge* strategy_step(\n";
			out << "    const strategy& s, int src, const bool* ap) {\n";
			out << "\tif (src < 0 || static_cast<std::size_t>(src) >= s.edges.size())\n";
			out << "\t\treturn nullptr;\n";
			out << "\tfor (const auto& e : s.edges[src]) {\n";
			out << "\t\tbool match = true;\n";
			out << "\t\tconst int n = s.num_inputs < (int)e.guard.size()\n";
			out << "\t\t            ? s.num_inputs : (int)e.guard.size();\n";
			out << "\t\tfor (int i = 0; i < n; ++i) {\n";
			out << "\t\t\tif (e.guard[i] == 1 && !ap[i]) { match = false; break; }\n";
			out << "\t\t\tif (e.guard[i] == -1 && ap[i]) { match = false; break; }\n";
			out << "\t\t}\n";
			out << "\t\tif (match) return &e;\n";
			out << "\t}\n";
			out << "\treturn nullptr;\n";
			out << "}\n";
			out << "} // namespace tau_codegen_detail\n\n";
		}
	}

	out << "class " << d.class_name << " {\n";
	out << "public:\n";

	out << "\tstruct inputs {\n";
	for (auto& f : d.inputs)
		out << "\t\tbool " << f.cpp_name << " = false;\n";
	out << "\t};\n\n";

	out << "\tstruct outputs {\n";
	for (auto& f : d.outputs) {
		if (f.kind == field_kind::flag)
			out << "\t\tbool " << f.cpp_name << " = false;\n";
		else
			out << "\t\ttref " << f.cpp_name << " = nullptr;"
			       "  // witness computed at synthesis time\n";
	}
	out << "\t\tbool ok = true;  // false: no edge matched (synthesis invariant violated)\n";
	out << "\t};\n\n";

	// Driver loops read these to decide whether to keep stepping.
	out << "\tstatic constexpr int lookback = " << d.lookback << ";\n";
	out << "\tstatic constexpr int highest_initial_pos = "
	    << d.highest_initial_pos << ";\n\n";

	if (!d.atoms.empty()) emit_atoms_appendix(d, out);

	if (!has_witness) {
		// ── Flag-only: one generic table-driven step(), shared by the
		// propositional and PWR-capable shapes (revisable only changes
		// whether revise()/revision_count()/strategy() are exposed).
		out << "\t" << d.class_name << "() { load_initial_strategy(); }\n\n";
		out << "\tint state() const noexcept { return state_; }\n\n";

		// Order (inputs, then flag outputs) must match codegen_strategy.h's
		// edge::guard -- revise() checks a strategy's aps against it.
		out << "\tstatic const std::vector<std::string>& program_aps() noexcept {\n";
		out << "\t\tstatic const std::vector<std::string> v = {\n";
		for (auto& f : d.inputs)
			out << "\t\t\t\"" << f.prop << "\",\n";
		for (size_t k = 0; k < nflag; ++k)
			out << "\t\t\t\"" << d.outputs[k].prop << "\",\n";
		out << "\t\t};\n";
		out << "\t\treturn v;\n";
		out << "\t}\n\n";

		out << "\toutputs step(const inputs& in) noexcept {\n";
		out << "\t\toutputs o;\n";
		if (!d.inputs.empty()) {
			out << "\t\tbool ap[" << d.inputs.size() << "] = {};\n";
			for (size_t k = 0; k < d.inputs.size(); ++k)
				out << "\t\tap[" << k << "] = in." << d.inputs[k].cpp_name << ";\n";
		} else {
			out << "\t\t(void)in;\n";
		}
		out << "\t\tconst auto* e = tau_codegen_detail::strategy_step(\n";
		out << "\t\t    strat_, state_, " << (d.inputs.empty() ? "nullptr" : "ap") << ");\n";
		out << "\t\tif (!e) { o.ok = false; return o; }\n";
		for (size_t k = 0; k < nflag; ++k) {
			out << "\t\tif (e->guard[" << d.inputs.size() + k << "] == 1) o."
			    << d.outputs[k].cpp_name << " = true;\n";
			out << "\t\telse if (e->guard[" << d.inputs.size() + k << "] == -1) o."
			    << d.outputs[k].cpp_name << " = false;\n";
		}
		out << "\t\tstate_ = e->dst;\n";
		out << "\t\treturn o;\n";
		out << "\t}\n\n";

		if (d.revisable) {
			out << "\t// Swap the strategy at runtime (PWR). CG-N5: validation\n";
			out << "\t// is real code, not assert-only -- a malformed strategy\n";
			out << "\t// is REFUSED (return false, running strategy and state\n";
			out << "\t// untouched) even under -DNDEBUG, where the asserts\n";
			out << "\t// compile out. On success the state machine resets to\n";
			out << "\t// the new initial state -- matching the interpreter's\n";
			out << "\t// behaviour where a revised spec restarts the unbound\n";
			out << "\t// continuation from the current time point.\n";
			// Public alias for the strategy type revise()/strategy() trade in,
			// so callers do not need to spell tau_codegen_detail::strategy.
			out << "\tusing strategy_type = tau_codegen_detail::strategy;\n";
			out << "\tbool revise(tau_codegen_detail::strategy new_strat) noexcept {\n";
			out << "\t\tbool valid = new_strat.num_states > 0\n";
			out << "\t\t\t&& new_strat.initial_state >= 0\n";
			out << "\t\t\t&& new_strat.initial_state < new_strat.num_states\n";
			out << "\t\t\t&& new_strat.num_inputs == " << d.inputs.size() << "\n";
			out << "\t\t\t&& (int)new_strat.edges.size() == new_strat.num_states\n";
			out << "\t\t\t// Empty aps means \"unset\"; only a non-empty, mismatching\n";
			out << "\t\t\t// list is refused.\n";
			out << "\t\t\t&& (new_strat.aps.empty() || new_strat.aps == program_aps());\n";
			out << "\t\tif (valid)\n";
			out << "\t\t\tfor (const auto& sv : new_strat.edges)\n";
			out << "\t\t\t\tfor (const auto& e : sv)\n";
			out << "\t\t\t\t\tvalid = valid\n";
			out << "\t\t\t\t\t\t&& (int)e.guard.size() == "
			    << d.inputs.size() + nflag << "\n";
			out << "\t\t\t\t\t\t&& e.dst >= 0\n";
			out << "\t\t\t\t\t\t&& e.dst < new_strat.num_states;\n";
			out << "\t\tassert(valid && \"revise(): malformed strategy refused\");\n";
			out << "\t\tif (!valid) return false;\n";
			out << "\t\tstrat_ = std::move(new_strat);\n";
			out << "\t\tstate_ = strat_.initial_state;\n";
			out << "\t\t++revision_count_;\n";
			out << "\t\treturn true;\n";
			out << "\t}\n\n";
			out << "\tint revision_count() const noexcept { return revision_count_; }\n";
			out << "\tconst tau_codegen_detail::strategy& strategy() const noexcept {\n";
			out << "\t\treturn strat_;\n\t}\n\n";
		}

		if (!d.open_streams.empty()) emit_open_streams_appendix(d, nflag, out);

		out << "\nprivate:\n";
		out << "\tint state_ = " << d.initial_state << ";\n";
		if (d.revisable) out << "\tint revision_count_ = 0;\n";
		out << "\ttau_codegen_detail::strategy strat_;\n\n";
		out << "\tvoid load_initial_strategy() {\n";
		out << "\t\tstrat_.num_states = " << d.num_states << ";\n";
		out << "\t\tstrat_.initial_state = " << d.initial_state << ";\n";
		out << "\t\tstrat_.num_inputs = " << d.inputs.size() << ";\n";
		out << "\t\tstrat_.aps = program_aps();\n";
		out << "\t\tstrat_.edges.resize(" << d.num_states << ");\n";
		for (int s = 0; s < d.num_states; ++s) {
			const auto& edges = (size_t)s < d.edges.size()
			                   ? d.edges[s] : std::vector<edge_desc>{};
			for (auto& e : edges) {
				out << "\t\tstrat_.edges[" << s << "].push_back({{";
				for (size_t k = 0; k < e.guard.size(); ++k) {
					if (k) out << ",";
					out << (int)e.guard[k];
				}
				out << "}, " << e.dst << "});\n";
			}
		}
		out << "\t\tstate_ = strat_.initial_state;\n";
		out << "\t}\n";
	} else {
		// ── Witness-bearing: a per-edge unrolled step() (revise()/
		// table-driven is refused for this shape at build_program_desc()).
		out << "\t" << d.class_name << "() = default;\n\n";
		out << "\tint state() const noexcept { return state_; }\n\n";

		out << "\toutputs step(const inputs& in) noexcept {\n";
		out << "\t\toutputs o;\n";
		if (d.inputs.empty()) out << "\t\t(void)in;\n";
		out << "\n\t\tswitch (state_) {\n";
		for (int s = 0; s < d.num_states; ++s) {
			out << "\t\tcase " << s << ": {\n";
			const auto& edges = (size_t)s < d.edges.size()
			                   ? d.edges[s] : std::vector<edge_desc>{};
			bool first = true;
			size_t edge_idx = 0;
			for (auto& e : edges) {
				std::ostringstream ig;
				bool any = false;
				for (size_t k = 0; k < d.inputs.size(); ++k) {
					if (e.guard[k] == 0) continue;
					if (any) ig << " && ";
					if (e.guard[k] == -1) ig << "!";
					ig << "in." << d.inputs[k].cpp_name;
					any = true;
				}
				out << "\t\t\t" << (first ? "if (" : "else if (")
				    << (any ? ig.str() : "true") << ") {\n";
				for (size_t k = 0; k < nflag; ++k) {
					std::int8_t g = e.guard[d.inputs.size() + k];
					if (g == 0) continue;
					out << "\t\t\t\to." << d.outputs[k].cpp_name << " = "
					    << (g == 1 ? "true" : "false") << ";\n";
				}
				// Hoisted to a function-local static: the factory expression
				// runs once (thread-safe, order-independent init) rather than
				// re-registering the same constant on every step().
				for (auto& [cpp_name, expr] : e.witness_ctors) {
					std::string sv = "w_s" + std::to_string(s) + "_e"
						+ std::to_string(edge_idx) + "_" + cpp_name;
					out << "\t\t\t\tstatic const tref " << sv << " = "
					    << expr << ";\n";
					out << "\t\t\t\to." << cpp_name << " = " << sv << ";\n";
				}
				out << "\t\t\t\tstate_ = " << e.dst << ";\n";
				out << "\t\t\t\treturn o;\n";
				out << "\t\t\t}\n";
				first = false;
				++edge_idx;
			}
			out << "\t\t\to.ok = false; return o;\n";
			out << "\t\t}\n";
		}
		out << "\t\t}\n";
		out << "\t\to.ok = false; return o;  // unreachable\n";
		out << "\t}\n\n";

		if (!d.open_streams.empty()) emit_open_streams_appendix(d, nflag, out);

		out << "\nprivate:\n";
		out << "\tint state_ = " << d.initial_state << ";\n";
	}

	out << "};\n";
}


} // namespace idni::tau_lang

#endif // __IDNI__TAU__CPP_CODEGEN_TMPL_H__
